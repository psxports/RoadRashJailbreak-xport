#include "rrj_debug.h"
#include "rrj_data.h"
#include "rrj_cd_transport.h"
#include "race_frame.h"
#include "race_global.h"
#include "race_bodyless_batch_002.h"
#include "native_file_callbacks.h"
#include "music_stop.h"
#include "psx.h"
#include "gpu.h"
#include "screen.h"
#include "draft_quick.h"
#include <stdio.h>
#include "native_game_api.h"
#include "game_main.h"
#include "race_leaf_batch_004.h"
#include "race_bodyless_batch_008.h"
/* Small leaf translations shared by the race dependency branch */
#include "race_leaf.h"
#include "solo_file_dependencies.h"
#include "race_trace_runtime.h"
#include "audio.h"
#include "audio_flush.h"
#include "fixed_math.h"
#include "menu_latch.h"
#include "pad.h"
#include "race_pause.h"
#include "spu.h"
#include "sdk_build_stubs.h"
#include "psx_spu.h"
#include "wip.h"
#include "xport.h"
#include <stdlib.h>
#include <string.h>

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

uint32_t sub_80018260(void)
{
    uint32_t index = r_u32(0x8005B3E8) - 1;
    uint32_t shifted, product;
    FUNCTION_MARKER(0x80018260, "SLUS_010.53");
    w_u32(0x8005B3E8, index);
    if (((int32_t)(uint32_t)(index)) < 0)
    {
        index = 2;
        w_u32(0x8005B3E8, index);
    }
    index = r_u32(0x8005B3E8);
    shifted = (uint32_t)(((int32_t)(uint32_t)(index << 3)) >> 1);
    product = shifted * r_u32(0x80052640);
    product = (uint32_t)(((int32_t)(uint32_t)(product)) >> 4);
    w_u32(0x800D6C08, product);
    return product;
}

uint32_t sub_8001DD08(uint32_t index, int32_t delay, uint32_t value)
{
    uint32_t record = 0x800D7428 + 24 * index;
    uint32_t result = r_u32(record + 8);
    FUNCTION_MARKER(0x8001DD08, "SLUS_010.53");
    if (result && value)
    {
        result = 0xFFFFFFFF;
        w_u8(record + 13, (uint8_t)value);
        if (delay == -1)
            w_u32(record + 20, 0);
        else
        {
            result = r_u32(r_u32(0x8005B2F8) + 12) + (uint32_t)delay;
            w_u32(record + 20, result);
        }
    }
    else
    {
        w_u8(record + 13, 0);
        w_u32(record + 20, 0);
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
    uint32_t result = r_u32(record + 8);
    FUNCTION_MARKER(0x8001DC94, "SLUS_010.53");
    if (result && delay >= 0)
    {
        w_u8(record + 12, 1);
        if (delay == -1)
            w_u32(record + 16, 0);
        else
        {
            result = r_u32(r_u32(0x8005B2F8) + 12) + (uint32_t)delay;
            w_u32(record + 16, result);
        }
    }
    else
    {
        w_u8(record + 12, 0);
        w_u32(record + 16, 0);
    }
    return result;
}

uint32_t sub_8001F934(int32_t group, uint32_t sample, uint32_t field)
{
    uint32_t base;
    uint32_t record;
    FUNCTION_MARKER(0x8001F934, "SLUS_010.53");
    if (group < 0 || ((int32_t)(uint32_t)(r_u32(0x800D6870))) < group)
        return 0;
    base = r_u32(r_u32(0x800D6874) + 4 * (uint32_t)group);
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
    uint32_t x_sign = (uint32_t)(x >> 31);
    uint32_t z_sign = (uint32_t)(z >> 31);
    int32_t ax = ((int32_t)(uint32_t)(((uint32_t)x + x_sign) ^ x_sign));
    int32_t az = ((int32_t)(uint32_t)(((uint32_t)z + z_sign) ^ z_sign));
    int32_t high = ax >= az ? ax : az;
    int32_t low = ax >= az ? az : ax;
    int32_t mixed = ((int32_t)(uint32_t)((uint32_t)low + (uint32_t)(low >> 1)));
    return ((int32_t)(uint32_t)((uint32_t)high - (uint32_t)(high >> 5) - (uint32_t)(high >> 7) + (uint32_t)(mixed >> 2) + (uint32_t)(mixed >> 6)));
}

int32_t sub_80019E40(uint32_t index, int32_t x, int32_t z, int32_t other_x, int32_t other_z, uint32_t *level_out, uint32_t *pan_out, uint32_t *ratio_out, uint32_t shift)
{
    uint32_t table = r_u32(0x8005B40C);
    uint32_t record = table + 72 * index;
    int32_t dx;
    int32_t dz;
    int32_t distance;
    uint32_t angle = 0;
    int32_t result = 0xFFFF;
    FUNCTION_MARKER(0x80019E40, "SLUS_010.53");
    if (!table)
    {
        xport_store_le32(level_out, 127);
        xport_store_le32(pan_out, 64);
        return 64;
    }
    xport_store_le32(pan_out, r_u32(record + 20));
    dx = ((int32_t)(uint32_t)((uint32_t)x - r_u32(record + 4)));
    dz = ((int32_t)(uint32_t)((uint32_t)z - r_u32(record + 8)));
    distance = leaf_metric(dx, dz) >> (shift & 31);
    if (distance > 0x400000)
    {
        xport_store_le32(level_out, 0);
        if (ratio_out)
        {
            xport_store_le32(ratio_out, 0x10000);
            return 0x10000;
        }
        return 0xFFFF;
    }
    if (distance > 0xFFFF)
    {
        uint32_t attenuation = 127u - (uint32_t)(distance >> 15);
        uint32_t sign = (uint32_t)(((int32_t)(uint32_t)(attenuation)) >> 31);
        uint32_t reduced = 127u - attenuation;
        uint32_t reduced_sign = (uint32_t)(((int32_t)(uint32_t)(reduced)) >> 31);
        xport_store_le32(level_out, (~sign & attenuation) + (reduced_sign & reduced));
        angle = sub_80020018((uint32_t)dx, 0u - (uint32_t)dz);
        result = 1;
        if (r_u32(r_u32(0x8005B2F8) + 48) == 1)
        {
            result = (int32_t)(((((r_u32(record) - angle) & 0xFFFu) >> 4) + 64u) & 0xFFu);
            xport_store_le32(pan_out, (uint32_t)result);
        }
    }
    else
    {
        xport_store_le32(level_out, 127);
        result = 127;
    }
    if (ratio_out)
    {
        int32_t sx = ((int32_t)(uint32_t)((uint32_t)other_x - r_u32(record + 12)));
        int32_t sz = ((int32_t)(uint32_t)((uint32_t)other_z - r_u32(record + 16)));
        int32_t metric = leaf_metric(sx, sz);
        uint32_t delta = (sub_80020018((uint32_t)sx, 0u - (uint32_t)sz) - angle) & 0xFFFu;
        int32_t sine = r_s16(0x8005624C + ((delta * 4) | 2));
        uint32_t product = (uint32_t)(metric >> 8) * (uint32_t)(sine >> 4);
        int32_t divisor = ((int32_t)(uint32_t)(product + 0x02000000u));
        if (divisor > 0)
            result = ((int32_t)(uint32_t)(sub_80010028(0x02000000, (uint32_t)divisor)));
        else
            result = ((int32_t)(uint32_t)(0u - sub_80010028(0x02000000, 0u - (uint32_t)divisor)));
        xport_store_le32(ratio_out, (uint32_t)result);
    }
    return result;
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
    uint32_t scalar_low_6;
    uint32_t scalar_low_7;

    uint32_t base = r_u32(0x8005B410 + 4 * listener);
    uint32_t result = 44 * slot;
    uint32_t record = base + result;
    uint32_t level_out;
    uint32_t pan_out;
    uint32_t ratio_out = 0x1F800368u;
    /* TODO: Resolve untouched ratio on the no-table path before moving its storage */
    uint8_t parameters[12];
    uint32_t mode;
    int32_t x;
    int32_t z;
    FUNCTION_MARKER(0x8001769C, "SLUS_010.53");
    if (!base)
        return result;
    x = ((int32_t)(uint32_t)(r_u32(record + 12))) + (((int32_t)(uint32_t)(r_u32(record + 20))) >> 6);
    z = ((int32_t)(uint32_t)(r_u32(record + 16))) + (((int32_t)(uint32_t)(r_u32(record + 24))) >> 6);
    w_u32(record + 12, (uint32_t)x);
    w_u32(record + 16, (uint32_t)z);
    sub_80019E40(listener, x, z, ((int32_t)(uint32_t)(r_u32(record + 20))), ((int32_t)(uint32_t)(r_u32(record + 24))), &level_out, &pan_out, xport_guest_ptr(ratio_out, 4), r_u32(record + 8) == 6 ? 3 : 0);
    if (r_u32(record + 8) >= 3 && r_u32(record + 8) <= 5)
        mode = 2;
    else if (r_u32(record + 8) == 2)
        mode = 4;
    else
        mode = 1;
    xport_store_le32(parameters, (uint32_t)((scalar_low_6 = (uint32_t)((uint64_t)(uint32_t)((int32_t)(((int32_t)(uint32_t)(r_u32(record + 28))))) * (uint32_t)((int32_t)(((int32_t)(uint32_t)(r_u32(ratio_out))))))), (((int32_t)(((int32_t)(uint32_t)(scalar_low_6)) >> ((uint32_t)(16)))))));
    xport_store_le32(parameters + 4, (uint32_t)((scalar_low_7 = (uint32_t)((uint64_t)(uint32_t)((int32_t)(((int32_t)(uint32_t)(xport_load_le32(&level_out))))) * (uint32_t)((int32_t)(((int32_t)(uint32_t)(r_u32(0x800D6C00 + 4 * mode))))))), (((int32_t)(((int32_t)(uint32_t)(scalar_low_7)) >> ((uint32_t)(7)))))));
    xport_store_le32(parameters + 8, xport_load_le32(&pan_out));
    result = sub_8001F174(sound, kind, 0, 1, parameters);
    w_u32(record + 32, result);
    w_u32(record + 4, 1);
    return 1;
}

uint32_t sub_80017BA0(int32_t x, int32_t z, uint32_t sound, uint32_t handle)
{
    uint32_t scalar_low_8;

    uint32_t state = r_u32(0x8005B2F8);
    uint32_t players;
    uint32_t index = 0;
    uint32_t level_out;
    uint32_t pan_out;
    uint8_t parameters[12];
    FUNCTION_MARKER(0x80017BA0, "SLUS_010.53");
    if (handle == r_u32(0x8005B420))
        return handle;
    players = r_u32(state + 48);
    if (!players)
        return 0;
    do
    {
        if (x || z)
            sub_80019E40(index, x, z, 0, 0, &level_out, &pan_out, NULL, 0);
        else
        {
            xport_store_le32(&level_out, 127);
            xport_store_le32(&pan_out, 64);
            index = players;
        }
        xport_store_le32(parameters, 0xFFFFFFFF);
        xport_store_le32(parameters + 4, (uint32_t)((scalar_low_8 = (uint32_t)((uint64_t)(uint32_t)((int32_t)(((int32_t)(uint32_t)(xport_load_le32(&level_out))))) * (uint32_t)((int32_t)(((int32_t)(uint32_t)(r_u32(0x800D6C0C))))))), (((int32_t)(((int32_t)(uint32_t)(scalar_low_8)) >> ((uint32_t)(7)))))));
        xport_store_le32(parameters + 8, xport_load_le32(&pan_out));
        if (!handle)
            handle = r_u32(0x8005B404);
        sub_8001F174(handle, sound, 0, 0, parameters);
        ++index;
    } while (index < r_u32(state + 48));
    return 0;
}

uint32_t sub_80017814(uint32_t slot, uint32_t listener)
{
    uint32_t offset = 44 * slot;
    uint32_t base_address = 0x8005B410 + 4 * listener;
    uint32_t base = r_u32(base_address);
    uint32_t record = base + offset;
    uint32_t listener_table;
    uint32_t count;
    uint32_t index;
    FUNCTION_MARKER(0x80017814, "SLUS_010.53");
    if (!record)
        return offset;
    if (r_u32(record + 4))
    {
        (uint32_t)sub_8001F7EC(r_u32(record + 32));
        listener_table = r_u32(0x8005B40C) + 72 * listener;
        count = r_u32(listener_table + 36);
        if (((int32_t)(uint32_t)(slot)) < ((int32_t)(uint32_t)(count)))
        {
            uint32_t child = base + 44 * count;
            for (index = 0; ((int32_t)(uint32_t)(index)) < ((int32_t)(uint32_t)(r_u32(listener_table + 40))); ++index, child += 44)
            {
                if (r_u16(record) == r_u16(child) && ((int32_t)(uint32_t)(r_u32(child + 8))) >= 3)
                    sub_80017814(count + index, listener);
            }
        }
        if (r_u32(record + 8) == 2)
        {
            int32_t sound_index = ((int32_t)(uint32_t)(r_u32(record + 40)));
            uint32_t sound = 0x800D6AA0 + 32 * (uint32_t)sound_index;
            if (r_u32(sound + 8) == 3 || sound_index < 0)
            {
                w_u32(sound + 8, 4);
                w_u32(listener_table + 44, 0);
            }
        }
    }
    w_u16(record, 224);
    w_u32(record + 8, 0);
    w_u32(record + 4, 0);
    w_u32(record + 36, r_u32(r_u32(0x8005B2F8) + 12));
    return 224;
}

static int32_t leaf_quaternion_reciprocal(uint32_t value)
{
    uint32_t scaled = value << 3;
    uint32_t magnitude = ((int32_t)(uint32_t)(scaled)) < 0 ? 0u - scaled : scaled;
    uint32_t divisor = (magnitude >> 1) + (uint32_t)(((int32_t)(uint32_t)(magnitude - 2)) >> 31);
    uint32_t result = 0x80000000u / divisor;
    return ((int32_t)(uint32_t)(scaled)) < 0 ? ((int32_t)(uint32_t)(0u - result)) : ((int32_t)(uint32_t)(result));
}

static int16_t leaf_quaternion_component(int32_t value, int32_t scale)
{
    uint32_t product = (uint32_t)((int64_t)value * scale);
    return (int16_t)(((int32_t)(uint32_t)(product)) >> 14);
}

static void leaf_direction_op(uint32_t diagonal_address, uint32_t direction_address, uint32_t output)
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
        w_u16(output + 2 * i, (uint32_t)value);
    }
}

static uint32_t leaf_80012AEC_values(uint32_t object, uint32_t index, uint32_t values[3])
{
    uint32_t records = r_u32(r_u32(object + 96) + 8);
    uint32_t record = records + 12 * index;
    uint32_t source = r_u32(record + 8);
    uint32_t shift;
    uint32_t i;
    if (!source)
        return 0xFFFFFFFFu;
    shift = (r_u16(r_u32(record) + 14) >> 12) & 31;
    for (i = 0; i < 3; ++i)
        values[i] = (uint32_t)(((int32_t)(uint32_t)((uint32_t)(int32_t)r_s16(source + 24 + 2 * i))) >> shift) << 10;
    return 0;
}

uint32_t sub_80012AEC(uint32_t object, uint32_t index, uint32_t output)
{
    uint32_t values[3];
    uint32_t result;
    uint32_t i;
    FUNCTION_MARKER(0x80012AEC, "SLUS_010.53");
    result = leaf_80012AEC_values(object, index, values);
    if (result)
        return result;
    for (i = 0; i < 3; ++i)
        w_u32(output + 4 * i, values[i]);
    return 0;
}

uint32_t sub_80012FC8(uint32_t object, uint32_t index)
{
    uint32_t values[3];
    uint32_t type;
    uint32_t result;
    FUNCTION_MARKER(0x80012FC8, "SLUS_010.53");
    (void)leaf_80012AEC_values(object, index, values);
    type = r_u16(object + 172) >> 5;
    if ((uint32_t)(type - 1) < 2)
    {
        w_u32(object + 312, values[0] << 1);
        w_u32(object + 308, values[1]);
        w_u32(object + 304, values[2]);
        return values[2];
    }
    w_u32(object + 304, values[0]);
    w_u32(object + 312, values[1] << 1);
    w_u32(object + 308, values[2]);
    result = type;
    if (!type)
    {
        uint32_t kind = r_u32(object + 180);
        int32_t value = ((int32_t)(uint32_t)(r_u32(object + 312)));
        if (kind >= 18)
            value = value * 11 / 16;
        else
        {
            uint32_t alternate = kind < 9 || r_u32(r_u32(0x8005B2F8) + 60) == 2;
            value = ((int32_t)(uint32_t)((uint32_t)((2 * alternate + 9) * value))) / 8;
        }
        w_u32(object + 312, (uint32_t)value);
        result = (uint32_t)(value - 4096);
        if (r_u16(object + 172) < r_u32(r_u32(0x8005B2F8) + 48))
            w_u32(object + 312, result);
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
        axes[i] = r_u32(0x8005B61C + 4 * i);
        diagonal[i] = r_s16(matrix + 8 * i);
    }
    trace = diagonal[0] + diagonal[1] + diagonal[2];
    if (((int32_t)(uint32_t)((uint32_t)trace << 4)) > 0)
    {
        root = sub_8004CF74(((uint32_t)trace << 4) + 0x10000u);
        w_u16(output + 6, (uint16_t)(root >> 1));
        reciprocal = leaf_quaternion_reciprocal(root);
        w_u16(output, (uint16_t)leaf_quaternion_component(r_s16(matrix + 10) - r_s16(matrix + 14), reciprocal));
        w_u16(output + 2, (uint16_t)leaf_quaternion_component(r_s16(matrix + 12) - r_s16(matrix + 4), reciprocal));
        w_u16(output + 4, (uint16_t)leaf_quaternion_component(r_s16(matrix + 2) - r_s16(matrix + 6), reciprocal));
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
        w_u16(output + 2 * largest, (uint16_t)(root >> 1));
        reciprocal = leaf_quaternion_reciprocal(root);
        w_u16(output + 6, (uint16_t)leaf_quaternion_component(r_s16(matrix + 2 * (3 * second + third)) - r_s16(matrix + 2 * (3 * third + second)), reciprocal));
        w_u16(output + 2 * second, (uint16_t)leaf_quaternion_component(r_s16(matrix + 2 * (3 * largest + second)) + r_s16(matrix + 2 * (3 * second + largest)), reciprocal));
        w_u16(output + 2 * third, (uint16_t)leaf_quaternion_component(r_s16(matrix + 2 * (3 * largest + third)) + r_s16(matrix + 2 * (3 * third + largest)), reciprocal));
        return (uint32_t)(int32_t)r_s16(output + 2 * third);
    }
}

uint32_t sub_8001A760(uint32_t actor_index, uint32_t mode)
{
    uint32_t difference, sign, magnitude;
    uint32_t direct = 0;
    uint32_t random_choice = 0;

    uint32_t state = r_u32(0x8005B2F8);
    uint32_t players = r_u32(state + 48);
    uint32_t actor = r_u32(0x8005B3A0) + 1096 * actor_index;
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
        uint32_t listener_table = r_u32(0x8005B40C) + listener_offset;
        uint32_t record_base = r_u32(0x8005B410 + 4 * listener);
        uint32_t other = r_u32(0x8005B268 + 4 * listener);
        uint32_t first;
        uint32_t limit;
        uint32_t slot;
        uint32_t record;
        uint32_t bank;
        uint32_t sample;
        uint32_t row = 0;
        uint32_t side = 0;
        uint32_t candidate = 255;
        uint32_t candidates[3] = {255, 255, 255};
        uint32_t found = 0;
        uint32_t code;
        if (!record_base)
            return 0;
        if (!mode)
        {
            difference = r_u32(other + 324) - r_u32(actor + 324);
            sign = (uint32_t)((int32_t)difference >> 31);
            magnitude = (difference + sign) ^ sign;
            if ((int32_t)magnitude > 0xF000)
                continue;
            if (r_u16(actor + 172) >= players && r_u32(listener_table + 44) && r_u32(actor + 1088))
                continue;
        }
        first = r_u32(listener_table + 36);
        limit = first + r_u32(listener_table + 40);
        record = record_base + 44 * first;
        for (slot = first; slot < limit; ++slot, record += 44)
        {
            if (r_u16(record) == actor_index && ((int32_t)(uint32_t)(r_u32(record + 8))) < 3)
            {
                if (!mode && r_u16(actor + 172) >= players && r_u32(actor + 1088))
                    return r_u32(actor + 1088);
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
            code = r_u8(r_u32(actor + 1084) + 1);
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
                    if (r_u32(sound + 12 + 4 * side) != candidate)
                    {
                        side ^= 1;
                        if (r_u32(sound + 12 + 4 * side) != candidate)
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
            {
                side = r_u32(0x8005B440) == 0;
                w_u32(0x8005B440, side);
            }
            if (r_u32(actor + 1088))
            {
                if ((candidate - 68 < 2) || (candidate == 7 && r_u16(actor + 172) >= players))
                {
                    int32_t now = (int32_t)((r_u32(state + 12) >> 8) & 0xFFFF);
                    difference = (uint32_t)now - (uint32_t)(int32_t)r_s16(actor + 870);
                    sign = (uint32_t)((int32_t)difference >> 31);
                    magnitude = (difference + sign) ^ sign;
                    if ((int32_t)magnitude < 21)
                        return 1;
                    w_u16(actor + 870, (uint16_t)now);
                }
            }
            bank = r_u32(0x800D6AA0 + 32 * row);
            sample = r_u32(0x800D6AA0 + 32 * row + 20 + 4 * side);
        }
        first = r_u32(listener_table + 36);
        limit = first + r_u32(listener_table + 40);
        record = record_base + 44 * first;
        for (slot = first; slot < limit; ++slot, record += 44)
        {
            if (!r_u32(record + 4))
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
            if (r_u16(actor + 172) < players || !r_u32(actor + 1088))
            {
                uint32_t source = r_u32(actor + 1088) ? actor : r_u32(actor + 856);
                uint32_t nearest = sub_8008B428(source + 172, 1) & 0xFFFF;
                if (nearest != 224)
                {
                    other = r_u32(0x8005B3A0) + 1096 * nearest;
                    difference = (r_u32(other + 324) - r_u32(source + 324)) << 4;
                    sign = (uint32_t)((int32_t)difference >> 31);
                    magnitude = (difference + sign) ^ sign;
                    if ((int32_t)magnitude <= 0x13FFFF && (r_u32(other + 560) & 0x08000000))
                    {
                        uint32_t vehicle = r_u32(other + 1084);
                        uint32_t table_index = r_u8(0x800D38B0 + r_u16(source + 172));
                        int32_t gear = r_s8(other + 946);
                        w_u8(vehicle + table_index + 16, 15);
                        if ((uint32_t)(r_u16(other + 956 + 8 * (uint32_t)(gear - 1)) - 4) < 13 && sub_800BC1EC(other, source, (int32_t)difference))
                        {
                            uint32_t event = 0x1F8003A0;
                            sub_800B92C0(vehicle);
                            w_u16(event, 16);
                            w_u16(event + 2, r_u16(actor + 172));
                            sub_800BCA68(xport_guest_ptr(event, 8), 2, other);
                        }
                    }
                }
            }
            if (r_u32(actor + 1088))
            {
                int32_t delta;
                if (r_u32(other + 360) == r_u32(actor + 360) && (!(r_u32(other + 360) >> 16) || r_u32(other + 336) == r_u32(actor + 336)))
                {
                    delta = (int32_t)(r_u32(other + 344) - r_u32(actor + 344));
                    if (((int32_t)(uint32_t)(r_u32(actor + 364))) < 0)
                        delta = (int32_t)(0u - (uint32_t)delta);
                }
                else
                {
                    delta = ((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(other + 184, 12), xport_guest_ptr(actor + 432, 6), xport_guest_ptr(actor + 184, 12))));
                }
                {
                    uint32_t flags = r_u32(actor + 852) + 572;
                    xport_update_u8(flags, XPORT_MEMORY_UPDATE_OR, delta < 0 ? 8 : 4);
                }
            }
        }
        if ((candidate & 0xFE) == 4 || (candidate & 0xFE) == 68)
            w_u32(0x800D6AA0 + 32 * row + 12 + 4 * side, (candidate & 1) + 68);
        else if (!have_sound && !(candidate & 8) && !random_choice)
            w_u32(0x800D6AA0 + 32 * row + 12 + 4 * side, 0xFFFFFFFF);
        if (direct)
        {
            sub_80017BA0(((int32_t)(uint32_t)(r_u32(actor + 184))), ((int32_t)(uint32_t)(r_u32(actor + 192))), sample, bank);
            w_u32(0x800D6AA0 + 32 * row + 8, 4);
        }
        else
        {
            w_u16(record, (uint16_t)actor_index);
            w_u32(record + 12, r_u32(actor + 184));
            w_u32(record + 16, r_u32(actor + 192));
            difference = (uint32_t)(int32_t)r_s16(actor + 450) * (uint32_t)((int32_t)r_u32(actor + 480) >> 4);
            w_u32(record + 20, (uint32_t)((int32_t)difference >> 8));
            difference = (uint32_t)(int32_t)r_s16(actor + 454) * (uint32_t)((int32_t)r_u32(actor + 480) >> 4);
            w_u32(record + 24, (uint32_t)((int32_t)difference >> 8));
            w_u32(record + 28, sub_8001F934(((int32_t)(uint32_t)(bank)), sample, 0));
            w_u32(record + 8, 2);
            sub_8001769C(slot, listener, bank, sample);
            w_u32(record + 36, r_u32(state + 12) + 500);
            if (r_u16(actor + 172) >= players && !mode)
                w_u32(listener_table + 44, 1);
            w_u32(0x800D6AA0 + 32 * row + 8, 3);
            w_u32(record + 40, row);
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
    uint32_t table = r_u32(object + 184);
    uint32_t first = r_u32(table + 4 * mapping);
    uint32_t duration = r_u32(object + 8 * first + 20);
    uint32_t state, clock, threshold, second;
    FUNCTION_MARKER(0x800C2178, "RASHCDG.BIN");
    if (((int32_t)(uint32_t)(duration)) <= 0)
        return 0;
    state = r_u32(0x8005B2F8);
    clock = r_u32(object);
    threshold = r_u32(state + 32);
    if (((int32_t)(uint32_t)(threshold)) < ((int32_t)(uint32_t)(clock - duration)))
        return 0;
    second = r_u16(pair + 2);
    if (second == 15)
        return 1;
    first = r_u32(table + 4 * second);
    return ((int32_t)(uint32_t)(r_u32(object + 8 * first + 20))) > 0;
}

uint32_t sub_8008CDF4(uint32_t index)
{
    FUNCTION_MARKER(0x8008CDF4, "RASHCDG.BIN");
    switch (index)
    {
        case 0:
            return r_s8(0x8005B2B9) < r_s8(0x8005B2B8);
        case 2:
            return r_u32(0x800D4B80) && ((int32_t)(uint32_t)(r_u32(0x800D4B74))) < 4 && ((int32_t)(uint32_t)(r_u32(0x800D4B70))) < ((int32_t)(uint32_t)(r_u32(0x800D8744))) && ((int32_t)(uint32_t)(r_u32(0x800D4C7C))) > 0;
        case 3:
            return ((int32_t)(uint32_t)(r_u32(0x800CF654))) < 16 && ((int32_t)(uint32_t)(r_u32(0x800D4C6C))) > 0;
        case 4:
            return (((int32_t)(uint32_t)(r_u32(0x800CD6CC))) < ((int32_t)(uint32_t)(r_u32(0x800CD6D0))) || r_u32(0x800D1814) >= 596) && ((int32_t)(uint32_t)(r_u32(0x800D4C9C))) > 0;
        case 5:
            return (((int32_t)(uint32_t)(r_u32(0x800CE59C))) < ((int32_t)(uint32_t)(r_u32(0x800CE5A0))) || r_u32(0x800D1814) >= 452) && ((int32_t)(uint32_t)(r_u32(0x800D4C9C))) > 0;
        case 6:
            return r_u32(0x800CD6C4) && ((int32_t)(uint32_t)(r_u32(0x800CD6AC))) < ((int32_t)(uint32_t)(r_u32(0x8005B214)));
        default:
            return 0;
    }
}

uint32_t sub_8009D9E4(void)
{
    int32_t remaining = ((int32_t)(uint32_t)(r_u32(r_u32(0x800CE50C))));
    uint32_t object = r_u32(0x800CE500);
    uint32_t stride = r_u32(0x800CE504);
    FUNCTION_MARKER(0x8009D9E4, "RASHCDG.BIN");
    while (remaining >= 0)
    {
        if (object && r_u16(object + 172) && !r_u32(object + 180))
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
    result = ((int32_t)(uint32_t)(product + (uint32_t)(scale >> 1)));
    return signed_kind < 0 ? ((int32_t)(uint32_t)(0u - (uint32_t)result)) : result;
}

uint32_t sub_80097308(uint32_t object)
{
    uint32_t state = r_u32(object + 852);
    uint32_t control = r_u32(object + 1084);
    uint32_t kind;
    uint32_t result;
    FUNCTION_MARKER(0x80097308, "RASHCDG.BIN");
    w_u8(control + 39, 0xFE);
    w_u32(state + 552, r_u32(state + 552) & 0xFFEFF067u);
    w_u32(object + 720, 0x20000);
    kind = r_u16(state + 544);
    if (kind < 72 || kind >= 74)
    {
        w_u32(object + 480, 0);
        w_u32(state + 488, 0);
        w_u32(state + 484, 0);
        w_u32(state + 480, 0);
        w_u32(state + 464, 0);
        w_u32(state + 460, 0);
        w_u32(state + 456, 0);
    }
    result = r_u32(r_u32(0x8005B2F8) + 16);
    w_u32(control + 40, result);
    w_u32(object + 924, 0);
    return result;
}

uint32_t sub_800A8BE0(uint16_t value, uint32_t packed_out)
{
    uint32_t packed = r_u32(packed_out);
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
    w_u32(packed_out, r_u32(packed_out) | (encoded << shift));
    return 1;
}

int32_t sub_8009DA4C(uint32_t source, uint32_t stopped_out)
{
    int32_t scalar_value_54;
    int32_t scalar_sign_54;

    int32_t remaining = ((int32_t)(uint32_t)(r_u32(r_u32(0x800CE50C))));
    uint32_t object = r_u32(0x800CE500);
    uint32_t stride = r_u32(0x800CE504);
    uint32_t nearest = 0x03E70000;
    FUNCTION_MARKER(0x8009DA4C, "RASHCDG.BIN");
    while (remaining >= 0)
    {
        if (object && r_u16(object + 172) && !r_u32(object + 180))
        {
            uint32_t distance = ((scalar_value_54 = (((int32_t)(uint32_t)((r_u32(source + 324) - r_u32(object + 324)) << 4)))), (scalar_sign_54 = scalar_value_54 >> 31), (((uint32_t)((uint32_t)((scalar_sign_54 + scalar_value_54) ^ scalar_sign_54)))));
            if (distance < nearest)
            {
                nearest = distance;
                w_u32(stopped_out, ((int32_t)(uint32_t)(r_u32(object + 480))) < 131);
            }
        }
        --remaining;
        object += stride;
    }
    return nearest == 0x03E70000 ? -65536 : ((int32_t)(uint32_t)(nearest));
}

uint32_t sub_8009E444(uint32_t point, uint32_t object)
{
    int32_t scalar_sign_55;
    int32_t scalar_sign_56;
    int32_t scalar_value_57;
    int32_t scalar_sign_57;

    int32_t x;
    int32_t z;
    int32_t high;
    int32_t low;
    int32_t combined;
    FUNCTION_MARKER(0x8009E444, "RASHCDG.BIN");
    if (!point || !object)
        return 0;
    {
        uint32_t state = r_u32(object + 852);
        uint32_t kind = r_u32(state + 604);
        if (kind == 3 || kind == 4)
            object = state;
    }
    x = (int32_t)r_s16(point + 186) - (((int32_t)(uint32_t)(r_u32(object + 184))) >> 16);
    z = (int32_t)r_s16(point + 194) - (((int32_t)(uint32_t)(r_u32(object + 192))) >> 16);
    x = ((int32_t)(uint32_t)(((scalar_sign_55 = ((int32_t)(x)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_55 + ((int32_t)(x))) ^ scalar_sign_55)))))));
    z = ((int32_t)(uint32_t)(((scalar_sign_56 = ((int32_t)(z)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_56 + ((int32_t)(z))) ^ scalar_sign_56)))))));
    high = x >= z ? x : z;
    low = x >= z ? z : x;
    combined = high - (high >> 5) - (high >> 7) + ((low + (low >> 1)) >> 2) + ((low + (low >> 1)) >> 6);
    return ((scalar_value_57 = (((int32_t)(uint32_t)((uint32_t)combined << 16)))), (scalar_sign_57 = scalar_value_57 >> 31), (((uint32_t)((uint32_t)((scalar_sign_57 + scalar_value_57) ^ scalar_sign_57)))));
}

int32_t sub_8009E768(uint32_t object)
{
    int32_t scalar_sign_58;
    int32_t scalar_value_59;
    int32_t scalar_sign_59;
    int32_t scalar_sign_60;
    int32_t scalar_value_61;
    int32_t scalar_sign_61;

    uint32_t record = 0;
    int32_t base = 0;
    int32_t scale;
    int32_t count;
    int32_t selector;
    FUNCTION_MARKER(0x8009E768, "RASHCDG.BIN");
    if (r_u16(object + 362))
    {
        uint32_t direction_out = 0x1F8003F4;
        record = sub_8003E1E8(r_u32(object + 328), (uint32_t)(int32_t)r_s16(r_u32(object + 356) + 10), direction_out);
        if (record)
        {
            scale = ((int32_t)(uint32_t)(r_u32(record + 4)));
            if (((int32_t)(uint32_t)(r_u32(direction_out))) > 0)
            {
                count = r_s16(record + 140);
                if (r_s16(record + 142) > 0)
                    base = ((int32_t)(uint32_t)(r_u32(record + 208)));
            }
            else
            {
                count = r_s16(record + 12);
                if (r_s16(record + 14) > 0)
                    base = ((int32_t)(uint32_t)(r_u32(record + 80)));
            }
            selector = r_s8(object + 508);
            return ((int32_t)(uint32_t)(((scalar_sign_58 = ((int32_t)(base)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_58 + ((int32_t)(base))) ^ scalar_sign_58))))) + ((scalar_value_59 = (sub_8009CF1C(scale, count, selector))), (scalar_sign_59 = scalar_value_59 >> 31), (((uint32_t)((uint32_t)((scalar_sign_59 + scalar_value_59) ^ scalar_sign_59)))))));
        }
    }
    else
    {
        record = r_u32(object + 372);
        if (record)
        {
            scale = ((int32_t)(uint32_t)(r_u32(object + 424)));
            if (((int32_t)(uint32_t)(r_u32(object + 364))) > 0)
            {
                count = r_s16(object + 420);
                if (r_s16(record + 142) > 0)
                    base = ((int32_t)(uint32_t)(r_u32(record + 208)));
            }
            else
            {
                count = r_s16(object + 408);
                scale = ((int32_t)(uint32_t)(0u - (uint32_t)scale));
                if (r_s16(record + 14) > 0)
                    base = ((int32_t)(uint32_t)(r_u32(record + 80)));
            }
            selector = r_s8(object + 508);
            return ((int32_t)(uint32_t)(((scalar_sign_60 = ((int32_t)(base)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_60 + ((int32_t)(base))) ^ scalar_sign_60))))) + ((scalar_value_61 = (sub_8009CF1C(scale, count, selector))), (scalar_sign_61 = scalar_value_61 >> 31), (((uint32_t)((uint32_t)((scalar_sign_61 + scalar_value_61) ^ scalar_sign_61)))))));
        }
    }
    return ((int32_t)(uint32_t)(r_u32(object + 364))) < 0 ? -117964 : 117964;
}

uint32_t sub_800A3F14(uint32_t config, uint32_t object)
{
    uint32_t flags = r_u16(config + 12);
    uint32_t result = flags & 2;
    uint32_t i;
    FUNCTION_MARKER(0x800A3F14, "RASHCDG.BIN");
    if (flags & 1)
    {
        uint32_t piece = r_u32(object + 340);
        for (i = 0; i < 9; ++i)
            w_u16(object + 432 + 2 * i, r_u16(piece + 2 + 2 * i));
        for (i = 0; i < 3; ++i)
            w_u16(object + 438 + 2 * i, 0u - r_u16(object + 438 + 2 * i));
    }
    else
    {
        for (i = 0; i < 9; ++i)
            w_u16(object + 432 + 2 * i, i == 0 || i == 4 || i == 8 ? 0x1000 : 0);
    }
    if (flags & 2)
    {
        for (i = 0; i < 3; ++i)
            w_u16(object + 444 + 2 * i, r_u16(config + 14 + 2 * i));
        if (flags & 1)
        {
            leaf_direction_op(object + 438, object + 444, object + 432);
            return sub_8002E468(xport_guest_ptr(object + 432, 6));
        }
        w_u16(object + 432, r_u16(config + 18));
        result = 0u - r_u16(config + 14);
        w_u16(object + 436, result);
    }
    return result;
}

uint32_t sub_800A07D0(uint32_t config, uint32_t object, uint32_t kind, uint32_t other)
{
    uint32_t position = 0x1F800380;
    uint32_t buffer = 0x1F800390;
    uint32_t segment;
    uint32_t incomplete = 1;
    FUNCTION_MARKER(0x800A07D0, "RASHCDG.BIN");
    w_u32(position, r_u32(config + 8));
    w_u32(position + 4, (uint32_t)(int32_t)r_s16(config + 60));
    w_u32(position + 8, r_u32(config + 36));
    segment = sub_80039DFC(0, ((position) ? (const uint32_t[3]){r_u32(position), 0, (uint32_t)r_u16((position) + 10) << 16} : NULL));
    uint32_t geometry_result = 0;
    if (segment)
    {
        const uint32_t route_position[3] = {r_u32(position), r_u32(position + 4), r_u32(position + 8)};
        uint32_t route_geometry[8] = {r_u32(buffer), r_u32(buffer + 4), r_u32(buffer + 8), r_u32(buffer + 12), r_u32(buffer + 16), r_u32(buffer + 20), r_u32(buffer + 24), r_u32(buffer + 28)};
        geometry_result = sub_8003A700(segment, route_position, route_geometry);
        w_u32(buffer, route_geometry[0]);
        w_u32(buffer + 4, route_geometry[1]);
        w_u32(buffer + 8, route_geometry[2]);
        w_u32(buffer + 12, route_geometry[3]);
        w_u32(buffer + 16, route_geometry[4]);
        w_u32(buffer + 20, route_geometry[5]);
        w_u32(buffer + 24, route_geometry[6]);
        w_u32(buffer + 28, route_geometry[7]);
    }
    if (segment && geometry_result)
    {
        uint32_t track = object + 328;
        uint32_t piece;
        uint32_t value;
        if (r_u16(position + 2) == 1)
            (void)sub_8003C758(buffer, config + 20);
        (void)sub_8001E0B4(xport_guest_ptr(track, 32), xport_guest_ptr(buffer, 1), 32);
        w_u32(object + 184, r_u32(config + 20));
        w_u32(object + 188, r_u32(config + 24));
        w_u32(object + 192, r_u32(config + 28));
        piece = r_u32(object + 340);
        sub_80036800(object + 184, piece, xport_guest_ptr(object + 344, 4), xport_guest_ptr(object + 348, 4));
        (void)sub_8002FAD4(object, 6, r_u16(config + 2), 0);
        {
            uint32_t dimensions[3];
            uint32_t shape_type;
            (void)leaf_80012AEC_values(object, r_u16(config + 2), dimensions);
            shape_type = r_u16(object + 172) >> 5;
            if (shape_type - 1u < 2u)
            {
                w_u32(object + 312, dimensions[0] << 1);
                w_u32(object + 308, dimensions[1]);
                w_u32(object + 304, dimensions[2]);
            }
            else
            {
                w_u32(object + 304, dimensions[0]);
                w_u32(object + 312, dimensions[1] << 1);
                w_u32(object + 308, dimensions[2]);
                if (!shape_type)
                {
                    uint32_t shape_kind = r_u32(object + 180);
                    uint32_t shape_height = r_u32(object + 312);
                    uint32_t product;
                    if (shape_kind >= 18u)
                    {
                        product = 11u * shape_height;
                        w_u32(object + 312, (uint32_t)((int32_t)product / 16));
                    }
                    else
                    {
                        uint32_t alternate = shape_kind < 9u || r_u32(r_u32(0x8005B2F8u) + 60) == 2u;
                        product = (2u * alternate + 9u) * shape_height;
                        shape_height = (uint32_t)((int32_t)product / 8);
                        w_u32(object + 312, shape_height);
                        if (r_u16(object + 172) < r_u32(r_u32(0x8005B2F8u) + 48))
                            w_u32(object + 312, shape_height - 4096u);
                    }
                }
            }
        }
        (void)sub_800A3F14(config, object);
        {
            const uint16_t route_direction[3] = {r_u16(object + 444), r_u16(object + 444 + 2), r_u16(object + 444 + 4)};
            const uint32_t route_record[8] = {r_u32(track), r_u32(track + 4), r_u32(track + 8), r_u32(track + 12), r_u32(track + 16), r_u32(track + 20), r_u32(track + 24), r_u32(track + 28)};
            uint32_t route_position[3] = {r_u32(object + 360), r_u32(object + 360 + 4), r_u32(object + 360 + 8)};
            uint32_t route_result = sub_8003662C(route_direction, route_record, route_position);
            w_u32(object + 360, route_position[0]);
            w_u32(object + 360 + 4, route_position[1]);
            w_u32(object + 360 + 8, route_position[2]);
            (void)route_result;
        }
        w_u32(object + 364, r_u32(position + 4));
        sub_8003AF9C(object + 172, 0, other);
        w_u32(object + 324, sub_8003B61C(object + 172));
        w_u32(object + 180, r_u16(config + 2));
        w_u16(object + 322, r_u16(config + 62));
        value = sub_80039F68(object + 172);
        w_u16(object + 320, value);
        if ((value << 16) != 0)
        {
            int32_t offset;
            uint32_t angle;
            (void)sub_8003DE28(object, 1, 0, 0xFFFFFFFFu);
            incomplete = 0;
            piece = r_u32(object + 340);
            offset = ((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(object + 184, 12), xport_guest_ptr(piece + 8, 6), xport_guest_ptr(piece + 20, 12))));
            (void)sub_8002EAD8(xport_guest_ptr(object + 184, 12), xport_guest_ptr(piece + 8, 6), 0u - (uint32_t)offset, xport_guest_ptr(object + 184, 12));
            (void)sub_8008BA18(object);
            angle = sub_80020018((uint32_t)(int32_t)r_s16(object + 444) << 4, (uint32_t)(int32_t)r_s16(object + 448) << 4);
            w_u32(object + 292, angle);
            w_u32(object + 296, (uint32_t)(int32_t)r_s16(0x8005624C + 4 * (angle & 0xFFF) + 2) << 4);
            w_u32(object + 176, 0xFFFFFFFFu);
            w_u32(object + 300, (uint32_t)(int32_t)r_s16(0x8005624C + 4 * (angle & 0xFFF)) << 4);
        }
    }
    if (incomplete)
        (void)sub_8008C000(object + 172, kind);
    return incomplete ^ 1;
}

uint32_t sub_800A2630(uint32_t config, uint32_t other)
{
    uint32_t object = 0;
    uint32_t current = r_u32(0x800CD6CC);
    uint32_t maximum = r_u32(0x800CD6D0);
    uint32_t base = r_u32(0x800CD6D4);
    FUNCTION_MARKER(0x800A2630, "RASHCDG.BIN");
    if (((int32_t)(uint32_t)(current)) < ((int32_t)(uint32_t)(maximum)) || r_u32(0x800D1814) >= 596)
    {
        uint32_t next = current + 1;
        uint32_t candidate = base + 596 * next;
        uint32_t secondary = r_u32(0x800D1660);
        uint32_t secondary_next = secondary + 1;
        while (((int32_t)(uint32_t)(next)) < ((int32_t)(uint32_t)(maximum + 1)) && r_u16(candidate + 172))
        {
            ++next;
            candidate += 596;
        }
        w_u32(0x800CD6CC, next);
        object = base + 596 * current;
        w_u16(object + 172, current + 128);
        candidate = 0x800D1664 + 24 * secondary_next;
        while (((int32_t)(uint32_t)(secondary_next)) < 18 && r_u32(candidate))
        {
            ++secondary_next;
            candidate += 24;
        }
        w_u32(0x800D1660, secondary_next);
        w_u32(object + 4, 0x800D1664 + 24 * secondary);
        w_u32(0x800CD6C8, r_u32(0x800CD6C8) + 1);
        if (((int32_t)(uint32_t)(maximum)) < ((int32_t)(uint32_t)(current)))
        {
            w_u32(0x800CD6D0, current);
            w_u32(0x800D1814, r_u32(0x800D1814) - 596);
        }
    }
    if (!object || !sub_800A07D0(config, object, 4, other))
        return 0;
    {
        int32_t first = ((int32_t)(uint32_t)((uint32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(object + 308))), ((int32_t)(uint32_t)(r_u32(object + 304))))));
        int32_t scale = ((int32_t)(uint32_t)((uint32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(object + 312))), first) << 9));
        uint32_t i;
        if (scale > 0x800000)
            scale = 0x800000;
        w_u32(object + 316, (uint32_t)scale);
        w_u32(object + 504, r_u32(object + 184));
        w_u32(object + 508, r_u32(object + 188));
        w_u32(object + 512, r_u32(object + 192));
        for (i = 0; i < 9; ++i)
            w_u16(object + 516 + 2 * i, r_u16(object + 432 + 2 * i));
        w_u32(object + 480, 0);
        w_u32(object + 592, 0);
        w_u16(object + 450, r_u16(object + 444));
        w_u16(object + 452, r_u16(object + 446));
        w_u16(object + 454, r_u16(object + 448));
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
    current = r_u32(segment + 12);
    count = (int32_t)r_s16(r_u32(segment + 8) + 8) + r_s16(r_u32(segment + 8) + 10);
    end = r_u32(r_u32(segment) + 52) + 52 * (uint32_t)count - 52;
    if (current == end)
        return current;
    next = current + 52;
    if (((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(position, 12), xport_guest_ptr(current + 66, 6), xport_guest_ptr(current + 72, 12)))) <= 0)
        return current;
    for (;;)
    {
        int32_t limit = r_s16(end);
        if (r_s16(current) >= limit)
            return current;
        current = next;
        if (r_s16(next) < limit)
            next += 52;
        if (((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(position, 12), xport_guest_ptr(next + 14, 6), xport_guest_ptr(next + 20, 12)))) <= 0)
            return current;
    }
}

uint32_t sub_800302C4(uint32_t object, uint32_t id)
{
    uint32_t table;
    int32_t index;

    FUNCTION_MARKER(0x800302C4, "SLUS_010.53");
    if (id)
    {
        table = r_u32(0x8005B2E4);
        for (index = 0; index < 34; ++index)
            if (r_u8(table + 12 * (uint32_t)index) == id)
                break;
        if (index >= 34)
            index = -1;
    }
    else
        index = r_s8(r_u32(object + 96) + 7);
    w_u16(object + 74, (int16_t)index);
    index = r_s16(object + 74);
    if (index != -1)
    {
        uint32_t flags = r_u32(object + 36);

        if ((flags & 0x3F400) == 0x3F400)
        {
            uint32_t value;
            int32_t low;
            uint32_t field;

            table = r_u32(0x8005B2E4);
            value = r_u16(table + 12 * (uint32_t)index + 10);
            low = ((int32_t)(uint32_t)((value & 63) - 40u)) >> 3;
            field = (uint32_t)(3 * (511 - (int32_t)(value >> 6)) + low) & 63;
            w_u32(object + 36, (flags & 0xFFFC0FFFu) | (field << 12));
        }
        return 0;
    }
    return 0xFFFFFFFFu;
}

uint32_t sub_8002FDEC(uint32_t object, uint32_t id, uint32_t allocate, uint32_t bind)
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
    if (id < 50 && r_u32(0x800CE1B0 + 16 * id))
        descriptor = 0x800CE1B0 + 16 * id;
    w_u32(object + 96, descriptor);
    if (!descriptor)
        return 0xFFFFFFFFu;
    index = (int32_t)r_u8(descriptor + 4) - 1;
    w_u8(object + 8, (uint8_t)index);
    w_u8(object + 10, (uint8_t)index);
    w_u8(object + 11, (uint8_t)index);
    models = r_u32(descriptor + 8);
    model = r_u32(models + 12 * (uint32_t)index);
    xport_update_u8(object + 9, XPORT_MEMORY_UPDATE_OR, 3);
    w_u32(object, model);
    first_count = r_u16(model + 24);
    record_count = r_u16(r_u32(models) + 24);
    if (allocate)
        w_u32(object + 4, sub_8001447C(24 * record_count, 0));
    records = r_u32(object + 4);
    if (!records)
        return 0xFFFFFFFFu;
    {
        uint32_t source = r_u32(models + 12 * (uint32_t)index + 4);
        for (i = 0; i < first_count; ++i)
            w_u32(records + 24 * i, r_u32(source + 4 * i));
    }
    xport_update_u8(object + 9, XPORT_MEMORY_UPDATE_AND, 0xFB);
    w_u32(object + 56, 0);
    w_u32(object + 60, 0);
    w_u32(object + 64, 0);
    w_u32(object + 68, 0);
    w_u8(object + 72, 0);
    w_u8(object + 73, 0xFF);
    w_u32(object + 52, 0);
    w_u32(object + 20, 0);
    w_u32(object + 16, 0);
    w_u32(object + 12, 0);
    w_u16(object + 32, 0);
    w_u16(object + 30, 0);
    w_u16(object + 28, 0);
    for (i = 0; i < record_count; ++i)
    {
        uint32_t record = records + 24 * i;
        w_u16(record + 6, 0);
        w_u16(record + 8, 0);
        w_u16(record + 10, 0);
        w_u16(record + 14, 0);
        w_u16(record + 16, 0);
        w_u16(record + 18, 0);
        w_u16(record + 4, 0x1000);
        w_u16(record + 12, 0x1000);
        w_u16(record + 20, 0x1000);
    }
    w_u32(object + 44, 0x7FFFFFFF);
    w_u32(object + 48, 0x7FFFFFFF);
    flags = r_u32(object + 36);
    old_field = (flags >> 12) & 63;
    saved_value = r_u32(object + 76);
    w_u32(object + 76, 0x10000);
    w_u32(object + 36, 0x3F000);
    w_u32(object + 40, r_u32(model + 16));
    kind = (r_u16(model + 14) & 0x78) >> 3;
    switch (kind)
    {
        case 1:
            w_u32(object + 100, 0x80054198);
            w_u32(object + 76, saved_value);
            flags = (r_u32(object + 36) | 0x400) & 0xFFFC0FFFu;
            flags = (flags | (old_field << 12)) & 0xFFFBFFFFu;
            w_u32(object + 36, flags);
            break;
        case 2:
            w_u32(object + 100, 0x80054178);
            flags = (r_u32(object + 36) | 0x400) & 0xFFFC0FFFu;
            flags |= old_field << 12;
            flags &= 0xFFFFFFFEu;
            flags &= 0xFFFBFFFFu;
            flags &= 0xF7FFFFFFu;
            flags &= 0xDFFFFFFFu;
            flags &= 0xEFFFFFFFu;
            flags &= 0xFF87FFFFu;
            flags &= 0x3FFFFFFFu;
            w_u32(object + 36, flags);
            break;
        case 3:
        {
            uint32_t model_id = r_u32(model + 8);
            uint32_t field;
            w_u32(object + 100, 0x80054160);
            flags = r_u32(object + 36) | 0x400;
            if (model_id == 300)
                field = 0x2F000;
            else if (model_id == 315)
                field = 0x2E000;
            else if (model_id == 309)
                field = 0x1C000;
            else
                field = ((sub_8001FC58() % 15 + 14) & 63) << 12;
            flags = ((flags & 0xFFFC0FFFu) | field) & 0xFFFBFFFFu;
            w_u32(object + 36, flags);
            break;
        }
        case 4:
            w_u32(object + 100, 0x800541B8);
            w_u32(object + 36, r_u32(object + 36) | 0x400);
            break;
        default:
            w_u32(object + 100, 0);
            break;
    }
    w_u16(object + 74, (uint32_t)-1);
    if (bind)
        (void)sub_800302C4(object, 0);
    return 0;
}

uint32_t sub_8002FAD4(uint32_t object, uint32_t kind, uint32_t selector, uint32_t allocate)
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
            uint32_t map_index = r_u32(0x8005AEC8 + 4 * alternate);
            mapped = r_s16(r_u32(descriptor + 12) + 2 * map_index);
            break;
        }
        case 2:
        {
            uint32_t adjusted = selector;
            if ((selector - 3) < 3 || (selector - 12) < 3)
                adjusted -= 3;
            mapped = r_s16(r_u32(descriptor + 12) + 2 * adjusted);
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
                uint32_t state = r_u32(0x8005B2F8);
                uint32_t mode = r_u8(state + 4);
                uint32_t divisor = mode == 44 ? (uint32_t)(count - 2 - (int32_t)(mode & 1)) : (uint32_t)(count - (int32_t)(mode & 1));
                selector = sub_8001FC58() % divisor;
            }
            if (selector >= (uint32_t)count)
                break;
            entry = r_u32(table + 4) + 4 * selector;
            selected = (uint32_t)((int32_t)r_s16(entry) - ((int32_t)(uint32_t)(r_u32(descriptor + 8))));
            mapped = r_s16(r_u32(descriptor + 12) + 2 * selected);
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
            entry = r_u32(table + 4) + 4 * index;
            selected = (uint32_t)((int32_t)r_s16(entry) - ((int32_t)(uint32_t)(r_u32(descriptor + 8))));
            map_index = selected < 30;
            mapped = r_s16(r_u32(descriptor + 12) + 2 * r_u32(0x8005AED0 + 4 * map_index));
            break;
        }
        case 5:
        case 6:
            mapped = r_s16(r_u32(descriptor + 12));
            break;
        default:
            break;
    }
    if (mapped < 0)
        return 0xFFFF;
    result = sub_8002FDEC(object, (uint32_t)mapped, allocate, r_u32(0x80054144 + 4 * kind));
    if (((int32_t)(uint32_t)(result)) < 0)
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
    int32_t scalar_value_62;
    int32_t scalar_sign_62;
    int32_t scalar_value_63;
    int32_t scalar_sign_63;

    uint32_t secondary = r_u32(object + 852);
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
        w_u32(secondary + 552, r_u32(secondary + 552) & 0xFFFE7FBFu);
        (void)sub_8002090C(object);
        w_u32(object + 568, r_u32(object + 568) | 0x08000000u);
        state = r_u32(0x8005B2F8);
        if (r_u16(object + 172) >= r_u32(state + 48))
        {
            uint32_t dx = ((scalar_value_62 = (((int32_t)(uint32_t)(r_u32(object + 184) - r_u32(0x800CD950))))), (scalar_sign_62 = scalar_value_62 >> 31), (((uint32_t)((uint32_t)((scalar_sign_62 + scalar_value_62) ^ scalar_sign_62)))));
            uint32_t dz = ((scalar_value_63 = (((int32_t)(uint32_t)(r_u32(object + 192) - r_u32(0x800CD958))))), (scalar_sign_63 = scalar_value_63 >> 31), (((uint32_t)((uint32_t)((scalar_sign_63 + scalar_value_63) ^ scalar_sign_63)))));
            uint32_t minimum = dx < dz ? dx : dz;
            if (((int32_t)(uint32_t)(dx + dz - minimum / 2)) > 0x780000)
            {
                (void)sub_80096564(object);
                for (i = 0; i < 9; ++i)
                    w_u16(object + 432 + 2 * i, r_u16(object + 516 + 2 * i));
                w_u16(object + 450, r_u16(object + 528));
                w_u16(object + 452, r_u16(object + 530));
                w_u16(object + 454, r_u16(object + 532));
                w_u16(object + 814, r_u16(object + 516));
                w_u16(object + 816, r_u16(object + 518));
                w_u16(object + 818, r_u16(object + 520));
            }
        }
        (void)sub_8002EAD8(xport_guest_ptr(r_u32(object + 340) + 20, 12), xport_guest_ptr(object + 444, 6), 0x1E0000, xport_guest_ptr(object + 880, 12));
        linked = r_u32(object + 540);
        result = linked;
        if (linked)
        {
            w_u32(linked + 36, 0);
            result = r_u32(0x800CE178) - 1;
            w_u32(0x800CE178, result);
        }
        w_u32(object + 540, 0);
    }
    return result;
}

int32_t sub_80093BE0(uint32_t first, uint32_t second, uint32_t third)
{
    int32_t scalar_sign_64;
    int32_t scalar_sign_65;

    int32_t left = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(second, 6), xport_guest_ptr(first, 6))));
    int32_t right = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(third, 6), xport_guest_ptr(first, 6))));
    int32_t angle;
    FUNCTION_MARKER(0x80093BE0, "RASHCDG.BIN");
    if (((scalar_sign_64 = ((int32_t)(left)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_64 + ((int32_t)(left))) ^ scalar_sign_64))))) < ((scalar_sign_65 = ((int32_t)(right)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_65 + ((int32_t)(right))) ^ scalar_sign_65))))))
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
    int32_t scalar_value_66;
    int32_t scalar_sign_66;

    int32_t speed;
    int32_t ratio;
    FUNCTION_MARKER(0x80093CAC, "RASHCDG.BIN");
    if (vector)
    {
        w_u32(object + 892, r_u32(vector) - r_u32(object + 880));
        w_u32(object + 896, r_u32(vector + 4) - r_u32(object + 884));
        w_u32(object + 900, r_u32(vector + 8) - r_u32(object + 888));
    }
    else
    {
        if (!scale || !magnitude)
            return;
        sub_8002EE50(r_u32(magnitude), xport_guest_ptr(scale, 6), xport_guest_ptr(object + 892, 12));
    }
    if (r_s16(object + 944) > 0)
        return;
    speed = magnitude ? (int32_t)((scalar_value_66 = (((int32_t)(uint32_t)(r_u32(magnitude))))), (scalar_sign_66 = scalar_value_66 >> 31), (((uint32_t)((uint32_t)((scalar_sign_66 + scalar_value_66) ^ scalar_sign_66))))) : ((int32_t)(uint32_t)(sub_8002E548(xport_guest_ptr(object + 892, 12))));
    if (speed < 131)
        return;
    if (alternate)
    {
        w_u32(object + 904, sub_80010028(0x10000, (uint32_t)speed));
        ratio = (int32_t)sub_80010028((uint32_t)speed, 0x10000) + 0x8000;
    }
    else
    {
        w_u32(object + 904, sub_80010028(0xA0000, (uint32_t)speed));
        ratio = (int32_t)sub_80010028((uint32_t)speed, 0xA0000) + 0x8000;
    }
    if (ratio > 0x7FFF00)
        ratio = 0x7FFF00;
    w_u16(object + 944, (int16_t)(ratio >> 8));
    if (!(int16_t)(ratio >> 8))
        w_u32(object + 908, 0);
}

static void leaf_80093CAC_scalar(uint32_t object, uint32_t scale, int32_t magnitude, uint32_t alternate)
{
    int32_t scalar_sign_67;

    int32_t speed;
    int32_t ratio;
    if (!scale)
        return;
    sub_8002EE50((uint32_t)magnitude, xport_guest_ptr(scale, 6), xport_guest_ptr(object + 892, 12));
    if (r_s16(object + 944) > 0)
        return;
    speed = (int32_t)((scalar_sign_67 = ((int32_t)(magnitude)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_67 + ((int32_t)(magnitude))) ^ scalar_sign_67)))));
    if (speed < 131)
        return;
    if (alternate)
    {
        w_u32(object + 904, sub_80010028(0x10000, (uint32_t)speed));
        ratio = (int32_t)sub_80010028((uint32_t)speed, 0x10000) + 0x8000;
    }
    else
    {
        w_u32(object + 904, sub_80010028(0xA0000, (uint32_t)speed));
        ratio = (int32_t)sub_80010028((uint32_t)speed, 0xA0000) + 0x8000;
    }
    if (ratio > 0x7FFF00)
        ratio = 0x7FFF00;
    w_u16(object + 944, (int16_t)(ratio >> 8));
    if (!(int16_t)(ratio >> 8))
        w_u32(object + 908, 0);
}

uint32_t sub_800C0E98(uint32_t object, uint32_t other, int32_t delta)
{
    int32_t scalar_sign_68;

    int32_t limit = ((int32_t)(uint32_t)(r_u32(0x80052F9C)));
    uint32_t players;
    int32_t magnitude;
    uint32_t special = 0;
    int32_t offset;
    FUNCTION_MARKER(0x800C0E98, "RASHCDG.BIN");
    if (limit < delta)
    {
        uint32_t state = r_u32(0x8005B2F8);
        uint32_t value = r_u32(other + 480) - r_u32(0x80053024 + 4 * r_u32(state + 60));
        w_u32(object + 924, value);
        return value;
    }
    if (delta < -limit)
    {
        uint32_t value = r_u32(r_u32(object + 556) + 224);
        uint32_t flags = r_u32(object + 564);
        players = r_u32(r_u32(0x8005B2F8) + 48);
        if (r_u16(other + 172) < players)
            flags |= 0x200;
        w_u32(object + 924, value);
        w_u32(object + 564, flags);
        return r_u16(other + 172) < players;
    }
    magnitude = (int32_t)((scalar_sign_68 = ((int32_t)(delta)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_68 + ((int32_t)(delta))) ^ scalar_sign_68)))));
    if (magnitude < ((int32_t)(uint32_t)(r_u32(0x80052F70))) && r_u16(other + 172) < r_u16(object + 172) && (r_u32(other + 560) & 0x08000000))
    {
        uint32_t record = other + 956 + 8 * (uint32_t)r_s8(other + 946);
        if (r_u16(record - 6) == r_u16(object + 172))
        {
            uint32_t type = r_u16(record - 8);
            if (type >= 14 && type < 17)
                return type < 17;
        }
    }
    special = magnitude < ((int32_t)(uint32_t)(r_u32(0x80052F70)));
    offset = (special || magnitude / 2 > 0x10000 ? magnitude / 2 - 0x10000 : 0) + 0x10000;
    if (delta >= 0)
        offset = -offset;
    w_u32(object + 924, r_u32(other + 480) + (uint32_t)offset);
    return r_u32(object + 924);
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
    uint32_t state = r_u32(0x8005B2F8);
    uint32_t descriptor = r_u32(object + 1084);
    uint32_t options = r_u32(0x8005AD48);
    uint32_t selector = 0;
    uint32_t enabled;
    uint32_t result;
    FUNCTION_MARKER(0x800BA7FC, "RASHCDG.BIN");
    if (((int32_t)(uint32_t)(r_u32(state + 16))) < 900 || (r_u8(descriptor) & 1))
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
            uint32_t piece = r_u32(object + 340);
            int32_t first = r_s16(piece + 2) * r_s16(object + 872);
            int32_t second = r_s16(piece + 6) * r_s16(object + 876);
            int32_t direction = ((int32_t)(uint32_t)(r_u32(object + 364))) < 0 ? 0x20000 : -0x20000;
            int32_t dot = (((int32_t)(uint32_t)((uint32_t)first << 4)) >> 16) + (((int32_t)(uint32_t)((uint32_t)second << 4)) >> 16);
            magnitude = dot < 0 ? -direction : direction;
        }
        else
        {
            int32_t offset = r_s16(object + 878) * 32;
            magnitude = r_u16(object + 392) == 4 ? ((int32_t)(uint32_t)(r_u32(object + 344) - (uint32_t)offset)) : offset;
        }
        leaf_80093CAC_scalar(object, object + 872, magnitude, 1);
        w_u16(object + 944, 1);
        w_u32(object + 908, 0x10000);
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
            result = value < 8 ? (r_u32(object + 48) >> (4 * value)) & 15 : 0;
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
    result = r_u32(object + 568) & 0x600;
    if (result)
        return result;
    other = r_u32(0x8005B3A0) + 1096 * other_id;
    delta = ((int32_t)(uint32_t)((r_u32(object + 324) - r_u32(other + 324)) << 4));
    if (!sub_800BB448(object, other, delta))
    {
        sub_800BC8DC(object);
        return 0;
    }
    if (((int32_t)(uint32_t)(r_u32(0x80052F64))) < delta)
    {
        uint32_t value = r_u32(object + 924);
        if (r_u16(object + 320) & 2)
            value = r_u32(r_u32(object + 556) + 224);
        w_u32(object + 924, value);
        result = r_u32(object + 564) & 0x80000;
        if (!result)
        {
            w_u32(object + 916, (uint32_t)delta);
            result = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(value)), ((int32_t)(uint32_t)(value)));
            w_u32(object + 920, result);
            w_u32(object + 564, r_u32(object + 564) & 0xFFF7FFFFu);
        }
        return result;
    }
    {
        uint32_t record = object + 940 + 8u * (uint32_t)(int32_t)r_s8(object + 946);
        if (r_u16(record) == 7 && r_u16(record + 2) == other_id)
        {
            uint32_t source = other + 956 + 8u * (uint32_t)((int32_t)r_s8(other + 946) - 1);
            uint16_t type = r_u16(source);
            w_u16(record, type);
            if ((uint16_t)(type - 4) < 2)
            {
                sub_800BC8DC(object);
                return 0;
            }
        }
    }
    w_u32(object + 924, r_u32(other + 924));
    result = r_u32(object + 564) & 0x80000;
    if (!result)
    {
        w_u32(object + 916, r_u32(other + 916));
        w_u32(object + 564, r_u32(object + 564) & 0xFFF7FFFFu);
        w_u32(object + 920, r_u32(other + 920));
        result = r_u32(object + 564);
    }
    return result;
}

static int32_t leaf_rider_clearance(uint32_t rider)
{
    if (!r_u32(rider + 372) || r_u16(rider + 362))
        return 0x39999;
    return (int32_t)((uint32_t)((uint64_t)r_u32(rider + 424) * (uint32_t)(r_s16(rider + 420) + ((((int32_t)(uint32_t)(r_u32(rider + 344))) >> 31) & (r_s16(rider + 408) - r_s16(rider + 420))))));
}

uint32_t sub_800BB448(uint32_t first, uint32_t second, int32_t delta)
{
    int32_t scalar_value_69;
    int32_t scalar_sign_69;

    int32_t clearance;
    uint32_t state;
    uint32_t players;
    uint32_t speed_limit;
    FUNCTION_MARKER(0x800BB448, "RASHCDG.BIN");
    if (!r_s16(second + 320) || r_u32(r_u32(second + 852) + 604) != 1)
        return 0;
    clearance = (int32_t)((scalar_value_69 = (((int32_t)(uint32_t)(r_u32(second + 344))))), (scalar_sign_69 = scalar_value_69 >> 31), (((uint32_t)((uint32_t)((scalar_sign_69 + scalar_value_69) ^ scalar_sign_69))))) - leaf_rider_clearance(second);
    if (delta < 6 * ((int32_t)(uint32_t)(r_u32(first + 308))) || ((int32_t)(uint32_t)(r_u32(0x80052F5C))) < delta || ((int32_t)(uint32_t)(r_u32(0x80052F50))) < clearance)
        return 0;
    state = r_u32(0x8005B2F8);
    players = r_u32(state + 48);
    if (r_u16(first + 172) >= players && (r_u8(r_u32(first + 1084) + 1) & 15) == 2)
        return 1;
    speed_limit = (uint32_t)((int32_t)((uint32_t)((uint64_t)r_u32(0x80053194 + 4 * r_u32(state + 60)) * r_u32(r_u32(first + 556) + 224))) / 128);
    return ((int32_t)(uint32_t)(r_u32(second + 480))) >= ((int32_t)(uint32_t)(speed_limit));
}

uint32_t sub_800BB8FC(uint32_t first, uint32_t second, int32_t delta)
{
    int32_t scalar_value_70;
    int32_t scalar_sign_70;

    int32_t clearance;
    uint32_t players;
    FUNCTION_MARKER(0x800BB8FC, "RASHCDG.BIN");
    if (!r_s16(second + 320) || r_u32(r_u32(second + 852) + 604) != 1)
        return 0;
    clearance = (int32_t)((scalar_value_70 = (((int32_t)(uint32_t)(r_u32(second + 344))))), (scalar_sign_70 = scalar_value_70 >> 31), (((uint32_t)((uint32_t)((scalar_sign_70 + scalar_value_70) ^ scalar_sign_70))))) - leaf_rider_clearance(second);
    if (delta < ((int32_t)(uint32_t)(r_u32(0x80052F54))) || ((int32_t)(uint32_t)(r_u32(0x80052F5C))) < delta || ((int32_t)(uint32_t)(r_u32(0x80052F50))) < clearance)
        return 0;
    if (((int32_t)(uint32_t)(r_u32(r_u32(first + 556) + 224))) >= ((int32_t)(uint32_t)(r_u32(second + 480))))
        return 1;
    players = r_u32(r_u32(0x8005B2F8) + 48);
    return r_u16(second + 172) < players;
}

uint32_t sub_800BBBB8(uint32_t first, uint32_t second, int32_t delta)
{
    int32_t difference;
    FUNCTION_MARKER(0x800BBBB8, "RASHCDG.BIN");
    if (!r_s16(second + 320) || r_u32(r_u32(second + 852) + 604) != 1)
        return 0;
    difference = ((int32_t)(uint32_t)(r_u32(first + 480))) - ((int32_t)(uint32_t)(r_u32(second + 480)));
    if (delta < -((int32_t)(uint32_t)(r_u32(0x80052F60))) || 2 * difference < delta)
        return 0;
    return 1;
}

uint32_t sub_800BBD44(uint32_t first, uint32_t second, int32_t delta)
{
    int32_t scalar_sign_71;
    int32_t scalar_value_72;
    int32_t scalar_sign_72;
    int32_t scalar_value_73;
    int32_t scalar_sign_73;

    uint32_t linked;
    uint32_t distance;
    FUNCTION_MARKER(0x800BBD44, "RASHCDG.BIN");
    if (!r_s16(second + 320) || r_u32(r_u32(second + 852) + 604) != 1)
        return 0;
    if (((scalar_sign_71 = ((int32_t)(delta)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_71 + ((int32_t)(delta))) ^ scalar_sign_71))))) >= (uint32_t)((int32_t)(uint32_t)(r_u32(0x80052F68))))
        return 0;
    distance = ((scalar_value_72 = (((int32_t)(uint32_t)(r_u32(second + 344))) - ((int32_t)(uint32_t)(r_u32(first + 344))))), (scalar_sign_72 = scalar_value_72 >> 31), (((uint32_t)((uint32_t)((scalar_sign_72 + scalar_value_72) ^ scalar_sign_72)))));
    linked = r_u32(second + 856);
    if (linked)
    {
        uint32_t linked_distance = ((scalar_value_73 = (((int32_t)(uint32_t)(r_u32(linked + 344))) - ((int32_t)(uint32_t)(r_u32(first + 344))))), (scalar_sign_73 = scalar_value_73 >> 31), (((uint32_t)((uint32_t)((scalar_sign_73 + scalar_value_73) ^ scalar_sign_73)))));
        if (distance < linked_distance)
            linked_distance = distance;
        distance = linked_distance;
    }
    return distance < (uint32_t)((int32_t)(uint32_t)(r_u32(0x80052F6C)));
}

uint32_t sub_800BBEBC(uint32_t object, uint16_t other_id)
{
    int32_t scalar_value_74;
    int32_t scalar_sign_74;
    int32_t scalar_sign_75;
    int32_t scalar_sign_76;
    int32_t scalar_value_77;
    int32_t scalar_sign_77;
    int32_t scalar_value_78;
    int32_t scalar_sign_78;
    int32_t scalar_value_79;
    int32_t scalar_sign_79;

    uint32_t other;
    uint32_t comparison;
    int32_t delta;
    uint32_t result;
    FUNCTION_MARKER(0x800BBEBC, "RASHCDG.BIN");
    if (other_id == 224)
        return 224;
    result = r_u32(object + 568) & 0x7FF;
    if (result)
        return result;
    other = r_u32(0x8005B3A0) + 1096 * other_id;
    comparison = other;
    if (r_u32(r_u32(other + 852) + 604) >= 3)
        comparison = r_u32(other + 852);
    if (r_u32(comparison + 360) == r_u32(object + 360))
        delta = ((int32_t)(uint32_t)(((scalar_value_74 = (((int32_t)(uint32_t)(r_u32(comparison + 368) - r_u32(object + 368))))), (scalar_sign_74 = scalar_value_74 >> 31), (((uint32_t)((uint32_t)((scalar_sign_74 + scalar_value_74) ^ scalar_sign_74)))))));
    else
    {
        int32_t dx = ((int32_t)(uint32_t)(r_u32(comparison + 184) - r_u32(object + 184))) >> 16;
        int32_t dz = ((int32_t)(uint32_t)(r_u32(comparison + 192) - r_u32(object + 192))) >> 16;
        int32_t maximum = ((int32_t)(uint32_t)(((scalar_sign_75 = ((int32_t)(dx)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_75 + ((int32_t)(dx))) ^ scalar_sign_75)))))));
        int32_t minimum = ((int32_t)(uint32_t)(((scalar_sign_76 = ((int32_t)(dz)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_76 + ((int32_t)(dz))) ^ scalar_sign_76)))))));
        int32_t mixed;
        if (maximum < minimum)
        {
            int32_t swap = maximum;
            maximum = minimum;
            minimum = swap;
        }
        mixed = minimum + (minimum >> 1);
        delta = ((int32_t)(uint32_t)((uint32_t)(maximum - (maximum >> 5) - (maximum >> 7) + (mixed >> 2) + (mixed >> 6)) << 16));
    }
    if (!sub_800BC120(object, other, delta))
    {
        sub_800BC8DC(object);
        return 0;
    }
    {
        int32_t first = ((int32_t)(uint32_t)(r_u32(comparison + 344)));
        int32_t second = ((int32_t)(uint32_t)(r_u32(object + 344)));
        int32_t side = ((int32_t)(uint32_t)(r_u32(comparison + 364) ^ r_u32(object + 364))) < 0 ? ((int32_t)(uint32_t)((uint32_t)first + (uint32_t)second)) : ((int32_t)(uint32_t)((uint32_t)first - (uint32_t)second));
        int32_t scale = side >= 0 ? ((int32_t)(uint32_t)(0u - r_u32(0x80052F58))) : ((int32_t)(uint32_t)(r_u32(0x80052F58)));
        int32_t vector[3];
        uint32_t i;
        uint32_t magnitude;
        uint32_t sum = 0;
        (void)sub_8002EAD8(xport_guest_ptr(comparison + 184, 12), xport_guest_ptr(comparison + 432, 6), (uint32_t)scale, xport_guest_ptr(object + 880, 12));
        for (i = 0; i < 3; ++i)
            vector[i] = ((int32_t)(uint32_t)(r_u32(object + 880 + 4 * i) - r_u32(object + 504 + 4 * i)));
        if (((scalar_value_77 = (vector[0])), (scalar_sign_77 = scalar_value_77 >> 31), (((uint32_t)((uint32_t)((scalar_sign_77 + scalar_value_77) ^ scalar_sign_77))))) > 0x5A8000 || ((scalar_value_78 = (vector[1])), (scalar_sign_78 = scalar_value_78 >> 31), (((uint32_t)((uint32_t)((scalar_sign_78 + scalar_value_78) ^ scalar_sign_78))))) > 0x5A8000 || ((scalar_value_79 = (vector[2])), (scalar_sign_79 = scalar_value_79 >> 31), (((uint32_t)((uint32_t)((scalar_sign_79 + scalar_value_79) ^ scalar_sign_79))))) > 0x5A8000)
            magnitude = 0x7FFF0000;
        else
        {
            for (i = 0; i < 3; ++i)
                sum += (uint32_t)((uint64_t)((int64_t)vector[i] * vector[i]) >> 16);
            magnitude = sub_8004CF74(sum) << 2;
        }
        result = r_u32(object + 564) & 0x80000;
        if (!result)
        {
            w_u32(object + 916, magnitude);
            w_u32(object + 920, 0);
            result = r_u32(object + 564) & 0xFFF7FFFFu;
            w_u32(object + 564, result);
        }
    }
    return result;
}

uint32_t sub_800BC618(uint32_t actor, uint16_t other_id, int32_t delta)
{
    int32_t scalar_value_80;
    int32_t scalar_sign_80;

    uint32_t state = r_u32(0x8005B2F8);
    uint32_t actor_id = r_u16(actor + 172);
    uint32_t other = 0;
    uint32_t players = r_u32(state + 48);
    FUNCTION_MARKER(0x800BC618, "RASHCDG.BIN");
    if (actor_id < players && r_u16(0x800CCAC0 + 2 * actor_id))
    {
        uint32_t mask = r_u16(0x800CCAC0 + 2 * actor_id) & (0xFFFFu - (1u << ((other_id - 1) & 31)));
        if (mask && !(mask & (mask - 1)))
        {
            uint32_t bit = 0;
            while (!((mask >> bit) & 1))
                ++bit;
            other = r_u32(0x8005B3A0) + 1096 * (bit + 1);
        }
    }
    else
    {
        uint32_t record = actor + 956 + 8 * (uint32_t)r_s8(actor + 946);
        uint32_t type = r_u16(record - 8);
        uint32_t id = r_u16(record - 6);
        if ((type == 6 || type == 7 || (type >= 14 && type < 18)) && id != other_id)
            other = r_u32(0x8005B3A0) + 1096 * id;
    }
    if (!other || ((scalar_value_80 = (16 * (((int32_t)(uint32_t)(r_u32(actor + 324))) - ((int32_t)(uint32_t)(r_u32(other + 324)))))), (scalar_sign_80 = scalar_value_80 >> 31), (((uint32_t)((uint32_t)((scalar_sign_80 + scalar_value_80) ^ scalar_sign_80))))) > 0x7FFFF)
        return 0;
    {
        int32_t other_x = ((int32_t)(uint32_t)(r_u32(other + 344)));
        int32_t actor_x = ((int32_t)(uint32_t)(r_u32(actor + 344)));
        int32_t direction = (((int32_t)(uint32_t)(r_u32(other + 364))) ^ ((int32_t)(uint32_t)(r_u32(actor + 364)))) >= 0 ? other_x - actor_x : other_x + actor_x;
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
    result = r_u32(object + 568) & 0x600;
    if (result)
        return result;
    other = r_u32(0x8005B3A0) + 1096 * other_id;
    delta = ((int32_t)(uint32_t)((r_u32(object + 324) - r_u32(other + 324)) << 4));
    if (!sub_800BBBB8(object, other, delta))
    {
        if (delta < -((int32_t)(uint32_t)(r_u32(0x80052F60))))
        {
            uint32_t state = r_u32(object + 1084);
            uint32_t other_state = r_u32(other + 1084);
            xport_update_u8(state + 68, XPORT_MEMORY_UPDATE_OR, 0x10);
            xport_update_u8(other_state + 68, XPORT_MEMORY_UPDATE_OR, 8);
        }
        sub_800BC8DC(object);
        return 0;
    }
    {
        int32_t other_x = ((int32_t)(uint32_t)(r_u32(other + 344)));
        int32_t object_x = ((int32_t)(uint32_t)(r_u32(object + 344)));
        int32_t lateral = ((int32_t)(uint32_t)(r_u32(other + 364) ^ r_u32(object + 364))) < 0 ? ((int32_t)(uint32_t)((uint32_t)other_x + (uint32_t)object_x)) : ((int32_t)(uint32_t)((uint32_t)other_x - (uint32_t)object_x));
        int32_t response = lateral < 0 ? ((int32_t)(uint32_t)(r_u32(0x80052F58))) : ((int32_t)(uint32_t)(0u - r_u32(0x80052F58)));
        response = ((int32_t)(uint32_t)((uint32_t)response << sub_800BC618(other, r_u16(object + 172), response)));
        sub_800BC4FC(object, other, response);
    }
    result = r_u32(object + 924);
    if (r_u16(object + 320) & 2)
    {
        int32_t total = ((int32_t)(uint32_t)(r_u32(r_u32(object + 556) + 224) + r_u32(object + 480)));
        result = (uint32_t)(total / 2);
    }
    w_u32(object + 924, result);
    w_u32(object + 916, 0);
    w_u32(object + 920, 0);
    result = r_u32(object + 564) & 0xFFF7FFFFu;
    w_u32(object + 564, result);
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
    result = r_u32(object + 568) & 0x600;
    if (result)
        return result;
    other = r_u32(0x8005B3A0) + 1096 * other_id;
    delta = ((int32_t)(uint32_t)((r_u32(object + 324) - r_u32(other + 324)) << 4));
    if (!sub_800BBD44(object, other, delta))
    {
        sub_800BC8DC(object);
        return 0;
    }
    {
        int32_t other_x = ((int32_t)(uint32_t)(r_u32(other + 344)));
        int32_t object_x = ((int32_t)(uint32_t)(r_u32(object + 344)));
        int32_t lateral = ((int32_t)(uint32_t)(r_u32(other + 364) ^ r_u32(object + 364))) < 0 ? ((int32_t)(uint32_t)((uint32_t)other_x + (uint32_t)object_x)) : ((int32_t)(uint32_t)((uint32_t)other_x - (uint32_t)object_x));
        int32_t response = ((int32_t)(uint32_t)(r_u32(0x80052F6C) + 0x10000u));
        if (lateral > 0)
            response = ((int32_t)(uint32_t)(0u - (uint32_t)response));
        sub_800BC4FC(object, other, response);
    }
    result = r_u32(object + 924);
    if (r_u16(object + 320) & 2)
        result = r_u32(r_u32(object + 556) + 224);
    w_u32(object + 924, result);
    w_u32(object + 916, 0);
    w_u32(object + 920, 0);
    result = r_u32(object + 564) & 0xFFF7FFFFu;
    w_u32(object + 564, result);
    return result;
}

uint32_t sub_800BC120(uint32_t object, uint32_t other, int32_t delta)
{
    int32_t scalar_sign_81;

    uint32_t state;
    uint32_t index;
    int32_t magnitude;
    FUNCTION_MARKER(0x800BC120, "RASHCDG.BIN");
    if (!r_s16(other + 320))
        return 0;
    if (r_u32(r_u32(other + 852) + 604) < 3 && ((int32_t)(uint32_t)(r_u32(other + 480))) > 585944)
        return 0;
    magnitude = ((int32_t)(uint32_t)(((scalar_sign_81 = ((int32_t)(delta)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_81 + ((int32_t)(delta))) ^ scalar_sign_81)))))));
    if (((int32_t)(uint32_t)(r_u32(0x80052F5C) << 1)) < magnitude)
        return 0;
    if (((int32_t)(uint32_t)(r_u32(object + 480))) > 585944)
        return 1;
    state = r_u32(0x8005B2F8);
    index = r_u32(state + 60) + ((0u - (r_u8(state + 4) & 1u)) & 3u);
    return ((int32_t)(uint32_t)(r_u32(0x800530A8 + 4 * index))) >= magnitude;
}

uint32_t sub_800BD388(uint32_t object)
{
    int32_t scalar_sign_82;

    uint32_t flags;
    int32_t speed;
    FUNCTION_MARKER(0x800BD388, "RASHCDG.BIN");
    if (!(r_u32(object + 560) & 0x08000000))
        return 0;
    flags = r_u32(object + 388);
    if (!(flags & 1))
        return 0;
    if (((int32_t)(uint32_t)(r_u32(object + 576))) <= 147455)
    {
        uint32_t piece = r_u32(object + 340);
        sub_8002EAD8(xport_guest_ptr(piece + 20, 12), xport_guest_ptr(piece + 14, 6), r_u32(object + 348), xport_guest_ptr(object + 880, 12));
        w_u16(object + 878, 0);
        xport_update_u8(object + 928, XPORT_MEMORY_UPDATE_OR, 0x40);
        return 1;
    }
    if (!(flags & 0x10))
        return 0;
    speed = ((int32_t)(uint32_t)(r_u32(object + 344)));
    {
        int32_t threshold = ((int32_t)(uint32_t)((flags >> 8 & 0xFFFu) << 16));
        int32_t magnitude = ((int32_t)(uint32_t)(((scalar_sign_82 = ((int32_t)(speed)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_82 + ((int32_t)(speed))) ^ scalar_sign_82)))))));
        int32_t response;
        if (threshold < 0)
            threshold += 7;
        threshold >>= 3;
        if (threshold >= magnitude || magnitude >= threshold + 0x50000)
            return 0;
        response = threshold + 0x10000;
        if (speed < 0)
            response = ((int32_t)(uint32_t)(0u - (uint32_t)response));
        response = ((int32_t)(uint32_t)((uint32_t)response - ((uint32_t)(int32_t)r_s16(object + 878) << 5)));
        sub_8002EAD8(xport_guest_ptr(object + 880, 12), xport_guest_ptr(object + 872, 6), (uint32_t)response, xport_guest_ptr(object + 880, 12));
        w_u16(object + 944, 0);
        w_u16(object + 878, (int16_t)(r_s16(object + 878) + (response >> 5)));
        w_u32(object + 924, (8 * r_u32(r_u32(0x8005B2F8) + 60) + 24) << 16);
    }
    return 1;
}

uint32_t sub_800BC1EC(uint32_t object, uint32_t other, int32_t delta)
{
    int32_t scalar_sign_83;
    int32_t scalar_sign_84;
    int32_t scalar_value_85;
    int32_t scalar_sign_85;

    uint32_t state = r_u32(0x8005B2F8);
    uint32_t other_id = r_u16(other + 172);
    uint32_t players = r_u32(state + 48);
    uint32_t other_class;
    int32_t separation;
    int32_t limit;
    FUNCTION_MARKER(0x800BC1EC, "RASHCDG.BIN");
    if (!r_s16(other + 320))
        return 0;
    other_class = r_u32(r_u32(other + 852) + 604);
    if (other_class >= 2 || (other_id >= players && other_class != 1))
        return 0;
    if (r_u8(state + 57) == 3 && other_id < players)
        return 1;
    limit = ((int32_t)(uint32_t)(r_u32(0x80052F84)));
    if (other_id < players)
        limit = ((int32_t)(uint32_t)((uint32_t)limit << 2));
    if (r_u32(other + 360) == r_u32(object + 360) && (!(r_u32(other + 360) >> 16) || r_u32(other + 336) == r_u32(object + 336)))
    {
        separation = ((int32_t)(uint32_t)(r_u32(other + 344) - r_u32(object + 344)));
        if (((int32_t)(uint32_t)(r_u32(object + 364))) < 0)
            separation = ((int32_t)(uint32_t)(0u - (uint32_t)separation));
    }
    else
        separation = ((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(other + 184, 12), xport_guest_ptr(object + 432, 6), xport_guest_ptr(object + 184, 12))));
    if (r_u32(other + 856) && separation < 0)
        separation = ((int32_t)(uint32_t)((uint32_t)separation + r_u32(other + 304) + r_u32(r_u32(other + 856) + 304)));
    if (((int32_t)(uint32_t)(((scalar_sign_83 = ((int32_t)(separation)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_83 + ((int32_t)(separation))) ^ scalar_sign_83))))))) > limit)
        return 0;
    limit = ((int32_t)(uint32_t)(r_u32(0x80052F80)));
    if (other_id < players)
        limit = ((int32_t)(uint32_t)((uint32_t)limit << 2));
    if (((int32_t)(uint32_t)(((scalar_sign_84 = ((int32_t)(delta)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_84 + ((int32_t)(delta))) ^ scalar_sign_84))))))) > limit)
        return 0;
    if (r_u32(object + 560) & 0x08000000)
    {
        uint32_t type = r_u8(r_u32(object + 1084) + 1) & 15;
        int32_t product = (int32_t)((uint32_t)r_u8(0x80052EE4 + 36 * type + 18) * r_u32(r_u32(object + 556) + 224));
        int32_t speed_limit = product < 0 ? (product + 127) >> 7 : product >> 7;
        if (((int32_t)(uint32_t)(r_u32(other + 480))) < speed_limit)
            return 0;
    }
    if (!(r_u32(object + 560) & 0x08000000) || r_u16(other + 362) == 1)
        return 1;
    return ((int32_t)(uint32_t)(r_u32(0x80052F50))) >= ((int32_t)(uint32_t)(((scalar_value_85 = (((int32_t)(uint32_t)(r_u32(other + 344))))), (scalar_sign_85 = scalar_value_85 >> 31), (((uint32_t)((uint32_t)((scalar_sign_85 + scalar_value_85) ^ scalar_sign_85))))) - (uint32_t)leaf_rider_clearance(other)));
}

uint32_t sub_800BF424(uint32_t object, uint32_t value)
{
    uint32_t state = r_u32(0x8005B2F8);
    uint32_t actor_id = r_u16(object + 172);
    uint32_t index;
    uint32_t record;
    FUNCTION_MARKER(0x800BF424, "RASHCDG.BIN");
    if (actor_id < r_u32(state + 48))
        index = actor_id;
    else
    {
        uint32_t existing = r_u32(object + 1088);
        if (existing)
            return existing;
        index = r_u16(r_u32(object + 856) + 172) + 2;
    }
    record = 0x800CD548 + 12 * index;
    w_u32(record, object);
    w_u32(record + 4, value);
    w_u32(record + 8, r_u32(state + 16));
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
        w_u32(object + 720, (uint32_t)r_u8(record) << 8);
        w_u32(object + 564, r_u32(object + 564) | 0x01000000u);
        result = magnitude + ((0u - direction) & (0u - 2 * magnitude));
        w_u32(object + 656, result);
    }
    return result;
}

uint32_t sub_800BF860(uint32_t object, uint32_t state, uint32_t alternate, uint32_t value_out)
{
    uint32_t first = r_u8(object + 569);
    uint32_t second = r_u8(object + r_u8(object + 570) + 574);
    uint32_t table = r_u32(r_u32(0x8005AD4C) + 12 * first + 8) + 12 * second;
    uint32_t result;
    FUNCTION_MARKER(0x800BF860, "RASHCDG.BIN");
    if (alternate)
    {
        w_u32(value_out, r_u16(table + 8));
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
    int32_t product = ((int32_t)(uint32_t)((uint32_t)value * 150u));
    int32_t level = product < 0 ? (product + 127) >> 7 : product >> 7;
    int32_t index;
    uint32_t clamped = level < 0 ? 0 : level > 255 ? 255u : (uint32_t)level;
    FUNCTION_MARKER(0x800BFD74, "RASHCDG.BIN");
    if (r_u32(object + 1088))
        index = r_u16(object + 172);
    else if (!(r_u8(r_u32(object + 852) + 572) & 0x40))
        index = (int32_t)r_u16(r_u32(object + 856) + 172) + 1 + (r_u32(r_u32(0x8005B2F8) + 48) >= 2);
    else
        index = -1;
    if (index >= 0)
    {
        product = ((int32_t)(uint32_t)((uint32_t)value * 100u));
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
    FUNCTION_MARKER(0x800BC4FC, "RASHCDG.BIN");
    if ((r_u16(other + 172) >> 5) == 0)
    {
        linked = r_u32(other + 856);
        direction = ((int32_t)(uint32_t)(r_u32(other + 364)));
        if (linked && ((direction >= 0) != (delta < 0)))
        {
            uint32_t radius = r_u32(other + 304) + r_u32(linked + 304);
            delta = direction < 0 ? ((int32_t)(uint32_t)((uint32_t)delta - radius)) : ((int32_t)(uint32_t)((uint32_t)delta + radius));
        }
    }
    vector = r_u32(other + 340);
    first_product = (int32_t)((uint32_t)(int32_t)r_s16(vector + 2) * (uint32_t)(int32_t)r_s16(object + 872));
    second_product = (int32_t)((uint32_t)(int32_t)r_s16(vector + 6) * (uint32_t)(int32_t)r_s16(object + 876));
    delta = ((int32_t)(uint32_t)((uint32_t)delta + r_u32(other + 344) - ((uint32_t)(int32_t)r_s16(object + 878) << 5)));
    first_product = (int32_t)((uint32_t)first_product << 4) >> 16;
    second_product = (int32_t)((uint32_t)second_product << 4) >> 16;
    if (((int32_t)(uint32_t)((uint32_t)first_product + (uint32_t)second_product)) < 0)
        delta = ((int32_t)(uint32_t)(0u - (uint32_t)delta));
    leaf_80093CAC_scalar(object, object + 872, delta, 1);
}

uint32_t sub_800C0BE8(uint32_t object, uint32_t other, int32_t delta, uint32_t flags)
{
    uint32_t target;
    FUNCTION_MARKER(0x800C0BE8, "RASHCDG.BIN");
    if (!(flags & 1))
    {
        if (r_u32(object + 1088) && sub_800BB8FC(object, other, ((int32_t)(uint32_t)(0u - (uint32_t)delta))))
        {
            uint32_t event = 0x1F8003F8;
            w_u16(event, 6);
            w_u16(event + 2, r_u16(other + 172));
            sub_800BCA68(xport_guest_ptr(event, 8), 2, object);
            return 0;
        }
    }
    else
    {
        uint32_t relation = flags & 2 ? r_u32(r_u32(other + 856) + 1084) : r_u32(other + 1084);
        uint32_t occupied = r_u8(relation + 15);
        uint32_t packed = r_u8(r_u32(object + 1084) + 61);
        if ((packed & 15) < (packed >> 4) && occupied)
            return 1;
    }
    target = r_u32(object + 852);
    if (r_u8(target + 572) & 0x40)
        sub_800C4550(77, target, 1);
    sub_800BC8DC(object);
    return 0;
}

uint32_t sub_800C0CE8(uint32_t object, uint32_t other, int32_t delta)
{
    int32_t scalar_sign_86;
    int32_t scalar_sign_87;
    int32_t scalar_sign_88;

    int32_t position_delta = ((int32_t)(uint32_t)(r_u32(other + 480) - r_u32(object + 480)));
    int32_t threshold = ((int32_t)(uint32_t)(((scalar_sign_86 = ((int32_t)(position_delta)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_86 + ((int32_t)(position_delta))) ^ scalar_sign_86))))) * 4u));
    int32_t magnitude = ((int32_t)(uint32_t)(((scalar_sign_87 = ((int32_t)(delta)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_87 + ((int32_t)(delta))) ^ scalar_sign_87)))))));
    FUNCTION_MARKER(0x800C0CE8, "RASHCDG.BIN");
    if (threshold < ((int32_t)(uint32_t)(r_u32(0x80052F9C))))
        threshold = ((int32_t)(uint32_t)(r_u32(0x80052F9C)));
    if (magnitude < threshold)
    {
        int32_t other_x = ((int32_t)(uint32_t)(r_u32(other + 344)));
        int32_t object_x = ((int32_t)(uint32_t)(r_u32(object + 344)));
        int32_t lateral = ((int32_t)(uint32_t)(r_u32(other + 364) ^ r_u32(object + 364))) < 0 ? ((int32_t)(uint32_t)((uint32_t)other_x + (uint32_t)object_x)) : ((int32_t)(uint32_t)((uint32_t)other_x - (uint32_t)object_x));
        uint32_t kind = 10;
        int32_t response;
        if (magnitude <= ((int32_t)(uint32_t)(r_u32(0x80052F70))))
        {
            kind = ((scalar_sign_88 = ((int32_t)(lateral)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_88 + ((int32_t)(lateral))) ^ scalar_sign_88))))) < r_u32(0x80052F74) ? 9 : 11;
        }
        if (kind == 11 && r_u16(other + 172) < r_u32(r_u32(0x8005B2F8) + 48))
            sub_8001A760(r_u16(object + 172), 0);
        response = ((int32_t)(uint32_t)(r_u32(0x80052F50 + kind * 4)));
        if (lateral > 0)
            response = ((int32_t)(uint32_t)(0u - (uint32_t)response));
        if (sub_800BC618(other, r_u16(object + 172), response))
            response = ((int32_t)(uint32_t)(0u - (uint32_t)response));
        sub_800BC4FC(object, other, response);
    }
    sub_800C0E98(object, other, delta);
    w_u32(object + 916, 0);
    w_u32(object + 920, 0);
    w_u32(object + 564, r_u32(object + 564) & 0xFFF7FFFFu);
    return r_u32(object + 564);
}

uint32_t sub_800C09B0(uint32_t object)
{
    uint32_t body, index, animation, action = 0;
    FUNCTION_MARKER(0x800C09B0, "RASHCDG.BIN");
    body = r_u32(object + 852);
    index = r_u8(body + 570);
    animation = r_u8(body + 574 + index);
    if (animation == 63)
    {
        if (sub_8005BE58(r_u32(body + 540)))
            action = 8;
    }
    else if (sub_8005BE58(r_u32(body + 540)))
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
        uint32_t groups = r_u32(0x8005AD4C);
        uint32_t animation_object = r_u32(body + 540);
        uint32_t table = r_u32(groups + 12u * group_index + 8);
        int32_t frame = ((int32_t)(uint32_t)(r_u32(animation_object + 16)));
        uint32_t limit = r_u8(table + 12u * animation + 11);
        action = frame >= (int32_t)limit;
        w_u8(body + 546, (uint8_t)action);
    }
    if (action & 14u)
    {
        uint32_t flags = r_u32(body + 552) & 0x08000000u ? 0x110 : 0x10;
        uint32_t group_index = r_u8(body + 569);
        uint32_t group = r_u32(0x8005AD4C) + 12u * group_index;
        uint32_t type;
        if (action & 2u)
            type = r_u16(r_u32(group + 8) + 12u * animation);
        else if (action & 4u)
        {
            type = r_u16(r_u32(group + 8) + 12u * animation + 2);
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

uint32_t sub_800C159C(uint32_t first, uint32_t second, int32_t direction, int32_t delta, uint32_t *flags_out)
{
    int32_t scalar_value_89;
    int32_t scalar_sign_89;
    int32_t scalar_sign_90;
    int32_t scalar_sign_91;

    uint32_t body = r_u32(first + 852);
    uint32_t group = r_u32(0x8005AD4C) + 12 * r_u8(body + 569);
    uint32_t animation = r_u8(body + 574 + r_u8(body + 570));
    uint32_t table = r_u32(group + 8);
    uint32_t distance = ((scalar_value_89 = (((int32_t)(uint32_t)(r_u32(second + 188) - r_u32(first + 188))))), (scalar_sign_89 = scalar_value_89 >> 31), (((uint32_t)((uint32_t)((scalar_sign_89 + scalar_value_89) ^ scalar_sign_89)))));
    uint32_t flags = xport_load_le32(flags_out);
    uint32_t active = (r_u16(table + 12 * animation + 8) && ((int32_t)(uint32_t)(distance)) < ((int32_t)(uint32_t)(r_u32(first + 312)))) || ((flags >> 8) & 1);
    FUNCTION_MARKER(0x800C159C, "RASHCDG.BIN");
    if (active && ((scalar_sign_90 = ((int32_t)(delta)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_90 + ((int32_t)(delta))) ^ scalar_sign_90))))) <= 65534)
    {
        int32_t phase = ((int32_t)(uint32_t)(r_u32(first + 636) - r_u32(second + 636)));
        int32_t second_limit = ((int32_t)(uint32_t)(r_u32(second + 312)));
        if (direction > 0)
            phase = ((int32_t)(uint32_t)(0u - (uint32_t)phase));
        if (((int32_t)(uint32_t)(distance)) < second_limit / 2)
        {
            uint32_t curve_index = ((uint32_t)phase * 163u >> 12) & 0x3FFCu;
            int32_t curve = r_s16(0x8005624C + curve_index) * 16;
            int32_t scaled = ((int32_t)(uint32_t)((uint32_t)sub_8001FC90(curve, ((int32_t)(uint32_t)(r_u32(second + 188) - r_u32(first + 188))) + 3 * second_limit / 4)));
            uint32_t threshold = (uint32_t)scaled + ((uint32_t)r_u8(table + 12 * animation + 10) << 12);
            threshold += r_u32(r_u32(first + 1084) + 8) + r_u32(first + 304) + r_u32(second + 304);
            if (((int32_t)(uint32_t)(((scalar_sign_91 = ((int32_t)(direction)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_91 + ((int32_t)(direction))) ^ scalar_sign_91))))))) < ((int32_t)(uint32_t)(threshold)))
                flags |= 1;
        }
    }
    flags |= 2;
    xport_store_le32(flags_out, flags);
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
    context = r_u32(object + 540);
    frame = ((int32_t)(uint32_t)(r_u32(context + 16) << 2));
    {
        uint32_t index = r_u32(context + 12);
        uint32_t selector = r_u8(r_u32(context + 4) + 12 * index);
        uint32_t table = r_u32(r_u32(context + 40) + 4);
        boundary = (int16_t)(r_u16(r_u32(table + 4 * selector) + 16) - 1);
    }
    if (boundary < frame && frame < 3 * boundary && (!(r_u8(object + 572) & 0x20) || !event || event == 2 || event == 6 || event == 7))
        return 1;
    return 0;
}

void sub_800C92F8(void)
{
    uint32_t object = r_u32(0x8005B38C);
    uint32_t marker = r_u32(0x8005B2E8);
    uint32_t flags = r_u32(object + 560);
    int32_t side = ((int32_t)(uint32_t)(r_u32(object + 364) ^ marker));
    FUNCTION_MARKER(0x800C92F8, "RASHCDG.BIN");
    if (side < 0 && !(flags & 0x08000000u))
    {
        int32_t dot = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(r_u32(object + 340) + 14, 6), xport_guest_ptr(object + 528, 6))));
        if (dot < -46333)
        {
            w_u32(object + 704, 0);
            w_u32(object + 560, flags | 0x28000000u);
            w_u32(object + 564, r_u32(object + 564) & 0xFFFFFDFFu);
        }
        return;
    }
    if (side >= 0 && (flags & 0x08000000u))
    {
        int32_t boundary = ((int32_t)(uint32_t)((uint32_t)(r_u16(0x8005317A) - 25u) << 16));
        if (((int32_t)(uint32_t)(r_u32(object + 368))) < boundary)
        {
            int32_t dot = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(r_u32(object + 340) + 14, 6), xport_guest_ptr(object + 528, 6))));
            if (dot > 62914)
                w_u32(object + 560, flags & 0xD7FFFFFFu);
        }
    }
}

uint32_t sub_800C2348(uint32_t input, uint32_t entity)
{
    uint32_t state = r_u32(entity + 1084);
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
        sub_80017BA0(((int32_t)(uint32_t)(r_u32(entity + 184))), ((int32_t)(uint32_t)(r_u32(entity + 192))), 78, 0);
    }
    if (changed)
    {
        (uint32_t)sub_800C1DD4(entity);
        w_u8(r_u32(entity + 852) + 573, 0);
    }
    return changed;
}

uint32_t sub_800116B8(void)
{
    FUNCTION_MARKER(0x800116B8, "SLUS_010.53");
    sub_80043DD4();
    rrj_missing_init_card(0u);
    rrj_missing_card_auto(0u);
    rrj_missing_start_card();
    (uint32_t)_bu_init();
    sub_80043DF4();
    return (uint32_t)ChangeClearPAD((uint32)(0));
}

uint32_t sub_80011708(void)
{
    uint32_t result;
    FUNCTION_MARKER(0x80011708, "SLUS_010.53");
    sub_80043DD4();
    result = rrj_missing_stop_card();
    sub_80043DF4();
    return result;
}

uint32_t sub_80011738(void)
{
    uint32_t state = 0x800D5D38;
    FUNCTION_MARKER(0x80011738, "SLUS_010.53");
    w_u32(0x8005B2F8, state);
    w_u8(state + 3, 1);
    w_u8(state + 4, 34);
    w_u32(state + 64, 4);
    w_u32(state + 76, 9);
    w_u8(state + 5, 3);
    w_u8(state + 7, 3);
    w_u32(state + 12, 0);
    w_u16(state + 40, 0);
    w_u16(state + 42, 0);
    w_u32(state + 44, 0);
    w_u16(state + 58, 0);
    w_u32(state + 60, 0);
    w_u32(state + 68, 0);
    w_u8(state + 56, 0);
    w_u32(state + 48, 1);
    w_u32(state + 72, 0);
    w_u32(state + 88, 0);
    w_u32(state + 92, 0);
    w_u32(state + 96, 1);
    w_u8(state + 6, 1);
    w_u16(state + 8, 120);
    return state;
}

uint32_t sub_800117BC(void)
{
    uint32_t state;
    uint32_t result;
    FUNCTION_MARKER(0x800117BC, "SLUS_010.53");
    sub_80011738();
    state = r_u32(0x8005B2F8);
    w_u8(state, 2);
    sub_8001C004();
    result = sub_80014528(0x80052318);
    if (((int32_t)(uint32_t)(result)) < 0)
        RRJ_DEBUG_PRINTF((const char *)xport_guest_cptr(0x80010744, 22));
    w_u32(0x8005AD5C, 0);
    sub_8001BFA8();
    sub_8001B494();
    sub_8001BC54();
    state = r_u32(0x8005B2F8);
    if (r_s8(state) == 2)
        sub_80018C1C(0);
    sub_800116B8();
    sub_8001C590();
    sub_800163D4();
    sub_8001BDB0();
    sub_8001462C();
    sub_80018C1C(1);
    sub_8001DFE4();
    return sub_800229B0();
}

uint32_t sub_800118A0(uint32_t mode, uint32_t buffer)
{
    char path[128];
    uint32_t result_out;
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
        w_u32(0x8005ACA8, 1);
    sprintf(path, (const char *)xport_guest_cptr(0x8005AC94u, 1), (const char *)xport_guest_cptr(source, 1));
    while (((int32_t)(uint32_t)(sub_80014680(path, r_u32(0x8005AD5C), xport_guest_ptr(buffer, 1), &result_out))) < 0)
        (void)RRJ_DEBUG_PRINTF((const char *)xport_guest_cptr(0x8001078C, 62), path, (unsigned int)buffer, (long)((int32_t)(uint32_t)(result_out)));
    sub_80043DD4();
    FlushCache();
    sub_80043DF4();
    w_u32(0x8005ACA8, r_u32(0x8005ACA8) | mode);
    return r_u32(0x8005ACA8);
}

uint32_t sub_800119C0(void)
{
    int32_t remaining = ((int32_t)(uint32_t)(r_u32(r_u32(0x800CE4DC))));
    uint32_t entity = r_u32(0x800CE4D0);
    uint32_t state;
    uint32_t result;
    FUNCTION_MARKER(0x800119C0, "SLUS_010.53");
    while (remaining >= 0)
    {
        uint32_t object = r_u32(entity + 0x354);
        uint32_t animation = sub_80012884(0x800CE170, object);
        uint32_t kind;
        w_u32(object + 0x21C, animation);
        if (r_u32(0x800CE190))
            sub_80012858(animation, r_u32(0x800CE190));
        (uint32_t)sub_800C4550((r_u16(object + 0xAC) & 1) ? 4 : 6, object, 1);
        if (r_s16(entity + 0x140))
        {
            (uint32_t)sub_8008B99C(entity);
            (uint32_t)sub_8008BA18(entity);
        }
        else
        {
            sub_80093F94(entity, 1);
        }
        kind = r_u32(entity + 0xB4);
        if (kind - 6 < 3 || kind - 15 < 3)
        {
            uint32_t secondary = r_u32(r_u32(entity + 0x358) + 0x354);
            animation = sub_80012884(0x800CE170, secondary);
            w_u32(secondary + 0x21C, animation);
            if (r_u32(0x800CE1A8))
                sub_80012858(animation, r_u32(0x800CE1A8));
            if (r_u8(secondary + 0x23C) & 0x20)
                (uint32_t)sub_800C4550(77, secondary, 1);
        }
        --remaining;
        entity += r_u32(0x800CE4D4);
    }
    (void)sub_800A41EC();
    (uint32_t)sub_800A4774(0);
    (uint32_t)sub_8008D89C();
    (uint32_t)sub_8009C308();
    state = r_u32(0x8005B2F8);
    if (!(r_u8(state + 4) & 1))
        sub_800901A8(1u);
    if (r_u32(0x8005B220))
    {
        uint32_t object = r_u32(0x8005B38C);
        w_u32(object + 0x230, r_u32(object + 0x230) | 0x08000000);
        sub_80012764(0);
    }
    sub_8001654C();
    if (r_u32(r_u32(0x8005B2F8) + 0x30) == 1)
    {
        while (r_u32(0x800CD694) == 2)
        {
            (uint32_t)sub_80030608();
            sub_800247E8();
            /* Deliver real timed interrupts while the original resource owner waits */
            rrj_native_wait_events();
        }
    }
    else
    {
        sub_800164B4();
    }
    sub_8001A424();
    result = r_u32(0x8005B220);
    if (result)
        return sub_80018C1C(0);
    return result;
}

uint32_t sub_800121E4(void)
{
    FUNCTION_MARKER(0x800121E4, "SLUS_010.53");
    sub_80020228();
    sub_80011708();
    sub_8001643C();
    sub_800229F4(2);
    return sub_8001B4C0();
}

uint32_t sub_80012224(void)
{
    uint32_t state;
    int32_t mode;
    FUNCTION_MARKER(0x80012224, "SLUS_010.53");
    sub_800403CC();
    sub_800140E8();
    sub_8001444C();
    sub_800117BC();
    sub_8002E080();
    state = r_u32(0x8005B2F8);
    if (r_s8(state) == 2)
    {
        sub_800118A0(16, r_u32(0x80010E34));
        (void)sub_8007FEDC();
    }
    while (r_s8(state = r_u32(0x8005B2F8)))
    {
        if (r_s8(state) == 2)
        {
            sub_800118A0(16, r_u32(0x80010E34));
            sub_8001B868();
            (void)sub_8007FF4C();
        }
        sub_800118A0(2, r_u32(0x80010E34));
        (void)sub_80064610();
        do
        {
            (void)sub_80063B90();
            sub_800118A0(4, r_u32(0x80010E34));
            sub_8001F080();
            state = r_u32(0x8005B2F8);
            if (r_u32(state + 0x30) == 1)
            {
                sub_800247A0();
                sub_80020E30(0);
            }
            sub_800119C0();
            sub_8001B868();
            state = r_u32(0x8005B2F8);
            for (;;)
            {
                mode = r_s8(state);
                if (!mode || mode == 2 || r_s8(state) == 5)
                    break;
                (uint32_t)sub_8001CB3C();
                state = r_u32(0x8005B2F8);
                if (r_s8(state + 3))
                {
                    w_u32(0x8005B228, 0x00050000);
                    w_u16(0x800CD542, 0);
                    w_u16(0x800CD540, 0);
                    sub_8008AB00(1094u);
                    (uint32_t)sub_800881B4(0x800CD898, 1094);
                    state = r_u32(0x8005B2F8);
                    if (r_u32(state + 0x30) >= 2)
                        (uint32_t)sub_800881B4(0x800CDD04, 1094);
                    (uint32_t)sub_8008CFDC();
                    (uint32_t)sub_8005E1D8(0x800CE170);
                    state = r_u32(0x8005B2F8);
                    w_u8(state + 3, 0);
                }
                else
                {
                    (uint32_t)sub_8001C428();
                }
                sub_80011C4C();
                state = r_u32(0x8005B2F8);
                if (r_s8(state + 2))
                {
                    w_u8(state, 3);
                    state = r_u32(0x8005B2F8);
                    w_u8(state + 2, 0);
                    sub_80018C1C(1);
                    sub_80020E30(1);
                    state = r_u32(0x8005B2F8);
                }
            }
            sub_8001C408();
            sub_80022FC0();
            sub_800118A0(2, r_u32(0x80010E34));
            (void)sub_80063A20();
            state = r_u32(0x8005B2F8);
            if (r_s8(state) != 5)
                break;
            sub_80063FA0();
        } while (r_s8(r_u32(0x8005B2F8)) == 5);
    }
    sub_800121E4();
    return 0;
}

/* Host resume points share the original resident loop */
static uint32_t rrj_resume_race_loop(int resume_frame)
{
    uint32_t state = 0;
    int32_t mode;
    int first_inner = resume_frame;
    int after_frame = resume_frame == 2;
    while (resume_frame || r_s8(state = r_u32(0x8005B2F8)))
    {
        if (!resume_frame)
        {
            if (r_s8(state) == 2)
            {
                sub_800118A0(16, r_u32(0x80010E34));
                sub_8001B868();
                (void)sub_8007FF4C();
            }
            sub_800118A0(2, r_u32(0x80010E34));
            (void)sub_80064610();
        }
        do
        {
            if (!resume_frame)
            {
                (void)sub_80063B90();
                sub_800118A0(4, r_u32(0x80010E34));
                sub_8001F080();
                state = r_u32(0x8005B2F8);
                if (r_u32(state + 0x30) == 1)
                {
                    sub_800247A0();
                    sub_80020E30(0);
                }
                sub_800119C0();
                sub_8001B868();
            }
            resume_frame = 0;
            if (!first_inner)
                state = r_u32(0x8005B2F8);
            for (;;)
            {
                if (after_frame)
                {
                    after_frame = 0;
                    first_inner = 0;
                    goto frame_complete;
                }
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
                (void)sub_8001CB3C();
                state = r_u32(0x8005B2F8);
                if (r_s8(state + 3))
                {
                    w_u32(0x8005B228, 0x00050000);
                    w_u16(0x800CD542, 0);
                    w_u16(0x800CD540, 0);
                    sub_8008AB00(1094u);
                    (void)sub_800881B4(0x800CD898, 1094);
                    state = r_u32(0x8005B2F8);
                    if (r_u32(state + 0x30) >= 2)
                        (void)sub_800881B4(0x800CDD04, 1094);
                    (void)sub_8008CFDC();
                    (void)sub_8005E1D8(0x800CE170);
                    state = r_u32(0x8005B2F8);
                    w_u8(state + 3, 0);
                }
                else
                {
                    if (rrj_trace_phase_boundary(rrj_host_context(), 0x8001C428))
                        return 0;
                    (void)sub_8001C428();
                }
                sub_80011C4C();
            frame_complete:
                if (rrj_trace_phase_boundary(rrj_host_context(), 0x8001241C))
                    return 0;
                state = r_u32(0x8005B2F8);
                if (r_s8(state + 2))
                {
                    w_u8(state, 3);
                    state = r_u32(0x8005B2F8);
                    w_u8(state + 2, 0);
                    sub_80018C1C(1);
                    sub_80020E30(1);
                    state = r_u32(0x8005B2F8);
                }
            }
            sub_8001C408();
            sub_80022FC0();
            sub_800118A0(2, r_u32(0x80010E34));
            (void)sub_80063A20();
            state = r_u32(0x8005B2F8);
            if (r_s8(state) != 5)
                break;
            sub_80063FA0();
        } while (r_s8(r_u32(0x8005B2F8)) == 5);
    }
    sub_800121E4();
    return 0;
}

uint32_t rrj_resume_after_race_frame(void)
{
    return rrj_resume_race_loop(2);
}

/* Continuation at the recorded race-loop checkpoint */
uint32_t sub_80012370(void)
{
    return rrj_resume_race_loop(1);
}

/* Resume after the saved menu loop without replacing its live state */
uint32_t rrj_resume_after_menu(void)
{
    return rrj_resume_race_loop(0);
}

uint32_t sub_80012764(uint32_t index)
{
    uint32_t object = r_u32(0x8005B268 + index * 4);
    uint32_t flags = r_u32(object + 0x230);
    uint32_t enabled = r_u32(0x8005AD40);
    uint32_t slot;
    uint32_t result;
    FUNCTION_MARKER(0x80012764, "SLUS_010.53");
    if (flags & 0x08000000)
    {
        slot = r_u8(r_u32(object + 0x43C) + 1) & 15;
        result = 1u << slot;
        if (!(enabled & result))
            return result;
        result = slot * 8;
        if (r_u32(object + 0x358) && r_u32(object + 0x440))
            return result;
    }
    else
    {
        uint32_t two_player = index == 0 && r_u32(r_u32(0x8005B2F8) + 48) == 2;
        slot = (enabled >> 24) - (two_player + 1);
        result = slot * 8;
        if (!(enabled & (1u << (slot & 31))))
            return result;
    }
    result = (slot * 7) << 6;
    w_u32(object + 0x22C, r_u32(0x8005B248) + result);
    return result;
}

void sub_80014044(uint8_t record[20], uint32_t tim)
{
    FUNCTION_MARKER(0x80014044, "SLUS_010.53");
    if (!rrj_tim_open(tim))
    {
        (void)rrj_tim_read(record);
        return;
    }
    (void)RRJ_DEBUG_PRINTF((const char *)xport_guest_cptr(0x800107DC, 17));
    xport_store_le32(record + 16, 0);
}

uint32_t sub_8001408C(const char *path, uint32_t *buffer_out, uint8_t record[20])
{
    uint32_t bytes;

    FUNCTION_MARKER(0x8001408C, "SLUS_010.53");
    if ((int32_t)sub_80014654(path, r_u32(0x8005AD5C), buffer_out, &bytes) < 0)
        return 1;
    (void)sub_80014044(record, xport_load_le32(buffer_out));
    return 0;
}

uint32_t sub_800140E8(void)
{
    FUNCTION_MARKER(0x800140E8, "SLUS_010.53");
    w_u32(0x8005253C, 0x8005AC8C);
    return 0;
}

/* Host diagnostic hook does not alter native allocation paths */
static RRJHeapFailureProbe heap_failure_probe;

RRJHeapFailureProbe rrj_heap_set_failure_probe(RRJHeapFailureProbe replacement)
{
    RRJHeapFailureProbe previous = heap_failure_probe;
    heap_failure_probe = replacement;
    return previous;
}

uint32_t sub_800142B4(uint32_t size, uint32_t index)
{
    uint32_t link = 0x800D6500 + index * 16;
    uint32_t block = r_u32(link);
    uint32_t requested = (size + 11) & 0xFFFFFFF8;
    FUNCTION_MARKER(0x800142B4, "SLUS_010.53");
    while (block)
    {
        uint32_t block_size = r_u32(block + 4);
        if (block_size == requested)
        {
            w_u32(link, r_u32(block));
            w_u32(block, requested);
            return block + 4;
        }
        if (requested < block_size)
        {
            uint32_t remainder = block + requested;
            w_u32(link, remainder);
            w_u32(remainder, r_u32(block));
            block_size = r_u32(block + 4);
            w_u32(remainder + 4, block_size - requested);
            w_u32(block, requested);
            return block + 4;
        }
        link = block;
        block = r_u32(block);
    }
    if (heap_failure_probe)
        heap_failure_probe(requested);
    else
        (void)RRJ_DEBUG_PRINTF((const char *)xport_guest_cptr(0x800107F0u, 1), (long)((int32_t)(uint32_t)(requested)), requested);
    return 0;
}

uint32_t sub_8001426C(uint32_t arena, uint32_t size, uint32_t index)
{
    uint32_t slot = 0x800D6500 + index * 16;
    uint32_t end = arena + (size & 0xFFFFFFF8);
    uint32_t available;
    FUNCTION_MARKER(0x8001426C, "SLUS_010.53");
    w_u32(slot + 12, arena);
    w_u32(slot + 8, end);
    w_u32(slot, arena);
    w_u32(arena, 0);
    end = r_u32(slot + 8);
    available = end - r_u32(slot + 12);
    w_u32(arena + 4, available);
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
    size = r_u32(block);
    w_u32(0x8005B3A8, r_u32(0x8005B3A8) + size);
    next = r_u32(0x800D6500);
    while (next && next < block)
    {
        previous = next;
        next = r_u32(previous);
    }
    if (previous && previous + r_u32(previous + 4) == block)
    {
        w_u32(previous + 4, r_u32(previous + 4) + size);
        merged = previous;
    }
    else
    {
        w_u32(block + 4, size);
        w_u32(block, next);
        if (previous)
            w_u32(previous, block);
        else
            w_u32(0x800D6500, block);
        merged = block;
    }
    if (next && merged + r_u32(merged + 4) == next)
    {
        w_u32(merged + 4, r_u32(merged + 4) + r_u32(next + 4));
        w_u32(merged, r_u32(next));
    }
}

uint32_t sub_8001444C(void)
{
    FUNCTION_MARKER(0x8001444C, "SLUS_010.53");
    return sub_8001426C(r_u32(0x800548D4), r_u32(0x800548D8), 0);
}

uint32_t sub_8001447C(uint32_t size, uint32_t pool)
{
    FUNCTION_MARKER(0x8001447C, "SLUS_010.53");
    if (size && pool < 2)
        return sub_800142B4(size, 0);
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

uint32_t sub_8001458C(const char *path, uint32_t device)
{
    uint32_t device_table;
    FUNCTION_MARKER(0x8001458C, "SLUS_010.53");
    device_table = r_u32(0x8005B3B0);
    if (((int32_t)(uint32_t)(device)) < 0 || ((int32_t)(uint32_t)(device)) >= ((int32_t)(uint32_t)(r_u32(device_table + 32))))
        return 0xFFFFFFF8;
    if (device == r_u32(0x8005AD5C))
        return sub_80014978(path);
    return sub_80014A30(path);
}

uint32_t sub_8001460C(uint32_t index)
{
    FUNCTION_MARKER(0x8001460C, "SLUS_010.53");
    sub_80014AE8(index);
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

uint32_t sub_80014654(const char *path, uint32_t unused, uint32_t *buffer_out, uint32_t *result_out)
{
    uint32_t result;

    FUNCTION_MARKER(0x80014654, "SLUS_010.53");
    result = sub_80014B40(path, buffer_out);
    xport_store_le32(result_out, result);
    return result;
}

uint32_t sub_80014680(const char *path, uint32_t device, void *buffer, uint32_t *result_out)
{
    uint32_t index;
    uint32_t result = 0xFFFFFFFF;
    FUNCTION_MARKER(0x80014680, "SLUS_010.53");
    if (r_u32(0x8005B200))
        (void)RRJ_DEBUG_PRINTF((const char *)xport_guest_cptr(0x80010820, 70), path, (long)((int32_t)(uint32_t)(device)), (unsigned int)(uintptr_t)buffer);
    index = sub_80014978(path);
    if (((int32_t)(uint32_t)(index)) < 0)
    {
        if (r_u32(0x8005B200))
            RRJ_DEBUG_PRINTF((const char *)xport_guest_cptr(0x80010894, 40));
    }
    else
    {
        result = sub_80014DE0(index);
        if (((int32_t)(uint32_t)(result)) < 0)
        {
            if (r_u32(0x8005B200))
                RRJ_DEBUG_PRINTF((const char *)xport_guest_cptr(0x80010868, 44), (long)((int32_t)(uint32_t)(result)));
        }
        else
            result = sub_80014BDC(index, result, buffer);
        sub_80014AE8(index);
    }
    xport_store_le32(result_out, result);
    return result;
}

uint32_t sub_80014938(void)
{
    uint32_t address = 0x800D66FC;
    int32_t remaining = 15;
    FUNCTION_MARKER(0x80014938, "SLUS_010.53");
    do
    {
        w_u32(address, 0);
        address -= 20;
        --remaining;
    } while (remaining >= 0);
    if (rrj_data_enabled())
    {
        rrj_data_reset();
        return 0;
    }
    return sub_80015114();
}

uint32_t sub_80014528(uint32_t table)
{
    FUNCTION_MARKER(0x80014528, "SLUS_010.53");
    w_u32(0x8005B3B0, table);
    sub_80014938();
    return 0;
}

uint32_t sub_80014780(uint32_t index, void *buffer, uint32_t count)
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
        if (((int32_t)(uint32_t)(count)) < ((int32_t)(uint32_t)(copied)))
            copied = count;
        temporary = sub_8001447C(2048, 0);
        sub_80014E5C(index, position & 0xFFFFF800, 0);
        fetched = ((int32_t)(uint32_t)(prefix + count)) < 2049 ? prefix + count : 2048;
        fetched = sub_80014BDC(index, fetched, xport_guest_ptr(temporary, 1));
        if (((int32_t)(uint32_t)(fetched)) >= 0)
            sub_8001E08C(buffer, xport_guest_ptr(temporary + prefix, 1), copied);
        sub_800144B8(temporary);
        if (((int32_t)(uint32_t)(copied)) >= ((int32_t)(uint32_t)(count)) || ((int32_t)(uint32_t)(fetched)) < ((int32_t)(uint32_t)(copied)))
            return copied;
    }
    return copied + sub_80014BDC(index, count - copied, (uint8_t *)buffer + copied);
}

uint32_t sub_80014894(uint32_t index, uint32_t buffer, uint32_t count, uint32_t callback)
{
    FUNCTION_MARKER(0x80014894, "SLUS_010.53");
    return sub_80014D18(index, count, buffer, callback);
}

uint32_t sub_800148BC(uint32_t index)
{
    FUNCTION_MARKER(0x800148BC, "SLUS_010.53");
    return sub_80014DE0(index);
}

static uint32_t leaf_open_path(const char *path)
{
    uint32_t index;
    uint32_t record;
    uint32_t result;
    if (rrj_data_enabled())
        return rrj_data_open(path);
    for (index = 0; index < 16; ++index)
    {
        record = 0x800D65D0 + index * 20;
        if (!r_u32(record))
            break;
    }
    if (index == 16)
        return 0xFFFFFFFF;
    result = sub_80015134(path, record + 4, record + 12);
    if (result == 0xFFFFFFFF)
        return result;
    w_u32(record + 8, 0);
    w_u32(record + 16, result);
    w_u32(record, r_u32(record) | 1);
    return index;
}

uint32_t sub_80014978(const char *path)
{
    FUNCTION_MARKER(0x80014978, "SLUS_010.53");
    return leaf_open_path(path);
}

uint32_t sub_80014A30(const char *path)
{
    FUNCTION_MARKER(0x80014A30, "SLUS_010.53");
    return leaf_open_path(path);
}

uint32_t sub_80014AE8(uint32_t index)
{
    uint32_t result = index < 16;
    FUNCTION_MARKER(0x80014AE8, "SLUS_010.53");
    if (rrj_data_enabled())
        return rrj_data_close(index);
    if (result)
    {
        uint32_t record = 0x800D65D0 + index * 20;
        result = r_u32(record) & 1;
        if (result)
        {
            uint32_t handle = r_u32(record + 16);
            w_u32(record, 0);
            return sub_800151BC(handle);
        }
    }
    return result;
}

uint32_t sub_80014B40(const char *path, uint32_t *output)
{
    uint32_t index;
    uint32_t result;

    FUNCTION_MARKER(0x80014B40, "SLUS_010.53");
    index = sub_80014978(path);
    if (index == 0xFFFFFFFF)
    {
        xport_store_le32(output, 0);
        result = 0xFFFFFFFF;
    }
    else
    {
        uint32_t record = 0x800D65D0 + index * 20;
        uint32_t buffer = sub_8001447C(r_u32(record + 12), 0);

        xport_store_le32(output, buffer);
        result = buffer ? sub_80014BDC(index, r_u32(record + 12), xport_guest_ptr(buffer, 1)) : 0xFFFFFFFF;
    }
    (void)sub_80014AE8(index);
    return result;
}

uint32_t sub_80014BDC(uint32_t index, uint32_t count, void *buffer)
{
    uint32_t record;
    uint32_t current;
    uint32_t end;
    uint32_t result;
    FUNCTION_MARKER(0x80014BDC, "SLUS_010.53");
    if (rrj_data_enabled())
        return rrj_data_read(index, buffer, count);
    if (index >= 16)
        return 0xFFFFFFFF;
    record = 0x800D65D0 + index * 20;
    if (!r_u32(record))
    {
        if (r_u32(0x8005B200))
            RRJ_DEBUG_PRINTF((const char *)xport_guest_cptr(0x800108BC, 37), (long)((int32_t)(uint32_t)(index)));
        return 0xFFFFFFFF;
    }
    current = r_u32(record + 8);
    end = r_u32(record + 12);
    if (end < current + count)
    {
        if (r_u32(0x8005B200))
            RRJ_DEBUG_PRINTF((const char *)xport_guest_cptr(0x800108E4, 39), (long)((int32_t)(uint32_t)(count)), (long)((int32_t)(uint32_t)(end - current)));
        count = end - current;
    }
    result = sub_800151FC(r_u32(record + 16), r_u32(record + 4), current, buffer, count);
    w_u32(record + 8, r_u32(record + 8) + count);
    sub_8001508C(index);
    return result;
}

uint32_t sub_80014D18(uint32_t index, uint32_t count, uint32_t buffer, uint32_t callback)
{
    FUNCTION_MARKER(0x80014D18u, "SLUS_010.53");
    if (rrj_data_enabled())
    {
        uint32_t read = rrj_data_read(index, xport_guest_ptr(buffer, count), count);
        if (read != UINT32_MAX)
        {
            w_u32(0x8005B3B8, callback);
            if (callback) rrj_file_function(callback)();
        }
        return read;
    }
    uint32_t record = 0x800D65D0u + index * 20u, current, end, handle, base;
    if (!r_u32(record))
        return UINT32_MAX;
    current = r_u32(record + 8u);
    end = r_u32(record + 12u);
    if (end < current + count)
        count = end - current;
    handle = r_u32(record + 16u);
    base = r_u32(record + 4u);
    current = r_u32(record + 8u);
    w_u32(0x8005B3B8u, callback);
    if (!sub_80015314(handle, base, current, xport_guest_ptr(buffer, 1), count, 0x800150ECu))
        return UINT32_MAX;
    current = r_u32(record + 8u);
    w_u32(record + 8u, current + count);
    (void)sub_8001508C(index);
    return count;
}

uint32_t sub_80014DE0(uint32_t index)
{
    uint32_t record = 0x800D65D0 + index * 20;
    FUNCTION_MARKER(0x80014DE0, "SLUS_010.53");
    if (r_u32(record) & 1)
        return r_u32(record + 12);
    if (r_u32(0x8005B200))
        RRJ_DEBUG_PRINTF((const char *)xport_guest_cptr(0x8001090C, 41), (long)((int32_t)(uint32_t)(index)));
    return 0xFFFFFFFF;
}

uint32_t sub_80014E5C(uint32_t index, uint32_t offset, uint32_t mode)
{
    uint32_t record = 0x800D65D0 + index * 20;
    uint32_t value;
    uint32_t sign;
    uint32_t difference;
    FUNCTION_MARKER(0x80014E5C, "SLUS_010.53");
    if (!(r_u32(record) & 1))
        return 0xFFFFFFFF;
    if (mode == 1)
        value = r_u32(record + 8) + offset;
    else if (((int32_t)(uint32_t)(mode)) >= 2 && mode == 2)
        value = r_u32(record + 12) + offset;
    else
        value = offset;
    sign = (uint32_t)(((int32_t)(uint32_t)(value)) >> 31);
    value &= ~sign;
    difference = r_u32(record + 12) - value;
    value += (uint32_t)(((int32_t)(uint32_t)(difference)) >> 31) & difference;
    w_u32(record + 8, value);
    sub_8001508C(index);
    return value;
}

uint32_t sub_80014F98(uint32_t index)
{
    uint32_t record = 0x800D65D0 + index * 20;
    FUNCTION_MARKER(0x80014F98, "SLUS_010.53");
    return (r_u32(record) & 1) ? r_u32(record + 8) : 0xFFFFFFFF;
}

uint32_t sub_8001508C(uint32_t index)
{
    uint32_t record = 0x800D65D0 + index * 20;
    uint32_t flags = r_u32(record);
    uint32_t result = flags & 1;
    FUNCTION_MARKER(0x8001508C, "SLUS_010.53");
    if (result)
    {
        result = ((int32_t)(uint32_t)(r_u32(record + 8))) < ((int32_t)(uint32_t)(r_u32(record + 12))) ? flags & ~2u : flags | 2;
        w_u32(record, result);
    }
    return result;
}

uint32_t sub_80015134(const char *path, uint32_t first_out, uint32_t second_out)
{
    char expanded[128];
    FUNCTION_MARKER(0x80015134u, "SLUS_010.53");
    sub_8001542C(expanded, (const char *)xport_guest_cptr(0x8005AD60u, 1), path);
    return sub_80015E74(expanded, first_out, second_out) ? 0u : 0xffffffffu;
}

uint32_t sub_800151BC(uint32_t value)
{
    FUNCTION_MARKER(0x800151BC, "SLUS_010.53");
    if ((value >> 16) == 1)
        return sub_8004DE44(value & 0xFFFF);
    return 1;
}

uint32_t sub_800151FC(uint32_t packed_handle, uint32_t base, uint32_t offset, void *buffer, uint32_t size)
{
    uint32_t handle = packed_handle & 0xFFFF;
    FUNCTION_MARKER(0x800151FC, "SLUS_010.53");
    if ((packed_handle >> 16) == 1)
    {
        rrj_missing_pc_seek(handle, offset, 0);
        return sub_8004DE78(handle, buffer, size);
    }
    w_u32(0x8005B3C0, 0);
    if (r_u32(0x8005B200))
    {
        (void)RRJ_DEBUG_PRINTF((const char *)xport_guest_cptr(0x80010968, 1), (long)((int32_t)(uint32_t)(packed_handle)), (long)((int32_t)(uint32_t)(base)), (long)((int32_t)(uint32_t)(offset)), (unsigned int)(uintptr_t)buffer, (long)((int32_t)(uint32_t)(size)));
    }
    sub_80015314(packed_handle, base, offset, buffer, size, 0x800151EC);
    while (!r_u32(0x8005B3C0))
        rrj_native_wait_events();
    if (r_u32(0x8005B200))
        RRJ_DEBUG_PRINTF((const char *)xport_guest_cptr(0x800109BC, 37));
    return size;
}

uint32_t sub_80015314(uint32_t packed_handle, uint32_t base, uint32_t offset, void *buffer, uint32_t size, uint32_t callback)
{
    FUNCTION_MARKER(0x80015314, "SLUS_010.53");
    if ((packed_handle >> 16) == 1)
    {
        rrj_missing_pc_seek(packed_handle, offset, 0);
        sub_8004DE78(packed_handle, buffer, size);
        if (callback)
            rrj_file_function(callback)();
    }
    else
    {
        int32_t blocks = ((int32_t)(uint32_t)(offset));
        unsigned pending = 0;

        if (callback == 0x800151ECu)
            while (r_u32(0x800D6728u) && pending++ < 64u)
            {
                uint32_t active_callback = rrj_trace_vblank_boundary(rrj_host_context());

                if (active_callback)
                    rrj_file_function(active_callback)();
            }
        if (r_u32(0x800D6728u))
            abort();
        if (blocks < 0)
            blocks += 2047;
        blocks >>= 11;
        sub_800155FC(base + (uint32_t)blocks);
        sub_8001562C(callback);
        if (r_u32(0x8005B200))
            RRJ_DEBUG_PRINTF((const char *)xport_guest_cptr(0x800109E4, 46), (long)((int32_t)(uint32_t)(size)), (unsigned int)(uintptr_t)buffer);

        if (callback == 0x800151EC && rrj_trace_runtime_is_live())
        {
            uint32_t sector = r_u32(0x800D6720);
            uint32_t completed = sector + (size + 2047) / 2048;
            /* The synchronous file API accepts native automatic C buffers */
            if (!rrj_live_cd_read(sector, buffer, size))
            {
                RRJ_DEBUG_FPRINTF(stderr, "RRJ native CD read failed: sector=%u bytes=%u\n", sector, size);
                abort();
            }
            w_u32(0x800D6720, completed);
            w_u32(0x800D6724, completed);
            w_u32(0x800D672C, completed);
            w_u32(0x800D6718, 0);
            w_u32(0x800D671C, 0);
            w_u32(0x800D6728, 0);
            (void)rrj_file_function(callback)();
            return 1;
        }
        sub_8001564C(buffer, size);
        if (callback == 0x800151ECu)
        {
            w_u32(0x800D6728u, 0);
            rrj_file_function(callback)();
        }
    }
    return 1;
}

char *sub_8001542C(char *destination, const char *prefix, const char *path)
{
    FUNCTION_MARKER(0x8001542Cu, "SLUS_010.53");
    if ((uint8_t)path[0] == '\\' || (uint8_t)path[1] == ':')
        return strcpy(destination, path);
    strcpy(destination, prefix);
    return strcat(destination, path);
}

uint32_t sub_80015C08(const uint8_t *bcd)
{
    uint32_t first;
    uint32_t second;
    uint32_t third;
    FUNCTION_MARKER(0x80015C08, "SLUS_010.53");
    first = bcd[0];
    second = bcd[1];
    third = bcd[2];
    first = 10 * (first >> 4) + (first & 15);
    second = 10 * (second >> 4) + (second & 15);
    third = 10 * (third >> 4) + (third & 15);
    return 75 * (60 * first + second) + third;
}

uint32_t sub_80015C88(uint8_t output[4], uint32_t frames)
{
    int32_t whole_seconds = ((int32_t)(uint32_t)(frames)) / 75;
    uint8_t frame = (uint8_t)(((int32_t)(uint32_t)(frames)) - whole_seconds * 75);
    int32_t minutes = whole_seconds / 60;
    uint8_t second = (uint8_t)(whole_seconds - minutes * 60);
    uint8_t minute;
    FUNCTION_MARKER(0x80015C88u, "SLUS_010.53");
    output[2] = frame;
    output[1] = second;
    output[3] = 0;
    output[0] = (uint8_t)minutes;
    minute = output[0];
    output[2] = (uint8_t)(16 * (frame / 10) + frame % 10);
    output[1] = (uint8_t)(16 * (second / 10) + second % 10);
    output[0] = (uint8_t)(16 * (minute / 10) + minute % 10);
    return 10 * (minute / 10);
}

uint32_t sub_800155FC(uint32_t value)
{
    uint32_t result;
    FUNCTION_MARKER(0x800155FC, "SLUS_010.53");
    while (r_u32(0x800D6728))
        rrj_native_wait_events();
    result = value + r_u32(0x8005B3D8);
    w_u32(0x800D6720, result);
    return result;
}

uint32_t sub_8001562C(uint32_t value)
{
    FUNCTION_MARKER(0x8001562C, "SLUS_010.53");
    while (r_u32(0x800D6728))
        rrj_native_wait_events();
    w_u32(0x800D6710, value);
    return 0x800D0000;
}

uint32_t sub_8001564C(void *first, uint32_t length)
{
    uint8_t position[8];
    uint32_t adjusted = length + 2047u;
    uint32_t previous, sector;
    FUNCTION_MARKER(0x8001564Cu, "SLUS_010.53");
    if (((int32_t)(uint32_t)(adjusted)) < 0)
        adjusted = length + 4094u;
    while (r_u32(0x800D6728u))
        rrj_native_wait_events();
    previous = r_u32(0x800D6724u);
    sector = r_u32(0x800D6720u);
    w_u32(0x800D6714u, rrj_file_buffer_address(first));
    w_u32(0x800D6718u, (uint32_t)(((int32_t)(uint32_t)(adjusted)) >> 11));
    w_u32(0x800D671Cu, length);
    if (previous == sector)
    {
        w_u32(0x800D6728u, 5u);
        return 5u;
    }
    (void)sub_80015C88(position, sector);
    w_u32(0x800D6728u, 1u);
    w_u32(0x800D6724u, UINT32_MAX);
    (void)CdSyncCallbackPSX(0x8001578Cu);
    (void)CdSync(0, NULL);
    return (uint32_t)CdControlF(2u, position);
}

uint32_t sub_80015734(void *first, uint32_t length)
{
    uint32_t result;
    FUNCTION_MARKER(0x80015734, "SLUS_010.53");
    w_u32(0x8005B3CC, 0);
    sub_8001562C(0x80015724);
    sub_8001564C(first, length);
    do
    {
        result = r_u32(0x8005B3CC);
    } while (!result);
    return result;
}

uint32_t sub_80015D94(void)
{
    FUNCTION_MARKER(0x80015D94, "SLUS_010.53");
    (uint32_t)CdInit();
    (uint32_t)CdControlB((uint8_t)(14), (0x8005B3C8 ? xport_guest_ptr(0x8005B3C8, 1) : NULL), NULL);
    CdReadyCallbackPSX(0x80015860);
    return CdSyncCallbackPSX(0);
}

uint32_t sub_80015F2C(uint32_t capacity)
{
    uint32_t result = r_u32(0x8005AD6C);
    FUNCTION_MARKER(0x80015F2C, "SLUS_010.53");
    if (!result)
    {
        w_u32(0x8005AD74, capacity);
        w_u32(0x8005AD70, 0);
        result = sub_8001447C(capacity * 12, 0);
        w_u32(0x8005AD6C, result);
        if (result)
            return sub_800160E0(r_u32(0x8005B3D0), r_u32(0x8005B3D4), 1, 0x8005AD78);
    }
    return result;
}

uint32_t sub_80015DD8(uint32_t capacity)
{
    CdlLOC toc[100] = {0};
    uint8_t bcd[4];
    uint8_t sector[2048] = {0};
    const uint8_t *leadout = (const uint8_t *)&toc[1];
    uint32_t first, second;
    FUNCTION_MARKER(0x80015DD8u, "SLUS_010.53");
    CdGetToc(toc);
    CdSync(0, NULL);
    first = sub_80015C08(leadout);
    w_u32(0x8005B3D8u, first);
    bcd[0] = leadout[0];
    bcd[1] = leadout[1];
    bcd[2] = 22u;
    bcd[3] = 0u;
    second = sub_80015C08(bcd);
    sub_800160B0(second - r_u32(0x8005B3D8u), sector);
    sub_8001E08C(xport_guest_ptr(0x8005B3D0u, 4), sector + 0x9Eu, 4u);
    sub_8001E08C(xport_guest_ptr(0x8005B3D4u, 4), sector + 0xA6u, 4u);
    return sub_80015F2C(capacity);
}

uint32_t sub_8001549C(uint32_t capacity)
{
    FUNCTION_MARKER(0x8001549C, "SLUS_010.53");
    w_u8(0x8005B3C8, 0xA0);
    sub_80015D94();
    w_u32(0x800D6710, 0);
    w_u32(0x800D6718, 0);
    w_u32(0x800D671C, 0);
    w_u32(0x800D6714, 0);
    w_u32(0x800D6720, 0);
    w_u32(0x800D6728, 0);
    w_u32(0x800D6724, 0xFFFFFFFF);
    w_u32(0x800D672C, 0);
    return sub_80015DD8(capacity);
}

uint32_t sub_80015114(void)
{
    FUNCTION_MARKER(0x80015114, "SLUS_010.53");
    return sub_8001549C(500);
}

uint32_t sub_80015E74(const char *path, uint32_t first_out, uint32_t second_out)
{
    uint32_t hash, index, offset, table, record;
    FUNCTION_MARKER(0x80015E74u, "SLUS_010.53");
    if (r_u32(0x8005AD6Cu))
    {
        hash = sub_80015F8C(path);
        index = 0u;
        offset = 0u;
        if (((int32_t)(uint32_t)(r_u32(0x8005AD70u))) > 0)
        {
            do
            {
                table = r_u32(0x8005AD6Cu);
                record = table + offset;
                if (hash == r_u32(record + 8u))
                {
                    w_u32(first_out, r_u32(record));
                    table = r_u32(0x8005AD6Cu);
                    w_u32(second_out, r_u32(table + offset + 4u));
                    return 1u;
                }
                ++index;
                offset += 12u;
            } while (((int32_t)(uint32_t)(index)) < ((int32_t)(uint32_t)(r_u32(0x8005AD70u))));
        }
    }
    w_u32(second_out, 0u);
    w_u32(first_out, 0u);
    return 0u;
}

uint32_t sub_80015F8C(const char *path)
{
    const char *cursor = path, *start = path;
    uint32_t hash = 0u, shift = 0u;
    FUNCTION_MARKER(0x80015F8Cu, "SLUS_010.53");
    while ((uint8_t)*cursor)
    {
        if ((uint8_t)*cursor == '\\')
            start = cursor;
        ++cursor;
    }
    while ((uint8_t)*start == '\\')
        ++start;
    while ((uint8_t)*start)
    {
        uint32_t value = rrj_solo_bios_toupper((uint8_t)*start++);
        hash = ((hash ^ (value << shift)) + value * value) ^ (value << ((24u - shift) & 31u));
        shift = (shift + 1u) % 24u;
    }
    return hash;
}

uint32_t sub_800160B0(uint32_t position, void *buffer)
{
    FUNCTION_MARKER(0x800160B0, "SLUS_010.53");
    sub_800155FC(position);
    return sub_80015734(buffer, 2048);
}

static uint32_t leaf_path_hash(const char *path)
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
        uint32_t value = rrj_solo_bios_toupper((uint8_t)*start++);
        hash = ((hash ^ (value << shift)) + value * value) ^ (value << ((24 - shift) & 31));
        shift = (shift + 1) % 24;
    }
    return hash;
}

static uint32_t leaf_walk_cd_directory(uint32_t position, uint32_t length, uint32_t depth, const char *path)
{
    uint32_t buffer = sub_8001447C(2048, 0);
    uint32_t next_position = position + 1;
    uint32_t offset;
    if (!buffer)
        return 0;
    sub_800160B0(position, xport_guest_ptr(buffer, 2048));
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
            file_size = r_u32((record + 10));
            if (file_size)
            {
                extent = r_u32((record + 2));
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
                    name[index] = (char)rrj_solo_bios_toupper(r_u8(record + 33 + index));
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
                    leaf_walk_cd_directory(extent, file_size, depth + 1, full_path);
                }
                else if (((int32_t)(uint32_t)(r_u32(0x8005AD70))) >= ((int32_t)(uint32_t)(r_u32(0x8005AD74))))
                {
                    RRJ_DEBUG_PRINTF((const char *)xport_guest_cptr(0x80010A74, 44));
                }
                else
                {
                    uint32_t count = r_u32(0x8005AD70);
                    uint32_t table = r_u32(0x8005AD6C);
                    uint32_t hash = leaf_path_hash(full_path);
                    uint32_t entry;
                    for (index = 0; ((int32_t)(uint32_t)(index)) < ((int32_t)(uint32_t)(count)); ++index)
                    {
                        if (r_u32(table + index * 12 + 8) == hash)
                            RRJ_DEBUG_PRINTF((const char *)xport_guest_cptr(0x80010A2C, 71));
                    }
                    entry = table + count * 12;
                    w_u32(entry + 4, file_size);
                    w_u32(entry, extent);
                    w_u32(entry + 8, hash);
                    w_u32(0x8005AD70, count + 1);
                }
            }
            offset += record_size;
        }
        length -= 2048;
        if (!length)
            break;
        sub_800160B0(next_position++, xport_guest_ptr(buffer, 2048));
        offset = 0;
        if (((int32_t)(uint32_t)(length)) <= 0)
            break;
    }
    return sub_800144B8(buffer);
}

uint32_t sub_800160E0(uint32_t position, uint32_t length, uint32_t depth, uint32_t path)
{
    char root[256];
    uint32_t index = 0;
    FUNCTION_MARKER(0x800160E0, "SLUS_010.53");
    while (index < sizeof(root) - 1 && r_u8(path + index))
    {
        root[index] = (char)r_u8(path + index);
        ++index;
    }
    root[index] = 0;
    return leaf_walk_cd_directory(position, length, depth, root);
}

uint32_t sub_800163D4(void)
{
    const uint32_t options[] = {1, 0x01002001, 4, 12, 5, 24, 0};
    uint32_t result;
    FUNCTION_MARKER(0x800163D4, "SLUS_010.53");
    sub_80019D9C();
    sub_8001EBF0();
    result = sub_8001EC30(options);
    sub_8001E138();
    if (!result)
        w_u8(0x8005B3F4, 1);
    return 1;
}

uint32_t sub_8001643C(void)
{
    FUNCTION_MARKER(0x8001643C, "SLUS_010.53");
    sub_8001E188();
    return sub_8001ECEC();
}

uint32_t sub_800164B4(void)
{
    uint32_t result = 64u;
    FUNCTION_MARKER(0x800164B4u, "SLUS_010.53");
    if (r_u32(r_u32(0x8005B2F8u) + 48u) == 2u)
    {
        uint8_t parameters[12];
        uint32_t value = r_u32(0x800D6C0Cu);
        xport_store_le32(parameters, 0xFFFFFFFFu);
        xport_store_le32(parameters + 4, (uint32_t)(((int32_t)(uint32_t)(value * 127u)) >> 7));
        xport_store_le32(parameters + 8, 64u);
        result = sub_8001F174(r_u32(0x8005B404u), 110u, 0u, 1u, parameters);
        w_u32(0x8005B438u, result);
    }
    return result;
}

uint32_t sub_8001654C(void)
{
    uint32_t scalar_denominator_1;
    uint32_t scalar_denominator_2;
    uint32_t scalar_denominator_3;
    uint32_t scalar_denominator_4;
    uint32_t scalar_denominator_5;

    uint32_t state = r_u32(0x8005B2F8);
    uint32_t players = r_u32(state + 48);
    uint32_t index;
    FUNCTION_MARKER(0x8001654C, "SLUS_010.53");
    for (index = 0; index < players; ++index)
    {
        uint32_t object = r_u32(0x8005B268 + index * 4);
        uint32_t output = r_u32(0x8005B42C) + index * 132;
        uint32_t table = r_u32(object + 556);
        uint32_t values = 0x800D6BD8;
        uint32_t profile;
        uint32_t i;
        int32_t value;
        int32_t denominator;
        uint32_t scale;
        w_u32(output, object);
        profile = 0x800525F4 + 20 * (r_u32(state + 72 + index * 4) / 9);
        w_u32(output + 44, profile);
        for (i = 0; i < 10; ++i)
        {
            value = ((int32_t)(uint32_t)((uint32_t)sub_8001FC90(r_u32(table + 188), r_u32(table + 20 + i * 4))));
            if (value)
            {
                denominator = (value >> 1) + ((value - 2) >> 31);
                scale = ((scalar_denominator_1 = ((uint32_t)denominator)), (((uint32_t)(scalar_denominator_1 ? ((uint32_t)(0x80000000u)) / scalar_denominator_1 : 0xFFFFFFFF))));
                w_u32(values + i * 4, (uint32_t)sub_8001FC90(r_u32(table + 20), scale));
            }
        }
        value = ((int32_t)(uint32_t)(r_u32(table + 180)));
        denominator = (value >> 1) + ((value - 2) >> 31);
        scale = ((scalar_denominator_2 = ((uint32_t)denominator)), (((uint32_t)(scalar_denominator_2 ? ((uint32_t)(0x80000000u)) / scalar_denominator_2 : 0xFFFFFFFF))));
        w_u32(output + 60, (uint32_t)sub_8001FC90(r_u32(profile) << 4, scale));
        value = ((int32_t)(uint32_t)(r_u32(table + 204)));
        denominator = (value >> 2) + (((value >> 1) - 2) >> 31);
        w_u32(output + 68, ((scalar_denominator_3 = ((uint32_t)denominator)), (((uint32_t)(scalar_denominator_3 ? ((uint32_t)(0x80000000u)) / scalar_denominator_3 : 0xFFFFFFFF)))));
        denominator = (value >> 3) + (((value >> 2) - 2) >> 31);
        w_u32(output + 72, ((scalar_denominator_4 = ((uint32_t)denominator)), (((uint32_t)(scalar_denominator_4 ? ((uint32_t)(0x80000000u)) / scalar_denominator_4 : 0xFFFFFFFF)))));
        value = (((int32_t)(uint32_t)(r_u32(table + 400))) >> 2) + (((int32_t)(uint32_t)(r_u32(table + 400))) >> 1);
        denominator = (value >> 1) + ((value - 2) >> 31);
        w_u32(output + 80, ((scalar_denominator_5 = ((uint32_t)denominator)), (((uint32_t)(scalar_denominator_5 ? ((uint32_t)(0x80000000u)) / scalar_denominator_5 : 0xFFFFFFFF)))));
    }
    return 0;
}

uint32_t sub_80018C1C(uint32_t enabled)
{
    uint8_t packet[12];
    uint32_t primary, secondary, result;
    uint32_t volume;
    uint32_t saved_volume, saved_gain, saved_mix, saved_level, saved_music;
    FUNCTION_MARKER(0x80018C1C, "SLUS_010.53");
    volume = r_u32(0x800D6C0C);
    xport_store_le32(packet + 8, 64);
    xport_store_le32(packet + 4, (uint32_t)(((int32_t)(uint32_t)((volume << 7) - volume)) >> 7));
    if (enabled)
    {
        uint32_t context, state, state_tag;
        result = r_u32(0x8005B434);
        if (result)
            return result;
        sub_8001F054(0, 0);
        context = r_u32(0x8005B40C);
        state = r_u32(0x8005B2F8);
        w_u32(context + 24, 0);
        if (r_u32(state + 48) == 2)
            w_u32(context + 96, 0);
        w_u32(0x800D6C20, r_u32(0x800D6C00));
        saved_mix = r_u32(0x800D6C08);
        saved_level = r_u32(0x800D6C0C);
        saved_music = r_u32(0x800D6C14);
        state_tag = r_u32(0x800CDAC0);
        w_u32(0x800D6C28, saved_mix);
        w_u32(0x800D6C2C, saved_level);
        w_u32(0x800D6C34, saved_music);
        if (state_tag != 2)
            w_u32(0x800D6C24, r_u32(0x800D6C04));
        else
        {
            state = r_u32(0x8005B2F8);
            if (!(r_u8(state + 4) & 1))
                w_u32(0x800D6C24, r_u32(0x800D6C04));
        }
        primary = r_u32(0x8005B438);
        w_u32(0x800D6C00, 0);
        w_u32(0x800D6C04, 0);
        w_u32(0x800D6C08, 0);
        xport_store_le32(packet, 0);
        if (primary)
            sub_8001F6A4(primary, packet);
        secondary = r_u32(0x800D6BC0);
        if (secondary)
            sub_8001F6A4(secondary, packet);
        w_u32(0x8005B434, 1);
        return 1;
    }
    saved_volume = r_u32(0x800D6C20);
    saved_gain = r_u32(0x800D6C24);
    saved_mix = r_u32(0x800D6C28);
    saved_level = r_u32(0x800D6C2C);
    saved_music = r_u32(0x800D6C34);
    primary = r_u32(0x8005B438);
    xport_store_le32(packet, 1486);
    w_u32(0x800D6C00, saved_volume);
    w_u32(0x800D6C04, saved_gain);
    w_u32(0x800D6C08, saved_mix);
    w_u32(0x800D6C0C, saved_level);
    w_u32(0x800D6C14, saved_music);
    if (primary)
        sub_8001F6A4(primary, packet);
    secondary = r_u32(0x800D6BC0);
    result = 743;
    if (secondary)
    {
        xport_store_le32(packet, 743);
        result = sub_8001F6A4(secondary, packet);
    }
    w_u32(0x8005B434, 0);
    return result;
}

uint32_t sub_80019D9C(void)
{
    static const uint32_t reset_words[] = {0x8005B40C, 0x8005B42C, 0x8005B410, 0x8005B414, 0x8005B3F8, 0x8005B418, 0x8005B3F0, 0x8005B430, 0x8005B408, 0x8005B428, 0x8005B3E4, 0x8005B3EC, 0x8005B434};
    uint32_t index;
    FUNCTION_MARKER(0x80019D9C, "SLUS_010.53");
    w_u32(0x8005B404, 0xFFFFFFFF);
    w_u32(0x8005B420, 0xFFFFFFFF);
    w_u32(0x8005B3FC, 0xFFFFFFFF);
    for (index = 0; index < sizeof reset_words / sizeof reset_words[0]; ++index)
        w_u32(reset_words[index], 0);
    w_u8(0x8005B3F4, 0);
    w_u32(0x8005B3E0, 1);
    for (index = 0; index < 7; ++index)
    {
        uint32_t value = (uint32_t)(((int32_t)(uint32_t)(r_u32(0x80052638 + index * 4) * 7)) >> 4);
        w_u32(0x800D6C00 + index * 4, value);
        w_u32(0x800D6C20 + index * 4, value);
    }
    return 0;
}

uint32_t sub_8001A424(void)
{
    uint32_t state = r_u32(0x8005B2F8);
    uint32_t mode = r_u8(state + 4);
    uint32_t players = r_u32(state + 48);
    uint32_t types = 0x800D6C40;
    uint32_t index;
    uint32_t result = 0x800D0000;
    FUNCTION_MARKER(0x8001A424, "SLUS_010.53");
    w_u32(types, 6);
    w_u32(types + 4, 0);
    w_u32(types + 8, 1);
    w_u32(types + 12, 4);
    w_u32(types + 16, 3);
    w_u32(types + 20, 11);
    w_u32(types + 24, 4);
    w_u32(types + 28, 255);
    w_u32(types + 32, 15);
    if (players == 2)
    {
        w_u32(types + 28, 22);
        w_u32(types + 32, 7);
    }
    if (mode == 33)
    {
        w_u32(types + 4, 7);
        w_u32(types + 8, 7);
        w_u32(types + 12, 7);
        w_u32(types + 24, 7);
    }
    if ((mode & 1) && mode != 33)
    {
        w_u32(types + 24, 23);
        if (players == 1)
            w_u32(types, 7);
        if (((int32_t)(uint32_t)(r_u32(0x8005B1F8))) < 3 || r_u32(r_u32(0x8005B3A0) + 2372) >= 9)
            w_u32(types + 4, 255);
        else
            w_u32(types + 8, 255);
        if (players == 1)
            w_u32(types + 12, 255);
    }
    for (index = 0; index < 9; ++index)
    {
        uint32_t record = 0x800D6AA0 + index * 32;
        if (!(r_u32(types + index * 4) & 8))
            w_u32(record + 28, sub_8001447C(56, 0));
        w_u32(record + 12, 255);
        w_u32(record + 16, 255);
    }
    if (((int32_t)(uint32_t)(r_u32(0x800D6858))) >= 0)
    {
        uint32_t stream = 0x800D6858;
        if (mode == 33)
            w_u32(stream + 4, 0x23C000);
        else
        {
            uint32_t random = (uint32_t)sub_80043F00(0xF2000002) & 0xFF;
            w_u32(stream + 4, ((133 * random) >> 8) << 14);
        }
        sub_80023148(stream, 7, 0, 0);
        if ((mode & 1) && mode != 33)
        {
            w_u32(stream + 4, r_u32(types + 8) == 255 ? 0x234000 : 0x238000);
            sub_80023148(stream, 1, 0, 0);
        }
        w_u32(0x800D6BC0, 0);
        w_u32(0x800D6BC4, 0);
        w_u32(0x800D6BC8, 1);
        w_u32(0x800D6BD0, 0);
        w_u32(0x800D6BD4, 0);
        w_u32(0x800D6B40, r_u32(0x8005B404));
        w_u32(0x800D6B44, 11);
        w_u32(0x800D6B48, 2);
        w_u32(0x800D6B4C, 11);
        w_u32(0x800D6B50, 11);
        w_u32(0x800D6B54, 88);
        w_u32(0x800D6B58, 89);
        w_u32(0x800D6B5C, 0);
        w_u32(0x8005B424, 0);
        result = 7;
        if (r_u32(0x800D6C60) == 15)
        {
            w_u32(0x800D6BA0, r_u32(0x8005B404));
            w_u32(0x800D6BA4, 7);
            w_u32(0x800D6BA8, 2);
            w_u32(0x800D6BAC, 15);
            w_u32(0x800D6BB0, 255);
            w_u32(0x800D6BB4, 108);
            w_u32(0x800D6BB8, 108);
            w_u32(0x800D6BBC, 0);
            result = 108;
        }
        w_u32(0x8005B440, 0);
    }
    return result;
}

uint32_t sub_8001B494(void)
{
    FUNCTION_MARKER(0x8001B494, "SLUS_010.53");
    w_u8(0x8005B450, 0);
    InitGeom();
    (uint32_t)ResetCallback();
    return 0;
}

static void game_vblank_callback(void)
{
    sub_8001B500();
}

static uint32_t game_vblank_register(PsxInterruptCallback callback)
{
    uint32_t previous_guest = VSyncCallbackPSX(0u);
    uint32_t previous_native = (uint32_t)VSyncCallback(callback);
    if (previous_native)
    {
        if (previous_native != (uint32_t)(uintptr_t)game_vblank_callback || previous_guest)
        {
            /* TODO Bind additional original VBlank callbacks */
            RRJ_DEBUG_FPUTS("RRJ: unresolved previous native VBlank callback\n", stderr);
            abort();
        }
        return 0x8001B500u;
    }
    return previous_guest;
}

/* Rebind the saved interrupt owner without resetting its guest state */
void rrj_restore_native_vblank(void)
{
    game_vblank_register(r_u8(0x8005B450) ? game_vblank_callback : NULL);
}

uint32_t sub_8001B4C0(void)
{
    FUNCTION_MARKER(0x8001B4C0, "SLUS_010.53");
    DrawSyncCallback(NULL);
    if (r_u8(0x8005B450))
        sub_8001B5C8();
    return (uint32_t)StopCallback();
}

uint32_t sub_8001B564(void)
{
    FUNCTION_MARKER(0x8001B564, "SLUS_010.53");
    w_u32(0x8005B44C, 0);
    w_u32(0x8005B448, 0);
    game_vblank_register(game_vblank_callback);
    w_u8(0x8005B450, 1);
    return 1;
}

uint32_t sub_8001B594(uint32_t callback)
{
    FUNCTION_MARKER(0x8001B594, "SLUS_010.53");
    if (!r_u8(0x8005B450) || r_u32(0x8005B448))
        return 3;
    w_u32(0x8005B448, callback);
    return 0;
}

uint32_t sub_8001B5C8(void)
{
    uint32_t result;
    FUNCTION_MARKER(0x8001B5C8, "SLUS_010.53");
    result = game_vblank_register(NULL);
    w_u32(0x8005B448, 0);
    w_u8(0x8005B450, 0);
    return result;
}

uint32_t sub_8001B5EC(void)
{
    char path[1024];
    uint32_t index;
    FUNCTION_MARKER(0x8001B5ECu, "SLUS_010.53");
    sprintf(path, (const char *)xport_guest_cptr(0x80010AA0u, 1), (const char *)xport_guest_cptr(0x80052464u, 1));
    index = sub_8001458C(path, r_u32(0x8005AD5Cu));
    if (((int32_t)(uint32_t)(index)) < 0)
        return (uint32_t)RRJ_DEBUG_PRINTF((const char *)xport_guest_cptr(0x80010AACu, 1), path);
    sub_80014780(index, xport_guest_ptr(0x800D6C68u, 1), 76u);
    return sub_8001460C(index);
}

uint32_t sub_8001BFA8(void)
{
    uint32_t previous;
    uint32_t result;
    FUNCTION_MARKER(0x8001BFA8, "SLUS_010.53");
    previous = r_u32(0x800D6C68);
    sub_8001B5EC();
    sub_8001B670(previous);
    result = 0x800D6C6C + 24 * r_u32(0x800D6C68);
    w_u32(0x8005B474, result);
    return result;
}

uint32_t sub_8001B670(uint32_t value)
{
    FUNCTION_MARKER(0x8001B670, "SLUS_010.53");
    w_u32(0x800D6C68, value);
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
    mode = r_u32(0x8005AD88);
    result = mode;
    if (mode)
    {
        w_u32(0x8005B468, 1);
        width = mode == 1 ? 262 : 136;
        for (;;)
        {
            result = r_u32(0x8005B460) + width - 1;
            if ((int32_t)r_u32(0x8005B45C) >= (int32_t)result)
                break;
            rrj_native_wait_events();
        }
    }
    w_u32(0x8005B468, 0);
    w_u32(0x8005AD88, 0);
    return result;
}

uint32_t sub_8001BC54(void)
{
    uint16_t rectangle[4];
    uint8_t record[20];
    uint32_t buffer = 0;
    uint32_t result;

    FUNCTION_MARKER(0x8001BC54, "SLUS_010.53");
    if (!sub_8001408C((const char *)xport_guest_cptr(0x80010ADC, 1), &buffer, record))
    {
        rectangle[0] = 160;
        rectangle[1] = 200;
        rectangle[2] = 66;
        rectangle[3] = 14;
        (void)rrj_gpu_upload((const uint8_t *)rectangle, xport_load_le32(record + 16));
        rectangle[1] = 456;
        (void)rrj_gpu_upload((const uint8_t *)rectangle, xport_load_le32(record + 16));
    }
    result = (uint32_t)DrawSync(0);
    if (buffer)
        return sub_800144B8(buffer);
    return result;
}

uint32_t sub_8001BDB0(void)
{
    uint32_t result;
    FUNCTION_MARKER(0x8001BDB0, "SLUS_010.53");
    sub_80043DA4();
    sub_8001B564();
    result = sub_8001B594(0x8001B700);
    sub_80043DB4();
    return result;
}

uint32_t sub_8001BDEC(void)
{
    uint32_t context;
    uint32_t result;
    FUNCTION_MARKER(0x8001BDEC, "SLUS_010.53");
    context = r_u32(0x8005B470);
    result = r_u32(context + 240) & 0x00FFFFFF;
    w_u32(context + 268, result);
    return result;
}

uint32_t sub_8001C004(void)
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
    w_u32(0x8005B470, context);
    w_u32(0x800D6DA8, 0);
    w_u32(0x8005B46C, 0);
    (uint32_t)ResetGraph(((int32_t)(uint32_t)(0)));
    sub_8001B6C4(0, 0, 1024, 512);
    context = r_u32(0x8005B470);
    w_u8(context + 5, 0);
    context = r_u32(0x8005B470);
    w_u8(context + 6, 1);
    context = r_u32(0x8005B470);
    w_u8(context + 7, 0);
    context = r_u32(0x8005B470);
    w_u8(context + 4, 1);
    context = r_u32(0x8005B470);
    w_u32(context + 268, r_u32(context + 240) & 0x00FFFFFF);
    (uint32_t)sub_8001BE08(0, 0, 384, 240);
    sub_80048414(0);
    context = r_u32(0x8005B470);
    w_u32(context, 0);
    SetDispMask(((int32_t)(uint32_t)(1)));
    sub_8001BDEC();
    (uint32_t)DrawSync(0);
    state = r_u32(0x8005B2F8);
    players = r_u32(state + 48);
    entries = r_s16(0x8005AD84 + (players - 1) * 2);
    size = (uint32_t)entries * 60;
    allocation = sub_8001447C(size, 0);
    context = r_u32(0x8005B470);
    state = r_u32(0x8005B2F8);
    w_u32(context + 240, allocation);
    players = r_u32(state + 48);
    entries = r_s16(0x8005AD84 + (players - 1) * 2);
    w_u32(0x800D75B0, allocation & 0x00FFFFFF);
    result = (allocation + (uint32_t)entries * 60 - 60) & 0x00FFFFFF;
    packet = r_u32(context + 240) & 0x00FFFFFF;
    w_u32(0x800D75B4, packet);
    w_u32(context + 268, packet);
    w_u32(0x8005B4DC, result);
    w_u32(0x8005B4D0, result);
    return result;
}

uint32_t sub_8001C408(void)
{
    uint32_t context;
    uint32_t ready;
    FUNCTION_MARKER(0x8001C408, "SLUS_010.53");
    context = r_u32(0x8005B470);
    do
    {
        ready = r_u8(context + 4);
        if (!ready)
            rrj_display_wait_step();
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
        sub_8001E0B4(xport_guest_ptr(base + index * 192, 192), xport_guest_ptr(base, 1), 192);
    sub_80043DA4();
    sub_8001E0B4(xport_guest_ptr(0x800D6DE0, 768), xport_guest_ptr(base, 1), 768);
    sub_80043DB4();
    return 0;
}

uint32_t sub_8001C590(void)
{
    int32_t index;
    FUNCTION_MARKER(0x8001C590, "SLUS_010.53");
    (uint32_t)rrj_missing_pad_init_mtap((uint32)(0x800D70E0), (uint32)(0x800D7104));
    for (index = 3; index >= 0; --index)
        w_u32(0x800D7428 + 24 * (uint32_t)index, (uint32_t)index);
    w_u32(0x800D7448, 1);
    w_u32(0x800D7430, 1);
    PadStartCom();
    return sub_8001C4D0();
}

uint32_t sub_8001DFE4(void)
{
    uint32_t handle;
    FUNCTION_MARKER(0x8001DFE4, "SLUS_010.53");
    sub_80043DA4();
    handle = (uint32_t)OpenEventPSX(0xF2000002, 2, 4096, 0x8001DF64);
    w_u32(0x800527D4, handle);
    if (!(uint32_t)EnableEvent(((int32_t)(uint32_t)(handle))))
        abort();
    (uint32_t)SetRCnt((uint32)(0xF2000002), (uint16)(0xFFFF), ((int32_t)(uint32_t)(4096)));
    (uint32_t)ResetRCnt((uint32)(0xF2000002));
    (uint32_t)StartRCnt((uint32)(0xF2000002));
    sub_80043DB4();
    w_u32(0x800527D8, 0);
    w_u32(0x800527DC, 0);
    return 0;
}

void *sub_8001E08C(void *destination, const void *source, uint32_t size)
{
    uint8_t *output = destination;
    const uint8_t *input = source;
    FUNCTION_MARKER(0x8001E08Cu, "SLUS_010.53");
    while (size)
    {
        uint8_t value = *input++;
        --size;
        *output++ = value;
    }
    return destination;
}

uint32_t sub_8001E138(void)
{
    FUNCTION_MARKER(0x8001E138, "SLUS_010.53");
    w_u32(0x8005B484, sub_80051058(0x8001E328));
    w_u32(0x8005B48C, sub_80051018());
    (void)SpuSetTransferMode(0);
    w_u32(0x8005B494, 0);
    w_u32(0x8005B480, 0);
    w_u32(0x8005B490, 0);
    w_u32(0x8005B498, 0);
    w_u32(0x8005B488, 1);
    return 1;
}

uint32_t sub_8001E188(void)
{
    uint32_t result;
    FUNCTION_MARKER(0x8001E188, "SLUS_010.53");
    result = r_u32(0x8005B488);
    if (result)
    {
        sub_8001E1D0();
        (void)SpuSetTransferMode(((int32_t)(uint32_t)(r_u32(0x8005B48C))));
        result = sub_80051058(r_u32(0x8005B484));
    }
    w_u32(0x8005B490, 0);
    w_u32(0x8005B498, 0);
    w_u32(0x8005B488, 0);
    return result;
}

uint32_t sub_8001E1D0(void)
{
    uint32_t active;
    uint32_t index;
    uint32_t result;
    FUNCTION_MARKER(0x8001E1D0, "SLUS_010.53");
    active = r_u32(0x8005B488);
    result = active;
    if (active)
    {
        index = r_u32(0x8005B494);
        w_u32(0x8005B480, index);
        result = r_u32(0x8005B490);
        w_u32(0x800D7488 + index * 24 + 12, 0);
    }
    return result;
}

typedef uint32_t (*RRJHeapFunction)(uint32_t value);

static RRJHeapFunction heap_allocate_function(uint32_t address)
{
    switch (address)
    {
        case 0x800144E8u:
            return sub_800144E8;
        default:
            /* TODO Bind additional native allocation callbacks */
            RRJ_DEBUG_FPRINTF(stderr, "RRJ: missing allocation callback adapter %08X\n", address);
            abort();
    }
}

static RRJHeapFunction heap_release_function(uint32_t address)
{
    switch (address)
    {
        case 0x800144B8u:
            return sub_800144B8;
        case 0x80014508u:
            return sub_80014508;
        default:
            /* TODO Bind additional native release callbacks */
            RRJ_DEBUG_FPRINTF(stderr, "RRJ: missing release callback adapter %08X\n", address);
            abort();
    }
}

uint32_t sub_8001E48C(uint32_t size)
{
    uint32_t allocator = r_u32(0x8005AD9C);
    uint32_t result;
    uint32_t index;
    FUNCTION_MARKER(0x8001E48C, "SLUS_010.53");
    if (allocator)
        result = heap_allocate_function(allocator)(size);
    else
        result = sub_8001447C(size, 0);
    if (result)
    {
        for (index = 0; index < size; ++index)
            w_u8(result + index, 0);
    }
    return result;
}

uint32_t sub_8001E500(uint32_t pointer)
{
    uint32_t release = r_u32(0x8005ADA0);
    FUNCTION_MARKER(0x8001E500, "SLUS_010.53");
    if (release)
        return heap_release_function(release)(pointer);
    sub_800144B8(pointer);
    return 0;
}

static uint32_t register_pair(uint32_t first, uint32_t second, uint32_t first_slot, uint32_t second_slot)
{
    if ((first || second) && (r_u32(first_slot) || r_u32(second_slot)))
        return 0xFFFFFFFF;
    if ((first && !second) || (!first && second))
        return 0xFFFFFFFF;
    w_u32(first_slot, first);
    w_u32(second_slot, second);
    return 0;
}

uint32_t sub_8001E544(uint32_t first, uint32_t second)
{
    FUNCTION_MARKER(0x8001E544, "SLUS_010.53");
    return register_pair(first, second, 0x8005AD9C, 0x8005ADA0);
}

uint32_t sub_8001E5A8(void)
{
    uint32_t index;
    FUNCTION_MARKER(0x8001E5A8, "SLUS_010.53");
    for (index = 0; index < 7; ++index)
        w_u32(0x800D6870 + index * 4, 0);
    for (index = 0; index < 3; ++index)
        w_u32(0x800D6898 - index * 20 + 28, 0);
    w_u32(0x800D69F0, 127);
    w_u32(0x800D69F4, 0);
    return sub_8001FBD4();
}

uint32_t sub_8001E614(const uint32_t *table)
{
    uint32_t value_out;
    uint32_t first_count = 1;
    uint32_t item_count = 24;
    uint32_t first;
    uint32_t items;
    uint32_t index;
    FUNCTION_MARKER(0x8001E614, "SLUS_010.53");
    if (sub_8001E7B0(table, 4, &value_out, NULL))
    {
        first_count = value_out;
        w_u32(0x800D6870, first_count);
    }
    if (sub_8001E7B0(table, 5, &value_out, NULL))
        item_count = value_out;
    first = sub_8001E48C(first_count * 4);
    w_u32(0x800D6874, first);
    if (!first)
    {
        sub_8001E738();
        return 0xFFFFFFFF;
    }
    items = sub_8001E48C(item_count * 44);
    w_u32(0x800D687C, items);
    if (!items)
    {
        sub_8001E738();
        return 0xFFFFFFFF;
    }
    for (index = 0; ((int32_t)(uint32_t)(index)) < ((int32_t)(uint32_t)(item_count)); ++index)
        w_u32(r_u32(0x800D687C) + index * 44 + 28, index);
    w_u32(0x800D6878, item_count);
    return 0;
}

uint32_t sub_8001E738(void)
{
    uint32_t pointer;
    uint32_t result;
    FUNCTION_MARKER(0x8001E738, "SLUS_010.53");
    pointer = r_u32(0x800D6874);
    result = pointer;
    if (pointer)
    {
        sub_8001E818();
        result = sub_8001E500(pointer);
        w_u32(0x800D6874, 0);
        w_u32(0x800D6870, 0);
    }
    pointer = r_u32(0x800D687C);
    if (pointer)
    {
        result = sub_8001E500(pointer);
        w_u32(0x800D687C, 0);
        w_u32(0x800D6878, 0);
    }
    return result;
}

uint32_t sub_8001E7B0(const uint32_t *table, uint32_t key, uint32_t *value_out, uint32_t *index_out)
{
    uint32_t index = 0;
    FUNCTION_MARKER(0x8001E7B0, "SLUS_010.53");
    if (!table)
        return 0;
    while (table[0])
    {
        if (table[0] == key)
        {
            if (value_out)
                *value_out = table[1];
            if (index_out)
                *index_out = index;
            return 1;
        }
        table += 2;
        ++index;
    }
    return 0;
}

uint32_t sub_8001E818(void)
{
    uint32_t index = 0;
    uint32_t count, result;
    FUNCTION_MARKER(0x8001E818, "SLUS_010.53");
    count = r_u32(0x800D6870);
    result = count;
    while (((int32_t)(uint32_t)(index)) < ((int32_t)(uint32_t)(count)))
    {
        sub_8001EE28(index);
        count = r_u32(0x800D6870);
        ++index;
        result = ((int32_t)(uint32_t)(index)) < ((int32_t)(uint32_t)(count));
    }
    return result;
}

void sub_8001E8C4(void)
{
    SpuCommonAttr attr = {0};
    FUNCTION_MARKER(0x8001E8C4, "SLUS_010.53");
    attr.mask = 0x3FC3;
    attr.mvol.left = 0x3FFF;
    attr.mvol.right = 0x3FFF;
    SpuSetCommonAttr(&attr);
}

uint32_t sub_8001EA54(uint32_t index)
{
    uint32_t base;
    uint32_t record;
    FUNCTION_MARKER(0x8001EA54, "SLUS_010.53");
    base = r_u32(0x800D6874);
    record = r_u32(base + index * 4);
    if (!record)
        return base;
    SpuFree(r_u32(record + 8));
    w_u32(record + 8, 0xFFFFFFFF);
    return 0xFFFFFFFF;
}

uint32_t sub_8001EB60(uint32_t flags)
{
    uint32_t result;
    FUNCTION_MARKER(0x8001EB60, "SLUS_010.53");
    result = r_u32(0x800D6888) | flags;
    w_u32(0x800D6888, result);
    return result;
}

void sub_8001EBF0(void)
{
    FUNCTION_MARKER(0x8001EBF0, "SLUS_010.53");
    sub_8004F2C8();
    rrj_missing_spu_start();
    (void)SpuInitMalloc(12, (char *)xport_guest_ptr(0x800D6A38, 13u * 8u));
    sub_8001E8C4();
}

uint32_t sub_8001EE28(uint32_t index)
{
    uint32_t count;
    uint32_t base;
    uint32_t result;
    FUNCTION_MARKER(0x8001EE28, "SLUS_010.53");
    count = r_u32(0x800D6870);
    result = ((int32_t)(uint32_t)(count)) < ((int32_t)(uint32_t)(index));
    if (((int32_t)(uint32_t)(count)) >= ((int32_t)(uint32_t)(index)) && ((int32_t)(uint32_t)(index)) >= 0)
    {
        base = r_u32(0x800D6874);
        result = base;
        if (base)
        {
            sub_8001EA54(index);
            base = r_u32(0x800D6874);
            result = base + index * 4;
            w_u32(result, 0);
        }
    }
    return result;
}

uint32_t sub_8001EC30(const uint32_t *pairs)
{
    uint32_t value_out = 0;
    uint32_t allocate = 0, release = 0, result;
    FUNCTION_MARKER(0x8001EC30, "SLUS_010.53");
    sub_8001E5A8();
    if (sub_8001E7B0(pairs, 2, &value_out, NULL))
        allocate = value_out;
    if (sub_8001E7B0(pairs, 3, &value_out, NULL))
        release = value_out;
    sub_8001E544(allocate, release);
    result = sub_8001E614(pairs);
    if (result)
        sub_8001ECEC();
    return result;
}

uint32_t sub_8001ECEC(void)
{
    FUNCTION_MARKER(0x8001ECEC, "SLUS_010.53");
    (void)sub_8001F0D4();
    sub_8004F2E8();
    sub_8001E738();
    return sub_8001E544(0, 0);
}

uint32_t sub_8001F054(uint32_t x, uint32_t y)
{
    uint8_t point[12];

    FUNCTION_MARKER(0x8001F054, "SLUS_010.53");
    xport_store_le16(point + 8, x);
    xport_store_le32(point, 0);
    xport_store_le16(point + 10, y);
    return sub_800505F8(point);
}

uint32_t sub_800505F8(const uint8_t *point)
{
    uint32_t flags = xport_load_le32(point);

    FUNCTION_MARKER(0x800505F8, "SLUS_010.53");
    if (!flags || (flags & 2))
    {
        uint16_t x = xport_load_le16(point + 8);
        w_u16(0x8005A3C4, x);
    }
    if (!flags || (flags & 4))
    {
        uint16_t y = xport_load_le16(point + 10);
        w_u16(0x8005A3C6, y);
    }
    rrj_spu_set_cd_volume((int16_t)r_u16(0x8005A3C4), (int16_t)r_u16(0x8005A3C6));
    return 0;
}

void sub_8001F080(void)
{
    SpuVoiceAttr attr = {0};
    FUNCTION_MARKER(0x8001F080, "SLUS_010.53");
    attr.voice = 0x00FFFFFF;
    attr.mask = 0x93;
    attr.volume.left = 0;
    attr.volume.right = 0;
    attr.pitch = 0;
    attr.addr = 0x4096;
    SpuSetVoiceAttr(&attr);
    SpuSetKey(1, attr.voice);
}

uint32_t sub_8001F0D4(void)
{
    uint32_t count;
    uint32_t record;
    uint32_t mask = 0;
    FUNCTION_MARKER(0x8001F0D4, "SLUS_010.53");
    count = r_u32(0x800D6878);
    record = r_u32(0x800D687C);
    while (count)
    {
        if (r_u32(record + 4))
        {
            mask |= 1u << (r_u32(record + 28) & 31u);
            (void)sub_8001FB58(record);
        }
        record += 44;
        --count;
    }
    sub_8001EB60(mask);
    return sub_8001EE94();
}

uint32_t sub_8001F37C(uint32_t alternate, const uint32_t event[3], uint32_t owner)
{
    FUNCTION_MARKER(0x8001F37Cu, "SLUS_010.53");
    uint32_t voice;
    uint32_t handle = 0;
    uint32_t left, right;
    if (r_u32(0x800D69F4u))
        left = right = xport_load_le32(&event[1]) << 7;
    else
    {
        uint32_t pan = xport_load_le32(&event[2]);
        uint32_t distance = ((int32_t)(uint32_t)(pan)) < 129 ? pan : 256u - pan;
        uint32_t volume = xport_load_le32(&event[1]) << 7;
        uint32_t scaled = (distance - 64u) * volume;
        uint32_t quarter = (uint32_t)(((int32_t)(uint32_t)(scaled)) >> 6);
        left = volume - (quarter & (uint32_t)(((int32_t)(uint32_t)(0u - quarter)) >> 31));
        left ^= (uint32_t)(((int32_t)(uint32_t)(pan)) >> 31);
        if (((int32_t)(uint32_t)(pan)) >= 129)
            left = 0u - left;
        right = volume + (quarter & (uint32_t)(((int32_t)(uint32_t)(scaled)) >> 31));
    }
    voice = sub_8001F9C4(1u);
    if (voice)
    {
        uint32_t channel = r_u32(voice + 28u);
        uint32_t generation = r_u32(voice + 4u);
        uint32_t slot;
        w_u32(voice + 8u, 0);
        handle = (channel << 27) + (generation & 0x07FFFFFFu);
        for (slot = 0x800D6870u; slot < 0x800D68ACu; slot += 20u)
        {
            if (!r_u32(slot + 28u))
            {
                w_u32(slot + 28u, 1u);
                w_u32(slot + 44u, handle);
                w_u32(slot + 40u, owner);
                w_u8(slot + 32u, 127u);
                w_u8(slot + 33u, 127u);
                w_u16(slot + 34u, alternate ? 0xB9FFu : 0x98FFu);
                w_u16(slot + 36u, 0x5FCEu);
                w_u32(voice + 8u, slot + 32u);
                break;
            }
        }
        if (!r_u32(voice + 8u))
        {
            (void)sub_8001FB58(voice);
            return 0;
        }
        w_u32(voice + 32u, xport_load_le32(&event[0]));
        channel = r_u32(voice + 28u);
        w_u32(voice + 36u, left);
        w_u32(voice + 40u, right);
        (void)SpuSetReverbVoice(0, 1u << (channel & 31u));
    }
    return handle;
}

uint32_t sub_8001F544(uint32_t count, const uint32_t *packets)
{
    uint32_t mask = 0;
    FUNCTION_MARKER(0x8001F544, "SLUS_010.53");
    if (((int32_t)(uint32_t)(count)) > 0)
    {
        uint32_t index;
        uint32_t table = r_u32(0x800D687C);
        for (index = 0; ((int32_t)(uint32_t)(index)) < ((int32_t)(uint32_t)(count)); ++index)
        {
            uint32_t packet = xport_load_le32(&packets[index]);
            uint32_t slot = packet >> 27;
            if ((r_u32(table + slot * 44 + 4) & 0x07FFFFFF) == (packet & 0x07FFFFFF))
                mask |= 1u << slot;
        }
    }
    return sub_8001EB28(mask);
}

uint32_t sub_8001F6A4(uint32_t handle, const uint8_t event[12])
{
    uint32_t record;
    uint32_t identity;
    uint32_t channel;
    uint8_t packet[64];
    uint32_t product;
    uint32_t left;
    uint32_t right;
    uint32_t amplitude;

    FUNCTION_MARKER(0x8001F6A4, "SLUS_010.53");
    record = r_u32(0x800D687Cu) + 44 * (handle >> 27);
    identity = r_u32(record + 4) & 0x07FFFFFFu;
    if (identity != (handle & 0x07FFFFFFu))
        return identity;
    if (r_u32(0x800D69F4u))
    {
        amplitude = r_u8(r_u32(record + 8));
        product = xport_load_le32(event + 4) * amplitude;
        left = right = product;
    }
    else
    {
        int32_t pan = ((int32_t)(uint32_t)(xport_load_le32(event + 8)));
        uint32_t distance = pan < 129 ? (uint32_t)pan : 256u - (uint32_t)pan;
        uint32_t scaled;
        int32_t quarter;
        uint32_t negative_quarter_mask;

        amplitude = r_u8(r_u32(record + 8));
        product = xport_load_le32(event + 4) * amplitude;
        scaled = (distance - 64u) * product;
        quarter = ((int32_t)(uint32_t)(scaled)) >> 6;
        negative_quarter_mask = (uint32_t)(((int32_t)(uint32_t)(0u - (uint32_t)quarter)) >> 31);
        left = (product - ((uint32_t)quarter & negative_quarter_mask)) ^ (uint32_t)(pan >> 31);
        if (pan >= 129)
            left = 0u - left;
        right = product + ((uint32_t)quarter & (uint32_t)(((int32_t)(uint32_t)(scaled)) >> 31));
    }
    channel = r_u32(record + 28);
    xport_store_le32(packet + 4, 19);
    xport_store_le16(packet + 8, left);
    xport_store_le16(packet + 10, right);
    xport_store_le32(packet, 1u << (channel & 31u));
    xport_store_le16(packet + 20, xport_load_le16(event));
    channel = r_u32(record + 28);
    return sub_80051C38(channel, packet);
}

uint32_t sub_80019C54(uint32_t handle, const uint8_t *event)
{
    uint32_t record;
    uint32_t identity;
    uint32_t channel;
    uint8_t packet[64];
    uint32_t product;
    uint32_t left;
    uint32_t right;
    uint32_t amplitude;

    FUNCTION_MARKER(0x80019C54, "SLUS_010.53");
    record = r_u32(0x800D687Cu) + 44 * (handle >> 27);
    identity = r_u32(record + 4) & 0x07FFFFFFu;
    if (identity != (handle & 0x07FFFFFFu))
        return identity;
    if (r_u32(0x800D69F4u))
    {
        amplitude = r_u8(r_u32(record + 8));
        product = xport_load_le32(event + 4) * amplitude;
        left = right = product;
    }
    else
    {
        int32_t pan = ((int32_t)(uint32_t)(xport_load_le32(event + 8)));
        uint32_t distance = pan < 129 ? (uint32_t)pan : 256u - (uint32_t)pan;
        uint32_t scaled;
        int32_t quarter;
        uint32_t negative_quarter_mask;

        amplitude = r_u8(r_u32(record + 8));
        product = xport_load_le32(event + 4) * amplitude;
        scaled = (distance - 64u) * product;
        quarter = ((int32_t)(uint32_t)(scaled)) >> 6;
        negative_quarter_mask = (uint32_t)(((int32_t)(uint32_t)(0u - (uint32_t)quarter)) >> 31);
        left = (product - ((uint32_t)quarter & negative_quarter_mask)) ^ (uint32_t)(pan >> 31);
        if (pan >= 129)
            left = 0u - left;
        right = product + ((uint32_t)quarter & (uint32_t)(((int32_t)(uint32_t)(scaled)) >> 31));
    }
    channel = r_u32(record + 28);
    xport_store_le32(packet + 4, 19);
    xport_store_le16(packet + 8, left);
    xport_store_le16(packet + 10, right);
    xport_store_le32(packet, 1u << (channel & 31u));
    xport_store_le16(packet + 20, xport_load_le16(event));
    channel = r_u32(record + 28);
    return sub_80051C38(channel, packet);
}

uint32_t sub_8001FBD4(void)
{
    uint32_t base = 0x800D6870;
    int32_t index;
    FUNCTION_MARKER(0x8001FBD4, "SLUS_010.53");
    for (index = 21; index >= 0; --index)
        w_u32(base + 0x58 + index * 4, 0xFFFFFFFF);
    for (index = 23; index >= 0; --index)
        w_u32(base + 0xB0 + index * 4, (uint32_t)index);
    for (index = 24; index >= 0; --index)
        w_u32(base + 0x110 + index * 4, 0xFFFFFFFF);
    w_u32(base + 0x174, 23);
    w_u32(base + 0x178, 0);
    w_u32(base + 0x17C, 0);
    return base;
}

uint32_t sub_80020228(void)
{
    uint32_t pointer;
    FUNCTION_MARKER(0x80020228, "SLUS_010.53");
    pointer = r_u32(0x8005ADB4);
    if (pointer)
        sub_800144B8(pointer);
    w_u32(0x8005ADB4, 0);
    pointer = r_u32(0x8005ADB8);
    if (pointer)
        sub_800144B8(pointer);
    w_u32(0x8005ADB8, 0);
    w_u32(0x8005ADBC, 0);
    return 0;
}

uint32_t sub_80020DB8(void)
{
    FUNCTION_MARKER(0x80020DB8, "SLUS_010.53");
    sub_80020F24();
    sub_80021174();
    sub_80020FF4(0, 0);
    return 0;
}

uint32_t sub_80020DEC(void)
{
    FUNCTION_MARKER(0x80020DEC, "SLUS_010.53");
    if (!r_u8(0x800D754A))
        return 0xFFFFFFFF;
    sub_800212FC();
    return 0;
}

uint32_t sub_80020E30(uint32_t muted)
{
    uint32_t base = 0x800D7548;
    uint8_t packet[12];
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
        xport_store_le32(packet, 0);
    else
    {
        uint32_t timer = r_u32(0x8005B2F8);
        uint32_t pitch = r_u32(0x800D7550);
        uint32_t delay = r_u32(0x800D75A4);
        uint32_t tick = r_u32(timer + 12);
        xport_store_le32(packet, pitch);
        w_u32(0x800D75A8, tick + delay);
    }
    count = r_u32(0x800D754C);
    result = count;
    for (index = 0; index < count; ++index)
    {
        uint32_t record = base + index * 20;
        uint32_t handle = r_u32(record + 16);
        if (handle)
        {
            xport_store_le32(packet + 4, r_u32(record + 20));
            xport_store_le32(packet + 8, r_u32(record + 24));
            handle = r_u32(record + 16);
            sub_8001F6A4(handle, packet);
        }
        count = r_u32(0x800D754C);
        result = index + 1 < count;
    }
    return result;
}

uint32_t sub_80020FF4(uint32_t first, uint32_t second)
{
    FUNCTION_MARKER(0x80020FF4, "SLUS_010.53");
    return register_pair(first, second, 0x8005B4C0, 0x8005B4BC);
}

uint32_t sub_80020F24(void)
{
    FUNCTION_MARKER(0x80020F24, "SLUS_010.53");
    if (r_u8(0x800D7548))
        sub_80021450();
    return 0;
}

uint32_t sub_80021174(void)
{
    uint32_t count;
    uint32_t index;
    FUNCTION_MARKER(0x80021174, "SLUS_010.53");
    count = r_u32(0x800D754C);
    for (index = 0; ((int32_t)(uint32_t)(index)) < ((int32_t)(uint32_t)(count)); ++index)
    {
        uint32_t record = 0x800D7548 + index * 20;
        SpuFree(r_u32(record + 32));
        w_u32(record + 32, 0);
        count = r_u32(0x800D754C);
    }
    return 0;
}

uint32_t sub_800212FC(void)
{
    uint32_t base = 0x800D7548;
    uint32_t packet[3];
    uint32_t handles[2];
    uint32_t count;
    uint32_t index;
    uint32_t pan = 0;
    uint32_t timer, delay, tick;
    uint32_t stride;
    FUNCTION_MARKER(0x800212FC, "SLUS_010.53");
    stride = r_u32(0x800D7598);
    w_u32(0x800D759C, 0);
    w_u32(0x800D7590, stride);
    rrj_spu_irq_callback_psx(0x8002169C);
    sub_80021644();
    if (!r_u32(0x800D6C14))
        return 0;
    packet[0] = r_u8(0x800D7549) ? 0 : r_u32(0x800D7550);
    count = r_u32(0x800D754C);
    for (index = 0; index < count; ++index)
    {
        uint32_t record = base + index * 20;
        uint32_t handle;
        uint32_t volume = r_u32(0x800D6C14);
        w_u32(record + 20, volume);
        packet[1] = volume;
        w_u32(record + 24, pan);
        packet[2] = pan;
        pan += 127;
        handle = sub_8001F37C(0, packet, r_u32(record + 32));
        w_u32(record + 16, handle);
        handles[index] = handle;
        count = r_u32(0x800D754C);
    }
    count = r_u32(0x800D754C);
    sub_8001F544(count, handles);
    timer = r_u32(0x8005B2F8);
    delay = r_u32(0x800D75A4);
    tick = r_u32(timer + 12);
    w_u8(base, 1);
    w_u32(0x800D75A8, tick + delay);
    return 1;
}

uint32_t sub_80021450(void)
{
    uint32_t count;
    uint32_t index;
    uint32_t result = 0x800D0000;
    FUNCTION_MARKER(0x80021450, "SLUS_010.53");
    sub_8002167C();
    rrj_spu_irq_callback_psx(0);
    w_u8(0x800D7548, 0);
    w_u8(0x800D754A, 0);
    count = r_u32(0x800D754C);
    for (index = 0; index < count; ++index)
    {
        (uint32_t)sub_8001F5D4(r_u32(0x800D7558 + index * 20), 0);
        count = r_u32(0x800D754C);
        result = index + 1 < count;
    }
    return result;
}

uint32_t sub_80021644(void)
{
    uint32_t address;
    FUNCTION_MARKER(0x80021644, "SLUS_010.53");
    address = r_u32(0x800D7568) + r_u32(0x800D7590);
    (void)sub_80050C58(address);
    return sub_80050B18(1);
}

uint32_t sub_8002167C(void)
{
    FUNCTION_MARKER(0x8002167C, "SLUS_010.53");
    return sub_80050B18(0);
}

uint32_t sub_800229B0(void)
{
    uint32_t result;
    FUNCTION_MARKER(0x800229B0, "SLUS_010.53");
    result = r_u32(0x8005AE18);
    if (!result)
    {
        w_u32(0x80053464, 0);
        sub_80022CC4();
        result = 1;
        w_u32(0x8005AE18, result);
    }
    w_u32(0x8005AE1C, 0);
    w_u32(0x8005AE20, 0);
    w_u32(0x8005B4E8, 0);
    w_u32(0x8005B4F0, 0);
    return result;
}

uint32_t sub_800229F4(uint32_t mode)
{
    uint32_t result;
    FUNCTION_MARKER(0x800229F4, "SLUS_010.53");
    sub_80022A1C(mode);
    result = r_u32(0x80053464);
    w_u32(0x8005AE18, 0);
    return result;
}

uint32_t sub_80022A1C(uint32_t mode)
{
    FUNCTION_MARKER(0x80022A1C, "SLUS_010.53");
    sub_80043DA4();
    if (r_u32(0x80053464))
        w_u32(r_u32(0x8005B4EC) + 24, 0);
    sub_80022CC4();
    sub_80043DB4();
    return sub_80022A78(mode);
}

uint32_t sub_80023438(void)
{
    uint32_t result;
    FUNCTION_MARKER(0x80023438, "SLUS_010.53");
    CdReadyCallbackPSX(0);
    rrj_cd_read_callback_psx(0u);
    w_u32(0x80010000, 0);
    result = sub_80014528(0x80052318);
    if (((int32_t)(uint32_t)(result)) < 0)
        result = (uint32_t)RRJ_DEBUG_PRINTF((const char *)xport_guest_cptr(0x80010BB0, 22));
    w_u32(0x8005AD5C, 0);
    return result;
}

uint32_t sub_80022FC0(void)
{
    uint32_t changed;
    uint32_t result;
    FUNCTION_MARKER(0x80022FC0, "SLUS_010.53");
    changed = sub_80022A1C(2);
    if (r_u32(r_u32(0x8005B2F8) + 0x30) == 1)
        sub_80024760();
    result = sub_800236C0();
    if (changed)
        return sub_80023438();
    return result;
}

uint32_t sub_80022A78(uint32_t mode)
{
    uint32_t attempts;
    FUNCTION_MARKER(0x80022A78, "SLUS_010.53");
    if (mode == 1)
    {
        while (r_u32(0x80053464))
            rrj_native_wait_events();
    }
    else if (mode == 2)
    {
        for (attempts = 0; attempts < 200 && r_u32(0x80053464); ++attempts)
            (uint32_t)VSync(((int32_t)(uint32_t)(0)));
    }
    return r_u32(0x80053464);
}

void sub_80022B0C(uint32_t busy)
{
    FUNCTION_MARKER(0x80022B0Cu, "SLUS_010.53");
    uint32_t valid = 1, status, record;
    if (busy)
        return;
    for (;;)
    {
        status = sub_80022E38(0x8005B4ECu);
        if (!status)
        {
            record = r_u32(0x8005B4ECu);
            if (r_u32(record) != 512u)
            {
                valid = sub_80030868(r_u32(record + 12u));
                if (!valid)
                    (void)RRJ_DEBUG_PRINTF((const char *)xport_guest_cptr(0x80010B94u, 1));
            }
        }
        if (status == 0xFFFFFF9Du)
        {
            w_u32(0x80053464u, 0);
            return;
        }
        if (!status && valid)
        {
            record = r_u32(0x8005B4ECu);
            w_u32(0x80053464u, 1);
            (void)sub_80022BEC(record);
            return;
        }
    }
}

uint32_t sub_80022BEC(uint32_t record)
{
    FUNCTION_MARKER(0x80022BECu, "SLUS_010.53");
    uint32_t handle = r_u32(record + 4u), offset = r_u32(record + 8u);
    uint32_t buffer, size, state, count, tick, result;
    (void)sub_80014634(handle, offset, 0);
    handle = r_u32(record + 4u);
    buffer = r_u32(record + 16u);
    size = r_u32(record + 28u);
    state = r_u32(0x8005B2F8u);
    count = r_u32(0x8005B4F0u);
    tick = r_u32(state + 12u);
    w_u32(0x8005B4F0u, count + 1u);
    w_u32(0x8005AE1Cu, tick);
    w_u32(0x8005AE20u, 1);
    result = sub_80014894(handle, buffer, size, 0x80022EECu);
    if (((int32_t)(uint32_t)(result)) < 0)
        w_u32(0x8005AE20u, 0);
    return result;
}

void sub_80022CC4(void)
{
    FUNCTION_MARKER(0x80022CC4, "SLUS_010.53");
    w_u32(0x80053474, 0);
    w_u32(0x80053470, 0);
    w_u32(0x8005346C, 0);
    w_u32(0x80053468, 0);
}

uint32_t sub_80022CEC(void)
{
    uint32_t result;
    FUNCTION_MARKER(0x80022CEC, "SLUS_010.53");
    sub_80043DA4();
    result = r_u32(0x80053474);
    sub_80043DB4();
    return result;
}

uint32_t sub_80022D20(const uint32_t record[8])
{
    FUNCTION_MARKER(0x80022D20u, "SLUS_010.53");
    uint32_t index, next, count, kind;
    if (r_u32(0x80053470u) == 49u)
        return 0xFFFFFF9Cu;
    index = r_u32(0x80053468u);
    (void)sub_8001E0B4(xport_guest_ptr(0x800D7740u + (index << 5), 32), record, 32);
    index = r_u32(0x80053468u);
    next = index + 1u;
    w_u32(0x80053468u, next);
    w_u32(0x80053468u, next < 50u ? next : 0);
    (void)sub_80043DA4();
    count = r_u32(0x80053470u);
    kind = xport_load_le32(record);
    w_u32(0x80053470u, count + 1u);
    if (kind == 8u)
        w_u32(0x80053474u, r_u32(0x80053474u) + 1u);
    (void)sub_80043DB4();
    if (r_u32(0x80053470u) == 1u && !r_u32(0x80053464u))
        sub_80022B0C(0);
    return 0;
}

uint32_t sub_80022E38(uint32_t output)
{
    uint32_t read_index;
    uint32_t record;
    FUNCTION_MARKER(0x80022E38, "SLUS_010.53");
    if (!r_u32(0x80053470))
        return 0xFFFFFF9D;
    read_index = r_u32(0x8005346C);
    record = 0x800D7740 + read_index * 32;
    w_u32(output, record);
    ++read_index;
    w_u32(0x8005346C, read_index < 50 ? read_index : 0);
    w_u32(0x80053470, r_u32(0x80053470) - 1);
    if (r_u32(record) == 8)
        w_u32(0x80053474, r_u32(0x80053474) - 1);
    return 0;
}

uint32_t sub_80023148(uint32_t stream, uint32_t count, uint32_t duplicate, uint32_t options)
{
    uint32_t outputs = 0x800D7D80;
    uint32_t job = 0x1F800300;
    uint32_t selected;
    uint32_t index;
    FUNCTION_MARKER(0x80023148, "SLUS_010.53");
    if (r_u32(stream + 20) & 2)
        return 0xFFFFFFFF;
    selected = sub_80030CD8(outputs, count, r_u32(stream + 8), duplicate, options);
    if (((int32_t)(uint32_t)(selected)) <= 0)
        return 1;
    for (index = 0; index < selected; ++index)
    {
        uint32_t record;
        if (sub_80023300(stream))
            break;
        record = r_u32(outputs + index * 4);
        w_u32(job, r_u32(stream + 8));
        w_u32(job + 4, r_u32(stream));
        w_u32(job + 8, r_u32(stream + 4));
        w_u32(job + 12, r_u32(record + 4));
        w_u32(job + 16, r_u32(record + 12));
        w_u32(job + 20, options);
        w_u32(job + 24, 0x80030894);
        w_u32(job + 28, 0x4000);
        if (sub_80022D20((const uint32_t *)xport_guest_cptr(job, 32)) == 0xFFFFFF9C)
            break;
        w_u32(stream + 4, r_u32(stream + 4) + 0x4000);
    }
    if (index < selected)
    {
        sub_80023358(outputs + index * 4, selected - index);
        return 0xFFFFFFFF;
    }
    return 1;
}

uint32_t sub_80023294(uint32_t record)
{
    FUNCTION_MARKER(0x80023294, "SLUS_010.53");
    return r_u32(record + 4);
}

uint32_t sub_800232A0(uint32_t record, uint32_t offset)
{
    uint32_t position;
    uint32_t limit;
    FUNCTION_MARKER(0x800232A0, "SLUS_010.53");
    position = offset + r_u32(record + 12);
    limit = r_u32(record + 16);
    if (position >= limit)
    {
        w_u32(record + 4, limit);
        sub_800233B4(record);
    }
    else
    {
        w_u32(record + 4, position);
        w_u32(record + 20, r_u32(record + 20) & ~2u);
    }
    return r_u32(record + 4);
}

uint32_t sub_80023300(uint32_t record)
{
    FUNCTION_MARKER(0x80023300, "SLUS_010.53");
    if (r_u32(record + 4) < r_u32(record + 16))
        sub_800233F4(record);
    else
        sub_800233B4(record);
    return r_u32(record + 20) & 2;
}

uint32_t sub_80023358(uint32_t records, uint32_t count)
{
    uint32_t index;
    FUNCTION_MARKER(0x80023358, "SLUS_010.53");
    for (index = 0; ((int32_t)(uint32_t)(index)) < ((int32_t)(uint32_t)(count)); ++index)
    {
        uint32_t record = r_u32(records + index * 4);
        sub_80030FD0(r_u32(record + 4));
    }
    return index < count;
}

static uint32_t set_record_active(uint32_t record, uint32_t active)
{
    uint32_t slot;
    uint32_t flags;
    flags = r_u32(record + 20);
    w_u32(record + 20, active ? flags | 2 : flags & ~2u);
    slot = 0x800D6510 + r_u32(record) * 24;
    w_u32(slot, r_u32(slot) & ~0x100u);
    return slot;
}

uint32_t sub_800233B4(uint32_t record)
{
    FUNCTION_MARKER(0x800233B4, "SLUS_010.53");
    return set_record_active(record, 1);
}

uint32_t sub_800233F4(uint32_t record)
{
    FUNCTION_MARKER(0x800233F4, "SLUS_010.53");
    return set_record_active(record, 0);
}

uint32_t sub_800236C0(void)
{
    uint32_t pointer;
    FUNCTION_MARKER(0x800236C0, "SLUS_010.53");
    pointer = r_u32(0x8005B4F8);
    if (pointer)
        sub_800144B8(pointer);
    pointer = r_u32(0x8005AE30);
    if (pointer)
    {
        sub_800144B8(pointer);
        w_u32(0x8005AE30, 0);
        w_u32(0x8005B518, 0);
    }
    return sub_8001460C(r_u32(0x8005B4FC));
}

uint32_t sub_80024760(void)
{
    FUNCTION_MARKER(0x80024760, "SLUS_010.53");
    sub_80020DB8();
    sub_8001460C(r_u32(0x800D6D88));
    return sub_8001460C(r_u32(0x800D6D8C));
}

uint32_t sub_800247A0(void)
{
    FUNCTION_MARKER(0x800247A0, "SLUS_010.53");
    w_u32(0x800CD694, 2);
    w_u32(0x8005B524, (r_u32(0x800CD680) + r_u32(0x8005B510)) >> 1);
    return sub_80024A54();
}

uint32_t sub_800247E8(void)
{
    uint32_t state = r_u32(0x800CD694);
    FUNCTION_MARKER(0x800247E8, "SLUS_010.53");
    if (state == 1)
    {
        w_u32(0x800CD694, 2);
        return sub_80024A54();
    }
    if (!state)
        return 2;
    if (state == 2)
    {
        uint32_t delay = r_u32(0x800D7588);
        if (delay)
        {
            if (((int32_t)(uint32_t)(r_u32(0x8005B520))) > 0)
                return sub_80024E64(r_u32(0x8005B520));
            return delay;
        }
        w_u8(0x800D754A, 1);
        sub_80020DEC();
        w_u32(0x800CD694, 3);
        if (((int32_t)(uint32_t)(r_u32(0x800D7F90))) >= 0)
            return sub_8001460C(r_u32(0x800D7F90));
        return 3;
    }
    if (state == 3)
    {
        uint32_t result = ((int32_t)(uint32_t)(sub_80025190())) < 4;
        if (result)
        {
            result = sub_800249B8();
            if (result)
                return sub_80024CB4(sub_800251D4());
        }
        return result;
    }
    return 3;
}

uint32_t sub_800249B8(void)
{
    FUNCTION_MARKER(0x800249B8, "SLUS_010.53");
    w_u32(0x800CD6A4, 1);
    return 1;
}

uint32_t sub_80024A54(void)
{
    FUNCTION_MARKER(0x80024A54, "SLUS_010.53");
    if (!sub_80024EF4(0xFFFFFFFF))
    {
        w_u32(0x800CD694, 0);
        return 0x800D0000;
    }
    sub_80024DA8();
    w_u32(0x8005B520, 8);
    return sub_80024E64(8);
}

uint32_t sub_80024B20(void)
{
    uint32_t base = 0x80053578;
    uint32_t count = r_u32(base);
    uint32_t active = 0;
    uint32_t available = 0;
    uint32_t index;
    uint32_t attempt;
    FUNCTION_MARKER(0x80024B20, "SLUS_010.53");
    for (index = 0; index < count; ++index)
    {
        uint32_t flags = r_u32(base + 16 + index * 12);
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
            w_u32(base + 16 + index * 12, r_u32(base + 16 + index * 12) & ~2u);
    }
    index = (count * ((uint32_t)sub_80043F00(0xF2000002) & 0xFF)) >> 8;
    for (attempt = 0; attempt < count; ++attempt)
    {
        uint32_t record = base + 16 + index * 12;
        uint32_t flags = r_u32(record);
        if ((flags & 1) && !(flags & 2))
        {
            w_u32(record, flags | 2);
            return index;
        }
        ++index;
        if (index >= count)
            index = 0;
    }
    return 0xFFFFFFFF;
}

uint32_t sub_80024CB4(uint32_t count)
{
    uint32_t stream = 0x800CD670;
    uint32_t remaining;
    uint32_t chunks;
    FUNCTION_MARKER(0x80024CB4, "SLUS_010.53");
    if (r_u32(0x800CE5D4))
        return r_u32(0x800CE5D4);
    remaining = r_u32(0x800CD69C) - sub_80023294(stream);
    if (!remaining)
    {
        if (sub_80024EF4(0xFFFFFFFF))
            remaining = r_u32(0x800CD69C) - sub_80023294(stream);
        else
        {
            w_u32(0x800CD694, 0);
            sub_80020DB8();
        }
    }
    chunks = remaining >> 14;
    if (chunks >= count)
        chunks = count;
    if (r_u32(0x8005B514) >= r_u32(0x8005B524) + r_u32(0x800CD674))
        w_u32(stream, r_u32(0x800CD68C));
    else
        w_u32(stream, r_u32(0x800CD688));
    return sub_80023148(stream, chunks, 1, 0);
}

uint32_t sub_80024AE8(uint32_t index, uint32_t begin, uint32_t end)
{
    uint32_t record = 0x80053578 + index * 12;
    uint32_t first = r_u32(record + 8);
    uint32_t size = r_u32(record + 12);
    FUNCTION_MARKER(0x80024AE8, "SLUS_010.53");
    w_u32(begin, first);
    w_u32(end, first + size);
    return size;
}

uint32_t sub_80024DA8(void)
{
    uint32_t state = r_u32(0x8005B2F8);
    uint32_t result;
    FUNCTION_MARKER(0x80024DA8, "SLUS_010.53");
    w_u32(0x800D7F90, 0xFFFFFFFF);
    result = r_u8(state + 4) & 1;
    if (!result)
    {
        int32_t delta;
        uint32_t index = r_u32(0x8005B1F0);
        result = sub_8001458C((const char *)xport_guest_cptr(0x80010C08, 1), r_u32(0x8005AD5C));
        w_u32(0x800D7F90, result);
        if (((int32_t)(uint32_t)(result)) < 0)
            return result;
        w_u32(0x800D7F98, 8);
        w_u32(0x800D7F9C, 0);
        w_u32(0x800D7FA0, sub_800148BC(result));
        w_u32(0x800D7FA4, 1);
        w_u32(0x800D7F94, index << 17);
        delta = (int32_t)r_s8(0x8005365C + index) * 16384;
        w_u32(0x800CD674, r_u32(0x800CD674) + (uint32_t)delta);
        result = (uint32_t)delta;
    }
    return result;
}

uint32_t sub_80024E64(uint32_t count)
{
    uint32_t stream = 0x800D7F90;
    uint32_t before;
    uint32_t consumed;
    FUNCTION_MARKER(0x80024E64, "SLUS_010.53");
    if (((int32_t)(uint32_t)(r_u32(stream))) < 0 || (r_u8(r_u32(0x8005B2F8) + 4) & 1))
        stream = 0x800CD670;
    before = r_u32(stream + 4);
    sub_80023148(stream, count, 0, 0);
    consumed = (r_u32(stream + 4) - before) >> 14;
    w_u32(0x8005B520, r_u32(0x8005B520) - consumed);
    return r_u32(0x8005B520);
}

uint32_t sub_80024EF4(uint32_t index)
{
    uint32_t begin = 0x1F800330;
    uint32_t end = 0x1F800334;
    FUNCTION_MARKER(0x80024EF4, "SLUS_010.53");
    if (((int32_t)(uint32_t)(index)) < 0)
        index = sub_80024B20();
    w_u32(0x8005B1F0, index);
    if (((int32_t)(uint32_t)(index)) < 0 || index >= r_u32(0x80053578))
        return 0;
    sub_80024AE8(index, begin, end);
    w_u32(0x800CD69C, r_u32(end));
    sub_800232A0(0x800CD670, r_u32(begin));
    return 1;
}

uint32_t sub_80025190(void)
{
    uint32_t result;
    FUNCTION_MARKER(0x80025190, "SLUS_010.53");
    result = r_u32(0x800D7584) - r_u32(0x800D7588);
    return result + r_u32(0x800D7F88) + sub_80022CEC();
}

uint32_t sub_800251D4(void)
{
    FUNCTION_MARKER(0x800251D4, "SLUS_010.53");
    return r_u32(0x800D7588) + 2;
}

uint32_t sub_8002E080(void)
{
    uint32_t table;
    uint32_t phase;
    FUNCTION_MARKER(0x8002E080, "SLUS_010.53");
    table = sub_8001447C(2048, 0);
    w_u32(0x8005B560, table);
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
        w_u16(table + phase * 2, (uint16_t)packed);
    }
    return 0;
}

uint32_t sub_80030868(uint32_t index)
{
    uint32_t base;
    FUNCTION_MARKER(0x80030868, "SLUS_010.53");
    base = r_u32(0x8005ACBC);
    return (r_u32(base + index * 36 + 44) ^ 1) & 1;
}

uint32_t sub_80030FD0(uint32_t index)
{
    uint32_t record;
    FUNCTION_MARKER(0x80030FD0, "SLUS_010.53");
    record = r_u32(0x8005ACBC) + index * 36 + 44;
    return (uint32_t)sub_800313EC(record);
}

uint32_t sub_8003E1E8(uint32_t object, uint32_t id, uint32_t direction_out)
{
    uint32_t header = 0;
    uint32_t link;
    uint32_t route;
    uint32_t entry;
    int32_t offset;
    FUNCTION_MARKER(0x8003E1E8, "SLUS_010.53");
    if (object && r_s16(object + 16) == 1 && !r_u32(object + 12))
        header = sub_80039AFC(r_u32(object));
    if (!header)
        return 0;
    link = sub_8003A37C(header, id);
    if (!link)
        return 0;
    route = sub_80039C90(object, id);
    if (!route)
        return 0;
    entry = r_u32(object + 48) + 28 * (uint32_t)(int32_t)r_s16(route + 20);
    if (!entry)
        return 0;
    if (r_s16(link + 2) > 0)
        offset = (int32_t)r_s16(entry + 18) << 4;
    else
        offset = ((int32_t)r_s16(entry + 18) + (int32_t)r_s16(entry + 16)) * 16 - 16;
    entry = r_u32(object + 56) + (uint32_t)offset;
    if (r_s16(entry + 4) < 0)
        return 0;
    w_u32(direction_out, (uint32_t)(int32_t)r_s16(link + 2));
    return r_u32(object + 60) + 264 * (uint32_t)(int32_t)r_s16(entry + 4);
}

uint32_t sub_80030CD8(uint32_t outputs, uint32_t count, uint32_t kind, uint32_t duplicate, uint32_t options)
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
        sub_80031368(r_u32(outputs + index * 4), kind);
    if (!duplicate || selected < 2 || selected >= count)
        return selected;
    for (index = 0; index < count - selected; ++index)
    {
        uint32_t record = r_u32(outputs + index * 4);
        w_u32(outputs + (selected + index) * 4, record);
        w_u32(record, r_u32(record) + 0x1000);
        w_u32(r_u32(0x8005ACBC) + 28, r_u32(r_u32(0x8005ACBC) + 28) + 1);
    }
    return count;
}

uint32_t sub_80031064(uint32_t output, uint32_t count)
{
    uint32_t base = r_u32(0x8005ACBC);
    uint32_t current = r_u32(base + 0xA50);
    uint32_t end = r_u32(base + 0xA54);
    uint32_t found = 0;
    uint32_t record;
    uint32_t locked = !r_u32(0x800541D0);
    FUNCTION_MARKER(0x80031064, "SLUS_010.53");
    if (locked)
        sub_80043DA4();
    record = base + current * 36 + 44;
    while (current <= end && ((int32_t)(uint32_t)(found)) < ((int32_t)(uint32_t)(count)))
    {
        if (!r_u32(record))
        {
            w_u32(output, record);
            output += 4;
            ++found;
            w_u32(record, 0x100);
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
    uint32_t base = r_u32(0x8005ACBC);
    uint32_t first = r_u32(base + 0xA40 + kind * 8);
    uint32_t last = r_u32(base + 0xA44 + kind * 8);
    uint32_t selected = 0;
    uint32_t skipped = 0;
    uint32_t index;
    uint32_t locked = !r_u32(0x800541D0);
    FUNCTION_MARKER(0x80031170, "SLUS_010.53");
    if (locked)
        sub_80043DA4();
    for (index = first; index <= last && ((int32_t)(uint32_t)(selected)) < ((int32_t)(uint32_t)(count)); ++index)
    {
        uint32_t record = base + 44 + index * 36;
        if (!r_u32(record))
        {
            if (bypass_reserved || ((int32_t)(uint32_t)(skipped)) >= ((int32_t)(uint32_t)(r_u32(base + 0xA5C))))
            {
                w_u32(outputs + selected * 4, record);
                ++selected;
                w_u32(record, 256);
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

uint32_t sub_80031368(uint32_t record, uint32_t kind)
{
    uint32_t base;
    uint32_t result = 16;
    FUNCTION_MARKER(0x80031368, "SLUS_010.53");
    w_u32(record, kind | 4);
    sub_800322D0(3, 2);
    base = r_u32(0x8005ACBC);
    if (kind == 8)
    {
        result = r_u32(base + 28) + 1;
        w_u32(base + 28, result);
    }
    else if (kind == 16)
    {
        result = r_u32(base + 24) + 1;
        w_u32(base + 24, result);
    }
    return result;
}

void sub_80043DA4(void)
{
    FUNCTION_MARKER(0x80043DA4u, "SLUS_010.53");
    (void)xport_bios_enter_critical();
    rrj_host_context()->cpu_status &= ~0x401u;
}

void sub_80043DB4(void)
{
    FUNCTION_MARKER(0x80043DB4u, "SLUS_010.53");
    rrj_host_context()->cpu_status |= 0x401u;
    spu_dispatch_pending_irq();
    xport_bios_exit_critical();
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
    previous = r_u32(0x80055F0C);
    w_u32(0x80055F0C, value);
    return previous;
}

uint32_t sub_8004DE44(uint32_t handle)
{
    FUNCTION_MARKER(0x8004DE44, "SLUS_010.53");
    return (uint32_t)PCclose(((int32_t)(uint32_t)(handle)));
}

uint32_t sub_8004DE78(uint32_t handle, void *buffer, uint32_t size)
{
    uint32_t total = 0;
    FUNCTION_MARKER(0x8004DE78, "SLUS_010.53");
    while (size)
    {
        uint32_t chunk = size > 0x8000 ? 0x8000 : size;
        uint32_t result = sub_8004DF38(0, handle, chunk, buffer);
        total += result;
        if (result == 0xFFFFFFFF)
            return 0xFFFFFFFF;
        buffer = (uint8_t *)buffer + result;
        size -= result;
        if (((int32_t)(uint32_t)(result)) < ((int32_t)(uint32_t)(chunk)))
            return total;
    }
    return total;
}

uint32_t sub_8004DF38(uint32_t zero, uint32_t handle, uint32_t size, void *buffer)
{
    FUNCTION_MARKER(0x8004DF38, "SLUS_010.53");
    return (uint32_t)PCread(((int32_t)(uint32_t)(handle)), (char *)buffer, ((int32_t)(uint32_t)(size)));
}

uint32_t sub_8004F2C8(void)
{
    FUNCTION_MARKER(0x8004F2C8, "SLUS_010.53");
    return _SpuInit(0u);
}

void sub_8004F2E8(void)
{
    FUNCTION_MARKER(0x8004F2E8, "SLUS_010.53");
    if (r_u32(0x8005A40C) == 1)
    {
        w_u32(0x8005A40C, 0);
        sub_80043DA4();
        w_u32(0x8005A454, 0);
        w_u32(0x8005A458, 0);
        (void)DMACallback(4u, NULL);
        (void)CloseEventPSX(r_u32(0x8005A3A4));
        (void)DisableEventPSX(r_u32(0x8005A3A4));
        sub_80043DB4();
    }
}

uint32_t sub_80051018(void)
{
    uint32_t state;
    FUNCTION_MARKER(0x80051018, "SLUS_010.53");
    state = r_u32(0x8005A438);
    state = state == 1 ? state : 0;
    w_u32(0x8005A3AC, state);
    return state;
}

uint32_t sub_80051058(uint32_t callback)
{
    uint32_t previous;
    FUNCTION_MARKER(0x80051058, "SLUS_010.53");
    previous = r_u32(0x8005A454);
    if (callback != previous)
        w_u32(0x8005A454, callback);
    return previous;
}

uint32_t sub_8009277C(uint32_t object)
{
    FUNCTION_MARKER(0x8009277C, "RASHCDG.BIN");
    return sub_80092784(object);
}

uint32_t sub_80092784(uint32_t object)
{
    int32_t scalar_sign_92;

    uint32_t state = r_u32(0x8005B2F8);
    uint32_t secondary = r_u32(object + 852);
    uint32_t animation = r_u32(object + 540);
    uint32_t options = 0x10;
    uint32_t record;
    uint32_t i;
    FUNCTION_MARKER(0x80092784, "RASHCDG.BIN");
    if (r_u8(state + 57) >= 2 && r_s8(secondary + 571) != -1)
        (void)sub_80095AEC(secondary);
    if (r_u32(0x8005B254) && ((uint32_t)r_u16(object + 172) < r_u32(state + 48) || (r_u16(object + 320) & 4)) && r_u32(0x800CE178) == r_u32(0x800CE17C))
        (void)sub_8008CC94();
    if (!animation)
    {
        animation = sub_80012884(0x800CE170, object);
        w_u32(object + 540, animation);
        if (!animation)
            return sub_800903F4(object, 0);
    }
    record = object + 948 + 8 * (int32_t)r_s8(object + 946);
    w_u16(record, 1);
    w_u16(record + 2, 224);
    w_u16(record + 6, (uint16_t)((((uint32_t)(2180u * r_u32(state + 16) + 0x8000u)) >> 16) | 0xC000u));
    w_u32(secondary + 552, (r_u32(secondary + 552) | 0x40u) & 0xBFE67FC7u);
    w_u32(object + 720, 0);
    (void)sub_80012838(object, secondary, 2, 0);
    w_u8(object + 72, 3);
    if (r_u32(object + 856) && r_u32(object + 1088))
    {
        uint32_t linked = r_u32(object + 856);
        (void)sub_800C4550(4, secondary, options);
        if ((r_u8(secondary + 572) & 0x10) && r_u8(state + 57) != 1)
        {
            uint32_t linked_secondary = r_u32(linked + 852);
            w_u16(linked_secondary + 320, r_u16(object + 320));
            (void)sub_80012838(object, linked_secondary, 2, 1);
            w_u8(object + 72, 3);
            w_u32(linked_secondary + 552, (r_u32(linked_secondary + 552) | 0x40u) & 0xBFE67FE7u);
            (void)sub_800C4550(77, linked_secondary, options);
        }
        w_u32(linked + 720, 0);
        (void)sub_80092AD4(object);
    }
    else
    {
        int32_t velocity = ((int32_t)(uint32_t)(r_u32(object + 652)));
        (void)sub_80012858(animation, r_u32(0x800CE1A0));
        if ((int16_t)r_u16(object + 320) != 0 && velocity < 0)
            options |= 0x100;
        if (((scalar_sign_92 = ((int32_t)(velocity)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_92 + ((int32_t)(velocity))) ^ scalar_sign_92))))) != 102943u)
            options |= 0x8000;
        (void)sub_800C4550(0, secondary, options);
    }
    for (i = 0; i < 9; ++i)
        w_u16(object + 432 + 2 * i, r_u16(object + 516 + 2 * i));
    {
        uint32_t id = r_u16(object + 172);
        if (id < r_u32(state + 48))
        {
            uint32_t player = 0x800CD898 + 1132 * id;
            (void)sub_80018440(id, 0);
            w_u32(player + 548, r_u32(player + 548) & 0xF5FFFF7Fu);
            return sub_800235B0(id, object);
        }
    }
    return 0;
}

uint32_t sub_800BF978(uint32_t actor)
{
    uint32_t descriptor = r_u32(actor + 1084);
    uint32_t secondary = r_u32(actor + 852);
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
    if (!(r_u32(actor + 560) & 0x08000000u) && !(r_u8(secondary + 572) & 0x40))
    {
        w_u8(secondary + 574, 0);
        xport_guest_fill(secondary + 575, 0x3F, 5);
    }
    else
        (void)sub_8001E08C(xport_guest_ptr(secondary + 574, 1), xport_guest_ptr(0x800CCAD0 + 6 * variant, 1), 6);
    return state & 0xFF;
}

uint32_t sub_80099710(uint32_t actor, uint32_t first, uint32_t second, uint32_t scale)
{
    int32_t scalar_sign_93;

    uint32_t product = (uint32_t)sub_80093BE0(actor + 528, first, second) * 25736u;
    int32_t step = ((int32_t)(uint32_t)(product)) >> 8;
    int32_t threshold = (int32_t)sub_8001FC90(0x40000, ((int32_t)(uint32_t)(scale)));
    uint32_t result;
    FUNCTION_MARKER(0x80099710, "RASHCDG.BIN");
    if (threshold < 3431)
        threshold = 3431;
    if (((scalar_sign_93 = ((int32_t)(step)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_93 + ((int32_t)(step))) ^ scalar_sign_93))))) < (uint32_t)threshold)
        return sub_8009277C(r_u32(actor + 596));
    if (((int32_t)(uint32_t)(r_u32(actor + 600))) > 0)
    {
        if (step < 0)
            step = ((int32_t)(uint32_t)((uint32_t)step + 0x6487Eu));
        w_u32(actor + 488, 0xFFFC0000u);
    }
    else
    {
        if (step < 0)
            step = -step;
        else
            step = 0x6487E - step;
        w_u32(actor + 488, 0x40000);
    }
    if (!r_u32(actor + 480))
    {
        int32_t delta = ((int32_t)(uint32_t)(r_u32(actor + 592) - r_u32(actor + 600)));
        uint32_t numerator = delta > 0 ? (uint32_t)delta : (uint32_t)-delta;
        uint32_t denominator = step > 0 ? (uint32_t)step : (uint32_t)-step;
        result = sub_80010028(numerator, denominator);
        if ((delta > 0 && step <= 0) || (delta <= 0 && step > 0))
            result = 0u - result;
        w_u32(actor + 592, result);
        w_u32(actor + 480, 0x80000);
    }
    result = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(actor + 592))), step);
    w_u32(actor + 484, r_u32(actor + 600) + result);
    return result;
}

uint32_t sub_800C12D8(uint32_t actor, uint32_t flags)
{
    uint32_t secondary = r_u32(actor + 852);
    uint32_t state;
    uint32_t table;
    int32_t index;
    uint32_t event;
    FUNCTION_MARKER(0x800C12D8, "RASHCDG.BIN");
    xport_update_u8(secondary + 572, XPORT_MEMORY_UPDATE_AND, 0xFDu);
    (void)sub_800BF978(actor);
    index = r_s8(secondary + 574 + r_u8(secondary + 570));
    state = r_u8(secondary + 569);
    w_u32(secondary + 560, 0);
    w_u32(secondary + 564, 0);
    w_u8(secondary + 568, 0);
    table = r_u32(r_u32(0x8005AD4C) + 12 * state + 8);
    event = r_u16(table + 12 * (uint32_t)index);
    return sub_800C4550(event, secondary, flags | 0x10u);
}

uint32_t sub_800C110C(uint32_t actor, uint32_t other)
{
    uint32_t secondary = r_u32(actor + 852);
    uint32_t linked = r_u32(actor + 856);
    uint32_t flags = 0;
    FUNCTION_MARKER(0x800C110C, "RASHCDG.BIN");
    xport_update_u8(secondary + 572, XPORT_MEMORY_UPDATE_AND, 0xFEu);
    if (!other && !linked)
        flags = (sub_8001FC58() & 1) << 8;
    else if (!r_u32(actor + 1088))
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
        uint32_t identity = r_u32(other + 360);
        uint32_t actor_identity = r_u32(actor + 360);
        if (identity != actor_identity || ((identity >> 16) && r_u32(other + 336) != r_u32(actor + 336)))
            (void)sub_800B6AAC(xport_guest_ptr(other + 184, 12), xport_guest_ptr(actor + 432, 6), xport_guest_ptr(actor + 184, 12));
        if (((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(other + 184, 12), xport_guest_ptr(actor + 432, 6), xport_guest_ptr(actor + 184, 12)))) < 0)
            flags = 0x100;
    }
    return sub_800C12D8(actor, flags);
}

uint32_t sub_800C122C(uint32_t actor, uint32_t unused, uint32_t flags)
{
    uint32_t descriptor = r_u32(actor + 1084);
    FUNCTION_MARKER(0x800C122C, "RASHCDG.BIN");
    if ((r_u8(descriptor + 61) & 3) == 2)
    {
        (void)sub_800B92C0(descriptor);
        descriptor = r_u32(actor + 1084);
    }
    if ((r_u8(descriptor + 60) & 0x80) && (!r_u8(descriptor + 46) || r_u8(descriptor + 46) == 4) && r_u32(0x800CE178) == r_u32(0x800CE17C))
        w_u8(descriptor + 60, 0x20);
    return sub_800C12D8(actor, flags);
}

uint32_t sub_8003B96C(uint32_t actor)
{
    FUNCTION_MARKER(0x8003B96C, "SLUS_010.53");
    if (sub_8003B8F4(actor + 172))
        return r_u32(actor + 324);
    return 0x7FFFF000;
}

uint32_t sub_800138E8(uint32_t actor, uint32_t mode)
{
    uint32_t state = r_u32(0x8005B2F8);
    uint32_t descriptor = r_u32(actor + 1084);
    uint32_t players = r_u32(state + 48);
    uint32_t actor_id = r_u16(actor + 172);
    uint32_t count = 1;
    uint32_t score;
    uint32_t object;
    int32_t index;
    FUNCTION_MARKER(0x800138E8, "SLUS_010.53");
    if ((r_u8(descriptor + 1) & 0x0F) == 2 && r_u8(descriptor + 39) < 0xF7 && (actor_id >= players || ((r_u8(state + 4) & 1) && !(r_u32(0x8005AD48) & 1))))
        return r_u32(0x8005B1F8) + 1;
    if (r_u32(descriptor + 40) || r_u8(descriptor + 39) >= 0xF8)
        return r_u8(descriptor + 39);
    score = sub_8003B96C(actor);
    index = ((int32_t)(uint32_t)(r_u32(r_u32(0x800CE4DC))));
    object = r_u32(0x800CE4D0);
    while (index >= 0)
    {
        uint32_t other_descriptor = r_u32(object + 1084);
        if ((actor_id < players || (r_u8(other_descriptor + 1) & 0x0F) != 2) && r_u16(object + 172) != actor_id && r_u32(0x8005B1F8) >= r_u8(other_descriptor + 39))
        {
            if (mode == 1)
                count += ((int32_t)(uint32_t)(r_u32(other_descriptor + 40))) > 0;
            else
            {
                uint32_t other_score = sub_8003B96C(object);
                count += ((int32_t)(uint32_t)(r_u32(other_descriptor + 40))) > 0 || ((int32_t)(uint32_t)(other_score)) < ((int32_t)(uint32_t)(score));
            }
        }
        --index;
        object += r_u32(0x800CE4D4);
    }
    return count;
}

uint32_t sub_800BAA2C(uint32_t actor, uint32_t record, uint32_t delta)
{
    int32_t scalar_sign_94;

    uint32_t state = r_u32(0x8005B2F8);
    uint32_t actor_id = r_u16(actor + 172);
    uint32_t players = r_u32(state + 48);
    uint32_t result = 0;
    uint32_t selected = 0;
    uint8_t mode = r_u8(state + 4);
    FUNCTION_MARKER(0x800BAA2C, "RASHCDG.BIN");
    if (mode & 1)
    {
        if (actor_id < players)
        {
            uint32_t descriptor = r_u32(actor + 1084);
            if ((r_u8(descriptor + 1) & 0x0F) == 2)
            {
                uint32_t flags = r_u32(0x8005AD48);
                if (flags & 1)
                {
                    uint32_t active = 0;
                    uint32_t object = r_u32(0x800CE4D0);
                    int32_t index = ((int32_t)(uint32_t)(r_u32(r_u32(0x800CE4DC))));
                    uint32_t rank = sub_800138E8(actor, 0);
                    while (index >= 0)
                    {
                        uint32_t other_secondary = r_u32(object + 852);
                        uint32_t kind = r_u32(other_secondary + 604) - 3;
                        if (r_u16(object + 320) && kind < 2)
                            ++active;
                        --index;
                        object += r_u32(0x800CE4D4);
                    }
                    result = ((int32_t)(uint32_t)(rank)) < ((int32_t)(uint32_t)(r_u32(0x8005B1F8) - (active + 1)));
                    if (!result)
                        selected = 1;
                }
                else if (flags & 2)
                {
                    uint32_t player = 0x800D6198 + 224 * actor_id;
                    w_u32(0x8005B290, 0);
                    w_u32(player + 116, r_u32(state + 16));
                    w_u32(0x8005AD48, (flags & 0xFFFFFFFDu) | 4);
                    w_u32(player + 108, 0);
                    result = player;
                }
                else if (flags & 4)
                {
                    uint32_t stopped;
                    w_u32(0x8005B290, r_u32(0x8005B290) + delta);
                    w_u32(0x1F8003F0, 0);
                    result = (uint32_t)sub_8009DA4C(actor, 0x1F8003F0);
                    stopped = r_u32(0x1F8003F0);
                    if (((int32_t)(uint32_t)(result)) < 0 || ((int32_t)(uint32_t)(r_u32(0x800530E4 + 4 * r_u32(state + 60)))) < ((int32_t)(uint32_t)(r_u32(0x8005B290))) || stopped)
                    {
                        result = (uint8_t)(r_u8(descriptor + 39) + 4) < 2;
                        if (!result)
                        {
                            result = sub_800A0708(actor);
                            selected = 1;
                        }
                    }
                }
            }
        }
    }
    else if (mode == 44 && actor_id >= players)
    {
        uint32_t primary = r_u32(0x8005B38C);
        int32_t distance = ((int32_t)(uint32_t)(r_u32(primary + 368) - (r_u16(0x80053182) << 16)));
        uint32_t magnitude = ((scalar_sign_94 = ((int32_t)(distance)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_94 + ((int32_t)(distance))) ^ scalar_sign_94)))));
        if (r_u32(actor + 180) < 18)
        {
            if (r_u8(state + 57) == 4 && ((int32_t)(uint32_t)((uint32_t)(int32_t)r_s8(0x8005ADF0) << 16)) < ((int32_t)(uint32_t)(magnitude)))
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
        uint32_t threshold = r_u16(record + 4) << 16;
        selected = ((int32_t)(uint32_t)(27904u * r_u32(state + 16))) >= ((int32_t)(uint32_t)(threshold));
        result = selected;
    }
    if (selected)
    {
        uint32_t value;
        if (r_s8(actor + 946) >= 2)
            record = actor + 956;
        w_u16(record, 4);
        w_u16(record + 4, 0);
        w_u16(record + 6, 0);
        value = ((2180u * r_u32(state + 16) + 0x8000u) >> 16) | 0xC000u;
        w_u16(record + 6, (uint16_t)value);
        w_u32(actor + 720, 0);
        result = value;
    }
    return result;
}

uint32_t sub_80098F2C(uint32_t actor, uint32_t direction, uint32_t source, uint32_t magnitude, uint32_t fixed_result)
{
    int32_t alignment = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(actor + 516, 6), xport_guest_ptr(direction, 6))));
    int32_t delta[3];
    int32_t scale;
    int32_t sum;
    uint32_t first;
    uint32_t second;
    uint32_t result;
    uint32_t scratch[3];
    FUNCTION_MARKER(0x80098F2C, "RASHCDG.BIN");
    delta[0] = ((int32_t)(uint32_t)(r_u32(actor + 184) - r_u32(actor + 580)));
    delta[1] = ((int32_t)(uint32_t)(r_u32(actor + 188) - r_u32(actor + 584)));
    delta[2] = ((int32_t)(uint32_t)(r_u32(actor + 192) - r_u32(actor + 588)));
    if (alignment < 655)
    {
        if (alignment > 0)
            alignment = 655;
        else if (alignment >= -654)
            alignment = -655;
    }
    if (alignment >= 0)
        scale = ((int32_t)(uint32_t)(0x80000000u / (uint32_t)(alignment >> 1)));
    else
        scale = ((int32_t)(uint32_t)(0u - 0x80000000u / ((uint32_t)(-alignment) >> 1)));
    sum = ((int32_t)((int32_t)(((int64_t)((int32_t)(delta[0])) * ((int32_t)((int32_t)r_s16(direction) * 16))) >> 16)));
    sum = ((int32_t)(uint32_t)((uint32_t)sum + (uint32_t)((int32_t)((int32_t)(((int64_t)((int32_t)(delta[1])) * ((int32_t)((int32_t)r_s16(direction + 2) * 16))) >> 16)))));
    sum = ((int32_t)(uint32_t)((uint32_t)sum + (uint32_t)((int32_t)((int32_t)(((int64_t)((int32_t)(delta[2])) * ((int32_t)((int32_t)r_s16(direction + 4) * 16))) >> 16)))));
    first = (uint32_t)sub_8001FC90(sum, scale);
    w_u32(actor + 592, first);
    sum = ((int32_t)((int32_t)(((int64_t)((int32_t)(delta[0])) * ((int32_t)((int32_t)r_s16(actor + 528) * 16))) >> 16)));
    sum = ((int32_t)(uint32_t)((uint32_t)sum + (uint32_t)((int32_t)((int32_t)(((int64_t)((int32_t)(delta[1])) * ((int32_t)((int32_t)r_s16(actor + 530) * 16))) >> 16)))));
    sum = ((int32_t)(uint32_t)((uint32_t)sum + (uint32_t)((int32_t)((int32_t)(((int64_t)((int32_t)(delta[2])) * ((int32_t)((int32_t)r_s16(actor + 532) * 16))) >> 16)))));
    second = (uint32_t)sub_8001FC90(sum, scale);
    w_u32(actor + 600, second);
    if (((int32_t)(uint32_t)(first ^ second)) < 0)
    {
        uint32_t flags = r_u32(actor + 552);
        w_u32(actor + 552, flags | (scale >= 0 ? 0x500u : 0x300u));
        result = fixed_result ? 0x7FFF0000u : sub_8004CF74(magnitude) << 2;
        w_u32(actor + 484, result);
        return result;
    }
    (void)sub_8002EAD8(xport_guest_ptr(actor + 580, 12), xport_guest_ptr(source, 6), 0u - second, xport_guest_ptr(actor + 456, 12));
    (void)sub_8002EAD8(xport_guest_ptr(actor + 456, 12), xport_guest_ptr(actor + 516, 6), first, scratch);
    scratch[0] -= r_u32(actor + 184);
    scratch[1] -= r_u32(actor + 188);
    scratch[2] -= r_u32(actor + 192);
    (void)sub_8002EAD8(xport_guest_ptr(actor + 456, 12), xport_guest_ptr(source, 6), second, scratch);
    scratch[0] -= r_u32(actor + 580);
    scratch[1] -= r_u32(actor + 584);
    scratch[2] -= r_u32(actor + 588);
    w_u32(actor + 480, 0);
    result = (r_u32(actor + 552) & 0xFFFFFFE7u) | 0x10u;
    w_u32(actor + 552, result);
    return result;
}

uint32_t sub_80098A50(uint32_t actor, uint32_t source, uint32_t axis_out, uint32_t direction_out)
{
    int32_t scalar_value_95;
    int32_t scalar_sign_95;
    int32_t scalar_value_96;
    int32_t scalar_sign_96;
    int32_t scalar_value_97;
    int32_t scalar_sign_97;
    int32_t scalar_value_98;
    int32_t scalar_sign_98;

    uint32_t source_value = r_u32(source + 652);
    uint32_t index;
    uint32_t scale = 0x10000;
    FUNCTION_MARKER(0x80098A50, "RASHCDG.BIN");
    if (((scalar_value_95 = (((int32_t)(uint32_t)(source_value)))), (scalar_sign_95 = scalar_value_95 >> 31), (((uint32_t)((uint32_t)((scalar_sign_95 + scalar_value_95) ^ scalar_sign_95))))) == 102943)
    {
        for (index = 0; index < 3; ++index)
        {
            w_u16(axis_out + 2 * index, r_u16(source + 438 + 2 * index));
            w_u16(direction_out + 2 * index, r_u16(source + 444 + 2 * index));
        }
    }
    else
    {
        for (index = 0; index < 3; ++index)
        {
            uint16_t axis = r_u16(source + 516 + 2 * index);
            if (((int32_t)(uint32_t)(source_value)) > 0)
                axis = (uint16_t)(0u - axis);
            w_u16(axis_out + 2 * index, axis);
            w_u16(direction_out + 2 * index, r_u16(source + 528 + 2 * index));
        }
    }
    (void)sub_8002EAD8(xport_guest_ptr(source + 504, 12), xport_guest_ptr(axis_out, 6), 0xFFFEE000, xport_guest_ptr(actor + 580, 12));
    (void)sub_8002EAD8(xport_guest_ptr(actor + 580, 12), xport_guest_ptr(direction_out, 6), 0xFFFFF7FD, xport_guest_ptr(actor + 580, 12));
    if (((int32_t)(uint32_t)(source_value)) < 0)
    {
        for (index = 0; index < 3; ++index)
            w_u16(direction_out + 2 * index, (uint16_t)(0u - r_u16(direction_out + 2 * index)));
    }
    if (r_u32(actor + 552) & 0x18)
        return 1;
    if (!r_u32(actor + 480))
    {
        int32_t dx = ((int32_t)(uint32_t)(r_u32(actor + 184) - r_u32(actor + 580)));
        int32_t dz = ((int32_t)(uint32_t)(r_u32(actor + 192) - r_u32(actor + 588)));
        int32_t sign_x = dx >> 31;
        int32_t sign_z = dz >> 31;
        int32_t sx = ((int32_t)(uint32_t)((uint32_t)(dx >> 16) + (uint32_t)sign_x)) ^ sign_x;
        int32_t sz = ((int32_t)(uint32_t)((uint32_t)(dz >> 16) + (uint32_t)sign_z)) ^ sign_z;
        int32_t maximum = sx > sz ? sx : sz;
        int32_t minimum = sx > sz ? sz : sx;
        int32_t mixed = minimum + (minimum >> 1);
        int32_t metric = maximum - (maximum >> 5) - (maximum >> 7) + (mixed >> 2) + (mixed >> 6);
        if (metric < 6)
        {
            uint32_t delta = 0x1F800340;
            w_u32(delta, (uint32_t)dx);
            w_u32(delta + 4, r_u32(actor + 188) - r_u32(actor + 584));
            w_u32(delta + 8, (uint32_t)dz);
            scale = sub_8002E548(xport_guest_ptr(delta, 12)) >> 1;
            if (scale > 0x10000)
                scale = 0x10000;
            if (scale < 0x1999)
            {
                (void)sub_8009277C(source);
                return 0;
            }
        }
    }
    if (!(r_u32(actor + 552) & 0x800))
    {
        uint32_t delta = 0x1F800340;
        uint32_t normalized = 0x1F800350;
        uint32_t matrix = 0x1F800360;
        uint32_t rotated = 0x1F800380;
        int32_t first = (int32_t)sub_8001FC90((uint32_t)((int32_t)r_s16(axis_out + 4) << 4), r_u32(actor + 184) - r_u32(actor + 580));
        int32_t second = (int32_t)sub_8001FC90((uint32_t)((int32_t)r_s16(axis_out) << 4), r_u32(actor + 192) - r_u32(actor + 588));
        int32_t side = first - second;
        uint32_t valid = 1;
        uint32_t length;
        uint32_t reciprocal;
        uint32_t selected = normalized;
        (void)sub_8002EAD8(xport_guest_ptr(actor + 580, 12), xport_guest_ptr(direction_out, 6), side > 0 ? scale : 0u - scale, xport_guest_ptr(actor + 456, 12));
        for (index = 0; index < 3; ++index)
            w_u32(delta + 4 * index, r_u32(actor + 184 + 4 * index) - r_u32(actor + 456 + 4 * index));
        while (((scalar_value_96 = (((int32_t)(uint32_t)(r_u32(delta))))), (scalar_sign_96 = scalar_value_96 >> 31), (((uint32_t)((uint32_t)((scalar_sign_96 + scalar_value_96) ^ scalar_sign_96))))) > 0x5A8000 || ((scalar_value_97 = (((int32_t)(uint32_t)(r_u32(delta + 4))))), (scalar_sign_97 = scalar_value_97 >> 31), (((uint32_t)((uint32_t)((scalar_sign_97 + scalar_value_97) ^ scalar_sign_97))))) > 0x5A8000 || ((scalar_value_98 = (((int32_t)(uint32_t)(r_u32(delta + 8))))), (scalar_sign_98 = scalar_value_98 >> 31), (((uint32_t)((uint32_t)((scalar_sign_98 + scalar_value_98) ^ scalar_sign_98))))) > 0x5A8000)
        {
            valid = 0;
            for (index = 0; index < 3; ++index)
                w_u32(delta + 4 * index, (uint32_t)(((int32_t)(uint32_t)(r_u32(delta + 4 * index))) >> 1));
        }
        length = sub_8002E548(xport_guest_ptr(delta, 12));
        if (length < 655)
            length = 655;
        reciprocal = 0x80000000u / (length >> 1);
        (void)sub_8002EED8(reciprocal, xport_guest_ptr(delta, 12), xport_guest_ptr(normalized, 6));
        if (valid)
        {
            uint32_t ratio = (uint32_t)sub_8001FC90(scale, reciprocal);
            uint32_t angle;
            if (ratio > 0x10000)
                ratio = 0x10000;
            angle = sub_8001FF3C(ratio);
            {
                const uint16_t rotation_axis[3] = {r_u16(source + 522), r_u16(source + 522 + 2), r_u16(source + 522 + 4)};
                uint16_t rotation_matrix[9];
                uint32_t rotation_index;
                (void)sub_8003FB34(rotation_axis, side < 0 ? angle - 1024 : 1024 - angle, rotation_matrix);
                for (rotation_index = 0; rotation_index < 9; ++rotation_index)
                    w_u16(matrix + 2 * rotation_index, rotation_matrix[rotation_index]);
            }
            (void)sub_8002EFF4(xport_guest_cptr(normalized, 6), xport_guest_cptr(matrix, 18), xport_guest_ptr(rotated, 6));
            selected = rotated;
        }
        (void)sub_8002EAD8(xport_guest_ptr(actor + 456, 12), xport_guest_ptr(selected, 6), scale, xport_guest_ptr(actor + 580, 12));
    }
    return 1;
}

uint32_t sub_800BEBA4(uint16_t packed_id, uint32_t actor)
{
    int32_t scalar_sign_99;
    int32_t scalar_value_100;
    int32_t scalar_sign_100;
    int32_t scalar_sign_101;

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
        record = r_u32(0x800CD6C4) + 280 * slot;
        factor_address = r_u32(record + 8) == 1 ? 0x800CCACC : 0x800CCACE;
    }
    else
    {
        uint32_t table = 0x800CE4D0 + 16 * group;
        object = r_u32(table) + r_u32(table + 4) * slot;
        record = object + 172;
        factor_address = 0x800CCAC8 + (r_u16(record) >> 5);
    }
    factor = r_u8(factor_address);
    if (r_u8(actor + 928) & 1)
    {
        uint32_t reference = r_u32(record + 188);
        if (reference == r_u32(actor + 360) && (!(reference >> 16) || r_u32(record + 164) == r_u32(actor + 336)))
            longitudinal = ((int32_t)(uint32_t)(r_u32(record + 172) - r_u32(actor + 344)));
        else
            longitudinal = ((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(record + 12, 12), xport_guest_ptr(actor + 432, 6), xport_guest_ptr(actor + 184, 12))));
    }
    else
    {
        longitudinal = ((int32_t)((int32_t)(((int64_t)((int32_t)((int32_t)r_s16(actor + 516) * 16)) * ((int32_t)(((int32_t)(uint32_t)(r_u32(record + 12) - r_u32(actor + 504)))))) >> 16)));
        longitudinal = ((int32_t)(uint32_t)((uint32_t)longitudinal + (uint32_t)((int32_t)((int32_t)(((int64_t)((int32_t)((int32_t)r_s16(actor + 518) * 16)) * ((int32_t)(((int32_t)(uint32_t)(r_u32(record + 16) - r_u32(actor + 508)))))) >> 16)))));
        longitudinal = ((int32_t)(uint32_t)((uint32_t)longitudinal + (uint32_t)((int32_t)((int32_t)(((int64_t)((int32_t)((int32_t)r_s16(actor + 520) * 16)) * ((int32_t)(((int32_t)(uint32_t)(r_u32(record + 20) - r_u32(actor + 512)))))) >> 16)))));
    }
    if (((scalar_sign_99 = ((int32_t)(longitudinal)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_99 + ((int32_t)(longitudinal))) ^ scalar_sign_99))))) > 0x20000)
        return 0x7FFF0000;
    if ((r_u8(actor + 928) & 1) && ((int32_t)(uint32_t)(r_u32(actor + 324))) > 0x3C000)
    {
        lateral = ((int32_t)(uint32_t)((r_u32(actor + 324) - r_u32(record + 152)) << 4));
    }
    else
    {
        lateral = ((int32_t)((int32_t)(((int64_t)((int32_t)((int32_t)r_s16(actor + 528) * 16)) * ((int32_t)(((int32_t)(uint32_t)(r_u32(record + 12) - r_u32(actor + 504)))))) >> 16)));
        lateral = ((int32_t)(uint32_t)((uint32_t)lateral + (uint32_t)((int32_t)((int32_t)(((int64_t)((int32_t)((int32_t)r_s16(actor + 530) * 16)) * ((int32_t)(((int32_t)(uint32_t)(r_u32(record + 16) - r_u32(actor + 508)))))) >> 16)))));
        lateral = ((int32_t)(uint32_t)((uint32_t)lateral + (uint32_t)((int32_t)((int32_t)(((int64_t)((int32_t)((int32_t)r_s16(actor + 532) * 16)) * ((int32_t)(((int32_t)(uint32_t)(r_u32(record + 20) - r_u32(actor + 512)))))) >> 16)))));
    }
    if (lateral < -((int32_t)(uint32_t)(r_u32(record + 136))))
        return 0x7FFF0000;
    relative_speed = ((int32_t)(uint32_t)(r_u32(actor + 480)));
    if ((r_u16(record) >> 5) < 4)
    {
        int32_t dot = (int16_t)(((int32_t)r_s16(object + 444) * r_s16(actor + 528)) >> 12);
        dot += (int16_t)(((int32_t)r_s16(object + 446) * r_s16(actor + 530)) >> 12);
        dot += (int16_t)(((int32_t)r_s16(object + 448) * r_s16(actor + 532)) >> 12);
        if (((scalar_value_100 = (((int32_t)(uint32_t)((uint32_t)dot << 4)))), (scalar_sign_100 = scalar_value_100 >> 31), (((uint32_t)((uint32_t)((scalar_sign_100 + scalar_value_100) ^ scalar_sign_100))))) >= 0x4000)
        {
            if (dot < 0)
                relative_speed = ((int32_t)(uint32_t)(r_u32(actor + 480) + r_u32(object + 480)));
            else
                relative_speed = ((int32_t)(uint32_t)(r_u32(actor + 480) - r_u32(object + 480)));
        }
    }
    if (relative_speed < 524 || lateral > 0xBF0000)
        return 0x7FFF0000;
    {
        int32_t ratio = lateral <= 0 ? -(int32_t)sub_80010028((uint32_t)-lateral, (uint32_t)relative_speed) : (int32_t)sub_80010028((uint32_t)lateral, (uint32_t)relative_speed);
        uint32_t scaled_factor = factor << 12;
        uint32_t distance;
        {
            uint32_t reference = r_u32(record + 188);
            if (reference == r_u32(actor + 360) && (!(reference >> 16) || r_u32(record + 164) == r_u32(actor + 336)))
                longitudinal = ((int32_t)(uint32_t)(r_u32(record + 172) - r_u32(actor + 344)));
            else
                longitudinal = ((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(record + 12, 12), xport_guest_ptr(actor + 432, 6), xport_guest_ptr(actor + 184, 12))));
        }
        distance = ((scalar_sign_101 = ((int32_t)(longitudinal)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_101 + ((int32_t)(longitudinal))) ^ scalar_sign_101)))));
        if (distance > 87162)
        {
            uint32_t curve = (uint32_t)(((uint64_t)distance * distance) >> 16);
            int32_t weighted = ((int32_t)(uint32_t)(9 * curve));
            curve = (uint32_t)(weighted < 0 ? (weighted + 15) >> 4 : weighted >> 4);
            scaled_factor = (uint32_t)(((uint64_t)scaled_factor * curve) >> 16);
        }
        if (!(r_u16(record) >> 5) && (r_u32(r_u32(object + 852) + 552) & 0x8000))
            return 0x7FFF0000;
        return (uint32_t)(((int64_t)((int32_t)(uint32_t)(scaled_factor)) * ratio) >> 16);
    }
}

uint32_t sub_8008AE94(uint32_t output, uint32_t count_io, uint32_t position, int32_t radius, uint32_t mask, uint16_t exclude_id)
{
    int32_t scalar_value_102;
    int32_t scalar_sign_102;
    int32_t scalar_value_103;
    int32_t scalar_sign_103;

    uint32_t state = r_u32(0x8005B2F8);
    uint32_t players = r_u32(state + 48);
    int32_t x_cell = (((int32_t)(uint32_t)(r_u32(position) - r_u32(0x800CCF98))) >> 21) + 11;
    int32_t z_cell = (((int32_t)(uint32_t)(r_u32(position + 8) - r_u32(0x800CCFA0))) >> 21) + 11;
    int32_t expanded = radius + ((radius + (int32_t)((uint32_t)radius >> 31)) >> 1);
    uint32_t plane;
    int32_t x_first;
    int32_t x_last;
    int32_t z_first;
    int32_t z_last;
    uint32_t count = 0;
    uint32_t capacity = r_u32(count_io);
    FUNCTION_MARKER(0x8008AE94, "RASHCDG.BIN");
    if (players != 1 && ((uint32_t)x_cell >= 24 || (uint32_t)z_cell >= 24))
    {
        z_cell = (((int32_t)(uint32_t)(r_u32(position + 8) - r_u32(0x800CCFA4))) >> 21) + 11;
        if (z_cell < 0)
            z_cell += 24;
    }
    plane = z_cell >= 24 && z_cell < (24 << ((players - 1) & 31));
    x_first = (((int32_t)(uint32_t)(r_u32(position) - (uint32_t)expanded - r_u32(0x800CCF98 + 4 * plane))) >> 21) + 11;
    x_last = (((int32_t)(uint32_t)(r_u32(position) + (uint32_t)expanded - r_u32(0x800CCF98 + 4 * plane))) >> 21) + 11;
    z_first = (((int32_t)(uint32_t)(r_u32(position + 8) - (uint32_t)expanded - r_u32(0x800CCFA0 + 4 * plane))) >> 21) + 11;
    z_last = (((int32_t)(uint32_t)(r_u32(position + 8) + (uint32_t)expanded - r_u32(0x800CCFA0 + 4 * plane))) >> 21) + 11;
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
        w_u32(count_io, 0);
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
                            record = r_u32(0x8005B3A0) + 1096 * packed;
                            id_address = record + 172;
                            x_address = record + 184;
                            z_address = record + 192;
                        }
                        else if (group == 1)
                        {
                            record = r_u32(0x8005B3A4) + 628 * (packed & 31);
                            id_address = record + 172;
                            x_address = record + 184;
                            z_address = record + 192;
                        }
                        else
                        {
                            uint32_t table = 0x800CE4D0 + 16 * group;
                            record = r_u32(table) + r_u32(table + 4) * (packed & 31) + r_u32(0x800CCA68 + 4 * (group - 2));
                            id_address = record;
                            x_address = record + 12;
                            z_address = record + 20;
                        }
                        id = r_u16(id_address);
                        if (id != exclude_id)
                        {
                            uint32_t dx = ((scalar_value_102 = (((int32_t)(uint32_t)(r_u32(x_address) - r_u32(position))))), (scalar_sign_102 = scalar_value_102 >> 31), (((uint32_t)((uint32_t)((scalar_sign_102 + scalar_value_102) ^ scalar_sign_102)))));
                            uint32_t dz = ((scalar_value_103 = (((int32_t)(uint32_t)(r_u32(z_address) - r_u32(position + 8))))), (scalar_sign_103 = scalar_value_103 >> 31), (((uint32_t)((uint32_t)((scalar_sign_103 + scalar_value_103) ^ scalar_sign_103)))));
                            uint32_t minimum = dx < dz ? dx : dz;
                            uint32_t metric = dx + dz - ((minimum + (minimum >> 31)) >> 1);
                            uint32_t duplicate = 0;
                            uint32_t index;
                            if ((uint32_t)radius >= metric)
                            {
                                for (index = 0; index < count; ++index)
                                {
                                    if (r_u16(output + 2 * index) == id)
                                    {
                                        duplicate = 1;
                                        break;
                                    }
                                }
                                if (!duplicate)
                                {
                                    w_u16(output + 2 * count, id);
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
    w_u32(count_io, count);
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
    result = r_u32(actor + 568) & 0x600;
    if (result)
        return result;
    other = r_u32(0x8005B3A0) + 1096 * other_id;
    if ((r_u8(r_u32(actor + 1084) + 1) & 0x0F) == 2)
        delta = ((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(other + 504, 12), xport_guest_ptr(actor + 528, 6), xport_guest_ptr(actor + 504, 12))));
    else
        delta = ((int32_t)(uint32_t)((r_u32(actor + 324) - r_u32(other + 324)) << 4));
    if (!sub_800BB8FC(actor, other, delta))
    {
        sub_800BC8DC(actor);
        return 0;
    }
    if (delta < ((int32_t)(uint32_t)(3u * (r_u32(actor + 480) - r_u32(other + 480)))))
    {
        int32_t other_value = ((int32_t)(uint32_t)(r_u32(other + 344)));
        int32_t actor_value = ((int32_t)(uint32_t)(r_u32(actor + 344)));
        int32_t relative = ((int32_t)(uint32_t)((uint32_t)other_value + (uint32_t)actor_value));
        int32_t limit;
        int32_t response;
        uint32_t kind;
        if (((int32_t)(uint32_t)(r_u32(other + 364) ^ r_u32(actor + 364))) >= 0)
            relative = other_value - actor_value;
        kind = r_u16(actor + 956 + 8 * ((int32_t)r_s8(actor + 946) - 2));
        limit = kind - 14 < 2 ? 0x20000 : ((int32_t)(uint32_t)(r_u32(actor + 304) + r_u32(other + 304)));
        if (limit >= relative && (((int32_t)(uint32_t)(r_u32(0x80052F64))) >= delta || -limit >= relative || ((uint32_t)(int32_t)r_s16(r_u32(actor + 340) + 36) >> 31) == (uint32_t)(((int32_t)(uint32_t)(r_u32(actor + 364))) > 0)))
            response = ((int32_t)(uint32_t)(r_u32(0x80052F58)));
        else
            response = -((int32_t)(uint32_t)(r_u32(0x80052F58)));
        if (sub_800BC618(other, r_u16(actor + 172), response))
        {
            if (kind - 14 < 3)
                response = -response;
            else
                response = ((int32_t)(uint32_t)((uint32_t)response << 1));
        }
        sub_800BC4FC(actor, other, response);
    }
    else if (r_s16(actor + 944) > 0)
    {
        int32_t magnitude = ((int32_t)(uint32_t)(sub_8002E548(xport_guest_ptr(actor + 892, 12))));
        if (magnitude < 16)
        {
            w_u32(actor + 908, 0);
            w_u16(actor + 944, 0);
        }
        else
        {
            uint32_t ratio = sub_80010028(163840, (uint32_t)magnitude);
            uint32_t inverse = sub_80010028((uint32_t)magnitude, 163840);
            w_u32(actor + 904, ratio);
            w_u16(actor + 944, (uint16_t)(((int32_t)(uint32_t)(0u - ((inverse << 8) + 0x8000u))) >> 16));
        }
    }
    if (r_u16(actor + 320) & 2)
    {
        uint32_t flags = r_u32(actor + 564);
        uint32_t state = r_u32(0x8005B2F8);
        if (r_u16(other + 172) < r_u32(state + 48))
            flags |= 0x200;
        w_u32(actor + 564, flags);
        w_u32(actor + 924, r_u32(r_u32(actor + 556) + 224));
    }
    result = r_u32(actor + 564) & 0x80000;
    if (!result)
    {
        uint32_t value = r_u32(actor + 924);
        result = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(value)), ((int32_t)(uint32_t)(value)));
        w_u32(actor + 916, (uint32_t)delta);
        w_u32(actor + 920, result);
        w_u32(actor + 564, r_u32(actor + 564) & 0xFFF7FFFFu);
    }
    return result;
}

uint32_t sub_8002E604(const uint32_t left[3], const uint32_t right[3])
{
    uint32_t result = 0;
    uint32_t axis;

    FUNCTION_MARKER(0x8002E604, "SLUS_010.53");
    for (axis = 0; axis < 3; ++axis)
        result += (uint32_t)((int64_t)(int32_t)xport_load_le32(left + axis) * (int32_t)xport_load_le32(right + axis) >> 16);
    return result;
}

uint32_t sub_8002EED8(uint32_t scale, const uint32_t vector[3], uint16_t output[3])
{
    uint32_t result = 0;
    uint32_t axis;
    FUNCTION_MARKER(0x8002EED8, "SLUS_010.53");
    for (axis = 0; axis < 3; ++axis)
    {
        uint32_t component = xport_load_le32((const uint8_t *)vector + 4u * axis);
        uint32_t product = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(scale)), ((int32_t)(uint32_t)(component)));
        result = (uint32_t)(((int32_t)(uint32_t)(product)) >> 4);
        xport_store_le16((uint8_t *)output + 2u * axis, (uint16_t)result);
    }
    return result;
}

uint32_t sub_8002EFF4(const uint16_t vector[3], const uint16_t matrix[9], uint16_t output[3])
{
    uint32_t column, result;
    int32_t first, second, third;
    FUNCTION_MARKER(0x8002EFF4, "SLUS_010.53");
    for (column = 0; column < 3; ++column)
    {
        first = (int32_t)(int16_t)vector[0] * (int32_t)(int16_t)matrix[column];
        second = (int32_t)(int16_t)vector[1] * (int32_t)(int16_t)matrix[column + 3];
        third = (int32_t)(int16_t)vector[2] * (int32_t)(int16_t)matrix[column + 6];
        result = (uint32_t)(first >> 12) + (uint32_t)(second >> 12) + (uint32_t)(third >> 12);
        output[column] = (uint16_t)result;
    }
    return result;
}

uint32_t sub_800CA05C(uint32_t mode)
{
    uint32_t primary = r_u32(0x8005B38C);
    uint32_t state = r_u32(0x8005B2F8);
    uint32_t event = 0x1F8003A0;
    uint32_t first = 0x1F8003B0;
    uint32_t second = 0x1F8003C0;
    uint32_t object;
    uint32_t affected = 0;
    int32_t index;
    FUNCTION_MARKER(0x800CA05C, "RASHCDG.BIN");
    w_u16(event, 18);
    if (mode)
    {
        uint32_t descriptor = r_u32(primary + 1084);
        w_u8(descriptor + 39, 0xFE);
        w_u32(descriptor + 40, r_u32(state + 16));
    }
    else
    {
        uint32_t linked = r_u32(primary + 856);
        sub_800BC8DC(linked);
        w_u16(event + 2, r_u16(primary + 172));
        (void)sub_800BCA68(xport_guest_ptr(event, 8), 0, linked);
        xport_update_u8(r_u32(primary + 852) + 572, XPORT_MEMORY_UPDATE_OR, 0x10);
    }
    index = ((int32_t)(uint32_t)(r_u32(r_u32(0x800CE4DC))));
    object = r_u32(0x800CE4D0);
    while (index >= 0)
    {
        uint32_t descriptor;
        uint32_t secondary;
        uint32_t type;
        if (!r_u16(object + 320))
            goto next_object;
        descriptor = r_u32(object + 1084);
        type = r_u8(descriptor + 1) & 0x0F;
        if (type == 2 || type == (uint32_t)(r_u8(r_u32(primary + 1084) + 1) & 0x0F))
            goto next_object;
        secondary = r_u32(object + 852);
        if (r_u16(secondary + 544) != 64)
            goto next_object;
        if (mode)
        {
            uint32_t primary_secondary = r_u32(primary + 852);
            int32_t length_first;
            int32_t length_second;
            int32_t dot;
            int32_t scale;
            int32_t ratio;
            w_u32(first, r_u32(primary_secondary + 184) - r_u32(secondary + 184));
            w_u32(first + 4, r_u32(primary_secondary + 188) - r_u32(secondary + 188));
            w_u32(first + 8, r_u32(primary_secondary + 192) - r_u32(secondary + 192));
            length_first = ((int32_t)(uint32_t)(sub_8002E548(xport_guest_ptr(first, 12))));
            if (length_first <= 0x3FFFF)
            {
                w_u32(0x8005B228, 0x50000);
                goto next_object;
            }
            w_u32(second, r_u32(object + 184) - r_u32(secondary + 184));
            w_u32(second + 4, r_u32(object + 188) - r_u32(secondary + 188));
            w_u32(second + 8, r_u32(object + 192) - r_u32(secondary + 192));
            length_second = ((int32_t)(uint32_t)(sub_8002E548(xport_guest_ptr(second, 12))));
            scale = (int32_t)(((int64_t)length_first * length_second) >> 16);
            dot = ((int32_t)(uint32_t)(sub_8002E604(xport_guest_ptr(first, 12), xport_guest_ptr(second, 12))));
            ratio = (int32_t)sub_80010028(dot > 0 ? (uint32_t)dot : (uint32_t)-dot, scale > 0 ? (uint32_t)scale : (uint32_t)-scale);
            if ((dot > 0 && scale <= 0) || (dot <= 0 && scale > 0))
                ratio = -ratio;
            if (ratio <= 52427)
                goto next_object;
            ++affected;
        }
        sub_800BC8DC(object);
        w_u16(event + 2, r_u16(object + 172));
        (void)sub_800BCA68(xport_guest_ptr(event, 8), 0, object);
        if (!mode)
        {
            uint32_t blend = sub_8001FC58() & 0x1F;
            int32_t value;
            if (blend >= 17)
                blend -= 16;
            value = ((int32_t)(uint32_t)((16 - blend) * r_u32(0x8005ADE8) + blend * r_u32(0x8005ADEC)));
            value = value < 0 ? (value + 15) >> 4 : value >> 4;
            w_u16(object + 968, (uint16_t)((79u * (uint32_t)value) >> 14));
        }
    next_object:
        --index;
        object += r_u32(0x800CE4D4);
    }
    if (affected)
        w_u32(0x8005B228, r_u32(0x8005ADE8) + 0xA0000);
    return r_u32(0x8005B228);
}

uint32_t sub_800C9E74(void)
{
    uint32_t actor = r_u32(0x8005B38C);
    uint32_t state = r_u32(0x8005B2F8);
    uint32_t piece;
    uint32_t result;
    int32_t distance;
    FUNCTION_MARKER(0x800C9E74, "RASHCDG.BIN");
    distance = ((int32_t)(uint32_t)((uint32_t)(int32_t)r_s8(0x8005ADF2) << 16));
    distance = ((int32_t)(uint32_t)((uint32_t)distance - r_u32(actor + 0x170)));
    distance = ((int32_t)(uint32_t)((uint32_t)distance + ((uint32_t)r_u16(0x80053174 + 4 * (r_u8(state + 57) - 1) + 2) << 16)));
    if (distance > 0xA0000)
    {
        int32_t product;
        piece = r_u32(actor + 0x154);
        product = (int32_t)r_u8(r_u32(actor + 0x43C) + 0x45) * ((int32_t)(uint32_t)(r_u32(r_u32(actor + 0x22C) + 0xE0)));
        if (product < 0)
            product += 0x7F;
        w_u32(actor + 0x39C, (uint32_t)(product >> 7));
        result = sub_8002EAD8(xport_guest_ptr(piece + 20, 12), xport_guest_ptr(piece + 14, 6), distance > 0x280000 ? 0x280000 : (uint32_t)distance, xport_guest_ptr(actor + 0x370, 12));
        if (distance <= 0x280000)
        {
            actor = r_u32(0x8005B38C);
            result = sub_8002EAD8(xport_guest_ptr(actor + 0x370, 12), xport_guest_ptr(r_u32(actor + 0x154) + 2, 6), (uint32_t)(int32_t)r_s8(0x8005ADF3) << 16, xport_guest_ptr(actor + 0x370, 12));
        }
        return result;
    }
    w_u32(actor + 0x39C, 0);
    result = sub_8002EAD8(xport_guest_ptr(actor + 0x1F8, 12), xport_guest_ptr(actor + 0x1C2, 6), r_u32(actor + 0x134) << 3, xport_guest_ptr(actor + 0x370, 12));
    w_u32(actor + 0x230, r_u32(actor + 0x230) | 0x40000);
    if (!(r_u32(actor + 0x234) & 0x80000))
    {
        w_u32(actor + 0x394, 0);
        w_u32(actor + 0x398, 0);
        w_u32(actor + 0x234, r_u32(actor + 0x234) & 0xFFF7FFFF);
    }
    if (r_u8(state + 57) == 2 && ((int32_t)(uint32_t)(r_u32(actor + 0x1E0))) <= 0x7FFF)
    {
        uint32_t linked = r_u32(actor + 0x358);
        int32_t index = (int32_t)r_s8(linked + 0x3B2) - 1;
        if (!r_u16(linked + 8 * (uint32_t)index + 0x3BC))
            return sub_800CA05C(0);
    }
    return result;
}

void sub_800BADB8(uint32_t actor, uint32_t contact)
{
    int32_t scalar_value_104;
    int32_t scalar_sign_104;

    uint32_t state = r_u32(0x8005B2F8);
    uint32_t actor_id = r_u16(actor + 172);
    uint32_t actor_type = r_u8(r_u32(actor + 1084) + 1) & 0x0F;
    uint32_t primary = r_u32(0x8005B38C);
    uint32_t retained = 0;
    FUNCTION_MARKER(0x800BADB8, "RASHCDG.BIN");
    if (actor_id < r_u32(state + 48) && (uint32_t)(r_u8(state + 57) - 1) < 2)
    {
        (void)sub_800C9E74();
    }
    else if (((int32_t)(uint32_t)(r_u32(actor + 480))) > 0x8000)
    {
        uint32_t primary_type = r_u8(r_u32(primary + 1084) + 1) & 0x0F;
        if (r_u8(state + 57) >= 4 && actor_type != primary_type)
        {
            int32_t relative;
            int32_t magnitude;
            uint32_t value = r_u32(actor + 576) - 0xA0000;
            w_u32(actor + 924, ((int32_t)(uint32_t)(value)) < 0 ? 0 : value);
            relative = ((int32_t)(uint32_t)(r_u32(actor + 344) + r_u32(primary + 344)));
            if (((int32_t)(uint32_t)(r_u32(actor + 364) ^ r_u32(primary + 364))) >= 0)
                relative = ((int32_t)(uint32_t)(r_u32(actor + 344) - r_u32(primary + 344)));
            if (r_u32(actor + 372) && !r_u16(actor + 362))
            {
                int16_t factor = r_s16(actor + (relative < 0 ? 408 : 420));
                magnitude = ((int32_t)(uint32_t)(r_u32(actor + 424))) * factor;
            }
            else
            {
                magnitude = 0x39999;
            }
            if (relative < 0)
                magnitude = -magnitude;
            magnitude = ((int32_t)(uint32_t)((uint32_t)magnitude - ((uint32_t)(int32_t)r_s16(actor + 878) << 5)));
            leaf_80093CAC_scalar(actor, actor + 872, magnitude, 1);
            return;
        }
        w_u32(actor + 924, 0);
        if (actor_id < r_u32(state + 48) || actor_type != 2 || r_u16(contact + 2) == 224)
        {
            uint32_t scale = (uint32_t)sub_8001FC90(0x80000, r_u32(actor + 308));
            (void)sub_8002EAD8(xport_guest_ptr(actor + 504, 12), xport_guest_ptr(actor + 450, 6), scale, xport_guest_ptr(actor + 880, 12));
            w_u32(actor + 560, r_u32(actor + 560) | 0x40000);
        }
        else
        {
            uint32_t other = r_u32(0x8005B3A0) + 1096 * r_u16(contact + 2);
            retained = ((scalar_value_104 = (((int32_t)(uint32_t)(sub_8009E444(actor, other))))), (scalar_sign_104 = scalar_value_104 >> 31), (((uint32_t)((uint32_t)((scalar_sign_104 + scalar_value_104) ^ scalar_sign_104)))));
            if (retained <= 0xFFFF)
                retained = 0x10000;
            if (r_u32(other + 372))
            {
                uint32_t piece = r_u32(other + 340);
                if (r_u32(other + 388) & 1)
                    (void)sub_8002EAD8(xport_guest_ptr(piece + 20, 12), xport_guest_ptr(piece + 14, 6), r_u32(other + 348), xport_guest_ptr(actor + 880, 12));
                else
                {
                    uint32_t direction = ((int32_t)(uint32_t)(r_u32(other + 364))) < 0 ? 0xFFFF0000 : 0x10000;
                    uint32_t scale = (uint32_t)sub_8001FC90(direction, r_u32(actor + 308));
                    (void)sub_8002EAD8(xport_guest_ptr(actor + 184, 12), xport_guest_ptr(piece + 14, 6), scale, xport_guest_ptr(actor + 880, 12));
                }
            }
        }
        if (r_u32(actor + 564) & 0x200)
            w_u32(actor + 564, r_u32(actor + 564) & 0xFFFFFDFF);
        if (!(r_u32(actor + 564) & 0x80000))
        {
            w_u32(actor + 916, retained);
            w_u32(actor + 920, 0);
            w_u32(actor + 564, r_u32(actor + 564) & 0xFFF7FFFF);
        }
    }
    {
        uint32_t secondary = r_u32(actor + 852);
        uint32_t secondary_id = r_u16(secondary + 544);
        if ((r_u16(contact + 6) & 0x8000) && r_u32(secondary + 604) == 1 && (secondary_id < 26 || secondary_id >= 38) && r_u16(0x800541D4 + 8 * secondary_id + 2) != 3)
        {
            uint32_t descriptor = r_u32(actor + 1084);
            uint32_t event = 9;
            if (actor_id < r_u32(state + 48) && r_u32(descriptor + 40) && r_u8(descriptor + 39) < 4 && !r_u8(state + 57))
                event = 1;
            (void)sub_800C4550(event, secondary, 2);
        }
    }
    if (actor_id < r_u32(state + 48))
    {
        uint32_t player = 0x800CD898 + 1132 * actor_id;
        if (!(r_u32(player + 552) & 0x44) && r_u8(state + 4) != 44)
            (void)sub_8008A998(player, 10);
    }
}

uint32_t sub_800BFF04(uint32_t first, uint32_t second, uint32_t flags, uint32_t *flags_out)
{
    uint32_t state = r_u32(0x8005B2F8);
    uint32_t first_secondary = r_u32(first + 852);
    uint32_t second_secondary = r_u32(second + 852);
    uint32_t first_id = r_u16(first + 172);
    uint32_t second_id = r_u16(second + 172);
    uint32_t player_count = r_u32(state + 48);
    uint32_t special;
    uint32_t result = 0;
    FUNCTION_MARKER(0x800BFF04, "RASHCDG.BIN");
    special = first_id < player_count || second_id < player_count || (r_u8(first_secondary + 572) & 0x20) || (r_u8(second_secondary + 572) & 0x20);
    if (!special)
        return 0;
    if (r_u16(0x800541D4 + 8 * r_u16(second_secondary + 544) + 2) != 3)
        return 3;
    {
        uint32_t second_descriptor = r_u32(second + 1084);
        uint32_t first_descriptor = r_u32(first + 1084);
        uint32_t animation;
        uint32_t entry;
        int32_t frame;
        uint32_t limit;
        if (!(r_u8(second_descriptor + 60) & 0x80))
            return 0;
        result = sub_800C2E9C(first_secondary, r_u8(second_descriptor + 46));
        if (!result)
            return result;
        animation = r_u32(second_secondary + 540);
        entry = r_u32(r_u32(r_u32(animation + 40) + 4) + 4 * r_u8(r_u32(animation + 4) + 12 * r_u32(animation + 12)));
        frame = (int16_t)(r_u16(entry + 16) - 1);
        limit = r_u32(animation + 16);
        if (3 * frame < ((int32_t)(uint32_t)(4 * limit)))
            xport_store_le32(flags_out, xport_load_le32(flags_out) | 0x40);
        if (xport_load_le32(flags_out) & 0x40)
            return sub_80017BA0(r_u32(first + 184), r_u32(first + 192), 88, 0);
        result = xport_load_le32(flags_out) | 0x80;
        if (frame >= ((int32_t)(uint32_t)(2 * limit)))
            return result;
        xport_store_le32(flags_out, result);
        w_u8(first_descriptor + 46, r_u8(second_descriptor + 46));
        w_u16(first_descriptor + 44, r_u16(first_descriptor + 44) | (uint16_t)(1u << (r_u8(second_descriptor + 46) & 31)));
        w_u16(second_descriptor + 44, r_u16(second_descriptor + 44) & (uint16_t)~(1u << (r_u8(second_descriptor + 46) & 31)));
        w_u8(second_descriptor + 46, 9);
        w_u8(second_descriptor + 70, (uint8_t)first_id);
        w_u8(first_descriptor + 71, (uint8_t)second_id);
        (void)sub_800BFE58(first, 0, 0);
        (void)sub_800BFE58(second, 0, 1);
        {
            uint32_t row = r_u8(second_secondary + 569);
            uint32_t column = r_u8(second_secondary + 570);
            int32_t variant = (int32_t)r_s8(second_secondary + 574 + column);
            uint32_t table = r_u32(0x8005AD4C) + 12 * row;
            uint32_t sound = r_u16(r_u32(table + 8) + 12 * (uint32_t)variant);
            uint32_t transfer = r_u8(second_secondary + 572) >> 7;
            uint32_t effect = (r_u8(first_descriptor + 1) & 0x0F) ? 0x61 : 0x60;
            (void)sub_80017BA0(r_u32(first + 184), r_u32(first + 192), effect, 0);
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
                    value += (r_u32(first_descriptor + 48) >> (4 * lane)) & 0x0F;
                packed = r_u32(first_descriptor + 48) & ~(0x0Fu << ((4 * lane) & 31));
                w_u32(first_descriptor + 48, packed | ((value & 0x0F) << ((4 * lane) & 31)));
                w_u8(first_descriptor + 47, (uint8_t)value);
                w_u32(second_descriptor + 48, r_u32(second_descriptor + 48) & ~(0x0Fu << ((4 * lane) & 31)));
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
    int32_t scalar_value_105;
    int32_t scalar_sign_105;
    int32_t scalar_value_106;
    int32_t scalar_sign_106;
    int32_t scalar_value_107;
    int32_t scalar_sign_107;
    int32_t scalar_value_108;
    int32_t scalar_sign_108;
    int32_t scalar_sign_109;

    uint32_t speed;
    uint32_t measured;
    uint32_t stopped;
    uint32_t result;
    FUNCTION_MARKER(0x8009989C, "RASHCDG.BIN");
    if (fixed_result)
    {
        while (((scalar_value_105 = (((int32_t)(uint32_t)(r_u32(vector))))), (scalar_sign_105 = scalar_value_105 >> 31), (((uint32_t)((uint32_t)((scalar_sign_105 + scalar_value_105) ^ scalar_sign_105))))) > 0x5A8000 || ((scalar_value_106 = (((int32_t)(uint32_t)(r_u32(vector + 4))))), (scalar_sign_106 = scalar_value_106 >> 31), (((uint32_t)((uint32_t)((scalar_sign_106 + scalar_value_106) ^ scalar_sign_106))))) > 0x5A8000 || ((scalar_value_107 = (((int32_t)(uint32_t)(r_u32(vector + 8))))), (scalar_sign_107 = scalar_value_107 >> 31), (((uint32_t)((uint32_t)((scalar_sign_107 + scalar_value_107) ^ scalar_sign_107))))) > 0x5A8000)
        {
            w_u32(vector, (uint32_t)(((int32_t)(uint32_t)(r_u32(vector))) >> 1));
            w_u32(vector + 4, (uint32_t)(((int32_t)(uint32_t)(r_u32(vector + 4))) >> 1));
            w_u32(vector + 8, (uint32_t)(((int32_t)(uint32_t)(r_u32(vector + 8))) >> 1));
        }
        measured = sub_8002E548(xport_guest_ptr(vector, 12));
        speed = 0x7FFF0000;
    }
    else
    {
        speed = sub_8004CF74(magnitude) << 2;
        measured = speed;
    }
    w_u32(actor + 484, speed);
    stopped = speed < 655 || (r_u32(actor + 480) && speed <= 0x7FFF);
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
        (void)sub_8002EED8(reciprocal, xport_guest_ptr(vector, 12), xport_guest_ptr(normalized, 6));
        for (index = 0; index < 3; ++index)
        {
            diagonal[index] = r_s16(actor + 522 + 2 * index);
            direction[index] = r_s16(normalized + 2 * index);
        }
        leaf_direction_op_local(diagonal, direction, first_cross);
        for (index = 0; index < 3; ++index)
            w_u16(first_cross_address + 2 * index, (uint16_t)first_cross[index]);
        if (!sub_8002E468(xport_guest_ptr(first_cross_address, 6)))
        {
            for (index = 0; index < 3; ++index)
                w_u16(first_cross_address + 2 * index, r_u16(actor + 528 + 2 * index));
        }
        for (index = 0; index < 3; ++index)
        {
            first_cross[index] = r_s16(first_cross_address + 2 * index);
            direction[index] = r_s16(actor + 522 + 2 * index);
        }
        leaf_direction_op_local(first_cross, direction, second_cross);
        for (index = 0; index < 3; ++index)
            w_u16(second_cross_address + 2 * index, (uint16_t)second_cross[index]);
        angle = sub_80093BE0(second_cross_address, actor + 528, actor + 516);
        scaled = ((scalar_value_108 = ((int32_t)sub_8001FC90(r_u32(actor + 488), threshold))), (scalar_sign_108 = scalar_value_108 >> 31), (((uint32_t)((uint32_t)((scalar_sign_108 + scalar_value_108) ^ scalar_sign_108)))));
        if (scaled < 3431)
            scaled = 3431;
        if ((uint32_t)(((uint64_t)25736 * ((scalar_sign_109 = ((int32_t)(angle)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_109 + ((int32_t)(angle))) ^ scalar_sign_109)))))) >> 8) >= scaled)
        {
            result = r_u32(actor + 552) | (angle >= 0 ? 0x500 : 0x300);
            w_u32(actor + 552, result);
            return result;
        }
        for (index = 0; index < 3; ++index)
        {
            w_u16(actor + 528 + 2 * index, r_u16(second_cross_address + 2 * index));
            w_u16(actor + 516 + 2 * index, r_u16(first_cross_address + 2 * index));
        }
    }
    w_u32(actor + 488, 0);
    if (speed < 0x3333)
    {
        stopped = 1;
    }
    else
    {
        int64_t dot = 0;
        uint32_t index;
        for (index = 0; index < 3; ++index)
            dot += (int64_t)((int32_t)(uint32_t)(r_u32(vector + 4 * index))) * ((int32_t)r_s16(actor + 528 + 2 * index) << 4);
        stopped = dot < 0;
    }
    if (stopped)
    {
        uint32_t index;
        for (index = 0; index < 3; ++index)
        {
            w_u32(actor + 184 + 4 * index, r_u32(actor + 580 + 4 * index));
            w_u16(actor + 450 + 2 * index, r_u16(actor + 528 + 2 * index));
        }
        w_u32(actor + 552, r_u32(actor + 552) | 8);
        w_u32(linked + 576, 0);
        w_u32(linked + 480, 0);
    }
    result = (r_u32(actor + 552) & 0xFFFFFE7F) | 0x100;
    w_u32(actor + 552, result);
    return result;
}

uint32_t sub_80096820(uint32_t state, uint32_t actor, uint32_t delta)
{
    uint32_t descriptor = r_u32(actor + 1084);
    uint32_t type = r_u8(descriptor + 1) & 0x0F;
    uint32_t race = r_u32(0x8005B2F8);
    uint32_t course = r_u32(race + 60);
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
            w_u32(0x8005B2EC, 0);
            (void)(uint32_t)sub_8001B244(4);
            if (type != 2)
            {
                table = r_u32(0x8005B2B0) ? 0x80053054 : 0x8005303C;
                w_u32(actor + 924, r_u32(table + 4 * course));
                return 0;
            }
            w_u8(descriptor + 39, (uint8_t)sub_800138E8(actor, 0));
            remaining = ((int32_t)(uint32_t)(r_u32(r_u32(0x800CE4DC))));
            object = r_u32(0x800CE4D0);
            for (index = remaining; index >= 0; --index, object += r_u32(0x800CE4D4))
            {
                if (r_s16(object + 320))
                {
                    uint32_t secondary = r_u32(object + 852);
                    if ((uint32_t)(r_u32(secondary + 604) - 3) < 2)
                        ++active;
                }
            }
            remaining = ((int32_t)(uint32_t)(r_u32(0x8005B1F8))) - (int32_t)(active + 1) - ((int32_t)(uint32_t)(r_u32(0x80053168 + 4 * course)));
            if (r_u8(descriptor + 39) < (uint32_t)remaining)
            {
                table = r_u32(0x8005B2B0) ? 0x80053048 : 0x80053030;
                w_u32(actor + 924, r_u32(table + 4 * course));
                return 0;
            }
            if (r_u32(r_u32(actor + 852) + 604) < 3 && !(r_u32(actor + 36) & 0x08000000))
            {
                (void)sub_80028034(actor);
                (void)sub_80020E30(0);
            }
            {
                uint32_t contact = sub_8003B4B0(r_u32(actor + 428), r_u16(actor + 360));
                if (contact)
                {
                    uint32_t vector = 0x1F800340;
                    int16_t x = r_s16(r_u32(actor + 340) + 14);
                    int16_t y = r_s16(r_u32(actor + 340) + 16);
                    int16_t z = r_s16(r_u32(actor + 340) + 18);
                    int32_t first;
                    int32_t second;
                    int32_t third;
                    int32_t combined;
                    if (((int32_t)(uint32_t)(r_u32(contact + 4))) < 0)
                    {
                        x = (int16_t)-x;
                        z = (int16_t)-z;
                    }
                    w_u16(vector, (uint16_t)x);
                    w_u16(vector + 2, (uint16_t)y);
                    w_u16(vector + 4, (uint16_t)z);
                    first = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(actor + 444, 6), xport_guest_ptr(vector, 6))));
                    second = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(actor + 444, 6), xport_guest_ptr(vector, 6))));
                    third = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(actor + 444, 6), xport_guest_ptr(vector, 6))));
                    combined = ((int32_t)(uint32_t)((uint32_t)((first >> 31) + second) ^ (uint32_t)(third >> 31)));
                    if (combined <= 62258)
                    {
                        table = r_u32(0x8005B2B0) ? 0x80053048 : 0x80053030;
                        w_u32(actor + 924, r_u32(table + 4 * course));
                        if (!r_u32(0x8005B220))
                            return 0;
                    }
                }
            }
            w_u32(0x8005ACD0, r_u32(race + 16));
            break;
        }
        case 2:
            if (type == 2)
            {
                uint32_t record = 0x800D6198 + 224 * r_u16(actor + 172);
                w_u32(record + 116, r_u32(race + 16));
                w_u32(0x8005B2EC, 0);
                w_u32(record + 108, 0);
                if (r_u16(actor + 948 + 8 * (uint32_t)(int32_t)r_s8(actor + 946)) >= 3)
                {
                    w_u32(0x8005AD48, (r_u32(0x8005AD48) & 0xFFFFFFF9u) | 4);
                    return 0;
                }
            }
            break;
        case 4:
            if (type == 2)
            {
                uint32_t timer = r_u32(0x8005B2EC) + delta;
                uint32_t position = (uint8_t)(r_u8(descriptor + 39) + 4);
                uint32_t near_front = position < 2;
                w_u32(0x8005B2EC, timer);
                if (timer >= 20001 && position >= 2 && r_u32(actor + 180) >= 0x12)
                    (void)(uint32_t)sub_8001B244(1);
                if (timer < (r_u32(0x800530F0 + 4 * course) >> 1))
                    return 0;
                w_u32(0x8005AD48, r_u32(0x8005AD48) & 0xFFFFFFFBu);
                if (near_front)
                {
                    uint32_t mode = r_u8(descriptor + 39) == 252 ? 0 : 2;
                    (void)(uint32_t)sub_8001B244(mode);
                    w_u32(0x8005AD48, r_u32(0x8005AD48) | 0x10);
                }
                else
                {
                    uint32_t record = 0x800D6198 + 224 * r_u16(actor + 172);
                    w_u32(0x8005AD48, r_u32(0x8005AD48) | 8);
                    w_u32(0x8005B2EC, 0);
                    w_u32(record + 116, r_u32(race + 16));
                }
                return 0;
            }
            break;
        case 8:
            if (type == 2)
            {
                uint32_t timer = r_u32(0x8005B2EC) + delta;
                w_u32(0x8005B2EC, timer);
                if (timer > (r_u32(0x800530F0 + 4 * course) >> 1))
                {
                    w_u32(0x8005B2EC, 0);
                    w_u32(0x8005AD48, r_u32(0x8005AD48) & 0xFFFFFFF7u);
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
        w_u32(0x8005AD48, r_u32(0x8005AD48) & 0xFFFFFFF8u);
    result = 1;
    if (!r_u32(0x8005B220) && !r_u32(descriptor + 40))
    {
        uint32_t record = 0x800CD898 + 1132 * r_u16(actor + 172);
        if (!r_u32(record + 772) && !(r_u32(record + 548) & 0x2008))
        {
            uint32_t contact = actor + 948 + 8 * (uint32_t)(int32_t)r_s8(actor + 946);
            uint32_t blocked = r_u16(contact) == 3 && ((r_u16(contact + 2) >> 5) == 3);
            if (((int32_t)(uint32_t)(r_u32(actor + 480))) <= 439457 || (!blocked && !r_u16(actor + 362)))
            {
                w_u32(actor + 560, r_u32(actor + 560) & 0xD7FFFFFFu);
                return 1;
            }
        }
    }
    return result;
}

uint32_t sub_80096818(uint32_t actor, uint32_t delta)
{
    FUNCTION_MARKER(0x80096818, "RASHCDG.BIN");
    return sub_80096820(r_u32(0x8005AD48), actor, delta);
}

uint32_t sub_8009AD50(uint32_t enabled, uint32_t specification, uint32_t actor)
{
    int32_t scalar_sign_110;
    int32_t scalar_sign_111;
    int32_t scalar_sign_112;

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
    w_u32(config, r_u32(specification + 8));
    w_u32(config + 8, r_u32(specification + 36));
    side = (int16_t)r_u16(specification + 60);
    w_u32(config + 4, (uint32_t)side);
    if ((r_u32(config) >> 16) == 1)
        return 0;
    if (!side)
    {
        side = (int32_t)(sub_8001FC58() & 1);
        if (!side)
            side = -1;
        w_u32(config + 4, (uint32_t)side);
    }
    segment = sub_80039DFC(0, ((config) ? (const uint32_t[3]){r_u32(config), 0, (uint32_t)r_u16((config) + 10) << 16} : NULL));
    if (!segment)
        return 0;
    current = r_u32(0x800CF654);
    if (((int32_t)(uint32_t)(current)) >= 16)
        return 0;
    next = current + 1;
    while (((int32_t)(uint32_t)(next)) < 16)
    {
        uint32_t candidate = 0x800CF660 + 512 * next;
        if (!r_u16(candidate + 172) && r_u32(candidate + 4))
            break;
        ++next;
    }
    object = 0x800CF660 + 512 * current;
    w_u32(0x800CF654, next);
    w_u16(object + 172, (uint16_t)(current + 96));
    w_u32(0x800CF650, r_u32(0x800CF650) + 1);
    if (((int32_t)(uint32_t)(r_u32(0x800CF658))) < ((int32_t)(uint32_t)(current)))
        w_u32(0x800CF658, current);
    w_u32(object + 180, r_u16(specification + 2));
    w_u16(object + 322, r_u16(specification + 62));
    uint32_t geometry_result = 0;
    {
        const uint32_t route_position[3] = {r_u32(config), r_u32(config + 4), r_u32(config + 8)};
        uint32_t route_geometry[8] = {r_u32(geometry), r_u32(geometry + 4), r_u32(geometry + 8), r_u32(geometry + 12), r_u32(geometry + 16), r_u32(geometry + 20), r_u32(geometry + 24), r_u32(geometry + 28)};
        geometry_result = sub_8003A700(segment, route_position, route_geometry);
        w_u32(geometry, route_geometry[0]);
        w_u32(geometry + 4, route_geometry[1]);
        w_u32(geometry + 8, route_geometry[2]);
        w_u32(geometry + 12, route_geometry[3]);
        w_u32(geometry + 16, route_geometry[4]);
        w_u32(geometry + 20, route_geometry[5]);
        w_u32(geometry + 24, route_geometry[6]);
        w_u32(geometry + 28, route_geometry[7]);
    }
    if (!geometry_result || r_s16(r_u32(geometry + 4) + 2) == 1)
    {
        (void)sub_8008C000(object + 172, 3);
        return 0;
    }
    (void)sub_8001E0B4(xport_guest_ptr(object + 328, 32), xport_guest_ptr(geometry, 1), 32);
    source = r_u32(object + 340);
    w_u32(object + 184, r_u32(source + 20));
    w_u32(object + 188, r_u32(source + 24));
    w_u32(object + 192, r_u32(source + 28));
    (void)sub_8002EAD8(xport_guest_ptr(source + 20, 12), xport_guest_ptr(source + 14, 6), r_u32(object + 348), xport_guest_ptr(object + 184, 12));
    w_u16(object + 450, 0);
    w_u16(object + 452, 0);
    w_u16(object + 454, 0);
    {
        const uint16_t route_direction[3] = {r_u16(object + 450), r_u16(object + 450 + 2), r_u16(object + 450 + 4)};
        const uint32_t route_record[8] = {r_u32(object + 328), r_u32(object + 328 + 4), r_u32(object + 328 + 8), r_u32(object + 328 + 12), r_u32(object + 328 + 16), r_u32(object + 328 + 20), r_u32(object + 328 + 24), r_u32(object + 328 + 28)};
        uint32_t route_position[3] = {r_u32(object + 360), r_u32(object + 360 + 4), r_u32(object + 360 + 8)};
        uint32_t route_result = sub_8003662C(route_direction, route_record, route_position);
        w_u32(object + 360, route_position[0]);
        w_u32(object + 360 + 4, route_position[1]);
        w_u32(object + 360 + 8, route_position[2]);
        (void)route_result;
    }
    {
        uint32_t secondary = r_u32(actor + 852);
        uint32_t reference;
        uint32_t progress;
        uint32_t position[3];
        if (r_u32(secondary + 604) < 3)
        {
            position[0] = r_u32(actor + 184);
            position[1] = r_u32(actor + 188);
            position[2] = r_u32(actor + 192);
            reference = r_u32(actor + 360);
            progress = r_u32(actor + 368);
        }
        else
        {
            position[0] = r_u32(secondary + 184);
            position[1] = r_u32(secondary + 188);
            position[2] = r_u32(secondary + 192);
            reference = r_u32(secondary + 360);
            progress = r_u32(secondary + 368);
        }
        if (r_u32(object + 360) == reference)
        {
            int32_t delta = ((int32_t)(uint32_t)(progress - r_u32(object + 368)));
            distance = ((int32_t)(uint32_t)((uint32_t)(((delta >> 31) + delta) ^ (delta >> 31)))) >> 16;
        }
        else
        {
            int32_t dx = ((int32_t)position[0] >> 16) - r_s16(object + 186);
            int32_t dz = ((int32_t)position[2] >> 16) - r_s16(object + 194);
            int32_t major = (int32_t)((scalar_sign_110 = ((int32_t)(dx)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_110 + ((int32_t)(dx))) ^ scalar_sign_110)))));
            int32_t minor = (int32_t)((scalar_sign_111 = ((int32_t)(dz)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_111 + ((int32_t)(dz))) ^ scalar_sign_111)))));
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
        uint32_t race = r_u32(0x8005B2F8);
        if (r_u8(race + 4) != 44 && ((int32_t)(uint32_t)(r_u32(race + 16))) > 0 && ((int32_t)(uint32_t)((uint32_t)distance << 16)) <= 0x77FFFF)
        {
            (void)sub_8008C000(object + 172, 3);
            return 0;
        }
    }
    w_u32(object + 364, r_u32(config + 4));
    turn = (int16_t)r_u16(specification + 64);
    if ((int32_t)((scalar_sign_112 = ((int32_t)(turn)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_112 + ((int32_t)(turn))) ^ scalar_sign_112))))) == 4)
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
    if (r_u32(object + 372))
    {
        int32_t first = sub_8009E768(object);
        int32_t second = sub_8009E768(object);
        int32_t third = sub_8009E768(object);
        w_u32(object + 344, (uint32_t)(((first >> 31) + second) ^ (third >> 31)));
    }
    else
    {
        w_u32(object + 344, 0x0001CCCC);
    }
    if (side < 0)
        w_u32(object + 344, (uint32_t)-((int32_t)(uint32_t)(r_u32(object + 344))));
    if (r_s8(object + 508) < 0)
        w_u32(object + 344, (uint32_t)-((int32_t)(uint32_t)(r_u32(object + 344))));
    (void)sub_8002EAD8(xport_guest_ptr(object + 184, 12), xport_guest_ptr(source + 2, 6), r_u32(object + 344), xport_guest_ptr(object + 184, 12));
    sub_8003DF54(object, 1, 0xFFFFFFFF);
    w_u32(object + 492, 0);
    w_u32(object + 496, 0);
    w_u16(object + 438, (uint16_t)-r_s16(source + 8));
    w_u16(object + 440, (uint16_t)-r_s16(source + 10));
    w_u16(object + 442, (uint16_t)-r_s16(source + 12));
    for (index = 0; index < 3; ++index)
    {
        w_u16(object + 444 + 2 * index, r_u16(source + 14 + 2 * index));
        w_u16(object + 432 + 2 * index, r_u16(source + 2 + 2 * index));
    }
    if (side < 0)
    {
        for (index = 0; index < 3; ++index)
        {
            w_u16(object + 432 + 2 * index, (uint16_t)-r_s16(object + 432 + 2 * index));
            w_u16(object + 444 + 2 * index, (uint16_t)-r_s16(object + 444 + 2 * index));
        }
    }
    (void)sub_8003AF9C(object + 172, 0, actor);
    w_u32(object + 324, sub_8003B61C(object + 172));
    w_u32(object + 180, sub_8002FAD4(object, 3, r_u16(specification + 2), 0));
    if (r_u32(object + 180) == 0xFFFF)
    {
        (void)sub_8008C000(object + 172, 3);
        return 0;
    }
    (void)sub_80012FC8(object, 0);
    (void)sub_8008BA18(object);
    axis = sub_80020018((uint32_t)(int32_t)r_s16(object + 444), (uint32_t)(int32_t)r_s16(object + 448));
    w_u32(object + 292, axis);
    w_u32(object + 296, (uint32_t)((int32_t)r_s16(0x8005624E + 4 * (axis & 0xFFF)) << 4));
    w_u32(object + 300, (uint32_t)((int32_t)r_s16(0x8005624C + 4 * (axis & 0xFFF)) << 4));
    w_u32(object + 484, 0x00026666);
    w_u16(object + 452, r_u16(object + 446));
    w_u16(object + 454, r_u16(object + 448));
    w_u16(object + 450, r_u16(object + 444));
    w_u8(object + 509, 0);
    w_u8(object + 510, 0);
    w_u32(object + 316, 0x05500000);
    w_u32(object + 504, 0xFFFF0000);
    w_u8(object + 511, 0xFF);
    w_u16(object + 320, (uint16_t)sub_80039F68(object + 172));
    w_u32(object + 176, 0xFFFFFFFF);
    if (!r_s16(object + 320))
    {
        (void)sub_8008C000(object + 172, 3);
        object = 0;
    }
    if ((r_u8(r_u32(0x8005B2F8) + 4) & 1) && object && !r_u32(object + 180))
        (void)sub_80028034(object);
    return object;
}

uint32_t sub_8009AD48(uint32_t specification, uint32_t actor)
{
    FUNCTION_MARKER(0x8009AD48, "RASHCDG.BIN");
    return sub_8009AD50((uint32_t)(int32_t)r_s16(0x800CE578), specification, actor);
}

uint32_t sub_8009D664(uint32_t actor, uint32_t source)
{
    uint32_t race = r_u32(0x8005B2F8);
    uint32_t specification = 0x1F800380;
    uint32_t route = 0;
    uint32_t route_id = 0xFFFFFFFF;
    uint32_t create = 0;
    int32_t direction = 0;
    uint32_t index;
    FUNCTION_MARKER(0x8009D664, "RASHCDG.BIN");
    if (sub_8009D9E4() && r_u32(0x8005B284) != 0xFFFFFFFF && ((int32_t)(uint32_t)(r_u32(race + 16) - r_u32(0x8005B284))) < 500)
        return 0;
    if (!sub_8008CDF4(3))
    {
        int32_t remaining = ((int32_t)(uint32_t)(r_u32(r_u32(0x800CE50C))));
        uint32_t object = r_u32(0x800CE500);
        uint32_t stride = r_u32(0x800CE504);
        uint32_t candidate = 0;
        while (remaining >= 0)
        {
            if (object && r_u16(object + 172) && r_u32(object + 180))
            {
                uint32_t player;
                for (player = 0; ((int32_t)(uint32_t)(player)) < ((int32_t)(uint32_t)(r_u32(race + 48))); ++player)
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
    if ((r_u32(actor + 360) >> 16) == 1)
    {
        route_id = r_u32(actor + 360) & 0xFFFF;
    }
    else
    {
        int32_t progress;
        direction = -((int32_t)(uint32_t)(r_u32(actor + 364)));
        if (((int32_t)(uint32_t)(r_u32(actor + 480))) < 131)
            direction = ((int32_t)(uint32_t)(r_u32(actor + 364)));
        route = sub_800245DC(r_u32(actor + 360) & 0xFFFF);
        if (route)
        {
            if (direction > 0)
            {
                route_id = r_u32(route + 12);
                progress = ((int32_t)(uint32_t)((r_u32(route + 4) >> 6) << 16)) - ((int32_t)(uint32_t)(r_u32(actor + 368)));
            }
            else
            {
                route_id = r_u32(route + 8);
                progress = ((int32_t)(uint32_t)(r_u32(actor + 368)));
            }
            if (progress > 0x7D0000)
            {
                w_u32(specification + 8, r_u32(actor + 360));
                w_u32(specification + 36, direction > 0 ? r_u32(actor + 368) - 0x7D0000 : r_u32(actor + 368) + 0x7D0000);
                w_u16(specification + 60, (uint16_t)direction);
                create = 1;
            }
        }
    }
    if (!create && route_id != 0xFFFFFFFF && route)
    {
        uint32_t alternatives = sub_800245F4(route_id);
        if (alternatives)
        {
            uint32_t count = r_u32(alternatives + 4);
            uint32_t entry = alternatives + 8;
            uint32_t found = 0;
            if (count >= 2)
            {
                if ((r_u32(actor + 360) >> 16) == 1)
                {
                    index = sub_8001FC58() % count;
                    entry += 8 * index;
                    found = 1;
                }
                else
                {
                    for (index = 0; index < count; ++index, entry += 8)
                    {
                        if (r_u32(entry) != (r_u32(actor + 360) & 0xFFFF))
                        {
                            found = 1;
                            break;
                        }
                    }
                }
                if (found && sub_800245DC(r_u32(entry)))
                {
                    uint32_t next_route = sub_800245DC(r_u32(entry));
                    w_u32(specification + 8, r_u32(entry));
                    if (((int32_t)(uint32_t)(r_u32(entry + 4))) > 0)
                        w_u32(specification + 36, ((r_u32(next_route + 4) >> 6) << 16) - 0x780000);
                    else
                        w_u32(specification + 36, 0x780000);
                    w_u16(specification + 60, r_u16(entry + 4));
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
    w_u16(specification + 64, 4);
    w_u16(specification + 2, (uint16_t)(r_u16(0x800CE578) - 1));
    w_u32(0x8005B284, r_u32(race + 16));
    {
        uint32_t object = sub_8009AD48(specification, actor);
        if (object)
        {
            w_u32(object + 480, 0x00283BBA);
            w_u8(object + 511, r_u8(source + 172));
        }
        return object;
    }
}

uint32_t sub_80096F30(uint32_t actor, uint32_t other, uint32_t mode)
{
    uint32_t race = r_u32(0x8005B2F8);
    uint32_t descriptor;
    uint32_t secondary;
    uint32_t result = 0;
    FUNCTION_MARKER(0x80096F30, "RASHCDG.BIN");
    if (r_u16(actor + 172) >= r_u32(race + 48))
        return 0;
    descriptor = r_u32(actor + 1084);
    if ((r_u8(descriptor + 1) & 0x0F) != 2)
        return 0;
    secondary = r_u32(actor + 852);
    if (r_u32(secondary + 604) >= 2 || (r_u32(secondary + 552) & 0x8000) || r_u32(0x8005AD48))
        return 0;
    if (r_u32(r_u32(other + 1084) + 40))
        return 0;
    if (r_u16(other + 172) == r_u8(race + 6))
    {
        if (r_u8(race + 4) == 33)
        {
            w_u8(descriptor + 39, 0xF8);
            w_u32(descriptor + 40, r_u32(race + 16));
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
        w_u32(0x8005AD44, r_u32(0x8005AD44) - 1);
        if (((int32_t)(uint32_t)(r_u32(0x8005AD44))) <= 0)
            w_u8(descriptor + 39, 0xFD);
    }
    (void)sub_80097308(other);
    if (r_u8(race + 4) != 33)
    {
        uint32_t obstacle = sub_8009D664(actor, other);
        if (obstacle)
        {
            uint32_t record = 0x800CD898 + 1132 * r_u16(actor + 172);
            w_u32(record + 776, 8);
            w_u32(record + 752, obstacle + 184);
            w_u32(record + 756, obstacle + 320);
            w_u32(record + 552, r_u32(record + 552) | 0x10);
            (void)sub_8008A998(record, 12);
        }
    }
    w_u32(actor + 560, r_u32(actor + 560) | 0x28000000);
    if (r_u32(actor + 560) & 0x08000000)
    {
        uint32_t type = r_u16(secondary + 544);
        if (r_u16(0x800541D4 + 8 * type + 2) == 3)
        {
            uint32_t table = r_u32(0x8005AD4C) + 12 * r_u8(secondary + 569);
            (void)sub_800C4550(r_u16(table), secondary, 2);
        }
    }
    (void)sub_800BCD10(actor);
    {
        uint32_t event = 0x1F8003D0;
        uint16_t state = ((int32_t)(uint32_t)(r_u32(actor + 480))) > 585944 ? 4 : 1;
        w_u16(event, state);
        w_u16(event + 2, 224);
        (void)sub_800BCA68(xport_guest_ptr(event, 8), 1, actor);
        w_u32(0x8005AD48, r_u32(0x8005AD48) | 2);
        state = r_u16(event);
        if (state == 1)
        {
            w_u32(actor + 720, 0x20000);
            w_u32(actor + 484, 0);
            w_u32(actor + 576, 0);
            w_u32(actor + 480, 0);
            w_u32(actor + 924, 0);
            w_u16(event + 4, 0);
            w_u32(actor + 464, 0);
            w_u32(actor + 460, 0);
            w_u32(actor + 456, 0);
        }
        else
        {
            w_u32(actor + 924, r_u32(0x80053048 + 4 * r_u32(race + 60)));
        }
    }
    result = (uint8_t)(r_u8(descriptor + 39) + 4) < 2;
    if (result)
    {
        uint32_t record = 0x800CD898 + 1132 * r_u16(actor + 172);
        w_u32(0x8005B230, 0);
        if (!(r_u32(record + 552) & 0x44))
            (void)sub_8008A998(record, mode);
        w_u32(descriptor + 40, r_u32(race + 16));
        result = 2;
        if (r_u8(descriptor + 39) == 0xFD && r_u32(race + 48) == 2)
        {
            uint32_t other_actor = r_u32(0x8005B268 + 4 * (r_u16(actor + 172) == 0));
            result = r_u32(race + 16);
            w_u32(r_u32(other_actor + 1084) + 40, result);
        }
    }
    return result;
}

uint32_t sub_800BF674(uint32_t object, uint8_t event, uint32_t direction, uint32_t actor)
{
    uint32_t secondary = r_u32(object + 852);
    uint32_t flags = r_u32(object + 568);
    uint32_t value;
    uint32_t special = (uint8_t)(event - 36) < 2 || event == 75 || event == 76 || event == 146 || event == 147;
    uint32_t race = r_u32(0x8005B2F8);
    uint32_t actor_id = r_u16(actor + 172);
    uint32_t result;
    FUNCTION_MARKER(0x800BF674, "RASHCDG.BIN");
    if (((int32_t)(uint32_t)(r_u32(object + 364))) < 0)
        flags |= 0x400000;
    else
        flags &= 0xFFBFFFFFu;
    w_u32(object + 568, flags);
    w_u32(secondary + 552, r_u32(secondary + 552) & 0xFFF9FFFFu);
    if (r_u32(object + 856) || (flags & 0x400))
    {
        if (special)
            value = 0x8000;
        else
            value = ((0u - direction) & 0xFFFE0000u) + 0x40000u | 0x8000u;
        w_u32(secondary + 552, r_u32(secondary + 552) | value);
        w_u32(object + 720, 0xFFFF0000);
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
            w_u32(secondary + 552, r_u32(secondary + 552) | 0x8000u | ((negated & 0xFFFE0000u) + 0x40000u));
        }
        w_u32(object + 568, (r_u32(object + 568) & 0xFFFF9FC0u) | value);
    }
    result = actor_id < r_u32(race + 48);
    if (result)
    {
        uint32_t slot = actor_id > 1 ? 1 : actor_id;
        w_u32(0x800D6198 + 224 * slot + 140, 1);
        result = r_u8(race + 4) & 1;
        if (result)
            return sub_80096F30(actor, object, 9);
    }
    return result;
}

uint32_t sub_800C17B0(uint32_t first, uint32_t second, uint32_t flags, uint32_t active)
{
    uint32_t first_secondary = r_u32(first + 852);
    uint32_t second_secondary = r_u32(second + 852);
    uint32_t first_descriptor = r_u32(first + 1084);
    uint32_t second_descriptor = r_u32(second + 1084);
    uint32_t race = r_u32(0x8005B2F8);
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
            uint32_t id = r_u32(second + 1088) ? r_u16(second + 172) : r_u16(r_u32(second + 856) + 172);
            w_u32(local + 4, r_u16(first_descriptor + 66));
            (void)sub_800A8BE0((uint16_t)id, local + 4);
            w_u16(first_descriptor + 66, r_u16(local + 4));
        }
        if (!(r_u8(second_secondary + 572) & 0x20))
        {
            w_u32(local + 4, r_u16(second_descriptor + 64));
            (void)sub_800A8BE0(r_u16(first + 172), local + 4);
            w_u16(second_descriptor + 64, r_u16(local + 4));
        }
        if (r_u8(first_secondary + 572) & 0x80)
        {
            w_u32(local, 3 * r_u32(local));
            (void)sub_80017BA0(r_u32(first + 184), r_u32(first + 192), 105, 0);
        }
        {
            uint32_t strength = r_u8(first_descriptor + 15);
            int32_t product;
            int32_t divisor;
            int32_t remaining;
            if (strength < 0x61)
                strength = 0x60;
            product = ((int32_t)(uint32_t)(r_u32(local) * strength * r_u8(first_descriptor + 12)));
            damage = product < 0 ? (product + 0x3FFF) >> 14 : product >> 14;
            divisor = ((int32_t)(uint32_t)(r_u32(0x80053188 + 4 * r_u32(race + 60))));
            divisor = divisor ? divisor : 1;
            remaining = (int32_t)r_u8(second_descriptor + 15) - damage;
            {
                int32_t decrement = (damage + 1) / divisor;
                if (remaining < 0)
                {
                    uint32_t sound = (r_u8(first_descriptor + 1) & 0x0F) ? 97 : 96;
                    w_u8(second_descriptor + 15, 0);
                    if (r_u16(first + 172) < r_u32(race + 48) || !r_u32(first + 1088))
                    {
                        (void)(uint32_t)sub_80017B6C(first, 109, 4, 0);
                        (void)(uint32_t)sub_80017B6C(first, sound, 10, 0);
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
                    uint32_t linked_descriptor = r_u32(r_u32(second + 856) + 1084);
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
        if (!(r_u32(second + 568) & 0x3EC) && !(r_u32(second + 560) & 0x20000000))
        {
            (void)sub_800BF674(second, r_u8(first_descriptor + 60), (flags >> 8) & 1, first);
            (void)sub_800BFE58(first, 1, 0);
            (void)sub_800BFE58(second, 1, 1);
        }
        if (r_u32(second + 856) != first)
        {
            uint32_t target = r_u32(second + 1088) ? second : r_u32(second + 856);
            uint32_t descriptor = r_u32(target + 1084);
            int32_t value = (int32_t)r_u8(descriptor + 37) - (r_u8(descriptor + 36) >> 4);
            w_u8(descriptor + 37, (uint8_t)(value < 0 ? 0 : value));
        }
    }
    (void)sub_800BF424(first, second);
    if (r_u32(second_secondary + 604) < 2)
    {
        uint32_t base_flags = flags & 0xFFFFFFF0u;
        uint32_t dispatch = base_flags ^ 0x100;
        if (active & 1)
        {
            if (!(r_u32(second + 568) & 0x7FF))
            {
                if (r_u32(second + 1088))
                {
                    uint32_t row = r_u8(first_secondary + 569);
                    if (r_u8(first_secondary + 572) & 0x20)
                        row -= 20;
                    (void)sub_800BF604(second, r_u8(first_secondary + 570), row, (base_flags >> 8) & 1);
                }
                if ((r_u16(second + 172) < r_u32(race + 48) || (!r_u32(second + 1088) && !(r_u8(second_secondary + 572) & 0x40))) && !r_u32(0x8005B220))
                    (void)sub_800BFD74(second, damage);
            }
            {
                uint32_t animation = r_u32(first_secondary + 540);
                uint32_t state = r_u32(animation + 36);
                w_u32(animation + 36, state | 0x80);
            }
            dispatch = base_flags ^ 0x100;
        }
        completed = 1;
        (void)sub_800C4550(event, second_secondary, dispatch | 0x10);
    }
    if (!(active & 1))
        return completed;
    if ((r_u16(first + 172) < r_u32(race + 48) || (!r_u32(first + 1088) && !(r_u8(first_secondary + 572) & 0x40))) && !r_u32(0x8005B220))
        (void)sub_800BFD74(first, damage);
    return completed;
}

uint32_t sub_800C1370(uint32_t first, uint32_t second)
{
    int32_t scalar_sign_113;
    int32_t scalar_sign_114;

    uint32_t first_secondary = r_u32(first + 852);
    uint32_t first_descriptor = r_u32(first + 1084);
    uint32_t relation = r_u32(second + 360);
    int32_t longitudinal;
    int32_t lateral;
    uint32_t direction;
    uint32_t flags_out = 0;
    FUNCTION_MARKER(0x800C1370, "RASHCDG.BIN");
    if (relation == r_u32(first + 360) && (!(relation >> 16) || r_u32(second + 336) == r_u32(first + 336)))
    {
        longitudinal = ((int32_t)(uint32_t)(r_u32(second + 344) - r_u32(first + 344)));
        if (((int32_t)(uint32_t)(r_u32(first + 364))) < 0)
            longitudinal = -longitudinal;
    }
    else
    {
        longitudinal = ((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(second + 184, 12), xport_guest_ptr(first + 432, 6), xport_guest_ptr(first + 184, 12))));
    }
    direction = longitudinal > 0 ? 0x100 : 0;
    longitudinal = (int32_t)((scalar_sign_113 = ((int32_t)(longitudinal)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_113 + ((int32_t)(longitudinal))) ^ scalar_sign_113)))));
    lateral = ((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(first + 504, 12), xport_guest_ptr(second + 528, 6), xport_guest_ptr(second + 504, 12))));
    if (r_u16(0x800541D4 + 8 * r_u16(first_secondary + 544) + 2) == 3)
    {
        w_u8(first_descriptor + 60, 71);
        if (sub_800C09B0(first))
            flags_out = 32;
    }
    else if ((int32_t)((scalar_sign_114 = ((int32_t)(lateral)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_114 + ((int32_t)(lateral))) ^ scalar_sign_114))))) <= 65534 && longitudinal < ((int32_t)(uint32_t)(r_u32(first_descriptor + 8) + r_u32(first + 304))))
    {
        w_u8(first_descriptor + 60, 71);
        (void)sub_800C122C(first, second, direction);
        flags_out = 48;
    }
    if (flags_out)
    {
        uint32_t result = sub_800C159C(first, second, longitudinal, lateral, &flags_out);
        if (result)
            return sub_800C17B0(first, second, direction, flags_out);
        return result;
    }
    if (r_u32(first_secondary + 604) == 1)
    {
        uint32_t type = r_u16(first_secondary + 544);
        if (r_u16(0x800541D4 + 8 * type + 2) != 3)
        {
            if (type != 16 || sub_8005BE58(r_u32(first_secondary + 540)))
                return sub_800C4550(16, first_secondary, direction | 2);
        }
        return 16;
    }
    return 1;
}

uint32_t sub_800BBE00(uint32_t actor, uint16_t other_id)
{
    int32_t scalar_sign_115;

    uint32_t other;
    int32_t delta;
    FUNCTION_MARKER(0x800BBE00, "RASHCDG.BIN");
    if (other_id == 224)
        return 224;
    other = r_u32(0x8005B3A0) + 1096 * other_id;
    delta = ((int32_t)(uint32_t)((r_u32(actor + 324) - r_u32(other + 324)) << 4));
    if (sub_800BC1EC(actor, other, delta) && (int32_t)((scalar_sign_115 = ((int32_t)(delta)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_115 + ((int32_t)(delta))) ^ scalar_sign_115))))) < ((int32_t)(uint32_t)(r_u32(0x80052F70))))
        (void)sub_800C1370(actor, other);
    return sub_800BBC20(actor, other_id);
}

uint32_t sub_800C035C(uint32_t actor, uint16_t other_id, uint32_t retained_product_hi)
{
    int32_t scalar_sign_116;
    int32_t scalar_sign_117;
    int32_t scalar_sign_118;

    uint32_t secondary = r_u32(actor + 852);
    uint32_t descriptor = r_u32(actor + 1084);
    uint32_t race = r_u32(0x8005B2F8);
    uint32_t other = other_id == 224 ? 0 : r_u32(0x8005B3A0) + 1096 * other_id;
    uint32_t setup = 0;
    uint32_t pending = 0;
    uint32_t flags_out = 0;
    uint32_t type = r_u16(secondary + 544);
    uint32_t result = 0;
    int32_t lateral;
    int32_t longitudinal;
    uint32_t response;
    uint32_t linked;
    FUNCTION_MARKER(0x800C035C, "RASHCDG.BIN");
    if (other_id != 224)
    {
        uint32_t other_secondary = r_u32(other + 852);
        uint32_t other_linked = r_u32(other + 856);
        uint32_t linked_secondary = r_u32(other_linked + 852);
        uint32_t physical = linked_secondary & 0x1FFFFFFFu;

        if (physical > 0x1FFFFCu && !(physical >= 0x1FC00000u && physical <= 0x1FC7FFFCu))
        {
            const uint32_t args[7] = {actor, other_id, other, other_secondary, other_linked, r_u32(other + 1084), linked_secondary};

            rrj_wip_handoff(rrj_host_context(), 0x800C035Cu, "race_collision", __func__, __FILE__, __LINE__, "invalid_linked_secondary", args, 7);
            exit(20);
        }
    }
    flags_out = (r_u8(descriptor + 60) & 0x40) ? 32 : 0;
    if (!(r_u32(actor + 560) & 0x08000000) && !(r_u8(secondary + 572) & 0x40))
    {
        if (type - 30 < 8)
        {
            rrj_trace_dispatch_target(0x800C1014u);
            return sub_800C1014(secondary);
        }
        if (r_u16(0x800541D4 + 8 * type + 2) == 3)
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
    lateral = ((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(actor + 504, 12), xport_guest_ptr(other + 528, 6), xport_guest_ptr(other + 504, 12))));
    if (retained_product_hi)
    {
        int64_t product = (int64_t)r_s16(other + 532) * 16 * (int64_t)(int32_t)(r_u32(actor + 512) - r_u32(other + 512));
        /* Preserve the high product word consumed by collision events */
        w_u32(retained_product_hi, (uint32_t)((uint64_t)product >> 32));
    }
    rrj_trace_dispatch_target(0x800C035Cu);
    if (r_u8(race + 57) == 3 && r_u16(other + 172) < r_u32(race + 48))
        response = 5;
    else
    {
        rrj_trace_dispatch_target(0x800BC1ECu);
        response = sub_800BC1EC(actor, other, -lateral);
        rrj_trace_dispatch_target(0x800C035Cu);
    }
    {
        uint32_t relation = r_u32(actor + 360);
        if (relation == r_u32(other + 360) && (!(relation >> 16) || r_u32(actor + 336) == r_u32(other + 336)))
        {
            longitudinal = ((int32_t)(uint32_t)(r_u32(actor + 344) - r_u32(other + 344)));
            if (((int32_t)(uint32_t)(r_u32(other + 364))) < 0)
                longitudinal = -longitudinal;
        }
        else
        {
            rrj_trace_dispatch_target(0x800B6AACu);
            longitudinal = ((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(actor + 184, 12), xport_guest_ptr(other + 432, 6), xport_guest_ptr(other + 184, 12))));
            if (retained_product_hi)
            {
                int64_t product = (int64_t)r_s16(other + 436) * 16 * (int64_t)(int32_t)(r_u32(actor + 192) - r_u32(other + 192));
                /* Preserve the high product word consumed by collision events */
                w_u32(retained_product_hi, (uint32_t)((uint64_t)product >> 32));
            }
            rrj_trace_dispatch_target(0x800C035Cu);
        }
    }
    {
        uint32_t current_secondary = r_u32(other + 852);
        uint32_t current_linked = r_u32(other + 856);
        uint32_t linked_secondary = r_u32(current_linked + 852);
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
    if (r_u8(r_u32(other + 852) + 572) & 0x10)
    {
        uint32_t linked_actor = r_u32(other + 856);
        if (linked_actor != actor && r_u32(r_u32(linked_actor + 852) + 604) < 2)
            linked = longitudinal > 0 ? 2 : 0;
    }
    if ((r_u32(actor + 560) & 0x08000000) || (r_u8(secondary + 572) & 0x40))
    {
        if (r_u16(0x800541D4 + 8 * type + 2) == 3)
            pending = sub_800C09B0(actor);
        if ((response & 4) || sub_800C0BE8(actor, other, lateral, response | linked))
        {
            if (!(r_u32(actor + 568) & 0x600) && (r_u32(actor + 560) & 0x08000000))
                (void)sub_800C0CE8(actor, other, lateral);
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
    if ((response & 4) && !((((int32_t)(uint32_t)(r_u32(other + 576))) >= 6553 && r_u32(r_u32(other + 852) + 604) == 1 && r_u32(r_u32(r_u32(other + 856) + 852) + 604) == 1) || (int32_t)((scalar_sign_116 = ((int32_t)(lateral)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_116 + ((int32_t)(lateral))) ^ scalar_sign_116))))) > 983039))
    {
        w_u32(actor + 924, 0);
        w_u32(actor + 564, r_u32(actor + 564) | 0x80000);
        w_u8(r_u32(other + 1084) + 39, 0xFE);
        if (!r_u32(r_u32(other + 1084) + 40))
        {
            if (r_u32(r_u32(other + 852) + 604) < 2)
                (void)sub_80092C7C(other, 9);
            w_u32(r_u32(other + 1084) + 40, r_u32(race + 16));
        }
        w_u32(other + 924, 0);
        w_u32(other + 564, r_u32(other + 564) | 0x80000);
        return r_u32(other + 564);
    }
    if (linked)
        longitudinal = ((int32_t)(uint32_t)((uint32_t)longitudinal - r_u32(other + 304) - r_u32(r_u32(other + 856) + 304)));
    if ((int32_t)((scalar_sign_117 = ((int32_t)(longitudinal)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_117 + ((int32_t)(longitudinal))) ^ scalar_sign_117))))) > 2 * ((int32_t)(uint32_t)(r_u32(0x80052F74))) || (int32_t)((scalar_sign_118 = ((int32_t)(lateral)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_118 + ((int32_t)(lateral))) ^ scalar_sign_118))))) > 45875)
        return 0;
    {
        uint32_t direction = longitudinal > 0 ? 0x100 : 0;
        if (linked)
            other = r_u32(other + 856);
        if (((r_u32(actor + 560) & 0x08000000) || (r_u8(secondary + 572) & 0x40)) && r_u16(0x800541D4 + 8 * type + 2) != 3)
        {
            (void)sub_800C122C(actor, other, direction);
            setup = 1;
        }
        if (setup && r_u8(descriptor + 47) && (r_u8(descriptor + 60) & 0x80) && r_s8(secondary + 571) != -1)
        {
            uint32_t lane = r_u8(descriptor + 46);
            flags_out |= 0x100;
            if (sub_800C159C(actor, other, longitudinal, lateral, &flags_out))
            {
                xport_update_u8(descriptor + 47, XPORT_MEMORY_UPDATE_SUBTRACT, 1);
                w_u32(descriptor + 48, (r_u32(descriptor + 48) & ~(0x0Fu << (4 * lane))) | ((r_u8(descriptor + 47) & 0x0F) << (4 * lane)));
            }
            flags_out &= 0xFFFFFEFFu;
        }
        if (r_u8(descriptor + 60) == 32)
            (void)sub_800BFF04(actor, other, direction, &flags_out);
        if (pending && sub_800C159C(actor, other, longitudinal, lateral, &flags_out))
            return sub_800C17B0(actor, other, direction, flags_out);
    }
    return result;
}

uint32_t sub_80095BF8(uint32_t actor, uint32_t delta)
{
    uint32_t race = r_u32(0x8005B2F8);
    uint32_t contact = actor + 948 + 8 * (uint32_t)(int32_t)r_s8(actor + 946);
    uint32_t reference = r_u32(0x8005B38C);
    uint32_t descriptor;
    uint32_t type;
    uint32_t course;
    uint32_t target;
    int32_t rank_delta;
    uint32_t mode;
    uint32_t table_index;
    uint32_t curve;
    int32_t position;
    int32_t speed;

    FUNCTION_MARKER(0x80095BF8, "RASHCDG.BIN");
    if (r_u32(race + 48) == 2)
    {
        uint32_t first = reference;
        uint32_t second = r_u32(0x8005B21C);
        uint32_t first_descriptor = r_u32(first + 1084);
        uint32_t second_descriptor = r_u32(second + 1084);
        if (r_u32(first_descriptor + 40) || r_u8(first_descriptor + 39) >= 0xF8)
            reference = second;
        else if (r_u32(second_descriptor + 40) || r_u8(second_descriptor + 39) >= 0xF8)
            reference = first;
        else
        {
            uint32_t nearer = (int32_t)r_u32(first + 324) < (int32_t)r_u32(second + 324);
            int32_t gap = (int32_t)r_u8(first_descriptor + 39) - r_u8(second_descriptor + 39);
            uint32_t candidate = r_u32(0x8005B268 + 4 * nearer);
            int32_t threshold = r_u8(r_u32(candidate + 1084) + 39) - (gap < 0 ? -gap : gap) / 2;
            uint32_t choose = nearer ^ ((int32_t)r_u8(r_u32(actor + 1084) + 39) < threshold);
            reference = r_u32(0x8005B268 + 4 * choose);
        }
    }
    descriptor = r_u32(actor + 1084);
    type = r_u8(descriptor + 1) & 15;
    course = r_u32(race + 60);
    target = r_u32(actor + 556) + 224;
    if (type == 2)
    {
        int32_t elapsed;
        if (!(r_u8(actor + 928) & 16))
            return 0;
        if (r_u8(race + 57) >= 4)
            return r_u32(0x80053030 + 4 * course);
        elapsed = (int32_t)(r_u32(race + 16) - ((uint32_t)r_u16(descriptor + 62) << 8)) / 300;
        elapsed = (int32_t)((uint32_t)elapsed - (uint32_t)(int32_t)r_s16(0x80053146 + 4 * course));
        if (elapsed > 0)
        {
            int32_t divisor = r_s16(0x80053152 + 4 * course);
            int32_t quotient = divisor ? (int32_t)((int64_t)elapsed / divisor) : -1;
            uint32_t reduction = ((uint32_t)quotient + 1) * r_u32(0x8005315C + 4 * course);
            int32_t adjusted = (int32_t)(r_u32(0x800D86F8) * (128 - reduction)) / 128;
            int32_t floor = (int32_t)r_u32(0x80053138 + 4 * course);
            if (adjusted < floor)
                adjusted = floor;
            if (adjusted < (int32_t)r_u32(target))
                w_u32(target, (uint32_t)adjusted);
        }
        position = (int32_t)(r_u32(actor + 324) - r_u32(reference + 324)) >> 12;
        speed = (int32_t)r_u32(target);
        if (r_s16(actor + 320) && position >= -63 && position < 12)
            w_u16(actor + 320, r_u16(actor + 320) | 2);
        if (r_u16(contact) == 4 && position < 0 && (r_u16(contact + 2) >> 5))
            return r_u32(0x80053138 + 4 * r_u32(race + 60));
        return (uint32_t)speed;
    }
    if (r_u8(race + 57) == 1 && r_u32(0x800CCC18))
        return r_u32(r_u32(0x800CCC18) + 480);
    if (r_u8(race + 57) == 3 && type == (uint32_t)(r_u8(r_u32(r_u32(0x8005B38C) + 1084) + 1) & 15))
        return 0;
    rank_delta = (int32_t)r_u8(r_u32(reference + 1084) + 39) - r_u8(descriptor + 39);
    if ((rank_delta < 0 ? -rank_delta : rank_delta) > (int32_t)r_u32(0x8005B1F8))
        return r_u32(target);
    mode = r_u8(race + 4);
    if ((mode & 1) && (r_u32(0x8005AD48) & 1))
        return r_u32((r_u32(0x8005B2B0) ? 0x80053054 : 0x8005303C) + 4 * course);
    table_index = (mode & 4) ? course + 6 : course + ((0 - (mode & 1)) & 3);
    curve = r_u32(0x80052FD0 + 4 * table_index);
    if ((int32_t)r_u32(race + 16) < (int32_t)((uint32_t)((int32_t)(300 * curve) >> 16) + 300))
    {
        int32_t timer;
        w_u8(descriptor, r_u8(descriptor) | 1);
        timer = (int32_t)r_u32(actor + 704);
        if (timer > 0)
        {
            timer = (int32_t)((uint32_t)timer - delta);
            if (timer > 0)
                w_u32(actor + 704, (uint32_t)timer);
            else
            {
                w_u32(actor + 704, 0);
                w_u32(actor + 564, r_u32(actor + 564) & 0xFFFFFDFF);
            }
        }
        else
        {
            int32_t half_rank = ((int32_t)r_u8(descriptor + 39) - 1) / 2;
            uint32_t interval = r_u32(0x80052FF4 + 4 * table_index);
            int32_t threshold = (int32_t)((uint32_t)(half_rank + 1) * (uint32_t)((int32_t)(300 * interval) >> 16));
            if (threshold < (int32_t)r_u32(race + 16) && !(r_u8(descriptor) & 2))
            {
                w_u32(actor + 564, r_u32(actor + 564) | 0x200);
                w_u32(actor + 704, curve - (uint32_t)half_rank * 2 * interval);
                w_u8(descriptor, r_u8(descriptor) | 2);
            }
        }
        return r_u32(target);
    }
    w_u8(descriptor, r_u8(descriptor) & 0xFE);
    position = (int32_t)r_u32(actor + 324);
    if (position <= 0)
        return 0xA0000;
    position = (int32_t)((uint32_t)position - r_u32(reference + 324)) >> 12;
    speed = (int32_t)r_u32(target);
    if (r_u8(race + 4) == 44 && (r_u8(descriptor) & 16))
        return (uint32_t)((int32_t)((uint32_t)r_u8(descriptor + 69) * (uint32_t)speed) / 128);
    if (r_u8(descriptor) & 8)
    {
        uint32_t comparison = r_u32(reference + 480) + 0xA0000;
        uint32_t flags = r_u32(actor + 564);
        if (position > 0)
            comparison -= 0xC0000;
        if ((int32_t)((uint32_t)speed - 0x20000) < (int32_t)comparison)
            flags |= 0x200;
        w_u32(actor + 564, flags);
        return comparison;
    }
    if (r_s16(actor + 320) && position >= -63 && position < 12)
    {
        int32_t product = (int32_t)((uint32_t)r_u8(descriptor + 69) * (uint32_t)speed);
        w_u8(actor + 928, r_u8(actor + 928) | 8);
        w_u16(actor + 320, r_u16(actor + 320) | 2);
        return (uint32_t)(product / 128);
    }
    if (r_u8(actor + 928) & 8)
    {
        int32_t phase;
        int32_t rank;
        int32_t band;
        int32_t extent;
        int32_t direction;
        int32_t gap;
        int32_t first;
        int32_t second;
        int32_t numerator;
        int32_t correction;
        uint32_t table;

        if (r_u32(0x800CCA84) != r_u32(race + 16))
        {
            int32_t progress = (int32_t)((uint32_t)((int32_t)(r_u32(reference + 324) + 4096) >> 12) * 4);
            int32_t divisor = (int32_t)r_u32(0x8005B244);
            int32_t quotient = divisor ? (int32_t)((int64_t)progress / divisor) : (progress < 0 ? 1 : -1);
            w_u32(0x800CCA84, r_u32(race + 16));
            w_u32(0x800CCA88, 3 - (uint32_t)quotient);
        }
        phase = (int32_t)r_u32(0x800CCA88);
        phase = phase >= 2 ? phase - 1 : 0;
        rank = r_u8(r_u32(reference + 1084) + 39);
        band = (rank - 1) / 3;
        if (band >= 5)
            band = 4;
        extent = rank_delta < 0 ? (int32_t)(r_u32(0x8005B1FC) - (uint32_t)rank) : rank;
        direction = rank_delta < 0 ? 3 : 0;
        gap = rank_delta < 0 ? -rank_delta : rank_delta;
        table = 0x800531AC + (uint32_t)phase * 20 + (uint32_t)band * 4;
        first = r_s8(table + (uint32_t)direction);
        second = r_s8(table + (uint32_t)(direction ^ 1));
        numerator = (int32_t)((uint32_t)first * (uint32_t)(gap - 1) + (uint32_t)second * ((uint32_t)extent - (uint32_t)gap + 1));
        correction = extent ? (int32_t)((int64_t)numerator / extent) : (numerator < 0 ? 1 : -1);
        w_u8(actor + 928, r_u8(actor + 928) & 0xF7);
        w_u8(actor + 929, (uint8_t)correction);
    }
    speed = (int32_t)(r_u32(target) * (uint32_t)((int32_t)r_s8(actor + 929) + 128)) / 128;
    if (!r_s16(actor + 320))
        return (uint32_t)sub_8001FC90(speed, (int32_t)r_u32(0x80052FA0 + 4 * (r_u8(r_u32(actor + 1084) + 1) & 15)));
    return (uint32_t)speed;
}

uint32_t sub_800954A0(uint32_t actor, uint32_t delta)
{
    uint32_t flags, dynamics, race, target, current, result;

    FUNCTION_MARKER(0x800954A0u, "RASHCDG.BIN");
    flags = r_u32(actor + 560);
    dynamics = r_u32(actor + 556);
    if (flags & 0x08000000u)
    {
        uint32_t track[8];
        uint32_t velocity = r_u32(actor + 480);
        uint32_t response, product, lateral, sign, direction, piece;
        int32_t magnitude;

        w_u32(actor + 924, r_u32(dynamics + 224));
        response = r_u32(dynamics + 420);
        product = (uint32_t)sub_8001FC90(response, velocity);
        lateral = r_u32(actor + 344);
        sign = (uint32_t)(((int32_t)(uint32_t)(lateral)) >> 31);
        magnitude = ((int32_t)(uint32_t)((((lateral + sign) ^ sign) << 1) + product));
        if (magnitude < 0xF0000)
            magnitude = 0xF0000;
        if (magnitude > 0x500000)
            magnitude = 0x500000;
        if (r_u8(r_u32(actor + 1084)) & 0x80)
            direction = 0u - r_u32(actor + 364);
        else
            direction = r_u32(actor + 364);
        (void)sub_800386DC(actor, (uint32_t)magnitude, r_u32(actor + 348), direction, xport_guest_ptr(actor + 328, 32), xport_guest_ptr(actor + 880, 12), track);
        piece = xport_load_le32((const uint8_t *)track + 12);
        result = sub_800B6AAC(xport_guest_ptr(actor + 880, 12), xport_guest_ptr(piece + 2, 6), xport_guest_ptr(piece + 20, 12));
        w_u16(actor + 878, (uint16_t)(((int32_t)(uint32_t)(result)) >> 5));
        piece = xport_load_le32((const uint8_t *)track + 12);
        w_u16(actor + 872, r_u16(piece + 2));
        piece = xport_load_le32((const uint8_t *)track + 12);
        w_u16(actor + 874, r_u16(piece + 4));
        piece = xport_load_le32((const uint8_t *)track + 12);
        w_u16(actor + 876, r_u16(piece + 6));
    }
    race = r_u32(0x8005B2F8u);
    if (r_u16(actor + 172) >= r_u32(race + 48))
        target = sub_80095BF8(actor, delta);
    else
    {
        w_u16(actor + 320, r_u16(actor + 320) | 2u);
        if (!(r_u32(actor + 560) & 0x08000000u))
            return 0;
        if (r_u8(race + 4) & 1u)
        {
            uint32_t player, desired;
            if (!sub_80096818(actor, delta))
                return 0x800D0000u;
            player = 0x800CD898u + 1132u * r_u16(actor + 172);
            result = r_u32(player + 540);
            desired = r_u32(player + 544);
            if (result != desired)
            {
                (void)sub_8008A998(player, desired);
                player = 0x800CD898u + 1132u * r_u16(actor + 172);
                (void)sub_80086AF8(player);
            }
        }
        {
            uint32_t factor = r_u8(r_u32(actor + 1084) + 69);
            uint32_t product = factor * r_u32(dynamics + 224);
            if (((int32_t)(uint32_t)(product)) < 0)
                product += 127;
            target = (uint32_t)(((int32_t)(uint32_t)(product)) >> 7);
        }
    }
    current = r_u32(actor + 924);
    result = ((int32_t)(uint32_t)(target)) < ((int32_t)(uint32_t)(current));
    if (result)
        current = target;
    w_u32(actor + 924, current);
    return result;
}

uint32_t sub_800BA304(uint32_t delta, uint32_t skip_mask, uint32_t active_out, uint32_t marked_out)
{
    uint32_t count = r_u32(0x8005B1F8);
    uint32_t actor = r_u32(0x8005B3A0);
    uint32_t index;
    FUNCTION_MARKER(0x800BA304, "RASHCDG.BIN");
    if ((int32_t)count <= 0)
        return 0x80060000u;
    for (index = 0; (int32_t)index < (int32_t)count; ++index, actor += 1096, count = r_u32(0x8005B1F8))
    {
        uint32_t bit = 1u << (index & 31);
        uint32_t state;
        uint32_t descriptor;
        if (skip_mask & bit)
            continue;
        state = r_u16(actor + 956 + 8 * (uint32_t)(int32_t)(r_s8(actor + 946) - 1));
        if (r_u32(actor + 564) & 0x200)
            w_u32(marked_out, r_u32(marked_out) | bit);
        if (state - 3 < 15)
        {
            (void)sub_800954A0(actor, delta);
            if (state < 4 || !sub_800BD388(actor))
                continue;
            w_u32(active_out, r_u32(active_out) | bit);
            if (state < 5)
                continue;
            descriptor = r_u32(actor + 1084);
            if (r_u32(descriptor + 40) || r_u8(descriptor + 39) >= 0xF8)
                continue;
            sub_800BC8DC(actor);
        }
        else
        {
            descriptor = r_u32(actor + 1084);
            if (r_u32(descriptor + 40) && ((int32_t)(uint32_t)(r_u32(actor + 480))) >= 132 && (r_u32(actor + 560) & 0x08000000) && r_s16(actor + 320))
                (void)sub_800954A0(actor, delta);
        }
    }
    return (int32_t)index < (int32_t)count;
}

static void race_leaf_copy9(uint32_t source, uint32_t destination)
{
    uint32_t index;
    for (index = 0; index < 9; ++index)
        w_u16(destination + 2 * index, r_u16(source + 2 * index));
}

static void race_leaf_heading(uint32_t object)
{
    uint32_t angle;
    uint32_t table;
    (void)sub_8008BA18(object);
    angle = sub_80020018((uint32_t)(int32_t)r_s16(object + 444), (uint32_t)(int32_t)r_s16(object + 448));
    w_u32(object + 292, angle);
    table = 0x8005624C + 4 * (angle & 0xFFF);
    w_u32(object + 296, (uint32_t)((int32_t)r_s16(table + 2) << 4));
    w_u32(object + 300, (uint32_t)((int32_t)r_s16(table) << 4));
}

uint32_t sub_800C9420(uint32_t group)
{
    const uint32_t specification = 0x1F800010;
    const uint32_t position = 0x1F800050;
    const uint32_t geometry = 0x1F800060;
    const uint32_t vector = 0x1F800090;
    const uint32_t matrix = 0x1F8000C0;
    uint32_t list = r_u32(0x8005B340 + 4 * group);
    int32_t count = ((int32_t)(uint32_t)(r_u32(0x8005B208 + 4 * group)));
    uint32_t current = r_u32(0x8005B38C);
    uint32_t actor_base = r_u32(0x8005B3A0);
    uint32_t player_count = r_u32(0x8005B1F8);
    uint32_t remaining[2];
    uint32_t actor_index = 0;
    uint32_t actor_offset = 0;
    uint32_t options = 1;
    uint32_t entry_offset = 0;
    int32_t entry_index;
    FUNCTION_MARKER(0x800C9420, "RASHCDG.BIN");
    remaining[0] = r_u16(0x8005B218);
    remaining[1] = remaining[0];
    w_u32(0x800CCC18, 0);
    if (count <= 0)
        return 0x800D0000;
    for (entry_index = 0; entry_index < count; ++entry_index)
    {
        uint32_t entry = list + entry_offset;
        uint32_t packed = r_u32(entry + 8);
        uint32_t kind = (packed >> 12) & 15;
        int32_t selector = (int32_t)packed >> 16;
        int32_t angle = -(((int32_t)(packed << 20) >> 8) / 360);
        if (((int32_t)(uint32_t)(r_u32(current + 364))) < 0)
        {
            w_u32(entry + 4, 0u - r_u32(entry + 4));
            w_u32(entry, 0u - r_u32(entry));
            angle = (angle + 2048) % 4096;
        }
        if (kind == 0)
        {
            uint32_t wanted = (r_u8(r_u32(current + 1084) + 1) & 15) == 0;
            uint32_t candidate_kind = 2;
            if (actor_index < player_count)
            {
                do
                {
                    uint32_t candidate = actor_base + 1096 * (actor_index + 1);
                    if (candidate_kind == wanted)
                        break;
                    candidate_kind = r_u8(r_u32(candidate + 1084) + 1) & 15;
                    actor_offset += 1096;
                    ++actor_index;
                } while (actor_index < player_count);
            }
            {
                uint32_t actor = actor_base + actor_offset;
                uint32_t secondary;
                if (!r_u32(0x8005B330))
                    w_u32(0x8005B330, actor);
                if (!group)
                    w_u32(r_u32(actor + 556) + 224, 0x00141DDD);
                w_u32(actor + 360, r_u16(0x80053178));
                w_u32(actor + 368, (r_u16(0x8005317A) << 16) + r_u32(entry + 4));
                w_u32(actor + 364, r_u32(current + 364));
                w_u32(actor + 292, (uint32_t)angle);
                w_u16(actor + 320, 1);
                w_u32(actor + 344, r_u32(entry));
                (void)sub_8009432C(actor);
                if (group)
                {
                    secondary = r_u32(actor + 852);
                    w_u16(actor + 966, 224);
                    w_u8(actor + 946, 2);
                    w_u16(actor + 964, 0);
                    w_u32(secondary + 604, 4);
                }
            }
        }
        else if (kind == 1)
        {
            uint32_t current_kind = r_u8(r_u32(current + 1084) + 1) & 15;
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
                uint32_t candidate_kind = r_u8(r_u32(candidate + 1084) + 1) & 15;
                if (candidate_kind == desired)
                    break;
                --remaining[side];
            }
            rider = actor_base + 1096 * remaining[side];
            body = r_u32(rider + 852);
            if (r_u32(rider + 856))
            {
                uint32_t linked = r_u32(current + 856);
                uint32_t linked_body = r_u32(linked + 852);
                xport_update_u8(linked_body + 572, XPORT_MEMORY_UPDATE_OR, 0x60);
                xport_update_u8(r_u32(current + 852) + 572, XPORT_MEMORY_UPDATE_OR, 0x10);
            }
            if (r_s8(body + 72) == 1)
                (void)sub_80068D20(rider, body, r_u32(rider + 856) != 0);
            w_u8(body + 72, 3);
            w_u32(position, r_u16(0x80053178));
            w_u32(position + 4, r_u32(current + 364));
            w_u32(position + 8, (r_u16(0x8005317A) << 16) + r_u32(entry + 4));
            w_u32(body + 360, r_u32(position));
            w_u32(body + 364, r_u32(position + 4));
            w_u32(body + 368, r_u32(position + 8));
            w_u32(body + 344, r_u32(entry));
            segment = sub_80039DFC(0, ((position) ? (const uint32_t[3]){r_u32(position), 0, (uint32_t)r_u16((position) + 10) << 16} : NULL));
            uint32_t geometry_result = 0;
            if (segment)
            {
                const uint32_t route_position[3] = {r_u32(position), r_u32(position + 4), r_u32(position + 8)};
                uint32_t route_geometry[8] = {r_u32(geometry), r_u32(geometry + 4), r_u32(geometry + 8), r_u32(geometry + 12), r_u32(geometry + 16), r_u32(geometry + 20), r_u32(geometry + 24), r_u32(geometry + 28)};
                geometry_result = sub_8003A700(segment, route_position, route_geometry);
                w_u32(geometry, route_geometry[0]);
                w_u32(geometry + 4, route_geometry[1]);
                w_u32(geometry + 8, route_geometry[2]);
                w_u32(geometry + 12, route_geometry[3]);
                w_u32(geometry + 16, route_geometry[4]);
                w_u32(geometry + 20, route_geometry[5]);
                w_u32(geometry + 24, route_geometry[6]);
                w_u32(geometry + 28, route_geometry[7]);
            }
            if (!segment || !geometry_result)
                goto next_entry;
            (void)sub_8001E0B4(xport_guest_ptr(body + 328, 32), xport_guest_ptr(geometry, 1), 32);
            source = r_u32(geometry + 12);
            (void)sub_8002EAD8(xport_guest_ptr(source + 20, 12), xport_guest_ptr(source + 14, 6), r_u32(geometry + 20), xport_guest_ptr(body + 184, 12));
            (void)sub_8002EAD8(xport_guest_ptr(body + 184, 12), xport_guest_ptr(source + 2, 6), r_u32(entry), xport_guest_ptr(body + 184, 12));
            w_u32(body + 508, 0);
            w_u16(body + 438, (uint16_t)-r_s16(source + 8));
            w_u16(body + 440, (uint16_t)-r_s16(source + 10));
            w_u16(body + 442, (uint16_t)-r_s16(source + 12));
            w_u16(body + 444, r_u16(source + 14));
            w_u16(body + 446, r_u16(source + 16));
            w_u16(body + 448, r_u16(source + 18));
            w_u16(body + 432, r_u16(source + 2));
            w_u16(body + 434, r_u16(source + 4));
            w_u16(body + 436, r_u16(source + 6));
            {
                const uint16_t rotation_axis[3] = {r_u16(source + 8), r_u16(source + 8 + 2), r_u16(source + 8 + 4)};
                uint16_t rotation_matrix[9];
                uint32_t rotation_index;
                (void)sub_8003FB34(rotation_axis, (uint32_t)angle, rotation_matrix);
                for (rotation_index = 0; rotation_index < 9; ++rotation_index)
                    w_u16(matrix + 2 * rotation_index, rotation_matrix[rotation_index]);
            }
            (void)sub_8003FA40(xport_guest_cptr(body + 432, 18), xport_guest_cptr(matrix, 18), xport_guest_ptr(body + 432, 18));
            w_u16(body + 450, r_u16(body + 444));
            w_u16(body + 452, r_u16(body + 446));
            w_u16(body + 454, r_u16(body + 448));
            race_leaf_copy9(body + 432, body + 516);
            race_leaf_heading(body);
            w_u16(body + 320, 1);
            (void)sub_80037450(body);
            if (desired == current_kind)
            {
                event = 66;
                if ((r_u16(body + 172) & 2) && !(r_u8(body + 572) & 0x40))
                {
                    uint32_t descriptor = r_u32(r_u32(body + 596) + 1084);
                    w_u8(descriptor + 46, 5);
                    w_u8(descriptor + 47, 0);
                    (void)sub_800958F0(body, 0);
                    event = 65;
                }
                (void)sub_800302C4(body, 151);
                w_u32(body + 36, (r_u32(body + 36) & 0xFFFC0FFF) | 0x00029000);
            }
            else
            {
                uint32_t descriptor = r_u32(r_u32(body + 596) + 1084);
                w_u8(descriptor + 46, 2);
                w_u8(descriptor + 47, 0);
                (void)sub_800958F0(body, 0);
                event = 64;
            }
            w_u16(rider + 966, 224);
            w_u16(rider + 964, 0);
            w_u8(rider + 946, 2);
            options |= 0x800 | (((3 * (sub_80043F00(0xF2000002) & 0xFF) >> 7) + 5) << 16);
            (void)sub_800C4550(event, body, options);
        }
        else if (kind == 3)
        {
            uint32_t object;
            uint32_t source;
            w_u32(specification + 8, r_u16(0x80053178));
            w_u32(specification + 36, (r_u16(0x8005317A) << 16) + r_u32(entry + 4));
            w_u16(specification + 2, (uint16_t)selector);
            w_u16(specification + 60, r_u16(current + 364));
            w_u16(specification + 64, (uint16_t)(3 - 2 * (entry_index & 1)));
            object = sub_8009AD48(specification, current);
            if (!object)
                goto next_entry;
            if (!group)
            {
                w_u32(object + 484, 0x00026666);
                w_u32(object + 480, 0);
                if (!r_u32(0x800CCC18))
                    w_u32(0x800CCC18, object);
                goto next_entry;
            }
            source = r_u32(object + 340);
            w_u32(object + 484, 0);
            w_u32(object + 480, 0);
            (void)sub_8002EAD8(xport_guest_ptr(r_u32(object + 340) + 20, 12), xport_guest_ptr(r_u32(object + 340) + 14, 6), r_u32(object + 348), xport_guest_ptr(object + 184, 12));
            (void)sub_8002EAD8(xport_guest_ptr(object + 184, 12), xport_guest_ptr(source + 2, 6), r_u32(entry), xport_guest_ptr(object + 184, 12));
            {
                const uint16_t rotation_axis[3] = {r_u16(source + 8), r_u16(source + 8 + 2), r_u16(source + 8 + 4)};
                uint16_t rotation_matrix[9];
                uint32_t rotation_index;
                (void)sub_8003FB34(rotation_axis, (uint32_t)angle, rotation_matrix);
                for (rotation_index = 0; rotation_index < 9; ++rotation_index)
                    w_u16(matrix + 2 * rotation_index, rotation_matrix[rotation_index]);
            }
            (void)sub_8003FA40(xport_guest_cptr(object + 432, 18), xport_guest_cptr(matrix, 18), xport_guest_ptr(object + 432, 18));
            w_u16(object + 450, r_u16(object + 444));
            w_u16(object + 452, r_u16(object + 446));
            w_u16(object + 454, r_u16(object + 448));
            race_leaf_heading(object);
        }
        else if (kind == 4)
        {
            uint32_t segment;
            uint32_t source;
            (void)sub_8001E100(specification, 0, 64);
            w_u32(specification + 8, r_u16(0x80053178));
            w_u32(specification + 36, (r_u16(0x8005317A) << 16) + r_u32(entry + 4));
            w_u16(specification + 2, (uint16_t)selector);
            w_u16(specification + 12, 3);
            w_u16(specification + 60, r_u16(current + 364));
            w_u16(specification + 62, (uint16_t)selector);
            w_u32(position, r_u32(specification + 8));
            w_u32(position + 4, (uint32_t)(int32_t)r_s16(specification + 60));
            w_u32(position + 8, r_u32(specification + 36));
            segment = sub_80039DFC(0, ((position) ? (const uint32_t[3]){r_u32(position), 0, (uint32_t)r_u16((position) + 10) << 16} : NULL));
            uint32_t geometry_result = 0;
            if (segment)
            {
                const uint32_t route_position[3] = {r_u32(position), r_u32(position + 4), r_u32(position + 8)};
                uint32_t route_geometry[8] = {r_u32(geometry), r_u32(geometry + 4), r_u32(geometry + 8), r_u32(geometry + 12), r_u32(geometry + 16), r_u32(geometry + 20), r_u32(geometry + 24), r_u32(geometry + 28)};
                geometry_result = sub_8003A700(segment, route_position, route_geometry);
                w_u32(geometry, route_geometry[0]);
                w_u32(geometry + 4, route_geometry[1]);
                w_u32(geometry + 8, route_geometry[2]);
                w_u32(geometry + 12, route_geometry[3]);
                w_u32(geometry + 16, route_geometry[4]);
                w_u32(geometry + 20, route_geometry[5]);
                w_u32(geometry + 24, route_geometry[6]);
                w_u32(geometry + 28, route_geometry[7]);
            }
            if (segment && geometry_result)
            {
                source = r_u32(geometry + 12);
                (void)sub_8002EAD8(xport_guest_ptr(source + 20, 12), xport_guest_ptr(source + 14, 6), r_u32(geometry + 20), xport_guest_ptr(specification + 20, 12));
                (void)sub_8002EAD8(xport_guest_ptr(specification + 20, 12), xport_guest_ptr(source + 2, 6), r_u32(entry), xport_guest_ptr(specification + 20, 12));
                {
                    const uint16_t rotation_axis[3] = {r_u16(source + 8), r_u16(source + 8 + 2), r_u16(source + 8 + 4)};
                    uint16_t rotation_matrix[9];
                    uint32_t rotation_index;
                    (void)sub_8003FB34(rotation_axis, (uint32_t)angle, rotation_matrix);
                    for (rotation_index = 0; rotation_index < 9; ++rotation_index)
                        w_u16(matrix + 2 * rotation_index, rotation_matrix[rotation_index]);
                }
                (void)sub_8002EFF4(xport_guest_cptr(source + 14, 6), xport_guest_cptr(matrix, 18), xport_guest_ptr(vector, 6));
                w_u16(specification + 14, r_u16(vector));
                w_u16(specification + 16, r_u16(vector + 2));
                w_u16(specification + 18, r_u16(vector + 4));
                (void)sub_800A2630(specification, current);
            }
        }
    next_entry:
        entry_offset += 12;
    }
    return 0;
}

uint32_t sub_800C8D4C(void)
{
    FUNCTION_MARKER(0x800C8D4C, "RASHCDG.BIN");
    return sub_800C8D5C();
}

uint32_t sub_800C8D5C(void)
{
    uint32_t race = r_u32(0x8005B2F8);
    uint32_t current = r_u32(0x8005B38C);
    uint32_t descriptor = r_u32(current + 1084);
    uint32_t event = 0x1F8003D0;
    uint32_t active = 0;
    int32_t offset = (int32_t)r_s8(0x8005ADF0) << 16;
    FUNCTION_MARKER(0x800C8D5C, "RASHCDG.BIN");
    switch (r_u8(race + 57))
    {
        case 1:
        {
            uint32_t time = r_u32(race + 16);
            w_u32(current + 704, 0);
            w_u32(current + 564, r_u32(current + 564) & 0xFFFFFDFF);
            active = ((int32_t)(uint32_t)(r_u32(0x8005ACC8))) < ((int32_t)(uint32_t)(time));
            if (active)
            {
                w_u32(descriptor + 40, time);
                w_u8(descriptor + 39, 0xFA);
            }
            (void)sub_800C9420(active ^ 1);
            if (active)
            {
                uint32_t player = 0x800CD898 + 1132 * r_u16(current + 172);
                uint32_t leader = r_u32(0x8005B330);
                w_u32(player + 776, 6);
                w_u32(player + 752, leader + 184);
                w_u32(player + 756, leader + 320);
                w_u32(player + 552, r_u32(player + 552) | 0x12);
                (void)sub_8008A998(player, 12);
                w_u32(current + 560, r_u32(current + 560) | 0x20000000);
            }
            return active;
        }
        case 2:
        {
            uint32_t contact;
            uint32_t state;
            w_u32(current + 704, 0);
            w_u32(current + 924, 0);
            w_u32(current + 560, r_u32(current + 560) | 0x28000000);
            w_u32(current + 564, r_u32(current + 564) & 0xFFFFFDFF);
            contact = current + 8 * (uint32_t)(int32_t)(r_s8(current + 946) - 1);
            state = r_u16(contact + 956);
            if (!state)
                return 0;
            if (state == 18)
                return 1;
            (void)sub_800BCD10(current);
            w_u16(event, 2);
            w_u16(event + 2, 224);
            (void)sub_800BCA68(xport_guest_ptr(event, 8), 0, current);
            w_u8(descriptor + 69, 64);
            (void)sub_8002EAD8(xport_guest_ptr(r_u32(current + 340) + 20, 12), xport_guest_ptr(r_u32(current + 340) + 14, 6), r_u32(current + 348), xport_guest_ptr(0x800CE5A8, 12));
            {
                uint32_t player = 0x800CD898 + 1132 * r_u16(current + 172);
                uint32_t linked_body = r_u32(r_u32(current + 856) + 852);
                w_u32(player + 776, 3);
                w_u32(player + 752, linked_body + 184);
                w_u32(player + 756, linked_body + 320);
                w_u32(player + 552, r_u32(player + 552) | 0x12);
                (void)sub_8008A998(player, 12);
            }
            return 0;
        }
        case 3:
        {
            int32_t direction = r_u16(0x80053186) < r_u16(0x80053182) ? -1 : 1;
            uint32_t kind = r_u8(descriptor + 1) & 15;
            uint32_t count_pointer = r_u32(0x800CE4DC);
            int32_t count = ((int32_t)(uint32_t)(r_u32(count_pointer)));
            uint32_t actor = r_u32(0x800CE4D0);
            uint32_t stride = r_u32(0x800CE4D4);
            w_u32(current + 560, r_u32(current + 560) & 0xD7FFFFFF);
            w_u8(descriptor + 69, r_u8(0x8005ADF1));
            w_u32(0x8005ACC0, 1);
            w_u32(0x8005B2E8, (uint32_t)direction);
            if (direction < 0)
                offset = -offset;
            while (count >= 0)
            {
                uint32_t actor_descriptor = r_u32(actor + 1084);
                if ((uint32_t)(r_u8(actor_descriptor + 1) & 15) == kind && actor != current)
                {
                    uint32_t secondary;
                    uint32_t animation;
                    w_u32(actor + 360, r_u16(0x80053180));
                    w_u32(actor + 364, (uint32_t)direction);
                    w_u32(actor + 924, 0);
                    w_u32(actor + 480, 0);
                    w_u32(actor + 368, (r_u16(0x80053182) << 16) + (uint32_t)offset);
                    w_u8(actor_descriptor + 69, r_u8(0x8005ADF1));
                    w_u32(actor + 704, 0);
                    w_u32(actor + 564, r_u32(actor + 564) & 0xFFFFFDFF);
                    offset += direction < 0 ? -393216 : 393216;
                    (void)sub_800903F4(actor, 1);
                    secondary = r_u32(actor + 852);
                    w_u16(actor + 956, 1);
                    animation = r_u8(0x80054114 + r_u32(secondary + 180));
                    (void)sub_800302C4(secondary, animation);
                    w_u32(secondary + 36, (r_u32(secondary + 36) & 0xFFFC0FFF) | ((kind ? 4 : 8) << 12));
                }
                --count;
                actor += stride;
            }
            return 0;
        }
        case 4:
        {
            uint32_t count_pointer = r_u32(0x800CE4DC);
            int32_t count = ((int32_t)(uint32_t)(r_u32(count_pointer)));
            uint32_t actor = r_u32(0x800CE4D0);
            uint32_t stride = r_u32(0x800CE4D4);
            uint32_t kind = r_u8(descriptor + 1) & 15;
            w_u32(0x8005ACC0, 0);
            w_u32(0x8005ACC4, 0);
            w_u32(current + 560, r_u32(current + 560) | 0x28000000);
            while (count >= 0)
            {
                uint32_t actor_descriptor = r_u32(actor + 1084);
                if ((uint32_t)(r_u8(actor_descriptor + 1) & 15) != kind)
                {
                    (void)sub_800BCD10(actor);
                    w_u16(event, 2);
                    w_u16(event + 2, 224);
                    (void)sub_800BCA68(xport_guest_ptr(event, 8), 1, actor);
                }
                --count;
                actor += stride;
            }
            (void)sub_8008A998(0x800CD898 + 1132 * r_u16(current + 172), 10);
            return 0;
        }
        case 5:
            w_u32(descriptor + 40, r_u32(race + 16));
            w_u8(descriptor + 39, 0xF9);
            w_u32(0x800CDAC0, r_u32(0x800CDAC0) | 0x14);
            return 0;
        default:
            return 0;
    }
}

static uint32_t race_leaf_checkpoint_progress(uint32_t actor, uint32_t secondary)
{
    uint32_t vector = 0x1F8000E0;
    uint32_t output = 0x1F8000F0;
    if (r_u32(secondary + 604) >= 3)
        return r_u32(secondary + 368);
    if (!r_s16(actor + 320))
        return r_u32(actor + 368);
    (void)sub_8002EAD8(xport_guest_ptr(actor + 504, 12), xport_guest_ptr(actor + 528, 6), r_u32(actor + 308), xport_guest_ptr(vector, 12));
    sub_80036800(vector, r_u32(actor + 340), NULL, output ? xport_guest_ptr(output, 4) : NULL);
    return r_u32(r_u32(actor + 340) + 40) + r_u32(output);
}

uint32_t sub_800B9958(uint32_t actor)
{
    uint32_t scalar_required_218;
    uint32_t scalar_path_219;
    uint32_t scalar_required_219;

    int32_t scalar_sign_121;
    int32_t scalar_sign_122;
    int32_t scalar_sign_123;

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
    race = r_u32(0x8005B2F8);
    descriptor = r_u32(actor + 1084);
    secondary = r_u32(actor + 852);
    if (r_u32(descriptor + 40))
    {
        if (r_u16(actor + 172) < r_u32(race + 48) && !(r_u32(actor + 560) & 0x08000000))
            w_u32(actor + 560, r_u32(actor + 560) | 0x08000000);
        return 1;
    }
    if (r_u16(actor + 172) < r_u32(race + 48) && r_u8(race + 4) != 33)
    {
        uint32_t phase = r_u8(race + 4);
        uint32_t time = r_u32(race + 16);
        uint32_t mode = r_u32(0x8005B220);
        if (mode && ((int32_t)(uint32_t)(mode)) < 3 && ((int32_t)(uint32_t)(time)) > 108000)
            finished = 1;
        else if ((phase == 36 && ((int32_t)(uint32_t)(r_u32(0x8005ACC8))) < ((int32_t)(uint32_t)(time))) || ((phase & 1) && ((int32_t)(uint32_t)(r_u32(0x8005ACC8) - (time - r_u32(0x8005ACD0)))) <= 0))
        {
            finished = 1;
            w_u8(descriptor + 39, 0xFA);
        }
        else if (phase & 4)
        {
            if (phase == 44)
                finished = ((int32_t)(uint32_t)(time)) > 216000;
            else
                finished = ((int32_t)(uint32_t)(time)) > 108000;
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
            if (r_u8(race + 4) == 44 && r_u16(actor + 172) < r_u32(race + 48))
            {
                if (r_u16(schedule) == r_u32(actor + 360))
                    stage_match = 2;
                if (!stage && ((int32_t)(uint32_t)(r_u32(0x8005ACC8))) < ((int32_t)(uint32_t)(r_u32(race + 16))) && !(r_u32(actor + 560) & 0x08000000))
                {
                    int32_t difference = (int16_t)r_u16(actor + 370) - (int32_t)r_u16(schedule + 2);
                    if (stage_match && ((int32_t)(uint32_t)(((scalar_sign_121 = ((int32_t)(difference)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_121 + ((int32_t)(difference))) ^ scalar_sign_121))))))) < 400)
                        w_u32(actor + 560, r_u32(actor + 560) | 0x28000000);
                    else
                    {
                        w_u8(descriptor + 39, 0xFA);
                        finished = 1;
                    }
                }
            }
            if (r_u32(secondary + 604) >= 3)
            {
                route = secondary + 360;
                path = r_u32(secondary + 428);
            }
            else
            {
                route = actor + 360;
                path = r_u32(actor + 428);
            }
            crossed = 0;
            if (r_u32(route) == r_u32(r_u32(0x800D6188)) && path)
                crossed = (r_u16(path + 118) >> (r_u16(actor + 172) & 31)) & 1;
            crossed |= stage_match;
            if (crossed)
            {
                uint32_t checkpoint = r_u32(0x800D6188);
                progress = race_leaf_checkpoint_progress(actor, secondary);
                if (crossed & 1)
                {
                    uint32_t delta = progress - r_u32(checkpoint + 4);
                    last_delta = ((int32_t)(uint32_t)(delta));
                    finished = ((scalar_required_218 = r_u32(((uint32_t)(checkpoint)) + 12)), (((uint32_t)(((!((uint32_t)(path)) || r_u32(((uint32_t)(path))) != scalar_required_218) && scalar_required_218 != 0xFFFFFFFF) ? (0) : (((int32_t)(uint32_t)(r_u32(((uint32_t)(checkpoint)) + 8) ^ ((uint32_t)(delta)))) >= 0)))));
                    if (r_u8(secondary + 572) & 0x10)
                    {
                        uint32_t linked_body = r_u32(r_u32(actor + 856) + 852);
                        uint32_t state = r_u32(linked_body + 604);
                        if (state - 3 < 2 && !(r_u32(linked_body + 360) >> 16) && (uint16_t)r_u32(linked_body + 360) == r_u32(checkpoint))
                        {
                            uint32_t linked_delta = r_u32(linked_body + 368) - r_u32(checkpoint + 4);
                            finished |= ((scalar_path_219 = (r_u32(linked_body + 428))), (scalar_required_219 = r_u32(((uint32_t)(checkpoint)) + 12)), (((uint32_t)(((!scalar_path_219 || r_u32(scalar_path_219) != scalar_required_219) && scalar_required_219 != 0xFFFFFFFF) ? (0) : (((int32_t)(uint32_t)(r_u32(((uint32_t)(checkpoint)) + 8) ^ ((uint32_t)(linked_delta)))) >= 0)))));
                        }
                    }
                }
                if (crossed & 2)
                {
                    uint32_t direction = r_u32(actor + 364);
                    uint32_t delta = progress - (r_u16(schedule + 2) << 16);
                    last_delta = ((int32_t)(uint32_t)(delta));
                    if (((int32_t)(uint32_t)(direction ^ r_u32(0x8005B2E8))) >= 0 && ((int32_t)(uint32_t)(direction ^ delta)) >= 0)
                    {
                        xport_update_u8(race + 57, XPORT_MEMORY_UPDATE_ADD, 1);
                        finished = sub_800C8D4C();
                    }
                    if (r_u8(race + 57) == 1)
                        sub_800C92F8();
                }
            }
        }
        else if (r_s16(actor + 320))
        {
            uint32_t track = r_u32(actor + 340);
            int16_t piece = r_s16(track);
            if ((!piece || piece == r_s16(r_u32(actor + 336) + 10) - 1) && sub_800394F0(xport_guest_ptr(actor + 328, 32), r_u32(actor + 364)))
                finished = 1;
        }
    }
    if (!finished)
        return 0;
    if (r_u16(actor + 172) < r_u32(race + 48))
    {
        uint32_t phase = r_u8(race + 4);
        uint32_t status = r_u8(descriptor + 39);
        if (!(r_u32(actor + 560) & 0x08000000))
        {
            w_u32(actor + 560, r_u32(actor + 560) | 0x08000000);
            w_u32(0x8005B230, 0);
        }
        if (phase == 36 && status < 0xF7 && ((int32_t)(uint32_t)(r_u32(0x8005ACC8))) >= ((int32_t)(uint32_t)(r_u32(race + 16))))
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
                uint32_t time = r_u32(race + 16);
                w_u32(0x800D9C4C, time);
                if ((phase == 44 && ((int32_t)(uint32_t)(time)) > 216000) || (phase != 44 && ((int32_t)(uint32_t)(time)) > 108000))
                    status = 0xFA;
            }
        }
        w_u8(descriptor + 39, (uint8_t)status);
        if (phase == 17 && ((r_u8(descriptor + 1) & 15) != 2))
        {
            uint32_t other = r_u32(0x8005B268 + 4 * (r_u16(actor + 172) == 0));
            uint32_t other_descriptor = r_u32(other + 1084);
            if (!r_u32(other_descriptor + 40) && r_u8(other_descriptor + 39) < 0xF8)
                w_u32(other_descriptor + 40, r_u32(race + 16));
        }
    }
    if (r_u8(descriptor + 39) < 0xF7)
        w_u8(descriptor + 39, (uint8_t)sub_800138E8(actor, 1));
    w_u32(descriptor + 40, r_u32(race + 16));
    (void)sub_800BC7CC(actor);
    (void)(uint32_t)sub_8003F680(r_u16(actor + 172), 1);
    if (actor == r_u32(0x8005B21C) && !(r_u8(race + 4) & 1))
    {
        uint32_t current = r_u32(0x8005B38C);
        uint32_t current_descriptor = r_u32(current + 1084);
        uint32_t status = r_u8(descriptor + 39);
        if (r_u32(current_descriptor + 40) == r_u32(race + 16) && status < 0xF7)
        {
            uint32_t current_status = r_u8(current_descriptor + 39);
            if (current_status < 0xF7)
            {
                int32_t current_delta = ((int32_t)(uint32_t)(r_u32(current + 368) - r_u32(r_u32(0x800D6188) + 4)));
                if (((scalar_sign_122 = ((int32_t)(last_delta)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_122 + ((int32_t)(last_delta))) ^ scalar_sign_122))))) < ((scalar_sign_123 = ((int32_t)(current_delta)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_123 + ((int32_t)(current_delta))) ^ scalar_sign_123))))))
                {
                    w_u8(descriptor + 39, (uint8_t)current_status);
                    w_u8(current_descriptor + 39, (uint8_t)status);
                    w_u32(0x800D5DA8 + 16 * (current_status - 1), r_u16(actor + 172));
                    w_u32(0x800D5DA8 + 16 * (status - 1), r_u16(current + 172));
                }
            }
        }
    }
    if (r_u32(race + 48) == 2 && r_s16(actor + 320) && r_u32(secondary + 604) < 2 && r_u16(actor + 172) >= 2)
    {
        uint32_t first = r_u32(actor + 48);
        uint32_t second = r_u32(actor + 44);
        uint32_t minimum = ((int32_t)(uint32_t)(second)) < ((int32_t)(uint32_t)(first)) ? second : first;
        if (((int32_t)(uint32_t)(minimum << 10)) >= 3201)
        {
            w_u16(actor + 320, 0);
            w_u16(secondary + 320, 0);
        }
    }
    return 1;
}

uint32_t sub_800B9794(uint32_t inactive_mask)
{
    int32_t count = ((int32_t)(uint32_t)(r_u32(0x8005B1F8)));
    uint32_t actor = r_u32(0x8005B3A0);
    int32_t index;
    FUNCTION_MARKER(0x800B9794, "RASHCDG.BIN");
    if (count <= 0)
        return 0x80060000;
    for (index = 0; index < count; ++index, actor += 1096)
    {
        uint32_t secondary = r_u32(actor + 852);
        uint32_t descriptor = r_u32(actor + 1084);
        uint32_t mark = 0;
        uint32_t status;
        if (r_u16(actor + 172) >= r_u32(r_u32(0x8005B2F8) + 48) && ((r_u8(descriptor + 1) & 15) == 2) && !(r_u8(actor + 928) & 0x10))
        {
            mark = 1;
        }
        else
        {
            w_u32(secondary + 552, r_u32(secondary + 552) & 0xFDFFFFFF);
            w_u32(actor + 560, r_u32(actor + 560) & 0xFFFBFFFF);
            xport_update_u8(actor + 928, XPORT_MEMORY_UPDATE_AND, 0xBF);
            if (r_u32(descriptor + 40) || sub_800B9958(actor))
            {
                status = r_u8(descriptor + 39);
                if (status < 0xF7 || status == 0xFA || status == 0xFE || status == 0xFF)
                    w_u32(actor + 560, r_u32(actor + 560) | 0x08000000);
                if ((uint8_t)(status + 2) < 2)
                    mark = 1;
            }
            if (!mark && !r_s16(actor + 320) && (r_u32(secondary + 604) < 3 || !r_s16(secondary + 320)))
                mark = 1;
        }
        if (mark)
            w_u32(inactive_mask, r_u32(inactive_mask) | (1u << (index & 31)));
    }
    return 0;
}

static void race_leaf_track_pose(uint32_t actor, uint32_t body)
{
    uint32_t track = r_u32(body + 340);
    uint32_t index;
    w_u32(actor + 504, r_u32(track + 20));
    w_u32(actor + 508, r_u32(track + 24));
    w_u32(actor + 512, r_u32(track + 28));
    for (index = 0; index < 3; ++index)
    {
        w_u32(actor + 184 + 4 * index, r_u32(actor + 504 + 4 * index));
        w_u16(actor + 528 + 2 * index, r_u16(track + 14 + 2 * index));
        w_u16(actor + 522 + 2 * index, (uint16_t)-r_s16(track + 8 + 2 * index));
        w_u16(actor + 516 + 2 * index, r_u16(track + 2 + 2 * index));
    }
    (void)sub_8001E0B4(xport_guest_ptr(actor + 328, 32), xport_guest_ptr(body + 328, 1), 32);
    w_u32(actor + 428, r_u32(body + 428));
    sub_8003AF9C(actor + 172, 1, 0);
    (void)sub_80096564(actor);
    for (index = 0; index < 3; ++index)
    {
        w_u16(actor + 444 + 2 * index, r_u16(actor + 528 + 2 * index));
        w_u16(actor + 432 + 2 * index, r_u16(actor + 522 + 2 * index));
        w_u16(actor + 438 + 2 * index, r_u16(actor + 516 + 2 * index));
    }
    if (r_s16(actor + 434) >= 0)
    {
        for (index = 0; index < 3; ++index)
            w_u16(actor + 438 + 2 * index, (uint16_t)-r_s16(actor + 438 + 2 * index));
    }
    else
    {
        for (index = 0; index < 3; ++index)
            w_u16(actor + 432 + 2 * index, (uint16_t)-r_s16(actor + 432 + 2 * index));
    }
    for (index = 0; index < 3; ++index)
        w_u16(actor + 450 + 2 * index, r_u16(actor + 528 + 2 * index));
    {
        const uint16_t route_direction[3] = {r_u16(actor + 450), r_u16(actor + 450 + 2), r_u16(actor + 450 + 4)};
        const uint32_t route_record[8] = {r_u32(actor + 328), r_u32(actor + 328 + 4), r_u32(actor + 328 + 8), r_u32(actor + 328 + 12), r_u32(actor + 328 + 16), r_u32(actor + 328 + 20), r_u32(actor + 328 + 24), r_u32(actor + 328 + 28)};
        uint32_t route_position[3] = {r_u32(actor + 360), r_u32(actor + 360 + 4), r_u32(actor + 360 + 8)};
        uint32_t route_result = sub_8003662C(route_direction, route_record, route_position);
        w_u32(actor + 360, route_position[0]);
        w_u32(actor + 360 + 4, route_position[1]);
        w_u32(actor + 360 + 8, route_position[2]);
        (void)route_result;
    }
    w_u32(actor + 456, 0);
    w_u32(actor + 460, 0);
    w_u32(actor + 464, 0);
    for (index = 0; index < 3; ++index)
        w_u16(actor + 814 + 2 * index, r_u16(actor + 516 + 2 * index));
    (void)sub_8002090C(actor);
    w_u32(actor + 568, r_u32(actor + 568) | ((r_u32(actor + 568) & 0x100) ? 0x100 : 0x900));
    w_u32(actor + 652, r_s16(actor + 434) < 0 ? 0xFFFE6DE1 : 102943);
    w_u32(actor + 492, r_u32(body + 492));
    w_u32(actor + 496, r_u32(body + 496));
    (void)sub_8001E0B4(xport_guest_ptr(actor + 372, 56), xport_guest_ptr(body + 372, 1), 56);
    w_u32(actor + 324, r_u32(body + 324));
    w_u32(actor + 344, 0);
    w_u32(actor + 348, 0);
    w_u16(actor + 392, 4);
    w_u16(actor + 394, 1);
    w_u32(actor + 388, r_u32(actor + 388) & 0xFFFFFFFE);
    (void)sub_8003A9D8(actor);
}

static uint32_t race_leaf_reset_body(uint32_t actor, uint32_t body)
{
    uint32_t track = r_u32(body + 340);
    uint32_t scale = r_u32(body + 304) + 2 * r_u32(actor + 304);
    uint32_t index;
    w_u32(actor + 480, 0);
    w_u32(body + 480, 0);
    w_u32(body + 488, 0);
    w_u32(body + 348, 0);
    w_u32(body + 344, scale);
    (void)sub_8002EAD8(xport_guest_ptr(track + 20, 12), xport_guest_ptr(track + 2, 6), scale, xport_guest_ptr(body + 184, 12));
    w_u32(body + 508, 0);
    for (index = 0; index < 3; ++index)
    {
        w_u16(body + 522 + 2 * index, (uint16_t)-r_s16(track + 8 + 2 * index));
        w_u16(body + 516 + 2 * index, r_u16(track + 14 + 2 * index));
        w_u16(body + 528 + 2 * index, (uint16_t)-r_s16(track + 2 + 2 * index));
        w_u16(body + 432 + 2 * index, r_u16(body + 522 + 2 * index));
        w_u16(body + 438 + 2 * index, r_u16(body + 516 + 2 * index));
        w_u16(body + 444 + 2 * index, r_u16(body + 528 + 2 * index));
        w_u16(body + 450 + 2 * index, r_u16(body + 528 + 2 * index));
    }
    w_u32(body + 456, 0);
    w_u32(body + 460, 0);
    w_u32(body + 464, 0);
    sub_8003AF9C(body + 172, 1, 0);
    {
        const uint16_t route_direction[3] = {r_u16(body + 450), r_u16(body + 450 + 2), r_u16(body + 450 + 4)};
        const uint32_t route_record[8] = {r_u32(body + 328), r_u32(body + 328 + 4), r_u32(body + 328 + 8), r_u32(body + 328 + 12), r_u32(body + 328 + 16), r_u32(body + 328 + 20), r_u32(body + 328 + 24), r_u32(body + 328 + 28)};
        uint32_t route_position[3] = {r_u32(body + 360), r_u32(body + 360 + 4), r_u32(body + 360 + 8)};
        uint32_t route_result = sub_8003662C(route_direction, route_record, route_position);
        w_u32(body + 360, route_position[0]);
        w_u32(body + 360 + 4, route_position[1]);
        w_u32(body + 360 + 8, route_position[2]);
        (void)route_result;
    }
    w_u16(body + 392, 4);
    w_u16(body + 394, 1);
    w_u32(body + 388, r_u32(body + 388) & 0xFFFFFFFE);
    return sub_8003A9D8(body);
}

uint32_t sub_80092E04(uint32_t actor, uint32_t delta)
{
    int32_t scalar_sign_124;
    int32_t scalar_sign_125;
    int32_t scalar_value_127;
    int32_t scalar_sign_127;
    int32_t scalar_value_128;
    int32_t scalar_sign_128;
    int32_t scalar_value_129;
    int32_t scalar_sign_129;

    uint32_t race = r_u32(0x8005B2F8);
    uint32_t body = r_u32(actor + 852);
    uint32_t source = actor;
    uint32_t linked_mode = (r_u8(body + 572) >> 5) & 1;
    uint32_t player = r_u16(actor + 172) < r_u32(race + 48);
    uint32_t blocked = 0;
    uint32_t far = 0;
    uint32_t distance;
    uint32_t result = 0;
    FUNCTION_MARKER(0x80092E04, "RASHCDG.BIN");
    if (linked_mode)
        source = r_u32(r_u32(body + 596) + 856);
    if (r_u8(race + 57) == 2)
    {
        if (player)
        {
            uint32_t descriptor = r_u32(source + 1084);
            if (!r_u32(descriptor + 40) && r_u8(descriptor + 39) < 0xF8)
                return sub_800CA05C(1);
            return r_u32(descriptor + 40);
        }
        else
        {
            uint32_t contact = source + 8 * (uint32_t)(int32_t)r_s8(source + 946) + 952;
            uint32_t timer = r_u16(contact);
            if (timer)
            {
                uint32_t step = (uint32_t)((int32_t)(300u * delta) >> 16);
                if (((int32_t)(uint32_t)(timer - step)) > 0)
                {
                    w_u16(contact, (uint16_t)(timer - step));
                    return step;
                }
                w_u16(contact, 0);
            }
            else
            {
                uint32_t current = r_u32(0x8005B38C);
                uint32_t descriptor = r_u32(current + 1084);
                if (r_u32(descriptor + 40) || r_u8(descriptor + 39) >= 0xF8)
                {
                    uint32_t current_body = r_u32(current + 852);
                    int32_t dx = ((int32_t)(uint32_t)(r_u32(body + 184) - r_u32(current_body + 184))) >> 16;
                    int32_t dz = ((int32_t)(uint32_t)(r_u32(body + 192) - r_u32(current_body + 192))) >> 16;
                    uint32_t major = ((scalar_sign_124 = ((int32_t)(dx)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_124 + ((int32_t)(dx))) ^ scalar_sign_124)))));
                    uint32_t minor = ((scalar_sign_125 = ((int32_t)(dz)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_125 + ((int32_t)(dz))) ^ scalar_sign_125)))));
                    if (major < minor)
                    {
                        uint32_t swap = major;
                        major = minor;
                        minor = swap;
                    }
                    if (major - (major >> 5) - (major >> 7) + ((minor + (minor >> 1)) >> 2) + ((minor + (minor >> 1)) >> 6) < 25)
                        w_u32(0x8005B228, 0x20000);
                }
            }
        }
    }
    if (!r_s16(body + 320))
        return 0;
    if ((r_u32(body + 552) & 0x1000) || (r_u32(source + 856) && r_u32(source + 1088) && !linked_mode) || (player && r_u32(0x8005B220)))
        blocked = 1;
    if (!blocked && player && r_s16(source + 320) && (r_u32(source + 388) & 1) && (r_u32(source + 568) & 0x100))
    {
        uint32_t track = r_u32(source + 340);
        int32_t dx = ((int32_t)(uint32_t)(r_u32(source + 184) - r_u32(track + 20)));
        int32_t dy = ((int32_t)(uint32_t)(r_u32(source + 188) - r_u32(track + 24)));
        int32_t dz = ((int32_t)(uint32_t)(r_u32(source + 192) - r_u32(track + 28)));
        int32_t dot = ((int32_t)((int32_t)(((int64_t)((int32_t)(dx)) * ((int32_t)r_s16(track + 8) * 16)) >> 16)));
        dot = (int32_t)((uint32_t)dot + (uint32_t)(((int64_t)dy * ((int32_t)r_s16(track + 10) * 16)) >> 16));
        dot = (int32_t)((uint32_t)dot + (uint32_t)(((int64_t)dz * ((int32_t)r_s16(track + 12) * 16)) >> 16));
        blocked = (int32_t)((0u - ((uint32_t)dot >> 31)) ^ ((uint32_t)dot + (0u - ((uint32_t)dot >> 31)))) > 0x10000;
    }
    distance = sub_8001FCB0((uint32_t)(((int32_t)(uint32_t)(r_u32(body + 184) - r_u32(source + 184))) >> 16), (uint32_t)(((int32_t)(uint32_t)(r_u32(body + 188) - r_u32(source + 188))) >> 16), (uint32_t)(((int32_t)(uint32_t)(r_u32(body + 192) - r_u32(source + 192))) >> 16));
    if (player)
        far = (int32_t)distance >= 201;
    if (!blocked && far && (r_u32(source + 560) & 0x08000000))
        blocked = 1;
    {
        uint32_t type = r_u16(body + 544);
        if (r_u16(0x800541D6 + 8 * type) != 8)
        {
            if (type - 69 >= 2)
                return 0;
            result = sub_8005BE58(r_u32(body + 540));
            if (!result || !blocked)
                return result;
        }
    }
    w_u32(body + 552, r_u32(body + 552) & 0xFFFFEFFF);
    if (blocked)
    {
        uint32_t linked;
        {
            uint32_t recovery_actor = source;
            uint32_t track = r_u32(body + 340);
            uint32_t index;
            uint32_t old_negative;
            uint32_t old_motion_flags;
            w_u16(recovery_actor + 320, 1);
            w_u32(body + 348, 0);
            w_u32(body + 344, 0);
            w_u32(recovery_actor + 348, 0);
            w_u32(recovery_actor + 344, 0);
            w_u32(recovery_actor + 504, r_u32(track + 20));
            w_u32(recovery_actor + 508, r_u32(track + 24));
            w_u32(recovery_actor + 512, r_u32(track + 28));
            for (index = 0; index < 3; ++index)
            {
                w_u32(recovery_actor + 184 + 4 * index, r_u32(recovery_actor + 504 + 4 * index));
                w_u16(recovery_actor + 528 + 2 * index, r_u16(track + 14 + 2 * index));
                w_u16(recovery_actor + 522 + 2 * index, (uint16_t)-r_s16(track + 8 + 2 * index));
                w_u16(recovery_actor + 516 + 2 * index, r_u16(track + 2 + 2 * index));
            }
            (void)sub_8001E0B4(xport_guest_ptr(recovery_actor + 328, 32), xport_guest_ptr(body + 328, 1), 32);
            w_u32(recovery_actor + 428, r_u32(body + 428));
            sub_8003AF9C(recovery_actor + 172, 1, 0);
            (void)sub_80096564(recovery_actor);
            old_negative = r_s16(recovery_actor + 434) < 0;
            for (index = 0; index < 3; ++index)
            {
                w_u16(recovery_actor + 444 + 2 * index, r_u16(recovery_actor + 528 + 2 * index));
                w_u16(recovery_actor + 432 + 2 * index, r_u16(recovery_actor + 522 + 2 * index));
                w_u16(recovery_actor + 438 + 2 * index, r_u16(recovery_actor + 516 + 2 * index));
            }
            if (!old_negative)
            {
                for (index = 0; index < 3; ++index)
                    w_u16(recovery_actor + 438 + 2 * index, (uint16_t)-r_s16(recovery_actor + 438 + 2 * index));
            }
            else
            {
                for (index = 0; index < 3; ++index)
                    w_u16(recovery_actor + 432 + 2 * index, (uint16_t)-r_s16(recovery_actor + 432 + 2 * index));
            }
            for (index = 0; index < 3; ++index)
                w_u16(recovery_actor + 450 + 2 * index, r_u16(recovery_actor + 528 + 2 * index));
            {
                const uint16_t route_direction[3] = {r_u16(recovery_actor + 450), r_u16(recovery_actor + 450 + 2), r_u16(recovery_actor + 450 + 4)};
                const uint32_t route_record[8] = {r_u32(recovery_actor + 328), r_u32(recovery_actor + 328 + 4), r_u32(recovery_actor + 328 + 8), r_u32(recovery_actor + 328 + 12), r_u32(recovery_actor + 328 + 16), r_u32(recovery_actor + 328 + 20), r_u32(recovery_actor + 328 + 24), r_u32(recovery_actor + 328 + 28)};
                uint32_t route_position[3] = {r_u32(recovery_actor + 360), r_u32(recovery_actor + 360 + 4), r_u32(recovery_actor + 360 + 8)};
                uint32_t route_result = sub_8003662C(route_direction, route_record, route_position);
                w_u32(recovery_actor + 360, route_position[0]);
                w_u32(recovery_actor + 360 + 4, route_position[1]);
                w_u32(recovery_actor + 360 + 8, route_position[2]);
                (void)route_result;
            }
            old_motion_flags = r_u32(recovery_actor + 568);
            w_u32(recovery_actor + 456, 0);
            w_u32(recovery_actor + 460, 0);
            w_u32(recovery_actor + 464, 0);
            for (index = 0; index < 3; ++index)
                w_u16(recovery_actor + 814 + 2 * index, r_u16(recovery_actor + 516 + 2 * index));
            (void)sub_8002090C(recovery_actor);
            w_u32(recovery_actor + 568, r_u32(recovery_actor + 568) | ((old_motion_flags & 0x100) ? 0x100 : 0x900));
            w_u32(recovery_actor + 652, old_negative ? 0xFFFE6DE1 : 102943);
            w_u32(recovery_actor + 492, r_u32(body + 492));
            w_u32(recovery_actor + 496, r_u32(body + 496));
            (void)sub_8001E0B4(xport_guest_ptr(recovery_actor + 372, 56), xport_guest_ptr(body + 372, 1), 56);
            w_u32(recovery_actor + 324, r_u32(body + 324));
            w_u32(recovery_actor + 344, 0);
            w_u32(recovery_actor + 348, 0);
            w_u16(recovery_actor + 392, 4);
            w_u16(recovery_actor + 394, 1);
            w_u32(recovery_actor + 388, r_u32(recovery_actor + 388) & 0xFFFFFFFE);
            (void)sub_8003A9D8(recovery_actor);
        }
        linked = r_u32(source + 856);
        if (linked && r_u32(source + 1088))
        {
            (void)sub_8001E0B4(xport_guest_ptr(linked + 328, 32), xport_guest_ptr(source + 328, 1), 32);
            w_u32(linked + 428, r_u32(source + 428));
            sub_8003AF9C(linked + 172, 1, 0);
            w_u32(linked + 360, r_u32(source + 360));
            w_u32(linked + 364, r_u32(source + 364));
            w_u32(linked + 368, r_u32(source + 368));
            w_u32(linked + 492, r_u32(source + 492));
            w_u32(linked + 496, r_u32(source + 496));
            (void)sub_8001E0B4(xport_guest_ptr(linked + 372, 56), xport_guest_ptr(source + 372, 1), 56);
            w_u32(linked + 344, 0);
            w_u32(linked + 348, 0);
            w_u16(linked + 392, 4);
            w_u16(linked + 394, 1);
            w_u16(linked + 320, 1);
            w_u32(linked + 324, r_u32(body + 324));
            w_u32(linked + 388, r_u32(linked + 388) & 0xFFFFFFFE);
            (void)sub_8002EAD8(xport_guest_ptr(source + 184, 12), xport_guest_ptr(source + 432, 6), r_u32(source + 304) + r_u32(linked + 304), xport_guest_ptr(linked + 184, 12));
        }
        if (r_u16(source + 172) < r_u32(r_u32(0x8005B2F8) + 48))
        {
            uint32_t descriptor = r_u32(source + 1084);
            if (!r_u8(descriptor + 37) || !r_u8(descriptor + 14))
            {
                (void)sub_80092C7C(source, 8);
                return race_leaf_reset_body(source, body);
            }
            else
            {
                uint32_t record = 0x800CD898 + 1132 * r_u16(source + 172);
                w_u32(record + 548, (r_u32(record + 548) & 0xFF807FF9) | 6);
                (void)sub_80086AF8(record);
            }
        }
        return sub_8009277C(source);
    }
    if (r_u8(r_u32(source + 1084) + 39) >= 0xF7)
    {
        result = r_u32(body + 552) & 0xFFEFFFE7;
        w_u32(body + 552, result);
        return result;
    }
    w_u32(body + 484, 0x7FFF0000);
    if (far || ((r_u32(source + 560) & 0x02000000) && (int32_t)distance < 3))
    {
        w_u32(body + 552, r_u32(body + 552) | 0x01000800);
    }
    else
    {
        uint32_t timer_active = (r_u32(body + 552) >> 20) & 1;
        if (timer_active)
        {
            uint32_t timer = r_u32(body + 600) - delta;
            w_u32(body + 600, timer);
            if (((int32_t)(uint32_t)(timer)) > 0)
                w_u32(body + 552, r_u32(body + 552) | 0x100);
            else
            {
                timer_active = 0;
                w_u32(body + 552, r_u32(body + 552) & 0xFFEFFFFF);
            }
        }
        if (!timer_active)
        {
            uint32_t axis = 0x1F800018;
            uint32_t direction = 0x1F800020;
            if (sub_80098A50(body, source, axis, direction))
            {
                uint32_t vector = 0x1F800028;
                uint32_t fixed_result = 0;
                uint32_t magnitude;
                w_u32(vector, r_u32(body + 580) - r_u32(body + 184));
                w_u32(vector + 4, r_u32(body + 584) - r_u32(body + 188));
                w_u32(vector + 8, r_u32(body + 588) - r_u32(body + 192));
                if (((scalar_value_127 = (((int32_t)(uint32_t)(r_u32(vector))))), (scalar_sign_127 = scalar_value_127 >> 31), (((int32_t)((uint32_t)scalar_value_127 + (uint32_t)scalar_sign_127) ^ scalar_sign_127))) <= 5931008 && ((scalar_value_128 = (((int32_t)(uint32_t)(r_u32(vector + 4))))), (scalar_sign_128 = scalar_value_128 >> 31), (((int32_t)((uint32_t)scalar_value_128 + (uint32_t)scalar_sign_128) ^ scalar_sign_128))) <= 5931008 && ((scalar_value_129 = (((int32_t)(uint32_t)(r_u32(vector + 8))))), (scalar_sign_129 = scalar_value_129 >> 31), (((int32_t)((uint32_t)scalar_value_129 + (uint32_t)scalar_sign_129) ^ scalar_sign_129))) <= 5931008)
                    magnitude = sub_8002F0F4(xport_guest_ptr(vector, 12));
                else
                {
                    magnitude = 0x7FFF0000;
                    fixed_result = 1;
                }
                if (linked_mode && (int32_t)magnitude <= 0x3FFFF)
                {
                    if (!r_u32(source + 480))
                        (void)sub_800903F4(source, 0);
                }
                else
                {
                    uint32_t flags = r_u32(body + 552);
                    uint32_t contact = (flags & 0x18) != 0;
                    if (!(flags & 0x800) || contact || (int32_t)magnitude <= 0x3FFFF)
                    {
                        w_u32(body + 552, flags & 0xFFFFF07F);
                        if (contact && (r_u32(source + 480) || r_u32(source + 488)))
                        {
                            w_u32(body + 552, r_u32(body + 552) & 0xFFFFFFE7);
                            contact = 0;
                        }
                        if (contact)
                        {
                            if (r_u32(body + 552) & 8)
                            {
                                uint32_t descriptor = r_u32(source + 1084);
                                if (player && (!r_u8(descriptor + 37) || !r_u8(descriptor + 14)))
                                    (void)sub_80092C7C(source, 8);
                                (void)sub_80098F2C(body, axis, direction, magnitude, fixed_result);
                            }
                            if (r_u32(body + 552) & 0x10)
                                (void)sub_80099710(body, axis, direction, delta);
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
    result = 0xFFEF0000u;
    if (r_u32(body + 552) & 0x800)
    {
        uint32_t id = r_u16(body + 172);
        w_u32(body + 552, r_u32(body + 552) & 0xFFEFFFE7);
        result = (id >> 5) == 1 && ((int32_t)(uint32_t)(id & 31)) < ((int32_t)(uint32_t)(r_u32(r_u32(0x8005B2F8) + 48)));
        if (result)
        {
            uint32_t owner = r_u32(body + 596);
            uint32_t record = 0x800CD898 + 1132 * r_u16(owner + 172);
            w_u32(record + 548, r_u32(record + 548) & 0xFFFFFF7F);
            result = record;
        }
    }
    return result;
}

uint32_t sub_800BA4CC(uint32_t delta, uint32_t skip_mask, uint32_t active_mask)
{
    int32_t count = ((int32_t)(uint32_t)(r_u32(0x8005B1F8)));
    uint32_t actor = r_u32(0x8005B3A0);
    int32_t index;
    FUNCTION_MARKER(0x800BA4CC, "RASHCDG.BIN");
    if (count <= 0)
        return 0x80060000;
    for (index = 0; index < count; ++index, actor += 1096)
    {
        uint32_t bit = 1u << (index & 31);
        if (!(skip_mask & bit))
        {
            uint32_t secondary = r_u32(actor + 852);
            uint32_t descriptor = r_u32(actor + 1084);
            uint32_t slot = (uint32_t)(int32_t)r_s8(actor + 946);
            uint32_t contact = actor + 948 + 8 * slot;
            uint32_t actor_id = r_u16(actor + 172);
            uint32_t players = r_u32(r_u32(0x8005B2F8) + 48);
            uint32_t state = r_u16(contact);
            uint16_t other_id = r_u16(contact + 2);
            if (actor_id >= players && other_id < players && state >= 14 && state < 17)
            {
                uint32_t address = 0x800CCAC0 + 2 * other_id;
                w_u16(address, r_u16(address) | (uint16_t)(1u << ((actor_id - 1) & 31)));
            }
            switch (state)
            {
                case 1:
                    if (!(r_u32(secondary + 552) & 0x40))
                    {
                        if (r_u16(contact + 4) || (actor_id < players && ((r_u8(descriptor + 1) & 15) == 2) && (r_u32(0x8005AD48) & 0x1F)))
                        {
                            uint32_t linked;
                            rrj_trace_dispatch_target(0x800BAA2Cu);
                            (void)sub_800BAA2C(actor, contact, delta);
                            rrj_trace_dispatch_target(0x800BA4CCu);
                            linked = r_u32(actor + 856);
                            if (linked && r_u16(contact) == 4)
                            {
                                uint32_t linked_slot = (uint32_t)(int32_t)r_s8(linked + 946);
                                rrj_trace_dispatch_target(0x800BAA2Cu);
                                (void)sub_800BAA2C(linked, linked + 948 + 8 * linked_slot, delta);
                                rrj_trace_dispatch_target(0x800BA4CCu);
                            }
                        }
                        else if (r_u32(secondary + 552) & 0x40)
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
                    if (!(active_mask & bit) && (r_u32(actor + 560) & 0x08000000))
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
                    (void)sub_800BBE00(actor, other_id);
                    break;
                case 16:
                    rrj_trace_dispatch_target(0x800C035Cu);
                    (void)sub_800C035C(actor, other_id, 0x801FFE8Cu);
                    break;
                case 17:
                    rrj_trace_dispatch_target(0x800BBEBCu);
                    (void)sub_800BBEBC(actor, other_id);
                    break;
                case 18:
                    rrj_trace_dispatch_target(0x80092E04u);
                    (void)sub_80092E04(actor, delta);
                    break;
                default:
                    break;
            }
            rrj_trace_dispatch_target(0x800BA4CCu);
            w_u16(contact + 6, r_u16(contact + 6) & 0x7FFF);
        }
    }
    return 0;
}

static int32_t race_leaf_signed_ratio(int32_t numerator, int32_t denominator)
{
    int32_t scalar_sign_130;
    int32_t scalar_sign_131;

    uint32_t quotient = sub_80010028(((scalar_sign_130 = ((int32_t)(numerator)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_130 + ((int32_t)(numerator))) ^ scalar_sign_130))))), ((scalar_sign_131 = ((int32_t)(denominator)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_131 + ((int32_t)(denominator))) ^ scalar_sign_131))))));
    return (numerator < 0) != (denominator < 0) ? ((int32_t)(uint32_t)(0u - quotient)) : ((int32_t)(uint32_t)(quotient));
}

uint32_t sub_800BD4D4(uint32_t actor)
{
    int32_t scalar_sign_132;
    int32_t scalar_sign_133;
    int32_t scalar_sign_134;
    int32_t scalar_sign_135;
    int32_t scalar_sign_136;
    int32_t scalar_sign_137;
    int32_t scalar_sign_138;
    int32_t scalar_value_139;
    int32_t scalar_sign_139;
    int32_t scalar_sign_140;
    int32_t scalar_sign_141;

    const uint32_t candidates = 0x1F800200;
    const uint32_t candidate_count = 0x1F800240;
    const uint32_t event = 0x801FFE88u;
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
    if (((int32_t)(uint32_t)(r_u32(actor + 480))) < 131)
    {
        if (r_u16(contact) == 3)
            sub_800BC8DC(actor);
        return 0;
    }
    range = ((int32_t)(uint32_t)(2u * (r_u32(actor + 480) + 0x141DDDu)));
    w_u32(candidate_count, 16);
    (void)sub_8008AE94(candidates, candidate_count, actor + 184, range, 73, r_u16(actor + 172));
    if (r_u32(candidate_count))
    {
        uint8_t actor_flags = r_u8(actor + 928) & 0xFE;
        w_u8(actor + 928, actor_flags);
        if (!(r_u32(actor + 388) & 1))
        {
            int32_t alignment = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(actor + 528, 6), xport_guest_ptr(r_u32(actor + 340) + 14, 6))));
            if (((scalar_sign_132 = ((int32_t)(alignment)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_132 + ((int32_t)(alignment))) ^ scalar_sign_132))))) > 53739)
                w_u8(actor + 928, actor_flags | 1);
        }
    }
    for (index = 0; index < r_u32(candidate_count); ++index)
    {
        uint16_t candidate = r_u16(candidates + 2 * index);
        int32_t distance = ((int32_t)(uint32_t)(sub_800BEBA4(candidate, actor)));
        if (distance < range)
        {
            range = distance;
            packed = candidate;
            group = candidate >> 5;
        }
    }
    if (packed == 224)
    {
        if (r_u16(contact) != 3)
            return 0;
        if (r_u8(actor + 928) & 1)
        {
            uint16_t previous = r_u16(contact + 2);
            if ((previous >> 5) == 3)
            {
                uint32_t previous_object = 0x800CF660 + 512 * (previous & 31);
                uint32_t valid = r_u16(previous_object + 320) != 0 && ((int32_t)(uint32_t)(r_u32(previous_object + 324) - (((int32_t)(uint32_t)(r_u32(previous_object + 308))) >> 4))) < ((int32_t)(uint32_t)(r_u32(actor + 324)));
                flags = valid << 5;
            }
        }
        if (!(flags & 0x20))
        {
            sub_800BC8DC(actor);
            return 0;
        }
        packed = r_u16(contact + 2);
        group = packed >> 5;
    }
    else if (r_u16(contact + 2) == packed && r_u16(contact) >= 5)
    {
        return 0;
    }
    switch (group)
    {
        case 0:
            object = r_u32(0x8005B3A0) + 1096 * (packed & 31);
            record = object + 172;
            relative = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(object + 528, 6), xport_guest_ptr(actor + 528, 6))));
            if (((scalar_sign_133 = ((int32_t)(relative)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_133 + ((int32_t)(relative))) ^ scalar_sign_133))))) >= 0x4000)
                relative = relative >= 0 ? ((int32_t)(uint32_t)(r_u32(actor + 480) - r_u32(object + 480))) : ((int32_t)(uint32_t)(r_u32(actor + 480) + r_u32(object + 480)));
            else
                relative = ((int32_t)(uint32_t)(r_u32(actor + 480)));
            break;
        case 3:
            object = 0x800CF660 + 512 * (packed & 31);
            record = object + 172;
            relative = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(object + 444, 6), xport_guest_ptr(actor + 528, 6))));
            if (((scalar_sign_134 = ((int32_t)(relative)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_134 + ((int32_t)(relative))) ^ scalar_sign_134))))) >= 0x4000)
                relative = relative >= 0 ? ((int32_t)(uint32_t)(r_u32(actor + 480) - r_u32(object + 480))) : ((int32_t)(uint32_t)(r_u32(actor + 480) + r_u32(object + 480)));
            else
                relative = ((int32_t)(uint32_t)(r_u32(actor + 480)));
            if (r_u32(actor + 360) == r_u32(object + 360) && !(r_u32(actor + 360) >> 16))
            {
                flags |= 2;
                if (r_u8(object + 510) & 2)
                    flags |= 0x40;
            }
            break;
        case 6:
            record = r_u32(0x800CD6C4) + 280 * (packed & 31);
            relative = ((int32_t)(uint32_t)(r_u32(actor + 480)));
            if (r_u32(record + 8) == 1)
                flags |= 0x10;
            break;
        default:
            return 0;
    }
    if ((r_u8(actor + 928) & 1) && r_u32(actor + 360) == r_u32(record + 188))
        separation = ((int32_t)(uint32_t)(16u * (r_u32(actor + 324) - r_u32(record + 152)) - r_u32(actor + 308) - r_u32(record + 136)));
    else
    {
        separation = ((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(record + 12, 12), xport_guest_ptr(actor + 528, 6), xport_guest_ptr(actor + 504, 12)) - r_u32(actor + 308)));
        if (!(flags & 2))
            separation = ((int32_t)(uint32_t)((uint32_t)separation - (uint32_t)sub_8001FC90(46333, ((int32_t)(uint32_t)(r_u32(record + 132) + r_u32(record + 136))))));
    }
    if (separation < ((int32_t)(uint32_t)(r_u32(actor + 308))) || ((int32_t)(uint32_t)(2u * (uint32_t)relative)) < separation)
    {
        if (r_u16(contact) == 3)
        {
            if (r_u16(contact + 2) != packed)
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
    if (!group && relative <= 0x13FFFF)
    {
        uint32_t other_contact;
        uint16_t kind = 7;
        int32_t lateral;
        if (r_u16(contact) >= 5)
        {
            if (r_u32(object + 360) == r_u32(actor + 360) && (!(r_u32(object + 360) >> 16) || r_u32(object + 336) == r_u32(actor + 336)))
            {
                lateral = ((int32_t)(uint32_t)(r_u32(object + 344) - r_u32(actor + 344)));
                if (((int32_t)(uint32_t)(r_u32(actor + 364))) < 0)
                    lateral = -lateral;
            }
            else
                lateral = ((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(object + 184, 12), xport_guest_ptr(actor + 432, 6), xport_guest_ptr(actor + 184, 12))));
            if (r_u32(object + 856) && lateral < 0)
                lateral = ((int32_t)(uint32_t)((uint32_t)lateral + r_u32(object + 304) + r_u32(r_u32(object + 856) + 304)));
            if (((scalar_sign_135 = ((int32_t)(lateral)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_135 + ((int32_t)(lateral))) ^ scalar_sign_135))))) > 98304 || separation > 655360)
                return 0;
        }
        other_contact = object + 948 + 8u * (uint32_t)(int32_t)r_s8(object + 946);
        if ((r_u8(r_u32(object + 1084) + 1) & 15) == (r_u8(r_u32(actor + 1084) + 1) & 15))
        {
            if (r_u16(other_contact) == 10 && r_u16(actor + 172) == 0)
                kind = 12;
            else if (r_u16(other_contact + 2) == r_u16(actor + 172))
                kind = 7;
            else if (4 * ((int32_t)(uint32_t)(r_u32(actor + 308))) < separation)
                kind = 5;
        }
        if (kind == 7)
        {
            if (!sub_800BBBB8(actor, object, separation))
                return 0;
        }
        else if (kind == 5 && !sub_800BB448(actor, object, separation))
            return 0;
        if (r_u16(contact) == 3)
            sub_800BC8DC(actor);
        w_u16(event, kind);
        w_u16(event + 2, packed);
        (void)sub_800BCA68(xport_guest_ptr(event, 8), 2, actor);
        if (kind == 7 && (uint16_t)(r_u16(other_contact) - 6) < 2 && sub_800BB448(actor, object, separation))
        {
            w_u16(event, 5);
            (void)sub_800BCA68(xport_guest_ptr(event, 8), 2, actor);
        }
        return 0;
    }
    if (r_u16(contact) == 3)
    {
        w_u16(contact + 2, (uint16_t)packed);
        if (!(r_u16(contact + 6) & 0x8000))
        {
            uint32_t state = r_u32(0x8005B2F8);
            uint32_t phase = 2180 * r_u32(state + 16);
            uint16_t stamp = r_u16(contact + 6);
            if ((int32_t)(phase - ((uint32_t)(stamp & 0x3FFF) << 16)) > 491520)
                w_u16(contact + 6, stamp | 0x8000 | (uint16_t)(((phase + 0x8000) >> 16) & 0x3FFF));
        }
    }
    else
    {
        w_u16(event, 3);
        w_u16(event + 2, (uint16_t)packed);
        (void)sub_800BCA68(xport_guest_ptr(event, 8), 2, actor);
        created = 1;
        contact = actor + 948 + 8u * (uint32_t)(int32_t)r_s8(actor + 946);
    }
    if (flags & 2)
    {
        int32_t delta = ((int32_t)(uint32_t)(r_u32(record + 172) - r_u32(actor + 344)));
        if (((int32_t)(uint32_t)(r_u32(actor + 364))) < 0)
            delta = -delta;
        edge = ((int32_t)(uint32_t)(r_u32(record + 132)));
        left = delta - edge;
        right = delta + edge;
    }
    else
    {
        int32_t heading_x = ((int32_t)(uint32_t)(r_u32(actor + 296)));
        int32_t heading_z = ((int32_t)(uint32_t)(0u - r_u32(actor + 300)));
        int32_t extra = 0;
        int32_t first_x;
        int32_t first_z;
        int32_t second_x;
        int32_t second_z;
        edge = ((int32_t)(uint32_t)(r_u32(record + 132)));
        if (flags & 0x10)
            edge = ((int32_t)(uint32_t)(r_u32(actor + 188) - r_u32(actor + 312))) >= ((int32_t)(uint32_t)(r_u32(record + 88))) ? ((int32_t)(uint32_t)(r_u32(record + 84))) : edge;
        else if (!(r_u16(record) >> 5) && r_u32(object + 856))
            extra = ((int32_t)(uint32_t)(2u * r_u32(r_u32(object + 856) + 304)));
        if (((int32_t)(uint32_t)((uint32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(actor + 296))), ((int32_t)(uint32_t)(r_u32(record + 124)))) + (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(actor + 300))), ((int32_t)(uint32_t)(r_u32(record + 128)))))) >= 0)
        {
            first_x = ((int32_t)(uint32_t)(r_u32(record + 24)));
            first_z = ((int32_t)(uint32_t)(r_u32(record + 32)));
            second_x = ((int32_t)(uint32_t)((uint32_t)first_x + (uint32_t)sub_8001FC90(2 * edge + extra, ((int32_t)(uint32_t)(r_u32(record + 124))))));
            second_z = ((int32_t)(uint32_t)((uint32_t)first_z - (uint32_t)sub_8001FC90(2 * edge + extra, ((int32_t)(uint32_t)(r_u32(record + 128))))));
        }
        else
        {
            first_x = ((int32_t)(uint32_t)(r_u32(record + 48) + (uint32_t)sub_8001FC90(extra, ((int32_t)(uint32_t)(r_u32(record + 124))))));
            first_z = ((int32_t)(uint32_t)(r_u32(record + 56) - (uint32_t)sub_8001FC90(extra, ((int32_t)(uint32_t)(r_u32(record + 128))))));
            second_x = ((int32_t)(uint32_t)(r_u32(record + 48) - (uint32_t)sub_8001FC90(2 * edge, ((int32_t)(uint32_t)(r_u32(record + 124))))));
            second_z = ((int32_t)(uint32_t)(r_u32(record + 56) + (uint32_t)sub_8001FC90(2 * edge, ((int32_t)(uint32_t)(r_u32(record + 128))))));
        }
        left = ((int32_t)(uint32_t)((uint32_t)sub_8001FC90(heading_x, ((int32_t)(uint32_t)((uint32_t)first_x - r_u32(actor + 184)))) + (uint32_t)sub_8001FC90(heading_z, ((int32_t)(uint32_t)((uint32_t)first_z - r_u32(actor + 192))))));
        right = ((int32_t)(uint32_t)((uint32_t)sub_8001FC90(heading_x, ((int32_t)(uint32_t)((uint32_t)second_x - r_u32(actor + 184)))) + (uint32_t)sub_8001FC90(heading_z, ((int32_t)(uint32_t)((uint32_t)second_z - r_u32(actor + 192))))));
    }
    {
        uint32_t sine_address = 0x8005624C + (((uint32_t)(163u * r_u32(actor + 652)) >> 12) & 0x3FFC);
        curve = ((int32_t)(uint32_t)((uint32_t)sub_8001FC90((int32_t)r_s16(sine_address) * 16, ((int32_t)(uint32_t)(r_u32(actor + 312))))));
        curve = ((int32_t)(uint32_t)(((scalar_sign_136 = ((int32_t)(curve)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_136 + ((int32_t)(curve))) ^ scalar_sign_136)))))));
    }
    if (((int32_t)(uint32_t)(r_u32(actor + 652))) < -1143)
        right = ((int32_t)(uint32_t)((uint32_t)right + (uint32_t)curve));
    else if (((int32_t)(uint32_t)(r_u32(actor + 652))) >= 1144)
        left = ((int32_t)(uint32_t)((uint32_t)left - (uint32_t)curve));
    {
        int32_t left_abs = ((int32_t)(uint32_t)(((scalar_sign_137 = ((int32_t)(left)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_137 + ((int32_t)(left))) ^ scalar_sign_137)))))));
        int32_t right_abs = ((int32_t)(uint32_t)(((scalar_sign_138 = ((int32_t)(right)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_138 + ((int32_t)(right))) ^ scalar_sign_138)))))));
        if ((left >= 0) != (right < 0))
        {
            int32_t nearest = left_abs < right_abs ? left_abs : right_abs;
            if (3 * ((int32_t)(uint32_t)(r_u32(actor + 304))) < nearest)
            {
                flags |= 0x20;
                if (!(flags & 2))
                    return created;
            }
        }
        if (left_abs < right_abs)
            flags |= 1;
        if (!(r_u16(contact + 6) & 0x8000))
            flags = (flags & ~1u) | ((r_u8(actor + 928) >> 2) & 1);
        if (flags & 2)
        {
            int32_t adjusted = curve + edge + ((int32_t)(uint32_t)(r_u32(actor + 304)));
            int32_t direction = ((int32_t)(uint32_t)(r_u32(actor + 652)));
            int32_t record_side = ((int32_t)(uint32_t)(r_u32(record + 172)));
            int32_t impulse = 0;
            if ((direction < -1143 && !(flags & 1)) || (direction >= 1144 && (flags & 1)))
            {
                flags |= 4;
                adjusted += curve;
            }
            if (adjusted < (int32_t)((scalar_value_139 = (((int32_t)(uint32_t)(r_u32(actor + 344) - (uint32_t)record_side)))), (scalar_sign_139 = scalar_value_139 >> 31), (((uint32_t)((uint32_t)((scalar_sign_139 + scalar_value_139) ^ scalar_sign_139))))))
                flags |= 0x20;
            if (record_side < 0)
                flags |= 8;
            if (r_s16(actor + ((flags & 8) ? 400 : 412) + 8) >= 2)
                impulse = race_leaf_signed_ratio((int32_t)((scalar_sign_140 = ((int32_t)(record_side)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_140 + ((int32_t)(record_side))) ^ scalar_sign_140))))), ((int32_t)(uint32_t)(r_u32(actor + 424))));
            impulse = (int32_t)(r_u32(actor + 424) * (uint32_t)(impulse >> 16));
            if (flags & 8)
                impulse = -impulse;
            if ((record_side >= 0) != ((((flags ^ 1) & 1) ^ (((int32_t)(uint32_t)(r_u32(actor + 364))) > 0)) != 0))
                impulse = ((int32_t)(uint32_t)((uint32_t)impulse + ((flags & 8) ? 0u - r_u32(actor + 424) : r_u32(actor + 424))));
            if (flags & 4)
            {
                int32_t signed_curve = ((((int32_t)(uint32_t)(r_u32(actor + 652))) >= 0) ^ (((int32_t)(uint32_t)(r_u32(actor + 364))) > 0)) ? curve : -curve;
                impulse = ((int32_t)(uint32_t)((uint32_t)impulse + 2u * (uint32_t)signed_curve));
            }
            impulse = ((int32_t)(uint32_t)((uint32_t)impulse - 32u * (uint32_t)(int32_t)r_s16(actor + 878)));
            if ((int16_t)(((int32_t)r_s16(r_u32(actor + 340) + 2) * r_s16(actor + 872)) >> 12) + (int16_t)(((int32_t)r_s16(r_u32(actor + 340) + 6) * r_s16(actor + 876)) >> 12) < 0)
                impulse = -impulse;
            w_u32(scalar, (uint32_t)impulse);
            sub_80093CAC(actor, 0, actor + 872, scalar, 0);
            w_u16(actor + 944, 1);
        }
        else
        {
            int32_t first = ((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(actor + 880, 12), xport_guest_ptr(actor + 516, 6), xport_guest_ptr(actor + 504, 12))));
            int32_t second = ((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(actor + 880, 12), xport_guest_ptr(actor + 528, 6), xport_guest_ptr(actor + 504, 12))));
            int32_t tangent;
            int32_t scale;
            if ((first < 1) == ((flags & 1) != 0))
                tangent = first;
            else
            {
                int32_t ratio = race_leaf_signed_ratio((int32_t)((scalar_sign_141 = ((int32_t)(first)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_141 + ((int32_t)(first))) ^ scalar_sign_141))))), second);
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
                scale = 2 * ((int32_t)(uint32_t)(r_u32(actor + 304)));
            else
                scale = ((int32_t)(uint32_t)((uint32_t)sub_8001FC90(separation / 4, ((int32_t)(uint32_t)(r_u32(actor + 304))))));
            if (flags & 1)
                scale = -left_abs - scale;
            else
                scale += right_abs;
            scale = race_leaf_signed_ratio(((int32_t)(uint32_t)((uint32_t)sub_8001FC90(scale, second))), separation);
            w_u32(vector, (uint32_t)((int32_t)sub_8001FC90(second, ((int32_t)(uint32_t)(r_u32(actor + 300)))) + (int32_t)sub_8001FC90(scale, ((int32_t)(uint32_t)(r_u32(actor + 296)))) + ((int32_t)(uint32_t)(r_u32(actor + 184)))));
            w_u32(vector + 4, r_u32(actor + 884));
            w_u32(vector + 8, (uint32_t)((int32_t)sub_8001FC90(second, ((int32_t)(uint32_t)(r_u32(actor + 296)))) + (int32_t)sub_8001FC90(scale, ((int32_t)(uint32_t)(0u - r_u32(actor + 300)))) + ((int32_t)(uint32_t)(r_u32(actor + 192)))));
            sub_80093CAC(actor, vector, 0, 0, 0);
        }
    }
    w_u8(actor + 928, (r_u8(actor + 928) & 0xFB) | (uint8_t)(4 * (flags & 1)));
    {
        uint32_t state = r_u32(0x8005B2F8);
        uint32_t mode = r_u32(state + 60);
        int32_t threshold = ((int32_t)(uint32_t)(r_u32(0x80053018 + 4 * mode)));
        int32_t ratio;
        if (flags & 0x40)
            threshold *= 2;
        if ((flags & 0x20) || separation >= threshold || separation <= 196608)
            return created;
        if ((r_u16(record) >> 5) == 3 && ((int32_t)(uint32_t)(r_u32(record + 140))) <= 104856 && (uint32_t)relative > 917504 && separation < threshold - 491520 * (3 - (int32_t)mode))
        {
            uint16_t count = r_u16(contact + 4);
            w_u32(actor + 560, r_u32(actor + 560) | 0x800);
            if ((uint32_t)(count - 1) < 300)
                w_u16(contact + 4, count + r_u16(state + 28));
            else
            {
                w_u16(contact + 4, 1);
                w_u32(actor + 560, r_u32(actor + 560) | 0x1000);
            }
            return created;
        }
        ratio = race_leaf_signed_ratio(separation - 196608, threshold - 196608);
        w_u32(actor + 924, (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(actor + 924))), ratio));
        w_u32(actor + 916, 0);
        w_u32(actor + 920, 0);
        w_u32(actor + 564, r_u32(actor + 564) & 0xFFF7FFFFu);
    }
    return created;
}

uint32_t sub_800BCD48(uint32_t delta, uint32_t skip_mask, uint32_t active_mask)
{
    int32_t scalar_value_142;
    int32_t scalar_sign_142;

    int32_t count = ((int32_t)(uint32_t)(r_u32(0x8005B1F8)));
    uint32_t actor = r_u32(0x8005B3A0);
    int32_t index;
    FUNCTION_MARKER(0x800BCD48, "RASHCDG.BIN");
    if (count <= 0)
        return 0x80060000;
    for (index = 0; index < count; ++index, actor += 1096)
    {
        uint32_t bit = 1u << (index & 31);
        uint32_t contact = actor + 948 + 8u * (uint32_t)(int32_t)r_s8(actor + 946);
        uint16_t kind = r_u16(contact);
        uint32_t flags;
        uint32_t created;
        int32_t timer;
        if (skip_mask & bit || !r_s16(actor + 320) || (r_u32(actor + 560) & 0x18000000) != 0x08000000 || (r_u8(r_u32(actor + 1084)) & 1) || kind < 3 || kind >= 18)
            continue;
        created = sub_800BD4D4(actor);
        flags = r_u32(actor + 564);
        if (!(flags & 0x200))
            continue;
        if (!created && ((scalar_value_142 = (((int32_t)(uint32_t)(r_u32(actor + 676))))), (scalar_sign_142 = scalar_value_142 >> 31), (((uint32_t)((uint32_t)((scalar_sign_142 + scalar_value_142) ^ scalar_sign_142))))) < 2130)
        {
            timer = active_mask & bit ? ((int32_t)(uint32_t)(r_u32(actor + 704) - delta)) : 49152;
            w_u32(actor + 704, (uint32_t)timer);
        }
        else
        {
            w_u32(actor + 564, flags & 0xFFFFFDFFu);
            w_u32(actor + 704, 0);
            continue;
        }
        if (timer < 0)
        {
            w_u32(actor + 704, 0);
            w_u32(actor + 564, r_u32(actor + 564) & 0xFFFFFDFFu);
        }
    }
    return 0;
}

uint32_t sub_800B9414(uint32_t delta)
{
    FUNCTION_MARKER(0x800B9414, "RASHCDG.BIN");
    return sub_800B941C(delta);
}

uint32_t sub_800B941C(uint32_t delta)
{
    const uint32_t inactive_out = 0x1F800270;
    const uint32_t active_out = 0x1F800274;
    const uint32_t marked_out = 0x1F800278;
    uint32_t race = r_u32(0x8005B2F8);
    uint32_t players = r_u32(race + 48);
    uint32_t finished_mask = 0;
    uint32_t index;
    FUNCTION_MARKER(0x800B941C, "RASHCDG.BIN");
    rrj_trace_dispatch_target(0x800B941Cu);
    w_u32(inactive_out, 0);
    w_u32(active_out, 0);
    w_u32(marked_out, 0);
    for (index = 0; index < players; ++index)
    {
        uint32_t actor = r_u32(0x8005B268 + 4 * index);
        uint32_t secondary = r_u32(actor + 852);
        uint32_t descriptor = r_u32(actor + 1084);
        uint32_t finished = 0;
        if (r_u8(secondary + 572) & 0x10)
        {
            uint32_t linked = r_u32(actor + 856);
            uint32_t linked_secondary = r_u32(linked + 852);
            uint32_t type = r_u16(linked_secondary + 544);
            w_u32(linked_secondary + 552, r_u32(linked_secondary + 552) & 0xFDFFFFFFu);
            if (r_u32(secondary + 604) < 2 && r_u16(0x800541D4 + 8 * type + 2) == 8 && !r_u32(actor + 480) && !r_u8(race + 57))
                (void)sub_800903F4(actor, 1);
        }
        if (r_u32(descriptor + 40) || r_u8(descriptor + 39) >= 0xF8)
            finished = 1u << (index & 31);
        finished_mask |= finished;
        if (finished_mask == (1u << (players & 31)) - 1)
        {
            int32_t timer = ((int32_t)(uint32_t)(r_u32(0x8005B230)));
            uint32_t player = 0x800CD898 + 1132 * index;
            if (timer < 0)
                timer = 0;
            timer = ((int32_t)(uint32_t)((uint32_t)timer + delta));
            w_u32(0x8005B230, (uint32_t)timer);
            if ((r_u32(player + 552) & 8) || timer >= ((int32_t)(uint32_t)(r_u32(0x8005B228))))
            {
                if (r_u32(0x8005B220))
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
    (void)sub_800B9794(inactive_out);
    rrj_trace_dispatch_target(0x800BA304u);
    (void)sub_800BA304(delta, r_u32(inactive_out), active_out, marked_out);
    rrj_trace_dispatch_target(0x800BA4CCu);
    (void)sub_800BA4CC(delta, r_u32(inactive_out), r_u32(active_out));
    rrj_trace_dispatch_target(0x800BCD48u);
    (void)sub_800BCD48(delta, r_u32(inactive_out), r_u32(marked_out));
    rrj_trace_dispatch_target(0x800B941Cu);
    for (index = 0; index < players; ++index)
    {
        uint32_t actor = r_u32(0x8005B268 + 4 * index);
        uint32_t secondary = r_u32(actor + 852);
        uint32_t linked = r_u32(actor + 856);
        if (r_u8(secondary + 572) & 0x10)
        {
            uint32_t contact = linked + 948 + 8u * (uint32_t)(int32_t)r_s8(linked + 946);
            if (r_u16(contact) == 16)
            {
                rrj_trace_dispatch_target(0x800C035Cu);
                (void)sub_800C035C(linked, r_u16(contact + 2), 0u);
                rrj_trace_dispatch_target(0x800B941Cu);
            }
        }
        if (linked && r_u8(race + 57) == 2)
        {
            uint32_t previous = linked + 948 + 8u * (uint32_t)((int32_t)r_s8(linked + 946) - 1);
            if (r_u16(previous) == 18)
            {
                rrj_trace_dispatch_target(0x80092E04u);
                (void)sub_80092E04(linked, delta);
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
    int32_t scalar_sign_143;
    int32_t scalar_sign_144;

    uint32_t race = r_u32(0x8005B2F8);
    uint32_t flags[4] = {0, 0, 0, 0};
    int32_t count = ((int32_t)(uint32_t)(r_u32(race + 48)));
    int32_t index;
    FUNCTION_MARKER(0x8009F700, "RASHCDG.BIN");
    for (index = 0; index < count && index < 4; ++index)
    {
        uint32_t player = 0x800CD898 + 1132u * (uint32_t)index;
        int32_t dx = (((int32_t)(uint32_t)(r_u32(player + 184))) >> 16) - (((int32_t)(uint32_t)(r_u32(actor + 184))) >> 16);
        int32_t dz = (int32_t)r_s16(player + 194) - r_s16(actor + 194);
        int32_t high = ((int32_t)(uint32_t)(((scalar_sign_143 = ((int32_t)(dx)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_143 + ((int32_t)(dx))) ^ scalar_sign_143)))))));
        int32_t low = ((int32_t)(uint32_t)(((scalar_sign_144 = ((int32_t)(dz)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_144 + ((int32_t)(dz))) ^ scalar_sign_144)))))));
        int32_t actor_x = ((int32_t)(uint32_t)(r_u32(actor + 184) - r_u32(player + 184)));
        int32_t actor_y = ((int32_t)(uint32_t)(r_u32(actor + 188) - r_u32(player + 188)));
        int32_t actor_z = ((int32_t)(uint32_t)(r_u32(actor + 192) - r_u32(player + 192)));
        int32_t sample_x = ((int32_t)(uint32_t)(r_u32(sample + 12) - r_u32(player + 184)));
        int32_t sample_y = ((int32_t)(uint32_t)(r_u32(sample + 16) - r_u32(player + 188)));
        int32_t sample_z = ((int32_t)(uint32_t)(r_u32(sample + 20) - r_u32(player + 192)));
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
        actor_dot = ((int32_t)(uint32_t)((uint32_t)sub_8001FC90(actor_x, axis_x) + (uint32_t)sub_8001FC90(actor_y, axis_y) + (uint32_t)sub_8001FC90(actor_z, axis_z)));
        sample_dot = ((int32_t)(uint32_t)((uint32_t)sub_8001FC90(sample_x, axis_x) + (uint32_t)sub_8001FC90(sample_y, axis_y) + (uint32_t)sub_8001FC90(sample_z, axis_z)));
        if (metric >= 160 && ((actor_dot < 0 && sample_dot < 0) || (r_u16(sample) >> 5) == 3))
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
    packed = r_u16(packed_id);
    group = packed >> 5;
    if (group >= 2 || ((int32_t)(uint32_t)(r_u32(actor + 480))) >= 131 || !(r_u8(actor + 509) & 0x10))
        return;
    if (!group)
    {
        uint32_t other = r_u32(0x8005B3A0) + 1096 * packed;
        if (r_u32(r_u32(other + 1084) + 40))
            return;
    }
    if (!(sub_8001FC58() & 7))
        sub_80017BA0(((int32_t)(uint32_t)(r_u32(actor + 184))), ((int32_t)(uint32_t)(r_u32(actor + 192))), 99 - (r_u32(actor + 180) & 1), 0);
}

uint32_t sub_8009FCDC(uint32_t actor)
{
    FUNCTION_MARKER(0x8009FCDC, "RASHCDG.BIN");
    return sub_8009FCE4(actor);
}

uint32_t sub_8009FCE4(uint32_t actor)
{
    int32_t scalar_sign_145;

    uint32_t race = r_u32(0x8005B2F8);
    uint32_t players = r_u32(race + 48);
    uint32_t result = 0;
    uint32_t index;
    FUNCTION_MARKER(0x8009FCE4, "RASHCDG.BIN");
    for (index = 0; index < players; ++index)
    {
        uint32_t player = r_u32(0x8005B268 + 4 * index);
        int32_t direction;
        int32_t distance;
        if (!actor || r_u32(r_u32(player + 852) + 604) >= 3 || ((int32_t)(uint32_t)(r_u32(player + 480))) <= 0x280000 || r_u32(player + 360) != r_u32(actor + 360))
            continue;
        direction = ((int32_t)(uint32_t)(r_u32(player + 364)));
        if (((int32_t)(uint32_t)(r_u32(player + 364) ^ r_u32(actor + 364))) >= 0)
            continue;
        if ((direction > 0 && ((int32_t)(uint32_t)(r_u32(player + 368))) >= ((int32_t)(uint32_t)(r_u32(actor + 368)))) || (direction < 0 && ((int32_t)(uint32_t)(r_u32(actor + 368))) >= ((int32_t)(uint32_t)(r_u32(player + 368)))))
            continue;
        if (r_u32(player + 360) == r_u32(actor + 360) && (!(r_u32(player + 360) >> 16) || r_u32(player + 336) == r_u32(actor + 336)))
            distance = ((int32_t)(uint32_t)(r_u32(player + 344) - r_u32(actor + 344)));
        else
            distance = ((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(player + 184, 12), xport_guest_ptr(actor + 432, 6), xport_guest_ptr(actor + 184, 12))));
        if (((scalar_sign_145 = ((int32_t)(distance)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_145 + ((int32_t)(distance))) ^ scalar_sign_145))))) < r_u32(actor + 304))
        {
            result = 1;
            (void)(uint32_t)sub_80017CF0(index, r_u32(actor + 180) & 1, r_u32(actor + 184), r_u32(actor + 192));
        }
    }
    return result;
}

uint32_t sub_8009FE90(uint32_t actor)
{
    uint32_t record;
    FUNCTION_MARKER(0x8009FE90, "RASHCDG.BIN");
    if (!actor || !r_u16(actor + 172))
        return 0;
    if (!r_s16(actor + 320))
    {
        (void)sub_8008C000(actor + 172, 3);
        return 0;
    }
    record = sub_80039A08(r_u32(r_u32(actor + 328)));
    if (record && r_u32(record + 12) && r_u32(actor + 328) == r_u32(record + 12))
        return 1;
    w_u16(actor + 320, 0);
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
    int32_t scalar_value_146;
    int32_t scalar_sign_146;

    uint32_t race = r_u32(0x8005B2F8);
    uint32_t players = r_u32(race + 48);
    uint32_t primary;
    uint32_t secondary;
    uint32_t index;
    FUNCTION_MARKER(0x800A04B8, "RASHCDG.BIN");
    if (!(r_u8(race + 4) & 0x10))
        return 0;
    primary = r_u32(0x8005B38C);
    secondary = r_u32(0x8005B21C);
    if (r_u32(primary + 360) == r_u32(secondary + 360))
    {
        uint32_t distances[2] = {0, 0};
        if (r_u32(actor + 360) != r_u32(primary + 360))
            return 0;
        for (index = 0; index < players && index < 2; ++index)
        {
            uint32_t player = r_u32(0x8005B268 + 4 * index);
            distances[index] = ((scalar_value_146 = (((int32_t)(uint32_t)(r_u32(actor + 368) - r_u32(player + 368))))), (scalar_sign_146 = scalar_value_146 >> 31), (((uint32_t)((uint32_t)((scalar_sign_146 + scalar_value_146) ^ scalar_sign_146)))));
        }
        return distances[1] < distances[0];
    }
    for (index = 0; index < players; ++index)
    {
        uint32_t player = r_u32(0x8005B268 + 4 * index);
        if (r_u32(actor + 360) == r_u32(player + 360))
            return index;
    }
    return 0;
}

uint32_t sub_8009B4C0(uint32_t actor, uint32_t delta, uint32_t force)
{
    uint32_t nearest;
    uint32_t result;
    uint32_t distance;
    uint8_t flags;

    FUNCTION_MARKER(0x8009B4C0, "RASHCDG.BIN");
    if (actor && r_u16(actor + 362) == 1)
    {
        result = r_u8(actor + 509) & 0xBC;
        w_u8(actor + 509, result);
        return result;
    }
    result = r_u8(actor + 509) & 0x40;
    if (result)
        return result;
    if ((int32_t)r_u32(actor + 480) < 131)
    {
        int32_t timer = (int32_t)r_u32(actor + 504);
        if (timer >= 0)
        {
            timer = (int32_t)((uint32_t)timer + delta);
            result = timer > 0x1FFFF;
            w_u32(actor + 504, (uint32_t)timer);
            if (!result)
                return result;
            w_u32(actor + 484, 0x26666);
            flags = r_u8(actor + 509);
            w_u32(actor + 504, 0xFFFF0000);
            result = (flags & 0xFC) | 0x40;
            w_u8(actor + 509, result);
            return result;
        }
    }
    flags = r_u8(actor + 509);
    if (flags & 1)
    {
        distance = sub_8003A5F4(actor + 360, &nearest, 1);
        if ((int32_t)r_u32(actor + 480) < 131)
        {
            w_u32(actor + 504, 0);
            return distance;
        }
        result = (int32_t)distance < (int32_t)(2 * r_u32(actor + 308));
        if (result)
            w_u32(actor + 504, 0);
        return result;
    }
    if (!force && !(flags & 2))
    {
        int32_t piece = r_s16(r_u32(actor + 340));
        if (piece)
        {
            int32_t last_piece = r_s16(r_u32(actor + 336) + 10) - 1;
            if (piece != last_piece)
                return (uint32_t)last_piece;
        }
    }
    if (r_u16(actor + 362))
    {
        result = r_u8(actor + 509) & 0xFD;
        w_u8(actor + 509, result);
        return result;
    }
    distance = sub_8003A5F4(actor + 360, &nearest, 1);
    if (nearest == UINT32_MAX)
        return 0xE0000;
    if ((int32_t)distance > 0xEFFFF)
        return 1;
    if ((int32_t)r_u32(actor + 480) < 132)
        return 1;
    {
        uint32_t speed = r_u32(actor + 480);
        uint32_t squared;
        uint32_t numerator;
        int32_t denominator;
        uint32_t positive_numerator;

        w_u8(actor + 509, r_u8(actor + 509) | 0x81);
        squared = (uint32_t)sub_8001FC90((int32_t)speed, (int32_t)speed);
        positive_numerator = (int32_t)(0 - squared) > 0;
        denominator = (int32_t)(distance << 1);
        speed = r_u32(actor + 480);
        squared = (uint32_t)sub_8001FC90((int32_t)speed, (int32_t)speed);
        numerator = positive_numerator ? 0 - squared : squared;
        if (positive_numerator)
        {
            if (denominator > 0)
                result = sub_80010028(numerator, (uint32_t)denominator);
            else
                result = 0 - sub_80010028(numerator, 0 - (uint32_t)denominator);
        }
        else
        {
            if (denominator > 0)
                result = 0 - sub_80010028(numerator, (uint32_t)denominator);
            else
                result = sub_80010028(numerator, 0 - (uint32_t)denominator);
        }
        w_u32(actor + 484, result);
        return result;
    }
}

uint32_t sub_8009BA5C(uint32_t actor)
{
    int32_t scalar_value_147;
    int32_t scalar_sign_147;

    uint32_t candidate = r_u32(0x800CE500);
    uint32_t stride = r_u32(0x800CE504);
    uint32_t result = 0;
    int32_t best_distance = -65536;
    int32_t count = ((int32_t)(uint32_t)(r_u32(r_u32(0x800CE50C))));
    FUNCTION_MARKER(0x8009BA5C, "RASHCDG.BIN");
    while (count >= 0)
    {
        if (r_u16(candidate + 172) && r_u32(candidate + 360) == r_u32(actor + 360) && ((int32_t)(uint32_t)(r_u32(candidate + 364) ^ r_u32(actor + 364))) >= 0)
        {
            int32_t direction = ((int32_t)(uint32_t)(r_u32(actor + 364)));
            int32_t actor_position = ((int32_t)(uint32_t)(r_u32(actor + 368)));
            int32_t candidate_position = ((int32_t)(uint32_t)(r_u32(candidate + 368)));
            if ((direction > 0 && actor_position < candidate_position) || (direction < 0 && candidate_position < actor_position))
            {
                int32_t distance = ((int32_t)(uint32_t)(((scalar_value_147 = (actor_position - candidate_position)), (scalar_sign_147 = scalar_value_147 >> 31), (((uint32_t)((uint32_t)((scalar_sign_147 + scalar_value_147) ^ scalar_sign_147)))))));
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
    int32_t scalar_sign_148;
    int32_t scalar_sign_149;

    uint32_t index = r_u8(actor + 511);
    uint32_t target;
    int32_t delta;
    FUNCTION_MARKER(0x800A05D4, "RASHCDG.BIN");
    if (((int32_t)(uint32_t)(index)) >= ((int32_t)(uint32_t)(r_u32(0x8005B1F8))))
        return 0;
    target = r_u32(0x8005B3A0) + 1096 * index;
    if (r_u32(r_u32(target + 852) + 604) >= 3)
        target = r_u32(target + 852);
    if (r_u32(actor + 360) != r_u32(target + 360))
        return 0;
    delta = ((int32_t)(uint32_t)(r_u32(target + 368) - r_u32(actor + 368)));
    if ((r_u32(actor + 360) >> 16) == 1)
        return ((scalar_sign_148 = ((int32_t)(delta)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_148 + ((int32_t)(delta))) ^ scalar_sign_148))))) <= 196607;
    if (((int32_t)(uint32_t)((uint32_t)delta ^ r_u32(actor + 364))) >= 0)
        return 0;
    if (((scalar_sign_149 = ((int32_t)(delta)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_149 + ((int32_t)(delta))) ^ scalar_sign_149))))) > 786432 || ((int32_t)(uint32_t)(r_u32(actor + 368))) <= 196607)
        return 1;
    return ((int32_t)(uint32_t)((uint32_t)sub_8003C520(actor + 328) - 196608)) < ((int32_t)(uint32_t)(r_u32(actor + 368)));
}

int32_t sub_800BF0F8(uint16_t packed_id, uint32_t actor)
{
    uint32_t group = packed_id >> 5;
    uint32_t table = 0x800CE4D0 + 16 * group;
    uint32_t candidate = r_u32(table) + r_u32(table + 4) * (packed_id & 31);
    int32_t separation;
    int32_t orientation;
    int32_t relative_speed;
    int32_t threshold;
    int32_t sign;
    int32_t orientation_abs;
    uint32_t candidate_radius;
    uint32_t candidate_length;
    uint32_t candidate_group;
    uint32_t perpendicular;
    uint32_t diagonal;

    FUNCTION_MARKER(0x800BF0F8, "RASHCDG.BIN");
    if (r_u32(candidate + 360) == r_u32(actor + 360) && (!(r_u32(candidate + 360) >> 16) || r_u32(candidate + 336) == r_u32(actor + 336)))
    {
        separation = (int32_t)(r_u32(candidate + 344) - r_u32(actor + 344));
        if ((int32_t)r_u32(actor + 364) < 0)
            separation = (int32_t)(0 - (uint32_t)separation);
    }
    else
        separation = (int32_t)sub_800B6AAC(xport_guest_ptr(candidate + 184, 12), xport_guest_ptr(actor + 432, 6), xport_guest_ptr(actor + 184, 12));
    if (!group)
    {
        uint32_t linked = r_u32(candidate + 856);
        if (linked)
        {
            uint32_t same_direction = (int32_t)(r_u32(candidate + 364) ^ r_u32(actor + 364)) >= 0;
            uint32_t negative_separation = (uint32_t)separation >> 31;
            if (same_direction == negative_separation)
            {
                uint32_t combined_radius = r_u32(candidate + 304) + r_u32(linked + 304);
                separation = (int32_t)(same_direction ? (uint32_t)separation + combined_radius : (uint32_t)separation - combined_radius);
            }
        }
    }
    sign = separation >> 31;
    separation = (int32_t)(((uint32_t)separation + (uint32_t)sign) ^ (uint32_t)sign);
    orientation = (int32_t)sub_8002E698(xport_guest_ptr(candidate + 444, 6), xport_guest_ptr(actor + 444, 6));
    sign = orientation >> 31;
    orientation_abs = (int32_t)(((uint32_t)orientation + (uint32_t)sign) ^ (uint32_t)sign);
    perpendicular = orientation_abs < 0x4000;
    diagonal = !perpendicular && orientation_abs <= 0xDDB1;
    candidate_radius = r_u32(candidate + 304);
    candidate_length = r_u32(candidate + 308);
    {
        int32_t diagonal_extent = (int32_t)(3 * (candidate_radius + candidate_length));
        diagonal_extent = (int32_t)((uint32_t)diagonal_extent + (diagonal_extent < 0 ? 3 : 0)) >> 2;
        threshold = (int32_t)(candidate_radius + ((0 - diagonal) & ((uint32_t)diagonal_extent - candidate_radius)) + ((0 - perpendicular) & (candidate_length - candidate_radius)) + r_u32(actor + 304));
    }
    if (separation > threshold)
        return 0x7FFF0000;
    candidate_group = r_u16(candidate + 172) >> 5;
    relative_speed = (int32_t)r_u32(actor + 480);
    if (candidate_group < 4 && !perpendicular)
        relative_speed = (int32_t)(orientation >= 0 ? (uint32_t)relative_speed - r_u32(candidate + 480) : (uint32_t)relative_speed + r_u32(candidate + 480));
    if ((r_u32(actor + 480) || r_u32(candidate + 480)) && relative_speed < 131)
        return 0x7FFF0000;
    separation = (int32_t)sub_800B6AAC(xport_guest_ptr(candidate + 184, 12), xport_guest_ptr(actor + 444, 6), xport_guest_ptr(actor + 184, 12));
    candidate_group = r_u16(candidate + 172) >> 5;
    if (candidate_group)
        separation = (int32_t)((uint32_t)separation - (2 * r_u32(actor + 308) + r_u32(candidate + 308)));
    else
        separation = (int32_t)((uint32_t)separation - r_u32(actor + 308));
    if (separation < 0)
    {
        int32_t magnitude;
        sign = separation >> 31;
        magnitude = (int32_t)(((uint32_t)separation + (uint32_t)sign) ^ (uint32_t)sign);
        return (int32_t)r_u32(actor + 308) < magnitude ? 0x7FFF0000 : separation;
    }
    if (relative_speed < 132)
        return separation;
    return (int32_t)sub_80010028((uint32_t)separation, (uint32_t)relative_speed);
}

uint32_t sub_8009B728(uint32_t actor, uint32_t delta)
{
    int32_t scalar_sign_153;
    int32_t scalar_sign_154;
    int32_t scalar_value_155;
    int32_t scalar_sign_155;

    uint32_t nearest_id, junction_distance = 0;
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
    timer = r_u32(0x8005B2E0) + delta;
    w_u32(0x8005B2E0, timer);
    if (timer <= 0x20000)
        return 0;
    if (((int32_t)(uint32_t)(r_u32(actor + 484))) > 0 && ((int32_t)(uint32_t)(r_u32(actor + 480))) > 0)
        return r_u32(actor + 480);
    player_index = sub_800A04B0(actor, 0, 0);
    player = r_u32(0x8005B268 + 4 * player_index);
    reference = 0x800CD898 + 1132 * player_index;
    facing_dot = ((int32_t)(uint32_t)((uint32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(actor + 184) - r_u32(reference + 184))), (int32_t)r_s16(reference + 444) * 16)));
    facing_dot = ((int32_t)(uint32_t)((uint32_t)facing_dot + (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(actor + 188) - r_u32(reference + 188))), (int32_t)r_s16(reference + 446) * 16)));
    facing_dot = ((int32_t)(uint32_t)((uint32_t)facing_dot + (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(actor + 192) - r_u32(reference + 192))), (int32_t)r_s16(reference + 448) * 16)));
    if (facing_dot >= 0 && ((int32_t)(uint32_t)(r_u32(actor + 480))) >= 132 && !(r_u32(actor + 360) >> 16) && r_u32(actor + 360) == r_u32(player + 360) && !(r_u8(actor + 509) & 0x13))
    {
        path_delta = ((int32_t)(uint32_t)(r_u32(actor + 368) - r_u32(player + 368)));
        if (((scalar_sign_153 = ((int32_t)(path_delta)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_153 + ((int32_t)(path_delta))) ^ scalar_sign_153))))) >= 0x280000 && ((scalar_sign_154 = ((int32_t)(path_delta)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_154 + ((int32_t)(path_delta))) ^ scalar_sign_154))))) <= 0x500000 && r_u32(actor + 372))
        {
            uint32_t segment = r_u32(actor + 372);
            segment_value = r_s16(segment + (((int32_t)(uint32_t)(r_u32(actor + 364))) > 0 ? 140 : 12));
            if (segment_value >= 2)
                junction_distance = sub_8003A5F4(actor + 360, &nearest_id, 1);
            if (segment_value >= 2 && junction_distance > 0x640000)
            {
                uint32_t nearest = sub_8009BA5C(actor);
                if (!nearest || ((scalar_value_155 = (((int32_t)(uint32_t)(r_u32(nearest + 368) - r_u32(actor + 368))))), (scalar_sign_155 = scalar_value_155 >> 31), (((uint32_t)((uint32_t)((scalar_sign_155 + scalar_value_155) ^ scalar_sign_155))))) > 0x190000)
                {
                    if ((sub_8001FC58() & 0x64) < 0x28)
                    {
                        uint8_t state = r_u8(actor + 508);
                        if (segment_value >= 3)
                        {
                            if (state == 2)
                                state = (r_u16(actor + 370) & 1) ? 3 : 1;
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
    w_u32(0x8005B2E0, 0);
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
    w_u32(count_out, 16);
    (void)sub_8008AE94(candidates, count_out, actor + 184, 3955095, 59, r_u16(actor + 172));
    count = r_u32(count_out);
    for (index = 0; index < count; ++index)
    {
        uint16_t candidate = r_u16(candidates + 2 * index);
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
            w_u32(actor + 484, 0x26666);
        w_u8(actor + 509, flags & 0xCF);
        return 0;
    }
    {
        uint32_t group = selected >> 5;
        uint32_t record;
        if (group == 6)
            record = r_u32(0x800CD6C4) + 280 * (selected & 31);
        else
        {
            uint32_t table = 0x800CE4D0 + 16 * group;
            record = r_u32(table) + r_u32(table + 4) * (selected & 31) + 172;
        }
        if (((int32_t)(uint32_t)(r_u32(actor + 480))) >= 132)
        {
            int32_t speed = ((int32_t)(uint32_t)(r_u32(actor + 480)));
            int32_t diameter = ((int32_t)(uint32_t)(2u * r_u32(actor + 308)));
            int32_t time = best - race_leaf_signed_ratio(diameter, speed);
            if (time >= 3276)
                w_u32(actor + 484, (uint32_t)-race_leaf_signed_ratio(speed, time));
            else
            {
                w_u32(actor + 484, 0);
                w_u32(actor + 480, 0);
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
    int32_t scalar_sign_156;
    int32_t scalar_value_157;
    int32_t scalar_sign_157;
    int32_t scalar_sign_158;
    int32_t scalar_sign_159;
    int32_t scalar_value_160;
    int32_t scalar_sign_160;

    const uint32_t scratch = 0x1F800360;
    int32_t remaining = ((int32_t)(uint32_t)(r_u32(r_u32(0x800CE50C))));
    uint32_t actor = r_u32(0x800CE500);
    uint32_t stride = r_u32(0x800CE504);
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
        if (!actor || !r_u16(actor + 172) || !sub_8009FE90(actor))
            goto next_actor;
        w_u16(actor + 320, (uint16_t)sub_80039F68(actor + 172));
        if (!r_s16(actor + 320))
            goto detach_actor;
        w_u32(actor + 468, r_u32(actor + 184));
        w_u32(actor + 472, r_u32(actor + 188));
        w_u32(actor + 476, r_u32(actor + 192));
        if ((r_u8(r_u32(0x8005B2F8) + 4) & 1) && !r_u32(actor + 180))
        {
            player_controlled = 1;
            if (sub_800A05D4(actor))
            {
                w_u32(actor + 484, 0);
                w_u32(actor + 480, 0);
            }
        }
        if ((uint32_t)(r_u8(r_u32(0x8005B2F8) + 57) - 1) < 2)
        {
            uint32_t kind = r_u32(actor + 180);
            if (kind && kind != 15)
            {
                uint32_t primary = r_u32(0x8005B38C);
                if (!r_u32(r_u32(primary + 1084) + 40))
                {
                    int32_t position = ((int32_t)(uint32_t)(r_u32(actor + 368) - ((uint32_t)r_u16(0x8005317A) << 16)));
                    if (((int32_t)(uint32_t)(r_u32(actor + 364) ^ r_u32(0x8005B2E8))) < 0)
                    {
                        if (position > 0 && position <= 5898239)
                            w_u32(actor + 484, 0xFFFD999A);
                    }
                    else if (position > -1310720 && position <= 2621439)
                    {
                        w_u32(actor + 484, 0xFFFD999A);
                    }
                }
            }
            else if (((int32_t)(uint32_t)(r_u32(actor + 480))) < 131 && ((int32_t)(uint32_t)(r_u32(actor + 484))) < 131)
            {
                w_u32(actor + 484, 0);
                w_u32(actor + 480, 0);
                goto next_actor;
            }
        }
        {
            int32_t speed = ((int32_t)(uint32_t)(r_u32(actor + 480) + (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(actor + 484))), ((int32_t)(uint32_t)(delta)))));
            int32_t maximum = 0x141DDD;
            w_u32(actor + 480, (uint32_t)speed);
            if (speed < 0)
            {
                w_u32(actor + 484, 0);
                w_u32(actor + 480, 0);
            }
            else if (!player_controlled && (r_u8(actor + 509) & 8) && speed > 585944)
            {
                w_u32(actor + 480, 585944);
                w_u32(actor + 484, 0);
            }
            else
            {
                if (player_controlled)
                {
                    uint32_t race = r_u32(0x8005B2F8);
                    int32_t divisor = ((int32_t)(uint32_t)(r_u32(0x800530F0 + 4 * r_u32(race + 60))));
                    int32_t target = race_leaf_signed_ratio(10223616, divisor);
                    uint32_t primary = r_u32(0x8005B38C);
                    uint32_t other_index = (r_u8(r_u32(primary + 1084) + 1) & 15) != 2;
                    uint32_t other = r_u32(0x8005B268 + 4 * other_index);
                    if (other)
                    {
                        int32_t difference = ((int32_t)(uint32_t)((uint32_t)target - r_u32(other + 480)));
                        maximum = difference > 0xFFFF ? difference : 0x10000;
                    }
                }
                if (((int32_t)(uint32_t)(r_u32(actor + 480))) >= maximum)
                {
                    w_u32(actor + 480, (uint32_t)maximum);
                    w_u32(actor + 484, 0);
                }
            }
        }
        old_piece = r_u32(actor + 340);
        old_offset = r_u32(actor + 348);
        old_path = r_u16(actor + 362);
        travel = ((int32_t)(uint32_t)((uint32_t)sub_8001FC90(((int32_t)(uint32_t)(delta)), ((int32_t)(uint32_t)(r_u32(actor + 480))))));
        new_piece = (uint32_t)sub_8003775C(actor + 172, xport_guest_ptr(actor + 328, 32), (uint32_t)travel);
        if (old_piece == new_piece)
        {
            int32_t direction = ((int32_t)(uint32_t)(r_u32(actor + 364)));
            if ((direction > 0 && travel >= ((int32_t)(uint32_t)(r_u32(old_piece + 32) - old_offset))) || (direction < 0 && travel >= ((int32_t)(uint32_t)(old_offset))))
                goto invalidate_actor;
        }
        else
        {
            int32_t alignment = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(new_piece + 14, 6), xport_guest_ptr(actor + 450, 6))));
            uint32_t index;
            for (index = 0; index < 3; ++index)
            {
                uint16_t tangent = r_u16(new_piece + 14 + 2 * index);
                uint16_t normal = r_u16(new_piece + 2 + 2 * index);
                w_u16(actor + 444 + 2 * index, tangent);
                w_u16(actor + 432 + 2 * index, normal);
                w_u16(actor + 450 + 2 * index, tangent);
            }
            if (alignment < 0)
            {
                movement_flags |= 2;
                for (index = 0; index < 3; ++index)
                {
                    w_u16(actor + 444 + 2 * index, (uint16_t)-r_s16(actor + 444 + 2 * index));
                    w_u16(actor + 432 + 2 * index, (uint16_t)-r_s16(actor + 432 + 2 * index));
                    w_u16(actor + 450 + 2 * index, (uint16_t)-r_s16(actor + 450 + 2 * index));
                }
            }
            for (index = 0; index < 3; ++index)
                w_u16(actor + 438 + 2 * index, (uint16_t)-r_s16(new_piece + 8 + 2 * index));
            {
                uint32_t angle = sub_80020018((uint32_t)((int32_t)r_s16(actor + 444) * 16), (uint32_t)((int32_t)r_s16(actor + 448) * 16));
                uint32_t table = 0x8005624C + 4 * (angle & 0xFFF);
                w_u32(actor + 292, angle);
                w_u32(actor + 296, (uint32_t)((int32_t)r_s16(table + 2) * 16));
                w_u32(actor + 300, (uint32_t)((int32_t)r_s16(table) * 16));
            }
        }
        if (!sub_8009FE90(actor))
            goto next_actor;
        {
            const uint16_t route_direction[3] = {r_u16(actor + 450), r_u16(actor + 450 + 2), r_u16(actor + 450 + 4)};
            const uint32_t route_record[8] = {r_u32(actor + 328), r_u32(actor + 328 + 4), r_u32(actor + 328 + 8), r_u32(actor + 328 + 12), r_u32(actor + 328 + 16), r_u32(actor + 328 + 20), r_u32(actor + 328 + 24), r_u32(actor + 328 + 28)};
            uint32_t route_position[3] = {r_u32(actor + 360), r_u32(actor + 360 + 4), r_u32(actor + 360 + 8)};
            uint32_t route_result = sub_8003662C(route_direction, route_record, route_position);
            w_u32(actor + 360, route_position[0]);
            w_u32(actor + 360 + 4, route_position[1]);
            w_u32(actor + 360 + 8, route_position[2]);
            (void)route_result;
        }
        {
            uint32_t hint = sub_8003DDB0(actor);
            (void)sub_8003DE28(actor, 0, 0, hint);
            sub_8003DF54(actor, 0, hint);
        }
        (void)sub_8003A9D8(actor);
        sub_8003AF9C(actor + 172, 1, 0);
        w_u32(actor + 324, sub_8003B61C(actor + 172));
        (void)sub_8002EAD8(xport_guest_ptr(new_piece + 20, 12), xport_guest_ptr(new_piece + 14, 6), r_u32(actor + 348), xport_guest_ptr(scratch, 12));
        {
            int32_t first = sub_8009E768(actor);
            int32_t second = sub_8009E768(actor);
            int32_t third = sub_8009E768(actor);
            int32_t target = ((int32_t)(uint32_t)((uint32_t)(first >> 31) + (uint32_t)second)) ^ (third >> 31);
            int32_t lateral = ((int32_t)(uint32_t)(r_u32(actor + 344)));
            if (r_s8(actor + 508) < 0)
                target = -target;
            lateral = ((int32_t)(uint32_t)(((scalar_sign_156 = ((int32_t)(lateral)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_156 + ((int32_t)(lateral))) ^ scalar_sign_156)))))));
            if (target != lateral)
            {
                int32_t speed = ((int32_t)(uint32_t)(r_u32(actor + 480)));
                int32_t divisor = ((r_u8(actor + 509) & 0x10) && speed >= 132) ? 327680 : 655360;
                int32_t rate = race_leaf_signed_ratio(speed, divisor);
                int32_t step = ((int32_t)(uint32_t)((uint32_t)sub_8001FC90(((int32_t)(uint32_t)(delta)), rate)));
                uint32_t difference = ((scalar_value_157 = (((int32_t)(uint32_t)((uint32_t)target - (uint32_t)lateral)))), (scalar_sign_157 = scalar_value_157 >> 31), (((uint32_t)((uint32_t)((scalar_sign_157 + scalar_value_157) ^ scalar_sign_157)))));
                if (((scalar_sign_158 = ((int32_t)(step)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_158 + ((int32_t)(step))) ^ scalar_sign_158))))) > difference)
                    step = ((int32_t)(uint32_t)(difference));
                lateral = lateral >= target ? ((int32_t)(uint32_t)((uint32_t)lateral - (uint32_t)step)) : ((int32_t)(uint32_t)((uint32_t)lateral + (uint32_t)step));
                w_u32(actor + 344, (uint32_t)lateral);
                movement_flags |= 2;
                if ((r_u8(actor + 510) & 2) && ((scalar_sign_159 = ((int32_t)(step)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_159 + ((int32_t)(step))) ^ scalar_sign_159))))) >= 4097)
                    movement_flags |= 4;
            }
            if (!(movement_flags & 4))
                xport_update_u8(actor + 510, XPORT_MEMORY_UPDATE_AND, 0xFD);
            if ((movement_flags & 2) || old_path != r_u16(actor + 362))
            {
                int32_t adjusted = ((int32_t)(uint32_t)(((scalar_value_160 = (((int32_t)(uint32_t)(r_u32(actor + 344))))), (scalar_sign_160 = scalar_value_160 >> 31), (((uint32_t)((uint32_t)((scalar_sign_160 + scalar_value_160) ^ scalar_sign_160)))))));
                if (((int32_t)(uint32_t)(r_u32(actor + 364))) <= 0)
                    adjusted = -adjusted;
                w_u32(actor + 344, (uint32_t)adjusted);
            }
            if (r_s8(actor + 508) < 0)
                w_u32(actor + 344, 0u - r_u32(actor + 344));
        }
        (void)sub_8002EAD8(xport_guest_ptr(scratch, 12), xport_guest_ptr(new_piece + 2, 6), r_u32(actor + 344), xport_guest_ptr(actor + 184, 12));
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
            int32_t threshold = ((int32_t)(uint32_t)((0x141DDDu << player_controlled) >> 1));
            if (((int32_t)(uint32_t)(r_u32(actor + 480))) < threshold && !(r_u8(actor + 509) & 0x11))
                w_u32(actor + 484, 0x26666);
        }
        (void)sub_8009B4C0(actor, delta, old_piece != new_piece);
        xport_update_u8(actor + 509, XPORT_MEMORY_UPDATE_AND, 0x7F);
        (void)sub_8008BA18(actor);
        (void)sub_8009B728(actor, delta);
        goto next_actor;
    invalidate_actor:
        w_u16(actor + 320, 0);
    detach_actor:
        (void)sub_8008C000(actor + 172, 3);
    next_actor:
        --remaining;
        actor += stride;
    }
    return stride;
}

uint32_t sub_80074D58(uint32_t actor)
{
    FUNCTION_MARKER(0x80074D58u, "RASHCDG.BIN");
    uint32_t linked = r_u32(actor + 856);
    uint32_t result = r_u32(actor + 560) & 0x400;
    if (result)
    {
        uint32_t state = r_u32(linked + 852);
        result = r_u32(state + 604) < 2;
        if (result)
        {
            w_u32(linked + 568, r_u32(linked + 568) | 8);
            w_u16(state + 456, r_u16(actor + 450));
            w_u16(state + 458, r_u16(actor + 452));
            w_u16(state + 460, r_u16(actor + 454));
            w_u32(state + 552, r_u32(state + 552) | 0x200000);
            w_u32(state + 480, r_u32(actor + 480) + 878916);
            (void)sub_80090D84(state);
            w_u32(linked + 568, r_u32(linked + 568) & ~8u);
            if (!r_u32(actor + 616))
            {
                w_u32(actor + 624, 3003949);
                w_u32(actor + 632, 0);
                w_u32(actor + 620, 0xFFF80000);
                w_u32(actor + 628, 0);
            }
            result = r_u32(actor + 560) & 0xFFFFFBFFu;
            w_u32(actor + 560, result);
        }
    }
    return result;
}

uint32_t sub_80074E6C(uint32_t actor, uint32_t delta)
{
    FUNCTION_MARKER(0x80074E6Cu, "RASHCDG.BIN");
    uint32_t flags = r_u32(actor + 564);
    uint32_t controls;
    if (flags & 0x200)
    {
        uint32_t timer = r_u32(actor + 704) + delta;
        w_u32(actor + 704, timer);
        if (((int32_t)(uint32_t)(r_u32(0x800D3974))) < ((int32_t)(uint32_t)(timer)))
            flags &= ~0x200u;
        w_u32(actor + 564, flags);
    }
    controls = r_u32(actor + 560);
    if (controls & 2)
    {
        if (controls & 4)
        {
            int8_t count = r_s8(actor + 848);
            if (count > 0 && (controls & 8))
            {
                w_u32(actor + 704, 0);
                w_u32(actor + 564, r_u32(actor + 564) | 0x200);
                w_u8(actor + 848, (uint8_t)(count - 1));
                if (sub_80027028(actor, 0))
                    (void)sub_80027540(actor, 0, 0);
                if (sub_80027028(actor, 1))
                    (void)sub_80027540(actor, 1, 2);
            }
            else
            {
                if (!(controls & 0x800))
                    controls |= 0x1800;
                w_u32(actor + 560, controls);
            }
            w_u32(actor + 560, r_u32(actor + 560) & ~4u);
        }
    }
    else
        w_u32(actor + 564, r_u32(actor + 564) & ~0x200u);
    flags = r_u32(actor + 564);
    if (r_u32(actor + 560) & 0x40)
        flags &= ~0x200u;
    w_u32(actor + 564, flags);
    return 0xFFFFFDFFu;
}

uint32_t sub_8007EF60(uint32_t specification, int32_t scale, int32_t lateral, int32_t grip)
{
    FUNCTION_MARKER(0x8007EF60u, "RASHCDG.BIN");
    int32_t projected = (int32_t)sub_8001FC90(lateral, ((int32_t)(uint32_t)(r_u32(specification + 192))));
    int32_t first_square = (int32_t)sub_8001FC90(projected, projected);
    int32_t second = (int32_t)sub_8001FC90(grip, scale);
    int32_t difference = (int32_t)sub_8001FC90(second, second) - first_square;
    int32_t root;
    if (difference < 0)
        return 0;
    root = (int32_t)(sub_8004CF74((uint32_t)difference) << 2);
    return (uint32_t)race_leaf_signed_ratio(root, ((int32_t)(uint32_t)(r_u32(specification + 200))));
}

uint32_t sub_8007E868(uint32_t vector, int32_t limit, int32_t minimum, int32_t value)
{
    FUNCTION_MARKER(0x8007E868u, "RASHCDG.BIN");
    uint32_t target = (uint32_t)minimum;
    uint32_t x, y, z, length, remaining, numerator, quotient, scale;
    if (value < limit)
    {
        if (value > 0)
        {
            if (limit > 0)
                quotient = 0u - sub_80010028((uint32_t)value, (uint32_t)limit);
            else
                quotient = sub_80010028((uint32_t)value, 0u - (uint32_t)limit);
        }
        else
        {
            if (limit > 0)
                quotient = sub_80010028(0u - (uint32_t)value, (uint32_t)limit);
            else
                quotient = 0u - sub_80010028(0u - (uint32_t)value, 0u - (uint32_t)limit);
        }
        target = quotient >> 4;
        if ((int16_t)target < (int16_t)minimum)
            target = (uint32_t)minimum;
    }
    if (r_s16(vector + 2) == (int16_t)target)
        return 0;
    x = (uint32_t)(int32_t)r_s16(vector) << 4;
    z = (uint32_t)(int32_t)r_s16(vector + 4) << 4;
    y = (uint32_t)(int32_t)(int16_t)target << 4;
    w_u16(vector + 2, (uint16_t)target);
    length = (uint32_t)sub_8001FC90((int32_t)x, (int32_t)x);
    length += (uint32_t)sub_8001FC90((int32_t)z, (int32_t)z);
    if ((int32_t)length < 6)
    {
        x = 0;
        z = 655;
        length = 6;
    }
    remaining = 65536u - (uint32_t)sub_8001FC90((int32_t)y, (int32_t)y);
    if ((int32_t)remaining > 0)
    {
        numerator = 65536u - (uint32_t)sub_8001FC90((int32_t)y, (int32_t)y);
        if ((int32_t)length > 0)
            quotient = sub_80010028(numerator, length);
        else
            quotient = 0u - sub_80010028(numerator, 0u - length);
    }
    else
    {
        numerator = (uint32_t)sub_8001FC90((int32_t)y, (int32_t)y) - 65536u;
        if ((int32_t)length > 0)
            quotient = 0u - sub_80010028(numerator, length);
        else
            quotient = sub_80010028(numerator, 0u - length);
    }
    scale = sub_8004CF74(quotient) << 2;
    quotient = (uint32_t)((int32_t)sub_8001FC90((int32_t)scale, (int32_t)x) >> 4);
    w_u16(vector, (uint16_t)quotient);
    quotient = (uint32_t)((int32_t)sub_8001FC90((int32_t)scale, (int32_t)z) >> 4);
    w_u16(vector + 4, (uint16_t)quotient);
    return quotient;
}

uint32_t sub_8008EE60(uint32_t actor)
{
    FUNCTION_MARKER(0x8008EE60u, "RASHCDG.BIN");
    uint32_t config = r_u32(actor + 540);
    uint32_t body = r_u32(actor + 596);
    uint32_t record = r_u32(config + 4) + 12 * r_u32(config + 12);
    uint32_t table = r_u32(r_u32(config + 40) + 4);
    uint32_t item = r_u32(table + 4 * r_u8(record));
    int32_t divisor = (int16_t)(r_u16(item + 16) - 1);
    int32_t fraction = (int32_t)((r_u32(config + 32) << 16) / r_u32(config + 24));
    int32_t longitudinal = (int32_t)sub_8001FC90(-71683, (int32_t)(r_u32(config + 16) << 16) + fraction) / divisor;
    uint32_t axis;
    uint32_t i;
    for (axis = 0; axis < 3; ++axis)
    {
        int32_t position = ((int32_t)(uint32_t)(r_u32(body + 184 + 4 * axis)));
        position += (int32_t)sub_8001FC90(32768, (int32_t)r_s16(body + 432 + 2 * axis) << 4);
        position += (int32_t)sub_8001FC90(-60417, (int32_t)r_s16(body + 438 + 2 * axis) << 4);
        position += (int32_t)sub_8001FC90(longitudinal, (int32_t)r_s16(body + 444 + 2 * axis) << 4);
        w_u32(actor + 184 + 4 * axis, (uint32_t)position);
    }
    for (i = 0; i < 9; ++i)
        w_u16(actor + 432 + 2 * i, r_u16(body + 432 + 2 * i));
    for (i = 0; i < 3; ++i)
        w_u16(actor + 450 + 2 * i, r_u16(body + 450 + 2 * i));
    i = r_u32(body + 480);
    if (((int32_t)(uint32_t)(i)) <= 327679)
        i = 327680;
    w_u32(actor + 480, i);
    return i;
}

uint32_t sub_80072C7C(uint32_t actor, uint32_t delta)
{
    FUNCTION_MARKER(0x80072C7Cu, "RASHCDG.BIN");
    uint32_t linked = r_u32(actor + 856);
    uint32_t flags;
    uint32_t specification;
    uint32_t choose_target;
    int32_t target = 0;
    int32_t current;
    int32_t rate = 0;
    int32_t next;
    if (r_u32(linked + 720))
        return 0;
    flags = r_u32(linked + 560);
    specification = r_u32(actor + 556);
    choose_target = (flags >> 20) & 1;
    if (!r_u32(actor + 576))
    {
        flags &= ~0x300u;
        choose_target = 0;
    }
    if (choose_target)
    {
        uint32_t player = r_u16(actor + 172);
        uint32_t count = r_u32(r_u32(0x8005B2F8) + 48);
        uint32_t mask;
        target = ((int32_t)(uint32_t)(r_u32(0x800CE540 + 8 * (player + (count < 2 ? 1 : 2)) + 4)));
        current = ((int32_t)(uint32_t)(r_u32(linked + 672)));
        mask = target < current ? 0x100u : current < target ? 0x200u : 0;
        flags |= mask | ((mask && !(flags & mask)) << 7);
        if (mask == 0x200)
            flags &= ~0x100u;
        else if (mask == 0x100)
            flags &= ~0x200u;
        w_u32(linked + 560, flags);
    }
    current = ((int32_t)(uint32_t)(r_u32(linked + 672)));
    if (flags & 0x100)
        rate = flags & 0x200 ? 0 : -((int32_t)(uint32_t)(r_u32(specification + 428)));
    else if (flags & 0x200)
        rate = ((int32_t)(uint32_t)(r_u32(specification + 428)));
    else if (!choose_target)
    {
        choose_target = 1;
        if (current > 0)
            rate = -2 * ((int32_t)(uint32_t)(r_u32(specification + 428)));
        else if (current < 0)
            rate = 2 * ((int32_t)(uint32_t)(r_u32(specification + 428)));
    }
    next = current + (int32_t)sub_8001FC90(rate, ((int32_t)(uint32_t)(delta)));
    if (choose_target)
    {
        if (current == target || (current < target && next > target) || (current > target && next < target))
            next = target;
    }
    if (next < -65536)
        next = -65536;
    if (next > 65536)
        next = 65536;
    w_u32(linked + 672, (uint32_t)next);
    if (((int32_t)(uint32_t)(r_u32(actor + 576))) <= 32768)
        return 0;
    next = (int32_t)sub_8001FC90(next, ((int32_t)(uint32_t)(r_u32(specification + 432))));
    return (uint32_t)race_leaf_signed_ratio(next, ((int32_t)(uint32_t)(r_u32(actor + 576))));
}

uint32_t sub_80073D74(uint32_t actor)
{
    FUNCTION_MARKER(0x80073D74u, "RASHCDG.BIN");
    int32_t yaw = ((int32_t)(uint32_t)(r_u32(actor + 636)));
    int32_t speed = ((int32_t)(uint32_t)(r_u32(actor + 576)));
    int32_t steering;
    uint32_t result;
    if (!yaw || speed <= 32768)
    {
        int32_t prior = r_s16(actor + 826);
        w_u32(actor + 488, 0);
        result = (uint32_t)(prior / 2);
    }
    else
    {
        uint32_t specification = r_u32(actor + 556);
        int32_t angle = yaw + ((int32_t)(uint32_t)(r_u32(actor + 668)));
        int32_t curve;
        int32_t tangent;
        int32_t first = (int32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(specification + 280))), speed) + 65536;
        int32_t second = (int32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(specification + 284))), speed) + 65536;
        if (angle < -91505)
            angle = -91505;
        if (angle > 91505)
            angle = 91505;
        curve = ((int32_t)(uint32_t)((uint32_t)sub_8001FEB4((uint32_t)((652LL * angle) >> 16))));
        tangent = (int32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(actor + 736))), curve) + ((int32_t)(uint32_t)(r_u32(actor + 732)));
        steering = race_leaf_signed_ratio(first, second);
        if (steering <= 64879)
        {
            int32_t denominator = (int32_t)sub_8001FC90(65536 - steering, speed);
            int32_t correction = race_leaf_signed_ratio(tangent, denominator);
            int32_t limit = ((int32_t)(uint32_t)(r_u32(specification + 232)));
            if (correction < -limit)
                correction = -limit;
            if (correction > limit)
                correction = limit;
            w_u32(actor + 488, (uint32_t)correction);
            steering = (int32_t)sub_8001FC90(steering, correction);
        }
        else
            w_u32(actor + 488, (uint32_t)steering);
        if (r_s8(actor + 8) >= 2)
            return 0;
        steering = race_leaf_signed_ratio(steering, speed);
        steering = (int32_t)(((int64_t)steering * ((int32_t)(uint32_t)(r_u32(specification + 4)))) >> 16);
        if (steering < -51471)
            steering = -51471;
        if (steering > 51471)
            steering = 51471;
        result = (uint32_t)((163 * steering) >> 14);
    }
    w_u16(actor + 826, (uint16_t)result);
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
        center[i] = ((int32_t)(uint32_t)(r_u32(actor + 184 + 4 * i))) + ((int32_t)(uint32_t)(r_u32(actor + 244 + 4 * i)));
    middle = ((int32_t)(uint32_t)(r_u32(actor + 312)));
    if (r_u16(0x800541D6 + 8 * r_u16(actor + 544)) == 5)
        middle /= 3;
    else
        middle /= 2;
    (void)sub_8002EE50(r_u32(actor + 304), xport_guest_ptr(actor + 256, 6), xport_guest_ptr(first_address, 12));
    (void)sub_8002EE50((uint32_t)middle, xport_guest_ptr(actor + 262, 6), xport_guest_ptr(second_address, 12));
    (void)sub_8002EE50(r_u32(actor + 308), xport_guest_ptr(actor + 268, 6), xport_guest_ptr(third_address, 12));
    for (i = 0; i < 3; ++i)
    {
        first[i] = ((int32_t)(uint32_t)(r_u32(first_address + 4 * i)));
        second[i] = ((int32_t)(uint32_t)(r_u32(second_address + 4 * i)));
        third[i] = ((int32_t)(uint32_t)(r_u32(third_address + 4 * i)));
    }
    for (i = 0; i < 8; ++i)
        for (j = 0; j < 3; ++j)
            corners[i][j] = first_sign[i] * first[j] + second_sign[i] * second[j] + third_sign[i] * third[j];
    for (i = 0; i < 8; ++i)
        for (j = 0; j < 3; ++j)
            w_u32(actor + 196 + 12 * i + 4 * j, (uint32_t)(center[j] + corners[i][j]));
    w_u8(actor + 547, 2);
    return 2;
}

uint32_t sub_8009246C(uint32_t actor)
{
    uint32_t local_position[3];
    uint16_t local_normal[3];
    const uint32_t *position = local_position;
    const uint16_t *normal = local_normal;
    uint32_t inverted = (r_u32(actor + 552) >> 30) & 1;
    uint32_t use_surface_position = 0;
    int32_t found;
    uint32_t dot = 0;
    uint32_t i;

    FUNCTION_MARKER(0x8009246C, "RASHCDG.BIN");
    if (r_u32(actor + 388) & 1)
    {
        found = (int32_t)sub_800A7BF8(actor, 0, local_position, local_normal, r_u32(actor + 536));
        w_u32(actor + 536, (uint32_t)found);
    }
    else
    {
        found = 0;
        w_u8(actor + 534, r_u32(actor + 372) ? r_u8(actor + 394) : 1);
        w_u32(actor + 536, 0);
    }
    if (found <= 0)
    {
        uint32_t piece = r_u32(actor + 340);

        position = (const uint32_t *)xport_guest_cptr(piece + 20, 12);
        normal = (const uint16_t *)xport_guest_cptr(piece + 8, 6);
    }
    if (found >= 0)
    {
        if ((int16_t)xport_load_le16((const uint8_t *)normal + 2) < -614)
        {
            for (i = 0; i < 3; ++i)
                w_u16(actor + 522 + 2 * i, (uint16_t)(0 - (uint32_t)xport_load_le16((const uint8_t *)normal + 2 * i)));
            use_surface_position = 1;
        }
    }
    else if (!r_u32(actor + 480))
    {
        w_u32(actor + 480, 117188);
        (void)sub_8002EE50(117188, xport_guest_ptr(actor + 450, 6), xport_guest_ptr(actor + 456, 12));
    }
    if (!inverted && !use_surface_position)
    {
        position = local_position;
        (void)sub_8002EAD8(xport_guest_ptr(actor + 184, 12), xport_guest_ptr(actor + 522, 6), r_u32(actor + 508), local_position);
    }
    for (i = 0; i < 3; ++i)
    {
        int32_t difference = (int32_t)(xport_load_le32((const uint8_t *)position + 4 * i) - r_u32(actor + 184 + 4 * i));
        int32_t component = (int16_t)r_u16(actor + 522 + 2 * i) * 16;

        dot += (uint32_t)(((int64_t)difference * component) >> 16);
    }
    if (!inverted)
        dot -= r_u32(actor + 508);
    return (uint32_t)sub_8002EAD8(xport_guest_ptr(actor + 184, 12), xport_guest_ptr(actor + 522, 6), dot, xport_guest_ptr(inverted ? actor + 504 : actor + 184, 12));
}

static void race_mount_parameters(uint32_t actor, int32_t *divisor, int32_t *travel)
{
    uint32_t config = r_u32(actor + 540);
    uint32_t record = r_u32(config + 4) + 12 * r_u32(config + 12);
    uint32_t table = r_u32(r_u32(config + 40) + 4);
    uint32_t item = r_u32(table + 4 * r_u8(record));
    *divisor = (int16_t)(r_u16(item + 16) - 1);
    *travel = (int32_t)(r_u32(config + 16) << 16) + (int32_t)((r_u32(config + 32) << 16) / r_u32(config + 24));
}

static void race_copy_halfwords(uint32_t output, uint32_t input, uint32_t count)
{
    uint32_t i;
    for (i = 0; i < count; ++i)
        w_u16(output + 2 * i, r_u16(input + 2 * i));
}

static void race_place_from_basis(uint32_t actor, uint32_t origin, uint32_t basis, int32_t first, int32_t second, int32_t third)
{
    uint32_t axis;
    for (axis = 0; axis < 3; ++axis)
    {
        int32_t value = ((int32_t)(uint32_t)(r_u32(origin + 4 * axis)));
        value += (int32_t)sub_8001FC90(first, (int32_t)r_s16(basis + 2 * axis) << 4);
        value += (int32_t)sub_8001FC90(second, (int32_t)r_s16(basis + 6 + 2 * axis) << 4);
        value += (int32_t)sub_8001FC90(third, (int32_t)r_s16(basis + 12 + 2 * axis) << 4);
        w_u32(actor + 184 + 4 * axis, (uint32_t)value);
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
    int32_t scalar_value_161;
    int32_t scalar_sign_161;

    FUNCTION_MARKER(0x8008E044u, "RASHCDG.BIN");
    const uint32_t vector = 0x1F800300;
    int32_t divisor;
    int32_t travel;
    uint32_t mirrored = (r_u32(actor + 552) >> 27) & 1;
    uint32_t body;
    int32_t first;
    int32_t second;
    int32_t third;
    int32_t magnitude;
    int32_t scale;
    uint32_t i;
    uint32_t result;
    race_mount_parameters(actor, &divisor, &travel);
    if (r_u8(actor + 572) & 0x20)
    {
        first = -77594 - (mirrored ? 65536 : 0);
        second = -59814;
        third = -47874;
        body = r_u32(r_u32(actor + 596) + 856);
    }
    else
    {
        uint32_t angle;
        int32_t sine;
        body = r_u32(actor + 596);
        angle = ((scalar_value_161 = ((int32_t)(((int64_t)652 * ((int32_t)(uint32_t)(r_u32(body + 652)))) >> 16))), (scalar_sign_161 = scalar_value_161 >> 31), (((uint32_t)((uint32_t)((scalar_sign_161 + scalar_value_161) ^ scalar_sign_161))))) & 0xFFF;
        sine = (int32_t)r_s16(0x8005624E + 4 * angle) << 4;
        first = -72862 - (int32_t)sub_8001FC90(65536 - sine, 117964);
        second = -70405;
        third = -18821;
    }
    first = (int32_t)sub_8001FC90(first, travel) / divisor;
    if (mirrored)
        first = -first;
    race_place_from_basis(actor, body + 184, body + 516, first, second, third);
    race_copy_halfwords(actor + 444, body + 522, 3);
    race_copy_halfwords(actor + 438, body + 516, 3);
    race_copy_halfwords(actor + 432, body + 528, 3);
    if (!mirrored)
    {
        for (i = 0; i < 3; ++i)
        {
            w_u16(actor + 438 + 2 * i, (uint16_t)-r_s16(actor + 438 + 2 * i));
            w_u16(actor + 432 + 2 * i, (uint16_t)-r_s16(actor + 432 + 2 * i));
        }
    }
    scale = (r_u8(actor + 572) & 0x20) ? -786432 + (mirrored ? 1572864 : 0) : -655360 + (mirrored ? 1310720 : 0);
    (void)sub_8002EE50((uint32_t)scale, xport_guest_ptr(body + 516, 6), xport_guest_ptr(vector, 12));
    for (i = 0; i < 3; ++i)
        w_u32(vector + 4 * i, r_u32(vector + 4 * i) + r_u32(body + 456 + 4 * i));
    magnitude = ((int32_t)(uint32_t)(sub_8002E548(xport_guest_ptr(vector, 12))));
    w_u32(actor + 480, (uint32_t)magnitude);
    scale = race_inverse_vector_length(magnitude);
    result = sub_8002EED8((uint32_t)scale, xport_guest_ptr(vector, 12), xport_guest_ptr(actor + 450, 6));
    if (size_out)
    {
        if (travel < 19660)
        {
            w_u32(size_out, r_u32(actor + 312));
            result = (uint32_t)(((int32_t)(uint32_t)(r_u32(actor + 312))) / 4);
            w_u32(actor + 312, result);
        }
        else if (travel <= 216267)
        {
            w_u32(size_out, r_u32(actor + 312));
            result = (uint32_t)(((int32_t)(uint32_t)(r_u32(actor + 312))) / 3);
            w_u32(actor + 312, result);
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
    race_mount_parameters(actor, &divisor, &travel);
    if (r_u8(actor + 572) & 0x20)
    {
        first = 36929;
        second = -135030;
        third = -21725;
        body = r_u32(r_u32(actor + 596) + 856);
    }
    else
    {
        body = r_u32(actor + 596);
        first = -65;
        second = -121910;
        third = 4384;
    }
    basis = body + 516;
    if (r_u32(actor + 552) & 0x10000)
    {
        third = 0;
        second = -170393;
        origin = (r_u32(body + 568) & 0x600) ? body + 184 : body + 504;
        race_copy_halfwords(actor + 432, body + 432, 9);
    }
    else
    {
        basis = actor + 432;
        origin = actor + 184;
        for (i = 0; i < 3; ++i)
            w_u32(origin + 4 * i, r_u32(body + 504 + 4 * i) + r_u32(actor + 580 + 4 * i));
        second = (int32_t)sub_8001FC90(second + 42598, travel) / divisor - 42598;
    }
    race_place_from_basis(actor, origin, basis, first, second, third);
    for (i = 0; i < 3; ++i)
        w_u16(actor + 450 + 2 * i, (uint16_t)-r_s16(actor + 438 + 2 * i));
    speed = r_u32(body + 480);
    if (((int32_t)(uint32_t)(speed)) <= 327679)
        speed = 327680;
    w_u32(actor + 480, speed);
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
    race_mount_parameters(actor, &divisor, &travel);
    if (r_u8(actor + 572) & 0x20)
    {
        first = 36536 + (int32_t)sub_8001FC90(23429 - 36536, travel) / divisor;
        second = -65536 + (int32_t)sub_8001FC90(-90885 + 65536, travel) / divisor;
        third = -51393;
        body = r_u32(r_u32(actor + 596) + 856);
    }
    else
    {
        first = (int32_t)sub_8001FC90(-11396, travel) / divisor;
        second = -49152 + (int32_t)sub_8001FC90(-71106 + 49152, travel) / divisor;
        third = -18435;
        body = r_u32(actor + 596);
    }
    if (r_u32(actor + 552) & 0x08000000)
        first = -first;
    race_place_from_basis(actor, body + 184, body + 432, first, second, third);
    race_copy_halfwords(actor + 444, body + 438, 3);
    race_copy_halfwords(actor + 432, body + 432, 3);
    race_copy_halfwords(actor + 438, body + 444, 3);
    for (i = 0; i < 3; ++i)
        w_u16(actor + 450 + 2 * i, (uint16_t)-r_s16(actor + 438 + 2 * i));
    if (size_out)
    {
        w_u32(size_out, r_u32(actor + 312));
        w_u32(actor + 312, (uint32_t)(((int32_t)(uint32_t)(r_u32(actor + 312))) / 2));
    }
    speed = r_u32(body + 480);
    result = ((int32_t)(uint32_t)(speed)) > 327679;
    if (!result)
        speed = 327680;
    w_u32(actor + 480, speed);
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
    race_mount_parameters(actor, &divisor, &travel);
    if (r_u8(actor + 572) & 0x20)
    {
        first = 36739;
        second = -72207 + (int32_t)sub_8001FC90(-85314 + 72207, travel) / divisor;
        third = (int32_t)sub_8001FC90(-108678, travel) / divisor;
        body = r_u32(r_u32(actor + 596) + 856);
    }
    else
    {
        first = 1218;
        second = -52114 + (int32_t)sub_8001FC90(-65221 + 52114, travel) / divisor;
        third = (int32_t)sub_8001FC90(-67718, travel) / divisor;
        body = r_u32(actor + 596);
    }
    race_place_from_basis(actor, body + 184, body + 432, first, second, third);
    race_copy_halfwords(actor + 432, body + 432, 9);
    race_copy_halfwords(actor + 450, actor + 444, 3);
    speed = ((int32_t)(uint32_t)(r_u32(body + 480))) - 327680;
    if (speed <= 0)
    {
        for (i = 0; i < 3; ++i)
            w_u16(actor + 450 + 2 * i, (uint16_t)-r_s16(actor + 450 + 2 * i));
        speed = 327680 - ((int32_t)(uint32_t)(r_u32(body + 480)));
    }
    w_u32(actor + 480, (uint32_t)speed);
    if (size_out)
    {
        w_u32(size_out, r_u32(actor + 312));
        speed = ((int32_t)(uint32_t)(r_u32(actor + 312))) / 2;
        w_u32(actor + 312, (uint32_t)speed);
    }
    return (uint32_t)speed;
}

static void race_copy_vector16(uint32_t output, uint32_t input)
{
    race_copy_halfwords(output, input, 3);
}

static void race_negate_vector16(uint32_t vector)
{
    uint32_t i;
    for (i = 0; i < 3; ++i)
        w_u16(vector + 2 * i, (uint16_t)-r_s16(vector + 2 * i));
}

static void race_outer_product16(uint32_t diagonal_address, uint32_t direction_address, uint32_t output)
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
        w_u16(output + 2 * i, (uint16_t)result[i]);
}

static void race_select_launch_direction(uint32_t actor, uint32_t body)
{
    uint32_t source = (r_u32(actor + 552) & 0x200000) ? actor + 456 : body + 450;
    race_copy_vector16(actor + 450, source);
}

static void race_tilt_launch_direction(uint32_t actor)
{
    int32_t x = r_u16(actor + 450);
    int32_t z = r_s16(actor + 454);
    x += (357 * z) >> 12;
    w_u16(actor + 450, (uint16_t)x);
    z -= (357 * (int16_t)x) >> 12;
    w_u16(actor + 454, (uint16_t)z);
}

static void race_mark_player_event(uint32_t actor, uint32_t body, uint32_t require_flag)
{
    uint32_t scalar_player_220;
    uint32_t scalar_count_220;

    uint32_t record;
    uint32_t bits;
    if (require_flag && !(r_u32(body + 568) & require_flag))
        return;
    record = ((scalar_player_220 = r_u16(((uint32_t)(body)) + 172)), (scalar_count_220 = r_u32(r_u32(0x8005B2F8) + 48)), (((uint32_t)(scalar_player_220 < scalar_count_220 ? 0x800CD898 + 1132 * scalar_player_220 : 0))));
    if (!record)
        return;
    bits = (r_u32(actor + 552) & 0x10000) ? 0x0E000000u : 0x06000000u;
    w_u32(record + 548, r_u32(record + 548) | bits | 0x80u);
}

static void race_rotate_mount_pair(uint32_t actor, int32_t angle, uint32_t second_plane)
{
    uint32_t scalar_address_222;
    uint32_t scalar_address_223;

    uint32_t scalar_low_11;
    uint32_t scalar_low_12;
    uint32_t scalar_low_13;
    uint32_t scalar_low_14;
    uint32_t scalar_low_15;
    uint32_t scalar_low_16;
    uint32_t scalar_low_17;
    uint32_t scalar_low_18;
    uint32_t scalar_low_19;
    uint32_t scalar_low_20;
    uint32_t scalar_low_21;
    uint32_t scalar_low_22;
    uint32_t scalar_low_23;
    uint32_t scalar_low_24;
    uint32_t scalar_low_25;
    uint32_t scalar_low_26;

    const uint32_t input_address = 0x1F800300;
    const uint32_t output_address = 0x1F800310;
    int16_t input[4];
    int16_t output[4];
    int32_t sine;
    int32_t cosine;
    uint32_t i;
    (void)sub_8005C338(r_u32(actor + 540), input_address);
    for (i = 0; i < 4; ++i)
        input[i] = r_s16(input_address + 2 * i);
    sine = ((scalar_address_222 = 0x8005624C + 4 * (((uint32_t)((uint32_t)(angle / 2))) & 0xFFF) + (((uint32_t)(0)) ? 2 : 0)), (((int32_t)(4 * (int32_t)r_s16(scalar_address_222)))));
    cosine = ((scalar_address_223 = 0x8005624C + 4 * (((uint32_t)((uint32_t)(angle / 2))) & 0xFFF) + (((uint32_t)(1)) ? 2 : 0)), (((int32_t)(4 * (int32_t)r_s16(scalar_address_223)))));
    if (!second_plane)
    {
        output[0] = (int16_t)(((scalar_low_11 = (uint32_t)((uint64_t)(uint32_t)((int32_t)(cosine)) * (uint32_t)((int32_t)(input[0])))), (((int32_t)(((int32_t)(uint32_t)(scalar_low_11)) >> ((uint32_t)(14)))))) + ((scalar_low_12 = (uint32_t)((uint64_t)(uint32_t)((int32_t)(sine)) * (uint32_t)((int32_t)(input[2])))), (((int32_t)(((int32_t)(uint32_t)(scalar_low_12)) >> ((uint32_t)(14)))))));
        output[1] = (int16_t)(((scalar_low_13 = (uint32_t)((uint64_t)(uint32_t)((int32_t)(cosine)) * (uint32_t)((int32_t)(input[1])))), (((int32_t)(((int32_t)(uint32_t)(scalar_low_13)) >> ((uint32_t)(14)))))) + ((scalar_low_14 = (uint32_t)((uint64_t)(uint32_t)((int32_t)(sine)) * (uint32_t)((int32_t)(input[3])))), (((int32_t)(((int32_t)(uint32_t)(scalar_low_14)) >> ((uint32_t)(14)))))));
        output[2] = (int16_t)(((scalar_low_15 = (uint32_t)((uint64_t)(uint32_t)((int32_t)(cosine)) * (uint32_t)((int32_t)(input[2])))), (((int32_t)(((int32_t)(uint32_t)(scalar_low_15)) >> ((uint32_t)(14)))))) - ((scalar_low_16 = (uint32_t)((uint64_t)(uint32_t)((int32_t)(sine)) * (uint32_t)((int32_t)(input[0])))), (((int32_t)(((int32_t)(uint32_t)(scalar_low_16)) >> ((uint32_t)(14)))))));
        output[3] = (int16_t)(((scalar_low_17 = (uint32_t)((uint64_t)(uint32_t)((int32_t)(cosine)) * (uint32_t)((int32_t)(input[3])))), (((int32_t)(((int32_t)(uint32_t)(scalar_low_17)) >> ((uint32_t)(14)))))) - ((scalar_low_18 = (uint32_t)((uint64_t)(uint32_t)((int32_t)(sine)) * (uint32_t)((int32_t)(input[1])))), (((int32_t)(((int32_t)(uint32_t)(scalar_low_18)) >> ((uint32_t)(14)))))));
    }
    else
    {
        output[0] = (int16_t)(((scalar_low_19 = (uint32_t)((uint64_t)(uint32_t)((int32_t)(cosine)) * (uint32_t)((int32_t)(input[0])))), (((int32_t)(((int32_t)(uint32_t)(scalar_low_19)) >> ((uint32_t)(14)))))) + ((scalar_low_20 = (uint32_t)((uint64_t)(uint32_t)((int32_t)(sine)) * (uint32_t)((int32_t)(input[3])))), (((int32_t)(((int32_t)(uint32_t)(scalar_low_20)) >> ((uint32_t)(14)))))));
        output[1] = (int16_t)(((scalar_low_21 = (uint32_t)((uint64_t)(uint32_t)((int32_t)(cosine)) * (uint32_t)((int32_t)(input[1])))), (((int32_t)(((int32_t)(uint32_t)(scalar_low_21)) >> ((uint32_t)(14)))))) - ((scalar_low_22 = (uint32_t)((uint64_t)(uint32_t)((int32_t)(sine)) * (uint32_t)((int32_t)(input[2])))), (((int32_t)(((int32_t)(uint32_t)(scalar_low_22)) >> ((uint32_t)(14)))))));
        output[2] = (int16_t)(((scalar_low_23 = (uint32_t)((uint64_t)(uint32_t)((int32_t)(cosine)) * (uint32_t)((int32_t)(input[2])))), (((int32_t)(((int32_t)(uint32_t)(scalar_low_23)) >> ((uint32_t)(14)))))) + ((scalar_low_24 = (uint32_t)((uint64_t)(uint32_t)((int32_t)(sine)) * (uint32_t)((int32_t)(input[1])))), (((int32_t)(((int32_t)(uint32_t)(scalar_low_24)) >> ((uint32_t)(14)))))));
        output[3] = (int16_t)(((scalar_low_25 = (uint32_t)((uint64_t)(uint32_t)((int32_t)(cosine)) * (uint32_t)((int32_t)(input[3])))), (((int32_t)(((int32_t)(uint32_t)(scalar_low_25)) >> ((uint32_t)(14)))))) - ((scalar_low_26 = (uint32_t)((uint64_t)(uint32_t)((int32_t)(sine)) * (uint32_t)((int32_t)(input[0])))), (((int32_t)(((int32_t)(uint32_t)(scalar_low_26)) >> ((uint32_t)(14)))))));
    }
    for (i = 0; i < 4; ++i)
        w_u16(output_address + 2 * i, (uint16_t)output[i]);
    (void)sub_8005C36C(r_u32(actor + 540), output_address);
}

uint32_t sub_80091468(uint32_t actor)
{
    uint32_t scalar_player_221;
    uint32_t scalar_count_221;

    FUNCTION_MARKER(0x80091468u, "RASHCDG.BIN");
    uint32_t attached = (r_u8(actor + 572) >> 5) & 1;
    uint32_t body = attached ? r_u32(r_u32(actor + 596) + 856) : r_u32(actor + 596);
    uint32_t mirrored;
    uint32_t options = 1;
    uint32_t duration = 10;
    uint32_t event;
    int32_t first_rotation = 0;
    int32_t second_rotation = 0;
    uint32_t value;
    if (attached)
        w_u32(body + 724, 0);
    w_u32(actor + 488, 0);
    w_u32(actor + 580, r_u32(0x800D3964));
    mirrored = (r_u32(actor + 552) >> 27) & 1;
    event = r_u16(actor + 544);
    switch (event)
    {
        case 38:
        {
            int32_t speed = ((int32_t)(uint32_t)(r_u32(actor + 480)));
            int32_t scale;
            w_u32(actor + 552, r_u32(actor + 552) & ~0x40000000u);
            if (speed < 196608)
                speed = 196608;
            w_u32(actor + 480, (uint32_t)speed);
            event = speed > 1310720 ? 48 : 51;
            options = (r_u32(actor + 552) & 0x08000000) ? 387 : 131;
            (void)sub_8008EE60(actor);
            race_select_launch_direction(actor, body);
            scale = event == 48 ? 55705 : 58982;
            value = (uint32_t)sub_8001FC90(scale, ((int32_t)(uint32_t)(r_u32(body + 480))));
            w_u32(actor + 480, value);
            (void)sub_8002EE50(value, xport_guest_ptr(actor + 450, 6), xport_guest_ptr(actor + 456, 12));
            race_copy_vector16(actor + 438, body + 522);
            race_copy_vector16(actor + 444, actor + 450);
            race_outer_product16(actor + 438, actor + 444, actor + 432);
            if (!sub_8002E468(xport_guest_ptr(actor + 432, 6)))
                (void)sub_8008CF74(actor);
            w_u32(actor + 600, 196608);
            w_u32(actor + 508, 0);
            break;
        }
        case 39:
        case 89:
        {
            int32_t angle;
            options = (mirrored ? 0x100u : 0) | 1u;
            event = 48;
            duration = attached ? 25 : 40;
            w_u32(actor + 552, r_u32(actor + 552) | 0x40000000u);
            (void)sub_8008E818(actor, 0);
            race_copy_vector16(actor + 432, body + 444);
            race_copy_vector16(actor + 444, body + 432);
            race_copy_vector16(actor + 438, body + 438);
            race_negate_vector16(actor + (mirrored ? 432 : 444));
            first_rotation = 1024;
            if (r_u32(body + 568) & 0x140)
                value = 327680;
            else
                value = (uint32_t)sub_8001FC90(78643, ((int32_t)(uint32_t)(r_u32(body + 480))));
            w_u32(actor + 480, value);
            race_select_launch_direction(actor, body);
            if (attached)
                race_tilt_launch_direction(actor);
            angle = (int32_t)sub_8001FF3C((uint32_t)((int32_t)r_s16(actor + 452) << 4));
            (void)sub_8007E868(actor + 450, value, (uint32_t)r_s16(0x8005624E + 4 * ((1137 - angle) & 0xFFF)), 406454);
            (void)sub_8002EE50(r_u32(actor + 480), xport_guest_ptr(actor + 450, 6), xport_guest_ptr(actor + 456, 12));
            (void)sub_8001A760(r_u16(body + 172), 1);
            if (!attached)
            {
                uint32_t record = ((scalar_player_221 = r_u16(((uint32_t)(body)) + 172)), (scalar_count_221 = r_u32(r_u32(0x8005B2F8) + 48)), (((uint32_t)(scalar_player_221 < scalar_count_221 ? 0x800CD898 + 1132 * scalar_player_221 : 0))));
                if (record)
                    w_u32(record + 548, r_u32(record + 548) | 0x06000080u);
            }
            w_u32(actor + 580, 65);
            break;
        }
        case 40:
        case 88:
        {
            int32_t angle;
            int32_t avoid = -1;
            options = 131;
            w_u32(actor + 552, r_u32(actor + 552) | 0x40000000u);
            (void)sub_8008E50C(actor);
            race_select_launch_direction(actor, body);
            if (attached)
                race_tilt_launch_direction(actor);
            if (r_u32(actor + 552) & 0x10000)
            {
                int32_t speed;
                (void)sub_8007E868(actor + 450, 0, (uint32_t)-2868, 0);
                speed = ((int32_t)(uint32_t)(r_u32(actor + 480))) / 3;
                if (speed < 327680)
                    speed = 327680;
                w_u32(actor + 480, (uint32_t)speed);
            }
            else
            {
                angle = (int32_t)sub_8001FF3C((uint32_t)((int32_t)r_s16(actor + 452) << 4));
                (void)sub_8007E868(actor + 450, r_u32(actor + 480), (uint32_t)r_s16(0x8005624E + 4 * ((1137 - angle) & 0xFFF)), 406454);
                second_rotation = (int32_t)sub_8001FF3C(sub_8002E698(xport_guest_ptr(actor + 444, 6), xport_guest_ptr(actor + 450, 6))) - 1024;
            }
            race_copy_vector16(actor + 438, body + 522);
            race_outer_product16(actor + 438, actor + 450, actor + 432);
            if (!sub_8002E468(xport_guest_ptr(actor + 432, 6)))
                (void)sub_8008CF74(actor);
            race_outer_product16(actor + 432, actor + 438, actor + 444);
            (void)sub_8002EE50(r_u32(actor + 480), xport_guest_ptr(actor + 450, 6), xport_guest_ptr(actor + 456, 12));
            value = r_u32(body + 852);
            if (r_u8(value + 572) & 0x10)
            {
                uint32_t candidate = attached ? r_u16(value + 544) : r_u16(r_u32(r_u32(body + 856) + 852) + 544);
                if (r_u16(0x800541D4 + 8 * candidate + 2) == 6)
                    avoid = (int32_t)candidate;
            }
            do
            {
                uint32_t remainder = sub_8001FC58() % 3;
                event = remainder == 0 ? 49 : 46 + (remainder == 1);
            } while ((int32_t)event == avoid);
            if (event == 47)
                duration = 20;
            (void)sub_8001A760(r_u16(body + 172), 1);
            if (!attached)
                race_mark_player_event(actor, body, 0);
            w_u32(actor + 580, 65);
            break;
        }
        case 41:
        case 90:
        {
            int32_t speed;
            options = 131;
            event = 43;
            w_u32(actor + 552, r_u32(actor + 552) | 0x40000000u);
            (void)sub_8008EB88(actor, 0);
            race_select_launch_direction(actor, body);
            race_copy_vector16(actor + 444, actor + 450);
            speed = ((int32_t)(uint32_t)(r_u32(body + 480)));
            if (speed <= 327680)
            {
                race_negate_vector16(actor + 450);
                speed = 327680 - speed;
            }
            else
                speed = (int32_t)sub_8001FC90(32768, speed);
            w_u32(actor + 480, (uint32_t)speed);
            (void)sub_8002EE50((uint32_t)speed, xport_guest_ptr(actor + 450, 6), xport_guest_ptr(actor + 456, 12));
            w_u16(actor + 432, r_u16(actor + 448));
            w_u16(actor + 434, 0);
            w_u16(actor + 436, (uint16_t)-r_s16(actor + 444));
            if (!sub_8002E468(xport_guest_ptr(actor + 432, 6)))
                (void)sub_8008CF74(actor);
            race_outer_product16(actor + 444, actor + 432, actor + 438);
            (void)sub_8001A760(r_u16(body + 172), 1);
            if (!attached)
                race_mark_player_event(actor, body, 0x200);
            break;
        }
        case 42:
        case 91:
        {
            int32_t angle;
            options = 131;
            w_u32(actor + 552, r_u32(actor + 552) | 0x40000000u);
            (void)sub_8008E044(actor, 0);
            (void)sub_8002EE50(r_u32(actor + 480), xport_guest_ptr(actor + 450, 6), xport_guest_ptr(actor + 456, 12));
            race_copy_vector16(actor + 438, body + 522);
            race_copy_vector16(actor + 444, actor + 450);
            race_outer_product16(actor + 438, actor + 444, actor + 432);
            (void)sub_8002E468(xport_guest_ptr(actor + 432, 6));
            race_outer_product16(actor + 444, actor + 432, actor + 438);
            angle = 1024 - (int32_t)sub_8001FF3C(sub_8002E698(xport_guest_ptr(actor + 450, 6), xport_guest_ptr(body + 444, 6)));
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
            (void)sub_8001A760(r_u16(body + 172), 1);
            if (!attached)
                race_mark_player_event(actor, body, 0);
            break;
        }
        default:
            break;
    }
    (void)sub_8008DF74(actor);
    if (!attached)
    {
        uint32_t player = r_u16(body + 172);
        if (player < r_u32(r_u32(0x8005B2F8) + 48))
            (void)sub_800235B0(player, actor);
    }
    if (first_rotation)
        race_rotate_mount_pair(actor, first_rotation, 0);
    if (second_rotation)
        race_rotate_mount_pair(actor, second_rotation, 1);
    (void)sub_800C4550(event & 0xFFFF, actor, options | 0x800u | (duration << 16));
    (void)sub_8009246C(actor);
    (void)sub_8008BD2C(actor);
    w_u8(actor + 535, 0);
    value = (r_u32(actor + 552) | 1u) & 0xFFD8FFFFu;
    w_u32(actor + 552, value);
    return value;
}

static void race_send_body_event(uint32_t body, uint16_t type, uint32_t mode)
{
    const uint32_t packet = 0x1F800300;
    w_u16(packet, type);
    w_u16(packet + 2, 224);
    (void)sub_800BCA68(xport_guest_ptr(packet, 8), mode, body);
}

uint32_t sub_80090D84(uint32_t actor)
{
    int32_t scalar_value_162;
    int32_t scalar_sign_162;
    int32_t scalar_value_163;
    int32_t scalar_sign_163;
    int32_t scalar_value_164;
    int32_t scalar_sign_164;

    FUNCTION_MARKER(0x80090D84u, "RASHCDG.BIN");
    uint32_t active = 1;
    uint32_t body;
    uint32_t attached;
    uint32_t flags;
    uint32_t value;
    if (r_u32(actor + 604) >= 2)
        goto finish;
    body = r_u32(actor + 596);
    attached = (r_u8(actor + 572) >> 5) & 1;
    w_u8(r_u32(body + 1084) + 15, 0);
    flags = r_u32(body + 568);
    if (!(flags & 0x7FF))
    {
        uint32_t actor_flags = r_u32(actor + 552);
        if (actor_flags & 0x60000)
            (void)sub_800C4550(((0u - attached) & 0x31) + 42, actor, (actor_flags & 0x40000) ? 0x100 : 0);
        else
            (void)sub_800C4550(((0u - attached) & 0x31) + 41, actor, 0);
    }
    else if ((flags & 0x18000) || ((flags & 0x400) && (r_u32(actor + 552) & 0x60000)))
        (void)sub_800C4550(42, actor, (r_u32(actor + 552) & 0x40000) ? 0x100 : 0);
    else if (flags & 0x20)
    {
        uint32_t variant = ((int32_t)(uint32_t)(r_u32(body + 676))) < 0 || ((flags & 0x40) && ((int32_t)(uint32_t)(r_u32(body + 652))) > 0);
        (void)sub_800C4550(((0u - attached) & 0x32) + 39, actor, variant << 8);
        active = 0;
    }
    else if (flags & 0x140)
    {
        (void)sub_800C4550(((0u - attached) & 0x33) + 38, actor, ((uint32_t)((int32_t)(uint32_t)(r_u32(body + 676))) >> 31) << 8);
        active = 0;
    }
    else if ((flags & 0x680) && !(flags & 0x40000))
    {
        (void)sub_800C4550(((0u - attached) & 0x31) + 41, actor, 0);
        active = (flags & 0x480) != 0;
    }
    else
    {
        uint32_t packed = r_u16(actor + 172);
        uint32_t global = r_u32(0x8005B2F8);
        (void)sub_800C4550(((0u - attached) & 0x30) + 40, actor, 0);
        if (!(flags & 0x20000))
            w_u32(actor + 480, (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(actor + 480))), 49152));
        if (((int32_t)(uint32_t)(r_u32(actor + 480))) < 327680)
            w_u32(actor + 480, 327680);
        race_copy_halfwords(actor + 432, body + 432, 9);
        w_u32(body + 568, flags & 0xFFF9FFFFu);
        active = 0;
        if ((packed >> 5) == 1 && (packed & 0x1F) < r_u32(global + 48))
        {
            uint32_t player_body = attached ? r_u32(body + 856) : body;
            uint32_t record = 0x800CD898 + 1132 * r_u16(player_body + 172);
            uint32_t choice;
            uint32_t linked;
            w_u32(record + 548, r_u32(record + 548) & 0xFFFD7FFFu);
            if (!r_u32(record + 772))
            {
                linked = r_u32(body + 828);
                if (linked && (r_u16(linked) >> 5) == 8 && (r_u16(linked + 2) & 0x200))
                    choice = 1;
                else
                {
                    choice = 0;
                    if (!(r_u32(actor + 552) & 0x10000) && ((scalar_value_162 = (((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(body + 820, 6), xport_guest_ptr(r_u32(body + 340) + 14, 6)))))), (scalar_sign_162 = scalar_value_162 >> 31), (((uint32_t)((uint32_t)((scalar_sign_162 + scalar_value_162) ^ scalar_sign_162))))) > 56753)
                    {
                        uint32_t random = sub_8001FC58() % 100;
                        if (random >= 50)
                            choice = 2;
                    }
                }
                if (choice == 1)
                    w_u32(record + 548, r_u32(record + 548) | 0x8000);
                else if (choice == 2)
                {
                    w_u32(record + 792, 0);
                    w_u32(record + 548, r_u32(record + 548) | ((flags & 4) ? 0x80020000u : 0x20000u));
                }
            }
        }
    }
    if (!active)
        (void)sub_800BF51C(body);
    (void)sub_8002EAD8(xport_guest_ptr(body + 184, 12), xport_guest_ptr(body + 438, 6), (uint32_t)-65536, xport_guest_ptr(actor + 184, 12));
    w_u32(actor + 468, r_u32(actor + 184));
    w_u32(actor + 472, r_u32(actor + 188));
    w_u32(actor + 476, r_u32(actor + 192));
    (void)sub_800BCD10(body);
    value = r_u32(body + 1084);
    race_send_body_event(body, (uint16_t)((0u - (uint32_t)(r_u32(value + 40) || r_u8(value + 39) >= 0xF8)) & 0xFFFE) + 4, 1);
    value = r_u32(body + 564);
    w_u32(body + 924, 0);
    w_u32(body + 916, 0);
    w_u32(body + 920, 0);
    w_u32(body + 564, value & 0xFFF7FFFFu);
    race_send_body_event(body, 0, 0);
    if (!attached)
    {
        uint32_t player = r_u16(body + 172);
        if (player < r_u32(r_u32(0x8005B2F8) + 48))
            (void)sub_80018440(player, 1);
    }
    if (r_u32(actor + 552) & 0x10000)
        (void)sub_80091468(actor);
    value = r_u32(0x8005B2F8);
    if ((r_u8(value + 4) & 1) && (r_u8(r_u32(body + 1084) + 1) & 0xF) != 2 && r_u8(value + 4) != 33)
    {
        uint32_t count = r_u32(value + 48);
        uint32_t i;
        for (i = 0; i < count; ++i)
        {
            uint32_t other = r_u32(0x8005B268 + 4 * i);
            uint32_t state = r_u32(other + 1084);
            if ((r_u8(state + 1) & 0xF) == 2 && r_u32(0x8005B1F8) >= r_u8(r_u32(body + 1084) + 39))
            {
                uint32_t x = ((scalar_value_163 = ((int32_t)r_s16(other + 186) - r_s16(body + 186))), (scalar_sign_163 = scalar_value_163 >> 31), (((uint32_t)((uint32_t)((scalar_sign_163 + scalar_value_163) ^ scalar_sign_163)))));
                uint32_t z = ((scalar_value_164 = ((int32_t)r_s16(other + 194) - r_s16(body + 194))), (scalar_sign_164 = scalar_value_164 >> 31), (((uint32_t)((uint32_t)((scalar_sign_164 + scalar_value_164) ^ scalar_sign_164)))));
                uint32_t major = x < z ? z : x;
                uint32_t minor = x < z ? x : z;
                uint32_t diagonal = minor + (minor >> 1);
                uint32_t distance = major - (major >> 5) - (major >> 7) + (diagonal >> 2) + (diagonal >> 6);
                uint32_t threshold_index = r_u32(value + 60);
                int32_t threshold = r_s16(0x8005309E + 4 * threshold_index);
                if (((int32_t)(uint32_t)(r_u32(other + 480))) > 585944 && ((int32_t)(uint32_t)(distance)) < threshold)
                    (void)sub_80096F30(other, body, 9);
            }
        }
    }
finish:
    value = r_u32(actor + 552) & 0xFFFF7FFFu;
    w_u32(actor + 552, value);
    return value;
}

static void race_resolve_launch_state(uint32_t actor)
{
    uint32_t crashed = (r_u32(actor + 552) & 0x8000) != 0;
    uint32_t type;
    if (crashed)
        (void)sub_80090D84(actor);
    type = r_u16(actor + 544);
    if (type == 89 || type == 39 || type == 88 || type == 40 || type == 38)
    {
        if (crashed)
            w_u32(actor + 552, r_u32(actor + 552) | 0x10000u);
        (void)sub_80091468(actor);
    }
}

uint32_t sub_80072994(uint32_t actor)
{
    FUNCTION_MARKER(0x80072994u, "RASHCDG.BIN");
    uint32_t linked = r_u32(actor + 852);
    uint32_t flags;
    uint32_t value;
    w_u32(actor + 932, 0);
    w_u32(actor + 564, r_u32(actor + 564) & 0xC0018200u);
    if (r_u8(linked + 572) & 0x10)
        race_resolve_launch_state(r_u32(r_u32(actor + 856) + 852));
    race_resolve_launch_state(linked);
    flags = r_u32(actor + 568) & 0xFFFC7F1Fu;
    w_u32(actor + 724, 0);
    w_u32(actor + 568, flags);
    if (flags & 0x10)
    {
        int32_t direction = ((int32_t)(uint32_t)(r_u32(actor + 676))) > 0 ? -196608 : 196608;
        int32_t lateral = ((int32_t)(uint32_t)(r_u32(actor + 636))) > 0 ? -65536 : 65536;
        w_u32(actor + 688, 0);
        w_u32(actor + 680, 0);
        w_u32(actor + 684, (uint32_t)direction);
        w_u32(actor + 644, 0);
        w_u32(actor + 688, r_u32(actor + 676));
        w_u32(actor + 640, (uint32_t)lateral);
        if (flags & 0x600)
            flags &= ~0x10u;
        w_u32(actor + 568, flags);
    }
    else if (flags & 0xF)
    {
        if ((flags & 2) && r_u32(linked + 604) >= 2)
        {
            flags = (flags & 0xFFFFFFF9u) | 4u;
            w_u32(actor + 568, flags);
        }
        if (flags & 0x600)
            w_u32(actor + 744, r_u32(actor + 488));
        else
        {
            (void)sub_8002EAD8(xport_guest_ptr(actor + 504, 12), xport_guest_ptr(actor + 528, 6), r_u32(actor + 308), xport_guest_ptr(actor + 784, 12));
            if (r_u32(actor + 552) && r_u32(actor + 488))
            {
                int32_t scale = (int32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(actor + 488))), ((int32_t)(uint32_t)(r_u32(actor + 552))));
                int32_t angle = -(int32_t)(((int64_t)652 * scale) >> 16);
                (void)sub_8002ED94(xport_guest_ptr(actor + 528, 6), xport_guest_ptr(actor + 516, 6), (uint32_t)angle);
            }
            if (!(r_u32(actor + 568) & 0xC))
                w_u32(actor + 724, r_u32(actor + 552));
        }
        w_u32(actor + 656, 0);
        w_u32(actor + 660, 0);
        w_u32(actor + 680, 0);
        w_u32(actor + 684, 0);
        w_u32(actor + 584, 0);
    }
    value = r_u32(actor + 568) & 0xFFFFEFFFu;
    w_u32(actor + 568, value);
    return value;
}

uint32_t sub_80071BCC(uint32_t actor)
{
    FUNCTION_MARKER(0x80071BCCu, "RASHCDG.BIN");
    uint32_t list;
    uint32_t flags;
    if (!r_s16(actor + 320))
        list = 0x8005B270;
    else
    {
        flags = r_u32(actor + 568);
        if (flags & 0x600)
        {
            uint32_t state = flags & 0x1FF;
            if (flags & 0x800)
            {
                (void)sub_80071D24(actor);
                flags = r_u32(actor + 568);
                state = flags & 0x1FF;
            }
            if (state && (flags & 0x1000))
                (void)sub_80072994(actor);
            list = (r_u32(actor + 568) & 0x400) ? 0x8005B2D8 : 0x8005B378;
        }
        else if (flags & 0x1FF)
        {
            if (flags & 0x800)
                (void)sub_800723FC(actor);
            if (r_u32(actor + 568) & 0x1000)
                (void)sub_80072994(actor);
            list = 0x8005B350;
        }
        else
        {
            (void)sub_8007F08C(actor);
            list = 0x8005B298;
        }
    }
    w_u32(r_u32(actor + 1088) + 4, r_u32(actor + 1092));
    w_u32(r_u32(actor + 1092), r_u32(actor + 1088));
    w_u32(r_u32(list + 4), actor + 1088);
    w_u32(actor + 1092, r_u32(list + 4));
    w_u32(list + 4, actor + 1088);
    w_u32(actor + 1088, list);
    flags = r_u32(actor + 568) & 0xF7FFFFFFu;
    w_u32(actor + 568, flags);
    return flags;
}

static uint32_t race_steering_ratio(uint32_t numerator, uint32_t denominator)
{
    uint32_t n = ((int32_t)(uint32_t)(numerator)) > 0 ? numerator : 0u - numerator;
    uint32_t d = ((int32_t)(uint32_t)(denominator)) > 0 ? denominator : 0u - denominator;
    uint32_t value = sub_80010028(n, d);
    return (((int32_t)(uint32_t)(numerator)) > 0) != (((int32_t)(uint32_t)(denominator)) > 0) ? 0u - value : value;
}

static uint32_t race_steering_angle(uint32_t forward, uint32_t direction)
{
    uint32_t scalar_sign_234;
    uint32_t scalar_sign_235;

    int64_t scalar_product_224;

    uint32_t octant, x, y, ratio, address, next, base, step, value;
    if (!forward && !direction)
        return 0;
    octant = (uint32_t)(((int32_t)(uint32_t)(direction)) >> 31) & 2u;
    if (((int32_t)(uint32_t)(forward)) < 0)
        octant |= 4u;
    x = ((scalar_sign_234 = (uint32_t)(((int32_t)(uint32_t)(((uint32_t)(forward)))) >> 31)), (((uint32_t)((((uint32_t)(forward)) + scalar_sign_234) ^ scalar_sign_234))));
    y = ((scalar_sign_235 = (uint32_t)(((int32_t)(uint32_t)(((uint32_t)(direction)))) >> 31)), (((uint32_t)((((uint32_t)(direction)) + scalar_sign_235) ^ scalar_sign_235))));
    if (((int32_t)(uint32_t)(y)) < ((int32_t)(uint32_t)(x)))
    {
        octant |= 1u;
        ratio = race_steering_ratio(y, x);
    }
    else
        ratio = race_steering_ratio(x, y);
    address = 0x8005285Cu + 4u * (uint32_t)(((int32_t)(uint32_t)(ratio)) >> 12);
    next = r_u32(address + 4);
    base = r_u32(address);
    step = ((next - base) << 4) + 8u;
    value = (uint32_t)(((int32_t)(uint32_t)(((scalar_product_224 = (int64_t)((int32_t)(uint32_t)(((uint32_t)(ratio & 4095u)))) * ((int32_t)(uint32_t)(((uint32_t)(step))))), (((uint32_t)((uint32_t)((uint64_t)scalar_product_224 >> 16))))) + base)) >> 20);
    switch (octant)
    {
        case 0:
            return value;
        case 1:
            return 1024u - value;
        case 2:
            return 2048u - value;
        case 3:
            return value + 1024u;
        case 4:
            return 0u - value;
        case 5:
            return value - 1024u;
        case 6:
            return value - 2048u;
        case 7:
            return 0xFFFFFC00u - value;
        default:
            return 0;
    }
}

uint32_t sub_80072FB4(uint32_t actor)
{
    uint32_t scalar_value_236;
    uint32_t scalar_sign_236;

    int64_t scalar_product_225;
    int64_t scalar_product_226;
    int64_t scalar_product_227;
    int64_t scalar_product_228;
    int64_t scalar_product_229;
    int64_t scalar_product_230;
    int64_t scalar_product_231;
    int64_t scalar_product_232;
    int64_t scalar_product_233;

    uint32_t flags, specification, mirrored, input_enabled, speed, actual_speed, linked;
    uint32_t previous, steering = 0, enabled = 1;

    FUNCTION_MARKER(0x80072FB4u, "RASHCDG.BIN");
    flags = r_u32(actor + 560);
    actual_speed = r_u32(actor + 480);
    speed = r_u32(actor + 576);
    specification = r_u32(actor + 556);
    mirrored = (flags >> 27) & 1u;
    input_enabled = (flags >> 20) & 1u;
    if (r_u32(actor + 568) & 0x600u)
        speed = actual_speed;
    linked = r_u32(actor + 856);
    previous = r_u32(linked ? linked + 488 : actor + 488);
    if (!mirrored)
    {
        if (input_enabled)
        {
            uint32_t low = r_u32(specification + 356);
            uint32_t player_scale = r_u32(0x800CE540u + 8u * r_u16(actor + 172) + 4);
            uint32_t scale;
            if (((int32_t)(uint32_t)(speed)) < ((int32_t)(uint32_t)(low)))
                scale = r_u32(specification + 388);
            else if (((int32_t)(uint32_t)(speed)) > ((int32_t)(uint32_t)(r_u32(specification + 360))))
                scale = r_u32(specification + 392);
            else
            {
                uint32_t slope = r_u32(specification + 396);
                uint32_t base = r_u32(specification + 388);
                scale = ((scalar_product_225 = (int64_t)((int32_t)(uint32_t)(((uint32_t)(speed - low)))) * ((int32_t)(uint32_t)(((uint32_t)(slope))))), (((uint32_t)((uint32_t)((uint64_t)scalar_product_225 >> 16))))) + base;
            }
            steering = ((scalar_product_226 = (int64_t)((int32_t)(uint32_t)(((uint32_t)(scale)))) * ((int32_t)(uint32_t)(((uint32_t)(player_scale))))), (((uint32_t)((uint32_t)((uint64_t)scalar_product_226 >> 16)))));
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
            uint32_t target = r_u32(actor + 880 + 4u * i);
            uint32_t position = r_u32(actor + 504 + 4u * i);
            delta[i] = target - position;
        }
        for (i = 0; i < 3; ++i)
            forward += ((scalar_product_227 = (int64_t)((int32_t)(uint32_t)(((uint32_t)(delta[i])))) * ((int32_t)(uint32_t)(((uint32_t)((uint32_t)r_s16(actor + 814 + 2u * i) << 4))))), (((uint32_t)((uint32_t)((uint64_t)scalar_product_227 >> 16)))));
        for (i = 0; i < 3; ++i)
            distance += ((scalar_product_228 = (int64_t)((int32_t)(uint32_t)(((uint32_t)(delta[i])))) * ((int32_t)(uint32_t)(((uint32_t)(delta[i]))))), (((uint32_t)((uint32_t)((uint64_t)scalar_product_228 >> 16)))));
        flags = r_u32(actor + 560);
        distance = (uint32_t)(((int32_t)(uint32_t)(distance)) >> 1);
        if (!(flags & 0x40000u) && ((int32_t)(uint32_t)(distance)) >= 17 && ((int32_t)(uint32_t)(distance)) <= 536838143)
        {
            uint32_t raw = ((scalar_product_229 = (int64_t)((int32_t)(uint32_t)(((uint32_t)(race_steering_ratio(forward, distance))))) * ((int32_t)(uint32_t)(((uint32_t)(speed))))), (((uint32_t)((uint32_t)((uint64_t)scalar_product_229 >> 16)))));
            uint32_t lower_delta, upper_delta;
            limit = r_u32(specification + 232);
            lower_delta = raw + limit;
            upper_delta = limit - raw;
            steering = raw + ((uint32_t)(((int32_t)(uint32_t)(lower_delta)) >> 31) & (0u - limit - raw)) + ((uint32_t)(((int32_t)(uint32_t)(upper_delta)) >> 31) & upper_delta);
        }
        for (i = 0; i < 3; ++i)
            direction += ((scalar_product_230 = (int64_t)((int32_t)(uint32_t)(((uint32_t)(delta[i])))) * ((int32_t)(uint32_t)(((uint32_t)((uint32_t)r_s16(actor + 450 + 2u * i) << 4))))), (((uint32_t)((uint32_t)((uint64_t)scalar_product_230 >> 16)))));
        limit = r_u32(specification + 224);
        slow = ((int32_t)(uint32_t)(r_u32(actor + 480))) < (((int32_t)(uint32_t)(limit)) >> 1);
        angle = ((scalar_value_236 = (race_steering_angle(forward, direction))), (scalar_sign_236 = (uint32_t)(((int32_t)(uint32_t)(scalar_value_236)) >> 31)), (((uint32_t)((scalar_value_236 + scalar_sign_236) ^ scalar_sign_236))));
        if (((int32_t)(uint32_t)(direction)) <= 0 || ((r_u32(actor + 560) & 0x400000u) && ((int32_t)(uint32_t)(angle)) >= 171))
        {
            if (slow)
            {
                steering = r_u32(specification + 232);
                if (((int32_t)(uint32_t)(forward)) < 0)
                    steering = 0u - steering;
            }
            w_u32(actor + 560, r_u32(actor + 560) | 0x400000u);
        }
        else if (r_u32(actor + 560) & 0x400000u)
            w_u32(actor + 560, (r_u32(actor + 560) | 0x01000000u) & 0xFFBFFFFFu);
    }
    if (enabled)
    {
        uint32_t mask = ((int32_t)(uint32_t)(steering)) < ((int32_t)(uint32_t)(previous)) ? 0x100u : ((int32_t)(uint32_t)(previous)) < ((int32_t)(uint32_t)(steering)) ? 0x200u : 0;
        uint32_t equal = mask == 0;
        uint32_t changed = 0;
        uint32_t clear_mask = 0x300u;
        if (mirrored && equal)
            mask = 0x100u;
        if (!equal)
            changed = (r_u32(actor + 560) & mask) == 0;
        flags = r_u32(actor + 560) | mask | (changed << 7);
        w_u32(actor + 560, flags);
        if (mask == 0x200u)
            clear_mask = 0x100u;
        if (mask == 0x100u)
            clear_mask -= 0x100u;
        w_u32(actor + 560, flags & ~clear_mask);
        flags = r_u32(actor + 560) | 0x200000u;
    }
    else
        flags = r_u32(actor + 560) & 0xFFDFFFFFu;
    w_u32(actor + 560, flags);
    flags = r_u32(actor + 560);
    w_u32(actor + 196, steering);
    {
        uint32_t target = 0x7FFFFFFFu;
        if (flags & 0x100u)
        {
            if (!(flags & 0x200u))
                target = r_u32(actor + 636);
        }
        else if (flags & 0x200u)
            target = 0u - r_u32(actor + 636);
        if (target == 0x7FFFFFFFu)
            flags = r_u32(actor + 560) | 0x8000u;
        else
        {
            uint32_t low, acceleration;
            if (r_u32(actor + 560) & 0x80u)
            {
                uint32_t physics = r_u32(actor + 564);
                physics = ((int32_t)(uint32_t)(target)) > 0 ? physics | 2u : physics & 0xFFFFFFFDu;
                w_u32(actor + 564, physics);
                w_u32(actor + 560, r_u32(actor + 560) & 0xFFFFFF7Fu);
            }
            low = r_u32(specification + 356);
            if (((int32_t)(uint32_t)(speed)) < ((int32_t)(uint32_t)(low)))
                acceleration = 0u - r_u32(specification + 376);
            else if (((int32_t)(uint32_t)(speed)) > ((int32_t)(uint32_t)(r_u32(specification + 360))))
                acceleration = 0u - r_u32(specification + 380);
            else
            {
                uint32_t slope = r_u32(specification + 384);
                uint32_t base = r_u32(specification + 376);
                acceleration = 0u - base - ((scalar_product_231 = (int64_t)((int32_t)(uint32_t)(((uint32_t)(speed - low)))) * ((int32_t)(uint32_t)(((uint32_t)(slope))))), (((uint32_t)((uint32_t)((uint64_t)scalar_product_231 >> 16)))));
            }
            w_u32(actor + 584, acceleration);
            if (r_u32(actor + 560) & 0x200u)
                w_u32(actor + 584, 0u - r_u32(actor + 584));
            if (r_u32(actor + 564) & 2u)
            {
                uint32_t offset = r_u32(specification + 292);
                if (((int32_t)(uint32_t)(0u - offset)) < ((int32_t)(uint32_t)(target)))
                {
                    uint32_t slope = r_u32(specification + 288);
                    uint32_t current = r_u32(actor + 584);
                    uint32_t weight = ((scalar_product_232 = (int64_t)((int32_t)(uint32_t)(((uint32_t)(target + offset)))) * ((int32_t)(uint32_t)(((uint32_t)(slope))))), (((uint32_t)((uint32_t)((uint64_t)scalar_product_232 >> 16))))) + 65536u;
                    w_u32(actor + 584, ((scalar_product_233 = (int64_t)((int32_t)(uint32_t)(((uint32_t)(weight)))) * ((int32_t)(uint32_t)(((uint32_t)(current))))), (((uint32_t)((uint32_t)((uint64_t)scalar_product_233 >> 16))))));
                }
            }
            flags = r_u32(actor + 560) & 0xFFFF7FFFu;
        }
    }
    w_u32(actor + 560, flags);
    return flags;
}

uint32_t sub_80074170(uint32_t actor)
{
    FUNCTION_MARKER(0x80074170u, "RASHCDG.BIN");
    uint32_t specification = r_u32(actor + 556);
    int32_t speed = (int32_t)r_u32(actor + 576);
    int32_t first = (int32_t)((uint32_t)(((int64_t)(int32_t)r_u32(specification + 280) * speed) >> 16) + 65536);
    int32_t second = (int32_t)((uint32_t)(((int64_t)(int32_t)r_u32(specification + 284) * speed) >> 16) + 65536);
    uint32_t quotient = sub_80010028(first > 0 ? (uint32_t)first : 0 - (uint32_t)first, second > 0 ? (uint32_t)second : 0 - (uint32_t)second);
    int32_t ratio = (int32_t)(((first > 0) != (second > 0)) ? 0 - quotient : quotient);
    int32_t projection = (int32_t)(((int64_t)ratio * (int32_t)r_u32(actor + 488)) >> 16);
    int32_t target;
    uint32_t magnitude;
    uint32_t quadrant;
    uint32_t table;
    uint32_t base;
    int32_t angle;
    uint32_t result;
    uint32_t limit;
    uint32_t lower_delta;
    uint32_t upper_delta;
    uint32_t clamped;
    if (speed >= 6554)
    {
        uint32_t divisor = (uint32_t)(speed >> 1) + (uint32_t)((int32_t)((uint32_t)speed - 2) >> 31);
        int32_t reciprocal = (int32_t)(divisor ? 0x80000000u / divisor : 0xFFFFFFFF);
        int32_t value = (int32_t)(((int64_t)projection * reciprocal) >> 16);
        uint32_t product = (uint32_t)(((int64_t)value * (int32_t)r_u32(specification + 4)) >> 16);
        limit = r_u32(specification + 240);
        lower_delta = product + limit;
        upper_delta = limit - product;
        clamped = product + ((uint32_t)((int32_t)lower_delta >> 31) & (0 - lower_delta)) + ((uint32_t)((int32_t)upper_delta >> 31) & upper_delta);
        w_u16(actor + 826, (uint16_t)((163 * clamped) >> 14));
    }
    target = (int32_t)((uint32_t)(((int64_t)(int32_t)(r_u32(actor + 488) - (uint32_t)projection) * (int32_t)r_u32(actor + 576)) >> 16) - r_u32(actor + 732));
    second = (int32_t)r_u32(actor + 736);
    quotient = sub_80010028(target > 0 ? (uint32_t)target : 0 - (uint32_t)target, second > 0 ? (uint32_t)second : 0 - (uint32_t)second);
    ratio = (int32_t)(((target > 0) != (second > 0)) ? 0 - quotient : quotient);
    quadrant = (uint32_t)(ratio >> 31) & 2;
    magnitude = ((uint32_t)(ratio >> 31) + (uint32_t)ratio) ^ (uint32_t)(ratio >> 31);
    if ((int32_t)magnitude > 65536)
    {
        uint32_t divisor = (uint32_t)((int32_t)magnitude >> 1) + (uint32_t)((int32_t)(magnitude - 2) >> 31);
        quadrant |= 1;
        magnitude = divisor ? 0x80000000u / divisor : 0xFFFFFFFF;
    }
    table = 0x8005285C + 4 * (uint32_t)((int32_t)magnitude >> 12);
    base = r_u32(table);
    {
        int32_t step = (int32_t)(((r_u32(table + 4) - base) << 4) + 8);
        uint32_t interpolated = (uint32_t)(((int64_t)(magnitude & 4095) * step) >> 16) + base;
        angle = (int32_t)interpolated >> 20;
    }
    if (quadrant == 1)
        angle = (int32_t)(1024 - (uint32_t)angle);
    else if (quadrant == 2)
        angle = (int32_t)(0 - (uint32_t)angle);
    else if (quadrant == 3)
        angle = (int32_t)((uint32_t)angle - 1024);
    result = (uint32_t)((int32_t)(25736 * (uint32_t)angle) >> 8) - r_u32(actor + 668);
    w_u32(actor + 636, result);
    limit = r_u32(specification + 228);
    lower_delta = result + limit;
    upper_delta = limit - result;
    clamped = result + ((uint32_t)((int32_t)lower_delta >> 31) & (0 - lower_delta)) + ((uint32_t)((int32_t)upper_delta >> 31) & upper_delta);
    if (clamped != result)
    {
        w_u32(actor + 636, clamped);
        return sub_80073D74(actor);
    }
    return result;
}

static int32_t race_clamp_symmetric(int32_t value, int32_t limit)
{
    if (value < -limit)
        return -limit;
    if (value > limit)
        return limit;
    return value;
}

uint32_t sub_80074570(uint32_t actor, uint32_t use_linked, int32_t correction, int32_t delta)
{
    int32_t scalar_angle_237;
    uint32_t scalar_index_237;
    int32_t scalar_angle_238;
    uint32_t scalar_index_238;

    int32_t scalar_sign_165;
    int32_t scalar_sign_166;
    int32_t scalar_value_167;
    int32_t scalar_sign_167;

    FUNCTION_MARKER(0x80074570u, "RASHCDG.BIN");
    uint32_t flags = r_u32(actor + 560);
    uint32_t specification = r_u32(actor + 556);
    uint32_t physics_active = (r_u32(actor + 568) & 0x600) != 0;
    int32_t speed = ((int32_t)(uint32_t)(r_u32(actor + (physics_active ? 480 : 576))));
    uint32_t mirrored = (flags >> 27) & 1;
    if ((flags & 0x108000) == 0x8000)
    {
        int32_t phase;
        int32_t next;
        uint32_t result;
        if (flags & 0x80)
        {
            w_u32(actor + 712, (flags & 0x10000) ? 0 : 0u - r_u32(specification + 416));
            w_u32(actor + 584, 0);
            flags &= ~0x80u;
            w_u32(actor + 560, flags);
            w_u32(actor + 564, r_u32(actor + 564) & ~2u);
        }
        phase = ((int32_t)(uint32_t)(r_u32(actor + 712)));
        next = phase + delta;
        if (phase < 0)
        {
            if (next > 0)
                next = 0;
            w_u32(actor + 712, (uint32_t)next);
        }
        else if (r_u32(actor + 636))
        {
            if (!phase)
            {
                int32_t low = ((int32_t)(uint32_t)(r_u32(specification + 356)));
                int32_t high = ((int32_t)(uint32_t)(r_u32(specification + 360)));
                int32_t divisor;
                int32_t yaw = ((int32_t)(uint32_t)(r_u32(actor + 636)));
                if (speed < low)
                    divisor = ((int32_t)(uint32_t)(r_u32(specification + 364)));
                else if (speed <= high)
                    divisor = ((int32_t)(uint32_t)(r_u32(specification + 364))) + (int32_t)sub_8001FC90(speed - low, ((int32_t)(uint32_t)(r_u32(specification + 372))));
                else
                    divisor = ((int32_t)(uint32_t)(r_u32(specification + 368)));
                w_u32(actor + 644, (uint32_t)yaw);
                phase = race_leaf_signed_ratio(yaw < 0 ? (int32_t)(0u - (uint32_t)yaw) : yaw, divisor);
                w_u32(actor + 728, (uint32_t)phase);
                if (phase < 9830)
                {
                    flags |= 0x20000u;
                    w_u32(actor + 560, flags);
                }
            }
            next = ((int32_t)(uint32_t)(r_u32(actor + 712))) + delta;
            w_u32(actor + 712, (uint32_t)next);
            if (next >= ((int32_t)(uint32_t)(r_u32(actor + 728))))
                w_u32(actor + 636, 0);
            else
            {
                int32_t ratio = race_leaf_signed_ratio(next, ((int32_t)(uint32_t)(r_u32(actor + 728))));
                int32_t base;
                int32_t weight;
                flags = r_u32(actor + 560);
                if (flags & 0x20000)
                {
                    base = ((int32_t)(uint32_t)(r_u32(actor + 644)));
                    weight = 65536 - ratio;
                }
                else if (flags & 0x10000)
                {
                    base = ((int32_t)(uint32_t)(r_u32(actor + 644)));
                    weight = ((scalar_angle_237 = (int32_t)sub_8001FC90(((int32_t)(102943)), ((int32_t)(ratio)))), (scalar_index_237 = (((uint32_t)(163 * scalar_angle_237) >> 13) & 0x1FFE) + 1), (((int32_t)((int32_t)r_s16(0x8005624C + 2 * scalar_index_237) << 4))));
                }
                else
                {
                    base = ((int32_t)(uint32_t)(r_u32(actor + 644))) >> 1;
                    weight = ((scalar_angle_238 = (int32_t)sub_8001FC90(((int32_t)(205887)), ((int32_t)(ratio)))), (scalar_index_238 = (((uint32_t)(163 * scalar_angle_238) >> 13) & 0x1FFE) + 1), (((int32_t)((int32_t)r_s16(0x8005624C + 2 * scalar_index_238) << 4)))) + 65536;
                }
                w_u32(actor + 636, (uint32_t)sub_8001FC90(base, weight));
            }
        }
        (void)sub_80073D74(actor);
        result = (uint32_t)r_s16(specification + 446);
        if (r_s16(specification + 446))
            w_u32(actor + 488, 0);
        if (use_linked)
        {
            result = r_u32(actor + 488);
            w_u32(r_u32(actor + 856) + 488, result);
        }
        if (correction)
        {
            int32_t value = ((int32_t)(uint32_t)(r_u32(actor + 488))) + correction;
            value = race_clamp_symmetric(value, ((int32_t)(uint32_t)(r_u32(specification + 232))));
            w_u32(actor + 488, (uint32_t)value);
            return sub_80074170(actor);
        }
        return result;
    }
    else
    {
        int32_t previous = ((int32_t)(uint32_t)(r_u32(use_linked ? r_u32(actor + 856) + 488 : actor + 488)));
        int32_t limit;
        int32_t integrated;
        flags &= 0xFFFCFFFFu;
        w_u32(actor + 560, flags);
        if (r_u32(actor + 616) || physics_active)
        {
            int32_t scale = ((int32_t)(uint32_t)(r_u32(specification + 312)));
            int32_t minimum;
            w_u32(actor + 584, (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(actor + 584))), scale));
            minimum = (int32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(specification + 232))), scale);
            limit = (int32_t)((scalar_sign_165 = ((int32_t)(previous)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_165 + ((int32_t)(previous))) ^ scalar_sign_165)))));
            if (limit < minimum)
                limit = minimum;
        }
        else
        {
            limit = ((int32_t)(uint32_t)(r_u32(specification + 232)));
            if (use_linked && previous > 0)
                w_u32(actor + 584, (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(actor + 584))), ((int32_t)(uint32_t)(r_u32(specification + 308)))));
        }
        if ((r_u32(actor + 564) & 6) == 4)
            w_u32(actor + 584, (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(actor + 584))), ((int32_t)(uint32_t)(r_u32(specification + 316)))));
        integrated = previous + (int32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(actor + 584))), delta);
        if (flags & 0x200000)
        {
            int32_t target = ((int32_t)(uint32_t)(r_u32(actor + 196)));
            if ((previous < target && integrated >= target) || (previous >= target && integrated <= target))
                integrated = target;
        }
        if (mirrored)
        {
            int32_t target = ((int32_t)(uint32_t)(r_u32(actor + 196)));
            flags = r_u32(actor + 560);
            if (flags & 0x1000000)
            {
                integrated = target;
                flags &= ~0x1000000u;
                w_u32(actor + 560, flags);
            }
            if (flags & 0x400000)
                w_u32(actor + 924, 327680);
            else if (integrated != target && ((scalar_sign_166 = ((int32_t)(target)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_166 + ((int32_t)(target))) ^ scalar_sign_166))))) >= 26215 && !(r_u8(r_u32(actor + 1084)) & 1))
            {
                int32_t ratio = (int32_t)((scalar_value_167 = (race_leaf_signed_ratio(target - previous, delta))), (scalar_sign_167 = scalar_value_167 >> 31), (((uint32_t)((uint32_t)((scalar_sign_167 + scalar_value_167) ^ scalar_sign_167)))));
                int32_t minimum = ((int32_t)(uint32_t)(r_u32(specification + 356)));
                int32_t lower = ((int32_t)(uint32_t)(r_u32(specification + 376)));
                int32_t upper = ((int32_t)(uint32_t)(r_u32(specification + 380)));
                if (ratio > lower)
                    w_u32(actor + 924, (uint32_t)minimum);
                else if (ratio > upper)
                    w_u32(actor + 924, (uint32_t)(minimum + race_leaf_signed_ratio(ratio - lower, ((int32_t)(uint32_t)(r_u32(specification + 384))))));
                if (((int32_t)(uint32_t)(r_u32(actor + 924))) < (((int32_t)(uint32_t)(r_u32(specification + 224))) >> 1))
                    w_u32(actor + 924, (uint32_t)(((int32_t)(uint32_t)(r_u32(specification + 224))) >> 1));
            }
        }
        if (use_linked)
            w_u32(r_u32(actor + 856) + 488, (uint32_t)integrated);
        integrated = race_clamp_symmetric(integrated + correction, limit);
        w_u32(actor + 488, (uint32_t)integrated);
        return sub_80074170(actor);
    }
}

void sub_80074C84(uint32_t list, int32_t delta)
{
    FUNCTION_MARKER(0x80074C84u, "RASHCDG.BIN");
    uint32_t link = r_u32(list + 4);
    while (link != list)
    {
        uint32_t actor = link - 1088;
        uint32_t specification = r_u32(actor + 852);
        int32_t correction = 0;
        if (r_u8(specification + 572) & 0x10)
            correction = (int32_t)sub_80072C7C(actor, (uint32_t)delta);
        if (r_u32(actor + 560) & 0x80000)
        {
            uint32_t use_linked = 0;
            if (r_u32(actor + 856))
                use_linked = r_u32(actor + 1088) != 0;
            (void)sub_80074570(actor, use_linked, correction, delta);
        }
        link = r_u32(link + 4);
    }
}

void sub_80073874(uint32_t list, int32_t delta)
{
    int32_t scalar_value_239;
    int32_t scalar_value_240;
    int32_t scalar_value_241;
    int32_t scalar_value_242;
    int32_t scalar_value_243;
    int32_t scalar_value_244;
    int32_t scalar_value_245;

    int32_t scalar_sign_168;

    FUNCTION_MARKER(0x80073874u, "RASHCDG.BIN");
    uint32_t link = r_u32(list + 4);
    while (link != list)
    {
        uint32_t actor = link - 1088;
        uint32_t flags = r_u32(actor + 560);
        uint32_t mirrored = (flags >> 27) & 1;
        uint32_t physics_active = (r_u32(actor + 568) & 0x600) != 0;
        uint32_t race = r_u32(0x8005B2F8);
        uint32_t player = r_u16(actor + 172);
        flags &= ~0x100000u;
        w_u32(actor + 560, flags);
        if (player < r_u32(race + 48) && !mirrored)
        {
            uint32_t record = 0x800D7128 + 192 * player;
            if (r_u32(record + 16))
            {
                flags |= 0x100000u;
                w_u32(actor + 560, flags);
            }
            if ((r_u8(r_u32(actor + 852) + 572) & 0x10) && r_u32(race + 52) >= 2)
            {
                uint32_t index = 3 * (player + (r_u32(race + 48) < 2 ? 1 : 2));
                uint32_t linked = r_u32(actor + 856);
                uint32_t linked_flags = r_u32(linked + 560);
                if (r_u32(0x800D7128 + 64 * index + 16))
                    linked_flags |= 0x100000u;
                else
                    linked_flags &= ~0x100000u;
                w_u32(linked + 560, linked_flags);
            }
        }
        flags = r_u32(actor + 560) & ~0x80000u;
        w_u32(actor + 560, flags);
        if ((physics_active || (r_u32(actor + 564) & 0x40)) && (r_u32(actor + 568) & 0x1FF))
            goto next_actor;
        if (r_u32(actor + 720))
        {
            (void)sub_80073D74(actor);
            goto next_actor;
        }
        if (mirrored && ((flags >> 13) & 1))
        {
            uint32_t specification = r_u32(actor + 556);
            if (flags & 0x4000)
            {
                int32_t side = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(actor + 528, 6), xport_guest_ptr(actor + 820, 6))));
                int32_t forward = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(actor + 516, 6), xport_guest_ptr(actor + 820, 6))));
                int32_t angle = (int32_t)(((int64_t)25736 * ((int32_t)(uint32_t)(sub_80020018((uint32_t)forward, (uint32_t)-side)))) >> 8);
                int32_t difference = (angle > 0 ? 102943 : -102943) - angle;
                int32_t magnitude = (int32_t)((scalar_sign_168 = ((int32_t)(difference)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_168 + ((int32_t)(difference))) ^ scalar_sign_168)))));
                int32_t divisor = ((int32_t)(uint32_t)(r_u32(specification + 232)));
                w_u32(actor + 724, (uint32_t)race_leaf_signed_ratio(magnitude, divisor));
                w_u32(actor + 488, difference < 0 ? 0u - (uint32_t)divisor : (uint32_t)divisor);
                flags = r_u32(actor + 560) & ~0x4000u;
                w_u32(actor + 560, flags);
            }
            {
                int32_t remaining = ((int32_t)(uint32_t)(r_u32(actor + 724)));
                int32_t steering = 0;
                flags = r_u32(actor + 560);
                if (remaining <= 0)
                    flags &= ~0x2000u;
                else
                    steering = ((int32_t)(uint32_t)(r_u32(actor + 488)));
                w_u32(actor + 560, flags);
                w_u32(actor + 488, (uint32_t)steering);
                if (r_u32(actor + 856))
                    w_u32(r_u32(actor + 856) + 488, (uint32_t)steering);
                w_u32(actor + 724, (uint32_t)(remaining - delta));
                (void)sub_80074170(actor);
            }
            goto next_actor;
        }
        {
            uint32_t specification = r_u32(actor + 852);
            int32_t speed = ((int32_t)(uint32_t)(r_u32(actor + (physics_active ? 480 : 576))));
            if (speed > 0 || (r_u32(specification + 604) < 2 && (flags & 0x42) == 2))
            {
                if ((flags & 0x2000) && !(flags & 0x300))
                {
                    if (((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(actor + 820, 6), xport_guest_ptr(actor + 516, 6)))) <= 0)
                        flags |= 0x100;
                    else
                        flags |= 0x200;
                    flags &= ~0x2000u;
                    w_u32(actor + 560, flags);
                }
                (void)sub_80072FB4(actor);
                w_u32(actor + 560, r_u32(actor + 560) | 0x80000u);
            }
            else if (!(r_u32(actor + 564) & 0x18000000) || !(flags & 0x300))
            {
                uint32_t linked = r_u32(actor + 856);
                w_u32(actor + 488, (uint32_t)((scalar_value_239 = (((int32_t)(uint32_t)(r_u32(actor + 488))))), (((int32_t)((scalar_value_239 + (int32_t)((uint32_t)scalar_value_239 >> 31)) >> 1)))));
                w_u32(actor + 636, (uint32_t)((scalar_value_240 = (((int32_t)(uint32_t)(r_u32(actor + 636))))), (((int32_t)((scalar_value_240 + (int32_t)((uint32_t)scalar_value_240 >> 31)) >> 1)))));
                w_u32(actor + 672, (uint32_t)((scalar_value_241 = (((int32_t)(uint32_t)(r_u32(actor + 672))))), (((int32_t)((scalar_value_241 + (int32_t)((uint32_t)scalar_value_241 >> 31)) >> 1)))));
                w_u32(actor + 652, r_u32(actor + 668));
                w_u16(actor + 826, (uint16_t)((scalar_value_242 = ((int16_t)r_u16(actor + 826))), (((int32_t)((scalar_value_242 + (int32_t)((uint32_t)scalar_value_242 >> 31)) >> 1)))));
                w_u32(actor + 676, (uint32_t)((scalar_value_243 = (((int32_t)(uint32_t)(r_u32(actor + 676))))), (((int32_t)((scalar_value_243 + (int32_t)((uint32_t)scalar_value_243 >> 31)) >> 1)))));
                w_u32(actor + 616, (uint32_t)((scalar_value_244 = (((int32_t)(uint32_t)(r_u32(actor + 616))))), (((int32_t)((scalar_value_244 + (int32_t)((uint32_t)scalar_value_244 >> 31)) >> 1)))));
                if (linked)
                    w_u32(linked + 488, (uint32_t)((scalar_value_245 = (((int32_t)(uint32_t)(r_u32(linked + 488))))), (((int32_t)((scalar_value_245 + (int32_t)((uint32_t)scalar_value_245 >> 31)) >> 1)))));
            }
        }
    next_actor:
        link = r_u32(link + 4);
    }
}

static int32_t race_clamp(int32_t value, int32_t low, int32_t high)
{
    if (value < low)
        return low;
    if (value > high)
        return high;
    return value;
}

static void race_root_prepare_pool(int32_t delta)
{
    int32_t remaining = r_s32(r_u32(0x800CE4DC));
    uint32_t actor = r_u32(0x800CE4D0);
    while (remaining >= 0)
    {
        if (r_u32(actor + 560) & 0x8000000)
        {
            uint32_t state = r_u32(actor + 564);
            w_u32(actor + 564, state & 0xF7FFFFFF);
            if (state & 0x200)
                w_u32(actor + 924, r_u32(r_u32(actor + 556) + 224) << 1);
        }
        if (r_s16(actor + 944))
        {
            uint32_t step = (uint32_t)sub_8001FC90(delta, r_s32(actor + 904));
            uint32_t phase = r_u32(actor + 908);
            uint32_t excess, axis, timer_step, next;
            int32_t timer;
            if (r_s16(actor + 944) < 0)
                phase -= step;
            else
                phase += step;
            excess = 0x10000 - phase;
            phase = (phase & ~(uint32_t)((int32_t)phase >> 31)) + (excess & (uint32_t)((int32_t)excess >> 31));
            w_u32(actor + 908, phase);
            for (axis = 0; axis < 3; ++axis)
            {
                uint32_t value = (uint32_t)(((int64_t)r_s32(actor + 892 + 4 * axis) * (int32_t)phase) >> 16);
                w_u32(actor + 880 + 4 * axis, r_u32(actor + 880 + 4 * axis) + value);
            }
            timer_step = (uint32_t)((int32_t)(((uint32_t)delta << 8) + 0x8000) >> 16);
            timer = r_s16(actor + 944);
            if (timer < 0)
            {
                next = (uint32_t)timer + timer_step;
                if ((int32_t)next > 0)
                    next = 0;
            }
            else if ((int32_t)((uint32_t)timer - timer_step) > 0)
                next = r_u16(actor + 944) - timer_step;
            else
            {
                uint32_t kind = r_u16(actor + 956 + 8 * ((uint32_t)r_s8(actor + 946) - 1));
                if (kind == 3 || (kind == 4 && timer == 1) || kind - 5 < 13)
                    next = 1;
                else
                {
                    uint32_t magnitude = sub_8002E548(xport_guest_cptr(actor + 892, 12));
                    if ((int32_t)magnitude < 16)
                    {
                        w_u32(actor + 908, 0);
                        next = 0;
                    }
                    else
                    {
                        w_u32(actor + 904, sub_80010028(163840, magnitude));
                        next = (uint32_t)((int32_t)(0 - ((sub_80010028(magnitude, 163840) << 8) + 0x8000)) >> 16);
                    }
                }
            }
            w_u16(actor + 944, next);
        }
        if (r_u32(actor + 568) & 0x8001800)
            (void)sub_80071BCC(actor);
        --remaining;
        actor += r_u32(0x800CE4D4);
    }
}

static void race_root_prepare_riders(void)
{
    uint32_t link = r_u32(0x8005B29C);
    while (link != 0x8005B298)
    {
        uint32_t actor = link - 1088, specification = r_u32(actor + 556);
        uint32_t lateral = r_u32(actor + 676), linked = r_u32(actor + 856) != 0;
        uint32_t absolute = (lateral + (uint32_t)((int32_t)lateral >> 31)) ^ (uint32_t)((int32_t)lateral >> 31);
        uint32_t stale = 0, threshold, flags, state, fast;
        if (r_u32(actor + 564) & 4)
            stale = r_u32(actor + 716) >> 31;
        threshold = r_u32(specification + 204);
        if (linked && (int32_t)lateral > 0)
            threshold = (uint32_t)((int32_t)threshold / 2);
        fast = (int32_t)absolute >= (int32_t)threshold;
        flags = r_u32(actor + 560);
        flags |= ((flags & 0x8000000) && r_s32(specification + 208) < (int32_t)absolute) ? 16 : 0;
        state = r_u32(actor + 564);
        w_u32(actor + 560, flags);
        if (!(state & 0x400) && (flags & 0x300) && (fast || stale) && !(flags & 0x20000000))
        {
            state = r_u32(actor + 568);
            state = r_s32(actor + 364) < 0 ? state | 0x400000 : state & 0xFFBFFFFF;
            w_u32(actor + 568, state);
            w_u32(actor + 716, 0);
            if (linked && r_s32(actor + 676) > 0)
                state = r_u32(actor + 568) | 0x820;
            else
            {
                uint32_t extra = 0;
                if ((r_u32(actor + 564) & 0x400) || (stale && !r_u32(actor + 596)) || (fast && ((r_s32(actor + 676) < 0 && (r_u32(actor + 560) & 0x100)) || (r_s32(actor + 676) > 0 && (r_u32(actor + 560) & 0x200)))))
                    extra = 1;
                state = r_u32(actor + 568) | 0x800 | (32 + (extra << 5));
            }
            w_u32(actor + 568, state);
            threshold = r_u32(actor + 576);
            if ((int32_t)threshold < 65536)
                threshold = 65536;
            w_u32(actor + 576, threshold);
        }
        if (!r_u32(actor + 616) && !r_u32(actor + 620) && !r_u32(actor + 624))
        {
            state = r_u32(actor + 568);
            if (!(state & 0x7FF) && r_u32(r_u32(actor + 852) + 604) >= 2)
                w_u32(actor + 568, state | 0x880);
        }
        {
            uint32_t grip = r_u32(0x800D38E0 + 4 * (uint32_t)r_s8(actor + 534));
            w_u32(actor + 756, 0);
            state = r_u32(actor + 564);
            w_u32(actor + 752, 0);
            if (!(state & 0x400))
            {
                uint32_t force, coefficient, player;
                threshold = (r_u32(actor + 560) & 0x8000000) ? 0 : 117188;
                force = 0 - (uint32_t)(((int64_t)(int32_t)grip * r_s32(actor + 580)) >> 16);
                if ((int32_t)threshold >= r_s32(actor + 576))
                    force = 0;
                player = r_u16(actor + 172);
                coefficient = r_u32(specification + 320);
                if (player < r_u32(r_u32(0x8005B2F8) + 48) && r_s8(0x800D80F1))
                {
                    uint32_t other = r_u32(0x8005B268 + 4 * (player == 0));
                    if ((int32_t)(r_u32(other + 324) + 49152) < r_s32(actor + 324))
                        coefficient -= r_u32(specification + 332);
                }
                w_u32(actor + 752, (uint32_t)(((int64_t)(int32_t)force * (int32_t)coefficient) >> 16));
                coefficient = r_u32(specification + 324);
                lateral = r_u32(actor + 676);
                w_u32(actor + 756, (uint32_t)(((int64_t)(int32_t)force * (int32_t)coefficient) >> 16));
                if (lateral)
                {
                    absolute = (lateral + (uint32_t)((int32_t)lateral >> 31)) ^ (uint32_t)((int32_t)lateral >> 31);
                    coefficient = (uint32_t)(((int64_t)r_s32(specification + 328) * (int32_t)absolute) >> 16) + 65536;
                    force = (uint32_t)(((int64_t)r_s32(actor + 752) * (int32_t)coefficient) >> 16);
                    grip = (uint32_t)(((int64_t)r_s32(actor + 756) * (int32_t)coefficient) >> 16);
                    w_u32(actor + 752, force);
                    w_u32(actor + 756, grip);
                }
            }
        }
        w_u32(actor + 764, r_u32(actor + 760) + r_u32(actor + 752));
        w_u32(actor + 748, (uint32_t)(((int64_t)r_s32(specification + 12) * r_s32(actor + 736)) >> 16));
        flags = r_u32(actor + 560);
        if (flags & 0x20)
        {
            int32_t pitch = r_s16(actor + 530);
            if (pitch < 0)
                threshold = 2343777;
            else if (pitch >= 3851)
                threshold = 1757832;
            else
            {
                uint32_t quotient = ((uint32_t)pitch << 12) / 3850;
                threshold = (uint32_t)(((int64_t)(int32_t)(quotient << 4) * -585945) >> 16) + 2343777;
            }
            state = r_u32(actor + 564);
            w_u32(actor + 564, (int32_t)threshold < r_s32(actor + 576) && !linked ? state | 0x80 : state & 0xFFFFFF7F);
        }
        else
        {
            state = r_u32(actor + 564);
            if (!(flags & 0x40))
                state &= 0xFFFFFF7F;
            w_u32(actor + 564, state);
        }
        {
            uint32_t divisor = (uint32_t)(r_s32(specification + 16) >> 1) + (uint32_t)((int32_t)(r_u32(specification + 16) - 2) >> 31);
            uint32_t inverse = divisor ? 0x80000000 / divisor : 0xFFFFFFFF;
            uint32_t value = (uint32_t)(((int64_t)r_s32(actor + 748) * (int32_t)inverse) >> 16), bonus = 0;
            state = r_u32(actor + 564);
            w_u32(actor + 600, value);
            if ((state & 0x80) && r_s32(actor + 576) < r_s32(specification + 304))
                bonus = 13106;
            value += bonus - 6553;
            w_u32(actor + 600, value);
            if ((int32_t)value < 0)
                value = 0;
            w_u32(actor + 600, value);
        }
        link = r_u32(link + 4);
    }
}

static void race_root_controls(int32_t delta)
{
    uint32_t link = r_u32(0x8005B29C);
    while (link != 0x8005B298)
    {
        uint32_t actor = link - 1088, cooldown = r_u32(actor + 720);
        if (cooldown)
        {
            if ((int32_t)cooldown > 0)
            {
                cooldown -= (uint32_t)delta;
                w_u32(actor + 720, cooldown);
                if ((int32_t)cooldown < 0)
                    cooldown = 0;
                w_u32(actor + 720, cooldown);
            }
            w_u32(actor + 596, 0);
            w_u32(actor + 588, 0);
        }
        else
        {
            uint32_t flags = r_u32(actor + 560), mirrored = (flags >> 27) & 1;
            uint32_t assisted = (flags >> 20) & 1, specification = r_u32(actor + 556);
            uint32_t command = 0, throttle = 0, brake = 0, accelerating, braking;
            if (!mirrored)
            {
                if (assisted)
                {
                    uint32_t scale = r_u32(0x800CE540 + 8 * r_u16(actor + 172));
                    uint32_t source = (int32_t)scale < 0 ? r_u32(actor + 592) : r_u32(actor + 600);
                    command = 0 - (uint32_t)(((int64_t)(int32_t)source * (int32_t)scale) >> 16);
                }
            }
            else if (r_u32(actor + 564) & 0x80000)
                command = 0 - r_u32(actor + 600);
            else
            {
                uint32_t timer = r_u32(actor + 916), bound = 0x7FFF0000;
                if ((int32_t)timer > 0)
                {
                    uint32_t difference = r_u32(actor + 920) - r_u32(actor + 580);
                    uint32_t divisor = (r_u32(actor + 600) - r_u32(actor + 760)) << 1;
                    int positive = (int32_t)difference > 0, positive_divisor = (int32_t)divisor > 0;
                    uint32_t quotient = sub_80010028(positive ? difference : 0 - difference, positive_divisor ? divisor : 0 - divisor);
                    bound = positive == positive_divisor ? timer + quotient : timer - quotient;
                }
                if ((int32_t)bound > 0x20000)
                {
                    uint32_t difference = r_u32(actor + 924) - r_u32(actor + 576), divisor = (uint32_t)delta;
                    uint32_t drag = r_u32(actor + 764), desired;
                    int positive = (int32_t)difference > 0, positive_divisor = delta > 0;
                    desired = sub_80010028(positive ? difference : 0 - difference, positive_divisor ? divisor : 0 - divisor);
                    if (positive != positive_divisor)
                        desired = 0 - desired;
                    command = desired - drag;
                    if ((int32_t)command >= 0)
                    {
                        uint32_t limit = r_u32(specification + 188);
                        if ((int32_t)command > (int32_t)limit)
                            command = limit;
                    }
                    else
                    {
                        uint32_t limit = 0 - r_u32(actor + 600);
                        command += r_u32(actor + 752);
                        if ((int32_t)command <= (int32_t)limit)
                            command = limit;
                    }
                }
                else if ((int32_t)bound <= 0)
                {
                    command = 0 - r_u32(actor + 600);
                    w_u32(actor + 564, r_u32(actor + 564) | 0x80000);
                }
            }
            if (mirrored)
            {
                if ((int32_t)command < 0)
                {
                    brake = 0 - command;
                    w_u32(actor + 588, 0);
                    flags = r_u32(actor + 560);
                    if ((int32_t)brake < r_s32(actor + 596))
                        flags &= 0xFFFFFFBF;
                    else
                        flags |= (flags & 0x40) ? 0x40 : 0x60;
                    w_u32(actor + 560, flags);
                    w_u32(actor + 560, r_u32(actor + 560) & 0xFFFFFFFD);
                }
                else if ((int32_t)command > 0)
                {
                    throttle = command;
                    flags = r_u32(actor + 560);
                    w_u32(actor + 596, 0);
                    w_u32(actor + 560, flags & 0xFFFFFFBF);
                    if (flags & 0x10)
                    {
                        uint32_t limit = sub_8007EF60(specification, r_s32(actor + 736), r_s32(actor + 652), r_s32(0x800D3920 + 4 * (uint32_t)r_s8(actor + 534)));
                        if ((int32_t)throttle > (int32_t)limit)
                            throttle = limit;
                        w_u32(actor + 560, r_u32(actor + 560) & 0xFFFFFFEF);
                    }
                    flags = r_u32(actor + 560);
                    if ((int32_t)throttle >= r_s32(actor + 588))
                        flags |= (flags & 2) ? 2 : 3;
                    else
                        flags &= 0xFFFFFFFD;
                    w_u32(actor + 560, flags);
                }
                else
                    w_u32(actor + 560, r_u32(actor + 560) & 0xFFFFFFBD);
            }
            else if (assisted)
            {
                if ((int32_t)command < 0)
                {
                    brake = 0 - command;
                    flags = r_u32(actor + 560);
                    w_u32(actor + 588, 0);
                    w_u32(actor + 560, (flags | 0x40) & 0xFFFFFFFD);
                }
                else if ((int32_t)command > 0)
                {
                    throttle = command;
                    flags = r_u32(actor + 560);
                    w_u32(actor + 596, 0);
                    w_u32(actor + 560, (flags & 0xFFFFFFBF) | 2);
                }
                else
                    w_u32(actor + 560, r_u32(actor + 560) & 0xFFFFFFBD);
            }
            flags = r_u32(actor + 560);
            accelerating = (flags >> 1) & 1;
            braking = (flags >> 6) & 1;
            if (!mirrored)
            {
                if (r_u32(actor + 856) && r_u32(actor + 1088))
                    (void)sub_80074D58(actor);
                (void)sub_80074E6C(actor, (uint32_t)delta);
                if (r_u32(0x8005B21C))
                {
                    uint32_t other = r_u32(0x8005B268 + 4 * (r_u16(actor + 172) == 0));
                    if (!(r_u32(other + 568) & 0x10000000))
                    {
                        uint32_t dx = r_u32(actor + 184) - r_u32(other + 184);
                        uint32_t dz = r_u32(actor + 192) - r_u32(other + 192);
                        int32_t x = ((int32_t)((uint32_t)((int32_t)dx >> 16) + (uint32_t)((int32_t)dx >> 31))) ^ ((int32_t)dx >> 31);
                        int32_t z = ((int32_t)((uint32_t)((int32_t)dz >> 16) + (uint32_t)((int32_t)dz >> 31))) ^ ((int32_t)dz >> 31);
                        uint32_t distance, side, threshold;
                        if (x < z)
                        {
                            int32_t exchange = x;
                            x = z;
                            z = exchange;
                        }
                        side = (uint32_t)z + (uint32_t)(z >> 1);
                        distance = (uint32_t)x - (uint32_t)(x >> 5) - (uint32_t)(x >> 7) + (uint32_t)((int32_t)side >> 2) + (uint32_t)((int32_t)side >> 6);
                        threshold = r_u32(0x800D39A0);
                        threshold += (0 - (uint32_t)r_s8(0x800D80F1)) & (r_u32(0x800D399C) - threshold);
                        if (r_u32(actor + 568) & 0x10000000)
                        {
                            if ((int32_t)threshold < (int32_t)distance || !(flags & 2) || (flags & 0x40) || r_s32(actor + 480) < (int32_t)sub_8001FC90(r_s32(0x800D3998), r_s32(specification + 224)) || r_s32(other + 480) < (int32_t)sub_8001FC90(r_s32(0x800D3994), r_s32(r_u32(other + 556) + 224)) || (r_u32(other + 568) & 0x7EC) || ((int32_t)distance < 5 && (r_u32(other + 560) & 0x40)))
                                w_u32(actor + 568, r_u32(actor + 568) & 0xEFFFFFFF);
                        }
                        else if ((int32_t)distance < (int32_t)threshold && (int32_t)sub_8002E698(xport_guest_cptr(actor + 450, 6), xport_guest_cptr(other + 450, 6)) > 45875)
                        {
                            uint32_t dot = 0, axis;
                            for (axis = 0; axis < 3; ++axis)
                            {
                                uint32_t difference = r_u32(other + 504 + 4 * axis) - r_u32(actor + 504 + 4 * axis);
                                dot += (uint32_t)(((int64_t)(int32_t)difference * (r_s16(actor + 450 + 2 * axis) * 16)) >> 16);
                            }
                            if ((int32_t)dot > 0 && (flags & 2) && !(flags & 0x40) && !(r_u32(actor + 388) & 1) && !r_u16(0x800CCAC0 + 2 * r_u16(actor + 172)) && r_s32(actor + 480) < (int32_t)(r_u32(other + 480) + 585944) && (int32_t)sub_8001FC90(r_s32(0x800D3998), r_s32(specification + 224)) < r_s32(actor + 480) && (int32_t)sub_8001FC90(r_s32(0x800D3994), r_s32(r_u32(other + 556) + 224)) < r_s32(other + 480))
                                w_u32(actor + 568, r_u32(actor + 568) | 0x10000000);
                        }
                        if (r_s32(actor + 324) <= 1228799 || r_s32(other + 324) <= 1228799)
                            w_u32(actor + 568, r_u32(actor + 568) & 0xEFFFFFFF);
                    }
                    if (r_u32(actor + 568) & 0x10000000)
                    {
                        uint32_t dot = 0, axis, pull, damping, desired, lower, upper, phase;
                        for (axis = 0; axis < 3; ++axis)
                        {
                            uint32_t difference = r_u32(other + 504 + 4 * axis) - r_u32(actor + 504 + 4 * axis);
                            dot += (uint32_t)(((int64_t)(int32_t)difference * (r_s16(actor + 450 + 2 * axis) * 16)) >> 16);
                        }
                        pull = (uint32_t)sub_8001FC90(r_s32(0x800D39A4), (int32_t)dot);
                        damping = (uint32_t)sub_8001FC90(r_s32(0x800D39A8), (int32_t)(r_u32(actor + 576) - r_u32(other + 576)));
                        desired = pull - damping;
                        lower = 0 - r_u32(actor + 600);
                        phase = desired - lower;
                        desired += (uint32_t)((int32_t)phase >> 31) & (lower - desired);
                        upper = r_u32(actor + 592);
                        desired += (uint32_t)((int32_t)(upper - (pull - damping)) >> 31) & (upper - (pull - damping));
                        phase = (uint32_t)sub_8001FC90((int32_t)desired, delta);
                        desired = r_u32(actor + 576) + phase;
                        w_u32(actor + 924, desired);
                        upper = r_u32(r_u32(actor + 556) + 224) - (desired - 585944);
                        w_u32(actor + 560, r_u32(actor + 560) | 0x1000000);
                        w_u32(actor + 924, (desired & ~(uint32_t)((int32_t)desired >> 31)) + (upper & (uint32_t)((int32_t)upper >> 31)));
                    }
                }
            }
            if (assisted)
            {
                w_u32(actor + 588, throttle);
                w_u32(actor + 596, brake);
            }
            else
            {
                uint32_t current, rate, step, next;
                rate = (uint32_t)(((int64_t)r_s32(specification + (accelerating ? 340 : 344)) * r_s32(actor + 592)) >> 16);
                step = (uint32_t)(((int64_t)(int32_t)rate * delta) >> 16);
                current = r_u32(actor + 588);
                next = accelerating ? current + step : current - step;
                if (mirrored && (accelerating ? (int32_t)current <= (int32_t)throttle && (int32_t)throttle < (int32_t)next : (int32_t)next < (int32_t)throttle && (int32_t)current >= (int32_t)throttle))
                    next = throttle;
                if (!accelerating)
                {
                    w_u32(actor + 588, next);
                    if ((int32_t)next < 0)
                        next = 0;
                }
                w_u32(actor + 588, next);
                rate = (uint32_t)(((int64_t)r_s32(specification + (braking ? 348 : 352)) * r_s32(actor + 600)) >> 16);
                step = (uint32_t)(((int64_t)(int32_t)rate * delta) >> 16);
                current = r_u32(actor + 596);
                next = braking ? current + step : current - step;
                if (mirrored && (braking ? (int32_t)current <= (int32_t)brake && (int32_t)brake < (int32_t)next : (int32_t)next < (int32_t)brake && (int32_t)current >= (int32_t)brake))
                    next = brake;
                if (!braking)
                {
                    w_u32(actor + 596, next);
                    if ((int32_t)next < 0)
                        next = 0;
                }
                w_u32(actor + 596, next);
            }
            {
                uint32_t brake_indicator = r_u32(actor + 480) ? (0 - braking) & 0xFFFF0000 : 0;
                uint32_t moving = r_u32(actor + 480);
                w_u32(actor + 696, brake_indicator);
                w_u32(actor + 700, moving ? (0 - accelerating) & 0xFFFF0000 : 0);
            }
            if (!accelerating && (!mirrored || !r_u32(actor + 588)))
            {
                uint32_t difference = r_u32(actor + 756) - r_u32(actor + 752);
                uint32_t friction = r_u32(actor + 756), drag = r_u32(actor + 764);
                w_u32(actor + 752, friction);
                w_u32(actor + 764, drag + difference);
            }
            if ((int32_t)(0 - r_u32(actor + 752)) >= r_s32(actor + 596))
                w_u32(actor + 596, 0);
            else
            {
                uint32_t drag = r_u32(actor + 760);
                w_u32(actor + 752, 0);
                w_u32(actor + 764, drag);
            }
        }
        link = r_u32(link + 4);
    }
}

static void race_root_physics(int32_t delta)
{
    uint32_t link = r_u32(0x8005B29C);
    while (link != 0x8005B298)
    {
        uint32_t actor = link - 1088, specification = r_u32(actor + 556), body = r_u32(actor + 852);
        uint32_t throttle, brake, grip, lateral, slip, traction, drive, brake_force, roll = 0, transition = 0;
        uint32_t value, magnitude, divisor, inverse, flags, state, previous_speed, skidding, stopping;
        int eligible = 0, shallow_bank;
        if (!(r_u32(actor + 388) & 1) || (int32_t)(sub_8002E698(xport_guest_cptr(actor + 450, 6), xport_guest_cptr(r_u32(actor + 340) + 2, 6)) ^ r_u32(actor + 344)) < 0)
            eligible = 1;
        throttle = r_u32(actor + 592);
        if (r_s32(actor + 588) < (int32_t)throttle)
            throttle = r_u32(actor + 588);
        brake = r_u32(actor + 600);
        w_u32(actor + 588, throttle);
        if (r_s32(actor + 596) < (int32_t)brake)
            brake = r_u32(actor + 596);
        grip = r_u32(0x800D3920 + 4 * (uint32_t)r_s8(actor + 534));
        w_u32(actor + 596, brake);
        if (r_u32(actor + 696) || r_u32(actor + 700))
        {
            uint32_t square, first, second;
            value = (uint32_t)sub_8001FC90((int32_t)grip, r_s32(actor + 736));
            square = (uint32_t)sub_8001FC90((int32_t)value, (int32_t)value);
            magnitude = (int32_t)square < 0 ? 0 - square : square;
            divisor = (uint32_t)((int32_t)magnitude >> 1) + (uint32_t)((int32_t)(magnitude - 2) >> 31);
            inverse = divisor ? 0x80000000 / divisor : 0xFFFFFFFF;
            if ((int32_t)square < 0)
                inverse = 0 - inverse;
            value = (uint32_t)sub_8001FC90(r_s32(actor + 652), r_s32(specification + 192));
            first = (uint32_t)sub_8001FC90((int32_t)value, (int32_t)value);
            if (r_u32(actor + 700))
            {
                value = (uint32_t)sub_8001FC90((int32_t)throttle, r_s32(specification + 200));
                second = (uint32_t)sub_8001FC90((int32_t)value, (int32_t)value);
                w_u32(actor + 700, (uint32_t)sub_8001FC90((int32_t)(first + second), (int32_t)inverse));
            }
            else
            {
                value = (uint32_t)sub_8001FC90((int32_t)brake, r_s32(specification + 196));
                second = (uint32_t)sub_8001FC90((int32_t)value, (int32_t)value);
                w_u32(actor + 696, (uint32_t)sub_8001FC90((int32_t)(first + second), (int32_t)inverse));
            }
        }
        lateral = r_u32(actor + 652);
        lateral = (lateral + (uint32_t)((int32_t)lateral >> 31)) ^ (uint32_t)((int32_t)lateral >> 31);
        value = (uint32_t)sub_8001FC90((int32_t)grip, r_s32(actor + 736));
        {
            uint32_t coefficient = r_u32(specification + 192);
            int positive = (int32_t)value > 0;
            int positive_coefficient = (int32_t)coefficient > 0;
            value = (uint32_t)sub_8001FC90((int32_t)grip, r_s32(actor + 736));
            slip = sub_80010028(positive ? value : 0 - value, positive_coefficient ? coefficient : 0 - coefficient);
            if (positive != positive_coefficient)
                slip = 0 - slip;
            slip -= lateral;
        }
        value = r_u32(actor + 616);
        if ((int32_t)value >= 11439)
            traction = 0x10000;
        else if ((int32_t)value < -11438)
            traction = 0xFFFF0000;
        else if (r_u32(actor + 576))
        {
            uint32_t table = (((r_u32(actor + 636) * 163) >> 13) & 0x1FFE) + 1;
            int32_t cosine = r_s16(0x8005624C + 2 * table) * 16;
            uint32_t scaled_mass = (uint32_t)(((int64_t)cosine * r_s32(specification + 16)) >> 16);
            uint32_t force = r_u32(actor + 588) - r_u32(actor + 596) + r_u32(actor + 752);
            traction = (uint32_t)(((int64_t)(int32_t)force * (int32_t)scaled_mass) >> 16);
            if ((int32_t)traction > 0)
            {
                divisor = (uint32_t)(((int64_t)r_s32(specification + 8) * r_s32(actor + 736)) >> 16);
                traction = (int32_t)divisor > 0 ? sub_80010028(traction, divisor) : 0 - sub_80010028(traction, 0 - divisor);
                if ((int32_t)traction <= 65535)
                    w_u32(actor + 564, r_u32(actor + 564) & 0xFFFFFEFF);
            }
            else if ((int32_t)traction < 0)
            {
                divisor = r_u32(actor + 748);
                traction = (int32_t)divisor > 0 ? 0 - sub_80010028(0 - traction, divisor) : sub_80010028(0 - traction, 0 - divisor);
            }
        }
        else
            traction = 0;
        drive = (r_s32(actor + 616) >= 0 || (r_u32(actor + 564) & 0x200000)) ? r_u32(actor + 588) : 0;
        brake_force = r_u32(actor + 596);
        value = r_u32(actor + 636);
        magnitude = (value + (uint32_t)((int32_t)value >> 31)) ^ (uint32_t)((int32_t)value >> 31);
        shallow_bank = (int32_t)magnitude < 22876;
        if (shallow_bank && (int32_t)slip <= 0)
        {
            uint32_t heading = sub_80020018(0 - r_u32(actor + 732), r_u32(actor + 740)) * 25736;
            uint32_t current, difference, adjusted;
            int increasing;
            heading = (uint32_t)((int32_t)heading >> 8);
            current = r_u32(actor + 676);
            difference = heading - current;
            if ((int32_t)difference > 205887)
                heading -= 411774;
            else if ((int32_t)difference < -205887)
                heading += 411774;
            increasing = (int32_t)heading >= r_s32(actor + 676);
            adjusted = heading + (increasing ? (uint32_t)-91505 : 91505);
            if (r_s32(actor + 576) > 327680)
            {
                uint32_t speed = r_u32(actor + 576) - 327680;
                uint32_t numerator = (uint32_t)sub_8001FC90((int32_t)adjusted, (int32_t)speed) + heading;
                int positive = (int32_t)numerator > 0;
                numerator = (uint32_t)sub_8001FC90((int32_t)adjusted, (int32_t)speed) + heading;
                adjusted = positive ? sub_80010028(numerator, speed + 1) : 0 - sub_80010028(0 - numerator, speed + 1);
            }
            difference = heading - adjusted;
            w_u32(actor + 708, (difference + (uint32_t)((int32_t)difference >> 31)) ^ (uint32_t)((int32_t)difference >> 31));
            w_u32(actor + 688, adjusted);
            w_u32(actor + 684, 0);
            if ((increasing && r_s32(actor + 676) < (int32_t)adjusted) || (!increasing && (int32_t)adjusted < r_s32(actor + 676)))
            {
                value = 643207 - r_u32(actor + 736);
                w_u32(actor + 680, increasing ? value : 0 - value);
            }
            else
                w_u32(actor + 680, 0);
            magnitude = (heading + (uint32_t)((int32_t)heading >> 31)) ^ (uint32_t)((int32_t)heading >> 31);
            if (r_u32(actor + 576) && (int32_t)magnitude > 91505)
                w_u32(actor + 744, (uint32_t)(int16_t)((int32_t)(643207 - r_u32(actor + 736)) >> 1));
            else
                w_u32(actor + 744, 0);
            value = r_u32(actor + 488);
            magnitude = (value + (uint32_t)((int32_t)value >> 31)) ^ (uint32_t)((int32_t)value >> 31);
            value = r_u32(actor + 744);
            if ((int32_t)magnitude >= (int32_t)value)
                value = 0;
            else if (r_s32(actor + 688) >= 0)
                value = 0 - value;
            w_u32(actor + 744, value);
            state = r_u32(actor + 564);
            w_u32(actor + 700, 0x10000);
            w_u32(actor + 696, 0x10000);
            w_u32(actor + 564, (state & 0xFFFFFBE3) | 0x400);
        }
        else if (r_u32(actor + 564) & 0x400)
        {
            value = r_u32(actor + 680);
            w_u32(actor + 744, 0);
            magnitude = r_u32(actor + 676);
            magnitude = (magnitude + (uint32_t)((int32_t)magnitude >> 31)) ^ (uint32_t)((int32_t)magnitude >> 31);
            if (!value || !r_u32(actor + 576) || (int32_t)magnitude < r_s32(actor + 708))
            {
                uint16_t direction[3] = {r_u16(actor + 528), r_u16(actor + 530), r_u16(actor + 532)};
                uint16_t side[3] = {r_u16(actor + 516), r_u16(actor + 518), r_u16(actor + 520)};
                w_u32(actor + 680, 0);
                w_u32(actor + 676, 0);
                w_u32(actor + 700, 0);
                w_u32(actor + 696, 0);
                w_u16(actor + 450, direction[0]);
                w_u16(actor + 452, direction[1]);
                w_u16(actor + 454, direction[2]);
                w_u16(actor + 814, side[0]);
                w_u16(actor + 816, side[1]);
                w_u16(actor + 818, side[2]);
                w_u32(actor + 764, 0 - (uint32_t)(((int64_t)(int32_t)slip * 655360) >> 16));
            }
        }
        else
        {
            throttle = r_u32(actor + 588);
            w_u32(actor + 744, 0);
            if ((int32_t)throttle > 0 && !r_u32(actor + 596))
            {
                if (r_s32(actor + 700) > 65536)
                {
                    state = r_u32(actor + 564);
                    w_u32(actor + 564, state & 0xFFFFFFFB);
                    if (!(state & 8))
                    {
                        w_u32(actor + 564, (state & 0xFFFFFFEB) | 0x10);
                        (void)sub_8002EAD8(xport_guest_cptr(actor + 504, 12), xport_guest_cptr(actor + 528, 6), r_u32(actor + 308), xport_guest_ptr(actor + 784, 12));
                    }
                    w_u32(actor + 564, r_u32(actor + 564) | 8);
                    throttle = r_u32(actor + 588);
                    divisor = r_u32(actor + 700);
                    drive = sub_80010028((int32_t)throttle > 0 ? throttle : 0 - throttle, (int32_t)divisor > 0 ? divisor : 0 - divisor);
                    if (((int32_t)throttle > 0) != ((int32_t)divisor > 0))
                        drive = 0 - drive;
                    if (r_s32(actor + 764) < (int32_t)(0 - drive) && eligible && shallow_bank)
                        drive += 0x8000 - drive - r_u32(actor + 764);
                }
                else
                    w_u32(actor + 564, r_u32(actor + 564) & 0xFFFFFFF3);
            }
            else if (r_s32(actor + 696) > 65536)
            {
                state = r_u32(actor + 564);
                w_u32(actor + 564, state & 0xFFFFFFF7);
                w_u32(actor + 564, (state & 4) ? (state & 0xFFFFFFF7) | 4 : (state & 0xFFFFFFE7) | 0x14);
                if (!(r_u32(actor + 560) & 0x8000000))
                {
                    brake = r_u32(actor + 596);
                    divisor = r_u32(actor + 696);
                    brake_force = sub_80010028((int32_t)brake > 0 ? brake : 0 - brake, (int32_t)divisor > 0 ? divisor : 0 - divisor);
                    if (((int32_t)brake > 0) != ((int32_t)divisor > 0))
                        brake_force = 0 - brake_force;
                }
            }
            else
                w_u32(actor + 564, r_u32(actor + 564) & 0xFFFFFFF3);
        }
        value = r_u32(actor + 616);
        flags = r_u32(actor + 560);
        state = r_u32(actor + 564);
        {
            uint32_t mode = (flags >> 11) & 1;
            int lift = (int32_t)value >= 0 && (((int32_t)traction > 65535 && !value && !(state & 0x920)) || (mode && (flags & 0x1000) && r_s32(actor + 576) > 146486) || ((state & 0x20) && (flags & 1) && !(flags & 0x8000000)));
            if (lift)
            {
                int32_t pitch = r_s16(actor + 530);
                roll = r_u32(specification + (mode ? 268 : 264));
                if (pitch >= 3548)
                    roll = 0;
                else if (pitch >= -2047)
                {
                    uint32_t pitch_scale = (uint32_t)(((int64_t)(56754 - pitch * 16) * 47976) >> 16);
                    roll = (uint32_t)(((int64_t)(int32_t)roll * (int32_t)pitch_scale) >> 16);
                }
                if (roll)
                {
                    transition = 1;
                    w_u32(actor + 564, r_u32(actor + 564) | 0x100);
                    if (!mode)
                    {
                        int positive = (int32_t)drive > 0, positive_traction = (int32_t)traction > 0;
                        drive = sub_80010028(positive ? drive : 0 - drive, positive_traction ? traction : 0 - traction);
                        if (positive != positive_traction)
                            drive = 0 - drive;
                    }
                    brake_force = 0;
                    w_u32(actor + 564, (r_u32(actor + 564) & 0xFFFFF7DF) | 0x20);
                }
                if (!r_u32(actor + 616) && (int32_t)roll > 0)
                    (void)sub_800C4550(22, body, 8);
                w_u32(actor + 560, r_u32(actor + 560) & 0xFFFFEFFE);
            }
            else if ((int32_t)traction <= -65536 && !r_u32(actor + 616) && !(r_u32(actor + 564) & 0x40) && (int32_t)lateral < 17157 && r_s32(actor + 576) < r_s32(specification + 304))
            {
                int32_t pitch = r_s16(actor + 530);
                if (pitch >= 3548)
                    roll = 0 - r_u32(specification + 272);
                else if (pitch >= -2047)
                {
                    uint32_t pitch_scale = (uint32_t)(((int64_t)(56754 - pitch * 16) * 47976) >> 16);
                    uint32_t base = r_u32(specification + 272);
                    roll = 0 - (uint32_t)(((int64_t)(int32_t)(3431 - base) * (int32_t)pitch_scale) >> 16) - base;
                }
                if (roll)
                {
                    transition = 1;
                    drive = 0;
                    w_u32(actor + 564, r_u32(actor + 564) | 0x40);
                }
            }
        }
        value = traction + (((uint32_t)((int32_t)(traction + 65536) >> 31)) & (0xFFFF0000 - traction));
        value += ((uint32_t)((int32_t)(65536 - traction) >> 31)) & (65536 - traction);
        previous_speed = r_u32(actor + 576);
        w_u32(actor + 608, value);
        value = 0;
        if ((int32_t)previous_speed > 0 || (int32_t)drive > 0 || (r_u32(actor + 564) & 0x400))
            value = drive - brake_force + r_u32(actor + 764);
        w_u32(actor + 484, value);
        if ((r_u32(actor + 560) & 0x42) == 0x42 && (int32_t)value > -458752)
            w_u32(actor + 484, 0xFFF90000);
        state = r_u32(actor + 564);
        flags = r_u32(actor + 560);
        previous_speed = r_u32(actor + 576);
        skidding = (state >> 10) & 1;
        stopping = (state >> 12) & 1;
        if (flags & 0x1000000)
        {
            flags = r_u32(actor + 560);
            w_u32(actor + 576, r_u32(actor + 924));
            w_u32(actor + 560, flags & 0xFEFFFFFF);
        }
        else
        {
            value = previous_speed + (uint32_t)(((int64_t)r_s32(actor + 484) * delta) >> 16);
            w_u32(actor + 576, value);
            if ((int32_t)value < 0)
                value = 0;
            w_u32(actor + 576, value);
            if (!value || stopping)
            {
                flags = r_u32(actor + 560);
                if ((flags & 0x300) && !(flags & 0x40))
                    w_u32(actor + 576, previous_speed);
                else if (stopping && !skidding)
                {
                    w_u32(actor + 484, 0);
                    w_u32(actor + 576, 0);
                    w_u8(actor + 849, 0);
                }
            }
        }
        state = r_u32(actor + 564);
        flags = r_u32(actor + 388);
        w_u32(actor + 564, state & 0xFFFFCFFF);
        if ((flags & 1) && r_s16(actor + 452) < -2048)
        {
            value = (uint32_t)sub_8001FC90(246375, r_s16(actor + 452) * 16);
            w_u32(actor + 576, sub_80074FB4(r_s32(actor + 576), r_s32(r_u32(actor + 556) + 224) >> 1, (int32_t)(value + 188723)));
        }
        value = !skidding && !stopping && (int32_t)previous_speed > 0 && !r_u32(actor + 576);
        state = r_u32(actor + 564) | (value << 13);
        w_u32(actor + 564, state);
        if (!(state & 0x200000) && transition)
        {
            uint32_t difference = r_u32(actor + 616) - roll;
            if ((int32_t)difference >= -655 && (int32_t)difference <= 655)
            {
                w_u32(actor + 616, roll);
                w_u32(actor + 624, 0);
                w_u32(actor + 620, 0);
            }
            else
            {
                int decreasing = (int32_t)difference > 0;
                value = decreasing ? sub_80010028(0x20000, difference) : 0 - sub_80010028(0x20000, 0 - difference);
                w_u32(actor + 624, value);
                value = r_u32(actor + 616);
                w_u32(actor + 632, 0);
                w_u32(actor + 620, decreasing ? 0xFFFE0000 : 0x20000);
                w_u32(actor + 628, value);
                if (!value)
                    value = decreasing ? 0xFFFFFFF0 : 16;
                w_u32(actor + 616, value);
            }
        }
        link = r_u32(link + 4);
    }
}

static void race_root_dynamics(int32_t delta)
{
    uint32_t link = r_u32(0x8005B29C);
    while (link != 0x8005B298)
    {
        uint32_t actor = link - 1088, specification = r_u32(actor + 556);
        uint32_t state = r_u32(actor + 564), value, magnitude, divisor, velocity, acceleration;
        int oscillating = 0;
        if (r_s32(actor + 576) <= 0 || (state & 0x400))
            w_u32(actor + 564, r_u32(actor + 564) & 0xFFF7FFFF);
        else
        {
            if (state & 8)
            {
                velocity = r_u32(actor + 676);
                if ((int32_t)velocity >= 0 && r_s32(specification + 244) < r_s32(actor + 636))
                    oscillating = 1;
                else if ((int32_t)velocity <= 0 && r_s32(actor + 636) < (int32_t)(0 - r_u32(specification + 244)))
                    oscillating = 1;
            }
            if (oscillating)
            {
                uint32_t amplitude, threshold, half, angular_velocity;
                if (r_u32(actor + 856) && r_s32(actor + 652) > 0)
                {
                    amplitude = (uint32_t)(r_s32(specification + 212) / 2);
                    threshold = (uint32_t)(r_s32(specification + 204) / 2);
                }
                else
                {
                    amplitude = r_u32(specification + 212);
                    threshold = r_u32(specification + 204);
                }
                if (r_u32(actor + 564) & 0x10)
                {
                    value = (uint32_t)sub_8001FC90(r_s32(actor + 576), r_s32(actor + 652));
                    value = (uint32_t)sub_8001FC90((int32_t)amplitude, (int32_t)value);
                    angular_velocity = r_u32(actor + 676);
                    w_u32(actor + 684, value);
                    w_u32(actor + 680, 0);
                    w_u32(actor + 676, (int32_t)(angular_velocity ^ value) < 0 ? 0 : angular_velocity);
                }
                half = (uint32_t)((int32_t)threshold >> 1);
                angular_velocity = r_u32(actor + 676);
                acceleration = r_u32(actor + 684);
                if (((int32_t)angular_velocity <= (int32_t)(0 - half) && (int32_t)acceleration < 0) || ((int32_t)angular_velocity >= (int32_t)half && (int32_t)acceleration > 0))
                    acceleration = 0 - acceleration;
                w_u32(actor + 684, acceleration);
                value = (uint32_t)sub_8001FC90((int32_t)acceleration, delta);
                value += r_u32(actor + 680);
                angular_velocity = r_u32(actor + 676);
                w_u32(actor + 680, value);
                if (((int32_t)angular_velocity > 0 && (int32_t)value < 0) || ((int32_t)angular_velocity < 0 && (int32_t)value > 0))
                {
                    angular_velocity = r_u32(actor + 676);
                    w_u32(actor + 680, 0);
                    w_u32(actor + 676, (int32_t)angular_velocity > 0 ? threshold : 0 - threshold);
                }
                value = (uint32_t)sub_8001FC90(r_s32(actor + 680), delta);
                w_u32(actor + 676, r_u32(actor + 676) + value);
            }
            else
            {
                uint32_t flags;
                velocity = r_u32(actor + 676);
                value = velocity * 5;
                value = velocity - (uint32_t)(((int64_t)(int32_t)value * delta) >> 16);
                w_u32(actor + 684, 0);
                magnitude = (value + (uint32_t)((int32_t)value >> 31)) ^ (uint32_t)((int32_t)value >> 31);
                w_u32(actor + 680, 0);
                w_u32(actor + 676, (int32_t)magnitude < 655 ? 0 : value);
                flags = r_u32(actor + 560);
                if (!(flags & 0x8000000))
                {
                    state = r_u32(actor + 564);
                    if ((state & 12) == 4)
                    {
                        if (!(state & 0x10) && (flags & 0x40) && (flags & 0x300) && !(state & 2))
                            w_u32(actor + 716, r_u32(actor + 716) - (uint32_t)delta);
                        else
                        {
                            velocity = r_u32(actor + 576);
                            value = r_u32(specification + 356);
                            if ((int32_t)velocity < (int32_t)value)
                                w_u32(actor + 716, r_u32(specification + 400));
                            else if (r_s32(specification + 360) < (int32_t)velocity)
                                w_u32(actor + 716, r_u32(specification + 404));
                            else
                            {
                                value = (uint32_t)sub_8001FC90((int32_t)(velocity - value), r_s32(specification + 408));
                                w_u32(actor + 716, r_u32(specification + 400) + value);
                            }
                            if (r_u32(actor + 388) & 1)
                                w_u32(actor + 716, (uint32_t)sub_8001FC90(r_s32(specification + 412), r_s32(actor + 716)));
                        }
                    }
                }
            }
            divisor = r_u32(specification + 220);
            value = r_u32(actor + 676);
            magnitude = (value + (uint32_t)((int32_t)value >> 31)) ^ (uint32_t)((int32_t)value >> 31);
            if ((int32_t)magnitude >= (int32_t)divisor)
                value = r_u32(specification + 216);
            else
            {
                int positive = (int32_t)magnitude > 0, positive_divisor = (int32_t)divisor > 0;
                value = sub_80010028(positive ? magnitude : 0 - magnitude, positive_divisor ? divisor : 0 - divisor);
                if (positive != positive_divisor)
                    value = 0 - value;
                value = (uint32_t)(((int64_t)r_s32(specification + 216) * (int32_t)value) >> 16);
            }
            w_u32(actor + 744, (uint32_t)(((int64_t)r_s32(actor + 488) * (int32_t)value) >> 16));
        }
        state = r_u32(actor + 564) & 0xFFFFFFEF;
        w_u32(actor + 564, state);
        if (state & 0x1000000)
        {
            divisor = r_u32(actor + 720);
            if ((int32_t)divisor < 65)
            {
                w_u32(actor + 656, 0);
                w_u32(actor + 660, 0);
                w_u32(actor + 720, 0);
            }
            else
            {
                acceleration = r_u32(actor + 656);
                value = (int32_t)acceleration > 0 ? 0 - sub_80010028(acceleration, divisor) : sub_80010028(0 - acceleration, divisor);
                acceleration = r_u32(actor + 656);
                divisor = r_u32(actor + 720);
                velocity = r_u32(actor + 636);
                w_u32(actor + 660, value);
                w_u32(actor + 724, 0);
                w_u32(actor + 664, velocity);
                value = (uint32_t)sub_8001FC90((int32_t)acceleration, (int32_t)divisor);
                w_u32(actor + 644, r_u32(actor + 636) + (uint32_t)((int32_t)value >> 1));
            }
            w_u32(actor + 564, r_u32(actor + 564) & 0xFEFFFFFF);
        }
        velocity = r_u32(actor + 660);
        if (velocity)
        {
            acceleration = r_u32(actor + 656);
            value = r_u32(actor + 636);
            magnitude = (value + (uint32_t)((int32_t)value >> 31)) ^ (uint32_t)((int32_t)value >> 31);
            if ((int32_t)(acceleration ^ velocity) < 0 && (int32_t)magnitude < r_s32(specification + 228))
            {
                uint32_t elapsed, half;
                value = (uint32_t)sub_8001FC90((int32_t)velocity, delta);
                elapsed = r_u32(actor + 724) + (uint32_t)delta;
                acceleration = r_u32(actor + 656) + value;
                w_u32(actor + 656, acceleration);
                w_u32(actor + 724, elapsed);
                value = (uint32_t)sub_8001FC90(r_s32(actor + 660), (int32_t)elapsed);
                half = (uint32_t)((int32_t)value / 2);
                value = (uint32_t)sub_8001FC90(r_s32(actor + 724), (int32_t)(r_u32(actor + 656) - half));
                w_u32(actor + 636, r_u32(actor + 664) + value);
            }
            else
            {
                value = r_u32(actor + 644);
                w_u32(actor + 720, 0);
                w_u32(actor + 660, 0);
                w_u32(actor + 656, 0);
                w_u32(actor + 636, value);
            }
            divisor = r_u32(specification + 228);
            value = r_u32(actor + 636);
            value += (uint32_t)((int32_t)(value + divisor) >> 31) & ((0 - divisor) - value);
            {
                uint32_t original_angle = r_u32(actor + 636);
                value += (uint32_t)((int32_t)(divisor - original_angle) >> 31) & (divisor - original_angle);
            }
            w_u32(actor + 636, value);
        }
        link = r_u32(link + 4);
    }
}

static void race_root_external(int32_t delta)
{
    int32_t remaining;
    uint32_t actor, cursor, end;
    w_u32(0x1F800000, 0x1F800004);
    remaining = r_s32(r_u32(0x800CE4DC));
    actor = r_u32(0x800CE4D0);
    while (remaining >= 0)
    {
        if (r_s16(actor + 320))
        {
            uint32_t x = r_u32(actor + 184), y = r_u32(actor + 188), z = r_u32(actor + 192);
            uint32_t linked = r_u32(actor + 856);
            w_u32(actor + 468, x);
            w_u32(actor + 472, y);
            w_u32(actor + 476, z);
            if (linked)
            {
                w_u32(linked + 468, r_u32(linked + 184));
                linked = r_u32(actor + 856);
                w_u32(linked + 472, r_u32(linked + 188));
                linked = r_u32(actor + 856);
                w_u32(linked + 476, r_u32(linked + 192));
            }
            (void)sub_8007F0BC(actor, (uint32_t)delta);
        }
        --remaining;
        actor += r_u32(0x800CE4D4);
    }
    (void)sub_80037338();
    (void)sub_8003E150();
    (void)sub_8003AE24();
    (void)sub_8003B520();
    cursor = 0x1F800004;
    end = r_u32(0x1F800000);
    actor = r_u32(cursor);
    while (cursor < end)
    {
        uint32_t position = actor + ((r_u32(actor + 568) & 0x600) ? 184 : 504);
        cursor += 4;
        (void)sub_8007504C(actor, position);
        end = r_u32(0x1F800000);
        actor = r_u32(cursor);
    }
    cursor = 0x1F800004;
    end = r_u32(0x1F800000);
    actor = r_u32(cursor);
    while (cursor < end)
    {
        cursor += 4;
        (void)sub_8007FA4C(actor);
        end = r_u32(0x1F800000);
        actor = r_u32(cursor);
    }
    remaining = r_s32(r_u32(0x800CE4DC));
    actor = r_u32(0x800CE4D0);
    while (remaining >= 0)
    {
        if (r_s16(actor + 320))
            (void)sub_800807F0(actor, delta);
        --remaining;
        actor += r_u32(0x800CE4D4);
    }
    (void)sub_80093E6C();
    (void)sub_800950E8();
}

uint32_t sub_80075EE0(int32_t delta)
{
    uint32_t count, index = 0, record = 0x800CD898;
    FUNCTION_MARKER(0x80075EE0, "RASHCDG.BIN");
    race_root_prepare_pool(delta);
    (void)sub_80073874(0x8005B298, delta);
    (void)sub_80073874(0x8005B2D8, delta);
    (void)sub_80074C84(0x8005B298, delta);
    (void)sub_80074C84(0x8005B2D8, delta);
    race_root_prepare_riders();
    race_root_controls(delta);
    race_root_physics(delta);
    race_root_dynamics(delta);
    race_root_external(delta);
    count = r_u32(r_u32(0x8005B2F8) + 48);
    if (!count)
        return 0x800D0000;
    do
    {
        uint32_t actor = r_u32(0x8005B268 + 4 * index), body = r_u32(actor + 852);
        uint32_t driving = r_u32(body + 604) < 3;
        if (driving)
        {
            uint32_t distance = (uint32_t)sub_8001FC90(r_s32(actor + 480), delta);
            w_u32(0x8005B380 + 4 * index, r_u32(0x8005B380 + 4 * index) + (uint32_t)((int32_t)distance >> 8));
        }
        body = r_u32(actor + 852);
        if ((r_u32(body + 552) & 0x80000) && r_u32(record + 540) != 6)
        {
            if (r_s32(record + 788) > 196608 || (driving && r_s32(actor + 44 + 4 * index) >= 4801))
                (void)sub_80092C7C(actor, 10);
            else
                w_u32(record + 788, r_u32(record + 788) + (uint32_t)delta);
        }
        record += 1132;
        count = r_u32(r_u32(0x8005B2F8) + 48);
        ++index;
    } while (index < count);
    return 0;
}

uint32_t sub_8008F138(uint32_t actor, int32_t delta)
{
    uint32_t size = 0;
    uint32_t result;
    uint32_t old_x;
    uint32_t old_y;
    uint32_t old_z;
    uint32_t type;

    FUNCTION_MARKER(0x8008F138u, "RASHCDG.BIN");
    result = (uint32_t)(int32_t)r_s16(actor + 320);
    if (!result)
        return result;
    old_y = r_u32(actor + 188);
    old_x = r_u32(actor + 184);
    old_z = r_u32(actor + 192);
    w_u32(actor + 472, old_y);
    type = r_u32(actor + 604);
    w_u32(actor + 468, old_x);
    w_u32(actor + 476, old_z);
    if (type == 2)
    {
        const uint32_t size_address = 0x1F8003F0u;
        uint32_t saved_size = r_u32(size_address);
        uint16_t kind = r_u16(actor + 544);
        uint32_t source;
        uint32_t third;
        uint32_t flags;

        w_u32(size_address, 0);
        if (kind == 38)
            (void)sub_8008EE60(actor);
        else if (kind == 39 || kind == 89)
            (void)sub_8008E818(actor, size_address);
        else if (kind == 40 || kind == 88)
            (void)sub_8008E50C(actor);
        else if (kind == 42 || kind == 91)
            (void)sub_8008E044(actor, size_address);
        else
            (void)sub_8008EB88(actor, size_address);
        size = r_u32(size_address);
        w_u32(size_address, saved_size);
        source = r_u32(actor + 596);
        w_u32(actor + 504, r_u32(source + 504));
        w_u32(actor + 508, r_u32(source + 508));
        flags = r_u32(actor + 552);
        third = r_u32(source + 512);
        w_u32(actor + 252, 0);
        w_u32(actor + 248, 0);
        w_u32(actor + 244, 0);
        w_u32(actor + 552, flags | 0xC0000000u);
        w_u8(actor + 547, 1);
        w_u32(actor + 512, third);
        (void)sub_8003FA18(9, xport_guest_cptr(actor + 432, 18), xport_guest_ptr(actor + 256, 18));
        (void)sub_8008DF74(actor);
    }
    else
    {
        uint32_t flags = r_u32(actor + 552);
        uint32_t active;

        if (flags & 0x40000000u)
            (void)sub_8002E570(xport_guest_ptr(actor + 184, 12), xport_guest_ptr(actor + 456, 12), (uint32_t)delta, xport_guest_ptr(actor + 184, 12));
        else if (!(flags & 0x18u))
        {
            uint32_t step = (uint32_t)sub_8001FC90(delta, (int32_t)r_u32(actor + 480));
            int32_t remaining;

            (void)sub_8002EAD8(xport_guest_ptr(actor + 184, 12), xport_guest_ptr(actor + 450, 6), step, xport_guest_ptr(actor + 184, 12));
            remaining = (int32_t)r_u32(actor + 508);
            if (remaining > 0)
            {
                uint32_t next = (uint32_t)remaining - 6553u;
                uint32_t displacement = (uint32_t)remaining;

                w_u32(actor + 508, next);
                if ((int32_t)next < 0)
                    w_u32(actor + 508, 0);
                else
                    displacement = 6553u;
                (void)sub_8002EAD8(xport_guest_ptr(actor + 184, 12), xport_guest_ptr(actor + 522, 6), displacement, xport_guest_ptr(actor + 184, 12));
            }
        }
        active = sub_800374D4(actor, 1) == 1;
        if (!active)
        {
            flags = r_u32(actor + 552);
            if (flags & 0x40000000u)
                active = 1;
            else
            {
                uint32_t source = r_u32(actor + 596);
                int32_t sequence = r_s8(source + 946);
                uint32_t entry = source + (uint32_t)(8 * sequence);

                active = (r_u16(entry + 954) & 0x8000u) != 0;
            }
        }
        if (active)
        {
            (void)sub_8009246C(actor);
            flags = r_u32(actor + 552);
            if (!(flags & 0x40000000u))
                w_u32(actor + 552, flags | 1u);
        }
    }
    (void)sub_8008BD2C(actor);
    if (size)
        w_u32(actor + 312, size);
    result = sub_80020018((uint32_t)((int32_t)r_s16(actor + 450) * 16), (uint32_t)((int32_t)r_s16(actor + 454) * 16));
    w_u32(actor + 292, result);
    {
        uint32_t cosine = (uint32_t)((int32_t)r_s16(0x8005624Eu + 4u * (result & 0xFFFu)) * 16);

        result = r_u32(actor + 292);
        w_u32(actor + 296, cosine);
        result = (uint32_t)((int32_t)r_s16(0x8005624Cu + 4u * (result & 0xFFFu)) * 16);
    }
    w_u32(actor + 300, result);
    return result;
}

uint32_t sub_8008F068(int32_t delta)
{
    FUNCTION_MARKER(0x8008F068u, "RASHCDG.BIN");
    int32_t count = ((int32_t)(uint32_t)(r_u32(r_u32(0x800CE4DC))));
    uint32_t actor = r_u32(0x800CE4D0);
    uint32_t stride = r_u32(0x800CE4D4);
    while (count-- >= 0)
    {
        uint32_t specification = r_u32(actor + 852);
        if (r_u32(specification + 604) >= 2)
            (void)sub_8008F138(specification, delta);
        if (r_u8(specification + 572) & 0x10)
        {
            uint32_t linked = r_u32(actor + 856);
            specification = r_u32(linked + 852);
            if (r_u32(specification + 604) >= 2)
                (void)sub_8008F138(specification, delta);
        }
        actor += stride;
    }
    return stride;
}

uint32_t sub_800A2898(int32_t delta)
{
    FUNCTION_MARKER(0x800A2898u, "RASHCDG.BIN");
    int32_t count = ((int32_t)(uint32_t)(r_u32(r_u32(0x800CE51C))));
    uint32_t actor = r_u32(0x800CE510);
    uint32_t stride = r_u32(0x800CE514);
    while (count-- >= 0)
    {
        if (r_u16(actor + 172))
        {
            if (r_u16(actor + 320) && ((int32_t)(uint32_t)(r_u32(actor + 480))) > 0)
            {
                uint32_t alternate = (r_u32(actor + 592) >> 1) & 1;
                uint32_t position = actor + 504 + ((0u - alternate) & 0x4C);
                uint32_t collision;
                w_u32(actor + 468, r_u32(actor + 184));
                w_u32(actor + 472, r_u32(actor + 188));
                w_u32(actor + 476, r_u32(actor + 192));
                (void)sub_8002E570(xport_guest_ptr(position, 12), xport_guest_ptr(actor + 456, 12), (uint32_t)delta, xport_guest_ptr(position, 12));
                if (alternate)
                    (void)sub_8002EAD8(xport_guest_ptr(actor + 580, 12), xport_guest_ptr(actor + 438, 6), (uint32_t)(((int32_t)(uint32_t)(r_u32(actor + 312))) >> 1), xport_guest_ptr(actor + 184, 12));
                else
                {
                    w_u32(actor + 184, r_u32(actor + 504));
                    w_u32(actor + 188, r_u32(actor + 508));
                    w_u32(actor + 192, r_u32(actor + 512));
                }
                collision = sub_80037450(actor);
                if (collision || alternate)
                {
                    (void)sub_800A3A84(actor);
                }
                if (!alternate)
                {
                    if (collision)
                    {
                        w_u32(actor + 184, r_u32(actor + 504));
                        w_u32(actor + 188, r_u32(actor + 508));
                        w_u32(actor + 192, r_u32(actor + 512));
                        w_u32(actor + 592, r_u32(actor + 592) | 0x80);
                    }
                    if (r_u32(actor + 592) & 4)
                        (void)sub_8002EAD8(xport_guest_ptr(actor + 504, 12), xport_guest_ptr(actor + 522, 6), 0u - r_u32(actor + 308), xport_guest_ptr(actor + 184, 12));
                }
                (void)sub_8008BA18(actor);
            }
            {
                uint32_t active = sub_80039F68(actor + 172);
                w_u16(actor + 320, (uint16_t)active);
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
    uint32_t result = r_u32(0x800CE4FC);
    int32_t count = (int32_t)r_u32(result);
    uint32_t actor = r_u32(0x800CE4F0);
    while (count >= 0)
    {
        if (r_u16(actor + 172))
        {
            if (r_u16(actor + 320))
            {
                uint32_t flags;
                uint32_t old_x = r_u32(actor + 184);
                uint32_t old_z = r_u32(actor + 192);
                w_u32(actor + 472, r_u32(actor + 188));
                flags = r_u32(actor + 552);
                w_u32(actor + 468, old_x);
                w_u32(actor + 476, old_z);
                if (flags & 0x20000000)
                {
                    if (flags & 0x40000000)
                    {
                        (void)sub_8002E570(xport_guest_ptr(actor + 184, 12), xport_guest_ptr(actor + 456, 12), (uint32_t)delta, xport_guest_ptr(actor + 184, 12));
                        (void)sub_80037450(actor);
                        (void)sub_8009246C(actor);
                    }
                    else
                    {
                        uint32_t step = (uint32_t)sub_8001FC90((uint32_t)delta, r_u32(actor + 480));
                        int32_t vertical;
                        (void)sub_8002EAD8(xport_guest_ptr(actor + 184, 12), xport_guest_ptr(actor + 450, 6), step, xport_guest_ptr(actor + 184, 12));
                        vertical = ((int32_t)(uint32_t)(r_u32(actor + 508)));
                        if (vertical > 0)
                        {
                            uint32_t vertical_step = vertical < 6553 ? (uint32_t)vertical : 6553;
                            vertical -= 6553;
                            if (vertical < 0)
                                vertical = 0;
                            w_u32(actor + 508, (uint32_t)vertical);
                            (void)sub_8002EAD8(xport_guest_ptr(actor + 184, 12), xport_guest_ptr(actor + 522, 6), vertical_step, xport_guest_ptr(actor + 184, 12));
                        }
                        if (sub_80037450(actor))
                        {
                            (void)sub_8009246C(actor);
                            w_u32(actor + 568, r_u32(actor + 568) | 1);
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
                    uint32_t angle = sub_80020018((uint32_t)((int32_t)r_s16(actor + 444) * 16), (uint32_t)((int32_t)r_s16(actor + 448) * 16));
                    w_u32(actor + 292, angle);
                    w_u32(actor + 296, (uint32_t)((int32_t)r_s16(0x8005624C + 4 * (angle & 0xFFF) + 2) * 16));
                    w_u32(actor + 300, (uint32_t)((int32_t)r_s16(0x8005624C + 4 * (angle & 0xFFF)) * 16));
                }
            }
            w_u16(actor + 320, (uint16_t)sub_80039F68(actor + 172));
        }
        result = r_u32(0x800CE4F4);
        actor += result;
        --count;
    }
    return result;
}

static uint32_t race_ca_step(uint32_t actor, int32_t *step)
{
    int32_t scalar_value_181;
    int32_t scalar_sign_181;
    int32_t scalar_value_182;
    int32_t scalar_sign_182;
    int32_t scalar_value_183;
    int32_t scalar_sign_183;

    int32_t side = ((int32_t)(uint32_t)(r_u32(actor + 344)));
    int32_t mode = ((int32_t)(uint32_t)(r_u32(actor + 388))) >> 20;
    int32_t limit = (int32_t)((scalar_value_181 = (((int32_t)(uint32_t)(r_u32(actor + 556))) - ((int32_t)(uint32_t)(r_u32(actor + 560))))), (scalar_sign_181 = scalar_value_181 >> 31), (((uint32_t)((uint32_t)((scalar_sign_181 + scalar_value_181) ^ scalar_sign_181)))));
    int32_t boundary = 0;
    if (!(mode & 0xF))
        boundary = side >= 0 ? (int32_t)((scalar_value_182 = (((int32_t)(uint32_t)(r_u32(actor + 412))) - side)), (scalar_sign_182 = scalar_value_182 >> 31), (((uint32_t)((uint32_t)((scalar_sign_182 + scalar_value_182) ^ scalar_sign_182))))) : (int32_t)((scalar_value_183 = (((int32_t)(uint32_t)(r_u32(actor + 400))) - side)), (scalar_sign_183 = scalar_value_183 >> 31), (((uint32_t)((uint32_t)((scalar_sign_183 + scalar_value_183) ^ scalar_sign_183)))));
    if (boundary < limit && *step >= boundary)
    {
        int32_t dot = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(r_u32(actor + 340) + 2, 6), xport_guest_ptr(actor + 444, 6))));
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

static void race_negate_short_vector(uint32_t vector)
{
    w_u16(vector, (uint16_t)-(int16_t)r_u16(vector));
    w_u16(vector + 2, (uint16_t)-(int16_t)r_u16(vector + 2));
    w_u16(vector + 4, (uint16_t)-(int16_t)r_u16(vector + 4));
}

static void race_ca_reflect(uint32_t actor)
{
    race_negate_short_vector(actor + 450);
    w_u32(actor + 560, 0);
    w_u16(actor + 434, 0);
    w_u16(actor + 444, r_u16(actor + 450));
    w_u16(actor + 446, r_u16(actor + 452));
    w_u16(actor + 448, r_u16(actor + 454));
    w_u16(actor + 432, r_u16(actor + 454));
    w_u16(actor + 436, (uint16_t)-(int16_t)r_u16(actor + 450));
    {
        uint8_t state = r_u8(actor + 565);
        w_u8(actor + 565, (uint8_t)(((state & 2) ? 0 : 2) | (state & 0xFD)));
    }
}

uint32_t sub_800CA564(uint32_t actor, int32_t delta)
{
    FUNCTION_MARKER(0x800CA564u, "RASHCDG.BIN");
    int32_t step = (int32_t)sub_8001FC90((uint32_t)delta, r_u32(actor + 480));
    uint32_t piece = r_u32(actor + 340);
    if (!(r_u8(actor + 565) & 4))
    {
        w_u16(actor + 438, 0);
        w_u16(actor + 440, 4096);
        w_u16(actor + 442, 0);
        race_copy_halfwords(actor + 444, piece + 2, 3);
        if (!r_u8(actor + 564) && ((int32_t)(uint32_t)(r_u32(actor + 344))) > 0)
            race_negate_short_vector(actor + 444);
        race_outer_product16(actor + 438, actor + 444, actor + 432);
        race_copy_halfwords(actor + 450, actor + 444, 3);
        xport_update_u8(actor + 565, XPORT_MEMORY_UPDATE_OR, 4);
        w_u32(actor + 556, r_u32(actor + 412) - r_u32(actor + 344));
    }
    if (race_ca_step(actor, &step))
    {
        (void)sub_8002EAD8(xport_guest_ptr(actor + 184, 12), xport_guest_ptr(actor + 450, 6), (uint32_t)step, xport_guest_ptr(actor + 184, 12));
        race_ca_reflect(actor);
        w_u32(actor + 556, r_u32(actor + 412) - r_u32(actor + 400));
    }
    else
    {
        w_u32(actor + 560, r_u32(actor + 560) + (uint32_t)step);
        (void)sub_8002EAD8(xport_guest_ptr(actor + 184, 12), xport_guest_ptr(actor + 450, 6), (uint32_t)step, xport_guest_ptr(actor + 184, 12));
    }
    (void)sub_80037450(actor);
    w_u32(actor + 324, sub_8003B61C(actor + 172));
    {
        uint32_t offset = sub_800B6AAC(xport_guest_ptr(actor + 184, 12), xport_guest_ptr(piece + 8, 6), xport_guest_ptr(piece + 20, 12));
        return sub_8002EAD8(xport_guest_ptr(actor + 184, 12), xport_guest_ptr(piece + 8, 6), 0u - offset, xport_guest_ptr(actor + 184, 12));
    }
}

uint32_t sub_800CA7C4(uint32_t actor, int32_t delta)
{
    int32_t step;
    uint32_t old_side;
    uint32_t piece;
    uint16_t x;
    uint16_t y;
    uint16_t z;
    uint8_t state;
    uint32_t result;

    FUNCTION_MARKER(0x800CA7C4, "RASHCDG.BIN");
    step = (int32_t)sub_8001FC90(delta, (int32_t)r_u32(actor + 480));
    old_side = r_u32(actor + 344);
    if (!(r_u8(actor + 565) & 4))
    {
        piece = r_u32(actor + 340);
        w_u16(actor + 438, 0);
        w_u16(actor + 440, 4096);
        w_u16(actor + 442, 0);
        w_u16(actor + 444, r_u16(piece + 14));
        w_u16(actor + 446, r_u16(piece + 16));
        z = r_u16(piece + 18);
        x = r_u16(actor + 444);
        state = r_u8(actor + 565);
        w_u16(actor + 434, 0);
        w_u8(actor + 565, state | 4);
        w_u16(actor + 448, z);
        w_u16(actor + 444, (uint16_t)(0 - x));
        y = r_u16(actor + 446);
        z = (uint16_t)(0 - z);
        w_u16(actor + 448, z);
        w_u16(actor + 432, z);
        w_u16(actor + 454, z);
        w_u16(actor + 446, (uint16_t)(0 - y));
        x = r_u16(actor + 444);
        y = r_u16(actor + 446);
        w_u16(actor + 436, (uint16_t)(0 - x));
        w_u16(actor + 450, x);
        w_u16(actor + 452, y);
    }
    if (race_ca_step(actor, &step))
    {
        (void)sub_8002EAD8(xport_guest_ptr(actor + 184, 12), xport_guest_ptr(actor + 450, 6), (uint32_t)step, xport_guest_ptr(actor + 184, 12));
        x = r_u16(actor + 450);
        y = r_u16(actor + 452);
        state = r_u8(actor + 565);
        w_u32(actor + 560, 0);
        w_u16(actor + 434, 0);
        w_u16(actor + 450, (uint16_t)(0 - x));
        z = r_u16(actor + 454);
        w_u16(actor + 452, (uint16_t)(0 - y));
        x = r_u16(actor + 450);
        y = r_u16(actor + 452);
        w_u16(actor + 454, (uint16_t)(0 - z));
        z = r_u16(actor + 454);
        w_u16(actor + 444, x);
        w_u8(actor + 565, ((state & 2) ? 0 : 2) | (state & 0xFD));
        w_u16(actor + 446, y);
        w_u16(actor + 436, (uint16_t)(0 - x));
        w_u16(actor + 448, z);
        w_u16(actor + 432, z);
    }
    else
    {
        w_u32(actor + 560, r_u32(actor + 560) + (uint32_t)step);
        (void)sub_8002EAD8(xport_guest_ptr(actor + 184, 12), xport_guest_ptr(actor + 450, 6), (uint32_t)step, xport_guest_ptr(actor + 184, 12));
    }
    (void)sub_80037450(actor);
    result = sub_8003B61C(actor + 172);
    piece = r_u32(actor + 340);
    w_u32(actor + 324, result);
    result = sub_800B6AAC(xport_guest_ptr(actor + 184, 12), xport_guest_ptr(piece + 8, 6), xport_guest_ptr(piece + 20, 12));
    (void)sub_8002EAD8(xport_guest_ptr(actor + 184, 12), xport_guest_ptr(r_u32(actor + 340) + 8, 6), 0 - result, xport_guest_ptr(actor + 184, 12));
    (void)sub_8002EAD8(xport_guest_ptr(actor + 184, 12), xport_guest_ptr(r_u32(actor + 340) + 2, 6), old_side - r_u32(actor + 344), xport_guest_ptr(actor + 184, 12));
    piece = r_u32(actor + 340);
    w_u32(actor + 344, old_side);
    w_u16(actor + 450, r_u16(piece + 14));
    w_u16(actor + 452, r_u16(piece + 16));
    state = r_u8(actor + 565);
    z = r_u16(piece + 18);
    w_u16(actor + 454, z);
    if (!(state & 2))
    {
        x = r_u16(actor + 450);
        w_u16(actor + 450, (uint16_t)(0 - x));
        y = r_u16(actor + 452);
        w_u16(actor + 454, (uint16_t)(0 - z));
        w_u16(actor + 452, (uint16_t)(0 - y));
    }
    x = r_u16(actor + 450);
    y = r_u16(actor + 452);
    z = r_u16(actor + 454);
    w_u16(actor + 434, 0);
    w_u16(actor + 444, x);
    result = 0 - (uint32_t)x;
    w_u16(actor + 446, y);
    w_u16(actor + 448, z);
    w_u16(actor + 432, z);
    w_u16(actor + 436, (uint16_t)result);
    return result;
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
    int32_t count = ((int32_t)(uint32_t)(r_u32(r_u32(0x800CE52C))));
    uint32_t actor = r_u32(0x800CE520);
    uint32_t stride = r_u32(0x800CE524);
    while (count-- >= 0)
    {
        uint16_t identity = r_u16(actor + 172);
        if (identity && (identity >> 5) == 5)
        {
            uint32_t active = sub_80039F68(actor + 172);
            w_u16(actor + 320, (uint16_t)active);
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
        uint32_t record = sub_80013204(r_u32(identity + 4), player);
        if (record && r_u32(record + 8) == r_u32(identity + 4))
        {
            uint32_t table = r_u32(r_u32(r_u32(record + 4) + 32) + 48);
            w_u16(table + 88 * r_u8(identity + 2) + 4, 0);
        }
    }
}

uint32_t sub_8008DCA0(uint32_t player)
{
    FUNCTION_MARKER(0x8008DCA0u, "RASHCDG.BIN");
    uint32_t state = 0x800D5CF8 + 32 * player;
    int32_t count = ((int32_t)(uint32_t)(r_u32(state)));
    uint32_t table = r_u32(state + 4);
    int32_t index;
    for (index = 0; index < count; ++index)
    {
        w_u16(table + 8 * index + 2, 0xFFFF);
        w_u32(table + 8 * index + 4, 0xFFFFFFFFu);
    }
    for (index = 0; index < 3; ++index)
    {
        w_u32(state + 4 * index + 8, 0);
        w_u32(state + 4 * index + 20, 0xFFFFFFFFu);
    }
    return index < 3;
}

uint32_t sub_8008DE18(uint32_t player, uint32_t group, uint32_t ordinal, uint32_t object)
{
    FUNCTION_MARKER(0x8008DE18u, "RASHCDG.BIN");
    uint32_t state = 0x800D5CF8 + 32 * player;
    uint32_t group_state = state + 4 * group;
    int32_t tail = ((int32_t)(uint32_t)(r_u32(group_state + 20)));
    uint32_t table = r_u32(state + 4);
    w_u32(group_state + 8, r_u32(group_state + 8) + 1);
    if (tail == -1)
        w_u32(group_state + 20, ordinal);
    else
    {
        int32_t next;
        while ((next = (int16_t)r_u16(table + 8 * tail + 2)) != -1)
            tail = next;
        w_u16(table + 8 * tail + 2, (uint16_t)ordinal);
    }
    w_u32(table + 8 * ordinal + 4, object);
    return table + 8 * ordinal;
}

uint32_t sub_8008DD20(uint32_t player, uint32_t object, uint32_t ordinal)
{
    int32_t scalar_sign_184;
    int32_t scalar_sign_185;

    FUNCTION_MARKER(0x8008DD20u, "RASHCDG.BIN");
    uint32_t record = 0x800CD898 + 1132 * player;
    int32_t first = (int16_t)r_u16(record + 186) - (int16_t)r_u16(object + 14);
    int32_t second = (int16_t)r_u16(record + 194) - (int16_t)r_u16(object + 22);
    int32_t large = (int32_t)((scalar_sign_184 = ((int32_t)(first)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_184 + ((int32_t)(first))) ^ scalar_sign_184)))));
    int32_t small = (int32_t)((scalar_sign_185 = ((int32_t)(second)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_185 + ((int32_t)(second))) ^ scalar_sign_185)))));
    int32_t distance;
    uint32_t group = 0;
    if (large < small)
    {
        int32_t swap = large;
        large = small;
        small = swap;
    }
    distance = large - (large >> 5) - (large >> 7) + ((small + (small >> 1)) >> 2) + ((small + (small >> 1)) >> 6);
    if (distance >= (((int32_t)(uint32_t)(r_u32(0x8005AD58))) >> 18))
        group = distance < (((int32_t)(uint32_t)(r_u32(0x8005AD58))) >> 16) ? 1 : 2;
    return sub_8008DE18(player, group, ordinal, (uint32_t)distance);
}

uint32_t sub_8008D89C(void)
{
    FUNCTION_MARKER(0x8008D89Cu, "RASHCDG.BIN");
    uint32_t players = r_u32(r_u32(0x8005B2F8) + 48);
    int32_t count = ((int32_t)(uint32_t)(r_u32(r_u32(0x800CE53C))));
    uint32_t object = r_u32(0x800CE530);
    uint32_t stride = r_u32(0x800CE534);
    uint32_t player;
    w_u32(0x800CD6B8, 0);
    w_u32(0x800CD6BC, 0);
    w_u32(0x800CD6C0, 0);
    for (player = 0; player < players; ++player)
        (void)sub_8008DCA0(player);
    while (count >= 0)
    {
        if (r_u16(object))
        {
            uint8_t mask = r_u8(object + 3);
            if (mask & 1)
            {
                w_u32(0x800CD6B8, r_u32(0x800CD6B8) + 1);
                (void)sub_8008DD20(0, object, (uint32_t)count);
            }
            if (mask & 2)
            {
                w_u32(0x800CD6BC, r_u32(0x800CD6BC) + 1);
                (void)sub_8008DD20(1, object, (uint32_t)count);
            }
            if (mask == 3)
                w_u32(0x800CD6C0, r_u32(0x800CD6C0) + 1);
        }
        --count;
        object += stride;
    }
    return stride;
}

uint32_t sub_8009AB60(void)
{
    FUNCTION_MARKER(0x8009AB60u, "RASHCDG.BIN");
    int32_t count = ((int32_t)(uint32_t)(r_u32(r_u32(0x800CE53C))));
    uint32_t identity = r_u32(0x800CE530);
    uint32_t stride = r_u32(0x800CE534);
    uint32_t players = r_u32(r_u32(0x8005B2F8) + 48);
    while (count-- >= 0)
    {
        if (r_u16(identity))
        {
            uint32_t active = sub_80039F68(identity);
            uint32_t player;
            w_u16(identity + 148, (uint16_t)active);
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

uint32_t sub_800A4774(int32_t delta)
{
    uint32_t nodes[4], keys[4];
    uint16_t normal[3];
    uint32_t node_count = 0, group, actor, position, point_count, identity, point;
    uint32_t x, z, cell, players, old, flags, enabled, phase, pass, radius, special;
    uint32_t packed, candidate, candidate_position, table, stride, dx, dz, sign, minimum, metric;
    uint32_t old_first, old_second, state, linked, sound, actor_moving, candidate_moving, magnitude;
    int32_t kind, column, row, remaining;
    FUNCTION_MARKER(0x800A4774, "RASHCDG.BIN");
    w_u32(0x800CCE38, (uint32_t)delta);
    for (row = 0; row < 48; ++row)
    {
        for (point = 0; point < 6; ++point)
            w_u32(0x800CD0B0 + 24 * (uint32_t)row + 4 * point, 0x80808080);
    }
    actor = r_u32(0x8005B38C);
    w_u32(0x800CCF98, r_u32(actor + 184) + 0xF0000);
    w_u32(0x800CCFA0, r_u32(actor + 192) + 0xF0000);
    if (r_u32(r_u32(0x8005B2F8) + 48) >= 2)
    {
        actor = r_u32(0x8005B21C);
        w_u32(0x800CCF9C, r_u32(actor + 184) + 0xF0000);
        w_u32(0x800CCFA4, r_u32(actor + 192) + 0xF0000);
    }
    for (kind = 6; kind >= 0; --kind)
    {
        group = 0x800CE4D0 + 16 * (uint32_t)kind;
        if (!r_u32(r_u32(group + 8)))
            continue;
        actor = r_u32(group);
        remaining = r_s32(r_u32(group + 12));
        while (remaining >= 0)
        {
            point_count = 4;
            position = actor + 196;
            if (kind == 1 && r_u32(actor + 604) < 2)
                goto next_insert;
            if (kind == 6)
            {
                identity = r_u16(actor);
                if (!identity || !r_s16(actor + 148))
                    goto next_insert;
                position = actor + 24;
                if (r_u32(actor + 8) == 1)
                {
                    position = actor + 12;
                    point_count = 1;
                }
            }
            else
            {
                identity = r_u16(actor + 172);
                if (!r_s16(actor + 320) || (kind && !identity))
                    goto next_insert;
                if (!kind)
                {
                    flags = r_u32(actor + 568);
                    enabled = r_u32(actor + 616);
                    w_u32(actor + 568, flags & 0xFDFFFFFF);
                    if (!enabled)
                        enabled = r_u32(actor + 676);
                    flags = r_u32(actor + 568);
                    w_u32(actor + 552, 0);
                    w_u32(actor + 568, flags | ((enabled != 0) << 24));
                }
            }
            for (point = 0; point < point_count; ++point, position += 12)
            {
                x = r_u32(position);
                z = r_u32(position + 8);
                column = ((int32_t)(x - r_u32(0x800CCF98)) >> 21) + 11;
                row = ((int32_t)(z - r_u32(0x800CCFA0)) >> 21) + 11;
                players = r_u32(r_u32(0x8005B2F8) + 48);
                if (players != 1 && ((uint32_t)column >= 24 || (uint32_t)row >= 24))
                {
                    column = ((int32_t)(x - r_u32(0x800CCF9C)) >> 21) + 11;
                    row = ((int32_t)(z - r_u32(0x800CCFA4)) >> 21) + 11;
                    if (row >= 0)
                        row += 24;
                }
                if ((uint32_t)column >= 24)
                    continue;
                players = r_u32(r_u32(0x8005B2F8) + 48);
                if ((uint32_t)row >= (24u << ((players - 1) & 31)))
                    continue;
                cell = 0x800CD0B0 + 24 * (uint32_t)row + (uint32_t)column;
                old = r_u8(cell);
                if (old == 128 || r_u8(0x800CCFA9 + 2 * old) != identity)
                {
                    w_u8(0x800CCFA8 + 2 * node_count, old);
                    w_u8(0x800CCFA9 + 2 * node_count, identity);
                    w_u8(cell, node_count);
                    if (++node_count == 128)
                        goto grid_ready;
                }
            }
        next_insert:
            stride = r_u32(group + 4);
            --remaining;
            actor += stride;
        }
    }
grid_ready:
    if (!node_count)
        return 0;
    w_u32(0x800CCF68, 0);
    (void)sub_8001E100(0x800CCE48, 0, 288);
    old_first = r_u32(0x800CCF78);
    old_second = r_u32(0x800CCF7C);
    w_u32(0x800CCF78, 1);
    w_u32(0x800CCF80, 0);
    w_u32(0x800CCF90, 0);
    w_u32(0x800CCF7C, 0);
    w_u32(0x800CCF70, old_first);
    w_u32(0x800CCF74, old_second);
    (void)sub_8001E100(0x800CCF88, 0, 8);
    for (phase = 0; phase < 6; ++phase)
    {
        pass = phase ? phase - 1 : 0;
        group = 0x800CE4D0 + 16 * pass;
        if (!r_u32(r_u32(group + 8)))
            continue;
        actor = r_u32(group);
        remaining = r_s32(r_u32(group + 12));
        while (remaining >= 0)
        {
            if (!r_s16(actor + 320))
                goto next_actor;
            radius = r_u32(0x800CCA8C);
            if (phase)
            {
                if ((pass && !r_u16(actor + 172)) || (pass == 1 && r_u32(actor + 604) < 2))
                    goto next_actor;
                special = (r_u32(actor + 388) & 1) != 0;
                if (!special && r_u32(actor + 372) && r_s16(actor + 392) != 4)
                    special = 1;
                radius = r_u32(0x800CCA8C + 4 * pass) + (special ? 0xA0000 : 0);
            }
            identity = r_u16(actor + 172);
            if (phase && pass == 4 && !r_u32(actor + 480))
                goto next_actor;
            for (point = 0; point < 4; ++point)
            {
                position = actor + 196 + 12 * point;
                x = r_u32(position);
                z = r_u32(position + 8);
                column = ((int32_t)(x - r_u32(0x800CCF98)) >> 21) + 11;
                row = ((int32_t)(z - r_u32(0x800CCFA0)) >> 21) + 11;
                players = r_u32(r_u32(0x8005B2F8) + 48);
                if (players != 1 && ((uint32_t)column >= 24 || (uint32_t)row >= 24))
                {
                    column = ((int32_t)(x - r_u32(0x800CCF9C)) >> 21) + 11;
                    row = ((int32_t)(z - r_u32(0x800CCFA4)) >> 21) + 11;
                    if (row >= 0)
                        row += 24;
                }
                nodes[point] = 128;
                if ((uint32_t)column >= 24)
                    continue;
                players = r_u32(r_u32(0x8005B2F8) + 48);
                if ((uint32_t)row >= (24u << ((players - 1) & 31)))
                    continue;
                old = r_u8(0x800CD0B0 + 24 * (uint32_t)row + (uint32_t)column);
                while (old != 128 && r_u8(0x800CCFA9 + 2 * old) != identity)
                    old = r_u8(0x800CCFA8 + 2 * old);
                nodes[point] = r_u8(0x800CCFA8 + 2 * old);
            }
            while (nodes[0] != 128 || nodes[1] != 128 || nodes[2] != 128 || nodes[3] != 128)
            {
                for (point = 0; point < 4; ++point)
                    keys[point] = r_u8(0x800CCFA9 + 2 * nodes[point]);
                packed = keys[0] < keys[1] ? keys[0] : keys[1];
                old = keys[2] < keys[3] ? keys[2] : keys[3];
                if (old < packed)
                    packed = old;
                if ((packed >> 5) == 6)
                {
                    candidate = r_u32(0x800CD6C4) + 280 * (packed & 31);
                    candidate_position = candidate + 12;
                }
                else
                {
                    table = 0x800CE4D0 + ((packed >> 1) & 0x7FF0);
                    stride = r_u32(table + 4);
                    candidate = r_u32(table) + stride * (packed & 31);
                    candidate_position = candidate + 184;
                }
                dx = r_u32(candidate_position) - r_u32(actor + 184);
                dz = r_u32(candidate_position + 8) - r_u32(actor + 192);
                sign = (uint32_t)((int32_t)dx >> 31);
                dx = (dx + sign) ^ sign;
                sign = (uint32_t)((int32_t)dz >> 31);
                dz = (dz + sign) ^ sign;
                minimum = (int32_t)dx < (int32_t)dz ? dx : dz;
                metric = dx + dz - (uint32_t)((int32_t)minimum / 2);
                if ((int32_t)metric < (int32_t)radius)
                {
                    kind = (int32_t)(packed >> 5);
                    if (!phase)
                    {
                        if (!kind)
                        {
                            flags = r_u32(actor + 568);
                            magnitude = r_u32(actor + 652);
                            sign = (uint32_t)((int32_t)magnitude >> 31);
                            magnitude = (magnitude + sign) ^ sign;
                            actor_moving = (flags & 0x1E0) && ((flags & 0x200000) || (int32_t)magnitude > 45752);
                            flags = r_u32(candidate + 568);
                            magnitude = r_u32(candidate + 652);
                            sign = (uint32_t)((int32_t)magnitude >> 31);
                            magnitude = (magnitude + sign) ^ sign;
                            candidate_moving = (flags & 0x1E0) && ((flags & 0x200000) || (int32_t)magnitude > 45752);
                            if ((candidate_moving || !actor_moving) && r_u16(r_u32(actor + 852) + 544))
                            {
                                if ((actor_moving || !candidate_moving) && r_u16(r_u32(candidate + 852) + 544))
                                    (void)sub_800AB7A0(actor, candidate, 0);
                                else
                                    (void)sub_800B09C4(actor, candidate + 172);
                            }
                            else
                                (void)sub_800B09C4(candidate, actor + 172);
                        }
                    }
                    else if (!pass)
                    {
                        switch (kind)
                        {
                            case 1:
                            case 2:
                                if ((uint32_t)(r_u16(candidate + 544) - 60) >= 4)
                                    (void)sub_800AD04C(actor, candidate);
                                break;
                            case 3:
                                (void)sub_800AC5BC(actor, candidate);
                                break;
                            case 4:
                                if ((r_u16(r_u32(candidate) + 14) & 2) && !(r_u32(candidate + 592) & 0x1A06))
                                    (void)sub_800AE794(actor, candidate + 172);
                                else
                                    (void)sub_800B09C4(actor, candidate + 172);
                                break;
                            case 5:
                                if (r_s32(candidate + 304) > 65536 && r_s32(candidate + 308) > 65536)
                                    (void)sub_800B0D8C(actor, candidate + 172);
                                else
                                    (void)sub_800B09C4(actor, candidate + 172);
                                break;
                            case 6:
                                if (r_u32(candidate + 8))
                                    (void)sub_800AE794(actor, candidate);
                                else if (r_s32(candidate + 132) > 65536 && r_s32(candidate + 136) > 65536)
                                    (void)sub_800B0D8C(actor, candidate);
                                else
                                    (void)sub_800B09C4(actor, candidate);
                                break;
                        }
                    }
                    else if (pass == 1 || pass == 2)
                    {
                        if (kind == 1 || kind == 2)
                            sub_800B2AF8();
                        else if (kind == 3)
                            (void)sub_800B2844(actor, candidate);
                        else if (kind == 4 || kind == 5)
                            (void)sub_800B2B00(actor, candidate + 172);
                        else if (kind == 6)
                            (void)sub_800B2B00(actor, candidate);
                    }
                    else if (pass == 3)
                    {
                        if (kind == 3)
                            (void)sub_800B2D44(actor, candidate);
                        else if (kind == 4)
                            (void)sub_800B2D88(actor, candidate);
                    }
                    else if (pass == 4)
                    {
                        if (kind == 5)
                            (void)sub_800B2E64(actor, candidate + 172);
                        else if (kind == 6)
                            (void)sub_800B2E64(actor, candidate);
                    }
                }
                for (point = 0; point < 4; ++point)
                {
                    if (r_u8(0x800CCFA9 + 2 * nodes[point]) == packed)
                        nodes[point] = r_u8(0x800CCFA8 + 2 * nodes[point]);
                }
            }
            if (phase && !pass)
            {
                state = sub_800B3AD0(actor, (r_u32(actor + 568) & 0x600) != 0);
                linked = r_u32(actor + 856);
                if (linked && (((state & 1) != ((state & 2) >> 1)) || (!state && ((r_s32(actor + 388) >> 20) & 15))))
                    (void)sub_800B3AD0(linked, (r_u32(actor + 568) & 0x600) != 0);
                if (!(r_u32(actor + 568) & 0x600) || (r_u32(actor + 564) & 0x800000))
                {
                    if ((r_u32(actor + 564) & 0x400000) && !(r_u32(actor + 568) & 15) && sub_80083F30(actor, actor + 172, xport_guest_ptr(actor + 820, 6), 4))
                    {
                        sound = (r_u32(actor + 564) & 14) ? 17 : 3;
                        sub_80017BA0(r_s32(actor + 184), r_s32(actor + 192), sound, 0);
                        if (r_u16(actor + 172) < r_u32(r_u32(0x8005B2F8) + 48) && r_u32(r_u32(actor + 852) + 604) < 2 && !r_u32(0x8005B220))
                            (void)sub_800B658C(actor, 0, r_s32(actor + 480), 1464860, 1);
                    }
                }
                else
                    (void)sub_800B1978(actor);
            }
            else if (phase && (pass == 1 || pass == 2))
            {
                (void)sub_800B3AD0(actor, (r_u32(actor + 552) >> 30) & 1);
                if ((r_u32(actor + 552) & 0xC0000000) == 0x40000000 && (int32_t)sub_8002E698(xport_guest_cptr(actor + 522, 6), xport_guest_cptr(actor + 450, 6)) > 0)
                    (void)sub_800B208C(actor);
            }
            else if (phase && pass == 4)
            {
                (void)sub_800B3AD0(actor, (r_u32(actor + 592) >> 1) & 1);
                if ((r_u32(actor + 592) & 3) == 2 && (int32_t)sub_8002E698(xport_guest_cptr(actor + 522, 6), xport_guest_cptr(actor + 450, 6)) > 0)
                {
                    for (point = 0; point < 3; ++point)
                        normal[point] = (uint16_t)(0 - r_u16(actor + 522 + 2 * point));
                    (void)sub_800B3344(actor, actor + 504, normal, 1);
                }
            }
        next_actor:
            stride = r_u32(group + 4);
            --remaining;
            actor += stride;
        }
        if (phase == 1 && r_u32(0x800CCF68))
            (void)sub_800A77B0();
    }
    return 0;
}

static void race_collision_negate_normal(uint32_t normal)
{
    uint32_t axis;
    for (axis = 0; axis < 3; ++axis)
        w_u16(normal + 2 * axis, (uint16_t)-(int16_t)r_u16(normal + 2 * axis));
}

static uint32_t race_collision_return(const uint8_t saved[256], uint32_t result)
{
    xport_guest_copy(xport_guest_ref(0x1F800100), xport_host_ref((void *)saved), 256);
    return result;
}

uint32_t sub_800B3AD0(uint32_t actor, uint32_t mode)
{
    int32_t scalar_value_190;
    int32_t scalar_sign_190;
    int32_t scalar_sign_191;
    int32_t scalar_sign_192;
    int32_t scalar_value_193;
    int32_t scalar_sign_193;
    int32_t scalar_sign_198;
    int32_t scalar_sign_199;
    int32_t scalar_sign_200;
    int32_t scalar_value_201;
    int32_t scalar_sign_201;
    int32_t scalar_sign_202;
    int32_t scalar_sign_203;
    int32_t scalar_sign_204;
    int32_t scalar_sign_205;
    int32_t scalar_sign_206;
    int32_t scalar_sign_207;
    int32_t scalar_sign_208;

    const uint32_t scratch = 0x1F800100;
    const uint32_t normal = scratch;
    const uint32_t edge_a = scratch + 8;
    const uint32_t edge_b = scratch + 20;
    const uint32_t probe_a = scratch + 32;
    const uint32_t response = scratch + 48;
    const uint32_t contact_point = scratch + 64;
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
    memcpy(saved, xport_guest_ptr(scratch, 256), 256);
    previous_material = r_u32(actor + 388) >> 20;
    w_u32(actor + 388, r_u32(actor + 388) & 0x000FFFEFu);
    model = r_u32(actor + 328);
    if ((r_u16(actor + 172) >> 5) == 4 && (r_u16(actor + 172) & 31) >= 30)
    {
        special = 1;
        special_actor = actor;
        w_u32(actor + 768, 0);
    }
    {
        uint32_t direct_group = r_u32(actor + 496);
        if (direct_group)
        {
            segment_count = r_u8(direct_group + 3);
            if (!segment_count)
                return race_collision_return(saved, 0);
            if (!(r_u32(actor + 388) & 1))
            {
                if (!r_u32(actor + 372) || r_u16(actor + 392) == 4)
                    return race_collision_return(saved, 0);
            }
            side_flags = 0;
            flags = 0x100;
            group_count = 1;
            segment_start = (uint32_t)(int32_t)r_s16(direct_group);
        }
        else
        {
            uint32_t selector = r_u32(actor + 492);
            int32_t travel = ((int32_t)(uint32_t)(r_u32(actor + 344)));
            uint32_t entry;
            if (!selector || !r_u32(actor + 372))
                return race_collision_return(saved, 0);
            if (travel >= 0)
            {
                group_count = r_u8(selector + 3);
                if (!group_count)
                    return race_collision_return(saved, 0);
                group_index = r_u8(selector + 2);
                if ((int32_t)(r_u32(actor + 424) * (uint32_t)(int32_t)r_s16(actor + 420) - 0x8000u) < travel)
                    flags = 0x80;
            }
            else
            {
                group_count = r_u8(selector + 1);
                if (!group_count)
                    return race_collision_return(saved, 0);
                group_index = r_u8(selector);
                if ((int32_t)(r_u32(actor + 424) * (uint32_t)(int32_t)r_s16(actor + 408) - 0x8000u) < (int32_t)(0u - (uint32_t)travel))
                    flags = 0x40;
            }
            entry = r_u32(model + 100) + 4 * group_index;
            segment_count = r_u8(entry + 3);
            segment_start = (uint32_t)(int32_t)r_s16(entry);
            if (!mode && !special && !flags)
                return race_collision_return(saved, 0);
        }
    }

    {
        uint32_t type = r_u16(actor + 172) >> 5;
        uint32_t linked_mode = type == 0;
        if (!type)
        {
            tracked = actor;
            linked_mode += r_u32(actor + 1088) == 0;
            if (r_u32(actor + 1088))
                (void)(uint32_t)sub_800A8FE8(actor);
            if ((int32_t)(((scalar_value_190 = (((int32_t)(uint32_t)(r_u32(actor + 676))))), (scalar_sign_190 = scalar_value_190 >> 31), (((uint32_t)((uint32_t)(((uint32_t)scalar_sign_190 + (uint32_t) scalar_value_190) ^ scalar_sign_190)))))) >= 2130)
                state = 2;
            if (r_u32(actor + 856) && r_u32(actor + 1088) && !(r_u32(actor + 320) & 8))
            {
                (void)(uint32_t)sub_800A8C78(actor, 1310720, 1835008);
                w_u16(r_u32(actor + 856) + 320, r_u16(actor + 320));
            }
            if (((int32_t)(uint32_t)(r_u32(actor + 636))) < 0)
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
                uint32_t entry = r_u32(model + 100) + 4 * group_index;
                uint32_t next_count = r_u8(entry + 3);
                segment_start = (uint32_t)(int32_t)r_s16(entry);
                segment_count = next_count;
                if (special && r_u32(special_actor + 768))
                    return race_collision_return(saved, 1);
            }
            geometry = r_u32(model + 96) + 40 * segment_start;
            surface = geometry;
            last_surface = geometry + 40 * segment_count - 40;
            direction = 1;
            {
                uint32_t basis = r_u32(actor + 340);
                int32_t first = ((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(geometry + 8, 12), xport_guest_ptr(basis + 2, 6), xport_guest_ptr(basis + 20, 12))));
                int32_t second = ((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(last_surface + 20, 12), xport_guest_ptr(basis + 2, 6), xport_guest_ptr(basis + 20, 12))));
                (void)sub_8002EAD8(xport_guest_ptr(last_surface + 20, 12), xport_guest_ptr(basis + 2, 6), (uint32_t)(first - second), xport_guest_ptr(edge_b, 12));
                if (((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(edge_b, 12), xport_guest_ptr(actor_normal, 6), xport_guest_ptr(actor + 184, 12)))) < ((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(geometry + 8, 12), xport_guest_ptr(actor_normal, 6), xport_guest_ptr(actor + 184, 12)))))
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
                int32_t first_projection = 0;
                int32_t second_projection = 0;

                if ((r_u16(surface + 2) & 15) == 0 || (r_u16(surface + 2) & 15) >= 9)
                    w_u16(surface + 2, 1026);
                if (special)
                {
                    if (!(r_u16(surface + 2) & 0x100))
                        goto reject_surface;
                    if (r_u32(special_actor + 768))
                    {
                        if ((flags & 0x10800) != 0x10000)
                            return race_collision_return(saved, 1);
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
                            w_u32(edge_a, (uint32_t)((int32_t)(r_u32(last_surface + 8) + r_u32(last_surface + 20)) / 2));
                            w_u32(edge_a + 4, (uint32_t)((int32_t)(r_u32(last_surface + 12) + r_u32(last_surface + 24)) / 2));
                            w_u32(edge_a + 8, (uint32_t)((int32_t)(r_u32(last_surface + 16) + r_u32(last_surface + 28)) / 2));
                            contact_distance = ((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(edge_a, 12), xport_guest_ptr(edge_cursor, 6), xport_guest_ptr(surface + 20, 12))));
                            current_dot = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(edge_cursor, 6), xport_guest_ptr(actor_normal, 6))));
                            next_dot = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(last_surface + 32, 6), xport_guest_ptr(actor_normal, 6))));
                            if (contact_distance <= 0x8000)
                            {
                                if (!(flags & 0x10000) && current_dot < -46333)
                                    flags |= 0x400;
                            }
                            else if (((scalar_sign_191 = ((int32_t)(current_dot)) >> 31), (((uint32_t)((uint32_t)(((uint32_t)scalar_sign_191 + (uint32_t) ((int32_t)(current_dot))) ^ scalar_sign_191))))) < ((scalar_sign_192 = ((int32_t)(next_dot)) >> 31), (((uint32_t)((uint32_t)(((uint32_t)scalar_sign_192 + (uint32_t) ((int32_t)(next_dot))) ^ scalar_sign_192))))))
                                flags |= 0x10400;
                            else
                                flags |= 0x10000;
                        }
                        if (!(flags & 0x400) && surface_index && (r_u16(surface - step + 2) & 0x100))
                        {
                            int32_t previous_dot = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(edge_cursor, 6), xport_guest_ptr(surface - step + 32, 6))));
                            int32_t actor_dot = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(edge_cursor, 6), xport_guest_ptr(actor_normal, 6))));
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
                w_u32(normal, r_u32(selected + 32));
                w_u16(normal + 4, r_u16(selected + 36));
                w_u32(edge_a, r_u32(selected + 8));
                w_u32(edge_a + 4, r_u32(selected + 12));
                w_u32(edge_a + 8, r_u32(selected + 16));
                w_u32(edge_b, r_u32(selected + 20));
                w_u32(edge_b + 4, r_u32(selected + 24));
                w_u32(edge_b + 8, r_u32(selected + 28));
                previous_flags = (uint16_t)material;
                surface_offset = ((int32_t)(uint32_t)(r_u32(selected + 4)));
                if (((scalar_value_193 = ((int16_t)r_u16(normal + 2))), (scalar_sign_193 = scalar_value_193 >> 31), (((uint32_t)((uint32_t)(((uint32_t)scalar_sign_193 + (uint32_t) scalar_value_193) ^ scalar_sign_193))))) >= 65)
                {
                    w_u16(normal + 2, 0);
                    (void)sub_8002E468(xport_guest_ptr(normal, 6));
                }
                flags = contact_flags & 0xFFFFFFE0u;

                if ((flags & 0x200) && (material & 15) == 5 && (mode || special))
                {
                    if ((previous_material & 15) != 5 && !special &&
                        (int32_t)r_u32(actor + 188) < (int32_t)(r_u32(r_u32(actor + 340) + 24) - (uint32_t)surface_offset + 0x10000u))
                    {
                        w_u16(normal, r_u16(r_u32(actor + 340) + 14));
                        w_u16(normal + 2, r_u16(r_u32(actor + 340) + 16));
                        w_u16(normal + 4, r_u16(r_u32(actor + 340) + 18));
                        flags |= 8;
                        ++segment_count;
                        if (((int32_t)(uint32_t)(r_u32(actor + 364))) > 0)
                            race_collision_negate_normal(normal);
                        if (direction < 0)
                        {
                            w_u32(edge_a, r_u32(selected + 20));
                            w_u32(edge_a + 4, r_u32(selected + 24));
                            w_u32(edge_a + 8, r_u32(selected + 28));
                        }
                        {
                            uint32_t basis = r_u32(actor + 340);
                            uint32_t collision = r_u32(actor + 372);
                            uint32_t scale = ((int32_t)(uint32_t)(r_u32(actor + 344))) >= 0 ? 0u - r_u32(collision + 144) : 0u - r_u32(collision + 16);
                            (void)sub_8002EAD8(xport_guest_ptr(edge_a, 12), xport_guest_ptr(basis + 2, 6), scale, xport_guest_ptr(edge_b, 12));
                        }
                    }
                    else if (special || r_s16(actor + 452) < 0)
                    {
                        flags |= 0x10;
                        w_u32(normal, 0x10000000);
                        w_u16(normal + 4, 0);
                        ++segment_count;
                        uint32_t basis = r_u32(actor + 340);
                        uint32_t collision = r_u32(actor + 372);
                        (void)sub_8002EAD8(xport_guest_ptr(basis + 20, 12), xport_guest_ptr(basis + 8, 6), (uint32_t)surface_offset, xport_guest_ptr(edge_a, 12));
                        (void)sub_8002EAD8(xport_guest_ptr(edge_a, 12), xport_guest_ptr(basis + 2, 6), r_u32(collision + 144), xport_guest_ptr(edge_b, 12));
                        (void)sub_8002EAD8(xport_guest_ptr(edge_a, 12), xport_guest_ptr(basis + 2, 6), r_u32(collision + 16), xport_guest_ptr(edge_a, 12));
                    }
                }
                if (!(flags & 0x1D8))
                    goto outer_next;

                {
                    uint32_t axis;
                    for (axis = 0; axis < 2; ++axis)
                    {
                        uint32_t endpoint = axis ? edge_b : edge_a;
                        uint32_t dx = r_u32(actor + 184) - r_u32(endpoint);
                        uint32_t dz = r_u32(actor + 192) - r_u32(endpoint + 8);
                        uint32_t sign_x = (uint32_t)((int32_t)dx >> 31);
                        uint32_t sign_z = (uint32_t)((int32_t)dz >> 31);
                        int32_t high = (int32_t)((dx + sign_x) ^ sign_x);
                        int32_t low = (int32_t)((dz + sign_z) ^ sign_z);
                        uint32_t combined;
                        int32_t estimate;
                        if (high < low)
                        {
                            int32_t swap = high;
                            high = low;
                            low = swap;
                        }
                        combined = (uint32_t)low + (uint32_t)(low >> 1);
                        estimate = (int32_t)((uint32_t)high - (uint32_t)(high >> 5) -
                            (uint32_t)(high >> 7) + (uint32_t)((int32_t)combined >> 2) +
                            (uint32_t)((int32_t)combined >> 6));
                        if (axis)
                            second_projection = estimate;
                        else
                            first_projection = estimate;
                    }
                    if (first_projection > 12517376 || second_projection > 12517376)
                        goto reject_surface;
                }

                if (material & 0x400)
                {
                    if ((r_u16(actor + 172) >> 5) == 1 && (material & 0xA00) == 0 && r_u32(actor + 604) == 4)
                        goto reject_surface;
                    flags |= sub_800B6AAC(xport_guest_ptr(actor + 468, 12), xport_guest_ptr(normal, 6), xport_guest_ptr(edge_a, 12)) >> 31;
                    if ((material & 15) == 1 && (flags & 1))
                        goto reject_surface;
                }

                motion_distance = ((int32_t)(uint32_t)((uint32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(0x800CCE38))), ((int32_t)(uint32_t)(r_u32(actor + 480))))));
                side_flags &= 0xFFFFFFF2u;
                if (!special && !(flags & 0x18) && !(material & 0x800))
                {
                    uint32_t game = r_u32(0x8005B2F8);
                    uint32_t actor_type = r_u16(actor + 172) >> 5;
                    int test = (actor_type < 2 && (uint32_t)(r_u16(actor + 172) & 31) < r_u32(game + 48));
                    if (!test)
                        test = (uint32_t)sub_800A8C78(actor, 1310720, 1835008) != 0;
                    if (test)
                    {
                        int32_t dot = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(actor_normal, 6), xport_guest_ptr(normal, 6))));
                        uint32_t absolute = ((scalar_sign_198 = ((int32_t)(dot)) >> 31), (((uint32_t)((uint32_t)(((uint32_t)scalar_sign_198 + (uint32_t) ((int32_t)(dot))) ^ scalar_sign_198)))));
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
                    else if ((previous_material & 15) == 0 || (int32_t)(((scalar_sign_199 = ((int32_t)(motion_distance)) >> 31), (((uint32_t)((uint32_t)(((uint32_t)scalar_sign_199 + (uint32_t) ((int32_t)(motion_distance))) ^ scalar_sign_199)))))) >= 131)
                    {
                        int32_t dot = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(actor_normal, 6), xport_guest_ptr(normal, 6))));
                        if (((scalar_sign_200 = ((int32_t)(dot)) >> 31), (((uint32_t)((uint32_t)(((uint32_t)scalar_sign_200 + (uint32_t) ((int32_t)(dot))) ^ scalar_sign_200))))) >= 11469)
                        {
                            flags |= 2;
                            if (!(flags & 0x20))
                            {
                                (void)sub_8002EAD8(xport_guest_ptr(actor + 184, 12), xport_guest_ptr(actor_normal, 6), (uint32_t)-motion_distance, xport_guest_ptr(probe_a, 12));
                                (void)sub_8002EAD8(xport_guest_ptr(actor + 184, 12), xport_guest_ptr(actor_normal, 6), (uint32_t)sub_8001FC90(46333, ((int32_t)(uint32_t)(r_u32(actor + 304) + r_u32(actor + 308)))), xport_guest_ptr(response, 12));
                                if (state == 1)
                                    state = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(actor_normal, 6), xport_guest_ptr(actor + 444, 6)))) >= 0;
                                flags |= 0x20;
                            }
                        }
                    }
                }

                if (flags & 4)
                {
                    uint32_t order = r_u32(0x800CCAA0 + 4 * side_flags);
                    uint32_t index;
                    int32_t travel_distance = motion_distance;
                    for (index = 0; index < 8; ++index)
                    {
                        int32_t candidate;
                        wheel = linked_mode ? ((order >> (4 * index)) & 7) : ((7 - index) & 7);
                        (void)sub_8002EAD8(xport_guest_ptr(actor + 196 + 12 * wheel, 12), xport_guest_ptr(actor_normal, 6), (uint32_t)-travel_distance, xport_guest_ptr(probe_a, 12));
                        candidate = ((int32_t)(uint32_t)((uint32_t)sub_800B6BD0(xport_guest_cptr(probe_a, 12), xport_guest_ptr(actor_normal, 6), xport_guest_ptr(normal, 6), xport_guest_cptr(edge_a, 12))));
                        if (candidate >= -4096 && candidate <= (int32_t)((uint32_t)travel_distance + 4096u))
                        {
                            (void)sub_8002EAD8(xport_guest_ptr(probe_a, 12), xport_guest_ptr(actor_normal, 6), (uint32_t)candidate, xport_guest_ptr(contact_point, 12));
                            contact_distance = -((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(actor + 196 + 12 * wheel, 12), xport_guest_ptr(normal, 6), xport_guest_ptr(edge_a, 12))));
                            if (contact_distance < 4096)
                                contact_distance = 4096;
                            if (linked_mode)
                            {
                                int32_t normal_dot = -((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(normal, 6), xport_guest_ptr(actor_normal, 6))));
                                if (normal_dot >= 132 && travel_distance >= 132)
                                {
                                    int32_t first = (int32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(0x800CCE38))), contact_distance);
                                    int32_t second = (int32_t)sub_8001FC90(normal_dot, travel_distance);
                                    uint32_t numerator = first > 0 ? (uint32_t)first : 0u - (uint32_t)first;
                                    uint32_t denominator = second > 0 ? (uint32_t)second : 0u - (uint32_t)second;
                                    uint32_t ratio = sub_80010028(numerator, denominator);
                                    if ((first > 0) != (second > 0))
                                        ratio = 0u - ratio;
                                    surface_time = (uint32_t)ratio;
                                }
                                if (((int32_t)(uint32_t)(surface_time)) > ((int32_t)(uint32_t)(r_u32(0x800CCE38))))
                                    surface_time = r_u32(0x800CCE38);
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
                        int32_t first = ((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(probe_a, 12), xport_guest_ptr(normal, 6), xport_guest_ptr(edge_a, 12))));
                        wheel = 8;
                        if ((first ^ ((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(response, 12), xport_guest_ptr(normal, 6), xport_guest_ptr(edge_a, 12))))) < 0)
                        {
                            uint32_t index;
                            int32_t chosen = (int32_t)(((0u - (flags & 1)) & 0x80020000u) + 0x3FFF0000u);
                            for (index = 0; index < 8; ++index)
                            {
                                int select = state == 2 || ((index >> 1) & 1) == state;
                                if (select)
                                {
                                    int32_t candidate = ((int32_t)(uint32_t)((uint32_t)sub_800B6BD0(xport_guest_cptr(actor + 196 + 12 * index, 12), xport_guest_ptr(actor_normal, 6), xport_guest_ptr(normal, 6), xport_guest_cptr(edge_a, 12))));
                                    if (((flags & 1) && chosen < candidate) || candidate < chosen)
                                    {
                                        wheel = index;
                                        chosen = candidate;
                                    }
                                }
                            }
                            contact_distance = chosen;
                            if (wheel < 8)
                                (void)sub_8002EAD8(xport_guest_ptr(actor + 196 + 12 * wheel, 12), xport_guest_ptr(actor_normal, 6), (uint32_t)chosen, xport_guest_ptr(contact_point, 12));
                        }
                    }
                    else
                    {
                        int32_t threshold = special ? 0x80000 : (int32_t)(3u * (r_u32(actor + 308) + r_u32(actor + 304) + (uint32_t)((int32_t)r_u32(actor + 312) / 2))) / 4;
                        if (flags & 1)
                            race_collision_negate_normal(normal);
                        {
                            uint32_t contact_depth, contact_count;
                            wheel = sub_800B6F40(actor + 196, xport_guest_cptr(normal, 6), edge_a, &contact_depth, &contact_count);
                            w_u32(scratch + 124, contact_count);
                            if (wheel != 8)
                                w_u32(scratch + 120, contact_depth);
                        }
                        if (r_u32(scratch + 124) == 8)
                        {
                            uint32_t identity = r_u16(actor + 172);
                            uint32_t player_count = r_u32(r_u32(0x8005B2F8) + 48);
                            int apply_threshold = !(material & 0x800);
                            if (!apply_threshold && identity < player_count)
                                apply_threshold = (r_u32(r_u32(actor + 852) + 552) & 0x80000) != 0;
                            if (!apply_threshold && (identity >> 5) == 1 &&
                                (int32_t)(identity & 31) < (int32_t)player_count)
                                apply_threshold = (r_u32(actor + 552) & 0x80000) != 0;
                            if (apply_threshold)
                            {
                                if (threshold < (int32_t)r_u32(scratch + 120))
                                    wheel = 8;
                                if ((material & 15) == 2 && group_count >= 2)
                                    wheel = 8;
                            }
                        }
                        contact_distance = ((int32_t)(uint32_t)(r_u32(scratch + 120)));
                        if (wheel < 8)
                            (void)sub_8002EAD8(xport_guest_ptr(actor + 196 + 12 * wheel, 12), xport_guest_ptr(normal, 6), (uint32_t)contact_distance, xport_guest_ptr(contact_point, 12));
                    }
                }

                w_u32(actor + 388, ((uint32_t)material << 20) | (r_u32(actor + 388) & 0x000FFFEFu));
                if (flags & 1)
                {
                    int32_t dx = ((int32_t)(uint32_t)(r_u32(edge_a) - r_u32(actor + 184)));
                    int32_t dz = ((int32_t)(uint32_t)(r_u32(edge_a + 8) - r_u32(actor + 192)));
                    int32_t distance = (int32_t)((uint32_t)(uint32_t)sub_8001FC90(dx, dx) + (uint32_t)sub_8001FC90(dz, dz));
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
                        tangent_x = (int32_t)r_s16(selected + 32) * 16;
                        tangent_z = (int32_t)r_s16(selected + 36) * 16;
                    }
                    else
                    {
                        tangent_x = (int32_t)r_s16(normal + 4) * 16;
                        tangent_z = -(int32_t)r_s16(normal) * 16;
                        if (((scalar_value_201 = ((int16_t)r_u16(normal + 2))), (scalar_sign_201 = scalar_value_201 >> 31), (((uint32_t)((uint32_t)(((uint32_t)scalar_sign_201 + (uint32_t) scalar_value_201) ^ scalar_sign_201))))) >= 65)
                        {
                            uint32_t length = (uint32_t)sub_8001FC90(tangent_x, tangent_x);
                            uint32_t first_root, second_root, third_root, denominator, reciprocal;
                            int negative;
                            int32_t scale;
                            length += (uint32_t)sub_8001FC90(tangent_z, tangent_z);
                            first_root = sub_8004CF74(length);
                            negative = (int32_t)(first_root << 2) < 0;
                            second_root = sub_8004CF74(length) << 2;
                            third_root = sub_8004CF74(length) << 2;
                            if (negative)
                            {
                                second_root = 0u - second_root;
                                third_root = 0u - third_root;
                            }
                            denominator = (uint32_t)((int32_t)second_root >> 1) +
                                (uint32_t)((int32_t)(third_root - 2u) >> 31);
                            reciprocal = denominator ? 0x80000000u / denominator : 0xffffffffu;
                            scale = (int32_t)(negative ? 0u - reciprocal : reciprocal);
                            tangent_x = (int32_t)sub_8001FC90(tangent_x, scale);
                            tangent_z = (int32_t)sub_8001FC90(tangent_z, scale);
                        }
                    }
                    first_dot = (int32_t)((uint32_t)(uint32_t)sub_8001FC90(tangent_x, ((int32_t)(uint32_t)(r_u32(contact_point) - r_u32(edge_a)))) + (uint32_t)sub_8001FC90(tangent_z, ((int32_t)(uint32_t)(r_u32(contact_point + 8) - r_u32(edge_a + 8)))));
                    second_dot = (int32_t)((uint32_t)(uint32_t)sub_8001FC90(tangent_x, ((int32_t)(uint32_t)(r_u32(edge_b) - r_u32(contact_point)))) + (uint32_t)sub_8001FC90(tangent_z, ((int32_t)(uint32_t)(r_u32(edge_b + 8) - r_u32(contact_point + 8)))));
                    tangent_ratio = first_dot;
                    if (material & 0x400)
                        front_limit = (int32_t)((uint32_t)(r_s16(actor + 482) / 16) * r_u32(actor + 304) + 2048u);
                    if (!(material & 0x400) && special && (flags & 0x10000))
                    {
                        side_limit = ((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(edge_b, 12), xport_guest_ptr((flags & 0x400) ? edge_cursor : last_surface + 32, 6), xport_guest_ptr((flags & 0x400) ? surface + 8 : last_surface + 8, 12))));
                        if (((scalar_sign_202 = ((int32_t)(side_limit)) >> 31), (((uint32_t)((uint32_t)(((uint32_t)scalar_sign_202 + (uint32_t) ((int32_t)(side_limit))) ^ scalar_sign_202))))) <= 0x8000)
                            side_limit = (int32_t)((uint32_t)((int32_t)(3u * r_u32(actor + 308)) / 2) + 2048u + 2u * r_u32(actor + 304));
                        else
                        {
                            side_limit = 2048;
                            front_limit = (int32_t)((uint32_t)((int32_t)(3u * r_u32(actor + 308)) / 2) + 2u * r_u32(actor + 304) + 2048u);
                        }
                    }
                    if (!(flags & 0x10000))
                        side_limit = front_limit;
                    if (front_limit < (int32_t)((scalar_sign_203 = ((int32_t)(first_dot)) >> 31), (((uint32_t)((uint32_t)(((uint32_t)scalar_sign_203 + (uint32_t) ((int32_t)(first_dot))) ^ scalar_sign_203))))) && side_limit < (int32_t)((scalar_sign_204 = ((int32_t)(second_dot)) >> 31), (((uint32_t)((uint32_t)(((uint32_t)scalar_sign_204 + (uint32_t) ((int32_t)(second_dot))) ^ scalar_sign_204))))) && (first_dot ^ second_dot) < 0)
                        goto reject_surface;

                    if ((material & 15) == 1 || (mode && (material & 0x400)))
                    {
                        int32_t height = ((int32_t)(uint32_t)(r_u32(edge_a + 4)));
                        if ((r_u16(actor + 172) >> 5) < 3)
                        {
                            height = (int32_t)r_u32(actor + 508);
                            w_u32(edge_a + 4, (uint32_t)height);
                        }
                        if ((material & 15) == 1 && r_u16(selected + 38))
                        {
                            int32_t a = (int32_t)((scalar_sign_205 = ((int32_t)(first_dot)) >> 31), (((uint32_t)((uint32_t)(((uint32_t)scalar_sign_205 + (uint32_t) ((int32_t)(first_dot))) ^ scalar_sign_205)))));
                            int32_t b = (int32_t)((scalar_sign_206 = ((int32_t)(second_dot)) >> 31), (((uint32_t)((uint32_t)(((uint32_t)scalar_sign_206 + (uint32_t) ((int32_t)(second_dot))) ^ scalar_sign_206)))));
                            int32_t total = (int32_t)((uint32_t)a + (uint32_t)b);
                            uint32_t ratio = sub_80010028(a > 0 ? (uint32_t)a : 0u - (uint32_t)a,
                                total > 0 ? (uint32_t)total : 0u - (uint32_t)total);
                            if ((a > 0) != (total > 0))
                                ratio = 0u - ratio;
                            tangent_ratio = (int32_t)ratio;
                            surface_offset = r_u16(selected + 38) == 1 ? (int32_t)(0x10000u - (uint32_t)tangent_ratio) : tangent_ratio;
                        }
                        for (index = 0; index < 8; ++index)
                            if ((int32_t)((uint32_t)height - (uint32_t)surface_offset) < ((int32_t)(uint32_t)(r_u32(actor + 200 + 12 * index))))
                                break;
                        if (index >= 8)
                        {
                            int reject = 1;
                            uint32_t identity = r_u16(actor + 172);
                            uint32_t player_count = r_u32(r_u32(0x8005B2F8) + 48);
                            if ((identity >> 5) < 2 && (identity & 31) < player_count && (material & 0x200))
                            {
                                uint32_t contact_count;
                                uint32_t contact_vertex = sub_800B6F40(actor + 196, xport_guest_cptr(normal, 6), edge_a, NULL, &contact_count);
                                w_u32(scratch + 124, contact_count);
                                if (contact_vertex != 8)
                                {
                                    int32_t dot = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(actor_normal, 6), xport_guest_ptr(normal, 6))));
                                    if (((scalar_sign_207 = ((int32_t)(dot)) >> 31), (((uint32_t)((uint32_t)(((uint32_t)scalar_sign_207 + (uint32_t) ((int32_t)(dot))) ^ scalar_sign_207))))) >= 22413)
                                    {
                                        if (r_u32(scratch + 124) == 8)
                                        {
                                            uint32_t owner = linked_mode ? r_u32(actor + 852) : actor;
                                            uint32_t owner_identity = linked_mode ? r_u16(actor + 172) : r_u16(r_u32(actor + 596) + 172);
                                            uint32_t record = 0x800CD898 + 1132 * owner_identity;
                                            w_u32(record + 788, 0xFFFF0000);
                                            w_u32(record + 548, r_u32(record + 548) | 0x40000);
                                            w_u32(owner + 552, r_u32(owner + 552) | 0x80000);
                                        }
                                    }
                                    else
                                    {
                                        reject = 0;
                                        if (dot > 0)
                                            race_collision_negate_normal(normal);
                                        flags &= ~1u;
                                    }
                                }
                            }
                            if (reject)
                                goto reject_surface;
                        }
                    }

                    if (flags & 2)
                    {
                        tangent_ratio = (int32_t)(((0u - (flags & 1)) & 0xFFFFC000u) + 0x2000u);
                        (void)sub_8002ECB8(xport_guest_ptr(actor_normal, 6), xport_guest_ptr(normal, 6), xport_guest_ptr(response, 12), ((uint32_t)contact_distance - (uint32_t)motion_distance - 4096u), (uint32_t)tangent_ratio);
                        if (flags & 1)
                            race_collision_negate_normal(normal);
                    }
                    else
                    {
                        if ((flags & 5) == 5)
                            race_collision_negate_normal(normal);
                        if ((r_u16(actor + 172) >> 5) != 1 && !(flags & 4))
                            contact_distance = (int32_t)((uint32_t)contact_distance + 0x2000u);
                        if ((material & 0x400) && !(flags & 1))
                            contact_distance = (int32_t)((uint32_t)contact_distance + ((uint32_t)(r_s16(actor + 482) / 16) << 13));
                        (void)sub_8002EE50((uint32_t)contact_distance, xport_guest_ptr(normal, 6), xport_guest_ptr(response, 12));
                    }

                    if (linked_mode)
                    {
                        uint32_t owner = tracked;
                        uint32_t maximum;
                        if (!r_u32(tracked + 1088))
                        {
                            tracked = r_u32(tracked + 856);
                            owner = tracked;
                        }
                        maximum = surface_time < r_u32(owner + 552) ? r_u32(owner + 552) : surface_time;
                        w_u32(owner + 552, maximum);
                        if ((r_u32(actor + 320) & 4) && r_u32(0x800CCF68) < 8)
                        {
                            uint32_t count = r_u32(0x800CCF68);
                            uint32_t record = 0x800CCE48 + 36 * count;
                            w_u16(record, r_u16(owner + 172));
                            w_u32(record + 12, selected);
                            w_u16(record + 2, r_u16(selected));
                            w_u16(record + 4, r_u16(normal));
                            w_u16(record + 6, r_u16(normal + 2));
                            w_u16(record + 10, (uint16_t)material);
                            w_u16(record + 8, r_u16(normal + 4));
                            w_u32(record + 16, r_u32(response));
                            w_u32(record + 20, r_u32(response + 4));
                            w_u32(record + 28, (uint32_t)surface_offset);
                            w_u32(record + 32, (uint32_t)contact_distance);
                            w_u32(record + 24, r_u32(response + 8));
                            if (linked_mode < 2)
                                w_u32(0x800CCF68, count + 1);
                            else
                                w_u32(0x800CCF68, count + sub_800B59F0(record));
                        }
                        else
                        {
                            uint16_t contact_normal[3];
                            uint32_t contact_translation[3], axis;
                            for (axis = 0; axis < 3; ++axis)
                            {
                                contact_normal[axis] = r_u16(normal + 2 * axis);
                                contact_translation[axis] = r_u32(response + 4 * axis);
                            }
                            (void)sub_800B12A0(tracked, contact_normal, material, (int32_t)surface_offset, selected, contact_translation);
                            for (axis = 0; axis < 3; ++axis)
                                w_u32(response + 4 * axis, contact_translation[axis]);
                        }
                    }
                    else if (special)
                    {
                        if (flags & 0x20000)
                        {
                            int32_t chosen = tangent_ratio;
                            int32_t cross = (int32_t)r_s16(surface - step + 32) * (int16_t)(tangent_x >> 4) / 4 + (int32_t)r_s16(surface - step + 36) * (int16_t)(tangent_z >> 4) / 4;
                            int32_t segment = (int32_t)((uint32_t)(uint32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(edge_b) - r_u32(edge_a))), tangent_x) + (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(edge_b + 8) - r_u32(edge_a + 8))), tangent_z));
                            int32_t impulse;
                            if ((cross ^ segment) >= 0)
                                chosen = second_dot;
                            impulse = (int32_t)((uint32_t)((scalar_sign_208 = ((int32_t)(chosen)) >> 31), (((uint32_t)((uint32_t)(((uint32_t)scalar_sign_208 + (uint32_t) ((int32_t)(chosen))) ^ scalar_sign_208))))) + 0x4000u);
                            if (cross < 0)
                                impulse = (int32_t)(0u - (uint32_t)impulse);
                            w_u32(response, r_u32(response) + (uint32_t)sub_8001FC90(tangent_x, impulse));
                            w_u32(response + 8, r_u32(response + 8) + (uint32_t)sub_8001FC90(tangent_z, impulse));
                        }
                        (void)(uint32_t)sub_800A8DF0(xport_guest_ptr(actor, 1), xport_guest_ptr(response, 12), 0);
                        if (!r_u32(special_actor + 768))
                            w_u32(special_actor + 768, selected);
                    }
                    else
                    {
                        uint32_t identity = r_u16(actor + 172);
                        uint32_t actor_type = identity >> 5;
                        (void)(uint32_t)sub_800A8DF0(xport_guest_ptr(actor, 1), xport_guest_ptr(response, 12), 1);
                        if (actor_type == 2)
                        {
                            uint32_t object = r_u32(0x800D4B80) + 572 * (identity & 31);
                            (void)sub_800A9868(object, xport_guest_cptr(normal, 6), 0, xport_guest_cptr(normal, 6), object + 172, 0);
                        }
                        else if (actor_type == 4)
                            (void)(uint32_t)sub_800B3344(r_u32(0x800CD6D4) + 596 * (identity & 31), 0, xport_guest_ptr(normal, 6), 0xFFFFFFFF);
                        else if (actor_type == 1)
                            (void)sub_800B2794(r_u32(0x8005B3A4) + 628u * (identity & 31u), normal);
                    }
                    result = wheel | 0x100;
                }

                if (!special)
                {
                    w_u32(actor + 388, r_u32(actor + 388) | (16 * (flags & 1)));
                    return race_collision_return(saved, result);
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
        w_u32(actor + 388, r_u32(actor + 388) | 0x10);
        uint32_t basis = r_u32(actor + 340);
        first = ((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(best_surface + 8, 12), xport_guest_ptr(basis + 2, 6), xport_guest_ptr(basis + 20, 12))));
        second = ((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(best_surface + 20, 12), xport_guest_ptr(basis + 2, 6), xport_guest_ptr(basis + 20, 12))));
        first = (int32_t)(((uint32_t)first + (uint32_t)(first >> 31)) ^ (uint32_t)(first >> 31));
        second = (int32_t)(((uint32_t)second + (uint32_t)(second >> 31)) ^ (uint32_t)(second >> 31));
        best_distance = second < first ? first : second;
        w_u32(actor + 388, ((uint32_t)((int16_t)(best_distance >> 13)) << 8) | (r_u32(actor + 388) & 0xFFF000FFu));
    }
    return race_collision_return(saved, result);
}

uint32_t sub_800A8FE8(uint32_t actor)
{
    const uint32_t vector = 0x1F8000E0;
    uint8_t saved[16];
    uint32_t flags = r_u32(actor + 568);
    uint32_t linked;
    int32_t length;
    int32_t root;
    int32_t delta;
    int32_t reciprocal;
    uint32_t axis;

    FUNCTION_MARKER(0x800A8FE8, "RASHCDG.BIN");
    if (!(flags & 0x01000000))
        return 0;
    memcpy(saved, xport_guest_ptr(vector, sizeof(saved)), sizeof(saved));
    w_u32(actor + 568, flags & 0xFEFFFFFFu);
    for (axis = 0; axis < 3; ++axis)
        w_u32(vector + 4 * axis, r_u32(actor + 184 + 4 * axis) - r_u32(actor + 468 + 4 * axis));
    length = ((int32_t)(uint32_t)(sub_8002F0F4(xport_guest_ptr(vector, 12))));
    if (length < 132)
    {
        xport_guest_copy(xport_guest_ref(vector), xport_host_ref((void *)saved), sizeof(saved));
        return 0;
    }
    flags = r_u32(actor + 568);
    if (!(flags & 0x02000000))
    {
        w_u16(actor + 864, r_u16(actor + 450));
        w_u16(actor + 866, r_u16(actor + 452));
        w_u16(actor + 868, r_u16(actor + 454));
        w_u32(actor + 860, r_u32(actor + 480));
        w_u32(actor + 568, flags | 0x02000000);
    }
    root = ((int32_t)(uint32_t)(sub_8004CF74((uint32_t)length) << 2));
    delta = ((int32_t)(uint32_t)(r_u32(0x800CCE38)));
    w_u32(actor + 480, (uint32_t)race_leaf_signed_ratio(root, delta));
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
    (void)sub_8002EED8((uint32_t)reciprocal, xport_guest_ptr(vector, 12), xport_guest_ptr(actor + 450, 6));
    linked = r_u32(actor + 856);
    xport_guest_copy(xport_guest_ref(vector), xport_host_ref((void *)saved), sizeof(saved));
    if (linked && r_u32(actor + 1088))
    {
        w_u32(actor + 568, r_u32(actor + 568) | 0x01000000);
        (void)sub_800A8FE8(linked);
    }
    return 1;
}

uint32_t sub_800A8C78(uint32_t actor, int32_t lateral_limit, int32_t distance_limit)
{
    int32_t scalar_sign_213;

    uint16_t state = r_u16(actor + 320);
    uint32_t player_count;
    uint32_t index;
    uint32_t result = 0;

    FUNCTION_MARKER(0x800A8C78, "RASHCDG.BIN");
    if (state & 8)
        return (state >> 2) & 1;
    w_u32(actor + 48, 0x7FFFFFFF);
    player_count = r_u32(r_u32(0x8005B2F8) + 48);
    for (index = 0; index < player_count; ++index)
    {
        uint32_t origin = 0x800CD950 + 1132 * index;
        int32_t distance = ((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(actor + 184, 12), xport_guest_ptr(0x800CDA54 + 1132 * index, 6), xport_guest_ptr(origin, 12))));
        if (distance > 0x8000 && distance < distance_limit)
        {
            distance = ((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(actor + 184, 12), xport_guest_ptr(0x800CDA48 + 1132 * index, 6), xport_guest_ptr(origin, 12))));
            if ((int32_t)((scalar_sign_213 = ((int32_t)(distance)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_213 + ((int32_t)(distance))) ^ scalar_sign_213))))) < lateral_limit)
                result = 1;
        }
        w_u32(actor + 44 + 4 * index, (uint32_t)(distance >> 10));
    }
    w_u16(actor + 320, (uint16_t)(state | 8 | (4 * result)));
    return result;
}

uint32_t sub_800B6F40(uint32_t points, const uint16_t normal[3], uint32_t origin, uint32_t *distance_out, uint32_t *count_out)
{
    uint32_t selected = 8;
    uint32_t negative_count = 0;
    uint32_t index;
    int32_t minimum = 0;

    FUNCTION_MARKER(0x800B6F40, "RASHCDG.BIN");
    for (index = 0; index < 8; ++index)
    {
        int32_t distance = ((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(points + 12 * index, 12), normal, xport_guest_ptr(origin, 12))));
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
        *count_out = negative_count;
    if (selected != 8 && distance_out)
        *distance_out = 0 - (uint32_t)minimum;
    return selected;
}

void sub_80079B20(uint32_t list, int32_t delta)
{
    FUNCTION_MARKER(0x80079B20u, "RASHCDG.BIN");
    uint32_t node = r_u32(list + 4u);
    while (node != list)
    {
        uint32_t actor = node - 1088u;
        int32_t gear = r_s8(actor + 849u);
        uint32_t specification = r_u32(actor + 556u);
        uint32_t motion = r_u32(actor + 620u);
        uint32_t ratio = r_u32(specification + 20u + 4u * (uint32_t)gear);
        uint32_t flags, state, value, other, sign, numerator, denominator, quotient;
        uint32_t first, second, gap, increment, upper, lower_mask, upper_mask;
        uint32_t offset, interval, curve_index, fraction, address, curve_value;
        uint32_t rpm, power, candidate_rpm, candidate_power, candidate_ratio;
        uint32_t acceleration, time, velocity, displacement, endpoint, keep;
        uint32_t x, z, bound, table_index, shift, bucket, target, difference;
        int64_t product;
        int has_motion = motion != 0;
        int reconsider_gear = 0;
        int crossing = 0;
        if (!has_motion)
            has_motion = r_u32(actor + 624u) != 0;
        if (r_s32(actor + 576u) >= 13107 || (r_u32(actor + 560u) & 0x42u) != 0x42u || (r_u32(actor + 568u) & 0x7FFu) || r_u32(r_u32(actor + 852u) + 604u) >= 2u || (r_u32(actor + 564u) & 0x40000u) || has_motion || (r_u32(actor + 560u) & 0x08000000u))
        {
            state = r_u32(actor + 564u);
            if (state & 0x08000000u)
            {
                state &= 0xF7FFFFFFu;
                w_u32(actor + 564u, state);
                if (!has_motion || r_s32(actor + 616u) >= 0)
                {
                    if (r_u32(actor + 560u) & 2u)
                        w_u32(actor + 564u, state | 0x10000000u);
                    w_u32(actor + 700u, 0u);
                    w_u32(actor + 616u, 0u);
                }
            }
            else
                w_u32(actor + 564u, state & 0xF7FFFFFFu);
        }
        else
            (void)sub_80075BE4(actor, delta);

        if (!(r_u32(actor + 564u) & 0x08000000u))
        {
            other = r_u32(specification + 180u);
            value = r_u32(actor + 604u);
            flags = r_u32(actor + 560u);
            if ((int32_t)value >= (int32_t)other)
                flags |= 0x00800000u;
            w_u32(actor + 560u, flags);
            if (flags & 0x00800000u)
            {
                int wheels = (r_u32(actor + 568u) & 0x600u) != 0;
                sign = (flags & 2u) ? 0xFFFFFFFFu : 0u;
                value = (flags & 0x40u) ? 0xD8F00000u : 0xFF9C0000u;
                other = (flags & 0x40u) ? 0x4E200000u : 0x27740000u;
                value = ((0u - (uint32_t)wheels) & (value + (sign & other) + 655360000u)) - 655360000u;
                increment = (uint32_t)sub_8001FC90((int32_t)value, delta);
                value = r_u32(actor + 604u) + increment;
                w_u32(actor + 604u, value);
                if (wheels)
                {
                    first = r_u32(specification + 176u);
                    second = r_u32(specification + 180u);
                    lower_mask = (uint32_t)((int32_t)(value - first) >> 31);
                    upper = second - value;
                    upper_mask = (uint32_t)((int32_t)upper >> 31);
                    w_u32(actor + 604u, value + (lower_mask & (first - value)) + (upper_mask & upper));
                }
                else
                {
                    other = r_u32(specification + 180u);
                    if ((int32_t)value < (int32_t)other)
                    {
                        reconsider_gear = 1;
                        flags = r_u32(actor + 560u);
                        w_u32(actor + 604u, other - 6553u);
                        w_u32(actor + 560u, flags & 0xFF7FFFFFu);
                    }
                }
                numerator = r_u32(actor + 604u);
                denominator = ratio;
                quotient = sub_80010028((int32_t)numerator > 0 ? numerator : 0u - numerator, (int32_t)denominator > 0 ? denominator : 0u - denominator);
                if (((int32_t)numerator > 0) != ((int32_t)denominator > 0))
                    quotient = 0u - quotient;
                w_u32(actor + 576u, quotient);
                goto evaluate_power;
            }
            if (!(flags & 2u))
                w_u32(actor + 564u, r_u32(actor + 564u) & 0xEFFFFFFFu);
            else if (r_s32(actor + 576u) < 13107)
            {
                state = r_u32(actor + 564u);
                if (state & 0x10000000u)
                    w_u32(actor + 564u, state | 1u);
                else
                {
                    value = r_u32(actor + 764u);
                    if ((int32_t)value < 0)
                    {
                        product = (int64_t)(int32_t)(0u - value) * 72089;
                        numerator = (uint32_t)((uint64_t)product >> 16);
                        quotient = sub_80010028((int32_t)numerator > 0 ? numerator : 0u - numerator, (int32_t)ratio > 0 ? ratio : 0u - ratio);
                        if (((int32_t)numerator > 0) != ((int32_t)ratio > 0))
                            quotient = 0u - quotient;
                        curve_index = 0;
                        rpm = r_u32(specification + 176u);
                        address = specification;
                        while ((int32_t)quotient >= r_s32(address + 60u))
                        {
                            increment = r_u32(specification + 184u);
                            ++curve_index;
                            rpm += increment;
                            address += 4u;
                            if (curve_index >= 29u)
                                break;
                        }
                        if (curve_index >= 29u)
                            w_u32(actor + 604u, 0u);
                        else if (!curve_index)
                            w_u32(actor + 604u, rpm);
                        else
                        {
                            second = r_u32(specification + 60u + 4u * curve_index);
                            numerator = second - quotient;
                            first = r_u32(specification + 60u + 4u * (curve_index - 1u));
                            denominator = second - first;
                            quotient = sub_80010028((int32_t)numerator > 0 ? numerator : 0u - numerator, (int32_t)denominator > 0 ? denominator : 0u - denominator);
                            if (((int32_t)numerator > 0) != ((int32_t)denominator > 0))
                                quotient = 0u - quotient;
                            interval = r_u32(specification + 184u);
                            product = (int64_t)(int32_t)quotient * (int32_t)interval;
                            w_u32(actor + 604u, rpm - (uint32_t)((uint64_t)product >> 16));
                        }
                        w_u32(actor + 564u, r_u32(actor + 564u) | 1u);
                    }
                }
            }
            if (!(r_u32(actor + 564u) & 1u))
            {
                value = r_u32(actor + 576u);
                product = (int64_t)(int32_t)value * (int32_t)ratio;
                w_u32(actor + 604u, (uint32_t)((uint64_t)product >> 16));
                reconsider_gear = 1;
            }
            else
            {
                numerator = r_u32(actor + 604u);
                velocity = r_u32(actor + 576u);
                quotient = sub_80010028((int32_t)numerator > 0 ? numerator : 0u - numerator, (int32_t)ratio > 0 ? ratio : 0u - ratio);
                if (((int32_t)numerator > 0) != ((int32_t)ratio > 0))
                    quotient = 0u - quotient;
                if ((int32_t)velocity >= (int32_t)quotient || !(r_u32(actor + 560u) & 2u))
                {
                    w_u32(actor + 564u, r_u32(actor + 564u) & 0xEFFFFFFEu);
                    reconsider_gear = 1;
                }
                else
                    reconsider_gear = 1;
            }
        }
    evaluate_power:
        rpm = r_u32(actor + 604u);
        curve_value = 0;
        if ((int32_t)rpm < r_s32(specification + 180u))
        {
            offset = rpm - r_u32(specification + 176u);
            if ((int32_t)offset < 6553)
                curve_value = r_u32(specification + 60u);
            else
            {
                interval = r_u32(specification + 184u);
                curve_index = interval ? (uint32_t)((int32_t)offset / (int32_t)interval) : 0xFFFFFFFFu;
                fraction = sub_80010028(offset, (int32_t)interval > 0 ? interval : 0u - interval);
                if ((int32_t)interval <= 0)
                    fraction = 0u - fraction;
                fraction -= curve_index << 16;
                address = specification + 60u + 4u * curve_index;
                if ((int32_t)fraction < 65)
                    curve_value = r_u32(address);
                else
                {
                    second = r_u32(address + 4u);
                    first = r_u32(address);
                    gap = second - first;
                    if ((int32_t)gap < 6)
                        curve_value = second;
                    else
                    {
                        increment = (uint32_t)sub_8001FC90((int32_t)fraction, (int32_t)(gap << 10));
                        curve_value = r_u32(address) + (uint32_t)((int32_t)increment / 1024);
                    }
                }
            }
        }
        product = (int64_t)(int32_t)ratio * (int32_t)curve_value;
        power = (uint32_t)((uint64_t)product >> 16);
        if (reconsider_gear)
        {
            gear = r_s8(actor + 849u);
            if (gear > 0)
            {
                candidate_ratio = r_u32(specification + 20u + 4u * (uint32_t)(gear - 1));
                value = r_u32(actor + 576u);
                candidate_rpm = (uint32_t)sub_8001FC90((int32_t)value, (int32_t)candidate_ratio);
                curve_value = 0;
                if ((int32_t)candidate_rpm < r_s32(specification + 180u))
                {
                    offset = candidate_rpm - r_u32(specification + 176u);
                    if ((int32_t)offset < 6553)
                        curve_value = r_u32(specification + 60u);
                    else
                    {
                        interval = r_u32(specification + 184u);
                        curve_index = interval ? (uint32_t)((int32_t)offset / (int32_t)interval) : 0xFFFFFFFFu;
                        fraction = sub_80010028(offset, (int32_t)interval > 0 ? interval : 0u - interval);
                        if ((int32_t)interval <= 0)
                            fraction = 0u - fraction;
                        fraction -= curve_index << 16;
                        address = specification + 60u + 4u * curve_index;
                        if ((int32_t)fraction < 65)
                            curve_value = r_u32(address);
                        else
                        {
                            second = r_u32(address + 4u);
                            first = r_u32(address);
                            gap = second - first;
                            if ((int32_t)gap < 6)
                                curve_value = second;
                            else
                            {
                                increment = (uint32_t)sub_8001FC90((int32_t)fraction, (int32_t)(gap << 10));
                                curve_value = r_u32(address) + (uint32_t)((int32_t)increment / 1024);
                            }
                        }
                    }
                }
                candidate_power = (uint32_t)sub_8001FC90((int32_t)candidate_ratio, (int32_t)curve_value);
                if ((int32_t)power < (int32_t)candidate_power)
                {
                    gear = r_u8(actor + 849u);
                    power = candidate_power;
                    w_u32(actor + 604u, candidate_rpm);
                    w_u8(actor + 849u, (uint32_t)gear - 1u);
                }
            }
            other = r_u8(specification + 444u);
            gear = r_s8(actor + 849u);
            if (gear < (int32_t)other - 1)
            {
                candidate_ratio = r_u32(specification + 20u + 4u * (uint32_t)(gear + 1));
                value = r_u32(actor + 576u);
                product = (int64_t)(int32_t)value * (int32_t)candidate_ratio;
                candidate_rpm = (uint32_t)((uint64_t)product >> 16);
                curve_value = 0;
                if ((int32_t)candidate_rpm < r_s32(specification + 180u))
                {
                    offset = candidate_rpm - r_u32(specification + 176u);
                    if ((int32_t)offset < 6553)
                        curve_value = r_u32(specification + 60u);
                    else
                    {
                        interval = r_u32(specification + 184u);
                        curve_index = interval ? (uint32_t)((int32_t)offset / (int32_t)interval) : 0xFFFFFFFFu;
                        fraction = sub_80010028(offset, (int32_t)interval > 0 ? interval : 0u - interval);
                        if ((int32_t)interval <= 0)
                            fraction = 0u - fraction;
                        fraction -= curve_index << 16;
                        address = specification + 60u + 4u * curve_index;
                        if ((int32_t)fraction < 65)
                            curve_value = r_u32(address);
                        else
                        {
                            second = r_u32(address + 4u);
                            first = r_u32(address);
                            gap = second - first;
                            if ((int32_t)gap < 6)
                                curve_value = second;
                            else
                            {
                                increment = (uint32_t)sub_8001FC90((int32_t)fraction, (int32_t)(gap << 10));
                                curve_value = r_u32(address) + (uint32_t)((int32_t)increment / 1024);
                            }
                        }
                    }
                }
                product = (int64_t)(int32_t)candidate_ratio * (int32_t)curve_value;
                candidate_power = (uint32_t)((uint64_t)product >> 16);
                if ((int32_t)power < (int32_t)candidate_power && (r_u32(actor + 560u) & 2u))
                {
                    gear = r_u8(actor + 849u);
                    power = candidate_power;
                    w_u32(actor + 604u, candidate_rpm);
                    w_u8(actor + 849u, (uint32_t)gear + 1u);
                }
            }
        }
        flags = r_u32(actor + 564u);
        w_u32(actor + 592u, power);
        if (flags & 0x200u)
            w_u32(actor + 592u, (uint32_t)sub_8001FC90(r_s32(specification + 336u), (int32_t)power));
        if (r_u32(actor + 568u) & 0x600u)
            goto next_actor;
        flags = r_u32(actor + 564u);
        if (flags & 0x200000u)
        {
            state = r_u32(actor + 564u);
            w_u32(actor + 620u, 0u);
            w_u32(actor + 624u, 0u);
            w_u32(actor + 564u, state & 0xFFFFB79Fu);
            w_u32(actor + 560u, r_u32(actor + 560u) & 0xFFFFF7FFu);
            goto integrate_lateral;
        }
        acceleration = r_u32(actor + 624u);
        if (acceleration)
        {
            increment = (uint32_t)sub_8001FC90((int32_t)acceleration, delta);
            velocity = r_u32(actor + 620u);
            value = velocity + increment;
            crossing = ((int32_t)value <= 0 && (int32_t)velocity > 0) || ((int32_t)value >= 0 && (int32_t)velocity < 0);
            if (!r_u32(actor + 616u))
                crossing = 0;
            w_u32(actor + 620u, value);
        }
        else if (r_u32(actor + 616u))
            crossing = r_u32(actor + 620u) == 0;
        if (!r_u32(actor + 616u) && !r_u32(actor + 620u))
        {
            flags = r_u32(actor + 564u);
            w_u32(actor + 624u, 0u);
            w_u32(actor + 564u, flags & 0xFFFFF7FFu);
        }
    integrate_lateral:
        if (r_u32(actor + 620u) || r_u32(actor + 624u))
        {
            acceleration = r_u32(actor + 624u);
            if (acceleration)
            {
                time = r_u32(actor + 632u) + (uint32_t)delta;
                w_u32(actor + 632u, time);
                increment = (uint32_t)sub_8001FC90((int32_t)acceleration, (int32_t)time);
                velocity = r_u32(actor + 620u) - (uint32_t)((int32_t)increment / 2);
                time = r_u32(actor + 632u);
                increment = (uint32_t)sub_8001FC90((int32_t)time, (int32_t)velocity);
                value = r_u32(actor + 628u) + increment;
            }
            else
            {
                increment = (uint32_t)sub_8001FC90(r_s32(actor + 620u), delta);
                value = r_u32(actor + 616u) + increment;
            }
            displacement = r_u32(actor + 616u);
            if (((int32_t)value <= 0 && (int32_t)displacement > 0) || ((int32_t)value >= 0 && (int32_t)displacement < 0))
            {
                x = r_u32(actor + 184u);
                z = r_u32(actor + 192u);
                flags = r_u32(actor + 564u);
                w_u32(actor + 624u, 0u);
                w_u32(actor + 620u, 0u);
                w_u32(actor + 616u, 0u);
                w_u32(actor + 564u, flags & 0xFFFFBF9Fu);
                w_u32(actor + 560u, r_u32(actor + 560u) & 0xFFFFF7FFu);
                (void)sub_80017BA0((int32_t)x, (int32_t)z, 54u, 0u);
            }
            else
                w_u32(actor + 616u, value);
            displacement = r_u32(actor + 616u);
            if (displacement)
            {
                velocity = r_u32(actor + 620u);
                if ((int32_t)(displacement ^ velocity) >= 0 && !(r_u32(actor + 568u) & 0xCu))
                {
                    acceleration = r_u32(actor + 624u);
                    if (!acceleration)
                        w_u32(actor + 620u, 0u - velocity);
                    else if ((int32_t)(acceleration ^ velocity) >= 0)
                        w_u32(actor + 624u, 0u - acceleration);
                }
            }
        }
        if (r_u32(actor + 616u) && (int32_t)(velocity = r_u32(actor + 620u)) < 0)
        {
            increment = (uint32_t)sub_8001FC90((int32_t)velocity, r_s32(actor + 308u));
            w_u32(actor + 480u, r_u32(actor + 576u) - increment);
        }
        else
            w_u32(actor + 480u, r_u32(actor + 576u));
        flags = r_u32(actor + 564u);
        if (flags & 0x40u)
        {
            if (crossing)
            {
                if (r_s32(actor + 576u) > 58594 && r_s32(actor + 596u) > 0)
                {
                    w_u32(actor + 620u, 0u);
                    w_u32(actor + 624u, 0u);
                }
                else
                {
                    displacement = r_u32(actor + 616u);
                    value = sub_80010028(0x80000u, (int32_t)displacement > 0 ? displacement : 0u - displacement);
                    if ((int32_t)displacement > 0)
                        value = 0u - value;
                    lower_mask = (uint32_t)((int32_t)(value + 32768000u) >> 31);
                    upper = 32768000u - value;
                    upper_mask = (uint32_t)((int32_t)upper >> 31);
                    displacement = r_u32(actor + 616u);
                    w_u32(actor + 624u, value + (lower_mask & (0u - 32768000u - value)) + (upper_mask & upper));
                    w_u32(actor + 632u, 0u);
                    w_u32(actor + 628u, displacement);
                }
            }
        }
        else if (flags & 0x20u)
        {
            if (!crossing && (r_s32(actor + 620u) < 0 || (r_u32(actor + 588u) && r_s32(actor + 576u) > 146485)))
            {
                if (r_s32(actor + 596u) > 0)
                {
                    w_u32(actor + 624u, 0u);
                    w_u32(actor + 620u, 0xFFFE0000u);
                }
            }
            else
            {
                displacement = r_u32(actor + 616u);
                value = sub_80010028(0x20000u, (int32_t)displacement > 0 ? displacement : 0u - displacement);
                if ((int32_t)displacement > 0)
                    value = 0u - value;
                lower_mask = (uint32_t)((int32_t)(value + 32768000u) >> 31);
                upper = 32768000u - value;
                upper_mask = (uint32_t)((int32_t)upper >> 31);
                displacement = r_u32(actor + 616u);
                w_u32(actor + 624u, value + (lower_mask & (0u - 32768000u - value)) + (upper_mask & upper));
                w_u32(actor + 632u, 0u);
                w_u32(actor + 620u, 0u);
                w_u32(actor + 628u, displacement);
            }
        }
        if (!(r_u32(actor + 564u) & 0x400u))
        {
            flags = r_u32(actor + 568u);
            if (!(flags & 0x7FFu) || (flags & 0x800u))
                goto smooth_steering;
        }
        acceleration = r_u32(actor + 684u);
        if (acceleration)
        {
            increment = (uint32_t)sub_8001FC90((int32_t)acceleration, delta);
            velocity = r_u32(actor + 680u) + increment;
            time = r_u32(actor + 724u) + (uint32_t)delta;
            w_u32(actor + 680u, velocity);
            w_u32(actor + 724u, time);
        }
        velocity = r_u32(actor + 680u);
        if (velocity)
        {
            acceleration = r_u32(actor + 684u);
            if (acceleration)
            {
                time = r_u32(actor + 724u);
                increment = (uint32_t)sub_8001FC90((int32_t)acceleration, (int32_t)time);
                velocity = r_u32(actor + 680u) - (uint32_t)((int32_t)increment / 2);
                time = r_u32(actor + 724u);
                increment = (uint32_t)sub_8001FC90((int32_t)time, (int32_t)velocity);
                value = r_u32(actor + 688u) + increment;
                endpoint = (r_u32(actor + 568u) & 0x20000u) ? (r_s32(actor + 652u) > 0 ? 102943u : 0u - 102943u) : 0u;
            }
            else
            {
                increment = (uint32_t)sub_8001FC90((int32_t)velocity, delta);
                value = r_u32(actor + 676u) + increment;
                endpoint = r_u32(actor + 688u);
            }
            keep = 0u;
            if ((int32_t)value >= (int32_t)endpoint && (int32_t)endpoint < r_s32(actor + 676u))
                keep = 0xFFFFFFFFu;
            else if ((int32_t)endpoint >= (int32_t)value && r_s32(actor + 676u) < (int32_t)endpoint)
                keep = 0xFFFFFFFFu;
            w_u32(actor + 676u, endpoint + (keep & (value - endpoint)));
            velocity = r_u32(actor + 680u);
            acceleration = r_u32(actor + 684u);
            w_u32(actor + 680u, keep & velocity);
            w_u32(actor + 684u, keep & acceleration);
        }
    smooth_steering:
        if (r_s8(actor + 8u) < 2)
        {
            value = (uint32_t)(int32_t)r_s16(actor + 530u) << 4;
            sign = (uint32_t)((int32_t)value < 0);
            if (sign)
                value = 0u - value;
            target = 1024u;
            if ((int32_t)value <= 65535)
            {
                bound = 65528u;
                table_index = 0u;
                if ((int32_t)value >= 65528)
                {
                    table_index = 52u;
                    shift = 0u;
                }
                else
                {
                    bound = 32768u;
                    shift = 15u;
                    while ((int32_t)value >= (int32_t)bound)
                    {
                        --shift;
                        bound += 1u << (shift & 31u);
                        table_index += 4u;
                    }
                    bound -= 1u << (shift & 31u);
                    shift -= 2u;
                }
                bucket = (uint32_t)((int32_t)(value - bound) >> (shift & 31u));
                bound += bucket << (shift & 31u);
                address = 0x800527E0u + 2u * (table_index + bucket);
                first = r_u16(address);
                second = r_u16(address + 2u);
                difference = (value - bound) * (second - first);
                target = (uint32_t)((int32_t)((uint32_t)((int32_t)difference >> (shift & 31u)) + 8u + first) >> 4);
            }
            if (sign)
                target = 0u - target;
            difference = (target - (uint32_t)(int32_t)r_s16(actor + 842u)) << 16;
            product = (int64_t)(int32_t)difference * 498073;
            difference = (uint32_t)((uint64_t)product >> 16);
            product = (int64_t)(int32_t)difference * delta;
            difference = (uint32_t)((uint64_t)product >> 16);
            value = r_u16(actor + 842u);
            w_u16(actor + 842u, value + (uint32_t)((int32_t)difference >> 16));
        }
    next_actor:
        node = r_u32(node + 4u);
    }
}

uint32_t sub_80078DB4(uint32_t list)
{
    uint32_t node;
    FUNCTION_MARKER(0x80078DB4, "RASHCDG.BIN");
    for (node = r_u32(list + 4); node != list; node = r_u32(node + 4))
    {
        uint32_t actor = node - 1088u;
        uint32_t flags = r_u32(actor + 568u);
        uint32_t secondary = r_u32(actor + 852u);
        uint32_t state = r_u32(actor + 364u);
        uint32_t sign = (uint32_t)((int32_t)state >> 31);
        uint32_t linked_mode, primary, direction, value, linked, mask;
        uint32_t x, y, z, horizontal, limit, ratio, scale, angle, dot;
        int32_t random_offset;

        if ((int32_t)((state + sign) ^ sign) >= 3)
            flags ^= 0x00400000u;
        w_u32(actor + 568u, flags);
        if (flags & 0x10u)
        {
            if (r_u32(actor + 676u))
                continue;
            w_u32(actor + 568u, flags & ~0x10u);
            value = r_u32(secondary + 604u) < 2u || (r_u32(actor + 856u) && r_u32(node));
            w_u32(actor + 568u, r_u32(actor + 568u) | (((0u - value) & 0x07FFF780u) + 0x880u));
            continue;
        }
        if (!(flags & 15u))
            continue;
        flags = r_u32(actor + 568u);
        value = r_u32(actor + 560u);
        w_u32(actor + 596u, 0);
        w_u32(actor + 588u, 0);
        linked_mode = (flags & 0x600u) != 0;
        w_u32(actor + 560u, value & 0xF8000000u);
        primary = flags & 1u;
        value = 0;
        if ((int32_t)r_u32(actor + 724u) < (int32_t)r_u32(actor + 728u))
            value = r_u32(actor + 488u);
        w_u32(actor + 488u, value);
        direction = actor + (linked_mode ? 444u : 528u);
        if (primary && ((r_u32(actor + 568u) & 0x2000u) ||
                (int32_t)sub_8002E698(xport_guest_cptr(node - 268u, 6), xport_guest_cptr(direction, 6)) >= 0))
        {
            if (linked_mode)
            {
                x = (uint32_t)(int32_t)r_s16(direction) << 4;
                z = (uint32_t)(int32_t)r_s16(direction + 4u) << 4;
                y = (uint32_t)(int32_t)r_s16(actor + 452u) << 4;
                horizontal = (uint32_t)sub_8001FC90((int32_t)x, (int32_t)x);
                horizontal += (uint32_t)sub_8001FC90((int32_t)z, (int32_t)z);
                if ((int32_t)horizontal < 6)
                {
                    x = 0;
                    z = 655;
                    horizontal = 6;
                }
                limit = 0x10000u - (uint32_t)sub_8001FC90((int32_t)y, (int32_t)y);
                if ((int32_t)limit > 0)
                {
                    limit = 0x10000u - (uint32_t)sub_8001FC90((int32_t)y, (int32_t)y);
                    ratio = (int32_t)horizontal > 0 ? sub_80010028(limit, horizontal) : 0u - sub_80010028(limit, 0u - horizontal);
                }
                else
                {
                    limit = (uint32_t)sub_8001FC90((int32_t)y, (int32_t)y) - 0x10000u;
                    ratio = (int32_t)horizontal > 0 ? 0u - sub_80010028(limit, horizontal) : sub_80010028(limit, 0u - horizontal);
                }
                scale = sub_8004CF74(ratio) << 2;
                value = (uint32_t)sub_8001FC90((int32_t)scale, (int32_t)x);
                w_u16(actor + 450u, (uint16_t)((int32_t)value >> 4));
                value = (uint32_t)sub_8001FC90((int32_t)scale, (int32_t)z);
                w_u16(actor + 454u, (uint16_t)((int32_t)value >> 4));
                (void)sub_8002EE50(r_u32(actor + 480u), xport_guest_cptr(actor + 450u, 6), xport_guest_ptr(actor + 456u, 12));
            }
            else
            {
                uint16_t axis_x = r_u16(actor + 516u), axis_z = r_u16(actor + 520u);
                w_u16(actor + 450u, r_u16(direction));
                w_u16(actor + 452u, r_u16(direction + 2u));
                value = r_u16(direction + 4u);
                y = r_u16(actor + 518u);
                w_u16(actor + 814u, axis_x);
                w_u16(actor + 818u, axis_z);
                w_u16(actor + 816u, (uint16_t)y);
                w_u16(actor + 454u, (uint16_t)value);
            }
            w_u32(actor + 568u, r_u32(actor + 568u) | 0x2000u);
        }
        flags = r_u32(actor + 568u);
        if ((flags & 8u) || ((flags & 4u) && r_u32(secondary + 604u) < 2u))
        {
            if (r_u32(secondary + 604u) < 2u)
            {
                value = r_u32(actor + 616u);
                if ((int32_t)value < -45752)
                    value = (uint32_t)-45752;
                w_u32(actor + 616u, value);
                if ((!linked_mode && (int32_t)value < 0) ||
                    (int32_t)r_u32(actor + 728u) < (int32_t)r_u32(actor + 724u))
                {
                    value = r_u32(secondary + 552u);
                    if (r_u32(secondary + 604u) < 2u)
                        value |= 0x8000u;
                    w_u32(secondary + 552u, value);
                    if (r_u8(r_u32(actor + 852u) + 572u) & 0x10u)
                    {
                        linked = r_u32(r_u32(actor + 856u) + 852u);
                        if (r_u32(linked + 604u) < 2u)
                            w_u32(linked + 552u, r_u32(linked + 552u) | 0x8000u);
                    }
                }
            }
            flags = r_u32(actor + 568u);
            if (!(flags & 8u) && (!linked_mode || !(flags & 4u)))
                continue;
            if ((linked_mode || (int32_t)r_u32(actor + 616u) >= -57190) &&
                (int32_t)r_u32(actor + 728u) >= (int32_t)r_u32(actor + 724u))
                continue;
            if (linked_mode)
            {
                if (r_u32(actor + 568u) & 4u)
                {
                    value = r_u32(actor + 480u);
                    if ((int32_t)value < 0x20000)
                        value = 0x20000u;
                    x = r_u16(actor + 516u);
                    y = r_u16(actor + 518u);
                    z = r_u16(actor + 520u);
                    w_u32(actor + 488u, value);
                    w_u16(actor + 796u, (uint16_t)x);
                    w_u16(actor + 798u, (uint16_t)y);
                    w_u16(actor + 800u, (uint16_t)z);
                    goto recovery_finish;
                }
            }
            else
            {
                (void)sub_8002EAD8(xport_guest_cptr(actor + 504u, 12), xport_guest_cptr(actor + 522u, 6),
                    0u - (r_u32(actor + 308u) << 1), xport_guest_ptr(actor + 184u, 12));
                (void)sub_8002EAD8(xport_guest_cptr(actor + 184u, 12), xport_guest_cptr(actor + 528u, 6),
                    r_u32(actor + 308u), xport_guest_ptr(actor + 184u, 12));
            }
            value = (uint32_t)sub_8001FC90(0xE666, (int32_t)r_u32(secondary + 480u));
            w_u32(actor + 480u, value);
            if ((int32_t)value < 0xA0000)
                value = 0xA0000u;
            w_u32(actor + 480u, value);
            random_offset = (int16_t)(sub_8001FC58() % 284u - 142u);
            random_offset = (int16_t)(random_offset > 0 ? random_offset + 142 : random_offset - 142);
            value = (uint32_t)random_offset * (uint32_t)(int32_t)r_s16(actor + 812u);
            z = (uint32_t)random_offset * (uint32_t)(int32_t)r_s16(actor + 808u);
            w_u16(actor + 452u, r_u16(actor + 810u));
            y = (uint32_t)(int32_t)r_s16(actor + 452u) << 4;
            w_u16(actor + 450u, (uint16_t)(r_u16(actor + 808u) + ((int32_t)value >> 12)));
            w_u16(actor + 454u, (uint16_t)(r_u16(actor + 812u) - ((int32_t)z >> 12)));
            angle = sub_8001FF3C(y);
            value = r_u16(0x8005624Cu + ((((0x4E3u - angle) & 0xFFFu) << 2) | 2u));
            scale = r_u32(actor + 480u);
            if ((int32_t)scale > 580648)
            {
                ratio = (int32_t)scale > 0 ? 0u - sub_80010028(580648u, scale) : sub_80010028(580648u, 0u - scale);
                ratio >>= 4;
                if ((int32_t)(ratio << 16) < (int32_t)(value << 16))
                    ratio = value;
                value = ratio;
            }
            if (r_s16(actor + 452u) != (int16_t)value)
            {
                x = (uint32_t)(int32_t)r_s16(actor + 450u) << 4;
                z = (uint32_t)(int32_t)r_s16(actor + 454u) << 4;
                y = (uint32_t)(int32_t)(int16_t)value << 4;
                w_u16(actor + 452u, (uint16_t)value);
                horizontal = (uint32_t)sub_8001FC90((int32_t)x, (int32_t)x);
                horizontal += (uint32_t)sub_8001FC90((int32_t)z, (int32_t)z);
                if ((int32_t)horizontal < 6)
                {
                    x = 0;
                    z = 655;
                    horizontal = 6;
                }
                limit = 0x10000u - (uint32_t)sub_8001FC90((int32_t)y, (int32_t)y);
                if ((int32_t)limit > 0)
                {
                    limit = 0x10000u - (uint32_t)sub_8001FC90((int32_t)y, (int32_t)y);
                    ratio = (int32_t)horizontal > 0 ? sub_80010028(limit, horizontal) : 0u - sub_80010028(limit, 0u - horizontal);
                }
                else
                {
                    limit = (uint32_t)sub_8001FC90((int32_t)y, (int32_t)y) - 0x10000u;
                    ratio = (int32_t)horizontal > 0 ? 0u - sub_80010028(limit, horizontal) : sub_80010028(limit, 0u - horizontal);
                }
                scale = sub_8004CF74(ratio) << 2;
                value = (uint32_t)sub_8001FC90((int32_t)scale, (int32_t)x);
                w_u16(actor + 450u, (uint16_t)((int32_t)value >> 4));
                value = (uint32_t)sub_8001FC90((int32_t)scale, (int32_t)z);
                w_u16(actor + 454u, (uint16_t)((int32_t)value >> 4));
            }
            value = r_u32(actor + 480u);
            if ((int32_t)value > 0xA0000)
                value = 0xA0000u;
            w_u32(actor + 488u, value);
            random_offset = (int16_t)(sub_8001FC58() % 714u - 357u);
            value = (uint32_t)random_offset * (uint32_t)(int32_t)r_s16(actor + 450u);
            w_u16(actor + 796u, (uint16_t)(r_u16(actor + 454u) + ((int32_t)value >> 12)));
            value = sub_8001FC58();
            z = (uint32_t)random_offset * (uint32_t)(int32_t)r_s16(actor + 454u);
            w_u32(actor + 728u, 0);
            x = r_u16(actor + 450u);
            w_u16(actor + 798u, (uint16_t)(value % 714u - 357u));
            w_u16(actor + 800u, (uint16_t)(((int32_t)z >> 12) - x));
            (void)sub_8002E468(xport_guest_ptr(actor + 796u, 6));
recovery_finish:
            flags = r_u32(actor + 568u);
            linked = r_u32(actor + 856u);
            w_u32(actor + 568u, (flags & 0xFFF00200u) | 0x40A00u);
            if (linked && r_u32(node))
                w_u32(linked + 568u, r_u32(linked + 568u) | 0x40000u);
            value = r_u32(actor + 616u);
            if ((int32_t)value < -57190)
                value = (uint32_t)-57190;
            w_u32(actor + 616u, value);
            continue;
        }
        if (primary)
            goto finish_motion;
        if (!linked_mode && (int32_t)r_u32(actor + 620u) < 0 && (int32_t)r_u32(actor + 616u) <= 0)
            continue;
        if (linked_mode || r_u32(secondary + 604u) < 2u)
            goto finish_motion;
        value = r_u32(actor + 652u);
        sign = (uint32_t)((int32_t)value >> 31);
        if (((value + sign) ^ sign) == 0x1921Fu)
            goto finish_motion;
        if (r_u32(actor + 660u) || (r_u32(actor + 568u) & 0x4000u))
            continue;
        scale = 0x8000u;
        y = r_u32(actor + 620u);
        if ((int32_t)y >= 66)
        {
            x = r_u32(actor + 624u);
            if ((int32_t)x > 0)
                scale = (int32_t)y > 0 ? 0u - sub_80010028(x, y) : sub_80010028(x, 0u - y);
            else
                scale = (int32_t)y > 0 ? sub_80010028(0u - x, y) : 0u - sub_80010028(0u - x, 0u - y);
        }
        if ((int32_t)scale < 0x8000)
            scale = 0x8000u;
        value = (int32_t)r_u32(actor + 652u) > 0 ? 0x1921Fu : 0xFFFE6DE1u;
        linked = r_u32(actor + 856u);
        w_u32(actor + 660u, value);
        if (linked && r_u32(node))
            w_u32(actor + 660u, 0xFFFE6DE1u);
        x = (r_u32(actor + 660u) - r_u32(actor + 652u)) << 1;
        if ((int32_t)x > 0)
            ratio = (int32_t)scale > 0 ? sub_80010028(x, scale) : 0u - sub_80010028(x, 0u - scale);
        else
            ratio = (int32_t)scale > 0 ? 0u - sub_80010028(0u - x, scale) : sub_80010028(0u - x, 0u - scale);
        value = r_u32(actor + 728u) - r_u32(actor + 724u);
        y = r_u32(actor + 652u);
        w_u32(actor + 660u, ratio);
        w_u32(actor + 656u, 0);
        w_u32(actor + 576u, 0);
        w_u32(actor + 488u, 0);
        w_u32(actor + 728u, value);
        w_u32(actor + 664u, y);
        if ((int32_t)value < 0)
            value = 0;
        w_u32(actor + 728u, value);
        w_u32(actor + 724u, 0);
        w_u32(actor + 688u, 0);
        dot = sub_8002E698(xport_guest_cptr(actor + 528u, 6), xport_guest_cptr(actor + 814u, 6));
        value = sub_8002E698(xport_guest_cptr(actor + 528u, 6), xport_guest_cptr(actor + 450u, 6));
        angle = sub_80020018(dot, value);
        value = (uint32_t)((int32_t)(angle * 25736u) >> 8);
        w_u32(actor + 676u, value);
        if ((int32_t)value > 0)
            ratio = (int32_t)scale > 0 ? 0u - sub_80010028(value, scale) : sub_80010028(value, 0u - scale);
        else
            ratio = (int32_t)scale > 0 ? sub_80010028(0u - value, scale) : 0u - sub_80010028(0u - value, 0u - scale);
        w_u32(actor + 680u, ratio);
        w_u32(actor + 568u, r_u32(actor + 568u) | 0x4000u);
        continue;
finish_motion:
        if ((int32_t)r_u32(actor + 724u) < (int32_t)r_u32(actor + 728u))
            continue;
        if (!primary && !linked_mode && (int32_t)r_u32(actor + 616u) < 0)
            continue;
        mask = 0u - primary;
        value = r_u32(actor + 616u);
        w_u32(actor + 620u, r_u32(actor + 620u) & mask);
        y = r_u32(actor + 624u);
        w_u32(actor + 688u, 0);
        w_u32(actor + 676u, 0);
        w_u32(actor + 624u, y & mask);
        w_u32(actor + 616u, value & mask);
        if (primary)
        {
            (void)sub_80073D74(actor);
            value = r_u32(secondary + 604u) >= 2u && !r_u32(actor + 856u);
            w_u32(actor + 568u, r_u32(actor + 568u) | ((0u - value) & 0x880u));
        }
        else
        {
            value = (r_u32(actor + 560u) & 0x300u) ? 0x1C9C4u : 0;
            w_u32(actor + 488u, 0);
            x = r_u16(actor + 528u);
            y = r_u16(actor + 530u);
            z = r_u16(actor + 532u);
            direction = r_u16(actor + 516u);
            horizontal = r_u16(actor + 518u);
            limit = r_u16(actor + 520u);
            w_u32(actor + 576u, value);
            w_u16(actor + 450u, (uint16_t)x);
            w_u16(actor + 452u, (uint16_t)y);
            w_u16(actor + 454u, (uint16_t)z);
            w_u16(actor + 814u, (uint16_t)direction);
            w_u16(actor + 816u, (uint16_t)horizontal);
            w_u16(actor + 818u, (uint16_t)limit);
            if (r_u32(secondary + 604u) < 2u)
            {
                value = r_u32(actor + 560u);
                if ((value & 0x08002000u) == 0x08000000u)
                    value |= 0x6000u;
                w_u32(actor + 560u, value);
            }
            else
            {
                value = sub_80043F00(0xF2000002u) & 255u;
                (void)sub_80017BA0((int32_t)r_u32(actor + 184u), (int32_t)r_u32(actor + 192u), ((value * 5u) >> 8) + 50u, 0);
                w_u32(actor + 568u, r_u32(actor + 568u) | 0x900u);
            }
        }
        value = r_u32(actor + 560u);
        if (r_u32(secondary + 604u) < 2u)
            value |= 0x10080u;
        w_u32(actor + 560u, value);
        flags = r_u32(actor + 568u);
        value = flags & 0xFFFF9FF0u;
        w_u32(actor + 568u, value);
        if (!(flags & 0x600u))
            value |= 0x08000000u;
        w_u32(actor + 568u, value);
        w_u32(actor + 828u, 0);
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
    if (((int32_t)(uint32_t)(distance)) <= 0)
    {
        distance = low - magnitude;
        if (((int32_t)(uint32_t)(span)) <= 0)
            return ((int32_t)(uint32_t)(sub_80010028(distance, low - high)));
        return -((int32_t)(uint32_t)(sub_80010028(distance, span)));
    }
    if (((int32_t)(uint32_t)(span)) > 0)
        return ((int32_t)(uint32_t)(sub_80010028(distance, span)));
    return -((int32_t)(uint32_t)(sub_80010028(distance, low - high)));
}

uint32_t sub_8007AC04(uint32_t list)
{
    int32_t scalar_sign_214;
    int32_t scalar_sign_215;

    uint32_t scalar_low_27;
    uint32_t scalar_low_28;
    uint32_t scalar_low_29;

    uint32_t node = r_u32(list + 4);
    uint32_t result = 0x80050000;

    FUNCTION_MARKER(0x8007AC04, "RASHCDG.BIN");
    while (node != list)
    {
        uint32_t actor = node - 1088;
        uint32_t state = r_u32(actor + 564);
        uint32_t flags = r_u32(actor + 568);
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
            w_u32(actor + 676, 0);
            w_u32(actor + 828, 0);
        }
        if (r_u32(actor + 676))
        {
            uint32_t index = ((uint32_t)(163u * r_u32(actor + 676)) >> 14) & 0xFFF;
            sine = (int32_t)r_s16(0x8005624C + 4 * index) << 4;
            cosine = (int32_t)r_s16(0x8005624E + 4 * index) << 4;
        }
        state = r_u32(actor + 564);
        flags = r_u32(actor + 568);
        {
            uint32_t constrained = (flags & 0x1F) || (state & 0x3400);
            uint32_t moving = (!(flags & 0x600) && r_u32(actor + 576)) || (state & 0x40000) || r_u32(actor + 488) || r_u32(actor + 676);
            if ((moving && !constrained) || (constrained && !(flags & 15)) || (flags & 0x4000))
            {
                if (sine || cosine < 0)
                {
                    (void)sub_8002EB78(xport_guest_ptr(actor + 528, 6), xport_guest_ptr(actor + 516, 6), xport_guest_ptr(actor + 450, 6), (uint32_t)cosine, 0u - (uint32_t)sine);
                    if (!sub_8002E468(xport_guest_ptr(actor + 450, 6)))
                        (void)sub_8008CF74(actor);
                    leaf_direction_op(actor + 522, actor + 450, actor + 814);
                }
                else
                {
                    unsigned i;
                    for (i = 0; i < 3; ++i)
                    {
                        w_u16(actor + 450 + 2 * i, r_u16(actor + 528 + 2 * i));
                        w_u16(actor + 814 + 2 * i, r_u16(actor + 516 + 2 * i));
                    }
                    w_u32(actor + 564, state & ~0x400u);
                }
            }
        }
        w_u32(actor + 732, (uint32_t)(157 * (int32_t)r_s16(actor + 518)));
        w_u32(actor + 736, (uint32_t)(157 * (int32_t)r_s16(actor + 524)));
        w_u32(actor + 740, (uint32_t)(157 * (int32_t)r_s16(actor + 530)));
        longitudinal = ((int32_t)(uint32_t)(r_u32(actor + 740)));
        if (r_u32(actor + 676))
        {
            longitudinal = (int32_t)(((int64_t)longitudinal * cosine) >> 16) - (int32_t)(((int64_t)((int32_t)(uint32_t)(r_u32(actor + 732))) * sine) >> 16);
        }
        w_u32(actor + 760, (uint32_t)longitudinal);
        angle = ((int32_t)(uint32_t)(sub_80020018(r_u32(actor + 732), r_u32(actor + 736))));
        heading = -((scalar_low_27 = (uint32_t)((uint64_t)(uint32_t)((int32_t)(25736)) * (uint32_t)((int32_t)(angle)))), (((int32_t)(((int32_t)(uint32_t)(scalar_low_27)) >> ((uint32_t)(8))))));
        w_u32(actor + 668, (uint32_t)heading);
        specification = r_u32(actor + 556);
        flags = r_u32(actor + 568);
        if (flags & 0x14C)
        {
            int32_t value = ((int32_t)(uint32_t)(r_u32(actor + 652)));
            int32_t ratio = 0;
            int32_t correction = 0;
            if (r_u32(actor + 488) && (r_u32(actor + 488) ^ (uint32_t)value) < 0x80000000u)
            {
                uint32_t magnitude = ((scalar_sign_214 = ((int32_t)(value)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_214 + ((int32_t)(value))) ^ scalar_sign_214)))));
                ratio = race_slip_ratio(magnitude, r_u32(specification + 296), r_u32(specification + 300));
                correction = ((int32_t)((int32_t)(((int64_t)((int32_t)(ratio)) * ((int32_t)(((int32_t)(uint32_t)(r_u32(specification + 236)))))) >> 16)));
                value = value > 0 ? value + correction : value - correction;
            }
            w_u32(actor + 672, (uint32_t)ratio);
            w_u32(actor + 636, (uint32_t)(value - heading));
        }
        else
        {
            int32_t value = ((int32_t)(uint32_t)(r_u32(actor + 636))) + heading;
            int32_t ratio = 0;
            int32_t correction = 0;
            if (r_u32(actor + 488) && (r_u32(actor + 488) ^ (uint32_t)value) < 0x80000000u)
            {
                uint32_t low = r_u32(specification + 296);
                uint32_t high = r_u32(specification + 300) + r_u32(specification + 236);
                ratio = race_slip_ratio(((scalar_sign_215 = ((int32_t)(value)) >> 31), (((uint32_t)((uint32_t)((scalar_sign_215 + ((int32_t)(value))) ^ scalar_sign_215))))), low, high);
                correction = ((int32_t)((int32_t)(((int64_t)((int32_t)(ratio)) * ((int32_t)(((int32_t)(uint32_t)(r_u32(specification + 236)))))) >> 16)));
            }
            w_u32(actor + 672, (uint32_t)ratio);
            value = value <= 0 ? value + correction : value - correction;
            w_u32(actor + 652, (uint32_t)value);
        }
        pitch = ((scalar_low_28 = (uint32_t)((uint64_t)(uint32_t)((int32_t)(652)) * (uint32_t)((int32_t)(((int32_t)(uint32_t)(r_u32(actor + 616))))))), (((int32_t)(((int32_t)(uint32_t)(scalar_low_28)) >> ((uint32_t)(16))))));
        flags = r_u32(actor + 568);
        if (!r_u32(actor + 856) || (flags & 0xEC))
            lateral = ((int32_t)(uint32_t)(r_u32(actor + 652)));
        else if (flags & 0x100)
            lateral = ((int32_t)(uint32_t)(r_u32(actor + 652))) > 0 ? 183010 : -102943;
        else
        {
            int32_t first = ((int32_t)(uint32_t)(r_u32(actor + 636)));
            int32_t second = ((int32_t)(uint32_t)(r_u32(actor + 652)));
            uint32_t mask = first < 0 ? 1u : 0u;
            lateral = ((int32_t)(uint32_t)(mask & (uint32_t)first));
            if (first < second)
                lateral = ((int32_t)(uint32_t)(mask & (uint32_t)second));
        }
        if (r_u16(actor + 172) >= r_u32(r_u32(0x8005B2F8) + 48) || r_u32(0x8005B220))
        {
            int32_t target = lateral - heading;
            int32_t filtered;
            if (flags & 0x7FF)
                filtered = (((int32_t)(uint32_t)(r_u32(actor + 648))) + 3 * target) / 4;
            else if (!r_u32(actor + 660) && r_s16(actor + 944))
                filtered = (3 * ((int32_t)(uint32_t)(r_u32(actor + 648))) + target) / 4;
            else
                filtered = (((int32_t)(uint32_t)(r_u32(actor + 648))) + target) / 2;
            w_u32(actor + 648, (uint32_t)filtered);
            lateral = filtered + heading;
        }
        angles[0] = (uint16_t)-pitch;
        angles[1] = 0;
        angles[2] = (uint16_t)-((scalar_low_29 = (uint32_t)((uint64_t)(uint32_t)((int32_t)(652)) * (uint32_t)((int32_t)(lateral)))), (((int32_t)(((int32_t)(uint32_t)(scalar_low_29)) >> ((uint32_t)(16))))));
        (void)sub_8004D2A4(angles, matrix);
        state = r_u32(actor + 564);
        if ((state & 0x200000) || ((flags & 0x100) && r_u32(actor + 772)))
        {
            (void)sub_8002EAD8(xport_guest_ptr(actor + 504, 12), xport_guest_ptr(actor + 522, 6), r_u32(actor + 772), xport_guest_ptr(actor + 184, 12));
        }
        else if (pitch)
        {
            int32_t scale = ((int32_t)(uint32_t)(r_u32(actor + 308)));
            uint32_t index = (uint32_t)pitch & 0xFFF;
            int32_t first;
            int32_t second;
            unsigned i;
            if (pitch > 0)
                scale = -scale;
            scale = (3 * scale) / 4;
            first = ((int32_t)(uint32_t)((uint32_t)sub_8001FC90(scale, (int32_t)r_s16(0x8005624C + 4 * index) << 4)));
            second = ((int32_t)(uint32_t)((uint32_t)sub_8001FC90(scale, 0x10000 - ((int32_t)r_s16(0x8005624E + 4 * index) << 4))));
            for (i = 0; i < 3; ++i)
            {
                int32_t position = ((int32_t)(uint32_t)(r_u32(actor + 504 + 4 * i)));
                position += (int32_t)(((int64_t)((int32_t)r_s16(actor + 438 + 2 * i) << 4) * first) >> 16);
                position += (int32_t)(((int64_t)((int32_t)r_s16(actor + 528 + 2 * i) << 4) * second) >> 16);
                w_u32(actor + 184 + 4 * i, (uint32_t)position);
            }
        }
        else
        {
            w_u32(actor + 184, r_u32(actor + 504));
            w_u32(actor + 188, r_u32(actor + 508));
            w_u32(actor + 192, r_u32(actor + 512));
        }
        w_u32(actor + 580, (uint32_t)((int32_t)((int32_t)(((int64_t)((int32_t)(((int32_t)(uint32_t)(r_u32(actor + 576))))) * ((int32_t)(((int32_t)(uint32_t)(r_u32(actor + 576)))))) >> 16))));
        (void)sub_8003FA40(matrix, xport_guest_cptr(actor + 516, 18), xport_guest_ptr(actor + 432, 18));
        result = actor + 436;
        node = r_u32(node + 4);
    }
    return result;
}

uint32_t sub_80095724(uint32_t actor, uint32_t delta)
{
    uint32_t descriptor;
    uint32_t target;

    FUNCTION_MARKER(0x80095724, "RASHCDG.BIN");
    if (!actor)
        return 0;
    if (r_s16(actor + 320))
    {
        if (!r_u32(0x800CCA80))
            w_u32(0x800CCA80, 1);
        return r_u32(0x800CCA80);
    }
    w_u32(actor + 568, r_u32(actor + 568) & ~0x800u);
    if (r_s16(0x800D6182) == -1)
        return 0;
    descriptor = r_u32(actor + 1084);
    if (((r_u8(descriptor + 1) & 15) == 2 && !(r_u8(actor + 928) & 0x10)) || (r_u8(descriptor) & 0x10) || r_u32(descriptor + 40) || r_u32(r_u32(actor + 852) + 604) >= 3)
        return 0;
    target = sub_80095BF8(actor, delta);
    w_u32(actor + 924, target);
    w_u32(actor + 480, target);
    sub_80097518(actor, ((int32_t)(uint32_t)(sub_8001FC90(((int32_t)(uint32_t)(delta)), ((int32_t)(uint32_t)(target))))));
    target = sub_8003B61C(actor + 172);
    w_u32(actor + 324, target);
    return target;
}

uint32_t sub_800A2A64(int32_t delta)
{
    uint32_t count_pointer = r_u32(0x800CE51C);
    int32_t remaining = r_s32(count_pointer);
    uint32_t actor = r_u32(0x800CE510), result = count_pointer;
    uint32_t flags, value, old_value, sign, magnitude, dot, drag, step, timer;
    uint32_t other_dot, negative, rate, product, index, sine, cosine, first, second;
    uint32_t denominator, reciprocal, vector[3];
    uint16_t quaternion[4], angles[3], matrix[9], captured[6];
    unsigned axis, component;
    int unchanged_basis;
    FUNCTION_MARKER(0x800A2A64, "RASHCDG.BIN");
    while (remaining >= 0)
    {
        if (!r_u16(actor + 172))
            goto next_object;
        if (r_u32(actor + 592) & 0x100)
        {
            value = 0 - (uint32_t)(r_s32(actor + 312) >> 1);
            (void)sub_8002EAD8(xport_guest_cptr(actor + 184, 12), xport_guest_cptr(actor + 438, 6), value, xport_guest_ptr(actor + 580, 12));
            (void)sub_80037450(actor);
            (void)sub_800A3A84(actor);
            flags = r_u32(actor + 592);
            if (!(flags & 2))
            {
                if (flags & 4)
                    (void)sub_8002EAD8(xport_guest_cptr(actor + 504, 12), xport_guest_cptr(actor + 522, 6), 0 - r_u32(actor + 308), xport_guest_ptr(actor + 184, 12));
                else
                {
                    first = r_u32(actor + 504);
                    second = r_u32(actor + 508);
                    value = r_u32(actor + 512);
                    w_u32(actor + 184, first);
                    w_u32(actor + 188, second);
                    w_u32(actor + 192, value);
                }
            }
            w_u32(actor + 592, r_u32(actor + 592) & 0xFFFFFEFF);
        }
        if (r_u32(actor + 592) & 1)
        {
            dot = sub_8002E698(xport_guest_cptr(actor + 444, 6), xport_guest_cptr(actor + 560, 6));
            sign = (uint32_t)((int32_t)dot >> 31);
            magnitude = (dot + sign) ^ sign;
            if ((int32_t)magnitude <= 61603)
            {
                for (axis = 0; axis < 3; ++axis)
                    xport_gte_write_control(2 * axis, (uint32_t)(int32_t)r_s16(actor + 444 + 2 * axis));
                for (axis = 0; axis < 3; ++axis)
                    xport_gte_write_data(9 + axis, (uint32_t)(int32_t)r_s16(actor + 560 + 2 * axis));
                xport_gte_execute(0x178000C);
                first = xport_gte_read_data(9);
                second = xport_gte_read_data(10);
                value = xport_gte_read_data(11);
                w_u16(actor + 566, first);
                w_u16(actor + 568, second);
                w_u16(actor + 570, value);
                (void)sub_8002E468(xport_guest_ptr(actor + 566, 6));
            }
            else
            {
                for (axis = 0; axis < 3; ++axis)
                    captured[axis] = r_u16(actor + 438 + 2 * axis);
                for (axis = 0; axis < 3; ++axis)
                    w_u16(actor + 566 + 2 * axis, captured[axis]);
            }
            for (axis = 0; axis < 3; ++axis)
                xport_gte_write_control(2 * axis, (uint32_t)(int32_t)r_s16(actor + 560 + 2 * axis));
            for (axis = 0; axis < 3; ++axis)
                xport_gte_write_data(9 + axis, (uint32_t)(int32_t)r_s16(actor + 566 + 2 * axis));
            xport_gte_execute(0x178000C);
            first = xport_gte_read_data(9);
            second = xport_gte_read_data(10);
            value = xport_gte_read_data(11);
            w_u16(actor + 572, first);
            w_u16(actor + 574, second);
            w_u16(actor + 576, value);
            value = 25736 * sub_8001FF3C(dot);
            for (axis = 0; axis < 3; ++axis)
            {
                captured[axis] = r_u16(actor + 572 + 2 * axis);
                captured[axis + 3] = r_u16(actor + 566 + 2 * axis);
            }
            w_u32(actor + 540, (uint32_t)((int32_t)value >> 8));
            w_u32(actor + 556, 0);
            for (axis = 0; axis < 3; ++axis)
                w_u16(actor + 528 + 2 * axis, captured[axis]);
            for (axis = 0; axis < 3; ++axis)
                w_u16(actor + 516 + 2 * axis, captured[axis + 3]);
            value = sub_8002E698(xport_guest_cptr(actor + 438, 6), xport_guest_cptr(actor + 566, 6));
            value = 25736 * (1024 - sub_8001FF3C(value));
            w_u32(actor + 548, (uint32_t)((int32_t)value >> 8));
            (void)sub_8002EE50(r_u32(actor + 480), xport_guest_ptr(actor + 450, 6), xport_guest_ptr(actor + 456, 12));
            value = 0 - (uint32_t)(r_s32(actor + 312) >> 1);
            (void)sub_8002EAD8(xport_guest_cptr(actor + 184, 12), xport_guest_cptr(actor + 438, 6), value, xport_guest_ptr(actor + 580, 12));
            w_u32(actor + 592, (r_u32(actor + 592) & 0xFFFFFFDE) | 4);
        }
        flags = r_u32(actor + 592);
        if (flags & 2)
        {
            if (flags & 8)
            {
                rate = r_u32(actor + 544);
                unchanged_basis = 1;
                if (rate)
                {
                    value = (uint32_t)sub_8001FC90((int32_t)rate, delta);
                    old_value = r_u32(actor + 540);
                    value += old_value;
                    if (((int32_t)value <= 0 && (int32_t)old_value >= 0) || ((int32_t)value >= 0 && (int32_t)old_value <= 0))
                    {
                        w_u32(actor + 540, 0);
                        w_u32(actor + 544, 0);
                    }
                    else
                        w_u32(actor + 540, value);
                }
                dot = sub_8002E698(xport_guest_cptr(actor + 566, 6), xport_guest_cptr(actor + 522, 6));
                flags = r_u32(actor + 592);
                if (!(flags & 0x20))
                {
                    if ((flags & 0x40) ? (int32_t)dot < 0 : (int32_t)dot > 0)
                        goto no_drag;
                    w_u32(actor + 592, flags | 0x20);
                    if (r_u32(actor + 180) != 2)
                        w_u32(actor + 488, 0);
                    w_u32(actor + 556, 0);
                }
                dot = sub_8002E698(xport_guest_cptr(actor + 560, 6), xport_guest_cptr(actor + 522, 6));
                other_dot = sub_8002E698(xport_guest_cptr(actor + 572, 6), xport_guest_cptr(actor + 522, 6));
                negative = other_dot >> 31;
                sign = (uint32_t)((int32_t)dot >> 31);
                magnitude = (dot + sign) ^ sign;
                if ((int32_t)magnitude < 6554)
                {
                    for (axis = 0; axis < 3; ++axis)
                        xport_gte_write_control(2 * axis, (uint32_t)(int32_t)r_s16(actor + 566 + 2 * axis));
                    for (axis = 0; axis < 3; ++axis)
                        xport_gte_write_data(9 + axis, (uint32_t)(int32_t)r_s16(actor + 522 + 2 * axis));
                    xport_gte_execute(0x178000C);
                    first = xport_gte_read_data(9);
                    second = xport_gte_read_data(10);
                    value = xport_gte_read_data(11);
                    w_u16(actor + 560, first);
                    w_u16(actor + 562, second);
                    w_u16(actor + 564, value);
                    if (negative)
                    {
                        first = r_u16(actor + 560);
                        second = r_u16(actor + 564);
                        w_u16(actor + 560, 0 - first);
                        value = r_u16(actor + 562);
                        w_u16(actor + 564, 0 - second);
                        w_u16(actor + 562, 0 - value);
                    }
                }
                else
                {
                    (void)sub_8002EA20(xport_guest_cptr(actor + 560, 6), xport_guest_cptr(actor + 522, 6), (int32_t)(0 - dot), vector);
                    for (axis = 0; axis < 3; ++axis)
                        w_u16(actor + 560 + 2 * axis, (uint32_t)((int32_t)xport_load_le32(vector + axis) >> 4));
                    unchanged_basis = 0;
                }
                (void)sub_8002E468(xport_guest_ptr(actor + 560, 6));
                for (axis = 0; axis < 3; ++axis)
                    xport_gte_write_control(2 * axis, (uint32_t)(int32_t)r_s16(actor + 522 + 2 * axis));
                for (axis = 0; axis < 3; ++axis)
                    xport_gte_write_data(9 + axis, (uint32_t)(int32_t)r_s16(actor + 560 + 2 * axis));
                xport_gte_execute(0x178000C);
                first = xport_gte_read_data(9);
                second = xport_gte_read_data(10);
                value = xport_gte_read_data(11);
                w_u16(actor + 566, first);
                w_u16(actor + 568, second);
                w_u16(actor + 570, value);
                if (!unchanged_basis)
                    (void)sub_8002E468(xport_guest_ptr(actor + 566, 6));
                for (axis = 0; axis < 3; ++axis)
                    xport_gte_write_control(2 * axis, (uint32_t)(int32_t)r_s16(actor + 560 + 2 * axis));
                for (axis = 0; axis < 3; ++axis)
                    xport_gte_write_data(9 + axis, (uint32_t)(int32_t)r_s16(actor + 566 + 2 * axis));
                xport_gte_execute(0x178000C);
                first = xport_gte_read_data(9);
                second = xport_gte_read_data(10);
                value = xport_gte_read_data(11);
                w_u16(actor + 572, first);
                w_u16(actor + 574, second);
                w_u16(actor + 576, value);
                if (negative)
                {
                    first = r_u16(actor + 572);
                    second = r_u16(actor + 574);
                    w_u16(actor + 572, 0 - first);
                    value = r_u16(actor + 576);
                    w_u16(actor + 574, 0 - second);
                    first = r_u16(actor + 566);
                    w_u16(actor + 576, 0 - value);
                    second = r_u16(actor + 568);
                    w_u16(actor + 566, 0 - first);
                    value = r_u16(actor + 570);
                    w_u16(actor + 568, 0 - second);
                    w_u16(actor + 570, 0 - value);
                }
            no_drag:
                drag = 0;
                if (!r_u32(actor + 488) && !r_u32(actor + 544) && unchanged_basis)
                    w_u32(actor + 592, r_u32(actor + 592) | 0x10);
            }
            else
            {
                timer = r_u16(actor + 578);
                if (timer)
                {
                    step = (uint32_t)delta;
                    if (delta >= (int32_t)timer)
                    {
                        step = timer;
                        w_u16(actor + 578, 0);
                    }
                    else
                        w_u16(actor + 578, timer - (uint32_t)delta);
                    rate = r_u32(actor + 544);
                    if (rate)
                    {
                        value = (uint32_t)sub_8001FC90((int32_t)rate, (int32_t)step);
                        w_u32(actor + 540, r_u32(actor + 540) + value);
                    }
                    rate = r_u32(actor + 552);
                    if (rate)
                    {
                        value = (uint32_t)sub_8001FC90((int32_t)rate, (int32_t)step);
                        w_u32(actor + 548, r_u32(actor + 548) + value);
                    }
                }
                value = (uint32_t)sub_8001FC90(65208, r_s32(actor + 488));
                w_u32(actor + 488, value);
                drag = (uint32_t)sub_8001FC90(r_s32(0x800D3964), delta);
            }
            rate = r_u32(actor + 488);
            if (rate)
            {
                value = (uint32_t)sub_8001FC90((int32_t)rate, delta) + r_u32(actor + 556);
                w_u32(actor + 556, value);
                if ((int32_t)value > 411773)
                    w_u32(actor + 556, value - 411774);
                else if ((int32_t)value <= -411774)
                    w_u32(actor + 556, value + 411774);
                if (r_u32(actor + 592) & 0x20)
                {
                    if (r_s32(actor + 556) >= 22876)
                    {
                        w_u32(actor + 556, 22876);
                        w_u32(actor + 488, 0);
                    }
                }
                else
                {
                    index = ((163 * r_u32(actor + 556)) >> 14) & 4095;
                    cosine = (uint32_t)(int32_t)r_s16(0x8005624E + 4 * index) << 4;
                    sine = (uint32_t)(int32_t)r_s16(0x8005624C + 4 * index) << 4;
                    (void)sub_8002EB78(xport_guest_cptr(actor + 528, 6), xport_guest_cptr(actor + 516, 6), xport_guest_ptr(actor + 572, 6), cosine, sine);
                    (void)sub_8002EB78(xport_guest_cptr(actor + 528, 6), xport_guest_cptr(actor + 516, 6), xport_guest_ptr(actor + 566, 6), 0 - sine, cosine);
                }
            }
            value = r_u32(actor + 592) & 0x20;
            if (value && r_u32(actor + 556))
                value = (uint32_t)((int32_t)(652 * r_u32(actor + 556)) >> 16);
            else
                value = 0;
            angles[0] = (uint16_t)(0 - value);
            angles[1] = (uint16_t)(0 - ((652 * r_u32(actor + 540)) >> 16));
            angles[2] = (uint16_t)(0 - ((652 * r_u32(actor + 548)) >> 16));
            (void)sub_8004D2A4(angles, matrix);
            for (component = 0; component < 4; ++component)
                xport_gte_write_control(component, xport_load_le32(matrix + 2 * component));
            xport_gte_write_control(4, (uint32_t)(int32_t)(int16_t)xport_load_le16(matrix + 8));
            for (axis = 0; axis < 3; ++axis)
            {
                xport_gte_write_data(9, r_u16(actor + 560 + 2 * axis));
                xport_gte_write_data(10, r_u16(actor + 566 + 2 * axis));
                xport_gte_write_data(11, r_u16(actor + 572 + 2 * axis));
                xport_gte_execute(0x49E012);
                first = xport_gte_read_data(9);
                second = xport_gte_read_data(10);
                value = xport_gte_read_data(11);
                w_u16(actor + 432 + 2 * axis, first);
                w_u16(actor + 438 + 2 * axis, second);
                w_u16(actor + 444 + 2 * axis, value);
            }
            value = (uint32_t)sub_8001FC90((int32_t)drag, r_s32(actor + 456));
            first = r_u32(actor + 456);
            second = r_u32(actor + 460);
            w_u32(actor + 456, first - value);
            value = (uint32_t)sub_8001FC90((int32_t)drag, (int32_t)second);
            value -= (uint32_t)sub_8001FC90(643207, delta);
            first = r_u32(actor + 460);
            second = r_u32(actor + 464);
            w_u32(actor + 460, first - value);
            value = (uint32_t)sub_8001FC90((int32_t)drag, (int32_t)second);
            w_u32(actor + 464, r_u32(actor + 464) - value);
            value = sub_8002E548(xport_guest_cptr(actor + 456, 12));
            w_u32(actor + 480, value);
            if (value)
            {
                negative = (int32_t)value < 0;
                magnitude = negative ? 0 - value : value;
                denominator = (uint32_t)((int32_t)magnitude >> 1) + (uint32_t)((int32_t)(magnitude - 2) >> 31);
                reciprocal = denominator ? 0x80000000 / denominator : 0xFFFFFFFF;
                if (negative)
                    reciprocal = 0 - reciprocal;
                (void)sub_8002EED8(reciprocal, xport_guest_cptr(actor + 456, 12), xport_guest_ptr(actor + 450, 6));
            }
            w_u32(actor + 484, r_u32(actor + 484) + (uint32_t)delta);
        }
        else if (flags & 0x200)
        {
            rate = r_u32(actor + 488);
            if (rate)
            {
                value = (uint32_t)sub_8001FC90((int32_t)rate, delta) + r_u32(actor + 556);
                w_u32(actor + 556, value);
                if ((int32_t)value > 65535)
                {
                    for (axis = 0; axis < 4; ++axis)
                        quaternion[axis] = r_u16(actor + 572 + 2 * axis);
                    flags = r_u32(actor + 592);
                    w_u32(actor + 488, 0);
                    w_u32(actor + 592, flags | 0x1800);
                }
                else
                {
                    step = (uint32_t)(int32_t)(int16_t)((int32_t)value >> 2);
                    for (axis = 0; axis < 4; ++axis)
                    {
                        product = (16384 - step) * (uint32_t)(int32_t)r_s16(actor + 560 + 2 * axis);
                        first = (uint32_t)((int32_t)product >> 14);
                        flags = r_u32(actor + 592);
                        second = (uint32_t)(int32_t)r_s16(actor + 572 + 2 * axis);
                        sign = (flags & 0x40) ? (uint32_t)(int32_t)(int16_t)(0 - step) : step;
                        product = sign * second;
                        quaternion[axis] = (uint16_t)(first + (uint32_t)((int32_t)product >> 14));
                    }
                    (void)sub_80071A28(quaternion);
                    if (r_s32(actor + 556) >= r_s32(actor + 484))
                    {
                        flags = r_u32(actor + 592);
                        w_u32(actor + 488, 0);
                        w_u32(actor + 592, flags | 0x1000);
                    }
                }
                (void)sub_8007198C(quaternion, xport_guest_ptr(actor + 432, 18));
                (void)sub_8008BA18(actor);
            }
            flags = r_u32(actor + 592);
            if (flags & 0x400)
                w_u32(actor + 592, flags & 0xFFFFFBFF);
            else if (!r_u32(actor + 488))
                w_u32(actor + 592, flags & 0xFFFFFDFF);
        }
        else if (r_s32(actor + 480) < 13108)
        {
            w_u32(actor + 488, 0);
            w_u32(actor + 480, 0);
            w_u32(actor + 464, 0);
            w_u32(actor + 460, 0);
            w_u32(actor + 456, 0);
        }
        else
        {
            timer = r_u32(actor + 484) - (uint32_t)delta;
            w_u32(actor + 484, timer);
            if ((int32_t)timer < 0)
                timer = 0;
            w_u32(actor + 484, timer);
            sign = r_s16(actor + 524) >= 3548 ? 0xFFFFFFFF : 0;
            w_u32(actor + 484, (sign & (timer - 65536)) + 65536);
            first = (uint32_t)sub_8001FC90((int32_t)((uint32_t)(int32_t)r_s16(actor + 530) << 4), 643207);
            second = (uint32_t)sub_8001FC90((int32_t)((uint32_t)(int32_t)r_s16(actor + 518) << 4), 643207);
            (void)sub_8002ECB8(xport_guest_cptr(actor + 528, 6), xport_guest_cptr(actor + 516, 6), vector, first, second);
            first = (uint32_t)sub_8001FC90(r_s32(actor + 484), delta);
            second = (uint32_t)sub_8001FC90((int32_t)(sign & 0xFFFF0000), delta);
            (void)sub_8002E6F8(vector, xport_guest_cptr(actor + 456, 12), vector, first, second);
            for (axis = 0; axis < 3; ++axis)
                w_u32(actor + 456 + 4 * axis, r_u32(actor + 456 + 4 * axis) + xport_load_le32(vector + axis));
            value = sub_8002E548(xport_guest_cptr(actor + 456, 12));
            timer = r_u32(actor + 484);
            w_u32(actor + 480, value);
            w_u32(actor + 484, (int32_t)timer > 0 ? timer - (uint32_t)delta : 0);
            value = r_u32(actor + 480);
            negative = (int32_t)value < 0;
            magnitude = negative ? 0 - value : value;
            denominator = (uint32_t)((int32_t)magnitude >> 1) + (uint32_t)((int32_t)(magnitude - 2) >> 31);
            reciprocal = denominator ? 0x80000000 / denominator : 0xFFFFFFFF;
            if (negative)
                reciprocal = 0 - reciprocal;
            (void)sub_8002EED8(reciprocal, xport_guest_cptr(actor + 456, 12), xport_guest_ptr(actor + 450, 6));
            if (r_u32(actor + 592) & 0x80)
            {
                for (axis = 0; axis < 3; ++axis)
                    xport_gte_write_control(2 * axis, (uint32_t)(int32_t)r_s16(actor + 516 + 2 * axis));
                for (axis = 0; axis < 3; ++axis)
                    xport_gte_write_data(9 + axis, (uint32_t)(int32_t)r_s16(actor + 522 + 2 * axis));
                xport_gte_execute(0x178000C);
                first = xport_gte_read_data(9);
                second = xport_gte_read_data(10);
                value = xport_gte_read_data(11);
                w_u16(actor + 528, first);
                w_u16(actor + 530, second);
                w_u16(actor + 532, value);
                (void)sub_8002E468(xport_guest_ptr(actor + 528, 6));
                for (axis = 0; axis < 3; ++axis)
                    xport_gte_write_control(2 * axis, (uint32_t)(int32_t)r_s16(actor + 522 + 2 * axis));
                for (axis = 0; axis < 3; ++axis)
                    xport_gte_write_data(9 + axis, (uint32_t)(int32_t)r_s16(actor + 528 + 2 * axis));
                xport_gte_execute(0x178000C);
                first = xport_gte_read_data(9);
                second = xport_gte_read_data(10);
                value = xport_gte_read_data(11);
                w_u16(actor + 516, first);
                w_u16(actor + 518, second);
                w_u16(actor + 520, value);
                w_u32(actor + 592, r_u32(actor + 592) & 0xFFFFFF7F);
            }
            (void)sub_800A40D4(actor);
        }
        value = sub_80020018((uint32_t)(int32_t)r_s16(actor + 450) << 4, (uint32_t)(int32_t)r_s16(actor + 454) << 4);
        w_u32(actor + 292, value);
        index = value & 4095;
        first = (uint32_t)(int32_t)r_s16(0x8005624E + 4 * index) << 4;
        index = r_u32(actor + 292) & 4095;
        w_u32(actor + 296, first);
        w_u32(actor + 300, (uint32_t)(int32_t)r_s16(0x8005624C + 4 * index) << 4);
    next_object:
        result = r_u32(0x800CE514);
        --remaining;
        actor += result;
    }
    return result;
}

uint32_t sub_8007B840(int32_t delta)
{
    uint32_t count_pointer = r_u32(0x800CE4DC);
    uint32_t actor = r_u32(0x800CE4D0);
    int32_t remaining = r_s32(count_pointer);
    uint32_t node, result;

    FUNCTION_MARKER(0x8007B840, "RASHCDG.BIN");
    while (remaining >= 0)
    {
        uint32_t value, first, second, third, flags;
        uint32_t contact_positions[2] = {0}, contact_normals[2] = {0};
        uint32_t inner_distance = 0, outer_distance = 0, distance = 0;
        uint32_t road_direction, road_piece, reach, penalty;
        int airborne, contact = 0;
        unsigned plane;

        if (r_u32(actor + 568) & 0x02000000)
        {
            value = r_u16(actor + 864);
            first = r_u16(actor + 866);
            second = r_u16(actor + 868);
            third = r_u32(actor + 860);
            w_u16(actor + 450, (uint16_t)value);
            w_u16(actor + 452, (uint16_t)first);
            w_u16(actor + 454, (uint16_t)second);
            w_u32(actor + 480, third);
        }
        if (r_u32(actor + 568) & 0x08001800)
            (void)sub_80071BCC(actor);
        if (!r_s16(actor + 320))
            goto next_active_actor;
        airborne = (r_u32(actor + 568) & 0x600) != 0;
        if (!(r_u32(r_u32(actor + 852) + 552) & 0x80000))
        {
            uint32_t identity = r_u16(actor + 172);
            penalty = (r_u32(actor + 568) & 0x1FF) ? 131072 : 0;
            if (identity < r_u32(r_u32(0x8005B2F8) + 48))
            {
                uint32_t marker = 0x800D43C0 + 28 * identity;
                if (r_s16(marker + 22) && r_u32(marker) == r_u32(actor + 360))
                {
                    if (!airborne)
                    {
                        inner_distance = 0 - penalty;
                        outer_distance = (uint32_t)(0 - 327680);
                    }
                    contact_positions[1] = marker + 4;
                    contact_normals[1] = marker + 16;
                }
            }
            value = r_u8(actor + 928);
            road_direction = r_u32(actor + 344);
            if (value & 0x40)
                w_u32(actor + 36, r_u32(actor + 36) | 0x200);
            if ((r_u32(actor + 388) & 1) && r_u32(actor + 372) && !(r_u32(actor + 36) & 0x200))
            {
                road_piece = r_u32(actor + 340);
                reach = r_u8(road_piece + 49);
                if ((int32_t)road_direction > 0)
                    reach = r_u8(road_piece + 48);
                if (reach)
                {
                    value = sub_8002E698(xport_guest_cptr(actor + 450, 6), xport_guest_cptr(road_piece + 2, 6));
                    if (airborne)
                        outer_distance = reach << 15;
                    else
                    {
                        inner_distance = reach << 15;
                        outer_distance = inner_distance - 327680;
                        inner_distance -= penalty;
                    }
                    first = (uint32_t)((int32_t)road_direction >> 31);
                    distance = (road_direction + first) ^ first;
                    road_piece = r_u32(actor + 340);
                    if ((int32_t)(r_u8(road_piece + 48) << 15) < (int32_t)road_direction && (int32_t)value > 0)
                        contact = 1;
                    else if ((int32_t)road_direction < (int32_t)(0 - (r_u8(road_piece + 49) << 15)) && (int32_t)value < 0)
                        contact = 1;
                }
                else
                {
                    value = sub_8003E338(actor);
                    if (value)
                    {
                        if (!airborne)
                        {
                            inner_distance = 0 - penalty;
                            outer_distance = (uint32_t)(0 - 196608);
                        }
                        contact_positions[0] = value + 4;
                        contact_normals[0] = value + 16;
                    }
                }
            }
            for (plane = 0; plane < 2; ++plane)
            {
                uint32_t position = contact_positions[plane];
                uint32_t normal = contact_normals[plane];
                if (position && !contact)
                {
                    value = sub_8002E698(xport_guest_cptr(actor + 450, 6), xport_guest_cptr(normal, 6));
                    if ((int32_t)value <= 0)
                    {
                        first = (uint32_t)(int32_t)r_s16(normal) << 4;
                        second = r_u32(position) - r_u32(actor + 184);
                        third = (uint32_t)sub_8001FC90((int32_t)first, (int32_t)second);
                        first = (uint32_t)(int32_t)r_s16(normal + 4) << 4;
                        second = r_u32(position + 8) - r_u32(actor + 192);
                        distance = third + (uint32_t)sub_8001FC90((int32_t)first, (int32_t)second);
                        contact = (int32_t)outer_distance < (int32_t)distance;
                    }
                    else
                        distance = 0;
                }
            }
            if (contact && (!airborne || !(r_u32(actor + 568) & 15)))
            {
                uint32_t normal = contact_normals[0] ? contact_normals[0] : contact_normals[1];
                if (normal)
                {
                    w_u16(actor + 820, r_u16(normal));
                    w_u16(actor + 822, r_u16(normal + 2));
                    w_u16(actor + 824, r_u16(normal + 4));
                }
                else
                {
                    normal = r_u32(actor + 340);
                    w_u16(actor + 820, r_u16(normal + 2));
                    w_u16(actor + 822, r_u16(normal + 4));
                    w_u16(actor + 824, r_u16(normal + 6));
                    if ((int32_t)road_direction > 0)
                    {
                        value = r_u16(actor + 820);
                        first = r_u16(actor + 824);
                        w_u16(actor + 820, (uint16_t)(0 - value));
                        value = r_u16(actor + 822);
                        w_u16(actor + 824, (uint16_t)(0 - first));
                        w_u16(actor + 822, (uint16_t)(0 - value));
                    }
                }
                if (airborne)
                {
                    if (sub_80083F30(actor, actor + 172, xport_guest_ptr(actor + 820, 6), 3))
                    {
                        (void)sub_80017BA0(r_s32(actor + 184), r_s32(actor + 192), (uint32_t)(int32_t)r_s16(actor + 322), 0);
                        if (r_u32(actor + 568) & 0x1000)
                            (void)sub_80072994(actor);
                    }
                }
                else
                {
                    if ((int32_t)inner_distance < (int32_t)distance)
                    {
                        flags = r_u32(actor + 560);
                        w_u32(actor + 576, 0);
                        w_u32(actor + 560, flags | 0x2000);
                    }
                    else
                    {
                        uint32_t numerator = inner_distance - distance;
                        uint32_t denominator = inner_distance - outer_distance;
                        int numerator_positive = (int32_t)numerator > 0;
                        int denominator_positive = (int32_t)denominator > 0;
                        uint32_t phase = sub_80010028(numerator_positive ? numerator : 0 - numerator, denominator_positive ? denominator : 0 - denominator);
                        if (numerator_positive != denominator_positive)
                            phase = 0 - phase;
                        first = (uint32_t)sub_80074FB4(r_s32(actor + 576), r_s32(r_u32(actor + 556) + 224) >> 1, (int32_t)phase);
                        second = r_u32(actor + 676);
                        w_u32(actor + 576, first);
                        third = (uint32_t)sub_8001FC90((int32_t)second, (int32_t)phase);
                        value = r_u32(actor + 696);
                        w_u32(actor + 676, third);
                        first = (int32_t)value > 65535 ? 62259 : value;
                        value = r_u32(actor + 700);
                        second = (int32_t)value > 65535 ? 62259 : value;
                        w_u32(actor + 696, first);
                        w_u32(actor + 700, second);
                    }
                    flags = r_u32(actor + 560);
                    if ((flags & 0x08000000) || !r_u32(actor + 480))
                    {
                        if (!(flags & 0x2000))
                            flags |= 0x6000;
                        w_u32(actor + 560, flags);
                    }
                }
            }
        }
        flags = r_u32(actor + 564);
        if ((flags & 0x18000) == 0x8000)
        {
            uint32_t preserve = (r_u32(actor + 568) >> 10) & 1;
            if (r_u32(actor + 832))
            {
                if ((flags & 0x20000) && !preserve)
                    (void)sub_80084BE8(actor, 1);
                (void)sub_8002076C(actor);
                if (preserve)
                    w_u32(actor + 828, r_u32(actor + 832));
                else
                    w_u32(actor + 832, 0);
            }
            flags = r_u32(actor + 564);
            w_u32(actor + 744, 0);
            w_u32(actor + 772, 0);
            w_u32(actor + 616, 0);
            w_u32(actor + 564, flags & 0xFFD97FFF);
        }
        if (r_u32(actor + 568) & 0x00800000)
        {
            (void)sub_800374D4(actor, ((r_u32(actor + 560) >> 27) ^ 1) & 1);
            (void)sub_8007504C(actor, actor + 184);
            if (r_u32(actor + 568) & 0x04000000)
            {
                uint16_t direction[3], side[3];
                value = sub_8002E698(xport_guest_cptr(actor + 522, 6), xport_guest_cptr(actor + 432, 6));
                value = sub_8001FF3C(value);
                value *= 25736;
                direction[0] = r_u16(actor + 528);
                direction[1] = r_u16(actor + 530);
                direction[2] = r_u16(actor + 532);
                side[0] = r_u16(actor + 516);
                side[1] = r_u16(actor + 518);
                side[2] = r_u16(actor + 520);
                w_u32(actor + 652, (uint32_t)((int32_t)value >> 8));
                w_u32(actor + 676, 0);
                w_u16(actor + 450, direction[0]);
                w_u16(actor + 452, direction[1]);
                w_u16(actor + 454, direction[2]);
                w_u16(actor + 814, side[0]);
                w_u16(actor + 816, side[1]);
                w_u16(actor + 818, side[2]);
                value = sub_8002E698(xport_guest_cptr(actor + 438, 6), xport_guest_cptr(actor + 528, 6));
                value = 25736 * sub_8001FF3C(value);
                first = (uint32_t)(int32_t)r_s16(actor + 518);
                w_u32(actor + 616, (uint32_t)((int32_t)value >> 8));
                second = (uint32_t)(int32_t)r_s16(actor + 524);
                first *= 157;
                w_u32(actor + 732, first);
                second *= 157;
                third = (uint32_t)(int32_t)r_s16(actor + 530) * 157;
                w_u32(actor + 736, second);
                w_u32(actor + 740, third);
                value = 25736 * sub_80020018(first, second);
                w_u32(actor + 668, 0 - (uint32_t)((int32_t)value >> 8));
                (void)sub_80073D74(actor);
                flags = r_u32(actor + 564);
                value = r_u32(actor + 616);
                w_u32(actor + 564, flags | 0x800);
                first = (int32_t)value > 0 ? (uint32_t)(0 - 262144) : 0;
                if ((int32_t)value < 0)
                    first += 262144;
                w_u32(actor + 620, first);
                w_u32(actor + 624, 0);
            }
            if (airborne)
            {
                for (plane = 0; plane < 3; ++plane)
                {
                    int32_t normal = r_s16(actor + 438 + 2 * plane);
                    value = r_u32(actor + 184 + 4 * plane);
                    w_u32(actor + 784 + 4 * plane, (uint32_t)(((int64_t)normal * -356512) >> 16) + value);
                }
                if (r_u32(actor + 856) && r_u32(actor + 1088))
                    (void)sub_8002EAD8(xport_guest_cptr(actor + 784, 12), xport_guest_cptr(actor + 432, 6), 2 * r_u32(actor + 304), xport_guest_ptr(actor + 784, 12));
            }
            else
            {
                if (!(r_u32(actor + 568) & 0x100))
                {
                    value = r_u32(actor + 616);
                    if (value)
                        (void)sub_8007ED64(actor, (uint32_t)((int32_t)(652 * value) >> 16));
                    else
                    {
                        value = r_u32(actor + 504);
                        first = r_u32(actor + 508);
                        second = r_u32(actor + 512);
                        w_u32(actor + 184, value);
                        w_u32(actor + 188, first);
                        w_u32(actor + 192, second);
                    }
                }
                (void)sub_8002EAD8(xport_guest_cptr(actor + 504, 12), xport_guest_cptr(actor + 528, 6), r_u32(actor + 308), xport_guest_ptr(actor + 784, 12));
            }
            value = r_u32(actor + 856);
            if (value)
                (void)sub_8002EAD8(xport_guest_cptr(actor + 184, 12), xport_guest_cptr(actor + 432, 6), r_u32(actor + 304) + r_u32(value + 304), xport_guest_ptr(value + 184, 12));
            if (r_s8(actor + 8) < 2)
                w_u16(actor + 842, (uint16_t)sub_8001FF3C((uint32_t)(int32_t)r_s16(actor + 530) << 4));
            w_u32(actor + 568, r_u32(actor + 568) & 0xFA7FFFFF);
        }
        w_u32(actor + 564, r_u32(actor + 564) & 0xFFFEFFFF);
    next_active_actor:
        value = r_u32(0x800CE4D4);
        --remaining;
        actor += value;
    }

    node = 0x8005B2D8;
    for (;;)
    {
        uint32_t value, first, second, third, flags, mask, sum, speed;
        uint32_t drag, velocity_x, velocity_y, velocity_z, acceleration_y;
        uint32_t roll, pitch, yaw, roll_sine, roll_cosine, pitch_sine, pitch_cosine, yaw_sine, yaw_cosine;
        uint32_t mixed_sine, mixed_cosine, rotation[9];
        node = r_u32(node + 4);
        if (node == 0x8005B2D8)
            break;
        actor = node - 1088;
        drag = (uint32_t)(((int64_t)r_s32(0x800D3960) * delta) >> 16);
        if (!(r_u32(actor + 564) & 0x100000))
        {
            velocity_x = r_u32(actor + 456);
            first = (uint32_t)(((int64_t)(int32_t)drag * (int32_t)velocity_x) >> 16);
            velocity_y = r_u32(actor + 460);
            second = (uint32_t)(((int64_t)(int32_t)drag * (int32_t)velocity_y) >> 16);
            acceleration_y = (uint32_t)(((int64_t)r_s32(actor + 484) * delta) >> 16);
            velocity_z = r_u32(actor + 464);
            third = (uint32_t)(((int64_t)(int32_t)drag * (int32_t)velocity_z) >> 16);
            w_u32(actor + 456, velocity_x - first);
            w_u32(actor + 460, velocity_y - (second - acceleration_y));
            w_u32(actor + 464, velocity_z - third);
        }
        velocity_x = r_u32(actor + 456);
        first = (uint32_t)(((int64_t)(int32_t)velocity_x * (int32_t)velocity_x) >> 16);
        velocity_y = r_u32(actor + 460);
        second = (uint32_t)(((int64_t)(int32_t)velocity_y * (int32_t)velocity_y) >> 16);
        velocity_z = r_u32(actor + 464);
        third = (uint32_t)(((int64_t)(int32_t)velocity_z * (int32_t)velocity_z) >> 16);
        sum = third + second + first;
        w_u32(actor + 580, sum);
        speed = 4 * sub_8004CF74(sum);
        w_u32(actor + 480, speed);
        w_u32(actor + 576, speed);
        if (speed)
        {
            uint32_t magnitude = (int32_t)speed < 0 ? 0 - speed : speed;
            uint32_t divisor = (uint32_t)((int32_t)magnitude >> 1) + (uint32_t)((int32_t)(magnitude - 2) >> 31);
            uint32_t reciprocal = divisor ? 0x80000000 / divisor : 0xFFFFFFFF;
            if ((int32_t)speed < 0)
                reciprocal = 0 - reciprocal;
            (void)sub_8002EED8(reciprocal, xport_guest_cptr(actor + 456, 12), xport_guest_ptr(actor + 450, 6));
        }
        value = r_u32(actor + 624);
        if (value)
        {
            first = (uint32_t)(((int64_t)(int32_t)value * delta) >> 16) + r_u32(actor + 620);
            value = r_u32(actor + 624);
            second = r_u32(actor + 624);
            w_u32(actor + 620, first);
            mask = (int32_t)(first ^ value) < 0 ? 0xFFFFFFFF : 0;
            w_u32(actor + 620, mask & first);
            w_u32(actor + 624, mask & second);
        }
        else
            w_u32(actor + 620, r_u32(r_u32(actor + 556) + 424));
        value = (uint32_t)(((int64_t)r_s32(actor + 620) * delta) >> 16) + r_u32(actor + 616);
        first = (uint32_t)((int32_t)(value + 80067) >> 31) & (0 - 80067 - value);
        second = (uint32_t)((int32_t)(80067 - value) >> 31) & (80067 - value);
        flags = r_u32(actor + 568);
        w_u32(actor + 616, value + first + second);
        if ((flags & 15) && !(flags & 0x4000))
        {
            uint32_t step = (uint32_t)delta;
            uint32_t elapsed = r_u32(actor + 724);
            uint32_t duration = r_u32(actor + 728);
            elapsed += (uint32_t)delta;
            w_u32(actor + 724, elapsed);
            if ((int32_t)duration < (int32_t)elapsed)
                step += duration - elapsed;
            first = (uint32_t)(((int64_t)r_s32(actor + 744) * (int32_t)step) >> 16);
            w_u32(actor + 692, r_u32(actor + 692) + first);
        }
        value = r_u32(actor + 640);
        if (value)
        {
            first = (uint32_t)(((int64_t)(int32_t)value * delta) >> 16) + r_u32(actor + 636);
            value = r_u32(actor + 640);
            second = r_u32(actor + 640);
            w_u32(actor + 672, 0);
            w_u32(actor + 636, first);
            mask = (int32_t)(first ^ value) < 0 ? 0xFFFFFFFF : 0;
            w_u32(actor + 636, mask & first);
            w_u32(actor + 640, mask & second);
        }
        value = r_u32(actor + 636);
        if (r_u32(actor + 856))
        {
            first = (int32_t)value < 0 ? r_u32(actor + 652) >> 31 : 0;
            second = r_u32(actor + 652);
            if ((int32_t)value < (int32_t)second)
                value = second;
            value &= 0 - first;
        }
        roll = (uint32_t)((int32_t)(652 * value) >> 16) & 4095;
        pitch = ((163 * r_u32(actor + 616)) >> 14) & 4095;
        roll_sine = (uint32_t)(int32_t)r_s16(0x8005624C + 4 * roll) << 4;
        pitch_sine = (uint32_t)(int32_t)r_s16(0x8005624C + 4 * pitch) << 4;
        yaw = ((163 * r_u32(actor + 692)) >> 14) & 4095;
        pitch_cosine = (uint32_t)(int32_t)r_s16(0x8005624E + 4 * pitch) << 4;
        yaw_sine = (uint32_t)(int32_t)r_s16(0x8005624C + 4 * yaw) << 4;
        yaw_cosine = (uint32_t)(int32_t)r_s16(0x8005624E + 4 * yaw) << 4;
        roll_cosine = (uint32_t)(int32_t)r_s16(0x8005624E + 4 * roll) << 4;
        mixed_sine = (uint32_t)sub_8001FC90((int32_t)roll_sine, (int32_t)pitch_sine);
        mixed_cosine = (uint32_t)sub_8001FC90((int32_t)pitch_sine, (int32_t)roll_cosine);
        first = (uint32_t)sub_8001FC90((int32_t)roll_cosine, (int32_t)yaw_cosine);
        rotation[0] = first + (uint32_t)sub_8001FC90((int32_t)mixed_sine, (int32_t)yaw_sine);
        rotation[1] = (uint32_t)sub_8001FC90((int32_t)roll_sine, (int32_t)pitch_cosine);
        first = (uint32_t)sub_8001FC90((int32_t)roll_cosine, (int32_t)yaw_sine);
        rotation[2] = (uint32_t)sub_8001FC90((int32_t)mixed_sine, (int32_t)yaw_cosine) - first;
        first = (uint32_t)sub_8001FC90((int32_t)roll_sine, (int32_t)yaw_cosine);
        rotation[3] = (uint32_t)sub_8001FC90((int32_t)mixed_cosine, (int32_t)yaw_sine) - first;
        rotation[4] = (uint32_t)sub_8001FC90((int32_t)roll_cosine, (int32_t)pitch_cosine);
        first = (uint32_t)sub_8001FC90((int32_t)roll_sine, (int32_t)yaw_sine);
        rotation[5] = first + (uint32_t)sub_8001FC90((int32_t)mixed_cosine, (int32_t)yaw_cosine);
        rotation[6] = (uint32_t)sub_8001FC90((int32_t)pitch_cosine, (int32_t)yaw_sine);
        rotation[7] = 0 - pitch_sine;
        rotation[8] = (uint32_t)sub_8001FC90((int32_t)pitch_cosine, (int32_t)yaw_cosine);
        for (value = 0; value < 7; ++value)
            w_u16(actor + 432 + 2 * value, (uint16_t)((int32_t)rotation[value] >> 4));
        w_u16(actor + 448, (uint16_t)((int32_t)rotation[8] >> 4));
        w_u16(actor + 446, (uint16_t)((int32_t)rotation[7] >> 4));
        (void)sub_8007EA64(actor);
        flags = r_u32(actor + 560);
        if (r_s32(actor + 768) > 196608)
            flags |= 0x800000;
        first = r_u32(actor + 768);
        w_u32(actor + 560, flags);
        if ((int32_t)first > 196608)
        {
            value = r_u32(actor + 852);
            flags = r_u32(value + 552);
            if (!(flags & 0x2000))
            {
                w_u32(value + 552, flags | 0x2000);
                (void)sub_800C4550(2, r_u32(actor + 852), 10);
            }
        }
        if (!(r_u32(actor + 560) & 0x08000000))
        {
            if (r_u32(actor + 856) && r_u32(actor + 1088))
                (void)sub_80074D58(actor);
            (void)sub_80074E6C(actor, (uint32_t)delta);
        }
    }
    sub_80079B20(0x8005B2D8, delta);
    sub_80079B20(0x8005B298, delta);
    sub_80079B20(0x8005B350, delta);

    node = r_u32(0x8005B354);
    while (node != 0x8005B350)
    {
        uint32_t body, flags, value, first, second, third, phase, numerator, denominator, sum, speed;
        uint32_t motion[3], rate, elapsed, limit, old, mask;
        int passenger, numerator_positive, denominator_positive, slope, pull;
        unsigned axis;
        actor = node - 1088;
        body = r_u32(actor + 852);
        passenger = (r_u8(body + 572) & 16) != 0;
        flags = r_u32(actor + 568);
        if (flags & 0x100)
        {
            if (!r_u16(body + 544) && !sub_8005BE58(r_u32(body + 540)))
            {
                uint32_t animation, descriptor, frame;
                w_u32(actor + 772, 0);
                w_u32(actor + 576, 0);
                w_u8(actor + 849, 0);
                animation = r_u32(body + 540);
                descriptor = r_u32(animation + 40);
                value = r_u32(animation + 12);
                first = r_u32(animation + 4) + 12 * value;
                first = r_u8(first);
                second = r_u32(descriptor + 4);
                descriptor = r_u32(second + 4 * first);
                frame = r_u32(animation + 16);
                value = (uint32_t)(int32_t)(int16_t)(r_u16(descriptor + 16) - 1);
                numerator = frame << 16;
                denominator = value << 16;
                numerator_positive = (int32_t)numerator > 0;
                denominator_positive = (int32_t)denominator > 0;
                phase = sub_80010028(numerator_positive ? numerator : 0 - numerator, denominator_positive ? denominator : 0 - denominator);
                if (numerator_positive != denominator_positive)
                    phase = 0 - phase;
                phase = (~(uint32_t)((int32_t)phase >> 31) & phase) + ((uint32_t)((int32_t)(65536 - phase) >> 31) & (65536 - phase));
                first = (uint32_t)sub_8001FC90((int32_t)phase, r_s32(actor + 668));
                value = r_u32(actor + 304);
                w_u32(actor + 652, first);
                second = (uint32_t)sub_8001FC90((int32_t)(65536 - phase), (int32_t)value / -2);
                w_u32(actor + 772, second);
                w_u32(actor + 568, r_u32(actor + 568) & 0xFFFFDFFF);
                goto ground_idle;
            }
            if (r_u32(body + 604) < 2)
                goto ground_idle;
            value = r_u32(actor + 304);
            limit = (uint32_t)((int32_t)value / -2);
            first = r_u32(actor + 772) + 6553;
            w_u32(actor + 772, first);
            if ((int32_t)first < (int32_t)limit)
                limit = first;
            slope = r_s16(actor + 524) < 3547;
            first = r_u32(actor + 732);
            w_u32(actor + 772, limit);
            second = r_u32(actor + 740);
            (void)sub_8002ECB8(xport_guest_cptr(actor + 528, 6), xport_guest_cptr(actor + 516, 6), motion, second, first);
            if (!r_u32(actor + 576) && slope)
            {
                sum = 0;
                for (axis = 0; axis < 3; ++axis)
                {
                    int32_t normal = r_s16(actor + 820 + 2 * axis) * 16;
                    value = (uint32_t)(((int64_t)(int32_t)motion[axis] * normal) >> 16);
                    sum += value;
                }
                if ((int32_t)sum > 0)
                    w_u32(actor + 576, 65536);
            }
            flags = r_u32(actor + 568);
            if (flags & 0x2000)
            {
                sum = 0;
                for (axis = 0; axis < 3; ++axis)
                {
                    int32_t normal = r_s16(actor + 820 + 2 * axis) * 16;
                    value = (uint32_t)(((int64_t)(int32_t)motion[axis] * normal) >> 16);
                    sum += value;
                }
                if ((int32_t)sum > 0)
                    w_u32(actor + 568, flags & 0xFFFFDFFF);
                if (r_u32(actor + 568) & 0x2000)
                {
                    w_u32(actor + 576, 0);
                    goto ground_idle;
                }
            }
            (void)sub_8002EE50(r_u32(actor + 480), xport_guest_cptr(actor + 450, 6), xport_guest_ptr(actor + 456, 12));
            elapsed = r_u32(actor + 724) - (uint32_t)delta;
            w_u32(actor + 724, elapsed);
            pull = r_u32(body + 604) == 4;
            if ((int32_t)elapsed < 0)
                elapsed = 0;
            w_u32(actor + 724, pull ? elapsed : 0);
            if (slope)
            {
                w_u32(actor + 724, 65536);
                w_u32(actor + 764, 0);
            }
            else
            {
                value = pull && r_u16(actor + 172) >= r_u32(r_u32(0x8005B2F8) + 48);
                w_u32(actor + 764, ((0 - value) & 0xFFFD8000) - 32768);
            }
            first = (uint32_t)sub_8001FC90(r_s32(actor + 724), delta);
            second = (uint32_t)sub_8001FC90(r_s32(actor + 764), delta);
            (void)sub_8002E6F8(motion, xport_guest_cptr(actor + 456, 12), motion, (int32_t)first, (int32_t)second);
            first = r_u32(actor + 456) + motion[0];
            w_u32(actor + 456, first);
            w_u32(actor + 460, r_u32(actor + 460) + motion[1]);
            w_u32(actor + 464, r_u32(actor + 464) + motion[2]);
            first = (uint32_t)(((int64_t)(int32_t)first * (int32_t)first) >> 16);
            value = r_u32(actor + 460);
            second = (uint32_t)(((int64_t)(int32_t)value * (int32_t)value) >> 16);
            value = r_u32(actor + 464);
            third = (uint32_t)(((int64_t)(int32_t)value * (int32_t)value) >> 16);
            old = r_u32(actor + 576);
            sum = third + second + first;
            w_u32(actor + 580, sum);
            speed = 4 * sub_8004CF74(sum);
            w_u32(actor + 480, speed);
            numerator = speed - old;
            w_u32(actor + 576, speed);
            numerator_positive = (int32_t)numerator > 0;
            denominator_positive = delta > 0;
            rate = sub_80010028(numerator_positive ? numerator : 0 - numerator, denominator_positive ? (uint32_t)delta : 0 - (uint32_t)delta);
            if (numerator_positive != denominator_positive)
                rate = 0 - rate;
            value = r_u32(actor + 480);
            w_u32(actor + 484, rate);
            if (value)
            {
                uint32_t magnitude = (int32_t)value < 0 ? 0 - value : value;
                uint32_t divisor = (uint32_t)((int32_t)magnitude >> 1) + (uint32_t)((int32_t)(magnitude - 2) >> 31);
                uint32_t reciprocal = divisor ? 0x80000000 / divisor : 0xFFFFFFFF;
                if ((int32_t)value < 0)
                    reciprocal = 0 - reciprocal;
                (void)sub_8002EED8(reciprocal, xport_guest_cptr(actor + 456, 12), xport_guest_ptr(actor + 450, 6));
            }
            first = (uint32_t)(int32_t)r_s16(actor + 522);
            second = (uint32_t)(int32_t)r_s16(actor + 524);
            xport_gte_write_control(0, first);
            third = (uint32_t)(int32_t)r_s16(actor + 526);
            xport_gte_write_control(2, second);
            xport_gte_write_control(4, third);
            first = (uint32_t)(int32_t)r_s16(actor + 450);
            second = (uint32_t)(int32_t)r_s16(actor + 452);
            third = (uint32_t)(int32_t)r_s16(actor + 454);
            xport_gte_write_data(9, first);
            xport_gte_write_data(10, second);
            xport_gte_write_data(11, third);
            xport_gte_execute(0x178000C);
            first = xport_gte_read_data(9);
            second = xport_gte_read_data(10);
            third = xport_gte_read_data(11);
            w_u16(actor + 814, (uint16_t)first);
            w_u16(actor + 816, (uint16_t)second);
            w_u16(actor + 818, (uint16_t)third);
            if (!sub_8002E468(xport_guest_ptr(actor + 814, 6)))
            {
                first = r_u16(actor + 432);
                second = r_u16(actor + 434);
                third = r_u16(actor + 436);
                w_u16(actor + 814, (uint16_t)first);
                w_u16(actor + 816, (uint16_t)second);
                w_u16(actor + 818, (uint16_t)third);
            }
            first = sub_8002E698(xport_guest_cptr(actor + 528, 6), xport_guest_cptr(actor + 814, 6));
            second = sub_8002E698(xport_guest_cptr(actor + 528, 6), xport_guest_cptr(actor + 450, 6));
            value = 25736 * sub_80020018(first, second);
            value = (uint32_t)((int32_t)value >> 8);
            w_u32(actor + 676, value);
            w_u32(actor + 688, value + 205887);
            goto ground_idle;
        }
        if (!(flags & 15) && r_s32(actor + 576) > 0)
        {
            first = (uint32_t)sub_8001FC90(r_s32(r_u32(actor + 556) + 324), r_s32(actor + 580));
            second = r_u32(0x800D38E0 + 4 * (uint32_t)(int32_t)r_s8(actor + 534));
            first = 0 - (uint32_t)sub_8001FC90((int32_t)second, (int32_t)first);
            flags = r_u32(actor + 568);
            w_u32(actor + 752, first);
            if (flags & 0x80)
            {
                int32_t time = r_s32(actor + 724);
                value = 10;
                if (time >= -196608)
                    value = time >= 0 ? 6 : (uint32_t)(6 - time / 49152);
                w_u32(actor + 752, r_u32(actor + 752) * value);
            }
            else if (flags & 16)
            {
                value = r_u32(actor + 676);
                mask = (uint32_t)((int32_t)value >> 31);
                value = 100 * ((value + mask) ^ mask) + 65536;
                w_u32(actor + 752, (uint32_t)sub_8001FC90((int32_t)value, (int32_t)first));
            }
            first = r_u32(actor + 760) + r_u32(actor + 752);
            w_u32(actor + 764, first);
            w_u32(actor + 484, first);
            second = (uint32_t)sub_8001FC90((int32_t)first, delta);
            value = r_u32(actor + 576) + second;
            w_u32(actor + 576, value);
            if ((int32_t)value < 146486)
                value = 146486;
            first = r_u32(actor + 388);
            w_u32(actor + 576, value);
            if ((first & 1) && r_s16(actor + 452) < -2048)
            {
                first = (uint32_t)sub_8001FC90(246375, r_s16(actor + 452) * 16) + 188723;
                value = (uint32_t)sub_80074FB4(r_s32(actor + 576), r_s32(r_u32(actor + 556) + 224) >> 1, (int32_t)first);
                w_u32(actor + 576, value);
            }
        }
        flags = r_u32(actor + 568);
        w_u32(actor + 564, r_u32(actor + 564) & 0xFFFFCFFF);
        if (flags & 0x80)
        {
            if (r_u32(actor + 856) && r_u32(actor + 1088))
                goto next_ground_actor;
            if (!(r_u32(actor + 560) & 0x04000000) && r_s32(actor + 576) > 327679)
            {
                value = r_u32(actor + 636);
                mask = (uint32_t)((int32_t)value >> 31);
                if (r_s32(actor + 724) <= 0 || (int32_t)((value + mask) ^ mask) <= 52428)
                {
                    speed = r_u32(actor + 576);
                    if ((int32_t)speed > 1114111)
                    {
                        w_u32(actor + 724, r_u32(actor + 724) - (uint32_t)delta);
                        goto next_ground_actor;
                    }
                    rate = (int32_t)speed > 327679 ? (uint32_t)sub_8001FC90(-3276, (int32_t)(speed - 1114112)) + 26214 : 65536;
                    elapsed = r_u32(actor + 724);
                    if ((int32_t)elapsed < 0)
                        w_u32(actor + 724, 0);
                    else
                    {
                        denominator = (uint32_t)((int32_t)rate >> 1) + (uint32_t)((int32_t)(rate - 2) >> 31);
                        limit = denominator ? 0x80000000 / denominator : 0xFFFFFFFF;
                        first = elapsed + (uint32_t)delta;
                        w_u32(actor + 724, first);
                        if ((int32_t)first >= (int32_t)(3 * limit))
                            w_u32(actor + 724, first - limit);
                    }
                    phase = (uint32_t)sub_8001FC90((int32_t)rate, r_s32(actor + 724));
                    first = (uint32_t)sub_8001FC90(65536, r_s16(0x8005624C + ((phase >> 2) & 0x3FFC)) * 16);
                    speed = r_u32(actor + 576);
                    rate = (int32_t)speed > 327679 ? (uint32_t)sub_8001FC90(-3822, (int32_t)(speed - 1114112)) + 6553 : 52428;
                    second = (uint32_t)sub_8001FC90((int32_t)rate, (int32_t)first);
                    elapsed = r_u32(actor + 724);
                    if ((int32_t)elapsed > 65535)
                    {
                        w_u32(actor + 488, first);
                        w_u32(actor + 636, second);
                    }
                    else
                    {
                        third = (uint32_t)sub_8001FC90((int32_t)(65536 - elapsed), r_s32(actor + 488));
                        third += (uint32_t)sub_8001FC90(r_s32(actor + 724), (int32_t)first);
                        value = r_u32(actor + 636);
                        elapsed = r_u32(actor + 724);
                        w_u32(actor + 488, third);
                        third = (uint32_t)sub_8001FC90((int32_t)(65536 - elapsed), (int32_t)value);
                        third += (uint32_t)sub_8001FC90(r_s32(actor + 724), (int32_t)second);
                        w_u32(actor + 636, third);
                    }
                    goto next_ground_actor;
                }
            }
            if (r_u32(body + 604) >= 2)
            {
                w_u32(actor + 560, r_u32(actor + 560) & 0xFBFFFFFF);
                w_u32(actor + 568, (r_u32(actor + 568) & 0xFFFFFF7F) | 0x200840);
            }
            goto next_ground_actor;
        }
        value = r_u32(actor + 660);
        if (value)
            w_u32(actor + 656, r_u32(actor + 656) + (uint32_t)sub_8001FC90((int32_t)value, delta));
        value = r_u32(actor + 656);
        if (value)
        {
            if (r_u32(actor + 660))
            {
                elapsed = r_u32(actor + 724) + (uint32_t)delta;
                w_u32(actor + 724, elapsed);
                first = (uint32_t)sub_8001FC90(r_s32(actor + 660), (int32_t)elapsed);
                second = (uint32_t)sub_8001FC90(r_s32(actor + 724), (int32_t)(r_u32(actor + 656) - (uint32_t)((int32_t)first / 2)));
                value = r_u32(actor + 664) + second;
            }
            else
                value = r_u32(actor + 652) + (uint32_t)sub_8001FC90((int32_t)value, delta);
            w_u32(actor + 652, value);
            value = r_u32(actor + 652);
            mask = (uint32_t)((int32_t)value >> 31);
            if ((int32_t)((value + mask) ^ mask) > 102942)
            {
                w_u32(actor + 652, (int32_t)value > 0 ? 102943 : (uint32_t)(0 - 102943));
                w_u32(actor + 660, 0);
                w_u32(actor + 656, 0);
            }
            w_u32(actor + 672, 0);
        }
        else
        {
            value = r_u32(actor + 640);
            if (value)
            {
                first = (uint32_t)sub_8001FC90((int32_t)value, delta);
                old = r_u32(actor + 636);
                limit = r_u32(actor + 644);
                value = old + first;
                mask = ((int32_t)value >= (int32_t)limit && (int32_t)limit < (int32_t)old) || ((int32_t)limit >= (int32_t)value && (int32_t)old < (int32_t)limit) ? 0xFFFFFFFF : 0;
                limit = r_u32(actor + 644);
                first = r_u32(actor + 640);
                w_u32(actor + 672, 0);
                w_u32(actor + 636, limit + (mask & (value - limit)));
                w_u32(actor + 640, mask & first);
            }
        }
        value = r_u32(actor + 584);
        if (value)
        {
            first = (uint32_t)sub_8001FC90((int32_t)value, delta);
            old = r_u32(actor + 488);
            value = old + first;
            if (((int32_t)value < 0 && (int32_t)old >= 0) || ((int32_t)value > 0 && (int32_t)old <= 0))
            {
                w_u32(actor + 584, 0);
                w_u32(actor + 488, 0);
            }
            else
                w_u32(actor + 488, value);
            limit = r_u32(r_u32(actor + 556) + 232);
            value = r_u32(actor + 488);
            first = (uint32_t)((int32_t)(value + limit) >> 31) & (0 - limit - value);
            second = (uint32_t)((int32_t)(limit - value) >> 31) & (limit - value);
            w_u32(actor + 488, value + first + second);
        }
        flags = r_u32(actor + 568);
        if (flags & 0x40)
        {
            value = r_u32(actor + 652);
            if ((flags & 0x18000) && (int32_t)(r_u32(actor + 656) ^ value) >= 0)
            {
                rate = (int32_t)value < 0 ? (uint32_t)(0 - 163840) : 163840;
                if (r_u32(actor + 684) != rate)
                {
                    first = r_u32(actor + 676);
                    w_u32(actor + 724, 0);
                    w_u32(actor + 688, first);
                }
                flags = r_u32(actor + 568);
                w_u32(actor + 684, rate);
                w_u32(actor + 568, flags | 0x20000);
            }
            else
                w_u32(actor + 684, 0);
            value = r_u32(actor + 652);
            mask = (uint32_t)((int32_t)value >> 31);
            value = (value + mask) ^ mask;
            if ((int32_t)value > 45752 || (r_u32(actor + 568) & 0x200000))
            {
                flags = r_u32(body + 552);
                if (r_u32(body + 604) < 2)
                    flags |= 0x8000;
                w_u32(body + 552, flags);
                if (passenger)
                {
                    first = r_u32(r_u32(actor + 856) + 852);
                    flags = r_u32(first + 552);
                    if (r_u32(first + 604) < 2)
                        flags |= 0x8000;
                    w_u32(first + 552, flags);
                }
            }
            if (value == 102943)
            {
                w_u32(actor + 568, (r_u32(actor + 568) & 0xFFDC7FBF) | 0x900);
                value = sub_80043F00(0xF2000002) & 255;
                (void)sub_80017BA0(r_s32(actor + 184), r_s32(actor + 192), ((5 * value) >> 8) + 50, 0);
            }
            goto next_ground_actor;
        }
        if (!(flags & 0x20))
            goto next_ground_actor;
        pull = r_u32(actor + 856) && r_u32(actor + 1088) && r_s32(actor + 676) > 0;
        if (pull)
        {
            first = r_u32(actor + 688);
            second = r_u32(actor + 676);
            numerator = first - second;
            if ((int32_t)numerator > 0)
                phase = sub_80010028(numerator, 22876) - 65536;
            else
                phase = 0 - 65536 - sub_80010028(second - first, 22876);
            w_u32(actor + 636, (uint32_t)sub_8001FC90((int32_t)phase, 34314));
            if (r_u32(actor + 676) != r_u32(actor + 688))
                goto next_ground_actor;
            goto ground_skid_start;
        }
        if (r_u32(actor + 636))
        {
            if ((r_u32(actor + 676) == r_u32(actor + 688) || !r_u32(actor + 576)) && !(r_u32(actor + 568) & 0x20000))
            {
                value = r_u32(actor + 636);
                w_u32(actor + 644, 0);
                w_u32(actor + 640, (int32_t)value > 0 ? (uint32_t)(0 - 294912) : 294912);
                value = r_u32(actor + 488);
                w_u32(actor + 584, (int32_t)value > 0 ? (uint32_t)(0 - 196608) : 196608);
                w_u32(actor + 568, r_u32(actor + 568) | 0x20000);
            }
            goto ground_skid_finish;
        }
    ground_skid_start:
        value = r_s32(actor + 676) > 0 ? 524288 : (uint32_t)(0 - 524288);
        first = r_u16(actor + 528);
        second = r_u16(actor + 530);
        third = (uint32_t)(int32_t)r_s16(actor + 452);
        w_u32(actor + 488, value);
        value = r_u16(actor + 532);
        w_u32(actor + 728, 0);
        w_u16(actor + 796, (uint16_t)first);
        w_u16(actor + 798, (uint16_t)second);
        w_u16(actor + 800, (uint16_t)value);
        value = sub_8001FF3C(third << 4);
        limit = r_u16(0x8005624E + 4 * ((1251 - value) & 4095));
        speed = r_u32(actor + 576);
        if ((int32_t)speed > 493551)
        {
            first = (0 - sub_80010028(493551, speed)) >> 4;
            if ((int32_t)(first << 16) < (int32_t)(limit << 16))
                first = limit;
            limit = first;
        }
        if (r_s16(actor + 452) != (int16_t)limit)
        {
            first = (uint32_t)(int32_t)r_s16(actor + 450) << 4;
            second = (uint32_t)(int32_t)r_s16(actor + 454) << 4;
            third = (uint32_t)(int32_t)(int16_t)limit << 4;
            w_u16(actor + 452, (uint16_t)limit);
            value = (uint32_t)sub_8001FC90((int32_t)first, (int32_t)first);
            sum = value + (uint32_t)sub_8001FC90((int32_t)second, (int32_t)second);
            if ((int32_t)sum < 6)
            {
                first = 0;
                second = 655;
                sum = 6;
            }
            value = 65536 - (uint32_t)sub_8001FC90((int32_t)third, (int32_t)third);
            numerator_positive = (int32_t)value > 0;
            denominator_positive = (int32_t)sum > 0;
            numerator = 65536 - (uint32_t)sub_8001FC90((int32_t)third, (int32_t)third);
            phase = sub_80010028(numerator_positive ? numerator : 0 - numerator, denominator_positive ? sum : 0 - sum);
            if (numerator_positive != denominator_positive)
                phase = 0 - phase;
            rate = 4 * sub_8004CF74(phase);
            value = (uint32_t)sub_8001FC90((int32_t)rate, (int32_t)first);
            w_u16(actor + 450, (uint16_t)((int32_t)value >> 4));
            value = (uint32_t)sub_8001FC90((int32_t)rate, (int32_t)second);
            w_u16(actor + 454, (uint16_t)((int32_t)value >> 4));
        }
        w_u32(actor + 568, (r_u32(actor + 568) & 0xFFFFFFDF) | 0x40A00);
    ground_skid_finish:
        if (!pull)
        {
            value = r_u32(actor + 636);
            if ((r_u32(actor + 568) & 0x200) || value <= 34313 || ((int32_t)value > -34314 && (int32_t)value < 0))
            {
                if (r_u32(body + 604) < 2)
                    w_u32(body + 552, r_u32(body + 552) | 0x8000);
                if (passenger)
                {
                    first = r_u32(r_u32(actor + 856) + 852);
                    if (r_u32(first + 604) < 2)
                        w_u32(first + 552, r_u32(first + 552) | 0x8000);
                }
            }
        }
        goto next_ground_actor;
    ground_idle:
        if (!r_u32(actor + 576))
        {
            value = r_u32(actor + 676);
            w_u32(actor + 680, 0);
            w_u32(actor + 724, 0);
            if (value)
            {
                first = r_u16(actor + 528);
                second = r_u16(actor + 530);
                third = r_u16(actor + 532);
                w_u16(actor + 450, (uint16_t)first);
                w_u16(actor + 452, (uint16_t)second);
                w_u16(actor + 454, (uint16_t)third);
                first = (uint32_t)(int32_t)r_s16(actor + 522);
                second = (uint32_t)(int32_t)r_s16(actor + 524);
                xport_gte_write_control(0, first);
                third = (uint32_t)(int32_t)r_s16(actor + 526);
                xport_gte_write_control(2, second);
                xport_gte_write_control(4, third);
                first = (uint32_t)(int32_t)r_s16(actor + 450);
                second = (uint32_t)(int32_t)r_s16(actor + 452);
                third = (uint32_t)(int32_t)r_s16(actor + 454);
                xport_gte_write_data(9, first);
                xport_gte_write_data(10, second);
                xport_gte_write_data(11, third);
                xport_gte_execute(0x178000C);
                first = xport_gte_read_data(9);
                second = xport_gte_read_data(10);
                third = xport_gte_read_data(11);
                w_u16(actor + 814, (uint16_t)first);
                w_u16(actor + 816, (uint16_t)second);
                w_u16(actor + 818, (uint16_t)third);
                if (!sub_8002E468(xport_guest_ptr(actor + 814, 6)))
                {
                    first = r_u16(actor + 432);
                    second = r_u16(actor + 434);
                    third = r_u16(actor + 436);
                    w_u16(actor + 814, (uint16_t)first);
                    w_u16(actor + 816, (uint16_t)second);
                    w_u16(actor + 818, (uint16_t)third);
                }
                w_u32(actor + 676, 0);
            }
        }
    next_ground_actor:
        node = r_u32(node + 4);
    }
    (void)sub_80078DB4(0x8005B350);
    (void)sub_80078DB4(0x8005B2D8);
    count_pointer = r_u32(0x800CE4DC);
    remaining = r_s32(count_pointer);
    actor = r_u32(0x800CE4D0);
    while (remaining >= 0)
    {
        uint32_t body, linked;
        if (r_u32(actor + 568) & 0x08001800)
            (void)sub_80071BCC(actor);
        body = r_u32(actor + 852);
        if (r_u32(body + 604) >= 2 && r_s16(body + 320))
            (void)sub_8008F404(body, delta);
        if (r_u8(r_u32(actor + 852) + 572) & 16)
        {
            linked = r_u32(r_u32(actor + 856) + 852);
            if (r_u32(linked + 604) >= 2 && r_s16(linked + 320))
                (void)sub_8008F404(linked, delta);
        }
        --remaining;
        actor += r_u32(0x800CE4D4);
    }
    (void)sub_8007AC04(0x8005B298);
    (void)sub_8007AC04(0x8005B350);

    node = 0x8005B378;
    for (;;)
    {
        uint32_t value, first, second, third, flags, mask, sum, speed, old_speed, drag;
        uint32_t roll_sine, roll_cosine, pitch_sine, pitch_cosine, yaw_sine, yaw_cosine;
        uint32_t mixed_sine, mixed_cosine, rotation[9], roll, yaw;
        uint32_t projection[3];
        uint16_t local_rotation[9];
        int crossed, orthogonal = 1;
        unsigned axis;
        node = r_u32(node + 4);
        if (node == 0x8005B378)
            break;
        actor = node - 1088;
        if (r_u32(actor + 568) & 0x80000)
        {
            value = r_u32(actor + 640);
            if (value)
            {
                uint32_t limit = 102943;
                first = (uint32_t)sub_8001FC90((int32_t)value, delta);
                second = r_u32(actor + 708) + first;
                value = r_u32(actor + 856);
                w_u32(actor + 708, second);
                if (value && (int32_t)second > 0)
                    limit = 183010;
                value = r_u32(actor + 708);
                first = (uint32_t)((int32_t)(value + limit) >> 31) & (0 - limit - value);
                second = (uint32_t)((int32_t)(limit - value) >> 31) & (limit - value);
                value += first + second;
                w_u32(actor + 708, value);
                mask = (uint32_t)((int32_t)value >> 31);
                if (((value + mask) ^ mask) == limit)
                    w_u32(actor + 640, 0);
            }
            value = sub_8002E698(xport_guest_cptr(actor + 808, 6), xport_guest_cptr(actor + 522, 6));
            flags = r_u32(actor + 568);
            crossed = (flags & 0x100000) ? (int32_t)value >= 0 : (int32_t)value <= 0;
            if (crossed && (int32_t)sub_8002E698(xport_guest_cptr(actor + 802, 6), xport_guest_cptr(actor + 522, 6)) < 0)
            {
                flags = r_u32(actor + 568);
                w_u32(actor + 568, (flags & 0x100000) ? flags & 0xFFEFFFFF : flags | 0x100000);
                crossed = 0;
            }
            if (!r_u32(actor + 488) || crossed)
            {
                w_u32(actor + 488, 0);
                value = sub_8002E698(xport_guest_cptr(actor + 796, 6), xport_guest_cptr(actor + 522, 6));
                mask = (uint32_t)((int32_t)value >> 31);
                if ((int32_t)((value + mask) ^ mask) < 6554)
                {
                    first = (uint32_t)(int32_t)r_s16(actor + 522);
                    second = (uint32_t)(int32_t)r_s16(actor + 524);
                    xport_gte_write_control(0, first);
                    third = (uint32_t)(int32_t)r_s16(actor + 526);
                    xport_gte_write_control(2, second);
                    xport_gte_write_control(4, third);
                    first = (uint32_t)(int32_t)r_s16(actor + 808);
                    second = (uint32_t)(int32_t)r_s16(actor + 810);
                    third = (uint32_t)(int32_t)r_s16(actor + 812);
                    xport_gte_write_data(9, first);
                    xport_gte_write_data(10, second);
                    xport_gte_write_data(11, third);
                    xport_gte_execute(0x178000C);
                    first = xport_gte_read_data(9);
                    second = xport_gte_read_data(10);
                    third = xport_gte_read_data(11);
                    w_u16(actor + 796, (uint16_t)first);
                    w_u16(actor + 798, (uint16_t)second);
                    w_u16(actor + 800, (uint16_t)third);
                }
                else
                {
                    (void)sub_8002EA20(xport_guest_cptr(actor + 796, 6), xport_guest_cptr(actor + 522, 6), (int32_t)(0 - value), projection);
                    w_u16(actor + 796, (uint16_t)((int32_t)projection[0] >> 4));
                    w_u16(actor + 798, (uint16_t)((int32_t)projection[1] >> 4));
                    orthogonal = 0;
                    w_u16(actor + 800, (uint16_t)((int32_t)projection[2] >> 4));
                }
                (void)sub_8002E468(xport_guest_ptr(actor + 796, 6));
                first = (uint32_t)(int32_t)r_s16(actor + 796);
                second = (uint32_t)(int32_t)r_s16(actor + 798);
                xport_gte_write_control(0, first);
                third = (uint32_t)(int32_t)r_s16(actor + 800);
                xport_gte_write_control(2, second);
                xport_gte_write_control(4, third);
                first = (uint32_t)(int32_t)r_s16(actor + 522);
                second = (uint32_t)(int32_t)r_s16(actor + 524);
                third = (uint32_t)(int32_t)r_s16(actor + 526);
                xport_gte_write_data(9, first);
                xport_gte_write_data(10, second);
                xport_gte_write_data(11, third);
                xport_gte_execute(0x178000C);
                first = xport_gte_read_data(9);
                second = xport_gte_read_data(10);
                third = xport_gte_read_data(11);
                w_u16(actor + 808, (uint16_t)first);
                w_u16(actor + 810, (uint16_t)second);
                w_u16(actor + 812, (uint16_t)third);
                if (orthogonal)
                {
                    first = r_u16(actor + 522);
                    second = r_u16(actor + 524);
                    third = r_u16(actor + 526);
                    w_u16(actor + 802, (uint16_t)first);
                    w_u16(actor + 804, (uint16_t)second);
                    w_u16(actor + 806, (uint16_t)third);
                }
                else
                {
                    (void)sub_8002E468(xport_guest_ptr(actor + 808, 6));
                    first = (uint32_t)(int32_t)r_s16(actor + 808);
                    second = (uint32_t)(int32_t)r_s16(actor + 810);
                    xport_gte_write_control(0, first);
                    third = (uint32_t)(int32_t)r_s16(actor + 812);
                    xport_gte_write_control(2, second);
                    xport_gte_write_control(4, third);
                    first = (uint32_t)(int32_t)r_s16(actor + 796);
                    second = (uint32_t)(int32_t)r_s16(actor + 798);
                    third = (uint32_t)(int32_t)r_s16(actor + 800);
                    xport_gte_write_data(9, first);
                    xport_gte_write_data(10, second);
                    xport_gte_write_data(11, third);
                    xport_gte_execute(0x178000C);
                    first = xport_gte_read_data(9);
                    second = xport_gte_read_data(10);
                    third = xport_gte_read_data(11);
                    w_u16(actor + 802, (uint16_t)first);
                    w_u16(actor + 804, (uint16_t)second);
                    w_u16(actor + 806, (uint16_t)third);
                }
            }
            if (!r_u32(actor + 488) && !r_u32(actor + 640) && orthogonal)
                w_u32(actor + 568, r_u32(actor + 568) | 0x200000);
            drag = 0;
        }
        else
        {
            value = r_u32(actor + 728);
            if (value)
            {
                uint32_t step = (uint32_t)delta;
                first = r_u32(actor + 724) + (uint32_t)delta;
                w_u32(actor + 724, first);
                if ((int32_t)value < (int32_t)first)
                {
                    step -= first - value;
                    w_u32(actor + 728, 0);
                }
                value = r_u32(actor + 656);
                if (value)
                    w_u32(actor + 692, r_u32(actor + 692) + (uint32_t)sub_8001FC90((int32_t)value, (int32_t)step));
                value = r_u32(actor + 640);
                if (value)
                    w_u32(actor + 708, r_u32(actor + 708) + (uint32_t)sub_8001FC90((int32_t)value, (int32_t)step));
            }
            w_u32(actor + 488, (uint32_t)sub_8001FC90(65208, r_s32(actor + 488)));
            drag = (uint32_t)sub_8001FC90(r_s32(0x800D3964), delta);
        }
        value = r_u32(actor + 488);
        if (value)
        {
            first = (uint32_t)sub_8001FC90((int32_t)value, delta);
            second = r_u32(actor + 616) + first;
            w_u32(actor + 616, second);
            if ((int32_t)second > 411773)
                w_u32(actor + 616, second - 411774);
            else if ((int32_t)second <= -411774)
                w_u32(actor + 616, second + 411774);
            value = ((163 * r_u32(actor + 616)) >> 14) & 4095;
            first = (uint32_t)(int32_t)r_s16(0x8005624E + 4 * value) << 4;
            second = (uint32_t)(int32_t)r_s16(0x8005624C + 4 * value) << 4;
            (void)sub_8002EB78(xport_guest_cptr(actor + 620, 6), xport_guest_cptr(actor + 628, 6), xport_guest_ptr(actor + 808, 6), first, second);
            (void)sub_8002EB78(xport_guest_cptr(actor + 620, 6), xport_guest_cptr(actor + 628, 6), xport_guest_ptr(actor + 802, 6), 0 - second, first);
        }
        pitch_sine = (uint32_t)(int32_t)r_s16(0x8005624C) << 4;
        roll = ((163 * r_u32(actor + 708)) >> 14) & 4095;
        yaw = ((163 * r_u32(actor + 692)) >> 14) & 4095;
        roll_sine = (uint32_t)(int32_t)r_s16(0x8005624C + 4 * roll) << 4;
        pitch_cosine = (uint32_t)(int32_t)r_s16(0x8005624E) << 4;
        yaw_sine = (uint32_t)(int32_t)r_s16(0x8005624C + 4 * yaw) << 4;
        yaw_cosine = (uint32_t)(int32_t)r_s16(0x8005624E + 4 * yaw) << 4;
        roll_cosine = (uint32_t)(int32_t)r_s16(0x8005624E + 4 * roll) << 4;
        mixed_sine = (uint32_t)sub_8001FC90((int32_t)roll_sine, (int32_t)pitch_sine);
        mixed_cosine = (uint32_t)sub_8001FC90((int32_t)pitch_sine, (int32_t)roll_cosine);
        first = (uint32_t)sub_8001FC90((int32_t)roll_cosine, (int32_t)yaw_cosine);
        rotation[0] = first + (uint32_t)sub_8001FC90((int32_t)mixed_sine, (int32_t)yaw_sine);
        rotation[1] = (uint32_t)sub_8001FC90((int32_t)roll_sine, (int32_t)pitch_cosine);
        first = (uint32_t)sub_8001FC90((int32_t)roll_cosine, (int32_t)yaw_sine);
        rotation[2] = (uint32_t)sub_8001FC90((int32_t)mixed_sine, (int32_t)yaw_cosine) - first;
        first = (uint32_t)sub_8001FC90((int32_t)roll_sine, (int32_t)yaw_cosine);
        rotation[3] = (uint32_t)sub_8001FC90((int32_t)mixed_cosine, (int32_t)yaw_sine) - first;
        rotation[4] = (uint32_t)sub_8001FC90((int32_t)roll_cosine, (int32_t)pitch_cosine);
        first = (uint32_t)sub_8001FC90((int32_t)roll_sine, (int32_t)yaw_sine);
        rotation[5] = first + (uint32_t)sub_8001FC90((int32_t)mixed_cosine, (int32_t)yaw_cosine);
        rotation[6] = (uint32_t)sub_8001FC90((int32_t)pitch_cosine, (int32_t)yaw_sine);
        rotation[7] = 0 - pitch_sine;
        rotation[8] = (uint32_t)sub_8001FC90((int32_t)pitch_cosine, (int32_t)yaw_cosine);
        for (axis = 0; axis < 9; ++axis)
            local_rotation[axis] = (uint16_t)((int32_t)rotation[axis] >> 4);
        first = (uint32_t)local_rotation[0] | ((uint32_t)local_rotation[1] << 16);
        second = (uint32_t)local_rotation[2] | ((uint32_t)local_rotation[3] << 16);
        xport_gte_write_control(0, first);
        xport_gte_write_control(1, second);
        first = (uint32_t)local_rotation[4] | ((uint32_t)local_rotation[5] << 16);
        second = (uint32_t)local_rotation[6] | ((uint32_t)local_rotation[7] << 16);
        third = local_rotation[8];
        xport_gte_write_control(2, first);
        xport_gte_write_control(3, second);
        xport_gte_write_control(4, third);
        for (axis = 0; axis < 3; ++axis)
        {
            first = r_u16(actor + 796 + 2 * axis);
            second = r_u16(actor + 802 + 2 * axis);
            third = r_u16(actor + 808 + 2 * axis);
            xport_gte_write_data(9, first);
            xport_gte_write_data(10, second);
            xport_gte_write_data(11, third);
            xport_gte_execute(0x49E012);
            first = xport_gte_read_data(9);
            second = xport_gte_read_data(10);
            third = xport_gte_read_data(11);
            w_u16(actor + 432 + 2 * axis, (uint16_t)first);
            w_u16(actor + 438 + 2 * axis, (uint16_t)second);
            w_u16(actor + 444 + 2 * axis, (uint16_t)third);
        }
        if (!(r_u32(actor + 564) & 0x100000))
        {
            first = (uint32_t)sub_8001FC90((int32_t)drag, r_s32(actor + 456));
            second = r_u32(actor + 456);
            third = r_u32(actor + 460);
            w_u32(actor + 456, second - first);
            first = (uint32_t)sub_8001FC90((int32_t)drag, (int32_t)third);
            first -= (uint32_t)sub_8001FC90(643207, delta);
            second = r_u32(actor + 460);
            third = r_u32(actor + 464);
            w_u32(actor + 460, second - first);
            first = (uint32_t)sub_8001FC90((int32_t)drag, (int32_t)third);
            w_u32(actor + 464, r_u32(actor + 464) - first);
        }
        value = r_u32(actor + 456);
        first = (uint32_t)(((int64_t)(int32_t)value * (int32_t)value) >> 16);
        value = r_u32(actor + 460);
        second = (uint32_t)(((int64_t)(int32_t)value * (int32_t)value) >> 16);
        value = r_u32(actor + 464);
        third = (uint32_t)(((int64_t)(int32_t)value * (int32_t)value) >> 16);
        old_speed = r_u32(actor + 480);
        sum = third + second + first;
        w_u32(actor + 580, sum);
        speed = 4 * sub_8004CF74(sum);
        w_u32(actor + 480, speed);
        w_u32(actor + 576, speed);
        if (speed)
        {
            uint32_t magnitude = (int32_t)speed < 0 ? 0 - speed : speed;
            uint32_t divisor = (uint32_t)((int32_t)magnitude >> 1) + (uint32_t)((int32_t)(magnitude - 2) >> 31);
            uint32_t reciprocal = divisor ? 0x80000000 / divisor : 0xFFFFFFFF;
            if ((int32_t)speed < 0)
                reciprocal = 0 - reciprocal;
            (void)sub_8002EED8(reciprocal, xport_guest_cptr(actor + 456, 12), xport_guest_ptr(actor + 450, 6));
        }
        if ((int32_t)old_speed > 585943 && r_s32(actor + 480) <= 585943)
        {
            w_u32(actor + 480, 585944);
            (void)sub_8002EE50(585944, xport_guest_cptr(actor + 450, 6), xport_guest_ptr(actor + 456, 12));
        }
        w_u32(actor + 756, r_u32(actor + 756) + (uint32_t)delta);
        (void)sub_8007EA64(actor);
    }
    result = 0x80060000;
    node = r_u32(0x8005B274);
    while (node != 0x8005B270)
    {
        result = sub_80095724(node - 1088, (uint32_t)delta);
        node = r_u32(node + 4);
    }
    return result;
}
