#include "race_leaf_batch_003.h"
#include "fixed_math.h"
#include "race_pause.h"
#include "xport.h"

uint32_t sub_8008DBA8(uint32_t player)
{
    FUNCTION_MARKER(0x8008DBA8, "RASHCDG.BIN");
    return ((int32_t)(uint32_t)(r_u32(0x800CF650u))) < ((int32_t)(uint32_t)(r_u32(0x800D8714u + 4u * player)));
}

int32_t sub_8003A98C(int32_t direction, uint32_t actor)
{
    FUNCTION_MARKER(0x8003A98C, "SLUS_010.53");
    if (((int32_t)(uint32_t)(r_u32(actor + 480))) < 131 || (sub_8001FC58() & 1u))
        return (int32_t)(0u - (uint32_t)direction);
    return direction;
}

uint32_t sub_8009F578(uint32_t packet)
{
    uint32_t table = 0x800CE500u;
    uint32_t actor = r_u32(table);
    int32_t remaining = ((int32_t)(uint32_t)(r_u32(r_u32(table + 12))));
    uint32_t stride = r_u32(table + 4);
    uint32_t nearest = 0;
    int32_t best = 0x03E70000;

    FUNCTION_MARKER(0x8009F578, "RASHCDG.BIN");
    while (remaining >= 0)
    {
        if (r_u16(actor + 172) && r_u32(actor + 360) == r_u32(packet + 8))
        {
            uint32_t difference = r_u32(actor + 368) - r_u32(packet + 36);
            uint32_t sign = (uint32_t)(((int32_t)(uint32_t)(difference)) >> 31);
            int32_t distance = ((int32_t)(uint32_t)((difference + sign) ^ sign));

            if (distance <= 0x000A0000)
                return 1;
            if ((r_u8(actor + 509) & 0x11u) && ((int32_t)(uint32_t)(r_u32(actor + 364) ^ (uint32_t)(int32_t)r_s16(packet + 60))) >= 0 && distance < best)
            {
                best = distance;
                nearest = actor;
            }
        }
        --remaining;
        actor += stride;
    }
    if (nearest)
    {
        uint32_t record = r_u32(nearest + 372);

        if (!record)
            return 1;
        {
            int32_t kind = ((int32_t)(uint32_t)(r_u32(nearest + 364))) > 0 ? (int16_t)r_u16(nearest + 420) : (int16_t)r_u16(nearest + 408);
            int32_t state = r_s8(nearest + 508);
            int32_t next;

            if (kind == 1)
                return 1;
            if (kind == 2)
            {
                if (state == 2)
                {
                    uint32_t value = r_u32(nearest + 344);
                    uint32_t sign = (uint32_t)(((int32_t)(uint32_t)(value)) >> 31);

                    next = ((int32_t)(uint32_t)((value + sign) ^ sign)) < ((int32_t)(uint32_t)(r_u32(record + 4))) ? 3 : 1;
                }
                else
                    next = state == 3 ? 1 : 3;
            }
            else
            {
                next = state + 1;
                if (next >= 4)
                    next = 1;
            }
            w_u16(packet + 64, (uint16_t)next);
            return 2;
        }
    }
    return 0;
}

uint32_t sub_8009F478(uint32_t player, uint32_t actor, uint32_t route)
{
    int32_t first;
    int32_t second;
    int32_t mode;
    int32_t clamped;
    uint32_t index;

    FUNCTION_MARKER(0x8009F478, "RASHCDG.BIN");
    if (!r_u32(actor + 372))
        return 0;
    route &= 0xFFFF;
    index = route < 36u ? route : 36u;
    first = r_u8(0x800524F0u + 2u * index);
    second = r_u8(0x800524F1u + 2u * index);
    mode = ((int32_t)(uint32_t)(r_u32(r_u32(0x8005B2F8u) + 60)));
    if (mode > 0)
    {
        if (mode == 1)
        {
            first -= 2;
            second += 2;
        }
        if (mode == 2)
        {
            --first;
            ++second;
        }
        if (mode == 3)
            ++second;
    }
    if (r_u8(r_u32(0x8005B2F8u) + 4) & 0x10u)
    {
        --first;
        second += 4;
    }
    if (r_u8(r_u32(0x8005B2F8u) + 4) & 8u)
    {
        first -= 3;
        second += 2;
    }
    clamped = first < 0 ? 0 : first;
    if (clamped > 15)
        clamped = 15;
    w_u32(0x800D8714u + 4u * player, (uint32_t)clamped);
    if (second < 0)
        second = 0;
    w_u32(0x800D871Cu + 4u * player, (uint32_t)second);
    return (uint32_t)second;
}

uint32_t sub_8009F44C(uint32_t player)
{
    uint32_t actor = r_u32(0x8005B268u + 4u * player);

    FUNCTION_MARKER(0x8009F44C, "RASHCDG.BIN");
    uint32_t route = r_u32(actor + 360);

    if (route >> 16)
        return route >> 16;
    return sub_8009F478(player, actor, route);
}

uint32_t sub_8009FF2C(uint32_t active[2], uint32_t state)
{
    uint32_t side[2] = {0, 0};
    uint32_t first = r_u32(0x8005B38Cu);
    uint32_t second = r_u32(0x8005B21Cu);
    int32_t dx;
    int32_t dz;
    int32_t major;
    int32_t minor;
    int32_t estimate;
    int32_t count;
    int32_t index;

    FUNCTION_MARKER(0x8009FF2C, "RASHCDG.BIN");
    if (!(r_u8(state + 4) & 0x10u))
        return 0;
    dx = (int16_t)r_u16(first + 186) - (int16_t)r_u16(second + 186);
    dz = (int16_t)r_u16(first + 194) - (int16_t)r_u16(second + 194);
    if (dx < 0)
        dx = (int32_t)(0u - (uint32_t)dx);
    if (dz < 0)
        dz = (int32_t)(0u - (uint32_t)dz);
    major = dx >= dz ? dx : dz;
    minor = dx >= dz ? dz : dx;
    minor += minor >> 1;
    estimate = major - (major >> 5) - (major >> 7) + (minor >> 2) + (minor >> 6);
    if (estimate >= 260)
        return 0;

    count = ((int32_t)(uint32_t)(r_u32(state + 48)));
    for (index = 0; index < count && index < 2; ++index)
    {
        uint32_t reference = 0x800CD898u + 1132u * ((uint32_t)index ^ 1u);
        uint32_t actor = r_u32(0x8005B268u + 4u * (uint32_t)index);
        int32_t x = ((int32_t)(uint32_t)(r_u32(actor + 184) - r_u32(reference + 184)));
        int32_t y = ((int32_t)(uint32_t)(r_u32(actor + 188) - r_u32(reference + 188)));
        int32_t z = ((int32_t)(uint32_t)(r_u32(actor + 192) - r_u32(reference + 192)));
        int32_t nx = (int16_t)r_u16(reference + 444) * 16;
        int32_t ny = (int16_t)r_u16(reference + 446) * 16;
        int32_t nz = (int16_t)r_u16(reference + 448) * 16;
        int32_t dot = (int32_t)(((int64_t)x * nx) >> 16);

        dot = ((int32_t)(uint32_t)((uint32_t)dot + (uint32_t)(int32_t)(((int64_t)y * ny) >> 16)));
        dot = ((int32_t)(uint32_t)((uint32_t)dot + (uint32_t)(int32_t)(((int64_t)z * nz) >> 16)));
        side[index] = dot >= 0;
    }
    if (side[0] == side[1])
    {
        for (index = 0; index < count && index < 2; ++index)
            if (active[index])
                active[index] = (uint32_t)index + 2u;
    }
    else if (side[0])
    {
        if (active[0])
            active[0] = 2;
        if (active[1])
            active[1] = 3;
    }
    else
    {
        if (active[1])
            active[1] = 2;
        if (active[0])
            active[0] = 3;
    }
    return 0;
}

uint32_t sub_8009FF24(uint32_t active[2])
{
    FUNCTION_MARKER(0x8009FF24, "RASHCDG.BIN");
    return sub_8009FF2C(active, r_u32(0x8005B2F8u));
}

uint32_t sub_800C45D8(uint32_t object, uint32_t *flags_out)
{
    uint32_t type = r_u16(object + 544);
    uint32_t result;

    FUNCTION_MARKER(0x800C45D8, "RASHCDG.BIN");
    xport_store_le32(flags_out, 16);
    switch (type)
    {
        case 0:
            return 4;
        case 2:
        case 22:
            xport_store_le32(flags_out, 3);
            return 11;
        case 3:
            return 7;
        case 6:
        {
            uint32_t linked = r_u32(object + 596);

            return ((int32_t)(uint32_t)(r_u32(linked + 480))) > 0x7FFF ? 7 : type;
        }
        case 9:
        case 10:
        case 76:
            result = 8;
            if (!(r_u8(object + 572) & 0x20u))
            {
                uint32_t linked = r_u32(object + 596);

                if (((int32_t)(uint32_t)(r_u32(linked + 480))) <= 0x7FFF)
                {
                    uint32_t state = r_u32(0x8005B2F8u);

                    result = 5;
                    if (r_u16(linked + 172) < r_u32(state + 48))
                    {
                        uint32_t descriptor = r_u32(linked + 1084);

                        if (r_u32(descriptor + 40) && r_u8(descriptor + 39) < 4u && !r_u8(state + 57))
                            result = 1;
                    }
                }
            }
            xport_store_le32(flags_out, xport_load_le32(flags_out) | 2u);
            return result;
        case 12:
        case 13:
        case 17:
        case 23:
            xport_store_le32(flags_out, 1);
            return 13;
        case 45:
        case 53:
        case 57:
            xport_store_le32(flags_out, 3);
            return 51;
        case 50:
        case 51:
        case 71:
        case 79:
            return type;
        case 52:
        case 56:
        case 58:
            xport_store_le32(flags_out, 3);
            return 50;
        case 54:
        case 59:
            result = 49;
            break;
        case 60:
        case 63:
            xport_store_le32(flags_out, 4096);
            result = 69;
            break;
        case 67:
            result = 69;
            break;
        case 61:
        case 62:
            xport_store_le32(flags_out, 4096);
            result = 70;
            break;
        case 68:
            result = 70;
            break;
        case 69:
        case 70:
            result = 71;
            break;
        case 78:
            return 79;
        default:
            if ((r_u16(object + 172) >> 5) != 1)
                return 48;
            if (r_u32(object + 604) < 2u && (r_u8(object + 572) & 0x20u))
                return 77;
            return r_u16(0x800CCBA0u + 2u * r_u32(object + 604));
    }
    xport_store_le32(flags_out, xport_load_le32(flags_out) | 2u);
    return result;
}

uint32_t sub_800A3ECC(uint32_t object, uint32_t index)
{
    uint32_t record = 0x800D43C0u + 28u * index;
    uint32_t result = (uint32_t)(int32_t)r_s16(record + 22);

    FUNCTION_MARKER(0x800A3ECC, "RASHCDG.BIN");
    if ((int16_t)result)
    {
        result = r_u32(record + 24);
        if (result == object)
        {
            w_u16(record + 22, 0);
            w_u32(record + 24, 0);
        }
    }
    return result;
}

static int32_t batch3_asr(int32_t value, uint32_t shift)
{
    uint32_t bits = (uint32_t)value;

    if (!shift)
        return value;
    bits >>= shift;
    if (value < 0)
        bits |= ~0u << (32u - shift);
    return ((int32_t)(uint32_t)(bits));
}

static int32_t batch3_ratio(int32_t numerator, int32_t denominator)
{
    uint32_t numerator_magnitude = numerator < 0 ? 0u - (uint32_t)numerator : (uint32_t)numerator;
    uint32_t denominator_magnitude = denominator < 0 ? 0u - (uint32_t)denominator : (uint32_t)denominator;
    uint32_t result = sub_80010028(numerator_magnitude, denominator_magnitude);

    if ((numerator < 0) != (denominator < 0))
        result = 0u - result;
    return ((int32_t)(uint32_t)(result));
}

static int32_t batch3_blend_quaternion(uint32_t amount, const int16_t first[4], const int16_t second[4], int16_t output[4])
{
    int32_t dot = 0;
    int32_t scaled_dot;
    uint32_t magnitude;
    uint32_t remaining = 0x10000u - amount;
    int32_t first_weight = ((int32_t)(uint32_t)(remaining));
    int32_t second_weight = ((int32_t)(uint32_t)(amount));
    int negative;
    uint32_t i;

    for (i = 0; i < 4; ++i)
        dot = ((int32_t)(uint32_t)((uint32_t)dot + (uint32_t)((int32_t)(((int32_t)(uint32_t)((uint32_t)((int64_t)((int32_t)(first[i])) * ((int32_t)(second[i])))))))));
    scaled_dot = (int32_t)((uint32_t)batch3_asr(dot, 14) << 2);
    negative = scaled_dot < 0;
    magnitude = negative ? 0u - (uint32_t)scaled_dot : (uint32_t)scaled_dot;
    if (((int32_t)(uint32_t)(0x10000u - magnitude)) > 16)
    {
        uint32_t angle = 1024u - sub_8001FF3C(magnitude);
        int32_t denominator = (int32_t)r_s16(0x8005624Cu + 4u * (((uint32_t)(angle)) & 0xFFFu)) << 4;
        uint32_t first_angle = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(remaining)), ((int32_t)(uint32_t)(angle)));
        uint32_t second_angle = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(amount)), ((int32_t)(uint32_t)(angle)));

        first_weight = batch3_ratio((int32_t)r_s16(0x8005624Cu + 4u * (((uint32_t)(first_angle)) & 0xFFFu)) << 4, denominator);
        second_weight = batch3_ratio((int32_t)r_s16(0x8005624Cu + 4u * (((uint32_t)(second_angle)) & 0xFFFu)) << 4, denominator);
    }
    if (negative)
        second_weight = ((int32_t)(uint32_t)(0u - (uint32_t)second_weight));
    first_weight = batch3_asr(((int32_t)(uint32_t)((uint32_t)first_weight << 14)), 16);
    second_weight = batch3_asr(((int32_t)(uint32_t)((uint32_t)second_weight << 14)), 16);
    for (i = 0; i < 4; ++i)
    {
        int32_t first_term = batch3_asr(((int32_t)(((int32_t)(uint32_t)((uint32_t)((int64_t)((int32_t)(first_weight)) * ((int32_t)(first[i]))))))), 14);
        int32_t second_term = batch3_asr(((int32_t)(((int32_t)(uint32_t)((uint32_t)((int64_t)((int32_t)(second_weight)) * ((int32_t)(second[i]))))))), 14);

        output[i] = (int16_t)((int32_t)(uint32_t)((uint32_t)first_term + (uint32_t)second_term));
    }
    return output[3];
}

uint32_t sub_800710C0(uint32_t amount, uint32_t first, uint32_t second, uint32_t output)
{
    int16_t first_values[4];
    int16_t second_values[4];
    int16_t output_values[4];
    int32_t result;
    uint32_t i;

    FUNCTION_MARKER(0x800710C0, "RASHCDG.BIN");
    for (i = 0; i < 4; ++i)
    {
        first_values[i] = (int16_t)r_u16(first + 2u * i);
        second_values[i] = (int16_t)r_u16(second + 2u * i);
    }
    result = batch3_blend_quaternion(amount, first_values, second_values, output_values);
    for (i = 0; i < 4; ++i)
        w_u16(output + 2u * i, (uint16_t)output_values[i]);
    return (uint32_t)result;
}

uint32_t sub_8005D36C(uint32_t actor)
{
    uint32_t segment = r_u32(actor + 4) + 12u * r_u32(actor + 12);
    int32_t segment_length = (int16_t)r_u16(segment + 4);
    int32_t amount = ((int32_t)(uint32_t)(r_u32(actor + 16) << 16)) / segment_length;
    int16_t position[3];
    uint32_t controller;
    uint32_t model;
    uint32_t count;
    uint32_t i;

    FUNCTION_MARKER(0x8005D36C, "RASHCDG.BIN");
    if (r_u32(actor + 36) & 4u)
    {
        int32_t divisor = ((int32_t)(((int32_t)(uint32_t)((uint32_t)((int64_t)((int32_t)(((int32_t)(uint32_t)(r_u32(actor + 24))))) * ((int32_t)(segment_length)))))));
        int32_t numerator = ((int32_t)(uint32_t)(r_u32(actor + 32) << 16));

        amount = ((int32_t)(uint32_t)((uint32_t)amount + (uint32_t)(numerator / divisor)));
    }
    for (i = 0; i < 3; ++i)
    {
        int32_t first = (int32_t)sub_8001FC90(((int32_t)(uint32_t)(0x10000u - (uint32_t)amount)), (int16_t)r_u16(actor + 0x6F0u + 2u * i));
        int32_t second = (int32_t)sub_8001FC90(amount, (int16_t)r_u16(actor + 0x6F6u + 2u * i));

        position[i] = (int16_t)((int32_t)(uint32_t)((uint32_t)first + (uint32_t)second));
    }
    controller = r_u32(actor);
    model = r_u32(controller);
    if ((r_u16(model + 14) & 1u) && !r_u32(controller + 52))
    {
        int32_t scale = ((int32_t)(uint32_t)(r_u32(model + 20)));

        for (i = 0; i < 3; ++i)
        {
            int32_t component = ((int32_t)(uint32_t)((uint32_t)(int32_t)position[i] << 12));

            position[i] = (int16_t)batch3_asr(((int32_t)(((int32_t)(uint32_t)((uint32_t)((int64_t)((int32_t)(component)) * ((int32_t)(scale))))))), 24);
        }
    }
    if (r_u8(segment + 2) & 0x20u)
    {
        uint32_t model_id = r_u16(controller + 0x220u);
        uint32_t type = r_u16(0x800541D4u + 8u * model_id + 2u);

        position[0] = 0;
        position[2] = 0;
        if (type != 8u && (model_id - 69u) >= 2u)
            position[1] = 0;
    }
    w_u16(controller + 28, (uint16_t)position[0]);
    w_u16(controller + 30, (uint16_t)position[1]);
    w_u16(controller + 32, (uint16_t)position[2]);

    count = r_u8(actor + 0x6E4u);
    for (i = 0; i < count; ++i)
    {
        if ((r_u32(actor + 0x6ECu) >> (i & 31u)) & 1u)
        {
            uint32_t first_address = actor + 0x6FCu + 16u * i;
            int16_t first_values[4];
            int16_t second_values[4];
            int16_t blended[4];
            int32_t quaternion[4];
            uint32_t matrix = r_u32(controller + 4) + 24u * i + 4u;
            uint32_t j;

            for (j = 0; j < 4; ++j)
            {
                first_values[j] = (int16_t)r_u16(first_address + 2u * j);
                second_values[j] = (int16_t)r_u16(first_address + 8u + 2u * j);
            }
            (void)batch3_blend_quaternion((uint32_t)amount, first_values, second_values, blended);
            for (j = 0; j < 4; ++j)
                quaternion[j] = ((int32_t)(uint32_t)((uint32_t)(int32_t)blended[j] << 2));
            (void)sub_8001005C(xport_guest_ptr(matrix, 18), quaternion);
        }
    }
    return 0;
}

uint32_t sub_800B6BD0(const uint32_t first[3], const uint16_t normal[3], const uint16_t plane[3], const uint32_t edge[3])
{
    int64_t scalar_product_9;
    int64_t scalar_product_10;
    int64_t scalar_product_11;
    int64_t scalar_product_12;
    int64_t scalar_product_13;
    int64_t scalar_product_14;

    uint32_t dot;
    uint32_t component[3];
    uint32_t first_dot;
    uint32_t edge_dot;
    uint32_t delta;
    uint32_t denominator;

    FUNCTION_MARKER(0x800B6BD0, "RASHCDG.BIN");
    dot = sub_8002E698(plane, normal);
    if (((int32_t)(uint32_t)(dot)) > -64 && ((int32_t)(uint32_t)(dot)) < 64)
        return 0x7FFF0000u;
    component[0] = (uint32_t)(int32_t)(int16_t)xport_load_le16((const uint8_t *)plane) << 4;
    first_dot = ((scalar_product_9 = (int64_t)((int32_t)(uint32_t)(((uint32_t)(xport_load_le32(first))))) * (int64_t)((int32_t)(uint32_t)(((uint32_t)(component[0]))))), (((uint32_t)((uint32_t)((uint64_t)scalar_product_9 >> 16)))));
    component[1] = (uint32_t)(int32_t)(int16_t)xport_load_le16((const uint8_t *)plane + 2) << 4;
    first_dot += ((scalar_product_10 = (int64_t)((int32_t)(uint32_t)(((uint32_t)(xport_load_le32(first + 1))))) * (int64_t)((int32_t)(uint32_t)(((uint32_t)(component[1]))))), (((uint32_t)((uint32_t)((uint64_t)scalar_product_10 >> 16)))));
    component[2] = (uint32_t)(int32_t)(int16_t)xport_load_le16((const uint8_t *)plane + 4) << 4;
    first_dot += ((scalar_product_11 = (int64_t)((int32_t)(uint32_t)(((uint32_t)(xport_load_le32(first + 2))))) * (int64_t)((int32_t)(uint32_t)(((uint32_t)(component[2]))))), (((uint32_t)((uint32_t)((uint64_t)scalar_product_11 >> 16)))));
    edge_dot = ((scalar_product_12 = (int64_t)((int32_t)(uint32_t)(((uint32_t)(xport_load_le32(edge))))) * (int64_t)((int32_t)(uint32_t)(((uint32_t)(component[0]))))), (((uint32_t)((uint32_t)((uint64_t)scalar_product_12 >> 16)))));
    edge_dot += ((scalar_product_13 = (int64_t)((int32_t)(uint32_t)(((uint32_t)(xport_load_le32(edge + 1))))) * (int64_t)((int32_t)(uint32_t)(((uint32_t)(component[1]))))), (((uint32_t)((uint32_t)((uint64_t)scalar_product_13 >> 16)))));
    edge_dot += ((scalar_product_14 = (int64_t)((int32_t)(uint32_t)(((uint32_t)(xport_load_le32(edge + 2))))) * (int64_t)((int32_t)(uint32_t)(((uint32_t)(component[2]))))), (((uint32_t)((uint32_t)((uint64_t)scalar_product_14 >> 16)))));
    delta = first_dot - edge_dot;
    denominator = 0u - dot;
    if (((int32_t)(uint32_t)(delta)) > 0)
    {
        if (((int32_t)(uint32_t)(denominator)) > 0)
            return sub_80010028(delta, denominator);
        return 0u - sub_80010028(delta, dot);
    }
    delta = 0u - delta;
    if (((int32_t)(uint32_t)(denominator)) > 0)
        return 0u - sub_80010028(delta, denominator);
    return sub_80010028(delta, dot);
}
