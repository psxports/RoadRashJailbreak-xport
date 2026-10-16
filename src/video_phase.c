#include "native_game_api.h"
#include "video_preview.h"
#include "video_stop.h"
#include <stdio.h>
#include "menu.h"
#include "screen.h"
#include "race_leaf.h"
#include "psx.h"
/* F8006DB5C, full phase branches. CD/codec/display effects use typed boundaries. */
#include "video_phase.h"
#include "video_tick.h"
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

static void close_video(void)
{
    uint32_t scalar_v_1;

    uint32_t flags;
    (void)sub_8005F7E0(((scalar_v_1 = ((uint32_t)r_u16(0x8009C688))), (((uint32_t)(scalar_v_1 & 0x8000 ? scalar_v_1 | 0xffff0000 : scalar_v_1)))));
    (void)sub_8001460C(r_u32(0x8009C684));
    flags = (uint32_t)r_u16(0x8009C5DA);
    w_u32(0x8009C684, 0xffffffff);
    w_u16(0x8009C5DA, (uint16_t)(flags & 0xfffe));
    screen(0);
}

uint32_t sub_8006DB5C(uint32_t menu)
{
    uint32_t scalar_v_2;
    uint32_t scalar_v_3;

    FUNCTION_MARKER(0x8006DB5Cu, "RASHCDF.BIN");
    char path[128];
    uint32_t item = r_u32(menu + 16), desc, phase, file, channel, x, y, mode, loop;
    if (!item)
        return 0;
    desc = item + 16;
    if (!((uint32_t)r_u16(menu) & 2))
    {
        if ((uint32_t)r_u16(0x8009C5DA) & 1)
            close_video();
        w_u16((menu + 2), (uint16_t)0);
        w_u8(0x8009C5E2, (uint8_t)1);
        return 0;
    }
    phase = (uint32_t)r_u16((menu + 2));
    if (phase != 32767)
    {
        w_u16((menu + 2), (uint16_t)(phase - 1));
        (void)(uint32_t)sub_8006DE5C(menu);
        return 0;
    }
    if ((uint32_t)sub_80022A78(0))
        return 0;
    if ((uint32_t)r_u16(0x8009C5DA) & 1)
        return 0;
    sprintf(path, (const char *)xport_guest_cptr(0x8005BF84u, 1), (const char *)xport_guest_cptr(r_u32(0x8008973Cu + 4u * r_u32(desc)), 1));
    file = sub_8001458C(path, 0u);
    w_u32(0x8009C684, file);
    if (((int32_t)(uint32_t)(file)) >= 0)
    {
        screen(1);
        channel = (uint32_t)r_u16((desc + 8));
        w_u32(desc + 16, 0);
        file = r_u32(0x8009C684);
        w_u16(0x8009C688, (uint16_t)channel);
        x = ((scalar_v_2 = ((uint32_t)r_u16((desc + 12)))), (((uint32_t)(scalar_v_2 & 0x8000 ? scalar_v_2 | 0xffff0000 : scalar_v_2))));
        y = ((scalar_v_3 = ((uint32_t)r_u16((desc + 14)))), (((uint32_t)(scalar_v_3 & 0x8000 ? scalar_v_3 | 0xffff0000 : scalar_v_3))));
        loop = (uint32_t)r_u16((desc + 10));
        mode = r_u32(desc + 4);
        if (sub_8005F36C(file, mode, ((uint32_t)(((uint32_t)(channel)) & 0x8000 ? ((uint32_t)(channel)) | 0xffff0000 : ((uint32_t)(channel)))), x, y, 2048u, 64u, 1024u, loop))
        {
            w_u32(desc + 16, r_u32(desc + 16) + 1);
            w_u16(0x8009C5DA, (uint16_t)((uint32_t)r_u16(0x8009C5DA) | 1));
        }
        else
            close_video();
    }
    w_u8(0x8009C5E2, (uint8_t)2);
    w_u16((menu + 2), (uint16_t)3);
    return 0;
}
