#include "psx.h"
/* Raw PSX controller decoding and original per-button repeat state. */
#include "pad.h"
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

uint32_t sub_8001C4A8(uint32_t pad, uint32_t mode)
{
    FUNCTION_MARKER(0x8001C4A8u, "SLUS_010.53");
    uint32_t table = 0x800526A4 + 60 * mode;
    rrj_write32(pad + 184, table);
    rrj_write32(pad + 180, mode + 1);
    return table;
}

uint32_t sub_8001C5F8(RRJSDKCall service)
{
    FUNCTION_MARKER(0x8001C5F8u, "SLUS_010.53");
    uint32_t count, multi, result, player, pad = 0x800D6DE0;
    rrj_write32(0x8005B478, 0);
    multi = (b(rrj_host_context(), 0x800D70E1) >> 4) == 8;
    count = rrj_read32(rrj_read32(0x8005B2F8) + 52);
    result = rrj_s32(count) < 3;
    if (!multi && !result)
    {
        result = rrj_read32(0x8005B220);
        count = 2;
        if (!result)
        {
            result = 1;
            rrj_write32(0x8005B478, 1);
        }
    }
    if (rrj_s32(count) <= 0)
        return result;
    for (player = 0; player < count; ++player, pad += 192)
    {
        uint32_t raw = 0x800D70E0 + (multi ? 2 + 8 * player : 36 * player), bits, type, old, i, n = 15, keys[19];
        uint8_t snapshot[8];
        if (!rrj_trace_input_packet(rrj_host_context(), player, raw))
            return result;
        rrj_write32(pad + 16, 0);
        rrj_write32(pad, rrj_read32(rrj_read32(0x8005B2F8) + 12));
        if (multi)
            for (i = 0; i < 8; ++i)
                snapshot[i] = (uint8_t)b(rrj_host_context(), raw + i);
        bits = ~(multi ? rrj_u32(snapshot) : rrj_read32(raw));
        rrj_write32(pad + 4, bits);
        if (!(bits & 255))
        {
            if (!rrj_read32(0x8005B220) && (player < 2 || b(rrj_host_context(), rrj_read32(0x8005B2F8) + 8 + player) == 1))
                rrj_write32(0x8005B478, 1);
            rrj_write32(pad + 188, 1);
            rrj_write32(pad + 4, 0);
        }
        else
        {
            bits = rrj_read32(pad + 4);
            old = rrj_read32(pad + 12);
            rrj_write32(pad + 188, 0);
            type = ((~bits) & 0xff00) >> 8;
            rrj_write32(pad + 12, type);
            if (old != type)
            {
                uint32_t mode = rrj_read32(pad + 180);
                if (type == 0x73)
                    (void)sub_8001C4A8(pad, (mode - 1) < 2 ? 3 : 4);
                else if (type == 0x41)
                    (void)sub_8001C4A8(pad, (mode == 4 || mode == 1) ? 0 : 2);
            }
            type = rrj_read32(pad + 12);
            if (type == 0x73)
                rrj_write32(pad + 16, 1);
            else if (type != 0x41)
                rrj_write32(pad + 12, 0);
            byte(rrj_host_context(), pad + 8, multi ? snapshot[7] : b(rrj_host_context(), raw + 7));
            type = multi ? snapshot[5] : b(rrj_host_context(), raw + 5);
            bits = rrj_read32(pad + 4);
            byte(rrj_host_context(), pad + 9, type);
            type = multi ? snapshot[6] : b(rrj_host_context(), raw + 6);
            rrj_write32(pad + 4, (bits >> 24) | ((bits >> 8) & 0xff00));
            byte(rrj_host_context(), pad + 10, type);
        }
        for (i = 0; i < 15; ++i)
            keys[i] = rrj_read32(pad + 4) & rrj_read32(0x80052658 + 4 * i);
        if (rrj_read32(pad + 16))
        {
            n = 19;
            keys[15] = b(rrj_host_context(), pad + 9) == 0;
            keys[16] = b(rrj_host_context(), pad + 9) == 255;
            keys[17] = b(rrj_host_context(), pad + 8) == 0;
            keys[18] = b(rrj_host_context(), pad + 8) == 255;
        }
        for (i = 0; i < n; ++i)
        {
            uint32_t rec = pad + 20 + 8 * i, held;
            if (keys[i])
            {
                held = rrj_read32(rec);
                if (rrj_s32(held) > 0)
                {
                    uint32_t first = b(rrj_host_context(), rec + 4);
                    if (first < 128 && b(rrj_host_context(), rec + 7) >= (first ? 31u : 3u))
                    {
                        byte(rrj_host_context(), rec + 4, 0);
                        byte(rrj_host_context(), rec + 6, 255);
                        byte(rrj_host_context(), rec + 7, 0);
                    }
                    byte(rrj_host_context(), rec + 7, b(rrj_host_context(), rec + 7) + 1);
                }
                else
                {
                    rrj_write32(rec, rrj_read32(pad));
                    byte(rrj_host_context(), rec + 6, b(rrj_host_context(), rec + 5) < 11 ? 2 : 1);
                    byte(rrj_host_context(), rec + 5, 0);
                    byte(rrj_host_context(), rec + 7, 0);
                    byte(rrj_host_context(), rec + 4, 1);
                }
            }
            else
            {
                byte(rrj_host_context(), rec + 4, 0);
                held = rrj_read32(rec);
                if (rrj_s32(held) > 0)
                    rrj_write32(rec, held - rrj_read32(pad));
            }
            if (b(rrj_host_context(), rec + 5) < 31)
                byte(rrj_host_context(), rec + 5, b(rrj_host_context(), rec + 5) + 1);
        }
        if (!service)
            abort();
        service(rrj_host_context(), 0x8001DDC4, player, multi);
    }
    return 0;
}
