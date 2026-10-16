#include "rrj_debug.h"
#include "game_main.h"
#include "native_game_api.h"
#include "race_leaf.h"
#include "race_leaf_batch_004.h"
#include "race_leaf_batch_000.h"
#include "menu_hint.h"
#include "menu_loop.h"
#include "race_bodyless_batch_002.h"
#include "solo_resource_dependencies.h"
#include <string.h>
#include "race_pause.h"
#include "fixed_math.h"
#include "xport.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint32_t sub_800AE794(uint32_t actor, uint32_t surface)
{
    uint32_t force = 0, result, moved, maximum, minimum, point, axis, radius, raised = 0;
    uint32_t mode = 6, valid = 0, follow, linked, power = 0, translation[3] = {0};
    uint16_t normal[3] = {0}, saved_normal[3] = {0}, saved_identity = 0, saved_flags = 0;
    uint32_t saved_translation[3] = {0}, saved_power = 0, source, angle, width, length;
    uint32_t forward, lateral, speed, fast, extension, roll, correction, sign, magnitude, queue;
    FUNCTION_MARKER(0x800AE794, "RASHCDG.BIN");
    if (r_u32(actor + 832) == surface)
        force = r_u8(actor + 565) >> 7;
    result = r_u32(actor + 828);
    if (result == surface && !force)
        return result;
    moved = sub_800A8FE8(actor);
    maximum = r_u32(actor + 200);
    if (r_u32(actor + 568) & 0x600)
    {
        minimum = maximum;
        for (point = 1; point < 8; ++point)
        {
            uint32_t value = r_u32(actor + 200 + 12 * point);
            if ((int32_t)maximum < (int32_t)value)
                maximum = value;
            else if ((int32_t)value < (int32_t)minimum)
                minimum = value;
        }
    }
    else
    {
        minimum = r_u32(actor + 248);
        for (point = 1; point < 4; ++point)
        {
            uint32_t high = r_u32(actor + 200 + 12 * point);
            uint32_t low = r_u32(actor + 248 + 12 * point);
            if ((int32_t)maximum < (int32_t)high)
                maximum = high;
            if ((int32_t)low < (int32_t)minimum)
                minimum = low;
        }
    }
    if (r_s32(surface + 140) < (int32_t)(r_u32(actor + 508) - maximum))
        return 1;
    minimum = r_u32(actor + 508) - minimum;
    if ((r_u16(surface) >> 5) == 6)
    {
        uint32_t upper = r_u32(surface + 88);
        uint32_t step = (int32_t)upper < (int32_t)minimum;
        radius = r_u32(surface + 132) + ((0 - step) & (r_u32(surface + 84) - r_u32(surface + 132))) - 16;
        if ((int32_t)radius > 65536)
            raised = step;
    }
    else
    {
        radius = r_u32(surface + 136);
        if (r_s32(surface + 132) < (int32_t)radius)
            radius = r_u32(surface + 132);
        radius -= 16;
    }
    do
    {
        mode = 6;
        power = 0;
        follow = r_u32(actor + 856) && r_u32(actor + 1088) && !force;
        if (sub_800A8C78(actor, 1310720, 1835008) && r_s32(actor + 676) > -102943 &&
            r_s32(actor + 676) <= 102942 && r_u32(actor + 676))
        {
            mode = sub_800ADC74(actor, surface, (int32_t)radius, translation, &power);
            valid = mode < 6;
        }
        else
        {
            valid = 0;
            if (moved)
            {
                angle = sub_80020018((uint32_t)(16 * r_s16(actor + 450)), (uint32_t)(16 * r_s16(actor + 454)));
                w_u32(actor + 292, angle);
                w_u32(actor + 296, (uint32_t)(16 * r_s16(0x8005624C + ((4 * (angle & 0xFFF)) | 2))));
                w_u32(actor + 300, (uint32_t)(16 * r_s16(0x8005624C + 4 * (r_u32(actor + 292) & 0xFFF))));
            }
            width = r_u32(actor + 304) + radius;
            length = r_u32(actor + 308) + radius;
            forward = (uint32_t)sub_8001FC90(r_s32(actor + 300), (int32_t)(r_u32(surface + 12) - r_u32(actor + 184)));
            forward += (uint32_t)sub_8001FC90(r_s32(actor + 296), (int32_t)(r_u32(surface + 20) - r_u32(actor + 192)));
            lateral = (uint32_t)sub_8001FC90(r_s32(actor + 296), (int32_t)(r_u32(surface + 12) - r_u32(actor + 184)));
            lateral += (uint32_t)sub_8001FC90((int32_t)(0 - r_u32(actor + 300)), (int32_t)(r_u32(surface + 20) - r_u32(actor + 192)));
            speed = r_u32(actor + 480);
            fast = (int32_t)speed > 1048576;
            extension = (0 - fast) & (uint32_t)sub_8001FC90(r_s32(0x800CCE38), (int32_t)speed);
            if ((int32_t)forward < (int32_t)length && (int32_t)(0 - length - extension) < (int32_t)forward)
            {
                roll = r_u32(actor + 636);
                power = (uint32_t)(16 * r_s16(0x8005624C + (((163 * roll) >> 12) & 0x3FFC)));
                power = (uint32_t)sub_8001FC90((int32_t)power, r_s32(actor + 312));
                if (r_s32(actor + 636) >= 0)
                    valid = (int32_t)lateral < (int32_t)(width + power) && (int32_t)(0 - width) < (int32_t)lateral;
                else
                    valid = (int32_t)lateral < (int32_t)width && (int32_t)(power - width) < (int32_t)lateral;
            }
            if (valid)
            {
                sign = (uint32_t)((int32_t)lateral >> 31);
                magnitude = (lateral + sign) ^ sign;
                if ((int32_t)(magnitude - radius) >= 8519)
                {
                    power = (int32_t)lateral >= 0 ? lateral - width : lateral + width;
                    mode = ((uint32_t)((int32_t)~lateral >> 31)) & 2;
                    translation[0] = (uint32_t)sub_8001FC90(r_s32(actor + 296), (int32_t)power);
                    translation[2] = 0 - (uint32_t)sub_8001FC90(r_s32(actor + 300), (int32_t)power);
                }
                else
                {
                    correction = 0 - length;
                    if ((int32_t)forward <= 0)
                        correction = length + ((0 - fast) & (0 - length - length));
                    power = forward + correction;
                    mode = (int32_t)forward > 0 ? 3 : 2 * fast + 1;
                    translation[0] = (uint32_t)sub_8001FC90(r_s32(actor + 300), (int32_t)power);
                    translation[2] = (uint32_t)sub_8001FC90(r_s32(actor + 296), (int32_t)power);
                }
                translation[1] = 0;
            }
        }
        follow = follow && (!valid || mode == 2 || mode == 3);
        if (valid)
        {
            source = actor + ((mode & 5) == 1 ? 450 : 814);
            for (axis = 0; axis < 3; ++axis)
                normal[axis] = r_u16(source + 2 * axis);
            if (mode & 2)
                for (axis = 0; axis < 3; ++axis)
                    normal[axis] = (uint16_t)(0 - (uint32_t)normal[axis]);
            if (follow)
            {
                saved_identity = r_u16(surface);
                for (axis = 0; axis < 3; ++axis)
                {
                    saved_normal[axis] = normal[axis];
                    saved_translation[axis] = translation[axis];
                }
                sign = (uint32_t)((int32_t)power >> 31);
                saved_power = (power + sign) ^ sign;
                saved_flags = (uint16_t)(mode | (raised << 4));
            }
        }
        if (!r_u32(actor + 1088))
            follow = 2;
        if (follow)
        {
            linked = r_u32(actor + 856);
            /* Keep the primary object when the original final handoff has no linked object */
            if (linked)
                actor = linked;
            moved = 1;
        }
    } while (follow == 1);
    result = r_u16(surface);
    if (saved_identity == result)
    {
        sign = (uint32_t)((int32_t)power >> 31);
        power = (power + sign) ^ sign;
        if (valid)
        {
            if (!mode || (saved_flags & 15) == 2)
            {
                mode = 3;
                for (axis = 0; axis < 3; ++axis)
                    normal[axis] = (uint16_t)(0 - (uint32_t)r_u16(actor + 450 + 2 * axis));
                (void)sub_8002EAD8(translation, normal, r_u32(actor + 308) - r_u32(r_u32(actor + 856) + 308), translation);
                for (axis = 0; axis < 3; ++axis)
                    translation[axis] += saved_translation[axis];
                power += saved_power;
            }
            else if (mode != 3 || (saved_flags & 15) != 3)
                for (axis = 0; axis < 3; ++axis)
                    translation[axis] += saved_translation[axis];
        }
        if (!valid || ((int32_t)power < (int32_t)saved_power && mode == 3 && (saved_flags & 15) == 3))
        {
            valid = 1;
            mode = saved_flags & 15;
            for (axis = 0; axis < 3; ++axis)
            {
                normal[axis] = saved_normal[axis];
                translation[axis] = saved_translation[axis];
            }
        }
        result = saved_flags >> 4;
        raised |= result;
    }
    if (!valid)
        return result;
    if (force || (r_u16(surface) >> 5) != 6 || !(r_u16(actor + 320) & 4))
        return sub_800AF0A0(actor, surface, normal, mode, raised, translation);
    queue = r_u32(0x800CCF68);
    if ((int32_t)queue < 0 || (int32_t)queue >= 8)
        return sub_800AF0A0(actor, surface, normal, mode, raised, translation);
    result = 0x800CCE48 + 36 * queue;
    w_u16(result, r_u16(actor + 172));
    w_u16(result + 2, r_u16(surface));
    for (axis = 0; axis < 3; ++axis)
        w_u16(result + 4 + 2 * axis, normal[axis]);
    w_u32(result + 16, translation[0]);
    w_u32(0x800CCF68, queue + 1);
    w_u32(result + 20, translation[1]);
    w_u16(result + 10, mode | (raised << 4));
    w_u32(result + 24, translation[2]);
    return result;
}


uint32_t sub_800ADC74(uint32_t actor, uint32_t surface, int32_t radius, uint32_t translation[3], uint32_t *power)
{
    uint32_t points[8][2], normals[8][2], distances[8], order[6], delta[3];
    uint32_t across[2], up[2], lean[2], forward[2], sign, magnitude, value;
    int32_t inverted, positive_roll, tilt, threshold, start, base, contact_mode;
    int32_t index, previous, vertex, best, special, minimum = 0, depth, dot, candidate;
    uint32_t packing, first, second, third, scale, numerator, denominator;
    FUNCTION_MARKER(0x800ADC74, "RASHCDG.BIN");
    inverted = r_s32(actor + 200) < r_s32(actor + 236);
    positive_roll = r_s32(actor + 636) >= 0;
    order[0] = 4 * inverted + 3;
    order[1] = 4 * inverted + 2;
    order[2] = inverted + 1;
    if (positive_roll)
        order[2] ^= 7;
    for (index = 0; index < 3; ++index)
        order[index + 3] = order[index] ^ 6;
    tilt = (int32_t)(652 * r_u32(actor + 676)) >> 16;
    value = (uint32_t)r_s16(actor + 524);
    sign = (uint32_t)((int32_t)value >> 31);
    magnitude = (value + sign) ^ sign;
    if ((int32_t)magnitude < 4033)
    {
        across[0] = (uint32_t)(16 * r_s16(actor + 432));
        across[1] = (uint32_t)(16 * r_s16(actor + 436));
        up[0] = (uint32_t)(16 * r_s16(actor + 444));
        up[1] = (uint32_t)(16 * r_s16(actor + 448));
        (void)sub_8002DF14(across);
        (void)sub_8002DF14(up);
    }
    else
    {
        across[0] = (uint32_t)(16 * r_s16(actor + 516));
        across[1] = (uint32_t)(16 * r_s16(actor + 520));
        up[0] = (uint32_t)(16 * r_s16(actor + 528));
        up[1] = (uint32_t)(16 * r_s16(actor + 532));
    }
    lean[0] = (uint32_t)(16 * r_s16(actor + 438));
    lean[1] = (uint32_t)(16 * r_s16(actor + 442));
    forward[0] = (uint32_t)(16 * r_s16(actor + 450));
    forward[1] = (uint32_t)(16 * r_s16(actor + 454));
    (void)sub_8002DF14(forward);
    threshold = 0;
    if (sub_8002DF14(lean))
        threshold = (int32_t)(1024 - sub_8001FF3C(sub_8002DFC0(across, lean)));
    threshold = (int32_t)((uint32_t)threshold + ((0 - (uint32_t)inverted) & (2048 - 2 * (uint32_t)threshold)) - 1024);
    if (inverted == positive_roll)
    {
        base = 7;
        if (tilt < 0)
        {
            base = 1;
            contact_mode = 2;
            if (tilt < threshold) { start = 3; packing = 0x321; }
            else { start = 2; packing = 0x231; }
        }
        else { start = 1; contact_mode = 3; packing = 0x132; }
    }
    else
    {
        base = 0;
        if (tilt <= 0) { start = 3; base = 2; contact_mode = 2; packing = 0x312; }
        else
        {
            contact_mode = 3;
            if (tilt < threshold) { start = 2; packing = 0x213; }
            else { start = 1; packing = 0x123; }
        }
    }
    scale = 0 - (uint32_t)sub_8001FC90(r_s32(0x800CCE38), r_s32(actor + 480));
    (void)sub_8002EE50(scale, xport_guest_cptr(actor + 450, 6), delta);
    points[start][0] = r_u32(actor + 196 + 12 * order[start]);
    points[start][1] = r_u32(actor + 204 + 12 * order[start]);
    previous = start;
    for (index = start + 1; index <= start + 3; ++index)
    {
        points[index][0] = r_u32(actor + 196 + 12 * order[previous]) + delta[0];
        points[index][1] = r_u32(actor + 204 + 12 * order[previous]) + delta[2];
        previous = index;
    }
    vertex = previous % 6;
    points[start + 4][0] = r_u32(actor + 196 + 12 * order[vertex]) + delta[0];
    points[start + 4][1] = r_u32(actor + 204 + 12 * order[vertex]) + delta[2];
    for (index = 5; index < 8; ++index)
    {
        points[(start + index) % 8][0] = r_u32(actor + 196 + 12 * order[vertex]);
        points[(start + index) % 8][1] = r_u32(actor + 204 + 12 * order[vertex]);
        if (index != 7)
            vertex = (vertex + 1) % 6;
    }
    best = vertex;
    normals[0][0] = 0 - up[0]; normals[0][1] = 0 - up[1];
    normals[4][0] = up[0]; normals[4][1] = up[1];
    first = packing & 15;
    normals[first][0] = 0 - across[0]; normals[first][1] = 0 - across[1];
    normals[first + 4][0] = across[0]; normals[first + 4][1] = across[1];
    second = (packing >> 4) & 15;
    normals[second][0] = inverted ? lean[1] : 0 - lean[1];
    normals[second][1] = inverted ? 0 - lean[0] : lean[0];
    normals[second + 4][0] = 0 - normals[second][0];
    normals[second + 4][1] = 0 - normals[second][1];
    third = packing >> 8;
    normals[third][0] = 0 - forward[1]; normals[third][1] = forward[0];
    normals[third + 4][0] = forward[1]; normals[third + 4][1] = 0 - forward[0];
    for (index = 0; index < 8; ++index)
    {
        distances[index] = (uint32_t)sub_8001FC90((int32_t)normals[index][0], (int32_t)(r_u32(surface + 12) - points[index][0]));
        distances[index] += (uint32_t)sub_8001FC90((int32_t)normals[index][1], (int32_t)(r_u32(surface + 20) - points[index][1]));
        if ((int32_t)distances[index] < (int32_t)(0 - (uint32_t)radius))
            return 6;
    }
    for (index = (start + 5) % 8; index != start; index = (index + 1) % 8)
        if ((int32_t)distances[index] < minimum)
        { minimum = (int32_t)distances[index]; best = index; }
    if (minimum < 0)
    {
        depth = (int32_t)((uint32_t)radius + (uint32_t)minimum);
        translation[0] = (uint32_t)sub_8001FC90((int32_t)normals[best][0], depth);
        translation[2] = (uint32_t)sub_8001FC90((int32_t)normals[best][1], depth);
    }
    else
    {
        minimum = 1310720;
        special = (base + 2 - contact_mode + 8) % 8;
        value = distances[special] - 9830;
        distances[special] = (int32_t)value < 0 ? 0 : value;
        for (index = (start + 5) % 8; index != start; index = (index + 1) % 8)
        {
            dot = (int32_t)sub_8002DFC0(normals[index], forward);
            sign = (uint32_t)(dot >> 31);
            denominator = ((uint32_t)dot + sign) ^ sign;
            candidate = 1310720;
            if ((int32_t)denominator >= 132)
            {
                value = distances[index];
                sign = (uint32_t)((int32_t)value >> 31);
                numerator = (value + sign) ^ sign;
                value = sub_80010028(numerator, denominator);
                if (((int32_t)distances[index] > 0) == (dot >= 0))
                    value = 0 - value;
                candidate = (int32_t)value;
            }
            if (candidate < minimum && (int32_t)(0 - (uint32_t)radius) < candidate)
            { minimum = candidate; best = index; }
        }
        if (minimum == 1310720)
            return 6;
        if (best == special)
        {
            dot = (int32_t)sub_8002DFC0(normals[best], forward);
            value = distances[best] + 9830;
            sign = (uint32_t)((int32_t)value >> 31);
            numerator = (value + sign) ^ sign;
            denominator = sub_8002DFC0(normals[best], forward);
            if (dot < 0)
                denominator = 0 - denominator;
            scale = sub_80010028(numerator, denominator);
            if (((int32_t)value > 0) == (dot >= 0))
                scale = 0 - scale;
            minimum = (int32_t)scale;
        }
        depth = (int32_t)((uint32_t)radius + (uint32_t)minimum);
        translation[0] = (uint32_t)sub_8001FC90((int32_t)forward[0], (int32_t)(0 - (uint32_t)depth));
        translation[2] = (uint32_t)sub_8001FC90((int32_t)forward[1], (int32_t)(0 - (uint32_t)depth));
    }
    translation[1] = 0;
    sign = (uint32_t)(depth >> 31);
    *power = ((uint32_t)depth + sign) ^ sign;
    if (best == base || best == (base + 1) % 8)
        return contact_mode;
    return contact_mode == 3 ? 0 : 3;
}

uint32_t sub_800B09C4(uint32_t actor, uint32_t surface)
{
    uint8_t temporary[512];
    uint8_t *other;
    const uint8_t *owner;
    const uint8_t *primary = xport_guest_cptr(actor, 1);
    uint32_t force = 0, result, identity, kind, table, other_address, axis;
    uint32_t actor_face = 0, other_face = 0, power = 0;
    uint32_t translation[3] = {0}, velocity[3], angle, projection, depth, width;
    int32_t zone;
    int rotate;
    FUNCTION_MARKER(0x800B09C4, "RASHCDG.BIN");
    if (r_u32(actor + 832) == surface)
        force = r_u8(actor + 565) >> 7;
    result = r_u32(actor + 828);
    if (result == surface && !force)
        return result;
    identity = r_u16(surface);
    kind = identity >> 5;
    if (kind < 5)
    {
        table = 0x800CE4D0 + 16 * kind;
        other_address = r_u32(table) + r_u32(table + 4) * (identity & 31);
        other = xport_guest_ptr(other_address, 1);
    }
    else
    {
        memset(temporary, 0, 172);
        memcpy(temporary + 172, xport_guest_cptr(surface, 280), 280);
        xport_store_le16(temporary + 450, r_u16(surface + 272));
        xport_store_le16(temporary + 452, r_u16(surface + 274));
        xport_store_le16(temporary + 454, r_u16(surface + 276));
        xport_store_le32(temporary + 480, 0);
        xport_store_le32(temporary + 456, 0);
        xport_store_le32(temporary + 460, 0);
        xport_store_le32(temporary + 464, 0);
        xport_store_le32(temporary + 488, 0);
        xport_store_le32(temporary + 492, 0);
        xport_store_le32(temporary + 496, 0);
        other = temporary;
    }
    rotate = sub_800A8FE8(actor) != 0;
    if (sub_800A8C78(actor, 1310720, 1835008))
    {
        owner = sub_800AAD30(actor, other, &actor_face, &other_face, translation, &power);
        if (owner && r_u32(actor + 856) && owner == xport_guest_cptr(r_u32(actor + 856), 1))
            owner = primary;
        if ((int32_t)power > 0)
        {
            projection = (uint32_t)sub_8001FC90(r_s32(actor + 480), (int32_t)power);
            (void)sub_8002EE50(projection, xport_guest_cptr(actor + 450, 6), velocity);
            (void)sub_800A8DF0(xport_guest_ptr(actor, 1), velocity, 1);
            if ((r_u16(surface) >> 5) < 5)
                (void)sub_800A8DF0(other, velocity, 1);
            power = power < r_u32(actor + 552) ? r_u32(actor + 552) : power;
            w_u32(actor + 552, power);
        }
    }
    else
    {
        if (rotate)
        {
            angle = sub_80020018((uint32_t)r_s16(actor + 450) << 4, (uint32_t)r_s16(actor + 454) << 4);
            w_u32(actor + 292, angle);
            w_u32(actor + 296, (uint32_t)r_s16(0x8005624C + ((4 * (angle & 0xFFF)) | 2)) << 4);
            w_u32(actor + 300, (uint32_t)r_s16(0x8005624C + 4 * (r_u32(actor + 292) & 0xFFF)) << 4);
        }
        owner = sub_800AA140(actor, other, &actor_face, &other_face, translation);
    }
    if (actor_face || force)
    {
        if (owner != primary)
        {
            for (axis = 0; axis < 3; ++axis)
                translation[axis] = (uint32_t)(int32_t)(int16_t)(0 - xport_load_le16(translation + axis));
        }
        actor_face = sub_800AF224(actor, surface, actor_face, other_face, translation);
    }
    if (!actor_face)
        return actor_face;
    depth = r_u32(surface + 188);
    if (depth == r_u32(actor + 360) && (!(depth >> 16) || r_u32(surface + 164) == r_u32(actor + 336)))
    {
        depth = r_u32(surface + 172) - r_u32(actor + 344);
        if (r_s32(actor + 364) < 0)
            depth = 0 - depth;
    }
    else
        depth = sub_800B6AAC(xport_guest_cptr(surface + 12, 12), xport_guest_cptr(actor + 432, 6), xport_guest_cptr(actor + 184, 12));
    width = r_u32(actor + 304);
    zone = (int32_t)depth >= (int32_t)(0 - width) ? 3 : 0;
    if ((int32_t)width < (int32_t)depth)
        --zone;
    return sub_800A9408(actor, 0, 0, 65536, zone);
}

uint32_t sub_800C6358(uint32_t order[3])
{
    uint32_t count, actor, index = 0, position, result;
    FUNCTION_MARKER(0x800C6358, "RASHCDG.BIN");
    count = r_u32(0x8005B1F8);
    result = count;
    if ((int32_t)count <= 0)
        return result;
    actor = r_u32(0x8005B3A0);
    do
    {
        uint32_t flags = r_u32(actor + 1084);
        position = r_u8(flags + 39) - 1;
        if (position < 3)
        {
            position = r_u8(flags + 39) - 1;
            xport_store_le32(order + position, index);
        }
        ++index;
        result = (int32_t)index < (int32_t)count;
        actor += 1096;
    } while (result);
    return result;
}

uint32_t sub_800C63CC(uint16_t rect[4])
{
    uint32_t ordering, result;
    FUNCTION_MARKER(0x800C63CC, "RASHCDG.BIN");
    ordering = r_u32(0x8005B598);
    xport_store_le16(rect, 67);
    xport_store_le16(rect + 1, 41);
    xport_store_le16(rect + 2, 248);
    xport_store_le16(rect + 3, 148);
    (void)sub_800400F4(0, 0, 0, ordering + 4, 57, 36, 326, 204);
    ordering = r_u32(0x8005B598);
    w_u32(0x800CCBF8, r_u32(ordering + 4) | 0x05000000);
    w_u32(ordering + 4, 0x000CCBF8);
    (void)sub_8002CB08(r_u32(0x8005AD2C), 94, rect, ordering, 0x14506E, 2);
    ordering = r_u32(0x8005B598);
    xport_store_le16(rect + 1, xport_load_le16(rect + 1) + 150);
    (void)sub_8002CB08(r_u32(0x8005AD2C), 107, rect, ordering, 0x6E5050, 2);
    result = xport_load_le16(rect + 1) - 150;
    xport_store_le16(rect + 1, result);
    return result;
}

void *sub_80048FBC(void *environment)
{
    uint32_t index;
    uint8_t *output = (uint8_t *)environment;
    FUNCTION_MARKER(0x80048FBC, "SLUS_010.53");
    if (output)
        for (index = 0; index < 92; ++index)
            output[index] = r_u8(0x80055F7C + index);
    return environment;
}

uint32_t sub_800C5918(void)
{
    uint32_t order[3], color, flags, position, dimensions;
    uint16_t rect[4];
    uint8_t environment[92];
    FUNCTION_MARKER(0x800C5918, "RASHCDG.BIN");
    order[0] = r_u32(0x8005BCE0);
    order[1] = r_u32(0x8005BCE4);
    order[2] = r_u32(0x8005BCE8);
    (void)sub_800C6358(order);
    color = r_u32(0x8005AF5C);
    w_u32(0x8005AF5C, (int32_t)color > 0x101088 ? 0x101010 : color + 7);
    color = r_u32(0x8005AF58);
    w_u32(0x8005AF58, (int32_t)color > 0x888080 ? 0x101010 : color + 0x70606);
    (void)sub_800C63CC(rect);
    flags = r_u8(r_u32(0x8005B2F8) + 4);
    position = xport_load_le16(rect) | ((uint32_t)xport_load_le16(rect + 1) << 16);
    dimensions = xport_load_le16(rect + 2) | ((uint32_t)xport_load_le16(rect + 3) << 16);
    if (flags & 0x22)
        (void)sub_800C64F4(order, position, dimensions);
    else if (flags & 4)
        (void)sub_800C67FC(position, dimensions);
    else if (flags & 1)
        (void)sub_800C6FAC();
    else if (flags & 0x10)
        (void)sub_800C79D4(order, position, dimensions);
    else if (flags & 8)
        (void)sub_800C65A0(order, position, dimensions);
    if (r_u8(r_u32(0x8005B2F8) + 4) & 0x10)
    {
        (void)sub_80048FBC(environment);
        (void)sub_8001C304((int16_t)xport_load_le16(environment), (int16_t)xport_load_le16(environment + 2), 384, 240, r_u32(0x8005B598) + 4);
    }
    if (r_u32(0x8005AF50))
    {
        w_u8(r_u32(0x8005B2F8), 2);
        w_u8(0x8005AF4C, 0);
        w_u8(0x800CCC10, 0);
        w_u32(0x8005AF50, 0);
        w_u32(0x800CCC14, 0);
    }
    w_u8(0x8005AF55, 0);
    return 0x80060000;
}

uint32_t sub_800C64F4(uint32_t order[3], uint32_t position, uint32_t dimensions)
{
    uint32_t mode;
    FUNCTION_MARKER(0x800C64F4, "RASHCDG.BIN");
    mode = r_u8(r_u32(0x8005B2F8) + 4);
    if (mode == 36)
        return sub_800C84F8(position, dimensions, 0);
    if (mode == 33)
        return sub_800C84F8(position, dimensions, 1);
    if (mode == 44)
        return sub_800C84F8(position, dimensions, 2);
    return sub_800C6634(order, position, dimensions);
}

uint32_t sub_800C65A0(uint32_t order[3], uint32_t position, uint32_t dimensions)
{
    uint32_t flags;
    FUNCTION_MARKER(0x800C65A0, "RASHCDG.BIN");
    flags = r_u8(r_u32(0x8005B2F8) + 4);
    if (flags & 8)
        return sub_800C6634(order, position, dimensions);
    if (flags & 0x18)
        return sub_800C79D4(order, position, dimensions);
    return flags & 0x18;
}

uint32_t sub_800C6FAC(void)
{
    FUNCTION_MARKER(0x800C6FAC, "RASHCDG.BIN");
    if (r_u8(r_u32(0x8005B2F8) + 4) & 0x10)
        return sub_800C6FF8();
    return sub_800C7510();
}

uint32_t sub_800C84C0(uint32_t actor, uint32_t player)
{
    uint32_t slot, flags, value;
    FUNCTION_MARKER(0x800C84C0, "RASHCDG.BIN");
    flags = r_u32(actor + 1084);
    value = r_u32(flags + 40);
    slot = 0x800D81D8 + 36 * player;
    w_u32(slot, value);
    flags = r_u32(actor + 1084);
    w_u32(slot + 32, r_u8(flags + 39));
    return slot;
}

uint32_t sub_800C6634(uint32_t order[3], uint32_t position, uint32_t dimensions)
{
    uint32_t actor, index, identity, flags, row;
    uint16_t x = (uint16_t)position;
    int16_t y = (int16_t)((position >> 16) + 15);
    int has_player = 0;
    FUNCTION_MARKER(0x800C6634, "RASHCDG.BIN");
    (void)sub_800C5B5C(x | ((uint32_t)(uint16_t)y << 16), dimensions);
    for (index = 0; index < 3; ++index)
    {
        identity = xport_load_le32(order + index);
        if (identity == 224)
            continue;
        actor = r_u32(0x8005B3A0) + 1096 * identity;
        if (r_u16(actor + 172) < r_u32(r_u32(0x8005B2F8) + 48))
        {
            has_player = 1;
            (void)sub_800C84C0(actor, 0);
        }
        flags = r_u32(actor + 1084);
        row = (uint32_t)y + 15 * r_u8(flags + 39);
        (void)sub_800C5C68(x | ((uint32_t)(uint16_t)row << 16), dimensions, actor, 0x505A50);
    }
    if (has_player)
        return 0x80060000;
    actor = r_u32(0x8005B3A0);
    (void)sub_800C84C0(actor, 0);
    if (r_u8(0x8005AF4C))
    {
        flags = r_u32(actor + 1084);
        w_u8(0x8005AF4C, 0);
        w_u8(flags + 39, 255);
        (void)sub_800C84C0(actor, 0);
        row = (uint32_t)y + 75;
    }
    else
        row = (uint32_t)y + 60;
    return sub_800C5DF0(x | ((uint32_t)(uint16_t)row << 16), dimensions, actor);
}

uint32_t sub_800C5B5C(uint32_t position, uint32_t dimensions)
{
    uint16_t rect[4];
    int32_t step = (int16_t)dimensions / 3;
    uint32_t result;
    FUNCTION_MARKER(0x800C5B5C, "RASHCDG.BIN");
    xport_store_le32(rect, position);
    xport_store_le32(rect + 2, dimensions);
    xport_store_le16(rect + 2, step);
    result = r_u8(r_u32(0x8005B2F8) + 4) & 0x10;
    if (result)
        return result;
    (void)sub_8002CB08(r_u32(0x8005AD2C), 97, rect, r_u32(0x8005B598), 0x14506E, 2);
    xport_store_le16(rect, xport_load_le16(rect) + step);
    (void)sub_8002CB08(r_u32(0x8005AD2C), 95, rect, r_u32(0x8005B598), 0x14506E, 2);
    xport_store_le16(rect, xport_load_le16(rect) + step);
    return sub_8002CB08(r_u32(0x8005AD2C), 96, rect, r_u32(0x8005B598), 0x14506E, 2);
}

uint32_t sub_800C83D8(int32_t time, uint32_t mode, uint32_t position, uint32_t dimensions, uint32_t color)
{
    uint8_t text[128];
    uint16_t rect[4];
    int32_t minutes = time / 18000;
    int32_t seconds = time / 300;
    uint32_t remainder_seconds, hundredths;
    FUNCTION_MARKER(0x800C83D8, "RASHCDG.BIN");
    remainder_seconds = (uint32_t)seconds - 60 * (uint32_t)minutes;
    hundredths = (uint32_t)(time / 3) - 100 * (uint32_t)seconds;
    xport_store_le32(rect, position);
    xport_store_le32(rect + 2, dimensions);
    (void)snprintf((char *)text, sizeof(text), (const char *)xport_guest_cptr(0x8005BD48, 1), minutes, (int32_t)remainder_seconds, (int32_t)hundredths);
    return sub_8002CC74(r_u32(0x8005AD2C), text, rect, r_u32(0x8005B598), color, mode);
}

uint32_t sub_800C5C68(uint32_t position, uint32_t dimensions, uint32_t actor, uint32_t color)
{
    uint16_t rect[4];
    uint8_t text[128];
    uint32_t flags, label;
    int32_t step;
    FUNCTION_MARKER(0x800C5C68, "RASHCDG.BIN");
    if (r_u16(actor + 172) < r_u32(r_u32(0x8005B2F8) + 48))
        return sub_800C5DF0(position, dimensions, actor);
    step = (int16_t)dimensions / 3;
    xport_store_le32(rect, position);
    xport_store_le32(rect + 2, dimensions);
    xport_store_le16(rect + 2, step);
    flags = r_u32(actor + 1084);
    label = r_u8(flags + 38);
    (void)snprintf((char *)text, sizeof(text), (const char *)xport_guest_cptr(0x8005BCEC, 1), (int32_t)r_u8(flags + 39));
    (void)sub_8002CC74(r_u32(0x8005AD2C), text, rect, r_u32(0x8005B598), color, 2);
    xport_store_le16(rect, xport_load_le16(rect) + step);
    (void)sub_8002CB08(r_u32(0x8005AD2C), label, rect, r_u32(0x8005B598), color, 2);
    xport_store_le16(rect, xport_load_le16(rect) + step);
    flags = r_u32(actor + 1084);
    return sub_800C83D8(r_s32(flags + 40), 2, xport_load_le32(rect), xport_load_le32(rect + 2), color);
}

uint32_t sub_800C5DF0(uint32_t position, uint32_t dimensions, uint32_t actor)
{
    uint16_t rect[4];
    uint8_t text[128];
    uint32_t flags, color, label, stats[6], result, row;
    int32_t step = (int16_t)dimensions / 3;
    FUNCTION_MARKER(0x800C5DF0, "RASHCDG.BIN");
    xport_store_le32(rect, position);
    xport_store_le32(rect + 2, dimensions);
    xport_store_le16(rect + 2, step);
    flags = r_u32(actor + 1084);
    if ((uint8_t)(r_u8(flags + 39) + 2) < 2)
    {
        (void)sub_8002CB08(r_u32(0x8005AD2C), r_u8(flags + 39) - 226, rect, r_u32(0x8005B598), r_u32(0x8005AF5C), 2);
        xport_store_le16(rect, xport_load_le16(rect) + 2 * step);
        flags = r_u32(actor + 1084);
        (void)sub_8002CB08(r_u32(0x8005AD2C), r_u8(flags + 39) - 226, rect, r_u32(0x8005B598), r_u32(0x8005AF5C), 2);
        color = r_u32(0x8005AF5C);
    }
    else
    {
        (void)snprintf((char *)text, sizeof(text), (const char *)xport_guest_cptr(0x8005BCEC, 1), (int32_t)r_u8(flags + 39));
        (void)sub_8002CC74(r_u32(0x8005AD2C), text, rect, r_u32(0x8005B598), r_u32(0x8005AF58), 2);
        xport_store_le16(rect, xport_load_le16(rect) + 2 * step);
        flags = r_u32(actor + 1084);
        (void)sub_800C83D8(r_s32(flags + 40), 2, xport_load_le32(rect), xport_load_le32(rect + 2), r_u32(0x8005AF58));
        color = r_u32(0x8005AF58);
    }
    label = 169;
    if (!(r_u8(r_u32(0x8005B2F8) + 4) & 8))
        label = r_u8(r_u32(actor + 1084) + 38);
    xport_store_le16(rect, xport_load_le16(rect) - step);
    (void)sub_8002CB08(r_u32(0x8005AD2C), label, rect, r_u32(0x8005B598), color, 2);
    if ((uint8_t)(r_u8(r_u32(actor + 1084) + 39) + 2) < 2)
        return 0x80060000;
    if (r_u8(r_u32(0x8005B2F8) + 4) & 8)
    {
        stats[0] = r_u16(0x800D81F2) + r_u16(0x800D823A);
        stats[1] = r_u8(0x800D81F1) + r_u8(0x800D8239);
        stats[2] = r_u8(0x800D81F0) + r_u8(0x800D8238);
        stats[3] = r_u16(0x800D81F6) + r_u16(0x800D823E);
        stats[4] = r_u8(0x800D81F5) + r_u8(0x800D823D);
        stats[5] = r_u8(0x800D81F4) + r_u8(0x800D823C);
    }
    else
    {
        stats[1] = r_u8(0x800D81F1);
        stats[0] = r_u16(0x800D81F2);
        stats[2] = r_u8(0x800D81F0);
        stats[3] = r_u16(0x800D81F6);
        stats[4] = r_u8(0x800D81F5);
        stats[5] = r_u8(0x800D81F4);
    }
    xport_store_le16(rect + 1, 131);
    xport_store_le16(rect, xport_load_le16(rect) - step);
    (void)sub_8002CB08(r_u32(0x8005AD2C), 179, rect, r_u32(0x8005B598), 0x14506E, 2);
    xport_store_le16(rect, xport_load_le16(rect) + 2 * step);
    (void)sub_8002CB08(r_u32(0x8005AD2C), 180, rect, r_u32(0x8005B598), 0x14506E, 2);
    for (row = 0; row < 3; ++row)
    {
        xport_store_le16(rect + 1, 146 + 15 * row);
        xport_store_le16(rect, xport_load_le16(rect) - 2 * step);
        (void)snprintf((char *)text, sizeof(text), (const char *)xport_guest_cptr(0x8005BCEC, 1), (int32_t)stats[row]);
        (void)sub_8002CC74(r_u32(0x8005AD2C), text, rect, r_u32(0x8005B598), 0x505A50, 2);
        xport_store_le16(rect, xport_load_le16(rect) + step);
        (void)sub_8002CB08(r_u32(0x8005AD2C), 175 + row, rect, r_u32(0x8005B598), 0x14506E, 2);
        xport_store_le16(rect, xport_load_le16(rect) + step);
        (void)snprintf((char *)text, sizeof(text), (const char *)xport_guest_cptr(0x8005BCEC, 1), (int32_t)stats[row + 3]);
        (void)sub_8002CC74(r_u32(0x8005AD2C), text, rect, r_u32(0x8005B598), 0x505A50, 2);
    }
    result = xport_load_le16(rect) - 2 * step;
    xport_store_le16(rect, result);
    return result;
}

uint32_t sub_800C84F8(uint32_t position, uint32_t dimensions, uint32_t mode)
{
    uint16_t rect[4];
    uint8_t text[128];
    uint32_t player = 0, actor, count, players, index, flags, status;
    uint32_t first_label = 0, last_label = 0, table, suffix, name;
    int16_t y = (int16_t)((position >> 16) + 15);
    FUNCTION_MARKER(0x800C84F8, "RASHCDG.BIN");
    xport_store_le32(rect, position);
    xport_store_le16(rect + 1, y);
    xport_store_le32(rect + 2, dimensions);
    count = r_u32(0x8005B1F8);
    if ((int32_t)count > 0)
    {
        players = r_u32(r_u32(0x8005B2F8) + 48);
        actor = r_u32(0x8005B3A0);
        for (index = 0; (int32_t)index < (int32_t)count; ++index)
        {
            if (r_u16(actor + 172) < players)
                player = actor;
            actor += 1096;
        }
    }
    if (!player)
    {
        RRJ_DEBUG_FPRINTF(stderr, "sub_800C84F8: result screen has no player actor\n");
        abort();
    }
    flags = r_u32(player + 1084);
    status = r_u8(flags + 39);
    mode &= 255;
    if ((uint8_t)(status + 2) < 2)
    {
        (void)sub_800C5DF0(xport_load_le32(rect), dimensions, player);
        if (mode != 2)
            goto finish;
        first_label = 127;
        last_label = 128;
    }
    else if (r_u8(0x8005AF4C))
    {
        flags = r_u32(player + 1084);
        w_u8(0x8005AF4C, 0);
        w_u8(flags + 39, 255);
        (void)sub_800C84C0(player, 0);
        (void)sub_800C5DF0(xport_load_le32(rect), dimensions, player);
        goto finish;
    }
    else if (!mode)
    {
        first_label = status == 251 ? 119 : 121;
        last_label = first_label + 1;
    }
    else if (mode == 1)
    {
        if (status != 250 && status != 248)
            goto finish;
        first_label = status == 250 ? 123 : 125;
        last_label = first_label + 1;
    }
    else if (mode == 2)
    {
        if (status == 250)
        {
            status = r_u8(r_u32(0x8005B2F8) + 57);
            if (status == 0)
                last_label = 131;
            else if (status == 1)
            {
                first_label = 129;
                last_label = 130;
            }
            else
                goto finish;
        }
        else if (status == 249)
        {
            name = r_u8(flags + 38);
            table = r_u32(0x8005B544);
            name = r_u32(table + 4 * name);
            suffix = r_u32(table + 528);
            (void)snprintf((char *)text, sizeof(text), (const char *)xport_guest_cptr(0x8005BD58, 1), (const char *)xport_guest_cptr(name, 1), (const char *)xport_guest_cptr(suffix, 1));
            xport_store_le16(rect + 1, y + 45);
            (void)sub_8002CC74(r_u32(0x8005AD2C), text, rect, r_u32(0x8005B598), 0x14506E, 2);
            last_label = 133;
        }
        else
            goto finish;
    }
    else
        goto finish;
    if (first_label)
    {
        xport_store_le16(rect + 1, y + 45);
        (void)sub_8002CB08(r_u32(0x8005AD2C), first_label, rect, r_u32(0x8005B598), 0x14506E, 2);
    }
    xport_store_le16(rect + 1, y + 60);
    (void)sub_8002CB08(r_u32(0x8005AD2C), last_label, rect, r_u32(0x8005B598), 0x14506E, 2);
finish:
    return sub_800C84C0(player, 0);
}

uint32_t sub_800C6FF8(void)
{
    uint16_t rect[4] = {67, 56, 248, 148};
    uint8_t text[128];
    uint32_t base, primary, other, flags, status, table, first, second, name;
    int32_t time, minutes, seconds;
    uint32_t remainder_seconds, hundredths;
    FUNCTION_MARKER(0x800C6FF8, "RASHCDG.BIN");
    base = r_u32(0x8005B3A0);
    if (r_u32(base + 180) < 18)
    {
        primary = base + 1096;
        other = base;
    }
    else
    {
        primary = base;
        other = base + 1096;
    }
    (void)sub_800C84C0(r_u32(0x8005B3A0), 0);
    (void)sub_800C84C0(r_u32(0x8005B3A0) + 1096, 1);
    xport_store_le16(rect + 1, 101);
    flags = r_u32(primary + 1084);
    status = r_u8(flags + 39);
    if (status == 252 || status == 253)
    {
        time = r_s32(flags + 40);
        minutes = time / 18000;
        seconds = time / 300;
        remainder_seconds = (uint32_t)seconds - 60 * (uint32_t)minutes;
        hundredths = (uint32_t)(time / 3) - 100 * (uint32_t)seconds;
        table = r_u32(0x8005B544);
        if (status == 252)
        {
            first = r_u32(table + 624);
            second = r_u32(table + 628);
            name = r_u32(table + 568);
            (void)snprintf((char *)text, sizeof(text), (const char *)xport_guest_cptr(0x8005BD04, 1), (const char *)xport_guest_cptr(second, 1), (const char *)xport_guest_cptr(name, 1), minutes, (int32_t)remainder_seconds, (int32_t)hundredths);
        }
        else
        {
            first = r_u32(table + 632);
            name = r_u32(table + 568);
            (void)snprintf((char *)text, sizeof(text), (const char *)xport_guest_cptr(0x8005BCF0, 1), (const char *)xport_guest_cptr(name, 1), minutes, (int32_t)remainder_seconds, (int32_t)hundredths);
        }
        (void)sub_8002CC74(r_u32(0x8005AD2C), xport_guest_cptr(first, 1), rect, r_u32(0x8005B598), 0x14506E, 2);
        xport_store_le16(rect + 1, 116);
        return sub_8002CC74(r_u32(0x8005AD2C), text, rect, r_u32(0x8005B598), 0x14506E, 2);
    }
    status = r_u8(r_u32(other + 1084) + 39);
    table = r_u32(0x8005B544);
    if (status == 250 || status == 255)
        return sub_8002CC74(r_u32(0x8005AD2C), xport_guest_cptr(r_u32(table + 644), 1), rect, r_u32(0x8005B598), 0x14506E, 2);
    first = r_u32(table + 636);
    second = r_u32(table + 640);
    (void)sub_8002CC74(r_u32(0x8005AD2C), xport_guest_cptr(first, 1), rect, r_u32(0x8005B598), 0x14506E, 2);
    xport_store_le16(rect + 1, 116);
    return sub_8002CC74(r_u32(0x8005AD2C), xport_guest_cptr(second, 1), rect, r_u32(0x8005B598), 0x14506E, 2);
}

uint32_t sub_800C7510(void)
{
    uint16_t rect[4] = {67, 56, 248, 148};
    uint8_t text[128], second_text[128];
    uint32_t actor, flags, status, table, first, second, name, random, index, left_round, right_round;
    const uint8_t *first_line, *second_line;
    int32_t time, minutes, seconds;
    uint32_t remainder_seconds, hundredths;
    FUNCTION_MARKER(0x800C7510, "RASHCDG.BIN");
    actor = r_u32(0x8005B3A0);
    (void)sub_800C84C0(actor, 0);
    xport_store_le16(rect + 1, 101);
    flags = r_u32(actor + 1084);
    status = r_u8(flags + 39);
    if ((uint8_t)(status + 2) < 2)
        return sub_800C5DF0(xport_load_le32(rect), xport_load_le32(rect + 2), actor);
    if (r_u8(0x8005AF4C))
    {
        flags = r_u32(actor + 1084);
        w_u8(0x8005AF4C, 0);
        w_u8(flags + 39, 255);
        (void)sub_800C84C0(actor, 0);
        return sub_800C5DF0(xport_load_le32(rect), xport_load_le32(rect + 2), actor);
    }
    time = r_s32(flags + 40);
    minutes = time / 18000;
    seconds = time / 300;
    remainder_seconds = (uint32_t)seconds - 60 * (uint32_t)minutes;
    hundredths = (uint32_t)(time / 3) - 100 * (uint32_t)seconds;
    table = r_u32(0x8005B544);
    if (status == 252)
    {
        first = r_u32(table + 596);
        name = r_u32(table + 568);
        if (!r_u8(0x800CCC10))
        {
            random = sub_8001FC58();
            w_u8(0x800CCC10, 1);
            w_u32(0x800CCC14, random % 6);
        }
        index = r_u32(0x800CCC14);
        table = r_u32(0x8005B544);
        second = r_u32(table + 600 + 4 * index);
        (void)snprintf((char *)text, sizeof(text), (const char *)xport_guest_cptr(0x8005BD04, 1), (const char *)xport_guest_cptr(first, 1), (const char *)xport_guest_cptr(name, 1), minutes, (int32_t)remainder_seconds, (int32_t)hundredths);
        first_line = text;
        second_line = xport_guest_cptr(second, 1);
    }
    else if (status == 253)
    {
        first = r_u32(table + 560);
        second = r_u32(table + 564);
        name = r_u32(table + 568);
        left_round = r_u8(r_u32(0x8005B2F8) + 7);
        right_round = r_u8(r_u32(0x8005B2F8) + 7);
        (void)snprintf((char *)text, sizeof(text), (const char *)xport_guest_cptr(0x8005BD1C, 1), (const char *)xport_guest_cptr(first, 1), (int32_t)left_round, (const char *)xport_guest_cptr(second, 1), (int32_t)right_round, (const char *)xport_guest_cptr(name, 1), minutes, (int32_t)remainder_seconds, (int32_t)hundredths);
        if (!r_u8(0x800CCC10))
        {
            random = sub_8001FC58();
            w_u8(0x800CCC10, 1);
            w_u32(0x800CCC14, random % 6);
        }
        index = r_u32(0x800CCC14);
        table = r_u32(0x8005B544);
        second = r_u32(table + 572 + 4 * index);
        (void)snprintf((char *)second_text, sizeof(second_text), (const char *)xport_guest_cptr(0x8005BD3C, 1), (const char *)xport_guest_cptr(second, 1));
        first_line = text;
        second_line = second_text;
    }
    else
    {
        first = r_u32(table + 536);
        if (!r_u8(0x800CCC10))
        {
            random = sub_8001FC58();
            w_u8(0x800CCC10, 1);
            w_u32(0x800CCC14, random % 5);
        }
        index = r_u32(0x800CCC14);
        table = r_u32(0x8005B544);
        second = r_u32(table + 540 + 4 * index);
        first_line = xport_guest_cptr(first, 1);
        second_line = xport_guest_cptr(second, 1);
    }
    (void)sub_8002CC74(r_u32(0x8005AD2C), first_line, rect, r_u32(0x8005B598), 0x14506E, 2);
    xport_store_le16(rect + 1, 116);
    return sub_8002CC74(r_u32(0x8005AD2C), second_line, rect, r_u32(0x8005B598), 0x6E5050, 2);
}

uint32_t sub_800C67FC(uint32_t position, uint32_t dimensions)
{
    uint16_t rect[4];
    uint8_t title[24], text[128];
    uint32_t copy[4], temporary[9];
    uint32_t actor, record, flags, status, best, time, color = 0x505A50;
    uint32_t index, candidate, source, target, chunk, word, first, second, rank = 1, selected;
    uint32_t identity, other_identity, result = 0;
    int16_t y = (int16_t)((position >> 16) + 15);
    int32_t step, minutes, seconds;
    uint32_t remainder_seconds, hundredths;
    FUNCTION_MARKER(0x800C67FC, "RASHCDG.BIN");
    xport_store_le32(rect, position);
    xport_store_le16(rect + 1, y);
    xport_store_le32(rect + 2, dimensions);
    actor = r_u32(0x8005B3A0);
    record = 0x80053A88 + 176 * ((uint32_t)r_s8(0x800D80E0) - 56);
    best = r_u32(record + 28);
    flags = r_u32(actor + 1084);
    status = r_u8(flags + 39);
    if (status == 255)
        w_u32(r_u32(actor + 1084) + 40, 99999999);
    else if (status == 250 || status == 254)
    {
        flags = r_u32(actor + 1084);
        status = r_u8(flags + 39);
        if (status == 255)
            w_u32(r_u32(actor + 1084) + 40, 99999999);
        else if (status == 254)
            w_u32(flags + 40, 99999998);
        else if (status == 250)
            w_u32(flags + 40, 99999999);
    }
    else
    {
        time = r_u32(flags + 40);
        if ((int32_t)best >= (int32_t)time)
        {
            best = time;
            w_u8(0x8005AF54, 1);
        }
        else if (r_s32(record + 168) >= (int32_t)time)
            w_u8(0x8005AF54, 2);
    }
    minutes = (int32_t)best / 18000;
    seconds = (int32_t)best / 300;
    remainder_seconds = (uint32_t)seconds - 60 * (uint32_t)minutes;
    hundredths = (uint32_t)((int32_t)best / 3) - 100 * (uint32_t)seconds;
    (void)snprintf((char *)title, sizeof(title), (const char *)xport_guest_cptr(0x8005BCF0, 1), (const char *)xport_guest_cptr(r_u32(r_u32(0x8005B544) + 672), 1), minutes, (int32_t)remainder_seconds, (int32_t)hundredths);
    status = r_u8(0x8005AF54);
    if (status == 1)
    {
        (void)sub_8002CB08(r_u32(0x8005AD2C), 118, rect, r_u32(0x8005B598), r_u32(0x8005AF58), 2);
        color = r_u32(0x8005AF58);
    }
    else if (status == 2)
        (void)sub_8002CB08(r_u32(0x8005AD2C), 117, rect, r_u32(0x8005B598), r_u32(0x8005AF58), 2);
    xport_store_le16(rect + 1, y + 15);
    (void)sub_8002CC74(r_u32(0x8005AD2C), title, rect, r_u32(0x8005B598), color, 2);
    (void)sub_800C84C0(actor, r_u8(0x800D80EB));
    for (index = 0; index < (uint32_t)r_u8(0x800D80EB) + 1; ++index)
    {
        source = 0x800D81D8 + 36 * index;
        target = 0x800CD588 + 36 * index;
        for (chunk = 0; chunk < 8; chunk += 4)
        {
            for (word = 0; word < 4; ++word)
                copy[word] = r_u32(source + 4 * (chunk + word));
            for (word = 0; word < 4; ++word)
                w_u32(target + 4 * (chunk + word), copy[word]);
        }
        w_u32(target + 32, r_u32(source + 32));
        w_u8(0x800CD578 + index, index);
    }
    selected = r_u8(0x800D80EB);
    for (index = 0; index < (uint32_t)r_u8(0x800D80EB) + 1; ++index)
    {
        first = 0x800CD588 + 36 * index;
        for (candidate = index + 1; candidate < (uint32_t)r_u8(0x800D80EB) + 1; ++candidate)
        {
            second = 0x800CD588 + 36 * candidate;
            if (r_s32(second) >= r_s32(first))
                continue;
            for (chunk = 0; chunk < 8; chunk += 4)
            {
                for (word = 0; word < 4; ++word)
                    temporary[chunk + word] = r_u32(second + 4 * (chunk + word));
            }
            temporary[8] = r_u32(second + 32);
            for (chunk = 0; chunk < 8; chunk += 4)
            {
                for (word = 0; word < 4; ++word)
                    copy[word] = r_u32(first + 4 * (chunk + word));
                for (word = 0; word < 4; ++word)
                    w_u32(second + 4 * (chunk + word), copy[word]);
            }
            w_u32(second + 32, r_u32(first + 32));
            for (word = 0; word < 9; ++word)
                w_u32(first + 4 * word, temporary[word]);
            identity = r_u8(0x800CD578 + index);
            other_identity = r_u8(0x800CD578 + candidate);
            w_u8(0x800CD578 + candidate, identity);
            w_u8(0x800CD578 + index, other_identity);
            if (candidate == selected)
                selected = index;
        }
        status = r_u32(first + 32);
        if (status != 250 && status != 255 && status != 254)
            w_u32(first + 32, (uint8_t)rank++);
    }
    step = (int16_t)dimensions / 3;
    xport_store_le16(rect + 2, step);
    for (index = 0; index < (uint32_t)r_u8(0x800D80EB) + 1; ++index)
    {
        record = 0x800CD588 + 36 * index;
        color = index == selected ? r_u32(0x8005AF58) : 0x14506E;
        xport_store_le16(rect + 1, y + 30 + 15 * index);
        xport_store_le16(rect, 67);
        status = r_u32(record + 32);
        if (status == 250 || status == 255 || status == 254)
            (void)sub_8002CB08(r_u32(0x8005AD2C), status == 254 ? 28 : 29, rect, r_u32(0x8005B598), r_u32(0x8005AF5C), 2);
        else
        {
            (void)snprintf((char *)text, sizeof(text), (const char *)xport_guest_cptr(0x8005BCEC, 1), (int32_t)status);
            (void)sub_8002CC74(r_u32(0x8005AD2C), text, rect, r_u32(0x8005B598), color, 2);
        }
        xport_store_le16(rect, xport_load_le16(rect) + step);
        (void)sub_8002CB08(r_u32(0x8005AD2C), r_u8(0x800CD578 + index) + 162, rect, r_u32(0x8005B598), r_u32(0x8005AF5C), 2);
        xport_store_le16(rect, xport_load_le16(rect) + step);
        status = r_u32(record + 32);
        if (status == 250 || status == 255 || status == 254)
            (void)sub_8002CB08(r_u32(0x8005AD2C), status == 254 ? 28 : 29, rect, r_u32(0x8005B598), r_u32(0x8005AF5C), 2);
        else
            (void)sub_800C83D8(r_s32(record), 2, xport_load_le32(rect), xport_load_le32(rect + 2), r_u32(0x8005AF5C));
        result = index + 1 < (uint32_t)r_u8(0x800D80EB) + 1;
    }
    return result;
}

uint32_t sub_800C836C(int32_t value, uint32_t mode, uint32_t position, uint32_t dimensions)
{
    uint8_t text[128];
    uint16_t rect[4];
    FUNCTION_MARKER(0x800C836C, "RASHCDG.BIN");
    xport_store_le32(rect, position);
    xport_store_le32(rect + 2, dimensions);
    (void)snprintf((char *)text, sizeof(text), (const char *)xport_guest_cptr(0x8005BCEC, 1), value);
    return sub_8002CC74(r_u32(0x8005AD2C), text, rect, r_u32(0x8005B598), 0x505A50, mode);
}

uint32_t sub_800C79D4(uint32_t order[3], uint32_t position, uint32_t dimensions)
{
    uint16_t rect[4], columns[2][4];
    uint8_t text[128];
    uint32_t stats[2][2][3], offsets[3] = {2, 1, 0};
    uint32_t actor, actors[2], identity, index, player, kind, metric, source, flags, status, first_status, second_status;
    uint32_t row, cooperative;
    int32_t step = (int16_t)dimensions / 3;
    int32_t height = (int16_t)(dimensions >> 16) / 2;
    FUNCTION_MARKER(0x800C79D4, "RASHCDG.BIN");
    xport_store_le32(rect, position);
    xport_store_le16(rect + 1, xport_load_le16(rect + 1) + 15);
    xport_store_le32(rect + 2, dimensions);
    (void)sub_800C5B5C(xport_load_le32(rect), dimensions);
    for (index = 0; index < 3; ++index)
    {
        identity = xport_load_le32(order + index);
        if (identity == 224)
            continue;
        actor = r_u32(0x8005B3A0) + 1096 * identity;
        if (r_u16(actor + 172) < r_u32(r_u32(0x8005B2F8) + 48))
            (void)sub_800C84C0(actor, identity);
    }
    actors[0] = r_u32(0x8005B3A0);
    (void)sub_800C84C0(actors[0], 0);
    actors[1] = actors[0] + 1096;
    (void)sub_800C84C0(actors[1], 1);
    for (player = 0; player < 2; ++player)
    {
        xport_store_le16(columns[player], xport_load_le16(rect) + 2 * step * player);
        xport_store_le16(columns[player] + 1, xport_load_le16(rect + 1));
        xport_store_le16(columns[player] + 2, step);
        xport_store_le16(columns[player] + 3, height);
    }
    cooperative = r_u8(r_u32(0x8005B2F8) + 4) & 8;
    (void)sub_8002CB08(r_u32(0x8005AD2C), cooperative ? 169 : 171, columns[0], r_u32(0x8005B598), 0x14506E, 0);
    (void)sub_8002CB08(r_u32(0x8005AD2C), cooperative ? 170 : 172, columns[1], r_u32(0x8005B598), 0x14506E, 1);
    for (kind = 0; kind < 2; ++kind)
    {
        for (metric = 0; metric < 3; ++metric)
        {
            for (player = 0; player < 2; ++player)
            {
                source = 0x800D81F0 + 36 * player + 4 * kind + offsets[metric];
                stats[player][kind][metric] = metric == 0 ? r_u16(source) : r_u8(source);
                if (cooperative)
                    stats[player][kind][metric] += metric == 0 ? r_u16(source + 72) : r_u8(source + 72);
            }
        }
    }
    for (row = 0; row < 2; ++row)
    {
        xport_store_le16(rect + 1, xport_load_le16(rect + 1) + 15);
        xport_store_le16(columns[0] + 1, xport_load_le16(rect + 1));
        xport_store_le16(columns[1] + 1, xport_load_le16(rect + 1));
        (void)sub_8002CB08(r_u32(0x8005AD2C), 173 + row, rect, r_u32(0x8005B598), 0x14506E, 2);
        for (player = 0; player < 2; ++player)
        {
            flags = r_u32(actors[player] + 1084);
            status = r_u8(flags + 39);
            if ((uint8_t)(status + 2) < 2)
                (void)sub_8002CB08(r_u32(0x8005AD2C), r_u8(flags + 39) - 226, columns[player], r_u32(0x8005B598), 0x505A50, 2);
            else if (!row)
                (void)sub_800C836C(r_u8(flags + 39), 2, xport_load_le32(columns[player]), xport_load_le32(columns[player] + 2));
            else
                (void)sub_800C83D8(r_s32(flags + 40), 2, xport_load_le32(columns[player]), xport_load_le32(columns[player] + 2), 0x505A50);
        }
    }
    for (row = 0; row < 3; ++row)
    {
        xport_store_le16(rect + 1, xport_load_le16(rect + 1) + 15);
        xport_store_le16(columns[0] + 1, xport_load_le16(rect + 1));
        xport_store_le16(columns[1] + 1, xport_load_le16(rect + 1));
        (void)sub_8002CB08(r_u32(0x8005AD2C), 175 + row, rect, r_u32(0x8005B598), 0x14506E, 2);
        for (player = 0; player < 2; ++player)
        {
            (void)snprintf((char *)text, sizeof(text), (const char *)xport_guest_cptr(0x8005BD40, 1), (int32_t)stats[player][0][row], (int32_t)stats[player][1][row]);
            (void)sub_8002CC74(r_u32(0x8005AD2C), text, columns[player], r_u32(0x8005B598), 0x505A50, 2);
        }
    }
    xport_store_le16(rect + 1, xport_load_le16(rect + 1) + 30);
    xport_store_le16(columns[0] + 1, xport_load_le16(rect + 1));
    xport_store_le16(columns[1] + 1, xport_load_le16(rect + 1));
    (void)sub_8002CB08(r_u32(0x8005AD2C), 178, rect, r_u32(0x8005B598), 0x14506E, 2);
    if (r_u8(0x8005AF55))
    {
        first_status = r_u8(r_u32(actors[0] + 1084) + 39);
        if ((uint8_t)(first_status + 2) < 2)
        {
            second_status = r_u8(r_u32(actors[1] + 1084) + 39);
            if (second_status < 247)
                w_u8(0x800D8212, r_u8(0x800D8212) + 1);
        }
        else
        {
            second_status = r_u8(r_u32(actors[1] + 1084) + 39);
            if (first_status < second_status)
                w_u8(0x800D81EE, r_u8(0x800D81EE) + 1);
            else if ((uint8_t)(second_status + 2) >= 2 && second_status < first_status)
                w_u8(0x800D8212, r_u8(0x800D8212) + 1);
        }
    }
    (void)sub_800C836C(r_s8(0x800D81EE), 2, xport_load_le32(columns[0]), xport_load_le32(columns[0] + 2));
    (void)sub_800C836C(r_s8(0x800D8212), 2, xport_load_le32(columns[1]), xport_load_le32(columns[1] + 2));
    w_u8(0x8005AF4C, 0);
    return 0x80060000;
}

uint32_t sub_8007FF4C(void)
{
    int32_t menu, linked;
    uint32_t next;
    FUNCTION_MARKER(0x8007FF4C, "RASHCDF.BIN");
    (void)sub_80080544();
    (void)sub_800665E8();
    (void)sub_800806D0();
    if (r_u32(0x8005B220))
    {
        w_u8(0x800D80E7, r_u8(0x800D80E7) + 1);
        if (r_s8(0x800D80E7) >= 38)
            w_u8(0x800D80E7, 32);
        (void)sub_8007FD20();
        menu = r_s8(0x800D80E4);
    }
    else
    {
        w_u8(0x800D80DD, r_u8(0x800D80DD) & 0xC7);
        w_u8(0x800D80F1, r_u8(0x800D80F2));
        switch (r_u32(0x800D80D8))
        {
            case 1:
                w_u8(0x800D80F0, 1);
                menu = (int32_t)sub_8007E2C8();
                break;
            case 4:
                w_u8(0x8009C5F2, 0);
                if (r_s8(0x800D80EE))
                {
                    w_u8(0x8009C5F2, 1);
                    w_u16(0x8009C5D4, 24);
                    if (sub_8007E7BC(r_s8(0x800D80E0), r_s32(0x800D81D8)))
                    {
                        w_u8(0x800D80F0, 1);
                        menu = 52;
                    }
                    else
                    {
                        menu = 24;
                        (void)sub_80068688(24);
                    }
                }
                else
                {
                    next = (r_u8(0x800D80EB) + 1) & 255;
                    w_u8(0x8009C5F3, next);
                    menu = next < r_u8(0x800D80EA) ? 40 : 38;
                    w_u8(0x8009C5F2, next >= r_u8(0x800D80EA));
                    w_u16(0x8009C5D4, menu);
                    if (sub_8007E7BC(r_s8(0x800D80E0), r_s32(0x800D81D8 + 36 * r_u8(0x800D80EB))))
                    {
                        w_u8(0x800D80F0, 1);
                        menu = 52;
                    }
                    else if (r_s8(0x8009C5F2))
                        (void)sub_80068688(38);
                }
                break;
            case 8:
                w_u8(0x800D80F0, 1);
                menu = (int32_t)sub_8007DDA8();
                break;
            case 16:
                menu = 30;
                break;
            case 17:
                menu = 32;
                break;
            case 24:
                menu = 34;
                break;
            case 32:
                w_u8(0x800D80F0, 1);
                menu = (int32_t)sub_8007BB34();
                break;
            default:
                menu = 4;
                break;
        }
    }
    (void)sub_80063908();
    (void)sub_800809E0((int16_t)menu);
    linked = r_s16(r_u32(r_u32(0x8009C68C) + 4 * (uint32_t)menu) + 10);
    while (linked != -1)
    {
        (void)sub_800809A8((uint32_t)linked);
        linked = r_s16(r_u32(r_u32(0x8009C68C) + 4 * (uint32_t)linked) + 10);
    }
    (void)sub_80080274();
    (void)sub_8007F37C();
    return sub_80080220();
}

void sub_800403CC(void)
{
    FUNCTION_MARKER(0x800403CC, "SLUS_010.53");
    if (!r_u32(0x800548D0))
        w_u32(0x800548D0, 1);
}

void sub_80063FA0(void)
{
    uint32_t source, destination, index;
    FUNCTION_MARKER(0x80063FA0, "RASHCDI.BIN");
    if (r_u32(0x800D80D8) != 32)
        return;
    w_u16(0x800D5784, r_u16(0x800D81E4));
    w_u8(0x800D5786, r_u8(0x800D81E6));
    w_u8(0x800D5787, r_u8(0x800D81E7));
    w_u32(0x800D5788, r_u32(0x800D81E8));
    source = 0x800D8118;
    destination = 0x800D5814;
    for (index = 0; index < 16; ++index)
    {
        w_u16(destination, r_u16(source));
        w_u8(destination + 2, r_u8(source + 2));
        w_u8(destination + 3, r_u8(source + 3));
        w_u32(destination + 4, r_u32(source + 4));
        destination += 72;
        source += 8;
    }
}

uint32_t sub_8007BB34(void)
{
    uint32_t menu = 10, first_reward = 0, second_reward = 0, small_reward = 0, bonus = 0;
    uint32_t source, destination, index, offset, table, counter, score, extra, factor, shift, bits;
    uint32_t phase, message, amount, limit, old_amount, gain, comparison_gain, output, voice, variant;
    int32_t outcome, kind = 2, level, item, stage, removal;
    FUNCTION_MARKER(0x8007BB34, "RASHCDF.BIN");
    w_u16(0x800D8104, r_u8(0x800D81F1));
    w_u16(0x800D8106, r_u8(0x800D81F5));
    if (!(r_u16(0x800D81E4) & 1) && (r_u32(0x800D81E8) & 15))
        w_u32(0x800D81E8, r_u32(0x800D81E8) & 0xFFFFFFF0);
    w_u16(0x800D81E4, r_u16(0x800D5784));
    w_u8(0x800D81E6, r_u8(0x800D5786));
    w_u8(0x800D81E7, r_u8(0x800D5787));
    w_u32(0x800D81E8, r_u32(0x800D5788));
    source = 0x800D5814;
    destination = 0x800D8118;
    for (index = 0; index < 16; ++index)
    {
        w_u16(destination, r_u16(source));
        w_u8(destination + 2, r_u8(source + 2));
        w_u8(destination + 3, r_u8(source + 3));
        w_u32(destination + 4, r_u32(source + 4));
        source += 72;
        destination += 8;
    }
    sub_8001FC84((3305 * (sub_80043F00(0xF2000002) & 255)) >> 6);
    w_u32(0x8009C67C, 300);
    w_u32(0x8009C680, 300);
    w_u16(0x8009C642, 0);
    w_u16(0x8009C640, 0);
    w_u16(0x8009C644, 0);
    w_u16(0x8009C648, 0);
    w_u16(0x8009C64A, 0);
    w_u16(0x8009C5EC, 0);
    outcome = r_s32(0x800D81F8);
    level = r_s8(0x800D80DC);
    offset = 4 * (uint32_t)level;
    if (outcome == 1 || outcome == 2 || outcome == 3 || outcome == 248 || outcome == 249 || outcome == 251)
    {
        kind = 1;
        table = 0x80089F44;
        counter = 0x800D80F8;
        if (outcome == 2) { table += 24; counter += 2; }
        if (outcome == 3) { table += 48; counter += 4; }
        score = r_u16(table + offset);
        w_u16(counter, r_u16(counter) + 1);
        w_u16(0x8009C640, score);
        index = r_u8(0x800D80E0);
        destination = 0x800D81C8 + (uint32_t)((int32_t)(index << 24) >> 27);
        w_u8(destination, r_u8(destination) | (1 << (index & 7)));
        w_u8(0x800D80E9, r_u8(0x800D80E9) + 1);
    }
    else
    {
        table = 0x8008A0C4;
        counter = 0x800D80FE;
        if (outcome >= 4 && outcome < 17)
            table = 0x80089F44 + 24 * ((uint32_t)outcome - 1);
        else if (outcome == 250)
            table = 0x8008A10C;
        else if (outcome == 254)
        {
            kind = 3;
            table = 0x8008A0F4;
            counter = 0x800D8100;
        }
        else if (outcome == 252 || outcome == 253 || outcome == 255)
        {
            kind = 4;
            table = 0x8008A0DC;
            counter = 0x800D8102;
        }
        score = r_u16(table + offset);
        w_u16(counter, r_u16(counter) + 1);
        w_u16(0x8009C640, score);
    }
    w_u16(0x8009C640, r_u16(0x8009C640) + sub_8001FC58() % 9);
    extra = sub_8001FC58() % 9;
    factor = r_u32(0x80089F2C + offset);
    extra += (r_u8(0x800D81F1) - factor * r_u8(0x800D81F5)) * r_u32(0x80089EE4 + offset);
    extra += (r_u8(0x800D81F0) - factor * r_u8(0x800D81F4)) * r_u32(0x80089EFC + offset);
    extra += (r_u16(0x800D81F2) - factor * r_u16(0x800D81F6)) * r_u32(0x80089F14 + offset);
    w_u16(0x8009C642, extra);
    score = r_u16(0x8009C640);
    w_u16(0x8009C644, (int32_t)(int16_t)score + (int32_t)(int16_t)extra > 0 ? score + extra : 0);
    if ((int32_t)r_u8(0x800D81EC) < r_s32(0x80089E54 + offset))
        w_u8(0x800D81EC, r_u8(0x80089E54 + offset));
    score = (uint32_t)r_s16(0x8009C644);
    if ((int32_t)score >= 63 && kind != 4 && kind != 3)
    {
        if (outcome == 248 || outcome == 251)
            bonus = 1;
        else if ((int32_t)score >= r_s32(0x80089E0C + offset))
        {
            first_reward = 1;
            second_reward = kind == 1;
        }
        else if ((int32_t)score >= r_s32(0x80089DF4 + offset))
        {
            if (outcome < 10)
            {
                first_reward = 1;
                w_u8(0x800D80ED, r_u8(0x800D80ED) + 1);
            }
            else
                small_reward = 1;
        }
        else if ((int32_t)score >= r_s32(0x80089DDC + offset) && outcome < 11)
            small_reward = 1;
    }
    else
    {
        w_u16(0x8009C642, 0);
        w_u16(0x8009C644, 0);
        if (r_s8(0x800D80ED) > 0)
        {
            removal = -1;
            for (item = 5; item >= 0; --item)
                if ((r_u32(0x800D81E8) >> (4 * item)) & 15) { removal = item; break; }
            if (kind == 4)
            {
                if (r_s32(0x80089E54 + offset) < (int32_t)r_u8(0x800D81EC))
                {
                    if (r_s32(0x80089E24 + offset) < (int32_t)r_u8(0x800D81EC))
                        amount = r_u8(0x800D81EC) - r_u8(0x80089E24 + offset);
                    else
                        amount = r_u8(0x80089E54 + offset);
                    w_u8(0x800D81EC, amount);
                    w_u16(0x8009C648, 1525);
                }
            }
            else if (kind == 3)
            {
                if (removal >= 0)
                {
                    w_u32(0x800D81E8, r_u32(0x800D81E8) & ~(15 << (4 * removal)));
                    if (r_u8(0x800D81E6) == (uint32_t)removal)
                        w_u8(0x800D81E7, 0);
                    w_u16(0x8009C648, 1526);
                }
                else
                    for (item = 8; item >= 0; --item)
                        if ((r_u16(0x800D81E4) >> item) & 1)
                        {
                            w_u16(0x800D81E4, r_u16(0x800D81E4) & ~(1 << item));
                            w_u8(0x800D81E6, 9);
                            w_u8(0x800D81E7, 0);
                            w_u16(0x8009C648, 1524);
                            break;
                        }
            }
            w_u8(0x800D80ED, r_u8(0x800D80ED) - 1);
        }
    }
    if (bonus)
    {
        if (level == 1) w_u8(0x800D81EC, 3);
        if (level == 3) w_u8(0x800D81EC, 6);
        w_u16(0x8009C648, 1590);
    }
    for (phase = 0; phase < 3; ++phase)
    {
        if (!(phase == 0 ? small_reward : phase == 1 ? first_reward : second_reward))
            continue;
        if (phase)
            w_u8(0x800D80ED, r_u8(0x800D80ED) + 1);
        stage = r_s8(0x800D80ED) % 8;
        w_u8(0x800D80ED, stage);
        if (stage <= 0 || stage > 7)
        {
            w_u8(0x800D80ED, r_u8(0x800D80ED) + 1);
            stage = 1;
        }
        output = phase == 2 ? 0x8009C64A : 0x8009C648;
        for (;;)
        {
            table = 0;
            gain = 0x80089E9C;
            comparison_gain = gain;
            if (stage == 1)
            {
                table = 0x8008A124 + 24 * (uint32_t)r_s8(0x800D81E1) + offset;
                if (phase && r_s32(table) >= 6)
                    table = 0;
                gain = comparison_gain = 0x80089EB4;
            }
            else if (stage == 2 || (phase && (stage == 4 || stage == 6)))
            {
                if (r_s32(0x80089E84 + offset) >= (int32_t)r_u8(0x800D81EC))
                {
                    gain = stage == 2 ? 0x80089E24 : 0x80089E3C;
                    w_u8(0x800D81EC, r_u8(0x800D81EC) + r_u8(gain + offset));
                    w_u16(output, 1529);
                    break;
                }
                if (!phase || stage == 6)
                    w_u8(0x800D80ED, r_u8(0x800D80ED) + 1);
                ++stage;
                continue;
            }
            else if (stage == 3 && !phase)
            {
                w_u8(0x800D80ED, r_u8(0x800D80ED) + 1);
                stage = 4;
                continue;
            }
            else if (stage == 3)
            {
                index = 24 * (uint32_t)r_s8(0x800D81E1) + offset;
                if (r_s32(0x8008A124 + index) < 6)
                    table = 0x8008A16C + index;
            }
            else if (!phase && (stage == 4 || stage == 5))
                table = 0x8008A16C + 24 * (uint32_t)r_s8(0x800D81E1) + offset;
            else if (stage == 5)
            {
                if (phase == 1)
                    table = 0x8008A1FC + 24 * (uint32_t)r_s8(0x800D81E1) + offset;
                else
                    table = 0x8008A124 + (r_s8(0x800D81E1) ? 0 : 24) + offset;
            }
            else if (phase && stage == 7)
            {
                index = 24 * (uint32_t)r_s8(0x800D81E1) + offset;
                if (r_s32(0x8008A16C + index) < 6)
                    table = 0x8008A1B4 + index;
                if (phase == 2)
                    gain = 0x80089EB4;
            }
            if (table)
            {
                item = r_s32(table);
                bits = r_u16(0x800D81E4);
                if (!((bits >> ((uint32_t)item & 31)) & 1))
                {
                    w_u16(0x800D81E4, bits | (1u << ((uint32_t)item & 31)));
                    w_u16(output, 1527);
                    break;
                }
                if (phase && !(stage == 5 && phase == 1))
                {
                    shift = (4 * (uint32_t)item) & 31;
                    old_amount = item < 8 ? (r_u32(0x800D81E8) >> shift) & 15 : 0;
                    amount = old_amount + r_u32(gain + offset);
                    limit = r_u32(0x80089ECC + offset);
                    message = 1595;
                    if ((int32_t)limit >= (int32_t)amount)
                        message = (int32_t)(amount - r_u32(comparison_gain + offset)) > 0 ? 1594 : 1528;
                    else
                        amount = limit;
                    w_u16(output, message);
                    w_u32(0x800D81E8, r_u32(0x800D81E8) & ~(15u << shift));
                    w_u32(0x800D81E8, r_u32(0x800D81E8) | ((amount & 15) << shift));
                    if (r_u8(0x800D81E6) == (uint32_t)item)
                        w_u8(0x800D81E7, amount);
                    break;
                }
            }
            if (stage == 7 || (!phase && stage >= 6))
            {
                amount = r_u8(0x800D81EC);
                message = r_s32(0x80089E84 + offset) < (int32_t)amount ? 1589 : 1529;
                w_u8(0x800D81EC, amount + 1);
                w_u16(output, message);
                if (phase == 2 && message == 1529)
                    w_u8(0x800D80ED, r_u8(0x800D80ED) + 1);
                break;
            }
            w_u8(0x800D80ED, r_u8(0x800D80ED) + 1);
            ++stage;
        }
    }
    voice = 0;
    message = 0;
    level = r_s8(0x800D80DC);
    switch (level)
    {
        case 1:
            menu = 14;
            if (sub_8007E724(28, 28))
            {
                menu = 12;
                w_u8(0x800D80E9, 0);
                w_u8(0x800D80DC, r_u8(0x800D80DC) + 1);
                w_u8(0x800D80DD, r_u8(0x800D80DD) | 16);
                voice = 0x3330424A;
                message = 1194;
            }
            else { voice = 0x32305450; message = 1198; }
            break;
        case 2:
            if (sub_8007E724(10, 18))
            {
                menu = 15;
                w_u8(0x800D80DC, r_u8(0x800D80DC) + 1);
                voice = 0x3130424A;
                message = 1202;
            }
            else if (kind == 1 && (r_u8(0x800D80E9) == 3 || r_u8(0x800D80E9) == 6))
            {
                voice = r_u8(0x800D80E9) == 3 ? 0x31305450 : 0x31305053;
                message = r_u8(0x800D80E9) == 3 ? 1170 : 1182;
            }
            break;
        case 3:
            menu = 18;
            if (sub_8007E724(29, 29))
            {
                menu = 16;
                w_u8(0x800D80E9, 0);
                w_u8(0x800D80DC, r_u8(0x800D80DC) + 1);
                w_u8(0x800D80DD, r_u8(0x800D80DD) | 16);
                voice = 0x3530424A;
                message = 1206;
            }
            else { voice = 0x32305053; message = 1210; }
            break;
        case 4:
            if (sub_8007E724(19, 27))
            {
                menu = 19;
                w_u8(0x800D80DC, r_u8(0x800D80DC) + 1);
                w_u8(0x800D80DD, r_u8(0x800D80DD) | 16);
                voice = 0x3230424A;
                message = 1214;
            }
            else if (kind == 1 && (r_u8(0x800D80E9) == 3 || r_u8(0x800D80E9) == 6))
            {
                voice = r_u8(0x800D80E9) == 3 ? 0x31305450 : 0x31305053;
                message = r_u8(0x800D80E9) == 3 ? 1174 : 1186;
            }
            break;
        case 5:
            menu = 23;
            if (sub_8007E724(30, 30))
            {
                menu = 20;
                index = r_u8(0x800D80E0);
                destination = 0x800D81C8 + (uint32_t)((int32_t)(index << 24) >> 27);
                w_u8(destination, r_u8(destination) & ~(1 << (index & 7)));
            }
            else { voice = 0x3830424A; message = 1218; }
            break;
        default:
            if (sub_8007E724(1, 9))
            {
                menu = 11;
                w_u8(0x800D80DC, r_u8(0x800D80DC) + 1);
                voice = 0x3430424A;
                message = 1190;
            }
            else if (kind == 1 && (r_u8(0x800D80E9) == 3 || r_u8(0x800D80E9) == 6))
            {
                voice = r_u8(0x800D80E9) == 3 ? 0x31305450 : 0x31305053;
                message = r_u8(0x800D80E9) == 3 ? 1166 : 1178;
            }
            break;
    }
    if (message)
    {
        (void)sub_800782B0(9, 1);
        w_u32(0x8009C67C, voice);
        w_u16(0x8009C5EC, message + (sub_8001FC58() & 3));
    }
    if (!r_s16(0x8009C5EC))
    {
        if (kind == 1)
        {
            index = r_s8(0x800D81E1) != 0;
            (void)sub_800782B0(index ? 27 : 26, 1);
            variant = sub_8001FC58() & 7;
            if (variant >= 3) variant = sub_8001FC58() & 7;
            w_u32(0x8009C67C, r_u32((index ? 0x80089404 : 0x800893DC) + 4 * variant));
            extra = sub_8001FC58() & 3;
            message = 4 * variant + ((!index && variant < 3) ? 1122 : 1134);
        }
        else if (kind == 2)
        {
            index = r_s8(0x800D81E1) != 0;
            (void)sub_800782B0(index ? 29 : 28, 1);
            variant = sub_8001FC58() % 10;
            if (variant >= 5) variant = sub_8001FC58() % 10;
            w_u32(0x8009C67C, r_u32((index ? 0x80089454 : 0x8008942C) + 4 * variant));
            extra = sub_8001FC58() & 3;
            message = 4 * variant + ((!index && variant < 3) ? 1222 : 1234);
            w_u16(0x8009C5EC, message + extra);
            if (variant >= 3 && variant < 5 && r_u8(0x800D81EC) < 2 && !extra)
                w_u8(0x800D81EC, r_u8(0x800D81EC) + r_u8(0x80089E6C + 4 * (uint32_t)r_s8(0x800D80DC)));
        }
        else
        {
            (void)sub_800782B0(kind == 3 ? 20 : 23, 1);
            variant = sub_8001FC58() % (kind == 3 ? 11 : 5);
            w_u32(0x8009C67C, r_u32((kind == 3 ? 0x80089520 : 0x80089574) + 4 * variant));
            extra = sub_8001FC58() & 3;
            message = 4 * variant + (kind == 3 ? 1314 : 1274);
        }
        if (kind != 2)
            w_u16(0x8009C5EC, message + extra);
    }
    w_u32(0x8009C680, kind == 1 ? 0x524E4E57 : kind == 2 ? 0x52534F4C : kind == 3 ? 0x44545342 : 0x444B5257);
    return menu;
}
