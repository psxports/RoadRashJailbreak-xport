#include "native_game_api.h"
#include "psx.h"
#include "race_bodyless_batch_003.h"
#include <stddef.h>

static uint32_t bodyless_call(RRJMemory *m, RRJRaceLeafCall call, uint32_t target, uint32_t a0, uint32_t a1)
{
    const uint32_t args[8] = {a0, a1, 0, 0, 0, 0, 0, 0};
    return call(m, target, args);
}

uint32_t sub_8002490C(uint32_t index, uint32_t enabled, uint32_t incoming_v0)
{
    FUNCTION_MARKER(0x8002490Cu, "SLUS_010.53");
    uint32_t count, address, value;
    if (rrj_s32(index) < 0)
        return incoming_v0;
    count = rrj_read32(0x80053578);
    if (index >= count)
        return 0;
    address = 0x80053578 + (((index << 1) + index) << 2);
    value = rrj_read32(address + 16);
    if (enabled)
        value |= 1;
    else
        value &= 0xfffffffe;
    rrj_write32(address + 16, value);
    return address;
}

uint32_t sub_800148DC(uint32_t first, uint32_t second)
{
    FUNCTION_MARKER(0x800148DCu, "SLUS_010.53");
    uint32_t value = (uint32_t)sub_8001500C(second);
    return (uint32_t)sub_80015C88(first, value);
}

uint32_t sub_8001500C(uint32_t index)
{
    FUNCTION_MARKER(0x8001500Cu, "SLUS_010.53");
    uint32_t address = 0x800d65d0 + (((index << 2) + index) << 2);
    uint32_t value, base;
    if (!(rrj_read32(address) & 1))
        return 0xffffffff;
    value = rrj_read32(address + 4);
    base = rrj_read32(0x8005b3d8);
    return value + base;
}

uint32_t sub_80046768(uint32_t position)
{
    FUNCTION_MARKER(0x80046768u, "SLUS_010.53");
    uint32_t minute = r_u8(position);
    uint32_t second = r_u8(position + 1), frames;
    minute = (minute >> 4) * 10 + (minute & 15);
    second = (second >> 4) * 10 + (second & 15);
    frames = (minute * 60 + second) * 75;
    second = r_u8(position + 2);
    return frames + (second >> 4) * 10 + (second & 15) - 150;
}

uint32_t sub_8004DE34(uint32_t address)
{
    FUNCTION_MARKER(0x8004DE34u, "SLUS_010.53");
    return r_u16(address);
}

uint32_t sub_80050F28(const uint32_t incoming_args[8], RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x80050F28u, "SLUS_010.53");
    uint32_t args[8], i, size = incoming_args[1];
    if (size > 0x7eff0)
        size = 0x7eff0;
    for (i = 0; i < 8; ++i)
        args[i] = incoming_args[i];
    args[1] = size;
    (void)call(rrj_host_context(), 0x8004ef7c, args);
    if (!rrj_read32(0x8005a454))
        rrj_write32(0x8005a450, 0);
    return size;
}

uint32_t sub_80015530(uint32_t runtime_gp, RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x80015530u, "SLUS_010.53");
    uint32_t args[8] = {0};
    (void)bodyless_call(rrj_host_context(), call, 0x800457fc, 0x80015860, 0);
    (void)bodyless_call(rrj_host_context(), call, 0x800457e8, 0, 0);
    args[0] = 14;
    args[1] = runtime_gp + 0x73c;
    args[2] = 0;
    return call(rrj_host_context(), 0x80045a80, args);
}

uint32_t sub_8001EFE8(uint32_t enabled, uint32_t incoming_sp, RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x8001EFE8u, "SLUS_010.53");
    uint32_t parameters = incoming_sp - 48 + 16;
    if (enabled)
    {
        (void)bodyless_call(rrj_host_context(), call, 0x800505a8, 1, 0);
        (void)bodyless_call(rrj_host_context(), call, 0x8004fa18, 1, 0);
        rrj_write32(parameters, 1);
        rrj_write32(parameters + 4, 3);
        (void)bodyless_call(rrj_host_context(), call, 0x8004fbf8, parameters, 0);
        return bodyless_call(rrj_host_context(), call, 0x80050678, 1, 0x00ffffff);
    }
    (void)bodyless_call(rrj_host_context(), call, 0x800505a8, 0, 0);
    return bodyless_call(rrj_host_context(), call, 0x8004fa18, 0, 0);
}

uint32_t sub_8001ED28(uint32_t context, uint32_t length, uint32_t slot, uint32_t callback, uint32_t token, uint32_t mode, RRJRaceLeafCall call)
{
    uint32_t table = rrj_read32(0x800d6874), index = slot, value, limit;
    uint32_t args[8] = {0};
    FUNCTION_MARKER(0x8001ED28, "SLUS_010.53");
    if (rrj_s32(slot) < 0)
    {
        index = 0;
        value = rrj_read32(table);
        table += 4;
        if (value)
        {
            limit = rrj_read32(0x800d6870);
            do
            {
                ++index;
                if (rrj_s32(limit) < rrj_s32(index))
                    return 0xffffffff;
                value = rrj_read32(table);
                table += 4;
            } while (value);
        }
    }
    table = rrj_read32(0x800d6874);
    if (rrj_read32(table + (index << 2)))
        return 0xfffffffe;
    args[0] = context;
    args[1] = length;
    args[2] = callback;
    args[4] = mode;
    args[3] = token;
    value = (uint32_t)sub_8001E938(args[0], args[1], args[2], args[3], args[4], call);
    if (rrj_s32(value) <= 0)
        return 0xfffffffd;
    rrj_write32(context + 8, value);
    (void)(uint32_t)sub_8001EAA4(context, value);
    table = rrj_read32(0x800d6874);
    rrj_write32(table + (index << 2), context);
    return index;
}

uint32_t sub_8001E938(uint32_t context, uint32_t length, uint32_t callback, uint32_t argument, uint32_t mode, RRJRaceLeafCall call)
{
    uint32_t parameters[6];
    uint32_t source = rrj_read32(context + 12);
    uint32_t result;
    uint32_t remaining = 0xfffff, prior;
    FUNCTION_MARKER(0x8001E938, "SLUS_010.53");
    result = bodyless_call(rrj_host_context(), call, 0x8004f3c8, source, length);
    if (rrj_s32(result) < 0)
        return 0xffffffff;
    parameters[0] = result;
    parameters[1] = source;
    parameters[2] = length;
    /* Callback-free requests retain the semantic token, not guest stack residue */
    parameters[4] = argument;
    parameters[5] = 1;
    if (callback)
    {
        parameters[3] = callback;
    }
    else if (mode)
    {
        parameters[3] = 0x8001e928;
        rrj_write32(0x8005b4a4, 0);
    }
    else
    {
        parameters[3] = 0;
        rrj_write32(0x8005b4a4, 1);
    }
    (void)sub_8001E22C_values(parameters, mode, call);
    if (callback || rrj_read32(0x8005b4a4))
        return result;
    do
    {
        prior = remaining;
        --remaining;
        if (rrj_s32(prior) <= 0)
            return 0;
    } while (!rrj_read32(0x8005b4a4));
    return result;
}

static uint32_t cd_parameter_value(RRJMemory *m, uint32_t guest, const uint32_t values[6], uint32_t index)
{
    return values ? values[index] : rrj_read32(guest + 4 * index);
}

static uint32_t cd_start_request(RRJMemory *m, uint32_t guest, const uint32_t values[6], RRJRaceLeafCall call)
{
    uint32_t handle = cd_parameter_value(m, guest, values, 0);
    uint32_t mode, destination, size;

    rrj_write32(0x8005b490, 1);
    (void)bodyless_call(m, call, 0x80050f88, handle, 0);
    mode = cd_parameter_value(m, guest, values, 5);
    destination = cd_parameter_value(m, guest, values, 2);
    size = cd_parameter_value(m, guest, values, 1);
    return bodyless_call(m, call, mode == 1 ? 0x80050f28 : 0x80050ec8, destination, size);
}

uint32_t sub_8001E418_values(const uint32_t parameters[6], RRJRaceLeafCall call)
{
    return cd_start_request(rrj_host_context(), 0, parameters, call);
}

uint32_t sub_8001E418(uint32_t parameters, RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x8001E418, "SLUS_010.53");
    return cd_start_request(rrj_host_context(), parameters, NULL, call);
}

static uint32_t cd_enqueue_request(RRJMemory *m, uint32_t guest, const uint32_t values[6], uint32_t locked, RRJRaceLeafCall call)
{
    uint32_t index, address, value, consumer, result, i;

    if (cd_parameter_value(m, guest, values, 1) < 16)
        return 1;
    if (locked)
        (void)bodyless_call(m, call, 0x80043da4, guest, locked);
    index = rrj_read32(0x8005b480);
    value = cd_parameter_value(m, guest, values, 0);
    address = 0x800d7488 + (((index << 1) + index) << 3);
    rrj_write32(address, value);
    value = cd_parameter_value(m, guest, values, 1);
    consumer = rrj_read32(0x8005b494);
    rrj_write32(address + 4, value);
    for (i = 2; i != 6; ++i)
    {
        value = cd_parameter_value(m, guest, values, i);
        rrj_write32(address + 4 * i, value);
    }
    value = index + 1;
    rrj_write32(0x8005b480, value);
    result = rrj_s32(value) < 8;
    if (!result)
        rrj_write32(0x8005b480, 0);
    if (index == consumer)
    {
        if (values)
            result = sub_8001E418_values(values, call);
        else
            result = bodyless_call(m, call, 0x8001e418, guest, index);
    }
    if (locked)
        result = bodyless_call(m, call, 0x80043db4, 0, 0);
    return result;
}

uint32_t sub_8001E22C_values(const uint32_t parameters[6], uint32_t locked, RRJRaceLeafCall call)
{
    return cd_enqueue_request(rrj_host_context(), 0, parameters, locked, call);
}

uint32_t sub_8001E22C(uint32_t parameters, uint32_t locked, RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x8001E22C, "SLUS_010.53");
    return cd_enqueue_request(rrj_host_context(), parameters, NULL, locked, call);
}

uint32_t sub_8001EAA4(uint32_t data, uint32_t base)
{
    FUNCTION_MARKER(0x8001EAA4u, "SLUS_010.53");
    uint32_t outer = 0, entry = data + 16, offset, block, field, inner, count, value;
    if (!r_u8(data + 4))
        return 0;
    do
    {
        offset = rrj_read32(entry);
        if (offset)
        {
            block = data + offset;
            if (r_u8(block))
            {
                inner = 0;
                field = block + 12;
                do
                {
                    value = rrj_read32(field);
                    ++inner;
                    rrj_write32(field, value + base);
                    count = r_u8(block);
                    field += 12;
                } while (rrj_s32(inner) < rrj_s32(count));
            }
        }
        count = r_u8(data + 4);
        ++outer;
        entry += 4;
    } while (rrj_s32(outer) < rrj_s32(count));
    return 0;
}

uint32_t sub_8001E328(RRJRaceLeafCall call)
{
    uint32_t result = rrj_read32(0x8005B498u);
    uint32_t consumer;
    uint32_t producer;
    uint32_t record;
    uint32_t parameters[6];
    uint32_t index;

    FUNCTION_MARKER(0x8001E328, "SLUS_010.53");
    if (result)
        return result;
    consumer = rrj_read32(0x8005B494u);
    producer = rrj_read32(0x8005B480u);
    if (consumer == producer)
        return 0x800D0000u;
    record = 0x800D7488u + 24 * consumer;
    for (index = 0; index < 6; ++index)
        parameters[index] = rrj_read32(record + 4 * index);
    rrj_write32(0x8005B498u, 1);
    if (parameters[3])
        (void)bodyless_call(rrj_host_context(), call, parameters[3], parameters[4], 0);
    consumer = rrj_read32(0x8005B494u) + 1;
    rrj_write32(0x8005B494u, consumer);
    if (rrj_s32(consumer) >= 8)
        rrj_write32(0x8005B494u, 0);
    result = rrj_read32(0x8005B480u);
    consumer = rrj_read32(0x8005B494u);
    if (result == consumer)
        rrj_write32(0x8005B490u, 0);
    else
    {
        record = 0x800D7488u + 24 * consumer;
        result = sub_8001E418(record, call);
    }
    rrj_write32(0x8005B498u, 0);
    return result;
}

uint32_t sub_8002CA64(uint32_t source, uint32_t x, uint32_t y, uint32_t runtime_gp, uint32_t incoming_sp, RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x8002CA64u, "SLUS_010.53");
    uint32_t parameters = incoming_sp - 40 + 16, index, address, packed;
    if (r_u16(runtime_gp + 0x8c0) >= 4)
        w_u16(runtime_gp + 0x8c0, 0);
    w_u16(parameters + 4, 16);
    index = r_u16(runtime_gp + 0x8c0);
    w_u16(parameters + 2, (uint16_t)y);
    w_u16(parameters + 6, 1);
    address = x + (index << 4);
    w_u16(runtime_gp + 0x8c0, (uint16_t)(index + 1));
    w_u16(parameters, (uint16_t)address);
    (void)bodyless_call(rrj_host_context(), call, 0x80048a6c, parameters, source);
    packed = (y << 6) | ((address >> 4) & 63);
    w_u16(runtime_gp + 0x8ac + (index << 1), (uint16_t)packed);
    return index;
}

uint32_t sub_8002C9A8(uint32_t incoming_v0)
{
    FUNCTION_MARKER(0x8002C9A8u, "SLUS_010.53");
    uint32_t address = rrj_read32(0x8005ae78), result = incoming_v0;
    if (address)
    {
        result = (uint32_t)sub_800144B8(address);
        rrj_write32(0x8005ae78, 0);
    }
    rrj_write32(0x8005b544, 0);
    return result;
}
