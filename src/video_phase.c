#include "native_game_api.h"
#include "video_phase.h"
#include "rrj_fmv.h"
#include "video_stop.h"
#include "screen.h"
#include "menu.h"
#include "race_leaf.h"
#include "psx.h"
#include <stdlib.h>

static void screen(uint32_t video)
{
    (void)sub_80080D08();
    (void)sub_8001BE08(0, video ? 8 : 0, video ? 320 : 512, video ? 224 : 240);
    (void)sub_8001BF1C(1, 0, 0, 0);
    (void)sub_8001C3F4();
    (void)sub_8001C408();
    w_u8((r_u32(0x8005B470) + 125), (uint8_t)video);
    w_u8((r_u32(0x8005B470) + 237), (uint8_t)video);
}

/* Replace the standalone fullscreen codec loop with the shared player */
uint32_t sub_8006DB5C(uint32_t menu)
{
    uint32_t item, descriptor, phase;
    char path[128];
    sint32 result;
    XportFmvPresentation presentation = {0};
    FUNCTION_MARKER(0x8006DB5Cu, "RASHCDF.BIN");
    item = r_u32(menu + 16);
    if (!item)
        return 0;
    if (!(r_u16(menu) & 2))
    {
        sub_8006FE6C();
        w_u16(menu + 2, 0);
        w_u8(0x8009C5E2, 1);
        return 0;
    }
    phase = r_u16(menu + 2);
    if (phase != 0x7fff)
    {
        w_u16(menu + 2, (uint16_t)(phase - 1));
        return 0;
    }
    if (sub_80022A78(0) || (r_u16(0x8009C5DA) & 1))
        return 0;
    descriptor = item + 16;
    if (!rrj_fmv_path(descriptor, path, sizeof(path)) || !rrj_fmv_install())
        abort();
    screen(1);
    presentation.width = 320;
    presentation.height = 224;
    presentation.video_x = r_s16(descriptor + 12);
    presentation.video_y = r_s16(descriptor + 14);
    presentation.controllers = 1;
    presentation.skip_buttons = PADstart | PADRdown;
    result = xport_fmv_play_presented(path, 0, r_u16(descriptor + 10), &presentation);
    screen(0);
    w_u32(0x8009C684, 0xffffffff);
    w_u16(0x8009C5DA, (uint16_t)(r_u16(0x8009C5DA) & 0xfff8));
    w_u8(0x8009C5E2, 1);
    w_u16(menu + 2, 3);
    if (result < 0)
        abort();
    return 0;
}
