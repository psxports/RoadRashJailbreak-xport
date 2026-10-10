#include "psx.h"
/* Controller state and actuator handshake, MIPS 8001DDC4. */
#include "pad_service.h"
#include <stdlib.h>

static void byte(RRJMemory *m, uint32_t a, uint32_t v)
{
    w_u8(a, (uint8_t)v);
}

uint32_t sub_8001DDC4(uint32_t player, uint32_t multi, RRJPadSDK sdk)
{
    FUNCTION_MARKER(0x8001DDC4u, "SLUS_010.53");
    uint32_t pad, port, i, state, result;
    if (!sdk)
        abort();
    if (player == 1)
        rrj_write32(0x800D7440, multi ? 1 : 16);
    pad = 0x800D7428 + 24 * player;
    port = rrj_read32(pad);
    for (i = 0; i < 2; ++i)
    {
        uint32_t until = rrj_read32(pad + 16 + 4 * i);
        if (until && rrj_s32(until) < rrj_s32(rrj_read32(rrj_read32(0x8005B2F8) + 12)))
        {
            byte(rrj_host_context(), pad + 12 + i, 0);
            rrj_write32(pad + 16 + 4 * i, 0);
        }
    }
    state = sdk(rrj_host_context(), 0x80040550, port, 0, 0);
    if (!state)
    {
        rrj_write32(pad + 4, 0);
        return 1;
    }
    if (state == 1)
    {
        rrj_write32(pad + 4, 0);
        byte(rrj_host_context(), pad + 13, 0);
        byte(rrj_host_context(), pad + 12, 0);
        rrj_write32(pad + 20, 0);
        rrj_write32(pad + 16, 0);
    }
    if (rrj_read32(pad + 4))
    {
        result = sdk(rrj_host_context(), 0x8004061C, port, 2, 0);
        if (result)
            return result;
        if (rrj_read32(pad + 16))
        {
            byte(rrj_host_context(), pad + 12, 64);
            byte(rrj_host_context(), pad + 13, 1);
            return 1;
        }
        byte(rrj_host_context(), pad + 12, 64);
        byte(rrj_host_context(), pad + 13, 0);
        return 64;
    }
    (void)sdk(rrj_host_context(), 0x80040910, port, pad + 12, 2);
    if (state == 2)
    {
        rrj_write32(pad + 4, 2);
        return 6;
    }
    if (state != 6)
        return 6;
    result = sdk(rrj_host_context(), 0x80040890, port, 0x8005AD94, 0);
    if (result)
        rrj_write32(pad + 4, 1);
    return 1;
}
