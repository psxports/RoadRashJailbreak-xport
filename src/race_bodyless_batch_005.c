#include "race_bodyless_batch_005.h"

static uint32_t bodyless_call(RRJMemory *m, RRJRaceLeafCall call, uint32_t target, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
    const uint32_t args[8] = {a0, a1, a2, a3, 0, 0, 0, 0};
    return call(m, target, args);
}

uint32_t sub_80020BEC(RRJMemory *m, const uint32_t arguments[4], uint32_t runtime_gp, uint32_t incoming_sp, RRJRaceLeafCall call)
{
    uint32_t frame = incoming_sp - 64, frequency, option, size, channels, callback;
    uint32_t result, count, index, cursor;
    uint64_t product;
    rrj_write32(m, incoming_sp, arguments[0]);
    rrj_write32(m, incoming_sp + 4, arguments[1]);
    rrj_write32(m, incoming_sp + 8, arguments[2]);
    rrj_write32(m, incoming_sp + 12, arguments[3]);
    rrj_write32(m, incoming_sp, arguments[0]);
    rrj_write32(m, frame + 28, 5);
    rrj_write32(m, frame + 32, 44100);
    rrj_write32(m, frame + 36, 1024);
    rrj_write32(m, frame + 40, 1);
    rrj_write32(m, frame + 16, 0);
    rrj_write32(m, frame + 20, 0);
    rrj_write32(m, frame + 24, 0);
    (void)bodyless_call(m, call, 0x800448b4, 0x800d7548, 104, 0, 0);
    rrj_write32(m, runtime_gp + 0x82c, 0);
    rrj_write32(m, runtime_gp + 0x834, 0);
    rrj_write32(m, runtime_gp + 0x830, 0);
    rrj_write32(m, 0x8005b48c, 0);
    rrj_write32(m, 0x8005b484, 0);
    (void)bodyless_call(m, call, 0x80021058, incoming_sp, 5, frame + 16, 0);
    (void)bodyless_call(m, call, 0x80021058, incoming_sp, 6, frame + 20, 0);
    (void)bodyless_call(m, call, 0x80021058, incoming_sp, 7, frame + 24, 0);
    (void)bodyless_call(m, call, 0x80021058, incoming_sp, 3, frame + 28, 0);
    (void)bodyless_call(m, call, 0x80021058, incoming_sp, 1, frame + 32, 0);
    (void)bodyless_call(m, call, 0x80021058, incoming_sp, 2, frame + 36, 0);
    (void)bodyless_call(m, call, 0x80021058, incoming_sp, 4, frame + 40, 0);
    option = rrj_read32(m, frame + 16);
    size = rrj_read32(m, frame + 20);
    (void)bodyless_call(m, call, 0x80020ff4, option, size, 0, 0);
    frequency = rrj_read32(m, frame + 32);
    option = r_u8(frame + 40);
    product = (uint64_t)(frequency << 12) * UINT64_C(0xbe37c63b);
    rrj_write32(m, 0x800d754c, 2);
    w_u8(0x800d75ac, 0);
    w_u8(0x800d75ad, 0);
    size = rrj_read32(m, frame + 36);
    channels = rrj_read32(m, frame + 28);
    callback = rrj_read32(m, frame + 24);
    rrj_write32(m, 0x800d75a4, 350);
    rrj_write32(m, 0x800d7580, size);
    rrj_write32(m, 0x800d7584, channels);
    rrj_write32(m, runtime_gp + 0x838, callback);
    rrj_write32(m, 0x800d7550, (uint32_t)(product >> 47));
    (void)bodyless_call(m, call, 0x800210bc, option, size, channels, 0);
    result = bodyless_call(m, call, 0x800210c4, 0, 0, 0, 0);
    (void)bodyless_call(m, call, 0x800211f4, 0, 0, 0, 0);
    count = rrj_read32(m, 0x800d754c);
    index = 0;
    cursor = 0x800d7548;
    if (count)
        do
        {
            rrj_write32(m, cursor + 28, 0);
            count = rrj_read32(m, 0x800d754c);
            ++index;
            cursor += 20;
        } while (index < count);
    return result;
}

uint32_t sub_80021058(RRJMemory *m, uint32_t pairs, uint32_t key, uint32_t output, uint32_t index_output)
{
    uint32_t value, index = 0;
    if (!pairs)
        return 0;
    value = rrj_read32(m, pairs);
    while (value)
    {
        if (value == key)
        {
            if (output)
            {
                value = rrj_read32(m, pairs + 4);
                rrj_write32(m, output, value);
            }
            if (index_output)
                rrj_write32(m, index_output, index);
            return 1;
        }
        pairs += 8;
        value = rrj_read32(m, pairs);
        ++index;
    }
    return 0;
}

uint32_t sub_800210BC(void)
{
    return 0;
}

uint32_t sub_800210C4(RRJMemory *m, RRJRaceLeafCall call)
{
    uint32_t count = rrj_read32(m, 0x800d754c), index = 0, cursor = 0x800d7548;
    uint32_t size, channels, result;
    if (count)
        do
        {
            size = rrj_read32(m, 0x800d7580);
            channels = rrj_read32(m, 0x800d7584);
            result = bodyless_call(m, call, 0x8004f3c8, size * channels, 0, 0, 0);
            ++index;
            if (rrj_s32(result) < 0)
            {
                (void)bodyless_call(m, call, 0x80044894, 0x80010b10, 0, 0, 0);
                return result;
            }
            rrj_write32(m, cursor + 32, result);
            count = rrj_read32(m, 0x800d754c);
            cursor += 20;
        } while (index < count);
    return 0;
}

uint32_t sub_800211F4(RRJMemory *m, uint32_t runtime_gp)
{
    uint32_t size = rrj_read32(m, 0x800d7580), count, channels, value;
    uint32_t index = 0, cursor = 0x800d7548, result = 1;
    w_u8(0x800d7548, 0);
    count = rrj_read32(m, 0x800d754c);
    channels = rrj_read32(m, 0x800d7584);
    w_u8(0x800d7549, 1);
    w_u8(0x800d754a, 0);
    w_u8(0x800d754b, 0);
    w_u8(0x800d75a1, 0);
    rrj_write32(m, 0x800d7554, 0);
    rrj_write32(m, 0x800d758c, 0);
    rrj_write32(m, 0x800d7590, 0);
    rrj_write32(m, 0x800d7594, 1);
    rrj_write32(m, 0x800d759c, 0);
    w_u8(0x800d75a0, 1);
    rrj_write32(m, 0x800d7588, channels);
    rrj_write32(m, 0x800d7598, size);
    if (count)
        do
        {
            rrj_write32(m, cursor + 16, 0);
            value = rrj_read32(m, 0x800d6c14);
            rrj_write32(m, cursor + 20, value);
            rrj_write32(m, cursor + 24, (index & 1) ? 127 : 0);
            count = rrj_read32(m, 0x800d754c);
            ++index;
            result = index < count;
            cursor += 20;
        } while (result);
    rrj_write32(m, runtime_gp + 0x83c, 0);
    return result;
}

uint32_t sub_80023498(RRJMemory *m, uint32_t runtime_gp, uint32_t incoming_sp, RRJRaceLeafCall call)
{
    uint32_t frame = incoming_sp - 56, context, handle, result;
    uint32_t args[8] = {frame + 24, 0x8005ae38, 0x8005ae40, 0, 0, 0, 0, 0};
    context = rrj_read32(m, 0x8005b2f8);
    rrj_write32(m, frame + 16, 0x8005ae48);
    args[3] = rrj_read32(m, context + 48);
    args[4] = 0x8005ae48;
    (void)call(m, 0x80043fd4, args);
    (void)bodyless_call(m, call, 0x80023f08, runtime_gp + 0x86c, frame + 24, 0, 0);
    context = rrj_read32(m, 0x8005b2f8);
    rrj_write32(m, frame + 16, 0x8005ae50);
    args[3] = rrj_read32(m, context + 48);
    args[4] = 0x8005ae50;
    (void)call(m, 0x80043fd4, args);
    result = bodyless_call(m, call, 0x8002428c, frame + 24, 0, 0, 0);
    if (!result)
        rrj_write32(m, runtime_gp + 0x88c, 0);
    context = rrj_read32(m, 0x8005b2f8);
    rrj_write32(m, runtime_gp + 0x880, 0);
    rrj_write32(m, runtime_gp + 0x87c, 0);
    rrj_write32(m, frame + 16, 0x8005ae58);
    args[3] = rrj_read32(m, context + 48);
    args[4] = 0x8005ae58;
    (void)call(m, 0x80043fd4, args);
    handle = bodyless_call(m, call, 0x80023100, frame + 24, 0, 0, 0);
    rrj_write32(m, runtime_gp + 0x870, handle);
    rrj_write32(m, runtime_gp + 0x888, 0);
    result = bodyless_call(m, call, 0x800148bc, handle, 0, 0, 0);
    rrj_write32(m, runtime_gp + 0x884, result);
    (void)bodyless_call(m, call, 0x8002379c, 0, 0, 0, 0);
    (void)bodyless_call(m, call, 0x80024354, 0, 0, 0, 0);
    (void)bodyless_call(m, call, 0x8002379c, 1, 0, 0, 0);
    result = bodyless_call(m, call, 0x80024354, 0, 0, 0, 0);
    rrj_write32(m, runtime_gp + 0x874, 0);
    return result;
}

uint32_t sub_80023F08(RRJMemory *m, uint32_t output, uint32_t name, RRJRaceLeafCall call)
{
    uint32_t handle, size, buffer, result, address, value;
    handle = bodyless_call(m, call, 0x80023100, name, 0, 0, 0);
    size = bodyless_call(m, call, 0x800148bc, handle, 0, 0, 0);
    buffer = bodyless_call(m, call, 0x8001447c, size, 0, 0, 0);
    result = bodyless_call(m, call, 0x80014780, handle, buffer, size, 0);
    (void)bodyless_call(m, call, 0x8001460c, handle, 0, 0, 0);
    if (rrj_s32(result) < 0)
        return 0;
    rrj_write32(m, output, buffer);
    value = rrj_read32(m, buffer + 24);
    rrj_write32(m, buffer + 24, buffer + value);
    address = rrj_read32(m, output);
    value = rrj_read32(m, address + 20);
    rrj_write32(m, address + 20, buffer + value);
    return 1;
}

uint32_t sub_8002428C(RRJMemory *m, uint32_t name, uint32_t runtime_gp, RRJRaceLeafCall call)
{
    uint32_t handle, size, buffer, result, cursor, count, index, value;
    handle = bodyless_call(m, call, 0x80023100, name, 0, 0, 0);
    if (rrj_s32(handle) < 0)
        return 0;
    size = bodyless_call(m, call, 0x800148bc, handle, 0, 0, 0);
    buffer = bodyless_call(m, call, 0x8001447c, size, 0, 0, 0);
    result = bodyless_call(m, call, 0x80014780, handle, buffer, size, 0);
    (void)bodyless_call(m, call, 0x8001460c, handle, 0, 0, 0);
    cursor = buffer + 8;
    if (rrj_s32(result) < 0)
        return 0;
    rrj_write32(m, runtime_gp + 0x1a4, buffer);
    value = r_u8(buffer + 4);
    rrj_write32(m, runtime_gp + 0x88c, cursor);
    count = (value - 8) >> 2;
    index = 0;
    if (count)
        do
        {
            value = rrj_read32(m, cursor);
            ++index;
            rrj_write32(m, cursor, value + buffer);
            cursor += 4;
        } while (rrj_s32(index) < rrj_s32(count));
    return 1;
}

uint32_t sub_80024354(RRJMemory *m, uint32_t runtime_gp, RRJRaceLeafCall call)
{
    uint32_t address = rrj_read32(m, runtime_gp + 0x1a8);
    rrj_write32(m, address + 4, 0);
    rrj_write32(m, address + 44, 0);
    rrj_write32(m, address + 60, 0);
    rrj_write32(m, address + 32, 0);
    rrj_write32(m, address + 72, UINT32_MAX);
    rrj_write32(m, address + 52, UINT32_MAX);
    rrj_write32(m, address + 92, 0);
    rrj_write32(m, address + 96, 0);
    rrj_write32(m, address + 76, 0);
    rrj_write32(m, address + 40, 0);
    rrj_write32(m, address + 36, 0);
    (void)bodyless_call(m, call, 0x80023da4, address + 80, 0, 0, 0);
    address = rrj_read32(m, runtime_gp + 0x1a8);
    rrj_write32(m, address + 100, 0);
    rrj_write32(m, address + 104, 0);
    rrj_write32(m, address + 108, 0);
    return address;
}

uint32_t sub_80023DA4(RRJMemory *m, uint32_t address, RRJNativeCallFrame *frame)
{
    frame->return_value = UINT32_MAX;
    rrj_write32(m, address + 8, 0);
    rrj_write32(m, address, frame->return_value);
    rrj_write32(m, address + 4, frame->return_value);
    return frame->return_value;
}

uint32_t sub_80023020(RRJMemory *m, RRJRaceLeafCall call)
{
    uint32_t address = rrj_read32(m, 0x800d6184), a0, a1, a2;
    a0 = rrj_read32(m, address);
    a1 = (uint32_t)r_s16(address + 6);
    a2 = rrj_read32(m, address + 8);
    (void)bodyless_call(m, call, 0x800235c8, a0, a1, a2, 0);
    return bodyless_call(m, call, 0x80023714, 0, 0, 0, 0);
}

uint32_t sub_800235C8(RRJMemory *m, uint32_t first, uint32_t second, uint32_t third, uint32_t runtime_gp, RRJRaceLeafCall call)
{
    uint32_t context = rrj_read32(m, 0x8005b2f8), count, index = 0;
    uint32_t record, data, value, target, result;
    count = rrj_read32(m, context + 48);
    if (!count)
        return 0;
    do
    {
        (void)bodyless_call(m, call, 0x8002379c, index, 0, 0, 0);
        record = rrj_read32(m, runtime_gp + 0x1a8);
        rrj_write32(m, record + 72, first);
        rrj_write32(m, record + 68, second);
        rrj_write32(m, record + 64, third);
        data = bodyless_call(m, call, 0x800245dc, first, 0, 0, 0);
        record = rrj_read32(m, runtime_gp + 0x1a8);
        rrj_write32(m, record + 92, data);
        value = rrj_read32(m, data + 4);
        rrj_write32(m, record + 96, value >> 6);
        value = rrj_read32(m, record + 64);
        data = rrj_read32(m, record + 92);
        value = rrj_read32(m, data + (rrj_s32(value) > 0 ? 12 : 8));
        target = rrj_read32(m, runtime_gp + 0x1a8);
        rrj_write32(m, record + 52, value);
        (void)bodyless_call(m, call, 0x80023db8, target + 80, 0, 0, 0);
        context = rrj_read32(m, 0x8005b2f8);
        count = rrj_read32(m, context + 48);
        ++index;
        result = index < count;
    } while (result);
    return result;
}

uint32_t sub_80023714(RRJMemory *m, uint32_t incoming_sp, RRJRaceLeafCall call)
{
    uint32_t buffer = incoming_sp - 40 + 16, context, mode, sequence, count, index = 0, result;
    context = rrj_read32(m, 0x8005b2f8);
    mode = rrj_read32(m, context + 48);
    sequence = rrj_read32(m, context + 64);
    (void)bodyless_call(m, call, 0x80043fd4, buffer, 0x80010bc8, mode, sequence);
    context = rrj_read32(m, 0x8005b2f8);
    count = rrj_read32(m, context + 48);
    if (!count)
        return 0;
    do
    {
        (void)bodyless_call(m, call, 0x8002379c, index, 0, 0, 0);
        (void)bodyless_call(m, call, 0x80024168, buffer, 0, 0, 0);
        context = rrj_read32(m, 0x8005b2f8);
        count = rrj_read32(m, context + 48);
        ++index;
        result = index < count;
    } while (result);
    return result;
}

uint32_t sub_80024168(RRJMemory *m, uint32_t name, uint32_t runtime_gp, uint32_t incoming_sp, RRJRaceLeafCall call)
{
    uint32_t frame = incoming_sp - 72, handle, result, first, second, address, flags, index = 0;
    handle = bodyless_call(m, call, 0x80023100, name, 0, 0, 0);
    if (rrj_s32(handle) < 0)
        return 0;
    result = bodyless_call(m, call, 0x80014780, handle, frame + 16, 40, 0);
    if (rrj_s32(result) < 0)
        return 0;
    first = rrj_read32(m, frame + 36);
    address = rrj_read32(m, runtime_gp + 0x1a8);
    second = rrj_read32(m, frame + 40);
    (void)bodyless_call(m, call, 0x80023c24, address + 8, handle, first, second);
    address = rrj_read32(m, runtime_gp + 0x1a8);
    flags = rrj_read32(m, address);
    rrj_write32(m, address + 56, 0);
    rrj_write32(m, address + 32, 1);
    flags <<= 16;
    do
    {
        address = rrj_read32(m, runtime_gp + 0x1a8);
        result = bodyless_call(m, call, 0x80023300, address + 8, 0, 0, 0);
        if (result)
            break;
        address = rrj_read32(m, runtime_gp + 0x1a8);
        (void)bodyless_call(m, call, 0x80023148, address + 8, 64, 0, flags);
        (void)bodyless_call(m, call, 0x80022a78, 2, 0, 0, 0);
        result = bodyless_call(m, call, 0x80022a78, 2, 0, 0, 0);
        if (result)
        {
            (void)bodyless_call(m, call, 0x800229f4, 2, 0, 0, 0);
            (void)bodyless_call(m, call, 0x800229b0, 0, 0, 0, 0);
        }
        ++index;
        (void)bodyless_call(m, call, 0x80030608, 0, 0, 0, 0);
    } while (rrj_s32(index) < 3);
    address = rrj_read32(m, runtime_gp + 0x1a8);
    rrj_write32(m, address + 32, 0);
    (void)bodyless_call(m, call, 0x80023300, address + 8, 0, 0, 0);
    (void)bodyless_call(m, call, 0x8001460c, handle, 0, 0, 0);
    (void)bodyless_call(m, call, 0x80023de4, 0, 0, 0, 0);
    first = rrj_read32(m, frame + 52);
    (void)bodyless_call(m, call, 0x800243bc, first, 0, 0, 0);
    return 1;
}

uint32_t sub_80023C24(RRJMemory *m, uint32_t address, uint32_t handle, uint32_t offset, uint32_t size)
{
    rrj_write32(m, address + 4, offset);
    rrj_write32(m, address + 12, offset);
    rrj_write32(m, address + 8, 16);
    rrj_write32(m, address, handle);
    rrj_write32(m, address + 16, offset + size);
    rrj_write32(m, address + 20, 1);
    return 1;
}

uint32_t sub_80033DAC(RRJMemory *m, uint32_t values, uint32_t auxiliary, uint32_t incoming_v0, RRJRaceLeafCall call)
{
    uint32_t second = rrj_read32(m, values + 4), first, third, result = incoming_v0;
    if (rrj_s32(second) < 0)
        return result;
    first = rrj_read32(m, values);
    result = rrj_s32(first) < rrj_s32(second);
    if (result)
        result = bodyless_call(m, call, 0x80033d70, values, auxiliary, 0, 1);
    third = rrj_read32(m, values + 8);
    if (rrj_s32(third) < 0)
        return result;
    second = rrj_read32(m, values + 4);
    result = rrj_s32(second) < rrj_s32(third);
    if (result)
    {
        (void)bodyless_call(m, call, 0x80033d70, values, auxiliary, 1, 2);
        second = rrj_read32(m, values + 4);
        first = rrj_read32(m, values);
        result = rrj_s32(first) < rrj_s32(second);
        if (result)
            result = bodyless_call(m, call, 0x80033d70, values, auxiliary, 0, 1);
    }
    return result;
}
