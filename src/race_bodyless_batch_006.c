#include "native_game_api.h"
#include "psx.h"
#include "race_bodyless_batch_006.h"

uint32_t sub_8002289C(uint8_t record[9], const uint32_t keys[3], const uint32_t pointers[3], uint32_t slot)
{
    FUNCTION_MARKER(0x8002289Cu, "SLUS_010.53");
    uint32_t key = xport_load_le16(record), value = xport_load_le32(keys), index = 0;
    const uint8_t *cursor = (const uint8_t *)keys;
    uint32_t pointer, item, address, context, mode, packed, result;
    while (((int32_t)(uint32_t)(key)) < ((int32_t)(uint32_t)(value)))
    {
        cursor += 4;
        value = xport_load_le32(cursor);
        ++index;
    }
    key = xport_load_le16(record);
    value = xport_load_le32((const uint8_t *)keys + (index << 2));
    if (key != value)
        index = 0;
    pointer = xport_load_le32((const uint8_t *)pointers + (index << 2));
    item = r_u32(pointer);
    address = 0x800d76d0 + (slot << 5) + (item << 3);
    value = r_u8(address + 7);
    record[8] = (uint8_t)value;
    value = r_u16(address + 4);
    xport_store_le16(record + 6, (uint16_t)value);
    context = r_u32(0x8005b2f8);
    mode = r_u32(context + 48) - 1;
    value = r_u8(0x800533b4 + ((slot + 2) << 2) + mode * 88 + 2);
    packed = (uint32_t)(((int32_t)(uint32_t)(item)) >> 1) + value;
    value = record[2];
    result = ((((packed & 16) << 4) + ((item & 1) << 7) + value + 96) << 6) | ((packed & 15) << 2) | 3;
    xport_store_le16(record + 4, (uint16_t)result);
    return result;
}

uint32_t sub_80033EA0(uint32_t flags, uint32_t output, const uint8_t record[9])
{
    FUNCTION_MARKER(0x80033EA0u, "SLUS_010.53");
    uint32_t mode = r_u32(flags + 8), value, nibble, result = 0;
    value = r_u8(output);
    nibble = value >> 4;
    if (!(mode & 64))
    {
        if (nibble >= 15)
        {
            nibble = 30;
            result = 1;
        }
        else if ((int8_t)record[8] < 0)
            nibble += 15;
    }
    value = r_u8(output);
    w_u8(output, (uint8_t)((nibble << 3) | (value & 15)));
    return result;
}

uint32_t sub_80022758(uint8_t record[9], const uint32_t keys[3], const uint32_t pointers[3], uint32_t slot)
{
    FUNCTION_MARKER(0x80022758u, "SLUS_010.53");
    uint32_t key = xport_load_le16(record), value = xport_load_le32(keys), context, mode;
    const uint8_t *cursor = (const uint8_t *)keys;
    uint32_t index = 0, pointer, item, address, table, group;
    uint32_t dividend, divisor, quotient, remainder, factor, product, packed, row, result;
    context = r_u32(0x8005b2f8);
    mode = r_u32(context + 48);
    while (((int32_t)(uint32_t)(key)) < ((int32_t)(uint32_t)(value)))
    {
        cursor += 4;
        value = xport_load_le32(cursor);
        ++index;
    }
    key = xport_load_le16(record);
    value = xport_load_le32((const uint8_t *)keys + (index << 2));
    if (key != value)
        index = 0;
    pointer = xport_load_le32((const uint8_t *)pointers + (index << 2));
    item = r_u32(pointer);
    address = 0x800d7710 + ((slot * 3 + item) << 3);
    value = r_u16(address + 4);
    xport_store_le16(record + 6, (uint16_t)value);
    group = (mode - 1) * 11;
    table = 0x80053254 + (slot << 3) + (group << 4);
    dividend = record[2];
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
    result = ((uint32_t)(((int32_t)(uint32_t)(packed)) >> 4)) & 63;
    xport_store_le16(record + 4, (uint16_t)(row | result));
    return result;
}

uint32_t sub_80033E7C(uint32_t output)
{
    FUNCTION_MARKER(0x80033E7Cu, "SLUS_010.53");
    return r_u8(output) | 0xf0;
}

uint32_t sub_80033E5C(uint32_t output)
{
    uint32_t value = r_u16(output + 2u);
    uint32_t result = value & 0xF0u;
    FUNCTION_MARKER(0x80033E5Cu, "SLUS_010.53");
    if (value & 15u)
        w_u16(output + 2u, (uint16_t)result);
    return result;
}

uint32_t sub_80033D70(uint32_t values[3], uint32_t auxiliary[3], uint32_t first_index, uint32_t second_index)
{
    uint8_t *first = (uint8_t *)values + (first_index << 2);
    uint8_t *second = (uint8_t *)values + (second_index << 2);
    uint32_t value, saved;
    FUNCTION_MARKER(0x80033D70u, "SLUS_010.53");
    value = xport_load_le32(second);
    saved = xport_load_le32(first);
    xport_store_le32(first, value);
    xport_store_le32(second, saved);
    first = (uint8_t *)auxiliary + (first_index << 2);
    second = (uint8_t *)auxiliary + (second_index << 2);
    value = xport_load_le32(second);
    saved = xport_load_le32(first);
    xport_store_le32(first, value);
    xport_store_le32(second, saved);
    return value;
}

uint32_t sub_80023DE4(void)
{
    FUNCTION_MARKER(0x80023DE4u, "SLUS_010.53");
    uint32_t record = r_u32(0x8005ae34u), state, table, count, key;
    uint32_t index = 0, offset = 0, cursor, value, handle, first, second, result;
    state = r_u32(record + 56);
    if (state == 1)
    {
        handle = r_u32(0x8005b4fcu);
        (void)(uint32_t)sub_80023C24(record + 8, handle, 0, 0);
    }
    else if (!state)
    {
        table = r_u32(0x8005b4f8u);
        count = r_u32(table + 12);
        if (((int32_t)(uint32_t)(count)) > 0)
        {
            key = r_u32(record + 72);
            do
            {
                cursor = r_u32(table + 20);
                value = r_u32(cursor + offset);
                if (value == key)
                    break;
                count = r_u32(table + 12);
                ++index;
                offset += 20;
            } while (((int32_t)(uint32_t)(index)) < ((int32_t)(uint32_t)(count)));
        }
        record = r_u32(0x8005ae34u);
        value = r_u32(record + 64);
        offset = ((index << 2) + index) << 2;
        table = r_u32(0x8005b4f8u);
        cursor = r_u32(table + 20);
        handle = r_u32(0x8005b4fcu);
        cursor += offset;
        if (((int32_t)(uint32_t)(value)) > 0)
        {
            first = r_u32(cursor + 4);
            second = r_u32(cursor + 8);
        }
        else
        {
            first = r_u32(cursor + 12);
            second = r_u32(cursor + 16);
        }
        (void)(uint32_t)sub_80023C24(record + 8, handle, first, second);
    }
    record = r_u32(0x8005ae34u);
    value = r_u32(record + 64);
    w_u32(record + 100, 0);
    result = (uint32_t)(((int32_t)(uint32_t)(value)) < 1) << 16;
    w_u32(record + 104, result);
    return result;
}

uint32_t sub_800243BC(uint32_t offset)
{
    FUNCTION_MARKER(0x800243BCu, "SLUS_010.53");
    uint32_t record = r_u32(0x8005AE34u);
    uint32_t value = r_u32(record + 20), limit = r_u32(record + 24), result;
    value += offset;
    w_u32(record + 12, value);
    result = limit < value;
    if (result)
        w_u32(record + 12, limit);
    return result;
}

uint32_t sub_8002F308(uint32_t source, uint32_t target, uint32_t kind, uint32_t state, uint32_t option)
{
    FUNCTION_MARKER(0x8002F308u, "SLUS_010.53");
    uint32_t pointer, value, special, context;
    w_u32(target + 568, source);
    (void)(uint32_t)sub_8001E100(target, 0, 172);
    w_u32(target + 304, 0x18000);
    w_u32(target + 308, 0x2000);
    w_u16(target + 320, 1);
    w_u32(target + 548, 256);
    w_u16(target + 172, (uint16_t)kind);
    w_u32(target + 544, state);
    w_u32(target + 540, state);
    w_u32(target + 180, 0);
    w_u32(target + 312, 0x1cccc);
    w_u32(target + 316, 0);
    w_u32(target + 552, 0);
    w_u32(target + 660, 0);
    w_u32(target + 584, 0);
    w_u32(target + 620, 0);
    w_u32(target + 616, 0);
    w_u32(target + 1116, 0x10000);
    if (option)
    {
        value = r_u32(target + 552);
        w_u32(target + 772, 1);
        w_u32(target + 552, value | 64);
    }
    else
    {
        pointer = r_u32(source + 1084);
        special = (r_u8(pointer) & 32) != 0;
        if (!special)
        {
            context = r_u32(0x8005b2f8);
            special = (r_u8(context + 4) & 1) != 0;
        }
        w_u32(target + 772, special);
        if (special)
        {
            w_u32(target + 540, 7);
            value = r_u32(target + 548) & 0xfffffeff;
        }
        else
            value = r_u32(target + 548) | 6;
        w_u32(target + 548, value);
    }
    (void)sub_8001E0B4(xport_guest_ptr(target + 328, 32), xport_guest_ptr(source + 328, 1), 32);
    (void)sub_8001E0B4(xport_guest_ptr(target + 360, 12), xport_guest_ptr(source + 360, 1), 12);
    (void)sub_8003AF9C(target + 172, 0, source);
    value = r_u32(source + 492);
    w_u32(target + 492, value);
    value = r_u32(source + 496);
    w_u32(target + 496, value);
    (void)sub_8001E0B4(xport_guest_ptr(target + 372, 56), xport_guest_ptr(source + 372, 1), 56);
    value = r_u32(source + 504);
    w_u32(target + 184, value);
    value = r_u32(source + 508);
    w_u32(target + 188, value);
    value = r_u32(source + 512);
    w_u32(target + 724, 0);
    w_u32(target + 720, 0);
    w_u32(target + 736, 0);
    w_u32(target + 732, 0);
    w_u32(target + 192, value);
    return value;
}

uint32_t sub_80018DC8(uint32_t mode)
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
    w_u32(0x8005B400, result);
    return result;
}

uint32_t sub_8002B83C(void)
{
    FUNCTION_MARKER(0x8002B83Cu, "SLUS_010.53");
    w_u32(0x800d8058, 80);
    w_u32(0x800d805c, 0x800538d0);
    w_u8(0x800d8061, 0);
    w_u8(0x800d8060, 0);
    w_u16(0x800d7fea, 2);
    w_u16(0x800d7fe8, 2);
    w_u16(0x800d7fee, 2);
    w_u16(0x800d7fec, 2);
    return 2;
}

void sub_8002201C(uint32_t value)
{
    FUNCTION_MARKER(0x8002201Cu, "SLUS_010.53");
    if (((int32_t)(uint32_t)(value)) >= 0 && value != r_u32(0x80053250))
    {
        sub_8004D1A4(value);
        w_u32(0x80053250, value);
    }
}

void sub_8004D1A4(uint32_t value)
{
    FUNCTION_MARKER(0x8004D1A4u, "SLUS_010.53");
    gte_write_h((uint16)value);
}

uint32_t sub_80022064(uint32_t x, uint32_t y)
{
    FUNCTION_MARKER(0x80022064u, "SLUS_010.53");
    uint32_t half_x = (uint32_t)(((int32_t)(uint32_t)(x)) >> 1), half_y = (uint32_t)(((int32_t)(uint32_t)(y)) >> 1);
    uint32_t result = (uint32_t)sub_8004D184(half_x, half_y);
    w_u32(0x8005B4E4, half_y);
    w_u32(0x8005B4E0, half_x);
    return result;
}

uint32_t sub_800248E4(uint32_t first, uint32_t second)
{
    FUNCTION_MARKER(0x800248E4u, "SLUS_010.53");
    (void)sub_80025038(first, second);
    return sub_800249C8();
}

uint32_t sub_80025038(uint32_t first, uint32_t second)
{
    FUNCTION_MARKER(0x80025038u, "SLUS_010.53");
    uint32_t index = r_u32(0x800d7f80), next, wrapped, count;
    w_u32(0x800d7e90 + (index << 3), first);
    index = r_u32(0x800d7f80);
    w_u32(0x800d7e90 + (index << 3) + 4, second);
    index = r_u32(0x800d7f80);
    next = index + 1;
    wrapped = ((int32_t)(uint32_t)(next)) < 30 ? next : 0;
    w_u32(0x800d7f80, next);
    count = r_u32(0x800d7f88);
    w_u32(0x800d7f80, wrapped);
    ++count;
    w_u32(0x800d7f88, count);
    return count;
}
