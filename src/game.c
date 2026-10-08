/* Native bootstrap and differential-probe entry point. No MIPS interpreter.
 * Default launch enters the translated State1 menu; probes are explicit modes.
 */
#include "fixed_math.h"
#include "flare.h"
#include "packet.h"
#include "platform_smoke.h"
#include "psx.h"
#include "psx_gpu.h"
#include "psx_spu.h"
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
#include "pad_poll.h"
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

static uint32_t probe_pad_sdk(RRJMemory *m, uint32_t fn, uint32_t a0, uint32_t a1, uint32_t a2)
{
    if (!m->sdk_user)
        abort();
    fprintf((FILE *)m->sdk_user, "%08X %08X %08X %08X\n", fn, a0, a1, a2);
    return fn == 0x80040550 ? pad_sdk_returns[0] : pad_sdk_returns[1];
}

static uint32_t screen_probe_mode, screen_probe_env_index;

static void probe_screen_env(RRJMemory *m, uint32_t fn, uint32_t address, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
    if (!m->sdk_user)
        abort();
    fprintf((FILE *)m->sdk_user, "%08X %08X %08X %08X %08X %08X\n", fn, address, x, y, w, h);
    if (screen_probe_mode == 1 && screen_probe_env_index == 0)
    {
        rrj_write32(m, 0x80160008, 0x13579BDF);
        rrj_write32(m, 0x8016000C, 0x2468ACE0);
        rrj_write32(m, 0x8005B470, 0x80160000);
    }
    if (screen_probe_mode == 2 && screen_probe_env_index == 1)
    {
        uint32_t context = rrj_read32(m, 0x8005B470);
        rrj_write32(m, context + 8, 0x31415926);
        rrj_write32(m, context + 12, 0x27182818);
    }
    ++screen_probe_env_index;
}

static uint32_t submenu_reply;
static uint32_t race_step_reply;
static uint32_t slot_find_count, object_dispatch_composed;
static uint32_t probe_reverb(RRJMemory *, uint32_t, uint32_t);
static uint32_t record_image(RRJMemory *, const uint8_t rect[8], uint32_t pixels);
static uint32_t probe_dispatch_critical(RRJMemory *, uint32_t, uint32_t, uint32_t, uint32_t);

static uint32_t probe_slot_service(RRJMemory *m, uint32_t fn, const uint32_t args[6])
{
    if (fn == 0x80033F14 && rrj_read32(m, 0x801E07F0) == 0x47454F4D)
        return sub_80033F14(m, args[0], args[1], args[2]);
    if (rrj_read32(m, 0x801E07DC) == 0x534C4F54)
    {
        if (fn == 0x800324CC)
            return sub_800324CC(m, args[0], args[1], args[2]);
        if (fn == 0x800335B4)
            return sub_800335B4(m, args[0], args[1], args[2]);
        if (fn == 0x80033B50)
            return sub_80033B50(m, args[0], args[1], args[2], args[3], probe_slot_service);
        if (rrj_read32(m, 0x801E07EC) == 0x55504C44)
        {
            if (fn == 0x8002227C)
                return sub_8002227C(m, args[0], args[1], args[2], args[3], args[4], record_image);
            if (fn == 0x80031008)
                return sub_80031008(m, args[0], probe_dispatch_critical);
            if (fn == 0x800333F4)
                return sub_800333F4(m, args[0], args[1], probe_slot_service);
        }
        if (fn == 0x8002227C || fn == 0x80031008)
        {
            uint32_t mode = rrj_read32(m, 0x801E07E0), slot = 0x800D9268 + 48 * rrj_read32(m, 0x801E07E8);
            if (fn == 0x8002227C && mode == 1)
                rrj_write32(m, slot + 8, 0x89ABCDEF);
            if (fn == 0x8002227C && mode == 2)
            {
                rrj_write32(m, slot + 12, 0x801E0C1C);
                rrj_write32(m, slot + 16, 0x801E0C20);
            }
            if (fn == 0x80031008 && mode == 2)
                rrj_write32(m, slot + 16, 0x801E0C80);
        }
    }
    unsigned i;
    if (!m->sdk_user)
        abort();
    fprintf((FILE *)m->sdk_user, "%08X", fn);
    for (i = 0; i < 6; ++i)
        fprintf((FILE *)m->sdk_user, " %08X", args[i]);
    fputc('\n', (FILE *)m->sdk_user);
    if (fn == 0x800324CC)
    {
        uint32_t mode = rrj_read32(m, 0x801E07D8);
        if (mode == 1)
        {
            rrj_write32(m, args[0] + 12, 1);
            rrj_write32(m, args[0] + 16, 2);
        }
        if (mode == 2)
        {
            rrj_write32(m, args[0] + 12, 0);
            rrj_write32(m, args[0] + 16, 0);
        }
    }
    if (fn == 0x800335B4)
    {
        if (slot_find_count >= 2)
            abort();
        return rrj_read32(m, 0x801E07D0 + 4 * slot_find_count++);
    }
    return 0;
}

static uint32_t object_dispatch_args[4];

static uint32_t probe_object_dispatch(RRJMemory *m, uint32_t fn, const uint32_t args[6])
{
    if (object_dispatch_composed >= 6 && fn == 0x8003CEC4)
        return sub_8003CEC4(m, args[0], args[1], args[2], probe_reverb);
    if (object_dispatch_composed >= 5 && fn == 0x8003CEC4)
        return rrj_probe_segment_load(m, args[0], args[1], args[2], probe_pad_sdk);
    if (object_dispatch_composed >= 4 && fn == 0x80023960)
        return sub_80023960(m, args[0], args[1]);
    if (object_dispatch_composed >= 4 && fn == 0x800136BC)
        return sub_800136BC(m, args[0], args[1], args[2]);
    if (object_dispatch_composed >= 3 && fn == 0x80032A20)
        return sub_80032A20(m, args);
    if (object_dispatch_composed >= 2 && fn == 0x80031E1C)
        return sub_80031E1C(m, args[0], args[1], args[2]);
    if (object_dispatch_composed)
    {
        if (fn == 0x80032C6C)
            return sub_80032C6C(m, args, probe_slot_service);
        if (fn == 0x80032CC8)
            return sub_80032CC8(m, args, probe_slot_service);
    }
    unsigned i;
    if (!m->sdk_user)
        abort();
    fprintf((FILE *)m->sdk_user, "%08X", fn);
    for (i = 0; i < 6; ++i)
        fprintf((FILE *)m->sdk_user, " %08X", args[i]);
    fputc('\n', (FILE *)m->sdk_user);
    if (fn != 0x80031E1C && fn != 0x80023960)
    {
        if (object_dispatch_args[3] == 1)
            rrj_write32(m, object_dispatch_args[0], 0);
        if (object_dispatch_args[3] == 2)
            rrj_write32(m, object_dispatch_args[0] + 20, 0x801E0180);
    }
    return object_dispatch_args[2];
}

static uint32_t probe_dispatch_critical(RRJMemory *m, uint32_t fn, uint32_t a0, uint32_t a1, uint32_t a2)
{
    if (!m->sdk_user)
        abort();
    fprintf((FILE *)m->sdk_user, "%08X 00000000 00000000 00000000 00000000 00000000 00000000\n", fn);
    return 0;
}

static uint32_t cleanup_mode, cleanup_reply;

static uint32_t probe_object_cleanup(RRJMemory *m, uint32_t fn, uint32_t a0, uint32_t a1, uint32_t a2)
{
    if (!m->sdk_user)
        abort();
    fprintf((FILE *)m->sdk_user, "%08X %08X %08X %08X\n", fn, a0, a1, a2);
    if (fn == 0x80043DA4)
    {
        if (cleanup_mode == 1)
            rrj_write32(m, 0x800541D0, 1);
        if (cleanup_mode == 2)
            rrj_write32(m, 0x8005ACBC, 0x801E0080);
    }
    return cleanup_reply;
}

static uint32_t queue_args[3], queue_index, queue_composed;

static uint32_t probe_object_queue(RRJMemory *m, uint32_t fn, uint32_t a0, uint32_t a1, uint32_t a2)
{
    uint32_t result = 0;
    if (queue_composed >= 3 && fn == 0x800313EC)
        return sub_800313EC(m, a0, probe_object_queue);
    if (queue_composed >= 2)
    {
        if (fn == 0x80031B4C)
            return sub_80031B4C(m, a0);
        if (fn == 0x800319F8)
            return sub_800319F8(m, a0, a1);
    }
    if (queue_composed)
    {
        if (fn == 0x80031D70)
            return sub_80031D70(m, probe_object_queue);
        if (fn == 0x80031BF8)
            return sub_80031BF8(m, a0, a1);
        if (fn == 0x800320CC)
            return sub_800320CC(m, a0);
        if (fn == 0x800320AC)
            return sub_800320AC(m);
    }
    if (!m->sdk_user)
        abort();
    fprintf((FILE *)m->sdk_user, "%08X %08X %08X %08X\n", fn, a0, a1, a2);
    if (fn == 0x80031D70)
    {
        if (queue_index < queue_args[0])
            result = 0x801E1000 + 64 * queue_index++;
    }
    if (fn == 0x80031BF8)
        result = rrj_read32(m, a0 + 12);
    if (fn == 0x800313EC && queue_args[1] == 1)
    {
        rrj_write32(m, a0 + 32, 0x87654321);
        rrj_write32(m, rrj_read32(m, a0 + 20) + 28, 0xDEADBEEF);
    }
    if (fn == 0x80031604 && queue_args[1] == 2)
        rrj_write32(m, rrj_read32(m, 0x8005ACBC) + 0xA58, 0);
    if (fn == 0x80031604 && queue_args[1] == 3)
        rrj_write32(m, 0x8005ACBC, 0x801E0100);
    if (fn == 0x800320CC && queue_args[1] == 4)
        rrj_write32(m, 0x8005AED8, 1);
    if (fn == 0x800320CC && queue_args[1] == 5)
        rrj_write32(m, 0x8005AED8, 0);
    if (fn == 0x800320AC)
        result = queue_args[2];
    return result;
}

static uint32_t player_update_args[3];

static uint32_t probe_player_update(RRJMemory *m, uint32_t fn, uint32_t a0, uint32_t a1, uint32_t a2)
{
    if (!m->sdk_user)
        abort();
    fprintf((FILE *)m->sdk_user, "%08X %08X %08X %08X\n", fn, a0, a1, a2);
    if (fn == 0x800312D0)
    {
        if (player_update_args[1] == 1)
            rrj_write32(m, rrj_read32(m, 0x8005B2F8) + 48, 1);
        if (player_update_args[1] == 2)
            rrj_write32(m, rrj_read32(m, 0x8005B2F8) + 48, 3);
        if (player_update_args[1] == 3)
            rrj_write32(m, 0x8005AE34, 0x801E0080);
    }
    return player_update_args[0];
}

static uint32_t race_pause_replies[2];
static uint32_t race_check_replies[4], race_check_index;

static uint32_t probe_race_player_check(RRJMemory *m, uint32_t player)
{
    if (!m->sdk_user || race_check_index >= 4)
        abort();
    fprintf((FILE *)m->sdk_user, "8008B99C %08X 00000000 00000000\n", player);
    return race_check_replies[race_check_index++];
}

static uint32_t probe_race_pause(RRJMemory *m, uint32_t fn)
{
    if (!m->sdk_user)
        abort();
    fprintf((FILE *)m->sdk_user, "%08X 00000000 00000000 00000000\n", fn);
    return fn == 0x80022C64 ? race_pause_replies[0] : (fn == 0x80023870 ? race_pause_replies[1] : 0);
}

static uint32_t race_frame_args[3];

static uint32_t probe_race_frame(RRJMemory *m, uint32_t fn, const uint32_t args[7])
{
    unsigned i;
    if (!m->sdk_user)
        abort();
    fprintf((FILE *)m->sdk_user, "%08X", fn);
    for (i = 0; i < 7; ++i)
        fprintf((FILE *)m->sdk_user, " %08X", args[i]);
    fputc('\n', (FILE *)m->sdk_user);
    if (race_frame_args[1] == 1 && fn == 0x80018C1C)
        w_u8(rrj_read32(m, 0x8005B2F8) + 1, (uint8_t)race_frame_args[2]);
    if (race_frame_args[1] == 2 && fn == 0x800C8CD4)
        rrj_write32(m, rrj_read32(m, 0x8005B2F8) + 48, 1);
    return race_frame_args[0];
}

static uint32_t race_service_args[3];

static uint32_t probe_race_service(RRJMemory *m, uint32_t fn, uint32_t a, uint32_t b)
{
    if (!m->sdk_user)
        abort();
    fprintf((FILE *)m->sdk_user, "%08X %08X %08X %08X\n", fn, a, b, 0u);
    if (race_service_args[0] == 1)
    {
        if (fn == 0x8009C308)
            rrj_write32(m, 0x8005B318, race_service_args[2]);
        if (fn == 0x800A2898 || fn == 0x800A2A64)
            rrj_write32(m, 0x8005B254, race_service_args[2]);
    }
    if (race_service_args[0] == 2 && fn == 0x8009B474 && a == 0)
        rrj_write32(m, 0x8005B318, race_service_args[2]);
    return race_service_args[1];
}

static uint32_t race_global_replies[2];
static uint32_t race_global_composed;

static uint32_t probe_race_composed_service(RRJMemory *m, uint32_t fn, uint32_t a, uint32_t b)
{
    if (!m->sdk_user)
        abort();
    fprintf((FILE *)m->sdk_user, "%08X %08X %08X %08X\n", fn, a, b, 0u);
    return race_global_replies[1];
}

static uint32_t probe_race_global(RRJMemory *m, uint32_t fn, uint32_t a)
{
    if (race_global_composed)
    {
        switch (fn)
        {
            case 0x8008AD38:
                return sub_G_8008AD38(m, a, probe_race_global);
            case 0x8008CD88:
                return sub_G_8008CD88(m, a, probe_race_composed_service);
            case 0x8008AC80:
                return sub_G_8008AC80(m, a, probe_race_composed_service);
            case 0x8008ACE8:
                return sub_G_8008ACE8(m, a, probe_race_composed_service);
            default:
                return probe_race_composed_service(m, fn, a, 0);
        }
    }
    if (!m->sdk_user)
        abort();
    fprintf((FILE *)m->sdk_user, "%08X %08X %08X %08X\n", fn, a, 0u, 0u);
    return fn == 0x8008AD38 ? race_global_replies[0] : (fn == 0x80090814 ? race_global_replies[1] : 0);
}

static uint32_t probe_race_step(RRJMemory *m, uint32_t fn, uint32_t a, uint32_t b)
{
    if (!m->sdk_user)
        abort();
    fprintf((FILE *)m->sdk_user, "%08X %08X %08X %08X\n", fn, a, b, 0u);
    return fn == 0x800A421C ? race_step_reply : 0;
}

static uint32_t probe_submenu(RRJMemory *m, uint32_t fn, uint32_t a, uint32_t b, uint32_t c)
{
    if (!m->sdk_user)
        abort();
    fprintf((FILE *)m->sdk_user, "%08X %08X %08X %08X\n", fn, a, b, c);
    return fn == 0x8006B03C || fn == 0x80080A70 || fn == 0x8002D298 ? submenu_reply : 0;
}

static uint32_t music_replies[2], music_voice_index;

static uint32_t probe_music_start(RRJMemory *m, uint32_t fn, uint32_t args[8])
{
    unsigned i;
    if (!m->sdk_user)
        abort();
    fprintf((FILE *)m->sdk_user, "%08X", fn);
    for (i = 0; i < 8; ++i)
        fprintf((FILE *)m->sdk_user, " %08X", args[i]);
    fputc('\n', (FILE *)m->sdk_user);
    if (fn == 0x80022D20)
        return music_replies[0];
    if (fn == 0x8001F37C)
        return music_replies[1] + music_voice_index++;
    return 0;
}

static uint32_t video_phase_replies[3];

static uint32_t probe_video_phase(RRJMemory *m, uint32_t fn, const uint32_t args[9])
{
    unsigned i;
    if (!m->sdk_user)
        abort();
    fprintf((FILE *)m->sdk_user, "%08X", fn);
    for (i = 0; i < 9; ++i)
        fprintf((FILE *)m->sdk_user, " %08X", args[i]);
    fputc('\n', (FILE *)m->sdk_user);
    return fn == 0x80022A78 ? video_phase_replies[0] : (fn == 0x8005F36C || fn == 0x8005F484) ? video_phase_replies[2] : 0;
}

static void probe_preview_stop(RRJMemory *m, uint32_t fn, uint32_t a, uint32_t b)
{
    uint32_t args[9] = {a, b, 0, 0, 0, 0, 0, 0, 0};
    (void)probe_video_phase(m, fn, args);
}

static uint32_t probe_video_open(RRJMemory *m, uint32_t format, uint32_t name)
{
    uint32_t args[9] = {format, name, 0, 0, 0, 0, 0, 0, 0};
    (void)probe_video_phase(m, 0x8001458C, args);
    return video_phase_replies[1];
}

static uint32_t vblank_replies[2];

static uint32_t probe_attract_sound(RRJMemory *m, uint32_t event)
{
    if (!m->sdk_user)
        abort();
    fprintf((FILE *)m->sdk_user, "8007EAC0 %08X 00000000\n", event);
    return 0x12345678;
}

static uint32_t probe_reverb(RRJMemory *m, uint32_t mode, uint32_t mask)
{
    if (!m->sdk_user)
        abort();
    fprintf((FILE *)m->sdk_user, "80050678 %08X %08X\n", mode, mask);
    return 0x12345678;
}

static uint32_t probe_loop_remaining;

static void probe_loop(RRJMemory *m, uint32_t fn, uint32_t a0, uint32_t a1)
{
    if (!m->sdk_user)
        abort();
    fprintf((FILE *)m->sdk_user, "%08X %08X %08X\n", fn, a0, a1);
    if (fn == 0x800803FC && --probe_loop_remaining == 0)
        w_u8(rrj_read32(m, 0x8005B2F8), 3);
}

static uint32_t probe_vblank(RRJMemory *m, uint32_t fn, uint32_t arg)
{
    if (!m->sdk_user)
        abort();
    fprintf((FILE *)m->sdk_user, "%08X %08X 00000000\n", fn, arg);
    return vblank_replies[fn == 0x800487C0 ? 0 : 1];
}

static uint32_t probe_voice_setup(RRJMemory *m, uint32_t voice, const RRJVoiceSetup *a)
{
    if (!m->sdk_user)
        abort();
    fprintf((FILE *)m->sdk_user, "80051C38 %08X %08X %08X %08X %08X %08X %08X %08X %08X\n", voice, a->voices, a->mask, (unsigned)a->left, (unsigned)a->right, (unsigned)a->pitch, a->address, (unsigned)a->adsr1, (unsigned)a->adsr2);
    return 0x12345678;
}

static void record_sdk(RRJMemory *memory, uint32_t function, uint32_t a0, uint32_t a1)
{
    FILE *log = (FILE *)memory->sdk_user;
    fprintf(log, "%08X %08X %08X\n", function, a0, a1);
}

/* Typed LoadImage test boundary. Normalize the pointed-to local RECT by value;
 * neither side compares host/PSX stack addresses or performs a GPU upload. */
static uint32_t record_image(RRJMemory *memory, const uint8_t rect[8], uint32_t pixels)
{
    fprintf((FILE *)memory->sdk_user, "80048A6C %08X %08X %08X\n", rrj_u32(rect), rrj_u32(rect + 4), pixels);
    return 0x12345678;
}

/* Explicit resolver used by item probes. Prepared descriptors run their real
 * resource function. The separate 80078F44 loader is still untranslated. */
static uint32_t item_resource(RRJMemory *memory, uint32_t function, uint32_t descriptor, uint32_t id)
{
    if (function == 0x8007A400)
        return sub_F_8007A400(memory, descriptor, id, memory->sdk_user ? record_image : rrj_gpu_upload);
    RRJ_WIP3(memory, function, "menu_resource", descriptor, id, 0);
    return 0;
}

/* Explicit unit-test interception of the GAME sound callee. Log the selected
 * halfword before the caller's store; do not install this in live gameplay. */
static uint32_t probe_navigation_sound(RRJMemory *memory, uint32_t event)
{
    rrj_sdk_call(memory, 0x8007EAC0, event, rrj_u16(rrj_at(memory, 0x801E0004, 2)));
    return 0x12345678;
}

static void *ram_pointer(uint32_t address, size_t size)
{
    uint32_t physical = address & 0x1fffffff;
    if (physical > sizeof(DRAM) || size > sizeof(DRAM) - physical)
    {
        fprintf(stderr, "Probe address outside PSX main RAM: %08X\n", address);
        exit(2);
    }
    return psx_addr(address, size);
}

static int read_file(const char *path, void *data, size_t size)
{
    FILE *f = xport_fopen(path, "rb");
    int ok;
    if (!f)
        return 0;
    ok = fread(data, 1, size, f) == size && fgetc(f) == EOF;
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
    if (rrj_u32(cpu_prefix + 84) != 2 || rrj_u32(cpu_prefix + 88) != 0x800CD898 || rrj_u32(cpu_prefix + 128) != 0x8005AC8C || rrj_u32(cpu_prefix + 152) != 0x80012370 || rrj_u32(cpu_prefix + 156) != 0x80012374 || cpu_prefix[218] || cpu_prefix[219] || cpu_prefix[222] != 34 || cpu_prefix[227] != 34 || memcmp(cpu_prefix + 236, SCRATCHPAD, sizeof(SCRATCHPAD)))
        return 0;
    return rrj_native_checkpoint_decode(abi, cpu_prefix, prefix) && fread(registers, 1, register_bytes, stream) == register_bytes && skip_checkpoint_block(stream, size - prefix - register_bytes);
}

static void restore_checkpoint_gte(const uint32 registers[64])
{
    const uint32 *control = registers + 32;
    MATRIX matrix;

    memset(&matrix, 0, sizeof(matrix));
    matrix.m[0][0] = (sint16)control[0];
    matrix.m[0][1] = (sint16)(control[0] >> 16);
    matrix.m[0][2] = (sint16)control[1];
    matrix.m[1][0] = (sint16)(control[1] >> 16);
    matrix.m[1][1] = (sint16)control[2];
    matrix.m[1][2] = (sint16)(control[2] >> 16);
    matrix.m[2][0] = (sint16)control[3];
    matrix.m[2][1] = (sint16)(control[3] >> 16);
    matrix.m[2][2] = (sint16)control[4];
    matrix.t[0] = (sint32)control[5];
    matrix.t[1] = (sint32)control[6];
    matrix.t[2] = (sint32)control[7];
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);
    SetGeomOffset((sint32)control[24], (sint32)control[25]);
    gte_write_h((uint16)control[26]);
}

static int read_file(const char *path, void *data, size_t size);
static int save_live_frame(const char *ram_path);

static int load_race_checkpoint(RRJMemory *m, const char *path, uint32_t *ordinal)
{
    static const char abi[] = "RoadRash race-loop checkpoint ABI 1";
    uint8_t stored_abi[64];
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
    if (fgetc(stream) != EOF || fclose(stream))
        return 0;
    restore_checkpoint_gte(gte_registers);
    m->checkpoint_abi = checkpoint_abi;
    m->cpu_status = checkpoint_abi.cpu_status;
    m->checkpoint_abi_valid = 1;
    return 1;
}

static uint32_t trace_race_call(RRJMemory *m, uint32_t target, const uint32_t args[8]);

static uint32_t trace_object_dispatch_call(RRJMemory *m, uint32_t target, const uint32_t args[6])
{
    const uint32_t wide_args[8] = {args[0], args[1], args[2], args[3], args[4], args[5], 0, 0};

    return trace_race_call(m, target, wide_args);
}

static int live_origin_x, live_origin_y;
static uint32_t live_race_draw_calls;

static void trace_race_sdk_call(RRJMemory *m, uint32_t target, uint32_t a0, uint32_t a1)
{
    const uint32_t args[8] = {a0, a1, 0, 0, 0, 0, 0, 0};
    (void)trace_race_call(m, target, args);
}

static uint32_t trace_race_pad_sdk(RRJMemory *m, uint32_t target, uint32_t port, uint32_t a1, uint32_t a2)
{
    if (target == 0x80040550)
        return port == 0 ? 6 : 0;
    if (target == 0x8004061C)
        return 0;
    if (target == 0x80040910)
        return 0;
    if (target == 0x80040890)
        return 0;
    abort();
}

static uint32_t trace_race_global_call(RRJMemory *m, uint32_t target, uint32_t argument)
{
    const uint32_t args[8] = {argument, 0, 0, 0, 0, 0, 0, 0};
    return trace_race_call(m, target, args);
}

static uint32_t trace_race_poll(RRJMemory *m)
{
    return sub_8001B700(m, trace_race_global_call);
}

static uint32_t trace_race_service_call(RRJMemory *m, uint32_t target, uint32_t first, uint32_t second)
{
    const uint32_t args[8] = {first, second, 0, 0, 0, 0, 0, 0};
    return trace_race_call(m, target, args);
}

static uint32_t trace_race_pause_call(RRJMemory *m, uint32_t target)
{
    const uint32_t args[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    return trace_race_call(m, target, args);
}

static uint32_t trace_race_player_call(RRJMemory *m, uint32_t target, uint32_t first, uint32_t second, uint32_t third)
{
    const uint32_t args[8] = {first, second, third, 0, 0, 0, 0, 0};
    return trace_race_call(m, target, args);
}

static uint32_t trace_race_call(RRJMemory *m, uint32_t target, const uint32_t args[8])
{
    rrj_trace_dispatch_target(target);
    if (target == 0x800150EC)
        return sub_800150EC(m, trace_race_call);
    if (target == 0x80022EEC)
        return sub_80022EEC(m, trace_race_call);
    if (target == 0x800315E8)
        return sub_800315E8(m, args[0]);
    if (target == 0x80031540)
        return sub_80031540(m, args[0]);
    if (target == 0x80031CD4)
        return sub_80031CD4(m, args[0], trace_race_call);
    if (target == 0x80030894)
        return sub_80030894(m, args[0], args[1], args[2], trace_race_call);
    if (target == 0x80090270)
        return sub_80090270(m);
    if (target == 0x80031604)
        return sub_80031604(m, args[0], args[1], trace_object_dispatch_call, trace_race_player_call);
    if (target == 0x80031BF8)
        return sub_80031BF8(m, args[0], args[1]);
    if (target == 0x80031B4C)
        return sub_80031B4C(m, args[0]);
    if (target == 0x80031E1C)
        return sub_80031E1C(m, args[0], args[1], args[2]);
    if (target == 0x80032A20)
        return sub_80032A20(m, args);
    if (target == 0x80032C6C)
        return sub_80032C6C(m, args, trace_object_dispatch_call);
    if (target == 0x800324CC)
        return sub_800324CC(m, args[0], args[1], args[2]);
    if (target == 0x800335B4)
        return sub_800335B4(m, args[0], args[1], args[2]);
    if (target == 0x80023960)
        return sub_80023960(m, args[0], args[1]);
    if (target == 0x800151EC)
        return sub_800151EC(m);
    if (target == 0x8009C308)
        return sub_8009C308(m, trace_race_call);
    if (target == 0x800B8018)
        return sub_800B8018(m, args[0], trace_race_global_call);
    if (target == 0x800B8020)
        return sub_800B8020(m, args[0], trace_race_call);
    if (target == 0x8009B474)
        return sub_8009B474(m, (int32_t)args[0], args[1], trace_race_call);
    if (target == 0x8009E89C)
        return sub_8009E89C(m, args[0], trace_race_call);
    if (target == 0x8009E8A4)
        return sub_8009E8A4(m, args[0], args[1]);
    if (target == 0x8009CFF4)
        return sub_8009CFF4(m, args[0], trace_race_call);
    if (target == 0x800C45D8)
        return sub_800C45D8(m, args[0], args[1]);
    if (target == 0x8005C418)
        return sub_8005C418(m, args[0]);
    if (target == 0x8005C8F4)
        return sub_8005C8F4(m, args[0]);
    if (target == 0x80044894)
        return 0;
    if (target == 0x800487C0)
        return (uint32_t)DrawSync((sint32)args[0]);
    if (target == 0x80048E24)
    {
        if (rrj_trace_runtime_is_live())
        {
            unsigned x = rrj_u16(rrj_at(m, args[0], 2));
            unsigned y = rrj_u16(rrj_at(m, args[0] + 2, 2));
            unsigned w = rrj_u16(rrj_at(m, args[0] + 4, 2));
            unsigned h = rrj_u16(rrj_at(m, args[0] + 6, 2));
            unsigned ox = rrj_u16(rrj_at(m, args[0] + 8, 2));
            unsigned oy = rrj_u16(rrj_at(m, args[0] + 10, 2));

            live_origin_x = (int)x;
            live_origin_y = (int)y;
            rrj_gpu_environment(0xe3000000 | x | (y << 10), x, y);
            rrj_gpu_environment(0xe4000000 | (x + w - 1) | ((y + h - 1) << 10), x, y);
            rrj_gpu_environment(0xe5000000 | (ox & 2047) | ((oy & 2047) << 11), x, y);
        }
        return args[0];
    }
    if (target == 0x80048FF0)
        return args[0];
    if (target == 0x80048CAC)
        return (uint32_t)(uintptr_t)ClearOTagR((uint32 *)rrj_at(m, args[0], 4 * args[1]), (sint32)args[1]);
    if (target == 0x80075EE0)
        return sub_80075EE0(m, (int32_t)args[0], trace_race_call, rrj_spu_reverb);
    if (target == 0x8008F068)
        return sub_8008F068(m, (int32_t)args[0], trace_race_call);
    if (target == 0x800A2898)
        return sub_800A2898(m, (int32_t)args[0], trace_race_call);
    if (target == 0x800A2A64)
        return sub_800A2A64(m, (int32_t)args[0], trace_race_call);
    if (target == 0x800A4774)
        return sub_800A4774(m, (int32_t)args[0], trace_race_call);
    if (target == 0x8007B840)
        return sub_8007B840(m, (int32_t)args[0], trace_race_call);
    if (target == 0x80079B20)
        return sub_80079B20(m, args[0], (int32_t)args[1], trace_race_call);
    if (target == 0x80078DB4)
        return sub_80078DB4(m, args[0]);
    if (target == 0x8007AC04)
        return sub_8007AC04(m, args[0]);
    if (target == 0x80095724)
        return sub_80095724(m, args[0], args[1], trace_race_call);
    if (target == 0x800A8C78)
        return sub_800A8C78(m, args[0], (int32_t)args[1], (int32_t)args[2]);
    if (target == 0x800A8FE8)
        return sub_800A8FE8(m, args[0]);
    if (target == 0x800B3AD0)
        return sub_800B3AD0(m, args[0], args[1], trace_race_call);
    if (target == 0x800B12A0)
        return sub_800B12A0(m, args[0], args[1], args[2], (int32_t)args[3], args[4], args[5]);
    if (target == 0x800B6F40)
        return sub_800B6F40(m, args[0], args[1], args[2], args[3], args[4]);
    if (target == 0x800CB304)
        return sub_800CB304(m, (int32_t)args[0], trace_race_call);
    if (target == 0x800CB4F8)
        return sub_800CB4F8(m, (int32_t)args[0], trace_race_call);
    if (target == 0x800CAAF0)
        return sub_800CAAF0(m, args[0], trace_race_call);
    if (target == 0x800CAB5C)
        return sub_800CAB5C(m, args[0], trace_race_call);
    if (target == 0x8005BE58)
        return sub_8005BE58(m, args[0]);
    if (target == 0x800CB02C)
        return sub_800CB02C(m, args[0]);
    if (target == 0x800A13C4)
        return sub_800A13C4(m, (int32_t)args[0], trace_race_call);
    if (target == 0x80090814)
        return sub_80090814(m, (int32_t)args[0], trace_race_call);
    if (target == 0x80090D84)
        return sub_80090D84(m, args[0], trace_race_call, rrj_spu_reverb);
    if (target == 0x80091468)
        return sub_80091468(m, args[0], trace_race_call);
    if (target == 0x800C5078)
        return sub_800C5078(m, args[0], trace_race_call);
    if (target == 0x800C47CC)
        return sub_800C47CC(m, args[0], trace_race_call);
    if (target == 0x800C4860)
        return sub_800C4860(m, args[0], trace_race_call);
    if (target == 0x800C4B30)
        return sub_800C4B30(m, args[0], trace_race_call);
    if (target == 0x800C4BA0)
        return sub_800C4BA0(m, args[0], trace_race_call);
    if (target == 0x800C4E18)
        return sub_800C4E18(m, args[0], trace_race_call);
    if (target == 0x800881B4)
        return sub_800881B4(m, args[0], (int32_t)args[1], trace_race_call);
    if (target == 0x800A421C)
        return sub_800A421C(m, args[0], trace_race_call);
    if (target == 0x800853E4)
        return sub_800853E4(m, args[0], args[1]);
    if (target == 0x80086E1C)
        return sub_80086E1C(m, args[0], trace_race_call);
    if (target == 0x8003775C)
        return sub_8003775C(m, args[0], args[1], args[2]);
    if (target == 0x800374D4)
        return sub_800374D4(m, args[0], args[1]);
    if (target == 0x80088140)
        return sub_80088140(m, args[0]);
    if (target == 0x8008CFDC)
        return sub_8008CFDC(m, trace_race_call);
    if (target == 0x8008DBCC)
        return sub_8008DBCC(m, args[0]);
    if (target == 0x800667C4)
        return sub_800667C4(m, args[0], args[1]);
    if (target == 0x8005E1D8)
        return sub_8005E1D8(m, args[0], trace_race_call);
    if (target == 0x8005D2A8)
        return sub_8005D2A8(m, args[0], trace_race_call);
    if (target == 0x8005E5A4)
        return sub_8005E5A4(m, args[0], args[1], trace_race_call);
    if (target == 0x8005E558)
        return sub_8005E558(m, args[0], args[1], args[2]);
    if (target == 0x8005D63C)
        return sub_8005D63C(m, args[0], trace_race_call);
    if (target == 0x80066A60)
        return sub_80066A60(m, args[0], args[1]);
    if (target == 0x800714FC)
        return sub_800714FC(m, args[0], args[1], args[2]);
    if (target == 0x8001B44C)
        return sub_8001B44C(m, args[0], args[1], args[2], args[3]);
    if (target == 0x80071BCC)
        return sub_80071BCC(m, args[0], trace_race_call, rrj_spu_reverb);
    if (target == 0x8001005C)
        return sub_8001005C(m, args[0], args[1]);
    if (target == 0x8005CB04)
        return sub_8005CB04(m, args[0], args[1], trace_race_call);
    if (target == 0x8005C58C)
        return sub_8005C58C(m, args[0], args[1], trace_race_call);
    if (target == 0x8005C52C)
        return sub_8005C52C(m, args[0]);
    if (target == 0x8005C4EC)
        return sub_8005C4EC(m, args[0], args[1]);
    if (target == 0x80011C4C)
        return sub_80011C4C(m, trace_race_call);
    if (target == 0x8002305C)
        return sub_8002305C(m, trace_race_pause_call);
    if (target == 0x800237B8)
        return sub_800237B8(m, trace_race_player_call);
    if (target == 0x80023A14)
        return sub_80023A14(m, trace_race_call);
    if (target == 0x80023DB8)
        return sub_80023DB8(m, args[0]);
    if (target == 0x80023FBC)
        return sub_80023FBC(m, trace_race_call);
    if (target == 0x80023CAC)
        return sub_80023CAC(m, trace_race_call);
    if (target == 0x800247E8)
        return sub_800247E8(m, trace_race_call);
    if (target == 0x80022C64)
        return sub_80022C64(m);
    if (target == 0x80012524)
        return sub_80012524(m, trace_race_service_call);
    if (target == 0x8001264C)
        return sub_8001264C(m);
    if (target == 0x80018E54)
        return sub_80018E54(m, args[0], trace_race_call);
    if (target == 0x80043E24)
        return sub_80043E24(m);
    if (target == 0x80043DC4)
        return sub_80043DC4(m, args[0]);
    if (target == 0x800C89A0)
        return sub_800C89A0(m);
    if (target == 0x800C8B24)
        return sub_800C8B24(m, args[0], trace_race_call);
    if (target == 0x80064B9C)
        return sub_80064B9C(m, args[0], trace_race_call);
    if (target == 0x800650D0)
        return sub_800650D0(m, args[0], trace_race_call);
    if (target == 0x800654B4)
        return sub_800654B4(m, (int32_t)args[0], (int32_t)args[1]);
    if (target == 0x800656A8)
        return sub_800656A8(m, (int32_t)args[0], (int32_t)args[1], (int32_t)args[2], (int32_t)args[3]);
    if (target == 0x800662BC)
        return sub_800662BC(m, trace_race_call);
    if (target == 0x80065174)
        return sub_80065174(m, trace_race_call);
    if (target == 0x800662C4)
        return sub_800662C4(m, args[0], trace_race_call);
    if (target == 0x80020400)
        return sub_80020400(m, args[0], args[1]);
    if (target == 0x800644F4)
        return sub_800644F4(m, trace_race_call);
    if (target == 0x800644FC)
        return sub_800644FC(m, args[0], trace_race_call);
    if (target == 0x8006396C)
        return sub_8006396C(m);
    if (target == 0x80063C5C)
        return sub_80063C5C(m, args[0]);
    if (target == 0x8002C4F8)
        return sub_8002C4F8(m, args[0], args[1], args[2]);
    if (target == 0x80021988)
        return sub_80021988(m, (int32_t)args[0], (int32_t)args[1], trace_race_call);
    if (target == 0x8002AF80)
        return sub_8002AF80(m, args[0]);
    if (target == 0x8002BAE8)
        return sub_8002BAE8(m, args[0], args[1], args[2]);
    if (target == 0x8002BE14)
        return sub_8002BE14(m, args[0], args[1], args[2]);
    if (target == 0x800C8CD4)
        return sub_800C8CD4(m, args[0], trace_race_call);
    if (target == 0x8004D184)
        return sub_8004D184(m, (int32_t)args[0], (int32_t)args[1]);
    if (target == 0x8001E084)
        return sub_8001E084(m);
    if (target == 0x8001C304)
        return sub_8001C304(m, (int32_t)args[0], (int32_t)args[1], (int32_t)args[2], (int32_t)args[3], args[6]);
    if (target == 0x8005FA68)
        return sub_8005FA68(m, args[0], args[1], args[2], args[3]);
    if (target == 0x800C5618)
        return sub_800C5618(m, args[0]);
    if (target == 0x8005FAC4)
        return sub_8005FAC4(m, args[0], args[1], args[2]);
    if (target == 0x80095410)
        return sub_80095410(m, args[0]);
    if (target == 0x8008B84C)
        return sub_8008B84C(m, args[0]);
    if (target == 0x8005F9B0)
        return sub_8005F9B0(m, args[0], (int32_t)args[1], (int32_t)args[2], (int32_t)args[3]);
    if (target == 0x800C5558)
        return sub_800C5558(m, args[0]);
    if (target == 0x8005F834)
        return sub_8005F834(m, args[0], args[1], args[2]);
    if (target == 0x80063408)
        return sub_80063408(m, args[0], args[1], args[2], args[3], args[4]);
    if (target == 0x800C5168)
        return sub_800C5168(m, args[0], args[1], args[2], args[3]);
    if (target == 0x8001FE80)
        return sub_8001FE80(m, (int32_t)args[0], args[1], args[2]);
    if (target == 0x8002DEC8)
        return sub_8002DEC8(m, args[0], (int32_t)args[1]);
    if (target == 0x8002DE40)
        return sub_8002DE40(m, (int32_t)args[0], args[1]);
    if (target == 0x800C5380)
        return sub_800C5380(m, args[0], (int32_t)args[1], args[2], args[3]);
    if (target == 0x80013E64)
        return sub_80013E64(m, args[0]);
    if (target == 0x80013AF8)
        return sub_80013AF8(m, args[0], args[1]);
    if (target == 0x800C569C)
        return sub_800C569C(m, args[0], args[1], (int32_t)args[2], args[3]);
    if (target == 0x800C52A8)
        return sub_800C52A8(m, args[0], args[1], args[2]);
    if (target == 0x800C52A0)
        return sub_800C52A0(m, args[0], args[1]);
    if (target == 0x80061E50)
        return sub_80061E50(m, args[0], args[1], args[2]);
    if (target == 0x8005FE58)
        return sub_8005FE58(m, args[0], args[1], args[2], args[3]);
    if (target == 0x80016528)
        return sub_80016528(m, trace_race_call);
    if (target == 0x8005FF84)
        return sub_8005FF84(m, args[0], args[1], args[2], args[3], trace_race_call);
    if (target == 0x80060178)
        return sub_80060178(m, args[0], args[1], args[2], args[3]);
    if (target == 0x800603E4)
        return sub_800603E4(m, args[0], args[1], args[2], args[3]);
    if (target == 0x800606F0)
        return sub_800606F0(m, args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
    if (target == 0x800606F8)
        return sub_800606F8(m, args[0], args[1], args[2], args[3], args[4], args[5], args[6], rrj_read32(m, 0x8005B2F8u));
    if (target == 0x80060C10)
        return sub_80060C10(m, args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
    if (target == 0x80060C18)
        return sub_80060C18(m, args[0], args[1], args[2], args[3], args[4], args[5], args[6], rrj_read32(m, 0x8005ACDCu));
    if (target == 0x8006148C)
        return sub_8006148C(m, args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
    if (target == 0x80061494)
        return sub_80061494(m, args[0], args[1], args[2], args[3], args[4], args[5], args[6], rrj_read32(m, 0x8005ACDCu));
    if (target == 0x80061F6C)
        return sub_80061F6C(m, args[0], args[1], args[2], args[3], args[4]);
    if (target == 0x80062368)
        return sub_80062368(m, args[0], args[1], args[2], args[3], args[4]);
    if (target == 0x80062610)
        return sub_80062610(m, args[0], args[1], args[2], args[3]);
    if (target == 0x80062618)
        return sub_80062618(m, args[0], args[1], args[2], rrj_read32(m, 0x8005B2F8u));
    if (target == 0x80062C40)
        return sub_80062C40(m, args[0], args[1], args[2], args[3], args[4]);
    if (target == 0x80062D9C)
        return sub_80062D9C(m, args[0], args[1], args[2], args[3]);
    if (target == 0x8004CE44)
        return sub_8004CE44(m, args[0], args[1], args[2], args[3], args[4]);
    if (target == 0x80062F34)
        return sub_80062F34(m, args[0], args[1], args[2], args[3]);
    if (target == 0x80063530)
        return sub_80063530(m, args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
    if (target == 0x800636F0)
        return sub_800636F0(m, args[0], args[1], args[2], args[3], args[4]);
    if (target == 0x8005F030)
        return sub_8005F030(m, args[0], args[1], args[2], args[3], args[4], args[5]);
    if (target == 0x8005FB4C)
        return sub_8005FB4C(m, args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
    if (target == 0x8003B9A8)
        return sub_8003B9A8(m, (int32_t)args[0], args[1]);
    if (target == 0x8003BC48)
        return sub_8003BC48(m, (int32_t)args[0], (int32_t)args[1], args[2], (int32_t)args[3]);
    if (target == 0x8003C590)
        return sub_8003C590(m, args[0]);
    if (target == 0x8005E848 || target == 0x8005E850)
        return sub_8005E848(m, trace_race_call);
    if (target == 0x8002CA5C)
        return sub_8002CA5C(m);
    if (target == 0x80043F00)
        return sub_80043F00(m, args[0]);
    if (target == 0x800270F0)
        return sub_800270F0(m, args[0]);
    if (target == 0x8004F0A8)
        return sub_8004F0A8(m, (int32_t)args[0], args[1]);
    if (target == 0x80051A38)
        return sub_80051A38(m, (int32_t)args[0], (int32_t)args[1], (int32_t)args[2], (int32_t)args[3]);
    if (target == 0x800506A8)
        return sub_800506A8(m, args[0], args[1], args[2], args[3]);
    if (target == 0x80051088)
        return sub_80051088(m, args[0], args[1]);
    if (target == 0x8001F900)
        return sub_8001F900(m, args[0], args[1]);
    if (target == 0x80051C38)
        return sub_80051C38(m, args[0], args[1]);
    if (target == 0x8001F874)
        return sub_8001F874(m, (int32_t)args[0], args[1]);
    if (target == 0x80016768)
        return sub_80016768(m, args[0], args[1], args[2], args[3], args[4], args[5]);
    if (target == 0x800167A4)
        return sub_800167A4(m, args[0], rrj_spu_reverb);
    if (target == 0x8001B244)
        return sub_8001B244(m, args[0], rrj_spu_reverb);
    if (target == 0x80027778)
        return sub_80027778(m, args[0], args[1], args[2], args[3]);
    if (target == 0x80027974)
        return sub_80027974(m, args[0], args[1], args[2]);
    if (target == 0x800179D8)
        return sub_800179D8(m, args[0], args[1], trace_race_call);
    if (target == 0x800169D0)
        return sub_800169D0(m, args[0], rrj_spu_reverb);
    if (target == 0x80016E4C)
        return sub_80016E4C(m, args[0], trace_race_call, rrj_spu_reverb);
    if (target == 0x800184AC)
        return sub_800184AC(m, args[0], trace_race_call, rrj_spu_reverb);
    if (target == 0x80018FAC)
        return sub_80018FAC(m, trace_race_call, rrj_spu_reverb);
    if (target == 0x80018C1C)
        return sub_80018C1C(m, args[0], trace_race_call);
    if (target == 0x800505F8)
        return sub_800505F8(m, args[0]);
    if (target == 0x8001C3F4)
        return sub_8001C3F4(m);
    if (target == 0x8001C428)
        return sub_8001C428(m);
    if (target == 0x8001C408)
        return sub_8001C408(m, trace_race_poll);
    if (target == 0x8001C5F8)
        return sub_8001C5F8(m, trace_race_sdk_call);
    if (target == 0x80020E30)
        return sub_80020E30(m, args[0], trace_race_call);
    if (target == 0x8002D2F4)
        return sub_8002D2F4(m, trace_race_call);
    if (target == 0x8001DDC4)
        return sub_8001DDC4(m, args[0], args[1], trace_race_pad_sdk);
    if (target == 0x8001B700)
        return sub_8001B700(m, trace_race_global_call);
    if (target == 0x8001FD24)
        return (uint32_t)sub_8001FD24(m, args[0], args[1], args[2], args[3]);
    if (target == 0x8001F7EC)
        return sub_8001F7EC(m, args[0], rrj_spu_reverb);
    if (target == 0x80019C54)
        return sub_80019C54(m, args[0], args[1], trace_race_call);
    if (target == 0x80019990)
        return sub_80019990(m, trace_race_call, rrj_spu_setup, rrj_spu_command);
    if (target == 0x8004CE14)
    {
        uint32_t result = (args[2] ? 0xE1000200u : 0xE1000000u) | (args[3] & 0x9FFu) | (args[1] ? 0x400u : 0);

        w_u8(args[0] + 3, 1);
        rrj_write32(m, args[0] + 4, result);
        return result;
    }
    if (target == 0x8002F2E8)
        return sub_8002F2E8(m, args[0], trace_race_call);
    if (target == 0x8002F17C)
        return sub_8002F17C(m, args[0], trace_race_call);
    if (target == 0x80070F9C)
        return sub_80070F9C(m, args[0], args[1]);
    if (target == 0x8004D264)
        return sub_80070F9C(m, args[0], args[1]);
    if (target == 0x8004D154)
        return args[0];
    if (target == 0x80048DB4)
    {
        if (rrj_trace_runtime_is_live())
        {
            ++live_race_draw_calls;
            if (!rrj_gpu_draw_ot(m, args[0], live_origin_x, live_origin_y))
                abort();
        }
        return 0;
    }
    if (target == 0x80048ACC)
    {
        uint32_t width = rrj_u16(rrj_at(m, args[0] + 4, 2));
        uint32_t height = rrj_u16(rrj_at(m, args[0] + 6, 2));

        xport_guest_fill(args[1], 0, width * height * 2);
        return 0;
    }
    if (target == 0x8004D7B4 || target == 0x8004D9A8 || target == 0x8004D9E4 || target == 0x8004D90C || target == 0x8004D988)
        return 0;
    if (target == 0x800674C8)
        return sub_800674C8(m);
    if (target == 0x8008D56C)
        return sub_8008D56C(m, args[0], trace_race_call);
    if (target == 0x8008B99C)
        return sub_8008B99C(m, args[0]);
    if (target == 0x80023870)
        return sub_80023870(m, sub_8008B99C);
    if (target == 0x80084E10)
        return sub_80084E10(m, args[0], args[1], trace_race_call);
    if (target == 0x80067AC4)
        return sub_80067AC4(m, args[0], args[1], trace_race_call);
    if (target == 0x80068468)
        return sub_80068468(m, args[0], args[1], trace_race_call);
    if (target == 0x8006780C)
        return sub_8006780C(m, args[0], args[1], trace_race_call);
    if (target == 0x80067690)
        return sub_80067690(m, args[0], args[1], trace_race_call);
    if (target == 0x80028E8C)
        return sub_80028E8C(m, args[0], args[1], args[2]);
    if (target == 0x80029174)
        return sub_80029174(m, args[0], args[1], args[2], args[3]);
    if (target == 0x8002926C)
        return sub_8002926C(m, args[0], args[1], args[2], args[3], args[4]);
    if (target == 0x800295AC)
        return sub_800295AC(m, args[0], args[1], args[2], args[3], args[4]);
    if (target == 0x8002AB14)
        return sub_8002AB14(m, args[0], args[1]);
    if (target == 0x8002A2C8)
        return sub_8002A2C8(m, args[0], args[1], args[2], args[3], args[4]);
    if (target == 0x80028E74)
        return sub_80028E74(m, args[0]);
    if (target == 0x800289E8)
        return sub_800289E8(m, args[0], args[1], args[2]);
    if (target == 0x80028C78)
        return sub_80028C78(m, args[0], args[1], args[2], args[3]);
    if (target == 0x8002A8E4)
        return sub_8002A8E4(m, args[0], args[1]);
    if (target == 0x80017F64)
        return sub_80017F64(m, args[0], args[1], trace_race_call);
    if (target == 0x800182B0)
        return sub_800182B0(m, args[0], trace_race_call);
    if (target == 0x8001836C)
        return sub_8001836C(m, args[0], trace_race_call);
    if (target == 0x8002A974)
        return sub_8002A974(m, args[0], args[1], args[2], args[3], trace_race_call);
    if (target == 0x80029D88)
        return sub_80029D88(m, args[0], args[1], args[2], args[3], trace_race_call);
    if (target == 0x8002823C)
        return sub_8002823C(m, args[0], args[1], trace_race_call);
    if (target == 0x80067770)
        return sub_80067770(m, args[0], args[1], trace_race_call);
    if (target == 0x800674D4)
        return sub_800674D4(m, args[0], (int32_t)args[1], args[2], trace_race_call);
    if (target == 0x80068D50)
        return sub_80068D50(m, args[0], args[1]);
    if (target == 0x80068E2C)
        return sub_80068E2C(m, args[0], args[1]);
    if (target == 0x80068EB8)
        return sub_80068EB8(m, args[0]);
    if (target == 0x80035958)
        return sub_80035958(m, args[0], trace_race_call);
    if (target == 0x80069200)
        return sub_80069200(m, args[0]);
    if (target == 0x8006929C)
        return sub_8006929C(m, args[0], args[1], args[2]);
    if (target == 0x80069784)
        return sub_80069784(m, args[0], args[1], args[2]);
    if (target == 0x80069CF0)
        return sub_80069CF0(m, args[0], args[1], args[2]);
    if (target == 0x8006A25C)
        return sub_8006A25C(m, args[0], args[1], args[2]);
    if (target == 0x8006A630)
        return sub_8006A630(m, args[0], args[1]);
    if (target == 0x8006C888)
        return sub_8006C888(m, args[0], args[1]);
    if (target == 0x8006D350)
        return sub_8006D350(m, args[0], args[1]);
    if (target == 0x8006DC20)
        return sub_8006DC20(m, args[0], args[1]);
    if (target == 0x8006E474)
        return sub_8006E474(m, args[0], args[1]);
    if (target == 0x8006F5D0)
        return sub_8006F5D0(m, args[0], args[1]);
    if (target == 0x80068FCC)
        return sub_80068FCC(m, args[0]);
    if (target == 0x800706A4)
        return sub_800706A4(m, args[0]);
    if (target == 0x8001298C)
        return sub_8001298C(m, args[0], args[1]);
    if (target == 0x800358C0)
        return sub_800358C0(m, args[0], trace_race_call);
    if (target == 0x80035F48)
        return sub_80035F48(m, args[0], trace_race_call);
    if (target == 0x800363F0)
        return sub_800363F0(m, args[0]);
    if (target == 0x80035040)
        return sub_80035040(m, args[0], args[1], (int32_t)args[2]);
    if (target == 0x800351EC)
        return sub_800351EC(m, args[0], args[1], (int32_t)args[2], args[3], args[4], args[5]);
    if (target == 0x800353C4)
        return sub_800353C4(m, args[0], (int32_t)args[1], args[2], args[3], trace_race_call);
    if (target == 0x80036438)
        return sub_80036438(m, (int32_t)args[0], args[1], args[2]);
    if (target == 0x80036614)
        return sub_80036614(m, args[0], args[1], args[2]);
    if (target == 0x80035680)
        return sub_80035680(m, (int32_t)args[0], args[1], args[2]);
    if (target == 0x800106DC)
        return sub_800106DC(m, args[0], args[1], args[2]);
    if (target == 0x8001064C)
        return sub_8001064C(m);
    if (target == 0x80030E58)
        return sub_80030E58(m, args[0], trace_race_call);
    if (target == 0x80033198)
        return sub_80033198(m, args[0], trace_race_call);
    if (target == 0x80023900)
        return sub_80023900(m, args[0], args[1], args[2], trace_race_call);
    if (target == 0x800243EC)
        return sub_800243EC(m, args[0], args[1]);
    if (target == 0x800318E8)
        return sub_800318E8(m, args[0], args[1], trace_race_call);
    if (target == 0x800312D0)
        return sub_800312D0(m, args[0]);
    if (target == 0x80013828)
        return sub_80013828(m, args[0]);
    if (target == 0x80023868)
        return sub_80023868(m, args[0]);
    if (target == 0x800313EC)
        return sub_800313EC(m, args[0], trace_race_player_call);
    if (target == 0x80030608)
        return sub_80030608(m, trace_race_player_call);
    if (target == 0x80031D70)
        return sub_80031D70(m, trace_race_player_call);
    if (target == 0x800320CC)
        return sub_800320CC(m, args[0]);
    if (target == 0x80043DA4)
    {
        sub_80043DA4(m);
        return 0;
    }
    if (target == 0x80043DB4)
    {
        sub_80043DB4(m);
        return 0;
    }
    if (target == 0x800C4550)
        return sub_800C4550(m, args[0], args[1], args[2]);
    if (target == 0x800CAA44)
        return sub_800CAA44(m, args[0], (int32_t)args[1]);
    if (target == 0x8009ACA4)
        return sub_8009ACA4(m);
    if (target == 0x8009AB60)
        return sub_8009AB60(m);
    if (target == 0x8001CB3C)
        return sub_8001CB3C_race(m, trace_race_call);
    if (target == 0x8008A998)
        return sub_8008A998(m, args[0], args[1]);
    if (target == 0x800C2348)
        return sub_800C2348(m, args[0], args[1], trace_race_call);
    if (target == 0x8008AB00)
        return sub_G_8008AB00(m, args[0], trace_race_global_call);
    if (target == 0x8008AD38)
        return sub_G_8008AD38(m, args[0], trace_race_global_call);
    if (target == 0x8008CD88)
        return sub_G_8008CD88(m, args[0], trace_race_service_call);
    if (target == 0x8008AC80)
        return sub_G_8008AC80(m, args[0], trace_race_service_call);
    if (target == 0x8008ACE8)
        return sub_G_8008ACE8(m, args[0], trace_race_service_call);
    if (target == 0x8009A298)
        return sub_8009A298(m, args[0], trace_race_call);
    if (target == 0x800B9414)
        return sub_800B9414(m, args[0], trace_race_call, rrj_spu_reverb);
    if (target == 0x800B941C)
        return sub_800B941C(m, args[0], trace_race_call, rrj_spu_reverb);
    if (target == 0x8007F0BC)
        return sub_8007F0BC(m, args[0], args[1]);
    if (target == 0x80037338)
        return sub_80037338(m);
    if (target == 0x8003E150)
        return sub_8003E150(m);
    if (target == 0x8003AE24)
        return sub_8003AE24(m);
    if (target == 0x8003B520)
        return sub_8003B520(m);
    if (target == 0x8007504C)
        return sub_8007504C(m, args[0], args[1]);
    if (target == 0x8007FA4C)
        return sub_8007FA4C(m, args[0]);
    if (target == 0x800807F0)
        return sub_800807F0(m, args[0], args[1]);
    if (target == 0x80093E6C)
        return sub_80093E6C(m, rrj_spu_reverb);
    if (target == 0x800950E8)
        return sub_800950E8(m, rrj_spu_reverb);
    if (target == 0x80032810)
        return sub_80032810(m, args[0], args[1], args[2]);
    if (target == 0x8003297C)
        return sub_8003297C(m, args[0], args[1]);
    if (target == 0x80033B50)
        return sub_80033B50(m, args[0], args[1], args[2], args[3], trace_object_dispatch_call);
    if (target == 0x80032190)
        return sub_80032190(m, args[0], args[1]);
    if (target == 0x800322D0)
        return sub_800322D0(m, args[0], args[1], trace_race_player_call);
    if (target == 0x800457E8)
    {
        uint32_t previous = rrj_read32(m, 0x8005AF60u);

        rrj_write32(m, 0x8005AF60u, args[0]);
        return previous;
    }
    if (target == 0x800457A8)
        return 0;
    if (target == 0x8004594C)
        return 1;
    if (target == 0x8005D36C)
        return sub_8005D36C(m, args[0]);
    if (target == 0x800B6BD0)
        return sub_800B6BD0(m, args[0], args[1], args[2], args[3]);
    if (target == 0x800C1DD4)
        return sub_800C1DD4(m, args[0]);
    if (target == 0x800C258C)
        return sub_800C258C(m, args[0], args[1]);
    if (target == 0x80017DA0)
        return sub_80017DA0(m, args[0], args[1], args[2], args[3]);
    rrj_wip_handoff(m, target, "race_loop", "trace_race_call", __FILE__, __LINE__, "translate_callback_target", args, 8);
    fprintf(stderr, "WIP handoff: %08X trace_race_call\n", target);
    exit(20);
}

static int trace_replay(RRJMemory *m)
{
    static uint8_t trace_spu[524288];
    const char *path = getenv("RRJ_PHASE_CHECKPOINT_LOAD");
    uint32_t ordinal;
    xport_set_headless(1);
    if (!path)
    {
        fputs("Cannot load native checkpoint: missing RRJ_PHASE_CHECKPOINT_LOAD\n", stderr);
        return 22;
    }
    if (!load_race_checkpoint(m, path, &ordinal))
    {
        fputs("Cannot load native checkpoint: incompatible Road Rash ABI\n", stderr);
        return 22;
    }
    rrj_spu_initialize(trace_spu);
    m->sdk_call = trace_race_sdk_call;
    rrj_wip_frame(rrj_read32(m, 0x800D5D48));
    if (!rrj_trace_runtime_init(m, ordinal))
    {
        fputs("Cannot initialize trace runtime: incomplete environment\n", stderr);
        return 14;
    }
    if (!rrj_trace_phase_boundary(m, 0x80012370))
        (void)sub_80012370(m, trace_race_call);
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
    static uint8_t race_spu[524288];
    uint32_t ordinal;
    int result;

    if (!load_race_checkpoint(m, "DATA/first-race.rrcp", &ordinal) || ordinal != 152 || !read_file("DATA/first-race.vram", VRAM, sizeof(VRAM)) || !read_file("DATA/first-race.spu", race_spu, sizeof race_spu))
    {
        fputs("Cannot load DATA/first-race runtime package\n", stderr);
        return 22;
    }
    live_race_draw_calls = 0;
    if (!rrj_trace_runtime_init_live(m))
    {
        fputs("Cannot initialize first-race runtime\n", stderr);
        return 22;
    }
    rrj_spu_initialize(race_spu);
    if (!xport_audio_init())
    {
        fputs("Windows audio device initialization failed\n", stderr);
        return 2;
    }
    PadInit(0);
    m->sdk_call = trace_race_sdk_call;
    rrj_wip_frame(rrj_read32(m, 0x800D5D48));
    gpu_begin();
    if (!rrj_trace_phase_boundary(m, 0x80012370))
        (void)sub_80012370(m, trace_race_call);
    result = rrj_trace_runtime_finish(0);
    xport_audio_shutdown();
    return result;
}

static int first_race_run(RRJMemory *m, int headless)
{
    int result;

    xport_set_headless(headless);
    if (ResetGraph(0) < 0)
        return 2;
    if (headless && !getenv("RRJ_LIVE_FRAME_LIMIT"))
        rrj_trace_set_live_limit(60);
    result = first_race_play(m);
    if (headless && result == 0)
    {
        printf("First race: draws=%u\n", live_race_draw_calls);
        if (!live_race_draw_calls || !save_live_frame("../status/first-race-smoke"))
            result = 2;
    }
    return result;
}

/* Diagnostic packet replay only; this does not run game logic. */
/* Test-only video seam: start/update calls are intercepted in GDB too.
 * Stop is verified only with original inactive-video flags. */
static uint32_t probe_video(RRJMemory *m, uint32_t target, uint32_t descriptor)
{
    if (m->sdk_user && target != 0x8006FE6C)
        record_sdk(m, target, descriptor, 0);
    return rrj_video_dummy(m, target, descriptor);
}

static void native_menu_draw(RRJMemory *m, uint32_t target, uint32_t menu, uint32_t entry)
{
    rrj_render_menu_entry(m, target, menu, entry, item_resource, probe_video);
}

static uint32_t live_video_call(RRJMemory *, uint32_t, const uint32_t args[9]);
static uint32_t live_video_open(RRJMemory *, uint32_t, uint32_t);
static uint32_t live_queue_call(RRJMemory *, uint32_t, uint32_t, uint32_t);

static uint32_t native_menu_frame(RRJMemory *m, uint32_t target, uint32_t menu, uint32_t a1)
{
    if (target == 0x80078BD8u)
        return sub_F_80078BD8(m, menu, a1, live_queue_call);
    if (target == 0x80078E80u)
        return sub_F_80078E80(m, menu, live_queue_call);
    if (target == 0x80022A78)
    {
        /* WIP CD queue wait: dummy reads create no outstanding native I/O. */
        if (menu != 2)
            abort();
        return 0;
    }
    if (target == 0x8006DB5C)
        return sub_F_8006DB5C(m, menu, live_video_call, live_video_open);
    if (target == 0x8006DE5C)
        return sub_F_8006DE5C(m, menu, live_video_call);
    if (target == 0x8006D5B0)
        return sub_F_8006D5B0(m, menu, native_menu_draw);
    if (target == 0x8006D630)
        return sub_F_8006D630(m, menu, native_menu_draw);
    RRJ_WIP3(m, target, "menu_frame", menu, a1, 0);
    return 0;
}

static void update_music(RRJMemory *m, uint32_t track)
{
    if (!m->sdk_call)
        abort();
    m->sdk_call(m, 0x8007EDE0, track, 0);
}

static uint32_t native_submenu(RRJMemory *m, uint32_t fn, uint32_t a, uint32_t b, uint32_t c)
{
    if (fn == 0x8007EAC0)
        return sub_F_8007EAC0(m, a);
    if (fn == 0x8006B03C)
        return sub_F_8006B03C(m, a, b, c, update_music);
    RRJ_WIP3(m, fn, "submenu", a, b, c);
    return 0;
}

static uint32_t native_menu_update(RRJMemory *m, uint32_t target, uint32_t menu)
{
    if (target == 0x8006DE5C)
        return sub_F_8006DE5C(m, menu, live_video_call);
    if (target == 0x8006AE6C)
        return sub_F_8006AE6C(m, menu, native_submenu);
    if (target == 0x8006A8FC)
        return sub_F_8006A8FC(m, menu, sub_F_8007EAC0);
    if (target == 0x80069418)
        return sub_F_80069418(m, menu, update_music);
    RRJ_WIP3(m, target, "menu_update", menu, 0, 0);
    return 0;
}

static int render_ot(RRJMemory *memory, char **argv)
{
    char *cursor = argv[4], *end;
    if (!read_file(argv[2], DRAM, sizeof(DRAM)) || !read_file(argv[3], VRAM, sizeof(VRAM)))
        return 2;
    gpu_begin();
    do
    {
        uint32_t address = (uint32_t)strtoul(cursor, &end, 16);
        if (end == cursor || !rrj_gpu_draw_ot(memory, address, atoi(argv[5]), atoi(argv[6])))
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
        rrj_write32(memory, 0x800F0004, 0x800FFFF0);
        for (i = 0; i < 4; ++i)
            rrj_put16(rrj_at(memory, 0x800F0010 + i * 2, 2), rectangles[test][i]);
        for (i = 0; i < w * h; ++i)
            rrj_put16(rrj_at(memory, 0x80100000 + i * 2, 2), i * 97 + 3);
        if (sub_F_80065768(memory, 0x800F0000, rrj_gpu_upload) != 0)
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
    puts("GPU upload integration: 3 transfers, 1572864 VRAM word checks PASS");
    for (test = 0; test < 2; ++test)
    {
        uint32_t id = test ? 0x8001 : 1, expected_return = test ? 0x800D76D0 : 0;
        memset(VRAM, 0x5A, sizeof(VRAM));
        rrj_write32(memory, 0x8005B2F8, 0x801E0000);
        rrj_write32(memory, 0x801E0030, 1);
        xport_guest_fill(0x800533B4, 0, 16);
        for (i = 0; i < 24; ++i)
            rrj_write32(memory, 0x800D9270 + 48 * i, 0xFFFFFFFF);
        rrj_write32(memory, 0x800D9268, 0xFFFFFFFF);
        for (i = 0; i < 8192; ++i)
        {
            rrj_put16(rrj_at(memory, 0x80100000 + 2 * i, 2), i * 97 + 3);
            rrj_put16(rrj_at(memory, 0x80104000 + 2 * i, 2), i * 31 + 7);
        }
        if (sub_8002227C(memory, 0, id, 0x80100000, 0x80104000, 0, rrj_gpu_upload) != expected_return)
            return 2;
        if (rrj_read32(memory, 0x800D9268) != 0 || rrj_read32(memory, 0x800D9270) != id)
            return 2;
        if (test && rrj_u16(rrj_at(memory, 0x800D76D6, 2)) != 0)
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
    puts("Race image upload 8002227C: 3 transfers, 1048576 VRAM word checks PASS");
    return 0;
}

static int input_smoke(RRJMemory *m, const char *fixture)
{
    uint32 buttons = 0, state, frame, events = 0;
    if (!read_file(fixture, DRAM, sizeof(DRAM)))
        return 2;
    xport_set_headless(1);
    PadInit(0);
    state = rrj_read32(m, 0x8005B2F8);
    w_u8(state, 2);
    rrj_write32(m, state + 52, 1);
    xport_guest_fill(0x800D6DE0, 0, 768);
    /* Make the first press a single-click event, as in initialized game data. */
    w_u8(0x800D6E09, 30);
    for (frame = 0; frame < 80; ++frame)
    {
        unsigned expected = frame == 0 ? 1 : (frame >= 32 && frame < 70 && (frame - 32) % 3 == 0) ? 255 : 0;
        unsigned actual;
        buttons = frame < 70 ? PADLup : 0;
        xport_input_override(1, buttons);
        rrj_write32(m, state + 12, frame + 1);
        rrj_input_read(m);
        rrj_input_latch(m);
        actual = r_u8(0x800D7152);
        if (actual != expected)
        {
            fprintf(stderr, "input frame %u: %u != %u\n", frame, actual, expected);
            return 2;
        }
        if (actual)
            ++events;
        if (r_u8(0x800D6E0A) != 0 || rrj_read32(m, 0x800D712C) != (frame < 70 ? (uint32)PADLup : 0u))
            return 2;
    }
    for (frame = 0; frame < 15; ++frame)
    {
        uint32 record = 0x800D6DF4 + 8 * frame;
        xport_guest_fill(0x800D6DE0, 0, 192);
        w_u8(record + 5, 30);
        buttons = rrj_read32(m, 0x80052658 + 4 * frame);
        xport_input_override(1, buttons);
        rrj_write32(m, state + 12, 100 + frame);
        rrj_input_read(m);
        rrj_input_latch(m);
        if (r_u8(0x800D7142 + 8 * frame) != 1 || rrj_read32(m, 0x800D712C) != buttons)
            return 2;
        buttons = 0;
        xport_input_override(1, buttons);
        rrj_input_read(m);
        rrj_input_latch(m);
        if (r_u8(0x800D7142 + 8 * frame) != 0)
            return 2;
    }
    printf("Host input integration: 80 frames, %u events, 15 mappings, press/repeat/release PASS\n", events);
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
    m->sdk_call = rrj_spu_command;
    handle = sub_F_8007EAC0(m, 0);
    if (!handle)
        return 2;
    (void)sub_8001EE94(m, rrj_spu_setup, rrj_spu_command);
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
    sub_8001EB44(m, 1u << (handle >> 27));
    (void)sub_8001EE94(m, rrj_spu_setup, rrj_spu_command);
    printf("SPU menu sound: frames=44100 nonzero=%u peak=%u handle=%08X PASS\n", nonzero, peak, handle);
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
        live_origin_x = rrj_s32(state[0]);
        live_origin_y = rrj_s32(state[1]);
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

static uint32_t live_music(RRJMemory *m, uint32_t fn, uint32_t args[8])
{
    if (fn == 0x8007F158)
        return sub_F_8007F158(m, args[0], rrj_spu_setup, rrj_spu_reverb);
    if (fn == 0x80022D20)
    {
        /* WIP CD read dummy: fail explicitly, do not invoke a success callback
         * or fabricate loaded SPU/music data. Game retains pending state. */
        if (!live_music_read_failures)
            fputs("WIP CD music reads unavailable\n", stderr);
        ++live_music_read_failures;
        return 0xffffffff;
    }
    if (fn == 0x80022A1C || fn == 0x80022A78)
        return 0; /* WIP CD queue owns no native I/O. */
    RRJ_WIP(m, fn, "callee", "music", "skip_call_return_zero", args, 8);
    return 0;
}

static uint32_t live_reset(RRJMemory *m, uint32_t fn, uint32_t a, uint32_t b, uint32_t c)
{
    if (fn == 0x8001E100)
        return sub_8001E100(m, a, b, c);
    if (fn == 0x800630C0)
        return sub_F_800630C0(m, a, b);
    if (fn == 0x80080A70)
        return sub_F_80080A70(m, a);
    RRJ_WIP3(m, fn, "reset", a, b, c);
    return 0;
}

static void live_sdk(RRJMemory *m, uint32_t fn, uint32_t a0, uint32_t a1)
{
    switch (fn)
    {
        case 0x80048DB4:
            if (!rrj_gpu_draw_ot(m, a0, live_origin_x, live_origin_y))
                abort();
            return;
        case 0x80048CAC:
            rrj_gpu_clear_ot(m, a0, a1);
            return;
        case 0x800487C0:
            return;
        case 0x80043DA4:
            sub_80043DA4(m);
            return;
        case 0x80043DB4:
            sub_80043DB4(m);
            return;
        case 0x80050D08:
        case 0x80050678:
            rrj_spu_command(m, fn, a0, a1);
            return;
        case 0x8006883C:
            (void)sub_F_8006883C(m, a0, live_reset);
            return;
        case 0x8002D250:
            (void)sub_8002D250(m);
            return;
        case 0x8007EDE0:
            (void)sub_F_8007EDE0(m, a0, live_music);
            return;
        case 0x8007F158:
            (void)sub_F_8007F158(m, a0, rrj_spu_setup, rrj_spu_reverb);
            return;
        default:
            RRJ_WIP3(m, fn, "live_effect", a0, a1, 0);
            return;
    }
}

static uint32_t live_service(RRJMemory *m, uint32_t fn, uint32_t a0)
{
    if (fn == 0x80019990)
    {
        /* Verified MIPS branch199B0->19C30. Engine modulation path not translated. */
        if (rrj_read32(m, 0x8005ACA8) & 4)
        {
            RRJ_WIP(m, 0x80019990, "function", "engine_audio", "return_zero_skip_service", &a0, 1);
            return 0;
        }
        return sub_8001EE94(m, rrj_spu_setup, rrj_spu_command);
    }
    if (fn == 0x8001C5F8)
    {
        rrj_input_read(m);
        return 0;
    }
    if (fn == 0x80048E24)
    {
        unsigned x = rrj_u16(rrj_at(m, a0, 2)), y = rrj_u16(rrj_at(m, a0 + 2, 2));
        live_origin_x = (int)x;
        live_origin_y = (int)y;
        gpu_put_draw_env(a0, (int)x, (int)y);
        return a0;
    }
    if (fn == 0x80048FF0)
        return a0; /* framebuffer presentation below; visual timing WIP */
    live_sdk(m, fn, a0, 0);
    return 0;
}

static uint32_t live_stream_call(RRJMemory *, uint32_t, const uint32_t[9]);

static uint32_t live_queue_call(RRJMemory *m, uint32_t fn, uint32_t a0, uint32_t a1)
{
    if (fn == 0x80061C44u)
    {
        uint32_t args[9] = {a0, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u};
        return live_stream_call(m, fn, args);
    }
    /* TODO Bind the original resource and decoder adapters */
    fprintf(stderr, "RRJ menu: missing adapter %08X (%08X, %08X)\n", fn, a0, a1);
    abort();
}

static void live_loop(RRJMemory *m, uint32_t fn, uint32_t a0, uint32_t a1)
{
    switch (fn)
    {
        case 0x8001CB3C:
            rrj_input_latch(m);
            return;
        case 0x8001C428:
            (void)sub_8001C428(m);
            return;
        case 0x800667E4:
            (void)sub_F_800667E4(m, native_menu_update, live_sdk);
            return;
        case 0x80066C34:
            sub_F_80066C34(m, native_menu_frame);
            return;
        case 0x8001C3F4:
            (void)sub_8001C3F4(m);
            return;
        case 0x8001C408:
        {
#if defined(LOCKSTEP_DEBUG)
            if (xport_run_mode() == RUN_MODE_LOCKSTEP_REPLAY)
            {
                (void)sub_8001C408(m, rrj_lockstep_poll);
                return;
            }
#endif
            uint32_t context = rrj_read32(m, 0x8005B470); /* cached by original1C408 */
            while (!r_u8(context + 4))
            {
                VSync(0);
                (void)sub_F_80064C30(m, live_service);
            }
            return;
        }
        case 0x8006711C:
            sub_F_8006711C(m, live_sdk);
            return;
        case 0x80080488:
            (void)sub_F_80080488(m, live_service);
            return;
        case 0x800803FC:
            sub_F_800803FC(m);
            return;
        case 0x80062774:
            sub_F_80062774(m, a0, a1, live_queue_call);
            return;
        default:
            live_sdk(m, fn, a0, a1);
            return;
    }
}

static uint32_t live_screen_call(RRJMemory *m, uint32_t fn, uint32_t arg)
{
    if (fn == 0x80048428)
        return sub_80048428(m);
    if (fn == 0x80047724)
    {
        SetDispMask((sint32)arg);
        return 0;
    }
    if (fn == 0x8001C408)
    {
        live_loop(m, fn, 0, 0);
        return 0;
    }
    if (fn == 0x80048944)
    {
        unsigned x = rrj_u16(rrj_at(m, arg, 2)), y = rrj_u16(rrj_at(m, arg + 2, 2));
        unsigned w = rrj_u16(rrj_at(m, arg + 4, 2)), h = rrj_u16(rrj_at(m, arg + 6, 2)), row;
        if (x > 1024 || y > 512 || w > 1024 - x || h > 512 - y)
            abort();
        for (row = 0; row < h; ++row)
            memset(VRAM + (y + row) * 1024 + x, 0, w * 2);
        gpu_begin();
        return 0;
    }
    live_sdk(m, fn, arg, 0);
    return 0;
}

static void live_screen_env(RRJMemory *m, uint32_t fn, uint32_t addr, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
    if (fn == 0x8004CC44)
    {
        (void)sub_8004CC44(m, addr, x, y, w, h, live_screen_call);
        return;
    }
    if (fn == 0x8004CD04)
    {
        (void)sub_8004CD04(m, addr, x, y, w, h);
        return;
    }
    RRJ_WIP(m, fn, "callee", "screen", "skip_call", ((const uint32_t[]){addr, x, y, w, h}), 5);
}

static uint32_t live_video_open(RRJMemory *m, uint32_t format, uint32_t name)
{
    /* WIP CD dummy: every open fails; no host filesystem operation. */
    return 0xffffffff;
}

static void live_video_stop(RRJMemory *m, uint32_t fn, uint32_t a, uint32_t b)
{
    /* WIP STR/MDEC shutdown and CD close: no native decoder/file is owned. */
    if (fn != 0x8005F7E0 && fn != 0x8001460C)
    {
        RRJ_WIP3(m, fn, "video_stop", a, b, 0);
    }
}

static uint32_t live_stream_bytes(RRJMemory *m, uint32_t fn, uint32_t argument, const uint8_t *bytes, uint32_t size)
{
    /* TODO Bind diagnostic DrawPrim and CD control byte buffers */
    fprintf(stderr, "Missing STR byte adapter %08X (%08X, %u bytes)\n", fn, argument, size);
    abort();
}

static void live_stream_header(RRJMemory *m, uint8_t header[12])
{
    /* TODO Bind the twelve-byte physical CD sector header */
    fprintf(stderr, "Missing STR CD sector header adapter\n");
    abort();
}

static uint32_t live_stream_call(RRJMemory *m, uint32_t fn, const uint32_t args[9])
{
    if (fn == 0x800611F4u)
        return sub_F_800611F4(m, args[0], live_stream_call, live_stream_header);
    if (fn == 0x80043DD4u)
    {
        m->cpu_status &= 0xfffffbfeu;
        return 0u;
    }
    if (fn == 0x80043DF4u)
    {
        m->cpu_status |= 0x401u;
        return 0u;
    }
    if (fn == 0x80061584u)
        return sub_F_80061584(m, args[0], live_stream_call);
    if (fn == 0x80061060u)
        return sub_F_80061060(m, args[0], args[1], args[2], live_stream_call);
    if (fn == 0x80061180u)
        return sub_F_80061180(m, args[0], args[1], args[2], live_stream_call);
    if (fn == 0x80060F44u)
        return sub_F_80060F44(m, args[0], args[1], args[2], live_stream_call);
    if (fn == 0x80061148u)
        return sub_F_80061148(m, args[0], args[1], args[2], live_stream_call);
    if (fn == 0x800611B8u)
        return sub_F_800611B8(m, args[0], args[1], args[2], live_stream_call);
    if (fn == 0x80061818u)
        return sub_F_80061818(m);
    if (fn == 0x8001E0B4u)
        return sub_8001E0B4(m, args[0], args[1], args[2]);
    if (fn == 0x80061C44u)
        return sub_F_80061C44(m, args[0], live_stream_call, live_stream_bytes);
    if (fn == 0x80061938u)
    {
        sub_F_80061938(m, live_stream_call, live_stream_bytes);
        return 0u;
    }
    /* TODO Bind CD command, audio and VLC adapters */
    fprintf(stderr, "Missing STR adapter %08X (%08X, %08X, %08X)\n", fn, args[0], args[1], args[2]);
    abort();
}

static uint32_t live_stream_read(RRJMemory *m, uint32_t *length)
{
    return sub_F_80061F04(m, length, live_stream_call);
}

static uint32_t live_video_call(RRJMemory *m, uint32_t fn, const uint32_t args[9])
{
    switch (fn)
    {
        case 0x80022A78:
            if (args[0])
                abort();
            return rrj_read32(m, 0x80053464); /* actual non-wait MIPS branch */
        case 0x8005F36C:
            return 0; /* WIP STR/MDEC: failure / end-of-stream */
        case 0x8005F484:
            return sub_F_8005F484(m, args[0], args[1], args[2], args[3], args[4], live_stream_read, live_stream_call);
        case 0x8005F7E0:
        case 0x8001460C:
            live_video_stop(m, fn, args[0], 0);
            return 0;
        case 0x80080D08:
            return sub_F_80080D08(m, live_screen_call);
        case 0x8001BE08:
            return sub_8001BE08(m, args[0], args[1], args[2], args[3], live_screen_call, live_screen_env);
        case 0x8001BF1C:
            return sub_8001BF1C(m, args[0], args[1], args[2], args[3], live_screen_call);
        case 0x8001C3F4:
            return sub_8001C3F4(m);
        case 0x8001C408:
            live_loop(m, fn, 0, 0);
            return 0;
        case 0x8006DE5C:
            return sub_F_8006DE5C(m, args[0], live_video_call);
        default:
            RRJ_WIP(m, fn, "callee", "video_effect", "skip_call_return_zero", args, 9);
            return 0;
    }
}

static uint32_t live_video(RRJMemory *m, uint32_t fn, uint32_t desc)
{
    switch (fn)
    {
        case 0x8006FE6C:
            return sub_F_8006FE6C(m, live_video_stop);
        case 0x8006FED4:
            return sub_F_8006FED4(m, live_video_stop);
        case 0x8006FEF4:
            return sub_F_8006FEF4(m, desc, live_video_call, live_video_open, live_video_stop);
        case 0x80070018:
            return sub_F_80070018(m, desc, live_video_call, live_video_open, live_video_stop);
        default:
            RRJ_WIP3(m, fn, "video_dispatch", desc, 0, 0);
            return 0;
    }
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
    const char *path = getenv("XPORT_MEMORY_OUTPUT");
    const char *manifest = getenv("XPORT_MEMORY_MANIFEST");
    const char *interval = getenv("XPORT_MEMORY_KEYFRAME_INTERVAL");
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
    if (!full_frame_output || fwrite(row, sizeof(row), 1, full_frame_output) != 1 || fwrite(&full_gpu_bytes, sizeof(full_gpu_bytes), 1, full_frame_output) != 1 || fflush(full_frame_output) || fflush(full_gpu_output))
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
    const char *directory = getenv("RRJ_FULL_ORIGIN");
    const char *gpu_path = getenv("RRJ_FULL_GPU_OUTPUT");
    const char *frame_path = getenv("RRJ_FULL_FRAMES_OUTPUT");
    const char *limit = getenv("RRJ_FULL_INTERVALS");
    const char *ot_path = getenv("RRJ_FULL_OT_SCHEDULE");
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
    m->sdk_call = live_sdk;
    rrj_video_bind(live_video);
    full_frame_events = full_intervals = 0;
    full_gpu_bytes = 0;
    full_failed = 0;
    full_ot_unread = ot_header[2];
    full_ot_has_next = full_ot_consumed = full_ot_pending_count = 0;
    full_current_frame = full_resume[2];
    if (!full_ot_read_next())
        full_failed = 1;
    psx_vblank_bind(full_vblank, NULL, rrj_read32(m, 0x8005B46C));
    gpu_set_transfer_observer(full_gpu_transfer, NULL);
    gpu_bind_linked_submit(full_ot_submit, NULL);
    if (setjmp(full_stop) == 0)
    {
        (void)sub_F_80064C30(m, live_service);
        live_loop(m, 0x8001C408, 0, 0);
        rrj_write32(m, 0x80088C44, 0);
        rrj_menu_finish_iteration(m, live_loop);
        while (r_u8(rrj_read32(m, 0x8005B2F8)) == 2)
        {
            rrj_menu_iteration(m, live_loop);
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
    printf("full_origin_probe frame=%u intervals=%u submissions=%u failed=%d full_convergence=0\n", full_resume[2], full_intervals, full_ot_consumed, full_failed);
    return full_failed ? 24 : 25;
}

static int menu_run(RRJMemory *m, char **argv, int navigation)
{
    uint32_t buttons = 0, last_selection = 0xffffffff;
    int trace = navigation == 1 || getenv("RRJ_TRACE_INPUT") != NULL;
    int enter_first_race = 0;
    static uint8_t spu[524288];
    unsigned frame, limit = (unsigned)strtoul(argv[5], NULL, 10);
    if (!read_file(argv[2], DRAM, sizeof(DRAM)) || !read_file(argv[3], VRAM, sizeof(VRAM)) || !read_file(argv[4], spu, sizeof spu))
        return 2;
    xport_set_headless(navigation == 2);
    if (ResetGraph(0) < 0)
        return 2;
    PadInit(0);
    rrj_spu_initialize(spu);
    if (!xport_audio_init())
    {
        fprintf(stderr, "Windows audio device initialization failed\n");
        return 2;
    }
    m->sdk_call = live_sdk;
    rrj_video_bind(live_video);
    for (frame = 0; frame < limit && !xport_isquit(); ++frame)
    {
        rrj_wip_frame(frame);
        if (r_u8(rrj_read32(m, 0x8005B2F8)) != 2)
            break;
        if (navigation == 1)
            buttons = frame >= 5 && frame < 10 ? PADLdown : frame >= 20 && frame < 25 ? PADLup : 0;
        if (navigation)
            xport_input_override(1, buttons);
        gpu_begin();
        rrj_menu_iteration(m, live_loop);
        if (rrj_u16(rrj_at(m, 0x8009C5D0, 2)) == 57)
        {
            enter_first_race = 1;
            break;
        }
        if (trace)
        {
            uint32_t menu = rrj_read32(m, rrj_read32(m, 0x8009C68C) + 4 * rrj_u16(rrj_at(m, 0x8009C5D0, 2)));
            uint32_t selection = rrj_u16(rrj_at(m, menu + 4, 2));
            if (selection != last_selection)
            {
                printf("navigation frame=%u selection=%u\n", frame, selection);
                last_selection = selection;
                fflush(stdout);
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
    xport_audio_shutdown();
    if (!xport_isquit() && (enter_first_race || r_u8(rrj_read32(m, 0x8005B2F8)) != 2))
        return first_race_play(m);
    printf("audio submitted=%u nonzero=%u peak=%u active=%u overruns=%u\n", g_xport_audio_submitted_buffers, g_xport_audio_nonzero_buffers, g_xport_audio_peak, g_xport_audio_backend_active, g_xport_audio_callback_overruns);
    printf("WIP native menu: iterations=%u menu=%u vblanks=%u; audio device stopped\n", frame, (unsigned)rrj_u16(rrj_at(m, 0x8009C5D0, 2)), rrj_read32(m, 0x8005B46C));
    printf("WIP CD music reads failed=%u\n", live_music_read_failures);
    return !argv[6] || (write_file(argv[6], DRAM, sizeof(DRAM)) && save_live_frame(argv[6])) ? 0 : 2;
}

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
            rrj_put32(output + 4 * i, fields[i]);
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
        return input_smoke(&memory, argv[2]);
    if (argc == 2 && strcmp(argv[1], "--gpu-upload-smoke") == 0)
        return upload_smoke(&memory);
    if (argc == 2 && strcmp(argv[1], "--first-race") == 0)
        return first_race_run(&memory, 0);
    if (argc == 2 && strcmp(argv[1], "--first-race-smoke") == 0)
        return first_race_run(&memory, 1);
    if (argc == 8 && strcmp(argv[1], "--render-ot") == 0)
        return render_ot(&memory, argv);
    if ((argc == 3 || argc == 4) && strcmp(argv[1], "--platform-smoke") == 0)
        return rrj_platform_smoke((unsigned)strtoul(argv[2], NULL, 10), argc == 4 && strcmp(argv[3], "--headless") == 0);
    if (argc == 1)
    {
        char *menu_args[] = {argv[0], "--menu-run", "../status/menu/state1-menu-ram.bin", "../status/menu/state1-vram.bin", "../status/menu/state1-spu.bin", "4294967295", NULL};
        return menu_run(&memory, menu_args, 0);
    }
    if (argc == 2 && strcmp(argv[1], "--version") == 0)
    {
        puts("Road Rash: Jailbreak - native C State1 menu, Debug x86; CD/card/video WIP.");
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
        args[i] = rrj_u32(argument_bytes + i * 4);
    if (argc == 8 || argc == 10)
    {
        memory.sdk_user = xport_fopen(argv[7], "w");
        if (!memory.sdk_user)
            return 2;
        memory.sdk_call = record_sdk;
    }
    switch (function)
    {
        case 0x80037338:
            result = sub_80037338(&memory);
            break;
        case 0x80012524:
            race_step_reply = args[0];
            result = sub_80012524(&memory, probe_race_step);
            break;
        case 0x8002305C:
            memcpy(race_pause_replies, args, sizeof(race_pause_replies));
            result = sub_8002305C(&memory, probe_race_pause);
            break;
        case 0x800324CC:
            result = sub_800324CC(&memory, args[0], args[1], args[2]);
            break;
        case 0x800335B4:
            result = sub_800335B4(&memory, args[0], args[1], args[2]);
            break;
        case 0x800329BC:
            result = sub_800329BC(&memory, args[0], args[1]);
            break;
        case 0x80032338:
            result = sub_80032338(&memory, args[0], args[1]);
            break;
        case 0x8003234C:
            result = sub_8003234C(&memory, args[0], args[1]);
            break;
        case 0x800325BC:
            result = sub_800325BC(&memory, args[0], args[1]);
            break;
        case 0x80013204:
            result = sub_80013204(&memory, args[0], args[1]);
            break;
        case 0x80013360:
            result = sub_80013360(&memory, args[0], args[1]);
            break;
        case 0x80012BA8:
            result = sub_80012BA8(&memory, args[0], args[1], args[2]);
            break;
        case 0x80013110:
            result = sub_80013110(&memory, args[0], args[1], args[2], args[3], args[4]);
            break;
        case 0x80013294:
            result = sub_80013294(&memory, args[0], args[1], args[2]);
            break;
        case 0x8001339C:
            result = sub_8001339C(&memory, args[0], args[1]);
            break;
        case 0x800135E8:
            result = sub_800135E8(&memory, args[0], args[1]);
            break;
        case 0x80032A20:
            result = sub_80032A20(&memory, args);
            break;
        case 0x80039A08:
            result = sub_80039A08(&memory, args[0]);
            break;
        case 0x8003CFCC:
            result = sub_8003CFCC(&memory, args[0], args[1]);
            break;
        case 0x80093E6C:
            result = args[5] ? sub_80093E6C(&memory, probe_reverb) : rrj_probe_actor_activity(&memory, probe_pad_sdk);
            break;
        case 0x80093ED4:
            sub_80093ED4(&memory, args[0], args[1], probe_reverb);
            result = 0;
            break;
        case 0x80093F94:
            sub_80093F94(&memory, args[0], args[1], probe_reverb);
            result = 0;
            break;
        case 0x800C2FF4:
            result = sub_800C2FF4(&memory, args[0], args[1], args[2]);
            break;
        case 0x80012858:
            result = sub_80012858(&memory, args[0], args[1]);
            break;
        case 0x80093FE4:
            sub_80093FE4(&memory, args[0], probe_reverb);
            result = 0;
            break;
        case 0x800A0708:
            result = sub_800A0708(&memory, args[0], probe_reverb);
            break;
        case 0x8001FC58:
            result = sub_8001FC58(&memory);
            break;
        case 0x80039C38:
            result = sub_80039C38(&memory, args[0]);
            break;
        case 0x8003F580:
            result = sub_8003F580(&memory, args[0], args[1]);
            break;
        case 0x8003F5D0:
            result = sub_8003F5D0(&memory, args[0]);
            break;
        case 0x8003C840:
            result = sub_8003C840(&memory, args[0], args[1], args[2]);
            break;
        case 0x8003C948:
            result = sub_8003C948(&memory, args[0], args[1], args[2]);
            break;
        case 0x8003F408:
            result = sub_8003F408(&memory, args[0], args[1]);
            break;
        case 0x8003CCA0:
            if (args[5])
            {
                uint32_t candidates[24];
                for (i = 0; i < 24; ++i)
                    candidates[i] = rrj_read32(&memory, args[1] + 4 * i);
                result = rrj_actor_route_choice_local(&memory, args[0], candidates, args[2]);
            }
            else
                result = sub_8003CCA0(&memory, args[0], args[1], args[2]);
            break;
        case 0x80039048:
            if (args[6])
            {
                uint32_t candidates[24], directions[3], output[8], previous[8], direction_out, previous_direction;
                for (i = 0; i < 24; ++i)
                    candidates[i] = rrj_read32(&memory, args[1] + 4 * i);
                for (i = 0; i < 3; ++i)
                    directions[i] = rrj_read32(&memory, args[2] + 4 * i);
                for (i = 0; i < 8; ++i)
                {
                    output[i] = rrj_read32(&memory, args[4] + 4 * i);
                    previous[i] = rrj_read32(&memory, args[1] - 32 + 4 * i);
                }
                direction_out = rrj_read32(&memory, args[5]);
                previous_direction = rrj_read32(&memory, args[2] - 4);
                result = rrj_commit_track_local(&memory, args[0], candidates, directions, args[3], output, &direction_out, previous, &previous_direction);
                for (i = 0; i < 8; ++i)
                    rrj_write32(&memory, args[4] + 4 * i, output[i]);
                rrj_write32(&memory, args[5], direction_out);
            }
            else
                result = sub_80039048(&memory, args[0], args[1], args[2], args[3], args[4], args[5]);
            break;
        case 0x80039BB0:
            result = sub_80039BB0(&memory, args[0], args[1], args[2], args[3]);
            break;
        case 0x8003775C:
            result = sub_8003775C(&memory, args[0], args[1], args[2]);
            break;
        case 0x80037A30:
            if (args[5])
            {
                uint32_t record[8], candidates[24], directions[3];
                for (i = 0; i < 8; ++i)
                    record[i] = rrj_read32(&memory, args[1] + 4 * i);
                for (i = 0; i < 24; ++i)
                    candidates[i] = rrj_read32(&memory, args[2] + 4 * i);
                for (i = 0; i < 3; ++i)
                    directions[i] = rrj_read32(&memory, args[3] + 4 * i);
                result = rrj_traverse_track_local(&memory, args[0], record, candidates, directions, args[4], 0);
                for (i = 0; i < 24; ++i)
                    rrj_write32(&memory, args[2] + 4 * i, candidates[i]);
                for (i = 0; i < 3; ++i)
                    rrj_write32(&memory, args[3] + 4 * i, directions[i]);
            }
            else
                result = sub_80037A30(&memory, args[0], args[1], args[2], args[3], args[4]);
            break;
        case 0x80037FBC:
            if (args[5])
            {
                uint32_t record[8], candidates[24], directions[3];
                for (i = 0; i < 8; ++i)
                    record[i] = rrj_read32(&memory, args[1] + 4 * i);
                for (i = 0; i < 24; ++i)
                    candidates[i] = rrj_read32(&memory, args[2] + 4 * i);
                for (i = 0; i < 3; ++i)
                    directions[i] = rrj_read32(&memory, args[3] + 4 * i);
                result = rrj_traverse_track_local(&memory, args[0], record, candidates, directions, args[4], 1);
                for (i = 0; i < 24; ++i)
                    rrj_write32(&memory, args[2] + 4 * i, candidates[i]);
                for (i = 0; i < 3; ++i)
                    rrj_write32(&memory, args[3] + 4 * i, directions[i]);
            }
            else
                result = sub_80037FBC(&memory, args[0], args[1], args[2], args[3], args[4]);
            break;
        case 0x8003A5F4:
            if (args[5])
            {
                uint32_t id;
                result = rrj_nearest_junction_local(&memory, args[0], &id, args[2]);
                rrj_write32(&memory, args[1], id);
            }
            else
                result = sub_8003A5F4(&memory, args[0], args[1], args[2]);
            break;
        case 0x8003A9D8:
            result = sub_8003A9D8(&memory, args[0]);
            break;
        case 0x8003DCB8:
            result = sub_8003DCB8(&memory, args[0], args[1]);
            break;
        case 0x8003B61C:
            result = sub_8003B61C(&memory, args[0]);
            break;
        case 0x8003B1C4:
            sub_8003B1C4(&memory, args[0], args[1], args[2]);
            result = 0;
            break;
        case 0x8003AF9C:
            sub_8003AF9C(&memory, args[0], args[1], args[2]);
            result = 0;
            break;
        case 0x8003B024:
            sub_8003B024(&memory, args[0], args[1], args[2], args[3]);
            result = 0;
            break;
        case 0x8003F4D8:
            result = sub_8003F4D8(&memory, args[0]);
            break;
        case 0x8003DDB0:
            result = sub_8003DDB0(&memory, args[0]);
            break;
        case 0x8003DFF4:
            result = sub_8003DFF4(&memory, args[0]);
            break;
        case 0x8003DE28:
            if (args[5])
            {
                uint32_t record[8];
                for (i = 0; i < 8; ++i)
                    record[i] = rrj_read32(&memory, args[2] + 4 * i);
                result = rrj_update_contact_local(&memory, args[0], args[1], record, args[3]);
                for (i = 0; i < 8; ++i)
                    rrj_write32(&memory, args[2] + 4 * i, record[i]);
            }
            else
                result = sub_8003DE28(&memory, args[0], args[1], args[2], args[3]);
            break;
        case 0x8003EE68:
            result = sub_8003EE68(&memory, args[0], args[1], args[2], args[3]);
            break;
        case 0x8003E754:
            result = sub_8003E754(&memory, args[0], args[1], args[2], args[3], args[4]);
            break;
        case 0x8003701C:
            result = sub_8003701C(&memory, args[0]);
            break;
        case 0x80036B14:
            if (args[5])
            {
                uint32_t record[8];
                for (i = 0; i < 8; ++i)
                    record[i] = rrj_read32(&memory, args[1] + 4 * i);
                result = rrj_follow_track_local(&memory, args[0], record, args[2]);
                for (i = 0; i < 8; ++i)
                    rrj_write32(&memory, args[1] + 4 * i, record[i]);
            }
            else
                result = sub_80036B14(&memory, args[0], args[1], args[2]);
            break;
        case 0x8003697C:
            result = sub_8003697C(&memory, args[0], args[1], args[2], args[3]);
            break;
        case 0x80036800:
            sub_80036800(&memory, args[0], args[1], args[2], args[3]);
            result = 0;
            break;
        case 0x8003E45C:
            result = sub_8003E45C(&memory, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]);
            break;
        case 0x8003ED14:
            result = sub_8003ED14(&memory, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8]);
            break;
        case 0x8003BD2C:
            result = sub_8003BD2C(&memory, args[0], args[1], args[2], args[3], args[4]);
            break;
        case 0x8003E61C:
            result = sub_8003E61C(args[0], args[1], args[2]);
            break;
        case 0x8003F204:
            sub_8003F204(&memory, args[0], args[1]);
            result = 0;
            break;
        case 0x8003DF54:
            sub_8003DF54(&memory, args[0], args[1], args[2]);
            result = 0;
            break;
        case 0x8003F1F0:
            result = sub_8003F1F0(&memory, args[0]);
            break;
        case 0x800B6AAC:
            result = sub_800B6AAC(&memory, args[0], args[1], args[2]);
            break;
        case 0x8003EB58:
            result = sub_8003EB58(&memory, args[0], args[1], args[2], args[3], args[4]);
            break;
        case 0x8003E67C:
            result = sub_8003E67C(&memory, args[0], args[1], args[2], args[3]);
            break;
        case 0x8003EF34:
            result = sub_8003EF34(&memory, args[0], args[1], args[2]);
            break;
        case 0x8003A468:
            result = sub_8003A468(&memory, args[0], args[1]);
            break;
        case 0x800951B8:
            sub_800951B8(&memory, args[0], args[1], probe_reverb);
            result = 0;
            break;
        case 0x800952AC:
            sub_800952AC(&memory, args[0], args[1], probe_reverb);
            result = 0;
            break;
        case 0x800903F4:
            result = sub_800903F4(&memory, args[0], args[1], probe_reverb);
            break;
        case 0x800BCA68:
            result = sub_800BCA68(&memory, args[0], args[1], args[2]);
            break;
        case 0x800C3104:
            result = sub_800C3104(&memory, args[0], args[1]);
            break;
        case 0x800BC8DC:
            sub_800BC8DC(&memory, args[0]);
            result = 0;
            break;
        case 0x800C1014:
            result = sub_800C1014(&memory, args[0]);
            break;
        case 0x800BFD24:
            result = sub_800BFD24(&memory, args[0], args[1], args[2]);
            break;
        case 0x800C4500:
            result = sub_800C4500(&memory, args[0], args[1], args[2]);
            break;
        case 0x800C4550:
            result = sub_800C4550(&memory, args[0], args[1], args[2]);
            break;
        case 0x800958F0:
            result = sub_800958F0(&memory, args[0], args[1]);
            break;
        case 0x800BFC5C:
            sub_800BFC5C(&memory, args[0], args[1], args[2], args[3]);
            result = 0;
            break;
        case 0x800C4454:
            sub_800C4454(&memory, args[0], args[1], args[2]);
            result = 0;
            break;
        case 0x800CB84C:
            result = sub_800CB84C(&memory, args[0]);
            break;
        case 0x800CC0B0:
            result = sub_800CC0B0(&memory, args[0]);
            break;
        case 0x8008C000:
            result = sub_8008C000(&memory, args[0], args[1]);
            break;
        case 0x8008CC94:
            result = sub_8008CC94(&memory);
            break;
        case 0x80043F00:
            result = sub_80043F00(&memory, args[0]);
            break;
        case 0x8002705C:
            result = sub_8002705C(&memory, args[0], args[1]);
            break;
        case 0x80027178:
            result = sub_80027178(&memory);
            break;
        case 0x800271CC:
            result = sub_800271CC(&memory, args[0], args[1]);
            break;
        case 0x800273EC:
            result = sub_800273EC(&memory, args[0], args[1], args[2], args[3], args[4]);
            break;
        case 0x80012884:
            result = sub_80012884(&memory, args[0], args[1]);
            break;
        case 0x8001298C:
            result = sub_8001298C(&memory, args[0], args[1]);
            break;
        case 0x800C2F84:
            sub_800C2F84(&memory, args[0], args[1], args[2], args[3]);
            result = 0;
            break;
        case 0x800C3E9C:
            result = sub_800C3E9C(&memory, args[0], args[1], args[2]);
            break;
        case 0x800C37B0:
            result = sub_800C37B0(&memory, args[0], args[1]);
            break;
        case 0x8005C018:
            if (argument_count != 8)
                return 2;
            result = sub_8005C018(&memory, args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
            break;
        case 0x8005C140:
            if (argument_count != 8)
                return 2;
            result = sub_8005C140(&memory, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]);
            break;
        case 0x8005BF6C:
            result = sub_8005BF6C(&memory, args[0], args[1], args[2], args[3], args[4]);
            break;
        case 0x8005C0B0:
            result = sub_8005C0B0(&memory, args[0], args[1], args[2], args[3], args[4]);
            break;
        case 0x8005C8F4:
            result = sub_8005C8F4(&memory, args[0]);
            break;
        case 0x8005BD74:
            result = sub_8005BD74(&memory, args[0]);
            break;
        case 0x8005CB70:
            result = sub_8005CB70(&memory, args[0], args[1], args[2]);
            break;
        case 0x800714FC:
            result = sub_800714FC(&memory, args[0], args[1], args[2]);
            break;
        case 0x8005E2BC:
            result = sub_8005E2BC(&memory, args[0], args[1]);
            break;
        case 0x8005C39C:
            result = sub_8005C39C(&memory, args[0], args[1]);
            break;
        case 0x8005BE58:
            result = sub_8005BE58(&memory, args[0]);
            break;
        case 0x8005C418:
            result = sub_8005C418(&memory, args[0]);
            break;
        case 0x8005BEF4:
            result = sub_8005BEF4(&memory, args[0]);
            break;
        case 0x8005BE0C:
            result = sub_8005BE0C(&memory, args[0]);
            break;
        case 0x80096564:
            result = sub_80096564(&memory, args[0]);
            break;
        case 0x8003662C:
            if (args[5])
            {
                uint16_t direction[3];
                uint32_t record[8], output[3];
                for (i = 0; i < 3; ++i)
                    direction[i] = rrj_u16(rrj_at(&memory, args[0] + 2 * i, 2));
                for (i = 0; i < 8; ++i)
                    record[i] = rrj_read32(&memory, args[1] + 4 * i);
                for (i = 0; i < 3; ++i)
                    output[i] = rrj_read32(&memory, args[2] + 4 * i);
                result = rrj_update_track_direction_local(&memory, direction, record, output);
                for (i = 0; i < 3; ++i)
                    rrj_write32(&memory, args[2] + 4 * i, output[i]);
            }
            else
                result = sub_8003662C(&memory, args[0], args[1], args[2]);
            break;
        case 0x80039AA0:
            result = sub_80039AA0(&memory, args[0]);
            break;
        case 0x800394F0:
            if (args[5])
            {
                uint32_t record[8];
                for (i = 0; i < 8; ++i)
                    record[i] = rrj_read32(&memory, args[0] + 4 * i);
                result = rrj_track_exit_local(&memory, record, args[1]);
            }
            else
                result = sub_800394F0(&memory, args[0], args[1]);
            break;
        case 0x8002E468:
            result = sub_8002E468(&memory, args[0]);
            break;
        case 0x8002E14C:
            result = sub_8002E14C(&memory, args[0]);
            break;
        case 0x8003B4B0:
            result = sub_8003B4B0(&memory, args[0], args[1]);
            break;
        case 0x8003B8F4:
            result = sub_8003B8F4(&memory, args[0]);
            break;
        case 0x80039AFC:
            result = sub_80039AFC(&memory, args[0]);
            break;
        case 0x8003A37C:
            result = sub_8003A37C(&memory, args[0], args[1]);
            break;
        case 0x8007EC30:
            result = sub_8007EC30(&memory, args[0]);
            break;
        case 0x80018440:
            result = sub_80018440(&memory, args[0], args[1], probe_reverb);
            break;
        case 0x80068D20:
            result = sub_80068D20(&memory, args[0], args[1], args[2]);
            break;
        case 0x80095AEC:
            result = sub_80095AEC(&memory, args[0]);
            break;
        case 0x80012838:
            result = sub_80012838(&memory, args[0], args[1], args[2], args[3]);
            break;
        case 0x80086AF8:
            result = sub_80086AF8(&memory, args[0]);
            break;
        case 0x800235B0:
            result = sub_800235B0(&memory, args[0], args[1]);
            break;
        case 0x8001E100:
            result = sub_8001E100(&memory, args[0], args[1], args[2]);
            break;
        case 0x800BCD10:
            result = sub_800BCD10(&memory, args[0]);
            break;
        case 0x80078C68:
            result = rrj_race_player_tail(&memory, args[0]);
            break;
        case 0x8008A998:
            if (args[5])
            {
                const uint32_t wide_args[8] = {args[0], args[1], 0, 0, 0, 0, 0, 0};
                result = trace_race_call(&memory, function, wide_args);
            }
            else
                result = sub_8008A998(&memory, args[0], args[1]);
            break;
        case 0x800BC7CC:
            result = sub_800BC7CC(&memory, args[0]);
            break;
        case 0x80092C7C:
            result = sub_80092C7C(&memory, args[0], args[1]);
            break;
        case 0x8002090C:
            result = sub_8002090C(&memory, args[0]);
            break;
        case 0x8002076C:
            result = sub_8002076C(&memory, args[0]);
            break;
        case 0x8002E698:
            result = sub_8002E698(&memory, args[0], args[1]);
            break;
        case 0x8002EE50:
            result = (uint64_t)sub_8002EE50(&memory, args[0], args[1], args[2]);
            break;
        case 0x8007F0BC:
            result = sub_8007F0BC(&memory, args[0], args[1]);
            break;
        case 0x80075EE0:
            result = sub_80075EE0(&memory, (int32_t)args[0], trace_race_call, probe_reverb);
            break;
        case 0x80078AA8:
            result = rrj_race_movement_block(&memory, args[0]);
            break; /* Frame block only. */
        case 0x80078AAC:
            result = rrj_race_movement_contact_block(&memory, args[0]);
            break; /* Extended frame block. */
        case 0x80078AB0:
            result = rrj_race_movement_route_block(&memory, args[0]);
            break; /* Movement/contact/route block. */
        case 0x80078AB4:
            result = rrj_race_movement_surface_block(&memory, args[0]);
            break; /* Through surface projection traversal. */
        case 0x80078AB8:
            result = rrj_race_movement_placement_block(&memory, args[0]);
            break; /* Through contact placement traversal. */
        case 0x80078ABC:
            result = rrj_race_movement_wheel_block(&memory, args[0]);
            break; /* Through all active objects' wheel/geometry update. */
        case 0x80078C58:
            result = rrj_race_activity_block(&memory, probe_reverb);
            break; /* Actual consecutive actor/body activity passes. */
        case 0x80078AC0:
            result = args[5] ? rrj_race_movement_player_block(&memory, args[0], probe_reverb) : rrj_race_movement_activity_block(&memory, args[0], probe_reverb);
            break;
        case 0x8003E150:
            result = sub_8003E150(&memory);
            break;
        case 0x8003AE24:
            result = sub_8003AE24(&memory);
            break;
        case 0x8007504C:
            result = sub_8007504C(&memory, args[0], args[1]);
            break;
        case 0x80075B08:
            result = sub_80075B08(&memory, args[0], args[1]);
            break;
        case 0x80075628:
            result = sub_80075628(&memory, args[0], args[1], args[2]);
            break;
        case 0x8007FA4C:
            result = sub_8007FA4C(&memory, args[0]);
            break;
        case 0x800807F0:
            result = sub_800807F0(&memory, args[0], args[1]);
            break;
        case 0x800BFE58:
            result = sub_800BFE58(&memory, args[0], args[1], args[2]);
            break;
        case 0x800BF51C:
            result = sub_800BF51C(&memory, args[0]);
            break;
        case 0x8002F0F4:
            result = sub_8002F0F4(&memory, args[0]);
            break;
        case 0x8002E548:
            result = sub_8002E548(&memory, args[0]);
            break;
        case 0x8002ED94:
            result = sub_8002ED94(&memory, args[0], args[1], args[2]);
            break;
        case 0x8007F08C:
            result = sub_8007F08C(&memory, args[0]);
            break;
        case 0x8004CF74:
            result = sub_8004CF74(&memory, args[0]);
            break;
        case 0x8002ECB8:
            result = (uint64_t)sub_8002ECB8(&memory, args[0], args[1], args[2], args[3], args[4]);
            break;
        case 0x800A7BF8:
            if (args[5])
            {
                uint32_t output[3];
                uint16_t normal[3];
                for (i = 0; i < 3; ++i)
                {
                    output[i] = rrj_read32(&memory, args[2] + 4 * i);
                    normal[i] = rrj_u16(rrj_at(&memory, args[3] + 2 * i, 2));
                }
                result = rrj_surface_hit_local(&memory, args[0], args[1], output, normal, args[4]);
                for (i = 0; i < 3; ++i)
                    rrj_put16(rrj_at(&memory, args[3] + 2 * i, 2), normal[i]);
                for (i = 0; i < 3; ++i)
                    rrj_write32(&memory, args[2] + 4 * i, output[i]);
            }
            else
                result = sub_800A7BF8(&memory, args[0], args[1], args[2], args[3], args[4]);
            break;
        case 0x800A8498:
            result = sub_800A8498(&memory, args[0], args[1], args[2], args[3], args[4], args[5]);
            break;
        case 0x800B6E08:
            if (args[5])
            {
                uint32_t point[3], vertices[12];
                for (i = 0; i < 3; ++i)
                    point[i] = rrj_read32(&memory, args[0] + 4 * i);
                for (i = 0; i < 12; ++i)
                    vertices[i] = rrj_read32(&memory, args[1] + 4 * i);
                result = rrj_polygon_contains_local(&memory, point, vertices, args[2], args[3]);
            }
            else
                result = sub_800B6E08(&memory, args[0], args[1], args[2], args[3]);
            break;
        case 0x800B6844:
            if (args[5])
            {
                uint32_t vertices[12];
                uint16_t normal[3];
                for (i = 0; i < 12; ++i)
                    vertices[i] = rrj_read32(&memory, args[0] + 4 * i);
                result = rrj_polygon_normal_local(&memory, vertices, args[1], normal);
                for (i = 0; i < 3; ++i)
                    rrj_put16(rrj_at(&memory, args[2] + 2 * i, 2), normal[i]);
            }
            else
                result = sub_800B6844(&memory, args[0], args[1], args[2]);
            break;
        case 0x8003B520:
            result = sub_8003B520(&memory);
            break;
        case 0x80071D24:
            result = sub_80071D24(&memory, args[0]);
            break;
        case 0x800723FC:
            result = sub_800723FC(&memory, args[0]);
            break;
        case 0x8002E570:
            result = sub_8002E570(&memory, args[0], args[1], args[2], args[3]);
            break;
        case 0x8002EB78:
            result = sub_8002EB78(&memory, args[0], args[1], args[2], args[3], args[4]);
            break;
        case 0x80010028:
            result = sub_80010028(args[0], args[1]);
            break;
        case 0x8001FF3C:
            result = sub_8001FF3C(&memory, args[0]);
            break;
        case 0x80020018:
            result = sub_80020018(&memory, args[0], args[1]);
            break;
        case 0x8009DAF8:
            result = sub_8009DAF8(&memory, args[0]);
            break;
        case 0x8002820C:
            result = sub_8002820C(&memory, args[0]);
            break;
        case 0x8002A738:
            result = sub_8002A738(&memory, args[0], args[1], args[2], args[3]);
            break;
        case 0x8002847C:
            result = sub_8002847C(&memory, args[0]);
            break;
        case 0x80039F68:
            result = sub_80039F68(&memory, args[0]);
            break;
        case 0x80037524:
            result = sub_80037524(&memory, args[0], args[1]);
            break;
        case 0x8003BE1C:
            result = sub_8003BE1C(&memory, args[0], args[1], args[2], args[3]);
            break;
        case 0x8003A700:
            result = sub_8003A700(&memory, args[0], args[1], args[2]);
            break;
        case 0x80039C90:
            result = sub_80039C90(&memory, args[0], args[1]);
            break;
        case 0x80039CFC:
            result = sub_80039CFC(&memory, args[0], args[1]);
            break;
        case 0x80039DFC:
            result = sub_80039DFC(&memory, args[0], args[1]);
            break;
        case 0x8003F3B4:
            result = sub_8003F3B4(&memory, args[0]);
            break;
        case 0x80039B60:
            result = sub_80039B60(&memory, args[0]);
            break;
        case 0x800374D4:
            result = sub_800374D4(&memory, args[0], args[1]);
            break;
        case 0x8004D2A4:
            if (args[5])
            {
                uint16_t angles[3], matrix[9];
                unsigned mi;
                for (mi = 0; mi < 3; ++mi)
                    angles[mi] = rrj_u16(rrj_at(&memory, args[0] + 2 * mi, 2));
                rrj_euler_rotation_local(&memory, angles, matrix);
                for (mi = 0; mi < 9; ++mi)
                    rrj_put16(rrj_at(&memory, args[1] + 2 * mi, 2), matrix[mi]);
                result = args[1];
            }
            else
                result = sub_8004D2A4(&memory, args[0], args[1]);
            break;
        case 0x8003FA40:
            if (args[5] == 2)
            {
                uint16_t matrix[9];
                unsigned mi;
                for (mi = 0; mi < 9; ++mi)
                    matrix[mi] = rrj_u16(rrj_at(&memory, args[0] + 2 * mi, 2));
                result = rrj_multiply_rotation_local_left(&memory, matrix, args[1], args[2]);
            }
            else if (args[5])
            {
                uint16_t matrix[9];
                unsigned mi;
                for (mi = 0; mi < 9; ++mi)
                    matrix[mi] = rrj_u16(rrj_at(&memory, args[1] + 2 * mi, 2));
                result = rrj_multiply_rotation_local(&memory, args[0], matrix, args[2]);
            }
            else
                result = sub_8003FA40(&memory, args[0], args[1], args[2]);
            break;
        case 0x8003FB34:
            if (args[5])
            {
                uint16_t matrix[9];
                unsigned mi;
                result = rrj_axis_rotation_local(&memory, args[0], args[1], matrix);
                for (mi = 0; mi < 9; ++mi)
                    rrj_put16(rrj_at(&memory, args[2] + 2 * mi, 2), matrix[mi]);
            }
            else
                result = sub_8003FB34(&memory, args[0], args[1], args[2]);
            break;
        case 0x80028034:
            result = sub_80028034(&memory, args[0]);
            break;
        case 0x8008BA18:
            result = sub_8008BA18(&memory, args[0]);
            break;
        case 0x8002E810:
            result = (uint64_t)sub_8002E810(&memory, args[0], args[1], args[2]);
            break;
        case 0x8009432C:
            result = sub_8009432C(&memory, args[0]);
            break;
        case 0x80037104:
            result = sub_80037104(&memory, args[0]);
            break;
        case 0x80037450:
            result = sub_80037450(&memory, args[0]);
            break;
        case 0x800396A8:
            if (args[5])
                sub_800396A8(&memory, args[0]);
            else
                rrj_correct_actor_track_with_prior(&memory, args[0], args[1]);
            result = 0;
            break;
        case 0x8003BFE8:
            rrj_correct_road_with_prior(&memory, args[0], args[1]);
            result = 0;
            break;
        case 0x8001FCB0:
            result = sub_8001FCB0(args[0], args[1], args[2]);
            break;
        case 0x8002EAD8:
            if (args[5])
            {
                uint32_t local[3];
                result = rrj_project_vector_local(&memory, args[0], args[1], args[2], local);
                for (i = 0; i < 3; ++i)
                    rrj_write32(&memory, args[3] + 4 * i, local[i]);
            }
            else
                result = sub_8002EAD8(&memory, args[0], args[1], args[2], args[3]);
            break;
        case 0x80099D48:
            result = sub_80099D48(&memory, args[0]);
            break;
        case 0x800950E8:
            result = args[5] ? sub_800950E8(&memory, probe_reverb) : rrj_probe_activity_list(&memory, probe_pad_sdk);
            break;
        case 0x8003CEC4:
            result = args[5] ? sub_8003CEC4(&memory, args[0], args[1], args[2], probe_reverb) : rrj_probe_segment_load(&memory, args[0], args[1], args[2], probe_pad_sdk);
            break;
        case 0x80023960:
            result = sub_80023960(&memory, args[0], args[1]);
            break;
        case 0x800136BC:
            result = sub_800136BC(&memory, args[0], args[1], args[2]);
            break;
        case 0x8003CA50:
            result = sub_8003CA50(&memory, args[0], args[1], args[2]);
            break;
        case 0x80031E1C:
            result = sub_80031E1C(&memory, args[0], args[1], args[2]);
            break;
        case 0x80033F14:
            result = sub_80033F14(&memory, args[0], args[1], args[2]);
            break;
        case 0x8002227C:
            result = sub_8002227C(&memory, args[0], args[1], args[2], args[3], args[4], record_image);
            break;
        case 0x80031008:
            result = sub_80031008(&memory, args[0], probe_dispatch_critical);
            break;
        case 0x800333F4:
            result = sub_800333F4(&memory, args[0], args[1], probe_slot_service);
            break;
        case 0x80033B50:
            result = sub_80033B50(&memory, args[0], args[1], args[2], args[3], probe_slot_service);
            break;
        case 0x80022218:
            result = sub_80022218(&memory, args[0], args[1]);
            break;
        case 0x80032C6C:
            slot_find_count = 0;
            result = sub_80032C6C(&memory, args, probe_slot_service);
            break;
        case 0x80032CC8:
            slot_find_count = 0;
            result = sub_80032CC8(&memory, args, probe_slot_service);
            break;
        case 0x80031604:
            memcpy(object_dispatch_args, args, sizeof(object_dispatch_args));
            object_dispatch_composed = args[5];
            slot_find_count = 0;
            result = sub_80031604(&memory, args[0], args[1], probe_object_dispatch, probe_dispatch_critical);
            break;
        case 0x80032190:
            result = sub_80032190(&memory, args[0], args[1]);
            break;
        case 0x800322D0:
            cleanup_mode = args[2];
            cleanup_reply = args[3];
            result = sub_800322D0(&memory, args[0], args[1], probe_object_cleanup);
            break;
        case 0x800313EC:
            cleanup_mode = args[2];
            cleanup_reply = args[3];
            result = sub_800313EC(&memory, args[0], probe_object_cleanup);
            break;
        case 0x80031B98:
            result = sub_80031B98(&memory, args[0], args[1]);
            break;
        case 0x80031B4C:
            result = sub_80031B4C(&memory, args[0]);
            break;
        case 0x800319F8:
            result = sub_800319F8(&memory, args[0], args[1]);
            break;
        case 0x80031D70:
            memcpy(queue_args, args, sizeof(queue_args));
            queue_composed = 0;
            result = sub_80031D70(&memory, probe_object_queue);
            break;
        case 0x80031BF8:
            result = sub_80031BF8(&memory, args[0], args[1]);
            break;
        case 0x800320CC:
            result = sub_800320CC(&memory, args[0]);
            break;
        case 0x800320AC:
            result = sub_800320AC(&memory);
            break;
        case 0x80030608:
            memcpy(queue_args, args, sizeof(queue_args));
            queue_index = 0;
            queue_composed = args[5];
            result = sub_80030608(&memory, probe_object_queue);
            break;
        case 0x8002379C:
            result = sub_8002379C(&memory, args[0]);
            break;
        case 0x80023860:
            result = sub_80023860(args[0]);
            break;
        case 0x800237B8:
            memcpy(player_update_args, args, sizeof(player_update_args));
            result = sub_800237B8(&memory, probe_player_update);
            break;
        case 0x80023D7C:
            memcpy(player_update_args, args, sizeof(player_update_args));
            result = sub_80023D7C(&memory, probe_player_update);
            break;
        case 0x80023C4C:
            memcpy(player_update_args, args, sizeof(player_update_args));
            result = sub_80023C4C(&memory, probe_player_update);
            break;
        case 0x800245DC:
            result = sub_800245DC(&memory, args[0]);
            break;
        case 0x800245F4:
            result = sub_800245F4(&memory, args[0]);
            break;
        case 0x80030410:
            result = sub_80030410(&memory, args[0], args[1]);
            break;
        case 0x8008B99C:
            result = sub_8008B99C(&memory, args[0]);
            break;
        case 0x80030500:
            result = sub_80030500(&memory, args[0], args[1]);
            break;
        case 0x80022C64:
            result = sub_80022C64(&memory);
            break;
        case 0x80023870:
            memcpy(race_check_replies, args, sizeof(race_check_replies));
            race_check_index = 0;
            result = sub_80023870(&memory, args[5] ? sub_8008B99C : probe_race_player_check);
            break;
        case 0x80011C4C:
            memcpy(race_frame_args, args, sizeof(race_frame_args));
            result = sub_80011C4C(&memory, probe_race_frame);
            break;
        case 0x8008CD88:
        case 0x8008AC80:
        case 0x8008ACE8:
            memcpy(race_service_args, args + 1, sizeof(race_service_args));
            result = function == 0x8008CD88 ? sub_G_8008CD88(&memory, args[0], probe_race_service) : function == 0x8008AC80 ? sub_G_8008AC80(&memory, args[0], probe_race_service) : sub_G_8008ACE8(&memory, args[0], probe_race_service);
            break;
        case 0x8008AB00:
            race_global_replies[0] = args[1];
            race_global_replies[1] = args[2];
            race_global_composed = args[4];
            result = sub_G_8008AB00(&memory, args[0], probe_race_global);
            break;
        case 0x8008AD38:
            result = sub_G_8008AD38(&memory, args[0], probe_race_global);
            break;
        case 0x8008AD40:
            result = sub_G_8008AD40(&memory, args[0], args[1], args[2], probe_race_global);
            break;
        case 0x8006883C:
            submenu_reply = args[1];
            result = sub_F_8006883C(&memory, args[0], probe_submenu);
            break;
        case 0x8002D250:
            result = sub_8002D250(&memory);
            break;
        case 0x8006AE6C:
            submenu_reply = args[1];
            result = sub_F_8006AE6C(&memory, args[0], probe_submenu);
            break;
        case 0x8007EDE0:
        case 0x8007EEB8:
        case 0x8007EF64:
            music_replies[0] = args[1];
            music_replies[1] = args[2];
            music_voice_index = 0;
            result = function == 0x8007EDE0 ? sub_F_8007EDE0(&memory, args[0], probe_music_start) : function == 0x8007EEB8 ? sub_F_8007EEB8(&memory, args[0], probe_music_start) : sub_F_8007EF64(&memory, probe_music_start);
            break;
        case 0x8006FEF4:
        case 0x80070018:
            video_phase_replies[0] = args[1];
            video_phase_replies[1] = args[2];
            video_phase_replies[2] = args[3];
            result = function == 0x8006FEF4 ? sub_F_8006FEF4(&memory, args[0], probe_video_phase, probe_video_open, probe_preview_stop) : sub_F_80070018(&memory, args[0], probe_video_phase, probe_video_open, probe_preview_stop);
            break;
        case 0x8006DE5C:
            video_phase_replies[2] = args[3];
            result = sub_F_8006DE5C(&memory, args[0], probe_video_phase);
            break;
        case 0x8006DB5C:
            video_phase_replies[0] = args[1];
            video_phase_replies[1] = args[2];
            video_phase_replies[2] = args[3];
            result = sub_F_8006DB5C(&memory, args[0], probe_video_phase, probe_video_open);
            break;
        case 0x8004CC44:
            vblank_replies[1] = args[5];
            result = sub_8004CC44(&memory, args[0], args[1], args[2], args[3], args[4], probe_vblank);
            break;
        case 0x8004CD04:
            result = sub_8004CD04(&memory, args[0], args[1], args[2], args[3], args[4]);
            break;
        case 0x8001BE08:
            screen_probe_mode = args[4];
            screen_probe_env_index = 0;
            result = sub_8001BE08(&memory, args[0], args[1], args[2], args[3], probe_vblank, probe_screen_env);
            screen_probe_mode = 0;
            break;
        case 0x800C2178:
            result = sub_800C2178(&memory, args[0], args[1]);
            break;
        case 0x8008B428:
            result = sub_8008B428(&memory, args[0], args[1]);
            break;
        case 0x8001BF1C:
            result = sub_8001BF1C(&memory, args[0], args[1], args[2], args[3], probe_vblank);
            break;
        case 0x80080D08:
            vblank_replies[1] = args[3];
            result = sub_F_80080D08(&memory, probe_vblank);
            break;
        case 0x8006FE6C:
            result = sub_F_8006FE6C(&memory, record_sdk);
            break;
        case 0x8006FED4:
            result = sub_F_8006FED4(&memory, record_sdk);
            break;
        case 0x8006D5B0:
            result = sub_F_8006D5B0(&memory, args[0], record_sdk);
            break;
        case 0x8006A8FC:
            result = sub_F_8006A8FC(&memory, args[0], probe_attract_sound);
            break;
        case 0x8001FB58:
            result = sub_8001FB58(&memory, args[0]);
            break;
        case 0x8001F7EC:
            result = sub_8001F7EC(&memory, args[0], probe_reverb);
            break;
        case 0x8001F5D4:
            result = sub_8001F5D4(&memory, args[0], args[1], probe_voice_setup, probe_reverb);
            break;
        case 0x8007F158:
            result = sub_F_8007F158(&memory, args[0], probe_voice_setup, probe_reverb);
            break;
        case 0x8001EB7C:
            result = sub_8001EB7C(&memory, args[0], args[1], args[2], args[3], probe_voice_setup);
            break;
        case 0x80080274:
            probe_loop_remaining = args[2];
            result = sub_F_80080274(&memory, probe_loop);
            break;
        case 0x80080488:
            vblank_replies[0] = args[2];
            vblank_replies[1] = args[3];
            result = sub_F_80080488(&memory, probe_vblank);
            break;
        case 0x8001EE94:
            result = sub_8001EE94(&memory, probe_voice_setup, memory.sdk_call);
            break;
        case 0x800600F8:
            result = sub_F_800600F8(&memory);
            break;
        case 0x8001B700:
            vblank_replies[0] = args[2];
            vblank_replies[1] = args[3];
            result = sub_8001B700(&memory, probe_vblank);
            break;
        case 0x80064C30:
            vblank_replies[0] = args[2];
            vblank_replies[1] = args[3];
            result = sub_F_80064C30(&memory, probe_vblank);
            break;
        case 0x8001E0B4:
            result = sub_8001E0B4(&memory, args[0], args[1], args[2]);
            break;
        case 0x8001CB3C:
            if (args[5])
                result = rrj_input_audit_probe(&memory, args);
            else
            {
                sub_8001CB3C_menu(&memory, memory.sdk_call);
                result = 0;
            }
            break;
        case 0x8001DDC4:
            pad_sdk_returns[0] = args[2];
            pad_sdk_returns[1] = args[3];
            result = sub_8001DDC4(&memory, args[0], args[1], probe_pad_sdk);
            break;
        case 0x8001C4A8:
            result = sub_8001C4A8(&memory, args[0], args[1]);
            break;
        case 0x8001C5F8:
            if (args[5])
            {
                pad_sdk_returns[0] = args[2];
                pad_sdk_returns[1] = args[3];
                result = rrj_pad_poll(&memory, probe_pad_sdk);
            }
            else
                result = sub_8001C5F8(&memory, memory.sdk_call);
            break;
        case 0x80080A70:
            result = sub_F_80080A70(&memory, args[0]);
            break;
        case 0x80066EF8:
            result = sub_F_80066EF8(&memory, memory.sdk_call);
            break;
        case 0x8006711C:
            result = sub_F_8006711C(&memory, memory.sdk_call);
            break;
        case 0x80066C34:
            sub_F_80066C34(&memory, native_menu_frame);
            result = 0;
            break;
        case 0x800667E4:
            result = sub_F_800667E4(&memory, native_menu_update, memory.sdk_call);
            break;
        case 0x8006738C:
            sub_F_8006738C(&memory, args[0], memory.sdk_call);
            result = 0;
            break;
        case 0x80080ACC:
            result = sub_F_80080ACC(&memory, args[0]);
            break;
        case 0x8006310C:
            sub_F_8006310C(&memory, args[0], memory.sdk_call);
            result = 0;
            break;
        case 0x80062D6C:
            result = sub_F_80062D6C(&memory, args[0]);
            break;
        case 0x800630C0:
            result = sub_F_800630C0(&memory, args[0], args[1]);
            break;
        case 0x80064254:
            result = sub_F_80064254(&memory, args[0]);
            break;
        case 0x80068448:
            result = sub_F_80068448(&memory);
            break;
        case 0x800649BC:
            result = sub_F_800649BC(&memory, args[0]);
            break;
        case 0x800680E8:
            result = sub_F_800680E8(&memory, args[0], args[1]);
            break;
        case 0x8006E4D8:
            result = sub_F_8006E4D8(&memory, args[0], args[1], probe_video);
            break;
        case 0x8006E6F4:
            result = sub_F_8006E6F4(&memory, args[0], args[1], probe_video);
            break;
        case 0x8006FAC8:
            result = sub_F_8006FAC8(&memory, args[0], args[1], NULL, NULL);
            break;
        case 0x800662CC:
            result = sub_F_800662CC(&memory, args[0], args[1], args[2], args[3], args[4], args[5]);
            break;
        case 0x80066318:
            result = sub_F_80066318(&memory, args[0], args[1], args[2], args[3], args[4], args[5]);
            break;
        case 0x80063C7C:
            result = sub_F_80063C7C(&memory, args[0], NULL);
            break;
        case 0x8006E8FC:
            result = sub_F_8006E8FC(&memory, args[0], args[1], item_resource, rrj_select_menu_image, NULL);
            break;
        case 0x80064154:
            result = sub_F_80064154(&memory, args[0]);
            break;
        case 0x800641D4:
            result = sub_F_800641D4(&memory);
            break;
        case 0x80072100:
            result = sub_F_80072100(&memory, args[0]);
            break;
        case 0x8002CB08:
            result = sub_8002CB08(&memory, args[0], args[1], args[2], args[3], args[4], args[5], rrj_blink_text);
            break;
        case 0x8006E7A4:
            result = sub_F_8006E7A4(&memory, args[0], args[1], item_resource, rrj_blink_text);
            break;
        case 0x8006F764:
            result = sub_F_8006F764(&memory, args[0], args[1], item_resource);
            break;
        case 0x8002D1F8:
            result = sub_8002D1F8(&memory, args[0], args[1], args[2], args[3]);
            break;
        case 0x8002D1A0:
            result = sub_8002D1A0(&memory, args[0], args[1], args[2]);
            break;
        case 0x8002D0D8:
            result = sub_8002D0D8(&memory, args[0], args[1]);
            break;
        case 0x8002CDC8:
            result = sub_8002CDC8(&memory, args[0], args[1], args[2], args[3], args[4], args[5]);
            break;
        case 0x80065768:
            result = sub_F_80065768(&memory, args[0], memory.sdk_user ? record_image : NULL);
            break;
        case 0x8007A400:
            result = sub_F_8007A400(&memory, args[0], args[1], memory.sdk_user ? record_image : NULL);
            break;
        case 0x8007A6C0:
            result = sub_F_8007A6C0(args[0], args[1]);
            break;
        case 0x800700F0:
            result = sub_F_800700F0(&memory, args[0], args[1], args[2], NULL);
            break;
        case 0x8006E894:
            result = sub_F_8006E894(&memory, args[0], args[1], NULL);
            break;
        /* Explicit GAME renderer interception in these dispatcher-only fixtures. */
        case 0x8006D3E0:
            result = sub_F_8006D3E0(&memory, args[0], record_sdk);
            break;
        case 0x8006D630:
            result = sub_F_8006D630(&memory, args[0], args[5] ? native_menu_draw : record_sdk);
            break;
        case 0x8006C700:
            result = sub_F_8006C700(&memory, args[0], args[1]);
            break;
        case 0x8006B03C:
            result = sub_F_8006B03C(&memory, args[0], args[1], args[2], NULL);
            break;
        case 0x80069418:
            result = sub_F_80069418(&memory, args[0], NULL);
            break;
        case 0x8001EB28:
            result = sub_8001EB28(&memory, args[0]);
            break;
        case 0x8001EB44:
            result = sub_8001EB44(&memory, args[0]);
            break;
        case 0x8001E86C:
            result = sub_8001E86C(&memory, args[0], args[1]);
            break;
        case 0x8001F9C4:
            result = sub_8001F9C4(&memory, args[0]);
            break;
        case 0x8001F174:
            result = sub_8001F174(&memory, args[0], args[1], args[2], args[3], ram_pointer(args[4], 12));
            break;
        case 0x8007EAC0:
            result = sub_F_8007EAC0(&memory, args[0]);
            break;
        /* Sixth fixture word selects full translated sound for integration probes;
     * it is not an argument of either original navigation function. */
        case 0x8006C3A4:
            result = sub_F_8006C3A4(&memory, args[0], args[5] ? sub_F_8007EAC0 : probe_navigation_sound);
            break;
        case 0x8006C558:
            result = sub_F_8006C558(&memory, args[0], args[5] ? sub_F_8007EAC0 : probe_navigation_sound);
            break;
        case 0x80064B30:
            result = sub_F_80064B30(&memory, args[0], args[1]);
            break;
        case 0x800685BC:
            sub_F_800685BC(&memory, args[0]);
            result = 0;
            break;
        case 0x8001C428:
            result = sub_8001C428(&memory);
            break;
        case 0x8001C3F4:
            result = sub_8001C3F4(&memory);
            break;
        case 0x800803FC:
            sub_F_800803FC(&memory);
            result = 0;
            break;
        case 0x8001FC90:
            result = (uint64_t)sub_8001FC90(rrj_s32(args[0]), rrj_s32(args[1]));
            break;
        case 0x8002E874:
            result = (uint64_t)sub_8002E874(ram_pointer(args[0], 12), ram_pointer(args[1], 12), ram_pointer(args[2], 12));
            break;
        case 0x8002E928:
            result = (uint64_t)sub_8002E928(ram_pointer(args[0], 12), ram_pointer(args[1], 36), ram_pointer(args[2], 12));
            break;
        case 0x80021BE8:
            result = sub_80021BE8(&memory);
            break;
        case 0x80021C98:
            result = sub_80021C98(&memory, args[0], args[1]);
            break;
        case 0x8002AF9C:
            result = sub_8002AF9C(&memory, ram_pointer(args[0], 16), ram_pointer(args[1], 4), ram_pointer(args[2], 4));
            break;
        case 0x8002B080:
            result = sub_8002B080(&memory, ram_pointer(args[0], 16), ram_pointer(args[1], 4), ram_pointer(args[2], 4));
            break;
        case 0x8002B164:
            result = sub_8002B164(&memory, ram_pointer(args[0], 16), ram_pointer(args[1], 4), ram_pointer(args[2], 4));
            break;
        case 0x8002B258:
            result = sub_8002B258(&memory, ram_pointer(args[0], 16), ram_pointer(args[1], 4), args[2], ram_pointer(args[3], 4));
            break;
        case 0x8002B3FC:
            result = sub_8002B3FC(&memory, ram_pointer(args[0], 4), ram_pointer(args[1], 4), args[2], args[3], (uint8_t)args[4], ram_pointer(args[5], 4));
            break;
        case 0x8002B4A0:
            result = sub_8002B4A0(&memory, ram_pointer(args[0], 4), ram_pointer(args[1], 4), args[2], args[3], ram_pointer(args[4], 4));
            break;
        case 0x8002B5B4:
            result = sub_8002B5B4(&memory, ram_pointer(args[0], 4), ram_pointer(args[1], 4), args[2], args[3], ram_pointer(args[4], 4));
            break;
        case 0x8002B68C:
            result = sub_8002B68C(&memory, ram_pointer(args[0], 4), ram_pointer(args[1], 4), args[2], args[3], ram_pointer(args[4], 4));
            break;
        case 0x8002B878:
            result = sub_8002B878(&memory, ram_pointer(args[0], 4), ram_pointer(args[1], 4), ram_pointer(args[2], 4));
            break;
        default:
        {
            uint32_t coverage_result;
            if (!rrj_coverage_probe(&memory, function, args, &coverage_result))
            {
                fprintf(stderr, "Function %08X is not translated.\n", function);
                return 2;
            }
            result = coverage_result;
            break;
        }
    }
    if (memory.sdk_user && fclose((FILE *)memory.sdk_user) != 0)
        return 2;
    rrj_put32(result_bytes, (uint32_t)result);
    rrj_put32(result_bytes + 4, (uint32_t)(result >> 32));
    if (argc == 10 && !write_file(argv[9], SCRATCHPAD, sizeof(SCRATCHPAD)))
        return 2;
    return write_file(argv[5], result_bytes, 8) && write_file(argv[6], DRAM, sizeof(DRAM)) ? 0 : 2;
}

// XPORT REVISION: 2026-09-29T20:00:00Z
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
        rrj_memory_bind(&memory);
        memory.sdk_call = live_sdk;
        rrj_video_bind(live_video);
        xport_set_exit_code(rrj_lockstep_run(&memory, live_loop, live_service));
        return;
    }
#endif
    xport_set_exit_code(rrj_main(xport_arg_count(), xport_arg_values()));
}
