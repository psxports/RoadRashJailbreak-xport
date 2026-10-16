#include "video_preview.h"
#include "video_stop.h"
#include "native_game_api.h"
#include "race_leaf.h"
#include "menu.h"
#include "screen.h"
#include "psx.h"
/* F8006DE5C: original video frame/end bookkeeping, codec/display boundaries. */
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

uint32_t sub_8006DE5C(uint32_t menu)
{
    uint32_t scalar_v_2;
    uint32_t scalar_v_3;
    uint32_t scalar_v_4;
    uint32_t scalar_v_5;

    FUNCTION_MARKER(0x8006DE5Cu, "RASHCDF.BIN");
    uint32_t flags = (uint32_t)r_u16(0x8009C5DA), item, desc, channel, frame, x, y, mode, result = 1;
    if (!(flags & 1))
        return 1;
    item = r_u32(menu + 16);
    if (!item)
        return 1;
    desc = item + 16;
    if (flags & 4)
    {
        channel = ((scalar_v_2 = ((uint32_t)r_u16(0x8009C688))), (((uint32_t)(scalar_v_2 & 0x8000 ? scalar_v_2 | 0xffff0000 : scalar_v_2))));
        w_u16(0x8009C5DA, (uint16_t)(flags & 0xfff8));
        (void)sub_8005F7E0(channel);
        (void)(uint32_t)sub_8001460C(r_u32(0x8009C684));
        w_u32(0x8009C684, 0xffffffff);
        screen(0);
        return 0;
    }
    frame = r_u32(desc + 16);
    x = ((scalar_v_3 = ((uint32_t)r_u16((desc + 12)))), (((uint32_t)(scalar_v_3 & 0x8000 ? scalar_v_3 | 0xffff0000 : scalar_v_3))));
    y = ((scalar_v_4 = ((uint32_t)r_u16((desc + 14)))), (((uint32_t)(scalar_v_4 & 0x8000 ? scalar_v_4 | 0xffff0000 : scalar_v_4))));
    channel = ((scalar_v_5 = ((uint32_t)r_u16(0x8009C688))), (((uint32_t)(scalar_v_5 & 0x8000 ? scalar_v_5 | 0xffff0000 : scalar_v_5))));
    mode = r_u32(desc + 4);
    if (!sub_8005F484(x, y, channel, mode, frame))
    {
        close_video();
        w_u8(0x8009C5E2, (uint8_t)1);
        result = 0;
    }
    w_u32(desc + 16, r_u32(desc + 16) + 1);
    return result;
}
