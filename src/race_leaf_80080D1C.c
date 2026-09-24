#include "race_leaf_80080D1C.h"
#include "fixed_math.h"
#include "race_pause.h"
#include "xport.h"

static int32_t leaf80_abs(int32_t value)
{
    return value < 0 ? rrj_s32(0u - (uint32_t)value) : value;
}

static int32_t leaf80_asr1(uint32_t value)
{
    return rrj_s32((value >> 1) | (value & 0x80000000u));
}

static int32_t leaf80_divide(int32_t numerator, int32_t denominator)
{
    uint32_t numerator_magnitude = numerator > 0
                                       ? (uint32_t)numerator
                                       : 0u - (uint32_t)numerator;
    uint32_t denominator_magnitude = denominator > 0
                                         ? (uint32_t)denominator
                                         : 0u - (uint32_t)denominator;
    uint32_t result =
        sub_80010028(numerator_magnitude, denominator_magnitude);

    if ((numerator > 0) != (denominator > 0))
        result = 0u - result;
    return rrj_s32(result);
}

static int16_t leaf80_cross_component(int64_t value)
{
    int64_t shifted = value >= 0 ? value / 4096
                                 : -(((-value) + 4095) / 4096);

    if (shifted > 32767)
        shifted = 32767;
    if (shifted < -32768)
        shifted = -32768;
    return (int16_t)shifted;
}

static void leaf80_cross(RRJMemory *m, uint32_t left, uint32_t right,
                         uint32_t output)
{
    int16_t a[3];
    int16_t b[3];
    int16_t cross[3];
    uint32_t i;

    for (i = 0; i < 3; ++i)
    {
        a[i] = (int16_t)rrj_u16(rrj_at(m, left + 2u * i, 2));
        b[i] = (int16_t)rrj_u16(rrj_at(m, right + 2u * i, 2));
    }
    cross[0] = leaf80_cross_component(
        (int64_t)a[1] * b[2] - (int64_t)a[2] * b[1]);
    cross[1] = leaf80_cross_component(
        (int64_t)a[2] * b[0] - (int64_t)a[0] * b[2]);
    cross[2] = leaf80_cross_component(
        (int64_t)a[0] * b[1] - (int64_t)a[1] * b[0]);
    for (i = 0; i < 3; ++i)
        rrj_put16(rrj_at(m, output + 2u * i, 2), (uint16_t)cross[i]);
}

static void leaf80_scaled_sum(RRJMemory *m, uint32_t left,
                              uint32_t right, int32_t scale,
                              int32_t output[3])
{
    uint32_t i;

    for (i = 0; i < 3; ++i)
    {
        int32_t left_value =
            (int16_t)rrj_u16(rrj_at(m, left + 2u * i, 2)) * 16;
        int32_t right_value =
            (int16_t)rrj_u16(rrj_at(m, right + 2u * i, 2)) * 16;

        output[i] = rrj_s32(
            (uint32_t)(int32_t)sub_8001FC90(right_value, scale) +
            (uint32_t)left_value);
    }
}

static void leaf80_store_short(RRJMemory *m, uint32_t output,
                               const int32_t values[3])
{
    uint32_t i;

    for (i = 0; i < 3; ++i)
        rrj_put16(rrj_at(m, output + 2u * i, 2),
                  (uint16_t)(values[i] >> 4));
}

static void leaf80_copy_short(RRJMemory *m, uint32_t output,
                              uint32_t input)
{
    uint32_t i;

    for (i = 0; i < 3; ++i)
        rrj_put16(rrj_at(m, output + 2u * i, 2),
                  rrj_u16(rrj_at(m, input + 2u * i, 2)));
}

uint32_t sub_80080D1C(RRJMemory *m, uint32_t actor, uint32_t vector,
                      uint32_t speed_io, uint32_t surface,
                      uint32_t normal, int32_t bias, int32_t angle,
                      int32_t comparison, int32_t minimum_speed,
                      int32_t angle_adjustment, uint32_t suppress_special)
{
    uint32_t flags = rrj_read32(m, actor + 568);
    uint32_t surface_type = rrj_u16(rrj_at(m, surface, 2)) >> 5;
    int32_t speed;
    uint32_t result = 1;
    uint32_t moving;
    uint32_t original_flags;
    int32_t angle_magnitude;
    int32_t temporary[3];
    uint32_t saved_bit;
    uint32_t i;

    FUNCTION_MARKER(0x80080D1C, "RASHCDG.BIN");
    if (rrj_s32(rrj_read32(m, actor + 364)) < 0)
        flags |= 0x00400000u;
    else
        flags &= ~0x00400000u;
    rrj_write32(m, actor + 568, flags);

    if (flags & 0x100u)
    {
        if (rrj_s32(rrj_read32(m, actor + 576)) > 292971)
        {
            int32_t dot = rrj_s32(sub_8002E698(m, vector, normal));
            int32_t first_dot;
            int32_t second_dot;
            int32_t collision_angle;

            (void)sub_8002EA20(m, vector, normal,
                               rrj_s32(0u - ((uint32_t)dot << 1)),
                               actor + 456);
            for (i = 0; i < 3; ++i)
                rrj_put16(
                    rrj_at(m, vector + 2u * i, 2),
                    (uint16_t)(rrj_s32(rrj_read32(
                                   m, actor + 456u + 4u * i)) >>
                               4));
            leaf80_cross(m, actor + 522, vector, actor + 814);
            if (!sub_8002E468(m, actor + 814))
                leaf80_copy_short(m, actor + 814, actor + 432);
            leaf80_cross(m, actor + 814, actor + 522, vector);
            speed = (int32_t)sub_8001FC90(
                0x8000, rrj_s32(rrj_read32(m, actor + 576)));
            rrj_write32(m, speed_io, (uint32_t)speed);
            rrj_write32(m, actor + 576, (uint32_t)speed);
            (void)sub_8002EE50(m, (uint32_t)speed, vector,
                               actor + 456);
            first_dot = rrj_s32(
                sub_8002E698(m, actor + 528, actor + 814));
            second_dot = rrj_s32(
                sub_8002E698(m, actor + 528, actor + 450));
            collision_angle = rrj_s32(
                sub_80020018(m, (uint32_t)first_dot,
                             (uint32_t)second_dot));
            rrj_write32(
                m, actor + 676,
                (uint32_t)(((int64_t)25736 * collision_angle) >> 8));
        }
        else
        {
            rrj_write32(m, actor + 576, 0);
        }
        flags = rrj_read32(m, actor + 568) & 0xFFFFFFF0u;
        if (surface_type == 3 || surface_type == 5 || surface_type == 6)
            flags |= 0x2000u;
        rrj_write32(m, actor + 568, flags);
        goto finish;
    }

    moving = (flags & 0x600u) != 0;
    original_flags =
        flags | (((!moving) & ((flags >> 4) & 1u)) << 27);
    flags = (flags & 0xFFFFEFEFu) |
            (((!moving) & ((flags >> 4) & 1u)) << 27);
    rrj_write32(m, actor + 568, flags);
    speed = moving ? rrj_s32(rrj_read32(m, speed_io))
                   : rrj_s32(rrj_read32(m, actor + 576));

    if (!moving && speed <= 292971)
    {
        if (original_flags & 2u)
            goto low_speed;
        if ((rrj_read32(m, actor + 564) & 0x4000u) &&
            minimum_speed < rrj_s32(rrj_read32(m, speed_io)))
            goto low_speed;
    }
    if (rrj_read32(m, actor + 564) & 0x4000u)
    {
        int32_t impulse;

        speed = minimum_speed < 0x10000 ? 0x10000 : minimum_speed;
        if (moving || !rrj_read32(m, actor + 616))
        {
            if (speed <= 655360)
            {
                int32_t ratio = leaf80_divide(speed, 655360);

                impulse = rrj_s32(
                    (uint32_t)(int32_t)sub_8001FC90(-163840, ratio) +
                    393216u);
            }
            else
            {
                impulse = 229376;
            }
            rrj_write32(
                m, actor + 624,
                (uint32_t)(int32_t)sub_8001FC90(0x40000, impulse));
            rrj_write32(m, actor + 632, 0);
            rrj_write32(m, actor + 620, 0xFFFE0000u);
            rrj_write32(m, actor + 628, rrj_read32(m, actor + 616));
            if (rrj_s32(rrj_read32(m, speed_io)) < minimum_speed)
            {
                rrj_write32(m, actor + 620,
                            0u - rrj_read32(m, actor + 620));
                rrj_write32(m, actor + 624,
                            0u - rrj_read32(m, actor + 624));
            }
        }
        goto update_speed;
    }

    {
        uint32_t owner = rrj_read32(m, actor + 852);

        angle_magnitude = leaf80_abs(angle);
        if (rrj_read32(m, owner + 604) < 2u)
        {
            rrj_write32(m, owner + 480, (uint32_t)speed);
            leaf80_copy_short(m, owner + 456, vector);
            rrj_write32(m, owner + 552,
                        rrj_read32(m, owner + 552) | 0x200000u);
        }
        if (*(uint8_t *)rrj_at(m, owner + 572, 1) & 0x10u)
        {
            uint32_t linked_owner = rrj_read32(
                m, rrj_read32(m, actor + 856) + 852);

            if (rrj_read32(m, linked_owner + 604) < 2u)
            {
                rrj_write32(m, linked_owner + 480, (uint32_t)speed);
                leaf80_copy_short(m, linked_owner + 456, vector);
                rrj_write32(
                    m, linked_owner + 552,
                    rrj_read32(m, linked_owner + 552) | 0x200000u);
            }
        }
    }

    leaf80_copy_short(m, actor + 808, vector);
    saved_bit = rrj_read32(m, actor + 568) & 1u;
    if (moving && (rrj_read32(m, actor + 568) & 4u))
    {
        for (i = 0; i < 3; ++i)
            temporary[i] = rrj_s32(
                0u - ((uint32_t)(int32_t)(int16_t)rrj_u16(
                          rrj_at(m, vector + 2u * i, 2)) <<
                      4));
    }
    else if (!moving && !suppress_special &&
             (rrj_read32(m, actor + 568) & 0xCu) == 4u &&
             (rrj_s32(rrj_read32(m, actor + 616)) >= 17158 ||
              (angle > 57189 &&
               rrj_s32(rrj_read32(m, actor + 676)) < -34314) ||
              (angle <= -57190 &&
               rrj_s32(rrj_read32(m, actor + 676)) > 34314)))
    {
        int32_t dot;

        rrj_write32(m, actor + 488, 0xFFF80000u);
        rrj_put16(rrj_at(m, actor + 798, 2), 0);
        rrj_write32(m, actor + 728, 0);
        rrj_write32(m, actor + 568,
                    (rrj_read32(m, actor + 568) & 0xFFF00400u) |
                        0x200u);
        rrj_put16(rrj_at(m, actor + 800, 2),
                  (uint16_t)(0u - rrj_u16(rrj_at(m, actor + 450, 2))));
        rrj_put16(rrj_at(m, actor + 796, 2),
                  rrj_u16(rrj_at(m, actor + 454, 2)));
        dot = rrj_s32(sub_8002E698(m, vector, normal));
        leaf80_scaled_sum(m, vector, normal,
                          rrj_s32(0u - ((uint32_t)dot << 1)),
                          temporary);
        speed = 327680;
        rrj_write32(m, speed_io, (uint32_t)speed);
    }
    else
    {
        int32_t vector_x =
            (int16_t)rrj_u16(rrj_at(m, vector, 2));
        int32_t vector_y =
            (int16_t)rrj_u16(rrj_at(m, vector + 2, 2));
        int32_t vector_z =
            (int16_t)rrj_u16(rrj_at(m, vector + 4, 2));
        int32_t normal_x =
            (int16_t)rrj_u16(rrj_at(m, normal, 2));
        int32_t normal_z =
            (int16_t)rrj_u16(rrj_at(m, normal + 4, 2));

        if (normal_z * vector_x - normal_x * vector_z <= 0)
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

    flags = rrj_read32(m, actor + 568);
    if (angle >= 0)
        flags &= ~0x00100000u;
    else
        flags |= 0x00100000u;
    rrj_write32(m, actor + 568, flags);
    leaf80_store_short(m, vector, temporary);
    if (!moving)
    {
        leaf80_cross(m, actor + 522, vector, actor + 814);
        if (!sub_8002E468(m, actor + 814))
            leaf80_copy_short(m, actor + 814, actor + 432);
        leaf80_cross(m, actor + 814, actor + 522, vector);
    }
    rrj_write32(m, actor + 828, surface);
    if (rrj_read32(m, actor + 568) & 0x200u)
        goto update_speed;

    if (saved_bit)
    {
        int32_t ratio = leaf80_divide(angle_magnitude, comparison);
        int32_t response = (int32_t)sub_8001FC90(-1311, ratio);
        int32_t lower;
        int32_t reciprocal;
        int32_t angular;

        speed = (int32_t)sub_8001FC90(
            rrj_s32((uint32_t)response + 65208u), speed);
        if (speed < 0x20000)
            speed = 0x20000;
        if (speed > 655360)
            speed = 655360;
        lower = speed;
        if (lower < 0x20000)
            lower = 0x20000;
        if (lower > 655360)
            lower = 655360;
        reciprocal = (int32_t)(0x80000000u / ((uint32_t)lower >> 1));
        rrj_write32(m, actor + 728, (uint32_t)reciprocal);
        angular = angle <= 0
                      ? rrj_s32((uint32_t)angle -
                                (uint32_t)angle_adjustment)
                      : rrj_s32((uint32_t)angle +
                                (uint32_t)angle_adjustment);
        rrj_write32(
            m, actor + 488,
            (uint32_t)(int32_t)sub_8001FC90(angular, lower));

        if (surface_type == 8 ||
            (surface_type == 6 &&
             rrj_s32(rrj_read32(m, surface + 132)) > 0x10000 &&
             rrj_s32(rrj_read32(m, surface + 136)) > 0x10000 &&
             rrj_s32(rrj_read32(m, surface + 140)) > 0x10000))
        {
            if (rrj_s32(rrj_read32(m, actor + 488) ^
                        rrj_read32(m, actor + 636)) < 0)
            {
                int32_t prior = rrj_s32(rrj_read32(m, actor + 636));
                int32_t scale = (int32_t)sub_8001FC90(
                    leaf80_abs(prior), lower);

                rrj_write32(m, actor + 644, 0);
                if (scale < 0x10000)
                    scale = 0x10000;
                rrj_write32(m, actor + 640,
                            prior < 0 ? (uint32_t)scale
                                      : 0u - (uint32_t)scale);
            }
            else
            {
                rrj_write32(m, actor + 640, 0);
            }
            goto update_speed;
        }
        else
        {
            int alternate = 0;
            int threshold = angle_magnitude < 17157;
            int32_t step = threshold ? 5719 : 11438;
            int32_t prior = rrj_s32(rrj_read32(m, actor + 636));
            int32_t target;
            uint32_t configuration = rrj_read32(m, actor + 556);
            int32_t limit = rrj_s32(
                rrj_read32(m, configuration + 228));
            int32_t delta;
            int32_t scale;

            if (!surface_type)
            {
                uint32_t other =
                    rrj_read32(m, 0x8005B3A0u) +
                    1096u * rrj_u16(rrj_at(m, surface, 2));

                if ((rrj_read32(m, other + 568) & 1u) &&
                    rrj_s32(rrj_read32(m, actor + 636) ^
                            rrj_read32(m, actor + 488)) < 0)
                    alternate =
                        rrj_s32(rrj_read32(m, actor + 636) ^
                                rrj_read32(m, other + 636)) < 0;
            }
            if (angle_magnitude > 34314)
                step += 5719;
            target = rrj_s32((uint32_t)prior +
                             (rrj_s32(rrj_read32(m, actor + 488)) >= 0
                                  ? (uint32_t)step
                                  : 0u - (uint32_t)step));
            rrj_write32(m, actor + 644, (uint32_t)target);
            if (!alternate || rrj_s32((uint32_t)target ^
                                      (uint32_t)prior) < 0)
                target = 0;
            if (target < -limit)
                target = -limit;
            if (target > limit)
                target = limit;
            rrj_write32(m, actor + 644, (uint32_t)target);
            delta = leaf80_abs(target) - leaf80_abs(prior);
            scale = (int32_t)sub_8001FC90(delta, lower);
            rrj_write32(m, actor + 640, (uint32_t)scale);
            scale = leaf80_abs(scale);
            if (scale < 0x10000)
                scale = 0x10000;
            rrj_write32(m, actor + 640,
                        prior < target ? (uint32_t)scale
                                       : 0u - (uint32_t)scale);
            goto update_speed;
        }
    }
    else
    {
        int32_t ratio = leaf80_divide(102943 - angle_magnitude,
                                      102943 - comparison);
        int32_t scale = (int32_t)sub_8001FC90(6553, ratio);
        int32_t acceleration;

        speed = (int32_t)sub_8001FC90(scale, speed);
        if (moving && speed < 196608)
            speed = 196608;
        acceleration = rrj_s32(rrj_read32(m, actor + 616));
        if (acceleration > 0)
            acceleration = 0;
        rrj_write32(m, actor + 616, (uint32_t)acceleration);
        {
            int32_t input_speed = rrj_s32(rrj_read32(m, speed_io));
            int32_t half_speed = leaf80_asr1(
                (uint32_t)(input_speed > 0) - (uint32_t)input_speed);
            rrj_write32(m, actor + 620, (uint32_t)half_speed);
            if (rrj_read32(m, actor + 568) & 8u)
            {
                int32_t numerator = leaf80_abs(acceleration) + 57190;

                rrj_write32(
                    m, actor + 728,
                    0u - (uint32_t)leaf80_divide(numerator, half_speed));
                rrj_write32(m, actor + 624, 0);
            }
            else
            {
                int32_t spring = half_speed;
                int32_t damping = leaf80_abs(acceleration) + 45752;
                int32_t velocity;
                int32_t reciprocal;

                if (spring < -1310720)
                    spring = -1310720;
                if (spring > -262144)
                    spring = -262144;
                rrj_write32(m, actor + 620, (uint32_t)spring);
                if (rrj_read32(m, actor + 568) & 4u)
                {
                    velocity = damping;
                }
                else
                {
                    int32_t clamped_bias = bias;

                    if (clamped_bias < -65536)
                        clamped_bias = -65536;
                    if (clamped_bias > 0)
                        clamped_bias = 0;
                    velocity = rrj_s32(
                        0u - (uint32_t)(int32_t)sub_8001FC90(
                                 clamped_bias, damping));
                }
                if (velocity < 11438)
                    velocity = 11438;
                rrj_write32(m, actor + 624, (uint32_t)velocity);
                reciprocal = rrj_s32(
                    0u - (uint32_t)leaf80_divide(
                             rrj_s32((uint32_t)velocity << 1), spring));
                rrj_write32(m, actor + 728, (uint32_t)reciprocal);
                rrj_write32(
                    m, actor + 624,
                    0u - (uint32_t)leaf80_divide(spring, reciprocal));
                rrj_write32(m, actor + 632, 0);
                rrj_write32(m, actor + 628,
                            rrj_read32(m, actor + 616));
                rrj_write32(m, actor + 728,
                            rrj_read32(m, actor + 728) << 1);
            }
        }

        if (rrj_read32(m, actor + 568) & 2u)
        {
            int clear_speed = 0;

            if (surface_type == 3)
                clear_speed = rrj_read32(m, surface + 480) == 0;
            else if (surface_type == 5 || surface_type == 6)
                clear_speed = 1;
            if (clear_speed)
                speed = 0;
        }
        {
            int32_t reciprocal = leaf80_divide(
                1143, rrj_s32(rrj_read32(m, actor + 728)));

            if (angle < 0)
                reciprocal = rrj_s32(0u - (uint32_t)reciprocal);
            if (rrj_read32(m, actor + 568) & 0xCu)
                reciprocal = rrj_s32((uint32_t)reciprocal << 2);
            rrj_write32(m, actor + 488, (uint32_t)reciprocal);
        }
        goto update_speed;
    }

low_speed:
    flags = rrj_read32(m, actor + 568) & 0xFFFFFFFCu;
    rrj_write32(m, actor + 568, flags);
    rrj_write32(m, actor + 728, 0);
    if (!(rrj_read32(m, actor + 560) & 0x300u))
        speed = 0;
    rrj_write32(m, actor + 564,
                (rrj_read32(m, actor + 564) & 0xFFFFAFFFu) |
                    0x1000u);
    {
        uint32_t active = 0;

        if (surface_type ||
            !(rrj_read32(
                  m, rrj_read32(m, 0x8005B3A0u) +
                         1096u * rrj_u16(rrj_at(m, surface, 2)) + 560) &
              0x2000u))
        {
            if ((rrj_read32(m, actor + 560) & 0x08002000u) ==
                0x08000000u)
                active = 1;
        }
        rrj_write32(m, actor + 560,
                    rrj_read32(m, actor + 560) |
                        (active ? 0x6000u : 0u));
    }
    result = 0;

update_speed:
    if (moving)
    {
        rrj_write32(m, speed_io, (uint32_t)speed);
        (void)sub_8002EE50(m, (uint32_t)speed, vector, actor + 456);
    }
    else
    {
        rrj_write32(m, actor + 576, (uint32_t)speed);
    }

finish:
    leaf80_copy_short(m, actor + 820, normal);
    return result;
}
