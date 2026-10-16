#include "race_leaf.h"
#include "psx.h"
/* Original background colour setup and clear-before-display-switch. */
#include "screen.h"
#include "menu.h"
#include <stdlib.h>

uint32_t sub_8001BF1C(uint32_t enabled, uint32_t red, uint32_t green, uint32_t blue)
{
    FUNCTION_MARKER(0x8001BF1Cu, "SLUS_010.53");
    uint32_t context, i;
    (void)(uint32_t)DrawSync(0);
    (void)(uint32_t)VSync(((int32_t)(uint32_t)(0)));
    context = r_u32(0x8005B470);
    if (context)
        for (i = 0; i < 2; ++i)
        {
            uint32_t address = context + 40 + 112 * i;
            w_u8(address, enabled);
            w_u8(address + 1, red);
            w_u8(address + 2, green);
            w_u8(address + 3, blue);
        }
    return 0;
}

uint32_t sub_80080D08(void)
{
    FUNCTION_MARKER(0x80080D08u, "RASHCDF.BIN");
    (void)ClearImage((PSX_RECT *)xport_guest_ptr(0x80088C7Cu, sizeof(PSX_RECT)), 0, 0, 0);
    (void)sub_8001C3F4();
    return sub_8001C408();
}

uint32_t sub_8001BE08(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    FUNCTION_MARKER(0x8001BE08u, "SLUS_010.53");
    uint32_t i, draw = 0x800D6CC8, attr = 0x800D6CE3, context, h, w;
    (void)(uint32_t)DrawSync(0);
    (void)(uint32_t)VSync(((int32_t)(uint32_t)(0)));
    w_u32(0x8005B470, 0x800D6CB8);
    w_u32(0x800D6CC0, width);
    w_u32(0x800D6CC4, height);
    for (i = 0; i < 2; ++i, draw += 112, attr += 112)
    {
        context = r_u32(0x8005B470);
        h = r_u32(context + 12);
        w = r_u32(context + 8);
        (void)SetDefDrawEnv((DRAWENV *)xport_guest_ptr(draw, sizeof(DRAWENV)), 0, 256 * i, ((int32_t)(uint32_t)(w)), ((int32_t)(uint32_t)(h)));
        context = r_u32(0x8005B470);
        h = r_u32(context + 12);
        w = r_u32(context + 8);
        (void)SetDefDispEnv((DISPENV *)xport_guest_ptr(draw + 92, sizeof(DISPENV)), 0, 256 * i, ((int32_t)(uint32_t)(w)), ((int32_t)(uint32_t)(h)));
        w_u8(attr - 5, 1);
        w_u8(attr - 4, 0);
        w_u16(attr + 79, (uint16_t)height);
        w_u16(attr + 75, (uint16_t)y);
        w_u8(attr - 3, 0);
        w_u8(attr - 2, 0);
        w_u8(attr - 1, 0);
        w_u8(attr, 0);
    }
    return 0;
}
