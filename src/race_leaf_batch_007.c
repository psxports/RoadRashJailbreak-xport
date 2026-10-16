#include "rrj_debug.h"
#include "native_file_callbacks.h"
#include <stdio.h>
#include "native_game_api.h"
#include "race_leaf_batch_007.h"
#include "race_pause.h"
#include "xport.h"

uint32_t sub_800150EC(void)
{
    FUNCTION_MARKER(0x800150ECu, "SLUS_010.53");
    uint32_t callback = r_u32(0x8005B3B8u);
    return callback ? rrj_file_function(callback)() : 0;
}

uint32_t sub_80022EEC(void)
{
    FUNCTION_MARKER(0x80022EECu, "SLUS_010.53");
    uint32_t record = r_u32(0x8005B4ECu), completed = r_u32(0x8005B4E8u);
    uint32_t callback, first, mode, token;
    w_u32(0x8005AE20u, 0);
    callback = r_u32(record + 24u);
    w_u32(0x8005B4E8u, completed + 1u);
    if (callback)
    {
        first = r_u32(record + 12u);
        mode = r_u32(record);
        token = r_u32(record + 20u);
        (void)rrj_read_complete_function(callback)(first, mode, token);
    }
    sub_80022B0C(0);
    return 0;
}

uint32_t sub_800315E8(uint32_t object)
{
    FUNCTION_MARKER(0x800315E8, "SLUS_010.53");
    w_u32(object + 8u, 0xFFFFFFFFu);
    w_u32(object + 20u, 0);
    w_u32(object + 24u, 0);
    w_u32(object + 16u, r_u32(object + 12u));
    return 0xFFFFFFFFu;
}

uint32_t sub_80031540(uint32_t object)
{
    uint32_t base = r_u32(object + 12u);
    uint32_t kind;
    uint32_t current;
    int32_t offset;

    FUNCTION_MARKER(0x80031540, "SLUS_010.53");
    w_u32(object + 20u, base);
    w_u32(object + 8u, r_u32(base));
    kind = r_u32(base) >> 28;
    if (kind == 1u || kind == 2u)
    {
        w_u32(object + 24u, 0);
        w_u32(object + 16u, base);
        return base;
    }
    current = base + 32u;
    w_u32(object + 16u, current);
    if (kind != 0u && kind != 8u)
    {
        w_u32(object + 24u, 0);
        return current;
    }
    offset = (int32_t)(r_u32(current) * 4u - 48u);
    if (offset > 0)
        w_u32(object + 24u, current + (uint32_t)offset);
    return current + (uint32_t)offset;
}

uint32_t sub_80031CD4(uint32_t object)
{
    FUNCTION_MARKER(0x80031CD4u, "SLUS_010.53");
    uint32_t context = r_u32(0x8005ACBCu), count, index, next, corrected, result;
    count = r_u32(context + 2356u);
    if (count == 64u)
        return (uint32_t)RRJ_DEBUG_PRINTF((const char *)xport_guest_cptr(0x80010D2Cu, 1));
    w_u32(object, r_u32(object) | 0x80u);
    index = r_u32(context + 2348u);
    w_u32(context + 2360u + (index << 2), object);
    index = r_u32(context + 2348u);
    next = index + 1u;
    corrected = ((int32_t)(uint32_t)(next)) < 0 ? index + 64u : next;
    count = r_u32(context + 2356u);
    result = next - ((uint32_t)(((int32_t)(uint32_t)(corrected)) >> 6) << 6);
    w_u32(context + 2348u, result);
    w_u32(context + 2356u, count + 1u);
    return result;
}

uint32_t sub_80030894(uint32_t index, uint32_t mode, uint32_t value)
{
    FUNCTION_MARKER(0x80030894u, "SLUS_010.53");
    uint32_t context = r_u32(0x8005ACBCu);
    uint32_t object = context + 44u + 36u * index;
    uint32_t flags = r_u32(object), first, second;
    if ((flags & 9u) == 9u)
        (void)RRJ_DEBUG_PRINTF((const char *)xport_guest_cptr(0x80010D00u, 1));
    flags = (r_u32(object) | 1u) & 0xFFFFFFFBu;
    w_u32(object, flags);
    (void)sub_80032190(2, 1);
    flags = r_u32(object);
    if (flags & 0x10u)
    {
        context = r_u32(0x8005ACBCu);
        first = r_u32(context + 4u);
        second = r_u32(context + 24u);
        w_u32(context + 4u, first + 1u);
        w_u32(context + 24u, second - 1u);
    }
    else if (flags & 8u)
    {
        context = r_u32(0x8005ACBCu);
        first = r_u32(context + 8u);
        second = r_u32(context + 28u);
        w_u32(context + 8u, first + 1u);
        w_u32(context + 28u, second - 1u);
    }
    flags = r_u32(object);
    w_u32(object + 32u, value);
    w_u32(object, flags | mode);
    if (mode == 8u)
    {
        w_u32(object, r_u32(object) | 0x20u);
        (void)sub_800315E8(object);
        return sub_80031604(object, 0);
    }
    if (mode == 16u)
    {
        (void)sub_80031540(object);
        return sub_80031CD4(object);
    }
    return 16;
}

uint32_t sub_800151EC(void)
{
    FUNCTION_MARKER(0x800151EC, "SLUS_010.53");
    w_u32(0x8005B3C0u, 1);
    return 1;
}

uint32_t sub_80090270(void)
{
    uint32_t count_address = r_u32(0x800CE4DCu);
    int32_t remaining = (int32_t)r_u32(count_address);
    uint32_t object = r_u32(0x800CE4D0u);
    uint32_t eligible = 0;
    uint32_t result = count_address;

    FUNCTION_MARKER(0x80090270, "RASHCDG.BIN");
    while (remaining >= 0)
    {
        uint32_t state = r_u32(0x8005B2F8u);
        uint32_t type = r_u32(object + 1084u);

        if ((int16_t)r_u16(object + 320u) == 0 && r_u16(object + 172u) >= r_u32(state + 48u) && (r_u8(type + 1u) & 15u) != 2u)
            ++eligible;
        --remaining;
        object += r_u32(0x800CE4D4u);
    }

    count_address = r_u32(0x800CE4DCu);
    remaining = (int32_t)r_u32(count_address);
    object = r_u32(0x800CE4D0u);
    result = count_address;
    while (remaining >= 0)
    {
        uint32_t state = r_u32(0x8005B2F8u);
        uint32_t type = r_u32(object + 1084u);

        if ((r_u8(type + 1u) & 15u) != 2u || r_u16(object + 172u) < r_u32(state + 48u))
        {
            if ((int16_t)r_u16(object + 320u) != 0)
            {
                int32_t rank = (int32_t)r_u8(type + 39u) - (int32_t)eligible - 1;
                uint16_t value;

                if (rank < 0)
                    rank = 0;
                value = rank / 2 ? (uint16_t)(4 * (rank / 2)) : 1u;
                w_u16(object + 960u, value);
                if (r_u8(r_u32(object + 852u) + 572u) & 0x10u)
                {
                    uint32_t linked = r_u32(object + 856u);
                    uint32_t slot = (uint32_t)(int32_t)r_s8(linked + 946u);

                    w_u16(linked + 952u + slot * 8u, value);
                }
            }
            else
            {
                w_u16(object + 960u, 1u);
            }
        }
        result = r_u32(0x800CE4D4u);
        --remaining;
        object += result;
    }
    return result;
}

uint32_t sub_8003775C(uint32_t identity, uint32_t record[8], uint32_t delta)
{
    uint32_t candidates[24] = {0}, directions[3] = {0};
    uint32_t current, direction, magnitude, covered, progress = 0;
    uint32_t segment, count;
    uint32_t (*neighbors)(uint32_t, uint32_t[8], uint32_t[24], uint32_t[3], uint32_t);

    FUNCTION_MARKER(0x8003775C, "SLUS_010.53");
    if (!record)
        return 0;
    current = record[3];
    if (!delta)
        return current;
    direction = identity ? r_u32(identity + 192) : ((int32_t)delta > 0 ? 1 : 0xFFFFFFFF);
    if ((int32_t)direction > 0)
    {
        uint32_t length = r_u32(current + 32);
        covered = length - record[5];
    }
    else
        covered = record[5];
    magnitude = (delta + (uint32_t)((int32_t)delta >> 31)) ^ (uint32_t)((int32_t)delta >> 31);
    directions[0] = direction;
    if ((int32_t)covered >= (int32_t)magnitude)
    {
        progress = record[5];
        progress = (int32_t)direction > 0 ? progress + magnitude : progress - magnitude;
    }
    else
    {
        segment = (uint32_t)(int32_t)r_s16(record[3]);
        if (!segment || segment == (uint32_t)(int32_t)r_s16(record[2] + 10) - 1)
        {
            neighbors = (int32_t)direction > 0 ? sub_80037A30 : sub_80037FBC;
            count = neighbors(identity, record, candidates, directions, 3);
            current = sub_80039048(identity, candidates, directions, count, record, &direction);
        }
        else
        {
            current = (int32_t)direction > 0 ? current + 52 : current - 52;
            record[3] = current;
        }
        while ((int32_t)covered < (int32_t)magnitude)
        {
            uint32_t length = r_u32(current + 32);
            uint32_t remaining = magnitude - covered;
            if ((int32_t)length >= (int32_t)remaining)
            {
                progress = (int32_t)direction > 0 ? remaining : length - remaining;
                covered = magnitude;
            }
            else
            {
                segment = (uint32_t)(int32_t)r_s16(record[3]);
                covered += length;
                if (!segment || segment == (uint32_t)(int32_t)r_s16(record[2] + 10) - 1)
                {
                    if (sub_800394F0(record, direction))
                        break;
                    neighbors = (int32_t)direction > 0 ? sub_80037A30 : sub_80037FBC;
                    count = neighbors(identity, record, candidates, directions, 3);
                    current = sub_80039048(identity, candidates, directions, count, record, &direction);
                }
                else
                {
                    current = (int32_t)direction > 0 ? current + 52 : current - 52;
                    record[3] = current;
                }
            }
        }
    }
    record[5] = progress;
    return current;
}
