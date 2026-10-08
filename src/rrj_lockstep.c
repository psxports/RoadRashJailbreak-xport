#include "rrj_lockstep.h"
#if defined(LOCKSTEP_DEBUG)
#include "xport_trace.h"
#include "psx_spu.h"
#include "psx_gpu.h"
#include "race_pause.h"
#include "spu.h"
#include "race_audio_frontier.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef struct RRJLockstepWait
{
    RRJMemory *memory;
    RRJVBlankCall service;
} RRJLockstepWait;

static RRJLockstepWait *active_wait;

enum { RRJ_MENU_IDLE, RRJ_MENU_WAITING, RRJ_MENU_AFTER_FRAME };
typedef struct RRJMenuCheckpoint
{
    uint32 magic, version, phase, ready_context, cpu_status;
    uint8 root_counters[48];
} RRJMenuCheckpoint;
static RRJMenuCheckpoint menu_checkpoint;

static void rrj_lockstep_restore_memory(RRJMemory *memory)
{
    memory->cpu_status = menu_checkpoint.cpu_status;
    memcpy(memory->root_counters, menu_checkpoint.root_counters, sizeof(memory->root_counters));
}

sint32 xport_game_checkpoint_io(FILE *file, sint32 load)
{
    RRJMenuCheckpoint state;
    RRJMemory *memory = active_wait ? active_wait->memory : NULL;
    if (!file || !memory || memory->bios || memory->native_dispatch || memory->checkpoint_abi_valid)
        return 0;
    if (load)
    {
        if (fread(&state, sizeof(state), 1, file) != 1 || state.magic != 0x4A52524Du || state.version != 1u ||
            state.phase != RRJ_MENU_AFTER_FRAME ||
            (state.ready_context & 0x1FFFFFFFu) > PSX_DRAM_SIZE - 5u ||
            r_u8(rrj_read32(memory, 0x8005B2F8u)) != 2u)
            return 0;
        menu_checkpoint = state;
        rrj_lockstep_restore_memory(memory);
    }
    else
    {
        if (menu_checkpoint.phase != RRJ_MENU_AFTER_FRAME || r_u8(rrj_read32(memory, 0x8005B2F8u)) != 2u)
            return 0;
        menu_checkpoint.magic = 0x4A52524Du;
        menu_checkpoint.version = 1u;
        menu_checkpoint.cpu_status = memory->cpu_status;
        memcpy(menu_checkpoint.root_counters, memory->root_counters, sizeof(menu_checkpoint.root_counters));
        if (fwrite(&menu_checkpoint, sizeof(menu_checkpoint), 1, file) != 1)
            return 0;
    }
    return rrj_game_checkpoint_io(file, load) && rrj_spu_checkpoint_io(file, load) && rrj_audio_checkpoint_io(file, load);
}

static sint32 rrj_lockstep_wait(void *context)
{
    RRJLockstepWait *wait = (RRJLockstepWait *)context;
    /* Other nested wait continuations have not been migrated */
    if (wait != active_wait || menu_checkpoint.phase != RRJ_MENU_WAITING)
        return 0;
    psx_vblank_signal();
    if (psx_vblank_failed() || !xport_lockstep_frame_done())
        return 0;
    menu_checkpoint.phase = RRJ_MENU_AFTER_FRAME;
    return 1;
}

static sint32 rrj_lockstep_after_frame(void)
{
    RRJLockstepWait *wait = active_wait;
    if (!wait || menu_checkpoint.phase != RRJ_MENU_AFTER_FRAME)
        return 0;
    (void)sub_F_80064C30(wait->memory, wait->service);
    menu_checkpoint.phase = RRJ_MENU_WAITING;
    return !psx_vblank_failed();
}

uint32_t rrj_lockstep_poll(RRJMemory *memory)
{
    if (!active_wait || active_wait->memory != memory || !rrj_lockstep_wait(active_wait) || !rrj_lockstep_after_frame())
    {
        fputs("RRJ lockstep: VBlank wait failed\n", stderr);
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
        text = getenv(name);
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

int rrj_lockstep_run(RRJMemory *memory, RRJLoopCall loop, RRJVBlankCall service)
{
    const char *continuation = getenv("XPORT_LOCKSTEP_CONTINUATION");
    const char *frame_text = getenv("XPORT_LOCKSTEP_START_FRAME");
    const char *spu_path = getenv("XPORT_LOCKSTEP_INITIAL_SPU_RAM");
    const char *load_path = getenv("XPORT_LOCKSTEP_PORTABLE_LOAD_PATH");
    char *end;
    unsigned long frame;
    RRJLockstepWait wait = {memory, service};
    active_wait = &wait;
    if (!continuation || strcmp(continuation, "rrj_menu_iteration") || !frame_text || !loop || !service)
    {
        fputs("RRJ lockstep: unsupported or missing native continuation\n", stderr);
        return 64;
    }
    frame = strtoul(frame_text, &end, 10);
    if (!*frame_text || *end || frame == 0u || !xport_lockstep_frame_init("SLUS_010.53"))
        return 65;
    if (!xport_lockstep_frame_begin_at((uint32)frame))
    {
        fprintf(stderr, "RRJ lockstep: %s\n", xport_lockstep_frame_error());
        xport_lockstep_frame_shutdown();
        return 65;
    }
    if (!(load_path && *load_path) && !rrj_lockstep_import_gte())
    {
        fputs("RRJ lockstep: missing or invalid GTE handoff state\n", stderr);
        return 66;
    }
    /* Bind existing guest pad buffers without clearing seeded state */
    {
        PsxPadReplayBinding pads = {{0x800D70E0u, 0x800D7104u}, 1u, {0, 0}};
        if (!pad_bind_replay(&pads))
            return 66;
    }
    if (load_path && *load_path)
    {
        uint32 next_frame;
        if (!xport_lockstep_checkpoint_load_portable(load_path, &next_frame) || !xport_lockstep_frame_resume(next_frame))
            return 69;
    }
    else
    {
        SpuInit();
        if (!spu_path || !spu_load_ram_file(spu_path))
            return 67;
        memset(&menu_checkpoint, 0, sizeof(menu_checkpoint));
    }
    /* TODO Import complete SPU voice and device state at the handoff */
    psx_vblank_bind(rrj_lockstep_wait, &wait, rrj_read32(memory, 0x8005B46Cu));
    /* Keep the jump target in this live frame rather than a returned callback */
    if (setjmp(xport_lockstep_checkpoint_context))
    {
        rrj_lockstep_restore_memory(memory);
        xport_lockstep_checkpoint_resumed();
    }
    while (!xport_isquit() && r_u8(rrj_read32(memory, 0x8005B2F8u)) == 2u)
    {
        if (menu_checkpoint.phase == RRJ_MENU_IDLE)
        {
            if (!rrj_menu_prepare_iteration(memory, loop))
            {
                rrj_menu_finish_iteration(memory, loop);
                continue;
            }
            menu_checkpoint.ready_context = rrj_read32(memory, 0x8005B470u);
            menu_checkpoint.phase = RRJ_MENU_WAITING;
        }
        if (menu_checkpoint.phase == RRJ_MENU_AFTER_FRAME && !rrj_lockstep_after_frame())
            return 69;
        while (!r_u8(menu_checkpoint.ready_context + 4u))
        {
            if (!rrj_lockstep_wait(&wait))
                return 69;
            if (xport_lockstep_mode == LOCKSTEP_MODE_PREFIX_GUARD)
            {
                menu_checkpoint.cpu_status = memory->cpu_status;
                memcpy(menu_checkpoint.root_counters, memory->root_counters, sizeof(menu_checkpoint.root_counters));
                if (!xport_lockstep_checkpoint_capture())
                    return 69;
            }
            if (!rrj_lockstep_after_frame())
                return 69;
        }
        rrj_write32(memory, 0x80088C44u, 0u);
        menu_checkpoint.phase = RRJ_MENU_IDLE;
        rrj_menu_finish_iteration(memory, loop);
    }
    psx_vblank_bind(NULL, NULL, 0u);
    active_wait = NULL;
    xport_lockstep_frame_shutdown();
    fputs("RRJ lockstep: continuation outside translated menu loop\n", stderr);
    return 68;
}

int rrj_lockstep_selftest(void)
{
    if (!xport_lockstep_dram || !xport_lockstep_scratchpad)
        return 70;
    if (getenv("XPORT_LOCKSTEP_GPU_CHECKPOINT_TEST"))
        return gpu_lockstep_portable_selftest() ? 0 : 79;
    if (getenv("XPORT_LOCKSTEP_CD_CHECKPOINT_TEST"))
        return spu_lockstep_portable_selftest() ? 0 : 80;
    if (getenv("RRJ_LOCKSTEP_CHECKPOINT_TEST"))
    {
        RRJMemory memory = {0};
        RRJLockstepWait wait = {&memory, NULL};
        const char *export_path = getenv("RRJ_LOCKSTEP_CHECKPOINT_EXPORT");
        const char *import_path = getenv("RRJ_LOCKSTEP_CHECKPOINT_IMPORT");
        uint32 next_frame;
        uint8 samples[16] = {0x0Cu}, restored[16];
        rrj_memory_bind(&memory);
        active_wait = &wait;
        if (!xport_lockstep_frame_init("SLUS_010.53"))
            return 72;
        if (import_path)
        {
            if (!xport_lockstep_checkpoint_load_portable(import_path, &next_frame) || next_frame != 17u ||
                !xport_lockstep_frame_resume(next_frame) || menu_checkpoint.phase != RRJ_MENU_AFTER_FRAME ||
                menu_checkpoint.ready_context != 0x800D1000u || memory.cpu_status != 0x401u ||
                memory.root_counters[47] != 0xA5u || r_u8(0x800D0000u) != 2u ||
                !spu_download(0x100u, restored, sizeof(restored)) || memcmp(samples, restored, sizeof(samples)))
                return 73;
            puts("RRJ_PROJECT_CHECKPOINT_IMPORT_OK");
            return 0;
        }
        if (!export_path || !xport_lockstep_frame_begin_at(17u))
            return 74;
        rrj_write32(&memory, 0x8005B2F8u, 0x800D0000u);
        w_u8(0x800D0000u, 2u);
        memory.cpu_status = 0x401u;
        memory.root_counters[47] = 0xA5u;
        menu_checkpoint.phase = RRJ_MENU_AFTER_FRAME;
        menu_checkpoint.ready_context = 0x800D1000u;
        SpuInit();
        if (!spu_upload(0x100u, samples, sizeof(samples)) ||
            !xport_lockstep_checkpoint_save_portable(export_path))
            return 75;
        if (getenv("RRJ_LOCKSTEP_CHECKPOINT_JUMP"))
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
            if (menu_checkpoint.phase != RRJ_MENU_AFTER_FRAME || menu_checkpoint.ready_context != 0x800D1000u ||
                memory.cpu_status != 0x401u || memory.root_counters[47] != 0xA5u || r_u8(0x800D0000u) != 2u ||
                !spu_download(0x100u, restored, sizeof(restored)) || memcmp(samples, restored, sizeof(samples)))
                return 78;
            puts("RRJ_PROJECT_CHECKPOINT_JUMP_OK");
        }
        puts("RRJ_PROJECT_CHECKPOINT_EXPORT_OK");
        return 0;
    }
    if (sub_80010028(7u, 16u) != 0x7000u ||
        !xport_lockstep_last_hit || xport_lockstep_last_hit->address != 0x80010028u ||
        xport_lockstep_last_hit->argument_count != 2u)
        return 71;
    puts("RRJ_LOCKSTEP_BINDING_OK");
    return 0;
}
#endif
