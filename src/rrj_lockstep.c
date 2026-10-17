#include "rrj_debug.h"
#include "rrj_lockstep.h"
#if defined(LOCKSTEP_DEBUG)
    #include "xport_trace.h"
    #include "psx_spu.h"
    #include "psx_gpu.h"
    #include "psx_press.h"
    #include "psx_stream.h"
    #include "race_pause.h"
    #include "race_pause_frontier.h"
    #include "race_frame.h"
    #include "spu.h"
    #include "race_audio_frontier.h"
    #include "rrj_cd_transport.h"
    #include "sdk_build_stubs.h"
    #include "race_bodyless_batch_002.h"
    #include "race_bodyless_batch_001.h"
    #include "video_preview.h"
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <errno.h>

typedef struct RRJLockstepGpu
{
    uint64 clock;
    uint32 dma[6];
    sint32 pending_ticks;
    uint32 status, command_words, event_active;
    uint64 next_clock, last_clock;
    sint32 period, interval;
    uint32 fifo_count;
    uint64 fifo[256];
} RRJLockstepGpu;

typedef struct RRJLockstepWait
{
    RRJMemory *memory;
    uint32 race;
    RRJLockstepGpu gpu;
} RRJLockstepWait;

static RRJLockstepWait *active_wait;
static uint32 initial_frame_resumed;

static PsxCrtcState device_crtc;
static uint64 device_clock;
static uint64 cd_drive[6];
static uint32 cd_ready_pending;
static uint32 cd_location_pending, cd_location_sector;

static void rrj_lockstep_cd_command(uint8 command, uint32 next_sector, uint32 mode)
{
    if (command == 2)
    {
        cd_location_pending = 1;
        cd_location_sector = next_sector;
    }
    else if (command == CdlPause || command == CdlStop)
    {
        cd_drive[0] = 0;
        cd_drive[5] &= ~0xE0;
        cd_set_reading_native(0);
        cd_ready_pending = 0;
    }
    else if (command == CdlSetmode)
    {
        cd_drive[4] = mode;
        cd_drive[2] = 33868800 / ((mode & 0x80) ? 150 : 75);
    }
    else if (command == 6 || command == 27)
    {
        if (!cd_drive[0] || cd_location_pending)
        {
            cd_drive[3] = cd_location_pending ? cd_location_sector : next_sector;
            cd_drive[4] = mode;
            cd_drive[2] = 33868800 / ((mode & 0x80) ? 150 : 75);
            cd_drive[1] = device_clock + cd_drive[2];
            cd_ready_pending = 0;
            if (!cd_restore_fifo_native(NULL, 0, mode, (uint32)cd_drive[3]))
                abort();
        }
        cd_location_pending = 0;
        cd_drive[0] = 1;
        cd_drive[5] = (cd_drive[5] & ~0xE0) | 0x20;
        cd_set_reading_native(1);
    }
}

static sint32 rrj_lockstep_import_gpu(uint32 frame)
{
    const char *path = RRJ_DEBUG_GETENV("XPORT_LOCKSTEP_HANDOFF_GPU_STATE");
    const char *record = RRJ_DEBUG_GETENV("XPORT_LOCKSTEP_RECORD_ID");
    FILE *file = path ? xport_fopen(path, "rb") : NULL;
    uint32 header[6], fifo_count = 0, packet[257], index;
    uint8 record_id[32];
    GpuRasterState raster;
    sint32 ok;
    char phase_path[1024];
    size_t path_length;
    uint32 phase_header[4];
    uint8 phase_record[32];
    if (!file)
        return 0;
    ok = fread(header, sizeof(header), 1, file) == 1 && fread(record_id, sizeof(record_id), 1, file) == 1 && fread(&raster, sizeof(raster), 1, file) == 1 && fread(&fifo_count, sizeof(fifo_count), 1, file) == 1;
    if (!ok || header[0] != 0x31485047 || header[1] != 1 || header[2] != frame || header[3] != 0x80012370 || fifo_count > 256 || !record || strlen(record) != 64)
    {
        fclose(file);
        return 0;
    }
    for (index = 0; index < 32; ++index)
    {
        char pair[3] = {record[2 * index], record[2 * index + 1], 0};
        if (record_id[index] != strtoul(pair, NULL, 16))
        {
            fclose(file);
            return 0;
        }
    }
    ok = fread(packet + 1, sizeof(uint32), fifo_count, file) == fifo_count && fread(VRAM, sizeof(VRAM), 1, file) == 1 && fgetc(file) == EOF;
    if (fclose(file) || !ok || !gpu_import_raster(&raster, VRAM, sizeof(VRAM)))
        return 0;
    for (index = 0; index < fifo_count; ++index)
        if ((packet[index + 1] >> 24) < 0xE1 || (packet[index + 1] >> 24) > 0xE6)
            return 0;
    if (!active_wait || !active_wait->race || ((header[5] & 0x01000000) && (header[5] & 0x00000603) != 0x00000401))
        return 0;
    path_length = strlen(path);
    if (path_length < 4 || path_length + 7 > sizeof(phase_path) || strcmp(path + path_length - 4, ".bin"))
        return 0;
    memcpy(phase_path, path, path_length - 4);
    memcpy(phase_path + path_length - 4, ".phase.bin", 11);
    file = xport_fopen(phase_path, "rb");
    if (!file)
        return 0;
    ok = fread(phase_header, sizeof(phase_header), 1, file) == 1 && fread(phase_record, sizeof(phase_record), 1, file) == 1 && fread(&active_wait->gpu.clock, sizeof(uint64), 1, file) == 1 && fread(active_wait->gpu.dma, sizeof(active_wait->gpu.dma), 1, file) == 1 && fread(&active_wait->gpu.pending_ticks, sizeof(sint32), 1, file) == 1 && fread(&active_wait->gpu.status, sizeof(uint32), 1, file) == 1 && fread(&active_wait->gpu.command_words, sizeof(uint32), 1, file) == 1 && fread(&active_wait->gpu.event_active, sizeof(uint32), 1, file) == 1 && fread(&active_wait->gpu.next_clock, sizeof(uint64), 1, file) == 1 && fread(&active_wait->gpu.last_clock, sizeof(uint64), 1, file) == 1 && fread(&active_wait->gpu.period, sizeof(sint32), 1, file) == 1 && fread(&active_wait->gpu.interval, sizeof(sint32), 1, file) == 1 && fread(&active_wait->gpu.fifo_count, sizeof(uint32), 1, file) == 1;
    if (!ok || active_wait->gpu.fifo_count != fifo_count || phase_header[0] != 0x31435047 || phase_header[1] != 1 || phase_header[2] != frame || phase_header[3] != header[3] || memcmp(phase_record, record_id, sizeof(record_id)) || active_wait->gpu.clock != device_clock || active_wait->gpu.dma[0] != header[4] || active_wait->gpu.dma[2] != header[5] || active_wait->gpu.dma[3] > 1 || active_wait->gpu.event_active > 1 || active_wait->gpu.command_words || (active_wait->gpu.event_active && (active_wait->gpu.last_clock > active_wait->gpu.clock || active_wait->gpu.next_clock <= active_wait->gpu.clock)) || (!active_wait->gpu.event_active && (active_wait->gpu.pending_ticks > 0 || active_wait->gpu.next_clock || active_wait->gpu.last_clock || active_wait->gpu.period || active_wait->gpu.interval)))
    {
        fclose(file);
        return 0;
    }
    ok = fread(active_wait->gpu.fifo, sizeof(uint64), fifo_count, file) == fifo_count && fgetc(file) == EOF;
    if (fclose(file) || !ok)
        return 0;
    for (index = 0; index < fifo_count; ++index)
        if ((uint32)active_wait->gpu.fifo[index] != packet[index + 1])
            return 0;
    /* Preserve pending device work and guest queue without executing it */
    return !gpu_linked_submit_failed() && !gpu_transfer_observation_failed();
}

static uint64 rrj_lockstep_clock(void *context)
{
    return device_clock;
}

static sint32 rrj_lockstep_guest_callback(void *context, uint32 address)
{
    if (address == 0x8001DF64)
        (void)sub_8001DF64();
    else if (address == 0x8001B500)
        (void)sub_8001B500();
    else
    {
        RRJ_DEBUG_FPRINTF(stderr, "RRJ lockstep: unsupported BIOS callback %08X\n", address);
        abort();
    }
    return 1;
}

static void rrj_lockstep_cd_irq(void)
{
    while (cd_ready_pending)
    {
        uint8 response = (uint8)cd_drive[5];
        --cd_ready_pending;
        cd_publish_guest_irq(1, &response, 1);
    }
}

static sint32 rrj_lockstep_import_cd(void)
{
    const char *path = RRJ_DEBUG_GETENV("XPORT_LOCKSTEP_HANDOFF_CD_STATE");
    const char *disc = RRJ_DEBUG_GETENV("XPORT_DISC_IMAGE");
    FILE *file = path ? xport_fopen(path, "rb") : NULL;
    uint32 count = 0;
    uint8 fifo[2340];
    sint32 ok;
    if (!file)
        return 0;
    ok = fread(cd_drive, sizeof(cd_drive), 1, file) == 1 && fread(&count, sizeof(count), 1, file) == 1 && count <= sizeof(fifo) && fread(fifo, 1, count, file) == count && fgetc(file) == EOF;
    if (fclose(file) || !ok || !disc || !cd_mount_cue(disc) || !cd_restore_fifo_native(fifo, count, (uint32)cd_drive[4], (uint32)cd_drive[3]))
        return 0;
    cd_ready_pending = 0;
    cd_location_pending = 0;
    (void)InterruptCallback(2u, rrj_lockstep_cd_irq);
    cd_bind_command_native(rrj_lockstep_cd_command);
    cd_set_reading_native((sint32)cd_drive[0]);
    return 1;
}

static sint32 rrj_lockstep_advance_devices(uint64 until, uint32 *vblanks)
{
    uint64 delta = until - device_clock;
    PsxCrtcStep step;
    if (until < device_clock || delta > 0x7fffffffu || !psx_crtc_advance(&device_crtc, (uint32)delta, &step))
        return 0;
    *vblanks += step.vblanks;
    device_clock = until;
    psx_root_counter_poll();
    return psx_timers_advance(0u, step.dots, step.hblanks);
}

static sint32 rrj_lockstep_cd_sector_event(void)
{
    sint32 result;
    if (!cd_drive[0] || !psx_callbacks_allowed())
        return 0;
    result = cd_read_sector_fifo_native((uint32)cd_drive[3], (uint32)cd_drive[4]);
    if (!result)
        return 0;
    ++cd_drive[3];
    cd_drive[1] += cd_drive[2];
    if (result == 1)
    {
        ++cd_ready_pending;
        psx_interrupt_raise(2);
    }
    return 1;
}

static sint32 rrj_lockstep_import_clock(void)
{
    const char *path = RRJ_DEBUG_GETENV("XPORT_LOCKSTEP_HANDOFF_CLOCK_STATE");
    FILE *file = path ? xport_fopen(path, "rb") : NULL;
    PsxTimerState timers;
    PsxIrqState irq;
    uint8 fired[3];
    sint32 ok;
    if (!file)
        return 0;
    ok = fread(&device_clock, sizeof(device_clock), 1, file) == 1 && fread(&device_crtc, sizeof(device_crtc), 1, file) == 1 && fread(&timers, sizeof(timers), 1, file) == 1 && fread(&irq, sizeof(irq), 1, file) == 1 && fread(fired, sizeof(fired), 1, file) == 1 && fgetc(file) == EOF;
    if (fclose(file) || !ok || !psx_crtc_validate(&device_crtc))
        return 0;
    psx_root_counter_bind_clock(rrj_lockstep_clock, NULL);
    (void)InterruptCallback(0, NULL);
    (void)rrj_spu_irq_callback_psx(r_u32(0x8005A458));
    if (!psx_timers_import(&timers) || !psx_root_counter_import_irq(&irq, fired) || !psx_bios_bind_events(r_u32(0x120u), r_u32(0x124u)))
        return 0;
    psx_bios_bind_guest_callback_service(rrj_lockstep_guest_callback, NULL);
    psx_pad_bind_owner_packets(0x800D9D70, 0x800D9E60, r_u32(0x80054900), 0x800DA6D0, 0x80054948, 0x80054944);
    return 1;
}

enum
{
    RRJ_MENU_IDLE,
    RRJ_MENU_WAITING,
    RRJ_MENU_AFTER_FRAME,
    RRJ_MENU_PENDING_IRQ,
    RRJ_RACE_PENDING_IRQ,
    RRJ_RACE_AFTER_FRAME,
    RRJ_RACE_PENDING_FRAME
};

typedef struct RRJMenuCheckpoint
{
    uint32 magic, version, phase, ready_context, cpu_status, menu_stack_pointer;
    uint8 root_counters[48];
    uint64 clock;
    PsxCrtcState crtc;
    uint64 cd[6];
    uint32 cd_pending;
    uint32 cd_location_pending, cd_location_sector;
    uint32 race_stage;
    RRJObjectContinuation objects;
    RRJLockstepGpu gpu;
} RRJMenuCheckpoint;

static RRJMenuCheckpoint menu_checkpoint;

void rrj_lockstep_object_wait(uint32_t player, uint32_t stage, uint32_t far_count, const uint32_t *far_indices, uint32_t near_count, const uint32_t *near_indices, int32_t near_minimum, int32_t near_maximum)
{
    RRJObjectContinuation saved;
    RRJObjectContinuation *objects = &saved;
    if (!active_wait || !active_wait->race)
        return;
    if (far_count > 12u || near_count > 12u || (stage != 1u && stage != 2u))
        abort();
    memset(objects, 0, sizeof(*objects));
    objects->player_index = player;
    objects->far_count = far_count;
    objects->near_count = near_count;
    objects->next_far_index = far_count;
    objects->near_minimum = near_minimum;
    objects->near_maximum = near_maximum;
    memcpy(objects->far_indices, far_indices, far_count * sizeof(uint32));
    memcpy(objects->near_indices, near_indices, near_count * sizeof(uint32));
    menu_checkpoint.objects = saved;
    menu_checkpoint.ready_context = r_u32(0x8005B470u);
    menu_checkpoint.race_stage = stage;
}

void rrj_lockstep_object_wait_end(void)
{
    menu_checkpoint.race_stage = 0u;
}

void rrj_lockstep_object_wait_resume(void)
{
    uint32 context = menu_checkpoint.ready_context;
    if (!psx_wait_events_resume())
        abort();
    while (!r_u8(context + 4u))
        rrj_display_wait_step();
}

static sint32 rrj_lockstep_checkpoint_prepare(void)
{
    RRJMemory *memory = active_wait ? active_wait->memory : NULL;
    if (!memory || (active_wait->race && ((menu_checkpoint.phase != RRJ_RACE_PENDING_IRQ && menu_checkpoint.phase != RRJ_RACE_PENDING_FRAME) || !menu_checkpoint.race_stage)))
        return 0;
    menu_checkpoint.cpu_status = memory->cpu_status;
    menu_checkpoint.menu_stack_pointer = memory->menu_stack_pointer;
    memcpy(menu_checkpoint.root_counters, memory->root_counters, sizeof(menu_checkpoint.root_counters));
    menu_checkpoint.clock = device_clock;
    menu_checkpoint.crtc = device_crtc;
    memcpy(menu_checkpoint.cd, cd_drive, sizeof(cd_drive));
    menu_checkpoint.cd_pending = cd_ready_pending;
    menu_checkpoint.cd_location_pending = cd_location_pending;
    menu_checkpoint.cd_location_sector = cd_location_sector;
    menu_checkpoint.gpu = active_wait->gpu;
    return 1;
}

static void rrj_lockstep_restore_memory(RRJMemory *memory)
{
    memory->cpu_status = menu_checkpoint.cpu_status;
    memory->menu_stack_pointer = menu_checkpoint.menu_stack_pointer;
    memcpy(memory->root_counters, menu_checkpoint.root_counters, sizeof(memory->root_counters));
    device_clock = menu_checkpoint.clock;
    device_crtc = menu_checkpoint.crtc;
    memcpy(cd_drive, menu_checkpoint.cd, sizeof(cd_drive));
    cd_ready_pending = menu_checkpoint.cd_pending;
    cd_location_pending = menu_checkpoint.cd_location_pending;
    cd_location_sector = menu_checkpoint.cd_location_sector;
    active_wait->gpu = menu_checkpoint.gpu;
    cd_bind_command_native(rrj_lockstep_cd_command);
    (void)InterruptCallback(2, rrj_lockstep_cd_irq);
    psx_root_counter_bind_clock(rrj_lockstep_clock, NULL);
    (void)InterruptCallback(0, NULL);
    (void)rrj_spu_irq_callback_psx(r_u32(0x8005A458));
    if (!psx_bios_bind_events(r_u32(0x120u), r_u32(0x124u)))
        abort();
    psx_bios_bind_guest_callback_service(rrj_lockstep_guest_callback, NULL);
    psx_pad_bind_owner_packets(0x800D9D70, 0x800D9E60, r_u32(0x80054900), 0x800DA6D0, 0x80054948, 0x80054944);
}

sint32 xport_game_checkpoint_io(FILE *file, sint32 load)
{
    RRJMenuCheckpoint state;
    RRJMemory *memory = active_wait ? active_wait->memory : NULL;
    if (!file || !memory || memory->bios || memory->checkpoint_abi_valid)
        return 0;
    if (load)
    {
        if (fread(&state, sizeof(state), 1, file) != 1 || state.magic != 0x4A52524Du || state.version != 2u || (state.phase != RRJ_MENU_AFTER_FRAME && state.phase != RRJ_MENU_PENDING_IRQ && state.phase != RRJ_RACE_PENDING_IRQ && state.phase != RRJ_RACE_PENDING_FRAME) || (state.ready_context & 0x1FFFFFFFu) > PSX_DRAM_SIZE - 5u || (!active_wait->race && r_u8(r_u32(0x8005B2F8u)) != 2u))
            return 0;
        menu_checkpoint = state;
        rrj_lockstep_restore_memory(memory);
    }
    else
    {
        if ((menu_checkpoint.phase != RRJ_MENU_AFTER_FRAME && menu_checkpoint.phase != RRJ_MENU_PENDING_IRQ && menu_checkpoint.phase != RRJ_RACE_PENDING_IRQ && menu_checkpoint.phase != RRJ_RACE_PENDING_FRAME) || (!active_wait->race && r_u8(r_u32(0x8005B2F8u)) != 2u))
            return 0;
        if (!rrj_lockstep_checkpoint_prepare())
            return 0;
        menu_checkpoint.magic = 0x4A52524Du;
        menu_checkpoint.version = 2u;
        menu_checkpoint.cpu_status = memory->cpu_status;
        menu_checkpoint.menu_stack_pointer = memory->menu_stack_pointer;
        memcpy(menu_checkpoint.root_counters, memory->root_counters, sizeof(menu_checkpoint.root_counters));
        menu_checkpoint.clock = device_clock;
        menu_checkpoint.crtc = device_crtc;
        memcpy(menu_checkpoint.cd, cd_drive, sizeof(cd_drive));
        menu_checkpoint.cd_pending = cd_ready_pending;
        menu_checkpoint.cd_location_pending = cd_location_pending;
        menu_checkpoint.cd_location_sector = cd_location_sector;
        if (fwrite(&menu_checkpoint, sizeof(menu_checkpoint), 1, file) != 1)
            return 0;
    }
    if (!rrj_game_checkpoint_io(file, load) || !rrj_spu_checkpoint_io(file, load) || !rrj_audio_checkpoint_io(file, load) || !mdec_psyq_checkpoint_io(file, load))
        return 0;
    if (load)
        live_decoder_restore();
    return 1;
}

static sint32 rrj_lockstep_wait(void *context)
{
    RRJLockstepWait *wait = (RRJLockstepWait *)context;
    uint32 pending_irq = menu_checkpoint.phase == RRJ_MENU_PENDING_IRQ || menu_checkpoint.phase == RRJ_RACE_PENDING_IRQ;
    uint32 pending_frame = menu_checkpoint.phase == RRJ_RACE_PENDING_FRAME;
    if (wait != active_wait || (!wait->race && menu_checkpoint.phase != RRJ_MENU_WAITING && menu_checkpoint.phase != RRJ_MENU_PENDING_IRQ))
        return 0;
    if (!pending_irq && !pending_frame)
    {
        uint64 frame_clock;
        uint32 vblanks = 0u;
        sint32 frame_boundary = wait->race ? xport_lockstep_device_frame_boundary() : 1;
        if (frame_boundary < 0 || !xport_lockstep_device_clock(&frame_clock) || frame_clock < device_clock || (frame_clock == device_clock && !frame_boundary))
            return 0;
        while (cd_drive[0] && cd_drive[1] < frame_clock)
        {
            if (!cd_drive[2] || !rrj_lockstep_advance_devices(cd_drive[1], &vblanks))
                return 0;
            if (!rrj_lockstep_cd_sector_event())
                return 0;
        }
        if (!rrj_lockstep_advance_devices(frame_clock, &vblanks) || vblanks > 1u)
            return 0;
        pending_irq = !wait->race || vblanks != 0u;
        /* Complete audio work before the published Original boundary */
        if (!spu_lockstep_checkpoint_synchronize())
            return 0;
        spu_dispatch_pending_irq();
        /* Deliver an IRQ before its deferred FrameDone */
        if (frame_boundary == 3 && pending_irq)
            frame_boundary = 0;
        if (frame_boundary)
        {
            menu_checkpoint.phase = !wait->race ? RRJ_MENU_PENDING_IRQ : pending_irq ? RRJ_RACE_PENDING_IRQ : RRJ_RACE_PENDING_FRAME;
            if (psx_vblank_failed() || !xport_lockstep_frame_done())
                return 0;
        }
    }
    if (pending_irq)
        psx_vblank_signal();
    rrj_spu_completion_service();
    rrj_cd_stream_service();
    if (psx_vblank_failed())
        return 0;
    menu_checkpoint.phase = wait->race ? RRJ_RACE_AFTER_FRAME : RRJ_MENU_AFTER_FRAME;
    return 1;
}

static sint32 rrj_lockstep_after_frame(void)
{
    RRJLockstepWait *wait = active_wait;
    if (!wait || menu_checkpoint.phase != RRJ_MENU_AFTER_FRAME)
        return 0;
    (void)sub_80064C30();
    menu_checkpoint.phase = RRJ_MENU_WAITING;
    return !psx_vblank_failed();
}

uint32_t rrj_lockstep_poll(RRJMemory *memory)
{
    if (active_wait && active_wait->memory == memory && active_wait->race)
    {
        if (!psx_wait_events())
        {
            RRJ_DEBUG_FPUTS("RRJ lockstep: race VBlank wait failed\n", stderr);
            abort();
        }
        return 1;
    }
    if (!active_wait || active_wait->memory != memory || !psx_wait_events() || !rrj_lockstep_after_frame())
    {
        RRJ_DEBUG_FPUTS("RRJ lockstep: VBlank wait failed\n", stderr);
        abort();
    }
    return 1u;
}

static int rrj_lockstep_import_gte(void)
{
    uint32 registers[32];
    unsigned index;
    for (index = 0; index < 32u; ++index)
    {
        char name[48], *end;
        const char *text;
        unsigned long value;
        snprintf(name, sizeof(name), "XPORT_LOCKSTEP_GTE_CONTROL_%u", index);
        text = RRJ_DEBUG_GETENV(name);
        if (!text || !*text || *text == '-')
            return 0;
        errno = 0;
        value = strtoul(text, &end, 0);
        if (*end || errno == ERANGE || value > UINT32_MAX)
            return 0;
        registers[index] = (uint32)value;
    }
    return psx_gte_import_control_registers(registers);
}

sint32 rrj_lockstep_import_initial_gte(const uint32 registers[64])
{
    static const uint8 signed_words[] = {1, 3, 5, 8, 9, 10, 11};
    static const uint8 unsigned_words[] = {7, 16, 17, 18, 19};
    static const uint8 command_words[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 18, 19, 20, 21, 22, 24, 25, 26, 27, 30};
    uint32 index, bits, leading = 0;
    if (!registers || registers[23] != 0)
        return 0;
    for (index = 0; index < sizeof(signed_words); ++index)
    {
        uint32 value = registers[signed_words[index]];
        if (value != (uint32)(sint32)(sint16)value)
            return 0;
    }
    for (index = 0; index < sizeof(unsigned_words); ++index)
        if (registers[unsigned_words[index]] > 0xFFFFu)
            return 0;
    bits = registers[30] & 0x80000000u ? ~registers[30] : registers[30];
    while (leading < 32 && !(bits & 0x80000000u))
    {
        ++leading;
        bits <<= 1;
    }
    if (registers[31] != leading || !psx_gte_import_control_registers(registers + 32))
        return 0;
    /* Restore the FIFO directly; writing SXYP would shift already restored entries */
    for (index = 0; index < sizeof(command_words); ++index)
        xport_gte_write_data(command_words[index], registers[command_words[index]]);
    for (index = 0; index < sizeof(command_words); ++index)
        if (xport_gte_read_data(command_words[index]) != registers[command_words[index]])
            return 0;
    return xport_gte_read_data(31) == registers[31];
}

sint32 rrj_lockstep_resume_initial_frame(const char *path)
{
    const char *record = RRJ_DEBUG_GETENV("XPORT_LOCKSTEP_RECORD_ID");
    const char *origin = RRJ_DEBUG_GETENV("RRJ_LOCKSTEP_INITIAL_STATE_SHA256");
    const char *start = RRJ_DEBUG_GETENV("XPORT_LOCKSTEP_START_FRAME");
    uint32 header[3] = {0}, clip[13] = {0}, row[4] = {0}, player = 0, index;
    uint8 identities[64], indices[9];
    RRJRoadContinuation road = {0};
    RRJObjectContinuation objects = {0};
    FILE *file;
    sint32 ok;
    if (!active_wait || !active_wait->race || initial_frame_resumed || !path || !record || strlen(record) != 64 || !origin || strlen(origin) != 64 || !start || strcmp(start, "0") || sizeof(road) != 612 || sizeof(objects) != 124)
        return 0;
    file = xport_fopen(path, "rb");
    if (!file)
        return 0;
    ok = fread(header, sizeof(header), 1, file) == 1 && fread(identities, sizeof(identities), 1, file) == 1 && fread(clip, sizeof(clip), 1, file) == 1 && fread(&road, sizeof(road), 1, file) == 1 && fread(row, sizeof(row), 1, file) == 1 && fread(&objects, sizeof(objects), 1, file) == 1 && fread(&player, sizeof(player), 1, file) == 1 && fgetc(file) == EOF;
    if (fclose(file) || !ok || header[0] != 0x314A5252 || header[1] != 1 || header[2] != 0x8006A0B0)
        return 0;
    for (index = 0; index < 64; ++index)
    {
        const char *text = index < 32 ? record : origin;
        uint32 position = 2 * (index & 31u);
        char pair[3] = {text[position], text[position + 1], 0};
        char *end;
        unsigned long value;
        if (!strchr("0123456789abcdefABCDEF", pair[0]) || !strchr("0123456789abcdefABCDEF", pair[1]))
            return 0;
        value = strtoul(pair, &end, 16);
        if (end != pair + 2 || identities[index] != value)
            return 0;
    }
    if (clip[2] >= 4 || road.remaining_faces == 0 || road.segment_count > 8 || (road.next_vertex_position & 1u) || road.next_vertex_position > 2 * road.segment_count || objects.far_count == 0 || objects.far_count > 12 || objects.near_count > 12 || objects.next_far_index > objects.far_count)
        return 0;
    for (index = 0; index < 9; ++index)
    {
        if (clip[index + 4] > 255)
            return 0;
        indices[index] = (uint8)clip[index + 4];
    }
    initial_frame_resumed = 1;
    /* All device state and the pending initial IRQ must be restored before entry */
    rrj_resume_sub_80069CF0(clip[0], clip[1], clip[2], indices, clip[3]);
    (void)rrj_resume_sub_8006E474(&road);
    rrj_resume_sub_80068FCC(row[0], row[1], row[2], row[3]);
    /* The intervening 80069200 epilogue returns 1 without game-memory writes */
    (void)rrj_resume_sub_80035958(&objects);
    (void)rrj_resume_sub_80011C4C(player);
    return 1;
}

static int rrj_lockstep_import_menu_stack(RRJMemory *memory)
{
    const char *text = RRJ_DEBUG_GETENV("XPORT_LOCKSTEP_CONTINUATION_ARG0");
    char *end;
    unsigned long value;
    uint32 physical, segment;
    if (!text || !*text || *text == '-')
        return 0;
    errno = 0;
    value = strtoul(text, &end, 0);
    if (*end || errno == ERANGE || value > UINT32_MAX)
        return 0;
    physical = (uint32)value & 0x1FFFFFFFu;
    segment = (uint32)value & 0xE0000000u;
    if ((segment != 0u && segment != 0x80000000u && segment != 0xA0000000u) || (physical & 3u) || physical < 0x30u || physical > PSX_DRAM_SIZE - 0x30u)
        return 0;
    memory->menu_stack_pointer = (uint32)value;
    return 1;
}

int rrj_lockstep_run(RRJMemory *memory)
{
    const char *continuation = RRJ_DEBUG_GETENV("XPORT_LOCKSTEP_CONTINUATION");
    const char *frame_text = RRJ_DEBUG_GETENV("XPORT_LOCKSTEP_START_FRAME");
    const char *spu_path = RRJ_DEBUG_GETENV("XPORT_LOCKSTEP_INITIAL_SPU_RAM");
    const char *load_path = RRJ_DEBUG_GETENV("XPORT_LOCKSTEP_PORTABLE_LOAD_PATH");
    char *end;
    unsigned long frame;
    RRJLockstepWait wait = {memory, 0};
    active_wait = &wait;
    xport_lockstep_checkpoint_bind(rrj_lockstep_checkpoint_prepare);
    if (!continuation || !frame_text || (strcmp(continuation, "rrj_menu_iteration") && strcmp(continuation, "rrj_race_iteration")))
    {
        RRJ_DEBUG_FPUTS("RRJ lockstep: unsupported or missing native continuation\n", stderr);
        return 64;
    }
    wait.race = !strcmp(continuation, "rrj_race_iteration");
    frame = strtoul(frame_text, &end, 10);
    if (!*frame_text || *end || frame == 0u || !xport_lockstep_frame_init("SLUS_010.53"))
        return 65;
    if (!xport_lockstep_frame_begin_at((uint32)frame))
    {
        RRJ_DEBUG_FPRINTF(stderr, "RRJ lockstep: %s\n", xport_lockstep_frame_error());
        xport_lockstep_frame_shutdown();
        return 65;
    }
    if (!wait.race && !(load_path && *load_path) && !rrj_lockstep_import_menu_stack(memory))
    {
        RRJ_DEBUG_FPUTS("RRJ lockstep: missing or invalid menu stack carrier\n", stderr);
        return 66;
    }
    if (!(load_path && *load_path) && !rrj_lockstep_import_gte())
    {
        RRJ_DEBUG_FPUTS("RRJ lockstep: missing or invalid GTE handoff state\n", stderr);
        return 66;
    }
    /* Bind existing guest pad buffers without clearing seeded state */
    {
        PsxPadReplayBinding pads = {{0x800D70E0u, 0x800D7104u}, 1u, {0, 0}};
        if (!pad_bind_replay(&pads))
            return 66;
    }
    /* Reuse the original SDK environment caches in the mapped game memory */
    {
        const GpuPsyqStateBinding gpu_state = {0x80055FD8u, 0x80055F7Cu, 0x80055F6Cu, 0x80055F6Fu, 0x80055F0Cu, 0u};
        if (!gpu_bind_psyq_state(&gpu_state))
            return 66;
    }
    if (load_path && *load_path)
    {
        uint32 next_frame;
        const char *disc = RRJ_DEBUG_GETENV("XPORT_DISC_IMAGE");
        if (!disc || !cd_mount_cue(disc))
            return 66;
        if (!xport_lockstep_checkpoint_load_portable(load_path, &next_frame) || !xport_lockstep_frame_begin_at(next_frame))
            return 69;
    }
    else
    {
        SpuInit();
        live_decoder_restore();
        if (!spu_path || !spu_load_ram_file(spu_path))
            return 67;
        memset(&menu_checkpoint, 0, sizeof(menu_checkpoint));
    }
    if (!(load_path && *load_path) && !spu_load_handoff_file(RRJ_DEBUG_GETENV("XPORT_LOCKSTEP_HANDOFF_SPU_STATE")))
    {
        RRJ_DEBUG_FPUTS("RRJ lockstep: missing SPU voice handoff state\n", stderr);
        return 66;
    }
    // Restore the SDK heap aliases from the guest state
    {
        SpuHeapBinding binding = {0};
        uint32 count = r_u32(0x8005A47Cu);
        uint32 shift = r_u32(0x8005A444u) & 31u;
        if (count >= 0x40000u)
            return 66;
        binding.entries = (uint32 *)xport_guest_ptr(r_u32(0x8005A484u), (count + 1u) * 8u);
        binding.last_index = (uint32 *)xport_guest_ptr(0x8005A480u, 4u);
        binding.capacity = count + 1u;
        binding.limit = count;
        binding.address_shift = shift;
        binding.alignment_mask = r_u32(0x8005A44Cu);
        binding.reserved_bytes = r_u32(0x8005A3B4u) ?
            (0x10000u - r_u32(0x8005A3B8u)) << shift : 0u;
        if (!spu_bind_heap(&binding))
            return 66;
    }
    if (!(load_path && *load_path))
    {
        // Reuse the seeded SDK transfer fields after restoring the SPU device
        SpuNativeTransferGuestBinding transfer = {0x8005A3ACu, 0x8005A438u, 0x8005A434u, 0x8005A450u, 0x8005A454u, 0x8005A470u, 0x8005A474u, r_u32(0x8005A444u) & 31u};
        if (!spu_bind_native_transfer_guest(&transfer))
            return 66;
    }
    if (!(load_path && *load_path) && !rrj_lockstep_import_clock())
    {
        RRJ_DEBUG_FPUTS("RRJ lockstep: missing or invalid clock handoff state\n", stderr);
        return 66;
    }
    if (wait.race && !(load_path && *load_path) && !rrj_lockstep_import_gpu((uint32)frame))
    {
        RRJ_DEBUG_FPUTS("RRJ lockstep: GPU handoff import failed\n", stderr);
        return 66;
    }
    if (!(load_path && *load_path) && !rrj_lockstep_import_cd())
    {
        RRJ_DEBUG_FPUTS("RRJ lockstep: missing or invalid CD handoff state\n", stderr);
        return 66;
    }
    psx_vblank_bind(rrj_lockstep_wait, &wait, r_u32(wait.race ? 0x80055ECC : 0x8005B46C));
    if (wait.race && !psx_vblank_bind_guest(0x80055ECC, 0x80055EAC, rrj_lockstep_guest_callback, NULL))
        return 66;
    if (setjmp(xport_lockstep_checkpoint_context))
    {
        rrj_lockstep_restore_memory(memory);
        psx_vblank_bind(rrj_lockstep_wait, &wait, r_u32(wait.race ? 0x80055ECC : 0x8005B46C));
        xport_lockstep_checkpoint_resumed();
    }
    else if (load_path && *load_path)
    {
        if (!xport_lockstep_checkpoint_capture())
            return 69;
        xport_lockstep_checkpoint_committed_break();
        if (!xport_lockstep_frame_resume(xport_lockstep_frame_next()))
            return 69;
    }
    if (wait.race)
    {
        if (menu_checkpoint.phase == RRJ_RACE_PENDING_IRQ || menu_checkpoint.phase == RRJ_RACE_PENDING_FRAME)
        {
            uint32 player = menu_checkpoint.objects.player_index;
            (void)rrj_resume_sub_80035958_wait(&menu_checkpoint.objects, menu_checkpoint.race_stage);
            (void)rrj_resume_sub_80011C4C(player);
            (void)rrj_resume_after_race_frame();
        }
        else
            (void)sub_80012370();
        psx_root_counter_bind_clock(NULL, NULL);
        (void)psx_vblank_bind_guest(0, 0, NULL, NULL);
        psx_vblank_bind(NULL, NULL, 0);
        active_wait = NULL;
        xport_lockstep_frame_shutdown();
        return 68;
    }
    while (!xport_isquit() && r_u8(r_u32(0x8005B2F8u)) == 2u)
    {
        if (menu_checkpoint.phase == RRJ_MENU_IDLE)
        {
            if (!rrj_menu_prepare_iteration())
            {
                rrj_menu_finish_iteration();
                continue;
            }
            menu_checkpoint.ready_context = r_u32(0x8005B470u);
            menu_checkpoint.phase = RRJ_MENU_WAITING;
        }
        if (menu_checkpoint.phase == RRJ_MENU_PENDING_IRQ && !psx_wait_events())
            return 69;
        if (menu_checkpoint.phase == RRJ_MENU_AFTER_FRAME && !rrj_lockstep_after_frame())
            return 69;
        while (!r_u8(menu_checkpoint.ready_context + 4u))
        {
            if (!(menu_checkpoint.phase == RRJ_MENU_PENDING_IRQ ? psx_wait_events_resume() : psx_wait_events()))
                return 69;
            if (!rrj_lockstep_after_frame())
                return 69;
        }
        w_u32(0x80088C44u, 0u);
        menu_checkpoint.phase = RRJ_MENU_IDLE;
        rrj_menu_finish_iteration();
    }
    psx_vblank_bind(NULL, NULL, 0u);
    active_wait = NULL;
    xport_lockstep_frame_shutdown();
    RRJ_DEBUG_FPUTS("RRJ lockstep: continuation outside translated menu loop\n", stderr);
    return 68;
}

static uint32 device_test_gpu_callbacks, device_test_mdec_callbacks;
extern int gpu_state_io(FILE *file, int load);
static uint32 *device_test_pixels;
static sint32 device_test_failed;
static uint32 device_test_wait_count;

static sint32 device_test_wait(void *context)
{
    ++device_test_wait_count;
    return context != NULL;
}

static void device_test_gpu_callback(void)
{
    ++device_test_gpu_callbacks;
    if (!psx_callbacks_allowed() || VRAM[0] != 31)
        device_test_failed = 1;
}

static void device_test_mdec_callback(void)
{
    ++device_test_mdec_callbacks;
    if (!psx_callbacks_allowed())
        device_test_failed = 1;
    if (device_test_mdec_callbacks == 1)
    {
        DecDCTout(device_test_pixels + 192, 192);
        if (device_test_pixels[192] != 0xCCCCCCCCu)
            device_test_failed = 1;
    }
}

static sint32 device_test_spu_callbacks(void)
{
    RRJMemory memory = {0};
    SpuNativeTransferGuestBinding binding = {0x8005A3AC, 0x8005A438, 0x8005A434, 0x8005A450, 0x8005A454, 0x8005A470, 0x8005A474, 3};
    uint32 context = 0x800D8000, source = 0x800D1000, masked, index;
    uint8 copied[768];
    uint8 *samples = (uint8 *)xport_guest_ptr(source, sizeof(copied));
    rrj_memory_bind(&memory);
    SpuInit();
    if (!samples || !spu_bind_native_transfer_guest(&binding))
        return 0;
    for (index = 0; index < sizeof(copied); ++index)
        samples[index] = (uint8)(index ^ 0x93);
    w_u32(0x800810BC, context);
    w_u32(0x8005A454, 0x8005FCE8);
    for (masked = 0; masked < 2; ++masked)
    {
        w_u32(context, source);
        w_u32(context + 31556, 0);
        w_u32(context + 31560, 1);
        w_u32(context + 31564, 1);
        w_u32(context + 31568, source);
        w_u32(context + 31572, 123);
        w_u32(context + 31580, 0x3000);
        w_u32(context + 31584, 0x4000);
        if (masked)
            EnterCriticalSectionPSX();
        /* The real handler writes state 2 only after submitting the second DMA */
        if (sub_8005FBDC(source, 123, 0) != sizeof(copied))
            return 0;
        if (masked)
        {
            if (r_u32(context + 31560) != 1 || r_u32(context + 31556) || VSync(-1) != 0)
                return 0;
            ExitCriticalSection();
        }
        if (r_u32(context + 31560) || r_u32(context + 31564) || r_u32(context + 31556) != 1 || r_u32(context) != source + 768)
            return 0;
        if (!spu_download(0x3000, copied, sizeof(copied)) || memcmp(samples, copied, sizeof(copied)) || !spu_download(0x4000, copied, sizeof(copied)) || memcmp(samples, copied, sizeof(copied)) || device_test_wait_count)
            return 0;
    }
    RRJ_DEBUG_PUTS("COOPERATIVE_SPU_CHAINED_CALLBACKS_OK");
    return 1;
}

static sint32 rrj_lockstep_device_selftest(void)
{
    uint32 input[513], index;
    uint16 *codes = (uint16 *)(input + 1);
    uint32 *packet = (uint32 *)xport_guest_ptr(0x80001000, 16);
    FILE *checkpoint = tmpfile();
    FILE *mdec_checkpoint = tmpfile();
    FILE *mdec_active_checkpoint = tmpfile();
    sint32 ok = 0;
    device_test_pixels = (uint32 *)xport_guest_ptr(0x80002000, 384 * sizeof(uint32));
    if (!packet || !checkpoint || !mdec_checkpoint || !mdec_active_checkpoint || !device_test_pixels)
        return 0;
    psx_vblank_bind(device_test_wait, &input, 0);
    gpu_init_empty();
    DrawSyncCallback(device_test_gpu_callback);
    packet[0] = 0x03FFFFFF;
    packet[1] = 0x600000FF;
    packet[2] = 0;
    packet[3] = 0x00010001;
    DrawOTag(packet);
    if (device_test_gpu_callbacks != 1 || device_test_wait_count)
        goto done;
    device_test_gpu_callbacks = 0;
    EnterCriticalSectionPSX();
    DrawOTag(packet);
    if (VRAM[0] != 31 || device_test_gpu_callbacks || !gpu_state_io(checkpoint, 0))
        goto done;
    input[0] = 0x30000200;
    for (index = 0; index < 1024; ++index)
        codes[index] = (index < 12 || (index >= 600 && index < 612)) && !(index & 1) ? 0 : 0xFE00;
    DecDCTReset(0);
    DecDCToutCallback(device_test_mdec_callback);
    memset(device_test_pixels, 0xCC, 384 * sizeof(uint32));
    DecDCTout(device_test_pixels, 192);
    if (DecDCToutSync(1) != 1 || !mdec_psyq_checkpoint_io(mdec_active_checkpoint, 0))
        goto done;
    DecDCTReset(0);
    if (fseek(mdec_active_checkpoint, 0, SEEK_SET) || !mdec_psyq_checkpoint_io(mdec_active_checkpoint, 1) || DecDCToutSync(1) != 1)
        goto done;
    DecDCTin(input, 1);
    memset(input + 1, 0xFF, sizeof(input) - 4);
    for (index = 0; index < 192; ++index)
        if (device_test_pixels[index] != 0x80808080)
            goto done;
    if (DecDCToutSync(1) || device_test_mdec_callbacks || VSync(-1) != 0 || device_test_gpu_callbacks || device_test_mdec_callbacks)
        goto done;
    if (!mdec_psyq_checkpoint_io(mdec_checkpoint, 0))
        goto done;
    ExitCriticalSection();
    if (device_test_mdec_callbacks != 2 || device_test_gpu_callbacks != 1 || device_test_failed || device_test_wait_count)
        goto done;
    for (index = 0; index < 384; ++index)
        if (device_test_pixels[index] != 0x80808080)
            goto done;
    DecDCTReset(0);
    memset(device_test_pixels + 192, 0xCC, 192 * sizeof(uint32));
    device_test_mdec_callbacks = 0;
    if (fseek(mdec_checkpoint, 0, SEEK_SET) || !mdec_psyq_checkpoint_io(mdec_checkpoint, 1) || device_test_mdec_callbacks)
        goto done;
    psx_sync_callbacks();
    if (device_test_mdec_callbacks != 2 || device_test_failed)
        goto done;
    for (index = 0; index < 384; ++index)
        if (device_test_pixels[index] != 0x80808080)
            goto done;
    if (fseek(checkpoint, 0, SEEK_SET) || !gpu_state_io(checkpoint, 1))
        goto done;
    psx_sync_callbacks();
    if (device_test_gpu_callbacks != 2 || device_test_failed)
        goto done;
    DecDCTReset(0);
    device_test_mdec_callbacks = 0;
    input[0] = 0x30000200;
    for (index = 0; index < 1024; ++index)
        codes[index] = (index < 12 || (index >= 600 && index < 612)) && !(index & 1) ? 0 : 0xFE00;
    memset(device_test_pixels, 0xCC, 384 * sizeof(uint32));
    DecDCTin(input, 1);
    DecDCTout(device_test_pixels, 192);
    if (device_test_mdec_callbacks != 2 || device_test_failed || device_test_wait_count || VSync(-1) != 0)
        goto done;
    if (!device_test_spu_callbacks())
        goto done;
    RRJ_DEBUG_PUTS("COOPERATIVE_GPU_MDEC_DATA_CALLBACKS_OK");
    ok = 1;
done:
    DecDCToutCallback(NULL);
    DrawSyncCallback(NULL);
    ExitCriticalSection();
    psx_vblank_bind(NULL, NULL, 0);
    fclose(checkpoint);
    fclose(mdec_checkpoint);
    fclose(mdec_active_checkpoint);
    return ok;
}

static sint32 rrj_checkpoint_spu_transfer_test(void)
{
    uint8 *source = (uint8 *)xport_guest_ptr(0x800D1200, 64);
    uint8 copied[64];
    uint32 index;
    if (!source)
        return 0;
    for (index = 0; index < 64; ++index)
        source[index] = (uint8)(index ^ 0xB3);
    if (SpuSetTransferMode(0) != 0 || SpuSetTransferStartAddr(0x2000) != 0x2000 || SpuWrite(source, 64) != 64 || !SpuIsTransferCompleted(0) || !spu_download(0x2000, copied, sizeof(copied)) || memcmp(source, copied, sizeof(copied)))
        return 0;
    if (RRJ_DEBUG_GETENV("RRJ_LOCKSTEP_GUEST_SPU_TEST") && (r_u16(0x800D1108) != 0x400 || r_u32(0x800D110C) != 1 || r_u32(0x800D1118) != 1))
        return 0;
    return 1;
}

static sint32 cd_command_test_wait(void *context)
{
    device_clock = cd_drive[1];
    return context && rrj_lockstep_cd_sector_event();
}

static sint32 rrj_lockstep_cd_command_selftest(void)
{
    const char *disc = RRJ_DEBUG_GETENV("XPORT_DISC_IMAGE");
    uint32 buffer[585];
    uint8 mode = 0xA0, response;
    CdlLOC location;
    uint64 deadline;
    sint32 ok = 0;
    if (!disc || !cd_mount_cue(disc))
        return 0;
    device_clock = 200;
    memset(cd_drive, 0, sizeof(cd_drive));
    cd_drive[5] = 2;
    cd_bind_command_native(rrj_lockstep_cd_command);
    psx_vblank_bind(cd_command_test_wait, buffer, 0);
    if (!CdControl(CdlSetmode, &mode, &response) || cd_drive[4] != 0xA0 || cd_drive[2] != 225792)
        goto done;
    CdIntToPos(16, &location);
    if (!CdControl(2, (uint8 *)&location, &response) || !cd_location_pending || cd_location_sector != 16 || !CdControl(6, NULL, &response))
        goto done;
    deadline = cd_drive[1];
    if (!cd_drive[0] || cd_drive[3] != 16 || deadline != device_clock + 225792 || !CdControl(6, NULL, &response) || cd_drive[1] != deadline)
        goto done;
    if (CdGetSector(buffer, 1) || rrj_lockstep_cd_sector_event() || !psx_wait_events())
        goto done;
    if (!CdGetSector(buffer, 585) || memcmp((uint8 *)buffer + 13, "CD001", 5) || CdGetSector(buffer, 1) || cd_drive[3] != 17)
        goto done;
    if (!CdControl(CdlPause, NULL, &response) || cd_drive[0] || cd_ready_pending)
        goto done;
    mode = 0x20;
    if (!CdControl(CdlSetmode, &mode, &response) || cd_drive[2] != 451584 || !CdControl(6, NULL, &response) || !cd_drive[0])
        goto done;
    if (!CdControl(CdlStop, NULL, &response) || cd_drive[0])
        goto done;
    RRJ_DEBUG_PUTS("COOPERATIVE_CD_COMMAND_QUEUE_OK");
    ok = 1;
done:
    cd_bind_command_native(NULL);
    psx_vblank_bind(NULL, NULL, 0);
    cd_unmount_image();
    return ok;
}

static sint32 rrj_lockstep_spu_handoff_selftest(void)
{
    const char *path = RRJ_DEBUG_GETENV("XPORT_LOCKSTEP_HANDOFF_SPU_STATE");
    const char *ram = RRJ_DEBUG_GETENV("XPORT_LOCKSTEP_INITIAL_SPU_RAM");
    uint32 words[51 + 24 * 54], index, value, active = 0;
    sint32 first[256] = {0}, second[256] = {0};
    size_t size = spu_lockstep_checkpoint_size();
    void *saved = malloc(size);
    FILE *file = path ? xport_fopen(path, "rb") : NULL;
    sint32 ok = 0;
    if (!saved || !file)
        goto done;
    if (fread(words, sizeof(words), 1, file) != 1)
        goto done;
    SpuInit();
    if (!ram || !spu_load_ram_file(ram) || !spu_load_handoff_file(path))
        goto done;
    for (index = 0; index < 24; ++index)
    {
        uint32 offset = 51 + index * 54;
        if (spu_voice_phase((sint32)index) != (SPU_ADSR_PHASE)words[offset + 14] || spu_voice_envelope((sint32)index) != words[offset + 6])
            goto done;
        if (words[offset + 14])
            ++active;
        if (!spu_mmio_read(NULL, 0x1F801C00 + index * 16 + 4, 2, &value) || value != words[offset + 2])
            goto done;
    }
    if (!active || !spu_lockstep_checkpoint_capture(saved, size))
        goto done;
    spu_mix(first, 128);
    if (!spu_lockstep_checkpoint_restore(saved, size))
        goto done;
    spu_mix(second, 128);
    if (memcmp(first, second, sizeof(first)))
        goto done;
    for (index = 0; index < 256 && !first[index]; ++index)
    {
    }
    if (index == 256)
        goto done;
    RRJ_DEBUG_PRINTF("COOPERATIVE_SPU_HANDOFF_OK active=%u\n", active);
    ok = 1;
done:
    if (file)
        fclose(file);
    free(saved);
    return ok;
}

static uint32 stream_test_complete, stream_test_limit;

static void stream_test_completed(void)
{
    ++stream_test_complete;
}

static void stream_test_limited(void)
{
    ++stream_test_limit;
}

static sint32 rrj_lockstep_stream_callback_selftest(void)
{
    uint32 *ring = (uint32 *)xport_guest_ptr(0x80010000, 8192);
    uint8 sector[2048] = {0};
    sint32 ok = 0;
    if (!ring)
        return 0;
    psx_vblank_bind(device_test_wait, ring, 0);
    StSetRing(ring, 4);
    StSetStream(0, 0, 2, stream_test_completed, stream_test_limited);
    sector[0] = 0x60;
    sector[1] = 1;
    sector[6] = 2;
    sector[8] = 1;
    if (stream_feed_sector(sector, NULL) != 1)
        goto done;
    sector[4] = 1;
    if (stream_feed_sector(sector, NULL) != 1 || StNextStatus(NULL, NULL) != StCOMPLETE || stream_test_complete != 1)
        goto done;
    sector[4] = 0;
    sector[8] = 2;
    if (stream_feed_sector(sector, NULL) != 0 || stream_test_limit != 1 || VSync(-1) != 0 || stream_test_complete != 1)
        goto done;
    if (device_test_wait_count)
        goto done;
    StUnSetRing();
    stream_test_complete = stream_test_limit = 0;
    StSetRing(ring, 4);
    StSetStream(0, 0, 2, stream_test_completed, stream_test_limited);
    EnterCriticalSectionPSX();
    sector[4] = 0;
    sector[8] = 1;
    if (stream_feed_sector(sector, NULL) != 1)
        goto done;
    sector[4] = 1;
    if (stream_feed_sector(sector, NULL) != 1 || stream_test_complete)
        goto done;
    sector[4] = 0;
    sector[8] = 2;
    if (stream_feed_sector(sector, NULL) != 0 || stream_test_limit || VSync(-1) != 0)
        goto done;
    ExitCriticalSection();
    if (stream_test_complete != 1 || stream_test_limit != 1 || device_test_wait_count)
        goto done;
    RRJ_DEBUG_PUTS("COOPERATIVE_STREAM_CALLBACKS_OK");
    ok = 1;
done:
    StUnSetRing();
    ExitCriticalSection();
    psx_vblank_bind(NULL, NULL, 0);
    return ok;
}

int rrj_lockstep_selftest(void)
{
    if (!xport_lockstep_dram || !xport_lockstep_scratchpad)
        return 70;
    if (RRJ_DEBUG_GETENV("XPORT_LOCKSTEP_GPU_CHECKPOINT_TEST"))
        return gpu_lockstep_portable_selftest() ? 0 : 79;
    if (RRJ_DEBUG_GETENV("XPORT_LOCKSTEP_CD_CHECKPOINT_TEST"))
        return spu_lockstep_portable_selftest() ? 0 : 80;
    if (RRJ_DEBUG_GETENV("XPORT_LOCKSTEP_DEVICE_CALLBACK_TEST"))
        return rrj_lockstep_device_selftest() ? 0 : 82;
    if (RRJ_DEBUG_GETENV("XPORT_LOCKSTEP_CD_COMMAND_TEST"))
        return rrj_lockstep_cd_command_selftest() ? 0 : 85;
    if (RRJ_DEBUG_GETENV("XPORT_LOCKSTEP_SPU_HANDOFF_TEST"))
        return rrj_lockstep_spu_handoff_selftest() ? 0 : 87;
    if (RRJ_DEBUG_GETENV("XPORT_LOCKSTEP_STREAM_CALLBACK_TEST"))
        return rrj_lockstep_stream_callback_selftest() ? 0 : 88;
    if (RRJ_DEBUG_GETENV("RRJ_LOCKSTEP_CHECKPOINT_TEST"))
    {
        RRJMemory memory = {0};
        RRJLockstepWait wait = {&memory, 0};
        const char *export_path = RRJ_DEBUG_GETENV("RRJ_LOCKSTEP_CHECKPOINT_EXPORT");
        const char *import_path = RRJ_DEBUG_GETENV("RRJ_LOCKSTEP_CHECKPOINT_IMPORT");
        uint32 next_frame;
        uint8 samples[16] = {0x0Cu}, restored[16];
        rrj_memory_bind(&memory);
        active_wait = &wait;
        if (RRJ_DEBUG_GETENV("XPORT_DISC_IMAGE") && !cd_mount_cue(RRJ_DEBUG_GETENV("XPORT_DISC_IMAGE")))
            return 86;
        if (!xport_lockstep_frame_init("SLUS_010.53"))
            return 72;
        if (import_path)
        {
            if (!xport_lockstep_checkpoint_load_portable(import_path, &next_frame) || next_frame != 17u || !xport_lockstep_frame_resume(next_frame) || menu_checkpoint.phase != RRJ_MENU_AFTER_FRAME || menu_checkpoint.ready_context != 0x800D1000u || memory.cpu_status != 0x401u || memory.menu_stack_pointer != 0x801FFF90u || memory.root_counters[47] != 0xA5u || r_u8(0x800D0000u) != 2u || !spu_download(0x100u, restored, sizeof(restored)) || memcmp(samples, restored, sizeof(samples)))
                return 73;
            RRJ_DEBUG_PUTS("RRJ_PROJECT_CHECKPOINT_IMPORT_OK");
            return rrj_checkpoint_spu_transfer_test() ? 0 : 83;
        }
        if (!export_path || !xport_lockstep_frame_begin_at(17u))
            return 74;
        w_u32(0x8005B2F8u, 0x800D0000u);
        w_u8(0x800D0000u, 2u);
        w_u32(0x120u, 0x800D2000u);
        w_u32(0x124u, 28u);
        memory.cpu_status = 0x401u;
        memory.menu_stack_pointer = 0x801FFF90u;
        memory.root_counters[47] = 0xA5u;
        menu_checkpoint.phase = RRJ_MENU_AFTER_FRAME;
        menu_checkpoint.ready_context = 0x800D1000u;
        SpuInit();
        if (RRJ_DEBUG_GETENV("RRJ_LOCKSTEP_GUEST_SPU_TEST"))
        {
            SpuNativeTransferGuestBinding binding = {0x800D1100, 0x800D1104, 0x800D1108, 0x800D110C, 0x800D1110, 0x800D1114, 0x800D1118, 3};
            w_u32(0x800D110C, 1);
            if (!spu_bind_native_transfer_guest(&binding))
                return 84;
        }
        if (!spu_upload(0x100u, samples, sizeof(samples)) || !xport_lockstep_checkpoint_save_portable(export_path))
            return 75;
        if (RRJ_DEBUG_GETENV("RRJ_LOCKSTEP_CHECKPOINT_JUMP"))
        {
            if (setjmp(xport_lockstep_checkpoint_context) == 0)
            {
                uint8 changed[16] = {0};
                if (!xport_lockstep_checkpoint_capture())
                    return 76;
                memory.cpu_status = 0u;
                memory.root_counters[47] = 0u;
                menu_checkpoint.phase = RRJ_MENU_IDLE;
                menu_checkpoint.ready_context = 0u;
                w_u8(0x800D0000u, 0u);
                if (!spu_upload(0x100u, changed, sizeof(changed)))
                    return 77;
                xport_lockstep_checkpoint_jump();
            }
            memset(&memory, 0, sizeof(memory));
            rrj_lockstep_restore_memory(&memory);
            rrj_memory_bind(&memory);
            if (menu_checkpoint.phase != RRJ_MENU_AFTER_FRAME || menu_checkpoint.ready_context != 0x800D1000u || memory.cpu_status != 0x401u || memory.menu_stack_pointer != 0x801FFF90u || memory.root_counters[47] != 0xA5u || r_u8(0x800D0000u) != 2u || !spu_download(0x100u, restored, sizeof(restored)) || memcmp(samples, restored, sizeof(samples)))
                return 78;
            RRJ_DEBUG_PUTS("RRJ_PROJECT_CHECKPOINT_JUMP_OK");
        }
        RRJ_DEBUG_PUTS("RRJ_PROJECT_CHECKPOINT_EXPORT_OK");
        return 0;
    }
    if (sub_80010028(7u, 16u) != 0x7000u || !xport_lockstep_last_hit || xport_lockstep_last_hit->address != 0x80010028u || xport_lockstep_last_hit->argument_count != 2u)
        return 71;
    RRJ_DEBUG_PUTS("RRJ_LOCKSTEP_BINDING_OK");
    return 0;
}
#endif
