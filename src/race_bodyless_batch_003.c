#include "rrj_debug.h"
#include "native_file_callbacks.h"
#include "native_game_api.h"
#include "psx.h"
#include "audio.h"
#include "psx_spu.h"
#include <stdio.h>
#include <stdlib.h>
#include "race_bodyless_batch_003.h"
#include <stddef.h>

void sub_8002490C(uint32_t index, uint32_t enabled)
{
    FUNCTION_MARKER(0x8002490Cu, "SLUS_010.53");
    uint32_t count, address, value;
    if (((int32_t)(uint32_t)(index)) < 0)
        return;
    count = r_u32(0x80053578);
    if (index >= count)
        return;
    address = 0x80053578 + (((index << 1) + index) << 2);
    value = r_u32(address + 16);
    if (enabled)
        value |= 1;
    else
        value &= 0xfffffffe;
    w_u32(address + 16, value);
}

uint32_t sub_800148DC(uint8_t output[4], uint32_t index)
{
    FUNCTION_MARKER(0x800148DCu, "SLUS_010.53");
    uint32_t value = sub_8001500C(index);
    return sub_80015C88(output, value);
}

uint32_t sub_8001500C(uint32_t index)
{
    FUNCTION_MARKER(0x8001500Cu, "SLUS_010.53");
    uint32_t address = 0x800d65d0 + (((index << 2) + index) << 2);
    uint32_t value, base;
    if (!(r_u32(address) & 1))
        return 0xffffffff;
    value = r_u32(address + 4);
    base = r_u32(0x8005b3d8);
    return value + base;
}

uint32_t sub_80046768(const uint8_t position[3])
{
    FUNCTION_MARKER(0x80046768u, "SLUS_010.53");
    uint32_t minute = position[0];
    uint32_t second = position[1], frames;
    minute = (minute >> 4) * 10 + (minute & 15);
    second = (second >> 4) * 10 + (second & 15);
    frames = (minute * 60 + second) * 75;
    second = position[2];
    return frames + (second >> 4) * 10 + (second & 15) - 150;
}

uint32_t sub_8004DE34(uint32_t address)
{
    FUNCTION_MARKER(0x8004DE34u, "SLUS_010.53");
    return r_u16(address);
}

uint32_t sub_80050F28(uint32_t source, uint32_t byte_count)
{
    uint32_t size = byte_count;
    FUNCTION_MARKER(0x80050F28u, "SLUS_010.53");
    if (size > 0x7EFF0u)
        size = 0x7EFF0u;
    (void)rrj_spu_write(source, size);
    if (!r_u32(0x8005A454u))
        w_u32(0x8005A450u, 0);
    return size;
}

uint32_t sub_80015530(void)
{
    FUNCTION_MARKER(0x80015530u, "SLUS_010.53");
    (void)CdReadyCallbackPSX(0x80015860u);
    (void)CdSyncCallbackPSX(0u);
    return (uint32_t)CdControlB(14u, xport_guest_ptr(0x8005B3C8u, 1), NULL);
}

uint32_t sub_8001EFE8(uint32_t enabled)
{
    FUNCTION_MARKER(0x8001EFE8u, "SLUS_010.53");
    if (enabled)
    {
        SpuReverbAttr parameters;
        (void)SpuReserveReverbWorkArea(1);
        (void)SpuSetReverb(1);
        parameters.mask = 1u;
        parameters.mode = 3;
        (void)SpuSetReverbModeParam(&parameters);
        return SpuSetReverbVoice(1, 0x00ffffffu);
    }
    (void)SpuReserveReverbWorkArea(0);
    return (uint32_t)SpuSetReverb(0);
}

uint32_t sub_8001ED28(uint32_t context, uint32_t length, uint32_t slot, uint32_t callback, uint32_t token, uint32_t mode)
{
    uint32_t table = r_u32(0x800d6874), index = slot, value, limit;
    uint32_t args[8] = {0};
    FUNCTION_MARKER(0x8001ED28, "SLUS_010.53");
    if (((int32_t)(uint32_t)(slot)) < 0)
    {
        index = 0;
        value = r_u32(table);
        table += 4;
        if (value)
        {
            limit = r_u32(0x800d6870);
            do
            {
                ++index;
                if (((int32_t)(uint32_t)(limit)) < ((int32_t)(uint32_t)(index)))
                    return 0xffffffff;
                value = r_u32(table);
                table += 4;
            } while (value);
        }
    }
    table = r_u32(0x800d6874);
    if (r_u32(table + (index << 2)))
        return 0xfffffffe;
    args[0] = context;
    args[1] = length;
    args[2] = callback;
    args[4] = mode;
    args[3] = token;
    value = (uint32_t)sub_8001E938(args[0], args[1], args[2], args[3], args[4]);
    if (((int32_t)(uint32_t)(value)) <= 0)
        return 0xfffffffd;
    w_u32(context + 8, value);
    (void)(uint32_t)sub_8001EAA4(context, value);
    table = r_u32(0x800d6874);
    w_u32(table + (index << 2), context);
    return index;
}

uint32_t sub_8001E938(uint32_t context, uint32_t length, uint32_t callback, uint32_t argument, uint32_t mode)
{
    uint32_t parameters[6];
    uint32_t source = r_u32(context + 12);
    uint32_t result;
    uint32_t remaining = 0xfffff, prior;
    FUNCTION_MARKER(0x8001E938, "SLUS_010.53");
    result = (uint32_t)SpuMalloc(((int32_t)(uint32_t)(source)));
    if (((int32_t)(uint32_t)(result)) < 0)
        return 0xffffffff;
    parameters[0] = result;
    parameters[1] = source;
    parameters[2] = length;
    /* TODO Original callback-free paths leave the token unwritten; this draft uses argument */
    parameters[4] = argument;
    parameters[5] = 1;
    if (callback)
    {
        parameters[3] = callback;
    }
    else if (mode)
    {
        parameters[3] = 0x8001e928;
        w_u32(0x8005b4a4, 0);
    }
    else
    {
        parameters[3] = 0;
        w_u32(0x8005b4a4, 1);
    }
    (void)sub_8001E22C(parameters, mode);
    if (callback || r_u32(0x8005b4a4))
        return result;
    do
    {
        prior = remaining;
        --remaining;
        if (((int32_t)(uint32_t)(prior)) <= 0)
            return 0;
    } while (!r_u32(0x8005b4a4));
    return result;
}

uint32_t sub_8001E418(const uint32_t parameters[6])
{
    uint32_t handle, mode, destination, size;
    const uint8_t *bytes = (const uint8_t *)parameters;
    FUNCTION_MARKER(0x8001E418, "SLUS_010.53");
    handle = xport_load_le32(bytes);
    w_u32(0x8005B490, 1);
    (void)SpuSetTransferStartAddr(handle);
    if (spu_transfer_failed())
    {
        RRJ_DEBUG_FPUTS("Native SpuSetTransferStartAddr service failed\n", stderr);
        abort();
    }
    mode = xport_load_le32(bytes + 20);
    destination = xport_load_le32(bytes + 8);
    size = xport_load_le32(bytes + 4);
    if (mode == 1)
        return sub_80050F28(destination, size);
    return sub_80050EC8(destination, size);
}

uint32_t sub_8001E22C(const uint32_t parameters[6], uint32_t locked)
{
    uint32_t index, address, value, consumer, result, i;
    const uint8_t *bytes = (const uint8_t *)parameters;
    FUNCTION_MARKER(0x8001E22C, "SLUS_010.53");
    if (xport_load_le32(bytes + 4) < 16)
        return 1;
    if (locked)
        (void)sub_80043DA4();
    index = r_u32(0x8005B480);
    value = xport_load_le32(bytes);
    address = 0x800D7488 + (((index << 1) + index) << 3);
    w_u32(address, value);
    value = xport_load_le32(bytes + 4);
    consumer = r_u32(0x8005B494);
    w_u32(address + 4, value);
    for (i = 2; i != 6; ++i)
    {
        value = xport_load_le32(bytes + 4 * i);
        w_u32(address + 4 * i, value);
    }
    value = index + 1;
    w_u32(0x8005B480, value);
    result = ((int32_t)(uint32_t)(value)) < 8;
    if (!result)
        w_u32(0x8005B480, 0);
    if (index == consumer)
        result = sub_8001E418(parameters);
    if (locked)
    {
        sub_80043DB4();
        result = 0;
    }
    return result;
}

uint32_t sub_8001EAA4(uint32_t data, uint32_t base)
{
    FUNCTION_MARKER(0x8001EAA4u, "SLUS_010.53");
    uint32_t outer = 0, entry = data + 16, offset, block, field, inner, count, value;
    if (!r_u8(data + 4))
        return 0;
    do
    {
        offset = r_u32(entry);
        if (offset)
        {
            block = data + offset;
            if (r_u8(block))
            {
                inner = 0;
                field = block + 12;
                do
                {
                    value = r_u32(field);
                    ++inner;
                    w_u32(field, value + base);
                    count = r_u8(block);
                    field += 12;
                } while (((int32_t)(uint32_t)(inner)) < ((int32_t)(uint32_t)(count)));
            }
        }
        count = r_u8(data + 4);
        ++outer;
        entry += 4;
    } while (((int32_t)(uint32_t)(outer)) < ((int32_t)(uint32_t)(count)));
    return 0;
}

uint32_t sub_8001E328(void)
{
    uint32_t result = r_u32(0x8005B498u);
    uint32_t consumer;
    uint32_t producer;
    uint32_t record;
    uint32_t parameters[6];
    uint32_t index;

    FUNCTION_MARKER(0x8001E328, "SLUS_010.53");
    if (result)
        return result;
    consumer = r_u32(0x8005B494u);
    producer = r_u32(0x8005B480u);
    if (consumer == producer)
        return 0x800D0000u;
    record = 0x800D7488u + 24 * consumer;
    for (index = 0; index < 6; ++index)
        parameters[index] = r_u32(record + 4 * index);
    w_u32(0x8005B498u, 1);
    if (parameters[3])
        (void)rrj_transfer_complete_function(parameters[3])(parameters[4]);
    consumer = r_u32(0x8005B494u) + 1;
    w_u32(0x8005B494u, consumer);
    if (((int32_t)(uint32_t)(consumer)) >= 8)
        w_u32(0x8005B494u, 0);
    result = r_u32(0x8005B480u);
    consumer = r_u32(0x8005B494u);
    if (result == consumer)
        w_u32(0x8005B490u, 0);
    else
    {
        record = 0x800D7488u + 24 * consumer;
        result = sub_8001E418(xport_guest_ptr(record, 24));
    }
    w_u32(0x8005B498u, 0);
    return result;
}

uint32_t sub_8002CA64(uint32_t source[8], uint32_t x, uint32_t y)
{
    PSX_RECT rectangle;
    uint32_t index, address, packed;
    FUNCTION_MARKER(0x8002CA64u, "SLUS_010.53");
    if (r_u16(0x8005B54C) >= 4)
        w_u16(0x8005B54C, 0);
    rectangle.w = 16;
    index = r_u16(0x8005B54C);
    xport_store_le16(&rectangle.y, (uint16_t)y);
    rectangle.h = 1;
    address = x + (index << 4);
    w_u16(0x8005B54C, (uint16_t)(index + 1));
    xport_store_le16(&rectangle.x, (uint16_t)address);
    (void)LoadImagePSX(&rectangle, source);
    packed = (y << 6) | ((address >> 4) & 63);
    w_u16(0x8005B538 + (index << 1), (uint16_t)packed);
    return index;
}

void sub_8002C9A8(void)
{
    FUNCTION_MARKER(0x8002C9A8u, "SLUS_010.53");
    uint32_t address = r_u32(0x8005AE78u);
    if (address)
    {
        (void)sub_800144B8(address);
        w_u32(0x8005AE78u, 0);
    }
    w_u32(0x8005B544u, 0);
}
