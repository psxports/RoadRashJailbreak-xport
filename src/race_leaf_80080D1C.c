#include "race_leaf_80080D1C.h"
#include "fixed_math.h"
#include "race_pause.h"
#include "xport.h"

static int32_t leaf80_divide(int32_t numerator, int32_t denominator)
{
    uint32_t numerator_magnitude = numerator > 0 ? (uint32_t)numerator : 0u - (uint32_t)numerator;
    uint32_t denominator_magnitude = denominator > 0 ? (uint32_t)denominator : 0u - (uint32_t)denominator;
    uint32_t result = sub_80010028(numerator_magnitude, denominator_magnitude);

    if ((numerator > 0) != (denominator > 0))
        result = 0u - result;
    return ((int32_t)(uint32_t)(result));
}

static int16_t leaf80_cross_component(int64_t value)
{
    int64_t shifted = value >= 0 ? value / 4096 : -(((-value) + 4095) / 4096);

    if (shifted > 32767)
        shifted = 32767;
    if (shifted < -32768)
        shifted = -32768;
    return (int16_t)shifted;
}

static void leaf80_cross(uint32_t left, uint32_t right, uint32_t output)
{
    int16_t a[3];
    int16_t b[3];
    int16_t cross[3];
    uint32_t i;

    for (i = 0; i < 3; ++i)
    {
        a[i] = (int16_t)r_u16(left + 2u * i);
        b[i] = (int16_t)r_u16(right + 2u * i);
    }
    cross[0] = leaf80_cross_component((int64_t)a[1] * b[2] - (int64_t)a[2] * b[1]);
    cross[1] = leaf80_cross_component((int64_t)a[2] * b[0] - (int64_t)a[0] * b[2]);
    cross[2] = leaf80_cross_component((int64_t)a[0] * b[1] - (int64_t)a[1] * b[0]);
    for (i = 0; i < 3; ++i)
        w_u16(output + 2u * i, (uint16_t)cross[i]);
}

static void leaf80_scaled_sum(uint32_t left, const uint16_t right[3], int32_t scale, int32_t output[3])
{
    uint32_t i;

    for (i = 0; i < 3; ++i)
    {
        int32_t left_value = (int16_t)r_u16(left + 2u * i) * 16;
        int32_t right_value = (int16_t)xport_load_le16((const uint8_t *)right + 2u * i) * 16;

        output[i] = ((int32_t)(uint32_t)((uint32_t)(int32_t)sub_8001FC90(right_value, scale) + (uint32_t)left_value));
    }
}

static void leaf80_store_short(uint32_t output, const int32_t values[3])
{
    uint32_t i;

    for (i = 0; i < 3; ++i)
        w_u16(output + 2u * i, (uint16_t)(values[i] >> 4));
}

static void leaf80_copy_short(uint32_t output, const uint16_t input[3])
{
    uint32_t i;

    for (i = 0; i < 3; ++i)
        w_u16(output + 2u * i, xport_load_le16((const uint8_t *)input + 2u * i));
}

uint32_t sub_80080D1C(uint32_t actor, uint32_t vector, uint32_t speed_io, uint32_t surface, const uint16_t normal[3], int32_t bias, int32_t angle, int32_t comparison, int32_t minimum_speed, int32_t angle_adjustment, uint32_t suppress_special)
{
    uint32_t flags = r_u32(actor + 568);
    uint32_t surface_type = r_u16(surface) >> 5;
    int32_t speed;
    uint32_t result = 1;
    uint32_t moving;
    uint32_t original_flags;
    int32_t angle_magnitude;
    int32_t temporary[3];
    uint32_t saved_bit;
    uint32_t i;

    FUNCTION_MARKER(0x80080D1C, "RASHCDG.BIN");
    if (((int32_t)(uint32_t)(r_u32(actor + 364))) < 0)
        flags |= 0x00400000u;
    else
        flags &= ~0x00400000u;
    w_u32(actor + 568, flags);

    if (flags & 0x100u)
    {
        if (((int32_t)(uint32_t)(r_u32(actor + 576))) > 292971)
        {
            int32_t dot = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(vector, 6), normal)));
            int32_t first_dot;
            int32_t second_dot;
            int32_t collision_angle;

            (void)sub_8002EA20(xport_guest_ptr(vector, 6), normal, ((int32_t)(uint32_t)(0u - ((uint32_t)dot << 1))), xport_guest_ptr(actor + 456, 12));
            for (i = 0; i < 3; ++i)
                w_u16(vector + 2u * i, (uint16_t)(((int32_t)(uint32_t)(r_u32(actor + 456u + 4u * i))) >> 4));
            leaf80_cross(actor + 522, vector, actor + 814);
            if (!sub_8002E468(xport_guest_ptr(actor + 814, 6)))
                leaf80_copy_short(actor + 814, xport_guest_ptr(actor + 432, 6));
            leaf80_cross(actor + 814, actor + 522, vector);
            speed = (int32_t)sub_8001FC90(0x8000, ((int32_t)(uint32_t)(r_u32(actor + 576))));
            w_u32(speed_io, (uint32_t)speed);
            w_u32(actor + 576, (uint32_t)speed);
            (void)sub_8002EE50((uint32_t)speed, xport_guest_ptr(vector, 6), xport_guest_ptr(actor + 456, 12));
            first_dot = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(actor + 528, 6), xport_guest_ptr(actor + 814, 6))));
            second_dot = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(actor + 528, 6), xport_guest_ptr(actor + 450, 6))));
            collision_angle = ((int32_t)(uint32_t)(sub_80020018((uint32_t)first_dot, (uint32_t)second_dot)));
            w_u32(actor + 676, (uint32_t)((int32_t)(25736 * (uint32_t)collision_angle) >> 8));
        }
        else
        {
            w_u32(actor + 576, 0);
        }
        flags = r_u32(actor + 568) & 0xFFFFFFF0u;
        if (surface_type == 3 || surface_type == 5 || surface_type == 6)
            flags |= 0x2000u;
        w_u32(actor + 568, flags);
        goto finish;
    }

    moving = (flags & 0x600u) != 0;
    original_flags = flags | (((!moving) & ((flags >> 4) & 1u)) << 27);
    flags = (flags & 0xFFFFEFEFu) | (((!moving) & ((flags >> 4) & 1u)) << 27);
    w_u32(actor + 568, flags);
    speed = moving ? ((int32_t)(uint32_t)(r_u32(speed_io))) : ((int32_t)(uint32_t)(r_u32(actor + 576)));

    if (!moving && speed <= 292971)
    {
        if (original_flags & 2u)
            goto low_speed;
        if ((r_u32(actor + 564) & 0x4000u) && minimum_speed < ((int32_t)(uint32_t)(r_u32(speed_io))))
            goto low_speed;
    }
    if (r_u32(actor + 564) & 0x4000u)
    {
        int32_t impulse;

        speed = minimum_speed < 0x10000 ? 0x10000 : minimum_speed;
        if (moving || !r_u32(actor + 616))
        {
            if (speed <= 655360)
            {
                int32_t ratio = leaf80_divide(speed, 655360);

                impulse = ((int32_t)(uint32_t)((uint32_t)(int32_t)sub_8001FC90(-163840, ratio) + 393216u));
            }
            else
            {
                impulse = 229376;
            }
            w_u32(actor + 624, (uint32_t)(int32_t)sub_8001FC90(0x40000, impulse));
            w_u32(actor + 632, 0);
            w_u32(actor + 620, 0xFFFE0000u);
            w_u32(actor + 628, r_u32(actor + 616));
            if (((int32_t)(uint32_t)(r_u32(speed_io))) < minimum_speed)
            {
                w_u32(actor + 620, 0u - r_u32(actor + 620));
                w_u32(actor + 624, 0u - r_u32(actor + 624));
            }
        }
        goto update_speed;
    }

    {
        uint32_t owner = r_u32(actor + 852);

        angle_magnitude = ((int32_t)(((int32_t)(angle)) < 0 ? ((int32_t)(uint32_t)(0u - (uint32_t)((int32_t)(angle)))) : ((int32_t)(angle))));
        if (r_u32(owner + 604) < 2u)
        {
            w_u32(owner + 480, (uint32_t)speed);
            leaf80_copy_short(owner + 456, xport_guest_ptr(vector, 6));
            w_u32(owner + 552, r_u32(owner + 552) | 0x200000u);
        }
        if (r_u8(owner + 572) & 0x10u)
        {
            uint32_t linked_owner = r_u32(r_u32(actor + 856) + 852);

            if (r_u32(linked_owner + 604) < 2u)
            {
                w_u32(linked_owner + 480, (uint32_t)speed);
                leaf80_copy_short(linked_owner + 456, xport_guest_ptr(vector, 6));
                w_u32(linked_owner + 552, r_u32(linked_owner + 552) | 0x200000u);
            }
        }
    }

    leaf80_copy_short(actor + 808, xport_guest_ptr(vector, 6));
    saved_bit = r_u32(actor + 568) & 1u;
    if (moving && (r_u32(actor + 568) & 4u))
    {
        for (i = 0; i < 3; ++i)
            temporary[i] = ((int32_t)(uint32_t)(0u - ((uint32_t)(int32_t)r_s16(vector + 2u * i) << 4)));
    }
    else if (!moving && !suppress_special && (r_u32(actor + 568) & 0xCu) == 4u && (((int32_t)(uint32_t)(r_u32(actor + 616))) >= 17158 || (angle > 57189 && ((int32_t)(uint32_t)(r_u32(actor + 676))) < -34314) || (angle <= -57190 && ((int32_t)(uint32_t)(r_u32(actor + 676))) > 34314)))
    {
        int32_t dot;

        w_u32(actor + 488, 0xFFF80000u);
        w_u16(actor + 798, 0);
        w_u32(actor + 728, 0);
        w_u32(actor + 568, (r_u32(actor + 568) & 0xFFF00600u) | 0x200u);
        w_u16(actor + 800, (uint16_t)(0u - r_u16(actor + 450)));
        w_u16(actor + 796, r_u16(actor + 454));
        dot = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(vector, 6), normal)));
        (void)sub_8002EA20(xport_guest_ptr(vector, 6), normal, (int32_t)(0 - ((uint32_t)dot << 1)), (uint32_t *)temporary);
        speed = 327680;
        w_u32(speed_io, (uint32_t)speed);
        goto store_vector;
    }
    else
    {
        int32_t vector_x = (int16_t)r_u16(vector);
        int32_t vector_y = (int16_t)r_u16(vector + 2);
        int32_t vector_z = (int16_t)r_u16(vector + 4);
        int32_t normal_x = (int16_t)xport_load_le16(normal);
        int32_t normal_z = (int16_t)xport_load_le16((const uint8_t *)normal + 4);

        if ((int32_t)((uint32_t)(normal_z * vector_x) - (uint32_t)(normal_x * vector_z)) <= 0)
        {
            temporary[0] = -16 * normal_z;
            temporary[2] = 16 * normal_x;
        }
        else
        {
            temporary[0] = 16 * normal_z;
            temporary[2] = -16 * normal_x;
        }
        temporary[1] = moving && vector_y > 0 ? 16 * vector_y : 0;
    }

    flags = r_u32(actor + 568);
    if (angle >= 0)
        flags &= ~0x00100000u;
    else
        flags |= 0x00100000u;
    w_u32(actor + 568, flags);
store_vector:
    leaf80_store_short(vector, temporary);
    if (!moving)
    {
        leaf80_cross(actor + 522, vector, actor + 814);
        if (!sub_8002E468(xport_guest_ptr(actor + 814, 6)))
            leaf80_copy_short(actor + 814, xport_guest_ptr(actor + 432, 6));
        leaf80_cross(actor + 814, actor + 522, vector);
    }
    w_u32(actor + 828, surface);
    if (r_u32(actor + 568) & 0x200u)
        goto update_speed;

    if (saved_bit)
    {
        int32_t ratio = leaf80_divide(angle_magnitude, comparison);
        int32_t response = (int32_t)sub_8001FC90(-1311, ratio);
        int32_t lower;
        int32_t reciprocal;
        int32_t angular;

        speed = (int32_t)sub_8001FC90(((int32_t)(uint32_t)((uint32_t)response + 65208u)), speed);
        {
            uint32_t below = (uint32_t)((int32_t)((uint32_t)speed - 0x20000) >> 31) & (0x20000 - (uint32_t)speed);
            uint32_t above = 655360 - (uint32_t)speed;
            uint32_t magnitude;
            uint32_t denominator;

            above &= (uint32_t)((int32_t)above >> 31);
            lower = (int32_t)((uint32_t)speed + below + above);
            magnitude = lower < 0 ? 0 - (uint32_t)lower : (uint32_t)lower;
            denominator = (uint32_t)((int32_t)magnitude >> 1) + (uint32_t)((int32_t)(magnitude - 2) >> 31);
            reciprocal = (int32_t)(denominator ? 0x80000000 / denominator : 0xFFFFFFFF);
            if (lower < 0)
                reciprocal = (int32_t)(0 - (uint32_t)reciprocal);
        }
        w_u32(actor + 728, (uint32_t)reciprocal);
        angular = angle <= 0 ? ((int32_t)(uint32_t)((uint32_t)angle - (uint32_t)angle_adjustment)) : ((int32_t)(uint32_t)((uint32_t)angle + (uint32_t)angle_adjustment));
        w_u32(actor + 488, (uint32_t)(int32_t)sub_8001FC90(angular, lower));

        if (surface_type == 8 || (surface_type == 6 && ((int32_t)(uint32_t)(r_u32(surface + 132))) > 0x10000 && ((int32_t)(uint32_t)(r_u32(surface + 136))) > 0x10000 && ((int32_t)(uint32_t)(r_u32(surface + 140))) > 0x10000))
        {
            if (((int32_t)(uint32_t)(r_u32(actor + 488) ^ r_u32(actor + 636))) < 0)
            {
                int32_t prior = ((int32_t)(uint32_t)(r_u32(actor + 636)));
                int32_t scale = (int32_t)sub_8001FC90(((int32_t)(((int32_t)(prior)) < 0 ? ((int32_t)(uint32_t)(0u - (uint32_t)((int32_t)(prior)))) : ((int32_t)(prior)))), lower);

                w_u32(actor + 644, 0);
                w_u32(actor + 640, (uint32_t)scale);
                if (scale < 0x10000)
                    scale = 0x10000;
                w_u32(actor + 640, (uint32_t)scale);
                w_u32(actor + 640, prior < 0 ? (uint32_t)scale : 0u - (uint32_t)scale);
            }
            else
            {
                w_u32(actor + 640, 0);
            }
            goto update_speed;
        }
        else
        {
            int alternate = 0;
            int threshold = angle_magnitude < 17157;
            int32_t step = threshold ? 5719 : 11438;
            int32_t prior = ((int32_t)(uint32_t)(r_u32(actor + 636)));
            int32_t target;
            uint32_t configuration = r_u32(actor + 556);
            int32_t limit = ((int32_t)(uint32_t)(r_u32(configuration + 228)));
            int32_t delta;
            int32_t scale;

            if (!surface_type)
            {
                uint32_t other = r_u32(0x8005B3A0u) + 1096u * r_u16(surface);

                if ((r_u32(other + 568) & 1u) && ((int32_t)(uint32_t)(r_u32(actor + 636) ^ r_u32(actor + 488))) < 0)
                    alternate = ((int32_t)(uint32_t)(r_u32(actor + 636) ^ r_u32(other + 636))) < 0;
            }
            if (angle_magnitude > 34314)
                step += 5719;
            target = ((int32_t)(uint32_t)((uint32_t)prior + (((int32_t)(uint32_t)(r_u32(actor + 488))) >= 0 ? (uint32_t)step : 0u - (uint32_t)step)));
            w_u32(actor + 644, (uint32_t)target);
            if (alternate && ((int32_t)(uint32_t)((uint32_t)target ^ (uint32_t)prior)) >= 0)
                target = 0;
            w_u32(actor + 644, (uint32_t)target);
            {
                uint32_t below = (uint32_t)((int32_t)((uint32_t)target + (uint32_t)limit) >> 31) & (0 - (uint32_t)limit - (uint32_t)target);
                uint32_t above = (uint32_t)limit - (uint32_t)target;

                above &= (uint32_t)((int32_t)above >> 31);
                target = (int32_t)((uint32_t)target + below + above);
            }
            w_u32(actor + 644, (uint32_t)target);
            delta = (int32_t)((target < 0 ? 0 - (uint32_t)target : (uint32_t)target) - (prior < 0 ? 0 - (uint32_t)prior : (uint32_t)prior));
            scale = (int32_t)sub_8001FC90(delta, lower);
            w_u32(actor + 640, (uint32_t)scale);
            scale = ((int32_t)(((int32_t)(scale)) < 0 ? ((int32_t)(uint32_t)(0u - (uint32_t)((int32_t)(scale)))) : ((int32_t)(scale))));
            if (scale < 0x10000)
                scale = 0x10000;
            w_u32(actor + 640, (uint32_t)scale);
            w_u32(actor + 640, prior < target ? (uint32_t)scale : 0u - (uint32_t)scale);
            goto update_speed;
        }
    }
    else
    {
        int32_t ratio = leaf80_divide((int32_t)(102943 - (uint32_t)angle_magnitude), (int32_t)(102943 - (uint32_t)comparison));
        int32_t scale = (int32_t)sub_8001FC90(6553, ratio);
        int32_t acceleration;

        speed = (int32_t)sub_8001FC90(scale, speed);
        if (moving && speed < 196608)
            speed = 196608;
        acceleration = ((int32_t)(uint32_t)(r_u32(actor + 616)));
        if (acceleration > 0)
            acceleration = 0;
        w_u32(actor + 616, (uint32_t)acceleration);
        {
            int32_t input_speed = ((int32_t)(uint32_t)(r_u32(speed_io)));
            int32_t negative_speed = (int32_t)(0 - (uint32_t)input_speed);
            int32_t half_speed = (int32_t)((uint32_t)negative_speed + ((uint32_t)negative_speed >> 31)) >> 1;
            w_u32(actor + 620, (uint32_t)half_speed);
            if (r_u32(actor + 568) & 8u)
            {
                int32_t numerator = (int32_t)((uint32_t)((int32_t)(((int32_t)(acceleration)) < 0 ? ((int32_t)(uint32_t)(0u - (uint32_t)((int32_t)(acceleration)))) : ((int32_t)(acceleration)))) + 57190);

                w_u32(actor + 728, 0u - (uint32_t)leaf80_divide(numerator, half_speed));
                w_u32(actor + 624, 0);
            }
            else
            {
                int32_t spring = half_speed;
                int32_t damping = (int32_t)((uint32_t)((int32_t)(((int32_t)(acceleration)) < 0 ? ((int32_t)(uint32_t)(0u - (uint32_t)((int32_t)(acceleration)))) : ((int32_t)(acceleration)))) + 45752);
                int32_t velocity;
                int32_t reciprocal;

                {
                    uint32_t below = (uint32_t)((int32_t)((uint32_t)half_speed + 1310720) >> 31) & (0 - 1310720 - (uint32_t)half_speed);
                    uint32_t above = 0 - 262144 - (uint32_t)half_speed;

                    above &= (uint32_t)((int32_t)above >> 31);
                    spring = (int32_t)((uint32_t)half_speed + below + above);
                }
                w_u32(actor + 620, (uint32_t)spring);
                w_u32(actor + 624, (uint32_t)damping);
                if (r_u32(actor + 568) & 4u)
                {
                    velocity = damping;
                }
                else
                {
                    int32_t clamped_bias = bias;

                    uint32_t below = (uint32_t)((int32_t)((uint32_t)bias + 65536) >> 31) & (0 - 65536 - (uint32_t)bias);
                    uint32_t above = 0 - (uint32_t)bias;

                    above &= (uint32_t)((int32_t)above >> 31);
                    clamped_bias = (int32_t)((uint32_t)bias + below + above);
                    velocity = ((int32_t)(uint32_t)(0u - (uint32_t)(int32_t)sub_8001FC90(clamped_bias, damping)));
                }
                w_u32(actor + 624, (uint32_t)velocity);
                if (velocity < 11438)
                    velocity = 11438;
                w_u32(actor + 624, (uint32_t)velocity);
                reciprocal = ((int32_t)(uint32_t)(0u - (uint32_t)leaf80_divide(((int32_t)(uint32_t)((uint32_t)velocity << 1)), spring)));
                w_u32(actor + 728, (uint32_t)reciprocal);
                w_u32(actor + 624, 0u - (uint32_t)leaf80_divide(spring, reciprocal));
                w_u32(actor + 632, 0);
                w_u32(actor + 628, r_u32(actor + 616));
                w_u32(actor + 728, r_u32(actor + 728) << 1);
            }
        }

        if ((r_u32(actor + 568) & 10) == 2)
        {
            int clear_speed = 0;

            if (surface_type == 3)
                clear_speed = r_u32(surface + 480) == 0;
            else if (surface_type == 5 || surface_type == 6)
                clear_speed = 1;
            if (clear_speed)
                speed = 0;
        }
        {
            int32_t reciprocal = leaf80_divide(1143, ((int32_t)(uint32_t)(r_u32(actor + 728))));

            if (angle < 0)
                reciprocal = ((int32_t)(uint32_t)(0u - (uint32_t)reciprocal));
            if (r_u32(actor + 568) & 0xCu)
                reciprocal = ((int32_t)(uint32_t)((uint32_t)reciprocal << 2));
            w_u32(actor + 488, (uint32_t)reciprocal);
        }
        goto update_speed;
    }

low_speed:
    flags = r_u32(actor + 568) & 0xFFFFFFFCu;
    w_u32(actor + 568, flags);
    w_u32(actor + 728, 0);
    if (!(r_u32(actor + 560) & 0x300u))
        speed = 0;
    w_u32(actor + 564, (r_u32(actor + 564) & 0xFFFFBFFFu) | 0x1000u);
    {
        uint32_t active = 0;

        if (surface_type || !(r_u32(r_u32(0x8005B3A0u) + 1096u * r_u16(surface) + 560) & 0x2000u))
        {
            if ((r_u32(actor + 560) & 0x08002000u) == 0x08000000u)
                active = 1;
        }
        w_u32(actor + 560, r_u32(actor + 560) | (active ? 0x6000u : 0u));
    }
    result = 0;

update_speed:
    if (moving)
    {
        w_u32(speed_io, (uint32_t)speed);
        (void)sub_8002EE50((uint32_t)speed, xport_guest_ptr(vector, 6), xport_guest_ptr(actor + 456, 12));
    }
    else
    {
        w_u32(actor + 576, (uint32_t)speed);
    }

finish:
    leaf80_copy_short(actor + 820, normal);
    return result;
}
