#include "race_leaf_batch_003.h"
#include "fixed_math.h"
#include "race_pause.h"
#include "xport.h"

uint32_t sub_8008DBA8(RRJMemory *m, uint32_t player)
{
    FUNCTION_MARKER(0x8008DBA8, "RASHCDG.BIN");
    return rrj_s32(rrj_read32(m, 0x800CF650u)) <
           rrj_s32(rrj_read32(m, 0x800D8714u + 4u * player));
}

int32_t sub_8003A98C(RRJMemory *m, int32_t direction, uint32_t actor)
{
    FUNCTION_MARKER(0x8003A98C, "SLUS_010.53");
    if (rrj_s32(rrj_read32(m, actor + 480)) < 131 ||
        (sub_8001FC58(m) & 1u))
        return (int32_t)(0u - (uint32_t)direction);
    return direction;
}

uint32_t sub_8009F578(RRJMemory *m, uint32_t packet)
{
    uint32_t table = 0x800CE500u;
    uint32_t actor = rrj_read32(m, table);
    int32_t remaining = rrj_s32(rrj_read32(m, rrj_read32(m, table + 12)));
    uint32_t stride = rrj_read32(m, table + 4);
    uint32_t nearest = 0;
    int32_t best = 0x03E70000;

    FUNCTION_MARKER(0x8009F578, "RASHCDG.BIN");
    while (remaining >= 0)
    {
        if (rrj_u16(rrj_at(m, actor + 172, 2)) &&
            rrj_read32(m, actor + 360) == rrj_read32(m, packet + 8))
        {
            uint32_t difference = rrj_read32(m, actor + 368) -
                                  rrj_read32(m, packet + 36);
            uint32_t sign = (uint32_t)(rrj_s32(difference) >> 31);
            int32_t distance = rrj_s32((difference + sign) ^ sign);

            if (distance <= 0x000A0000)
                return 1;
            if ((*(uint8_t *)rrj_at(m, actor + 509, 1) & 0x11u) &&
                rrj_s32(rrj_read32(m, actor + 364) ^
                        (uint32_t)(int32_t)(int16_t)rrj_u16(
                            rrj_at(m, packet + 60, 2))) >= 0 &&
                distance < best)
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
        uint32_t record = rrj_read32(m, nearest + 372);

        if (!record)
            return 1;
        {
            int32_t kind = rrj_s32(rrj_read32(m, nearest + 364)) > 0
                               ? (int16_t)rrj_u16(
                                     rrj_at(m, nearest + 420, 2))
                               : (int16_t)rrj_u16(
                                     rrj_at(m, nearest + 408, 2));
            int32_t state = *(int8_t *)rrj_at(m, nearest + 508, 1);
            int32_t next;

            if (kind == 1)
                return 1;
            if (kind == 2)
            {
                if (state == 2)
                {
                    uint32_t value = rrj_read32(m, nearest + 344);
                    uint32_t sign = (uint32_t)(rrj_s32(value) >> 31);

                    next = rrj_s32((value + sign) ^ sign) <
                                   rrj_s32(rrj_read32(m, record + 4))
                               ? 3
                               : 1;
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
            rrj_put16(rrj_at(m, packet + 64, 2), (uint16_t)next);
            return 2;
        }
    }
    return 0;
}

uint32_t sub_8009F478(RRJMemory *m, uint32_t player, uint32_t actor,
                      uint32_t route)
{
    int32_t first;
    int32_t second;
    int32_t mode;
    int32_t clamped;
    uint32_t index;

    FUNCTION_MARKER(0x8009F478, "RASHCDG.BIN");
    if (!rrj_read32(m, actor + 372))
        return 0;
    index = route < 36u ? route : 36u;
    first = *(uint8_t *)rrj_at(m, 0x800524F0u + 2u * index, 1);
    second = *(uint8_t *)rrj_at(m, 0x800524F1u + 2u * index, 1);
    mode = rrj_s32(rrj_read32(m, rrj_read32(m, 0x8005B2F8u) + 60));
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
    if (*(uint8_t *)rrj_at(m, rrj_read32(m, 0x8005B2F8u) + 4, 1) &
        0x10u)
    {
        --first;
        second += 4;
    }
    if (*(uint8_t *)rrj_at(m, rrj_read32(m, 0x8005B2F8u) + 4, 1) & 8u)
    {
        first -= 3;
        second += 2;
    }
    clamped = first < 0 ? 0 : first;
    if (clamped > 15)
        clamped = 15;
    rrj_write32(m, 0x800D8714u + 4u * player, (uint32_t)clamped);
    if (second < 0)
        second = 0;
    rrj_write32(m, 0x800D871Cu + 4u * player, (uint32_t)second);
    return (uint32_t)second;
}

uint32_t sub_8009F44C(RRJMemory *m, uint32_t player)
{
    uint32_t actor = rrj_read32(m, 0x8005B268u + 4u * player);

    FUNCTION_MARKER(0x8009F44C, "RASHCDG.BIN");
    return sub_8009F478(m, player, actor, rrj_read32(m, actor + 360));
}

uint32_t sub_8009FF2C(RRJMemory *m, uint32_t active[2], uint32_t state)
{
    uint32_t side[2] = {0, 0};
    uint32_t first = rrj_read32(m, 0x8005B38Cu);
    uint32_t second = rrj_read32(m, 0x8005B21Cu);
    int32_t dx;
    int32_t dz;
    int32_t major;
    int32_t minor;
    int32_t estimate;
    int32_t count;
    int32_t index;

    FUNCTION_MARKER(0x8009FF2C, "RASHCDG.BIN");
    if (!(*(uint8_t *)rrj_at(m, state + 4, 1) & 0x10u))
        return 0;
    dx = (int16_t)rrj_u16(rrj_at(m, first + 186, 2)) -
         (int16_t)rrj_u16(rrj_at(m, second + 186, 2));
    dz = (int16_t)rrj_u16(rrj_at(m, first + 194, 2)) -
         (int16_t)rrj_u16(rrj_at(m, second + 194, 2));
    if (dx < 0)
        dx = (int32_t)(0u - (uint32_t)dx);
    if (dz < 0)
        dz = (int32_t)(0u - (uint32_t)dz);
    major = dx >= dz ? dx : dz;
    minor = dx >= dz ? dz : dx;
    minor += minor >> 1;
    estimate = major - (major >> 5) - (major >> 7) +
               (minor >> 2) + (minor >> 6);
    if (estimate >= 260)
        return 0;

    count = rrj_s32(rrj_read32(m, state + 48));
    for (index = 0; index < count && index < 2; ++index)
    {
        uint32_t reference = 0x800CD898u +
                             1132u * ((uint32_t)index ^ 1u);
        uint32_t actor = rrj_read32(
            m, 0x8005B268u + 4u * (uint32_t)index);
        int32_t x = rrj_s32(rrj_read32(m, actor + 184) -
                            rrj_read32(m, reference + 184));
        int32_t y = rrj_s32(rrj_read32(m, actor + 188) -
                            rrj_read32(m, reference + 188));
        int32_t z = rrj_s32(rrj_read32(m, actor + 192) -
                            rrj_read32(m, reference + 192));
        int32_t nx = (int16_t)rrj_u16(rrj_at(m, reference + 444, 2)) *
                     16;
        int32_t ny = (int16_t)rrj_u16(rrj_at(m, reference + 446, 2)) *
                     16;
        int32_t nz = (int16_t)rrj_u16(rrj_at(m, reference + 448, 2)) *
                     16;
        int32_t dot = (int32_t)(((int64_t)x * nx) >> 16);

        dot = rrj_s32((uint32_t)dot +
                      (uint32_t)(int32_t)(((int64_t)y * ny) >> 16));
        dot = rrj_s32((uint32_t)dot +
                      (uint32_t)(int32_t)(((int64_t)z * nz) >> 16));
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

uint32_t sub_8009FF24(RRJMemory *m, uint32_t active[2])
{
    FUNCTION_MARKER(0x8009FF24, "RASHCDG.BIN");
    return sub_8009FF2C(m, active, rrj_read32(m, 0x8005B2F8u));
}

uint32_t sub_800C45D8(RRJMemory *m, uint32_t object, uint32_t flags_out)
{
    uint32_t type = rrj_u16(rrj_at(m, object + 544, 2));
    uint32_t result;

    FUNCTION_MARKER(0x800C45D8, "RASHCDG.BIN");
    rrj_write32(m, flags_out, 16);
    switch (type)
    {
        case 0:
            return 4;
        case 2:
        case 22:
            rrj_write32(m, flags_out, 3);
            return 11;
        case 3:
            return 7;
        case 6:
        {
            uint32_t linked = rrj_read32(m, object + 596);

            return rrj_s32(rrj_read32(m, linked + 480)) > 0x7FFF ? 7 : type;
        }
        case 9:
        case 10:
        case 76:
            result = 8;
            if (!(*(uint8_t *)rrj_at(m, object + 572, 1) & 0x20u))
            {
                uint32_t linked = rrj_read32(m, object + 596);

                if (rrj_s32(rrj_read32(m, linked + 480)) <= 0x7FFF)
                {
                    uint32_t state = rrj_read32(m, 0x8005B2F8u);

                    result = 5;
                    if (rrj_u16(rrj_at(m, linked + 172, 2)) <
                        rrj_read32(m, state + 48))
                    {
                        uint32_t descriptor = rrj_read32(m, linked + 1084);

                        if (rrj_read32(m, descriptor + 40) &&
                            *(uint8_t *)rrj_at(m, descriptor + 39, 1) < 4u &&
                            !*(uint8_t *)rrj_at(m, state + 57, 1))
                            result = 1;
                    }
                }
            }
            rrj_write32(m, flags_out, rrj_read32(m, flags_out) | 2u);
            return result;
        case 12:
        case 13:
        case 17:
        case 23:
            rrj_write32(m, flags_out, 1);
            return 13;
        case 45:
        case 53:
        case 57:
            rrj_write32(m, flags_out, 3);
            return 51;
        case 50:
        case 51:
        case 71:
        case 79:
            return type;
        case 52:
        case 56:
        case 58:
            rrj_write32(m, flags_out, 3);
            return 50;
        case 54:
        case 59:
            result = 49;
            break;
        case 60:
        case 63:
            rrj_write32(m, flags_out, 4096);
            result = 69;
            break;
        case 67:
            result = 69;
            break;
        case 61:
        case 62:
            rrj_write32(m, flags_out, 4096);
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
            if ((rrj_u16(rrj_at(m, object + 172, 2)) >> 5) != 1)
                return 48;
            if (rrj_read32(m, object + 604) < 2u &&
                (*(uint8_t *)rrj_at(m, object + 572, 1) & 0x20u))
                return 77;
            return rrj_u16(rrj_at(m, 0x800CCBA0u +
                                      2u * rrj_read32(m, object + 604),
                                  2));
    }
    rrj_write32(m, flags_out, rrj_read32(m, flags_out) | 2u);
    return result;
}

uint32_t sub_800A3ECC(RRJMemory *m, uint32_t object, uint32_t index)
{
    uint32_t record = 0x800D43C0u + 28u * index;
    uint32_t result = (uint32_t)(int32_t)(int16_t)rrj_u16(
        rrj_at(m, record + 22, 2));

    FUNCTION_MARKER(0x800A3ECC, "RASHCDG.BIN");
    if ((int16_t)result)
    {
        result = rrj_read32(m, record + 24);
        if (result == object)
        {
            rrj_put16(rrj_at(m, record + 22, 2), 0);
            rrj_write32(m, record + 24, 0);
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
    return rrj_s32(bits);
}

static int32_t batch3_mul_low(int32_t first, int32_t second)
{
    return rrj_s32((uint32_t)((int64_t)first * second));
}

static int32_t batch3_sine(RRJMemory *m, uint32_t angle)
{
    return (int16_t)rrj_u16(
        rrj_at(m, 0x8005624Cu + 4u * (angle & 0xFFFu), 2));
}

static int32_t batch3_ratio(int32_t numerator, int32_t denominator)
{
    uint32_t numerator_magnitude = numerator < 0
                                       ? 0u - (uint32_t)numerator
                                       : (uint32_t)numerator;
    uint32_t denominator_magnitude = denominator < 0
                                         ? 0u - (uint32_t)denominator
                                         : (uint32_t)denominator;
    uint32_t result = sub_80010028(numerator_magnitude,
                                   denominator_magnitude);

    if ((numerator < 0) != (denominator < 0))
        result = 0u - result;
    return rrj_s32(result);
}

static int32_t batch3_blend_quaternion(RRJMemory *m, uint32_t amount,
                                       const int16_t first[4],
                                       const int16_t second[4],
                                       int16_t output[4])
{
    int32_t dot = 0;
    int32_t scaled_dot;
    uint32_t magnitude;
    uint32_t remaining = 0x10000u - amount;
    int32_t first_weight = rrj_s32(remaining);
    int32_t second_weight = rrj_s32(amount);
    int negative;
    uint32_t i;

    for (i = 0; i < 4; ++i)
        dot = rrj_s32((uint32_t)dot +
                      (uint32_t)batch3_mul_low(first[i], second[i]));
    scaled_dot = rrj_s32((uint32_t)dot << 2);
    negative = scaled_dot < 0;
    magnitude = negative ? 0u - (uint32_t)scaled_dot
                         : (uint32_t)scaled_dot;
    if (rrj_s32(0x10000u - magnitude) > 16)
    {
        uint32_t angle = 1024u - sub_8001FF3C(m, remaining);
        int32_t denominator = batch3_sine(m, angle) << 4;
        uint32_t first_angle =
            (uint32_t)sub_8001FC90(rrj_s32(remaining),
                                   rrj_s32(angle));
        uint32_t second_angle =
            (uint32_t)sub_8001FC90(rrj_s32(amount), rrj_s32(angle));

        first_weight = batch3_ratio(batch3_sine(m, first_angle) << 4,
                                    denominator);
        second_weight = batch3_ratio(batch3_sine(m, second_angle) << 4,
                                     denominator);
    }
    if (negative)
        second_weight = rrj_s32(0u - (uint32_t)second_weight);
    first_weight = batch3_asr(rrj_s32((uint32_t)first_weight << 14), 16);
    second_weight = batch3_asr(rrj_s32((uint32_t)second_weight << 14), 16);
    for (i = 0; i < 4; ++i)
    {
        int32_t first_term = batch3_asr(
            batch3_mul_low(first_weight, first[i]), 14);
        int32_t second_term = batch3_asr(
            batch3_mul_low(second_weight, second[i]), 14);

        output[i] = (int16_t)rrj_s32((uint32_t)first_term +
                                     (uint32_t)second_term);
    }
    return output[3];
}

uint32_t sub_800710C0(RRJMemory *m, uint32_t amount, uint32_t first,
                      uint32_t second, uint32_t output)
{
    int16_t first_values[4];
    int16_t second_values[4];
    int16_t output_values[4];
    int32_t result;
    uint32_t i;

    FUNCTION_MARKER(0x800710C0, "RASHCDG.BIN");
    for (i = 0; i < 4; ++i)
    {
        first_values[i] =
            (int16_t)rrj_u16(rrj_at(m, first + 2u * i, 2));
        second_values[i] =
            (int16_t)rrj_u16(rrj_at(m, second + 2u * i, 2));
    }
    result = batch3_blend_quaternion(m, amount, first_values, second_values,
                                     output_values);
    for (i = 0; i < 4; ++i)
        rrj_put16(rrj_at(m, output + 2u * i, 2),
                  (uint16_t)output_values[i]);
    return (uint32_t)result;
}

uint32_t sub_8005D36C(RRJMemory *m, uint32_t actor)
{
    uint32_t segment = rrj_read32(m, actor + 4) +
                       12u * rrj_read32(m, actor + 12);
    int32_t segment_length =
        (int16_t)rrj_u16(rrj_at(m, segment + 4, 2));
    int32_t amount = rrj_s32(rrj_read32(m, actor + 16) << 16) /
                     segment_length;
    int16_t position[3];
    uint32_t controller;
    uint32_t model;
    uint32_t count;
    uint32_t i;

    FUNCTION_MARKER(0x8005D36C, "RASHCDG.BIN");
    if (rrj_read32(m, actor + 36) & 4u)
    {
        int32_t divisor = batch3_mul_low(
            rrj_s32(rrj_read32(m, actor + 24)), segment_length);
        int32_t numerator = rrj_s32(rrj_read32(m, actor + 32) << 16);

        amount = rrj_s32((uint32_t)amount +
                         (uint32_t)(numerator / divisor));
    }
    for (i = 0; i < 3; ++i)
    {
        int32_t first = (int32_t)sub_8001FC90(
            rrj_s32(0x10000u - (uint32_t)amount),
            (int16_t)rrj_u16(rrj_at(m, actor + 0x6F0u + 2u * i, 2)));
        int32_t second = (int32_t)sub_8001FC90(
            amount,
            (int16_t)rrj_u16(rrj_at(m, actor + 0x6F6u + 2u * i, 2)));

        position[i] = (int16_t)rrj_s32((uint32_t)first +
                                       (uint32_t)second);
    }
    controller = rrj_read32(m, actor);
    model = rrj_read32(m, controller);
    if ((rrj_u16(rrj_at(m, model + 14, 2)) & 1u) &&
        !rrj_read32(m, controller + 52))
    {
        int32_t scale = rrj_s32(rrj_read32(m, model + 20));

        for (i = 0; i < 3; ++i)
        {
            int32_t component = rrj_s32((uint32_t)(int32_t)position[i]
                                        << 12);

            position[i] = (int16_t)batch3_asr(
                batch3_mul_low(component, scale), 24);
        }
    }
    if (*(uint8_t *)rrj_at(m, segment + 2, 1) & 0x20u)
    {
        uint32_t model_id = rrj_u16(rrj_at(m, controller + 0x220u, 2));
        uint32_t type = rrj_u16(
            rrj_at(m, 0x800541D4u + 8u * model_id + 2u, 2));

        position[0] = 0;
        position[2] = 0;
        if (type != 8u && (model_id - 69u) >= 2u)
            position[1] = 0;
    }
    rrj_put16(rrj_at(m, controller + 28, 2), (uint16_t)position[0]);
    rrj_put16(rrj_at(m, controller + 30, 2), (uint16_t)position[1]);
    rrj_put16(rrj_at(m, controller + 32, 2), (uint16_t)position[2]);

    count = *(uint8_t *)rrj_at(m, actor + 0x6E4u, 1);
    for (i = 0; i < count; ++i)
    {
        if ((rrj_read32(m, actor + 0x6ECu) >> (i & 31u)) & 1u)
        {
            uint32_t first_address = actor + 0x6FCu + 16u * i;
            int16_t first_values[4];
            int16_t second_values[4];
            int16_t blended[4];
            int32_t quaternion[4];
            uint32_t matrix = rrj_read32(m, controller + 4) + 24u * i + 4u;
            uint32_t j;

            for (j = 0; j < 4; ++j)
            {
                first_values[j] = (int16_t)rrj_u16(
                    rrj_at(m, first_address + 2u * j, 2));
                second_values[j] = (int16_t)rrj_u16(
                    rrj_at(m, first_address + 8u + 2u * j, 2));
            }
            (void)batch3_blend_quaternion(m, (uint32_t)amount,
                                          first_values, second_values,
                                          blended);
            for (j = 0; j < 4; ++j)
                quaternion[j] = rrj_s32((uint32_t)(int32_t)blended[j]
                                        << 2);
            (void)rrj_quaternion_to_matrix_values(m, matrix, quaternion);
        }
    }
    return 0;
}

uint32_t sub_800B6BD0(RRJMemory *m, uint32_t first, uint32_t normal,
                      uint32_t plane, uint32_t edge)
{
    int32_t plane_dot = rrj_s32(sub_8002E698(m, plane, normal));
    int32_t first_dot = 0;
    int32_t edge_dot = 0;
    int32_t delta;
    int32_t denominator = rrj_s32(0u - (uint32_t)plane_dot);
    uint32_t i;

    FUNCTION_MARKER(0x800B6BD0, "RASHCDG.BIN");
    if (plane_dot > -64 && plane_dot < 64)
        return 0x7FFF0000u;
    for (i = 0; i < 3; ++i)
    {
        int32_t component = rrj_s32(
            (uint32_t)(int32_t)(int16_t)rrj_u16(
                rrj_at(m, plane + 2u * i, 2))
            << 4);
        int32_t first_term = (int32_t)sub_8001FC90(
            rrj_s32(rrj_read32(m, first + 4u * i)), component);
        int32_t edge_term = (int32_t)sub_8001FC90(
            rrj_s32(rrj_read32(m, edge + 4u * i)), component);

        first_dot = rrj_s32((uint32_t)first_dot +
                            (uint32_t)first_term);
        edge_dot = rrj_s32((uint32_t)edge_dot + (uint32_t)edge_term);
    }
    delta = rrj_s32((uint32_t)first_dot - (uint32_t)edge_dot);
    if (delta > 0)
    {
        if (denominator > 0)
            return sub_80010028((uint32_t)delta,
                                (uint32_t)denominator);
        return 0u - sub_80010028((uint32_t)delta,
                                 (uint32_t)plane_dot);
    }
    delta = rrj_s32(0u - (uint32_t)delta);
    if (denominator > 0)
        return 0u - sub_80010028((uint32_t)delta,
                                 (uint32_t)denominator);
    return sub_80010028((uint32_t)delta, (uint32_t)plane_dot);
}
