#include "draft_quick.h"

/* Unverified quick drafts for SLUS_010.53 */
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

static uint32_t quick01_call3(RRJMemory *m, uint32_t target, uint32_t a, uint32_t b, uint32_t c)
{
    uint32_t args[3] = {a, b, c};
    return rrj_draft_call(m, target, args, 3);
}

static uint32_t quick01_reciprocal(uint32_t value)
{
    uint32_t magnitude = (int32_t)value < 0 ? 0u - value : value;
    uint32_t denominator = (uint32_t)((int32_t)magnitude >> 1) + (uint32_t)((int32_t)(magnitude - 2u) >> 31);
    uint32_t quotient = denominator ? 0x80000000u / denominator : 0xffffffffu;
    return (int32_t)value < 0 ? 0u - quotient : quotient;
}

void rrj_draft_80014264(void)
{
}

uint32_t rrj_draft_800145EC(uint32_t a1, uint32_t a2)
{
    return quick01_call2(rrj_host_context(), 0x8001458Cu, a1, a2);
}

uint32_t rrj_draft_80014910(uint32_t a1, uint32_t a2)
{
    quick01_call2(rrj_host_context(), 0x80015C88u, a1, a2);
    return 0;
}

uint32_t rrj_draft_80015198(void)
{
    quick01_call1(rrj_host_context(), 0x80044894u, 0x80010938u);
    return 0xffffffffu;
}

uint32_t rrj_draft_800154FC(void)
{
    quick01_call1(rrj_host_context(), 0x800457FCu, 0);
    rrj_write32(0x800D6724u, 0xffffffffu);
    rrj_write32(0x800D6728u, 0);
    return 0x800D6710u;
}

uint32_t rrj_draft_8001556C(uint32_t a1)
{
    uint32_t buffer, frame, result;
    while (rrj_read32(0x800D6728u))
    {
    }
    /* TODO Integration must provide addressable local storage adapters */
    buffer = rrj_draft_local_alloc(rrj_host_context(), 8);
    frame = a1 + rrj_read32(0x8005B3D8u);
    rrj_write32(0x800D6720u, frame);
    quick01_call2(rrj_host_context(), 0x80015C88u, buffer, frame);
    quick01_call1(rrj_host_context(), 0x800457E8u, 0x8001578Cu);
    rrj_write32(0x800D6728u, 3);
    rrj_write32(0x800D6724u, 0xffffffffu);
    quick01_call2(rrj_host_context(), 0x800457A8u, 0, 0);
    result = quick01_call2(rrj_host_context(), 0x8004594Cu, 2, buffer);
    rrj_draft_local_free(rrj_host_context(), buffer);
    return result;
}

uint32_t rrj_draft_80015B60(void)
{
    uint32_t buffer = rrj_draft_local_alloc(rrj_host_context(), 8);
    uint32_t result;
    do
    {
        quick01_call3(rrj_host_context(), 0x80045810u, 1, buffer, 0);
    } while (quick01_call0(rrj_host_context(), 0x80045664u) & 0x10u);
    quick01_call2(rrj_host_context(), 0x800457A8u, 0, 0);
    rrj_draft_80015B04();
    rrj_draft_80015B04();
    rrj_draft_80015B04();
    rrj_draft_80015B04();
    rrj_draft_80015B04();
    rrj_draft_80015B04();
    rrj_draft_80015B04();
    do
    {
        quick01_call3(rrj_host_context(), 0x80045810u, 19, 0, buffer);
        result = quick01_call2(rrj_host_context(), 0x800457A8u, 0, buffer);
    } while (result != 2);
    rrj_draft_local_free(rrj_host_context(), buffer);
    return result;
}

uint32_t rrj_draft_800245B4(uint32_t incoming_result)
{
    uint32_t handle = rrj_read32(0x8005AE64u);
    uint32_t result = incoming_result;
    if (handle)
        result = quick01_call1(rrj_host_context(), 0x800144B8u, handle);
    rrj_write32(0x8005AE64u, 0);
    return result;
}

uint32_t rrj_draft_8002D298(void)
{
    uint32_t index;
    for (index = 0; index < 18; ++index)
    {
        uint32_t address = 0x800D80D8u + 252u + (index >> 3);
        w_u8(address, r_u8(address) & ~(1u << (index & 7)));
    }
    return 0;
}

uint32_t rrj_draft_8002E010(uint32_t a1)
{
    int64_t x = (int32_t)rrj_read32(a1);
    int64_t y = (int32_t)rrj_read32(a1 + 4);
    uint32_t sum = (uint32_t)((x * x) >> 16) + (uint32_t)((y * y) >> 16);
    return quick01_call1(rrj_host_context(), 0x8004CF74u, sum) << 2;
}

uint32_t rrj_draft_8002DF14(uint32_t a1)
{
    uint32_t magnitude = rrj_draft_8002E010(a1);
    uint32_t reciprocal, x, y;
    if (!magnitude)
        return 0;
    reciprocal = quick01_reciprocal(magnitude);
    x = quick01_call2(rrj_host_context(), 0x8001FC90u, reciprocal, rrj_read32(a1));
    y = rrj_read32(a1 + 4);
    rrj_write32(a1, x);
    rrj_write32(a1 + 4, quick01_call2(rrj_host_context(), 0x8001FC90u, reciprocal, y));
    return 1;
}

uint32_t rrj_draft_8002DFC0(uint32_t a1, uint32_t a2)
{
    uint32_t first = quick01_call2(rrj_host_context(), 0x8001FC90u, rrj_read32(a1), rrj_read32(a2));
    uint32_t second = quick01_call2(rrj_host_context(), 0x8001FC90u, rrj_read32(a1 + 4), rrj_read32(a2 + 4));
    return first + second;
}

uint32_t rrj_draft_8002F4D8(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t output1, uint32_t output2, uint32_t output3, uint32_t output4)
{
    uint32_t numerator = (int32_t)a5 <= 0 ? 0u - a5 : a5;
    uint32_t denominator = (int32_t)a4 <= 0 ? 0u - a4 : a4;
    uint32_t ratio = quick01_call2(rrj_host_context(), 0x80010028u, numerator, denominator);
    uint32_t reciprocal, first, second, third;
    if (((int32_t)a5 > 0) != ((int32_t)a4 > 0))
        ratio = 0u - ratio;
    reciprocal = quick01_reciprocal(ratio);
    first = quick01_call2(rrj_host_context(), 0x8001FC90u, a1, ratio);
    second = quick01_call2(rrj_host_context(), 0x8001FC90u, a2, reciprocal - ratio);
    third = quick01_call2(rrj_host_context(), 0x8001FC90u, a3, reciprocal);
    rrj_write32(output1, 3u * (first + second - third));
    rrj_write32(output2, a5);
    rrj_write32(output3, (a4 + a5) << 1);
    rrj_write32(output4, a4);
    return output4;
}

uint32_t rrj_draft_8003BA4C(uint32_t owner, uint32_t id, uint32_t output, uint32_t weights, uint32_t limit)
{
    uint32_t descriptor = quick01_call2(rrj_host_context(), 0x8003B9A8u, owner, 0);
    uint32_t object, metadata, links, count, index;
    uint32_t args[4];
    if (!descriptor)
        return 0;
    object = rrj_read32(descriptor + 12);
    if (!object)
        return 0;
    metadata = quick01_call1(rrj_host_context(), 0x80039B60u, rrj_read32(object));
    if (!metadata)
        return 0;
    links = rrj_draft_local_alloc(rrj_host_context(), 16);
    /* TODO The original local list has four entries and assumes a compatible limit */
    args[0] = metadata;
    args[1] = id;
    args[2] = links;
    args[3] = limit;
    count = rrj_draft_call(rrj_host_context(), 0x80039BB0u, args, 4);
    if ((int32_t)count < 0 || (int32_t)limit < (int32_t)count)
    {
        rrj_draft_local_free(rrj_host_context(), links);
        return 0;
    }
    for (index = 0; (int32_t)index < (int32_t)count; ++index)
    {
        uint32_t link = rrj_read32(links + 4u * index);
        uint32_t record = quick01_call1(rrj_host_context(), 0x80039C38u, link);
        uint32_t section, segment, vertex, weight;
        int32_t vertex_index, direction;
        if (!record)
        {
            rrj_draft_local_free(rrj_host_context(), links);
            return 0;
        }
        section = rrj_read32(object + 44) + ((uint32_t)(int32_t)r_s16(record + 2) << 5);
        segment = rrj_read32(object + 48) + 28u * ((uint32_t)(int32_t)r_s16(section + 20) + (uint32_t)(int32_t)r_s16(record + 4));
        vertex_index = r_s16(segment + 8);
        direction = r_s16(record + 8);
        if (r_s16(link + 6) > 0)
        {
            if (direction <= 0)
                vertex_index = (int32_t)((uint32_t)vertex_index - 1u + (uint32_t)(int32_t)r_s16(segment + 10));
            weight = (uint32_t)direction;
        }
        else
        {
            if (direction > 0)
                vertex_index = (int32_t)((uint32_t)vertex_index - 1u + (uint32_t)(int32_t)r_s16(segment + 10));
            weight = 0u - (uint32_t)direction;
        }
        vertex = rrj_read32(object + 52) + 52u * (uint32_t)vertex_index;
        rrj_write32(weights, weight);
        rrj_write32(output + 20, 0);
        rrj_write32(output + 16, 0);
        rrj_write32(output, object);
        rrj_write32(output + 4, section);
        rrj_write32(output + 8, segment);
        rrj_write32(output + 12, vertex);
        rrj_write32(output + 24, metadata);
        rrj_write32(output + 28, link);
        output += 32;
        weights += 4;
    }
    rrj_draft_local_free(rrj_host_context(), links);
    return count;
}

uint32_t rrj_draft_8003CB6C(uint32_t a1, uint32_t a2)
{
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

uint32_t rrj_draft_8003D844(uint32_t a1, uint32_t a2)
{
    uint32_t id = a1 & 0xffffu;
    uint32_t object, entry, owner, metadata, record, slot, buffer;
    int32_t selected;
    if ((int32_t)id >= r_s16(rrj_read32(0x8005B240u) + 40))
        return 2;
    metadata = quick01_call1(rrj_host_context(), 0x80039A08u, id);
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
            if (!quick01_call2(rrj_host_context(), 0x8003CFCCu, buffer, id))
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
                    uint32_t value = quick01_call1(rrj_host_context(), 0x80039F68u, item + input_offset);
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

uint32_t rrj_draft_8004071C(uint32_t a1, uint32_t index, uint32_t field, uint32_t getter_callback)
{
    uint32_t args[3] = {a1, index, field};
    /* TODO Getter ABI is carried in the incoming callback */
    uint32_t object = rrj_draft_indirect(rrj_host_context(), getter_callback, args, 3);
    uint32_t count = r_u8(object + 233);
    if ((int32_t)index < 0)
        return count;
    if ((int32_t)index >= (int32_t)count)
        return 0;
    if (field - 1u >= 5)
        return 0;
    return r_u8(rrj_read32(object + 4) + 5u * index + field - 1u);
}

uint32_t rrj_draft_80040D80(void)
{
    quick01_call0(rrj_host_context(), 0x80043DA4u);
    quick01_call2(rrj_host_context(), 0x80043E54u, 3, 1);
    quick01_call2(rrj_host_context(), 0x80043E44u, 2, 0x800D9CB0u);
    /* TODO Legacy void helper ABI leaves the observable return carrier unspecified */
    return quick01_call0(rrj_host_context(), 0x80043DB4u);
}

uint32_t rrj_draft_80044EA4(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
    uint32_t args[4] = {a0, a1, a2, a3};
    return rrj_draft_bios(rrj_host_context(), 0xB0u, 0x13u, args, 4);
}

uint32_t rrj_draft_80045234(void)
{
    uint32_t object, source;
    /* TODO Original global saved return address is omitted under the ordinary C draft ABI */
    quick01_call0(rrj_host_context(), 0x80043DA4u);
    object = rrj_draft_bios(rrj_host_context(), 0xB0u, 0x56u, 0, 0);
    object = rrj_read32(object + 24);
    for (source = 0x800452A4u; source != 0x800452B0u; source += 4)
    {
        rrj_write32(object + 112, rrj_read32(source));
        object += 4;
    }
    quick01_call0(rrj_host_context(), 0x80043D44u);
    /* TODO Legacy helper return carrier requires an integration adapter */
    return quick01_call0(rrj_host_context(), 0x80043DB4u);
}

uint32_t rrj_draft_80045BCC(void)
{
    /* TODO Legacy helper may require semantic inputs absent from this pseudocode signature */
    quick01_call0(rrj_host_context(), 0x80047248u);
    return 1;
}

uint32_t rrj_draft_80045C74(void)
{
    uint32_t result = quick01_call1(rrj_host_context(), 0x800457E8u, rrj_read32(0x80054ABCu));
    rrj_write32(0x80054AB8u, 0);
    return result;
}

uint32_t rrj_draft_8004B230(uint32_t ordering_table, uint32_t debug_level)
{
    uint32_t status, args[2];
    if (debug_level >= 2)
    {
        args[0] = 0x80011248u;
        args[1] = ordering_table;
        rrj_draft_indirect(rrj_host_context(), rrj_read32(0x80055F68u), args, 2);
    }
    status = quick01_call1(rrj_host_context(), 0x80047724u, 0xffffffffu);
    rrj_write32(0x800560A8u, status + 240);
    rrj_write32(0x800560ACu, 0);
    status = rrj_read32(rrj_read32(0x80056080u));
    for (;;)
    {
        if (!(status & 0x01000000u) && (rrj_read32(rrj_read32(0x80056074u)) & 0x04000000u))
            break;
        if (quick01_call0(rrj_host_context(), 0x8004AD28u))
            return 0xffffffffu;
        status = rrj_read32(rrj_read32(0x80056080u));
    }
    quick01_call2(rrj_host_context(), 0x80047994u, 2, 0x8004B324u);
    args[0] = ordering_table;
    rrj_draft_indirect(rrj_host_context(), rrj_read32(rrj_read32(0x80055F64u) + 24), args, 1);
    return 0;
}

uint32_t rrj_draft_8004EC5C(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t register_base)
{
    uint32_t control;
    /* TODO Register base arrives through a nonstandard semantic input */
    w_u16(register_base + 422, a2);
    quick01_call0(rrj_host_context(), 0x8004F230u);
    control = rrj_read32(0x8005A41Cu);
    w_u16(control + 426, r_u16(control + 426) | 0x30u);
    quick01_call0(rrj_host_context(), 0x8004F230u);
    quick01_call0(rrj_host_context(), 0x8004F208u);
    rrj_write32(rrj_read32(0x8005A420u), a1);
    rrj_write32(rrj_read32(0x8005A424u), (a3 << 16) | 0x10u);
    rrj_write32(0x8005A46Cu, 1);
    rrj_write32(rrj_read32(0x8005A428u), 0x01000200u);
    return 1;
}
