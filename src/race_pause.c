#include "race_bodyless_batch_007.h"
#include "race_pause_frontier.h"
#include "gpu.h"
/* Resident8002305C: refresh pause conditions; callees require native binding. */
#include "race_pause.h"
#include "native_game_api.h"
#include "menu_latch.h"
#include "fixed_math.h"
#include "wip.h"
#include "xport.h"
#include <stdlib.h>

uint32_t sub_80022C64(void)
{
    FUNCTION_MARKER(0x80022C64u, "SLUS_010.53");
    uint32_t elapsed;
    if (!r_u32(0x8005AE20))
        return 0;
    elapsed = r_u32(r_u32(0x8005B2F8) + 12) - r_u32(0x8005AE1C);
    if (((int32_t)(uint32_t)(elapsed)) < 0)
        elapsed = 0u - elapsed;
    if (((int32_t)(uint32_t)(elapsed)) < 241)
        return 0;
    return ((int32_t)(uint32_t)(elapsed)) < 3001 ? 1u : 2u;
}

uint32_t sub_80023870(void)
{
    FUNCTION_MARKER(0x80023870u, "SLUS_010.53");
    uint32_t result;
    result = sub_8008B99C(r_u32(0x8005B38C)) >> 31;
    result |= sub_8008B99C(0x800CD898) >> 31;
    if (r_u32(r_u32(0x8005B2F8) + 48) == 2)
    {
        result |= sub_8008B99C(r_u32(0x8005B21C)) >> 31;
        result |= sub_8008B99C(0x800CDD04) >> 31;
    }
    return result;
}

uint32_t sub_8002305C(void)
{
    FUNCTION_MARKER(0x8002305Cu, "SLUS_010.53");
    uint32_t reason, result;
    (void)sub_800237B8();
    (void)sub_80030608();
    if (r_u32(r_u32(0x8005B2F8) + 48) == 1)
        (void)sub_800247E8();
    reason = sub_80022C64();
    if (!reason && r_u8(r_u32(0x8005B2F8)) != 4)
        return 4;
    result = 2u * (sub_80023870() != 0) | (reason == 2);
    w_u16(r_u32(0x8005B2F8) + 40, result);
    return result;
}

/* 80030500: first matching object range, lower inclusive / upper exclusive. */
uint32_t sub_80030500(uint32_t tag, uint32_t position)
{
    FUNCTION_MARKER(0x80030500u, "SLUS_010.53");
    uint32_t base = r_u32(0x8005ACBC), count = r_u32(base + 0xA58);
    uint32_t i, entry = base + 44;
    for (i = 0; ((int32_t)(uint32_t)(i)) < ((int32_t)(uint32_t)(count)); ++i, entry += 36)
    {
        uint32_t object, kind, ranges, j;
        if ((r_u32(entry) & 0x12) != 0x12)
            continue;
        object = r_u32(entry + 20);
        kind = r_u32(object) >> 28;
        if (kind != 0 && kind != 8)
            continue;
        ranges = r_u32(entry + 24);
        for (j = 0; j < 4; ++j, ranges += 12)
        {
            uint32_t candidate = r_u32(ranges);
            if (candidate == 0xFFFFFFFF)
                break;
            if (tag == candidate && ((int32_t)(uint32_t)(position)) >= ((int32_t)(uint32_t)(r_u32(ranges + 4))) && ((int32_t)(uint32_t)(position)) < ((int32_t)(uint32_t)(r_u32(ranges + 8))))
                return r_u32(r_u32(entry + 20)) & 0x0FFFFFFF;
        }
    }
    return 0xFFFFFFFF;
}

uint32_t sub_800245DC(uint32_t index)
{
    FUNCTION_MARKER(0x800245DCu, "SLUS_010.53");
    return r_u32(r_u32(0x8005AE64) + 24) + (index << 4);
}

uint32_t sub_800245F4(uint32_t index)
{
    FUNCTION_MARKER(0x800245F4u, "SLUS_010.53");
    return r_u32(r_u32(0x8005AE64) + 20) + ((index + (index << 2)) << 3);
}

uint32_t sub_80030410(uint32_t input, uint32_t step)
{
    uint32_t scalar_value_1;
    uint32_t scalar_value_2;
    uint32_t scalar_value_3;

    FUNCTION_MARKER(0x80030410u, "SLUS_010.53");
    uint32_t tag = r_u32(input + 188), position;
    uint32_t reference = r_u16(0x800CDA00);
    if (!(tag >> 16))
    {
        position = ((scalar_value_1 = (r_u32(input + 196))), (((uint32_t)((!((uint32_t)(10))) ? (scalar_value_1) : ((scalar_value_1 >> ((uint32_t)(10))) | ((scalar_value_1 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(10)))) : 0))))));
        step = ((scalar_value_2 = (step << 16)), (((uint32_t)((!((uint32_t)(16))) ? (scalar_value_2) : ((scalar_value_2 >> ((uint32_t)(16))) | ((scalar_value_2 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(16)))) : 0))))));
        if (step && (tag & 0xFFFF) == reference)
        {
            uint32_t target = ((scalar_value_3 = (r_u32(0x800CDA08))), (((uint32_t)((!((uint32_t)(10))) ? (scalar_value_3) : ((scalar_value_3 >> ((uint32_t)(10))) | ((scalar_value_3 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(10)))) : 0)))))), adjusted;
            if (((int32_t)(uint32_t)(position)) < ((int32_t)(uint32_t)(target)))
            {
                adjusted = position + step;
                if (((int32_t)(uint32_t)(target)) < ((int32_t)(uint32_t)(adjusted)))
                    adjusted = target;
            }
            else
            {
                adjusted = position - step;
                if (((int32_t)(uint32_t)(adjusted)) < ((int32_t)(uint32_t)(target)))
                    adjusted = target;
            }
            position = adjusted;
        }
        tag = r_u16(input + 188);
    }
    else
    {
        uint32_t first = sub_800245F4(tag & 0xFFFF), second;
        tag = r_u32(first + 8);
        second = sub_800245DC(tag);
        position = ((int32_t)(uint32_t)(r_u32(first + 12))) > 0 ? 1u : r_u32(second + 4) - 1;
    }
    return sub_80030500(tag, position);
}

uint32_t sub_8008B99C(uint32_t player)
{
    uint32_t scalar_value_4;
    uint32_t scalar_value_5;

    FUNCTION_MARKER(0x8008B99Cu, "RASHCDG.BIN");
    uint32_t type = r_u16(player + 172), value = 0, result;
    if ((type >> 5) != 6 && ((type >> 5) != 4 || (type & 31) < 30))
    {
        uint32_t shift = r_u16(r_u32(player) + 14) >> 12;
        value = ((scalar_value_4 = (r_u32(player + 40))), (((uint32_t)((!((uint32_t)(shift))) ? (scalar_value_4) : ((scalar_value_4 >> ((uint32_t)(shift))) | ((scalar_value_4 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(shift)))) : 0))))));
    }
    result = sub_80030410(player + 172, ((scalar_value_5 = (value << 16)), (((uint32_t)((!((uint32_t)(16))) ? (scalar_value_5) : ((scalar_value_5 >> ((uint32_t)(16))) | ((scalar_value_5 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(16)))) : 0)))))));
    w_u32(player + 176, result);
    return result;
}

uint32_t sub_8002379C(uint32_t index)
{
    FUNCTION_MARKER(0x8002379Cu, "SLUS_010.53");
    w_u32(0x8005AE34, 0x80053478 + (index << 7));
    return 0x80053478;
}

uint32_t sub_80023860(uint32_t value)
{
    FUNCTION_MARKER(0x80023860u, "SLUS_010.53");
    return value;
}

uint32_t sub_80023D7C(void)
{
    FUNCTION_MARKER(0x80023D7Cu, "SLUS_010.53");
    return sub_80030E58(r_u32(r_u32(0x8005AE34)));
}

uint32_t sub_80023C4C(void)
{
    FUNCTION_MARKER(0x80023C4Cu, "SLUS_010.53");
    uint32_t result, record;
    result = sub_800312D0(r_u32(r_u32(0x8005AE34)));
    record = r_u32(0x8005AE34);
    if (result && !(r_u32(record + 28) & 2))
    {
        w_u32(record + 108, 1);
        return 1;
    }
    w_u32(record + 108, 0);
    return record;
}

uint32_t sub_800237B8(void)
{
    FUNCTION_MARKER(0x800237B8u, "SLUS_010.53");
    uint32_t index = 0;
    while (index < r_u32(r_u32(0x8005B2F8) + 48))
    {
        uint32_t record;
        (void)sub_8002379C(index);
        (void)sub_80023A14();
        record = r_u32(0x8005AE34);
        if (r_u32(record + 44))
            (void)sub_80031E1C(r_u32(record + 72), 0, index);
        ++index;
        (void)sub_80023FBC();
        (void)sub_80023D7C();
        (void)sub_80023C4C();
    }
    return sub_80023CAC();
}

/* 80030608: queued object classification, pending dispatch and record sweep. */
uint32_t sub_80030608(void)
{
    FUNCTION_MARKER(0x80030608u, "SLUS_010.53");
    uint32_t pending[64], count = 0, object, i, record;
    while ((object = sub_80031D70()) != 0)
    {
        uint32_t classification = sub_80031BF8(object, r_u8(object + 34));
        if (((int32_t)(uint32_t)(classification)) < 0)
        {
            /* Original has 64 stack slots; overflow corrupts its saved registers. */
            if (count >= 64)
                abort();
            pending[count++] = object;
        }
        else
        {
            uint32_t value = r_u32(r_u32(object + 20) + 28) >> 1;
            uint32_t saved = r_u32(object + 32), target;
            (void)sub_800313EC(object);
            record = r_u32(0x8005ACBC) + 44 + 36 * classification;
            value = sub_80023860(value);
            target = r_u32(record + 20);
            w_u32(target + 28, (r_u32(target + 28) & 1) | (value << 1));
            w_u32(record + 32, saved);
        }
    }
    for (i = 0; i < count; ++i)
    {
        uint32_t kind;
        object = pending[i];
        if (r_u32(object) & 0x20)
            continue;
        kind = r_u32(r_u32(object + 20)) >> 28;
        if (kind == 1 || kind == 2)
            (void)(kind == 1 ? sub_800319F8(object, r_u8(object + 34)) : sub_80031B4C(object));
        else
            w_u32(object, r_u32(object) | 0x20);
    }
    record = r_u32(0x8005ACBC) + 44;
    for (i = 0; ((int32_t)(uint32_t)(i)) < ((int32_t)(uint32_t)(r_u32(r_u32(0x8005ACBC) + 0xA58))); ++i, record += 36)
    {
        if ((r_u32(record) & 0x22) == 0x20)
            (void)sub_80031604(record, r_u8(record + 34));
    }
    (void)sub_800320CC(5);
    if (!r_u32(0x8005AED8))
        return 0;
    w_u32(0x8005AED8, 0);
    return sub_800320AC();
}

uint32_t sub_80031D70(void)
{
    FUNCTION_MARKER(0x80031D70u, "SLUS_010.53");
    uint32_t base, object = 0;
    (void)sub_80043DA4();
    base = r_u32(0x8005ACBC);
    if (r_u32(base + 0x934))
    {
        uint32_t index, next, rounded, count;
        index = r_u32(base + 0x930);
        object = r_u32(base + 0x938 + (index << 2));
        w_u32(object, r_u32(object) & 0xFFFFFF7F);
        index = r_u32(base + 0x930);
        next = index + 1;
        rounded = ((int32_t)(uint32_t)(next)) < 0 ? index + 64 : next;
        rounded = ((uint32_t)((!((uint32_t)(6))) ? (((uint32_t)(rounded))) : ((((uint32_t)(rounded)) >> ((uint32_t)(6))) | ((((uint32_t)(rounded)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(6)))) : 0)))) << 6;
        count = r_u32(base + 0x934);
        w_u32(base + 0x930, next - rounded);
        w_u32(base + 0x934, count - 1);
    }
    (void)sub_80043DB4();
    return object;
}

uint32_t sub_80031BF8(uint32_t object, uint32_t group)
{
    FUNCTION_MARKER(0x80031BF8u, "SLUS_010.53");
    uint32_t base = r_u32(0x8005ACBC), descriptor = r_u32(object + 20);
    uint32_t word = r_u32(descriptor), index = r_u32(base + (group << 3) + 0xA40);
    uint32_t record = base + 44 + 36 * index, bit = r_u32(descriptor + 28) & 1;
    uint32_t last = r_u32(base + (group << 3) + 0xA44);
    while (index <= last)
    {
        if (record != object && (r_u32(record) & 0x91) == 0x11)
        {
            uint32_t other = r_u32(record + 20);
            if (r_u32(other) == word && ((word >> 28) != 1 || (r_u32(other + 28) & 1) == bit))
                return index;
        }
        ++index;
        record += 36;
    }
    return 0xFFFFFFFF;
}

uint32_t sub_800320CC(uint32_t mode)
{
    FUNCTION_MARKER(0x800320CCu, "SLUS_010.53");
    uint32_t offset;
    switch (mode)
    {
        case 1:
            offset = 16;
            break;
        case 2:
            offset = 8;
            break;
        case 3:
            offset = 20;
            break;
        case 4:
            offset = 4;
            break;
        case 7:
            offset = 28;
            break;
        case 8:
            offset = 24;
            break;
        default:
            return 0xFFFFFFFF;
    }
    return r_u32(r_u32(0x8005ACBC) + offset);
}

uint32_t sub_800320AC(void)
{
    FUNCTION_MARKER(0x800320ACu, "SLUS_010.53");
    return sub_800320CC(6);
}

uint32_t sub_80031B98(uint32_t left, uint32_t right)
{
    FUNCTION_MARKER(0x80031B98u, "SLUS_010.53");
    uint32_t i;
    for (i = 0; i < 32; ++i)
    {
        uint8_t a = r_u8(left + i), b = r_u8(right + i);
        w_u8(left + i, b);
        w_u8(right + i, a);
    }
    return 0;
}

uint32_t sub_80031B4C(uint32_t object)
{
    FUNCTION_MARKER(0x80031B4Cu, "SLUS_010.53");
    uint32_t base = r_u32(object + 16), target = base + 16352, result;
    (void)sub_80031B98(base, target);
    result = r_u32(object);
    w_u32(object + 20, target);
    result |= 0x20;
    w_u32(object, result);
    return result;
}

uint32_t sub_800319F8(uint32_t object, uint32_t group)
{
    FUNCTION_MARKER(0x800319F8u, "SLUS_010.53");
    uint32_t base, index, last, record, descriptor, word, bit, found = 0, left, right, data, result;
    result = r_u32(object) & 0x20;
    if (result)
        return result;
    base = r_u32(0x8005ACBC);
    index = r_u32(base + (group << 3) + 0xA40);
    record = base + 44 + 36 * index;
    descriptor = r_u32(object + 20);
    last = r_u32(base + (group << 3) + 0xA44);
    bit = r_u32(descriptor + 28) & 1;
    word = r_u32(descriptor);
    while (index <= last)
    {
        if (record != object && (r_u32(record) & 0x91) == 0x11 && r_u32(r_u32(record + 20)) == word)
        {
            found = record;
            break;
        }
        ++index;
        record += 36;
    }
    if (!found)
        return 1;
    left = bit ? found : object;
    right = bit ? object : found;
    (void)sub_80031B98(r_u32(left + 16), r_u32(right + 16) + 16224);
    data = r_u32(right + 16);
    (void)sub_80031B98(data, data + 16352);
    w_u32(left + 20, r_u32(right + 16) + 16224);
    w_u32(right + 20, r_u32(right + 16) + 16352);
    w_u32(left, r_u32(left) | 0x20);
    result = r_u32(right) | 0x20;
    w_u32(right, result);
    return result;
}

uint32_t sub_80032190(uint32_t from, uint32_t to)
{
    static const uint32_t offsets[4] = {16, 0, 20, 12};
    uint32_t address, result;
    FUNCTION_MARKER(0x80032190, "SLUS_010.53");
    if (from < 4)
    {
        address = r_u32(0x8005ACBC) + offsets[from];
        w_u32(address, r_u32(address) - 1);
    }
    if (to >= 4)
        return 3;
    address = r_u32(0x8005ACBC) + offsets[to];
    result = r_u32(address) + 1;
    w_u32(address, result);
    return result;
}

uint32_t sub_800322D0(uint32_t from, uint32_t to)
{
    FUNCTION_MARKER(0x800322D0u, "SLUS_010.53");
    uint32_t result;
    if (!r_u32(0x800541D0u))
    {
        sub_80043DA4();
    }
    (void)sub_80032190(from, to);
    result = r_u32(0x800541D0u);
    if (!result)
    {
        sub_80043DB4();
    }
    return result;
}

uint32_t sub_800313EC(uint32_t object)
{
    FUNCTION_MARKER(0x800313ECu, "SLUS_010.53");
    uint32_t flags = r_u32(object), retained = (flags >> 12) & 15, original_active = flags & 1;
    uint32_t result, address, offset;
    if (original_active)
    {
        w_u32(object, flags & 0xFFFFFFDC);
        (void)sub_800322D0(1, 0);
    }
    else
        (void)sub_800322D0(2, 0);
    flags = r_u32(object);
    result = flags & 8;
    if (flags & 16)
    {
        offset = original_active ? 4u : 24u;
        address = r_u32(0x8005ACBC) + offset;
        result = r_u32(address) - 1;
        w_u32(address, result);
    }
    else if (result)
    {
        offset = original_active ? 8u : 28u;
        address = r_u32(0x8005ACBC) + offset;
        result = r_u32(address) - 1;
        w_u32(address, result);
    }
    if (retained)
    {
        w_u32(object, (r_u32(object) - 4096) | 4);
        return sub_800322D0(0, 2);
    }
    w_u32(object + 8, 0);
    w_u32(object, 0);
    w_u32(object + 16, 0);
    w_u32(object + 20, 0);
    return result;
}

uint32_t sub_80031604(uint32_t object, uint32_t group)
{
    uint32_t flags = r_u32(object), buffer = r_u32(object + 16);
    uint32_t id = 0, kind = 0, status, descriptor, extra, value, result, target;
    FUNCTION_MARKER(0x80031604, "SLUS_010.53");
    if (flags & 8)
    {
        (void)sub_800248E4(r_u32(object + 4), buffer);
        status = 1;
    }
    else
    {
        descriptor = r_u32(object + 20);
        extra = r_u32(object + 24);
        value = sub_80023860(r_u32(descriptor + 28) >> 1) << 1;
        id = r_u32(descriptor);
        kind = id >> 28;
        id &= 0x0FFFFFFF;
        value |= r_u32(descriptor + 28) & 1;
        w_u32(descriptor + 28, value);
        switch (kind)
        {
            case 0:
            case 8:
                (void)sub_80031E1C(r_u32(r_u32(0x8005AE34) + 72), object, group);
                status = sub_80032A20(buffer, id, kind, extra, object + 28, group);
                break;
            case 9:
                status = sub_80032A20(buffer, id, kind, 0, 0, group);
                break;
            case 1:
                status = (r_u32(object) & 0x20) ? sub_80032CC8(buffer, id, value & 1, value, descriptor + 4, group) : 0;
                break;
            case 2:
                status = (r_u32(object) & 0x20) ? sub_80032C6C(buffer, id, value, descriptor + 4, group) : 0;
                break;
            case 3:
                status = sub_8003CEC4(buffer, id & 0xFFFF, group);
                if (status == 3)
                    status = 1;
                break;
            case 4:
                status = r_u32(r_u32(0x8005B2F8) + 48) == 1 ? sub_800136BC(buffer, descriptor + 4, r_u32(descriptor)) : 2;
                break;
            case 10:
                status = sub_8001A0C0(buffer, id, r_u32(object + 4));
                break;
            default:
                status = 2;
                break;
        }
    }
    if (status != 1)
    {
        if (!status)
            return 2;
        if (status == 2 || status == 3)
            return sub_800313EC(object);
        return 3;
    }
    flags = r_u32(object);
    result = flags | 2;
    if (!flags)
        return result;
    w_u32(object, result);
    result &= 16;
    if (!result)
        return result;
    (void)sub_80023960(r_u32(object + 20), group);
    if (kind != 0 && kind != 8)
        return 8;
    target = r_u32(0x800D6184);
    result = r_u32(target + 16);
    if (id != result)
    {
        target = r_u32(0x800D6188);
        result = r_u32(target + 16);
        if (id != result)
            return result;
    }
    w_u32(0x8005B310, target);
    return result;
}

uint32_t sub_80022218(uint32_t id, uint32_t group)
{
    FUNCTION_MARKER(0x80022218u, "SLUS_010.53");
    uint32_t index = 24 * group, end = index + 24, record = 0x800D9268 + index * 48;
    while (((int32_t)(uint32_t)(index)) < ((int32_t)(uint32_t)(end)))
    {
        if (r_u32(record + 8) == id)
            return index;
        ++index;
        record += 48;
    }
    return 0xFFFFFFFF;
}

uint32_t sub_8002227C(uint32_t channel, uint32_t id, uint32_t pixels0, uint32_t pixels1, uint32_t group)
{
    FUNCTION_MARKER(0x8002227Cu, "SLUS_010.53");
    uint32_t count = r_u32(r_u32(0x8005B2F8) + 48), kind = id >> 15;
    uint32_t index = sub_80022218(id, group), slot, config, bits, parity, result;
    uint8_t rect[8];
    if (index == 0xFFFFFFFF)
        index = sub_80022218(0xFFFFFFFF, group);
    slot = 0x800D9268 + 48 * index;
    w_u32(slot + 8, id);
    if (kind > 1)
        return 1;
    config = 0x800533B4 + 88 * (count - 1) + 4 * (group + (kind ? 2 : 0));
    bits = r_u8(config + 2);
    xport_store_le16(rect + 4, 64);
    xport_store_le16(rect + 6, 128);
    if (!kind)
    {
        xport_store_le16(rect, ((channel & 15) + (bits & 15)) << 6);
        xport_store_le16(rect + 2, 0);
        (void)rrj_gpu_upload(rect, pixels0);
        xport_store_le16(rect + 2, xport_load_le16(rect + 2) + 128);
        result = rrj_gpu_upload(rect, pixels1);
    }
    else
    {
        parity = (channel & 1) << 7;
        xport_store_le16(rect, ((bits & 15) << 6) + ((channel << 5) & 0x3C0));
        xport_store_le16(rect + 2, r_u8(config + 1) + ((bits & 16) << 4) + parity);
        (void)rrj_gpu_upload(rect, pixels0);
        result = 0x800D76D0 + 32 * group + 8 * channel;
        w_u8(result + 6, 0);
        w_u8(result + 7, (uint8_t)parity);
    }
    w_u32(slot, channel);
    return result;
}

uint32_t sub_80031008(uint32_t buffer)
{
    FUNCTION_MARKER(0x80031008u, "SLUS_010.53");
    uint32_t base = r_u32(0x8005ACBC), count = r_u32(base + 0xA58);
    uint32_t record = base + 44, index;
    for (index = 0; ((int32_t)(uint32_t)(index)) < ((int32_t)(uint32_t)(count)); ++index, record += 36)
        if (r_u32(record + 16) == buffer)
            break;
    /* The original cleans the following record when no match exists. */
    return sub_800313EC(record);
}

uint32_t sub_800333F4(uint32_t mode, uint32_t group)
{
    FUNCTION_MARKER(0x800333F4u, "SLUS_010.53");
    uint32_t index = 12 * group, end = index + 12, record = 0x800D87E8 + 1344 * group;
    while (((int32_t)(uint32_t)(index)) < ((int32_t)(uint32_t)(end)))
    {
        (void)sub_80033F14(record, mode, group);
        ++index;
        record += 112;
    }
    return 0;
}

uint32_t sub_80034DEC(uint32_t id, uint32_t buffer, uint32_t mode, uint32_t side, uint32_t value, uint32_t data, uint32_t group)
{
    FUNCTION_MARKER(0x80034DECu, "SLUS_010.53");
    uint32_t index = sub_80022218(id, group), record, found;
    if (index == 0xFFFFFFFF)
        index = sub_80022218(0xFFFFFFFF, group);
    else
    {
        record = 0x800D9268 + index * 48;
        if (!mode && ((!side && r_u32(record + 12)) || (side == 1 && r_u32(record + 16))))
            return 3;
        if (mode == 1 && r_u32(record + 12))
            return 3;
        if (r_u32(record) != 0xFFFFFFFF)
            return 3;
    }
    record = 0x800D9268 + index * 48;
    w_u32(record + 8, id);
    if (!mode)
    {
        w_u32(record + 4, 0);
        w_u32(record + (side ? 16 : 12), buffer);
    }
    else
    {
        w_u32(record + 12, buffer);
        w_u32(record + 4, mode);
    }
    w_u32(record + 20, value);
    (void)sub_8001E0B4(xport_guest_ptr(record + 24, 24), xport_guest_ptr(data, 1), 24);
    (void)sub_800324CC(record, mode, group);
    if (mode != 1 && (mode || !r_u32(record + 12) || !r_u32(record + 16)))
        return 1;
    found = sub_800335B4(mode, id, group);
    if (((int32_t)(uint32_t)(found)) < 0)
    {
        found = sub_800335B4(mode, 0xFFFFFFFF, group);
        if (found != 0xFFFFFFFF)
            (void)sub_80033B50(found, index, mode, group);
    }
    return 1;
}

uint32_t sub_80032C6C(uint32_t buffer, uint32_t id, uint32_t value, uint32_t data, uint32_t group)
{
    FUNCTION_MARKER(0x80032C6Cu, "SLUS_010.53");
    return sub_80034DEC(((uint32_t)(((((uint32_t)(id)) >> 16) & 0x8000) | ((((uint32_t)(id)) >> 13) & 0x7C00) | (((uint32_t)(id)) & 0x3FF))) | 0x8000, buffer, 1, 0, value, data, group);
}

uint32_t sub_80032CC8(uint32_t buffer, uint32_t id, uint32_t side, uint32_t value, uint32_t data, uint32_t group)
{
    FUNCTION_MARKER(0x80032CC8u, "SLUS_010.53");
    return sub_80034DEC(((uint32_t)(((((uint32_t)(id)) >> 16) & 0x8000) | ((((uint32_t)(id)) >> 13) & 0x7C00) | (((uint32_t)(id)) & 0x3FF))), buffer, 0, side != 0, value, data, group);
}

uint32_t sub_800324CC(uint32_t slot, uint32_t mode, uint32_t group)
{
    FUNCTION_MARKER(0x800324CCu, "SLUS_010.53");
    uint32_t index = 12 * group, end = index + 12, record = 0x800D87E8 + index * 112, j;
    if (((int32_t)(uint32_t)(index)) >= ((int32_t)(uint32_t)(end)))
        return 0x800E0000;
    do
    {
        for (j = 0; j < 2; ++j)
        {
            if (r_u32(record + (mode ? 88 : 80) + 4 * j) == r_u32(slot + 8))
                w_u32(record + (mode ? 96 : 104) + 4 * j, slot);
        }
        ++index;
        record += 112;
    } while (((int32_t)(uint32_t)(index)) < ((int32_t)(uint32_t)(end)));
    return 0;
}

uint32_t sub_800335B4(uint32_t mode, uint32_t id, uint32_t group)
{
    FUNCTION_MARKER(0x800335B4u, "SLUS_010.53");
    uint32_t count = r_u32(mode ? 0x8005AEE8 : 0x8005AEE4), index;
    uint32_t record = mode ? 0x800D76D0 + 32 * group : 0x800D7710 + 24 * group;
    for (index = 0; ((int32_t)(uint32_t)(index)) < ((int32_t)(uint32_t)(count)); ++index, record += 8)
        if (r_u32(record) == id)
            return index;
    return 0xFFFFFFFF;
}

uint32_t sub_80033B50(uint32_t channel, uint32_t index, uint32_t mode, uint32_t group)
{
    FUNCTION_MARKER(0x80033B50u, "SLUS_010.53");
    uint32_t slot = 0x800D9268 + 48 * index, record, target, id, value, first, second;
    if (!r_u32(slot + 12))
        return 1;
    if (r_u32(slot + 16) && !mode)
    {
        id = r_u32(slot + 8);
        first = r_u32(slot + 12);
        second = r_u32(slot + 16);
        (void)sub_8002227C(channel, id, first, second, group);
        (void)sub_80031008(r_u32(slot + 12));
        (void)sub_80031008(r_u32(slot + 16));
        record = 0x800D7710 + 24 * group + 8 * channel;
        w_u32(slot + 12, 0);
        w_u32(slot + 16, 0);
        w_u32(slot, channel);
        target = 0x800D8768 + 4 * (r_u16(record + 4) & 31);
        w_u32(target, r_u32(slot + 8));
        w_u32(record, r_u32(slot + 8));
        (void)sub_800333F4(0, group);
    }
    else if (r_u32(slot + 12) && mode == 1)
    {
        id = r_u32(slot + 8);
        first = r_u32(slot + 12);
        second = r_u32(slot + 16);
        (void)sub_8002227C(channel, id, first, second, group);
        (void)sub_80031008(r_u32(slot + 12));
        w_u32(slot + 12, 0);
        w_u32(slot + 16, 0);
        w_u32(slot, channel);
        record = 0x800D76D0 + 32 * group + 8 * channel;
        target = 0x800D8768 + 4 * (r_u16(record + 4) & 31);
        id = r_u32(slot + 8);
        if (channel & 1)
            w_u32(target, (r_u32(target) & 0xFFFF0000) | id);
        else
        {
            value = r_u16(target) | (id << 16);
            w_u32(target, value);
            w_u16(record + 4, r_u16(record + 4) | 0x800);
        }
        w_u32(record, r_u32(slot + 8));
        (void)sub_800333F4(1, group);
    }
    return 1;
}

/* 33F14 local material/lookup helpers retain C stack storage, not PSX addresses. */

static void material_special(uint8_t material[9])
{
    uint32_t count, cfg, divisor, quotient, remainder, bits, x, y;
    material[8] = (uint8_t)r_u8(0x800D6168);
    xport_store_le16(material + 6, r_u16(0x800D6160));
    count = r_u32(r_u32(0x8005B2F8) + 48);
    cfg = 0x80053254 + 176 * (count - 1);
    divisor = (uint32_t)r_u8((cfg + 68));
    quotient = divisor ? material[2] / divisor : 0xFFFFFFFF;
    remainder = divisor ? material[2] % divisor : material[2];
    bits = (uint32_t)r_u8((0x800533D6 + 88 * (count - 1)));
    x = ((bits & 15) << 6) + (uint32_t)r_s16((cfg + 64)) + remainder * (uint32_t)r_u8((cfg + 70));
    y = ((bits & 16) << 4) + (uint32_t)r_s16((cfg + 66)) + quotient;
    /* 8004CD84 / PsyQ getClut: arithmetic shift low six bits are identical. */
    xport_store_le16(material + 4, (y << 6) | ((x >> 4) & 63));
}

static void material_add_uv(uint32_t polygon, uint8_t offset, unsigned quad)
{
    static const unsigned positions[4] = {5, 9, 13, 15};
    unsigned i;
    for (i = 0; i < 3 + quad; ++i)
    {
        uint32_t address = polygon + positions[i];
        w_u8(address, r_u8(address) + offset);
    }
}

uint32_t sub_80033F14(uint32_t record, uint32_t mode, uint32_t group)
{
    FUNCTION_MARKER(0x80033F14u, "SLUS_010.53");
    uint32_t flags, result, first, second, model, polygon, index, table_offset, count, ids[3], pointers[3];
    uint8_t material[9] = {0};
    unsigned j, quad;
    if (r_u32(record) == 0xFFFFFFFF && r_u32(record + 8) == 0xFFFFFFFF)
        return 0xFFFFFFFF;
    flags = r_u32(record + 12);
    if (!(flags & 16) && (flags & 3))
        w_u32(record + 12, flags | 16);
    result = r_u32(record + 12) & (mode ? 8u : 4u);
    if (result)
        return result;
    first = r_u32(record + (mode ? 96 : 104));
    if (!first)
        return 0;
    result = r_u32(first);
    if (((int32_t)(uint32_t)(result)) < 0)
        return result;
    second = r_u32(record + (mode ? 100 : 108));
    if (second)
    {
        result = r_u32(second);
        if (((int32_t)(uint32_t)(result)) < 0)
            return result;
    }
    else if (r_u32(record + (mode ? 92 : 84)) != 0xFFFFFFFF)
        return 0xFFFFFFFF;
    model = r_u32(record + 4);
    if (!mode)
    {
        result = r_u32(model + 60);
        if (!result)
            return result;
    }
    for (j = 0; j < 2; ++j)
    {
        pointers[j] = r_u32(record + (mode ? 96 : 104) + 4 * j);
        ids[j] = r_u32(record + (mode ? 88 : 80) + 4 * j);
    }
    ids[2] = 0xFFFFFFFF;
    pointers[2] = 0;
    sub_80033DAC(ids, pointers);
    w_u32(record + 12, r_u32(record + 12) | (mode ? 8u : 4u));
    model = r_u32(record + 4);
    polygon = r_u32(model + (mode ? 56 : 60));
    index = mode ? 0 : r_u16(model + 4) + r_u16(model + 6);
    count = r_u16(model + 4) + (mode ? 1u : 2u) * r_u16(model + 6);
    result = mode ? count : 2u * r_u16(model + 6);
    table_offset = 12 * index;
    while (((int32_t)(uint32_t)(index)) < ((int32_t)(uint32_t)(count)))
    {
        for (quad = 0; quad < 2; ++quad)
        {
            uint32_t remaining = r_u32(r_u32(model + 40) + table_offset + 4 + 4 * quad);
            while (((int32_t)(uint32_t)(remaining)) > 0)
            {
                uint32_t special;
                xport_store_le16(material, r_u16(polygon + 10));
                material[2] = (uint8_t)r_u8((polygon + 1));
                special = ((xport_load_le16(material) >> 10) & 31) == 30;
                if (special)
                    material_special(material);
                else if (mode)
                    sub_8002289C(material, ids, pointers, group);
                else
                    sub_80022758(material, ids, pointers, group);
                if (!mode && special)
                    material_add_uv(polygon, material[8], quad);
                w_u16(polygon + 10, xport_load_le16(material + 6));
                w_u16(polygon + 6, xport_load_le16(material + 4));
                if (mode)
                {
                    if (sub_80033EA0(record + 4, polygon, material))
                        material_add_uv(polygon, material[8], quad);
                }
                else
                {
                    (void)sub_80033E7C(polygon);
                    (void)sub_80033E5C(polygon);
                }
                --remaining;
                polygon += quad ? 24 : 20;
            }
            model = r_u32(record + 4);
        }
        ++index;
        count = r_u16(model + 4) + (mode ? 1u : 2u) * r_u16(model + 6);
        result = ((int32_t)(uint32_t)(index)) < ((int32_t)(uint32_t)(count));
        table_offset += 12;
    }
    if (mode)
    {
        w_u32(record + 12, r_u32(record + 12) | 32);
        result = r_u32(record + 12) | 64;
        w_u32(record + 12, result);
    }
    return result;
}

uint32_t sub_80031E1C(uint32_t track, uint32_t object, uint32_t group)
{
    uint32_t scalar_value_7;

    FUNCTION_MARKER(0x80031E1Cu, "SLUS_010.53");
    uint32_t index = 0, end = 0, info;
    if (!object)
    {
        uint32_t manager = r_u32(0x8005ACBC);
        index = r_u32(manager + 8 * group + 0xA40);
        end = r_u32(manager + 8 * group + 0xA44);
        object = manager + 44 + 36 * index;
    }
    info = sub_800245DC(track);
    while (index <= end)
    {
        if ((r_u32(object) & 17) == 17)
        {
            uint32_t kind = r_u32(r_u32(object + 20)) >> 28;
            if (!kind || kind == 8)
            {
                uint32_t entries = r_u32(object + 24), i, entry = entries, value;
                for (i = 0; i < 4; ++i, entry += 12)
                    if (r_u32(entry) == track)
                        break;
                if (i < 4)
                    value = ((scalar_value_7 = (r_u32(entry + 4))), (((uint32_t)((!((uint32_t)(6))) ? (scalar_value_7) : ((scalar_value_7 >> ((uint32_t)(6))) | ((scalar_value_7 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(6)))) : 0))))));
                else
                {
                    uint32_t first = r_u32(entries), first_info = sub_800245DC(first), side;
                    value = 0xFFFF0000;
                    for (side = 0; side < 2; ++side)
                    {
                        uint32_t table = sub_800245F4(r_u32(info + 8 + 4 * side));
                        uint32_t count = r_u32(table + 4), j;
                        for (j = 0; j < count; ++j)
                            if (r_u32(table + 8 + 8 * j) == first)
                                break;
                        if (j < r_u32(table + 4))
                        {
                            value = ((int32_t)(uint32_t)(r_u32(table + 12 + 8 * j))) > 0 ? r_u32(entries + 4) : r_u32(first_info + 4) - r_u32(entries + 8);
                            value = ((uint32_t)((!((uint32_t)(6))) ? (((uint32_t)(value))) : ((((uint32_t)(value)) >> ((uint32_t)(6))) | ((((uint32_t)(value)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(6)))) : 0))));
                            value = side ? value + 0x10000 : 0u - value;
                            break;
                        }
                    }
                }
                w_u32(object + 28, value);
            }
        }
        ++index;
        object += 36;
    }
    return 1;
}

uint32_t sub_800329BC(uint32_t id, uint32_t group)
{
    FUNCTION_MARKER(0x800329BCu, "SLUS_010.53");
    uint32_t i = 12 * group, end = i + 12, p = 0x800D87E8 + 1344 * group;
    for (; ((int32_t)(uint32_t)(i)) < ((int32_t)(uint32_t)(end)); ++i, p += 112)
        if (r_u32(p) == id)
            return i;
    return 0xFFFFFFFF;
}

uint32_t sub_80032338(uint32_t model_pointer, uint32_t buffer)
{
    FUNCTION_MARKER(0x80032338u, "SLUS_010.53");
    w_u32(r_u32(model_pointer) + 60, buffer);
    return 0;
}

uint32_t sub_8003234C(uint32_t record, uint32_t buffer)
{
    FUNCTION_MARKER(0x8003234Cu, "SLUS_010.53");
    static const uint32_t fields[8] = {36, 32, 40, 44, 48, 52, 56, 60};
    uint32_t limit = 2 * (r_u32(buffer) - 13), i, low = 0, high = 0, model;
    for (i = 0; ((int32_t)(uint32_t)(i)) < ((int32_t)(uint32_t)(limit)); ++i)
    {
        uint32_t id = r_u16(buffer + 4 + 2 * i);
        if (id == 0xFFFF)
            continue;
        if (id & 0x8000)
        {
            if (high < 2)
                w_u32(record + 88 + 4 * high++, id);
        }
        else if (low < 2)
            w_u32(record + 80 + 4 * low++, id);
    }
    model = buffer + 4 * r_u32(buffer);
    w_u32(record + 4, model);
    w_u8(record + 72, (uint8_t)((uint32_t)r_u8((model + 6)) + 1));
    w_u8(record + 73, (uint8_t)((uint32_t)r_u8((model + 4)) + 2 * r_u16(model + 6)));
    for (i = 0; i < 8; ++i)
    {
        if (i)
            model = r_u32(record + 4);
        w_u32(model + fields[i], r_u32(model + fields[i]) + buffer);
    }
    return 1;
}

uint32_t sub_800325BC(uint32_t record, uint32_t group)
{
    FUNCTION_MARKER(0x800325BCu, "SLUS_010.53");
    uint32_t mode, i;
    for (mode = 0; mode < 2; ++mode)
        for (i = 0; i < 2; ++i)
        {
            uint32_t id = r_u32(record + 80 + 8 * mode + 4 * i), index;
            if (id == 0xFFFFFFFF)
                break;
            index = sub_80022218(id, group);
            if (index != 0xFFFFFFFF)
                w_u32(record + (mode ? 96 : 104) + 4 * i, 0x800D9268 + 48 * index);
        }
    return 1;
}

uint32_t sub_80013204(uint32_t id, uint32_t group)
{
    FUNCTION_MARKER(0x80013204u, "SLUS_010.53");
    uint32_t record = 0x800D87E8 + 1344 * group, end = record + 1344;
    while (record < end)
    {
        uint32_t tag = r_u32(record), match = r_u32(record + 8), model = r_u32(record + 4);
        if (tag != 0xFFFFFFFF && match != 0xFFFFFFFF && model && match == id)
            return record;
        record += 112;
    }
    return 0;
}

uint32_t sub_80013360(uint32_t id, uint32_t group)
{
    FUNCTION_MARKER(0x80013360u, "SLUS_010.53");
    return (uint32_t)r_u8((r_u32(0x8005B2F8) + 4)) & 16 ? sub_80013204(id, group == 0) : 0;
}

uint32_t sub_80012BA8(uint32_t header, uint32_t id, uint32_t group)
{
    FUNCTION_MARKER(0x80012BA8u, "SLUS_010.53");
    uint32_t out = 0x800D43C0 + 28 * group, last;
    w_u32(out, r_u32(header + 8));
    w_u32(out + 4, r_u32(header + 20));
    w_u32(out + 8, r_u32(header + 24));
    w_u32(out + 12, r_u32(header + 28));
    w_u16(out + 16, r_u16(header + 14));
    w_u16(out + 18, r_u16(header + 16));
    last = r_u16(header + 18);
    w_u32(out + 24, id);
    w_u16(out + 22, 1);
    w_u16(out + 20, last);
    return out;
}

uint32_t sub_80013110(uint32_t kind, uint32_t point, uint32_t group, uint32_t extra, uint32_t extended)
{
    uint32_t scalar_value_16;

    FUNCTION_MARKER(0x80013110u, "SLUS_010.53");
    uint32_t player = 0x800CD898 + 1132 * group, x = (uint32_t)r_s16((player + 186)) - (uint32_t)r_s16((point + 2));
    uint32_t y = (uint32_t)r_s16((player + 194)) - (uint32_t)r_s16((point + 10)), sign, t, distance, limit;
    sign = ((uint32_t)((!((uint32_t)(31))) ? (((uint32_t)(x))) : ((((uint32_t)(x)) >> ((uint32_t)(31))) | ((((uint32_t)(x)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(31)))) : 0))));
    x = (x + sign) ^ sign;
    sign = ((uint32_t)((!((uint32_t)(31))) ? (((uint32_t)(y))) : ((((uint32_t)(y)) >> ((uint32_t)(31))) | ((((uint32_t)(y)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(31)))) : 0))));
    y = (y + sign) ^ sign;
    if (((int32_t)(uint32_t)(x)) < ((int32_t)(uint32_t)(y)))
    {
        t = x;
        x = y;
        y = t;
    }
    t = y + ((uint32_t)((!((uint32_t)(1))) ? (((uint32_t)(y))) : ((((uint32_t)(y)) >> ((uint32_t)(1))) | ((((uint32_t)(y)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(1)))) : 0))));
    distance = x - ((uint32_t)((!((uint32_t)(5))) ? (((uint32_t)(x))) : ((((uint32_t)(x)) >> ((uint32_t)(5))) | ((((uint32_t)(x)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(5)))) : 0)))) - ((uint32_t)((!((uint32_t)(7))) ? (((uint32_t)(x))) : ((((uint32_t)(x)) >> ((uint32_t)(7))) | ((((uint32_t)(x)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(7)))) : 0)))) + ((uint32_t)((!((uint32_t)(2))) ? (((uint32_t)(t))) : ((((uint32_t)(t)) >> ((uint32_t)(2))) | ((((uint32_t)(t)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(2)))) : 0)))) + ((uint32_t)((!((uint32_t)(6))) ? (((uint32_t)(t))) : ((((uint32_t)(t)) >> ((uint32_t)(6))) | ((((uint32_t)(t)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(6)))) : 0))));
    if (kind == 6)
        limit = ((scalar_value_16 = (r_u32(0x8005AD58) + extra)), (((uint32_t)((!((uint32_t)(16))) ? (scalar_value_16) : ((scalar_value_16 >> ((uint32_t)(16))) | ((scalar_value_16 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(16)))) : 0))))));
    else
        limit = kind < 2 ? (extended ? 350u : 300u) : (extended ? 230u : 200u);
    return ((int32_t)(uint32_t)(distance)) < ((int32_t)(uint32_t)(limit));
}

uint32_t sub_80013294(uint32_t kind, uint32_t id, uint32_t group)
{
    FUNCTION_MARKER(0x80013294u, "SLUS_010.53");
    uint32_t object, nearby, flags, bit, value;
    if (((id - 1) & 0xFFFF) >= 223)
        return 0;
    object = r_u32(0x800CD6C4) + 280 * (id & 31);
    nearby = sub_80013110(kind, object + 12, group, 0, 1);
    flags = (uint32_t)r_u8((object + 3));
    bit = 1u << (group & 31);
    if (flags & bit)
    {
        if (nearby)
            return 0;
        value = flags & (1u << ((group ^ 1) & 31));
    }
    else
    {
        if (!nearby)
            return 0;
        value = flags | bit;
    }
    w_u8(object + 3, (uint8_t)value);
    return 1;
}

uint32_t sub_8001339C(uint32_t id, uint32_t group)
{
    FUNCTION_MARKER(0x8001339Cu, "SLUS_010.53");
    static const uint32_t counts[5] = {2, 0, 6, 4, 8}, fields[5] = {40, 36, 48, 44, 52}, strides[5] = {76, 68, 88, 64, 64};
    uint32_t source, target, result, pass;
    if (!((uint32_t)r_u8((r_u32(0x8005B2F8) + 4)) & 16))
        return 0;
    source = sub_80013204(id, group);
    result = sub_80013360(id, group);
    if (!source || !result)
        return result;
    source = r_u32(r_u32(source + 4) + 32);
    target = r_u32(r_u32(result + 4) + 32);
    for (pass = 0; pass < 5; ++pass)
    {
        uint32_t i = 0, offset = 0;
        result = r_u16(source + counts[pass]);
        while (i < r_u16(source + counts[pass]))
        {
            uint32_t sp = r_u32(source + fields[pass]), tp = r_u32(target + fields[pass]);
            uint32_t state = pass == 3 ? r_u32(0x8005B2F8) : 0;
            w_u16(tp + offset + 4, r_u16(sp + offset + 4));
            if (pass == 2)
            {
                sp = r_u32(source + 48) + offset;
                if (((int32_t)(uint32_t)((uint32_t)r_s16((sp + 4)))) > 0)
                    (void)sub_80013294(6, r_u16(sp + 4), group ^ 1);
            }
            if (pass == 3 && ((uint32_t)r_u8((state + 4)) & 16) && r_u16(r_u32(target + 44) + offset + 2) == 50)
                (void)sub_80012BA8(target, id, group);
            ++i;
            result = i < r_u16(source + counts[pass]);
            offset += strides[pass];
        }
    }
    return result;
}

uint32_t sub_800135E8(uint32_t model_pointer, uint32_t group)
{
    FUNCTION_MARKER(0x800135E8u, "SLUS_010.53");
    uint32_t header = r_u32(r_u32(model_pointer) + 32), i, value;
    for (i = 0; i < 5; ++i)
    {
        value = r_u16(header + 2 * i) ? header + r_u32(header + 36 + 4 * i) : 0;
        w_u32(header + 36 + 4 * i, value);
    }
    return sub_8001339C(r_u32(model_pointer + 4), group);
}

uint32_t sub_80032A20(uint32_t buffer, uint32_t id, uint32_t kind, uint32_t extra, uint32_t owner, uint32_t group)
{
    FUNCTION_MARKER(0x80032A20u, "SLUS_010.53");
    uint32_t index, record, flags;
    if (((int32_t)(uint32_t)(r_u32(0x8005AEDC))) >= 24)
        return 0;
    index = sub_800329BC(id & 0x0FFFFFFF, group);
    if (kind == 0 || kind == 8)
    {
        if (index != 0xFFFFFFFF)
            return 3;
        index = sub_800329BC(0xFFFFFFFF, group);
        if (index == 0xFFFFFFFF)
            return 0;
        record = 0x800D87E8 + 112 * index;
        w_u32(record, id);
        if (r_u32(record + 8) == 0xFFFFFFFF)
        {
            w_u32(record + 8, id);
            w_u32(record + 12, 0);
            w_u32(record + 76, extra);
            (void)sub_8003234C(record, buffer);
            w_u32(0x8005AEDC, r_u32(0x8005AEDC) + 1);
            (void)sub_800135E8(record + 4, group);
            flags = r_u32(record + 12);
            w_u32(record + 12, flags | 1);
            if (kind)
                w_u32(r_u32(record + 4) + 60, 0);
            else
                w_u32(record + 12, flags | 3);
            w_u32(record + 68, owner);
        }
    }
    else
    {
        if (index == 0xFFFFFFFF)
            return 0;
        record = 0x800D87E8 + 112 * index;
        if (!r_u32(record + 4))
            return 0;
        if (r_u32(record + 12) & 2)
            return 3;
        (void)sub_80032338(record + 4, buffer);
        w_u32(record + 12, r_u32(record + 12) | 2);
    }
    (void)sub_800325BC(record, group);
    (void)sub_80033F14(record, 0, group);
    (void)sub_80033F14(record, 1, group);
    return 1;
}

uint32_t sub_80023960(uint32_t input, uint32_t group)
{
    FUNCTION_MARKER(0x80023960u, "SLUS_010.53");
    uint32_t state = 0x80053478 + 128 * group, id = r_u32(state + 72), i, value, result;
    for (i = 0; i < 4; ++i)
        if ((uint32_t)r_s16((input + 4 + 6 * i)) == id)
            break;
    if (i == 4)
        return 0;
    value = (uint32_t)r_s16((input + 6 * i + (((int32_t)(uint32_t)(r_u32(state + 64))) > 0 ? 6 : 8)));
    i = ((int32_t)(uint32_t)(r_u32(state + 64))) > 0;
    w_u32(state + 104, value);
    result = i ? value - r_u32(state + 68) : r_u32(state + 68) - value;
    w_u32(state + 100, result);
    return result;
}

uint32_t sub_800136BC(uint32_t buffer, uint32_t input, uint32_t value)
{
    FUNCTION_MARKER(0x800136BCu, "SLUS_010.53");
    uint32_t manager, record, i, j, last;
    if (r_u32(0x8005B2D0) || r_u32(buffer + 12) != 0x50414E4F)
        return 2;
    manager = r_u32(0x8005B278);
    if (((int32_t)(uint32_t)((uint32_t)r_s16((manager + 34)))) >= 20)
        return 0;
    record = manager + 740;
    for (i = 0; i < 20; ++i, record += 12)
        if (r_u32(record))
        {
            uint32_t candidate = r_u32(record + 4), id = r_u16(input);
            /* Each candidate triple compares to the same input triple in MIPS. */
            for (j = 0; j < 4; ++j, candidate += 6)
                if (r_u16(candidate) != id || r_u16(candidate + 2) != r_u16(input + 2) || r_u16(candidate + 4) != r_u16(input + 4))
                    break;
            if (j == 4)
                return 3;
        }
    record = r_u32(0x8005B278) + 740;
    for (i = 0; i < 20; ++i, record += 12)
        if (!r_u32(record))
            break;
    if (i == 20)
        return 0;
    last = r_u16(buffer + 4);
    manager = r_u32(0x8005B278);
    w_u32(buffer + 24, last);
    last = r_u32(buffer + 8);
    w_u32(buffer + 36, buffer);
    w_u32(buffer + 28, last);
    w_u16(manager + 34, r_u16(manager + 34) + 1);
    w_u32(record, buffer);
    w_u32(record + 4, input);
    w_u32(record + 8, value);
    return 1;
}

uint32_t sub_8003CA50(uint32_t id, uint32_t buffer, uint32_t group)
{
    FUNCTION_MARKER(0x8003CA50u, "SLUS_010.53");
    uint32_t state = r_u32(0x8005B2F8), free_index = 0xFFFFFFFF, i, record;
    for (i = 0; i < 6; ++i)
    {
        record = 0x800D4B10 + 16 * i;
        if (r_u32(record) == id)
        {
            if (!((uint32_t)r_u8((state + 4)) & 16) || group == r_u32(record + 4))
                return 0xFFFFFFFD;
            w_u32(record + 8 + 4 * group, buffer);
            return 0xFFFFFFFF;
        }
        if (free_index == 0xFFFFFFFF && r_u32(record) == 0xFFFFFFFF)
            free_index = i;
    }
    if (free_index == 0xFFFFFFFF)
        return 0;
    record = 0x800D4B10 + 16 * free_index;
    w_u32(record, id);
    w_u32(record + 4, group);
    w_u32(record + 8 + 4 * group, buffer);
    w_u32(record + 8 + 4 * (group ^ 1), 0);
    i = r_u32(0x8005B320) + 1;
    group = ((int32_t)(uint32_t)(free_index)) < ((int32_t)(uint32_t)(r_u32(0x8005B31C)));
    w_u32(0x8005B320, i);
    if (!group)
        w_u32(0x8005B31C, free_index);
    return 1;
}

uint32_t sub_80039A08(uint32_t id)
{
    FUNCTION_MARKER(0x80039A08u, "SLUS_010.53");
    uint32_t manager, mapped, threshold, address;
    if (((int32_t)(uint32_t)(id)) < 0)
        return 0;
    manager = r_u32(0x8005B240);
    if (((int32_t)(uint32_t)(id)) >= ((int32_t)(uint32_t)((uint32_t)r_s16((manager + 40)))))
        return 0;
    threshold = (uint32_t)r_s16((manager + 44));
    mapped = ((int32_t)(uint32_t)(id)) < ((int32_t)(uint32_t)(threshold)) ? id + (uint32_t)r_s16((manager + 42)) : id - threshold;
    manager = r_u32(0x8005B240);
    if (((int32_t)(uint32_t)(mapped)) >= ((int32_t)(uint32_t)((uint32_t)r_s16((manager + 40)))))
        return 0;
    address = r_u32(manager + 28) + (mapped << 5);
    return r_u32(address) == id ? address : 0;
}

static unsigned segment_tag_equal(uint32_t a, uint32_t b)
{
    unsigned i;
    for (i = 0; i < 4; ++i)
    {
        uint32_t x = (uint32_t)r_u8((a + i)), y = (uint32_t)r_u8((b + i));
        if (x != y)
            return 0;
        if (!x)
            return 1;
    }
    return 1;
}

uint32_t sub_8003CFCC(uint32_t buffer, uint32_t id)
{
    FUNCTION_MARKER(0x8003CFCCu, "SLUS_010.53");
    static const unsigned fields[12] = {44, 48, 52, 56, 60, 64, 68, 72, 92, 96, 100, 104};
    uint32_t record = sub_80039A08(id & 0xFFFF), p = buffer + 108, end = buffer + 16384, global, data;
    unsigned i;
    if (!record)
        return 0;
    while (p < end)
    {
        for (i = 0; i < 12; ++i)
            if (segment_tag_equal(p, 0x8005AEEC + 8 * i))
                break;
        if (i == 12)
            break;
        w_u32(buffer + fields[i], p + 8);
        p += r_u32(p + 4);
    }
    global = r_u32(0x8005AD54);
    w_u32(record + 12, buffer);
    if (!global)
        w_u32(0x8005AD54, buffer);
    if (((uint32_t)r_u8((r_u32(0x8005B2F8) + 4)) & 16) && buffer && r_u32(buffer) == 11)
    {
        data = r_u32(buffer + 96) + 1560;
        w_u32(data + 8, 0x0B897B7D);
        w_u32(data + 12, 0xFFF0DC54);
        w_u32(data + 16, 0x1538BC84);
        data = r_u32(buffer + 96) + 400;
        w_u32(data + 8, 0x0B775E29);
        w_u32(data + 12, 0xFFF0BB49);
        w_u32(data + 16, 0x1515ACCF);
        data = r_u32(buffer + 96) + 1000;
        w_u32(data + 8, 0x0B38FCEC);
        w_u32(data + 12, 0xFFF0BB49);
        w_u32(data + 16, 0x15361E79);
        data = r_u32(buffer + 96) + 1360;
        w_u32(data + 12, r_u32(data + 24));
    }
    if (buffer && r_u32(buffer) == 105)
    {
        data = r_u32(buffer + 96) + 2520;
        w_u32(data + 8, 0xED2431AD);
        w_u32(data + 12, 0xFE062800);
        w_u32(data + 16, 0xEBC76784);
        data = r_u32(buffer + 96) + 2480;
        w_u32(data + 20, 0xED2431AD);
        w_u32(data + 24, 0xFE062800);
        w_u32(data + 28, 0xEBC76784);
        w_u32(data + 8, 0xED08D8CA);
        w_u32(data + 12, 0xFE0A3D4D);
        w_u32(data + 16, 0xEBC6469D);
        data = r_u32(buffer + 96) + 2440;
        w_u32(data + 20, 0xED08D8CA);
        w_u32(data + 24, 0xFE0A3D4D);
        w_u32(data + 28, 0xEBC6469D);
        w_u32(data + 8, 0xECED651D);
        w_u32(data + 12, 0xFE09E617);
        w_u32(data + 16, 0xEBBF1AEC);
    }
    return 1;
}

/* Effects-only ABI: all audited callers discard the residual MIPS v0. */
void sub_80093F94(uint32_t object, uint32_t previous)
{
    FUNCTION_MARKER(0x80093F94u, "RASHCDG.BIN");
    uint32_t active;
    if (!object)
        return;
    active = r_u16(object + 320);
    if ((active & 1u) == (previous & 1u))
        return;
    if (active)
        (void)sub_8009432C(object);
    else
        sub_80093FE4(object);
}

void sub_80093ED4(uint32_t object, uint32_t force)
{
    FUNCTION_MARKER(0x80093ED4u, "RASHCDG.BIN");
    uint32_t previous, value;
    if (!r_u16(object + 320) && r_u16(object + 172) >= r_u32(r_u32(0x8005B2F8) + 48) && ((uint32_t)r_u8((r_u32(object + 1084) + 1)) & 15u) == 2u && !((uint32_t)r_u8((object + 928)) & 16u))
        return;
    previous = (uint32_t)r_s16((object + 320));
    value = force ? 0 : sub_80039F68(object + 172);
    w_u16(object + 320, (uint16_t)((0u - value) & ((r_u16(object + 320) & 2u) | 1u)));
    sub_80093F94(object, previous);
}

/* 80037338: live scratchpad actor list shared with the frame's other passes. */
uint32_t sub_80037338(void)
{
    FUNCTION_MARKER(0x80037338u, "SLUS_010.53");
    uint32_t cursor = 0x1f800004u, end = r_u32(0x1f800000u), actor, flags, state;
    for (;;)
    {
        /* The original fetches the sentinel entry before comparing the end. */
        actor = r_u32(cursor);
        if (cursor >= end)
            return 0;
        if (r_u32(actor + 828))
        {
            flags = r_u32(actor + 388) & 0xffffff7fu;
            state = r_u32(actor + 564) & 0xfbffffffu;
            w_u32(actor + 388, flags);
            w_u32(actor + 564, state);
        }
        else
        {
            w_u32(actor + 388, r_u32(actor + 388) | 0x80u);
            w_u32(actor + 564, r_u32(actor + 564) | 0x04000000u);
        }
        if (r_u32(actor + 560) & 0x08000000u)
            flags = r_u32(actor + 388) & 0xffffffdfu;
        else
        {
            sub_800396A8(actor);
            flags = r_u32(actor + 388) | 0x20u;
        }
        w_u32(actor + 388, flags);
        (void)sub_8003701C(actor);
        if (r_u32(actor + 564) & 0x40000u)
            w_u32(actor + 364, r_u32(r_u32(actor + 832) + 192));
        (void)sub_80037104(actor);
        cursor += 4;
        end = r_u32(0x1f800000u);
    }
}

/* 8003AE24: route assignment, opposite-direction marker and road zones. */
uint32_t sub_8003AE24(void)
{
    FUNCTION_MARKER(0x8003AE24u, "SLUS_010.53");
    uint32_t cursor = 0x1F800004, end = r_u32(0x1F800000), actor, linked, descriptor, word, record;
    actor = r_u32(cursor);
    /* The initial branch delay slot overwrites v0 with LUI 800D. */
    if (cursor >= end)
        return 0x800D0000;
    do
    {
        sub_8003AF9C(actor + 172, 1, 0);
        linked = r_u32(actor + 856);
        if (linked)
            w_u32(linked + 428, r_u32(actor + 428));
        descriptor = r_u32(actor + 1084);
        w_u8(descriptor, (uint8_t)((uint32_t)r_u8(descriptor) & 127u));
        if ((uint32_t)r_s16(0x800D6182) != 0xffffffffu && r_u32(r_u32(actor + 852) + 604) < 3u && (r_u16(actor + 172) < r_u32(r_u32(0x8005B2F8) + 48) || ((uint32_t)r_u8((r_u32(actor + 1084) + 1)) & 15u) != 2u))
        {
            word = r_u32(actor + 360);
            if (!(word >> 16))
            {
                record = sub_8003B4B0(r_u32(actor + 428), word & 65535u);
                if (record && ((r_u32(record + 4) ^ r_u32(actor + 364)) & 0x80000000u))
                {
                    descriptor = r_u32(actor + 1084);
                    w_u8(descriptor, (uint8_t)((uint32_t)r_u8(descriptor) | 128u));
                }
            }
        }
        (void)sub_8003A9D8(actor);
        cursor += 4;
        end = r_u32(0x1F800000);
        actor = r_u32(cursor);
    } while (cursor < end);
    return 0;
}

uint32_t sub_8003B520(void)
{
    FUNCTION_MARKER(0x8003B520u, "SLUS_010.53");
    uint32_t cursor = 0x1F800004, end = r_u32(0x1F800000), actor, previous, value, limit, descriptor, linked;
    for (;;)
    {
        actor = r_u32(cursor);
        if (cursor >= end)
            return 0;
        previous = r_u32(actor + 324);
        value = sub_8003B61C(actor + 172);
        w_u32(actor + 324, value);
        limit = 0x649581;
        while (((int32_t)(uint32_t)(value)) < ((int32_t)(uint32_t)(limit)))
        {
            descriptor = r_u32(actor + 1084);
            if ((uint32_t)r_u8((descriptor + 68)) & 1u)
                break;
            if (((int32_t)(uint32_t)(limit)) < ((int32_t)(uint32_t)(previous)))
                w_u8(descriptor + 68, (uint8_t)((uint32_t)r_u8((descriptor + 68)) | 1u));
            else
                limit += 0xFFE6DAA0u;
            value = r_u32(actor + 324);
        }
        linked = r_u32(actor + 856);
        if (linked)
            w_u32(linked + 324, r_u32(actor + 324));
        cursor += 4;
        end = r_u32(0x1F800000);
    }
}

/* 8003E150: refresh contacts for queued actors and their linked actors. */
uint32_t sub_8003E150(void)
{
    FUNCTION_MARKER(0x8003E150u, "SLUS_010.53");
    uint32_t cursor = 0x1F800004, end = r_u32(0x1F800000), actor, linked;
    for (;;)
    {
        actor = r_u32(cursor); /* Sentinel load precedes unsigned test. */
        if (cursor >= end)
            return 0;
        (void)sub_8003DFF4(actor);
        linked = r_u32(actor + 0x358);
        if (linked)
            (void)sub_8003DFF4(linked);
        if (r_u32(actor + 0x238) & 0x800u)
            w_u32(actor + 0x184, r_u32(actor + 0x184) | 0x40u);
        cursor += 4;
        end = r_u32(0x1F800000);
    }
}

uint32_t sub_80093E6C(void)
{
    FUNCTION_MARKER(0x80093E6Cu, "RASHCDG.BIN");
    uint32_t result = r_u32(0x800CE4DC), count = r_u32(result), object = r_u32(0x800CE4D0);
    while (((int32_t)(uint32_t)(count)) >= 0)
    {
        sub_80093ED4(object, 0);
        result = r_u32(0x800CE4D4);
        --count;
        object += result;
    }
    return result;
}

uint32_t sub_800950E8(void)
{
    uint32_t result, count, object, body;
    FUNCTION_MARKER(0x800950E8, "RASHCDG.BIN");
    result = r_u32(0x800CE4DC);
    count = r_u32(result);
    object = r_u32(0x800CE4D0);
    while ((int32_t)count >= 0)
    {
        body = r_u32(object + 852);
        if (r_u32(body + 604) - 3 < 2)
            sub_800951B8(body, 0);
        body = r_u32(object + 852);
        if (r_u8(body + 572) & 16)
        {
            body = r_u32(r_u32(object + 856) + 852);
            if (r_u32(body + 604) - 3 < 2)
                sub_800951B8(body, 0);
        }
        result = r_u32(0x800CE4D4);
        --count;
        object += result;
    }
    return result;
}

uint32_t sub_8003CEC4(uint32_t buffer, uint32_t id, uint32_t group)
{
    uint32_t result, player;
    FUNCTION_MARKER(0x8003CEC4, "SLUS_010.53");
    id &= 0xFFFF;
    if ((int32_t)id >= r_s16(r_u32(0x8005B240) + 40))
        return 2;
    result = sub_8003CA50(id, buffer, group);
    if ((int32_t)result <= 0)
        return (int32_t)result < 0 ? 0 - result : result;
    if (!sub_8003CFCC(buffer, id))
        return 0;
    player = r_u32(0x8005B38C);
    if (player && !r_u32(r_u32(player + 1084) + 40))
        goto refresh;
    player = r_u32(0x8005B21C);
    if (!player || r_u32(r_u32(player + 1084) + 40))
        return 1;
refresh:
    (void)sub_80093E6C();
    (void)sub_800950E8();
    return 1;
}

/* RASHCDG 80099D48: follow the linked object's road/junction position. */
uint32_t sub_80099D48(uint32_t object)
{
    FUNCTION_MARKER(0x80099D48u, "RASHCDG.BIN");
    uint32_t road = 0, linked = r_u32(object + 596), route = r_u32(object + 360);
    uint32_t id = route & 0xFFFF, linked_id = r_u16(linked + 360);
    uint32_t junction = 0, linked_road = 0, linked_junction = 0, step = 0, unmatched = 1;
    uint32_t p, end, length, zero_progress = 0, previous, progress, result;
    if (id == linked_id)
    {
        if (!(route >> 16))
            step = ((int32_t)(uint32_t)(r_u32(object + 368))) < ((int32_t)(uint32_t)(r_u32(linked + 368))) ? 0x50000 : 0xFFFB0000;
        goto finish;
    }
    if (route >> 16)
        junction = sub_800245F4(id);
    else
        road = sub_800245DC(id);
    if (r_u16(linked + 362))
        linked_junction = sub_800245F4(linked_id);
    else
        linked_road = sub_800245DC(linked_id);
    if (linked_road)
    {
        length = (r_u32(linked_road + 4) >> 6) << 16;
        if (junction)
        {
            p = junction + 8;
            end = p + (r_u32(junction + 4) << 3);
            while (p < end)
            {
                if (r_u32(p) == linked_id)
                {
                    road = linked_road;
                    w_u32(object + 360, linked_id);
                    unmatched = 0;
                    zero_progress = ((int32_t)(uint32_t)(r_u32(p + 4))) > 0;
                    break;
                }
                p += 8;
            }
        }
        else
        {
            uint32_t head = r_u32(road + 8), linked_head = r_u32(linked_road + 8), linked_tail = r_u32(linked_road + 12);
            if (head == linked_head || head == linked_tail)
            {
                unmatched = 0;
                step = 0xFFFB0000;
            }
            else
            {
                uint32_t tail = r_u32(road + 12);
                if (tail == linked_head || tail == linked_tail)
                {
                    unmatched = 0;
                    step = 0x50000;
                }
            }
        }
        if (unmatched)
        {
            road = linked_road;
            w_u32(object + 360, linked_id);
            zero_progress = ((int32_t)(uint32_t)(((uint32_t)((!((uint32_t)(1))) ? (((uint32_t)(length))) : ((((uint32_t)(length)) >> ((uint32_t)(1))) | ((((uint32_t)(length)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(1)))) : 0)))))) < ((int32_t)(uint32_t)(r_u32(linked + 368)));
        }
        if (!step)
            w_u32(object + 368, zero_progress ? 0 : length);
    }
    else
    {
        p = linked_junction + 8;
        if (road)
        {
            end = p + (r_u32(linked_junction + 4) << 3);
            while (p < end)
            {
                if (r_u32(p) == id)
                {
                    step = ((int32_t)(uint32_t)(r_u32(p + 4))) < 0 ? 0x50000 : 0xFFFB0000;
                    unmatched = 0;
                    break;
                }
                p += 8;
            }
        }
        if (unmatched)
        {
            id = r_u32(linked_junction + 8);
            w_u32(object + 360, id);
            road = sub_800245DC(id);
            w_u32(object + 368, ((int32_t)(uint32_t)(r_u32(linked_junction + 12))) < 0 ? 0 : (r_u32(road + 4) >> 6) << 16);
        }
    }
finish:
    previous = r_u32(object + 368);
    progress = previous + step;
    w_u32(object + 368, progress);
    if (!road)
        return previous;
    if (((int32_t)(uint32_t)(progress)) < 0)
        result = r_u16(road + 8) | 0x10000;
    else
    {
        length = (r_u32(road + 4) >> 6) << 16;
        if (((int32_t)(uint32_t)(length)) >= ((int32_t)(uint32_t)(progress)))
            return 0;
        result = r_u16(road + 12) | 0x10000;
    }
    w_u32(object + 360, result);
    return result;
}

uint32_t sub_80039C90(uint32_t buffer, uint32_t id)
{
    FUNCTION_MARKER(0x80039C90u, "SLUS_010.53");
    uint32_t p, end;
    if (!buffer || (uint32_t)r_s16((buffer + 16)) != 1)
        return 0;
    p = r_u32(buffer + 44);
    end = p + ((uint32_t)r_s16((buffer + 18)) << 5);
    while (p < end)
    {
        if (!((uint32_t)r_s16((p + 2)) | (r_u32(p + 12) - id)))
            return p;
        p += 32;
    }
    return 0;
}


uint32_t sub_80039CFC(uint32_t record, const uint32_t position[3])
{
    uint32_t buffer;
    uint32_t kind;
    uint32_t tag;
    uint32_t route;
    int32_t progress;
    FUNCTION_MARKER(0x80039CFC, "SLUS_010.53");
    if (!position || !record)
        return 0;
    buffer = r_u32(record + 12);
    if (!buffer)
        return 0;
    kind = (uint32_t)r_s16(record + 4);
    tag = (kind << 16) | r_u16(record + 16);
    progress = (int16_t)(position[2] >> 16);
    if (!kind)
        return tag == position[0] && progress >= r_s16(record + 22) && progress <= r_s16(record + 26);
    if (tag == position[0])
        return !r_u32(record + 28);
    route = sub_80039C90(buffer, position[0] & 0xFFFF);
    return route && !r_s16(route + 2) && progress >= r_s16(route + 26) && progress <= r_s16(route + 30);
}

uint32_t sub_80039DFC(uint32_t object_position, const uint32_t position[3])
{
    uint32_t prior_id = 0xFFFFFFFF;
    uint32_t prior_buffer = 0;
    uint32_t record;
    uint32_t id;
    uint32_t index;
    uint32_t tag;
    FUNCTION_MARKER(0x80039DFC, "SLUS_010.53");
    if (object_position && r_s16(object_position + 148))
    {
        record = sub_80039A08(r_u32(r_u32(object_position + 156)));
        if (!record)
            return 0;
        tag = r_u16(object_position);
        if (((tag >> 5) < 2 && (int32_t)(tag & 31) < r_s32(r_u32(0x8005B2F8) + 48)) || sub_80039CFC(record, position))
            return r_u32(record + 12);
        prior_buffer = r_u32(record + 12);
        prior_id = r_u32(r_u32(object_position + 156));
    }
    for (index = 0; (int32_t)index <= r_s32(0x8005B31C); ++index)
    {
        id = r_u32(0x800D4B10 + 16 * index);
        if (id == 0xFFFFFFFF || id == prior_id)
            continue;
        record = sub_80039A08(id);
        if (record && sub_80039CFC(record, position))
            return prior_id == 0xFFFFFFFF ? r_u32(record + 12) : prior_buffer;
    }
    return 0;
}

uint32_t sub_8003F3B4(uint32_t id)
{
    FUNCTION_MARKER(0x8003F3B4u, "SLUS_010.53");
    uint32_t count = (uint32_t)r_s16(0x800D6182), p, i;
    if (((int32_t)(uint32_t)(count)) <= 0)
        return 0;
    p = r_u32(0x800D6194);
    for (i = 0; ((int32_t)(uint32_t)(i)) < ((int32_t)(uint32_t)(count)); ++i, p += 120)
        if (r_u32(p) == id)
            return p;
    return 0;
}

uint32_t sub_80039B60(uint32_t id)
{
    FUNCTION_MARKER(0x80039B60u, "SLUS_010.53");
    uint32_t manager, count, p, i;
    if (((int32_t)(uint32_t)(id)) < 0)
        return 0;
    manager = r_u32(0x8005B240);
    count = (uint32_t)r_s16((manager + 60));
    p = r_u32(manager + 48);
    for (i = 0; ((int32_t)(uint32_t)(i)) < ((int32_t)(uint32_t)(count)); ++i, p += 12)
        if ((uint32_t)r_s16(p) == id)
            return p;
    return 0;
}

uint32_t sub_8002EAD8(const uint32_t origin[3], const uint16_t direction[3], uint32_t scale, uint32_t output[3])
{
    uint32_t axis;
    uint32_t result = 0;

    FUNCTION_MARKER(0x8002EAD8u, "SLUS_010.53");
    for (axis = 0; axis < 3; ++axis)
    {
        int32_t component = (int16_t)xport_load_le16((const uint8_t *)direction + 2u * axis) * 16;
        int64_t product = (int64_t)component * ((int32_t)(uint32_t)(scale));
        uint32_t position = xport_load_le32((const uint8_t *)origin + 4u * axis);
        uint32_t scaled = (uint32_t)((uint64_t)product >> 16);

        result = scaled + position;
        xport_store_le32((uint8_t *)output + 4u * axis, result);
    }
    return result;
}

/* Sequential loads preserve shifted aliases. Upper return word is residual v1,
 * not a carry from the origin addition (8002E570..8002E600). */
uint64_t sub_8002E570(const uint32_t origin[3], const uint32_t velocity[3], uint32_t scale, uint32_t output[3])
{
    uint64_t shifted = 0;
    uint32_t value = 0;
    uint32_t axis;
    FUNCTION_MARKER(0x8002E570, "SLUS_010.53");
    for (axis = 0; axis < 3; ++axis)
    {
        uint32_t velocity_value = xport_load_le32((const uint8_t *)velocity + 4u * axis);
        int64_t product = (int64_t)((int32_t)(uint32_t)(velocity_value)) * ((int32_t)(uint32_t)(scale));
        uint32_t origin_value = xport_load_le32((const uint8_t *)origin + 4u * axis);
        shifted = ((uint64_t)product >> 16) | (product < 0 ? UINT64_C(0xFFFF000000000000) : 0);
        value = (uint32_t)shifted + origin_value;
        xport_store_le32((uint8_t *)output + 4u * axis, value);
    }
    return (shifted & UINT64_C(0xFFFFFFFF00000000)) | value;
}

uint64_t sub_8002EB78(const uint16_t input_a[3], const uint16_t input_b[3], uint16_t output[3], uint32_t scale_a, uint32_t scale_b)
{
    uint32_t first[3], second[3];
    uint64_t second_shifted = 0;
    uint32_t result = 0;
    uint32_t axis;
    FUNCTION_MARKER(0x8002EB78u, "SLUS_010.53");
    for (axis = 0; axis < 3; ++axis)
    {
        int32_t left = (int16_t)xport_load_le16((const uint8_t *)input_a + 2u * axis) * 16;
        int64_t product = (int64_t)((int32_t)(uint32_t)(scale_a)) * left;
        int32_t right = (int16_t)xport_load_le16((const uint8_t *)input_b + 2u * axis) * 16;
        first[axis] = (uint32_t)((uint64_t)product >> 16);
        product = (int64_t)((int32_t)(uint32_t)(scale_b)) * right;
        second_shifted = ((uint64_t)product >> 16) | (product < 0 ? UINT64_C(0xFFFF000000000000) : 0);
        second[axis] = (uint32_t)second_shifted;
    }
    for (axis = 0; axis < 3; ++axis)
    {
        result = (uint32_t)(((int32_t)(uint32_t)(first[axis] + second[axis])) >> 4);
        xport_store_le16((uint8_t *)output + 2u * axis, (uint16_t)result);
    }
    return (second_shifted & UINT64_C(0xFFFFFFFF00000000)) | result;
}

uint32_t sub_80037524(uint32_t record[8], uint32_t delta)
{
    FUNCTION_MARKER(0x80037524, "SLUS_010.53");
    uint32_t subrecord, segment, route, distance, last, current, progress;
    uint32_t candidate = 0;
    int32_t index, limit, progress_high;
    if (!record)
        return 0;
    subrecord = record[2];
    segment = record[0];
    route = record[1];
    distance = r_u32(segment + 68);
    last = r_u32(segment + 52) + 52 * ((uint32_t)r_s16(subrecord + 8) + (uint32_t)r_s16(subrecord + 10)) - 52;
    current = r_u32(record[3] + 40);
    progress = (int32_t)delta > 0 ? current + delta : current - delta;
    if (r_s16(route + 2) || r_s16(segment + 28) <= 0 || !distance)
        return 0;
    progress_high = (int32_t)progress >> 16;
    if (progress_high >= (r_s32(route + 24) >> 16) && progress_high <= r_s16(route + 30))
    {
        index = ((int32_t)(progress - r_u32(route + 24)) >> 16) / 50 - 1;
        limit = r_s16(subrecord + 22) - 1;
        if (limit < index)
            index = limit;
        if (index < 0)
            candidate = r_u32(segment + 52) + 52 * (uint32_t)r_s16(subrecord + 8);
        else
            candidate = r_u32(segment + 52) + 52 * ((uint32_t)r_s16(subrecord + 8) + (uint32_t)r_s16(distance + 4 * ((uint32_t)r_s16(subrecord + 20) + (uint32_t)index) + 2));
        limit = r_s16(subrecord + 10);
        while (r_s16(candidate) < limit)
        {
            if (r_s32(candidate + 32) >= (int32_t)(progress - r_u32(candidate + 40)))
                break;
            candidate += 52;
        }
        if (candidate <= last)
        {
            record[3] = candidate;
            record[5] = progress - r_u32(candidate + 40);
            return candidate;
        }
    }
    else if (progress_high < r_s16(route + 30))
    {
        candidate = r_u32(segment + 52) + 52 * (uint32_t)r_s16(subrecord + 8);
        record[5] = 0;
        record[3] = candidate;
        return candidate;
    }
    record[3] = last;
    record[5] = r_u32(last + 32);
    return candidate;
}

uint32_t sub_8003BE1C(const uint32_t record[8], uint32_t position, uint32_t selector, uint32_t *output)
{
    FUNCTION_MARKER(0x8003BE1C, "SLUS_010.53");
    uint32_t route, segment, piece, metadata, manager, entry, link;
    int32_t i, j;
    if (!record || selector == 0xFFFFFFFF)
        return 0;
    route = record[1];
    segment = record[0];
    for (i = 0; i < r_s16(route + 22); ++i)
    {
        piece = r_u32(segment + 48) + 28 * ((uint32_t)r_s16(route + 20) + (uint32_t)i);
        if (((int32_t)(r_u32(piece + 12) + 0x8000) >> 16) != ((int32_t)position >> 12))
            continue;
        metadata = sub_80039B60(r_u32(segment));
        if (!metadata)
            continue;
        for (j = 0; j < r_s16(metadata + 8); ++j)
        {
            manager = r_u32(0x8005B240);
            entry = r_u32(manager + 52) + 12 * ((uint32_t)r_s16(metadata + 6) + (uint32_t)j);
            if ((uint32_t)r_s16(entry + 10) != selector)
                continue;
            link = r_u32(manager + 56) + 12 * (uint32_t)r_s16(entry + 2);
            if (piece != r_u32(segment + 48) + 28 * ((uint32_t)r_s16(route + 20) + (uint32_t)r_s16(link + 4)))
                continue;
            *output = (uint32_t)r_s16(link + 8);
            if (r_s16(entry + 6) <= 0)
                *output = 0 - *output;
            return piece;
        }
    }
    return 0;
}

uint32_t sub_8003A700(uint32_t segment, const uint32_t position[3], uint32_t output[8])
{
    FUNCTION_MARKER(0x8003A700, "SLUS_010.53");
    uint32_t route_word, high, route, subrecord, record, metadata, entry = 0;
    uint32_t base1, base2, pieces, start, link, direction;
    int32_t i, count;
    if (!segment)
        return 0;
    route_word = position[0];
    high = route_word >> 16;
    if (!high)
    {
        route = r_s16(segment + 16) ? sub_80039C90(segment, route_word & 0xFFFF) : r_u32(segment + 44);
        if (!route)
            return 0;
        subrecord = r_u32(segment + 48) + 28 * (uint32_t)r_s16(route + 20);
        output[0] = segment;
        output[1] = route;
        output[2] = subrecord;
        output[4] = 0;
        output[5] = 0;
        output[6] = 0;
        output[7] = 0;
        output[3] = r_u32(segment + 52) + 52 * (uint32_t)r_s16(subrecord + 8);
        (void)sub_80037524(output, position[2] - r_u32(route + 24));
        return 1;
    }
    if ((uint32_t)r_s16(segment + 16) != high || r_u32(segment + 8) != (route_word & 0xFFFF) || r_u32(segment + 12))
        return 0;
    route = r_u32(segment + 44);
    subrecord = r_u32(segment + 48);
    record = sub_8003F3B4(r_u32(segment + 8));
    if (!record)
        return 0;
    if (r_s32(record + 16) > 0)
    {
        output[0] = segment;
        output[1] = route;
        subrecord = sub_8003BE1C(output, r_u32(record + 8), r_u32(record + 84), &direction);
    }
    if (!subrecord)
        return 0;
    output[0] = segment;
    output[1] = route;
    output[2] = subrecord;
    output[3] = r_u32(segment + 52) + 52 * (uint32_t)r_s16(subrecord + 8);
    metadata = sub_80039B60(r_u32(segment));
    if (metadata)
    {
        count = r_s16(metadata + 8);
        if (count > 0)
        {
            start = (uint32_t)r_s16(metadata + 6);
            record = r_u32(0x8005B240);
            pieces = r_u32(segment + 48);
            base1 = r_u32(record + 52);
            base2 = r_u32(record + 56);
            for (i = 0; i < count; ++i)
            {
                entry = base1 + 12 * (start + (uint32_t)i);
                link = base2 + 12 * (uint32_t)r_s16(entry + 2);
                if (r_s16(link + 6) && pieces + 28 * (uint32_t)r_s16(link + 4) == subrecord)
                    break;
            }
        }
        output[6] = metadata;
        output[7] = entry;
    }
    else
    {
        output[6] = 0;
        output[7] = 0;
    }
    output[5] = 0;
    output[4] = 0;
    return 1;
}

uint32_t sub_80039F68(uint32_t input)
{
    uint32_t scalar_value_33;

    FUNCTION_MARKER(0x80039F68u, "SLUS_010.53");
    uint32_t segment = sub_80039DFC(input, ((input + 188) ? (const uint32_t[3]){r_u32(input + 188), 0, (uint32_t)r_u16((input + 188) + 10) << 16} : NULL)), state = 0, aggregate = 0, tag, object, piece, i, count, minimum = 999;
    if (!segment)
        return 0;
    state = (uint32_t)r_s16((input + 148));
    tag = r_u16(input);
    if ((tag >> 5) < 2 && !state)
    {
        uint32_t geometry_result = 0;
        {
            const uint32_t route_position[3] = {r_u32(input + 188), r_u32(input + 188 + 4), r_u32(input + 188 + 8)};
            uint32_t route_geometry[8] = {r_u32(input + 156), r_u32(input + 156 + 4), r_u32(input + 156 + 8), r_u32(input + 156 + 12), r_u32(input + 156 + 16), r_u32(input + 156 + 20), r_u32(input + 156 + 24), r_u32(input + 156 + 28)};
            geometry_result = sub_8003A700(segment, route_position, route_geometry);
            w_u32(input + 156, route_geometry[0]);
            w_u32(input + 156 + 4, route_geometry[1]);
            w_u32(input + 156 + 8, route_geometry[2]);
            w_u32(input + 156 + 12, route_geometry[3]);
            w_u32(input + 156 + 16, route_geometry[4]);
            w_u32(input + 156 + 20, route_geometry[5]);
            w_u32(input + 156 + 24, route_geometry[6]);
            w_u32(input + 156 + 28, route_geometry[7]);
        }
        if (!geometry_result)
            return 0;
        piece = r_u32(input + 168);
        (void)sub_8002EAD8(xport_guest_ptr(piece + 20, 12), xport_guest_ptr(piece + 14, 6), r_u32(input + 176), xport_guest_ptr(input + 12, 12));
        (void)sub_8002EAD8(xport_guest_ptr(input + 12, 12), xport_guest_ptr(piece + 2, 6), r_u32(input + 172), xport_guest_ptr(input + 12, 12));
    }
    tag = r_u16(input);
    count = r_u32(r_u32(0x8005B2F8) + 48);
    if ((tag >> 5) < 2 && ((int32_t)(uint32_t)(tag & 31)) < ((int32_t)(uint32_t)(count)))
    {
        if (tag >> 5)
            state = 1;
        else
        {
            object = r_u32(0x8005B3A0) + 1096 * tag;
            if (object)
            {
                if (!r_u32(object + 1088))
                    object = r_u32(object + 856);
                if (object && r_u32(r_u32(object + 852) + 604) < 3)
                    state = 1;
            }
        }
        if (state)
            return state;
    }
    /* Original computes this minimum but does not use it to select a branch. */
    i = 0;
    do
    {
        uint32_t player = 0x800CD898 + 1132 * i, x = (uint32_t)r_s16((player + 186)) - (uint32_t)r_s16((input + 14)), y = (uint32_t)r_s16((player + 194)) - (uint32_t)r_s16((input + 22)), sign, t, d;
        sign = ((uint32_t)((!((uint32_t)(31))) ? (((uint32_t)(x))) : ((((uint32_t)(x)) >> ((uint32_t)(31))) | ((((uint32_t)(x)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(31)))) : 0))));
        x = (x + sign) ^ sign;
        sign = ((uint32_t)((!((uint32_t)(31))) ? (((uint32_t)(y))) : ((((uint32_t)(y)) >> ((uint32_t)(31))) | ((((uint32_t)(y)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(31)))) : 0))));
        y = (y + sign) ^ sign;
        if (((int32_t)(uint32_t)(x)) < ((int32_t)(uint32_t)(y)))
        {
            t = x;
            x = y;
            y = t;
        }
        t = y + ((uint32_t)((!((uint32_t)(1))) ? (((uint32_t)(y))) : ((((uint32_t)(y)) >> ((uint32_t)(1))) | ((((uint32_t)(y)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(1)))) : 0))));
        d = x - ((uint32_t)((!((uint32_t)(5))) ? (((uint32_t)(x))) : ((((uint32_t)(x)) >> ((uint32_t)(5))) | ((((uint32_t)(x)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(5)))) : 0)))) - ((uint32_t)((!((uint32_t)(7))) ? (((uint32_t)(x))) : ((((uint32_t)(x)) >> ((uint32_t)(7))) | ((((uint32_t)(x)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(7)))) : 0)))) + ((uint32_t)((!((uint32_t)(2))) ? (((uint32_t)(t))) : ((((uint32_t)(t)) >> ((uint32_t)(2))) | ((((uint32_t)(t)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(2)))) : 0)))) + ((uint32_t)((!((uint32_t)(6))) ? (((uint32_t)(t))) : ((((uint32_t)(t)) >> ((uint32_t)(6))) | ((((uint32_t)(t)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(6)))) : 0))));
        if (((int32_t)(uint32_t)(d)) < ((int32_t)(uint32_t)(minimum)))
            minimum = d;
        count = r_u32(r_u32(0x8005B2F8) + 48);
        ++i;
    } while (i < count);
    if (state)
    {
        for (i = 0; ((int32_t)(uint32_t)(i)) < ((int32_t)(uint32_t)(r_u32(r_u32(0x8005B2F8) + 48))); ++i)
            aggregate |= sub_80013110(r_u16(input) >> 5, input + 12, i, 0x140000, 1);
        state = aggregate;
    }
    if (!(uint32_t)r_s16((input + 148)))
    {
        for (i = 0; ((int32_t)(uint32_t)(i)) < ((int32_t)(uint32_t)(r_u32(r_u32(0x8005B2F8) + 48))); ++i)
            aggregate |= sub_80013110(r_u16(input) >> 5, input + 12, i, 0, 0);
        state = aggregate;
    }
    if (!state)
        return 0;
    tag = r_u16(input);
    if (tag >> 5)
        return state;
    segment = r_u32(0x8005B2F8);
    count = r_u32(segment + 48);
    object = r_u32(0x8005B3A0) + 1096 * tag;
    if (r_u16(object + 172) < count || ((uint32_t)r_u8((r_u32(object + 1084) + 1)) & 15) != 2)
        return state;
    {
        uint32_t distances[2] = {0, 0};
        for (i = 0; ((int32_t)(uint32_t)(i)) < ((int32_t)(uint32_t)(r_u32(segment + 48))); ++i)
        {
            uint32_t d = ((scalar_value_33 = (r_u32(object + 324) - r_u32(r_u32(0x8005B268 + 4 * i) + 324))), (((uint32_t)((!((uint32_t)(12))) ? (scalar_value_33) : ((scalar_value_33 >> ((uint32_t)(12))) | ((scalar_value_33 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(12)))) : 0))))));
            /* Original has exactly two local words; >2 players corrupt its stack. */
            if (i >= 2)
                abort();
            if (((int32_t)(uint32_t)(d)) < 0)
                d = 0u - d;
            distances[i] = d;
        }
        if (r_u32(r_u32(0x8005B2F8) + 48) == 2 && ((int32_t)(uint32_t)(distances[0])) < 201)
            return state;
        i = r_u32(r_u32(0x8005B2F8) + 48) == 2 ? 1u : 0u;
        if (((int32_t)(uint32_t)(distances[i])) >= 201)
            w_u8(object + 928, (uint8_t)((uint32_t)r_u8((object + 928)) | 32));
    }
    return state;
}

uint32_t sub_8009DAF8(uint32_t change)
{
    FUNCTION_MARKER(0x8009DAF8u, "RASHCDG.BIN");
    uint32_t value = r_u32(0x800D86F0) + (((int32_t)(uint32_t)(change)) > 0 ? 1u : 0xFFFFFFFF), limit;
    w_u32(0x800D86F0, value);
    limit = r_u32(0x800D86F4);
    value = r_u32(0x800D86F0);
    if (((int32_t)(uint32_t)(value)) >= ((int32_t)(uint32_t)(limit)))
        value = limit;
    w_u32(0x800D86F0, value);
    if (((int32_t)(uint32_t)(value)) < 0)
        value = 0;
    w_u32(0x800D86F0, value);
    return value;
}

uint32_t sub_8002820C(uint32_t object)
{
    FUNCTION_MARKER(0x8002820Cu, "SLUS_010.53");
    uint32_t result = r_u32(object + 36) & 0xC7FFFFFF;
    w_u32(object + 36, result);
    return result;
}

uint32_t sub_8002A738(uint32_t object, uint32_t previous, uint32_t entry, uint32_t next)
{
    FUNCTION_MARKER(0x8002A738u, "SLUS_010.53");
    uint32_t kind = (r_u32(entry) >> 6) & 15, value, shift = 0, mask = 0, result;
    switch (kind)
    {
        case 1:
        case 3:
            shift = 25;
            mask = 3;
            break;
        case 2:
            if (((r_u32(entry) >> 14) & 255) == 2)
            {
                shift = 23;
                mask = 3;
            }
            else
            {
                shift = 19;
                mask = 15;
            }
            break;
        case 4:
            shift = 30;
            mask = 3;
            break;
        case 5:
            (void)sub_8002820C(object);
            break;
        case 7:
            shift = 19;
            mask = 15;
            break;
        default:
            break;
    }
    if (mask)
    {
        value = r_u32(object + 36);
        w_u32(object + 36, (value & ~(mask << shift)) | (((((value >> shift) & mask) - 1) & mask) << shift));
    }
    value = r_u32(entry);
    w_u8(entry + 60, 0);
    result = (value & 0xFFFFC000) | 63;
    w_u32(entry, result);
    if (previous)
    {
        result = (r_u32(previous) & 0xFFFFFFC0) | (next & 63);
        w_u32(previous, result);
    }
    else
        w_u8(object + 73, (uint8_t)next);
    return result;
}

uint32_t sub_8002847C(uint32_t object)
{
    uint32_t scalar_value_34;

    FUNCTION_MARKER(0x8002847Cu, "SLUS_010.53");
    uint32_t index = ((uint32_t)r_u8((object + 73)) ^ 128) - 128, entry = 0x800D39B0 + 112 * index, result = 0xFFFFFFFF, next;
    if (index == 0xFFFFFFFF)
        return result;
    while (entry)
    {
        next = ((scalar_value_34 = (r_u32(entry) << 26)), (((uint32_t)((!((uint32_t)(26))) ? (scalar_value_34) : ((scalar_value_34 >> ((uint32_t)(26))) | ((scalar_value_34 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(26)))) : 0))))));
        (void)sub_8002A738(object, 0, entry, next);
        result = next << 3;
        if (next == 0xFFFFFFFF)
            entry = 0;
        else
        {
            result = (result - next) << 4;
            entry = 0x800D39B0 + result;
        }
    }
    return result;
}

uint32_t sub_80010028(uint32_t numerator, uint32_t denominator)
{
    FUNCTION_MARKER_ARGS(0x80010028u, "SLUS_010.53", 1u, 2u, XPORT_CALL_SCALAR(numerator), XPORT_CALL_SCALAR(denominator));
    uint32_t divisor = denominator >> 1, reciprocal = divisor ? 0x80000000u / divisor : 0xFFFFFFFF;
    return (uint32_t)(((uint64_t)numerator * reciprocal) >> 16);
}

uint32_t sub_8001FF3C(uint32_t input)
{
    uint32_t scalar_value_161;

    uint32_t scalar_value_35;
    uint32_t scalar_shift_35;
    uint32_t scalar_shift_36;

    FUNCTION_MARKER(0x8001FF3Cu, "SLUS_010.53");
    uint32_t negative = ((int32_t)(uint32_t)(input)) < 0, magnitude = negative ? 0u - input : input, result = 1024, base, index = 0, shift, q, address, lo, hi, product;
    if (((int32_t)(uint32_t)(magnitude)) <= 65535)
    {
        base = 65528;
        shift = 0;
        if (((int32_t)(uint32_t)(magnitude)) >= 65528)
            index = 52;
        else
        {
            base = 32768;
            shift = 15;
            while (((int32_t)(uint32_t)(magnitude)) >= ((int32_t)(uint32_t)(base)))
            {
                --shift;
                base += 1u << (shift & 31);
                index += 4;
            }
            base -= 1u << (shift & 31);
            shift -= 2;
        }
        q = ((scalar_value_35 = (magnitude - base)), (scalar_shift_35 = (shift & 31)), (((uint32_t)((!scalar_shift_35) ? (scalar_value_35) : ((scalar_value_35 >> scalar_shift_35) | ((scalar_value_35 & 0x80000000) ? (0xFFFFFFFFu << (32 - scalar_shift_35)) : 0))))));
        base += q << (shift & 31);
        index += q;
        address = 0x800527E0 + 2 * index;
        lo = r_u16(address);
        hi = r_u16(address + 2);
        product = (magnitude - base) * (hi - lo);
        result = ((scalar_value_161 = (((scalar_shift_36 = (shift & 31)), (((uint32_t)((!scalar_shift_36) ? (((uint32_t)(product))) : ((((uint32_t)(product)) >> scalar_shift_36) | ((((uint32_t)(product)) & 0x80000000) ? (0xFFFFFFFFu << (32 - scalar_shift_36)) : 0)))))) + 8 + lo)), (((uint32_t)((!((uint32_t)(4))) ? (scalar_value_161) : ((scalar_value_161 >> ((uint32_t)(4))) | ((scalar_value_161 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(4)))) : 0))))));
    }
    return negative ? 0u - result : result;
}

uint32_t sub_80020018(uint32_t x, uint32_t y)
{
    uint32_t scalar_value_40;

    FUNCTION_MARKER(0x80020018u, "SLUS_010.53");
    uint32_t sign, octant, n, d, ratio, address, base, step, value;
    int64_t product;
    if (!x && !y)
        return 0;
    sign = ((uint32_t)((!((uint32_t)(31))) ? (((uint32_t)(y))) : ((((uint32_t)(y)) >> ((uint32_t)(31))) | ((((uint32_t)(y)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(31)))) : 0))));
    octant = sign & 2;
    if (((int32_t)(uint32_t)(x)) < 0)
        octant |= 4;
    y = (y + sign) ^ sign;
    sign = ((uint32_t)((!((uint32_t)(31))) ? (((uint32_t)(x))) : ((((uint32_t)(x)) >> ((uint32_t)(31))) | ((((uint32_t)(x)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(31)))) : 0))));
    x = (x + sign) ^ sign;
    if (((int32_t)(uint32_t)(y)) < ((int32_t)(uint32_t)(x)))
    {
        octant |= 1;
        n = y;
        d = x;
    }
    else
    {
        n = x;
        d = y;
    }
    ratio = sub_80010028(((int32_t)(uint32_t)(n)) > 0 ? n : 0u - n, ((int32_t)(uint32_t)(d)) > 0 ? d : 0u - d);
    if ((((int32_t)(uint32_t)(n)) > 0) != (((int32_t)(uint32_t)(d)) > 0))
        ratio = 0u - ratio;
    address = 0x8005285C + 4 * ((uint32_t)((!((uint32_t)(12))) ? (((uint32_t)(ratio))) : ((((uint32_t)(ratio)) >> ((uint32_t)(12))) | ((((uint32_t)(ratio)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(12)))) : 0))));
    base = r_u32(address);
    step = ((r_u32(address + 4) - base) << 4) + 8;
    product = (int64_t)(ratio & 4095) * (int64_t)((int32_t)(uint32_t)(step));
    value = ((scalar_value_40 = ((uint32_t)((uint64_t)product >> 16) + base)), (((uint32_t)((!((uint32_t)(20))) ? (scalar_value_40) : ((scalar_value_40 >> ((uint32_t)(20))) | ((scalar_value_40 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(20)))) : 0))))));
    switch (octant)
    {
        case 0:
            return value;
        case 1:
            return 1024 - value;
        case 2:
            return 2048 - value;
        case 3:
            return value + 1024;
        case 4:
            return 0u - value;
        case 5:
            return value - 1024;
        case 6:
            return value - 2048;
        case 7:
            return 0xFFFFFC00 - value;
        default:
            return 0;
    }
}

/* 2E698 typed return effect: MVMVA color row1 * IR vector, sf=0, then MAC1 ASR8. */
uint32_t sub_8002E698(const uint16_t left[3], const uint16_t right[3])
{
    uint32_t first;
    uint32_t second;
    uint32_t third;
    FUNCTION_MARKER(0x8002E698u, "SLUS_010.53");
    first = (uint32_t)(int32_t)(int16_t)xport_load_le16(left);
    second = (uint32_t)(int32_t)(int16_t)xport_load_le16(left + 1);
    third = (uint32_t)(int32_t)(int16_t)xport_load_le16(left + 2);
    xport_gte_write_data(9u, first);
    xport_gte_write_data(10u, second);
    xport_gte_write_data(11u, third);
    first = xport_load_le16(right);
    second = xport_load_le16(right + 1);
    third = (uint32_t)(int32_t)(int16_t)xport_load_le16(right + 2);
    xport_gte_write_control(16u, first | (second << 16));
    xport_gte_write_control(17u, third);
    xport_gte_execute(0x45E012u);
    return (uint32_t)(((int32_t)(uint32_t)(xport_gte_read_data(25u))) >> 8);
}

int64_t sub_8002EE50(uint32_t scale, const uint16_t direction[3], uint32_t output[3])
{
    uint32_t axis;
    int64_t result = 0;
    FUNCTION_MARKER(0x8002EE50u, "SLUS_010.53");
    for (axis = 0; axis < 3; ++axis)
    {
        int32_t component = (int32_t)(int16_t)xport_load_le16(direction + axis) * 16;
        result = ((int64_t)((int32_t)(uint32_t)(scale)) * component) >> 16;
        xport_store_le32(output + axis, (uint32_t)result);
    }
    return result;
}



int64_t sub_8002E810(uint32_t scale, const uint32_t input[3], uint32_t output[3])
{
    uint32_t axis;
    int64_t result = 0;
    FUNCTION_MARKER(0x8002E810, "SLUS_010.53");
    for (axis = 0; axis < 3; ++axis)
    {
        result = sub_8001FC90((int32_t)scale, (int32_t)xport_load_le32(input + axis));
        xport_store_le32(output + axis, (uint32_t)result);
    }
    return result;
}



/* Collision target response; return contract is the caller-observed low word. */
uint32_t sub_8002076C(uint32_t object)
{
    FUNCTION_MARKER(0x8002076Cu, "SLUS_010.53");
    uint32_t target = r_u32(object + 832), tag, kind, record, value, other, dot;
    if (!target)
        return 3;
    tag = r_u16(target);
    kind = tag >> 5;
    if (kind == 0)
    {
        record = r_u32(0x8005B3A0) + 1096 * (tag & 31);
        value = r_u32(record + 560) & 0xFDFFFFFF;
        w_u32(record + 560, value);
        return value;
    }
    if (kind < 3)
        return 1;
    if (kind > 4)
        return 4;
    if (kind == 4)
    {
        record = r_u32(0x800CD6D4) + 596 * (tag & 31);
        value = (r_u16(r_u32(record) + 14) & 0xF80) >> 7;
        if (value - 3 >= 3)
            return 0;
        if (r_u32(record + 592) & 0x200)
            w_u32(record + 556, 0x10000);
        return 0x10000;
    }
    value = r_u32(object + 564) & 0x60000;
    if (value != 0x40000)
        return value;
    other = 0x800CF660 + 512 * (tag & 31);
    dot = sub_8002E698(xport_guest_ptr(other + 450, 6), xport_guest_ptr(object + 450, 6));
    value = r_u32(object + 576) + (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(other + 480))), ((int32_t)(uint32_t)(dot)));
    w_u32(object + 576, value);
    if (((int32_t)(uint32_t)(value)) < 146486)
        value = 146486;
    w_u32(object + 576, value);
    w_u32(object + 480, value);
    return (uint32_t)sub_8002EE50(value, xport_guest_ptr(object + 450, 6), xport_guest_ptr(object + 456, 12));
}

uint32_t sub_8002090C(uint32_t object)
{
    uint32_t scalar_value_41;

    FUNCTION_MARKER(0x8002090Cu, "SLUS_010.53");
    static const uint16_t first[] = {780, 776, 772, 768, 764, 760, 756, 752, 748, 728, 724, 720, 716, 712, 704, 696, 700, 688, 676, 680, 684, 692, 656, 660, 672, 644, 636};
    static const uint16_t second[] = {640, 616, 620, 624, 584, 744, 488, 608, 604, 596, 588, 580, 576, 484};
    static const uint16_t halves[] = {846, 844, 842, 840, 838, 836, 826};
    uint32_t config = r_u32(object + 556), value, n, d, positive_n, positive_d, angle, x, y, z, linked;
    unsigned i;
    if (r_u32(object + 832))
        (void)sub_8002076C(object);
    value = r_u32(object + 560);
    for (i = 0; i < sizeof(first) / sizeof(first[0]); ++i)
        w_u32(object + first[i], 0);
    w_u32(object + 560, value & 0xF8000000);
    w_u32(object + 564, r_u32(object + 564) & 0xC0000000);
    w_u32(object + 568, r_u32(object + 568) & 0xF7B00000);
    for (i = 0; i < sizeof(second) / sizeof(second[0]); ++i)
        w_u32(object + second[i], 0);
    n = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(config + 12))), 643207);
    positive_n = ((int32_t)(uint32_t)(n)) > 0;
    positive_d = ((int32_t)(uint32_t)(r_u32(config + 16))) > 0;
    n = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(config + 12))), 643207);
    d = r_u32(config + 16);
    value = sub_80010028(positive_n ? n : 0u - n, positive_d ? d : 0u - d);
    if (positive_n != positive_d)
        value = 0u - value;
    w_u32(object + 600, value);
    value = r_u32(config + 188);
    angle = (uint32_t)r_s16((object + 530));
    w_u8(object + 849, 0);
    w_u32(object + 828, 0);
    w_u32(object + 932, 0);
    w_u32(object + 708, 0);
    for (i = 0; i < sizeof(halves) / sizeof(halves[0]); ++i)
        w_u16(object + halves[i], 0);
    w_u32(object + 592, value);
    value = sub_8001FF3C(angle << 4);
    x = (uint32_t)r_s16((object + 518));
    w_u16(object + 842, value);
    x *= 157;
    y = (uint32_t)r_s16((object + 524));
    w_u32(object + 732, x);
    y *= 157;
    z = (uint32_t)r_s16((object + 530));
    w_u32(object + 736, y);
    w_u32(object + 740, z * 157);
    value = sub_80020018(x, y);
    linked = r_u32(object + 856);
    w_u32(object + 668, 0u - ((scalar_value_41 = (value * 25736)), (((uint32_t)((!((uint32_t)(8))) ? (scalar_value_41) : ((scalar_value_41 >> ((uint32_t)(8))) | ((scalar_value_41 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(8)))) : 0)))))));
    if (linked && r_u32(object + 1088))
        w_u32(linked + 488, 0);
    value = r_u32(object + 668) + r_u32(object + 636);
    w_u32(object + 652, value);
    return value;
}

uint32_t sub_80012838(uint32_t object, uint32_t child, uint32_t kind, uint32_t index)
{
    FUNCTION_MARKER(0x80012838u, "SLUS_010.53");
    uint32_t entry = object + (index << 3);
    w_u8(child + 72, 1);
    w_u32(entry + 56, child);
    w_u32(entry + 60, kind);
    w_u32(child + 52, object);
    return 1;
}

uint32_t sub_80086AF8(uint32_t object)
{
    FUNCTION_MARKER(0x80086AF8u, "RASHCDG.BIN");
    uint32_t value = r_u32(r_u32(object + 568) + 480);
    w_u32(object + 736, 0);
    w_u32(object + 720, 0);
    w_u32(object + 732, 0);
    w_u32(object + 708, 0);
    w_u32(object + 724, value);
    return value;
}

uint32_t sub_800235B0(uint32_t index, uint32_t object)
{
    FUNCTION_MARKER(0x800235B0u, "SLUS_010.53");
    w_u32(0x80053478 + (index << 7) + 4, object);
    return 0x80053478;
}

uint32_t sub_8001E100(uint32_t destination, uint32_t value, uint32_t length)
{
    FUNCTION_MARKER(0x8001E100u, "SLUS_010.53");
    uint32_t address = destination, word = value | (value << 8) | (value << 16) | (value << 24);
    while (length)
    {
        w_u32(address, word);
        length -= 4;
        address += 4;
    }
    return destination;
}

uint32_t sub_800BCD10(uint32_t object)
{
    FUNCTION_MARKER(0x800BCD10u, "RASHCDG.BIN");
    uint32_t length = (((uint32_t)r_u8((object + 946)) ^ 128) - 128) << 3;
    uint32_t result = sub_8001E100(object + 956, 0, length);
    w_u8(object + 946, 0);
    return result;
}

uint32_t sub_80068D20(uint32_t object, uint32_t expected, uint32_t index)
{
    FUNCTION_MARKER(0x80068D20u, "RASHCDG.BIN");
    uint32_t entry = object + (index << 3), child = r_u32(entry + 56);
    if (child == expected)
    {
        w_u8(child + 72, 0);
        w_u32(entry + 56, 0);
        w_u32(entry + 60, 0);
        w_u32(child + 52, 0);
    }
    return child;
}

uint32_t sub_80095AEC(uint32_t object)
{
    FUNCTION_MARKER(0x80095AECu, "RASHCDG.BIN");
    uint32_t slot = ((uint32_t)r_u8((object + 571)) ^ 128) - 128, bit = 1u << (slot & 31), flags = r_u32(0x8005AD50), attachment;
    if (!(flags & bit))
        return ~bit;
    w_u32(0x8005AD50, flags & ~bit);
    slot = ((uint32_t)r_u8((object + 571)) ^ 128) - 128;
    (void)sub_80068D20(object, 0x800CF018 + 172 * slot, 0);
    attachment = r_u32(object + 556);
    if (attachment)
    {
        w_u32(attachment + 36, 0);
        w_u32(0x800CE178, r_u32(0x800CE178) - 1);
    }
    if ((uint32_t)r_u8((object + 572)) & 128)
    {
        slot = ((uint32_t)r_u8((object + 571)) ^ 128) - 128;
        (void)sub_8002847C(0x800CF018 + 172 * slot);
        w_u8(object + 572, (uint8_t)((uint32_t)r_u8((object + 572)) & 127));
    }
    w_u32(object + 556, 0);
    w_u8(object + 571, 255);
    return 0xFFFFFFFF;
}

uint32_t sub_80018440(uint32_t index, uint32_t pending)
{
    FUNCTION_MARKER(0x80018440u, "SLUS_010.53");
    uint32_t offset = index * 132, base = r_u32(0x8005B42C), slot = base + offset;
    if (!base)
        return offset;
    if (pending)
    {
        w_u32(slot + 124, 7);
        return 7;
    }
    (void)sub_8001F7EC(r_u32(slot + 36));
    (void)sub_8001F7EC(r_u32(slot + 40));
    w_u32(slot + 84, 0xFFFFFFFF);
    w_u32(slot + 88, 0xFFFFFFFF);
    w_u32(slot + 124, 0);
    return 0xFFFFFFFF;
}

uint32_t sub_8007EC30(uint32_t object)
{
    FUNCTION_MARKER(0x8007EC30u, "RASHCDG.BIN");
    uint32_t piece = r_u32(object + 340), linked, values[9], i;
    uint32_t diagonal[3], direction[3], product[3];
    for (i = 0; i < 3; ++i)
        values[i] = r_u16(object + 450 + 2 * i);
    for (i = 0; i < 3; ++i)
        w_u16(object + 528 + 2 * i, values[i]);
    for (i = 0; i < 3; ++i)
        w_u16(object + 522 + 2 * i, 0u - r_u16(piece + 8 + 2 * i));
    diagonal[0] = (uint32_t)(int32_t)r_s16(object + 522);
    diagonal[1] = (uint32_t)(int32_t)r_s16(object + 522 + 2);
    xport_gte_write_control(0, diagonal[0]);
    diagonal[2] = (uint32_t)(int32_t)r_s16(object + 522 + 4);
    xport_gte_write_control(2, diagonal[1]);
    xport_gte_write_control(4, diagonal[2]);
    direction[0] = (uint32_t)(int32_t)r_s16(object + 528);
    direction[1] = (uint32_t)(int32_t)r_s16(object + 528 + 2);
    direction[2] = (uint32_t)(int32_t)r_s16(object + 528 + 4);
    xport_gte_write_data(9, direction[0]);
    xport_gte_write_data(10, direction[1]);
    xport_gte_write_data(11, direction[2]);
    xport_gte_execute(0x178000C);
    product[0] = xport_gte_read_data(9);
    product[1] = xport_gte_read_data(10);
    product[2] = xport_gte_read_data(11);
    w_u16(object + 516, (uint16_t)product[0]);
    w_u16(object + 516 + 2, (uint16_t)product[1]);
    w_u16(object + 516 + 4, (uint16_t)product[2]);
    for (i = 0; i < 9; ++i)
        values[i] = r_u16(object + 516 + 2 * i);
    linked = r_u32(object + 856);
    w_u32(object + 676, 0);
    w_u32(object + 616, 0);
    w_u32(object + 636, 0);
    w_u32(object + 672, 0);
    w_u32(object + 488, 0);
    for (i = 0; i < 9; ++i)
        w_u16(object + 432 + 2 * i, values[i]);
    for (i = 0; i < 3; ++i)
        w_u16(object + 814 + 2 * i, values[i]);
    if (linked)
        w_u32(linked + 488, 0);
    return values[0];
}

uint32_t sub_8003B4B0(uint32_t table, uint32_t id)
{
    FUNCTION_MARKER(0x8003B4B0u, "SLUS_010.53");
    uint32_t count, i, address;
    if ((uint32_t)r_s16(0x800D6182) == 0xFFFFFFFF || !table)
        return 0;
    count = r_u32(table + 12);
    if (((int32_t)(uint32_t)(count)) <= 0)
        return 0;
    for (i = 0, address = table + 20; i < count; ++i, address += 16)
        if (r_u32(address) == id)
            return address;
    return 0;
}

uint32_t sub_8003B8F4(uint32_t record)
{
    FUNCTION_MARKER(0x8003B8F4u, "SLUS_010.53");
    uint32_t table, id;
    if (!record)
        return 0;
    table = r_u32(record + 256);
    if (!table)
        return 0;
    id = r_u32(record + 188);
    if ((id >> 16) == 1)
        return (id & 65535) == r_u32(table);
    return sub_8003B4B0(table, id & 65535) != 0;
}

uint32_t sub_80039AFC(uint32_t index)
{
    FUNCTION_MARKER(0x80039AFCu, "SLUS_010.53");
    uint32_t header, record;
    if (((int32_t)(uint32_t)(index)) < 0)
        return 0;
    header = r_u32(0x8005B240);
    if (((int32_t)(uint32_t)(index)) >= ((int32_t)(uint32_t)((uint32_t)r_s16((header + 44)))))
        return 0;
    record = r_u32(header + 36) + 104 * index;
    return (uint32_t)r_s16(record) == index ? record : 0;
}

uint32_t sub_8003A37C(uint32_t record, uint32_t id)
{
    FUNCTION_MARKER(0x8003A37Cu, "SLUS_010.53");
    uint32_t count, i, address;
    if (!record)
        return 0;
    count = (uint32_t)r_s16((record + 2));
    if (((int32_t)(uint32_t)(count)) <= 0 || ((int32_t)(uint32_t)(count)) >= 5)
        return 0;
    for (i = 0, address = record + 8; i < count; ++i, address += 24)
        if ((uint32_t)r_s16((address + 4)) == id)
            return address;
    return 0;
}

uint32_t sub_8002E14C(uint32_t output[3])
{
    uint32_t vector[3], magnitude[3], maximum, sign, bit, shift, sum = 0u;
    uint32_t table, entry, expanded, scale, axis;
    int64_t product;
    FUNCTION_MARKER(0x8002E14Cu, "SLUS_010.53");
    for (axis = 0; axis < 3; ++axis)
        vector[axis] = xport_load_le32((const uint8_t *)output + 4u * axis);
    for (axis = 0; axis < 3; ++axis)
    {
        sign = (uint32_t)((int32_t)vector[axis] >> 31);
        magnitude[axis] = (sign + vector[axis]) ^ sign;
    }
    maximum = (int32_t)magnitude[1] < (int32_t)magnitude[0] ? magnitude[0] : magnitude[1];
    if ((int32_t)magnitude[2] >= (int32_t)maximum)
        maximum = magnitude[2];
    bit = 0u;
    if (maximum)
    {
        xport_gte_write_data(30u, maximum);
        bit = 31u - xport_gte_read_data(31u);
    }
    if ((int32_t)bit < 21)
    {
        shift = (20u - bit) & 31u;
        for (axis = 0; axis < 3; ++axis)
            vector[axis] <<= shift;
    }
    else
    {
        shift = (bit - 20u) & 31u;
        for (axis = 0; axis < 3; ++axis)
            vector[axis] = (uint32_t)((int32_t)vector[axis] >> shift);
    }
    for (axis = 0; axis < 3; ++axis)
    {
        product = (int64_t)(int32_t)vector[axis] * (int32_t)vector[axis];
        sum += (uint32_t)((uint64_t)product >> 16);
    }
    if ((int32_t)sum <= 0)
        return sum;
    xport_gte_write_data(30u, sum);
    shift = xport_gte_read_data(31u);
    table = r_u32(0x8005B560u);
    shift = 22u - (shift & ~1u);
    if ((int32_t)shift <= 0)
        shift = 0u;
    entry = r_u16(table + 2u * (sum >> (shift & 31u)));
    expanded = (entry >> 5) << (entry & 31u);
    scale = (uint32_t)((int32_t)expanded >> ((shift >> 1) & 31u));
    for (axis = 0; axis < 3; ++axis)
    {
        product = (int64_t)(int32_t)vector[axis] * (int32_t)scale;
        xport_store_le32((uint8_t *)output + 4u * axis, (uint32_t)((uint64_t)product >> 16));
    }
    return expanded;
}

uint32_t sub_8002E468(uint16_t output[3])
{
    int32_t vector[3];
    uint32_t sum, shift, entry, expanded, scale, axis;
    uint64_t squared = 0;
    FUNCTION_MARKER(0x8002E468u, "SLUS_010.53");
    for (axis = 0; axis < 3; ++axis)
    {
        vector[axis] = (int16_t)xport_load_le16((const uint8_t *)output + 2u * axis);
        xport_gte_write_data(9u + axis, (uint32_t)vector[axis]);
    }
    xport_gte_execute(0xA00428u);
    for (axis = 0; axis < 3; ++axis)
        squared += xport_gte_read_data(25u + axis);
    /* Preserve the original signed ADD overflow boundary */
    if (squared > 0x7FFFFFFFu)
        abort();
    sum = (uint32_t)squared;
    xport_gte_write_data(30u, sum);
    shift = 22u - (xport_gte_read_data(31u) & ~1u);
    if ((int32_t)shift <= 0)
        shift = 0;
    entry = r_u16(r_u32(0x8005B560u) + 2u * (sum >> shift));
    expanded = (entry >> 5) << (entry & 31u);
    scale = (uint32_t)((int32_t)expanded >> (shift >> 1));
    for (axis = 0; axis < 3; ++axis)
    {
        uint32_t product = (uint32_t)vector[axis] * scale;
        xport_store_le16((uint8_t *)output + 2u * axis, (uint16_t)((int32_t)product >> 12));
    }
    return sum;
}

/* 8004CF74: PsyQ table square root; signed LZCS and signed table loads. */
uint32_t sub_8004CF74(uint32_t input)
{
    uint32_t scalar_value_49;
    uint32_t scalar_shift_50;

    FUNCTION_MARKER(0x8004CF74u, "SLUS_010.53");
    uint32_t leading, even, shift, normalized, value;

    xport_gte_write_data(30u, input);
    leading = xport_gte_read_data(31u);
    if (leading == 32)
        return 0;
    even = leading & ~1u;
    shift = ((scalar_value_49 = (19 - even)), (((uint32_t)((!((uint32_t)(1))) ? (scalar_value_49) : ((scalar_value_49 >> ((uint32_t)(1))) | ((scalar_value_49 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(1)))) : 0))))));
    normalized = even >= 24 ? input << ((even - 24) & 31) : ((scalar_shift_50 = (24 - even)), (((uint32_t)((!scalar_shift_50) ? (((uint32_t)(input))) : ((((uint32_t)(input)) >> scalar_shift_50) | ((((uint32_t)(input)) & 0x80000000) ? (0xFFFFFFFFu << (32 - scalar_shift_50)) : 0))))));
    value = (uint32_t)r_s16((0x800560CC + 2 * (normalized - 64)));
    return ((int32_t)(uint32_t)(shift)) < 0 ? value >> ((0u - shift) & 31) : value << (shift & 31);
}

uint32_t sub_80039AA0(uint32_t index)
{
    FUNCTION_MARKER(0x80039AA0u, "SLUS_010.53");
    uint32_t header = r_u32(0x8005B240), relative = index - (uint32_t)r_s16((header + 44)), address;
    if (((int32_t)(uint32_t)(index)) < 0 || ((int32_t)(uint32_t)(relative)) >= ((int32_t)(uint32_t)((uint32_t)r_s16((header + 42)))))
        return 0;
    address = r_u32(header + 32) + (relative << 3);
    return (uint32_t)r_s16(address) == index ? address : 0;
}

uint32_t sub_800394F0(uint32_t track[8], uint32_t direction)
{
    uint32_t root, lookup, point, target;
    int32_t segment;
    FUNCTION_MARKER(0x800394F0, "SLUS_010.53");
    if (!track[0])
        return 1;
    if (r_s16(track[1] + 2) == 1)
        return 0;
    segment = r_s16(track[3]);
    if (segment || (int32_t)direction >= 0)
    {
        if (segment < r_s16(track[2] + 10) - 1 || (int32_t)direction <= 0)
            return 0;
    }
    root = track[0];
    if (!r_s16(root + 16))
    {
        lookup = sub_80039AA0(r_u32(root));
        if (!lookup)
            return 1;
        target = (uint32_t)(int32_t)r_s16(lookup + 4);
        if ((int32_t)direction > 0)
            target = (uint32_t)(int32_t)r_s16(lookup + 2);
    }
    else
    {
        lookup = sub_80039AFC(r_u32(root));
        if (!lookup)
            return 1;
        point = sub_8003A37C(lookup, r_u32(track[1] + 12));
        if (!point)
            return 1;
        if (r_u32(root + 12) == 1)
        {
            if ((int32_t)direction > 0)
                target = (uint32_t)(int32_t)(r_s16(point + 2) > 0 ? r_s16(point) : r_s16(lookup + 6));
            else
                target = (uint32_t)(int32_t)(r_s16(point + 2) <= 0 ? r_s16(point) : r_s16(lookup + 6));
        }
        else
        {
            if ((int32_t)(direction ^ (uint32_t)(int32_t)r_s16(point + 2)) < 0)
                return 0;
            target = (uint32_t)(int32_t)r_s16(lookup + 6);
        }
    }
    if (target == UINT32_MAX)
        return 1;
    lookup = sub_80039A08(target);
    if (!lookup)
        return 1;
    return r_u32(lookup + 12) == 0;
}

uint32_t sub_8003662C(const uint16_t direction[3], const uint32_t track[8], uint32_t output[3])
{
    uint32_t record, dot, prior, sign, magnitude, value, progress, root, segment, last, limit;
    uint16_t tangent[3];
    uint32_t i;
    FUNCTION_MARKER(0x8003662C, "SLUS_010.53");
    record = track[1];
    output[0] = ((uint32_t)r_u16(record + 2) << 16) | r_u16(record + 12);
    for (i = 0; i < 3; ++i)
        tangent[i] = r_u16(track[3] + 14 + 2 * i);
    dot = sub_8002E698(direction, tangent);
    prior = output[1];
    sign = (uint32_t)((int32_t)prior >> 31);
    magnitude = (prior + sign) ^ sign;
    if ((int32_t)dot >= 0)
        value = (magnitude == 5 ? 3 : 1) + ((int32_t)prior < 1);
    else
        value = (magnitude == 5 ? 0xFFFFFFFD : 0xFFFFFFFF) - ((int32_t)prior >= 0);
    output[1] = value;
    progress = r_u32(track[3] + 40) + track[5];
    output[2] = progress;
    if (r_s16(track[1] + 2))
        return 1;
    root = track[0];
    if (r_s16(root + 16) == 1 && !r_u32(root + 12))
    {
        value = (int32_t)progress < 0 ? 0 : progress;
        output[2] = value;
        record = track[1];
        if (r_u32(record + 24))
        {
            limit = r_u32(record + 28);
            if ((int32_t)limit < (int32_t)value)
                output[2] = limit;
        }
    }
    segment = (uint32_t)r_s16(track[3]);
    if (segment)
    {
        last = (uint32_t)r_s16(track[2] + 10) - 1;
        if (segment != last)
            return last;
    }
    value = sub_800394F0((uint32_t *)track, output[1]);
    if (!value)
        return 0;
    record = track[1];
    value = output[2];
    limit = r_u32(record + 28);
    if ((int32_t)limit < (int32_t)value)
        value = limit;
    output[2] = value;
    if ((int32_t)value < 0)
        value = 0;
    output[2] = value;
    return value;
}

uint32_t sub_80096564(uint32_t object)
{
    FUNCTION_MARKER(0x80096564u, "RASHCDG.BIN");
    uint32_t flip = 0, selected, secondary, id, dot, record, route, lookup, point, vector[3], i;
    uint32_t diagonal[3], direction[3], product[3];
    if (sub_8003B8F4(object + 172))
    {
        secondary = r_u32(object + 852);
        selected = r_u32(secondary + 604) >= 3 ? secondary : object;
        id = r_u32(selected + 360);
        if (!(id >> 16))
        {
            dot = sub_8002E698(xport_guest_ptr(r_u32(selected + 340) + 14, 6), xport_guest_ptr(object + 528, 6));
            record = sub_8003B4B0(r_u32(selected + 428), r_u16(selected + 360));
            if (record && ((int32_t)(uint32_t)(r_u32(record + 4))) < 0 && ((int32_t)(uint32_t)(dot)) > 0)
                flip = 1;
        }
        else
        {
            route = sub_8003F3B4(id & 65535);
            lookup = sub_80039AFC(r_u32(r_u32(selected + 328)));
            if (route && lookup)
            {
                point = sub_8003A37C(lookup, r_u32(route + 84));
                if (point)
                {
                    for (i = 0; i < 3; ++i)
                        vector[i] = r_u32(point + 12 + 4 * i) - r_u32(selected + 184 + 4 * i);
                    (void)sub_8002E14C(vector);
                    for (i = 0; i < 3; ++i)
                        w_u16(object + 528 + 2 * i, (uint16_t)((int32_t)vector[i] >> 4));
                    diagonal[0] = (uint32_t)(int32_t)r_s16(object + 522);
                    diagonal[1] = (uint32_t)(int32_t)r_s16(object + 522 + 2);
                    xport_gte_write_control(0, diagonal[0]);
                    diagonal[2] = (uint32_t)(int32_t)r_s16(object + 522 + 4);
                    xport_gte_write_control(2, diagonal[1]);
                    xport_gte_write_control(4, diagonal[2]);
                    direction[0] = (uint32_t)(int32_t)r_s16(object + 528);
                    direction[1] = (uint32_t)(int32_t)r_s16(object + 528 + 2);
                    direction[2] = (uint32_t)(int32_t)r_s16(object + 528 + 4);
                    xport_gte_write_data(9, direction[0]);
                    xport_gte_write_data(10, direction[1]);
                    xport_gte_write_data(11, direction[2]);
                    xport_gte_execute(0x178000C);
                    product[0] = xport_gte_read_data(9);
                    product[1] = xport_gte_read_data(10);
                    product[2] = xport_gte_read_data(11);
                    w_u16(object + 516, (uint16_t)product[0]);
                    w_u16(object + 516 + 2, (uint16_t)product[1]);
                    w_u16(object + 516 + 4, (uint16_t)product[2]);
                    (void)sub_8002E468(xport_guest_ptr(object + 516, 6));
                    diagonal[0] = (uint32_t)(int32_t)r_s16(object + 516);
                    diagonal[1] = (uint32_t)(int32_t)r_s16(object + 516 + 2);
                    xport_gte_write_control(0, diagonal[0]);
                    diagonal[2] = (uint32_t)(int32_t)r_s16(object + 516 + 4);
                    xport_gte_write_control(2, diagonal[1]);
                    xport_gte_write_control(4, diagonal[2]);
                    direction[0] = (uint32_t)(int32_t)r_s16(object + 522);
                    direction[1] = (uint32_t)(int32_t)r_s16(object + 522 + 2);
                    direction[2] = (uint32_t)(int32_t)r_s16(object + 522 + 4);
                    xport_gte_write_data(9, direction[0]);
                    xport_gte_write_data(10, direction[1]);
                    xport_gte_write_data(11, direction[2]);
                    xport_gte_execute(0x178000C);
                    product[0] = xport_gte_read_data(9);
                    product[1] = xport_gte_read_data(10);
                    product[2] = xport_gte_read_data(11);
                    w_u16(object + 528, (uint16_t)product[0]);
                    w_u16(object + 528 + 2, (uint16_t)product[1]);
                    w_u16(object + 528 + 4, (uint16_t)product[2]);
                }
            }
        }
    }
    else if (r_u32(object + 568) & 0x400000)
        flip = 1;
    if (flip)
    {
        for (i = 0; i < 3; ++i)
            w_u16(object + 528 + 2 * i, 0u - r_u16(object + 528 + 2 * i));
        for (i = 0; i < 3; ++i)
            w_u16(object + 516 + 2 * i, 0u - r_u16(object + 516 + 2 * i));
    }
    return sub_8003662C(xport_guest_cptr(object + 528, 6), xport_guest_cptr(object + 328, 32), xport_guest_ptr(object + 360, 12));
}

/* Overlay800C2FF4: commit state class, frame stamp and animation flags. */
uint32_t sub_800C2FF4(uint32_t type, uint32_t object, uint32_t flags)
{
    FUNCTION_MARKER(0x800C2FF4u, "RASHCDG.BIN");
    uint32_t kind, state, global, value, stamp;
    if ((r_u16(object + 172) >> 5) != 1)
    {
        value = r_u32(object + 552);
        w_u16(object + 544, type);
        w_u32(object + 552, ((value | 0x20000000u) & 0xffff00ffu) | 0x300u);
        return 0xffff00ffu;
    }
    kind = r_u16(0x800541D4 + 8 * (type & 65535u) + 2);
    state = kind == 0 ? 0u : kind < 5 ? 1u : kind == 5 ? 2u : kind == 6 ? 3u : 4u;
    global = r_u32(0x8005B2F8);
    value = r_u32(object + 552);
    w_u32(object + 604, state);
    w_u16(object + 544, type);
    stamp = r_u32(global + 16);
    value |= 0x02000000u;
    w_u32(object + 552, value);
    w_u32(object + 548, stamp);
    value = (state - 2u) < 2u ? value | 0x20000000u : value & 0xdfffffffu;
    w_u32(object + 552, value);
    value = r_u32(object + 552);
    value = (flags & 0x100u) ? value | 0x08000000u : value & 0xf7ffffffu;
    w_u32(object + 552, value);
    w_u8(object + 546, 0);
    return value;
}

/* Overlay8005BE0C: clear animation flag bit1, return the stored word. */
uint32_t sub_8005BE0C(uint32_t animation)
{
    FUNCTION_MARKER(0x8005BE0Cu, "RASHCDG.BIN");
    uint32_t value = r_u32(animation + 36) & 0xfffffffdu;
    w_u32(animation + 36, value);
    return value;
}

/* Resident80012858: bind animation set, preserving pointer reloads. */
uint32_t sub_80012858(uint32_t object, uint32_t set)
{
    FUNCTION_MARKER(0x80012858u, "SLUS_010.53");
    uint32_t header = r_u32(object), animation;
    w_u32(object + 40, set);
    w_u16(object + 48, 0);
    xport_update_u8(header + 9, XPORT_MEMORY_UPDATE_OR, 4);
    animation = r_u32(object + 4);
    w_u8(animation + 1, 5);
    return 5;
}

/* Overlay8005BEF4: test whether the animation transition may proceed. */
uint32_t sub_8005BEF4(uint32_t object)
{
    FUNCTION_MARKER(0x8005BEF4u, "RASHCDG.BIN");
    uint32_t animation = r_u32(object + 4);
    if (r_u8(animation + 1) == 5)
        return 1;
    if (r_u32(object + 12) == 1 && r_u8(animation + 13) == 5)
        return 1;
    if (r_u8(animation + 1) >= 2)
        return 0;
    if (r_u8(animation + 13) == 3)
        return 0;
    return r_u32(object + 12) == 0;
}

/* Overlay8005BE58: animation completion, signed frame comparison. */
uint32_t sub_8005BE58(uint32_t object)
{
    FUNCTION_MARKER(0x8005BE58u, "RASHCDG.BIN");
    uint32_t animation, record, type;
    if (!(r_u32(object + 36) & 2u))
        return 1;
    record = 12u * r_u32(object + 12);
    animation = r_u32(object + 4);
    record += animation;
    type = r_u8(record + 1);
    if (type == 5)
        return 1;
    if (type != 1)
        return 0;
    if (r_u8(animation + 1) == 3)
        return 0;
    if (r_u8(animation + 13) == 3)
        return 0;
    return ((int32_t)(uint32_t)(r_u32(object + 16))) >= ((int32_t)(r_u16(record + 6) ^ 32768u) - 32768);
}

/* Overlay8005C418: resolve jump/advance animation records with original bound. */
uint32_t sub_8005C418(uint32_t object)
{
    FUNCTION_MARKER(0x8005C418u, "RASHCDG.BIN");
    uint32_t animation = r_u32(object + 4), index = r_u32(object + 12);
    uint32_t count = r_u32(object + 8), attempt = index, done = 0;
    for (;;)
    {
        uint32_t record = animation + 12u * index;
        uint32_t type = r_u8(record + 1), prior;
        switch (type)
        {
            case 0:
            case 1:
            case 3:
                done = 1;
                break;
            case 2:
                index = (uint32_t)((int32_t)(r_u16(record + 4) ^ 32768u) - 32768);
                break;
            case 4:
                ++index;
                if (index == count)
                    index = 0;
                break;
            case 5:
                done = 1;
                w_u32(object + 36, r_u32(object + 36) & 0xfffffffdu);
                break;
            default:
                break;
        }
        prior = attempt++;
        if (((int32_t)(uint32_t)(count)) < ((int32_t)(uint32_t)(prior)))
        {
            w_u32(object + 36, r_u32(object + 36) & 0xfffffffdu);
            return index;
        }
        if (done)
            return index;
    }
}

/* Overlay8005E2BC: initialize raw/compressed animation channel caches. */
uint32_t sub_8005E2BC(uint32_t object, uint32_t output)
{
    uint32_t scalar_value_54;

    FUNCTION_MARKER(0x8005E2BCu, "RASHCDG.BIN");
    uint32_t dest = output + 4, source = r_u32(object + 44), data = source + 24;
    uint32_t remaining, stride, channels, i, p, value, result;
    w_u16(output + 2, 4u * r_u8(source + 15) + 3);
    remaining = r_u32(source + 4);
    stride = 2u * r_u16(source + 16);
    channels = 4u * r_u8(source + 15);
    w_u16(output, 0xffff);
    remaining -= 24;
    if (r_u8(source + 13) & 1)
    {
        p = output + 10;
        for (i = 0; i < 3; ++i)
        {
            remaining -= stride;
            xport_update_u8(p + 14, XPORT_MEMORY_UPDATE_AND, 0xfd);
            w_u32(dest, data);
            value = r_u16(data);
            dest += 24;
            w_u16(p - 2, value);
            w_u16(p, value);
            data += 2u * r_u16(source + 16);
            p += 24;
        }
    }
    else
    {
        p = output + 8;
        for (i = 0; i < 3; ++i)
        {
            dest += 24;
            value = r_u8(p + 16);
            w_u16(p + 2, 0);
            w_u16(p, 0);
            w_u8(p + 16, (uint8_t)((value & 0xfc) | 2));
            p += 24;
        }
    }
    result = ((int32_t)(uint32_t)(remaining)) < 3;
    if (r_u8(source + 13) & 2)
    {
        if (result)
            return 0x800d0000;
        p = dest + 6;
        do
        {
            value = r_u8(p + 14) | 2;
            w_u8(p + 14, (uint8_t)value);
            value = (value & 0xfe) | (r_u16(data + 2) >> 15);
            w_u8(p + 14, (uint8_t)value);
            value = r_u16(data + 2) & 0x7fff;
            w_u32(p + 2, value);
            if (value & 0x4000)
                w_u32(p + 2, value | 0xffff8000);
            if (r_u8(p + 14) & 1)
            {
                uint32_t product, bits, table;
                value = r_u16(data + 4) & 0x7fff;
                w_u16(p + 10, value);
                product = r_u32(p + 2) * value;
                bits = (r_u16(data + 6) & 0x7fff) >> 8;
                w_u8(p + 16, (uint8_t)bits);
                w_u16(p - 2, ((uint32_t)((!((uint32_t)(9))) ? (((uint32_t)(product))) : ((((uint32_t)(product)) >> ((uint32_t)(9))) | ((((uint32_t)(product)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(9)))) : 0)))));
                w_u32(dest, data + 6);
                w_u8(p + 17, 8);
                value = r_u8(p + 14);
                bits = r_u8(p + 16);
                w_u32(p + 6, 0);
                w_u8(p + 12, 0);
                w_u8(p + 14, (uint8_t)(value & 0xfb));
                table = r_u32(0x800CC654 + 4 * bits);
                value = r_u8(table);
                w_u8(p + 13, (uint8_t)((value - 1) & ((1u << (bits & 31)) - 1)));
            }
            else
            {
                value = r_u32(p + 2) << 6;
                w_u16(p - 2, value);
                w_u16(p, value);
            }
            value = r_u16(p - 2);
            dest += 24;
            w_u16(p, value);
            value = ((scalar_value_54 = (r_u16(data) << 16)), (((uint32_t)((!((uint32_t)(16))) ? (scalar_value_54) : ((scalar_value_54 >> ((uint32_t)(16))) | ((scalar_value_54 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(16)))) : 0))))));
            p += 24;
            remaining -= 2u * (value + 1);
            data += 2u * (value + 1);
            result = ((int32_t)(uint32_t)(remaining)) < 3;
        } while (!result);
    }
    else if (channels)
    {
        p = dest + 6;
        for (i = 0; i < channels; ++i)
        {
            xport_update_u8(p + 14, XPORT_MEMORY_UPDATE_AND, 0xfd);
            w_u32(dest, data);
            value = r_u16(data);
            dest += 24;
            w_u16(p - 2, value);
            w_u16(p, value);
            data += 2u * r_u16(source + 16);
            p += 24;
        }
        result = 0;
    }
    return result;
}

uint32_t sub_8005C39C(uint32_t object, uint32_t id)
{
    FUNCTION_MARKER(0x8005C39Cu, "RASHCDG.BIN");
    uint32_t cached = r_u32(object + 44);
    id &= 255;
    if (!cached || r_u8(cached + 14) != id || r_u16(object + 48) != 65535)
    {
        uint32_t table = r_u32(r_u32(object + 40) + 4);
        w_u32(object + 44, r_u32(table + 4 * id));
        (void)sub_8005E2BC(object, object + 48);
    }
    return r_u32(object + 44);
}

/* Overlay800714FC: Q14 quaternion product, wrapped32 sums before ASR14. */
uint32_t rrj_pose_quaternion_product(const uint16_t left[4], const uint16_t right[4], uint16_t output[4])
{
    uint32_t scalar_value_55;
    uint32_t scalar_value_56;
    uint32_t scalar_value_57;

    uint32_t a[4], b[4], r[4], i;
    for (i = 0; i < 4; ++i)
    {
        a[i] = ((scalar_value_55 = ((uint32_t)left[i] << 16)), (((uint32_t)((!((uint32_t)(16))) ? (scalar_value_55) : ((scalar_value_55 >> ((uint32_t)(16))) | ((scalar_value_55 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(16)))) : 0))))));
        b[i] = ((scalar_value_56 = ((uint32_t)right[i] << 16)), (((uint32_t)((!((uint32_t)(16))) ? (scalar_value_56) : ((scalar_value_56 >> ((uint32_t)(16))) | ((scalar_value_56 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(16)))) : 0))))));
    }
    r[3] = a[3] * b[3] - a[0] * b[0] - a[1] * b[1] - a[2] * b[2];
    r[0] = a[3] * b[0] + a[0] * b[3] + a[1] * b[2] - a[2] * b[1];
    r[1] = a[3] * b[1] + a[1] * b[3] + a[2] * b[0] - a[0] * b[2];
    r[2] = a[3] * b[2] + a[2] * b[3] + a[0] * b[1] - a[1] * b[0];
    for (i = 0; i < 4; ++i)
        output[i] = (uint16_t)((scalar_value_57 = (r[i])), (((uint32_t)((!((uint32_t)(14))) ? (scalar_value_57) : ((scalar_value_57 >> ((uint32_t)(14))) | ((scalar_value_57 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(14)))) : 0))))));
    return output[3];
}

uint32_t sub_800714FC(uint32_t left, uint32_t right, uint32_t output)
{
    FUNCTION_MARKER(0x800714FCu, "RASHCDG.BIN");
    uint16_t a[4], b[4], result[4];
    uint32_t i, value;
    for (i = 0; i < 4; ++i)
    {
        a[i] = r_u16(left + 2 * i);
        b[i] = r_u16(right + 2 * i);
    }
    value = rrj_pose_quaternion_product(a, b, result);
    for (i = 0; i < 4; ++i)
        w_u16(output + 2 * i, result[i]);
    return value;
}

static uint32_t pose_joint_block(uint32_t object, uint32_t source, uint32_t output, uint32_t mirror, uint32_t correction)
{
    uint32_t i = 0, mask = 0;
    if (!r_u8(source + 15))
        return 0;
    do
    {
        uint32_t joint = mirror ? r_u8(0x800CC1B0 + i) : i;
        uint32_t kind = (r_u16(r_u32(r_u32(object)) + 14) & 0x78) >> 3;
        uint16_t q[4], rotation[4], v[4];
        uint32_t k, dest = output + 16 * joint;
        mask |= 1u << (joint & 31);
        for (k = 0; k < 4; ++k)
            q[k] = r_u16(object + 128 + 96 * i + 24 * k);
        if ((kind == 1 || kind == 4) && correction == 1 && ((r_u32(0x800CC1C4) >> (joint & 31)) & 1))
        {
            for (k = 0; k < 4; ++k)
                rotation[k] = r_u16(0x800CC1CE + 8 * joint + 2 * k);
            (void)rrj_pose_quaternion_product(rotation, q, q);
        }
        for (k = 0; k < 4; ++k)
            v[k] = q[k];
        if (mirror)
        {
            if ((kind == 1 || kind == 4) && joint == 0)
            {
                v[0] = (uint16_t)(0u - q[2]);
                v[1] = (uint16_t)(0u - q[3]);
                v[2] = (uint16_t)(0u - q[0]);
                v[3] = (uint16_t)(0u - q[1]);
            }
            else if (kind == 1 || kind == 4 || kind == 5)
            {
                v[0] = (uint16_t)(0u - q[0]);
                v[1] = (uint16_t)(0u - q[1]);
            }
            else
            {
                v[1] = (uint16_t)(0u - q[1]);
                v[2] = (uint16_t)(0u - q[2]);
            }
        }
        for (k = 0; k < 4; ++k)
            w_u16(dest + 2 * k, v[k]);
        ++i;
    } while (i < r_u8(source + 15));
    return mask;
}

/* Overlay8005CB70: capture two corrected/mirrored poses, including cache switch. */
uint32_t sub_8005CB70(uint32_t object, uint32_t record, uint32_t output)
{
    FUNCTION_MARKER(0x8005CB70u, "RASHCDG.BIN");
    uint32_t parent, flags, id, correction = 0, xoffset = 0, yoffset = 0, source = 0, mirror, masks[2], part, k;
    uint16_t translation[3];
    w_u32(output + 4, 10);
    parent = r_u32(object);
    flags = r_u32(parent + 36);
    id = r_u8(record + 28);
    if ((flags & 0x40000) && r_u32(parent + 52))
    {
        xoffset = 1148;
        yoffset = ((int32_t)(uint32_t)(r_u32(parent + 76))) > 65535 ? 10u : 140u;
    }
    else
    {
        parent = r_u32(object);
        if (((r_u16(r_u32(parent) + 14) & 0x78) >> 3) == 1)
        {
            uint32_t linked = r_u32(parent + 52);
            if (linked)
                correction = (r_u16(r_u32(linked) + 14) & 0xf80) >> 7;
        }
    }
    for (part = 0; part < 2; ++part)
    {
        if (part)
            source = sub_8005C39C(object, id);
        translation[0] = r_u16(object + 56);
        if (!part)
            source = r_u32(object + 44);
        mirror = r_u8(record + (part ? 26 : 14)) & 1;
        translation[1] = r_u16(object + 80);
        translation[2] = r_u16(object + 104);
        if (correction == 1)
            for (k = 0; k < 3; ++k)
                translation[k] = (uint16_t)(translation[k] + r_u16(0x800CC1C8 + 2 * k));
        w_u16(output + 12 + 6 * part, mirror ? xoffset - translation[0] : translation[0]);
        w_u16(output + 14 + 6 * part, translation[1] + yoffset);
        w_u16(output + 16 + 6 * part, translation[2]);
        if (part)
            w_u8(output, 20);
        masks[part] = pose_joint_block(object, source, output + 24 + 8 * part, mirror, correction);
    }
    flags = masks[0] & masks[1] & r_u32(object + 1760);
    w_u32(output + 8, flags);
    return flags;
}

/* Overlay8005C8F4: initialize the selected animation record. */
uint32_t sub_8005C8F4(uint32_t object)
{
    uint32_t scalar_value_58;
    uint32_t scalar_value_59;
    uint32_t scalar_value_60;

    FUNCTION_MARKER(0x8005C8F4u, "RASHCDG.BIN");
    uint32_t index = r_u32(object + 12);
    uint32_t animation = r_u32(object + 4);
    uint32_t record = animation + 12u * index;
    uint32_t type = r_u8(record + 1), value, source, limit_frame;
    if (type == 3)
    {
        value = r_u8(record + 2);
        if (!(value & 2) || !(value >> 7))
        {
            (void)sub_8005CB70(object, record, object + 1764);
            xport_update_u8(record + 2, XPORT_MEMORY_UPDATE_OR, 2);
        }
        w_u32(object + 16, 0);
        w_u32(object + 1756, r_u32(record + 8));
        value = (r_u8(record + 2) & 8) ? r_u8(record + 3) : r_u32(object + 1768);
        if (!value)
            value = 10;
        w_u32(object + 24, value);
        return value;
    }
    if (type != 0 && type != 1)
        return 3;
    source = sub_8005C39C(object, r_u8(record));
    if (type == 0)
        w_u32(object + 16, 0);
    value = (r_u8(record + 2) & 8) ? r_u8(record + 3) : r_u16(source + 18);
    if (!value)
        value = 10;
    w_u32(object + 24, value);
    w_u32(object + 28, 0);
    if (type == 0)
    {
        w_u32(object + 20, ((scalar_value_58 = (r_u16(record + 4) << 16)), (((uint32_t)((!((uint32_t)(16))) ? (scalar_value_58) : ((scalar_value_58 >> ((uint32_t)(16))) | ((scalar_value_58 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(16)))) : 0)))))));
        value = r_u32(record + 8);
        w_u32(object + 1756, value);
        return value;
    }
    w_u32(object + 1756, r_u32(record + 8));
    limit_frame = ((scalar_value_59 = (r_u16(record + 6) << 16)), (((uint32_t)((!((uint32_t)(16))) ? (scalar_value_59) : ((scalar_value_59 >> ((uint32_t)(16))) | ((scalar_value_59 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(16)))) : 0))))));
    value = r_u16(source + 16);
    if (((int32_t)(uint32_t)(limit_frame)) >= (int32_t)value)
        w_u16(record + 6, value - 1);
    value = ((scalar_value_60 = (r_u16(record + 4) << 16)), (((uint32_t)((!((uint32_t)(16))) ? (scalar_value_60) : ((scalar_value_60 >> ((uint32_t)(16))) | ((scalar_value_60 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(16)))) : 0))))));
    w_u32(object + 16, value);
    return value;
}

/* Overlay8005BD74: reset and activate using full native record resolution/update. */
uint32_t sub_8005BD74(uint32_t object)
{
    FUNCTION_MARKER(0x8005BD74u, "RASHCDG.BIN");
    uint32_t value = r_u32(object + 36), i;
    static const uint32_t offsets[] = {12, 16, 20, 28, 32, 1756};
    for (i = 0; i < 6; ++i)
        w_u32(object + offsets[i], 0);
    w_u32(object + 24, 10);
    w_u32(object + 36, value & 0xe5);
    value = sub_8005C418(object);
    w_u32(object + 12, value);
    (void)sub_8005C8F4(object);
    value = r_u32(object + 4);
    if (!value)
        return value;
    value = r_u32(object + 8);
    if (!value)
        return value;
    value = (r_u32(object + 36) & 0xfffffffdu) | 2u;
    w_u32(object + 36, value);
    return value;
}

static void animation_command_tail(uint32_t animation, uint32_t kind)
{
    uint32_t flags = r_u8(animation + 2), payload = r_u32(animation + 8);
    uint32_t id = r_u8(animation), speed = r_u8(animation + 3);
    if (kind == 2)
    {
        w_u16(animation + 4, 0xffff);
        w_u16(animation + 6, 0);
    }
    w_u8(animation + 13, (uint8_t)kind);
    w_u16(animation + 16, 0);
    w_u16(animation + 18, 0);
    w_u32(animation + 20, payload);
    w_u8(animation + 14, (uint8_t)flags);
    w_u8(animation + 12, (uint8_t)id);
    w_u8(animation + 15, (uint8_t)speed);
}

/* Overlay8005BF6C: finite animation command, five real arguments. */
uint32_t sub_8005BF6C(uint32_t object, uint32_t id, uint32_t flags, uint32_t speed, uint32_t payload)
{
    FUNCTION_MARKER(0x8005BF6Cu, "RASHCDG.BIN");
    uint32_t set = r_u32(object + 40), animation = r_u32(object + 4);
    uint32_t table = r_u32(set + 4);
    uint32_t physical = table & 0x1FFFFFFFu;
    uint32_t entry;

    if (physical > PSX_DRAM_SIZE - 4u || id > (PSX_DRAM_SIZE - physical - 4u) / 4u)
    {
        const uint32_t args[8] = {object, id, flags, speed, payload, set, table, animation};

        rrj_wip_handoff(rrj_host_context(), 0x8005BF6Cu, "animation", __func__, __FILE__, __LINE__, "invalid_animation_table", args, 8);
        exit(20);
    }
    entry = r_u32(table + 4u * id);
    uint32_t length = r_u16(entry + 16);
    w_u8(animation + 2, (uint8_t)flags);
    w_u8(animation + 1, 1);
    w_u8(animation, (uint8_t)id);
    w_u16(animation + 4, 0);
    w_u32(animation + 8, payload);
    w_u16(animation + 6, length - 1);
    w_u8(animation + 3, (uint8_t)((flags & 8) ? speed : 0));
    animation_command_tail(animation, 5);
    return sub_8005BD74(object);
}

/* Overlay8005C0B0: looping animation command, five real arguments. */
uint32_t sub_8005C0B0(uint32_t object, uint32_t id, uint32_t flags, uint32_t speed, uint32_t payload)
{
    FUNCTION_MARKER(0x8005C0B0u, "RASHCDG.BIN");
    uint32_t animation = r_u32(object + 4);
    w_u8(animation + 2, (uint8_t)flags);
    w_u32(animation + 8, payload);
    w_u8(animation, (uint8_t)id);
    w_u8(animation + 1, 0);
    w_u8(animation + 3, (uint8_t)((flags & 8) ? speed : 0));
    animation_command_tail(animation, 2);
    return sub_8005BD74(object);
}

/* Overlay8005C018: explicit frame interval, seven real arguments. */
uint32_t sub_8005C018(uint32_t object, uint32_t id, uint32_t flags, uint32_t start, uint32_t end, uint32_t speed, uint32_t payload)
{
    FUNCTION_MARKER(0x8005C018u, "RASHCDG.BIN");
    uint32_t animation = r_u32(object + 4);
    w_u8(animation + 2, (uint8_t)flags);
    w_u8(animation, (uint8_t)id);
    w_u8(animation + 1, 1);
    w_u16(animation + 4, start);
    w_u16(animation + 6, end);
    w_u32(animation + 8, payload);
    w_u8(animation + 3, (uint8_t)((flags & 8) ? speed : 0));
    animation_command_tail(animation, 5);
    return sub_8005BD74(object);
}

/* Overlay8005C140: queue or immediately activate a pose transition. */
uint32_t sub_8005C140(uint32_t object, uint32_t target, uint32_t mode, uint32_t flags, uint32_t first_id, uint32_t extra, uint32_t speed, uint32_t payload)
{
    uint32_t scalar_value_61;

    FUNCTION_MARKER(0x8005C140u, "RASHCDG.BIN");
    uint32_t animation = r_u32(object + 4), new_bit = flags & 1;
    uint32_t old_bit = r_u8(animation + 2) & 1;
    uint32_t old_id = r_u8(animation), frame, value, captured_speed;
    first_id &= 255;
    if (mode == 1)
    {
        if (sub_8005BE58(object))
            mode = 0;
        else if (!r_u8(animation + 1))
            w_u8(animation + 1, 1);
    }
    if (!mode)
        frame = r_u32(object + 16);
    else if (mode == 1)
    {
        uint32_t source = r_u32(r_u32(r_u32(object + 40) + 4) + 4 * old_id);
        frame = ((scalar_value_61 = ((r_u16(source + 16) - 1) << 16)), (((uint32_t)((!((uint32_t)(16))) ? (scalar_value_61) : ((scalar_value_61 >> ((uint32_t)(16))) | ((scalar_value_61 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(16)))) : 0))))));
        animation += 12;
    }
    else
    {
        frame = 0xffffffff;
        new_bit = 0;
    }
    w_u8(animation + 2, (uint8_t)flags);
    if (flags & 8)
    {
        w_u8(animation + 2, (uint8_t)(flags | 8));
        w_u8(animation + 3, (uint8_t)speed);
    }
    else
        w_u8(animation + 3, 0);
    value = r_u8(animation + 2);
    w_u8(animation, (uint8_t)first_id);
    w_u8(animation + 1, 3);
    w_u16(animation + 4, 5);
    w_u16(animation + 6, 0);
    w_u32(animation + 8, 0);
    w_u8(animation + 12, 0);
    w_u8(animation + 14, (uint8_t)flags);
    w_u8(animation + 13, 4);
    w_u16(animation + 16, old_id);
    w_u16(animation + 18, frame);
    w_u32(animation + 20, 0);
    w_u8(animation + 26, 0);
    w_u8(animation + 24, 0);
    w_u8(animation + 2, (uint8_t)((extra << 5) | (value & 0xdf) | 0x84));
    w_u8(animation + 14, (uint8_t)((flags & 0xfe) | old_bit));
    if (flags & 8)
        xport_update_u8(animation + 26, XPORT_MEMORY_UPDATE_OR, 8);
    captured_speed = r_u8(animation + 3);
    value = r_u8(animation + 26);
    w_u8(animation + 25, 4);
    w_u16(animation + 28, target);
    w_u16(animation + 30, 0);
    w_u8(animation + 36, 0);
    w_u8(animation + 37, 5);
    w_u32(animation + 32, payload);
    w_u8(animation + 27, (uint8_t)captured_speed);
    w_u8(animation + 26, (uint8_t)((value & 0xfe) | new_bit));
    return mode ? payload : sub_8005BD74(object);
}

/* Overlay800C37B0: choose queued intermediary states from byte tables. */
uint32_t sub_800C37B0(uint32_t object, uint32_t target)
{
    FUNCTION_MARKER(0x800C37B0u, "RASHCDG.BIN");
    uint32_t pair = 0x800CCBAC, current = r_u8(pair);
    uint32_t state = r_u16(object + 544), offset, entry, count, i;
    uint16_t sequence[257];
    while (current < state)
    {
        pair += 2;
        current = r_u8(pair);
    }
    if (r_u8(pair) != r_u16(object + 544))
        return 224;
    offset = r_u8(pair + 1);
    entry = 0x800CCBBC + offset + 2;
    target &= 65535;
    while (r_u8(entry) < target)
        ++entry;
    if (r_u8(entry) != target)
        return 224;
    sequence[0] = (uint16_t)target;
    i = 0;
    while (i < r_u8(0x800CCBBC + offset))
    {
        ++i;
        sequence[i] = r_u8(0x800CCBBC + offset + i);
    }
    count = r_u8(0x800CCBBC + offset);
    sequence[count + 1] = r_u16(object + 544);
    count = r_u8(0x800CCBBC + offset);
    for (i = 0; i < count; ++i)
        w_u16(object + 610 + 2 * i, sequence[i]);
    w_u16(object + 608, count);
    return sequence[count];
}

uint32_t sub_800C3E9C(uint32_t type, uint32_t object, uint32_t *flags_out)
{
    uint32_t flags = *flags_out, mode = flags & 6, anim_flags = flags >> 8;
    uint32_t first = flags & 1, speed = (flags >> 16) & 255;
    uint32_t identity = r_u16(object + 172);
    uint32_t old = r_u16(object + 544);
    uint32_t animation = r_u32(object + 540), actor = (identity >> 5) == 1 ? object : 0;
    uint32_t gate = flags & 24, route = 224, allowed = 0, old_entry, new_entry, value, payload, anim_id;
    uint32_t old_group, new_group;
    FUNCTION_MARKER(0x800C3E9C, "RASHCDG.BIN");
    type &= 65535;
    if (!r_u16(object + 320))
        return 0;
    if (type == 11 || type == 77 || type == 13 || type == 78 || type == 4)
    {
        flags &= 0xfffffffe;
        anim_flags |= 64;
    }
    old_entry = 0x800541D4 + 8 * old;
    if (r_u16(old_entry + 2) == 2 && mode == 4)
        mode = 2;
    if (old == type && (type == 11 || type == 77 || type == 13 || type == 78 || type == 4))
        return 0;
    if (!actor || gate == 16 || old == 224)
        allowed = 1;
    else if (gate == 8 || gate == 0)
    {
        value = r_u16(0x800541D4 + 8 * type + 2);
        if (r_u32(old_entry + 4) & (1u << (value & 31)))
            allowed = 1;
        else if (!gate && !(flags & 32))
        {
            route = sub_800C37B0(actor, type) & 65535;
            if (route != 224)
                allowed = 1;
        }
    }
    if (!allowed)
        return 0;
    if (route != 224)
        type = route;
    else if (actor && !(flags & 32) && r_u16(actor + 608))
        w_u16(actor + 608, 0);
    if (!old && type != 4)
        return 0;
    value = ((flags & 64) << 7) | (r_u32(animation + 36) & 0xffffff7f);
    w_u32(animation + 36, value);
    old_entry = 0x800541D4 + 8 * old;
    new_entry = 0x800541D4 + 8 * type;
    old_group = r_u32(old_entry) & 15;
    new_group = r_u32(new_entry) & 15;
    if (old_group != new_group)
    {
        (void)sub_80012858(animation, r_u32(0x800CE190 + 4 * new_group));
        if (r_u16(old_entry + 2) == 5 && r_u16(new_entry + 2) == 6)
        {
            uint32_t parent, slot = 0;
            if (r_u8(actor + 572) & 32)
            {
                parent = r_u32(r_u32(actor + 596) + 856);
                slot = 1;
            }
            else
                parent = r_u32(actor + 596);
            (void)sub_80068D20(parent, actor, slot);
            w_u8(actor + 72, 3);
        }
    }
    value = r_u32(object + 552) & 0xefffffff;
    w_u32(object + 552, value);
    if (mode)
    {
        uint32_t queue_mode = 1;
        if (mode == 2)
        {
            w_u32(object + 552, value | 0x10000000);
            queue_mode = 0;
        }
        else if (!sub_8005BEF4(animation))
        {
            if (route != 224)
            {
                value = r_u16(actor + 608);
                w_u16(actor + 608, value + 1);
                w_u16(actor + 610 + (value << 1), type);
            }
            return 0;
        }
        anim_id = (r_u32(new_entry) >> 4) & 4095;
        value = r_u32(0x8005B3E4);
        payload = value ? r_u32(value + 4 * type) : 0;
        (void)sub_8005C140(animation, anim_id, queue_mode, anim_flags & 255, first == 0, (flags >> 7) & 1, speed, payload);
    }
    else
    {
        anim_id = (r_u32(new_entry) >> 4) & 4095;
        value = r_u32(0x8005B3E4);
        payload = value ? r_u32(value + 4 * type) : 0;
        if (first)
            (void)sub_8005C0B0(animation, anim_id, anim_flags & 255, speed, payload);
        else if (flags & 0x8000)
            (void)sub_8005C018(animation, anim_id, anim_flags & 255, 6, 11, speed, payload);
        else
            (void)sub_8005BF6C(animation, anim_id, anim_flags & 255, speed, payload);
    }
    *flags_out = flags;
    if (route != 224)
        *flags_out = flags | 32;
    return 1;
}

/* Overlay 800C2F84: select/restart the auxiliary rider animation.
 * The null-object path leaves MIPS v0 untouched; callers use this as void. */
void sub_800C2F84(uint32_t type, uint32_t object, uint32_t flags, uint32_t speed)
{
    FUNCTION_MARKER(0x800C2F84u, "RASHCDG.BIN");
    uint32_t animation = r_u32(object + 556);
    if (!animation)
        return;
    if (r_u16(object + 544) == (type & 65535))
    {
        (void)sub_8005BD74(animation);
    }
    else if (((type - 145) & 65535) < 41)
    {
        (void)sub_8005BF6C(r_u32(object + 556), (type & 65535) - 145, flags & 255, speed, 0);
    }
}

/* Resident 80012884: allocate a 2108-byte animation slot. */
uint32_t sub_80012884(uint32_t pool, uint32_t object)
{
    FUNCTION_MARKER(0x80012884u, "SLUS_010.53");
    uint32_t count = r_u32(pool + 8), capacity = r_u32(pool + 12), index = 0, slot, flags, kind, value;
    if (count == capacity)
        return 0;
    if (((int32_t)(uint32_t)(capacity)) > 0)
    {
        slot = r_u32(pool);
        while (r_u32(slot + 36))
        {
            ++index;
            slot += 2108;
            if (((int32_t)(uint32_t)(index)) >= ((int32_t)(uint32_t)(capacity)))
                break;
        }
    }
    slot = r_u32(pool) + 2108 * index;
    flags = r_u32(slot + 36);
    w_u32(slot, object);
    w_u32(slot + 1760, 0xffffffff);
    w_u32(slot + 36, flags | 5);
    kind = (r_u16(r_u32(object) + 14) & 120) >> 3;
    if (kind == 1 || kind == 4)
    {
        value = r_u32(0x80052390 + 4 * (kind == 4));
        flags = r_u32(slot + 36);
        w_u32(slot + 1760, value);
        w_u32(slot + 36, (flags & 0xfffffffb) | 4);
    }
    w_u8(r_u32(slot + 4) + 1, 5);
    count = r_u32(pool + 8);
    w_u32(pool + 8, count + 1);
    return slot;
}

/* Resident 8001298C: switch model and rebind its joint records. */
uint32_t sub_8001298C(uint32_t object, uint32_t id)
{
    FUNCTION_MARKER(0x8001298Cu, "SLUS_010.53");
    uint32_t old = (uint32_t)(int32_t)r_s8(object + 8), set, offset, model, value, count, i, source, destination;
    static const uint32_t zero_offsets[6] = {6, 8, 10, 14, 16, 18};
    static const uint32_t diagonal_offsets[3] = {4, 12, 20};
    if (id == old)
        return old;
    set = r_u32(object + 96);
    if (((int32_t)(uint32_t)(id)) >= r_u8(set + 4))
        return old;
    set = r_u32(object + 96);
    offset = 12 * id;
    w_u8(object + 8, (uint8_t)id);
    model = r_u32(r_u32(set + 8) + offset);
    w_u32(object, model);
    value = r_u32(model + 16);
    model = r_u32(object);
    w_u32(object + 40, value);
    count = r_u16(model + 24);
    for (i = 0; i < count; ++i)
    {
        set = r_u32(object + 96);
        source = r_u32(set + 8);
        destination = r_u32(object + 4) + 24 * i;
        source = r_u32(source + offset + 4);
        value = r_u32(source + 4 * i);
        w_u32(destination, value);
    }
    model = r_u32(object);
    if (((r_u16(model + 14) & 120) >> 3) == 5 && (old == 0 || old == 4))
    {
        for (i = 0; i < 6; ++i)
            w_u16(r_u32(object + 4) + zero_offsets[i], 0);
        for (i = 0; i < 3; ++i)
            w_u16(r_u32(object + 4) + diagonal_offsets[i], 4096);
    }
    return old;
}

/* Resident counter mirror and effect allocation/linking, audited MIPS. */
uint32_t sub_80043F00(uint32_t id)
{
    FUNCTION_MARKER(0x80043F00u, "SLUS_010.53");
    id &= 65535;
    if (id >= 3)
        return 0;
    return r_u16(r_u32(0x800549B8) + 16 * id);
}

uint32_t sub_8002705C(uint32_t object, uint32_t mode)
{
    FUNCTION_MARKER(0x8002705Cu, "SLUS_010.53");
    uint32_t value = sub_80043F00(0xf2000002) & 255;
    w_u16(object + 56, value << 4);
    if (!mode)
    {
        w_u16(object + 58, 50);
        return 50;
    }
    if (((int32_t)(uint32_t)(mode)) < 18)
    {
        w_u16(object + 58, 120);
        return 120;
    }
    value = sub_80043F00(0xf2000002) & 255;
    w_u16(object + 58, (uint32_t)(int32_t)(int8_t)(((255 * value) >> 8) - 127));
    return value;
}

uint32_t sub_80027178(void)
{
    FUNCTION_MARKER(0x80027178u, "SLUS_010.53");
    uint32_t slot = r_u32(0x800D8068), i;
    for (i = 0; i < 20; ++i, slot += 112)
        if (!((r_u32(slot) >> 6) & 15))
            return i;
    return 0xffffffff;
}

uint32_t sub_800271CC(uint32_t object, uint32_t id)
{
    FUNCTION_MARKER(0x800271CCu, "SLUS_010.53");
    uint32_t current = (uint32_t)(int32_t)r_s8(object + 73), slot, value, next;
    if (current == 0xffffffff)
    {
        w_u8(object + 73, (uint8_t)id);
        return 0xfffffff8;
    }
    slot = 0x800D39B0 + 112 * current;
    value = r_u32(slot);
    while ((value & 63) != 63)
    {
        next = value & 63;
        if (next & 32)
            next |= 0xffffffc0;
        slot = 0x800D39B0 + 112 * next;
        value = r_u32(slot);
    }
    value = (r_u32(slot) & 0xffffffc0) | (id & 63);
    w_u32(slot, value);
    return value;
}

uint32_t sub_800273EC(uint32_t object, uint32_t subtype, uint32_t duration, uint32_t kind, uint32_t flags)
{
    FUNCTION_MARKER(0x800273ECu, "SLUS_010.53");
    uint32_t identity, global, excluded, index, slot, value, stamp;
    if (flags & 2)
    {
        identity = r_u16(object + 172);
        global = r_u32(0x8005B2F8);
        excluded = ((int32_t)(uint32_t)(identity & 31)) >= ((int32_t)(uint32_t)(r_u32(global + 48)));
        if ((identity >> 5) >= 2 || excluded)
            return excluded;
    }
    index = sub_80027178();
    if (index == 0xffffffff)
        return index;
    slot = 0x800D39B0 + 112 * index;
    value = r_u32(slot);
    w_u8(slot + 60, (uint8_t)flags);
    value = (value & 0xfffffc3f) | ((kind & 15) << 6);
    w_u32(slot, value);
    value = (((value & 0xffc03fff) | ((subtype & 255) << 14)) & 0xffffc3ff) | 0x400;
    global = r_u32(0x8005B2F8);
    w_u32(slot, value);
    stamp = r_u32(global + 16);
    w_u32(slot + 52, duration);
    w_u32(slot + 48, stamp);
    (void)sub_8002705C(slot, 120);
    w_u32(slot + 36, 0);
    w_u32(slot + 44, 0);
    w_u32(slot + 40, 0);
    w_u8(slot + 61, 0);
    w_u16(slot + 62, 30);
    return sub_800271CC(object, index);
}

uint32_t sub_800CB84C(uint32_t object)
{
    FUNCTION_MARKER(0x800CB84Cu, "RASHCDG.BIN");
    uint32_t id = ((uint32_t)r_u8((object + 567)) ^ 128) - 128, mask, value;
    if (id == 0xffffffff)
        return id;
    mask = 1u << (id & 31);
    value = r_u32(0x8005AD50);
    if (!(value & mask))
        return ~mask;
    w_u32(0x8005AD50, value & ~mask);
    id = ((uint32_t)r_u8((object + 567)) ^ 128) - 128;
    return sub_80068D20(object, 0x800CF018 + 172 * id, 0);
}

uint32_t sub_800CC0B0(uint32_t object)
{
    FUNCTION_MARKER(0x800CC0B0u, "RASHCDG.BIN");
    uint32_t animation = r_u32(object + 540), value;
    if (animation)
    {
        w_u32(animation + 36, 0);
        value = r_u32(0x800CE178);
        w_u32(0x800CE178, value - 1);
        w_u32(object + 540, 0);
    }
    value = (uint32_t)r_u8((object + 566));
    return value ? value : sub_800CB84C(object);
}

/* Overlay8008C000: release object bookkeeping for kinds2..6. */
uint32_t sub_8008C000(uint32_t identity, uint32_t kind)
{
    FUNCTION_MARKER(0x8008C000u, "RASHCDG.BIN");
    uint32_t index = r_u16(identity) & 31, pool, stride, base_offset, active_offset, increment = 0;
    uint32_t value, last, account, base, result, cell;
    if (kind < 2 || kind > 6)
        return 0x80060000;
    switch (kind)
    {
        case 2:
            value = r_u16(identity) & 31;
            (void)sub_800CC0B0(r_u32(0x800D4B80) + 572 * value);
            pool = 0x800D4B70;
            stride = 572;
            base_offset = 16;
            active_offset = 172;
            break;
        case 3:
            if (!r_u32(identity + 8))
            {
                value = 0x800CF660 + 512 * (r_u16(identity) & 31);
                if (r_u32(value + 36) & 0x08000000)
                {
                    (void)sub_8002847C(value);
                    (void)sub_8002820C(value);
                }
            }
            pool = 0x800CF650;
            stride = 512;
            base_offset = 0;
            active_offset = 172;
            break;
        case 4:
            pool = 0x800CD6C8;
            stride = 596;
            base_offset = 12;
            active_offset = 172;
            increment = 596;
            break;
        case 5:
            pool = 0x800CE598;
            stride = 0u - 452;
            base_offset = 12;
            active_offset = 172;
            increment = 452;
            break;
        default:
            pool = 0x800CD6A8;
            stride = 280;
            base_offset = 28;
            active_offset = 0;
            break;
    }
    value = r_u32(pool);
    if (((int32_t)(uint32_t)(value)) > 0)
        w_u32(pool, value - 1);
    if (r_u32(pool + 8) == index)
    {
        do
        {
            last = r_u32(pool + 8);
            account = r_u32(0x800D1814) + increment;
            --last;
            w_u32(pool + 8, last);
            w_u32(0x800D1814, account);
            if (((int32_t)(uint32_t)(last)) < 0)
                break;
            base = kind == 3 ? pool + 16 : r_u32(pool + base_offset);
        } while (!r_u16(base + stride * last + active_offset));
    }
    result = ((int32_t)(uint32_t)(index)) < ((int32_t)(uint32_t)(r_u32(pool + 4)));
    if (result)
        w_u32(pool + 4, index);
    if (kind == 4 || kind == 5)
    {
        base = r_u32(pool + 12);
        cell = r_u32(base + stride * index + 4);
        w_u32(cell, 0);
        result = (cell - 0x800D1664) * 0xaaaaaaab;
        value = ((uint32_t)((!((uint32_t)(3))) ? (((uint32_t)(result))) : ((((uint32_t)(result)) >> ((uint32_t)(3))) | ((((uint32_t)(result)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(3)))) : 0))));
        if (((int32_t)(uint32_t)(value)) < ((int32_t)(uint32_t)(r_u32(0x800D1660))))
            w_u32(0x800D1660, value);
    }
    w_u16(identity, 0);
    return result;
}

uint32_t sub_8008CC94(void)
{
    FUNCTION_MARKER(0x8008CC94u, "RASHCDG.BIN");
    uint32_t best = 0, distance = 0, count = r_u32(r_u32(0x800CE4FC)), object = r_u32(0x800CE4F0), player, value, sign, animation;
    if (((int32_t)(uint32_t)(count)) >= 0)
    {
        player = r_u32(0x8005B38C);
        do
        {
            if (r_u16(object + 172))
            {
                value = r_u32(player + 324) - r_u32(object + 324);
                sign = ((uint32_t)((!((uint32_t)(31))) ? (((uint32_t)(value))) : ((((uint32_t)(value)) >> ((uint32_t)(31))) | ((((uint32_t)(value)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(31)))) : 0))));
                value = (value + sign) ^ sign;
                if (((int32_t)(uint32_t)(distance)) < ((int32_t)(uint32_t)(value)))
                {
                    distance = value;
                    best = object;
                }
            }
            value = r_u32(0x800CE4F4);
            --count;
            object += value;
        } while (((int32_t)(uint32_t)(count)) >= 0);
    }
    if (!best)
        return 0;
    animation = r_u32(best + 540);
    (void)sub_8008C000(best + 172, 2);
    return animation;
}

uint32_t sub_800958F0(uint32_t object, uint32_t mode)
{
    FUNCTION_MARKER(0x800958F0u, "RASHCDG.BIN");
    uint32_t i, slot = 0x800CF018, mask, value, parent, descriptor, kind, animation;
    if (r_u32(0x8005AD50) == 255)
        return 0;
    for (i = 0; i < 8; ++i, slot += 172)
    {
        value = r_u32(0x8005AD50);
        mask = 1u << i;
        if (value & mask)
            continue;
        parent = r_u32(object + 596);
        w_u32(0x8005AD50, value | mask);
        w_u8(object + 571, (uint8_t)i);
        descriptor = r_u32(parent + 1084);
        kind = (uint32_t)r_u8((descriptor + 46));
        if (!kind || kind == 4)
        {
            if (r_u32(0x8005B254) && r_u16(parent + 172) < r_u32(r_u32(0x8005B2F8) + 48) && r_u32(0x800CE178) == r_u32(0x800CE17C))
                (void)sub_8008CC94();
            animation = sub_80012884(0x800CE170, slot);
            w_u32(object + 556, animation);
            if (!animation)
            {
                value = ((uint32_t)r_u8((object + 571)) ^ 128) - 128;
                mask = 1u << (value & 31);
                w_u32(0x8005AD50, r_u32(0x8005AD50) & ~mask);
                w_u8(object + 571, 255);
                return 0;
            }
            (void)sub_80012858(animation, r_u32(0x800CE19C));
        }
        descriptor = r_u32(r_u32(object + 596) + 1084);
        kind = (uint32_t)r_u8((descriptor + 46));
        if (kind != (((uint32_t)r_u8((slot + 8)) ^ 128) - 128))
            (void)sub_8001298C(slot, kind);
        (void)sub_80012838(object, slot, mode ? 7 : 10, 0);
        descriptor = r_u32(r_u32(object + 596) + 1084);
        if ((uint32_t)r_u8((descriptor + 47)))
        {
            (void)sub_800273EC(slot, 0, 1500, 6, 0);
            w_u8(object + 572, (uint8_t)((uint32_t)r_u8((object + 572)) | 128));
        }
        return 1;
    }
    return 0;
}

void sub_800BFC5C(uint32_t object, uint32_t previous, uint32_t type, uint32_t flags)
{
    FUNCTION_MARKER(0x800BFC5Cu, "RASHCDG.BIN");
    if (r_u16(0x800541D4 + 8 * (previous & 65535) + 2) != 3)
    {
        if (!((uint32_t)r_u8((r_u32(r_u32(object + 596) + 1084) + 60)) & 128))
            return;
        (void)sub_800958F0(object, flags & 256);
    }
    if ((uint32_t)r_u8((r_u32(r_u32(object + 596) + 1084) + 60)) & 128)
        sub_800C2F84(type & 65535, object, (flags >> 8) & 255, (flags >> 16) & 255);
}

void sub_800C4454(uint32_t type, uint32_t object, uint32_t flags)
{
    FUNCTION_MARKER(0x800C4454u, "RASHCDG.BIN");
    uint32_t kind, old, animation, parent, physical;
    type &= 65535;
    physical = object & 0x1FFFFFFFu;
    if (physical > PSX_DRAM_SIZE - 600u)
    {
        const uint32_t args[3] = {type, object, flags};

        rrj_wip_handoff(rrj_host_context(), 0x800C4454u, "race_state", __func__, __FILE__, __LINE__, "invalid_object", args, 3);
        exit(20);
    }
    kind = r_u16(0x800541D4 + 8 * type + 2);
    old = r_u16(object + 544);
    if (kind == 3)
    {
        sub_800BFC5C(object, old, type, flags);
        return;
    }
    if (type)
        return;
    parent = r_u32(object + 596);
    physical = parent & 0x1FFFFFFFu;
    if (physical > PSX_DRAM_SIZE - 544u)
    {
        const uint32_t args[4] = {type, object, flags, parent};

        rrj_wip_handoff(rrj_host_context(), 0x800C4454u, "race_state", __func__, __FILE__, __LINE__, "invalid_parent", args, 4);
        exit(20);
    }
    animation = r_u32(parent + 540);
    if (flags & 0x8000)
        (void)sub_8005C018(animation, 0, (flags >> 8) & 255, 6, 11, 0, 0);
    else
        (void)sub_8005BF6C(animation, 0, (flags >> 8) & 255, 0, 0);
}

void sub_800BC8DC(uint32_t object)
{
    FUNCTION_MARKER(0x800BC8DCu, "RASHCDG.BIN");
    uint32_t count = ((uint32_t)r_u8((object + 946)) ^ 128) - 128, last, kind, descriptor, linked, target, global, index, mask, value;
    if (!count)
        return;
    last = object + 956 + 8 * (count - 1);
    kind = r_u16(last);
    if (kind - 10 < 7)
    {
        descriptor = r_u32(object + 1084);
        w_u8(descriptor + 61, (uint8_t)((uint32_t)r_u8((descriptor + 61)) & 240));
    }
    if (r_u32(object + 560) & 0x08000000)
    {
        linked = r_u32(object + 852);
        w_u32(object + 704, 0);
        kind = r_u16(linked + 544);
        if (r_u16(0x800541D4 + 8 * kind + 2) == 3)
        {
            index = (uint32_t)r_u8((linked + 569));
            target = r_u16(r_u32(0x8005AD4C) + 12 * index);
            (void)sub_800C4550(target, linked, 2);
        }
    }
    (void)sub_8001E100(last, 0, 8);
    count = (uint32_t)r_u8((object + 946)) - 1;
    last = object + 956 + 8 * ((((count & 255) ^ 128) - 128) - 1);
    w_u8(object + 946, (uint8_t)count);
    if (r_u16(last) != 16)
        return;
    global = r_u32(0x8005B2F8);
    index = r_u16(last + 2);
    if (index >= r_u32(global + 48))
        return;
    value = (uint32_t)r_u8((global + 4));
    if (value == 36 || value == 44)
        return;
    mask = r_u16(0x800CCAC0 + 2 * index);
    if (mask && (mask & (0u - mask)) != mask)
        sub_800BC8DC(object);
}

uint32_t sub_800C1014(uint32_t object)
{
    FUNCTION_MARKER(0x800C1014u, "RASHCDG.BIN");
    uint32_t parent = r_u32(object + 596), count, last, global, index, value, animation;
    w_u8(object + 569, 41);
    w_u8(object + 570, 0);
    if (!(r_u32(parent + 560) & 0x08000000) && !((uint32_t)r_u8((object + 572)) & 64))
    {
        count = ((uint32_t)r_u8((parent + 946)) ^ 128) - 128;
        last = parent + 956 + 8 * (count - 1);
        if (r_u16(last) == 16)
        {
            global = r_u32(0x8005B2F8);
            index = r_u16(parent + 172);
            if (index < r_u32(global + 48))
            {
                value = r_u16(last + 2);
                w_u16(0x800CCB6C + 2 * index, value);
                index = r_u16(parent + 172);
                value = r_u32(global + 16);
                w_u32(0x800CCB70 + 4 * index, value);
            }
            sub_800BC8DC(parent);
        }
    }
    animation = r_u32(object + 540);
    w_u32(animation + 36, r_u32(animation + 36) & 0xffffff7f);
    value = (uint32_t)r_u8((object + 572)) & 253;
    w_u8(object + 572, (uint8_t)value);
    return value;
}

uint32_t sub_800BFD24(uint32_t object, uint32_t previous, uint32_t type)
{
    FUNCTION_MARKER(0x800BFD24u, "RASHCDG.BIN");
    if (r_u16(0x800541D4 + 8 * (type & 65535) + 2) == 3)
        return 3;
    (void)sub_80095AEC(object);
    return sub_800C1014(object);
}

uint32_t sub_800C4500(uint32_t type, uint32_t object, uint32_t flags)
{
    FUNCTION_MARKER(0x800C4500u, "RASHCDG.BIN");
    uint32_t previous = r_u16(object + 544);
    if (r_u16(0x800541D4 + 8 * previous + 2) != 3)
        return 3;
    return sub_800BFD24(object, previous, type & 65535);
}

uint32_t sub_800C4550(uint32_t type, uint32_t object, uint32_t flags)
{
    FUNCTION_MARKER(0x800C4550u, "RASHCDG.BIN");
    uint32_t result;
    type &= 65535;
    result = sub_800C3E9C(type, object, &flags);
    if (result == 1)
    {
        (void)sub_800C4500(type, object, flags);
        sub_800C4454(type, object, flags);
        (void)sub_800C2FF4(type, object, flags);
    }
    return result;
}

static uint32_t enqueue_actor_event(uint8_t *event, uint32_t mode, uint32_t object)
{
    uint32_t count, search, destination, first, second, global, value, linked, type;
    count = ((uint32_t)r_u8((object + 946)) ^ 128) - 128;
    if (((int32_t)(uint32_t)(count)) >= 16)
    {
        w_u8(object + 946, 1);
        (void)sub_8001E100(object + 964, 0, 120);
    }
    count = ((uint32_t)r_u8((object + 946)) ^ 128) - 128;
    search = (uint32_t)r_u8((object + 946));
    destination = object + 956 + 8 * count;
    if (count && mode == 1)
    {
        value = destination - 8;
        if (r_u16(value) == xport_load_le16((event + 0)) && r_u16(value + 2) == xport_load_le16((event + 2)))
            return 1;
    }
    else if (count && mode == 2)
    {
        value = object + 956 + 8 * (count - 1);
        for (;;)
        {
            if (r_u16(value) == xport_load_le16((event + 0)) && r_u16(value + 2) == xport_load_le16((event + 2)))
                break;
            --search;
            count = ((search & 255) ^ 128) - 128;
            if (((int32_t)(uint32_t)(count)) <= 0)
                break;
            value = object + 956 + 8 * (count - 1);
        }
        count = ((search & 255) ^ 128) - 128;
        if (((int32_t)(uint32_t)(count)) > 0)
        {
            if (count == (((uint32_t)r_u8((object + 946)) ^ 128) - 128))
                return 1;
            xport_store_le16((event + 6), r_u16(value + 6));
            do
            {
                ++search;
                first = r_u32(value + 8);
                second = r_u32(value + 12);
                w_u32(value, first);
                w_u32(value + 4, second);
                count = ((uint32_t)r_u8((object + 946)) ^ 128) - 128;
                value += 8;
            } while (((int32_t)(uint32_t)(((search & 255) ^ 128) - 128)) < ((int32_t)(uint32_t)(count)));
            first = xport_load_le32((event + 0));
            second = xport_load_le32((event + 4));
            w_u32(value, first);
            w_u32(value + 4, second);
            return 1;
        }
        count = ((uint32_t)r_u8((object + 946)) ^ 128) - 128;
        destination = object + 956 + 8 * count;
    }
    w_u8(object + 946, (uint8_t)((uint32_t)r_u8((object + 946)) + 1));
    global = r_u32(0x8005B2F8);
    first = xport_load_le32((event + 0));
    second = xport_load_le32((event + 4));
    w_u32(destination, first);
    w_u32(destination + 4, second);
    w_u16(destination + 4, 0);
    w_u16(destination + 6, 0);
    value = ((2180 * r_u32(global + 16) + 0x8000) >> 16) | 0xc000;
    w_u16(destination + 6, value);
    if (r_u32(object + 560) & 0x08000000)
    {
        linked = r_u32(object + 852);
        type = r_u16(linked + 544);
        if (r_u16(0x800541D4 + 8 * type + 2) == 3)
        {
            value = (uint32_t)r_u8((linked + 569));
            type = r_u16(r_u32(0x8005AD4C) + 12 * value);
            (void)sub_800C4550(type, linked, 2);
        }
    }
    return 1;
}

uint32_t sub_800BCA68(uint8_t event[8], uint32_t mode, uint32_t object)
{
    FUNCTION_MARKER(0x800BCA68u, "RASHCDG.BIN");
    return enqueue_actor_event(event, mode, object);
}

uint32_t sub_800C3104(uint32_t object, uint32_t inactive)
{
    FUNCTION_MARKER(0x800C3104u, "RASHCDG.BIN");
    uint32_t active = r_u16(object + 320), type, animation;
    w_u32(object + 604, 0);
    w_u16(object + 608, 0);
    if (active)
    {
        type = r_u16(object + 544);
        if (type == 6 || type == 77)
            return 77;
        return sub_800C4550(((uint32_t)r_u8((object + 572)) & 32) ? 77 : 6, object, 16);
    }
    (void)sub_800C4500(224, object, 0);
    animation = r_u32(object + 540);
    if (animation)
        (void)sub_8005BE0C(animation);
    if (inactive)
    {
        w_u16(object + 544, 224);
        return 224;
    }
    w_u16(object + 544, 73);
    w_u32(object + 604, 4);
    return 4;
}

/* Overlay800903F4: restore actor physics, attachments and player state. */
uint32_t sub_800903F4(uint32_t object, uint32_t reset)
{
    FUNCTION_MARKER(0x800903F4u, "RASHCDG.BIN");
    uint32_t piece, x, y, z, secondary, sibling, other, value, flags, global, descriptor, player, index, i;
    uint16_t axes[6];
    uint8_t event[8] = {0};
    if (reset)
    {
        if (r_u16(object + 320))
        {
            piece = r_u32(object + 340);
            (void)sub_8002EAD8(xport_guest_ptr(piece + 20, 12), xport_guest_ptr(piece + 14, 6), r_u32(object + 348), xport_guest_ptr(object + 504, 12));
            x = r_u32(object + 504);
            y = r_u32(object + 508);
            z = r_u32(object + 512);
            piece = r_u32(object + 340);
            w_u32(object + 184, x);
            w_u32(object + 188, y);
            w_u32(object + 192, z);
            for (i = 0; i < 3; ++i)
                w_u16(object + 450 + 2 * i, r_u16(piece + 14 + 2 * i));
            if (r_u32(object + 568) & 0x00400000)
            {
                x = r_u16(object + 450);
                w_u32(object + 364, 0xfffffffe);
                z = r_u16(object + 454);
                w_u16(object + 450, 0u - x);
                y = r_u16(object + 452);
                w_u16(object + 454, 0u - z);
                w_u16(object + 452, 0u - y);
            }
            else
                w_u32(object + 364, 2);
            (void)sub_8007EC30(object);
        }
        w_u32(object + 344, 0);
    }
    (void)sub_8002090C(object);
    flags = r_u32(object + 568);
    secondary = r_u32(object + 852);
    w_u32(object + 568, flags | 0x08000000);
    w_u32(secondary + 552, r_u32(secondary + 552) & 0xbfe67fc7);
    secondary = r_u32(object + 852);
    w_u16(secondary + 320, r_u16(object + 320));
    secondary = r_u32(object + 852);
    if (r_u32(secondary + 556))
    {
        (void)sub_80095AEC(secondary);
        w_u32(r_u32(object + 852) + 556, 0);
    }
    (void)sub_80012838(object, r_u32(object + 852), 2, 0);
    secondary = r_u32(object + 852);
    w_u8(object + 72, 0);
    (void)sub_800C3104(secondary, 1);
    secondary = r_u32(object + 852);
    if (((uint32_t)r_u8((secondary + 572)) & 16) && (uint32_t)r_u8((r_u32(0x8005B2F8) + 57)) != 1)
    {
        sibling = r_u32(object + 856);
        other = r_u32(sibling + 852);
        value = r_u16(object + 320);
        flags = r_u32(other + 552);
        w_u16(other + 320, value);
        w_u32(other + 552, flags & 0xbfe67fc7);
        (void)sub_80012838(object, other, 2, 1);
        (void)sub_800C3104(other, 1);
        w_u32(other + 604, 1);
        w_u32(r_u32(object + 856) + 720, 0);
    }
    (void)sub_800BCD10(object);
    descriptor = r_u32(object + 1084);
    value = (r_u32(descriptor + 40) || (uint32_t)r_u8((descriptor + 39)) >= 248) ? 2 : 4;
    xport_store_le16(event, value);
    xport_store_le16(event + 2, 224);
    (void)sub_800BCA68(event, 1, object);
    if (r_u16(object + 320))
    {
        (void)sub_80096564(object);
        for (i = 0; i < 3; ++i)
            axes[i] = r_u16(object + 528 + 2 * i);
        for (i = 0; i < 3; ++i)
            axes[i + 3] = r_u16(object + 516 + 2 * i);
        secondary = r_u32(object + 852);
        for (i = 0; i < 3; ++i)
            w_u16(object + 450 + 2 * i, axes[i]);
        for (i = 0; i < 3; ++i)
            w_u16(object + 814 + 2 * i, axes[i + 3]);
        if (((uint32_t)r_u8((secondary + 572)) & 16) && (uint32_t)r_u8((r_u32(0x8005B2F8) + 57)) != 1)
        {
            sibling = r_u32(object + 856);
            value = r_u32(object + 344);
            w_u32(sibling + 344, value);
            flags = r_u32(object + 364);
            sibling = r_u32(object + 856);
            x = r_u32(object + 304);
            y = r_u32(sibling + 304);
            value = r_u32(sibling + 344);
            w_u32(sibling + 344, ((int32_t)(uint32_t)(flags)) < 0 ? value - (x + y) : value + x + y);
            sibling = r_u32(object + 856);
            value = r_u32(object + 364);
            w_u32(sibling + 364, value);
        }
    }
    global = r_u32(0x8005B2F8);
    index = r_u16(object + 172);
    if (index >= r_u32(global + 48))
        return 0;
    player = 0x800CD898 + 1132 * index;
    w_u32(player + 548, r_u32(player + 548) & 0xf5ffff7f);
    if ((uint32_t)r_u8((global + 4)) == 44 && (uint32_t)r_u8((global + 57)) == 2)
    {
        flags = r_u32(player + 552);
        value = r_u32(player + 776);
        w_u32(player + 552, flags | 16);
        if (((int32_t)(uint32_t)(value)) < 5)
        {
            w_u32(player + 776, 5);
            w_u32(player + 792, 0);
        }
        w_u8(r_u32(0x8005B2F8) + 10, 2);
    }
    else
        w_u32(player + 548, (r_u32(player + 548) | 6) & 0xff807fff);
    (void)sub_80086AF8(player);
    (void)sub_80018440(r_u16(object + 172), 0);
    return sub_800235B0(r_u16(object + 172), object);
}

/* 80093FE4: inactive actor cleanup, including actual activation when required. */
void sub_80093FE4(uint32_t object)
{
    FUNCTION_MARKER(0x80093FE4u, "RASHCDG.BIN");
    uint32_t global, secondary, flags, animation;
    uint32_t state;
    if (!object || r_u16(object + 320))
        return;
    global = r_u32(0x8005B2F8);
    if (r_u16(object + 172) < r_u32(global + 48) || (uint32_t)((uint32_t)r_u8((global + 57)) - 1u) >= 2u)
    {
        (void)sub_8002090C(object);
        secondary = r_u32(object + 852);
        if (r_u32(secondary + 604) >= 3u && r_u16(secondary + 320))
            w_u32(object + 568, r_u32(object + 568) | 0x100u);
        else
        {
            w_u16(secondary + 320, 0);
            (void)sub_800903F4(object, 1);
        }
    }
    flags = r_u32(object + 568);
    animation = r_u32(object + 540);
    w_u32(object + 568, flags | 0x08000000u);
    if (animation)
    {
        w_u32(animation + 36, 0);
        w_u32(0x800CE178, r_u32(0x800CE178) - 1u);
        w_u32(object + 540, 0);
    }
    global = r_u32(0x8005B2F8);
    if (r_u16(object + 172) >= r_u32(global + 48) && ((uint32_t)r_u8((r_u32(object + 1084) + 1)) & 15u) == 2u)
    {
        state = (uint32_t)r_u8((object + 928));
        if (state & 16u)
        {
            w_u8(object + 928, (uint8_t)(state & 0xEFu));
            w_u32(object + 324, r_u32(0x8005B244) << 12);
            (void)sub_8009DAF8(0xFFFFFFFFu);
            if (r_u32(object + 36) & 0x08000000u)
            {
                (void)sub_8002847C(object);
                (void)sub_8002820C(object);
            }
        }
        w_u8(object + 928, (uint8_t)((uint32_t)r_u8((object + 928)) & 0xDFu));
    }
}

/* 800A0708: activate and place the actor at its directional track offset. */
uint32_t sub_800A0708(uint32_t object)
{
    FUNCTION_MARKER(0x800A0708u, "RASHCDG.BIN");
    uint32_t global, value, record, offset = 0, direction, piece, scale, x, y, z;
    (void)sub_800903F4(object, 1);
    global = r_u32(0x8005B2F8);
    value = r_u32(0x80053138 + 4u * r_u32(global + 60));
    record = r_u32(object + 372);
    w_u32(object + 576, value);
    w_u32(object + 480, value);
    w_u32(object + 924, value);
    if (record)
    {
        direction = r_u32(object + 364);
        offset = r_u32(record + 80);
        if (((int32_t)(uint32_t)(direction)) >= 0)
            offset = r_u32(record + 208);
    }
    piece = r_u32(object + 340);
    direction = r_u32(object + 364);
    scale = offset - 0x8000u + ((0u - direction) & 0x10000u);
    w_u32(object + 344, scale);
    (void)sub_8002EAD8(xport_guest_ptr(object + 184, 12), xport_guest_ptr(piece + 2, 6), scale, xport_guest_ptr(object + 184, 12));
    x = r_u32(object + 184);
    y = r_u32(object + 188);
    z = r_u32(object + 192);
    w_u32(object + 504, x);
    w_u32(object + 508, y);
    w_u32(object + 512, z);
    return x;
}

/* 8003A468: rebuild track position using the loaded segment search. */
uint32_t sub_8003A468(uint32_t position, uint32_t output)
{
    FUNCTION_MARKER(0x8003A468u, "SLUS_010.53");
    uint32_t segment = sub_80039DFC(0, ((position) ? (const uint32_t[3]){r_u32(position), 0, (uint32_t)r_u16((position) + 10) << 16} : NULL)), route = 0, count, i, word, base, subrecord, piece, delta, limit;
    if (!segment)
        return 0;
    if (!(uint32_t)r_s16((segment + 16)))
        route = r_u32(segment + 44);
    else
    {
        count = (uint32_t)r_s16((segment + 18));
        if (((int32_t)(uint32_t)(count)) > 0)
        {
            word = r_u32(position);
            base = r_u32(segment + 44);
            for (i = 0; ((int32_t)(uint32_t)(i)) < ((int32_t)(uint32_t)(count)); ++i)
            {
                route = base + 32 * i;
                if ((uint32_t)r_s16((route + 2)) == (word >> 16) && r_u32(route + 12) == (word & 65535u))
                    break;
            }
        }
        if ((uint32_t)r_s16((route + 2)))
            return 0;
    }
    subrecord = r_u32(segment + 48) + 28u * (uint32_t)r_s16((route + 20));
    piece = r_u32(segment + 52) + 52u * (uint32_t)r_s16((subrecord + 8));
    w_u32(output, segment);
    w_u32(output + 4, route);
    w_u32(output + 8, subrecord);
    w_u32(output + 12, piece);
    w_u32(output + 20, 0);
    w_u32(output + 16, 0);
    w_u32(output + 24, 0);
    w_u32(output + 28, 0);
    delta = r_u32(position + 8) - r_u32(route + 24);
    if (((int32_t)(uint32_t)(delta)) < 0)
        delta = 0;
    limit = r_u32(route + 28);
    if (((int32_t)(uint32_t)(limit)) < ((int32_t)(uint32_t)(delta)))
        delta = limit;
    {
        uint32_t route_geometry[8] = {r_u32(output), r_u32(output + 4), r_u32(output + 8), r_u32(output + 12), r_u32(output + 16), r_u32(output + 20), r_u32(output + 24), r_u32(output + 28)};
        (void)sub_80037524(route_geometry, delta);
        w_u32(output + 12, route_geometry[3]);
        w_u32(output + 20, route_geometry[5]);
    }
    return 1;
}

uint32_t sub_8003E67C(uint32_t surface, uint32_t position, uint32_t side, uint32_t output)
{
    uint32_t block, main_width, width, outer, sign, magnitude, value;
    int32_t count;
    FUNCTION_MARKER(0x8003E67C, "SLUS_010.53");
    sign = (uint32_t)((int32_t)position >> 31);
    position = (position + sign) ^ sign;
    block = surface + (side == 2 ? 136 : 8);
    main_width = r_u32(block + 8);
    count = r_s16(block + 6);
    outer = r_u32(block + 72);
    width = main_width;
    if (count > 0)
    {
        sign = (uint32_t)((int32_t)outer >> 31);
        magnitude = (outer + sign) ^ sign;
        if ((int32_t)position < (int32_t)magnitude)
        {
            value = r_u16(block + 82);
            w_u16(output, 5);
            w_u16(output + 2, (uint16_t)value);
            return 0;
        }
    }
    if (r_s16(block + 2) > 0)
        width = r_u32(block + 24);
    sign = (uint32_t)((int32_t)width >> 31);
    magnitude = (width + sign) ^ sign;
    if ((int32_t)position < (int32_t)magnitude)
    {
        w_u16(output + 2, 1);
        w_u16(output, 4);
        return 0;
    }
    sign = (uint32_t)((int32_t)main_width >> 31);
    magnitude = (main_width + sign) ^ sign;
    if ((int32_t)position < (int32_t)magnitude)
    {
        value = r_u16(block + 18);
        w_u16(output, 1);
        w_u16(output + 2, (uint16_t)value);
        return 0;
    }
    w_u16(output + 2, 0);
    w_u16(output, 0);
    return 1;
}

/* 8003EF34: select/reuse a surface interval and update its lateral bands. */
uint32_t sub_8003EF34(uint32_t record, uint32_t position, uint32_t output)
{
    FUNCTION_MARKER(0x8003EF34u, "SLUS_010.53");
    uint32_t segment = r_u32(record), route = r_u32(record + 4), subrecord = r_u32(record + 8);
    uint32_t kind = (uint32_t)r_s16((route + 2)), piece = r_u32(record + 12), count = 0, interval, surface = 0, changed = 0, flags = 0;
    uint32_t progress, last, lateral, start, end, index, base, i, p, value;
    if (!kind)
    {
        count = (uint32_t)r_s16((subrecord + 16));
        if (((int32_t)(uint32_t)(count)) < 0)
            count = 0;
    }
    interval = r_u32(output + 4);
    index = (uint32_t)r_s16((subrecord + 8)) + (uint32_t)r_s16((subrecord + 10));
    progress = r_u32(position + 8);
    last = r_u32(segment + 52) + 52u * index - 52u;
    kind = (uint32_t)r_s16(piece);
    lateral = r_u32(record + 16);
    if ((!kind && ((int32_t)(uint32_t)(progress)) < ((int32_t)(uint32_t)(r_u32(piece + 40)))) || (piece == last && ((int32_t)(uint32_t)(r_u32(piece + 40) + r_u32(piece + 32))) < ((int32_t)(uint32_t)(progress))))
    {
        start = r_u32(piece + 40);
        end = start + r_u32(piece + 32);
        progress = r_u32(position + 8);
        if (((int32_t)(uint32_t)(end)) < ((int32_t)(uint32_t)(progress)))
            progress = end;
        if (((int32_t)(uint32_t)(start)) >= ((int32_t)(uint32_t)(progress)))
            progress = start;
    }
    if (!count || !r_u32(output))
        interval = 0;
    if (interval)
    {
        start = r_u32(interval + 8);
        value = r_u32(position + 8);
        if (((int32_t)(uint32_t)(value)) < ((int32_t)(uint32_t)(start)) || ((int32_t)(uint32_t)(r_u32(interval + 12))) < ((int32_t)(uint32_t)(value)) || ((int32_t)(uint32_t)((uint32_t)r_s16((interval + 4)))) < 0)
            interval = 0;
        else
            surface = r_u32(output);
    }
    if (((int32_t)(uint32_t)(count)) > 0 && !interval)
    {
        progress = ((uint32_t)((!((uint32_t)(16))) ? (((uint32_t)(progress))) : ((((uint32_t)(progress)) >> ((uint32_t)(16))) | ((((uint32_t)(progress)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(16)))) : 0))));
        index = (uint32_t)r_s16((subrecord + 18));
        base = r_u32(segment + 56);
        for (i = 0; ((int32_t)(uint32_t)(i)) < ((int32_t)(uint32_t)(count)); ++i)
        {
            p = base + 16u * (index + i);
            if (((int32_t)(uint32_t)(progress)) >= ((int32_t)(uint32_t)((uint32_t)r_s16((p + 10)))) && ((int32_t)(uint32_t)((uint32_t)r_s16((p + 14)))) >= ((int32_t)(uint32_t)(progress)))
            {
                interval = p;
                value = (uint32_t)r_s16((p + 4));
                surface = 0;
                if (((int32_t)(uint32_t)(value)) >= 0)
                    surface = r_u32(segment + 60) + 264u * value;
                break;
            }
        }
        changed = 1;
    }
    if (surface)
    {
        w_u32(output + 52, r_u32(surface + 4));
        w_u32(output + 40, r_u32(surface + 144));
        w_u16(output + 48, r_u16(surface + 140));
        w_u32(output + 28, r_u32(surface + 16));
        w_u16(output + 36, r_u16(surface + 12));
        flags = sub_8003E67C(surface, lateral, ((int32_t)(uint32_t)(lateral)) < 0 ? 1u : 2u, output + 20);
    }
    value = r_u32(output + 16);
    w_u32(output, surface);
    w_u32(output + 4, interval);
    w_u32(output + 8, 0);
    w_u32(output + 12, 0);
    w_u32(output + 16, (value & 0xfffffff0u) | flags);
    return changed;
}

/* 800B6AAC falls through800B6B04: each product is shifted before summation. */

uint32_t sub_800B6AAC(const uint32_t point[3], const uint16_t normal[3], const uint32_t origin[3])
{
    uint32_t axis;
    uint32_t result = 0;
    FUNCTION_MARKER(0x800B6AAC, "RASHCDG.BIN");
    for (axis = 0; axis < 3; ++axis)
    {
        uint32_t component = (uint32_t)(int32_t)(int16_t)xport_load_le16((const uint8_t *)normal + 2u * axis) << 4;
        uint32_t value = xport_load_le32((const uint8_t *)point + 4u * axis);
        uint32_t base = xport_load_le32((const uint8_t *)origin + 4u * axis);
        int64_t product = (int64_t)((int32_t)(uint32_t)(component)) * (int64_t)((int32_t)(uint32_t)(value - base));
        result += (uint32_t)((uint64_t)product >> 16);
    }
    return result;
}

uint32_t sub_8003EB58(uint32_t point, uint32_t header, uint32_t *first, uint32_t *next, uint32_t hint)
{
    uint32_t result = 0xFFFFFFFFu, current = 0u, following = 0u, entry, planes[4];
    int32_t count;
    uint16_t normal[3];
    FUNCTION_MARKER(0x8003EB58u, "SLUS_010.53");
    if ((int32_t)hint >= 0)
    {
        count = r_s16(header + 2u);
        if ((int32_t)hint < count)
        {
            result = hint;
            current = hint;
            if ((int32_t)hint < count - 1)
                following = hint + 1u;
            goto outputs;
        }
    }
    count = r_s16(header + 2u);
    if (count > 0)
    {
        do
        {
            /* Reject input exceeding the original four-plane local buffer */
            if (current >= 4u)
                abort();
            entry = header + 8u + 24u * current;
            xport_store_le16(normal, (uint16_t)(0u - r_u16(entry + 10u)));
            xport_store_le16((uint8_t *)normal + 2u, r_u16(entry + 8u));
            xport_store_le16((uint8_t *)normal + 4u, r_u16(entry + 6u));
            planes[current++] = sub_800B6AAC(xport_guest_cptr(point, 12), normal, xport_guest_cptr(entry + 12u, 12));
            count = r_s16(header + 2u);
        } while ((int32_t)current < count);
        count = r_s16(header + 2u);
    }
    current = 0u;
    if (count > 0)
    {
        do
        {
            following = (int32_t)current < count - 1 ? current + 1u : 0u;
            ++result;
            if ((int32_t)planes[current] <= 0 && (int32_t)planes[following] > 0)
                break;
            ++current;
        } while ((int32_t)current < count);
    }
outputs:
    if ((int32_t)result >= 0)
    {
        if (first)
            xport_store_le32(first, header + 24u * current + 8u);
        if (next)
            xport_store_le32(next, header + 24u * following + 8u);
    }
    return result;
}

/* 8003F204: junction contact descriptor selected by plane index or pair. */
void sub_8003F204(uint32_t object, uint32_t hint)
{
    FUNCTION_MARKER(0x8003F204u, "SLUS_010.53");
    uint32_t header, first = 0, next = 0, index, segment, count, entry, i, a, b, found = 0;
    if (!object)
        return;
    header = sub_80039AFC(r_u32(r_u32(object + 328)));
    if (!header)
        return;
    index = sub_8003EB58(object + 184, header, &first, &next, hint);
    if (((int32_t)(uint32_t)(index)) < 0)
        return;
    segment = r_u32(object + 328);
    count = (uint32_t)r_s16((segment + 78));
    if (((int32_t)(uint32_t)(count)) >= 4)
        count = 3;
    entry = r_u32(segment + 92);
    if (entry && ((int32_t)(uint32_t)(count)) > 0)
    {
        for (i = 0; ((int32_t)(uint32_t)(i)) < ((int32_t)(uint32_t)(count)); ++i, entry += 20)
        {
            if ((uint32_t)r_u8((entry + 2)) != 1)
                continue;
            a = (uint32_t)r_u8((entry + 8));
            if (a == 255)
            {
                if (index == (uint32_t)r_u8((entry + 3)))
                    found = 1;
            }
            else
            {
                b = (uint32_t)r_s16((first + 4));
                if (a == b && (uint32_t)r_u8((entry + 9)) == (uint32_t)r_s16((next + 4)))
                    found = 1;
                else if ((uint32_t)r_u8((entry + 9)) == b && a == (uint32_t)r_s16((next + 4)))
                    found = 1;
            }
            if (found)
                break;
        }
    }
    if (!found)
    {
        w_u32(object + 496, 0);
        return;
    }
    w_u32(object + 496, object + 500);
    w_u8(object + 502, (uint8_t)r_u8(entry));
    w_u16(object + 500, r_u16(entry + 4));
    w_u8(object + 503, (uint8_t)r_u8((entry + 6)));
}

uint32_t sub_8003F1F0(uint32_t object)
{
    FUNCTION_MARKER(0x8003F1F0u, "SLUS_010.53");
    uint32_t result = r_u32(object + 340) + 44u;
    w_u32(object + 492, result);
    return result;
}

/* Both contact update branches are used for effects, with no caller return use. */
void sub_8003DF54(uint32_t object, uint32_t reset, uint32_t hint)
{
    FUNCTION_MARKER(0x8003DF54u, "SLUS_010.53");
    uint32_t previous;
    if (reset == 1)
        (void)sub_8001E100(object + 492, 0, 12);
    if (r_u16(object + 362) == 1)
    {
        previous = r_u32(object + 496);
        w_u32(object + 492, 0);
        if (previous == 0xffffffffu)
            w_u32(object + 496, 0);
        sub_8003F204(object, hint);
    }
    else
    {
        previous = r_u32(object + 492);
        w_u32(object + 496, 0);
        if (previous == 0xffffffffu)
            w_u32(object + 492, 0);
        (void)sub_8003F1F0(object);
    }
}

uint32_t sub_8003BD2C(uint32_t record, uint32_t first, uint32_t next, uint32_t *offset, uint32_t *entry)
{
    uint32_t metadata, index = 0, manager, candidate, link, route, root, table, segment, value;
    FUNCTION_MARKER(0x8003BD2C, "SLUS_010.53");
    if (!record)
        return 0;
    metadata = r_u32(record + 24);
    if (!metadata || r_s16(metadata + 8) <= 0)
        return 0;
    do
    {
        segment = (uint32_t)r_s16(metadata + 6) + index;
        manager = r_u32(0x8005B240);
        candidate = r_u32(manager + 52) + 12 * segment;
        if ((uint32_t)r_s16(candidate + 8) == first && (uint32_t)r_s16(candidate + 10) == next)
        {
            segment = (uint32_t)r_s16(candidate + 2);
            link = r_u32(manager + 56) + 12 * segment;
            route = r_u32(record + 4);
            segment = (uint32_t)r_s16(link + 4) + (uint32_t)r_s16(route + 20);
            root = r_u32(record);
            value = (uint32_t)r_s16(link + 8);
            table = r_u32(root + 48);
            *offset = value;
            *entry = candidate;
            return table + 28 * segment;
        }
        metadata = r_u32(record + 24);
        ++index;
    } while ((int32_t)index < r_s16(metadata + 8));
    return 0;
}

/* 8003E61C: signed fixed-point blend, truncating each product separately. */
uint32_t sub_8003E61C(uint32_t weight, uint32_t first, uint32_t next)
{
    FUNCTION_MARKER(0x8003E61Cu, "SLUS_010.53");
    int64_t a = (int64_t)((int32_t)(uint32_t)(65536u - weight)) * (int64_t)((int32_t)(uint32_t)(first));
    int64_t b = (int64_t)((int32_t)(uint32_t)(weight)) * (int64_t)((int32_t)(uint32_t)(next));
    return (uint32_t)((uint64_t)a >> 16) + (uint32_t)((uint64_t)b >> 16);
}

uint32_t sub_8003E45C(uint32_t first, uint32_t next, uint32_t weight, uint32_t position, uint32_t side_a, uint32_t side_b, uint32_t reference, uint32_t output)
{
    uint32_t a, b, main_width, inner, x, y, sign, magnitude, value;
    int32_t count_a, count_b;
    FUNCTION_MARKER(0x8003E45C, "SLUS_010.53");
    if ((int32_t)(position ^ reference) < 0)
        position = 0;
    sign = (uint32_t)((int32_t)position >> 31);
    position = (position + sign) ^ sign;
    a = first + (side_a == 2 ? 136 : 8);
    b = next + (side_b == 2 ? 136 : 8);
    x = r_u32(a + 8);
    y = r_u32(b + 8);
    sign = (uint32_t)((int32_t)x >> 31);
    x = (x + sign) ^ sign;
    sign = (uint32_t)((int32_t)y >> 31);
    y = (y + sign) ^ sign;
    main_width = sub_8003E61C(weight, x, y);
    inner = main_width;
    x = r_u32(a + 8);
    y = r_u32(b + 8);
    sign = (uint32_t)((int32_t)x >> 31);
    x = (x + sign) ^ sign;
    sign = (uint32_t)((int32_t)y >> 31);
    y = (y + sign) ^ sign;
    count_a = r_s16(a + 2);
    if (count_a > 0)
    {
        x = r_u32(a + 24);
        sign = (uint32_t)((int32_t)x >> 31);
        x = (x + sign) ^ sign;
        count_b = r_s16(b + 2);
        goto interpolate_inner;
    }
    count_b = r_s16(b + 2);
    if (count_b > 0)
    {
        if (count_a)
        {
            x = r_u32(a + 24);
            sign = (uint32_t)((int32_t)x >> 31);
            x = (x + sign) ^ sign;
            count_b = r_s16(b + 2);
        }
interpolate_inner:
        if (count_b)
        {
            y = r_u32(b + 24);
            sign = (uint32_t)((int32_t)y >> 31);
            y = (y + sign) ^ sign;
        }
        inner = sub_8003E61C(weight, x, y);
    }
    sign = (uint32_t)((int32_t)inner >> 31);
    magnitude = (inner + sign) ^ sign;
    if ((int32_t)position < (int32_t)magnitude)
    {
        w_u16(output + 2, 1);
        w_u32(output + 4, 0);
        w_u16(output, 4);
        return 0;
    }
    sign = (uint32_t)((int32_t)main_width >> 31);
    magnitude = (main_width + sign) ^ sign;
    if ((int32_t)position < (int32_t)magnitude)
    {
        w_u32(output + 4, inner);
        value = r_u16(a + 18);
        w_u16(output, 1);
        w_u16(output + 2, (uint16_t)value);
        return 0;
    }
    w_u32(output + 4, main_width);
    w_u16(output + 2, 0);
    w_u16(output, 0);
    return 1;
}

uint32_t sub_8003ED14(uint32_t first, uint32_t interval, uint32_t orientation_a, uint32_t orientation_b, uint32_t next, uint32_t position, uint32_t weight, uint32_t output, uint32_t reference)
{
    uint32_t side_a, side_b, a, b, x, y, sign, width, material, flags, value, old;
    FUNCTION_MARKER(0x8003ED14, "SLUS_010.53");
    side_a = (int32_t)orientation_a > 0 ? 1 : 2;
    side_b = (int32_t)orientation_b > 0 ? 2 : 1;
    a = first + (side_a == 2 ? 136 : 8);
    b = next + (side_b == 2 ? 136 : 8);
    x = r_u32(a + 8);
    y = r_u32(b + 8);
    sign = (uint32_t)((int32_t)x >> 31);
    x = (x + sign) ^ sign;
    sign = (uint32_t)((int32_t)y >> 31);
    y = (y + sign) ^ sign;
    width = sub_8003E61C(weight, x, y);
    material = r_u16(a + 4);
    flags = sub_8003E45C(first, next, weight, position, side_a, side_b, reference, output + 20);
    x = r_u32(first + 4);
    y = r_u32(next + 4);
    value = sub_8003E61C(weight, x, y);
    w_u32(output + 28, 0);
    w_u16(output + 36, 0);
    w_u32(output + 40, width);
    old = r_u32(output + 16);
    w_u32(output + 52, value);
    w_u32(output, first);
    w_u32(output + 4, interval);
    w_u32(output + 8, next);
    w_u32(output + 12, interval);
    w_u32(output + 16, (old & 0xFFFFFFF0) | flags);
    w_u16(output + 48, (uint16_t)material);
    return 0;
}

void sub_80036800(uint32_t point, uint32_t piece, uint32_t *lateral, uint32_t *longitudinal)
{
    uint32_t sum, delta, axis;
    int32_t normal;
    int64_t product;
    FUNCTION_MARKER(0x80036800, "SLUS_010.53");
    if (lateral)
    {
        sum = 0;
        for (axis = 0; axis < 3; ++axis)
        {
            normal = r_s16(piece + 2 + 2 * axis);
            delta = r_u32(point + 4 * axis) - r_u32(piece + 20 + 4 * axis);
            product = (int64_t)(normal * 16) * (int32_t)delta;
            sum += (uint32_t)((uint64_t)product >> 16);
        }
        *lateral = sum;
    }
    if (longitudinal)
    {
        sum = 0;
        for (axis = 0; axis < 3; ++axis)
        {
            normal = r_s16(piece + 14 + 2 * axis);
            delta = r_u32(point + 4 * axis) - r_u32(piece + 20 + 4 * axis);
            product = (int64_t)(normal * 16) * (int32_t)delta;
            sum += (uint32_t)((uint64_t)product >> 16);
        }
        *longitudinal = sum;
    }
}

/* 8003697C: distance from a segment start or its opposite endpoint. */
uint32_t sub_8003697C(uint32_t previous, uint32_t direction, uint32_t piece, uint32_t point)
{
    FUNCTION_MARKER(0x8003697Cu, "SLUS_010.53");
    uint32_t origin[3], axis[3], i, length, result = 0;
    for (i = 0; i < 3; ++i)
    {
        origin[i] = r_u32(piece + 20 + 4 * i);
        axis[i] = (uint32_t)r_s16((piece + 14 + 2 * i)) << 4;
    }
    if (((int32_t)(uint32_t)(direction)) <= 0 && previous != direction)
    {
        length = r_u32(piece + 32);
        for (i = 0; i < 3; ++i)
        {
            int64_t product = (int64_t)((int32_t)(uint32_t)(axis[i])) * (int64_t)((int32_t)(uint32_t)(length));
            origin[i] += (uint32_t)((uint64_t)product >> 16);
        }
    }
    for (i = 0; i < 3; ++i)
    {
        uint32_t delta = r_u32(point + 4 * i) - origin[i];
        int64_t product = (int64_t)((int32_t)(uint32_t)(axis[i])) * (int64_t)((int32_t)(uint32_t)(delta));
        result += (uint32_t)((uint64_t)product >> 16);
    }
    return result;
}

/* 8001FC58: original 32-bit LCG, including seed writeback. */
uint32_t sub_8001FC58(void)
{
    FUNCTION_MARKER(0x8001FC58u, "SLUS_010.53");
    uint32_t value = r_u32(0x8005B4A8u) * 1664525u + 1013904223u;
    w_u32(0x8005B4A8u, value);
    return value;
}

/* 80039C38: signed link index into the manager's 12-byte records. */
uint32_t sub_80039C38(uint32_t link)
{
    FUNCTION_MARKER(0x80039C38u, "SLUS_010.53");
    uint32_t index, manager;
    if (!link)
        return 0;
    index = (uint32_t)r_s16((link + 2));
    if (((int32_t)(uint32_t)(index)) < 0)
        return 0;
    manager = r_u32(0x8005B240u);
    if (((int32_t)(uint32_t)(index)) >= ((int32_t)(uint32_t)((uint32_t)r_s16((manager + 64)))))
        return 0;
    return r_u32(manager + 56) + 12u * index;
}

/* 8003F580: membership in a route's signed-count road list. */
uint32_t sub_8003F580(uint32_t record, uint32_t road)
{
    FUNCTION_MARKER(0x8003F580u, "SLUS_010.53");
    uint32_t count, i;
    if (!record || ((int32_t)(uint32_t)(road)) < 0)
        return 0;
    count = r_u32(record + 16);
    for (i = 0; ((int32_t)(uint32_t)(i)) < ((int32_t)(uint32_t)(count)); ++i)
    {
        if (r_u32(record + 84) == road)
            return 1;
        record += 4;
    }
    return 0;
}

/* 8003F5D0: find the first 120-byte route containing a road. */
uint32_t sub_8003F5D0(uint32_t road)
{
    FUNCTION_MARKER(0x8003F5D0u, "SLUS_010.53");
    uint32_t i = 0, offset = 0, record;
    if (((int32_t)(uint32_t)((uint32_t)r_s16(0x800D6182u) + 1u)) <= 0)
        return 0;
    do
    {
        record = r_u32(0x800D6194u) + offset;
        if (sub_8003F580(record, road))
            return record;
        ++i;
        offset += 120;
    } while (((int32_t)(uint32_t)(i)) < ((int32_t)(uint32_t)((uint32_t)r_s16(0x800D6182u) + 1u)));
    return 0;
}

/* 8003C840/8003C948: resolve the loaded segment beyond either endpoint. */
static uint32_t adjacent_track_segment(uint32_t segment, uint32_t link_id, unsigned backward)
{
    uint32_t record = sub_80039A08(r_u32(segment)), target = 0xffffffffu, header, link;
    if (!record)
        return 0;
    if (!(uint32_t)r_s16((record + 4)))
    {
        header = sub_80039AA0(r_u32(segment));
        if (header)
            target = (uint32_t)r_s16((header + (backward ? 4u : 2u)));
    }
    else
    {
        header = sub_80039AFC(r_u32(segment));
        if (header)
        {
            if (!r_u32(record + 28))
                target = (uint32_t)r_s16((header + 6));
            else
            {
                link = sub_8003A37C(header, link_id);
                if (link)
                {
                    unsigned positive = ((int32_t)(uint32_t)((uint32_t)r_s16((link + 2)))) > 0;
                    target = (uint32_t)r_s16((((positive != backward) ? link : header + 6)));
                }
            }
        }
    }
    record = sub_80039A08(target);
    return record ? r_u32(record + 12) : 0;
}

uint32_t sub_8003C840(uint32_t actor, uint32_t segment, uint32_t link_id)
{
    FUNCTION_MARKER(0x8003C840u, "SLUS_010.53");
    return adjacent_track_segment(segment, link_id, 0);
}

uint32_t sub_8003C948(uint32_t actor, uint32_t segment, uint32_t link_id)
{
    FUNCTION_MARKER(0x8003C948u, "SLUS_010.53");
    return adjacent_track_segment(segment, link_id, 1);
}

/* 8003F408: find an explicitly connected route; table count is exclusive. */
uint32_t sub_8003F408(uint32_t record, uint32_t id)
{
    FUNCTION_MARKER(0x8003F408u, "SLUS_010.53");
    uint32_t result = 0, count, i, n, j, p;
    if ((uint32_t)r_s16(0x800D6182u) == 0xffffffffu)
        return 0;
    if (!record || id == 0xffffffffu)
        return 0;
    if (r_u32(record) == id)
        return record;
    count = r_u32(record + 16);
    for (i = 0; ((int32_t)(uint32_t)(i)) < ((int32_t)(uint32_t)(count)); ++i, record += 4)
    {
        if (r_u32(record + 100) != id)
            continue;
        n = (uint32_t)r_s16(0x800D6182u);
        if (((int32_t)(uint32_t)(n)) <= 0)
            continue;
        p = r_u32(0x800D6194u);
        for (j = 0; ((int32_t)(uint32_t)(j)) < ((int32_t)(uint32_t)(n)); ++j, p += 120)
        {
            if (r_u32(p) == id)
            {
                result = p;
                break;
            }
        }
    }
    return result;
}

/* 8003CCA0: choose a candidate road for a player or pursuing actor. */

uint32_t sub_8003CCA0(uint32_t actor, const uint32_t candidates[24], uint32_t count)
{
    uint32_t desired = 0xFFFFFFFF, result = count - 2;
    uint32_t identity, target, road, random_index = 0, selected_route, index;
    int player;

    FUNCTION_MARKER(0x8003CCA0, "SLUS_010.53");
    if (r_u16(actor + 172) >= r_u32(r_u32(0x8005B2F8) + 48) && (r_u8(r_u32(actor + 1084) + 1) & 15) == 2 && r_s32(actor + 480) < 131)
    {
        index = (uint32_t)(int32_t)r_s8(actor + 946);
        identity = r_u16(actor + 8 * (index - 1) + 958);
        if (identity != 224 && !(identity >> 5))
        {
            target = r_u32(0x8005B3A0) + 1096 * identity;
            if (target)
                desired = r_u16(target + 360);
        }
    }
    if (r_s16(0x800D6182) == -1)
        return count == 2 ? sub_8001FC58() % count : result;
    road = candidates[0];
    target = r_u32(actor + 428);
    road = r_u32(road + 8);
    selected_route = sub_8003F408(target, road);
    if (!selected_route)
        return result;
    if (r_s32(selected_route + 16) >= 2)
    {
        uint32_t random = sub_8001FC58();
        random_index = count ? random % count : random;
    }
    if ((int32_t)count <= 0)
        return result;
    target = selected_route + 4 * random_index;
    player = r_u16(actor + 172) < r_u32(r_u32(0x8005B2F8) + 48);
    for (index = 0; (int32_t)index < (int32_t)count; ++index)
    {
        if (!player && (r_u8(r_u32(actor + 1084) + 1) & 15) == 2 && desired != 0xFFFFFFFF)
        {
            road = (uint32_t)(int32_t)r_s16(candidates[8 * index + 7] + 10);
            if (road == desired)
                return index;
        }
        else
        {
            road = (uint32_t)(int32_t)r_s16(candidates[8 * index + 7] + 10);
            if (road == r_u32(target + 84))
                return index;
        }
    }
    return result;
}

uint32_t sub_80039048(uint32_t identity_ptr, uint32_t candidates[24], uint32_t directions[3], uint32_t count, uint32_t output[8], uint32_t *direction_out)
{
    uint32_t kind = 6, actor = 0, index = 0xFFFFFFFF, desired = 0xFFFFFFFF;
    uint32_t identity, traffic, player, road, value, old, current;

    FUNCTION_MARKER(0x80039048, "SLUS_010.53");
    if (identity_ptr)
    {
        identity = r_u16(identity_ptr);
        kind = identity >> 5;
        if (!kind)
            actor = r_u32(0x8005B3A0) + 1096 * identity;
    }
    if (!candidates[7])
    {
        if (actor && r_u32(actor + 356))
        {
            w_u8(actor + 947, 255);
            w_u32(actor + 948, 0);
            w_u32(actor + 952, 0);
        }
        if (output[2] == candidates[2])
            output[3] = candidates[3];
        else
            (void)sub_8001E0B4(output, candidates, 32);
        value = directions[0];
        if (value == 1 || value == 0xFFFFFFFF)
            *direction_out = value;
        return output[3];
    }
    if ((int32_t)count < 2)
    {
        if (output[2] == candidates[2])
            output[3] = candidates[3];
        else
        {
            (void)sub_8001E0B4(output, candidates, 32);
            *direction_out = directions[0];
        }
        return output[3];
    }
    if (kind == 3)
    {
        identity = r_u16(identity_ptr);
        traffic = 0x800CF660 + 512 * (identity & 31);
        if (sub_8003F3B4(r_u32(candidates[1] + 12)))
        {
            if ((r_u8(r_u32(0x8005B2F8) + 4) & 1) && !r_u32(traffic + 180))
            {
                player = r_u8(traffic + 511);
                if ((int32_t)player < r_s32(0x8005B1F8))
                {
                    current = r_u32(0x8005B3A0) + 1096 * player;
                    if (current)
                    {
                        value = r_u32(current + 360);
                        if (!(value >> 16))
                            desired = value;
                    }
                }
            }
            for (current = 0; (int32_t)current < (int32_t)count; ++current)
            {
                value = r_u8(r_u32(0x8005B2F8) + 4);
                road = (uint32_t)(int32_t)r_s16(candidates[8 * current + 7] + 10);
                if (value & 16)
                {
                    if (sub_8003F5D0(road) && road - 11 >= 2 && road != 20 && road != 26)
                    {
                        index = current;
                        break;
                    }
                }
                else if ((desired != 0xFFFFFFFF && road == desired) || sub_8003F5D0(road))
                {
                    index = current;
                    break;
                }
            }
        }
        if (index == 0xFFFFFFFF)
        {
            if (r_u8(r_u32(0x8005B2F8) + 4) & 16)
            {
                for (current = 0; (int32_t)current < (int32_t)count; ++current)
                {
                    value = r_u16(candidates[8 * current + 7] + 10);
                    road = (uint32_t)(int32_t)(int16_t)value;
                    if (value - 11 >= 2 && road != 20 && road != 26)
                    {
                        index = current;
                        break;
                    }
                }
            }
            else
                index = sub_8001FC58() % count;
        }
    }
    else if (kind)
        index = sub_8001FC58() % count;
    else if (actor)
    {
        if (r_u16(actor + 172) < r_u32(r_u32(0x8005B2F8) + 48) || (r_u8(r_u32(actor + 1084) + 1) & 15) != 2)
        {
            old = r_u32(actor + 948);
            if (old)
            {
                int32_t old_route = r_s16(old + 2);
                int32_t next_route = r_s16(candidates[6] + 2);
                if (old_route == next_route)
                    index = (uint32_t)(int32_t)r_s8(actor + 947);
                else
                {
                    w_u8(actor + 947, 255);
                    w_u32(actor + 948, 0);
                    w_u32(actor + 952, 0);
                }
            }
        }
        if (index == 0xFFFFFFFF)
            index = sub_8003CCA0(actor, candidates, count);
    }
    /* Repair the original out-of-range route selection without reading before either array */
    if (index >= count)
        index = 0;
    (void)sub_8001E0B4(output, candidates + 8 * index, 32);
    *direction_out = directions[index];
    if (actor && !r_u32(actor + 948))
    {
        w_u8(actor + 947, (uint8_t)index);
        w_u32(actor + 948, output[6]);
        w_u32(actor + 952, output[7]);
    }
    return output[3];
}

uint32_t sub_80039BB0(uint32_t metadata, uint32_t id, uint32_t *output, uint32_t limit)
{
    uint32_t matches = 0, index = 0, manager, entry;
    FUNCTION_MARKER(0x80039BB0, "SLUS_010.53");
    if (!metadata || r_s16(metadata + 8) <= 0)
        return 0;
    manager = r_u32(0x8005B240);
    do
    {
        entry = r_u32(manager + 52) + 12 * ((uint32_t)r_s16(metadata + 6) + index);
        if ((uint32_t)r_s16(entry + 8) == id)
        {
            if ((int32_t)limit < (int32_t)matches)
                return UINT32_MAX;
            output[matches++] = entry;
        }
        ++index;
    } while ((int32_t)index < r_s16(metadata + 8));
    return matches;
}

/* Enumerate the next track pieces in either direction */
static uint32_t traverse_track_candidates(uint32_t actor, uint32_t record[8], uint32_t *output, uint32_t *directions, uint32_t limit, unsigned backward)
{
    uint32_t segment = record[0], route = record[1], sub = record[2];
    uint32_t last = record[7], saved = record[3], piece = 0, state = 0, count = 0;
    uint32_t old, header, link = 0xffffffff, lookup, metadata, n, i, index, base, value, positive;
    uint32_t *p, *links;
    int32_t capacity;
    if (backward ? (uint32_t)r_s16(saved) != 0 : ((int32_t)(uint32_t)((uint32_t)r_s16(saved))) < ((int32_t)(uint32_t)((uint32_t)r_s16((sub + 10)) - 1)))
    {
        piece = backward ? saved - 52 : saved + 52;
        route = 0;
        state = 1;
        goto tail;
    }
    old = segment;
    if (!(uint32_t)r_s16((segment + 16)))
    {
        segment = backward ? sub_8003C948(actor, segment, 0xffffffff) : sub_8003C840(actor, segment, 0xffffffff);
        if (!segment)
            goto tail;
        route = !(uint32_t)r_s16((segment + 16)) ? r_u32(segment + 44) : sub_80039C90(segment, r_u32(old + 8));
        if (route)
            state = 1;
        goto tail;
    }
    header = sub_80039AFC(r_u32(segment));
    if (!header)
        goto tail;
    if (r_u32(segment + 12) == 1)
    {
        link = sub_8003A37C(header, r_u32(route + 12));
        if (!link)
            goto tail;
        value = r_u32(route + 12);
        segment = backward ? sub_8003C948(actor, segment, value) : sub_8003C840(actor, segment, value);
        if (!segment)
            goto tail;
        route = !(uint32_t)r_s16((segment + 16)) ? r_u32(segment + 44) : sub_80039C90(segment, (uint32_t)r_s16((link + 4)));
        if (route)
            state = 1;
        goto tail;
    }
    if ((uint32_t)r_s16((route + 2)))
    {
        if (!backward && !last)
            goto tail;
        if (backward)
            route = 0;
        value = 0xffffffff;
        if (last)
        {
            lookup = sub_80039C38(last);
            /* Repair the backward original null-lookup dereference */
            if (!lookup)
                goto tail;
            positive = ((int32_t)(uint32_t)((uint32_t)r_s16((last + 6)))) > 0;
            index = ((int32_t)(uint32_t)((uint32_t)r_s16((lookup + 8)))) > 0;
            value = (uint32_t)r_s16((last + ((positive == index) ? (backward ? 8 : 10) : (backward ? 10 : 8))));
        }
        route = 0;
        link = sub_8003A37C(header, value);
        if (link)
            route = sub_80039C90(segment, (uint32_t)r_s16((link + 4)));
        if (route)
        {
            base = r_u32(segment + 52);
            sub = r_u32(segment + 48) + 28 * (uint32_t)r_s16((route + 20));
            index = (uint32_t)r_s16((sub + 8));
            value = (uint32_t)r_s16((link + 2));
            if (backward ? ((int32_t)(uint32_t)(value)) <= 0 : ((int32_t)(uint32_t)(value)) < 0)
                index += (uint32_t)r_s16((sub + 10)) - 1;
            piece = base + 52 * index;
            state = 2;
            directions[0] = (uint32_t)r_s16((link + 2));
        }
        goto tail;
    }
    link = sub_8003A37C(header, r_u32(route + 12));
    if (!link)
        goto tail;
    positive = ((int32_t)(uint32_t)((uint32_t)r_s16((link + 2)))) > 0;
    if (positive != backward)
    {
        value = r_u32(route + 12);
        route = 0;
        segment = backward ? sub_8003C948(actor, segment, value) : sub_8003C840(actor, segment, value);
        if (segment)
            route = sub_80039C90(segment, (uint32_t)r_s16((link + 4)));
        if (route)
            state = 1;
        goto tail;
    }
    metadata = sub_80039B60(r_u32(segment));
    if (!metadata)
        goto tail;
    /* Allocate all metadata entries to repair the original four-slot overflow */
    capacity = r_s16(metadata + 8);
    links = malloc((size_t)(capacity > 0 ? capacity : 1) * sizeof(*links));
    if (!links)
        abort();
    n = sub_80039BB0(metadata, (uint32_t)r_s16(link + 4), links, limit);
    if ((int32_t)n <= 0 || (int32_t)n >= (int32_t)limit)
    {
        free(links);
        goto tail;
    }
    for (i = 0; i < n; ++i)
    {
        last = links[i];
        lookup = sub_80039C38(last);
        if (!lookup)
            continue;
        route = r_u32(segment + 44) + 32 * (uint32_t)r_s16((lookup + 2));
        sub = r_u32(segment + 48) + 28 * ((uint32_t)r_s16((route + 20)) + (uint32_t)r_s16((lookup + 4)));
        positive = ((int32_t)(uint32_t)((uint32_t)r_s16((last + 6)))) > 0;
        index = (uint32_t)r_s16((sub + 8));
        value = (uint32_t)r_s16((lookup + 8));
        base = r_u32(segment + 52);
        if (positive != (uint32_t)(((int32_t)(uint32_t)(value)) > 0))
            index += (uint32_t)r_s16((sub + 10)) - 1;
        piece = base + 52 * index;
        value = (uint32_t)r_s16((lookup + 8));
        directions[0] = positive ? value : 0 - value;
        ++directions;
        p = output + 8 * count;
        p[5] = record[5];
        value = record[4];
        p[0] = segment;
        p[1] = route;
        p[2] = sub;
        p[3] = piece;
        p[7] = last;
        p[6] = metadata;
        p[4] = value;
        ++count;
        state = 1;
    }
    free(links);
tail:
    if (!state)
    {
        p = output + 8 * count;
        p[2] = record[2];
        p[3] = record[3];
        return count + 1;
    }
    if (count)
        return count;
    if (!route)
    {
        value = record[2];
        if (sub == value || piece == saved)
        {
            output[2] = value;
            output[3] = piece;
            return 1;
        }
    }
    if (state != 2)
    {
        base = r_u32(segment + 52);
        sub = r_u32(segment + 48) + 28 * (uint32_t)r_s16((route + 20));
        index = (uint32_t)r_s16((sub + 8));
        if (backward)
            index += (uint32_t)r_s16((sub + 10)) - 1;
        directions[0] = backward ? 0xffffffff : 1;
        piece = base + 52 * index;
    }
    output[5] = record[5];
    value = record[4];
    output[0] = segment;
    output[1] = route;
    output[2] = sub;
    output[3] = piece;
    output[6] = 0;
    output[7] = 0;
    output[4] = value;
    return 1;
}

uint32_t sub_80037A30(uint32_t actor, uint32_t record[8], uint32_t output[24], uint32_t directions[3], uint32_t limit)
{
    FUNCTION_MARKER(0x80037A30u, "SLUS_010.53");
    return traverse_track_candidates(actor, record, output, directions, limit, 0);
}

uint32_t sub_80037FBC(uint32_t actor, uint32_t record[8], uint32_t output[24], uint32_t directions[3], uint32_t limit)
{
    FUNCTION_MARKER(0x80037FBCu, "SLUS_010.53");
    return traverse_track_candidates(actor, record, output, directions, limit, 1);
}

uint32_t sub_80036B14(uint32_t actor, uint32_t record[8], uint32_t point)
{
    uint32_t temporary[8] = {0}, candidates[24] = {0}, directions[3] = {0};
    uint32_t selected, candidate, distance = 0, previous, direction, last_distance = 0;
    uint32_t count, index, difference, sign, axis, delta;
    uint32_t (*traverse)(uint32_t, uint32_t *, uint32_t *, uint32_t *, uint32_t);
    FUNCTION_MARKER(0x80036B14, "SLUS_010.53");
    temporary[2] = record[2];
    temporary[3] = record[3];
    selected = record[3];
    for (axis = 0; axis < 3; ++axis)
    {
        int64_t product;
        delta = r_u32(point + 4 * axis) - r_u32(selected + 20 + 4 * axis);
        product = (int64_t)(int32_t)delta * ((int32_t)r_s16(selected + 14 + 2 * axis) * 16);
        distance += (uint32_t)((uint64_t)product >> 16);
    }
    if (!distance)
        goto done;
    direction = (int32_t)distance > 0 ? 1 : UINT32_MAX;
    previous = direction;
    directions[0] = direction;
    candidate = selected;
    index = (uint32_t)r_s16(temporary[3]);
    if (index && index != (uint32_t)r_s16(temporary[2] + 10) - 1)
    {
        candidate += 52 * previous;
        temporary[3] = candidate;
    }
    else
    {
        (void)sub_8001E0B4(temporary, record, 32);
        temporary[3] = candidate;
        traverse = (int32_t)direction <= 0 ? sub_80037FBC : sub_80037A30;
        count = traverse(actor, temporary, candidates, directions, 3);
        candidate = sub_80039048(actor, candidates, directions, count, temporary, &direction);
    }
    distance = sub_8003697C(previous, direction, candidate, point);
    while (((int32_t)direction > 0 && (int32_t)distance >= 0) || ((int32_t)direction < 0 && (int32_t)distance < 0))
    {
        if (record[2] == temporary[2])
            record[3] = temporary[3];
        else
            (void)sub_8001E0B4(record, temporary, 32);
        selected = record[3];
        if ((r_u32(actor + 216) & 128) && r_s16(selected + 50))
            w_u32(r_u32(0x8005B3A0) + 1096 * r_u16(actor) + 828, selected);
        previous = direction;
        index = (uint32_t)r_s16(temporary[3]);
        if (index && index != (uint32_t)r_s16(temporary[2] + 10) - 1)
        {
            candidate += 52 * previous;
            temporary[3] = candidate;
        }
        else
        {
            if (!temporary[0])
            {
                (void)sub_8001E0B4(temporary, record, 32);
                temporary[3] = candidate;
            }
            if (sub_800394F0(temporary, direction))
            {
                if (r_u16(actor) < r_u32(r_u32(0x8005B2F8) + 48))
                    w_u32(0x8005B580, 1);
                break;
            }
            traverse = (int32_t)direction <= 0 ? sub_80037FBC : sub_80037A30;
            count = traverse(actor, temporary, candidates, directions, 3);
            candidate = sub_80039048(actor, candidates, directions, count, temporary, &direction);
        }
        distance = sub_8003697C(previous, direction, candidate, point);
        difference = distance - last_distance;
        sign = (uint32_t)((int32_t)difference >> 31);
        difference = (difference + sign) ^ sign;
        last_distance = distance;
        if ((int32_t)difference < 131)
            break;
    }
    if ((int32_t)previous < 0 && (int32_t)direction < 0 && (int32_t)distance > 0 && (int32_t)distance < r_s32(temporary[3] + 32))
    {
        if (record[2] == temporary[2])
            record[3] = temporary[3];
        else
            (void)sub_8001E0B4(record, temporary, 32);
        selected = record[3];
        if ((r_u32(actor + 216) & 128) && r_s16(selected + 50))
            w_u32(r_u32(0x8005B3A0) + 1096 * r_u16(actor) + 828, selected);
    }
done:
    w_u32(actor + 216, r_u32(actor + 216) & 0xFFFFFF7F);
    return selected;
}

/* 8003E754: junction surface attachment, actual five-argument ABI. */
uint32_t sub_8003E754(uint32_t actor, uint32_t header, uint32_t output, uint32_t track_output[8], uint32_t hint)
{
    FUNCTION_MARKER(0x8003E754, "SLUS_010.53");
    uint32_t root = r_u32(actor + 328), first = 0, next = 0, reference = 1, link = 0;
    uint32_t record[8], vector[3] = {0}, sub, piece, length, lateral, longitudinal, index, interval, pair;
    uint32_t a, b, side_a, side_b, surface, next_surface, progress, delta, clamped, ratio, result, value, i, sign;
    uint16_t normal[3];
    if (!header || !r_u32(root + 64))
        goto missing;
    if (((int32_t)(uint32_t)(sub_8003EB58(actor + 184, header, &first, &next, hint))) < 0)
        goto missing;
    sub = sub_8003BD2C(actor + 328, (uint32_t)r_s16((first + 4)), (uint32_t)r_s16((next + 4)), &reference, &link);
    length = r_u32(sub + 12);
    (void)sub_8001E0B4(record, xport_guest_cptr(actor + 328, 32), 32);
    record[2] = sub;
    record[3] = r_u32(root + 52) + 52 * (uint32_t)r_s16((sub + 8));
    piece = sub_80036B14(actor + 172, record, actor + 184);
    sub_80036800(actor + 184, piece, &lateral, &longitudinal);
    if (r_s16(header + 2) == 2)
    {
        sign = (uint32_t)((int32_t)lateral >> 31);
        lateral = (lateral + sign) ^ sign;
    }
    record[4] = lateral;
    record[5] = longitudinal;
    for (i = 0; i < 3; ++i)
        normal[i] = r_u16(piece + 14 + 2 * i);
    /* Prior direction changes only magnitude: all downstream tests use its sign.
       This caller consumes only progress, independent of the original stale word. */
    (void)sub_8003662C(normal, record, vector);
    index = (uint32_t)r_s16((sub + 18));
    if (((int32_t)(uint32_t)(index)) < 0)
        goto missing;
    interval = r_u32(root + 56) + 16 * index;
    pair = r_u32(root + 64) + 20 * index;
    a = (uint32_t)r_s16((pair + 4));
    if (a == 0xffffffff)
        goto missing;
    b = (uint32_t)r_s16((pair + 6));
    if (b == 0xffffffff)
        goto missing;
    if ((uint32_t)r_s16((first + 4)) == (uint32_t)r_s16((pair + 10)) && (uint32_t)r_s16((next + 4)) == (uint32_t)r_s16((pair + 12)))
    {
        side_a = (uint32_t)r_s16((first + 2));
        side_b = (uint32_t)r_s16((next + 2));
        value = r_u32(root + 60);
        surface = value + 264 * a;
        next_surface = value + 264 * b;
    }
    else
    {
        b = (uint32_t)r_s16((pair + 6));
        value = r_u32(root + 60);
        a = (uint32_t)r_s16((pair + 4));
        surface = value + 264 * b;
        next_surface = value + 264 * a;
        side_a = (uint32_t)r_s16((next + 2));
        side_b = (uint32_t)r_s16((first + 2));
    }
    progress = vector[2];
    delta = length - progress;
    clamped = (~(uint32_t)((int32_t)progress >> 31) & progress) + ((uint32_t)((int32_t)delta >> 31) & delta);
    ratio = sub_80010028((int32_t)clamped <= 0 ? 0 - clamped : clamped, (int32_t)length <= 0 ? 0 - length : length);
    if ((((int32_t)(uint32_t)(clamped)) > 0) != (((int32_t)(uint32_t)(length)) > 0))
        ratio = 0 - ratio;
    if (!surface)
        goto missing;
    result = sub_8003ED14(surface, interval, side_a, side_b, next_surface, lateral, ratio, output, reference);
    if (track_output)
    {
        if ((uint32_t)r_s16((record[1] + 2)) == 1)
        {
            track_output[0] = record[0];
            value = record[1];
            track_output[2] = sub;
            track_output[3] = piece;
            track_output[4] = lateral;
            track_output[5] = longitudinal;
            track_output[1] = value;
            track_output[7] = link;
            track_output[6] = record[6];
        }
        else
            track_output[3] = 0;
    }
    return result & 255;
missing:
    value = r_u32(output + 16);
    w_u32(output, 0);
    w_u32(output + 16, value & 0xfffffff0);
    if (track_output)
        track_output[3] = 0;
    return 1;
}

/* 8003EE68: junction-only contact selection; fourth argument is plane hint. */
uint32_t sub_8003EE68(uint32_t actor, uint32_t output, uint32_t track_output[8], uint32_t hint)
{
    FUNCTION_MARKER(0x8003EE68, "SLUS_010.53");
    uint32_t kind = (uint32_t)r_s16((r_u32(actor + 332) + 2)), root = r_u32(actor + 328);
    uint32_t header, result = 0, selected = 0, value;
    if (kind == 1)
    {
        header = sub_80039AFC(r_u32(root));
        if (header)
        {
            result = sub_8003E754(actor, header, output, track_output, hint);
            selected = r_u32(output);
        }
    }
    if (!selected)
    {
        value = r_u32(output + 16);
        w_u32(output, 0);
        w_u32(output + 16, value & 0xfffffff0);
        if (track_output)
            track_output[3] = 0;
    }
    return result & 255;
}

/* 8003DE28: update actor contact and retain the previous contact flag. */
uint32_t sub_8003DE28(uint32_t actor, uint32_t reset, uint32_t track_output[8], uint32_t hint)
{
    FUNCTION_MARKER(0x8003DE28, "SLUS_010.53");
    uint32_t previous = 0, interval, result;
    if (r_u32(actor + 372))
        previous = r_u32(actor + 388) & 1;
    if (reset == 1)
    {
        w_u32(actor + 372, 0);
        w_u32(actor + 376, 0);
        w_u32(actor + 380, 0);
        w_u32(actor + 384, 0);
    }
    if (r_u16(actor + 362) == 1)
    {
        interval = r_u32(actor + 376);
        if (interval && ((int32_t)(uint32_t)((uint32_t)r_s16((interval + 4)))) >= 0)
            w_u32(actor + 376, 0);
        result = sub_8003EE68(actor, actor + 372, track_output, hint);
    }
    else
    {
        interval = r_u32(actor + 376);
        if (interval && ((int32_t)(uint32_t)((uint32_t)r_s16((interval + 4)))) < 0)
            w_u32(actor + 376, 0);
        result = sub_8003EF34(actor + 328, actor + 360, actor + 372);
        if (track_output)
            track_output[3] = 0;
    }
    if (r_u32(actor + 372) && ((r_u32(actor + 388) & 1) || previous))
        result = 1;
    return result & 255;
}

/* 800952AC: actor state transition, effects-only return contract. */
void sub_800952AC(uint32_t actor, uint32_t requested)
{
    FUNCTION_MARKER(0x800952ACu, "RASHCDG.BIN");
    uint32_t active, linked, piece;
    uint8_t event[8] = {0};
    if (!actor)
        return;
    active = r_u16(actor + 320);
    if ((active & 1u) == (requested & 1u))
        return;
    if (active)
    {
        if (!sub_8003A468(actor + 360, actor + 328))
        {
            w_u16(actor + 320, 0);
            return;
        }
        w_u32(actor + 600, 0xffff0000u);
        (void)sub_8003DE28(actor, 1, 0, 0xffffffffu);
        sub_8003DF54(actor, 1, 0xffffffffu);
        piece = r_u32(actor + 340);
        (void)sub_8002EAD8(xport_guest_ptr(piece + 20, 12), xport_guest_ptr(piece + 14, 6), r_u32(actor + 348), xport_guest_ptr(actor + 184, 12));
        piece = r_u32(actor + 340);
        (void)sub_8002EAD8(xport_guest_ptr(actor + 184, 12), xport_guest_ptr(piece + 2, 6), r_u32(actor + 344), xport_guest_ptr(actor + 184, 12));
        w_u32(actor + 508, 0);
        w_u32(actor + 552, 0);
        w_u16(actor + 544, 224);
        w_u32(actor + 604, 0);
        (void)sub_800C4550(73, actor, 17);
    }
    else
    {
        linked = r_u32(actor + 596);
        if ((uint32_t)r_s16((linked + 320)))
        {
            (void)sub_800C3104(actor, 0);
            (void)sub_800BCD10(r_u32(actor + 596));
            linked = r_u32(actor + 596);
            xport_store_le16(event, 4);
            xport_store_le16(event + 2, r_u16(linked + 172));
            /* Mode zero overwrites both queued tail halves before their use. */
            (void)sub_800BCA68(event, 0, r_u32(actor + 596));
            xport_store_le16(event, 18);
            (void)sub_800BCA68(event, 0, r_u32(actor + 596));
        }
        else
            (void)sub_800903F4(linked, 1);
    }
}

/* 800951B8: choose actor activity then apply its transition. */
void sub_800951B8(uint32_t actor, uint32_t force_inactive)
{
    FUNCTION_MARKER(0x800951B8u, "RASHCDG.BIN");
    uint32_t previous = (uint32_t)r_s16((actor + 320)), tag, count, value, linked;
    if (!previous)
        (void)sub_80099D48(actor);
    else if (r_u32(actor + 552) & 0x04000000u)
    {
        tag = r_u16(actor + 172);
        count = r_u32(r_u32(0x8005B2F8) + 48);
        if (((tag >> 5) != 1 || ((int32_t)(uint32_t)(tag & 31u)) >= ((int32_t)(uint32_t)(count))) && !((uint32_t)r_u8((actor + 572)) & 32u))
        {
            force_inactive = 1;
            linked = r_u32(actor + 596);
            value = r_u32(actor + 552);
            linked = r_u32(linked + 360);
            w_u32(actor + 552, value & 0xfbffffffu);
            w_u32(actor + 360, linked + 1u);
        }
    }
    if (force_inactive)
        w_u16(actor + 320, 0);
    else
        w_u16(actor + 320, sub_80039F68(actor + 172));
    sub_800952AC(actor, previous);
}

/* 8003701C: follow actor position and update its road coordinates. */
uint32_t sub_8003701C(uint32_t actor)
{
    FUNCTION_MARKER(0x8003701Cu, "SLUS_010.53");
    uint32_t sub = r_u32(actor + 336), old_piece = r_u32(actor + 340), old_id = (uint32_t)r_s16((sub + 6));
    uint32_t piece = sub_80036B14(actor + 172, xport_guest_ptr(actor + 328, 32), actor + 184), prior, mask, value, flags;
    sub = r_u32(actor + 336);
    prior = r_u32(actor + 364);
    mask = (uint32_t)r_s16((sub + 6)) == old_id ? 0xffffffffu : 0;
    value = ((int32_t)(uint32_t)(prior)) > 0 ? 5u + (mask & (prior - 5u)) : 0xfffffffbu + (mask & (prior + 5u));
    w_u32(actor + 364, value);
    sub_80036800(actor + 184, piece, xport_guest_ptr(actor + 344, 4), xport_guest_ptr(actor + 348, 4));
    {
        const uint16_t route_direction[3] = {r_u16(actor + 450), r_u16(actor + 450 + 2), r_u16(actor + 450 + 4)};
        const uint32_t route_record[8] = {r_u32(actor + 328), r_u32(actor + 328 + 4), r_u32(actor + 328 + 8), r_u32(actor + 328 + 12), r_u32(actor + 328 + 16), r_u32(actor + 328 + 20), r_u32(actor + 328 + 24), r_u32(actor + 328 + 28)};
        uint32_t route_position[3] = {r_u32(actor + 360), r_u32(actor + 360 + 4), r_u32(actor + 360 + 8)};
        uint32_t route_result = sub_8003662C(route_direction, route_record, route_position);
        w_u32(actor + 360, route_position[0]);
        w_u32(actor + 360 + 4, route_position[1]);
        w_u32(actor + 360 + 8, route_position[2]);
        (void)route_result;
    }
    flags = r_u32(actor + 388);
    flags = piece == old_piece ? flags & 0xffffffbfu : flags | 64u;
    w_u32(actor + 388, flags);
    return flags;
}

/* 8003DDB0: optional junction-plane hint for the actor position. */
uint32_t sub_8003DDB0(uint32_t actor)
{
    FUNCTION_MARKER(0x8003DDB0u, "SLUS_010.53");
    uint32_t header;
    if (!actor || r_u16(actor + 362) != 1)
        return 0xffffffffu;
    header = sub_80039AFC(r_u32(r_u32(actor + 328)));
    if (!header)
        return 0xffffffffu;
    return sub_8003EB58(actor + 184, header, NULL, NULL, 0xffffffffu);
}

/* 8003DFF4: refresh contact, optionally adopt its junction track record. */
uint32_t sub_8003DFF4(uint32_t actor)
{
    FUNCTION_MARKER(0x8003DFF4u, "SLUS_010.53");
    uint32_t hint = sub_8003DDB0(actor), record[8] = {0}, result, flags, value, i;
    result = sub_8003DE28(actor, 0, record, hint) & 255u;
    if ((r_u16(actor + 172) >> 5) < 2)
    {
        flags = r_u32(actor + 388);
        if (((flags & 32u) || (r_u32(actor + 372) && (flags & 1u))) && r_u16(actor + 362) == 1 && (uint32_t)r_s16((r_u32(actor + 332) + 2)) == 1 && record[3] && r_u32(actor + 336) != record[2] && record[1] && (uint32_t)r_s16((record[1] + 2)) == 1)
        {
            for (i = 0; i < 8; ++i)
                w_u32(actor + 328 + 4 * i, record[i]);
            {
                const uint16_t route_direction[3] = {r_u16(actor + 450), r_u16(actor + 450 + 2), r_u16(actor + 450 + 4)};
                const uint32_t route_record[8] = {r_u32(actor + 328), r_u32(actor + 328 + 4), r_u32(actor + 328 + 8), r_u32(actor + 328 + 12), r_u32(actor + 328 + 16), r_u32(actor + 328 + 20), r_u32(actor + 328 + 24), r_u32(actor + 328 + 28)};
                uint32_t route_position[3] = {r_u32(actor + 360), r_u32(actor + 360 + 4), r_u32(actor + 360 + 8)};
                uint32_t route_result = sub_8003662C(route_direction, route_record, route_position);
                w_u32(actor + 360, route_position[0]);
                w_u32(actor + 360 + 4, route_position[1]);
                w_u32(actor + 360 + 8, route_position[2]);
                (void)route_result;
            }
        }
    }
    if (r_u32(actor + 388) & 65u)
        result = 1;
    sub_8003DF54(actor, 0, hint);
    value = (0u - result) & 64u;
    w_u32(actor + 388, r_u32(actor + 388) | value);
    return value;
}

/* 8003A5F4: choose an endpoint junction and return its road distance. */
uint32_t sub_8003A5F4(uint32_t position, uint32_t *output, uint32_t mode)
{
    uint32_t word, id = 0xFFFFFFFF, distance = 0, road, progress, junction;
    FUNCTION_MARKER(0x8003A5F4, "SLUS_010.53");
    word = r_u32(position);
    if ((word >> 16) == 1)
        id = word & 65535;
    else
    {
        road = sub_800245DC(word & 65535);
        if (road)
        {
            word = r_u32(road + 4);
            id = r_u32(road + 12);
            progress = r_u32(position + 8);
            distance = ((word >> 6) << 16) - progress;
            if ((!mode && (int32_t)progress < (int32_t)distance) || (r_s32(position + 4) < 0 && mode == 1))
            {
                distance = r_u32(position + 8);
                id = r_u32(road + 8);
            }
        }
    }
    if (id != 0xFFFFFFFF)
    {
        junction = sub_800245F4(id);
        if (junction)
        {
            id = 0xFFFFFFFF;
            if (r_u32(junction + 4) >= 3)
                id = r_u32(junction);
        }
    }
    *output = id;
    return distance;
}

/* 8003F4D8: first loaded route containing the requested member. */
uint32_t sub_8003F4D8(uint32_t id)
{
    FUNCTION_MARKER(0x8003F4D8u, "SLUS_010.53");
    uint32_t i = 0, record;
    if (((int32_t)(uint32_t)((uint32_t)r_s16(0x800D6182))) <= 0)
        return 0;
    do
    {
        record = r_u32(0x800D6194) + 120u * i;
        if (sub_8003B4B0(record, id))
            return record;
        ++i;
    } while (((int32_t)(uint32_t)(i)) < ((int32_t)(uint32_t)((uint32_t)r_s16(0x800D6182))));
    return 0;
}

/* 8003B024: assign a route and update the player's membership mask. */
void sub_8003B024(uint32_t input, uint32_t player, uint32_t kind, uint32_t other)
{
    FUNCTION_MARKER(0x8003B024u, "SLUS_010.53");
    uint32_t table, id, flags, found = 0, linked, count;
    if (!input)
        return;
    if (player)
    {
        count = r_u32(r_u32(0x8005B2F8) + 48);
        if (r_u16(player + 172) < count || ((uint32_t)r_u8((r_u32(player + 1084) + 1)) & 15u) != 2)
        {
            count = (uint32_t)r_s16(0x800D6182);
            table = r_u32(0x800D6194);
            w_u32(input + 256, table + 120u * count);
        }
        else if (other)
            w_u32(input + 256, r_u32(other + 428));
        else
        {
            found = sub_8003F4D8(r_u16(input + 188));
            if (found)
                w_u32(input + 256, found);
        }
        table = r_u32(player + 428);
        id = r_u16(player + 172);
        flags = r_u16(table + 118);
        w_u16(table + 118, flags | (1u << (id & 31u)));
        return;
    }
    if (kind == 1)
    {
        id = r_u16(input) & 31u;
        linked = r_u32(0x8005B3A4) + 628u * id;
        table = linked ? r_u32(r_u32(linked + 596) + 428) : 0;
        w_u32(input + 256, table);
        return;
    }
    if (other)
        found = sub_8003B4B0(r_u32(other + 428), r_u16(input + 188));
    if (!found)
    {
        found = sub_8003F4D8(r_u16(input + 188));
        if (found)
        {
            w_u32(input + 256, found);
            return;
        }
        if (!other)
            return;
    }
    w_u32(input + 256, r_u32(other + 428));
}

/* 8003B1C4: refresh the route after entering a road or junction.
 * Architectural v0 is unused by the sole wrapper and its callers. */
void sub_8003B1C4(uint32_t input, uint32_t kind, uint32_t player)
{
    FUNCTION_MARKER(0x8003B1C4u, "SLUS_010.53");
    uint32_t word = r_u32(input + 188), table, found, id, i, count, flags, mask;
    if ((word >> 16) == 1)
    {
        if (kind == 1 || (!kind && player))
        {
            table = r_u32(input + 256);
            if (!table || ((int32_t)(uint32_t)(r_u32(table + 16))) <= 0)
                return;
            id = word & 65535u;
            if (r_u32(table) == id)
            {
                if (player)
                {
                    mask = 1u << (r_u16(player + 172) & 31u);
                    flags = r_u16(table + 118);
                    w_u16(table + 118, flags | mask);
                }
                return;
            }
            i = 0;
            do
            {
                found = r_u32(table + 100 + 4u * i);
                if (found != 0xffffffffu && found == id)
                {
                    found = sub_8003F3B4(found);
                    if (found)
                        w_u32(input + 256, found);
                    if (player)
                    {
                        if (found)
                        {
                            table = r_u32(input + 256);
                            id = r_u16(player + 172);
                        }
                        else
                        {
                            id = r_u16(player + 172);
                            table = r_u32(input + 256);
                        }
                        mask = 1u << (id & 31u);
                        flags = r_u16(table + 118);
                        w_u16(table + 118, found ? flags | mask : flags & ~mask);
                    }
                    return;
                }
                count = r_u32(table + 16);
                ++i;
            } while (((int32_t)(uint32_t)(i)) < ((int32_t)(uint32_t)(count)));
            if (player)
            {
                id = r_u16(player + 172);
                table = r_u32(input + 256);
                flags = r_u16(table + 118);
                w_u16(table + 118, flags & ~(1u << (id & 31u)));
            }
            return;
        }
        found = sub_8003F3B4(r_u16(input + 188));
        if (found)
            w_u32(input + 256, found);
        return;
    }
    if (sub_8003B4B0(r_u32(input + 256), word & 65535u))
        return;
    if (kind >= 2 || (player && !kind && ((uint32_t)r_u8((r_u32(player + 1084) + 1)) & 15u) != 2 && r_u16(player + 172) >= r_u32(r_u32(0x8005B2F8) + 48)))
    {
        found = sub_8003F4D8(r_u16(input + 188));
        if (found)
            w_u32(input + 256, found);
        return;
    }
    table = r_u32(input + 256);
    if (!table || !player || ((int32_t)(uint32_t)(r_u32(table + 16))) <= 0)
        return;
    if (((int32_t)(uint32_t)(sub_8003A5F4(player + 360, &id, 0))) > 0x320000)
        return;
    if (((int32_t)(uint32_t)(r_u32(table + 16))) <= 0)
        return;
    i = 0;
    do
    {
        found = r_u32(table + 100 + 4u * i);
        if (found != 0xffffffffu && found == id)
        {
            found = sub_8003F3B4(found);
            if (found && sub_8003F580(found, r_u16(player + 360)))
                w_u32(input + 256, found);
            return;
        }
        count = r_u32(table + 16);
        ++i;
    } while (((int32_t)(uint32_t)(i)) < ((int32_t)(uint32_t)(count)));
}

/* 8003AF9C: select initial assignment versus route refresh. */
void sub_8003AF9C(uint32_t input, uint32_t refresh, uint32_t other)
{
    FUNCTION_MARKER(0x8003AF9Cu, "SLUS_010.53");
    uint32_t tag, kind, player = 0;
    if (!input)
        return;
    tag = r_u16(input);
    kind = tag >> 5;
    if (!kind)
        player = r_u32(0x8005B3A0) + 1096u * tag;
    if (r_u32(input + 256) && refresh)
        sub_8003B1C4(input, kind, player);
    else
        sub_8003B024(input, player, kind, other);
}

/* 8003B61C: distance along the assigned route, with junction projection. */
uint32_t sub_8003B61C(uint32_t input)
{
    uint32_t scalar_value_68;
    uint32_t scalar_value_71;
    uint32_t scalar_value_73;

    FUNCTION_MARKER(0x8003B61Cu, "SLUS_010.53");
    uint32_t count = (uint32_t)r_s16(0x800D6182), table = r_u32(input + 256);
    uint32_t word, record, link, distance, progress, direction = 0, marker, id, value, sign, piece, offset, local[8] = {0};
    if (count == 0xffffffffu)
        return 0;
    if (!table)
        return r_u32(input + 196);
    word = r_u32(input + 188);
    if ((word >> 16) == 1)
    {
        record = sub_8003F408(table, word & 65535u);
        if (!record)
            goto fallback;
        count = r_u32(record + 16);
        distance = r_u32(record + 4);
        if (((int32_t)(uint32_t)(count)) > 0)
        {
            const uint32_t route_geometry[8] = {r_u32(input + 156), r_u32(input + 156 + 4), r_u32(input + 156 + 8), r_u32(input + 156 + 12), r_u32(input + 156 + 16), r_u32(input + 156 + 20), r_u32(input + 156 + 24), r_u32(input + 156 + 28)};
            link = sub_8003BE1C(route_geometry, r_u32(record + 8), r_u32(record + 84), &direction);
            if (link)
            {
                if (link == r_u32(input + 164))
                    progress = r_u32(input + 196);
                else
                {
                    local[0] = r_u32(input + 156);
                    local[1] = r_u32(input + 160);
                    local[2] = link;
                    local[3] = r_u32(r_u32(input + 156) + 52) + 52u * (uint32_t)r_s16((link + 8));
                    piece = sub_80036B14(input, local, input + 12);
                    offset = sub_800B6AAC(xport_guest_ptr(input + 12, 12), xport_guest_ptr(piece + 14, 6), xport_guest_ptr(piece + 20, 12));
                    progress = r_u32(piece + 40) + offset;
                }
                if (((int32_t)(uint32_t)(direction)) > 0)
                    distance = distance - r_u32(record + 8) + ((scalar_value_68 = (r_u32(link + 12) - progress)), (((uint32_t)((!((uint32_t)(4))) ? (scalar_value_68) : ((scalar_value_68 >> ((uint32_t)(4))) | ((scalar_value_68 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(4)))) : 0))))));
                else
                    distance = distance - r_u32(record + 8) + ((uint32_t)((!((uint32_t)(4))) ? (((uint32_t)(progress))) : ((((uint32_t)(progress)) >> ((uint32_t)(4))) | ((((uint32_t)(progress)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(4)))) : 0))));
            }
        }
    }
    else
    {
        marker = r_u32(0x800D6188);
        id = r_u32(marker + 12);
        if ((id == r_u32(table) || id == 0xffffffffu) && !(word >> 16) && (word & 65535u) == r_u32(marker))
        {
            direction = r_u32(marker + 8);
            if ((((int32_t)(uint32_t)(direction)) > 0 && ((int32_t)(uint32_t)(r_u32(input + 196))) >= ((int32_t)(uint32_t)(r_u32(marker + 4)))) || (((int32_t)(uint32_t)(direction)) < 0 && ((int32_t)(uint32_t)(r_u32(marker + 4))) >= ((int32_t)(uint32_t)(r_u32(input + 196)))))
                return 0;
            marker = r_u32(0x800D6188);
            value = r_u32(marker + 4) - r_u32(input + 196);
            sign = ((uint32_t)((!((uint32_t)(31))) ? (((uint32_t)(value))) : ((((uint32_t)(value)) >> ((uint32_t)(31))) | ((((uint32_t)(value)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(31)))) : 0))));
            distance = ((scalar_value_71 = ((sign + value) ^ sign)), (((uint32_t)((!((uint32_t)(4))) ? (scalar_value_71) : ((scalar_value_71 >> ((uint32_t)(4))) | ((scalar_value_71 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(4)))) : 0))))));
        }
        else
        {
            record = sub_8003B4B0(table, r_u16(input + 188));
            if (!record)
                goto fallback;
            if (((int32_t)(uint32_t)(r_u32(record + 4))) > 0)
            {
                progress = r_u32(record + 8) - r_u32(input + 196);
                id = (uint32_t)r_s16((record + 14));
            }
            else
            {
                progress = r_u32(input + 196);
                id = (uint32_t)r_s16((record + 12));
            }
            distance = ((uint32_t)((!((uint32_t)(4))) ? (((uint32_t)(progress))) : ((((uint32_t)(progress)) >> ((uint32_t)(4))) | ((((uint32_t)(progress)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(4)))) : 0))));
            if (id != 0xffffffffu)
            {
                record = sub_8003F408(table, id);
                if (record)
                    distance += r_u32(record + 4);
            }
        }
    }
    return ((int32_t)(uint32_t)(distance)) < 0 ? 0 : distance;
fallback:
    distance = ((scalar_value_73 = (r_u32(input + 196))), (((uint32_t)((!((uint32_t)(4))) ? (scalar_value_73) : ((scalar_value_73 >> ((uint32_t)(4))) | ((scalar_value_73 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(4)))) : 0))))));
    return ((int32_t)(uint32_t)(distance)) < 0 ? 0 : distance;
}

/* 8003DCB8: opposite signs of projected zone boundaries. */
uint32_t sub_8003DCB8(uint32_t actor, uint32_t zone)
{
    FUNCTION_MARKER(0x8003DCB8u, "SLUS_010.53");
    uint32_t nx = (uint32_t)r_s16((r_u32(actor + 340) + 14)) << 4;
    uint32_t nz = (uint32_t)r_s16((r_u32(actor + 340) + 18)) << 4;
    uint32_t x = r_u32(actor + 184), z, ax, az, bx, bz;
    int64_t product = (int64_t)((int32_t)(uint32_t)(nx)) * ((int32_t)(uint32_t)(x - r_u32(zone + 20)));
    ax = (uint32_t)((uint64_t)product >> 16);
    z = r_u32(actor + 192);
    product = (int64_t)((int32_t)(uint32_t)(nz)) * ((int32_t)(uint32_t)(z - r_u32(zone + 28)));
    az = (uint32_t)((uint64_t)product >> 16);
    product = (int64_t)((int32_t)(uint32_t)(nx)) * ((int32_t)(uint32_t)(x - r_u32(zone + 8)));
    bx = (uint32_t)((uint64_t)product >> 16);
    product = (int64_t)((int32_t)(uint32_t)(nz)) * ((int32_t)(uint32_t)(z - r_u32(zone + 16)));
    bz = (uint32_t)((uint64_t)product >> 16);
    return ((ax + az) ^ (bx + bz)) >> 31;
}

/* 8003A9D8: road-zone flags, counter and propagation to linked actors. */
uint32_t sub_8003A9D8(uint32_t actor)
{
    FUNCTION_MARKER(0x8003A9D8u, "SLUS_010.53");
    uint32_t thresholds[6], i, j, flags, previous, descriptor, side = 0, count, zone, kind, tag, speed, index, limit, node;
    for (i = 0; i < 6; ++i)
        thresholds[i] = r_u32(0x80010DFCu + 4 * i);
    flags = r_u32(actor + 36);
    descriptor = r_u32(actor + 496);
    previous = (flags >> 5) & 3u;
    flags &= 0xfffffc9fu;
    w_u32(actor + 36, flags);
    if (descriptor)
        count = 1;
    else
    {
        side = r_u32(actor + 492);
        if (!side)
            return flags;
        if (((int32_t)(uint32_t)(r_u32(actor + 344))) >= 0)
            side += 2;
        count = (uint32_t)r_u8((side + 1));
    }
    if (count)
    {
        if (!descriptor && side)
            descriptor = r_u32(r_u32(actor + 328) + 100) + 4u * (uint32_t)r_u8(side);
        for (i = 0; i < count; ++i, descriptor += 4)
        {
            zone = r_u32(r_u32(actor + 328) + 96) + 40u * (uint32_t)r_s16(descriptor);
            for (j = 0; j < (uint32_t)r_u8((descriptor + 3)); ++j, zone += 40)
            {
                if (sub_8003DCB8(actor, zone))
                {
                    kind = r_u16(zone + 2) & 15u;
                    switch (kind)
                    {
                        case 1:
                        case 2:
                            flags = r_u32(actor + 36);
                            flags = (flags & 0xfffffdffu) | (r_u16(zone + 2) & 512u);
                            w_u32(actor + 36, flags);
                            break;
                        case 3:
                            w_u32(actor + 36, r_u32(actor + 36) | 768u);
                            break;
                        case 5:
                            w_u32(actor + 36, (r_u32(actor + 36) & 0xffffff9fu) | 800u);
                            break;
                        case 4:
                        case 6:
                        case 7:
                        case 8:
                            tag = r_u16(actor + 172);
                            if ((tag >> 5) != 4 || (tag & 31u) < 30)
                            {
                                speed = r_u32(actor + 480);
                                if (((uint32_t)((!((uint32_t)(16))) ? (((uint32_t)(speed))) : ((((uint32_t)(speed)) >> ((uint32_t)(16))) | ((((uint32_t)(speed)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(16)))) : 0)))))
                                {
                                    index = ((uint32_t)((!((uint32_t)(20))) ? (((uint32_t)(speed))) : ((((uint32_t)(speed)) >> ((uint32_t)(20))) | ((((uint32_t)(speed)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(20)))) : 0))));
                                    index = ((int32_t)(uint32_t)(index)) < 0 ? 0 : (index > 5 ? 5 : index);
                                    limit = thresholds[index];
                                    flags = r_u32(actor + 36);
                                    if (((flags >> 1) & 15u) < limit)
                                        w_u32(actor + 36, (flags & 0xffffff9fu) | (previous ? 64u : 0));
                                    else if (previous)
                                    {
                                        w_u32(actor + 36, flags & 0xffffffe1u);
                                        if (sub_8001FC58() & limit)
                                        {
                                            w_u32(actor + 36, r_u32(actor + 36) & 0xffffff9fu);
                                            index = sub_8001FC58() % limit;
                                            w_u32(actor + 36, (r_u32(actor + 36) & 0xffffffe1u) | ((index & 15u) << 1));
                                        }
                                    }
                                    else
                                        w_u32(actor + 36, (flags & 0xffffffe1u & 0xffffff9fu) | 64u);
                                    flags = r_u32(actor + 36);
                                    w_u32(actor + 36, (flags & 0xffffffe1u) | (((((flags >> 1) & 15u) + 1u) & 15u) << 1));
                                }
                            }
                            flags = r_u32(actor + 36) | 512u;
                            w_u32(actor + 36, flags);
                            kind = r_u16(zone + 2) & 15u;
                            w_u32(actor + 36, (flags & 0xfffffeffu) | ((kind != 6 && kind != 8) ? 256u : 0));
                            break;
                        default:
                            break;
                    }
                }
                if (r_u32(actor + 36) & 96u)
                    break;
            }
            if (r_u32(actor + 36) & 96u)
                break;
        }
    }
    for (i = 0; i < 2; ++i)
    {
        node = r_u32(actor + 56 + 8 * i);
        while (node)
        {
            flags = r_u32(node + 36);
            flags = (flags & 0xffffff9fu) | (r_u32(actor + 36) & 96u);
            w_u32(node + 36, flags);
            node = r_u32(node + 56 + 8 * i);
        }
    }
    return 0;
}

/* 8001FCB0: signed approximate vector length, including wrapped abs edges. */
uint32_t sub_8001FCB0(uint32_t x, uint32_t y, uint32_t z)
{
    FUNCTION_MARKER(0x8001FCB0u, "SLUS_010.53");
    uint32_t sign, temp, sum;
    sign = ((uint32_t)((!((uint32_t)(31))) ? (((uint32_t)(x))) : ((((uint32_t)(x)) >> ((uint32_t)(31))) | ((((uint32_t)(x)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(31)))) : 0))));
    x = (sign + x) ^ sign;
    sign = ((uint32_t)((!((uint32_t)(31))) ? (((uint32_t)(y))) : ((((uint32_t)(y)) >> ((uint32_t)(31))) | ((((uint32_t)(y)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(31)))) : 0))));
    y = (sign + y) ^ sign;
    sign = ((uint32_t)((!((uint32_t)(31))) ? (((uint32_t)(z))) : ((((uint32_t)(z)) >> ((uint32_t)(31))) | ((((uint32_t)(z)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(31)))) : 0))));
    z = (sign + z) ^ sign;
    if (((int32_t)(uint32_t)(x)) < ((int32_t)(uint32_t)(y)))
    {
        temp = x;
        x = y;
        y = temp;
    }
    if (((int32_t)(uint32_t)(x)) < ((int32_t)(uint32_t)(z)))
    {
        temp = x;
        x = z;
        z = temp;
    }
    sum = y + z;
    return x - ((uint32_t)((!((uint32_t)(4))) ? (((uint32_t)(x))) : ((((uint32_t)(x)) >> ((uint32_t)(4))) | ((((uint32_t)(x)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(4)))) : 0)))) + ((uint32_t)((!((uint32_t)(2))) ? (((uint32_t)(sum))) : ((((uint32_t)(sum)) >> ((uint32_t)(2))) | ((((uint32_t)(sum)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(2)))) : 0)))) + ((uint32_t)((!((uint32_t)(3))) ? (((uint32_t)(sum))) : ((((uint32_t)(sum)) >> ((uint32_t)(3))) | ((((uint32_t)(sum)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(3)))) : 0))));
}

uint32_t sub_800396A8(uint32_t actor)
{
    uint16_t old_id;
    uint32_t route, root, header, result, entry, selected = 0, index, point;
    uint32_t best = 0xFFFF0000, delta[3], saved[3], x, z, sign, minimum, score;
    uint32_t dot, segment, piece, value, mask;
    FUNCTION_MARKER(0x800396A8, "SLUS_010.53");
    old_id = r_u16(r_u32(actor + 336) + 6);
    if (!r_u16(actor + 362))
    {
        (void)sub_8003BFE8(actor);
        goto update_direction;
    }
    route = r_u32(actor + 332);
    root = r_u32(actor + 328);
    if (r_s16(route + 2) != 1 || r_s16(route))
        goto update_direction;
    header = sub_80039AFC(r_u32(root));
    if (!header)
        return 0;
    result = (uint32_t)r_s16(header + 2);
    for (index = 0; (int32_t)index < r_s16(header + 2); ++index)
    {
        entry = header + 8 + 24 * index;
        delta[0] = r_u32(actor + 184) - r_u32(entry + 12);
        delta[1] = r_u32(actor + 188) - r_u32(entry + 16);
        delta[2] = r_u32(actor + 192) - r_u32(entry + 20);
        sign = (uint32_t)((int32_t)delta[0] >> 31);
        x = (delta[0] + sign) ^ sign;
        sign = (uint32_t)((int32_t)delta[2] >> 31);
        z = (delta[2] + sign) ^ sign;
        if ((int32_t)x > 0x5A8000 || (int32_t)z > 0x5A8000)
            score = 0x7FFF0000;
        else
        {
            x = r_u32(actor + 184) - r_u32(entry + 12);
            z = r_u32(actor + 192) - r_u32(entry + 20);
            sign = (uint32_t)((int32_t)x >> 31);
            x = (x + sign) ^ sign;
            sign = (uint32_t)((int32_t)z >> 31);
            z = (z + sign) ^ sign;
            minimum = (int32_t)x < (int32_t)z ? x : z;
            score = x + z - (uint32_t)((int32_t)(minimum + (minimum >> 31)) >> 1);
        }
        if (best == 0xFFFF0000 || (int32_t)score < (int32_t)best)
        {
            selected = entry;
            best = score;
            saved[0] = delta[0];
            saved[1] = delta[1];
            saved[2] = delta[2];
        }
        result = (int32_t)(index + 1) < r_s16(header + 2);
    }
    if (!selected)
        return result;
    dot = 0;
    for (point = 0; point < 3; ++point)
    {
        int64_t product = (int64_t)(int32_t)saved[point] * ((int32_t)r_s16(selected + 6 + 2 * point) * 16);
        dot += (uint32_t)((uint64_t)product >> 16);
    }
    if ((int32_t)dot < -131)
    {
        route = sub_80039C90(root, (uint32_t)r_s16(selected + 4));
        if (!route)
            return 0;
        segment = r_u32(root + 48) + 28 * (uint32_t)r_s16(route + 20);
        index = (uint32_t)r_s16(segment + 8);
        if (r_s16(selected + 2) <= 0)
            index += (uint32_t)r_s16(segment + 10) - 1;
        piece = r_u32(root + 52) + 52 * index;
        sub_80036800(actor + 184, piece, xport_guest_ptr(actor + 344, 4), xport_guest_ptr(actor + 348, 4));
        w_u32(actor + 332, route);
        w_u32(actor + 336, segment);
        w_u32(actor + 340, piece);
        w_u32(actor + 352, 0);
        w_u32(actor + 356, 0);
    }
update_direction:
    mask = r_s16(r_u32(actor + 336) + 6) == (int16_t)old_id ? UINT32_MAX : 0;
    value = r_u32(actor + 364);
    result = (int32_t)value > 0 ? 5 + (mask & (value - 5)) : 0xFFFFFFFB + (mask & (value + 5));
    w_u32(actor + 364, result);
    return result;
}

/* 80037104: projected track record and attached actor placement. */
uint32_t sub_80037104(uint32_t actor)
{
    FUNCTION_MARKER(0x80037104u, "SLUS_010.53");
    uint32_t record[8] = {0}, i, flag, scale, origin, linked, result, flags;
    for (i = 0; i < 4; ++i)
        record[i] = r_u32(actor + 328 + 4 * i);
    flag = (r_u32(actor + 568) & 0x600u) != 0;
    scale = r_u32(actor + 308);
    origin = actor + (flag ? 184u : 504u);
    if (!flag)
        scale <<= 1;
    (void)sub_8002EAD8(xport_guest_ptr(origin, 12), xport_guest_ptr(actor + 528, 6), scale, xport_guest_ptr(actor + 244, 12));
    (void)sub_80036B14(actor + 172, record, actor + 244);
    result = record[3];
    linked = r_u32(actor + 856);
    w_u32(actor + 256, result);
    if (!linked)
        return result;
    result = r_u32(actor + 1088);
    if (!result)
        return 0;
    if (flag)
        origin = actor + 184;
    else
    {
        (void)sub_8002EAD8(xport_guest_ptr(actor + 504, 12), xport_guest_ptr(actor + 528, 6), r_u32(actor + 308), xport_guest_ptr(linked + 504, 12));
        origin = linked + 504;
    }
    scale = r_u32(actor + 304) + r_u32(linked + 304);
    (void)sub_8002EAD8(xport_guest_ptr(origin, 12), xport_guest_ptr(actor + 516, 6), scale, xport_guest_ptr(linked + 184, 12));
    if (r_u32(actor + 560) & 0x08000000u)
        flags = r_u32(linked + 388) & ~32u;
    else
    {
        sub_800396A8(linked);
        flags = r_u32(linked + 388) | 32u;
    }
    w_u32(linked + 388, flags);
    return sub_8003701C(linked);
}

/* 80037450: ordered actor track/contact/route/surface update. */
uint32_t sub_80037450(uint32_t actor)
{
    FUNCTION_MARKER(0x80037450u, "SLUS_010.53");
    uint32_t distance;
    (void)sub_8003701C(actor);
    if (!(r_u16(actor + 172) >> 5))
        (void)sub_80037104(actor);
    (void)sub_8003DFF4(actor);
    sub_8003AF9C(actor + 172, 1, 0);
    distance = sub_8003B61C(actor + 172);
    w_u32(actor + 324, distance);
    (void)sub_8003A9D8(actor);
    return (r_u32(actor + 388) >> 6) & 1u;
}

/* 800374D4: optional correction, then the complete actor track update. */
uint32_t sub_800374D4(uint32_t actor, uint32_t correct)
{
    FUNCTION_MARKER(0x800374D4u, "SLUS_010.53");
    uint32_t flags;
    if (correct)
    {
        sub_800396A8(actor);
        flags = r_u32(actor + 388) | 32u;
    }
    else
        flags = r_u32(actor + 388) & ~32u;
    w_u32(actor + 388, flags);
    return sub_80037450(actor);
}

/* 80028034: attach the table-selected effect set using actual slot/list code. */
uint32_t sub_80028034(uint32_t actor)
{
    uint32_t scalar_value_95;

    FUNCTION_MARKER(0x80028034u, "SLUS_010.53");
    uint32_t variant = 0, count, table, value, delta, id, slot, flags, stamp, i, kind, byte_value;
    kind = (r_u16(r_u32(actor) + 14) & 120u) >> 3;
    if (kind == 2)
    {
        value = r_u32(actor + 180);
        if (value < 18)
            return 1;
        delta = 20u - value;
        variant = value + (((scalar_value_95 = (value - 18u)), (((uint32_t)((!((uint32_t)(31))) ? (scalar_value_95) : ((scalar_value_95 >> ((uint32_t)(31))) | ((scalar_value_95 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(31)))) : 0)))))) & (18u - value)) + (((uint32_t)((!((uint32_t)(31))) ? (((uint32_t)(delta))) : ((((uint32_t)(delta)) >> ((uint32_t)(31))) | ((((uint32_t)(delta)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(31)))) : 0)))) & delta) - 18u;
        count = 6;
        table = 0x800536dc;
    }
    else
    {
        count = 4;
        table = 0x800536e8;
    }
    if ((r_u32(actor + 36) >> 27) & 1u)
        return 1;
    for (i = 0; i < count; ++i, table += 2)
    {
        id = sub_80027178();
        if (id == 0xffffffffu)
            continue;
        slot = 0x800d39b0 + 112u * id;
        flags = (r_u32(slot) & 0xfffffc3fu) | 0x140u;
        w_u32(slot, flags);
        flags = r_u32(slot);
        stamp = r_u32(r_u32(0x8005b2f8) + 16);
        w_u32(slot + 36, 0);
        w_u32(slot + 44, 0);
        w_u32(slot + 40, 0);
        w_u8(slot + 61, 0);
        w_u32(slot + 48, stamp);
        byte_value = (uint32_t)r_u8(table);
        w_u8(slot + 96, 5);
        flags = (flags & 0xffc03fffu) | (((byte_value + count * variant) & 255u) << 14);
        w_u32(slot, flags);
        byte_value = (uint32_t)r_u8((table + 1));
        flags = (flags & 0xffffc3ffu) | 0x800u;
        w_u32(slot, flags);
        w_u8(slot + 97, (uint8_t)byte_value);
        (void)sub_800271CC(actor, id);
    }
    flags = r_u32(actor + 36) | 0x38000000u;
    w_u32(actor + 36, flags);
    return flags;
}

/* 8008BA18: eight world-space actor corners with class-specific offsets. */
uint32_t sub_8008BA18(uint32_t actor)
{
    uint32_t scalar_value_97;

    FUNCTION_MARKER(0x8008BA18u, "RASHCDG.BIN");
    uint32_t tag = r_u16(actor + 172), height = r_u32(actor + 312), width = r_u32(actor + 304);
    uint32_t depth, product, a[3], b[3], c[3], points[8][3], i, k, shift_all = 0, shift_top = 0;
    if (!(tag >> 5) && r_u32(actor + 180) < 18u && r_u32(r_u32(actor + 852) + 604) >= 2u)
    {
        product = 5u * height;
        height = ((scalar_value_97 = (product + (((int32_t)(uint32_t)(product)) < 0 ? 7u : 0u))), (((uint32_t)((!((uint32_t)(3))) ? (scalar_value_97) : ((scalar_value_97 >> ((uint32_t)(3))) | ((scalar_value_97 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(3)))) : 0))))));
    }
    depth = r_u32(actor + 308);
    (void)sub_8002EE50(width, xport_guest_ptr(actor + 432, 6), a);
    (void)sub_8002EE50(height, xport_guest_ptr(actor + 438, 6), b);
    (void)sub_8002EE50(depth, xport_guest_ptr(actor + 444, 6), c);
    for (k = 0; k < 3; ++k)
    {
        points[0][k] = 0u - a[k] - c[k];
        points[1][k] = a[k] - c[k];
        points[2][k] = a[k] + c[k];
        points[3][k] = c[k] - a[k];
        points[4][k] = 0u - a[k] - b[k] - c[k];
        points[5][k] = a[k] - b[k] - c[k];
        points[6][k] = a[k] - b[k] + c[k];
        points[7][k] = 0u - a[k] - b[k] + c[k];
    }
    tag = r_u16(actor + 172);
    if ((tag >> 5) == 3)
    {
        shift_top = 1;
        (void)sub_8002EE50(0x2000, xport_guest_ptr(actor + 438, 6), b);
    }
    else if ((tag >> 5) == 4 && (tag & 31u) >= 30u)
    {
        shift_all = 1;
        (void)sub_8002E810(0x8000, b, b);
    }
    for (i = 0; i < 8; ++i)
    {
        if (shift_all || (shift_top && i < 4))
            for (k = 0; k < 3; ++k)
                points[i][k] += b[k];
        for (k = 0; k < 3; ++k)
            w_u32(actor + 196 + 12 * i + 4 * k, points[i][k] + r_u32(actor + 184 + 4 * k));
    }
    return 0;
}

uint16_t *sub_8003FA40(const uint16_t left[9], const uint16_t right[9], uint16_t output[9])
{
    uint32_t first, second, last, column, x, y, z;
    FUNCTION_MARKER(0x8003FA40, "SLUS_010.53");
    first = (uint32_t)left[0] | ((uint32_t)left[1] << 16);
    second = (uint32_t)left[2] | ((uint32_t)left[3] << 16);
    xport_gte_write_control(0, first);
    xport_gte_write_control(1, second);
    first = (uint32_t)left[4] | ((uint32_t)left[5] << 16);
    second = (uint32_t)left[6] | ((uint32_t)left[7] << 16);
    /* Control4 consumes only the signed final halfword */
    last = (uint32_t)(int32_t)(int16_t)left[8];
    xport_gte_write_control(2, first);
    xport_gte_write_control(3, second);
    xport_gte_write_control(4, last);
    for (column = 0; column < 3; ++column)
    {
        x = right[column];
        y = right[column + 3];
        z = right[column + 6];
        xport_gte_write_data(9, x);
        xport_gte_write_data(10, y);
        xport_gte_write_data(11, z);
        xport_gte_execute(0x49E012);
        x = xport_gte_read_data(9);
        y = xport_gte_read_data(10);
        z = xport_gte_read_data(11);
        output[column] = (uint16_t)x;
        output[column + 3] = (uint16_t)y;
        output[column + 6] = (uint16_t)z;
    }
    return output + 1;
}

uint32_t sub_8003FB34(const uint16_t axis[3], uint32_t angle, uint16_t output[9])
{
    FUNCTION_MARKER(0x8003FB34, "SLUS_010.53");
    uint32_t vector[3], square[3], sine[3], cross[3], diagonal[3], values[9];
    uint32_t cosine, sin_value, complement, product, i, j, k;
    cosine = (uint32_t)r_s16(0x8005624C + 4 * (angle & 4095) + 2) << 4;
    sin_value = (uint32_t)r_s16(0x8005624C + 4 * (angle & 4095)) << 4;
    complement = 65536 - cosine;
    for (i = 0; i < 3; ++i)
        vector[i] = (uint32_t)(int32_t)(int16_t)axis[i];
    for (i = 0; i < 3; ++i)
    {
        square[i] = (uint32_t)sub_8001FC90((int32_t)(vector[i] << 4), (int32_t)(vector[i] << 4));
        sine[i] = (uint32_t)sub_8001FC90((int32_t)(vector[i] << 4), (int32_t)sin_value);
        j = (i + 1) % 3;
        k = (i + 2) % 3;
        product = vector[j] * vector[k];
        cross[i] = (uint32_t)sub_8001FC90((int32_t)((uint32_t)((int32_t)(product << 4) >> 16) << 4), (int32_t)complement);
        diagonal[i] = square[i] + (uint32_t)sub_8001FC90((int32_t)cosine, (int32_t)(65536 - square[i]));
    }
    values[0] = diagonal[0];
    values[1] = cross[2] + sine[2];
    values[2] = cross[1] - sine[1];
    values[3] = cross[2] - sine[2];
    values[4] = diagonal[1];
    values[5] = cross[0] + sine[0];
    values[6] = cross[1] + sine[1];
    values[7] = cross[0] - sine[0];
    values[8] = diagonal[2];
    for (i = 0; i < 9; ++i)
        output[i] = (uint16_t)((int32_t)values[i] >> 4);
    return (uint32_t)((int32_t)values[8] >> 4);
}

/* 8009432C: complete placement/activation flow; actual callees throughout. */
uint32_t sub_8009432C(uint32_t actor)
{
    uint32_t scalar_shift_98;
    uint32_t scalar_value_106;

    FUNCTION_MARKER(0x8009432Cu, "RASHCDG.BIN");
    uint32_t angle = (uint32_t)sub_8001FC90(0x3c0000, 11), flags = 0, lateral = 0, player = 0;
    uint32_t global, descriptor, secondary, tag, count, rank, value, other, side, piece, link, type, i, k, x, y, z, t, u, speed;
    uint32_t distances[2];
    uint16_t matrix[9], axes[9];
    uint8_t event[8] = {0};
    if (!actor)
        return angle;
    global = r_u32(0x8005b2f8);
    if ((uint32_t)r_u8((global + 4)) == 44 && !(uint32_t)r_u8((global + 57)))
        w_u16(actor + 320, 0);
    if (!(uint32_t)r_s16((actor + 320)))
        return 0x80060000u;
    global = r_u32(0x8005b2f8);
    tag = r_u16(actor + 172);
    if (((uint32_t)r_u8((global + 4)) & 1u) && tag >= r_u32(global + 48) && (uint32_t)r_u8((r_u32(actor + 1084) + 39)) == 254)
    {
        flags = 61;
        angle = (uint32_t)sub_8001FC90(0xa0000, 11);
        w_u32(actor + 560, r_u32(actor + 560) | 0x20000000u);
    }
    else
    {
        global = r_u32(0x8005b2f8);
        value = (uint32_t)r_u8((global + 57));
        if (value == 1)
        {
            flags = r_u32(r_u32(r_u32(0x8005b38c) + 1084) + 40) ? 67u : 123u;
            lateral = r_u32(actor + 344);
            angle = r_u32(actor + 292);
            w_u32(actor + 924, 0);
        }
        else if (value >= 3 && r_u16(actor + 172) >= r_u32(global + 48) && (((uint32_t)r_u8((r_u32(actor + 1084) + 1)) & 15u) == ((uint32_t)r_u8((r_u32(r_u32(0x8005b38c) + 1084) + 1)) & 15u)))
        {
            flags = 85;
            descriptor = r_u32(actor + 1084);
            w_u8(descriptor, (uint8_t)((uint32_t)r_u8(descriptor) | 16u));
            angle = (uint32_t)sub_8001FC90(0x9b0000, 11);
        }
        else
        {
            secondary = r_u32(actor + 852);
            flags = 1;
            if (r_u32(secondary + 604) >= 3 && (uint32_t)r_s16((secondary + 320)))
                flags = 161;
            count = r_u32(r_u32(0x8005b2f8) + 48);
            if (r_u16(actor + 172) >= count)
            {
                if (count == 2)
                {
                    x = r_u32(0x8005b268);
                    y = r_u32(0x8005b26c);
                    player = r_u32(0x8005b268 + 4u * (((int32_t)(uint32_t)(r_u32(y + 324))) < ((int32_t)(uint32_t)(r_u32(x + 324)))));
                }
                else
                    player = r_u32(0x8005b38c);
                if (((int32_t)(uint32_t)(r_u32(0x8005b1f8))) >= (int32_t)(uint32_t)r_u8((r_u32(player + 1084) + 39)))
                {
                    descriptor = r_u32(actor + 1084);
                    if (r_u32(descriptor + 40))
                    {
                        count = r_u32(r_u32(0x8005b2f8) + 48);
                        rank = (uint32_t)r_u8((descriptor + 39));
                        if (((int32_t)(uint32_t)(((scalar_shift_98 = ((count - 1u) & 31u)), (((uint32_t)((!scalar_shift_98) ? (((uint32_t)(8))) : ((((uint32_t)(8)) >> scalar_shift_98) | ((((uint32_t)(8)) & 0x80000000) ? (0xFFFFFFFFu << (32 - scalar_shift_98)) : 0)))))))) < (int32_t)rank)
                            flags = 0;
                        else
                        {
                            value = r_u32(r_u32(0x800d6188) + 4);
                            descriptor = r_u32(actor + 1084);
                            w_u32(actor + 368, value);
                            rank = (uint32_t)r_u8((descriptor + 39));
                            flags |= 60u;
                            w_u32(actor + 560, r_u32(actor + 560) | 0x20000000u);
                            w_u32(actor + 368, value + 196608u * (rank >> 1) * (1u - 2u * (rank & 1u)));
                        }
                    }
                }
            }
        }
    }
    if (!flags || !sub_8003A468(actor + 360, actor + 328))
    {
        global = r_u32(0x8005b2f8);
        tag = r_u16(actor + 172);
        w_u16(actor + 320, 0);
        if (tag < r_u32(global + 48))
            return 1;
        goto ai_tail;
    }
    link = sub_8003B4B0(r_u32(actor + 428), r_u32(r_u32(actor + 332) + 12));
    (void)sub_8003DE28(actor, 1, 0, 0xffffffffu);
    if (flags & 4u)
    {
        global = r_u32(0x8005b2f8);
        if ((uint32_t)r_u8((global + 57)) >= 3)
        {
            side = ((r_u16(actor + 172) >> 1) - 1u) & 1u;
            if (!side)
                angle = 0u - angle;
        }
        else
            side = r_u32(global + 48) == 2 && r_u32(r_u32(0x800d6188)) == 32;
        value = 2u * r_u32(actor + 308);
        value = side ? r_u32(actor + 412) - value : r_u32(actor + 400) + value;
        w_u32(actor + 344, value);
        w_u32(actor + 924, 0);
    }
    value = ((uint32_t)r_u8((actor + 946)) ^ 128u) - 128u;
    type = r_u16(actor + 956 + 8u * (value - 1u));
    if (flags & 2u)
        w_u32(actor + 344, lateral);
    else
    {
        global = r_u32(0x8005b2f8);
        tag = r_u16(actor + 172);
        if (!(flags & 4u) && !(tag >= r_u32(global + 48) && ((uint32_t)r_u8((r_u32(actor + 1084) + 1)) & 15u) == 2) && type != 1)
            w_u32(actor + 344, 0);
        else if (r_u16(actor + 172) >= r_u32(r_u32(0x8005b2f8) + 48) && ((uint32_t)r_u8((r_u32(actor + 1084) + 1)) & 15u) == 2 && type == 1)
            w_u32(actor + 344, r_u32(actor + (((int32_t)(uint32_t)(r_u32(actor + 364))) > 0 ? 416u : 404u)));
    }
    piece = r_u32(actor + 340);
    (void)sub_8002EAD8(xport_guest_ptr(piece + 20, 12), xport_guest_ptr(piece + 14, 6), r_u32(actor + 348), xport_guest_ptr(actor + 184, 12));
    (void)sub_8002EAD8(xport_guest_ptr(actor + 184, 12), xport_guest_ptr(piece + 2, 6), r_u32(actor + 344), xport_guest_ptr(actor + 184, 12));
    x = r_u32(actor + 184);
    y = r_u32(actor + 188);
    z = r_u32(actor + 192);
    w_u32(actor + 504, x);
    w_u32(actor + 508, y);
    w_u32(actor + 512, z);
    w_u32(actor + 468, x);
    w_u32(actor + 472, y);
    w_u32(actor + 476, z);
    (void)sub_80037450(actor);
    sub_8003DF54(actor, 1, 0xffffffffu);
    for (i = 0; i < 5; ++i)
        w_u16(actor + 432 + 2 * i, r_u16(piece + 2 + 2 * i));
    x = r_u16(actor + 438);
    y = r_u16(piece + 12);
    w_u16(actor + 438, 0u - x);
    x = r_u16(actor + 440);
    w_u16(actor + 442, y);
    w_u16(actor + 442, 0u - y);
    w_u16(actor + 440, 0u - x);
    for (i = 0; i < 3; ++i)
        w_u16(actor + 444 + 2 * i, r_u16(piece + 14 + 2 * i));
    if (link)
    {
        w_u32(actor + 364, r_u32(link + 4));
        if (((int32_t)(uint32_t)(r_u32(link + 4))) < 0)
        {
            for (i = 0; i < 3; ++i)
                w_u16(actor + 444 + 2 * i, 0u - r_u16(actor + 444 + 2 * i));
            for (i = 0; i < 3; ++i)
                w_u16(actor + 432 + 2 * i, 0u - r_u16(actor + 432 + 2 * i));
        }
    }
    else
        w_u32(actor + 364, 1);
    global = r_u32(0x8005b2f8);
    count = r_u32(global + 48);
    if (r_u16(actor + 172) >= count && ((uint32_t)r_u8((r_u32(actor + 1084) + 1)) & 15u) == 2)
    {
        if (count == 2)
        {
            for (i = 0; i < 2; ++i)
            {
                other = r_u32(0x8005b268 + 4 * i);
                x = (uint32_t)r_s16((actor + 186)) - (uint32_t)r_s16((other + 186));
                y = (uint32_t)r_s16((actor + 194)) - (uint32_t)r_s16((other + 194));
                t = ((uint32_t)((!((uint32_t)(31))) ? (((uint32_t)(x))) : ((((uint32_t)(x)) >> ((uint32_t)(31))) | ((((uint32_t)(x)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(31)))) : 0))));
                x = (x + t) ^ t;
                t = ((uint32_t)((!((uint32_t)(31))) ? (((uint32_t)(y))) : ((((uint32_t)(y)) >> ((uint32_t)(31))) | ((((uint32_t)(y)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(31)))) : 0))));
                y = (y + t) ^ t;
                if (((int32_t)(uint32_t)(x)) < ((int32_t)(uint32_t)(y)))
                {
                    t = x;
                    x = y;
                    y = t;
                }
                t = y + ((uint32_t)((!((uint32_t)(1))) ? (((uint32_t)(y))) : ((((uint32_t)(y)) >> ((uint32_t)(1))) | ((((uint32_t)(y)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(1)))) : 0))));
                distances[i] = x - ((uint32_t)((!((uint32_t)(5))) ? (((uint32_t)(x))) : ((((uint32_t)(x)) >> ((uint32_t)(5))) | ((((uint32_t)(x)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(5)))) : 0)))) - ((uint32_t)((!((uint32_t)(7))) ? (((uint32_t)(x))) : ((((uint32_t)(x)) >> ((uint32_t)(7))) | ((((uint32_t)(x)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(7)))) : 0)))) + ((uint32_t)((!((uint32_t)(2))) ? (((uint32_t)(t))) : ((((uint32_t)(t)) >> ((uint32_t)(2))) | ((((uint32_t)(t)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(2)))) : 0)))) + ((uint32_t)((!((uint32_t)(6))) ? (((uint32_t)(t))) : ((((uint32_t)(t)) >> ((uint32_t)(6))) | ((((uint32_t)(t)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(6)))) : 0))));
            }
            player = r_u32(0x8005b268 + 4u * (((int32_t)(uint32_t)(distances[1])) < ((int32_t)(uint32_t)(distances[0]))));
        }
        else
            player = r_u32(0x8005b38c);
        if (r_u32(player + 360) == r_u32(actor + 360) && ((int32_t)(uint32_t)(r_u32(actor + 364) ^ r_u32(player + 364))) < 0)
        {
            for (i = 0; i < 3; ++i)
                w_u16(actor + 444 + 2 * i, 0u - r_u16(actor + 444 + 2 * i));
            for (i = 0; i < 3; ++i)
                w_u16(actor + 432 + 2 * i, 0u - r_u16(actor + 432 + 2 * i));
            w_u32(actor + 364, 0u - r_u32(actor + 364));
        }
    }
    if (flags & 16u)
    {
        (void)sub_8003FB34((const uint16_t[3]){r_u16(piece + 8), r_u16(piece + 10), r_u16(piece + 12)}, angle, matrix);
        (void)sub_8003FA40(xport_guest_cptr(actor + 432, 18), matrix, xport_guest_ptr(actor + 432, 18));
    }
    for (i = 0; i < 9; ++i)
        axes[i] = r_u16(actor + 432 + 2 * i);
    for (i = 0; i < 9; ++i)
        w_u16(actor + 516 + 2 * i, axes[i]);
    (void)sub_8002090C(actor);
    if (flags & 136u)
    {
        value = r_u32(actor + 668);
        if (flags & 8u)
        {
            w_u32(actor + 636, 28595);
            w_u32(actor + 652, value + 28595u);
        }
        else
        {
            w_u32(actor + 652, 102943);
            w_u32(actor + 636, 102943u - value);
        }
        (void)sub_8003FB34((const uint16_t[3]){r_u16(actor + 444), r_u16(actor + 446), r_u16(actor + 448)}, ((scalar_value_106 = (652u * r_u32(actor + 652))), (((uint32_t)((!((uint32_t)(16))) ? (scalar_value_106) : ((scalar_value_106 >> ((uint32_t)(16))) | ((scalar_value_106 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(16)))) : 0)))))), matrix);
        (void)sub_8003FA40(xport_guest_cptr(actor + 432, 18), matrix, xport_guest_ptr(actor + 432, 18));
        w_u32(actor + 568, r_u32(actor + 568) | 256u);
    }
    if (flags & 32u)
        (void)sub_80068D20(actor, r_u32(actor + 852), 0);
    (void)sub_8008BA18(actor);
    value = sub_80020018((uint32_t)r_s16((actor + 444)) << 4, (uint32_t)r_s16((actor + 448)) << 4);
    w_u32(actor + 292, value);
    x = (uint32_t)r_s16((0x8005624c + 4 * (value & 4095u) + 2));
    value = r_u32(actor + 292);
    y = 0x8005624c + 4 * (value & 4095u);
    z = r_u32(actor + 308);
    w_u32(actor + 296, x << 4);
    w_u32(actor + 300, (uint32_t)r_s16(y) << 4);
    (void)sub_8002EAD8(xport_guest_ptr(actor + 184, 12), xport_guest_ptr(actor + 528, 6), 2u * z, xport_guest_ptr(actor + 880, 12));
    for (i = 0; i < 3; ++i)
    {
        axes[i] = r_u16(actor + 444 + 2 * i);
        axes[i + 3] = r_u16(actor + 432 + 2 * i);
    }
    speed = r_u32(actor + 924);
    w_u16(actor + 450, axes[0]);
    w_u32(actor + 480, speed);
    w_u32(actor + 576, speed);
    w_u16(actor + 452, axes[1]);
    w_u16(actor + 454, axes[2]);
    for (i = 0; i < 3; ++i)
        w_u16(actor + 814 + 2 * i, axes[i + 3]);
    w_u8(actor + 849, (uint8_t)(((int32_t)(uint32_t)(speed)) > 655360 ? (uint32_t)r_u8((r_u32(actor + 556) + 444)) - 1u : 0u));
    if ((flags & 64u) && r_u16(actor + 956) == 1)
    {
        value = r_u32(actor + 564);
        w_u16(actor + 960, 1);
        w_u32(actor + 564, value & ~512u);
    }
    if (!(flags & 32u))
    {
        secondary = r_u32(actor + 852);
        (void)sub_8001E0B4(xport_guest_ptr(secondary + 328, 32), xport_guest_ptr(actor + 328, 1), 32);
        for (k = 0; k < 2; ++k)
            for (i = 0; i < 3; ++i)
                w_u32(secondary + (k ? 468u : 184u) + 4 * i, r_u32(actor + 184 + 4 * i));
        value = r_u16(actor + 320);
        w_u32(secondary + 552, 0);
        w_u16(secondary + 320, value);
        if (!(flags & 8u))
            (void)sub_8007EC30(actor);
        (void)sub_80012838(actor, secondary, 2, 0);
        speed = r_u32(actor + 924);
        w_u8(actor + 72, 0);
        w_u32(actor + 480, speed);
        w_u32(actor + 576, speed);
        value = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(speed)), ((int32_t)(uint32_t)(speed)));
        speed = r_u32(actor + 480);
        w_u32(actor + 580, value);
        w_u8(actor + 849, (uint8_t)(((int32_t)(uint32_t)(speed)) > 655360 ? (uint32_t)r_u8((r_u32(actor + 556) + 444)) - 1u : 0u));
        for (k = 0; k < 3; ++k)
            for (i = 0; i < 3; ++i)
            {
                value = 444u - 6u * k + 2u * i;
                w_u16(secondary + value, r_u16(actor + value));
            }
        global = r_u32(0x8005b2f8);
        if (r_u16(actor + 172) >= r_u32(global + 48) && ((uint32_t)r_u8((r_u32(actor + 1084) + 1)) & 15u) == 2 && !r_u32(secondary + 540))
            w_u32(secondary + 540, sub_80012884(0x800ce170, secondary));
        (void)sub_80012858(r_u32(secondary + 540), r_u32(0x800ce190));
        global = r_u32(0x8005b2f8);
        value = 11;
        u = 1;
        if (r_u16(actor + 172) >= r_u32(global + 48) && ((uint32_t)r_u8((r_u32(actor + 1084) + 1)) & 15u) == 2 && !((uint32_t)r_u8((actor + 928)) & 16u))
        {
            value = 4;
            u = 0;
        }
        w_u32(secondary + 604, u);
        (void)sub_800C4550(value, secondary, 1);
    }
    w_u32(actor + 568, r_u32(actor + 568) | 0x08000000u);
ai_tail:
    if (r_u16(actor + 172) < r_u32(r_u32(0x8005b2f8) + 48))
        return 1;
    value = (uint32_t)r_u8((r_u32(actor + 1084) + 1)) & 15u;
    if (value != 2)
        return value;
    if ((uint32_t)r_s16((actor + 320)) && ((uint32_t)r_u8((actor + 928)) & 16u))
    {
        (void)sub_800BCD10(actor);
        type = 4;
    }
    else
    {
        (void)sub_8009DAF8(1);
        w_u8(actor + 928, (uint8_t)(((uint32_t)r_u8((actor + 928)) | 16u) & 223u));
        sub_8003AF9C(actor + 172, 0, player);
        w_u32(r_u32(actor + 1084) + 40, 0);
        (void)sub_800BCD10(actor);
        type = (uint32_t)r_s16((actor + 320)) ? 1u : 4u;
    }
    xport_store_le16(event, type);
    xport_store_le16(event + 2, 224);
    (void)sub_800BCA68(event, 1, actor);
    return sub_80028034(actor);
}

/* Motion transition 800723FC: branch priority 40,20,80,100. Unsigned
 * intermediates retain the MIPS wrapping sign masks at integer extremes. */
uint32_t sub_800723FC(uint32_t actor)
{
    uint32_t scalar_value_107;
    uint32_t scalar_value_108;

    FUNCTION_MARKER(0x800723FCu, "RASHCDG.BIN");
    uint32_t flags = r_u32(actor + 0x238), secondary = r_u32(actor + 0x354);
    uint32_t angle, rate, target, scale, value, sign, dot, linked;
    w_u32(actor + 0x3A4, 0);
    w_u32(actor + 0x2E8, 0);
    w_u32(actor + 0x234, r_u32(actor + 0x234) & 0xC0018200u);
    if (flags & 0x40)
    {
        angle = r_u32(actor + 0x28C);
        w_u32(actor + 0x2D4, 0);
        w_u32(actor + 0x2AC, 0);
        w_u32(actor + 0x298, angle);
        target = ((int32_t)(uint32_t)(angle)) > 0 ? 0x1921Fu : 0u - 0x1921Fu;
        if (r_u32(actor + 0x238) & 0x18000)
        {
            w_u32(actor + 0x248, 0);
            scale = 0x1AAAA;
            rate = (uint32_t)r_u8(0x800531F1) << 10;
            if (rate < 0x3333)
                rate = 0x3333;
            sign = (r_u32(actor + 0x238) >> 15) & 1;
            if ((sign && ((int32_t)(uint32_t)(angle)) > 0) || (!sign && ((int32_t)(uint32_t)(angle)) < 0))
            {
                if (((int32_t)(uint32_t)(angle)) < 0)
                    rate = 0u - rate;
            }
            else if (!angle)
                rate = ((0u - sign) & (2u * rate)) - rate;
            else if (((int32_t)(uint32_t)(angle)) > 0)
                rate = 0u - rate;
            w_u32(actor + 0x290, rate);
            w_u32(actor + 0x294, ((int32_t)(uint32_t)(rate)) < 0 ? 0xFFFF0000u : 0x10000u);
            w_u32(actor + 0x2A8, 0);
            w_u32(actor + 0x2B0, target);
        }
        else
        {
            scale = 0x35555;
            value = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(2u * (target - angle))), 0xB1C71);
            w_u32(actor + 0x294, value);
            w_u32(actor + 0x290, 0);
            if (r_u32(actor + 0x238) & 0x20000)
            {
                w_u32(actor + 0x2B0, target);
                value = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(target - r_u32(actor + 0x2A4))), 0x35555);
                w_u32(actor + 0x2A8, value);
            }
        }
        value = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(actor + 0x1E8))), ((int32_t)(uint32_t)(scale)));
        w_u32(actor + 0x248, 0u - value);
        value = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(actor + 0x268))), ((int32_t)(uint32_t)(scale)));
        w_u32(actor + 0x26C, 0u - value);
    }
    else if (flags & 0x20)
    {
        angle = r_u32(actor + 0x2A4);
        target = 0x1921F;
        if (r_u32(actor + 0x358) && ((int32_t)(uint32_t)(angle)) > 0)
            target += angle - 0x138C3;
        if (((int32_t)(uint32_t)(angle)) < 0 || (!angle && ((int32_t)(uint32_t)(r_u32(actor + 0x28C))) <= 0))
            target = 0u - target;
        w_u32(actor + 0x2B0, target);
        rate = r_u32(actor + 0x2A8);
        if (((int32_t)(uint32_t)(rate ^ target)) < 0)
            rate = 0u - rate;
        sign = ((int32_t)(uint32_t)(target)) > 0;
        if (!rate)
            rate = (sign << 18) - 0x20000;
        rate *= 4;
        if (((int32_t)(uint32_t)(rate)) < 0)
        {
            if (((int32_t)(uint32_t)(rate)) > -0x20000)
                rate = 0xFFFE0000;
        }
        else if (rate < 0x20000)
            rate = 0x20000;
        value = angle + (((scalar_value_107 = (angle + 0x1921F)), (((uint32_t)((!((uint32_t)(31))) ? (scalar_value_107) : ((scalar_value_107 >> ((uint32_t)(31))) | ((scalar_value_107 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(31)))) : 0)))))) & (0u - 0x1921F - angle));
        value += ((scalar_value_108 = (0x1921F - angle)), (((uint32_t)((!((uint32_t)(31))) ? (scalar_value_108) : ((scalar_value_108 >> ((uint32_t)(31))) | ((scalar_value_108 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(31)))) : 0)))))) & (0x1921F - angle);
        w_u32(actor + 0x2A4, value);
        dot = ((uint32_t)((!((uint32_t)(31))) ? (((uint32_t)(value))) : ((((uint32_t)(value)) >> ((uint32_t)(31))) | ((((uint32_t)(value)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(31)))) : 0))));
        dot = (value + dot) ^ dot;
        if (((int32_t)(uint32_t)(dot)) > 0x1921E)
            rate = 0;
        w_u32(actor + 0x2A8, rate);
        w_u32(actor + 0x2AC, 0);
        value = r_u32(actor + 0x27C);
        if (!value || (r_u32(actor + 0x358) && ((int32_t)(uint32_t)(value)) > 0))
            w_u32(actor + 0x27C, sign ? 0x41u : 0u - 0x41u);
        w_u32(actor + 0x238, r_u32(actor + 0x238) & 0xFFFDFFFFu);
    }
    else if (flags & 0x80)
    {
        value = r_u32(actor + 0x27C);
        sign = ((uint32_t)((!((uint32_t)(31))) ? (((uint32_t)(value))) : ((((uint32_t)(value)) >> ((uint32_t)(31))) | ((((uint32_t)(value)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(31)))) : 0))));
        value = (value + sign) ^ sign;
        w_u32(actor + 0x2D4, ((int32_t)(uint32_t)(value)) > 0x8000 ? 0xCCCu : 0u);
        w_u32(actor + 0x2AC, 0);
        w_u32(actor + 0x2B0, 0);
        w_u32(actor + 0x2A8, 0u - r_u32(actor + 0x2A4));
        if (r_u32(secondary + 0x25C) < 2)
        {
            w_u16(secondary + 0x1C8, r_u16(actor + 0x1C2));
            w_u16(secondary + 0x1CA, r_u16(actor + 0x1C4));
            value = r_u16(actor + 0x1C6);
            w_u32(secondary + 0x228, r_u32(secondary + 0x228) | 0x208000);
            w_u16(secondary + 0x1CC, (uint16_t)value);
        }
    }
    else if (flags & 0x100)
    {
        w_u32(actor + 0x2AC, 0);
        dot = sub_8002E698(xport_guest_ptr(actor + 0x210, 6), xport_guest_ptr(r_u32(actor + 0x154) + 2, 6));
        if (r_u32(actor + 0x238) & 0x400000)
            dot = 0u - dot;
        w_u32(actor + 0x2A8, ((int32_t)(uint32_t)(dot)) > 0 ? 0xFFFFC000u : 0x4000u);
        angle = r_u32(actor + 0x28C);
        w_u32(actor + 0x268, 0);
        w_u32(actor + 0x26C, 0);
        w_u32(actor + 0x270, 0);
        linked = r_u32(actor + 0x358);
        w_u32(actor + 0x28C, ((int32_t)(uint32_t)(angle)) > 0 ? 0x1921Fu : 0u - 0x1921Fu);
        w_u32(actor + 0x290, 0);
        w_u32(actor + 0x294, 0);
        w_u32(actor + 0x280, 0);
        w_u32(actor + 0x1E8, 0);
        w_u32(actor + 0x248, 0);
        if (linked)
            w_u32(linked + 0x1E8, 0);
        value = r_u32(actor + 0x1E0);
        w_u32(actor + 0x2C0, 0);
        w_u32(actor + 0x25C, 0);
        w_u32(actor + 0x2D4, 0x30000);
        (void)sub_8002EE50(value, xport_guest_ptr(actor + 0x1C2, 6), xport_guest_ptr(actor + 0x1C8, 12));
        w_u32(actor + 0x238, r_u32(actor + 0x238) & 0xEFFFFFFFu);
    }
    value = r_u32(actor + 0x238) & 0xFFFFF7FFu;
    w_u32(actor + 0x238, value);
    return value;
}

uint16_t *sub_8004D2A4(const uint16_t angles[3], uint16_t output[9])
{
    uint32_t sx, cx, sy, cy, sz, cz, packed, index, product, first, second, cross;
    int32_t angle, z_angle;
    FUNCTION_MARKER(0x8004D2A4, "SLUS_010.53");
    angle = (int16_t)xport_load_le16(angles);
    index = (angle < 0 ? 0 - (uint32_t)angle : (uint32_t)angle) & 4095;
    packed = r_u32(0x8005624C + 4 * index);
    sx = (uint32_t)(int32_t)(int16_t)packed;
    if (angle < 0)
        sx = 0 - sx;
    cx = (uint32_t)((int32_t)packed >> 16);
    angle = (int16_t)xport_load_le16(angles + 1);
    index = (angle < 0 ? 0 - (uint32_t)angle : (uint32_t)angle) & 4095;
    packed = r_u32(0x8005624C + 4 * index);
    sy = (uint32_t)(int32_t)(int16_t)packed;
    if (angle < 0)
        sy = 0 - sy;
    cy = (uint32_t)((int32_t)packed >> 16);
    z_angle = (int16_t)xport_load_le16(angles + 2);
    xport_store_le16(output + 2, (uint16_t)sy);
    product = 0 - cy * sx;
    xport_store_le16(output + 5, (uint16_t)((int32_t)product >> 12));
    product = cy * cx;
    xport_store_le16(output + 8, (uint16_t)((int32_t)product >> 12));
    index = (z_angle < 0 ? 0 - (uint32_t)z_angle : (uint32_t)z_angle) & 4095;
    packed = r_u32(0x8005624C + 4 * index);
    sz = (uint32_t)(int32_t)(int16_t)packed;
    if (z_angle < 0)
        sz = 0 - sz;
    cz = (uint32_t)((int32_t)packed >> 16);
    product = cz * cy;
    xport_store_le16(output, (uint16_t)((int32_t)product >> 12));
    product = 0 - sz * cy;
    xport_store_le16(output + 1, (uint16_t)((int32_t)product >> 12));
    product = cz * (0 - sy);
    cross = (uint32_t)((int32_t)product >> 12);
    product = cross * sx;
    first = (uint32_t)((int32_t)product >> 12);
    product = sz * cx;
    second = (uint32_t)((int32_t)product >> 12);
    xport_store_le16(output + 3, (uint16_t)(second - first));
    product = cross * cx;
    first = (uint32_t)((int32_t)product >> 12);
    product = sz * sx;
    second = (uint32_t)((int32_t)product >> 12);
    xport_store_le16(output + 6, (uint16_t)(second + first));
    product = sz * (0 - sy);
    cross = (uint32_t)((int32_t)product >> 12);
    product = cross * sx;
    first = (uint32_t)((int32_t)product >> 12);
    product = cz * cx;
    second = (uint32_t)((int32_t)product >> 12);
    xport_store_le16(output + 4, (uint16_t)(second + first));
    product = cross * cx;
    first = (uint32_t)((int32_t)product >> 12);
    product = cz * sx;
    second = (uint32_t)((int32_t)product >> 12);
    xport_store_le16(output + 7, (uint16_t)(second - first));
    return output;
}

uint32_t sub_80071D24(uint32_t actor)
{

    FUNCTION_MARKER(0x80071D24u, "RASHCDG.BIN");
    static const uint16_t clear[] = {0x290, 0x304, 0x26C, 0x270, 0x280, 0x2AC, 0x248, 0x2E8, 0x300, 0x260, 0x2A8};
    uint32_t value = r_u32(actor + 0x1E0), a, b, c, sign, n, d, np, dp, dot, secondary;
    unsigned i;
    uint32_t diagonal[3], direction[3], product[3];
    uint16_t angles[3], matrix[9], copied[6];
    for (i = 0; i < sizeof(clear) / sizeof(clear[0]); ++i)
        w_u32(actor + clear[i], 0);
    (void)sub_8002EE50(value, xport_guest_ptr(actor + 0x1C2, 6), xport_guest_ptr(actor + 0x1C8, 12));
    if (r_u32(actor + 0x238) & 0x400)
    {
        a = sub_8001FF3C(0u - ((uint32_t)r_s16((actor + 0x1BE)) << 4));
        if (r_u16(0x8005624E + 4 * (a & 4095u)))
        {
            b = sub_80020018((uint32_t)r_s16((actor + 0x1BC)) << 4, (uint32_t)r_s16((actor + 0x1C0)) << 4);
            c = sub_80020018((uint32_t)r_s16((actor + 0x1B2)) << 4, (uint32_t)r_s16((actor + 0x1B8)) << 4);
        }
        else
        {
            b = sub_80020018((uint32_t)r_s16((actor + 0x210)) << 4, (uint32_t)r_s16((actor + 0x214)) << 4);
            c = (uint32_t)((int32_t)(652 * r_u32(actor + 0x27C)) >> 16);
        }
        sign = (uint32_t)((int32_t)c >> 31);
        value = (c + sign) ^ sign;
        if (((int32_t)(uint32_t)(value)) > 0x400)
        {
            a = (((int32_t)(uint32_t)(a)) < 0 ? 0xFFFFF800u : 0x800u) - a;
            b += ((int32_t)(uint32_t)(b)) > 0 ? 0xFFFFF800u : 0x800u;
            c += ((int32_t)(uint32_t)(c)) > 0 ? 0xFFFFF800u : 0x800u;
        }
        w_u32(actor + 0x268, (uint32_t)((int32_t)(25736 * a) >> 8));
        w_u32(actor + 0x2B4, (uint32_t)((int32_t)(25736 * b) >> 8));
        w_u32(actor + 0x27C, (uint32_t)((int32_t)(25736 * c) >> 8));
        value = (uint32_t)r_s16((actor + 0x1C4)) << 4;
        sign = (uint32_t)((int32_t)value >> 31);
        value = (value + sign) ^ sign;
        d = 0x1E0000;
        if (((int32_t)(uint32_t)(value)) <= 0xB4FC)
            d = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(value)), 0x279A9D) + 0x20000;
        value = r_u32(actor + 0x1CC);
        n = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(value)), ((int32_t)(uint32_t)(value)));
        np = (int32_t)n > 0;
        dp = (int32_t)d > 0;
        value = r_u32(actor + 0x1CC);
        n = (uint32_t)sub_8001FC90((int32_t)value, (int32_t)value);
        value = sub_80010028(np ? n : 0 - n, dp ? d : 0 - d);
        if (np != dp)
            value = 0u - value;
        w_u32(actor + 0x1E4, value);
        if ((int32_t)value <= 0x9D086)
            w_u32(actor + 0x1E4, 0x9D087);
    }
    else
    {
        value = r_u32(actor + 0x268);
        w_u32(actor + 0x25C, 0);
        w_u32(actor + 0x2F4, 0);
        angles[0] = (uint16_t)(0 - (uint32_t)((int32_t)(652 * value) >> 16));
        angles[1] = 0;
        angles[2] = (uint16_t)(0 - (uint32_t)((int32_t)(652 * r_u32(actor + 0x28C)) >> 16));
        (void)sub_8004D2A4(angles, matrix);
        (void)sub_8003FA40(matrix, xport_guest_cptr(actor + 0x204, 18), xport_guest_ptr(actor + 0x1B0, 18));
        dot = sub_8002E698(xport_guest_ptr(actor + 0x1BC, 6), xport_guest_ptr(actor + 0x31C, 6));
        sign = (uint32_t)((int32_t)dot >> 31);
        value = (dot + sign) ^ sign;
        if (((int32_t)(uint32_t)(value)) > 0xF0A3)
        {
            for (i = 0; i < 3; ++i)
                copied[i] = r_u16(actor + 0x1B6 + 2 * i);
            for (i = 0; i < 3; ++i)
                w_u16(actor + 0x322 + 2 * i, copied[i]);
        }
        else
        {
            diagonal[0] = (uint32_t)(int32_t)r_s16(actor + 0x1BC);
            diagonal[1] = (uint32_t)(int32_t)r_s16(actor + 0x1BC + 2);
            xport_gte_write_control(0, diagonal[0]);
            diagonal[2] = (uint32_t)(int32_t)r_s16(actor + 0x1BC + 4);
            xport_gte_write_control(2, diagonal[1]);
            xport_gte_write_control(4, diagonal[2]);
            direction[0] = (uint32_t)(int32_t)r_s16(actor + 0x31C);
            direction[1] = (uint32_t)(int32_t)r_s16(actor + 0x31C + 2);
            direction[2] = (uint32_t)(int32_t)r_s16(actor + 0x31C + 4);
            xport_gte_write_data(9, direction[0]);
            xport_gte_write_data(10, direction[1]);
            xport_gte_write_data(11, direction[2]);
            xport_gte_execute(0x178000C);
            product[0] = xport_gte_read_data(9);
            product[1] = xport_gte_read_data(10);
            product[2] = xport_gte_read_data(11);
            w_u16(actor + 0x322, (uint16_t)product[0]);
            w_u16(actor + 0x322 + 2, (uint16_t)product[1]);
            w_u16(actor + 0x322 + 4, (uint16_t)product[2]);
            (void)sub_8002E468(xport_guest_ptr(actor + 0x322, 6));
        }
        diagonal[0] = (uint32_t)(int32_t)r_s16(actor + 0x31C);
        diagonal[1] = (uint32_t)(int32_t)r_s16(actor + 0x31C + 2);
        xport_gte_write_control(0, diagonal[0]);
        diagonal[2] = (uint32_t)(int32_t)r_s16(actor + 0x31C + 4);
        xport_gte_write_control(2, diagonal[1]);
        xport_gte_write_control(4, diagonal[2]);
        direction[0] = (uint32_t)(int32_t)r_s16(actor + 0x322);
        direction[1] = (uint32_t)(int32_t)r_s16(actor + 0x322 + 2);
        direction[2] = (uint32_t)(int32_t)r_s16(actor + 0x322 + 4);
        xport_gte_write_data(9, direction[0]);
        xport_gte_write_data(10, direction[1]);
        xport_gte_write_data(11, direction[2]);
        xport_gte_execute(0x178000C);
        product[0] = xport_gte_read_data(9);
        product[1] = xport_gte_read_data(10);
        product[2] = xport_gte_read_data(11);
        w_u16(actor + 0x328, (uint16_t)product[0]);
        w_u16(actor + 0x328 + 2, (uint16_t)product[1]);
        w_u16(actor + 0x328 + 4, (uint16_t)product[2]);
        value = sub_8001FF3C(dot);
        for (i = 0; i < 3; ++i)
            copied[i] = r_u16(actor + 0x328 + 2 * i);
        for (i = 0; i < 3; ++i)
            copied[i + 3] = r_u16(actor + 0x322 + 2 * i);
        w_u32(actor + 0x2B4, (uint32_t)((int32_t)(25736 * value) >> 8));
        w_u32(actor + 0x268, 0);
        for (i = 0; i < 3; ++i)
            w_u16(actor + 0x26C + 2 * i, copied[i]);
        for (i = 0; i < 3; ++i)
            w_u16(actor + 0x274 + 2 * i, copied[i + 3]);
        value = sub_8001FF3C(sub_8002E698(xport_guest_ptr(actor + 0x1B6, 6), xport_guest_ptr(actor + 0x322, 6)));
        w_u32(actor + 0x2C4, (uint32_t)((int32_t)(25736 * (0x400 - value)) >> 8));
        secondary = r_u32(actor + 0x354);
        value = r_u32(secondary + 0x228);
        if (r_u32(secondary + 0x25C) < 2)
            value |= 0x18000;
        w_u32(secondary + 0x228, value);
        if (r_u8(r_u32(actor + 0x354) + 0x23C) & 16)
        {
            secondary = r_u32(r_u32(actor + 0x358) + 0x354);
            value = r_u32(secondary + 0x228);
            if (r_u32(secondary + 0x25C) < 2)
                value |= 0x18000;
            w_u32(secondary + 0x228, value);
        }
        w_u32(actor + 0x2D4, 0);
    }
    for (i = 0; i < 3; ++i)
    {
        int64_t placement = (int64_t)((int32_t)r_s16(actor + 0x1B6 + 2 * i) * 16) * -0x570A;
        w_u32(actor + 0x310 + 4 * i, (uint32_t)((uint64_t)placement >> 16) + r_u32(actor + 0xB8 + 4 * i));
    }
    if (r_u32(actor + 0x358) && r_u32(actor + 0x440))
        (void)sub_8002EAD8(xport_guest_ptr(actor + 0x310, 12), xport_guest_ptr(actor + 0x1B0, 6), 2u * r_u32(actor + 0x130), xport_guest_ptr(actor + 0x310, 12));
    w_u32(actor + 0x238, r_u32(actor + 0x238) & 0xEFC7F7FFu);
    (void)sub_8002076C(actor);
    value = r_u32(actor + 0x234);
    secondary = r_u32(actor + 0x354);
    w_u32(actor + 0x234, value & 0xE7CE0383u);
    value = r_u32(secondary + 0x228) & 0xFFFFDFFFu;
    w_u32(secondary + 0x228, value);
    return value;
}

/* Actual movement producer 8007F0BC, including shared scratchpad append.
 * Components and products retain their individual fixed-point truncations. */
uint32_t sub_8007F0BC(uint32_t actor, uint32_t delta)
{
    FUNCTION_MARKER(0x8007F0BCu, "RASHCDG.BIN");
    uint32_t flags = r_u32(actor + 0x238), speed = r_u32(actor + 0x240), old = r_u32(actor + 0x1E0);
    uint32_t road_local, inhibit, special, normal, rotation_delta, rate, value, limit, road, sum, index, cosine, sine;
    unsigned i, j;
    if (!(flags & 0x600 ? old : speed) && !(r_u32(actor + 0x234) & 0x8000) && !(flags & 15) && !r_u32(actor + 0x1E8) && !r_u32(actor + 0x2A4))
        return 0;
    if (flags & 0x600)
    {
        if (r_u32(actor + 0x238) & 0x800)
        {
            (void)sub_80071D24(actor);
            w_u32(actor + 0x238, r_u32(actor + 0x238) | 0x08000000);
            w_u32(actor + 0x234, r_u32(actor + 0x234) | 0x00800000);
        }
        (void)sub_8002E570(xport_guest_ptr(actor + 0x310, 12), xport_guest_ptr(actor + 0x1C8, 12), delta, xport_guest_ptr(actor + 0x310, 12));
        (void)sub_8002EAD8(xport_guest_ptr(actor + 0x310, 12), xport_guest_ptr(actor + 0x1B6, 6), 0x570A, xport_guest_ptr(actor + 0xB8, 12));
        if (r_u32(actor + 0x358) && r_u32(actor + 0x440))
            (void)sub_8002EAD8(xport_guest_ptr(actor + 0xB8, 12), xport_guest_ptr(actor + 0x1B0, 6), 0u - 2u * r_u32(actor + 0x130), xport_guest_ptr(actor + 0xB8, 12));
    }
    else
    {
        for (i = 0; i < 3; ++i)
            w_u32(actor + 0x31C + 4 * i, r_u32(actor + 0xB8 + 4 * i) - r_u32(actor + 0x1F8 + 4 * i));
        (void)sub_8002EE50(speed, xport_guest_ptr(actor + 0x1C2, 6), xport_guest_ptr(actor + 0x1C8, 12));
        flags = r_u32(actor + 0x238);
        if ((flags & 0x800) && (flags & 0x1E0))
        {
            (void)sub_800723FC(actor);
            w_u32(actor + 0x238, r_u32(actor + 0x238) | 0x08000000);
        }
        flags = r_u32(actor + 0x234);
        road_local = (flags >> 18) & 1;
        if ((r_u32(actor + 0x238) & 15) || (flags & 8))
        {
            (void)sub_8002E570(xport_guest_ptr(actor + 0x310, 12), xport_guest_ptr(actor + 0x1C8, 12), delta, xport_guest_ptr(actor + 0x310, 12));
            (void)sub_8002EAD8(xport_guest_ptr(actor + 0x310, 12), xport_guest_ptr(actor + 0x210, 6), 0u - r_u32(actor + 0x134), xport_guest_ptr(actor + 0x1F8, 12));
        }
        else if (road_local)
        {
            for (j = 0; j < 2; ++j)
            {
                road = r_u32(actor + 0x340);
                sum = 0;
                for (i = 0; i < 3; ++i)
                    sum += (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(actor + 0x1C8 + 4 * i))), ((int32_t)(uint32_t)((uint32_t)r_s16((road + 0x104 + 12 * j + 2 * i)) << 4)));
                value = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(sum)), ((int32_t)(uint32_t)(delta)));
                w_u32(actor + 0x308 + 4 * j, r_u32(actor + 0x308 + 4 * j) + value);
            }
        }
        else
            (void)sub_8002E570(xport_guest_ptr(actor + 0x1F8, 12), xport_guest_ptr(actor + 0x1C8, 12), delta, xport_guest_ptr(actor + 0x1F8, 12));
        flags = r_u32(actor + 0x238);
        inhibit = (flags >> 14) & 1;
        for (i = 0; i < 3; ++i)
            w_u32(actor + 0xB8 + 4 * i, r_u32(actor + 0x1F8 + 4 * i) + r_u32(actor + 0x31C + 4 * i));
        special = ((flags & 31) || (r_u32(actor + 0x234) & 0x3400)) && !inhibit;
        normal = (!(r_u32(actor + 0x238) & 0x600) && r_u32(actor + 0x240)) || inhibit || road_local || r_u32(actor + 0x1E8) || r_u32(actor + 0x2A4);
        rotation_delta = delta;
        if ((r_u32(actor + 0x238) & 15) && !inhibit)
        {
            value = r_u32(actor + 0x2D4) + delta;
            limit = r_u32(actor + 0x2D8);
            w_u32(actor + 0x2D4, value);
            if (((int32_t)(uint32_t)(limit)) < ((int32_t)(uint32_t)(value)))
                rotation_delta = delta + limit - value;
        }
        if (special)
        {
            rate = r_u32(actor + 0x2E8);
            if (!rate)
                rate = r_u32(actor + 0x1E8);
            if (rate)
            {
                value = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(rate)), ((int32_t)(uint32_t)(rotation_delta)));
                index = ((163u * value) >> 14) & 4095;
                cosine = (uint32_t)r_s16((0x8005624E + 4 * index)) << 4;
                sine = (uint32_t)r_s16((0x8005624C + 4 * index)) << 4;
                (void)sub_8002EB78(xport_guest_ptr(actor + 0x210, 6), xport_guest_ptr(actor + 0x204, 6), xport_guest_ptr(actor + 0x210, 6), cosine, sine);
            }
        }
        else if (normal)
        {
            if (((int32_t)(uint32_t)(r_u32(actor + 0x24C))) <= 0 && ((int32_t)(uint32_t)(r_u32(actor + 0x240))) < 0x3334 && !(r_u32(actor + 0x230) & 0x300))
                w_u32(actor + 0x240, 0);
            if (r_u32(actor + 0x1E8) || r_u32(actor + 0x2A4) || road_local)
            {
                rate = r_u32(actor + 0x1E8) + r_u32(actor + 0x2E8);
                value = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(rate)), ((int32_t)(uint32_t)(delta))) + r_u32(actor + 0x2A4);
                index = ((163u * value) >> 14) & 4095;
                cosine = (uint32_t)r_s16((0x8005624E + 4 * index)) << 4;
                sine = (uint32_t)r_s16((0x8005624C + 4 * index)) << 4;
                (void)sub_8002EB78(xport_guest_ptr(actor + 0x1C2, 6), xport_guest_ptr(actor + 0x32E, 6), xport_guest_ptr(actor + 0x210, 6), cosine, sine);
            }
        }
        (void)sub_8002EAD8(xport_guest_ptr(actor + 0x1F8, 12), xport_guest_ptr(actor + 0x210, 6), 0u - r_u32(actor + 0x134), xport_guest_ptr(actor + 0x1F8, 12));
    }
    value = r_u32(0x1F800000);
    w_u32(value, actor);
    value = r_u32(0x1F800000) + 4;
    w_u32(0x1F800000, value);
    return value;
}

uint32_t sub_800B6E08(const uint32_t point[3], const uint32_t *vertices, uint32_t count, uint32_t axis)
{
    uint32_t x = axis + 1, y, prior = count - 1, index, px, py, dx, dy, first, second;
    FUNCTION_MARKER(0x800B6E08, "RASHCDG.BIN");
    x = (int32_t)x < 3 ? x : 0;
    y = x + 1;
    y = (int32_t)y < 3 ? y : 0;
    if ((int32_t)count <= 0)
        return 1;
    for (index = 0; (int32_t)index < (int32_t)count; ++index)
    {
        py = vertices[3 * prior + y];
        px = vertices[3 * prior + x];
        dy = vertices[3 * index + y] - py;
        dx = vertices[3 * index + x] - px;
        first = (uint32_t)sub_8001FC90((int32_t)dy, (int32_t)(point[x] - px));
        second = (uint32_t)sub_8001FC90((int32_t)dx, (int32_t)(point[y] - vertices[3 * prior + y]));
        if ((int32_t)(first - second) >= 0)
            return 0;
        prior = index;
    }
    return 1;
}

uint32_t sub_800B6844(const uint32_t *vertices, uint32_t count, uint16_t output[3])
{
    uint32_t vector[3] = {0}, index = 0, axis, a, b, x, y, sign, large;
    FUNCTION_MARKER(0x800B6844, "RASHCDG.BIN");
    if ((int32_t)(count - 1) > 0)
    {
        do
        {
            a = 3 * index;
            b = a + 3;
            for (axis = 0; axis < 3; ++axis)
            {
                x = (axis + 1) % 3;
                y = (axis + 2) % 3;
                vector[axis] += (uint32_t)sub_8001FC90((int32_t)(vertices[a + x] - vertices[b + x]), (int32_t)(vertices[a + y] + vertices[b + y]));
            }
            ++index;
        } while ((int32_t)index < (int32_t)(count - 1));
    }
    a = 3 * index;
    for (axis = 0; axis < 3; ++axis)
    {
        x = (axis + 1) % 3;
        y = (axis + 2) % 3;
        vector[axis] += (uint32_t)sub_8001FC90((int32_t)(vertices[a + x] - vertices[x]), (int32_t)(vertices[a + y] + vertices[y]));
    }
    for (;;)
    {
        large = 0;
        for (axis = 0; axis < 3; ++axis)
        {
            sign = (uint32_t)((int32_t)vector[axis] >> 31);
            if ((int32_t)((vector[axis] + sign) ^ sign) > 0x5A8000)
                large = 1;
        }
        if (!large)
            break;
        for (axis = 0; axis < 3; ++axis)
            vector[axis] = (uint32_t)((int32_t)vector[axis] >> 1);
    }
    (void)sub_8002E14C(vector);
    for (axis = 0; axis < 3; ++axis)
        output[axis] = (uint16_t)((int32_t)vector[axis] >> 4);
    return (uint32_t)((int32_t)vector[2] >> 4);
}


uint32_t sub_800A8498(uint32_t position, uint32_t direction, uint32_t *geometry, uint32_t *vertex_output, uint32_t origin[3], uint32_t *selection)
{
    uint32_t bank, end, slot, attempt, handle, header, indices, group, tries, count, i, j;
    uint32_t point[3], vertices[18], minimum[2], maximum[2], start, index, value, best, chosen;
    uint32_t x, z, sign, temporary, distance, edge[3], normal[3], dot, cross, entry, offset, base, upper;
    int64_t product;
    FUNCTION_MARKER(0x800A8498, "RASHCDG.BIN");
    bank = (*selection & 0x80000000) ? 12 : 0;
    end = bank + 12;
    slot = *selection & 63;
    for (attempt = bank; attempt < end; ++attempt, ++slot)
    {
        if (slot < bank || slot >= end)
            slot = bank;
        handle = 0x800D87EC + 112 * slot;
        if (r_u32(handle - 4) == UINT32_MAX)
            continue;
        header = r_u32(handle);
        *vertex_output = r_u32(header + 52) + 4;
        header = r_u32(handle);
        indices = r_u32(header + 44);
        origin[0] = r_u32(header + 8) << 10;
        origin[1] = r_u32(r_u32(handle) + 12) << 10;
        origin[2] = r_u32(r_u32(handle) + 16) << 10;
        for (i = 0; i < 3; ++i)
            point[i] = r_u32(position + 4 * i) - origin[i];
        group = (*selection >> 6) & 7;
        header = r_u32(handle);
        for (tries = 0; tries < r_u16(header + 6); ++tries, ++group)
        {
            if (group >= r_u16(header + 6))
                group = 0;
            start = (uint32_t)r_s16(indices + 2 * group + 2);
            count = (uint32_t)r_s16(indices + 2 * group + 4) - start;
            minimum[0] = minimum[1] = 0x3FFF0000;
            maximum[0] = maximum[1] = 0xC0010000;
            /* TODO Retain the existing bound check until malformed hull handling is resolved */
            if ((int32_t)count > 6 || ((int32_t)count > 0 && (count & 1)))
                abort();
            for (i = 0; (int32_t)i < (int32_t)count; ++i)
            {
                index = (uint32_t)r_s16(indices + 2 * ((uint32_t)r_s16(indices + 2 * group + 2) + i));
                for (j = 0; j < 3; ++j)
                    vertices[3 * (count - 1 - i) + j] = (uint32_t)r_s16(*vertex_output + 8 * index + 2 * j) << 10;
                for (j = 0; j < 2; ++j)
                {
                    value = vertices[3 * (count - 1 - i) + 2 * j];
                    if ((int32_t)value < (int32_t)minimum[j])
                        minimum[j] = value;
                    if ((int32_t)value > (int32_t)maximum[j])
                        maximum[j] = value;
                }
            }
            if ((int32_t)point[0] >= (int32_t)minimum[0] && (int32_t)point[0] <= (int32_t)maximum[0] &&
                (int32_t)point[2] >= (int32_t)minimum[1] && (int32_t)point[2] <= (int32_t)maximum[1] && sub_800B6E08(point, vertices, count, 1))
            {
                *selection = (*selection & 0xFFFFF800) | slot | (group << 6);
                chosen = 0;
                best = 0x3FFF0000;
                for (i = 0; i < count; i += 2)
                {
                    x = point[0] - (uint32_t)((int32_t)(vertices[3 * i] + vertices[3 * (i + 1)]) / 2);
                    sign = (uint32_t)((int32_t)x >> 31);
                    x = (x + sign) ^ sign;
                    z = point[2] - (uint32_t)((int32_t)(vertices[3 * i + 2] + vertices[3 * (i + 1) + 2]) / 2);
                    sign = (uint32_t)((int32_t)z >> 31);
                    z = (z + sign) ^ sign;
                    if ((int32_t)x < (int32_t)z)
                    {
                        temporary = x; x = z; z = temporary;
                    }
                    temporary = z + (uint32_t)((int32_t)z >> 1);
                    distance = x - (uint32_t)((int32_t)x >> 5) - (uint32_t)((int32_t)x >> 7) +
                        (uint32_t)((int32_t)temporary >> 2) + (uint32_t)((int32_t)temporary >> 6);
                    if ((int32_t)distance < (int32_t)best)
                    {
                        chosen = (uint32_t)((int32_t)(count - i - 1) / 2);
                        best = distance;
                    }
                }
                *selection |= chosen << 9;
                index = count - 2 * chosen;
                for (i = 0; i < 3; ++i)
                {
                    edge[i] = (uint32_t)((int32_t)(vertices[3 * (index - 2) + i] - vertices[3 * (index - 1) + i]) / 4);
                    normal[i] = (uint32_t)((int32_t)r_s16(direction + 2 * i) * 16);
                }
                dot = 0;
                for (i = 0; i < 3; ++i)
                {
                    product = (int64_t)(int32_t)edge[i] * (int32_t)normal[i];
                    dot += (uint32_t)((uint64_t)product >> 16);
                }
                product = (int64_t)(int32_t)edge[2] * (int32_t)normal[0];
                cross = (uint32_t)((uint64_t)product >> 16);
                product = (int64_t)(int32_t)edge[0] * (int32_t)normal[2];
                cross -= (uint32_t)((uint64_t)product >> 16);
                value = *selection;
                if ((int32_t)cross < 0)
                    value |= 0x2000;
                *selection = value | ((uint32_t)((int32_t)dot >> 31) & 0x1000);
                upper = 0;
                if (*selection & 0x800)
                {
                    header = r_u32(handle);
                    base = r_u32(header + 60);
                    if (base)
                    {
                        offset = r_u16(header + 4) + r_u16(header + 6);
                        entry = r_u32(header + 40) + 12 * (offset + group);
                        *geometry = base + r_u32(entry);
                        upper = 1;
                    }
                }
                if (!upper)
                {
                    if (!r_u32(r_u32(handle) + 56))
                        goto missing;
                    *selection &= 0xFFFFF7FF;
                    header = r_u32(handle);
                    offset = r_u16(header + 4);
                    entry = r_u32(header + 40) + 12 * (offset + group);
                    value = r_u32(entry);
                    base = r_u32(header + 56);
                    *geometry = base + value;
                }
                header = r_u32(handle);
                offset = r_u16(header + 4);
                if (upper)
                    offset += r_u16(header + 6);
                entry = r_u32(header + 40) + 12 * (offset + group);
                return r_u32(entry + 4) | (r_u32(entry + 8) << 16);
            }
            header = r_u32(handle);
        }
    }
missing:
    *selection = 0;
    return 0;
}

uint32_t sub_800A7BF8(uint32_t actor, uint32_t position, uint32_t output[3], uint16_t normal_out[3], uint32_t prior)
{
    uint32_t geometry = 0, vertex_base = 0, origin[3], selection = prior & 0x7ff, distance = r_u32(actor + 44), reply;
    uint32_t point[3], vertices[12], table, offset, same, i, j, n, value, category, index, last, finish, step, group_step, limit, iterations = 0, status;
    uint32_t bounds_min[2], bounds_max[2], quad, face, face_index, small, point_index, normal_start = 0;
    uint8_t ranges[6];
    FUNCTION_MARKER(0x800A7BF8, "RASHCDG.BIN");
    if (!position)
        position = actor + 184;
    if (r_u32(r_u32(0x8005B2F8) + 48) >= 2 && ((int32_t)(uint32_t)(r_u32(actor + 48))) < ((int32_t)(uint32_t)(distance)))
    {
        distance = r_u32(actor + 48);
        selection |= 0x80000000;
    }
    if (((int32_t)(uint32_t)(distance)) < 1280)
        selection |= 0x800;
    reply = sub_800A8498(position, actor + 450, &geometry, &vertex_base, origin, &selection);
    selection &= 0x7fffffff;
    if (!reply)
        return 0xffffffff;
    for (i = 0; i < 3; ++i)
        point[i] = r_u32(position + 4 * i) - origin[i];
    same = ((selection ^ prior) & 0xfff) == 0;
    offset = same ? ((prior >> 14) & 1023) : 0xffffffff;
    table = r_u32(r_u32(0x800D87EC + 112 * (selection & 63)) + 48);
    if (offset >= 512)
    {
        n = (uint32_t)r_u8((table + ((selection >> 11) & 1)));
        offset = 2;
        for (i = 0; i < n; ++i)
        {
            value = (uint32_t)r_u8((table + offset));
            if ((value >> 7) == ((selection >> 11) & 1) && ((value >> 4) & 7) == ((selection >> 6) & 7) && ((value >> 2) & 3) == ((selection >> 9) & 3))
                break;
            offset += 2 + (uint32_t)r_u8((table + offset + 1));
        }
        if (i == n)
            return 0xffffffff;
    }
    table += offset;
    n = (uint32_t)r_u8((table + 1));
    if (!n)
        return (offset << 14) | (selection & 0xfff) | 0xff000000;
    index = (prior >> 24) & 127;
    category = (prior >> 12) & 3;
    ranges[0] = 2;
    ranges[1] = (uint8_t)(n + 1);
    ranges[2] = (uint8_t)(n + 4);
    value = (uint32_t)ranges[2] + (uint32_t)r_u8((table + ranges[2] - 1));
    ranges[3] = (uint8_t)(value - 1);
    ranges[4] = (uint8_t)(value + 2);
    ranges[5] = (uint8_t)(ranges[4] + ((uint32_t)r_u8((table + ranges[4] - 1)) & (0 - ((selection >> 11) & 1))) - 1);
    step = 1 - ((selection >> 11) & 2);
    group_step = 1 - ((selection >> 12) & 2);
    if (category == 3 || ranges[2 * category + 1] < ranges[2 * category])
        category = 0;
    if (!same || index == 127 || index < 2)
        index = ranges[2 * category + (step >> 31)];
    last = ranges[2 * category + (((int32_t)(uint32_t)(step)) < 0 ? step + 1 : step)];
    finish = group_step == 1 ? ((selection >> 11) & 1) + 1 : 0;
    while (finish && ranges[2 * finish + 1] < ranges[2 * finish])
        --finish;
    limit = (uint32_t)ranges[1] - ranges[0] + ranges[3] - ranges[2] + ((0 - ((selection >> 11) & 1)) & ((uint32_t)ranges[5] - ranges[4] + 1)) + 2;
    do
    {
        bounds_min[0] = bounds_min[1] = 0x3fff0000;
        bounds_max[0] = bounds_max[1] = 0xc0010000;
        face_index = (uint32_t)r_u8((table + index));
        small = reply & 65535;
        quad = face_index >= small;
        face_index -= quad ? small : 0;
        face = geometry + 14 + 20 * (quad ? small : face_index) + (quad ? 2 * (12 * face_index + 1) : 0);
        n = 3 + quad;
        for (i = 0; i < n; ++i)
        {
            for (j = 0; j < 3; ++j)
                vertices[3 * i + j] = (uint32_t)r_s16((vertex_base + 8 * r_u16(face + 2 * i) + 2 * j)) << 10;
            for (j = 0; j < 2; ++j)
            {
                value = vertices[3 * i + 2 * j];
                if (((int32_t)(uint32_t)(value)) < ((int32_t)(uint32_t)(bounds_min[j])))
                    bounds_min[j] = value;
                if (((int32_t)(uint32_t)(value)) > ((int32_t)(uint32_t)(bounds_max[j])))
                    bounds_max[j] = value;
            }
        }
        if (((int32_t)(uint32_t)(point[0])) >= ((int32_t)(uint32_t)(bounds_min[0])) && ((int32_t)(uint32_t)(point[0])) <= ((int32_t)(uint32_t)(bounds_max[0])) && ((int32_t)(uint32_t)(point[2])) >= ((int32_t)(uint32_t)(bounds_min[1])) && ((int32_t)(uint32_t)(point[2])) <= ((int32_t)(uint32_t)(bounds_max[1])) && sub_800B6E08(point, vertices, n, 1))
        {
            normal_start = 0;
            point_index = 0;
            if (quad)
            {
                point_index = 3;
                if (sub_800B6E08(point, vertices + 3, 3, 1))
                    normal_start = 3;
                else
                    for (i = 0; i < 3; ++i)
                        vertices[6 + i] = vertices[9 + i];
            }
            (void)sub_800B6844(vertices + normal_start, 3, normal_out);
            {
                uint16_t nx = normal_out[0], nz = normal_out[2], ny;
                normal_out[0] = (uint16_t)(0 - nx);
                ny = normal_out[1];
                normal_out[2] = (uint16_t)(0 - nz);
                normal_out[1] = (uint16_t)(0 - ny);
            }
            for (i = 0; i < 3; ++i)
                output[i] = vertices[point_index + i] + origin[i];
            w_u8(actor + 534, (uint8_t)(r_u16(face - (quad ? 14 : 12)) >> 12));
            status = 2;
        }
        else
        {
            status = 0;
            if (index == last)
            {
                if (category == finish)
                    status = 1;
                else
                {
                    category += group_step;
                    value = 2 * category + (step >> 31);
                    index = ranges[value];
                    last = ranges[value + step];
                }
            }
            else
                index += step;
        }
        ++iterations;
        if (((int32_t)(uint32_t)(limit)) < ((int32_t)(uint32_t)(iterations)))
            break;
    } while (!status);
    if (status == 2)
        return (index << 24) | (offset << 14) | (category << 12) | (selection & 0xfff);
    return ((0 - status) & (((offset << 14) | (selection & 0xfff) | 0xff000000) + 1)) - 1;
}

/* 80075628: contact-triggered mode transition and direction adjustment. */
uint32_t sub_80075628(uint32_t actor, uint32_t normal, uint32_t distance)
{
    uint32_t scalar_value_148;
    uint32_t scalar_value_151;
    uint32_t scalar_value_152;
    uint32_t scalar_value_153;
    uint32_t scalar_value_154;

    FUNCTION_MARKER(0x80075628u, "RASHCDG.BIN");
    uint32_t queried, trigger = 0, speed, dot, value, modified, angles[2], owner, sign, target, offset, x, z, y, sum, numerator, ratio, scale;
    if ((r_u32(actor + 564) & 0x61D800u) || (r_u32(actor + 568) & 0x7FFu))
        return 0;
    queried = r_u32(actor + 388) & 1u;
    if ((((int32_t)(uint32_t)(distance)) > 32768 && ((int32_t)(uint32_t)(r_u32(actor + 480))) > 732430) || (uint32_t)r_u8((actor + 534)) == 4)
    {
        trigger = 1;
        queried = 1;
    }
    else if (queried)
    {
        dot = sub_8002E698(xport_guest_ptr(normal, 6), xport_guest_ptr(actor + 450, 6));
        if (((int32_t)(uint32_t)(dot)) <= 0)
        {
            value = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(dot)), ((int32_t)(uint32_t)((uint32_t)r_s16((normal + 2)) << 4)));
            modified = ((uint32_t)r_s16((actor + 452)) - ((scalar_value_148 = (value << 12)), (((uint32_t)((!((uint32_t)(16))) ? (scalar_value_148) : ((scalar_value_148 >> ((uint32_t)(16))) | ((scalar_value_148 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(16)))) : 0))))))) << 4;
            /* Inline MIPS uses the same table/interpolation as 8001FF3C. */
            angles[0] = sub_8001FF3C((uint32_t)r_s16((actor + 452)) << 4);
            angles[1] = sub_8001FF3C(modified);
            trigger = ((int32_t)(uint32_t)(angles[1] - angles[0])) >= ((int32_t)(uint32_t)(r_u32(r_u32(actor + 556) + 440)));
        }
    }
    else if (r_u32(actor + 828))
    {
        dot = sub_8002E698(xport_guest_ptr(r_u32(actor + 340) + 14, 6), xport_guest_ptr(actor + 450, 6));
        owner = (uint32_t)r_s16((r_u32(actor + 828) + 50));
        if (((int32_t)(uint32_t)(owner ^ r_u32(actor + 364))) >= 0)
        {
            sign = ((uint32_t)((!((uint32_t)(31))) ? (((uint32_t)(owner))) : ((((uint32_t)(owner)) >> ((uint32_t)(31))) | ((((uint32_t)(owner)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(31)))) : 0))));
            value = (owner + sign) ^ sign;
            if (((int32_t)(uint32_t)(value)) < ((int32_t)(uint32_t)((uint32_t)r_s16((actor + 482)))))
            {
                sign = ((uint32_t)((!((uint32_t)(31))) ? (((uint32_t)(dot))) : ((((uint32_t)(dot)) >> ((uint32_t)(31))) | ((((uint32_t)(dot)) & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(31)))) : 0))));
                trigger = ((int32_t)(uint32_t)((dot + sign) ^ sign)) > 46333;
            }
        }
    }
    if (!trigger)
        return 0;
    w_u32(actor + 568, r_u32(actor + 568) | 0xC00u);
    if (queried)
        return 1;
    speed = r_u32(actor + 480);
    target = (uint32_t)r_s16((actor + 452)) << 4;
    if (((int32_t)(uint32_t)(speed)) <= 3515664)
    {
        offset = ((int32_t)(uint32_t)(speed)) > 2343776 ? ((scalar_value_151 = ((uint32_t)sub_8001FC90(80629, ((int32_t)(uint32_t)(3515665 - speed))))), (((uint32_t)((!((uint32_t)(16))) ? (scalar_value_151) : ((scalar_value_151 >> ((uint32_t)(16))) | ((scalar_value_151 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(16)))) : 0)))))) : 22;
        value = ((offset + 1024 - sub_8001FF3C(target)) & 4095) * 4;
        target = (uint32_t)r_s16((0x8005624C + (value | 2u))) << 4;
    }
    target >>= 4;
    speed = r_u32(actor + 576);
    if (((int32_t)(uint32_t)(speed)) > 29032446)
    {
        value = (0u - sub_80010028(29032446, speed)) >> 4;
        if (((int32_t)(uint32_t)(value << 16)) < ((int32_t)(uint32_t)(target << 16)))
            value = target;
        target = value;
    }
    y = ((scalar_value_152 = (target << 16)), (((uint32_t)((!((uint32_t)(16))) ? (scalar_value_152) : ((scalar_value_152 >> ((uint32_t)(16))) | ((scalar_value_152 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(16)))) : 0))))));
    if (y == (uint32_t)r_s16((actor + 452)))
        return 1;
    x = (uint32_t)r_s16((actor + 450)) << 4;
    z = (uint32_t)r_s16((actor + 454)) << 4;
    w_u16(actor + 452, (uint16_t)target);
    y <<= 4;
    sum = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(x)), ((int32_t)(uint32_t)(x))) + (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(z)), ((int32_t)(uint32_t)(z)));
    if (((int32_t)(uint32_t)(sum)) < 6)
    {
        x = 0;
        z = 655;
        sum = 6;
    }
    numerator = 65536 - (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(y)), ((int32_t)(uint32_t)(y)));
    ratio = ((int32_t)(uint32_t)(numerator)) > 0 ? sub_80010028(numerator, sum) : 0u - sub_80010028(0u - numerator, sum);
    scale = sub_8004CF74(ratio) << 2;
    w_u16(actor + 450, (uint16_t)((scalar_value_153 = ((uint32_t)sub_8001FC90(((int32_t)(uint32_t)(scale)), ((int32_t)(uint32_t)(x))))), (((uint32_t)((!((uint32_t)(4))) ? (scalar_value_153) : ((scalar_value_153 >> ((uint32_t)(4))) | ((scalar_value_153 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(4)))) : 0)))))));
    w_u16(actor + 454, (uint16_t)((scalar_value_154 = ((uint32_t)sub_8001FC90(((int32_t)(uint32_t)(scale)), ((int32_t)(uint32_t)(z))))), (((uint32_t)((!((uint32_t)(4))) ? (scalar_value_154) : ((scalar_value_154 >> ((uint32_t)(4))) | ((scalar_value_154 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(4)))) : 0)))))));
    return 1;
}

/* 80075B08: contact deflection with signed16-truncated products. */
uint32_t sub_80075B08(uint32_t actor, uint32_t normal)
{
    uint32_t scalar_value_155;
    uint32_t scalar_value_156;

    FUNCTION_MARKER(0x80075B08u, "RASHCDG.BIN");
    uint32_t x, z, value;
    if ((r_u32(actor + 568) & 0x60Fu) || (r_u32(actor + 564) & 0x400u))
        return 0;
    if (((int32_t)(uint32_t)((uint32_t)r_s16((normal + 2)))) >= 2633)
        return 0;
    x = (uint32_t)r_s16((actor + 450)) * (uint32_t)r_s16(normal);
    z = (uint32_t)r_s16((actor + 454)) * (uint32_t)r_s16((normal + 4));
    value = ((scalar_value_155 = (x << 4)), (((uint32_t)((!((uint32_t)(16))) ? (scalar_value_155) : ((scalar_value_155 >> ((uint32_t)(16))) | ((scalar_value_155 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(16)))) : 0)))))) + ((scalar_value_156 = (z << 4)), (((uint32_t)((!((uint32_t)(16))) ? (scalar_value_156) : ((scalar_value_156 >> ((uint32_t)(16))) | ((scalar_value_156 & 0x80000000) ? (0xFFFFFFFFu << (32 - ((uint32_t)(16)))) : 0))))));
    if (((int32_t)(uint32_t)(value)) <= 0)
        return 0;
    w_u16(actor + 820, (uint16_t)(0u - r_u16(normal)));
    w_u16(actor + 822, 0);
    w_u16(actor + 824, (uint16_t)(0u - r_u16(normal + 4)));
    (void)sub_8002E468(xport_guest_ptr(actor + 820, 6));
    w_u32(actor + 564, r_u32(actor + 564) | 0x400000u);
    return 1;
}

/* 8002ECB8: sum two scaled short vectors, preserving output aliases. */
int64_t sub_8002ECB8(const uint16_t left[3], const uint16_t right[3], uint32_t output[3], uint32_t scale_left, uint32_t scale_right)
{
    uint32_t axis;
    int64_t result = 0;
    FUNCTION_MARKER(0x8002ECB8, "SLUS_010.53");
    for (axis = 0; axis < 3; ++axis)
    {
        uint32_t component = (uint32_t)(int32_t)(int16_t)xport_load_le16((const uint8_t *)left + 2u * axis) << 4;
        uint32_t first = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(scale_left)), ((int32_t)(uint32_t)(component)));
        component = (uint32_t)(int32_t)(int16_t)xport_load_le16((const uint8_t *)right + 2u * axis) << 4;
        result = sub_8001FC90(((int32_t)(uint32_t)(scale_right)), ((int32_t)(uint32_t)(component)));
        xport_store_le32((uint8_t *)output + 4u * axis, first + (uint32_t)result);
    }
    return result;
}

/* 8007504C: project actor motion onto road or queried geometry; update child. */
uint32_t sub_8007504C(uint32_t actor, uint32_t position)
{

    FUNCTION_MARKER(0x8007504Cu, "RASHCDG.BIN");
    uint32_t first_point[3], second_point[3], difference[3], queried = r_u32(actor + 388) & 1u;
    uint32_t saved_material = 0, result, reply, normal_address, flags, i, dot = 0, linked, road, other_road;
    uint16_t normal_local[3], captured[3];
    int local_normal = 0;
    const uint8_t *plane;
    if (queried)
    {
        plane = (const uint8_t *)first_point;
        normal_address = actor + 268;
        reply = sub_800A7BF8(actor, position, first_point, xport_guest_ptr(normal_address, 6), r_u32(actor + 536));
        flags = r_u32(actor + 388);
        if (reply == r_u32(actor + 536))
            flags &= ~64u;
        else
        {
            w_u32(actor + 536, reply);
            flags |= 64u;
        }
        w_u32(actor + 388, flags);
        saved_material = (uint32_t)r_u8((actor + 534));
    }
    else
    {
        road = r_u32(actor + 340);
        reply = 0;
        w_u32(actor + 536, 0);
        plane = xport_guest_cptr(road + 20, 12);
        normal_address = road + 8;
        w_u8(actor + 534, (uint8_t)(r_u32(actor + 372) ? (uint32_t)r_u8((actor + 394)) : 1));
    }
    if (((int32_t)(uint32_t)(reply)) >= 0 && ((int32_t)(uint32_t)((uint32_t)r_s16((normal_address + 2)))) < -615)
    {
        for (i = 0; i < 3; ++i)
            w_u16(actor + 274 + 2 * i, (uint16_t)(0u - r_u16(normal_address + 2 * i)));
    }
    else
    {
        for (i = 0; i < 3; ++i)
            captured[i] = r_u16(actor + 522 + 2 * i);
        plane = xport_guest_cptr(actor + 504, 12);
        for (i = 0; i < 3; ++i)
            w_u16(actor + 274 + 2 * i, captured[i]);
        if (!r_u32(actor + 480) && (uint32_t)r_u8((r_u32(actor + 1084) + 39)) != 255)
        {
            w_u32(actor + 480, 117188);
            (void)sub_8002EE50(117188, xport_guest_ptr(actor + 450, 6), xport_guest_ptr(actor + 456, 12));
        }
    }
    for (i = 0; i < 3; ++i)
        difference[i] = xport_load_le32(plane + 4 * i) - r_u32(position + 4 * i);
    for (i = 0; i < 3; ++i)
    {
        int64_t product = (int64_t)(int32_t)difference[i] * ((int32_t)r_s16(actor + 274 + 2 * i) * 16);
        dot += (uint32_t)((uint64_t)product >> 16);
    }
    for (i = 0; i < 3; ++i)
    {
        int64_t product = (int64_t)((int32_t)r_s16(actor + 274 + 2 * i) * 16) * (int32_t)dot;
        w_u32(actor + 504 + 4 * i, (uint32_t)((uint64_t)product >> 16) + r_u32(position + 4 * i));
    }
    w_u32(actor + 260, dot);
    if (queried)
    {
        reply = sub_800A7BF8(actor, actor + 244, second_point, xport_guest_ptr(actor + 268, 6), r_u32(actor + 572));
        flags = r_u32(actor + 564) & 0xfdffffffu;
        w_u32(actor + 564, flags);
        if (((int32_t)(uint32_t)(reply)) < 0)
        {
            for (i = 0; i < 3; ++i)
                captured[i] = r_u16(actor + 274 + 2 * i);
            for (i = 0; i < 3; ++i)
                w_u16(actor + 268 + 2 * i, captured[i]);
            for (i = 0; i < 3; ++i)
                w_u32(actor + 280 + 4 * i, xport_load_le32(plane + 4 * i));
        }
        else
        {
            if (reply != r_u32(actor + 572))
                w_u32(actor + 564, flags | 0x02000000u);
            captured[0] = r_u16(actor + 268);
            captured[2] = r_u16(actor + 272);
            w_u16(actor + 268, (uint16_t)(0u - captured[0]));
            captured[1] = r_u16(actor + 270);
            w_u16(actor + 272, (uint16_t)(0u - captured[2]));
            w_u16(actor + 270, (uint16_t)(0u - captured[1]));
            for (i = 0; i < 3; ++i)
                w_u32(actor + 280 + 4 * i, second_point[i]);
        }
        result = (uint32_t)r_u8((actor + 534));
        w_u32(actor + 572, reply);
        w_u8(actor + 534, (uint8_t)saved_material);
    }
    else
    {
        road = r_u32(actor + 256);
        w_u32(actor + 572, 0);
        for (i = 0; i < 2; ++i)
            w_u16(actor + 268 + 2 * i, (uint16_t)(0u - r_u16(road + 8 + 2 * i)));
        captured[2] = r_u16(road + 12);
        other_road = r_u32(actor + 340);
        w_u16(actor + 272, (uint16_t)(0u - captured[2]));
        for (i = 0; i < 3; ++i)
            w_u32(actor + 280 + 4 * i, r_u32(road + 20 + 4 * i));
        flags = r_u32(actor + 564);
        flags = other_road == road ? flags & 0xfdffffffu : flags | 0x02000000u;
        w_u32(actor + 564, flags);
        result = r_u32(actor + 372) ? (uint32_t)r_u8((actor + 394)) : 1;
    }
    w_u8(actor + 535, (uint8_t)result);
    linked = r_u32(actor + 856);
    if (!linked)
        return result;
    result = r_u32(actor + 1088);
    if (!result)
        return result;
    w_u32(linked + 44, r_u32(actor + 44));
    flags = r_u32(linked + 388) & 1u;
    w_u32(linked + 48, r_u32(actor + 48));
    if (flags)
    {
        plane = (const uint8_t *)first_point;
        local_normal = 1;
        reply = sub_800A7BF8(linked, 0, first_point, normal_local, r_u32(linked + 536));
        w_u32(linked + 536, reply);
    }
    else
    {
        reply = 0;
        w_u8(linked + 534, (uint8_t)(r_u32(linked + 372) ? (uint32_t)r_u8((linked + 394)) : 1));
        w_u32(linked + 536, 0);
    }
    if (((int32_t)(uint32_t)(reply)) <= 0)
    {
        road = r_u32(linked + 340);
        plane = xport_guest_cptr(road + 20, 12);
        normal_address = road + 8;
        local_normal = 0;
        if (((int32_t)(uint32_t)(reply)) < 0)
        {
            if (r_u32(linked + 480))
                return 65536;
            w_u32(linked + 480, 117188);
            return (uint32_t)sub_8002EE50(117188, xport_guest_ptr(linked + 450, 6), xport_guest_ptr(linked + 456, 12));
        }
    }
    result = local_normal ? (uint32_t)(int32_t)(int16_t)normal_local[1] : (uint32_t)r_s16((normal_address + 2));
    result = ((int32_t)(uint32_t)(result)) < -614;
    if (!result)
        return result;
    for (i = 0; i < 3; ++i)
        w_u16(linked + 814 + 2 * i, (uint16_t)(0u - (local_normal ? normal_local[i] : r_u16(normal_address + 2 * i))));
    for (i = 0; i < 3; ++i)
    {
        result = xport_load_le32(plane + 4 * i);
        w_u32(linked + 784 + 4 * i, result);
    }
    return result;
}

uint32_t sub_800807F0(uint32_t actor, uint32_t delta)
{
    FUNCTION_MARKER(0x800807F0u, "RASHCDG.BIN");
    uint32_t linked = r_u32(actor + 856) != 0, step, x, z, other, value;
    if ((int8_t)r_s8((actor + 8)) < 2)
    {
        step = (uint32_t)((int32_t)(2608 * (uint32_t)sub_8001FC90((int32_t)r_u32(actor + 480), (int32_t)delta)) >> 16);
        if (r_u32(actor + 568) & 0x7FFu)
        {
            value = (uint32_t)(int32_t)r_s16(actor + 840);
            if ((int32_t)value >= 23)
                w_u16(actor + 840, (uint16_t)(value - 34));
            else if (!value)
                w_u16(actor + 840, (uint16_t)step);
            step = r_u16(actor + 840);
            x = r_u16(actor + 836) - step;
            z = r_u16(actor + 838) - step;
            w_u16(actor + 836, (uint16_t)x);
            w_u16(actor + 838, (uint16_t)z);
            if (linked)
            {
                other = r_u32(actor + 856);
                w_u16(other + 836, (uint16_t)(r_u16(other + 836) - step));
            }
        }
        else
        {
            x = r_u16(actor + 836);
            z = r_u16(actor + 838);
            if (r_u32(actor + 564) & 0x08000000u)
            {
                x -= step;
                z -= 170;
            }
            else
            {
                value = r_u32(actor + 616);
                x -= ((int32_t)(uint32_t)(value)) > 0 ? 22 : step;
                z -= ((int32_t)(uint32_t)(value)) < 0 ? 22 : step;
            }
            w_u16(actor + 836, (uint16_t)x);
            w_u16(actor + 838, (uint16_t)z);
            if (linked)
            {
                value = r_u32(actor + 636);
                if (((int32_t)(uint32_t)(value)) < 0)
                {
                    uint32_t rate = (uint32_t)(int32_t)r_s16(actor + 840);
                    if ((int32_t)rate >= 23)
                        w_u16(actor + 840, (uint16_t)(rate - 34));
                    else if (!rate)
                        w_u16(actor + 840, (uint16_t)step);
                    step = r_u16(actor + 840);
                }
                other = r_u32(actor + 856);
                w_u16(other + 836, (uint16_t)(r_u16(other + 836) - step));
                if (((int32_t)(uint32_t)(value)) >= 0)
                    w_u16(actor + 840, 0);
                /* This branch also normalizes the linked wheel phase */
                while (((int32_t)(uint32_t)((uint32_t)r_s16((r_u32(actor + 856) + 836)))) < -4096)
                {
                    other = r_u32(actor + 856);
                    w_u16(other + 836, (uint16_t)(r_u16(other + 836) + 4096));
                }
            }
        }
        value = (uint32_t)(int32_t)r_s16(actor + 836);
        while ((int32_t)value < -4096)
        {
            value = (uint32_t)(int32_t)(int16_t)(value + 4096);
            w_u16(actor + 836, (uint16_t)value);
        }
        value = (uint32_t)(int32_t)r_s16(actor + 838);
        while ((int32_t)value < -4096)
        {
            value = (uint32_t)(int32_t)(int16_t)(value + 4096);
            w_u16(actor + 838, (uint16_t)value);
        }
    }
    (void)sub_8008BA18(actor);
    if (linked)
    {
        other = r_u32(actor + 856) + 432;
        (void)sub_8003FA18(9, xport_guest_cptr(actor + 432, 18), xport_guest_ptr(other, 18));
        other = r_u32(actor + 856);
        value = r_u32(actor + 304) + r_u32(other + 304);
        (void)sub_8002EAD8(xport_guest_ptr(actor + 184, 12), xport_guest_ptr(actor + 432, 6), value, xport_guest_ptr(other + 184, 12));
        (void)sub_8008BA18(r_u32(actor + 856));
    }
    value = sub_80020018((uint32_t)r_s16((actor + 528)) << 4, (uint32_t)r_s16((actor + 532)) << 4);
    w_u32(actor + 292, value);
    x = (uint32_t)r_s16((0x8005624C + (((value & 4095) * 4) | 2))) << 4;
    value = 0x8005624C + 4 * (r_u32(actor + 292) & 4095);
    w_u32(actor + 296, x);
    value = (uint32_t)r_s16(value) << 4;
    w_u32(actor + 300, value);
    return value;
}

/* 8007FA4C: contact response, orientation and final position commit. */
uint32_t sub_8007FA4C(uint32_t actor)
{


    FUNCTION_MARKER(0x8007FA4Cu, "RASHCDG.BIN");
    uint32_t special = (r_u32(actor + 568) & 0x600u) != 0, distance = r_u32(actor + 260);
    uint32_t flags = r_u32(actor + 564), mode = r_u32(actor + 568), queried, allowed, reply = 0;
    uint32_t vector[3], cross[3], axis[3], i, dot, ratio, scale, linked, owner, x, y, z, correction;
    uint32_t denominator, numerator_positive, denominator_positive, sign, magnitude[3], maximum, bit, shift, sum, table, entry, expanded;
    uint32_t normalized[3], diagonal[3], direction[3], product[3];
    uint64_t squared;
    uint16_t average[3], captured[3];
    int use_actor_normal = 0, orientation_done = 0;
    w_u32(actor + 564, flags & 0xFFBFFFFFu);
    if (!(mode & 0x600u))
    {
        flags = r_u32(actor + 388);
        queried = flags & 1u;
        allowed = (mode & 0x80000Fu) == 0;
        if ((flags & 64u) && allowed)
            reply = queried && ((int32_t)(uint32_t)(distance)) < 0 ? sub_80075B08(actor, actor + 274) : sub_80075628(actor, actor + 274, r_u32(actor + 260));
        distance = 0;
        if (reply)
            goto finish;
        for (i = 0; i < 3; ++i)
            vector[i] = r_u32(actor + 280 + 4 * i) - r_u32(actor + 244 + 4 * i);
        dot = 0;
        for (i = 0; i < 3; ++i)
        {
            int64_t projection = (int64_t)(int32_t)vector[i] * ((int32_t)r_s16(actor + 268 + 2 * i) * 16);
            dot += (uint32_t)((uint64_t)projection >> 16);
        }
        if ((r_u32(actor + 564) & 0x02000000u) && allowed)
            reply = queried && ((int32_t)(uint32_t)(dot)) < 0 ? sub_80075B08(actor, actor + 268) : sub_80075628(actor, actor + 268, dot);
        if (reply)
            goto finish;
        for (i = 0; i < 3; ++i)
            captured[i] = r_u16(actor + 274 + 2 * i);
        for (i = 0; i < 3; ++i)
            w_u16(actor + 522 + 2 * i, captured[i]);
        for (i = 0; i < 3; ++i)
            average[i] = (uint16_t)((((int32_t)(uint32_t)((uint32_t)r_s16((actor + 268 + 2 * i)))) + ((int32_t)(uint32_t)((uint32_t)r_s16((actor + 522 + 2 * i))))) / 2);
        denominator = sub_8002E698(average, xport_guest_cptr(actor + 268, 6));
        numerator_positive = (int32_t)dot > 0;
        denominator_positive = (int32_t)denominator > 0;
        ratio = sub_80010028(numerator_positive ? dot : 0 - dot, denominator_positive ? denominator : 0 - denominator);
        if (numerator_positive != denominator_positive)
            ratio = 0 - ratio;
        for (i = 0; i < 3; ++i)
        {
            int64_t placement = (int64_t)((int32_t)(int16_t)average[i] * 16) * (int32_t)ratio;
            w_u32(actor + 784 + 4 * i, (uint32_t)((uint64_t)placement >> 16) + r_u32(actor + 244 + 4 * i));
        }
        for (i = 0; i < 3; ++i)
            vector[i] = r_u32(actor + 784 + 4 * i) - r_u32(actor + 504 + 4 * i);
        for (i = 0; i < 3; ++i)
        {
            sign = (uint32_t)((int32_t)vector[i] >> 31);
            magnitude[i] = (vector[i] + sign) ^ sign;
        }
        maximum = (int32_t)magnitude[1] < (int32_t)magnitude[0] ? magnitude[0] : magnitude[1];
        if ((int32_t)magnitude[2] >= (int32_t)maximum)
            maximum = magnitude[2];
        bit = 0;
        if (maximum)
        {
            xport_gte_write_data(30, maximum);
            bit = 31 - xport_gte_read_data(31);
        }
        if ((int32_t)bit < 21)
        {
            shift = (20 - bit) & 31;
            for (i = 0; i < 3; ++i)
                normalized[i] = vector[i] << shift;
        }
        else
        {
            shift = (bit - 20) & 31;
            for (i = 0; i < 3; ++i)
                normalized[i] = (uint32_t)((int32_t)vector[i] >> shift);
        }
        sum = 0;
        for (i = 0; i < 3; ++i)
        {
            int64_t component_squared = (int64_t)(int32_t)normalized[i] * (int32_t)normalized[i];
            sum += (uint32_t)((uint64_t)component_squared >> 16);
        }
        if ((int32_t)sum > 0)
        {
            xport_gte_write_data(30, sum);
            shift = xport_gte_read_data(31);
            table = r_u32(0x8005B560);
            shift = 22 - (shift & 0xFFFFFFFE);
            if ((int32_t)shift <= 0)
                shift = 0;
            entry = r_u16(table + 2 * (sum >> (shift & 31)));
            expanded = (entry >> 5) << (entry & 31);
            scale = (uint32_t)((int32_t)expanded >> ((shift >> 1) & 31));
            for (i = 0; i < 3; ++i)
            {
                int64_t component = (int64_t)(int32_t)normalized[i] * (int32_t)scale;
                vector[i] = (uint32_t)((uint64_t)component >> 16);
            }
        }
        w_u16(actor + 528, (uint16_t)((int32_t)vector[0] >> 4));
        scale = r_u32(actor + 308);
        for (i = 1; i < 3; ++i)
            w_u16(actor + 528 + 2 * i, (uint16_t)((int32_t)vector[i] >> 4));
        for (i = 0; i < 3; ++i)
        {
            int64_t placement = (int64_t)((int32_t)r_s16(actor + 528 + 2 * i) * 16) * (int32_t)scale;
            w_u32(actor + 504 + 4 * i, (uint32_t)((uint64_t)placement >> 16) + r_u32(actor + 504 + 4 * i));
        }
        linked = r_u32(actor + 856);
        if (linked && r_u32(actor + 1088))
        {
            for (i = 0; i < 3; ++i)
                average[i] = (uint16_t)((((int32_t)(uint32_t)((uint32_t)r_s16((linked + 814 + 2 * i)))) + ((int32_t)(uint32_t)((uint32_t)r_s16((actor + 522 + 2 * i))))) / 2);
            reply = sub_8002E698(average, xport_guest_ptr(linked + 814, 6));
            for (i = 0; i < 3; ++i)
                vector[i] = r_u32(linked + 184 + 4 * i) - r_u32(linked + 784 + 4 * i);
            dot = 0;
            for (i = 0; i < 3; ++i)
            {
                int64_t projection = (int64_t)(int32_t)vector[i] * ((int32_t)r_s16(linked + 814 + 2 * i) * 16);
                dot += (uint32_t)((uint64_t)projection >> 16);
            }
            numerator_positive = (int32_t)dot > 0;
            denominator_positive = (int32_t)reply > 0;
            ratio = sub_80010028(numerator_positive ? dot : 0 - dot, denominator_positive ? reply : 0 - reply);
            if (numerator_positive == denominator_positive)
                ratio = 0 - ratio;
            (void)sub_8002EAD8(xport_guest_cptr(linked + 184, 12), average, ratio, xport_guest_ptr(linked + 184, 12));
            for (i = 0; i < 3; ++i)
                vector[i] = r_u32(linked + 184 + 4 * i) - r_u32(actor + 504 + 4 * i);
            axis[1] = (uint32_t)(int32_t)r_s16(actor + 530) << 4;
            axis[0] = (uint32_t)(int32_t)r_s16(actor + 528) << 4;
            axis[2] = (uint32_t)(int32_t)r_s16(actor + 532) << 4;
            x = (uint32_t)sub_8001FC90((int32_t)axis[1], (int32_t)vector[2]);
            y = (uint32_t)sub_8001FC90((int32_t)axis[2], (int32_t)vector[1]);
            cross[0] = x - y;
            x = (uint32_t)sub_8001FC90((int32_t)axis[2], (int32_t)vector[0]);
            y = (uint32_t)sub_8001FC90((int32_t)axis[0], (int32_t)vector[2]);
            cross[1] = x - y;
            x = (uint32_t)sub_8001FC90((int32_t)axis[0], (int32_t)vector[1]);
            y = (uint32_t)sub_8001FC90((int32_t)axis[1], (int32_t)vector[0]);
            cross[2] = x - y;
            (void)sub_8002E14C(cross);
            use_actor_normal = 1;
            if (((int32_t)(uint32_t)(cross[1])) >= 9841)
            {
                for (i = 0; i < 3; ++i)
                    w_u16(actor + 522 + 2 * i, (uint16_t)((int32_t)cross[i] >> 4));
                diagonal[0] = (uint32_t)(int32_t)r_s16(actor + 522 + 0);
                diagonal[1] = (uint32_t)(int32_t)r_s16(actor + 522 + 2);
                xport_gte_write_control(0, diagonal[0]);
                diagonal[2] = (uint32_t)(int32_t)r_s16(actor + 522 + 4);
                xport_gte_write_control(2, diagonal[1]);
                xport_gte_write_control(4, diagonal[2]);
                direction[0] = (uint32_t)(int32_t)r_s16(actor + 528 + 0);
                direction[1] = (uint32_t)(int32_t)r_s16(actor + 528 + 2);
                direction[2] = (uint32_t)(int32_t)r_s16(actor + 528 + 4);
                xport_gte_write_data(9, direction[0]);
                xport_gte_write_data(10, direction[1]);
                xport_gte_write_data(11, direction[2]);
                xport_gte_execute(0x178000C);
                product[0] = xport_gte_read_data(9);
                product[1] = xport_gte_read_data(10);
                product[2] = xport_gte_read_data(11);
                w_u16(actor + 516 + 0, (uint16_t)product[0]);
                w_u16(actor + 516 + 2, (uint16_t)product[1]);
                w_u16(actor + 516 + 4, (uint16_t)product[2]);
                orientation_done = 1;
            }
        }
        if (!orientation_done)
        {
            diagonal[0] = use_actor_normal ? (uint32_t)(int32_t)r_s16(actor + 522) : (uint32_t)(int32_t)(int16_t)average[0];
            diagonal[1] = use_actor_normal ? (uint32_t)(int32_t)r_s16(actor + 524) : (uint32_t)(int32_t)(int16_t)average[1];
            xport_gte_write_control(0, diagonal[0]);
            diagonal[2] = use_actor_normal ? (uint32_t)(int32_t)r_s16(actor + 526) : (uint32_t)(int32_t)(int16_t)average[2];
            xport_gte_write_control(2, diagonal[1]);
            xport_gte_write_control(4, diagonal[2]);
            direction[0] = (uint32_t)(int32_t)r_s16(actor + 528 + 0);
            direction[1] = (uint32_t)(int32_t)r_s16(actor + 528 + 2);
            direction[2] = (uint32_t)(int32_t)r_s16(actor + 528 + 4);
            xport_gte_write_data(9, direction[0]);
            xport_gte_write_data(10, direction[1]);
            xport_gte_write_data(11, direction[2]);
            xport_gte_execute(0x178000C);
            product[0] = xport_gte_read_data(9);
            product[1] = xport_gte_read_data(10);
            product[2] = xport_gte_read_data(11);
            w_u16(actor + 516 + 0, (uint16_t)product[0]);
            w_u16(actor + 516 + 2, (uint16_t)product[1]);
            w_u16(actor + 516 + 4, (uint16_t)product[2]);
            for (i = 0; i < 3; ++i)
                direction[i] = (uint32_t)(int32_t)r_s16(actor + 516 + 2 * i);
            for (i = 0; i < 3; ++i)
                xport_gte_write_data(9 + i, direction[i]);
            xport_gte_execute(0xA00428);
            product[0] = xport_gte_read_data(25);
            product[1] = xport_gte_read_data(26);
            product[2] = xport_gte_read_data(27);
            squared = (uint64_t)product[0] + product[1] + product[2];
            /* Preserve the original signed ADD overflow boundary */
            if (squared > 0x7FFFFFFF)
                abort();
            sum = (uint32_t)squared;
            xport_gte_write_data(30, sum);
            shift = xport_gte_read_data(31);
            table = r_u32(0x8005B560);
            shift = 22 - (shift & 0xFFFFFFFE);
            if ((int32_t)shift <= 0)
                shift = 0;
            entry = r_u16(table + 2 * ((uint32_t)((int32_t)sum >> (shift & 31))));
            expanded = (entry >> 5) << (entry & 31);
            scale = (uint32_t)((int32_t)expanded >> ((shift >> 1) & 31));
            for (i = 0; i < 3; ++i)
            {
                uint32_t component = direction[i] * scale;
                w_u16(actor + 516 + 2 * i, (uint16_t)((int32_t)component >> 12));
            }
            diagonal[0] = (uint32_t)(int32_t)r_s16(actor + 528 + 0);
            diagonal[1] = (uint32_t)(int32_t)r_s16(actor + 528 + 2);
            xport_gte_write_control(0, diagonal[0]);
            diagonal[2] = (uint32_t)(int32_t)r_s16(actor + 528 + 4);
            xport_gte_write_control(2, diagonal[1]);
            xport_gte_write_control(4, diagonal[2]);
            direction[0] = (uint32_t)(int32_t)r_s16(actor + 516 + 0);
            direction[1] = (uint32_t)(int32_t)r_s16(actor + 516 + 2);
            direction[2] = (uint32_t)(int32_t)r_s16(actor + 516 + 4);
            xport_gte_write_data(9, direction[0]);
            xport_gte_write_data(10, direction[1]);
            xport_gte_write_data(11, direction[2]);
            xport_gte_execute(0x178000C);
            product[0] = xport_gte_read_data(9);
            product[1] = xport_gte_read_data(10);
            product[2] = xport_gte_read_data(11);
            w_u16(actor + 522 + 0, (uint16_t)product[0]);
            w_u16(actor + 522 + 2, (uint16_t)product[1]);
            w_u16(actor + 522 + 4, (uint16_t)product[2]);
        }
    }
finish:
    flags = r_u32(actor + 564);
    if (flags & 0x04000000u)
    {
        mode = r_u32(actor + 568);
        if (mode & 0x400u)
        {
            owner = r_u32(actor + 828);
            if (owner)
            {
                mode = ((int32_t)(uint32_t)((uint32_t)r_s16((owner + 50)) ^ r_u32(actor + 364))) < 0 ? mode & 0xDFFFFFFFu : mode | 0x20000000u;
                w_u32(actor + 568, mode);
            }
            w_u32(actor + 708, (r_u32(actor + 568) & 0x20000000u) ? 6553 : 0);
        }
        flags = r_u32(actor + 564);
        w_u32(actor + 828, 0);
        w_u32(actor + 564, flags & 0xFBFFFFFFu);
    }
    if (special)
    {
        reply = r_u32(actor + 768);
        if (((int32_t)(uint32_t)(distance)) < ((int32_t)(uint32_t)(reply)))
            distance = reply;
        flags = r_u32(actor + 564) & 0xFF7FFFFFu;
        w_u32(actor + 768, distance);
        w_u32(actor + 564, flags);
        if (((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(actor + 522, 6), xport_guest_ptr(actor + 450, 6)))) < 0)
            flags |= 0x800000u;
        w_u32(actor + 564, flags);
        return flags;
    }
    flags = r_u32(actor + 564);
    if (flags & 0x400000u)
    {
        x = r_u32(actor + 468);
        y = r_u32(actor + 472);
        z = r_u32(actor + 476);
        correction = ((r_u32(actor + 560) >> 27) ^ 1) & 1;
        reply = x - r_u32(actor + 796);
        w_u32(actor + 184, x);
        w_u32(actor + 188, y);
        w_u32(actor + 504, reply);
        w_u32(actor + 192, z);
        y -= r_u32(actor + 800);
        z -= r_u32(actor + 804);
        w_u32(actor + 508, y);
        w_u32(actor + 512, z);
        return sub_800374D4(actor, correction);
    }
    if (flags & 0x40000u)
    {
        owner = r_u32(actor + 832);
        (void)sub_8002ECB8(xport_guest_ptr(owner + 260, 6), xport_guest_ptr(owner + 272, 6), xport_guest_ptr(actor + 504, 12), r_u32(actor + 776), r_u32(actor + 780));
        for (i = 0; i < 3; ++i)
            w_u32(actor + 504 + 4 * i, r_u32(actor + 504 + 4 * i) + r_u32(r_u32(actor + 832) + 12 + 4 * i));
    }
    if (r_u32(actor + 568) & 0x600u)
    {
        (void)sub_80071D24(actor);
        w_u32(actor + 568, r_u32(actor + 568) | 0x08000000u);
        reply = r_u32(actor + 564) | 0x800000u;
        w_u32(actor + 564, reply);
        return reply;
    }
    x = r_u32(actor + 504) + r_u32(actor + 796);
    y = r_u32(actor + 800);
    z = r_u32(actor + 804);
    w_u32(actor + 184, x);
    reply = r_u32(actor + 508) + y;
    z += r_u32(actor + 512);
    w_u32(actor + 188, reply);
    w_u32(actor + 192, z);
    return reply;
}

/* 8008A998: player state flags and actor target selection, exact residual v0. */
uint32_t sub_8008A998(uint32_t player, uint32_t requested)
{
    FUNCTION_MARKER(0x8008A998u, "RASHCDG.BIN");
    uint32_t same = requested == r_u32(player + 544), value, flags, target;
    if (requested < 4 && r_u32(player + 540) != requested)
    {
        value = r_u32(player + 548);
        w_u32(player + 544, requested);
        target = r_u32(player + 568);
        w_u32(player + 548, (value & 0xffffffe7u) | (same ? 2u : 6u));
        if (r_u16(target + 172) >> 5)
            w_u32(player + 568, r_u32(0x8005B268 + 4 * (r_u16(player + 172) == 158)));
    }
    flags = r_u32(player + 552);
    w_u32(player + 772, requested >= 7);
    if (flags & 1)
        w_u32(player + 548, r_u32(player + 548) | 256u);
    flags = r_u32(player + 552) & 0xfffffffau;
    value = r_u32(player + 540);
    w_u32(player + 552, flags);
    value = value == 12 ? flags | 128u : flags & 0xfffffc7fu;
    w_u32(player + 552, value);
    w_u32(player + 540, requested);
    if (same)
    {
        value = r_u32(player + 552) & 64u;
        if (value)
        {
            value = r_u32(player + 552);
            w_u32(player + 772, 1);
            value |= 16u;
            w_u32(player + 552, value);
        }
    }
    return value;
}

/* 800BC7CC: reset event sequence, preserving prior last event 0 or 18. */
uint32_t sub_800BC7CC(uint32_t actor)
{
    FUNCTION_MARKER(0x800BC7CCu, "RASHCDG.BIN");
    uint32_t count = ((uint32_t)r_u8((actor + 946)) ^ 128u) - 128u;
    uint32_t flags = r_u32(actor + 560), body = r_u32(actor + 852);
    uint32_t last = r_u16(actor + 956 + 8 * (count - 1u)), type, value;
    uint8_t event[8] = {0};
    if (flags & 0x08000000u)
    {
        type = r_u16(body + 544);
        if (r_u16(0x800541D4 + 8 * type + 2) == 3)
        {
            type = r_u16(r_u32(0x8005AD4C) + 12 * (uint32_t)r_u8((body + 569)));
            (void)sub_800C4550(type, body, 2);
        }
    }
    (void)sub_800BCD10(actor);
    xport_store_le16(event, 2);
    xport_store_le16(event + 2, 224);
    (void)sub_800BCA68(event, 1, actor);
    value = 18;
    if (last == 0 || last == 18)
    {
        xport_store_le16(event, (uint16_t)last);
        xport_store_le16(event + 2, last == 18 ? r_u16(actor + 172) : 224);
        value = sub_800BCA68(event, 0, actor);
    }
    w_u16(actor + 944, 0);
    w_u32(actor + 908, 0);
    return value;
}

/* 80092C7C: mark actor stopped and commit the player reason/state. */
uint32_t sub_80092C7C(uint32_t actor, uint32_t reason)
{
    FUNCTION_MARKER(0x80092C7Cu, "RASHCDG.BIN");
    uint32_t flags = r_u32(actor + 560), descriptor = r_u32(actor + 1084), value, player;
    w_u32(actor + 560, flags | 0x08000000u);
    value = r_u32(descriptor + 40);
    w_u32(0x8005B230, 0);
    if (!value)
    {
        w_u8(descriptor + 39, (uint8_t)(reason == 9 ? 254 : 255));
        value = r_u32(r_u32(0x8005B2F8) + 16);
        w_u32(descriptor + 40, value);
        (void)sub_800BC7CC(actor);
        if ((uint32_t)r_u8((descriptor + 39)) == 255 && r_u32(r_u32(actor + 852) + 604) < 2)
        {
            (void)sub_8002090C(actor);
            w_u32(actor + 568, r_u32(actor + 568) | 0x08000000u);
        }
    }
    player = 0x800CD898 + 1132u * r_u16(actor + 172);
    if (reason == 10)
    {
        w_u32(player + 772, 1);
        player = 0x800CD898 + 1132u * r_u16(actor + 172);
        w_u32(player + 552, r_u32(player + 552) | 20u);
        return player;
    }
    value = r_u32(player + 552) & 4u;
    return value ? value : sub_8008A998(player, reason);
}

/* 8002F0F4: truncate each signed square independently before wrapping sum. */
uint32_t sub_8002F0F4(const uint32_t vector[3])
{
    uint32_t sum = 0;
    uint32_t axis;
    FUNCTION_MARKER(0x8002F0F4, "SLUS_010.53");
    for (axis = 0; axis < 3; ++axis)
    {
        int64_t value = ((int32_t)(uint32_t)(xport_load_le32((const uint8_t *)vector + 4u * axis)));
        sum += (uint32_t)((uint64_t)(value * value) >> 16);
    }
    return sum;
}

uint32_t sub_8002E548(const uint32_t vector[3])
{
    FUNCTION_MARKER(0x8002E548, "SLUS_010.53");
    return sub_8004CF74(sub_8002F0F4(vector)) << 2;
}

/* 8002ED94: local first result, then in-place second, then halfword copies. */
uint32_t sub_8002ED94(uint16_t left[3], uint16_t right[3], uint32_t angle)
{
    uint32_t table = 0x8005624C + 4 * (angle & 4095);
    uint32_t cosine, sine;
    uint16_t saved[3];

    FUNCTION_MARKER(0x8002ED94, "SLUS_010.53");
    cosine = (uint32_t)(int32_t)r_s16(table + 2) << 4;
    sine = (uint32_t)(int32_t)r_s16(table) << 4;
    (void)sub_8002EB78(left, right, saved, cosine, 0 - sine);
    (void)sub_8002EB78(left, right, right, sine, cosine);
    xport_store_le16(left, saved[0]);
    xport_store_le16(left + 1, saved[1]);
    xport_store_le16(left + 2, saved[2]);
    return saved[2];
}

uint32_t sub_8007F08C(uint32_t actor)
{
    FUNCTION_MARKER(0x8007F08Cu, "RASHCDG.BIN");
    uint32_t value = r_u32(actor + 560), linked = r_u32(actor + 856);
    value &= 0xff7fffffu;
    w_u32(actor + 560, value);
    if (linked)
    {
        value = r_u32(actor + 488);
        w_u32(linked + 488, value);
    }
    return value;
}

/* 800BFE58: participant counter; byte wraps, residual v0 does not. */
uint32_t sub_800BFE58(uint32_t actor, uint32_t kind, uint32_t side)
{
    FUNCTION_MARKER(0x800BFE58u, "RASHCDG.BIN");
    uint32_t count = r_u32(r_u32(0x8005B2F8) + 48);
    uint32_t index = r_u16(actor + 172), address, value;
    if (index >= count)
    {
        value = r_u32(actor + 1088);
        if (value)
            return value;
        index = r_u16(r_u32(actor + 856) + 172) + 2u;
    }
    address = 0x800D81D8 + 36u * index + 4u * side + kind + 24u;
    value = (uint32_t)r_u8(address) + 1u;
    w_u8(address, (uint8_t)value);
    return value;
}

/* 800BF51C: credit each matching recent interaction, including negative age. */
uint32_t sub_800BF51C(uint32_t actor)
{
    FUNCTION_MARKER(0x800BF51Cu, "RASHCDG.BIN");
    uint32_t slot = 0x800CD548, index, age, tag;
    for (index = 0; index < 4; ++index, slot += 12)
    {
        if (r_u32(slot + 4) != actor)
            continue;
        age = r_u32(r_u32(0x8005B2F8) + 16) - r_u32(slot + 8);
        if (((int32_t)(uint32_t)(age)) >= 240)
            continue;
        (void)sub_800BFE58(r_u32(slot), 1, 0);
        (void)sub_800BFE58(actor, 1, 1);
        tag = r_u16(r_u32(slot) + 172);
        if (tag > 1)
            tag = 1;
        w_u32(0x800D6198 + 224u * tag + 140, 1);
    }
    return 0;
}

uint32_t sub_8005C338(uint32_t actor, uint32_t output)
{
    FUNCTION_MARKER(0x8005C338u, "RASHCDG.BIN");
    static const uint32_t offsets[4] = {128, 152, 176, 200};
    uint32_t i;
    uint32_t value = 0;
    for (i = 0; i < 4; ++i)
    {
        value = r_u16(actor + offsets[i]);
        w_u16(output + 2 * i, (uint16_t)value);
    }
    return value;
}

uint32_t sub_8005C36C(uint32_t actor, uint32_t input)
{
    FUNCTION_MARKER(0x8005C36Cu, "RASHCDG.BIN");
    static const uint32_t offsets[4] = {128, 152, 176, 200};
    uint32_t i;
    uint32_t value = 0;
    for (i = 0; i < 4; ++i)
    {
        value = r_u16(input + 2 * i);
        w_u16(actor + offsets[i], (uint16_t)value);
    }
    return value;
}

uint32_t sub_80074FB4(int32_t limit, int32_t value, int32_t phase)
{
    FUNCTION_MARKER(0x80074FB4u, "RASHCDG.BIN");
    int32_t weight = (phase < 0 ? 0 : phase) + (phase > 65536 ? 65536 - phase : 0);
    int32_t first;
    int32_t second;
    first = (int32_t)sub_8001FC90(value, weight);
    if (first >= limit)
        return (uint32_t)limit;
    second = (int32_t)sub_8001FC90(limit, weight);
    return (uint32_t)(first >= second ? first : second);
}

uint32_t sub_8008CF74(uint32_t object)
{
    FUNCTION_MARKER(0x8008CF74u, "RASHCDG.BIN");
    static const uint32_t zero_offsets[] = {0x1BC, 0x1BE, 0x1BA, 0x1B6, 0x1B4, 0x1B2, 0x212, 0x210, 0x20E, 0x20A, 0x208, 0x206};
    static const uint32_t one_offsets[] = {0x1C0, 0x1B8, 0x1B0, 0x214, 0x20C, 0x204, 0x1C6};
    uint32_t i;
    for (i = 0; i < sizeof(zero_offsets) / sizeof(zero_offsets[0]); ++i)
        w_u16(object + zero_offsets[i], 0);
    for (i = 0; i < sizeof(one_offsets) / sizeof(one_offsets[0]); ++i)
        w_u16(object + one_offsets[i], 4096);
    w_u16(object + 0x1C2, 0);
    w_u16(object + 0x1C4, 0);
    return 4096;
}

uint32_t sub_8008DF74(uint32_t object)
{
    FUNCTION_MARKER(0x8008DF74u, "RASHCDG.BIN");
    uint32_t body = r_u32(object + 596);
    uint32_t i;
    uint32_t value;
    if (r_u8(object + 572) & 0x20)
        body = r_u32(body + 856);
    for (i = 0; i < 9; ++i)
    {
        value = r_u16(object + 432 + 2 * i);
        w_u16(object + 516 + 2 * i, (uint16_t)value);
    }
    (void)sub_8001E0B4(xport_guest_ptr(object + 328, 32), xport_guest_ptr(body + 328, 1), 32);
    (void)sub_8001E0B4(xport_guest_ptr(object + 360, 12), xport_guest_ptr(body + 360, 1), 12);
    w_u32(object + 492, r_u32(body + 492));
    w_u32(object + 496, r_u32(body + 496));
    (void)sub_8001E0B4(xport_guest_ptr(object + 372, 56), xport_guest_ptr(body + 372, 1), 56);
    w_u32(object + 428, r_u32(body + 428));
    sub_8003AF9C(object + 172, 1, 0);
    w_u32(object + 324, r_u32(body + 324));
    value = r_u16(body + 320);
    w_u16(object + 320, (uint16_t)value);
    return value;
}
