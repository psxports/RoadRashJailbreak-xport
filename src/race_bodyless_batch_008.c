#include "native_game_api.h"
#include "psx.h"
#include "race_pause.h"
#include "race_bodyless_batch_008.h"

uint32_t sub_8001A2E4(uint32_t identifier)
{
    uint32_t arguments[8] = {0};
    uint32_t mode = 2, index, attempt = 0, address, tag, entry, state, handle, current;
    FUNCTION_MARKER(0x8001A2E4, "SLUS_010.53");
    arguments[0] = rrj_read32(0x8005B42Cu);
    if (arguments[0])
    {
        arguments[0] = rrj_read32(arguments[0]);
        if (arguments[0])
        {
            arguments[1] = 0;
            if ((uint32_t)sub_800138E8(arguments[0], arguments[1]) < 6)
                mode = 1;
        }
    }
    arguments[0] = 0xf2000002;
    index = (9 * ((uint32_t)sub_80043F00(arguments[0]) & 255)) >> 8;
    arguments[0] = identifier & 255;
    arguments[2] = index;
    arguments[3] = 0;
    do
    {
        address = 0x800d6c40 + (index << 2);
        tag = rrj_read32(address);
        arguments[1] = tag;
        if ((tag & 15) < 2)
            rrj_write32(address, (tag & 3) | (mode << 4));
        if (rrj_read32(address) == (identifier & 255))
        {
            entry = 0x800d6aa0 + (index << 5);
            if (((uint32_t)r_u8(entry + 4) >> 4) != ((identifier & 255) >> 4))
                return index;
            state = rrj_read32(entry + 8);
            arguments[1] = state;
            if (state >= 3)
            {
                handle = rrj_read32(entry);
                current = rrj_read32(0x8005B404u);
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

typedef struct RRJCurveRoutes
{
    uint32_t boundary[8];
    uint32_t candidates[24];
    uint32_t directions[4];
} RRJCurveRoutes;

typedef uint32_t (*RRJCurveNeighbors)(uint32_t, uint32_t[8], uint32_t[24], uint32_t[3], uint32_t);

static void curve_copy_record(uint32_t destination[8], const uint32_t source[8])
{
    uint32_t index;
    for (index = 0; index < 8; ++index)
        rrj_put32((uint8_t *)destination + 4u * index, rrj_u32((const uint8_t *)source + 4u * index));
}

static int curve_endpoint(const uint32_t record[8], uint32_t piece)
{
    uint32_t index = curve_signed_half(piece);
    return !index || index == curve_signed_half(rrj_u32((const uint8_t *)record + 8) + 10) - 1u;
}

static uint32_t curve_neighbor(uint32_t record[8], RRJCurveRoutes *routes, uint32_t identity, uint32_t *direction, int resolve)
{
    RRJCurveNeighbors neighbors = rrj_s32(*direction) > 0 ? sub_80037A30 : sub_80037FBC;
    uint32_t count = neighbors(identity, record, routes->candidates, routes->directions, 3);
    if (resolve)
        return sub_80039048(identity, routes->candidates, routes->directions, count, record, direction);
    return count;
}

static uint32_t *curve_boundary(uint32_t source[8], RRJCurveRoutes *routes, uint32_t actor, uint32_t *direction)
{
    routes->directions[0] = *direction;
    curve_copy_record(routes->boundary, source);
    (void)curve_neighbor(routes->boundary, routes, actor + 172, direction, 1);
    return routes->boundary;
}

static uint32_t curve_project(uint32_t piece, uint32_t tangent_offset, uint32_t coefficient, uint32_t output[3])
{
    return sub_8002EAD8(rrj_at(piece + 20, 12), rrj_at(piece + tangent_offset, 6), coefficient, output);
}

static uint32_t curve_ratio(uint32_t numerator, uint32_t denominator)
{
    int numerator_positive = rrj_s32(numerator) > 0;
    int denominator_positive = rrj_s32(denominator) > 0;
    uint32_t result = sub_80010028(numerator_positive ? numerator : 0u - numerator,
                                 denominator_positive ? denominator : 0u - denominator);
    return numerator_positive != denominator_positive ? 0u - result : result;
}

static uint32_t curve_extrapolation_weight(uint32_t distance, uint32_t length, uint32_t endpoint[3], int right, uint32_t middle[3])
{
    uint32_t result;
    if (rrj_s32(length) > 0)
    {
        uint32_t blend_weight = 0u - sub_80010028(0, length);
        (void)sub_8002E6F8(middle, endpoint, endpoint, rrj_s32(0x10000u - blend_weight), rrj_s32(blend_weight));
        length = 0;
    }
    result = sub_80010028(0u - distance, 0u - length);
    return right ? 0x8000u - result : 0x8000u + result;
}

uint32_t sub_800386DC(uint32_t actor, uint32_t target_distance, uint32_t initial_distance, uint32_t input_direction, uint32_t input_record[8], uint32_t position_output[3], uint32_t copy_output[8])
{
    uint32_t *record = input_record;
    uint32_t *saved_record;
    uint32_t record_copy[8];
    RRJCurveRoutes routes;
    uint32_t first[3], middle[3], last[3];
    uint32_t *endpoint;
    uint32_t saved_piece = rrj_u32((const uint8_t *)input_record + 12);
    uint32_t original_piece = saved_piece;
    uint32_t direction = rrj_s32(input_direction) > 0 ? 1u : UINT32_MAX;
    uint32_t piece, next_piece, old_direction, length, distance;
    uint32_t old_coefficient, next_coefficient, coefficient, result, weight, component;
    uint32_t square, half, first_weight, middle_weight, first_product, middle_product;
    int smooth;

    FUNCTION_MARKER(0x800386DCu, "SLUS_010.53");
    distance = rrj_s32(input_direction) > 0 ? rrj_read32(original_piece + 32) - initial_distance : initial_distance;
    if (rrj_s32(distance) < rrj_s32(target_distance))
    {
        uint32_t identity = actor + 172;
        do
        {
            piece = record[3];
            if (!curve_endpoint(record, piece))
                record[3] = piece + 52u * direction;
            else
            {
                routes.directions[0] = direction;
                if (record == input_record)
                {
                    record = record_copy;
                    curve_copy_record(record, input_record);
                }
                result = sub_800394F0(record, direction);
                if (result)
                {
                    piece = record[3];
                    result = curve_project(piece, 14, rrj_s32(direction) > 0 ? target_distance - distance : distance - target_distance, position_output);
                    if (copy_output)
                    {
                        curve_copy_record(copy_output, record);
                        result = (uint32_t)(uintptr_t)copy_output;
                    }
                    return result;
                }
                (void)curve_neighbor(record, &routes, identity, &direction, 1);
            }
            piece = record[3];
            distance += rrj_read32(piece + 32);
        } while (rrj_s32(distance) < rrj_s32(target_distance));
    }
    saved_record = record;
    if (copy_output)
        curve_copy_record(copy_output, record);
    original_piece = record[3];
    old_direction = direction;
    length = rrj_read32(original_piece + 32);
    distance -= target_distance;
    if (rrj_s32(direction) > 0)
    {
        old_coefficient = curve_signed_byte(original_piece + 38) << 13;
        distance = length - distance;
    }
    else
        old_coefficient = curve_signed_byte(original_piece + 39) << 13;
    piece = record[3];
    direction = 1;
    if (curve_endpoint(record, piece))
        record = curve_boundary(saved_record, &routes, actor, &direction);
    else
        record[3] = piece + 52;
    next_piece = record[3];
    if (rrj_s32(direction) > 0)
        next_coefficient = curve_signed_byte(next_piece + (rrj_s32(old_direction) > 0 ? 38 : 39)) << 13;
    else
    {
        next_coefficient = curve_signed_byte(next_piece + (rrj_s32(old_direction) > 0 ? 39 : 38)) << 13;
        length += rrj_read32(next_piece + 32);
    }
    if (rrj_s32(distance) < (rrj_s32(length) >> 1))
    {
        (void)curve_project(next_piece, 2, next_coefficient, last);
        result = curve_project(original_piece, 2, old_coefficient, middle);
        endpoint = last;
        smooth = rrj_s32(distance) < 0;
        if (smooth)
        {
            weight = curve_extrapolation_weight(distance, length, endpoint, 0, middle);
            record = saved_record;
            record[3] = original_piece;
            direction = UINT32_MAX;
            if (curve_endpoint(record, original_piece))
                record = curve_boundary(saved_record, &routes, actor, &direction);
            else
                record[3] = original_piece - 52;
            if (rrj_s32(direction) < 0)
            {
                piece = record[3];
                coefficient = curve_signed_byte(piece + (rrj_s32(old_direction) > 0 ? 38 : 39)) << 13;
            }
            else
            {
                if (rrj_s32(curve_signed_half(record[2] + 10)) >= 2)
                    record[3] += 52;
                else
                    (void)curve_neighbor(record, &routes, actor + 172, &direction, 0);
                piece = record[3];
                component = r_u8(piece + 38);
                if (rrj_s32(direction ^ old_direction) < 0)
                    coefficient = (uint32_t)(rrj_s32(component << 24) >> 11);
                else
                    coefficient = curve_signed_byte(piece + 39) << 13;
            }
            result = curve_project(piece, 2, coefficient, first);
        }
        else
            weight = curve_ratio(distance, length);
    }
    else
    {
        (void)curve_project(original_piece, 2, old_coefficient, first);
        result = curve_project(next_piece, 2, next_coefficient, middle);
        distance = length - distance;
        endpoint = first;
        smooth = rrj_s32(distance) < 0;
        if (smooth)
        {
            uint32_t preceding_direction;
            weight = curve_extrapolation_weight(distance, length, endpoint, 1, middle);
            piece = record[3];
            saved_record = record;
            preceding_direction = direction;
            if (curve_endpoint(record, piece))
                record = curve_boundary(saved_record, &routes, actor, &direction);
            else
                record[3] = piece + 52u * preceding_direction;
            component = rrj_s32(preceding_direction) < 0 && rrj_s32(direction) > 0;
            piece = record[3] + 52u * component;
            coefficient = curve_signed_byte(piece + 38) << 13;
            result = curve_project(piece, 2, coefficient, last);
        }
        else
            weight = curve_ratio(distance, length);
    }
    if (!smooth)
        result = sub_8002E6F8(middle, endpoint, position_output, rrj_s32(0x10000u - weight), rrj_s32(weight));
    else
    {
        square = (uint32_t)sub_8001FC90(weight, weight);
        half = (uint32_t)(rrj_s32(square) >> 1);
        first_weight = half + 0x8000u - weight;
        middle_weight = weight - (square - 0x8000u);
        for (component = 0; component < 3; ++component)
        {
            first_product = (uint32_t)sub_8001FC90(first_weight, first[component]);
            middle_product = (uint32_t)sub_8001FC90(middle_weight, middle[component]);
            result = (uint32_t)sub_8001FC90(half, last[component]);
            rrj_put32((uint8_t *)position_output + 4u * component, first_product + middle_product + result);
        }
    }
    rrj_put32((uint8_t *)input_record + 12, saved_piece);
    return result;
}

uint32_t sub_8003A3CC(uint32_t arguments[8])
{
    FUNCTION_MARKER(0x8003A3CCu, "SLUS_010.53");
    uint32_t table, count, index = 0, random;
    if (curve_signed_half(0x800d6182) == UINT32_MAX)
        return UINT32_MAX;
    arguments[0] = rrj_read32(arguments[0] + 428);
    table = (uint32_t)sub_8003F408(arguments[0], arguments[1]);
    if (!table)
        return UINT32_MAX;
    count = rrj_read32(table + 16);
    if (rrj_s32(count) >= 2)
    {
        random = (uint32_t)sub_8001FC58();
        count = rrj_read32(table + 16);
        index = count ? random % count : random;
    }
    return rrj_read32(table + (index << 2) + 84);
}

uint32_t sub_8002C928(uint32_t incoming_sp, uint32_t arguments[8], RRJBodylessRegistersCall call)
{
    FUNCTION_MARKER(0x8002C928u, "SLUS_010.53");
    uint32_t frame = incoming_sp - 32, slot = arguments[0], point = arguments[1], x, y, offset;
    arguments[3] = 0x800d7fc8;
    arguments[2] = arguments[3] + (slot << 2);
    x = r_u16(point);
    offset = r_u16(arguments[2] + 32);
    arguments[3] = 0x800d7ff0;
    rrj_put16(rrj_at(frame + 16, 2), (uint16_t)(x + offset - 2));
    y = r_u16(point + 2);
    arguments[1] = r_u16(arguments[2] + 34);
    rrj_put16(rrj_at(frame + 20, 2), 5);
    rrj_put16(rrj_at(frame + 22, 2), 5);
    y += arguments[1] - 2;
    arguments[1] = arguments[3] + 52 * slot;
    arguments[0] = frame + 16;
    rrj_put16(rrj_at(frame + 18, 2), (uint16_t)y);
    return call(rrj_host_context(), 0x80048acc, arguments);
}

uint32_t sub_8002CC74(uint32_t incoming_sp, uint32_t arguments[8])
{
    FUNCTION_MARKER(0x8002CC74u, "SLUS_010.53");
    uint32_t frame = incoming_sp - 48, mode = rrj_read32(incoming_sp + 20);
    uint32_t first = arguments[0], second = arguments[1], rectangle = arguments[2], fourth = arguments[3];
    uint32_t extra = rrj_read32(incoming_sp + 16), x, width, result;
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
        rrj_write32(frame + 16, fourth);
        rrj_write32(frame + 20, extra);
    }
    else
    {
        arguments[1] = second;
        result = (uint32_t)sub_8002D0D8(arguments[0], arguments[1]);
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
        rrj_write32(frame + 16, fourth);
        rrj_write32(frame + 20, extra);
        if (mode == 2)
            width = (uint32_t)(rrj_s32(width << 16) >> 17);
        arguments[2] = (uint32_t)(int32_t)(int16_t)(uint16_t)(x + width - result);
    }
    arguments[4] = fourth;
    arguments[5] = extra;
    return (uint32_t)sub_8002CDC8(arguments[0], arguments[1], arguments[2], arguments[3], arguments[4], arguments[5]);
}
