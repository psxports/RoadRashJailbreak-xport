#include "rrj_debug.h"
#include "rrj_cd_transport.h"
#include "race_pause_frontier.h"
#include "packet.h"
#include <stdlib.h>
#include <stdio.h>
#include "psx.h"
#include "sdk_build_stubs.h"
#include "native_game_api.h"
#include "draft_quick.h"
#include "game_main.h"
#include "psx_memory.h"
#include "race_pause.h"
#include "race_leaf_batch_004.h"
#include "race_leaf_80080D1C.h"
#include "race_leaf.h"
#include "fixed_math.h"
#include "xport.h"

static void contact_copy(uint16_t output[3], const uint16_t input[3])
{
    unsigned axis;
    for (axis = 0; axis < 3; ++axis)
        xport_store_le16((uint8_t *)output + 2u * axis, xport_load_le16((const uint8_t *)input + 2u * axis));
}

static void contact_negate(uint16_t vector[3])
{
    uint16_t x = xport_load_le16(vector);
    uint16_t z = xport_load_le16((uint8_t *)vector + 4);
    xport_store_le16(vector, (uint16_t)(0u - x));
    x = xport_load_le16((uint8_t *)vector + 2);
    xport_store_le16((uint8_t *)vector + 4, (uint16_t)(0u - z));
    xport_store_le16((uint8_t *)vector + 2, (uint16_t)(0u - x));
}

static int32_t contact_word_dot(const uint32_t vector[3], const uint16_t normal[3])
{
    uint32_t sum = 0;
    unsigned axis;
    for (axis = 0; axis < 3; ++axis)
    {
        int32_t word = ((int32_t)(uint32_t)(xport_load_le32((const uint8_t *)vector + 4u * axis)));
        int32_t half = (int16_t)xport_load_le16((const uint8_t *)normal + 2u * axis) * 16;
        sum += (uint32_t)((uint64_t)((int64_t)word * half) >> 16);
    }
    return ((int32_t)(uint32_t)(sum));
}

static int32_t contact_ratio(int32_t numerator, int32_t denominator)
{
    uint32_t value = sub_80010028(numerator > 0 ? (uint32_t)numerator : 0u - (uint32_t)numerator, denominator > 0 ? (uint32_t)denominator : 0u - (uint32_t)denominator);
    return ((int32_t)(uint32_t)((numerator > 0) == (denominator > 0) ? value : 0u - value));
}

static int32_t contact_angle(uint32_t actor, uint32_t direction, const uint16_t normal[3])
{
    uint32_t forward = sub_8002E698(xport_guest_ptr(direction, 6), normal);
    uint32_t side = sub_8002E698(xport_guest_ptr(actor + 814, 6), normal);
    int32_t angle = ((int32_t)(uint32_t)(25736u * sub_80020018(side, 0u - forward))) >> 8;
    return ((int32_t)(uint32_t)((angle > 0 ? 102943u : 0u - 102943u) - (uint32_t)angle));
}

static void contact_swap(uint32_t actors[2], uint32_t directions[2], uint32_t speeds[2])
{
    uint32_t temporary = actors[0];
    actors[0] = actors[1];
    actors[1] = temporary;
    temporary = directions[0];
    directions[0] = directions[1];
    directions[1] = temporary;
    temporary = speeds[0];
    speeds[0] = speeds[1];
    speeds[1] = temporary;
}

static void contact_faces(uint32_t face, uint32_t *front, uint32_t *back, uint32_t *side)
{
    uint32_t index = face & 255u;
    if (face & 0x200u)
    {
        *front = index == 3;
        *back = index == 1;
        *side = index == 2;
    }
    else
    {
        *front = (index >> 1) & 1u;
        *back = index < 8 && !(face & 2u);
        *side = (face & 1u) ^ ((face & 2u) >> 1);
    }
}

static void contact_cross(uint16_t output[3], const uint16_t left[3], uint32_t right)
{
    unsigned axis;
    for (axis = 0; axis < 3; ++axis)
        xport_gte_write_control(axis * 2u, (uint32_t)(int32_t)(int16_t)xport_load_le16((const uint8_t *)left + 2u * axis));
    for (axis = 0; axis < 3; ++axis)
        xport_gte_write_data(9u + axis, (uint32_t)(int32_t)r_s16(right + 2u * axis));
    xport_gte_execute(0x178000Cu);
    for (axis = 0; axis < 3; ++axis)
        output[axis] = (uint16_t)xport_gte_read_data(9u + axis);
}

uint32_t sub_80081D7C(uint32_t actor, uint32_t other, uint32_t actor_face, uint32_t other_face, uint16_t normal[3])
{
    uint32_t scalar_divisor_8;

    int32_t scalar_value_2;
    int32_t scalar_value_3;
    int32_t scalar_value_4;
    int32_t scalar_value_5;

    uint32_t actors[2] = {actor, other};
    uint32_t directions[2], speeds[2];
    uint32_t delta[3], velocity[3], response_speed[2];
    uint16_t response_normal[2][3];
    int32_t reaction[2] = {0, 0};
    int32_t angle[2], threshold[2], approach[2];
    uint32_t adjustment[2] = {1143, 1143};
    uint32_t front[2], back[2], side[2];
    uint32_t flags[2], player_count;
    unsigned index, axis;
    int general = 1, knockdown = -1, reverse = 0, both_players;
    FUNCTION_MARKER(0x80081D7C, "RASHCDG.BIN");
    for (index = 0; index < 2; ++index)
    {
        uint32_t current = actors[index];
        w_u32(current + 564, r_u32(current + 564) & ~0x4000u);
        if (!(r_u32(current + 568) & 0x02000000u))
        {
            contact_copy(xport_guest_ptr(current + 864, 6), xport_guest_ptr(current + 450, 6));
            w_u32(current + 860, r_u32(current + 480));
            w_u32(current + 568, r_u32(current + 568) | 0x02000000u);
        }
        directions[index] = current + 864;
        speeds[index] = current + 860;
    }
    player_count = r_u32(r_u32(0x8005B2F8u) + 48);
    both_players = r_u16(actor + 172) < player_count && r_u16(other + 172) < player_count;
    flags[0] = r_u32(actor + 568);
    flags[1] = r_u32(other + 568);
    if ((flags[0] | flags[1]) & 0x200u)
    {
        general = 0;
        if ((flags[0] & flags[1]) & 0x200u)
        {
            int32_t minimum = (int32_t)sub_8001FC90(52428, ((int32_t)(uint32_t)(r_u32(speeds[0]))));
            if (((int32_t)(uint32_t)(sub_80084564(normal, directions[0], speeds[0], actors[0] + 456, 5, minimum, 0))) >= 0)
                (void)sub_800849D8(actors[0]);
            contact_negate(normal);
            minimum = (int32_t)sub_8001FC90(52428, ((int32_t)(uint32_t)(r_u32(speeds[1]))));
            if (((int32_t)(uint32_t)(sub_80084564(normal, directions[1], speeds[1], actors[1] + 456, 5, minimum, 0))) >= 0)
                (void)sub_800849D8(actors[1]);
        }
        else
        {
            unsigned active = (flags[0] & 0x200u) ? 0 : 1;
            unsigned passive = 1u - active;
            uint32_t current = actors[active], target = actors[passive];
            int32_t minimum;
            if (active)
                contact_negate(normal);
            minimum = (int32_t)sub_8001FC90(52428, ((int32_t)(uint32_t)(r_u32(speeds[active]))));
            if (((int32_t)(uint32_t)(sub_80084564(normal, directions[active], speeds[active], current + 456, 5, minimum, 0))) >= 0)
                (void)sub_800849D8(current);
            flags[passive] = r_u32(target + 568);
            if (flags[passive] & 0x400u)
            {
                for (axis = 0; axis < 3; ++axis)
                    delta[axis] = r_u32(current + 184 + 4u * axis) - r_u32(target + 184 + 4u * axis);
                if (contact_word_dot(delta, xport_guest_ptr(current + 450, 6)) >= 0 || (r_u32(current + 560) & 0x20000000u))
                {
                    contact_negate(normal);
                    (void)sub_80083F30(target, current + 172, normal, 5);
                }
                else
                {
                    int32_t speed;
                    w_u32(target + 568, r_u32(target + 568) & ~0x400u);
                    w_u32(target + 568, r_u32(target + 568) | 0xA00u);
                    contact_copy(xport_guest_ptr(target + 450, 6), xport_guest_ptr(current + 450, 6));
                    speed = (int32_t)sub_8001FC90(52428, ((int32_t)(uint32_t)(r_u32(current + 480))));
                    w_u32(target + 480, speed < 1638400 ? 1638400u : (uint32_t)speed);
                    w_u16(target + 796, r_u16(target + 454));
                    w_u16(target + 798, 0);
                    w_u16(target + 800, (uint16_t)(0u - r_u16(target + 450)));
                    w_u32(target + 488, 327680);
                    w_u32(target + 728, 0);
                }
            }
            else if (!(flags[passive] & 0x7FFu) && (both_players || r_u16(target + 172) >= r_u32(r_u32(0x8005B2F8u) + 48)) && !(r_u32(target + 560) & 0x20000000u))
            {
                w_u32(target + 568, flags[passive] | 0x200840u);
                knockdown = (int)passive;
            }
        }
    }
    else if ((flags[0] | flags[1]) & 0x400u)
    {
        int32_t height;
        for (axis = 0; axis < 3; ++axis)
            delta[axis] = r_u32(other + 184 + 4u * axis) - r_u32(actor + 184 + 4u * axis);
        height = contact_word_dot(delta, xport_guest_ptr(actor + 438, 6));
        if (((int32_t)(uint32_t)(r_u32(actor + 312))) / 2 < ((int32_t)(((int32_t)(height)) < 0 ? ((int32_t)(uint32_t)(0u - (uint32_t)((int32_t)(height)))) : ((int32_t)(height)))))
        {
            unsigned lower = height > 0 ? 0 : 1;
            unsigned upper = 1u - lower;
            uint32_t current = actors[upper];
            uint32_t current_flags = r_u32(current + 568);
            if (current_flags & 0x400u)
            {
                int32_t minimum = (int32_t)sub_8001FC90(52428, ((int32_t)(uint32_t)(r_u32(speeds[upper]))));
                (void)sub_80084564(xport_guest_ptr(actors[lower] + 522, 6), directions[upper], speeds[upper], current + 456, 0, minimum, 0);
            }
            else if (!(current_flags & 0x7FFu) && (both_players || r_u16(current + 172) >= r_u32(r_u32(0x8005B2F8u) + 48)) && !(r_u32(current + 560) & 0x20000000u))
            {
                knockdown = (int)upper;
                w_u32(current + 568, current_flags | 0x200840u);
                w_u32(actors[lower] + 828, current + 172);
            }
            general = 0;
        }
    }
    if (!general)
    {
        if (knockdown >= 0)
            (void)sub_8001A760(r_u16(actors[knockdown] + 172), 1);
        return 1;
    }
    for (index = 0; index < 2; ++index)
        (void)sub_8002EE50(r_u32(speeds[index]), xport_guest_ptr(directions[index], 6), xport_guest_ptr(actors[index] + 456, 12));
    for (axis = 0; axis < 3; ++axis)
        velocity[axis] = r_u32(actors[0] + 456 + 4u * axis) - r_u32(actors[1] + 456 + 4u * axis);
    angle[0] = contact_angle(actors[0], directions[0], normal);
    threshold[0] = ((int32_t)(uint32_t)((uint32_t)angle[0] ^ r_u32(actors[0] + 488))) < 0 ? 74348 : 62910;
    contact_faces(actor_face, &front[0], &back[0], &side[0]);
    contact_faces(other_face, &front[1], &back[1], &side[1]);
    if (((front[0] | back[0]) & (front[1] | back[1]) & ~(back[1] & back[0])) != 0)
    {
        if (front[0] & back[1])
            contact_swap(actors, directions, speeds);
        else if (!(back[0] & front[1]))
        {
            int32_t difference;
            unsigned special = 0;
            approach[0] = contact_word_dot(velocity, xport_guest_ptr(directions[1], 6));
            approach[1] = ((int32_t)(uint32_t)(0u - (uint32_t)contact_word_dot(velocity, xport_guest_ptr(directions[0], 6))));
            difference = ((int32_t)(uint32_t)(r_u32(speeds[1]) - r_u32(speeds[0])));
            if (difference < -655360)
            {
                int32_t temporary = approach[0];
                special = 1;
                contact_swap(actors, directions, speeds);
                approach[0] = approach[1];
                approach[1] = temporary;
            }
            else if (difference > 655360)
                special = 1;
            contact_copy(response_normal[1], xport_guest_ptr(directions[0], 6));
            contact_copy(response_normal[0], xport_guest_ptr(directions[1], 6));
            for (index = 0; index < 2; ++index)
            {
                if (index < special)
                    reaction[index] = 16 + 16 * (approach[index] < -1464860 && !(r_u32(actors[index] + 560) & 0x20000000u));
                else
                {
                    reaction[index] = approach[index] < -3222693 ? 8 : approach[index] < -1757832 ? 4 : 2;
                    if (r_u32(actors[index] + 560) & 0x20000000u)
                        reaction[index] = 2;
                }
            }
            goto apply_reactions;
        }
        if (((int32_t)(uint32_t)(r_u32(speeds[0]))) < ((int32_t)(uint32_t)(r_u32(speeds[1]))))
        {
            contact_copy(response_normal[1], xport_guest_ptr(directions[0], 6));
            contact_copy(response_normal[0], xport_guest_ptr(directions[1], 6));
            contact_negate(response_normal[0]);
            contact_negate(response_normal[1]);
            reverse = 1;
            reaction[0] = reaction[1] = -1;
        }
    }
    else if (((scalar_value_2 = (angle[0])), (((int32_t)(scalar_value_2 < 0 ? ((int32_t)(uint32_t)(0u - (uint32_t)scalar_value_2)) : scalar_value_2)))) > threshold[0])
    {
        unsigned receiving, striking;
        int outside;
        contact_copy(response_normal[0], normal);
        contact_copy(response_normal[1], normal);
        contact_negate(response_normal[1]);
        approach[0] = contact_word_dot(velocity, normal);
        approach[1] = ((int32_t)(uint32_t)(0u - (uint32_t)approach[0]));
        striking = approach[0] > 0 || ((int32_t)(uint32_t)(r_u32(speeds[1]) - r_u32(speeds[0]))) > 878916 ? 1 : 0;
        receiving = 1u - striking;
        reaction[receiving] = approach[striking] < -1464860 ? 32 : 16;
        if (r_u32(actors[receiving] + 560) & 0x20000000u)
            reaction[receiving] = 16;
        /* The original reads an unwritten second angle and threshold on this branch */
        if (striking == 1)
        {
            angle[1] = contact_angle(actors[1], directions[1], response_normal[1]);
            threshold[1] = ((int32_t)(uint32_t)((uint32_t)angle[1] ^ r_u32(actors[1] + 488))) < 0 ? 74348 : 62910;
        }
        outside = angle[striking] < -threshold[striking] || angle[striking] > threshold[striking];
        reaction[striking] = !outside ? 1 : approach[striking] < -3222693 ? 8 : approach[striking] < -1757832 ? 4 : 0;
        if ((r_u32(actors[striking] + 560) & 0x20000000u) && reaction[striking] != 1)
            reaction[striking] = 0;
        if (!reaction[striking])
            reaction[striking] = -1;
    }
    else if (r_u32(actor + 828) != other + 172 && r_u32(other + 828) != actor + 172)
    {
        if (((int32_t)(uint32_t)(r_u32(speeds[0]))) <= 0x10000 && ((int32_t)(uint32_t)(r_u32(speeds[1]))) <= 0x10000)
        {
            for (index = 0; index < 2; ++index)
            {
                contact_copy(response_normal[index], xport_guest_ptr(actors[index] + 814, 6));
                if (side[index])
                    contact_negate(response_normal[index]);
            }
        }
        else
        {
            int32_t scale = contact_ratio(((int32_t)(uint32_t)(r_u32(actors[1] + 316))), ((int32_t)(uint32_t)(r_u32(actors[0] + 316))));
            uint32_t magnitude, second_magnitude, third_magnitude, divisor, reciprocal;
            (void)sub_8002E570(xport_guest_ptr(actors[0] + 456, 12), xport_guest_ptr(actors[1] + 456, 12), (uint32_t)scale, delta);
            while (((scalar_value_3 = (((int32_t)(uint32_t)(delta[0])))), (((int32_t)(scalar_value_3 < 0 ? ((int32_t)(uint32_t)(0u - (uint32_t)scalar_value_3)) : scalar_value_3)))) > 5931008 || ((scalar_value_4 = (((int32_t)(uint32_t)(delta[1])))), (((int32_t)(scalar_value_4 < 0 ? ((int32_t)(uint32_t)(0u - (uint32_t)scalar_value_4)) : scalar_value_4)))) > 5931008 || ((scalar_value_5 = (((int32_t)(uint32_t)(delta[2])))), (((int32_t)(scalar_value_5 < 0 ? ((int32_t)(uint32_t)(0u - (uint32_t)scalar_value_5)) : scalar_value_5)))) > 5931008)
                for (axis = 0; axis < 3; ++axis)
                    delta[axis] = (uint32_t)(((int32_t)(uint32_t)(delta[axis])) >> 1);
            magnitude = sub_8002E548(delta);
            second_magnitude = sub_8002E548(delta);
            third_magnitude = sub_8002E548(delta);
            if (((int32_t)(uint32_t)(magnitude)) < 0)
            {
                divisor = (uint32_t)(((int32_t)(uint32_t)(0u - second_magnitude)) >> 1) + (uint32_t)(((int32_t)(uint32_t)(0u - third_magnitude - 2u)) >> 31);
                reciprocal = 0u - ((uint32_t)(((uint32_t)(divisor)) ? 0x80000000u / ((uint32_t)(divisor)) : UINT32_MAX));
            }
            else
            {
                divisor = (uint32_t)(((int32_t)(uint32_t)(second_magnitude)) >> 1) + (uint32_t)(((int32_t)(uint32_t)(third_magnitude - 2u)) >> 31);
                reciprocal = ((uint32_t)(((uint32_t)(divisor)) ? 0x80000000u / ((uint32_t)(divisor)) : UINT32_MAX));
            }
            (void)sub_8002EED8(reciprocal, delta, response_normal[0]);
            contact_cross(response_normal[1], response_normal[0], actors[0] + 522);
            contact_copy(response_normal[0], response_normal[1]);
            if (((int32_t)(uint32_t)(sub_8002E698(response_normal[0], xport_guest_ptr(directions[0], 6)))) > 0)
                contact_negate(response_normal[0]);
            else
                contact_negate(response_normal[1]);
        }
        reaction[0] = reaction[1] = 1;
        approach[0] = contact_word_dot(velocity, response_normal[0]);
        approach[1] = ((int32_t)(uint32_t)(0u - (uint32_t)contact_word_dot(velocity, response_normal[1])));
        if ((actor_face & 0x200u) && (other_face & 0x200u))
            adjustment[0] = adjustment[1] = 0;
    }
apply_reactions:
    if (!(reaction[0] | reaction[1]))
        return (uint32_t)reaction[0];
    for (index = 0; index < 2; ++index)
    {
        angle[index] = contact_angle(actors[index], directions[index], response_normal[index]);
        threshold[index] = ((int32_t)(uint32_t)((uint32_t)angle[index] ^ r_u32(actors[index] + 488))) < 0 ? 74348 : 62910;
        if (r_u32(actors[index] + 568) & 0x100u)
            reaction[index] = 16;
    }
    {
        unsigned order[2] = {0, 1};
        if (reaction[0] >= 16 || reaction[1] >= 16 || reverse)
        {
            uint32_t mass = r_u32(actors[0] + 316) + r_u32(actors[1] + 316);
            uint32_t reciprocal = ((scalar_divisor_8 = ((uint32_t)(((int32_t)(uint32_t)(mass)) >> 1) + (uint32_t)(((int32_t)(uint32_t)(mass - 2u)) >> 31))), (((uint32_t)(scalar_divisor_8 ? 0x80000000u / scalar_divisor_8 : UINT32_MAX))));
            uint32_t dot = sub_8002E698(xport_guest_ptr(directions[0], 6), xport_guest_ptr(directions[1], 6));
            int32_t projected, first_mass, second_mass;
            order[0] = reaction[1] >= 16;
            order[1] = !order[0];
            projected = (int32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(speeds[order[0]]))), ((int32_t)(uint32_t)(dot)));
            first_mass = (int32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(actors[order[0]] + 316))), ((int32_t)(uint32_t)(reciprocal)));
            second_mass = (int32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(actors[order[1]] + 316))), ((int32_t)(uint32_t)(reciprocal)));
            if (projected < 0)
                projected = 0;
            (void)sub_80083864(first_mass, second_mass, projected, ((int32_t)(uint32_t)(r_u32(speeds[order[1]]))), &response_speed[order[0]], &response_speed[order[1]]);
        }
        else
        {
            response_speed[0] = r_u32(speeds[0]);
            response_speed[1] = r_u32(speeds[1]);
        }
        for (index = 0; index < 2; ++index)
        {
            unsigned selected = order[index], opposite = !selected;
            uint32_t current = actors[selected], current_flags = r_u32(current + 568);
            if (current_flags & 0x80u)
            {
                adjustment[selected] = 11438;
                w_u32(current + 560, r_u32(current + 560) | 0x04000000u);
            }
            if (!reaction[selected])
                continue;
            current_flags = r_u32(current + 568);
            if (current_flags & 0xFu)
            {
                current_flags &= 0xFFF007F0u;
                w_u32(current + 568, current_flags);
                w_u32(current + 828, 0);
                w_u32(current + 568, current_flags | 0x08000000u);
            }
            if (reaction[selected] == -1)
                w_u32(current + 564, r_u32(current + 564) | 0x4000u);
            else
                w_u32(current + 568, r_u32(current + 568) | (uint32_t)reaction[selected]);
            if (reaction[selected] >= 16)
                reaction[selected] = ((int32_t)(uint32_t)(sub_80080B10(current, directions[selected], speeds[selected], directions[opposite], ((int32_t)(uint32_t)(response_speed[selected])))));
            else
            {
                /* The -1 response sets actor+564 bit 4000 and never consumes bias */
                int32_t bias = reaction[selected] == -1 ? 0 : (int32_t)sub_8001FC90(approach[selected], 2443);
                reaction[selected] = ((int32_t)(uint32_t)(sub_80080D1C(current, directions[selected], speeds[selected], actors[opposite] + 172, response_normal[selected], bias, angle[selected], threshold[selected], ((int32_t)(uint32_t)(response_speed[selected])), ((int32_t)(uint32_t)(adjustment[selected])), 0)));
            }
            current_flags = r_u32(current + 568);
            w_u32(current + 568, current_flags | ((current_flags & 0x1Fu) ? 0x1000u : ((current_flags & 0x220u) != 0) << 11));
        }
    }
    return (uint32_t)reaction[0] | (uint32_t)reaction[1];
}

uint32_t sub_80014000(uint32_t a1)
{
    FUNCTION_MARKER(0x80014000u, "SLUS_010.53");
    uint32_t result = r_u16(a1 + 172);
    if (result)
    {
        uint32_t p = r_u32(a1 + 276);
        w_u16(a1 + 172, 0);
        if (r_s8(p) < 0)
            w_u8(p, 0);
        result = r_u32(0x8005B314u) - 1;
        w_u32(0x8005B314u, result);
    }
    return result;
}

void sub_80014264(void)
{
    FUNCTION_MARKER(0x80014264u, "SLUS_010.53");
}

uint32_t sub_8001E928(void)
{
    FUNCTION_MARKER(0x8001E928u, "SLUS_010.53");
    w_u32(0x8005B4A4u, 1);
    return 1;
}

uint32_t sub_80027028(uint32_t a1, uint32_t a2)
{
    FUNCTION_MARKER(0x80027028u, "SLUS_010.53");
    uint32_t p = 0x800537DAu + 6 * r_u32(a1 + 180) + 2 * a2;
    return r_s8(p) != -1;
}

uint32_t sub_8002D298(void)
{
    FUNCTION_MARKER(0x8002D298u, "SLUS_010.53");
    uint32_t index;
    for (index = 0; index < 18; ++index)
    {
        uint32_t address = 0x800D80D8u + 252u + (index >> 3);
        w_u8(address, r_u8(address) & ~(1u << (index & 7)));
    }
    return 0;
}

uint32_t sub_800144E8(uint32_t a1)
{
    FUNCTION_MARKER(0x800144E8u, "SLUS_010.53");
    return sub_8001447C(a1, 1);
}

uint32_t sub_80014910(uint32_t a1, uint32_t a2)
{
    FUNCTION_MARKER(0x80014910u, "SLUS_010.53");
    (void)sub_80015C88(xport_guest_ptr(a1, 4), a2);
    return 0;
}

uint32_t sub_80027540(uint32_t actor, uint32_t side, uint32_t kind)
{
    FUNCTION_MARKER(0x80027540u, "SLUS_010.53");
    uint32_t header = r_u32(0x8005B2F8u);
    uint32_t actor_index = r_u16(actor + 172);
    uint32_t result = r_u32(header + 48);
    uint32_t id, effect, bits, resource, source, indices, vertex;
    uint32_t actor_data, model_index, model_list, model;
    int32_t source_vertex, source_indices, index;
    if (actor_index >= result)
        return result;
    result = (uint32_t)(int32_t)r_s8(actor + 8);
    if (((int32_t)(uint32_t)(result)) > 0)
        return result;
    result = (r_u32(actor + 36) >> 30) < 2;
    if (!result)
        return result;
    id = sub_80027178();
    if (id == UINT32_MAX)
        return UINT32_MAX;
    bits = r_u32(actor + 36);
    w_u32(actor + 36, (bits & 0x3FFFFFFFu) | (((bits >> 30) + 1) << 30));
    effect = 0x800D39B0u + 112u * id;
    bits = r_u32(effect);
    w_u8(effect + 60, 0);
    bits = (bits & 0xFFFFFC3Fu) | 0x100u;
    w_u32(effect, bits);
    bits = ((bits & 0xFFC03FFFu) | ((side & 255u) << 14)) & 0xFFFFC3FFu;
    header = r_u32(0x8005B2F8u);
    w_u32(effect, bits | 0x400u);
    resource = r_u32(header + 16);
    w_u8(effect + 61, 0);
    header = r_u32(0x8005B2F8u);
    w_u32(effect + 52, 900);
    w_u16(effect + 62, 30);
    w_u32(effect + 96, 9);
    w_u32(effect + 36, 0);
    w_u32(effect + 44, 0);
    w_u32(effect + 40, 0);
    w_u32(effect + 48, resource);
    resource = r_u32(header + 16);
    w_u8(effect + 108, (uint8_t)kind);
    w_u8(effect + 109, 4);
    w_u32(effect + 100, resource);
    for (uint32_t i = 0; i < 4; ++i)
        w_u8(effect + 104 + i, (uint8_t)(i + 1));
    source = 0x800537DAu + 6u * r_u32(actor + 180);
    bits = r_u32(effect);
    actor_data = r_u32(actor);
    source += (bits >> 13) & 0x1FEu;
    source_indices = r_s8(source + 1);
    model_index = r_u16(actor_data + 24);
    source_vertex = r_s8(source);
    indices = 0x800536F0u + 4u * (uint32_t)source_indices;
    model_list = r_u32(actor + 4);
    model = r_u32(model_list + 24u * model_index - 24u);
    vertex = r_u32(model + 20);
    index = r_s8(indices + 2);
    vertex += 20u * (uint32_t)source_vertex + 4u;
    w_u16(effect + 76, r_u16(vertex + 2u * (uint32_t)index + 12u));
    index = r_s8(indices + 3);
    w_u16(effect + 78, r_u16(vertex + 2u * (uint32_t)index + 12u));
    return sub_800271CC(actor, id);
}

uint32_t sub_8001556C(uint32_t offset)
{
    uint8_t position[8];
    uint32_t sector;
    FUNCTION_MARKER(0x8001556Cu, "SLUS_010.53");
    while (r_u32(0x800D6728u))
        rrj_native_wait_events();
    sector = offset + r_u32(0x8005B3D8u);
    w_u32(0x800D6720u, sector);
    (void)sub_80015C88(position, sector);
    (void)CdSyncCallbackPSX(0x8001578Cu);
    w_u32(0x800D6728u, 3u);
    w_u32(0x800D6724u, UINT32_MAX);
    (void)CdSync(0, NULL);
    return (uint32_t)CdControlF(2u, position);
}

uint32_t sub_80015A18(uint32_t event, const uint8_t *response)
{
    uint32_t state, first, length;
    FUNCTION_MARKER(0x80015A18u, "SLUS_010.53");
    w_u32(0x800D6724u, UINT32_MAX);
    w_u32(0x8005AD68u, r_u32(0x8005AD68u) + 1u);
    (void)CdSyncCallbackPSX(0u);
    if (response[0] & 16u)
        (void)sub_80015B60();
    else
        (void)CdSync(0, NULL);
    if (r_u32(0x800D6728u) >= 6u)
        return 0x80010000u;
    state = r_u32(0x800D6728u);
    switch (state)
    {
        case 0u:
        case 3u:
        case 4u:
            first = r_u32(0x800D6720u);
            length = r_u32(0x8005B3D8u);
            w_u32(0x800D6728u, 0u);
            return sub_8001556C(first - length);
        case 1u:
        case 2u:
        case 5u:
            first = r_u32(0x800D6714u);
            length = r_u32(0x800D671Cu);
            w_u32(0x800D6728u, 0u);
            return sub_8001564C(xport_guest_ptr(first, 1), length);
        default:
            /* TODO Bind a jump target if an interrupt changes the checked table index */
            RRJ_DEBUG_FPUTS("RRJ: unsupported CD recovery jump-table target\n", stderr);
            abort();
    }
}

uint64_t sub_80015B04(void)
{
    volatile uint32_t count = 0u;
    uint32_t previous;
    FUNCTION_MARKER(0x80015B04u, "SLUS_010.53");
    do
    {
        previous = count;
        count = previous + 1u;
    } while (previous <= 1499999u);
    return ((uint64_t)1u << 32) | (previous + 1u);
}

uint32_t sub_80015B60(void)
{
    uint8_t response[8];
    uint32_t result;
    FUNCTION_MARKER(0x80015B60u, "SLUS_010.53");
    do
    {
        (void)CdControl(1u, response, NULL);
    } while (CdStatus() & 16u);
    (void)CdSync(0, NULL);
    (void)sub_80015B04();
    (void)sub_80015B04();
    (void)sub_80015B04();
    (void)sub_80015B04();
    (void)sub_80015B04();
    (void)sub_80015B04();
    (void)sub_80015B04();
    do
    {
        (void)CdControl(19u, NULL, response);
        result = (uint32_t)CdSync(0, response);
    } while (result != 2u);
    return result;
}

uint32_t sub_80035E60(uint32_t output, uint32_t player_index)
{
    FUNCTION_MARKER(0x80035E60u, "SLUS_010.53");
    uint32_t count = 0;
    uint32_t triple = player_index * 3u;
    uint32_t index = triple * 4u;
    uint32_t end = index + 12u;
    uint32_t entry = 0x800D87E8u + triple * 448u;
    if (((int32_t)(uint32_t)(index)) < ((int32_t)(uint32_t)(end)))
    {
        do
        {
            if (r_u32(entry) != UINT32_MAX && r_u32(entry + 8u) != UINT32_MAX && r_u32(entry + 4u) && (sub_800363F0(entry) & 255u))
            {
                w_u32(output, index);
                output += 4u;
                ++count;
            }
            ++index;
            entry += 112u;
        } while (((int32_t)(uint32_t)(index)) < ((int32_t)(uint32_t)(end)));
    }
    return count;
}

uint32_t sub_80050EC8(uint32_t destination, uint32_t bytes)
{
    FUNCTION_MARKER(0x80050EC8u, "SLUS_010.53");
    uint32_t limited = bytes > 0x7EFF0u ? 0x7EFF0u : bytes;
    /* TODO Bind the original SDK SPU read transfer */
    (void)rrj_missing_spu_read(destination, limited);
    if (!r_u32(0x8005A454u))
        w_u32(0x8005A450u, 0);
    return limited;
}

static uint32_t contact_pair_dot(const uint32_t point[2], const uint32_t reference[2], const uint32_t normal[2])
{
    uint32_t x = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(point[0] - reference[0])), ((int32_t)(uint32_t)(normal[0])));
    uint32_t y = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(point[1] - reference[1])), ((int32_t)(uint32_t)(normal[1])));
    return x + y;
}

static uint32_t contact_five_quarters(uint32_t value)
{
    uint32_t product = (value << 2) + value;
    if (((int32_t)(uint32_t)(product)) < 0)
        product += 3;
    return (uint32_t)(((int32_t)(uint32_t)(product)) >> 2);
}

uint32_t sub_800AA474(uint32_t actor, uint32_t other, uint32_t linked, uint32_t angle, uint32_t lateral, uint32_t longitudinal, uint32_t *actor_face, uint32_t *other_face, uint32_t translation[3])
{
    uint32_t owners[2], local[4][2], projected[4][2], normals[4][2], distances[4];
    uint32_t first_vector[3], second_vector[3];
    uint32_t *ordered_faces[2], *reference_faces, *point_faces;
    uint32_t(*references)[2], (*points)[2];
    uint32_t reference_owner = 0, point_owner = 0, hit[2] = {131, 131};
    uint32_t width, height, corner, selected_point = 0, selected_edge = 0, scale_x, scale_y;
    unsigned orientation, pass, vertex, edge, axis;
    int found = 0;

    FUNCTION_MARKER(0x800AA474, "RASHCDG.BIN");
    if (linked & 2u)
        other = r_u32(other + 856);
    else if (linked & 1u)
        actor = r_u32(actor + 856);
    orientation = ((int32_t)(uint32_t)(longitudinal)) < 1;
    if (((int32_t)(uint32_t)(angle)) >= 2049)
        orientation ^= 1u;
    if ((((int32_t)(uint32_t)(lateral)) > 0 && orientation == 1) || (((int32_t)(uint32_t)(lateral)) <= 0 && orientation == 0))
    {
        owners[0] = actor;
        owners[1] = other;
        ordered_faces[0] = actor_face;
        ordered_faces[1] = other_face;
    }
    else
    {
        owners[0] = other;
        owners[1] = actor;
        ordered_faces[0] = other_face;
        ordered_faces[1] = actor_face;
    }
    width = r_u32(owners[0] + 304);
    height = r_u32(owners[0] + 312);
    local[0][0] = 0u - width;
    local[0][1] = 0;
    local[1][0] = width;
    local[1][1] = 0;
    local[2][0] = width;
    local[2][1] = 0u - height;
    local[3][0] = 0u - width;
    local[3][1] = 0u - height;
    for (vertex = 0; vertex < 4; ++vertex)
    {
        static const uint32_t corners[2][4] = {{3, 2, 6, 7}, {0, 1, 5, 4}};
        corner = owners[1] + 196 + 12u * corners[orientation][vertex];
        projected[vertex][0] = sub_800B6AAC(xport_guest_ptr(corner, 12), xport_guest_ptr(owners[0] + 432, 6), xport_guest_ptr(owners[0] + 184, 12));
        projected[vertex][1] = sub_800B6AAC(xport_guest_ptr(corner, 12), xport_guest_ptr(owners[0] + 438, 6), xport_guest_ptr(owners[0] + 184, 12));
    }
    for (pass = 0; pass < 2; ++pass)
    {
        uint32_t inside = 0;
        int rotated = 0;
        if (!pass)
        {
            reference_owner = owners[0];
            point_owner = owners[1];
            references = local;
            points = projected;
            reference_faces = ordered_faces[0];
            point_faces = ordered_faces[1];
            normals[0][0] = 0x10000;
            normals[0][1] = 0;
            normals[1][0] = 0;
            normals[1][1] = 0xFFFF0000u;
            normals[2][0] = 0xFFFF0000u;
            normals[2][1] = 0;
            normals[3][0] = 0;
            normals[3][1] = 0x10000;
        }
        else
        {
            uint32_t previous[2] = {projected[3][0], projected[3][1]};
            reference_owner = owners[1];
            point_owner = owners[0];
            references = projected;
            points = local;
            reference_faces = ordered_faces[1];
            point_faces = ordered_faces[0];
            for (edge = 0; edge < 4; ++edge)
            {
                uint32_t squared, root, second_root, third_root, divisor, reciprocal;
                normals[edge][0] = projected[edge][1] - previous[1];
                normals[edge][1] = previous[0] - projected[edge][0];
                squared = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(normals[edge][0])), ((int32_t)(uint32_t)(normals[edge][0])));
                squared += (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(normals[edge][1])), ((int32_t)(uint32_t)(normals[edge][1])));
                if (((int32_t)(uint32_t)(squared)) < 17)
                {
                    xport_store_le32(other_face, 0);
                    xport_store_le32(actor_face, 0);
                    return 0;
                }
                root = sub_8004CF74(squared) << 2;
                second_root = sub_8004CF74(squared) << 2;
                third_root = sub_8004CF74(squared) << 2;
                if (((int32_t)(uint32_t)(root)) < 0)
                {
                    second_root = 0u - second_root;
                    third_root = 0u - third_root;
                }
                divisor = (uint32_t)(((int32_t)(uint32_t)(second_root)) >> 1) + (uint32_t)(((int32_t)(uint32_t)(third_root - 2u)) >> 31);
                reciprocal = ((uint32_t)(((uint32_t)(divisor)) ? 0x80000000u / ((uint32_t)(divisor)) : UINT32_MAX));
                if (((int32_t)(uint32_t)(root)) < 0)
                    reciprocal = 0u - reciprocal;
                normals[edge][0] = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(reciprocal)), ((int32_t)(uint32_t)(normals[edge][0])));
                normals[edge][1] = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(reciprocal)), ((int32_t)(uint32_t)(normals[edge][1])));
                previous[0] = projected[edge][0];
                previous[1] = projected[edge][1];
            }
            rotated = ((int32_t)(uint32_t)(normals[0][0])) < 1024;
        }
        for (vertex = 0; vertex < 4; ++vertex)
        {
            for (edge = 0; edge < 4; ++edge)
            {
                distances[edge] = contact_pair_dot(points[vertex], references[edge], normals[edge]);
                if (((int32_t)(uint32_t)(distances[edge] + 1024u)) < 0)
                    break;
            }
            if (edge == 4)
            {
                inside = 1u << vertex;
                break;
            }
        }
        if (inside)
        {
            uint32_t shifted[2], value, separation;
            int32_t ratio;
            int positive = ((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(point_owner + 468, 12), xport_guest_ptr(reference_owner + 516, 6), xport_guest_ptr(reference_owner + 468, 12)))) >= 0;
            if (positive)
            {
                selected_point = inside & 1u ? 0u : 3u;
                selected_edge = rotated ? (((int32_t)(uint32_t)(normals[0][1])) < 0 ? 4u : 5u) : 2u;
                shifted[0] = points[selected_point][0] + 0x80000u;
            }
            else
            {
                selected_point = inside & 2u ? 1u : 2u;
                selected_edge = rotated ? (((int32_t)(uint32_t)(normals[0][1])) < 0 ? 5u : 4u) : 0u;
                shifted[0] = points[selected_point][0] - 0x80000u;
            }
            shifted[1] = points[selected_point][1];
            /* Decode face codes before indexing four edges */
            if (selected_edge == 4)
                selected_edge = 1;
            else if (selected_edge == 5)
                selected_edge = 3;
            value = contact_pair_dot(shifted, references[selected_edge], normals[selected_edge]);
            separation = distances[selected_edge] - value;
            if (((int32_t)(uint32_t)(separation)) < 131 && ((int32_t)(uint32_t)(value)) < 131)
                found = 1;
            else
            {
                ratio = contact_ratio(((int32_t)(uint32_t)(0u - value)), ((int32_t)(uint32_t)(separation)));
                if (ratio >= -15)
                {
                    hit[0] = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(0x10000u - (uint32_t)ratio)), ((int32_t)(uint32_t)(shifted[0])));
                    hit[0] += (uint32_t)sub_8001FC90(ratio, ((int32_t)(uint32_t)(points[selected_point][0])));
                    hit[1] = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(0x10000u - (uint32_t)ratio)), ((int32_t)(uint32_t)(shifted[1])));
                    hit[1] += (uint32_t)sub_8001FC90(ratio, ((int32_t)(uint32_t)(points[selected_point][1])));
                    found = 1;
                }
            }
        }
        if (found)
            break;
    }
    if (!found)
    {
        xport_store_le32(other_face, 0);
        xport_store_le32(actor_face, 0);
        return 0;
    }
    xport_store_le32(point_faces, selected_point | 0x100u);
    xport_store_le32(reference_faces, (selected_edge == 1 ? 4u : selected_edge == 3 ? 5u : selected_edge) | 0x200u);
    if (selected_edge == 0 || selected_edge == 2)
        xport_store_le32(point_faces, (selected_edge ^ 2u) | 0x200u);
    scale_x = contact_five_quarters(hit[0] - points[selected_point][0]);
    scale_y = contact_five_quarters(hit[1] - points[selected_point][1]);
    (void)sub_8002EE50(scale_x, xport_guest_ptr(owners[0] + 432, 6), first_vector);
    (void)sub_8002EE50(scale_y, xport_guest_ptr(owners[0] + 438, 6), second_vector);
    for (axis = 0; axis < 3; ++axis)
        xport_store_le32((uint8_t *)translation + 4u * axis, first_vector[axis] + second_vector[axis]);
    return point_owner;
}

uint32_t sub_800901A8(uint32_t event_type)
{
    uint8_t event[8] = {0};
    uint32_t result, actor, linked;
    int32_t remaining;
    FUNCTION_MARKER(0x800901A8u, "RASHCDG.BIN");
    result = r_u32(0x800CE4DCu);
    remaining = ((int32_t)(uint32_t)(r_u32(result)));
    actor = r_u32(0x800CE4D0u);
    if (remaining >= 0)
        do
        {
            sub_800BCD10(actor);
            xport_store_le16(event, (uint16_t)event_type);
            xport_store_le16(event + 2, 224u);
            sub_800BCA68(event, 1u, actor);
            linked = r_u32(actor + 852u);
            if (r_u8(linked + 572u) & 0x10u)
            {
                linked = r_u32(actor + 856u);
                sub_800BCD10(linked);
                linked = r_u32(actor + 856u);
                sub_800BCA68(event, 1u, linked);
            }
            result = r_u32(0x800CE4D4u);
            --remaining;
            actor += result;
        } while (remaining >= 0);
    return result;
}

static uint32_t effect_distance(uint32_t horizontal, uint32_t depth)
{
    uint32_t small = horizontal, large = depth, adjusted;
    if (((int32_t)(uint32_t)(depth)) < ((int32_t)(uint32_t)(horizontal)))
    {
        small = depth;
        large = horizontal;
    }
    adjusted = small + ((uint32_t)(((uint32_t)(small)) & 0x80000000u ? ~(~((uint32_t)(small)) >> ((unsigned)(1u))) : ((uint32_t)(small)) >> ((unsigned)(1u))));
    return large - ((uint32_t)(((uint32_t)(large)) & 0x80000000u ? ~(~((uint32_t)(large)) >> ((unsigned)(5u))) : ((uint32_t)(large)) >> ((unsigned)(5u)))) - ((uint32_t)(((uint32_t)(large)) & 0x80000000u ? ~(~((uint32_t)(large)) >> ((unsigned)(7u))) : ((uint32_t)(large)) >> ((unsigned)(7u)))) + ((uint32_t)(((uint32_t)(adjusted)) & 0x80000000u ? ~(~((uint32_t)(adjusted)) >> ((unsigned)(2u))) : ((uint32_t)(adjusted)) >> ((unsigned)(2u)))) + ((uint32_t)(((uint32_t)(adjusted)) & 0x80000000u ? ~(~((uint32_t)(adjusted)) >> ((unsigned)(6u))) : ((uint32_t)(adjusted)) >> ((unsigned)(6u))));
}

uint32_t sub_800A2138(uint32_t player_index)
{
    uint32_t scalar_value_19;
    uint32_t scalar_value_21;

    static const uint32_t matrix_offsets[9] = {0u, 6u, 12u, 2u, 8u, 14u, 4u, 10u, 16u};
    uint32_t effect_index, effect, selected, best, view, player;
    uint32_t horizontal, depth, sign, delta, count, list, entry, source, distance;
    uint32_t x, y, z, matrix, element, shape;
    int32_t candidate;
    FUNCTION_MARKER(0x800A2138u, "RASHCDG.BIN");
    for (effect_index = 0u; effect_index < 3u; ++effect_index)
    {
        effect = r_u32(0x8005B304u) + 280u * effect_index;
        if (!r_u16(effect + 172u))
            continue;
        if (r_u32(effect + 176u) == UINT32_MAX)
        {
            selected = UINT32_MAX;
            best = UINT32_MAX;
            view = player_index;
            if (((int32_t)(uint32_t)(view)) < 2)
                do
                {
                    player = r_u32(0x8005B268u + 4u * view);
                    delta = r_u32(effect + 192u) - r_u32(player + 192u);
                    sign = ((uint32_t)(((uint32_t)(delta)) & 0x80000000u ? ~(~((uint32_t)(delta)) >> ((unsigned)(31u))) : ((uint32_t)(delta)) >> ((unsigned)(31u))));
                    depth = (sign + ((uint32_t)(((uint32_t)(delta)) & 0x80000000u ? ~(~((uint32_t)(delta)) >> ((unsigned)(16u))) : ((uint32_t)(delta)) >> ((unsigned)(16u))))) ^ sign;
                    delta = r_u32(effect + 184u) - r_u32(player + 184u);
                    sign = ((uint32_t)(((uint32_t)(delta)) & 0x80000000u ? ~(~((uint32_t)(delta)) >> ((unsigned)(31u))) : ((uint32_t)(delta)) >> ((unsigned)(31u))));
                    horizontal = (sign + ((uint32_t)(((uint32_t)(delta)) & 0x80000000u ? ~(~((uint32_t)(delta)) >> ((unsigned)(16u))) : ((uint32_t)(delta)) >> ((unsigned)(16u))))) ^ sign;
                    if (effect_distance(horizontal, depth) < 120u)
                    {
                        selected = r_u32(player + 176u);
                        break;
                    }
                    count = r_u32(0x8005B568u + 4u * view);
                    candidate = ((int32_t)(uint32_t)(count - 1u));
                    list = 0x800D9B80u + 48u * view + 4u * (uint32_t)candidate;
                    while (candidate >= 0)
                    {
                        entry = r_u32(list);
                        if (entry != UINT32_MAX)
                        {
                            entry = 0x800D87ECu + 112u * entry;
                            source = r_u32(entry);
                            delta = (uint32_t)r_s16(effect + 194u) - ((scalar_value_19 = (r_u32(source + 16u))), (((uint32_t)(scalar_value_19 & 0x80000000u ? ~(~scalar_value_19 >> ((unsigned)(6u))) : scalar_value_19 >> ((unsigned)(6u))))));
                            sign = ((uint32_t)(((uint32_t)(delta)) & 0x80000000u ? ~(~((uint32_t)(delta)) >> ((unsigned)(31u))) : ((uint32_t)(delta)) >> ((unsigned)(31u))));
                            depth = (sign + delta) ^ sign;
                            delta = (uint32_t)r_s16(effect + 186u) - ((scalar_value_21 = (r_u32(source + 8u))), (((uint32_t)(scalar_value_21 & 0x80000000u ? ~(~scalar_value_21 >> ((unsigned)(6u))) : scalar_value_21 >> ((unsigned)(6u))))));
                            sign = ((uint32_t)(((uint32_t)(delta)) & 0x80000000u ? ~(~((uint32_t)(delta)) >> ((unsigned)(31u))) : ((uint32_t)(delta)) >> ((unsigned)(31u))));
                            horizontal = (sign + delta) ^ sign;
                            distance = effect_distance(horizontal, depth);
                            if (distance < best)
                            {
                                best = distance;
                                selected = r_u32(entry + 4u);
                            }
                        }
                        --candidate;
                        list -= 4u;
                    }
                    ++view;
                } while (((int32_t)(uint32_t)(view)) < 2);
            w_u32(effect + 176u, selected);
        }
        x = r_u32(effect + 184u);
        z = r_u32(effect + 192u);
        w_u32(effect + 12u, ((uint32_t)(((uint32_t)(x)) & 0x80000000u ? ~(~((uint32_t)(x)) >> ((unsigned)(10u))) : ((uint32_t)(x)) >> ((unsigned)(10u)))));
        y = r_u32(effect + 188u);
        w_u32(effect + 20u, ((uint32_t)(((uint32_t)(z)) & 0x80000000u ? ~(~((uint32_t)(z)) >> ((unsigned)(10u))) : ((uint32_t)(z)) >> ((unsigned)(10u)))));
        shape = (uint32_t)r_s8(effect + 72u);
        w_u32(effect + 16u, ((uint32_t)(((uint32_t)(y)) & 0x80000000u ? ~(~((uint32_t)(y)) >> ((unsigned)(10u))) : ((uint32_t)(y)) >> ((unsigned)(10u)))));
        matrix = shape == 3u ? effect + 104u : r_u32(effect + 4u) + 4u;
        for (element = 0u; element < 9u; ++element)
        {
            uint16_t value = r_u16(effect + 196u + 2u * element);
            w_u16(matrix + matrix_offsets[element], value);
        }
        sub_80067AC4(effect, player_index);
    }
    return 0u;
}

void sub_80064CC8(uint32_t player_index)
{
    SVECTOR direction = {0};
    sint32 screen, projection, flags;
    uint32_t player, angle, phase, center, span, sector, first, end;
    uint32_t packet, context, bytes, progress, column, x, y, row, resource, slot;
    int32_t width, rounded, half_span;
    int16_t tile;

    FUNCTION_MARKER(0x80064CC8u, "RASHCDG.BIN");
    player = r_u32(0x8005AEC0u + 4u * player_index);
    angle = r_u16(player + 124u) & 0xFFFu;
    direction.vx = (int16_t)r_u16(0x8005624Cu + 4u * angle);
    player = r_u32(0x8005AEC0u + 4u * player_index);
    angle = r_u16(player + 124u) & 0xFFFu;
    direction.vz = (int16_t)r_u16(0x8005624Eu + 4u * angle);
    RotTransPers(&direction, &screen, &projection, &flags);
    y = (uint32_t)(int16_t)((uint32_t)screen >> 16) - (uint32_t)r_s16(0x800D513Cu) - 31u;
    if (r_u32(0x8005B2D0u) && r_u32(r_u32(0x8005B2F8u) + 48u) == 1u)
    {
        w_u16(0x800D5EECu, 7u);
        w_u16(0x800D5EEEu + 2u * player_index, 192u);
        center = 192u;
    }
    else
    {
        uint32_t offsets = r_u32(0x8005B474u) + 8u * player_index;
        width = r_s16(offsets + 4u);
        rounded = width + 63;
        if (rounded < 0)
            rounded = width + 126;
        w_u16(0x800D5EECu, (uint16_t)(((rounded >> 6) + 1) | 1));
        center = r_u16(offsets) + (uint32_t)(r_s16(offsets + 4u) >> 1);
        w_u16(0x800D5EEEu + 2u * player_index, (uint16_t)center);
    }
    player = r_u32(0x8005AEC0u + 4u * player_index);
    angle = r_u16(player + 124u) & 0xFFFu;
    phase = 27u * angle >> 6;
    sector = (uint32_t)((int16_t)phase >> 6);
    span = r_u16(0x800D5EECu);
    half_span = (int16_t)span >> 1;
    first = sector - (uint32_t)half_span;
    if (((int32_t)(uint32_t)(first)) < 0)
        first += 27u;
    end = first + span;
    x = center - (phase - r_u16(0x800D5EF2u + 2u * sector)) - ((uint32_t)half_span << 6);
    bytes = 48u * (uint32_t)r_s16(0x800D5EECu);
    context = r_u32(0x8005B470u);
    packet = r_u32(context + 268u);
    if (packet + bytes >= r_u32(0x8005B4D0u))
    {
        packet = sub_80021C98(packet, bytes);
        w_u32(r_u32(0x8005B470u) + 268u, packet);
    }
    bytes = 48u * (uint32_t)r_s16(0x800D5EECu);
    context = r_u32(0x8005B470u);
    packet = r_u32(context + 268u);
    w_u32(context + 268u, packet + bytes);
    column = first;
    for (progress = first; (int16_t)progress < (int16_t)end; ++progress, ++column, x += 64u)
    {
        uint32_t tile_y = y;
        if ((int16_t)column == 27)
            column = 0u;
        for (row = 0u; (int16_t)row < 2; ++row, tile_y += 31u)
        {
            tile = r_s16(0x800D56C4u + 4u * (uint32_t)(int16_t)column + 2u * (uint32_t)(int16_t)row);
            if (tile == -1)
                continue;
            w_u32(packet, 0x05000000u);
            resource = 0x800D5144u + 44u * (uint32_t)(int32_t)tile;
            w_u32(packet + 4u, r_u32(resource + 4u));
            w_u16(packet + 12u, (uint16_t)x);
            w_u16(packet + 14u, (uint16_t)tile_y);
            w_u32(packet + 8u, r_u32(0x800D5140u));
            w_u32(packet + 20u, 0x001F0040u);
            w_u32(packet + 16u, r_u32(resource));
            slot = r_u32(0x8005B59Cu);
            w_u32(packet, (r_u32(packet) & 0xFF000000u) | (r_u32(slot) & 0x00FFFFFFu));
            w_u32(slot, (r_u32(slot) & 0xFF000000u) | (packet & 0x00FFFFFFu));
            packet += 24u;
        }
    }
}

uint32_t sub_8005BE20(uint32_t animation)
{
    uint32_t flags, result = 0xFFFFFFFDu;
    FUNCTION_MARKER(0x8005BE20u, "RASHCDG.BIN");
    flags = r_u32(animation + 36u);
    if (flags & 2u)
    {
        result = flags & 0xFFFFFFFDu;
        w_u32(animation + 36u, result);
    }
    return result;
}

uint32_t sub_8005BE44(uint32_t animation)
{
    uint32_t flags;
    FUNCTION_MARKER(0x8005BE44u, "RASHCDG.BIN");
    flags = r_u32(animation + 36u);
    w_u32(animation + 28u, 0u);
    flags |= 2u;
    w_u32(animation + 36u, flags);
    return flags;
}

uint32_t sub_80085224(uint32_t actor, uint32_t player_index)
{
    static const uint8_t offsets[9] = {0, 6, 12, 2, 8, 14, 4, 10, 16};
    uint32_t x, y, z, matrix, element;
    int32_t shape;
    FUNCTION_MARKER(0x80085224u, "RASHCDG.BIN");
    x = r_u32(actor + 184u);
    z = r_u32(actor + 192u);
    w_u32(actor + 12u, (uint32_t)(((int32_t)(uint32_t)(x)) >> 10));
    y = r_u32(actor + 188u);
    w_u32(actor + 20u, (uint32_t)(((int32_t)(uint32_t)(z)) >> 10));
    shape = r_s8(actor + 72u);
    w_u32(actor + 16u, (uint32_t)(((int32_t)(uint32_t)(y)) >> 10));
    matrix = shape == 3 ? actor + 104u : r_u32(actor + 4u) + 4u;
    for (element = 0u; element < 9u; ++element)
        w_u16(matrix + offsets[element], r_u16(actor + 432u + 2u * element));
    return sub_80067AC4(actor, player_index);
}

uint32_t sub_800B2E64(uint32_t actor, uint32_t bounds)
{
    uint32_t face, depth, corner[3], translation[3], identity;
    uint16_t normal[3];
    uint32_t box = bounds + 24u, basis = bounds + 260u;
    FUNCTION_MARKER(0x800B2E64u, "RASHCDG.BIN");
    if (sub_800B7030(xport_guest_cptr(actor + 196u, 96), xport_guest_cptr(basis, 18), xport_guest_cptr(box, 96), 16u, &face, &depth) == 8u)
        return 0u;
    sub_800B675C(xport_guest_cptr(box, 96), xport_guest_cptr(basis, 18), xport_load_le32(&face), normal, corner);
    sub_8002EE50(xport_load_le32(&depth) + 0x2000u, normal, translation);
    identity = r_u16(actor + 172u);
    sub_800A8DF0(xport_guest_ptr(actor, 1), translation, (identity >> 5) != 4u || (identity & 31u) < 30u);
    identity = r_u16(actor + 172u);
    if ((identity >> 5) != 4u || (identity & 31u) < 30u)
    {
        if (r_u32(actor + 592u) & 2u)
            sub_800B3344(actor, 0u, normal, 0xFFFFFFFFu);
        else
            w_u32(actor + 480u, 0u);
    }
    return 1u;
}

uint32_t sub_800B2794(uint32_t actor, uint32_t normal)
{
    uint32_t speed, sign, magnitude, difference, level, result;
    int32_t quotient;
    FUNCTION_MARKER(0x800B2794u, "RASHCDG.BIN");
    speed = r_u32(actor + 480u);
    sign = (uint32_t)(((int32_t)(uint32_t)(speed)) >> 31);
    magnitude = (speed + sign) ^ sign;
    if (((int32_t)(uint32_t)(magnitude)) < 0)
        magnitude += 0x1FFFFu;
    quotient = ((int32_t)(uint32_t)(magnitude)) >> 17;
    level = ~(uint32_t)(((int32_t)(uint32_t)(magnitude)) >> 31) & (uint32_t)quotient;
    difference = 4u - (uint32_t)quotient;
    level += (uint32_t)(((int32_t)(uint32_t)(difference)) >> 31) & difference;
    result = sub_800A9868(actor, xport_guest_cptr(normal, 6), 0, xport_guest_cptr(normal, 6), actor + 172u, 0u);
    if (result && ((int32_t)(uint32_t)(level)) > 0)
        return sub_80017BA0(((int32_t)(uint32_t)(r_u32(actor + 184u))), ((int32_t)(uint32_t)(r_u32(actor + 192u))), 19u, 0u);
    return result;
}

uint32_t sub_800B59F0(uint32_t record)
{
    uint32_t index, existing = 0u, axis;
    int32_t alignment;
    int found = 0;
    FUNCTION_MARKER(0x800B59F0u, "RASHCDG.BIN");
    index = r_u32(0x800CCF68u) - 1u;
    while (((int32_t)(uint32_t)(index)) >= 0)
    {
        existing = 0x800CCE48u + 36u * index;
        if (r_u16(existing) == r_u16(record) && r_u16(existing + 2u) == r_u16(record + 2u))
        {
            found = 1;
            break;
        }
        --index;
    }
    if (!found)
        return 1u;
    alignment = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(record + 4u, 6), xport_guest_ptr(existing + 4u, 6))));
    if (alignment > 0xE000)
    {
        if (((int32_t)(uint32_t)(r_u32(record + 32u))) >= ((int32_t)(uint32_t)(r_u32(existing + 32u))))
            sub_8001E0B4(xport_guest_ptr(existing, 36u), xport_guest_ptr(record, 36), 36u);
    }
    else if (alignment > -46333)
    {
        for (axis = 0u; axis < 3u; ++axis)
            w_u32(existing + 16u + 4u * axis, r_u32(existing + 16u + 4u * axis) + r_u32(record + 16u + 4u * axis));
    }
    return 0u;
}

void sub_80097518(uint32_t actor, int32_t delta)
{
    uint32_t remaining = (uint32_t)delta;
    uint32_t route, resolved, segment, position, limit, sign;
    int32_t next;
    FUNCTION_MARKER(0x80097518u, "RASHCDG.BIN");
    if (!actor || delta <= 0 || r_s16(actor + 320u))
        return;
    do
    {
        sub_8003AF9C(actor + 172u, 1u, 0u);
        route = r_u32(actor + 360u);
        if ((route >> 16) == 1u)
        {
            resolved = sub_8003A3CC(actor, route & 0xFFFFu);
            if (resolved == 0xFFFFFFFFu)
                return;
            segment = sub_8003B4B0(r_u32(actor + 428u), resolved);
            if (!segment)
                return;
            w_u32(actor + 360u, resolved & 0xFFFFu);
            w_u32(actor + 364u, r_u32(segment + 4u));
            position = ((int32_t)(uint32_t)(r_u32(segment + 4u))) <= 0 ? r_u32(segment + 8u) : 0u;
            w_u32(actor + 368u, position);
        }
        else
        {
            segment = sub_8003B4B0(r_u32(actor + 428u), route & 0xFFFFu);
            if (!segment)
                return;
        }
        next = -1;
        if (((int32_t)(uint32_t)(r_u32(segment + 4u))) > 0)
        {
            position = r_u32(actor + 368u) + remaining;
            w_u32(actor + 368u, position);
            limit = r_u32(segment + 8u);
            remaining = 0u;
            if (((int32_t)(uint32_t)(limit)) < ((int32_t)(uint32_t)(position)))
            {
                next = r_s16(segment + 14u);
                remaining = position - limit;
                if (next == -1)
                    w_u32(actor + 368u, limit);
            }
        }
        else
        {
            position = r_u32(actor + 368u) - remaining;
            w_u32(actor + 368u, position);
            if (((int32_t)(uint32_t)(position)) >= 0)
                remaining = 0u;
            else
            {
                next = r_s16(segment + 12u);
                sign = (uint32_t)(((int32_t)(uint32_t)(position)) >> 31);
                remaining = (position + sign) ^ sign;
                if (next == -1)
                    w_u32(actor + 368u, 0u);
            }
        }
        if (next != -1)
        {
            w_u32(actor + 360u, (uint32_t)(uint16_t)next | 0x10000u);
            w_u32(actor + 368u, 0u);
        }
    } while (((int32_t)(uint32_t)(remaining)) > 0);
}

uint32_t sub_80075BE4(uint32_t actor, int32_t delta)
{
    FUNCTION_MARKER(0x80075BE4u, "RASHCDG.BIN");
    uint32_t specification = r_u32(actor + 556u);
    uint32_t selected = 0, index, step, target, value, flags, sign, increment;
    uint32_t lower, upper, lower_mask, upper_mask, steering, product, quotient;
    uint32_t random, remainder, result;
    int32_t maximum = 0;
    int64_t scaled;
    for (index = 0; index < 29; ++index)
    {
        int32_t candidate = r_s32(specification + 60u + 4u * index);
        if (maximum < candidate)
        {
            selected = index;
            maximum = candidate;
        }
    }
    step = r_u32(specification + 184u);
    target = r_u32(specification + 176u) + selected * step;
    increment = (uint32_t)sub_8001FC90(655360000, delta);
    value = r_u32(actor + 604u) + increment;
    w_u32(actor + 604u, value);
    if ((int32_t)value < (int32_t)target)
        target = value;
    flags = r_u32(actor + 560u);
    w_u32(actor + 604u, target);
    if (flags & 0x300u)
    {
        sign = 0u - ((flags >> 8) & 1u);
        increment = (uint32_t)sub_8001FC90((int32_t)((sign & 0xFFFF79F6u) + 17157u), delta);
        value = r_u32(actor + 636u) + increment;
        lower = value + 17157u;
        upper = 17157u - value;
        lower_mask = (uint32_t)((int32_t)lower >> 31);
        upper_mask = (uint32_t)((int32_t)upper >> 31);
        w_u32(actor + 636u, value + (lower_mask & (0u - 17157u - value)) + (upper_mask & upper));

        increment = (uint32_t)sub_8001FC90((int32_t)((sign & 0xFFFE0000u) | 0x10000u), delta);
        value = r_u32(actor + 488u) + increment;
        lower = value + 65536u;
        upper = 65536u - value;
        lower_mask = (uint32_t)((int32_t)lower >> 31);
        upper_mask = (uint32_t)((int32_t)upper >> 31);
        w_u32(actor + 488u, value + (lower_mask & (0u - 65536u - value)) + (upper_mask & upper));

        increment = (uint32_t)sub_8001FC90((int32_t)((sign & 0xFFFF999Au) + 13107u), delta);
        value = r_u32(actor + 676u) + increment;
        steering = r_u32(actor + 636u);
        lower = value + 13107u;
        upper = 13107u - value;
        lower_mask = (uint32_t)((int32_t)lower >> 31);
        upper_mask = (uint32_t)((int32_t)upper >> 31);
        w_u32(actor + 676u, value + (lower_mask & (0u - 13107u - value)) + (upper_mask & upper));
        product = steering * 341u;
        scaled = (int64_t)(int32_t)product * INT64_C(0x3D1DD3BF);
        quotient = (uint32_t)(int32_t)(scaled >> 44) - (uint32_t)((int32_t)product >> 31);
        w_u16(actor + 826u, quotient);
    }
    value = r_u32(actor + 636u);
    sign = (uint32_t)((int32_t)value >> 31);
    if ((int32_t)((value + sign) ^ sign) < 1143)
    {
        random = sub_8001FC58();
        quotient = (uint32_t)(((uint64_t)(random >> 1) * UINT64_C(0x72AC755D)) >> 41);
        value = r_u32(actor + 636u) - 1143u;
        remainder = random - quotient * 2286u;
        w_u32(actor + 636u, value + remainder);
    }
    random = sub_8001FC58();
    quotient = (uint32_t)(((uint64_t)(random >> 1) * UINT64_C(0x72AC755D)) >> 41);
    w_u32(actor + 700u, 0x20000u);
    flags = r_u32(actor + 564u);
    value = r_u32(actor + 616u) - 1143u;
    w_u32(actor + 576u, 0u);
    w_u32(actor + 564u, flags | 0x08000000u);
    value += random - quotient * 2286u;
    lower = value + 5719u;
    upper = 5719u - value;
    lower_mask = (uint32_t)((int32_t)lower >> 31);
    upper_mask = (uint32_t)((int32_t)upper >> 31);
    result = upper_mask & upper;
    w_u32(actor + 616u, value + (lower_mask & (0u - 5719u - value)) + result);
    return result;
}

uint32_t sub_800A3A84(uint32_t actor)
{
    FUNCTION_MARKER(0x800A3A84u, "RASHCDG.BIN");
    uint32_t point[3], displacement[3], point_address = 0, normal_address = 0;
    uint16_t normal[3];
    uint32_t status, source, axis, sum = 0, value;
    int point_local = 0, normal_local = 0, use_contact_normal;
    int64_t product;
    if (r_u32(actor + 388u) & 1u)
    {
        status = sub_800A7BF8(actor, 0u, point, normal, r_u32(actor + 536u));
        w_u32(actor + 536u, status);
        point_local = normal_local = 1;
    }
    else
    {
        status = 0u;
        w_u8(actor + 534u, r_u32(actor + 372u) ? r_u8(actor + 394u) : 1u);
        w_u32(actor + 536u, 0u);
    }
    if ((int32_t)status <= 0)
    {
        source = r_u32(actor + 340u);
        point_address = source + 20u;
        normal_address = source + 8u;
        point_local = normal_local = 0;
    }
    if ((int32_t)status < 0)
    {
        if (!r_u32(actor + 480u))
        {
            w_u32(actor + 480u, 117188u);
            (void)sub_8002EE50(117188u, (const uint16_t *)xport_guest_cptr(actor + 450u, 6), (uint32_t *)xport_guest_ptr(actor + 456u, 12));
        }
        use_contact_normal = 0;
    }
    else
        use_contact_normal = (normal_local ? (int16_t)normal[1] : r_s16(normal_address + 2u)) < -614;
    if (use_contact_normal)
    {
        for (axis = 0; axis < 3; ++axis)
        {
            value = normal_local ? normal[axis] : r_u16(normal_address + 2u * axis);
            w_u16(actor + 522u + 2u * axis, 0u - value);
        }
    }
    else
    {
        point_address = actor + 504u;
        point_local = 0;
    }
    for (axis = 0; axis < 3; ++axis)
    {
        value = point_local ? point[axis] : r_u32(point_address + 4u * axis);
        displacement[axis] = value - r_u32(actor + 184u + 4u * axis);
    }
    for (axis = 0; axis < 3; ++axis)
    {
        value = (uint32_t)(int32_t)r_s16(actor + 522u + 2u * axis) << 4;
        product = (int64_t)(int32_t)displacement[axis] * (int32_t)value;
        sum += (uint32_t)((uint64_t)product >> 16);
    }
    return (uint32_t)sub_8002EAD8(xport_guest_cptr(actor + 184u, 12), xport_guest_cptr(actor + 522u, 6), sum, xport_guest_ptr(actor + 504u, 12));
}

uint32_t sub_80014508(uint32_t a1)
{
    FUNCTION_MARKER(0x80014508u, "SLUS_010.53");
    return sub_800144B8(a1);
}

uint32_t sub_8008F404(uint32_t actor, int32_t delta)
{
    uint32_t flags, category, linked, result, elapsed, random, lateral, speed, value;
    int32_t nonnegative;
    uint16_t perpendicular[3];
    unsigned axis;
    FUNCTION_MARKER(0x8008F404u, "RASHCDG.BIN");
    if (r_u16(0x800541D6u + 8u * r_u16(actor + 544u)) != 5u)
    {
        if (r_u32(actor + 552u) & 2u)
        {
            (void)sub_800374D4(actor, 1u);
            (void)sub_8009246C(actor);
            w_u32(actor + 552u, (r_u32(actor + 552u) | 1u) & 0xFFFFFFFDu);
        }
        flags = r_u32(actor + 552u);
        if (flags & 0x40000000u)
            (void)sub_800976C4(actor, delta);
        else if (!(flags & 0x04000000u))
            (void)sub_80097BCC(actor, delta);
    }
    category = r_u16(actor + 172u);
    result = (category >> 5) == 1u && (int32_t)(category & 31u) < r_s32(r_u32(0x8005B2F8u) + 48u);
    if (!result)
        return result;
    linked = 0x800CD898u + 1132u * r_u16(r_u32(actor + 596u) + 172u);
    result = r_u32(linked + 548u) & 0x20000u;
    if (!result)
        return result;
    elapsed = r_u32(linked + 792u) + (uint32_t)delta;
    w_u32(linked + 792u, elapsed);
    result = 43u;
    if ((int32_t)elapsed <= 32768)
        return result;
    w_u32(linked + 792u, 0u);
    if (r_u16(actor + 544u) == 43u)
    {
        result = r_u32(linked + 548u) & 0xFFF1FFFFu;
        w_u32(linked + 548u, result);
        return result;
    }
    for (axis = 0; axis < 3; ++axis)
        xport_gte_write_control(2u * axis, (uint32_t)(int32_t)r_s16(actor + 522u + 2u * axis));
    for (axis = 0; axis < 3; ++axis)
        xport_gte_write_data(9u + axis, (uint32_t)(int32_t)r_s16(actor + 450u + 2u * axis));
    xport_gte_execute(0x178000Cu);
    for (axis = 0; axis < 3; ++axis)
        xport_store_le16((uint8_t *)perpendicular + 2u * axis, (uint16_t)xport_gte_read_data(9u + axis));
    (void)sub_8002E468(perpendicular);
    nonnegative = (r_u32(linked + 548u) >> 31) ^ 1u;
    if (sub_8001FC58() & 1u)
    {
        lateral = (sub_8001FC58() & 0xFFFFu) - 32768u;
        speed = (uint32_t)(r_s32(actor + 480u) / 3);
    }
    else
    {
        random = sub_8001FC58();
        lateral = ((0u - (uint32_t)nonnegative) & (random % 196608u)) + 196608u;
        if (sub_8001FC58() & 1u)
            lateral = 0u - lateral;
        speed = r_u32(actor + 480u);
    }
    if ((int32_t)speed < 786432)
        speed = 786432u;
    value = (uint32_t)sub_8001FC90((int32_t)r_s16(actor + 450u) * 16, (int32_t)speed);
    w_u32(linked + 184u, r_u32(actor + 184u) + value);
    value = (uint32_t)sub_8001FC90((int32_t)r_s16(actor + 454u) * 16, (int32_t)speed);
    w_u32(linked + 192u, r_u32(actor + 192u) + value);
    value = (uint32_t)sub_8001FC90((int32_t)(int16_t)xport_load_le16(perpendicular) * 16, (int32_t)lateral);
    w_u32(linked + 184u, r_u32(linked + 184u) + value);
    result = (uint32_t)sub_8001FC90((int32_t)(int16_t)xport_load_le16((const uint8_t *)perpendicular + 4u) * 16, (int32_t)lateral);
    value = r_u32(linked + 192u) + result;
    flags = (r_u32(linked + 548u) & 0x7FFDFFFFu) | 0xC0000u;
    w_u32(linked + 192u, value);
    w_u32(linked + 548u, flags);
    return result;
}

uint32_t sub_800976C4(uint32_t actor, int32_t delta)
{
    uint32_t table, category, normal_address = 0, wall, record;
    uint32_t x, z, larger, smaller, approximation, sign, dot, value, other;
    uint32_t damping, gravity, speed, magnitude, denominator, reciprocal;
    int32_t distance, facing;
    int nearby = 0, barrier = 0;
    uint16_t reversed[3];
    const uint16_t *normal = NULL;
    FUNCTION_MARKER(0x800976C4u, "RASHCDG.BIN");
    if (!(r_u32(actor + 552u) & 0x80000u))
    {
        category = r_u16(actor + 172u);
        if ((category >> 5) == 1u && (int32_t)(category & 31u) < r_s32(r_u32(0x8005B2F8u) + 48u))
        {
            table = 0x800D43C0u + 28u * r_u16(r_u32(actor + 596u) + 172u);
            if (r_s16(table + 22u) && r_u32(table) == r_u32(actor + 360u))
            {
                x = r_u32(table + 4u) - r_u32(actor + 184u);
                z = r_u32(table + 12u) - r_u32(actor + 192u);
                sign = (uint32_t)((int32_t)x >> 31);
                larger = (sign + (uint32_t)((int32_t)x >> 16)) ^ sign;
                sign = (uint32_t)((int32_t)z >> 31);
                smaller = (sign + (uint32_t)((int32_t)z >> 16)) ^ sign;
                if ((int32_t)larger < (int32_t)smaller)
                {
                    value = larger;
                    larger = smaller;
                    smaller = value;
                }
                value = smaller + (uint32_t)((int32_t)smaller >> 1);
                approximation = larger - (uint32_t)((int32_t)larger >> 5) - (uint32_t)((int32_t)larger >> 7);
                approximation += (uint32_t)((int32_t)value >> 2) + (uint32_t)((int32_t)value >> 6);
                if ((int32_t)approximation < 50)
                {
                    if ((int32_t)(uint32_t)sub_8002E698(xport_guest_cptr(actor + 450u, 6), xport_guest_cptr(table + 16u, 6)) <= 0)
                    {
                        x = r_u32(table + 4u) - r_u32(actor + 184u);
                        dot = (uint32_t)r_s16(table + 16u) * (uint32_t)((int32_t)x >> 16);
                        z = r_u32(table + 12u) - r_u32(actor + 192u);
                        dot += (uint32_t)r_s16(table + 20u) * (uint32_t)((int32_t)z >> 16);
                        nearby = (int32_t)dot > 0;
                    }
                    normal_address = table + 16u;
                }
            }
        }
        if ((r_u32(actor + 388u) & 1u) && r_u32(actor + 372u) && !(r_u32(actor + 36u) & 0x200u))
        {
            wall = r_u32(actor + 340u);
            distance = r_s32(actor + 344u);
            value = r_u8(wall + 49u);
            if ((distance > 0 && r_u8(wall + 48u)) || (distance <= 0 && value))
            {
                facing = (int32_t)(uint32_t)sub_8002E698(xport_guest_cptr(actor + 450u, 6), xport_guest_cptr(r_u32(actor + 340u) + 2u, 6));
                wall = r_u32(actor + 340u);
                barrier = ((int32_t)(r_u8(wall + 48u) << 15) < distance && facing > 0) || (distance < -(int32_t)(r_u8(wall + 49u) << 15) && facing < 0);
                if (barrier)
                {
                    normal_address = r_u32(actor + 340u) + 2u;
                    if (facing > 0)
                    {
                        xport_store_le16(reversed, (uint16_t)(0u - r_u16(normal_address)));
                        xport_store_le16((uint8_t *)reversed + 2u, (uint16_t)(0u - r_u16(normal_address + 2u)));
                        xport_store_le16((uint8_t *)reversed + 4u, (uint16_t)(0u - r_u16(normal_address + 4u)));
                        normal = reversed;
                    }
                }
            }
            else
            {
                record = sub_8003E338(actor);
                if (record)
                {
                    if ((int32_t)(uint32_t)sub_8002E698(xport_guest_cptr(actor + 450u, 6), xport_guest_cptr(record + 16u, 6)) <= 0)
                    {
                        x = r_u32(record + 4u) - r_u32(actor + 184u);
                        dot = (uint32_t)r_s16(record + 16u) * (uint32_t)((int32_t)x >> 16);
                        z = r_u32(record + 12u) - r_u32(actor + 192u);
                        dot += (uint32_t)r_s16(record + 20u) * (uint32_t)((int32_t)z >> 16);
                        barrier = (int32_t)dot > 0;
                    }
                    if (barrier)
                        normal_address = record + 16u;
                }
            }
        }
        if (barrier || nearby)
        {
            if (!normal)
                normal = xport_guest_cptr(normal_address, 6);
            if (sub_800A966C(actor, normal, 0x80000) == 2u)
                (void)sub_800C4550(43u, actor, 3u);
        }
    }
    damping = (uint32_t)sub_8001FC90(r_s32(actor + 580u), delta);
    value = (uint32_t)sub_8001FC90((int32_t)damping, r_s32(actor + 456u));
    x = r_u32(actor + 456u);
    other = r_u32(actor + 460u);
    w_u32(actor + 456u, x - value);
    value = (uint32_t)sub_8001FC90((int32_t)damping, (int32_t)other);
    gravity = (uint32_t)sub_8001FC90(643207, delta);
    x = r_u32(actor + 460u);
    other = r_u32(actor + 464u);
    w_u32(actor + 460u, x - (value - gravity));
    value = (uint32_t)sub_8001FC90((int32_t)damping, (int32_t)other);
    w_u32(actor + 464u, r_u32(actor + 464u) - value);
    speed = sub_8002E548(xport_guest_cptr(actor + 456u, 12));
    w_u32(actor + 480u, speed);
    if (speed)
    {
        magnitude = (int32_t)speed < 0 ? 0u - speed : speed;
        denominator = (uint32_t)((int32_t)magnitude >> 1) + (uint32_t)((int32_t)(magnitude - 2u) >> 31);
        reciprocal = denominator ? 0x80000000u / denominator : 0xFFFFFFFFu;
        if ((int32_t)speed < 0)
            reciprocal = 0u - reciprocal;
        (void)sub_8002EED8(reciprocal, xport_guest_cptr(actor + 456u, 12), xport_guest_ptr(actor + 450u, 6));
    }
    (void)sub_8007EA64(actor);
    category = r_u16(actor + 172u);
    if ((category >> 5) == 1u && (int32_t)(category & 31u) < r_s32(r_u32(0x8005B2F8u) + 48u) && !(r_u32(actor + 388u) & 1u) && sub_80038550(actor, r_u32(actor + 348u), r_u32(actor + 344u), actor + 328u, actor + 522u) == 1u)
    {
        value = r_u16(actor + 522u);
        other = r_u16(actor + 526u);
        w_u16(actor + 522u, (uint16_t)(0u - value));
        value = r_u16(actor + 524u);
        w_u16(actor + 526u, (uint16_t)(0u - other));
        w_u16(actor + 524u, (uint16_t)(0u - value));
    }
    value = r_u32(actor + 552u) & 0x7FF9FFFFu;
    w_u32(actor + 552u, value);
    return value;
}

uint32_t sub_8007EA64(uint32_t actor)
{
    uint32_t result;
    unsigned axis;
    FUNCTION_MARKER(0x8007EA64u, "RASHCDG.BIN");
    for (axis = 0; axis < 3; ++axis)
        xport_gte_write_control(2u * axis, (uint32_t)(int32_t)r_s16(actor + 522u + 2u * axis));
    for (axis = 0; axis < 3; ++axis)
        xport_gte_write_data(9u + axis, (uint32_t)(int32_t)r_s16(actor + 444u + 2u * axis));
    xport_gte_execute(0x178000Cu);
    for (axis = 0; axis < 3; ++axis)
        w_u16(actor + 516u + 2u * axis, (uint16_t)xport_gte_read_data(9u + axis));
    result = sub_8002E468(xport_guest_ptr(actor + 516u, 6));
    if (result)
    {
        for (axis = 0; axis < 3; ++axis)
            xport_gte_write_control(2u * axis, (uint32_t)(int32_t)r_s16(actor + 516u + 2u * axis));
        for (axis = 0; axis < 3; ++axis)
            xport_gte_write_data(9u + axis, (uint32_t)(int32_t)r_s16(actor + 522u + 2u * axis));
        xport_gte_execute(0x178000Cu);
        result = actor + 528u;
        for (axis = 0; axis < 3; ++axis)
            w_u16(actor + 528u + 2u * axis, (uint16_t)xport_gte_read_data(9u + axis));
    }
    else
    {
        for (axis = 0; axis < 3; ++axis)
            xport_gte_write_control(2u * axis, (uint32_t)(int32_t)r_s16(actor + 432u + 2u * axis));
        for (axis = 0; axis < 3; ++axis)
            xport_gte_write_data(9u + axis, (uint32_t)(int32_t)r_s16(actor + 522u + 2u * axis));
        xport_gte_execute(0x178000Cu);
        for (axis = 0; axis < 3; ++axis)
            w_u16(actor + 528u + 2u * axis, (uint16_t)xport_gte_read_data(9u + axis));
        result = sub_8002E468(xport_guest_ptr(actor + 528u, 6));
        if (!result)
            result = sub_8008CF74(actor);
        for (axis = 0; axis < 3; ++axis)
            xport_gte_write_control(2u * axis, (uint32_t)(int32_t)r_s16(actor + 522u + 2u * axis));
        for (axis = 0; axis < 3; ++axis)
            xport_gte_write_data(9u + axis, (uint32_t)(int32_t)r_s16(actor + 528u + 2u * axis));
        xport_gte_execute(0x178000Cu);
        for (axis = 0; axis < 3; ++axis)
            w_u16(actor + 516u + 2u * axis, (uint16_t)xport_gte_read_data(9u + axis));
    }
    return result;
}

uint32_t sub_800714A0(uint16_t output[9], const uint16_t quaternion[4])
{
    int32_t values[4];
    unsigned component;
    FUNCTION_MARKER(0x800714A0, "RASHCDG.BIN");
    for (component = 0; component < 4; ++component)
        values[component] = (int16_t)xport_load_le16(quaternion + component) * 4;
    return sub_8001005C(output, values);
}

uint32_t sub_8007198C(const uint16_t quaternion[4], uint16_t output[9])
{
    uint16_t matrix[9];
    uint32_t value = 0;
    unsigned element;
    FUNCTION_MARKER(0x8007198C, "RASHCDG.BIN");
    (void)sub_800714A0(matrix, quaternion);
    for (element = 0; element < 9; ++element)
    {
        value = xport_load_le16(matrix + (element % 3) * 3 + element / 3);
        xport_store_le16(output + element, (uint16_t)value);
    }
    return value;
}

uint32_t sub_80071A28(uint16_t quaternion[4])
{
    uint32_t values[4], squares[4], sum, magnitude, second, third, denominator, reciprocal, value = 0;
    unsigned component;
    int negative;
    FUNCTION_MARKER(0x80071A28, "RASHCDG.BIN");
    for (component = 0; component < 4; ++component)
        values[component] = (uint32_t)((int16_t)xport_load_le16(quaternion + component) * 4);
    for (component = 0; component < 4; ++component)
        squares[component] = (uint32_t)sub_8001FC90((int32_t)values[component], (int32_t)values[component]);
    sum = squares[0] + squares[1] + squares[2] + squares[3];
    magnitude = sub_8004CF74(sum) << 2;
    negative = (int32_t)magnitude < 0;
    second = sub_8004CF74(sum) << 2;
    third = sub_8004CF74(sum) << 2;
    if (negative)
    {
        second = 0 - second;
        third = 0 - third;
    }
    denominator = (uint32_t)((int32_t)second >> 1) + (uint32_t)((int32_t)(third - 2) >> 31);
    reciprocal = denominator ? 0x80000000 / denominator : 0xFFFFFFFF;
    if (negative)
        reciprocal = 0 - reciprocal;
    for (component = 0; component < 4; ++component)
    {
        value = (uint32_t)sub_8001FC90((int32_t)values[component], (int32_t)reciprocal);
        value = (uint32_t)((int32_t)value >> 2);
        xport_store_le16(quaternion + component, (uint16_t)value);
    }
    return value;
}

uint32_t sub_800A40D4(uint32_t actor)
{
    uint32_t first, second, third, normal_x, normal_y, normal_z, other_x, other_y, other_z, mask, type;
    FUNCTION_MARKER(0x800A40D4, "RASHCDG.BIN");
    if (!(r_u32(actor + 592) & 4))
        return sub_8003FA18(9, xport_guest_cptr(actor + 516, 18), xport_guest_ptr(actor + 432, 18));
    first = r_u16(actor + 516);
    second = r_u16(actor + 518);
    third = r_u16(actor + 520);
    mask = 0 - ((uint32_t)(int32_t)r_s16(actor + 446) >> 31);
    w_u16(actor + 432, first);
    normal_x = r_u16(actor + 522);
    w_u16(actor + 434, second);
    normal_y = r_u16(actor + 524);
    w_u16(actor + 436, third);
    normal_z = r_u16(actor + 526);
    w_u16(actor + 444, normal_x + (mask & (0 - normal_x - normal_x)));
    other_x = r_u16(actor + 528);
    w_u16(actor + 446, normal_y + (mask & (0 - normal_y - normal_y)));
    other_y = r_u16(actor + 530);
    w_u16(actor + 438, (mask & (2 * other_x)) - other_x);
    other_z = r_u16(actor + 532);
    w_u16(actor + 440, (mask & (2 * other_y)) - other_y);
    type = r_u32(actor + 180);
    w_u16(actor + 448, normal_z + (mask & (0 - normal_z - normal_z)));
    w_u16(actor + 442, (mask & (2 * other_z)) - other_z);
    if (type == 2)
        return sub_8002ED94(xport_guest_ptr(actor + 444, 6), xport_guest_ptr(actor + 438, 6), (mask & 454) - 227);
    return 2;
}

uint32_t sub_8002E388(uint32_t vector[3])
{
    uint32_t x, y, z, first, second, third, result, leading, table, index, entry, scale, product;
    int32_t shift;
    FUNCTION_MARKER(0x8002E388, "SLUS_010.53");
    x = xport_load_le32(vector);
    y = xport_load_le32(vector + 1);
    z = xport_load_le32(vector + 2);
    xport_gte_write_data(9, x);
    xport_gte_write_data(10, y);
    xport_gte_write_data(11, z);
    xport_gte_execute(0xA00428);
    first = xport_gte_read_data(25);
    second = xport_gte_read_data(26);
    third = xport_gte_read_data(27);
    result = first + second + third;
    xport_gte_write_data(30, result);
    leading = xport_gte_read_data(31);
    table = r_u32(0x8005B560);
    shift = (int32_t)(22 - (leading & 0xFFFFFFFE));
    if (shift <= 0)
        shift = 0;
    index = (uint32_t)((int32_t)result >> ((uint32_t)shift & 31));
    entry = r_u16(table + (index << 1));
    scale = (entry >> 5) << (entry & 31);
    scale = (uint32_t)((int32_t)scale >> (((uint32_t)shift >> 1) & 31));
    product = x * scale;
    xport_store_le32(vector, (uint32_t)((int32_t)product >> 12));
    product = y * scale;
    xport_store_le32(vector + 1, (uint32_t)((int32_t)product >> 12));
    product = z * scale;
    xport_store_le32(vector + 2, (uint32_t)((int32_t)product >> 12));
    return result;
}

uint32_t sub_800A41EC(void)
{
    FUNCTION_MARKER(0x800A41EC, "RASHCDG.BIN");
    w_u8(0x800CD0A8, 0x80);
    w_u8(0x800CD0A9, 0xFF);
    w_u32(0x800CCF78, 1);
    w_u32(0x800CCF7C, 0);
    return 0x800CCF78;
}
