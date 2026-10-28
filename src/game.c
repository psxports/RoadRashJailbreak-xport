#include "rrj_debug.h"
#include "game_main.h"
#include "rrj_runtime.h"
#if defined(_WIN32)
    #include <windows.h>
#endif
#include "native_file_callbacks.h"
/* Native initialization and original callback bindings */
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
#include "psx.h"
#include "psx_gpu.h"
#include "psx_spu.h"
#include "psx_press.h"
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wip.h"
#define abort() rrj_wip_stop(__func__, __FILE__, __LINE__)

const uint32 xport_gpu_graph_type_address = 0x80055F0Cu;


static uint32_t live_spu_pending_callback;
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

static uint32_t update_video_menu(uint32_t menu)
{
    return sub_8006DB5C(menu);
}

static uint32_t update_video_tick(uint32_t menu)
{
    return sub_8006DE5C(menu);
}

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
    (void)VSync(0);
}

void xport_main(void)
{
    RRJMemory memory = {0};
    char *arguments[] = {"RRJ", NULL};
    int argc = 1;
    memory.display_wait_service = native_display_wait_service;
    rrj_memory_bind(&memory);
    rrj_wip_options(&argc, arguments, &memory);
    if (!rrj_runtime_init(&memory))
    {
        xport_set_exit_code(2);
        return;
    }
    PadInit(0);
    if (!xport_audio_init())
    {
        xport_set_exit_code(2);
        return;
    }
    (void)sub_80012224();
    xport_audio_shutdown();
}
