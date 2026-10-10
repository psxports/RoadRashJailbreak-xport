#include "native_game_api.h"
#include "psx.h"
#include "race_bodyless_batch_006.h"

static uint32_t bodyless_call(RRJMemory *m, RRJRaceLeafCall call, uint32_t target, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
    const uint32_t args[8] = {a0, a1, a2, a3, 0, 0, 0, 0};
    return call(m, target, args);
}

uint32_t sub_8002289C(uint32_t record, uint32_t keys, uint32_t pointers, uint32_t slot)
{
    FUNCTION_MARKER(0x8002289Cu, "SLUS_010.53");
    uint32_t key = r_u16(record), value = rrj_read32(keys), index = 0;
    uint32_t cursor = keys, pointer, item, address, context, mode, packed, result;
    while (rrj_s32(key) < rrj_s32(value))
    {
        cursor += 4;
        value = rrj_read32(cursor);
        ++index;
    }
    key = r_u16(record);
    value = rrj_read32(keys + (index << 2));
    if (key != value)
        index = 0;
    pointer = rrj_read32(pointers + (index << 2));
    item = rrj_read32(pointer);
    address = 0x800d76d0 + (slot << 5) + (item << 3);
    value = r_u8(address + 7);
    w_u8(record + 8, (uint8_t)value);
    value = r_u16(address + 4);
    w_u16(record + 6, (uint16_t)value);
    context = rrj_read32(0x8005b2f8);
    mode = rrj_read32(context + 48) - 1;
    value = r_u8(0x800533b4 + ((slot + 2) << 2) + mode * 88 + 2);
    packed = (uint32_t)(rrj_s32(item) >> 1) + value;
    value = r_u8(record + 2);
    result = ((((packed & 16) << 4) + ((item & 1) << 7) + value + 96) << 6) | ((packed & 15) << 2) | 3;
    w_u16(record + 4, (uint16_t)result);
    return result;
}

uint32_t sub_80033EA0(uint32_t flags, uint32_t output, uint32_t record)
{
    FUNCTION_MARKER(0x80033EA0u, "SLUS_010.53");
    uint32_t mode = rrj_read32(flags + 8), value, nibble, result = 0;
    value = r_u8(output);
    nibble = value >> 4;
    if (!(mode & 64))
    {
        if (nibble >= 15)
        {
            nibble = 30;
            result = 1;
        }
        else if (r_s8(record + 8) < 0)
            nibble += 15;
    }
    value = r_u8(output);
    w_u8(output, (uint8_t)((nibble << 3) | (value & 15)));
    return result;
}

uint32_t sub_80022758(uint32_t record, uint32_t keys, uint32_t pointers, uint32_t slot)
{
    FUNCTION_MARKER(0x80022758u, "SLUS_010.53");
    uint32_t key = r_u16(record), value = rrj_read32(keys), context, mode;
    uint32_t cursor = keys, index = 0, pointer, item, address, table, group;
    uint32_t dividend, divisor, quotient, remainder, factor, product, packed, row, result;
    context = rrj_read32(0x8005b2f8);
    mode = rrj_read32(context + 48);
    while (rrj_s32(key) < rrj_s32(value))
    {
        cursor += 4;
        value = rrj_read32(cursor);
        ++index;
    }
    key = r_u16(record);
    value = rrj_read32(keys + (index << 2));
    if (key != value)
        index = 0;
    pointer = rrj_read32(pointers + (index << 2));
    item = rrj_read32(pointer);
    address = 0x800d7710 + ((slot * 3 + item) << 3);
    value = r_u16(address + 4);
    w_u16(record + 6, (uint16_t)value);
    group = (mode - 1) * 11;
    table = 0x80053254 + (slot << 3) + (group << 4);
    dividend = r_u8(record + 2);
    divisor = r_u8(table + 4);
    quotient = divisor ? dividend / divisor : UINT32_MAX;
    remainder = divisor ? dividend % divisor : dividend;
    factor = r_u8(table + 6);
    product = remainder * factor;
    value = r_u8(0x800533b4 + (slot << 2) + (group << 3) + 2);
    row = (uint32_t)r_s16(table + 2);
    packed = ((item & 15) + (value & 15)) << 6;
    row = (row + quotient) << 6;
    value = (uint32_t)r_s16(table);
    packed += value + product;
    result = ((uint32_t)(rrj_s32(packed) >> 4)) & 63;
    w_u16(record + 4, (uint16_t)(row | result));
    return result;
}

uint32_t sub_80033E7C(uint32_t output)
{
    FUNCTION_MARKER(0x80033E7Cu, "SLUS_010.53");
    return r_u8(output) | 0xf0;
}

uint32_t sub_80033E5C(RRJNativeCallFrame *frame, uint32_t arguments[8])
{
    FUNCTION_MARKER(0x80033E5Cu, "SLUS_010.53");
    frame->secondary_result = r_u16(arguments[1] + 2);
    frame->return_value = frame->secondary_result & 0xf0;
    if (frame->secondary_result & 15)
        w_u16(arguments[1] + 2, (uint16_t)frame->return_value);
    return frame->return_value;
}

uint32_t sub_80033D70(uint32_t values, uint32_t auxiliary, uint32_t first_index, uint32_t second_index)
{
    FUNCTION_MARKER(0x80033D70u, "SLUS_010.53");
    uint32_t first = values + (first_index << 2), second = values + (second_index << 2);
    uint32_t value = rrj_read32(second), saved = rrj_read32(first);
    rrj_write32(first, value);
    rrj_write32(second, saved);
    first = auxiliary + (first_index << 2);
    second = auxiliary + (second_index << 2);
    value = rrj_read32(second);
    saved = rrj_read32(first);
    rrj_write32(first, value);
    rrj_write32(second, saved);
    return value;
}

uint32_t sub_80023DE4(void)
{
    FUNCTION_MARKER(0x80023DE4u, "SLUS_010.53");
    uint32_t record = rrj_read32(0x8005ae34u), state, table, count, key;
    uint32_t index = 0, offset = 0, cursor, value, handle, first, second, result;
    state = rrj_read32(record + 56);
    if (state == 1)
    {
        handle = rrj_read32(0x8005b4fcu);
        (void)(uint32_t)sub_80023C24(record + 8, handle, 0, 0);
    }
    else if (!state)
    {
        table = rrj_read32(0x8005b4f8u);
        count = rrj_read32(table + 12);
        if (rrj_s32(count) > 0)
        {
            key = rrj_read32(record + 72);
            do
            {
                cursor = rrj_read32(table + 20);
                value = rrj_read32(cursor + offset);
                if (value == key)
                    break;
                count = rrj_read32(table + 12);
                ++index;
                offset += 20;
            } while (rrj_s32(index) < rrj_s32(count));
        }
        record = rrj_read32(0x8005ae34u);
        value = rrj_read32(record + 64);
        offset = ((index << 2) + index) << 2;
        table = rrj_read32(0x8005b4f8u);
        cursor = rrj_read32(table + 20);
        handle = rrj_read32(0x8005b4fcu);
        cursor += offset;
        if (rrj_s32(value) > 0)
        {
            first = rrj_read32(cursor + 4);
            second = rrj_read32(cursor + 8);
        }
        else
        {
            first = rrj_read32(cursor + 12);
            second = rrj_read32(cursor + 16);
        }
        (void)(uint32_t)sub_80023C24(record + 8, handle, first, second);
    }
    record = rrj_read32(0x8005ae34u);
    value = rrj_read32(record + 64);
    rrj_write32(record + 100, 0);
    result = (uint32_t)(rrj_s32(value) < 1) << 16;
    rrj_write32(record + 104, result);
    return result;
}

uint32_t sub_800243BC(uint32_t offset)
{
    FUNCTION_MARKER(0x800243BCu, "SLUS_010.53");
    uint32_t record = rrj_read32(0x8005AE34u);
    uint32_t value = rrj_read32(record + 20), limit = rrj_read32(record + 24), result;
    value += offset;
    rrj_write32(record + 12, value);
    result = limit < value;
    if (result)
        rrj_write32(record + 12, limit);
    return result;
}

uint32_t sub_8002F308(uint32_t source, uint32_t target, uint32_t kind, uint32_t state, uint32_t incoming_sp)
{
    FUNCTION_MARKER(0x8002F308u, "SLUS_010.53");
    uint32_t option, pointer, value, special, context;
    rrj_write32(target + 568, source);
    option = rrj_read32(incoming_sp + 16);
    (void)(uint32_t)sub_8001E100(target, 0, 172);
    rrj_write32(target + 304, 0x18000);
    rrj_write32(target + 308, 0x2000);
    w_u16(target + 320, 1);
    rrj_write32(target + 548, 256);
    w_u16(target + 172, (uint16_t)kind);
    rrj_write32(target + 544, state);
    rrj_write32(target + 540, state);
    rrj_write32(target + 180, 0);
    rrj_write32(target + 312, 0x1cccc);
    rrj_write32(target + 316, 0);
    rrj_write32(target + 552, 0);
    rrj_write32(target + 660, 0);
    rrj_write32(target + 584, 0);
    rrj_write32(target + 620, 0);
    rrj_write32(target + 616, 0);
    rrj_write32(target + 1116, 0x10000);
    if (option)
    {
        value = rrj_read32(target + 552);
        rrj_write32(target + 772, 1);
        rrj_write32(target + 552, value | 64);
    }
    else
    {
        pointer = rrj_read32(source + 1084);
        special = (r_u8(pointer) & 32) != 0;
        if (!special)
        {
            context = rrj_read32(0x8005b2f8);
            special = (r_u8(context + 4) & 1) != 0;
        }
        rrj_write32(target + 772, special);
        if (special)
        {
            rrj_write32(target + 540, 7);
            value = rrj_read32(target + 548) & 0xfffffeff;
        }
        else
            value = rrj_read32(target + 548) | 6;
        rrj_write32(target + 548, value);
    }
    (void)(uint32_t)sub_8001E0B4(target + 328, source + 328, 32);
    (void)(uint32_t)sub_8001E0B4(target + 360, source + 360, 12);
    (void)sub_8003AF9C(target + 172, 0, source);
    value = rrj_read32(source + 492);
    rrj_write32(target + 492, value);
    value = rrj_read32(source + 496);
    rrj_write32(target + 496, value);
    (void)(uint32_t)sub_8001E0B4(target + 372, source + 372, 56);
    value = rrj_read32(source + 504);
    rrj_write32(target + 184, value);
    value = rrj_read32(source + 508);
    rrj_write32(target + 188, value);
    value = rrj_read32(source + 512);
    rrj_write32(target + 724, 0);
    rrj_write32(target + 720, 0);
    rrj_write32(target + 736, 0);
    rrj_write32(target + 732, 0);
    rrj_write32(target + 192, value);
    return value;
}

uint32_t sub_80018DC8(uint32_t mode, uint32_t runtime_gp)
{
    FUNCTION_MARKER(0x80018DC8u, "SLUS_010.53");
    uint32_t result;
    switch (mode)
    {
        case 1:
            result = 21;
            break;
        case 2:
            result = 22;
            break;
        case 4:
            result = 23;
            break;
        case 5:
            result = 24;
            break;
        default:
            result = UINT32_MAX;
            break;
    }
    rrj_write32(runtime_gp + 0x774, result);
    return result;
}

uint32_t sub_8002B83C(void)
{
    FUNCTION_MARKER(0x8002B83Cu, "SLUS_010.53");
    rrj_write32(0x800d8058, 80);
    rrj_write32(0x800d805c, 0x800538d0);
    w_u8(0x800d8061, 0);
    w_u8(0x800d8060, 0);
    w_u16(0x800d7fea, 2);
    w_u16(0x800d7fe8, 2);
    w_u16(0x800d7fee, 2);
    w_u16(0x800d7fec, 2);
    return 2;
}

uint32_t sub_8002201C(uint32_t value, uint32_t incoming_v0)
{
    FUNCTION_MARKER(0x8002201Cu, "SLUS_010.53");
    uint32_t result = incoming_v0;
    if (rrj_s32(value) >= 0)
    {
        result = rrj_read32(0x80053250);
        if (value != result)
        {
            result = sub_8004D1A4(value, result);
            rrj_write32(0x80053250, value);
        }
    }
    return result;
}

uint32_t sub_8004D1A4(uint32_t value, uint32_t incoming_v0)
{
    FUNCTION_MARKER(0x8004D1A4u, "SLUS_010.53");
    gte_write_h((uint16)value);
    return incoming_v0;
}

uint32_t sub_80022064(uint32_t x, uint32_t y, uint32_t runtime_gp)
{
    FUNCTION_MARKER(0x80022064u, "SLUS_010.53");
    uint32_t half_x = (uint32_t)(rrj_s32(x) >> 1), half_y = (uint32_t)(rrj_s32(y) >> 1);
    uint32_t result = (uint32_t)sub_8004D184(half_x, half_y);
    rrj_write32(runtime_gp + 0x858, half_y);
    rrj_write32(runtime_gp + 0x854, half_x);
    return result;
}

uint32_t sub_800248E4(uint32_t arguments[8], RRJBodylessRegistersCall call)
{
    FUNCTION_MARKER(0x800248E4u, "SLUS_010.53");
    (void)(uint32_t)sub_80025038(arguments[0], arguments[1]);
    return call(rrj_host_context(), 0x800249c8, arguments);
}

uint32_t sub_80025038(uint32_t first, uint32_t second)
{
    FUNCTION_MARKER(0x80025038u, "SLUS_010.53");
    uint32_t index = rrj_read32(0x800d7f80), next, wrapped, count;
    rrj_write32(0x800d7e90 + (index << 3), first);
    index = rrj_read32(0x800d7f80);
    rrj_write32(0x800d7e90 + (index << 3) + 4, second);
    index = rrj_read32(0x800d7f80);
    next = index + 1;
    wrapped = rrj_s32(next) < 30 ? next : 0;
    rrj_write32(0x800d7f80, next);
    count = rrj_read32(0x800d7f88);
    rrj_write32(0x800d7f80, wrapped);
    ++count;
    rrj_write32(0x800d7f88, count);
    return count;
}
