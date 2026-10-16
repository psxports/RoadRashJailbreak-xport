#include "rrj_debug.h"
#include "game_main.h"
#if defined(_WIN32)
    #include <windows.h>
#endif
#include "native_file_callbacks.h"
/* Native bootstrap and differential-probe entry point. No MIPS interpreter.
 * Default launch enters the translated State1 menu; probes are explicit modes.
 */
#include "fixed_math.h"
#include "draft_quick.h"
#include "native_game_api.h"
#include "first2_runtime_dependencies.h"
#include "first2_camera_dependencies.h"
#include "first2_camera_surface.h"
#include "first2_render_dependencies.h"
#include "solo_resource_dependencies.h"
#include "solo_file_dependencies.h"
#include "solo_resource_complete.h"
#include "flare.h"
#include "packet.h"
#include "platform_smoke.h"
#include "psx.h"
#include "psx_gpu.h"
#include "psx_spu.h"
#include "psx_press.h"
#include "xport_trace.h"
#include "menu.h"
#include "gpu.h"
#include "menu_navigation.h"
#include "audio.h"
#include "menu_input.h"
#include "submenu.h"
#include "menu_flags.h"
#include "menu_reset.h"
#include "race_step.h"
#include "race_global.h"
#include "race_services.h"
#include "race_frame.h"
#include "race_pause.h"
#include "race_pause_frontier.h"
#include "race_leaf.h"
#include "race_bodyless_batch_002.h"
#include "race_bodyless_batch_003.h"
#include "race_bodyless_batch_004.h"
#include "race_bodyless_batch_007.h"
#include "race_bodyless_batch_008.h"
#include "race_leaf_frontier.h"
#include "race_leaf_batch_000.h"
#include "race_leaf_batch_001.h"
#include "race_leaf_batch_002.h"
#include "race_leaf_batch_003.h"
#include "race_leaf_batch_004.h"
#include "race_leaf_batch_005.h"
#include "race_leaf_batch_006.h"
#include "race_global_800B8020.h"
#include "race_global_8009E8A4.h"
#include "race_global_8009D004.h"
#include "race_global_8008C45C.h"
#include "race_audio_frontier.h"
#include "race_trace_runtime.h"
#include "menu_draw.h"
#include "menu_sprite.h"
#include "resource_lookup.h"
#include "resource.h"
#include "font.h"
#include "menu_item.h"
#include "menu_hint.h"
#include "entry_kind.h"
#include "resource_record.h"
#include "menu_center.h"
#include "menu_info.h"
#include "word_wrap.h"
#include "text_id.h"
#include "info_panel.h"
#include "menu_video.h"
#include "menu_render.h"
#include "resource_select.h"
#include "resource_groups.h"
#include "resource_value.h"
#include "resource_next.h"
#include "resource_apply.h"
#include "menu_first.h"
#include "resource_refresh.h"
#include "menu_update.h"
#include "menu_frame.h"
#include "menu_transition.h"
#include "pad.h"
#include "pad_service.h"
#include "menu_latch.h"
#include "input.h"
#include "vblank.h"
#include "audio_flush.h"
#include "spu.h"
#include "menu_cleanup.h"
#include "menu_loop.h"
#include "music_stop.h"
#include "music_start.h"
#include "attract.h"
#include "menu_phase.h"
#include "video_stop.h"
#include "screen.h"
#include "screen_env.h"
#include "video_phase.h"
#include "video_tick.h"
#include "video_preview.h"
#include "race_leaf_batch_007.h"
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wip.h"
#include "rrj_lockstep.h"
#define abort() rrj_wip_stop(__func__, __FILE__, __LINE__)

const uint32 xport_gpu_graph_type_address = 0x80055F0Cu;

/* Explicit test seam: record SDK calls, no GPU side effects. Enabled only when
 * a boundary-log file is provided. The ordinary bootstrap does not install it. */
/* Test-only selected SDK returns; enabled solely by the DDC4 probe. */
static uint32_t pad_sdk_returns[2];

static uint32_t trace_race_call(RRJMemory *m, uint32_t target, const uint32_t args[8]);

static uint32_t probe_pad_sdk(RRJMemory *m, uint32_t fn, uint32_t a0, uint32_t a1, uint32_t a2)
{
    if (!m->sdk_user)
        abort();
    RRJ_DEBUG_FPRINTF((FILE *)m->sdk_user, "%08X %08X %08X %08X\n", fn, a0, a1, a2);
    return fn == 0x80040550 ? pad_sdk_returns[0] : pad_sdk_returns[1];
}

static uint32_t screen_probe_mode, screen_probe_env_index;

static uint32_t submenu_reply;
static uint32_t race_step_reply;
static uint32_t slot_find_count, object_dispatch_composed;
static uint32_t probe_reverb(RRJMemory *, uint32_t, uint32_t);
static uint32_t record_image(const uint8_t rect[8], uint32_t pixels);
static uint32_t probe_dispatch_critical(RRJMemory *, uint32_t, uint32_t, uint32_t, uint32_t);

static uint32_t object_dispatch_args[4];

static uint32_t probe_dispatch_critical(RRJMemory *m, uint32_t fn, uint32_t a0, uint32_t a1, uint32_t a2)
{
    if (!m->sdk_user)
        abort();
    RRJ_DEBUG_FPRINTF((FILE *)m->sdk_user, "%08X 00000000 00000000 00000000 00000000 00000000 00000000\n", fn);
    return 0;
}

static uint32_t cleanup_mode, cleanup_reply;

static uint32_t probe_object_cleanup(RRJMemory *m, uint32_t fn, uint32_t a0, uint32_t a1, uint32_t a2)
{
    if (!m->sdk_user)
        abort();
    RRJ_DEBUG_FPRINTF((FILE *)m->sdk_user, "%08X %08X %08X %08X\n", fn, a0, a1, a2);
    if (fn == 0x80043DA4)
    {
        if (cleanup_mode == 1)
            w_u32(0x800541D0, 1);
        if (cleanup_mode == 2)
            w_u32(0x8005ACBC, 0x801E0080);
    }
    return cleanup_reply;
}

static uint32_t queue_args[3], queue_index, queue_composed;

static uint32_t probe_queue_cleanup_body(uint32_t object);

static uint32_t probe_object_queue(RRJMemory *m, uint32_t fn, uint32_t a0, uint32_t a1, uint32_t a2)
{
    uint32_t result = 0;
    if (queue_composed >= 3 && fn == 0x800313EC)
        return probe_queue_cleanup_body(a0);
    if (queue_composed >= 2)
    {
        if (fn == 0x80031B4C)
            return sub_80031B4C(a0);
        if (fn == 0x800319F8)
            return sub_800319F8(a0, a1);
    }
    if (queue_composed)
    {
        if (fn == 0x80031D70)
            return sub_80031D70();
        if (fn == 0x80031BF8)
            return sub_80031BF8(a0, a1);
        if (fn == 0x800320CC)
            return sub_800320CC(a0);
        if (fn == 0x800320AC)
            return sub_800320AC();
    }
    if (!m->sdk_user)
        abort();
    RRJ_DEBUG_FPRINTF((FILE *)m->sdk_user, "%08X %08X %08X %08X\n", fn, a0, a1, a2);
    if (fn == 0x80031D70)
    {
        if (queue_index < queue_args[0])
            result = 0x801E1000 + 64 * queue_index++;
    }
    if (fn == 0x80031BF8)
        result = r_u32(a0 + 12);
    if (fn == 0x800313EC && queue_args[1] == 1)
    {
        w_u32(a0 + 32, 0x87654321);
        w_u32(r_u32(a0 + 20) + 28, 0xDEADBEEF);
    }
    if (fn == 0x80031604 && queue_args[1] == 2)
        w_u32(r_u32(0x8005ACBC) + 0xA58, 0);
    if (fn == 0x80031604 && queue_args[1] == 3)
        w_u32(0x8005ACBC, 0x801E0100);
    if (fn == 0x800320CC && queue_args[1] == 4)
        w_u32(0x8005AED8, 1);
    if (fn == 0x800320CC && queue_args[1] == 5)
        w_u32(0x8005AED8, 0);
    if (fn == 0x800320AC)
        result = queue_args[2];
    return result;
}

static uint32_t probe_counter_enter(void)
{
    return probe_object_cleanup(rrj_host_context(), 0x80043DA4u, 0, 0, 0);
}

static uint32_t probe_counter_leave(void)
{
    return probe_object_cleanup(rrj_host_context(), 0x80043DB4u, 0, 0, 0);
}

static uint32_t probe_counter_dispatch_enter(void)
{
    return probe_dispatch_critical(rrj_host_context(), 0x80043DA4u, 0, 0, 0);
}

static uint32_t probe_counter_dispatch_leave(void)
{
    return probe_dispatch_critical(rrj_host_context(), 0x80043DB4u, 0, 0, 0);
}

static uint32_t probe_counter_queue_enter(void)
{
    return probe_object_queue(rrj_host_context(), 0x80043DA4u, 0, 0, 0);
}

static uint32_t probe_counter_queue_leave(void)
{
    return probe_object_queue(rrj_host_context(), 0x80043DB4u, 0, 0, 0);
}

static const RRJObjectCounterProbe counter_cleanup_probe = {probe_counter_enter, probe_counter_leave};
static const RRJObjectCounterProbe counter_dispatch_probe = {probe_counter_dispatch_enter, probe_counter_dispatch_leave};
static const RRJObjectCounterProbe counter_queue_probe = {probe_counter_queue_enter, probe_counter_queue_leave};

static uint32_t probe_counter_body(uint32_t from, uint32_t to)
{
    const RRJObjectCounterProbe *previous = rrj_object_counter_set_probe(&counter_cleanup_probe);
    uint32_t result = sub_800322D0(from, to);
    (void)rrj_object_counter_set_probe(previous);
    return result;
}

static uint32_t probe_cleanup_body(uint32_t object)
{
    const RRJObjectCounterProbe *previous = rrj_object_counter_set_probe(&counter_cleanup_probe);
    uint32_t result = sub_800313EC(object);
    (void)rrj_object_counter_set_probe(previous);
    return result;
}

static uint32_t probe_queue_cleanup_body(uint32_t object)
{
    const RRJObjectCounterProbe *previous = rrj_object_counter_set_probe(&counter_queue_probe);
    uint32_t result = sub_800313EC(object);
    (void)rrj_object_counter_set_probe(previous);
    return result;
}

static uint32_t probe_dispatch_body(uint32_t object, uint32_t group)
{
    const RRJObjectCounterProbe *previous = rrj_object_counter_set_probe(&counter_dispatch_probe);
    uint32_t result = sub_80031604(object, group);
    (void)rrj_object_counter_set_probe(previous);
    return result;
}

static uint32_t player_update_args[3];

static uint32_t race_pause_replies[2];
static uint32_t race_check_replies[4], race_check_index;

static uint32_t probe_race_player_check(uint32_t player)
{
    RRJMemory *m = rrj_host_context();
    if (!m->sdk_user || race_check_index >= 4)
        abort();
    RRJ_DEBUG_FPRINTF((FILE *)m->sdk_user, "8008B99C %08X 00000000 00000000\n", player);
    return race_check_replies[race_check_index++];
}

static uint32_t race_frame_args[3];

static uint32_t race_service_args[3];

static uint32_t race_global_replies[2];
static uint32_t race_global_composed;

static uint32_t probe_race_composed_service(RRJMemory *m, uint32_t fn, uint32_t a, uint32_t b)
{
    if (!m->sdk_user)
        abort();
    RRJ_DEBUG_FPRINTF((FILE *)m->sdk_user, "%08X %08X %08X %08X\n", fn, a, b, 0u);
    return race_global_replies[1];
}

static uint32_t music_replies[2];

static uint32_t video_phase_replies[3];

static uint32_t probe_video_phase(RRJMemory *m, uint32_t fn, const uint32_t args[9])
{
    unsigned i;
    if (!m->sdk_user)
        abort();
    RRJ_DEBUG_FPRINTF((FILE *)m->sdk_user, "%08X", fn);
    for (i = 0; i < 9; ++i)
        RRJ_DEBUG_FPRINTF((FILE *)m->sdk_user, " %08X", args[i]);
    fputc('\n', (FILE *)m->sdk_user);
    return fn == 0x80022A78 ? video_phase_replies[0] : (fn == 0x8005F36C || fn == 0x8005F484) ? video_phase_replies[2] : 0;
}

static uint32_t vblank_replies[2];

static uint32_t probe_reverb(RRJMemory *m, uint32_t mode, uint32_t mask)
{
    if (!m->sdk_user)
        abort();
    RRJ_DEBUG_FPRINTF((FILE *)m->sdk_user, "80050678 %08X %08X\n", mode, mask);
    return 0x12345678;
}

static uint32_t probe_loop_remaining;

/* Typed LoadImage test boundary. Normalize the pointed-to local RECT by value;
 * neither side compares host/PSX stack addresses or performs a GPU upload. */

/* Explicit unit-test interception of the GAME sound callee. Log the selected
 * halfword before the caller's store; do not install this in live gameplay. */

static void *ram_pointer(uint32_t address, size_t size)
{
    uint32_t physical = address & 0x1fffffff;
    if (physical > sizeof(DRAM) || size > sizeof(DRAM) - physical)
    {
        RRJ_DEBUG_FPRINTF(stderr, "Probe address outside PSX main RAM: %08X\n", address);
        exit(2);
    }
    return psx_addr(address, size);
}

static int read_file(const char *path, void *data, size_t size)
{
    FILE *f = xport_fopen(path, "rb");
    int ok;
    uint8_t trailing;
    if (!f)
        return 0;
    ok = fread(data, 1, size, f) == size && fread(&trailing, 1, 1, f) == 0;
    fclose(f);
    return ok;
}

static int read_checkpoint_block(FILE *stream, void *data, uint32_t expected, uint32_t actual)
{
    return actual == expected && fread(data, 1, expected, stream) == expected;
}

static int skip_checkpoint_block(FILE *stream, uint32_t size)
{
    uint8_t discarded[4096];
    while (size)
    {
        size_t part = size < sizeof discarded ? size : sizeof discarded;
        if (fread(discarded, 1, part, stream) != part)
            return 0;
        size -= (uint32_t)part;
    }
    return 1;
}

static int read_checkpoint_gte(FILE *stream, uint32_t size, uint32_t registers[64], RRJNativeCheckpointABI *abi)
{
    const uint32_t prefix = 1260;
    const uint32_t register_bytes = 64 * sizeof(uint32_t);
    uint8_t cpu_prefix[1260];

    if (size != 6637 || fread(cpu_prefix, 1, prefix, stream) != prefix)
        return 0;
    if (xport_load_le32(cpu_prefix + 84) != 2 || xport_load_le32(cpu_prefix + 88) != 0x800CD898 || xport_load_le32(cpu_prefix + 128) != 0x8005AC8C || xport_load_le32(cpu_prefix + 152) != 0x80012370 || xport_load_le32(cpu_prefix + 156) != 0x80012374 || cpu_prefix[218] || cpu_prefix[219] || cpu_prefix[222] != 34 || cpu_prefix[227] != 34 || memcmp(cpu_prefix + 236, SCRATCHPAD, sizeof(SCRATCHPAD)))
        return 0;
    return rrj_native_checkpoint_decode(abi, cpu_prefix, prefix) && fread(registers, 1, register_bytes, stream) == register_bytes && skip_checkpoint_block(stream, size - prefix - register_bytes);
}

static void restore_checkpoint_gte(const uint32 registers[64])
{
    if (!psx_gte_import_control_registers(registers + 32))
    {
        RRJ_DEBUG_FPRINTF(stderr, "Cannot restore checkpoint GTE control registers\n");
        abort();
    }
}

static int read_file(const char *path, void *data, size_t size);
static int save_live_frame(const char *ram_path);

static int load_race_checkpoint(RRJMemory *m, const char *path, uint32_t *ordinal)
{
    static const char abi[] = "RoadRash race-loop checkpoint ABI 1";
    uint8_t stored_abi[64], trailing;
    uint32_t magic, pc, sizes[5];
    uint32_t gte_registers[64];
    RRJNativeCheckpointABI checkpoint_abi;
    FILE *stream;
    unsigned i;

    m->checkpoint_abi_valid = 0;
    if (!path || !(stream = xport_fopen(path, "rb")))
        return 0;
    if (fread(&magic, 4, 1, stream) != 1 || fread(stored_abi, 1, sizeof stored_abi, stream) != sizeof stored_abi || fread(&pc, 4, 1, stream) != 1 || fread(ordinal, 4, 1, stream) != 1 || fread(sizes, sizeof sizes, 1, stream) != 1 || magic != 0x31504A52 || memcmp(stored_abi, abi, sizeof abi) || pc != 0x80012370 || !read_checkpoint_block(stream, DRAM, sizeof(DRAM), sizes[0]) || !read_checkpoint_block(stream, SCRATCHPAD, sizeof(SCRATCHPAD), sizes[1]) || !read_checkpoint_gte(stream, sizes[2], gte_registers, &checkpoint_abi))
    {
        fclose(stream);
        return 0;
    }
    for (i = 3; i < 5; ++i)
        if (!sizes[i] || sizes[i] > 32u * 1024u * 1024u || !skip_checkpoint_block(stream, sizes[i]))
        {
            fclose(stream);
            return 0;
        }
    if (fread(&trailing, 1, 1, stream) != 0 || fclose(stream))
        return 0;
    restore_checkpoint_gte(gte_registers);
    m->checkpoint_abi = checkpoint_abi;
    m->cpu_status = checkpoint_abi.cpu_status;
    m->checkpoint_abi_valid = 1;
    return 1;
}

static uint32_t live_spu_pending_callback;

static void live_spu_restore(void)
{
    SpuHeapBinding heap = {0};
    SpuNativeTransferGuestBinding transfer = {0};
    uint32_t count = r_u32(0x8005A47Cu);
    uint32_t shift = r_u32(0x8005A444u) & 31u;

    if (count >= 0x40000u)
    {
        RRJ_DEBUG_FPUTS("Native SPU heap capacity exceeds guest RAM\n", stderr);
        abort();
    }
    heap.entries = (uint32_t *)xport_guest_ptr(r_u32(0x8005A484u), (count + 1u) * 8u);
    heap.last_index = (uint32_t *)xport_guest_ptr(0x8005A480u, 4);
    heap.capacity = count + 1u;
    heap.limit = count;
    heap.address_shift = shift;
    heap.alignment_mask = r_u32(0x8005A44Cu);
    heap.reserved_bytes = r_u32(0x8005A3B4u) ? (0x10000u - r_u32(0x8005A3B8u)) << shift : 0u;
    transfer.requested_mode = 0x8005A3ACu;
    transfer.normalized_mode = 0x8005A438u;
    transfer.address_units = 0x8005A434u;
    transfer.completion = 0x8005A450u;
    transfer.completion_callback = 0x8005A454u;
    transfer.source_address = 0x8005A470u;
    transfer.dma_blocks = 0x8005A474u;
    transfer.address_shift = shift;
    if (!spu_bind_heap(&heap) || !spu_bind_native_transfer_guest(&transfer))
    {
        RRJ_DEBUG_FPUTS("Native SPU saved heap/transfer binding failed\n", stderr);
        abort();
    }
    live_spu_pending_callback = 0;
}

void rrj_spu_completion_service(void);

uint32_t rrj_spu_write(uint32_t source, uint32_t byte_count)
{
    uint32_t bytes = byte_count > 0x7EFF0u ? 0x7EFF0u : byte_count;
    uint32_t span = r_u32(0x8005A438u) == 1u ? (bytes + 1u) & ~1u : (bytes + 63u) & ~63u;
    uint32_t callback = r_u32(0x8005A454u);
    uint32_t result = SpuWrite(xport_guest_ptr(source, span ? span : 1u), bytes);
    if (spu_transfer_failed() || live_spu_pending_callback)
    {
        RRJ_DEBUG_FPUTS("Native SpuWrite transfer failed or completion still pending\n", stderr);
        abort();
    }
    if (!callback)
    {
        RRJ_DEBUG_FPUTS("Missing native SPU BIOS completion event F0000009/20\n", stderr);
        abort();
    }
    live_spu_pending_callback = callback;
    psx_wait_bind_service(4, rrj_spu_completion_service);
    psx_wait_signal_service(4);
    psx_sync_callbacks();
    return result;
}

void rrj_spu_completion_service(void)
{
    uint32_t callback;

    /* Dispatch actual audio IRQs queued by the native sample renderer */
    spu_dispatch_pending_irq();
    if (!psx_callbacks_allowed())
        return;
    callback = live_spu_pending_callback;
    if (!callback)
        return;
    if (spu_transfer_failed())
    {
        RRJ_DEBUG_FPUTS("Native SPU transfer completion failed\n", stderr);
        abort();
    }
    if (!SpuIsTransferCompleted(0))
        return;
    live_spu_pending_callback = 0;
    switch (callback)
    {
        case 0x8001E328:
            (void)sub_8001E328();
            break;
        case 0x8005FCE8:
            (void)sub_8005FCE8();
            break;
        default:
            RRJ_DEBUG_FPRINTF(stderr, "RRJ: missing native SPU completion callback %08X\n", callback);
            abort();
    }
}

static int live_origin_x, live_origin_y;
static uint32_t live_race_draw_calls;

static void trace_display_wait_service(void)
{
    (void)sub_8001B700();
}

static uint32_t trace_race_call(RRJMemory *m, uint32_t target, const uint32_t args[8])
{
    rrj_trace_dispatch_target(target);
    if (target == 0x8001A0C0u)
        return sub_8001A0C0(args[0], args[1], args[2]);
    if (target == 0x8001EE28u)
        return sub_8001EE28(args[0]);
    if (target == 0x8001A2E4u)
        return sub_8001A2E4(args[0]);
    if (target == 0x800138E8u)
        return sub_800138E8(args[0], args[1]);
    if (target == 0x800136BCu)
        return sub_800136BC(args[0], args[1], args[2]);
    if (target == 0x800319F8u)
        return sub_800319F8(args[0], args[1]);
    if (target == 0x8002227Cu)
        return sub_8002227C(args[0], args[1], args[2], args[3], args[4]);
    if (target == 0x80031008u)
        return sub_80031008(args[0]);
    if (target == 0x800333F4u)
        return sub_800333F4(args[0], args[1]);
    if (target == 0x80033F14u)
        return sub_80033F14(args[0], args[1], args[2]);
    if (target == 0x8001ED28u)
        return sub_8001ED28(args[0], args[1], args[2], args[3], args[4], args[5]);
    if (target == 0x8001E938u)
        return sub_8001E938(args[0], args[1], args[2], args[3], args[4]);
    if (target == 0x8001EAA4u)
        return sub_8001EAA4(args[0], args[1]);
    if (target == 0x8001E328u)
        return sub_8001E328();
    if (target == 0x8001E418u)
        return sub_8001E418(xport_guest_ptr(args[0], 24));
    if (target == 0x80016464u)
        return sub_80016464(args[0]);
    if (target == 0x80030FA0u)
        return sub_80030FA0(args[0]);
    if (target == 0x80030FD0u)
        return sub_80030FD0(args[0]);
    if (target == 0x8004F3C8u && rrj_trace_runtime_is_live())
    {
        int32_t result = SpuMalloc((int32_t)args[0]);
        if (spu_heap_failed())
        {
            RRJ_DEBUG_FPUTS("Native SpuMalloc saved heap service failed\n", stderr);
            abort();
        }
        return (uint32_t)result;
    }
    if (target == 0x80050F88u && rrj_trace_runtime_is_live())
    {
        uint32_t result = SpuSetTransferStartAddr(args[0]);
        if (spu_transfer_failed())
        {
            RRJ_DEBUG_FPUTS("Native SpuSetTransferStartAddr service failed\n", stderr);
            abort();
        }
        return result;
    }
    if (target == 0x80050F28u && rrj_trace_runtime_is_live())
        return sub_80050F28(args[0], args[1]);
    if (target == 0x80050EC8u && rrj_trace_runtime_is_live())
    {
        return sub_80050EC8(args[0], args[1]);
    }
    if (target == 0x800C2218u)
        return sub_800C2218(args[0], args[1], (int32_t)args[2]);
    if (target == 0x8003D844u)
        return sub_8003D844(args[0], args[1]);
    if (target == 0x80032938u)
        return sub_80032938(args[0], args[1]);
    if (target == 0x80038550u)
        return sub_80038550(args[0], args[1], args[2], args[3], args[4]);
    if (target == 0x8003CB6Cu)
        return sub_8003CB6C(args[0], args[1]);
    if (target == 0x80017CF0u)
        return sub_80017CF0(args[0], args[1], args[2], args[3]);
    if (target == 0x80017B6Cu)
        return sub_80017B6C(args[0], args[1], args[2], args[3]);
    if (target == 0x8003E338u)
        return sub_8003E338(args[0]);
    if (target == 0x8002DF14u)
        return sub_8002DF14(xport_guest_ptr(args[0], 8));
    if (target == 0x8002E010u)
        return sub_8002E010(xport_guest_cptr(args[0], 8));
    if (target == 0x8002DFC0u)
        return sub_8002DFC0(xport_guest_cptr(args[0], 8), xport_guest_cptr(args[1], 8));
    if (target == 0x8001C298u)
        return sub_8001C298();
    if (target == 0x8001BBC0u)
        return sub_8001BBC0();
    if (target == 0x800245B4u)
    {
        RRJ_DEBUG_FPRINTF(stderr, "Missing incoming-result carrier for 800245B4\n");
        abort();
    }
    if (target == 0x8003D39Cu)
        return sub_8003D39C(args[0], args[1]);
    if (target == 0x80039A08u)
        return sub_80039A08(args[0]);
    if (target == 0x8003CFCCu)
        return sub_8003CFCC(args[0], args[1]);
    if (target == 0x80039F68u)
        return sub_80039F68(args[0]);
    if (target == 0x8008C000u)
        return sub_8008C000(args[0], args[1]);
    if (target == 0x80034D38u)
        return (uint32_t)sub_80034D38((int32_t)args[0], args[1], args[2]);
    if (target == 0x800144B8u)
        return sub_800144B8(args[0]);
    if (target == 0x800394F0u)
        return sub_800394F0(xport_guest_ptr(args[0], 32), args[1]);
    if (target == 0x8001E0B4u)
    {
        (void)sub_8001E0B4(xport_guest_ptr(args[0], args[2]), xport_guest_cptr(args[1], args[2]), args[2]);
        return args[0];
    }
    if (target == 0x80037A30u)
        return sub_80037A30(args[0], xport_guest_ptr(args[1], 32), xport_guest_ptr(args[2], 96), xport_guest_ptr(args[3], 12), args[4]);
    if (target == 0x8001FEB4u)
        return sub_8001FEB4((int32_t)args[0]);
    if (target == 0x8001FC90u)
        return (uint32_t)sub_8001FC90((int32_t)args[0], (int32_t)args[1]);
    if (target == 0x80010028u)
        return sub_80010028(args[0], args[1]);
    if (target == 0x8002E468u)
        return sub_8002E468(xport_guest_ptr(args[0], 6));
    if (target == 0x80017BA0u)
        return sub_80017BA0((int32_t)args[0], (int32_t)args[1], args[2], args[3]);
    if (target == 0x80039AFCu)
        return sub_80039AFC(args[0]);
    if (target == 0x8003EB58u)
        return sub_8003EB58(args[0], args[1], args[2] ? xport_guest_ptr(args[2], 4) : NULL, args[3] ? xport_guest_ptr(args[3], 4) : NULL, args[4]);
    if (target == 0x8004CF74u)
        return sub_8004CF74(args[0]);
    if (target == 0x8001408Cu)
        return sub_8001408C((const char *)xport_guest_cptr(args[0], 1), xport_guest_ptr(args[1], 4), xport_guest_ptr(args[2], 20));
    if (target == 0x80048A6Cu)
    {
        PSX_RECT *rectangle = (PSX_RECT *)xport_guest_ptr(args[0], sizeof(PSX_RECT));
        size_t bytes = rectangle->w > 0 && rectangle->h > 0 ? (size_t)rectangle->w * (size_t)rectangle->h * 2u : 1u;
        return (uint32_t)LoadImagePSX(rectangle, (uint32 *)xport_guest_ptr(args[1], bytes));
    }
    if (target == 0x800150EC)
        return sub_800150EC();
    if (target == 0x80022EEC)
        return sub_80022EEC();
    if (target == 0x800315E8)
        return sub_800315E8(args[0]);
    if (target == 0x80031540)
        return sub_80031540(args[0]);
    if (target == 0x80031CD4)
        return sub_80031CD4(args[0]);
    if (target == 0x80030894)
        return sub_80030894(args[0], args[1], args[2]);
    if (target == 0x80090270)
        return sub_80090270();
    if (target == 0x80031604)
        return sub_80031604(args[0], args[1]);
    if (target == 0x80031BF8)
        return sub_80031BF8(args[0], args[1]);
    if (target == 0x80031B4C)
        return sub_80031B4C(args[0]);
    if (target == 0x80031E1C)
        return sub_80031E1C(args[0], args[1], args[2]);
    if (target == 0x80032A20)
        return sub_80032A20(args[0], args[1], args[2], args[3], args[4], args[5]);
    if (target == 0x80032C6C)
        return sub_80032C6C(args[0], args[1], args[2], args[3], args[4]);
    if (target == 0x80032CC8u)
        return sub_80032CC8(args[0], args[1], args[2], args[3], args[4], args[5]);
    if (target == 0x8003CEC4u)
        return sub_8003CEC4(args[0], args[1], args[2]);
    if (target == 0x800324CC)
        return sub_800324CC(args[0], args[1], args[2]);
    if (target == 0x800335B4)
        return sub_800335B4(args[0], args[1], args[2]);
    if (target == 0x80023960)
        return sub_80023960(args[0], args[1]);
    if (target == 0x800151EC)
        return sub_800151EC();
    if (target == 0x8009C308)
        return sub_8009C308();
    if (target == 0x800B8018)
        return sub_800B8018(args[0]);
    if (target == 0x800B8020)
        return sub_800B8020(args[0]);
    if (target == 0x8009B474)
        return sub_8009B474((int32_t)args[0], args[1]);
    if (target == 0x8009E89C)
        return sub_8009E89C(args[0]);
    if (target == 0x8009E8A4)
        return sub_8009E8A4(args[0], args[1]);
    if (target == 0x8009CFF4)
        return sub_8009CFF4(args[0]);
    if (target == 0x800C45D8)
        return sub_800C45D8(args[0], xport_guest_ptr(args[1], 4));
    if (target == 0x8005C418)
        return sub_8005C418(args[0]);
    if (target == 0x8005C8F4)
        return sub_8005C8F4(args[0]);
    if (target == 0x80044894)
        return 0;
    if (target == 0x800487C0)
        return (uint32_t)DrawSync((sint32)args[0]);
    if (target == 0x80048E24)
    {
        if (rrj_trace_runtime_is_live())
        {
            DRAWENV *environment = (DRAWENV *)xport_guest_ptr(args[0], sizeof(DRAWENV));
            live_origin_x = environment->clip.x;
            live_origin_y = environment->clip.y;
            PutDrawEnv(environment);
        }
        return args[0];
    }
    if (target == 0x80048FF0)
    {
        if (rrj_trace_runtime_is_live())
            PutDispEnv((DISPENV *)xport_guest_ptr(args[0], sizeof(DISPENV)));
        return args[0];
    }
    if (target == 0x80048CAC)
    {
        return rrj_gpu_clear_ot(args[0], args[1]);
    }
    if (target == 0x80075EE0)
        return sub_80075EE0((int32_t)args[0]);
    if (target == 0x8008F068)
        return sub_8008F068((int32_t)args[0]);
    if (target == 0x800A2898)
        return sub_800A2898((int32_t)args[0]);
    if (target == 0x800A2A64)
        return sub_800A2A64((int32_t)args[0]);
    if (target == 0x800A4774)
        return sub_800A4774((int32_t)args[0]);
    if (target == 0x8007B840)
        return sub_8007B840((int32_t)args[0]);
    if (target == 0x80079B20)
    {
        sub_80079B20(args[0], (int32_t)args[1]);
        return 0;
    }
    if (target == 0x80078DB4)
        return sub_80078DB4(args[0]);
    if (target == 0x8007AC04)
        return sub_8007AC04(args[0]);
    if (target == 0x80095724)
        return sub_80095724(args[0], args[1]);
    if (target == 0x800A8C78)
        return sub_800A8C78(args[0], (int32_t)args[1], (int32_t)args[2]);
    if (target == 0x800A8FE8)
        return sub_800A8FE8(args[0]);
    if (target == 0x800B3AD0)
        return sub_800B3AD0(args[0], args[1]);
    if (target == 0x800B12A0)
    {
        uint16_t normal[3];
        uint32_t translation[3], axis, result;
        for (axis = 0; axis < 3; ++axis)
        {
            normal[axis] = r_u16(args[1] + 2 * axis);
            if (args[5])
                translation[axis] = r_u32(args[5] + 4 * axis);
        }
        result = sub_800B12A0(args[0], normal, args[2], (int32_t)args[3], args[4], args[5] ? translation : NULL);
        if (args[5])
            for (axis = 0; axis < 3; ++axis)
                w_u32(args[5] + 4 * axis, translation[axis]);
        return result;
    }
    if (target == 0x800B6F40)
    {
        uint32_t contact_depth = 0, contact_count = 0, result;
        result = sub_800B6F40(args[0], xport_guest_cptr(args[1], 6), args[2], args[3] ? &contact_depth : NULL, args[4] ? &contact_count : NULL);
        if (args[4])
            w_u32(args[4], contact_count);
        if (result != 8 && args[3])
            w_u32(args[3], contact_depth);
        return result;
    }
    if (target == 0x800CB304)
        return sub_800CB304((int32_t)args[0]);
    if (target == 0x800CB4F8)
        return sub_800CB4F8((int32_t)args[0]);
    if (target == 0x800CAAF0)
        return sub_800CAAF0(args[0]);
    if (target == 0x8005BE58)
        return sub_8005BE58(args[0]);
    if (target == 0x800CB02C)
        return sub_800CB02C(args[0]);
    if (target == 0x800A13C4)
        return sub_800A13C4((int32_t)args[0]);
    if (target == 0x80090814)
        return sub_80090814((int32_t)args[0]);
    if (target == 0x80090D84)
        return sub_80090D84(args[0]);
    if (target == 0x80091468)
        return sub_80091468(args[0]);
    if (target == 0x800C5078)
        return sub_800C5078(args[0]);
    if (target == 0x800C47CC)
        return sub_800C47CC(args[0]);
    if (target == 0x800C4860)
        return sub_800C4860(args[0]);
    if (target == 0x800C4B30)
        return sub_800C4B30(args[0]);
    if (target == 0x800C4BA0)
        return sub_800C4BA0(args[0]);
    if (target == 0x800C4E18)
        return sub_800C4E18(args[0]);
    if (target == 0x800881B4)
        return sub_800881B4(args[0], (int32_t)args[1]);
    if (target == 0x80086584)
        return sub_80086584(args[0], args[1], args[2], args[3]);
    if (target == 0x800863EC)
        return sub_800863EC(args[0]);
    if (target == 0x8008AAB0)
        return sub_8008AAB0(args[0]);
    if (target == 0x800A8DF0)
        return sub_800A8DF0(xport_guest_ptr(args[0], 1), xport_guest_ptr(args[1], 12), args[2]);
    if (target == 0x800657A8)
        return sub_800657A8();
    if (target == 0x800662B4)
        return 0;
    if (target == 0x800A421C)
        return sub_800A421C(args[0]);
    if (target == 0x800853E4)
        return sub_800853E4(args[0], args[1]);
    if (target == 0x80086E1C)
        return sub_80086E1C(args[0]);
    if (target == 0x8003775C)
        return sub_8003775C(args[0], args[1] ? xport_guest_ptr(args[1], 32) : NULL, args[2]);
    if (target == 0x800374D4)
        return sub_800374D4(args[0], args[1]);
    if (target == 0x80088140)
        return sub_80088140(args[0]);
    if (target == 0x8008CFDC)
        return sub_8008CFDC();
    if (target == 0x8008DBCC)
        return sub_8008DBCC(args[0]);
    if (target == 0x800667C4)
        return sub_800667C4(args[0], args[1]);
    if (target == 0x8005E1D8)
        return sub_8005E1D8(args[0]);
    if (target == 0x8005D2A8)
        return sub_8005D2A8(args[0]);
    if (target == 0x8005E5A4)
        return sub_8005E5A4(args[0], args[1]);
    if (target == 0x8005E558)
        return sub_8005E558(args[0], args[1], args[2]);
    if (target == 0x8005D63C)
        return sub_8005D63C(args[0]);
    if (target == 0x80066A60)
        return sub_80066A60(args[0], args[1]);
    if (target == 0x800714FC)
        return sub_800714FC(args[0], args[1], args[2]);
    if (target == 0x8001B44C)
        return sub_8001B44C(args[0], args[1], args[2], args[3]);
    if (target == 0x80071BCC)
        return sub_80071BCC(args[0]);
    if (target == 0x8001005C)
        return sub_8001005C(xport_guest_ptr(args[0], 18), xport_guest_cptr(args[1], 16));
    if (target == 0x8005CB04)
        return sub_8005CB04(args[0], args[1]);
    if (target == 0x8005C58C)
        return sub_8005C58C(args[0], args[1]);
    if (target == 0x8005C52C)
        return sub_8005C52C(args[0]);
    if (target == 0x8005C4EC)
        return sub_8005C4EC(args[0], args[1]);
    if (target == 0x80011C4C)
        return sub_80011C4C();
    if (target == 0x8002305C)
        return sub_8002305C();
    if (target == 0x800237B8)
        return sub_800237B8();
    if (target == 0x80023A14)
        return sub_80023A14();
    if (target == 0x80023DB8)
        return sub_80023DB8(args[0]);
    if (target == 0x80023FBC)
        return sub_80023FBC();
    if (target == 0x80023CAC)
        return sub_80023CAC();
    if (target == 0x800247E8)
        return sub_800247E8();
    if (target == 0x80022C64)
        return sub_80022C64();
    if (target == 0x80012524)
        return sub_80012524();
    if (target == 0x8001264C)
        return sub_8001264C();
    if (target == 0x80018E54)
        return sub_80018E54(args[0]);
    if (target == 0x80043E24)
        return sub_80043E24();
    if (target == 0x80043DC4)
        return sub_80043DC4(args[0]);
    if (target == 0x800C89A0)
        return sub_800C89A0();
    if (target == 0x800C8B24)
    {
        sub_800C8B24(args[0]);
        return 0;
    }
    if (target == 0x80064B9C)
        return sub_80064B9C(args[0]);
    if (target == 0x800650D0)
        return sub_800650D0(args[0]);
    if (target == 0x800654B4)
        return sub_800654B4((int32_t)args[0], (int32_t)args[1]);
    if (target == 0x800656A8)
        return sub_800656A8((int32_t)args[0], (int32_t)args[1], (int32_t)args[2], (int32_t)args[3]);
    if (target == 0x800662BC)
        return sub_800662BC();
    if (target == 0x80065174)
        return sub_80065174();
    if (target == 0x800662C4)
        return sub_800662C4(args[0]);
    if (target == 0x80020400)
        return sub_80020400(args[0], args[1]);
    if (target == 0x800644F4)
        return sub_800644F4();
    if (target == 0x800644FC)
        return sub_800644FC(args[0]);
    if (target == 0x8006396C)
        return sub_8006396C();
    if (target == 0x80063C5C)
        return sub_80063C5C(args[0]);
    if (target == 0x8002C4F8)
        return sub_8002C4F8(args[0], args[1], args[2]);
    if (target == 0x80021988)
        return sub_80021988((int32_t)args[0], (int32_t)args[1]);
    if (target == 0x8002AF80)
        return sub_8002AF80(args[0]);
    if (target == 0x8002BAE8)
        return sub_8002BAE8(args[0], args[1], args[2]);
    if (target == 0x8002BE14)
        return sub_8002BE14(args[0], args[1], args[2]);
    if (target == 0x800C8CD4)
    {
        sub_800C8CD4(args[0]);
        return 0u;
    }
    if (target == 0x8004D184)
        return sub_8004D184((int32_t)args[0], (int32_t)args[1]);
    if (target == 0x8001E084)
        return sub_8001E084();
    if (target == 0x8001C304)
        return sub_8001C304((int32_t)args[0], (int32_t)args[1], (int32_t)args[2], (int32_t)args[3], args[6]);
    if (target == 0x8005FA68)
        return sub_8005FA68(args[0], args[1], args[2], args[3]);
    if (target == 0x800C5618)
        return sub_800C5618(args[0]);
    if (target == 0x8005FAC4)
        return sub_8005FAC4(args[0], args[1], args[2]);
    if (target == 0x80095410)
        return sub_80095410(args[0]);
    if (target == 0x8008B84C)
        return sub_8008B84C(args[0]);
    if (target == 0x8005F9B0)
        return sub_8005F9B0(args[0], (int32_t)args[1], (int32_t)args[2], (int32_t)args[3]);
    if (target == 0x800C5558)
        return sub_800C5558(args[0]);
    if (target == 0x8005F834)
        return sub_8005F834(args[0], args[1], args[2]);
    if (target == 0x80063408)
        return sub_80063408(args[0], args[1], args[2], args[3], args[4]);
    if (target == 0x800C5168)
        return sub_800C5168(args[0], args[1], args[2], args[3]);
    if (target == 0x8001FE80)
        return sub_8001FE80((int32_t)args[0], args[1], args[2]);
    if (target == 0x8002DEC8)
        return sub_8002DEC8(args[0], (int32_t)args[1]);
    if (target == 0x8002DE40)
        return sub_8002DE40((int32_t)args[0], args[1]);
    if (target == 0x800C5380)
        return sub_800C5380(args[0], (int32_t)args[1], args[2], args[3]);
    if (target == 0x80013E64)
        return sub_80013E64(args[0]);
    if (target == 0x80013AF8)
        return sub_80013AF8(args[0], args[1]);
    if (target == 0x800C569C)
        return sub_800C569C(args[0], args[1], (int32_t)args[2], args[3]);
    if (target == 0x800C52A8)
        return sub_800C52A8(args[0], args[1], args[2]);
    if (target == 0x800C52A0)
        return sub_800C52A0(args[0], args[1]);
    if (target == 0x80061E50)
        return sub_80061E50(args[0], args[1], args[2]);
    if (target == 0x8005FE58)
        return sub_8005FE58(args[0], args[1], args[2], args[3]);
    if (target == 0x80016528)
        return sub_80016528();
    if (target == 0x8005FF84)
        return sub_8005FF84(args[0], args[1], args[2], args[3]);
    if (target == 0x80060178)
        return sub_80060178(args[0], args[1], args[2], args[3]);
    if (target == 0x800603E4)
        return sub_800603E4(args[0], args[1], args[2], args[3]);
    if (target == 0x800606F0)
        return sub_800606F0(args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
    if (target == 0x800606F8)
        return sub_800606F8(args[0], args[1], args[2], args[3], args[4], args[5], args[6], r_u32(0x8005B2F8u));
    if (target == 0x80060C10)
        return sub_80060C10(args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
    if (target == 0x80060C18)
        return sub_80060C18(args[0], args[1], args[2], args[3], args[4], args[5], args[6], r_u32(0x8005ACDCu));
    if (target == 0x8006148C)
        return sub_8006148C(args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
    if (target == 0x80061494)
        return sub_80061494(args[0], args[1], args[2], args[3], args[4], args[5], args[6], r_u32(0x8005ACDCu));
    if (target == 0x80061F6C)
        return sub_80061F6C(args[0], args[1], args[2], args[3], args[4]);
    if (target == 0x80062368)
        return sub_80062368(args[0], args[1], args[2], args[3], args[4]);
    if (target == 0x80062610)
        return sub_80062610(args[0], args[1], args[2], args[3]);
    if (target == 0x80062618)
        return sub_80062618(args[0], args[1], args[2], r_u32(0x8005B2F8u));
    if (target == 0x80062C40)
        return sub_80062C40(args[0], args[1], args[2], args[3], args[4]);
    if (target == 0x80062D9C)
        return sub_80062D9C(args[0], args[1], args[2], args[3]);
    if (target == 0x8004CE44)
        return sub_8004CE44(args[0], args[1], args[2], args[3], args[4]);
    if (target == 0x80062F34)
        return sub_80062F34(args[0], args[1], args[2], args[3]);
    if (target == 0x80063530)
        return sub_80063530(args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
    if (target == 0x800636F0)
        return sub_800636F0(args[0], args[1], args[2], args[3], args[4]);
    if (target == 0x8005F030)
        return sub_8005F030(args[0], args[1], args[2], args[3], args[4], args[5]);
    if (target == 0x8005FB4C)
        return sub_8005FB4C(args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
    if (target == 0x8003B9A8)
        return sub_8003B9A8((int32_t)args[0], args[1]);
    if (target == 0x8003BC48)
        return sub_8003BC48((int32_t)args[0], (int32_t)args[1], xport_guest_ptr(args[2], 4), (int32_t)args[3]);
    if (target == 0x8003C590)
        return sub_8003C590(args[0]);
    if (target == 0x8005E848 || target == 0x8005E850)
        return sub_8005E848();
    if (target == 0x8002CA5C)
        return sub_8002CA5C();
    if (target == 0x80043F00)
        return sub_80043F00(args[0]);
    if (target == 0x800270F0)
        return sub_800270F0(args[0]);
    if (target == 0x8004F0A8)
        return sub_8004F0A8((int32_t)args[0], args[1]);
    if (target == 0x80051A38)
        return sub_80051A38((int32_t)args[0], (int32_t)args[1], (int32_t)args[2], (int32_t)args[3]);
    if (target == 0x800506A8)
        return sub_800506A8(args[0], args[1], args[2], args[3]);
    if (target == 0x80051088)
        return sub_80051088(args[0], args[1]);
    if (target == 0x8001F900)
        return sub_8001F900(args[0], args[1]);
    if (target == 0x80051C38)
        return sub_80051C38(args[0], xport_guest_ptr(args[1], 64));
    if (target == 0x8001F874)
        return sub_8001F874((int32_t)args[0], args[1]);
    if (target == 0x80016768)
        return sub_80016768(args[0], args[1], args[2], args[3], args[4], args[5]);
    if (target == 0x800167A4)
        return sub_800167A4(args[0]);
    if (target == 0x8001B244)
        return sub_8001B244(args[0]);
    if (target == 0x80027778)
        return sub_80027778(args[0], args[1], args[2], args[3]);
    if (target == 0x80027974)
        return sub_80027974(args[0], args[1], args[2]);
    if (target == 0x800179D8)
        return sub_800179D8(args[0], args[1]);
    if (target == 0x800169D0)
        return sub_800169D0(args[0]);
    if (target == 0x80016E4C)
        return sub_80016E4C(args[0]);
    if (target == 0x800184AC)
        return sub_800184AC(args[0]);
    if (target == 0x80018FAC)
        return sub_80018FAC();
    if (target == 0x80018C1C)
        return sub_80018C1C(args[0]);
    if (target == 0x800505F8)
        return sub_800505F8(xport_guest_ptr(args[0], 12));
    if (target == 0x8001C3F4)
        return sub_8001C3F4();
    if (target == 0x8001C428)
        return sub_8001C428();
    if (target == 0x8001C408)
        return sub_8001C408();
    if (target == 0x8001C5F8)
        return sub_8001C5F8();
    if (target == 0x80020E30)
        return sub_80020E30(args[0]);
    if (target == 0x8002D2F4)
        return sub_8002D2F4();
    if (target == 0x8001DDC4)
        return sub_8001DDC4(args[0], args[1]);
    if (target == 0x8001B700)
        return sub_8001B700();
    if (target == 0x8001FD24)
        return (uint32_t)sub_8001FD24(args[0], args[1], args[2], args[3]);
    if (target == 0x8001F7EC)
        return sub_8001F7EC(args[0]);
    if (target == 0x80019C54)
        return sub_80019C54(args[0], xport_guest_ptr(args[1], 12));
    if (target == 0x80019990)
        return sub_80019990();
    if (target == 0x8004CE14)
    {
        uint32_t result = (args[2] ? 0xE1000200u : 0xE1000000u) | (args[3] & 0x9FFu) | (args[1] ? 0x400u : 0);

        w_u8(args[0] + 3, 1);
        w_u32(args[0] + 4, result);
        return result;
    }
    if (target == 0x8002F2E8)
        return sub_8002F2E8(args[0]);
    if (target == 0x8002F17C)
        return sub_8002F17C(args[0]);
    if (target == 0x80070F9C)
        return sub_80070F9C(args[0], args[1]);
    if (target == 0x8004D264)
        return sub_80070F9C(args[0], args[1]);
    if (target == 0x8004D154)
    {
        uint32_t matrix[5];
        uint32_t index;

        for (index = 0; index != 5; ++index)
            matrix[index] = r_u32(args[0] + 4 * index);
        for (index = 0; index != 5; ++index)
            xport_gte_write_control(index, matrix[index]);
        return 0;
    }
    if (target == 0x80048DB4)
    {
        if (rrj_trace_runtime_is_live())
        {
            ++live_race_draw_calls;
            if (!rrj_gpu_draw_ot(args[0]))
                abort();
        }
        return 0;
    }
    if (target == 0x80048ACC)
    {
        PSX_RECT *rectangle = (PSX_RECT *)xport_guest_ptr(args[0], sizeof(PSX_RECT));
        size_t bytes = rectangle->w > 0 && rectangle->h > 0 ? (size_t)rectangle->w * rectangle->h * 2u : 0;
        return (uint32_t)StoreImage(rectangle, (uint32 *)xport_guest_ptr(args[1], bytes));
    }
    if (target == 0x8004D7B4 || target == 0x8004D9A8 || target == 0x8004D9E4 || target == 0x8004D90C || target == 0x8004D988)
        return 0;
    if (target == 0x800674C8)
        return sub_800674C8();
    if (target == 0x8008D56C)
        return sub_8008D56C(args[0]);
    if (target == 0x8008B99C)
        return sub_8008B99C(args[0]);
    if (target == 0x80023870)
        return sub_80023870();
    if (target == 0x80084E10)
        return sub_80084E10(args[0], args[1]);
    if (target == 0x80067AC4)
        return sub_80067AC4(args[0], args[1]);
    if (target == 0x80068468)
        return sub_80068468(args[0], args[1]);
    if (target == 0x8006780C)
        return sub_8006780C(args[0], args[1]);
    if (target == 0x80067690)
        return sub_80067690(args[0], args[1]);
    if (target == 0x80028E8C)
        return sub_80028E8C(args[0], args[1], args[2]);
    if (target == 0x80029174)
        return sub_80029174(args[0], args[1], args[2], args[3]);
    if (target == 0x8002926C)
        return sub_8002926C(args[0], args[1], args[2], args[3], args[4]);
    if (target == 0x800295AC)
        return sub_800295AC(args[0], args[1], args[2], args[3], args[4]);
    if (target == 0x8002AB14)
        return sub_8002AB14(args[0], args[1]);
    if (target == 0x8002A2C8)
        return sub_8002A2C8(args[0], args[1], args[2], args[3], args[4]);
    if (target == 0x80028E74)
        return sub_80028E74(args[0]);
    if (target == 0x800289E8)
        return sub_800289E8(args[0], args[1], args[2]);
    if (target == 0x80028C78)
        return sub_80028C78(args[0], args[1], args[2], args[3]);
    if (target == 0x8002A8E4)
        return sub_8002A8E4(args[0], args[1]);
    if (target == 0x80017F64)
        return sub_80017F64(args[0], args[1]);
    if (target == 0x800182B0)
        return sub_800182B0(args[0]);
    if (target == 0x8001836C)
        return sub_8001836C(args[0]);
    if (target == 0x8002A974)
        return sub_8002A974(args[0], args[1], args[2], args[3]);
    if (target == 0x80029D88)
        return sub_80029D88(args[0], args[1], args[2], args[3]);
    if (target == 0x8002823C)
        return sub_8002823C(args[0], args[1]);
    if (target == 0x80067770)
        return sub_80067770(args[0], args[1]);
    if (target == 0x800674D4)
        return sub_800674D4(args[0], (int32_t)args[1], args[2]);
    if (target == 0x80068D50)
        return sub_80068D50(args[0], args[1]);
    if (target == 0x80068E2C)
        return sub_80068E2C(args[0], args[1]);
    if (target == 0x80068EB8)
        return sub_80068EB8(args[0]);
    if (target == 0x80035958)
        return sub_80035958(args[0]);
    if (target == 0x80069200)
        return sub_80069200(args[0]);
    if (target == 0x8006929C)
        return sub_8006929C(args[0], args[1], args[2]);
    if (target == 0x80069784)
        return sub_80069784(args[0], args[1], args[2]);
    if (target == 0x80069CF0)
        return sub_80069CF0(args[0], args[1], args[2]);
    if (target == 0x8006A25C)
        return sub_8006A25C(args[0], args[1], args[2]);
    if (target == 0x8006A630)
        return sub_8006A630(args[0], args[1]);
    if (target == 0x8006C888)
        return sub_8006C888(args[0], args[1]);
    if (target == 0x8006D350)
        return sub_8006D350(args[0], args[1]);
    if (target == 0x8006DC20)
        return sub_8006DC20(args[0], args[1]);
    if (target == 0x8006E474)
        return sub_8006E474(args[0], args[1]);
    if (target == 0x8006F5D0)
        return sub_8006F5D0(args[0], args[1]);
    if (target == 0x80068FCC)
        return sub_80068FCC(args[0]);
    if (target == 0x800706A4)
        return sub_800706A4(args[0]);
    if (target == 0x8001298C)
        return sub_8001298C(args[0], args[1]);
    if (target == 0x800358C0)
        return sub_800358C0(args[0]);
    if (target == 0x80035F48)
        return sub_80035F48(args[0]);
    if (target == 0x800363F0)
        return sub_800363F0(args[0]);
    if (target == 0x80035040)
        return sub_80035040(args[0], args[1], (int32_t)args[2]);
    if (target == 0x800351EC)
        return sub_800351EC(args[0], args[1], (int32_t)args[2], args[3], args[4], args[5]);
    if (target == 0x800353C4)
        return sub_800353C4(args[0], (int32_t)args[1], args[2], args[3]);
    if (target == 0x80036438)
        return sub_80036438((int32_t)args[0], args[1], args[2]);
    if (target == 0x80036614)
        return sub_80036614(args[0], args[1], args[2]);
    if (target == 0x80035680)
        return sub_80035680((int32_t)args[0], args[1], args[2]);
    if (target == 0x800106DC)
        return sub_800106DC(args[0], args[1], args[2]);
    if (target == 0x8001064C)
        return sub_8001064C();
    if (target == 0x80030E58)
        return sub_80030E58(args[0]);
    if (target == 0x80033198)
        return sub_80033198(args[0]);
    if (target == 0x80023900)
        return sub_80023900(args[0], args[1], args[2]);
    if (target == 0x800243EC)
        return sub_800243EC(args[0], args[1]);
    if (target == 0x800318E8)
        return sub_800318E8(args[0], args[1]);
    if (target == 0x800312D0)
        return sub_800312D0(args[0]);
    if (target == 0x80013828)
        return sub_80013828(args[0]);
    if (target == 0x80023868)
        return sub_80023868(args[0]);
    if (target == 0x800313EC)
        return sub_800313EC(args[0]);
    if (target == 0x80030608)
        return sub_80030608();
    if (target == 0x80031D70)
        return sub_80031D70();
    if (target == 0x800320CC)
        return sub_800320CC(args[0]);
    if (target == 0x80043DA4)
    {
        sub_80043DA4();
        return 0;
    }
    if (target == 0x80043DB4)
    {
        sub_80043DB4();
        return 0;
    }
    if (target == 0x800C4550)
        return sub_800C4550(args[0], args[1], args[2]);
    if (target == 0x800CAA44)
        return sub_800CAA44(args[0], (int32_t)args[1]);
    if (target == 0x8009ACA4)
        return sub_8009ACA4();
    if (target == 0x8009AB60)
        return sub_8009AB60();
    if (target == 0x8001CB3C)
    {
        static uint32_t last_buttons = 0xffffffffu;
        uint32_t result = sub_8001CB3C();
        if (rrj_trace_runtime_is_live() && RRJ_DEBUG_GETENV("RRJ_TRACE_INPUT"))
        {
            uint32_t buttons = r_u32(0x800D6DE4u);
            if (buttons != last_buttons)
            {
                uint32_t actor = r_u32(0x8005B38Cu);
                RRJ_DEBUG_FPRINTF(stdout, "native_input buttons=%04X actor=%08X flags=%08X speed=%u clock=%u\n", buttons, actor, actor ? r_u32(actor + 560u) : 0u, actor ? r_u32(actor + 480u) : 0u, r_u32(0x800D5D44u));
                last_buttons = buttons;
            }
        }
        return result;
    }
    if (target == 0x8008A998)
        return sub_8008A998(args[0], args[1]);
    if (target == 0x800C2348)
        return sub_800C2348(args[0], args[1]);
    if (target == 0x8008AB00)
        return sub_8008AB00(args[0]);
    if (target == 0x8008AD38)
        return sub_8008AD38(args[0]);
    if (target == 0x8008CD88)
        return sub_8008CD88(args[0]);
    if (target == 0x8008AC80)
        return sub_8008AC80(args[0]);
    if (target == 0x8008ACE8)
        return sub_8008ACE8(args[0]);
    if (target == 0x8009A298)
        return sub_8009A298(args[0]);
    if (target == 0x800B9414)
        return sub_800B9414(args[0]);
    if (target == 0x800B941C)
        return sub_800B941C(args[0]);
    if (target == 0x8007F0BC)
        return sub_8007F0BC(args[0], args[1]);
    if (target == 0x80037338)
        return sub_80037338();
    if (target == 0x8003E150)
        return sub_8003E150();
    if (target == 0x8003AE24)
        return sub_8003AE24();
    if (target == 0x8003B520)
        return sub_8003B520();
    if (target == 0x8007504C)
        return sub_8007504C(args[0], args[1]);
    if (target == 0x8007FA4C)
        return sub_8007FA4C(args[0]);
    if (target == 0x800807F0)
        return sub_800807F0(args[0], args[1]);
    if (target == 0x80093E6C)
        return sub_80093E6C();
    if (target == 0x800950E8)
        return sub_800950E8();
    if (target == 0x80032810)
        return sub_80032810(args[0], args[1], args[2]);
    if (target == 0x8003297C)
        return sub_8003297C(args[0], args[1]);
    if (target == 0x80033B50)
        return sub_80033B50(args[0], args[1], args[2], args[3]);
    if (target == 0x80032190)
        return sub_80032190(args[0], args[1]);
    if (target == 0x800322D0)
        return sub_800322D0(args[0], args[1]);
    if (target == 0x800457E8)
    {
        uint32_t previous = r_u32(0x8005AF60u);

        w_u32(0x8005AF60u, args[0]);
        return previous;
    }
    if (target == 0x800457A8)
        return 0;
    if (target == 0x8004594C)
        return 1;
    if (target == 0x8005D36C)
        return sub_8005D36C(args[0]);
    if (target == 0x800B6BD0)
        return sub_800B6BD0(xport_guest_cptr(args[0], 12), xport_guest_ptr(args[1], 6), xport_guest_ptr(args[2], 6), xport_guest_cptr(args[3], 12));
    if (target == 0x800C1DD4)
        return sub_800C1DD4(args[0]);
    if (target == 0x800C258C)
        return sub_800C258C(args[0], args[1]);
    if (target == 0x80017DA0)
        return sub_80017DA0(args[0], args[1], args[2], args[3]);
    rrj_wip_handoff(m, target, "race_loop", "trace_race_call", __FILE__, __LINE__, "translate_callback_target", args, 8);
    RRJ_DEBUG_FPRINTF(stderr, "WIP handoff: %08X trace_race_call\n", target);
    exit(20);
}

static int trace_replay(RRJMemory *m)
{
    static uint8_t trace_spu[524288];
    const char *path = RRJ_DEBUG_GETENV("RRJ_PHASE_CHECKPOINT_LOAD");
    uint32_t ordinal;
    xport_set_headless(1);
    if (!path)
    {
        RRJ_DEBUG_FPUTS("Cannot load native checkpoint: missing RRJ_PHASE_CHECKPOINT_LOAD\n", stderr);
        return 22;
    }
    if (!load_race_checkpoint(m, path, &ordinal))
    {
        RRJ_DEBUG_FPUTS("Cannot load native checkpoint: incompatible Road Rash ABI\n", stderr);
        return 22;
    }
    rrj_spu_initialize(trace_spu);
    rrj_wip_frame(r_u32(0x800D5D48));
    if (!rrj_trace_runtime_init(m, ordinal))
    {
        RRJ_DEBUG_FPUTS("Cannot initialize trace runtime: incomplete environment\n", stderr);
        return 14;
    }
    if (!rrj_trace_phase_boundary(m, 0x80012370))
        (void)sub_80012370();
    return rrj_trace_runtime_finish(0);
}

static int write_file(const char *path, const void *data, size_t size)
{
    FILE *f = xport_fopen(path, "wb");
    int ok;
    if (!f)
        return 0;
    ok = fwrite(data, 1, size, f) == size;
    if (fclose(f) != 0)
        ok = 0;
    return ok;
}

static int first_race_play(RRJMemory *m)
{
    m->display_wait_service = trace_display_wait_service;
    static uint8_t race_spu[524288];
    GpuRasterState raster;
    uint32_t ordinal;
    int result;

    if (!load_race_checkpoint(m, "DATA/first-race.rrcp", &ordinal) || ordinal != 152 || !read_file("DATA/first-race.vram", VRAM, sizeof(VRAM)) || !read_file("DATA/first-race.spu", race_spu, sizeof race_spu) || !read_file("DATA/first-race.raster", &raster, sizeof raster) || !gpu_import_raster(&raster, VRAM, sizeof(VRAM)))
    {
        RRJ_DEBUG_FPUTS("Cannot load DATA/first-race runtime package\n", stderr);
        return 22;
    }
    live_race_draw_calls = 0;
    if (!rrj_trace_runtime_init_live(m))
    {
        RRJ_DEBUG_FPUTS("Cannot initialize first-race runtime\n", stderr);
        return 22;
    }
    rrj_spu_initialize(race_spu);
    live_spu_restore();
    rrj_trace_runtime_set_live_poll(rrj_spu_completion_service);
    if (!xport_audio_init())
    {
        RRJ_DEBUG_FPUTS("Windows audio device initialization failed\n", stderr);
        return 2;
    }
    PadInit(0);
    rrj_wip_frame(r_u32(0x800D5D48));
    gpu_begin();
    if (!rrj_trace_phase_boundary(m, 0x80012370))
        (void)sub_80012370();
    result = rrj_trace_runtime_finish(0);
    {
        const char *output = RRJ_DEBUG_GETENV("RRJ_LIVE_RAM_OUTPUT");
        if (output && *output && !write_file(output, DRAM, sizeof(DRAM)))
            result = 2;
    }
    xport_audio_shutdown();
    return result;
}

static int first_race_run(RRJMemory *m, int headless)
{
    int result;

    xport_set_headless(headless);
    if (ResetGraph(0) < 0)
        return 2;
    if (headless && !RRJ_DEBUG_GETENV("RRJ_LIVE_FRAME_LIMIT"))
        rrj_trace_set_live_limit(60);
    result = first_race_play(m);
    if (headless && result == 0)
    {
        RRJ_DEBUG_PRINTF("First race: draws=%u\n", live_race_draw_calls);
        if (!live_race_draw_calls || !save_live_frame("../status/first-race-smoke"))
            result = 2;
    }
    return result;
}

/* Diagnostic packet replay only; this does not run game logic. */

static uint32_t update_video_menu(uint32_t menu)
{
    return sub_8006DB5C(menu);
}

static uint32_t update_video_tick(uint32_t menu)
{
    return sub_8006DE5C(menu);
}

/* The original loading entries discard the table's menu argument */
static uint32_t rrj_menu_loading_update(uint32_t menu)
{
    return sub_8006C354();
}

static uint32_t rrj_menu_loading_draw(uint32_t menu)
{
    return sub_8006E008();
}

RRJMenuUpdateFunction rrj_menu_update_function(uint32_t address)
{
    switch (address)
    {
        case 0x80061584u:
            return sub_80061584;
        case 0x80062D6Cu:
            return sub_80062D6C;
        case 0x80063C7Cu:
            return sub_80063C7C;
        case 0x80064154u:
            return sub_80064154;
        case 0x80064254u:
            return sub_80064254;
        case 0x800649BCu:
            return sub_800649BC;
        case 0x80065768u:
            return sub_80065768;
        case 0x8006883Cu:
            return sub_8006883C;
        case 0x80069418u:
            return sub_80069418;
        case 0x8006A8FCu:
            return sub_8006A8FC;
        case 0x8006AE6Cu:
            return sub_8006AE6C;
        case 0x80069440:
            return sub_80069440;
        case 0x80069560:
            return sub_80069560;
        case 0x8006AAF8:
            return sub_8006AAF8;
        case 0x8006AC80:
            return sub_8006AC80;
        case 0x8006A838:
            return sub_8006A838;
        case 0x8006AB58:
            return sub_8006AB58;
        case 0x8006AD20:
            return sub_8006AD20;
        case 0x8006B7BC:
            return sub_8006B7BC;
        case 0x8006C354:
            return rrj_menu_loading_update;
        case 0x8006E008:
            return rrj_menu_loading_draw;
        case 0x8006C3A4u:
            return sub_8006C3A4;
        case 0x8006C558u:
            return sub_8006C558;
        case 0x8006D3E0u:
            return sub_8006D3E0;
        case 0x8006D5B0u:
            return sub_8006D5B0;
        case 0x8006D630u:
            return sub_8006D630;
        case 0x80072100u:
            return sub_80072100;
        case 0x80076370u:
            return sub_80076370;
        case 0x8007EAC0u:
            return sub_8007EAC0;
        case 0x8007ED34u:
            return sub_8007ED34;
        case 0x8007EDE0u:
            return sub_8007EDE0;
        case 0x8007EEB8u:
            return sub_8007EEB8;
        case 0x8007F0D0u:
            return sub_8007F0D0;
        case 0x8007E9D0u:
            return sub_8007E9D0;
        case 0x8007F158u:
            return sub_8007F158;
        case 0x80080A70u:
            return sub_80080A70;
        case 0x80080ACCu:
            return sub_80080ACC;
        case 0x8006DB5Cu:
            return update_video_menu;
        case 0x8006DE5Cu:
            return update_video_tick;
        default:
            RRJ_DEBUG_FPRINTF(stderr, "RRJ: unresolved menu update function %08X\n", address);
            abort();
    }
}

static int render_ot(char **argv)
{
    char *cursor = argv[4], *end;
    if (!read_file(argv[2], DRAM, sizeof(DRAM)) || !read_file(argv[3], VRAM, sizeof(VRAM)))
        return 2;
    gpu_begin();
    do
    {
        uint32_t address = (uint32_t)strtoul(cursor, &end, 16);
        if (end == cursor || !rrj_gpu_draw_ot(address))
            return 2;
        cursor = *end == ',' ? end + 1 : end;
    } while (*cursor);
    return gpu_save_frame(argv[7]) ? 0 : 2;
}

/* Integration check: translated wrapper -> AA LoadImage -> real raster VRAM.
 * Covers odd row lengths, bottom/right edge, and an entire VRAM transfer. */
static int upload_smoke(RRJMemory *memory)
{
    static const uint16_t rectangles[3][4] = {{17, 33, 5, 3}, {1023, 511, 1, 1}, {0, 0, 1024, 512}};
    unsigned test, x, y, i;
    xport_set_headless(1);
    for (test = 0; test < 3; ++test)
    {
        unsigned rx = rectangles[test][0], ry = rectangles[test][1];
        unsigned w = rectangles[test][2], h = rectangles[test][3];
        memset(VRAM, 0x5A, sizeof(VRAM));
        w_u32(0x800F0004, 0x800FFFF0);
        for (i = 0; i < 4; ++i)
            w_u16(0x800F0010 + i * 2, rectangles[test][i]);
        for (i = 0; i < w * h; ++i)
            w_u16(0x80100000 + i * 2, i * 97 + 3);
        if (sub_80065768(0x800F0000) != 0)
            return 2;
        for (y = 0; y < 512; ++y)
            for (x = 0; x < 1024; ++x)
            {
                uint16_t expected = 0x5A5A;
                if (x >= rx && x < rx + w && y >= ry && y < ry + h)
                    expected = (uint16_t)(((y - ry) * w + x - rx) * 97 + 3);
                if (VRAM[y * 1024 + x] != expected)
                    return 2;
            }
    }
    RRJ_DEBUG_PUTS("GPU upload integration: 3 transfers, 1572864 VRAM word checks PASS");
    for (test = 0; test < 2; ++test)
    {
        uint32_t id = test ? 0x8001 : 1, expected_return = test ? 0x800D76D0 : 0;
        memset(VRAM, 0x5A, sizeof(VRAM));
        w_u32(0x8005B2F8, 0x801E0000);
        w_u32(0x801E0030, 1);
        xport_guest_fill(0x800533B4, 0, 16);
        for (i = 0; i < 24; ++i)
            w_u32(0x800D9270 + 48 * i, 0xFFFFFFFF);
        w_u32(0x800D9268, 0xFFFFFFFF);
        for (i = 0; i < 8192; ++i)
        {
            w_u16(0x80100000 + 2 * i, i * 97 + 3);
            w_u16(0x80104000 + 2 * i, i * 31 + 7);
        }
        if (sub_8002227C(0, id, 0x80100000, 0x80104000, 0) != expected_return)
            return 2;
        if (r_u32(0x800D9268) != 0 || r_u32(0x800D9270) != id)
            return 2;
        if (test && r_u16(0x800D76D6) != 0)
            return 2;
        for (y = 0; y < 512; ++y)
            for (x = 0; x < 1024; ++x)
            {
                uint16_t expected = 0x5A5A;
                if (x < 64 && y < 128)
                    expected = (uint16_t)((y * 64 + x) * 97 + 3);
                if (!test && x < 64 && y >= 128 && y < 256)
                    expected = (uint16_t)(((y - 128) * 64 + x) * 31 + 7);
                if (VRAM[y * 1024 + x] != expected)
                    return 2;
            }
    }
    RRJ_DEBUG_PUTS("Race image upload 8002227C: 3 transfers, 1048576 VRAM word checks PASS");
    return 0;
}

static int input_smoke(const char *fixture)
{
    uint32 buttons = 0, state, frame, events = 0;
    if (!read_file(fixture, DRAM, sizeof(DRAM)))
        return 2;
    xport_set_headless(1);
    PadInit(0);
    state = r_u32(0x8005B2F8);
    w_u8(state, 2);
    w_u32(state + 52, 1);
    xport_guest_fill(0x800D6DE0, 0, 768);
    /* Make the first press a single-click event, as in initialized game data. */
    w_u8(0x800D6E09, 30);
    for (frame = 0; frame < 80; ++frame)
    {
        unsigned expected = frame == 0 ? 1 : (frame >= 32 && frame < 70 && (frame - 32) % 3 == 0) ? 255 : 0;
        unsigned actual;
        buttons = frame < 70 ? PADLup : 0;
        xport_input_override(1, buttons);
        w_u32(state + 12, frame + 1);
        rrj_input_read();
        rrj_input_latch();
        actual = r_u8(0x800D7152);
        if (actual != expected)
        {
            RRJ_DEBUG_FPRINTF(stderr, "input frame %u: %u != %u\n", frame, actual, expected);
            return 2;
        }
        if (actual)
            ++events;
        if (r_u8(0x800D6E0A) != 0 || r_u32(0x800D712C) != (frame < 70 ? (uint32)PADLup : 0u))
            return 2;
    }
    for (frame = 0; frame < 15; ++frame)
    {
        uint32 record = 0x800D6DF4 + 8 * frame;
        xport_guest_fill(0x800D6DE0, 0, 192);
        w_u8(record + 5, 30);
        buttons = r_u32(0x80052658 + 4 * frame);
        xport_input_override(1, buttons);
        w_u32(state + 12, 100 + frame);
        rrj_input_read();
        rrj_input_latch();
        if (r_u8(0x800D7142 + 8 * frame) != 1 || r_u32(0x800D712C) != buttons)
            return 2;
        buttons = 0;
        xport_input_override(1, buttons);
        rrj_input_read();
        rrj_input_latch();
        if (r_u8(0x800D7142 + 8 * frame) != 0)
            return 2;
    }
    RRJ_DEBUG_PRINTF("Host input integration: 80 frames, %u events, 15 mappings, press/repeat/release PASS\n", events);
    return 0;
}

static int audio_smoke(RRJMemory *m, const char *memory_path, const char *spu_path, const char *output)
{
    static uint8_t samples[0x80000];
    static int16_t pcm[44100 * 2];
    uint32_t handle, i, nonzero = 0, peak = 0;
    if (!read_file(memory_path, DRAM, sizeof(DRAM)) || !read_file(spu_path, samples, sizeof samples))
        return 2;
    rrj_spu_initialize(samples);
    handle = sub_8007EAC0(0);
    if (!handle)
        return 2;
    (void)sub_8001EE94();
    spu_render(pcm, 44100);
    for (i = 0; i < 44100 * 2; ++i)
    {
        uint32_t v = (uint32_t)(pcm[i] < 0 ? -(int32_t)pcm[i] : pcm[i]);
        nonzero += v != 0;
        if (v > peak)
            peak = v;
    }
    if (!write_file(output, pcm, sizeof pcm) || nonzero < 100 || !peak)
        return 2;
    sub_8001EB44(1u << (handle >> 27));
    (void)sub_8001EE94();
    RRJ_DEBUG_PRINTF("SPU menu sound: frames=44100 nonzero=%u peak=%u handle=%08X PASS\n", nonzero, peak, handle);
    return 0;
}

/* WIP integrated State1 runner. Classified missing calls support discovery. */
static uint32_t live_music_read_failures;

#if defined(LOCKSTEP_DEBUG)
sint32 rrj_game_checkpoint_io(FILE *file, sint32 load)
{
    uint32 state[4];
    if (!file)
        return 0;
    if (load)
    {
        if (fread(state, sizeof(state), 1, file) != 1)
            return 0;
        live_origin_x = ((int32_t)(uint32_t)(state[0]));
        live_origin_y = ((int32_t)(uint32_t)(state[1]));
        live_race_draw_calls = state[2];
        live_music_read_failures = state[3];
        return 1;
    }
    state[0] = (uint32)live_origin_x;
    state[1] = (uint32)live_origin_y;
    state[2] = live_race_draw_calls;
    state[3] = live_music_read_failures;
    return fwrite(state, sizeof(state), 1, file) == 1;
}
#endif

void live_decoder_restore(void)
{
    uint32_t input = r_u32(0x80055EE0u);
    uint32_t output = r_u32(0x80055EE4u);
    DecDCTCallback prior;
    if (input || (output && output != 0x80062674u && output != 0x80013AE4))
    {
        RRJ_DEBUG_FPRINTF(stderr, "Missing saved decoder callbacks: input=%08X output=%08X\n", input, output);
        abort();
    }
    prior = DecDCToutCallback(output == 0x80013AE4 ? sub_80013AE4 : output ? rrj_resource_decoder_output : NULL);
    if (prior && prior != rrj_resource_decoder_output && prior != sub_80013AE4)
    {
        DecDCToutCallback(prior);
        RRJ_DEBUG_FPRINTF(stderr, "Unmapped existing decoder output callback\n");
        abort();
    }
    prior = DecDCTinCallback(NULL);
    if (prior)
    {
        DecDCTinCallback(prior);
        RRJ_DEBUG_FPRINTF(stderr, "Unmapped existing decoder input callback\n");
        abort();
    }
}

static uint32_t complete_resource_read(uint32_t first, uint32_t mode, uint32_t descriptor)
{
    return sub_800781A8(descriptor);
}

static uint32_t complete_transfer_latch(uint32_t unused)
{
    return sub_8001E928();
}

static uint32_t complete_transfer_stream(uint32_t unused)
{
    return sub_80021884();
}

/* Adapt original callbacks which do not consume the response argument */
static uint32_t rrj_cd_preview_sync(uint32_t event, uint32_t response)
{
    return sub_80061584(event);
}

static uint32_t rrj_cd_preview_ready(uint32_t event, uint32_t response)
{
    return sub_800611F4(event);
}

static uint32_t rrj_cd_restore_sync(uint32_t event, uint32_t response)
{
    return sub_80045C74();
}

RRJ_CD_CALLBACK rrj_cd_callback(uint32_t address)
{
    switch (address)
    {
        case 0x8001578C:
            return sub_8001578C;
        case 0x80015860:
            return sub_80015860;
        case 0x80061584:
            return rrj_cd_preview_sync;
        case 0x800611F4:
            return rrj_cd_preview_ready;
        case 0x80045C74:
            return rrj_cd_restore_sync;
        default:
            /* TODO Bind additional original CD callback targets */
            RRJ_DEBUG_FPRINTF(stderr, "RRJ: unresolved CD callback %08X\n", address);
            abort();
    }
}

RRJFileFunction rrj_file_function(uint32_t address)
{
    switch (address)
    {
        case 0x800150ECu:
            return sub_800150EC;
        case 0x800602ECu:
            return sub_800602EC;
        case 0x80015724u:
            return sub_80015724;
        case 0x800151ECu:
            return sub_800151EC;
        case 0x80022EECu:
            return sub_80022EEC;
        default:
            RRJ_DEBUG_FPRINTF(stderr, "RRJ: unresolved file completion %08X\n", address);
            abort();
    }
}

static uint32_t complete_music_data(uint32_t first, uint32_t mode, uint32_t token)
{
    return sub_8007E958();
}

RRJReadCompleteFunction rrj_read_complete_function(uint32_t address)
{
    switch (address)
    {
        case 0x80030894u:
            return sub_80030894;
        case 0x8007E958u:
            return complete_music_data;
        case 0x800781A8u:
            return complete_resource_read;
        case 0x8007F028u:
            return sub_8007F028;
        default:
            RRJ_DEBUG_FPRINTF(stderr, "RRJ: unresolved read completion %08X\n", address);
            abort();
    }
}

RRJTransferCompleteFunction rrj_transfer_complete_function(uint32_t address)
{
    switch (address)
    {
        case 0x80016464u:
            return sub_80016464;
        case 0x8001E928u:
            return complete_transfer_latch;
        case 0x80021884u:
            return complete_transfer_stream;
        case 0x8007F0D0u:
            return sub_8007F0D0;
        case 0x8007E9D0u:
            return sub_8007E9D0;
        default:
            RRJ_DEBUG_FPRINTF(stderr, "RRJ: unresolved transfer completion %08X\n", address);
            abort();
    }
}

static void native_display_wait_service(void)
{
#if defined(LOCKSTEP_DEBUG)
    if (xport_run_mode() == RUN_MODE_LOCKSTEP_REPLAY)
    {
        (void)rrj_lockstep_poll(rrj_host_context());
        return;
    }
#endif
    (void)VSync(0);
}

static int save_live_frame(const char *ram_path)
{
    char path[1024];
    if (snprintf(path, sizeof path, "%s.ppm", ram_path) < 0 || strlen(ram_path) + 5 >= sizeof path)
        return 0;
    return gpu_save_frame(path);
}

static FILE *full_gpu_output;
static FILE *full_frame_output;
static jmp_buf full_stop;
static uint32_t full_resume[12];
static uint32_t full_frame_events;
static uint32_t full_intervals;
static uint32_t full_limit;
static uint64_t full_gpu_bytes;
static int full_failed;
static FILE *full_ot_schedule;
static uint32_t full_ot_next_row[2];
static uint32_t full_ot_unread;
static uint32_t full_ot_has_next;
static uint32_t full_ot_consumed;
static uint32_t full_current_frame;
static uint32_t full_ot_pending[64][2];
static uint32_t full_ot_pending_count;
static XportMemorySnapshotWriter full_memory_writer;
static int full_memory_enabled;

static int full_memory_start(void)
{
    const char *path = RRJ_DEBUG_GETENV("XPORT_MEMORY_OUTPUT");
    const char *manifest = RRJ_DEBUG_GETENV("XPORT_MEMORY_MANIFEST");
    const char *interval = RRJ_DEBUG_GETENV("XPORT_MEMORY_KEYFRAME_INTERVAL");
    uint32_t keyframes = interval ? (uint32_t)strtoul(interval, NULL, 10) : 300u;
    full_memory_enabled = 0;
    if (!path && !manifest)
        return 1;
    if (!path || !manifest || !keyframes || !xport_memory_snapshot_open(&full_memory_writer, path, manifest, keyframes) || !xport_memory_snapshot_register_psx_regions(&full_memory_writer))
    {
        xport_memory_snapshot_abort(&full_memory_writer);
        return 0;
    }
    full_memory_enabled = 1;
    return 1;
}

static int full_path(char path[1024], const char *directory, const char *name)
{
    int length = snprintf(path, 1024, "%s/%s", directory, name);
    return length >= 0 && length < 1024;
}

static int full_read(const char *directory, const char *name, void *data, size_t size)
{
    char path[1024];
    return full_path(path, directory, name) && read_file(path, data, size);
}

static void full_gpu_transfer(void *context, uint32 operation, uint32 address, const uint32 *words, uint32 count)
{
    uint64_t tick = 0;
    uint32_t header[3] = {operation, address, count};
    if (!full_gpu_output || fwrite(&tick, sizeof(tick), 1, full_gpu_output) != 1 || fwrite(header, sizeof(header), 1, full_gpu_output) != 1 || fwrite(words, sizeof(uint32), count, full_gpu_output) != count)
    {
        full_failed = 1;
        longjmp(full_stop, 1);
    }
    ++full_frame_events;
    full_gpu_bytes += 20u + (uint64_t)count * 4u;
}

static int full_ot_read_next(void)
{
    if (!full_ot_unread)
    {
        full_ot_has_next = 0;
        return 1;
    }
    if (fread(full_ot_next_row, sizeof(full_ot_next_row), 1, full_ot_schedule) != 1)
        return 0;
    --full_ot_unread;
    full_ot_has_next = 1;
    return 1;
}

static sint32 full_ot_submit(void *context, uint32 address, uint32 copy_bytes)
{
    uint32_t frame;
    if (!full_ot_has_next || full_ot_next_row[1] != address || full_ot_next_row[0] < full_current_frame || full_ot_pending_count >= 64)
        return 0;
    frame = full_ot_next_row[0];
    ++full_ot_consumed;
    if (!full_ot_read_next())
        return 0;
    if (frame == full_current_frame)
        return gpu_execute_linked_dma(address);
    full_ot_pending[full_ot_pending_count][0] = frame;
    full_ot_pending[full_ot_pending_count][1] = address;
    ++full_ot_pending_count;
    return 1;
}

static void full_ot_advance(void)
{
    uint32_t consumed = 0;
    while (consumed < full_ot_pending_count && full_ot_pending[consumed][0] == full_current_frame)
    {
        if (!gpu_execute_linked_dma(full_ot_pending[consumed][1]))
        {
            full_failed = 1;
            longjmp(full_stop, 1);
        }
        ++consumed;
    }
    if (consumed)
    {
        memmove(full_ot_pending, full_ot_pending + consumed, (full_ot_pending_count - consumed) * sizeof(full_ot_pending[0]));
        full_ot_pending_count -= consumed;
    }
    if ((full_ot_pending_count && full_ot_pending[0][0] < full_current_frame) || (full_ot_has_next && full_ot_next_row[0] < full_current_frame))
    {
        full_failed = 1;
        longjmp(full_stop, 1);
    }
}

static sint32 full_vblank(void *context)
{
    uint32_t row[2] = {full_resume[2] + full_intervals, full_frame_events};
    XportMemorySnapshotBoundary boundary = {full_resume[2] + full_intervals, full_current_frame, full_ot_consumed, 1u};
    if (full_memory_enabled && !xport_memory_snapshot_capture_psx(&full_memory_writer, boundary))
    {
        full_failed = 1;
        longjmp(full_stop, 1);
    }
    if (full_ot_has_next && full_ot_next_row[0] == full_current_frame)
    {
        full_failed = 1;
        longjmp(full_stop, 1);
    }
    if (!full_frame_output || fwrite(row, sizeof(row), 1, full_frame_output) != 1 || fwrite(&full_gpu_bytes, sizeof(full_gpu_bytes), 1, full_frame_output) != 1 || RRJ_DEBUG_FFLUSH(full_frame_output) || RRJ_DEBUG_FFLUSH(full_gpu_output))
    {
        full_failed = 1;
        longjmp(full_stop, 1);
    }
    if (++full_intervals >= full_limit)
        longjmp(full_stop, 1);
    full_frame_events = 0;
    ++full_current_frame;
    psx_vblank_signal();
    full_ot_advance();
    return !psx_vblank_failed();
}

static int full_origin_probe(RRJMemory *m)
{
    static uint8_t spu[524288];
    uint32_t gte[64];
    GpuRasterState raster;
    PsxIrqState irq;
    const char *directory = RRJ_DEBUG_GETENV("RRJ_FULL_ORIGIN");
    const char *gpu_path = RRJ_DEBUG_GETENV("RRJ_FULL_GPU_OUTPUT");
    const char *frame_path = RRJ_DEBUG_GETENV("RRJ_FULL_FRAMES_OUTPUT");
    const char *limit = RRJ_DEBUG_GETENV("RRJ_FULL_INTERVALS");
    const char *ot_path = RRJ_DEBUG_GETENV("RRJ_FULL_OT_SCHEDULE");
    uint32_t ot_header[6];
    uint8_t ot_digest[32];
    if (!directory || !gpu_path || !frame_path || !full_read(directory, "resume.bin", full_resume, sizeof(full_resume)) || full_resume[0] != 0x31464A52 || full_resume[1] != 1 || full_resume[2] != 3012 || full_resume[3] != 0x8001C414 || full_resume[4] != 0x8001C418 || full_resume[6] != 0x8008034C || full_resume[7] != 0x8001C410 || !full_read(directory, "ram.bin", DRAM, sizeof(DRAM)) || !full_read(directory, "scratchpad.bin", SCRATCHPAD, sizeof(SCRATCHPAD)) || !full_read(directory, "gte.bin", gte, sizeof(gte)) || !full_read(directory, "vram.bin", VRAM, sizeof(VRAM)) || !full_read(directory, "spu-ram.bin", spu, sizeof(spu)) || !full_read(directory, "raster.bin", &raster, sizeof(raster)) || !full_read(directory, "irq.bin", &irq, sizeof(irq)) || !psx_irq_validate(&irq) || !psx_irq_pending(&irq) || !gpu_import_raster(&raster, VRAM, sizeof(VRAM)))
        return 22;
    full_limit = limit ? (uint32_t)strtoul(limit, NULL, 10) : 1;
    if (!full_limit)
        return 22;
    full_gpu_output = xport_fopen(gpu_path, "wb");
    full_frame_output = xport_fopen(frame_path, "wb");
    full_ot_schedule = ot_path ? xport_fopen(ot_path, "rb") : NULL;
    if (!full_gpu_output || !full_frame_output || !full_ot_schedule || fread(ot_header, sizeof(ot_header), 1, full_ot_schedule) != 1 || fread(ot_digest, sizeof(ot_digest), 1, full_ot_schedule) != 1 || ot_header[0] != 0x31544F52 || ot_header[1] != 1 || ot_header[3] != full_resume[2] || ot_header[4] != 5773 || ot_header[5] != 8)
    {
        if (full_gpu_output)
            fclose(full_gpu_output);
        if (full_frame_output)
            fclose(full_frame_output);
        if (full_ot_schedule)
            fclose(full_ot_schedule);
        return 23;
    }
    restore_checkpoint_gte(gte);
    rrj_spu_initialize(spu);
    if (!full_memory_start())
        return 22;
    xport_set_headless(1);
    full_frame_events = full_intervals = 0;
    full_gpu_bytes = 0;
    full_failed = 0;
    full_ot_unread = ot_header[2];
    full_ot_has_next = full_ot_consumed = full_ot_pending_count = 0;
    full_current_frame = full_resume[2];
    if (!full_ot_read_next())
        full_failed = 1;
    psx_vblank_bind(full_vblank, NULL, r_u32(0x8005B46C));
    gpu_set_transfer_observer(full_gpu_transfer, NULL);
    gpu_bind_linked_submit(full_ot_submit, NULL);
    if (setjmp(full_stop) == 0)
    {
        (void)sub_80064C30();
        (void)sub_8001C408();
        w_u32(0x80088C44, 0);
        rrj_menu_finish_iteration();
        while (r_u8(r_u32(0x8005B2F8)) == 2)
        {
            rrj_menu_iteration();
        }
        full_failed = 1;
    }
    gpu_bind_linked_submit(NULL, NULL);
    gpu_set_transfer_observer(NULL, NULL);
    if (fclose(full_gpu_output) || fclose(full_frame_output) || fclose(full_ot_schedule))
        full_failed = 1;
    if (full_memory_enabled && !xport_memory_snapshot_finish(&full_memory_writer))
    {
        xport_memory_snapshot_abort(&full_memory_writer);
        full_failed = 1;
    }
    full_memory_enabled = 0;
    full_gpu_output = full_frame_output = full_ot_schedule = NULL;
    RRJ_DEBUG_PRINTF("full_origin_probe frame=%u intervals=%u submissions=%u failed=%d full_convergence=0\n", full_resume[2], full_intervals, full_ot_consumed, full_failed);
    return full_failed ? 24 : 25;
}

static int menu_run(RRJMemory *m, char **argv, int navigation)
{
    m->display_wait_service = native_display_wait_service;
    uint32_t buttons = 0, last_selection = 0xffffffff;
    int trace = navigation == 1 || RRJ_DEBUG_GETENV("RRJ_TRACE_INPUT") != NULL;
    static uint8_t spu[524288];
    unsigned frame, limit = (unsigned)strtoul(argv[5], NULL, 10);
    if (!read_file(argv[2], DRAM, sizeof(DRAM)) || !read_file(argv[3], VRAM, sizeof(VRAM)) || !read_file(argv[4], spu, sizeof spu))
        return 2;
    xport_set_headless(navigation == 2);
    if (ResetGraph(0) < 0)
        return 2;
    DecDCTReset(0);
    live_decoder_restore();
    PadInit(0);
    rrj_spu_initialize(spu);
    live_spu_restore();
    if (!xport_audio_init())
    {
        RRJ_DEBUG_FPRINTF(stderr, "Windows audio device initialization failed\n");
        return 2;
    }
    if (!rrj_trace_runtime_init_live(m))
        return 2;
    rrj_trace_runtime_set_live_poll(rrj_spu_completion_service);
    rrj_restore_native_vblank();
    for (frame = 0; frame < limit && !xport_isquit(); ++frame)
    {
        rrj_wip_frame(frame);
        if (r_u8(r_u32(0x8005B2F8)) != 2)
            break;
        if (navigation == 1)
            buttons = frame >= 5 && frame < 10 ? PADLdown : frame >= 20 && frame < 25 ? PADLup : 0;
        if (navigation)
            xport_input_override(1, buttons);
        gpu_begin();
        rrj_menu_iteration();
        if (trace)
        {
            uint32_t menu = r_u32(r_u32(0x8009C68C) + 4 * r_u16(0x8009C5D0));
            uint32_t selection = r_u16(menu + 4);
            if (selection != last_selection)
            {
                RRJ_DEBUG_PRINTF("navigation frame=%u selection=%u\n", frame, selection);
                last_selection = selection;
                RRJ_DEBUG_FFLUSH(stdout);
            }
        }
        if (!gpu_present())
        {
            if (xport_isquit())
                break;
            xport_audio_shutdown();
            return 2;
        }
    }
    if (!xport_isquit() && r_u8(r_u32(0x8005B2F8)) != 2)
    {
        int result;

        /* Finish the menu owner before resuming the resident race loader */
        (void)sub_8007F37C();
        (void)sub_80080220();
        m->display_wait_service = trace_display_wait_service;
        live_race_draw_calls = 0;
        (void)rrj_resume_after_menu();
        result = rrj_trace_runtime_finish(0);
        xport_audio_shutdown();
        return result;
    }
    xport_audio_shutdown();
    RRJ_DEBUG_PRINTF("audio submitted=%u nonzero=%u peak=%u active=%u overruns=%u\n", g_xport_audio_submitted_buffers, g_xport_audio_nonzero_buffers, g_xport_audio_peak, g_xport_audio_backend_active, g_xport_audio_callback_overruns);
    RRJ_DEBUG_PRINTF("WIP native menu: iterations=%u menu=%u vblanks=%u; audio device stopped\n", frame, (unsigned)r_u16(0x8009C5D0), r_u32(0x8005B46C));
    RRJ_DEBUG_PRINTF("WIP CD music reads failed=%u\n", live_music_read_failures);
    return !argv[6] || (write_file(argv[6], DRAM, sizeof(DRAM)) && save_live_frame(argv[6])) ? 0 : 2;
}

#ifndef NDEBUG
static int rrj_main(int argc, char **argv)
{
    RRJMemory memory = {0};
    uint8_t argument_bytes[36], result_bytes[8];
    uint32_t function, args[9] = {0}, argument_count;
    uint64_t result;
    unsigned i;
    for (i = 1; i < (unsigned)argc;)
    {
        if (strcmp(argv[i], "--uncapped") == 0)
        {
            xport_set_headless(1);
            memmove(&argv[i], &argv[i + 1], (argc - i) * sizeof(argv[0]));
            --argc;
        }
        else
            ++i;
    }
    if (!rrj_wip_options(&argc, argv, &memory))
        return 2;
    memory.display_wait_service = trace_display_wait_service;
    rrj_memory_bind(&memory);
    if (argc == 4 && strcmp(argv[1], "--checkpoint-abi") == 0)
    {
        uint32_t ordinal;
        uint8_t output[106];
        const RRJNativeCheckpointABI *abi = &memory.checkpoint_abi;
        const RRJNativeCallFrame *frame = &abi->frame;
        uint32_t fields[25];

        xport_set_headless(1);
        if (!load_race_checkpoint(&memory, argv[2], &ordinal) || !memory.checkpoint_abi_valid)
            return 22;
        fields[0] = 0x31414a52;
        fields[1] = 1;
        fields[2] = abi->checkpoint_pc;
        fields[3] = abi->checkpoint_npc;
        fields[4] = abi->global_pointer;
        fields[5] = frame->stack_pointer;
        fields[6] = frame->return_address;
        fields[7] = frame->return_value;
        fields[8] = frame->secondary_result;
        fields[9] = frame->preserved_s0;
        fields[10] = frame->preserved_s1;
        fields[11] = frame->preserved_s2;
        fields[12] = frame->preserved_s3;
        fields[13] = frame->preserved_s4;
        for (i = 0; i < 4; ++i)
            fields[14 + i] = abi->arguments[i];
        fields[18] = abi->frame_pointer;
        fields[19] = abi->multiply_high;
        fields[20] = abi->multiply_low;
        fields[21] = abi->preserved_s5;
        fields[22] = abi->preserved_s6;
        fields[23] = abi->preserved_s7;
        fields[24] = memory.cpu_status;
        for (i = 0; i < 25; ++i)
            xport_store_le32(output + 4 * i, fields[i]);
        memcpy(output + 100, abi->pipeline_flags, 6);
        return write_file(argv[3], output, sizeof(output)) ? 0 : 23;
    }
    if (argc == 2 && strcmp(argv[1], "--trace-replay") == 0)
        return trace_replay(&memory);
    if (argc == 2 && strcmp(argv[1], "--full-origin-probe") == 0)
        return full_origin_probe(&memory);
    if (argc == 7 && strcmp(argv[1], "--menu-run") == 0)
        return menu_run(&memory, argv, 0);
    if (argc == 7 && strcmp(argv[1], "--menu-idle") == 0)
        return menu_run(&memory, argv, 2);
    if (argc == 7 && strcmp(argv[1], "--menu-nav") == 0)
        return menu_run(&memory, argv, 1);
    if (argc == 5 && strcmp(argv[1], "--audio-smoke") == 0)
        return audio_smoke(&memory, argv[2], argv[3], argv[4]);
    if (argc == 3 && strcmp(argv[1], "--input-smoke") == 0)
        return input_smoke(argv[2]);
    if (argc == 2 && strcmp(argv[1], "--gpu-upload-smoke") == 0)
        return upload_smoke(&memory);
    if (argc == 2 && strcmp(argv[1], "--first-race") == 0)
        return first_race_run(&memory, 0);
    if (argc == 2 && strcmp(argv[1], "--first-race-smoke") == 0)
        return first_race_run(&memory, 1);
    if (argc == 8 && strcmp(argv[1], "--render-ot") == 0)
        return render_ot(argv);
    if ((argc == 3 || argc == 4) && strcmp(argv[1], "--platform-smoke") == 0)
        return rrj_platform_smoke((unsigned)strtoul(argv[2], NULL, 10), argc == 4 && strcmp(argv[3], "--headless") == 0);
    if (argc == 1)
    {
        char *menu_args[] = {argv[0], "--menu-run", "../status/menu/state1-menu-ram.bin", "../status/menu/state1-vram.bin", "../status/menu/state1-spu.bin", "4294967295", NULL};
        return menu_run(&memory, menu_args, 0);
    }
    if (argc == 2 && strcmp(argv[1], "--version") == 0)
    {
        RRJ_DEBUG_PUTS("Road Rash: Jailbreak - native C State1 menu, Debug x86; CD/card/video WIP.");
        return 0;
    }
    if (argc == 10 && strcmp(argv[1], "--probe-scratch") == 0)
    {
        if (!read_file(argv[8], SCRATCHPAD, sizeof(SCRATCHPAD)))
            return 2;
    }
    else if ((argc != 7 && argc != 8) || (strcmp(argv[1], "--probe") != 0 && strcmp(argv[1], "--probe8") != 0 && strcmp(argv[1], "--probe9") != 0))
        return 2;
    argument_count = strcmp(argv[1], "--probe9") == 0 ? 9u : strcmp(argv[1], "--probe8") == 0 ? 8u : 6u;
    function = (uint32_t)strtoul(argv[2], NULL, 16);
    if (!read_file(argv[3], DRAM, sizeof(DRAM)) || !read_file(argv[4], argument_bytes, 4u * argument_count))
        return 2;
    for (i = 0; i < argument_count; ++i)
        args[i] = xport_load_le32(argument_bytes + i * 4);
    if (argc == 8 || argc == 10)
    {
        memory.sdk_user = xport_fopen(argv[7], "w");
        if (!memory.sdk_user)
            return 2;
    }
    switch (function)
    {
        case 0x80037338:
            result = sub_80037338();
            break;
        case 0x80012524:
            race_step_reply = args[0];
            result = sub_80012524();
            break;
        case 0x8002305C:
            memcpy(race_pause_replies, args, sizeof(race_pause_replies));
            result = sub_8002305C();
            break;
        case 0x800324CC:
            result = sub_800324CC(args[0], args[1], args[2]);
            break;
        case 0x800335B4:
            result = sub_800335B4(args[0], args[1], args[2]);
            break;
        case 0x800329BC:
            result = sub_800329BC(args[0], args[1]);
            break;
        case 0x80032338:
            result = sub_80032338(args[0], args[1]);
            break;
        case 0x8003234C:
            result = sub_8003234C(args[0], args[1]);
            break;
        case 0x800325BC:
            result = sub_800325BC(args[0], args[1]);
            break;
        case 0x80013204:
            result = sub_80013204(args[0], args[1]);
            break;
        case 0x80013360:
            result = sub_80013360(args[0], args[1]);
            break;
        case 0x80012BA8:
            result = sub_80012BA8(args[0], args[1], args[2]);
            break;
        case 0x80013110:
            result = sub_80013110(args[0], args[1], args[2], args[3], args[4]);
            break;
        case 0x80013294:
            result = sub_80013294(args[0], args[1], args[2]);
            break;
        case 0x8001339C:
            result = sub_8001339C(args[0], args[1]);
            break;
        case 0x800135E8:
            result = sub_800135E8(args[0], args[1]);
            break;
        case 0x80032A20:
            result = sub_80032A20(args[0], args[1], args[2], args[3], args[4], args[5]);
            break;
        case 0x80039A08:
            result = sub_80039A08(args[0]);
            break;
        case 0x8003CFCC:
            result = sub_8003CFCC(args[0], args[1]);
            break;
        case 0x80093E6C:
            result = sub_80093E6C();
            break;
        case 0x80093ED4:
            sub_80093ED4(args[0], args[1]);
            result = 0;
            break;
        case 0x80093F94:
            sub_80093F94(args[0], args[1]);
            result = 0;
            break;
        case 0x800C2FF4:
            result = sub_800C2FF4(args[0], args[1], args[2]);
            break;
        case 0x80012858:
            result = sub_80012858(args[0], args[1]);
            break;
        case 0x80093FE4:
            sub_80093FE4(args[0]);
            result = 0;
            break;
        case 0x800A0708:
            result = sub_800A0708(args[0]);
            break;
        case 0x8001FC58:
            result = sub_8001FC58();
            break;
        case 0x80039C38:
            result = sub_80039C38(args[0]);
            break;
        case 0x8003F580:
            result = sub_8003F580(args[0], args[1]);
            break;
        case 0x8003F5D0:
            result = sub_8003F5D0(args[0]);
            break;
        case 0x8003C840:
            result = sub_8003C840(args[0], args[1], args[2]);
            break;
        case 0x8003C948:
            result = sub_8003C948(args[0], args[1], args[2]);
            break;
        case 0x8003F408:
            result = sub_8003F408(args[0], args[1]);
            break;
        case 0x8003CCA0:
            if (args[5])
            {
                uint32_t candidates[24];
                for (i = 0; i < 24; ++i)
                    candidates[i] = r_u32(args[1] + 4 * i);
                result = sub_8003CCA0(args[0], candidates, args[2]);
            }
            else
                result = sub_8003CCA0(args[0], xport_guest_cptr(args[1], 32 * args[2]), args[2]);
            break;
        case 0x80039048:
            result = sub_80039048(args[0], xport_guest_ptr(args[1], 96), xport_guest_ptr(args[2], 12), args[3], xport_guest_ptr(args[4], 32), xport_guest_ptr(args[5], 4));
            break;
        case 0x80039BB0:
            result = sub_80039BB0(args[0], args[1], xport_guest_ptr(args[2], 4), args[3]);
            break;
        case 0x8003775C:
            result = sub_8003775C(args[0], args[1] ? xport_guest_ptr(args[1], 32) : NULL, args[2]);
            break;
        case 0x80037A30:
            result = sub_80037A30(args[0], xport_guest_ptr(args[1], 32), xport_guest_ptr(args[2], 96), xport_guest_ptr(args[3], 12), args[4]);
            break;
        case 0x80037FBC:
            result = sub_80037FBC(args[0], xport_guest_ptr(args[1], 32), xport_guest_ptr(args[2], 96), xport_guest_ptr(args[3], 12), args[4]);
            break;
        case 0x8003A5F4:
        {
            uint32_t id;
            result = sub_8003A5F4(args[0], &id, args[2]);
            w_u32(args[1], id);
            break;
        }
        case 0x8003A9D8:
            result = sub_8003A9D8(args[0]);
            break;
        case 0x8003DCB8:
            result = sub_8003DCB8(args[0], args[1]);
            break;
        case 0x8003B61C:
            result = sub_8003B61C(args[0]);
            break;
        case 0x8003B1C4:
            sub_8003B1C4(args[0], args[1], args[2]);
            result = 0;
            break;
        case 0x8003AF9C:
            sub_8003AF9C(args[0], args[1], args[2]);
            result = 0;
            break;
        case 0x8003B024:
            sub_8003B024(args[0], args[1], args[2], args[3]);
            result = 0;
            break;
        case 0x8003F4D8:
            result = sub_8003F4D8(args[0]);
            break;
        case 0x8003DDB0:
            result = sub_8003DDB0(args[0]);
            break;
        case 0x8003DFF4:
            result = sub_8003DFF4(args[0]);
            break;
        case 0x8003DE28:
            result = sub_8003DE28(args[0], args[1], args[2] ? xport_guest_ptr(args[2], 32) : NULL, args[3]);
            break;
        case 0x8003EE68:
            result = sub_8003EE68(args[0], args[1], args[2] ? xport_guest_ptr(args[2], 32) : NULL, args[3]);
            break;
        case 0x8003E754:
            result = sub_8003E754(args[0], args[1], args[2], args[3] ? xport_guest_ptr(args[3], 32) : NULL, args[4]);
            break;
        case 0x8003701C:
            result = sub_8003701C(args[0]);
            break;
        case 0x80036B14:
            result = sub_80036B14(args[0], xport_guest_ptr(args[1], 32), args[2]);
            break;
        case 0x8003697C:
            result = sub_8003697C(args[0], args[1], args[2], args[3]);
            break;
        case 0x80036800:
            sub_80036800(args[0], args[1], args[2] ? xport_guest_ptr(args[2], 4) : NULL, args[3] ? xport_guest_ptr(args[3], 4) : NULL);
            result = 0;
            break;
        case 0x8003E45C:
            result = sub_8003E45C(args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]);
            break;
        case 0x8003ED14:
            result = sub_8003ED14(args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8]);
            break;
        case 0x8003BD2C:
            result = sub_8003BD2C(args[0], args[1], args[2], xport_guest_ptr(args[3], 4), xport_guest_ptr(args[4], 4));
            break;
        case 0x8003E61C:
            result = sub_8003E61C(args[0], args[1], args[2]);
            break;
        case 0x8003F204:
            sub_8003F204(args[0], args[1]);
            result = 0;
            break;
        case 0x8003DF54:
            sub_8003DF54(args[0], args[1], args[2]);
            result = 0;
            break;
        case 0x8003F1F0:
            result = sub_8003F1F0(args[0]);
            break;
        case 0x800B6AAC:
            result = sub_800B6AAC(xport_guest_ptr(args[0], 12), xport_guest_ptr(args[1], 6), xport_guest_ptr(args[2], 12));
            break;
        case 0x8003EB58:
            result = sub_8003EB58(args[0], args[1], args[2] ? xport_guest_ptr(args[2], 4) : NULL, args[3] ? xport_guest_ptr(args[3], 4) : NULL, args[4]);
            break;
        case 0x8003E67C:
            result = sub_8003E67C(args[0], args[1], args[2], args[3]);
            break;
        case 0x8003EF34:
            result = sub_8003EF34(args[0], args[1], args[2]);
            break;
        case 0x8003A468:
            result = sub_8003A468(args[0], args[1]);
            break;
        case 0x800951B8:
            sub_800951B8(args[0], args[1]);
            result = 0;
            break;
        case 0x800952AC:
            sub_800952AC(args[0], args[1]);
            result = 0;
            break;
        case 0x800903F4:
            result = sub_800903F4(args[0], args[1]);
            break;
        case 0x800BCA68:
            result = sub_800BCA68(xport_guest_ptr(args[0], 8), args[1], args[2]);
            break;
        case 0x800C3104:
            result = sub_800C3104(args[0], args[1]);
            break;
        case 0x800BC8DC:
            sub_800BC8DC(args[0]);
            result = 0;
            break;
        case 0x800C1014:
            result = sub_800C1014(args[0]);
            break;
        case 0x800BFD24:
            result = sub_800BFD24(args[0], args[1], args[2]);
            break;
        case 0x800C4500:
            result = sub_800C4500(args[0], args[1], args[2]);
            break;
        case 0x800C4550:
            result = sub_800C4550(args[0], args[1], args[2]);
            break;
        case 0x800958F0:
            result = sub_800958F0(args[0], args[1]);
            break;
        case 0x800BFC5C:
            sub_800BFC5C(args[0], args[1], args[2], args[3]);
            result = 0;
            break;
        case 0x800C4454:
            sub_800C4454(args[0], args[1], args[2]);
            result = 0;
            break;
        case 0x800CB84C:
            result = sub_800CB84C(args[0]);
            break;
        case 0x800CC0B0:
            result = sub_800CC0B0(args[0]);
            break;
        case 0x8008C000:
            result = sub_8008C000(args[0], args[1]);
            break;
        case 0x8008CC94:
            result = sub_8008CC94();
            break;
        case 0x80043F00:
            result = sub_80043F00(args[0]);
            break;
        case 0x8002705C:
            result = sub_8002705C(args[0], args[1]);
            break;
        case 0x80027178:
            result = sub_80027178();
            break;
        case 0x800271CC:
            result = sub_800271CC(args[0], args[1]);
            break;
        case 0x800273EC:
            result = sub_800273EC(args[0], args[1], args[2], args[3], args[4]);
            break;
        case 0x80012884:
            result = sub_80012884(args[0], args[1]);
            break;
        case 0x8001298C:
            result = sub_8001298C(args[0], args[1]);
            break;
        case 0x800C2F84:
            sub_800C2F84(args[0], args[1], args[2], args[3]);
            result = 0;
            break;
        case 0x800C3E9C:
            result = sub_800C3E9C(args[0], args[1], xport_guest_ptr(args[2], 4));
            break;
        case 0x800C37B0:
            result = sub_800C37B0(args[0], args[1]);
            break;
        case 0x8005C018:
            if (argument_count != 8)
                return 2;
            result = sub_8005C018(args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
            break;
        case 0x8005C140:
            if (argument_count != 8)
                return 2;
            result = sub_8005C140(args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]);
            break;
        case 0x8005BF6C:
            result = sub_8005BF6C(args[0], args[1], args[2], args[3], args[4]);
            break;
        case 0x8005C0B0:
            result = sub_8005C0B0(args[0], args[1], args[2], args[3], args[4]);
            break;
        case 0x8005C8F4:
            result = sub_8005C8F4(args[0]);
            break;
        case 0x8005BD74:
            result = sub_8005BD74(args[0]);
            break;
        case 0x8005CB70:
            result = sub_8005CB70(args[0], args[1], args[2]);
            break;
        case 0x800714FC:
            result = sub_800714FC(args[0], args[1], args[2]);
            break;
        case 0x8005E2BC:
            result = sub_8005E2BC(args[0], args[1]);
            break;
        case 0x8005C39C:
            result = sub_8005C39C(args[0], args[1]);
            break;
        case 0x8005BE58:
            result = sub_8005BE58(args[0]);
            break;
        case 0x8005C418:
            result = sub_8005C418(args[0]);
            break;
        case 0x8005BEF4:
            result = sub_8005BEF4(args[0]);
            break;
        case 0x8005BE0C:
            result = sub_8005BE0C(args[0]);
            break;
        case 0x80096564:
            result = sub_80096564(args[0]);
            break;
        case 0x8003662C:
        {
            uint16_t direction[3];
            uint32_t record[8], output[3];
            for (i = 0; i < 3; ++i)
                direction[i] = r_u16(args[0] + 2 * i);
            for (i = 0; i < 8; ++i)
                record[i] = r_u32(args[1] + 4 * i);
            for (i = 0; i < 3; ++i)
                output[i] = r_u32(args[2] + 4 * i);
            result = sub_8003662C(direction, record, args[1] == args[2] ? record : output);
            if (args[1] == args[2])
            {
                for (i = 0; i < 8; ++i)
                    w_u32(args[2] + 4 * i, record[i]);
            }
            else
            {
                for (i = 0; i < 3; ++i)
                    w_u32(args[2] + 4 * i, output[i]);
            }
            break;
        }
        case 0x80039AA0:
            result = sub_80039AA0(args[0]);
            break;
        case 0x800394F0:
            if (args[5])
            {
                uint32_t record[8];
                for (i = 0; i < 8; ++i)
                    record[i] = r_u32(args[0] + 4 * i);
                result = sub_800394F0(record, args[1]);
            }
            else
                result = sub_800394F0(xport_guest_ptr(args[0], 32), args[1]);
            break;
        case 0x8002E468:
            result = sub_8002E468(xport_guest_ptr(args[0], 6));
            break;
        case 0x8002E14C:
            result = sub_8002E14C(xport_guest_ptr(args[0], 12));
            break;
        case 0x8003B4B0:
            result = sub_8003B4B0(args[0], args[1]);
            break;
        case 0x8003B8F4:
            result = sub_8003B8F4(args[0]);
            break;
        case 0x80039AFC:
            result = sub_80039AFC(args[0]);
            break;
        case 0x8003A37C:
            result = sub_8003A37C(args[0], args[1]);
            break;
        case 0x8007EC30:
            result = sub_8007EC30(args[0]);
            break;
        case 0x80018440:
            result = sub_80018440(args[0], args[1]);
            break;
        case 0x80068D20:
            result = sub_80068D20(args[0], args[1], args[2]);
            break;
        case 0x80095AEC:
            result = sub_80095AEC(args[0]);
            break;
        case 0x80012838:
            result = sub_80012838(args[0], args[1], args[2], args[3]);
            break;
        case 0x80086AF8:
            result = sub_80086AF8(args[0]);
            break;
        case 0x800235B0:
            result = sub_800235B0(args[0], args[1]);
            break;
        case 0x8001E100:
            result = sub_8001E100(args[0], args[1], args[2]);
            break;
        case 0x800BCD10:
            result = sub_800BCD10(args[0]);
            break;
        case 0x80078C68:
            result = rrj_race_player_tail(args[0]);
            break;
        case 0x8008A998:
            if (args[5])
            {
                const uint32_t wide_args[8] = {args[0], args[1], 0, 0, 0, 0, 0, 0};
                result = trace_race_call(&memory, function, wide_args);
            }
            else
                result = sub_8008A998(args[0], args[1]);
            break;
        case 0x800BC7CC:
            result = sub_800BC7CC(args[0]);
            break;
        case 0x80092C7C:
            result = sub_80092C7C(args[0], args[1]);
            break;
        case 0x8002090C:
            result = sub_8002090C(args[0]);
            break;
        case 0x8002076C:
            result = sub_8002076C(args[0]);
            break;
        case 0x8002E698:
            result = sub_8002E698(xport_guest_ptr(args[0], 6), xport_guest_ptr(args[1], 6));
            break;
        case 0x8002EE50:
            result = (uint64_t)sub_8002EE50(args[0], xport_guest_ptr(args[1], 6), xport_guest_ptr(args[2], 12));
            break;
        case 0x8007F0BC:
            result = sub_8007F0BC(args[0], args[1]);
            break;
        case 0x80075EE0:
            result = sub_80075EE0((int32_t)args[0]);
            break;
        case 0x80078AA8:
            result = rrj_race_movement_block(args[0]);
            break; /* Frame block only. */
        case 0x80078AAC:
            result = rrj_race_movement_contact_block(args[0]);
            break; /* Extended frame block. */
        case 0x80078AB0:
            result = rrj_race_movement_route_block(args[0]);
            break; /* Movement/contact/route block. */
        case 0x80078AB4:
            result = rrj_race_movement_surface_block(args[0]);
            break; /* Through surface projection traversal. */
        case 0x80078AB8:
            result = rrj_race_movement_placement_block(args[0]);
            break; /* Through contact placement traversal. */
        case 0x80078ABC:
            result = rrj_race_movement_wheel_block(args[0]);
            break; /* Through all active objects' wheel/geometry update. */
        case 0x80078C58:
            result = rrj_race_activity_block();
            break; /* Actual consecutive actor/body activity passes. */
        case 0x80078AC0:
            result = args[5] ? rrj_race_movement_player_block(args[0]) : rrj_race_movement_activity_block(args[0]);
            break;
        case 0x8003E150:
            result = sub_8003E150();
            break;
        case 0x8003AE24:
            result = sub_8003AE24();
            break;
        case 0x8007504C:
            result = sub_8007504C(args[0], args[1]);
            break;
        case 0x80075B08:
            result = sub_80075B08(args[0], args[1]);
            break;
        case 0x80075628:
            result = sub_80075628(args[0], args[1], args[2]);
            break;
        case 0x8007FA4C:
            result = sub_8007FA4C(args[0]);
            break;
        case 0x800807F0:
            result = sub_800807F0(args[0], args[1]);
            break;
        case 0x800BFE58:
            result = sub_800BFE58(args[0], args[1], args[2]);
            break;
        case 0x800BF51C:
            result = sub_800BF51C(args[0]);
            break;
        case 0x8002F0F4:
            result = sub_8002F0F4(xport_guest_ptr(args[0], 12));
            break;
        case 0x8002E548:
            result = sub_8002E548(xport_guest_ptr(args[0], 12));
            break;
        case 0x8002ED94:
            result = sub_8002ED94(xport_guest_ptr(args[0], 6), xport_guest_ptr(args[1], 6), args[2]);
            break;
        case 0x8007F08C:
            result = sub_8007F08C(args[0]);
            break;
        case 0x8004CF74:
            result = sub_8004CF74(args[0]);
            break;
        case 0x8002ECB8:
            result = (uint64_t)sub_8002ECB8(xport_guest_ptr(args[0], 6), xport_guest_ptr(args[1], 6), xport_guest_ptr(args[2], 12), args[3], args[4]);
            break;
        case 0x800A7BF8:
            result = sub_800A7BF8(args[0], args[1], xport_guest_ptr(args[2], 12), xport_guest_ptr(args[3], 6), args[4]);
            break;
        case 0x800A8498:
            result = sub_800A8498(args[0], args[1], xport_guest_ptr(args[2], 4), xport_guest_ptr(args[3], 4), xport_guest_ptr(args[4], 12), xport_guest_ptr(args[5], 4));
            break;
        case 0x800B6E08:
            result = sub_800B6E08(xport_guest_cptr(args[0], 12), xport_guest_cptr(args[1], 4), args[2], args[3]);
            break;
        case 0x800B6844:
            result = sub_800B6844(xport_guest_cptr(args[0], 12), args[1], xport_guest_ptr(args[2], 6));
            break;
        case 0x8003B520:
            result = sub_8003B520();
            break;
        case 0x80071D24:
            result = sub_80071D24(args[0]);
            break;
        case 0x800723FC:
            result = sub_800723FC(args[0]);
            break;
        case 0x8002E570:
            result = sub_8002E570(xport_guest_ptr(args[0], 12), xport_guest_ptr(args[1], 12), args[2], xport_guest_ptr(args[3], 12));
            break;
        case 0x8002EB78:
            result = sub_8002EB78(xport_guest_ptr(args[0], 6), xport_guest_ptr(args[1], 6), xport_guest_ptr(args[2], 6), args[3], args[4]);
            break;
        case 0x80010028:
            result = sub_80010028(args[0], args[1]);
            break;
        case 0x8001FF3C:
            result = sub_8001FF3C(args[0]);
            break;
        case 0x80020018:
            result = sub_80020018(args[0], args[1]);
            break;
        case 0x8009DAF8:
            result = sub_8009DAF8(args[0]);
            break;
        case 0x8002820C:
            result = sub_8002820C(args[0]);
            break;
        case 0x8002A738:
            result = sub_8002A738(args[0], args[1], args[2], args[3]);
            break;
        case 0x8002847C:
            result = sub_8002847C(args[0]);
            break;
        case 0x80039F68:
            result = sub_80039F68(args[0]);
            break;
        case 0x80037524:
        {
            if (args[0])
            {
                uint32_t route_geometry[8] = {r_u32(args[0]), r_u32(args[0] + 4), r_u32(args[0] + 8), r_u32(args[0] + 12), 0, r_u32(args[0] + 20), 0, 0};
                result = sub_80037524(route_geometry, args[1]);
                w_u32(args[0] + 12, route_geometry[3]);
                w_u32(args[0] + 20, route_geometry[5]);
            }
            else
                result = sub_80037524(NULL, args[1]);
            break;
        }
        case 0x8003BE1C:
        {
            uint32_t direction;
            if (args[0] && args[2] != 0xFFFFFFFF)
            {
                const uint32_t route_geometry[8] = {r_u32(args[0]), r_u32(args[0] + 4)};
                result = sub_8003BE1C(route_geometry, args[1], args[2], &direction);
                if (result)
                    w_u32(args[3], direction);
            }
            else
                result = sub_8003BE1C(NULL, args[1], args[2], &direction);
            break;
        }
        case 0x8003A700:
        {
            result = 0;
            if (args[0])
            {
                const uint32_t route_position[3] = {r_u32(args[1]), r_u32(args[1] + 4), r_u32(args[1] + 8)};
                uint32_t route_geometry[8] = {r_u32(args[2]), r_u32(args[2] + 4), r_u32(args[2] + 8), r_u32(args[2] + 12), r_u32(args[2] + 16), r_u32(args[2] + 20), r_u32(args[2] + 24), r_u32(args[2] + 28)};
                result = sub_8003A700(args[0], route_position, route_geometry);
                w_u32(args[2], route_geometry[0]);
                w_u32(args[2] + 4, route_geometry[1]);
                w_u32(args[2] + 8, route_geometry[2]);
                w_u32(args[2] + 12, route_geometry[3]);
                w_u32(args[2] + 16, route_geometry[4]);
                w_u32(args[2] + 20, route_geometry[5]);
                w_u32(args[2] + 24, route_geometry[6]);
                w_u32(args[2] + 28, route_geometry[7]);
            }
            break;
        }
        case 0x80039C90:
            result = sub_80039C90(args[0], args[1]);
            break;
        case 0x80039CFC:
            result = sub_80039CFC(args[0], ((args[1]) ? (const uint32_t[3]){r_u32(args[1]), 0, (uint32_t)r_u16((args[1]) + 10) << 16} : NULL));
            break;
        case 0x80039DFC:
            result = sub_80039DFC(args[0], ((args[1]) ? (const uint32_t[3]){r_u32(args[1]), 0, (uint32_t)r_u16((args[1]) + 10) << 16} : NULL));
            break;
        case 0x8003F3B4:
            result = sub_8003F3B4(args[0]);
            break;
        case 0x80039B60:
            result = sub_80039B60(args[0]);
            break;
        case 0x800374D4:
            result = sub_800374D4(args[0], args[1]);
            break;
        case 0x8004D2A4:
            (void)sub_8004D2A4(xport_guest_cptr(args[0], 6), xport_guest_ptr(args[1], 18));
            result = args[1];
            break;
        case 0x8003FA40:
            (void)sub_8003FA40(xport_guest_cptr(args[0], 18), xport_guest_cptr(args[1], 18), xport_guest_ptr(args[2], 18));
            result = args[2] + 2;
            break;
        case 0x8003FB34:
        {
            const uint16_t axis[3] = {r_u16(args[0]), r_u16(args[0] + 2), r_u16(args[0] + 4)};
            uint16_t matrix[9];
            unsigned mi;
            result = sub_8003FB34(axis, args[1], matrix);
            for (mi = 0; mi < 9; ++mi)
                w_u16(args[2] + 2 * mi, matrix[mi]);
            break;
        }
        case 0x80028034:
            result = sub_80028034(args[0]);
            break;
        case 0x8008BA18:
            result = sub_8008BA18(args[0]);
            break;
        case 0x8002E810:
            result = (uint64_t)sub_8002E810(args[0], xport_guest_cptr(args[1], 12), xport_guest_ptr(args[2], 12));
            break;
        case 0x8009432C:
            result = sub_8009432C(args[0]);
            break;
        case 0x80037104:
            result = sub_80037104(args[0]);
            break;
        case 0x80037450:
            result = sub_80037450(args[0]);
            break;
        case 0x800396A8:
            result = sub_800396A8(args[0]);
            break;
        case 0x8003BFE8:
            result = sub_8003BFE8(args[0]);
            break;
        case 0x8001FCB0:
            result = sub_8001FCB0(args[0], args[1], args[2]);
            break;
        case 0x8002EAD8:
            if (args[5])
            {
                uint32_t local[3];
                result = sub_8002EAD8(xport_guest_ptr(args[0], 12), xport_guest_ptr(args[1], 6), args[2], local);
                for (i = 0; i < 3; ++i)
                    w_u32(args[3] + 4 * i, local[i]);
            }
            else
                result = sub_8002EAD8(xport_guest_ptr(args[0], 12), xport_guest_ptr(args[1], 6), args[2], xport_guest_ptr(args[3], 12));
            break;
        case 0x80099D48:
            result = sub_80099D48(args[0]);
            break;
        case 0x800950E8:
            result = sub_800950E8();
            break;
        case 0x8003CEC4:
            result = sub_8003CEC4(args[0], args[1], args[2]);
            break;
        case 0x80023960:
            result = sub_80023960(args[0], args[1]);
            break;
        case 0x800136BC:
            result = sub_800136BC(args[0], args[1], args[2]);
            break;
        case 0x8003CA50:
            result = sub_8003CA50(args[0], args[1], args[2]);
            break;
        case 0x80031E1C:
            result = sub_80031E1C(args[0], args[1], args[2]);
            break;
        case 0x80033F14:
            result = sub_80033F14(args[0], args[1], args[2]);
            break;
        case 0x8002227C:
            result = sub_8002227C(args[0], args[1], args[2], args[3], args[4]);
            break;
        case 0x80031008:
            result = sub_80031008(args[0]);
            break;
        case 0x800333F4:
            result = sub_800333F4(args[0], args[1]);
            break;
        case 0x80033B50:
            result = sub_80033B50(args[0], args[1], args[2], args[3]);
            break;
        case 0x80022218:
            result = sub_80022218(args[0], args[1]);
            break;
        case 0x80032C6C:
            slot_find_count = 0;
            result = sub_80032C6C(args[0], args[1], args[2], args[3], args[4]);
            break;
        case 0x80032CC8:
            slot_find_count = 0;
            result = sub_80032CC8(args[0], args[1], args[2], args[3], args[4], args[5]);
            break;
        case 0x80031604:
            memcpy(object_dispatch_args, args, sizeof(object_dispatch_args));
            object_dispatch_composed = args[5];
            slot_find_count = 0;
            result = probe_dispatch_body(args[0], args[1]);
            break;
        case 0x80032190:
            result = sub_80032190(args[0], args[1]);
            break;
        case 0x800322D0:
            cleanup_mode = args[2];
            cleanup_reply = args[3];
            result = probe_counter_body(args[0], args[1]);
            break;
        case 0x800313EC:
            cleanup_mode = args[2];
            cleanup_reply = args[3];
            result = probe_cleanup_body(args[0]);
            break;
        case 0x80031B98:
            result = sub_80031B98(args[0], args[1]);
            break;
        case 0x80031B4C:
            result = sub_80031B4C(args[0]);
            break;
        case 0x800319F8:
            result = sub_800319F8(args[0], args[1]);
            break;
        case 0x80031D70:
            memcpy(queue_args, args, sizeof(queue_args));
            queue_composed = 0;
            result = sub_80031D70();
            break;
        case 0x80031BF8:
            result = sub_80031BF8(args[0], args[1]);
            break;
        case 0x800320CC:
            result = sub_800320CC(args[0]);
            break;
        case 0x800320AC:
            result = sub_800320AC();
            break;
        case 0x80030608:
            memcpy(queue_args, args, sizeof(queue_args));
            queue_index = 0;
            queue_composed = args[5];
            result = sub_80030608();
            break;
        case 0x8002379C:
            result = sub_8002379C(args[0]);
            break;
        case 0x80023860:
            result = sub_80023860(args[0]);
            break;
        case 0x800237B8:
            memcpy(player_update_args, args, sizeof(player_update_args));
            result = sub_800237B8();
            break;
        case 0x80023D7C:
            memcpy(player_update_args, args, sizeof(player_update_args));
            result = sub_80023D7C();
            break;
        case 0x80023C4C:
            memcpy(player_update_args, args, sizeof(player_update_args));
            result = sub_80023C4C();
            break;
        case 0x800245DC:
            result = sub_800245DC(args[0]);
            break;
        case 0x800245F4:
            result = sub_800245F4(args[0]);
            break;
        case 0x80030410:
            result = sub_80030410(args[0], args[1]);
            break;
        case 0x8008B99C:
            result = sub_8008B99C(args[0]);
            break;
        case 0x80030500:
            result = sub_80030500(args[0], args[1]);
            break;
        case 0x80022C64:
            result = sub_80022C64();
            break;
        case 0x80023870:
            memcpy(race_check_replies, args, sizeof(race_check_replies));
            race_check_index = 0;
            result = sub_80023870();
            break;
        case 0x80011C4C:
            memcpy(race_frame_args, args, sizeof(race_frame_args));
            result = sub_80011C4C();
            break;
        case 0x8008CD88:
        case 0x8008AC80:
        case 0x8008ACE8:
            memcpy(race_service_args, args + 1, sizeof(race_service_args));
            result = function == 0x8008CD88 ? sub_8008CD88(args[0]) : function == 0x8008AC80 ? sub_8008AC80(args[0]) : sub_8008ACE8(args[0]);
            break;
        case 0x8008AB00:
            race_global_replies[0] = args[1];
            race_global_replies[1] = args[2];
            race_global_composed = args[4];
            result = sub_8008AB00(args[0]);
            break;
        case 0x8008AD38:
            result = sub_8008AD38(args[0]);
            break;
        case 0x8008AD40:
            result = sub_8008AD40(args[0], args[1]);
            break;
        case 0x8006883C:
            submenu_reply = args[1];
            result = sub_8006883C(args[0]);
            break;
        case 0x8002D250:
            result = sub_8002D250();
            break;
        case 0x8006AE6C:
            submenu_reply = args[1];
            result = sub_8006AE6C(args[0]);
            break;
        case 0x8007EDE0:
        case 0x8007EEB8:
        case 0x8007EF64:
            music_replies[0] = args[1];
            music_replies[1] = args[2];
            result = function == 0x8007EDE0 ? sub_8007EDE0(args[0]) : function == 0x8007EEB8 ? sub_8007EEB8(args[0]) : sub_8007EF64();
            break;
        case 0x8006FEF4:
        case 0x80070018:
            video_phase_replies[0] = args[1];
            video_phase_replies[1] = args[2];
            video_phase_replies[2] = args[3];
            result = function == 0x8006FEF4 ? sub_8006FEF4(args[0]) : sub_80070018(args[0]);
            break;
        case 0x8006DE5C:
            video_phase_replies[2] = args[3];
            result = sub_8006DE5C(args[0]);
            break;
        case 0x8006DB5C:
            video_phase_replies[0] = args[1];
            video_phase_replies[1] = args[2];
            video_phase_replies[2] = args[3];
            result = sub_8006DB5C(args[0]);
            break;
        case 0x8004CC44:
            vblank_replies[1] = args[5];
            result = sub_8004CC44(args[0], args[1], args[2], args[3], args[4]);
            break;
        case 0x8004CD04:
            result = sub_8004CD04(args[0], args[1], args[2], args[3], args[4]);
            break;
        case 0x8001BE08:
            screen_probe_mode = args[4];
            screen_probe_env_index = 0;
            result = sub_8001BE08(args[0], args[1], args[2], args[3]);
            screen_probe_mode = 0;
            break;
        case 0x800C2178:
            result = sub_800C2178(args[0], args[1]);
            break;
        case 0x8008B428:
            result = sub_8008B428(args[0], args[1]);
            break;
        case 0x8001BF1C:
            result = sub_8001BF1C(args[0], args[1], args[2], args[3]);
            break;
        case 0x80080D08:
            vblank_replies[1] = args[3];
            result = sub_80080D08();
            break;
        case 0x8006FE6C:
            result = sub_8006FE6C();
            break;
        case 0x8006FED4:
            result = sub_8006FED4();
            break;
        case 0x8006D5B0:
            result = sub_8006D5B0(args[0]);
            break;
        case 0x8006A8FC:
            result = sub_8006A8FC(args[0]);
            break;
        case 0x8001FB58:
            result = sub_8001FB58(args[0]);
            break;
        case 0x8001F7EC:
            result = sub_8001F7EC(args[0]);
            break;
        case 0x8001F5D4:
            result = sub_8001F5D4(args[0], args[1]);
            break;
        case 0x8007F158:
            result = sub_8007F158(args[0]);
            break;
        case 0x8001EB7C:
            result = sub_8001EB7C(args[0], args[1], args[2], args[3]);
            break;
        case 0x80080274:
            probe_loop_remaining = args[2];
            result = sub_80080274();
            break;
        case 0x80080488:
            vblank_replies[0] = args[2];
            vblank_replies[1] = args[3];
            result = sub_80080488();
            break;
        case 0x8001EE94:
            result = sub_8001EE94();
            break;
        case 0x800600F8:
            result = sub_800600F8();
            break;
        case 0x8001B700:
            vblank_replies[0] = args[2];
            vblank_replies[1] = args[3];
            result = sub_8001B700();
            break;
        case 0x80064C30:
            vblank_replies[0] = args[2];
            vblank_replies[1] = args[3];
            result = sub_80064C30();
            break;
        case 0x8001E0B4:
            (void)sub_8001E0B4(xport_guest_ptr(args[0], args[2]), xport_guest_cptr(args[1], args[2]), args[2]);
            result = args[0];
            break;
        case 0x8001CB3C:
            if (args[5])
                result = rrj_input_audit_probe(&memory, args);
            else
            {
                result = sub_8001CB3C();
            }
            break;
        case 0x8001DDC4:
            pad_sdk_returns[0] = args[2];
            pad_sdk_returns[1] = args[3];
            result = sub_8001DDC4(args[0], args[1]);
            break;
        case 0x8001C4A8:
            result = sub_8001C4A8(args[0], args[1]);
            break;
        case 0x8001C5F8:
            result = sub_8001C5F8();
            break;
        case 0x80080A70:
            result = sub_80080A70(args[0]);
            break;
        case 0x80066EF8:
            result = sub_80066EF8();
            break;
        case 0x8006711C:
            result = sub_8006711C();
            break;
        case 0x80066C34:
            sub_80066C34();
            result = 0;
            break;
        case 0x800667E4:
            result = sub_800667E4();
            break;
        case 0x8006738C:
            sub_8006738C(args[0]);
            result = 0;
            break;
        case 0x80080ACC:
            result = sub_80080ACC(args[0]);
            break;
        case 0x8006310C:
            sub_8006310C(args[0]);
            result = 0;
            break;
        case 0x80062D6C:
            result = sub_80062D6C(args[0]);
            break;
        case 0x800630C0:
            result = sub_800630C0(args[0], args[1]);
            break;
        case 0x80064254:
            result = sub_80064254(args[0]);
            break;
        case 0x80068448:
            result = sub_80068448();
            break;
        case 0x800649BC:
            result = sub_800649BC(args[0]);
            break;
        case 0x800680E8:
            result = sub_800680E8(args[0], args[1]);
            break;
        case 0x8006E4D8:
            result = sub_8006E4D8(args[0], args[1]);
            break;
        case 0x8006E6F4:
            result = sub_8006E6F4(args[0], args[1]);
            break;
        case 0x8006FAC8:
            result = sub_8006FAC8(args[0], args[1]);
            break;
        case 0x800662CC:
            result = sub_800662CC(args[0], args[1], xport_guest_cptr(args[2], 8), args[3], args[4], args[5]);
            break;
        case 0x80066318:
            result = sub_80066318(args[0], args[1], xport_guest_cptr(args[2], 8), args[3], args[4], args[5]);
            break;
        case 0x80063C7C:
            result = sub_80063C7C(args[0]);
            break;
        case 0x8006E8FC:
            result = sub_8006E8FC(args[0], args[1]);
            break;
        case 0x80064154:
            result = sub_80064154(args[0]);
            break;
        case 0x800641D4:
            result = sub_800641D4();
            break;
        case 0x80072100:
            result = sub_80072100(args[0]);
            break;
        case 0x8002CB08:
            result = sub_8002CB08(args[0], args[1], xport_guest_cptr(args[2], 8), args[3], args[4], args[5]);
            break;
        case 0x8006E7A4:
            result = sub_8006E7A4(args[0], args[1]);
            break;
        case 0x8006F764:
            result = sub_8006F764(args[0], args[1]);
            break;
        case 0x8002D1F8:
            result = sub_8002D1F8(args[0], args[1], args[2], args[3]);
            break;
        case 0x8002D1A0:
            result = sub_8002D1A0(args[0], args[1], args[2]);
            break;
        case 0x8002D0D8:
            result = sub_8002D0D8(args[0], xport_guest_cptr(args[1], 1));
            break;
        case 0x8002CDC8:
            result = sub_8002CDC8(args[0], xport_guest_cptr(args[1], 1), args[2], args[3], args[4], args[5]);
            break;
        case 0x80065768:
            result = sub_80065768(args[0]);
            break;
        case 0x8007A400:
            result = sub_8007A400(args[0], args[1]);
            break;
        case 0x8007A6C0:
            result = sub_8007A6C0(args[0], args[1]);
            break;
        case 0x800700F0:
            result = sub_800700F0(args[0], xport_guest_cptr(args[1], 12), args[2]);
            break;
        case 0x8006E894:
            result = sub_8006E894(args[0], args[1]);
            break;
        /* Explicit GAME renderer interception in these dispatcher-only fixtures. */
        case 0x8006D3E0:
            result = sub_8006D3E0(args[0]);
            break;
        case 0x8006D630:
            result = sub_8006D630(args[0]);
            break;
        case 0x8006C700:
            result = sub_8006C700(args[0], args[1]);
            break;
        case 0x8006B03C:
            result = sub_8006B03C(args[0], args[1], args[2]);
            break;
        case 0x80069418:
            result = sub_80069418(args[0]);
            break;
        case 0x8001EB28:
            result = sub_8001EB28(args[0]);
            break;
        case 0x8001EB44:
            result = sub_8001EB44(args[0]);
            break;
        case 0x8001E86C:
            result = sub_8001E86C(args[0], args[1]);
            break;
        case 0x8001F9C4:
            result = sub_8001F9C4(args[0]);
            break;
        case 0x8001F174:
            result = sub_8001F174(args[0], args[1], args[2], args[3], ram_pointer(args[4], 12));
            break;
        case 0x8007EAC0:
            result = sub_8007EAC0(args[0]);
            break;
        /* Sixth fixture word selects full translated sound for integration probes;
     * it is not an argument of either original navigation function. */
        case 0x8006C3A4:
            result = sub_8006C3A4(args[0]);
            break;
        case 0x8006C558:
            result = sub_8006C558(args[0]);
            break;
        case 0x80064B30:
            result = sub_80064B30(args[0], args[1]);
            break;
        case 0x800685BC:
            sub_800685BC(args[0]);
            result = 0;
            break;
        case 0x8001C428:
            result = sub_8001C428();
            break;
        case 0x8001C3F4:
            result = sub_8001C3F4();
            break;
        case 0x800803FC:
            sub_800803FC();
            result = 0;
            break;
        case 0x8001FC90:
            result = (uint64_t)sub_8001FC90(((int32_t)(uint32_t)(args[0])), ((int32_t)(uint32_t)(args[1])));
            break;
        case 0x8002E874:
            result = (uint64_t)sub_8002E874(ram_pointer(args[0], 12), ram_pointer(args[1], 12), ram_pointer(args[2], 12));
            break;
        case 0x8002E928:
            result = (uint64_t)sub_8002E928(ram_pointer(args[0], 12), ram_pointer(args[1], 36), ram_pointer(args[2], 12));
            break;
        case 0x80021BE8:
            result = sub_80021BE8(args[0], args[1]);
            break;
        case 0x80021C98:
            result = sub_80021C98(args[0], args[1]);
            break;
        case 0x8002AF9C:
            result = sub_8002AF9C(ram_pointer(args[0], 16), ram_pointer(args[1], 4), ram_pointer(args[2], 4));
            break;
        case 0x8002B080:
            result = sub_8002B080(ram_pointer(args[0], 16), ram_pointer(args[1], 4), ram_pointer(args[2], 4));
            break;
        case 0x8002B164:
            result = sub_8002B164(ram_pointer(args[0], 16), ram_pointer(args[1], 4), ram_pointer(args[2], 4));
            break;
        case 0x8002B258:
            result = sub_8002B258(ram_pointer(args[0], 16), ram_pointer(args[1], 4), args[2], ram_pointer(args[3], 4));
            break;
        case 0x8002B3FC:
            result = sub_8002B3FC(ram_pointer(args[0], 4), ram_pointer(args[1], 4), args[2], args[3], (uint8_t)args[4], ram_pointer(args[5], 4));
            break;
        case 0x8002B4A0:
            result = sub_8002B4A0(ram_pointer(args[0], 4), ram_pointer(args[1], 4), args[2], args[3], ram_pointer(args[4], 4));
            break;
        case 0x8002B5B4:
            result = sub_8002B5B4(ram_pointer(args[0], 4), ram_pointer(args[1], 4), args[2], args[3], ram_pointer(args[4], 4));
            break;
        case 0x8002B68C:
            result = sub_8002B68C(ram_pointer(args[0], 4), ram_pointer(args[1], 4), args[2], args[3], ram_pointer(args[4], 4));
            break;
        case 0x8002B878:
            result = sub_8002B878(ram_pointer(args[0], 4), ram_pointer(args[1], 4), ram_pointer(args[2], 4));
            break;
        default:
        {
            uint32_t coverage_result;
            if (!rrj_coverage_probe(&memory, function, args, &coverage_result))
            {
                RRJ_DEBUG_FPRINTF(stderr, "Function %08X is not translated.\n", function);
                return 2;
            }
            result = coverage_result;
            break;
        }
    }
    if (memory.sdk_user && fclose((FILE *)memory.sdk_user) != 0)
        return 2;
    xport_store_le32(result_bytes, (uint32_t)result);
    xport_store_le32(result_bytes + 4, (uint32_t)(result >> 32));
    if (argc == 10 && !write_file(argv[9], SCRATCHPAD, sizeof(SCRATCHPAD)))
        return 2;
    return write_file(argv[5], result_bytes, 8) && write_file(argv[6], DRAM, sizeof(DRAM)) ? 0 : 2;
}

// XPORT REVISION: 2026-09-29T20:00:00Z
/* Preserve the host command line without depending on CRT startup globals */
static int rrj_host_main(void)
{
#if defined(_WIN32)
    const char *input = GetCommandLineA();
    size_t capacity = strlen(input) + 1u;
    char *storage = (char *)malloc(capacity);
    char **arguments = (char **)malloc((capacity + 1u) * sizeof(*arguments));
    char *output = storage;
    int count = 0, result;
    if (!storage || !arguments)
    {
        free(storage);
        free(arguments);
        return 2;
    }
    while (*input)
    {
        int quoted = 0;
        while (*input == ' ' || *input == '\t')
            ++input;
        if (!*input)
            break;
        arguments[count++] = output;
        while (*input && (quoted || (*input != ' ' && *input != '\t')))
        {
            size_t slashes = 0;
            while (*input == '\\')
            {
                ++slashes;
                ++input;
            }
            if (*input == '"')
            {
                size_t pairs = slashes / 2u;
                while (pairs--)
                    *output++ = '\\';
                if (slashes & 1u)
                    *output++ = *input++;
                else if (quoted && input[1] == '"')
                {
                    *output++ = '"';
                    input += 2;
                }
                else
                {
                    quoted = !quoted;
                    ++input;
                }
            }
            else
            {
                while (slashes--)
                    *output++ = '\\';
                if (*input && (quoted || (*input != ' ' && *input != '\t')))
                    *output++ = *input++;
            }
        }
        *output++ = '\0';
    }
    arguments[count] = NULL;
    result = rrj_main(count, arguments);
    free(arguments);
    free(storage);
    return result;
#else
    char *arguments[] = {"RRJ", NULL};
    return rrj_main(1, arguments);
#endif
}

#endif

void xport_main(void)
{
#if defined(LOCKSTEP_DEBUG)
    if (xport_run_mode() == RUN_MODE_LOCKSTEP_SELFTEST)
    {
        xport_set_exit_code(rrj_lockstep_selftest());
        return;
    }
    if (xport_run_mode() == RUN_MODE_LOCKSTEP_REPLAY)
    {
        RRJMemory memory = {0};
        memory.display_wait_service = native_display_wait_service;
        rrj_memory_bind(&memory);
        xport_set_exit_code(rrj_lockstep_run(&memory));
        return;
    }
#endif
#ifndef NDEBUG
    xport_set_exit_code(rrj_host_main());
#else
    RRJMemory memory = {0};
    char *arguments[] = {"RRJ", NULL};
    char *menu_args[] = {"RRJ", "--menu-run", "../status/menu/state1-menu-ram.bin", "../status/menu/state1-vram.bin", "../status/menu/state1-spu.bin", "4294967295", NULL};
    int argc = 1;
    rrj_wip_options(&argc, arguments, &memory);
    memory.display_wait_service = native_display_wait_service;
    rrj_memory_bind(&memory);
    xport_set_exit_code(menu_run(&memory, menu_args, 0));
#endif
}
