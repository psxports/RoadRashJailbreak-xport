#include "rrj_debug.h"
#include "rrj_data.h"
#include "race_trace_runtime.h"
#include "rrj_cd_transport.h"
#include "psx.h"
#include "psx_spu.h"
#include "psx_gpu.h"
#include "xport_trace.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct RRJTraceRuntime
{
    FILE *phases;
    FILE *input_calls;
    FILE *actors;
    FILE *world;
    FILE *inputs;
    FILE *disc;
    uint32_t phase_base;
    uint32_t phase_count;
    uint32_t phase_limit;
    uint32_t input_ordinal;
    uint32_t input_end;
    uint32_t game_time;
    uint32_t aux_time;
    uint32_t last_pc;
    uint32_t dispatch_target;
    uint32_t last_tick;
    uint32_t pending_input[3];
    uint32_t pending_store_slots;
    uint32_t cd_pending;
    uint32_t cd_resume_ready;
    int has_pending_input;
    int failed;
    int live;
    RRJLivePoll live_poll;
    char progress_path[1024];
} RRJTraceRuntime;

static RRJTraceRuntime trace;
static uint32 live_limit_override;

void rrj_trace_runtime_set_live_poll(RRJLivePoll poll)
{
    trace.live_poll = poll;
}

void rrj_trace_set_live_limit(uint32 limit)
{
    live_limit_override = limit;
}

static uint8_t trace_bios[524288];

static int trace_load_bios(RRJMemory *m)
{
    const char *path = RRJ_DEBUG_GETENV("RRJ_BIOS_IMAGE");
    FILE *stream = xport_fopen(path && *path ? path : "../tools/duckstation/data/user/bios/scph5502.bin", "rb");
    int loaded;
    uint8_t trailing;

    if (!stream)
        return 0;
    loaded = fread(trace_bios, sizeof(trace_bios), 1, stream) == 1 && fread(&trailing, 1, 1, stream) == 0;
    if (fclose(stream))
        loaded = 0;
    if (loaded)
    {
        m->bios = trace_bios;
        rrj_memory_bind(m);
    }
    return loaded;
}

#ifndef NDEBUG
static int trace_load_context(uint32_t *input_ordinal)
{
    const char *checkpoint = RRJ_DEBUG_GETENV("RRJ_PHASE_CHECKPOINT_LOAD");
    uint32_t context[12];

    if (!checkpoint)
        return 0;
    if (!xport_trace_read_context(checkpoint, 0x32504346u, 0x80012370u, trace.phase_base, context))
        return 0;
    *input_ordinal = context[3];
    return 1;
}

static int trace_progress(uint32_t pc, uint32_t tick)
{
    uint32_t value[3] = {trace.phase_base + trace.phase_count, tick, pc};

    return xport_trace_write_progress(trace.progress_path, trace.phase_count, value, 3u);
}

#endif

static int trace_phase_gpu(uint32_t pc, uint32_t tick)
{
    uint32_t record[5] = {17, tick, 8, pc, 0};

    return fwrite(record, sizeof(record), 1, trace.phases) == 1;
}

static int trace_cd_transfer(void)
{
    const char *path = RRJ_DEBUG_GETENV("RRJ_DISC_IMAGE");
    uint8_t buffer[2048];
    uint32_t sector = r_u32(0x800D6720u);
    uint32_t destination = r_u32(0x800D6714u);
    uint32_t count = r_u32(0x800D6718u);
    uint32_t index;

    if (!trace.disc)
        trace.disc = xport_fopen(path && *path ? path : "../iso/Road Rash - Jailbreak (USA).bin", "rb");
    if (!trace.disc || sector < 150u || count > 1024u)
        return 0;
    for (index = 0; index < count; ++index)
    {
        int64_t offset = ((int64_t)sector - 150 + index) * 2352 + 24;

        if (offset > LONG_MAX || fseek(trace.disc, (long)offset, SEEK_SET) || fread(buffer, 2048, 1, trace.disc) != 1 || xport_guest_copy(xport_guest_ref(destination + 2048u * index), xport_host_ref(buffer), sizeof(buffer)))
            return 0;
    }
    return 1;
}


/* Read the project's Mode2 user data into a synchronous native destination */
int rrj_live_cd_read(uint32_t sector, void *destination, uint32_t length)
{
    uint8_t bytes[2048];
    uint8_t *output = destination;
    const char *path = RRJ_DEBUG_GETENV("RRJ_DISC_IMAGE");
    if (!trace.live || sector < 150 || !destination || length > 0x200000)
        return 0;
    if (!trace.disc)
        trace.disc = xport_fopen(path && *path ? path : "../iso/Road Rash - Jailbreak (USA).bin", "rb");
    if (!trace.disc)
        return 0;
    while (length)
    {
        int64_t offset = ((int64_t)sector - 150) * 2352 + 24;
        uint32_t chunk = length < sizeof(bytes) ? length : sizeof(bytes);
        if (offset > LONG_MAX || fseek(trace.disc, (long)offset, SEEK_SET) || fread(bytes, sizeof(bytes), 1, trace.disc) != 1)
            return 0;
        memcpy(output, bytes, chunk);
        output += chunk;
        length -= chunk;
        ++sector;
    }
    return 1;
}

/* Resume the saved read-ready sector before publishing its completion */
static int trace_cd_resume_sector(void)
{
    const char *path = RRJ_DEBUG_GETENV("RRJ_DISC_IMAGE");
    uint8_t buffer[2048];
    uint32_t sector = r_u32(0x800D6720u);
    uint32_t destination = r_u32(0x800D6714u);
    uint32_t count = r_u32(0x800D6718u);
    uint32_t remaining = r_u32(0x800D671Cu);
    uint32_t length = remaining < sizeof(buffer) ? remaining : sizeof(buffer);
    int64_t offset = ((int64_t)sector - 150) * 2352 + 24;

    if (sector < 150u || !remaining || count != (remaining + 2047u) / 2048u || remaining > 0x200000u || (destination & 0x1FFFFFFFu) >= 0x200000u || length > 0x200000u - (destination & 0x1FFFFFFFu) || offset > LONG_MAX)
        return 0;
    if (!trace.disc)
        trace.disc = xport_fopen(path && *path ? path : "../iso/Road Rash - Jailbreak (USA).bin", "rb");
    if (!trace.disc || fseek(trace.disc, (long)offset, SEEK_SET) || fread(buffer, sizeof(buffer), 1, trace.disc) != 1 || !xport_guest_copy(xport_guest_ref(destination), xport_host_ref(buffer), length))
        return 0;
    w_u32(0x800D6724u, sector + 1u);
    w_u32(0x800D6714u, destination + length);
    w_u32(0x800D671Cu, remaining - length);
    w_u32(0x800D6718u, count - 1u);
    w_u32(0x800D6720u, sector + 1u);
    if (count == 1u)
        w_u32(0x800D672Cu, sector + 1u);
    return 1;
}

static int trace_game_boundary(uint32_t tick)
{
    uint32_t count = r_u32(0x800D5D68);
    uint32_t zero = 0;
    uint32_t input[3];

    if (count > 4)
        return 0;
    if (fwrite(&tick, 4, 1, trace.actors) != 1 || fwrite(xport_guest_ptr(0x800D6DE0, 768), 768, 1, trace.actors) != 1 || fwrite(xport_guest_ptr(0x800D5D38, 256), 256, 1, trace.actors) != 1)
        return 0;
    if (fwrite(&tick, 4, 1, trace.world) != 1 || fwrite(&count, 4, 1, trace.world) != 1 || fwrite(xport_guest_ptr(0x800D5D38, 256), 256, 1, trace.world) != 1 || fwrite(xport_guest_ptr(0x800D6DE0, 768), 768, 1, trace.world) != 1 || fwrite(xport_guest_ptr(0x800D6170, 64 * count), 64, count, trace.world) != count || fwrite(xport_guest_ptr(0x8005B580, 64), 64, 1, trace.world) != 1 || fwrite(xport_guest_ptr(0x800D5D6C, 4), 4, 1, trace.world) != 1 || fwrite(xport_guest_ptr(0x8005B46C, 4), 4, 1, trace.world) != 1 || fwrite(&zero, 4, 1, trace.world) != 1)
        return 0;
    input[0] = tick;
    input[1] = (~r_u16(0x800D70E2)) & 0xFFFF;
    input[2] = input[1];
    if (trace.has_pending_input && fwrite(trace.pending_input, sizeof(trace.pending_input), 1, trace.inputs) != 1)
        return 0;
    memcpy(trace.pending_input, input, sizeof(input));
    trace.has_pending_input = 1;
    return 1;
}

int rrj_trace_runtime_init(RRJMemory *m, uint32_t phase_base)
{
#ifndef NDEBUG
    const char *phase_path = RRJ_DEBUG_GETENV("RRJ_TRACE_PHASE_PATH");
    const char *calls_path = RRJ_DEBUG_GETENV("RRJ_TRACE_INPUT_CALLS");
    const char *output = RRJ_DEBUG_GETENV("RRJ_AUDIT_OUTPUT");
    const char *progress = RRJ_DEBUG_GETENV("RRJ_PROGRESS_PATH");
    uint32_t header[3] = {0x31504646, 3, 1312};
    uint32_t magic;

    memset(&trace, 0, sizeof(trace));
    trace.phase_base = phase_base;
    if (!phase_path || !calls_path || !output || !xport_trace_env_u32("RRJ_MENU_PHASE_COUNT", 1000000u, &trace.phase_limit) || !trace.phase_limit || !xport_trace_env_u32("RRJ_TRACE_INPUT_CALLS_END", 1000000u, &trace.input_end) || !trace_load_context(&trace.input_ordinal) || !trace_load_bios(m))
        return 0;
    trace.phases = xport_fopen(phase_path, "wb");
    trace.input_calls = xport_fopen(calls_path, "rb");
    if (!trace.phases || !trace.input_calls || fread(&magic, sizeof(magic), 1, trace.input_calls) != 1 || magic != 0x31494646 || fseek(trace.input_calls, 4 + 44 * (long)trace.input_ordinal, SEEK_SET) || fwrite(header, sizeof(header), 1, trace.phases) != 1 || !xport_trace_open_suffix(&trace.actors, output, ".actors", "wb") || !xport_trace_open_suffix(&trace.world, output, ".world", "wb") || !xport_trace_open_suffix(&trace.inputs, output, ".inputs", "wb"))
        return 0;
    if (progress)
    {
        if (strlen(progress) >= sizeof(trace.progress_path))
            return 0;
        strcpy(trace.progress_path, progress);
    }
    {
        uint32_t display = r_u32(0x8005B470);
        uint32_t slots = r_u8(display + 244);
        uint32_t slot;

        if (slots > 16)
            return 0;
        for (slot = 0; slot < slots; ++slot)
            if (r_u32(0x800D75D0 + 64 * slot))
                trace.pending_store_slots |= 1u << slot;
    }
    return 1;
#else
    return 0;
#endif
}

int rrj_trace_runtime_init_live(RRJMemory *m)
{
    const char *limit = RRJ_DEBUG_GETENV("RRJ_LIVE_FRAME_LIMIT");
    const GpuPsyqStateBinding gpu_state = {
        0x80055FD8, 0x80055F7C, 0x80055F6C, 0x80055F6F, 0x80055F0C, 0
    };

    /* Bind the original SDK caches used by display and drawing environment readback */
    if (!gpu_bind_psyq_state(&gpu_state))
    {
        RRJ_DEBUG_FPUTS("Native GPU SDK state binding failed\n", stderr);
        return 0;
    }

    memset(&trace, 0, sizeof(trace));
    trace.live = 1;
    {
        uint32_t state = r_u32(0x800D6728u);

        if (state != 0u && state != 1u && state != 2u && state != 5u)
        {
            RRJ_DEBUG_FPRINTF(stderr, "Native CD handoff unsupported state %u\n", state);
            return 0;
        }
        trace.cd_pending = state != 0u;
        trace.cd_resume_ready = state != 0u;
    }
    if (live_limit_override)
        trace.phase_limit = live_limit_override;
    else if (limit && *limit)
        trace.phase_limit = (uint32_t)strtoul(limit, NULL, 10);
    if (!trace_load_bios(m))
        return 0;
#if !defined(LOCKSTEP_DEBUG)
    return rrj_data_init();
#else
    {
        const char *cue = RRJ_DEBUG_GETENV("RRJ_DISC_CUE");
        const char *path = cue && *cue ? cue : "../iso/Road Rash - Jailbreak (USA).cue";

        if (!cd_mount_cue(path))
        {
            RRJ_DEBUG_FPRINTF(stderr, "Native CD image mount failed: %s\n", path);
            return 0;
        }
        if (!CdInit())
        {
            RRJ_DEBUG_FPUTS("Native CD initialization failed\n", stderr);
            cd_unmount_image();
            return 0;
        }
    }
    if (!rrj_cd_stream_restore())
    {
        RRJ_DEBUG_FPUTS("Native saved video CD transport restoration failed\n", stderr);
        return 0;
    }
    return 1;
#endif
}

int rrj_trace_runtime_is_live(void)
{
    return trace.live;
}

uint32_t rrj_trace_vblank_boundary(RRJMemory *m)
{
    uint32_t cd_state;
    uint32_t slot;

    if (!trace.input_calls && !trace.live)
        return 0;
    if (trace.pending_store_slots)
    {
        for (slot = 0; slot < 16; ++slot)
            if (trace.pending_store_slots & (1u << slot))
                w_u32(0x800D75D0 + 64 * slot, 0);
        trace.pending_store_slots = 0;
    }
    if (trace.live && trace.live_poll)
        trace.live_poll();
    if (rrj_data_enabled())
        return 0;
    if (trace.live)
        rrj_cd_stream_service();
    cd_state = r_u32(0x800D6728u);
    if (trace.live && cd_state != 0u && cd_state != 1u && cd_state != 2u && cd_state != 5u)
    {
        RRJ_DEBUG_FPRINTF(stderr, "Native CD service unsupported state %u\n", cd_state);
        abort();
    }
    if (trace.live && cd_state == 5u && !trace.cd_pending)
    {
        trace.cd_pending = 1;
        trace.cd_resume_ready = 1;
    }
    if (cd_state == 1u)
    {
        trace.cd_pending = 1;
        trace.cd_resume_ready = trace.live != 0;
        w_u32(0x800D6728u, 2u);
    }
    else if (trace.cd_pending && cd_state == 2u)
    {
        if (!trace.live && !trace_cd_transfer())
        {
            RRJ_DEBUG_FPRINTF(stderr, "Diagnostic CD transfer failed at sector %u\n", r_u32(0x800D6720u));
            abort();
        }
        w_u32(0x800D6728u, 5u);
        w_u32(0x800D6724u, r_u32(0x800D6720u));
        w_u32(0x8005AF60u, 0);
    }
    else if (trace.cd_pending && cd_state == 5u)
    {
        if (trace.cd_resume_ready)
        {
            if (!trace_cd_resume_sector())
            {
                RRJ_DEBUG_FPRINTF(stderr, "Native CD handoff read failed at sector %u\n", r_u32(0x800D6720u));
                abort();
            }
            if (r_u32(0x800D6718u))
                return 0;
            trace.cd_resume_ready = 0;
        }
        trace.cd_pending = 0;
        w_u32(0x800D6728u, 0);
        return r_u32(0x800D6710u);
    }
    return 0;
}

int rrj_trace_phase_boundary(RRJMemory *m, uint32_t pc)
{
#if defined(LOCKSTEP_DEBUG)
    if (xport_run_mode() == RUN_MODE_LOCKSTEP_REPLAY)
        return 0;
#endif
#ifndef NDEBUG
    uint32_t tick = r_u32(0x800D5D48);
    uint32_t header[3] = {15, tick, 1312};
    uint32_t context[8] = {pc, (uint32_t)VSync(-1), 0, 0, r_u32(0x8005B2F8), r_u32(0x8005B2F8), r_u32(0x800D70E0), r_u32(0x800D7104)};

#endif

    if (trace.live)
    {
        if (pc != 0x80012370)
            return trace.failed;
        if (trace.phase_count && !gpu_present())
        {
            trace.failed = 1;
            return 1;
        }
        if (trace.phase_count)
            VSync(0);
        if (trace.phase_limit && trace.phase_count >= trace.phase_limit)
            return 1;
        gpu_begin();
        ++trace.phase_count;
        return 0;
    }
#ifndef NDEBUG
    if (trace.failed || trace.phase_count >= trace.phase_limit)
        return 1;
    if (fwrite(header, sizeof(header), 1, trace.phases) != 1 || fwrite(context, sizeof(context), 1, trace.phases) != 1 || fwrite(xport_guest_ptr(0x800D5D38, 256), 256, 1, trace.phases) != 1 || fwrite(xport_guest_ptr(0x800D6DE0, 768), 768, 1, trace.phases) != 1 || fwrite(xport_guest_ptr(0x800D5D38, 256), 256, 1, trace.phases) != 1 || !trace_phase_gpu(pc, tick) || (pc == 0x80012370 && !trace_game_boundary(tick)))
    {
        trace.failed = 1;
        return 1;
    }
    ++trace.phase_count;
    trace.last_pc = pc;
    trace.last_tick = tick;
    if (pc == 0x80012370)
        trace.game_time = r_u32(0x800D5D44);
    if (pc == 0x8001241C)
        trace.aux_time = r_u32(0x800D5D44);
    if (!trace_progress(pc, tick))
        RRJ_DEBUG_FPRINTF(stderr, "Diagnostic progress update failed at phase %u tick %u\n", trace.phase_base + trace.phase_count, tick);
    return trace.failed || trace.phase_count == trace.phase_limit;
#else
    return 1;
#endif
}

int rrj_trace_input_packet(RRJMemory *m, uint32_t controller, uint32_t raw)
{
    uint32_t expected[2];
    uint8_t packet[36];
    uint32_t tick = r_u32(0x800D5D48);

    if (trace.live)
    {
        uint32_t buttons = PadRead((sint32)controller);
        uint8_t *live_packet = xport_guest_ptr(raw, 4);

        live_packet[0] = 0;
        live_packet[1] = 0x41;
        live_packet[2] = (uint8_t)(~buttons >> 8);
        live_packet[3] = (uint8_t)~buttons;
        return 1;
    }
    if (!trace.input_calls)
        return 1;
    if (trace.failed)
        return 0;
    if (fread(expected, sizeof(expected), 1, trace.input_calls) != 1 || fread(packet, sizeof(packet), 1, trace.input_calls) != 1 || expected[0] != tick || expected[1] != controller)
    {
        RRJ_DEBUG_FPRINTF(stderr,
                "Diagnostic input mismatch ordinal %u actual_tick %u expected_tick %u "
                "actual_controller %u expected_controller %u vblank %d poll_counter %u\n",
                trace.input_ordinal, tick, expected[0], controller, expected[1], VSync(-1), r_u32(r_u32(0x8005B2F8) + 100));
        trace.failed = 1;
        return 0;
    }
    xport_guest_copy(xport_guest_ref(raw), xport_host_ref(packet), sizeof(packet));
    ++trace.input_ordinal;
    return 1;
}

uint32_t rrj_trace_vblanks_before_aux(RRJMemory *m)
{
    long position;
    uint32_t ordinal;
    uint32_t current;
    uint32_t current_time;
    uint32_t advance = 0;
    uint32_t target;
    uint32_t expected[2];
    uint8_t packet[36];
    uint32_t result = 0;

    if (!trace.input_calls || trace.failed)
        return 0;
    position = ftell(trace.input_calls);
    if (position < 0)
    {
        trace.failed = 1;
        return 0;
    }
    current = r_u32(0x800D5D48);
    ordinal = trace.input_ordinal;
    while (ordinal < trace.input_end && fread(expected, sizeof(expected), 1, trace.input_calls) == 1 && fread(packet, sizeof(packet), 1, trace.input_calls) == 1)
    {
        ++ordinal;
        if (expected[0] == current)
            continue;
        if (expected[0] > current && expected[0] - current <= 30)
            advance = expected[0] - current;
        break;
    }
    if (fseek(trace.input_calls, position, SEEK_SET))
    {
        trace.failed = 1;
        return 0;
    }
    current_time = r_u32(0x800D5D44);
    target = (trace.aux_time ? trace.aux_time : trace.game_time) + advance;
    if (target > current_time && (target - current_time) % 5 == 0)
        result = (target - current_time) / 5;
    return result;
}

int rrj_trace_input_pending_at_tick(RRJMemory *m)
{
    long position;
    uint32_t expected[2];
    int result;

    if (!trace.input_calls || trace.failed || trace.input_ordinal >= trace.input_end)
        return 0;
    position = ftell(trace.input_calls);
    if (position < 0 || fread(expected, sizeof(expected), 1, trace.input_calls) != 1 || fseek(trace.input_calls, position, SEEK_SET))
    {
        trace.failed = 1;
        return 0;
    }
    result = expected[0] == r_u32(0x800D5D48);
    return result;
}

int rrj_trace_runtime_failed(void)
{
    return trace.failed;
}

void rrj_trace_dispatch_target(uint32_t target)
{
    trace.dispatch_target = target;
}

uint32_t rrj_trace_current_dispatch(void)
{
    return trace.dispatch_target;
}

int rrj_trace_runtime_finish(int result)
{
    uint32_t footer[4] = {5, 0, 4, 0};
    uint32_t expected[2];
    uint8_t packet[36];
    int ok;

    if (trace.live)
    {
        if (trace.disc)
            fclose(trace.disc);
        memset(&trace, 0, sizeof(trace));
        return result;
    }
    if (!trace.failed && trace.phase_count == trace.phase_limit && trace.last_pc == 0x80012370 && trace.input_ordinal + 1 == trace.input_end && fread(expected, sizeof(expected), 1, trace.input_calls) == 1 && fread(packet, sizeof(packet), 1, trace.input_calls) == 1 && expected[0] == trace.last_tick)
        ++trace.input_ordinal;
    ok = !trace.failed && trace.phase_count == trace.phase_limit && trace.input_ordinal == trace.input_end && result == 0;
    footer[3] = ok != 0;

    if (trace.phases && (fwrite(footer, sizeof(footer), 1, trace.phases) != 1 || fclose(trace.phases)))
        ok = 0;
    if (trace.input_calls && fclose(trace.input_calls))
        ok = 0;
    if (trace.actors && fclose(trace.actors))
        ok = 0;
    if (trace.world && fclose(trace.world))
        ok = 0;
    if (trace.inputs && fclose(trace.inputs))
        ok = 0;
    if (trace.disc && fclose(trace.disc))
        ok = 0;
    RRJ_DEBUG_PRINTF("diagnostic_input_calls %u\n", trace.input_ordinal);
    return ok ? 0 : 14;
}
