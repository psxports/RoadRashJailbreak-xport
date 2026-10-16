#include "race_pause.h"
#include "native_game_api.h"
#include "psx.h"
#include "gpu.h"
#include "draft_quick.h"

/* Unverified saved C integration for the first2 selected set */

uint32_t sub_8003D844(uint32_t a1, uint32_t a2)
{
    FUNCTION_MARKER(0x8003D844u, "SLUS_010.53");
    uint32_t id = a1 & 0xffffu;
    uint32_t object, entry, owner, metadata, record, slot, buffer;
    int32_t selected;
    if ((int32_t)id >= r_s16(r_u32(0x8005B240u) + 40))
        return 2;
    metadata = (uint32_t)sub_80039A08(id);
    if (!metadata || !r_u32(metadata + 12))
        return 2;
    object = 0x800CD898u + 1132u * a2;
    entry = r_u32(object + 328);
    owner = r_u32(0x8005B268u + 4u * a2);
    if (entry && r_u32(entry) == id)
        return 2;
    if (owner)
    {
        uint32_t linked = r_u32(owner + 852);
        if (r_u32(linked + 604) < 3)
        {
            entry = r_u32(owner + 328);
            if (entry && r_u32(entry) == id)
                return 2;
        }
        linked = r_u32(owner + 852);
        if (r_u32(linked + 604) - 3u < 2)
        {
            entry = r_u32(linked + 328);
            if (entry && r_u32(entry) == id)
                return 2;
        }
    }
    if ((r_u8(r_u32(0x8005B2F8u) + 4) & 1) && r_u32(object + 772) && r_u32(0x8005AD48u) == 1)
        return 2;
    selected = (int32_t)sub_8003CB6C(id, a2);
    if (selected >= 0 && r_u32(r_u32(0x8005B2F8u) + 48) == 2)
    {
        if (selected != 6)
        {
            slot = 0x800D4B10u + 16u * (uint32_t)selected;
            buffer = r_u32(slot + 8 + 4u * r_u32(slot + 4));
            record = r_u32(metadata + 12);
            if (!(uint32_t)sub_8003CFCC(buffer, id))
                return 0;
            sub_8003D39C(buffer, record);
        }
        return 1;
    }
    {
        uint32_t refresh = r_u32(0x8005B38Cu);
        uint32_t group;
        w_u32(metadata + 12, 0);
        if (!refresh && !r_u32(0x8005B21Cu))
            return 1;
        /* TODO Overlay targets are unavailable and must fail fast during integration */
        (uint32_t)sub_80093E6C();
        (uint32_t)sub_800950E8();
        for (group = 0; group < 5; ++group)
        {
            uint32_t table = 0x800CE4F0u + 16u * group;
            int32_t remaining = (int32_t)r_u32(r_u32(table + 12));
            uint32_t item = r_u32(table);
            uint32_t input_offset = group == 4 ? 0u : 172u;
            uint32_t result_offset = group == 4 ? 148u : 320u;
            while (remaining >= 0)
            {
                if (r_u16(item + input_offset))
                {
                    uint32_t value = (uint32_t)sub_80039F68(item + input_offset);
                    w_u16(item + result_offset, value);
                    if (!(value << 16))
                        (uint32_t)sub_8008C000(item + input_offset, group + 2);
                }
                --remaining;
                item += r_u32(table + 4);
            }
        }
    }
    return 1;
}

uint32_t sub_80032938(uint32_t a1, uint32_t a2)
{
    FUNCTION_MARKER(0x80032938u, "SLUS_010.53");
    uint32_t args[3] = {(((a1 | 0x80000000u) >> 13) & 0x7C00u) | 0x8000u | (a1 & 0x3FFu), 1, a2};
    (uint32_t)sub_80034D38(args[0], args[1], args[2]);
    return 1;
}

void sub_800245B4(void)
{
    uint32_t handle;
    FUNCTION_MARKER(0x800245B4, "SLUS_010.53");
    handle = r_u32(0x8005AE64);
    if (handle)
        (void)sub_800144B8(handle);
    w_u32(0x8005AE64, 0);
}

uint32_t sub_80038550(uint32_t actor, uint32_t distance, uint32_t direction, uint32_t track_record, uint32_t output)
{
    uint32_t record[8], candidates[24], directions[4];
    uint32_t track, value, denominator, numerator, quotient, gap, clamped, index;
    int32_t marker;
    int numerator_positive, denominator_positive;
    FUNCTION_MARKER(0x80038550u, "SLUS_010.53");
    xport_store_le32((uint8_t *)candidates + 28u, 0u);
    track = r_u32(track_record + 12u);
    marker = r_s16(track);
    if ((!marker || marker == r_s16(r_u32(track_record + 8u) + 10u) - 1) && sub_800394F0(xport_guest_ptr(track_record, 32), 1u))
        return 0u;
    /* Inline the original eight-word forward copy into the native local record */
    for (index = 0; index < 8; ++index)
        xport_store_le32((uint8_t *)record + 4u * index, r_u32(track_record + 4u * index));
    if (!sub_80037A30(actor + 172u, record, candidates, directions, 3u))
        return 0u;
    value = (uint32_t)sub_8001FEB4(r_s16(track + 36u));
    value = (uint32_t)sub_8001FC90((int32_t)direction, (int32_t)value);
    denominator = r_u32(track + 32u) - value;
    numerator_positive = (int32_t)distance > 0;
    denominator_positive = (int32_t)denominator > 0;
    numerator = numerator_positive ? distance : 0u - distance;
    value = denominator_positive ? denominator : 0u - denominator;
    quotient = sub_80010028(numerator, value);
    if (numerator_positive != denominator_positive)
        quotient = 0u - quotient;
    gap = 65536u - quotient;
    clamped = (~(uint32_t)((int32_t)quotient >> 31) & quotient) + ((uint32_t)((int32_t)gap >> 31) & gap);
    value = xport_load_le32((const uint8_t *)candidates + 12u);
    (void)sub_8002EB78(xport_guest_cptr(track + 8u, 6), xport_guest_cptr(value + 8u, 6), xport_guest_ptr(output, 6), 65536u - clamped, clamped);
    (void)sub_8002E468(xport_guest_ptr(output, 6));
    return 1u;
}

uint32_t sub_8003CB6C(uint32_t a1, uint32_t a2)
{
    FUNCTION_MARKER(0x8003CB6Cu, "SLUS_010.53");
    uint32_t index = 0, record = 0x800D4B10u;
    uint32_t other = a2 ^ 1u;
    if ((int32_t)r_u32(0x8005B31Cu) < 0)
        return 0xffffffffu;
    for (;;)
    {
        uint32_t value = r_u32(record);
        if (value != 0xffffffffu && value == a1)
            break;
        record += 16;
        ++index;
        if ((int32_t)r_u32(0x8005B31Cu) < (int32_t)index)
            return 0xffffffffu;
    }
    if (r_u32(record + 8 + 4u * other))
    {
        w_u32(record + 8 + 4u * a2, 0);
        if (r_u32(record + 4) != a2)
            return 6;
        w_u32(record + 4, other);
        return index;
    }
    {
        uint32_t count = r_u32(0x8005B320u);
        uint32_t last = r_u32(0x8005B31Cu);
        w_u32(record, 0xffffffffu);
        w_u32(record + 8, 0);
        w_u32(record + 12, 0);
        w_u32(0x8005B320u, count - 1);
        if ((int32_t)index >= (int32_t)last && last != 0xffffffffu)
        {
            do
            {
                last = r_u32(0x8005B31Cu);
                if (r_u32(0x800D4B10u + 16u * last) != 0xffffffffu)
                    break;
                w_u32(0x8005B31Cu, last - 1);
            } while (last - 1 != 0xffffffffu);
        }
    }
    return 0xffffffffu;
}

uint32_t sub_80017CF0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
{
    FUNCTION_MARKER(0x80017CF0u, "SLUS_010.53");
    uint32_t difference = r_u32(0x8005B41Cu) - r_u32(r_u32(0x8005B2F8u) + 12);
    uint32_t sign = (uint32_t)((int32_t)difference >> 31);
    uint32_t result = (int32_t)((difference + sign) ^ sign) < 76;
    if (!result)
    {
        uint32_t table = r_u32(0x8005B40Cu);
        uint32_t args[4];
        if (table)
        {
            uint32_t p = table + 72 * a1;
            uint32_t x = r_u32(p + 4), z = r_u32(p + 8);
            a3 = x + (uint32_t)((int32_t)(a3 - x) >> 2);
            a4 = z + (uint32_t)((int32_t)(a4 - z) >> 2);
        }
        args[0] = a3;
        args[1] = a4;
        args[2] = a2 + 98;
        args[3] = 0;
        (uint32_t)sub_80017BA0(args[0], args[1], args[2], args[3]);
        result = r_u32(r_u32(0x8005B2F8u) + 12);
        w_u32(0x8005B41Cu, result);
    }
    return result;
}

uint32_t sub_80017B6C(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
{
    FUNCTION_MARKER(0x80017B6Cu, "SLUS_010.53");
    uint32_t result = 72u * a4 + r_u32(0x8005B40Cu) + 4u * (a2 == 0x6D);
    w_u32(result + 48, a2);
    w_u32(result + 56, a1);
    w_u32(result + 64, a3);
    return result;
}

uint32_t sub_8003E338(uint32_t actor)
{
    uint32_t owner, header, first, next, candidate, table, index;
    int32_t count, left, value;
    FUNCTION_MARKER(0x8003E338u, "SLUS_010.53");
    if (!actor || !r_s16(actor + 320u))
        return 0u;
    owner = r_u32(actor + 328u);
    if (!owner || !r_u32(owner + 104u))
        return 0u;
    header = sub_80039AFC(r_u32(owner));
    if (!header || (int32_t)sub_8003EB58(actor + 184u, header, &first, &next, 0xFFFFFFFFu) < 0)
        return 0u;
    table = r_u32(r_u32(actor + 328u) + 104u);
    count = r_s16(table + 2u);
    if (count <= 0)
        return 0u;
    candidate = table + 4u;
    left = r_s16(first + 4u);
    for (index = 0; (int32_t)index < count; ++index, candidate += 24u)
    {
        value = r_s16(candidate);
        if (value == left && r_s16(candidate + 2u) == r_s16(next + 4u))
            return candidate;
        if (r_s16(candidate + 2u) == left && value == r_s16(next + 4u))
            return candidate;
    }
    return 0u;
}

uint32_t sub_8002DF14(uint32_t vector[2])
{
    uint32_t magnitude, absolute, denominator, reciprocal, x, y;
    FUNCTION_MARKER(0x8002DF14u, "SLUS_010.53");
    magnitude = sub_8002E010(vector);
    if (!magnitude)
        return 0u;
    absolute = (int32_t)magnitude < 0 ? 0u - magnitude : magnitude;
    denominator = (uint32_t)((int32_t)absolute >> 1) + (uint32_t)((int32_t)(absolute - 2u) >> 31);
    reciprocal = denominator ? 0x80000000u / denominator : 0xFFFFFFFFu;
    if ((int32_t)magnitude < 0)
        reciprocal = 0u - reciprocal;
    x = (uint32_t)sub_8001FC90((int32_t)reciprocal, (int32_t)xport_load_le32(vector));
    y = xport_load_le32(vector + 1);
    xport_store_le32(vector, x);
    xport_store_le32(vector + 1, (uint32_t)sub_8001FC90((int32_t)reciprocal, (int32_t)y));
    return 1u;
}

uint32_t sub_8002E010(const uint32_t a1[2])
{
    FUNCTION_MARKER(0x8002E010u, "SLUS_010.53");
    int64_t x = (int32_t)xport_load_le32(a1);
    int64_t y = (int32_t)xport_load_le32(a1 + 1);
    uint32_t sum = (uint32_t)((x * x) >> 16) + (uint32_t)((y * y) >> 16);
    return (uint32_t)sub_8004CF74(sum) << 2;
}

uint32_t sub_8002DFC0(const uint32_t a1[2], const uint32_t a2[2])
{
    FUNCTION_MARKER(0x8002DFC0u, "SLUS_010.53");
    uint32_t first = (uint32_t)sub_8001FC90(xport_load_le32(a1), xport_load_le32(a2));
    uint32_t second = (uint32_t)sub_8001FC90(xport_load_le32(a1 + 1), xport_load_le32(a2 + 1));
    return first + second;
}

uint32_t sub_8001C298(void)
{
    FUNCTION_MARKER(0x8001C298u, "SLUS_010.53");
    uint32_t owner = r_u32(0x8005B470u), index = 0, result;
    result = (uint8_t)r_u8((owner + 244));
    if (!result)
        return result;
    do
    {
        uint32_t p = r_u32(owner + 248 + index * 4);
        if (p)
            (uint32_t)sub_800144B8(p);
        owner = r_u32(0x8005B470u);
        result = (uint8_t)r_u8((owner + 244));
        ++index;
        result = (uint32_t)((int32_t)index < (int32_t)result);
        /* The branch delay slot overwrites the comparison result */
        if (!result)
            return index * 4;
    } while (1);
}

uint32_t sub_8001BBC0(void)
{
    uint16_t rectangle[4];
    uint8_t record[20];
    uint32_t buffer = 0;
    uint32_t result;

    FUNCTION_MARKER(0x8001BBC0, "SLUS_010.53");
    if (!sub_8001408C((const char *)xport_guest_cptr(0x80010AC8, 1), &buffer, record))
    {
        rectangle[0] = 122;
        rectangle[1] = 200;
        rectangle[2] = 140;
        rectangle[3] = 20;
        (void)rrj_gpu_upload((const uint8_t *)rectangle, xport_load_le32(record + 16));
        rectangle[1] = 456;
        (void)rrj_gpu_upload((const uint8_t *)rectangle, xport_load_le32(record + 16));
    }
    result = (uint32_t)DrawSync(0);
    if (buffer)
        return sub_800144B8(buffer);
    return result;
}
