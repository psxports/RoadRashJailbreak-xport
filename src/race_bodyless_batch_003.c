#include "race_bodyless_batch_003.h"

static uint32_t bodyless_call(RRJMemory *m, RRJRaceLeafCall call, uint32_t target, uint32_t a0, uint32_t a1)
{
    const uint32_t args[8] = {a0, a1, 0, 0, 0, 0, 0, 0};
    return call(m, target, args);
}

uint32_t sub_8002490C(RRJMemory *m, uint32_t index, uint32_t enabled, uint32_t incoming_v0)
{
    uint32_t count, address, value;
    if (rrj_s32(index) < 0)
        return incoming_v0;
    count = rrj_read32(m, 0x80053578);
    if (index >= count)
        return 0;
    address = 0x80053578 + (((index << 1) + index) << 2);
    value = rrj_read32(m, address + 16);
    if (enabled)
        value |= 1;
    else
        value &= 0xfffffffe;
    rrj_write32(m, address + 16, value);
    return address;
}

uint32_t sub_800148DC(RRJMemory *m, uint32_t first, uint32_t second, RRJRaceLeafCall call)
{
    uint32_t value = bodyless_call(m, call, 0x8001500c, second, second);
    return bodyless_call(m, call, 0x80015c88, first, value);
}

uint32_t sub_8001500C(RRJMemory *m, uint32_t index)
{
    uint32_t address = 0x800d65d0 + (((index << 2) + index) << 2);
    uint32_t value, base;
    if (!(rrj_read32(m, address) & 1))
        return 0xffffffff;
    value = rrj_read32(m, address + 4);
    base = rrj_read32(m, 0x8005b3d8);
    return value + base;
}

uint32_t sub_80046768(RRJMemory *m, uint32_t position)
{
    uint32_t minute = r_u8(position);
    uint32_t second = r_u8(position + 1), frames;
    minute = (minute >> 4) * 10 + (minute & 15);
    second = (second >> 4) * 10 + (second & 15);
    frames = (minute * 60 + second) * 75;
    second = r_u8(position + 2);
    return frames + (second >> 4) * 10 + (second & 15) - 150;
}

uint32_t sub_8004DE34(RRJMemory *m, uint32_t address)
{
    return r_u16(address);
}

uint32_t sub_80050F28(RRJMemory *m, const uint32_t incoming_args[8], RRJRaceLeafCall call)
{
    uint32_t args[8], i, size = incoming_args[1];
    if (size > 0x7eff0)
        size = 0x7eff0;
    for (i = 0; i < 8; ++i)
        args[i] = incoming_args[i];
    args[1] = size;
    (void)call(m, 0x8004ef7c, args);
    if (!rrj_read32(m, 0x8005a454))
        rrj_write32(m, 0x8005a450, 0);
    return size;
}

uint32_t sub_80015530(RRJMemory *m, uint32_t runtime_gp, RRJRaceLeafCall call)
{
    uint32_t args[8] = {0};
    (void)bodyless_call(m, call, 0x800457fc, 0x80015860, 0);
    (void)bodyless_call(m, call, 0x800457e8, 0, 0);
    args[0] = 14;
    args[1] = runtime_gp + 0x73c;
    args[2] = 0;
    return call(m, 0x80045a80, args);
}

uint32_t sub_8001EFE8(RRJMemory *m, uint32_t enabled, uint32_t incoming_sp, RRJRaceLeafCall call)
{
    uint32_t parameters = incoming_sp - 48 + 16;
    if (enabled)
    {
        (void)bodyless_call(m, call, 0x800505a8, 1, 0);
        (void)bodyless_call(m, call, 0x8004fa18, 1, 0);
        rrj_write32(m, parameters, 1);
        rrj_write32(m, parameters + 4, 3);
        (void)bodyless_call(m, call, 0x8004fbf8, parameters, 0);
        return bodyless_call(m, call, 0x80050678, 1, 0x00ffffff);
    }
    (void)bodyless_call(m, call, 0x800505a8, 0, 0);
    return bodyless_call(m, call, 0x8004fa18, 0, 0);
}

uint32_t sub_8001ED28(RRJMemory *m, uint32_t context, uint32_t length, uint32_t slot, uint32_t callback, uint32_t incoming_sp, RRJRaceLeafCall call)
{
    uint32_t table = rrj_read32(m, 0x800d6874), index = slot, value, limit;
    uint32_t frame = incoming_sp - 48, args[8] = {0};
    if (rrj_s32(slot) < 0)
    {
        index = 0;
        value = rrj_read32(m, table);
        table += 4;
        if (value)
        {
            limit = rrj_read32(m, 0x800d6870);
            do
            {
                ++index;
                if (rrj_s32(limit) < rrj_s32(index))
                    return 0xffffffff;
                value = rrj_read32(m, table);
                table += 4;
            } while (value);
        }
    }
    table = rrj_read32(m, 0x800d6874);
    if (rrj_read32(m, table + (index << 2)))
        return 0xfffffffe;
    args[0] = context;
    args[1] = length;
    args[2] = callback;
    args[4] = rrj_read32(m, frame + 68);
    args[3] = rrj_read32(m, frame + 64);
    rrj_write32(m, frame + 16, args[4]);
    value = call(m, 0x8001e938, args);
    if (rrj_s32(value) <= 0)
        return 0xfffffffd;
    rrj_write32(m, context + 8, value);
    (void)bodyless_call(m, call, 0x8001eaa4, context, value);
    table = rrj_read32(m, 0x800d6874);
    rrj_write32(m, table + (index << 2), context);
    return index;
}

uint32_t sub_8001E938(RRJMemory *m, uint32_t context, uint32_t length, uint32_t callback, uint32_t argument, uint32_t incoming_sp, RRJRaceLeafCall call)
{
    uint32_t frame = incoming_sp - 80, parameters = frame + 16;
    uint32_t source = rrj_read32(m, context + 12);
    uint32_t mode = rrj_read32(m, frame + 96);
    uint32_t result = bodyless_call(m, call, 0x8004f3c8, source, length);
    uint32_t remaining = 0xfffff, prior;
    if (rrj_s32(result) < 0)
        return 0xffffffff;
    rrj_write32(m, parameters, result);
    rrj_write32(m, parameters + 4, source);
    rrj_write32(m, parameters + 8, length);
    rrj_write32(m, parameters + 20, 1);
    if (callback)
    {
        rrj_write32(m, parameters + 12, callback);
        rrj_write32(m, parameters + 16, argument);
    }
    else if (mode)
    {
        rrj_write32(m, parameters + 12, 0x8001e928);
        rrj_write32(m, 0x8005b4a4, 0);
    }
    else
    {
        rrj_write32(m, parameters + 12, 0);
        rrj_write32(m, 0x8005b4a4, 1);
    }
    (void)bodyless_call(m, call, 0x8001e22c, parameters, mode);
    if (callback || rrj_read32(m, 0x8005b4a4))
        return result;
    do
    {
        prior = remaining;
        --remaining;
        if (rrj_s32(prior) <= 0)
            return 0;
    } while (!rrj_read32(m, 0x8005b4a4));
    return result;
}

uint32_t sub_8001E22C(RRJMemory *m, uint32_t parameters, uint32_t locked, RRJRaceLeafCall call)
{
    uint32_t index, address, value, consumer, result, i;
    if (rrj_read32(m, parameters + 4) < 16)
        return 1;
    if (locked)
        (void)bodyless_call(m, call, 0x80043da4, parameters, locked);
    index = rrj_read32(m, 0x8005b480);
    value = rrj_read32(m, parameters);
    address = 0x800d7488 + (((index << 1) + index) << 3);
    rrj_write32(m, address, value);
    value = rrj_read32(m, parameters + 4);
    consumer = rrj_read32(m, 0x8005b494);
    rrj_write32(m, address + 4, value);
    for (i = 8; i <= 20; i += 4)
    {
        value = rrj_read32(m, parameters + i);
        rrj_write32(m, address + i, value);
    }
    value = index + 1;
    rrj_write32(m, 0x8005b480, value);
    result = rrj_s32(value) < 8;
    if (!result)
        rrj_write32(m, 0x8005b480, 0);
    if (index == consumer)
        result = bodyless_call(m, call, 0x8001e418, parameters, index);
    if (locked)
        result = bodyless_call(m, call, 0x80043db4, 0, 0);
    return result;
}

uint32_t sub_8001E418(RRJMemory *m, uint32_t parameters, RRJRaceLeafCall call)
{
    uint32_t handle = rrj_read32(m, parameters), mode, destination, size;
    rrj_write32(m, 0x8005b490, 1);
    (void)bodyless_call(m, call, 0x80050f88, handle, 0);
    mode = rrj_read32(m, parameters + 20);
    destination = rrj_read32(m, parameters + 8);
    size = rrj_read32(m, parameters + 4);
    return bodyless_call(m, call, mode == 1 ? 0x80050f28 : 0x80050ec8, destination, size);
}

uint32_t sub_8001EAA4(RRJMemory *m, uint32_t data, uint32_t base)
{
    uint32_t outer = 0, entry = data + 16, offset, block, field, inner, count, value;
    if (!r_u8(data + 4))
        return 0;
    do
    {
        offset = rrj_read32(m, entry);
        if (offset)
        {
            block = data + offset;
            if (r_u8(block))
            {
                inner = 0;
                field = block + 12;
                do
                {
                    value = rrj_read32(m, field);
                    ++inner;
                    rrj_write32(m, field, value + base);
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

uint32_t sub_8001E328(RRJMemory *m, uint32_t incoming_sp, RRJRaceLeafCall call)
{
    uint32_t result = rrj_read32(m, 0x8005b498), consumer, producer, record;
    uint32_t parameters = incoming_sp - 48 + 16, target, argument, values[4];
    if (result)
        return result;
    consumer = rrj_read32(m, 0x8005b494);
    producer = rrj_read32(m, 0x8005b480);
    if (consumer == producer)
        return 0x800d0000;
    record = 0x800d7488 + (((consumer << 1) + consumer) << 3);
    values[0] = rrj_read32(m, record);
    values[1] = rrj_read32(m, record + 4);
    values[2] = rrj_read32(m, record + 8);
    values[3] = rrj_read32(m, record + 12);
    rrj_write32(m, parameters, values[0]);
    rrj_write32(m, parameters + 4, values[1]);
    rrj_write32(m, parameters + 8, values[2]);
    rrj_write32(m, parameters + 12, values[3]);
    values[0] = rrj_read32(m, record + 16);
    values[1] = rrj_read32(m, record + 20);
    rrj_write32(m, parameters + 16, values[0]);
    rrj_write32(m, parameters + 20, values[1]);
    target = rrj_read32(m, parameters + 12);
    rrj_write32(m, 0x8005b498, 1);
    if (target)
    {
        argument = rrj_read32(m, parameters + 16);
        (void)bodyless_call(m, call, target, argument, 0);
    }
    consumer = rrj_read32(m, 0x8005b494) + 1;
    rrj_write32(m, 0x8005b494, consumer);
    if (rrj_s32(consumer) >= 8)
        rrj_write32(m, 0x8005b494, 0);
    result = rrj_read32(m, 0x8005b480);
    consumer = rrj_read32(m, 0x8005b494);
    if (result != consumer)
    {
        record = 0x800d7488 + (((consumer << 1) + consumer) << 3);
        result = bodyless_call(m, call, 0x8001e418, record, 0);
    }
    else
        rrj_write32(m, 0x8005b490, 0);
    rrj_write32(m, 0x8005b498, 0);
    return result;
}

uint32_t sub_8002CA64(RRJMemory *m, uint32_t source, uint32_t x, uint32_t y, uint32_t runtime_gp, uint32_t incoming_sp, RRJRaceLeafCall call)
{
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
    (void)bodyless_call(m, call, 0x80048a6c, parameters, source);
    packed = (y << 6) | ((address >> 4) & 63);
    w_u16(runtime_gp + 0x8ac + (index << 1), (uint16_t)packed);
    return index;
}

uint32_t sub_8002C9A8(RRJMemory *m, uint32_t incoming_v0, RRJRaceLeafCall call)
{
    uint32_t address = rrj_read32(m, 0x8005ae78), result = incoming_v0;
    if (address)
    {
        result = bodyless_call(m, call, 0x800144b8, address, 0);
        rrj_write32(m, 0x8005ae78, 0);
    }
    rrj_write32(m, 0x8005b544, 0);
    return result;
}
