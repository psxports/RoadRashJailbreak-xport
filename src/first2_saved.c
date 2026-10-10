#include "native_game_api.h"
#include "psx.h"
#include "draft_quick.h"

/* Unverified saved C integration for the first2 selected set */

static uint32_t quick01_call0(RRJMemory *m, uint32_t target)
{
    return rrj_draft_call(m, target, 0, 0);
}

static uint32_t quick01_call1(RRJMemory *m, uint32_t target, uint32_t a)
{
    uint32_t args[1] = {a};
    return rrj_draft_call(m, target, args, 1);
}

static uint32_t quick01_call2(RRJMemory *m, uint32_t target, uint32_t a, uint32_t b)
{
    uint32_t args[2] = {a, b};
    return rrj_draft_call(m, target, args, 2);
}

static uint32_t quick01_reciprocal(uint32_t value)
{
    uint32_t magnitude = (int32_t)value < 0 ? 0u - value : value;
    uint32_t denominator = (uint32_t)((int32_t)magnitude >> 1) + (uint32_t)((int32_t)(magnitude - 2u) >> 31);
    uint32_t quotient = denominator ? 0x80000000u / denominator : 0xffffffffu;
    return (int32_t)value < 0 ? 0u - quotient : quotient;
}

static uint8_t q03_read8(RRJMemory *m, uint32_t p)
{
    return *(uint8_t *)rrj_at(p, 1);
}

static uint16_t q03_read16(RRJMemory *m, uint32_t p)
{
    uint8_t *b = (uint8_t *)rrj_at(p, 2);
    return (uint16_t)(b[0] | ((uint16_t)b[1] << 8));
}

static void q03_write16(RRJMemory *m, uint32_t p, uint16_t v)
{
    uint8_t *b = (uint8_t *)rrj_at(p, 2);
    b[0] = (uint8_t)v;
    b[1] = (uint8_t)(v >> 8);
}

static uint32_t q03_call1(RRJMemory *m, uint32_t target, uint32_t a)
{
    return rrj_draft_call(m, target, &a, 1);
}

static uint32_t q03_call2(RRJMemory *m, uint32_t target, uint32_t a, uint32_t b)
{
    uint32_t args[2] = {a, b};
    return rrj_draft_call(m, target, args, 2);
}

uint32_t rrj_draft_8003D844(uint32_t a1, uint32_t a2)
{
    FUNCTION_MARKER(0x8003D844u, "SLUS_010.53");
    uint32_t id = a1 & 0xffffu;
    uint32_t object, entry, owner, metadata, record, slot, buffer;
    int32_t selected;
    if ((int32_t)id >= r_s16(rrj_read32(0x8005B240u) + 40))
        return 2;
    metadata = (uint32_t)sub_80039A08(id);
    if (!metadata || !rrj_read32(metadata + 12))
        return 2;
    object = 0x800CD898u + 1132u * a2;
    entry = rrj_read32(object + 328);
    owner = rrj_read32(0x8005B268u + 4u * a2);
    if (entry && rrj_read32(entry) == id)
        return 2;
    if (owner)
    {
        uint32_t linked = rrj_read32(owner + 852);
        if (rrj_read32(linked + 604) < 3)
        {
            entry = rrj_read32(owner + 328);
            if (entry && rrj_read32(entry) == id)
                return 2;
        }
        linked = rrj_read32(owner + 852);
        if (rrj_read32(linked + 604) - 3u < 2)
        {
            entry = rrj_read32(linked + 328);
            if (entry && rrj_read32(entry) == id)
                return 2;
        }
    }
    if ((r_u8(rrj_read32(0x8005B2F8u) + 4) & 1) && rrj_read32(object + 772) && rrj_read32(0x8005AD48u) == 1)
        return 2;
    selected = (int32_t)rrj_draft_8003CB6C(id, a2);
    if (selected >= 0 && rrj_read32(rrj_read32(0x8005B2F8u) + 48) == 2)
    {
        if (selected != 6)
        {
            slot = 0x800D4B10u + 16u * (uint32_t)selected;
            buffer = rrj_read32(slot + 8 + 4u * rrj_read32(slot + 4));
            record = rrj_read32(metadata + 12);
            if (!(uint32_t)sub_8003CFCC(buffer, id))
                return 0;
            rrj_draft_8003D39C(buffer, record);
        }
        return 1;
    }
    {
        uint32_t refresh = rrj_read32(0x8005B38Cu);
        uint32_t group;
        rrj_write32(metadata + 12, 0);
        if (!refresh && !rrj_read32(0x8005B21Cu))
            return 1;
        /* TODO Overlay targets are unavailable and must fail fast during integration */
        quick01_call0(rrj_host_context(), 0x80093E6Cu);
        quick01_call0(rrj_host_context(), 0x800950E8u);
        for (group = 0; group < 5; ++group)
        {
            uint32_t table = 0x800CE4F0u + 16u * group;
            int32_t remaining = (int32_t)rrj_read32(rrj_read32(table + 12));
            uint32_t item = rrj_read32(table);
            uint32_t input_offset = group == 4 ? 0u : 172u;
            uint32_t result_offset = group == 4 ? 148u : 320u;
            while (remaining >= 0)
            {
                if (r_u16(item + input_offset))
                {
                    uint32_t value = (uint32_t)sub_80039F68(item + input_offset);
                    w_u16(item + result_offset, value);
                    if (!(value << 16))
                        quick01_call2(rrj_host_context(), 0x8008C000u, item + input_offset, group + 2);
                }
                --remaining;
                item += rrj_read32(table + 4);
            }
        }
    }
    return 1;
}

uint32_t rrj_draft_80032938(uint32_t a1, uint32_t a2)
{
    FUNCTION_MARKER(0x80032938u, "SLUS_010.53");
    uint32_t args[3] = {(((a1 | 0x80000000u) >> 13) & 0x7C00u) | 0x8000u | (a1 & 0x3FFu), 1, a2};
    (uint32_t)sub_80034D38(args[0], args[1], args[2]);
    return 1;
}

uint32_t rrj_draft_800245B4(uint32_t incoming_result)
{
    FUNCTION_MARKER(0x800245B4u, "SLUS_010.53");
    uint32_t handle = rrj_read32(0x8005AE64u);
    uint32_t result = incoming_result;
    if (handle)
        result = (uint32_t)sub_800144B8(handle);
    rrj_write32(0x8005AE64u, 0);
    return result;
}

uint32_t rrj_draft_80038550(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t output)
{
    FUNCTION_MARKER(0x80038550u, "SLUS_010.53");
    uint32_t track = rrj_read32(a4 + 12);
    uint32_t local, args[5], result, delta, quotient, clamped;
    int32_t marker = r_s16(track);
    if (!marker || marker == (int32_t)r_s16(rrj_read32(a4 + 8) + 10) - 1)
    {
        args[0] = a4;
        args[1] = 1;
        if (rrj_draft_call(rrj_host_context(), 0x800394F0u, args, 2))
            return 0;
    }
    local = rrj_draft_local_alloc(rrj_host_context(), 0xA8);
    /* TODO Bind local guest buffers without inventing initialized bytes */
    rrj_write32(local + 0x54, 0);
    args[0] = local + 0x18;
    args[1] = a4;
    args[2] = 32;
    rrj_draft_call(rrj_host_context(), 0x8001E0B4u, args, 3);
    args[0] = a1 + 172;
    args[1] = local + 0x18;
    args[2] = local + 0x38;
    args[3] = local + 0x98;
    args[4] = 3;
    result = rrj_draft_call(rrj_host_context(), 0x80037A30u, args, 5);
    if (!result)
    {
        rrj_draft_local_free(rrj_host_context(), local);
        return 0;
    }
    args[0] = (uint32_t)(int32_t)r_s16(track + 36);
    result = rrj_draft_call(rrj_host_context(), 0x8001FEB4u, args, 1);
    args[0] = a3;
    args[1] = result;
    result = (uint32_t)rrj_draft_call64(rrj_host_context(), 0x8001FC90u, args, 2);
    delta = rrj_read32(track + 32) - result;
    args[0] = (int32_t)a2 <= 0 ? 0u - a2 : a2;
    args[1] = (int32_t)delta <= 0 ? 0u - delta : delta;
    quotient = rrj_draft_call(rrj_host_context(), 0x80010028u, args, 2);
    if (((int32_t)a2 > 0) != ((int32_t)delta > 0))
        quotient = 0u - quotient;
    clamped = (~(uint32_t)((int32_t)quotient >> 31) & quotient) + ((uint32_t)((int32_t)(0x10000u - quotient) >> 31) & (0x10000u - quotient));
    args[0] = track + 8;
    args[1] = rrj_read32(local + 0x44) + 8;
    args[2] = output;
    args[3] = 0x10000u - clamped;
    args[4] = clamped;
    rrj_draft_call64(rrj_host_context(), 0x8002EB78u, args, 5);
    args[0] = output;
    rrj_draft_call(rrj_host_context(), 0x8002E468u, args, 1);
    rrj_draft_local_free(rrj_host_context(), local);
    return 1;
}

uint32_t rrj_draft_8003CB6C(uint32_t a1, uint32_t a2)
{
    FUNCTION_MARKER(0x8003CB6Cu, "SLUS_010.53");
    uint32_t index = 0, record = 0x800D4B10u;
    uint32_t other = a2 ^ 1u;
    if ((int32_t)rrj_read32(0x8005B31Cu) < 0)
        return 0xffffffffu;
    for (;;)
    {
        uint32_t value = rrj_read32(record);
        if (value != 0xffffffffu && value == a1)
            break;
        record += 16;
        ++index;
        if ((int32_t)rrj_read32(0x8005B31Cu) < (int32_t)index)
            return 0xffffffffu;
    }
    if (rrj_read32(record + 8 + 4u * other))
    {
        rrj_write32(record + 8 + 4u * a2, 0);
        if (rrj_read32(record + 4) != a2)
            return 6;
        rrj_write32(record + 4, other);
        return index;
    }
    {
        uint32_t count = rrj_read32(0x8005B320u);
        uint32_t last = rrj_read32(0x8005B31Cu);
        rrj_write32(record, 0xffffffffu);
        rrj_write32(record + 8, 0);
        rrj_write32(record + 12, 0);
        rrj_write32(0x8005B320u, count - 1);
        if ((int32_t)index >= (int32_t)last && last != 0xffffffffu)
        {
            do
            {
                last = rrj_read32(0x8005B31Cu);
                if (rrj_read32(0x800D4B10u + 16u * last) != 0xffffffffu)
                    break;
                rrj_write32(0x8005B31Cu, last - 1);
            } while (last - 1 != 0xffffffffu);
        }
    }
    return 0xffffffffu;
}

uint32_t rrj_draft_80017CF0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
{
    FUNCTION_MARKER(0x80017CF0u, "SLUS_010.53");
    uint32_t difference = rrj_read32(0x8005B41Cu) - rrj_read32(rrj_read32(0x8005B2F8u) + 12);
    uint32_t sign = (uint32_t)((int32_t)difference >> 31);
    uint32_t result = (int32_t)((difference + sign) ^ sign) < 76;
    if (!result)
    {
        uint32_t table = rrj_read32(0x8005B40Cu);
        uint32_t args[4];
        if (table)
        {
            uint32_t p = table + 72 * a1;
            uint32_t x = rrj_read32(p + 4), z = rrj_read32(p + 8);
            a3 = x + (uint32_t)((int32_t)(a3 - x) >> 2);
            a4 = z + (uint32_t)((int32_t)(a4 - z) >> 2);
        }
        args[0] = a3;
        args[1] = a4;
        args[2] = a2 + 98;
        args[3] = 0;
        (uint32_t)sub_80017BA0(args[0], args[1], args[2], args[3]);
        result = rrj_read32(rrj_read32(0x8005B2F8u) + 12);
        rrj_write32(0x8005B41Cu, result);
    }
    return result;
}

uint32_t rrj_draft_80017B6C(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
{
    FUNCTION_MARKER(0x80017B6Cu, "SLUS_010.53");
    uint32_t result = 72u * a4 + rrj_read32(0x8005B40Cu) + 4u * (a2 == 0x6D);
    rrj_write32(result + 48, a2);
    rrj_write32(result + 56, a1);
    rrj_write32(result + 64, a3);
    return result;
}

uint32_t rrj_draft_8003E338(uint32_t a1)
{
    FUNCTION_MARKER(0x8003E338u, "SLUS_010.53");
    uint32_t owner, p, local, candidate, left, right, i;
    int32_t found = -1, count;
    if (!a1 || !q03_read16(rrj_host_context(), a1 + 320))
        return 0;
    owner = rrj_read32(a1 + 328);
    if (!owner || !rrj_read32(owner + 104))
        return 0;
    p = (uint32_t)sub_80039AFC(rrj_read32(owner));
    if (!p)
        return 0;
    local = rrj_draft_local_alloc(rrj_host_context(), 8);
    {
        uint32_t args[5] = {a1 + 184, p, local, local + 4, 0xFFFFFFFFu};
        found = (int32_t)(uint32_t)sub_8003EB58(args[0], args[1], args[2], args[3], args[4]);
    }
    if (found < 0)
    {
        rrj_draft_local_free(rrj_host_context(), local);
        return 0;
    }
    p = rrj_read32(rrj_read32(a1 + 328) + 104);
    count = (int16_t)q03_read16(rrj_host_context(), p + 2);
    if (count <= 0)
    {
        rrj_draft_local_free(rrj_host_context(), local);
        return 0;
    }
    left = rrj_read32(local);
    right = rrj_read32(local + 4);
    candidate = p + 4;
    left = (uint32_t)(int32_t)(int16_t)q03_read16(rrj_host_context(), left + 4);
    for (i = 0; (int32_t)i < count; ++i, candidate += 24)
    {
        uint32_t first = (uint32_t)(int32_t)(int16_t)q03_read16(rrj_host_context(), candidate);
        if (first == left && (int16_t)q03_read16(rrj_host_context(), candidate + 2) == (int16_t)q03_read16(rrj_host_context(), right + 4))
        {
            rrj_draft_local_free(rrj_host_context(), local);
            return candidate;
        }
        if ((int16_t)q03_read16(rrj_host_context(), candidate + 2) == (int32_t)left && first == (uint32_t)(int32_t)(int16_t)q03_read16(rrj_host_context(), right + 4))
        {
            rrj_draft_local_free(rrj_host_context(), local);
            return candidate;
        }
    }
    rrj_draft_local_free(rrj_host_context(), local);
    return 0;
}

uint32_t rrj_draft_8002DF14(uint32_t a1)
{
    FUNCTION_MARKER(0x8002DF14u, "SLUS_010.53");
    uint32_t magnitude = rrj_draft_8002E010(a1);
    uint32_t reciprocal, x, y;
    if (!magnitude)
        return 0;
    reciprocal = quick01_reciprocal(magnitude);
    x = (uint32_t)sub_8001FC90(reciprocal, rrj_read32(a1));
    y = rrj_read32(a1 + 4);
    rrj_write32(a1, x);
    rrj_write32(a1 + 4, (uint32_t)sub_8001FC90(reciprocal, y));
    return 1;
}

uint32_t rrj_draft_8002E010(uint32_t a1)
{
    FUNCTION_MARKER(0x8002E010u, "SLUS_010.53");
    int64_t x = (int32_t)rrj_read32(a1);
    int64_t y = (int32_t)rrj_read32(a1 + 4);
    uint32_t sum = (uint32_t)((x * x) >> 16) + (uint32_t)((y * y) >> 16);
    return (uint32_t)sub_8004CF74(sum) << 2;
}

uint32_t rrj_draft_8002DFC0(uint32_t a1, uint32_t a2)
{
    FUNCTION_MARKER(0x8002DFC0u, "SLUS_010.53");
    uint32_t first = (uint32_t)sub_8001FC90(rrj_read32(a1), rrj_read32(a2));
    uint32_t second = (uint32_t)sub_8001FC90(rrj_read32(a1 + 4), rrj_read32(a2 + 4));
    return first + second;
}

uint32_t rrj_draft_8001C298(void)
{
    FUNCTION_MARKER(0x8001C298u, "SLUS_010.53");
    uint32_t owner = rrj_read32(0x8005B470u), index = 0, result;
    result = q03_read8(rrj_host_context(), owner + 244);
    if (!result)
        return result;
    do
    {
        uint32_t p = rrj_read32(owner + 248 + index * 4);
        if (p)
            (uint32_t)sub_800144B8(p);
        owner = rrj_read32(0x8005B470u);
        result = q03_read8(rrj_host_context(), owner + 244);
        ++index;
        result = (uint32_t)((int32_t)index < (int32_t)result);
        /* The branch delay slot overwrites the comparison result */
        if (!result)
            return index * 4;
    } while (1);
}

uint32_t rrj_draft_8001BBC0(void)
{
    FUNCTION_MARKER(0x8001BBC0u, "SLUS_010.53");
    uint32_t local = rrj_draft_local_alloc(rrj_host_context(), 36), result, image;
    uint32_t args[3] = {0x80010AC8u, local + 32, local + 8};
    rrj_write32(local + 32, 0);
    result = rrj_draft_call(rrj_host_context(), 0x8001408Cu, args, 3);
    if (!result)
    {
        image = rrj_read32(local + 24);
        q03_write16(rrj_host_context(), local, 122);
        q03_write16(rrj_host_context(), local + 2, 200);
        q03_write16(rrj_host_context(), local + 4, 140);
        q03_write16(rrj_host_context(), local + 6, 20);
        q03_call2(rrj_host_context(), 0x80048A6Cu, local, image);
        image = rrj_read32(local + 24);
        q03_write16(rrj_host_context(), local + 2, 456);
        q03_call2(rrj_host_context(), 0x80048A6Cu, local, image);
    }
    result = q03_call1(rrj_host_context(), 0x800487C0u, 0);
    image = rrj_read32(local + 32);
    if (image)
        result = (uint32_t)sub_800144B8(image);
    rrj_draft_local_free(rrj_host_context(), local);
    return result;
}
