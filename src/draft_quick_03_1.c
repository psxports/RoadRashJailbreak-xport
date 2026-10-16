#include "rrj_debug.h"
#include "fixed_math.h"
#include <stdlib.h>
#include <stdio.h>
#include "sdk_build_stubs.h"
#include "psx.h"
#include "native_game_api.h"
/* UNVERIFIED DRAFT Ordinary C derived from complete IDA and MIPS */
#include "draft_quick.h"
#include "race_bodyless_batch_002.h"

uint32_t sub_80014550(const char *path, uint32_t device)
{
    FUNCTION_MARKER(0x80014550u, "SLUS_010.53");
    uint32_t result = sub_8001458C(path, device);
    if ((int32_t)result < 0)
        return (uint32_t)-5;
    (uint32_t)sub_8001460C(result);
    return 0;
}

uint32_t sub_80031BC8(uint32_t a1, uint32_t a2)
{
    FUNCTION_MARKER(0x80031BC8u, "SLUS_010.53");
    return sub_80023900(r_u32(a1) >> 28, a1 + 4, a2) == 0;
}

void sub_80040444(void)
{
    FUNCTION_MARKER(0x80040444, "SLUS_010.53");
    /* The linked image contains zero destructor entries */
}

uint32_t sub_80045C0C(uint32_t a0, uint32_t a1)
{
    FUNCTION_MARKER(0x80045C0Cu, "SLUS_010.53");
    return sub_80047654(a0, a1) == 0;
}

uint32_t sub_80048118(uint32_t channel, uint32_t callback)
{
    uint32_t slot = 0x80055EAC + 4 * channel;
    uint32_t previous;
    FUNCTION_MARKER(0x80048118, "SLUS_010.53");
    previous = r_u32(slot);
    if (callback != previous)
        w_u32(slot, callback);
    return previous;
}

typedef uint32_t (*RRJ_DMA_CALLBACK_SET)(uint32_t channel, uint32_t callback);

static RRJ_DMA_CALLBACK_SET dma_callback_set_function(uint32_t address)
{
    if (address == 0x80048118)
        return sub_80048118;
    RRJ_DEBUG_FPRINTF(stderr, "Missing DMA callback setter at %08X\n", address);
    abort();
}

uint32_t sub_800479F8(uint32_t channel, uint32_t callback)
{
    RRJ_DMA_CALLBACK_SET function;
    FUNCTION_MARKER(0x800479F8, "SLUS_010.53");
    function = dma_callback_set_function(r_u32(r_u32(0x80055E8C) + 20));
    return function(channel, callback);
}

uint32_t sub_800407E8(uint32_t port, int32_t index, int32_t field)
{
    uint32_t owner, count, row;
    FUNCTION_MARKER(0x800407E8, "SLUS_010.53");
    owner = rrj_pad_function(r_u32(0x80054910))(port);
    count = r_u8(owner + 234);
    if (index < 0)
        return count;
    if (index >= (int32_t)count)
        return 0;
    row = r_u32(owner + 8) + 8 * (uint32_t)index;
    count = r_u8(row);
    if (field < 0)
        return count;
    return field < (int32_t)count ? r_u8(r_u32(row + 4) + (uint32_t)field) : 0;
}

#ifndef NDEBUG
RRJ_SDK_PRINTF rrj_sdk_printf_function(uint32_t address)
{
    if (address == 0x80044894)
        return printf;
    RRJ_DEBUG_FPRINTF(stderr, "RRJ: missing SDK printf target %08X\n", address);
    abort();
}
#endif

uint32_t sub_80048BE4(uint32_t ordering_table, uint32_t count)
{
    uint32_t current = ordering_table, remaining = count;
    FUNCTION_MARKER(0x80048BE4, "SLUS_010.53");
#ifndef NDEBUG
    if (r_u8(0x80055F6E) >= 2)
    {
        RRJ_SDK_PRINTF print = rrj_sdk_printf_function(r_u32(0x80055F68));
        (void)print((const char *)xport_guest_cptr(0x80011218, 1), ordering_table, (int32_t)count);
    }
#endif
    --remaining;
    while (remaining)
    {
        --remaining;
        w_u8(current + 3, 0);
        w_u32(current, (r_u32(current) & 0xFF000000) | ((current + 4) & 0xFFFFFF));
        current += 4;
    }
    w_u32(0x8005602C, (0x80056018 & 0xFFFFFF) | 0x04000000);
    w_u32(current, 0x8005602C & 0xFFFFFF);
    return current;
}

uint64_t sub_8002EF48(uint32_t a1, uint32_t a2, uint32_t a3)
{
    FUNCTION_MARKER(0x8002EF48u, "SLUS_010.53");
    uint8_t matrix[36];
    uint32_t index;
    for (index = 0; index < 9; ++index)
        xport_store_le32(matrix + 4 * index, (uint32_t)(int32_t)r_s16(a2 + 2 * index) << 4);
    return (uint64_t)sub_8002E928(xport_guest_cptr(a1, 12), matrix, xport_guest_ptr(a3, 12));
}

int32_t sub_8003FDF0(int16_t a1, int16_t a2, int16_t a3, uint32_t a4)
{
    uint32_t scalar_v_6;
    uint32_t scalar_n_6;
    uint32_t scalar_n_7;
    uint32_t scalar_v_8;
    uint32_t scalar_n_8;

    FUNCTION_MARKER(0x8003FDF0u, "SLUS_010.53");
    uint32_t first = (uint32_t)(int32_t)r_s16((0x8005624Cu + 4u * ((uint16_t)a1 & 4095))) << 4;
    uint32_t third = (uint32_t)(int32_t)r_s16((0x8005624Cu + 4u * ((uint16_t)a3 & 4095))) << 4;
    uint32_t first_c = (uint32_t)(int32_t)r_s16((0x8005624Eu + 4u * ((uint16_t)a1 & 4095))) << 4;
    uint32_t second = (uint32_t)(int32_t)r_s16((0x8005624Cu + 4u * ((uint16_t)a2 & 4095))) << 4;
    uint32_t second_c = (uint32_t)(int32_t)r_s16((0x8005624Eu + 4u * ((uint16_t)a2 & 4095))) << 4;
    uint32_t third_c = (uint32_t)(int32_t)r_s16((0x8005624Eu + 4u * ((uint16_t)a3 & 4095))) << 4;
    uint32_t combined = (uint32_t)sub_8001FC90((int32_t)(third), (int32_t)(first)), combined_c = (uint32_t)sub_8001FC90((int32_t)(first), (int32_t)(third_c));
    uint32_t elements[9], t, result, i;
    t = (uint32_t)sub_8001FC90((int32_t)(third_c), (int32_t)(second_c));
    elements[0] = t + (uint32_t)sub_8001FC90((int32_t)(combined), (int32_t)(second));
    elements[1] = (uint32_t)sub_8001FC90((int32_t)(third), (int32_t)(first_c));
    t = (uint32_t)sub_8001FC90((int32_t)(third_c), (int32_t)(second));
    elements[2] = (uint32_t)sub_8001FC90((int32_t)(combined), (int32_t)(second_c)) - t;
    t = (uint32_t)sub_8001FC90((int32_t)(third), (int32_t)(second_c));
    elements[3] = (uint32_t)sub_8001FC90((int32_t)(combined_c), (int32_t)(second)) - t;
    elements[4] = (uint32_t)sub_8001FC90((int32_t)(third_c), (int32_t)(first_c));
    t = (uint32_t)sub_8001FC90((int32_t)(third), (int32_t)(second));
    elements[5] = t + (uint32_t)sub_8001FC90((int32_t)(combined_c), (int32_t)(second_c));
    elements[6] = (uint32_t)sub_8001FC90((int32_t)(first_c), (int32_t)(second));
    elements[7] = 0u - first;
    result = (uint32_t)sub_8001FC90((int32_t)(first_c), (int32_t)(second_c));
    for (i = 0; i < 7; ++i)
        w_u16((a4 + i * 2), ((uint16_t)((uint16_t)((scalar_v_6 = (elements[i])), (scalar_n_6 = (4)), (scalar_n_6 &= 31), (((uint32_t)((!scalar_n_6) ? (scalar_v_6) : ((scalar_v_6 >> scalar_n_6) | ((scalar_v_6 & 0x80000000u) ? (~0u << (32 - scalar_n_6)) : 0)))))))));
    result = ((scalar_n_7 = (4)), (scalar_n_7 &= 31), (((uint32_t)((!scalar_n_7) ? (((uint32_t)(result))) : ((((uint32_t)(result)) >> scalar_n_7) | ((((uint32_t)(result)) & 0x80000000u) ? (~0u << (32 - scalar_n_7)) : 0))))));
    w_u16((a4 + 16), ((uint16_t)((uint16_t)result)));
    w_u16((a4 + 14), ((uint16_t)((uint16_t)((scalar_v_8 = (elements[7])), (scalar_n_8 = (4)), (scalar_n_8 &= 31), (((uint32_t)((!scalar_n_8) ? (scalar_v_8) : ((scalar_v_8 >> scalar_n_8) | ((scalar_v_8 & 0x80000000u) ? (~0u << (32 - scalar_n_8)) : 0)))))))));
    return (int32_t)result;
}

uint32_t sub_800461D4(void)
{
    uint32_t result = 1, start;
    FUNCTION_MARKER(0x800461D4, "SLUS_010.53");
    if (r_u32(0x80054A08) & 1)
        (void)CdDataSync(0);
    w_u32(0x80054AA8, 0);
    if (r_u32(0x80054AB8))
    {
        start = (uint32_t)VSync(-1);
        if (!r_u32(0x80054AB8))
            result = 0;
        else
        {
            for (;;)
            {
                if ((uint32_t)((uint32_t)VSync(-1) - start) >= 121)
                {
                    (void)CdSyncCallbackPSX(r_u32(0x80054ABC));
                    w_u32(0x80054AB8, 0);
                    result = UINT32_MAX;
                    break;
                }
                if (!r_u32(0x80054AB8))
                {
                    result = 0;
                    break;
                }
            }
        }
    }
    if (!result)
        return 0;
    (void)CdReadyCallbackPSX(r_u32(0x80054AC0));
    if (r_u32(0x80054A08) & 1)
        rrj_cd_data_callback_psx(r_u32(0x80054AC4));
    (void)CdSyncCallbackPSX(0x80045C74);
    (void)CdControlF(9, NULL);
    w_u32(0x80054AB8, 1);
    result = r_u32(0x80054AB8);
    if (!result)
        return 0;
    start = (uint32_t)VSync(-1);
    result = start;
    if (!r_u32(0x80054AB8))
        return result;
    for (;;)
    {
        if ((uint32_t)((uint32_t)VSync(-1) - start) >= 121)
        {
            result = CdSyncCallbackPSX(r_u32(0x80054ABC));
            w_u32(0x80054AB8, 0);
            return result;
        }
        result = r_u32(0x80054AB8);
        if (!result)
            return result;
    }
}

uint32_t sub_8002F634(uint32_t intervals, uint32_t samples, uint32_t output, int32_t count)
{
    uint32_t *coefficients, *constants;
    uint32_t value = 0, index, result, a, b, slope, intercept, mixed;
    uint32_t constant, previous, diagonal, following, magnitude, denominator, reciprocal;
    uint32_t first, second, third, interval, next_interval;
    FUNCTION_MARKER(0x8002F634, "SLUS_010.53");
    if (count <= 1)
    {
        w_u32(output + 4, 0);
        return 0;
    }
    /* Size the native arrays by the caller's count instead of overflowing the original ten slots */
    coefficients = calloc((size_t)count, sizeof(*coefficients));
    constants = calloc((size_t)count, sizeof(*constants));
    if (!coefficients || !constants)
    {
        free(coefficients);
        free(constants);
        abort();
    }
    coefficients[1] = 0x10000;
    for (index = 1; (int32_t)index < count; ++index)
    {
        next_interval = r_u32(intervals + 4 * index);
        first = r_u32(samples + 4 * (index - 1));
        second = r_u32(samples + 4 * index);
        third = r_u32(samples + 4 * (index + 1));
        interval = r_u32(intervals + 4 * (index - 1));
        (void)sub_8002F4D8(first, second, third, interval, next_interval, &constant, &previous, &diagonal, &following);
        a = (uint32_t)sub_8001FC90((int32_t)previous, (int32_t)coefficients[index - 1]);
        b = (uint32_t)sub_8001FC90((int32_t)diagonal, (int32_t)coefficients[index]);
        slope = a + b;
        a = (uint32_t)sub_8001FC90((int32_t)previous, (int32_t)constants[index - 1]);
        b = (uint32_t)sub_8001FC90((int32_t)diagonal, (int32_t)constants[index]);
        intercept = constant + a + b;
        if ((int32_t)index == count - 1)
        {
            mixed = intercept + (uint32_t)sub_8001FC90((int32_t)following, r_s32(output + 4 * (index + 1)));
            if ((int32_t)mixed > 0)
            {
                mixed = intercept + (uint32_t)sub_8001FC90((int32_t)following, r_s32(output + 4 * (index + 1)));
                value = (int32_t)slope > 0 ? 0 - sub_80010028(mixed, slope) : sub_80010028(mixed, 0 - slope);
            }
            else
            {
                mixed = intercept + (uint32_t)sub_8001FC90((int32_t)following, r_s32(output + 4 * (index + 1)));
                value = (int32_t)slope > 0 ? sub_80010028(0 - mixed, slope) : 0 - sub_80010028(0 - mixed, 0 - slope);
            }
        }
        else
        {
            magnitude = (int32_t)following < 0 ? 0 - following : following;
            denominator = (uint32_t)((int32_t)magnitude >> 1) + (uint32_t)((int32_t)(magnitude - 2) >> 31);
            reciprocal = denominator ? 0x80000000 / denominator : UINT32_MAX;
            if ((int32_t)following < 0)
                reciprocal = 0 - reciprocal;
            coefficients[index + 1] = 0 - (uint32_t)sub_8001FC90((int32_t)slope, (int32_t)reciprocal);
            constants[index + 1] = 0 - (uint32_t)sub_8001FC90((int32_t)intercept, (int32_t)reciprocal);
        }
    }
    w_u32(output + 4, value);
    result = count > 2;
    for (index = 2; (int32_t)index < count; ++index)
    {
        a = (uint32_t)sub_8001FC90((int32_t)coefficients[index], (int32_t)value);
        w_u32(output + 4 * index, a + constants[index]);
        result = (int32_t)(index + 1) < count;
    }
    free(coefficients);
    free(constants);
    return result;
}
