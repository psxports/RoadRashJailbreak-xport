#include "race_leaf_batch_005.h"
#include "fixed_math.h"
#include "race_audio_frontier.h"
#include "race_leaf_800AF3B0.h"
#include "race_pause.h"
#include "xport.h"

static int32_t batch5_floor_shift16(int64_t value)
{
    return (int32_t)(value / 65536 - (value < 0 && value % 65536 != 0));
}

static int32_t batch5_projection(uint32_t point, uint32_t origin, uint32_t axis)
{
    int32_t result = 0;
    uint32_t i;

    for (i = 0; i < 3; ++i)
    {
        int32_t delta = ((int32_t)(uint32_t)(r_u32(point + 4u * i) - r_u32(origin + 4u * i)));
        int32_t component = (int16_t)r_u16(axis + 2u * i) * 16;

        result = ((int32_t)(uint32_t)((uint32_t)result + (uint32_t)batch5_floor_shift16((int64_t)delta * component)));
    }
    return result;
}

uint32_t sub_800B5EB4(uint32_t first, uint32_t second, uint32_t direction, uint32_t plane, int32_t base_offset, uint32_t origin, uint32_t edge_first, uint32_t edge_second, uint32_t edge_third, uint32_t first_axis, uint32_t second_axis, int32_t first_limit, int32_t second_limit, uint32_t side_out, uint32_t position_out)
{
    uint32_t scalar_magnitude_8;
    uint32_t scalar_result_8;
    uint32_t scalar_magnitude_9;
    uint32_t scalar_result_9;

    int32_t first_on_first = batch5_projection(origin, first, first_axis);
    int32_t first_on_second = batch5_projection(origin, first, second_axis);
    int32_t second_on_first = batch5_projection(origin, second, first_axis);
    int32_t second_on_second = batch5_projection(origin, second, second_axis);
    uint32_t first_first_region = ((uint32_t)(((uint32_t)((int32_t)(first_on_first)) >> 31) + (((int32_t)(first_limit)) < ((int32_t)(first_on_first)) ? 2u : 0u)));
    uint32_t first_second_region = ((uint32_t)(((uint32_t)((int32_t)(first_on_second)) >> 31) + (((int32_t)(second_limit)) < ((int32_t)(first_on_second)) ? 2u : 0u)));
    uint32_t second_first_region = ((uint32_t)(((uint32_t)((int32_t)(second_on_first)) >> 31) + (((int32_t)(first_limit)) < ((int32_t)(second_on_first)) ? 2u : 0u)));
    uint32_t second_second_region = ((uint32_t)(((uint32_t)((int32_t)(second_on_second)) >> 31) + (((int32_t)(second_limit)) < ((int32_t)(second_on_second)) ? 2u : 0u)));
    int32_t first_dot;
    int32_t second_dot;
    int32_t first_near;
    int32_t first_far;
    int32_t second_near;
    int32_t second_far;
    int32_t near_value;
    int32_t far_value;

    FUNCTION_MARKER(0x800B5EB4, "RASHCDG.BIN");
    w_u32(side_out, 0);
    w_u32(position_out, 0);
    if ((!first_first_region && !first_second_region) || (!second_first_region && !second_second_region))
        goto intersect;
    if (!first_first_region)
    {
        if (first_second_region == second_second_region)
            return 0xFFFFFFFFu;
        if (first_second_region)
            goto region_crossed;
        goto compare_first_regions;
    }
    if (first_second_region)
    {
        if (first_first_region == second_first_region)
            return 0xFFFFFFFFu;
        if (first_second_region == second_second_region)
            return 0xFFFFFFFFu;
        goto region_crossed;
    }

compare_first_regions:
    if (first_first_region == second_first_region)
        return 0xFFFFFFFFu;

region_crossed:
    if ((!first_first_region && !second_first_region) || (!first_second_region && !second_second_region))
        goto intersect;

    {
        int32_t sign = ((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(origin, 12), xport_guest_ptr(plane, 6), xport_guest_ptr(first, 12))));

        if (((int32_t)(uint32_t)((uint32_t)sign ^ sub_800B6AAC(xport_guest_ptr(edge_first, 12), xport_guest_ptr(plane, 6), xport_guest_ptr(first, 12)))) < 0)
            goto intersect;
        if (((int32_t)(uint32_t)((uint32_t)sign ^ sub_800B6AAC(xport_guest_ptr(edge_second, 12), xport_guest_ptr(plane, 6), xport_guest_ptr(first, 12)))) < 0)
            goto intersect;
        if (((int32_t)(uint32_t)((uint32_t)sign ^ sub_800B6AAC(xport_guest_ptr(edge_third, 12), xport_guest_ptr(plane, 6), xport_guest_ptr(first, 12)))) < 0)
            goto intersect;
        return 0xFFFFFFFFu;
    }

intersect:
    first_dot = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(direction, 6), xport_guest_ptr(first_axis, 6))));
    second_dot = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(direction, 6), xport_guest_ptr(second_axis, 6))));
    if (((int32_t)(((int32_t)(first_dot)) < 0 ? ((int32_t)(uint32_t)(0u - (uint32_t)((int32_t)(first_dot)))) : ((int32_t)(first_dot)))) >= 655)
    {
        if (((int32_t)(((int32_t)(second_dot)) < 0 ? ((int32_t)(uint32_t)(0u - (uint32_t)((int32_t)(second_dot)))) : ((int32_t)(second_dot)))) >= 655)
        {
            int32_t first_reciprocal = ((scalar_magnitude_8 = ((int32_t)(first_dot)) < 0 ? 0u - (uint32_t)((int32_t)(first_dot)) : (uint32_t)((int32_t)(first_dot))), (scalar_result_8 = 0x80000000u / (scalar_magnitude_8 >> 1)), (((int32_t)(((int32_t)(first_dot)) < 0 ? ((int32_t)(uint32_t)(0u - scalar_result_8)) : ((int32_t)(uint32_t)(scalar_result_8))))));
            int32_t second_reciprocal = ((scalar_magnitude_9 = ((int32_t)(second_dot)) < 0 ? 0u - (uint32_t)((int32_t)(second_dot)) : (uint32_t)((int32_t)(second_dot))), (scalar_result_9 = 0x80000000u / (scalar_magnitude_9 >> 1)), (((int32_t)(((int32_t)(second_dot)) < 0 ? ((int32_t)(uint32_t)(0u - scalar_result_9)) : ((int32_t)(uint32_t)(scalar_result_9))))));

            if (first_reciprocal <= 0)
            {
                first_near = (int32_t)sub_8001FC90(((int32_t)(uint32_t)((uint32_t)first_on_first - (uint32_t)first_limit)), first_reciprocal);
                first_far = ((int32_t)(uint32_t)(0u - (uint32_t)(int32_t)sub_8001FC90(second_on_first, first_reciprocal)));
            }
            else
            {
                first_near = (int32_t)sub_8001FC90(first_on_first, first_reciprocal);
                first_far = (int32_t)sub_8001FC90(((int32_t)(uint32_t)((uint32_t)first_limit - (uint32_t)second_on_first)), first_reciprocal);
            }
            if (second_reciprocal <= 0)
            {
                second_near = (int32_t)sub_8001FC90(((int32_t)(uint32_t)((uint32_t)first_on_second - (uint32_t)second_limit)), second_reciprocal);
                second_far = ((int32_t)(uint32_t)(0u - (uint32_t)(int32_t)sub_8001FC90(second_on_second, second_reciprocal)));
            }
            else
            {
                second_near = (int32_t)sub_8001FC90(first_on_second, second_reciprocal);
                second_far = (int32_t)sub_8001FC90(((int32_t)(uint32_t)((uint32_t)second_limit - (uint32_t)second_on_second)), second_reciprocal);
            }
        }
        else
        {
            second_near = first_limit - first_on_first;
            if (first_dot > 0)
                second_near = first_on_first;
            second_far = second_on_first;
            if (first_dot > 0)
                second_far = first_limit - second_on_first;
            first_near = second_near;
            first_far = second_far;
        }
    }
    else
    {
        first_near = second_limit - first_on_second;
        if (second_dot > 0)
            first_near = first_on_second;
        first_far = second_on_second;
        if (second_dot > 0)
            first_far = second_limit - second_on_second;
        second_near = first_near;
        second_far = first_far;
    }

    near_value = first_near < second_near ? first_near : second_near;
    far_value = first_far < second_far ? first_far : second_far;
    w_u32(side_out, far_value < near_value);
    w_u32(position_out, (uint32_t)base_offset + (uint32_t)(near_value < far_value ? near_value : far_value));
    return 1;
}

uint32_t sub_800B12A0(uint32_t actor, uint16_t normal[3], uint32_t contact_flags, int32_t surface_speed, uint32_t surface, uint32_t translation[3])
{
    int high_speed = 0;
    uint32_t option_base;
    uint32_t options;
    int32_t dot;
    int32_t mode;
    int32_t original_surface_speed;
    int32_t response;
    uint32_t response_mode;
    uint32_t result;

    FUNCTION_MARKER(0x800B12A0, "RASHCDG.BIN");
    if (r_u32(actor + 832) == actor + 172 && !(r_u32(actor + 564) & 0x8000u))
        return 0;
    if (!(r_u32(actor + 560) & 0x08000000u) && !(contact_flags & 0x200u) && (r_u32(actor + 388) & 0x10u))
        high_speed = ((int32_t)(uint32_t)(r_u32(0x800D3970u))) < ((int32_t)(uint32_t)(r_u32(actor + 480)));

    if (high_speed && (r_u32(actor + 568) & 0x400u))
    {
        uint32_t cached = actor + 864;
        int32_t angle;
        int16_t cosine;

        if (!(r_u32(actor + 568) & 0x02000000u))
        {
            uint32_t i;

            for (i = 0; i < 3; ++i)
                w_u16(cached + 2u * i, r_u16(actor + 450u + 2u * i));
            w_u32(actor + 860, r_u32(actor + 480));
            w_u32(actor + 568, r_u32(actor + 568) | 0x02000000u);
        }
        angle = ((int32_t)(uint32_t)(sub_8001FF3C((uint32_t)(int32_t)r_s16(cached + 2) << 4)));
        cosine = (int16_t)r_u16(0x8005624Eu + 4u * (((uint32_t)1365 - (uint32_t)angle) & 4095u));
        (void)sub_8007E868(cached, ((int32_t)(uint32_t)(r_u32(actor + 576))), cosine, 435486);
        return (uint32_t)sub_8002EE50(r_u32(actor + 860), xport_guest_ptr(cached, 6), xport_guest_ptr(actor + 456, 12));
    }

    option_base = high_speed ? 8u : 0u;
    options = option_base | ((contact_flags & 0x400u) ? 1u : 5u);
    dot = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(actor + 450, 6), normal)));
    mode = 3;
    if (dot >= -56754)
    {
        int32_t actor_x = (int16_t)r_u16(actor + 450);
        int32_t actor_z = (int16_t)r_u16(actor + 454);
        int32_t normal_x = (int16_t)xport_load_le16(normal);
        int32_t normal_z = (int16_t)xport_load_le16(normal + 2);

        mode = 2 * (actor_x * normal_z >= actor_z * normal_x);
    }
    original_surface_speed = ((int32_t)(uint32_t)(r_u32(surface + 4)));
    w_u32(surface + 4, surface_speed < 0 ? 6553600u : (uint32_t)surface_speed);
    result = sub_800AF3B0(actor, surface, normal, mode, options, translation);
    if (!result)
    {
        w_u32(surface + 4, (uint32_t)original_surface_speed);
        return (uint32_t)(dot >> 31);
    }

    response = (int32_t)sub_8001FC90(((int32_t)(((int32_t)(dot)) < 0 ? ((int32_t)(uint32_t)(0u - (uint32_t)((int32_t)(dot)))) : ((int32_t)(dot)))), ((int32_t)(uint32_t)(r_u32(actor + 480))));
    if ((contact_flags & 0xFu) == 8u)
    {
        response_mode = 0;
    }
    else
    {
        int32_t selector = ((contact_flags & 0xFu) == 2u || (contact_flags & 0x400u)) ? -1 : 0;

        response_mode = (uint32_t)((selector & ~1) + 6) << 16;
    }
    response = ((int32_t)(uint32_t)(sub_800A9408(actor, response, (int32_t)response_mode, 0x10000, mode)));
    if (((int32_t)(uint32_t)(r_u32(actor + 480))) > 65918)
    {
        uint32_t sound = ((5u * (sub_80043F00(0xF2000002u) & 0xFFu)) >> 8) + 50u;
        uint32_t kind = (contact_flags >> 4) & 0xFu;
        int quiet = response < 2;

        if (kind == 1)
            sound = 10;
        else if (kind == 2)
            sound = 11;
        else if (kind == 3)
            sound = 12;
        else if (kind == 4)
            sound = 13;
        (void)sub_80017BA0(r_u32(actor + 184), r_u32(actor + 192), sound, 0);
        if (r_u16(actor + 172) < r_u32(r_u32(0x8005B2F8u) + 48) && r_u32(r_u32(actor + 852) + 604) < 2u && !r_u32(0x8005B220u))
        {
            (void)sub_800B658C(actor, 0, ((int32_t)(uint32_t)(r_u32(actor + 480))), 1464860, 1);
        }
        if (!quiet)
            (void)sub_80017BA0(r_u32(actor + 184), r_u32(actor + 192), 49, 0);
    }

    result = 2;
    if (mode == 2)
        result = sub_80027258(actor, 3);
    else if (!mode)
        result = sub_80027258(actor, 4);
    w_u32(surface + 4, (uint32_t)original_surface_speed);
    return result;
}
