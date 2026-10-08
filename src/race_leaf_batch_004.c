#include "race_leaf_batch_004.h"
#include "fixed_math.h"
#include "race_leaf_80080D1C.h"
#include "race_leaf.h"
#include "race_pause.h"
#include "xport.h"

static int32_t batch4_blend_nonnegative(int32_t first, int32_t second, int32_t first_scale, int32_t second_scale, int32_t *first_output, int32_t *second_output)
{
    int32_t first_result = (int32_t)sub_8001FC90(rrj_s32((uint32_t)first - (uint32_t)second), first_scale);
    int32_t second_result;
    int32_t call_result;

    first_result = rrj_s32((uint32_t)first_result + (uint32_t)(int32_t)sub_8001FC90(rrj_s32((uint32_t)second << 1), second_scale));
    *first_output = first_result < 0 ? 0 : first_result;

    second_result = (int32_t)sub_8001FC90(rrj_s32((uint32_t)first << 1), first_scale);
    call_result = (int32_t)sub_8001FC90(rrj_s32((uint32_t)second - (uint32_t)first), second_scale);
    second_result = rrj_s32((uint32_t)second_result + (uint32_t)call_result);
    *second_output = second_result < 0 ? 0 : second_result;
    return call_result;
}

uint32_t sub_80083864(RRJMemory *m, int32_t first, int32_t second, int32_t first_scale, int32_t second_scale, uint32_t first_output, uint32_t second_output)
{
    int32_t first_value;
    int32_t second_value;
    int32_t result;

    FUNCTION_MARKER(0x80083864, "RASHCDG.BIN");
    result = batch4_blend_nonnegative(first, second, first_scale, second_scale, &first_value, &second_value);
    rrj_write32(m, first_output, (uint32_t)first_value);
    rrj_write32(m, second_output, (uint32_t)second_value);
    return (uint32_t)result;
}

uint32_t sub_80084BE8(RRJMemory *m, uint32_t actor, uint32_t current)
{
    uint32_t source = actor + 450;
    uint32_t tag;
    uint32_t type;
    int32_t angle_base = 56;
    int32_t value = 435486;
    uint32_t flags;
    uint32_t angle;

    FUNCTION_MARKER(0x80084BE8, "RASHCDG.BIN");
    (void)sub_8002EAD8(m, actor + 504, actor + 522, rrj_read32(m, actor + 768), actor + 184);
    tag = rrj_u16(rrj_at(m, rrj_read32(m, actor + 832), 2));
    type = tag >> 5;
    if (type == 3)
    {
        angle_base = 170;
        value = 725811;
    }
    else if (type == 5)
    {
        uint32_t object = rrj_read32(m, rrj_read32(m, 0x800CE5A4u) + 452u * (tag & 31u));
        uint32_t kind = (rrj_u16(rrj_at(m, object + 14, 2)) & 0xF80u) >> 7;

        angle_base = kind == 8 ? 568 : 455;
        if (kind == 6)
            angle_base -= 114;
        value = kind == 8 ? 725811 : 580648;
        if (kind == 6)
            value -= 145162;
    }
    if (!current)
    {
        source = actor + 864;
        flags = rrj_read32(m, actor + 568);
        if (!(flags & 0x02000000u))
        {
            rrj_put16(rrj_at(m, actor + 864, 2), rrj_u16(rrj_at(m, actor + 450, 2)));
            rrj_put16(rrj_at(m, actor + 866, 2), rrj_u16(rrj_at(m, actor + 452, 2)));
            rrj_put16(rrj_at(m, actor + 868, 2), rrj_u16(rrj_at(m, actor + 454, 2)));
            rrj_write32(m, actor + 860, rrj_read32(m, actor + 480));
            rrj_write32(m, actor + 568, flags | 0x02000000u);
        }
    }
    angle = sub_8001FF3C(m, (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, source + 2, 2)) << 4);
    (void)sub_8007E868(m, source, rrj_s32(rrj_read32(m, actor + 576)), (int16_t)rrj_u16(rrj_at(m, 0x8005624Cu + (4u * ((uint32_t)(angle_base + 1024) - angle) & 0x3FFCu) + 2u, 2)), value);
    flags = rrj_read32(m, actor + 568) | 0xC00u;
    rrj_write32(m, actor + 568, flags);
    return flags;
}

static int32_t batch4_clamp_ratio(int32_t scale, int32_t value, int32_t divisor, int32_t factor)
{
    int32_t result = (int32_t)((int64_t)scale * value / divisor);
    int32_t limit = scale / factor;

    if (result < 0)
        result = 0;
    if (result > limit)
        result = limit;
    return result;
}

uint32_t sub_800B658C(RRJMemory *m, uint32_t actor, uint32_t other, int32_t value, int32_t divisor, int32_t factor)
{
    int32_t first = batch4_clamp_ratio(255, value, divisor, factor);
    int32_t second = batch4_clamp_ratio(150, value, divisor, factor);
    uint32_t state = rrj_read32(m, 0x8005B2F8u);
    uint32_t result = 0;

    FUNCTION_MARKER(0x800B658C, "RASHCDG.BIN");
    if (!other || ((rrj_u16(rrj_at(m, other + 172, 2)) >> 5) == 1u && (rrj_u16(rrj_at(m, other + 172, 2)) & 31u) < rrj_read32(m, state + 48)))
        result = sub_8001DD74(m, rrj_u16(rrj_at(m, actor + 172, 2)), second, 150, (uint32_t)first);
    if (!other || (r_u8(other + 572) & 0x20u))
    {
        uint32_t link = rrj_read32(m, actor + 856);
        uint32_t linked_actor;

        if (!link)
            return 0;
        linked_actor = rrj_read32(m, link + 852);
        result = r_u8(linked_actor + 572) & 0x40u;
        if (!result)
        {
            int eligible = 0;

            if (link && (r_u8(rrj_read32(m, actor + 852) + 572) & 0x10u) && rrj_read32(m, linked_actor + 604) < 2u)
                eligible = 1;
            if (other)
                eligible = 1;
            if (eligible)
            {
                uint32_t source = other ? rrj_read32(m, rrj_read32(m, other + 596) + 856) : actor;
                uint32_t index = rrj_u16(rrj_at(m, source + 172, 2)) + 1u + (rrj_read32(m, state + 48) >= 2u);

                return sub_8001DD74(m, index, second, 150, (uint32_t)first);
            }
        }
    }
    return result;
}

static uint32_t batch4_translate_actor(RRJMemory *m, uint32_t actor, const uint32_t delta[3], uint32_t mark_dirty)
{
    uint32_t tag = rrj_u16(rrj_at(m, actor + 172, 2));
    uint32_t type = tag >> 5;
    uint32_t linked = rrj_read32(m, actor + 856);
    int move_linked = type == 0 && linked && rrj_read32(m, actor + 1088);
    uint32_t i;

    for (i = 0; i < 3; ++i)
    {
        uint32_t value = delta[i];

        rrj_write32(m, actor + 184u + 4u * i, rrj_read32(m, actor + 184u + 4u * i) + value);
        if (move_linked)
            rrj_write32(m, linked + 184u + 4u * i, rrj_read32(m, linked + 184u + 4u * i) + value);
    }
    for (i = 0; i < 8; ++i)
    {
        uint32_t axis;

        for (axis = 0; axis < 3; ++axis)
        {
            uint32_t value = delta[axis];
            uint32_t offset = 196u + 12u * i + 4u * axis;

            rrj_write32(m, actor + offset, rrj_read32(m, actor + offset) + value);
            if (move_linked)
                rrj_write32(m, linked + offset, rrj_read32(m, linked + offset) + value);
        }
    }
    if (!mark_dirty)
        return 1;
    if (type == 0)
    {
        uint32_t result = rrj_read32(m, actor + 568) | 0x01800000u;

        rrj_write32(m, actor + 568, result);
        return result;
    }
    if (type == 1)
    {
        uint32_t result = rrj_read32(m, actor + 552) | 2u;

        rrj_write32(m, actor + 552, result);
        return result;
    }
    if (type == 2)
    {
        uint32_t result = rrj_read32(m, actor + 568) | 2u;

        rrj_write32(m, actor + 568, result);
        return result;
    }
    if (type == 4)
    {
        uint32_t result = rrj_read32(m, actor + 592) | 0x100u;

        rrj_write32(m, actor + 592, result);
        return result;
    }
    return 4;
}

uint32_t sub_800A8DF0(RRJMemory *m, uint32_t actor, uint32_t delta, uint32_t mark_dirty)
{
    uint32_t values[3];
    uint32_t i;

    FUNCTION_MARKER(0x800A8DF0, "RASHCDG.BIN");
    for (i = 0; i < 3; ++i)
        values[i] = rrj_read32(m, delta + 4u * i);
    return batch4_translate_actor(m, actor, values, mark_dirty);
}

static int16_t batch4_cross_component(int64_t value)
{
    int64_t shifted = value >= 0 ? value / 4096 : -(((-value) + 4095) / 4096);

    if (shifted > 32767)
        shifted = 32767;
    if (shifted < -32768)
        shifted = -32768;
    return (int16_t)shifted;
}

static void batch4_outer_product(RRJMemory *m, uint32_t left, uint32_t right, uint32_t output)
{
    int16_t left_value[3];
    int16_t right_value[3];
    int16_t cross[3];
    uint32_t i;

    for (i = 0; i < 3; ++i)
    {
        left_value[i] = (int16_t)rrj_u16(rrj_at(m, left + 2u * i, 2));
        right_value[i] = (int16_t)rrj_u16(rrj_at(m, right + 2u * i, 2));
    }
    cross[0] = batch4_cross_component((int64_t)left_value[1] * right_value[2] - (int64_t)left_value[2] * right_value[1]);
    cross[1] = batch4_cross_component((int64_t)left_value[2] * right_value[0] - (int64_t)left_value[0] * right_value[2]);
    cross[2] = batch4_cross_component((int64_t)left_value[0] * right_value[1] - (int64_t)left_value[1] * right_value[0]);
    for (i = 0; i < 3; ++i)
        rrj_put16(rrj_at(m, output + 2u * i, 2), (uint16_t)cross[i]);
}

uint32_t sub_80080B10(RRJMemory *m, uint32_t actor, uint32_t normal, uint32_t distance_out, uint32_t input, int32_t distance)
{
    uint32_t flags = rrj_read32(m, actor + 568);
    int16_t basis[3];
    int16_t vector[3];
    int16_t cross[3];
    uint32_t cross_address = actor + 814;
    uint32_t linked;
    int32_t first_dot;
    int32_t second_dot;
    int32_t angle;
    uint32_t i;

    FUNCTION_MARKER(0x80080B10, "RASHCDG.BIN");
    if (rrj_s32(rrj_read32(m, actor + 364)) < 0)
        flags |= 0x00400000u;
    else
        flags &= ~0x00400000u;
    rrj_write32(m, actor + 568, flags);
    if (distance <= 0x1FFFF)
    {
        flags = (flags & 0xF7FFFFCFu) | 0x08000000u;
        rrj_write32(m, actor + 568, flags);
        return 0;
    }
    for (i = 0; i < 3; ++i)
        rrj_put16(rrj_at(m, normal + 2u * i, 2), rrj_u16(rrj_at(m, input + 2u * i, 2)));
    if (flags & 0x600u)
    {
        rrj_write32(m, distance_out, (uint32_t)distance);
        (void)sub_8002EE50(m, (uint32_t)distance, normal, actor + 456);
        return 1;
    }
    for (i = 0; i < 3; ++i)
    {
        basis[i] = (int16_t)rrj_u16(rrj_at(m, actor + 522u + 2u * i, 2));
        vector[i] = (int16_t)rrj_u16(rrj_at(m, normal + 2u * i, 2));
    }
    cross[0] = batch4_cross_component((int64_t)basis[1] * vector[2] - (int64_t)basis[2] * vector[1]);
    cross[1] = batch4_cross_component((int64_t)basis[2] * vector[0] - (int64_t)basis[0] * vector[2]);
    cross[2] = batch4_cross_component((int64_t)basis[0] * vector[1] - (int64_t)basis[1] * vector[0]);
    for (i = 0; i < 3; ++i)
        rrj_put16(rrj_at(m, cross_address + 2u * i, 2), (uint16_t)cross[i]);
    if (!sub_8002E468(m, cross_address))
        for (i = 0; i < 3; ++i)
            rrj_put16(rrj_at(m, cross_address + 2u * i, 2), rrj_u16(rrj_at(m, actor + 432u + 2u * i, 2)));
    linked = rrj_read32(m, actor + 856);
    rrj_write32(m, actor + 576, (uint32_t)distance);
    rrj_write32(m, actor + 488, 0);
    if (linked)
        rrj_write32(m, linked + 488, 0);
    first_dot = rrj_s32(sub_8002E698(m, actor + 528, cross_address));
    second_dot = rrj_s32(sub_8002E698(m, actor + 528, actor + 450));
    angle = rrj_s32(sub_80020018(m, (uint32_t)first_dot, (uint32_t)second_dot));
    rrj_write32(m, actor + 676, (uint32_t)(((int64_t)25736 * angle) >> 8));
    rrj_write32(m, actor + 680, 0);
    if (flags & 0x100u)
    {
        rrj_write32(m, actor + 656, rrj_s32(rrj_read32(m, actor + 636)) > 0 ? 0x00020000u : 0xFFFE0000u);
        rrj_write32(m, actor + 568, flags & 0xFFFFFFCFu);
    }
    return 1;
}

uint32_t sub_800A3CBC(RRJMemory *m, uint32_t actor)
{
    static const int16_t identity[9] = {4096, 0, 0, 0, 4096, 0, 0, 0, 4096};
    uint32_t i;

    FUNCTION_MARKER(0x800A3CBC, "RASHCDG.BIN");
    batch4_outer_product(m, actor + 432, actor + 522, actor + 528);
    if (sub_8002E468(m, actor + 528))
    {
        batch4_outer_product(m, actor + 522, actor + 528, actor + 516);
        return actor + 516;
    }

    batch4_outer_product(m, actor + 522, actor + 444, actor + 516);
    if (!sub_8002E468(m, actor + 516))
    {
        for (i = 0; i < 9; ++i)
            rrj_put16(rrj_at(m, actor + 432u + 2u * i, 2), (uint16_t)identity[i]);
        for (i = 0; i < 9; ++i)
            rrj_put16(rrj_at(m, actor + 516u + 2u * i, 2), (uint16_t)identity[i]);
    }
    batch4_outer_product(m, actor + 516, actor + 522, actor + 528);
    return 4096;
}

uint32_t sub_800849D8(RRJMemory *m, uint32_t actor)
{
    uint32_t flags;
    int32_t first_random;
    int32_t second_random;
    int32_t velocity;
    int32_t speed;
    int32_t bound;
    int32_t target;
    int32_t result;

    FUNCTION_MARKER(0x800849D8, "RASHCDG.BIN");
    flags = rrj_read32(m, actor + 568);
    if (flags & 0x00080000u)
        return 0x8000u;

    rrj_write32(m, actor + 724, 0);
    rrj_write32(m, actor + 728, 0x8000u);
    first_random = (int32_t)(sub_8001FC58(m) % 0xB2B8u) - 22876;
    second_random = (int32_t)(sub_8001FC58(m) % 0xB2B8u) - 22876;
    rrj_write32(m, actor + 656, (uint32_t)first_random << 1);
    rrj_write32(m, actor + 640, (uint32_t)second_random << 1);
    velocity = (int32_t)sub_8001FC90(rrj_s32(rrj_read32(m, actor + 488)), (int32_t)(sub_8001FC58(m) % 0x6667u) + 0xCCCC);
    rrj_write32(m, actor + 488, (uint32_t)velocity);

    speed = rrj_s32(rrj_read32(m, actor + 480));
    bound = speed > 0x9FFFF ? 0xA0000 : speed / 3;
    target = 0x18000;
    if (velocity < 0)
    {
        target = -target;
        bound = -bound;
    }
    if (velocity < target)
        velocity = target;
    if (velocity > bound)
        velocity = bound;
    rrj_write32(m, actor + 488, (uint32_t)velocity);

    result = (int16_t)rrj_u16(rrj_at(m, actor + 796, 2)) * (int16_t)rrj_u16(rrj_at(m, actor + 454, 2)) - (int16_t)rrj_u16(rrj_at(m, actor + 800, 2)) * (int16_t)rrj_u16(rrj_at(m, actor + 450, 2));
    if ((result < 0 && velocity > 0) || (result > 0 && velocity < 0))
    {
        velocity = rrj_s32(0u - (uint32_t)velocity);
        rrj_write32(m, actor + 488, (uint32_t)velocity);
        return (uint32_t)velocity;
    }
    return (uint32_t)result;
}

uint32_t sub_800AD9BC(RRJMemory *m, uint32_t actor, int32_t value, int32_t direction, int32_t active, int32_t blend)
{
    int32_t doubled = rrj_s32(rrj_read32(m, actor + 308) << 1);
    int32_t ratio;
    int32_t half = value >> 1;
    int32_t angle;
    int32_t magnitude;
    int32_t result;
    uint32_t flags;

    FUNCTION_MARKER(0x800AD9BC, "RASHCDG.BIN");
    rrj_write32(m, actor + 768, 0u - (uint32_t)value);
    flags = rrj_read32(m, actor + 564) | 0x00218000u | ((0u - (uint32_t)active) & 0x00020000u);
    rrj_write32(m, actor + 564, flags);

    if (value > 0)
    {
        ratio = (int32_t)sub_80010028((uint32_t)value, doubled > 0 ? (uint32_t)doubled : 0u - (uint32_t)doubled);
    }
    else if (doubled > 0)
    {
        ratio = rrj_s32(0u - sub_80010028(0u - (uint32_t)value, (uint32_t)doubled));
    }
    else
    {
        ratio = (int32_t)sub_80010028(0u - (uint32_t)value, 0u - (uint32_t)doubled);
    }
    if (ratio > 64880)
    {
        ratio = 64880;
        half = (int32_t)sub_8001FC90(64880, rrj_s32(rrj_read32(m, actor + 308)));
    }
    angle = rrj_s32(sub_8001FF3C(m, (uint32_t)ratio));
    magnitude = (int32_t)(((int64_t)25736 * angle) >> 8);

    if (blend <= 0x7FFF)
    {
        int32_t scale = (int32_t)sub_8001FC90(blend, 0x20000);
        int32_t table_angle = (int32_t)sub_8001FC90(magnitude, scale);
        uint32_t table_offset = ((163u * (uint32_t)table_angle) >> 12) & 0x3FFCu;
        int32_t sine = (int16_t)rrj_u16(rrj_at(m, 0x8005624Cu + table_offset, 2));

        result = (int32_t)sub_8001FC90(rrj_s32(rrj_read32(m, actor + 308)), sine << 4);
        rrj_write32(m, actor + 772, 0u - (uint32_t)result);
        result = table_angle;
    }
    else if (blend < 0x10000 && (!active || !direction))
    {
        int32_t scale = (int32_t)sub_8001FC90(0x10000 - blend, 0x20000);

        result = (int32_t)sub_8001FC90(value - half, scale) - value;
        rrj_write32(m, actor + 772, (uint32_t)result);
        result = active ? magnitude : (int32_t)sub_8001FC90(magnitude, scale);
    }
    else
    {
        rrj_write32(m, actor + 772, 0u - (uint32_t)value);
        if (active)
        {
            (void)sub_80084BE8(m, actor, 0);
            result = magnitude;
            direction = 1;
            rrj_write32(m, actor + 564, rrj_read32(m, actor + 564) & 0xFFFEFFFFu);
        }
        else
        {
            result = 0;
        }
    }

    result = rrj_s32(((0u - (uint32_t)direction) & ((uint32_t)result << 1)) - (uint32_t)result);
    if (blend <= 0x7FFF && result > 0 && result < rrj_s32(rrj_read32(m, actor + 616)))
        result = rrj_s32(rrj_read32(m, actor + 616));
    rrj_write32(m, actor + 616, (uint32_t)result);
    return (uint32_t)result;
}

static uint32_t sub_800B16FC(RRJMemory *m, uint32_t actor, uint32_t value_pointer, uint32_t state)
{
    uint32_t tag = rrj_u16(rrj_at(m, actor + 172, 2));
    int32_t lateral = rrj_s32(rrj_read32(m, actor + 460));
    uint32_t quotient = sub_80010028(lateral > 0 ? (uint32_t)lateral : 0u - (uint32_t)lateral, 0x80000u);
    int32_t signed_quotient = lateral > 0 ? (int32_t)quotient : rrj_s32(0u - quotient);
    int32_t remainder = rrj_s32(0x30000u - (uint32_t)signed_quotient);
    int32_t adjusted = signed_quotient < 0 ? 0 : signed_quotient;
    int32_t dot;
    int32_t scale;
    uint32_t linked;
    uint32_t result;

    FUNCTION_MARKER(0x800B16FC, "RASHCDG.BIN");
    if (tag < rrj_read32(m, state + 48))
    {
        uint32_t record = 0x800CD898u + 1132u * tag;

        if (remainder < 0)
            adjusted = rrj_s32((uint32_t)adjusted + (uint32_t)remainder);
        rrj_write32(m, record + 740, (uint32_t)adjusted);
    }

    dot = rrj_s32(sub_8002E698(m, actor + 450, actor + 444));
    scale = dot < 0 ? 6553 : 58982;
    rrj_write32(m, actor + 576, (uint32_t)sub_8001FC90(scale, rrj_s32(rrj_read32(m, value_pointer))));
    linked = rrj_read32(m, actor + 852);
    rrj_write32(m, actor + 568, rrj_read32(m, actor + 568) & 0xFFFFFBFFu);
    rrj_write32(m, linked + 552, rrj_read32(m, linked + 552) & 0xFFFFDFFFu);
    if (rrj_s32(rrj_read32(m, actor + 768)) > 0x30000)
    {
        (void)sub_80017BA0(m, rrj_s32(rrj_read32(m, actor + 184)), rrj_s32(rrj_read32(m, actor + 192)), 54, 0);
        if (tag < rrj_read32(m, rrj_read32(m, 0x8005B2F8u) + 48) && rrj_read32(m, linked + 604) < 2u && !rrj_read32(m, 0x8005B220u))
            (void)sub_800B658C(m, actor, 0, rrj_s32(rrj_read32(m, value_pointer)), 1464860, 1);
    }
    result = rrj_read32(m, actor + 568) | 0x0C000000u;
    rrj_write32(m, actor + 568, result);
    return result;
}

uint32_t sub_800B16F4(RRJMemory *m, uint32_t actor, uint32_t value_pointer)
{
    FUNCTION_MARKER(0x800B16F4, "RASHCDG.BIN");
    return sub_800B16FC(m, actor, value_pointer, rrj_read32(m, 0x8005B2F8u));
}

static void batch4_rotate_short_vectors(RRJMemory *m, int16_t left[3], int16_t right[3], int32_t angle)
{
    uint32_t table = 0x8005624Cu + 4u * ((uint32_t)angle & 4095u);
    int32_t sine = (int16_t)rrj_u16(rrj_at(m, table, 2)) * 16;
    int32_t cosine = (int16_t)rrj_u16(rrj_at(m, table + 2, 2)) * 16;
    int16_t saved_left[3];
    int16_t saved_right[3];
    uint32_t i;

    for (i = 0; i < 3; ++i)
    {
        uint32_t left_value = (uint32_t)(int32_t)left[i] << 4;
        uint32_t right_value = (uint32_t)(int32_t)right[i] << 4;
        uint32_t first = (uint32_t)sub_8001FC90(cosine, rrj_s32(left_value));
        uint32_t second = (uint32_t)sub_8001FC90(-sine, rrj_s32(right_value));
        uint32_t third = (uint32_t)sub_8001FC90(sine, rrj_s32(left_value));
        uint32_t fourth = (uint32_t)sub_8001FC90(cosine, rrj_s32(right_value));

        saved_left[i] = (int16_t)(rrj_s32(first + second) >> 4);
        saved_right[i] = (int16_t)(rrj_s32(third + fourth) >> 4);
    }
    for (i = 0; i < 3; ++i)
    {
        left[i] = saved_left[i];
        right[i] = saved_right[i];
    }
}

static int32_t batch4_signed_reciprocal(uint32_t scaled)
{
    uint32_t magnitude = rrj_s32(scaled) < 0 ? 0u - scaled : scaled;
    uint32_t divisor = (magnitude >> 1) + (uint32_t)(rrj_s32(magnitude - 2u) >> 31);
    uint32_t result = 0x80000000u / divisor;

    return rrj_s32(scaled) < 0 ? rrj_s32(0u - result) : rrj_s32(result);
}

static int32_t batch4_quaternion_reciprocal(uint32_t value)
{
    return batch4_signed_reciprocal(value << 3);
}

static int16_t batch4_quaternion_component(int32_t value, int32_t scale)
{
    uint32_t product = (uint32_t)((int64_t)value * scale);

    return (int16_t)(rrj_s32(product) >> 14);
}

static void batch4_matrix_to_quaternion(RRJMemory *m, const int16_t matrix[9], uint32_t output)
{
    uint32_t axes[3];
    int32_t diagonal[3];
    int32_t trace;
    uint32_t root;
    int32_t reciprocal;
    uint32_t i;

    for (i = 0; i < 3; ++i)
    {
        axes[i] = rrj_read32(m, 0x8005B61Cu + 4u * i);
        diagonal[i] = matrix[4u * i];
    }
    trace = diagonal[0] + diagonal[1] + diagonal[2];
    if (rrj_s32((uint32_t)trace << 4) > 0)
    {
        root = sub_8004CF74(m, ((uint32_t)trace << 4) + 0x10000u);
        rrj_put16(rrj_at(m, output + 6, 2), (uint16_t)(root >> 1));
        reciprocal = batch4_quaternion_reciprocal(root);
        rrj_put16(rrj_at(m, output, 2), (uint16_t)batch4_quaternion_component(matrix[5] - matrix[7], reciprocal));
        rrj_put16(rrj_at(m, output + 2, 2), (uint16_t)batch4_quaternion_component(matrix[6] - matrix[2], reciprocal));
        rrj_put16(rrj_at(m, output + 4, 2), (uint16_t)batch4_quaternion_component(matrix[1] - matrix[3], reciprocal));
        return;
    }
    {
        uint32_t largest = diagonal[0] < diagonal[1] ? 1u : 0u;
        uint32_t second;
        uint32_t third;

        if (diagonal[largest] < diagonal[2])
            largest = 2;
        second = axes[largest];
        third = axes[second];
        root = sub_8004CF74(m, ((uint32_t)(diagonal[largest] - diagonal[second] - diagonal[third]) << 4) + 0x10000u);
        rrj_put16(rrj_at(m, output + 2u * largest, 2), (uint16_t)(root >> 1));
        reciprocal = batch4_quaternion_reciprocal(root);
        rrj_put16(rrj_at(m, output + 6, 2), (uint16_t)batch4_quaternion_component(matrix[3u * second + third] - matrix[3u * third + second], reciprocal));
        rrj_put16(rrj_at(m, output + 2u * second, 2), (uint16_t)batch4_quaternion_component(matrix[3u * largest + second] + matrix[3u * second + largest], reciprocal));
        rrj_put16(rrj_at(m, output + 2u * third, 2), (uint16_t)batch4_quaternion_component(matrix[3u * largest + third] + matrix[3u * third + largest], reciprocal));
    }
}

uint32_t sub_800B3838(RRJMemory *m, uint32_t actor, uint32_t first_axis, uint32_t second_axis, uint32_t third_axis, int32_t value)
{
    int16_t matrix[9];
    int32_t direction = 1024;
    int32_t dot;
    uint32_t i;
    uint32_t sum = 0;
    uint32_t flags;
    int32_t response;
    int32_t combined;

    FUNCTION_MARKER(0x800B3838, "RASHCDG.BIN");
    dot = rrj_s32(sub_8002E698(m, actor + 444, first_axis));
    for (i = 0; i < 3; ++i)
    {
        int16_t first = (int16_t)rrj_u16(rrj_at(m, first_axis + 2u * i, 2));
        int16_t second = (int16_t)rrj_u16(rrj_at(m, second_axis + 2u * i, 2));
        int16_t third = (int16_t)rrj_u16(rrj_at(m, third_axis + 2u * i, 2));

        matrix[i] = dot <= 0 ? (int16_t)(0u - (uint16_t)second) : second;
        matrix[3u + i] = third;
        matrix[6u + i] = dot <= 0 ? (int16_t)(0u - (uint16_t)first) : first;
    }
    if (dot > 0)
        direction = -1024;
    batch4_rotate_short_vectors(m, matrix, matrix + 6, (int32_t)(sub_8001FC58(m) % 0x2AAu) - 341);
    flags = rrj_read32(m, actor + 592) | 0x200u;
    rrj_write32(m, actor + 592, flags);
    batch4_rotate_short_vectors(m, matrix + 3, matrix + 6, -direction);
    batch4_matrix_to_quaternion(m, matrix, actor + 572);

    for (i = 0; i < 4; ++i)
    {
        int32_t left = (int16_t)rrj_u16(rrj_at(m, actor + 572u + 2u * i, 2));
        int32_t right = (int16_t)rrj_u16(rrj_at(m, actor + 560u + 2u * i, 2));

        sum += (uint32_t)(left * right);
    }
    flags &= ~0x40u;
    rrj_write32(m, actor + 592, flags);
    if (rrj_s32((uint32_t)(rrj_s32(sum) >> 14) << 2) < 0)
        rrj_write32(m, actor + 592, flags | 0x40u);

    response = (int32_t)sub_8001FC90(6443, rrj_s32((uint32_t)value - 65918u));
    rrj_write32(m, actor + 484, (uint32_t)response);
    rrj_write32(m, actor + 488, (uint32_t)response << 1);
    combined = rrj_s32((uint32_t)response + rrj_read32(m, actor + 556));
    rrj_write32(m, actor + 484, (uint32_t)combined);
    if (combined > 49152)
    {
        rrj_write32(m, actor + 484, 0x10000u);
        return 0x10000u;
    }
    return 0;
}

uint32_t sub_800A9408(RRJMemory *m, uint32_t actor, int32_t amount, int32_t ratio, int32_t divisor, int32_t mode)
{
    uint32_t flags = rrj_read32(m, actor + 568);
    uint32_t linked = rrj_read32(m, actor + 852);
    int active = (flags & 0x22Cu) != 0;
    int32_t clamped;
    int32_t scaled;
    uint32_t options = mode ? 10u : 266u;

    FUNCTION_MARKER(0x800A9408, "RASHCDG.BIN");
    if (active && divisor != 0x10000)
    {
        uint32_t ratio_magnitude = ratio > 0 ? (uint32_t)ratio : 0u - (uint32_t)ratio;
        uint32_t divisor_magnitude = divisor > 0 ? (uint32_t)divisor : 0u - (uint32_t)divisor;
        uint32_t quotient = sub_80010028(ratio_magnitude, divisor_magnitude);

        ratio = (ratio > 0) == (divisor > 0) ? (int32_t)quotient : rrj_s32(0u - quotient);
    }
    scaled = rrj_s32(5u * (uint32_t)amount) / rrj_s32(rrj_read32(m, rrj_read32(m, actor + 556) + 224));
    clamped = scaled < 0 ? 0 : scaled;
    if (clamped > 4)
        clamped = 4;

    if (rrj_read32(m, linked + 604) == 1)
    {
        uint32_t first_tag = rrj_u16(rrj_at(m, linked + 544, 2));
        uint32_t second_tag = rrj_u16(rrj_at(m, linked + 610, 2));
        int first_allowed = first_tag - 26u >= 12u && rrj_u16(rrj_at(m, 0x800541D4u + 8u * first_tag + 2u, 2)) != 3u;
        int second_allowed = !rrj_u16(rrj_at(m, linked + 608, 2)) || (second_tag - 26u >= 12u && rrj_u16(rrj_at(m, 0x800541D4u + 8u * second_tag + 2u, 2)) != 3u);

        if (first_allowed && second_allowed)
        {
            uint32_t state;

            if (mode == 1)
                state = 27;
            else if (mode == 0 || mode == 2)
                state = 26;
            else if (mode == 3)
                state = 28;
            else
                state = 21;
            (void)sub_800C4550(m, state, linked, options);
        }
    }
    if (active)
    {
        uint32_t controller = rrj_read32(m, actor + 1084);
        uint32_t current = r_u8(controller + 37);
        uint32_t product = (uint32_t)((int64_t)clamped * ratio);
        int32_t next = (int32_t)current - (rrj_s32(product) >> 16);

        if (next > 0)
        {
            uint8_t controller_flags = r_u8(controller + 68);

            if (((current & 0x80u) && next < 128) || (current >= 0x40u && next < 64))
                controller_flags |= 0x40u;
            w_u8(controller + 68, controller_flags);
            w_u8(rrj_read32(m, actor + 1084) + 37, (uint8_t)next);
        }
        else
        {
            w_u8(controller + 37, 0);
        }
    }
    return (uint32_t)clamped;
}

uint32_t sub_80084564(RRJMemory *m, uint32_t first, uint32_t second, uint32_t magnitude_pointer, uint32_t output, int32_t response, int32_t minimum, uint32_t zero_vertical)
{
    int32_t dot;
    int32_t absolute_vertical;
    int shallow;
    int32_t scale;
    uint32_t i;

    FUNCTION_MARKER(0x80084564, "RASHCDG.BIN");
    dot = rrj_s32(sub_8002E698(m, second, first));
    if (dot >= 0)
        return 0xFFFFFFFFu;
    absolute_vertical = (int16_t)rrj_u16(rrj_at(m, first + 2, 2));
    if (absolute_vertical < 0)
        absolute_vertical = -absolute_vertical;
    shallow = absolute_vertical < 2048;

    if (shallow && (int16_t)rrj_u16(rrj_at(m, second + 2, 2)) < 0)
    {
        int32_t x = -(int16_t)rrj_u16(rrj_at(m, first + 4, 2)) * 16;
        int32_t z = (int16_t)rrj_u16(rrj_at(m, first, 2)) * 16;
        uint32_t norm = (uint32_t)sub_8001FC90(x, x) + (uint32_t)sub_8001FC90(z, z);
        uint32_t root = sub_8004CF74(m, norm);
        int32_t reciprocal = batch4_signed_reciprocal(root << 2);
        int32_t normal_x = (int32_t)sub_8001FC90(x, reciprocal);
        int32_t normal_z = (int32_t)sub_8001FC90(z, reciprocal);
        int32_t vector_x = (int16_t)rrj_u16(rrj_at(m, second, 2)) * 16;
        int32_t vector_z = (int16_t)rrj_u16(rrj_at(m, second + 4, 2)) * 16;
        int32_t projection = rrj_s32((uint32_t)sub_8001FC90(normal_x, vector_x) + (uint32_t)sub_8001FC90(normal_z, vector_z));
        int32_t doubled = rrj_s32((uint32_t)projection << 1);
        int32_t value;

        value = rrj_s32((uint32_t)sub_8001FC90(doubled, normal_x) - (uint32_t)vector_x);
        rrj_put16(rrj_at(m, second, 2), (uint16_t)(int16_t)(value >> 4));
        if (zero_vertical)
        {
            rrj_put16(rrj_at(m, second + 2, 2), 0);
        }
        else
        {
            value = rrj_s32(46333u - ((uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, second + 2, 2)) << 4));
            rrj_put16(rrj_at(m, second + 2, 2), (uint16_t)(int16_t)(value >> 5));
        }
        value = rrj_s32((uint32_t)sub_8001FC90(doubled, normal_z) - (uint32_t)vector_z);
        rrj_put16(rrj_at(m, second + 4, 2), (uint16_t)(int16_t)(value >> 4));
        minimum /= 2;
    }
    else
    {
        int32_t distance = rrj_s32(rrj_read32(m, magnitude_pointer));
        int32_t incidence = rrj_s32(0u - (uint32_t)dot);
        int32_t angle;

        if (!minimum)
        {
            if (distance <= 0x1FFFF)
                response = 0;
            else if (distance <= 0x9FFFF)
            {
                int32_t ramp = (int32_t)sub_8001FC90(distance - 0x20000, 0x2000);

                response = (int32_t)sub_8001FC90(ramp, rrj_s32((uint32_t)response << 16)) >> 16;
            }
        }
        angle = rrj_s32(sub_8001FF3C(m, (uint32_t)incidence));
        scale = incidence;
        if (response)
        {
            uint32_t product = (uint32_t)((int64_t)response * angle);
            int32_t tangent_angle = rrj_s32(product) / 10;
            int32_t tangent;
            int32_t cosine;

            if (tangent_angle < 56)
                tangent_angle = 56;
            if (tangent_angle > 512)
                tangent_angle = 512;
            tangent = rrj_s32(sub_8001FEB4(m, tangent_angle));
            cosine = (int16_t)rrj_u16(rrj_at(m, 0x8005624Eu + 4u * ((uint32_t)angle & 4095u), 2)) * 16;
            scale = rrj_s32((uint32_t)incidence + (uint32_t)sub_8001FC90(cosine, tangent));
        }
        if (incidence > 64552 && scale < 72089)
            scale = 72089;
        (void)sub_8002EA20(m, second, first, scale, output);
        for (i = 0; i < 3; ++i)
            rrj_put16(rrj_at(m, second + 2u * i, 2), (uint16_t)(int16_t)(rrj_s32(rrj_read32(m, output + 4u * i)) >> 4));
    }

    scale = (int32_t)sub_8001FC90(shallow ? 6553 : 52428, rrj_s32(rrj_read32(m, magnitude_pointer)));
    rrj_write32(m, magnitude_pointer, (uint32_t)scale);
    if (minimum < scale)
        minimum = scale;
    rrj_write32(m, magnitude_pointer, (uint32_t)minimum);
    (void)sub_8002E468(m, second);
    (void)sub_8002EE50(m, (uint32_t)minimum, second, output);
    return (uint32_t)shallow;
}

static int32_t batch4_dot_difference_axis(RRJMemory *m, const int32_t difference[3], uint32_t axis)
{
    uint32_t sum = 0;
    uint32_t i;

    for (i = 0; i < 3; ++i)
    {
        int32_t component = (int16_t)rrj_u16(rrj_at(m, axis + 2u * i, 2)) * 16;

        sum += (uint32_t)sub_8001FC90(difference[i], component);
    }
    return rrj_s32(sum);
}

static void batch4_copy_axis(RRJMemory *m, uint32_t axis, uint32_t output, int negate)
{
    uint32_t i;

    for (i = 0; i < 3; ++i)
    {
        uint16_t value = rrj_u16(rrj_at(m, axis + 2u * i, 2));

        if (negate)
            value = (uint16_t)(0u - value);
        rrj_put16(rrj_at(m, output + 2u * i, 2), value);
    }
}

uint32_t sub_800B0510(RRJMemory *m, uint32_t actor, uint32_t other, uint32_t output)
{
    int32_t position_difference[3];
    int32_t velocity_difference[3];
    uint32_t dot = 0;
    int32_t side;
    uint32_t i;

    FUNCTION_MARKER(0x800B0510, "RASHCDG.BIN");
    for (i = 0; i < 3; ++i)
        position_difference[i] = rrj_s32(rrj_read32(m, other + 184u + 4u * i) - rrj_read32(m, actor + 184u + 4u * i));
    (void)sub_8002EE50(m, rrj_read32(m, other + 480), other + 450, other + 456);
    for (i = 0; i < 3; ++i)
    {
        velocity_difference[i] = rrj_s32(rrj_read32(m, other + 456u + 4u * i) - rrj_read32(m, actor + 456u + 4u * i));
        dot += (uint32_t)sub_8001FC90(position_difference[i], velocity_difference[i]);
    }
    if (rrj_s32(dot) > 0)
        return 0;

    side = batch4_dot_difference_axis(m, position_difference, actor + 444);
    if (side >= 0)
    {
        batch4_copy_axis(m, actor + 444, output, 0);
        return 1;
    }
    side = batch4_dot_difference_axis(m, position_difference, actor + 432);
    if (side >= 0)
    {
        batch4_copy_axis(m, actor + 432, output, 0);
        return 1;
    }

    for (i = 0; i < 3; ++i)
        position_difference[i] = rrj_s32(rrj_read32(m, other + 184u + 4u * i) - rrj_read32(m, actor + 196u + 4u * i));
    side = batch4_dot_difference_axis(m, position_difference, actor + 432);
    if (side <= 0)
    {
        batch4_copy_axis(m, actor + 432, output, 1);
        return 1;
    }
    side = batch4_dot_difference_axis(m, position_difference, actor + 444);
    if (side <= 0)
    {
        batch4_copy_axis(m, actor + 444, output, 1);
        return 1;
    }
    side = batch4_dot_difference_axis(m, position_difference, actor + 438);
    batch4_copy_axis(m, actor + 438, output, side < 0);
    return 1;
}

uint32_t sub_80017B30(RRJMemory *m, int32_t value)
{
    FUNCTION_MARKER(0x80017B30, "SLUS_010.53");
    if (value < 0)
        value = 0;
    if (value > 51)
        value = 51;
    return r_u8(0x800525C0u + (uint32_t)value);
}

static uint32_t batch4_find_penetrating_point(RRJMemory *m, uint32_t points, uint32_t normal, uint32_t origin, uint32_t *distance)
{
    uint32_t selected = 8;
    int32_t minimum = 0;
    uint32_t i;

    for (i = 0; i < 8; ++i)
    {
        int32_t value = rrj_s32(sub_800B6AAC(m, points + 12u * i, normal, origin));

        if (value < minimum)
        {
            minimum = value;
            selected = i;
        }
    }
    if (selected != 8)
        *distance = 0u - (uint32_t)minimum;
    return selected;
}

uint32_t sub_800B3344(RRJMemory *m, uint32_t actor, uint32_t origin, uint32_t normal, uint32_t mode)
{
    uint32_t distance = 0;
    uint32_t proceed = 1;
    uint32_t flags;

    FUNCTION_MARKER(0x800B3344, "RASHCDG.BIN");
    if (origin && batch4_find_penetrating_point(m, actor + 196, normal, origin, &distance) >= 8)
        return 0;
    if (mode)
    {
        uint32_t translation[3];

        (void)rrj_scale_short_local(m, distance, normal, translation);
        (void)batch4_translate_actor(m, actor, translation, 1);
    }
    if (mode == 1)
    {
        flags = rrj_read32(m, actor + 592);
        if (rrj_s32(rrj_read32(m, actor + 484)) <= 0x7FFF || (flags & 8u))
        {
            int32_t projection = rrj_s32(sub_8002E698(m, actor + 450, normal));
            uint32_t i;

            if (projection < -58982)
                projection = -58982;
            if (flags & 0x10u)
            {
                rrj_write32(m, actor + 592, flags & 0xFFFFFFF5u);
                (void)sub_800A3CBC(m, actor);
                rrj_write32(m, actor + 484, 0x30000u);
            }
            else
            {
                if (!(flags & 8u))
                {
                    int32_t velocity = rrj_s32(rrj_read32(m, actor + 488));

                    if (rrj_s32(sub_8002E698(m, actor + 566, actor + 522)) < 0)
                        flags |= 0x40u;
                    if (rrj_read32(m, actor + 540))
                    {
                        uint32_t magnitude = velocity < 0 ? 0u - (uint32_t)velocity : (uint32_t)velocity;
                        int32_t response = rrj_s32(magnitude) / 4;

                        if (response < 0x20000)
                            response = 0x20000;
                        if (rrj_s32(rrj_read32(m, actor + 540)) > 0)
                            response = -response;
                        rrj_write32(m, actor + 544, (uint32_t)response);
                        if (velocity <= 0)
                        {
                            if (velocity > -327680)
                                velocity = -327680;
                        }
                        else if (velocity < 327680)
                        {
                            velocity = 327680;
                        }
                        rrj_write32(m, actor + 488, (uint32_t)velocity);
                    }
                    else
                    {
                        rrj_write32(m, actor + 544, 0);
                    }
                    flags |= 8u;
                    rrj_write32(m, actor + 592, flags);
                }
                projection /= 2;
            }
            (void)sub_8002EA20(m, actor + 450, normal, -projection, actor + 456);
            for (i = 0; i < 3; ++i)
                rrj_put16(rrj_at(m, actor + 450u + 2u * i, 2), (uint16_t)(int16_t)(rrj_s32(rrj_read32(m, actor + 456u + 4u * i)) >> 4));
            (void)sub_8002E468(m, actor + 450);
            (void)sub_8002EE50(m, rrj_read32(m, actor + 480), actor + 450, actor + 456);
            proceed = 0;
        }
        else
        {
            rrj_write32(m, actor + 484, 0);
        }
    }
    if (proceed)
    {
        int32_t contact = (int32_t)sub_80084564(m, normal, actor + 450, actor + 480, actor + 456, 5, mode == 1 ? 0 : 393216, 0);

        if (contact >= 0)
        {
            int32_t first_random;
            int32_t second_random;
            int32_t velocity;
            int32_t speed;
            int32_t bound;
            int32_t target;
            int32_t cross;

            rrj_put16(rrj_at(m, actor + 578, 2), 0x8000u);
            first_random = (int32_t)(sub_8001FC58(m) % 0xB2B8u) - 22876;
            second_random = (int32_t)(sub_8001FC58(m) % 0xB2B8u) - 22876;
            rrj_write32(m, actor + 544, (uint32_t)first_random << 1);
            rrj_write32(m, actor + 552, (uint32_t)second_random << 1);
            velocity = (int32_t)sub_8001FC90(rrj_s32(rrj_read32(m, actor + 488)), (int32_t)(sub_8001FC58(m) % 0x2667u) + 0xCCCC);
            if (velocity > 655360)
                velocity = 655360;
            rrj_write32(m, actor + 488, (uint32_t)velocity);
            speed = rrj_s32(rrj_read32(m, actor + 480));
            bound = speed > 0x9FFFF ? 0xA0000 : speed / 3;
            target = 0x18000;
            if (velocity < 0)
            {
                target = -target;
                bound = -bound;
            }
            if (velocity < target)
                velocity = target;
            if (velocity > bound)
                velocity = bound;
            rrj_write32(m, actor + 488, (uint32_t)velocity);
            cross = (int16_t)rrj_u16(rrj_at(m, actor + 560, 2)) * (int16_t)rrj_u16(rrj_at(m, actor + 454, 2)) - (int16_t)rrj_u16(rrj_at(m, actor + 564, 2)) * (int16_t)rrj_u16(rrj_at(m, actor + 450, 2));
            if ((cross < 0 && velocity > 0) || (cross > 0 && velocity < 0))
                rrj_write32(m, actor + 488, 0u - (uint32_t)velocity);
        }
        (void)sub_80017BA0(m, rrj_s32(rrj_read32(m, actor + 184)), rrj_s32(rrj_read32(m, actor + 192)), sub_80017B30(m, rrj_s32(rrj_read32(m, actor + 180))) + 14, 0);
    }
    return 1;
}

uint32_t sub_800B2F94(RRJMemory *m, uint32_t actor, uint32_t other, uint32_t normal)
{
    int32_t other_speed;
    uint32_t result;

    FUNCTION_MARKER(0x800B2F94, "RASHCDG.BIN");
    if (rrj_read32(m, actor + 592) & 2u)
        return sub_800B3344(m, actor, 0, normal, 0);

    other_speed = rrj_s32(rrj_read32(m, other + 480));
    rrj_write32(m, actor + 480, (uint32_t)sub_8001FC90(72089, other_speed));
    if (other_speed <= 327680)
    {
        uint32_t i;

        for (i = 0; i < 3; ++i)
            rrj_put16(rrj_at(m, actor + 450u + 2u * i, 2), rrj_u16(rrj_at(m, other + 450u + 2u * i, 2)));
        (void)sub_8002EE50(m, rrj_read32(m, actor + 480), actor + 450, actor + 456);
    }
    else
    {
        int32_t perturbation = (int16_t)((sub_8001FC58(m) % 0x2CAu) - 357u);
        int32_t other_x = (int16_t)rrj_u16(rrj_at(m, other + 450, 2));
        int32_t other_y = (int16_t)rrj_u16(rrj_at(m, other + 452, 2));
        int32_t other_z = (int16_t)rrj_u16(rrj_at(m, other + 454, 2));
        int32_t angle;
        uint32_t kind;
        uint32_t angle_index;
        int32_t cosine;
        uint32_t magnitude = 406454u;
        int32_t velocity;
        int32_t direction;

        rrj_put16(rrj_at(m, actor + 450, 2), (uint16_t)(other_x + ((perturbation * other_z) >> 12)));
        rrj_put16(rrj_at(m, actor + 452, 2), (uint16_t)other_y);
        rrj_put16(rrj_at(m, actor + 454, 2), (uint16_t)(other_z - ((perturbation * other_x) >> 12)));
        angle = rrj_s32(sub_8001FF3C(m, (uint32_t)(other_y * 16)));
        kind = (rrj_u16(rrj_at(m, rrj_read32(m, actor) + 14, 2)) & 0xF80u) >> 7;
        angle_index = (uint32_t)(1479 - angle + ((kind == 2 || kind == 5) ? 57 : 0) - ((kind == 1 || kind == 4) ? 57 : 0)) & 4095u;
        cosine = (int16_t)rrj_u16(rrj_at(m, 0x8005624Eu + 4u * angle_index, 2));
        if (kind == 2 || kind == 5)
            magnitude += 0x15439u;
        if (kind == 1 || kind == 4)
            magnitude += 0xFFFE3A5Eu;
        (void)sub_8007E868(m, actor + 450, rrj_s32(rrj_read32(m, actor + 480)), cosine, rrj_s32(magnitude));

        velocity = other_speed / 3;
        if (velocity > 655360)
            velocity = 655360;
        rrj_write32(m, actor + 488, (uint32_t)velocity);
        direction = (int16_t)((sub_8001FC58(m) % 0x2CAu) - 357u);
        rrj_put16(rrj_at(m, actor + 560, 2), (uint16_t)((int16_t)rrj_u16(rrj_at(m, actor + 454, 2)) + ((direction * (int16_t)rrj_u16(rrj_at(m, actor + 450, 2))) >> 12)));
        rrj_put16(rrj_at(m, actor + 562, 2), (uint16_t)((sub_8001FC58(m) % 0x2CAu) - 357u));
        rrj_put16(rrj_at(m, actor + 564, 2), (uint16_t)(((direction * (int16_t)rrj_u16(rrj_at(m, actor + 454, 2))) >> 12) - (int16_t)rrj_u16(rrj_at(m, actor + 450, 2))));
        rrj_put16(rrj_at(m, actor + 578, 2), 0);
        (void)sub_8002E468(m, actor + 560);
        rrj_write32(m, actor + 592, rrj_read32(m, actor + 592) | 3u);
    }

    result = sub_80017B30(m, rrj_s32(rrj_read32(m, actor + 180)));
    (void)sub_80017BA0(m, rrj_s32(rrj_read32(m, actor + 184)), rrj_s32(rrj_read32(m, actor + 192)), result, 0);
    return 1;
}

static int32_t batch4_floor_shift(int64_t value, uint32_t shift)
{
    int64_t divisor = INT64_C(1) << shift;

    return (int32_t)(value / divisor - (value < 0 && value % divisor != 0));
}

static int32_t batch4_abs32(int32_t value)
{
    return value < 0 ? rrj_s32(0u - (uint32_t)value) : value;
}

static void batch4_copy_short(RRJMemory *m, uint32_t output, uint32_t input)
{
    uint32_t i;

    for (i = 0; i < 3; ++i)
        rrj_put16(rrj_at(m, output + 2u * i, 2), rrj_u16(rrj_at(m, input + 2u * i, 2)));
}

static uint32_t batch4_leading_sign(uint32_t value)
{
    uint32_t count = 0;

    if (value & 0x80000000u)
        value = ~value;
    while (count < 32 && !(value & 0x80000000u))
    {
        ++count;
        value <<= 1;
    }
    return count;
}

static int32_t batch4_short_dot_values(const int16_t left[3], RRJMemory *m, uint32_t right)
{
    int64_t sum = 0;
    uint32_t i;

    for (i = 0; i < 3; ++i)
        sum += (int64_t)left[i] * (int16_t)rrj_u16(rrj_at(m, right + 2u * i, 2));
    return batch4_floor_shift(sum, 8);
}

static void batch4_normalize_xz(RRJMemory *m, int16_t vector[3])
{
    uint32_t squared = (uint32_t)((int64_t)vector[0] * vector[0] + (int64_t)vector[2] * vector[2]);
    uint32_t shift = 22u - (batch4_leading_sign(squared) & ~1u);
    uint32_t entry;
    int32_t scale;
    uint32_t i;

    if (rrj_s32(shift) <= 0)
        shift = 0;
    entry = rrj_u16(rrj_at(m, rrj_read32(m, 0x8005B560u) + 2u * (squared >> shift), 2));
    scale = rrj_s32(((entry >> 5) << (entry & 31)) >> (shift >> 1));
    for (i = 0; i < 3; ++i)
        vector[i] = (int16_t)batch4_floor_shift((int64_t)vector[i] * scale, 12);
}

uint32_t sub_80083928(RRJMemory *m, uint32_t actor, uint32_t other, uint32_t normal)
{
    uint32_t other_velocity[3];
    int32_t difference[3];
    int32_t approach = 0;
    int32_t cached_dot;
    int32_t axis_dot;
    int32_t angle;
    int32_t offset;
    int32_t threshold;
    int32_t code;
    int32_t result;
    uint32_t flags;
    uint32_t collision_speed;
    uint32_t unused_speed;
    uint32_t i;

    FUNCTION_MARKER(0x80083928, "RASHCDG.BIN");
    (void)rrj_scale_short_local(m, rrj_read32(m, other + 480), other + 450, other_velocity);
    rrj_write32(m, actor + 564, rrj_read32(m, actor + 564) & ~0x4000u);
    if (!(rrj_read32(m, actor + 568) & 0x02000000u))
    {
        batch4_copy_short(m, actor + 864, actor + 450);
        rrj_write32(m, actor + 860, rrj_read32(m, actor + 480));
        rrj_write32(m, actor + 568, rrj_read32(m, actor + 568) | 0x02000000u);
    }
    flags = rrj_read32(m, actor + 568);
    if ((flags & 0x200u) || ((flags & 0x400u) && batch4_abs32((int16_t)rrj_u16(rrj_at(m, normal + 2, 2))) >= 2048))
    {
        result = rrj_s32(sub_80084564(m, normal, actor + 864, actor + 860, actor + 456, (flags & 0x200u) ? 5 : 0, 0x30000, 0));
        if (result >= 0)
            (void)sub_800849D8(m, actor);
        return (uint32_t)result;
    }

    (void)sub_8002EE50(m, rrj_read32(m, actor + 860), actor + 864, actor + 456);
    for (i = 0; i < 3; ++i)
    {
        difference[i] = rrj_s32(rrj_read32(m, actor + 456u + 4u * i) - other_velocity[i]);
        approach = rrj_s32((uint32_t)approach + (uint32_t)batch4_floor_shift((int64_t)difference[i] * (16 * (int16_t)rrj_u16(rrj_at(m, normal + 2u * i, 2))), 16));
    }
    cached_dot = rrj_s32(sub_8002E698(m, actor + 864, normal));
    axis_dot = rrj_s32(sub_8002E698(m, actor + 814, normal));
    angle = batch4_floor_shift((int64_t)25736 * rrj_s32(sub_80020018(m, (uint32_t)axis_dot, 0u - (uint32_t)cached_dot)), 8);
    offset = (angle <= 0 ? -102943 : 102943) - angle;
    threshold = rrj_s32((uint32_t)offset ^ rrj_read32(m, actor + 488)) < 0 ? 74348 : 62910;
    if (approach >= 1)
        return 0;
    cached_dot = rrj_s32(sub_8002E698(m, actor + 864, other + 450));

    if (batch4_abs32(offset) > threshold)
    {
        int middle = approach < -1171888 && approach >= -2343777;
        int low_dot = cached_dot <= 50199;

        code = 2 * low_dot + (-middle & (4 - 2 * low_dot)) + (approach < -2343777 ? 8 - 2 * low_dot : 0);
        if (code)
        {
            if (rrj_read32(m, actor + 560) & 0x20000000u)
                code = 2;
        }
        else
        {
            code = -1;
        }
    }
    else
    {
        int32_t contact_dot = rrj_s32(sub_8002E698(m, other + 450, normal));
        int middle = contact_dot > 45875 && approach < -439458 && approach >= -732430;
        int severe = contact_dot > 45875 && approach < -732430;

        code = (-middle & 0xFu) + (-severe & 0x1Fu) + 1;
        if (code == 32 && (rrj_read32(m, actor + 560) & 0x20000000u))
            code = 16;
        else if (code == 1)
            code = rrj_read32(m, actor + 828) != other + 172;
    }
    if (!code)
        return 0;

    flags = rrj_read32(m, actor + 568);
    if (flags & 0xFu)
    {
        rrj_write32(m, actor + 568, flags & 0xFFF007F0u);
        rrj_write32(m, actor + 828, 0);
        rrj_write32(m, actor + 568, (flags & 0xF7F007F0u) | 0x08000000u);
    }
    if (code >= 16 || code == -1)
    {
        int32_t first_mass = rrj_s32(rrj_read32(m, actor + 316));
        int32_t second_mass = rrj_s32(rrj_read32(m, other + 316));
        int32_t mass_sum = rrj_s32((uint32_t)first_mass + (uint32_t)second_mass);
        uint32_t reciprocal = 0x80000000u / ((uint32_t)mass_sum >> 1);
        int32_t first_scale = (int32_t)sub_8001FC90(first_mass, rrj_s32(reciprocal));
        int32_t second_scale = (int32_t)sub_8001FC90(second_mass, rrj_s32(reciprocal));
        int32_t second_speed = (int32_t)sub_8001FC90(rrj_s32(rrj_read32(m, other + 480)), batch4_abs32(cached_dot));
        int32_t first_output;
        int32_t second_output;

        (void)batch4_blend_nonnegative(first_scale, second_scale, rrj_s32(rrj_read32(m, actor + 480)), second_speed, &first_output, &second_output);
        collision_speed = (uint32_t)first_output;
        unused_speed = (uint32_t)second_output;
    }
    else
    {
        collision_speed = rrj_read32(m, actor + 860);
    }

    if (code == -1)
        rrj_write32(m, actor + 564, rrj_read32(m, actor + 564) | 0x4000u);
    else
        rrj_write32(m, actor + 568, rrj_read32(m, actor + 568) | (uint32_t)code);
    if (code >= 16)
    {
        result = rrj_s32(sub_80080B10(m, actor, actor + 864, actor + 860, other + 450, (int32_t)collision_speed));
    }
    else
    {
        result = rrj_s32(sub_80080D1C(m, actor, actor + 864, actor + 860, other + 172, normal, (int32_t)sub_8001FC90(approach, 3664), offset, threshold, (int32_t)collision_speed, 1143, 0));
    }
    flags = rrj_read32(m, actor + 568);
    flags |= (flags & 0x1Fu) ? 0x1000u : ((flags & 0x220u) != 0) << 11;
    rrj_write32(m, actor + 568, flags);
    if (flags & 0x130u)
        xport_update_u8(other + 509, XPORT_MEMORY_UPDATE_OR, 0x10u);
    return (uint32_t)result;
}

uint32_t sub_80083F30(RRJMemory *m, uint32_t actor, uint32_t contact, uint32_t normal, int32_t mode)
{
    uint32_t flags;
    int32_t approach = 0;
    int32_t first_dot = 0;
    int32_t offset;
    int32_t threshold;
    int32_t speed_offset = 0;
    int32_t code;
    int32_t result;
    uint32_t i;

    FUNCTION_MARKER(0x80083F30, "RASHCDG.BIN");
    rrj_write32(m, actor + 564, rrj_read32(m, actor + 564) & ~0x4000u);
    if (!(rrj_read32(m, actor + 568) & 0x02000000u))
    {
        batch4_copy_short(m, actor + 864, actor + 450);
        rrj_write32(m, actor + 860, rrj_read32(m, actor + 480));
        rrj_write32(m, actor + 568, rrj_read32(m, actor + 568) | 0x02000000u);
    }
    flags = rrj_read32(m, actor + 568);
    if ((flags & 0x200u) || ((flags & 0x400u) && batch4_abs32((int16_t)rrj_u16(rrj_at(m, normal + 2, 2))) >= 2048))
    {
        result = rrj_s32(sub_80084564(m, normal, actor + 864, actor + 860, actor + 456, (flags & 0x200u) ? 5 : 0, 0x30000, 0));
        if (result >= 0)
            (void)sub_800849D8(m, actor);
        return (uint32_t)result;
    }

    if (mode == 2)
    {
        offset = 0;
    }
    else
    {
        for (i = 0; i < 3; ++i)
            approach = rrj_s32((uint32_t)approach + (uint32_t)batch4_floor_shift((int64_t)rrj_s32(rrj_read32(m, actor + 456u + 4u * i)) * (16 * (int16_t)rrj_u16(rrj_at(m, normal + 2u * i, 2))), 16));
        if (mode >= 2)
        {
            if (rrj_read32(m, actor + 568) & 0x600u)
            {
                int16_t forward[3] = {(int16_t)rrj_u16(rrj_at(m, actor + 450, 2)), 0, (int16_t)rrj_u16(rrj_at(m, actor + 454, 2))};
                int16_t side[3];

                batch4_normalize_xz(m, forward);
                side[0] = forward[2];
                side[1] = 0;
                side[2] = (int16_t)-forward[0];
                first_dot = batch4_short_dot_values(forward, m, normal);
                offset = batch4_floor_shift((int64_t)25736 * rrj_s32(sub_80020018(m, (uint32_t)batch4_short_dot_values(side, m, normal), 0u - (uint32_t)first_dot)), 8);
            }
            else
            {
                first_dot = rrj_s32(sub_8002E698(m, actor + 864, normal));
                offset = batch4_floor_shift((int64_t)25736 * rrj_s32(sub_80020018(m, sub_8002E698(m, actor + 814, normal), 0u - (uint32_t)first_dot)), 8);
            }
            offset = (offset > 0 ? 102943 : -102943) - offset;
            if (offset < -102943)
                offset = -102943;
            if (offset > 102943)
                offset = 102943;
        }
        else
        {
            offset = mode == 1 ? 11438 : -11438;
        }
    }

    if (first_dot >= 1)
        return 0;
    if (mode == 2)
    {
        threshold = 0;
        code = -1;
        speed_offset = (int32_t)sub_8001FC90(655, rrj_s32(rrj_read32(m, actor + 860)));
    }
    else
    {
        int32_t index;

        threshold = rrj_s32((uint32_t)offset ^ rrj_read32(m, actor + 488)) < 0 ? 74348 : 62910;
        if (mode < 2)
        {
            uint32_t table_index;
            int32_t scale_a;
            int32_t scale_b;

            index = (int16_t)batch4_floor_shift((int64_t)163 * offset, 14);
            table_index = (uint32_t)index & 4095u;
            scale_b = (int16_t)rrj_u16(rrj_at(m, 0x8005624Eu + 4u * table_index, 2)) * 16;
            if (index < 0)
                scale_b = -scale_b;
            scale_a = (int16_t)rrj_u16(rrj_at(m, 0x8005624Cu + 4u * table_index, 2)) * 16;
            if (index > 0)
                scale_a = -scale_a;
            (void)sub_8002EB78(m, actor + 864, actor + 814, normal, (uint32_t)scale_a, (uint32_t)scale_b);
        }
        if (approach > 0)
            approach = 0;
        if (batch4_abs32(offset) <= threshold)
        {
            code = rrj_read32(m, actor + 828) != contact;
        }
        else
        {
            int active = approach < -1171888 && !(rrj_read32(m, actor + 560) & 0x20000000u);
            int severe = mode == 5 && approach < -2343777;

            code = (active && severe ? 6 : 0) + (active && !severe ? 2 : 0) + 2;
        }
    }
    if (!code)
        return 0;

    flags = rrj_read32(m, actor + 568);
    if (flags & 0xFu)
    {
        rrj_write32(m, actor + 568, flags & 0xFFF007F0u);
        rrj_write32(m, actor + 828, 0);
        rrj_write32(m, actor + 568, (flags & 0xF7F007F0u) | 0x08000000u);
    }
    if (code == -1)
        rrj_write32(m, actor + 564, rrj_read32(m, actor + 564) | 0x4000u);
    else
        rrj_write32(m, actor + 568, rrj_read32(m, actor + 568) | (uint32_t)code);
    result = rrj_s32(sub_80080D1C(m, actor, actor + 864, actor + 860, contact, normal, (int32_t)sub_8001FC90(approach, 3664), offset, threshold, rrj_s32(rrj_read32(m, actor + 860) - (uint32_t)speed_offset), 1143, 1));
    flags = rrj_read32(m, actor + 568);
    if (flags & 0x1Fu)
    {
        if (mode == 3 && (flags & 0xCu))
        {
            uint32_t owner = rrj_read32(m, actor + 852);

            rrj_write32(m, owner + 552, rrj_read32(m, owner + 552) | 0x10000u);
            if (r_u8(owner + 572) & 0x10u)
            {
                uint32_t linked_owner = rrj_read32(m, rrj_read32(m, actor + 856) + 852);

                rrj_write32(m, linked_owner + 552, rrj_read32(m, linked_owner + 552) | 0x10000u);
            }
        }
        flags |= 0x1000u;
    }
    else if (flags & 0x220u)
    {
        flags |= 0x800u;
    }
    rrj_write32(m, actor + 568, flags);
    return (uint32_t)result;
}
