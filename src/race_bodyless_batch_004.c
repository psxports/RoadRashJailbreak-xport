#include "native_game_api.h"
#include "solo_file_dependencies.h"
#include <stdio.h>
#include <stdlib.h>
#include "psx.h"
#include "race_bodyless_batch_004.h"

static uint32_t bodyless_call(RRJMemory *m, RRJRaceLeafCall call, uint32_t target, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
    const uint32_t args[8] = {a0, a1, a2, a3, 0, 0, 0, 0};
    return call(m, target, args);
}

uint32_t sub_8002C9D4(uint32_t index)
{
    FUNCTION_MARKER(0x8002C9D4u, "SLUS_010.53");
    uint32_t address = 0x800d8078 + (((index << 1) + index) << 3), allocation;
    if (r_u8(address))
    {
        allocation = rrj_read32(address + 4);
        if (allocation)
            (void)(uint32_t)sub_800144B8(allocation);
    }
    address = 0x800d8078 + (((index << 1) + index) << 3);
    w_u8(address, 0);
    w_u8(address + 1, 0);
    w_u8(address + 2, 0);
    rrj_write32(address + 4, 0);
    return address;
}

uint32_t sub_8001B67C(uint32_t index)
{
    FUNCTION_MARKER(0x8001B67Cu, "SLUS_010.53");
    uint32_t address;
    rrj_write32(0x800d6c68, index);
    address = 0x800d6c6c + (((index << 1) + index) << 3);
    rrj_write32(0x8005B474u, address);
    return address;
}

uint32_t sub_8001BCE8(uint32_t mode, uint32_t incoming_sp, RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x8001BCE8u, "SLUS_010.53");
    uint32_t frame = incoming_sp - 152, a1, a2, value, result;
    (void)bodyless_call(rrj_host_context(), call, 0x800494e8, frame + 16, 0, 0, 0);
    (void)bodyless_call(rrj_host_context(), call, 0x80048fbc, frame + 40, 0, 0, 0);
    a1 = (uint32_t)r_s16(frame + 40);
    a2 = (uint32_t)r_s16(frame + 42);
    if (mode == 1)
    {
        rrj_write32(0x8005b45c, 61);
        rrj_write32(0x8005b460, 61);
        w_u16(frame + 136, 61);
        w_u16(frame + 140, 262);
        w_u16(frame + 142, 9);
        value = r_u16(frame + 18);
        a1 += 61;
        a2 += 198;
        value += 216;
    }
    else
    {
        rrj_write32(0x8005b45c, 124);
        rrj_write32(0x8005b460, 124);
        w_u16(frame + 136, 122);
        w_u16(frame + 140, 140);
        w_u16(frame + 142, 20);
        value = r_u16(frame + 18);
        a1 += 122;
        a2 += 160;
        value += 200;
    }
    w_u16(frame + 138, (uint16_t)value);
    result = bodyless_call(rrj_host_context(), call, 0x80048b2c, frame + 136, a1, a2, 0);
    rrj_write32(0x8005b464, 0);
    rrj_write32(0x8005ad88, mode);
    rrj_write32(0x8005b468, 0);
    return result;
}

uint32_t sub_8001B8BC(uint32_t incoming_sp, RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x8001B8BCu, "SLUS_010.53");
    uint32_t frame = incoming_sp - 200;
    uint32_t v0, v1, a0, a1, a2, a3, t0, t1, t2, argument;
    int64_t product;
    if (rrj_read32(0x8005ad88) == 1)
    {
        w_u16(frame + 62, 9);
        w_u16(frame + 56, 61);
        w_u16(frame + 58, 216);
        w_u16(frame + 60, 262);
        w_u16(frame + 64, 61);
        w_u16(frame + 66, 216);
        w_u16(frame + 68, 262);
        v0 = 8;
    }
    else
    {
        w_u16(frame + 56, 122);
        w_u16(frame + 58, 200);
        w_u16(frame + 60, 140);
        w_u16(frame + 62, 20);
        w_u16(frame + 64, 124);
        w_u16(frame + 66, 213);
        w_u16(frame + 68, 136);
        v0 = 5;
    }
    w_u16(frame + 70, (uint16_t)v0);
    if (!rrj_read32(0x8005b468))
    {
        v0 = rrj_read32(0x8005b2f8);
        a0 = rrj_read32(v0 + 100);
        product = (int64_t)rrj_s32(a0) * INT64_C(0x66666667);
        v1 = (uint32_t)((uint64_t)product >> 32);
        v1 = (uint32_t)(rrj_s32(v1) >> 2) - (uint32_t)(rrj_s32(a0) >> 31);
        v0 = ((v1 << 2) + v1) << 1;
        if (a0 != v0)
            return v0;
    }
    if (!rrj_read32(0x8005b468))
    {
        v0 = (uint32_t)r_s16(frame + 68);
        v1 = rrj_read32(0x8005b460);
        a0 = rrj_read32(0x8005b45c);
        v1 += v0;
        if (rrj_s32(a0) >= rrj_s32(v1))
        {
            v0 = rrj_read32(0x8005b460);
            rrj_write32(0x8005b45c, v0);
            v0 = rrj_read32(0x8005b464);
            rrj_write32(0x8005b464, (v0 + 1) & 3);
            goto geometry;
        }
    }
    v1 = rrj_read32(0x8005b45c);
    v0 = rrj_read32(0x8005b468);
    v1 += 1 + ((0 - v0) & 8);
    rrj_write32(0x8005b45c, v1);
geometry:
    (void)bodyless_call(rrj_host_context(), call, 0x800494e8, frame + 72, 0, 0, 0);
    (void)bodyless_call(rrj_host_context(), call, 0x80048fbc, frame + 96, 0, 0, 0);
    t0 = r_u16(frame + 56);
    v0 = r_u16(frame + 96);
    v1 = r_u16(frame + 58);
    a3 = r_u16(frame + 62);
    t1 = r_u16(frame + 60);
    t0 += v0;
    a1 = (uint32_t)(int32_t)(int16_t)t0;
    v0 = r_u16(frame + 98);
    w_u16(frame + 40, (uint16_t)t0);
    w_u16(frame + 44, (uint16_t)t1);
    w_u16(frame + 46, (uint16_t)a3);
    w_u16(frame + 48, (uint16_t)t0);
    w_u16(frame + 52, (uint16_t)t1);
    w_u16(frame + 54, (uint16_t)a3);
    v1 += v0;
    a2 = (uint32_t)(int32_t)(int16_t)v1;
    w_u16(frame + 42, (uint16_t)(v1 - (a3 << 1)));
    w_u16(frame + 50, (uint16_t)v1);
    (void)bodyless_call(rrj_host_context(), call, 0x80048b2c, frame + 40, a1, a2, a3);
    a0 = frame + 16;
    w_u8(a0 + 3, 5);
    w_u8(a0 + 7, 0x28);
    w_u8(a0 + 4, 200);
    w_u8(a0 + 5, 200);
    w_u8(a0 + 6, 200);
    w_u8(a0 + 7, 0x2a);
    t1 = r_u16(frame + 64);
    a1 = rrj_read32(0x8005b45c);
    a2 = rrj_read32(0x8005b460);
    v1 = rrj_read32(0x8005b45c);
    v0 = rrj_read32(0x8005b460);
    a1 -= a2;
    v1 -= v0;
    v0 = rrj_read32(0x8005b460);
    a2 = rrj_read32(0x8005b45c);
    v1 = (uint32_t)(rrj_s32(v1) >> 31);
    w_u16(a0 + 8, (uint16_t)t1);
    t0 = r_u16(frame + 66);
    v0 -= a2;
    a1 += v1 & v0;
    w_u16(a0 + 10, (uint16_t)t0);
    t2 = r_u16(frame + 68);
    a3 = (uint32_t)r_s16(frame + 68);
    v0 = rrj_read32(0x8005b45c);
    a2 = rrj_read32(0x8005b460);
    v1 = rrj_read32(0x8005b45c);
    w_u16(a0 + 14, (uint16_t)t0);
    w_u16(a0 + 16, (uint16_t)t1);
    v0 = a3 - (v0 - a2);
    a2 = rrj_read32(0x8005b460);
    v0 = (uint32_t)(rrj_s32(v0) >> 31);
    v1 = t2 - (v1 - a2);
    a1 += v0 & v1;
    a1 += t1;
    w_u16(a0 + 12, (uint16_t)a1);
    v0 = r_u16(frame + 70);
    a1 = rrj_read32(0x8005b45c);
    a2 = rrj_read32(0x8005b460);
    t0 += v0;
    v0 = rrj_read32(0x8005b45c);
    v1 = rrj_read32(0x8005b460);
    a1 -= a2;
    w_u16(a0 + 18, (uint16_t)t0);
    v0 -= v1;
    v1 = rrj_read32(0x8005b460);
    a2 = rrj_read32(0x8005b45c);
    v0 = (uint32_t)(rrj_s32(v0) >> 31);
    a1 += v0 & (v1 - a2);
    v1 = rrj_read32(0x8005b45c);
    a2 = rrj_read32(0x8005b460);
    v0 = rrj_read32(0x8005b45c);
    v1 -= a2;
    a3 -= v1;
    v1 = rrj_read32(0x8005b460);
    a3 = (uint32_t)(rrj_s32(a3) >> 31);
    t2 -= v0 - v1;
    a1 += a3 & t2;
    t1 += a1;
    w_u16(a0 + 20, (uint16_t)t1);
    w_u16(a0 + 22, (uint16_t)t0);
    (void)bodyless_call(rrj_host_context(), call, 0x80048d58, a0, a1, a2, a3);
    v0 = (uint32_t)r_s16(frame + 58);
    a2 = (uint32_t)r_s16(frame + 74);
    a1 = (uint32_t)r_s16(frame + 56);
    argument = v0 + a2;
    return bodyless_call(rrj_host_context(), call, 0x80048b2c, frame + 48, a1, argument, 0);
}

uint32_t sub_8001C1AC(RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x8001C1ACu, "SLUS_010.53");
    uint32_t index, address, mode, count, allocation;
    for (index = 0; index < 4; ++index)
    {
        rrj_write32(0x800d75c0 + (index << 2), 0);
        rrj_write32(0x800d75d0 + (index << 6), 0);
    }
    address = rrj_read32(0x8005b2f8);
    mode = rrj_read32(address + 48);
    address = rrj_read32(0x8005b470);
    w_u8(address + 244, mode == 1 ? 2 : 4);
    address = rrj_read32(0x8005b470);
    count = r_u8(address + 244);
    index = 0;
    if (count)
        do
        {
            allocation = (uint32_t)sub_8001447C(0x1450, 0, call);
            address = rrj_read32(0x8005b470);
            rrj_write32(address + (index << 2) + 248, allocation);
            count = r_u8(address + 244);
            ++index;
        } while (rrj_s32(index) < rrj_s32(count));
    address = rrj_read32(0x8005b470);
    allocation = rrj_read32(address + 248);
    rrj_write32(0x8005ae00, 0);
    rrj_write32(address + 264, allocation);
    return bodyless_call(rrj_host_context(), call, 0x80048cac, allocation, 0x514, 0, 0);
}

uint32_t sub_8001C498(void)
{
    FUNCTION_MARKER(0x8001C498u, "SLUS_010.53");
    return rrj_read32(0x800d6c68);
}

uint32_t sub_8002D2E0(uint32_t runtime_gp, RRJNativeCallFrame *frame)
{
    FUNCTION_MARKER(0x8002D2E0u, "SLUS_010.53");
    frame->return_value = 0x80050000;
    w_u8(runtime_gp + 0x22d, 0);
    w_u8(runtime_gp + 0x22c, 0);
    w_u16(0x800540da, 0);
    return frame->return_value;
}

uint32_t sub_800303BC(uint32_t index, uint32_t key, uint32_t value)
{
    FUNCTION_MARKER(0x800303BCu, "SLUS_010.53");
    uint32_t record = 0x800d4c38 + (index << 4), base, address, result;
    base = rrj_read32(record + 8);
    key -= base;
    address = rrj_read32(record + 12);
    address += key << 1;
    result = (uint32_t)r_s16(address);
    if (rrj_s32(result) < 0)
    {
        w_u16(address, (uint16_t)value);
        result = rrj_read32(record + 4) + 1;
        rrj_write32(record + 4, result);
    }
    return result;
}

uint32_t sub_80016464(uint32_t handle, RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x80016464, "SLUS_010.53");
    if (handle)
    {
        rrj_write32(0x800d6aa0 + ((handle >> 11) & 0xfe0) + 8, 2);
        return bodyless_call(rrj_host_context(), call, 0x80030fa0, handle & 255, 0, 0, 0);
    }
    rrj_write32(0x8005B3E0u, 1);
    return 1;
}

uint32_t sub_80030810(uint32_t first_output, uint32_t second_output)
{
    FUNCTION_MARKER(0x80030810u, "SLUS_010.53");
    uint32_t context = rrj_read32(0x8005acbc), value;
    if (!rrj_read32(context + 0xa3c))
        return 0;
    value = rrj_read32(context + 0xa38);
    if (!value)
        return 0;
    rrj_write32(first_output, value);
    context = rrj_read32(0x8005acbc);
    value = rrj_read32(context + 0xa58);
    rrj_write32(second_output, value << 14);
    return 1;
}

uint32_t sub_800244E0(RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x800244E0u, "SLUS_010.53");
    uint32_t context = rrj_read32(0x8005b2f8), file_name, handle, size, buffer, value;
    file_name = rrj_read32(context + 48) == 2 ? 0x80010bd8 : 0x80010be4;
    (void)bodyless_call(rrj_host_context(), call, 0x80043fd4, 0x800d7e80, file_name, 0, 0);
    handle = (uint32_t)sub_80023100(0x800d7e80);
    size = (uint32_t)sub_800148BC(handle, call);
    buffer = rrj_read32(0x8005ae64);
    if (!buffer)
    {
        value = (uint32_t)sub_8001447C(size, 0, call);
        rrj_write32(0x8005ae64, value);
    }
    buffer = rrj_read32(0x8005ae64);
    (void)(uint32_t)sub_80014780(handle, buffer, size, call);
    (void)(uint32_t)sub_8001460C(handle, call);
    buffer = rrj_read32(0x8005ae64);
    (void)bodyless_call(rrj_host_context(), call, 0x80024610, buffer, size, 0, 0);
    buffer = rrj_read32(0x8005ae64);
    value = rrj_read32(buffer + 8);
    rrj_write32(0x8005ae60, value);
    return value;
}

uint32_t sub_80023100(uint32_t name)
{
    FUNCTION_MARKER(0x80023100u, "SLUS_010.53");
    char path[128] = {0};
    uint32_t sources[2] = {rrj_read32(0x8005AE2Cu), name};
    uint32_t part, length = 0;
    for (part = 0; part < 2; ++part)
    {
        uint32_t source = sources[part];
        uint32_t character;
        while ((character = r_u8(source++)) != 0)
        {
            /* TODO The original strcat assumes the expanded name fits 128 bytes */
            if (length == sizeof path - 1)
            {
                fputs("sub_80023100: expanded name exceeds original path capacity\n", stderr);
                abort();
            }
            path[length++] = (char)character;
        }
    }
    return rrj_solo_open_path(rrj_host_context(), path, 0);
}

uint32_t sub_80024610(RRJNativeCallFrame *frame, uint32_t arguments[8])
{
    FUNCTION_MARKER(0x80024610u, "SLUS_010.53");
    frame->return_value = rrj_read32(arguments[0] + 20);
    frame->secondary_result = rrj_read32(arguments[0] + 24);
    frame->return_value += arguments[0];
    frame->secondary_result += arguments[0];
    rrj_write32(arguments[0] + 20, frame->return_value);
    frame->return_value = 1;
    rrj_write32(arguments[0] + 24, frame->secondary_result);
    return frame->return_value;
}

uint32_t sub_80022F78(RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x80022F78u, "SLUS_010.53");
    uint32_t context = rrj_read32(0x8005b2f8);
    if (rrj_read32(context + 48) == 1)
        (void)bodyless_call(rrj_host_context(), call, 0x80024630, 0, 0, 0, 0);
    (void)bodyless_call(rrj_host_context(), call, 0x80023498, 0, 0, 0, 0);
    return (uint32_t)sub_80022A1C(2, call);
}

uint32_t sub_80024630(uint32_t runtime_gp, uint32_t incoming_sp, RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x80024630u, "SLUS_010.53");
    uint32_t frame = incoming_sp - 88, name, first, second, result, flags;
    const uint32_t args[8] = {1, 16000, 2, 0x2000, 3, 8, 4, 0};
    (void)(uint32_t)sub_80024FF8();
    name = rrj_read32(runtime_gp + 0x1dc);
    rrj_write32(0x8005b4e8, 0);
    rrj_write32(0x80053658, 0);
    first = (uint32_t)sub_80023100(name);
    if (rrj_s32(first) < 0)
        return first;
    name = rrj_read32(runtime_gp + 0x1e0);
    second = (uint32_t)sub_80023100(name);
    if (rrj_s32(second) < 0)
        return second;
    rrj_write32(0x800cd6a0, UINT32_MAX);
    rrj_write32(0x800cd6a4, 1);
    rrj_write32(0x800cd670, first);
    rrj_write32(0x800cd674, 0);
    rrj_write32(0x800cd678, 8);
    rrj_write32(0x800cd67c, 0);
    result = (uint32_t)sub_800148BC(first, call);
    rrj_write32(0x800cd680, result);
    rrj_write32(0x800cd688, first);
    rrj_write32(0x800cd68c, second);
    rrj_write32(0x800cd69c, 0);
    rrj_write32(frame + 16, 3);
    rrj_write32(frame + 24, 4);
    rrj_write32(frame + 32, 7);
    rrj_write32(frame + 36, 0x80024f78);
    rrj_write32(frame + 40, 5);
    rrj_write32(frame + 20, 8);
    rrj_write32(frame + 28, 0);
    rrj_write32(frame + 44, 0);
    flags = rrj_read32(0x800cd684);
    rrj_write32(runtime_gp + 0x564, 0);
    rrj_write32(frame + 48, 6);
    rrj_write32(frame + 52, 0);
    rrj_write32(frame + 56, 0);
    rrj_write32(0x800cd684, flags | 1);
    result = call(rrj_host_context(), 0x80020bec, args);
    if (rrj_s32(result) >= 0)
        rrj_write32(0x800cd694, 0);
    return result;
}

uint32_t sub_80024FF8(void)
{
    FUNCTION_MARKER(0x80024FF8u, "SLUS_010.53");
    uint32_t index, address = 0x800d7e90;
    rrj_write32(address + 248, 0);
    rrj_write32(address + 244, 0);
    rrj_write32(address + 240, 0);
    for (index = 0; index < 30; ++index)
    {
        rrj_write32(address, UINT32_MAX);
        rrj_write32(address + 4, 0);
        address += 8;
    }
    return 0;
}
