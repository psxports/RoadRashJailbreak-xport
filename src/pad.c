#include "psx.h"
/* Raw PSX controller decoding and original per-button repeat state. */
#include "pad.h"
#include "rrj_runtime.h"
#include "pad_service.h"

uint32_t sub_8001C4A8(uint32_t pad, uint32_t mode)
{
    FUNCTION_MARKER(0x8001C4A8u, "SLUS_010.53");
    uint32_t table = 0x800526A4 + 60 * mode;
    w_u32(pad + 184, table);
    w_u32(pad + 180, mode + 1);
    return table;
}

uint32_t sub_8001C5F8(void)
{
    FUNCTION_MARKER(0x8001C5F8u, "SLUS_010.53");
    uint32_t count, multi, result, player, pad = 0x800D6DE0;
    w_u32(0x8005B478, 0);
    multi = ((uint32_t)r_u8(0x800D70E1) >> 4) == 8;
    count = r_u32(r_u32(0x8005B2F8) + 52);
    result = ((int32_t)(uint32_t)(count)) < 3;
    if (!multi && !result)
    {
        result = r_u32(0x8005B220);
        count = 2;
        if (!result)
        {
            result = 1;
            w_u32(0x8005B478, 1);
        }
    }
    if (((int32_t)(uint32_t)(count)) <= 0)
        return result;
    for (player = 0; player < count; ++player, pad += 192)
    {
        uint32_t raw = 0x800D70E0 + (multi ? 2 + 8 * player : 36 * player), bits, type, old, i, n = 15, keys[19];
        uint8_t snapshot[8];
        rrj_runtime_input(player, raw);
        w_u32(pad + 16, 0);
        w_u32(pad, r_u32(r_u32(0x8005B2F8) + 12));
        if (multi)
            for (i = 0; i < 8; ++i)
                snapshot[i] = (uint8_t)r_u8((raw + i));
        bits = ~(multi ? xport_load_le32(snapshot) : r_u32(raw));
        w_u32(pad + 4, bits);
        if (!(bits & 255))
        {
            if (!r_u32(0x8005B220) && (player < 2 || (uint32_t)r_u8((r_u32(0x8005B2F8) + 8 + player)) == 1))
                w_u32(0x8005B478, 1);
            w_u32(pad + 188, 1);
            w_u32(pad + 4, 0);
        }
        else
        {
            bits = r_u32(pad + 4);
            old = r_u32(pad + 12);
            w_u32(pad + 188, 0);
            type = ((~bits) & 0xff00) >> 8;
            w_u32(pad + 12, type);
            if (old != type)
            {
                uint32_t mode = r_u32(pad + 180);
                if (type == 0x73)
                    (void)sub_8001C4A8(pad, (mode - 1) < 2 ? 3 : 4);
                else if (type == 0x41)
                    (void)sub_8001C4A8(pad, (mode == 4 || mode == 1) ? 0 : 2);
            }
            type = r_u32(pad + 12);
            if (type == 0x73)
                w_u32(pad + 16, 1);
            else if (type != 0x41)
                w_u32(pad + 12, 0);
            w_u8((pad + 8), (uint8_t)(multi ? snapshot[7] : (uint32_t)r_u8((raw + 7))));
            type = multi ? snapshot[5] : (uint32_t)r_u8((raw + 5));
            bits = r_u32(pad + 4);
            w_u8((pad + 9), (uint8_t)type);
            type = multi ? snapshot[6] : (uint32_t)r_u8((raw + 6));
            w_u32(pad + 4, (bits >> 24) | ((bits >> 8) & 0xff00));
            w_u8((pad + 10), (uint8_t)type);
        }
        for (i = 0; i < 15; ++i)
            keys[i] = r_u32(pad + 4) & r_u32(0x80052658 + 4 * i);
        if (r_u32(pad + 16))
        {
            n = 19;
            keys[15] = (uint32_t)r_u8((pad + 9)) == 0;
            keys[16] = (uint32_t)r_u8((pad + 9)) == 255;
            keys[17] = (uint32_t)r_u8((pad + 8)) == 0;
            keys[18] = (uint32_t)r_u8((pad + 8)) == 255;
        }
        for (i = 0; i < n; ++i)
        {
            uint32_t rec = pad + 20 + 8 * i, held;
            if (keys[i])
            {
                held = r_u32(rec);
                if (((int32_t)(uint32_t)(held)) > 0)
                {
                    uint32_t first = (uint32_t)r_u8((rec + 4));
                    if (first < 128 && (uint32_t)r_u8((rec + 7)) >= (first ? 31u : 3u))
                    {
                        w_u8((rec + 4), (uint8_t)0);
                        w_u8((rec + 6), (uint8_t)255);
                        w_u8((rec + 7), (uint8_t)0);
                    }
                    w_u8((rec + 7), (uint8_t)((uint32_t)r_u8((rec + 7)) + 1));
                }
                else
                {
                    w_u32(rec, r_u32(pad));
                    w_u8((rec + 6), (uint8_t)((uint32_t)r_u8((rec + 5)) < 11 ? 2 : 1));
                    w_u8((rec + 5), (uint8_t)0);
                    w_u8((rec + 7), (uint8_t)0);
                    w_u8((rec + 4), (uint8_t)1);
                }
            }
            else
            {
                w_u8((rec + 4), (uint8_t)0);
                held = r_u32(rec);
                if (((int32_t)(uint32_t)(held)) > 0)
                    w_u32(rec, held - r_u32(pad));
            }
            if ((uint32_t)r_u8((rec + 5)) < 31)
                w_u8((rec + 5), (uint8_t)((uint32_t)r_u8((rec + 5)) + 1));
        }
        (void)sub_8001DDC4(player, multi);
    }
    return 0;
}
