#include "video_stop.h"
#include "music_stop.h"
#include "native_game_api.h"
#include "psx.h"
/* Main update hierarchy, actual zero-game-argument F800667E4 entry. */
#include "menu_update.h"
#include "menu.h"
#include "resource_select.h"
#include "resource_refresh.h"
#include "resource_apply.h"
#include "menu_video.h"
#include <stdlib.h>

uint32_t sub_800667E4(void)
{
    uint32_t scalar_v_14;
    uint32_t scalar_v_15;
    uint32_t scalar_v_16;

    uint32_t scalar_v_12;

    FUNCTION_MARKER(0x800667E4u, "RASHCDF.BIN");
    uint32_t id, menu, list, root, fn, done = 0, mode, enabled, request = 1;
    w_u8(0x8009C5E3, (uint8_t)0);
    if (r_u32(0x800D712C) || r_u32(0x800D71EC))
        w_u32(0x8005ACAC, 0);
    id = (uint32_t)r_u16(0x8009C5D0);
    w_u16(0x8009C5D8, (uint16_t)id);
    menu = r_u32(r_u32(0x8009C68C) + 4 * ((uint32_t)(((uint32_t)(id)) < 32768 ? ((uint32_t)(id)) : ((uint32_t)(id)) | 0xffff0000)));
    w_u32(0x8009C5C8, menu);
    switch (id)
    {
        case 0:
        case 1:
        case 2:
        case 6:
        case 11:
        case 12:
        case 15:
        case 16:
        case 19:
        case 20:
        case 22:
        case 58:
            if ((uint32_t)r_u8(0x8009C5E0))
            {
                (uint32_t)sub_8007F158(1);
                w_u8(0x8009C5E0, (uint8_t)0);
            }
            request = 0;
            break;
        case 3:
            request = 0;
            break;
        case 4:
        case 5:
        case 29:
            w_u8(0x800D80F3, (uint8_t)0);
            break;
        case 40:
            w_u8(0x800D80EB, (uint8_t)r_u8(0x8009C5F3));
            break;
        default:
            break;
    }
    if (request && (uint32_t)r_u8(0x8009C5DF) && !(uint32_t)r_u8(0x8009C5E0) && !((uint32_t)r_u16(0x8009C5DA) & 1))
    {
        (uint32_t)sub_8007EDE0((uint32_t)r_u8(0x8009C5DE));
        w_u8(0x8009C5E0, (uint8_t)1);
    }
    if ((uint32_t)r_u16(0x8009C5DA) & 4)
        (void)sub_8006FED4();
    menu = r_u32(0x8009C5C8);
    list = r_u32(menu + 16);
    w_u32(0x8009C654, list ? list + 120 * ((scalar_v_14 = ((uint32_t)r_u16((menu + 4)))), (((uint32_t)(scalar_v_14 < 32768 ? scalar_v_14 : scalar_v_14 | 0xffff0000)))) : 0);
    (void)sub_800680E8(r_u32(0x8009C5C8), r_u32(0x8009C654));
    root = r_u32(0x8009C664);
    if (root)
    {
        fn = r_u32(0x8009CC40 + 4 * ((scalar_v_12 = ((uint32_t)r_u8((root + 2)))), (((uint32_t)(scalar_v_12 < 128 ? scalar_v_12 : scalar_v_12 | 0xffffff00)))));
        if (fn)
        {
            done = rrj_menu_update_function(fn)(r_u32(0x8009C5C8));
        }
    }
    (void)sub_80064B30(r_u32(0x8009C5C8), r_u32(0x8009C654));
    while ((uint32_t)r_u16(0x8009C5D8) != 65535 && !done)
    {
        menu = r_u32(r_u32(0x8009C68C) + 4 * ((scalar_v_15 = ((uint32_t)r_u16(0x8009C5D8))), (((uint32_t)(scalar_v_15 < 32768 ? scalar_v_15 : scalar_v_15 | 0xffff0000)))));
        fn = r_u32(0x8009C8C0 + 4 * ((scalar_v_16 = ((uint32_t)r_u16((menu + 6)))), (((uint32_t)(scalar_v_16 < 32768 ? scalar_v_16 : scalar_v_16 | 0xffff0000)))));
        w_u32(0x8009C5C8, menu);
        if (fn)
        {
            done = rrj_menu_update_function(fn)(menu);
        }
        w_u16(0x8009C5D8, (uint16_t)r_u16((r_u32(0x8009C5C8) + 10)));
    }
    mode = r_u32(0x800D80D8);
    enabled = 0;
    if (mode == 32)
        enabled = r_u32(0x8005AE7C) & 1;
    if (mode == 1)
        enabled = r_u32(0x8005AE7C) & 2;
    if (mode == 8)
        enabled = r_u32(0x8005AE7C) & 4;
    if (enabled && ((uint32_t)r_u8(0x800D7182) || (uint32_t)r_u8(0x800D7192)))
    {
        uint32_t left = (uint32_t)r_u8(0x800D7182);
        w_u8(0x800D80E9, (uint8_t)0);
        w_u8(0x800D80DD, (uint8_t)((uint32_t)r_u8(0x800D80DD) | 24));
        (uint32_t)sub_8002D250();
        mode = (uint32_t)r_u8(0x800D80DC);
        w_u8(0x800D80DC, (uint8_t)(left ? (mode == 2 ? 0 : mode == 4 ? 2 : 4) : (mode == 2 ? 4 : mode == 4 ? 0 : 2)));
    }
    sub_8006738C(0x800D81D8);
    sub_800685BC(r_u32(0x8009C654));
    sub_8006310C(r_u32(0x8009C664));
    if (((int32_t)(uint32_t)(r_u32(0x8005ACAC))) < 1801)
        return 4;
    if ((uint32_t)r_u16(0x8009C5D0) == 4)
        w_u16(0x8009C5D2, (uint16_t)58);
    return 58;
}
