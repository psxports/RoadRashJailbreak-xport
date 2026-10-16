#include "rrj_debug.h"
#include "game_main.h"
#include "fixed_math.h"
#include "native_game_api.h"
#include "race_leaf.h"
#include "race_pause.h"
#include "race_leaf_batch_004.h"
#include "race_leaf_800AF3B0.h"
#include "race_audio_frontier.h"
#include "xport.h"
#include "psx.h"
#include "menu.h"
#include "screen.h"
#include "packet.h"
#include "resource.h"
#include "menu_input.h"
#include "audio.h"
#include "music_start.h"
#include "menu_item.h"
#include "entry_kind.h"
#include "resource_value.h"
#include "draft_quick.h"
#include "race_leaf_batch_005.h"
#include "race_leaf_batch_006.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

uint32_t sub_800A91AC(uint32_t actor_face, uint32_t other_face, uint32_t actor, uint32_t other, uint32_t translation[3])
{
    uint32_t point[3], vertex, axis, product_a, product_b, width, three_width, eighth;
    uint32_t magnitude, sign, combined_length, distance_word, result;
    int32_t distance, cross, threshold, edge;
    FUNCTION_MARKER(0x800A91AC, "RASHCDG.BIN");
    if (!(other_face & 0x200) || ((other_face & 255) != 3 && (other_face & 255) != 1))
        return other_face;
    vertex = actor + 12 * (actor_face & 255);
    for (axis = 0; axis < 3; ++axis)
        point[axis] = translation[axis] + r_u32(vertex + 196 + 4 * axis);
    product_a = (uint32_t)((int32_t)r_s16(actor + 450) * r_s16(other + 448));
    product_b = (uint32_t)((int32_t)r_s16(actor + 454) * r_s16(other + 444));
    cross = (int32_t)(product_a - product_b);
    distance = (int32_t)sub_800B6AAC(point, xport_guest_cptr(other + 432, 6), xport_guest_cptr(other + 184, 12));
    width = r_u32(other + 304);
    three_width = 3 * width;
    if ((int32_t)three_width < 0)
        three_width += 3;
    threshold = (int32_t)three_width >> 2;
    eighth = width;
    if ((int32_t)eighth < 0)
        eighth += 7;
    eighth = (uint32_t)((int32_t)eighth >> 3);
    if (distance > threshold && distance < (int32_t)(width + eighth) && cross > 0)
    {
        distance_word = (uint32_t)distance - (uint32_t)threshold;
        result = 0x202;
    }
    else if (distance < (int32_t)(0 - (uint32_t)threshold) && distance > (int32_t)(0 - width - eighth) && cross < 0)
    {
        distance_word = (uint32_t)distance + (uint32_t)threshold;
        result = 0x200;
    }
    else
        return other_face;
    (void)sub_8002EE50(distance_word, xport_guest_cptr(other + 432, 6), point);
    edge = (int32_t)sub_800B6AAC(xport_guest_cptr(actor + 468, 12), xport_guest_cptr(other + 444, 6), xport_guest_cptr(other + 468, 12));
    sign = (uint32_t)(edge >> 31);
    magnitude = ((uint32_t)edge + sign) ^ sign;
    combined_length = r_u32(other + 308) + r_u32(actor + 308) - 4096;
    if ((int32_t)magnitude < (int32_t)combined_length)
    {
        for (axis = 0; axis < 3; ++axis)
            translation[axis] = point[axis];
    }
    else
    {
        for (axis = 0; axis < 3; ++axis)
            translation[axis] += point[axis];
    }
    return result;
}

uint32_t sub_800B6B58(const uint32_t point[3], const uint16_t normal[3], const uint32_t origin[3])
{
    uint32_t first_delta, second_delta, first_component, second_component;
    int64_t first_product, second_product;
    FUNCTION_MARKER(0x800B6B58, "RASHCDG.BIN");
    first_component = (uint32_t)(int32_t)(int16_t)xport_load_le16(normal) << 4;
    first_delta = xport_load_le32(point) - xport_load_le32(origin);
    first_product = (int64_t)(int32_t)first_component * (int32_t)first_delta;
    second_component = (uint32_t)(int32_t)(int16_t)xport_load_le16(normal + 2) << 4;
    second_delta = xport_load_le32(point + 2) - xport_load_le32(origin + 2);
    second_product = (int64_t)(int32_t)second_component * (int32_t)second_delta;
    return (uint32_t)((uint64_t)first_product >> 16) + (uint32_t)((uint64_t)second_product >> 16);
}

uint32_t sub_800AF224(uint32_t actor, uint32_t surface, uint32_t actor_face, uint32_t surface_face, uint32_t translation[3])
{
    uint16_t normal[3] = {0, 0, 0};
    uint32_t mode = 6, options = 0, axis;
    int32_t dot;
    FUNCTION_MARKER(0x800AF224, "RASHCDG.BIN");
    if (actor_face)
    {
        if (actor_face & 0x100)
        {
            (void)sub_800B675C(xport_guest_cptr(surface + 24, 96), xport_guest_cptr(surface + 260, 18), surface_face & 255, normal, NULL);
            dot = (int32_t)sub_8002E698(xport_guest_cptr(actor + 450, 6), normal);
            mode = dot < -56754 ? 3 : dot > 56754 ? 1 : ((2 * (actor_face & 1)) ^ (actor_face & 2));
        }
        else
        {
            mode = actor_face & 255;
            if (surface_face & 0x200)
                (void)sub_800B675C(xport_guest_cptr(surface + 24, 96), xport_guest_cptr(surface + 260, 18), surface_face & 255, normal, NULL);
            else
            {
                (void)sub_800B675C(xport_guest_cptr(actor + 196, 96), xport_guest_cptr(actor + 516, 18), mode, normal, NULL);
                for (axis = 0; axis < 3; ++axis)
                    normal[axis] = (uint16_t)(0 - normal[axis]);
            }
        }
        options = 1;
    }
    else if (!(r_u32(actor + 564) & 0x8000) || r_u32(actor + 832) != surface)
        return actor_face;
    if (r_s32(surface + 140) > (int32_t)(r_u32(actor + 312) << 1))
        options |= 4;
    return sub_800AF3B0(actor, surface, normal, (int32_t)mode, options, translation);
}

uint32_t sub_800AC958(uint32_t actor, uint32_t other, uint32_t actor_face, uint32_t other_face, uint32_t translation[3])
{
    uint32_t mode = 6, alignment, magnitude, sign, axis, sound, flags, value, angle;
    uint16_t normal[3];
    uint32_t delta[3];
    int32_t dot, damage, cosine, sine;
    FUNCTION_MARKER(0x800AC958, "RASHCDG.BIN");
    /* Initialize the plane normal missing from the original non-box branch */
    for (axis = 0; axis < 3; ++axis)
        normal[axis] = r_u16(actor + 516 + 2 * axis);
    if (sub_800AF224(actor, other + 172, actor_face, other_face, translation))
    {
        if (other_face & 0x200)
        {
            (void)sub_800B675C(xport_guest_cptr(other + 196, 96), xport_guest_cptr(other + 432, 18), other_face & 255, normal, NULL);
            dot = (int32_t)sub_8002E698(xport_guest_cptr(actor + 450, 6), normal);
            mode = dot > 56754 ? 1 : dot < -56754 ? 3 : 2 * ((int32_t)r_s16(actor + 450) * (int16_t)normal[2] >= (int32_t)r_s16(actor + 454) * (int16_t)normal[0]);
        }
        else
            mode = 2 * ((int32_t)sub_800B6B58(xport_guest_cptr(other + 184, 12), xport_guest_cptr(actor + 516, 6), xport_guest_cptr(actor + 504, 12)) > 0);
        if (mode & 5)
        {
            alignment = (uint32_t)sub_8001FC90(r_s32(actor + 296), r_s32(other + 296));
            alignment += (uint32_t)sub_8001FC90(r_s32(actor + 300), r_s32(other + 300));
        }
        else
        {
            alignment = 0 - (uint32_t)sub_8001FC90(r_s32(actor + 296), (int32_t)(int16_t)normal[2] * 16);
            alignment -= (uint32_t)sub_8001FC90(r_s32(actor + 300), (int32_t)(int16_t)normal[0] * 16);
        }
        value = (uint32_t)sub_8001FC90((int32_t)alignment, (int32_t)(r_u32(actor + 480) - r_u32(other + 480)));
        sign = (uint32_t)((int32_t)value >> 31);
        magnitude = (value + sign) ^ sign;
        damage = (int32_t)sub_800A9408(actor, (int32_t)magnitude, 327680, 0x10000, (int32_t)mode);
        if (r_s32(actor + 480) > 65918 || r_s32(other + 480) > 65918)
        {
            sound = 48;
            if (damage < 3 && !(r_u32(actor + 564) & 0x8000))
                sound = damage < 2 ? ((sub_80043F00(0xF2000002) & 255) >> 6) + 50 : 18;
            (void)sub_80017BA0(r_s32(other + 184), r_s32(other + 192), sound, 0);
            if (r_u16(actor + 172) < r_u32(r_u32(0x8005B2F8) + 48) && r_u32(r_u32(actor + 852) + 604) < 2 && !r_u32(0x8005B220))
            {
                value = (r_u32(actor + 568) & 0x20D) ? r_u32(actor + 480) : magnitude;
                (void)sub_800B658C(actor, 0, (int32_t)value, 1464860, 1);
            }
        }
        if ((r_u32(actor + 568) & 0x20E) && (!(other_face & 0x200) || (other_face & 255) == 1 || (other_face & 255) == 3))
        {
            w_u32(other + 484, 0);
            if ((r_u32(actor + 568) & 2) && (other_face & 255) == 1)
            {
                value = r_u32(other + 480) - magnitude;
                w_u32(other + 480, value);
                if ((int32_t)value < 0)
                    value = 0;
                w_u32(other + 480, value);
            }
            else
            {
                value = r_u8(other + 509);
                w_u32(other + 480, 0);
                w_u8(other + 509, value | 16);
            }
        }
    }
    flags = r_u32(actor + 564);
    if ((flags & 0x28000) == 0x8000)
    {
        if (!r_u32(actor + 616) && r_u32(actor + 772))
        {
            if (flags & 0x40000)
            {
                if (r_u32(actor + 488))
                {
                    value = sub_8002E698(xport_guest_cptr(actor + 450, 6), xport_guest_cptr(other + 432, 6));
                    angle = sub_8002E698(xport_guest_cptr(actor + 450, 6), xport_guest_cptr(other + 444, 6));
                    w_u32(actor + 692, sub_80020018(value, angle));
                }
                else
                {
                    angle = r_u32(actor + 692) & 4095;
                    cosine = (int32_t)r_s16(0x8005624E + 4 * angle) * 16;
                    sine = (int32_t)r_s16(0x8005624C + 4 * angle) * 16;
                    (void)sub_8002EB78(xport_guest_cptr(other + 444, 6), xport_guest_cptr(other + 432, 6), xport_guest_ptr(actor + 528, 6), (uint32_t)cosine, (uint32_t)sine);
                    xport_gte_write_control(0, (uint32_t)(int32_t)r_s16(actor + 522));
                    xport_gte_write_control(2, (uint32_t)(int32_t)r_s16(actor + 524));
                    xport_gte_write_control(4, (uint32_t)(int32_t)r_s16(actor + 526));
                    for (axis = 0; axis < 3; ++axis)
                        xport_gte_write_data(9 + axis, (uint32_t)(int32_t)r_s16(actor + 528 + 2 * axis));
                    xport_gte_execute(0x178000C);
                    for (axis = 0; axis < 3; ++axis)
                        w_u16(actor + 516 + 2 * axis, xport_gte_read_data(9 + axis));
                    (void)sub_8002E468(xport_guest_ptr(actor + 516, 6));
                }
            }
            else
            {
                dot = (int32_t)sub_8002E698(xport_guest_cptr(other + 450, 6), xport_guest_cptr(actor + 450, 6));
                value = r_u32(actor + 576) - (uint32_t)sub_8001FC90(r_s32(other + 480), dot);
                w_u32(actor + 576, value);
                if ((int32_t)value < 146486)
                    value = 146486;
                w_u32(actor + 576, value);
                w_u32(actor + 480, value);
                for (axis = 0; axis < 3; ++axis)
                    delta[axis] = r_u32(actor + 504 + 4 * axis) - r_u32(other + 184 + 4 * axis);
                for (angle = 0; angle < 2; ++angle)
                {
                    uint32_t projection = 0;
                    for (axis = 0; axis < 3; ++axis)
                    {
                        int64_t product = (int64_t)(int32_t)delta[axis] * ((int32_t)r_s16(other + 432 + 12 * angle + 2 * axis) * 16);
                        projection += (uint32_t)((uint64_t)product >> 16);
                    }
                    w_u32(actor + 776 + 4 * angle, projection);
                }
                value = sub_8002E698(xport_guest_cptr(actor + 450, 6), xport_guest_cptr(other + 432, 6));
                angle = sub_8002E698(xport_guest_cptr(actor + 450, 6), xport_guest_cptr(other + 444, 6));
                w_u32(actor + 692, sub_80020018(value, angle));
                w_u32(actor + 564, r_u32(actor + 564) | 0x40000);
            }
        }
        else if (r_u32(actor + 564) & 0x40000)
        {
            dot = (int32_t)sub_8002E698(xport_guest_cptr(other + 450, 6), xport_guest_cptr(actor + 450, 6));
            value = r_u32(actor + 576) + (uint32_t)sub_8001FC90(r_s32(other + 480), dot);
            w_u32(actor + 576, value);
            if ((int32_t)value < 146486)
                value = 146486;
            w_u32(actor + 576, value);
            w_u32(actor + 480, value);
            (void)sub_8002EE50(value, xport_guest_cptr(actor + 450, 6), xport_guest_ptr(actor + 456, 12));
            w_u32(actor + 564, r_u32(actor + 564) & 0xFFFBFFFF);
        }
    }
    value = ((r_u32(actor + 36) >> 25) & 3) < 2;
    if (!value)
        return value;
    if (!mode)
        return sub_80027258(actor, 4);
    if (mode == 2)
        return sub_80027258(actor, 3);
    return 2;
}

uint32_t sub_800AC5BC(uint32_t actor, uint32_t other)
{
    uint32_t forced = 0, response[3] = {0, 0, 0}, actor_face = 0, other_face = 0, power = 0;
    uint32_t movable, linked, result, allow_adjust = 0, value, axis, count, entry;
    uint32_t displacement[3];
    int rotate;
    FUNCTION_MARKER(0x800AC5BC, "RASHCDG.BIN");
    if (r_u32(actor + 832) == other + 172)
        forced = r_u8(actor + 565) >> 7;
    if (!(r_u32(actor + 568) & 1))
    {
        result = r_u32(actor + 828);
        if (result == other + 172 && !forced)
            return result;
    }
    rotate = sub_800A8FE8(actor) != 0;
    if (sub_800A8C78(actor, 0x140000, 0x1C0000))
    {
        {
            uint8_t *native_owner = sub_800AAD30(actor, xport_guest_ptr(other, 1), &actor_face, &other_face, response, &power);
            movable = native_owner ? 0x80000000 + (uint32_t)((uintptr_t)native_owner - (uintptr_t)DRAM) : 0;
        }
        result = movable;
        allow_adjust = (actor_face >> 8) & 1;
        if (movable)
        {
            linked = r_u32(actor + 856);
            result = linked;
            if (linked)
            {
                uint32_t identity = r_u16(movable + 172);
                int equal_sides = (actor_face & 1) == ((actor_face & 2) >> 1);
                allow_adjust &= (identity == r_u16(linked + 172) && !equal_sides) || (identity == r_u16(actor + 172) && (actor_face & 255) < 8 && equal_sides);
                if (movable == r_u32(actor + 856))
                    movable = actor;
            }
        }
    }
    else
    {
        if (rotate)
        {
            value = sub_80020018((uint32_t)((int32_t)r_s16(actor + 450) * 16), (uint32_t)((int32_t)r_s16(actor + 454) * 16));
            w_u32(actor + 292, value);
            w_u32(actor + 296, (uint32_t)((int32_t)r_s16(0x8005624E + 4 * (value & 4095)) * 16));
            value = r_u32(actor + 292);
            w_u32(actor + 300, (uint32_t)((int32_t)r_s16(0x8005624C + 4 * (value & 4095)) * 16));
        }
        movable = (sub_800AA140(actor, xport_guest_cptr(other, 512), &actor_face, &other_face, response) ? other : 0);
        result = movable;
        allow_adjust = 0;
    }
    if (!actor_face && !forced)
        return result;
    if (forced)
    {
        for (axis = 0; axis < 3; ++axis)
            response[axis] = 0;
    }
    else
    {
        if (allow_adjust)
            other_face = sub_800A91AC(actor_face, other_face, actor, other, response);
        if (movable == other)
            for (axis = 0; axis < 3; ++axis)
                response[axis] = 0 - response[axis];
    }
    if ((int32_t)power > 0)
    {
        value = (uint32_t)(sub_8001FC90(r_s32(other + 480), (int32_t)power));
        (void)sub_8002EE50(value, xport_guest_cptr(other + 450, 6), displacement);
        (void)sub_800A8DF0(xport_guest_ptr(actor, 1), displacement, 1);
        (void)sub_800A8DF0(xport_guest_ptr(other, 1), displacement, 1);
        value = r_u32(actor + 552);
        w_u32(actor + 552, power < value ? value : power);
    }
    if (forced || !(r_u16(actor + 320) & 4) || r_s32(0x800CCF68) >= 8)
        return sub_800AC958(actor, other, actor_face, other_face, response);
    count = r_u32(0x800CCF68);
    if (count >= 8)
    {
        RRJ_DEBUG_FPUTS("Actor contact queue index is outside its table\n", stderr);
        abort();
    }
    entry = 0x800CCE48 + 36 * count;
    w_u16(entry, r_u16(actor + 172));
    w_u16(entry + 2, r_u16(other + 172));
    w_u32(entry + 16, response[0]);
    w_u32(0x800CCF68, count + 1);
    w_u32(entry + 20, response[1]);
    result = actor_face | (other_face << 16);
    w_u32(entry + 28, result);
    w_u32(entry + 24, response[2]);
    return result;
}

uint32_t sub_800B11B4(uint32_t actor, uint32_t surface, uint32_t actor_face, uint32_t surface_face, uint32_t translation[3], uint16_t normal[3])
{
    uint32_t result, mode, magnitude, sign;
    int32_t dot;
    FUNCTION_MARKER(0x800B11B4, "RASHCDG.BIN");
    result = sub_800AF224(actor, surface, actor_face, surface_face, translation);
    if (!result)
        return result;
    dot = (int32_t)sub_8002E698(xport_guest_cptr(actor + 450, 6), normal);
    if (dot > 56754)
        mode = 1;
    else if (dot < -56754)
        mode = 3;
    else
        mode = 2 * ((int32_t)r_s16(actor + 450) * (int16_t)normal[2] >= (int32_t)r_s16(actor + 454) * (int16_t)normal[0]);
    result = (uint32_t)(sub_8001FC90((int32_t)(0 - (uint32_t)dot), r_s32(actor + 480)));
    sign = (uint32_t)((int32_t)result >> 31);
    magnitude = (result + sign) ^ sign;
    return sub_800A9408(actor, (int32_t)magnitude, r_s32(surface + 144), r_s32(actor + 316), (int32_t)mode);
}

uint32_t sub_800B0D8C(uint32_t actor, uint32_t surface)
{
    uint32_t forced = 0, linked, excluded, face, depth = 0, vertex, contact, follow;
    uint32_t translation[3], axis, count, entry, result, flags;
    uint16_t normal[3];
    int32_t ratio = 0, approach, projected;
    FUNCTION_MARKER(0x800B0D8C, "RASHCDG.BIN");
    if ((r_u16(surface) >> 5) == 6 && r_s32(surface + 140) > 0x20000 && r_u32(surface + 8) != 1)
        (void)sub_800B2C98(actor, surface);
    if (r_u32(actor + 832) == surface)
        forced = r_u8(actor + 565) >> 7;
    result = r_u32(actor + 828);
    if (result == surface && !forced)
        return result;
    (void)sub_800A8FE8(actor);
    excluded = (r_u32(actor + 568) & 0x600) ? 16 : 48;
    for (;;)
    {
        vertex = sub_800B7030(xport_guest_cptr(actor + 196, 96), xport_guest_cptr(surface + 260, 18), xport_guest_cptr(surface + 24, 96), excluded, &face, &depth);
        contact = vertex == 8 ? 0 : vertex | 0x100;
        linked = r_u32(actor + 856);
        follow = linked && r_u32(actor + 1088) && !forced && (vertex == 8 || (vertex & 1) != ((vertex & 2) >> 1));
        if (contact)
        {
            (void)sub_800B675C(xport_guest_cptr(surface + 24, 96), xport_guest_cptr(surface + 260, 18), face, normal, NULL);
            (void)sub_8002EE50(depth + 0x2000, normal, translation);
            if (r_u16(actor + 172) < r_u32(r_u32(0x8005B2F8) + 48) && r_s32(actor + 480) >= 132)
            {
                approach = (int32_t)(0 - sub_8002E698(normal, xport_guest_cptr(actor + 450, 6)));
                if (approach > 0)
                {
                    projected = (int32_t)sub_8001FC90(approach, r_s32(actor + 480));
                    if (projected >= 132 && (int32_t)depth < projected)
                    {
                        if ((int32_t)depth > 0)
                            ratio = (int32_t)sub_80010028(depth, (uint32_t)projected);
                        else
                            ratio = (int32_t)(0 - sub_80010028(0 - depth, (uint32_t)projected));
                    }
                    if (ratio >= r_s32(0x800CCE38))
                        ratio = r_s32(0x800CCE38);
                    flags = r_u32(actor + 552);
                    w_u32(actor + 552, (uint32_t)ratio < flags ? flags : (uint32_t)ratio);
                }
            }
        }
        if (forced)
        {
            for (axis = 0; axis < 3; ++axis)
            {
                translation[axis] = 0;
                normal[axis] = 0;
            }
        }
        result = 1;
        if (contact || forced)
        {
            if (!forced && ((r_u16(actor + 320) & 4) || r_u32(actor + 856)) && r_s32(0x800CCF68) < 8)
            {
                count = r_u32(0x800CCF68);
                if (count >= 8)
                {
                    RRJ_DEBUG_FPUTS("Large bounds contact queue index is outside its table\n", stderr);
                    abort();
                }
                linked = r_u32(actor + 856);
                /* Keep the primary actor when the original redirect has no link */
                if (!r_u32(actor + 1088) && linked)
                {
                    follow = 2;
                    actor = linked;
                }
                entry = 0x800CCE48 + 36 * count;
                w_u16(entry, r_u16(actor + 172));
                w_u16(entry + 2, r_u16(surface));
                for (axis = 0; axis < 3; ++axis)
                    w_u16(entry + 4 + 2 * axis, normal[axis]);
                w_u32(entry + 16, translation[0]);
                w_u32(entry + 20, translation[1]);
                w_u32(entry + 28, contact | ((face | 0x200) << 16));
                w_u32(entry + 32, depth);
                w_u32(entry + 24, translation[2]);
                if (follow == 2)
                {
                    result = sub_800B59F0(entry);
                    w_u32(0x800CCF68, r_u32(0x800CCF68) + result);
                }
                else
                    w_u32(0x800CCF68, count + 1);
                result = 1;
            }
            else
            {
                follow = 0;
                (void)sub_800B11B4(actor, surface, contact, face | 0x200, translation, normal);
            }
        }
        if (follow != 1)
            return result;
        actor = r_u32(actor + 856);
    }
}

uint32_t sub_80013F8C(void)
{
    FUNCTION_MARKER(0x80013F8Cu, "SLUS_010.53");
    uint32_t p, result = 0x800D0000u;
    if (r_u32(0x8005AD24u))
    {
        int32_t half;
        p = r_u32(0x8005B474u);
        half = r_s16(p + 4);
        w_u16(0x800D5EECu, (uint16_t)(half / 64 + 1));
        w_u16(0x800D5EEEu, (uint16_t)(r_u16(p) + (half >> 1)));
        result = (uint32_t)((int32_t)r_s16(p + 12) >> 1);
        w_u16(0x800D5EF0u, (uint16_t)(r_u16(p + 8) + result));
    }
    return result;
}



uint32_t sub_8006C354(void)
{
    uint32_t flags;
    FUNCTION_MARKER(0x8006C354, "RASHCDF.BIN");
    w_u32(0x8005ACAC, 0);
    flags = r_u16(0x8009C5DA);
    if (flags & 1)
    {
        w_u16(0x8009C5DA, flags | 4);
        w_u8(0x8009C5E2, 1);
    }
    else
    {
        w_u8(0x8009C5E2, 0);
        w_u8(r_u32(0x8005B2F8), 3);
    }
    return 0;
}

uint32_t sub_8006E008(void)
{
    char path[128];
    uint8_t frame_count_bytes[4], frames[800];
    uint32_t success = 1, image = 0, descriptor, frame_bytes, queue, context, packet, rectangle;
    int32_t selected, frame_count, path_length;
    PSX_RECT area;
    FUNCTION_MARKER(0x8006E008, "RASHCDF.BIN");
    if (r_s8(0x8009C5E2))
        return success;
    (void)sub_80078EF4();
    if (r_u32(0x800D80D8) & 16)
    {
        path_length = snprintf(path, sizeof(path), "%s%s", (const char *)xport_guest_cptr(0x8005248C, 1), "ssload.tcm");
        selected = r_s8(0x800D80E0);
        if (selected >= 38)
            selected -= 19;
        --selected;
    }
    else
    {
        path_length = snprintf(path, sizeof(path), "%s%s", (const char *)xport_guest_cptr(0x8005248C, 1), "fsload.tcm");
        selected = (r_u32(0x8005B220) ? r_s8(0x800D80E7) : r_s8(0x800D80E0)) - 1;
    }
    if (path_length < 0 || path_length >= (int32_t)sizeof(path))
    {
        RRJ_DEBUG_FPUTS("RRJ loading screen path exceeds its buffer\n", stderr);
        abort();
    }
    descriptor = sub_8001458C(path, r_u32(0x8005AD5C));
    if ((int32_t)descriptor < 0)
        success = 0;
    if (success)
    {
        if (sub_80014780(descriptor, frame_count_bytes, 4) != 4)
        {
            (void)sub_8001460C(descriptor);
            success = 0;
        }
        else
        {
            frame_count = (int32_t)xport_load_le32(frame_count_bytes);
            /* Reject the original off-by-one and negative frame-count defects */
            if (frame_count < 0 || frame_count >= 101 || selected < 0 || selected >= frame_count)
            {
                RRJ_DEBUG_FPRINTF(stderr, "Invalid loading frame: requested=%d count=%d\n", selected, frame_count);
                (void)sub_8001460C(descriptor);
                success = 0;
            }
            else
            {
                uint32_t table_bytes = (uint32_t)frame_count * 8;
                if (sub_80014780(descriptor, frames, table_bytes) != table_bytes)
                {
                    (void)sub_8001460C(descriptor);
                    success = 0;
                }
                else
                {
                    frame_bytes = xport_load_le32(frames + 8 * selected);
                    (void)sub_80014634(descriptor, xport_load_le32(frames + 8 * selected + 4), 0);
                    image = sub_8001447C(frame_bytes, 1);
                    if (!image)
                        success = 0;
                    else if (sub_80014780(descriptor, xport_guest_ptr(image, frame_bytes), frame_bytes) != frame_bytes)
                        success = 0;
                    (void)sub_8001460C(descriptor);
                }
            }
        }
    }
    (void)DrawSync(0);
    (void)VSync(0);
    (void)sub_80080D08();
    (void)sub_8001BE08(0, 0, 384, 240);
    (void)sub_8001C3F4();
    (void)sub_8001C408();
    context = r_u32(0x8005B470);
    rectangle = context + 112 * r_u8(context + 5) + 16;
    area.x = r_s16(rectangle);
    area.y = r_s16(rectangle + 2);
    area.w = 384;
    area.h = 240;
    packet = r_u32(context + 268);
    if (packet + 12 >= r_u32(0x8005B4D0))
    {
        packet = sub_80021C98(packet, 12);
        w_u32(r_u32(0x8005B470) + 268, packet);
    }
    context = r_u32(0x8005B470);
    packet = r_u32(context + 268);
    w_u32(context + 268, packet + 12);
    SetDrawArea(xport_guest_ptr(packet, 12), &area);
    DrawPrim(xport_guest_ptr(packet, 12));
    if (success)
    {
        queue = r_u32(0x8009C2F0);
        /* Keep corrupted queue counts outside the fixed six-entry array */
        if ((int32_t)queue >= 0 && (int32_t)queue < 6)
        {
            uint32_t entry = 0x8009C2F8 + 36 * queue;
            w_u32(entry + 8, 3);
            w_u32(entry, image);
            w_u32(entry + 12, 1);
            w_u32(entry + 16, 0);
            w_u32(entry + 28, 0);
            w_u32(entry + 32, 0);
            w_u32(0x8009C2F0, queue + 1);
        }
        queue = r_u32(0x8009C2F0);
        if (queue)
            sub_80062774(0x8009C2F8, queue);
        w_u32(0x8009C2F0, 0);
        (void)DrawSync(0);
        (void)VSync(0);
    }
    if (image)
        (void)sub_800144B8(image);
    return success;
}


uint32_t sub_8006460C(uint32_t resource)
{
    int32_t previous = r_s8(resource + 3), selected = previous - 1;
    uint32_t result;
    FUNCTION_MARKER(0x8006460C, "RASHCDF.BIN");
    /* Reject invalid resource ranges before the original circular scan */
    if (r_s8(resource) <= 0 || previous < 0 || previous >= r_s8(resource))
    {
        RRJ_DEBUG_FPUTS("RRJ resource selection has an invalid range\n", stderr);
        abort();
    }
    if (selected < 0)
        selected = r_s8(resource) - 1;
    if (selected != previous)
    {
        do
        {
            uint32_t record = r_u32(resource + 4) + 12 * (uint32_t)selected;
            uint32_t flags, bit;
            int32_t kind;
            if (!(r_u8(record + 3) & r_u8(0x8009C5E4)))
                goto next_8006460C;
            kind = r_s8(record);
            if (kind == 4 || kind == 8)
            {
                bit = r_u8(record + 1);
                if ((r_u8(0x800D80D8 + (kind == 4 ? 240 : 252) + (uint32_t)(r_s8(record + 1) >> 3)) >> (bit & 7)) & 1)
                    goto next_8006460C;
            }
            flags = r_u8(record + 2);
            if ((flags & 64) && !r_u32(0x800D7138 + 192 * (uint32_t)r_s8(0x8009C5E6)))
                goto next_8006460C;
            if ((flags & 32) && r_u32(0x800D7138 + 192 * (uint32_t)r_s8(0x8009C5E6)))
                goto next_8006460C;
            if ((flags & 128) && (r_u8(0x800D70E1) >> 4) != 8)
                goto next_8006460C;
            break;
        next_8006460C:
            selected = selected - 1;
            if (selected < 0)
        selected = r_s8(resource) - 1;
        } while (selected != previous);
    }
    result = sub_8007EAC0(selected == r_s8(resource + 3) ? 4 : 1);
    w_u8(resource + 3, (uint32_t)selected);
    return result;
}


uint32_t sub_800647E4(uint32_t resource)
{
    int32_t previous = r_s8(resource + 3), selected = previous + 1;
    uint32_t result;
    FUNCTION_MARKER(0x800647E4, "RASHCDF.BIN");
    /* Reject invalid resource ranges before the original circular scan */
    if (r_s8(resource) <= 0 || previous < 0 || previous >= r_s8(resource))
    {
        RRJ_DEBUG_FPUTS("RRJ resource selection has an invalid range\n", stderr);
        abort();
    }
    if (selected >= r_s8(resource))
        selected = 0;
    if (selected != previous)
    {
        do
        {
            uint32_t record = r_u32(resource + 4) + 12 * (uint32_t)selected;
            uint32_t flags, bit;
            int32_t kind;
            if (!(r_u8(record + 3) & r_u8(0x8009C5E4)))
                goto next_800647E4;
            kind = r_s8(record);
            if (kind == 4 || kind == 8)
            {
                bit = r_u8(record + 1);
                if ((r_u8(0x800D80D8 + (kind == 4 ? 240 : 252) + (uint32_t)(r_s8(record + 1) >> 3)) >> (bit & 7)) & 1)
                    goto next_800647E4;
            }
            flags = r_u8(record + 2);
            if ((flags & 64) && !r_u32(0x800D7138 + 192 * (uint32_t)r_s8(0x8009C5E6)))
                goto next_800647E4;
            if ((flags & 32) && r_u32(0x800D7138 + 192 * (uint32_t)r_s8(0x8009C5E6)))
                goto next_800647E4;
            if ((flags & 128) && (r_u8(0x800D70E1) >> 4) != 8)
                goto next_800647E4;
            break;
        next_800647E4:
            selected = selected + 1;
            if (selected >= r_s8(resource))
        selected = 0;
        } while (selected != previous);
    }
    result = sub_8007EAC0(selected == r_s8(resource + 3) ? 4 : 1);
    w_u8(resource + 3, (uint32_t)selected);
    return result;
}


uint32_t sub_8006B800(uint32_t menu, uint32_t resource, int32_t first, int32_t end)
{
    uint32_t handled = 0, table, pad, target, entry, selector, event;
    int32_t player, kind;
    FUNCTION_MARKER(0x8006B800, "RASHCDF.BIN");
    if (!resource)
        return 0;
    if (!r_s8(0x8009C5E5))
    {
        for (player = 0; player < 2 && !handled; ++player)
        {
            pad = 0x800D7128 + 192 * (uint32_t)player;
            if (r_s8(pad + 74) <= 0)
                continue;
            handled = 2;
            table = 0x8009C7B8 + 4 * (uint32_t)r_s16(0x8009C5D0);
            if (r_u32(table) == 0xFFFFFFFF)
                (void)sub_8007EAC0(4);
            else if (!(r_u16(menu) & 0x400) || !r_s8(0x800D80F0))
            {
                w_u16(0x8009C5D2, r_u16(table));
                (void)sub_8007EAC0(3);
                handled = 1;
                w_u8(0x8009C5E3, r_u8(0x8009C5E3) | 4);
            }
            else
            {
                target = r_u8(r_u32(0x8005B2F8) + 4) == 4 ? 55 : 54;
                w_u16(0x8009C5D2, target);
                w_u8(0x8009C5E8, r_u8(table));
                table = r_u32(0x8009C68C);
                w_u16(r_u32(table + 4 * target) + 10, r_u16(0x8009C5D0));
                w_u8(r_u32(table + 4 * target) + 14, r_u8(menu + 14));
                w_u8(r_u32(r_u32(0x8009C68C) + 4 * target) + 15, r_u8(menu + 15));
                handled = 1;
                w_u8(0x8009C5E3, r_u8(0x8009C5E3) | 4);
                (void)sub_8007EAC0(3);
            }
        }
    }
    for (player = first; player < end && !handled; ++player)
    {
        pad = 0x800D7128 + 192 * (uint32_t)player;
        if (r_s8(pad + 26))
        {
            (void)sub_8006460C(resource);
            handled = 2;
            w_u8(0x8009C5E3, r_u8(0x8009C5E3) | 64);
        }
        else if (r_s8(pad + 34))
        {
            (void)sub_800647E4(resource);
            handled = 2;
            w_u8(0x8009C5E3, r_u8(0x8009C5E3) | 128);
        }
        else if (!r_s8(0x8009C5E5))
        {
            if (r_s8(pad + 82) > 0)
            {
                w_u8(0x8009C5E3, r_u8(0x8009C5E3) | 1);
                entry = r_u32(menu + 16) + 120 * (uint32_t)r_s16(menu + 4);
                kind = r_s16(entry + 8);
                handled = 2;
                event = 4;
                if (kind >= 12 && kind < 14)
                {
                    selector = r_u16(entry + 18);
                    table = 0x8009CAF8 + 4 * selector;
                    if (r_u32(table) != 0xFFFFFFFF)
                    {
                        w_u16(0x8009C5D2, r_u16(table));
                        handled = 1;
                        event = 2;
                    }
                    else
                    {
                        table = 0x8009C9B0 + 4 * selector;
                        if (r_u32(table) != 0xFFFFFFFF)
                        {
                            w_u16(menu + 4, sub_8006C700(menu, (uint32_t)r_s16(table)));
                            event = 2;
                        }
                        else
                        {
                            target = 0;
                            switch (selector)
                            {
                                case 35: target = 37; break;
                                case 37: target = 34; break;
                                case 41: target = 43; break;
                                case 43:
                                    if ((r_s8(0x800D81DF) < 18) == (r_s8(0x800D8203) < 18))
                                    {
                                        target = 41;
                                        event = 4;
                                    }
                                    else
                                        target = 40;
                                    break;
                                case 50: target = 53; break;
                                case 53: target = 49; break;
                                default: break;
                            }
                            if (target)
                            {
                                w_u16(menu + 4, sub_8006C700(menu, target));
                                if (selector != 43 || target != 41)
                                    event = 2;
                            }
                        }
                    }
                }
                else if (kind == 17)
                {
                    table = 0x8009C8A8 + 4 * r_u16(entry + 100);
                    if (r_u32(table) != 0xFFFFFFFF)
                    {
                        w_u16(menu + 4, sub_8006C700(menu, (uint32_t)r_s16(table)));
                        event = 2;
                    }
                }
                (void)sub_8007EAC0(event);
            }
            else if (r_s8(pad + 74) > 0)
            {
                handled = 2;
                table = 0x8009C7B8 + 4 * (uint32_t)r_s16(0x8009C5D0);
                if (r_u32(table) == 0xFFFFFFFF)
                    (void)sub_8007EAC0(4);
                else
                {
                    w_u16(0x8009C5D2, r_u16(table));
                    (void)sub_8007EAC0(3);
                    handled = 1;
                    w_u8(0x8009C5E3, r_u8(0x8009C5E3) | 4);
                }
            }
            else if (r_s8(pad + 58) > 0)
            {
                handled = 2;
                if (r_u16(menu) & 0x100)
                {
                    if (!r_s8(0x8009C5EE))
                        (void)sub_8007EAC0(4);
                    else if (r_s8(0x8009C5DF))
                    {
                        (void)sub_8007EDE0(r_u8(0x8009C5DE));
                        w_u8(0x8009C5E0, 1);
                    }
                }
                else
                {
                    target = r_u8(0x8009C5D0);
                    w_u8(0x800D80E8, target);
                    table = r_u32(0x8009C68C) + 4 * (uint32_t)(int32_t)(int8_t)target;
                    target = r_u8(r_u32(table) + 4);
                    w_u16(0x8009C5D2, 42);
                    w_u8(0x800D80E5, target);
                    (void)sub_8007EAC0(2);
                    handled = 1;
                    w_u8(0x8009C5E3, r_u8(0x8009C5E3) | 8);
                }
            }
            else if (r_s8(pad + 66) > 0)
            {
                handled = 2;
                if (r_u16(menu) & 0x200)
                    (void)sub_8007EAC0(4);
                else
                {
                    (void)sub_8007EAC0(2);
                    w_u16(0x8009C5D2, 53);
                    table = r_u32(0x8009C68C);
                    w_u8(0x8009C5E3, r_u8(0x8009C5E3) | 2);
                    w_u16(r_u32(table + 212) + 10, r_u16(menu + 6));
                    w_u8(r_u32(table + 212) + 14, r_u8(menu + 14));
                    w_u8(r_u32(r_u32(0x8009C68C) + 212) + 15, r_u8(menu + 15));
                }
            }
        }
    }
    return handled;
}

uint32_t sub_8006B7BC(uint32_t menu)
{
    int32_t first, end;
    uint32_t result;
    FUNCTION_MARKER(0x8006B7BC, "RASHCDF.BIN");
    w_u8(0x8009C5E5, 1);
    first = r_s8(menu + 14);
    end = first + r_s8(menu + 15);
    result = sub_8006B800(menu, r_u32(0x8009C664), first, end);
    w_u8(0x8009C5E5, 0);
    return result;
}


uint32_t sub_8006F0B8(uint32_t menu, uint32_t entry)
{
    uint32_t rectangle = entry + 16, font_record, context, tiles, lines, row, parameters, state;
    uint32_t ot, font_ot, slot, y, x, upper_y, current, desired, lower, upper, numerator, width;
    int32_t character, divisor;
    FUNCTION_MARKER(0x8006F0B8, "RASHCDF.BIN");
    slot = sub_80072100(r_u32(menu + 16) + 120 * (uint32_t)r_s16(menu + 4));
    if (slot == 4)
        character = r_s8(0x800D81DF);
    else if (slot == 5)
        character = r_s8(0x800D8203);
    else
        return 1;
    font_record = 0x800D8078 + 24 * r_u32(0x8009C5B8);
    if (!r_s8(font_record))
        return 1;
    w_u8(font_record + 3, 0);
    y = r_u16(rectangle);
    slot = 4 * (uint32_t)r_s8(0x8009C5E1);
    ot = r_u32(0x8009CFC8) + slot + 20;
    font_ot = r_u32(0x8009CFC8) + slot + 4;
    context = r_u32(0x8005B470);
    tiles = r_u32(context + 268);
    if (tiles + 96 >= r_u32(0x8005B4D0))
    {
        tiles = sub_80021C98(tiles, 96);
        w_u32(r_u32(0x8005B470) + 268, tiles);
    }
    context = r_u32(0x8005B470);
    tiles = r_u32(context + 268);
    w_u32(context + 268, tiles + 96);
    if (tiles + 192 >= r_u32(0x8005B4D0))
    {
        lines = sub_80021C98(tiles + 96, 96);
        w_u32(r_u32(0x8005B470) + 268, lines);
    }
    context = r_u32(0x8005B470);
    lines = r_u32(context + 268);
    w_u32(context + 268, lines + 96);
    for (row = 0; row < 3; ++row)
    {
        parameters = 0x80088CD0 + 12 * row;
        state = 0x8009C608 + 4 * row;
        sub_8002CD78(r_u32(0x8009C5B8), (uint32_t)r_s16(parameters + 8),
            (uint32_t)r_s16(rectangle + 2), (uint32_t)(int32_t)(int16_t)y, font_ot, r_u32(rectangle + 8));
        w_u32(lines, r_u32(ot) | 0x07000000);
        w_u32(lines + 4, 0x4C010101);
        x = (uint32_t)r_s16(rectangle + 4);
        upper_y = ((uint32_t)(int32_t)(int16_t)y + 8) << 16;
        w_u32(lines + 8, x | (y << 16));
        w_u32(lines + 12, ((uint32_t)r_s16(rectangle + 4) + 120) | (y << 16));
        w_u32(lines + 16, ((uint32_t)r_s16(rectangle + 4) + 120) | upper_y);
        w_u32(lines + 20, (uint32_t)r_s16(rectangle + 4) | upper_y);
        w_u32(lines + 28, 0x55555555);
        w_u32(lines + 24, (uint32_t)r_s16(rectangle + 4) | (y << 16));
        w_u32(ot, lines);
        lines += 32;
        current = r_u32(state);
        desired = r_u32(0x80088CF4 + 12 * (uint32_t)character + 4 * row);
        if ((int32_t)current < (int32_t)desired)
        {
            lower = r_u32(parameters);
            if ((int32_t)current < (int32_t)lower)
                w_u32(state, lower);
            w_u32(state, r_u32(state) + 1);
        }
        else if ((int32_t)desired < (int32_t)current)
        {
            upper = r_u32(parameters + 4);
            if ((int32_t)upper < (int32_t)current)
                w_u32(state, upper);
            w_u32(state, r_u32(state) - 1);
        }
        upper = r_u32(parameters + 4);
        numerator = ((r_u16(state) - r_u16(parameters)) & 0xFFFF) * 120;
        divisor = (int32_t)(upper - r_u32(parameters));
        /* MIPS DIV returns minus one for a nonnegative dividend and zero divisor */
        width = divisor ? (uint32_t)((int32_t)numerator / divisor) : 0xFFFFFFFF;
        w_u32(tiles, r_u32(ot) | 0x03000000);
        w_u32(tiles + 4, r_u32(rectangle + 16) | 0x62000000);
        w_u16(tiles + 8, r_u16(rectangle + 4));
        w_u16(tiles + 10, y);
        w_u16(tiles + 12, width);
        w_u16(tiles + 14, 8);
        w_u32(ot, tiles);
        w_u32(tiles + 16, tiles | 0x03000000);
        w_u32(tiles + 20, r_u32(rectangle + 12) | 0x62000000);
        w_u16(tiles + 26, y);
        w_u16(tiles + 28, 120 - width);
        w_u16(tiles + 30, 8);
        w_u16(tiles + 24, r_u16(rectangle + 4) + width);
        w_u32(ot, tiles + 16);
        y = (y + r_u16(rectangle + 6)) & 0xFFFF;
        tiles += 32;
    }
    return 1;
}


uint32_t sub_80069440(uint32_t menu)
{
    uint32_t result;
    FUNCTION_MARKER(0x80069440, "RASHCDF.BIN");
    if (r_s16(menu + 2) == 0x7FFF)
    {
        w_u8(0x8009C690, r_u8(0x800D80E1));
        w_u8(0x8009C691, r_u8(0x800D80E2));
        w_u8(0x8009C692, r_u8(0x800D80E3));
    }
    result = sub_80069418(menu);
    if (r_u8(0x8009C5E3) == 4 && result == 1)
    {
        w_u8(0x800D80E1, r_u8(0x8009C690));
        (void)sub_800630C0(r_u32(0x8009C4DC), (uint32_t)r_s8(0x8009C690));
        w_u8(0x800D80E2, r_u8(0x8009C691));
        (void)sub_800630C0(r_u32(0x8009C4E0), (uint32_t)r_s8(0x8009C691));
        w_u8(0x800D80E3, r_u8(0x8009C692));
        (void)sub_800630C0(r_u32(0x8009C4E4), (uint32_t)r_s8(0x8009C692));
    }
    return result ? result : 2;
}

uint32_t sub_80069560(uint32_t menu)
{
    int32_t first;
    uint32_t result;
    FUNCTION_MARKER(0x80069560, "RASHCDF.BIN");
    if (r_s16(menu + 2) == 0x7FFF)
    {
        w_u8(0x8009C5EF, r_u8(0x800D80E0));
        (void)sub_800630C0(r_u32(0x8009C4D8), (uint32_t)r_s8(0x800D80E0));
    }
    first = r_s8(menu + 14);
    result = sub_8006B03C(menu, (uint32_t)first, (uint32_t)(first + r_s8(menu + 15)));
    if (result == 1)
    {
        w_u16(0x8009C5D2, r_u16(0x8009C5D4));
        if (r_s8(0x8009C5F2))
            (void)sub_80068688(r_s8(0x800D80EE) ? 24 : 38);
    }
    return result;
}

uint32_t sub_8006AAF8(uint32_t menu)
{
    uint32_t result = 0;
    FUNCTION_MARKER(0x8006AAF8, "RASHCDF.BIN");
    w_u32(0x8005ACAC, 0);
    if (!r_s16(menu + 2))
        result = !(r_u16(0x8009C5DA) & 1);
    if (result)
        w_u16(0x8009C5D2, r_u16(0x8009C6C8 + 4 * (uint32_t)r_s16(0x8009C5D0)));
    return result;
}

uint32_t sub_8006AC80(uint32_t menu)
{
    uint32_t result = 0;
    FUNCTION_MARKER(0x8006AC80, "RASHCDF.BIN");
    if (r_s16(menu + 2) == 0x7FFF)
        w_u16(0x8009C5DC, r_u32(0x80088C40));
    else if (r_u32(0x80088C40) - (uint32_t)r_s16(0x8009C5DC) >= 301)
    {
        (void)sub_80022A78(2);
        w_u32(0x8005ACAC, 0);
        result = 1;
    }
    if (result)
        w_u16(0x8009C5D2, r_u16(0x8009C6C8 + 4 * (uint32_t)r_s16(0x8009C5D0)));
    return result;
}

uint32_t sub_8006A838(uint32_t menu)
{
    uint32_t result;
    FUNCTION_MARKER(0x8006A838, "RASHCDF.BIN");
    w_u32(0x8005ACAC, 0);
    if (r_s16(menu + 2) == 0x7FFF)
        w_u16(0x8009C5DA, r_u16(0x8009C5DA) & 0xFFF7);
    result = sub_80069418(menu);
    if (!result && (r_u16(0x8009C5DA) & 8))
    {
        result = 1;
        w_u16(0x8009C5D2, r_u16(0x8009C6C8 + 4 * (uint32_t)r_s16(0x8009C5D0)));
    }
    return result;
}


uint32_t sub_8006AB58(uint32_t menu)
{
    int32_t player;
    uint32_t pad, next, sound;
    FUNCTION_MARKER(0x8006AB58, "RASHCDF.BIN");
    if (r_s16(menu + 2))
    {
        w_u8(0x8009C5E9, 0);
        (void)sub_800630C0(r_u32(0x8009C540), 0);
    }
    for (player = r_s8(menu + 14); player < r_s8(menu + 14) + r_s8(menu + 15); ++player)
    {
        pad = 0x800D7128 + 192 * (uint32_t)player;
        if (r_s8(pad + 82) > 0)
        {
            sound = 2;
            next = r_s8(0x8009C5E9) ? (uint32_t)r_s8(0x8009C5E8) : r_u16(menu + 10);
        }
        else if (r_s8(pad + 74) > 0)
        {
            sound = 3;
            next = r_u16(menu + 10);
        }
        else
            continue;
        w_u16(0x8009C5D2, next);
        (void)sub_8007EAC0(sound);
        return 1;
    }
    return 2;
}

uint32_t sub_8006AD20(uint32_t menu)
{
    int32_t player;
    uint32_t pad;
    FUNCTION_MARKER(0x8006AD20, "RASHCDF.BIN");
    if (r_s16(menu + 2) == 0x7FFF)
        w_u32(0x8005ACAC, 0);
    for (player = r_s8(menu + 14); player < r_s8(menu + 14) + r_s8(menu + 15); ++player)
    {
        pad = 0x800D7128 + 192 * (uint32_t)player;
        if (r_s8(pad + 82) || r_s8(pad + 66) || r_s8(pad + 74) || r_s8(pad + 122))
        {
            (void)sub_8007EAC0(3);
            w_u16(0x8009C5D2, r_u16(menu + 10));
            return 1;
        }
    }
    if (r_s32(0x8005ACAC) >= 901)
    {
        w_u32(0x8005ACAC, 0);
        (void)sub_8007EAC0(3);
        w_u16(0x8009C5D2, r_u16(menu + 10));
        return 1;
    }
    return 2;
}


uint32_t sub_800B208C(uint32_t actor)
{
    uint16_t normal[3], direction[3];
    uint32_t depth, translation[3], tag, rider, result, sign, animation, event, axis, projection;
    uint32_t entity, frames, sequence, last, extra;
    int32_t dot;
    FUNCTION_MARKER(0x800B208C, "RASHCDG.BIN");
    for (axis = 0; axis < 3; ++axis)
        normal[axis] = (uint16_t)(0 - (uint32_t)r_u16(actor + 522 + 2 * axis));
    if (sub_800B6F40(actor + 196, normal, actor + 504, &depth, NULL) >= 8)
        return 0;
    tag = r_u16(actor + 172);
    rider = (tag >> 5) == 1 ? actor : 0;
    if (((tag >> 5) == 1 && (int32_t)(tag & 31) < r_s32(r_u32(0x8005B2F8) + 48)) ||
        ((tag >> 5) == 1 && (r_u8(actor + 572) & 32)))
        (void)sub_80027778(actor, 3, 300, 1);
    if (r_s8(actor + 534) == 4 || (r_u32(actor + 552) & 0x04000000))
    {
        sign = (uint32_t)((int32_t)depth >> 31);
        if ((int32_t)((depth + sign) ^ sign) > 327680)
        {
            w_u32(actor + 464, 0);
            w_u32(actor + 460, 0);
            w_u32(actor + 456, 0);
            w_u32(actor + 552, (r_u32(actor + 552) | 0x04000000) & 0xBFFFFFFF);
        }
        tag = r_u16(actor + 172);
        result = (tag >> 5) == 1 && (int32_t)(tag & 31) < r_s32(r_u32(0x8005B2F8) + 48);
        if (result)
        {
            result = r_u32(actor + 552) | 0x80000;
            w_u32(actor + 552, result);
        }
        goto finish;
    }
    (void)sub_8002EE50(depth, normal, translation);
    (void)sub_800A8DF0(xport_guest_ptr(actor, 1), translation, 1);
    if ((int16_t)normal[1] >= -2632)
    {
        if (sub_800A966C(actor, normal, 0x80000) == 2 && r_u16(actor + 544) != 43)
            (void)sub_800C4550(43, actor, 3);
        goto sound;
    }
    if (r_s8(actor + 535) <= 0 && r_s32(actor + 480) > 439457)
    {
        (void)sub_800A966C(actor, normal, 0);
        goto sound;
    }
    projection = 0;
    for (axis = 0; axis < 3; ++axis)
    {
        int32_t delta = (int32_t)(r_u32(actor + 504 + 4 * axis) - r_u32(actor + 184 + 4 * axis));
        int32_t component = 16 * r_s16(actor + 522 + 2 * axis);
        projection += (uint32_t)(((int64_t)delta * component) >> 16);
    }
    w_u32(actor + 508, projection);
    if ((int32_t)projection < 0)
        projection = 0;
    w_u32(actor + 508, projection);
    tag = r_u16(actor + 172);
    if ((tag >> 5) == 1 && (int32_t)(tag & 31) < r_s32(r_u32(0x8005B2F8) + 48) &&
        !(r_u32(actor + 388) & 1) &&
        sub_80038550(actor, r_u32(actor + 348), r_u32(actor + 344), actor + 328, actor + 438) == 1)
    {
        uint32_t x = r_u16(actor + 438), z = r_u16(actor + 442), y;
        w_u16(actor + 438, 0 - x);
        y = r_u16(actor + 440);
        w_u16(actor + 442, 0 - z);
        w_u16(actor + 440, 0 - y);
    }
    (void)sub_8007EA64(actor);
    dot = (int32_t)sub_8002E698(xport_guest_cptr(actor + 528, 6), xport_guest_cptr(actor + 450, 6));
    for (axis = 0; axis < 3; ++axis)
        direction[axis] = r_u16(actor + 528 + 2 * axis);
    for (axis = 0; axis < 3; ++axis)
        w_u16(actor + 450 + 2 * axis, direction[axis]);
    if (dot < 0)
        for (axis = 0; axis < 3; ++axis)
            w_u16(actor + 450 + 2 * axis, 0 - (uint32_t)direction[axis]);
    event = r_u16(actor + 544);
    if (event == 46)
    {
        uint32_t random = sub_8001FC58() % 3;
        event = random == 0 ? 53 : random == 1 ? 52 : 54;
    }
    else if (event == 47)
        event = 58;
    else if (event == 43)
        event = 45;
    if (event == 48)
        w_u32(r_u32(actor + 540) + 24, 4);
    else if (event == 49)
    {
        w_u32(r_u32(actor + 540) + 24, 8);
        animation = r_u32(actor + 540);
        frames = r_u32(animation + 4) + 12 * r_u32(animation + 12);
        sequence = r_u32(r_u32(r_u32(animation + 40) + 4) + 4 * r_u8(frames));
        last = (uint32_t)(int32_t)(int16_t)(r_u16(sequence + 16) - 1);
        extra = r_u32(0x8005B3E4);
        if (extra)
            extra = r_u32(extra + 196);
        (void)sub_8005C018(animation, (r_u32(0x8005435C) >> 4) & 0xFFF, 0,
            (uint32_t)r_s16(animation + 16), last, 0, extra);
        (void)sub_800C2FF4(49, actor, 0);
    }
    else
        (void)sub_800C4550(event, actor, 2);
    if (rider)
        w_u32(rider + 600, 196608);
    else
        w_u32(actor + 488, 196608);
    (void)sub_8002EE50(r_u32(actor + 480), xport_guest_cptr(actor + 450, 6), xport_guest_ptr(actor + 456, 12));
    w_u8(actor + 535, 0);
    w_u32(actor + 552, r_u32(actor + 552) & 0xBFFFFFFF);
    tag = r_u16(actor + 172);
    if ((tag >> 5) == 1 && (int32_t)(tag & 31) < r_s32(r_u32(0x8005B2F8) + 48))
    {
        entity = 0x800CD898 + 1132 * r_u16(r_u32(rider + 596) + 172);
        w_u32(entity + 548, r_u32(entity + 548) & 0xFFFFFF7F);
    }
    goto feedback;
sound:
    (void)sub_80017BA0(r_s32(actor + 184), r_s32(actor + 192), 55, 0);
feedback:
    if (rider)
    {
        tag = r_u16(rider + 172);
        if ((((tag >> 5) == 1 && (int32_t)(tag & 31) < r_s32(r_u32(0x8005B2F8) + 48)) ||
            (r_u32(rider + 572) & 0x60) == 32) && !r_u32(0x8005B220))
            (void)sub_800B658C(r_u32(rider + 596), rider, r_s32(rider + 480), 0x165A1C, 2);
    }
    result = r_u8(actor + 535) + 1;
    w_u8(actor + 535, result);
finish:
    if (rider)
    {
        result = r_u32(actor + 552) & 0x80000;
        if (result)
        {
            entity = 0x800CD898 + 1132 * r_u16(r_u32(rider + 596) + 172);
            result = r_u32(entity + 788);
            if ((int32_t)result < 0)
                w_u32(entity + 788, 0);
        }
    }
    return result;
}


uint32_t sub_800B1978(uint32_t actor)
{
    uint16_t normal[3];
    uint32_t depth, second_depth, vertex, linked, axis, flags, result, sign, translation[3];
    uint32_t projection, speed, magnitude, entity, angle;
    int32_t response, side, forward;
    FUNCTION_MARKER(0x800B1978, "RASHCDG.BIN");
    for (axis = 0; axis < 3; ++axis)
        normal[axis] = (uint16_t)(0 - (uint32_t)r_u16(actor + 522 + 2 * axis));
    vertex = sub_800B6F40(actor + 196, normal, actor + 504, &depth, NULL);
    linked = r_u32(actor + 856);
    if (linked && (vertex == 8 || (vertex & 1) != ((vertex & 2) >> 1)))
    {
        uint32_t second_vertex = sub_800B6F40(linked + 196, normal, actor + 504, &second_depth, NULL);
        if (second_vertex < 8)
        {
            vertex = second_vertex;
            depth = second_depth;
        }
    }
    if (vertex >= 8)
        return 0;
    if ((r_u32(actor + 568) & 0x400) && !(r_s32(actor + 708) < (int32_t)depth))
        return 0;
    if (r_s8(actor + 534) == 4 || (r_u32(actor + 564) & 0x100000))
    {
        sign = (uint32_t)((int32_t)depth >> 31);
        if ((int32_t)((depth + sign) ^ sign) > 327680)
        {
            w_u32(actor + 464, 0);
            w_u32(actor + 460, 0);
            w_u32(actor + 456, 0);
            w_u32(actor + 564, r_u32(actor + 564) | 0x100000);
        }
        if (r_u16(actor + 172) < r_u32(r_u32(0x8005B2F8) + 48))
        {
            entity = r_u32(actor + 852);
            w_u32(entity + 552, r_u32(entity + 552) | 0x80000);
        }
        goto finish;
    }
    w_u32(actor + 564, r_u32(actor + 564) & 0xFFEFFFFF);
    (void)sub_8002EE50(depth, normal, translation);
    (void)sub_800A8DF0(xport_guest_ptr(actor, 1), translation, 1);
    if (!(r_u32(actor + 568) & 0x02000000))
    {
        uint16_t x = r_u16(actor + 450), y = r_u16(actor + 452), z = r_u16(actor + 454);
        flags = r_u32(actor + 568) | 0x02000000;
        speed = r_u32(actor + 480);
        w_u16(actor + 864, x);
        w_u16(actor + 866, y);
        w_u16(actor + 868, z);
        w_u32(actor + 860, speed);
        w_u32(actor + 568, flags);
    }
    if (r_u32(actor + 564) & 0x8000)
        goto finish;
    flags = r_u32(actor + 568);
    if (flags & 0x400)
        (void)sub_800B16F4(actor, actor + 860);
    else if (flags & 0x200)
    {
        if (r_s32(actor + 756) <= 32767 || (flags & 0x80000))
        {
            response = (int32_t)sub_8002E698(xport_guest_cptr(actor + 864, 6), normal);
            if (response < -58982)
                response = -58982;
            flags = r_u32(actor + 568);
            if (flags & 0x200000)
            {
                w_u32(actor + 616, 0);
                w_u32(actor + 688, 0);
                w_u32(actor + 632, 0);
                w_u32(actor + 628, 0);
                w_u32(actor + 624, 0);
                w_u32(actor + 620, 0);
                w_u32(actor + 640, 0);
                projection = 0;
                for (axis = 0; axis < 3; ++axis)
                {
                    int32_t delta = (int32_t)(r_u32(actor + 184 + 4 * axis) - r_u32(actor + 504 + 4 * axis));
                    int32_t component = 16 * r_s16(actor + 522 + 2 * axis);
                    projection += (uint32_t)(((int64_t)delta * component) >> 16);
                }
                w_u32(actor + 772, projection);
                if ((int32_t)projection > 0)
                    projection = 0;
                w_u32(actor + 772, projection);
                for (axis = 0; axis < 3; ++axis)
                    xport_gte_write_control(2 * axis, (uint32_t)r_s16(actor + 522 + 2 * axis));
                for (axis = 0; axis < 3; ++axis)
                    xport_gte_write_data(9 + axis, (uint32_t)r_s16(actor + 864 + 2 * axis));
                xport_gte_execute(0x178000C);
                for (axis = 0; axis < 3; ++axis)
                    w_u16(actor + 814 + 2 * axis, xport_gte_read_data(9 + axis));
                linked = r_u32(actor + 856);
                w_u32(actor + 488, 0);
                if (linked)
                    w_u32(linked + 488, 0);
                side = (int32_t)sub_8002E698(xport_guest_cptr(actor + 528, 6), xport_guest_cptr(actor + 814, 6));
                forward = (int32_t)sub_8002E698(xport_guest_cptr(actor + 528, 6), xport_guest_cptr(actor + 450, 6));
                angle = sub_80020018((uint32_t)side, (uint32_t)forward);
                w_u32(actor + 676, (uint32_t)((int32_t)(25736 * angle) >> 8));
                w_u32(actor + 652, r_s32(actor + 708) > 0 ? 102943 : (uint32_t)-102943);
                w_u32(actor + 568, (r_u32(actor + 568) & 0xFFC7FDFF) | 0x900);
            }
            else
            {
                if (!(flags & 0x80000))
                {
                    w_u32(actor + 568, flags & 0xFFCFFFFF);
                    if ((int32_t)sub_8002E698(xport_guest_cptr(actor + 808, 6), xport_guest_cptr(actor + 522, 6)) < 0)
                        w_u32(actor + 568, r_u32(actor + 568) | 0x100000);
                    speed = r_u32(actor + 488);
                    sign = (uint32_t)((int32_t)speed >> 31);
                    magnitude = (uint32_t)((int32_t)((speed + sign) ^ sign) / 4);
                    w_u32(actor + 640, magnitude);
                    if ((int32_t)magnitude < 131072)
                        magnitude = 131072;
                    w_u32(actor + 640, magnitude);
                    if ((int32_t)(r_u32(actor + 692) ^ r_u32(actor + 488)) >= 0)
                        w_u32(actor + 640, 0 - magnitude);
                    speed = r_u32(actor + 488);
                    if ((int32_t)speed <= 0)
                    {
                        if ((int32_t)speed > -327680)
                            speed = (uint32_t)-327680;
                    }
                    else if ((int32_t)speed < 327680)
                        speed = 327680;
                    w_u32(actor + 488, speed);
                    w_u32(actor + 568, r_u32(actor + 568) | 0x80000);
                }
                response /= 2;
            }
            (void)sub_8002EA20(xport_guest_cptr(actor + 864, 6), normal, (int32_t)(0 - (uint32_t)response), xport_guest_ptr(actor + 456, 12));
            for (axis = 0; axis < 3; ++axis)
                w_u16(actor + 864 + 2 * axis, (uint32_t)(r_s32(actor + 456 + 4 * axis) >> 4));
            (void)sub_8002E468(xport_guest_ptr(actor + 864, 6));
            (void)sub_8002EE50(r_u32(actor + 860), xport_guest_cptr(actor + 864, 6), xport_guest_ptr(actor + 456, 12));
        }
        else
        {
            int32_t contact;
            (void)sub_80017BA0(r_s32(actor + 184), r_s32(actor + 192), 17, 0);
            contact = (int32_t)sub_80084564(normal, actor + 864, actor + 860, actor + 456, 3,
                r_s32(actor + 860) > 585943 ? 585944 : 0, 0);
            if (contact >= 0 && r_s32(actor + 756) <= 131071)
                (void)sub_800849D8(actor);
            w_u32(actor + 756, 0);
        }
    }
    (void)sub_8002076C(actor);
    w_u32(actor + 832, 0);
    w_u32(actor + 828, 0);
finish:
    result = r_u32(r_u32(actor + 852) + 552) & 0x80000;
    if (result)
    {
        entity = 0x800CD898 + 1132 * r_u16(actor + 172);
        result = r_u32(entity + 788);
        if ((int32_t)result < 0)
            w_u32(entity + 788, 0);
    }
    return result;
}


uint32_t sub_800AF0A0(uint32_t actor, uint32_t surface, uint16_t normal[3], uint32_t mode, uint32_t force, uint32_t translation[3])
{
    uint32_t longitudinal = (mode & 5) == 1, point[3], extent, previous_contact = 0, result;
    int32_t speed, ratio;
    FUNCTION_MARKER(0x800AF0A0, "RASHCDG.BIN");
    extent = r_u32(actor + 304);
    if (longitudinal)
        extent += r_u32(actor + 308) - r_u32(actor + 304);
    if (mode < 2)
        extent = 0 - extent;
    (void)sub_8002EAD8(xport_guest_cptr(actor + 184, 12),
        xport_guest_cptr(actor + 432 + (longitudinal ? 12 : 0), 6), extent, point);
    if (r_u32(actor + 832) == surface)
        previous_contact = r_u8(actor + 565) >> 7;
    speed = r_s32(actor + 480);
    result = sub_800AF3B0(actor, surface, normal, (int32_t)mode, force ? 1 : 3, translation);
    if (result)
    {
        uint32_t keep_speed = 0;
        if (!previous_contact && (r_u32(actor + 564) & 0x8000))
            keep_speed = r_u32(actor + 832) == surface;
        ratio = (r_u16(surface) >> 5) == 6 ? 196608 : 32768;
        result = sub_800A9408(actor, longitudinal || keep_speed ? speed : speed >> 2, ratio, 65536, (int32_t)mode);
    }
    return result;
}

/* The IDA 800A77B8 chunk continues with v0 from the preceding queue load */
uint32_t sub_800A77B0(void)
{
    uint32_t count = r_u32(0x800CCF68), marked = 0, queue, identity, kind, actor, surface, packed, flags, result;
    uint32_t index;
    int32_t remaining;
    FUNCTION_MARKER(0x800A77B0, "RASHCDG.BIN");
    if (count > 8)
    {
        RRJ_DEBUG_FPUTS("RRJ contact queue count exceeds eight entries\n", stderr);
        abort();
    }
    for (remaining = (int32_t)(count - 1); remaining >= 0; --remaining)
    {
        queue = 0x800CCE48 + 36 * (uint32_t)remaining;
        identity = r_u16(queue + 2);
        kind = identity >> 5;
        if (!kind)
            continue;
        actor = r_u32(0x8005B3A0) + 1096 * r_u16(queue);
        if (kind == 8)
            (void)sub_800B12A0(actor, xport_guest_ptr(queue + 4, 6), r_u16(queue + 10) & 0xFFF,
                r_s32(queue + 28), r_u32(queue + 12), xport_guest_ptr(queue + 16, 12));
        else if (kind == 3)
        {
            surface = 0x800CF660 + 512 * (identity & 31);
            packed = r_u32(queue + 28);
            (void)sub_800AC958(actor, surface, packed & 0xFFFF, (uint32_t)((int32_t)packed >> 16), xport_guest_ptr(queue + 16, 12));
        }
        else
        {
            surface = kind == 6 ? r_u32(0x800CD6C4) + 280 * (identity & 31) :
                r_u32(0x800CE5A4) + 452 * (identity & 31) + 172;
            if ((r_u16(surface) >> 5) == 6 && r_u32(surface + 8) == 1)
            {
                flags = r_u16(queue + 10);
                (void)sub_800AF0A0(actor, surface, xport_guest_ptr(queue + 4, 6), flags & 0xFFEF,
                    flags & 16, xport_guest_ptr(queue + 16, 12));
            }
            else
            {
                packed = r_u32(queue + 28);
                (void)sub_800B11B4(actor, surface, packed & 0xFFFF, (uint32_t)((int32_t)packed >> 16),
                    xport_guest_ptr(queue + 16, 12), xport_guest_ptr(queue + 4, 6));
            }
        }
        identity = r_u16(actor + 172);
        marked |= 1u << (identity & 31);
        if ((r_u32(0x800CCF70 + 4 * (identity >> 4)) >> (2 * (identity & 15))) & 3)
            (void)sub_800A7AB4(actor, xport_guest_cptr(queue + 16, 12), 31);
    }
    result = r_u32(0x800CCF68);
    index = 0;
    if ((int32_t)result > 0)
    {
        do
        {
            if (index >= 8)
            {
                RRJ_DEBUG_FPUTS("RRJ contact queue grew outside its storage\n", stderr);
                abort();
            }
            queue = 0x800CCE48 + 36 * index;
            if (!(r_u16(queue + 2) >> 5))
            {
                actor = r_u32(0x8005B3A0) + 1096 * r_u16(queue);
                if (!((marked >> (r_u16(actor + 172) & 31)) & 1))
                {
                    surface = r_u32(0x8005B3A0) + 1096 * r_u16(queue + 2);
                    packed = r_u32(queue + 28);
                    (void)sub_800AC130(actor, surface, packed & 0xFFFF, (uint32_t)((int32_t)packed >> 16));
                }
            }
            ++index;
            result = (int32_t)index < r_s32(0x800CCF68);
        } while (result);
    }
    return result;
}


uint32_t sub_800AD04C(uint32_t actor, uint32_t other)
{
    uint32_t force = 0, power = 0, rider = 0, same_rider = 0, result, tag, body, owner;
    uint32_t actor_face = 0, other_face = 0, translation[3] = {0}, delta[3], velocity[3];
    uint16_t normal[3];
    uint32_t axis, flags, contact, selected, minimum, old_extent, extent, projection, sign, angle, adjusted;
    uint32_t relative, quotient, difference, severity, alignment, target;
    int32_t index;
    FUNCTION_MARKER(0x800AD04C, "RASHCDG.BIN");
    if (r_u32(actor + 832) == other + 172)
        force = r_u8(actor + 565) >> 7;
    tag = r_u16(other + 172);
    if ((tag >> 5) == 1)
        rider = r_u32(0x8005B3A4) + 628 * (tag & 31);
    if (rider)
    {
        body = r_u32(actor + 852);
        tag = r_u16(rider + 172);
        if (r_u16(body + 172) == tag ||
            ((r_u8(body + 572) & 16) && r_u16(r_u32(r_u32(actor + 856) + 852) + 172) == tag))
            same_rider = 1;
        result = r_u32(rider + 604) == 2 ? 2 : r_u32(actor + 568) & 0xEC;
        if (result && same_rider)
            return result;
    }
    result = other + 172;
    if (r_u32(actor + 828) == result && !force)
        return result;
    /* The original sticky-contact path can otherwise read an unwritten stack normal */
    for (axis = 0; axis < 3; ++axis)
        normal[axis] = r_u16(actor + 516 + 2 * axis);
    contact = sub_800A8FE8(actor);
    if (sub_800A8C78(actor, 1310720, 1835008))
    {
        {
            uint8_t *native_owner = sub_800AAD30(actor, xport_guest_ptr(other, 1), &actor_face, &other_face, translation, &power);
            owner = native_owner ? 0x80000000 + (uint32_t)((uintptr_t)native_owner - (uintptr_t)DRAM) : 0;
        }
        if (owner && owner == r_u32(actor + 856))
            owner = actor;
        if ((int32_t)power > 0)
        {
            projection = (uint32_t)sub_8001FC90(r_s32(actor + 480), (int32_t)power);
            (void)sub_8002EE50(projection, xport_guest_cptr(actor + 450, 6), velocity);
            (void)sub_800A8DF0(xport_guest_ptr(actor, 1), velocity, 1);
            (void)sub_800A8DF0(xport_guest_ptr(other, 1), velocity, 1);
            power = power < r_u32(actor + 552) ? r_u32(actor + 552) : power;
            w_u32(actor + 552, power);
        }
    }
    else
    {
        if (contact)
        {
            angle = sub_80020018((uint32_t)(16 * r_s16(actor + 450)), (uint32_t)(16 * r_s16(actor + 454)));
            w_u32(actor + 292, angle);
            w_u32(actor + 296, (uint32_t)(16 * r_s16(0x8005624C + ((4 * (angle & 0xFFF)) | 2))));
            w_u32(actor + 300, (uint32_t)(16 * r_s16(0x8005624C + 4 * (r_u32(actor + 292) & 0xFFF))));
        }
        owner = (sub_800AA140(actor, xport_guest_cptr(other, 512), &actor_face, &other_face, translation) ? other : 0);
    }
    if (owner)
    {
        target = actor;
        if (r_u16(owner + 172) == r_u16(actor + 172))
            target = other;
        selected = r_u16(owner + 172) == r_u16(actor + 172) ? other_face : actor_face;
        (void)sub_800B675C(xport_guest_cptr(target + 196, 96), xport_guest_cptr(target + 432, 18), selected & 255, normal, NULL);
        if (target == other)
        {
            normal[0] = (uint16_t)(0 - (uint32_t)normal[0]);
            normal[2] = (uint16_t)(0 - (uint32_t)normal[2]);
            normal[1] = (uint16_t)(0 - (uint32_t)normal[1]);
        }
    }
    contact = other_face;
    if (other_face && rider && same_rider)
    {
        flags = r_u32(actor + 568);
        if ((!r_u32(actor + 480) && (uint32_t)(r_u16(rider + 544) - 72) < 2) || (flags & 0xE0) ||
            ((r_u32(rider + 552) & 0x40000000) && (flags & 0x600)) ||
            (r_u8(r_u32(0x8005B2F8) + 57) == 2 && rider == r_u32(r_u32(r_u32(0x8005B38C) + 856) + 852)))
        {
            other_face = 0;
            contact = 0;
        }
    }
    if (contact || force)
    {
        if (!(r_u32(actor + 568) & 0x02000000))
        {
            uint16_t x = r_u16(actor + 450), y = r_u16(actor + 452), z = r_u16(actor + 454);
            flags = r_u32(actor + 568) | 0x02000000;
            power = r_u32(actor + 480);
            w_u16(actor + 864, x);
            w_u16(actor + 866, y);
            w_u16(actor + 868, z);
            w_u32(actor + 860, power);
            w_u32(actor + 568, flags);
        }
        for (axis = 0; axis < 3; ++axis)
            delta[axis] = r_u32(other + 184 + 4 * axis) - r_u32(actor + 184 + 4 * axis);
        (void)sub_8002EE50(r_u32(other + 480), xport_guest_cptr(other + 450, 6), velocity);
        for (axis = 0; axis < 3; ++axis)
            velocity[axis] -= r_u32(actor + 456 + 4 * axis);
        contact = force || !(r_u32(other + 552) & 0x20000000) || (int32_t)sub_8002E604(delta, velocity) < 0;
        if (contact)
        {
            if (!force && (r_u32(other + 552) & 0x40000000))
            {
                if (rider && !(r_u32(rider + 552) & 32) && !(r_u32(actor + 560) & 0x20000000))
                {
                    projection = 0;
                    for (axis = 0; axis < 3; ++axis)
                        projection += (uint32_t)(((int64_t)(int32_t)delta[axis] * (16 * r_s16(actor + 522 + 2 * axis))) >> 16);
                    if ((int32_t)projection < -49152)
                        (void)sub_800C29F0(rider, actor);
                }
            }
            else
            {
                selected = 7;
                minimum = r_u32(other + 284);
                for (index = 6; index >= 0; --index)
                {
                    power = r_u32(other + 200 + 12 * (uint32_t)index);
                    if ((int32_t)power < (int32_t)minimum)
                    {
                        selected = (uint32_t)index;
                        minimum = power;
                    }
                }
                old_extent = r_u32(other + 308);
                w_u32(other + 308, r_u32(other + 312));
                for (axis = 0; axis < 3; ++axis)
                    delta[axis] = r_u32(other + 196 + 12 * selected + 4 * axis) - r_u32(other + 184 + 4 * axis);
                projection = 0;
                for (axis = 0; axis < 3; ++axis)
                    projection += (uint32_t)(((int64_t)(int32_t)delta[axis] * (16 * r_s16(actor + 522 + 2 * axis))) >> 16);
                sign = (uint32_t)((int32_t)projection >> 31);
                extent = (projection + sign) ^ sign;
                w_u32(other + 312, projection);
                if ((int32_t)extent < 19660)
                    extent = 19660;
                w_u32(other + 312, extent);
                contact = sub_800AF224(actor, other + 172, actor_face, other_face, NULL);
                power = r_u32(other + 308);
                w_u32(other + 308, old_extent);
                w_u32(other + 312, power);
                force = 0;
                if (r_u32(actor + 832) == other + 172)
                    force = r_u8(actor + 565) >> 7;
            }
            if (contact)
            {
                alignment = (uint32_t)sub_8001FC90(r_s32(actor + 296), r_s32(other + 296));
                alignment += (uint32_t)sub_8001FC90(r_s32(actor + 300), r_s32(other + 300));
                power = (uint32_t)sub_8001FC90((int32_t)alignment, r_s32(other + 480));
                relative = r_u32(actor + 480) - power;
                sign = (uint32_t)((int32_t)relative >> 31);
                relative = (relative + sign) ^ sign;
                quotient = (uint32_t)((int32_t)relative >> 17);
                adjusted = relative;
                if ((int32_t)relative < 0)
                {
                    adjusted += 0x1FFFF;
                    quotient = (uint32_t)((int32_t)adjusted >> 17);
                }
                difference = 4 - quotient;
                severity = (~(uint32_t)((int32_t)adjusted >> 31) & quotient) +
                    ((uint32_t)((int32_t)difference >> 31) & difference);
                if (sub_800A9868(other, normal, r_s32(actor + 860), xport_guest_cptr(actor + 864, 6), actor + 172, force) &&
                    (int32_t)severity > 0)
                {
                    (void)sub_80017BA0(r_s32(other + 184), r_s32(other + 192), 19, 0);
                    if (r_u16(actor + 172) < r_u32(r_u32(0x8005B2F8) + 48) &&
                        r_u32(r_u32(actor + 852) + 604) < 2 && !r_u32(0x8005B220))
                        (void)sub_800B658C(actor, 0, r_s32(actor + 480), 0x23C361, 2);
                    if (r_u16(other + 172) < 64)
                        (void)sub_8001A760(r_u16(r_u32(rider + 596) + 172), 1);
                    else
                        (void)sub_8001B44C(r_u32(other + 184), r_u32(other + 192), other, 1);
                }
            }
        }
    }
    result = other_face;
    if (other_face && (!contact || !force))
    {
        result = 5;
        if (r_u16(0x800541D4 + 8 * r_u16(other + 544) + 2) != 5)
        {
            target = owner;
            if (r_u16(owner + 172) == r_u16(actor + 172))
            {
                target = other;
                translation[0] = 0 - translation[0];
                translation[2] = 0 - translation[2];
                translation[1] = 0 - translation[1];
            }
            result = sub_800A8DF0(xport_guest_ptr(target, 1), translation, 1);
        }
    }
    return result;
}


uint32_t sub_800C29F0(uint32_t rider, uint32_t actor)
{
    uint32_t injured[2] = {0}, linked, body, target, axis, slot, sign, magnitude, positive;
    uint32_t participant, stats, result, controller;
    int32_t dot, remaining;
    FUNCTION_MARKER(0x800C29F0, "RASHCDG.BIN");
    dot = (int32_t)sub_8002E698(xport_guest_cptr(actor + 444, 6), xport_guest_cptr(rider + 450, 6));
    linked = r_u32(actor + 856) != 0;
    sign = (uint32_t)(dot >> 31);
    magnitude = ((uint32_t)dot + sign) ^ sign;
    if ((int32_t)magnitude <= 56754)
    {
        dot = (int32_t)sub_8002E698(xport_guest_cptr(actor + 432, 6), xport_guest_cptr(rider + 450, 6));
        positive = dot > 0;
        target = dot <= 0 && linked ? r_u32(actor + 856) : actor;
        body = r_u32(target + 852);
        if (r_u32(body + 604) < 2)
        {
            if (linked)
            {
                w_u32(body + 552, r_u32(body + 552) & 0xFFF9FFFF);
                body = r_u32(target + 852);
                w_u32(body + 552, r_u32(body + 552) | 0x8000 | ((positive << 17) + 0x20000));
            }
            else
            {
                w_u32(body + 552, r_u32(body + 552) | 0x8000);
                w_u32(target + 568, r_u32(target + 568) | 0x840 | (((0 - positive) & 0xFFFF8000) + 0x10000));
            }
            injured[0] = target;
        }
    }
    else
    {
        positive = dot > 0;
        if (!positive)
            w_u32(actor + 568, r_u32(actor + 568) | 0x880);
        for (slot = 0; slot < (uint32_t)(linked ? 2 : 1); ++slot)
        {
            target = slot ? r_u32(actor + 856) : actor;
            body = r_u32(target + 852);
            if (r_u32(body + 604) >= 2)
                continue;
            for (axis = 0; axis < 3; ++axis)
                w_u16(r_u32(target + 852) + 456 + 2 * axis, r_u16(actor + 864 + 2 * axis));
            if (positive)
                w_u32(r_u32(target + 852) + 480, (uint32_t)sub_8001FC90(78643, r_s32(rider + 480)));
            body = r_u32(target + 852);
            w_u32(body + 552, r_u32(body + 552) | (positive ? 0x204000 : 0x208000));
            injured[slot] = target;
        }
        if (positive && !r_u32(actor + 616))
        {
            w_u32(actor + 624, 3003949);
            w_u32(actor + 632, 0);
            w_u32(actor + 620, (uint32_t)-524288);
        }
    }
    for (slot = 0; slot < 2; ++slot)
    {
        if (!injured[slot])
            continue;
        (void)sub_800BFE58(r_u32(rider + 596), 1, 0);
        (void)sub_800BFE58(injured[slot], 1, 1);
        participant = r_u16(r_u32(rider + 596) + 172);
        if (participant < r_u32(r_u32(0x8005B2F8) + 48))
        {
            stats = 1 - participant;
            stats = participant + (stats & (uint32_t)((int32_t)stats >> 31));
            w_u32(0x800D6198 + 224 * stats + 140, 1);
        }
    }
    controller = r_u32(actor + 1084);
    result = r_u8(controller + 36) >> 3;
    remaining = (int32_t)r_u8(controller + 37) - (int32_t)result;
    if (remaining < 0)
        remaining = 0;
    w_u8(controller + 37, (uint32_t)remaining);
    return result;
}
