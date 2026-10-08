#include "race_trace_runtime.h"
#include "psx.h"
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
    int has_pending_input;
    int failed;
    int live;
    char progress_path[1024];
} RRJTraceRuntime;

static RRJTraceRuntime trace;
static uint32 live_limit_override;

void rrj_trace_set_live_limit(uint32 limit)
{
    live_limit_override = limit;
}

static uint8_t trace_bios[524288];

static int trace_load_bios(RRJMemory *m)
{
    const char *path = getenv("RRJ_BIOS_IMAGE");
    FILE *stream = xport_fopen(path && *path ? path : "../tools/duckstation/data/user/bios/scph5502.bin", "rb");
    int loaded;

    if (!stream)
        return 0;
    loaded = fread(trace_bios, sizeof(trace_bios), 1, stream) == 1 && fgetc(stream) == EOF;
    if (fclose(stream))
        loaded = 0;
    if (loaded)
    {
        m->bios = trace_bios;
        rrj_memory_bind(m);
    }
    return loaded;
}

static int trace_load_context(uint32_t *input_ordinal)
{
    const char *checkpoint = getenv("RRJ_PHASE_CHECKPOINT_LOAD");
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

static int trace_phase_gpu(uint32_t pc, uint32_t tick)
{
    uint32_t record[5] = {17, tick, 8, pc, 0};

    return fwrite(record, sizeof(record), 1, trace.phases) == 1;
}

static int trace_cd_transfer(RRJMemory *m)
{
    const char *path = getenv("RRJ_DISC_IMAGE");
    uint8_t buffer[2048];
    uint32_t sector = rrj_read32(m, 0x800D6720u);
    uint32_t destination = rrj_read32(m, 0x800D6714u);
    uint32_t count = rrj_read32(m, 0x800D6718u);
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

static int trace_game_boundary(RRJMemory *m, uint32_t tick)
{
    uint32_t count = rrj_read32(m, 0x800D5D68);
    uint32_t zero = 0;
    uint32_t input[3];

    if (count > 4)
        return 0;
    if (fwrite(&tick, 4, 1, trace.actors) != 1 || fwrite(rrj_at(m, 0x800D6DE0, 768), 768, 1, trace.actors) != 1 || fwrite(rrj_at(m, 0x800D5D38, 256), 256, 1, trace.actors) != 1)
        return 0;
    if (fwrite(&tick, 4, 1, trace.world) != 1 || fwrite(&count, 4, 1, trace.world) != 1 || fwrite(rrj_at(m, 0x800D5D38, 256), 256, 1, trace.world) != 1 || fwrite(rrj_at(m, 0x800D6DE0, 768), 768, 1, trace.world) != 1 || fwrite(rrj_at(m, 0x800D6170, 64 * count), 64, count, trace.world) != count || fwrite(rrj_at(m, 0x8005B580, 64), 64, 1, trace.world) != 1 || fwrite(rrj_at(m, 0x800D5D6C, 4), 4, 1, trace.world) != 1 || fwrite(rrj_at(m, 0x8005B46C, 4), 4, 1, trace.world) != 1 || fwrite(&zero, 4, 1, trace.world) != 1)
        return 0;
    input[0] = tick;
    input[1] = (~rrj_u16(rrj_at(m, 0x800D70E2, 2))) & 0xFFFF;
    input[2] = input[1];
    if (trace.has_pending_input && fwrite(trace.pending_input, sizeof(trace.pending_input), 1, trace.inputs) != 1)
        return 0;
    memcpy(trace.pending_input, input, sizeof(input));
    trace.has_pending_input = 1;
    return 1;
}

int rrj_trace_runtime_init(RRJMemory *m, uint32_t phase_base)
{
    const char *phase_path = getenv("RRJ_TRACE_PHASE_PATH");
    const char *calls_path = getenv("RRJ_TRACE_INPUT_CALLS");
    const char *output = getenv("RRJ_AUDIT_OUTPUT");
    const char *progress = getenv("RRJ_PROGRESS_PATH");
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
        uint32_t display = rrj_read32(m, 0x8005B470);
        uint32_t slots = r_u8(display + 244);
        uint32_t slot;

        if (slots > 16)
            return 0;
        for (slot = 0; slot < slots; ++slot)
            if (rrj_read32(m, 0x800D75D0 + 64 * slot))
                trace.pending_store_slots |= 1u << slot;
    }
    return 1;
}

int rrj_trace_runtime_init_live(RRJMemory *m)
{
    const char *limit = getenv("RRJ_LIVE_FRAME_LIMIT");

    memset(&trace, 0, sizeof(trace));
    trace.live = 1;
    if (live_limit_override)
        trace.phase_limit = live_limit_override;
    else if (limit && *limit)
        trace.phase_limit = (uint32_t)strtoul(limit, NULL, 10);
    return trace_load_bios(m);
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
                rrj_write32(m, 0x800D75D0 + 64 * slot, 0);
        trace.pending_store_slots = 0;
    }
    cd_state = rrj_read32(m, 0x800D6728u);
    if (cd_state == 1u)
    {
        trace.cd_pending = 1;
        rrj_write32(m, 0x800D6728u, 2u);
    }
    else if (trace.cd_pending && cd_state == 2u)
    {
        if (!trace_cd_transfer(m))
        {
            fprintf(stderr, "Diagnostic CD transfer failed at sector %u\n", rrj_read32(m, 0x800D6720u));
            abort();
        }
        rrj_write32(m, 0x800D6728u, 5u);
        rrj_write32(m, 0x800D6724u, rrj_read32(m, 0x800D6720u));
        rrj_write32(m, 0x8005AF60u, 0);
    }
    else if (trace.cd_pending && cd_state == 5u)
    {
        trace.cd_pending = 0;
        rrj_write32(m, 0x800D6728u, 0);
        return rrj_read32(m, 0x800D6710u);
    }
    return 0;
}

int rrj_trace_phase_boundary(RRJMemory *m, uint32_t pc)
{
    uint32_t tick = rrj_read32(m, 0x800D5D48);
    uint32_t header[3] = {15, tick, 1312};
    uint32_t context[8] = {pc, (uint32_t)VSync(-1), 0, 0, rrj_read32(m, 0x8005B2F8), rrj_read32(m, 0x8005B2F8), rrj_read32(m, 0x800D70E0), rrj_read32(m, 0x800D7104)};

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
    if (trace.failed || trace.phase_count >= trace.phase_limit)
        return 1;
    if (fwrite(header, sizeof(header), 1, trace.phases) != 1 || fwrite(context, sizeof(context), 1, trace.phases) != 1 || fwrite(rrj_at(m, 0x800D5D38, 256), 256, 1, trace.phases) != 1 || fwrite(rrj_at(m, 0x800D6DE0, 768), 768, 1, trace.phases) != 1 || fwrite(rrj_at(m, 0x800D5D38, 256), 256, 1, trace.phases) != 1 || !trace_phase_gpu(pc, tick) || (pc == 0x80012370 && !trace_game_boundary(m, tick)))
    {
        trace.failed = 1;
        return 1;
    }
    ++trace.phase_count;
    trace.last_pc = pc;
    trace.last_tick = tick;
    if (pc == 0x80012370)
        trace.game_time = rrj_read32(m, 0x800D5D44);
    if (pc == 0x8001241C)
        trace.aux_time = rrj_read32(m, 0x800D5D44);
    if (!trace_progress(pc, tick))
        fprintf(stderr, "Diagnostic progress update failed at phase %u tick %u\n", trace.phase_base + trace.phase_count, tick);
    return trace.failed || trace.phase_count == trace.phase_limit;
}

int rrj_trace_input_packet(RRJMemory *m, uint32_t controller, uint32_t raw)
{
    uint32_t expected[2];
    uint8_t packet[36];
    uint32_t tick = rrj_read32(m, 0x800D5D48);

    if (trace.live)
    {
        uint32_t buttons = PadRead((sint32)controller);
        uint8_t *live_packet = rrj_at(m, raw, 4);

        live_packet[0] = 0;
        live_packet[1] = 0x41;
        live_packet[2] = (uint8_t)~buttons;
        live_packet[3] = (uint8_t)(~buttons >> 8);
        return 1;
    }
    if (!trace.input_calls)
        return 1;
    if (trace.failed)
        return 0;
    if (fread(expected, sizeof(expected), 1, trace.input_calls) != 1 || fread(packet, sizeof(packet), 1, trace.input_calls) != 1 || expected[0] != tick || expected[1] != controller)
    {
        fprintf(stderr,
                "Diagnostic input mismatch ordinal %u actual_tick %u expected_tick %u "
                "actual_controller %u expected_controller %u vblank %d poll_counter %u\n",
                trace.input_ordinal, tick, expected[0], controller, expected[1], VSync(-1), rrj_read32(m, rrj_read32(m, 0x8005B2F8) + 100));
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
    current = rrj_read32(m, 0x800D5D48);
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
    current_time = rrj_read32(m, 0x800D5D44);
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
    result = expected[0] == rrj_read32(m, 0x800D5D48);
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
    printf("diagnostic_input_calls %u\n", trace.input_ordinal);
    return ok ? 0 : 14;
}
