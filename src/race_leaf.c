#include "native_game_api.h"
#include "game_main.h"
#include "race_bodyless_batch_008.h"
/* Small leaf translations shared by the race dependency branch */
#include "race_leaf.h"
#include "race_trace_runtime.h"
#include "audio.h"
#include "fixed_math.h"
#include "menu_latch.h"
#include "pad.h"
#include "race_pause.h"
#include "spu.h"
#include "wip.h"
#include "xport.h"
#include <stdlib.h>
#include <string.h>

static uint32_t leaf_call(RRJMemory *m, RRJRaceLeafCall call, uint32_t target, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
    const uint32_t args[8] = {a0, a1, a2, a3, 0, 0, 0, 0};
    if (!call)
        abort();
    return call(m, target, args);
}

static uint32_t leaf_call8(RRJMemory *m, RRJRaceLeafCall call, uint32_t target, const uint32_t args[8])
{
    if (!call)
        abort();
    return call(m, target, args);
}

static uint32_t leaf_leading_sign(uint32_t value)
{
    uint32_t expected = value >> 31;
    uint32_t count = 0;
    while (count < 32 && (value >> 31) == expected)
    {
        ++count;
        value <<= 1;
    }
    return count;
}

static uint32_t leaf_divu(uint32_t numerator, uint32_t denominator)
{
    return denominator ? numerator / denominator : 0xFFFFFFFF;
}

static uint32_t leaf_read_le32(RRJMemory *m, uint32_t address);

uint32_t sub_80018260(void)
{
    uint32_t index = rrj_read32(0x8005B3E8) - 1;
    uint32_t shifted, product;
    FUNCTION_MARKER(0x80018260, "SLUS_010.53");
    rrj_write32(0x8005B3E8, index);
    if (rrj_s32(index) < 0)
    {
        index = 2;
        rrj_write32(0x8005B3E8, index);
    }
    index = rrj_read32(0x8005B3E8);
    shifted = (uint32_t)(rrj_s32(index << 3) >> 1);
    product = shifted * rrj_read32(0x80052640);
    product = (uint32_t)(rrj_s32(product) >> 4);
    rrj_write32(0x800D6C08, product);
    return product;
}

uint32_t sub_8001DD08(uint32_t index, int32_t delay, uint32_t value)
{
    uint32_t record = 0x800D7428 + 24 * index;
    uint32_t result = rrj_read32(record + 8);
    FUNCTION_MARKER(0x8001DD08, "SLUS_010.53");
    if (result && value)
    {
        result = 0xFFFFFFFF;
        w_u8(record + 13, (uint8_t)value);
        if (delay == -1)
            rrj_write32(record + 20, 0);
        else
        {
            result = rrj_read32(rrj_read32(0x8005B2F8) + 12) + (uint32_t)delay;
            rrj_write32(record + 20, result);
        }
    }
    else
    {
        w_u8(record + 13, 0);
        rrj_write32(record + 20, 0);
    }
    return result;
}

uint32_t sub_8001DD74(uint32_t index, int32_t first_delay, int32_t second_delay, uint32_t value)
{
    FUNCTION_MARKER(0x8001DD74, "SLUS_010.53");
    sub_8001DC94(index, first_delay);
    return sub_8001DD08(index, second_delay, value);
}

uint32_t sub_8001DC94(uint32_t index, int32_t delay)
{
    uint32_t record = 0x800D7428 + 24 * index;
    uint32_t result = rrj_read32(record + 8);
    FUNCTION_MARKER(0x8001DC94, "SLUS_010.53");
    if (result && delay >= 0)
    {
        w_u8(record + 12, 1);
        if (delay == -1)
            rrj_write32(record + 16, 0);
        else
        {
            result = rrj_read32(rrj_read32(0x8005B2F8) + 12) + (uint32_t)delay;
            rrj_write32(record + 16, result);
        }
    }
    else
    {
        w_u8(record + 12, 0);
        rrj_write32(record + 16, 0);
    }
    return result;
}

uint32_t sub_8001F934(int32_t group, uint32_t sample, uint32_t field)
{
    uint32_t base;
    uint32_t record;
    FUNCTION_MARKER(0x8001F934, "SLUS_010.53");
    if (group < 0 || rrj_s32(rrj_read32(0x800D6870)) < group)
        return 0;
    base = rrj_read32(rrj_read32(0x800D6874) + 4 * (uint32_t)group);
    if (!base)
        return 0;
    record = sub_8001E86C(base, sample);
    if (!record)
        return 0;
    return r_u16(record + 12 * field + 10);
}

int32_t sub_8001CA58(uint32_t input, uint32_t table)
{
    int32_t signed_value = (int32_t)input - 127;
    int32_t magnitude = signed_value < 0 ? 1 - signed_value : signed_value;
    uint32_t threshold = r_u16(0x800D3990);
    uint32_t step = r_u16(0x800D3992);
    uint32_t limit = table + 8;
    uint32_t total;
    int32_t value;
    FUNCTION_MARKER(0x8001CA58, "SLUS_010.53");
    if ((int32_t)threshold >= magnitude)
        return 0;
    total = threshold + step;
    while ((int32_t)total < magnitude && table < limit)
    {
        total += step;
        table += 2;
    }
    if (table < limit)
    {
        uint32_t offset = (uint32_t)magnitude - (total - step);
        value = r_u16(table) + (int32_t)(offset * (r_u16(table + 2) - r_u16(table))) / (int32_t)step;
    }
    else if (magnitude < 128)
    {
        uint32_t adjusted = step - (total - 128);
        uint32_t offset = (uint32_t)magnitude + step - total;
        value = r_u16(table) + (int32_t)(offset * (r_u16(table + 2) - r_u16(table))) / (int32_t)adjusted;
    }
    else
    {
        value = 0x10000;
    }
    return signed_value < 0 ? -value : value;
}

static int32_t leaf_metric(int32_t x, int32_t z)
{
    int32_t ax = x < 0 ? -x : x;
    int32_t az = z < 0 ? -z : z;
    int32_t high = ax >= az ? ax : az;
    int32_t low = ax >= az ? az : ax;
    int32_t mixed = low + (low >> 1);
    return high - (high >> 5) - (high >> 7) + (mixed >> 2) + (mixed >> 6);
}

int32_t sub_80019E40(uint32_t index, int32_t x, int32_t z, int32_t other_x, int32_t other_z, uint32_t level_out, uint32_t pan_out, uint32_t ratio_out, uint32_t shift)
{
    uint32_t table = rrj_read32(0x8005B40C);
    uint32_t record = table + 72 * index;
    int32_t dx;
    int32_t dz;
    int32_t distance;
    int16_t angle = 0;
    int32_t result = 0xFFFF;
    FUNCTION_MARKER(0x80019E40, "SLUS_010.53");
    if (!table)
    {
        rrj_write32(level_out, 127);
        rrj_write32(pan_out, 64);
        return 64;
    }
    rrj_write32(pan_out, rrj_read32(record + 20));
    dx = x - rrj_s32(rrj_read32(record + 4));
    dz = z - rrj_s32(rrj_read32(record + 8));
    distance = leaf_metric(dx, dz) >> (shift & 31);
    if (distance > 0x400000)
    {
        rrj_write32(level_out, 0);
        if (ratio_out)
            rrj_write32(ratio_out, 0x10000);
        return 0xFFFF;
    }
    if (distance > 0xFFFF)
    {
        int32_t attenuation = 127 - (distance >> 15);
        int32_t sign = attenuation >> 31;
        rrj_write32(level_out, (uint32_t)((~sign & attenuation) + (sign & (distance >> 15))));
        angle = (int16_t)sub_80020018((uint32_t)dx, (uint32_t)-dz);
        result = 1;
        if (rrj_read32(rrj_read32(0x8005B2F8) + 48) == 1)
            rrj_write32(pan_out, (uint8_t)(((((uint16_t)rrj_u16(rrj_at(record, 2)) - angle) & 0xFFF) >> 4) + 64));
    }
    else
    {
        rrj_write32(level_out, 127);
        result = 127;
    }
    if (ratio_out)
    {
        int32_t sx = other_x - rrj_s32(rrj_read32(record + 12));
        int32_t sz = other_z - rrj_s32(rrj_read32(record + 16));
        uint32_t delta = ((uint16_t)sub_80020018((uint32_t)sx, (uint32_t)-sz) - (uint16_t)angle) & 0xFFF;
        int32_t sine = r_s16(0x8005624C + ((delta * 4) | 2));
        int32_t divisor = (int32_t)((uint32_t)((uint64_t)(uint32_t)(leaf_metric(sx, sz) >> 8) * (uint32_t)(sine >> 4))) + 0x02000000;
        if (divisor > 0)
            result = (int32_t)sub_80010028(0x02000000, (uint32_t)divisor);
        else
            result = -(int32_t)sub_80010028(0x02000000, (uint32_t)-divisor);
        rrj_write32(ratio_out, (uint32_t)result);
    }
    return result;
}

static int32_t leaf_mul_asr(int32_t left, int32_t right, uint32_t shift)
{
    uint32_t low = (uint32_t)((uint64_t)(uint32_t)left * (uint32_t)right);
    return rrj_s32(low) >> shift;
}

static int32_t leaf_product_asr16(int32_t left, int32_t right)
{
    return (int32_t)(((int64_t)left * right) >> 16);
}

static void leaf_direction_op_local(const int16_t diagonal[3], const int16_t direction[3], int16_t output[3])
{
    int64_t cross[3];
    uint32_t index;
    cross[0] = (int64_t)direction[2] * diagonal[1] - (int64_t)direction[1] * diagonal[2];
    cross[1] = (int64_t)direction[0] * diagonal[2] - (int64_t)direction[2] * diagonal[0];
    cross[2] = (int64_t)direction[1] * diagonal[0] - (int64_t)direction[0] * diagonal[1];
    for (index = 0; index < 3; ++index)
    {
        int64_t value = cross[index] / 4096 - (cross[index] < 0 && cross[index] % 4096 != 0);
        if (value > 32767)
            value = 32767;
        if (value < -32768)
            value = -32768;
        output[index] = (int16_t)value;
    }
}

uint32_t sub_8001769C(uint32_t slot, uint32_t listener, uint32_t sound, uint32_t kind)
{
    uint32_t base = rrj_read32(0x8005B410 + 4 * listener);
    uint32_t result = 44 * slot;
    uint32_t record = base + result;
    uint32_t level_out = 0x1F800360;
    uint32_t pan_out = level_out + 4;
    uint32_t ratio_out = pan_out + 4;
    uint8_t parameters[12];
    uint32_t mode;
    int32_t x;
    int32_t z;
    FUNCTION_MARKER(0x8001769C, "SLUS_010.53");
    if (!base)
        return result;
    x = rrj_s32(rrj_read32(record + 12)) + (rrj_s32(rrj_read32(record + 20)) >> 6);
    z = rrj_s32(rrj_read32(record + 16)) + (rrj_s32(rrj_read32(record + 24)) >> 6);
    rrj_write32(record + 12, (uint32_t)x);
    rrj_write32(record + 16, (uint32_t)z);
    sub_80019E40(listener, x, z, rrj_s32(rrj_read32(record + 20)), rrj_s32(rrj_read32(record + 24)), level_out, pan_out, ratio_out, rrj_read32(record + 8) == 6 ? 3 : 0);
    if (rrj_read32(record + 8) >= 3 && rrj_read32(record + 8) <= 5)
        mode = 2;
    else if (rrj_read32(record + 8) == 2)
        mode = 4;
    else
        mode = 1;
    rrj_put32(parameters, (uint32_t)leaf_mul_asr(rrj_s32(rrj_read32(record + 28)), rrj_s32(rrj_read32(ratio_out)), 16));
    rrj_put32(parameters + 4, (uint32_t)leaf_mul_asr(rrj_s32(rrj_read32(level_out)), rrj_s32(rrj_read32(0x800D6C00 + 4 * mode)), 7));
    rrj_put32(parameters + 8, rrj_read32(pan_out));
    result = sub_8001F174(sound, kind, 0, 1, parameters);
    rrj_write32(record + 32, result);
    rrj_write32(record + 4, 1);
    return 1;
}

uint32_t sub_80017BA0(int32_t x, int32_t z, uint32_t sound, uint32_t handle)
{
    uint32_t state = rrj_read32(0x8005B2F8);
    uint32_t players;
    uint32_t index = 0;
    uint32_t level_out = 0x1F800380;
    uint32_t pan_out = level_out + 4;
    uint8_t parameters[12];
    FUNCTION_MARKER(0x80017BA0, "SLUS_010.53");
    if (handle == rrj_read32(0x8005B420))
        return handle;
    players = rrj_read32(state + 48);
    if (!players)
        return 0;
    do
    {
        if (x || z)
            sub_80019E40(index, x, z, 0, 0, level_out, pan_out, 0, 0);
        else
        {
            rrj_write32(level_out, 127);
            rrj_write32(pan_out, 64);
            index = players;
        }
        rrj_put32(parameters, 0xFFFFFFFF);
        rrj_put32(parameters + 4, (uint32_t)leaf_mul_asr(rrj_s32(rrj_read32(level_out)), rrj_s32(rrj_read32(0x800D6C0C)), 7));
        rrj_put32(parameters + 8, rrj_read32(pan_out));
        if (!handle)
            handle = rrj_read32(0x8005B404);
        sub_8001F174(handle, sound, 0, 0, parameters);
        ++index;
    } while (index < rrj_read32(state + 48));
    return 0;
}

uint32_t sub_80017814(uint32_t slot, uint32_t listener)
{
    uint32_t offset = 44 * slot;
    uint32_t base_address = 0x8005B410 + 4 * listener;
    uint32_t base = rrj_read32(base_address);
    uint32_t record = base + offset;
    uint32_t listener_table;
    uint32_t count;
    uint32_t index;
    FUNCTION_MARKER(0x80017814, "SLUS_010.53");
    if (!record)
        return offset;
    if (rrj_read32(record + 4))
    {
        (uint32_t)sub_8001F7EC(rrj_read32(record + 32));
        listener_table = rrj_read32(0x8005B40C) + 72 * listener;
        count = rrj_read32(listener_table + 36);
        if (rrj_s32(slot) < rrj_s32(count))
        {
            uint32_t child = base + 44 * count;
            for (index = 0; rrj_s32(index) < rrj_s32(rrj_read32(listener_table + 40)); ++index, child += 44)
            {
                if (r_u16(record) == r_u16(child) && rrj_s32(rrj_read32(child + 8)) >= 3)
                    sub_80017814(count + index, listener);
            }
        }
        if (rrj_read32(record + 8) == 2)
        {
            int32_t sound_index = rrj_s32(rrj_read32(record + 40));
            uint32_t sound = 0x800D6AA0 + 32 * (uint32_t)sound_index;
            if (rrj_read32(sound + 8) == 3 || sound_index < 0)
            {
                rrj_write32(sound + 8, 4);
                rrj_write32(listener_table + 44, 0);
            }
        }
    }
    w_u16(record, 224);
    rrj_write32(record + 8, 0);
    rrj_write32(record + 4, 0);
    rrj_write32(record + 36, rrj_read32(rrj_read32(0x8005B2F8) + 12));
    return 224;
}

static uint32_t leaf_abs32(int32_t value)
{
    int32_t sign = value >> 31;
    return (uint32_t)((sign + value) ^ sign);
}

static int32_t leaf_quaternion_reciprocal(uint32_t value)
{
    uint32_t scaled = value << 3;
    uint32_t magnitude = rrj_s32(scaled) < 0 ? 0u - scaled : scaled;
    uint32_t divisor = (magnitude >> 1) + (uint32_t)(rrj_s32(magnitude - 2) >> 31);
    uint32_t result = 0x80000000u / divisor;
    return rrj_s32(scaled) < 0 ? rrj_s32(0u - result) : rrj_s32(result);
}

static int16_t leaf_quaternion_component(int32_t value, int32_t scale)
{
    uint32_t product = (uint32_t)((int64_t)value * scale);
    return (int16_t)(rrj_s32(product) >> 14);
}

static void leaf_direction_op(RRJMemory *m, uint32_t diagonal_address, uint32_t direction_address, uint32_t output)
{
    int32_t diagonal[3];
    int32_t direction[3];
    int64_t cross[3];
    uint32_t i;
    for (i = 0; i < 3; ++i)
    {
        diagonal[i] = r_s16(diagonal_address + 2 * i);
        direction[i] = r_s16(direction_address + 2 * i);
    }
    cross[0] = (int64_t)direction[2] * diagonal[1] - (int64_t)direction[1] * diagonal[2];
    cross[1] = (int64_t)direction[0] * diagonal[2] - (int64_t)direction[2] * diagonal[0];
    cross[2] = (int64_t)direction[1] * diagonal[0] - (int64_t)direction[0] * diagonal[1];
    for (i = 0; i < 3; ++i)
    {
        int64_t value = cross[i] / 4096 - (cross[i] < 0 && cross[i] % 4096 != 0);
        if (value > 32767)
            value = 32767;
        if (value < -32768)
            value = -32768;
        rrj_put16(rrj_at(output + 2 * i, 2), (uint32_t)value);
    }
}

static uint32_t leaf_80012AEC_values(RRJMemory *m, uint32_t object, uint32_t index, uint32_t values[3])
{
    uint32_t records = rrj_read32(rrj_read32(object + 96) + 8);
    uint32_t record = records + 12 * index;
    uint32_t source = rrj_read32(record + 8);
    uint32_t shift;
    uint32_t i;
    if (!source)
        return 0xFFFFFFFFu;
    shift = (r_u16(rrj_read32(record) + 14) >> 12) & 31;
    for (i = 0; i < 3; ++i)
        values[i] = (uint32_t)(rrj_s32((uint32_t)(int32_t)r_s16(source + 24 + 2 * i)) >> shift) << 10;
    return 0;
}

uint32_t sub_80012AEC(uint32_t object, uint32_t index, uint32_t output)
{
    uint32_t values[3];
    uint32_t result;
    uint32_t i;
    FUNCTION_MARKER(0x80012AEC, "SLUS_010.53");
    result = leaf_80012AEC_values(rrj_host_context(), object, index, values);
    if (result)
        return result;
    for (i = 0; i < 3; ++i)
        rrj_write32(output + 4 * i, values[i]);
    return 0;
}

uint32_t sub_80012FC8(uint32_t object, uint32_t index)
{
    uint32_t values[3];
    uint32_t type;
    uint32_t result;
    FUNCTION_MARKER(0x80012FC8, "SLUS_010.53");
    (void)leaf_80012AEC_values(rrj_host_context(), object, index, values);
    type = r_u16(object + 172) >> 5;
    if ((uint32_t)(type - 1) < 2)
    {
        rrj_write32(object + 312, values[0] << 1);
        rrj_write32(object + 308, values[1]);
        rrj_write32(object + 304, values[2]);
        return values[2];
    }
    rrj_write32(object + 304, values[0]);
    rrj_write32(object + 312, values[1] << 1);
    rrj_write32(object + 308, values[2]);
    result = type;
    if (!type)
    {
        uint32_t kind = rrj_read32(object + 180);
        int32_t value = rrj_s32(rrj_read32(object + 312));
        if (kind >= 18)
            value = value * 11 / 16;
        else
        {
            uint32_t alternate = kind < 9 || rrj_read32(rrj_read32(0x8005B2F8) + 60) == 2;
            value = rrj_s32((uint32_t)((2 * alternate + 9) * value)) / 8;
        }
        rrj_write32(object + 312, (uint32_t)value);
        result = (uint32_t)(value - 4096);
        if (r_u16(object + 172) < rrj_read32(rrj_read32(0x8005B2F8) + 48))
            rrj_write32(object + 312, result);
    }
    return result;
}

uint32_t sub_800716C0(uint32_t matrix, uint32_t output)
{
    uint32_t axes[3];
    int32_t diagonal[3];
    int32_t trace;
    uint32_t root;
    int32_t reciprocal;
    uint32_t i;
    FUNCTION_MARKER(0x800716C0, "RASHCDG.BIN");
    for (i = 0; i < 3; ++i)
    {
        axes[i] = rrj_read32(0x8005B61C + 4 * i);
        diagonal[i] = r_s16(matrix + 8 * i);
    }
    trace = diagonal[0] + diagonal[1] + diagonal[2];
    if (rrj_s32((uint32_t)trace << 4) > 0)
    {
        root = sub_8004CF74(((uint32_t)trace << 4) + 0x10000u);
        rrj_put16(rrj_at(output + 6, 2), (uint16_t)(root >> 1));
        reciprocal = leaf_quaternion_reciprocal(root);
        rrj_put16(rrj_at(output, 2), (uint16_t)leaf_quaternion_component(r_s16(matrix + 10) - r_s16(matrix + 14), reciprocal));
        rrj_put16(rrj_at(output + 2, 2), (uint16_t)leaf_quaternion_component(r_s16(matrix + 12) - r_s16(matrix + 4), reciprocal));
        rrj_put16(rrj_at(output + 4, 2), (uint16_t)leaf_quaternion_component(r_s16(matrix + 2) - r_s16(matrix + 6), reciprocal));
        return (uint32_t)(int32_t)r_s16(output + 4);
    }
    {
        uint32_t largest = diagonal[0] < diagonal[1] ? 1 : 0;
        uint32_t second;
        uint32_t third;
        if (diagonal[largest] < diagonal[2])
            largest = 2;
        second = axes[largest];
        third = axes[second];
        root = sub_8004CF74(((uint32_t)(diagonal[largest] - diagonal[second] - diagonal[third]) << 4) + 0x10000u);
        rrj_put16(rrj_at(output + 2 * largest, 2), (uint16_t)(root >> 1));
        reciprocal = leaf_quaternion_reciprocal(root);
        rrj_put16(rrj_at(output + 6, 2), (uint16_t)leaf_quaternion_component(r_s16(matrix + 2 * (3 * second + third)) - r_s16(matrix + 2 * (3 * third + second)), reciprocal));
        rrj_put16(rrj_at(output + 2 * second, 2), (uint16_t)leaf_quaternion_component(r_s16(matrix + 2 * (3 * largest + second)) + r_s16(matrix + 2 * (3 * second + largest)), reciprocal));
        rrj_put16(rrj_at(output + 2 * third, 2), (uint16_t)leaf_quaternion_component(r_s16(matrix + 2 * (3 * largest + third)) + r_s16(matrix + 2 * (3 * third + largest)), reciprocal));
        return (uint32_t)(int32_t)r_s16(output + 2 * third);
    }
}

uint32_t sub_8001A760(uint32_t actor_index, uint32_t mode)
{
    uint32_t state = rrj_read32(0x8005B2F8);
    uint32_t players = rrj_read32(state + 48);
    uint32_t actor = rrj_read32(0x8005B3A0) + 1096 * actor_index;
    uint32_t listener;
    uint32_t have_sound = 0;
    uint32_t saved_bank = 0;
    uint32_t saved_sample = 0;
    uint32_t saved_row = 0;
    uint32_t saved_side = 0;
    uint32_t saved_candidate = 255;
    FUNCTION_MARKER(0x8001A760, "SLUS_010.53");
    if (!players)
        return 1096 * actor_index;
    for (listener = 0; listener < players; ++listener)
    {
        uint32_t listener_offset = 72 * listener;
        uint32_t listener_table = rrj_read32(0x8005B40C) + listener_offset;
        uint32_t record_base = rrj_read32(0x8005B410 + 4 * listener);
        uint32_t other = rrj_read32(0x8005B268 + 4 * listener);
        uint32_t first;
        uint32_t limit;
        uint32_t slot;
        uint32_t record;
        uint32_t direct = 0;
        uint32_t bank;
        uint32_t sample;
        uint32_t row = 0;
        uint32_t side = 0;
        uint32_t candidate = 255;
        uint32_t candidates[3] = {255, 255, 255};
        uint32_t random_choice = 0;
        uint32_t found = 0;
        uint32_t code;
        if (!record_base)
            return 0;
        if (!mode)
        {
            if (leaf_abs32(rrj_s32(rrj_read32(other + 324)) - rrj_s32(rrj_read32(actor + 324))) > 0xF000)
                continue;
            if (r_u16(actor + 172) >= players && rrj_read32(listener_table + 44) && rrj_read32(actor + 1088))
                continue;
        }
        first = rrj_read32(listener_table + 36);
        limit = first + rrj_read32(listener_table + 40);
        record = record_base + 44 * first;
        for (slot = first; slot < limit; ++slot, record += 44)
        {
            if (r_u16(record) == actor_index && rrj_s32(rrj_read32(record + 8)) < 3)
            {
                if (!mode && r_u16(actor + 172) >= players && rrj_read32(actor + 1088))
                    return rrj_read32(actor + 1088);
                sub_80017814(slot, listener);
                break;
            }
        }
        if (have_sound)
        {
            bank = saved_bank;
            sample = saved_sample;
            row = saved_row;
            side = saved_side;
            candidate = saved_candidate;
        }
        else
        {
            code = r_u8(rrj_read32(actor + 1084) + 1);
            if (mode)
            {
                candidates[0] = 3;
                candidates[1] = 11;
            }
            else if (r_u16(actor + 172) < players)
            {
                if ((code & 15) == 2 && r_u8(state + 4) != 33)
                    candidates[0] = 7;
                else
                    candidates[0] = (actor_index << 4) | 6;
                random_choice = 1;
            }
            else if ((code & 15) == 2 || r_u8(state + 4) == 33)
            {
                candidates[0] = 7;
                candidates[1] = 15;
                random_choice = 1;
            }
            else if ((((code >> 4) - 1) & 0xFFFFFFFF) < 2 || (code >> 4) == 5)
            {
                candidates[0] = players == 2 ? code & 3 : code;
                candidates[1] = (code & 3) + 4;
                candidates[2] = (code & 3) + 68;
            }
            else
            {
                candidates[0] = (code & 3) + 4;
                candidates[1] = (code & 3) + 68;
            }
            for (slot = 0; slot < 3 && candidates[slot] != 255 && !found; ++slot)
            {
                uint32_t attempts;
                candidate = candidates[slot];
                side = (sub_80043F00(0xF2000002) & 255) >> 7;
                row = (9 * (sub_80043F00(0xF2000002) & 255)) >> 8;
                for (attempts = 0; attempts < 9; ++attempts)
                {
                    uint32_t sound = 0x800D6AA0 + 32 * row;
                    if (rrj_read32(sound + 12 + 4 * side) != candidate)
                    {
                        side ^= 1;
                        if (rrj_read32(sound + 12 + 4 * side) != candidate)
                        {
                            if (++row >= 9)
                                row = 0;
                            continue;
                        }
                    }
                    found = 1;
                    break;
                }
            }
            if (!found)
                return 6;
            if (candidate == 6 || ((code & 15) == 2 && r_u16(actor + 172) < players))
                rrj_write32(0x8005B440, rrj_read32(0x8005B440) == 0);
            if (rrj_read32(actor + 1088))
            {
                if ((candidate - 68 < 2) || (candidate == 7 && r_u16(actor + 172) >= players))
                {
                    int32_t now = (int32_t)((rrj_read32(state + 12) >> 8) & 0xFFFF);
                    if (leaf_abs32(now - r_s16(actor + 870)) < 21)
                        return 1;
                    w_u16(actor + 870, (uint16_t)now);
                }
            }
            bank = rrj_read32(0x800D6AA0 + 32 * row);
            sample = rrj_read32(0x800D6AA0 + 32 * row + 20 + 4 * side);
        }
        first = rrj_read32(listener_table + 36);
        limit = first + rrj_read32(listener_table + 40);
        record = record_base + 44 * first;
        for (slot = first; slot < limit; ++slot, record += 44)
        {
            if (!rrj_read32(record + 4))
            {
                found = 1;
                break;
            }
        }
        if (slot >= limit)
            found = 0;
        if (!found)
        {
            if (mode || r_u16(actor + 172) < players)
                direct = 1;
            else
                continue;
        }
        if (!mode)
        {
            if (r_u16(actor + 172) < players || !rrj_read32(actor + 1088))
            {
                uint32_t source = rrj_read32(actor + 1088) ? actor : rrj_read32(actor + 856);
                uint32_t nearest = sub_8008B428(source + 172, 1) & 0xFFFF;
                if (nearest != 224)
                {
                    other = rrj_read32(0x8005B3A0) + 1096 * nearest;
                    if (leaf_abs32(16 * (rrj_s32(rrj_read32(other + 324)) - rrj_s32(rrj_read32(source + 324)))) <= 0x13FFFF && (rrj_read32(other + 560) & 0x08000000))
                    {
                        uint32_t vehicle = rrj_read32(other + 1084);
                        uint32_t table_index = r_u8(0x800D38B0 + r_u16(source + 172));
                        int32_t gear = r_s8(other + 946);
                        w_u8(vehicle + table_index + 16, 15);
                        if ((uint32_t)(r_u16(other + 956 + 8 * (uint32_t)(gear - 1)) - 4) < 13 && sub_800BC1EC(other, source, 0))
                        {
                            uint32_t event = 0x1F8003A0;
                            sub_800B92C0(vehicle);
                            w_u16(event, 16);
                            w_u16(event + 2, r_u16(actor + 172));
                            sub_800BCA68(event, 2, other);
                        }
                    }
                }
            }
            if (rrj_read32(actor + 1088))
            {
                int32_t delta;
                if (rrj_read32(other + 360) == rrj_read32(actor + 360) && (!(rrj_read32(other + 360) >> 16) || rrj_read32(other + 336) == rrj_read32(actor + 336)))
                {
                    delta = rrj_s32(rrj_read32(other + 344)) - rrj_s32(rrj_read32(actor + 344));
                    if (rrj_s32(rrj_read32(actor + 364)) < 0)
                        delta = -delta;
                }
                else
                {
                    delta = rrj_s32(sub_800B6AAC(rrj_at(other + 184, 12), rrj_at(actor + 432, 6), rrj_at(actor + 184, 12)));
                }
                {
                    uint32_t flags = rrj_read32(actor + 852) + 572;
                    xport_update_u8(flags, XPORT_MEMORY_UPDATE_OR, delta < 0 ? 8 : 4);
                }
            }
        }
        if ((candidate & 0xFE) == 4 || (candidate & 0xFE) == 68)
            rrj_write32(0x800D6AA0 + 32 * row + 12 + 4 * side, (candidate & 1) + 68);
        else if (!have_sound && !(candidate & 8) && !random_choice)
            rrj_write32(0x800D6AA0 + 32 * row + 12 + 4 * side, 0xFFFFFFFF);
        if (direct)
        {
            sub_80017BA0(rrj_s32(rrj_read32(actor + 184)), rrj_s32(rrj_read32(actor + 192)), sample, bank);
            rrj_write32(0x800D6AA0 + 32 * row + 8, 4);
        }
        else
        {
            w_u16(record, (uint16_t)actor_index);
            rrj_write32(record + 12, rrj_read32(actor + 184));
            rrj_write32(record + 16, rrj_read32(actor + 192));
            rrj_write32(record + 20, (uint32_t)leaf_mul_asr(r_s16(actor + 450), rrj_s32(rrj_read32(actor + 480)) >> 4, 8));
            rrj_write32(record + 24, (uint32_t)leaf_mul_asr(r_s16(actor + 454), rrj_s32(rrj_read32(actor + 480)) >> 4, 8));
            rrj_write32(record + 28, sub_8001F934(rrj_s32(bank), sample, 0));
            rrj_write32(record + 8, 2);
            sub_8001769C(slot, listener, bank, sample);
            rrj_write32(record + 36, rrj_read32(state + 12) + 500);
            if (r_u16(actor + 172) >= players && !mode)
                rrj_write32(listener_table + 44, 1);
            rrj_write32(0x800D6AA0 + 32 * row + 8, 3);
            rrj_write32(record + 40, row);
        }
        have_sound = 1;
        saved_bank = bank;
        saved_sample = sample;
        saved_row = row;
        saved_side = side;
        saved_candidate = candidate;
    }
    return 0;
}

uint32_t sub_800C2178(uint32_t object, uint32_t index)
{
    uint32_t pair = 0x800CCB78 + 4 * index;
    uint32_t mapping = r_u16(pair);
    uint32_t table = rrj_read32(object + 184);
    uint32_t first = rrj_read32(table + 4 * mapping);
    uint32_t duration = rrj_read32(object + 8 * first + 20);
    uint32_t state, clock, threshold, second;
    FUNCTION_MARKER(0x800C2178, "RASHCDG.BIN");
    if (rrj_s32(duration) <= 0)
        return 0;
    state = rrj_read32(0x8005B2F8);
    clock = rrj_read32(object);
    threshold = rrj_read32(state + 32);
    if (rrj_s32(threshold) < rrj_s32(clock - duration))
        return 0;
    second = r_u16(pair + 2);
    if (second == 15)
        return 1;
    first = rrj_read32(table + 4 * second);
    return rrj_s32(rrj_read32(object + 8 * first + 20)) > 0;
}

uint32_t sub_8008CDF4(uint32_t index)
{
    FUNCTION_MARKER(0x8008CDF4, "RASHCDG.BIN");
    switch (index)
    {
        case 0:
            return r_s8(0x8005B2B9) < r_s8(0x8005B2B8);
        case 2:
            return rrj_read32(0x800D4B80) && rrj_s32(rrj_read32(0x800D4B74)) < 4 && rrj_s32(rrj_read32(0x800D4B70)) < rrj_s32(rrj_read32(0x800D8744)) && rrj_s32(rrj_read32(0x800D4C7C)) > 0;
        case 3:
            return rrj_s32(rrj_read32(0x800CF654)) < 16 && rrj_s32(rrj_read32(0x800D4C6C)) > 0;
        case 4:
            return (rrj_s32(rrj_read32(0x800CD6CC)) < rrj_s32(rrj_read32(0x800CD6D0)) || rrj_read32(0x800D1814) >= 596) && rrj_s32(rrj_read32(0x800D4C9C)) > 0;
        case 5:
            return (rrj_s32(rrj_read32(0x800CE59C)) < rrj_s32(rrj_read32(0x800CE5A0)) || rrj_read32(0x800D1814) >= 452) && rrj_s32(rrj_read32(0x800D4C9C)) > 0;
        case 6:
            return rrj_read32(0x800CD6C4) && rrj_s32(rrj_read32(0x800CD6AC)) < rrj_s32(rrj_read32(0x8005B214));
        default:
            return 0;
    }
}

uint32_t sub_8009D9E4(void)
{
    int32_t remaining = rrj_s32(rrj_read32(rrj_read32(0x800CE50C)));
    uint32_t object = rrj_read32(0x800CE500);
    uint32_t stride = rrj_read32(0x800CE504);
    FUNCTION_MARKER(0x8009D9E4, "RASHCDG.BIN");
    while (remaining >= 0)
    {
        if (object && r_u16(object + 172) && !rrj_read32(object + 180))
            return 1;
        --remaining;
        object += stride;
    }
    return 0;
}

int32_t sub_8009CF1C(int32_t scale, int32_t count, int32_t kind)
{
    int8_t signed_kind = (int8_t)kind;
    int32_t magnitude = signed_kind < 0 ? -(int32_t)signed_kind : (int32_t)signed_kind;
    uint32_t index = 0;
    uint32_t product;
    int32_t result;
    FUNCTION_MARKER(0x8009CF1C, "RASHCDG.BIN");
    if (magnitude == 2)
    {
        int32_t half = (count - 1) / 2;
        index = (uint32_t)half;
        if (count >= 3)
            index += sub_8001FC58() % (uint32_t)(count - 1 - half);
    }
    else if (magnitude == 3)
        index = (uint32_t)(count - 1);
    product = (uint32_t)scale * index;
    result = rrj_s32(product + (uint32_t)(scale >> 1));
    return signed_kind < 0 ? rrj_s32(0u - (uint32_t)result) : result;
}

uint32_t sub_80097308(uint32_t object)
{
    uint32_t state = rrj_read32(object + 852);
    uint32_t control = rrj_read32(object + 1084);
    uint32_t kind;
    uint32_t result;
    FUNCTION_MARKER(0x80097308, "RASHCDG.BIN");
    w_u8(control + 39, 0xFE);
    rrj_write32(state + 552, rrj_read32(state + 552) & 0xFFEFF067u);
    rrj_write32(object + 720, 0x20000);
    kind = r_u16(state + 544);
    if (kind < 72 || kind >= 74)
    {
        rrj_write32(object + 480, 0);
        rrj_write32(state + 488, 0);
        rrj_write32(state + 484, 0);
        rrj_write32(state + 480, 0);
        rrj_write32(state + 464, 0);
        rrj_write32(state + 460, 0);
        rrj_write32(state + 456, 0);
    }
    result = rrj_read32(rrj_read32(0x8005B2F8) + 16);
    rrj_write32(control + 40, result);
    rrj_write32(object + 924, 0);
    return result;
}

uint32_t sub_800A8BE0(uint16_t value, uint32_t packed_out)
{
    uint32_t packed = rrj_read32(packed_out);
    uint32_t shift = 0;
    uint32_t encoded = ((uint32_t)value + 1) & 31;
    FUNCTION_MARKER(0x800A8BE0, "RASHCDG.BIN");
    while (packed)
    {
        if ((packed & 31) == (uint32_t)value + 1)
            return 1;
        packed >>= 5;
        shift += 5;
    }
    if (shift >= 30)
        return 0;
    rrj_write32(packed_out, rrj_read32(packed_out) | (encoded << shift));
    return 1;
}

int32_t sub_8009DA4C(uint32_t source, uint32_t stopped_out)
{
    int32_t remaining = rrj_s32(rrj_read32(rrj_read32(0x800CE50C)));
    uint32_t object = rrj_read32(0x800CE500);
    uint32_t stride = rrj_read32(0x800CE504);
    uint32_t nearest = 0x03E70000;
    FUNCTION_MARKER(0x8009DA4C, "RASHCDG.BIN");
    while (remaining >= 0)
    {
        if (object && r_u16(object + 172) && !rrj_read32(object + 180))
        {
            uint32_t distance = leaf_abs32(rrj_s32((rrj_read32(source + 324) - rrj_read32(object + 324)) << 4));
            if (distance < nearest)
            {
                nearest = distance;
                rrj_write32(stopped_out, rrj_s32(rrj_read32(object + 480)) < 131);
            }
        }
        --remaining;
        object += stride;
    }
    return nearest == 0x03E70000 ? -65536 : rrj_s32(nearest);
}

uint32_t sub_8009E444(uint32_t point, uint32_t object)
{
    int32_t x;
    int32_t z;
    int32_t high;
    int32_t low;
    int32_t combined;
    FUNCTION_MARKER(0x8009E444, "RASHCDG.BIN");
    if (!point || !object)
        return 0;
    {
        uint32_t state = rrj_read32(object + 852);
        uint32_t kind = rrj_read32(state + 604);
        if (kind == 3 || kind == 4)
            object = state;
    }
    x = (int32_t)r_s16(point + 186) - (rrj_s32(rrj_read32(object + 184)) >> 16);
    z = (int32_t)r_s16(point + 194) - (rrj_s32(rrj_read32(object + 192)) >> 16);
    x = rrj_s32(leaf_abs32(x));
    z = rrj_s32(leaf_abs32(z));
    high = x >= z ? x : z;
    low = x >= z ? z : x;
    combined = high - (high >> 5) - (high >> 7) + ((low + (low >> 1)) >> 2) + ((low + (low >> 1)) >> 6);
    return leaf_abs32(rrj_s32((uint32_t)combined << 16));
}

int32_t sub_8009E768(uint32_t object)
{
    uint32_t record = 0;
    int32_t base = 0;
    int32_t scale;
    int32_t count;
    int32_t selector;
    FUNCTION_MARKER(0x8009E768, "RASHCDG.BIN");
    if (r_u16(object + 362))
    {
        uint32_t direction_out = 0x1F8003F4;
        record = sub_8003E1E8(rrj_read32(object + 328), (uint32_t)(int32_t)r_s16(rrj_read32(object + 356) + 10), direction_out);
        if (record)
        {
            scale = rrj_s32(rrj_read32(record + 4));
            if (rrj_s32(rrj_read32(direction_out)) > 0)
            {
                count = r_s16(record + 140);
                if (r_s16(record + 142) > 0)
                    base = rrj_s32(rrj_read32(record + 208));
            }
            else
            {
                count = r_s16(record + 12);
                if (r_s16(record + 14) > 0)
                    base = rrj_s32(rrj_read32(record + 80));
            }
            selector = r_s8(object + 508);
            return rrj_s32(leaf_abs32(base) + leaf_abs32(sub_8009CF1C(scale, count, selector)));
        }
    }
    else
    {
        record = rrj_read32(object + 372);
        if (record)
        {
            scale = rrj_s32(rrj_read32(object + 424));
            if (rrj_s32(rrj_read32(object + 364)) > 0)
            {
                count = r_s16(object + 420);
                if (r_s16(record + 142) > 0)
                    base = rrj_s32(rrj_read32(record + 208));
            }
            else
            {
                count = r_s16(object + 408);
                scale = rrj_s32(0u - (uint32_t)scale);
                if (r_s16(record + 14) > 0)
                    base = rrj_s32(rrj_read32(record + 80));
            }
            selector = r_s8(object + 508);
            return rrj_s32(leaf_abs32(base) + leaf_abs32(sub_8009CF1C(scale, count, selector)));
        }
    }
    return rrj_s32(rrj_read32(object + 364)) < 0 ? -117964 : 117964;
}

uint32_t sub_800A3F14(uint32_t config, uint32_t object)
{
    uint32_t flags = r_u16(config + 12);
    uint32_t result = flags & 2;
    uint32_t i;
    FUNCTION_MARKER(0x800A3F14, "RASHCDG.BIN");
    if (flags & 1)
    {
        uint32_t piece = rrj_read32(object + 340);
        for (i = 0; i < 9; ++i)
            rrj_put16(rrj_at(object + 432 + 2 * i, 2), rrj_u16(rrj_at(piece + 2 + 2 * i, 2)));
        for (i = 0; i < 3; ++i)
            rrj_put16(rrj_at(object + 438 + 2 * i, 2), 0u - rrj_u16(rrj_at(object + 438 + 2 * i, 2)));
    }
    else
    {
        for (i = 0; i < 9; ++i)
            rrj_put16(rrj_at(object + 432 + 2 * i, 2), i == 0 || i == 4 || i == 8 ? 0x1000 : 0);
    }
    if (flags & 2)
    {
        for (i = 0; i < 3; ++i)
            rrj_put16(rrj_at(object + 444 + 2 * i, 2), rrj_u16(rrj_at(config + 14 + 2 * i, 2)));
        if (flags & 1)
        {
            leaf_direction_op(rrj_host_context(), object + 438, object + 444, object + 432);
            return sub_8002E468(object + 432);
        }
        rrj_put16(rrj_at(object + 432, 2), rrj_u16(rrj_at(config + 18, 2)));
        result = 0u - rrj_u16(rrj_at(config + 14, 2));
        rrj_put16(rrj_at(object + 436, 2), result);
    }
    return result;
}

uint32_t sub_800A07D0(uint32_t config, uint32_t object, uint32_t kind, uint32_t other, RRJRaceLeafCall call)
{
    uint32_t position = 0x1F800380;
    uint32_t buffer = 0x1F800390;
    uint32_t segment;
    uint32_t incomplete = 1;
    FUNCTION_MARKER(0x800A07D0, "RASHCDG.BIN");
    rrj_write32(position, rrj_read32(config + 8));
    rrj_write32(position + 4, (uint32_t)(int32_t)r_s16(config + 60));
    rrj_write32(position + 8, rrj_read32(config + 36));
    segment = sub_80039DFC(0, position);
    if (segment && sub_8003A700(segment, position, buffer))
    {
        uint32_t track = object + 328;
        uint32_t piece;
        uint32_t value;
        if (r_u16(position + 2) == 1)
            (void)sub_8003C758(buffer, config + 20);
        (void)sub_8001E0B4(track, buffer, 32);
        rrj_write32(object + 184, rrj_read32(config + 20));
        rrj_write32(object + 188, rrj_read32(config + 24));
        rrj_write32(object + 192, rrj_read32(config + 28));
        piece = rrj_read32(object + 340);
        sub_80036800(object + 184, piece, object + 344, object + 348);
        (void)sub_8002FAD4(object, 6, r_u16(config + 2), 0, call);
        (void)sub_8001298C(object, r_u16(config + 2));
        (void)sub_800A3F14(config, object);
        (void)sub_8003662C(object + 444, track, object + 360);
        rrj_write32(object + 364, rrj_read32(position + 4));
        sub_8003AF9C(object + 172, 0, other);
        rrj_write32(object + 324, sub_8003B61C(object + 172));
        rrj_write32(object + 180, r_u16(config + 2));
        rrj_put16(rrj_at(object + 322, 2), rrj_u16(rrj_at(config + 62, 2)));
        value = sub_80039F68(object + 172);
        rrj_put16(rrj_at(object + 320, 2), value);
        if ((value << 16) != 0)
        {
            int32_t offset;
            uint32_t angle;
            (void)sub_8003DE28(object, 1, 0, 0xFFFFFFFFu);
            incomplete = 0;
            piece = rrj_read32(object + 340);
            offset = rrj_s32(sub_800B6AAC(rrj_at(object + 184, 12), rrj_at(piece + 8, 6), rrj_at(piece + 20, 12)));
            (void)sub_8002EAD8(rrj_at(object + 184, 12), rrj_at(piece + 8, 6), 0u - (uint32_t)offset, rrj_at(object + 184, 12));
            (void)sub_8008BA18(object);
            angle = sub_80020018((uint32_t)(int32_t)r_s16(object + 444) << 4, (uint32_t)(int32_t)r_s16(object + 448) << 4);
            rrj_write32(object + 292, angle);
            rrj_write32(object + 296, (uint32_t)(int32_t)r_s16(0x8005624C + 4 * (angle & 0xFFF) + 2) << 4);
            rrj_write32(object + 300, (uint32_t)(int32_t)r_s16(0x8005624C + 4 * (angle & 0xFFF)) << 4);
            rrj_write32(object + 176, 0xFFFFFFFFu);
        }
    }
    if (incomplete)
        (void)sub_8008C000(object + 172, kind);
    return incomplete ^ 1;
}

uint32_t sub_800A2630(uint32_t config, uint32_t other, RRJRaceLeafCall call)
{
    uint32_t object = 0;
    uint32_t current = rrj_read32(0x800CD6CC);
    uint32_t maximum = rrj_read32(0x800CD6D0);
    uint32_t base = rrj_read32(0x800CD6D4);
    FUNCTION_MARKER(0x800A2630, "RASHCDG.BIN");
    if (rrj_s32(current) < rrj_s32(maximum) || rrj_read32(0x800D1814) >= 596)
    {
        uint32_t next = current + 1;
        uint32_t candidate = base + 596 * next;
        uint32_t secondary = rrj_read32(0x800D1660);
        uint32_t secondary_next = secondary + 1;
        while (rrj_s32(next) < rrj_s32(maximum + 1) && r_u16(candidate + 172))
        {
            ++next;
            candidate += 596;
        }
        rrj_write32(0x800CD6CC, next);
        object = base + 596 * current;
        rrj_put16(rrj_at(object + 172, 2), current + 128);
        candidate = 0x800D1664 + 24 * secondary_next;
        while (rrj_s32(secondary_next) < 18 && rrj_read32(candidate))
        {
            ++secondary_next;
            candidate += 24;
        }
        rrj_write32(0x800D1660, secondary_next);
        rrj_write32(object + 4, 0x800D1664 + 24 * secondary);
        rrj_write32(0x800CD6C8, rrj_read32(0x800CD6C8) + 1);
        if (rrj_s32(maximum) < rrj_s32(current))
        {
            rrj_write32(0x800CD6D0, current);
            rrj_write32(0x800D1814, rrj_read32(0x800D1814) - 596);
        }
    }
    if (!object || !sub_800A07D0(config, object, 4, other, call))
        return 0;
    {
        int32_t first = rrj_s32((uint32_t)sub_8001FC90(rrj_s32(rrj_read32(object + 308)), rrj_s32(rrj_read32(object + 304))));
        int32_t scale = rrj_s32((uint32_t)sub_8001FC90(rrj_s32(rrj_read32(object + 312)), first) << 9);
        uint32_t i;
        if (scale > 0x800000)
            scale = 0x800000;
        rrj_write32(object + 316, (uint32_t)scale);
        rrj_write32(object + 504, rrj_read32(object + 184));
        rrj_write32(object + 508, rrj_read32(object + 188));
        rrj_write32(object + 512, rrj_read32(object + 192));
        for (i = 0; i < 9; ++i)
            rrj_put16(rrj_at(object + 516 + 2 * i, 2), rrj_u16(rrj_at(object + 432 + 2 * i, 2)));
        rrj_write32(object + 480, 0);
        rrj_write32(object + 592, 0);
        rrj_put16(rrj_at(object + 450, 2), rrj_u16(rrj_at(object + 444, 2)));
        rrj_put16(rrj_at(object + 452, 2), rrj_u16(rrj_at(object + 446, 2)));
        rrj_put16(rrj_at(object + 454, 2), rrj_u16(rrj_at(object + 448, 2)));
        sub_8003DF54(object, 1, 0xFFFFFFFFu);
        (void)sub_800716C0(object + 432, object + 560);
    }
    return object;
}

uint32_t sub_8003C758(uint32_t segment, uint32_t position)
{
    uint32_t current;
    uint32_t end;
    uint32_t next;
    int32_t count;
    FUNCTION_MARKER(0x8003C758, "SLUS_010.53");
    if (!segment)
        return 0;
    current = rrj_read32(segment + 12);
    count = (int32_t)r_s16(rrj_read32(segment + 8) + 8) + r_s16(rrj_read32(segment + 8) + 10);
    end = rrj_read32(rrj_read32(segment) + 52) + 52 * (uint32_t)count - 52;
    if (current == end)
        return current;
    next = current + 52;
    if (rrj_s32(sub_800B6AAC(rrj_at(position, 12), rrj_at(current + 66, 6), rrj_at(current + 72, 12))) <= 0)
        return current;
    for (;;)
    {
        int32_t limit = r_s16(end);
        if (r_s16(current) >= limit)
            return current;
        current = next;
        if (r_s16(next) < limit)
            next += 52;
        if (rrj_s32(sub_800B6AAC(rrj_at(position, 12), rrj_at(next + 14, 6), rrj_at(next + 20, 12))) <= 0)
            return current;
    }
}

uint32_t sub_800302C4(uint32_t object, uint32_t id)
{
    uint32_t table = rrj_read32(0x8005B2E4);
    int32_t index;
    uint32_t result = 0xFFFFFFFFu;
    FUNCTION_MARKER(0x800302C4, "SLUS_010.53");
    if (id)
    {
        for (index = 0; index < 34; ++index)
            if (r_u8(table + 12 * (uint32_t)index) == (uint8_t)id)
                break;
        if (index >= 34)
            index = -1;
    }
    else
        index = r_s8(rrj_read32(object + 96) + 7);
    w_u16(object + 74, (int16_t)index);
    if (index != -1)
    {
        uint32_t flags = rrj_read32(object + 36);
        if ((flags & 0x3F400) == 0x3F400)
        {
            uint32_t value = r_u16(table + 12 * (uint32_t)index + 10);
            int32_t low = rrj_s32((value & 63) - 40u) >> 3;
            uint32_t field = (uint32_t)(3 * (511 - (int32_t)(value >> 6)) + low) & 63;
            rrj_write32(object + 36, (flags & 0xFFFC0FFFu) | (field << 12));
            result = 0;
        }
    }
    return result;
}

uint32_t sub_8002FDEC(uint32_t object, uint32_t id, uint32_t allocate, uint32_t bind, RRJRaceLeafCall call)
{
    uint32_t descriptor = 0;
    uint32_t models;
    uint32_t model;
    uint32_t first_count;
    uint32_t record_count;
    uint32_t records;
    uint32_t flags;
    uint32_t old_field;
    uint32_t saved_value;
    uint32_t kind;
    uint32_t i;
    int32_t index;
    FUNCTION_MARKER(0x8002FDEC, "SLUS_010.53");
    if (id < 50 && rrj_read32(0x800CE1B0 + 16 * id))
        descriptor = 0x800CE1B0 + 16 * id;
    rrj_write32(object + 96, descriptor);
    if (!descriptor)
        return 0xFFFFFFFFu;
    index = (int32_t)r_u8(descriptor + 4) - 1;
    w_u8(object + 8, (uint8_t)index);
    w_u8(object + 10, (uint8_t)index);
    w_u8(object + 11, (uint8_t)index);
    models = rrj_read32(descriptor + 8);
    model = rrj_read32(models + 12 * (uint32_t)index);
    xport_update_u8(object + 9, XPORT_MEMORY_UPDATE_OR, 3);
    rrj_write32(object, model);
    first_count = r_u16(model + 24);
    record_count = r_u16(rrj_read32(models) + 24);
    if (allocate)
        rrj_write32(object + 4, sub_8001447C(24 * record_count, 0, call));
    records = rrj_read32(object + 4);
    if (!records)
        return 0xFFFFFFFFu;
    {
        uint32_t source = rrj_read32(models + 12 * (uint32_t)index + 4);
        for (i = 0; i < first_count; ++i)
            rrj_write32(records + 24 * i, rrj_read32(source + 4 * i));
    }
    xport_update_u8(object + 9, XPORT_MEMORY_UPDATE_AND, 0xFB);
    rrj_write32(object + 56, 0);
    rrj_write32(object + 60, 0);
    rrj_write32(object + 64, 0);
    rrj_write32(object + 68, 0);
    w_u8(object + 72, 0);
    w_u8(object + 73, 0xFF);
    rrj_write32(object + 52, 0);
    rrj_write32(object + 20, 0);
    rrj_write32(object + 16, 0);
    rrj_write32(object + 12, 0);
    rrj_put16(rrj_at(object + 32, 2), 0);
    rrj_put16(rrj_at(object + 30, 2), 0);
    rrj_put16(rrj_at(object + 28, 2), 0);
    for (i = 0; i < record_count; ++i)
    {
        uint32_t record = records + 24 * i;
        rrj_put16(rrj_at(record + 6, 2), 0);
        rrj_put16(rrj_at(record + 8, 2), 0);
        rrj_put16(rrj_at(record + 10, 2), 0);
        rrj_put16(rrj_at(record + 14, 2), 0);
        rrj_put16(rrj_at(record + 16, 2), 0);
        rrj_put16(rrj_at(record + 18, 2), 0);
        rrj_put16(rrj_at(record + 4, 2), 0x1000);
        rrj_put16(rrj_at(record + 12, 2), 0x1000);
        rrj_put16(rrj_at(record + 20, 2), 0x1000);
    }
    rrj_write32(object + 44, 0x7FFFFFFF);
    rrj_write32(object + 48, 0x7FFFFFFF);
    flags = rrj_read32(object + 36);
    old_field = (flags >> 12) & 63;
    saved_value = rrj_read32(object + 76);
    rrj_write32(object + 76, 0x10000);
    rrj_write32(object + 36, 0x3F000);
    rrj_write32(object + 40, rrj_read32(model + 16));
    kind = (r_u16(model + 14) & 0x78) >> 3;
    switch (kind)
    {
        case 1:
            rrj_write32(object + 100, 0x80054198);
            rrj_write32(object + 76, saved_value);
            flags = (rrj_read32(object + 36) | 0x400) & 0xFFFC0FFFu;
            flags = (flags | (old_field << 12)) & 0xFFFBFFFFu;
            rrj_write32(object + 36, flags);
            break;
        case 2:
            rrj_write32(object + 100, 0x80054178);
            flags = (rrj_read32(object + 36) | 0x400) & 0xFFFC0FFFu;
            flags |= old_field << 12;
            flags &= 0xFFFFFFFEu;
            flags &= 0xFFFBFFFFu;
            flags &= 0xF7FFFFFFu;
            flags &= 0xDFFFFFFFu;
            flags &= 0xEFFFFFFFu;
            flags &= 0xFF87FFFFu;
            flags &= 0x3FFFFFFFu;
            rrj_write32(object + 36, flags);
            break;
        case 3:
        {
            uint32_t model_id = rrj_read32(model + 8);
            uint32_t field;
            rrj_write32(object + 100, 0x80054160);
            flags = rrj_read32(object + 36) | 0x400;
            if (model_id == 300)
                field = 0x2F000;
            else if (model_id == 315)
                field = 0x2E000;
            else if (model_id == 309)
                field = 0x1C000;
            else
                field = ((sub_8001FC58() % 15 + 14) & 63) << 12;
            flags = ((flags & 0xFFFC0FFFu) | field) & 0xFFFBFFFFu;
            rrj_write32(object + 36, flags);
            break;
        }
        case 4:
            rrj_write32(object + 100, 0x800541B8);
            rrj_write32(object + 36, rrj_read32(object + 36) | 0x400);
            break;
        default:
            rrj_write32(object + 100, 0);
            break;
    }
    w_u16(object + 74, (uint32_t)-1);
    if (bind)
        (void)sub_800302C4(object, 0);
    return 0;
}

uint32_t sub_8002FAD4(uint32_t object, uint32_t kind, uint32_t selector, uint32_t allocate, RRJRaceLeafCall call)
{
    uint32_t descriptor = 0x800D4C38 + 16 * kind;
    uint32_t selected = selector;
    uint32_t entry = 0;
    int32_t mapped = -1;
    uint32_t result;
    FUNCTION_MARKER(0x8002FAD4, "SLUS_010.53");
    switch (kind)
    {
        case 1:
        {
            uint32_t alternate = (selector - 9) < 9;
            uint32_t map_index = rrj_read32(0x8005AEC8 + 4 * alternate);
            mapped = r_s16(rrj_read32(descriptor + 12) + 2 * map_index);
            break;
        }
        case 2:
        {
            uint32_t adjusted = selector;
            if ((selector - 3) < 3 || (selector - 12) < 3)
                adjusted -= 3;
            mapped = r_s16(rrj_read32(descriptor + 12) + 2 * adjusted);
            break;
        }
        case 3:
        {
            uint32_t table = 0x800CE560 + 8 * kind;
            int32_t count = r_s16(table);
            if (!count)
                break;
            if (selector == 0xFFFF)
            {
                uint32_t state = rrj_read32(0x8005B2F8);
                uint32_t mode = r_u8(state + 4);
                uint32_t divisor = mode == 44 ? (uint32_t)(count - 2 - (int32_t)(mode & 1)) : (uint32_t)(count - (int32_t)(mode & 1));
                selector = sub_8001FC58() % divisor;
            }
            if (selector >= (uint32_t)count)
                break;
            entry = rrj_read32(table + 4) + 4 * selector;
            selected = (uint32_t)((int32_t)r_s16(entry) - rrj_s32(rrj_read32(descriptor + 8)));
            mapped = r_s16(rrj_read32(descriptor + 12) + 2 * selected);
            break;
        }
        case 4:
        {
            uint32_t table = 0x800CE560 + 8 * kind;
            uint32_t count = (uint32_t)(int32_t)r_s16(table);
            uint32_t index;
            uint32_t map_index;
            if (!count)
                break;
            index = sub_8001FC58() % count;
            if (index >= (uint32_t)(int32_t)r_s16(table))
                break;
            entry = rrj_read32(table + 4) + 4 * index;
            selected = (uint32_t)((int32_t)r_s16(entry) - rrj_s32(rrj_read32(descriptor + 8)));
            map_index = selected < 30;
            mapped = r_s16(rrj_read32(descriptor + 12) + 2 * rrj_read32(0x8005AED0 + 4 * map_index));
            break;
        }
        case 5:
        case 6:
            mapped = r_s16(rrj_read32(descriptor + 12));
            break;
        default:
            break;
    }
    if (mapped < 0)
        return 0xFFFF;
    result = sub_8002FDEC(object, (uint32_t)mapped, allocate, rrj_read32(0x80054144 + 4 * kind), call);
    if (rrj_s32(result) < 0)
        return 0xFFFF;
    switch (kind)
    {
        case 1:
            result = sub_800302C4(object, r_u8(0x80054114 + selected));
            break;
        case 2:
            result = sub_800302C4(object, r_u8(0x8005412C + selected));
            break;
        case 4:
            result = sub_800302C4(object, (uint32_t)(int32_t)r_s16(entry + 2));
            break;
        case 5:
        case 6:
            (void)sub_8001298C(object, selected);
            result = 0;
            break;
        default:
            break;
    }
    return result ? 0xFFFF : selected;
}

uint32_t sub_80092AD4(uint32_t object)
{
    uint32_t secondary = rrj_read32(object + 852);
    uint32_t type = r_u16(secondary + 544);
    uint32_t result = r_u16(0x800541D6 + 8 * type);
    FUNCTION_MARKER(0x80092AD4, "RASHCDG.BIN");
    if (!result && type)
    {
        uint32_t state;
        uint32_t linked;
        uint32_t i;
        w_u8(object + 72, 0);
        sub_800BC8DC(object);
        rrj_write32(secondary + 552, rrj_read32(secondary + 552) & 0xFFFE7FBFu);
        (void)sub_8002090C(object);
        rrj_write32(object + 568, rrj_read32(object + 568) | 0x08000000u);
        state = rrj_read32(0x8005B2F8);
        if (r_u16(object + 172) >= rrj_read32(state + 48))
        {
            uint32_t dx = leaf_abs32(rrj_s32(rrj_read32(object + 184) - rrj_read32(0x800CD950)));
            uint32_t dz = leaf_abs32(rrj_s32(rrj_read32(object + 192) - rrj_read32(0x800CD958)));
            uint32_t minimum = dx < dz ? dx : dz;
            if (rrj_s32(dx + dz - minimum / 2) > 0x780000)
            {
                (void)sub_80096564(object);
                for (i = 0; i < 9; ++i)
                    rrj_put16(rrj_at(object + 432 + 2 * i, 2), rrj_u16(rrj_at(object + 516 + 2 * i, 2)));
                rrj_put16(rrj_at(object + 450, 2), rrj_u16(rrj_at(object + 528, 2)));
                rrj_put16(rrj_at(object + 452, 2), rrj_u16(rrj_at(object + 530, 2)));
                rrj_put16(rrj_at(object + 454, 2), rrj_u16(rrj_at(object + 532, 2)));
                rrj_put16(rrj_at(object + 814, 2), rrj_u16(rrj_at(object + 516, 2)));
                rrj_put16(rrj_at(object + 816, 2), rrj_u16(rrj_at(object + 518, 2)));
                rrj_put16(rrj_at(object + 818, 2), rrj_u16(rrj_at(object + 520, 2)));
            }
        }
        (void)sub_8002EAD8(rrj_at(rrj_read32(object + 340) + 20, 12), rrj_at(object + 444, 6), 0x1E0000, rrj_at(object + 880, 12));
        linked = rrj_read32(object + 540);
        result = linked;
        if (linked)
        {
            rrj_write32(linked + 36, 0);
            result = rrj_read32(0x800CE178) - 1;
            rrj_write32(0x800CE178, result);
        }
        rrj_write32(object + 540, 0);
    }
    return result;
}

int32_t sub_80093BE0(uint32_t first, uint32_t second, uint32_t third)
{
    int32_t left = rrj_s32(sub_8002E698(rrj_at(second, 6), rrj_at(first, 6)));
    int32_t right = rrj_s32(sub_8002E698(rrj_at(third, 6), rrj_at(first, 6)));
    int32_t angle;
    FUNCTION_MARKER(0x80093BE0, "RASHCDG.BIN");
    if (leaf_abs32(left) < leaf_abs32(right))
    {
        angle = 1024 - (int32_t)sub_8001FF3C((uint32_t)left);
        return right < 0 ? -angle : angle;
    }
    angle = 1024 - (int32_t)sub_8001FF3C((uint32_t)right);
    if (left > 0)
        return 1024 - angle;
    return angle < 1024 ? angle + 1024 : angle - 3072;
}

void sub_80093CAC(uint32_t object, uint32_t vector, uint32_t scale, uint32_t magnitude, uint32_t alternate)
{
    int32_t speed;
    int32_t ratio;
    FUNCTION_MARKER(0x80093CAC, "RASHCDG.BIN");
    if (vector)
    {
        rrj_write32(object + 892, rrj_read32(vector) - rrj_read32(object + 880));
        rrj_write32(object + 896, rrj_read32(vector + 4) - rrj_read32(object + 884));
        rrj_write32(object + 900, rrj_read32(vector + 8) - rrj_read32(object + 888));
    }
    else
    {
        if (!scale || !magnitude)
            return;
        sub_8002EE50(rrj_read32(magnitude), rrj_at(scale, 6), rrj_at(object + 892, 12));
    }
    if (r_s16(object + 944) > 0)
        return;
    speed = magnitude ? (int32_t)leaf_abs32(rrj_s32(rrj_read32(magnitude))) : rrj_s32(sub_8002E548(object + 892));
    if (speed < 131)
        return;
    if (alternate)
    {
        rrj_write32(object + 904, sub_80010028(0x10000, (uint32_t)speed));
        ratio = (int32_t)sub_80010028((uint32_t)speed, 0x10000) + 0x8000;
    }
    else
    {
        rrj_write32(object + 904, sub_80010028(0xA0000, (uint32_t)speed));
        ratio = (int32_t)sub_80010028((uint32_t)speed, 0xA0000) + 0x8000;
    }
    if (ratio > 0x7FFF00)
        ratio = 0x7FFF00;
    w_u16(object + 944, (int16_t)(ratio >> 8));
    if (!(int16_t)(ratio >> 8))
        rrj_write32(object + 908, 0);
}

static void leaf_80093CAC_scalar(RRJMemory *m, uint32_t object, uint32_t scale, int32_t magnitude, uint32_t alternate)
{
    int32_t speed;
    int32_t ratio;
    if (!scale)
        return;
    sub_8002EE50((uint32_t)magnitude, rrj_at(scale, 6), rrj_at(object + 892, 12));
    if (r_s16(object + 944) > 0)
        return;
    speed = (int32_t)leaf_abs32(magnitude);
    if (speed < 131)
        return;
    if (alternate)
    {
        rrj_write32(object + 904, sub_80010028(0x10000, (uint32_t)speed));
        ratio = (int32_t)sub_80010028((uint32_t)speed, 0x10000) + 0x8000;
    }
    else
    {
        rrj_write32(object + 904, sub_80010028(0xA0000, (uint32_t)speed));
        ratio = (int32_t)sub_80010028((uint32_t)speed, 0xA0000) + 0x8000;
    }
    if (ratio > 0x7FFF00)
        ratio = 0x7FFF00;
    w_u16(object + 944, (int16_t)(ratio >> 8));
    if (!(int16_t)(ratio >> 8))
        rrj_write32(object + 908, 0);
}

uint32_t sub_800C0E98(uint32_t object, uint32_t other, int32_t delta)
{
    int32_t limit = rrj_s32(rrj_read32(0x80052F9C));
    uint32_t players;
    int32_t magnitude;
    uint32_t special = 0;
    int32_t offset;
    FUNCTION_MARKER(0x800C0E98, "RASHCDG.BIN");
    if (limit < delta)
    {
        uint32_t state = rrj_read32(0x8005B2F8);
        uint32_t value = rrj_read32(other + 480) - rrj_read32(0x80053024 + 4 * rrj_read32(state + 60));
        rrj_write32(object + 924, value);
        return value;
    }
    if (delta < -limit)
    {
        uint32_t value = rrj_read32(rrj_read32(object + 556) + 224);
        uint32_t flags = rrj_read32(object + 564);
        players = rrj_read32(rrj_read32(0x8005B2F8) + 48);
        if (r_u16(other + 172) < players)
            flags |= 0x200;
        rrj_write32(object + 924, value);
        rrj_write32(object + 564, flags);
        return r_u16(other + 172) < players;
    }
    magnitude = (int32_t)leaf_abs32(delta);
    if (magnitude < rrj_s32(rrj_read32(0x80052F70)) && r_u16(other + 172) < r_u16(object + 172) && (rrj_read32(other + 560) & 0x08000000))
    {
        uint32_t record = other + 956 + 8 * (uint32_t)r_s8(other + 946);
        if (r_u16(record - 6) == r_u16(object + 172))
        {
            uint32_t type = r_u16(record - 8);
            if (type >= 14 && type < 17)
                return type < 17;
        }
    }
    special = magnitude < rrj_s32(rrj_read32(0x80052F70));
    offset = (special || magnitude / 2 > 0x10000 ? magnitude / 2 - 0x10000 : 0) + 0x10000;
    if (delta >= 0)
        offset = -offset;
    rrj_write32(object + 924, rrj_read32(other + 480) + (uint32_t)offset);
    return rrj_read32(object + 924);
}

uint32_t sub_800B92C0(uint32_t object)
{
    uint32_t threshold = sub_8001FC58() & 0x7F;
    uint32_t sum = 0;
    uint32_t index = 0;
    FUNCTION_MARKER(0x800B92C0, "RASHCDG.BIN");
    do
        sum += r_u8(0x8005ADC4 + index++);
    while (sum < threshold);
    w_u8(object + 60, r_u8(object + 51 + index));
    return sub_800B9340(object);
}

uint32_t sub_800BA7F4(uint32_t object)
{
    FUNCTION_MARKER(0x800BA7F4, "RASHCDG.BIN");
    return sub_800BA7FC(object);
}

uint32_t sub_800BA7FC(uint32_t object)
{
    uint32_t state = rrj_read32(0x8005B2F8);
    uint32_t descriptor = rrj_read32(object + 1084);
    uint32_t options = rrj_read32(0x8005AD48);
    uint32_t selector = 0;
    uint32_t enabled;
    uint32_t result;
    FUNCTION_MARKER(0x800BA7FC, "RASHCDG.BIN");
    if (rrj_s32(rrj_read32(state + 16)) < 900 || (r_u8(descriptor) & 1))
    {
        int32_t value = (int32_t)r_u16(object + 172) - 1;
        selector = value < 0 ? 0 : (uint32_t)value;
    }
    enabled = (options & 1) || (((r_u8(descriptor + 1) & 15) == 2) && (options & 4));
    if (!enabled && r_u8(state + 57) >= 4)
        enabled = (((uint8_t)(r_u16(object + 172) >> 1) - 1) & 1) != 0;
    if (!enabled)
        enabled = selector & 1;
    if (enabled)
    {
        int32_t magnitude;
        if (!(r_u8(state + 4) & 1) || (!(options & 1) && ((r_u8(descriptor + 1) & 15) != 2 || !(options & 4))))
        {
            uint32_t piece = rrj_read32(object + 340);
            int32_t first = r_s16(piece + 2) * r_s16(object + 872);
            int32_t second = r_s16(piece + 6) * r_s16(object + 876);
            int32_t direction = rrj_s32(rrj_read32(object + 364)) < 0 ? 0x20000 : -0x20000;
            int32_t dot = (rrj_s32((uint32_t)first << 4) >> 16) + (rrj_s32((uint32_t)second << 4) >> 16);
            magnitude = dot < 0 ? -direction : direction;
        }
        else
        {
            int32_t offset = r_s16(object + 878) * 32;
            magnitude = r_u16(object + 392) == 4 ? rrj_s32(rrj_read32(object + 344) - (uint32_t)offset) : offset;
        }
        leaf_80093CAC_scalar(rrj_host_context(), object, object + 872, magnitude, 1);
        w_u16(object + 944, 1);
        rrj_write32(object + 908, 0x10000);
        result = r_u8(descriptor) | 4;
    }
    else
    {
        w_u16(object + 944, 0);
        result = r_u8(descriptor) & 0xFB;
    }
    w_u8(descriptor, (uint8_t)result);
    return result;
}

uint32_t sub_800B9340(uint32_t object)
{
    uint32_t flags = r_u8(object + 60);
    uint32_t result = flags & 0x20 ? 9 : 0;
    FUNCTION_MARKER(0x800B9340, "RASHCDG.BIN");
    if (flags & 0x80)
    {
        uint32_t mask = r_u16(object + 44);
        if (mask & 0x1FF)
        {
            uint32_t table = 0x80052EEC + 36 * (r_u8(object + 1) & 15);
            uint32_t index = 0;
            uint32_t value;
            do
            {
                value = r_u8(table + index++);
                w_u8(object + 46, (uint8_t)value);
            } while (!((mask >> (value & 31)) & 1));
            result = value < 8 ? (rrj_read32(object + 48) >> (4 * value)) & 15 : 0;
            w_u8(object + 47, (uint8_t)result);
        }
        else
        {
            w_u8(object + 46, 9);
            w_u8(object + 47, 0);
            w_u8(object + 60, 0x20);
            result = 0x20;
        }
    }
    else if (flags & 0x20)
    {
        w_u8(object + 46, 9);
        w_u8(object + 47, 0);
    }
    return result;
}

uint32_t sub_800BB280(uint32_t object, uint16_t other_id)
{
    uint32_t other;
    int32_t delta;
    uint32_t result;
    FUNCTION_MARKER(0x800BB280, "RASHCDG.BIN");
    if (other_id == 224)
        return 224;
    result = rrj_read32(object + 568) & 0x600;
    if (result)
        return result;
    other = rrj_read32(0x8005B3A0) + 1096 * other_id;
    delta = rrj_s32((rrj_read32(object + 324) - rrj_read32(other + 324)) << 4);
    if (!sub_800BB448(object, other, delta))
    {
        sub_800BC8DC(object);
        return 0;
    }
    if (rrj_s32(rrj_read32(0x80052F64)) < delta)
    {
        uint32_t value = rrj_read32(object + 924);
        if (r_u16(object + 320) & 2)
            value = rrj_read32(rrj_read32(object + 556) + 224);
        rrj_write32(object + 924, value);
        result = rrj_read32(object + 564) & 0x80000;
        if (!result)
        {
            rrj_write32(object + 916, (uint32_t)delta);
            result = (uint32_t)sub_8001FC90(rrj_s32(value), rrj_s32(value));
            rrj_write32(object + 920, result);
            rrj_write32(object + 564, rrj_read32(object + 564) & 0xFFF7FFFFu);
        }
        return result;
    }
    {
        uint32_t record = object + 940 + 8u * (uint32_t)(int32_t)r_s8(object + 946);
        if (r_u16(record) == 7 && r_u16(record + 2) == other_id)
        {
            uint32_t source = other + 956 + 8u * (uint32_t)((int32_t)r_s8(other + 946) - 1);
            uint16_t type = r_u16(source);
            rrj_put16(rrj_at(record, 2), type);
            if ((uint16_t)(type - 4) < 2)
            {
                sub_800BC8DC(object);
                return 0;
            }
        }
    }
    rrj_write32(object + 924, rrj_read32(other + 924));
    result = rrj_read32(object + 564) & 0x80000;
    if (!result)
    {
        rrj_write32(object + 916, rrj_read32(other + 916));
        rrj_write32(object + 564, rrj_read32(object + 564) & 0xFFF7FFFFu);
        rrj_write32(object + 920, rrj_read32(other + 920));
        result = rrj_read32(object + 564);
    }
    return result;
}

static int32_t leaf_rider_clearance(RRJMemory *m, uint32_t rider)
{
    if (!rrj_read32(rider + 372) || r_u16(rider + 362))
        return 0x39999;
    return (int32_t)((uint32_t)((uint64_t)rrj_read32(rider + 424) * (uint32_t)(r_s16(rider + 420) + ((rrj_s32(rrj_read32(rider + 344)) >> 31) & (r_s16(rider + 408) - r_s16(rider + 420))))));
}

uint32_t sub_800BB448(uint32_t first, uint32_t second, int32_t delta)
{
    int32_t clearance;
    uint32_t state;
    uint32_t players;
    uint32_t speed_limit;
    FUNCTION_MARKER(0x800BB448, "RASHCDG.BIN");
    if (!r_s16(second + 320) || rrj_read32(rrj_read32(second + 852) + 604) != 1)
        return 0;
    clearance = (int32_t)leaf_abs32(rrj_s32(rrj_read32(second + 344))) - leaf_rider_clearance(rrj_host_context(), second);
    if (delta < 6 * rrj_s32(rrj_read32(first + 308)) || rrj_s32(rrj_read32(0x80052F5C)) < delta || rrj_s32(rrj_read32(0x80052F50)) < clearance)
        return 0;
    state = rrj_read32(0x8005B2F8);
    players = rrj_read32(state + 48);
    if (r_u16(first + 172) >= players && (r_u8(rrj_read32(first + 1084) + 1) & 15) == 2)
        return 1;
    speed_limit = (uint32_t)((int32_t)((uint32_t)((uint64_t)rrj_read32(0x80053194 + 4 * rrj_read32(state + 60)) * rrj_read32(rrj_read32(first + 556) + 224))) / 128);
    return rrj_s32(rrj_read32(second + 480)) >= rrj_s32(speed_limit);
}

uint32_t sub_800BB8FC(uint32_t first, uint32_t second, int32_t delta)
{
    int32_t clearance;
    uint32_t players;
    FUNCTION_MARKER(0x800BB8FC, "RASHCDG.BIN");
    if (!r_s16(second + 320) || rrj_read32(rrj_read32(second + 852) + 604) != 1)
        return 0;
    clearance = (int32_t)leaf_abs32(rrj_s32(rrj_read32(second + 344))) - leaf_rider_clearance(rrj_host_context(), second);
    if (delta < rrj_s32(rrj_read32(0x80052F54)) || rrj_s32(rrj_read32(0x80052F5C)) < delta || rrj_s32(rrj_read32(0x80052F50)) < clearance)
        return 0;
    if (rrj_s32(rrj_read32(rrj_read32(first + 556) + 224)) >= rrj_s32(rrj_read32(second + 480)))
        return 1;
    players = rrj_read32(rrj_read32(0x8005B2F8) + 48);
    return r_u16(second + 172) < players;
}

uint32_t sub_800BBBB8(uint32_t first, uint32_t second, int32_t delta)
{
    int32_t difference;
    FUNCTION_MARKER(0x800BBBB8, "RASHCDG.BIN");
    if (!r_s16(second + 320) || rrj_read32(rrj_read32(second + 852) + 604) != 1)
        return 0;
    difference = rrj_s32(rrj_read32(first + 480)) - rrj_s32(rrj_read32(second + 480));
    if (delta < -rrj_s32(rrj_read32(0x80052F60)) || 2 * difference < delta)
        return 0;
    return 1;
}

uint32_t sub_800BBD44(uint32_t first, uint32_t second, int32_t delta)
{
    uint32_t linked;
    uint32_t distance;
    FUNCTION_MARKER(0x800BBD44, "RASHCDG.BIN");
    if (!r_s16(second + 320) || rrj_read32(rrj_read32(second + 852) + 604) != 1)
        return 0;
    if (leaf_abs32(delta) >= (uint32_t)rrj_s32(rrj_read32(0x80052F68)))
        return 0;
    distance = leaf_abs32(rrj_s32(rrj_read32(second + 344)) - rrj_s32(rrj_read32(first + 344)));
    linked = rrj_read32(second + 856);
    if (linked)
    {
        uint32_t linked_distance = leaf_abs32(rrj_s32(rrj_read32(linked + 344)) - rrj_s32(rrj_read32(first + 344)));
        if (distance < linked_distance)
            linked_distance = distance;
        distance = linked_distance;
    }
    return distance < (uint32_t)rrj_s32(rrj_read32(0x80052F6C));
}

uint32_t sub_800BBEBC(uint32_t object, uint16_t other_id)
{
    uint32_t other;
    uint32_t comparison;
    int32_t delta;
    uint32_t result;
    FUNCTION_MARKER(0x800BBEBC, "RASHCDG.BIN");
    if (other_id == 224)
        return 224;
    result = rrj_read32(object + 568) & 0x7FF;
    if (result)
        return result;
    other = rrj_read32(0x8005B3A0) + 1096 * other_id;
    comparison = other;
    if (rrj_read32(rrj_read32(other + 852) + 604) >= 3)
        comparison = rrj_read32(other + 852);
    if (rrj_read32(comparison + 360) == rrj_read32(object + 360))
        delta = rrj_s32(leaf_abs32(rrj_s32(rrj_read32(comparison + 368) - rrj_read32(object + 368))));
    else
    {
        int32_t dx = rrj_s32(rrj_read32(comparison + 184) - rrj_read32(object + 184)) >> 16;
        int32_t dz = rrj_s32(rrj_read32(comparison + 192) - rrj_read32(object + 192)) >> 16;
        int32_t maximum = rrj_s32(leaf_abs32(dx));
        int32_t minimum = rrj_s32(leaf_abs32(dz));
        int32_t mixed;
        if (maximum < minimum)
        {
            int32_t swap = maximum;
            maximum = minimum;
            minimum = swap;
        }
        mixed = minimum + (minimum >> 1);
        delta = rrj_s32((uint32_t)(maximum - (maximum >> 5) - (maximum >> 7) + (mixed >> 2) + (mixed >> 6)) << 16);
    }
    if (!sub_800BC120(object, other, delta))
    {
        sub_800BC8DC(object);
        return 0;
    }
    {
        int32_t first = rrj_s32(rrj_read32(comparison + 344));
        int32_t second = rrj_s32(rrj_read32(object + 344));
        int32_t side = rrj_s32(rrj_read32(comparison + 364) ^ rrj_read32(object + 364)) < 0 ? rrj_s32((uint32_t)first + (uint32_t)second) : rrj_s32((uint32_t)first - (uint32_t)second);
        int32_t scale = side >= 0 ? rrj_s32(0u - rrj_read32(0x80052F58)) : rrj_s32(rrj_read32(0x80052F58));
        int32_t vector[3];
        uint32_t i;
        uint32_t magnitude;
        uint32_t sum = 0;
        (void)sub_8002EAD8(rrj_at(comparison + 184, 12), rrj_at(comparison + 432, 6), (uint32_t)scale, rrj_at(object + 880, 12));
        for (i = 0; i < 3; ++i)
            vector[i] = rrj_s32(rrj_read32(object + 880 + 4 * i) - rrj_read32(object + 504 + 4 * i));
        if (leaf_abs32(vector[0]) > 0x5A8000 || leaf_abs32(vector[1]) > 0x5A8000 || leaf_abs32(vector[2]) > 0x5A8000)
            magnitude = 0x7FFF0000;
        else
        {
            for (i = 0; i < 3; ++i)
                sum += (uint32_t)((uint64_t)((int64_t)vector[i] * vector[i]) >> 16);
            magnitude = sub_8004CF74(sum) << 2;
        }
        result = rrj_read32(object + 564) & 0x80000;
        if (!result)
        {
            rrj_write32(object + 916, magnitude);
            rrj_write32(object + 920, 0);
            result = rrj_read32(object + 564) & 0xFFF7FFFFu;
            rrj_write32(object + 564, result);
        }
    }
    return result;
}

uint32_t sub_800BC618(uint32_t actor, uint16_t other_id, int32_t delta)
{
    uint32_t state = rrj_read32(0x8005B2F8);
    uint32_t actor_id = r_u16(actor + 172);
    uint32_t other = 0;
    uint32_t players = rrj_read32(state + 48);
    FUNCTION_MARKER(0x800BC618, "RASHCDG.BIN");
    if (actor_id < players && r_u16(0x800CCAC0 + 2 * actor_id))
    {
        uint32_t mask = r_u16(0x800CCAC0 + 2 * actor_id) & (0xFFFFu - (1u << ((other_id - 1) & 31)));
        if (mask && !(mask & (mask - 1)))
        {
            uint32_t bit = 0;
            while (!((mask >> bit) & 1))
                ++bit;
            other = rrj_read32(0x8005B3A0) + 1096 * (bit + 1);
        }
    }
    else
    {
        uint32_t record = actor + 956 + 8 * (uint32_t)r_s8(actor + 946);
        uint32_t type = r_u16(record - 8);
        uint32_t id = r_u16(record - 6);
        if ((type == 6 || type == 7 || (type >= 14 && type < 18)) && id != other_id)
            other = rrj_read32(0x8005B3A0) + 1096 * id;
    }
    if (!other || leaf_abs32(16 * (rrj_s32(rrj_read32(actor + 324)) - rrj_s32(rrj_read32(other + 324)))) > 0x7FFFF)
        return 0;
    {
        int32_t other_x = rrj_s32(rrj_read32(other + 344));
        int32_t actor_x = rrj_s32(rrj_read32(actor + 344));
        int32_t direction = (rrj_s32(rrj_read32(other + 364)) ^ rrj_s32(rrj_read32(actor + 364))) >= 0 ? other_x - actor_x : other_x + actor_x;
        return ((direction ^ delta) >> 31) + 1;
    }
}

uint32_t sub_800BBA18(uint32_t object, uint16_t other_id)
{
    uint32_t result;
    uint32_t other;
    int32_t delta;
    FUNCTION_MARKER(0x800BBA18, "RASHCDG.BIN");
    if (other_id == 224)
        return 224;
    result = rrj_read32(object + 568) & 0x600;
    if (result)
        return result;
    other = rrj_read32(0x8005B3A0) + 1096 * other_id;
    delta = rrj_s32((rrj_read32(object + 324) - rrj_read32(other + 324)) << 4);
    if (!sub_800BBBB8(object, other, delta))
    {
        if (delta < -rrj_s32(rrj_read32(0x80052F60)))
        {
            uint32_t state = rrj_read32(object + 1084);
            uint32_t other_state = rrj_read32(other + 1084);
            xport_update_u8(state + 68, XPORT_MEMORY_UPDATE_OR, 0x10);
            xport_update_u8(other_state + 68, XPORT_MEMORY_UPDATE_OR, 8);
        }
        sub_800BC8DC(object);
        return 0;
    }
    {
        int32_t other_x = rrj_s32(rrj_read32(other + 344));
        int32_t object_x = rrj_s32(rrj_read32(object + 344));
        int32_t lateral = rrj_s32(rrj_read32(other + 364) ^ rrj_read32(object + 364)) < 0 ? rrj_s32((uint32_t)other_x + (uint32_t)object_x) : rrj_s32((uint32_t)other_x - (uint32_t)object_x);
        int32_t response = lateral < 0 ? rrj_s32(rrj_read32(0x80052F58)) : rrj_s32(0u - rrj_read32(0x80052F58));
        response = rrj_s32((uint32_t)response << sub_800BC618(other, r_u16(object + 172), response));
        sub_800BC4FC(object, other, response);
    }
    result = rrj_read32(object + 924);
    if (r_u16(object + 320) & 2)
    {
        int32_t total = rrj_s32(rrj_read32(rrj_read32(object + 556) + 224) + rrj_read32(object + 480));
        result = (uint32_t)(total / 2);
    }
    rrj_write32(object + 924, result);
    rrj_write32(object + 916, 0);
    rrj_write32(object + 920, 0);
    result = rrj_read32(object + 564) & 0xFFF7FFFFu;
    rrj_write32(object + 564, result);
    return result;
}

uint32_t sub_800BBC20(uint32_t object, uint16_t other_id)
{
    uint32_t result;
    uint32_t other;
    int32_t delta;
    FUNCTION_MARKER(0x800BBC20, "RASHCDG.BIN");
    if (other_id == 224)
        return 224;
    result = rrj_read32(object + 568) & 0x600;
    if (result)
        return result;
    other = rrj_read32(0x8005B3A0) + 1096 * other_id;
    delta = rrj_s32((rrj_read32(object + 324) - rrj_read32(other + 324)) << 4);
    if (!sub_800BBD44(object, other, delta))
    {
        sub_800BC8DC(object);
        return 0;
    }
    {
        int32_t other_x = rrj_s32(rrj_read32(other + 344));
        int32_t object_x = rrj_s32(rrj_read32(object + 344));
        int32_t lateral = rrj_s32(rrj_read32(other + 364) ^ rrj_read32(object + 364)) < 0 ? rrj_s32((uint32_t)other_x + (uint32_t)object_x) : rrj_s32((uint32_t)other_x - (uint32_t)object_x);
        int32_t response = rrj_s32(rrj_read32(0x80052F6C) + 0x10000u);
        if (lateral > 0)
            response = rrj_s32(0u - (uint32_t)response);
        sub_800BC4FC(object, other, response);
    }
    result = rrj_read32(object + 924);
    if (r_u16(object + 320) & 2)
        result = rrj_read32(rrj_read32(object + 556) + 224);
    rrj_write32(object + 924, result);
    rrj_write32(object + 916, 0);
    rrj_write32(object + 920, 0);
    result = rrj_read32(object + 564) & 0xFFF7FFFFu;
    rrj_write32(object + 564, result);
    return result;
}

uint32_t sub_800BC120(uint32_t object, uint32_t other, int32_t delta)
{
    uint32_t state;
    uint32_t index;
    int32_t magnitude;
    FUNCTION_MARKER(0x800BC120, "RASHCDG.BIN");
    if (!r_s16(other + 320))
        return 0;
    if (rrj_read32(rrj_read32(other + 852) + 604) < 3 && rrj_s32(rrj_read32(other + 480)) > 585944)
        return 0;
    magnitude = rrj_s32(leaf_abs32(delta));
    if (rrj_s32(rrj_read32(0x80052F5C) << 1) < magnitude)
        return 0;
    if (rrj_s32(rrj_read32(object + 480)) > 585944)
        return 1;
    state = rrj_read32(0x8005B2F8);
    index = rrj_read32(state + 60) + ((0u - (r_u8(state + 4) & 1u)) & 3u);
    return rrj_s32(rrj_read32(0x800530A8 + 4 * index)) >= magnitude;
}

uint32_t sub_800BD388(uint32_t object)
{
    uint32_t flags;
    int32_t speed;
    FUNCTION_MARKER(0x800BD388, "RASHCDG.BIN");
    if (!(rrj_read32(object + 560) & 0x08000000))
        return 0;
    flags = rrj_read32(object + 388);
    if (!(flags & 1))
        return 0;
    if (rrj_s32(rrj_read32(object + 576)) <= 147455)
    {
        uint32_t piece = rrj_read32(object + 340);
        sub_8002EAD8(rrj_at(piece + 20, 12), rrj_at(piece + 14, 6), rrj_read32(object + 348), rrj_at(object + 880, 12));
        w_u16(object + 878, 0);
        xport_update_u8(object + 928, XPORT_MEMORY_UPDATE_OR, 0x40);
        return 1;
    }
    if (!(flags & 0x10))
        return 0;
    speed = rrj_s32(rrj_read32(object + 344));
    {
        int32_t threshold = rrj_s32((flags >> 8 & 0xFFFu) << 16);
        int32_t magnitude = rrj_s32(leaf_abs32(speed));
        int32_t response;
        if (threshold < 0)
            threshold += 7;
        threshold >>= 3;
        if (threshold >= magnitude || magnitude >= threshold + 0x50000)
            return 0;
        response = threshold + 0x10000;
        if (speed < 0)
            response = rrj_s32(0u - (uint32_t)response);
        response = rrj_s32((uint32_t)response - ((uint32_t)(int32_t)r_s16(object + 878) << 5));
        sub_8002EAD8(rrj_at(object + 880, 12), rrj_at(object + 872, 6), (uint32_t)response, rrj_at(object + 880, 12));
        w_u16(object + 944, 0);
        w_u16(object + 878, (int16_t)(r_s16(object + 878) + (response >> 5)));
        rrj_write32(object + 924, (8 * rrj_read32(rrj_read32(0x8005B2F8) + 60) + 24) << 16);
    }
    return 1;
}

uint32_t sub_800BC1EC(uint32_t object, uint32_t other, int32_t delta)
{
    uint32_t state = rrj_read32(0x8005B2F8);
    uint32_t other_id = r_u16(other + 172);
    uint32_t players = rrj_read32(state + 48);
    uint32_t other_class;
    int32_t separation;
    int32_t limit;
    FUNCTION_MARKER(0x800BC1EC, "RASHCDG.BIN");
    if (!r_s16(other + 320))
        return 0;
    other_class = rrj_read32(rrj_read32(other + 852) + 604);
    if (other_class >= 2 || (other_id >= players && other_class != 1))
        return 0;
    if (r_u8(state + 57) == 3 && other_id < players)
        return 1;
    limit = rrj_s32(rrj_read32(0x80052F84));
    if (other_id < players)
        limit = rrj_s32((uint32_t)limit << 2);
    if (rrj_read32(other + 360) == rrj_read32(object + 360) && (!(rrj_read32(other + 360) >> 16) || rrj_read32(other + 336) == rrj_read32(object + 336)))
    {
        separation = rrj_s32(rrj_read32(other + 344) - rrj_read32(object + 344));
        if (rrj_s32(rrj_read32(object + 364)) < 0)
            separation = rrj_s32(0u - (uint32_t)separation);
    }
    else
        separation = rrj_s32(sub_800B6AAC(rrj_at(other + 184, 12), rrj_at(object + 432, 6), rrj_at(object + 184, 12)));
    if (rrj_read32(other + 856) && separation < 0)
        separation = rrj_s32((uint32_t)separation + rrj_read32(other + 304) + rrj_read32(rrj_read32(other + 856) + 304));
    if (rrj_s32(leaf_abs32(separation)) > limit)
        return 0;
    limit = rrj_s32(rrj_read32(0x80052F80));
    if (other_id < players)
        limit = rrj_s32((uint32_t)limit << 2);
    if (rrj_s32(leaf_abs32(delta)) > limit)
        return 0;
    if (rrj_read32(object + 560) & 0x08000000)
    {
        uint32_t type = r_u8(rrj_read32(object + 1084) + 1) & 15;
        int32_t product = (int32_t)((uint32_t)r_u8(0x80052EE4 + 36 * type + 18) * rrj_read32(rrj_read32(object + 556) + 224));
        int32_t speed_limit = product < 0 ? (product + 127) >> 7 : product >> 7;
        if (rrj_s32(rrj_read32(other + 480)) < speed_limit)
            return 0;
    }
    if (!(rrj_read32(object + 560) & 0x08000000) || r_u16(other + 362) == 1)
        return 1;
    return rrj_s32(rrj_read32(0x80052F50)) >= rrj_s32(leaf_abs32(rrj_s32(rrj_read32(other + 344))) - (uint32_t)leaf_rider_clearance(rrj_host_context(), other));
}

uint32_t sub_800BF424(uint32_t object, uint32_t value)
{
    uint32_t state = rrj_read32(0x8005B2F8);
    uint32_t actor_id = r_u16(object + 172);
    uint32_t index;
    uint32_t record;
    FUNCTION_MARKER(0x800BF424, "RASHCDG.BIN");
    if (actor_id < rrj_read32(state + 48))
        index = actor_id;
    else
    {
        uint32_t existing = rrj_read32(object + 1088);
        if (existing)
            return existing;
        index = r_u16(rrj_read32(object + 856) + 172) + 2;
    }
    record = 0x800CD548 + 12 * index;
    rrj_write32(record, object);
    rrj_write32(record + 4, value);
    rrj_write32(record + 8, rrj_read32(state + 16));
    return record;
}

uint32_t sub_800BF604(uint32_t object, uint32_t expected, uint32_t index, uint32_t direction)
{
    uint32_t result = r_u8(0x80053210 + index);
    FUNCTION_MARKER(0x800BF604, "RASHCDG.BIN");
    if (expected == result)
    {
        uint32_t record = 0x800531E8 + 2 * index;
        uint32_t magnitude = (uint32_t)r_u8(record + 1) << 10;
        rrj_write32(object + 720, (uint32_t)r_u8(record) << 8);
        rrj_write32(object + 564, rrj_read32(object + 564) | 0x01000000u);
        result = magnitude + ((0u - direction) & (0u - 2 * magnitude));
        rrj_write32(object + 656, result);
    }
    return result;
}

uint32_t sub_800BF860(uint32_t object, uint32_t state, uint32_t alternate, uint32_t value_out)
{
    uint32_t first = r_u8(object + 569);
    uint32_t second = r_u8(object + r_u8(object + 570) + 574);
    uint32_t table = rrj_read32(rrj_read32(0x8005AD4C) + 12 * first + 8) + 12 * second;
    uint32_t result;
    FUNCTION_MARKER(0x800BF860, "RASHCDG.BIN");
    if (alternate)
    {
        rrj_write32(value_out, r_u16(table + 8));
        result = r_u16(table + 4);
    }
    else
        result = r_u16(table + 6);
    if (r_u8(state + 572) & 0x20)
    {
        switch (result)
        {
            case 30:
            case 35:
                result = 83;
                break;
            case 31:
            case 32:
                result = 85;
                break;
            case 33:
                result = 86;
                break;
            case 34:
                result = 84;
                break;
            default:
                result = 87;
                break;
        }
    }
    return result;
}

uint32_t sub_800BFD74(uint32_t object, int32_t value)
{
    int32_t product = rrj_s32((uint32_t)value * 150u);
    int32_t level = product < 0 ? (product + 127) >> 7 : product >> 7;
    int32_t index;
    uint32_t clamped = level < 0 ? 0 : level > 255 ? 255u : (uint32_t)level;
    FUNCTION_MARKER(0x800BFD74, "RASHCDG.BIN");
    if (rrj_read32(object + 1088))
        index = r_u16(object + 172);
    else if (!(r_u8(rrj_read32(object + 852) + 572) & 0x40))
        index = (int32_t)r_u16(rrj_read32(object + 856) + 172) + 1 + (rrj_read32(rrj_read32(0x8005B2F8) + 48) >= 2);
    else
        index = -1;
    if (index >= 0)
    {
        product = rrj_s32((uint32_t)value * 100u);
        return sub_8001DD74((uint32_t)index, product < 0 ? (product + 127) >> 7 : product >> 7, 100, clamped);
    }
    return (uint32_t)value << 1;
}

void sub_800BC4FC(uint32_t object, uint32_t other, int32_t delta)
{
    uint32_t linked;
    int32_t direction;
    uint32_t vector;
    int32_t first_product;
    int32_t second_product;
    uint32_t magnitude = 0x1F8003FC;
    FUNCTION_MARKER(0x800BC4FC, "RASHCDG.BIN");
    if ((r_u16(other + 172) >> 5) == 0)
    {
        linked = rrj_read32(other + 856);
        direction = rrj_s32(rrj_read32(other + 364));
        if (linked && ((direction >= 0) != (delta < 0)))
        {
            uint32_t radius = rrj_read32(other + 304) + rrj_read32(linked + 304);
            delta = direction < 0 ? rrj_s32((uint32_t)delta - radius) : rrj_s32((uint32_t)delta + radius);
        }
    }
    vector = rrj_read32(other + 340);
    first_product = (int32_t)((uint32_t)(int32_t)r_s16(vector + 2) * (uint32_t)(int32_t)r_s16(object + 872));
    second_product = (int32_t)((uint32_t)(int32_t)r_s16(vector + 6) * (uint32_t)(int32_t)r_s16(object + 876));
    delta = rrj_s32((uint32_t)delta + rrj_read32(other + 344) - ((uint32_t)(int32_t)r_s16(object + 878) << 5));
    first_product = (int32_t)((uint32_t)first_product << 4) >> 16;
    second_product = (int32_t)((uint32_t)second_product << 4) >> 16;
    if (rrj_s32((uint32_t)first_product + (uint32_t)second_product) < 0)
        delta = rrj_s32(0u - (uint32_t)delta);
    rrj_write32(magnitude, (uint32_t)delta);
    sub_80093CAC(object, 0, object + 872, magnitude, 1);
}

uint32_t sub_800C0BE8(uint32_t object, uint32_t other, int32_t delta, uint32_t flags)
{
    uint32_t target;
    FUNCTION_MARKER(0x800C0BE8, "RASHCDG.BIN");
    if (!(flags & 1))
    {
        if (rrj_read32(object + 1088) && sub_800BB8FC(object, other, rrj_s32(0u - (uint32_t)delta)))
        {
            uint32_t event = 0x1F8003F8;
            w_u16(event, 6);
            w_u16(event + 2, r_u16(other + 172));
            sub_800BCA68(event, 2, object);
            return 0;
        }
    }
    else
    {
        uint32_t relation = flags & 2 ? rrj_read32(rrj_read32(other + 856) + 1084) : rrj_read32(other + 1084);
        uint32_t occupied = r_u8(relation + 15);
        uint32_t packed = r_u8(rrj_read32(object + 1084) + 61);
        if ((packed & 15) < (packed >> 4) && occupied)
            return 1;
    }
    target = rrj_read32(object + 852);
    if (r_u8(target + 572) & 0x40)
        sub_800C4550(77, target, 1);
    sub_800BC8DC(object);
    return 0;
}

uint32_t sub_800C0CE8(uint32_t object, uint32_t other, int32_t delta, RRJRaceLeafCall call)
{
    int32_t position_delta = rrj_s32(rrj_read32(other + 480) - rrj_read32(object + 480));
    int32_t threshold = rrj_s32(leaf_abs32(position_delta) * 4u);
    int32_t magnitude = rrj_s32(leaf_abs32(delta));
    FUNCTION_MARKER(0x800C0CE8, "RASHCDG.BIN");
    if (threshold < rrj_s32(rrj_read32(0x80052F9C)))
        threshold = rrj_s32(rrj_read32(0x80052F9C));
    if (magnitude < threshold)
    {
        int32_t other_x = rrj_s32(rrj_read32(other + 344));
        int32_t object_x = rrj_s32(rrj_read32(object + 344));
        int32_t lateral = rrj_s32(rrj_read32(other + 364) ^ rrj_read32(object + 364)) < 0 ? rrj_s32((uint32_t)other_x + (uint32_t)object_x) : rrj_s32((uint32_t)other_x - (uint32_t)object_x);
        uint32_t kind = 10;
        int32_t response;
        if (magnitude <= rrj_s32(rrj_read32(0x80052F70)))
        {
            kind = leaf_abs32(lateral) < rrj_read32(0x80052F74) ? 9 : 11;
        }
        if (kind == 11 && r_u16(other + 172) < rrj_read32(rrj_read32(0x8005B2F8) + 48))
            sub_8001A760(r_u16(object + 172), 0);
        response = rrj_s32(rrj_read32(0x80052F50 + kind * 4));
        if (lateral > 0)
            response = rrj_s32(0u - (uint32_t)response);
        if (sub_800BC618(other, r_u16(object + 172), response))
            response = rrj_s32(0u - (uint32_t)response);
        sub_800BC4FC(object, other, response);
    }
    sub_800C0E98(object, other, delta);
    rrj_write32(object + 916, 0);
    rrj_write32(object + 920, 0);
    rrj_write32(object + 564, rrj_read32(object + 564) & 0xFFF7FFFFu);
    return rrj_read32(object + 564);
}

uint32_t sub_800C09B0(uint32_t object)
{
    uint32_t body, index, animation, action = 0;
    FUNCTION_MARKER(0x800C09B0, "RASHCDG.BIN");
    body = rrj_read32(object + 852);
    index = r_u8(body + 570);
    animation = r_u8(body + 574 + index);
    if (animation == 63)
    {
        if (sub_8005BE58(rrj_read32(body + 540)))
            action = 8;
    }
    else if (sub_8005BE58(rrj_read32(body + 540)))
    {
        uint32_t next;
        index = r_u8(body + 570) + 1u;
        next = r_u8(body + 574 + index);
        w_u8(body + 570, (uint8_t)index);
        if (next == 63)
            action = 4;
        else
        {
            animation = next;
            action = 2;
        }
    }
    else if (!r_u8(body + 546))
    {
        uint32_t group_index = r_u8(body + 569);
        uint32_t groups = rrj_read32(0x8005AD4C);
        uint32_t animation_object = rrj_read32(body + 540);
        uint32_t table = rrj_read32(groups + 12u * group_index + 8);
        int32_t frame = rrj_s32(rrj_read32(animation_object + 16));
        uint32_t limit = r_u8(table + 12u * animation + 11);
        action = frame >= (int32_t)limit;
        w_u8(body + 546, (uint8_t)action);
    }
    if (action & 14u)
    {
        uint32_t flags = rrj_read32(body + 552) & 0x08000000u ? 0x110 : 0x10;
        uint32_t group_index = r_u8(body + 569);
        uint32_t group = rrj_read32(0x8005AD4C) + 12u * group_index;
        uint32_t type;
        if (action & 2u)
            type = r_u16(rrj_read32(group + 8) + 12u * animation);
        else if (action & 4u)
        {
            type = r_u16(rrj_read32(group + 8) + 12u * animation + 2);
            if (type == 224)
            {
                type = r_u16(group);
                flags |= 2;
            }
        }
        else
            type = r_u16(group);
        rrj_trace_dispatch_target(0x800C4550u);
        (void)sub_800C4550(type, body, flags);
        rrj_trace_dispatch_target(0x800C09B0u);
    }
    return action & 1;
}

uint32_t sub_800C159C(uint32_t first, uint32_t second, int32_t direction, int32_t delta, uint32_t flags_out)
{
    uint32_t body = rrj_read32(first + 852);
    uint32_t group = rrj_read32(0x8005AD4C) + 12 * r_u8(body + 569);
    uint32_t animation = r_u8(body + 574 + r_u8(body + 570));
    uint32_t table = rrj_read32(group + 8);
    uint32_t distance = leaf_abs32(rrj_s32(rrj_read32(second + 188) - rrj_read32(first + 188)));
    uint32_t flags = rrj_read32(flags_out);
    uint32_t active = (r_u16(table + 12 * animation + 8) && rrj_s32(distance) < rrj_s32(rrj_read32(first + 312))) || ((flags >> 8) & 1);
    FUNCTION_MARKER(0x800C159C, "RASHCDG.BIN");
    if (active && leaf_abs32(delta) <= 65534)
    {
        int32_t phase = rrj_s32(rrj_read32(first + 636) - rrj_read32(second + 636));
        int32_t second_limit = rrj_s32(rrj_read32(second + 312));
        if (direction > 0)
            phase = rrj_s32(0u - (uint32_t)phase);
        if (rrj_s32(distance) < second_limit / 2)
        {
            uint32_t curve_index = ((uint32_t)phase * 163u >> 12) & 0x3FFCu;
            int32_t curve = r_s16(0x8005624C + curve_index) * 16;
            int32_t scaled = rrj_s32((uint32_t)sub_8001FC90(curve, rrj_s32(rrj_read32(second + 188) - rrj_read32(first + 188)) + 3 * second_limit / 4));
            uint32_t threshold = (uint32_t)scaled + ((uint32_t)r_u8(table + 12 * animation + 10) << 12);
            threshold += rrj_read32(rrj_read32(first + 1084) + 8) + rrj_read32(first + 304) + rrj_read32(second + 304);
            if (rrj_s32(leaf_abs32(direction)) < rrj_s32(threshold))
                flags |= 1;
        }
    }
    flags |= 2;
    rrj_write32(flags_out, flags);
    return active && !(flags & 0x80);
}

uint32_t sub_800C2030(uint32_t object)
{
    uint32_t index = (uint32_t)r_u8(object + 570) + 1;
    FUNCTION_MARKER(0x800C2030, "RASHCDG.BIN");
    xport_update_u8(object + 572, XPORT_MEMORY_UPDATE_OR, 2);
    while (index < 6)
    {
        w_u8(object + index + 574, 63);
        ++index;
    }
    return object + index;
}

uint32_t sub_800C2E9C(uint32_t object, uint8_t event)
{
    uint32_t state_index = r_u8(object + 570);
    uint32_t state = r_u8(object + state_index + 574);
    uint32_t context;
    int32_t frame;
    int32_t boundary;
    FUNCTION_MARKER(0x800C2E9C, "RASHCDG.BIN");
    if (r_u8(object + 569) || (int8_t)state != 1)
        return 0;
    context = rrj_read32(object + 540);
    frame = rrj_s32(rrj_read32(context + 16) << 2);
    {
        uint32_t index = rrj_read32(context + 12);
        uint32_t selector = r_u8(rrj_read32(context + 4) + 12 * index);
        uint32_t table = rrj_read32(rrj_read32(context + 40) + 4);
        boundary = (int16_t)(r_u16(rrj_read32(table + 4 * selector) + 16) - 1);
    }
    if (boundary < frame && frame < 3 * boundary && (!(r_u8(object + 572) & 0x20) || !event || event == 2 || event == 6 || event == 7))
        return 1;
    return 0;
}

void sub_800C92F8(void)
{
    uint32_t object = rrj_read32(0x8005B38C);
    uint32_t marker = rrj_read32(0x8005B2E8);
    uint32_t flags = rrj_read32(object + 560);
    int32_t side = rrj_s32(rrj_read32(object + 364) ^ marker);
    FUNCTION_MARKER(0x800C92F8, "RASHCDG.BIN");
    if (side < 0 && !(flags & 0x08000000u))
    {
        int32_t dot = rrj_s32(sub_8002E698(rrj_at(rrj_read32(object + 340) + 14, 6), rrj_at(object + 528, 6)));
        if (dot < -46333)
        {
            rrj_write32(object + 704, 0);
            rrj_write32(object + 560, flags | 0x28000000u);
            rrj_write32(object + 564, rrj_read32(object + 564) & 0xFFFFFDFFu);
        }
        return;
    }
    if (side >= 0 && (flags & 0x08000000u))
    {
        int32_t boundary = rrj_s32((uint32_t)(r_u16(0x8005317A) - 25u) << 16);
        if (rrj_s32(rrj_read32(object + 368)) < boundary)
        {
            int32_t dot = rrj_s32(sub_8002E698(rrj_at(rrj_read32(object + 340) + 14, 6), rrj_at(object + 528, 6)));
            if (dot > 62914)
                rrj_write32(object + 560, flags & 0xD7FFFFFFu);
        }
    }
}

uint32_t sub_800C2348(uint32_t input, uint32_t entity)
{
    uint32_t state = rrj_read32(entity + 1084);
    uint32_t changed = 0;
    uint32_t value;
    uint32_t code = 0;
    uint32_t play = 0;
    FUNCTION_MARKER(0x800C2348, "RASHCDG.BIN");
    if (sub_800C2178(input, 8))
        code = 77;
    else if (sub_800C2178(input, 4))
        code = 75;
    else if (sub_800C2178(input, 3))
        code = 71;
    else if (sub_800C2178(input, 7))
    {
        value = r_u8(state + 46);
        if (value == 9)
            code = 38;
        else if (value == 5 || value == 1)
            code = 148;
        else if ((uint8_t)(value - 6) < 3)
            play = 1;
    }
    else if (sub_800C2178(input, 5))
    {
        value = r_u8(state + 46);
        if ((uint32_t)(value - 2) < 2 || value == 0 || value == 8)
            code = 148;
        else if ((uint8_t)(value - 6) < 2)
            play = 1;
    }
    else if (sub_800C2178(input, 6))
    {
        value = r_u8(state + 46);
        if (value == 4 || value == 6 || value == 7)
            code = 148;
        else if (value == 8)
            play = 1;
    }
    else if (sub_800C2178(input, 1))
    {
        value = r_u8(state + 46);
        if (value == 9)
            code = 32;
        else if ((uint8_t)(value - 6) >= 3)
            code = 142;
        else
            play = 1;
    }
    else if (sub_800C2178(input, 2))
    {
        value = r_u8(state + 46);
        if (value == 9)
            code = 36;
        else if ((uint8_t)(value - 6) < 3)
            play = 1;
        else
            code = 146;
    }
    if (code)
    {
        w_u8(state + 60, (uint8_t)code);
        changed = 1;
    }
    else if (play)
    {
        sub_80017BA0(rrj_s32(rrj_read32(entity + 184)), rrj_s32(rrj_read32(entity + 192)), 78, 0);
    }
    if (changed)
    {
        (uint32_t)sub_800C1DD4(entity);
        w_u8(rrj_read32(entity + 852) + 573, 0);
    }
    return changed;
}

uint32_t sub_800116B8(RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x800116B8, "SLUS_010.53");
    sub_80043DD4();
    leaf_call(rrj_host_context(), call, 0x80044A94, 0, 0, 0, 0);
    leaf_call(rrj_host_context(), call, 0x80044A14, 0, 0, 0, 0);
    leaf_call(rrj_host_context(), call, 0x80044B24, 0, 0, 0, 0);
    leaf_call(rrj_host_context(), call, 0x80043D54, 0, 0, 0, 0);
    sub_80043DF4();
    return leaf_call(rrj_host_context(), call, 0x80043E14, 0, 0, 0, 0);
}

uint32_t sub_80011708(RRJRaceLeafCall call)
{
    uint32_t result;
    FUNCTION_MARKER(0x80011708, "SLUS_010.53");
    sub_80043DD4();
    result = leaf_call(rrj_host_context(), call, 0x80044B6C, 0, 0, 0, 0);
    sub_80043DF4();
    return result;
}

uint32_t sub_80011738(void)
{
    uint32_t state = 0x800D5D38;
    FUNCTION_MARKER(0x80011738, "SLUS_010.53");
    rrj_write32(0x8005B2F8, state);
    w_u8(state + 3, 1);
    w_u8(state + 4, 34);
    rrj_write32(state + 64, 4);
    rrj_write32(state + 76, 9);
    w_u8(state + 5, 3);
    w_u8(state + 7, 3);
    rrj_write32(state + 12, 0);
    rrj_put16(rrj_at(state + 40, 2), 0);
    rrj_put16(rrj_at(state + 42, 2), 0);
    rrj_write32(state + 44, 0);
    rrj_put16(rrj_at(state + 58, 2), 0);
    rrj_write32(state + 60, 0);
    rrj_write32(state + 68, 0);
    w_u8(state + 56, 0);
    rrj_write32(state + 48, 1);
    rrj_write32(state + 72, 0);
    rrj_write32(state + 88, 0);
    rrj_write32(state + 92, 0);
    rrj_write32(state + 96, 1);
    w_u8(state + 6, 1);
    rrj_put16(rrj_at(state + 8, 2), 120);
    return state;
}

uint32_t sub_800117BC(RRJRaceLeafCall call)
{
    uint32_t state;
    uint32_t result;
    FUNCTION_MARKER(0x800117BC, "SLUS_010.53");
    sub_80011738();
    state = rrj_read32(0x8005B2F8);
    w_u8(state, 2);
    sub_8001C004(call);
    result = sub_80014528(0x80052318, call);
    if (rrj_s32(result) < 0)
        leaf_call(rrj_host_context(), call, 0x80044894, 0x80010744, 0, 0, 0);
    rrj_write32(0x8005AD5C, 0);
    sub_8001BFA8(call);
    sub_8001B494(call);
    sub_8001BC54(call);
    state = rrj_read32(0x8005B2F8);
    if (r_s8(state) == 2)
        sub_80018C1C(0);
    sub_800116B8(call);
    sub_8001C590(call);
    sub_800163D4(call);
    sub_8001BDB0(call);
    sub_8001462C();
    sub_80018C1C(1);
    sub_8001DFE4(call);
    return sub_800229B0();
}

uint32_t sub_800118A0(uint32_t mode, uint32_t buffer, RRJRaceLeafCall call)
{
    uint32_t path = 0x1F800200;
    uint32_t result_out = 0x1F8003D8;
    uint32_t source;
    FUNCTION_MARKER(0x800118A0, "SLUS_010.53");
    if (mode == 2)
        source = 0x8001075C;
    else if (mode == 4)
        source = 0x80010768;
    else if (mode == 8)
        source = 0x80010774;
    else if (mode == 16)
        source = 0x80010780;
    else
        return 0x80010000;
    if (mode != 8)
        rrj_write32(0x8005ACA8, 1);
    leaf_call(rrj_host_context(), call, 0x80043FD4, path, 0x8005AC94, source, 0);
    while (rrj_s32(sub_80014680(path, rrj_read32(0x8005AD5C), buffer, result_out, call)) < 0)
        leaf_call(rrj_host_context(), call, 0x80044894, 0x8001078C, path, buffer, rrj_read32(result_out));
    sub_80043DD4();
    leaf_call(rrj_host_context(), call, 0x80043D44, 0, 0, 0, 0);
    sub_80043DF4();
    rrj_write32(0x8005ACA8, rrj_read32(0x8005ACA8) | mode);
    return rrj_read32(0x8005ACA8);
}

uint32_t sub_800119C0(RRJRaceLeafCall call)
{
    int32_t remaining = rrj_s32(rrj_read32(rrj_read32(0x800CE4DC)));
    uint32_t entity = rrj_read32(0x800CE4D0);
    uint32_t stride = rrj_read32(0x800CE4D4);
    uint32_t state;
    uint32_t result;
    FUNCTION_MARKER(0x800119C0, "SLUS_010.53");
    while (remaining >= 0)
    {
        uint32_t object = rrj_read32(entity + 0x354);
        uint32_t animation = sub_80012884(0x800CE170, object);
        uint32_t kind = rrj_read32(entity + 0xB4);
        rrj_write32(object + 0x21C, animation);
        if (rrj_read32(0x800CE190))
            sub_80012858(animation, rrj_read32(0x800CE190));
        leaf_call(rrj_host_context(), call, 0x800C4550, (r_u16(object + 0xAC) & 1) ? 4 : 6, object, 1, 0);
        if (r_s16(entity + 0x140))
        {
            leaf_call(rrj_host_context(), call, 0x8008B99C, entity, 0, 0, 0);
            leaf_call(rrj_host_context(), call, 0x8008BA18, entity, 0, 0, 0);
        }
        else
        {
            leaf_call(rrj_host_context(), call, 0x80093F94, entity, 1, 0, 0);
        }
        if (kind - 6 < 3 || kind - 15 < 3)
        {
            uint32_t secondary = rrj_read32(rrj_read32(entity + 0x358) + 0x354);
            animation = sub_80012884(0x800CE170, secondary);
            rrj_write32(secondary + 0x21C, animation);
            if (rrj_read32(0x800CE1A8))
                sub_80012858(animation, rrj_read32(0x800CE1A8));
            if (r_u8(secondary + 0x23C) & 0x20)
                leaf_call(rrj_host_context(), call, 0x800C4550, 77, secondary, 1, 0);
        }
        --remaining;
        entity += stride;
    }
    leaf_call(rrj_host_context(), call, 0x800A41EC, 0, 0, 0, 0);
    leaf_call(rrj_host_context(), call, 0x800A4774, 0, 0, 0, 0);
    leaf_call(rrj_host_context(), call, 0x8008D89C, 0, 0, 0, 0);
    leaf_call(rrj_host_context(), call, 0x8009C308, 0, 0, 0, 0);
    state = rrj_read32(0x8005B2F8);
    if (!(r_u8(state + 4) & 1))
        leaf_call(rrj_host_context(), call, 0x800901A8, 1, 0, 0, 0);
    if (rrj_read32(0x8005B220))
    {
        uint32_t object = rrj_read32(0x8005B38C);
        rrj_write32(object + 0x230, rrj_read32(object + 0x230) | 0x08000000);
        sub_80012764(0);
    }
    sub_8001654C();
    if (rrj_read32(state + 0x30) == 1)
    {
        while (rrj_read32(0x800CD694) == 2)
        {
            leaf_call(rrj_host_context(), call, 0x80030608, 0, 0, 0, 0);
            sub_800247E8(call);
        }
    }
    else
    {
        sub_800164B4(call);
    }
    sub_8001A424(call);
    result = rrj_read32(0x8005B220);
    if (result)
        return sub_80018C1C(0);
    return result;
}

uint32_t sub_800121E4(RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x800121E4, "SLUS_010.53");
    sub_80020228();
    sub_80011708(call);
    sub_8001643C(call);
    sub_800229F4(2, call);
    return sub_8001B4C0(call);
}

uint32_t sub_80012224(RRJRaceLeafCall call)
{
    uint32_t state;
    int32_t mode;
    FUNCTION_MARKER(0x80012224, "SLUS_010.53");
    leaf_call(rrj_host_context(), call, 0x800403CC, 0, 0, 0, 0);
    sub_800140E8();
    sub_8001444C();
    sub_800117BC(call);
    sub_8002E080(call);
    state = rrj_read32(0x8005B2F8);
    if (r_s8(state) == 2)
    {
        sub_800118A0(16, rrj_read32(0x80010E34), call);
        leaf_call(rrj_host_context(), call, 0x8007FEDC, 0, 0, 0, 0);
    }
    while (r_s8(state = rrj_read32(0x8005B2F8)))
    {
        if (r_s8(state) == 2)
        {
            sub_800118A0(16, rrj_read32(0x80010E34), call);
            sub_8001B868();
            leaf_call(rrj_host_context(), call, 0x8007FF4C, 0, 0, 0, 0);
        }
        sub_800118A0(2, rrj_read32(0x80010E34), call);
        leaf_call(rrj_host_context(), call, 0x80064610, 0, 0, 0, 0);
        do
        {
            leaf_call(rrj_host_context(), call, 0x80063B90, 0, 0, 0, 0);
            sub_800118A0(4, rrj_read32(0x80010E34), call);
            sub_8001F080(call);
            state = rrj_read32(0x8005B2F8);
            if (rrj_read32(state + 0x30) == 1)
            {
                sub_800247A0(call);
                sub_80020E30(0);
            }
            sub_800119C0(call);
            sub_8001B868();
            state = rrj_read32(0x8005B2F8);
            for (;;)
            {
                mode = r_s8(state);
                if (!mode || mode == 2 || r_s8(state) == 5)
                    break;
                (uint32_t)sub_8001CB3C_race(call);
                state = rrj_read32(0x8005B2F8);
                if (r_s8(state + 3))
                {
                    rrj_write32(0x8005B228, 0x00050000);
                    rrj_put16(rrj_at(0x800CD542, 2), 0);
                    rrj_put16(rrj_at(0x800CD540, 2), 0);
                    leaf_call(rrj_host_context(), call, 0x8008AB00, 1094, 0, 0, 0);
                    leaf_call(rrj_host_context(), call, 0x800881B4, 0x800CD898, 1094, 0, 0);
                    state = rrj_read32(0x8005B2F8);
                    if (rrj_read32(state + 0x30) >= 2)
                        leaf_call(rrj_host_context(), call, 0x800881B4, 0x800CDD04, 1094, 0, 0);
                    leaf_call(rrj_host_context(), call, 0x8008CFDC, 0x800CDD04, 0, 0, 0);
                    leaf_call(rrj_host_context(), call, 0x8005E1D8, 0x800CE170, 0, 0, 0);
                    state = rrj_read32(0x8005B2F8);
                    w_u8(state + 3, 0);
                }
                else
                {
                    (uint32_t)sub_8001C428();
                }
                leaf_call(rrj_host_context(), call, 0x80011C4C, 0, 0, 0, 0);
                state = rrj_read32(0x8005B2F8);
                if (r_s8(state + 2))
                {
                    w_u8(state, 3);
                    state = rrj_read32(0x8005B2F8);
                    w_u8(state + 2, 0);
                    sub_80018C1C(1);
                    sub_80020E30(1);
                    state = rrj_read32(0x8005B2F8);
                }
            }
            leaf_call(rrj_host_context(), call, 0x8001C408, 0, 0, 0, 0);
            sub_80022FC0(call);
            sub_800118A0(2, rrj_read32(0x80010E34), call);
            leaf_call(rrj_host_context(), call, 0x80063A20, 0, 0, 0, 0);
            state = rrj_read32(0x8005B2F8);
            if (r_s8(state) != 5)
                break;
            leaf_call(rrj_host_context(), call, 0x80063FA0, 0, 0, 0, 0);
        } while (r_s8(rrj_read32(0x8005B2F8)) == 5);
    }
    sub_800121E4(call);
    return 0;
}

/* Continuation at the recorded race-loop checkpoint */
uint32_t sub_80012370(RRJRaceLeafCall call)
{
    uint32_t state = 0;
    int32_t mode;
    int resume_frame = 1;
    int first_inner = 1;
    while (resume_frame || r_s8(state = rrj_read32(0x8005B2F8)))
    {
        if (!resume_frame)
        {
            if (r_s8(state) == 2)
            {
                sub_800118A0(16, rrj_read32(0x80010E34), call);
                sub_8001B868();
                leaf_call(rrj_host_context(), call, 0x8007FF4C, 0, 0, 0, 0);
            }
            sub_800118A0(2, rrj_read32(0x80010E34), call);
            leaf_call(rrj_host_context(), call, 0x80064610, 0, 0, 0, 0);
        }
        do
        {
            if (!resume_frame)
            {
                leaf_call(rrj_host_context(), call, 0x80063B90, 0, 0, 0, 0);
                sub_800118A0(4, rrj_read32(0x80010E34), call);
                sub_8001F080(call);
                state = rrj_read32(0x8005B2F8);
                if (rrj_read32(state + 0x30) == 1)
                {
                    sub_800247A0(call);
                    sub_80020E30(0);
                }
                sub_800119C0(call);
                sub_8001B868();
            }
            resume_frame = 0;
            if (!first_inner)
                state = rrj_read32(0x8005B2F8);
            for (;;)
            {
                if (!first_inner)
                {
                    mode = r_s8(state);
                    if (!mode || mode == 2 || r_s8(state) == 5)
                        break;
                }
                if (!first_inner && rrj_trace_phase_boundary(rrj_host_context(), 0x80012370))
                    return 0;
                first_inner = 0;
                if (rrj_trace_phase_boundary(rrj_host_context(), 0x8001CB3C))
                    return 0;
                leaf_call(rrj_host_context(), call, 0x8001CB3C, 0, 0, 0, 0);
                state = rrj_read32(0x8005B2F8);
                if (r_s8(state + 3))
                {
                    rrj_write32(0x8005B228, 0x00050000);
                    rrj_put16(rrj_at(0x800CD542, 2), 0);
                    rrj_put16(rrj_at(0x800CD540, 2), 0);
                    leaf_call(rrj_host_context(), call, 0x8008AB00, 1094, 0, 0, 0);
                    leaf_call(rrj_host_context(), call, 0x800881B4, 0x800CD898, 1094, 0, 0);
                    state = rrj_read32(0x8005B2F8);
                    if (rrj_read32(state + 0x30) >= 2)
                        leaf_call(rrj_host_context(), call, 0x800881B4, 0x800CDD04, 1094, 0, 0);
                    leaf_call(rrj_host_context(), call, 0x8008CFDC, 0x800CDD04, 0, 0, 0);
                    leaf_call(rrj_host_context(), call, 0x8005E1D8, 0x800CE170, 0, 0, 0);
                    state = rrj_read32(0x8005B2F8);
                    w_u8(state + 3, 0);
                }
                else
                {
                    if (rrj_trace_phase_boundary(rrj_host_context(), 0x8001C428))
                        return 0;
                    leaf_call(rrj_host_context(), call, 0x8001C428, 0, 0, 0, 0);
                }
                leaf_call(rrj_host_context(), call, 0x80011C4C, 0, 0, 0, 0);
                if (rrj_trace_phase_boundary(rrj_host_context(), 0x8001241C))
                    return 0;
                state = rrj_read32(0x8005B2F8);
                if (r_s8(state + 2))
                {
                    w_u8(state, 3);
                    state = rrj_read32(0x8005B2F8);
                    w_u8(state + 2, 0);
                    sub_80018C1C(1);
                    sub_80020E30(1);
                    state = rrj_read32(0x8005B2F8);
                }
            }
            leaf_call(rrj_host_context(), call, 0x8001C408, 0, 0, 0, 0);
            sub_80022FC0(call);
            sub_800118A0(2, rrj_read32(0x80010E34), call);
            leaf_call(rrj_host_context(), call, 0x80063A20, 0, 0, 0, 0);
            state = rrj_read32(0x8005B2F8);
            if (r_s8(state) != 5)
                break;
            leaf_call(rrj_host_context(), call, 0x80063FA0, 0, 0, 0, 0);
        } while (r_s8(rrj_read32(0x8005B2F8)) == 5);
    }
    sub_800121E4(call);
    return 0;
}

uint32_t sub_80012764(uint32_t index)
{
    uint32_t object = rrj_read32(0x8005B268 + index * 4);
    uint32_t flags = rrj_read32(object + 0x230);
    uint32_t enabled = rrj_read32(0x8005AD40);
    uint32_t slot;
    uint32_t result;
    FUNCTION_MARKER(0x80012764, "SLUS_010.53");
    if (flags & 0x08000000)
    {
        slot = r_u8(rrj_read32(object + 0x43C) + 1) & 15;
        result = 1u << slot;
        if (!(enabled & result))
            return result;
        result = slot * 8;
        if (rrj_read32(object + 0x358) && rrj_read32(object + 0x440))
            return result;
    }
    else
    {
        uint32_t two_player = index == 0 && rrj_read32(rrj_read32(0x8005B2F8) + 48) == 2;
        slot = (enabled >> 24) - (two_player + 1);
        result = slot * 8;
        if (!(enabled & (1u << (slot & 31))))
            return result;
    }
    result = (slot * 7) << 6;
    rrj_write32(object + 0x22C, rrj_read32(0x8005B248) + result);
    return result;
}

uint32_t sub_80014044(uint32_t record, uint32_t tim, RRJRaceLeafCall call)
{
    uint32_t result;
    FUNCTION_MARKER(0x80014044, "SLUS_010.53");
    if (!leaf_call(rrj_host_context(), call, 0x8004B3C4, tim, 0, 0, 0))
        return leaf_call(rrj_host_context(), call, 0x8004B3D4, record, 0, 0, 0);
    result = leaf_call(rrj_host_context(), call, 0x80044894, 0x800107DC, 0, 0, 0);
    rrj_write32(record + 16, 0);
    return result;
}

uint32_t sub_8001408C(uint32_t path, uint32_t buffer_out, uint32_t record, RRJRaceLeafCall call)
{
    uint32_t result_out = 0x1F8003D8;
    FUNCTION_MARKER(0x8001408C, "SLUS_010.53");
    rrj_write32(result_out, 0);
    if (rrj_s32(sub_80014654(path, rrj_read32(0x8005AD5C), buffer_out, result_out, call)) < 0)
        return 1;
    sub_80014044(record, rrj_read32(buffer_out), call);
    return 0;
}

uint32_t sub_800140E8(void)
{
    FUNCTION_MARKER(0x800140E8, "SLUS_010.53");
    rrj_write32(0x8005253C, 0x8005AC8C);
    return 0;
}

uint32_t sub_800142B4(uint32_t size, uint32_t index, RRJRaceLeafCall call)
{
    uint32_t link = 0x800D6500 + index * 16;
    uint32_t block = rrj_read32(link);
    uint32_t requested = (size + 11) & 0xFFFFFFF8;
    FUNCTION_MARKER(0x800142B4, "SLUS_010.53");
    while (block)
    {
        uint32_t block_size = rrj_read32(block + 4);
        if (block_size == requested)
        {
            rrj_write32(link, rrj_read32(block));
            rrj_write32(block, requested);
            return block + 4;
        }
        if (requested < block_size)
        {
            uint32_t remainder = block + requested;
            rrj_write32(link, remainder);
            rrj_write32(remainder, rrj_read32(block));
            block_size = rrj_read32(block + 4);
            rrj_write32(remainder + 4, block_size - requested);
            rrj_write32(block, requested);
            return block + 4;
        }
        link = block;
        block = rrj_read32(block);
    }
    leaf_call(rrj_host_context(), call, 0x80044894, 0x800107F0, requested, requested, 0);
    return 0;
}

uint32_t sub_8001426C(uint32_t arena, uint32_t size, uint32_t index)
{
    uint32_t slot = 0x800D6500 + index * 16;
    uint32_t end = arena + (size & 0xFFFFFFF8);
    uint32_t available;
    FUNCTION_MARKER(0x8001426C, "SLUS_010.53");
    rrj_write32(slot + 12, arena);
    rrj_write32(slot + 8, end);
    rrj_write32(slot, arena);
    rrj_write32(arena, 0);
    end = rrj_read32(slot + 8);
    available = end - rrj_read32(slot + 12);
    rrj_write32(arena + 4, available);
    return available;
}

void sub_80014370(uint32_t pointer)
{
    uint32_t block;
    uint32_t size;
    uint32_t next;
    uint32_t previous = 0;
    uint32_t merged;
    FUNCTION_MARKER(0x80014370, "SLUS_010.53");
    if (!pointer)
        return;
    block = pointer - 4;
    size = rrj_read32(block);
    rrj_write32(0x8005B3A8, rrj_read32(0x8005B3A8) + size);
    next = rrj_read32(0x800D6500);
    while (next && next < block)
    {
        previous = next;
        next = rrj_read32(previous);
    }
    if (previous && previous + rrj_read32(previous + 4) == block)
    {
        rrj_write32(previous + 4, rrj_read32(previous + 4) + size);
        merged = previous;
    }
    else
    {
        rrj_write32(block + 4, size);
        rrj_write32(block, next);
        if (previous)
            rrj_write32(previous, block);
        else
            rrj_write32(0x800D6500, block);
        merged = block;
    }
    if (next && merged + rrj_read32(merged + 4) == next)
    {
        rrj_write32(merged + 4, rrj_read32(merged + 4) + rrj_read32(next + 4));
        rrj_write32(merged, rrj_read32(next));
    }
}

uint32_t sub_8001444C(void)
{
    FUNCTION_MARKER(0x8001444C, "SLUS_010.53");
    return sub_8001426C(rrj_read32(0x800548D4), rrj_read32(0x800548D8), 0);
}

uint32_t sub_8001447C(uint32_t size, uint32_t pool, RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x8001447C, "SLUS_010.53");
    if (size && pool < 2)
        return sub_800142B4(size, 0, call);
    return 0;
}

uint32_t sub_800144B8(uint32_t pointer)
{
    FUNCTION_MARKER(0x800144B8, "SLUS_010.53");
    if (!pointer)
        return 0xFFFFFFFF;
    sub_80014370(pointer);
    return 0;
}

uint32_t sub_8001458C(uint32_t path, uint32_t device, RRJRaceLeafCall call)
{
    uint32_t device_table;
    FUNCTION_MARKER(0x8001458C, "SLUS_010.53");
    device_table = rrj_read32(0x8005B3B0);
    if (rrj_s32(device) < 0 || rrj_s32(device) >= rrj_s32(rrj_read32(device_table + 32)))
        return 0xFFFFFFF8;
    if (device == rrj_read32(0x8005AD5C))
        return sub_80014978(path, call);
    return sub_80014A30(path, call);
}

uint32_t sub_8001460C(uint32_t index, RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x8001460C, "SLUS_010.53");
    sub_80014AE8(index, call);
    return 0;
}

uint32_t sub_8001462C(void)
{
    FUNCTION_MARKER(0x8001462C, "SLUS_010.53");
    return 0;
}

uint32_t sub_80014634(uint32_t index, uint32_t offset, uint32_t mode)
{
    FUNCTION_MARKER(0x80014634, "SLUS_010.53");
    return sub_80014E5C(index, offset, mode);
}

uint32_t sub_80014654(uint32_t path, uint32_t unused, uint32_t buffer_out, uint32_t result_out, RRJRaceLeafCall call)
{
    uint32_t result;
    FUNCTION_MARKER(0x80014654, "SLUS_010.53");
    result = sub_80014B40(path, buffer_out, call);
    rrj_write32(result_out, result);
    return result;
}

uint32_t sub_80014680(uint32_t path, uint32_t device, uint32_t buffer, uint32_t result_out, RRJRaceLeafCall call)
{
    uint32_t index;
    uint32_t result = 0xFFFFFFFF;
    FUNCTION_MARKER(0x80014680, "SLUS_010.53");
    if (rrj_read32(0x8005B200))
        leaf_call(rrj_host_context(), call, 0x80044894, 0x80010820, path, device, buffer);
    index = sub_80014978(path, call);
    if (rrj_s32(index) < 0)
    {
        if (rrj_read32(0x8005B200))
            leaf_call(rrj_host_context(), call, 0x80044894, 0x80010894, 0, 0, 0);
    }
    else
    {
        result = sub_80014DE0(index, call);
        if (rrj_s32(result) < 0)
        {
            if (rrj_read32(0x8005B200))
                leaf_call(rrj_host_context(), call, 0x80044894, 0x80010868, result, 0, 0);
        }
        else
            result = sub_80014BDC(index, result, buffer, call);
        sub_80014AE8(index, call);
    }
    rrj_write32(result_out, result);
    return result;
}

uint32_t sub_80014938(RRJRaceLeafCall call)
{
    uint32_t address = 0x800D66FC;
    int32_t remaining = 15;
    FUNCTION_MARKER(0x80014938, "SLUS_010.53");
    do
    {
        rrj_write32(address, 0);
        address -= 20;
        --remaining;
    } while (remaining >= 0);
    return sub_80015114(call);
}

uint32_t sub_80014528(uint32_t table, RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x80014528, "SLUS_010.53");
    rrj_write32(0x8005B3B0, table);
    sub_80014938(call);
    return 0;
}

uint32_t sub_80014780(uint32_t index, uint32_t buffer, uint32_t count, RRJRaceLeafCall call)
{
    uint32_t copied = 0;
    uint32_t position;
    uint32_t prefix;
    FUNCTION_MARKER(0x80014780, "SLUS_010.53");
    position = sub_80014F98(index);
    prefix = position & 0x7FF;
    if (prefix)
    {
        uint32_t temporary;
        uint32_t fetched;
        copied = 2048 - prefix;
        if (rrj_s32(count) < rrj_s32(copied))
            copied = count;
        temporary = sub_8001447C(2048, 0, call);
        sub_80014E5C(index, position & 0xFFFFF800, 0);
        fetched = prefix + count < 2049 ? prefix + count : 2048;
        fetched = sub_80014BDC(index, fetched, temporary, call);
        if (rrj_s32(fetched) >= 0)
            sub_8001E08C(buffer, temporary + prefix, copied);
        sub_800144B8(temporary);
        if (rrj_s32(copied) >= rrj_s32(count) || rrj_s32(fetched) < rrj_s32(copied))
            return copied;
    }
    return copied + sub_80014BDC(index, count - copied, buffer + copied, call);
}

uint32_t sub_80014894(uint32_t index, uint32_t buffer, uint32_t count, uint32_t callback, RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x80014894, "SLUS_010.53");
    return sub_80014D18(index, count, buffer, callback, call);
}

uint32_t sub_800148BC(uint32_t index, RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x800148BC, "SLUS_010.53");
    return sub_80014DE0(index, call);
}

static uint32_t leaf_open_path(RRJMemory *m, uint32_t path, RRJRaceLeafCall call)
{
    uint32_t index;
    uint32_t record;
    uint32_t result;
    for (index = 0; index < 16; ++index)
    {
        record = 0x800D65D0 + index * 20;
        if (!rrj_read32(record))
            break;
    }
    if (index == 16)
        return 0xFFFFFFFF;
    result = sub_80015134(path, record + 4, record + 12, call);
    if (result == 0xFFFFFFFF)
        return result;
    rrj_write32(record + 8, 0);
    rrj_write32(record + 16, result);
    rrj_write32(record, rrj_read32(record) | 1);
    return index;
}

uint32_t sub_80014978(uint32_t path, RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x80014978, "SLUS_010.53");
    return leaf_open_path(rrj_host_context(), path, call);
}

uint32_t sub_80014A30(uint32_t path, RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x80014A30, "SLUS_010.53");
    return leaf_open_path(rrj_host_context(), path, call);
}

uint32_t sub_80014AE8(uint32_t index, RRJRaceLeafCall call)
{
    uint32_t result = index < 16;
    FUNCTION_MARKER(0x80014AE8, "SLUS_010.53");
    if (result)
    {
        uint32_t record = 0x800D65D0 + index * 20;
        result = rrj_read32(record) & 1;
        if (result)
        {
            uint32_t handle = rrj_read32(record + 16);
            rrj_write32(record, 0);
            return sub_800151BC(handle, call);
        }
    }
    return result;
}

uint32_t sub_80014B40(uint32_t path, uint32_t output, RRJRaceLeafCall call)
{
    uint32_t index;
    uint32_t result;
    FUNCTION_MARKER(0x80014B40, "SLUS_010.53");
    index = sub_80014978(path, call);
    if (index == 0xFFFFFFFF)
    {
        rrj_write32(output, 0);
        result = 0xFFFFFFFF;
    }
    else
    {
        uint32_t record = 0x800D65D0 + index * 20;
        uint32_t buffer = sub_8001447C(rrj_read32(record + 12), 0, call);
        rrj_write32(output, buffer);
        result = buffer ? sub_80014BDC(index, rrj_read32(record + 12), buffer, call) : 0xFFFFFFFF;
    }
    sub_80014AE8(index, call);
    return result;
}

uint32_t sub_80014BDC(uint32_t index, uint32_t count, uint32_t buffer, RRJRaceLeafCall call)
{
    uint32_t record;
    uint32_t current;
    uint32_t end;
    uint32_t result;
    FUNCTION_MARKER(0x80014BDC, "SLUS_010.53");
    if (index >= 16)
        return 0xFFFFFFFF;
    record = 0x800D65D0 + index * 20;
    if (!rrj_read32(record))
    {
        if (rrj_read32(0x8005B200))
            leaf_call(rrj_host_context(), call, 0x80044894, 0x800108BC, index, 0, 0);
        return 0xFFFFFFFF;
    }
    current = rrj_read32(record + 8);
    end = rrj_read32(record + 12);
    if (end < current + count)
    {
        if (rrj_read32(0x8005B200))
            leaf_call(rrj_host_context(), call, 0x80044894, 0x800108E4, count, end - current, 0);
        count = end - current;
    }
    result = sub_800151FC(rrj_read32(record + 16), rrj_read32(record + 4), current, buffer, count, call);
    rrj_write32(record + 8, current + count);
    sub_8001508C(index);
    return result;
}

uint32_t sub_80014D18(uint32_t index, uint32_t count, uint32_t buffer, uint32_t callback, RRJRaceLeafCall call)
{
    uint32_t record = 0x800D65D0 + index * 20;
    uint32_t current;
    uint32_t end;
    FUNCTION_MARKER(0x80014D18, "SLUS_010.53");
    if (!rrj_read32(record))
        return 0xFFFFFFFF;
    current = rrj_read32(record + 8);
    end = rrj_read32(record + 12);
    if (end < current + count)
        count = end - current;
    rrj_write32(0x8005B3B8, callback);
    if (!sub_80015314(rrj_read32(record + 16), rrj_read32(record + 4), current, buffer, count, 0x800150EC, call))
        return 0xFFFFFFFF;
    rrj_write32(record + 8, current + count);
    sub_8001508C(index);
    return count;
}

uint32_t sub_80014DE0(uint32_t index, RRJRaceLeafCall call)
{
    uint32_t record = 0x800D65D0 + index * 20;
    FUNCTION_MARKER(0x80014DE0, "SLUS_010.53");
    if (rrj_read32(record) & 1)
        return rrj_read32(record + 12);
    if (rrj_read32(0x8005B200))
        leaf_call(rrj_host_context(), call, 0x80044894, 0x8001090C, index, 0, 0);
    return 0xFFFFFFFF;
}

uint32_t sub_80014E5C(uint32_t index, uint32_t offset, uint32_t mode)
{
    uint32_t record = 0x800D65D0 + index * 20;
    uint32_t value;
    uint32_t sign;
    uint32_t difference;
    FUNCTION_MARKER(0x80014E5C, "SLUS_010.53");
    if (!(rrj_read32(record) & 1))
        return 0xFFFFFFFF;
    if (mode == 1)
        value = rrj_read32(record + 8) + offset;
    else if (rrj_s32(mode) >= 2 && mode == 2)
        value = rrj_read32(record + 12) + offset;
    else
        value = offset;
    sign = (uint32_t)(rrj_s32(value) >> 31);
    value &= ~sign;
    difference = rrj_read32(record + 12) - value;
    value += (uint32_t)(rrj_s32(difference) >> 31) & difference;
    rrj_write32(record + 8, value);
    sub_8001508C(index);
    return value;
}

uint32_t sub_80014F98(uint32_t index)
{
    uint32_t record = 0x800D65D0 + index * 20;
    FUNCTION_MARKER(0x80014F98, "SLUS_010.53");
    return (rrj_read32(record) & 1) ? rrj_read32(record + 8) : 0xFFFFFFFF;
}

uint32_t sub_8001508C(uint32_t index)
{
    uint32_t record = 0x800D65D0 + index * 20;
    uint32_t flags = rrj_read32(record);
    uint32_t result = flags & 1;
    FUNCTION_MARKER(0x8001508C, "SLUS_010.53");
    if (result)
    {
        result = rrj_s32(rrj_read32(record + 8)) < rrj_s32(rrj_read32(record + 12)) ? flags & ~2u : flags | 2;
        rrj_write32(record, result);
    }
    return result;
}

uint32_t sub_80015134(uint32_t path, uint32_t first_out, uint32_t second_out, RRJRaceLeafCall call)
{
    uint32_t expanded = 0x1F800200;
    FUNCTION_MARKER(0x80015134, "SLUS_010.53");
    sub_8001542C(expanded, 0x8005AD60, path, call);
    return sub_80015E74(expanded, first_out, second_out, call) ? 0 : 0xFFFFFFFF;
}

uint32_t sub_800151BC(uint32_t value, RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x800151BC, "SLUS_010.53");
    if ((value >> 16) == 1)
        return sub_8004DE44(value & 0xFFFF, call);
    return 1;
}

uint32_t sub_800151FC(uint32_t packed_handle, uint32_t base, uint32_t offset, uint32_t buffer, uint32_t size, RRJRaceLeafCall call)
{
    uint32_t handle = packed_handle & 0xFFFF;
    FUNCTION_MARKER(0x800151FC, "SLUS_010.53");
    if ((packed_handle >> 16) == 1)
    {
        leaf_call(rrj_host_context(), call, RRJ_RACE_BREAK_SEEK, handle, handle, offset, 0);
        return sub_8004DE78(handle, buffer, size, call);
    }
    rrj_write32(0x8005B3C0, 0);
    if (rrj_read32(0x8005B200))
    {
        const uint32_t args[8] = {0x80010968, packed_handle, base, offset, buffer, size, 0, 0};
        leaf_call8(rrj_host_context(), call, 0x80044894, args);
    }
    sub_80015314(packed_handle, base, offset, buffer, size, 0x800151EC, call);
    while (!rrj_read32(0x8005B3C0))
        ;
    if (rrj_read32(0x8005B200))
        leaf_call(rrj_host_context(), call, 0x80044894, 0x800109BC, 0, 0, 0);
    return size;
}

uint32_t sub_80015314(uint32_t packed_handle, uint32_t base, uint32_t offset, uint32_t buffer, uint32_t size, uint32_t callback, RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x80015314, "SLUS_010.53");
    if ((packed_handle >> 16) == 1)
    {
        leaf_call(rrj_host_context(), call, RRJ_RACE_BREAK_SEEK, packed_handle, packed_handle, offset, 0);
        sub_8004DE78(packed_handle, buffer, size, call);
        if (callback)
            leaf_call(rrj_host_context(), call, callback, 0, 0, 0, 0);
    }
    else
    {
        int32_t blocks = rrj_s32(offset);
        unsigned pending = 0;

        if (callback == 0x800151ECu)
            while (rrj_read32(0x800D6728u) && pending++ < 64u)
            {
                uint32_t active_callback = rrj_trace_vblank_boundary(rrj_host_context());

                if (active_callback)
                    leaf_call(rrj_host_context(), call, active_callback, 0, 0, 0, 0);
            }
        if (rrj_read32(0x800D6728u))
            abort();
        if (blocks < 0)
            blocks += 2047;
        blocks >>= 11;
        sub_800155FC(base + (uint32_t)blocks);
        sub_8001562C(callback);
        if (rrj_read32(0x8005B200))
            leaf_call(rrj_host_context(), call, 0x80044894, 0x800109E4, size, buffer, 0);
        sub_8001564C(buffer, size, call);
        if (callback == 0x800151ECu)
        {
            rrj_write32(0x800D6728u, 0);
            leaf_call(rrj_host_context(), call, callback, 0, 0, 0, 0);
        }
    }
    return 1;
}

uint32_t sub_8001542C(uint32_t context, uint32_t unused, uint32_t path, RRJRaceLeafCall call)
{
    uint32_t result;
    FUNCTION_MARKER(0x8001542C, "SLUS_010.53");
    if (r_u8(path) == '\\' || r_u8(path + 1) == ':')
        return leaf_call(rrj_host_context(), call, 0x800448E4, context, path, 0, 0);
    leaf_call(rrj_host_context(), call, 0x800448E4, context, 0, 0, 0);
    result = leaf_call(rrj_host_context(), call, 0x80044864, context, path, 0, 0);
    return result;
}

uint32_t sub_80015C08(uint32_t bcd)
{
    uint32_t first;
    uint32_t second;
    uint32_t third;
    FUNCTION_MARKER(0x80015C08, "SLUS_010.53");
    first = r_u8(bcd);
    second = r_u8(bcd + 1);
    third = r_u8(bcd + 2);
    first = 10 * (first >> 4) + (first & 15);
    second = 10 * (second >> 4) + (second & 15);
    third = 10 * (third >> 4) + (third & 15);
    return 75 * (60 * first + second) + third;
}

uint32_t sub_80015C88(uint32_t output, uint32_t frames)
{
    int32_t whole_seconds = rrj_s32(frames) / 75;
    uint8_t frame = (uint8_t)(rrj_s32(frames) - whole_seconds * 75);
    int32_t minutes = whole_seconds / 60;
    uint8_t second = (uint8_t)(whole_seconds - minutes * 60);
    uint8_t minute;
    FUNCTION_MARKER(0x80015C88, "SLUS_010.53");
    w_u8(output + 2, frame);
    w_u8(output + 1, second);
    w_u8(output + 3, 0);
    w_u8(output, (uint8_t)minutes);
    minute = (uint8_t)r_u8(output);
    w_u8(output + 2, (uint8_t)(16 * (frame / 10) + frame % 10));
    w_u8(output + 1, (uint8_t)(16 * (second / 10) + second % 10));
    w_u8(output, (uint8_t)(16 * (minute / 10) + minute % 10));
    return 10 * (minute / 10);
}

uint32_t sub_800155FC(uint32_t value)
{
    uint32_t result;
    FUNCTION_MARKER(0x800155FC, "SLUS_010.53");
    while (rrj_read32(0x800D6728))
        ;
    result = value + rrj_read32(0x8005B3D8);
    rrj_write32(0x800D6720, result);
    return result;
}

uint32_t sub_8001562C(uint32_t value)
{
    FUNCTION_MARKER(0x8001562C, "SLUS_010.53");
    while (rrj_read32(0x800D6728))
        ;
    rrj_write32(0x800D6710, value);
    return 0x800D0000;
}

uint32_t sub_8001564C(uint32_t first, uint32_t length, RRJRaceLeafCall call)
{
    uint32_t adjusted = length + 2047;
    uint32_t bcd = 0x1F8003F0;
    FUNCTION_MARKER(0x8001564C, "SLUS_010.53");
    if (rrj_s32(adjusted) < 0)
        adjusted = length + 4094;
    while (rrj_read32(0x800D6728))
        ;
    rrj_write32(0x800D6714, first);
    rrj_write32(0x800D6718, (uint32_t)(rrj_s32(adjusted) >> 11));
    rrj_write32(0x800D671C, length);
    if (rrj_read32(0x800D6724) == rrj_read32(0x800D6720))
    {
        rrj_write32(0x800D6728, 5);
        return 5;
    }
    sub_80015C88(bcd, rrj_read32(0x800D6720));
    rrj_write32(0x800D6728, 1);
    rrj_write32(0x800D6724, 0xFFFFFFFF);
    leaf_call(rrj_host_context(), call, 0x800457E8, 0x8001578C, 0, 0, 0);
    leaf_call(rrj_host_context(), call, 0x800457A8, 0, 0, 0, 0);
    return leaf_call(rrj_host_context(), call, 0x8004594C, 2, bcd, 0, 0);
}

uint32_t sub_80015734(uint32_t first, uint32_t length, RRJRaceLeafCall call)
{
    uint32_t result;
    FUNCTION_MARKER(0x80015734, "SLUS_010.53");
    rrj_write32(0x8005B3CC, 0);
    sub_8001562C(0x80015724);
    sub_8001564C(first, length, call);
    do
    {
        result = rrj_read32(0x8005B3CC);
    } while (!result);
    return result;
}

uint32_t sub_80015D94(RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x80015D94, "SLUS_010.53");
    leaf_call(rrj_host_context(), call, 0x80045504, 0, 0, 0, 0);
    leaf_call(rrj_host_context(), call, 0x80045A80, 14, 0x8005B3C8, 0, 0);
    leaf_call(rrj_host_context(), call, 0x800457FC, 0x80015860, 0, 0, 0);
    return leaf_call(rrj_host_context(), call, 0x800457E8, 0, 0, 0, 0);
}

uint32_t sub_80015F2C(uint32_t capacity, RRJRaceLeafCall call)
{
    uint32_t result = rrj_read32(0x8005AD6C);
    FUNCTION_MARKER(0x80015F2C, "SLUS_010.53");
    if (!result)
    {
        rrj_write32(0x8005AD74, capacity);
        rrj_write32(0x8005AD70, 0);
        result = sub_8001447C(capacity * 12, 0, call);
        rrj_write32(0x8005AD6C, result);
        if (result)
            return sub_800160E0(rrj_read32(0x8005B3D0), rrj_read32(0x8005B3D4), 1, 0x8005AD78, call);
    }
    return result;
}

uint32_t sub_80015DD8(uint32_t capacity, RRJRaceLeafCall call)
{
    uint32_t output = 0x1F8003E0;
    uint32_t bcd = 0x1F8003E8;
    uint32_t buffer;
    uint32_t first;
    uint32_t second;
    FUNCTION_MARKER(0x80015DD8, "SLUS_010.53");
    leaf_call(rrj_host_context(), call, 0x800452B4, output, 0, 0, 0);
    leaf_call(rrj_host_context(), call, 0x800457A8, 0, 0, 0, 0);
    first = sub_80015C08(output + 4);
    rrj_write32(0x8005B3D8, first);
    w_u8(bcd, r_u8(output + 4));
    w_u8(bcd + 1, r_u8(output + 5));
    w_u8(bcd + 2, 22);
    w_u8(bcd + 3, 0);
    second = sub_80015C08(bcd);
    buffer = sub_8001447C(2048, 0, call);
    if (!buffer)
        return 0;
    sub_800160B0(second - first, buffer, call);
    rrj_write32(0x8005B3D0, leaf_read_le32(rrj_host_context(), buffer + 0x9E));
    rrj_write32(0x8005B3D4, leaf_read_le32(rrj_host_context(), buffer + 0xA6));
    sub_800144B8(buffer);
    return sub_80015F2C(capacity, call);
}

uint32_t sub_8001549C(uint32_t capacity, RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x8001549C, "SLUS_010.53");
    w_u8(0x8005B3C8, 0xA0);
    sub_80015D94(call);
    rrj_write32(0x800D6710, 0);
    rrj_write32(0x800D6718, 0);
    rrj_write32(0x800D671C, 0);
    rrj_write32(0x800D6714, 0);
    rrj_write32(0x800D6720, 0);
    rrj_write32(0x800D6728, 0);
    rrj_write32(0x800D6724, 0xFFFFFFFF);
    rrj_write32(0x800D672C, 0);
    return sub_80015DD8(capacity, call);
}

uint32_t sub_80015114(RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x80015114, "SLUS_010.53");
    return sub_8001549C(500, call);
}

uint32_t sub_80015E74(uint32_t path, uint32_t first_out, uint32_t second_out, RRJRaceLeafCall call)
{
    uint32_t table = rrj_read32(0x8005AD6C);
    uint32_t count = rrj_read32(0x8005AD70);
    uint32_t index;
    uint32_t hash;
    FUNCTION_MARKER(0x80015E74, "SLUS_010.53");
    if (table && rrj_s32(count) > 0)
    {
        hash = sub_80015F8C(path, call);
        for (index = 0; rrj_s32(index) < rrj_s32(count); ++index)
        {
            uint32_t record = table + index * 12;
            if (rrj_read32(record + 8) == hash)
            {
                rrj_write32(first_out, rrj_read32(record));
                rrj_write32(second_out, rrj_read32(record + 4));
                return 1;
            }
        }
    }
    rrj_write32(second_out, 0);
    rrj_write32(first_out, 0);
    return 0;
}

uint32_t sub_80015F8C(uint32_t path, RRJRaceLeafCall call)
{
    uint32_t cursor = path;
    uint32_t start = path;
    uint32_t hash = 0;
    uint32_t shift = 0;
    FUNCTION_MARKER(0x80015F8C, "SLUS_010.53");
    while (r_u8(cursor))
    {
        if (r_u8(cursor) == '\\')
            start = cursor;
        ++cursor;
    }
    if (r_u8(start) == '\\')
    {
        do
        {
            ++start;
        } while (r_u8(start) == '\\');
    }
    while (r_u8(start))
    {
        uint32_t value = leaf_call(rrj_host_context(), call, 0x80044884, r_u8(start++), 0, 0, 0);
        hash = ((hash ^ (value << shift)) + value * value) ^ (value << ((24 - shift) & 31));
        shift = (shift + 1) % 24;
    }
    return hash;
}

uint32_t sub_800160B0(uint32_t position, uint32_t buffer, RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x800160B0, "SLUS_010.53");
    sub_800155FC(position);
    return sub_80015734(buffer, 2048, call);
}

static uint32_t leaf_read_le32(RRJMemory *m, uint32_t address)
{
    return r_u32(address);
}

static uint32_t leaf_path_hash(RRJMemory *m, const char *path, RRJRaceLeafCall call)
{
    const char *start = path;
    const char *cursor = path;
    uint32_t hash = 0;
    uint32_t shift = 0;
    while (*cursor)
    {
        if (*cursor == '\\')
            start = cursor;
        ++cursor;
    }
    while (*start == '\\')
        ++start;
    while (*start)
    {
        uint32_t value = leaf_call(m, call, 0x80044884, (uint8_t)*start++, 0, 0, 0);
        hash = ((hash ^ (value << shift)) + value * value) ^ (value << ((24 - shift) & 31));
        shift = (shift + 1) % 24;
    }
    return hash;
}

static uint32_t leaf_walk_cd_directory(RRJMemory *m, uint32_t position, uint32_t length, uint32_t depth, const char *path, RRJRaceLeafCall call)
{
    uint32_t buffer = sub_8001447C(2048, 0, call);
    uint32_t next_position = position + 1;
    uint32_t offset;
    if (!buffer)
        return 0;
    sub_800160B0(position, buffer, call);
    offset = r_u8(buffer);
    offset += r_u8(buffer + offset);
    for (;;)
    {
        while (offset < 2048)
        {
            uint32_t record = buffer + offset;
            uint32_t record_size = r_u8(record);
            uint32_t extent;
            uint32_t file_size;
            uint32_t name_length;
            uint32_t copy_length;
            uint32_t index;
            char name[16];
            char full_path[256];
            if (!record_size || (r_u8(record + 25) & 0x80))
                break;
            extent = leaf_read_le32(m, record + 2);
            if (extent)
            {
                file_size = leaf_read_le32(m, record + 10);
                name_length = r_u8(record + 32);
                for (index = 0; index < name_length; ++index)
                {
                    if (r_u8(record + 33 + index) == ';')
                    {
                        name_length = index;
                        break;
                    }
                }
                copy_length = name_length < sizeof(name) - 1 ? name_length : sizeof(name) - 1;
                for (index = 0; index < copy_length; ++index)
                    name[index] = (char)leaf_call(m, call, 0x80044884, r_u8(record + 33 + index), 0, 0, 0);
                name[copy_length] = 0;
                copy_length = 0;
                while (path[copy_length] && copy_length < sizeof(full_path) - 1)
                {
                    full_path[copy_length] = path[copy_length];
                    ++copy_length;
                }
                if (copy_length < sizeof(full_path) - 1)
                    full_path[copy_length++] = '\\';
                for (index = 0; name[index] && copy_length < sizeof(full_path) - 1; ++index)
                    full_path[copy_length++] = name[index];
                full_path[copy_length] = 0;
                if (r_u8(record + 25) & 2)
                {
                    leaf_walk_cd_directory(m, extent, file_size, depth + 1, full_path, call);
                }
                else if (rrj_s32(rrj_read32(0x8005AD70)) >= rrj_s32(rrj_read32(0x8005AD74)))
                {
                    leaf_call(m, call, 0x80044894, 0x80010A74, 0x80060000, 0, 0);
                }
                else
                {
                    uint32_t count = rrj_read32(0x8005AD70);
                    uint32_t table = rrj_read32(0x8005AD6C);
                    uint32_t hash = leaf_path_hash(m, full_path, call);
                    uint32_t entry;
                    for (index = 0; rrj_s32(index) < rrj_s32(count); ++index)
                    {
                        if (rrj_read32(table + index * 12 + 8) == hash)
                            leaf_call(m, call, 0x80044894, 0x80010A2C, 0, 0, 0);
                    }
                    entry = table + count * 12;
                    rrj_write32(entry, extent);
                    rrj_write32(entry + 4, file_size);
                    rrj_write32(entry + 8, hash);
                    rrj_write32(0x8005AD70, count + 1);
                }
            }
            offset += record_size;
        }
        length -= 2048;
        if (!length)
            break;
        sub_800160B0(next_position++, buffer, call);
        offset = 0;
        if (rrj_s32(length) <= 0)
            break;
    }
    return sub_800144B8(buffer);
}

uint32_t sub_800160E0(uint32_t position, uint32_t length, uint32_t depth, uint32_t path, RRJRaceLeafCall call)
{
    char root[256];
    uint32_t index = 0;
    FUNCTION_MARKER(0x800160E0, "SLUS_010.53");
    while (index < sizeof(root) - 1 && r_u8(path + index))
    {
        root[index] = *(char *)rrj_at(path + index, 1);
        ++index;
    }
    root[index] = 0;
    return leaf_walk_cd_directory(rrj_host_context(), position, length, depth, root, call);
}

uint32_t sub_800163D4(RRJRaceLeafCall call)
{
    uint32_t result;
    FUNCTION_MARKER(0x800163D4, "SLUS_010.53");
    sub_80019D9C();
    sub_8001EBF0(call);
    result = sub_8001EC30(1, 0x01002001, 4, 12, 5, 24, 0, 0, call);
    sub_8001E138(call);
    if (!result)
        w_u8(0x8005B3F4, 1);
    return 1;
}

uint32_t sub_8001643C(RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x8001643C, "SLUS_010.53");
    sub_8001E188(call);
    return sub_8001ECEC(call);
}

uint32_t sub_800164B4(RRJRaceLeafCall call)
{
    uint32_t result = 64;
    FUNCTION_MARKER(0x800164B4, "SLUS_010.53");
    if (rrj_read32(rrj_read32(0x8005B2F8) + 48) == 2)
    {
        uint32_t value = rrj_read32(0x800D6C0C);
        uint32_t extra = 0x1F800340;
        const uint32_t args[8] = {rrj_read32(0x8005B404), 110, 0, 1, extra, 64, 0, 0};
        rrj_write32(extra, 0xFFFFFFFF);
        rrj_write32(extra + 4, (uint32_t)(rrj_s32(value * 127) >> 7));
        result = leaf_call8(rrj_host_context(), call, 0x8001F174, args);
        rrj_write32(0x8005B438, result);
    }
    return result;
}

uint32_t sub_8001654C(void)
{
    uint32_t state = rrj_read32(0x8005B2F8);
    uint32_t players = rrj_read32(state + 48);
    uint32_t index;
    FUNCTION_MARKER(0x8001654C, "SLUS_010.53");
    for (index = 0; index < players; ++index)
    {
        uint32_t object = rrj_read32(0x8005B268 + index * 4);
        uint32_t output = rrj_read32(0x8005B42C) + index * 132;
        uint32_t table = rrj_read32(object + 556);
        uint32_t values = 0x800D6BD8;
        uint32_t profile;
        uint32_t i;
        int32_t value;
        int32_t denominator;
        uint32_t scale;
        rrj_write32(output, object);
        profile = 0x800525F4 + 20 * (rrj_read32(state + 72 + index * 4) / 9);
        rrj_write32(output + 44, profile);
        for (i = 0; i < 10; ++i)
        {
            value = rrj_s32((uint32_t)sub_8001FC90(rrj_read32(table + 188), rrj_read32(table + 20 + i * 4)));
            if (value)
            {
                denominator = (value >> 1) + ((value - 2) >> 31);
                scale = leaf_divu(0x80000000u, (uint32_t)denominator);
                rrj_write32(values + i * 4, (uint32_t)sub_8001FC90(rrj_read32(table + 20), scale));
            }
        }
        value = rrj_s32(rrj_read32(table + 180));
        denominator = (value >> 1) + ((value - 2) >> 31);
        scale = leaf_divu(0x80000000u, (uint32_t)denominator);
        rrj_write32(output + 60, (uint32_t)sub_8001FC90(rrj_read32(profile) << 4, scale));
        value = rrj_s32(rrj_read32(table + 204));
        denominator = (value >> 2) + (((value >> 1) - 2) >> 31);
        rrj_write32(output + 68, leaf_divu(0x80000000u, (uint32_t)denominator));
        denominator = (value >> 3) + (((value >> 2) - 2) >> 31);
        rrj_write32(output + 72, leaf_divu(0x80000000u, (uint32_t)denominator));
        value = (rrj_s32(rrj_read32(table + 400)) >> 2) + (rrj_s32(rrj_read32(table + 400)) >> 1);
        denominator = (value >> 1) + ((value - 2) >> 31);
        rrj_write32(output + 80, leaf_divu(0x80000000u, (uint32_t)denominator));
    }
    return 0;
}

uint32_t sub_80018C1C(uint32_t enabled)
{
    uint32_t packet = 0x1F800340;
    uint32_t primary;
    uint32_t secondary;
    uint32_t result;
    FUNCTION_MARKER(0x80018C1C, "SLUS_010.53");
    rrj_write32(packet + 8, 64);
    rrj_write32(packet + 4, (uint32_t)((127 * rrj_s32(rrj_read32(0x800D6C0C))) >> 7));
    if (enabled)
    {
        uint32_t context;
        uint32_t state;
        result = rrj_read32(0x8005B434);
        if (result)
            return result;
        sub_8001F054(0, 0);
        context = rrj_read32(0x8005B40C);
        state = rrj_read32(0x8005B2F8);
        rrj_write32(context + 24, 0);
        if (rrj_read32(state + 48) == 2)
            rrj_write32(context + 96, 0);
        rrj_write32(0x800D6C20, rrj_read32(0x800D6C00));
        rrj_write32(0x800D6C28, rrj_read32(0x800D6C08));
        rrj_write32(0x800D6C2C, rrj_read32(0x800D6C0C));
        rrj_write32(0x800D6C34, rrj_read32(0x800D6C14));
        if (rrj_read32(0x800CDAC0) != 2 || !(r_u8(state + 4) & 1))
            rrj_write32(0x800D6C24, rrj_read32(0x800D6C04));
        rrj_write32(0x800D6C00, 0);
        rrj_write32(0x800D6C04, 0);
        rrj_write32(0x800D6C08, 0);
        rrj_write32(packet, 0);
        primary = rrj_read32(0x8005B438);
        secondary = rrj_read32(0x800D6BC0);
        if (primary)
            sub_8001F6A4(primary, packet);
        if (secondary)
            sub_8001F6A4(secondary, packet);
        rrj_write32(0x8005B434, 1);
        return 1;
    }
    rrj_write32(packet, 1486);
    rrj_write32(0x800D6C00, rrj_read32(0x800D6C20));
    rrj_write32(0x800D6C04, rrj_read32(0x800D6C24));
    rrj_write32(0x800D6C08, rrj_read32(0x800D6C28));
    rrj_write32(0x800D6C0C, rrj_read32(0x800D6C2C));
    rrj_write32(0x800D6C14, rrj_read32(0x800D6C34));
    primary = rrj_read32(0x8005B438);
    secondary = rrj_read32(0x800D6BC0);
    if (primary)
        sub_8001F6A4(primary, packet);
    result = 743;
    if (secondary)
    {
        rrj_write32(packet, 743);
        result = sub_8001F6A4(secondary, packet);
    }
    rrj_write32(0x8005B434, 0);
    return result;
}

uint32_t sub_80019D9C(void)
{
    static const uint32_t reset_words[] = {0x8005B40C, 0x8005B42C, 0x8005B410, 0x8005B414, 0x8005B3F8, 0x8005B418, 0x8005B3F0, 0x8005B430, 0x8005B408, 0x8005B428, 0x8005B3E4, 0x8005B3EC, 0x8005B434};
    uint32_t index;
    FUNCTION_MARKER(0x80019D9C, "SLUS_010.53");
    rrj_write32(0x8005B404, 0xFFFFFFFF);
    rrj_write32(0x8005B420, 0xFFFFFFFF);
    rrj_write32(0x8005B3FC, 0xFFFFFFFF);
    for (index = 0; index < sizeof reset_words / sizeof reset_words[0]; ++index)
        rrj_write32(reset_words[index], 0);
    w_u8(0x8005B3F4, 0);
    rrj_write32(0x8005B3E0, 1);
    for (index = 0; index < 7; ++index)
    {
        uint32_t value = (uint32_t)(rrj_s32(rrj_read32(0x80052638 + index * 4) * 7) >> 4);
        rrj_write32(0x800D6C00 + index * 4, value);
        rrj_write32(0x800D6C20 + index * 4, value);
    }
    return 0;
}

uint32_t sub_8001A424(RRJRaceLeafCall call)
{
    uint32_t state = rrj_read32(0x8005B2F8);
    uint32_t mode = r_u8(state + 4);
    uint32_t players = rrj_read32(state + 48);
    uint32_t types = 0x800D6C40;
    uint32_t index;
    uint32_t result = 0x800D0000;
    FUNCTION_MARKER(0x8001A424, "SLUS_010.53");
    rrj_write32(types, 6);
    rrj_write32(types + 4, 0);
    rrj_write32(types + 8, 1);
    rrj_write32(types + 12, 4);
    rrj_write32(types + 16, 3);
    rrj_write32(types + 20, 11);
    rrj_write32(types + 24, 4);
    rrj_write32(types + 28, 255);
    rrj_write32(types + 32, 15);
    if (players == 2)
    {
        rrj_write32(types + 28, 22);
        rrj_write32(types + 32, 7);
    }
    if (mode == 33)
    {
        rrj_write32(types + 4, 7);
        rrj_write32(types + 8, 7);
        rrj_write32(types + 12, 7);
        rrj_write32(types + 24, 7);
    }
    if ((mode & 1) && mode != 33)
    {
        rrj_write32(types + 24, 23);
        if (players == 1)
            rrj_write32(types, 7);
        if (rrj_s32(rrj_read32(0x8005B1F8)) < 3 || rrj_read32(rrj_read32(0x8005B3A0) + 2372) >= 9)
            rrj_write32(types + 4, 255);
        else
            rrj_write32(types + 8, 255);
        if (players == 1)
            rrj_write32(types + 12, 255);
    }
    for (index = 0; index < 9; ++index)
    {
        uint32_t record = 0x800D6AA0 + index * 32;
        if (!(rrj_read32(types + index * 4) & 8))
            rrj_write32(record + 28, sub_8001447C(56, 0, call));
        rrj_write32(record + 12, 255);
        rrj_write32(record + 16, 255);
    }
    if (rrj_s32(rrj_read32(0x800D6858)) >= 0)
    {
        uint32_t stream = 0x800D6858;
        if (mode == 33)
            rrj_write32(stream + 4, 0x23C000);
        else
        {
            uint32_t random = (uint32_t)sub_80043F00(0xF2000002) & 0xFF;
            rrj_write32(stream + 4, ((133 * random) >> 8) << 14);
        }
        sub_80023148(stream, 7, 0, 0, call);
        if ((mode & 1) && mode != 33)
        {
            rrj_write32(stream + 4, rrj_read32(types + 8) == 255 ? 0x234000 : 0x238000);
            sub_80023148(stream, 1, 0, 0, call);
        }
        rrj_write32(0x800D6BC0, 0);
        rrj_write32(0x800D6BC4, 0);
        rrj_write32(0x800D6BC8, 1);
        rrj_write32(0x800D6BD0, 0);
        rrj_write32(0x800D6BD4, 0);
        rrj_write32(0x800D6B40, rrj_read32(0x8005B404));
        rrj_write32(0x800D6B44, 11);
        rrj_write32(0x800D6B48, 2);
        rrj_write32(0x800D6B4C, 11);
        rrj_write32(0x800D6B50, 11);
        rrj_write32(0x800D6B54, 88);
        rrj_write32(0x800D6B58, 89);
        rrj_write32(0x800D6B5C, 0);
        rrj_write32(0x8005B424, 0);
        result = 7;
        if (rrj_read32(0x800D6C60) == 15)
        {
            rrj_write32(0x800D6BA0, rrj_read32(0x8005B404));
            rrj_write32(0x800D6BA4, 7);
            rrj_write32(0x800D6BA8, 2);
            rrj_write32(0x800D6BAC, 15);
            rrj_write32(0x800D6BB0, 255);
            rrj_write32(0x800D6BB4, 108);
            rrj_write32(0x800D6BB8, 108);
            rrj_write32(0x800D6BBC, 0);
            result = 108;
        }
        rrj_write32(0x8005B440, 0);
    }
    return result;
}

uint32_t sub_8001B494(RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x8001B494, "SLUS_010.53");
    w_u8(0x8005B450, 0);
    leaf_call(rrj_host_context(), call, 0x8004CEEC, 0, 0, 0, 0);
    leaf_call(rrj_host_context(), call, 0x80047934, 0, 0, 0, 0);
    return 0;
}

uint32_t sub_8001B4C0(RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x8001B4C0, "SLUS_010.53");
    leaf_call(rrj_host_context(), call, 0x800486C8, 0, 0, 0, 0);
    if (r_u8(0x8005B450))
        sub_8001B5C8(call);
    return leaf_call(rrj_host_context(), call, 0x80047A28, 0, 0, 0, 0);
}

uint32_t sub_8001B564(RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x8001B564, "SLUS_010.53");
    rrj_write32(0x8005B44C, 0);
    rrj_write32(0x8005B448, 0);
    leaf_call(rrj_host_context(), call, 0x800479C4, 0x8001B500, 0, 0, 0);
    w_u8(0x8005B450, 1);
    return 1;
}

uint32_t sub_8001B594(uint32_t callback)
{
    FUNCTION_MARKER(0x8001B594, "SLUS_010.53");
    if (!r_u8(0x8005B450) || rrj_read32(0x8005B448))
        return 3;
    rrj_write32(0x8005B448, callback);
    return 0;
}

uint32_t sub_8001B5C8(RRJRaceLeafCall call)
{
    uint32_t result;
    FUNCTION_MARKER(0x8001B5C8, "SLUS_010.53");
    result = leaf_call(rrj_host_context(), call, 0x800479C4, 0, 0, 0, 0);
    rrj_write32(0x8005B448, 0);
    w_u8(0x8005B450, 0);
    return result;
}

uint32_t sub_8001B5EC(RRJRaceLeafCall call)
{
    uint32_t path = 0x1F800000;
    uint32_t index;
    FUNCTION_MARKER(0x8001B5EC, "SLUS_010.53");
    leaf_call(rrj_host_context(), call, 0x80043FD4, path, 0x80010AA0, 0x80052464, 0);
    index = sub_8001458C(path, rrj_read32(0x8005AD5C), call);
    if (rrj_s32(index) < 0)
        return leaf_call(rrj_host_context(), call, 0x80044894, 0x80010AAC, path, 0, 0);
    sub_80014780(index, 0x800D6C68, 76, call);
    return sub_8001460C(index, call);
}

uint32_t sub_8001BFA8(RRJRaceLeafCall call)
{
    uint32_t previous;
    uint32_t result;
    FUNCTION_MARKER(0x8001BFA8, "SLUS_010.53");
    previous = rrj_read32(0x800D6C68);
    sub_8001B5EC(call);
    sub_8001B670(previous);
    result = 0x800D6C6C + 24 * rrj_read32(0x800D6C68);
    rrj_write32(0x8005B474, result);
    return result;
}

uint32_t sub_8001B670(uint32_t value)
{
    FUNCTION_MARKER(0x8001B670, "SLUS_010.53");
    rrj_write32(0x800D6C68, value);
    return 0x800D0000;
}

uint32_t sub_8001B6C4(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    PSX_RECT rectangle;
    FUNCTION_MARKER(0x8001B6C4, "SLUS_010.53");
    rectangle.x = (sint16)(uint16_t)x;
    rectangle.y = (sint16)(uint16_t)y;
    rectangle.w = (sint16)(uint16_t)width;
    rectangle.h = (sint16)(uint16_t)height;
    return (uint32_t)ClearImage(&rectangle, 0, 0, 0);
}

uint32_t sub_8001B868(void)
{
    uint32_t mode;
    uint32_t result;
    uint32_t width;
    FUNCTION_MARKER(0x8001B868, "SLUS_010.53");
    mode = rrj_read32(0x8005AD88);
    result = mode;
    if (mode)
    {
        rrj_write32(0x8005B468, 1);
        width = mode == 1 ? 262 : 136;
        do
            result = rrj_read32(0x8005B460) + width - 1;
        while (rrj_s32(rrj_read32(0x8005B45C)) < rrj_s32(result));
    }
    rrj_write32(0x8005B468, 0);
    rrj_write32(0x8005AD88, 0);
    return result;
}

uint32_t sub_8001BC54(RRJRaceLeafCall call)
{
    uint32_t rectangle = 0x1F8003A0;
    uint32_t record = 0x1F8003B0;
    uint32_t buffer_out = 0x1F8003D0;
    uint32_t result;
    FUNCTION_MARKER(0x8001BC54, "SLUS_010.53");
    rrj_write32(buffer_out, 0);
    if (!sub_8001408C(0x80010ADC, buffer_out, record, call))
    {
        rrj_put16(rrj_at(rectangle, 2), 160);
        rrj_put16(rrj_at(rectangle + 2, 2), 200);
        rrj_put16(rrj_at(rectangle + 4, 2), 66);
        rrj_put16(rrj_at(rectangle + 6, 2), 14);
        leaf_call(rrj_host_context(), call, 0x80048A6C, rectangle, rrj_read32(record + 16), 0, 0);
        rrj_put16(rrj_at(rectangle + 2, 2), 456);
        leaf_call(rrj_host_context(), call, 0x80048A6C, rectangle, rrj_read32(record + 16), 0, 0);
    }
    result = leaf_call(rrj_host_context(), call, 0x800487C0, 0, 0, 0, 0);
    if (rrj_read32(buffer_out))
        return sub_800144B8(rrj_read32(buffer_out));
    return result;
}

uint32_t sub_8001BDB0(RRJRaceLeafCall call)
{
    uint32_t result;
    FUNCTION_MARKER(0x8001BDB0, "SLUS_010.53");
    sub_80043DA4();
    sub_8001B564(call);
    result = sub_8001B594(0x8001B700);
    sub_80043DB4();
    return result;
}

uint32_t sub_8001BDEC(void)
{
    uint32_t context;
    uint32_t result;
    FUNCTION_MARKER(0x8001BDEC, "SLUS_010.53");
    context = rrj_read32(0x8005B470);
    result = rrj_read32(context + 240) & 0x00FFFFFF;
    rrj_write32(context + 268, result);
    return result;
}

uint32_t sub_8001C004(RRJRaceLeafCall call)
{
    uint32_t context = 0x800D6CB8;
    uint32_t state;
    uint32_t players;
    int32_t entries;
    uint32_t size;
    uint32_t allocation;
    uint32_t packet;
    uint32_t result;
    FUNCTION_MARKER(0x8001C004, "SLUS_010.53");
    rrj_write32(0x8005B470, context);
    rrj_write32(0x800D6DA8, 0);
    rrj_write32(0x8005B46C, 0);
    leaf_call(rrj_host_context(), call, 0x80048444, 0, 0, 0, 0);
    sub_8001B6C4(0, 0, 1024, 512);
    context = rrj_read32(0x8005B470);
    w_u8(context + 5, 0);
    context = rrj_read32(0x8005B470);
    w_u8(context + 6, 1);
    context = rrj_read32(0x8005B470);
    w_u8(context + 7, 0);
    context = rrj_read32(0x8005B470);
    w_u8(context + 4, 1);
    context = rrj_read32(0x8005B470);
    rrj_write32(context + 268, rrj_read32(context + 240) & 0x00FFFFFF);
    leaf_call(rrj_host_context(), call, 0x8001BE08, 0, 0, 384, 240);
    sub_80048414(0);
    context = rrj_read32(0x8005B470);
    rrj_write32(context, 0);
    leaf_call(rrj_host_context(), call, 0x80048728, 1, 0, 0, 0);
    sub_8001BDEC();
    leaf_call(rrj_host_context(), call, 0x800487C0, 0, 0, 0, 0);
    state = rrj_read32(0x8005B2F8);
    players = rrj_read32(state + 48);
    entries = r_s16(0x8005AD84 + (players - 1) * 2);
    size = (uint32_t)entries * 60;
    allocation = sub_8001447C(size, 0, call);
    context = rrj_read32(0x8005B470);
    state = rrj_read32(0x8005B2F8);
    rrj_write32(context + 240, allocation);
    players = rrj_read32(state + 48);
    entries = r_s16(0x8005AD84 + (players - 1) * 2);
    rrj_write32(0x800D75B0, allocation & 0x00FFFFFF);
    result = (allocation + (uint32_t)entries * 60 - 60) & 0x00FFFFFF;
    packet = rrj_read32(context + 240) & 0x00FFFFFF;
    rrj_write32(0x800D75B4, packet);
    rrj_write32(context + 268, packet);
    rrj_write32(0x8005B4DC, result);
    rrj_write32(0x8005B4D0, result);
    return result;
}

uint32_t sub_8001C408(RRJRaceLeafPoll poll)
{
    uint32_t context;
    uint32_t ready;
    FUNCTION_MARKER(0x8001C408, "SLUS_010.53");
    context = rrj_read32(0x8005B470);
    do
    {
        ready = r_u8(context + 4);
        if (!ready && poll)
            poll(rrj_host_context());
    } while (!ready);
    return ready;
}

uint32_t sub_8001C4D0(void)
{
    uint32_t base = 0x800D7128;
    uint32_t address;
    uint32_t index;
    FUNCTION_MARKER(0x8001C4D0, "SLUS_010.53");
    sub_8001E100(base, 0, 192);
    sub_8001C4A8(base, 0);
    address = base + 144;
    for (index = 0; index < 19; ++index)
    {
        w_u8(address + 25, 30);
        address -= 8;
    }
    for (index = 1; index < 4; ++index)
        sub_8001E0B4(base + index * 192, base, 192);
    sub_80043DA4();
    sub_8001E0B4(0x800D6DE0, base, 768);
    sub_80043DB4();
    return 0;
}

uint32_t sub_8001C590(RRJRaceLeafCall call)
{
    int32_t index;
    FUNCTION_MARKER(0x8001C590, "SLUS_010.53");
    leaf_call(rrj_host_context(), call, 0x80042964, 0x800D70E0, 0x800D7104, 0, 0);
    for (index = 3; index >= 0; --index)
        rrj_write32(0x800D7428 + 24 * (uint32_t)index, (uint32_t)index);
    rrj_write32(0x800D7448, 1);
    rrj_write32(0x800D7430, 1);
    leaf_call(rrj_host_context(), call, 0x800404C4, 0, 0, 0, 0);
    return sub_8001C4D0();
}

uint32_t sub_8001DFE4(RRJRaceLeafCall call)
{
    uint32_t handle;
    FUNCTION_MARKER(0x8001DFE4, "SLUS_010.53");
    sub_80043DA4();
    handle = leaf_call(rrj_host_context(), call, 0x80043D64, 0xF2000002, 2, 4096, 0x8001DF64);
    rrj_write32(0x800527D4, handle);
    if (!leaf_call(rrj_host_context(), call, 0x80043D84, handle, 0, 0, 0))
        abort();
    leaf_call(rrj_host_context(), call, 0x80043E64, 0xF2000002, 0xFFFF, 4096, 0);
    leaf_call(rrj_host_context(), call, 0x80043F9C, 0xF2000002, 0, 0, 0);
    leaf_call(rrj_host_context(), call, 0x80043F38, 0xF2000002, 0, 0, 0);
    sub_80043DB4();
    rrj_write32(0x800527D8, 0);
    rrj_write32(0x800527DC, 0);
    return 0;
}

uint32_t sub_8001E08C(uint32_t destination, uint32_t source, uint32_t size)
{
    uint32_t result = destination;
    FUNCTION_MARKER(0x8001E08C, "SLUS_010.53");
    while (size)
    {
        w_u8(destination++, r_u8(source++));
        --size;
    }
    return result;
}

uint32_t sub_8001E138(RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x8001E138, "SLUS_010.53");
    rrj_write32(0x8005B484, sub_80051058(0x8001E328));
    rrj_write32(0x8005B48C, sub_80051018());
    leaf_call(rrj_host_context(), call, 0x80050FE8, 0, 0, 0, 0);
    rrj_write32(0x8005B494, 0);
    rrj_write32(0x8005B480, 0);
    rrj_write32(0x8005B490, 0);
    rrj_write32(0x8005B498, 0);
    rrj_write32(0x8005B488, 1);
    return 1;
}

uint32_t sub_8001E188(RRJRaceLeafCall call)
{
    uint32_t result;
    FUNCTION_MARKER(0x8001E188, "SLUS_010.53");
    result = rrj_read32(0x8005B488);
    if (result)
    {
        sub_8001E1D0();
        leaf_call(rrj_host_context(), call, 0x80050FE8, rrj_read32(0x8005B48C), 0, 0, 0);
        result = sub_80051058(rrj_read32(0x8005B484));
    }
    rrj_write32(0x8005B490, 0);
    rrj_write32(0x8005B498, 0);
    rrj_write32(0x8005B488, 0);
    return result;
}

uint32_t sub_8001E1D0(void)
{
    uint32_t active;
    uint32_t index;
    uint32_t result;
    FUNCTION_MARKER(0x8001E1D0, "SLUS_010.53");
    active = rrj_read32(0x8005B488);
    result = active;
    if (active)
    {
        index = rrj_read32(0x8005B494);
        rrj_write32(0x8005B480, index);
        result = rrj_read32(0x8005B490);
        rrj_write32(0x800D7488 + index * 24 + 12, 0);
    }
    return result;
}

uint32_t sub_8001E48C(uint32_t size, RRJRaceLeafCall call)
{
    uint32_t allocator = rrj_read32(0x8005AD9C);
    uint32_t result;
    uint32_t index;
    FUNCTION_MARKER(0x8001E48C, "SLUS_010.53");
    if (allocator)
        result = leaf_call(rrj_host_context(), call, allocator, size, 0, 0, 0);
    else
        result = sub_8001447C(size, 0, call);
    if (result)
    {
        for (index = 0; index < size; ++index)
            w_u8(result + index, 0);
    }
    return result;
}

uint32_t sub_8001E500(uint32_t pointer, RRJRaceLeafCall call)
{
    uint32_t release = rrj_read32(0x8005ADA0);
    FUNCTION_MARKER(0x8001E500, "SLUS_010.53");
    if (release)
        return leaf_call(rrj_host_context(), call, release, pointer, 0, 0, 0);
    sub_800144B8(pointer);
    return 0;
}

static uint32_t register_pair(RRJMemory *m, uint32_t first, uint32_t second, uint32_t first_slot, uint32_t second_slot)
{
    if ((first || second) && (rrj_read32(first_slot) || rrj_read32(second_slot)))
        return 0xFFFFFFFF;
    if ((first && !second) || (!first && second))
        return 0xFFFFFFFF;
    rrj_write32(first_slot, first);
    rrj_write32(second_slot, second);
    return 0;
}

uint32_t sub_8001E544(uint32_t first, uint32_t second)
{
    FUNCTION_MARKER(0x8001E544, "SLUS_010.53");
    return register_pair(rrj_host_context(), first, second, 0x8005AD9C, 0x8005ADA0);
}

uint32_t sub_8001E5A8(void)
{
    uint32_t index;
    FUNCTION_MARKER(0x8001E5A8, "SLUS_010.53");
    for (index = 0; index < 7; ++index)
        rrj_write32(0x800D6870 + index * 4, 0);
    for (index = 0; index < 3; ++index)
        rrj_write32(0x800D6898 - index * 20 + 28, 0);
    rrj_write32(0x800D69F0, 127);
    rrj_write32(0x800D69F4, 0);
    return sub_8001FBD4();
}

uint32_t sub_8001E614(uint32_t table, RRJRaceLeafCall call)
{
    uint32_t value_out = 0x1F8003F8;
    uint32_t first_count = 1;
    uint32_t item_count = 24;
    uint32_t first;
    uint32_t items;
    uint32_t index;
    FUNCTION_MARKER(0x8001E614, "SLUS_010.53");
    if (sub_8001E7B0(table, 4, value_out, 0))
    {
        first_count = rrj_read32(value_out);
        rrj_write32(0x800D6870, first_count);
    }
    if (sub_8001E7B0(table, 5, value_out, 0))
        item_count = rrj_read32(value_out);
    first = sub_8001E48C(first_count * 4, call);
    rrj_write32(0x800D6874, first);
    if (!first)
    {
        sub_8001E738(call);
        return 0xFFFFFFFF;
    }
    items = sub_8001E48C(item_count * 44, call);
    rrj_write32(0x800D687C, items);
    if (!items)
    {
        sub_8001E738(call);
        return 0xFFFFFFFF;
    }
    for (index = 0; rrj_s32(index) < rrj_s32(item_count); ++index)
        rrj_write32(items + index * 44 + 28, index);
    rrj_write32(0x800D6878, item_count);
    return 0;
}

uint32_t sub_8001E738(RRJRaceLeafCall call)
{
    uint32_t pointer;
    uint32_t result;
    FUNCTION_MARKER(0x8001E738, "SLUS_010.53");
    pointer = rrj_read32(0x800D6874);
    result = pointer;
    if (pointer)
    {
        sub_8001E818(call);
        result = sub_8001E500(pointer, call);
        rrj_write32(0x800D6874, 0);
        rrj_write32(0x800D6870, 0);
    }
    pointer = rrj_read32(0x800D687C);
    if (pointer)
    {
        result = sub_8001E500(pointer, call);
        rrj_write32(0x800D687C, 0);
        rrj_write32(0x800D6878, 0);
    }
    return result;
}

uint32_t sub_8001E7B0(uint32_t table, uint32_t key, uint32_t value_out, uint32_t index_out)
{
    uint32_t index = 0;
    FUNCTION_MARKER(0x8001E7B0, "SLUS_010.53");
    if (!table)
        return 0;
    while (rrj_read32(table))
    {
        if (rrj_read32(table) == key)
        {
            if (value_out)
                rrj_write32(value_out, rrj_read32(table + 4));
            if (index_out)
                rrj_write32(index_out, index);
            return 1;
        }
        table += 8;
        ++index;
    }
    return 0;
}

uint32_t sub_8001E818(RRJRaceLeafCall call)
{
    uint32_t index = 0;
    uint32_t count;
    FUNCTION_MARKER(0x8001E818, "SLUS_010.53");
    count = rrj_read32(0x800D6870);
    while (rrj_s32(index) < rrj_s32(count))
    {
        sub_8001EE28(index++, call);
        count = rrj_read32(0x800D6870);
    }
    return index < count;
}

uint32_t sub_8001E8C4(RRJRaceLeafCall call)
{
    uint32_t descriptor = 0x1F8003A0;
    FUNCTION_MARKER(0x8001E8C4, "SLUS_010.53");
    xport_guest_fill(descriptor, 0, 40);
    rrj_write32(descriptor, 0x3FC3);
    rrj_put16(rrj_at(descriptor + 4, 2), 0x3FFF);
    rrj_put16(rrj_at(descriptor + 6, 2), 0x3FFF);
    return leaf_call(rrj_host_context(), call, 0x800510B8, descriptor, 0, 0, 0);
}

uint32_t sub_8001EA54(uint32_t index, RRJRaceLeafCall call)
{
    uint32_t base;
    uint32_t record;
    FUNCTION_MARKER(0x8001EA54, "SLUS_010.53");
    base = rrj_read32(0x800D6874);
    record = rrj_read32(base + index * 4);
    if (!record)
        return base;
    leaf_call(rrj_host_context(), call, 0x8004F998, rrj_read32(record + 8), 0, 0, 0);
    rrj_write32(record + 8, 0xFFFFFFFF);
    return 0xFFFFFFFF;
}

uint32_t sub_8001EB60(uint32_t flags)
{
    uint32_t result;
    FUNCTION_MARKER(0x8001EB60, "SLUS_010.53");
    result = rrj_read32(0x800D6888) | flags;
    rrj_write32(0x800D6888, result);
    return result;
}

uint32_t sub_8001EBF0(RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x8001EBF0, "SLUS_010.53");
    sub_8004F2C8(call);
    leaf_call(rrj_host_context(), call, 0x8004E6E0, 0, 0, 0, 0);
    leaf_call(rrj_host_context(), call, 0x8004F368, 12, 0x800D6A38, 0, 0);
    return sub_8001E8C4(call);
}

uint32_t sub_8001EE28(uint32_t index, RRJRaceLeafCall call)
{
    uint32_t count;
    uint32_t base;
    uint32_t result;
    FUNCTION_MARKER(0x8001EE28, "SLUS_010.53");
    count = rrj_read32(0x800D6870);
    result = rrj_s32(count) < rrj_s32(index);
    if (rrj_s32(count) >= rrj_s32(index) && rrj_s32(index) >= 0)
    {
        base = rrj_read32(0x800D6874);
        result = base;
        if (base)
        {
            sub_8001EA54(index, call);
            result = base + index * 4;
            rrj_write32(result, 0);
        }
    }
    return result;
}

uint32_t sub_8001EC30(uint32_t first, uint32_t second, uint32_t third, uint32_t fourth, uint32_t fifth, uint32_t sixth, uint32_t seventh, uint32_t eighth, RRJRaceLeafCall call)
{
    uint32_t table = 0x1F800300;
    uint32_t value_out = 0x1F800320;
    uint32_t allocate = 0;
    uint32_t release = 0;
    uint32_t result;
    FUNCTION_MARKER(0x8001EC30, "SLUS_010.53");
    rrj_write32(table, first);
    rrj_write32(table + 4, second);
    rrj_write32(table + 8, third);
    rrj_write32(table + 12, fourth);
    rrj_write32(table + 16, fifth);
    rrj_write32(table + 20, sixth);
    rrj_write32(table + 24, seventh);
    rrj_write32(table + 28, eighth);
    rrj_write32(value_out, 0);
    sub_8001E5A8();
    if (sub_8001E7B0(table, 2, value_out, 0))
        allocate = rrj_read32(value_out);
    if (sub_8001E7B0(table, 3, value_out, 0))
        release = rrj_read32(value_out);
    sub_8001E544(allocate, release);
    result = sub_8001E614(table, call);
    if (result)
        sub_8001ECEC(call);
    return result;
}

uint32_t sub_8001ECEC(RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x8001ECEC, "SLUS_010.53");
    sub_8001F0D4(call);
    sub_8004F2E8(call);
    sub_8001E738(call);
    return sub_8001E544(0, 0);
}

uint32_t sub_8001F054(uint32_t x, uint32_t y)
{
    uint32_t point = 0x1F8003D0;
    FUNCTION_MARKER(0x8001F054, "SLUS_010.53");
    rrj_write32(point, 0);
    rrj_put16(rrj_at(point + 8, 2), x);
    rrj_put16(rrj_at(point + 10, 2), y);
    return (uint32_t)sub_800505F8(point);
}

uint32_t sub_800505F8(uint32_t point)
{
    uint32_t flags = rrj_read32(point);

    FUNCTION_MARKER(0x800505F8, "SLUS_010.53");
    if (!flags || (flags & 2))
    {
        uint16_t x = rrj_u16(rrj_at(point + 8, 2));
        rrj_put16(rrj_at(0x8005A3C4, 2), x);
    }
    if (!flags || (flags & 4))
    {
        uint16_t y = rrj_u16(rrj_at(point + 10, 2));
        rrj_put16(rrj_at(0x8005A3C6, 2), y);
    }
    rrj_spu_set_cd_volume((int16_t)rrj_u16(rrj_at(0x8005A3C4, 2)), (int16_t)rrj_u16(rrj_at(0x8005A3C6, 2)));
    return 0;
}

uint32_t sub_8001F080(RRJRaceLeafCall call)
{
    uint32_t descriptor = 0x1F800360;
    FUNCTION_MARKER(0x8001F080, "SLUS_010.53");
    xport_guest_fill(descriptor, 0, 32);
    rrj_write32(descriptor, 0x00FFFFFF);
    rrj_write32(descriptor + 4, 147);
    rrj_write32(descriptor + 28, 16534);
    leaf_call(rrj_host_context(), call, 0x80051438, descriptor, 0, 0, 0);
    return leaf_call(rrj_host_context(), call, 0x80050D08, 1, rrj_read32(descriptor), 0, 0);
}

uint32_t sub_8001F0D4(RRJRaceLeafCall call)
{
    uint32_t count;
    uint32_t record;
    uint32_t mask = 0;
    FUNCTION_MARKER(0x8001F0D4, "SLUS_010.53");
    count = rrj_read32(0x800D6878);
    record = rrj_read32(0x800D687C);
    while (count)
    {
        if (rrj_read32(record + 4))
        {
            mask |= 1u << rrj_read32(record + 28);
            (uint32_t)sub_8001FB58(record);
        }
        record += 44;
        --count;
    }
    sub_8001EB60(mask);
    return leaf_call(rrj_host_context(), call, 0x8001EE94, 0, 0, 0, 0);
}

uint32_t sub_8001F37C(uint32_t alternate, uint32_t event, uint32_t owner, RRJRaceLeafCall call)
{
    uint32_t voice;
    uint32_t handle = 0;
    int32_t volume = rrj_s32(rrj_read32(event + 4) << 7);
    int32_t left;
    int32_t right;
    FUNCTION_MARKER(0x8001F37C, "SLUS_010.53");
    if (rrj_read32(0x800D69F4))
        left = right = volume;
    else
    {
        int32_t pan = rrj_s32(rrj_read32(event + 8));
        int32_t distance = pan < 129 ? pan : 256 - pan;
        int32_t scaled = rrj_s32((uint32_t)(distance - 64) * (uint32_t)volume);
        int32_t quarter = scaled >> 6;
        left = volume - (quarter & ((-quarter) >> 31));
        left ^= pan >> 31;
        if (pan >= 129)
            left = -left;
        right = volume + (quarter & (scaled >> 31));
    }
    voice = (uint32_t)sub_8001F9C4(1);
    if (voice)
    {
        uint32_t channel = rrj_read32(voice + 28);
        uint32_t slot;
        uint32_t metadata = 0;
        handle = (channel << 27) + (rrj_read32(voice + 4) & 0x07FFFFFF);
        rrj_write32(voice + 8, 0);
        for (slot = 0x800D6870; slot < 0x800D68AC; slot += 20)
        {
            if (!rrj_read32(slot + 28))
            {
                metadata = slot + 32;
                rrj_write32(slot + 28, 1);
                rrj_write32(slot + 44, handle);
                rrj_write32(slot + 40, owner);
                w_u8(slot + 32, 127);
                w_u8(slot + 33, 127);
                rrj_put16(rrj_at(slot + 34, 2), alternate ? 0xB9FF : 0x98FF);
                rrj_put16(rrj_at(slot + 36, 2), 0x5FCE);
                rrj_write32(voice + 8, metadata);
                break;
            }
        }
        if (!rrj_read32(voice + 8))
        {
            (uint32_t)sub_8001FB58(voice);
            return 0;
        }
        rrj_write32(voice + 32, rrj_read32(event));
        rrj_write32(voice + 36, (uint32_t)left);
        rrj_write32(voice + 40, (uint32_t)right);
        leaf_call(rrj_host_context(), call, 0x80050678, 0, 1u << (channel & 31), 0, 0);
    }
    return handle;
}

uint32_t sub_8001F544(uint32_t count, uint32_t packets)
{
    uint32_t index;
    uint32_t mask = 0;
    uint32_t table;
    FUNCTION_MARKER(0x8001F544, "SLUS_010.53");
    table = rrj_read32(0x800D687C);
    for (index = 0; rrj_s32(index) < rrj_s32(count); ++index)
    {
        uint32_t packet = rrj_read32(packets + index * 4);
        uint32_t slot = packet >> 27;
        if ((rrj_read32(table + slot * 44 + 4) & 0x07FFFFFF) == (packet & 0x07FFFFFF))
            mask |= 1u << slot;
    }
    return (uint32_t)sub_8001EB28(mask);
}

uint32_t sub_8001F6A4(uint32_t handle, uint32_t event)
{
    uint32_t record;
    uint32_t identity;
    uint32_t channel;
    uint32_t packet = 0x1F800360;
    int32_t product;
    int32_t left;
    int32_t right;
    FUNCTION_MARKER(0x8001F6A4, "SLUS_010.53");
    record = rrj_read32(0x800D687C) + 44 * (handle >> 27);
    identity = rrj_read32(record + 4) & 0x07FFFFFF;
    if (identity != (handle & 0x07FFFFFF))
        return identity;
    product = rrj_s32(rrj_read32(event + 4) * r_u8(rrj_read32(record + 8)));
    if (rrj_read32(0x800D69F4))
        left = right = product;
    else
    {
        int32_t pan = rrj_s32(rrj_read32(event + 8));
        int32_t distance = pan < 129 ? pan : 256 - pan;
        int32_t scaled = rrj_s32((uint32_t)(distance - 64) * (uint32_t)product);
        int32_t quarter = scaled >> 6;
        left = product - (quarter & ((-quarter) >> 31));
        left ^= pan >> 31;
        if (pan >= 129)
            left = -left;
        right = product + (quarter & (scaled >> 31));
    }
    channel = rrj_read32(record + 28);
    rrj_write32(packet, 1u << (channel & 31));
    rrj_write32(packet + 4, 19);
    rrj_put16(rrj_at(packet + 8, 2), (uint32_t)left);
    rrj_put16(rrj_at(packet + 10, 2), (uint32_t)right);
    rrj_put16(rrj_at(packet + 20, 2), r_u16(event));
    return (uint32_t)sub_80051C38(channel, packet);
}

uint32_t sub_80019C54(uint32_t handle, uint32_t event)
{
    uint32_t record;
    uint32_t identity;
    uint32_t channel;
    uint32_t packet = 0x1F800360;
    int32_t product;
    int32_t left;
    int32_t right;

    FUNCTION_MARKER(0x80019C54, "SLUS_010.53");
    record = rrj_read32(0x800D687C) + 44 * (handle >> 27);
    identity = rrj_read32(record + 4) & 0x07FFFFFF;
    if (identity != (handle & 0x07FFFFFF))
        return identity;
    product = rrj_s32(rrj_read32(event + 4) * r_u8(rrj_read32(record + 8)));
    if (rrj_read32(0x800D69F4))
        left = right = product;
    else
    {
        int32_t pan = rrj_s32(rrj_read32(event + 8));
        int32_t distance = pan < 129 ? pan : 256 - pan;
        int32_t scaled = rrj_s32((uint32_t)(distance - 64) * (uint32_t)product);
        int32_t quarter = scaled >> 6;

        left = product - (quarter & ((-quarter) >> 31));
        left ^= pan >> 31;
        if (pan >= 129)
            left = -left;
        right = product + (quarter & (scaled >> 31));
    }
    channel = rrj_read32(record + 28);
    rrj_write32(packet, 1u << (channel & 31));
    rrj_write32(packet + 4, 19);
    rrj_put16(rrj_at(packet + 8, 2), (uint32_t)left);
    rrj_put16(rrj_at(packet + 10, 2), (uint32_t)right);
    rrj_put16(rrj_at(packet + 20, 2), r_u16(event));
    return (uint32_t)sub_80051C38(channel, packet);
}

uint32_t sub_8001FBD4(void)
{
    uint32_t base = 0x800D6870;
    int32_t index;
    FUNCTION_MARKER(0x8001FBD4, "SLUS_010.53");
    for (index = 21; index >= 0; --index)
        rrj_write32(base + 0x58 + index * 4, 0xFFFFFFFF);
    for (index = 23; index >= 0; --index)
        rrj_write32(base + 0xB0 + index * 4, (uint32_t)index);
    for (index = 24; index >= 0; --index)
        rrj_write32(base + 0x110 + index * 4, 0xFFFFFFFF);
    rrj_write32(base + 0x174, 23);
    rrj_write32(base + 0x178, 0);
    rrj_write32(base + 0x17C, 0);
    return base;
}

uint32_t sub_80020228(void)
{
    uint32_t pointer;
    FUNCTION_MARKER(0x80020228, "SLUS_010.53");
    pointer = rrj_read32(0x8005ADB4);
    if (pointer)
        sub_800144B8(pointer);
    rrj_write32(0x8005ADB4, 0);
    pointer = rrj_read32(0x8005ADB8);
    if (pointer)
        sub_800144B8(pointer);
    rrj_write32(0x8005ADB8, 0);
    rrj_write32(0x8005ADBC, 0);
    return 0;
}

uint32_t sub_80020DB8(RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x80020DB8, "SLUS_010.53");
    sub_80020F24(call);
    sub_80021174(call);
    sub_80020FF4(0, 0);
    return 0;
}

uint32_t sub_80020DEC(RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x80020DEC, "SLUS_010.53");
    if (!r_u8(0x800D754A))
        return 0xFFFFFFFF;
    sub_800212FC(call);
    return 0;
}

uint32_t sub_80020E30(uint32_t muted)
{
    uint32_t base = 0x800D7548;
    uint32_t packet = 0x1F800340;
    uint32_t result;
    uint32_t count;
    uint32_t index;
    FUNCTION_MARKER(0x80020E30, "SLUS_010.53");
    result = r_u8(0x800D75A1);
    if (result && !muted)
        return result;
    result = r_u8(base);
    w_u8(base + 1, (uint8_t)muted);
    if (!result)
        return result;
    if (muted)
        rrj_write32(packet, 0);
    else
    {
        rrj_write32(packet, rrj_read32(0x800D7550));
        rrj_write32(0x800D75A8, rrj_read32(rrj_read32(0x8005B2F8) + 12) + rrj_read32(0x800D75A4));
    }
    count = rrj_read32(0x800D754C);
    result = count;
    for (index = 0; index < count; ++index)
    {
        uint32_t record = base + index * 20;
        uint32_t handle = rrj_read32(record + 16);
        if (handle)
        {
            rrj_write32(packet + 4, rrj_read32(record + 20));
            rrj_write32(packet + 8, rrj_read32(record + 24));
            sub_8001F6A4(handle, packet);
        }
        result = index + 1 < count;
    }
    return result;
}

uint32_t sub_80020FF4(uint32_t first, uint32_t second)
{
    FUNCTION_MARKER(0x80020FF4, "SLUS_010.53");
    return register_pair(rrj_host_context(), first, second, 0x8005B4C0, 0x8005B4BC);
}

uint32_t sub_80020F24(RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x80020F24, "SLUS_010.53");
    if (r_u8(0x800D7548))
        sub_80021450(call);
    return 0;
}

uint32_t sub_80021174(RRJRaceLeafCall call)
{
    uint32_t count;
    uint32_t index;
    FUNCTION_MARKER(0x80021174, "SLUS_010.53");
    count = rrj_read32(0x800D754C);
    for (index = 0; rrj_s32(index) < rrj_s32(count); ++index)
    {
        uint32_t record = 0x800D7548 + index * 20;
        leaf_call(rrj_host_context(), call, 0x8004F998, rrj_read32(record + 32), 0, 0, 0);
        rrj_write32(record + 32, 0);
        count = rrj_read32(0x800D754C);
    }
    return 0;
}

uint32_t sub_800212FC(RRJRaceLeafCall call)
{
    uint32_t base = 0x800D7548;
    uint32_t packet = 0x1F800300;
    uint32_t handles = 0x1F800320;
    uint32_t count;
    uint32_t index;
    uint32_t pan = 0;
    FUNCTION_MARKER(0x800212FC, "SLUS_010.53");
    rrj_write32(0x800D759C, 0);
    rrj_write32(0x800D7590, rrj_read32(0x800D7598));
    leaf_call(rrj_host_context(), call, 0x80050C98, 0x8002169C, 0, 0, 0);
    sub_80021644(call);
    if (!rrj_read32(0x800D6C14))
        return 0;
    rrj_write32(packet, r_u8(0x800D7549) ? 0 : rrj_read32(0x800D7550));
    count = rrj_read32(0x800D754C);
    for (index = 0; index < count; ++index)
    {
        uint32_t record = base + index * 20;
        uint32_t handle;
        rrj_write32(record + 20, rrj_read32(0x800D6C14));
        rrj_write32(packet + 4, rrj_read32(0x800D6C14));
        rrj_write32(record + 24, pan);
        rrj_write32(packet + 8, pan);
        pan += 127;
        handle = sub_8001F37C(0, packet, rrj_read32(record + 32), call);
        rrj_write32(record + 16, handle);
        rrj_write32(handles + index * 4, handle);
    }
    sub_8001F544(count, handles);
    w_u8(base, 1);
    rrj_write32(0x800D75A8, rrj_read32(rrj_read32(0x8005B2F8) + 12) + rrj_read32(0x800D75A4));
    return 1;
}

uint32_t sub_80021450(RRJRaceLeafCall call)
{
    uint32_t count;
    uint32_t index;
    uint32_t result = 0x800D0000;
    FUNCTION_MARKER(0x80021450, "SLUS_010.53");
    sub_8002167C(call);
    leaf_call(rrj_host_context(), call, 0x80050C98, 0, 0, 0, 0);
    w_u8(0x800D7548, 0);
    w_u8(0x800D754A, 0);
    count = rrj_read32(0x800D754C);
    for (index = 0; index < count; ++index)
    {
        leaf_call(rrj_host_context(), call, 0x8001F5D4, rrj_read32(0x800D7558 + index * 20), 0, 0, 0);
        count = rrj_read32(0x800D754C);
        result = index + 1 < count;
    }
    return result;
}

uint32_t sub_80021644(RRJRaceLeafCall call)
{
    uint32_t address;
    FUNCTION_MARKER(0x80021644, "SLUS_010.53");
    address = rrj_read32(0x800D7568) + rrj_read32(0x800D7590);
    leaf_call(rrj_host_context(), call, 0x80050C58, address, 0, 0, 0);
    return leaf_call(rrj_host_context(), call, 0x80050B18, 1, 0, 0, 0);
}

uint32_t sub_8002167C(RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x8002167C, "SLUS_010.53");
    return leaf_call(rrj_host_context(), call, 0x80050B18, 0, 0, 0, 0);
}

uint32_t sub_800229B0(void)
{
    uint32_t result;
    FUNCTION_MARKER(0x800229B0, "SLUS_010.53");
    result = rrj_read32(0x8005AE18);
    if (!result)
    {
        rrj_write32(0x80053464, 0);
        sub_80022CC4();
        result = 1;
        rrj_write32(0x8005AE18, result);
    }
    rrj_write32(0x8005AE1C, 0);
    rrj_write32(0x8005AE20, 0);
    rrj_write32(0x8005B4E8, 0);
    rrj_write32(0x8005B4F0, 0);
    return result;
}

uint32_t sub_800229F4(uint32_t mode, RRJRaceLeafCall call)
{
    uint32_t result;
    FUNCTION_MARKER(0x800229F4, "SLUS_010.53");
    sub_80022A1C(mode, call);
    result = rrj_read32(0x80053464);
    rrj_write32(0x8005AE18, 0);
    return result;
}

uint32_t sub_80022A1C(uint32_t mode, RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x80022A1C, "SLUS_010.53");
    sub_80043DA4();
    if (rrj_read32(0x80053464))
        rrj_write32(rrj_read32(0x8005B4EC) + 24, 0);
    sub_80022CC4();
    sub_80043DB4();
    return sub_80022A78(mode, call);
}

uint32_t sub_80023438(RRJRaceLeafCall call)
{
    uint32_t result;
    FUNCTION_MARKER(0x80023438, "SLUS_010.53");
    leaf_call(rrj_host_context(), call, 0x800457FC, 0, 0, 0, 0);
    leaf_call(rrj_host_context(), call, 0x80046614, 0, 0, 0, 0);
    rrj_write32(0x80010000, 0);
    result = sub_80014528(0x80052318, call);
    if (rrj_s32(result) < 0)
        result = leaf_call(rrj_host_context(), call, 0x80044894, 0x80010BB0, 0, 0, 0);
    rrj_write32(0x8005AD5C, 0);
    return result;
}

uint32_t sub_80022FC0(RRJRaceLeafCall call)
{
    uint32_t changed;
    uint32_t result;
    FUNCTION_MARKER(0x80022FC0, "SLUS_010.53");
    changed = sub_80022A1C(2, call);
    if (rrj_read32(rrj_read32(0x8005B2F8) + 0x30) == 1)
        sub_80024760(call);
    result = sub_800236C0(call);
    if (changed)
        return sub_80023438(call);
    return result;
}

uint32_t sub_80022A78(uint32_t mode, RRJRaceLeafCall call)
{
    uint32_t attempts;
    FUNCTION_MARKER(0x80022A78, "SLUS_010.53");
    if (mode == 1)
    {
        while (rrj_read32(0x80053464))
            ;
    }
    else if (mode == 2)
    {
        for (attempts = 0; attempts < 200 && rrj_read32(0x80053464); ++attempts)
            leaf_call(rrj_host_context(), call, 0x80047724, 0, 0, 0, 0);
    }
    return rrj_read32(0x80053464);
}

void sub_80022B0C(uint32_t busy, RRJRaceLeafCall call)
{
    uint32_t valid = 1;
    FUNCTION_MARKER(0x80022B0C, "SLUS_010.53");
    if (busy)
        return;
    for (;;)
    {
        uint32_t status = sub_80022E38(0x8005B4EC);
        uint32_t record = rrj_read32(0x8005B4EC);
        if (!status && rrj_read32(record) != 512)
        {
            valid = sub_80030868(rrj_read32(record + 12));
            if (!valid)
                leaf_call(rrj_host_context(), call, 0x80044894, 0x80010B94, 0, 0, 0);
        }
        if (status == 0xFFFFFF9D)
        {
            rrj_write32(0x80053464, 0);
            return;
        }
        if (!status && valid)
        {
            rrj_write32(0x80053464, 1);
            sub_80022BEC(record, call);
            return;
        }
    }
}

uint32_t sub_80022BEC(uint32_t record, RRJRaceLeafCall call)
{
    uint32_t result;
    FUNCTION_MARKER(0x80022BEC, "SLUS_010.53");
    sub_80014634(rrj_read32(record + 4), rrj_read32(record + 8), 0);
    rrj_write32(0x8005B4F0, rrj_read32(0x8005B4F0) + 1);
    rrj_write32(0x8005AE1C, rrj_read32(rrj_read32(0x8005B2F8) + 12));
    rrj_write32(0x8005AE20, 1);
    result = sub_80014894(rrj_read32(record + 4), rrj_read32(record + 16), rrj_read32(record + 28), 0x80022EEC, call);
    if (rrj_s32(result) < 0)
        rrj_write32(0x8005AE20, 0);
    return result;
}

void sub_80022CC4(void)
{
    FUNCTION_MARKER(0x80022CC4, "SLUS_010.53");
    rrj_write32(0x80053474, 0);
    rrj_write32(0x80053470, 0);
    rrj_write32(0x8005346C, 0);
    rrj_write32(0x80053468, 0);
}

uint32_t sub_80022CEC(void)
{
    uint32_t result;
    FUNCTION_MARKER(0x80022CEC, "SLUS_010.53");
    sub_80043DA4();
    result = rrj_read32(0x80053474);
    sub_80043DB4();
    return result;
}

uint32_t sub_80022D20(uint32_t record, RRJRaceLeafCall call)
{
    uint32_t write_index;
    uint32_t count;
    FUNCTION_MARKER(0x80022D20, "SLUS_010.53");
    if (rrj_read32(0x80053470) == 49)
        return 0xFFFFFF9C;
    write_index = rrj_read32(0x80053468);
    sub_8001E0B4(0x800D7740 + write_index * 32, record, 32);
    ++write_index;
    rrj_write32(0x80053468, write_index < 50 ? write_index : 0);
    sub_80043DA4();
    count = rrj_read32(0x80053470) + 1;
    rrj_write32(0x80053470, count);
    if (rrj_read32(record) == 8)
        rrj_write32(0x80053474, rrj_read32(0x80053474) + 1);
    sub_80043DB4();
    if (count == 1 && !rrj_read32(0x80053464))
        sub_80022B0C(0, call);
    return 0;
}

uint32_t sub_80022E38(uint32_t output)
{
    uint32_t read_index;
    uint32_t record;
    FUNCTION_MARKER(0x80022E38, "SLUS_010.53");
    if (!rrj_read32(0x80053470))
        return 0xFFFFFF9D;
    read_index = rrj_read32(0x8005346C);
    record = 0x800D7740 + read_index * 32;
    rrj_write32(output, record);
    ++read_index;
    rrj_write32(0x8005346C, read_index < 50 ? read_index : 0);
    rrj_write32(0x80053470, rrj_read32(0x80053470) - 1);
    if (rrj_read32(record) == 8)
        rrj_write32(0x80053474, rrj_read32(0x80053474) - 1);
    return 0;
}

uint32_t sub_80023148(uint32_t stream, uint32_t count, uint32_t duplicate, uint32_t options, RRJRaceLeafCall call)
{
    uint32_t outputs = 0x800D7D80;
    uint32_t job = 0x1F800300;
    uint32_t selected;
    uint32_t index;
    FUNCTION_MARKER(0x80023148, "SLUS_010.53");
    if (rrj_read32(stream + 20) & 2)
        return 0xFFFFFFFF;
    selected = sub_80030CD8(outputs, count, rrj_read32(stream + 8), duplicate, options, call);
    if (rrj_s32(selected) <= 0)
        return 1;
    for (index = 0; index < selected; ++index)
    {
        uint32_t record;
        if (sub_80023300(stream))
            break;
        record = rrj_read32(outputs + index * 4);
        rrj_write32(job, rrj_read32(stream + 8));
        rrj_write32(job + 4, rrj_read32(stream));
        rrj_write32(job + 8, rrj_read32(stream + 4));
        rrj_write32(job + 12, rrj_read32(record + 4));
        rrj_write32(job + 16, rrj_read32(record + 12));
        rrj_write32(job + 20, options);
        rrj_write32(job + 24, 0x80030894);
        rrj_write32(job + 28, 0x4000);
        if (sub_80022D20(job, call) == 0xFFFFFF9C)
            break;
        rrj_write32(stream + 4, rrj_read32(stream + 4) + 0x4000);
    }
    if (index < selected)
    {
        sub_80023358(outputs + index * 4, selected - index, call);
        return 0xFFFFFFFF;
    }
    return 1;
}

uint32_t sub_80023294(uint32_t record)
{
    FUNCTION_MARKER(0x80023294, "SLUS_010.53");
    return rrj_read32(record + 4);
}

uint32_t sub_800232A0(uint32_t record, uint32_t offset)
{
    uint32_t position;
    uint32_t limit;
    FUNCTION_MARKER(0x800232A0, "SLUS_010.53");
    position = offset + rrj_read32(record + 12);
    limit = rrj_read32(record + 16);
    if (position >= limit)
    {
        rrj_write32(record + 4, limit);
        sub_800233B4(record);
    }
    else
    {
        rrj_write32(record + 4, position);
        rrj_write32(record + 20, rrj_read32(record + 20) & ~2u);
    }
    return rrj_read32(record + 4);
}

uint32_t sub_80023300(uint32_t record)
{
    FUNCTION_MARKER(0x80023300, "SLUS_010.53");
    if (rrj_read32(record + 4) < rrj_read32(record + 16))
        sub_800233F4(record);
    else
        sub_800233B4(record);
    return rrj_read32(record + 20) & 2;
}

uint32_t sub_80023358(uint32_t records, uint32_t count, RRJRaceLeafCall call)
{
    uint32_t index;
    FUNCTION_MARKER(0x80023358, "SLUS_010.53");
    for (index = 0; rrj_s32(index) < rrj_s32(count); ++index)
    {
        uint32_t record = rrj_read32(records + index * 4);
        sub_80030FD0(rrj_read32(record + 4), call);
    }
    return index < count;
}

static uint32_t set_record_active(RRJMemory *m, uint32_t record, uint32_t active)
{
    uint32_t slot;
    uint32_t flags;
    flags = rrj_read32(record + 20);
    rrj_write32(record + 20, active ? flags | 2 : flags & ~2u);
    slot = 0x800D6510 + rrj_read32(record) * 24;
    rrj_write32(slot, rrj_read32(slot) & ~0x100u);
    return slot;
}

uint32_t sub_800233B4(uint32_t record)
{
    FUNCTION_MARKER(0x800233B4, "SLUS_010.53");
    return set_record_active(rrj_host_context(), record, 1);
}

uint32_t sub_800233F4(uint32_t record)
{
    FUNCTION_MARKER(0x800233F4, "SLUS_010.53");
    return set_record_active(rrj_host_context(), record, 0);
}

uint32_t sub_800236C0(RRJRaceLeafCall call)
{
    uint32_t pointer;
    FUNCTION_MARKER(0x800236C0, "SLUS_010.53");
    pointer = rrj_read32(0x8005B4F8);
    if (pointer)
        sub_800144B8(pointer);
    pointer = rrj_read32(0x8005AE30);
    if (pointer)
    {
        sub_800144B8(pointer);
        rrj_write32(0x8005AE30, 0);
        rrj_write32(0x8005B518, 0);
    }
    return sub_8001460C(rrj_read32(0x8005B4FC), call);
}

uint32_t sub_80024760(RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x80024760, "SLUS_010.53");
    sub_80020DB8(call);
    sub_8001460C(rrj_read32(0x800D6D88), call);
    return sub_8001460C(rrj_read32(0x800D6D8C), call);
}

uint32_t sub_800247A0(RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x800247A0, "SLUS_010.53");
    rrj_write32(0x800CD694, 2);
    rrj_write32(0x8005B524, (rrj_read32(0x800CD680) + rrj_read32(0x8005B510)) >> 1);
    return sub_80024A54(call);
}

uint32_t sub_800247E8(RRJRaceLeafCall call)
{
    uint32_t state = rrj_read32(0x800CD694);
    FUNCTION_MARKER(0x800247E8, "SLUS_010.53");
    if (state == 1)
    {
        rrj_write32(0x800CD694, 2);
        return sub_80024A54(call);
    }
    if (!state)
        return 2;
    if (state == 2)
    {
        uint32_t delay = rrj_read32(0x800D7588);
        if (delay)
        {
            if (rrj_s32(rrj_read32(0x8005B520)) > 0)
                return sub_80024E64(rrj_read32(0x8005B520), call);
            return delay;
        }
        w_u8(0x800D754A, 1);
        sub_80020DEC(call);
        rrj_write32(0x800CD694, 3);
        if (rrj_s32(rrj_read32(0x800D7F90)) >= 0)
            return sub_8001460C(rrj_read32(0x800D7F90), call);
        return 3;
    }
    if (state == 3)
    {
        uint32_t result = rrj_s32(sub_80025190()) < 4;
        if (result)
        {
            result = sub_800249B8();
            if (result)
                return sub_80024CB4(sub_800251D4(), call);
        }
        return result;
    }
    return 3;
}

uint32_t sub_800249B8(void)
{
    FUNCTION_MARKER(0x800249B8, "SLUS_010.53");
    rrj_write32(0x800CD6A4, 1);
    return 1;
}

uint32_t sub_80024A54(RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x80024A54, "SLUS_010.53");
    if (!sub_80024EF4(0xFFFFFFFF))
    {
        rrj_write32(0x800CD694, 0);
        return 0x800D0000;
    }
    sub_80024DA8(call);
    rrj_write32(0x8005B520, 8);
    return sub_80024E64(8, call);
}

uint32_t sub_80024B20(void)
{
    uint32_t base = 0x80053578;
    uint32_t count = rrj_read32(base);
    uint32_t active = 0;
    uint32_t available = 0;
    uint32_t index;
    uint32_t attempt;
    FUNCTION_MARKER(0x80024B20, "SLUS_010.53");
    for (index = 0; index < count; ++index)
    {
        uint32_t flags = rrj_read32(base + 16 + index * 12);
        if (flags & 1)
        {
            ++active;
            if (!(flags & 2))
                ++available;
        }
    }
    if (!active)
        return 0xFFFFFFFF;
    if (!available)
    {
        for (index = 0; index < count; ++index)
            rrj_write32(base + 16 + index * 12, rrj_read32(base + 16 + index * 12) & ~2u);
    }
    index = (count * ((uint32_t)sub_80043F00(0xF2000002) & 0xFF)) >> 8;
    for (attempt = 0; attempt < count; ++attempt)
    {
        uint32_t record = base + 16 + index * 12;
        uint32_t flags = rrj_read32(record);
        if ((flags & 1) && !(flags & 2))
        {
            rrj_write32(record, flags | 2);
            return index;
        }
        ++index;
        if (index >= count)
            index = 0;
    }
    return 0xFFFFFFFF;
}

uint32_t sub_80024CB4(uint32_t count, RRJRaceLeafCall call)
{
    uint32_t stream = 0x800CD670;
    uint32_t remaining;
    uint32_t chunks;
    FUNCTION_MARKER(0x80024CB4, "SLUS_010.53");
    if (rrj_read32(0x800CE5D4))
        return rrj_read32(0x800CE5D4);
    remaining = rrj_read32(0x800CD69C) - sub_80023294(stream);
    if (!remaining)
    {
        if (sub_80024EF4(0xFFFFFFFF))
            remaining = rrj_read32(0x800CD69C) - sub_80023294(stream);
        else
        {
            rrj_write32(0x800CD694, 0);
            sub_80020DB8(call);
        }
    }
    chunks = remaining >> 14;
    if (chunks >= count)
        chunks = count;
    if (rrj_read32(0x8005B514) >= rrj_read32(0x8005B524) + rrj_read32(0x800CD674))
        rrj_write32(stream, rrj_read32(0x800CD68C));
    else
        rrj_write32(stream, rrj_read32(0x800CD688));
    return sub_80023148(stream, chunks, 1, 0, call);
}

uint32_t sub_80024AE8(uint32_t index, uint32_t begin, uint32_t end)
{
    uint32_t record = 0x80053578 + index * 12;
    uint32_t first = rrj_read32(record + 8);
    uint32_t size = rrj_read32(record + 12);
    FUNCTION_MARKER(0x80024AE8, "SLUS_010.53");
    rrj_write32(begin, first);
    rrj_write32(end, first + size);
    return size;
}

uint32_t sub_80024DA8(RRJRaceLeafCall call)
{
    uint32_t state = rrj_read32(0x8005B2F8);
    uint32_t result;
    FUNCTION_MARKER(0x80024DA8, "SLUS_010.53");
    rrj_write32(0x800D7F90, 0xFFFFFFFF);
    result = r_u8(state + 4) & 1;
    if (!result)
    {
        int32_t delta;
        uint32_t index = rrj_read32(0x8005B1F0);
        result = sub_8001458C(0x80010C08, rrj_read32(0x8005AD5C), call);
        rrj_write32(0x800D7F90, result);
        if (rrj_s32(result) < 0)
            return result;
        rrj_write32(0x800D7F98, 8);
        rrj_write32(0x800D7F9C, 0);
        rrj_write32(0x800D7FA0, sub_800148BC(result, call));
        rrj_write32(0x800D7FA4, 1);
        rrj_write32(0x800D7F94, index << 17);
        delta = (int32_t)r_s8(0x8005365C + index) * 16384;
        rrj_write32(0x800CD674, rrj_read32(0x800CD674) + (uint32_t)delta);
        result = (uint32_t)delta;
    }
    return result;
}

uint32_t sub_80024E64(uint32_t count, RRJRaceLeafCall call)
{
    uint32_t stream = 0x800D7F90;
    uint32_t before;
    uint32_t consumed;
    FUNCTION_MARKER(0x80024E64, "SLUS_010.53");
    if (rrj_s32(rrj_read32(stream)) < 0 || (r_u8(rrj_read32(0x8005B2F8) + 4) & 1))
        stream = 0x800CD670;
    before = rrj_read32(stream + 4);
    sub_80023148(stream, count, 0, 0, call);
    consumed = (rrj_read32(stream + 4) - before) >> 14;
    rrj_write32(0x8005B520, rrj_read32(0x8005B520) - consumed);
    return rrj_read32(0x8005B520);
}

uint32_t sub_80024EF4(uint32_t index)
{
    uint32_t begin = 0x1F800330;
    uint32_t end = 0x1F800334;
    FUNCTION_MARKER(0x80024EF4, "SLUS_010.53");
    if (rrj_s32(index) < 0)
        index = sub_80024B20();
    rrj_write32(0x8005B1F0, index);
    if (rrj_s32(index) < 0 || index >= rrj_read32(0x80053578))
        return 0;
    sub_80024AE8(index, begin, end);
    rrj_write32(0x800CD69C, rrj_read32(end));
    sub_800232A0(0x800CD670, rrj_read32(begin));
    return 1;
}

uint32_t sub_80025190(void)
{
    uint32_t result;
    FUNCTION_MARKER(0x80025190, "SLUS_010.53");
    result = rrj_read32(0x800D7584) - rrj_read32(0x800D7588);
    return result + rrj_read32(0x800D7F88) + sub_80022CEC();
}

uint32_t sub_800251D4(void)
{
    FUNCTION_MARKER(0x800251D4, "SLUS_010.53");
    return rrj_read32(0x800D7588) + 2;
}

uint32_t sub_8002E080(RRJRaceLeafCall call)
{
    uint32_t table;
    uint32_t phase;
    FUNCTION_MARKER(0x8002E080, "SLUS_010.53");
    table = sub_8001447C(2048, 0, call);
    rrj_write32(0x8005B560, table);
    for (phase = 0; phase < 1024; ++phase)
    {
        uint32_t divisor = (phase >> 1) + ((phase - 2) >> 31);
        uint32_t value = sub_8004CF74(0x80000000u / divisor) << 2;
        uint32_t leading = value ? leaf_leading_sign(value) : 0;
        int32_t shift = 21 - (int32_t)leading;
        uint32_t packed;
        if (shift > 0)
            value >>= (uint32_t)shift;
        packed = (value << 5) | (uint32_t)shift;
        rrj_put16(rrj_at(table + phase * 2, 2), (uint16_t)packed);
    }
    return 0;
}

uint32_t sub_80030868(uint32_t index)
{
    uint32_t base;
    FUNCTION_MARKER(0x80030868, "SLUS_010.53");
    base = rrj_read32(0x8005ACBC);
    return (rrj_read32(base + index * 36 + 44) ^ 1) & 1;
}

uint32_t sub_80030FD0(uint32_t index, RRJRaceLeafCall call)
{
    uint32_t record;
    FUNCTION_MARKER(0x80030FD0, "SLUS_010.53");
    record = rrj_read32(0x8005ACBC) + index * 36 + 44;
    return leaf_call(rrj_host_context(), call, 0x800313EC, record, 0, 0, 0);
}

uint32_t sub_8003E1E8(uint32_t object, uint32_t id, uint32_t direction_out)
{
    uint32_t header = 0;
    uint32_t link;
    uint32_t route;
    uint32_t entry;
    int32_t offset;
    FUNCTION_MARKER(0x8003E1E8, "SLUS_010.53");
    if (object && r_s16(object + 16) == 1 && !rrj_read32(object + 12))
        header = sub_80039AFC(rrj_read32(object));
    if (!header)
        return 0;
    link = sub_8003A37C(header, id);
    if (!link)
        return 0;
    route = sub_80039C90(object, id);
    if (!route)
        return 0;
    entry = rrj_read32(object + 48) + 28 * (uint32_t)(int32_t)r_s16(route + 20);
    if (!entry)
        return 0;
    if (r_s16(link + 2) > 0)
        offset = (int32_t)r_s16(entry + 18) << 4;
    else
        offset = ((int32_t)r_s16(entry + 18) + (int32_t)r_s16(entry + 16)) * 16 - 16;
    entry = rrj_read32(object + 56) + (uint32_t)offset;
    if (r_s16(entry + 4) < 0)
        return 0;
    rrj_write32(direction_out, (uint32_t)(int32_t)r_s16(link + 2));
    return rrj_read32(object + 60) + 264 * (uint32_t)(int32_t)r_s16(entry + 4);
}

uint32_t sub_80030CD8(uint32_t outputs, uint32_t count, uint32_t kind, uint32_t duplicate, uint32_t options, RRJRaceLeafCall call)
{
    uint32_t category = (options >> 16) & 0xFF;
    uint32_t selected = 0;
    uint32_t index;
    FUNCTION_MARKER(0x80030CD8, "SLUS_010.53");
    if (!count)
        return 0;
    if (kind == 8)
    {
        selected = sub_80031064(outputs, count);
        if (!duplicate)
            selected += sub_80031170(outputs + selected * 4, count - selected, 0, category);
    }
    else
    {
        selected = sub_80031170(outputs, count, 0, category);
        if (duplicate)
            selected += sub_80031064(outputs + selected * 4, count - selected);
    }
    for (index = 0; index < selected; ++index)
        sub_80031368(rrj_read32(outputs + index * 4), kind, call);
    if (!duplicate || selected < 2 || selected >= count)
        return selected;
    for (index = 0; index < count - selected; ++index)
    {
        uint32_t record = rrj_read32(outputs + index * 4);
        rrj_write32(outputs + (selected + index) * 4, record);
        rrj_write32(record, rrj_read32(record) + 0x1000);
        rrj_write32(rrj_read32(0x8005ACBC) + 28, rrj_read32(rrj_read32(0x8005ACBC) + 28) + 1);
    }
    return count;
}

uint32_t sub_80031064(uint32_t output, uint32_t count)
{
    uint32_t base = rrj_read32(0x8005ACBC);
    uint32_t current = rrj_read32(base + 0xA50);
    uint32_t end = rrj_read32(base + 0xA54);
    uint32_t found = 0;
    uint32_t record;
    uint32_t locked = !rrj_read32(0x800541D0);
    FUNCTION_MARKER(0x80031064, "SLUS_010.53");
    if (locked)
        sub_80043DA4();
    record = base + current * 36 + 44;
    while (current <= end && rrj_s32(found) < rrj_s32(count))
    {
        if (!rrj_read32(record))
        {
            rrj_write32(output, record);
            output += 4;
            ++found;
            rrj_write32(record, 0x100);
            (uint32_t)sub_80032190(0, 3);
        }
        ++current;
        record += 36;
    }
    if (locked)
        sub_80043DB4();
    return found;
}

uint32_t sub_80031170(uint32_t outputs, uint32_t count, uint32_t bypass_reserved, uint32_t kind)
{
    uint32_t base = rrj_read32(0x8005ACBC);
    uint32_t first = rrj_read32(base + 0xA40 + kind * 8);
    uint32_t last = rrj_read32(base + 0xA44 + kind * 8);
    uint32_t selected = 0;
    uint32_t skipped = 0;
    uint32_t index;
    uint32_t locked = !rrj_read32(0x800541D0);
    FUNCTION_MARKER(0x80031170, "SLUS_010.53");
    if (locked)
        sub_80043DA4();
    for (index = first; index <= last && rrj_s32(selected) < rrj_s32(count); ++index)
    {
        uint32_t record = base + 44 + index * 36;
        if (!rrj_read32(record))
        {
            if (bypass_reserved || rrj_s32(skipped) >= rrj_s32(rrj_read32(base + 0xA5C)))
            {
                rrj_write32(outputs + selected * 4, record);
                ++selected;
                rrj_write32(record, 256);
                (uint32_t)sub_80032190(0, 3);
            }
            else
                ++skipped;
        }
        if (index == 0xFFFFFFFF)
            break;
    }
    if (locked)
        sub_80043DB4();
    return selected;
}

uint32_t sub_80031368(uint32_t record, uint32_t kind, RRJRaceLeafCall call)
{
    uint32_t base;
    uint32_t result = 16;
    FUNCTION_MARKER(0x80031368, "SLUS_010.53");
    rrj_write32(record, kind | 4);
    leaf_call(rrj_host_context(), call, 0x800322D0, 3, 2, 0, 0);
    base = rrj_read32(0x8005ACBC);
    if (kind == 8)
    {
        result = rrj_read32(base + 28) + 1;
        rrj_write32(base + 28, result);
    }
    else if (kind == 16)
    {
        result = rrj_read32(base + 24) + 1;
        rrj_write32(base + 24, result);
    }
    return result;
}

void sub_80043DA4(void)
{
    FUNCTION_MARKER(0x80043DA4u, "SLUS_010.53");
    rrj_host_context()->cpu_status &= ~0x401u;
}

void sub_80043DB4(void)
{
    FUNCTION_MARKER(0x80043DB4u, "SLUS_010.53");
    rrj_host_context()->cpu_status |= 0x401u;
}

void sub_80043DD4(void)
{
    FUNCTION_MARKER(0x80043DD4, "SLUS_010.53");
}

void sub_80043DF4(void)
{
    FUNCTION_MARKER(0x80043DF4, "SLUS_010.53");
}

uint32_t sub_80048414(uint32_t value)
{
    uint32_t previous;
    FUNCTION_MARKER(0x80048414, "SLUS_010.53");
    previous = rrj_read32(0x80055F0C);
    rrj_write32(0x80055F0C, value);
    return previous;
}

uint32_t sub_8004DE44(uint32_t handle, RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x8004DE44, "SLUS_010.53");
    return leaf_call(rrj_host_context(), call, RRJ_RACE_BREAK_CLOSE, handle, handle, 0, 0);
}

uint32_t sub_8004DE78(uint32_t handle, uint32_t buffer, uint32_t size, RRJRaceLeafCall call)
{
    uint32_t total = 0;
    FUNCTION_MARKER(0x8004DE78, "SLUS_010.53");
    while (size)
    {
        uint32_t chunk = size > 0x8000 ? 0x8000 : size;
        uint32_t result = sub_8004DF38(0, handle, chunk, buffer, call);
        total += result;
        if (result == 0xFFFFFFFF)
            return 0xFFFFFFFF;
        buffer += result;
        size -= result;
        if (rrj_s32(result) < rrj_s32(chunk))
            return total;
    }
    return total;
}

uint32_t sub_8004DF38(uint32_t zero, uint32_t handle, uint32_t size, uint32_t buffer, RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x8004DF38, "SLUS_010.53");
    return leaf_call(rrj_host_context(), call, RRJ_RACE_BREAK_READ, zero, handle, size, buffer);
}

uint32_t sub_8004F2C8(RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x8004F2C8, "SLUS_010.53");
    return leaf_call(rrj_host_context(), call, 0x8004E5F8, 0, 0, 0, 0);
}

void sub_8004F2E8(RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x8004F2E8, "SLUS_010.53");
    if (rrj_read32(0x8005A40C) == 1)
    {
        uint32_t context;
        rrj_write32(0x8005A40C, 0);
        sub_80043DA4();
        rrj_write32(0x8005A454, 0);
        rrj_write32(0x8005A458, 0);
        leaf_call(rrj_host_context(), call, 0x8004F298, 0, 0, 0, 0);
        context = rrj_read32(0x8005A3A4);
        leaf_call(rrj_host_context(), call, 0x80043D74, context, 0, 0, 0);
        leaf_call(rrj_host_context(), call, 0x80043D94, context, 0, 0, 0);
        sub_80043DB4();
    }
}

uint32_t sub_80051018(void)
{
    uint32_t state;
    FUNCTION_MARKER(0x80051018, "SLUS_010.53");
    state = rrj_read32(0x8005A438);
    state = state == 1 ? state : 0;
    rrj_write32(0x8005A3AC, state);
    return state;
}

uint32_t sub_80051058(uint32_t callback)
{
    uint32_t previous;
    FUNCTION_MARKER(0x80051058, "SLUS_010.53");
    previous = rrj_read32(0x8005A454);
    if (callback != previous)
        rrj_write32(0x8005A454, callback);
    return previous;
}

uint32_t sub_8009277C(uint32_t object, RRJReverbCall reverb)
{
    FUNCTION_MARKER(0x8009277C, "RASHCDG.BIN");
    return sub_80092784(object, reverb);
}

uint32_t sub_80092784(uint32_t object, RRJReverbCall reverb)
{
    uint32_t state = rrj_read32(0x8005B2F8);
    uint32_t secondary = rrj_read32(object + 852);
    uint32_t animation = rrj_read32(object + 540);
    uint32_t options = 0x10;
    uint32_t record;
    uint32_t i;
    FUNCTION_MARKER(0x80092784, "RASHCDG.BIN");
    if (r_u8(state + 57) >= 2 && r_s8(secondary + 571) != -1)
        (void)sub_80095AEC(secondary);
    if (rrj_read32(0x8005B254) && ((uint32_t)rrj_u16(rrj_at(object + 172, 2)) < rrj_read32(state + 48) || (rrj_u16(rrj_at(object + 320, 2)) & 4)) && rrj_read32(0x800CE178) == rrj_read32(0x800CE17C))
        (void)sub_8008CC94();
    if (!animation)
    {
        animation = sub_80012884(0x800CE170, object);
        rrj_write32(object + 540, animation);
        if (!animation)
            return sub_800903F4(object, 0, reverb);
    }
    record = object + 948 + 8 * (int32_t)r_s8(object + 946);
    rrj_put16(rrj_at(record, 2), 1);
    rrj_put16(rrj_at(record + 2, 2), 224);
    rrj_put16(rrj_at(record + 6, 2), (uint16_t)((((uint32_t)(2180u * rrj_read32(state + 16) + 0x8000u)) >> 16) | 0xC000u));
    rrj_write32(secondary + 552, (rrj_read32(secondary + 552) | 0x40u) & 0xBFE67FC7u);
    rrj_write32(object + 720, 0);
    (void)sub_80012838(object, secondary, 2, 0);
    w_u8(object + 72, 3);
    if (rrj_read32(object + 856) && rrj_read32(object + 1088))
    {
        uint32_t linked = rrj_read32(object + 856);
        (void)sub_800C4550(4, secondary, options);
        if ((r_u8(secondary + 572) & 0x10) && r_u8(state + 57) != 1)
        {
            uint32_t linked_secondary = rrj_read32(linked + 852);
            rrj_put16(rrj_at(linked_secondary + 320, 2), rrj_u16(rrj_at(object + 320, 2)));
            (void)sub_80012838(object, linked_secondary, 2, 1);
            w_u8(object + 72, 3);
            rrj_write32(linked_secondary + 552, (rrj_read32(linked_secondary + 552) | 0x40u) & 0xBFE67FE7u);
            (void)sub_800C4550(77, linked_secondary, options);
        }
        rrj_write32(linked + 720, 0);
        (void)sub_80092AD4(object);
    }
    else
    {
        int32_t velocity = rrj_s32(rrj_read32(object + 652));
        (void)sub_80012858(animation, rrj_read32(0x800CE1A0));
        if ((int16_t)rrj_u16(rrj_at(object + 320, 2)) != 0 && velocity < 0)
            options |= 0x100;
        if (leaf_abs32(velocity) != 102943u)
            options |= 0x8000;
        (void)sub_800C4550(0, secondary, options);
    }
    for (i = 0; i < 9; ++i)
        rrj_put16(rrj_at(object + 432 + 2 * i, 2), rrj_u16(rrj_at(object + 516 + 2 * i, 2)));
    {
        uint32_t id = rrj_u16(rrj_at(object + 172, 2));
        if (id < rrj_read32(state + 48))
        {
            uint32_t player = 0x800CD898 + 1132 * id;
            (void)sub_80018440(id, 0);
            rrj_write32(player + 548, rrj_read32(player + 548) & 0xF5FFFF7Fu);
            return sub_800235B0(id, object);
        }
    }
    return 0;
}

uint32_t sub_800BF978(uint32_t actor)
{
    uint32_t descriptor = rrj_read32(actor + 1084);
    uint32_t secondary = rrj_read32(actor + 852);
    uint32_t kind = r_u8(descriptor + 60);
    uint32_t variant = kind & 0x1F;
    uint32_t state = 0;
    uint32_t category;
    FUNCTION_MARKER(0x800BF978, "RASHCDG.BIN");
    if (kind & 0x40)
    {
        if (kind >= 0x47 && kind <= 0x4A)
            state = 10;
        else if (kind >= 0x4B && kind <= 0x4C)
            state = 11;
        else
        {
            state = 17;
            if (kind == 0x4D)
                variant = 23;
        }
    }
    else if (kind & 0x20)
    {
        if (kind <= 0x23)
            state = 0;
        else if (kind <= 0x25)
            state = 1;
        else
        {
            state = 16;
            if (kind == 0x26)
                variant = 23;
        }
    }
    else
    {
        category = r_u8(descriptor + 46);
        if (category == 0)
        {
            state = 6;
            if ((uint8_t)(kind + 0x72) < 4)
            {
                if (kind == 0x8F)
                    variant = 22;
            }
            else if ((uint8_t)(kind + 0x6E) < 2)
                state = 7;
            else
            {
                state = 14;
                variant = 23;
            }
        }
        else if (category == 2 || category == 3)
        {
            state = 2;
            if ((uint8_t)(kind + 0x72) < 4)
            {
                if (kind == 0x8F)
                    variant = 22;
            }
            else if ((uint8_t)(kind + 0x6E) < 2)
                state = 3;
            else
            {
                state = 12;
                variant = 23;
            }
        }
        else if (category == 1 || category == 5)
        {
            state = 4;
            if ((uint8_t)(kind + 0x72) < 4)
            {
                if (kind == 0x8F)
                    variant = 22;
            }
            else if ((uint8_t)(kind + 0x6E) < 2)
                state = 5;
            else
            {
                state = 13;
                variant = 23;
            }
        }
        else if (category == 4)
        {
            if ((uint8_t)(kind + 0x72) < 4)
                state = 8;
            else if ((uint8_t)(kind + 0x6E) < 2)
                state = 9;
            else
                state = 15;
        }
        else if (category == 6 || category == 7)
        {
            state = 18;
            variant = 25 - (variant & 1);
        }
        else if (category == 8)
        {
            state = 19;
            variant = 25 - (variant & 1);
        }
    }
    if (r_u8(secondary + 572) & 0x20)
        state = state == 19 ? 32 : state + 20;
    w_u8(secondary + 569, (uint8_t)state);
    w_u8(secondary + 570, 0);
    if (!(rrj_read32(actor + 560) & 0x08000000u) && !(r_u8(secondary + 572) & 0x40))
    {
        w_u8(secondary + 574, 0);
        xport_guest_fill(secondary + 575, 0x3F, 5);
    }
    else
        (void)sub_8001E08C(secondary + 574, 0x800CCAD0 + 6 * variant, 6);
    return state & 0xFF;
}

uint32_t sub_80099710(uint32_t actor, uint32_t first, uint32_t second, uint32_t scale, RRJReverbCall reverb)
{
    uint32_t product = (uint32_t)sub_80093BE0(actor + 528, first, second) * 25736u;
    int32_t step = rrj_s32(product) >> 8;
    int32_t threshold = (int32_t)sub_8001FC90(0x40000, rrj_s32(scale));
    uint32_t result;
    FUNCTION_MARKER(0x80099710, "RASHCDG.BIN");
    if (threshold < 3431)
        threshold = 3431;
    if (leaf_abs32(step) < (uint32_t)threshold)
        return sub_8009277C(rrj_read32(actor + 596), reverb);
    if (rrj_s32(rrj_read32(actor + 600)) > 0)
    {
        if (step < 0)
            step = rrj_s32((uint32_t)step + 0x6487Eu);
        rrj_write32(actor + 488, 0xFFFC0000u);
    }
    else
    {
        if (step < 0)
            step = -step;
        else
            step = 0x6487E - step;
        rrj_write32(actor + 488, 0x40000);
    }
    if (!rrj_read32(actor + 480))
    {
        int32_t delta = rrj_s32(rrj_read32(actor + 592) - rrj_read32(actor + 600));
        uint32_t numerator = delta > 0 ? (uint32_t)delta : (uint32_t)-delta;
        uint32_t denominator = step > 0 ? (uint32_t)step : (uint32_t)-step;
        result = sub_80010028(numerator, denominator);
        if ((delta > 0 && step <= 0) || (delta <= 0 && step > 0))
            result = 0u - result;
        rrj_write32(actor + 592, result);
        rrj_write32(actor + 480, 0x80000);
    }
    result = (uint32_t)sub_8001FC90(rrj_s32(rrj_read32(actor + 592)), step);
    rrj_write32(actor + 484, rrj_read32(actor + 600) + result);
    return result;
}

uint32_t sub_800C12D8(uint32_t actor, uint32_t flags)
{
    uint32_t secondary = rrj_read32(actor + 852);
    uint32_t state;
    uint32_t table;
    int32_t index;
    uint32_t event;
    FUNCTION_MARKER(0x800C12D8, "RASHCDG.BIN");
    xport_update_u8(secondary + 572, XPORT_MEMORY_UPDATE_AND, 0xFDu);
    (void)sub_800BF978(actor);
    index = r_s8(secondary + 574 + r_u8(secondary + 570));
    state = r_u8(secondary + 569);
    rrj_write32(secondary + 560, 0);
    rrj_write32(secondary + 564, 0);
    w_u8(secondary + 568, 0);
    table = rrj_read32(rrj_read32(0x8005AD4C) + 12 * state + 8);
    event = rrj_u16(rrj_at(table + 12 * (uint32_t)index, 2));
    return sub_800C4550(event, secondary, flags | 0x10u);
}

uint32_t sub_800C110C(uint32_t actor, uint32_t other)
{
    uint32_t secondary = rrj_read32(actor + 852);
    uint32_t linked = rrj_read32(actor + 856);
    uint32_t flags = 0;
    FUNCTION_MARKER(0x800C110C, "RASHCDG.BIN");
    xport_update_u8(secondary + 572, XPORT_MEMORY_UPDATE_AND, 0xFEu);
    if (!other && !linked)
        flags = (sub_8001FC58() & 1) << 8;
    else if (!rrj_read32(actor + 1088))
    {
        if (other == linked)
            flags = 0x100;
    }
    else if (linked)
    {
        if (other != linked)
            flags = 0x100;
    }
    else
    {
        uint32_t identity = rrj_read32(other + 360);
        uint32_t actor_identity = rrj_read32(actor + 360);
        if (identity != actor_identity || ((identity >> 16) && rrj_read32(other + 336) != rrj_read32(actor + 336)))
            (void)sub_800B6AAC(rrj_at(other + 184, 12), rrj_at(actor + 432, 6), rrj_at(actor + 184, 12));
        if (rrj_s32(sub_800B6AAC(rrj_at(other + 184, 12), rrj_at(actor + 432, 6), rrj_at(actor + 184, 12))) < 0)
            flags = 0x100;
    }
    return sub_800C12D8(actor, flags);
}

uint32_t sub_800C122C(uint32_t actor, uint32_t unused, uint32_t flags)
{
    uint32_t descriptor = rrj_read32(actor + 1084);
    FUNCTION_MARKER(0x800C122C, "RASHCDG.BIN");
    if ((r_u8(descriptor + 61) & 3) == 2)
    {
        (void)sub_800B92C0(descriptor);
        descriptor = rrj_read32(actor + 1084);
    }
    if ((r_u8(descriptor + 60) & 0x80) && (!r_u8(descriptor + 46) || r_u8(descriptor + 46) == 4) && rrj_read32(0x800CE178) == rrj_read32(0x800CE17C))
        w_u8(descriptor + 60, 0x20);
    return sub_800C12D8(actor, flags);
}

uint32_t sub_8003B96C(uint32_t actor)
{
    FUNCTION_MARKER(0x8003B96C, "SLUS_010.53");
    if (sub_8003B8F4(actor + 172))
        return rrj_read32(actor + 324);
    return 0x7FFFF000;
}

uint32_t sub_800138E8(uint32_t actor, uint32_t mode)
{
    uint32_t state = rrj_read32(0x8005B2F8);
    uint32_t descriptor = rrj_read32(actor + 1084);
    uint32_t players = rrj_read32(state + 48);
    uint32_t actor_id = rrj_u16(rrj_at(actor + 172, 2));
    uint32_t count = 1;
    uint32_t score;
    uint32_t object;
    int32_t index;
    FUNCTION_MARKER(0x800138E8, "SLUS_010.53");
    if ((r_u8(descriptor + 1) & 0x0F) == 2 && r_u8(descriptor + 39) < 0xF7 && (actor_id >= players || ((r_u8(state + 4) & 1) && !(rrj_read32(0x8005AD48) & 1))))
        return rrj_read32(0x8005B1F8) + 1;
    if (rrj_read32(descriptor + 40) || r_u8(descriptor + 39) >= 0xF8)
        return r_u8(descriptor + 39);
    score = sub_8003B96C(actor);
    index = rrj_s32(rrj_read32(rrj_read32(0x800CE4DC)));
    object = rrj_read32(0x800CE4D0);
    while (index >= 0)
    {
        uint32_t other_descriptor = rrj_read32(object + 1084);
        if ((actor_id < players || (r_u8(other_descriptor + 1) & 0x0F) != 2) && rrj_u16(rrj_at(object + 172, 2)) != actor_id && rrj_read32(0x8005B1F8) >= r_u8(other_descriptor + 39))
        {
            if (mode == 1)
                count += rrj_s32(rrj_read32(other_descriptor + 40)) > 0;
            else
            {
                uint32_t other_score = sub_8003B96C(object);
                count += rrj_s32(rrj_read32(other_descriptor + 40)) > 0 || rrj_s32(other_score) < rrj_s32(score);
            }
        }
        --index;
        object += rrj_read32(0x800CE4D4);
    }
    return count;
}

uint32_t sub_800BAA2C(uint32_t actor, uint32_t record, uint32_t delta, RRJReverbCall reverb)
{
    uint32_t state = rrj_read32(0x8005B2F8);
    uint32_t actor_id = rrj_u16(rrj_at(actor + 172, 2));
    uint32_t players = rrj_read32(state + 48);
    uint32_t result = 0;
    uint32_t selected = 0;
    uint8_t mode = r_u8(state + 4);
    FUNCTION_MARKER(0x800BAA2C, "RASHCDG.BIN");
    if (mode & 1)
    {
        if (actor_id < players)
        {
            uint32_t descriptor = rrj_read32(actor + 1084);
            if ((r_u8(descriptor + 1) & 0x0F) == 2)
            {
                uint32_t flags = rrj_read32(0x8005AD48);
                if (flags & 1)
                {
                    uint32_t active = 0;
                    uint32_t object = rrj_read32(0x800CE4D0);
                    int32_t index = rrj_s32(rrj_read32(rrj_read32(0x800CE4DC)));
                    uint32_t rank = sub_800138E8(actor, 0);
                    while (index >= 0)
                    {
                        uint32_t other_secondary = rrj_read32(object + 852);
                        uint32_t kind = rrj_read32(other_secondary + 604) - 3;
                        if (rrj_u16(rrj_at(object + 320, 2)) && kind < 2)
                            ++active;
                        --index;
                        object += rrj_read32(0x800CE4D4);
                    }
                    result = rrj_s32(rank) < rrj_s32(rrj_read32(0x8005B1F8) - (active + 1));
                    if (!result)
                        selected = 1;
                }
                else if (flags & 2)
                {
                    uint32_t player = 0x800D6198 + 224 * actor_id;
                    rrj_write32(0x8005B290, 0);
                    rrj_write32(player + 116, rrj_read32(state + 16));
                    rrj_write32(0x8005AD48, (flags & 0xFFFFFFFDu) | 4);
                    rrj_write32(player + 108, 0);
                    result = player;
                }
                else if (flags & 4)
                {
                    uint32_t stopped;
                    rrj_write32(0x8005B290, rrj_read32(0x8005B290) + delta);
                    rrj_write32(0x1F8003F0, 0);
                    result = (uint32_t)sub_8009DA4C(actor, 0x1F8003F0);
                    stopped = rrj_read32(0x1F8003F0);
                    if (rrj_s32(result) < 0 || rrj_s32(rrj_read32(0x800530E4 + 4 * rrj_read32(state + 60))) < rrj_s32(rrj_read32(0x8005B290)) || stopped)
                    {
                        result = (uint8_t)(r_u8(descriptor + 39) + 4) < 2;
                        if (!result)
                        {
                            result = sub_800A0708(actor, reverb);
                            selected = 1;
                        }
                    }
                }
            }
        }
    }
    else if (mode == 44 && actor_id >= players)
    {
        uint32_t primary = rrj_read32(0x8005B38C);
        int32_t distance = rrj_s32(rrj_read32(primary + 368) - (rrj_u16(rrj_at(0x80053182, 2)) << 16));
        uint32_t magnitude = leaf_abs32(distance);
        if (rrj_read32(actor + 180) < 18)
        {
            if (r_u8(state + 57) == 4 && rrj_s32((uint32_t)(int32_t)r_s8(0x8005ADF0) << 16) < rrj_s32(magnitude))
                selected = 1;
        }
        else
        {
            uint32_t race_state = r_u8(state + 57);
            if (race_state == 1 || race_state == 3)
                selected = 1;
        }
    }
    else
    {
        uint32_t threshold = rrj_u16(rrj_at(record + 4, 2)) << 16;
        selected = rrj_s32(27904u * rrj_read32(state + 16)) >= rrj_s32(threshold);
        result = selected;
    }
    if (selected)
    {
        uint32_t value;
        if (r_s8(actor + 946) >= 2)
            record = actor + 956;
        rrj_put16(rrj_at(record, 2), 4);
        rrj_put16(rrj_at(record + 4, 2), 0);
        rrj_put16(rrj_at(record + 6, 2), 0);
        value = ((2180u * rrj_read32(state + 16) + 0x8000u) >> 16) | 0xC000u;
        rrj_put16(rrj_at(record + 6, 2), (uint16_t)value);
        rrj_write32(actor + 720, 0);
        result = value;
    }
    return result;
}

uint32_t sub_80098F2C(uint32_t actor, uint32_t direction, uint32_t source, uint32_t magnitude, uint32_t fixed_result)
{
    int32_t alignment = rrj_s32(sub_8002E698(rrj_at(actor + 516, 6), rrj_at(direction, 6)));
    int32_t delta[3];
    int32_t scale;
    int32_t sum;
    uint32_t first;
    uint32_t second;
    uint32_t result;
    uint32_t scratch = 0x1F8003E0;
    FUNCTION_MARKER(0x80098F2C, "RASHCDG.BIN");
    delta[0] = rrj_s32(rrj_read32(actor + 184) - rrj_read32(actor + 580));
    delta[1] = rrj_s32(rrj_read32(actor + 188) - rrj_read32(actor + 584));
    delta[2] = rrj_s32(rrj_read32(actor + 192) - rrj_read32(actor + 588));
    if (alignment < 655)
    {
        if (alignment > 0)
            alignment = 655;
        else if (alignment >= -654)
            alignment = -655;
    }
    if (alignment >= 0)
        scale = rrj_s32(0x80000000u / (uint32_t)(alignment >> 1));
    else
        scale = rrj_s32(0u - 0x80000000u / ((uint32_t)(-alignment) >> 1));
    sum = leaf_product_asr16(delta[0], (int32_t)r_s16(direction) * 16);
    sum = rrj_s32((uint32_t)sum + (uint32_t)leaf_product_asr16(delta[1], (int32_t)r_s16(direction + 2) * 16));
    sum = rrj_s32((uint32_t)sum + (uint32_t)leaf_product_asr16(delta[2], (int32_t)r_s16(direction + 4) * 16));
    first = (uint32_t)sub_8001FC90(sum, scale);
    rrj_write32(actor + 592, first);
    sum = leaf_product_asr16(delta[0], (int32_t)r_s16(actor + 528) * 16);
    sum = rrj_s32((uint32_t)sum + (uint32_t)leaf_product_asr16(delta[1], (int32_t)r_s16(actor + 530) * 16));
    sum = rrj_s32((uint32_t)sum + (uint32_t)leaf_product_asr16(delta[2], (int32_t)r_s16(actor + 532) * 16));
    second = (uint32_t)sub_8001FC90(sum, scale);
    rrj_write32(actor + 600, second);
    if (rrj_s32(first ^ second) < 0)
    {
        uint32_t flags = rrj_read32(actor + 552);
        rrj_write32(actor + 552, flags | (scale >= 0 ? 0x500u : 0x300u));
        result = fixed_result ? 0x7FFF0000u : sub_8004CF74(magnitude) << 2;
        rrj_write32(actor + 484, result);
        return result;
    }
    (void)sub_8002EAD8(rrj_at(actor + 580, 12), rrj_at(source, 6), 0u - second, rrj_at(actor + 456, 12));
    (void)sub_8002EAD8(rrj_at(actor + 456, 12), rrj_at(actor + 516, 6), first, rrj_at(scratch, 12));
    rrj_write32(scratch, rrj_read32(scratch) - rrj_read32(actor + 184));
    rrj_write32(scratch + 4, rrj_read32(scratch + 4) - rrj_read32(actor + 188));
    rrj_write32(scratch + 8, rrj_read32(scratch + 8) - rrj_read32(actor + 192));
    (void)sub_8002EAD8(rrj_at(actor + 456, 12), rrj_at(source, 6), second, rrj_at(scratch, 12));
    rrj_write32(scratch, rrj_read32(scratch) - rrj_read32(actor + 580));
    rrj_write32(scratch + 4, rrj_read32(scratch + 4) - rrj_read32(actor + 584));
    rrj_write32(scratch + 8, rrj_read32(scratch + 8) - rrj_read32(actor + 588));
    rrj_write32(actor + 480, 0);
    result = (rrj_read32(actor + 552) & 0xFFFFFFE7u) | 0x10u;
    rrj_write32(actor + 552, result);
    return result;
}

uint32_t sub_80098A50(uint32_t actor, uint32_t source, uint32_t axis_out, uint32_t direction_out, RRJReverbCall reverb)
{
    uint32_t source_value = rrj_read32(source + 652);
    uint32_t index;
    uint32_t scale = 0x10000;
    FUNCTION_MARKER(0x80098A50, "RASHCDG.BIN");
    if (leaf_abs32(rrj_s32(source_value)) == 102943)
    {
        for (index = 0; index < 3; ++index)
        {
            rrj_put16(rrj_at(axis_out + 2 * index, 2), rrj_u16(rrj_at(source + 438 + 2 * index, 2)));
            rrj_put16(rrj_at(direction_out + 2 * index, 2), rrj_u16(rrj_at(source + 444 + 2 * index, 2)));
        }
    }
    else
    {
        for (index = 0; index < 3; ++index)
        {
            uint16_t axis = rrj_u16(rrj_at(source + 516 + 2 * index, 2));
            if (rrj_s32(source_value) > 0)
                axis = (uint16_t)(0u - axis);
            rrj_put16(rrj_at(axis_out + 2 * index, 2), axis);
            rrj_put16(rrj_at(direction_out + 2 * index, 2), rrj_u16(rrj_at(source + 528 + 2 * index, 2)));
        }
    }
    (void)sub_8002EAD8(rrj_at(source + 504, 12), rrj_at(axis_out, 6), 0xFFFEE000, rrj_at(actor + 580, 12));
    (void)sub_8002EAD8(rrj_at(actor + 580, 12), rrj_at(direction_out, 6), 0xFFFFF7FD, rrj_at(actor + 580, 12));
    if (rrj_s32(source_value) < 0)
    {
        for (index = 0; index < 3; ++index)
            rrj_put16(rrj_at(direction_out + 2 * index, 2), (uint16_t)(0u - rrj_u16(rrj_at(direction_out + 2 * index, 2))));
    }
    if (rrj_read32(actor + 552) & 0x18)
        return 1;
    if (!rrj_read32(actor + 480))
    {
        int32_t dx = rrj_s32(rrj_read32(actor + 184) - rrj_read32(actor + 580));
        int32_t dz = rrj_s32(rrj_read32(actor + 192) - rrj_read32(actor + 588));
        int32_t sign_x = dx >> 31;
        int32_t sign_z = dz >> 31;
        int32_t sx = rrj_s32((uint32_t)(dx >> 16) + (uint32_t)sign_x) ^ sign_x;
        int32_t sz = rrj_s32((uint32_t)(dz >> 16) + (uint32_t)sign_z) ^ sign_z;
        int32_t maximum = sx > sz ? sx : sz;
        int32_t minimum = sx > sz ? sz : sx;
        int32_t mixed = minimum + (minimum >> 1);
        int32_t metric = maximum - (maximum >> 5) - (maximum >> 7) + (mixed >> 2) + (mixed >> 6);
        if (metric < 6)
        {
            uint32_t delta = 0x1F800340;
            rrj_write32(delta, (uint32_t)dx);
            rrj_write32(delta + 4, rrj_read32(actor + 188) - rrj_read32(actor + 584));
            rrj_write32(delta + 8, (uint32_t)dz);
            scale = sub_8002E548(delta) >> 1;
            if (scale > 0x10000)
                scale = 0x10000;
            if (scale < 0x1999)
            {
                (void)sub_8009277C(source, reverb);
                return 0;
            }
        }
    }
    if (!(rrj_read32(actor + 552) & 0x800))
    {
        uint32_t delta = 0x1F800340;
        uint32_t normalized = 0x1F800350;
        uint32_t matrix = 0x1F800360;
        uint32_t rotated = 0x1F800380;
        int32_t first = (int32_t)sub_8001FC90((uint32_t)((int32_t)r_s16(axis_out + 4) << 4), rrj_read32(actor + 184) - rrj_read32(actor + 580));
        int32_t second = (int32_t)sub_8001FC90((uint32_t)((int32_t)r_s16(axis_out) << 4), rrj_read32(actor + 192) - rrj_read32(actor + 588));
        int32_t side = first - second;
        uint32_t valid = 1;
        uint32_t length;
        uint32_t reciprocal;
        uint32_t selected = normalized;
        (void)sub_8002EAD8(rrj_at(actor + 580, 12), rrj_at(direction_out, 6), side > 0 ? scale : 0u - scale, rrj_at(actor + 456, 12));
        for (index = 0; index < 3; ++index)
            rrj_write32(delta + 4 * index, rrj_read32(actor + 184 + 4 * index) - rrj_read32(actor + 456 + 4 * index));
        while (leaf_abs32(rrj_s32(rrj_read32(delta))) > 0x5A8000 || leaf_abs32(rrj_s32(rrj_read32(delta + 4))) > 0x5A8000 || leaf_abs32(rrj_s32(rrj_read32(delta + 8))) > 0x5A8000)
        {
            valid = 0;
            for (index = 0; index < 3; ++index)
                rrj_write32(delta + 4 * index, (uint32_t)(rrj_s32(rrj_read32(delta + 4 * index)) >> 1));
        }
        length = sub_8002E548(delta);
        if (length < 655)
            length = 655;
        reciprocal = 0x80000000u / (length >> 1);
        (void)sub_8002EED8(reciprocal, delta, normalized);
        if (valid)
        {
            uint32_t ratio = (uint32_t)sub_8001FC90(scale, reciprocal);
            uint32_t angle;
            if (ratio > 0x10000)
                ratio = 0x10000;
            angle = sub_8001FF3C(ratio);
            (void)sub_8003FB34(source + 522, side < 0 ? angle - 1024 : 1024 - angle, matrix);
            (void)sub_8002EFF4(normalized, matrix, rotated);
            selected = rotated;
        }
        (void)sub_8002EAD8(rrj_at(actor + 456, 12), rrj_at(selected, 6), scale, rrj_at(actor + 580, 12));
    }
    return 1;
}

uint32_t sub_800BEBA4(uint16_t packed_id, uint32_t actor)
{
    uint32_t group = packed_id >> 5;
    uint32_t slot = packed_id & 31;
    uint32_t object = 0;
    uint32_t record;
    uint32_t factor_address;
    uint32_t factor;
    int32_t longitudinal;
    int32_t lateral;
    int32_t relative_speed;
    FUNCTION_MARKER(0x800BEBA4, "RASHCDG.BIN");
    if (group == 6)
    {
        record = rrj_read32(0x800CD6C4) + 280 * slot;
        factor_address = rrj_read32(record + 8) == 1 ? 0x800CCACC : 0x800CCACE;
    }
    else
    {
        uint32_t table = 0x800CE4D0 + 16 * group;
        object = rrj_read32(table) + rrj_read32(table + 4) * slot;
        record = object + 172;
        factor_address = 0x800CCAC8 + (rrj_u16(rrj_at(record, 2)) >> 5);
    }
    factor = r_u8(factor_address);
    if (r_u8(actor + 928) & 1)
    {
        uint32_t reference = rrj_read32(record + 188);
        if (reference == rrj_read32(actor + 360) && (!(reference >> 16) || rrj_read32(record + 164) == rrj_read32(actor + 336)))
            longitudinal = rrj_s32(rrj_read32(record + 172) - rrj_read32(actor + 344));
        else
            longitudinal = rrj_s32(sub_800B6AAC(rrj_at(record + 12, 12), rrj_at(actor + 432, 6), rrj_at(actor + 184, 12)));
    }
    else
    {
        longitudinal = leaf_product_asr16((int32_t)r_s16(actor + 516) * 16, rrj_s32(rrj_read32(record + 12) - rrj_read32(actor + 504)));
        longitudinal = rrj_s32((uint32_t)longitudinal + (uint32_t)leaf_product_asr16((int32_t)r_s16(actor + 518) * 16, rrj_s32(rrj_read32(record + 16) - rrj_read32(actor + 508))));
        longitudinal = rrj_s32((uint32_t)longitudinal + (uint32_t)leaf_product_asr16((int32_t)r_s16(actor + 520) * 16, rrj_s32(rrj_read32(record + 20) - rrj_read32(actor + 512))));
    }
    if (leaf_abs32(longitudinal) > 0x20000)
        return 0x7FFF0000;
    if ((r_u8(actor + 928) & 1) && rrj_s32(rrj_read32(actor + 324)) > 0x3C000)
    {
        lateral = rrj_s32((rrj_read32(actor + 324) - rrj_read32(record + 152)) << 4);
    }
    else
    {
        lateral = leaf_product_asr16((int32_t)r_s16(actor + 528) * 16, rrj_s32(rrj_read32(record + 12) - rrj_read32(actor + 504)));
        lateral = rrj_s32((uint32_t)lateral + (uint32_t)leaf_product_asr16((int32_t)r_s16(actor + 530) * 16, rrj_s32(rrj_read32(record + 16) - rrj_read32(actor + 508))));
        lateral = rrj_s32((uint32_t)lateral + (uint32_t)leaf_product_asr16((int32_t)r_s16(actor + 532) * 16, rrj_s32(rrj_read32(record + 20) - rrj_read32(actor + 512))));
    }
    if (lateral < -rrj_s32(rrj_read32(record + 136)))
        return 0x7FFF0000;
    relative_speed = rrj_s32(rrj_read32(actor + 480));
    if ((rrj_u16(rrj_at(record, 2)) >> 5) < 4)
    {
        int32_t dot = (int16_t)(((int32_t)r_s16(object + 444) * r_s16(actor + 528)) >> 12);
        dot += (int16_t)(((int32_t)r_s16(object + 446) * r_s16(actor + 530)) >> 12);
        dot += (int16_t)(((int32_t)r_s16(object + 448) * r_s16(actor + 532)) >> 12);
        if (leaf_abs32(rrj_s32((uint32_t)dot << 4)) >= 0x4000)
        {
            if (dot < 0)
                relative_speed = rrj_s32(rrj_read32(actor + 480) + rrj_read32(object + 480));
            else
                relative_speed = rrj_s32(rrj_read32(actor + 480) - rrj_read32(object + 480));
        }
    }
    if (relative_speed < 524 || lateral > 0xBF0000)
        return 0x7FFF0000;
    {
        int32_t ratio = lateral <= 0 ? -(int32_t)sub_80010028((uint32_t)-lateral, (uint32_t)relative_speed) : (int32_t)sub_80010028((uint32_t)lateral, (uint32_t)relative_speed);
        uint32_t scaled_factor = factor << 12;
        uint32_t distance;
        {
            uint32_t reference = rrj_read32(record + 188);
            if (reference == rrj_read32(actor + 360) && (!(reference >> 16) || rrj_read32(record + 164) == rrj_read32(actor + 336)))
                longitudinal = rrj_s32(rrj_read32(record + 172) - rrj_read32(actor + 344));
            else
                longitudinal = rrj_s32(sub_800B6AAC(rrj_at(record + 12, 12), rrj_at(actor + 432, 6), rrj_at(actor + 184, 12)));
        }
        distance = leaf_abs32(longitudinal);
        if (distance > 87162)
        {
            uint32_t curve = (uint32_t)(((uint64_t)distance * distance) >> 16);
            int32_t weighted = rrj_s32(9 * curve);
            curve = (uint32_t)(weighted < 0 ? (weighted + 15) >> 4 : weighted >> 4);
            scaled_factor = (uint32_t)(((uint64_t)scaled_factor * curve) >> 16);
        }
        if (!(rrj_u16(rrj_at(record, 2)) >> 5) && (rrj_read32(rrj_read32(object + 852) + 552) & 0x8000))
            return 0x7FFF0000;
        return (uint32_t)(((int64_t)rrj_s32(scaled_factor) * ratio) >> 16);
    }
}

uint32_t sub_8008AE94(uint32_t output, uint32_t count_io, uint32_t position, int32_t radius, uint32_t mask, uint16_t exclude_id)
{
    uint32_t state = rrj_read32(0x8005B2F8);
    uint32_t players = rrj_read32(state + 48);
    int32_t x_cell = (rrj_s32(rrj_read32(position) - rrj_read32(0x800CCF98)) >> 21) + 11;
    int32_t z_cell = (rrj_s32(rrj_read32(position + 8) - rrj_read32(0x800CCFA0)) >> 21) + 11;
    int32_t expanded = radius + ((radius + (int32_t)((uint32_t)radius >> 31)) >> 1);
    uint32_t plane;
    int32_t x_first;
    int32_t x_last;
    int32_t z_first;
    int32_t z_last;
    uint32_t count = 0;
    uint32_t capacity = rrj_read32(count_io);
    FUNCTION_MARKER(0x8008AE94, "RASHCDG.BIN");
    if (players != 1 && ((uint32_t)x_cell >= 24 || (uint32_t)z_cell >= 24))
    {
        z_cell = (rrj_s32(rrj_read32(position + 8) - rrj_read32(0x800CCFA4)) >> 21) + 11;
        if (z_cell < 0)
            z_cell += 24;
    }
    plane = z_cell >= 24 && z_cell < (24 << ((players - 1) & 31));
    x_first = (rrj_s32(rrj_read32(position) - (uint32_t)expanded - rrj_read32(0x800CCF98 + 4 * plane)) >> 21) + 11;
    x_last = (rrj_s32(rrj_read32(position) + (uint32_t)expanded - rrj_read32(0x800CCF98 + 4 * plane)) >> 21) + 11;
    z_first = (rrj_s32(rrj_read32(position + 8) - (uint32_t)expanded - rrj_read32(0x800CCFA0 + 4 * plane)) >> 21) + 11;
    z_last = (rrj_s32(rrj_read32(position + 8) + (uint32_t)expanded - rrj_read32(0x800CCFA0 + 4 * plane)) >> 21) + 11;
    if (x_first < 0)
        x_first = 0;
    if (z_first < 0)
        z_first = 0;
    if (x_last >= 24)
        x_last = 23;
    if (z_last >= 24)
        z_last = 23;
    if (x_first >= 24 || z_first >= 24 || x_last < 0 || z_last < 0)
    {
        rrj_write32(count_io, 0);
        return 0;
    }
    {
        int32_t z;
        uint32_t plane_offset = plane ? 24 : 0;
        for (z = z_first; z <= z_last; ++z)
        {
            int32_t x;
            for (x = x_first; x <= x_last; ++x)
            {
                uint32_t node = r_u8(0x800CD0B0 + 24 * (plane_offset + (uint32_t)z) + (uint32_t)x);
                while (node != 0x80)
                {
                    uint32_t link = 0x800CCFA8 + 2 * node;
                    uint32_t next = r_u8(link);
                    uint32_t packed = r_u8(link + 1);
                    uint32_t group = packed >> 5;
                    if (mask & (1u << (group & 31)))
                    {
                        uint32_t record;
                        uint32_t id_address;
                        uint32_t x_address;
                        uint32_t z_address;
                        uint16_t id;
                        if (group == 0)
                        {
                            record = rrj_read32(0x8005B3A0) + 1096 * packed;
                            id_address = record + 172;
                            x_address = record + 184;
                            z_address = record + 192;
                        }
                        else if (group == 1)
                        {
                            record = rrj_read32(0x8005B3A4) + 628 * (packed & 31);
                            id_address = record + 172;
                            x_address = record + 184;
                            z_address = record + 192;
                        }
                        else
                        {
                            uint32_t table = 0x800CE4D0 + 16 * group;
                            record = rrj_read32(table) + rrj_read32(table + 4) * (packed & 31) + rrj_read32(0x800CCA68 + 4 * (group - 2));
                            id_address = record;
                            x_address = record + 12;
                            z_address = record + 20;
                        }
                        id = rrj_u16(rrj_at(id_address, 2));
                        if (id != exclude_id)
                        {
                            uint32_t dx = leaf_abs32(rrj_s32(rrj_read32(x_address) - rrj_read32(position)));
                            uint32_t dz = leaf_abs32(rrj_s32(rrj_read32(z_address) - rrj_read32(position + 8)));
                            uint32_t minimum = dx < dz ? dx : dz;
                            uint32_t metric = dx + dz - ((minimum + (minimum >> 31)) >> 1);
                            uint32_t duplicate = 0;
                            uint32_t index;
                            if ((uint32_t)radius >= metric)
                            {
                                for (index = 0; index < count; ++index)
                                {
                                    if (rrj_u16(rrj_at(output + 2 * index, 2)) == id)
                                    {
                                        duplicate = 1;
                                        break;
                                    }
                                }
                                if (!duplicate)
                                {
                                    rrj_put16(rrj_at(output + 2 * count, 2), id);
                                    ++count;
                                    if (count == capacity)
                                        return count;
                                }
                            }
                        }
                    }
                    node = next;
                }
            }
        }
    }
    rrj_write32(count_io, count);
    return count;
}

uint32_t sub_800BB5B8(uint32_t actor, uint16_t other_id)
{
    uint32_t other;
    int32_t delta;
    uint32_t result;
    FUNCTION_MARKER(0x800BB5B8, "RASHCDG.BIN");
    if (other_id == 224)
        return 224;
    result = rrj_read32(actor + 568) & 0x600;
    if (result)
        return result;
    other = rrj_read32(0x8005B3A0) + 1096 * other_id;
    if ((r_u8(rrj_read32(actor + 1084) + 1) & 0x0F) == 2)
        delta = rrj_s32(sub_800B6AAC(rrj_at(other + 504, 12), rrj_at(actor + 528, 6), rrj_at(actor + 504, 12)));
    else
        delta = rrj_s32((rrj_read32(actor + 324) - rrj_read32(other + 324)) << 4);
    if (!sub_800BB8FC(actor, other, delta))
    {
        sub_800BC8DC(actor);
        return 0;
    }
    if (delta < rrj_s32(3u * (rrj_read32(actor + 480) - rrj_read32(other + 480))))
    {
        int32_t other_value = rrj_s32(rrj_read32(other + 344));
        int32_t actor_value = rrj_s32(rrj_read32(actor + 344));
        int32_t relative = rrj_s32((uint32_t)other_value + (uint32_t)actor_value);
        int32_t limit;
        int32_t response;
        uint32_t kind;
        if (rrj_s32(rrj_read32(other + 364) ^ rrj_read32(actor + 364)) >= 0)
            relative = other_value - actor_value;
        kind = rrj_u16(rrj_at(actor + 956 + 8 * ((int32_t)r_s8(actor + 946) - 2), 2));
        limit = kind - 14 < 2 ? 0x20000 : rrj_s32(rrj_read32(actor + 304) + rrj_read32(other + 304));
        if (limit >= relative && (rrj_s32(rrj_read32(0x80052F64)) >= delta || -limit >= relative || ((uint32_t)(int32_t)r_s16(rrj_read32(actor + 340) + 36) >> 31) == (uint32_t)(rrj_s32(rrj_read32(actor + 364)) > 0)))
            response = rrj_s32(rrj_read32(0x80052F58));
        else
            response = -rrj_s32(rrj_read32(0x80052F58));
        if (sub_800BC618(other, rrj_u16(rrj_at(actor + 172, 2)), response))
        {
            if (kind - 14 < 3)
                response = -response;
            else
                response = rrj_s32((uint32_t)response << 1);
        }
        sub_800BC4FC(actor, other, response);
    }
    else if (r_s16(actor + 944) > 0)
    {
        int32_t magnitude = rrj_s32(sub_8002E548(actor + 892));
        if (magnitude < 16)
        {
            rrj_write32(actor + 908, 0);
            rrj_put16(rrj_at(actor + 944, 2), 0);
        }
        else
        {
            uint32_t ratio = sub_80010028(163840, (uint32_t)magnitude);
            uint32_t inverse = sub_80010028((uint32_t)magnitude, 163840);
            rrj_write32(actor + 904, ratio);
            rrj_put16(rrj_at(actor + 944, 2), (uint16_t)(rrj_s32(0u - ((inverse << 8) + 0x8000u)) >> 16));
        }
    }
    if (rrj_u16(rrj_at(actor + 320, 2)) & 2)
    {
        uint32_t flags = rrj_read32(actor + 564);
        uint32_t state = rrj_read32(0x8005B2F8);
        if (rrj_u16(rrj_at(other + 172, 2)) < rrj_read32(state + 48))
            flags |= 0x200;
        rrj_write32(actor + 564, flags);
        rrj_write32(actor + 924, rrj_read32(rrj_read32(actor + 556) + 224));
    }
    result = rrj_read32(actor + 564) & 0x80000;
    if (!result)
    {
        uint32_t value = rrj_read32(actor + 924);
        result = (uint32_t)sub_8001FC90(rrj_s32(value), rrj_s32(value));
        rrj_write32(actor + 916, (uint32_t)delta);
        rrj_write32(actor + 920, result);
        rrj_write32(actor + 564, rrj_read32(actor + 564) & 0xFFF7FFFFu);
    }
    return result;
}

uint32_t sub_8002E604(uint32_t left, uint32_t right)
{
    int32_t result;
    FUNCTION_MARKER(0x8002E604, "SLUS_010.53");
    result = leaf_product_asr16(rrj_s32(rrj_read32(left)), rrj_s32(rrj_read32(right)));
    result = rrj_s32((uint32_t)result + (uint32_t)leaf_product_asr16(rrj_s32(rrj_read32(left + 4)), rrj_s32(rrj_read32(right + 4))));
    result = rrj_s32((uint32_t)result + (uint32_t)leaf_product_asr16(rrj_s32(rrj_read32(left + 8)), rrj_s32(rrj_read32(right + 8))));
    return (uint32_t)result;
}

uint32_t sub_8002EED8(uint32_t scale, uint32_t vector, uint32_t output)
{
    int32_t result;
    uint32_t index;
    FUNCTION_MARKER(0x8002EED8, "SLUS_010.53");
    for (index = 0; index < 3; ++index)
    {
        result = (int32_t)sub_8001FC90(scale, rrj_read32(vector + 4 * index)) >> 4;
        rrj_put16(rrj_at(output + 2 * index, 2), (uint16_t)result);
    }
    return (uint32_t)result;
}

uint32_t sub_8002EFF4(uint32_t vector, uint32_t matrix, uint32_t output)
{
    int32_t result = 0;
    uint32_t row;
    uint32_t column;
    FUNCTION_MARKER(0x8002EFF4, "SLUS_010.53");
    for (column = 0; column < 3; ++column)
    {
        result = 0;
        for (row = 0; row < 3; ++row)
        {
            int32_t left = r_s16(vector + 2 * row);
            int32_t right = r_s16(matrix + 2 * (3 * row + column));
            result = rrj_s32((uint32_t)result + (uint32_t)((left * right) >> 12));
        }
        rrj_put16(rrj_at(output + 2 * column, 2), (uint16_t)result);
    }
    return (uint32_t)result;
}

uint32_t sub_800CA05C(uint32_t mode)
{
    uint32_t primary = rrj_read32(0x8005B38C);
    uint32_t state = rrj_read32(0x8005B2F8);
    uint32_t event = 0x1F8003A0;
    uint32_t first = 0x1F8003B0;
    uint32_t second = 0x1F8003C0;
    uint32_t object;
    uint32_t affected = 0;
    int32_t index;
    FUNCTION_MARKER(0x800CA05C, "RASHCDG.BIN");
    rrj_put16(rrj_at(event, 2), 18);
    if (mode)
    {
        uint32_t descriptor = rrj_read32(primary + 1084);
        w_u8(descriptor + 39, 0xFE);
        rrj_write32(descriptor + 40, rrj_read32(state + 16));
    }
    else
    {
        uint32_t linked = rrj_read32(primary + 856);
        sub_800BC8DC(linked);
        rrj_put16(rrj_at(event + 2, 2), rrj_u16(rrj_at(primary + 172, 2)));
        (void)sub_800BCA68(event, 0, linked);
        xport_update_u8(rrj_read32(primary + 852) + 572, XPORT_MEMORY_UPDATE_OR, 0x10);
    }
    index = rrj_s32(rrj_read32(rrj_read32(0x800CE4DC)));
    object = rrj_read32(0x800CE4D0);
    while (index >= 0)
    {
        uint32_t descriptor;
        uint32_t secondary;
        uint32_t type;
        if (!rrj_u16(rrj_at(object + 320, 2)))
            goto next_object;
        descriptor = rrj_read32(object + 1084);
        type = r_u8(descriptor + 1) & 0x0F;
        if (type == 2 || type == (uint32_t)(r_u8(rrj_read32(primary + 1084) + 1) & 0x0F))
            goto next_object;
        secondary = rrj_read32(object + 852);
        if (rrj_u16(rrj_at(secondary + 544, 2)) != 64)
            goto next_object;
        if (mode)
        {
            uint32_t primary_secondary = rrj_read32(primary + 852);
            int32_t length_first;
            int32_t length_second;
            int32_t dot;
            int32_t scale;
            int32_t ratio;
            rrj_write32(first, rrj_read32(primary_secondary + 184) - rrj_read32(secondary + 184));
            rrj_write32(first + 4, rrj_read32(primary_secondary + 188) - rrj_read32(secondary + 188));
            rrj_write32(first + 8, rrj_read32(primary_secondary + 192) - rrj_read32(secondary + 192));
            length_first = rrj_s32(sub_8002E548(first));
            if (length_first <= 0x3FFFF)
            {
                rrj_write32(0x8005B228, 0x50000);
                goto next_object;
            }
            rrj_write32(second, rrj_read32(object + 184) - rrj_read32(secondary + 184));
            rrj_write32(second + 4, rrj_read32(object + 188) - rrj_read32(secondary + 188));
            rrj_write32(second + 8, rrj_read32(object + 192) - rrj_read32(secondary + 192));
            length_second = rrj_s32(sub_8002E548(second));
            scale = (int32_t)(((int64_t)length_first * length_second) >> 16);
            dot = rrj_s32(sub_8002E604(first, second));
            ratio = (int32_t)sub_80010028(dot > 0 ? (uint32_t)dot : (uint32_t)-dot, scale > 0 ? (uint32_t)scale : (uint32_t)-scale);
            if ((dot > 0 && scale <= 0) || (dot <= 0 && scale > 0))
                ratio = -ratio;
            if (ratio <= 52427)
                goto next_object;
            ++affected;
        }
        sub_800BC8DC(object);
        rrj_put16(rrj_at(event + 2, 2), rrj_u16(rrj_at(object + 172, 2)));
        (void)sub_800BCA68(event, 0, object);
        if (!mode)
        {
            uint32_t blend = sub_8001FC58() & 0x1F;
            int32_t value;
            if (blend >= 17)
                blend -= 16;
            value = rrj_s32((16 - blend) * rrj_read32(0x8005ADE8) + blend * rrj_read32(0x8005ADEC));
            value = value < 0 ? (value + 15) >> 4 : value >> 4;
            rrj_put16(rrj_at(object + 968, 2), (uint16_t)((79u * (uint32_t)value) >> 14));
        }
    next_object:
        --index;
        object += rrj_read32(0x800CE4D4);
    }
    if (affected)
        rrj_write32(0x8005B228, rrj_read32(0x8005ADE8) + 0xA0000);
    return rrj_read32(0x8005B228);
}

uint32_t sub_800C9E74(void)
{
    uint32_t actor = rrj_read32(0x8005B38C);
    uint32_t state = rrj_read32(0x8005B2F8);
    uint32_t piece;
    uint32_t result;
    int32_t distance;
    FUNCTION_MARKER(0x800C9E74, "RASHCDG.BIN");
    distance = rrj_s32((uint32_t)(int32_t)r_s8(0x8005ADF2) << 16);
    distance = rrj_s32((uint32_t)distance - rrj_read32(actor + 0x170));
    distance = rrj_s32((uint32_t)distance + ((uint32_t)rrj_u16(rrj_at(0x80053174 + 4 * (r_u8(state + 57) - 1) + 2, 2)) << 16));
    if (distance > 0xA0000)
    {
        int32_t product;
        piece = rrj_read32(actor + 0x154);
        product = (int32_t)r_u8(rrj_read32(actor + 0x43C) + 0x45) * rrj_s32(rrj_read32(rrj_read32(actor + 0x22C) + 0xE0));
        if (product < 0)
            product += 0x7F;
        rrj_write32(actor + 0x39C, (uint32_t)(product >> 7));
        result = sub_8002EAD8(rrj_at(piece + 20, 12), rrj_at(piece + 14, 6), distance > 0x280000 ? 0x280000 : (uint32_t)distance, rrj_at(actor + 0x370, 12));
        if (distance <= 0x280000)
        {
            actor = rrj_read32(0x8005B38C);
            result = sub_8002EAD8(rrj_at(actor + 0x370, 12), rrj_at(rrj_read32(actor + 0x154) + 2, 6), (uint32_t)(int32_t)r_s8(0x8005ADF3) << 16, rrj_at(actor + 0x370, 12));
        }
        return result;
    }
    rrj_write32(actor + 0x39C, 0);
    result = sub_8002EAD8(rrj_at(actor + 0x1F8, 12), rrj_at(actor + 0x1C2, 6), rrj_read32(actor + 0x134) << 3, rrj_at(actor + 0x370, 12));
    rrj_write32(actor + 0x230, rrj_read32(actor + 0x230) | 0x40000);
    if (!(rrj_read32(actor + 0x234) & 0x80000))
    {
        rrj_write32(actor + 0x394, 0);
        rrj_write32(actor + 0x398, 0);
        rrj_write32(actor + 0x234, rrj_read32(actor + 0x234) & 0xFFF7FFFF);
    }
    if (r_u8(state + 57) == 2 && rrj_s32(rrj_read32(actor + 0x1E0)) <= 0x7FFF)
    {
        uint32_t linked = rrj_read32(actor + 0x358);
        int32_t index = (int32_t)r_s8(linked + 0x3B2) - 1;
        if (!rrj_u16(rrj_at(linked + 8 * (uint32_t)index + 0x3BC, 2)))
            return sub_800CA05C(0);
    }
    return result;
}

void sub_800BADB8(uint32_t actor, uint32_t contact)
{
    uint32_t state = rrj_read32(0x8005B2F8);
    uint32_t actor_id = rrj_u16(rrj_at(actor + 172, 2));
    uint32_t actor_type = r_u8(rrj_read32(actor + 1084) + 1) & 0x0F;
    uint32_t primary = rrj_read32(0x8005B38C);
    uint32_t retained = 0;
    FUNCTION_MARKER(0x800BADB8, "RASHCDG.BIN");
    if (actor_id < rrj_read32(state + 48) && (uint32_t)(r_u8(state + 57) - 1) < 2)
    {
        (void)sub_800C9E74();
    }
    else if (rrj_s32(rrj_read32(actor + 480)) > 0x8000)
    {
        uint32_t primary_type = r_u8(rrj_read32(primary + 1084) + 1) & 0x0F;
        if (r_u8(state + 57) >= 4 && actor_type != primary_type)
        {
            int32_t relative;
            int32_t magnitude;
            uint32_t value = rrj_read32(actor + 576) - 0xA0000;
            rrj_write32(actor + 924, rrj_s32(value) < 0 ? 0 : value);
            relative = rrj_s32(rrj_read32(actor + 344) + rrj_read32(primary + 344));
            if (rrj_s32(rrj_read32(actor + 364) ^ rrj_read32(primary + 364)) >= 0)
                relative = rrj_s32(rrj_read32(actor + 344) - rrj_read32(primary + 344));
            if (rrj_read32(actor + 372) && !rrj_u16(rrj_at(actor + 362, 2)))
            {
                int16_t factor = r_s16(actor + (relative < 0 ? 408 : 420));
                magnitude = rrj_s32(rrj_read32(actor + 424)) * factor;
            }
            else
            {
                magnitude = 0x39999;
            }
            if (relative < 0)
                magnitude = -magnitude;
            magnitude = rrj_s32((uint32_t)magnitude - ((uint32_t)(int32_t)r_s16(actor + 878) << 5));
            leaf_80093CAC_scalar(rrj_host_context(), actor, actor + 872, magnitude, 1);
            return;
        }
        rrj_write32(actor + 924, 0);
        if (actor_id < rrj_read32(state + 48) || actor_type != 2 || rrj_u16(rrj_at(contact + 2, 2)) == 224)
        {
            uint32_t scale = (uint32_t)sub_8001FC90(0x80000, rrj_read32(actor + 308));
            (void)sub_8002EAD8(rrj_at(actor + 504, 12), rrj_at(actor + 450, 6), scale, rrj_at(actor + 880, 12));
            rrj_write32(actor + 560, rrj_read32(actor + 560) | 0x40000);
        }
        else
        {
            uint32_t other = rrj_read32(0x8005B3A0) + 1096 * rrj_u16(rrj_at(contact + 2, 2));
            retained = leaf_abs32(rrj_s32(sub_8009E444(actor, other)));
            if (retained <= 0xFFFF)
                retained = 0x10000;
            if (rrj_read32(other + 372))
            {
                uint32_t piece = rrj_read32(other + 340);
                if (rrj_read32(other + 388) & 1)
                    (void)sub_8002EAD8(rrj_at(piece + 20, 12), rrj_at(piece + 14, 6), rrj_read32(other + 348), rrj_at(actor + 880, 12));
                else
                {
                    uint32_t direction = rrj_s32(rrj_read32(other + 364)) < 0 ? 0xFFFF0000 : 0x10000;
                    uint32_t scale = (uint32_t)sub_8001FC90(direction, rrj_read32(actor + 308));
                    (void)sub_8002EAD8(rrj_at(actor + 184, 12), rrj_at(piece + 14, 6), scale, rrj_at(actor + 880, 12));
                }
            }
        }
        if (rrj_read32(actor + 564) & 0x200)
            rrj_write32(actor + 564, rrj_read32(actor + 564) & 0xFFFFFDFF);
        if (!(rrj_read32(actor + 564) & 0x80000))
        {
            rrj_write32(actor + 916, retained);
            rrj_write32(actor + 920, 0);
            rrj_write32(actor + 564, rrj_read32(actor + 564) & 0xFFF7FFFF);
        }
    }
    {
        uint32_t secondary = rrj_read32(actor + 852);
        uint32_t secondary_id = rrj_u16(rrj_at(secondary + 544, 2));
        if ((rrj_u16(rrj_at(contact + 6, 2)) & 0x8000) && rrj_read32(secondary + 604) == 1 && (secondary_id < 26 || secondary_id >= 38) && rrj_u16(rrj_at(0x800541D4 + 8 * secondary_id + 2, 2)) != 3)
        {
            uint32_t descriptor = rrj_read32(actor + 1084);
            uint32_t event = 9;
            if (actor_id < rrj_read32(state + 48) && rrj_read32(descriptor + 40) && r_u8(descriptor + 39) < 4 && !r_u8(state + 57))
                event = 1;
            (void)sub_800C4550(event, secondary, 2);
        }
    }
    if (actor_id < rrj_read32(state + 48))
    {
        uint32_t player = 0x800CD898 + 1132 * actor_id;
        if (!(rrj_read32(player + 552) & 0x44) && r_u8(state + 4) != 44)
            (void)sub_8008A998(player, 10);
    }
}

uint32_t sub_800BFF04(uint32_t first, uint32_t second, uint32_t flags, uint32_t flags_out)
{
    uint32_t state = rrj_read32(0x8005B2F8);
    uint32_t first_secondary = rrj_read32(first + 852);
    uint32_t second_secondary = rrj_read32(second + 852);
    uint32_t first_id = rrj_u16(rrj_at(first + 172, 2));
    uint32_t second_id = rrj_u16(rrj_at(second + 172, 2));
    uint32_t player_count = rrj_read32(state + 48);
    uint32_t special;
    uint32_t result = 0;
    FUNCTION_MARKER(0x800BFF04, "RASHCDG.BIN");
    special = first_id < player_count || second_id < player_count || (r_u8(first_secondary + 572) & 0x20) || (r_u8(second_secondary + 572) & 0x20);
    if (!special)
        return 0;
    if (rrj_u16(rrj_at(0x800541D4 + 8 * rrj_u16(rrj_at(second_secondary + 544, 2)) + 2, 2)) != 3)
        return 3;
    {
        uint32_t second_descriptor = rrj_read32(second + 1084);
        uint32_t first_descriptor = rrj_read32(first + 1084);
        uint32_t animation;
        uint32_t entry;
        int32_t frame;
        uint32_t limit;
        if (!(r_u8(second_descriptor + 60) & 0x80))
            return 0;
        result = sub_800C2E9C(first_secondary, r_u8(second_descriptor + 46));
        if (!result)
            return result;
        animation = rrj_read32(second_secondary + 540);
        entry = rrj_read32(rrj_read32(rrj_read32(animation + 40) + 4) + 4 * r_u8(rrj_read32(animation + 4) + 12 * rrj_read32(animation + 12)));
        frame = (int16_t)(rrj_u16(rrj_at(entry + 16, 2)) - 1);
        limit = rrj_read32(animation + 16);
        if (3 * frame < rrj_s32(4 * limit))
            rrj_write32(flags_out, rrj_read32(flags_out) | 0x40);
        if (rrj_read32(flags_out) & 0x40)
            return sub_80017BA0(rrj_read32(first + 184), rrj_read32(first + 192), 88, 0);
        result = rrj_read32(flags_out) | 0x80;
        if (frame >= rrj_s32(2 * limit))
            return result;
        rrj_write32(flags_out, result);
        w_u8(first_descriptor + 46, r_u8(second_descriptor + 46));
        rrj_put16(rrj_at(first_descriptor + 44, 2), rrj_u16(rrj_at(first_descriptor + 44, 2)) | (uint16_t)(1u << (r_u8(second_descriptor + 46) & 31)));
        rrj_put16(rrj_at(second_descriptor + 44, 2), rrj_u16(rrj_at(second_descriptor + 44, 2)) & (uint16_t)~(1u << (r_u8(second_descriptor + 46) & 31)));
        w_u8(second_descriptor + 46, 9);
        w_u8(second_descriptor + 70, (uint8_t)first_id);
        w_u8(first_descriptor + 71, (uint8_t)second_id);
        (void)sub_800BFE58(first, 0, 0);
        (void)sub_800BFE58(second, 0, 1);
        {
            uint32_t row = r_u8(second_secondary + 569);
            uint32_t column = r_u8(second_secondary + 570);
            int32_t variant = (int32_t)r_s8(second_secondary + 574 + column);
            uint32_t table = rrj_read32(0x8005AD4C) + 12 * row;
            uint32_t sound = rrj_u16(rrj_at(rrj_read32(table + 8) + 12 * (uint32_t)variant, 2));
            uint32_t transfer = r_u8(second_secondary + 572) >> 7;
            uint32_t effect = (r_u8(first_descriptor + 1) & 0x0F) ? 0x61 : 0x60;
            (void)sub_80017BA0(rrj_read32(first + 184), rrj_read32(first + 192), effect, 0);
            w_u8(first_descriptor + 60, 0x8E);
            w_u8(second_descriptor + 60, 0x20);
            (void)sub_800C4550(16, second_secondary, ((flags & 0xFFFFFFF0) ^ 0x100) | 0x10);
            (void)sub_800958F0(first_secondary, flags & 0x100);
            if (transfer)
            {
                uint32_t lane = r_u8(first_descriptor + 46);
                uint32_t value = r_u8(second_descriptor + 47);
                uint32_t packed;
                xport_update_u8(second_secondary + 572, XPORT_MEMORY_UPDATE_AND, 0x7F);
                if (lane < 8)
                    value += (rrj_read32(first_descriptor + 48) >> (4 * lane)) & 0x0F;
                packed = rrj_read32(first_descriptor + 48) & ~(0x0Fu << ((4 * lane) & 31));
                rrj_write32(first_descriptor + 48, packed | ((value & 0x0F) << ((4 * lane) & 31)));
                w_u8(first_descriptor + 47, (uint8_t)value);
                rrj_write32(second_descriptor + 48, rrj_read32(second_descriptor + 48) & ~(0x0Fu << ((4 * lane) & 31)));
                (void)sub_800273EC(0x800CF018 + 172 * (uint32_t)(int32_t)r_s8(first_secondary + 571), 0, 1500, 6, 0);
                xport_update_u8(first_secondary + 572, XPORT_MEMORY_UPDATE_OR, 0x80);
            }
            w_u8(second_descriptor + 47, 0);
            sub_800C2F84(sound, first_secondary, (flags >> 8) & 0xFF, (flags >> 16) & 0xFF);
            return sub_800C2030(first_secondary);
        }
    }
}

uint32_t sub_8009989C(uint32_t actor, uint32_t linked, uint32_t vector, uint32_t magnitude, uint32_t fixed_result, uint32_t threshold)
{
    uint32_t speed;
    uint32_t measured;
    uint32_t stopped;
    uint32_t result;
    FUNCTION_MARKER(0x8009989C, "RASHCDG.BIN");
    if (fixed_result)
    {
        while (leaf_abs32(rrj_s32(rrj_read32(vector))) > 0x5A8000 || leaf_abs32(rrj_s32(rrj_read32(vector + 4))) > 0x5A8000 || leaf_abs32(rrj_s32(rrj_read32(vector + 8))) > 0x5A8000)
        {
            rrj_write32(vector, (uint32_t)(rrj_s32(rrj_read32(vector)) >> 1));
            rrj_write32(vector + 4, (uint32_t)(rrj_s32(rrj_read32(vector + 4)) >> 1));
            rrj_write32(vector + 8, (uint32_t)(rrj_s32(rrj_read32(vector + 8)) >> 1));
        }
        measured = sub_8002E548(vector);
        speed = 0x7FFF0000;
    }
    else
    {
        speed = sub_8004CF74(magnitude) << 2;
        measured = speed;
    }
    rrj_write32(actor + 484, speed);
    stopped = speed < 655 || (rrj_read32(actor + 480) && speed <= 0x7FFF);
    if (!stopped)
    {
        uint32_t normalized = 0x1F800300;
        uint32_t first_cross_address = 0x1F800310;
        uint32_t second_cross_address = 0x1F800320;
        int16_t diagonal[3];
        int16_t direction[3];
        int16_t first_cross[3];
        int16_t second_cross[3];
        uint32_t reciprocal = 0x80000000u / (measured >> 1);
        uint32_t index;
        int32_t angle;
        uint32_t scaled;
        (void)sub_8002EED8(reciprocal, vector, normalized);
        for (index = 0; index < 3; ++index)
        {
            diagonal[index] = r_s16(actor + 522 + 2 * index);
            direction[index] = r_s16(normalized + 2 * index);
        }
        leaf_direction_op_local(diagonal, direction, first_cross);
        for (index = 0; index < 3; ++index)
            rrj_put16(rrj_at(first_cross_address + 2 * index, 2), (uint16_t)first_cross[index]);
        if (!sub_8002E468(first_cross_address))
        {
            for (index = 0; index < 3; ++index)
                rrj_put16(rrj_at(first_cross_address + 2 * index, 2), rrj_u16(rrj_at(actor + 528 + 2 * index, 2)));
        }
        for (index = 0; index < 3; ++index)
        {
            first_cross[index] = r_s16(first_cross_address + 2 * index);
            direction[index] = r_s16(actor + 522 + 2 * index);
        }
        leaf_direction_op_local(first_cross, direction, second_cross);
        for (index = 0; index < 3; ++index)
            rrj_put16(rrj_at(second_cross_address + 2 * index, 2), (uint16_t)second_cross[index]);
        angle = sub_80093BE0(second_cross_address, actor + 528, actor + 516);
        scaled = leaf_abs32((int32_t)sub_8001FC90(rrj_read32(actor + 488), threshold));
        if (scaled < 3431)
            scaled = 3431;
        if ((uint32_t)(((uint64_t)25736 * leaf_abs32(angle)) >> 8) >= scaled)
        {
            result = rrj_read32(actor + 552) | (angle >= 0 ? 0x500 : 0x300);
            rrj_write32(actor + 552, result);
            return result;
        }
        for (index = 0; index < 3; ++index)
        {
            rrj_put16(rrj_at(actor + 528 + 2 * index, 2), rrj_u16(rrj_at(second_cross_address + 2 * index, 2)));
            rrj_put16(rrj_at(actor + 516 + 2 * index, 2), rrj_u16(rrj_at(first_cross_address + 2 * index, 2)));
        }
    }
    rrj_write32(actor + 488, 0);
    if (speed < 0x3333)
    {
        stopped = 1;
    }
    else
    {
        int64_t dot = 0;
        uint32_t index;
        for (index = 0; index < 3; ++index)
            dot += (int64_t)rrj_s32(rrj_read32(vector + 4 * index)) * ((int32_t)r_s16(actor + 528 + 2 * index) << 4);
        stopped = dot < 0;
    }
    if (stopped)
    {
        uint32_t index;
        for (index = 0; index < 3; ++index)
        {
            rrj_write32(actor + 184 + 4 * index, rrj_read32(actor + 580 + 4 * index));
            rrj_put16(rrj_at(actor + 450 + 2 * index, 2), rrj_u16(rrj_at(actor + 528 + 2 * index, 2)));
        }
        rrj_write32(actor + 552, rrj_read32(actor + 552) | 8);
        rrj_write32(linked + 576, 0);
        rrj_write32(linked + 480, 0);
    }
    result = (rrj_read32(actor + 552) & 0xFFFFFE7F) | 0x100;
    rrj_write32(actor + 552, result);
    return result;
}

uint32_t sub_80096820(uint32_t state, uint32_t actor, uint32_t delta, RRJRaceLeafCall call)
{
    uint32_t descriptor = rrj_read32(actor + 1084);
    uint32_t type = r_u8(descriptor + 1) & 0x0F;
    uint32_t race = rrj_read32(0x8005B2F8);
    uint32_t course = rrj_read32(race + 60);
    uint32_t result = 0;
    FUNCTION_MARKER(0x80096820, "RASHCDG.BIN");
    switch (state)
    {
        case 1:
        {
            uint32_t table;
            uint32_t active = 0;
            int32_t remaining;
            int32_t index;
            uint32_t object;
            rrj_write32(0x8005B2EC, 0);
            (void)(uint32_t)sub_8001B244(4);
            if (type != 2)
            {
                table = rrj_read32(0x8005B2B0) ? 0x80053054 : 0x8005303C;
                rrj_write32(actor + 924, rrj_read32(table + 4 * course));
                return 0;
            }
            w_u8(descriptor + 39, (uint8_t)sub_800138E8(actor, 0));
            remaining = rrj_s32(rrj_read32(rrj_read32(0x800CE4DC)));
            object = rrj_read32(0x800CE4D0);
            for (index = remaining; index >= 0; --index, object += rrj_read32(0x800CE4D4))
            {
                if (r_s16(object + 320))
                {
                    uint32_t secondary = rrj_read32(object + 852);
                    if ((uint32_t)(rrj_read32(secondary + 604) - 3) < 2)
                        ++active;
                }
            }
            remaining = rrj_s32(rrj_read32(0x8005B1F8)) - (int32_t)(active + 1) - rrj_s32(rrj_read32(0x80053168 + 4 * course));
            if (r_u8(descriptor + 39) < (uint32_t)remaining)
            {
                table = rrj_read32(0x8005B2B0) ? 0x80053048 : 0x80053030;
                rrj_write32(actor + 924, rrj_read32(table + 4 * course));
                return 0;
            }
            if (rrj_read32(rrj_read32(actor + 852) + 604) < 3 && !(rrj_read32(actor + 36) & 0x08000000))
            {
                (void)sub_80028034(actor);
                (void)sub_80020E30(0);
            }
            {
                uint32_t contact = sub_8003B4B0(rrj_read32(actor + 428), rrj_u16(rrj_at(actor + 360, 2)));
                if (contact)
                {
                    uint32_t vector = 0x1F800340;
                    int16_t x = r_s16(rrj_read32(actor + 340) + 14);
                    int16_t y = r_s16(rrj_read32(actor + 340) + 16);
                    int16_t z = r_s16(rrj_read32(actor + 340) + 18);
                    int32_t first;
                    int32_t second;
                    int32_t third;
                    int32_t combined;
                    if (rrj_s32(rrj_read32(contact + 4)) < 0)
                    {
                        x = (int16_t)-x;
                        z = (int16_t)-z;
                    }
                    rrj_put16(rrj_at(vector, 2), (uint16_t)x);
                    rrj_put16(rrj_at(vector + 2, 2), (uint16_t)y);
                    rrj_put16(rrj_at(vector + 4, 2), (uint16_t)z);
                    first = rrj_s32(sub_8002E698(rrj_at(actor + 444, 6), rrj_at(vector, 6)));
                    second = rrj_s32(sub_8002E698(rrj_at(actor + 444, 6), rrj_at(vector, 6)));
                    third = rrj_s32(sub_8002E698(rrj_at(actor + 444, 6), rrj_at(vector, 6)));
                    combined = rrj_s32((uint32_t)((first >> 31) + second) ^ (uint32_t)(third >> 31));
                    if (combined <= 62258)
                    {
                        table = rrj_read32(0x8005B2B0) ? 0x80053048 : 0x80053030;
                        rrj_write32(actor + 924, rrj_read32(table + 4 * course));
                        if (!rrj_read32(0x8005B220))
                            return 0;
                    }
                }
            }
            rrj_write32(0x8005ACD0, rrj_read32(race + 16));
            break;
        }
        case 2:
            if (type == 2)
            {
                uint32_t record = 0x800D6198 + 224 * rrj_u16(rrj_at(actor + 172, 2));
                rrj_write32(record + 116, rrj_read32(race + 16));
                rrj_write32(0x8005B2EC, 0);
                rrj_write32(record + 108, 0);
                if (rrj_u16(rrj_at(actor + 948 + 8 * (uint32_t)(int32_t)r_s8(actor + 946), 2)) >= 3)
                {
                    rrj_write32(0x8005AD48, (rrj_read32(0x8005AD48) & 0xFFFFFFF9u) | 4);
                    return 0;
                }
            }
            break;
        case 4:
            if (type == 2)
            {
                uint32_t timer = rrj_read32(0x8005B2EC) + delta;
                uint32_t position = (uint8_t)(r_u8(descriptor + 39) + 4);
                uint32_t near_front = position < 2;
                rrj_write32(0x8005B2EC, timer);
                if (timer >= 20001 && position >= 2 && rrj_read32(actor + 180) >= 0x12)
                    (void)(uint32_t)sub_8001B244(1);
                if (timer < (rrj_read32(0x800530F0 + 4 * course) >> 1))
                    return 0;
                rrj_write32(0x8005AD48, rrj_read32(0x8005AD48) & 0xFFFFFFFBu);
                if (near_front)
                {
                    uint32_t mode = r_u8(descriptor + 39) == 252 ? 0 : 2;
                    (void)(uint32_t)sub_8001B244(mode);
                    rrj_write32(0x8005AD48, rrj_read32(0x8005AD48) | 0x10);
                }
                else
                {
                    uint32_t record = 0x800D6198 + 224 * rrj_u16(rrj_at(actor + 172, 2));
                    rrj_write32(0x8005AD48, rrj_read32(0x8005AD48) | 8);
                    rrj_write32(0x8005B2EC, 0);
                    rrj_write32(record + 116, rrj_read32(race + 16));
                }
                return 0;
            }
            break;
        case 8:
            if (type == 2)
            {
                uint32_t timer = rrj_read32(0x8005B2EC) + delta;
                rrj_write32(0x8005B2EC, timer);
                if (timer > (rrj_read32(0x800530F0 + 4 * course) >> 1))
                {
                    rrj_write32(0x8005B2EC, 0);
                    rrj_write32(0x8005AD48, rrj_read32(0x8005AD48) & 0xFFFFFFF7u);
                }
                return 0;
            }
            break;
        case 16:
            if (type == 2)
                return 0;
            break;
        default:
            break;
    }
    if (type == 2)
        rrj_write32(0x8005AD48, rrj_read32(0x8005AD48) & 0xFFFFFFF8u);
    result = 1;
    if (!rrj_read32(0x8005B220) && !rrj_read32(descriptor + 40))
    {
        uint32_t record = 0x800CD898 + 1132 * rrj_u16(rrj_at(actor + 172, 2));
        if (!rrj_read32(record + 772) && !(rrj_read32(record + 548) & 0x2008))
        {
            uint32_t contact = actor + 948 + 8 * (uint32_t)(int32_t)r_s8(actor + 946);
            uint32_t blocked = rrj_u16(rrj_at(contact, 2)) == 3 && ((rrj_u16(rrj_at(contact + 2, 2)) >> 5) == 3);
            if (rrj_s32(rrj_read32(actor + 480)) <= 439457 || (!blocked && !rrj_u16(rrj_at(actor + 362, 2))))
            {
                rrj_write32(actor + 560, rrj_read32(actor + 560) & 0xD7FFFFFFu);
                return 1;
            }
        }
    }
    return result;
}

uint32_t sub_80096818(uint32_t actor, uint32_t delta, RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x80096818, "RASHCDG.BIN");
    return sub_80096820(rrj_read32(0x8005AD48), actor, delta, call);
}

uint32_t sub_8009AD50(uint32_t enabled, uint32_t specification, uint32_t actor, RRJRaceLeafCall call)
{
    uint32_t config = 0x1F800300;
    uint32_t geometry = 0x1F800320;
    int32_t side;
    uint32_t segment;
    uint32_t object;
    uint32_t current;
    uint32_t next;
    uint32_t source;
    int32_t distance;
    int32_t turn;
    uint32_t axis;
    uint32_t index;
    FUNCTION_MARKER(0x8009AD50, "RASHCDG.BIN");
    if (!enabled)
        return 0;
    rrj_write32(config, rrj_read32(specification + 8));
    rrj_write32(config + 8, rrj_read32(specification + 36));
    side = (int16_t)rrj_u16(rrj_at(specification + 60, 2));
    rrj_write32(config + 4, (uint32_t)side);
    if ((rrj_read32(config) >> 16) == 1)
        return 0;
    if (!side)
    {
        side = (int32_t)(sub_8001FC58() & 1);
        if (!side)
            side = -1;
        rrj_write32(config + 4, (uint32_t)side);
    }
    segment = sub_80039DFC(0, config);
    if (!segment)
        return 0;
    current = rrj_read32(0x800CF654);
    if (rrj_s32(current) >= 16)
        return 0;
    next = current + 1;
    while (rrj_s32(next) < 16)
    {
        uint32_t candidate = 0x800CF660 + 512 * next;
        if (!rrj_u16(rrj_at(candidate + 172, 2)) && rrj_read32(candidate + 4))
            break;
        ++next;
    }
    object = 0x800CF660 + 512 * current;
    rrj_write32(0x800CF654, next);
    rrj_put16(rrj_at(object + 172, 2), (uint16_t)(current + 96));
    rrj_write32(0x800CF650, rrj_read32(0x800CF650) + 1);
    if (rrj_s32(rrj_read32(0x800CF658)) < rrj_s32(current))
        rrj_write32(0x800CF658, current);
    rrj_write32(object + 180, rrj_u16(rrj_at(specification + 2, 2)));
    rrj_put16(rrj_at(object + 322, 2), rrj_u16(rrj_at(specification + 62, 2)));
    if (!sub_8003A700(segment, config, geometry) || r_s16(rrj_read32(geometry + 4) + 2) == 1)
    {
        (void)sub_8008C000(object + 172, 3);
        return 0;
    }
    (void)sub_8001E0B4(object + 328, geometry, 32);
    source = rrj_read32(object + 340);
    rrj_write32(object + 184, rrj_read32(source + 20));
    rrj_write32(object + 188, rrj_read32(source + 24));
    rrj_write32(object + 192, rrj_read32(source + 28));
    (void)sub_8002EAD8(rrj_at(source + 20, 12), rrj_at(source + 14, 6), rrj_read32(object + 348), rrj_at(object + 184, 12));
    rrj_put16(rrj_at(object + 450, 2), 0);
    rrj_put16(rrj_at(object + 452, 2), 0);
    rrj_put16(rrj_at(object + 454, 2), 0);
    (void)sub_8003662C(object + 450, object + 328, object + 360);
    {
        uint32_t secondary = rrj_read32(actor + 852);
        uint32_t reference;
        uint32_t progress;
        uint32_t position[3];
        if (rrj_read32(secondary + 604) < 3)
        {
            position[0] = rrj_read32(actor + 184);
            position[1] = rrj_read32(actor + 188);
            position[2] = rrj_read32(actor + 192);
            reference = rrj_read32(actor + 360);
            progress = rrj_read32(actor + 368);
        }
        else
        {
            position[0] = rrj_read32(secondary + 184);
            position[1] = rrj_read32(secondary + 188);
            position[2] = rrj_read32(secondary + 192);
            reference = rrj_read32(secondary + 360);
            progress = rrj_read32(secondary + 368);
        }
        if (rrj_read32(object + 360) == reference)
        {
            int32_t delta = rrj_s32(progress - rrj_read32(object + 368));
            distance = rrj_s32((uint32_t)(((delta >> 31) + delta) ^ (delta >> 31))) >> 16;
        }
        else
        {
            int32_t dx = ((int32_t)position[0] >> 16) - r_s16(object + 186);
            int32_t dz = ((int32_t)position[2] >> 16) - r_s16(object + 194);
            int32_t major = (int32_t)leaf_abs32(dx);
            int32_t minor = (int32_t)leaf_abs32(dz);
            if (major < minor)
            {
                int32_t swap = major;
                major = minor;
                minor = swap;
            }
            distance = major - (major >> 5) - (major >> 7) + ((minor + (minor >> 1)) >> 2) + ((minor + (minor >> 1)) >> 6);
        }
    }
    {
        uint32_t race = rrj_read32(0x8005B2F8);
        if (r_u8(race + 4) != 44 && rrj_s32(rrj_read32(race + 16)) > 0 && rrj_s32((uint32_t)distance << 16) <= 0x77FFFF)
        {
            (void)sub_8008C000(object + 172, 3);
            return 0;
        }
    }
    rrj_write32(object + 364, rrj_read32(config + 4));
    turn = (int16_t)rrj_u16(rrj_at(specification + 64, 2));
    if ((int32_t)leaf_abs32(turn) == 4)
    {
        int32_t candidate = 0;
        uint32_t attempts = 0;
        do
        {
            candidate = (int32_t)(sub_8001FC58() & 3);
            w_u8(object + 508, (uint8_t)candidate);
            ++attempts;
        } while (candidate == (int32_t)r_s8(0x8005B358) && attempts < 5);
        w_u8(0x8005B358, (uint8_t)candidate);
    }
    else
    {
        w_u8(object + 508, (uint8_t)turn);
    }
    if (!r_s8(object + 508))
        w_u8(object + 508, 1);
    if (turn < 0)
        w_u8(object + 508, (uint8_t)-r_s8(object + 508));
    (void)sub_8003DE28(object, 1, 0, 0xFFFFFFFF);
    if (rrj_read32(object + 372))
    {
        int32_t first = sub_8009E768(object);
        int32_t second = sub_8009E768(object);
        int32_t third = sub_8009E768(object);
        rrj_write32(object + 344, (uint32_t)(((first >> 31) + second) ^ (third >> 31)));
    }
    else
    {
        rrj_write32(object + 344, 0x0001CCCC);
    }
    if (side < 0)
        rrj_write32(object + 344, (uint32_t)-rrj_s32(rrj_read32(object + 344)));
    if (r_s8(object + 508) < 0)
        rrj_write32(object + 344, (uint32_t)-rrj_s32(rrj_read32(object + 344)));
    (void)sub_8002EAD8(rrj_at(object + 184, 12), rrj_at(source + 2, 6), rrj_read32(object + 344), rrj_at(object + 184, 12));
    sub_8003DF54(object, 1, 0xFFFFFFFF);
    rrj_write32(object + 492, 0);
    rrj_write32(object + 496, 0);
    rrj_put16(rrj_at(object + 438, 2), (uint16_t)-r_s16(source + 8));
    rrj_put16(rrj_at(object + 440, 2), (uint16_t)-r_s16(source + 10));
    rrj_put16(rrj_at(object + 442, 2), (uint16_t)-r_s16(source + 12));
    for (index = 0; index < 3; ++index)
    {
        rrj_put16(rrj_at(object + 444 + 2 * index, 2), rrj_u16(rrj_at(source + 14 + 2 * index, 2)));
        rrj_put16(rrj_at(object + 432 + 2 * index, 2), rrj_u16(rrj_at(source + 2 + 2 * index, 2)));
    }
    if (side < 0)
    {
        for (index = 0; index < 3; ++index)
        {
            rrj_put16(rrj_at(object + 432 + 2 * index, 2), (uint16_t)-r_s16(object + 432 + 2 * index));
            rrj_put16(rrj_at(object + 444 + 2 * index, 2), (uint16_t)-r_s16(object + 444 + 2 * index));
        }
    }
    (void)sub_8003AF9C(object + 172, 0, actor);
    rrj_write32(object + 324, sub_8003B61C(object + 172));
    rrj_write32(object + 180, sub_8002FAD4(object, 3, rrj_u16(rrj_at(specification + 2, 2)), 0, call));
    if (rrj_read32(object + 180) == 0xFFFF)
    {
        (void)sub_8008C000(object + 172, 3);
        return 0;
    }
    (void)sub_80012FC8(object, 0);
    (void)sub_8008BA18(object);
    axis = sub_80020018((uint32_t)(int32_t)r_s16(object + 444), (uint32_t)(int32_t)r_s16(object + 448));
    rrj_write32(object + 292, axis);
    rrj_write32(object + 296, (uint32_t)((int32_t)r_s16(0x8005624E + 4 * (axis & 0xFFF)) << 4));
    rrj_write32(object + 300, (uint32_t)((int32_t)r_s16(0x8005624C + 4 * (axis & 0xFFF)) << 4));
    rrj_write32(object + 484, 0x00026666);
    rrj_put16(rrj_at(object + 452, 2), rrj_u16(rrj_at(object + 446, 2)));
    rrj_put16(rrj_at(object + 454, 2), rrj_u16(rrj_at(object + 448, 2)));
    rrj_put16(rrj_at(object + 450, 2), rrj_u16(rrj_at(object + 444, 2)));
    w_u8(object + 509, 0);
    w_u8(object + 510, 0);
    rrj_write32(object + 316, 0x05500000);
    rrj_write32(object + 504, 0xFFFF0000);
    w_u8(object + 511, 0xFF);
    rrj_put16(rrj_at(object + 320, 2), (uint16_t)sub_80039F68(object + 172));
    rrj_write32(object + 176, 0xFFFFFFFF);
    if (!r_s16(object + 320))
    {
        (void)sub_8008C000(object + 172, 3);
        object = 0;
    }
    if ((r_u8(rrj_read32(0x8005B2F8) + 4) & 1) && object && !rrj_read32(object + 180))
        (void)sub_80028034(object);
    return object;
}

uint32_t sub_8009AD48(uint32_t specification, uint32_t actor, RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x8009AD48, "RASHCDG.BIN");
    return sub_8009AD50((uint32_t)(int32_t)r_s16(0x800CE578), specification, actor, call);
}

uint32_t sub_8009D664(uint32_t actor, uint32_t source, RRJRaceLeafCall call)
{
    uint32_t race = rrj_read32(0x8005B2F8);
    uint32_t specification = 0x1F800380;
    uint32_t route = 0;
    uint32_t route_id = 0xFFFFFFFF;
    uint32_t create = 0;
    int32_t direction = 0;
    uint32_t index;
    FUNCTION_MARKER(0x8009D664, "RASHCDG.BIN");
    if (sub_8009D9E4() && rrj_read32(0x8005B284) != 0xFFFFFFFF && rrj_s32(rrj_read32(race + 16) - rrj_read32(0x8005B284)) < 500)
        return 0;
    if (!sub_8008CDF4(3))
    {
        int32_t remaining = rrj_s32(rrj_read32(rrj_read32(0x800CE50C)));
        uint32_t object = rrj_read32(0x800CE500);
        uint32_t stride = rrj_read32(0x800CE504);
        uint32_t candidate = 0;
        while (remaining >= 0)
        {
            if (object && rrj_u16(rrj_at(object + 172, 2)) && rrj_read32(object + 180))
            {
                uint32_t player;
                for (player = 0; rrj_s32(player) < rrj_s32(rrj_read32(race + 48)); ++player)
                {
                    if (!((r_u8(object + 9) >> (player & 31)) & 1))
                    {
                        candidate = object;
                        break;
                    }
                }
                if (candidate)
                    break;
            }
            --remaining;
            object += stride;
        }
        if (candidate)
            (void)sub_8008C000(candidate + 172, 3);
    }
    if ((rrj_read32(actor + 360) >> 16) == 1)
    {
        route_id = rrj_read32(actor + 360) & 0xFFFF;
    }
    else
    {
        int32_t progress;
        direction = -rrj_s32(rrj_read32(actor + 364));
        if (rrj_s32(rrj_read32(actor + 480)) < 131)
            direction = rrj_s32(rrj_read32(actor + 364));
        route = sub_800245DC(rrj_read32(actor + 360) & 0xFFFF);
        if (route)
        {
            if (direction > 0)
            {
                route_id = rrj_read32(route + 12);
                progress = rrj_s32((rrj_read32(route + 4) >> 6) << 16) - rrj_s32(rrj_read32(actor + 368));
            }
            else
            {
                route_id = rrj_read32(route + 8);
                progress = rrj_s32(rrj_read32(actor + 368));
            }
            if (progress > 0x7D0000)
            {
                rrj_write32(specification + 8, rrj_read32(actor + 360));
                rrj_write32(specification + 36, direction > 0 ? rrj_read32(actor + 368) - 0x7D0000 : rrj_read32(actor + 368) + 0x7D0000);
                rrj_put16(rrj_at(specification + 60, 2), (uint16_t)direction);
                create = 1;
            }
        }
    }
    if (!create && route_id != 0xFFFFFFFF && route)
    {
        uint32_t alternatives = sub_800245F4(route_id);
        if (alternatives)
        {
            uint32_t count = rrj_read32(alternatives + 4);
            uint32_t entry = alternatives + 8;
            uint32_t found = 0;
            if (count >= 2)
            {
                if ((rrj_read32(actor + 360) >> 16) == 1)
                {
                    index = sub_8001FC58() % count;
                    entry += 8 * index;
                    found = 1;
                }
                else
                {
                    for (index = 0; index < count; ++index, entry += 8)
                    {
                        if (rrj_read32(entry) != (rrj_read32(actor + 360) & 0xFFFF))
                        {
                            found = 1;
                            break;
                        }
                    }
                }
                if (found && sub_800245DC(rrj_read32(entry)))
                {
                    uint32_t next_route = sub_800245DC(rrj_read32(entry));
                    rrj_write32(specification + 8, rrj_read32(entry));
                    if (rrj_s32(rrj_read32(entry + 4)) > 0)
                        rrj_write32(specification + 36, ((rrj_read32(next_route + 4) >> 6) << 16) - 0x780000);
                    else
                        rrj_write32(specification + 36, 0x780000);
                    rrj_put16(rrj_at(specification + 60, 2), rrj_u16(rrj_at(entry + 4, 2)));
                    create = 1;
                }
            }
        }
    }
    if (!create)
        return 0;
    for (index = 0; index < 68; ++index)
    {
        if (index != 8 && index != 9 && index != 36 && index != 37 && index != 60 && index != 61)
            w_u8(specification + index, 0);
    }
    rrj_put16(rrj_at(specification + 64, 2), 4);
    rrj_put16(rrj_at(specification + 2, 2), (uint16_t)(rrj_u16(rrj_at(0x800CE578, 2)) - 1));
    rrj_write32(0x8005B284, rrj_read32(race + 16));
    {
        uint32_t object = sub_8009AD48(specification, actor, call);
        if (object)
        {
            rrj_write32(object + 480, 0x00283BBA);
            w_u8(object + 511, r_u8(source + 172));
        }
        return object;
    }
}

uint32_t sub_80096F30(uint32_t actor, uint32_t other, uint32_t mode, RRJRaceLeafCall call)
{
    uint32_t race = rrj_read32(0x8005B2F8);
    uint32_t descriptor;
    uint32_t secondary;
    uint32_t result = 0;
    FUNCTION_MARKER(0x80096F30, "RASHCDG.BIN");
    if (rrj_u16(rrj_at(actor + 172, 2)) >= rrj_read32(race + 48))
        return 0;
    descriptor = rrj_read32(actor + 1084);
    if ((r_u8(descriptor + 1) & 0x0F) != 2)
        return 0;
    secondary = rrj_read32(actor + 852);
    if (rrj_read32(secondary + 604) >= 2 || (rrj_read32(secondary + 552) & 0x8000) || rrj_read32(0x8005AD48))
        return 0;
    if (rrj_read32(rrj_read32(other + 1084) + 40))
        return 0;
    if (rrj_u16(rrj_at(other + 172, 2)) == r_u8(race + 6))
    {
        if (r_u8(race + 4) == 33)
        {
            w_u8(descriptor + 39, 0xF8);
            rrj_write32(descriptor + 40, rrj_read32(race + 16));
        }
        else
        {
            w_u8(descriptor + 39, 0xFC);
            (void)(uint32_t)sub_8001B244(0);
        }
    }
    else
    {
        if (r_u8(race + 4) == 33)
            return 33;
        rrj_write32(0x8005AD44, rrj_read32(0x8005AD44) - 1);
        if (rrj_s32(rrj_read32(0x8005AD44)) <= 0)
            w_u8(descriptor + 39, 0xFD);
    }
    (void)sub_80097308(other);
    if (r_u8(race + 4) != 33)
    {
        uint32_t obstacle = sub_8009D664(actor, other, call);
        if (obstacle)
        {
            uint32_t record = 0x800CD898 + 1132 * rrj_u16(rrj_at(actor + 172, 2));
            rrj_write32(record + 776, 8);
            rrj_write32(record + 752, obstacle + 184);
            rrj_write32(record + 756, obstacle + 320);
            rrj_write32(record + 552, rrj_read32(record + 552) | 0x10);
            (void)sub_8008A998(record, 12);
        }
    }
    rrj_write32(actor + 560, rrj_read32(actor + 560) | 0x28000000);
    if (rrj_read32(actor + 560) & 0x08000000)
    {
        uint32_t type = rrj_u16(rrj_at(secondary + 544, 2));
        if (rrj_u16(rrj_at(0x800541D4 + 8 * type + 2, 2)) == 3)
        {
            uint32_t table = rrj_read32(0x8005AD4C) + 12 * r_u8(secondary + 569);
            (void)sub_800C4550(rrj_u16(rrj_at(table, 2)), secondary, 2);
        }
    }
    (void)sub_800BCD10(actor);
    {
        uint32_t event = 0x1F8003D0;
        uint16_t state = rrj_s32(rrj_read32(actor + 480)) > 585944 ? 4 : 1;
        rrj_put16(rrj_at(event, 2), state);
        rrj_put16(rrj_at(event + 2, 2), 224);
        (void)sub_800BCA68(event, 1, actor);
        rrj_write32(0x8005AD48, rrj_read32(0x8005AD48) | 2);
        state = rrj_u16(rrj_at(event, 2));
        if (state == 1)
        {
            rrj_write32(actor + 720, 0x20000);
            rrj_write32(actor + 484, 0);
            rrj_write32(actor + 576, 0);
            rrj_write32(actor + 480, 0);
            rrj_write32(actor + 924, 0);
            rrj_put16(rrj_at(event + 4, 2), 0);
            rrj_write32(actor + 464, 0);
            rrj_write32(actor + 460, 0);
            rrj_write32(actor + 456, 0);
        }
        else
        {
            rrj_write32(actor + 924, rrj_read32(0x80053048 + 4 * rrj_read32(race + 60)));
        }
    }
    result = (uint8_t)(r_u8(descriptor + 39) + 4) < 2;
    if (result)
    {
        uint32_t record = 0x800CD898 + 1132 * rrj_u16(rrj_at(actor + 172, 2));
        rrj_write32(0x8005B230, 0);
        if (!(rrj_read32(record + 552) & 0x44))
            (void)sub_8008A998(record, mode);
        rrj_write32(descriptor + 40, rrj_read32(race + 16));
        result = 2;
        if (r_u8(descriptor + 39) == 0xFD && rrj_read32(race + 48) == 2)
        {
            uint32_t other_actor = rrj_read32(0x8005B268 + 4 * (rrj_u16(rrj_at(actor + 172, 2)) == 0));
            result = rrj_read32(race + 16);
            rrj_write32(rrj_read32(other_actor + 1084) + 40, result);
        }
    }
    return result;
}

uint32_t sub_800BF674(uint32_t object, uint8_t event, uint32_t direction, uint32_t actor, RRJRaceLeafCall call)
{
    uint32_t secondary = rrj_read32(object + 852);
    uint32_t flags = rrj_read32(object + 568);
    uint32_t value;
    uint32_t special = (uint8_t)(event - 36) < 2 || event == 75 || event == 76 || event == 146 || event == 147;
    uint32_t race = rrj_read32(0x8005B2F8);
    uint32_t actor_id = rrj_u16(rrj_at(actor + 172, 2));
    uint32_t result;
    FUNCTION_MARKER(0x800BF674, "RASHCDG.BIN");
    if (rrj_s32(rrj_read32(object + 364)) < 0)
        flags |= 0x400000;
    else
        flags &= 0xFFBFFFFFu;
    rrj_write32(object + 568, flags);
    rrj_write32(secondary + 552, rrj_read32(secondary + 552) & 0xFFF9FFFFu);
    if (rrj_read32(object + 856) || (flags & 0x400))
    {
        if (special)
            value = 0x8000;
        else
            value = ((0u - direction) & 0xFFFE0000u) + 0x40000u | 0x8000u;
        rrj_write32(secondary + 552, rrj_read32(secondary + 552) | value);
        rrj_write32(object + 720, 0xFFFF0000);
    }
    else
    {
        uint32_t negated = 0u - direction;
        if (special)
        {
            value = 0x880;
        }
        else
        {
            value = ((negated & 0x8000) + 0x8000) | 0x840;
            rrj_write32(secondary + 552, rrj_read32(secondary + 552) | 0x8000u | ((negated & 0xFFFE0000u) + 0x40000u));
        }
        rrj_write32(object + 568, (rrj_read32(object + 568) & 0xFFFF9FC0u) | value);
    }
    result = actor_id < rrj_read32(race + 48);
    if (result)
    {
        uint32_t slot = actor_id > 1 ? 1 : actor_id;
        rrj_write32(0x800D6198 + 224 * slot + 140, 1);
        result = r_u8(race + 4) & 1;
        if (result)
            return sub_80096F30(actor, object, 9, call);
    }
    return result;
}

uint32_t sub_800C17B0(uint32_t first, uint32_t second, uint32_t flags, uint32_t active, RRJRaceLeafCall call)
{
    uint32_t first_secondary = rrj_read32(first + 852);
    uint32_t second_secondary = rrj_read32(second + 852);
    uint32_t first_descriptor = rrj_read32(first + 1084);
    uint32_t second_descriptor = rrj_read32(second + 1084);
    uint32_t race = rrj_read32(0x8005B2F8);
    uint32_t local = 0x1F8003C0;
    uint32_t event = sub_800BF860(first_secondary, second_secondary, active & 1, local);
    int32_t damage = 0;
    uint32_t completed = 0;
    FUNCTION_MARKER(0x800C17B0, "RASHCDG.BIN");
    if (active & 1)
    {
        uint8_t packed = r_u8(first_descriptor + 61);
        packed = (uint8_t)((((packed & 0x0F) + 1) & 0x0F) | (packed & 0xF0));
        w_u8(first_descriptor + 61, packed);
        if (!(r_u8(first_secondary + 572) & 0x20))
        {
            uint32_t id = rrj_read32(second + 1088) ? rrj_u16(rrj_at(second + 172, 2)) : rrj_u16(rrj_at(rrj_read32(second + 856) + 172, 2));
            rrj_write32(local + 4, rrj_u16(rrj_at(first_descriptor + 66, 2)));
            (void)sub_800A8BE0((uint16_t)id, local + 4);
            rrj_put16(rrj_at(first_descriptor + 66, 2), rrj_u16(rrj_at(local + 4, 2)));
        }
        if (!(r_u8(second_secondary + 572) & 0x20))
        {
            rrj_write32(local + 4, rrj_u16(rrj_at(second_descriptor + 64, 2)));
            (void)sub_800A8BE0(rrj_u16(rrj_at(first + 172, 2)), local + 4);
            rrj_put16(rrj_at(second_descriptor + 64, 2), rrj_u16(rrj_at(local + 4, 2)));
        }
        if (r_u8(first_secondary + 572) & 0x80)
        {
            rrj_write32(local, 3 * rrj_read32(local));
            (void)sub_80017BA0(rrj_read32(first + 184), rrj_read32(first + 192), 105, 0);
        }
        {
            uint32_t strength = r_u8(first_descriptor + 15);
            int32_t product;
            int32_t divisor;
            int32_t remaining;
            if (strength < 0x61)
                strength = 0x60;
            product = rrj_s32(rrj_read32(local) * strength * r_u8(first_descriptor + 12));
            damage = product < 0 ? (product + 0x3FFF) >> 14 : product >> 14;
            divisor = rrj_s32(rrj_read32(0x80053188 + 4 * rrj_read32(race + 60)));
            divisor = divisor ? divisor : 1;
            remaining = (int32_t)r_u8(second_descriptor + 15) - damage;
            {
                int32_t decrement = (damage + 1) / divisor;
                if (remaining < 0)
                {
                    uint32_t sound = (r_u8(first_descriptor + 1) & 0x0F) ? 97 : 96;
                    w_u8(second_descriptor + 15, 0);
                    if (rrj_u16(rrj_at(first + 172, 2)) < rrj_read32(race + 48) || !rrj_read32(first + 1088))
                    {
                        (void)(uint32_t)rrj_draft_80017B6C(first, 109, 4, 0);
                        (void)(uint32_t)rrj_draft_80017B6C(first, sound, 10, 0);
                    }
                }
                else
                {
                    uint8_t marker = r_u8(second_descriptor + 68);
                    uint32_t old = r_u8(second_descriptor + 15);
                    if ((old >= 0x40 && remaining < 0x40) || (old >= 0x20 && remaining < 0x20))
                        marker |= 0x20;
                    w_u8(second_descriptor + 68, marker);
                    w_u8(second_descriptor + 15, (uint8_t)remaining);
                }
                remaining = (int32_t)r_u8(second_descriptor + 14) - decrement;
                if ((r_u8(second_secondary + 572) & 0x20))
                {
                    uint32_t linked_descriptor = rrj_read32(rrj_read32(second + 856) + 1084);
                    uint32_t linked = r_u8(linked_descriptor + 14);
                    if ((int32_t)linked >= remaining)
                        remaining = (int32_t)linked;
                }
                if (remaining > 0)
                {
                    uint8_t marker = r_u8(second_descriptor + 68);
                    uint32_t old = r_u8(second_descriptor + 14);
                    if ((old >= 0x40 && remaining < 0x40) || (old >= 0x20 && remaining < 0x20))
                        marker |= 0x20;
                    w_u8(second_descriptor + 68, marker);
                    w_u8(second_descriptor + 14, (uint8_t)remaining);
                }
                else
                {
                    w_u8(second_descriptor + 14, 0);
                }
            }
        }
        (void)sub_800BFE58(first, 2, 0);
        (void)sub_800BFE58(second, 2, 1);
    }
    if (!r_u8(second_descriptor + 15))
    {
        if (!(rrj_read32(second + 568) & 0x3EC) && !(rrj_read32(second + 560) & 0x20000000))
        {
            (void)sub_800BF674(second, r_u8(first_descriptor + 60), (flags >> 8) & 1, first, call);
            (void)sub_800BFE58(first, 1, 0);
            (void)sub_800BFE58(second, 1, 1);
        }
        if (rrj_read32(second + 856) != first)
        {
            uint32_t target = rrj_read32(second + 1088) ? second : rrj_read32(second + 856);
            uint32_t descriptor = rrj_read32(target + 1084);
            int32_t value = (int32_t)r_u8(descriptor + 37) - (r_u8(descriptor + 36) >> 4);
            w_u8(descriptor + 37, (uint8_t)(value < 0 ? 0 : value));
        }
    }
    (void)sub_800BF424(first, second);
    if (rrj_read32(second_secondary + 604) < 2)
    {
        uint32_t base_flags = flags & 0xFFFFFFF0u;
        uint32_t dispatch = base_flags ^ 0x100;
        if (active & 1)
        {
            if (!(rrj_read32(second + 568) & 0x7FF))
            {
                if (rrj_read32(second + 1088))
                {
                    uint32_t row = r_u8(first_secondary + 569);
                    if (r_u8(first_secondary + 572) & 0x20)
                        row -= 20;
                    (void)sub_800BF604(second, r_u8(first_secondary + 570), row, (base_flags >> 8) & 1);
                }
                if ((rrj_u16(rrj_at(second + 172, 2)) < rrj_read32(race + 48) || (!rrj_read32(second + 1088) && !(r_u8(second_secondary + 572) & 0x40))) && !rrj_read32(0x8005B220))
                    (void)sub_800BFD74(second, damage);
            }
            {
                uint32_t animation = rrj_read32(first_secondary + 540);
                uint32_t state = rrj_read32(animation + 36);
                rrj_write32(animation + 36, state | 0x80);
            }
            dispatch = base_flags ^ 0x100;
        }
        completed = 1;
        (void)sub_800C4550(event, second_secondary, dispatch | 0x10);
    }
    if (!(active & 1))
        return completed;
    if ((rrj_u16(rrj_at(first + 172, 2)) < rrj_read32(race + 48) || (!rrj_read32(first + 1088) && !(r_u8(first_secondary + 572) & 0x40))) && !rrj_read32(0x8005B220))
        (void)sub_800BFD74(first, damage);
    return completed;
}

uint32_t sub_800C1370(uint32_t first, uint32_t second, RRJRaceLeafCall call)
{
    uint32_t first_secondary = rrj_read32(first + 852);
    uint32_t first_descriptor = rrj_read32(first + 1084);
    uint32_t relation = rrj_read32(second + 360);
    int32_t longitudinal;
    int32_t lateral;
    uint32_t direction;
    uint32_t active = 0;
    uint32_t flags_out = 0x1F8003E0;
    FUNCTION_MARKER(0x800C1370, "RASHCDG.BIN");
    rrj_write32(flags_out, 0);
    if (relation == rrj_read32(first + 360) && (!(relation >> 16) || rrj_read32(second + 336) == rrj_read32(first + 336)))
    {
        longitudinal = rrj_s32(rrj_read32(second + 344) - rrj_read32(first + 344));
        if (rrj_s32(rrj_read32(first + 364)) < 0)
            longitudinal = -longitudinal;
    }
    else
    {
        longitudinal = rrj_s32(sub_800B6AAC(rrj_at(second + 184, 12), rrj_at(first + 432, 6), rrj_at(first + 184, 12)));
    }
    direction = longitudinal > 0 ? 0x100 : 0;
    longitudinal = (int32_t)leaf_abs32(longitudinal);
    lateral = rrj_s32(sub_800B6AAC(rrj_at(first + 504, 12), rrj_at(second + 528, 6), rrj_at(second + 504, 12)));
    if (rrj_u16(rrj_at(0x800541D4 + 8 * rrj_u16(rrj_at(first_secondary + 544, 2)) + 2, 2)) == 3)
    {
        w_u8(first_descriptor + 60, 71);
        if (sub_800C09B0(first))
            active = 32;
    }
    else if ((int32_t)leaf_abs32(lateral) <= 65534 && longitudinal < rrj_s32(rrj_read32(first_descriptor + 8) + rrj_read32(first + 304)))
    {
        w_u8(first_descriptor + 60, 71);
        (void)sub_800C122C(first, second, direction);
        active = 48;
    }
    if (active)
    {
        uint32_t result = sub_800C159C(first, second, longitudinal, lateral, flags_out);
        if (result)
            return sub_800C17B0(first, second, direction, active, call);
        return result;
    }
    if (rrj_read32(first_secondary + 604) == 1)
    {
        uint32_t type = rrj_u16(rrj_at(first_secondary + 544, 2));
        if (rrj_u16(rrj_at(0x800541D4 + 8 * type + 2, 2)) != 3)
        {
            if (type != 16 || sub_8005BE58(rrj_read32(first_secondary + 540)))
                return sub_800C4550(16, first_secondary, direction | 2);
        }
        return 16;
    }
    return 1;
}

uint32_t sub_800BBE00(uint32_t actor, uint16_t other_id, RRJRaceLeafCall call)
{
    uint32_t other;
    int32_t delta;
    FUNCTION_MARKER(0x800BBE00, "RASHCDG.BIN");
    if (other_id == 224)
        return 224;
    other = rrj_read32(0x8005B3A0) + 1096 * other_id;
    delta = rrj_s32((rrj_read32(actor + 324) - rrj_read32(other + 324)) << 4);
    if (sub_800BC1EC(actor, other, delta) && (int32_t)leaf_abs32(delta) < rrj_s32(rrj_read32(0x80052F70)))
        (void)sub_800C1370(actor, other, call);
    return sub_800BBC20(actor, other_id);
}

uint32_t sub_800C035C(uint32_t actor, uint16_t other_id, RRJRaceLeafCall call)
{
    uint32_t secondary = rrj_read32(actor + 852);
    uint32_t descriptor = rrj_read32(actor + 1084);
    uint32_t race = rrj_read32(0x8005B2F8);
    uint32_t other = other_id == 224 ? 0 : rrj_read32(0x8005B3A0) + 1096 * other_id;
    uint32_t setup = 0;
    uint32_t pending = 0;
    uint32_t flags_out = 0x1F8003E8;
    uint32_t type = rrj_u16(rrj_at(secondary + 544, 2));
    uint32_t result = 0;
    int32_t lateral;
    int32_t longitudinal;
    uint32_t response;
    uint32_t linked;
    FUNCTION_MARKER(0x800C035C, "RASHCDG.BIN");
    if (other_id != 224)
    {
        uint32_t other_secondary = rrj_read32(other + 852);
        uint32_t other_linked = rrj_read32(other + 856);
        uint32_t linked_secondary = rrj_read32(other_linked + 852);
        uint32_t physical = linked_secondary & 0x1FFFFFFFu;

        if (physical > 0x1FFFFCu && !(physical >= 0x1FC00000u && physical <= 0x1FC7FFFCu))
        {
            const uint32_t args[7] = {actor, other_id, other, other_secondary, other_linked, rrj_read32(other + 1084), linked_secondary};

            rrj_wip_handoff(rrj_host_context(), 0x800C035Cu, "race_collision", __func__, __FILE__, __LINE__, "invalid_linked_secondary", args, 7);
            exit(20);
        }
    }
    rrj_write32(flags_out, (r_u8(descriptor + 60) & 0x40) ? 32 : 0);
    if (!(rrj_read32(actor + 560) & 0x08000000) && !(r_u8(secondary + 572) & 0x40))
    {
        if (type - 30 < 8)
        {
            rrj_trace_dispatch_target(0x800C1014u);
            return sub_800C1014(secondary);
        }
        if (rrj_u16(rrj_at(0x800541D4 + 8 * type + 2, 2)) == 3)
        {
            rrj_trace_dispatch_target(0x800C09B0u);
            pending = sub_800C09B0(actor);
            rrj_trace_dispatch_target(0x800C035Cu);
        }
        else
        {
            rrj_trace_dispatch_target(0x800C110Cu);
            (void)sub_800C110C(actor, other);
            rrj_trace_dispatch_target(0x800C035Cu);
            setup = 1;
        }
        if (other_id == 224)
            return 224;
    }
    else if (other_id == 224)
    {
        return 224;
    }
    rrj_trace_dispatch_target(0x800B6AACu);
    lateral = rrj_s32(sub_800B6AAC(rrj_at(actor + 504, 12), rrj_at(other + 528, 6), rrj_at(other + 504, 12)));
    rrj_trace_dispatch_target(0x800C035Cu);
    if (r_u8(race + 57) == 3 && rrj_u16(rrj_at(other + 172, 2)) < rrj_read32(race + 48))
        response = 5;
    else
    {
        rrj_trace_dispatch_target(0x800BC1ECu);
        response = sub_800BC1EC(actor, other, -lateral);
        rrj_trace_dispatch_target(0x800C035Cu);
    }
    {
        uint32_t relation = rrj_read32(actor + 360);
        if (relation == rrj_read32(other + 360) && (!(relation >> 16) || rrj_read32(actor + 336) == rrj_read32(other + 336)))
        {
            longitudinal = rrj_s32(rrj_read32(actor + 344) - rrj_read32(other + 344));
            if (rrj_s32(rrj_read32(other + 364)) < 0)
                longitudinal = -longitudinal;
        }
        else
        {
            rrj_trace_dispatch_target(0x800B6AACu);
            longitudinal = rrj_s32(sub_800B6AAC(rrj_at(actor + 184, 12), rrj_at(other + 432, 6), rrj_at(other + 184, 12)));
            rrj_trace_dispatch_target(0x800C035Cu);
        }
    }
    {
        uint32_t current_secondary = rrj_read32(other + 852);
        uint32_t current_linked = rrj_read32(other + 856);
        uint32_t linked_secondary = rrj_read32(current_linked + 852);
        uint32_t secondary_physical = current_secondary & 0x1FFFFFFFu;
        uint32_t linked_physical = linked_secondary & 0x1FFFFFFFu;

        if ((secondary_physical > 0x1FFFFCu && !(secondary_physical >= 0x1FC00000u && secondary_physical <= 0x1FC7FFFCu)) || (linked_physical > 0x1FFFFCu && !(linked_physical >= 0x1FC00000u && linked_physical <= 0x1FC7FFFCu)))
        {
            const uint32_t args[8] = {actor, other_id, other, current_secondary, current_linked, linked_secondary, response, (uint32_t)lateral};

            rrj_wip_handoff(rrj_host_context(), 0x800C035Cu, "race_collision", __func__, __FILE__, __LINE__, "invalid_runtime_link", args, 8);
            exit(20);
        }
    }
    linked = 0;
    if (r_u8(rrj_read32(other + 852) + 572) & 0x10)
    {
        uint32_t linked_actor = rrj_read32(other + 856);
        if (linked_actor != actor && rrj_read32(rrj_read32(linked_actor + 852) + 604) < 2)
            linked = longitudinal > 0 ? 2 : 0;
    }
    if ((rrj_read32(actor + 560) & 0x08000000) || (r_u8(secondary + 572) & 0x40))
    {
        if (rrj_u16(rrj_at(0x800541D4 + 8 * type + 2, 2)) == 3)
            pending = sub_800C09B0(actor);
        if ((response & 4) || sub_800C0BE8(actor, other, lateral, response | linked))
        {
            if (!(rrj_read32(actor + 568) & 0x600) && (rrj_read32(actor + 560) & 0x08000000))
                (void)sub_800C0CE8(actor, other, lateral, call);
            if (type - 30 < 8)
                return type - 30 < 8;
        }
        else
        {
            return 0;
        }
    }
    if (!response)
        return 0;
    if ((response & 4) && !((rrj_s32(rrj_read32(other + 576)) >= 6553 && rrj_read32(rrj_read32(other + 852) + 604) == 1 && rrj_read32(rrj_read32(rrj_read32(other + 856) + 852) + 604) == 1) || (int32_t)leaf_abs32(lateral) > 983039))
    {
        rrj_write32(actor + 924, 0);
        rrj_write32(actor + 564, rrj_read32(actor + 564) | 0x80000);
        w_u8(rrj_read32(other + 1084) + 39, 0xFE);
        if (!rrj_read32(rrj_read32(other + 1084) + 40))
        {
            if (rrj_read32(rrj_read32(other + 852) + 604) < 2)
                (void)sub_80092C7C(other, 9);
            rrj_write32(rrj_read32(other + 1084) + 40, rrj_read32(race + 16));
        }
        rrj_write32(other + 924, 0);
        rrj_write32(other + 564, rrj_read32(other + 564) | 0x80000);
        return rrj_read32(other + 564);
    }
    if (linked)
        longitudinal = rrj_s32((uint32_t)longitudinal - rrj_read32(other + 304) - rrj_read32(rrj_read32(other + 856) + 304));
    if ((int32_t)leaf_abs32(longitudinal) > 2 * rrj_s32(rrj_read32(0x80052F74)) || (int32_t)leaf_abs32(lateral) > 45875)
        return 0;
    {
        uint32_t direction = longitudinal > 0 ? 0x100 : 0;
        if (linked)
            other = rrj_read32(other + 856);
        if (((rrj_read32(actor + 560) & 0x08000000) || (r_u8(secondary + 572) & 0x40)) && rrj_u16(rrj_at(0x800541D4 + 8 * type + 2, 2)) != 3)
        {
            (void)sub_800C122C(actor, other, direction);
            setup = 1;
        }
        if (setup && r_u8(descriptor + 47) && (r_u8(descriptor + 60) & 0x80) && r_s8(secondary + 571) != -1)
        {
            uint32_t lane = r_u8(descriptor + 46);
            rrj_write32(flags_out, rrj_read32(flags_out) | 0x100);
            if (sub_800C159C(actor, other, longitudinal, lateral, flags_out))
            {
                xport_update_u8(descriptor + 47, XPORT_MEMORY_UPDATE_SUBTRACT, 1);
                rrj_write32(descriptor + 48, (rrj_read32(descriptor + 48) & ~(0x0Fu << (4 * lane))) | ((r_u8(descriptor + 47) & 0x0F) << (4 * lane)));
            }
            rrj_write32(flags_out, rrj_read32(flags_out) & 0xFFFFFEFFu);
        }
        if (r_u8(descriptor + 60) == 32)
            (void)sub_800BFF04(actor, other, direction, flags_out);
        if (pending && sub_800C159C(actor, other, longitudinal, lateral, flags_out))
            return sub_800C17B0(actor, other, direction, rrj_read32(flags_out), call);
    }
    return result;
}

uint32_t sub_80095BF8(uint32_t actor, uint32_t delta)
{
    uint32_t race = rrj_read32(0x8005B2F8);
    uint32_t descriptor = rrj_read32(actor + 1084);
    uint32_t type = r_u8(descriptor + 1) & 0x0F;
    uint32_t reference = rrj_read32(0x8005B38C);
    uint32_t course = rrj_read32(race + 60);
    uint32_t target = rrj_read32(actor + 556) + 224;
    uint32_t contact = actor + 948 + 8 * (uint32_t)(int32_t)r_s8(actor + 946);
    FUNCTION_MARKER(0x80095BF8, "RASHCDG.BIN");
    if (rrj_read32(race + 48) == 2)
    {
        uint32_t first = rrj_read32(0x8005B38C);
        uint32_t second = rrj_read32(0x8005B21C);
        uint32_t first_descriptor = rrj_read32(first + 1084);
        uint32_t second_descriptor = rrj_read32(second + 1084);
        if (rrj_read32(first_descriptor + 40) || r_u8(first_descriptor + 39) >= 0xF8)
            reference = second;
        else if (rrj_read32(second_descriptor + 40) || r_u8(second_descriptor + 39) >= 0xF8)
            reference = first;
        else
        {
            uint32_t nearer = rrj_s32(rrj_read32(first + 324)) < rrj_s32(rrj_read32(second + 324));
            int32_t rank_delta = (int32_t)r_u8(first_descriptor + 39) - r_u8(second_descriptor + 39);
            uint32_t candidate = rrj_read32(0x8005B268 + 4 * nearer);
            uint32_t choose = nearer ^ (r_u8(descriptor + 39) < (uint32_t)((int32_t)r_u8(rrj_read32(candidate + 1084) + 39) - (int32_t)leaf_abs32(rank_delta) / 2));
            reference = rrj_read32(0x8005B268 + 4 * choose);
        }
    }
    if (type == 2)
    {
        int32_t relative;
        int32_t elapsed;
        int32_t speed = rrj_s32(rrj_read32(target));
        if (!(r_u8(actor + 928) & 0x10))
            return 0;
        if (r_u8(race + 57) >= 4)
            return rrj_read32(0x80053030 + 4 * course);
        elapsed = (rrj_s32(rrj_read32(race + 16) - ((uint32_t)rrj_u16(rrj_at(descriptor + 62, 2)) << 8)) / 300) - r_s16(0x80053146 + 4 * course);
        if (elapsed > 0)
        {
            int32_t divisor = r_s16(0x80053152 + 4 * course);
            int32_t reduction = divisor ? (elapsed / divisor + 1) * rrj_s32(rrj_read32(0x8005315C + 4 * course)) : 0;
            int32_t adjusted = rrj_s32(rrj_read32(0x800D86F8)) * (128 - reduction);
            int32_t floor = rrj_s32(rrj_read32(0x80053138 + 4 * course));
            adjusted = adjusted < 0 ? (adjusted + 127) >> 7 : adjusted >> 7;
            if (adjusted < floor)
                adjusted = floor;
            if (adjusted < speed)
            {
                speed = adjusted;
                rrj_write32(target, (uint32_t)speed);
            }
        }
        relative = (rrj_s32(rrj_read32(actor + 324) - rrj_read32(reference + 324))) >> 12;
        if (r_s16(actor + 320) && relative >= -63 && relative < 12)
            rrj_put16(rrj_at(actor + 320, 2), rrj_u16(rrj_at(actor + 320, 2)) | 2);
        if (rrj_u16(rrj_at(contact, 2)) == 4 && relative < 0 && (rrj_u16(rrj_at(contact + 2, 2)) >> 5))
            return rrj_read32(0x80053138 + 4 * course);
        return (uint32_t)speed;
    }
    if (r_u8(race + 57) == 1 && rrj_read32(0x800CCC18))
        return rrj_read32(rrj_read32(0x800CCC18) + 480);
    if (r_u8(race + 57) == 3)
    {
        uint32_t primary_descriptor = rrj_read32(rrj_read32(0x8005B38C) + 1084);
        if (type == (uint32_t)(r_u8(primary_descriptor + 1) & 0x0F))
            return 0;
    }
    {
        int32_t rank_delta = (int32_t)r_u8(rrj_read32(reference + 1084) + 39) - r_u8(descriptor + 39);
        uint32_t mode = r_u8(race + 4);
        uint32_t table_index;
        uint32_t curve;
        int32_t position;
        int32_t speed;
        if ((int32_t)leaf_abs32(rank_delta) > rrj_s32(rrj_read32(0x8005B1F8)))
            return 0;
        if ((mode & 1) && (rrj_read32(0x8005AD48) & 1))
            return rrj_read32((rrj_read32(0x8005B2B0) ? 0x80053054 : 0x8005303C) + 4 * course);
        table_index = (mode & 4) ? course + 6 : course + ((0u - (mode & 1)) & 3);
        curve = rrj_read32(0x80052FD0 + 4 * table_index);
        if (rrj_s32(rrj_read32(race + 16)) < rrj_s32(((300u * curve) >> 16) + 300))
        {
            xport_update_u8(descriptor, XPORT_MEMORY_UPDATE_OR, 1);
            if (rrj_s32(rrj_read32(actor + 704)) > 0)
            {
                int32_t timer = rrj_s32(rrj_read32(actor + 704) - delta);
                if (timer > 0)
                    rrj_write32(actor + 704, (uint32_t)timer);
                else
                {
                    rrj_write32(actor + 704, 0);
                    rrj_write32(actor + 564, rrj_read32(actor + 564) & 0xFFFFFDFFu);
                }
            }
            else
            {
                uint32_t rank = r_u8(descriptor + 39);
                uint32_t interval = rrj_read32(0x80052FF4 + 4 * table_index);
                if ((((rank - 1) / 2 + 1) * ((300u * interval) >> 16)) < rrj_read32(race + 16) && !(r_u8(descriptor) & 2))
                {
                    rrj_write32(actor + 564, rrj_read32(actor + 564) | 0x200);
                    rrj_write32(actor + 704, curve - (((rank - 1) / 2) * 2 * interval));
                    xport_update_u8(descriptor, XPORT_MEMORY_UPDATE_OR, 2);
                }
            }
            return rrj_read32(target);
        }
        xport_update_u8(descriptor, XPORT_MEMORY_UPDATE_AND, 0xFE);
        position = rrj_s32(rrj_read32(actor + 324));
        if (position <= 0)
            return 0xA0000;
        position = rrj_s32((uint32_t)(position - rrj_read32(reference + 324))) >> 12;
        speed = rrj_s32(rrj_read32(target));
        if (r_u8(race + 4) == 44 && (r_u8(descriptor) & 0x10))
            return (uint32_t)((r_u8(descriptor + 69) * speed) / 128);
        if (r_u8(descriptor) & 8)
        {
            int32_t comparison = rrj_s32(rrj_read32(reference + 480)) + 0xA0000;
            if (position > 0)
                comparison = rrj_s32(rrj_read32(reference + 480)) - 0x20000;
            if (speed - 0x20000 < comparison)
                rrj_write32(actor + 564, rrj_read32(actor + 564) | 0x200);
            return (uint32_t)comparison;
        }
        if (r_s16(actor + 320) && position >= -63 && position < 12)
        {
            int32_t product = r_u8(descriptor + 69) * speed;
            rrj_put16(rrj_at(actor + 320, 2), rrj_u16(rrj_at(actor + 320, 2)) | 2);
            xport_update_u8(actor + 928, XPORT_MEMORY_UPDATE_OR, 8);
            return (uint32_t)(product < 0 ? (product + 127) >> 7 : product >> 7);
        }
        if (r_u8(actor + 928) & 8)
        {
            xport_update_u8(actor + 928, XPORT_MEMORY_UPDATE_AND, 0xF7);
            w_u8(actor + 929, 0);
        }
        speed = (speed * ((int32_t)r_s8(actor + 929) + 128)) / 128;
        if (!r_s16(actor + 320))
            return (uint32_t)sub_8001FC90((uint32_t)speed, rrj_read32(0x80052FA0 + 4 * type));
        return (uint32_t)speed;
    }
}

uint32_t sub_800954A0(uint32_t actor, uint32_t delta, RRJRaceLeafCall call)
{
    uint32_t flags, dynamics, race, target, current, result;

    FUNCTION_MARKER(0x800954A0u, "RASHCDG.BIN");
    flags = rrj_read32(actor + 560);
    dynamics = rrj_read32(actor + 556);
    if (flags & 0x08000000u)
    {
        uint32_t track[8];
        uint32_t velocity = rrj_read32(actor + 480);
        uint32_t response, product, lateral, sign, direction, piece;
        int32_t magnitude;

        rrj_write32(actor + 924, rrj_read32(dynamics + 224));
        response = rrj_read32(dynamics + 420);
        product = (uint32_t)sub_8001FC90(response, velocity);
        lateral = rrj_read32(actor + 344);
        sign = (uint32_t)(rrj_s32(lateral) >> 31);
        magnitude = rrj_s32((((lateral + sign) ^ sign) << 1) + product);
        if (magnitude < 0xF0000)
            magnitude = 0xF0000;
        if (magnitude > 0x500000)
            magnitude = 0x500000;
        if (r_u8(rrj_read32(actor + 1084)) & 0x80)
            direction = 0u - rrj_read32(actor + 364);
        else
            direction = rrj_read32(actor + 364);
        (void)sub_800386DC(actor, (uint32_t)magnitude, rrj_read32(actor + 348), direction,
                         rrj_at(actor + 328, 32), rrj_at(actor + 880, 12), track);
        piece = rrj_u32((const uint8_t *)track + 12);
        result = sub_800B6AAC(rrj_at(actor + 880, 12), rrj_at(piece + 2, 6), rrj_at(piece + 20, 12));
        rrj_put16(rrj_at(actor + 878, 2), (uint16_t)(rrj_s32(result) >> 5));
        piece = rrj_u32((const uint8_t *)track + 12);
        rrj_put16(rrj_at(actor + 872, 2), r_u16(piece + 2));
        piece = rrj_u32((const uint8_t *)track + 12);
        rrj_put16(rrj_at(actor + 874, 2), r_u16(piece + 4));
        piece = rrj_u32((const uint8_t *)track + 12);
        rrj_put16(rrj_at(actor + 876, 2), r_u16(piece + 6));
    }
    race = rrj_read32(0x8005B2F8u);
    if (r_u16(actor + 172) >= rrj_read32(race + 48))
        target = sub_80095BF8(actor, delta);
    else
    {
        rrj_put16(rrj_at(actor + 320, 2), r_u16(actor + 320) | 2u);
        if (!(rrj_read32(actor + 560) & 0x08000000u))
            return 0;
        if (r_u8(race + 4) & 1u)
        {
            uint32_t player, desired;
            if (!sub_80096818(actor, delta, call))
                return 0x800D0000u;
            player = 0x800CD898u + 1132u * r_u16(actor + 172);
            result = rrj_read32(player + 540);
            desired = rrj_read32(player + 544);
            if (result != desired)
            {
                (void)sub_8008A998(player, desired);
                player = 0x800CD898u + 1132u * r_u16(actor + 172);
                (void)sub_80086AF8(player);
            }
        }
        {
            uint32_t factor = r_u8(rrj_read32(actor + 1084) + 69);
            uint32_t product = factor * rrj_read32(dynamics + 224);
            if (rrj_s32(product) < 0)
                product += 127;
            target = (uint32_t)(rrj_s32(product) >> 7);
        }
    }
    current = rrj_read32(actor + 924);
    result = rrj_s32(target) < rrj_s32(current);
    if (result)
        current = target;
    rrj_write32(actor + 924, current);
    return result;
}

uint32_t sub_800BA304(uint32_t delta, uint32_t skip_mask, uint32_t active_out, uint32_t marked_out, RRJRaceLeafCall call)
{
    uint32_t count = rrj_read32(0x8005B1F8);
    uint32_t actor = rrj_read32(0x8005B3A0);
    uint32_t index;
    FUNCTION_MARKER(0x800BA304, "RASHCDG.BIN");
    for (index = 0; index < count; ++index, actor += 1096)
    {
        uint32_t bit = 1u << (index & 31);
        uint32_t state;
        uint32_t descriptor;
        if (skip_mask & bit)
            continue;
        state = rrj_u16(rrj_at(actor + 948 + 8 * (uint32_t)(int32_t)(r_s8(actor + 946) - 1), 2));
        if (rrj_read32(actor + 564) & 0x200)
            rrj_write32(marked_out, rrj_read32(marked_out) | bit);
        if (state - 3 < 15)
        {
            (void)sub_800954A0(actor, delta, call);
            if (state < 4 || !sub_800BD388(actor))
                continue;
            rrj_write32(active_out, rrj_read32(active_out) | bit);
            if (state < 5)
                continue;
            descriptor = rrj_read32(actor + 1084);
            if (rrj_read32(descriptor + 40) || r_u8(descriptor + 39) >= 0xF8)
                continue;
            sub_800BC8DC(actor);
        }
        else
        {
            descriptor = rrj_read32(actor + 1084);
            if (rrj_read32(descriptor + 40) && rrj_s32(rrj_read32(actor + 480)) >= 132 && (rrj_read32(actor + 560) & 0x08000000) && r_s16(actor + 320))
                (void)sub_800954A0(actor, delta, call);
        }
    }
    return index < count;
}

static void race_leaf_copy9(RRJMemory *m, uint32_t source, uint32_t destination)
{
    uint32_t index;
    for (index = 0; index < 9; ++index)
        rrj_put16(rrj_at(destination + 2 * index, 2), rrj_u16(rrj_at(source + 2 * index, 2)));
}

static void race_leaf_heading(RRJMemory *m, uint32_t object)
{
    uint32_t angle;
    uint32_t table;
    (void)sub_8008BA18(object);
    angle = sub_80020018((uint32_t)(int32_t)r_s16(object + 444), (uint32_t)(int32_t)r_s16(object + 448));
    rrj_write32(object + 292, angle);
    table = 0x8005624C + 4 * (angle & 0xFFF);
    rrj_write32(object + 296, (uint32_t)((int32_t)r_s16(table + 2) << 4));
    rrj_write32(object + 300, (uint32_t)((int32_t)r_s16(table) << 4));
}

uint32_t sub_800C9420(uint32_t group, RRJRaceLeafCall call)
{
    const uint32_t specification = 0x1F800010;
    const uint32_t position = 0x1F800050;
    const uint32_t geometry = 0x1F800060;
    const uint32_t vector = 0x1F800090;
    const uint32_t matrix = 0x1F8000C0;
    uint32_t list = rrj_read32(0x8005B340 + 4 * group);
    int32_t count = rrj_s32(rrj_read32(0x8005B208 + 4 * group));
    uint32_t current = rrj_read32(0x8005B38C);
    uint32_t actor_base = rrj_read32(0x8005B3A0);
    uint32_t player_count = rrj_read32(0x8005B1F8);
    uint32_t remaining[2];
    uint32_t actor_index = 0;
    uint32_t actor_offset = 0;
    uint32_t options = 1;
    uint32_t entry_offset = 0;
    int32_t entry_index;
    FUNCTION_MARKER(0x800C9420, "RASHCDG.BIN");
    remaining[0] = rrj_u16(rrj_at(0x8005B218, 2));
    remaining[1] = remaining[0];
    rrj_write32(0x800CCC18, 0);
    if (count <= 0)
        return 0x800D0000;
    for (entry_index = 0; entry_index < count; ++entry_index)
    {
        uint32_t entry = list + entry_offset;
        uint32_t packed = rrj_read32(entry + 8);
        uint32_t kind = (packed >> 12) & 15;
        int32_t selector = (int32_t)packed >> 16;
        int32_t angle = -(((int32_t)(packed << 20) >> 8) / 360);
        if (rrj_s32(rrj_read32(current + 364)) < 0)
        {
            rrj_write32(entry + 4, 0u - rrj_read32(entry + 4));
            rrj_write32(entry, 0u - rrj_read32(entry));
            angle = (angle + 2048) % 4096;
        }
        if (kind == 0)
        {
            uint32_t wanted = (r_u8(rrj_read32(current + 1084) + 1) & 15) == 0;
            uint32_t candidate_kind = 2;
            if (actor_index < player_count)
            {
                do
                {
                    uint32_t candidate = actor_base + 1096 * (actor_index + 1);
                    if (candidate_kind == wanted)
                        break;
                    candidate_kind = r_u8(rrj_read32(candidate + 1084) + 1) & 15;
                    actor_offset += 1096;
                    ++actor_index;
                } while (actor_index < player_count);
            }
            {
                uint32_t actor = actor_base + actor_offset;
                uint32_t secondary;
                if (!rrj_read32(0x8005B330))
                    rrj_write32(0x8005B330, actor);
                if (!group)
                    rrj_write32(rrj_read32(actor + 556) + 224, 0x00141DDD);
                rrj_write32(actor + 360, rrj_u16(rrj_at(0x80053178, 2)));
                rrj_write32(actor + 368, (rrj_u16(rrj_at(0x8005317A, 2)) << 16) + rrj_read32(entry + 4));
                rrj_write32(actor + 364, rrj_read32(current + 364));
                rrj_write32(actor + 292, (uint32_t)angle);
                rrj_put16(rrj_at(actor + 320, 2), 1);
                rrj_write32(actor + 344, rrj_read32(entry));
                (void)sub_8009432C(actor);
                if (group)
                {
                    secondary = rrj_read32(actor + 852);
                    rrj_put16(rrj_at(actor + 966, 2), 224);
                    w_u8(actor + 946, 2);
                    rrj_put16(rrj_at(actor + 964, 2), 0);
                    rrj_write32(secondary + 604, 4);
                }
            }
        }
        else if (kind == 1)
        {
            uint32_t current_kind = r_u8(rrj_read32(current + 1084) + 1) & 15;
            uint32_t high_group = selector >= 17;
            uint32_t desired = current_kind ? (high_group ? 0 : current_kind) : high_group;
            uint32_t side = high_group ? 0 : 1;
            uint32_t rider;
            uint32_t body;
            uint32_t source;
            uint32_t event;
            uint32_t segment;
            while (remaining[side] > 0)
            {
                uint32_t candidate = actor_base + 1096 * (remaining[side] - 1);
                uint32_t candidate_kind = r_u8(rrj_read32(candidate + 1084) + 1) & 15;
                if (candidate_kind == desired)
                    break;
                --remaining[side];
            }
            rider = actor_base + 1096 * remaining[side];
            body = rrj_read32(rider + 852);
            if (rrj_read32(rider + 856))
            {
                uint32_t linked = rrj_read32(current + 856);
                uint32_t linked_body = rrj_read32(linked + 852);
                xport_update_u8(linked_body + 572, XPORT_MEMORY_UPDATE_OR, 0x60);
                xport_update_u8(rrj_read32(current + 852) + 572, XPORT_MEMORY_UPDATE_OR, 0x10);
            }
            if (r_s8(body + 72) == 1)
                (void)sub_80068D20(rider, body, rrj_read32(rider + 856) != 0);
            w_u8(body + 72, 3);
            rrj_write32(position, rrj_u16(rrj_at(0x80053178, 2)));
            rrj_write32(position + 4, rrj_read32(current + 364));
            rrj_write32(position + 8, (rrj_u16(rrj_at(0x8005317A, 2)) << 16) + rrj_read32(entry + 4));
            rrj_write32(body + 360, rrj_read32(position));
            rrj_write32(body + 364, rrj_read32(position + 4));
            rrj_write32(body + 368, rrj_read32(position + 8));
            rrj_write32(body + 344, rrj_read32(entry));
            segment = sub_80039DFC(0, position);
            if (!segment || !sub_8003A700(segment, position, geometry))
                goto next_entry;
            (void)sub_8001E0B4(body + 328, geometry, 32);
            source = rrj_read32(geometry + 12);
            (void)sub_8002EAD8(rrj_at(source + 20, 12), rrj_at(source + 14, 6), rrj_read32(geometry + 20), rrj_at(body + 184, 12));
            (void)sub_8002EAD8(rrj_at(body + 184, 12), rrj_at(source + 2, 6), rrj_read32(entry), rrj_at(body + 184, 12));
            rrj_write32(body + 508, 0);
            rrj_put16(rrj_at(body + 438, 2), (uint16_t)-r_s16(source + 8));
            rrj_put16(rrj_at(body + 440, 2), (uint16_t)-r_s16(source + 10));
            rrj_put16(rrj_at(body + 442, 2), (uint16_t)-r_s16(source + 12));
            rrj_put16(rrj_at(body + 444, 2), rrj_u16(rrj_at(source + 14, 2)));
            rrj_put16(rrj_at(body + 446, 2), rrj_u16(rrj_at(source + 16, 2)));
            rrj_put16(rrj_at(body + 448, 2), rrj_u16(rrj_at(source + 18, 2)));
            rrj_put16(rrj_at(body + 432, 2), rrj_u16(rrj_at(source + 2, 2)));
            rrj_put16(rrj_at(body + 434, 2), rrj_u16(rrj_at(source + 4, 2)));
            rrj_put16(rrj_at(body + 436, 2), rrj_u16(rrj_at(source + 6, 2)));
            (void)sub_8003FB34(source + 8, (uint32_t)angle, matrix);
            (void)sub_8003FA40(body + 432, matrix, body + 432);
            rrj_put16(rrj_at(body + 450, 2), rrj_u16(rrj_at(body + 444, 2)));
            rrj_put16(rrj_at(body + 452, 2), rrj_u16(rrj_at(body + 446, 2)));
            rrj_put16(rrj_at(body + 454, 2), rrj_u16(rrj_at(body + 448, 2)));
            race_leaf_copy9(rrj_host_context(), body + 432, body + 516);
            race_leaf_heading(rrj_host_context(), body);
            rrj_put16(rrj_at(body + 320, 2), 1);
            (void)sub_80037450(body);
            if (desired == current_kind)
            {
                event = 66;
                if ((rrj_u16(rrj_at(body + 172, 2)) & 2) && !(r_u8(body + 572) & 0x40))
                {
                    uint32_t descriptor = rrj_read32(rrj_read32(body + 596) + 1084);
                    w_u8(descriptor + 46, 5);
                    w_u8(descriptor + 47, 0);
                    (void)sub_800958F0(body, 0);
                    event = 65;
                }
                (void)sub_800302C4(body, 151);
                rrj_write32(body + 36, (rrj_read32(body + 36) & 0xFFFC0FFF) | 0x00029000);
            }
            else
            {
                uint32_t descriptor = rrj_read32(rrj_read32(body + 596) + 1084);
                w_u8(descriptor + 46, 2);
                w_u8(descriptor + 47, 0);
                (void)sub_800958F0(body, 0);
                event = 64;
            }
            rrj_put16(rrj_at(rider + 966, 2), 224);
            rrj_put16(rrj_at(rider + 964, 2), 0);
            w_u8(rider + 946, 2);
            options |= 0x800 | (((3 * (sub_80043F00(0xF2000002) & 0xFF) >> 7) + 5) << 16);
            (void)sub_800C4550(event, body, options);
        }
        else if (kind == 3)
        {
            uint32_t object;
            uint32_t source;
            rrj_write32(specification + 8, rrj_u16(rrj_at(0x80053178, 2)));
            rrj_write32(specification + 36, (rrj_u16(rrj_at(0x8005317A, 2)) << 16) + rrj_read32(entry + 4));
            rrj_put16(rrj_at(specification + 2, 2), (uint16_t)selector);
            rrj_put16(rrj_at(specification + 60, 2), rrj_u16(rrj_at(current + 364, 2)));
            rrj_put16(rrj_at(specification + 64, 2), (uint16_t)(3 - 2 * (entry_index & 1)));
            object = sub_8009AD48(specification, current, call);
            if (!object)
                goto next_entry;
            if (!group)
            {
                rrj_write32(object + 484, 0x00026666);
                rrj_write32(object + 480, 0);
                if (!rrj_read32(0x800CCC18))
                    rrj_write32(0x800CCC18, object);
                goto next_entry;
            }
            source = rrj_read32(object + 340);
            rrj_write32(object + 484, 0);
            rrj_write32(object + 480, 0);
            (void)sub_8002EAD8(rrj_at(rrj_read32(object + 340) + 20, 12), rrj_at(rrj_read32(object + 340) + 14, 6), rrj_read32(object + 348), rrj_at(object + 184, 12));
            (void)sub_8002EAD8(rrj_at(object + 184, 12), rrj_at(source + 2, 6), rrj_read32(entry), rrj_at(object + 184, 12));
            (void)sub_8003FB34(source + 8, (uint32_t)angle, matrix);
            (void)sub_8003FA40(object + 432, matrix, object + 432);
            rrj_put16(rrj_at(object + 450, 2), rrj_u16(rrj_at(object + 444, 2)));
            rrj_put16(rrj_at(object + 452, 2), rrj_u16(rrj_at(object + 446, 2)));
            rrj_put16(rrj_at(object + 454, 2), rrj_u16(rrj_at(object + 448, 2)));
            race_leaf_heading(rrj_host_context(), object);
        }
        else if (kind == 4)
        {
            uint32_t segment;
            uint32_t source;
            (void)sub_8001E100(specification, 0, 64);
            rrj_write32(specification + 8, rrj_u16(rrj_at(0x80053178, 2)));
            rrj_write32(specification + 36, (rrj_u16(rrj_at(0x8005317A, 2)) << 16) + rrj_read32(entry + 4));
            rrj_put16(rrj_at(specification + 2, 2), (uint16_t)selector);
            rrj_put16(rrj_at(specification + 12, 2), 3);
            rrj_put16(rrj_at(specification + 60, 2), rrj_u16(rrj_at(current + 364, 2)));
            rrj_put16(rrj_at(specification + 62, 2), (uint16_t)selector);
            rrj_write32(position, rrj_read32(specification + 8));
            rrj_write32(position + 4, (uint32_t)(int32_t)r_s16(specification + 60));
            rrj_write32(position + 8, rrj_read32(specification + 36));
            segment = sub_80039DFC(0, position);
            if (segment && sub_8003A700(segment, position, geometry))
            {
                source = rrj_read32(geometry + 12);
                (void)sub_8002EAD8(rrj_at(source + 20, 12), rrj_at(source + 14, 6), rrj_read32(geometry + 20), rrj_at(specification + 20, 12));
                (void)sub_8002EAD8(rrj_at(specification + 20, 12), rrj_at(source + 2, 6), rrj_read32(entry), rrj_at(specification + 20, 12));
                (void)sub_8003FB34(source + 8, (uint32_t)angle, matrix);
                (void)sub_8002EFF4(source + 14, matrix, vector);
                rrj_put16(rrj_at(specification + 14, 2), rrj_u16(rrj_at(vector, 2)));
                rrj_put16(rrj_at(specification + 16, 2), rrj_u16(rrj_at(vector + 2, 2)));
                rrj_put16(rrj_at(specification + 18, 2), rrj_u16(rrj_at(vector + 4, 2)));
                (void)sub_800A2630(specification, current, call);
            }
        }
    next_entry:
        entry_offset += 12;
    }
    return 0;
}

uint32_t sub_800C8D4C(RRJRaceLeafCall call, RRJReverbCall reverb)
{
    FUNCTION_MARKER(0x800C8D4C, "RASHCDG.BIN");
    return sub_800C8D5C(call, reverb);
}

uint32_t sub_800C8D5C(RRJRaceLeafCall call, RRJReverbCall reverb)
{
    uint32_t race = rrj_read32(0x8005B2F8);
    uint32_t current = rrj_read32(0x8005B38C);
    uint32_t descriptor = rrj_read32(current + 1084);
    uint32_t event = 0x1F8003D0;
    uint32_t active = 0;
    int32_t offset = (int32_t)r_s8(0x8005ADF0) << 16;
    FUNCTION_MARKER(0x800C8D5C, "RASHCDG.BIN");
    switch (r_u8(race + 57))
    {
        case 1:
        {
            uint32_t time = rrj_read32(race + 16);
            rrj_write32(current + 704, 0);
            rrj_write32(current + 564, rrj_read32(current + 564) & 0xFFFFFDFF);
            active = rrj_s32(rrj_read32(0x8005ACC8)) < rrj_s32(time);
            if (active)
            {
                rrj_write32(descriptor + 40, time);
                w_u8(descriptor + 39, 0xFA);
            }
            (void)sub_800C9420(active ^ 1, call);
            if (active)
            {
                uint32_t player = 0x800CD898 + 1132 * rrj_u16(rrj_at(current + 172, 2));
                uint32_t leader = rrj_read32(0x8005B330);
                rrj_write32(player + 776, 6);
                rrj_write32(player + 752, leader + 184);
                rrj_write32(player + 756, leader + 320);
                rrj_write32(player + 552, rrj_read32(player + 552) | 0x12);
                (void)sub_8008A998(player, 12);
                rrj_write32(current + 560, rrj_read32(current + 560) | 0x20000000);
            }
            return active;
        }
        case 2:
        {
            uint32_t contact;
            uint32_t state;
            rrj_write32(current + 704, 0);
            rrj_write32(current + 924, 0);
            rrj_write32(current + 560, rrj_read32(current + 560) | 0x28000000);
            rrj_write32(current + 564, rrj_read32(current + 564) & 0xFFFFFDFF);
            contact = current + 8 * (uint32_t)(int32_t)(r_s8(current + 946) - 1);
            state = rrj_u16(rrj_at(contact + 956, 2));
            if (!state)
                return 0;
            if (state == 18)
                return 1;
            (void)sub_800BCD10(current);
            rrj_put16(rrj_at(event, 2), 2);
            rrj_put16(rrj_at(event + 2, 2), 224);
            (void)sub_800BCA68(event, 0, current);
            w_u8(descriptor + 69, 64);
            (void)sub_8002EAD8(rrj_at(rrj_read32(current + 340) + 20, 12), rrj_at(rrj_read32(current + 340) + 14, 6), rrj_read32(current + 348), rrj_at(0x800CE5A8, 12));
            {
                uint32_t player = 0x800CD898 + 1132 * rrj_u16(rrj_at(current + 172, 2));
                uint32_t linked_body = rrj_read32(rrj_read32(current + 856) + 852);
                rrj_write32(player + 776, 3);
                rrj_write32(player + 752, linked_body + 184);
                rrj_write32(player + 756, linked_body + 320);
                rrj_write32(player + 552, rrj_read32(player + 552) | 0x12);
                (void)sub_8008A998(player, 12);
            }
            return 0;
        }
        case 3:
        {
            int32_t direction = rrj_u16(rrj_at(0x80053186, 2)) < rrj_u16(rrj_at(0x80053182, 2)) ? -1 : 1;
            uint32_t kind = r_u8(descriptor + 1) & 15;
            uint32_t count_pointer = rrj_read32(0x800CE4DC);
            int32_t count = rrj_s32(rrj_read32(count_pointer));
            uint32_t actor = rrj_read32(0x800CE4D0);
            uint32_t stride = rrj_read32(0x800CE4D4);
            rrj_write32(current + 560, rrj_read32(current + 560) & 0xD7FFFFFF);
            w_u8(descriptor + 69, r_u8(0x8005ADF1));
            rrj_write32(0x8005ACC0, 1);
            rrj_write32(0x8005B2E8, (uint32_t)direction);
            if (direction < 0)
                offset = -offset;
            while (count >= 0)
            {
                uint32_t actor_descriptor = rrj_read32(actor + 1084);
                if ((uint32_t)(r_u8(actor_descriptor + 1) & 15) == kind && actor != current)
                {
                    uint32_t secondary;
                    uint32_t animation;
                    rrj_write32(actor + 360, rrj_u16(rrj_at(0x80053180, 2)));
                    rrj_write32(actor + 364, (uint32_t)direction);
                    rrj_write32(actor + 924, 0);
                    rrj_write32(actor + 480, 0);
                    rrj_write32(actor + 368, (rrj_u16(rrj_at(0x80053182, 2)) << 16) + (uint32_t)offset);
                    w_u8(actor_descriptor + 69, r_u8(0x8005ADF1));
                    rrj_write32(actor + 704, 0);
                    rrj_write32(actor + 564, rrj_read32(actor + 564) & 0xFFFFFDFF);
                    offset += direction < 0 ? -393216 : 393216;
                    (void)sub_800903F4(actor, 1, reverb);
                    secondary = rrj_read32(actor + 852);
                    rrj_put16(rrj_at(actor + 956, 2), 1);
                    animation = r_u8(0x80054114 + rrj_read32(secondary + 180));
                    (void)sub_800302C4(secondary, animation);
                    rrj_write32(secondary + 36, (rrj_read32(secondary + 36) & 0xFFFC0FFF) | ((kind ? 4 : 8) << 12));
                }
                --count;
                actor += stride;
            }
            return 0;
        }
        case 4:
        {
            uint32_t count_pointer = rrj_read32(0x800CE4DC);
            int32_t count = rrj_s32(rrj_read32(count_pointer));
            uint32_t actor = rrj_read32(0x800CE4D0);
            uint32_t stride = rrj_read32(0x800CE4D4);
            uint32_t kind = r_u8(descriptor + 1) & 15;
            rrj_write32(0x8005ACC0, 0);
            rrj_write32(0x8005ACC4, 0);
            rrj_write32(current + 560, rrj_read32(current + 560) | 0x28000000);
            while (count >= 0)
            {
                uint32_t actor_descriptor = rrj_read32(actor + 1084);
                if ((uint32_t)(r_u8(actor_descriptor + 1) & 15) != kind)
                {
                    (void)sub_800BCD10(actor);
                    rrj_put16(rrj_at(event, 2), 2);
                    rrj_put16(rrj_at(event + 2, 2), 224);
                    (void)sub_800BCA68(event, 1, actor);
                }
                --count;
                actor += stride;
            }
            (void)sub_8008A998(0x800CD898 + 1132 * rrj_u16(rrj_at(current + 172, 2)), 10);
            return 0;
        }
        case 5:
            rrj_write32(descriptor + 40, rrj_read32(race + 16));
            w_u8(descriptor + 39, 0xF9);
            rrj_write32(0x800CDAC0, rrj_read32(0x800CDAC0) | 0x14);
            return 0;
        default:
            return 0;
    }
}

static uint32_t race_leaf_checkpoint_progress(RRJMemory *m, uint32_t actor, uint32_t secondary)
{
    uint32_t vector = 0x1F8000E0;
    uint32_t output = 0x1F8000F0;
    if (rrj_read32(secondary + 604) >= 3)
        return rrj_read32(secondary + 368);
    if (!r_s16(actor + 320))
        return rrj_read32(actor + 368);
    (void)sub_8002EAD8(rrj_at(actor + 504, 12), rrj_at(actor + 528, 6), rrj_read32(actor + 308), rrj_at(vector, 12));
    sub_80036800(vector, rrj_read32(actor + 340), 0, output);
    return rrj_read32(rrj_read32(actor + 340) + 40) + rrj_read32(output);
}

static uint32_t race_leaf_checkpoint_crossed(RRJMemory *m, uint32_t path, uint32_t delta, uint32_t checkpoint)
{
    uint32_t required = rrj_read32(checkpoint + 12);
    if ((!path || rrj_read32(path) != required) && required != 0xFFFFFFFF)
        return 0;
    return rrj_s32(rrj_read32(checkpoint + 8) ^ delta) >= 0;
}

uint32_t sub_800B9958(uint32_t actor, RRJRaceLeafCall call, RRJReverbCall reverb)
{
    uint32_t race;
    uint32_t descriptor;
    uint32_t secondary;
    uint32_t finished = 0;
    uint32_t stage_match = 0;
    uint32_t progress = 0;
    int32_t last_delta = 0;
    FUNCTION_MARKER(0x800B9958, "RASHCDG.BIN");
    if (!actor)
        return 0;
    race = rrj_read32(0x8005B2F8);
    descriptor = rrj_read32(actor + 1084);
    secondary = rrj_read32(actor + 852);
    if (rrj_read32(descriptor + 40))
    {
        if (rrj_u16(rrj_at(actor + 172, 2)) < rrj_read32(race + 48) && !(rrj_read32(actor + 560) & 0x08000000))
            rrj_write32(actor + 560, rrj_read32(actor + 560) | 0x08000000);
        return 1;
    }
    if (rrj_u16(rrj_at(actor + 172, 2)) < rrj_read32(race + 48) && r_u8(race + 4) != 33)
    {
        uint32_t phase = r_u8(race + 4);
        uint32_t time = rrj_read32(race + 16);
        uint32_t mode = rrj_read32(0x8005B220);
        if (mode && rrj_s32(mode) < 3 && rrj_s32(time) > 108000)
            finished = 1;
        else if ((phase == 36 && rrj_s32(rrj_read32(0x8005ACC8)) < rrj_s32(time)) || ((phase & 1) && rrj_s32(rrj_read32(0x8005ACC8) - (time - rrj_read32(0x8005ACD0))) <= 0))
        {
            finished = 1;
            w_u8(descriptor + 39, 0xFA);
        }
        else if (phase & 4)
        {
            if (phase == 44)
                finished = rrj_s32(time) > 216000;
            else
                finished = rrj_s32(time) > 108000;
        }
    }
    if (!finished)
    {
        if (r_s16(0x800D6182) != -1)
        {
            uint32_t stage = r_u8(race + 57);
            uint32_t schedule = 0x80053174 + 4 * stage;
            uint32_t route;
            uint32_t path;
            uint32_t crossed;
            if (r_u8(race + 4) == 44 && rrj_u16(rrj_at(actor + 172, 2)) < rrj_read32(race + 48))
            {
                if (rrj_u16(rrj_at(schedule, 2)) == rrj_read32(actor + 360))
                    stage_match = 2;
                if (!stage && rrj_s32(rrj_read32(0x8005ACC8)) < rrj_s32(rrj_read32(race + 16)) && !(rrj_read32(actor + 560) & 0x08000000))
                {
                    int32_t difference = (int16_t)rrj_u16(rrj_at(actor + 370, 2)) - (int32_t)rrj_u16(rrj_at(schedule + 2, 2));
                    if (stage_match && rrj_s32(leaf_abs32(difference)) < 400)
                        rrj_write32(actor + 560, rrj_read32(actor + 560) | 0x28000000);
                    else
                    {
                        w_u8(descriptor + 39, 0xFA);
                        finished = 1;
                    }
                }
            }
            if (rrj_read32(secondary + 604) >= 3)
            {
                route = secondary + 360;
                path = rrj_read32(secondary + 428);
            }
            else
            {
                route = actor + 360;
                path = rrj_read32(actor + 428);
            }
            crossed = 0;
            if (rrj_read32(route) == rrj_read32(rrj_read32(0x800D6188)) && path)
                crossed = (rrj_u16(rrj_at(path + 118, 2)) >> (rrj_u16(rrj_at(actor + 172, 2)) & 31)) & 1;
            crossed |= stage_match;
            if (crossed)
            {
                uint32_t checkpoint = rrj_read32(0x800D6188);
                progress = race_leaf_checkpoint_progress(rrj_host_context(), actor, secondary);
                if (crossed & 1)
                {
                    uint32_t delta = progress - rrj_read32(checkpoint + 4);
                    last_delta = rrj_s32(delta);
                    finished = race_leaf_checkpoint_crossed(rrj_host_context(), path, delta, checkpoint);
                    if (r_u8(secondary + 572) & 0x10)
                    {
                        uint32_t linked_body = rrj_read32(rrj_read32(actor + 856) + 852);
                        uint32_t state = rrj_read32(linked_body + 604);
                        if (state - 3 < 2 && !(rrj_read32(linked_body + 360) >> 16) && (uint16_t)rrj_read32(linked_body + 360) == rrj_read32(checkpoint))
                        {
                            uint32_t linked_delta = rrj_read32(linked_body + 368) - rrj_read32(checkpoint + 4);
                            finished |= race_leaf_checkpoint_crossed(rrj_host_context(), rrj_read32(linked_body + 428), linked_delta, checkpoint);
                        }
                    }
                }
                if (crossed & 2)
                {
                    uint32_t direction = rrj_read32(actor + 364);
                    uint32_t delta = progress - (rrj_u16(rrj_at(schedule + 2, 2)) << 16);
                    last_delta = rrj_s32(delta);
                    if (rrj_s32(direction ^ rrj_read32(0x8005B2E8)) >= 0 && rrj_s32(direction ^ delta) >= 0)
                    {
                        xport_update_u8(race + 57, XPORT_MEMORY_UPDATE_ADD, 1);
                        finished = sub_800C8D4C(call, reverb);
                    }
                    if (r_u8(race + 57) == 1)
                        sub_800C92F8();
                }
            }
        }
        else if (r_s16(actor + 320))
        {
            uint32_t track = rrj_read32(actor + 340);
            int16_t piece = r_s16(track);
            if ((!piece || piece == r_s16(rrj_read32(actor + 336) + 10) - 1) && sub_800394F0(rrj_at(actor + 328, 32), rrj_read32(actor + 364)))
                finished = 1;
        }
    }
    if (!finished)
        return 0;
    if (rrj_u16(rrj_at(actor + 172, 2)) < rrj_read32(race + 48))
    {
        uint32_t phase = r_u8(race + 4);
        uint32_t status = r_u8(descriptor + 39);
        if (!(rrj_read32(actor + 560) & 0x08000000))
        {
            rrj_write32(actor + 560, rrj_read32(actor + 560) | 0x08000000);
            rrj_write32(0x8005B230, 0);
        }
        if (phase == 36 && status < 0xF7 && rrj_s32(rrj_read32(0x8005ACC8)) >= rrj_s32(rrj_read32(race + 16)))
            status = 0xFB;
        else if (phase == 33)
            status = 0xFA;
        else
        {
            if ((phase & 1) && ((r_u8(descriptor + 1) & 15) == 2))
                status = 0xFA;
            if (r_u8(race + 57) == 1)
                status = 0xFA;
            else if (r_u8(race + 57) == 2)
                status = 0xFE;
            else if (phase & 4)
            {
                uint32_t time = rrj_read32(race + 16);
                rrj_write32(0x800D9C4C, time);
                if ((phase == 44 && rrj_s32(time) > 216000) || (phase != 44 && rrj_s32(time) > 108000))
                    status = 0xFA;
            }
        }
        w_u8(descriptor + 39, (uint8_t)status);
        if (phase == 17 && ((r_u8(descriptor + 1) & 15) != 2))
        {
            uint32_t other = rrj_read32(0x8005B268 + 4 * (rrj_u16(rrj_at(actor + 172, 2)) == 0));
            uint32_t other_descriptor = rrj_read32(other + 1084);
            if (!rrj_read32(other_descriptor + 40) && r_u8(other_descriptor + 39) < 0xF8)
                rrj_write32(other_descriptor + 40, rrj_read32(race + 16));
        }
    }
    if (r_u8(descriptor + 39) < 0xF7)
        w_u8(descriptor + 39, (uint8_t)sub_800138E8(actor, 1));
    rrj_write32(descriptor + 40, rrj_read32(race + 16));
    (void)sub_800BC7CC(actor);
    (void)(uint32_t)sub_8003F680(rrj_u16(rrj_at(actor + 172, 2)), 1);
    if (actor == rrj_read32(0x8005B21C) && !(r_u8(race + 4) & 1))
    {
        uint32_t current = rrj_read32(0x8005B38C);
        uint32_t current_descriptor = rrj_read32(current + 1084);
        uint32_t status = r_u8(descriptor + 39);
        if (rrj_read32(current_descriptor + 40) == rrj_read32(race + 16) && status < 0xF7)
        {
            uint32_t current_status = r_u8(current_descriptor + 39);
            if (current_status < 0xF7)
            {
                int32_t current_delta = rrj_s32(rrj_read32(current + 368) - rrj_read32(rrj_read32(0x800D6188) + 4));
                if (leaf_abs32(last_delta) < leaf_abs32(current_delta))
                {
                    w_u8(descriptor + 39, (uint8_t)current_status);
                    w_u8(current_descriptor + 39, (uint8_t)status);
                    rrj_write32(0x800D5DA8 + 16 * (current_status - 1), rrj_u16(rrj_at(actor + 172, 2)));
                    rrj_write32(0x800D5DA8 + 16 * (status - 1), rrj_u16(rrj_at(current + 172, 2)));
                }
            }
        }
    }
    if (rrj_read32(race + 48) == 2 && r_s16(actor + 320) && rrj_read32(secondary + 604) < 2 && rrj_u16(rrj_at(actor + 172, 2)) >= 2)
    {
        uint32_t first = rrj_read32(actor + 48);
        uint32_t second = rrj_read32(actor + 44);
        uint32_t minimum = rrj_s32(second) < rrj_s32(first) ? second : first;
        if (rrj_s32(minimum << 10) >= 3201)
        {
            rrj_put16(rrj_at(actor + 320, 2), 0);
            rrj_put16(rrj_at(secondary + 320, 2), 0);
        }
    }
    return 1;
}

uint32_t sub_800B9794(uint32_t inactive_mask, RRJRaceLeafCall call, RRJReverbCall reverb)
{
    int32_t count = rrj_s32(rrj_read32(0x8005B1F8));
    uint32_t actor = rrj_read32(0x8005B3A0);
    int32_t index;
    FUNCTION_MARKER(0x800B9794, "RASHCDG.BIN");
    if (count <= 0)
        return 0x80060000;
    for (index = 0; index < count; ++index, actor += 1096)
    {
        uint32_t secondary = rrj_read32(actor + 852);
        uint32_t descriptor = rrj_read32(actor + 1084);
        uint32_t mark = 0;
        uint32_t status;
        if (rrj_u16(rrj_at(actor + 172, 2)) >= rrj_read32(rrj_read32(0x8005B2F8) + 48) && ((r_u8(descriptor + 1) & 15) == 2) && !(r_u8(actor + 928) & 0x10))
        {
            mark = 1;
        }
        else
        {
            rrj_write32(secondary + 552, rrj_read32(secondary + 552) & 0xFDFFFFFF);
            rrj_write32(actor + 560, rrj_read32(actor + 560) & 0xFFFBFFFF);
            xport_update_u8(actor + 928, XPORT_MEMORY_UPDATE_AND, 0xBF);
            if (rrj_read32(descriptor + 40) || sub_800B9958(actor, call, reverb))
            {
                status = r_u8(descriptor + 39);
                if (status < 0xF7 || status == 0xFA || status == 0xFE || status == 0xFF)
                    rrj_write32(actor + 560, rrj_read32(actor + 560) | 0x08000000);
                if ((uint8_t)(status + 2) < 2)
                    mark = 1;
            }
            if (!mark && !r_s16(actor + 320) && (rrj_read32(secondary + 604) < 3 || !r_s16(secondary + 320)))
                mark = 1;
        }
        if (mark)
            rrj_write32(inactive_mask, rrj_read32(inactive_mask) | (1u << (index & 31)));
    }
    return 0;
}

static void race_leaf_track_pose(RRJMemory *m, uint32_t actor, uint32_t body)
{
    uint32_t track = rrj_read32(body + 340);
    uint32_t index;
    rrj_write32(actor + 504, rrj_read32(track + 20));
    rrj_write32(actor + 508, rrj_read32(track + 24));
    rrj_write32(actor + 512, rrj_read32(track + 28));
    for (index = 0; index < 3; ++index)
    {
        rrj_write32(actor + 184 + 4 * index, rrj_read32(actor + 504 + 4 * index));
        rrj_put16(rrj_at(actor + 528 + 2 * index, 2), rrj_u16(rrj_at(track + 14 + 2 * index, 2)));
        rrj_put16(rrj_at(actor + 522 + 2 * index, 2), (uint16_t)-r_s16(track + 8 + 2 * index));
        rrj_put16(rrj_at(actor + 516 + 2 * index, 2), rrj_u16(rrj_at(track + 2 + 2 * index, 2)));
    }
    (void)sub_8001E0B4(actor + 328, body + 328, 32);
    rrj_write32(actor + 428, rrj_read32(body + 428));
    sub_8003AF9C(actor + 172, 1, 0);
    (void)sub_80096564(actor);
    for (index = 0; index < 3; ++index)
    {
        rrj_put16(rrj_at(actor + 444 + 2 * index, 2), rrj_u16(rrj_at(actor + 528 + 2 * index, 2)));
        rrj_put16(rrj_at(actor + 432 + 2 * index, 2), rrj_u16(rrj_at(actor + 522 + 2 * index, 2)));
        rrj_put16(rrj_at(actor + 438 + 2 * index, 2), rrj_u16(rrj_at(actor + 516 + 2 * index, 2)));
    }
    if (r_s16(actor + 434) >= 0)
    {
        for (index = 0; index < 3; ++index)
            rrj_put16(rrj_at(actor + 438 + 2 * index, 2), (uint16_t)-r_s16(actor + 438 + 2 * index));
    }
    else
    {
        for (index = 0; index < 3; ++index)
            rrj_put16(rrj_at(actor + 432 + 2 * index, 2), (uint16_t)-r_s16(actor + 432 + 2 * index));
    }
    for (index = 0; index < 3; ++index)
        rrj_put16(rrj_at(actor + 450 + 2 * index, 2), rrj_u16(rrj_at(actor + 528 + 2 * index, 2)));
    (void)sub_8003662C(actor + 450, actor + 328, actor + 360);
    rrj_write32(actor + 456, 0);
    rrj_write32(actor + 460, 0);
    rrj_write32(actor + 464, 0);
    for (index = 0; index < 3; ++index)
        rrj_put16(rrj_at(actor + 814 + 2 * index, 2), rrj_u16(rrj_at(actor + 516 + 2 * index, 2)));
    (void)sub_8002090C(actor);
    rrj_write32(actor + 568, rrj_read32(actor + 568) | ((rrj_read32(actor + 568) & 0x100) ? 0x100 : 0x900));
    rrj_write32(actor + 652, r_s16(actor + 434) < 0 ? 0xFFFE6DE1 : 102943);
    rrj_write32(actor + 492, rrj_read32(body + 492));
    rrj_write32(actor + 496, rrj_read32(body + 496));
    (void)sub_8001E0B4(actor + 372, body + 372, 56);
    rrj_write32(actor + 324, rrj_read32(body + 324));
    rrj_write32(actor + 344, 0);
    rrj_write32(actor + 348, 0);
    rrj_put16(rrj_at(actor + 392, 2), 4);
    rrj_put16(rrj_at(actor + 394, 2), 1);
    rrj_write32(actor + 388, rrj_read32(actor + 388) & 0xFFFFFFFE);
    (void)sub_8003A9D8(actor);
}

static uint32_t race_leaf_reset_body(RRJMemory *m, uint32_t actor, uint32_t body)
{
    uint32_t track = rrj_read32(body + 340);
    uint32_t scale = rrj_read32(body + 304) + 2 * rrj_read32(actor + 304);
    uint32_t index;
    rrj_write32(actor + 480, 0);
    rrj_write32(body + 480, 0);
    rrj_write32(body + 488, 0);
    rrj_write32(body + 348, 0);
    rrj_write32(body + 344, scale);
    (void)sub_8002EAD8(rrj_at(track + 20, 12), rrj_at(track + 2, 6), scale, rrj_at(body + 184, 12));
    rrj_write32(body + 508, 0);
    for (index = 0; index < 3; ++index)
    {
        rrj_put16(rrj_at(body + 522 + 2 * index, 2), (uint16_t)-r_s16(track + 8 + 2 * index));
        rrj_put16(rrj_at(body + 516 + 2 * index, 2), rrj_u16(rrj_at(track + 14 + 2 * index, 2)));
        rrj_put16(rrj_at(body + 528 + 2 * index, 2), (uint16_t)-r_s16(track + 2 + 2 * index));
        rrj_put16(rrj_at(body + 432 + 2 * index, 2), rrj_u16(rrj_at(body + 522 + 2 * index, 2)));
        rrj_put16(rrj_at(body + 438 + 2 * index, 2), rrj_u16(rrj_at(body + 516 + 2 * index, 2)));
        rrj_put16(rrj_at(body + 444 + 2 * index, 2), rrj_u16(rrj_at(body + 528 + 2 * index, 2)));
        rrj_put16(rrj_at(body + 450 + 2 * index, 2), rrj_u16(rrj_at(body + 528 + 2 * index, 2)));
    }
    rrj_write32(body + 456, 0);
    rrj_write32(body + 460, 0);
    rrj_write32(body + 464, 0);
    sub_8003AF9C(body + 172, 1, 0);
    (void)sub_8003662C(body + 450, body + 328, body + 360);
    rrj_put16(rrj_at(body + 392, 2), 4);
    rrj_put16(rrj_at(body + 394, 2), 1);
    rrj_write32(body + 388, rrj_read32(body + 388) & 0xFFFFFFFE);
    return sub_8003A9D8(body);
}

uint32_t sub_80092E04(uint32_t actor, uint32_t delta, RRJReverbCall reverb)
{
    uint32_t race = rrj_read32(0x8005B2F8);
    uint32_t body = rrj_read32(actor + 852);
    uint32_t source = actor;
    uint32_t linked_mode = (r_u8(body + 572) >> 5) & 1;
    uint32_t player = rrj_u16(rrj_at(actor + 172, 2)) < rrj_read32(race + 48);
    uint32_t blocked = 0;
    uint32_t far = 0;
    uint32_t distance;
    uint32_t result = 0;
    FUNCTION_MARKER(0x80092E04, "RASHCDG.BIN");
    if (linked_mode)
        source = rrj_read32(rrj_read32(body + 596) + 856);
    if (r_u8(race + 57) == 2)
    {
        if (player)
        {
            uint32_t descriptor = rrj_read32(source + 1084);
            if (!rrj_read32(descriptor + 40) && r_u8(descriptor + 39) < 0xF8)
                return sub_800CA05C(1);
            return rrj_read32(descriptor + 40);
        }
        else
        {
            uint32_t contact = source + 8 * (uint32_t)(int32_t)r_s8(source + 946) + 952;
            uint32_t timer = rrj_u16(rrj_at(contact, 2));
            if (timer)
            {
                uint32_t step = (300 * delta) >> 16;
                if (rrj_s32(timer - step) > 0)
                {
                    rrj_put16(rrj_at(contact, 2), (uint16_t)(timer - step));
                    return step;
                }
                rrj_put16(rrj_at(contact, 2), 0);
            }
            else
            {
                uint32_t current = rrj_read32(0x8005B38C);
                uint32_t descriptor = rrj_read32(current + 1084);
                if (rrj_read32(descriptor + 40) || r_u8(descriptor + 39) >= 0xF8)
                {
                    uint32_t current_body = rrj_read32(current + 852);
                    int32_t dx = rrj_s32(rrj_read32(body + 184) - rrj_read32(current_body + 184)) >> 16;
                    int32_t dz = rrj_s32(rrj_read32(body + 192) - rrj_read32(current_body + 192)) >> 16;
                    uint32_t major = leaf_abs32(dx);
                    uint32_t minor = leaf_abs32(dz);
                    if (major < minor)
                    {
                        uint32_t swap = major;
                        major = minor;
                        minor = swap;
                    }
                    if (major - (major >> 5) - (major >> 7) + ((minor + (minor >> 1)) >> 2) + ((minor + (minor >> 1)) >> 6) < 25)
                        rrj_write32(0x8005B228, 0x20000);
                }
            }
        }
    }
    if (!r_s16(body + 320))
        return 0;
    if ((rrj_read32(body + 552) & 0x1000) || (rrj_read32(source + 856) && rrj_read32(source + 1088) && !linked_mode) || (player && rrj_read32(0x8005B220)))
        blocked = 1;
    if (!blocked && player && r_s16(source + 320) && (rrj_read32(source + 388) & 1) && (rrj_read32(source + 568) & 0x100))
    {
        uint32_t track = rrj_read32(source + 340);
        int32_t dx = rrj_s32(rrj_read32(source + 184) - rrj_read32(track + 20));
        int32_t dy = rrj_s32(rrj_read32(source + 188) - rrj_read32(track + 24));
        int32_t dz = rrj_s32(rrj_read32(source + 192) - rrj_read32(track + 28));
        int32_t dot = leaf_product_asr16(dx, (int32_t)r_s16(track + 8) << 4);
        dot += leaf_product_asr16(dy, (int32_t)r_s16(track + 10) << 4);
        dot += leaf_product_asr16(dz, (int32_t)r_s16(track + 12) << 4);
        blocked = leaf_abs32(dot) > 0x10000;
    }
    distance = sub_8001FCB0((uint32_t)(rrj_s32(rrj_read32(body + 184) - rrj_read32(source + 184)) >> 16), (uint32_t)(rrj_s32(rrj_read32(body + 188) - rrj_read32(source + 188)) >> 16), (uint32_t)(rrj_s32(rrj_read32(body + 192) - rrj_read32(source + 192)) >> 16));
    if (player)
        far = distance >= 201;
    if (!blocked && far && (rrj_read32(source + 560) & 0x08000000))
        blocked = 1;
    {
        uint32_t type = rrj_u16(rrj_at(body + 544, 2));
        if (rrj_u16(rrj_at(0x800541D6 + 8 * type, 2)) != 8)
        {
            if (type - 69 >= 2)
                return 0;
            result = sub_8005BE58(rrj_read32(body + 540));
            if (!result || !blocked)
                return result;
        }
    }
    rrj_write32(body + 552, rrj_read32(body + 552) & 0xFFFFEFFF);
    if (blocked)
    {
        uint32_t linked = rrj_read32(source + 856);
        race_leaf_track_pose(rrj_host_context(), source, body);
        if (linked && rrj_read32(source + 1088))
        {
            uint32_t index;
            (void)sub_8001E0B4(linked + 328, source + 328, 32);
            rrj_write32(linked + 428, rrj_read32(source + 428));
            sub_8003AF9C(linked + 172, 1, 0);
            rrj_write32(linked + 360, rrj_read32(source + 360));
            rrj_write32(linked + 364, rrj_read32(source + 364));
            rrj_write32(linked + 368, rrj_read32(source + 368));
            rrj_write32(linked + 492, rrj_read32(source + 492));
            rrj_write32(linked + 496, rrj_read32(source + 496));
            (void)sub_8001E0B4(linked + 372, source + 372, 56);
            rrj_write32(linked + 344, 0);
            rrj_write32(linked + 348, 0);
            rrj_put16(rrj_at(linked + 392, 2), 4);
            rrj_put16(rrj_at(linked + 394, 2), 1);
            rrj_put16(rrj_at(linked + 320, 2), 1);
            rrj_write32(linked + 324, rrj_read32(body + 324));
            rrj_write32(linked + 388, rrj_read32(linked + 388) & 0xFFFFFFFE);
            (void)sub_8002EAD8(rrj_at(source + 184, 12), rrj_at(source + 432, 6), rrj_read32(source + 304) + rrj_read32(linked + 304), rrj_at(linked + 184, 12));
            for (index = 0; index < 3; ++index)
                rrj_put16(rrj_at(linked + 450 + 2 * index, 2), rrj_u16(rrj_at(source + 450 + 2 * index, 2)));
        }
        if (rrj_u16(rrj_at(source + 172, 2)) < rrj_read32(race + 48))
        {
            uint32_t descriptor = rrj_read32(source + 1084);
            if (!r_u8(descriptor + 37) || !r_u8(descriptor + 14))
            {
                (void)sub_80092C7C(source, 8);
                return race_leaf_reset_body(rrj_host_context(), source, body);
            }
            else
            {
                uint32_t record = 0x800CD898 + 1132 * rrj_u16(rrj_at(source + 172, 2));
                rrj_write32(record + 548, (rrj_read32(record + 548) & 0xFF807FF9) | 6);
                (void)sub_80086AF8(record);
            }
        }
        return sub_8009277C(source, reverb);
    }
    if (r_u8(rrj_read32(source + 1084) + 39) >= 0xF7)
    {
        result = rrj_read32(body + 552) & 0xFFEFFFE7;
        rrj_write32(body + 552, result);
        return result;
    }
    rrj_write32(body + 484, 0x7FFF0000);
    if (far || ((rrj_read32(source + 560) & 0x02000000) && distance < 3))
    {
        rrj_write32(body + 552, rrj_read32(body + 552) | 0x01000800);
    }
    else
    {
        uint32_t timer_active = (rrj_read32(body + 552) >> 20) & 1;
        if (timer_active)
        {
            uint32_t timer = rrj_read32(body + 600) - delta;
            rrj_write32(body + 600, timer);
            if (rrj_s32(timer) > 0)
                rrj_write32(body + 552, rrj_read32(body + 552) | 0x100);
            else
            {
                timer_active = 0;
                rrj_write32(body + 552, rrj_read32(body + 552) & 0xFFEFFFFF);
            }
        }
        if (!timer_active)
        {
            uint32_t axis = 0x1F800018;
            uint32_t direction = 0x1F800020;
            if (sub_80098A50(body, source, axis, direction, reverb))
            {
                uint32_t vector = 0x1F800028;
                uint32_t fixed_result = 0;
                uint32_t magnitude;
                rrj_write32(vector, rrj_read32(body + 580) - rrj_read32(body + 184));
                rrj_write32(vector + 4, rrj_read32(body + 584) - rrj_read32(body + 188));
                rrj_write32(vector + 8, rrj_read32(body + 588) - rrj_read32(body + 192));
                if (leaf_abs32(rrj_s32(rrj_read32(vector))) <= 5931008 && leaf_abs32(rrj_s32(rrj_read32(vector + 4))) <= 5931008 && leaf_abs32(rrj_s32(rrj_read32(vector + 8))) <= 5931008)
                    magnitude = sub_8002F0F4(vector);
                else
                {
                    magnitude = 0x7FFF0000;
                    fixed_result = 1;
                }
                if (linked_mode && magnitude <= 0x3FFFF)
                {
                    if (!rrj_read32(source + 480))
                        (void)sub_800903F4(source, 0, reverb);
                }
                else
                {
                    uint32_t flags = rrj_read32(body + 552);
                    uint32_t contact = (flags & 0x18) != 0;
                    if (!(flags & 0x800) || contact || magnitude <= 0x3FFFF)
                    {
                        rrj_write32(body + 552, flags & 0xFFFFF07F);
                        if (contact && (rrj_read32(source + 480) || rrj_read32(source + 488)))
                        {
                            rrj_write32(body + 552, rrj_read32(body + 552) & 0xFFFFFFE7);
                            contact = 0;
                        }
                        if (contact)
                        {
                            if (rrj_read32(body + 552) & 8)
                            {
                                uint32_t descriptor = rrj_read32(source + 1084);
                                if (player && (!r_u8(descriptor + 37) || !r_u8(descriptor + 14)))
                                    (void)sub_80092C7C(source, 8);
                                (void)sub_80098F2C(body, axis, direction, magnitude, fixed_result);
                            }
                            if (rrj_read32(body + 552) & 0x10)
                                (void)sub_80099710(body, axis, direction, delta, reverb);
                        }
                        else
                        {
                            (void)sub_8009989C(body, source, vector, magnitude, fixed_result, delta);
                        }
                    }
                }
            }
        }
    }
    if (rrj_read32(body + 552) & 0x800)
    {
        uint32_t id = rrj_u16(rrj_at(body + 172, 2));
        rrj_write32(body + 552, rrj_read32(body + 552) & 0xFFEFFFE7);
        result = (id >> 5) == 1 && rrj_s32(id & 31) < rrj_s32(rrj_read32(race + 48));
        if (result)
        {
            uint32_t owner = rrj_read32(body + 596);
            uint32_t record = 0x800CD898 + 1132 * rrj_u16(rrj_at(owner + 172, 2));
            rrj_write32(record + 548, rrj_read32(record + 548) & 0xFFFFFF7F);
        }
    }
    return result;
}

uint32_t sub_800BA4CC(uint32_t delta, uint32_t skip_mask, uint32_t active_mask, RRJRaceLeafCall call, RRJReverbCall reverb)
{
    int32_t count = rrj_s32(rrj_read32(0x8005B1F8));
    uint32_t actor = rrj_read32(0x8005B3A0);
    int32_t index;
    FUNCTION_MARKER(0x800BA4CC, "RASHCDG.BIN");
    if (count <= 0)
        return 0x80060000;
    for (index = 0; index < count; ++index, actor += 1096)
    {
        uint32_t bit = 1u << (index & 31);
        if (!(skip_mask & bit))
        {
            uint32_t secondary = rrj_read32(actor + 852);
            uint32_t descriptor = rrj_read32(actor + 1084);
            uint32_t slot = (uint32_t)(int32_t)r_s8(actor + 946);
            uint32_t contact = actor + 948 + 8 * slot;
            uint32_t actor_id = rrj_u16(rrj_at(actor + 172, 2));
            uint32_t players = rrj_read32(rrj_read32(0x8005B2F8) + 48);
            uint32_t state = rrj_u16(rrj_at(contact, 2));
            uint16_t other_id = rrj_u16(rrj_at(contact + 2, 2));
            if (actor_id >= players && other_id < players && state >= 14 && state < 17)
            {
                uint32_t address = 0x800CCAC0 + 2 * other_id;
                rrj_put16(rrj_at(address, 2), rrj_u16(rrj_at(address, 2)) | (uint16_t)(1u << ((actor_id - 1) & 31)));
            }
            switch (state)
            {
                case 1:
                    if (!(rrj_read32(secondary + 552) & 0x40))
                    {
                        if (rrj_u16(rrj_at(contact + 4, 2)) || (actor_id < players && ((r_u8(descriptor + 1) & 15) == 2) && (rrj_read32(0x8005AD48) & 0x1F)))
                        {
                            uint32_t linked;
                            rrj_trace_dispatch_target(0x800BAA2Cu);
                            (void)sub_800BAA2C(actor, contact, delta, reverb);
                            rrj_trace_dispatch_target(0x800BA4CCu);
                            linked = rrj_read32(actor + 856);
                            if (linked && rrj_u16(rrj_at(contact, 2)) == 4)
                            {
                                uint32_t linked_slot = (uint32_t)(int32_t)r_s8(linked + 946);
                                rrj_trace_dispatch_target(0x800BAA2Cu);
                                (void)sub_800BAA2C(linked, linked + 948 + 8 * linked_slot, delta, reverb);
                                rrj_trace_dispatch_target(0x800BA4CCu);
                            }
                        }
                        else if (rrj_read32(secondary + 552) & 0x40)
                        {
                            rrj_trace_dispatch_target(0x80092AD4u);
                            (void)sub_80092AD4(actor);
                            rrj_trace_dispatch_target(0x800BA4CCu);
                        }
                    }
                    else
                    {
                        rrj_trace_dispatch_target(0x80092AD4u);
                        (void)sub_80092AD4(actor);
                        rrj_trace_dispatch_target(0x800BA4CCu);
                    }
                    break;
                case 2:
                    rrj_trace_dispatch_target(0x800BADB8u);
                    sub_800BADB8(actor, contact);
                    break;
                case 4:
                    if (!(active_mask & bit) && (rrj_read32(actor + 560) & 0x08000000))
                    {
                        rrj_trace_dispatch_target(0x800BA7F4u);
                        (void)sub_800BA7F4(actor);
                    }
                    break;
                case 5:
                    rrj_trace_dispatch_target(0x800BB280u);
                    (void)sub_800BB280(actor, other_id);
                    break;
                case 6:
                    rrj_trace_dispatch_target(0x800BB5B8u);
                    (void)sub_800BB5B8(actor, other_id);
                    break;
                case 7:
                    rrj_trace_dispatch_target(0x800BBA18u);
                    (void)sub_800BBA18(actor, other_id);
                    break;
                case 8:
                    rrj_trace_dispatch_target(0x800BBC20u);
                    (void)sub_800BBC20(actor, other_id);
                    break;
                case 9:
                    rrj_trace_dispatch_target(0x800BBE00u);
                    (void)sub_800BBE00(actor, other_id, call);
                    break;
                case 16:
                    rrj_trace_dispatch_target(0x800C035Cu);
                    (void)sub_800C035C(actor, other_id, call);
                    break;
                case 17:
                    rrj_trace_dispatch_target(0x800BBEBCu);
                    (void)sub_800BBEBC(actor, other_id);
                    break;
                case 18:
                    rrj_trace_dispatch_target(0x80092E04u);
                    (void)sub_80092E04(actor, delta, reverb);
                    break;
                default:
                    break;
            }
            rrj_trace_dispatch_target(0x800BA4CCu);
            rrj_put16(rrj_at(contact + 6, 2), rrj_u16(rrj_at(contact + 6, 2)) & 0x7FFF);
        }
    }
    return 0;
}

static int32_t race_leaf_signed_ratio(int32_t numerator, int32_t denominator)
{
    uint32_t quotient = sub_80010028(leaf_abs32(numerator), leaf_abs32(denominator));
    return (numerator < 0) != (denominator < 0) ? rrj_s32(0u - quotient) : rrj_s32(quotient);
}

uint32_t sub_800BD4D4(uint32_t actor)
{
    const uint32_t candidates = 0x1F800200;
    const uint32_t candidate_count = 0x1F800240;
    const uint32_t event = 0x1F800244;
    const uint32_t scalar = 0x1F800248;
    const uint32_t vector = 0x1F800250;
    uint32_t flags = 0;
    uint32_t created = 0;
    uint32_t packed = 224;
    uint32_t group = 7;
    uint32_t object = 0;
    uint32_t record = 0;
    uint32_t contact;
    int32_t range;
    int32_t relative;
    int32_t separation;
    int32_t left;
    int32_t right;
    int32_t edge;
    int32_t curve;
    uint32_t index;
    FUNCTION_MARKER(0x800BD4D4, "RASHCDG.BIN");
    if (!actor)
        return 0;
    contact = actor + 948 + 8u * (uint32_t)(int32_t)r_s8(actor + 946);
    if (rrj_s32(rrj_read32(actor + 480)) < 131)
    {
        if (rrj_u16(rrj_at(contact, 2)) == 3)
            sub_800BC8DC(actor);
        return 0;
    }
    range = rrj_s32(2u * (rrj_read32(actor + 480) + 0x141DDDu));
    rrj_write32(candidate_count, 16);
    (void)sub_8008AE94(candidates, candidate_count, actor + 184, range, 73, rrj_u16(rrj_at(actor + 172, 2)));
    if (rrj_read32(candidate_count))
    {
        uint8_t actor_flags = r_u8(actor + 928) & 0xFE;
        w_u8(actor + 928, actor_flags);
        if (!(rrj_read32(actor + 388) & 1))
        {
            int32_t alignment = rrj_s32(sub_8002E698(rrj_at(actor + 528, 6), rrj_at(rrj_read32(actor + 340) + 14, 6)));
            if (leaf_abs32(alignment) > 53739)
                w_u8(actor + 928, actor_flags | 1);
        }
    }
    for (index = 0; index < rrj_read32(candidate_count); ++index)
    {
        uint16_t candidate = rrj_u16(rrj_at(candidates + 2 * index, 2));
        int32_t distance = rrj_s32(sub_800BEBA4(candidate, actor));
        if (distance < range)
        {
            range = distance;
            packed = candidate;
            group = candidate >> 5;
        }
    }
    if (packed == 224)
    {
        if (rrj_u16(rrj_at(contact, 2)) != 3)
            return 0;
        if (r_u8(actor + 928) & 1)
        {
            uint16_t previous = rrj_u16(rrj_at(contact + 2, 2));
            if ((previous >> 5) == 3)
            {
                uint32_t previous_object = 0x800CF660 + 512 * (previous & 31);
                uint32_t valid = rrj_u16(rrj_at(previous_object + 320, 2)) != 0 && rrj_s32(rrj_read32(previous_object + 324) - (rrj_s32(rrj_read32(previous_object + 308)) >> 4)) < rrj_s32(rrj_read32(actor + 324));
                flags = valid << 5;
            }
        }
        if (!(flags & 0x20))
        {
            sub_800BC8DC(actor);
            return 0;
        }
        packed = rrj_u16(rrj_at(contact + 2, 2));
        group = packed >> 5;
    }
    else if (rrj_u16(rrj_at(contact + 2, 2)) == packed && rrj_u16(rrj_at(contact, 2)) >= 5)
    {
        return 0;
    }
    switch (group)
    {
        case 0:
            object = rrj_read32(0x8005B3A0) + 1096 * (packed & 31);
            record = object + 172;
            relative = rrj_s32(sub_8002E698(rrj_at(object + 528, 6), rrj_at(actor + 528, 6)));
            if (leaf_abs32(relative) >= 0x4000)
                relative = relative >= 0 ? rrj_s32(rrj_read32(actor + 480) - rrj_read32(object + 480)) : rrj_s32(rrj_read32(actor + 480) + rrj_read32(object + 480));
            else
                relative = rrj_s32(rrj_read32(actor + 480));
            break;
        case 3:
            object = 0x800CF660 + 512 * (packed & 31);
            record = object + 172;
            relative = rrj_s32(sub_8002E698(rrj_at(object + 444, 6), rrj_at(actor + 528, 6)));
            if (leaf_abs32(relative) >= 0x4000)
                relative = relative >= 0 ? rrj_s32(rrj_read32(actor + 480) - rrj_read32(object + 480)) : rrj_s32(rrj_read32(actor + 480) + rrj_read32(object + 480));
            else
                relative = rrj_s32(rrj_read32(actor + 480));
            if (rrj_read32(actor + 360) == rrj_read32(object + 360) && !(rrj_read32(actor + 360) >> 16))
            {
                flags |= 2;
                if (r_u8(object + 510) & 2)
                    flags |= 0x40;
            }
            break;
        case 6:
            record = rrj_read32(0x800CD6C4) + 280 * (packed & 31);
            relative = rrj_s32(rrj_read32(actor + 480));
            if (rrj_read32(record + 8) == 1)
                flags |= 0x10;
            break;
        default:
            return 0;
    }
    if ((r_u8(actor + 928) & 1) && rrj_read32(actor + 360) == rrj_read32(record + 188))
        separation = rrj_s32(16u * (rrj_read32(actor + 324) - rrj_read32(record + 152)) - rrj_read32(actor + 308) - rrj_read32(record + 136));
    else
    {
        separation = rrj_s32(sub_800B6AAC(rrj_at(record + 12, 12), rrj_at(actor + 528, 6), rrj_at(actor + 504, 12)) - rrj_read32(actor + 308));
        if (!(flags & 2))
            separation = rrj_s32((uint32_t)separation - (uint32_t)sub_8001FC90(46333, rrj_s32(rrj_read32(record + 132) + rrj_read32(record + 136))));
    }
    if (separation < rrj_s32(rrj_read32(actor + 308)) || rrj_s32(2u * (uint32_t)relative) < separation)
    {
        if (rrj_u16(rrj_at(contact, 2)) == 3)
        {
            if (rrj_u16(rrj_at(contact + 2, 2)) != packed)
            {
                sub_800BC8DC(actor);
                return 0;
            }
            if ((r_u8(actor + 928) & 1) && group == 3)
                flags |= 0x20;
            if (!(flags & 0x20))
            {
                sub_800BC8DC(actor);
                return 0;
            }
        }
        else if (!(flags & 0x20))
        {
            return 0;
        }
    }
    if (!group && (uint32_t)relative <= 0x13FFFF)
    {
        uint32_t other_contact;
        uint16_t kind = 7;
        int32_t lateral;
        if (rrj_u16(rrj_at(contact, 2)) < 5)
            goto create_contact;
        if (rrj_read32(object + 360) == rrj_read32(actor + 360) && (!(rrj_read32(object + 360) >> 16) || rrj_read32(object + 336) == rrj_read32(actor + 336)))
        {
            lateral = rrj_s32(rrj_read32(object + 344) - rrj_read32(actor + 344));
            if (rrj_s32(rrj_read32(actor + 364)) < 0)
                lateral = -lateral;
        }
        else
            lateral = rrj_s32(sub_800B6AAC(rrj_at(object + 184, 12), rrj_at(actor + 432, 6), rrj_at(actor + 184, 12)));
        if (rrj_read32(object + 856) && lateral < 0)
            lateral = rrj_s32((uint32_t)lateral + rrj_read32(object + 304) + rrj_read32(rrj_read32(object + 856) + 304));
        if (leaf_abs32(lateral) > 98304 || separation > 655360)
            return 0;
        other_contact = object + 948 + 8u * (uint32_t)(int32_t)r_s8(object + 946);
        if ((r_u8(rrj_read32(object + 1084) + 1) & 15) == (r_u8(rrj_read32(actor + 1084) + 1) & 15))
        {
            if (rrj_u16(rrj_at(other_contact, 2)) == 10 && rrj_u16(rrj_at(actor + 172, 2)) == 0)
                kind = 12;
            else if (rrj_u16(rrj_at(other_contact + 2, 2)) == rrj_u16(rrj_at(actor + 172, 2)))
                kind = 7;
            else if (4 * rrj_s32(rrj_read32(actor + 308)) < separation)
                kind = 5;
        }
        if (kind == 7)
        {
            if (!sub_800BBBB8(actor, object, separation))
                return 0;
        }
        else if (kind == 5 && !sub_800BB448(actor, object, separation))
            return 0;
        if (rrj_u16(rrj_at(contact, 2)) == 3)
            sub_800BC8DC(actor);
        rrj_put16(rrj_at(event, 2), kind);
        rrj_put16(rrj_at(event + 2, 2), packed);
        (void)sub_800BCA68(event, 2, actor);
        if (kind == 7 && (uint16_t)(rrj_u16(rrj_at(other_contact, 2)) - 6) < 2 && sub_800BB448(actor, object, separation))
        {
            rrj_put16(rrj_at(event, 2), 5);
            (void)sub_800BCA68(event, 2, actor);
        }
        return 0;
    }
create_contact:
    if (rrj_u16(rrj_at(contact, 2)) == 3)
    {
        rrj_put16(rrj_at(contact + 2, 2), (uint16_t)packed);
        if (!(rrj_u16(rrj_at(contact + 6, 2)) & 0x8000))
        {
            uint32_t state = rrj_read32(0x8005B2F8);
            uint32_t phase = 2180 * rrj_read32(state + 16);
            uint16_t stamp = rrj_u16(rrj_at(contact + 6, 2));
            if (phase - ((uint32_t)(stamp & 0x3FFF) << 16) > 491520)
                rrj_put16(rrj_at(contact + 6, 2), stamp | 0x8000 | (uint16_t)(((phase + 0x8000) >> 16) & 0x3FFF));
        }
    }
    else
    {
        rrj_put16(rrj_at(event, 2), 3);
        rrj_put16(rrj_at(event + 2, 2), (uint16_t)packed);
        (void)sub_800BCA68(event, 2, actor);
        created = 1;
        contact = actor + 948 + 8u * (uint32_t)(int32_t)r_s8(actor + 946);
    }
    if (flags & 2)
    {
        int32_t delta = rrj_s32(rrj_read32(record + 172) - rrj_read32(actor + 344));
        if (rrj_s32(rrj_read32(actor + 364)) < 0)
            delta = -delta;
        edge = rrj_s32(rrj_read32(record + 132));
        left = delta - edge;
        right = delta + edge;
    }
    else
    {
        int32_t heading_x = rrj_s32(rrj_read32(actor + 296));
        int32_t heading_z = rrj_s32(0u - rrj_read32(actor + 300));
        int32_t extra = 0;
        int32_t first_x;
        int32_t first_z;
        int32_t second_x;
        int32_t second_z;
        edge = rrj_s32(rrj_read32(record + 132));
        if (flags & 0x10)
            edge = rrj_s32(rrj_read32(actor + 188) - rrj_read32(actor + 312)) >= rrj_s32(rrj_read32(record + 88)) ? rrj_s32(rrj_read32(record + 84)) : edge;
        else if (!(rrj_u16(rrj_at(record, 2)) >> 5) && rrj_read32(object + 856))
            extra = rrj_s32(2u * rrj_read32(rrj_read32(object + 856) + 304));
        if (rrj_s32((uint32_t)sub_8001FC90(rrj_s32(rrj_read32(actor + 296)), rrj_s32(rrj_read32(record + 124))) + (uint32_t)sub_8001FC90(rrj_s32(rrj_read32(actor + 300)), rrj_s32(rrj_read32(record + 128)))) >= 0)
        {
            first_x = rrj_s32(rrj_read32(record + 24));
            first_z = rrj_s32(rrj_read32(record + 32));
            second_x = rrj_s32((uint32_t)first_x + (uint32_t)sub_8001FC90(2 * edge + extra, rrj_s32(rrj_read32(record + 124))));
            second_z = rrj_s32((uint32_t)first_z - (uint32_t)sub_8001FC90(2 * edge + extra, rrj_s32(rrj_read32(record + 128))));
        }
        else
        {
            first_x = rrj_s32(rrj_read32(record + 48) + (uint32_t)sub_8001FC90(extra, rrj_s32(rrj_read32(record + 124))));
            first_z = rrj_s32(rrj_read32(record + 56) - (uint32_t)sub_8001FC90(extra, rrj_s32(rrj_read32(record + 128))));
            second_x = rrj_s32(rrj_read32(record + 48) - (uint32_t)sub_8001FC90(2 * edge, rrj_s32(rrj_read32(record + 124))));
            second_z = rrj_s32(rrj_read32(record + 56) + (uint32_t)sub_8001FC90(2 * edge, rrj_s32(rrj_read32(record + 128))));
        }
        left = rrj_s32((uint32_t)sub_8001FC90(heading_x, rrj_s32((uint32_t)first_x - rrj_read32(actor + 184))) + (uint32_t)sub_8001FC90(heading_z, rrj_s32((uint32_t)first_z - rrj_read32(actor + 192))));
        right = rrj_s32((uint32_t)sub_8001FC90(heading_x, rrj_s32((uint32_t)second_x - rrj_read32(actor + 184))) + (uint32_t)sub_8001FC90(heading_z, rrj_s32((uint32_t)second_z - rrj_read32(actor + 192))));
    }
    {
        uint32_t sine_address = 0x8005624C + (((uint32_t)(163u * rrj_read32(actor + 652)) >> 12) & 0x3FFC);
        curve = rrj_s32((uint32_t)sub_8001FC90((int32_t)r_s16(sine_address) * 16, rrj_s32(rrj_read32(actor + 312))));
        curve = rrj_s32(leaf_abs32(curve));
    }
    if (rrj_s32(rrj_read32(actor + 652)) < -1143)
        right = rrj_s32((uint32_t)right + (uint32_t)curve);
    else if (rrj_s32(rrj_read32(actor + 652)) >= 1144)
        left = rrj_s32((uint32_t)left - (uint32_t)curve);
    {
        int32_t left_abs = rrj_s32(leaf_abs32(left));
        int32_t right_abs = rrj_s32(leaf_abs32(right));
        if ((left >= 0) != (right < 0))
        {
            int32_t nearest = left_abs < right_abs ? left_abs : right_abs;
            if (3 * rrj_s32(rrj_read32(actor + 304)) < nearest)
            {
                flags |= 0x20;
                if (!(flags & 2))
                    return created;
            }
        }
        if (left_abs < right_abs)
            flags |= 1;
        if (!(rrj_u16(rrj_at(contact + 6, 2)) & 0x8000))
            flags = (flags & ~1u) | ((r_u8(actor + 928) >> 2) & 1);
        if (flags & 2)
        {
            int32_t adjusted = curve + edge + rrj_s32(rrj_read32(actor + 304));
            int32_t direction = rrj_s32(rrj_read32(actor + 652));
            int32_t record_side = rrj_s32(rrj_read32(record + 172));
            int32_t impulse = 0;
            if ((direction < -1143 && !(flags & 1)) || (direction >= 1144 && (flags & 1)))
            {
                flags |= 4;
                adjusted += curve;
            }
            if (adjusted < (int32_t)leaf_abs32(rrj_s32(rrj_read32(actor + 344) - (uint32_t)record_side)))
                flags |= 0x20;
            if (record_side < 0)
                flags |= 8;
            if (r_s16(actor + ((flags & 8) ? 400 : 412) + 8) >= 2)
                impulse = race_leaf_signed_ratio((int32_t)leaf_abs32(record_side), rrj_s32(rrj_read32(actor + 424)));
            impulse = rrj_s32((uint32_t)sub_8001FC90(rrj_s32(rrj_read32(actor + 424)), impulse));
            if (flags & 8)
                impulse = -impulse;
            if ((record_side >= 0) != ((((flags ^ 1) & 1) ^ (rrj_s32(rrj_read32(actor + 364)) > 0)) != 0))
                impulse = rrj_s32((uint32_t)impulse + ((flags & 8) ? 0u - rrj_read32(actor + 424) : rrj_read32(actor + 424)));
            if (flags & 4)
            {
                int32_t signed_curve = ((rrj_s32(rrj_read32(actor + 652)) >= 0) ^ (rrj_s32(rrj_read32(actor + 364)) > 0)) ? curve : -curve;
                impulse = rrj_s32((uint32_t)impulse + 2u * (uint32_t)signed_curve);
            }
            impulse = rrj_s32((uint32_t)impulse - 32u * (uint32_t)(int32_t)r_s16(actor + 878));
            if ((int16_t)(((int32_t)r_s16(rrj_read32(actor + 340) + 2) * r_s16(actor + 872)) >> 12) + (int16_t)(((int32_t)r_s16(rrj_read32(actor + 340) + 6) * r_s16(actor + 876)) >> 12) < 0)
                impulse = -impulse;
            rrj_write32(scalar, (uint32_t)impulse);
            sub_80093CAC(actor, 0, actor + 872, scalar, 0);
            rrj_put16(rrj_at(actor + 944, 2), 1);
        }
        else
        {
            int32_t first = rrj_s32(sub_800B6AAC(rrj_at(actor + 880, 12), rrj_at(actor + 516, 6), rrj_at(actor + 504, 12)));
            int32_t second = rrj_s32(sub_800B6AAC(rrj_at(actor + 880, 12), rrj_at(actor + 528, 6), rrj_at(actor + 504, 12)));
            int32_t tangent;
            int32_t scale;
            if ((first < 1) == ((flags & 1) != 0))
                tangent = first;
            else
            {
                int32_t ratio = race_leaf_signed_ratio((int32_t)leaf_abs32(first), second);
                int32_t side_value;
                if ((flags & 1) ? right_abs > 0 : left_abs > 0)
                    side_value = race_leaf_signed_ratio((flags & 1) ? right_abs : left_abs, separation);
                else
                    side_value = race_leaf_signed_ratio((flags & 1) ? -right_abs : -left_abs, separation);
                if (side_value < 2 * ratio)
                    flags ^= 1;
                tangent = first;
            }
            if (separation > 196607)
                scale = 2 * rrj_s32(rrj_read32(actor + 304));
            else
                scale = rrj_s32((uint32_t)sub_8001FC90(separation / 4, rrj_s32(rrj_read32(actor + 304))));
            if (flags & 1)
                scale = -left_abs - scale;
            else
                scale += right_abs;
            scale = race_leaf_signed_ratio(rrj_s32((uint32_t)sub_8001FC90(scale, second)), separation);
            rrj_write32(vector, (uint32_t)((int32_t)sub_8001FC90(second, rrj_s32(rrj_read32(actor + 300))) + (int32_t)sub_8001FC90(scale, rrj_s32(rrj_read32(actor + 296))) + rrj_s32(rrj_read32(actor + 184))));
            rrj_write32(vector + 4, rrj_read32(actor + 884));
            rrj_write32(vector + 8, (uint32_t)((int32_t)sub_8001FC90(second, rrj_s32(rrj_read32(actor + 296))) + (int32_t)sub_8001FC90(scale, rrj_s32(0u - rrj_read32(actor + 300))) + rrj_s32(rrj_read32(actor + 192))));
            sub_80093CAC(actor, vector, 0, 0, 0);
        }
    }
    w_u8(actor + 928, (r_u8(actor + 928) & 0xFB) | (uint8_t)(4 * (flags & 1)));
    {
        uint32_t state = rrj_read32(0x8005B2F8);
        uint32_t mode = rrj_read32(state + 60);
        int32_t threshold = rrj_s32(rrj_read32(0x80053018 + 4 * mode));
        int32_t ratio;
        if (flags & 0x40)
            threshold *= 2;
        if ((flags & 0x20) || separation >= threshold || separation <= 196608)
            return created;
        if ((rrj_u16(rrj_at(record, 2)) >> 5) == 3 && rrj_s32(rrj_read32(record + 140)) <= 104856 && (uint32_t)relative > 917504 && separation < threshold - 491520 * (3 - (int32_t)mode))
        {
            uint16_t count = rrj_u16(rrj_at(contact + 4, 2));
            rrj_write32(actor + 560, rrj_read32(actor + 560) | 0x800);
            if ((uint32_t)(count - 1) < 300)
                rrj_put16(rrj_at(contact + 4, 2), count + rrj_u16(rrj_at(state + 28, 2)));
            else
            {
                rrj_put16(rrj_at(contact + 4, 2), 1);
                rrj_write32(actor + 560, rrj_read32(actor + 560) | 0x1000);
            }
            return created;
        }
        ratio = race_leaf_signed_ratio(separation - 196608, threshold - 196608);
        rrj_write32(actor + 924, (uint32_t)sub_8001FC90(rrj_s32(rrj_read32(actor + 924)), ratio));
        rrj_write32(actor + 916, 0);
        rrj_write32(actor + 920, 0);
        rrj_write32(actor + 564, rrj_read32(actor + 564) & 0xFFF7FFFFu);
    }
    return created;
}

uint32_t sub_800BCD48(uint32_t delta, uint32_t skip_mask, uint32_t active_mask)
{
    int32_t count = rrj_s32(rrj_read32(0x8005B1F8));
    uint32_t actor = rrj_read32(0x8005B3A0);
    int32_t index;
    FUNCTION_MARKER(0x800BCD48, "RASHCDG.BIN");
    if (count <= 0)
        return 0x80060000;
    for (index = 0; index < count; ++index, actor += 1096)
    {
        uint32_t bit = 1u << (index & 31);
        uint32_t contact = actor + 948 + 8u * (uint32_t)(int32_t)r_s8(actor + 946);
        uint16_t kind = rrj_u16(rrj_at(contact, 2));
        uint32_t flags;
        uint32_t created;
        int32_t timer;
        if (skip_mask & bit || !r_s16(actor + 320) || (rrj_read32(actor + 560) & 0x18000000) != 0x08000000 || (r_u8(rrj_read32(actor + 1084)) & 1) || kind < 3 || kind >= 18)
            continue;
        created = sub_800BD4D4(actor);
        flags = rrj_read32(actor + 564);
        if (!(flags & 0x200))
            continue;
        if (!created && leaf_abs32(rrj_s32(rrj_read32(actor + 676))) < 2130)
        {
            timer = active_mask & bit ? rrj_s32(rrj_read32(actor + 704) - delta) : 49152;
            rrj_write32(actor + 704, (uint32_t)timer);
        }
        else
        {
            rrj_write32(actor + 564, flags & 0xFFFFFDFFu);
            rrj_write32(actor + 704, 0);
            continue;
        }
        if (timer < 0)
        {
            rrj_write32(actor + 704, 0);
            rrj_write32(actor + 564, rrj_read32(actor + 564) & 0xFFFFFDFFu);
        }
    }
    return 0;
}

uint32_t sub_800B9414(uint32_t delta, RRJRaceLeafCall call, RRJReverbCall reverb)
{
    FUNCTION_MARKER(0x800B9414, "RASHCDG.BIN");
    return sub_800B941C(delta, call, reverb);
}

uint32_t sub_800B941C(uint32_t delta, RRJRaceLeafCall call, RRJReverbCall reverb)
{
    const uint32_t inactive_out = 0x1F800270;
    const uint32_t active_out = 0x1F800274;
    const uint32_t marked_out = 0x1F800278;
    uint32_t race = rrj_read32(0x8005B2F8);
    uint32_t players = rrj_read32(race + 48);
    uint32_t finished_mask = 0;
    uint32_t index;
    FUNCTION_MARKER(0x800B941C, "RASHCDG.BIN");
    rrj_trace_dispatch_target(0x800B941Cu);
    rrj_write32(inactive_out, 0);
    rrj_write32(active_out, 0);
    rrj_write32(marked_out, 0);
    for (index = 0; index < players; ++index)
    {
        uint32_t actor = rrj_read32(0x8005B268 + 4 * index);
        uint32_t secondary = rrj_read32(actor + 852);
        uint32_t descriptor = rrj_read32(actor + 1084);
        uint32_t finished = 0;
        if (r_u8(secondary + 572) & 0x10)
        {
            uint32_t linked = rrj_read32(actor + 856);
            uint32_t linked_secondary = rrj_read32(linked + 852);
            uint32_t type = rrj_u16(rrj_at(linked_secondary + 544, 2));
            rrj_write32(linked_secondary + 552, rrj_read32(linked_secondary + 552) & 0xFDFFFFFFu);
            if (rrj_read32(secondary + 604) < 2 && rrj_u16(rrj_at(0x800541D4 + 8 * type + 2, 2)) == 8 && !rrj_read32(actor + 480) && !r_u8(race + 57))
                (void)sub_800903F4(actor, 1, reverb);
        }
        if (rrj_read32(descriptor + 40) || r_u8(descriptor + 39) >= 0xF8)
            finished = 1u << (index & 31);
        finished_mask |= finished;
        if (finished_mask == (1u << (players & 31)) - 1)
        {
            int32_t timer = rrj_s32(rrj_read32(0x8005B230));
            uint32_t player = 0x800CD898 + 1132 * index;
            if (timer < 0)
                timer = 0;
            timer = rrj_s32((uint32_t)timer + delta);
            rrj_write32(0x8005B230, (uint32_t)timer);
            if ((rrj_read32(player + 552) & 8) || timer >= rrj_s32(rrj_read32(0x8005B228)))
            {
                if (rrj_read32(0x8005B220))
                    w_u8(race, 2);
                else
                {
                    (void)(uint32_t)sub_8003F708();
                    w_u8(race, 6);
                }
                (void)sub_80018C1C(1);
            }
        }
    }
    rrj_trace_dispatch_target(0x800B9794u);
    (void)sub_800B9794(inactive_out, call, reverb);
    rrj_trace_dispatch_target(0x800BA304u);
    (void)sub_800BA304(delta, rrj_read32(inactive_out), active_out, marked_out, call);
    rrj_trace_dispatch_target(0x800BA4CCu);
    (void)sub_800BA4CC(delta, rrj_read32(inactive_out), rrj_read32(active_out), call, reverb);
    rrj_trace_dispatch_target(0x800BCD48u);
    (void)sub_800BCD48(delta, rrj_read32(inactive_out), rrj_read32(marked_out));
    rrj_trace_dispatch_target(0x800B941Cu);
    for (index = 0; index < players; ++index)
    {
        uint32_t actor = rrj_read32(0x8005B268 + 4 * index);
        uint32_t secondary = rrj_read32(actor + 852);
        uint32_t linked = rrj_read32(actor + 856);
        if (r_u8(secondary + 572) & 0x10)
        {
            uint32_t contact = linked + 948 + 8u * (uint32_t)(int32_t)r_s8(linked + 946);
            if (rrj_u16(rrj_at(contact, 2)) == 16)
            {
                rrj_trace_dispatch_target(0x800C035Cu);
                (void)sub_800C035C(linked, rrj_u16(rrj_at(contact + 2, 2)), call);
                rrj_trace_dispatch_target(0x800B941Cu);
            }
        }
        if (linked && r_u8(race + 57) == 2)
        {
            uint32_t previous = linked + 948 + 8u * (uint32_t)((int32_t)r_s8(linked + 946) - 1);
            if (rrj_u16(rrj_at(previous, 2)) == 18)
            {
                rrj_trace_dispatch_target(0x80092E04u);
                (void)sub_80092E04(linked, delta, reverb);
                rrj_trace_dispatch_target(0x800B941Cu);
            }
        }
    }
    return 0;
}

uint32_t sub_8009F6F8(uint32_t actor, uint32_t sample)
{
    FUNCTION_MARKER(0x8009F6F8, "RASHCDG.BIN");
    return sub_8009F700(actor, sample);
}

uint32_t sub_8009F700(uint32_t actor, uint32_t sample)
{
    uint32_t race = rrj_read32(0x8005B2F8);
    uint32_t flags[4] = {0, 0, 0, 0};
    int32_t count = rrj_s32(rrj_read32(race + 48));
    int32_t index;
    FUNCTION_MARKER(0x8009F700, "RASHCDG.BIN");
    for (index = 0; index < count && index < 4; ++index)
    {
        uint32_t player = 0x800CD898 + 1132u * (uint32_t)index;
        int32_t dx = (rrj_s32(rrj_read32(player + 184)) >> 16) - (rrj_s32(rrj_read32(actor + 184)) >> 16);
        int32_t dz = (int32_t)r_s16(player + 194) - r_s16(actor + 194);
        int32_t high = rrj_s32(leaf_abs32(dx));
        int32_t low = rrj_s32(leaf_abs32(dz));
        int32_t actor_x = rrj_s32(rrj_read32(actor + 184) - rrj_read32(player + 184));
        int32_t actor_y = rrj_s32(rrj_read32(actor + 188) - rrj_read32(player + 188));
        int32_t actor_z = rrj_s32(rrj_read32(actor + 192) - rrj_read32(player + 192));
        int32_t sample_x = rrj_s32(rrj_read32(sample + 12) - rrj_read32(player + 184));
        int32_t sample_y = rrj_s32(rrj_read32(sample + 16) - rrj_read32(player + 188));
        int32_t sample_z = rrj_s32(rrj_read32(sample + 20) - rrj_read32(player + 192));
        int32_t axis_x = (int32_t)r_s16(player + 444) * 16;
        int32_t axis_y = (int32_t)r_s16(player + 446) * 16;
        int32_t axis_z = (int32_t)r_s16(player + 448) * 16;
        int32_t actor_dot;
        int32_t sample_dot;
        int32_t metric;
        if (high < low)
        {
            int32_t swap = high;
            high = low;
            low = swap;
        }
        metric = high - (high >> 5) - (high >> 7) + ((low + (low >> 1)) >> 2) + ((low + (low >> 1)) >> 6);
        actor_dot = rrj_s32((uint32_t)sub_8001FC90(actor_x, axis_x) + (uint32_t)sub_8001FC90(actor_y, axis_y) + (uint32_t)sub_8001FC90(actor_z, axis_z));
        sample_dot = rrj_s32((uint32_t)sub_8001FC90(sample_x, axis_x) + (uint32_t)sub_8001FC90(sample_y, axis_y) + (uint32_t)sub_8001FC90(sample_z, axis_z));
        if (metric >= 160 && ((actor_dot < 0 && sample_dot < 0) || (rrj_u16(rrj_at(sample, 2)) >> 5) == 3))
            flags[index] = 1;
    }
    if (!(r_u8(race + 4) & 0x10))
        return flags[0];
    return flags[0] && flags[1];
}

void sub_8009F9EC(uint32_t actor, uint32_t packed_id)
{
    uint32_t group;
    uint16_t packed;
    FUNCTION_MARKER(0x8009F9EC, "RASHCDG.BIN");
    if (!actor || !packed_id)
        return;
    packed = rrj_u16(rrj_at(packed_id, 2));
    group = packed >> 5;
    if (group >= 2 || rrj_s32(rrj_read32(actor + 480)) >= 131 || !(r_u8(actor + 509) & 0x10))
        return;
    if (!group)
    {
        uint32_t other = rrj_read32(0x8005B3A0) + 1096 * packed;
        if (rrj_read32(rrj_read32(other + 1084) + 40))
            return;
    }
    if (!(sub_8001FC58() & 7))
        sub_80017BA0(rrj_s32(rrj_read32(actor + 184)), rrj_s32(rrj_read32(actor + 192)), 99 - (rrj_read32(actor + 180) & 1), 0);
}

uint32_t sub_8009FCDC(uint32_t actor)
{
    FUNCTION_MARKER(0x8009FCDC, "RASHCDG.BIN");
    return sub_8009FCE4(actor);
}

uint32_t sub_8009FCE4(uint32_t actor)
{
    uint32_t race = rrj_read32(0x8005B2F8);
    uint32_t players = rrj_read32(race + 48);
    uint32_t result = 0;
    uint32_t index;
    FUNCTION_MARKER(0x8009FCE4, "RASHCDG.BIN");
    for (index = 0; index < players; ++index)
    {
        uint32_t player = rrj_read32(0x8005B268 + 4 * index);
        int32_t direction;
        int32_t distance;
        if (!actor || rrj_read32(rrj_read32(player + 852) + 604) >= 3 || rrj_s32(rrj_read32(player + 480)) <= 0x280000 || rrj_read32(player + 360) != rrj_read32(actor + 360))
            continue;
        direction = rrj_s32(rrj_read32(player + 364));
        if (rrj_s32(rrj_read32(player + 364) ^ rrj_read32(actor + 364)) >= 0)
            continue;
        if ((direction > 0 && rrj_s32(rrj_read32(player + 368)) >= rrj_s32(rrj_read32(actor + 368))) || (direction < 0 && rrj_s32(rrj_read32(actor + 368)) >= rrj_s32(rrj_read32(player + 368))))
            continue;
        if (rrj_read32(player + 360) == rrj_read32(actor + 360) && (!(rrj_read32(player + 360) >> 16) || rrj_read32(player + 336) == rrj_read32(actor + 336)))
            distance = rrj_s32(rrj_read32(player + 344) - rrj_read32(actor + 344));
        else
            distance = rrj_s32(sub_800B6AAC(rrj_at(player + 184, 12), rrj_at(actor + 432, 6), rrj_at(actor + 184, 12)));
        if (leaf_abs32(distance) < rrj_read32(actor + 304))
        {
            result = 1;
            (void)(uint32_t)rrj_draft_80017CF0(index, rrj_read32(actor + 180) & 1, rrj_read32(actor + 184), rrj_read32(actor + 192));
        }
    }
    return result;
}

uint32_t sub_8009FE90(uint32_t actor)
{
    uint32_t record;
    FUNCTION_MARKER(0x8009FE90, "RASHCDG.BIN");
    if (!actor || !rrj_u16(rrj_at(actor + 172, 2)))
        return 0;
    if (!r_s16(actor + 320))
    {
        (void)sub_8008C000(actor + 172, 3);
        return 0;
    }
    record = sub_80039A08(rrj_read32(rrj_read32(actor + 328)));
    if (record && rrj_read32(record + 12) && rrj_read32(actor + 328) == rrj_read32(record + 12))
        return 1;
    rrj_put16(rrj_at(actor + 320, 2), 0);
    (void)sub_8008C000(actor + 172, 3);
    return 0;
}

uint32_t sub_800A04B0(uint32_t actor, uint32_t second, uint32_t third)
{
    FUNCTION_MARKER(0x800A04B0, "RASHCDG.BIN");
    return sub_800A04B8(actor, second, third);
}

uint32_t sub_800A04B8(uint32_t actor, uint32_t second, uint32_t third)
{
    uint32_t race = rrj_read32(0x8005B2F8);
    uint32_t players = rrj_read32(race + 48);
    uint32_t primary;
    uint32_t secondary;
    uint32_t index;
    FUNCTION_MARKER(0x800A04B8, "RASHCDG.BIN");
    if (!(r_u8(race + 4) & 0x10))
        return 0;
    primary = rrj_read32(0x8005B38C);
    secondary = rrj_read32(0x8005B21C);
    if (rrj_read32(primary + 360) == rrj_read32(secondary + 360))
    {
        uint32_t distances[2] = {0, 0};
        if (rrj_read32(actor + 360) != rrj_read32(primary + 360))
            return 0;
        for (index = 0; index < players && index < 2; ++index)
        {
            uint32_t player = rrj_read32(0x8005B268 + 4 * index);
            distances[index] = leaf_abs32(rrj_s32(rrj_read32(actor + 368) - rrj_read32(player + 368)));
        }
        return distances[1] < distances[0];
    }
    for (index = 0; index < players; ++index)
    {
        uint32_t player = rrj_read32(0x8005B268 + 4 * index);
        if (rrj_read32(actor + 360) == rrj_read32(player + 360))
            return index;
    }
    return 0;
}

uint32_t sub_8009B4C0(uint32_t actor, uint32_t delta, uint32_t force)
{
    const uint32_t nearest_out = 0x1F800280;
    uint8_t flags;
    uint32_t distance;
    FUNCTION_MARKER(0x8009B4C0, "RASHCDG.BIN");
    if (!actor)
        return 0;
    flags = r_u8(actor + 509);
    if (rrj_u16(rrj_at(actor + 362, 2)) == 1)
    {
        w_u8(actor + 509, flags & 0xBC);
        return flags & 0xBC;
    }
    if (flags & 0x40)
        return 0x40;
    if (rrj_s32(rrj_read32(actor + 480)) < 131)
    {
        int32_t timer = rrj_s32(rrj_read32(actor + 504));
        if (timer >= 0)
        {
            timer = rrj_s32((uint32_t)timer + delta);
            rrj_write32(actor + 504, (uint32_t)timer);
        }
        if (timer <= 0x1FFFF)
            return 0;
        rrj_write32(actor + 484, 0x26666);
        rrj_write32(actor + 504, 0xFFFF0000);
        flags = (flags & 0xFC) | 0x40;
        w_u8(actor + 509, flags);
    }
    if (flags & 2)
    {
        distance = sub_8003A5F4(actor + 360, nearest_out, 1);
        if (rrj_s32(rrj_read32(actor + 480)) < 131 || rrj_s32(distance) < rrj_s32(2u * rrj_read32(actor + 308)))
            rrj_write32(actor + 504, 0);
        return distance;
    }
    if (force || (flags & 4) || !r_s16(rrj_read32(actor + 340)) || r_s16(rrj_read32(actor + 340)) == r_s16(rrj_read32(actor + 336) + 10) - 1)
    {
        if (rrj_u16(rrj_at(actor + 362, 2)))
        {
            w_u8(actor + 509, flags & 0xFD);
            return flags & 0xFD;
        }
        distance = sub_8003A5F4(actor + 360, nearest_out, 1);
        if (rrj_read32(nearest_out) != 0xFFFFFFFF && distance <= 0xEFFFF && rrj_s32(rrj_read32(actor + 480)) >= 132)
        {
            int32_t squared = rrj_s32((uint32_t)sub_8001FC90(rrj_s32(rrj_read32(actor + 480)), rrj_s32(rrj_read32(actor + 480))));
            w_u8(actor + 509, flags | 0x81);
            rrj_write32(actor + 484, (uint32_t)-race_leaf_signed_ratio(squared, rrj_s32(2u * distance)));
        }
    }
    if (rrj_u16(rrj_at(actor + 362, 2)))
        xport_update_u8(actor + 509, XPORT_MEMORY_UPDATE_AND, 0xFD);
    return 0;
}

uint32_t sub_8009BA5C(uint32_t actor)
{
    uint32_t candidate = rrj_read32(0x800CE500);
    uint32_t stride = rrj_read32(0x800CE504);
    uint32_t result = 0;
    int32_t best_distance = -65536;
    int32_t count = rrj_s32(rrj_read32(rrj_read32(0x800CE50C)));
    FUNCTION_MARKER(0x8009BA5C, "RASHCDG.BIN");
    while (count >= 0)
    {
        if (rrj_u16(rrj_at(candidate + 172, 2)) && rrj_read32(candidate + 360) == rrj_read32(actor + 360) && rrj_s32(rrj_read32(candidate + 364) ^ rrj_read32(actor + 364)) >= 0)
        {
            int32_t direction = rrj_s32(rrj_read32(actor + 364));
            int32_t actor_position = rrj_s32(rrj_read32(actor + 368));
            int32_t candidate_position = rrj_s32(rrj_read32(candidate + 368));
            if ((direction > 0 && actor_position < candidate_position) || (direction < 0 && candidate_position < actor_position))
            {
                int32_t distance = rrj_s32(leaf_abs32(actor_position - candidate_position));
                if (best_distance < 0 || distance < best_distance)
                {
                    best_distance = distance;
                    result = candidate;
                }
            }
        }
        --count;
        candidate += stride;
    }
    return result;
}

uint32_t sub_800A05D4(uint32_t actor)
{
    uint32_t index = r_u8(actor + 511);
    uint32_t target;
    int32_t delta;
    FUNCTION_MARKER(0x800A05D4, "RASHCDG.BIN");
    if (rrj_s32(index) >= rrj_s32(rrj_read32(0x8005B1F8)))
        return 0;
    target = rrj_read32(0x8005B3A0) + 1096 * index;
    if (rrj_read32(rrj_read32(target + 852) + 604) >= 3)
        target = rrj_read32(target + 852);
    if (rrj_read32(actor + 360) != rrj_read32(target + 360))
        return 0;
    delta = rrj_s32(rrj_read32(target + 368) - rrj_read32(actor + 368));
    if ((rrj_read32(actor + 360) >> 16) == 1)
        return leaf_abs32(delta) <= 196607;
    if (rrj_s32((uint32_t)delta ^ rrj_read32(actor + 364)) >= 0)
        return 0;
    if (leaf_abs32(delta) > 786432 || rrj_s32(rrj_read32(actor + 368)) <= 196607)
        return 1;
    return rrj_s32((uint32_t)sub_8003C520(actor + 328) - 196608) < rrj_s32(rrj_read32(actor + 368));
}

int32_t sub_800BF0F8(uint16_t packed_id, uint32_t actor)
{
    uint32_t group = packed_id >> 5;
    uint32_t table = 0x800CE4D0 + 16 * group;
    uint32_t candidate = rrj_read32(table) + rrj_read32(table + 4) * (packed_id & 31);
    int32_t separation;
    int32_t orientation;
    int32_t relative_speed;
    int32_t threshold;
    uint32_t candidate_group;
    uint32_t candidate_radius;
    uint32_t actor_radius;
    uint32_t orientation_abs;
    FUNCTION_MARKER(0x800BF0F8, "RASHCDG.BIN");
    if (rrj_read32(candidate + 360) == rrj_read32(actor + 360) && (!(rrj_read32(candidate + 360) >> 16) || rrj_read32(candidate + 336) == rrj_read32(actor + 336)))
    {
        separation = rrj_s32(rrj_read32(candidate + 344) - rrj_read32(actor + 344));
        if (rrj_s32(rrj_read32(actor + 364)) < 0)
            separation = rrj_s32(0u - (uint32_t)separation);
    }
    else
    {
        separation = rrj_s32(sub_800B6AAC(rrj_at(candidate + 184, 12), rrj_at(actor + 432, 6), rrj_at(actor + 184, 12)));
    }
    if (!group && rrj_read32(candidate + 856))
    {
        uint32_t linked = rrj_read32(candidate + 856);
        uint32_t same_direction = rrj_s32(rrj_read32(candidate + 364) ^ rrj_read32(actor + 364)) >= 0;
        uint32_t positive_separation = separation >= 0;
        if (same_direction == positive_separation)
        {
            uint32_t combined_radius = rrj_read32(candidate + 304) + rrj_read32(linked + 304);
            separation = same_direction ? rrj_s32((uint32_t)separation + combined_radius) : rrj_s32((uint32_t)separation - combined_radius);
        }
    }
    orientation = rrj_s32(sub_8002E698(rrj_at(candidate + 444, 6), rrj_at(actor + 444, 6)));
    orientation_abs = leaf_abs32(orientation);
    candidate_radius = rrj_read32(candidate + 304);
    actor_radius = rrj_read32(actor + 304);
    threshold = rrj_s32(candidate_radius + actor_radius);
    if (orientation_abs < 0x4000)
        threshold = rrj_s32((uint32_t)threshold + rrj_read32(candidate + 308) - candidate_radius);
    if (orientation_abs >= 0x4000 && orientation_abs <= 0xDDB1)
    {
        int32_t diagonal = (3 * rrj_s32(rrj_read32(candidate + 304) + rrj_read32(candidate + 308))) / 4;
        threshold = rrj_s32((uint32_t)threshold + (uint32_t)(diagonal - rrj_s32(candidate_radius)));
    }
    if (rrj_s32(leaf_abs32(separation)) > threshold)
        return 0x7FFF0000;
    candidate_group = rrj_u16(rrj_at(candidate + 172, 2)) >> 5;
    relative_speed = rrj_s32(rrj_read32(actor + 480));
    if (candidate_group < 4 && orientation_abs >= 0x4000)
    {
        if (orientation >= 0)
            relative_speed = rrj_s32((uint32_t)relative_speed - rrj_read32(candidate + 480));
        else
            relative_speed = rrj_s32((uint32_t)relative_speed + rrj_read32(candidate + 480));
    }
    if ((rrj_read32(actor + 480) || rrj_read32(candidate + 480)) && relative_speed < 131)
        return 0x7FFF0000;
    separation = rrj_s32(sub_800B6AAC(rrj_at(candidate + 184, 12), rrj_at(actor + 444, 6), rrj_at(actor + 184, 12)));
    if (candidate_group)
        separation = rrj_s32((uint32_t)separation - (2u * rrj_read32(actor + 308) + rrj_read32(candidate + 308)));
    else
        separation = rrj_s32((uint32_t)separation - rrj_read32(actor + 308));
    if (separation < 0)
        return actor_radius < leaf_abs32(separation) ? 0x7FFF0000 : separation;
    if (relative_speed < 132)
        return separation;
    return race_leaf_signed_ratio(separation, relative_speed);
}

uint32_t sub_8009B728(uint32_t actor, uint32_t delta)
{
    const uint32_t nearest_out = 0x1F800280;
    uint32_t timer;
    uint32_t player_index;
    uint32_t player;
    uint32_t reference;
    int32_t facing_dot;
    int32_t path_delta;
    int16_t segment_value;
    FUNCTION_MARKER(0x8009B728, "RASHCDG.BIN");
    if (r_u8(actor + 510) & 1)
        return 0x80060000;
    timer = rrj_read32(0x8005B2E0) + delta;
    rrj_write32(0x8005B2E0, timer);
    if (timer <= 0x20000)
        return 0;
    if (rrj_s32(rrj_read32(actor + 484)) > 0 && rrj_s32(rrj_read32(actor + 480)) > 0)
        return rrj_read32(actor + 480);
    player_index = sub_800A04B0(actor, 0, 0);
    player = rrj_read32(0x8005B268 + 4 * player_index);
    reference = 0x800CD898 + 1132 * player_index;
    facing_dot = rrj_s32((uint32_t)sub_8001FC90(rrj_s32(rrj_read32(actor + 184) - rrj_read32(reference + 184)), (int32_t)r_s16(reference + 444) * 16));
    facing_dot = rrj_s32((uint32_t)facing_dot + (uint32_t)sub_8001FC90(rrj_s32(rrj_read32(actor + 188) - rrj_read32(reference + 188)), (int32_t)r_s16(reference + 446) * 16));
    facing_dot = rrj_s32((uint32_t)facing_dot + (uint32_t)sub_8001FC90(rrj_s32(rrj_read32(actor + 192) - rrj_read32(reference + 192)), (int32_t)r_s16(reference + 448) * 16));
    if (facing_dot >= 0 && rrj_s32(rrj_read32(actor + 480)) >= 132 && !(rrj_read32(actor + 360) >> 16) && rrj_read32(actor + 360) == rrj_read32(player + 360) && !(r_u8(actor + 509) & 0x13))
    {
        path_delta = rrj_s32(rrj_read32(actor + 368) - rrj_read32(player + 368));
        if (leaf_abs32(path_delta) >= 0x280000 && leaf_abs32(path_delta) <= 0x500000 && rrj_read32(actor + 372))
        {
            uint32_t segment = rrj_read32(actor + 372);
            segment_value = r_s16(segment + (rrj_s32(rrj_read32(actor + 364)) > 0 ? 140 : 12));
            if (segment_value >= 2 && sub_8003A5F4(actor + 360, nearest_out, 1) > 0x640000)
            {
                uint32_t nearest = sub_8009BA5C(actor);
                if (!nearest || leaf_abs32(rrj_s32(rrj_read32(nearest + 368) - rrj_read32(actor + 368))) > 0x190000)
                {
                    if ((sub_8001FC58() & 0x64) < 0x28)
                    {
                        uint8_t state = r_u8(actor + 508);
                        if (segment_value >= 3)
                        {
                            if (state == 2)
                                state = (rrj_u16(rrj_at(actor + 370, 2)) & 1) ? 3 : 1;
                            else
                                state = 2;
                        }
                        else if (state != 2)
                        {
                            state ^= 3;
                        }
                        w_u8(actor + 508, state);
                        xport_update_u8(actor + 510, XPORT_MEMORY_UPDATE_OR, 3);
                    }
                }
            }
        }
    }
    rrj_write32(0x8005B2E0, 0);
    return 0x80060000;
}

uint32_t sub_800BE7D8(uint32_t actor)
{
    const uint32_t candidates = 0x1F800300;
    const uint32_t count_out = 0x1F800340;
    uint16_t selected = 224;
    int32_t best = 655360;
    uint32_t count;
    uint32_t index;
    uint8_t flags;
    FUNCTION_MARKER(0x800BE7D8, "RASHCDG.BIN");
    rrj_write32(count_out, 16);
    (void)sub_8008AE94(candidates, count_out, actor + 184, 3955095, 59, rrj_u16(rrj_at(actor + 172, 2)));
    count = rrj_read32(count_out);
    for (index = 0; index < count; ++index)
    {
        uint16_t candidate = rrj_u16(rrj_at(candidates + 2 * index, 2));
        int32_t value = sub_800BF0F8(candidate, actor);
        if (value < best)
        {
            best = value;
            selected = candidate;
        }
    }
    flags = r_u8(actor + 509);
    if (selected == 224)
    {
        if (flags & 0x30)
            rrj_write32(actor + 484, 0x26666);
        w_u8(actor + 509, flags & 0xCF);
        return 0;
    }
    {
        uint32_t group = selected >> 5;
        uint32_t record;
        if (group == 6)
            record = rrj_read32(0x800CD6C4) + 280 * (selected & 31);
        else
        {
            uint32_t table = 0x800CE4D0 + 16 * group;
            record = rrj_read32(table) + rrj_read32(table + 4) * (selected & 31) + 172;
        }
        if (rrj_s32(rrj_read32(actor + 480)) >= 132)
        {
            int32_t speed = rrj_s32(rrj_read32(actor + 480));
            int32_t diameter = rrj_s32(2u * rrj_read32(actor + 308));
            int32_t time = best - race_leaf_signed_ratio(diameter, speed);
            if (time >= 3276)
                rrj_write32(actor + 484, (uint32_t)-race_leaf_signed_ratio(speed, time));
            else
            {
                rrj_write32(actor + 484, 0);
                rrj_write32(actor + 480, 0);
            }
        }
        if (!(flags & 0x10))
        {
            flags |= 0x80;
            w_u8(actor + 509, flags);
        }
        w_u8(actor + 509, flags | 0x10);
        return record;
    }
}

uint32_t sub_8009A298(uint32_t delta)
{
    const uint32_t scratch = 0x1F800360;
    int32_t remaining = rrj_s32(rrj_read32(rrj_read32(0x800CE50C)));
    uint32_t actor = rrj_read32(0x800CE500);
    uint32_t stride = rrj_read32(0x800CE504);
    FUNCTION_MARKER(0x8009A298, "RASHCDG.BIN");
    while (remaining >= 0)
    {
        uint32_t player_controlled = 0;
        uint32_t movement_flags = 0;
        uint32_t old_piece;
        uint32_t new_piece;
        uint32_t old_offset;
        uint16_t old_path;
        int32_t travel;
        if (!actor || !rrj_u16(rrj_at(actor + 172, 2)) || !sub_8009FE90(actor))
            goto next_actor;
        rrj_put16(rrj_at(actor + 320, 2), (uint16_t)sub_80039F68(actor + 172));
        if (!r_s16(actor + 320))
            goto detach_actor;
        rrj_write32(actor + 468, rrj_read32(actor + 184));
        rrj_write32(actor + 472, rrj_read32(actor + 188));
        rrj_write32(actor + 476, rrj_read32(actor + 192));
        if ((r_u8(rrj_read32(0x8005B2F8) + 4) & 1) && !rrj_read32(actor + 180))
        {
            player_controlled = 1;
            if (sub_800A05D4(actor))
            {
                rrj_write32(actor + 484, 0);
                rrj_write32(actor + 480, 0);
            }
        }
        if ((uint32_t)(r_u8(rrj_read32(0x8005B2F8) + 57) - 1) < 2)
        {
            uint32_t kind = rrj_read32(actor + 180);
            if (kind && kind != 15)
            {
                uint32_t primary = rrj_read32(0x8005B38C);
                if (!rrj_read32(rrj_read32(primary + 1084) + 40))
                {
                    int32_t position = rrj_s32(rrj_read32(actor + 368) - ((uint32_t)rrj_u16(rrj_at(0x8005317A, 2)) << 16));
                    if (rrj_s32(rrj_read32(actor + 364) ^ rrj_read32(0x8005B2E8)) < 0)
                    {
                        if (position > 0 && position <= 5898239)
                            rrj_write32(actor + 484, 0xFFFD999A);
                    }
                    else if (position > -1310720 && position <= 2621439)
                    {
                        rrj_write32(actor + 484, 0xFFFD999A);
                    }
                }
            }
            else if (rrj_s32(rrj_read32(actor + 480)) < 131 && rrj_s32(rrj_read32(actor + 484)) < 131)
            {
                rrj_write32(actor + 484, 0);
                rrj_write32(actor + 480, 0);
                goto next_actor;
            }
        }
        {
            int32_t speed = rrj_s32(rrj_read32(actor + 480) + (uint32_t)sub_8001FC90(rrj_s32(rrj_read32(actor + 484)), rrj_s32(delta)));
            int32_t maximum = 0x141DDD;
            rrj_write32(actor + 480, (uint32_t)speed);
            if (speed < 0)
            {
                rrj_write32(actor + 484, 0);
                rrj_write32(actor + 480, 0);
            }
            else if (!player_controlled && (r_u8(actor + 509) & 8) && speed > 585944)
            {
                rrj_write32(actor + 480, 585944);
                rrj_write32(actor + 484, 0);
            }
            else
            {
                if (player_controlled)
                {
                    uint32_t race = rrj_read32(0x8005B2F8);
                    int32_t divisor = rrj_s32(rrj_read32(0x800530F0 + 4 * rrj_read32(race + 60)));
                    int32_t target = race_leaf_signed_ratio(10223616, divisor);
                    uint32_t primary = rrj_read32(0x8005B38C);
                    uint32_t other_index = (r_u8(rrj_read32(primary + 1084) + 1) & 15) != 2;
                    uint32_t other = rrj_read32(0x8005B268 + 4 * other_index);
                    if (other)
                    {
                        int32_t difference = rrj_s32((uint32_t)target - rrj_read32(other + 480));
                        maximum = difference > 0xFFFF ? difference : 0x10000;
                    }
                }
                if (rrj_s32(rrj_read32(actor + 480)) >= maximum)
                {
                    rrj_write32(actor + 480, (uint32_t)maximum);
                    rrj_write32(actor + 484, 0);
                }
            }
        }
        old_piece = rrj_read32(actor + 340);
        old_offset = rrj_read32(actor + 348);
        old_path = rrj_u16(rrj_at(actor + 362, 2));
        travel = rrj_s32((uint32_t)sub_8001FC90(rrj_s32(delta), rrj_s32(rrj_read32(actor + 480))));
        new_piece = (uint32_t)sub_8003775C(actor + 172, actor + 328, (uint32_t)travel);
        if (old_piece == new_piece)
        {
            int32_t direction = rrj_s32(rrj_read32(actor + 364));
            if ((direction > 0 && travel >= rrj_s32(rrj_read32(old_piece + 32) - old_offset)) || (direction < 0 && travel >= rrj_s32(old_offset)))
                goto invalidate_actor;
        }
        else
        {
            int32_t alignment = rrj_s32(sub_8002E698(rrj_at(new_piece + 14, 6), rrj_at(actor + 450, 6)));
            uint32_t index;
            for (index = 0; index < 3; ++index)
            {
                uint16_t tangent = rrj_u16(rrj_at(new_piece + 14 + 2 * index, 2));
                uint16_t normal = rrj_u16(rrj_at(new_piece + 2 + 2 * index, 2));
                rrj_put16(rrj_at(actor + 444 + 2 * index, 2), tangent);
                rrj_put16(rrj_at(actor + 432 + 2 * index, 2), normal);
                rrj_put16(rrj_at(actor + 450 + 2 * index, 2), tangent);
            }
            if (alignment < 0)
            {
                movement_flags |= 2;
                for (index = 0; index < 3; ++index)
                {
                    rrj_put16(rrj_at(actor + 444 + 2 * index, 2), (uint16_t)-r_s16(actor + 444 + 2 * index));
                    rrj_put16(rrj_at(actor + 432 + 2 * index, 2), (uint16_t)-r_s16(actor + 432 + 2 * index));
                    rrj_put16(rrj_at(actor + 450 + 2 * index, 2), (uint16_t)-r_s16(actor + 450 + 2 * index));
                }
            }
            for (index = 0; index < 3; ++index)
                rrj_put16(rrj_at(actor + 438 + 2 * index, 2), (uint16_t)-r_s16(new_piece + 8 + 2 * index));
            {
                uint32_t angle = sub_80020018((uint32_t)((int32_t)r_s16(actor + 444) * 16), (uint32_t)((int32_t)r_s16(actor + 448) * 16));
                uint32_t table = 0x8005624C + 4 * (angle & 0xFFF);
                rrj_write32(actor + 292, angle);
                rrj_write32(actor + 296, (uint32_t)((int32_t)r_s16(table + 2) * 16));
                rrj_write32(actor + 300, (uint32_t)((int32_t)r_s16(table) * 16));
            }
        }
        if (!sub_8009FE90(actor))
            goto next_actor;
        (void)sub_8003662C(actor + 450, actor + 328, actor + 360);
        {
            uint32_t hint = sub_8003DDB0(actor);
            (void)sub_8003DE28(actor, 0, 0, hint);
            sub_8003DF54(actor, 0, hint);
        }
        (void)sub_8003A9D8(actor);
        sub_8003AF9C(actor + 172, 1, 0);
        rrj_write32(actor + 324, sub_8003B61C(actor + 172));
        (void)sub_8002EAD8(rrj_at(new_piece + 20, 12), rrj_at(new_piece + 14, 6), rrj_read32(actor + 348), rrj_at(scratch, 12));
        {
            int32_t first = sub_8009E768(actor);
            int32_t second = sub_8009E768(actor);
            int32_t third = sub_8009E768(actor);
            int32_t target = rrj_s32((uint32_t)(first >> 31) + (uint32_t)second) ^ (third >> 31);
            int32_t lateral = rrj_s32(rrj_read32(actor + 344));
            if (r_s8(actor + 508) < 0)
                target = -target;
            lateral = rrj_s32(leaf_abs32(lateral));
            if (target != lateral)
            {
                int32_t speed = rrj_s32(rrj_read32(actor + 480));
                int32_t divisor = ((r_u8(actor + 509) & 0x10) && speed >= 132) ? 327680 : 655360;
                int32_t rate = race_leaf_signed_ratio(speed, divisor);
                int32_t step = rrj_s32((uint32_t)sub_8001FC90(rrj_s32(delta), rate));
                uint32_t difference = leaf_abs32(rrj_s32((uint32_t)target - (uint32_t)lateral));
                if (leaf_abs32(step) > difference)
                    step = rrj_s32(difference);
                lateral = lateral >= target ? rrj_s32((uint32_t)lateral - (uint32_t)step) : rrj_s32((uint32_t)lateral + (uint32_t)step);
                rrj_write32(actor + 344, (uint32_t)lateral);
                movement_flags |= 2;
                if ((r_u8(actor + 510) & 2) && leaf_abs32(step) >= 4097)
                    movement_flags |= 4;
            }
            if (!(movement_flags & 4))
                xport_update_u8(actor + 510, XPORT_MEMORY_UPDATE_AND, 0xFD);
            if ((movement_flags & 2) || old_path != rrj_u16(rrj_at(actor + 362, 2)))
            {
                int32_t adjusted = rrj_s32(leaf_abs32(rrj_s32(rrj_read32(actor + 344))));
                if (rrj_s32(rrj_read32(actor + 364)) <= 0)
                    adjusted = -adjusted;
                rrj_write32(actor + 344, (uint32_t)adjusted);
            }
            if (r_s8(actor + 508) < 0)
                rrj_write32(actor + 344, 0u - rrj_read32(actor + 344));
        }
        (void)sub_8002EAD8(rrj_at(scratch, 12), rrj_at(new_piece + 2, 6), rrj_read32(actor + 344), rrj_at(actor + 184, 12));
        movement_flags |= sub_8009FCDC(actor);
        {
            uint32_t contact = sub_800BE7D8(actor);
            if (contact && (r_u8(actor + 509) & 0x10))
            {
                if (!(movement_flags & 1))
                    sub_8009F9EC(actor, contact);
                if (sub_8009F6F8(actor, contact))
                    goto invalidate_actor;
            }
        }
        {
            int32_t threshold = rrj_s32((0x141DDDu << player_controlled) >> 1);
            if (rrj_s32(rrj_read32(actor + 480)) < threshold && !(r_u8(actor + 509) & 0x11))
                rrj_write32(actor + 484, 0x26666);
        }
        (void)sub_8009B4C0(actor, delta, old_piece != new_piece);
        xport_update_u8(actor + 509, XPORT_MEMORY_UPDATE_AND, 0x7F);
        (void)sub_8008BA18(actor);
        (void)sub_8009B728(actor, delta);
        goto next_actor;
    invalidate_actor:
        rrj_put16(rrj_at(actor + 320, 2), 0);
    detach_actor:
        (void)sub_8008C000(actor + 172, 3);
    next_actor:
        --remaining;
        actor += stride;
    }
    return stride;
}

uint32_t sub_80074D58(uint32_t actor, RRJRaceLeafCall call, RRJReverbCall reverb)
{
    FUNCTION_MARKER(0x80074D58u, "RASHCDG.BIN");
    uint32_t linked = rrj_read32(actor + 856);
    uint32_t result = rrj_read32(actor + 560) & 0x400;
    if (result)
    {
        uint32_t state = rrj_read32(linked + 852);
        result = rrj_read32(state + 604) < 2;
        if (result)
        {
            rrj_write32(linked + 568, rrj_read32(linked + 568) | 8);
            rrj_put16(rrj_at(state + 456, 2), rrj_u16(rrj_at(actor + 450, 2)));
            rrj_put16(rrj_at(state + 458, 2), rrj_u16(rrj_at(actor + 452, 2)));
            rrj_put16(rrj_at(state + 460, 2), rrj_u16(rrj_at(actor + 454, 2)));
            rrj_write32(state + 552, rrj_read32(state + 552) | 0x200000);
            rrj_write32(state + 480, rrj_read32(actor + 480) + 878916);
            (void)sub_80090D84(state, call, reverb);
            rrj_write32(linked + 568, rrj_read32(linked + 568) & ~8u);
            if (!rrj_read32(actor + 616))
            {
                rrj_write32(actor + 624, 3003949);
                rrj_write32(actor + 632, 0);
                rrj_write32(actor + 620, 0xFFF80000);
                rrj_write32(actor + 628, 0);
            }
            result = rrj_read32(actor + 560) & 0xFFFFFBFFu;
            rrj_write32(actor + 560, result);
        }
    }
    return result;
}

uint32_t sub_80074E6C(uint32_t actor, uint32_t delta, RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x80074E6Cu, "RASHCDG.BIN");
    uint32_t flags = rrj_read32(actor + 564);
    uint32_t controls;
    if (flags & 0x200)
    {
        uint32_t timer = rrj_read32(actor + 704) + delta;
        rrj_write32(actor + 704, timer);
        if (rrj_s32(rrj_read32(0x800D3974)) < rrj_s32(timer))
            flags &= ~0x200u;
        rrj_write32(actor + 564, flags);
    }
    controls = rrj_read32(actor + 560);
    if (controls & 2)
    {
        if (controls & 4)
        {
            int8_t count = r_s8(actor + 848);
            if (count > 0 && (controls & 8))
            {
                rrj_write32(actor + 704, 0);
                rrj_write32(actor + 564, rrj_read32(actor + 564) | 0x200);
                w_u8(actor + 848, (uint8_t)(count - 1));
                if (leaf_call(rrj_host_context(), call, 0x80027028, actor, 0, 0, 0))
                    (void)leaf_call(rrj_host_context(), call, 0x80027540, actor, 0, 0, 0);
                if (leaf_call(rrj_host_context(), call, 0x80027028, actor, 1, 0, 0))
                    (void)leaf_call(rrj_host_context(), call, 0x80027540, actor, 1, 2, 0);
            }
            else
            {
                if (!(controls & 0x800))
                    controls |= 0x1800;
                rrj_write32(actor + 560, controls);
            }
            rrj_write32(actor + 560, rrj_read32(actor + 560) & ~4u);
        }
    }
    else
        rrj_write32(actor + 564, rrj_read32(actor + 564) & ~0x200u);
    flags = rrj_read32(actor + 564);
    if (rrj_read32(actor + 560) & 0x40)
        flags &= ~0x200u;
    rrj_write32(actor + 564, flags);
    return 0xFFFFFDFFu;
}

uint32_t sub_8007EF60(uint32_t specification, int32_t scale, int32_t lateral, int32_t grip)
{
    FUNCTION_MARKER(0x8007EF60u, "RASHCDG.BIN");
    int32_t projected = (int32_t)sub_8001FC90(lateral, rrj_s32(rrj_read32(specification + 192)));
    int32_t first_square = (int32_t)sub_8001FC90(projected, projected);
    int32_t second = (int32_t)sub_8001FC90(grip, scale);
    int32_t difference = (int32_t)sub_8001FC90(second, second) - first_square;
    int32_t root;
    if (difference < 0)
        return 0;
    root = (int32_t)(sub_8004CF74((uint32_t)difference) << 2);
    return (uint32_t)race_leaf_signed_ratio(root, rrj_s32(rrj_read32(specification + 200)));
}

uint32_t sub_8007E868(uint32_t vector, int32_t limit, int32_t minimum, int32_t value)
{
    FUNCTION_MARKER(0x8007E868u, "RASHCDG.BIN");
    int32_t target = value;
    int32_t x;
    int32_t y;
    int32_t z;
    int32_t length;
    int32_t remaining;
    int32_t scale;
    if (value < limit)
    {
        uint32_t quotient;
        if (value <= 0)
            quotient = limit > 0 ? sub_80010028((uint32_t)-value, (uint32_t)limit) : 0u - sub_80010028((uint32_t)-value, (uint32_t)-limit);
        else
            quotient = limit > 0 ? 0u - sub_80010028((uint32_t)value, (uint32_t)limit) : sub_80010028((uint32_t)value, (uint32_t)-limit);
        target = (int16_t)(quotient >> 4);
        if (target < (int16_t)minimum)
            target = (int16_t)minimum;
    }
    if (r_s16(vector + 2) == (int16_t)target)
        return (uint32_t)(uint16_t)r_s16(vector + 2);
    x = (int32_t)r_s16(vector) << 4;
    y = (int16_t)target << 4;
    z = (int32_t)r_s16(vector + 4) << 4;
    rrj_put16(rrj_at(vector + 2, 2), (uint16_t)target);
    length = (int32_t)sub_8001FC90(x, x) + (int32_t)sub_8001FC90(z, z);
    if (length < 6)
    {
        x = 0;
        z = 655;
        length = 6;
    }
    remaining = 65536 - (int32_t)sub_8001FC90(y, y);
    scale = (int32_t)(sub_8004CF74((uint32_t)race_leaf_signed_ratio(remaining, length)) << 2);
    rrj_put16(rrj_at(vector, 2), (uint16_t)((int32_t)sub_8001FC90(scale, x) >> 4));
    target = (int32_t)sub_8001FC90(scale, z) >> 4;
    rrj_put16(rrj_at(vector + 4, 2), (uint16_t)target);
    return (uint32_t)target;
}

uint32_t sub_8008EE60(uint32_t actor)
{
    FUNCTION_MARKER(0x8008EE60u, "RASHCDG.BIN");
    uint32_t config = rrj_read32(actor + 540);
    uint32_t body = rrj_read32(actor + 596);
    uint32_t record = rrj_read32(config + 4) + 12 * rrj_read32(config + 12);
    uint32_t table = rrj_read32(rrj_read32(config + 40) + 4);
    uint32_t item = rrj_read32(table + 4 * r_u8(record));
    int32_t divisor = (int16_t)(rrj_u16(rrj_at(item + 16, 2)) - 1);
    int32_t fraction = (int32_t)((rrj_read32(config + 32) << 16) / rrj_read32(config + 24));
    int32_t longitudinal = (int32_t)sub_8001FC90(-71683, (int32_t)(rrj_read32(config + 16) << 16) + fraction) / divisor;
    uint32_t axis;
    uint32_t i;
    for (axis = 0; axis < 3; ++axis)
    {
        int32_t position = rrj_s32(rrj_read32(body + 184 + 4 * axis));
        position += (int32_t)sub_8001FC90(32768, (int32_t)r_s16(body + 432 + 2 * axis) << 4);
        position += (int32_t)sub_8001FC90(-60417, (int32_t)r_s16(body + 438 + 2 * axis) << 4);
        position += (int32_t)sub_8001FC90(longitudinal, (int32_t)r_s16(body + 444 + 2 * axis) << 4);
        rrj_write32(actor + 184 + 4 * axis, (uint32_t)position);
    }
    for (i = 0; i < 9; ++i)
        rrj_put16(rrj_at(actor + 432 + 2 * i, 2), rrj_u16(rrj_at(body + 432 + 2 * i, 2)));
    for (i = 0; i < 3; ++i)
        rrj_put16(rrj_at(actor + 450 + 2 * i, 2), rrj_u16(rrj_at(body + 450 + 2 * i, 2)));
    i = rrj_read32(body + 480);
    if (rrj_s32(i) <= 327679)
        i = 327680;
    rrj_write32(actor + 480, i);
    return i;
}

uint32_t sub_80072C7C(uint32_t actor, uint32_t delta)
{
    FUNCTION_MARKER(0x80072C7Cu, "RASHCDG.BIN");
    uint32_t linked = rrj_read32(actor + 856);
    uint32_t flags;
    uint32_t specification;
    uint32_t choose_target;
    int32_t target = 0;
    int32_t current;
    int32_t rate = 0;
    int32_t next;
    if (rrj_read32(linked + 720))
        return 0;
    flags = rrj_read32(linked + 560);
    specification = rrj_read32(actor + 556);
    choose_target = (flags >> 20) & 1;
    if (!rrj_read32(actor + 576))
    {
        flags &= ~0x300u;
        choose_target = 0;
    }
    if (choose_target)
    {
        uint32_t player = rrj_u16(rrj_at(actor + 172, 2));
        uint32_t count = rrj_read32(rrj_read32(0x8005B2F8) + 48);
        uint32_t mask;
        target = rrj_s32(rrj_read32(0x800CE540 + 8 * (player + (count < 2 ? 1 : 2)) + 4));
        current = rrj_s32(rrj_read32(linked + 672));
        mask = target < current ? 0x100u : current < target ? 0x200u : 0;
        flags |= mask | ((mask && !(flags & mask)) << 7);
        if (mask == 0x200)
            flags &= ~0x100u;
        else if (mask == 0x100)
            flags &= ~0x200u;
        rrj_write32(linked + 560, flags);
    }
    current = rrj_s32(rrj_read32(linked + 672));
    if (flags & 0x100)
        rate = flags & 0x200 ? 0 : -rrj_s32(rrj_read32(specification + 428));
    else if (flags & 0x200)
        rate = rrj_s32(rrj_read32(specification + 428));
    else if (!choose_target)
    {
        choose_target = 1;
        if (current > 0)
            rate = -2 * rrj_s32(rrj_read32(specification + 428));
        else if (current < 0)
            rate = 2 * rrj_s32(rrj_read32(specification + 428));
    }
    next = current + (int32_t)sub_8001FC90(rate, rrj_s32(delta));
    if (choose_target)
    {
        if (current == target || (current < target && next > target) || (current > target && next < target))
            next = target;
    }
    if (next < -65536)
        next = -65536;
    if (next > 65536)
        next = 65536;
    rrj_write32(linked + 672, (uint32_t)next);
    if (rrj_s32(rrj_read32(actor + 576)) <= 32768)
        return 0;
    next = (int32_t)sub_8001FC90(next, rrj_s32(rrj_read32(specification + 432)));
    return (uint32_t)race_leaf_signed_ratio(next, rrj_s32(rrj_read32(actor + 576)));
}

uint32_t sub_80073D74(uint32_t actor)
{
    FUNCTION_MARKER(0x80073D74u, "RASHCDG.BIN");
    int32_t yaw = rrj_s32(rrj_read32(actor + 636));
    int32_t speed = rrj_s32(rrj_read32(actor + 576));
    int32_t steering;
    uint32_t result;
    if (!yaw || speed <= 32768)
    {
        int32_t prior = r_s16(actor + 826);
        rrj_write32(actor + 488, 0);
        result = (uint32_t)(prior / 2);
    }
    else
    {
        uint32_t specification = rrj_read32(actor + 556);
        int32_t angle = yaw + rrj_s32(rrj_read32(actor + 668));
        int32_t curve;
        int32_t tangent;
        int32_t first = (int32_t)sub_8001FC90(rrj_s32(rrj_read32(specification + 280)), speed) + 65536;
        int32_t second = (int32_t)sub_8001FC90(rrj_s32(rrj_read32(specification + 284)), speed) + 65536;
        if (angle < -91505)
            angle = -91505;
        if (angle > 91505)
            angle = 91505;
        curve = rrj_s32((uint32_t)sub_8001FEB4((uint32_t)((652LL * angle) >> 16)));
        tangent = (int32_t)sub_8001FC90(rrj_s32(rrj_read32(actor + 736)), curve) + rrj_s32(rrj_read32(actor + 732));
        steering = race_leaf_signed_ratio(first, second);
        if (steering <= 64879)
        {
            int32_t denominator = (int32_t)sub_8001FC90(65536 - steering, speed);
            int32_t correction = race_leaf_signed_ratio(tangent, denominator);
            int32_t limit = rrj_s32(rrj_read32(specification + 232));
            if (correction < -limit)
                correction = -limit;
            if (correction > limit)
                correction = limit;
            rrj_write32(actor + 488, (uint32_t)correction);
            steering = (int32_t)sub_8001FC90(steering, correction);
        }
        else
            rrj_write32(actor + 488, (uint32_t)steering);
        if (r_s8(actor + 8) >= 2)
            return 0;
        steering = race_leaf_signed_ratio(steering, speed);
        steering = (int32_t)(((int64_t)steering * rrj_s32(rrj_read32(specification + 4))) >> 16);
        if (steering < -51471)
            steering = -51471;
        if (steering > 51471)
            steering = 51471;
        result = (uint32_t)((163 * steering) >> 14);
    }
    rrj_put16(rrj_at(actor + 826, 2), (uint16_t)result);
    return result;
}

uint32_t sub_8008BD2C(uint32_t actor)
{
    FUNCTION_MARKER(0x8008BD2Cu, "RASHCDG.BIN");
    const uint32_t first_address = 0x1F800300;
    const uint32_t second_address = 0x1F800310;
    const uint32_t third_address = 0x1F800320;
    int32_t center[3];
    int32_t first[3];
    int32_t second[3];
    int32_t third[3];
    int32_t corners[8][3];
    static const int8_t first_sign[8] = {-1, 1, 1, -1, -1, 1, 1, -1};
    static const int8_t second_sign[8] = {1, 1, 1, 1, -1, -1, -1, -1};
    static const int8_t third_sign[8] = {-1, -1, 1, 1, -1, -1, 1, 1};
    uint32_t i;
    uint32_t j;
    int32_t middle;
    if (r_s8(actor + 547) != 1)
        return sub_8008BA18(actor);
    for (i = 0; i < 3; ++i)
        center[i] = rrj_s32(rrj_read32(actor + 184 + 4 * i)) + rrj_s32(rrj_read32(actor + 244 + 4 * i));
    middle = rrj_s32(rrj_read32(actor + 312));
    if (rrj_u16(rrj_at(0x800541D6 + 8 * rrj_u16(rrj_at(actor + 544, 2)), 2)) == 5)
        middle /= 3;
    else
        middle /= 2;
    (void)sub_8002EE50(rrj_read32(actor + 304), rrj_at(actor + 256, 6), rrj_at(first_address, 12));
    (void)sub_8002EE50((uint32_t)middle, rrj_at(actor + 262, 6), rrj_at(second_address, 12));
    (void)sub_8002EE50(rrj_read32(actor + 308), rrj_at(actor + 268, 6), rrj_at(third_address, 12));
    for (i = 0; i < 3; ++i)
    {
        first[i] = rrj_s32(rrj_read32(first_address + 4 * i));
        second[i] = rrj_s32(rrj_read32(second_address + 4 * i));
        third[i] = rrj_s32(rrj_read32(third_address + 4 * i));
    }
    for (i = 0; i < 8; ++i)
        for (j = 0; j < 3; ++j)
            corners[i][j] = first_sign[i] * first[j] + second_sign[i] * second[j] + third_sign[i] * third[j];
    for (i = 0; i < 8; ++i)
        for (j = 0; j < 3; ++j)
            rrj_write32(actor + 196 + 12 * i + 4 * j, (uint32_t)(center[j] + corners[i][j]));
    w_u8(actor + 547, 2);
    return 2;
}

uint32_t sub_8009246C(uint32_t actor)
{
    FUNCTION_MARKER(0x8009246Cu, "RASHCDG.BIN");
    const uint32_t position_scratch = 0x1F800300;
    const uint32_t normal_scratch = 0x1F800310;
    uint32_t position = 0;
    uint32_t normal = 0;
    uint32_t inverted = (rrj_read32(actor + 552) >> 30) & 1;
    uint32_t use_surface_position = 0;
    int32_t found;
    int32_t dot = 0;
    uint32_t i;
    if (rrj_read32(actor + 388) & 1)
    {
        position = position_scratch;
        normal = normal_scratch;
        found = rrj_s32(sub_800A7BF8(actor, 0, position, normal, actor + 536));
        rrj_write32(actor + 536, (uint32_t)found);
    }
    else
    {
        found = 0;
        w_u8(actor + 534, rrj_read32(actor + 372) ? r_u8(actor + 394) : 1);
        rrj_write32(actor + 536, 0);
    }
    if (found <= 0)
    {
        uint32_t piece = rrj_read32(actor + 340);
        position = piece + 20;
        normal = piece + 8;
    }
    if (found >= 0)
    {
        if (r_s16(normal + 2) < -614)
        {
            for (i = 0; i < 3; ++i)
                rrj_put16(rrj_at(actor + 522 + 2 * i, 2), (uint16_t)-r_s16(normal + 2 * i));
            use_surface_position = 1;
        }
    }
    else if (!rrj_read32(actor + 480))
    {
        rrj_write32(actor + 480, 117188);
        (void)sub_8002EE50(117188, rrj_at(actor + 450, 6), rrj_at(actor + 456, 12));
    }
    if (!inverted && !use_surface_position)
    {
        position = position_scratch;
        (void)sub_8002EAD8(rrj_at(actor + 184, 12), rrj_at(actor + 522, 6), rrj_read32(actor + 508), rrj_at(position, 12));
    }
    for (i = 0; i < 3; ++i)
    {
        int32_t difference = rrj_s32(rrj_read32(position + 4 * i)) - rrj_s32(rrj_read32(actor + 184 + 4 * i));
        int32_t component = (int32_t)r_s16(actor + 522 + 2 * i) << 4;
        dot += (int32_t)(((int64_t)difference * component) >> 16);
    }
    if (!inverted)
        dot -= rrj_s32(rrj_read32(actor + 508));
    return sub_8002EAD8(rrj_at(actor + 184, 12), rrj_at(actor + 522, 6), (uint32_t)dot, rrj_at(inverted ? actor + 504 : actor + 184, 12));
}

static void race_mount_parameters(RRJMemory *m, uint32_t actor, int32_t *divisor, int32_t *travel)
{
    uint32_t config = rrj_read32(actor + 540);
    uint32_t record = rrj_read32(config + 4) + 12 * rrj_read32(config + 12);
    uint32_t table = rrj_read32(rrj_read32(config + 40) + 4);
    uint32_t item = rrj_read32(table + 4 * r_u8(record));
    *divisor = (int16_t)(rrj_u16(rrj_at(item + 16, 2)) - 1);
    *travel = (int32_t)(rrj_read32(config + 16) << 16) + (int32_t)((rrj_read32(config + 32) << 16) / rrj_read32(config + 24));
}

static void race_copy_halfwords(RRJMemory *m, uint32_t output, uint32_t input, uint32_t count)
{
    uint32_t i;
    for (i = 0; i < count; ++i)
        rrj_put16(rrj_at(output + 2 * i, 2), rrj_u16(rrj_at(input + 2 * i, 2)));
}

static void race_place_from_basis(RRJMemory *m, uint32_t actor, uint32_t origin, uint32_t basis, int32_t first, int32_t second, int32_t third)
{
    uint32_t axis;
    for (axis = 0; axis < 3; ++axis)
    {
        int32_t value = rrj_s32(rrj_read32(origin + 4 * axis));
        value += (int32_t)sub_8001FC90(first, (int32_t)r_s16(basis + 2 * axis) << 4);
        value += (int32_t)sub_8001FC90(second, (int32_t)r_s16(basis + 6 + 2 * axis) << 4);
        value += (int32_t)sub_8001FC90(third, (int32_t)r_s16(basis + 12 + 2 * axis) << 4);
        rrj_write32(actor + 184 + 4 * axis, (uint32_t)value);
    }
}

static int32_t race_inverse_vector_length(int32_t magnitude)
{
    uint32_t absolute = magnitude < 0 ? 0u - (uint32_t)magnitude : (uint32_t)magnitude;
    uint32_t divisor = (absolute >> 1) + (uint32_t)((int32_t)(absolute - 2) >> 31);
    uint32_t scale = 0x80000000u / divisor;
    return magnitude < 0 ? -(int32_t)scale : (int32_t)scale;
}

uint32_t sub_8008E044(uint32_t actor, uint32_t size_out)
{
    FUNCTION_MARKER(0x8008E044u, "RASHCDG.BIN");
    const uint32_t vector = 0x1F800300;
    int32_t divisor;
    int32_t travel;
    uint32_t mirrored = (rrj_read32(actor + 552) >> 27) & 1;
    uint32_t body;
    int32_t first;
    int32_t second;
    int32_t third;
    int32_t magnitude;
    int32_t scale;
    uint32_t i;
    uint32_t result;
    race_mount_parameters(rrj_host_context(), actor, &divisor, &travel);
    if (r_u8(actor + 572) & 0x20)
    {
        first = -77594 - (mirrored ? 65536 : 0);
        second = -59814;
        third = -47874;
        body = rrj_read32(rrj_read32(actor + 596) + 856);
    }
    else
    {
        uint32_t angle;
        int32_t sine;
        body = rrj_read32(actor + 596);
        angle = leaf_abs32((int32_t)(((int64_t)652 * rrj_s32(rrj_read32(body + 652))) >> 16)) & 0xFFF;
        sine = (int32_t)r_s16(0x8005624E + 4 * angle) << 4;
        first = -72862 - (int32_t)sub_8001FC90(65536 - sine, 117964);
        second = -70405;
        third = -18821;
    }
    first = (int32_t)sub_8001FC90(first, travel) / divisor;
    if (mirrored)
        first = -first;
    race_place_from_basis(rrj_host_context(), actor, body + 184, body + 516, first, second, third);
    race_copy_halfwords(rrj_host_context(), actor + 444, body + 522, 3);
    race_copy_halfwords(rrj_host_context(), actor + 438, body + 516, 3);
    race_copy_halfwords(rrj_host_context(), actor + 432, body + 528, 3);
    if (!mirrored)
    {
        for (i = 0; i < 3; ++i)
        {
            rrj_put16(rrj_at(actor + 438 + 2 * i, 2), (uint16_t)-r_s16(actor + 438 + 2 * i));
            rrj_put16(rrj_at(actor + 432 + 2 * i, 2), (uint16_t)-r_s16(actor + 432 + 2 * i));
        }
    }
    scale = (r_u8(actor + 572) & 0x20) ? -786432 + (mirrored ? 1572864 : 0) : -655360 + (mirrored ? 1310720 : 0);
    (void)sub_8002EE50((uint32_t)scale, rrj_at(body + 516, 6), rrj_at(vector, 12));
    for (i = 0; i < 3; ++i)
        rrj_write32(vector + 4 * i, rrj_read32(vector + 4 * i) + rrj_read32(body + 456 + 4 * i));
    magnitude = rrj_s32(sub_8002E548(vector));
    rrj_write32(actor + 480, (uint32_t)magnitude);
    scale = race_inverse_vector_length(magnitude);
    result = sub_8002EED8((uint32_t)scale, vector, actor + 450);
    if (size_out)
    {
        if (travel < 19660)
        {
            rrj_write32(size_out, rrj_read32(actor + 312));
            result = (uint32_t)(rrj_s32(rrj_read32(actor + 312)) / 4);
            rrj_write32(actor + 312, result);
        }
        else if (travel <= 216267)
        {
            rrj_write32(size_out, rrj_read32(actor + 312));
            result = (uint32_t)(rrj_s32(rrj_read32(actor + 312)) / 3);
            rrj_write32(actor + 312, result);
        }
        else
            result = 1;
    }
    return result;
}

uint32_t sub_8008E50C(uint32_t actor)
{
    FUNCTION_MARKER(0x8008E50Cu, "RASHCDG.BIN");
    int32_t divisor;
    int32_t travel;
    uint32_t body;
    uint32_t basis;
    uint32_t origin;
    int32_t first;
    int32_t second;
    int32_t third;
    uint32_t i;
    uint32_t speed;
    race_mount_parameters(rrj_host_context(), actor, &divisor, &travel);
    if (r_u8(actor + 572) & 0x20)
    {
        first = 36929;
        second = -135030;
        third = -21725;
        body = rrj_read32(rrj_read32(actor + 596) + 856);
    }
    else
    {
        body = rrj_read32(actor + 596);
        first = -65;
        second = -121910;
        third = 4384;
    }
    basis = body + 516;
    if (rrj_read32(actor + 552) & 0x10000)
    {
        third = 0;
        second = -170393;
        origin = (rrj_read32(body + 568) & 0x600) ? body + 184 : body + 504;
        race_copy_halfwords(rrj_host_context(), actor + 432, body + 432, 9);
    }
    else
    {
        basis = actor + 432;
        origin = actor + 184;
        for (i = 0; i < 3; ++i)
            rrj_write32(origin + 4 * i, rrj_read32(body + 504 + 4 * i) + rrj_read32(actor + 580 + 4 * i));
        second = (int32_t)sub_8001FC90(second + 42598, travel) / divisor - 42598;
    }
    race_place_from_basis(rrj_host_context(), actor, origin, basis, first, second, third);
    for (i = 0; i < 3; ++i)
        rrj_put16(rrj_at(actor + 450 + 2 * i, 2), (uint16_t)-r_s16(actor + 438 + 2 * i));
    speed = rrj_read32(body + 480);
    if (rrj_s32(speed) <= 327679)
        speed = 327680;
    rrj_write32(actor + 480, speed);
    return speed;
}

uint32_t sub_8008E818(uint32_t actor, uint32_t size_out)
{
    FUNCTION_MARKER(0x8008E818u, "RASHCDG.BIN");
    int32_t divisor;
    int32_t travel;
    uint32_t body;
    int32_t first;
    int32_t second;
    int32_t third;
    uint32_t speed;
    uint32_t result;
    uint32_t i;
    race_mount_parameters(rrj_host_context(), actor, &divisor, &travel);
    if (r_u8(actor + 572) & 0x20)
    {
        first = 36536 + (int32_t)sub_8001FC90(23429 - 36536, travel) / divisor;
        second = -65536 + (int32_t)sub_8001FC90(-90885 + 65536, travel) / divisor;
        third = -51393;
        body = rrj_read32(rrj_read32(actor + 596) + 856);
    }
    else
    {
        first = (int32_t)sub_8001FC90(-11396, travel) / divisor;
        second = -49152 + (int32_t)sub_8001FC90(-71106 + 49152, travel) / divisor;
        third = -18435;
        body = rrj_read32(actor + 596);
    }
    if (rrj_read32(actor + 552) & 0x08000000)
        first = -first;
    race_place_from_basis(rrj_host_context(), actor, body + 184, body + 432, first, second, third);
    race_copy_halfwords(rrj_host_context(), actor + 444, body + 438, 3);
    race_copy_halfwords(rrj_host_context(), actor + 432, body + 432, 3);
    race_copy_halfwords(rrj_host_context(), actor + 438, body + 444, 3);
    for (i = 0; i < 3; ++i)
        rrj_put16(rrj_at(actor + 450 + 2 * i, 2), (uint16_t)-r_s16(actor + 438 + 2 * i));
    if (size_out)
    {
        rrj_write32(size_out, rrj_read32(actor + 312));
        rrj_write32(actor + 312, (uint32_t)(rrj_s32(rrj_read32(actor + 312)) / 2));
    }
    speed = rrj_read32(body + 480);
    result = rrj_s32(speed) > 327679;
    if (!result)
        speed = 327680;
    rrj_write32(actor + 480, speed);
    return result;
}

uint32_t sub_8008EB88(uint32_t actor, uint32_t size_out)
{
    FUNCTION_MARKER(0x8008EB88u, "RASHCDG.BIN");
    int32_t divisor;
    int32_t travel;
    uint32_t body;
    int32_t first;
    int32_t second;
    int32_t third;
    int32_t speed;
    uint32_t i;
    race_mount_parameters(rrj_host_context(), actor, &divisor, &travel);
    if (r_u8(actor + 572) & 0x20)
    {
        first = 36739;
        second = -72207 + (int32_t)sub_8001FC90(-85314 + 72207, travel) / divisor;
        third = (int32_t)sub_8001FC90(-108678, travel) / divisor;
        body = rrj_read32(rrj_read32(actor + 596) + 856);
    }
    else
    {
        first = 1218;
        second = -52114 + (int32_t)sub_8001FC90(-65221 + 52114, travel) / divisor;
        third = (int32_t)sub_8001FC90(-67718, travel) / divisor;
        body = rrj_read32(actor + 596);
    }
    race_place_from_basis(rrj_host_context(), actor, body + 184, body + 432, first, second, third);
    race_copy_halfwords(rrj_host_context(), actor + 432, body + 432, 9);
    race_copy_halfwords(rrj_host_context(), actor + 450, actor + 444, 3);
    speed = rrj_s32(rrj_read32(body + 480)) - 327680;
    if (speed <= 0)
    {
        for (i = 0; i < 3; ++i)
            rrj_put16(rrj_at(actor + 450 + 2 * i, 2), (uint16_t)-r_s16(actor + 450 + 2 * i));
        speed = 327680 - rrj_s32(rrj_read32(body + 480));
    }
    rrj_write32(actor + 480, (uint32_t)speed);
    if (size_out)
    {
        rrj_write32(size_out, rrj_read32(actor + 312));
        speed = rrj_s32(rrj_read32(actor + 312)) / 2;
        rrj_write32(actor + 312, (uint32_t)speed);
    }
    return (uint32_t)speed;
}

static void race_copy_vector16(RRJMemory *m, uint32_t output, uint32_t input)
{
    race_copy_halfwords(m, output, input, 3);
}

static void race_negate_vector16(RRJMemory *m, uint32_t vector)
{
    uint32_t i;
    for (i = 0; i < 3; ++i)
        rrj_put16(rrj_at(vector + 2 * i, 2), (uint16_t)-r_s16(vector + 2 * i));
}

static void race_outer_product16(RRJMemory *m, uint32_t diagonal_address, uint32_t direction_address, uint32_t output)
{
    int16_t diagonal[3];
    int16_t direction[3];
    int16_t result[3];
    uint32_t i;
    for (i = 0; i < 3; ++i)
    {
        diagonal[i] = r_s16(diagonal_address + 2 * i);
        direction[i] = r_s16(direction_address + 2 * i);
    }
    leaf_direction_op_local(diagonal, direction, result);
    for (i = 0; i < 3; ++i)
        rrj_put16(rrj_at(output + 2 * i, 2), (uint16_t)result[i]);
}

static void race_select_launch_direction(RRJMemory *m, uint32_t actor, uint32_t body)
{
    uint32_t source = (rrj_read32(actor + 552) & 0x200000) ? actor + 456 : body + 450;
    race_copy_vector16(m, actor + 450, source);
}

static void race_tilt_launch_direction(RRJMemory *m, uint32_t actor)
{
    int32_t x = rrj_u16(rrj_at(actor + 450, 2));
    int32_t z = r_s16(actor + 454);
    x += (357 * z) >> 12;
    rrj_put16(rrj_at(actor + 450, 2), (uint16_t)x);
    z -= (357 * (int16_t)x) >> 12;
    rrj_put16(rrj_at(actor + 454, 2), (uint16_t)z);
}

static uint32_t race_player_record(RRJMemory *m, uint32_t body)
{
    uint32_t player = rrj_u16(rrj_at(body + 172, 2));
    uint32_t count = rrj_read32(rrj_read32(0x8005B2F8) + 48);
    return player < count ? 0x800CD898 + 1132 * player : 0;
}

static void race_mark_player_event(RRJMemory *m, uint32_t actor, uint32_t body, uint32_t require_flag)
{
    uint32_t record;
    uint32_t bits;
    if (require_flag && !(rrj_read32(body + 568) & require_flag))
        return;
    record = race_player_record(m, body);
    if (!record)
        return;
    bits = (rrj_read32(actor + 552) & 0x10000) ? 0x0E000000u : 0x06000000u;
    rrj_write32(record + 548, rrj_read32(record + 548) | bits | 0x80u);
}

static int32_t race_trig_scaled(RRJMemory *m, uint32_t angle, uint32_t cosine)
{
    uint32_t address = 0x8005624C + 4 * (angle & 0xFFF) + (cosine ? 2 : 0);
    return 4 * (int32_t)r_s16(address);
}

static void race_rotate_mount_pair(RRJMemory *m, uint32_t actor, int32_t angle, uint32_t second_plane)
{
    const uint32_t input_address = 0x1F800300;
    const uint32_t output_address = 0x1F800310;
    int16_t input[4];
    int16_t output[4];
    int32_t sine;
    int32_t cosine;
    uint32_t i;
    (void)sub_8005C338(rrj_read32(actor + 540), input_address);
    for (i = 0; i < 4; ++i)
        input[i] = r_s16(input_address + 2 * i);
    sine = race_trig_scaled(m, (uint32_t)(angle / 2), 0);
    cosine = race_trig_scaled(m, (uint32_t)(angle / 2), 1);
    if (!second_plane)
    {
        output[0] = (int16_t)(leaf_mul_asr(cosine, input[0], 14) + leaf_mul_asr(sine, input[2], 14));
        output[1] = (int16_t)(leaf_mul_asr(cosine, input[1], 14) + leaf_mul_asr(sine, input[3], 14));
        output[2] = (int16_t)(leaf_mul_asr(cosine, input[2], 14) - leaf_mul_asr(sine, input[0], 14));
        output[3] = (int16_t)(leaf_mul_asr(cosine, input[3], 14) - leaf_mul_asr(sine, input[1], 14));
    }
    else
    {
        output[0] = (int16_t)(leaf_mul_asr(cosine, input[0], 14) + leaf_mul_asr(sine, input[3], 14));
        output[1] = (int16_t)(leaf_mul_asr(cosine, input[1], 14) - leaf_mul_asr(sine, input[2], 14));
        output[2] = (int16_t)(leaf_mul_asr(cosine, input[2], 14) + leaf_mul_asr(sine, input[1], 14));
        output[3] = (int16_t)(leaf_mul_asr(cosine, input[3], 14) - leaf_mul_asr(sine, input[0], 14));
    }
    for (i = 0; i < 4; ++i)
        rrj_put16(rrj_at(output_address + 2 * i, 2), (uint16_t)output[i]);
    (void)sub_8005C36C(rrj_read32(actor + 540), output_address);
}

uint32_t sub_80091468(uint32_t actor)
{
    FUNCTION_MARKER(0x80091468u, "RASHCDG.BIN");
    uint32_t attached = (r_u8(actor + 572) >> 5) & 1;
    uint32_t body = attached ? rrj_read32(rrj_read32(actor + 596) + 856) : rrj_read32(actor + 596);
    uint32_t mirrored;
    uint32_t options = 1;
    uint32_t duration = 10;
    uint32_t event;
    int32_t first_rotation = 0;
    int32_t second_rotation = 0;
    uint32_t value;
    if (attached)
        rrj_write32(body + 724, 0);
    rrj_write32(actor + 488, 0);
    rrj_write32(actor + 580, rrj_read32(0x800D3964));
    mirrored = (rrj_read32(actor + 552) >> 27) & 1;
    event = rrj_u16(rrj_at(actor + 544, 2));
    switch (event)
    {
        case 38:
        {
            int32_t speed = rrj_s32(rrj_read32(actor + 480));
            int32_t scale;
            rrj_write32(actor + 552, rrj_read32(actor + 552) & ~0x40000000u);
            if (speed < 196608)
                speed = 196608;
            rrj_write32(actor + 480, (uint32_t)speed);
            event = speed > 1310720 ? 48 : 51;
            options = (rrj_read32(actor + 552) & 0x08000000) ? 387 : 131;
            (void)sub_8008EE60(actor);
            race_select_launch_direction(rrj_host_context(), actor, body);
            scale = event == 48 ? 55705 : 58982;
            value = (uint32_t)sub_8001FC90(scale, rrj_s32(rrj_read32(body + 480)));
            rrj_write32(actor + 480, value);
            (void)sub_8002EE50(value, rrj_at(actor + 450, 6), rrj_at(actor + 456, 12));
            race_copy_vector16(rrj_host_context(), actor + 438, body + 522);
            race_copy_vector16(rrj_host_context(), actor + 444, actor + 450);
            race_outer_product16(rrj_host_context(), actor + 438, actor + 444, actor + 432);
            if (!sub_8002E468(actor + 432))
                (void)sub_8008CF74(actor);
            rrj_write32(actor + 600, 196608);
            rrj_write32(actor + 508, 0);
            break;
        }
        case 39:
        case 89:
        {
            int32_t angle;
            options = (mirrored ? 0x100u : 0) | 1u;
            event = 48;
            duration = attached ? 25 : 40;
            rrj_write32(actor + 552, rrj_read32(actor + 552) | 0x40000000u);
            (void)sub_8008E818(actor, 0);
            race_copy_vector16(rrj_host_context(), actor + 432, body + 444);
            race_copy_vector16(rrj_host_context(), actor + 444, body + 432);
            race_copy_vector16(rrj_host_context(), actor + 438, body + 438);
            race_negate_vector16(rrj_host_context(), actor + (mirrored ? 432 : 444));
            first_rotation = 1024;
            if (rrj_read32(body + 568) & 0x140)
                value = 327680;
            else
                value = (uint32_t)sub_8001FC90(78643, rrj_s32(rrj_read32(body + 480)));
            rrj_write32(actor + 480, value);
            race_select_launch_direction(rrj_host_context(), actor, body);
            if (attached)
                race_tilt_launch_direction(rrj_host_context(), actor);
            angle = (int32_t)sub_8001FF3C((uint32_t)((int32_t)r_s16(actor + 452) << 4));
            (void)sub_8007E868(actor + 450, value, (uint32_t)r_s16(0x8005624E + 4 * ((1137 - angle) & 0xFFF)), 406454);
            (void)sub_8002EE50(rrj_read32(actor + 480), rrj_at(actor + 450, 6), rrj_at(actor + 456, 12));
            (void)sub_8001A760(rrj_u16(rrj_at(body + 172, 2)), 1);
            if (!attached)
            {
                uint32_t record = race_player_record(rrj_host_context(), body);
                if (record)
                    rrj_write32(record + 548, rrj_read32(record + 548) | 0x06000080u);
            }
            rrj_write32(actor + 580, 65);
            break;
        }
        case 40:
        case 88:
        {
            int32_t angle;
            int32_t avoid = -1;
            options = 131;
            rrj_write32(actor + 552, rrj_read32(actor + 552) | 0x40000000u);
            (void)sub_8008E50C(actor);
            race_select_launch_direction(rrj_host_context(), actor, body);
            if (attached)
                race_tilt_launch_direction(rrj_host_context(), actor);
            if (rrj_read32(actor + 552) & 0x10000)
            {
                int32_t speed;
                (void)sub_8007E868(actor + 450, 0, (uint32_t)-2868, 0);
                speed = rrj_s32(rrj_read32(actor + 480)) / 3;
                if (speed < 327680)
                    speed = 327680;
                rrj_write32(actor + 480, (uint32_t)speed);
            }
            else
            {
                angle = (int32_t)sub_8001FF3C((uint32_t)((int32_t)r_s16(actor + 452) << 4));
                (void)sub_8007E868(actor + 450, rrj_read32(actor + 480), (uint32_t)r_s16(0x8005624E + 4 * ((1137 - angle) & 0xFFF)), 406454);
                second_rotation = (int32_t)sub_8001FF3C(sub_8002E698(rrj_at(actor + 444, 6), rrj_at(actor + 450, 6))) - 1024;
            }
            race_copy_vector16(rrj_host_context(), actor + 438, body + 522);
            race_outer_product16(rrj_host_context(), actor + 438, actor + 450, actor + 432);
            if (!sub_8002E468(actor + 432))
                (void)sub_8008CF74(actor);
            race_outer_product16(rrj_host_context(), actor + 432, actor + 438, actor + 444);
            (void)sub_8002EE50(rrj_read32(actor + 480), rrj_at(actor + 450, 6), rrj_at(actor + 456, 12));
            value = rrj_read32(body + 852);
            if (r_u8(value + 572) & 0x10)
            {
                uint32_t candidate = attached ? rrj_u16(rrj_at(value + 544, 2)) : rrj_u16(rrj_at(rrj_read32(rrj_read32(body + 856) + 852) + 544, 2));
                if (rrj_u16(rrj_at(0x800541D4 + 8 * candidate + 2, 2)) == 6)
                    avoid = (int32_t)candidate;
            }
            do
            {
                uint32_t remainder = sub_8001FC58() % 3;
                event = remainder == 0 ? 49 : 46 + (remainder == 1);
            } while ((int32_t)event == avoid);
            if (event == 47)
                duration = 20;
            (void)sub_8001A760(rrj_u16(rrj_at(body + 172, 2)), 1);
            if (!attached)
                race_mark_player_event(rrj_host_context(), actor, body, 0);
            rrj_write32(actor + 580, 65);
            break;
        }
        case 41:
        case 90:
        {
            int32_t speed;
            options = 131;
            event = 43;
            rrj_write32(actor + 552, rrj_read32(actor + 552) | 0x40000000u);
            (void)sub_8008EB88(actor, 0);
            race_select_launch_direction(rrj_host_context(), actor, body);
            race_copy_vector16(rrj_host_context(), actor + 444, actor + 450);
            speed = rrj_s32(rrj_read32(body + 480));
            if (speed <= 327680)
            {
                race_negate_vector16(rrj_host_context(), actor + 450);
                speed = 327680 - speed;
            }
            else
                speed = (int32_t)sub_8001FC90(32768, speed);
            rrj_write32(actor + 480, (uint32_t)speed);
            (void)sub_8002EE50((uint32_t)speed, rrj_at(actor + 450, 6), rrj_at(actor + 456, 12));
            rrj_put16(rrj_at(actor + 432, 2), rrj_u16(rrj_at(actor + 448, 2)));
            rrj_put16(rrj_at(actor + 434, 2), 0);
            rrj_put16(rrj_at(actor + 436, 2), (uint16_t)-r_s16(actor + 444));
            if (!sub_8002E468(actor + 432))
                (void)sub_8008CF74(actor);
            race_outer_product16(rrj_host_context(), actor + 444, actor + 432, actor + 438);
            (void)sub_8001A760(rrj_u16(rrj_at(body + 172, 2)), 1);
            if (!attached)
                race_mark_player_event(rrj_host_context(), actor, body, 0x200);
            break;
        }
        case 42:
        case 91:
        {
            int32_t angle;
            options = 131;
            rrj_write32(actor + 552, rrj_read32(actor + 552) | 0x40000000u);
            (void)sub_8008E044(actor, 0);
            (void)sub_8002EE50(rrj_read32(actor + 480), rrj_at(actor + 450, 6), rrj_at(actor + 456, 12));
            race_copy_vector16(rrj_host_context(), actor + 438, body + 522);
            race_copy_vector16(rrj_host_context(), actor + 444, actor + 450);
            race_outer_product16(rrj_host_context(), actor + 438, actor + 444, actor + 432);
            (void)sub_8002E468(actor + 432);
            race_outer_product16(rrj_host_context(), actor + 444, actor + 432, actor + 438);
            angle = 1024 - (int32_t)sub_8001FF3C(sub_8002E698(rrj_at(actor + 450, 6), rrj_at(body + 444, 6)));
            first_rotation = angle;
            if (angle < 683)
            {
                duration = 20;
                event = 48;
                if (!mirrored)
                    options = 387;
            }
            else
                event = 46;
            (void)sub_8001A760(rrj_u16(rrj_at(body + 172, 2)), 1);
            if (!attached)
                race_mark_player_event(rrj_host_context(), actor, body, 0);
            break;
        }
        default:
            break;
    }
    (void)sub_8008DF74(actor);
    if (!attached)
    {
        uint32_t player = rrj_u16(rrj_at(body + 172, 2));
        if (player < rrj_read32(rrj_read32(0x8005B2F8) + 48))
            (void)sub_800235B0(player, actor);
    }
    if (first_rotation)
        race_rotate_mount_pair(rrj_host_context(), actor, first_rotation, 0);
    if (second_rotation)
        race_rotate_mount_pair(rrj_host_context(), actor, second_rotation, 1);
    (void)sub_800C4550(event & 0xFFFF, actor, options | 0x800u | (duration << 16));
    (void)sub_8009246C(actor);
    (void)sub_8008BD2C(actor);
    w_u8(actor + 535, 0);
    value = (rrj_read32(actor + 552) | 1u) & 0xFFD8FFFFu;
    rrj_write32(actor + 552, value);
    return value;
}

static void race_send_body_event(RRJMemory *m, uint32_t body, uint16_t type, uint32_t mode)
{
    const uint32_t packet = 0x1F800300;
    rrj_put16(rrj_at(packet, 2), type);
    rrj_put16(rrj_at(packet + 2, 2), 224);
    (void)sub_800BCA68(packet, mode, body);
}

uint32_t sub_80090D84(uint32_t actor, RRJRaceLeafCall call, RRJReverbCall reverb)
{
    FUNCTION_MARKER(0x80090D84u, "RASHCDG.BIN");
    uint32_t active = 1;
    uint32_t body;
    uint32_t attached;
    uint32_t flags;
    uint32_t value;
    if (rrj_read32(actor + 604) >= 2)
        goto finish;
    body = rrj_read32(actor + 596);
    attached = (r_u8(actor + 572) >> 5) & 1;
    w_u8(rrj_read32(body + 1084) + 15, 0);
    flags = rrj_read32(body + 568);
    if (!(flags & 0x7FF))
    {
        uint32_t actor_flags = rrj_read32(actor + 552);
        if (actor_flags & 0x60000)
            (void)sub_800C4550(((0u - attached) & 0x31) + 42, actor, (actor_flags & 0x40000) ? 0x100 : 0);
        else
            (void)sub_800C4550(((0u - attached) & 0x31) + 41, actor, 0);
    }
    else if ((flags & 0x18000) || ((flags & 0x400) && (rrj_read32(actor + 552) & 0x60000)))
        (void)sub_800C4550(42, actor, (rrj_read32(actor + 552) & 0x40000) ? 0x100 : 0);
    else if (flags & 0x20)
    {
        uint32_t variant = rrj_s32(rrj_read32(body + 676)) < 0 || ((flags & 0x40) && rrj_s32(rrj_read32(body + 652)) > 0);
        (void)sub_800C4550(((0u - attached) & 0x32) + 39, actor, variant << 8);
        active = 0;
    }
    else if (flags & 0x140)
    {
        (void)sub_800C4550(((0u - attached) & 0x33) + 38, actor, ((uint32_t)rrj_s32(rrj_read32(body + 676)) >> 31) << 8);
        active = 0;
    }
    else if ((flags & 0x680) && !(flags & 0x40000))
    {
        (void)sub_800C4550(((0u - attached) & 0x31) + 41, actor, 0);
        active = (flags & 0x480) != 0;
    }
    else
    {
        uint32_t packed = rrj_u16(rrj_at(actor + 172, 2));
        uint32_t global = rrj_read32(0x8005B2F8);
        (void)sub_800C4550(((0u - attached) & 0x30) + 40, actor, 0);
        if (!(flags & 0x20000))
            rrj_write32(actor + 480, (uint32_t)sub_8001FC90(rrj_s32(rrj_read32(actor + 480)), 49152));
        if (rrj_s32(rrj_read32(actor + 480)) < 327680)
            rrj_write32(actor + 480, 327680);
        race_copy_halfwords(rrj_host_context(), actor + 432, body + 432, 9);
        rrj_write32(body + 568, flags & 0xFFF9FFFFu);
        active = 0;
        if ((packed >> 5) == 1 && (packed & 0x1F) < rrj_read32(global + 48))
        {
            uint32_t player_body = attached ? rrj_read32(body + 856) : body;
            uint32_t record = 0x800CD898 + 1132 * rrj_u16(rrj_at(player_body + 172, 2));
            uint32_t choice;
            uint32_t linked;
            rrj_write32(record + 548, rrj_read32(record + 548) & 0xFFFD7FFFu);
            if (!rrj_read32(record + 772))
            {
                linked = rrj_read32(body + 828);
                if (linked && (rrj_u16(rrj_at(linked, 2)) >> 5) == 8 && (rrj_u16(rrj_at(linked + 2, 2)) & 0x200))
                    choice = 1;
                else
                {
                    choice = 0;
                    if (!(rrj_read32(actor + 552) & 0x10000) && leaf_abs32(rrj_s32(sub_8002E698(rrj_at(body + 820, 6), rrj_at(rrj_read32(body + 340) + 14, 6)))) > 56753)
                    {
                        uint32_t random = sub_8001FC58() % 100;
                        if (random >= 50)
                            choice = 2;
                    }
                }
                if (choice == 1)
                    rrj_write32(record + 548, rrj_read32(record + 548) | 0x8000);
                else if (choice == 2)
                {
                    rrj_write32(record + 792, 0);
                    rrj_write32(record + 548, rrj_read32(record + 548) | ((flags & 4) ? 0x80020000u : 0x20000u));
                }
            }
        }
    }
    if (!active)
        (void)sub_800BF51C(body);
    (void)sub_8002EAD8(rrj_at(body + 184, 12), rrj_at(body + 438, 6), (uint32_t)-65536, rrj_at(actor + 184, 12));
    rrj_write32(actor + 468, rrj_read32(actor + 184));
    rrj_write32(actor + 472, rrj_read32(actor + 188));
    rrj_write32(actor + 476, rrj_read32(actor + 192));
    (void)sub_800BCD10(body);
    value = rrj_read32(body + 1084);
    race_send_body_event(rrj_host_context(), body, (uint16_t)((0u - (uint32_t)(rrj_read32(value + 40) || r_u8(value + 39) >= 0xF8)) & 0xFFFE) + 4, 1);
    value = rrj_read32(body + 564);
    rrj_write32(body + 924, 0);
    rrj_write32(body + 916, 0);
    rrj_write32(body + 920, 0);
    rrj_write32(body + 564, value & 0xFFF7FFFFu);
    race_send_body_event(rrj_host_context(), body, 0, 0);
    if (!attached)
    {
        uint32_t player = rrj_u16(rrj_at(body + 172, 2));
        if (player < rrj_read32(rrj_read32(0x8005B2F8) + 48))
            (void)sub_80018440(player, 1);
    }
    if (rrj_read32(actor + 552) & 0x10000)
        (void)sub_80091468(actor);
    value = rrj_read32(0x8005B2F8);
    if ((r_u8(value + 4) & 1) && (r_u8(rrj_read32(body + 1084) + 1) & 0xF) != 2 && r_u8(value + 4) != 33)
    {
        uint32_t count = rrj_read32(value + 48);
        uint32_t i;
        for (i = 0; i < count; ++i)
        {
            uint32_t other = rrj_read32(0x8005B268 + 4 * i);
            uint32_t state = rrj_read32(other + 1084);
            if ((r_u8(state + 1) & 0xF) == 2 && rrj_read32(0x8005B1F8) >= r_u8(rrj_read32(body + 1084) + 39))
            {
                uint32_t x = leaf_abs32((int32_t)r_s16(other + 186) - r_s16(body + 186));
                uint32_t z = leaf_abs32((int32_t)r_s16(other + 194) - r_s16(body + 194));
                uint32_t major = x < z ? z : x;
                uint32_t minor = x < z ? x : z;
                uint32_t diagonal = minor + (minor >> 1);
                uint32_t distance = major - (major >> 5) - (major >> 7) + (diagonal >> 2) + (diagonal >> 6);
                uint32_t threshold_index = rrj_read32(value + 60);
                int32_t threshold = r_s16(0x8005309E + 4 * threshold_index);
                if (rrj_s32(rrj_read32(other + 480)) > 585944 && rrj_s32(distance) < threshold)
                    (void)sub_80096F30(other, body, 9, call);
            }
        }
    }
finish:
    value = rrj_read32(actor + 552) & 0xFFFF7FFFu;
    rrj_write32(actor + 552, value);
    return value;
}

static void race_resolve_launch_state(RRJMemory *m, uint32_t actor, RRJRaceLeafCall call, RRJReverbCall reverb)
{
    uint32_t crashed = (rrj_read32(actor + 552) & 0x8000) != 0;
    uint32_t type;
    if (crashed)
        (void)sub_80090D84(actor, call, reverb);
    type = rrj_u16(rrj_at(actor + 544, 2));
    if (type == 89 || type == 39 || type == 88 || type == 40 || type == 38)
    {
        if (crashed)
            rrj_write32(actor + 552, rrj_read32(actor + 552) | 0x10000u);
        (void)sub_80091468(actor);
    }
}

uint32_t sub_80072994(uint32_t actor, RRJRaceLeafCall call, RRJReverbCall reverb)
{
    FUNCTION_MARKER(0x80072994u, "RASHCDG.BIN");
    uint32_t linked = rrj_read32(actor + 852);
    uint32_t flags;
    uint32_t value;
    rrj_write32(actor + 932, 0);
    rrj_write32(actor + 564, rrj_read32(actor + 564) & 0xC0018200u);
    if (r_u8(linked + 572) & 0x10)
        race_resolve_launch_state(rrj_host_context(), rrj_read32(rrj_read32(actor + 856) + 852), call, reverb);
    race_resolve_launch_state(rrj_host_context(), linked, call, reverb);
    flags = rrj_read32(actor + 568) & 0xFFFC7F1Fu;
    rrj_write32(actor + 724, 0);
    rrj_write32(actor + 568, flags);
    if (flags & 0x10)
    {
        int32_t direction = rrj_s32(rrj_read32(actor + 676)) > 0 ? -196608 : 196608;
        int32_t lateral = rrj_s32(rrj_read32(actor + 636)) > 0 ? -65536 : 65536;
        rrj_write32(actor + 688, 0);
        rrj_write32(actor + 680, 0);
        rrj_write32(actor + 684, (uint32_t)direction);
        rrj_write32(actor + 644, 0);
        rrj_write32(actor + 688, rrj_read32(actor + 676));
        rrj_write32(actor + 640, (uint32_t)lateral);
        if (flags & 0x600)
            flags &= ~0x10u;
        rrj_write32(actor + 568, flags);
    }
    else if (flags & 0xF)
    {
        if ((flags & 2) && rrj_read32(linked + 604) >= 2)
        {
            flags = (flags & 0xFFFFFFF9u) | 4u;
            rrj_write32(actor + 568, flags);
        }
        if (flags & 0x600)
            rrj_write32(actor + 744, rrj_read32(actor + 488));
        else
        {
            (void)sub_8002EAD8(rrj_at(actor + 504, 12), rrj_at(actor + 528, 6), rrj_read32(actor + 308), rrj_at(actor + 784, 12));
            if (rrj_read32(actor + 552) && rrj_read32(actor + 488))
            {
                int32_t scale = (int32_t)sub_8001FC90(rrj_s32(rrj_read32(actor + 488)), rrj_s32(rrj_read32(actor + 552)));
                int32_t angle = -(int32_t)(((int64_t)652 * scale) >> 16);
                (void)sub_8002ED94(actor + 528, actor + 516, (uint32_t)angle);
            }
            if (!(rrj_read32(actor + 568) & 0xC))
                rrj_write32(actor + 724, rrj_read32(actor + 552));
        }
        rrj_write32(actor + 656, 0);
        rrj_write32(actor + 660, 0);
        rrj_write32(actor + 680, 0);
        rrj_write32(actor + 684, 0);
        rrj_write32(actor + 584, 0);
    }
    value = rrj_read32(actor + 568) & 0xFFFFEFFFu;
    rrj_write32(actor + 568, value);
    return value;
}

uint32_t sub_80071BCC(uint32_t actor, RRJRaceLeafCall call, RRJReverbCall reverb)
{
    FUNCTION_MARKER(0x80071BCCu, "RASHCDG.BIN");
    uint32_t list;
    uint32_t flags;
    if (!r_s16(actor + 320))
        list = 0x8005B270;
    else
    {
        flags = rrj_read32(actor + 568);
        if (flags & 0x600)
        {
            uint32_t state = flags & 0x1FF;
            if (flags & 0x800)
            {
                (void)sub_80071D24(actor);
                flags = rrj_read32(actor + 568);
                state = flags & 0x1FF;
            }
            if (state && (flags & 0x1000))
                (void)sub_80072994(actor, call, reverb);
            list = (rrj_read32(actor + 568) & 0x400) ? 0x8005B2D8 : 0x8005B378;
        }
        else if (flags & 0x1FF)
        {
            if (flags & 0x800)
                (void)sub_800723FC(actor);
            if (rrj_read32(actor + 568) & 0x1000)
                (void)sub_80072994(actor, call, reverb);
            list = 0x8005B350;
        }
        else
        {
            (void)sub_8007F08C(actor);
            list = 0x8005B298;
        }
    }
    rrj_write32(rrj_read32(actor + 1088) + 4, rrj_read32(actor + 1092));
    rrj_write32(rrj_read32(actor + 1092), rrj_read32(actor + 1088));
    rrj_write32(rrj_read32(list + 4), actor + 1088);
    rrj_write32(actor + 1092, rrj_read32(list + 4));
    rrj_write32(list + 4, actor + 1088);
    rrj_write32(actor + 1088, list);
    flags = rrj_read32(actor + 568) & 0xF7FFFFFFu;
    rrj_write32(actor + 568, flags);
    return flags;
}

static uint32_t race_steering_mul(uint32_t left, uint32_t right)
{
    int64_t product = (int64_t)rrj_s32(left) * rrj_s32(right);
    return (uint32_t)((uint64_t)product >> 16);
}

static uint32_t race_steering_abs(uint32_t value)
{
    uint32_t sign = (uint32_t)(rrj_s32(value) >> 31);
    return (value + sign) ^ sign;
}

static uint32_t race_steering_ratio(uint32_t numerator, uint32_t denominator)
{
    uint32_t n = rrj_s32(numerator) > 0 ? numerator : 0u - numerator;
    uint32_t d = rrj_s32(denominator) > 0 ? denominator : 0u - denominator;
    uint32_t value = sub_80010028(n, d);
    return (rrj_s32(numerator) > 0) != (rrj_s32(denominator) > 0) ? 0u - value : value;
}

static uint32_t race_steering_angle(uint32_t forward, uint32_t direction)
{
    uint32_t octant, x, y, ratio, address, next, base, step, value;
    if (!forward && !direction)
        return 0;
    octant = (uint32_t)(rrj_s32(direction) >> 31) & 2u;
    if (rrj_s32(forward) < 0)
        octant |= 4u;
    x = race_steering_abs(forward);
    y = race_steering_abs(direction);
    if (rrj_s32(y) < rrj_s32(x))
    {
        octant |= 1u;
        ratio = race_steering_ratio(y, x);
    }
    else
        ratio = race_steering_ratio(x, y);
    address = 0x8005285Cu + 4u * (uint32_t)(rrj_s32(ratio) >> 12);
    next = rrj_read32(address + 4);
    base = rrj_read32(address);
    step = ((next - base) << 4) + 8u;
    value = (uint32_t)(rrj_s32(race_steering_mul(ratio & 4095u, step) + base) >> 20);
    switch (octant)
    {
        case 0: return value;
        case 1: return 1024u - value;
        case 2: return 2048u - value;
        case 3: return value + 1024u;
        case 4: return 0u - value;
        case 5: return value - 1024u;
        case 6: return value - 2048u;
        case 7: return 0xFFFFFC00u - value;
        default: return 0;
    }
}

uint32_t sub_80072FB4(uint32_t actor)
{
    uint32_t flags, specification, mirrored, input_enabled, speed, actual_speed, linked;
    uint32_t previous, steering = 0, enabled = 1;

    FUNCTION_MARKER(0x80072FB4u, "RASHCDG.BIN");
    flags = rrj_read32(actor + 560);
    actual_speed = rrj_read32(actor + 480);
    speed = rrj_read32(actor + 576);
    specification = rrj_read32(actor + 556);
    mirrored = (flags >> 27) & 1u;
    input_enabled = (flags >> 20) & 1u;
    if (rrj_read32(actor + 568) & 0x600u)
        speed = actual_speed;
    linked = rrj_read32(actor + 856);
    previous = rrj_read32(linked ? linked + 488 : actor + 488);
    if (!mirrored)
    {
        if (input_enabled)
        {
            uint32_t low = rrj_read32(specification + 356);
            uint32_t player_scale = rrj_read32(0x800CE540u + 8u * r_u16(actor + 172) + 4);
            uint32_t scale;
            if (rrj_s32(speed) < rrj_s32(low))
                scale = rrj_read32(specification + 388);
            else if (rrj_s32(speed) > rrj_s32(rrj_read32(specification + 360)))
                scale = rrj_read32(specification + 392);
            else
            {
                uint32_t slope = rrj_read32(specification + 396);
                uint32_t base = rrj_read32(specification + 388);
                scale = race_steering_mul(speed - low, slope) + base;
            }
            steering = race_steering_mul(scale, player_scale);
        }
        else
            enabled = 0;
    }
    else
    {
        uint32_t delta[3], forward = 0, direction = 0, distance = 0;
        uint32_t angle, limit, slow;
        unsigned i;
        for (i = 0; i < 3; ++i)
        {
            uint32_t target = rrj_read32(actor + 880 + 4u * i);
            uint32_t position = rrj_read32(actor + 504 + 4u * i);
            delta[i] = target - position;
        }
        for (i = 0; i < 3; ++i)
            forward += race_steering_mul(delta[i], (uint32_t)r_s16(actor + 814 + 2u * i) << 4);
        for (i = 0; i < 3; ++i)
            distance += race_steering_mul(delta[i], delta[i]);
        flags = rrj_read32(actor + 560);
        distance = (uint32_t)(rrj_s32(distance) >> 1);
        if (!(flags & 0x40000u) && rrj_s32(distance) >= 17 && rrj_s32(distance) <= 536838143)
        {
            uint32_t raw = race_steering_mul(race_steering_ratio(forward, distance), speed);
            uint32_t lower_delta, upper_delta;
            limit = rrj_read32(specification + 232);
            lower_delta = raw + limit;
            upper_delta = limit - raw;
            steering = raw + ((uint32_t)(rrj_s32(lower_delta) >> 31) & (0u - limit - raw))
                           + ((uint32_t)(rrj_s32(upper_delta) >> 31) & upper_delta);
        }
        for (i = 0; i < 3; ++i)
            direction += race_steering_mul(delta[i], (uint32_t)r_s16(actor + 450 + 2u * i) << 4);
        limit = rrj_read32(specification + 224);
        slow = rrj_s32(rrj_read32(actor + 480)) < (rrj_s32(limit) >> 1);
        angle = race_steering_abs(race_steering_angle(forward, direction));
        if (rrj_s32(direction) <= 0 ||
            ((rrj_read32(actor + 560) & 0x400000u) && rrj_s32(angle) >= 171))
        {
            if (slow)
            {
                steering = rrj_read32(specification + 232);
                if (rrj_s32(forward) < 0)
                    steering = 0u - steering;
            }
            rrj_write32(actor + 560, rrj_read32(actor + 560) | 0x400000u);
        }
        else if (rrj_read32(actor + 560) & 0x400000u)
            rrj_write32(actor + 560, (rrj_read32(actor + 560) | 0x01000000u) & 0xFFBFFFFFu);
    }
    if (enabled)
    {
        uint32_t mask = rrj_s32(steering) < rrj_s32(previous) ? 0x100u :
                        rrj_s32(previous) < rrj_s32(steering) ? 0x200u : 0;
        uint32_t equal = mask == 0;
        uint32_t changed = 0;
        uint32_t clear_mask = 0x300u;
        if (mirrored && equal)
            mask = 0x100u;
        if (!equal)
            changed = (rrj_read32(actor + 560) & mask) == 0;
        flags = rrj_read32(actor + 560) | mask | (changed << 7);
        rrj_write32(actor + 560, flags);
        if (mask == 0x200u)
            clear_mask = 0x100u;
        if (mask == 0x100u)
            clear_mask -= 0x100u;
        rrj_write32(actor + 560, flags & ~clear_mask);
        flags = rrj_read32(actor + 560) | 0x200000u;
    }
    else
        flags = rrj_read32(actor + 560) & 0xFFDFFFFFu;
    rrj_write32(actor + 560, flags);
    flags = rrj_read32(actor + 560);
    rrj_write32(actor + 196, steering);
    {
        uint32_t target = 0x7FFFFFFFu;
        if (flags & 0x100u)
        {
            if (!(flags & 0x200u))
                target = rrj_read32(actor + 636);
        }
        else if (flags & 0x200u)
            target = 0u - rrj_read32(actor + 636);
        if (target == 0x7FFFFFFFu)
            flags = rrj_read32(actor + 560) | 0x8000u;
        else
        {
            uint32_t low, acceleration;
            if (rrj_read32(actor + 560) & 0x80u)
            {
                uint32_t physics = rrj_read32(actor + 564);
                physics = rrj_s32(target) > 0 ? physics | 2u : physics & 0xFFFFFFFDu;
                rrj_write32(actor + 564, physics);
                rrj_write32(actor + 560, rrj_read32(actor + 560) & 0xFFFFFF7Fu);
            }
            low = rrj_read32(specification + 356);
            if (rrj_s32(speed) < rrj_s32(low))
                acceleration = 0u - rrj_read32(specification + 376);
            else if (rrj_s32(speed) > rrj_s32(rrj_read32(specification + 360)))
                acceleration = 0u - rrj_read32(specification + 380);
            else
            {
                uint32_t slope = rrj_read32(specification + 384);
                uint32_t base = rrj_read32(specification + 376);
                acceleration = 0u - base - race_steering_mul(speed - low, slope);
            }
            rrj_write32(actor + 584, acceleration);
            if (rrj_read32(actor + 560) & 0x200u)
                rrj_write32(actor + 584, 0u - rrj_read32(actor + 584));
            if (rrj_read32(actor + 564) & 2u)
            {
                uint32_t offset = rrj_read32(specification + 292);
                if (rrj_s32(0u - offset) < rrj_s32(target))
                {
                    uint32_t slope = rrj_read32(specification + 288);
                    uint32_t current = rrj_read32(actor + 584);
                    uint32_t weight = race_steering_mul(target + offset, slope) + 65536u;
                    rrj_write32(actor + 584, race_steering_mul(weight, current));
                }
            }
            flags = rrj_read32(actor + 560) & 0xFFFF7FFFu;
        }
    }
    rrj_write32(actor + 560, flags);
    return flags;
}

uint32_t sub_80074170(uint32_t actor)
{
    FUNCTION_MARKER(0x80074170u, "RASHCDG.BIN");
    uint32_t specification = rrj_read32(actor + 556);
    int32_t speed = rrj_s32(rrj_read32(actor + 576));
    int32_t first = (int32_t)sub_8001FC90(rrj_s32(rrj_read32(specification + 280)), speed) + 65536;
    int32_t second = (int32_t)sub_8001FC90(rrj_s32(rrj_read32(specification + 284)), speed) + 65536;
    int32_t ratio = race_leaf_signed_ratio(first, second);
    int32_t projection = (int32_t)sub_8001FC90(ratio, rrj_s32(rrj_read32(actor + 488)));
    int32_t target;
    int32_t angle;
    int32_t result;
    if (speed >= 6554)
    {
        int32_t value = (int32_t)sub_8001FC90(projection, race_inverse_vector_length(speed));
        int32_t limit = rrj_s32(rrj_read32(specification + 240));
        value = (int32_t)sub_8001FC90(value, rrj_s32(rrj_read32(specification + 4)));
        if (value < -limit)
            value = -limit;
        if (value > limit)
            value = limit;
        rrj_put16(rrj_at(actor + 826, 2), (uint16_t)((163 * value) >> 14));
    }
    target = (int32_t)sub_8001FC90(rrj_s32(rrj_read32(actor + 488)) - projection, speed) - rrj_s32(rrj_read32(actor + 732));
    ratio = race_leaf_signed_ratio(target, rrj_s32(rrj_read32(actor + 736)));
    angle = rrj_s32(sub_8001FF3C((uint32_t)ratio));
    result = (int32_t)(((int64_t)25736 * angle) >> 8) - rrj_s32(rrj_read32(actor + 668));
    {
        int32_t limit = rrj_s32(rrj_read32(specification + 228));
        int32_t clamped = result;
        if (clamped < -limit)
            clamped = -limit;
        if (clamped > limit)
            clamped = limit;
        rrj_write32(actor + 636, (uint32_t)clamped);
        if (clamped != result)
            return sub_80073D74(actor);
    }
    return (uint32_t)result;
}

static int32_t race_clamp_symmetric(int32_t value, int32_t limit)
{
    if (value < -limit)
        return -limit;
    if (value > limit)
        return limit;
    return value;
}

static int32_t race_phase_cosine(RRJMemory *m, int32_t scale, int32_t phase)
{
    int32_t angle = (int32_t)sub_8001FC90(scale, phase);
    uint32_t index = (((uint32_t)(163 * angle) >> 13) & 0x1FFE) + 1;
    return (int32_t)r_s16(0x8005624C + 2 * index) << 4;
}

uint32_t sub_80074570(uint32_t actor, uint32_t use_linked, int32_t correction, int32_t delta)
{
    FUNCTION_MARKER(0x80074570u, "RASHCDG.BIN");
    uint32_t flags = rrj_read32(actor + 560);
    uint32_t specification = rrj_read32(actor + 556);
    uint32_t physics_active = (rrj_read32(actor + 568) & 0x600) != 0;
    int32_t speed = rrj_s32(rrj_read32(actor + (physics_active ? 480 : 576)));
    uint32_t mirrored = (flags >> 27) & 1;
    if ((flags & 0x108000) == 0x8000)
    {
        int32_t phase;
        int32_t next;
        uint32_t result;
        if (flags & 0x80)
        {
            rrj_write32(actor + 712, (flags & 0x10000) ? 0 : 0u - rrj_read32(specification + 416));
            rrj_write32(actor + 584, 0);
            flags &= ~0x80u;
            rrj_write32(actor + 560, flags);
            rrj_write32(actor + 564, rrj_read32(actor + 564) & ~2u);
        }
        phase = rrj_s32(rrj_read32(actor + 712));
        next = phase + delta;
        if (phase < 0)
        {
            if (next > 0)
                next = 0;
            rrj_write32(actor + 712, (uint32_t)next);
        }
        else if (rrj_read32(actor + 636))
        {
            if (!phase)
            {
                int32_t low = rrj_s32(rrj_read32(specification + 356));
                int32_t high = rrj_s32(rrj_read32(specification + 360));
                int32_t divisor;
                int32_t yaw = rrj_s32(rrj_read32(actor + 636));
                if (speed < low)
                    divisor = rrj_s32(rrj_read32(specification + 364));
                else if (speed <= high)
                    divisor = rrj_s32(rrj_read32(specification + 364)) + (int32_t)sub_8001FC90(speed - low, rrj_s32(rrj_read32(specification + 372)));
                else
                    divisor = rrj_s32(rrj_read32(specification + 368));
                rrj_write32(actor + 644, (uint32_t)yaw);
                phase = race_leaf_signed_ratio(yaw, divisor);
                rrj_write32(actor + 728, (uint32_t)phase);
                if (phase < 9830)
                {
                    flags |= 0x20000u;
                    rrj_write32(actor + 560, flags);
                }
            }
            next = rrj_s32(rrj_read32(actor + 712)) + delta;
            rrj_write32(actor + 712, (uint32_t)next);
            if (next >= rrj_s32(rrj_read32(actor + 728)))
                rrj_write32(actor + 636, 0);
            else
            {
                int32_t ratio = race_leaf_signed_ratio(next, rrj_s32(rrj_read32(actor + 728)));
                int32_t base;
                int32_t weight;
                flags = rrj_read32(actor + 560);
                if (flags & 0x20000)
                {
                    base = rrj_s32(rrj_read32(actor + 644));
                    weight = 65536 - ratio;
                }
                else if (flags & 0x10000)
                {
                    base = rrj_s32(rrj_read32(actor + 644));
                    weight = race_phase_cosine(rrj_host_context(), 102943, ratio);
                }
                else
                {
                    base = rrj_s32(rrj_read32(actor + 644)) >> 1;
                    weight = race_phase_cosine(rrj_host_context(), 205887, ratio) + 65536;
                }
                rrj_write32(actor + 636, (uint32_t)sub_8001FC90(base, weight));
            }
        }
        (void)sub_80073D74(actor);
        result = (uint32_t)r_s16(specification + 446);
        if (r_s16(specification + 446))
            rrj_write32(actor + 488, 0);
        if (use_linked)
        {
            result = rrj_read32(actor + 488);
            rrj_write32(rrj_read32(actor + 856) + 488, result);
        }
        if (correction)
        {
            int32_t value = rrj_s32(rrj_read32(actor + 488)) + correction;
            value = race_clamp_symmetric(value, rrj_s32(rrj_read32(specification + 232)));
            rrj_write32(actor + 488, (uint32_t)value);
            return sub_80074170(actor);
        }
        return result;
    }
    else
    {
        int32_t previous = rrj_s32(rrj_read32(use_linked ? rrj_read32(actor + 856) + 488 : actor + 488));
        int32_t limit;
        int32_t integrated;
        flags &= 0xFFFCFFFFu;
        rrj_write32(actor + 560, flags);
        if (rrj_read32(actor + 616) || physics_active)
        {
            int32_t scale = rrj_s32(rrj_read32(specification + 312));
            int32_t minimum;
            rrj_write32(actor + 584, (uint32_t)sub_8001FC90(rrj_s32(rrj_read32(actor + 584)), scale));
            minimum = (int32_t)sub_8001FC90(rrj_s32(rrj_read32(specification + 232)), scale);
            limit = (int32_t)leaf_abs32(previous);
            if (limit < minimum)
                limit = minimum;
        }
        else
        {
            limit = rrj_s32(rrj_read32(specification + 232));
            if (use_linked && previous > 0)
                rrj_write32(actor + 584, (uint32_t)sub_8001FC90(rrj_s32(rrj_read32(actor + 584)), rrj_s32(rrj_read32(specification + 308))));
        }
        if ((rrj_read32(actor + 564) & 6) == 4)
            rrj_write32(actor + 584, (uint32_t)sub_8001FC90(rrj_s32(rrj_read32(actor + 584)), rrj_s32(rrj_read32(specification + 316))));
        integrated = previous + (int32_t)sub_8001FC90(rrj_s32(rrj_read32(actor + 584)), delta);
        if (flags & 0x200000)
        {
            int32_t target = rrj_s32(rrj_read32(actor + 196));
            if ((previous < target && integrated >= target) || (previous > target && integrated <= target) || previous == target)
                integrated = target;
        }
        if (mirrored)
        {
            int32_t target = rrj_s32(rrj_read32(actor + 196));
            flags = rrj_read32(actor + 560);
            if (flags & 0x1000000)
            {
                integrated = target;
                flags &= ~0x1000000u;
                rrj_write32(actor + 560, flags);
            }
            if (flags & 0x400000)
                rrj_write32(actor + 924, 327680);
            else if (integrated != target && leaf_abs32(target) >= 26215 && !(r_u8(rrj_read32(actor + 1084)) & 1))
            {
                int32_t ratio = (int32_t)leaf_abs32(race_leaf_signed_ratio(target - previous, delta));
                int32_t minimum = rrj_s32(rrj_read32(specification + 356));
                int32_t lower = rrj_s32(rrj_read32(specification + 376));
                int32_t upper = rrj_s32(rrj_read32(specification + 380));
                if (ratio > lower)
                {
                    if (ratio > upper)
                        rrj_write32(actor + 924, (uint32_t)(minimum + race_leaf_signed_ratio(ratio - lower, rrj_s32(rrj_read32(specification + 384)))));
                }
                else
                    rrj_write32(actor + 924, (uint32_t)minimum);
                if (rrj_s32(rrj_read32(actor + 924)) < (rrj_s32(rrj_read32(specification + 224)) >> 1))
                    rrj_write32(actor + 924, (uint32_t)(rrj_s32(rrj_read32(specification + 224)) >> 1));
            }
        }
        if (use_linked)
            rrj_write32(rrj_read32(actor + 856) + 488, (uint32_t)integrated);
        integrated = race_clamp_symmetric(integrated + correction, limit);
        rrj_write32(actor + 488, (uint32_t)integrated);
        return sub_80074170(actor);
    }
}

void sub_80074C84(uint32_t list, int32_t delta)
{
    FUNCTION_MARKER(0x80074C84u, "RASHCDG.BIN");
    uint32_t link = rrj_read32(list + 4);
    while (link != list)
    {
        uint32_t actor = link - 1088;
        uint32_t specification = rrj_read32(actor + 852);
        int32_t correction = 0;
        if (r_u8(specification + 572) & 0x10)
            correction = (int32_t)sub_80072C7C(actor, (uint32_t)delta);
        if (rrj_read32(actor + 560) & 0x80000)
        {
            uint32_t use_linked = 0;
            if (rrj_read32(actor + 856))
                use_linked = rrj_read32(actor + 1088) != 0;
            (void)sub_80074570(actor, use_linked, correction, delta);
        }
        link = rrj_read32(link + 4);
    }
}

static int32_t race_half_toward_zero(int32_t value)
{
    return (value + (int32_t)((uint32_t)value >> 31)) >> 1;
}

void sub_80073874(uint32_t list, int32_t delta)
{
    FUNCTION_MARKER(0x80073874u, "RASHCDG.BIN");
    uint32_t link = rrj_read32(list + 4);
    while (link != list)
    {
        uint32_t actor = link - 1088;
        uint32_t flags = rrj_read32(actor + 560);
        uint32_t mirrored = (flags >> 27) & 1;
        uint32_t physics_active = (rrj_read32(actor + 568) & 0x600) != 0;
        uint32_t race = rrj_read32(0x8005B2F8);
        uint32_t player = r_u16(actor + 172);
        flags &= ~0x100000u;
        rrj_write32(actor + 560, flags);
        if (player < rrj_read32(race + 48) && !mirrored)
        {
            uint32_t record = 0x800D7128 + 192 * player;
            if (rrj_read32(record + 16))
            {
                flags |= 0x100000u;
                rrj_write32(actor + 560, flags);
            }
            if ((r_u8(rrj_read32(actor + 852) + 572) & 0x10) && rrj_read32(race + 52) >= 2)
            {
                uint32_t index = 3 * (player + (rrj_read32(race + 48) < 2 ? 1 : 2));
                uint32_t linked = rrj_read32(actor + 856);
                uint32_t linked_flags = rrj_read32(linked + 560);
                if (rrj_read32(0x800D7128 + 64 * index + 16))
                    linked_flags |= 0x100000u;
                else
                    linked_flags &= ~0x100000u;
                rrj_write32(linked + 560, linked_flags);
            }
        }
        flags = rrj_read32(actor + 560) & ~0x80000u;
        rrj_write32(actor + 560, flags);
        if ((physics_active || (rrj_read32(actor + 564) & 0x40)) && (rrj_read32(actor + 568) & 0x1FF))
            goto next_actor;
        if (rrj_read32(actor + 720))
        {
            (void)sub_80073D74(actor);
            goto next_actor;
        }
        if (mirrored && ((flags >> 13) & 1))
        {
            uint32_t specification = rrj_read32(actor + 556);
            if (flags & 0x4000)
            {
                int32_t side = rrj_s32(sub_8002E698(rrj_at(actor + 528, 6), rrj_at(actor + 820, 6)));
                int32_t forward = rrj_s32(sub_8002E698(rrj_at(actor + 516, 6), rrj_at(actor + 820, 6)));
                int32_t angle = (int32_t)(((int64_t)25736 * rrj_s32(sub_80020018((uint32_t)forward, (uint32_t)-side))) >> 8);
                int32_t difference = (angle > 0 ? 102943 : -102943) - angle;
                int32_t magnitude = (int32_t)leaf_abs32(difference);
                int32_t divisor = rrj_s32(rrj_read32(specification + 232));
                rrj_write32(actor + 724, (uint32_t)race_leaf_signed_ratio(magnitude, divisor));
                rrj_write32(actor + 488, difference < 0 ? 0u - (uint32_t)divisor : (uint32_t)divisor);
                flags = rrj_read32(actor + 560) & ~0x4000u;
                rrj_write32(actor + 560, flags);
            }
            {
                int32_t remaining = rrj_s32(rrj_read32(actor + 724));
                int32_t steering = 0;
                flags = rrj_read32(actor + 560);
                if (remaining <= 0)
                    flags &= ~0x2000u;
                else
                    steering = rrj_s32(rrj_read32(actor + 488));
                rrj_write32(actor + 560, flags);
                rrj_write32(actor + 488, (uint32_t)steering);
                if (rrj_read32(actor + 856))
                    rrj_write32(rrj_read32(actor + 856) + 488, (uint32_t)steering);
                rrj_write32(actor + 724, (uint32_t)(remaining - delta));
                (void)sub_80074170(actor);
            }
            goto next_actor;
        }
        {
            uint32_t specification = rrj_read32(actor + 852);
            int32_t speed = rrj_s32(rrj_read32(actor + (physics_active ? 480 : 576)));
            if (speed > 0 || (rrj_read32(specification + 604) < 2 && (flags & 0x42) == 2))
            {
                if ((flags & 0x2000) && !(flags & 0x300))
                {
                    if (rrj_s32(sub_8002E698(rrj_at(actor + 820, 6), rrj_at(actor + 516, 6))) <= 0)
                        flags |= 0x100;
                    else
                        flags |= 0x200;
                    flags &= ~0x2000u;
                    rrj_write32(actor + 560, flags);
                }
                (void)sub_80072FB4(actor);
                rrj_write32(actor + 560, rrj_read32(actor + 560) | 0x80000u);
            }
            else if (!(rrj_read32(actor + 564) & 0x18000000) || !(flags & 0x300))
            {
                uint32_t linked = rrj_read32(actor + 856);
                rrj_write32(actor + 488, (uint32_t)race_half_toward_zero(rrj_s32(rrj_read32(actor + 488))));
                rrj_write32(actor + 636, (uint32_t)race_half_toward_zero(rrj_s32(rrj_read32(actor + 636))));
                rrj_write32(actor + 672, (uint32_t)race_half_toward_zero(rrj_s32(rrj_read32(actor + 672))));
                rrj_write32(actor + 652, rrj_read32(actor + 668));
                rrj_put16(rrj_at(actor + 826, 2), (uint16_t)race_half_toward_zero((int16_t)r_u16(actor + 826)));
                rrj_write32(actor + 676, (uint32_t)race_half_toward_zero(rrj_s32(rrj_read32(actor + 676))));
                rrj_write32(actor + 616, (uint32_t)race_half_toward_zero(rrj_s32(rrj_read32(actor + 616))));
                if (linked)
                    rrj_write32(linked + 488, (uint32_t)race_half_toward_zero(rrj_s32(rrj_read32(linked + 488))));
            }
        }
    next_actor:
        link = rrj_read32(link + 4);
    }
}

static int32_t race_mul(int32_t first, int32_t second)
{
    return (int32_t)(((int64_t)first * second) >> 16);
}

static int32_t race_clamp(int32_t value, int32_t low, int32_t high)
{
    if (value < low)
        return low;
    if (value > high)
        return high;
    return value;
}

static void race_root_prepare_pool(RRJMemory *m, int32_t delta, RRJRaceLeafCall call, RRJReverbCall reverb)
{
    int32_t count = rrj_s32(rrj_read32(rrj_read32(0x800CE4DC)));
    uint32_t actor = rrj_read32(0x800CE4D0);
    uint32_t stride = rrj_read32(0x800CE4D4);
    while (count-- >= 0)
    {
        uint32_t flags = rrj_read32(actor + 560);
        if (flags & 0x8000000)
        {
            uint32_t state = rrj_read32(actor + 564);
            rrj_write32(actor + 564, state & ~0x8000000u);
            if (state & 0x200)
                rrj_write32(actor + 924, 2u * rrj_read32(rrj_read32(actor + 556) + 224));
        }
        if (r_u16(actor + 944))
        {
            int16_t timer = r_s16(actor + 944);
            int32_t step = (int32_t)sub_8001FC90((uint32_t)delta, rrj_read32(actor + 904));
            int32_t phase = rrj_s32(rrj_read32(actor + 908)) + (timer < 0 ? -step : step);
            int32_t weight = race_clamp(phase, 0, 65536);
            uint8_t sequence = r_u8(actor + 946);
            uint16_t kind = r_u16(actor + 956 + 8 * ((uint8_t)(sequence - 1)));
            rrj_write32(actor + 908, (uint32_t)weight);
            rrj_write32(actor + 880, rrj_read32(actor + 880) + (uint32_t)race_mul(rrj_s32(rrj_read32(actor + 892)), weight));
            rrj_write32(actor + 884, rrj_read32(actor + 884) + (uint32_t)race_mul(rrj_s32(rrj_read32(actor + 896)), weight));
            rrj_write32(actor + 888, rrj_read32(actor + 888) + (uint32_t)race_mul(rrj_s32(rrj_read32(actor + 900)), weight));
            timer = (int16_t)(timer + (timer < 0 ? ((delta << 8) + 0x8000) >> 16 : -(((delta << 8) + 0x8000) >> 16)));
            if ((timer < 0 && timer + (((delta << 8) + 0x8000) >> 16) > 0) || (timer >= 0 && timer <= 0))
                timer = 0;
            if (!timer)
            {
                uint32_t terminal = kind == 3 || (kind == 4 && r_s16(actor + 944) == 1) || (kind >= 5 && kind < 18);
                if (terminal)
                    timer = 1;
                else
                {
                    uint32_t magnitude = sub_8002E548(actor + 892);
                    if (magnitude >= 16)
                    {
                        rrj_write32(actor + 904, sub_80010028(163840, magnitude));
                        timer = (int16_t)-((int32_t)((sub_80010028(magnitude, 163840) << 8) + 0x8000) >> 16);
                    }
                    else
                    {
                        rrj_write32(actor + 908, 0);
                        timer = 0;
                    }
                }
            }
            rrj_put16(rrj_at(actor + 944, 2), (uint16_t)timer);
        }
        if (rrj_read32(actor + 568) & 0x8001800)
            (void)sub_80071BCC(actor, call, reverb);
        actor += stride;
    }
}

static void race_root_prepare_riders(RRJMemory *m)
{
    uint32_t list = 0x8005B298;
    uint32_t link = rrj_read32(list + 4);
    while (link != list)
    {
        uint32_t actor = link - 1088;
        uint32_t specification = rrj_read32(actor + 556);
        int32_t lateral = rrj_s32(rrj_read32(actor + 676));
        int32_t absolute = (int32_t)leaf_abs32(lateral);
        uint32_t linked = rrj_read32(actor + 856) != 0;
        uint32_t flags = rrj_read32(actor + 560);
        uint32_t stale_direction = 0;
        if (rrj_read32(actor + 564) & 4)
            stale_direction = (uint32_t)rrj_s32(rrj_read32(actor + 716)) >> 31;
        {
            uint32_t threshold = rrj_read32(specification + 204);
            uint32_t fast = linked && lateral > 0 ? absolute >= (int32_t)(threshold / 2) : absolute >= (int32_t)threshold;
            uint32_t wide = (flags & 0x8000000) && rrj_s32(rrj_read32(specification + 208)) < absolute;
            flags |= wide << 4;
            rrj_write32(actor + 560, flags);
            if (!(rrj_read32(actor + 564) & 0x400) && (flags & 0x300) && (fast || stale_direction) && !(flags & 0x20000000))
            {
                uint32_t state = rrj_read32(actor + 568);
                uint32_t extra = 0;
                if (rrj_s32(rrj_read32(actor + 364)) >= 0)
                    state &= ~0x400000u;
                else
                    state |= 0x400000u;
                rrj_write32(actor + 716, 0);
                if (linked && lateral > 0)
                    state |= 0x820;
                else
                {
                    if ((rrj_read32(actor + 564) & 0x400) || (stale_direction && !rrj_read32(actor + 596)) || (fast && ((lateral < 0 && (flags & 0x100)) || (lateral > 0 && (flags & 0x200)))))
                        extra = 32;
                    state |= 0x820 | extra;
                }
                rrj_write32(actor + 568, state);
                if (rrj_s32(rrj_read32(actor + 576)) < 65536)
                    rrj_write32(actor + 576, 65536);
            }
        }
        if (!rrj_read32(actor + 616) && !rrj_read32(actor + 620) && !rrj_read32(actor + 624))
        {
            uint32_t state = rrj_read32(actor + 568);
            if (!(state & 0x7FF) && rrj_read32(rrj_read32(actor + 852) + 604) >= 2)
                rrj_write32(actor + 568, state | 0x880);
        }
        {
            int32_t grip = rrj_s32(rrj_read32(0x800D38E0 + 4 * (int8_t)r_u8(actor + 534)));
            int32_t force = -race_mul(grip, rrj_s32(rrj_read32(actor + 580)));
            int32_t longitudinal = 0;
            int32_t transverse = 0;
            rrj_write32(actor + 752, 0);
            rrj_write32(actor + 756, 0);
            if (!(rrj_read32(actor + 564) & 0x400))
            {
                if (!(flags & 0x8000000) && rrj_s32(rrj_read32(actor + 576)) <= 117188)
                    force = 0;
                longitudinal = race_mul(force, rrj_s32(rrj_read32(specification + 320)));
                transverse = race_mul(force, rrj_s32(rrj_read32(specification + 324)));
                if (lateral)
                {
                    int32_t scale = race_mul(rrj_s32(rrj_read32(specification + 328)), absolute) + 65536;
                    longitudinal = race_mul(longitudinal, scale);
                    transverse = race_mul(transverse, scale);
                }
                rrj_write32(actor + 752, (uint32_t)longitudinal);
                rrj_write32(actor + 756, (uint32_t)transverse);
            }
            rrj_write32(actor + 764, rrj_read32(actor + 760) + (uint32_t)longitudinal);
            rrj_write32(actor + 748, (uint32_t)race_mul(rrj_s32(rrj_read32(specification + 12)), rrj_s32(rrj_read32(actor + 736))));
            {
                int32_t divisor = (rrj_s32(rrj_read32(specification + 16)) >> 1) + (rrj_s32(rrj_read32(specification + 16)) - 2 < 0 ? -1 : 0);
                int32_t inverse = (int32_t)(0x80000000u / (uint32_t)divisor);
                int32_t value = race_mul(rrj_s32(rrj_read32(actor + 748)), inverse) - 6553;
                if ((rrj_read32(actor + 564) & 0x80) && rrj_s32(rrj_read32(actor + 576)) < rrj_s32(rrj_read32(specification + 304)))
                    value += 0x3332;
                if (value < 0)
                    value = 0;
                rrj_write32(actor + 600, (uint32_t)value);
            }
        }
        link = rrj_read32(link + 4);
    }
}

static void race_root_controls(RRJMemory *m, int32_t delta, RRJRaceLeafCall call, RRJReverbCall reverb)
{
    uint32_t list = 0x8005B298;
    uint32_t link = rrj_read32(list + 4);
    while (link != list)
    {
        uint32_t actor = link - 1088;
        int32_t cooldown = rrj_s32(rrj_read32(actor + 720));
        if (cooldown)
        {
            if (cooldown > 0)
            {
                cooldown -= delta;
                if (cooldown < 0)
                    cooldown = 0;
                rrj_write32(actor + 720, (uint32_t)cooldown);
            }
            rrj_write32(actor + 596, 0);
            rrj_write32(actor + 588, 0);
            link = rrj_read32(link + 4);
            continue;
        }
        {
            uint32_t flags = rrj_read32(actor + 560);
            uint32_t mirrored = (flags >> 27) & 1;
            uint32_t assisted = (flags & 0x100000) != 0;
            uint32_t specification = rrj_read32(actor + 556);
            int32_t command = 0;
            int32_t throttle = 0;
            int32_t brake = 0;
            if (!mirrored)
            {
                if (assisted)
                {
                    int32_t scale = rrj_s32(rrj_read32(0x800CE540 + 8 * r_u16(actor + 172)));
                    int32_t source = scale < 0 ? rrj_s32(rrj_read32(actor + 592)) : rrj_s32(rrj_read32(actor + 600));
                    command = -race_mul(source, scale);
                }
            }
            else if (rrj_read32(actor + 564) & 0x80000)
                command = -rrj_s32(rrj_read32(actor + 600));
            else
            {
                int32_t timer = rrj_s32(rrj_read32(actor + 916));
                int32_t bound = 0x7FFF0000;
                if (timer > 0)
                {
                    int32_t difference = rrj_s32(rrj_read32(actor + 920)) - rrj_s32(rrj_read32(actor + 580));
                    int32_t divisor = 2 * (rrj_s32(rrj_read32(actor + 600)) - rrj_s32(rrj_read32(actor + 760)));
                    int32_t quotient = race_leaf_signed_ratio(difference, divisor);
                    bound = timer + quotient;
                }
                if (bound > 0x20000)
                {
                    int32_t difference = rrj_s32(rrj_read32(actor + 924)) - rrj_s32(rrj_read32(actor + 576));
                    int32_t desired = race_leaf_signed_ratio(difference, delta);
                    int32_t relative = desired - rrj_s32(rrj_read32(actor + 764));
                    if (relative >= 0)
                    {
                        command = relative;
                        if (command > rrj_s32(rrj_read32(specification + 188)))
                            command = rrj_s32(rrj_read32(specification + 188));
                    }
                    else
                    {
                        command = relative + rrj_s32(rrj_read32(actor + 752));
                        if (command < -rrj_s32(rrj_read32(actor + 600)))
                            command = -rrj_s32(rrj_read32(actor + 600));
                    }
                }
                else if (bound <= 0)
                {
                    command = -rrj_s32(rrj_read32(actor + 600));
                    rrj_write32(actor + 564, rrj_read32(actor + 564) | 0x80000u);
                }
            }
            flags = rrj_read32(actor + 560);
            if (mirrored)
            {
                if (command < 0)
                {
                    brake = -command;
                    rrj_write32(actor + 588, 0);
                    if (brake < rrj_s32(rrj_read32(actor + 596)))
                        flags &= ~0x40u;
                    else
                    {
                        if (!(flags & 0x40))
                            flags |= 0x20;
                        flags |= 0x40;
                    }
                }
                else if (command > 0)
                {
                    throttle = command;
                    rrj_write32(actor + 596, 0);
                    flags &= ~0x40u;
                    if (flags & 0x10)
                    {
                        int32_t limit = (int32_t)sub_8007EF60(specification, rrj_s32(rrj_read32(actor + 736)), rrj_s32(rrj_read32(actor + 652)), rrj_s32(rrj_read32(0x800D38E0 + 4 * ((int8_t)r_u8(actor + 534) + 16))));
                        if (throttle > limit)
                            throttle = limit;
                        flags &= ~0x10u;
                    }
                    if (throttle >= rrj_s32(rrj_read32(actor + 588)))
                    {
                        if (!(flags & 2))
                            flags |= 1;
                        flags |= 2;
                    }
                    else
                        flags &= ~2u;
                }
                else
                    flags &= ~0x42u;
            }
            else if (assisted)
            {
                if (command < 0)
                {
                    brake = -command;
                    rrj_write32(actor + 588, 0);
                    flags = (flags & ~0x42u) | 0x40;
                }
                else if (command > 0)
                {
                    throttle = command;
                    rrj_write32(actor + 596, 0);
                    flags = (flags & ~0x42u) | 2;
                }
                else
                    flags &= ~0x42u;
            }
            rrj_write32(actor + 560, flags);
            if (!mirrored)
            {
                if (rrj_read32(actor + 856) && rrj_read32(link))
                    (void)sub_80074D58(actor, call, reverb);
                (void)sub_80074E6C(actor, (uint32_t)delta, call);
            }
            flags = rrj_read32(actor + 560);
            if (assisted)
            {
                rrj_write32(actor + 588, (uint32_t)throttle);
                rrj_write32(actor + 596, (uint32_t)brake);
            }
            else
            {
                int32_t current = rrj_s32(rrj_read32(actor + 588));
                int32_t rate = race_mul(rrj_s32(rrj_read32(specification + ((flags & 2) ? 340 : 344))), rrj_s32(rrj_read32(actor + 592)));
                int32_t next = current + (flags & 2 ? race_mul(rate, delta) : -race_mul(rate, delta));
                if (mirrored && ((flags & 2) ? current <= throttle && next > throttle : next < throttle && current >= throttle))
                    next = throttle;
                if (next < 0)
                    next = 0;
                rrj_write32(actor + 588, (uint32_t)next);
                current = rrj_s32(rrj_read32(actor + 596));
                rate = race_mul(rrj_s32(rrj_read32(specification + ((flags & 0x40) ? 348 : 352))), rrj_s32(rrj_read32(actor + 600)));
                next = current + (flags & 0x40 ? race_mul(rate, delta) : -race_mul(rate, delta));
                if (mirrored && ((flags & 0x40) ? current <= brake && next > brake : next < brake && current >= brake))
                    next = brake;
                if (next < 0)
                    next = 0;
                rrj_write32(actor + 596, (uint32_t)next);
            }
            flags = rrj_read32(actor + 560);
            rrj_write32(actor + 696, rrj_read32(actor + 480) ? (0u - ((flags >> 6) & 1u)) & 0xFFFF0000u : 0);
            rrj_write32(actor + 700, rrj_read32(actor + 480) ? (0u - ((flags >> 1) & 1u)) & 0xFFFF0000u : 0);
            if (!(flags & 2) && (!mirrored || !rrj_read32(actor + 588)))
            {
                int32_t difference = rrj_s32(rrj_read32(actor + 756)) - rrj_s32(rrj_read32(actor + 752));
                rrj_write32(actor + 752, rrj_read32(actor + 756));
                rrj_write32(actor + 764, rrj_read32(actor + 764) + (uint32_t)difference);
            }
            if (-rrj_s32(rrj_read32(actor + 752)) >= rrj_s32(rrj_read32(actor + 596)))
                rrj_write32(actor + 596, 0);
            else
            {
                rrj_write32(actor + 752, 0);
                rrj_write32(actor + 764, rrj_read32(actor + 760));
            }
        }
        link = rrj_read32(link + 4);
    }
}

static int32_t race_signed_inverse(int32_t value)
{
    int32_t absolute = (int32_t)leaf_abs32(value);
    int32_t divisor = (absolute >> 1) + (absolute - 2 < 0 ? -1 : 0);
    int32_t inverse = (int32_t)(0x80000000u / (uint32_t)divisor);
    return value < 0 ? -inverse : inverse;
}

static void race_root_physics(RRJMemory *m, int32_t delta)
{
    uint32_t list = 0x8005B298;
    uint32_t link = rrj_read32(list + 4);
    while (link != list)
    {
        uint32_t actor = link - 1088;
        uint32_t specification = rrj_read32(actor + 556);
        uint32_t secondary = rrj_read32(actor + 852);
        uint32_t flags = rrj_read32(actor + 560);
        int32_t throttle = rrj_s32(rrj_read32(actor + 588));
        int32_t brake = rrj_s32(rrj_read32(actor + 596));
        int32_t maximum = rrj_s32(rrj_read32(actor + 592));
        int32_t braking_maximum = rrj_s32(rrj_read32(actor + 600));
        int32_t grip = rrj_s32(rrj_read32(0x800D38E0 + 4 * ((int8_t)r_u8(actor + 534) + 16)));
        int32_t projection = race_mul(grip, rrj_s32(rrj_read32(actor + 736)));
        int32_t lateral = rrj_s32(rrj_read32(actor + 652));
        int32_t slip;
        int32_t traction;
        int32_t drive;
        int32_t brake_force = brake;
        int32_t target_roll = 0;
        uint32_t transition = 0;
        if (throttle > maximum)
            throttle = maximum;
        if (brake > braking_maximum)
            brake = braking_maximum;
        rrj_write32(actor + 588, (uint32_t)throttle);
        rrj_write32(actor + 596, (uint32_t)brake);
        if (rrj_read32(actor + 696) || rrj_read32(actor + 700))
        {
            int32_t squared = race_mul(projection, projection);
            int32_t inverse = race_signed_inverse(squared);
            int32_t first = race_mul(race_mul(lateral, rrj_s32(rrj_read32(specification + 192))), race_mul(lateral, rrj_s32(rrj_read32(specification + 192))));
            if (rrj_read32(actor + 700))
            {
                int32_t second = race_mul(race_mul(throttle, rrj_s32(rrj_read32(specification + 200))), race_mul(throttle, rrj_s32(rrj_read32(specification + 200))));
                rrj_write32(actor + 700, (uint32_t)race_mul(first + second, inverse));
            }
            else
            {
                int32_t second = race_mul(race_mul(brake, rrj_s32(rrj_read32(specification + 196))), race_mul(brake, rrj_s32(rrj_read32(specification + 196))));
                rrj_write32(actor + 696, (uint32_t)race_mul(first + second, inverse));
            }
        }
        slip = race_leaf_signed_ratio(projection, rrj_s32(rrj_read32(specification + 192))) - (int32_t)leaf_abs32(lateral);
        {
            int32_t suspension = rrj_s32(rrj_read32(actor + 616));
            if (suspension >= 11439)
                traction = 65536;
            else if (suspension < -11438)
                traction = -65536;
            else if (rrj_read32(actor + 576))
            {
                uint32_t index = (((uint32_t)(163 * rrj_s32(rrj_read32(actor + 636))) >> 13) & 0x1FFE) + 1;
                int32_t cosine = (int32_t)r_s16(0x8005624C + 2 * index) << 4;
                int32_t raw = race_mul(throttle - brake + rrj_s32(rrj_read32(actor + 752)), race_mul(cosine, rrj_s32(rrj_read32(specification + 16))));
                if (raw < 0)
                    traction = race_leaf_signed_ratio(raw, rrj_s32(rrj_read32(actor + 748)));
                else if (raw > 0)
                {
                    traction = race_leaf_signed_ratio(raw, race_mul(rrj_s32(rrj_read32(specification + 8)), rrj_s32(rrj_read32(actor + 736))));
                    if (traction <= 65535)
                        rrj_write32(actor + 564, rrj_read32(actor + 564) & ~0x100u);
                }
                else
                    traction = 0;
            }
            else
                traction = 0;
        }
        drive = ((rrj_s32(rrj_read32(actor + 616)) >= 0 || (rrj_read32(actor + 564) & 0x200000)) ? throttle : 0);
        if ((int32_t)leaf_abs32(rrj_s32(rrj_read32(actor + 636))) < 22876 && slip <= 0)
        {
            int32_t heading = (int32_t)(((int64_t)25736 * rrj_s32(sub_80020018(0u - rrj_read32(actor + 732), rrj_read32(actor + 740)))) >> 8);
            int32_t current = rrj_s32(rrj_read32(actor + 676));
            int32_t difference = heading - current;
            int32_t adjusted;
            if (difference > 205887)
                heading -= 411774;
            else if (difference < -205887)
                heading += 411774;
            adjusted = heading + (heading < current ? 91505 : -91505);
            if (rrj_s32(rrj_read32(actor + 576)) > 327680)
            {
                int32_t speed = rrj_s32(rrj_read32(actor + 576)) - 327680;
                adjusted = race_leaf_signed_ratio(race_mul(adjusted, speed) + heading, speed + 1);
            }
            rrj_write32(actor + 708, leaf_abs32(heading - adjusted));
            rrj_write32(actor + 688, (uint32_t)adjusted);
            rrj_write32(actor + 684, 0);
            if ((heading >= current && current < adjusted) || (heading < current && adjusted < current))
            {
                uint32_t packed = ((uint32_t)(uint16_t)(-12153 - r_s16(actor + 736))) | (((643207u - rrj_read32(actor + 736)) >> 16) << 16);
                rrj_write32(actor + 680, heading >= current ? packed : 0u - packed);
            }
            else
                rrj_write32(actor + 680, 0);
            if (rrj_read32(actor + 576) && (int32_t)leaf_abs32(heading) > 91505)
                rrj_write32(actor + 744, (uint32_t)(int16_t)((643207 - rrj_s32(rrj_read32(actor + 736))) / 2));
            else
                rrj_write32(actor + 744, 0);
            if ((int32_t)leaf_abs32(rrj_s32(rrj_read32(actor + 488))) >= rrj_s32(rrj_read32(actor + 744)))
                rrj_write32(actor + 744, 0);
            else if (rrj_s32(rrj_read32(actor + 688)) >= 0)
                rrj_write32(actor + 744, 0u - rrj_read32(actor + 744));
            rrj_write32(actor + 700, 65536);
            rrj_write32(actor + 696, 65536);
            rrj_write32(actor + 564, (rrj_read32(actor + 564) & 0xFFFFFBE3u) | 0x400);
        }
        else if (rrj_read32(actor + 564) & 0x400)
        {
            rrj_write32(actor + 744, 0);
            if (!rrj_read32(actor + 680) || !rrj_read32(actor + 576) || (int32_t)leaf_abs32(rrj_s32(rrj_read32(actor + 676))) < rrj_s32(rrj_read32(actor + 708)))
            {
                rrj_write32(actor + 680, 0);
                rrj_write32(actor + 676, 0);
                rrj_write32(actor + 700, 0);
                rrj_write32(actor + 696, 0);
                race_copy_halfwords(m, actor + 450, actor + 528, 3);
                race_copy_halfwords(m, actor + 814, actor + 516, 3);
                rrj_write32(actor + 764, (uint32_t)-race_mul(655360, slip));
            }
        }
        else
        {
            rrj_write32(actor + 744, 0);
            if (throttle > 0 && !brake && rrj_s32(rrj_read32(actor + 700)) > 65536)
            {
                uint32_t state = rrj_read32(actor + 564);
                rrj_write32(actor + 564, (state & 8) ? (state & ~4u) : ((state & ~0x14u) | 0x18));
                drive = race_leaf_signed_ratio(throttle, rrj_s32(rrj_read32(actor + 700)));
            }
            else
            {
                rrj_write32(actor + 564, rrj_read32(actor + 564) & ~0xCu);
                if (rrj_s32(rrj_read32(actor + 696)) > 65536 && !(flags & 0x8000000))
                    brake_force = race_leaf_signed_ratio(brake, rrj_s32(rrj_read32(actor + 696)));
            }
        }
        {
            int32_t suspension = rrj_s32(rrj_read32(actor + 616));
            uint32_t mode = (flags >> 11) & 1;
            if (suspension >= 0 && (traction > 65535 || mode || (rrj_read32(actor + 564) & 0x20)))
            {
                int32_t pitch = r_s16(actor + 530);
                target_roll = rrj_s32(rrj_read32(specification + (mode ? 268 : 264)));
                if (pitch < 3548)
                {
                    if (pitch >= -2047)
                        target_roll = race_mul(target_roll, race_mul(47976, 56754 - 16 * pitch));
                }
                else
                    target_roll = 0;
                if (target_roll)
                {
                    transition = 1;
                    rrj_write32(actor + 564, (rrj_read32(actor + 564) | 0x120u) & ~0x800u);
                    if (!mode)
                        drive = race_leaf_signed_ratio(drive, traction);
                    brake_force = 0;
                }
                if (!suspension && target_roll > 0)
                    (void)sub_800C4550(22, secondary, 8);
                rrj_write32(actor + 560, rrj_read32(actor + 560) & 0xFFFFEFFEu);
            }
            else if (traction <= -65536 && !suspension && !(rrj_read32(actor + 564) & 0x40) && (int32_t)leaf_abs32(lateral) < 17157 && rrj_s32(rrj_read32(actor + 576)) < rrj_s32(rrj_read32(specification + 304)))
            {
                int32_t pitch = r_s16(actor + 530);
                if (pitch < 3548)
                    target_roll = pitch >= -2047 ? -race_mul(3431 - rrj_s32(rrj_read32(specification + 272)), race_mul(47976, 56754 - 16 * pitch)) - rrj_s32(rrj_read32(specification + 272)) : 0;
                else
                    target_roll = -rrj_s32(rrj_read32(specification + 272));
                if (target_roll)
                {
                    transition = 1;
                    drive = 0;
                    rrj_write32(actor + 564, rrj_read32(actor + 564) | 0x40u);
                }
            }
        }
        rrj_write32(actor + 608, (uint32_t)race_clamp(traction, -65536, 65536));
        {
            int32_t acceleration = 0;
            int32_t old_speed = rrj_s32(rrj_read32(actor + 576));
            uint32_t state = rrj_read32(actor + 564);
            if (old_speed > 0 || drive > 0 || (state & 0x400))
                acceleration = drive - brake_force + rrj_s32(rrj_read32(actor + 764));
            if ((flags & 0x42) == 0x42 && acceleration > -458752)
                acceleration = -458752;
            rrj_write32(actor + 484, (uint32_t)acceleration);
            if (flags & 0x1000000)
            {
                rrj_write32(actor + 576, rrj_read32(actor + 924));
                rrj_write32(actor + 560, flags & ~0x1000000u);
            }
            else
            {
                int32_t speed = old_speed + race_mul(acceleration, delta);
                if (speed < 0)
                    speed = 0;
                if ((!speed || (state & 0x1000)) && (flags & 0x300) && !(flags & 0x40))
                    speed = old_speed;
                else if ((state & 0x1000) && !(state & 0x400))
                {
                    rrj_write32(actor + 484, 0);
                    w_u8(actor + 849, 0);
                    speed = 0;
                }
                rrj_write32(actor + 576, (uint32_t)speed);
            }
            rrj_write32(actor + 564, rrj_read32(actor + 564) & 0xFFFFCFFFu);
            if (!(state & 0x400) && !(state & 0x1000) && old_speed > 0 && !rrj_read32(actor + 576))
                rrj_write32(actor + 564, rrj_read32(actor + 564) | 0x2000u);
        }
        if (!(rrj_read32(actor + 564) & 0x200000) && transition)
        {
            int32_t difference = rrj_s32(rrj_read32(actor + 616)) - target_roll;
            if (difference >= -655 && difference < 656)
            {
                rrj_write32(actor + 616, (uint32_t)target_roll);
                rrj_write32(actor + 624, 0);
                rrj_write32(actor + 620, 0);
            }
            else
            {
                rrj_write32(actor + 624, difference >= 0 ? sub_80010028(0x20000, (uint32_t)difference) : 0u - sub_80010028(0x20000, (uint32_t)-difference));
                rrj_write32(actor + 632, 0);
                rrj_write32(actor + 620, difference >= 0 ? 0xFFFE0000u : 0x20000u);
                rrj_write32(actor + 628, rrj_read32(actor + 616));
                if (!rrj_read32(actor + 616))
                    rrj_write32(actor + 616, difference >= 0 ? 0xFFFFFFF0u : 16u);
            }
        }
        link = rrj_read32(link + 4);
    }
}

static void race_root_dynamics(RRJMemory *m, int32_t delta)
{
    uint32_t list = 0x8005B298;
    uint32_t link = rrj_read32(list + 4);
    while (link != list)
    {
        uint32_t actor = link - 1088;
        uint32_t specification = rrj_read32(actor + 556);
        uint32_t state = rrj_read32(actor + 564);
        if (rrj_s32(rrj_read32(actor + 576)) <= 0 || (state & 0x400))
            rrj_write32(actor + 564, state & ~0x80000u);
        else
        {
            int32_t velocity = rrj_s32(rrj_read32(actor + 676));
            int32_t displacement = rrj_s32(rrj_read32(actor + 684));
            uint32_t active = 0;
            if (state & 8)
            {
                if ((velocity >= 0 && rrj_s32(rrj_read32(specification + 244)) < rrj_s32(rrj_read32(actor + 636))) || (velocity <= 0 && rrj_s32(rrj_read32(actor + 636)) < -rrj_s32(rrj_read32(specification + 244))))
                    active = 1;
            }
            if (active)
            {
                int32_t amplitude = rrj_read32(actor + 856) && rrj_s32(rrj_read32(actor + 652)) > 0 ? rrj_s32(rrj_read32(specification + 212)) / 2 : rrj_s32(rrj_read32(specification + 212));
                int32_t threshold = rrj_read32(actor + 856) && rrj_s32(rrj_read32(actor + 652)) > 0 ? rrj_s32(rrj_read32(specification + 204)) / 2 : rrj_s32(rrj_read32(specification + 204));
                if (state & 0x10)
                {
                    rrj_write32(actor + 684, (uint32_t)race_mul(amplitude, race_mul(rrj_s32(rrj_read32(actor + 576)), rrj_s32(rrj_read32(actor + 652)))));
                    rrj_write32(actor + 680, 0);
                    if ((velocity ^ rrj_s32(rrj_read32(actor + 684))) < 0)
                        velocity = 0;
                }
                if (!(-threshold / 2 < velocity || rrj_s32(rrj_read32(actor + 684)) >= 0) || !(velocity < threshold / 2 || rrj_s32(rrj_read32(actor + 684)) <= 0))
                {
                    int32_t acceleration = rrj_s32(rrj_read32(actor + 684));
                    if ((velocity < -threshold / 2 && acceleration < 0) || (velocity >= threshold / 2 && acceleration > 0))
                        acceleration = -acceleration;
                    rrj_write32(actor + 684, (uint32_t)acceleration);
                    displacement += race_mul(acceleration, delta);
                    rrj_write32(actor + 680, (uint32_t)displacement);
                    if ((velocity > 0 && displacement < 0) || (velocity < 0 && displacement > 0))
                    {
                        displacement = velocity > 0 ? threshold : -threshold;
                        rrj_write32(actor + 680, 0);
                        rrj_write32(actor + 676, (uint32_t)displacement);
                    }
                    rrj_write32(actor + 676, rrj_read32(actor + 676) + (uint32_t)race_mul(displacement, delta));
                }
                else
                    active = 0;
            }
            if (!active)
            {
                int32_t next = velocity - race_mul(5 * velocity, delta);
                rrj_write32(actor + 684, 0);
                rrj_write32(actor + 680, 0);
                rrj_write32(actor + 676, leaf_abs32(next) >= 655 ? (uint32_t)next : 0);
                if (!(rrj_read32(actor + 560) & 0x8000000) && (state & 0xC) == 4)
                {
                    if (!(state & 0x10) && (rrj_read32(actor + 560) & 0x40) && (rrj_read32(actor + 560) & 0x300) && !(state & 2))
                        rrj_write32(actor + 716, rrj_read32(actor + 716) - (uint32_t)delta);
                    else
                    {
                        int32_t speed = rrj_s32(rrj_read32(actor + 576));
                        int32_t low = rrj_s32(rrj_read32(specification + 356));
                        int32_t high = rrj_s32(rrj_read32(specification + 360));
                        int32_t value = speed < low ? rrj_s32(rrj_read32(specification + 400)) : speed <= high ? rrj_s32(rrj_read32(specification + 400)) + race_mul(speed - low, rrj_s32(rrj_read32(specification + 408))) : rrj_s32(rrj_read32(specification + 404));
                        if (rrj_read32(actor + 388) & 1)
                            value = race_mul(rrj_s32(rrj_read32(specification + 412)), value);
                        rrj_write32(actor + 716, (uint32_t)value);
                    }
                }
            }
            {
                int32_t absolute = (int32_t)leaf_abs32(rrj_s32(rrj_read32(actor + 676)));
                int32_t threshold = rrj_s32(rrj_read32(specification + 220));
                int32_t scale = absolute >= threshold ? rrj_s32(rrj_read32(specification + 216)) : race_mul(rrj_s32(rrj_read32(specification + 216)), race_leaf_signed_ratio(absolute, threshold));
                rrj_write32(actor + 744, (uint32_t)race_mul(rrj_s32(rrj_read32(actor + 488)), scale));
            }
        }
        state = rrj_read32(actor + 564) & ~0x10u;
        rrj_write32(actor + 564, state);
        if (state & 0x1000000)
        {
            int32_t duration = rrj_s32(rrj_read32(actor + 720));
            if (duration >= 65)
            {
                int32_t acceleration = rrj_s32(rrj_read32(actor + 656));
                int32_t velocity = -race_leaf_signed_ratio(acceleration, duration);
                rrj_write32(actor + 660, (uint32_t)velocity);
                rrj_write32(actor + 724, 0);
                rrj_write32(actor + 664, rrj_read32(actor + 636));
                rrj_write32(actor + 644, rrj_read32(actor + 636) + (uint32_t)(race_mul(acceleration, duration) >> 1));
            }
            else
            {
                rrj_write32(actor + 656, 0);
                rrj_write32(actor + 660, 0);
                rrj_write32(actor + 720, 0);
            }
            rrj_write32(actor + 564, state & ~0x1000000u);
        }
        if (rrj_read32(actor + 660))
        {
            int32_t acceleration = rrj_s32(rrj_read32(actor + 656));
            int32_t velocity = rrj_s32(rrj_read32(actor + 660));
            int32_t angle = rrj_s32(rrj_read32(actor + 636));
            if ((acceleration ^ velocity) < 0 && (int32_t)leaf_abs32(angle) < rrj_s32(rrj_read32(specification + 228)))
            {
                int32_t elapsed = rrj_s32(rrj_read32(actor + 724)) + delta;
                acceleration += race_mul(velocity, delta);
                rrj_write32(actor + 656, (uint32_t)acceleration);
                rrj_write32(actor + 724, (uint32_t)elapsed);
                rrj_write32(actor + 636, rrj_read32(actor + 664) + (uint32_t)race_mul(elapsed, acceleration - race_mul(velocity, elapsed) / 2));
            }
            else
            {
                rrj_write32(actor + 720, 0);
                rrj_write32(actor + 660, 0);
                rrj_write32(actor + 656, 0);
                rrj_write32(actor + 636, rrj_read32(actor + 644));
            }
            rrj_write32(actor + 636, (uint32_t)race_clamp(rrj_s32(rrj_read32(actor + 636)), -rrj_s32(rrj_read32(specification + 228)), rrj_s32(rrj_read32(specification + 228))));
        }
        link = rrj_read32(link + 4);
    }
}

static void race_root_external(RRJMemory *m, int32_t delta, RRJRaceLeafCall call)
{
    int32_t count;
    uint32_t actor;
    uint32_t stride;
    uint32_t args[8] = {0};
    rrj_write32(0x1F800000, 0x1F800004);
    count = rrj_s32(rrj_read32(rrj_read32(0x800CE4DC)));
    actor = rrj_read32(0x800CE4D0);
    stride = rrj_read32(0x800CE4D4);
    while (count-- >= 0)
    {
        if (r_u16(actor + 320))
        {
            uint32_t linked = rrj_read32(actor + 856);
            rrj_write32(actor + 468, rrj_read32(actor + 184));
            rrj_write32(actor + 472, rrj_read32(actor + 188));
            rrj_write32(actor + 476, rrj_read32(actor + 192));
            if (linked)
            {
                rrj_write32(linked + 468, rrj_read32(linked + 184));
                rrj_write32(linked + 472, rrj_read32(linked + 188));
                rrj_write32(linked + 476, rrj_read32(linked + 192));
            }
            args[0] = actor;
            args[1] = (uint32_t)delta;
            (void)leaf_call8(m, call, 0x8007F0BC, args);
        }
        actor += stride;
    }
    (void)leaf_call(m, call, 0x80037338, 0, 0, 0, 0);
    (void)leaf_call(m, call, 0x8003E150, 0, 0, 0, 0);
    (void)leaf_call(m, call, 0x8003AE24, 0, 0, 0, 0);
    (void)leaf_call(m, call, 0x8003B520, 0, 0, 0, 0);
    {
        uint32_t cursor = 0x1F800004;
        while (cursor < rrj_read32(0x1F800000))
        {
            actor = rrj_read32(cursor);
            args[0] = actor;
            args[1] = (rrj_read32(actor + 568) & 0x600) ? actor + 184 : actor + 504;
            (void)leaf_call8(m, call, 0x8007504C, args);
            cursor += 4;
        }
        cursor = 0x1F800004;
        while (cursor < rrj_read32(0x1F800000))
        {
            actor = rrj_read32(cursor);
            args[0] = actor;
            args[1] = (uint32_t)delta;
            (void)leaf_call8(m, call, 0x8007FA4C, args);
            cursor += 4;
        }
    }
    count = rrj_s32(rrj_read32(rrj_read32(0x800CE4DC)));
    actor = rrj_read32(0x800CE4D0);
    while (count-- >= 0)
    {
        if (r_u16(actor + 320))
        {
            args[0] = actor;
            args[1] = (uint32_t)delta;
            (void)leaf_call8(m, call, 0x800807F0, args);
        }
        actor += stride;
    }
    (void)leaf_call(m, call, 0x80093E6C, actor, 0, 0, 0);
    (void)leaf_call(m, call, 0x800950E8, 0, 0, 0, 0);
}

uint32_t sub_80075EE0(int32_t delta, RRJRaceLeafCall call, RRJReverbCall reverb)
{
    FUNCTION_MARKER(0x80075EE0u, "RASHCDG.BIN");
    uint32_t count;
    uint32_t index;
    race_root_prepare_pool(rrj_host_context(), delta, call, reverb);
    sub_80073874(0x8005B298, delta);
    sub_80073874(0x8005B2D8, delta);
    sub_80074C84(0x8005B298, delta);
    sub_80074C84(0x8005B2D8, delta);
    race_root_prepare_riders(rrj_host_context());
    race_root_controls(rrj_host_context(), delta, call, reverb);
    race_root_physics(rrj_host_context(), delta);
    race_root_dynamics(rrj_host_context(), delta);
    race_root_external(rrj_host_context(), delta, call);
    count = rrj_read32(rrj_read32(0x8005B2F8) + 48);
    for (index = 0; index < count; ++index)
    {
        uint32_t actor = rrj_read32(0x8005B268 + 4 * index);
        uint32_t record = 0x800CD898 + 1132 * index;
        uint32_t specification = rrj_read32(actor + 852);
        if (rrj_read32(specification + 604) < 3)
            rrj_write32(0x8005B380 + 4 * index, rrj_read32(0x8005B380 + 4 * index) + ((uint32_t)sub_8001FC90(rrj_read32(actor + 480), (uint32_t)delta) >> 8));
        if ((rrj_read32(specification + 552) & 0x80000) && rrj_read32(record + 540) != 6)
        {
            if (rrj_s32(rrj_read32(record + 788)) <= 196608 && (rrj_read32(specification + 604) >= 3 || rrj_s32(rrj_read32(actor + 44 + 4 * index)) < 4801))
                rrj_write32(record + 788, rrj_read32(record + 788) + (uint32_t)delta);
            else
                (void)sub_80092C7C(actor, 10);
        }
    }
    return index < count;
}

uint32_t sub_8008F138(uint32_t actor, int32_t delta)
{
    FUNCTION_MARKER(0x8008F138u, "RASHCDG.BIN");
    uint32_t size = 0;
    uint32_t result = (uint32_t)(int32_t)r_s16(actor + 320);
    if (!result)
        return result;
    rrj_write32(actor + 468, rrj_read32(actor + 184));
    rrj_write32(actor + 472, rrj_read32(actor + 188));
    rrj_write32(actor + 476, rrj_read32(actor + 192));
    if (rrj_read32(actor + 604) == 2)
    {
        uint16_t kind = r_u16(actor + 544);
        if (kind == 38)
            (void)sub_8008EE60(actor);
        else if (kind == 39 || kind == 89)
            (void)sub_8008E818(actor, 0x1F8003F0);
        else if (kind == 40 || kind == 88)
            (void)sub_8008E50C(actor);
        else if (kind == 42 || kind == 91)
            (void)sub_8008E044(actor, 0x1F8003F0);
        else
            (void)sub_8008EB88(actor, 0x1F8003F0);
        size = rrj_read32(0x1F8003F0);
        {
            uint32_t source = rrj_read32(actor + 596);
            rrj_write32(actor + 504, rrj_read32(source + 504));
            rrj_write32(actor + 508, rrj_read32(source + 508));
            rrj_write32(actor + 512, rrj_read32(source + 512));
        }
        rrj_write32(actor + 244, 0);
        rrj_write32(actor + 248, 0);
        rrj_write32(actor + 252, 0);
        rrj_write32(actor + 552, rrj_read32(actor + 552) | 0xC0000000u);
        w_u8(actor + 547, 1);
        (void)(uint32_t)sub_8003FA18(9, actor + 432, actor + 256);
        (void)sub_8008DF74(actor);
    }
    else
    {
        uint32_t flags = rrj_read32(actor + 552);
        if (flags & 0x40000000)
            (void)sub_8002E570(actor + 184, actor + 456, (uint32_t)delta, actor + 184);
        else if (!(flags & 0x18))
        {
            uint32_t step = (uint32_t)sub_8001FC90((uint32_t)delta, rrj_read32(actor + 480));
            int32_t vertical;
            (void)sub_8002EAD8(rrj_at(actor + 184, 12), rrj_at(actor + 450, 6), step, rrj_at(actor + 184, 12));
            vertical = rrj_s32(rrj_read32(actor + 508));
            if (vertical > 0)
            {
                vertical -= 6553;
                if (vertical < 0)
                    vertical = 0;
                rrj_write32(actor + 508, (uint32_t)vertical);
                (void)sub_8002EAD8(rrj_at(actor + 184, 12), rrj_at(actor + 522, 6), vertical ? 6553 : 0, rrj_at(actor + 184, 12));
            }
        }
        {
            uint32_t source = rrj_read32(actor + 596);
            int8_t sequence = r_s8(source + 946);
            uint32_t active = sub_800374D4(actor, 1) == 1 || (flags & 0x40000000) || (r_u16(source + 954 + 8 * sequence) & 0x8000);
            if (active)
            {
                (void)sub_8009246C(actor);
                if (!(rrj_read32(actor + 552) & 0x40000000))
                    rrj_write32(actor + 552, rrj_read32(actor + 552) | 1);
            }
        }
    }
    (void)sub_8008BD2C(actor);
    if (size)
        rrj_write32(actor + 312, size);
    result = sub_80020018((uint32_t)((int32_t)r_s16(actor + 450) << 4), (uint32_t)((int32_t)r_s16(actor + 454) << 4));
    rrj_write32(actor + 292, result);
    rrj_write32(actor + 296, (uint32_t)((int32_t)r_s16(0x8005624C + 4 * (result & 0xFFF) + 2) << 4));
    result = (uint32_t)((int32_t)r_s16(0x8005624C + 4 * (result & 0xFFF)) << 4);
    rrj_write32(actor + 300, result);
    return result;
}

uint32_t sub_8008F068(int32_t delta)
{
    FUNCTION_MARKER(0x8008F068u, "RASHCDG.BIN");
    int32_t count = rrj_s32(rrj_read32(rrj_read32(0x800CE4DC)));
    uint32_t actor = rrj_read32(0x800CE4D0);
    uint32_t stride = rrj_read32(0x800CE4D4);
    while (count-- >= 0)
    {
        uint32_t specification = rrj_read32(actor + 852);
        if (rrj_read32(specification + 604) >= 2)
            (void)sub_8008F138(specification, delta);
        if (r_u8(specification + 572) & 0x10)
        {
            uint32_t linked = rrj_read32(actor + 856);
            specification = rrj_read32(linked + 852);
            if (rrj_read32(specification + 604) >= 2)
                (void)sub_8008F138(specification, delta);
        }
        actor += stride;
    }
    return stride;
}

uint32_t sub_800A2898(int32_t delta, RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x800A2898u, "RASHCDG.BIN");
    int32_t count = rrj_s32(rrj_read32(rrj_read32(0x800CE51C)));
    uint32_t actor = rrj_read32(0x800CE510);
    uint32_t stride = rrj_read32(0x800CE514);
    while (count-- >= 0)
    {
        if (r_u16(actor + 172))
        {
            if (r_u16(actor + 320) && rrj_s32(rrj_read32(actor + 480)) > 0)
            {
                uint32_t alternate = (rrj_read32(actor + 592) >> 1) & 1;
                uint32_t position = actor + 504 + ((0u - alternate) & 0x4C);
                uint32_t collision;
                rrj_write32(actor + 468, rrj_read32(actor + 184));
                rrj_write32(actor + 472, rrj_read32(actor + 188));
                rrj_write32(actor + 476, rrj_read32(actor + 192));
                (void)sub_8002E570(position, actor + 456, (uint32_t)delta, position);
                if (alternate)
                    (void)sub_8002EAD8(rrj_at(actor + 580, 12), rrj_at(actor + 438, 6), (uint32_t)(rrj_s32(rrj_read32(actor + 312)) >> 1), rrj_at(actor + 184, 12));
                else
                {
                    rrj_write32(actor + 184, rrj_read32(actor + 504));
                    rrj_write32(actor + 188, rrj_read32(actor + 508));
                    rrj_write32(actor + 192, rrj_read32(actor + 512));
                }
                collision = sub_80037450(actor);
                if (collision || alternate)
                {
                    const uint32_t args[8] = {actor, 0, 0, 0, 0, 0, 0, 0};
                    (void)leaf_call8(rrj_host_context(), call, 0x800A3A84, args);
                }
                if (!alternate)
                {
                    if (collision)
                    {
                        rrj_write32(actor + 184, rrj_read32(actor + 504));
                        rrj_write32(actor + 188, rrj_read32(actor + 508));
                        rrj_write32(actor + 192, rrj_read32(actor + 512));
                        rrj_write32(actor + 592, rrj_read32(actor + 592) | 0x80);
                    }
                    if (rrj_read32(actor + 592) & 4)
                        (void)sub_8002EAD8(rrj_at(actor + 504, 12), rrj_at(actor + 522, 6), 0u - rrj_read32(actor + 308), rrj_at(actor + 184, 12));
                }
                (void)sub_8008BA18(actor);
            }
            {
                uint32_t active = sub_80039F68(actor + 172);
                rrj_put16(rrj_at(actor + 320, 2), (uint16_t)active);
                if (!(active << 16))
                    (void)sub_8008C000(actor + 172, 4);
            }
        }
        actor += stride;
    }
    return stride;
}

uint32_t sub_800CB304(int32_t delta)
{
    FUNCTION_MARKER(0x800CB304u, "RASHCDG.BIN");
    int32_t count = rrj_s32(rrj_read32(rrj_read32(0x800CE4FC)));
    uint32_t actor = rrj_read32(0x800CE4F0);
    uint32_t stride = rrj_read32(0x800CE4F4);
    while (count-- >= 0)
    {
        if (r_u16(actor + 172))
        {
            if (r_u16(actor + 320))
            {
                uint32_t flags = rrj_read32(actor + 552);
                rrj_write32(actor + 468, rrj_read32(actor + 184));
                rrj_write32(actor + 472, rrj_read32(actor + 188));
                rrj_write32(actor + 476, rrj_read32(actor + 192));
                if (flags & 0x20000000)
                {
                    if (flags & 0x40000000)
                    {
                        (void)sub_8002E570(actor + 184, actor + 456, (uint32_t)delta, actor + 184);
                        (void)sub_80037450(actor);
                        (void)sub_8009246C(actor);
                    }
                    else
                    {
                        uint32_t step = (uint32_t)sub_8001FC90((uint32_t)delta, rrj_read32(actor + 480));
                        int32_t vertical;
                        (void)sub_8002EAD8(rrj_at(actor + 184, 12), rrj_at(actor + 450, 6), step, rrj_at(actor + 184, 12));
                        vertical = rrj_s32(rrj_read32(actor + 508));
                        if (vertical > 0)
                        {
                            vertical -= 6553;
                            if (vertical < 0)
                                vertical = 0;
                            rrj_write32(actor + 508, (uint32_t)vertical);
                            (void)sub_8002EAD8(rrj_at(actor + 184, 12), rrj_at(actor + 522, 6), vertical ? 6553 : 0, rrj_at(actor + 184, 12));
                        }
                        if (sub_80037450(actor))
                        {
                            (void)sub_8009246C(actor);
                            rrj_write32(actor + 568, rrj_read32(actor + 568) | 1);
                        }
                    }
                }
                else if (!r_u8(actor + 553))
                {
                    const uint32_t args[8] = {actor, (uint32_t)delta, 0, 0, 0, 0, 0, 0};
                    (void)(uint32_t)sub_800CAA44(args[0], args[1]);
                }
                (void)sub_8008BD2C(actor);
                {
                    uint32_t angle = sub_80020018((uint32_t)((int32_t)r_s16(actor + 444) << 4), (uint32_t)((int32_t)r_s16(actor + 448) << 4));
                    rrj_write32(actor + 292, angle);
                    rrj_write32(actor + 296, (uint32_t)((int32_t)r_s16(0x8005624C + 4 * (angle & 0xFFF) + 2) << 4));
                    rrj_write32(actor + 300, (uint32_t)((int32_t)r_s16(0x8005624C + 4 * (angle & 0xFFF)) << 4));
                }
            }
            rrj_put16(rrj_at(actor + 320, 2), (uint16_t)sub_80039F68(actor + 172));
        }
        actor += stride;
    }
    return stride;
}

static uint32_t race_ca_step(RRJMemory *m, uint32_t actor, int32_t *step)
{
    int32_t side = rrj_s32(rrj_read32(actor + 344));
    int32_t mode = rrj_s32(rrj_read32(actor + 388)) >> 20;
    int32_t limit = (int32_t)leaf_abs32(rrj_s32(rrj_read32(actor + 556)) - rrj_s32(rrj_read32(actor + 560)));
    int32_t boundary = 0;
    if (!(mode & 0xF))
        boundary = side >= 0 ? (int32_t)leaf_abs32(rrj_s32(rrj_read32(actor + 412)) - side) : (int32_t)leaf_abs32(rrj_s32(rrj_read32(actor + 400)) - side);
    if (boundary < limit && *step >= boundary)
    {
        int32_t dot = rrj_s32(sub_8002E698(rrj_at(rrj_read32(actor + 340) + 2, 6), rrj_at(actor + 444, 6)));
        if ((side >= 0 && dot > 0) || (side < 0 && dot < 0))
        {
            *step = boundary;
            return 1;
        }
    }
    if (*step < limit)
        return 0;
    *step = limit;
    return 1;
}

static void race_negate_short_vector(RRJMemory *m, uint32_t vector)
{
    rrj_put16(rrj_at(vector, 2), (uint16_t)-(int16_t)r_u16(vector));
    rrj_put16(rrj_at(vector + 2, 2), (uint16_t)-(int16_t)r_u16(vector + 2));
    rrj_put16(rrj_at(vector + 4, 2), (uint16_t)-(int16_t)r_u16(vector + 4));
}

static void race_ca_reflect(RRJMemory *m, uint32_t actor)
{
    race_negate_short_vector(m, actor + 450);
    rrj_write32(actor + 560, 0);
    rrj_put16(rrj_at(actor + 434, 2), 0);
    rrj_put16(rrj_at(actor + 444, 2), r_u16(actor + 450));
    rrj_put16(rrj_at(actor + 446, 2), r_u16(actor + 452));
    rrj_put16(rrj_at(actor + 448, 2), r_u16(actor + 454));
    rrj_put16(rrj_at(actor + 432, 2), r_u16(actor + 454));
    rrj_put16(rrj_at(actor + 436, 2), (uint16_t)-(int16_t)r_u16(actor + 450));
    {
        uint8_t state = r_u8(actor + 565);
        w_u8(actor + 565, (uint8_t)(((state & 2) ? 0 : 2) | (state & 0xFD)));
    }
}

uint32_t sub_800CA564(uint32_t actor, int32_t delta)
{
    FUNCTION_MARKER(0x800CA564u, "RASHCDG.BIN");
    int32_t step = (int32_t)sub_8001FC90((uint32_t)delta, rrj_read32(actor + 480));
    uint32_t piece = rrj_read32(actor + 340);
    if (!(r_u8(actor + 565) & 4))
    {
        rrj_put16(rrj_at(actor + 438, 2), 0);
        rrj_put16(rrj_at(actor + 440, 2), 4096);
        rrj_put16(rrj_at(actor + 442, 2), 0);
        race_copy_halfwords(rrj_host_context(), actor + 444, piece + 2, 3);
        if (!r_u8(actor + 564) && rrj_s32(rrj_read32(actor + 344)) > 0)
            race_negate_short_vector(rrj_host_context(), actor + 444);
        race_outer_product16(rrj_host_context(), actor + 438, actor + 444, actor + 432);
        race_copy_halfwords(rrj_host_context(), actor + 450, actor + 444, 3);
        xport_update_u8(actor + 565, XPORT_MEMORY_UPDATE_OR, 4);
        rrj_write32(actor + 556, rrj_read32(actor + 412) - rrj_read32(actor + 344));
    }
    if (race_ca_step(rrj_host_context(), actor, &step))
    {
        (void)sub_8002EAD8(rrj_at(actor + 184, 12), rrj_at(actor + 450, 6), (uint32_t)step, rrj_at(actor + 184, 12));
        race_ca_reflect(rrj_host_context(), actor);
        rrj_write32(actor + 556, rrj_read32(actor + 412) - rrj_read32(actor + 400));
    }
    else
    {
        rrj_write32(actor + 560, rrj_read32(actor + 560) + (uint32_t)step);
        (void)sub_8002EAD8(rrj_at(actor + 184, 12), rrj_at(actor + 450, 6), (uint32_t)step, rrj_at(actor + 184, 12));
    }
    (void)sub_80037450(actor);
    rrj_write32(actor + 324, sub_8003B61C(actor + 172));
    {
        uint32_t offset = sub_800B6AAC(rrj_at(actor + 184, 12), rrj_at(piece + 8, 6), rrj_at(piece + 20, 12));
        return sub_8002EAD8(rrj_at(actor + 184, 12), rrj_at(piece + 8, 6), 0u - offset, rrj_at(actor + 184, 12));
    }
}

uint32_t sub_800CA7C4(uint32_t actor, int32_t delta)
{
    FUNCTION_MARKER(0x800CA7C4u, "RASHCDG.BIN");
    int32_t step = (int32_t)sub_8001FC90((uint32_t)delta, rrj_read32(actor + 480));
    int32_t old_side = rrj_s32(rrj_read32(actor + 344));
    uint32_t piece = rrj_read32(actor + 340);
    if (!(r_u8(actor + 565) & 4))
    {
        rrj_put16(rrj_at(actor + 438, 2), 0);
        rrj_put16(rrj_at(actor + 440, 2), 4096);
        rrj_put16(rrj_at(actor + 442, 2), 0);
        race_copy_halfwords(rrj_host_context(), actor + 444, piece + 14, 3);
        rrj_put16(rrj_at(actor + 434, 2), 0);
        xport_update_u8(actor + 565, XPORT_MEMORY_UPDATE_OR, 4);
        race_negate_short_vector(rrj_host_context(), actor + 444);
        rrj_put16(rrj_at(actor + 432, 2), r_u16(actor + 448));
        rrj_put16(rrj_at(actor + 436, 2), (uint16_t)-(int16_t)r_u16(actor + 444));
        race_copy_halfwords(rrj_host_context(), actor + 450, actor + 444, 3);
        rrj_put16(rrj_at(actor + 434, 2), 0);
    }
    if (race_ca_step(rrj_host_context(), actor, &step))
    {
        (void)sub_8002EAD8(rrj_at(actor + 184, 12), rrj_at(actor + 450, 6), (uint32_t)step, rrj_at(actor + 184, 12));
        race_ca_reflect(rrj_host_context(), actor);
    }
    else
    {
        rrj_write32(actor + 560, rrj_read32(actor + 560) + (uint32_t)step);
        (void)sub_8002EAD8(rrj_at(actor + 184, 12), rrj_at(actor + 450, 6), (uint32_t)step, rrj_at(actor + 184, 12));
    }
    (void)sub_80037450(actor);
    rrj_write32(actor + 324, sub_8003B61C(actor + 172));
    {
        uint32_t offset = sub_800B6AAC(rrj_at(actor + 184, 12), rrj_at(piece + 8, 6), rrj_at(piece + 20, 12));
        (void)sub_8002EAD8(rrj_at(actor + 184, 12), rrj_at(piece + 8, 6), 0u - offset, rrj_at(actor + 184, 12));
    }
    (void)sub_8002EAD8(rrj_at(actor + 184, 12), rrj_at(piece + 2, 6), (uint32_t)(old_side - rrj_s32(rrj_read32(actor + 344))), rrj_at(actor + 184, 12));
    rrj_write32(actor + 344, (uint32_t)old_side);
    race_copy_halfwords(rrj_host_context(), actor + 450, piece + 14, 3);
    if (!(r_u8(actor + 565) & 2))
        race_negate_short_vector(rrj_host_context(), actor + 450);
    rrj_put16(rrj_at(actor + 434, 2), 0);
    race_copy_halfwords(rrj_host_context(), actor + 444, actor + 450, 3);
    rrj_put16(rrj_at(actor + 432, 2), r_u16(actor + 454));
    rrj_put16(rrj_at(actor + 436, 2), (uint16_t)-(int16_t)r_u16(actor + 450));
    return (uint32_t)(int32_t)r_s16(actor + 436);
}

uint32_t sub_800CAA44(uint32_t actor, int32_t delta)
{
    FUNCTION_MARKER(0x800CAA44u, "RASHCDG.BIN");
    if (r_u8(actor + 565) & 1)
        return 1;
    if (r_u8(actor + 564) == 1)
        return sub_800CA564(actor, delta);
    if (r_u8(actor + 564) == 2)
        return sub_800CA7C4(actor, delta);
    return 2;
}

uint32_t sub_8009ACA4(void)
{
    FUNCTION_MARKER(0x8009ACA4u, "RASHCDG.BIN");
    int32_t count = rrj_s32(rrj_read32(rrj_read32(0x800CE52C)));
    uint32_t actor = rrj_read32(0x800CE520);
    uint32_t stride = rrj_read32(0x800CE524);
    while (count-- >= 0)
    {
        uint16_t identity = r_u16(actor + 172);
        if (identity && (identity >> 5) == 5)
        {
            uint32_t active = sub_80039F68(actor + 172);
            rrj_put16(rrj_at(actor + 320, 2), (uint16_t)active);
            if (!(active << 16))
                (void)sub_8008C000(actor + 172, 5);
        }
        actor += stride;
    }
    return stride;
}

void sub_8009F3D0(uint32_t identity, uint32_t player)
{
    FUNCTION_MARKER(0x8009F3D0u, "RASHCDG.BIN");
    if (identity)
    {
        uint32_t record = sub_80013204(rrj_read32(identity + 4), player);
        if (record && rrj_read32(record + 8) == rrj_read32(identity + 4))
        {
            uint32_t table = rrj_read32(rrj_read32(rrj_read32(record + 4) + 32) + 48);
            rrj_put16(rrj_at(table + 88 * r_u8(identity + 2) + 4, 2), 0);
        }
    }
}

uint32_t sub_8008DCA0(uint32_t player)
{
    FUNCTION_MARKER(0x8008DCA0u, "RASHCDG.BIN");
    uint32_t state = 0x800D5CF8 + 32 * player;
    int32_t count = rrj_s32(rrj_read32(state));
    uint32_t table = rrj_read32(state + 4);
    int32_t index;
    for (index = 0; index < count; ++index)
    {
        rrj_put16(rrj_at(table + 8 * index + 2, 2), 0xFFFF);
        rrj_write32(table + 8 * index + 4, 0xFFFFFFFFu);
    }
    for (index = 0; index < 3; ++index)
    {
        rrj_write32(state + 4 * index + 8, 0);
        rrj_write32(state + 4 * index + 20, 0xFFFFFFFFu);
    }
    return index < 3;
}

uint32_t sub_8008DE18(uint32_t player, uint32_t group, uint32_t ordinal, uint32_t object)
{
    FUNCTION_MARKER(0x8008DE18u, "RASHCDG.BIN");
    uint32_t state = 0x800D5CF8 + 32 * player;
    uint32_t group_state = state + 4 * group;
    int32_t tail = rrj_s32(rrj_read32(group_state + 20));
    uint32_t table = rrj_read32(state + 4);
    rrj_write32(group_state + 8, rrj_read32(group_state + 8) + 1);
    if (tail == -1)
        rrj_write32(group_state + 20, ordinal);
    else
    {
        int32_t next;
        while ((next = (int16_t)r_u16(table + 8 * tail + 2)) != -1)
            tail = next;
        rrj_put16(rrj_at(table + 8 * tail + 2, 2), (uint16_t)ordinal);
    }
    rrj_write32(table + 8 * ordinal + 4, object);
    return table + 8 * ordinal;
}

uint32_t sub_8008DD20(uint32_t player, uint32_t object, uint32_t ordinal)
{
    FUNCTION_MARKER(0x8008DD20u, "RASHCDG.BIN");
    uint32_t record = 0x800CD898 + 1132 * player;
    int32_t first = (int16_t)r_u16(record + 186) - (int16_t)r_u16(object + 14);
    int32_t second = (int16_t)r_u16(record + 194) - (int16_t)r_u16(object + 22);
    int32_t large = (int32_t)leaf_abs32(first);
    int32_t small = (int32_t)leaf_abs32(second);
    int32_t distance;
    uint32_t group = 0;
    if (large < small)
    {
        int32_t swap = large;
        large = small;
        small = swap;
    }
    distance = large - (large >> 5) - (large >> 7) + ((small + (small >> 1)) >> 2) + ((small + (small >> 1)) >> 6);
    if (distance >= (rrj_s32(rrj_read32(0x8005AD58)) >> 18))
        group = distance < (rrj_s32(rrj_read32(0x8005AD58)) >> 16) ? 1 : 2;
    return sub_8008DE18(player, group, ordinal, object);
}

uint32_t sub_8008D89C(void)
{
    FUNCTION_MARKER(0x8008D89Cu, "RASHCDG.BIN");
    uint32_t players = rrj_read32(rrj_read32(0x8005B2F8) + 48);
    int32_t count = rrj_s32(rrj_read32(rrj_read32(0x800CE53C)));
    uint32_t object = rrj_read32(0x800CE530);
    uint32_t stride = rrj_read32(0x800CE534);
    uint32_t player;
    rrj_write32(0x800CD6B8, 0);
    rrj_write32(0x800CD6BC, 0);
    rrj_write32(0x800CD6C0, 0);
    for (player = 0; player < players; ++player)
        (void)sub_8008DCA0(player);
    while (count >= 0)
    {
        if (r_u16(object))
        {
            uint8_t mask = r_u8(object + 3);
            if (mask & 1)
            {
                rrj_write32(0x800CD6B8, rrj_read32(0x800CD6B8) + 1);
                (void)sub_8008DD20(0, object, (uint32_t)count);
            }
            if (mask & 2)
            {
                rrj_write32(0x800CD6BC, rrj_read32(0x800CD6BC) + 1);
                (void)sub_8008DD20(1, object, (uint32_t)count);
            }
            if (mask == 3)
                rrj_write32(0x800CD6C0, rrj_read32(0x800CD6C0) + 1);
        }
        --count;
        object += stride;
    }
    return stride;
}

uint32_t sub_8009AB60(void)
{
    FUNCTION_MARKER(0x8009AB60u, "RASHCDG.BIN");
    int32_t count = rrj_s32(rrj_read32(rrj_read32(0x800CE53C)));
    uint32_t identity = rrj_read32(0x800CE530);
    uint32_t stride = rrj_read32(0x800CE534);
    uint32_t players = rrj_read32(rrj_read32(0x8005B2F8) + 48);
    while (count-- >= 0)
    {
        if (r_u16(identity))
        {
            uint32_t active = sub_80039F68(identity);
            uint32_t player;
            rrj_put16(rrj_at(identity + 148, 2), (uint16_t)active);
            if (active << 16)
            {
                for (player = 0; player < players; ++player)
                    (void)sub_80013294(6, r_u16(identity), player);
            }
            else
            {
                for (player = 0; player < players; ++player)
                    sub_8009F3D0(identity, player);
                (void)sub_8008C000(identity, 6);
            }
        }
        identity += stride;
    }
    return sub_8008D89C();
}

static uint8_t race_grid_packed(RRJMemory *m, uint8_t node)
{
    return r_u8(0x800CCFA9u + 2u * node);
}

static uint8_t race_grid_next(RRJMemory *m, uint8_t node)
{
    return r_u8(0x800CCFA8u + 2u * node);
}

static int race_grid_cell(RRJMemory *m, int32_t x, int32_t z, uint32_t players, uint32_t *cell)
{
    int32_t column = (rrj_s32((uint32_t)x - rrj_read32(0x800CCF98)) >> 21) + 11;
    int32_t row = (rrj_s32((uint32_t)z - rrj_read32(0x800CCFA0)) >> 21) + 11;
    if (players != 1 && ((uint32_t)column >= 24 || (uint32_t)row >= 24))
    {
        column = (rrj_s32((uint32_t)x - rrj_read32(0x800CCF9C)) >> 21) + 11;
        row = (rrj_s32((uint32_t)z - rrj_read32(0x800CCFA4)) >> 21) + 11;
        if (row >= 0)
            row += 24;
    }
    if ((uint32_t)column >= 24 || (uint32_t)row >= (24u << ((players - 1u) & 31u)))
        return 0;
    *cell = (uint32_t)column + 24u * (uint32_t)row;
    return 1;
}

static uint8_t race_grid_after_identity(RRJMemory *m, int32_t x, int32_t z, uint32_t players, uint8_t identity)
{
    uint32_t cell;
    uint8_t node;
    if (!race_grid_cell(m, x, z, players, &cell))
        return 0x80;
    node = r_u8(0x800CD0B0u + cell);
    while (node != 0x80 && race_grid_packed(m, node) != identity)
        node = race_grid_next(m, node);
    return race_grid_next(m, node);
}

static uint32_t race_grid_candidate(RRJMemory *m, uint8_t packed, uint32_t *position)
{
    uint32_t kind = packed >> 5;
    uint32_t slot = packed & 31u;
    uint32_t object;
    if (kind == 6)
    {
        object = rrj_read32(0x800CD6C4) + 280u * slot;
        *position = object + 12;
    }
    else
    {
        uint32_t group = 0x800CE4D0u + 16u * kind;
        object = rrj_read32(group) + rrj_read32(group + 4) * slot;
        *position = object + 184;
    }
    return object;
}

static uint8_t race_grid_take(RRJMemory *m, uint8_t nodes[4])
{
    uint32_t first = race_grid_packed(m, nodes[0]);
    uint32_t second = race_grid_packed(m, nodes[1]);
    uint32_t third = race_grid_packed(m, nodes[2]);
    uint32_t fourth = race_grid_packed(m, nodes[3]);
    uint32_t left = first < second ? first : second;
    uint32_t right = third < fourth ? third : fourth;
    uint8_t packed = (uint8_t)(left < right ? left : right);
    uint32_t index;
    for (index = 0; index < 4; ++index)
    {
        if (race_grid_packed(m, nodes[index]) == packed)
            nodes[index] = race_grid_next(m, nodes[index]);
    }
    return packed;
}

static int race_grid_lists_empty(const uint8_t nodes[4])
{
    return nodes[0] == 0x80 && nodes[1] == 0x80 && nodes[2] == 0x80 && nodes[3] == 0x80;
}

static uint32_t race_grid_metric(RRJMemory *m, uint32_t first, uint32_t second)
{
    uint32_t dx = leaf_abs32(rrj_s32(rrj_read32(first) - rrj_read32(second)));
    uint32_t dz = leaf_abs32(rrj_s32(rrj_read32(first + 8) - rrj_read32(second + 8)));
    return dx + dz - (dx < dz ? dx : dz) / 2u;
}

static void race_grid_insert_objects(RRJMemory *m, uint32_t players, uint32_t *node_count)
{
    int32_t kind;
    for (kind = 6; kind >= 0 && *node_count < 128; --kind)
    {
        uint32_t group = 0x800CE4D0u + 16u * (uint32_t)kind;
        uint32_t count_pointer = rrj_read32(group + 8);
        int32_t slot;
        uint32_t object;
        uint32_t stride;
        if (!count_pointer || !rrj_read32(count_pointer))
            continue;
        object = rrj_read32(group);
        stride = rrj_read32(group + 4);
        slot = rrj_s32(rrj_read32(rrj_read32(group + 12)));
        while (slot-- >= 0 && *node_count < 128)
        {
            uint32_t position = object + 196;
            uint32_t point_count = 4;
            uint8_t packed;
            uint32_t point;
            int active = 0;
            if (kind == 6)
            {
                packed = (uint8_t)r_u16(object);
                active = packed && r_s16(object + 148);
                if (active)
                {
                    if (rrj_read32(object + 8) == 1)
                    {
                        position = object + 12;
                        point_count = 1;
                    }
                    else
                    {
                        position = object + 24;
                    }
                }
            }
            else
            {
                packed = (uint8_t)r_u16(object + 172);
                active = r_s16(object + 320) != 0;
                if (kind == 1 && rrj_read32(object + 604) < 2)
                    active = 0;
                if (active && kind == 0)
                {
                    uint32_t enabled = rrj_read32(object + 616) || rrj_read32(object + 676);
                    rrj_write32(object + 568, rrj_read32(object + 568) & ~0x02000000u);
                    rrj_write32(object + 552, 0);
                    rrj_write32(object + 568, rrj_read32(object + 568) | (enabled << 24));
                }
                if (!packed)
                    active = 0;
            }
            if (active)
            {
                for (point = 0; point < point_count && *node_count < 128; ++point, position += 12)
                {
                    uint32_t cell;
                    if (race_grid_cell(m, rrj_s32(rrj_read32(position)), rrj_s32(rrj_read32(position + 8)), players, &cell))
                    {
                        uint8_t old = r_u8(0x800CD0B0u + cell);
                        if (old == 0x80 || race_grid_packed(m, old) != packed)
                        {
                            w_u8(0x800CCFA8u + 2u * *node_count, old);
                            w_u8(0x800CCFA9u + 2u * *node_count, packed);
                            w_u8(0x800CD0B0u + cell, (uint8_t)(*node_count));
                            ++*node_count;
                        }
                    }
                }
            }
            object += stride;
        }
    }
}

static void race_grid_player_pairs(RRJMemory *m, uint32_t players, RRJRaceLeafCall call)
{
    uint32_t group = 0x800CE4D0;
    uint32_t count_pointer = rrj_read32(group + 8);
    int32_t slot;
    uint32_t actor;
    uint32_t stride;
    if (!count_pointer || !rrj_read32(count_pointer))
        return;
    actor = rrj_read32(group);
    stride = rrj_read32(group + 4);
    slot = rrj_s32(rrj_read32(rrj_read32(group + 12)));
    while (slot-- >= 0)
    {
        uint8_t nodes[4];
        uint8_t identity;
        uint32_t point;
        if (!r_s16(actor + 320))
        {
            actor += stride;
            continue;
        }
        identity = (uint8_t)r_u16(actor + 172);
        for (point = 0; point < 4; ++point)
        {
            uint32_t position = actor + 196 + 12u * point;
            nodes[point] = race_grid_after_identity(m, rrj_s32(rrj_read32(position)), rrj_s32(rrj_read32(position + 8)), players, identity);
        }
        while (!race_grid_lists_empty(nodes))
        {
            uint8_t packed = race_grid_take(m, nodes);
            if (!(packed >> 5))
            {
                uint32_t candidate_position;
                uint32_t candidate = race_grid_candidate(m, packed, &candidate_position);
                if (race_grid_metric(m, candidate_position, actor + 184) < rrj_read32(0x800CCA8C))
                {
                    uint32_t actor_flags = rrj_read32(actor + 568);
                    uint32_t candidate_flags = rrj_read32(candidate + 568);
                    int actor_moving = (actor_flags & 0x1E0) && ((actor_flags & 0x200000) || leaf_abs32(rrj_s32(rrj_read32(actor + 652))) > 45752);
                    int candidate_moving = (candidate_flags & 0x1E0) && ((candidate_flags & 0x200000) || leaf_abs32(rrj_s32(rrj_read32(candidate + 652))) > 45752);
                    if ((candidate_moving || !actor_moving) && r_u16(rrj_read32(actor + 852) + 544))
                    {
                        if ((actor_moving || !candidate_moving) && r_u16(rrj_read32(candidate + 852) + 544))
                            (void)leaf_call(m, call, 0x800AB7A0, actor, candidate, 0, 0);
                        else
                            (void)leaf_call(m, call, 0x800B09C4, actor, candidate + 172, 0, 0);
                    }
                    else
                    {
                        (void)leaf_call(m, call, 0x800B09C4, candidate, actor + 172, 0, 0);
                    }
                }
            }
        }
        actor += stride;
    }
}

static void race_grid_dispatch_pair(RRJMemory *m, uint32_t pass, uint32_t actor, uint32_t candidate, uint32_t packed_address, uint8_t packed, RRJRaceLeafCall call)
{
    uint32_t kind = packed >> 5;
    if (pass == 0)
    {
        if (kind == 1 || kind == 2)
        {
            if ((uint32_t)(r_u16(candidate + 544) - 60u) >= 4)
                (void)leaf_call(m, call, 0x800AD04C, actor, 0, 0, 0);
        }
        else if (kind == 3)
            (void)leaf_call(m, call, 0x800AC5BC, actor, candidate, packed_address, 0);
        else if (kind == 4)
        {
            uint32_t header = rrj_read32(candidate);
            if ((r_u16(header + 14) & 2) && !(rrj_read32(candidate + 592) & 0x1A06))
                (void)leaf_call(m, call, 0x800AE794, actor, candidate + 172, packed_address, 0);
            else
                (void)leaf_call(m, call, 0x800B09C4, actor, candidate + 172, 0, 0);
        }
        else if (kind == 5)
        {
            if (rrj_s32(rrj_read32(candidate + 304)) > 0x10000 && rrj_s32(rrj_read32(candidate + 308)) > 0x10000)
                (void)leaf_call(m, call, 0x800B0D8C, actor, 0, 0, 0);
            else
                (void)leaf_call(m, call, 0x800B09C4, actor, candidate + 172, 0, 0);
        }
        else if (kind == 6)
        {
            if (rrj_read32(candidate + 8))
                (void)leaf_call(m, call, 0x800AE794, actor, candidate, packed_address, 0);
            else if (rrj_s32(rrj_read32(candidate + 132)) > 0x10000 && rrj_s32(rrj_read32(candidate + 136)) > 0x10000)
                (void)leaf_call(m, call, 0x800B0D8C, actor, 0, 0, 0);
            else
                (void)leaf_call(m, call, 0x800B09C4, actor, candidate, 0, 0);
        }
    }
    else if (pass == 1 || pass == 2)
    {
        if (kind == 1 || kind == 2)
            (void)leaf_call(m, call, 0x800B2AF8, actor, candidate, packed_address, 0);
        else if (kind == 3)
            (void)sub_800B2844(actor, candidate);
        else if (kind == 4 || kind == 5)
            (void)leaf_call(m, call, 0x800B2B00, actor, candidate + 172, packed_address, 0);
        else if (kind == 6)
            (void)leaf_call(m, call, 0x800B2B00, actor, candidate, packed_address, 0);
    }
    else if (pass == 3)
    {
        if (kind == 3)
            (void)leaf_call(m, call, 0x800B2D44, actor, candidate, packed_address, 0);
        else if (kind == 4)
            (void)leaf_call(m, call, 0x800B2D88, actor, candidate, packed_address, 0);
    }
    else if (pass == 4)
    {
        if (kind == 5)
            (void)leaf_call(m, call, 0x800B2E64, actor, candidate + 172, 0, 0);
        else if (kind == 6)
            (void)leaf_call(m, call, 0x800B2E64, actor, candidate, 0, 0);
    }
}

static void race_grid_actor_post(RRJMemory *m, uint32_t pass, uint32_t actor, RRJRaceLeafCall call)
{
    if (pass == 0)
    {
        uint32_t state = leaf_call(m, call, 0x800B3AD0, actor, (rrj_read32(actor + 568) & 0x600) != 0, 0, 0);
        uint32_t linked = rrj_read32(actor + 856);
        if (linked && (((state & 1) != ((state & 2) >> 1)) || (!state && ((rrj_s32(rrj_read32(actor + 388)) >> 20) & 15))))
            (void)leaf_call(m, call, 0x800B3AD0, linked, (rrj_read32(actor + 568) & 0x600) != 0, 0, 0);
        if (!(rrj_read32(actor + 568) & 0x600) || (rrj_read32(actor + 564) & 0x800000))
        {
            if ((rrj_read32(actor + 564) & 0x400000) && !(rrj_read32(actor + 568) & 15) && leaf_call(m, call, 0x80083F30, actor, actor + 172, actor + 820, 4))
            {
                uint32_t sound = (rrj_read32(actor + 564) & 14) ? 17 : 3;
                sub_80017BA0(rrj_s32(rrj_read32(actor + 184)), rrj_s32(rrj_read32(actor + 192)), sound, 0);
                if (r_u16(actor + 172) < rrj_read32(rrj_read32(0x8005B2F8) + 48) && rrj_read32(rrj_read32(actor + 852) + 604) < 2 && !rrj_read32(0x8005B220))
                {
                    const uint32_t args[8] = {actor, 0, rrj_read32(actor + 480), 1464860, 1, 0, 0, 0};
                    (void)leaf_call8(m, call, 0x800B658C, args);
                }
            }
        }
        else
            (void)leaf_call(m, call, 0x800B1978, actor, 0, 0, 0);
    }
    else if (pass == 1 || pass == 2)
    {
        (void)leaf_call(m, call, 0x800B3AD0, actor, (rrj_read32(actor + 552) >> 30) & 1, 0, 0);
        if ((rrj_read32(actor + 552) & 0xC0000000u) == 0x40000000u && rrj_s32(sub_8002E698(rrj_at(actor + 522, 6), rrj_at(actor + 450, 6))) > 0)
            (void)leaf_call(m, call, 0x800B208C, actor, 0, 0, 0);
    }
    else if (pass == 4)
    {
        (void)leaf_call(m, call, 0x800B3AD0, actor, (rrj_read32(actor + 592) >> 1) & 1, 0, 0);
        if ((rrj_read32(actor + 592) & 3) == 2 && rrj_s32(sub_8002E698(rrj_at(actor + 522, 6), rrj_at(actor + 450, 6))) > 0)
        {
            uint32_t vector = 0x1F8003E0;
            rrj_put16(rrj_at(vector, 2), (uint16_t)-(int16_t)r_u16(actor + 522));
            rrj_put16(rrj_at(vector + 2, 2), (uint16_t)-(int16_t)r_u16(actor + 524));
            rrj_put16(rrj_at(vector + 4, 2), (uint16_t)-(int16_t)r_u16(actor + 526));
            (void)leaf_call(m, call, 0x800B3344, actor, actor + 504, vector, 1);
        }
    }
}

uint32_t sub_800A4774(int32_t delta, RRJRaceLeafCall call)
{
    uint32_t players = rrj_read32(rrj_read32(0x8005B2F8) + 48);
    uint32_t node_count = 0;
    uint32_t pass;
    FUNCTION_MARKER(0x800A4774, "RASHCDG.BIN");
    rrj_write32(0x800CCE38, (uint32_t)delta);
    xport_guest_fill(0x800CD0B0, 0x80, 1152);
    rrj_write32(0x800CCF98, rrj_read32(rrj_read32(0x8005B38C) + 184) + 0xF0000);
    rrj_write32(0x800CCFA0, rrj_read32(rrj_read32(0x8005B38C) + 192) + 0xF0000);
    if (players >= 2)
    {
        rrj_write32(0x800CCF9C, rrj_read32(rrj_read32(0x8005B21C) + 184) + 0xF0000);
        rrj_write32(0x800CCFA4, rrj_read32(rrj_read32(0x8005B21C) + 192) + 0xF0000);
    }
    race_grid_insert_objects(rrj_host_context(), players, &node_count);
    if (!node_count)
        return 0;
    rrj_write32(0x800CCF68, 0);
    (void)sub_8001E100(0x800CCE48, 0, 288);
    rrj_write32(0x800CCF70, rrj_read32(0x800CCF78));
    rrj_write32(0x800CCF74, rrj_read32(0x800CCF7C));
    rrj_write32(0x800CCF78, 1);
    rrj_write32(0x800CCF7C, 0);
    rrj_write32(0x800CCF80, 0);
    rrj_write32(0x800CCF90, 0);
    (void)sub_8001E100(0x800CCF88, 0, 8);
    race_grid_player_pairs(rrj_host_context(), players, call);
    for (pass = 0; pass < 5; ++pass)
    {
        uint32_t group = 0x800CE4D0u + 16u * pass;
        uint32_t count_pointer = rrj_read32(group + 8);
        if (count_pointer && rrj_read32(count_pointer))
        {
            uint32_t actor = rrj_read32(group);
            uint32_t stride = rrj_read32(group + 4);
            int32_t slot = rrj_s32(rrj_read32(rrj_read32(group + 12)));
            while (slot-- >= 0)
            {
                uint8_t nodes[4];
                uint8_t identity;
                uint32_t point;
                uint32_t radius;
                int special;
                if (!r_s16(actor + 320) || (pass && !r_u16(actor + 172)) || (pass == 1 && rrj_read32(actor + 604) < 2))
                {
                    actor += stride;
                    continue;
                }
                special = (rrj_read32(actor + 388) & 1) != 0;
                if (!special && rrj_read32(actor + 372))
                {
                    if (r_s16(actor + 392) == 4)
                    {
                        actor += stride;
                        continue;
                    }
                    special = 1;
                }
                radius = rrj_read32(0x800CCA8C + 4u * pass) + (special ? 0xA0000u : 0);
                identity = (uint8_t)r_u16(actor + 172);
                for (point = 0; point < 4; ++point)
                {
                    uint32_t position = actor + 196 + 12u * point;
                    nodes[point] = race_grid_after_identity(rrj_host_context(), rrj_s32(rrj_read32(position)), rrj_s32(rrj_read32(position + 8)), players, identity);
                }
                if (pass != 4 || rrj_read32(actor + 480))
                {
                    while (!race_grid_lists_empty(nodes))
                    {
                        uint8_t packed = race_grid_take(rrj_host_context(), nodes);
                        uint32_t candidate_position;
                        uint32_t candidate = race_grid_candidate(rrj_host_context(), packed, &candidate_position);
                        if (race_grid_metric(rrj_host_context(), candidate_position, actor + 184) < radius)
                        {
                            uint32_t packed_address = 0x1F8003DC;
                            rrj_write32(packed_address, packed);
                            race_grid_dispatch_pair(rrj_host_context(), pass, actor, candidate, packed_address, packed, call);
                        }
                    }
                    race_grid_actor_post(rrj_host_context(), pass, actor, call);
                }
                actor += stride;
            }
        }
        if (!pass && rrj_read32(0x800CCF68))
            (void)leaf_call(rrj_host_context(), call, 0x800A77B0, 0, 0, 0, 0);
    }
    return 0;
}

static uint32_t race_collision_call6(RRJMemory *m, RRJRaceLeafCall call, uint32_t target, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5)
{
    const uint32_t args[8] = {a0, a1, a2, a3, a4, a5, 0, 0};
    return leaf_call8(m, call, target, args);
}

static void race_collision_negate_normal(RRJMemory *m, uint32_t normal)
{
    uint32_t axis;
    for (axis = 0; axis < 3; ++axis)
        rrj_put16(rrj_at(normal + 2 * axis, 2), (uint16_t)-(int16_t)r_u16(normal + 2 * axis));
}

static uint32_t race_collision_return(RRJMemory *m, const uint8_t saved[256], uint32_t result)
{
    xport_guest_copy(xport_guest_ref(0x1F800100), xport_host_ref((void *)saved), 256);
    return result;
}

uint32_t sub_800B3AD0(uint32_t actor, uint32_t mode, RRJRaceLeafCall call)
{
    const uint32_t scratch = 0x1F800100;
    const uint32_t normal = scratch;
    const uint32_t edge_a = scratch + 8;
    const uint32_t edge_b = scratch + 20;
    const uint32_t probe_a = scratch + 32;
    const uint32_t response = scratch + 48;
    uint8_t saved[256];
    uint32_t state = 1;
    uint32_t special = 0;
    uint32_t side_flags = 0;
    int32_t side_limit = 2048;
    uint32_t tracked = 0;
    uint32_t special_actor = 0;
    int32_t best_distance = 0x7FFF0000;
    uint32_t surface_time = 0;
    uint32_t best_surface = 0;
    uint32_t result = 0;
    uint32_t previous_material;
    uint32_t flags = 0;
    uint32_t model;
    uint32_t group_count;
    uint32_t group_index = 0;
    uint32_t segment_count;
    uint32_t segment_start;
    uint32_t outer_index;
    uint32_t actor_normal = actor + 450;

    FUNCTION_MARKER(0x800B3AD0, "RASHCDG.BIN");
    memcpy(saved, rrj_at(scratch, 256), 256);
    previous_material = rrj_read32(actor + 388) >> 20;
    rrj_write32(actor + 388, rrj_read32(actor + 388) & 0x000FFFEFu);
    model = rrj_read32(actor + 328);
    if ((r_u16(actor + 172) >> 5) == 4 && (r_u16(actor + 172) & 31) >= 30)
    {
        special = 1;
        special_actor = actor;
        rrj_write32(actor + 768, 0);
    }
    {
        uint32_t direct_group = rrj_read32(actor + 496);
        if (direct_group)
        {
            segment_count = r_u8(direct_group + 3);
            if (!segment_count)
                return race_collision_return(rrj_host_context(), saved, 0);
            if (!(rrj_read32(actor + 388) & 1) && rrj_read32(actor + 372))
            {
                if (r_u16(actor + 392) == 4)
                    return race_collision_return(rrj_host_context(), saved, 0);
            }
            side_flags = 0;
            flags = 0x100;
            group_count = 1;
            segment_start = (uint32_t)(int32_t)r_s16(direct_group);
        }
        else
        {
            uint32_t selector = rrj_read32(actor + 492);
            int32_t travel = rrj_s32(rrj_read32(actor + 344));
            uint32_t entry;
            if (!selector || !rrj_read32(actor + 372))
                return race_collision_return(rrj_host_context(), saved, 0);
            if (travel >= 0)
            {
                group_count = r_u8(selector + 3);
                if (!group_count)
                    return race_collision_return(rrj_host_context(), saved, 0);
                group_index = r_u8(selector + 2);
                if (rrj_s32(rrj_read32(actor + 424)) * (int32_t)r_s16(actor + 420) - 0x8000 < travel)
                    flags = 0x80;
            }
            else
            {
                group_count = r_u8(selector + 1);
                if (!group_count)
                    return race_collision_return(rrj_host_context(), saved, 0);
                group_index = r_u8(selector);
                if (rrj_s32(rrj_read32(actor + 424)) * (int32_t)r_s16(actor + 408) - 0x8000 < -travel)
                    flags = 0x40;
            }
            entry = rrj_read32(model + 100) + 4 * group_index;
            segment_count = r_u8(entry + 3);
            segment_start = (uint32_t)(int32_t)r_s16(entry);
            if (!mode && !special && !flags)
                return race_collision_return(rrj_host_context(), saved, 0);
        }
    }

    {
        uint32_t type = r_u16(actor + 172) >> 5;
        uint32_t linked_mode = type == 0;
        if (!type)
        {
            tracked = actor;
            linked_mode += rrj_read32(actor + 1088) == 0;
            if (rrj_read32(actor + 1088))
                (void)(uint32_t)sub_800A8FE8(actor);
            if (leaf_abs32(rrj_s32(rrj_read32(actor + 676))) >= 2130)
                state = 2;
            if (rrj_read32(actor + 856) && rrj_read32(actor + 1088) && !(rrj_read32(actor + 320) & 8))
            {
                (void)(uint32_t)sub_800A8C78(actor, 1310720, 1835008);
                rrj_put16(rrj_at(rrj_read32(actor + 856) + 320, 2), r_u16(actor + 320));
            }
            if (rrj_s32(rrj_read32(actor + 636)) < 0)
                side_flags |= 2;
        }

        for (outer_index = 0; outer_index < group_count; ++outer_index, ++group_index)
        {
            uint32_t geometry;
            uint32_t surface;
            uint32_t last_surface;
            int32_t direction;
            uint32_t surface_index;
            uint16_t previous_flags = 0;
            uint32_t step;
            uint32_t edge_cursor;

            if (outer_index)
            {
                uint32_t entry = rrj_read32(model + 100) + 4 * group_index;
                uint32_t next_count = r_u8(entry + 3);
                segment_start = (uint32_t)(int32_t)r_s16(entry);
                segment_count = next_count;
                if (special && rrj_read32(special_actor + 768))
                    return race_collision_return(rrj_host_context(), saved, 1);
            }
            geometry = rrj_read32(model + 96) + 40 * segment_start;
            surface = geometry;
            last_surface = geometry + 40 * segment_count - 40;
            direction = 1;
            {
                uint32_t basis = rrj_read32(actor + 340);
                int32_t first = rrj_s32(sub_800B6AAC(rrj_at(geometry + 8, 12), rrj_at(basis + 2, 6), rrj_at(basis + 20, 12)));
                int32_t second = rrj_s32(sub_800B6AAC(rrj_at(last_surface + 20, 12), rrj_at(basis + 2, 6), rrj_at(basis + 20, 12)));
                (void)sub_8002EAD8(rrj_at(last_surface + 20, 12), rrj_at(basis + 2, 6), (uint32_t)(first - second), rrj_at(edge_b, 12));
                if (rrj_s32(sub_800B6AAC(rrj_at(edge_b, 12), rrj_at(actor_normal, 6), rrj_at(actor + 184, 12))) < rrj_s32(sub_800B6AAC(rrj_at(geometry + 8, 12), rrj_at(actor_normal, 6), rrj_at(actor + 184, 12))))
                {
                    direction = -1;
                    surface = last_surface;
                }
            }
            flags &= 0xFFFEF3FFu;
            surface_index = 0;
            step = (uint32_t)(40 * direction);
            edge_cursor = surface + 32;

            while (surface_index < segment_count)
            {
                uint32_t selected = 0;
                uint32_t material;
                uint32_t contact_flags;
                uint32_t wheel = 8;
                int32_t contact_distance = 0;
                int32_t motion_distance;
                int32_t surface_offset;
                int32_t tangent_ratio = 0;
                int32_t first_projection;
                int32_t second_projection;

                if ((r_u16(surface + 2) & 15) == 0 || (r_u16(surface + 2) & 15) >= 9)
                    rrj_put16(rrj_at(surface + 2, 2), 1026);
                if (special)
                {
                    if (!(r_u16(surface + 2) & 0x100))
                        goto reject_surface;
                    if (rrj_read32(special_actor + 768))
                    {
                        if ((flags & 0x10800) != 0x10000)
                            return race_collision_return(rrj_host_context(), saved, 1);
                        (void)(uint32_t)sub_800A451C(special_actor, 1);
                        flags |= 0x800;
                    }
                    flags &= ~0x20000u;
                    if (!(flags & 0x18))
                    {
                        if (!(flags & 0x800))
                            flags &= ~0x10000u;
                        if (flags & 0x400)
                        {
                            selected = last_surface - step;
                            flags &= ~0x400u;
                        }
                        else if (surface_index < segment_count - 1 && (r_u16(surface + step + 2) & 0x100))
                        {
                            int32_t current_dot;
                            int32_t next_dot;
                            last_surface = surface + step;
                            rrj_write32(edge_a, (rrj_read32(last_surface + 8) + rrj_read32(last_surface + 20)) / 2);
                            rrj_write32(edge_a + 4, (rrj_read32(last_surface + 12) + rrj_read32(last_surface + 24)) / 2);
                            rrj_write32(edge_a + 8, (rrj_read32(last_surface + 16) + rrj_read32(last_surface + 28)) / 2);
                            contact_distance = rrj_s32(sub_800B6AAC(rrj_at(edge_a, 12), rrj_at(edge_cursor, 6), rrj_at(surface + 20, 12)));
                            current_dot = rrj_s32(sub_8002E698(rrj_at(edge_cursor, 6), rrj_at(actor_normal, 6)));
                            next_dot = rrj_s32(sub_8002E698(rrj_at(last_surface + 32, 6), rrj_at(actor_normal, 6)));
                            if (contact_distance <= 0x8000)
                            {
                                if (!(flags & 0x10000) && current_dot < -46333)
                                    flags |= 0x400;
                            }
                            else if (leaf_abs32(current_dot) < leaf_abs32(next_dot))
                                flags |= 0x10400;
                            else
                                flags |= 0x10000;
                        }
                        if (!(flags & 0x400) && surface_index && (r_u16(surface - step + 2) & 0x100))
                        {
                            int32_t previous_dot = rrj_s32(sub_8002E698(rrj_at(edge_cursor, 6), rrj_at(surface - step + 32, 6)));
                            int32_t actor_dot = rrj_s32(sub_8002E698(rrj_at(edge_cursor, 6), rrj_at(actor_normal, 6)));
                            if (previous_dot <= 46332 && actor_dot > 56754)
                                flags |= 0x20000;
                            if (flags & 0x20000)
                                flags &= ~0x10000u;
                        }
                        if (flags & 0x400)
                            selected = last_surface;
                    }
                }
                if (!selected)
                    selected = surface;

                contact_flags = flags & ~0x200u;
                material = r_u16(selected + 2);
                if ((int16_t)material != (int16_t)previous_flags)
                    contact_flags |= 0x200;
                rrj_write32(normal, rrj_read32(selected + 32));
                rrj_put16(rrj_at(normal + 4, 2), r_u16(selected + 36));
                rrj_write32(edge_a, rrj_read32(selected + 8));
                rrj_write32(edge_a + 4, rrj_read32(selected + 12));
                rrj_write32(edge_a + 8, rrj_read32(selected + 16));
                rrj_write32(edge_b, rrj_read32(selected + 20));
                rrj_write32(edge_b + 4, rrj_read32(selected + 24));
                rrj_write32(edge_b + 8, rrj_read32(selected + 28));
                previous_flags = (uint16_t)material;
                surface_offset = rrj_s32(rrj_read32(selected + 4));
                if (leaf_abs32((int16_t)r_u16(normal + 2)) >= 65)
                {
                    rrj_put16(rrj_at(normal + 2, 2), 0);
                    (void)sub_8002E468(normal);
                }
                flags = contact_flags & 0xFFFFFFE0u;

                if ((flags & 0x200) && (material & 15) == 5 && (mode || special))
                {
                    if ((previous_material & 15) == 5 || special || r_s16(actor + 452) < 0)
                    {
                        flags |= 0x10;
                        rrj_write32(normal, 0x10000000);
                        rrj_put16(rrj_at(normal + 4, 2), 0);
                        ++segment_count;
                        uint32_t basis = rrj_read32(actor + 340);
                        uint32_t collision = rrj_read32(actor + 372);
                        (void)sub_8002EAD8(rrj_at(basis + 20, 12), rrj_at(basis + 8, 6), (uint32_t)surface_offset, rrj_at(edge_a, 12));
                        (void)sub_8002EAD8(rrj_at(edge_a, 12), rrj_at(basis + 2, 6), rrj_read32(collision + 144), rrj_at(edge_b, 12));
                        (void)sub_8002EAD8(rrj_at(edge_a, 12), rrj_at(basis + 2, 6), rrj_read32(collision + 16), rrj_at(edge_b, 12));
                    }
                    else if (rrj_s32(rrj_read32(actor + 188)) < rrj_s32(rrj_read32(rrj_read32(actor + 340) + 24) - (uint32_t)surface_offset + 0x10000))
                    {
                        rrj_put16(rrj_at(normal, 2), r_u16(actor + 354));
                        rrj_put16(rrj_at(normal + 2, 2), r_u16(actor + 356));
                        rrj_put16(rrj_at(normal + 4, 2), r_u16(actor + 358));
                        flags |= 8;
                        ++segment_count;
                        if (rrj_s32(rrj_read32(actor + 364)) > 0)
                            race_collision_negate_normal(rrj_host_context(), normal);
                        if (direction < 0)
                        {
                            rrj_write32(edge_a, rrj_read32(selected + 20));
                            rrj_write32(edge_a + 4, rrj_read32(selected + 24));
                            rrj_write32(edge_a + 8, rrj_read32(selected + 28));
                        }
                        {
                            uint32_t basis = rrj_read32(actor + 340);
                            uint32_t collision = rrj_read32(actor + 372);
                            uint32_t scale = rrj_s32(rrj_read32(actor + 344)) >= 0 ? 0u - rrj_read32(collision + 144) : 0u - rrj_read32(collision + 16);
                            (void)sub_8002EAD8(rrj_at(edge_a, 12), rrj_at(basis + 2, 6), scale, rrj_at(edge_b, 12));
                        }
                    }
                }
                if (!(flags & 0x1D8))
                    goto outer_next;

                {
                    int32_t ax = rrj_s32(rrj_read32(actor + 184));
                    int32_t az = rrj_s32(rrj_read32(actor + 192));
                    uint32_t dx0 = leaf_abs32(ax - rrj_s32(rrj_read32(edge_a)));
                    uint32_t dz0 = leaf_abs32(az - rrj_s32(rrj_read32(edge_a + 8)));
                    uint32_t dx1 = leaf_abs32(ax - rrj_s32(rrj_read32(edge_b)));
                    uint32_t dz1 = leaf_abs32(az - rrj_s32(rrj_read32(edge_b + 8)));
                    uint32_t high;
                    uint32_t low;
                    if (dx0 < dz0)
                    {
                        high = dz0;
                        low = dx0;
                    }
                    else
                    {
                        high = dx0;
                        low = dz0;
                    }
                    first_projection = (int32_t)(high - (high >> 5) - (high >> 7) + ((low + (low >> 1)) >> 2) + ((low + (low >> 1)) >> 6));
                    if (dx1 < dz1)
                    {
                        high = dz1;
                        low = dx1;
                    }
                    else
                    {
                        high = dx1;
                        low = dz1;
                    }
                    second_projection = (int32_t)(high - (high >> 5) - (high >> 7) + ((low + (low >> 1)) >> 2) + ((low + (low >> 1)) >> 6));
                    if (first_projection > 12517376 || second_projection > 12517376)
                        goto reject_surface;
                }

                if (material & 0x400)
                {
                    if ((r_u16(actor + 172) >> 5) == 1 && (material & 0xA00) == 0 && rrj_read32(actor + 604) == 4)
                        goto reject_surface;
                    flags |= sub_800B6AAC(rrj_at(actor + 468, 12), rrj_at(normal, 6), rrj_at(edge_a, 12)) >> 31;
                    if ((material & 15) == 1 && (flags & 1))
                        goto reject_surface;
                }

                motion_distance = rrj_s32((uint32_t)sub_8001FC90(rrj_s32(rrj_read32(0x800CCE38)), rrj_s32(rrj_read32(actor + 480))));
                side_flags &= 0xFFFFFFF2u;
                if (!special && !(flags & 0x18) && !(material & 0x800))
                {
                    uint32_t game = rrj_read32(0x8005B2F8);
                    uint32_t actor_type = r_u16(actor + 172) >> 5;
                    int test = (actor_type < 2 && (uint32_t)(r_u16(actor + 172) & 31) < rrj_read32(game + 48));
                    if (!test)
                        test = (uint32_t)sub_800A8C78(actor, 1310720, 1835008) != 0;
                    if (test)
                    {
                        int32_t dot = rrj_s32(sub_8002E698(rrj_at(actor_normal, 6), rrj_at(normal, 6)));
                        uint32_t absolute = leaf_abs32(dot);
                        if ((previous_material & 15) == 0 || (motion_distance >= 132 && absolute > 5701))
                        {
                            flags |= 4;
                            if (motion_distance < 0x20000)
                                motion_distance = 0x20000;
                            if (linked_mode)
                            {
                                int32_t cross = (int32_t)r_s16(actor + 454) * (int16_t)r_u16(normal) >= (int32_t)r_s16(actor + 450) * (int16_t)r_u16(normal + 4);
                                side_flags |= 4 * (cross ^ (flags & 1)) | (absolute <= 37485);
                            }
                        }
                    }
                    else if ((previous_material & 15) == 0 || leaf_abs32(motion_distance) >= 131)
                    {
                        int32_t dot = rrj_s32(sub_8002E698(rrj_at(actor_normal, 6), rrj_at(normal, 6)));
                        if (leaf_abs32(dot) >= 11469)
                        {
                            flags |= 2;
                            if (!(flags & 0x20))
                            {
                                (void)sub_8002EAD8(rrj_at(actor + 184, 12), rrj_at(actor_normal, 6), (uint32_t)-motion_distance, rrj_at(probe_a, 12));
                                (void)sub_8002EAD8(rrj_at(actor + 184, 12), rrj_at(actor_normal, 6), (uint32_t)sub_8001FC90(46333, rrj_s32(rrj_read32(actor + 304) + rrj_read32(actor + 308))), rrj_at(response, 12));
                                if (state == 1)
                                    state = rrj_s32(sub_8002E698(rrj_at(actor_normal, 6), rrj_at(actor + 444, 6))) >= 0;
                                flags |= 0x20;
                            }
                        }
                    }
                }

                if (flags & 4)
                {
                    uint32_t order = rrj_read32(0x800CCAA0 + 4 * side_flags);
                    uint32_t index;
                    int32_t travel_distance = motion_distance;
                    for (index = 0; index < 8; ++index)
                    {
                        int32_t candidate;
                        wheel = linked_mode ? ((order >> (4 * index)) & 7) : ((7 - index) & 7);
                        (void)sub_8002EAD8(rrj_at(actor + 196 + 12 * wheel, 12), rrj_at(actor_normal, 6), (uint32_t)-travel_distance, rrj_at(probe_a, 12));
                        candidate = rrj_s32((uint32_t)sub_800B6BD0(probe_a, rrj_at(actor_normal, 6), rrj_at(normal, 6), edge_a));
                        if (candidate >= -4096 && candidate <= travel_distance + 4096)
                        {
                            (void)sub_8002EAD8(rrj_at(probe_a, 12), rrj_at(actor_normal, 6), (uint32_t)candidate, rrj_at(edge_b, 12));
                            contact_distance = -rrj_s32(sub_800B6AAC(rrj_at(actor + 196 + 12 * wheel, 12), rrj_at(normal, 6), rrj_at(edge_a, 12)));
                            if (contact_distance < 4096)
                                contact_distance = 4096;
                            if (linked_mode)
                            {
                                int32_t normal_dot = -rrj_s32(sub_8002E698(rrj_at(normal, 6), rrj_at(actor_normal, 6)));
                                if (normal_dot >= 132 && travel_distance >= 132)
                                {
                                    int32_t first = (int32_t)sub_8001FC90(rrj_s32(rrj_read32(0x800CCE38)), contact_distance);
                                    int32_t second = (int32_t)sub_8001FC90(normal_dot, travel_distance);
                                    uint32_t numerator = (uint32_t)(first < 0 ? -first : first);
                                    uint32_t denominator = (uint32_t)(second < 0 ? -second : second);
                                    int32_t ratio = denominator ? (int32_t)sub_80010028(numerator, denominator) : 0;
                                    if ((first < 0) != (second < 0))
                                        ratio = -ratio;
                                    surface_time = (uint32_t)ratio;
                                }
                                if (rrj_s32(surface_time) > rrj_s32(rrj_read32(0x800CCE38)))
                                    surface_time = rrj_read32(0x800CCE38);
                            }
                            break;
                        }
                    }
                    if (index >= 8)
                        wheel = 8;
                }
                else
                {
                    if (flags & 2)
                    {
                        int32_t first = rrj_s32(sub_800B6AAC(rrj_at(probe_a, 12), rrj_at(normal, 6), rrj_at(edge_a, 12)));
                        wheel = 8;
                        if ((first ^ rrj_s32(sub_800B6AAC(rrj_at(response, 12), rrj_at(normal, 6), rrj_at(edge_a, 12)))) < 0)
                        {
                            uint32_t index;
                            int32_t chosen = (int32_t)(((0u - (flags & 1)) & 0x80020000u) + 0x3FFF0000u);
                            for (index = 0; index < 8; ++index)
                            {
                                int select = state == 2 || ((index >> 1) & 1) == state;
                                if (select)
                                {
                                    int32_t candidate = rrj_s32((uint32_t)sub_800B6BD0(actor + 196 + 12 * index, rrj_at(actor_normal, 6), rrj_at(normal, 6), edge_a));
                                    if (((flags & 1) && chosen < candidate) || (!(flags & 1) && candidate < chosen))
                                    {
                                        wheel = index;
                                        chosen = candidate;
                                    }
                                }
                            }
                            contact_distance = chosen;
                            if (wheel < 8)
                                (void)sub_8002EAD8(rrj_at(actor + 196 + 12 * wheel, 12), rrj_at(actor_normal, 6), (uint32_t)chosen, rrj_at(edge_b, 12));
                        }
                    }
                    else
                    {
                        int32_t threshold = special ? 0x80000 : (3 * (rrj_s32(rrj_read32(actor + 308)) + rrj_s32(rrj_read32(actor + 304)) + rrj_s32(rrj_read32(actor + 312)) / 2)) / 4;
                        if (flags & 1)
                            race_collision_negate_normal(rrj_host_context(), normal);
                        wheel = sub_800B6F40(actor + 196, normal, edge_a, scratch + 120, scratch + 124);
                        if (rrj_read32(scratch + 124) == 8)
                        {
                            uint32_t identity = r_u16(actor + 172);
                            uint32_t player_count = rrj_read32(rrj_read32(0x8005B2F8) + 48);
                            if ((material & 0x800) && !(identity < player_count || ((r_u16(actor + 852 + 172) >> 5) == 1 && (identity & 31) < player_count && (rrj_read32(actor + 552) & 0x80000))))
                                wheel = 8;
                            if (threshold < rrj_s32(rrj_read32(scratch + 120)))
                                wheel = 8;
                            if ((material & 15) == 2 && group_count >= 2)
                                wheel = 8;
                        }
                        contact_distance = rrj_s32(rrj_read32(scratch + 120));
                        if (wheel < 8)
                            (void)sub_8002EAD8(rrj_at(actor + 196 + 12 * wheel, 12), rrj_at(normal, 6), (uint32_t)contact_distance, rrj_at(edge_b, 12));
                    }
                }

                rrj_write32(actor + 388, ((uint32_t)material << 20) | (rrj_read32(actor + 388) & 0x000FFFEFu));
                if (flags & 1)
                {
                    int32_t dx = rrj_s32(rrj_read32(edge_a) - rrj_read32(actor + 184));
                    int32_t dz = rrj_s32(rrj_read32(edge_a + 8) - rrj_read32(actor + 192));
                    int32_t distance = (int32_t)sub_8001FC90(dx, dx) + (int32_t)sub_8001FC90(dz, dz);
                    if (distance < best_distance)
                    {
                        best_surface = selected;
                        best_distance = distance;
                    }
                }
                if (wheel == 8)
                    goto reject_surface;

                {
                    int32_t tangent_x;
                    int32_t tangent_z;
                    int32_t first_dot;
                    int32_t second_dot;
                    int32_t front_limit = 2048;
                    uint32_t index;
                    if (flags & 0x10)
                    {
                        tangent_x = (int32_t)r_s16(selected + 32) << 4;
                        tangent_z = (int32_t)r_s16(selected + 36) << 4;
                    }
                    else
                    {
                        tangent_x = (int32_t)r_s16(normal + 4) << 4;
                        tangent_z = -(int32_t)r_s16(normal) << 4;
                        if (leaf_abs32((int16_t)r_u16(normal + 2)) >= 65)
                        {
                            int32_t length = (int32_t)sub_8001FC90(tangent_x, tangent_x) + (int32_t)sub_8001FC90(tangent_z, tangent_z);
                            int32_t root = rrj_s32(sub_8004CF74((uint32_t)length));
                            int32_t scale = root ? (int32_t)(0x80000000u / (uint32_t)(2 * root)) : 0;
                            tangent_x = (int32_t)sub_8001FC90(tangent_x, scale);
                            tangent_z = (int32_t)sub_8001FC90(tangent_z, scale);
                        }
                    }
                    first_dot = (int32_t)sub_8001FC90(tangent_x, rrj_s32(rrj_read32(edge_b) - rrj_read32(edge_a))) + (int32_t)sub_8001FC90(tangent_z, rrj_s32(rrj_read32(edge_b + 8) - rrj_read32(edge_a + 8)));
                    second_dot = (int32_t)sub_8001FC90(tangent_x, rrj_s32(rrj_read32(edge_a + 12) - rrj_read32(edge_b))) + (int32_t)sub_8001FC90(tangent_z, rrj_s32(rrj_read32(edge_a + 20) - rrj_read32(edge_b + 8)));
                    tangent_ratio = first_dot;
                    if (material & 0x400)
                        front_limit = ((int16_t)r_u16(actor + 482) / 16) * rrj_s32(rrj_read32(actor + 304)) + 2048;
                    if (special && (flags & 0x10000))
                    {
                        side_limit = rrj_s32(sub_800B6AAC(rrj_at(edge_b, 12), rrj_at((flags & 0x400) ? edge_cursor : last_surface + 32, 6), rrj_at((flags & 0x400) ? surface + 8 : last_surface + 8, 12)));
                        if (leaf_abs32(side_limit) <= 0x8000)
                            side_limit = 3 * rrj_s32(rrj_read32(actor + 308)) / 2 + 2048 + 2 * rrj_s32(rrj_read32(actor + 304));
                        else
                        {
                            side_limit = 2048;
                            front_limit = 3 * rrj_s32(rrj_read32(actor + 308)) / 2 + 2 * rrj_s32(rrj_read32(actor + 304)) + 2048;
                        }
                    }
                    else
                        side_limit = front_limit;
                    if ((flags & 0x10000) && front_limit < (int32_t)leaf_abs32(first_dot) && side_limit < (int32_t)leaf_abs32(second_dot) && (first_dot ^ second_dot) < 0)
                        goto reject_surface;

                    if ((material & 15) == 1 || (mode && (material & 0x400)))
                    {
                        int32_t height = rrj_s32(rrj_read32(edge_a + 4));
                        if ((r_u16(actor + 172) >> 5) < 3)
                            height = rrj_s32(rrj_read32(actor + 508));
                        if ((material & 15) == 1 && r_u16(selected + 38))
                        {
                            int32_t a = (int32_t)leaf_abs32(first_dot);
                            int32_t b = (int32_t)leaf_abs32(second_dot);
                            tangent_ratio = (a + b) ? (int32_t)sub_80010028((uint32_t)a, (uint32_t)(a + b)) : 0;
                            surface_offset = r_u16(selected + 38) == 1 ? 0x10000 - tangent_ratio : tangent_ratio;
                        }
                        for (index = 0; index < 8; ++index)
                            if (height - surface_offset < rrj_s32(rrj_read32(actor + 200 + 12 * index)))
                                break;
                        if (index >= 8)
                        {
                            int reject = 1;
                            uint32_t identity = r_u16(actor + 172);
                            uint32_t player_count = rrj_read32(rrj_read32(0x8005B2F8) + 48);
                            if ((identity >> 5) < 2 && (identity & 31) < player_count && (material & 0x200) && sub_800B6F40(actor + 196, normal, edge_a, 0, scratch + 124) != 8)
                            {
                                int32_t dot = rrj_s32(sub_8002E698(rrj_at(actor_normal, 6), rrj_at(normal, 6)));
                                if (leaf_abs32(dot) >= 22413)
                                {
                                    if (rrj_read32(scratch + 124) == 8)
                                    {
                                        uint32_t owner = linked_mode ? rrj_read32(actor + 852) : actor;
                                        uint32_t owner_identity = linked_mode ? r_u16(actor + 172) : r_u16(rrj_read32(actor + 596) + 172);
                                        uint32_t record = 0x800CD898 + 1132 * owner_identity;
                                        rrj_write32(record + 788, 0xFFFF0000);
                                        rrj_write32(record + 548, rrj_read32(record + 548) | 0x40000);
                                        rrj_write32(owner + 552, rrj_read32(owner + 552) | 0x80000);
                                    }
                                }
                                else
                                {
                                    reject = 0;
                                    if (dot > 0)
                                        race_collision_negate_normal(rrj_host_context(), normal);
                                    flags &= ~1u;
                                }
                            }
                            if (reject)
                                goto reject_surface;
                        }
                    }

                    if (flags & 2)
                    {
                        tangent_ratio = (int32_t)(((0u - (flags & 1)) & 0xFFFFC000u) + 0x2000u);
                        (void)sub_8002ECB8(rrj_at(actor_normal, 6), rrj_at(normal, 6), rrj_at(response, 12), (uint32_t)(contact_distance - motion_distance - 4096), (uint32_t)tangent_ratio);
                        if (flags & 1)
                            race_collision_negate_normal(rrj_host_context(), normal);
                    }
                    else
                    {
                        if ((flags & 5) == 5)
                            race_collision_negate_normal(rrj_host_context(), normal);
                        if ((r_u16(actor + 172) >> 5) != 1 && !(flags & 4))
                            contact_distance += 0x2000;
                        if ((material & 0x400) && !(flags & 1))
                            contact_distance += ((int16_t)r_u16(actor + 482) / 16) << 13;
                        (void)sub_8002EE50((uint32_t)contact_distance, rrj_at(normal, 6), rrj_at(response, 12));
                    }

                    if (linked_mode)
                    {
                        uint32_t owner = tracked;
                        uint32_t maximum;
                        if (!rrj_read32(tracked + 1088))
                        {
                            tracked = rrj_read32(tracked + 856);
                            owner = tracked;
                        }
                        maximum = surface_time < rrj_read32(owner + 552) ? rrj_read32(owner + 552) : surface_time;
                        rrj_write32(owner + 552, maximum);
                        if ((rrj_read32(actor + 320) & 4) && rrj_read32(0x800CCF68) < 8)
                        {
                            uint32_t count = rrj_read32(0x800CCF68);
                            uint32_t record = 0x800CCE48 + 36 * count;
                            rrj_put16(rrj_at(record, 2), r_u16(owner + 172));
                            rrj_put16(rrj_at(record + 2, 2), r_u16(selected));
                            rrj_write32(record + 4, rrj_read32(normal));
                            rrj_put16(rrj_at(record + 8, 2), r_u16(normal + 4));
                            rrj_put16(rrj_at(record + 10, 2), (uint16_t)material);
                            rrj_write32(record + 12, selected);
                            rrj_write32(record + 16, rrj_read32(response));
                            rrj_write32(record + 20, rrj_read32(response + 4));
                            rrj_write32(record + 24, rrj_read32(response + 8));
                            rrj_write32(record + 28, (uint32_t)surface_offset);
                            rrj_write32(record + 32, (uint32_t)contact_distance);
                            if (linked_mode < 2)
                                rrj_write32(0x800CCF68, count + 1);
                            else
                                rrj_write32(0x800CCF68, count + leaf_call(rrj_host_context(), call, 0x800B59F0, record, 0, 0, 0));
                        }
                        else
                            (void)(uint32_t)sub_800B12A0(tracked, normal, material, (uint32_t)surface_offset, selected, response);
                    }
                    else if (special)
                    {
                        if (flags & 0x20000)
                        {
                            int32_t chosen = tangent_ratio;
                            int32_t cross = (int32_t)r_s16(surface - step + 32) * (int16_t)(tangent_x >> 4) / 4 + (int32_t)r_s16(surface - step + 36) * (int16_t)(tangent_z >> 4) / 4;
                            int32_t segment = (int32_t)sub_8001FC90(rrj_s32(rrj_read32(edge_b) - rrj_read32(edge_a)), tangent_x) + (int32_t)sub_8001FC90(rrj_s32(rrj_read32(edge_b + 8) - rrj_read32(edge_a + 8)), tangent_z);
                            int32_t impulse;
                            if ((cross ^ segment) >= 0)
                                chosen = second_dot;
                            impulse = (int32_t)leaf_abs32(chosen) + 0x4000;
                            if (cross < 0)
                                impulse = -impulse;
                            rrj_write32(response, rrj_read32(response) + (uint32_t)sub_8001FC90(tangent_x, impulse));
                            rrj_write32(response + 8, rrj_read32(response + 8) + (uint32_t)sub_8001FC90(tangent_z, impulse));
                        }
                        (void)(uint32_t)sub_800A8DF0(actor, rrj_at(response, 12), 0);
                        if (!rrj_read32(special_actor + 768))
                            rrj_write32(special_actor + 768, selected);
                    }
                    else
                    {
                        uint32_t identity = r_u16(actor + 172);
                        uint32_t actor_type = identity >> 5;
                        (void)(uint32_t)sub_800A8DF0(actor, rrj_at(response, 12), 1);
                        if (actor_type == 2)
                        {
                            uint32_t object = rrj_read32(0x800D4B80) + 572 * (identity & 31);
                            (void)race_collision_call6(rrj_host_context(), call, 0x800A9868, object, normal, 0, normal, object + 172, 0);
                        }
                        else if (actor_type == 4)
                            (void)(uint32_t)sub_800B3344(rrj_read32(0x800CD6D4) + 596 * (identity & 31), 0, normal, 0xFFFFFFFF);
                        else if (actor_type == 1)
                            (void)leaf_call(rrj_host_context(), call, 0x800B2794, rrj_read32(0x8005B3A4) + 628 * (identity & 31), normal, material, 0);
                    }
                    result = wheel | 0x100;
                }

                if (!special)
                {
                    rrj_write32(actor + 388, rrj_read32(actor + 388) | (16 * (flags & 1)));
                    return race_collision_return(rrj_host_context(), saved, result);
                }

            reject_surface:
                ++surface_index;
                if (!(flags & 0x18))
                {
                    edge_cursor += step;
                    surface += step;
                }
                continue;
            }

        outer_next:
            continue;
        }
    }

    if (!result && best_surface)
    {
        int32_t first;
        int32_t second;
        rrj_write32(actor + 388, rrj_read32(actor + 388) | 0x10);
        uint32_t basis = rrj_read32(actor + 340);
        first = rrj_s32(sub_800B6AAC(rrj_at(best_surface + 8, 12), rrj_at(basis + 2, 6), rrj_at(basis + 20, 12)));
        second = rrj_s32(sub_800B6AAC(rrj_at(best_surface + 20, 12), rrj_at(basis + 2, 6), rrj_at(basis + 20, 12)));
        best_distance = (int32_t)(leaf_abs32(first) < leaf_abs32(second) ? leaf_abs32(first) : leaf_abs32(second));
        rrj_write32(actor + 388, ((uint32_t)((int16_t)(best_distance >> 13)) << 8) | (rrj_read32(actor + 388) & 0xFFF000FFu));
    }
    return race_collision_return(rrj_host_context(), saved, result);
}

uint32_t sub_800A8FE8(uint32_t actor)
{
    const uint32_t vector = 0x1F8000E0;
    uint8_t saved[16];
    uint32_t flags = rrj_read32(actor + 568);
    uint32_t linked;
    int32_t length;
    int32_t root;
    int32_t delta;
    int32_t reciprocal;
    uint32_t axis;

    FUNCTION_MARKER(0x800A8FE8, "RASHCDG.BIN");
    if (!(flags & 0x01000000))
        return 0;
    memcpy(saved, rrj_at(vector, sizeof(saved)), sizeof(saved));
    rrj_write32(actor + 568, flags & 0xFEFFFFFFu);
    for (axis = 0; axis < 3; ++axis)
        rrj_write32(vector + 4 * axis, rrj_read32(actor + 184 + 4 * axis) - rrj_read32(actor + 468 + 4 * axis));
    length = rrj_s32(sub_8002F0F4(vector));
    if (length < 132)
    {
        xport_guest_copy(xport_guest_ref(vector), xport_host_ref((void *)saved), sizeof(saved));
        return 0;
    }
    flags = rrj_read32(actor + 568);
    if (!(flags & 0x02000000))
    {
        rrj_put16(rrj_at(actor + 864, 2), r_u16(actor + 450));
        rrj_put16(rrj_at(actor + 866, 2), r_u16(actor + 452));
        rrj_put16(rrj_at(actor + 868, 2), r_u16(actor + 454));
        rrj_write32(actor + 860, rrj_read32(actor + 480));
        rrj_write32(actor + 568, flags | 0x02000000);
    }
    root = rrj_s32(sub_8004CF74((uint32_t)length) << 2);
    delta = rrj_s32(rrj_read32(0x800CCE38));
    rrj_write32(actor + 480, (uint32_t)race_leaf_signed_ratio(root, delta));
    if (root < 0)
    {
        uint32_t magnitude = (uint32_t)-root;
        uint32_t denominator = (magnitude >> 1) + (uint32_t)(((int32_t)magnitude - 2) >> 31);
        reciprocal = denominator ? -(int32_t)(0x80000000u / denominator) : 0;
    }
    else
    {
        uint32_t magnitude = (uint32_t)root;
        uint32_t denominator = (magnitude >> 1) + (uint32_t)(((int32_t)magnitude - 2) >> 31);
        reciprocal = denominator ? (int32_t)(0x80000000u / denominator) : 0;
    }
    (void)sub_8002EED8((uint32_t)reciprocal, vector, actor + 450);
    linked = rrj_read32(actor + 856);
    xport_guest_copy(xport_guest_ref(vector), xport_host_ref((void *)saved), sizeof(saved));
    if (linked && rrj_read32(actor + 1088))
    {
        rrj_write32(actor + 568, rrj_read32(actor + 568) | 0x01000000);
        (void)sub_800A8FE8(linked);
    }
    return 1;
}

uint32_t sub_800A8C78(uint32_t actor, int32_t lateral_limit, int32_t distance_limit)
{
    uint16_t state = r_u16(actor + 320);
    uint32_t player_count;
    uint32_t index;
    uint32_t result = 0;

    FUNCTION_MARKER(0x800A8C78, "RASHCDG.BIN");
    if (state & 8)
        return (state >> 2) & 1;
    rrj_write32(actor + 48, 0x7FFFFFFF);
    player_count = rrj_read32(rrj_read32(0x8005B2F8) + 48);
    for (index = 0; index < player_count; ++index)
    {
        uint32_t origin = 0x800CD950 + 1132 * index;
        int32_t distance = rrj_s32(sub_800B6AAC(rrj_at(actor + 184, 12), rrj_at(0x800CDA54 + 1132 * index, 6), rrj_at(origin, 12)));
        if (distance > 0x8000 && distance < distance_limit)
        {
            distance = rrj_s32(sub_800B6AAC(rrj_at(actor + 184, 12), rrj_at(0x800CDA48 + 1132 * index, 6), rrj_at(origin, 12)));
            if ((int32_t)leaf_abs32(distance) < lateral_limit)
                result = 1;
        }
        rrj_write32(actor + 44 + 4 * index, (uint32_t)(distance >> 10));
    }
    rrj_put16(rrj_at(actor + 320, 2), (uint16_t)(state | 8 | (4 * result)));
    return result;
}

uint32_t sub_800B6F40(uint32_t points, uint32_t normal, uint32_t origin, uint32_t distance_out, uint32_t count_out)
{
    uint32_t selected = 8;
    uint32_t negative_count = 0;
    uint32_t index;
    int32_t minimum = 0;

    FUNCTION_MARKER(0x800B6F40, "RASHCDG.BIN");
    for (index = 0; index < 8; ++index)
    {
        int32_t distance = rrj_s32(sub_800B6AAC(rrj_at(points + 12 * index, 12), rrj_at(normal, 6), rrj_at(origin, 12)));
        if (distance < 0)
        {
            ++negative_count;
            if (distance < minimum)
            {
                minimum = distance;
                selected = index;
            }
        }
    }
    if (count_out)
        rrj_write32(count_out, negative_count);
    if (selected != 8 && distance_out)
        rrj_write32(distance_out, (uint32_t)-minimum);
    return selected;
}

static int32_t race_power_curve(RRJMemory *m, uint32_t specification, int32_t speed)
{
    int32_t low = rrj_s32(rrj_read32(specification + 176));
    int32_t high = rrj_s32(rrj_read32(specification + 180));
    int32_t interval = rrj_s32(rrj_read32(specification + 184));
    int32_t offset;
    int32_t index;
    int32_t fraction;
    int32_t first;
    int32_t second;

    if (speed >= high)
        return 0;
    offset = speed - low;
    if (offset < 6553)
        return rrj_s32(rrj_read32(specification + 60));
    index = interval ? offset / interval : 0;
    fraction = interval ? (int32_t)sub_80010028((uint32_t)offset, (uint32_t)interval) - (index << 16) : 0;
    first = rrj_s32(rrj_read32(specification + 60 + 4 * index));
    if (fraction < 65)
        return first;
    second = rrj_s32(rrj_read32(specification + 64 + 4 * index));
    if (second - first < 6)
        return second;
    return first + (int32_t)(sub_8001FC90(fraction, (second - first) << 10) / 1024);
}

uint32_t sub_80079B20(uint32_t list, int32_t delta, RRJRaceLeafCall call)
{
    uint32_t node = rrj_read32(list + 4);

    FUNCTION_MARKER(0x80079B20, "RASHCDG.BIN");
    while (node != list)
    {
        uint32_t actor = node - 1088;
        uint32_t specification = rrj_read32(actor + 556);
        int32_t gear = r_s8(actor + 849);
        int32_t ratio = rrj_s32(rrj_read32(specification + 20 + 4 * gear));
        uint32_t state = rrj_read32(actor + 564);
        uint32_t flags = rrj_read32(actor + 560);
        int32_t speed = rrj_s32(rrj_read32(actor + 576));
        int32_t motion = rrj_s32(rrj_read32(actor + 620));
        int32_t acceleration = rrj_s32(rrj_read32(actor + 624));
        int32_t curve;
        int32_t power;

        if (speed < 13107 || (flags & 0x42) != 0x42 || (rrj_read32(actor + 568) & 0x7FF) || rrj_read32(rrj_read32(actor + 852) + 604) >= 2 || (state & 0x40000) || motion || acceleration || (flags & 0x08000000))
        {
            state &= 0xF7FFFFFF;
            if ((rrj_read32(actor + 564) & 0x08000000) && ((!motion && !acceleration) || rrj_s32(rrj_read32(actor + 616)) >= 0))
            {
                if (flags & 2)
                    state |= 0x10000000;
                rrj_write32(actor + 700, 0);
                rrj_write32(actor + 616, 0);
            }
            rrj_write32(actor + 564, state);
        }
        else
        {
            (void)leaf_call(rrj_host_context(), call, 0x80075BE4, actor, (uint32_t)delta, 0, 0);
            state = rrj_read32(actor + 564);
        }
        if (!(state & 0x08000000))
        {
            if (rrj_s32(rrj_read32(actor + 604)) >= rrj_s32(rrj_read32(specification + 180)))
            {
                flags |= 0x00800000;
                rrj_write32(actor + 560, flags);
            }
            if (!(flags & 0x00800000) && !(flags & 2))
            {
                state &= 0xEFFFFFFF;
                rrj_write32(actor + 564, state);
            }
            if (!(state & 1))
            {
                rrj_write32(actor + 604, (uint32_t)sub_8001FC90(speed, ratio));
                state &= 0xEFFFFFFE;
                rrj_write32(actor + 564, state);
            }
        }
        curve = race_power_curve(rrj_host_context(), specification, rrj_s32(rrj_read32(actor + 604)));
        power = (int32_t)sub_8001FC90(ratio, curve);
        rrj_write32(actor + 592, (uint32_t)power);
        if (state & 0x200)
            rrj_write32(actor + 592, (uint32_t)sub_8001FC90(rrj_s32(rrj_read32(specification + 336)), power));
        if (!(rrj_read32(actor + 568) & 0x600))
        {
            if (!motion && !acceleration)
            {
                rrj_write32(actor + 624, 0);
                state &= 0xFFFFF7FF;
                rrj_write32(actor + 564, state);
                rrj_write32(actor + 480, rrj_read32(actor + 576));
            }
        }
        if (r_s8(actor + 8) < 2)
        {
            int32_t target = rrj_s32(sub_8001FF3C((uint32_t)((int32_t)r_s16(actor + 530) << 4)));
            int32_t current = r_s16(actor + 842);
            int32_t change = (int32_t)sub_8001FC90((target - current) << 16, 0x79999);
            change = (int32_t)sub_8001FC90(change, delta) >> 16;
            rrj_put16(rrj_at(actor + 842, 2), (uint16_t)(current + change));
        }
        node = rrj_read32(node + 4);
    }
    return 0;
}

uint32_t sub_80078DB4(uint32_t list)
{
    uint32_t node = rrj_read32(list + 4);

    FUNCTION_MARKER(0x80078DB4, "RASHCDG.BIN");
    while (node != list)
    {
        uint32_t actor = node - 1088;
        uint32_t flags = rrj_read32(actor + 568);
        int32_t state = rrj_s32(rrj_read32(actor + 364));
        if ((state < 0 ? -state : state) >= 3)
            flags ^= 0x00400000;
        rrj_write32(actor + 568, flags);
        node = rrj_read32(node + 4);
    }
    return 0;
}

static int32_t race_slip_ratio(uint32_t magnitude, uint32_t low, uint32_t high)
{
    uint32_t distance;
    uint32_t span;

    if (magnitude < low)
        return 0;
    distance = magnitude - low;
    if (magnitude >= high)
        return 0x10000;
    span = high - low;
    if (rrj_s32(distance) <= 0)
    {
        distance = low - magnitude;
        if (rrj_s32(span) <= 0)
            return rrj_s32(sub_80010028(distance, low - high));
        return -rrj_s32(sub_80010028(distance, span));
    }
    if (rrj_s32(span) > 0)
        return rrj_s32(sub_80010028(distance, span));
    return -rrj_s32(sub_80010028(distance, low - high));
}

uint32_t sub_8007AC04(uint32_t list)
{
    uint32_t node = rrj_read32(list + 4);
    uint32_t result = 0x80050000;

    FUNCTION_MARKER(0x8007AC04, "RASHCDG.BIN");
    while (node != list)
    {
        uint32_t actor = node - 1088;
        uint32_t state = rrj_read32(actor + 564);
        uint32_t flags = rrj_read32(actor + 568);
        int32_t sine = 0;
        int32_t cosine = 0x10000;
        int32_t angle;
        int32_t heading;
        int32_t longitudinal;
        int32_t lateral;
        int32_t pitch;
        uint32_t specification;
        uint16_t angles[3];
        uint16_t matrix[9];

        if (state & 0x3000)
        {
            rrj_write32(actor + 676, 0);
            rrj_write32(actor + 828, 0);
        }
        if (rrj_read32(actor + 676))
        {
            uint32_t index = ((uint32_t)(163u * rrj_read32(actor + 676)) >> 14) & 0xFFF;
            sine = (int32_t)r_s16(0x8005624C + 4 * index) << 4;
            cosine = (int32_t)r_s16(0x8005624E + 4 * index) << 4;
        }
        state = rrj_read32(actor + 564);
        flags = rrj_read32(actor + 568);
        {
            uint32_t constrained = (flags & 0x1F) || (state & 0x3400);
            uint32_t moving = (!(flags & 0x600) && rrj_read32(actor + 576)) || (state & 0x40000) || rrj_read32(actor + 488) || rrj_read32(actor + 676);
            if ((moving && !constrained) || (constrained && !(flags & 15)) || (flags & 0x4000))
            {
                if (sine || cosine < 0)
                {
                    (void)sub_8002EB78(actor + 528, actor + 516, actor + 450, (uint32_t)cosine, 0u - (uint32_t)sine);
                    if (!sub_8002E468(actor + 450))
                        (void)sub_8008CF74(actor);
                    leaf_direction_op(rrj_host_context(), actor + 522, actor + 450, actor + 814);
                }
                else
                {
                    unsigned i;
                    for (i = 0; i < 3; ++i)
                    {
                        rrj_put16(rrj_at(actor + 450 + 2 * i, 2), rrj_u16(rrj_at(actor + 528 + 2 * i, 2)));
                        rrj_put16(rrj_at(actor + 814 + 2 * i, 2), rrj_u16(rrj_at(actor + 516 + 2 * i, 2)));
                    }
                    rrj_write32(actor + 564, state & ~0x400u);
                }
            }
        }
        rrj_write32(actor + 732, (uint32_t)(157 * (int32_t)r_s16(actor + 518)));
        rrj_write32(actor + 736, (uint32_t)(157 * (int32_t)r_s16(actor + 524)));
        rrj_write32(actor + 740, (uint32_t)(157 * (int32_t)r_s16(actor + 530)));
        longitudinal = rrj_s32(rrj_read32(actor + 740));
        if (rrj_read32(actor + 676))
        {
            longitudinal = (int32_t)(((int64_t)longitudinal * cosine) >> 16) - (int32_t)(((int64_t)rrj_s32(rrj_read32(actor + 732)) * sine) >> 16);
        }
        rrj_write32(actor + 760, (uint32_t)longitudinal);
        angle = rrj_s32(sub_80020018(rrj_read32(actor + 732), rrj_read32(actor + 736)));
        heading = -leaf_mul_asr(25736, angle, 8);
        rrj_write32(actor + 668, (uint32_t)heading);
        specification = rrj_read32(actor + 556);
        flags = rrj_read32(actor + 568);
        if (flags & 0x14C)
        {
            int32_t value = rrj_s32(rrj_read32(actor + 652));
            int32_t ratio = 0;
            int32_t correction = 0;
            if (rrj_read32(actor + 488) && (rrj_read32(actor + 488) ^ (uint32_t)value) < 0x80000000u)
            {
                uint32_t magnitude = leaf_abs32(value);
                ratio = race_slip_ratio(magnitude, rrj_read32(specification + 296), rrj_read32(specification + 300));
                correction = leaf_product_asr16(ratio, rrj_s32(rrj_read32(specification + 236)));
                value = value > 0 ? value + correction : value - correction;
            }
            rrj_write32(actor + 672, (uint32_t)ratio);
            rrj_write32(actor + 636, (uint32_t)(value - heading));
        }
        else
        {
            int32_t value = rrj_s32(rrj_read32(actor + 636)) + heading;
            int32_t ratio = 0;
            int32_t correction = 0;
            if (rrj_read32(actor + 488) && (rrj_read32(actor + 488) ^ (uint32_t)value) < 0x80000000u)
            {
                uint32_t low = rrj_read32(specification + 296);
                uint32_t high = rrj_read32(specification + 300) + rrj_read32(specification + 236);
                ratio = race_slip_ratio(leaf_abs32(value), low, high);
                correction = leaf_product_asr16(ratio, rrj_s32(rrj_read32(specification + 236)));
            }
            rrj_write32(actor + 672, (uint32_t)ratio);
            value = value <= 0 ? value + correction : value - correction;
            rrj_write32(actor + 652, (uint32_t)value);
        }
        pitch = leaf_mul_asr(652, rrj_s32(rrj_read32(actor + 616)), 16);
        flags = rrj_read32(actor + 568);
        if (!rrj_read32(actor + 856) || (flags & 0xEC))
            lateral = rrj_s32(rrj_read32(actor + 652));
        else if (flags & 0x100)
            lateral = rrj_s32(rrj_read32(actor + 652)) > 0 ? 183010 : -102943;
        else
        {
            int32_t first = rrj_s32(rrj_read32(actor + 636));
            int32_t second = rrj_s32(rrj_read32(actor + 652));
            uint32_t mask = first < 0 ? 1u : 0u;
            lateral = rrj_s32(mask & (uint32_t)first);
            if (first < second)
                lateral = rrj_s32(mask & (uint32_t)second);
        }
        if (r_u16(actor + 172) >= rrj_read32(rrj_read32(0x8005B2F8) + 48) || rrj_read32(0x8005B220))
        {
            int32_t target = lateral - heading;
            int32_t filtered;
            if (flags & 0x7FF)
                filtered = (rrj_s32(rrj_read32(actor + 648)) + 3 * target) / 4;
            else if (!rrj_read32(actor + 660) && r_s16(actor + 944))
                filtered = (3 * rrj_s32(rrj_read32(actor + 648)) + target) / 4;
            else
                filtered = (rrj_s32(rrj_read32(actor + 648)) + target) / 2;
            rrj_write32(actor + 648, (uint32_t)filtered);
            lateral = filtered + heading;
        }
        angles[0] = (uint16_t)-pitch;
        angles[1] = 0;
        angles[2] = (uint16_t)-leaf_mul_asr(652, lateral, 16);
        rrj_euler_rotation_local(rrj_host_context(), angles, matrix);
        state = rrj_read32(actor + 564);
        if ((state & 0x200000) || ((flags & 0x100) && rrj_read32(actor + 772)))
        {
            (void)sub_8002EAD8(rrj_at(actor + 504, 12), rrj_at(actor + 522, 6), rrj_read32(actor + 772), rrj_at(actor + 184, 12));
        }
        else if (pitch)
        {
            int32_t scale = rrj_s32(rrj_read32(actor + 308));
            uint32_t index = (uint32_t)pitch & 0xFFF;
            int32_t first;
            int32_t second;
            unsigned i;
            if (pitch > 0)
                scale = -scale;
            scale = (3 * scale) / 4;
            first = rrj_s32((uint32_t)sub_8001FC90(scale, (int32_t)r_s16(0x8005624C + 4 * index) << 4));
            second = rrj_s32((uint32_t)sub_8001FC90(scale, 0x10000 - ((int32_t)r_s16(0x8005624E + 4 * index) << 4)));
            for (i = 0; i < 3; ++i)
            {
                int32_t position = rrj_s32(rrj_read32(actor + 504 + 4 * i));
                position += (int32_t)(((int64_t)((int32_t)r_s16(actor + 438 + 2 * i) << 4) * first) >> 16);
                position += (int32_t)(((int64_t)((int32_t)r_s16(actor + 528 + 2 * i) << 4) * second) >> 16);
                rrj_write32(actor + 184 + 4 * i, (uint32_t)position);
            }
        }
        else
        {
            rrj_write32(actor + 184, rrj_read32(actor + 504));
            rrj_write32(actor + 188, rrj_read32(actor + 508));
            rrj_write32(actor + 192, rrj_read32(actor + 512));
        }
        rrj_write32(actor + 580, (uint32_t)leaf_product_asr16(rrj_s32(rrj_read32(actor + 576)), rrj_s32(rrj_read32(actor + 576))));
        (void)rrj_multiply_rotation_local_left(rrj_host_context(), matrix, actor + 516, actor + 432);
        result = actor + 436;
        node = rrj_read32(node + 4);
    }
    return result;
}

uint32_t sub_80095724(uint32_t actor, uint32_t delta, RRJRaceLeafCall call)
{
    uint32_t descriptor;
    uint32_t target;

    FUNCTION_MARKER(0x80095724, "RASHCDG.BIN");
    if (!actor)
        return 0;
    if (r_s16(actor + 320))
    {
        if (!rrj_read32(0x800CCA80))
            rrj_write32(0x800CCA80, 1);
        return rrj_read32(0x800CCA80);
    }
    rrj_write32(actor + 568, rrj_read32(actor + 568) & ~0x800u);
    if (r_s16(0x800D6182) == -1)
        return 0;
    descriptor = rrj_read32(actor + 1084);
    if (((r_u8(descriptor + 1) & 15) == 2 && !(r_u8(actor + 928) & 0x10)) || (r_u8(descriptor) & 0x10) || rrj_read32(descriptor + 40) || rrj_read32(rrj_read32(actor + 852) + 604) >= 3)
        return 0;
    target = sub_80095BF8(actor, delta);
    rrj_write32(actor + 924, target);
    rrj_write32(actor + 480, target);
    (void)leaf_call(rrj_host_context(), call, 0x80097518, actor, (uint32_t)sub_8001FC90((int32_t)delta, rrj_s32(target)), 0, 0);
    target = sub_8003B61C(actor + 172);
    rrj_write32(actor + 324, target);
    return target;
}

uint32_t sub_800A2A64(int32_t delta, RRJRaceLeafCall call)
{
    uint32_t count_pointer = rrj_read32(0x800CE51C);

    FUNCTION_MARKER(0x800A2A64, "RASHCDG.BIN");
    if (rrj_s32(rrj_read32(count_pointer)) < 0)
        return count_pointer;
    return leaf_call(rrj_host_context(), call, 0x800A2ABC, (uint32_t)delta, 0, 0, 0);
}

uint32_t sub_8007B840(int32_t delta, RRJRaceLeafCall call)
{
    uint32_t actor = rrj_read32(0x800CE4D0);
    int32_t remaining = rrj_s32(rrj_read32(rrj_read32(0x800CE4DC)));

    FUNCTION_MARKER(0x8007B840, "RASHCDG.BIN");
    while (remaining >= 0)
    {
        uint32_t flags = rrj_read32(actor + 568);
        if (flags & 0x02000000)
        {
            rrj_put16(rrj_at(actor + 450, 2), r_u16(actor + 864));
            rrj_put16(rrj_at(actor + 452, 2), r_u16(actor + 866));
            rrj_put16(rrj_at(actor + 454, 2), r_u16(actor + 868));
            rrj_write32(actor + 480, rrj_read32(actor + 860));
        }
        if (flags & 0x08001800)
            (void)leaf_call(rrj_host_context(), call, 0x80071BCC, actor, (uint32_t)delta, 0, 0);
        if (r_u16(actor + 320))
        {
            uint32_t state = rrj_read32(actor + 564);
            if ((state & 0x18000) == 0x8000)
            {
                uint32_t linked = rrj_read32(actor + 832);
                uint32_t preserve = (flags >> 10) & 1;
                if (linked)
                {
                    if ((state & 0x20000) && !preserve)
                        (void)(uint32_t)sub_80084BE8(actor, 1);
                    (void)sub_8002076C(actor);
                    if (preserve)
                        rrj_write32(actor + 828, linked);
                    else
                        rrj_write32(actor + 832, 0);
                }
                rrj_write32(actor + 744, 0);
                rrj_write32(actor + 772, 0);
                rrj_write32(actor + 616, 0);
                state &= 0xFFD97FFF;
                rrj_write32(actor + 564, state);
            }
            if (flags & 0x00800000)
            {
                (void)sub_800374D4(actor, ((rrj_read32(actor + 560) >> 27) ^ 1) & 1);
                (void)sub_8007504C(actor, actor + 184);
            }
            rrj_write32(actor + 564, rrj_read32(actor + 564) & ~0x10000u);
        }
        actor += rrj_read32(0x800CE4D4);
        --remaining;
    }
    (void)sub_80079B20(0x8005B2D8, delta, call);
    (void)sub_80079B20(0x8005B298, delta, call);
    (void)sub_80079B20(0x8005B350, delta, call);
    (void)sub_80078DB4(0x8005B350);
    (void)sub_80078DB4(0x8005B2D8);
    actor = rrj_read32(0x800CE4D0);
    remaining = rrj_s32(rrj_read32(rrj_read32(0x800CE4DC)));
    while (remaining >= 0)
    {
        uint32_t body = rrj_read32(actor + 852);
        if (rrj_read32(actor + 568) & 0x08001800)
            (void)leaf_call(rrj_host_context(), call, 0x80071BCC, actor, (uint32_t)delta, 0, 0);
        if (rrj_read32(body + 604) >= 2 && r_u16(body + 320))
            (void)leaf_call(rrj_host_context(), call, 0x8008F404, actor, (uint32_t)delta, 0, 0);
        if (r_u8(body + 572) & 0x10)
        {
            uint32_t linked = rrj_read32(actor + 856);
            uint32_t linked_body = rrj_read32(linked + 852);
            if (rrj_read32(linked_body + 604) >= 2 && r_u16(linked_body + 320))
                (void)leaf_call(rrj_host_context(), call, 0x8008F404, linked, (uint32_t)delta, 0, 0);
        }
        actor += rrj_read32(0x800CE4D4);
        --remaining;
    }
    (void)sub_8007AC04(0x8005B298);
    (void)sub_8007AC04(0x8005B350);
    {
        uint32_t list = 0x8005B270;
        uint32_t node = rrj_read32(list + 4);
        while (node != list)
        {
            (void)sub_80095724(node - 1088, (uint32_t)delta, call);
            node = rrj_read32(node + 4);
        }
    }
    return 0;
}
