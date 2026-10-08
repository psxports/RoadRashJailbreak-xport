#include "race_bodyless_batch_008.h"

uint32_t sub_8001A2E4(RRJMemory *m, uint32_t runtime_gp, uint32_t arguments[8], RRJBodylessRegistersCall call)
{
    uint32_t identifier = arguments[0], mode = 2, index, attempt = 0, address, tag, entry, state, handle, current;
    arguments[0] = rrj_read32(m, runtime_gp + 0x7a0);
    if (arguments[0])
    {
        arguments[0] = rrj_read32(m, arguments[0]);
        if (arguments[0])
        {
            arguments[1] = 0;
            if (call(m, 0x800138e8, arguments) < 6)
                mode = 1;
        }
    }
    arguments[0] = 0xf2000002;
    index = (9 * (call(m, 0x80043f00, arguments) & 255)) >> 8;
    arguments[0] = identifier & 255;
    arguments[2] = index;
    arguments[3] = 0;
    do
    {
        address = 0x800d6c40 + (index << 2);
        tag = rrj_read32(m, address);
        arguments[1] = tag;
        if ((tag & 15) < 2)
            rrj_write32(m, address, (tag & 3) | (mode << 4));
        if (rrj_read32(m, address) == (identifier & 255))
        {
            entry = 0x800d6aa0 + (index << 5);
            if (((uint32_t)r_u8(entry + 4) >> 4) != ((identifier & 255) >> 4))
                return index;
            state = rrj_read32(m, entry + 8);
            arguments[1] = state;
            if (state >= 3)
            {
                handle = rrj_read32(m, entry);
                current = rrj_read32(m, runtime_gp + 0x778);
                if (handle != current)
                    return index;
            }
            if (!state)
                return index;
        }
        ++index;
        if (rrj_s32(index) >= 9)
            index = 0;
        ++attempt;
        arguments[2] = index;
        arguments[3] = attempt;
    } while (rrj_s32(attempt) < 9);
    return UINT32_MAX;
}

static uint32_t curve_signed_half(uint32_t address)
{
    return (uint32_t)(int32_t)(int16_t)r_u16(address);
}

static uint32_t curve_signed_byte(uint32_t address)
{
    return (uint32_t)(int32_t)(int8_t)r_u8(address);
}

static int curve_endpoint(RRJMemory *m, uint32_t record, uint32_t piece)
{
    uint32_t index = curve_signed_half(piece);
    if (!index)
        return 1;
    return index == curve_signed_half(rrj_read32(m, record + 8) + 10) - 1;
}

static uint32_t curve_copy(RRJMemory *m, uint32_t destination, uint32_t source, uint32_t arguments[8], RRJBodylessRegistersCall call)
{
    arguments[0] = destination;
    arguments[1] = source;
    arguments[2] = 32;
    return call(m, 0x8001e0b4, arguments);
}

static uint32_t curve_neighbor(RRJMemory *m, uint32_t record, uint32_t frame, uint32_t actor, int reload_actor, int resolve, uint32_t selected_direction, uint32_t arguments[8], RRJBodylessRegistersCall call)
{
    uint32_t direction = resolve ? rrj_read32(m, frame + 248) : selected_direction;
    uint32_t target = rrj_s32(direction) > 0 ? 0x80037a30 : 0x80037fbc;
    uint32_t result;
    arguments[1] = record;
    arguments[2] = frame + 136;
    arguments[3] = frame + 232;
    if (reload_actor)
        actor = rrj_read32(m, frame + 252) + 172;
    rrj_write32(m, frame + 16, 3);
    arguments[4] = 3;
    arguments[0] = actor;
    result = call(m, target, arguments);
    if (resolve)
    {
        arguments[0] = actor;
        arguments[1] = frame + 136;
        arguments[2] = frame + 232;
        arguments[3] = result;
        rrj_write32(m, frame + 16, record);
        rrj_write32(m, frame + 20, frame + 248);
        arguments[4] = record;
        arguments[5] = frame + 248;
        result = call(m, 0x80039048, arguments);
    }
    return result;
}

static uint32_t curve_boundary(RRJMemory *m, uint32_t frame, uint32_t arguments[8], RRJBodylessRegistersCall call)
{
    uint32_t record = frame + 104;
    uint32_t source = rrj_read32(m, frame + 260);
    uint32_t direction = rrj_read32(m, frame + 248);
    rrj_write32(m, frame + 232, direction);
    (void)curve_copy(m, record, source, arguments, call);
    (void)curve_neighbor(m, record, frame, 0, 1, 1, 0, arguments, call);
    return record;
}

static uint32_t curve_project(RRJMemory *m, uint32_t piece, uint32_t tangent_offset, uint32_t coefficient, uint32_t output, uint32_t arguments[8], RRJBodylessRegistersCall call)
{
    arguments[0] = piece + 20;
    arguments[1] = piece + tangent_offset;
    arguments[2] = coefficient;
    arguments[3] = output;
    return call(m, 0x8002ead8, arguments);
}

static uint32_t curve_ratio(RRJMemory *m, uint32_t numerator, uint32_t denominator, uint32_t arguments[8], RRJBodylessRegistersCall call)
{
    int numerator_positive = rrj_s32(numerator) > 0;
    int denominator_positive = rrj_s32(denominator) > 0;
    uint32_t result;
    arguments[0] = numerator_positive ? numerator : 0 - numerator;
    arguments[1] = denominator_positive ? denominator : 0 - denominator;
    result = call(m, 0x80010028, arguments);
    return numerator_positive != denominator_positive ? 0 - result : result;
}

static uint32_t curve_extrapolation_weight(RRJMemory *m, uint32_t frame, uint32_t distance, uint32_t length, uint32_t endpoint, int right, uint32_t arguments[8], RRJBodylessRegistersCall call)
{
    uint32_t result, blend_weight;
    if (rrj_s32(length) > 0)
    {
        arguments[0] = 0;
        arguments[1] = length;
        blend_weight = 0 - call(m, 0x80010028, arguments);
        rrj_write32(m, frame + 16, blend_weight);
        arguments[4] = blend_weight;
        arguments[0] = frame + 40;
        arguments[1] = endpoint;
        arguments[2] = endpoint;
        arguments[3] = 0x10000 - blend_weight;
        (void)call(m, 0x8002e6f8, arguments);
        length = 0;
    }
    arguments[0] = 0 - distance;
    arguments[1] = 0 - length;
    result = call(m, 0x80010028, arguments);
    return right ? 0x8000 - result : 0x8000 + result;
}

uint32_t sub_800386DC(RRJMemory *m, uint32_t incoming_sp, uint32_t arguments[8], RRJBodylessRegistersCall call)
{
    uint32_t frame = incoming_sp - 312;
    uint32_t record = rrj_read32(m, incoming_sp + 16);
    uint32_t copy_output = rrj_read32(m, incoming_sp + 24);
    uint32_t target_distance = arguments[1], initial_distance = arguments[2], input_direction = arguments[3];
    uint32_t original_piece, piece, next_piece, direction, old_direction, length, distance;
    uint32_t old_coefficient, next_coefficient, coefficient, actor, result, weight, endpoint, component;
    uint32_t square, half, first_weight, middle_weight, first_product, middle_product, output;
    int smooth;
    rrj_write32(m, frame + 252, arguments[0]);
    original_piece = rrj_read32(m, record + 12);
    arguments[1] = initial_distance;
    rrj_write32(m, frame + 256, original_piece);
    if (rrj_s32(input_direction) > 0)
    {
        length = rrj_read32(m, original_piece + 32);
        rrj_write32(m, frame + 248, 1);
        distance = length - initial_distance;
    }
    else
    {
        rrj_write32(m, frame + 248, UINT32_MAX);
        distance = initial_distance;
    }
    if (rrj_s32(distance) < rrj_s32(target_distance))
    {
        actor = rrj_read32(m, frame + 252) + 172;
        do
        {
            piece = rrj_read32(m, record + 12);
            arguments[0] = piece;
            if (!curve_endpoint(m, record, piece))
            {
                direction = rrj_read32(m, frame + 248);
                rrj_write32(m, record + 12, piece + 52 * direction);
            }
            else
            {
                direction = rrj_read32(m, frame + 248);
                output = rrj_read32(m, incoming_sp + 16);
                rrj_write32(m, frame + 232, direction);
                if (record == output)
                {
                    record = frame + 72;
                    (void)curve_copy(m, record, output, arguments, call);
                }
                arguments[1] = rrj_read32(m, frame + 248);
                arguments[0] = record;
                result = call(m, 0x800394f0, arguments);
                if (result)
                {
                    piece = rrj_read32(m, record + 12);
                    direction = rrj_read32(m, frame + 248);
                    result = curve_project(m, piece, 14, rrj_s32(direction) > 0 ? target_distance - distance : distance - target_distance, rrj_read32(m, incoming_sp + 20), arguments, call);
                    arguments[0] = copy_output;
                    if (copy_output)
                        result = curve_copy(m, copy_output, record, arguments, call);
                    return result;
                }
                (void)curve_neighbor(m, record, frame, actor, 0, 1, 0, arguments, call);
            }
            piece = rrj_read32(m, record + 12);
            distance += rrj_read32(m, piece + 32);
        } while (rrj_s32(distance) < rrj_s32(target_distance));
    }
    rrj_write32(m, frame + 260, record);
    if (copy_output)
        (void)curve_copy(m, copy_output, record, arguments, call);
    direction = rrj_read32(m, frame + 248);
    original_piece = rrj_read32(m, record + 12);
    rrj_write32(m, frame + 264, direction);
    length = rrj_read32(m, original_piece + 32);
    distance -= target_distance;
    if (rrj_s32(direction) > 0)
    {
        old_coefficient = curve_signed_byte(original_piece + 38) << 13;
        distance = length - distance;
    }
    else
        old_coefficient = curve_signed_byte(original_piece + 39) << 13;
    piece = rrj_read32(m, record + 12);
    arguments[0] = piece;
    component = curve_signed_half(piece);
    rrj_write32(m, frame + 248, 1);
    if (!component || component == curve_signed_half(rrj_read32(m, record + 8) + 10) - 1)
        record = curve_boundary(m, frame, arguments, call);
    else
        rrj_write32(m, record + 12, piece + 52);
    direction = rrj_read32(m, frame + 248);
    next_piece = rrj_read32(m, record + 12);
    old_direction = rrj_read32(m, frame + 264);
    if (rrj_s32(direction) > 0)
        next_coefficient = curve_signed_byte(next_piece + (rrj_s32(old_direction) > 0 ? 38 : 39)) << 13;
    else
    {
        next_coefficient = curve_signed_byte(next_piece + (rrj_s32(old_direction) > 0 ? 39 : 38)) << 13;
        length += rrj_read32(m, next_piece + 32);
    }
    if (rrj_s32(distance) < (rrj_s32(length) >> 1))
    {
        (void)curve_project(m, next_piece, 2, next_coefficient, frame + 56, arguments, call);
        result = curve_project(m, original_piece, 2, old_coefficient, frame + 40, arguments, call);
        endpoint = frame + 56;
        smooth = rrj_s32(distance) < 0;
        if (smooth)
        {
            weight = curve_extrapolation_weight(m, frame, distance, length, endpoint, 0, arguments, call);
            record = rrj_read32(m, frame + 260);
            arguments[0] = weight;
            rrj_write32(m, record + 12, original_piece);
            arguments[1] = original_piece;
            component = curve_signed_half(original_piece);
            rrj_write32(m, frame + 248, UINT32_MAX);
            if (!component || component == curve_signed_half(rrj_read32(m, record + 8) + 10) - 1)
                record = curve_boundary(m, frame, arguments, call);
            else
                rrj_write32(m, record + 12, original_piece - 52);
            direction = rrj_read32(m, frame + 248);
            if (rrj_s32(direction) < 0)
            {
                old_direction = rrj_read32(m, frame + 264);
                piece = rrj_read32(m, record + 12);
                coefficient = curve_signed_byte(piece + (rrj_s32(old_direction) > 0 ? 38 : 39)) << 13;
            }
            else
            {
                if (rrj_s32(curve_signed_half(rrj_read32(m, record + 8) + 10)) >= 2)
                {
                    piece = rrj_read32(m, record + 12);
                    rrj_write32(m, record + 12, piece + 52);
                }
                else
                    (void)curve_neighbor(m, record, frame, 0, 1, 0, direction, arguments, call);
                direction = rrj_read32(m, frame + 248);
                old_direction = rrj_read32(m, frame + 264);
                piece = rrj_read32(m, record + 12);
                component = r_u8(piece + 38);
                if (rrj_s32(direction ^ old_direction) < 0)
                    coefficient = (uint32_t)(rrj_s32(component << 24) >> 11);
                else
                    coefficient = curve_signed_byte(piece + 39) << 13;
            }
            result = curve_project(m, piece, 2, coefficient, frame + 24, arguments, call);
        }
        else
            weight = curve_ratio(m, distance, length, arguments, call);
    }
    else
    {
        (void)curve_project(m, original_piece, 2, old_coefficient, frame + 24, arguments, call);
        result = curve_project(m, next_piece, 2, next_coefficient, frame + 40, arguments, call);
        distance = length - distance;
        endpoint = frame + 24;
        smooth = rrj_s32(distance) < 0;
        if (smooth)
        {
            weight = curve_extrapolation_weight(m, frame, distance, length, endpoint, 1, arguments, call);
            piece = rrj_read32(m, record + 12);
            arguments[0] = piece;
            rrj_write32(m, frame + 260, record);
            component = curve_signed_half(piece);
            old_direction = rrj_read32(m, frame + 248);
            if (!component || component == curve_signed_half(rrj_read32(m, record + 8) + 10) - 1)
                record = curve_boundary(m, frame, arguments, call);
            else
                rrj_write32(m, record + 12, piece + 52 * old_direction);
            component = 0;
            if (rrj_s32(old_direction) < 0)
                component = rrj_s32(rrj_read32(m, frame + 248)) > 0;
            piece = rrj_read32(m, record + 12) + 52 * component;
            coefficient = curve_signed_byte(piece + 38) << 13;
            result = curve_project(m, piece, 2, coefficient, frame + 56, arguments, call);
        }
        else
            weight = curve_ratio(m, distance, length, arguments, call);
    }
    if (!smooth)
    {
        arguments[0] = frame + 40;
        arguments[1] = endpoint;
        arguments[3] = 0x10000;
        arguments[2] = rrj_read32(m, incoming_sp + 20);
        arguments[3] -= weight;
        rrj_write32(m, frame + 16, weight);
        arguments[4] = weight;
        result = call(m, 0x8002e6f8, arguments);
    }
    arguments[0] = weight;
    if (smooth)
    {
        arguments[1] = weight;
        square = call(m, 0x8001fc90, arguments);
        half = (uint32_t)(rrj_s32(square) >> 1);
        first_weight = half + 0x8000 - weight;
        middle_weight = weight - (square - 0x8000);
        for (component = 0; component < 12; component += 4)
        {
            arguments[1] = rrj_read32(m, frame + 24 + component);
            arguments[0] = first_weight;
            first_product = call(m, 0x8001fc90, arguments);
            arguments[0] = middle_weight;
            arguments[1] = rrj_read32(m, frame + 40 + component);
            middle_product = call(m, 0x8001fc90, arguments);
            arguments[0] = half;
            arguments[1] = rrj_read32(m, frame + 56 + component);
            result = call(m, 0x8001fc90, arguments);
            output = rrj_read32(m, incoming_sp + 20);
            rrj_write32(m, output + component, first_product + middle_product + result);
        }
    }
    piece = rrj_read32(m, frame + 256);
    output = rrj_read32(m, incoming_sp + 16);
    rrj_write32(m, output + 12, piece);
    return result;
}

uint32_t sub_8003A3CC(RRJMemory *m, uint32_t arguments[8], RRJBodylessRegistersCall call)
{
    uint32_t table, count, index = 0, random;
    if (curve_signed_half(0x800d6182) == UINT32_MAX)
        return UINT32_MAX;
    arguments[0] = rrj_read32(m, arguments[0] + 428);
    table = call(m, 0x8003f408, arguments);
    if (!table)
        return UINT32_MAX;
    count = rrj_read32(m, table + 16);
    if (rrj_s32(count) >= 2)
    {
        random = call(m, 0x8001fc58, arguments);
        count = rrj_read32(m, table + 16);
        index = count ? random % count : random;
    }
    return rrj_read32(m, table + (index << 2) + 84);
}

uint32_t sub_8002C928(RRJMemory *m, uint32_t incoming_sp, uint32_t arguments[8], RRJBodylessRegistersCall call)
{
    uint32_t frame = incoming_sp - 32, slot = arguments[0], point = arguments[1], x, y, offset;
    arguments[3] = 0x800d7fc8;
    arguments[2] = arguments[3] + (slot << 2);
    x = r_u16(point);
    offset = r_u16(arguments[2] + 32);
    arguments[3] = 0x800d7ff0;
    rrj_put16(rrj_at(m, frame + 16, 2), (uint16_t)(x + offset - 2));
    y = r_u16(point + 2);
    arguments[1] = r_u16(arguments[2] + 34);
    rrj_put16(rrj_at(m, frame + 20, 2), 5);
    rrj_put16(rrj_at(m, frame + 22, 2), 5);
    y += arguments[1] - 2;
    arguments[1] = arguments[3] + 52 * slot;
    arguments[0] = frame + 16;
    rrj_put16(rrj_at(m, frame + 18, 2), (uint16_t)y);
    return call(m, 0x80048acc, arguments);
}

uint32_t sub_8002CC74(RRJMemory *m, uint32_t incoming_sp, uint32_t arguments[8], RRJBodylessRegistersCall call)
{
    uint32_t frame = incoming_sp - 48, mode = rrj_read32(m, incoming_sp + 20);
    uint32_t first = arguments[0], second = arguments[1], rectangle = arguments[2], fourth = arguments[3];
    uint32_t extra = rrj_read32(m, incoming_sp + 16), x, width, result;
    if (mode != 0 && mode != 1 && mode != 2)
    {
        arguments[0] = first;
        return 2;
    }
    arguments[0] = first;
    if (!mode)
    {
        arguments[2] = curve_signed_half(rectangle);
        arguments[3] = curve_signed_half(rectangle + 2);
        arguments[1] = second;
        rrj_write32(m, frame + 16, fourth);
        rrj_write32(m, frame + 20, extra);
    }
    else
    {
        arguments[1] = second;
        result = call(m, 0x8002d0d8, arguments);
        arguments[0] = first;
        if (mode == 1)
        {
            x = r_u16(rectangle);
            arguments[2] = x;
            width = r_u16(rectangle + 4);
            arguments[3] = curve_signed_half(rectangle + 2);
            arguments[1] = second;
        }
        else
        {
            arguments[1] = second;
            width = r_u16(rectangle + 4);
            x = r_u16(rectangle);
            arguments[2] = x;
            arguments[3] = curve_signed_half(rectangle + 2);
            result = (uint32_t)(rrj_s32(result) >> 1);
        }
        rrj_write32(m, frame + 16, fourth);
        rrj_write32(m, frame + 20, extra);
        if (mode == 2)
            width = (uint32_t)(rrj_s32(width << 16) >> 17);
        arguments[2] = (uint32_t)(int32_t)(int16_t)(uint16_t)(x + width - result);
    }
    arguments[4] = fourth;
    arguments[5] = extra;
    return call(m, 0x8002cdc8, arguments);
}
