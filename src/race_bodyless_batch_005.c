#include "rrj_debug.h"
#include <stdio.h>
#include <string.h>
#include "native_game_api.h"
#include "psx.h"
#include "psx_spu.h"
#include "race_bodyless_batch_005.h"

uint32_t sub_80020BEC(const uint32_t *pairs)
{
    FUNCTION_MARKER(0x80020BECu, "SLUS_010.53");
    uint32_t first = 0, second = 0, callback = 0;
    uint32_t channels = 5, frequency = 44100, size = 1024, option = 1;
    uint32_t result, count, index, cursor;
    uint64_t product;
    memset(xport_guest_ptr(0x800D7548, 104), 0, 104);
    w_u32(0x8005B4B8, 0);
    w_u32(0x8005B4C0, 0);
    w_u32(0x8005B4BC, 0);
    w_u32(0x8005B48C, 0);
    w_u32(0x8005B484, 0);
    (void)sub_80021058(pairs, 5, &first, NULL);
    (void)sub_80021058(pairs, 6, &second, NULL);
    (void)sub_80021058(pairs, 7, &callback, NULL);
    (void)sub_80021058(pairs, 3, &channels, NULL);
    (void)sub_80021058(pairs, 1, &frequency, NULL);
    (void)sub_80021058(pairs, 2, &size, NULL);
    (void)sub_80021058(pairs, 4, &option, NULL);
    (void)sub_80020FF4(first, second);
    product = (uint64_t)(frequency << 12) * UINT64_C(0xBE37C63B);
    w_u32(0x800D754C, 2);
    w_u8(0x800D75AC, 0);
    w_u8(0x800D75AD, 0);
    w_u32(0x800D75A4, 350);
    w_u32(0x800D7580, size);
    w_u32(0x800D7584, channels);
    /* TODO Migrate the stored guest callback to typed native registration */
    w_u32(0x8005B4C4, callback);
    w_u32(0x800D7550, (uint32_t)(product >> 47));
    /* The original empty 210BC body does not consume the low option byte */
    (void)sub_800210BC();
    result = sub_800210C4();
    (void)sub_800211F4();
    count = r_u32(0x800D754C);
    index = 0;
    cursor = 0x800D7548;
    if (count)
        do
        {
            w_u32(cursor + 28, 0);
            count = r_u32(0x800D754C);
            ++index;
            cursor += 20;
        } while (index < count);
    return result;
}

uint32_t sub_80021058(const uint32_t *pairs, uint32_t key, uint32_t *output, uint32_t *index_output)
{
    FUNCTION_MARKER(0x80021058u, "SLUS_010.53");
    uint32_t value, index = 0;
    if (!pairs)
        return 0;
    value = pairs[0];
    while (value)
    {
        if (value == key)
        {
            if (output)
                *output = pairs[1];
            if (index_output)
                *index_output = index;
            return 1;
        }
        pairs += 2;
        value = pairs[0];
        ++index;
    }
    return 0;
}

uint32_t sub_800210BC(void)
{
    FUNCTION_MARKER(0x800210BCu, "SLUS_010.53");
    return 0;
}

uint32_t sub_800210C4(void)
{
    FUNCTION_MARKER(0x800210C4u, "SLUS_010.53");
    uint32_t count = r_u32(0x800d754c), index = 0, cursor = 0x800d7548;
    uint32_t size, channels, result;
    if (count)
        do
        {
            size = r_u32(0x800d7580);
            channels = r_u32(0x800d7584);
            result = (uint32_t)SpuMalloc(((int32_t)(uint32_t)(size * channels)));
            ++index;
            if (((int32_t)(uint32_t)(result)) < 0)
            {
                (void)RRJ_DEBUG_PRINTF((const char *)xport_guest_cptr(0x80010b10, 26));
                return result;
            }
            w_u32(cursor + 32, result);
            count = r_u32(0x800d754c);
            cursor += 20;
        } while (index < count);
    return 0;
}

uint32_t sub_800211F4(void)
{
    FUNCTION_MARKER(0x800211F4u, "SLUS_010.53");
    uint32_t size = r_u32(0x800d7580), count, channels, value;
    uint32_t index = 0, cursor = 0x800d7548, result = 1;
    w_u8(0x800d7548, 0);
    count = r_u32(0x800d754c);
    channels = r_u32(0x800d7584);
    w_u8(0x800d7549, 1);
    w_u8(0x800d754a, 0);
    w_u8(0x800d754b, 0);
    w_u8(0x800d75a1, 0);
    w_u32(0x800d7554, 0);
    w_u32(0x800d758c, 0);
    w_u32(0x800d7590, 0);
    w_u32(0x800d7594, 1);
    w_u32(0x800d759c, 0);
    w_u8(0x800d75a0, 1);
    w_u32(0x800d7588, channels);
    w_u32(0x800d7598, size);
    if (count)
        do
        {
            w_u32(cursor + 16, 0);
            value = r_u32(0x800d6c14);
            w_u32(cursor + 20, value);
            w_u32(cursor + 24, (index & 1) ? 127 : 0);
            count = r_u32(0x800d754c);
            ++index;
            result = index < count;
            cursor += 20;
        } while (result);
    w_u32(0x8005B4C8u, 0);
    return result;
}

uint32_t sub_80023498(void)
{
    char name[16];
    uint32_t context, handle, result;
    FUNCTION_MARKER(0x80023498u, "SLUS_010.53");
    context = r_u32(0x8005B2F8u);
    sprintf(name, (const char *)xport_guest_cptr(0x8005AE38u, 1), (const char *)xport_guest_cptr(0x8005AE40u, 1), ((int32_t)(uint32_t)(r_u32(context + 48u))), (const char *)xport_guest_cptr(0x8005AE48u, 1));
    sub_80023F08(0x8005B4F8u, name);
    context = r_u32(0x8005B2F8u);
    sprintf(name, (const char *)xport_guest_cptr(0x8005AE38u, 1), (const char *)xport_guest_cptr(0x8005AE40u, 1), ((int32_t)(uint32_t)(r_u32(context + 48u))), (const char *)xport_guest_cptr(0x8005AE50u, 1));
    if (!sub_8002428C(name))
        w_u32(0x8005B518u, 0u);
    context = r_u32(0x8005B2F8u);
    w_u32(0x8005B50Cu, 0u);
    w_u32(0x8005B508u, 0u);
    sprintf(name, (const char *)xport_guest_cptr(0x8005AE38u, 1), (const char *)xport_guest_cptr(0x8005AE40u, 1), ((int32_t)(uint32_t)(r_u32(context + 48u))), (const char *)xport_guest_cptr(0x8005AE58u, 1));
    handle = sub_80023100(name);
    w_u32(0x8005B4FCu, handle);
    w_u32(0x8005B514u, 0u);
    result = sub_800148BC(handle);
    w_u32(0x8005B510u, result);
    sub_8002379C(0u);
    sub_80024354();
    sub_8002379C(1u);
    result = sub_80024354();
    w_u32(0x8005B500u, 0u);
    return result;
}

uint32_t sub_80023F08(uint32_t output, const char *name)
{
    FUNCTION_MARKER(0x80023F08u, "SLUS_010.53");
    uint32_t handle, size, buffer, result, address, value;
    handle = (uint32_t)sub_80023100(name);
    size = (uint32_t)sub_800148BC(handle);
    buffer = (uint32_t)sub_8001447C(size, 0);
    result = (uint32_t)sub_80014780(handle, xport_guest_ptr(buffer, 1), size);
    (void)(uint32_t)sub_8001460C(handle);
    if (((int32_t)(uint32_t)(result)) < 0)
        return 0;
    w_u32(output, buffer);
    value = r_u32(buffer + 24);
    w_u32(buffer + 24, buffer + value);
    address = r_u32(output);
    value = r_u32(address + 20);
    w_u32(address + 20, buffer + value);
    return 1;
}

uint32_t sub_8002428C(const char *name)
{
    FUNCTION_MARKER(0x8002428Cu, "SLUS_010.53");
    uint32_t handle, size, buffer, result, cursor, count, index, value;
    handle = (uint32_t)sub_80023100(name);
    if (((int32_t)(uint32_t)(handle)) < 0)
        return 0;
    size = (uint32_t)sub_800148BC(handle);
    buffer = (uint32_t)sub_8001447C(size, 0);
    result = (uint32_t)sub_80014780(handle, xport_guest_ptr(buffer, 1), size);
    (void)(uint32_t)sub_8001460C(handle);
    cursor = buffer + 8;
    if (((int32_t)(uint32_t)(result)) < 0)
        return 0;
    w_u32(0x8005AE30u, buffer);
    value = r_u8(buffer + 4);
    w_u32(0x8005B518u, cursor);
    count = (value - 8) >> 2;
    index = 0;
    if (count)
        do
        {
            value = r_u32(cursor);
            ++index;
            w_u32(cursor, value + buffer);
            cursor += 4;
        } while (((int32_t)(uint32_t)(index)) < ((int32_t)(uint32_t)(count)));
    return 1;
}

uint32_t sub_80024354(void)
{
    FUNCTION_MARKER(0x80024354u, "SLUS_010.53");
    uint32_t address = r_u32(0x8005AE34u);
    w_u32(address + 4, 0);
    w_u32(address + 44, 0);
    w_u32(address + 60, 0);
    w_u32(address + 32, 0);
    w_u32(address + 72, UINT32_MAX);
    w_u32(address + 52, UINT32_MAX);
    w_u32(address + 92, 0);
    w_u32(address + 96, 0);
    w_u32(address + 76, 0);
    w_u32(address + 40, 0);
    w_u32(address + 36, 0);
    (void)(uint32_t)sub_80023DA4(address + 80);
    address = r_u32(0x8005AE34u);
    w_u32(address + 100, 0);
    w_u32(address + 104, 0);
    w_u32(address + 108, 0);
    return address;
}

uint32_t sub_80023DA4(uint32_t address)
{
    FUNCTION_MARKER(0x80023DA4u, "SLUS_010.53");
    w_u32(address + 8, 0);
    w_u32(address, 0xffffffffu);
    w_u32(address + 4, 0xffffffffu);
    return 0xffffffffu;
}

uint32_t sub_80023020(void)
{
    FUNCTION_MARKER(0x80023020u, "SLUS_010.53");
    uint32_t address = r_u32(0x800d6184), a0, a1, a2;
    a0 = r_u32(address);
    a1 = (uint32_t)r_s16(address + 6);
    a2 = r_u32(address + 8);
    (void)(uint32_t)sub_800235C8(a0, a1, a2);
    return sub_80023714();
}

uint32_t sub_800235C8(uint32_t first, uint32_t second, uint32_t third)
{
    FUNCTION_MARKER(0x800235C8u, "SLUS_010.53");
    uint32_t context = r_u32(0x8005b2f8), count, index = 0;
    uint32_t record, data, value, target, result;
    count = r_u32(context + 48);
    if (!count)
        return 0;
    do
    {
        (void)(uint32_t)sub_8002379C(index);
        record = r_u32(0x8005AE34u);
        w_u32(record + 72, first);
        w_u32(record + 68, second);
        w_u32(record + 64, third);
        data = (uint32_t)sub_800245DC(first);
        record = r_u32(0x8005AE34u);
        w_u32(record + 92, data);
        value = r_u32(data + 4);
        w_u32(record + 96, value >> 6);
        value = r_u32(record + 64);
        data = r_u32(record + 92);
        value = r_u32(data + (((int32_t)(uint32_t)(value)) > 0 ? 12 : 8));
        target = r_u32(0x8005AE34u);
        w_u32(record + 52, value);
        (void)(uint32_t)sub_80023DB8(target + 80);
        context = r_u32(0x8005b2f8);
        count = r_u32(context + 48);
        ++index;
        result = index < count;
    } while (result);
    return result;
}

uint32_t sub_80023714(void)
{
    char name[16];
    uint32_t context, mode, sequence, count, index = 0u, result;
    FUNCTION_MARKER(0x80023714u, "SLUS_010.53");
    context = r_u32(0x8005B2F8u);
    mode = r_u32(context + 48u);
    sequence = r_u32(context + 64u);
    sprintf(name, (const char *)xport_guest_cptr(0x80010BC8u, 1), (int)((int32_t)(uint32_t)(mode)), (long)((int32_t)(uint32_t)(sequence)));
    context = r_u32(0x8005B2F8u);
    count = r_u32(context + 48u);
    if (!count)
        return count;
    do
    {
        sub_8002379C(index);
        sub_80024168(name);
        context = r_u32(0x8005B2F8u);
        count = r_u32(context + 48u);
        ++index;
        result = index < count;
    } while (result);
    return result;
}

uint32_t sub_80024168(const char *name)
{
    FUNCTION_MARKER(0x80024168u, "SLUS_010.53");
    uint8_t header[40] = {0};
    uint32_t handle, result, first, second, address, flags, index = 0;
    handle = (uint32_t)sub_80023100(name);
    if (((int32_t)(uint32_t)(handle)) < 0)
        return 0;
    result = (uint32_t)sub_80014780(handle, header, 40u);
    if (((int32_t)(uint32_t)(result)) < 0)
        return 0;
    first = xport_load_le32(header + 20u);
    address = r_u32(0x8005AE34u);
    second = xport_load_le32(header + 24u);
    (void)(uint32_t)sub_80023C24(address + 8, handle, first, second);
    address = r_u32(0x8005AE34u);
    flags = r_u32(address);
    w_u32(address + 56, 0);
    w_u32(address + 32, 1);
    flags <<= 16;
    do
    {
        address = r_u32(0x8005AE34u);
        result = (uint32_t)sub_80023300(address + 8);
        if (result)
            break;
        address = r_u32(0x8005AE34u);
        (void)(uint32_t)sub_80023148(address + 8, 64, 0, flags);
        (void)(uint32_t)sub_80022A78(2);
        result = (uint32_t)sub_80022A78(2);
        if (result)
        {
            (void)(uint32_t)sub_800229F4(2);
            (void)(uint32_t)sub_800229B0();
        }
        ++index;
        (void)(uint32_t)sub_80030608();
    } while (((int32_t)(uint32_t)(index)) < 3);
    address = r_u32(0x8005AE34u);
    w_u32(address + 32, 0);
    (void)(uint32_t)sub_80023300(address + 8);
    (void)(uint32_t)sub_8001460C(handle);
    (void)sub_80023DE4();
    first = xport_load_le32(header + 36u);
    (void)(uint32_t)sub_800243BC(first);
    return 1;
}

uint32_t sub_80023C24(uint32_t address, uint32_t handle, uint32_t offset, uint32_t size)
{
    FUNCTION_MARKER(0x80023C24u, "SLUS_010.53");
    w_u32(address + 4, offset);
    w_u32(address + 12, offset);
    w_u32(address + 8, 16);
    w_u32(address, handle);
    w_u32(address + 16, offset + size);
    w_u32(address + 20, 1);
    return 1;
}

void sub_80033DAC(uint32_t values[3], uint32_t auxiliary[3])
{
    uint32_t second, first, third;
    FUNCTION_MARKER(0x80033DACu, "SLUS_010.53");
    second = xport_load_le32((uint8_t *)values + 4);
    if (((int32_t)(uint32_t)(second)) < 0)
        return;
    first = xport_load_le32(values);
    if (((int32_t)(uint32_t)(first)) < ((int32_t)(uint32_t)(second)))
        (void)sub_80033D70(values, auxiliary, 0, 1);
    third = xport_load_le32((uint8_t *)values + 8);
    if (((int32_t)(uint32_t)(third)) < 0)
        return;
    second = xport_load_le32((uint8_t *)values + 4);
    if (((int32_t)(uint32_t)(second)) < ((int32_t)(uint32_t)(third)))
    {
        (void)sub_80033D70(values, auxiliary, 1, 2);
        second = xport_load_le32((uint8_t *)values + 4);
        first = xport_load_le32(values);
        if (((int32_t)(uint32_t)(first)) < ((int32_t)(uint32_t)(second)))
            (void)sub_80033D70(values, auxiliary, 0, 1);
    }
}
