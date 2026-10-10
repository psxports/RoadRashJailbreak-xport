#include "psx.h"
/* Original resident/menu VBlank counters and display-ready gates. */
#include "vblank.h"
#include "race_trace_runtime.h"
#include <stdlib.h>

static uint32_t b(RRJMemory *m, uint32_t a)
{
    return r_u8(a);
}

static void byte(RRJMemory *m, uint32_t a, uint32_t v)
{
    w_u8(a, (uint8_t)v);
}

static uint32_t call(RRJMemory *m, RRJVBlankCall cb, uint32_t f, uint32_t a)
{
    if (!cb)
        abort();
    return cb(m, f, a);
}

uint32_t sub_F_800600F8(void)
{
    FUNCTION_MARKER(0x800600F8u, "RASHCDF.BIN");
    uint32_t data = rrj_read32(0x800810BC), result = rrj_read32(data + 31520);
    if (result)
    {
        rrj_write32(data + 31524, rrj_read32(data + 31524) + 256);
        result = rrj_read32(data + 31528) + 1;
        rrj_write32(data + 31528, result);
    }
    return result;
}

uint32_t sub_8001B700(RRJVBlankCall cb)
{
    FUNCTION_MARKER(0x8001B700u, "SLUS_010.53");
    uint32_t state = rrj_read32(0x8005B2F8), frames = rrj_read32(0x8005B46C), ticks = rrj_read32(state + 12), count, ctx, result;
    uint32_t cd_callback;
    rrj_write32(0x8005B46C, frames + 1);
    cd_callback = rrj_trace_vblank_boundary(rrj_host_context());
    if (cd_callback)
        (void)call(rrj_host_context(), cb, cd_callback, 0);
    count = rrj_read32(state + 100);
    rrj_write32(state + 12, ticks + 5);
    ctx = rrj_read32(0x8005B470);
    rrj_write32(state + 100, count + 1);
    if (!b(rrj_host_context(), ctx + 4) && !call(rrj_host_context(), cb, 0x800487C0, 1))
    {
        ctx = rrj_read32(0x8005B470);
        (void)call(rrj_host_context(), cb, 0x80048E24, ctx + 112 * b(rrj_host_context(), ctx + 6) + 16);
        ctx = rrj_read32(0x8005B470);
        (void)call(rrj_host_context(), cb, 0x80048FF0, ctx + 112 * b(rrj_host_context(), ctx + 5) + 108);
        ctx = rrj_read32(0x8005B470);
        byte(rrj_host_context(), ctx + 6, 1 - b(rrj_host_context(), ctx + 6));
        ctx = rrj_read32(0x8005B470);
        byte(rrj_host_context(), ctx + 5, 1 - b(rrj_host_context(), ctx + 5));
        byte(rrj_host_context(), rrj_read32(0x8005B470) + 4, 1);
        if (rrj_read32(0x8005B588) == 1)
        {
            uint32_t ot = rrj_read32(0x8005B59C);
            rrj_write32(0x8005B588, 2);
            (void)call(rrj_host_context(), cb, 0x80048DB4, ot + 16);
        }
    }
    (void)call(rrj_host_context(), cb, 0x80019990, 0);
    if (rrj_read32(rrj_read32(0x8005B2F8) + 100) & 1)
        (void)call(rrj_host_context(), cb, 0x8001C5F8, 0);
    result = rrj_read32(0x8005AD88);
    if (result)
        result = call(rrj_host_context(), cb, 0x8001B8BC, 0);
    return result;
}

uint32_t sub_F_80064C30(RRJVBlankCall cb)
{
    FUNCTION_MARKER(0x80064C30u, "RASHCDF.BIN");
    uint32_t counter = rrj_read32(0x80088C40), ctx = rrj_read32(0x8005B470), suppress = 0;
    rrj_write32(0x80088C40, counter + 1);
    rrj_write32(0x80088C44, rrj_read32(0x80088C44) + 1);
    if (!b(rrj_host_context(), ctx + 4) && (rrj_u16(rrj_at(0x8009C5DA, 2)) & 1) && rrj_read32(0x80088C44) < 4)
    {
        byte(rrj_host_context(), ctx + 4, 1);
        suppress = 1;
    }
    else
    {
        ctx = rrj_read32(0x8005B470);
        if (!b(rrj_host_context(), ctx + 4) && rrj_read32(0x80088C44) < 2)
        {
            byte(rrj_host_context(), ctx + 4, 1);
            suppress = 1;
        }
    }
    (void)sub_8001B700(cb);
    if (suppress)
        byte(rrj_host_context(), rrj_read32(0x8005B470) + 4, 0);
    rrj_write32(0x8005ACAC, rrj_read32(0x8005ACAC) + 1);
    rrj_write32(0x80088C50, rrj_read32(0x80088C50) + 1);
    rrj_write32(0x80088C4C, rrj_read32(0x80088C4C) + 1);
    if (rrj_read32(0x80088C48))
        rrj_write32(0x80088C48, rrj_read32(0x80088C48) - 1);
    return sub_F_800600F8();
}
