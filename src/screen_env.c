#include "psx.h"
/* PsyQ default environments, verified against this executable's MIPS. */
#include "screen_env.h"
#include <stdlib.h>

static void half(RRJMemory *m, uint32_t a, uint32_t v)
{
    rrj_put16(rrj_at(a, 2), (uint16_t)v);
}

static void byte(RRJMemory *m, uint32_t a, uint32_t v)
{
    w_u8(a, (uint8_t)v);
}

uint32_t sub_8004CC44(uint32_t p, uint32_t x, uint32_t y, uint32_t w, uint32_t h, RRJScreenCall call)
{
    FUNCTION_MARKER(0x8004CC44u, "SLUS_010.53");
    uint32_t mode;
    if (!call)
        abort();
    mode = call(rrj_host_context(), 0x80048428, 0);
    half(rrj_host_context(), p, x);
    half(rrj_host_context(), p + 2, y);
    half(rrj_host_context(), p + 4, w);
    half(rrj_host_context(), p + 12, 0);
    half(rrj_host_context(), p + 14, 0);
    half(rrj_host_context(), p + 16, 0);
    half(rrj_host_context(), p + 18, 0);
    byte(rrj_host_context(), p + 25, 0);
    byte(rrj_host_context(), p + 26, 0);
    byte(rrj_host_context(), p + 27, 0);
    byte(rrj_host_context(), p + 22, 1);
    half(rrj_host_context(), p + 6, h);
    byte(rrj_host_context(), p + 23, rrj_s32(h) < (mode ? 289 : 257));
    half(rrj_host_context(), p + 8, x);
    half(rrj_host_context(), p + 10, y);
    half(rrj_host_context(), p + 20, 10);
    byte(rrj_host_context(), p + 24, 0);
    return p;
}

uint32_t sub_8004CD04(uint32_t p, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
    FUNCTION_MARKER(0x8004CD04u, "SLUS_010.53");
    half(rrj_host_context(), p, x);
    half(rrj_host_context(), p + 2, y);
    half(rrj_host_context(), p + 4, w);
    half(rrj_host_context(), p + 8, 0);
    half(rrj_host_context(), p + 10, 0);
    half(rrj_host_context(), p + 12, 0);
    half(rrj_host_context(), p + 14, 0);
    byte(rrj_host_context(), p + 17, 0);
    byte(rrj_host_context(), p + 16, 0);
    byte(rrj_host_context(), p + 19, 0);
    byte(rrj_host_context(), p + 18, 0);
    half(rrj_host_context(), p + 6, h);
    return p;
}
