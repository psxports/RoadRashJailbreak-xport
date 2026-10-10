#include "native_game_api.h"
#include "psx.h"
#include "race_bodyless_batch_007.h"

uint32_t sub_800249C8(uint32_t incoming_sp, uint32_t arguments[8], RRJBodylessRegistersCall call)
{
    FUNCTION_MARKER(0x800249C8u, "SLUS_010.53");
    uint32_t result = rrj_read32(0x800cd694) < 2;
    uint32_t output = incoming_sp - 32 + 16;
    if (result)
        return result;
    if (!(uint32_t)sub_80020F50())
        return 0x800d0000;
    result = r_u8(0x800d75a0);
    if (!result)
        return result;
    result = rrj_read32(0x800d7588);
    if (!result)
        return result;
    arguments[0] = output;
    result = call(rrj_host_context(), 0x80025098, arguments);
    arguments[0] = rrj_read32(output);
    rrj_write32(0x800cd6a0, result);
    rrj_write32(0x800cd690, 0);
    (void)call(rrj_host_context(), 0x80024aa0, arguments);
    return call(rrj_host_context(), 0x80020f9c, arguments);
}

uint32_t sub_80020F50(void)
{
    FUNCTION_MARKER(0x80020F50u, "SLUS_010.53");
    if (rrj_read32(0x800d7564))
        return 0;
    return rrj_read32(0x800d7578) == 0;
}

uint32_t sub_80025098(uint32_t arguments[8], RRJBodylessRegistersCall call)
{
    FUNCTION_MARKER(0x80025098u, "SLUS_010.53");
    uint32_t result;
    rrj_write32(0x80053658, 1);
    result = call(rrj_host_context(), 0x800250c8, arguments);
    rrj_write32(0x80053658, 0);
    return result;
}

uint32_t sub_800250C8(uint32_t arguments[8])
{
    FUNCTION_MARKER(0x800250C8u, "SLUS_010.53");
    uint32_t output = arguments[0], index, next, wrapped, flag, count, item, second;
    count = rrj_read32(0x800d7f88);
    if (rrj_s32(count) <= 0)
    {
        rrj_write32(output, 0);
        return UINT32_MAX;
    }
    index = rrj_read32(0x800d7f84);
    next = index + 1;
    wrapped = rrj_s32(next) < 30 ? next : 0;
    arguments[0] = wrapped;
    rrj_write32(0x800d7f84, next);
    flag = rrj_read32(0x80053658);
    rrj_write32(0x800d7f84, wrapped);
    if (!flag)
        (void)sub_80043DA4();
    count = rrj_read32(0x800d7f88);
    arguments[0] = rrj_read32(0x80053658);
    rrj_write32(0x800d7f88, count - 1);
    if (!arguments[0])
        (void)sub_80043DB4();
    item = 0x800d7e90 + (index << 3);
    second = rrj_read32(item + 4);
    rrj_write32(output, second);
    return rrj_read32(item);
}

uint32_t sub_80024AA0(uint32_t arguments[8], RRJBodylessRegistersCall call)
{
    FUNCTION_MARKER(0x80024AA0u, "SLUS_010.53");
    uint32_t pointer = arguments[0];
    uint32_t result = (uint32_t)sub_80020F50();
    arguments[0] = pointer;
    if (result)
    {
        arguments[1] = 0x2000;
        arguments[2] = 0;
        (void)call(rrj_host_context(), 0x80020f7c, arguments);
        arguments[0] = pointer + 0x2000;
        arguments[1] = 0x2000;
        arguments[2] = 1;
        result = call(rrj_host_context(), 0x80020f7c, arguments);
    }
    return result;
}

uint32_t sub_80020F7C(RRJNativeCallFrame *frame, uint32_t arguments[8])
{
    FUNCTION_MARKER(0x80020F7Cu, "SLUS_010.53");
    frame->secondary_result = 0x800d7548;
    frame->return_value = arguments[2] << 2;
    frame->return_value += arguments[2];
    frame->return_value <<= 2;
    frame->return_value += frame->secondary_result;
    rrj_write32(frame->return_value + 28, arguments[0]);
    return frame->return_value;
}

uint32_t sub_80020F9C(uint32_t arguments[8], RRJBodylessRegistersCall call)
{
    FUNCTION_MARKER(0x80020F9Cu, "SLUS_010.53");
    uint32_t result = r_u8(0x800d75a0);
    if (result)
    {
        result = rrj_read32(0x800d7564);
        if (result)
        {
            result = rrj_read32(0x800d7588);
            if (result)
            {
                rrj_write32(0x800d7554, 0);
                result = call(rrj_host_context(), 0x800214d0, arguments);
            }
        }
    }
    return result;
}

uint32_t sub_800214D0(uint32_t incoming_sp, uint32_t arguments[8], RRJBodylessRegistersCall call)
{
    FUNCTION_MARKER(0x800214D0u, "SLUS_010.53");
    uint32_t index = rrj_read32(0x800d7554), count, capacity, offset, mode = 0;
    uint32_t size, item, sum, pointer, parameters = incoming_sp - 64 + 16;
    w_u8(0x800d75a0, 0);
    if (!index)
    {
        if (!rrj_read32(0x800d7564))
            return 0;
        if (!rrj_read32(0x800d7588))
            return 0;
    }
    count = rrj_read32(0x800d7588);
    capacity = rrj_read32(0x800d7584);
    if (count == capacity)
    {
        (void)call(rrj_host_context(), 0x8002167c, arguments);
        w_u8(0x800d754b, 1);
    }
    if (!index)
    {
        count = rrj_read32(0x800d7588);
        rrj_write32(0x800d7588, count - 1);
    }
    offset = rrj_read32(0x800d758c);
    if (offset)
    {
        capacity = rrj_read32(0x800d7584);
        size = rrj_read32(0x800d7580);
        arguments[3] = (capacity - 1) * size;
        mode = offset == arguments[3] ? 1 : 2;
    }
    item = 0x800d7548 + ((index << 2) + index) * 4;
    arguments[0] = rrj_read32(item + 28);
    arguments[1] = mode;
    (void)(uint32_t)sub_800212B0(arguments[0], arguments[1]);
    sum = rrj_read32(item + 32);
    offset = rrj_read32(0x800d758c);
    size = rrj_read32(0x800d7580);
    sum += offset;
    rrj_write32(parameters, sum);
    rrj_write32(parameters + 4, size);
    pointer = rrj_read32(item + 28);
    mode = r_u8(0x800d7548);
    rrj_write32(parameters + 8, pointer);
    arguments[4] = sum;
    arguments[5] = size;
    arguments[6] = pointer;
    if (mode && rrj_read32(0x800d7590) == offset)
    {
        rrj_write32(parameters, sum + 64);
        rrj_write32(parameters + 4, size - 64);
        rrj_write32(parameters + 8, pointer + 64);
        arguments[4] = sum + 64;
        arguments[5] = size - 64;
        arguments[6] = pointer + 64;
    }
    arguments[0] = parameters;
    arguments[1] = 0;
    arguments[2] = size;
    rrj_write32(parameters + 12, 0x80021884);
    rrj_write32(parameters + 16, 0);
    rrj_write32(parameters + 20, 1);
    arguments[7] = 0x80021884;
    return call(rrj_host_context(), 0x8001e22c, arguments);
}

uint32_t sub_800212B0(uint32_t pointer, uint32_t mode)
{
    FUNCTION_MARKER(0x800212B0u, "SLUS_010.53");
    uint32_t address, result = 0x800d0000;
    if (!mode)
        w_u8(pointer + 1, r_u8(pointer + 1) | 4);
    if (mode == 1)
    {
        address = pointer + rrj_read32(0x800d7580) - 15;
        result = r_u8(address) | 1;
        w_u8(address, (uint8_t)result);
    }
    return result;
}

uint32_t sub_80021884(uint32_t runtime_gp, uint32_t arguments[8], RRJBodylessRegistersCall call)
{
    FUNCTION_MARKER(0x80021884u, "SLUS_010.53");
    uint32_t index = rrj_read32(0x800d7554), limit, count = 0, target, size, capacity, offset, flag;
    arguments[0] = 0x800d7548;
    limit = rrj_read32(0x800d754c);
    arguments[1] = limit;
    ++index;
    rrj_write32(0x800d7554, index);
    if (index < limit)
        return call(rrj_host_context(), 0x800214d0, arguments);
    rrj_write32(0x800d7554, 0);
    if (limit)
    {
        arguments[1] = 0x800d7548;
        do
        {
            rrj_write32(arguments[0] + 28, 0);
            limit = rrj_read32(0x800d754c);
            ++count;
            arguments[0] += 20;
        } while (count < limit);
    }
    target = rrj_read32(runtime_gp + 0x838);
    if (target)
    {
        arguments[0] = 3;
        (void)call(rrj_host_context(), target, arguments);
    }
    size = rrj_read32(0x800d7580);
    capacity = rrj_read32(0x800d7584);
    offset = rrj_read32(0x800d758c) + size;
    arguments[2] = capacity * size;
    rrj_write32(0x800d758c, offset);
    if (offset == arguments[2])
        rrj_write32(0x800d758c, 0);
    flag = r_u8(0x800d754b);
    w_u8(0x800d75a0, 1);
    if (flag)
    {
        offset = rrj_read32(0x800d758c);
        rrj_write32(0x800d759c, 0);
        rrj_write32(0x800d7590, offset);
        (void)call(rrj_host_context(), 0x80021644, arguments);
        w_u8(0x800d754b, 0);
    }
    if (rrj_read32(0x800d7564) && rrj_read32(0x800d7588))
        return call(rrj_host_context(), 0x800214d0, arguments);
    return 0;
}

uint32_t sub_80024F78(uint32_t incoming_sp, uint32_t arguments[8], RRJBodylessRegistersCall call)
{
    FUNCTION_MARKER(0x80024F78u, "SLUS_010.53");
    uint32_t result, output = incoming_sp - 32 + 16;
    if (rrj_s32(arguments[0]) < 3)
        return 1;
    if (rrj_s32(arguments[0]) >= 5)
        return 0x800d0000;
    arguments[0] = rrj_read32(0x800cd6a0);
    if (rrj_s32(arguments[0]) >= 0)
    {
        (void)call(rrj_host_context(), 0x80030fa0, arguments);
        rrj_write32(0x800cd6a0, UINT32_MAX);
    }
    arguments[0] = output;
    result = call(rrj_host_context(), 0x80025098, arguments);
    rrj_write32(0x800cd6a0, result);
    if (rrj_s32(result) < 0)
    {
        rrj_write32(0x800cd690, 1);
        return 1;
    }
    arguments[0] = rrj_read32(output);
    result = call(rrj_host_context(), 0x80024aa0, arguments);
    rrj_write32(0x800cd690, 0);
    return result;
}

uint32_t sub_80030FA0(uint32_t arguments[8], RRJBodylessRegistersCall call)
{
    FUNCTION_MARKER(0x80030FA0u, "SLUS_010.53");
    uint32_t result;
    rrj_write32(0x800541d0, 1);
    result = call(rrj_host_context(), 0x80030fd0, arguments);
    rrj_write32(0x800541d0, 0);
    return result;
}

uint32_t sub_8002169C(uint32_t runtime_gp, uint32_t arguments[8], RRJBodylessRegistersCall call)
{
    FUNCTION_MARKER(0x8002169Cu, "SLUS_010.53");
    uint32_t timer, old_tick, tick, busy, active, progress, period, count, capacity;
    uint32_t queued = 0, state, target, size, cursor, stride, product, base, delay;
    (void)call(rrj_host_context(), 0x8002167c, arguments);
    arguments[3] = 0x800d0000;
    arguments[2] = 0x800d7548;
    timer = rrj_read32(0x8005b2f8);
    old_tick = rrj_read32(runtime_gp + 0x824);
    arguments[0] = old_tick;
    tick = rrj_read32(timer + 12);
    busy = r_u8(0x800d75ac);
    arguments[1] = busy;
    rrj_write32(runtime_gp + 0x828, tick - old_tick);
    rrj_write32(runtime_gp + 0x824, tick);
    if (busy)
        return 1;
    active = r_u8(0x800d7548);
    w_u8(0x800d75ac, 1);
    if (active)
    {
        progress = rrj_read32(0x800d759c);
        period = rrj_read32(0x800d7594);
        ++progress;
        rrj_write32(0x800d759c, progress);
        if (progress == period)
        {
            count = rrj_read32(0x800d7588);
            capacity = rrj_read32(0x800d7584);
            if (count < capacity)
                rrj_write32(0x800d7588, count + 1);
            rrj_write32(0x800d759c, 0);
            queued = 1;
        }
        if (r_u8(0x800d75a0) && rrj_read32(0x800d7588))
        {
            state = rrj_read32(0x800d7564);
            if (!state)
            {
                target = rrj_read32(runtime_gp + 0x838);
                if (target)
                {
                    arguments[0] = 3;
                    (void)call(rrj_host_context(), target, arguments);
                }
                state = rrj_read32(0x800d7564);
                if (!state)
                {
                    capacity = rrj_read32(0x800d7584);
                    count = rrj_read32(0x800d7588);
                    if (count >= capacity - 1)
                    {
                        target = rrj_read32(runtime_gp + 0x838);
                        if (target)
                        {
                            arguments[0] = 4;
                            (void)call(rrj_host_context(), target, arguments);
                        }
                    }
                    state = rrj_read32(0x800d7564);
                }
            }
            if (state)
            {
                rrj_write32(0x800d7554, 0);
                (void)call(rrj_host_context(), 0x800214d0, arguments);
            }
        }
        capacity = rrj_read32(0x800d7584);
        size = rrj_read32(0x800d7580);
        product = capacity * size;
        cursor = rrj_read32(0x800d7590);
        stride = rrj_read32(0x800d7598);
        cursor += stride;
        rrj_write32(0x800d7590, cursor);
        if (cursor == product)
            rrj_write32(0x800d7590, 0);
        base = rrj_read32(0x800d7568);
        arguments[0] = base + rrj_read32(0x800d7590);
        (void)call(rrj_host_context(), 0x80050c58, arguments);
        arguments[0] = 1;
        (void)call(rrj_host_context(), 0x80050b18, arguments);
        timer = rrj_read32(0x8005b2f8);
        delay = rrj_read32(0x800d75a4);
        tick = rrj_read32(timer + 12);
        rrj_write32(0x800d75a8, tick + delay);
        if (queued)
        {
            target = rrj_read32(runtime_gp + 0x838);
            if (target)
            {
                arguments[0] = 5;
                (void)call(rrj_host_context(), target, arguments);
            }
        }
    }
    w_u8(0x800d75ac, 0);
    return 0x800d0000;
}

static uint32_t road_signed_half(uint32_t address)
{
    return (uint32_t)(int32_t)(int16_t)r_u16(address);
}

static uint32_t road_stack_distance(RRJMemory *m, uint32_t actor, uint32_t frame, uint32_t arguments[8], RRJBodylessRegistersCall call)
{
    uint32_t x = rrj_read32(actor + 184), projected_x, projected_z, y, projected_y, z;
    projected_x = rrj_read32(frame + 64);
    projected_z = rrj_read32(frame + 72);
    x -= projected_x;
    rrj_write32(frame + 80, x);
    y = rrj_read32(actor + 188);
    projected_y = rrj_read32(frame + 68);
    arguments[0] = (uint32_t)(rrj_s32(x) >> 16);
    y -= projected_y;
    rrj_write32(frame + 84, y);
    z = rrj_read32(actor + 192);
    arguments[1] = (uint32_t)(rrj_s32(y) >> 16);
    z -= projected_z;
    arguments[2] = (uint32_t)(rrj_s32(z) >> 16);
    rrj_write32(frame + 88, z);
    return call(m, 0x8001fcb0, arguments);
}

uint32_t sub_8003BFE8(uint32_t actor, uint32_t incoming_sp, uint32_t arguments[8], RRJBodylessRegistersCall call)
{
    FUNCTION_MARKER(0x8003BFE8u, "SLUS_010.53");
    uint32_t frame = incoming_sp - 144, result, root, distance, nearest, best, header;
    uint32_t index = 0, entry, word, route, route_index, direction, sub, piece, progress;
    uint32_t first, second, origin, orientation, candidate, found, table, tag, bit_index, threshold, flags, mask;
    result = r_u16(actor + 362);
    if (result)
        return result;
    result = rrj_read32(actor + 372);
    if (!result)
        return result;
    result = rrj_read32(actor + 388) & 1;
    arguments[0] = frame + 16;
    if (!result)
        return result;
    arguments[1] = 0;
    root = rrj_read32(actor + 328);
    arguments[2] = 32;
    (void)(uint32_t)sub_8001E100(arguments[0], arguments[1], arguments[2]);
    arguments[0] = actor + 360;
    arguments[1] = frame + 96;
    arguments[2] = 0;
    rrj_write32(frame + 16, root);
    distance = (uint32_t)sub_8003A5F4(arguments[0], arguments[1], arguments[2]);
    arguments[0] = distance;
    nearest = rrj_read32(frame + 96);
    if (nearest == UINT32_MAX)
        return 0x00310000;
    result = rrj_s32(distance) > 0x0031ffff;
    arguments[3] = frame + 64;
    if (result)
        return result;
    piece = rrj_read32(actor + 340);
    arguments[2] = rrj_read32(actor + 348);
    arguments[0] = piece + 20;
    arguments[1] = piece + 14;
    (void)(uint32_t)sub_8002EAD8(rrj_at(arguments[0], 12), rrj_at(arguments[1], 6), arguments[2], rrj_at(arguments[3], 12));
    best = road_stack_distance(rrj_host_context(), actor, frame, arguments, call);
    result = road_signed_half(root + 16);
    if (result != 1)
        return 1;
    result = rrj_read32(root + 12);
    if (result)
        return result;
    arguments[0] = rrj_read32(root);
    header = (uint32_t)sub_80039AFC(arguments[0]);
    if (!header)
        return header;
    result = road_signed_half(header + 2);
    if (rrj_s32(result) <= 0)
        return result;
    do
    {
        entry = header + 8 + 24 * index;
        word = rrj_read32(actor + 360);
        arguments[0] = word;
        if (!(word >> 16))
        {
            direction = road_signed_half(entry + 4);
            if ((word & 0xffff) == direction)
                goto next_road;
        }
        arguments[1] = road_signed_half(entry + 4);
        arguments[0] = root;
        route = (uint32_t)sub_80039C90(arguments[0], arguments[1]);
        if (!route)
            return route;
        route_index = road_signed_half(route + 20);
        direction = road_signed_half(entry + 2);
        arguments[0] = direction;
        sub = rrj_read32(root + 48) + 28 * route_index;
        arguments[3] = sub;
        if (rrj_s32(direction) > 0)
        {
            first = road_signed_half(sub + 8);
            piece = rrj_read32(root + 52) + 52 * first;
            progress = 0;
        }
        else
        {
            first = road_signed_half(sub + 8);
            second = road_signed_half(sub + 10);
            piece = rrj_read32(root + 52) + 52 * (first + second) - 52;
            progress = rrj_read32(piece + 32);
        }
        arguments[0] = actor + 172;
        arguments[1] = frame + 16;
        arguments[2] = actor + 184;
        rrj_write32(frame + 20, route);
        rrj_write32(frame + 24, sub);
        rrj_write32(frame + 28, piece);
        rrj_write32(frame + 36, progress);
        piece = (uint32_t)sub_80036B14(arguments[0], arguments[1], arguments[2]);
        arguments[0] = actor + 184;
        arguments[1] = piece + 2;
        origin = piece + 20;
        arguments[2] = origin;
        first = call(rrj_host_context(), 0x800b6aac, arguments);
        arguments[0] = actor + 184;
        orientation = piece + 14;
        arguments[1] = orientation;
        arguments[2] = origin;
        rrj_write32(frame + 32, first);
        second = call(rrj_host_context(), 0x800b6aac, arguments);
        arguments[0] = actor + 450;
        arguments[1] = frame + 16;
        arguments[2] = frame + 48;
        rrj_write32(frame + 36, second);
        (void)(uint32_t)sub_8003662C(arguments[0], arguments[1], arguments[2]);
        arguments[0] = origin;
        arguments[1] = orientation;
        arguments[2] = rrj_read32(frame + 36);
        arguments[3] = frame + 64;
        (void)(uint32_t)sub_8002EAD8(rrj_at(arguments[0], 12), rrj_at(arguments[1], 6), arguments[2], rrj_at(arguments[3], 12));
        distance = road_stack_distance(rrj_host_context(), actor, frame, arguments, call);
        if (rrj_s32(distance) >= rrj_s32(best))
            goto next_road;
        word = rrj_read32(actor + 360);
        arguments[0] = word;
        if (word >> 16)
            goto next_road;
        candidate = rrj_read32(frame + 48);
        if (candidate >> 16)
            goto next_road;
        arguments[0] = actor + 328;
        if (word == candidate)
            goto next_road;
        arguments[1] = frame + 16;
        arguments[2] = 32;
        (void)(uint32_t)sub_8001E0B4(arguments[0], arguments[1], arguments[2]);
        arguments[0] = actor + 360;
        arguments[1] = frame + 48;
        arguments[2] = 12;
        (void)(uint32_t)sub_8001E0B4(arguments[0], arguments[1], arguments[2]);
        arguments[0] = actor;
        arguments[1] = 1;
        arguments[2] = 0;
        arguments[3] = UINT32_MAX;
        (void)(uint32_t)sub_8003DE28(arguments[0], arguments[1], arguments[2], arguments[3]);
        arguments[0] = actor;
        arguments[1] = 1;
        arguments[2] = UINT32_MAX;
        (void)sub_8003DF54(arguments[0], arguments[1], arguments[2]);
        arguments[0] = rrj_read32(frame + 96);
        found = (uint32_t)sub_8003F3B4(arguments[0]);
        arguments[1] = found;
        if (found)
            rrj_write32(actor + 428, found);
        table = rrj_read32(actor + 428);
        arguments[2] = table;
        if (table)
        {
            tag = r_u16(actor + 172);
            arguments[0] = tag & 31;
            if ((tag >> 5) < 2)
            {
                threshold = rrj_read32(0x8005b1f8);
                bit_index = tag & 31;
                if (rrj_s32(bit_index) >= rrj_s32(threshold))
                    bit_index -= threshold;
                arguments[0] = bit_index;
                mask = 1u << (bit_index & 31);
                flags = r_u16(table + 118);
                flags = found ? flags | mask : flags & ~mask;
                w_u16(table + 118, (uint16_t)flags);
            }
        }
        arguments[0] = actor + 172;
        arguments[1] = 1;
        arguments[2] = 0;
        (void)sub_8003AF9C(arguments[0], arguments[1], arguments[2]);
        arguments[0] = actor + 172;
        result = (uint32_t)sub_8003B61C(arguments[0]);
        rrj_write32(actor + 324, result);
        return result;
    next_road:
        result = road_signed_half(header + 2);
        ++index;
    } while (rrj_s32(index) < rrj_s32(result));
    return index << 1;
}

uint32_t sub_800102A4(uint32_t destination, uint32_t packed, uint32_t incoming_v0)
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
                rrj_write32(output + (words << 2), 0);
            }
        }
        end += 32;
    }
    return incoming_v0;
}

uint32_t sub_8001A0C0(uint32_t record, uint32_t identifier, uint32_t owner_token, RRJBodylessRegistersCall call)
{
    uint32_t arguments[8] = {0};
    uint32_t index, entry, state, timer, bound, channel = 0, context_offset = 0, context;
    uint32_t slot, table, row, count, limit, old_handle, token, first, second;
    FUNCTION_MARKER(0x8001A0C0, "SLUS_010.53");
    arguments[0] = identifier;
    index = (uint32_t)sub_8001A2E4(arguments[0]);
    if (rrj_s32(index) < 0)
        return 2;
    entry = 0x800d6aa0 + (index << 5);
    state = rrj_read32(entry + 8);
    if (state == 3)
    {
        if ((identifier & 255) != 6 && (identifier & 255) != 22)
            return 2;
        timer = rrj_read32(0x8005b2f8);
        bound = rrj_read32(timer + 48);
        if (bound)
        {
            do
            {
                context = rrj_read32(0x8005B40Cu) + context_offset;
                arguments[1] = context;
                slot = rrj_read32(context + 36);
                arguments[0] = slot;
                table = rrj_read32(0x8005B410u + (channel << 2));
                row = table + 44 * slot;
                count = rrj_read32(context + 40);
                limit = slot + count;
                while (rrj_s32(slot) < rrj_s32(limit))
                {
                    timer = rrj_read32(0x8005b2f8);
                    state = r_u16(row);
                    bound = rrj_read32(timer + 48);
                    if (state < bound)
                    {
                        state = rrj_read32(row + 8);
                        arguments[0] = slot;
                        if (state == 2)
                        {
                            arguments[1] = channel;
                            (void)call(rrj_host_context(), 0x80017814, arguments);
                        }
                    }
                    ++slot;
                    row += 44;
                }
                timer = rrj_read32(0x8005b2f8);
                bound = rrj_read32(timer + 48);
                ++channel;
                context_offset += 72;
            } while (channel < bound);
        }
    }
    old_handle = rrj_read32(entry);
    arguments[0] = old_handle;
    rrj_write32(entry + 8, 1);
    if (rrj_s32(old_handle) >= 0)
        (void)call(rrj_host_context(), 0x8001ee28, arguments);
    arguments[1] = record + 8;
    arguments[0] = rrj_read32(entry + 28);
    arguments[2] = 56;
    (void)(uint32_t)sub_8001E0B4(arguments[0], arguments[1], arguments[2]);
    arguments[1] = record + 64;
    token = owner_token;
    token |= 0x80000000 | (index << 16);
    arguments[0] = rrj_read32(entry + 28);
    arguments[2] = rrj_read32(entry);
    arguments[3] = 0x80016464;
    arguments[4] = token;
    arguments[5] = 1;
    first = call(rrj_host_context(), 0x8001ed28, arguments);
    rrj_write32(entry, first);
    rrj_write32(entry + 4, identifier);
    first = rrj_read32(record);
    rrj_write32(entry + 12, first);
    second = rrj_read32(record + 4);
    rrj_write32(entry + 20, 0);
    rrj_write32(entry + 24, 1);
    rrj_write32(entry + 16, second);
    return 1;
}
