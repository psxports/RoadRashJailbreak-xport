#include "psx.h"
/* Original background colour setup and clear-before-display-switch. */
#include "screen.h"
#include "menu.h"
#include <stdlib.h>

uint32_t sub_8001BF1C(uint32_t enabled, uint32_t red, uint32_t green, uint32_t blue, RRJScreenCall call)
{
    FUNCTION_MARKER(0x8001BF1Cu, "SLUS_010.53");
    uint32_t context, i;
    if (!call)
        abort();
    (void)call(rrj_host_context(), 0x800487C0, 0);
    (void)call(rrj_host_context(), 0x80047724, 0);
    context = rrj_read32(0x8005B470);
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

uint32_t sub_F_80080D08(RRJScreenCall call)
{
    FUNCTION_MARKER(0x80080D08u, "RASHCDF.BIN");
    if (!call)
        abort();
    (void)call(rrj_host_context(), 0x80048944, 0x80088C7C);
    (void)sub_8001C3F4();
    return call(rrj_host_context(), 0x8001C408, 0);
}

uint32_t sub_8001BE08(uint32_t x, uint32_t y, uint32_t width, uint32_t height, RRJScreenCall call, RRJScreenEnv env)
{
    FUNCTION_MARKER(0x8001BE08u, "SLUS_010.53");
    uint32_t i, draw = 0x800D6CC8, attr = 0x800D6CE3, context, h, w;
    if (!call || !env)
        abort();
    (void)call(rrj_host_context(), 0x800487C0, 0);
    (void)call(rrj_host_context(), 0x80047724, 0);
    rrj_write32(0x8005B470, 0x800D6CB8);
    rrj_write32(0x800D6CC0, width);
    rrj_write32(0x800D6CC4, height);
    for (i = 0; i < 2; ++i, draw += 112, attr += 112)
    {
        context = rrj_read32(0x8005B470);
        h = rrj_read32(context + 12);
        w = rrj_read32(context + 8);
        env(rrj_host_context(), 0x8004CC44, draw, 0, 256 * i, w, h);
        context = rrj_read32(0x8005B470);
        h = rrj_read32(context + 12);
        w = rrj_read32(context + 8);
        env(rrj_host_context(), 0x8004CD04, draw + 92, 0, 256 * i, w, h);
        w_u8(attr - 5, 1);
        w_u8(attr - 4, 0);
        rrj_put16(rrj_at(attr + 79, 2), (uint16_t)height);
        rrj_put16(rrj_at(attr + 75, 2), (uint16_t)y);
        w_u8(attr - 3, 0);
        w_u8(attr - 2, 0);
        w_u8(attr - 1, 0);
        w_u8(attr, 0);
    }
    return 0;
}
