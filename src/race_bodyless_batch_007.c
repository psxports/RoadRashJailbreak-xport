#include "rrj_debug.h"
#include <stdio.h>
#include <stdlib.h>
#include "native_game_api.h"
#include "race_pause.h"
#include "psx.h"
#include "race_bodyless_batch_007.h"
#include "race_bodyless_batch_003.h"
#include "sdk_build_stubs.h"

typedef uint32_t (*RRJStreamEventFunction)(uint32_t state);

static RRJStreamEventFunction stream_event_function(uint32_t address)
{
    switch (address)
    {
        case 0x80024F78u:
            return sub_80024F78;
        default:
            RRJ_DEBUG_FPRINTF(stderr, "RRJ: unresolved stream event function %08X\n", address);
            abort();
    }
}

uint32_t sub_800249C8(void)
{
    FUNCTION_MARKER(0x800249C8u, "SLUS_010.53");
    uint32_t result = r_u32(0x800cd694) < 2;
    uint32_t output;
    if (result)
        return result;
    if (!(uint32_t)sub_80020F50())
        return 0x800d0000;
    result = r_u8(0x800d75a0);
    if (!result)
        return result;
    result = r_u32(0x800d7588);
    if (!result)
        return result;
    result = sub_80025098(&output);
    w_u32(0x800cd6a0, result);
    w_u32(0x800cd690, 0);
    (void)sub_80024AA0(output);
    return sub_80020F9C();
}

uint32_t sub_80020F50(void)
{
    FUNCTION_MARKER(0x80020F50u, "SLUS_010.53");
    if (r_u32(0x800d7564))
        return 0;
    return r_u32(0x800d7578) == 0;
}

uint32_t sub_80025098(uint32_t *output)
{
    uint32_t result;
    FUNCTION_MARKER(0x80025098u, "SLUS_010.53");
    w_u32(0x80053658, 1);
    result = sub_800250C8(output);
    w_u32(0x80053658, 0);
    return result;
}

uint32_t sub_800250C8(uint32_t *output)
{
    uint32_t index, next, wrapped, flag, count, item, second;
    FUNCTION_MARKER(0x800250C8u, "SLUS_010.53");
    count = r_u32(0x800D7F88);
    if (((int32_t)(uint32_t)(count)) <= 0)
    {
        xport_store_le32(output, 0);
        return UINT32_MAX;
    }
    index = r_u32(0x800D7F84);
    next = index + 1u;
    wrapped = ((int32_t)(uint32_t)(next)) < 30 ? next : 0;
    w_u32(0x800D7F84, next);
    flag = r_u32(0x80053658);
    w_u32(0x800D7F84, wrapped);
    if (!flag)
        (void)sub_80043DA4();
    count = r_u32(0x800D7F88);
    flag = r_u32(0x80053658);
    w_u32(0x800D7F88, count - 1u);
    if (!flag)
        (void)sub_80043DB4();
    item = 0x800D7E90u + (index << 3);
    second = r_u32(item + 4);
    xport_store_le32(output, second);
    return r_u32(item);
}

uint32_t sub_80024AA0(uint32_t pointer)
{
    FUNCTION_MARKER(0x80024AA0u, "SLUS_010.53");
    uint32_t result = sub_80020F50();
    if (result)
    {
        (void)sub_80020F7C(pointer, 0x2000u, 0);
        result = sub_80020F7C(pointer + 0x2000u, 0x2000u, 1);
    }
    return result;
}

uint32_t sub_80020F7C(uint32_t pointer, uint32_t byte_count, uint32_t slot)
{
    FUNCTION_MARKER(0x80020F7Cu, "SLUS_010.53");
    uint32_t record = 0x800D7548u + ((slot << 2) + slot) * 4u;
    w_u32(record + 28u, pointer);
    return record;
}

uint32_t sub_80020F9C(void)
{
    FUNCTION_MARKER(0x80020F9Cu, "SLUS_010.53");
    uint32_t result = r_u8(0x800d75a0);
    if (result)
    {
        result = r_u32(0x800d7564);
        if (result)
        {
            result = r_u32(0x800d7588);
            if (result)
            {
                w_u32(0x800d7554, 0);
                result = sub_800214D0();
            }
        }
    }
    return result;
}

uint32_t sub_800214D0(void)
{
    FUNCTION_MARKER(0x800214D0u, "SLUS_010.53");
    uint32_t parameters[6];
    uint32_t index = r_u32(0x800d7554), count, capacity, offset, mode = 0;
    uint32_t size, item, sum, pointer;
    w_u8(0x800d75a0, 0);
    if (!index)
    {
        if (!r_u32(0x800d7564))
            return 0;
        if (!r_u32(0x800d7588))
            return 0;
    }
    count = r_u32(0x800d7588);
    capacity = r_u32(0x800d7584);
    if (count == capacity)
    {
        (void)sub_8002167C();
        w_u8(0x800d754b, 1);
    }
    if (!index)
    {
        count = r_u32(0x800d7588);
        w_u32(0x800d7588, count - 1u);
    }
    offset = r_u32(0x800d758c);
    if (offset)
    {
        capacity = r_u32(0x800d7584);
        size = r_u32(0x800d7580);
        mode = offset == (capacity - 1u) * size ? 1 : 2;
    }
    item = 0x800d7548u + ((index << 2) + index) * 4u;
    (void)sub_800212B0(r_u32(item + 28), mode);
    sum = r_u32(item + 32);
    offset = r_u32(0x800d758c);
    size = r_u32(0x800d7580);
    sum += offset;
    parameters[0] = sum;
    parameters[1] = size;
    pointer = r_u32(item + 28);
    mode = r_u8(0x800d7548);
    parameters[2] = pointer;
    if (mode && r_u32(0x800d7590) == offset)
    {
        parameters[0] = sum + 64u;
        parameters[1] = size - 64u;
        parameters[2] = pointer + 64u;
    }
    parameters[3] = 0x80021884u;
    parameters[4] = 0;
    parameters[5] = 1;
    return sub_8001E22C(parameters, 0);
}

uint32_t sub_800212B0(uint32_t pointer, uint32_t mode)
{
    FUNCTION_MARKER(0x800212B0u, "SLUS_010.53");
    uint32_t address, result = 0x800d0000;
    if (!mode)
        w_u8(pointer + 1, r_u8(pointer + 1) | 4);
    if (mode == 1)
    {
        address = pointer + r_u32(0x800d7580) - 15;
        result = r_u8(address) | 1;
        w_u8(address, (uint8_t)result);
    }
    return result;
}

uint32_t sub_80021884(void)
{
    FUNCTION_MARKER(0x80021884u, "SLUS_010.53");
    uint32_t index = r_u32(0x800d7554), limit, count = 0, target, size, capacity, offset, flag, cursor;
    cursor = 0x800d7548;
    limit = r_u32(0x800d754c);
    ++index;
    w_u32(0x800d7554, index);
    if (index < limit)
        return sub_800214D0();
    w_u32(0x800d7554, 0);
    if (limit)
    {
        do
        {
            w_u32(cursor + 28, 0);
            limit = r_u32(0x800d754c);
            ++count;
            cursor += 20;
        } while (count < limit);
    }
    target = r_u32(0x8005B4C4u);
    if (target)
    {
        (void)stream_event_function(target)(3);
    }
    size = r_u32(0x800d7580);
    capacity = r_u32(0x800d7584);
    offset = r_u32(0x800d758c) + size;
    capacity *= size;
    w_u32(0x800d758c, offset);
    if (offset == capacity)
        w_u32(0x800d758c, 0);
    flag = r_u8(0x800d754b);
    w_u8(0x800d75a0, 1);
    if (flag)
    {
        offset = r_u32(0x800d758c);
        w_u32(0x800d759c, 0);
        w_u32(0x800d7590, offset);
        (void)sub_80021644();
        w_u8(0x800d754b, 0);
    }
    if (r_u32(0x800d7564) && r_u32(0x800d7588))
        return sub_800214D0();
    return 0;
}

uint32_t sub_80024F78(uint32_t state)
{
    FUNCTION_MARKER(0x80024F78u, "SLUS_010.53");
    uint32_t result, output, handle;
    if (((int32_t)(uint32_t)(state)) < 3)
        return 1;
    if (((int32_t)(uint32_t)(state)) >= 5)
        return 0x800d0000;
    handle = r_u32(0x800cd6a0);
    if (((int32_t)(uint32_t)(handle)) >= 0)
    {
        (void)sub_80030FA0(handle);
        w_u32(0x800cd6a0, UINT32_MAX);
    }
    result = sub_80025098(&output);
    w_u32(0x800cd6a0, result);
    if (((int32_t)(uint32_t)(result)) < 0)
    {
        w_u32(0x800cd690, 1);
        return 1;
    }
    result = sub_80024AA0(output);
    w_u32(0x800cd690, 0);
    return result;
}

uint32_t sub_80030FA0(uint32_t index)
{
    FUNCTION_MARKER(0x80030FA0u, "SLUS_010.53");
    uint32_t result;
    w_u32(0x800541d0, 1);
    result = sub_80030FD0(index);
    w_u32(0x800541d0, 0);
    return result;
}

uint32_t sub_8002169C(void)
{
    FUNCTION_MARKER(0x8002169Cu, "SLUS_010.53");
    uint32_t timer, old_tick, tick, busy, active, progress, period, count, capacity;
    uint32_t queued = 0, state, target, size, cursor, stride, product, base, delay;
    (void)sub_8002167C();
    timer = r_u32(0x8005b2f8);
    old_tick = r_u32(0x8005B4B0u);
    tick = r_u32(timer + 12);
    busy = r_u8(0x800d75ac);
    w_u32(0x8005B4B4u, tick - old_tick);
    w_u32(0x8005B4B0u, tick);
    if (busy)
        return 1;
    active = r_u8(0x800d7548);
    w_u8(0x800d75ac, 1);
    if (active)
    {
        progress = r_u32(0x800d759c);
        period = r_u32(0x800d7594);
        ++progress;
        w_u32(0x800d759c, progress);
        if (progress == period)
        {
            count = r_u32(0x800d7588);
            capacity = r_u32(0x800d7584);
            if (count < capacity)
                w_u32(0x800d7588, count + 1);
            w_u32(0x800d759c, 0);
            queued = 1;
        }
        if (r_u8(0x800d75a0) && r_u32(0x800d7588))
        {
            state = r_u32(0x800d7564);
            if (!state)
            {
                target = r_u32(0x8005B4C4u);
                if (target)
                {
                    (void)stream_event_function(target)(3);
                }
                state = r_u32(0x800d7564);
                if (!state)
                {
                    capacity = r_u32(0x800d7584);
                    count = r_u32(0x800d7588);
                    if (count >= capacity - 1)
                    {
                        target = r_u32(0x8005B4C4u);
                        if (target)
                        {
                            (void)stream_event_function(target)(4);
                        }
                    }
                    state = r_u32(0x800d7564);
                }
            }
            if (state)
            {
                w_u32(0x800d7554, 0);
                (void)sub_800214D0();
            }
        }
        capacity = r_u32(0x800d7584);
        size = r_u32(0x800d7580);
        product = capacity * size;
        cursor = r_u32(0x800d7590);
        stride = r_u32(0x800d7598);
        cursor += stride;
        w_u32(0x800d7590, cursor);
        if (cursor == product)
            w_u32(0x800d7590, 0);
        base = r_u32(0x800d7568);
        (void)sub_80050C58(base + r_u32(0x800d7590));
        (void)sub_80050B18(1);
        timer = r_u32(0x8005b2f8);
        delay = r_u32(0x800d75a4);
        tick = r_u32(timer + 12);
        w_u32(0x800d75a8, tick + delay);
        if (queued)
        {
            target = r_u32(0x8005B4C4u);
            if (target)
            {
                (void)stream_event_function(target)(5);
            }
        }
    }
    w_u8(0x800d75ac, 0);
    return 0x800d0000;
}

uint32_t sub_8003BFE8(uint32_t actor)
{
    uint32_t result, root, nearest, distance, best, header, index, entry, word;
    uint32_t route, segment, piece, progress, found, table, tag, bit, threshold, mask;
    uint32_t record[8] = {0}, position[3] = {0}, projected[3];
    uint16_t direction[3];
    FUNCTION_MARKER(0x8003BFE8, "SLUS_010.53");
    result = r_u16(actor + 362);
    if (result)
        return result;
    result = r_u32(actor + 372);
    if (!result)
        return result;
    result = r_u32(actor + 388) & 1;
    if (!result)
        return result;
    root = r_u32(actor + 328);
    record[0] = root;
    distance = sub_8003A5F4(actor + 360, &nearest, 0);
    if (nearest == UINT32_MAX)
        return 0x310000;
    if ((int32_t)distance > 0x31FFFF)
        return 1;
    piece = r_u32(actor + 340);
    (void)sub_8002EAD8(xport_guest_cptr(piece + 20, 12), xport_guest_cptr(piece + 14, 6), r_u32(actor + 348), projected);
    best = sub_8001FCB0((uint32_t)((int32_t)(r_u32(actor + 184) - projected[0]) >> 16),
        (uint32_t)((int32_t)(r_u32(actor + 188) - projected[1]) >> 16),
        (uint32_t)((int32_t)(r_u32(actor + 192) - projected[2]) >> 16));
    if (r_s16(root + 16) != 1)
        return 1;
    result = r_u32(root + 12);
    if (result)
        return result;
    header = sub_80039AFC(r_u32(root));
    if (!header)
        return 0;
    result = (uint32_t)r_s16(header + 2);
    if ((int32_t)result <= 0)
        return result;
    /* Repair the original uninitialized direction with the actor's current direction */
    position[1] = r_u32(actor + 364);
    index = 0;
    do
    {
        entry = header + 8 + 24 * index;
        word = r_u32(actor + 360);
        if (!(word >> 16) && (word & 0xFFFF) == (uint32_t)r_s16(entry + 4))
            goto next_road;
        route = sub_80039C90(root, (uint32_t)r_s16(entry + 4));
        if (!route)
            return 0;
        segment = r_u32(root + 48) + 28 * (uint32_t)r_s16(route + 20);
        if (r_s16(entry + 2) > 0)
        {
            piece = r_u32(root + 52) + 52 * (uint32_t)r_s16(segment + 8);
            progress = 0;
        }
        else
        {
            piece = r_u32(root + 52) + 52 * ((uint32_t)r_s16(segment + 8) + (uint32_t)r_s16(segment + 10)) - 52;
            progress = r_u32(piece + 32);
        }
        record[1] = route;
        record[2] = segment;
        record[3] = piece;
        record[5] = progress;
        piece = sub_80036B14(actor + 172, record, actor + 184);
        record[4] = sub_800B6AAC(xport_guest_cptr(actor + 184, 12), xport_guest_cptr(piece + 2, 6), xport_guest_cptr(piece + 20, 12));
        record[5] = sub_800B6AAC(xport_guest_cptr(actor + 184, 12), xport_guest_cptr(piece + 14, 6), xport_guest_cptr(piece + 20, 12));
        direction[0] = r_u16(actor + 450);
        direction[1] = r_u16(actor + 452);
        direction[2] = r_u16(actor + 454);
        (void)sub_8003662C(direction, record, position);
        (void)sub_8002EAD8(xport_guest_cptr(piece + 20, 12), xport_guest_cptr(piece + 14, 6), record[5], projected);
        distance = sub_8001FCB0((uint32_t)((int32_t)(r_u32(actor + 184) - projected[0]) >> 16),
            (uint32_t)((int32_t)(r_u32(actor + 188) - projected[1]) >> 16),
            (uint32_t)((int32_t)(r_u32(actor + 192) - projected[2]) >> 16));
        if ((int32_t)distance >= (int32_t)best)
            goto next_road;
        word = r_u32(actor + 360);
        if ((word >> 16) || (position[0] >> 16) || word == position[0])
            goto next_road;
        (void)sub_8001E0B4(xport_guest_ptr(actor + 328, 32), record, 32);
        (void)sub_8001E0B4(xport_guest_ptr(actor + 360, 12), position, 12);
        (void)sub_8003DE28(actor, 1, 0, UINT32_MAX);
        (void)sub_8003DF54(actor, 1, UINT32_MAX);
        found = sub_8003F3B4(nearest);
        if (found)
            w_u32(actor + 428, found);
        table = r_u32(actor + 428);
        if (table)
        {
            tag = r_u16(actor + 172);
            if ((tag >> 5) < 2)
            {
                bit = tag & 31;
                threshold = r_u32(0x8005B1F8);
                if ((int32_t)bit >= (int32_t)threshold)
                    bit -= threshold;
                mask = 1u << (bit & 31);
                result = r_u16(table + 118);
                w_u16(table + 118, found ? result | mask : result & ~mask);
            }
        }
        sub_8003AF9C(actor + 172, 1, 0);
        result = sub_8003B61C(actor + 172);
        w_u32(actor + 324, result);
        return result;
next_road:
        ++index;
    } while ((int32_t)index < r_s16(header + 2));
    return index << 1;
}

void sub_800102A4(uint32_t destination, uint32_t packed)
{
    FUNCTION_MARKER(0x800102A4u, "SLUS_010.53");
    uint32_t end = destination + 30, index, nibble, output, count, words;
    for (index = 0; index < 16; ++index)
    {
        nibble = (r_u8(packed + (index >> 1)) >> (4 * ((index + 1) & 1))) & 15;
        if (nibble)
        {
            output = end - (nibble << 1);
            count = nibble + 1;
            if (output & 3)
            {
                w_u16(output, 0);
                output += 2;
                --count;
            }
            words = count >> 1;
            while (words)
            {
                --words;
                w_u32(output + (words << 2), 0);
            }
        }
        end += 32;
    }
}

uint32_t sub_8001A0C0(uint32_t record, uint32_t identifier, uint32_t owner_token)
{
    uint32_t index, entry, state, timer, bound, channel = 0, context_offset = 0, context;
    uint32_t slot, table, row, count, limit, old_handle, token, first, second;
    FUNCTION_MARKER(0x8001A0C0, "SLUS_010.53");
    index = (uint32_t)sub_8001A2E4(identifier);
    if (((int32_t)(uint32_t)(index)) < 0)
        return 2;
    entry = 0x800d6aa0 + (index << 5);
    state = r_u32(entry + 8);
    if (state == 3)
    {
        if ((identifier & 255) != 6 && (identifier & 255) != 22)
            return 2;
        timer = r_u32(0x8005b2f8);
        bound = r_u32(timer + 48);
        if (bound)
        {
            do
            {
                context = r_u32(0x8005B40Cu) + context_offset;
                slot = r_u32(context + 36);
                table = r_u32(0x8005B410u + (channel << 2));
                row = table + 44 * slot;
                count = r_u32(context + 40);
                limit = slot + count;
                while (((int32_t)(uint32_t)(slot)) < ((int32_t)(uint32_t)(limit)))
                {
                    timer = r_u32(0x8005b2f8);
                    state = r_u16(row);
                    bound = r_u32(timer + 48);
                    if (state < bound)
                    {
                        state = r_u32(row + 8);
                        if (state == 2)
                        {
                            (void)sub_80017814(slot, channel);
                        }
                    }
                    ++slot;
                    row += 44;
                }
                timer = r_u32(0x8005b2f8);
                bound = r_u32(timer + 48);
                ++channel;
                context_offset += 72;
            } while (channel < bound);
        }
    }
    old_handle = r_u32(entry);
    w_u32(entry + 8, 1);
    if (((int32_t)(uint32_t)(old_handle)) >= 0)
        (void)sub_8001EE28(old_handle);
    (void)sub_8001E0B4(xport_guest_ptr(r_u32(entry + 28), 56), xport_guest_ptr(record + 8, 1), 56);
    token = owner_token;
    token |= 0x80000000 | (index << 16);
    context = r_u32(entry + 28);
    old_handle = r_u32(entry);
    first = sub_8001ED28(context, record + 64, old_handle, 0x80016464, token, 1);
    w_u32(entry, first);
    w_u32(entry + 4, identifier);
    first = r_u32(record);
    w_u32(entry + 12, first);
    second = r_u32(record + 4);
    w_u32(entry + 20, 0);
    w_u32(entry + 24, 1);
    w_u32(entry + 16, second);
    return 1;
}
