#include "race_leaf_batch_004.h"
#include "fixed_math.h"
#include "race_leaf_80080D1C.h"
#include "race_leaf.h"
#include "race_pause.h"
#include "xport.h"

uint32_t sub_80083864(int32_t first, int32_t second, int32_t first_scale, int32_t second_scale, uint32_t *first_output, uint32_t *second_output)
{
    uint32_t first_product;
    uint32_t second_product;
    uint32_t result;
    uint32_t value;
    FUNCTION_MARKER(0x80083864, "RASHCDG.BIN");
    first_product = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)((uint32_t)first - (uint32_t)second)), first_scale);
    second_product = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)((uint32_t)second << 1)), second_scale);
    value = first_product + second_product;
    xport_store_le32(first_output, value);
    if (((int32_t)(uint32_t)(value)) < 0)
        value = 0;
    xport_store_le32(first_output, value);
    first_product = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)((uint32_t)first << 1)), first_scale);
    result = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)((uint32_t)second - (uint32_t)first)), second_scale);
    value = first_product + result;
    xport_store_le32(second_output, value);
    if (((int32_t)(uint32_t)(value)) < 0)
        value = 0;
    xport_store_le32(second_output, value);
    return result;
}

uint32_t sub_80084BE8(uint32_t actor, uint32_t current)
{
    uint32_t source = actor + 450;
    uint32_t tag;
    uint32_t type;
    int32_t angle_base = 56;
    int32_t value = 435486;
    uint32_t flags;
    uint32_t angle;

    FUNCTION_MARKER(0x80084BE8, "RASHCDG.BIN");
    (void)sub_8002EAD8(xport_guest_ptr(actor + 504, 12), xport_guest_ptr(actor + 522, 6), r_u32(actor + 768), xport_guest_ptr(actor + 184, 12));
    tag = r_u16(r_u32(actor + 832));
    type = tag >> 5;
    if (type == 3)
    {
        angle_base = 170;
        value = 725811;
    }
    else if (type == 5)
    {
        uint32_t object = r_u32(r_u32(0x800CE5A4u) + 452u * (tag & 31u));
        uint32_t kind = (r_u16(object + 14) & 0xF80u) >> 7;

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
        flags = r_u32(actor + 568);
        if (!(flags & 0x02000000u))
        {
            w_u16(actor + 864, r_u16(actor + 450));
            w_u16(actor + 866, r_u16(actor + 452));
            w_u16(actor + 868, r_u16(actor + 454));
            w_u32(actor + 860, r_u32(actor + 480));
            w_u32(actor + 568, flags | 0x02000000u);
        }
    }
    angle = sub_8001FF3C((uint32_t)(int32_t)r_s16(source + 2) << 4);
    (void)sub_8007E868(source, ((int32_t)(uint32_t)(r_u32(actor + 576))), (int16_t)r_u16(0x8005624Cu + (4u * ((uint32_t)(angle_base + 1024) - angle) & 0x3FFCu) + 2u), value);
    flags = r_u32(actor + 568) | 0xC00u;
    w_u32(actor + 568, flags);
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

uint32_t sub_800B658C(uint32_t actor, uint32_t other, int32_t value, int32_t divisor, int32_t factor)
{
    int32_t first = batch4_clamp_ratio(255, value, divisor, factor);
    int32_t second = batch4_clamp_ratio(150, value, divisor, factor);
    uint32_t state = r_u32(0x8005B2F8u);
    uint32_t result = 0;

    FUNCTION_MARKER(0x800B658C, "RASHCDG.BIN");
    if (!other || ((r_u16(other + 172) >> 5) == 1u && (r_u16(other + 172) & 31u) < r_u32(state + 48)))
        result = sub_8001DD74(r_u16(actor + 172), second, 150, (uint32_t)first);
    if (!other || (r_u8(other + 572) & 0x20u))
    {
        uint32_t link = r_u32(actor + 856);
        uint32_t linked_actor;

        if (!link)
            return 0;
        linked_actor = r_u32(link + 852);
        result = r_u8(linked_actor + 572) & 0x40u;
        if (!result)
        {
            int eligible = 0;

            if (link && (r_u8(r_u32(actor + 852) + 572) & 0x10u) && r_u32(linked_actor + 604) < 2u)
                eligible = 1;
            if (other)
                eligible = 1;
            if (eligible)
            {
                uint32_t source = other ? r_u32(r_u32(other + 596) + 856) : actor;
                uint32_t index = r_u16(source + 172) + 1u + (r_u32(state + 48) >= 2u);

                return sub_8001DD74(index, second, 150, (uint32_t)first);
            }
        }
    }
    return result;
}

uint32_t sub_800A8DF0(uint8_t *actor, const uint32_t delta[3], uint32_t mark_dirty)
{
    uint32_t position;
    uint32_t displacement;
    uint32_t type;
    uint32_t linked;
    uint32_t vertex;
    uint32_t axis;
    uint32_t result;
    int move_linked = 0;
    FUNCTION_MARKER(0x800A8DF0, "RASHCDG.BIN");
    position = xport_load_le32(actor + 184);
    displacement = xport_load_le32(delta);
    xport_store_le32(actor + 184, position + displacement);
    position = xport_load_le32(actor + 188);
    displacement = xport_load_le32(delta + 1);
    type = xport_load_le16(actor + 172) >> 5;
    xport_store_le32(actor + 188, position + displacement);
    position = xport_load_le32(actor + 192);
    displacement = xport_load_le32(delta + 2);
    xport_store_le32(actor + 192, position + displacement);
    if (!type && xport_load_le32(actor + 856))
        move_linked = xport_load_le32(actor + 1088) != 0;
    if (move_linked)
    {
        for (axis = 0; axis < 3; ++axis)
        {
            linked = xport_load_le32(actor + 856);
            displacement = xport_load_le32(delta + axis);
            position = r_u32(linked + 184u + 4u * axis);
            w_u32(linked + 184u + 4u * axis, position + displacement);
        }
    }
    for (vertex = 0; vertex < 8; ++vertex)
    {
        for (axis = 0; axis < 3; ++axis)
        {
            uint32_t offset = 196u + 12u * vertex + 4u * axis;
            position = xport_load_le32(actor + offset);
            displacement = xport_load_le32(delta + axis);
            xport_store_le32(actor + offset, position + displacement);
        }
        if (move_linked)
        {
            for (axis = 0; axis < 3; ++axis)
            {
                uint32_t offset = 196u + 12u * vertex + 4u * axis;
                linked = xport_load_le32(actor + 856);
                displacement = xport_load_le32(delta + axis);
                position = r_u32(linked + offset);
                w_u32(linked + offset, position + displacement);
            }
        }
    }
    if (!mark_dirty)
        return 1;
    if (!type)
    {
        result = xport_load_le32(actor + 568) | 0x01800000u;
        xport_store_le32(actor + 568, result);
        return result;
    }
    if (type == 1)
    {
        result = xport_load_le32(actor + 552) | 2u;
        xport_store_le32(actor + 552, result);
        return result;
    }
    if (type == 2)
    {
        result = xport_load_le32(actor + 568) | 2u;
        xport_store_le32(actor + 568, result);
        return result;
    }
    if (type == 4)
    {
        result = xport_load_le32(actor + 592) | 0x100u;
        xport_store_le32(actor + 592, result);
        return result;
    }
    return 4;
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

static void batch4_outer_product(uint32_t left, uint32_t right, uint32_t output)
{
    int16_t left_value[3];
    int16_t right_value[3];
    int16_t cross[3];
    uint32_t i;

    for (i = 0; i < 3; ++i)
    {
        left_value[i] = (int16_t)r_u16(left + 2u * i);
        right_value[i] = (int16_t)r_u16(right + 2u * i);
    }
    cross[0] = batch4_cross_component((int64_t)left_value[1] * right_value[2] - (int64_t)left_value[2] * right_value[1]);
    cross[1] = batch4_cross_component((int64_t)left_value[2] * right_value[0] - (int64_t)left_value[0] * right_value[2]);
    cross[2] = batch4_cross_component((int64_t)left_value[0] * right_value[1] - (int64_t)left_value[1] * right_value[0]);
    for (i = 0; i < 3; ++i)
        w_u16(output + 2u * i, (uint16_t)cross[i]);
}

uint32_t sub_80080B10(uint32_t actor, uint32_t normal, uint32_t distance_out, uint32_t input, int32_t distance)
{
    uint32_t flags = r_u32(actor + 568);
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
    if (((int32_t)(uint32_t)(r_u32(actor + 364))) < 0)
        flags |= 0x00400000u;
    else
        flags &= ~0x00400000u;
    w_u32(actor + 568, flags);
    if (distance <= 0x1FFFF)
    {
        flags = (flags & 0xF7FFFFCFu) | 0x08000000u;
        w_u32(actor + 568, flags);
        return 0;
    }
    for (i = 0; i < 3; ++i)
        w_u16(normal + 2u * i, r_u16(input + 2u * i));
    if (flags & 0x600u)
    {
        w_u32(distance_out, (uint32_t)distance);
        (void)sub_8002EE50((uint32_t)distance, xport_guest_ptr(normal, 6), xport_guest_ptr(actor + 456, 12));
        return 1;
    }
    for (i = 0; i < 3; ++i)
    {
        basis[i] = (int16_t)r_u16(actor + 522u + 2u * i);
        vector[i] = (int16_t)r_u16(normal + 2u * i);
    }
    cross[0] = batch4_cross_component((int64_t)basis[1] * vector[2] - (int64_t)basis[2] * vector[1]);
    cross[1] = batch4_cross_component((int64_t)basis[2] * vector[0] - (int64_t)basis[0] * vector[2]);
    cross[2] = batch4_cross_component((int64_t)basis[0] * vector[1] - (int64_t)basis[1] * vector[0]);
    for (i = 0; i < 3; ++i)
        w_u16(cross_address + 2u * i, (uint16_t)cross[i]);
    if (!sub_8002E468(xport_guest_ptr(cross_address, 6)))
        for (i = 0; i < 3; ++i)
            w_u16(cross_address + 2u * i, r_u16(actor + 432u + 2u * i));
    linked = r_u32(actor + 856);
    w_u32(actor + 576, (uint32_t)distance);
    w_u32(actor + 488, 0);
    if (linked)
        w_u32(linked + 488, 0);
    first_dot = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(actor + 528, 6), xport_guest_ptr(cross_address, 6))));
    second_dot = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(actor + 528, 6), xport_guest_ptr(actor + 450, 6))));
    angle = ((int32_t)(uint32_t)(sub_80020018((uint32_t)first_dot, (uint32_t)second_dot)));
    w_u32(actor + 676, (uint32_t)(((int64_t)25736 * angle) >> 8));
    w_u32(actor + 680, 0);
    if (flags & 0x100u)
    {
        w_u32(actor + 656, ((int32_t)(uint32_t)(r_u32(actor + 636))) > 0 ? 0x00020000u : 0xFFFE0000u);
        w_u32(actor + 568, flags & 0xFFFFFFCFu);
    }
    return 1;
}

uint32_t sub_800A3CBC(uint32_t actor)
{
    static const int16_t identity[9] = {4096, 0, 0, 0, 4096, 0, 0, 0, 4096};
    uint32_t i;

    FUNCTION_MARKER(0x800A3CBC, "RASHCDG.BIN");
    batch4_outer_product(actor + 432, actor + 522, actor + 528);
    if (sub_8002E468(xport_guest_ptr(actor + 528, 6)))
    {
        batch4_outer_product(actor + 522, actor + 528, actor + 516);
        return actor + 516;
    }

    batch4_outer_product(actor + 522, actor + 444, actor + 516);
    if (!sub_8002E468(xport_guest_ptr(actor + 516, 6)))
    {
        for (i = 0; i < 9; ++i)
            w_u16(actor + 432u + 2u * i, (uint16_t)identity[i]);
        for (i = 0; i < 9; ++i)
            w_u16(actor + 516u + 2u * i, (uint16_t)identity[i]);
    }
    batch4_outer_product(actor + 516, actor + 522, actor + 528);
    return 4096;
}

uint32_t sub_800849D8(uint32_t actor)
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
    flags = r_u32(actor + 568);
    if (flags & 0x00080000u)
        return 0x8000u;

    w_u32(actor + 724, 0);
    w_u32(actor + 728, 0x8000u);
    first_random = (int32_t)(sub_8001FC58() % 0xB2B8u) - 22876;
    second_random = (int32_t)(sub_8001FC58() % 0xB2B8u) - 22876;
    w_u32(actor + 656, (uint32_t)first_random << 1);
    w_u32(actor + 640, (uint32_t)second_random << 1);
    velocity = (int32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(actor + 488))), (int32_t)(sub_8001FC58() % 0x6667u) + 0xCCCC);
    w_u32(actor + 488, (uint32_t)velocity);

    speed = ((int32_t)(uint32_t)(r_u32(actor + 480)));
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
    w_u32(actor + 488, (uint32_t)velocity);

    result = (int16_t)r_u16(actor + 796) * (int16_t)r_u16(actor + 454) - (int16_t)r_u16(actor + 800) * (int16_t)r_u16(actor + 450);
    if ((result < 0 && velocity > 0) || (result > 0 && velocity < 0))
    {
        velocity = ((int32_t)(uint32_t)(0u - (uint32_t)velocity));
        w_u32(actor + 488, (uint32_t)velocity);
        return (uint32_t)velocity;
    }
    return (uint32_t)result;
}

uint32_t sub_800AD9BC(uint32_t actor, int32_t value, int32_t direction, int32_t active, int32_t blend)
{
    int32_t doubled = ((int32_t)(uint32_t)(r_u32(actor + 308) << 1));
    int32_t ratio;
    int32_t half = value >> 1;
    int32_t angle;
    int32_t magnitude;
    int32_t result;
    uint32_t flags;

    FUNCTION_MARKER(0x800AD9BC, "RASHCDG.BIN");
    w_u32(actor + 768, 0u - (uint32_t)value);
    flags = r_u32(actor + 564) | 0x00218000u | ((0u - (uint32_t)active) & 0x00020000u);
    w_u32(actor + 564, flags);

    if (value > 0)
    {
        ratio = (int32_t)sub_80010028((uint32_t)value, doubled > 0 ? (uint32_t)doubled : 0u - (uint32_t)doubled);
    }
    else if (doubled > 0)
    {
        ratio = ((int32_t)(uint32_t)(0u - sub_80010028(0u - (uint32_t)value, (uint32_t)doubled)));
    }
    else
    {
        ratio = (int32_t)sub_80010028(0u - (uint32_t)value, 0u - (uint32_t)doubled);
    }
    if (ratio > 64880)
    {
        ratio = 64880;
        half = (int32_t)sub_8001FC90(64880, ((int32_t)(uint32_t)(r_u32(actor + 308))));
    }
    angle = ((int32_t)(uint32_t)(sub_8001FF3C((uint32_t)ratio)));
    magnitude = (int32_t)(((int64_t)25736 * angle) >> 8);

    if (blend <= 0x7FFF)
    {
        int32_t scale = (int32_t)sub_8001FC90(blend, 0x20000);
        int32_t table_angle = (int32_t)sub_8001FC90(magnitude, scale);
        uint32_t table_offset = ((163u * (uint32_t)table_angle) >> 12) & 0x3FFCu;
        int32_t sine = (int16_t)r_u16(0x8005624Cu + table_offset);

        result = (int32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(actor + 308))), sine << 4);
        w_u32(actor + 772, 0u - (uint32_t)result);
        result = table_angle;
    }
    else if (blend < 0x10000 && (!active || !direction))
    {
        int32_t scale = (int32_t)sub_8001FC90(0x10000 - blend, 0x20000);

        result = (int32_t)sub_8001FC90(value - half, scale) - value;
        w_u32(actor + 772, (uint32_t)result);
        result = active ? magnitude : (int32_t)sub_8001FC90(magnitude, scale);
    }
    else
    {
        w_u32(actor + 772, 0u - (uint32_t)value);
        if (active)
        {
            (void)sub_80084BE8(actor, 0);
            result = magnitude;
            direction = 1;
            w_u32(actor + 564, r_u32(actor + 564) & 0xFFFEFFFFu);
        }
        else
        {
            result = 0;
        }
    }

    result = ((int32_t)(uint32_t)(((0u - (uint32_t)direction) & ((uint32_t)result << 1)) - (uint32_t)result));
    if (blend <= 0x7FFF && result > 0 && result < ((int32_t)(uint32_t)(r_u32(actor + 616))))
        result = ((int32_t)(uint32_t)(r_u32(actor + 616)));
    w_u32(actor + 616, (uint32_t)result);
    return (uint32_t)result;
}

static uint32_t sub_800B16FC(uint32_t actor, uint32_t value_pointer, uint32_t state)
{
    uint32_t tag = r_u16(actor + 172);
    int32_t lateral = ((int32_t)(uint32_t)(r_u32(actor + 460)));
    uint32_t quotient = sub_80010028(lateral > 0 ? (uint32_t)lateral : 0u - (uint32_t)lateral, 0x80000u);
    int32_t signed_quotient = lateral > 0 ? (int32_t)quotient : ((int32_t)(uint32_t)(0u - quotient));
    int32_t remainder = ((int32_t)(uint32_t)(0x30000u - (uint32_t)signed_quotient));
    int32_t adjusted = signed_quotient < 0 ? 0 : signed_quotient;
    int32_t dot;
    int32_t scale;
    uint32_t linked;
    uint32_t result;

    FUNCTION_MARKER(0x800B16FC, "RASHCDG.BIN");
    if (tag < r_u32(state + 48))
    {
        uint32_t record = 0x800CD898u + 1132u * tag;

        if (remainder < 0)
            adjusted = ((int32_t)(uint32_t)((uint32_t)adjusted + (uint32_t)remainder));
        w_u32(record + 740, (uint32_t)adjusted);
    }

    dot = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(actor + 450, 6), xport_guest_ptr(actor + 444, 6))));
    scale = dot < 0 ? 6553 : 58982;
    w_u32(actor + 576, (uint32_t)sub_8001FC90(scale, ((int32_t)(uint32_t)(r_u32(value_pointer)))));
    linked = r_u32(actor + 852);
    w_u32(actor + 568, r_u32(actor + 568) & 0xFFFFFBFFu);
    w_u32(linked + 552, r_u32(linked + 552) & 0xFFFFDFFFu);
    if (((int32_t)(uint32_t)(r_u32(actor + 768))) > 0x30000)
    {
        (void)sub_80017BA0(((int32_t)(uint32_t)(r_u32(actor + 184))), ((int32_t)(uint32_t)(r_u32(actor + 192))), 54, 0);
        if (tag < r_u32(r_u32(0x8005B2F8u) + 48) && r_u32(linked + 604) < 2u && !r_u32(0x8005B220u))
            (void)sub_800B658C(actor, 0, ((int32_t)(uint32_t)(r_u32(value_pointer))), 1464860, 1);
    }
    result = r_u32(actor + 568) | 0x0C000000u;
    w_u32(actor + 568, result);
    return result;
}

uint32_t sub_800B16F4(uint32_t actor, uint32_t value_pointer)
{
    FUNCTION_MARKER(0x800B16F4, "RASHCDG.BIN");
    return sub_800B16FC(actor, value_pointer, r_u32(0x8005B2F8u));
}

static void batch4_rotate_short_vectors(int16_t left[3], int16_t right[3], int32_t angle)
{
    uint32_t table = 0x8005624Cu + 4u * ((uint32_t)angle & 4095u);
    int32_t sine = (int16_t)r_u16(table) * 16;
    int32_t cosine = (int16_t)r_u16(table + 2) * 16;
    int16_t saved_left[3];
    int16_t saved_right[3];
    uint32_t i;

    for (i = 0; i < 3; ++i)
    {
        uint32_t left_value = (uint32_t)(int32_t)left[i] << 4;
        uint32_t right_value = (uint32_t)(int32_t)right[i] << 4;
        uint32_t first = (uint32_t)sub_8001FC90(cosine, ((int32_t)(uint32_t)(left_value)));
        uint32_t second = (uint32_t)sub_8001FC90(-sine, ((int32_t)(uint32_t)(right_value)));
        uint32_t third = (uint32_t)sub_8001FC90(sine, ((int32_t)(uint32_t)(left_value)));
        uint32_t fourth = (uint32_t)sub_8001FC90(cosine, ((int32_t)(uint32_t)(right_value)));

        saved_left[i] = (int16_t)(((int32_t)(uint32_t)(first + second)) >> 4);
        saved_right[i] = (int16_t)(((int32_t)(uint32_t)(third + fourth)) >> 4);
    }
    for (i = 0; i < 3; ++i)
    {
        left[i] = saved_left[i];
        right[i] = saved_right[i];
    }
}

static int32_t batch4_signed_reciprocal(uint32_t scaled)
{
    uint32_t magnitude = ((int32_t)(uint32_t)(scaled)) < 0 ? 0u - scaled : scaled;
    uint32_t divisor = (magnitude >> 1) + (uint32_t)(((int32_t)(uint32_t)(magnitude - 2u)) >> 31);
    uint32_t result = 0x80000000u / divisor;

    return ((int32_t)(uint32_t)(scaled)) < 0 ? ((int32_t)(uint32_t)(0u - result)) : ((int32_t)(uint32_t)(result));
}

static int16_t batch4_quaternion_component(int32_t value, int32_t scale)
{
    uint32_t product = (uint32_t)((int64_t)value * scale);

    return (int16_t)(((int32_t)(uint32_t)(product)) >> 14);
}

static void batch4_matrix_to_quaternion(const int16_t matrix[9], uint32_t output)
{
    uint32_t axes[3];
    int32_t diagonal[3];
    int32_t trace;
    uint32_t root;
    int32_t reciprocal;
    uint32_t i;

    for (i = 0; i < 3; ++i)
    {
        axes[i] = r_u32(0x8005B61Cu + 4u * i);
        diagonal[i] = matrix[4u * i];
    }
    trace = diagonal[0] + diagonal[1] + diagonal[2];
    if (((int32_t)(uint32_t)((uint32_t)trace << 4)) > 0)
    {
        root = sub_8004CF74(((uint32_t)trace << 4) + 0x10000u);
        w_u16(output + 6, (uint16_t)(root >> 1));
        reciprocal = ((int32_t)(batch4_signed_reciprocal(((uint32_t)(root)) << 3)));
        w_u16(output, (uint16_t)batch4_quaternion_component(matrix[5] - matrix[7], reciprocal));
        w_u16(output + 2, (uint16_t)batch4_quaternion_component(matrix[6] - matrix[2], reciprocal));
        w_u16(output + 4, (uint16_t)batch4_quaternion_component(matrix[1] - matrix[3], reciprocal));
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
        root = sub_8004CF74(((uint32_t)(diagonal[largest] - diagonal[second] - diagonal[third]) << 4) + 0x10000u);
        w_u16(output + 2u * largest, (uint16_t)(root >> 1));
        reciprocal = ((int32_t)(batch4_signed_reciprocal(((uint32_t)(root)) << 3)));
        w_u16(output + 6, (uint16_t)batch4_quaternion_component(matrix[3u * second + third] - matrix[3u * third + second], reciprocal));
        w_u16(output + 2u * second, (uint16_t)batch4_quaternion_component(matrix[3u * largest + second] + matrix[3u * second + largest], reciprocal));
        w_u16(output + 2u * third, (uint16_t)batch4_quaternion_component(matrix[3u * largest + third] + matrix[3u * third + largest], reciprocal));
    }
}

uint32_t sub_800B3838(uint32_t actor, uint32_t first_axis, uint32_t second_axis, uint32_t third_axis, int32_t value)
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
    dot = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(actor + 444, 6), xport_guest_ptr(first_axis, 6))));
    for (i = 0; i < 3; ++i)
    {
        int16_t first = (int16_t)r_u16(first_axis + 2u * i);
        int16_t second = (int16_t)r_u16(second_axis + 2u * i);
        int16_t third = (int16_t)r_u16(third_axis + 2u * i);

        matrix[i] = dot <= 0 ? (int16_t)(0u - (uint16_t)second) : second;
        matrix[3u + i] = third;
        matrix[6u + i] = dot <= 0 ? (int16_t)(0u - (uint16_t)first) : first;
    }
    if (dot > 0)
        direction = -1024;
    batch4_rotate_short_vectors(matrix, matrix + 6, (int32_t)(sub_8001FC58() % 0x2AAu) - 341);
    flags = r_u32(actor + 592) | 0x200u;
    w_u32(actor + 592, flags);
    batch4_rotate_short_vectors(matrix + 3, matrix + 6, -direction);
    batch4_matrix_to_quaternion(matrix, actor + 572);

    for (i = 0; i < 4; ++i)
    {
        int32_t left = (int16_t)r_u16(actor + 572u + 2u * i);
        int32_t right = (int16_t)r_u16(actor + 560u + 2u * i);

        sum += (uint32_t)(left * right);
    }
    flags &= ~0x40u;
    w_u32(actor + 592, flags);
    if (((int32_t)(uint32_t)((uint32_t)(((int32_t)(uint32_t)(sum)) >> 14) << 2)) < 0)
        w_u32(actor + 592, flags | 0x40u);

    response = (int32_t)sub_8001FC90(6443, ((int32_t)(uint32_t)((uint32_t)value - 65918u)));
    w_u32(actor + 484, (uint32_t)response);
    w_u32(actor + 488, (uint32_t)response << 1);
    combined = ((int32_t)(uint32_t)((uint32_t)response + r_u32(actor + 556)));
    w_u32(actor + 484, (uint32_t)combined);
    if (combined > 49152)
    {
        w_u32(actor + 484, 0x10000u);
        return 0x10000u;
    }
    return 0;
}

uint32_t sub_800A9408(uint32_t actor, int32_t amount, int32_t ratio, int32_t divisor, int32_t mode)
{
    uint32_t flags = r_u32(actor + 568);
    uint32_t linked = r_u32(actor + 852);
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

        ratio = (ratio > 0) == (divisor > 0) ? (int32_t)quotient : ((int32_t)(uint32_t)(0u - quotient));
    }
    scaled = ((int32_t)(uint32_t)(5u * (uint32_t)amount)) / ((int32_t)(uint32_t)(r_u32(r_u32(actor + 556) + 224)));
    clamped = scaled < 0 ? 0 : scaled;
    if (clamped > 4)
        clamped = 4;

    if (r_u32(linked + 604) == 1)
    {
        uint32_t first_tag = r_u16(linked + 544);
        uint32_t second_tag = r_u16(linked + 610);
        int first_allowed = first_tag - 26u >= 12u && r_u16(0x800541D4u + 8u * first_tag + 2u) != 3u;
        int second_allowed = !r_u16(linked + 608) || (second_tag - 26u >= 12u && r_u16(0x800541D4u + 8u * second_tag + 2u) != 3u);

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
            (void)sub_800C4550(state, linked, options);
        }
    }
    if (active)
    {
        uint32_t controller = r_u32(actor + 1084);
        uint32_t current = r_u8(controller + 37);
        uint32_t product = (uint32_t)((int64_t)clamped * ratio);
        int32_t next = (int32_t)current - (((int32_t)(uint32_t)(product)) >> 16);

        if (next > 0)
        {
            uint8_t controller_flags = r_u8(controller + 68);

            if (((current & 0x80u) && next < 128) || (current >= 0x40u && next < 64))
                controller_flags |= 0x40u;
            w_u8(controller + 68, controller_flags);
            w_u8(r_u32(actor + 1084) + 37, (uint8_t)next);
        }
        else
        {
            w_u8(controller + 37, 0);
        }
    }
    return (uint32_t)clamped;
}

uint32_t sub_80084564(const uint16_t first[3], uint32_t second, uint32_t magnitude_pointer, uint32_t output, int32_t response, int32_t minimum, uint32_t zero_vertical)
{
    int32_t dot;
    int32_t absolute_vertical;
    int shallow;
    int32_t scale;
    uint32_t i;

    FUNCTION_MARKER(0x80084564, "RASHCDG.BIN");
    dot = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(second, 6), first)));
    if (dot >= 0)
        return 0xFFFFFFFFu;
    absolute_vertical = (int16_t)xport_load_le16((const uint8_t *)first + 2);
    if (absolute_vertical < 0)
        absolute_vertical = -absolute_vertical;
    shallow = absolute_vertical < 2048;

    if (shallow && (int16_t)r_u16(second + 2) < 0)
    {
        int32_t x = -(int16_t)xport_load_le16((const uint8_t *)first + 4) * 16;
        int32_t z = (int16_t)xport_load_le16(first) * 16;
        uint32_t roots[3];
        uint32_t denominator;
        uint32_t reciprocal_bits;
        int negative;
        int32_t reciprocal;
        int32_t normal_x;

        for (i = 0; i < 3; ++i)
        {
            uint32_t squared_x = (uint32_t)sub_8001FC90(x, x);
            uint32_t squared_z = (uint32_t)sub_8001FC90(z, z);

            roots[i] = sub_8004CF74(squared_x + squared_z);
        }
        negative = ((int32_t)(uint32_t)(roots[0] << 2)) < 0;
        if (negative)
        {
            uint32_t half = (uint32_t)(((int32_t)(uint32_t)(0u - (roots[1] << 2))) >> 1);
            uint32_t correction = (uint32_t)(((int32_t)(uint32_t)((0u - (roots[2] << 2)) - 2u)) >> 31);

            denominator = half + correction;
        }
        else
        {
            uint32_t half = (uint32_t)(((int32_t)(uint32_t)(roots[1] << 2)) >> 1);
            uint32_t correction = (uint32_t)(((int32_t)(uint32_t)((roots[2] << 2) - 2u)) >> 31);

            denominator = half + correction;
        }
        reciprocal_bits = denominator ? 0x80000000u / denominator : 0xFFFFFFFFu;
        if (negative)
            reciprocal_bits = 0u - reciprocal_bits;
        reciprocal = ((int32_t)(uint32_t)(reciprocal_bits));
        normal_x = (int32_t)sub_8001FC90(x, reciprocal);
        int32_t normal_z = (int32_t)sub_8001FC90(z, reciprocal);
        int32_t vector_x = (int16_t)r_u16(second) * 16;
        int32_t vector_z = (int16_t)r_u16(second + 4) * 16;
        uint32_t projection_x = (uint32_t)sub_8001FC90(normal_x, vector_x);
        uint32_t projection_z = (uint32_t)sub_8001FC90(normal_z, vector_z);
        int32_t projection = ((int32_t)(uint32_t)(projection_x + projection_z));
        int32_t doubled = ((int32_t)(uint32_t)((uint32_t)projection << 1));
        int32_t value;

        value = ((int32_t)(uint32_t)((uint32_t)sub_8001FC90(doubled, normal_x) - (uint32_t)vector_x));
        w_u16(second, (uint16_t)(int16_t)(value >> 4));
        if (zero_vertical)
        {
            w_u16(second + 2, 0);
        }
        else
        {
            value = ((int32_t)(uint32_t)(46333u - ((uint32_t)(int32_t)r_s16(second + 2) << 4)));
            w_u16(second + 2, (uint16_t)(int16_t)(value >> 5));
        }
        value = ((int32_t)(uint32_t)((uint32_t)sub_8001FC90(doubled, normal_z) - (uint32_t)vector_z));
        w_u16(second + 4, (uint16_t)(int16_t)(value >> 4));
        minimum /= 2;
    }
    else
    {
        int32_t incidence = ((int32_t)(uint32_t)(0u - (uint32_t)dot));
        int32_t angle;

        if (!minimum)
        {
            int32_t distance = ((int32_t)(uint32_t)(r_u32(magnitude_pointer)));

            if (distance <= 0x1FFFF)
                response = 0;
            else if (distance <= 0x9FFFF)
            {
                int32_t ramp = (int32_t)sub_8001FC90(distance - 0x20000, 0x2000);

                response = (int32_t)sub_8001FC90(ramp, ((int32_t)(uint32_t)((uint32_t)response << 16))) >> 16;
            }
        }
        angle = ((int32_t)(uint32_t)(sub_8001FF3C((uint32_t)incidence)));
        scale = incidence;
        if (response)
        {
            uint32_t product = (uint32_t)((int64_t)response * angle);
            int32_t tangent_angle = ((int32_t)(uint32_t)(product)) / 10;
            int32_t tangent;
            int32_t cosine;

            if (tangent_angle < 56)
                tangent_angle = 56;
            if (tangent_angle > 512)
                tangent_angle = 512;
            tangent = ((int32_t)(uint32_t)(sub_8001FEB4(tangent_angle)));
            cosine = (int16_t)r_u16(0x8005624Eu + 4u * ((uint32_t)angle & 4095u)) * 16;
            scale = ((int32_t)(uint32_t)((uint32_t)incidence + (uint32_t)sub_8001FC90(cosine, tangent)));
        }
        if (incidence > 64552 && scale < 72089)
            scale = 72089;
        (void)sub_8002EA20(xport_guest_ptr(second, 6), first, scale, xport_guest_ptr(output, 12));
        for (i = 0; i < 3; ++i)
            w_u16(second + 2u * i, (uint16_t)(int16_t)(((int32_t)(uint32_t)(r_u32(output + 4u * i))) >> 4));
    }

    scale = (int32_t)sub_8001FC90(shallow ? 6553 : 52428, ((int32_t)(uint32_t)(r_u32(magnitude_pointer))));
    w_u32(magnitude_pointer, (uint32_t)scale);
    if (minimum < scale)
        minimum = scale;
    w_u32(magnitude_pointer, (uint32_t)minimum);
    (void)sub_8002E468(xport_guest_ptr(second, 6));
    (void)sub_8002EE50(r_u32(magnitude_pointer), xport_guest_ptr(second, 6), xport_guest_ptr(output, 12));
    return (uint32_t)shallow;
}

uint32_t sub_800B0510(uint32_t actor, uint32_t other, uint16_t output[3])
{
    uint32_t position_difference[3];
    uint32_t velocity_difference[3];
    uint32_t selected_axis = actor + 438;
    uint32_t axis;
    uint32_t pass;
    int negate = 0;

    FUNCTION_MARKER(0x800B0510, "RASHCDG.BIN");
    for (axis = 0; axis < 3; ++axis)
        position_difference[axis] = r_u32(other + 184 + 4 * axis) - r_u32(actor + 184 + 4 * axis);
    (void)sub_8002EE50(r_u32(other + 480), xport_guest_ptr(other + 450, 6), xport_guest_ptr(other + 456, 12));
    for (axis = 0; axis < 3; ++axis)
        velocity_difference[axis] = r_u32(other + 456 + 4 * axis) - r_u32(actor + 456 + 4 * axis);
    if ((int32_t)sub_8002E604(position_difference, velocity_difference) > 0)
        return 0;

    for (pass = 0; pass < 5; ++pass)
    {
        uint32_t projection = 0;

        if (pass == 2)
            for (axis = 0; axis < 3; ++axis)
                position_difference[axis] = r_u32(other + 184 + 4 * axis) - r_u32(actor + 196 + 4 * axis);
        selected_axis = actor + (pass == 0 || pass == 3 ? 444 : pass == 4 ? 438 : 432);
        for (axis = 0; axis < 3; ++axis)
            projection += (uint32_t)((int64_t)(int32_t)position_difference[axis] * (16 * r_s16(selected_axis + 2 * axis)) >> 16);
        if (pass < 2 && (int32_t)projection >= 0)
            break;
        if (pass >= 2 && pass < 4 && (int32_t)projection <= 0)
        {
            negate = 1;
            break;
        }
        if (pass == 4)
            negate = (int32_t)projection < 0;
    }
    xport_store_le16(output, r_u16(selected_axis));
    xport_store_le16(output + 1, r_u16(selected_axis + 2));
    if (negate)
    {
        uint16_t first = xport_load_le16(output);
        uint16_t third = r_u16(selected_axis + 4);

        xport_store_le16(output, (uint16_t)(0 - first));
        first = xport_load_le16(output + 1);
        xport_store_le16(output + 2, third);
        xport_store_le16(output + 2, (uint16_t)(0 - third));
        xport_store_le16(output + 1, (uint16_t)(0 - first));
    }
    else
        xport_store_le16(output + 2, r_u16(selected_axis + 4));
    return 1;
}

uint32_t sub_80017B30(int32_t value)
{
    FUNCTION_MARKER(0x80017B30, "SLUS_010.53");
    if (value < 0)
        value = 0;
    if (value > 51)
        value = 51;
    return r_u8(0x800525C0u + (uint32_t)value);
}

static uint32_t batch4_find_penetrating_point(uint32_t points, const uint16_t normal[3], uint32_t origin, uint32_t *distance)
{
    uint32_t selected = 8;
    int32_t minimum = 0;
    uint32_t i;

    for (i = 0; i < 8; ++i)
    {
        int32_t value = ((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(points + 12u * i, 12), normal, xport_guest_ptr(origin, 12))));

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

uint32_t sub_800B3344(uint32_t actor, uint32_t origin, const uint16_t normal[3], uint32_t mode)
{
    uint32_t distance = 0;
    uint32_t proceed = 1;
    uint32_t flags;

    FUNCTION_MARKER(0x800B3344, "RASHCDG.BIN");
    if (origin && batch4_find_penetrating_point(actor + 196, normal, origin, &distance) >= 8)
        return 0;
    if (mode)
    {
        uint32_t translation[3];

        (void)sub_8002EE50(distance, normal, translation);
        (void)sub_800A8DF0(xport_guest_ptr(actor, 1), translation, 1);
    }
    if (mode == 1)
    {
        flags = r_u32(actor + 592);
        if (((int32_t)(uint32_t)(r_u32(actor + 484))) <= 0x7FFF || (flags & 8u))
        {
            int32_t projection = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(actor + 450, 6), normal)));
            uint32_t i;

            if (projection < -58982)
                projection = -58982;
            if (flags & 0x10u)
            {
                w_u32(actor + 592, flags & 0xFFFFFFF5u);
                (void)sub_800A3CBC(actor);
                w_u32(actor + 484, 0x30000u);
            }
            else
            {
                if (!(flags & 8u))
                {
                    int32_t velocity = ((int32_t)(uint32_t)(r_u32(actor + 488)));

                    if (((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(actor + 566, 6), xport_guest_ptr(actor + 522, 6)))) < 0)
                        flags |= 0x40u;
                    if (r_u32(actor + 540))
                    {
                        uint32_t magnitude = velocity < 0 ? 0u - (uint32_t)velocity : (uint32_t)velocity;
                        int32_t response = ((int32_t)(uint32_t)(magnitude)) / 4;

                        if (response < 0x20000)
                            response = 0x20000;
                        if (((int32_t)(uint32_t)(r_u32(actor + 540))) > 0)
                            response = -response;
                        w_u32(actor + 544, (uint32_t)response);
                        if (velocity <= 0)
                        {
                            if (velocity > -327680)
                                velocity = -327680;
                        }
                        else if (velocity < 327680)
                        {
                            velocity = 327680;
                        }
                        w_u32(actor + 488, (uint32_t)velocity);
                    }
                    else
                    {
                        w_u32(actor + 544, 0);
                    }
                    flags |= 8u;
                    w_u32(actor + 592, flags);
                }
                projection /= 2;
            }
            (void)sub_8002EA20(xport_guest_ptr(actor + 450, 6), normal, -projection, xport_guest_ptr(actor + 456, 12));
            for (i = 0; i < 3; ++i)
                w_u16(actor + 450u + 2u * i, (uint16_t)(int16_t)(((int32_t)(uint32_t)(r_u32(actor + 456u + 4u * i))) >> 4));
            (void)sub_8002E468(xport_guest_ptr(actor + 450, 6));
            (void)sub_8002EE50(r_u32(actor + 480), xport_guest_ptr(actor + 450, 6), xport_guest_ptr(actor + 456, 12));
            proceed = 0;
        }
        else
        {
            w_u32(actor + 484, 0);
        }
    }
    if (proceed)
    {
        int32_t contact = (int32_t)sub_80084564(normal, actor + 450, actor + 480, actor + 456, 5, mode == 1 ? 0 : 393216, 0);

        if (contact >= 0)
        {
            int32_t first_random;
            int32_t second_random;
            int32_t velocity;
            int32_t speed;
            int32_t bound;
            int32_t target;
            int32_t cross;

            w_u16(actor + 578, 0x8000u);
            first_random = (int32_t)(sub_8001FC58() % 0xB2B8u) - 22876;
            second_random = (int32_t)(sub_8001FC58() % 0xB2B8u) - 22876;
            w_u32(actor + 544, (uint32_t)first_random << 1);
            w_u32(actor + 552, (uint32_t)second_random << 1);
            velocity = (int32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(actor + 488))), (int32_t)(sub_8001FC58() % 0x2667u) + 0xCCCC);
            if (velocity > 655360)
                velocity = 655360;
            w_u32(actor + 488, (uint32_t)velocity);
            speed = ((int32_t)(uint32_t)(r_u32(actor + 480)));
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
            w_u32(actor + 488, (uint32_t)velocity);
            cross = (int16_t)r_u16(actor + 560) * (int16_t)r_u16(actor + 454) - (int16_t)r_u16(actor + 564) * (int16_t)r_u16(actor + 450);
            if ((cross < 0 && velocity > 0) || (cross > 0 && velocity < 0))
                w_u32(actor + 488, 0u - (uint32_t)velocity);
        }
        (void)sub_80017BA0(((int32_t)(uint32_t)(r_u32(actor + 184))), ((int32_t)(uint32_t)(r_u32(actor + 192))), sub_80017B30(((int32_t)(uint32_t)(r_u32(actor + 180)))) + 14, 0);
    }
    return 1;
}

uint32_t sub_800B2F94(uint32_t actor, uint32_t other, const uint16_t normal[3])
{
    int32_t other_speed;
    uint32_t result;

    FUNCTION_MARKER(0x800B2F94, "RASHCDG.BIN");
    if (r_u32(actor + 592) & 2u)
        return sub_800B3344(actor, 0, normal, 0);

    other_speed = ((int32_t)(uint32_t)(r_u32(other + 480)));
    w_u32(actor + 480, (uint32_t)sub_8001FC90(72089, other_speed));
    if (other_speed <= 327680)
    {
        uint32_t i;

        for (i = 0; i < 3; ++i)
            w_u16(actor + 450u + 2u * i, r_u16(other + 450u + 2u * i));
        (void)sub_8002EE50(r_u32(actor + 480), xport_guest_ptr(actor + 450, 6), xport_guest_ptr(actor + 456, 12));
    }
    else
    {
        int32_t perturbation = (int16_t)((sub_8001FC58() % 0x2CAu) - 357u);
        int32_t other_x = (int16_t)r_u16(other + 450);
        int32_t other_y = (int16_t)r_u16(other + 452);
        int32_t other_z = (int16_t)r_u16(other + 454);
        int32_t angle;
        uint32_t kind;
        uint32_t angle_index;
        int32_t cosine;
        uint32_t magnitude = 406454u;
        int32_t velocity;
        int32_t direction;

        w_u16(actor + 450, (uint16_t)(other_x + ((perturbation * other_z) >> 12)));
        w_u16(actor + 452, (uint16_t)other_y);
        w_u16(actor + 454, (uint16_t)(other_z - ((perturbation * other_x) >> 12)));
        angle = ((int32_t)(uint32_t)(sub_8001FF3C((uint32_t)(other_y * 16))));
        kind = (r_u16(r_u32(actor) + 14) & 0xF80u) >> 7;
        angle_index = (uint32_t)(1479 - angle + ((kind == 2 || kind == 5) ? 57 : 0) - ((kind == 1 || kind == 4) ? 57 : 0)) & 4095u;
        cosine = (int16_t)r_u16(0x8005624Eu + 4u * angle_index);
        if (kind == 2 || kind == 5)
            magnitude += 0x15439u;
        if (kind == 1 || kind == 4)
            magnitude += 0xFFFE3A5Eu;
        (void)sub_8007E868(actor + 450, ((int32_t)(uint32_t)(r_u32(actor + 480))), cosine, ((int32_t)(uint32_t)(magnitude)));

        velocity = other_speed / 3;
        if (velocity > 655360)
            velocity = 655360;
        w_u32(actor + 488, (uint32_t)velocity);
        direction = (int16_t)((sub_8001FC58() % 0x2CAu) - 357u);
        w_u16(actor + 560, (uint16_t)((int16_t)r_u16(actor + 454) + ((direction * (int16_t)r_u16(actor + 450)) >> 12)));
        w_u16(actor + 562, (uint16_t)((sub_8001FC58() % 0x2CAu) - 357u));
        w_u16(actor + 564, (uint16_t)(((direction * (int16_t)r_u16(actor + 454)) >> 12) - (int16_t)r_u16(actor + 450)));
        w_u16(actor + 578, 0);
        (void)sub_8002E468(xport_guest_ptr(actor + 560, 6));
        w_u32(actor + 592, r_u32(actor + 592) | 3u);
    }

    result = sub_80017B30(((int32_t)(uint32_t)(r_u32(actor + 180))));
    (void)sub_80017BA0(((int32_t)(uint32_t)(r_u32(actor + 184))), ((int32_t)(uint32_t)(r_u32(actor + 192))), result, 0);
    return 1;
}

static int32_t batch4_floor_shift(int64_t value, uint32_t shift)
{
    int64_t divisor = INT64_C(1) << shift;

    return (int32_t)(value / divisor - (value < 0 && value % divisor != 0));
}

static void batch4_copy_short(uint32_t output, uint32_t input)
{
    uint32_t i;

    for (i = 0; i < 3; ++i)
        w_u16(output + 2u * i, r_u16(input + 2u * i));
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

static void batch4_normalize_xz(int16_t vector[3])
{
    uint32_t squared = (uint32_t)((int64_t)vector[0] * vector[0] + (int64_t)vector[2] * vector[2]);
    uint32_t shift = 22u - (batch4_leading_sign(squared) & ~1u);
    uint32_t entry;
    int32_t scale;
    uint32_t i;

    if (((int32_t)(uint32_t)(shift)) <= 0)
        shift = 0;
    entry = r_u16(r_u32(0x8005B560u) + 2u * (squared >> shift));
    scale = ((int32_t)(uint32_t)(((entry >> 5) << (entry & 31)) >> (shift >> 1)));
    for (i = 0; i < 3; ++i)
        vector[i] = (int16_t)batch4_floor_shift((int64_t)vector[i] * scale, 12);
}

uint32_t sub_80083928(uint32_t actor, uint32_t other, uint16_t normal[3])
{
    int32_t scalar_value_3;

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
    (void)sub_8002EE50(r_u32(other + 480), xport_guest_ptr(other + 450, 6), other_velocity);
    w_u32(actor + 564, r_u32(actor + 564) & ~0x4000u);
    if (!(r_u32(actor + 568) & 0x02000000u))
    {
        uint16_t cached_x = r_u16(actor + 450);
        uint16_t cached_y = r_u16(actor + 452);
        uint16_t cached_z = r_u16(actor + 454);
        uint32_t cached_flags = r_u32(actor + 568) | 0x02000000;
        uint32_t cached_speed = r_u32(actor + 480);

        w_u16(actor + 864, cached_x);
        w_u16(actor + 866, cached_y);
        w_u16(actor + 868, cached_z);
        w_u32(actor + 860, cached_speed);
        w_u32(actor + 568, cached_flags);
    }
    flags = r_u32(actor + 568);
    if ((flags & 0x200u) || ((flags & 0x400u) && ((scalar_value_3 = ((int16_t)xport_load_le16(normal + 1))), (((int32_t)(scalar_value_3 < 0 ? ((int32_t)(uint32_t)(0u - (uint32_t)scalar_value_3)) : scalar_value_3)))) >= 2048))
    {
        result = ((int32_t)(uint32_t)(sub_80084564(normal, actor + 864, actor + 860, actor + 456, (flags & 0x200u) ? 5 : 0, 0x30000, 0)));
        if (result >= 0)
            (void)sub_800849D8(actor);
        return (uint32_t)result;
    }

    (void)sub_8002EE50(r_u32(actor + 860), xport_guest_ptr(actor + 864, 6), xport_guest_ptr(actor + 456, 12));
    for (i = 0; i < 3; ++i)
    {
        difference[i] = ((int32_t)(uint32_t)(r_u32(actor + 456u + 4u * i) - other_velocity[i]));
        approach = ((int32_t)(uint32_t)((uint32_t)approach + (uint32_t)batch4_floor_shift((int64_t)difference[i] * (16 * (int16_t)xport_load_le16(normal + i)), 16)));
    }
    cached_dot = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(actor + 864, 6), normal)));
    axis_dot = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(actor + 814, 6), normal)));
    angle = (int32_t)(25736 * sub_80020018((uint32_t)axis_dot, 0u - (uint32_t)cached_dot)) >> 8;
    offset = (angle <= 0 ? -102943 : 102943) - angle;
    threshold = ((int32_t)(uint32_t)((uint32_t)offset ^ r_u32(actor + 488))) < 0 ? 74348 : 62910;
    cached_dot = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(actor + 864, 6), xport_guest_ptr(other + 450, 6))));
    if (approach >= 1)
        return 0;

    if (((int32_t)(((int32_t)(offset)) < 0 ? ((int32_t)(uint32_t)(0u - (uint32_t)((int32_t)(offset)))) : ((int32_t)(offset)))) > threshold)
    {
        int middle = approach < -1171888 && approach >= -2343777;
        int low_dot = cached_dot <= 50199;

        code = 2 * low_dot + (-middle & (4 - 2 * low_dot)) + (approach < -2343777 ? 8 - 2 * low_dot : 0);
        if (code)
        {
            if (r_u32(actor + 560) & 0x20000000u)
                code = 2;
        }
        else
        {
            code = -1;
        }
    }
    else
    {
        int32_t contact_dot = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(other + 450, 6), normal)));
        int middle = contact_dot > 45875 && approach < -439458 && approach >= -732430;
        int severe = contact_dot > 45875 && approach < -732430;

        code = (-middle & 0xFu) + (-severe & 0x1Fu) + 1;
        if (code == 32 && (r_u32(actor + 560) & 0x20000000u))
            code = 16;
        else if (code == 1)
            code = r_u32(actor + 828) != other + 172;
    }
    if (!code)
        return 0;

    flags = r_u32(actor + 568);
    if (flags & 0xFu)
    {
        w_u32(actor + 568, flags & 0xFFF007F0u);
        w_u32(actor + 828, 0);
        w_u32(actor + 568, (flags & 0xF7F007F0u) | 0x08000000u);
    }
    if (code >= 16 || code == -1)
    {
        int32_t first_mass = ((int32_t)(uint32_t)(r_u32(actor + 316)));
        int32_t second_mass = ((int32_t)(uint32_t)(r_u32(other + 316)));
        int32_t mass_sum = ((int32_t)(uint32_t)((uint32_t)first_mass + (uint32_t)second_mass));
        uint32_t denominator = (uint32_t)(mass_sum >> 1) + (uint32_t)((int32_t)((uint32_t)mass_sum - 2) >> 31);
        uint32_t reciprocal = denominator ? 0x80000000u / denominator : 0xFFFFFFFF;
        int32_t first_scale = (int32_t)sub_8001FC90(first_mass, ((int32_t)(uint32_t)(reciprocal)));
        int32_t second_scale = (int32_t)sub_8001FC90(second_mass, ((int32_t)(uint32_t)(reciprocal)));
        int32_t second_speed = (int32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(other + 480))), ((int32_t)(((int32_t)(cached_dot)) < 0 ? ((int32_t)(uint32_t)(0u - (uint32_t)((int32_t)(cached_dot)))) : ((int32_t)(cached_dot)))));
        uint32_t first_output;
        uint32_t second_output;

        (void)sub_80083864(first_scale, second_scale, ((int32_t)(uint32_t)(r_u32(actor + 480))), second_speed, &first_output, &second_output);
        collision_speed = (uint32_t)first_output;
        unused_speed = (uint32_t)second_output;
    }
    else
    {
        collision_speed = r_u32(actor + 860);
    }

    if (code == -1)
        w_u32(actor + 564, r_u32(actor + 564) | 0x4000u);
    else
        w_u32(actor + 568, r_u32(actor + 568) | (uint32_t)code);
    if (code >= 16)
    {
        result = ((int32_t)(uint32_t)(sub_80080B10(actor, actor + 864, actor + 860, other + 450, (int32_t)collision_speed)));
    }
    else
    {
        result = ((int32_t)(uint32_t)(sub_80080D1C(actor, actor + 864, actor + 860, other + 172, normal, (int32_t)sub_8001FC90(approach, 3664), offset, threshold, (int32_t)collision_speed, 1143, 0)));
    }
    flags = r_u32(actor + 568);
    flags |= (flags & 0x1Fu) ? 0x1000u : ((flags & 0x220u) != 0) << 11;
    w_u32(actor + 568, flags);
    if (flags & 0x130u)
        xport_update_u8(other + 509, XPORT_MEMORY_UPDATE_OR, 0x10u);
    return (uint32_t)result;
}

uint32_t sub_80083F30(uint32_t actor, uint32_t contact, uint16_t normal[3], int32_t mode)
{
    int32_t scalar_value_6;

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
    w_u32(actor + 564, r_u32(actor + 564) & ~0x4000u);
    if (!(r_u32(actor + 568) & 0x02000000u))
    {
        batch4_copy_short(actor + 864, actor + 450);
        w_u32(actor + 860, r_u32(actor + 480));
        w_u32(actor + 568, r_u32(actor + 568) | 0x02000000u);
    }
    flags = r_u32(actor + 568);
    if ((flags & 0x200u) || ((flags & 0x400u) && ((scalar_value_6 = ((int16_t)xport_load_le16((const uint8_t *)normal + 2))), (((int32_t)(scalar_value_6 < 0 ? ((int32_t)(uint32_t)(0u - (uint32_t)scalar_value_6)) : scalar_value_6)))) >= 2048))
    {
        result = ((int32_t)(uint32_t)(sub_80084564(normal, actor + 864, actor + 860, actor + 456, (flags & 0x200u) ? 5 : 0, 0x30000, 0)));
        if (result >= 0)
            (void)sub_800849D8(actor);
        return (uint32_t)result;
    }

    if (mode == 2)
    {
        offset = 0;
    }
    else
    {
        for (i = 0; i < 3; ++i)
            approach = ((int32_t)(uint32_t)((uint32_t)approach + (uint32_t)batch4_floor_shift((int64_t)((int32_t)(uint32_t)(r_u32(actor + 456u + 4u * i))) * (16 * (int16_t)xport_load_le16((const uint8_t *)normal + 2u * i)), 16)));
        if (mode >= 2)
        {
            if (r_u32(actor + 568) & 0x600u)
            {
                int16_t forward[3] = {(int16_t)r_u16(actor + 450), 0, (int16_t)r_u16(actor + 454)};
                int16_t side[3];

                batch4_normalize_xz(forward);
                side[0] = forward[2];
                side[1] = 0;
                side[2] = (int16_t)-forward[0];
                first_dot = ((int32_t)(uint32_t)(sub_8002E698((const uint16_t *)forward, normal)));
                offset = batch4_floor_shift((int64_t)25736 * ((int32_t)(uint32_t)(sub_80020018((uint32_t)((int32_t)(uint32_t)(sub_8002E698((const uint16_t *)side, normal))), 0u - (uint32_t)first_dot))), 8);
            }
            else
            {
                first_dot = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(actor + 864, 6), normal)));
                offset = batch4_floor_shift((int64_t)25736 * ((int32_t)(uint32_t)(sub_80020018(sub_8002E698(xport_guest_ptr(actor + 814, 6), normal), 0u - (uint32_t)first_dot))), 8);
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
        speed_offset = (int32_t)sub_8001FC90(655, ((int32_t)(uint32_t)(r_u32(actor + 860))));
    }
    else
    {
        int32_t index;

        threshold = ((int32_t)(uint32_t)((uint32_t)offset ^ r_u32(actor + 488))) < 0 ? 74348 : 62910;
        if (mode < 2)
        {
            uint32_t table_index;
            int32_t scale_a;
            int32_t scale_b;

            index = (int16_t)batch4_floor_shift((int64_t)163 * offset, 14);
            table_index = (uint32_t)index & 4095u;
            scale_b = (int16_t)r_u16(0x8005624Eu + 4u * table_index) * 16;
            if (index < 0)
                scale_b = -scale_b;
            scale_a = (int16_t)r_u16(0x8005624Cu + 4u * table_index) * 16;
            if (index > 0)
                scale_a = -scale_a;
            (void)sub_8002EB78(xport_guest_ptr(actor + 864, 6), xport_guest_ptr(actor + 814, 6), normal, (uint32_t)scale_a, (uint32_t)scale_b);
        }
        if (approach > 0)
            approach = 0;
        if (((int32_t)(((int32_t)(offset)) < 0 ? ((int32_t)(uint32_t)(0u - (uint32_t)((int32_t)(offset)))) : ((int32_t)(offset)))) <= threshold)
        {
            code = r_u32(actor + 828) != contact;
        }
        else
        {
            int active = approach < -1171888 && !(r_u32(actor + 560) & 0x20000000u);
            int severe = mode == 5 && approach < -2343777;

            code = (active && severe ? 6 : 0) + (active && !severe ? 2 : 0) + 2;
        }
    }
    if (!code)
        return 0;

    flags = r_u32(actor + 568);
    if (flags & 0xFu)
    {
        w_u32(actor + 568, flags & 0xFFF007F0u);
        w_u32(actor + 828, 0);
        w_u32(actor + 568, (flags & 0xF7F007F0u) | 0x08000000u);
    }
    if (code == -1)
        w_u32(actor + 564, r_u32(actor + 564) | 0x4000u);
    else
        w_u32(actor + 568, r_u32(actor + 568) | (uint32_t)code);
    result = ((int32_t)(uint32_t)(sub_80080D1C(actor, actor + 864, actor + 860, contact, normal, (int32_t)sub_8001FC90(approach, 3664), offset, threshold, ((int32_t)(uint32_t)(r_u32(actor + 860) - (uint32_t)speed_offset)), 1143, 1)));
    flags = r_u32(actor + 568);
    if (flags & 0x1Fu)
    {
        if (mode == 3 && (flags & 0xCu))
        {
            uint32_t owner = r_u32(actor + 852);

            w_u32(owner + 552, r_u32(owner + 552) | 0x10000u);
            if (r_u8(owner + 572) & 0x10u)
            {
                uint32_t linked_owner = r_u32(r_u32(actor + 856) + 852);

                w_u32(linked_owner + 552, r_u32(linked_owner + 552) | 0x10000u);
            }
        }
        flags |= 0x1000u;
    }
    else if (flags & 0x220u)
    {
        flags |= 0x800u;
    }
    w_u32(actor + 568, flags);
    return (uint32_t)result;
}
