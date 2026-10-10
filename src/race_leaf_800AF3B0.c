#include "race_leaf_800AF3B0.h"
#include "fixed_math.h"
#include "race_leaf.h"
#include "race_leaf_batch_004.h"
#include "race_leaf_batch_005.h"
#include "race_pause.h"
#include "xport.h"

typedef struct AFShortVector
{
    uint32_t address;
    const int16_t *local;
} AFShortVector;

static int32_t af_floor_shift(int64_t value, uint32_t shift)
{
    int64_t divisor = INT64_C(1) << shift;

    return (int32_t)(value / divisor - (value < 0 && value % divisor != 0));
}

static int32_t af_abs(int32_t value)
{
    return value < 0 ? rrj_s32(0u - (uint32_t)value) : value;
}

static int16_t af_short(RRJMemory *m, AFShortVector vector, uint32_t index)
{
    if (vector.local)
        return vector.local[index];
    return (int16_t)rrj_u16(rrj_at(vector.address + 2u * index, 2));
}

static int32_t af_project_guest_from_local(RRJMemory *m, uint32_t point, const int32_t origin[3], AFShortVector axis)
{
    int32_t result = 0;
    uint32_t i;

    for (i = 0; i < 3; ++i)
    {
        int32_t delta = rrj_s32(rrj_read32(point + 4u * i) - (uint32_t)origin[i]);
        int32_t component = af_short(m, axis, i) * 16;

        result = rrj_s32((uint32_t)result + (uint32_t)af_floor_shift((int64_t)delta * component, 16));
    }
    return result;
}

static int32_t af_plane_guest_from_local(RRJMemory *m, uint32_t point, uint32_t normal, const int32_t origin[3])
{
    int32_t result = 0;
    uint32_t i;

    for (i = 0; i < 3; ++i)
    {
        int32_t delta = rrj_s32(rrj_read32(point + 4u * i) - (uint32_t)origin[i]);
        int32_t component = (int16_t)rrj_u16(rrj_at(normal + 2u * i, 2)) * 16;

        result = rrj_s32((uint32_t)result + (uint32_t)af_floor_shift((int64_t)delta * component, 16));
    }
    return result;
}

static uint32_t af_region(int32_t projection, int32_t limit)
{
    return ((uint32_t)projection >> 31) + (limit < projection ? 2u : 0u);
}

static int32_t af_reciprocal(int32_t value)
{
    uint32_t magnitude = value < 0 ? 0u - (uint32_t)value : (uint32_t)value;
    uint32_t result = 0x80000000u / (magnitude >> 1);

    return value < 0 ? rrj_s32(0u - result) : rrj_s32(result);
}

static uint32_t af_sweep(RRJMemory *m, const int32_t first[3], const int32_t second[3], uint32_t direction, uint32_t plane, int32_t base_offset, uint32_t center, uint32_t edge_first, uint32_t edge_second, uint32_t edge_third, AFShortVector first_axis, AFShortVector second_axis, int32_t first_limit, int32_t second_limit, int32_t *side_out, int32_t *position_out)
{
    int32_t first_on_first = af_project_guest_from_local(m, center, first, first_axis);
    int32_t first_on_second = af_project_guest_from_local(m, center, first, second_axis);
    int32_t second_on_first = af_project_guest_from_local(m, center, second, first_axis);
    int32_t second_on_second = af_project_guest_from_local(m, center, second, second_axis);
    uint32_t first_first_region = af_region(first_on_first, first_limit);
    uint32_t first_second_region = af_region(first_on_second, second_limit);
    uint32_t second_first_region = af_region(second_on_first, first_limit);
    uint32_t second_second_region = af_region(second_on_second, second_limit);
    AFShortVector direction_vector = {direction, NULL};
    int32_t first_dot;
    int32_t second_dot;
    int32_t first_near;
    int32_t first_far;
    int32_t second_near;
    int32_t second_far;
    int32_t near_value;
    int32_t far_value;
    int32_t sign;

    *side_out = 0;
    *position_out = 0;
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
        if (first_first_region == second_first_region || first_second_region == second_second_region)
            return 0xFFFFFFFFu;
        goto region_crossed;
    }

compare_first_regions:
    if (first_first_region == second_first_region)
        return 0xFFFFFFFFu;

region_crossed:
    if ((!first_first_region && !second_first_region) || (!first_second_region && !second_second_region))
        goto intersect;
    sign = af_plane_guest_from_local(m, center, plane, first);
    if (rrj_s32((uint32_t)sign ^ (uint32_t)af_plane_guest_from_local(m, edge_first, plane, first)) < 0 || rrj_s32((uint32_t)sign ^ (uint32_t)af_plane_guest_from_local(m, edge_second, plane, first)) < 0 || rrj_s32((uint32_t)sign ^ (uint32_t)af_plane_guest_from_local(m, edge_third, plane, first)) < 0)
        goto intersect;
    return 0xFFFFFFFFu;

intersect:
    first_dot = 0;
    second_dot = 0;
    {
        uint32_t i;

        for (i = 0; i < 3; ++i)
        {
            int32_t direction_component = af_short(m, direction_vector, i);

            first_dot += direction_component * af_short(m, first_axis, i);
            second_dot += direction_component * af_short(m, second_axis, i);
        }
        first_dot = af_floor_shift(first_dot, 8);
        second_dot = af_floor_shift(second_dot, 8);
    }
    if (af_abs(first_dot) >= 655)
    {
        if (af_abs(second_dot) >= 655)
        {
            int32_t first_reciprocal = af_reciprocal(first_dot);
            int32_t second_reciprocal = af_reciprocal(second_dot);

            if (first_reciprocal <= 0)
            {
                first_near = (int32_t)sub_8001FC90(rrj_s32((uint32_t)first_on_first - (uint32_t)first_limit), first_reciprocal);
                first_far = rrj_s32(0u - (uint32_t)(int32_t)sub_8001FC90(second_on_first, first_reciprocal));
            }
            else
            {
                first_near = (int32_t)sub_8001FC90(first_on_first, first_reciprocal);
                first_far = (int32_t)sub_8001FC90(rrj_s32((uint32_t)first_limit - (uint32_t)second_on_first), first_reciprocal);
            }
            if (second_reciprocal <= 0)
            {
                second_near = (int32_t)sub_8001FC90(rrj_s32((uint32_t)first_on_second - (uint32_t)second_limit), second_reciprocal);
                second_far = rrj_s32(0u - (uint32_t)(int32_t)sub_8001FC90(second_on_second, second_reciprocal));
            }
            else
            {
                second_near = (int32_t)sub_8001FC90(first_on_second, second_reciprocal);
                second_far = (int32_t)sub_8001FC90(rrj_s32((uint32_t)second_limit - (uint32_t)second_on_second), second_reciprocal);
            }
        }
        else
        {
            second_near = first_dot > 0 ? first_on_first : first_limit - first_on_first;
            second_far = first_dot > 0 ? first_limit - second_on_first : second_on_first;
            first_near = second_near;
            first_far = second_far;
        }
    }
    else
    {
        first_near = second_dot > 0 ? first_on_second : second_limit - first_on_second;
        first_far = second_dot > 0 ? second_limit - second_on_second : second_on_second;
        second_near = first_near;
        second_far = first_far;
    }
    near_value = first_near < second_near ? first_near : second_near;
    far_value = first_far < second_far ? first_far : second_far;
    *side_out = far_value < near_value;
    *position_out = rrj_s32((uint32_t)base_offset + (uint32_t)(near_value < far_value ? near_value : far_value));
    return 1;
}

static void af_scale_short_local(RRJMemory *m, uint32_t scale, uint32_t direction, int32_t output[3])
{
    uint32_t values[3];
    uint32_t i;

    (void)sub_8002EE50(scale, rrj_at(direction, 6), values);
    for (i = 0; i < 3; ++i)
        output[i] = rrj_s32(values[i]);
}

static uint32_t af_select_mode(uint32_t special, int32_t surface_flags, uint32_t embedded, uint32_t simple_type, int32_t collision_mode, uint32_t options)
{
    int special_bit = special && (surface_flags & 0x200);
    int v77 = 5 - special_bit;
    int v78 = collision_mode != 2;
    int v76 = (options & 2u) ? ((collision_mode & 5) == 0) : 0;
    int v80 = -v76;
    int v83 = (options & 4u) ? -special_bit + 3 + special_bit : 5 - special_bit;
    int v84 = simple_type || embedded;
    int v86 = -v84;
    int v88 = 5 - special_bit;
    int v89 = collision_mode != 2;
    int v87 = (options & 2u) ? ((collision_mode & 5) == 0) : 0;
    int v91 = -v87;
    int v93 = (options & 4u) ? -special_bit + 3 + special_bit : 5 - special_bit;
    int v96 = (options & 4u) ? v89 - (v88 - 2 + special_bit) : v89 - v88;
    int v97 = v93 + (v91 & v96);

    if (options & 4u)
        return (uint32_t)(v97 + (v86 & (2 - (v83 + (v80 & (v78 - (v77 - 2 + special_bit)))))));
    return (uint32_t)(v97 + (v86 & (2 - (v83 + (v80 & (v78 - v77))))));
}

uint32_t sub_800AF3B0(uint32_t actor, uint32_t surface, uint32_t normal, int32_t collision_mode, uint32_t options, uint32_t translation)
{
    int32_t speed = rrj_s32(rrj_read32(actor + 576));
    uint32_t type = rrj_u16(rrj_at(surface, 2)) >> 5;
    uint32_t special = type == 8;
    uint32_t embedded = 0;
    uint32_t linked = 0;
    uint32_t object = 0;
    int32_t surface_flags = 0;
    int32_t surface_value;
    int special_subtype = 0;
    int simple_type = type == 1 || type == 2;
    uint32_t actor_flags = rrj_read32(actor + 564);
    int sticky = (actor_flags & 0x8000u) != 0;
    int previous_surface = rrj_read32(actor + 828) == surface;
    int active_surface = rrj_read32(actor + 832) == surface;
    int sticky_active = sticky && active_surface;
    int has_response = (rrj_read32(actor + 568) & 0xEu) != 0;
    int low_speed;
    int moving_side = (rrj_read32(actor + 568) & 0x400u) != 0;
    int embedded_state = 0;
    int accept = 0;
    int apply_linked = 0;
    int relative_speed = 0;
    int high_speed;
    int collision = 0;
    int collision_side = 0;
    int collision_position = 0;
    int result;
    uint32_t collision_normal = normal;
    int16_t inverse_normal[3];

    FUNCTION_MARKER(0x800AF3B0, "RASHCDG.BIN");
    if (rrj_read32(actor + 568) & 0x600u)
        speed = rrj_s32(rrj_read32(actor + 480));
    if (special)
    {
        special_subtype = (((int16_t)rrj_u16(rrj_at(surface + 2, 2))) & 0xFu) == 1;
        surface_flags = (int16_t)rrj_u16(rrj_at(surface + 2, 2));
        surface_value = rrj_s32(rrj_read32(surface + 4));
    }
    else
    {
        uint32_t table = 0x800CE4D0u + 16u * type;

        object = rrj_read32(table) + rrj_read32(table + 4) * (rrj_u16(rrj_at(surface, 2)) & 31u);
        embedded = type == 4 ? object : 0;
        linked = type == 3 ? object : 0;
        surface_value = rrj_s32(rrj_read32(surface + 140));
    }
    low_speed = speed <= 65917 && !sticky_active && (!linked || !rrj_read32(linked + 480));
    if (low_speed)
    {
        if (!simple_type && !embedded && !(rrj_read32(actor + 560) & 0x300u))
            rrj_write32(actor + 576, 0);
        apply_linked = embedded != 0;
        goto post_contact;
    }

    if (embedded)
    {
        uint32_t kind = (rrj_u16(rrj_at(rrj_read32(embedded) + 14, 2)) & 0xF80u) >> 7;

        if ((uint32_t)(kind - 3) < 3u)
        {
            uint32_t state = rrj_read32(embedded + 592);

            if (!(state & 4u) && speed <= 732429)
            {
                if (state & 0x800u)
                {
                    sticky_active = 1;
                }
                else
                {
                    embedded_state = -1;
                    if (!(state & 0x200u))
                    {
                        (void)sub_800B3838(embedded, actor + 450, actor + 814, actor + 522, speed);
                        (void)sub_80017BA0(rrj_read32(surface + 12), rrj_read32(surface + 20), sub_80017B30(rrj_s32(rrj_read32(surface + 8))), 0);
                        embedded_state = 1;
                    }
                    rrj_write32(embedded + 592, state | 0x400u);
                }
            }
        }
    }
    accept = embedded_state > 0;
    if (embedded_state)
        goto embedded_done;

    if (!embedded)
    {
        if (sticky_active || sticky)
            goto acceptance_done;
        if (moving_side)
        {
            if ((type == 5 || type == 6) && !previous_surface)
                sticky_active = surface_value < rrj_s32(rrj_read32(0x800D396Cu));
            goto acceptance_done;
        }
        if (rrj_read32(actor + 568) & 0x7FFu)
            goto acceptance_done;
        if (type)
        {
            if (special)
            {
                if (options & 8u)
                    sticky_active = 1;
                else if (!special_subtype)
                    goto acceptance_done;
            }
            {
                int32_t acceleration = rrj_s32(rrj_read32(actor + 616));
                int32_t low = rrj_s32(rrj_read32(0x800D3968u));
                int32_t range = rrj_s32(rrj_read32(0x800D396Cu) - (uint32_t)low);

                if (acceleration < 0)
                    acceleration = 0;
                if (surface_value >= rrj_s32((uint32_t)low + (uint32_t)(int32_t)sub_8001FC90(acceleration, range)))
                    goto acceptance_done;
            }
        }
        sticky_active = 1;
    }
    else
    {
        int32_t alignment = rrj_s32(sub_8002E698(rrj_at(embedded + 438, 6), rrj_at(embedded + 522, 6)));
        uint32_t state = rrj_read32(embedded + 592);
        uint32_t kind = (rrj_u16(rrj_at(rrj_read32(embedded) + 14, 2)) & 0xF80u) >> 7;

        if (sticky_active || (!sticky && !(rrj_read32(actor + 568) & 0x7FFu) && !(state & 2u) && ((rrj_read32(embedded + 180) == 30 && af_abs(alignment) < 6553) || ((uint32_t)(kind - 3) < 3u && (state & 4u)))))
            sticky_active = 1;
        else
            sticky_active = 0;
    }

acceptance_done:
    high_speed = rrj_s32(rrj_read32(0x800D3970u)) < speed && !(rrj_read32(actor + 568) & 0x7FFu) && !(rrj_read32(actor + 564) & 0x4000u) && !active_surface;
    if (sticky_active)
        high_speed |= (rrj_read32(actor + 564) >> 17) & 1u;
    else if (linked)
    {
        int32_t alignment = rrj_s32(sub_8002E698(rrj_at(linked + 450, 6), rrj_at(actor + 450, 6)));

        relative_speed = rrj_s32((uint32_t)speed - (uint32_t)(int32_t)sub_8001FC90(rrj_s32(rrj_read32(linked + 480)), alignment));
        high_speed = 2 * rrj_s32(rrj_read32(0x800D3970u)) < relative_speed;
    }
    if (!sticky_active)
        goto no_candidate;
    if (special_subtype)
    {
        sticky = 1;
        goto candidate_done;
    }
    if (moving_side)
    {
        (void)sub_800B16F4(actor, actor + 576);
        high_speed = 0;
        rrj_write32(actor + 564, rrj_read32(actor + 564) & ~0x20000u);
    }

    {
        int32_t scaled[3];
        int32_t first[3];
        int32_t second[3];
        uint32_t i;

        af_scale_short_local(rrj_host_context(), rrj_read32(actor + 308), actor + 528, scaled);
        for (i = 0; i < 3; ++i)
        {
            int32_t center = rrj_s32(rrj_read32(actor + 504u + 4u * i));

            first[i] = rrj_s32((uint32_t)center + (uint32_t)scaled[i]);
            second[i] = rrj_s32((uint32_t)center - (uint32_t)scaled[i]);
        }
        if (type != 8)
        {
            uint32_t center;
            uint32_t edge_first;
            uint32_t edge_second;
            uint32_t edge_third;
            AFShortVector axis_first;
            AFShortVector axis_second;
            int16_t local_axis[3];
            int32_t first_limit = rrj_s32(rrj_read32(surface + 132) << 1);
            int32_t second_limit;

            axis_first.local = NULL;
            axis_second.local = NULL;
            if (type == 4 || type == 1 || type == 2)
            {
                int32_t near_limit = rrj_s32(rrj_read32(surface + 136));
                int32_t far_limit = rrj_s32(rrj_read32(surface + 140));

                second_limit = rrj_s32((uint32_t)(near_limit << 1) + ((type == 1 || type == 2) ? (uint32_t)far_limit - (uint32_t)(near_limit << 1) : 0u));
                axis_second.address = surface + 266;
                if ((int16_t)rrj_u16(rrj_at(surface + 274, 2)) >= 0)
                {
                    center = surface + 36;
                    edge_first = surface + 24;
                    edge_second = surface + 72;
                    edge_third = surface + 84;
                    axis_first.address = surface + 260;
                }
                else
                {
                    center = surface + 96;
                    edge_first = surface + 108;
                    edge_second = surface + 60;
                    edge_third = surface + 48;
                    local_axis[0] = (int16_t)(0u - rrj_u16(rrj_at(surface + 266, 2)));
                    local_axis[1] = (int16_t)(0u - rrj_u16(rrj_at(surface + 268, 2)));
                    local_axis[2] = (int16_t)(0u - rrj_u16(rrj_at(surface + 270, 2)));
                    axis_first.local = local_axis;
                    axis_first.address = 0;
                }
            }
            else if (!type)
            {
                second_limit = rrj_s32(rrj_read32(surface + 132));
                axis_second.address = surface + 272;
                if ((int16_t)rrj_u16(rrj_at(surface + 262, 2)) > 0)
                {
                    center = surface + 96;
                    edge_first = surface + 48;
                    edge_second = surface + 36;
                    edge_third = surface + 84;
                    local_axis[0] = (int16_t)(0u - rrj_u16(rrj_at(surface + 266, 2)));
                    local_axis[1] = (int16_t)(0u - rrj_u16(rrj_at(surface + 268, 2)));
                    local_axis[2] = (int16_t)(0u - rrj_u16(rrj_at(surface + 270, 2)));
                    axis_first.local = local_axis;
                    axis_first.address = 0;
                }
                else
                {
                    center = surface + 60;
                    edge_first = surface + 108;
                    edge_second = surface + 72;
                    edge_third = surface + 24;
                    axis_first.address = surface + 266;
                }
            }
            else
            {
                center = surface + 48;
                edge_first = surface + 60;
                edge_second = surface + 24;
                edge_third = surface + 36;
                axis_first.address = surface + 260;
                axis_second.address = surface + 272;
                second_limit = rrj_s32(rrj_read32(surface + 140));
            }
            collision = rrj_s32(af_sweep(rrj_host_context(), first, second, actor + 528, actor + 516, rrj_s32(rrj_read32(actor + 308) << 1), center, edge_first, edge_second, edge_third, axis_first, axis_second, first_limit, second_limit, &collision_side, &collision_position));
        }
        else
        {
            int32_t first_projection = 0;
            int32_t second_projection = 0;

            for (i = 0; i < 3; ++i)
            {
                int32_t surface_point = rrj_s32(rrj_read32(surface + 8u + 4u * i));
                int32_t component = (int16_t)rrj_u16(rrj_at(normal + 2u * i, 2)) * 16;

                first_projection = rrj_s32((uint32_t)first_projection + (uint32_t)af_floor_shift((int64_t)rrj_s32((uint32_t)first[i] - (uint32_t)surface_point) * component, 16));
                second_projection = rrj_s32((uint32_t)second_projection + (uint32_t)af_floor_shift((int64_t)rrj_s32((uint32_t)second[i] - (uint32_t)surface_point) * component, 16));
            }
            if ((first_projection > 0 && second_projection >= 0) || (first_projection <= 0 && second_projection <= 0))
            {
                collision = -1;
                collision_side = first_projection <= 0;
                collision_position = first_projection <= 0 ? 0x10000 : 0;
            }
            else
            {
                int32_t dot = rrj_s32(sub_8002E698(rrj_at(actor + 528, 6), rrj_at(normal, 6)));

                collision_side = first_projection <= 0;
                collision_position = af_abs((int32_t)sub_8001FC90(dot, first_projection > 0 ? second_projection : first_projection));
                collision = 1;
            }
        }
    }

    if (collision < 0 && (high_speed || (options & 1u)))
    {
        collision_side = (options & 1u) != 0;
        collision_position = (options & 1u) ? 0 : rrj_s32(rrj_read32(surface + 136));
        collision = 1;
    }

candidate_done:
    if (collision > 0 || special_subtype)
    {
        rrj_write32(actor + 832, surface);
        if (!type)
            rrj_write32(object + 560, rrj_read32(object + 560) | 0x02000000u);
        if (!sticky && linked)
        {
            int32_t adjusted = relative_speed < 0 ? 0 : relative_speed;

            if (!high_speed)
            {
                speed -= adjusted >> 1;
            }
            else
            {
                int32_t threshold = rrj_s32(rrj_read32(0x800D3970u));
                int32_t ratio = af_reciprocal(4 * threshold);
                int32_t difference = relative_speed - 2 * threshold;
                int32_t normalized = (int32_t)sub_8001FC90(difference, ratio);
                int32_t clamped = normalized;

                if (clamped < 0)
                    clamped = 0;
                if (clamped > 0x10000)
                    clamped = 0x10000;
                speed = (int32_t)sub_8001FC90(rrj_s32((uint32_t)(int32_t)sub_8001FC90(clamped, -9830) + 58982u), rrj_s32(rrj_read32(actor + 576)));
            }
            rrj_write32(actor + 480, (uint32_t)speed);
            rrj_write32(actor + 576, (uint32_t)speed);
        }
        (void)sub_800AD9BC(actor, surface_value, collision_side, high_speed, collision_position);
        if (rrj_read32(actor + 568) & 0x400u)
            rrj_write32(actor + 828, rrj_read32(actor + 832));
        translation = 0;
        accept = 0;
        goto post_contact;
    }
    accept = 0;
    if (collision)
        goto embedded_done;

no_candidate:
    accept = type || !moving_side || !previous_surface;
    if (embedded)
        apply_linked = 1;

embedded_done:
post_contact:
    if (apply_linked)
    {
        int32_t y = -((int32_t)rrj_u16(rrj_at(translation + 4, 2)));

        rrj_write32(translation, 0u - rrj_read32(translation));
        rrj_write32(translation + 4, (uint32_t)y);
        rrj_write32(translation + 8, 0u - (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(translation + 8, 2)));
        (void)sub_800A8DF0(embedded, rrj_at(translation, 12), 1);
        accept = 0;
        translation = 0;
        if (!rrj_read32(embedded + 480) || sub_800B0510(actor, embedded, actor + 820))
            accept = 1;
        if (accept)
        {
            accept = 0;
            if (sub_800B2F94(embedded, actor, actor + 820))
                accept = !low_speed;
        }
    }

    if (!moving_side || af_abs((int16_t)rrj_u16(rrj_at(normal + 2, 2))) >= 2048 || (type && surface_value > 0x7FFF))
    {
        if (translation)
            (void)sub_800A8DF0(actor, rrj_at(translation, 12), 1);
    }
    else
    {
        uint32_t i;

        for (i = 0; i < 3; ++i)
        {
            inverse_normal[i] = (int16_t)(0u - rrj_u16(rrj_at(actor + 522u + 2u * i, 2)));
            rrj_put16(rrj_at(actor + 820u + 2u * i, 2), (uint16_t)inverse_normal[i]);
        }
        collision_normal = actor + 820;
    }

    if (!accept || has_response)
    {
        result = 0;
        if (!sticky_active && (rrj_read32(actor + 564) & 0x8000u))
            result = rrj_read32(actor + 832) == surface;
    }
    else if (linked)
    {
        result = rrj_s32(sub_80083928(actor, linked, collision_normal));
    }
    else
    {
        result = rrj_s32(sub_80083F30(actor, surface, collision_normal, rrj_s32(af_select_mode(special, surface_flags, embedded, simple_type, collision_mode, options))));
    }

    if (!special)
    {
        if (!simple_type && embedded_state >= 0)
        {
            if (!result)
                return 0;
            if (!linked && !low_speed)
            {
                if (!apply_linked)
                    (void)sub_80017BA0(rrj_read32(surface + 12), rrj_read32(surface + 20), (int16_t)rrj_u16(rrj_at(surface + 150, 2)), 0);
                if (rrj_u16(rrj_at(actor + 172, 2)) < rrj_read32(rrj_read32(0x8005B2F8u) + 48) && rrj_read32(rrj_read32(actor + 852) + 604) < 2u && !rrj_read32(0x8005B220u))
                    (void)sub_800B658C(actor, 0, speed, (embedded ? 0x0D6945 : 0) + 1464860, embedded ? 2 : 1);
            }
        }
        return 1;
    }
    return (uint32_t)result;
}
