#include "native_game_api.h"
#include "psx.h"
#include "race_pause.h"
#include "race_bodyless_batch_008.h"

uint32_t sub_8001A2E4(uint32_t identifier)
{
    uint32_t arguments[8] = {0};
    uint32_t mode = 2, index, attempt = 0, address, tag, entry, state, handle, current;
    FUNCTION_MARKER(0x8001A2E4, "SLUS_010.53");
    arguments[0] = r_u32(0x8005B42Cu);
    if (arguments[0])
    {
        arguments[0] = r_u32(arguments[0]);
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
        tag = r_u32(address);
        arguments[1] = tag;
        if ((tag & 15) < 2)
            w_u32(address, (tag & 3) | (mode << 4));
        if (r_u32(address) == (identifier & 255))
        {
            entry = 0x800d6aa0 + (index << 5);
            if (((uint32_t)r_u8(entry + 4) >> 4) != ((identifier & 255) >> 4))
                return index;
            state = r_u32(entry + 8);
            arguments[1] = state;
            if (state >= 3)
            {
                handle = r_u32(entry);
                current = r_u32(0x8005B404u);
                if (handle != current)
                    return index;
            }
            if (!state)
                return index;
        }
        ++index;
        if (((int32_t)(uint32_t)(index)) >= 9)
            index = 0;
        ++attempt;
        arguments[2] = index;
        arguments[3] = attempt;
    } while (((int32_t)(uint32_t)(attempt)) < 9);
    return UINT32_MAX;
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
        xport_store_le32((uint8_t *)destination + 4u * index, xport_load_le32((const uint8_t *)source + 4u * index));
}

static int curve_endpoint(const uint32_t record[8], uint32_t piece)
{
    uint32_t index = (uint32_t)(int32_t)r_s16(((uint32_t)(piece)));
    return !index || index == (uint32_t)(int32_t)r_s16(((uint32_t)(xport_load_le32((const uint8_t *)record + 8) + 10))) - 1u;
}

static uint32_t curve_neighbor(uint32_t record[8], RRJCurveRoutes *routes, uint32_t identity, uint32_t *direction, int resolve)
{
    RRJCurveNeighbors neighbors = ((int32_t)(uint32_t)(*direction)) > 0 ? sub_80037A30 : sub_80037FBC;
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
    return sub_8002EAD8(xport_guest_ptr(piece + 20, 12), xport_guest_ptr(piece + tangent_offset, 6), coefficient, output);
}

static uint32_t curve_ratio(uint32_t numerator, uint32_t denominator)
{
    int numerator_positive = ((int32_t)(uint32_t)(numerator)) > 0;
    int denominator_positive = ((int32_t)(uint32_t)(denominator)) > 0;
    uint32_t result = sub_80010028(numerator_positive ? numerator : 0u - numerator, denominator_positive ? denominator : 0u - denominator);
    return numerator_positive != denominator_positive ? 0u - result : result;
}

static uint32_t curve_extrapolation_weight(uint32_t distance, uint32_t length, uint32_t endpoint[3], int right, uint32_t middle[3])
{
    uint32_t result;
    if (((int32_t)(uint32_t)(length)) > 0)
    {
        uint32_t blend_weight = 0u - sub_80010028(0, length);
        (void)sub_8002E6F8(middle, endpoint, endpoint, ((int32_t)(uint32_t)(0x10000u - blend_weight)), ((int32_t)(uint32_t)(blend_weight)));
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
    uint32_t saved_piece = xport_load_le32((const uint8_t *)input_record + 12);
    uint32_t original_piece = saved_piece;
    uint32_t direction = ((int32_t)(uint32_t)(input_direction)) > 0 ? 1u : UINT32_MAX;
    uint32_t piece, next_piece, old_direction, length, distance;
    uint32_t old_coefficient, next_coefficient, coefficient, result, weight, component;
    uint32_t square, half, first_weight, middle_weight, first_product, middle_product;
    int smooth;

    FUNCTION_MARKER(0x800386DCu, "SLUS_010.53");
    distance = ((int32_t)(uint32_t)(input_direction)) > 0 ? r_u32(original_piece + 32) - initial_distance : initial_distance;
    if (((int32_t)(uint32_t)(distance)) < ((int32_t)(uint32_t)(target_distance)))
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
                    result = curve_project(piece, 14, ((int32_t)(uint32_t)(direction)) > 0 ? target_distance - distance : distance - target_distance, position_output);
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
            distance += r_u32(piece + 32);
        } while (((int32_t)(uint32_t)(distance)) < ((int32_t)(uint32_t)(target_distance)));
    }
    saved_record = record;
    if (copy_output)
        curve_copy_record(copy_output, record);
    original_piece = record[3];
    old_direction = direction;
    length = r_u32(original_piece + 32);
    distance -= target_distance;
    if (((int32_t)(uint32_t)(direction)) > 0)
    {
        old_coefficient = (uint32_t)(int32_t)r_s8(((uint32_t)(original_piece + 38))) << 13;
        distance = length - distance;
    }
    else
        old_coefficient = (uint32_t)(int32_t)r_s8(((uint32_t)(original_piece + 39))) << 13;
    piece = record[3];
    direction = 1;
    if (curve_endpoint(record, piece))
        record = curve_boundary(saved_record, &routes, actor, &direction);
    else
        record[3] = piece + 52;
    next_piece = record[3];
    if (((int32_t)(uint32_t)(direction)) > 0)
        next_coefficient = (uint32_t)(int32_t)r_s8(((uint32_t)(next_piece + (((int32_t)(uint32_t)(old_direction)) > 0 ? 38 : 39)))) << 13;
    else
    {
        next_coefficient = (uint32_t)(int32_t)r_s8(((uint32_t)(next_piece + (((int32_t)(uint32_t)(old_direction)) > 0 ? 39 : 38)))) << 13;
        length += r_u32(next_piece + 32);
    }
    if (((int32_t)(uint32_t)(distance)) < (((int32_t)(uint32_t)(length)) >> 1))
    {
        (void)curve_project(next_piece, 2, next_coefficient, last);
        result = curve_project(original_piece, 2, old_coefficient, middle);
        endpoint = last;
        smooth = ((int32_t)(uint32_t)(distance)) < 0;
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
            if (((int32_t)(uint32_t)(direction)) < 0)
            {
                piece = record[3];
                coefficient = (uint32_t)(int32_t)r_s8(((uint32_t)(piece + (((int32_t)(uint32_t)(old_direction)) > 0 ? 38 : 39)))) << 13;
            }
            else
            {
                if ((int32_t)r_s16(((uint32_t)(record[2] + 10))) >= 2)
                    record[3] += 52;
                else
                    (void)curve_neighbor(record, &routes, actor + 172, &direction, 0);
                piece = record[3];
                component = r_u8(piece + 38);
                if (((int32_t)(uint32_t)(direction ^ old_direction)) < 0)
                    coefficient = (uint32_t)(((int32_t)(uint32_t)(component << 24)) >> 11);
                else
                    coefficient = (uint32_t)(int32_t)r_s8(((uint32_t)(piece + 39))) << 13;
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
        smooth = ((int32_t)(uint32_t)(distance)) < 0;
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
            component = ((int32_t)(uint32_t)(preceding_direction)) < 0 && ((int32_t)(uint32_t)(direction)) > 0;
            piece = record[3] + 52u * component;
            coefficient = (uint32_t)(int32_t)r_s8(((uint32_t)(piece + 38))) << 13;
            result = curve_project(piece, 2, coefficient, last);
        }
        else
            weight = curve_ratio(distance, length);
    }
    if (!smooth)
        result = sub_8002E6F8(middle, endpoint, position_output, ((int32_t)(uint32_t)(0x10000u - weight)), ((int32_t)(uint32_t)(weight)));
    else
    {
        square = (uint32_t)sub_8001FC90(weight, weight);
        half = (uint32_t)(((int32_t)(uint32_t)(square)) >> 1);
        first_weight = half + 0x8000u - weight;
        middle_weight = weight - (square - 0x8000u);
        for (component = 0; component < 3; ++component)
        {
            first_product = (uint32_t)sub_8001FC90(first_weight, first[component]);
            middle_product = (uint32_t)sub_8001FC90(middle_weight, middle[component]);
            result = (uint32_t)sub_8001FC90(half, last[component]);
            xport_store_le32((uint8_t *)position_output + 4u * component, first_product + middle_product + result);
        }
    }
    xport_store_le32((uint8_t *)input_record + 12, saved_piece);
    return result;
}

uint32_t sub_8003A3CC(uint32_t actor, uint32_t id)
{
    FUNCTION_MARKER(0x8003A3CCu, "SLUS_010.53");
    uint32_t table, count, index = 0, random;
    if ((uint32_t)(int32_t)r_s16(((uint32_t)(0x800d6182))) == UINT32_MAX)
        return UINT32_MAX;
    actor = r_u32(actor + 428);
    table = (uint32_t)sub_8003F408(actor, id);
    if (!table)
        return UINT32_MAX;
    count = r_u32(table + 16);
    if (((int32_t)(uint32_t)(count)) >= 2)
    {
        random = (uint32_t)sub_8001FC58();
        count = r_u32(table + 16);
        index = count ? random % count : random;
    }
    return r_u32(table + (index << 2) + 84);
}

uint32_t sub_8002C928(uint32_t slot, const void *point)
{
    PSX_RECT rectangle;
    uint32_t offset = 0x800D7FC8u + 4u * slot;
    uint32_t x, y, adjustment;
    FUNCTION_MARKER(0x8002C928u, "SLUS_010.53");
    x = xport_load_le16(point);
    adjustment = r_u16(offset + 32u);
    xport_store_le16(&rectangle.x, (uint16_t)(x + adjustment - 2u));
    y = xport_load_le16((const uint8_t *)point + 2);
    adjustment = r_u16(offset + 34u);
    rectangle.w = 5;
    rectangle.h = 5;
    xport_store_le16(&rectangle.y, (uint16_t)(y + adjustment - 2u));
    return (uint32_t)StoreImage(&rectangle, (uint32 *)xport_guest_ptr(0x800D7FF0u + 52u * slot, 50));
}

uint32_t sub_8002CC74(uint32_t font, const uint8_t *text, const uint16_t rectangle[4], uint32_t link, uint32_t color, uint32_t mode)
{
    uint32_t x, y, width, text_width;
    FUNCTION_MARKER(0x8002CC74u, "SLUS_010.53");
    if (mode > 2)
        return 2;
    if (!mode)
    {
        x = (uint32_t)(int32_t)(int16_t)xport_load_le16(rectangle);
        y = (uint32_t)(int32_t)(int16_t)xport_load_le16(rectangle + 1);
    }
    else
    {
        text_width = sub_8002D0D8(font, text);
        if (mode == 1)
        {
            x = xport_load_le16(rectangle);
            width = xport_load_le16(rectangle + 2);
            y = (uint32_t)(int32_t)(int16_t)xport_load_le16(rectangle + 1);
        }
        else
        {
            width = xport_load_le16(rectangle + 2);
            x = xport_load_le16(rectangle);
            y = (uint32_t)(int32_t)(int16_t)xport_load_le16(rectangle + 1);
            text_width = (uint32_t)(((int32_t)(uint32_t)(text_width)) >> 1);
            width = (uint32_t)(((int32_t)(uint32_t)(width << 16)) >> 17);
        }
        x = (uint32_t)(int32_t)(int16_t)(uint16_t)(x + width - text_width);
    }
    return sub_8002CDC8(font, text, x, y, link, color);
}
