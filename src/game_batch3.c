#include "rrj_debug.h"
#include "game_main.h"
#include "race_leaf.h"
#include "race_pause.h"
#include "race_bodyless_batch_004.h"
#include "xport.h"
#include "gpu.h"
#include "psx.h"
#include "menu_latch.h"
#include "fixed_math.h"
#include <stdio.h>
#include <stdlib.h>

uint32_t sub_8005D338(void)
{
    uint32_t owner;
    uint32_t count;
    uint32_t buffer;

    FUNCTION_MARKER(0x8005D338, "RASHCDI.BIN");
    owner = r_u32(0x8005ACBC);
    if (r_u32(owner + 2620))
        return 1;
    count = r_u32(owner + 2648);
    w_u32(owner + 2620, 1);
    buffer = sub_8001447C(count << 14, 0);
    w_u32(r_u32(0x8005ACBC) + 2616, buffer);
    return buffer;
}

uint32_t sub_8005D38C(void)
{
    uint32_t owner;
    uint32_t buffer;
    uint32_t index = 0;
    uint32_t result;

    FUNCTION_MARKER(0x8005D38C, "RASHCDI.BIN");
    owner = r_u32(0x8005ACBC);
    result = r_u32(owner + 2648);
    buffer = r_u32(owner + 2616);
    if ((int32_t)result > 0)
    {
        do
        {
            uint32_t record = owner + 44 + 36 * index;

            w_u32(record, 0);
            w_u32(record + 12, buffer);
            buffer += 16384;
            w_u32(record + 4, index);
            ++index;
            w_u32(record + 16, 0);
            w_u32(record + 20, 0);
            w_u32(record + 24, 0);
            w_u32(record + 32, 0);
            result = (int32_t)index < r_s32(owner + 2648);
        } while (result);
    }
    return result;
}

uint32_t sub_8005D3F4(void)
{
    uint32_t owner;

    FUNCTION_MARKER(0x8005D3F4, "RASHCDI.BIN");
    owner = r_u32(0x8005ACBC);
    w_u32(owner + 2352, 0);
    w_u32(owner + 2348, 0);
    w_u32(owner + 2356, 0);
    return owner;
}

uint32_t sub_8005D410(void)
{
    uint32_t owner;
    uint32_t count;

    FUNCTION_MARKER(0x8005D410, "RASHCDI.BIN");
    if (r_u32(r_u32(0x8005B2F8) + 48) == 1)
    {
        owner = r_u32(0x8005ACBC);
        w_u32(owner + 2628, 29);
        w_u32(owner + 2640, 30);
        w_u32(owner + 2644, 31);
        w_u32(owner + 2648, 32);
        w_u32(owner + 2624, 0);
        w_u32(owner + 2636, 0);
        w_u32(owner + 2632, 0);
        w_u32(owner + 2652, 2);
    }
    else
    {
        owner = r_u32(0x8005ACBC);
        w_u32(owner + 2628, 15);
        w_u32(owner + 2632, 16);
        w_u32(owner + 2636, 31);
        w_u32(owner + 2624, 0);
        w_u32(owner + 2644, 0);
        w_u32(owner + 2640, 0);
        w_u32(owner + 2648, 32);
        w_u32(owner + 2652, 0);
    }
    owner = r_u32(0x8005ACBC);
    count = r_u32(owner + 2648);
    w_u32(owner, 0);
    w_u32(owner + 4, 0);
    w_u32(owner + 8, 0);
    w_u32(owner + 12, 0);
    w_u32(owner + 20, 0);
    w_u32(owner + 24, 0);
    w_u32(owner + 32, 0);
    w_u32(owner + 36, 0);
    w_u32(owner + 40, 0);
    w_u32(owner + 28, 0);
    w_u32(owner + 16, count);
    (void)sub_8005D338();
    (void)sub_8005D38C();
    return sub_8005D3F4();
}

uint32_t sub_8005D500(void)
{
    FUNCTION_MARKER(0x8005D500, "RASHCDI.BIN");
    return sub_8005D410();
}

void sub_8005D330(void)
{
    FUNCTION_MARKER(0x8005D330, "RASHCDI.BIN");
}

uint32_t sub_8005EBDC(void)
{
    uint32_t display;

    FUNCTION_MARKER(0x8005EBDC, "RASHCDI.BIN");
    display = sub_8001C498();
    w_u32(0x8005AD14, r_u32(r_u32(0x8005B2F8) + 48) == 2 ? display : 0xFFFFFFFF);
    return 0x80060000;
}

uint32_t sub_8005ED94(void)
{
    uint32_t buffer;
    uint32_t state;
    uint32_t index;
    uint32_t display;

    FUNCTION_MARKER(0x8005ED94, "RASHCDI.BIN");
    buffer = sub_8001447C(3960 * r_u32(r_u32(0x8005B2F8) + 48), 0);
    w_u32(0x8005B2F0, buffer);
    if (!buffer)
        return buffer;
    state = r_u32(0x8005B2F8);
    for (index = 0; (int32_t)index < r_s32(state + 48); ++index)
    {
        w_u32(0x8005B288 + 4 * index, 0);
        w_u32(0x8005ACEC + 4 * index, buffer);
        buffer += 3960;
    }
    w_u32(0x8005B388, 0);
    for (index = 0; index < 22; ++index)
    {
        w_u16(0x800D49B2 + 8 * index, 65535);
        w_u16(0x800D49B0 + 8 * index, 65535);
    }
    for (index = 0; index < 62; ++index)
    {
        w_u16(0x800D45D6 + 16 * index, 65535);
        w_u16(0x800D45DA + 16 * index, 65535);
    }
    state = r_u32(0x8005B2F8);
    for (index = 0; (int32_t)index < r_s32(state + 48); ++index)
    {
        uint32_t record = r_u32(0x8005ACEC + 4 * index);
        uint32_t slot;

        for (slot = 0; slot < 110; ++slot)
        {
            w_u32(record + 20, 0);
            w_u16(record + 30, 0);
            w_u16(record + 28, 0);
            record += 36;
        }
    }
    w_u32(0x8005AD10, 0);
    w_u32(0x8005AD0C, 0);
    w_u32(0x8005ACD8, 1);
    w_u32(0x8005ACDC, 0);
    (void)sub_8005EBDC();
    display = sub_8001C498();
    w_u32(0x8005AD14, r_u32(r_u32(0x8005B2F8) + 48) == 2 ? display : 0xFFFFFFFF);
    return 0x80060000;
}

uint32_t sub_8005D568(uint32_t count)
{
    uint32_t buffer;

    FUNCTION_MARKER(0x8005D568, "RASHCDI.BIN");
    buffer = sub_8001447C(21 * count, 0);
    w_u32(0x8005ACB0, buffer);
    w_u32(0x8005ACB4, buffer + 16 * count);
    w_u32(0x8005ACB8, buffer + 20 * count);
    return buffer;
}

uint32_t sub_8005E930(void)
{
    uint32_t value = 0;
    uint32_t index;

    FUNCTION_MARKER(0x8005E930, "RASHCDI.BIN");
    for (index = 0; index < 4; ++index)
    {
        value <<= 4;
        while (!(value & 2))
            value |= (sub_80043F00(0xF2000002) & 255) >> 6;
    }
    return value & 65535;
}

uint32_t sub_8005E4E8(uint32_t players)
{
    uint32_t buffer;
    uint32_t bytes;
    uint32_t table;
    uint32_t flags;
    uint32_t y;
    uint16_t rectangle[4];

    FUNCTION_MARKER(0x8005E4E8, "RASHCDI.BIN");
    if ((int32_t)sub_80014654((const char *)xport_guest_cptr(0x8005B6BC, 1), r_u32(0x8005AD5C), &buffer, &bytes) < 0)
        return 0xFFFFFFFF;
    table = 0x800533B4 + 88 * (players - 1);
    flags = r_u8(table + 34);
    rectangle[0] = (uint16_t)((flags & 15) << 6);
    flags = r_u8(table + 34);
    y = r_u8(table + 33);
    rectangle[2] = 64;
    rectangle[3] = 128;
    y += 16 * (flags & 16);
    w_u32(0x800D6168, y);
    rectangle[1] = (uint16_t)y;
    (void)rrj_gpu_upload((const uint8_t *)rectangle, buffer);
    (void)sub_800144B8(buffer);
    w_u16(0x800D6160, ((rectangle[1] & 256) >> 4) | ((rectangle[0] & 1023) >> 6) | ((rectangle[1] & 512) << 2));
    return 0;
}

uint32_t sub_8005E6CC(const char *path, uint32_t players)
{
    uint32_t destination;
    uint32_t buffer;
    uint32_t table;
    uint32_t flags;
    uint32_t count;
    uint32_t slot;
    uint32_t offset;
    uint32_t dimensions;
    uint16_t rectangle[4];
    uint8_t record[20];

    FUNCTION_MARKER(0x8005E6CC, "RASHCDI.BIN");
    destination = 0x800D5F88 + 12 * r_u32(0x800D6108);
    if (sub_8001408C(path, &buffer, record))
        return 0xFFFFFFFF;
    table = 0x800533B4 + 88 * (players - 1);
    flags = r_u8(table + 58);
    count = r_u32(0x800D6108);
    rectangle[0] = (uint16_t)(((flags & 15) << 6) + ((16 * count) & 960));
    slot = count & 3;
    offset = 60 * slot;
    flags = r_u8(table + 58);
    rectangle[1] = (uint16_t)(r_u8(table + 57) + 16 * (flags & 16) + offset);
    dimensions = xport_load_le32(record + 12);
    rectangle[2] = r_u16(dimensions + 4);
    rectangle[3] = r_u16(dimensions + 6);
    w_u8(destination, 255);
    w_u8(destination + 1, slot);
    w_u8(destination + 2, 0);
    w_u8(destination + 3, offset);
    w_u16(destination + 8, ((rectangle[1] & 256) >> 4) | ((rectangle[0] & 1023) >> 6) | 128 | ((rectangle[1] & 512) << 2));
    w_u16(destination + 6, rectangle[0]);
    (void)rrj_gpu_upload((const uint8_t *)rectangle, xport_load_le32(record + 16));
    count = r_u32(0x800D6108);
    w_u32(0x8005B348, destination);
    w_u32(0x800D6108, count + 1);
    w_u16(destination + 10, 65535);
    (void)sub_800144B8(buffer);
    return 0;
}

static uint32_t sub_8005E848(uint32_t players)
{
    uint32_t limit;
    int32_t index;

    FUNCTION_MARKER(0x8005E848, "RASHCDI.BIN");
    w_u32(0x800D6108, 0);
    w_u32(0x800D610C, 0);
    w_u32(0x800D6110, 0);
    w_u32(0x800D6114, 0);
    w_u32(0x800D6118, 0);
    limit = 0x8005AE08 + 4 * (players - 1);
    index = r_s32(limit);
    w_u32(0x8005B2E4, 0x800D5F70);
    if (index > 0)
    {
        index = 0;
        do
        {
            w_u8(0x800D5F88 + 12 * (uint32_t)index, 255);
            ++index;
        } while (index < r_s32(limit));
    }
    limit = 0x8005AE10 + 4 * (players - 1);
    if (r_s32(limit) > 0)
    {
        index = 0;
        do
        {
            w_u8(0x800D60A8 + 12 * (uint32_t)index, 255);
            ++index;
        } while (index < r_s32(limit));
    }
    w_u8(0x800D5F70, 255);
    w_u8(0x800D5F7C, 255);
    return sub_8005E6CC((const char *)xport_guest_cptr(0x8005B6D0, 1), players);
}

uint32_t sub_8005D9EC(void)
{
    FUNCTION_MARKER(0x8005D9EC, "RASHCDI.BIN");
    (void)sub_8005E848(r_u32(r_u32(0x8005B2F8) + 48));
    return sub_8005E4E8(r_u32(r_u32(0x8005B2F8) + 48));
}

static uint32_t sub_8005D63C(uint32_t players)
{
    uint32_t record;
    uint32_t group;
    uint32_t player;
    uint32_t index;
    uint32_t table = 0x800533B4 + 88 * (players - 1);

    FUNCTION_MARKER(0x8005D63C, "RASHCDI.BIN");
    for (record = 0x800D87E8; (int32_t)record < (int32_t)0x800D9268; record += 112)
    {
        w_u32(record, 0xFFFFFFFF);
        for (index = 0; index < 2; ++index)
        {
            w_u32(record + 104 + 4 * index, 0);
            w_u32(record + 96 + 4 * index, 0);
            w_u32(record + 80 + 4 * index, 0xFFFFFFFF);
            w_u32(record + 88 + 4 * index, 0xFFFFFFFF);
        }
        w_u32(record + 8, 0xFFFFFFFF);
        w_u32(record + 12, 0);
        w_u32(record + 76, 0);
        w_u32(record + 56, 0);
    }
    player = 0;
    for (index = 0; index < 12; ++index)
    {
        for (player = 0; (int32_t)player < (int32_t)players; ++player)
            w_u32(0x800D9B80 + 4 * index + 48 * player, 0xFFFFFFFF);
    }
    w_u32(0x8005B568 + 4 * player, 0);
    for (index = 0; index < 48; ++index)
    {
        record = 0x800D9268 + 48 * index;
        w_u32(record + 8, 0xFFFFFFFF);
        w_u32(record + 12, 0);
        w_u32(record + 16, 0);
        w_u32(record, 0xFFFFFFFF);
        w_u32(record + 20, 0xFFFFFFFF);
        (void)sub_8001E100(record + 24, 0xFFFFFFFF, 24);
    }
    for (group = 0; group < 3; ++group)
    {
        for (player = 0; (int32_t)player < (int32_t)players; ++player)
        {
            record = 0x800D7710 + 8 * group + 24 * player;
            w_u32(record, 0xFFFFFFFF);
            w_u16(record + 4, ((((r_u8(table + 4 * player + 2) & 15) << 6) + ((group & 15) << 6)) & 1023) >> 6);
        }
    }
    for (group = 0; group < 4; ++group)
    {
        for (player = 0; (int32_t)player < (int32_t)players; ++player)
        {
            record = 0x800D76D0 + 8 * group + 32 * player;
            w_u32(record, 0xFFFFFFFF);
            w_u16(record + 4, ((((r_u8(table + 8 + 4 * player + 2) & 15) << 6) + ((32 * group) & 960)) & 1023) >> 6);
        }
    }
    w_u32(0x8005AEDC, 0);
    w_u16(0x800D6162, 30720);
    w_u32(0x8005AEE4, players == 1 ? 3 : 2);
    w_u32(0x8005AEE8, 4);
    return 4;
}

uint32_t sub_8005E9A0(uint32_t players)
{
    uint32_t pixels[8] = {0};
    PSX_RECT rectangle;
    uint32_t table = 0x800533B4 + 88 * (players - 1);
    uint32_t row;
    uint32_t column;
    uint32_t index;
    uint32_t flags;

    FUNCTION_MARKER(0x8005E9A0, "RASHCDI.BIN");
    rectangle.h = 8;
    rectangle.w = 2;
    flags = r_u8(table + 86);
    rectangle.y = (int16_t)(r_u8(table + 85) + 16 * (flags & 16) + 2);
    for (row = 0; row < 4; ++row)
    {
        for (column = 0; column < 24; ++column)
        {
            rectangle.x = (int16_t)(((r_u8(table + 86) & 15) << 6) + 8 + 2 * column);
            for (index = 0; index < 4; ++index)
                pixels[index] = (sub_8005E930() << 16) | 0x1111;
            for (index = 4; index < 8; ++index)
                pixels[index] = sub_8005E930() | 0x11110000;
            (void)LoadImagePSX(&rectangle, pixels);
        }
        rectangle.y = (int16_t)((uint16_t)rectangle.y + 8);
    }
    pixels[0] = 0x10000;
    pixels[1] = 0x6F7B739C;
    for (index = 2; index < 8; ++index)
        pixels[index] = 0;
    flags = r_u8(table + 86);
    rectangle.x = (int16_t)(((flags & 15) << 6) + 16);
    flags = r_u8(table + 86);
    rectangle.w = 16;
    rectangle.h = 1;
    rectangle.y = (int16_t)(r_u8(table + 85) + 16 * (flags & 16));
    return (uint32_t)LoadImagePSX(&rectangle, pixels);
}

uint32_t sub_8005D8D0(void)
{
    FUNCTION_MARKER(0x8005D8D0, "RASHCDI.BIN");
    (void)sub_8005D568(1200);
    (void)sub_8005D63C(r_u32(r_u32(0x8005B2F8) + 48));
    return sub_8005E9A0(r_u32(r_u32(0x8005B2F8) + 48));
}

uint32_t sub_80063500(void)
{
    uint32_t index;

    FUNCTION_MARKER(0x80063500, "RASHCDI.BIN");
    w_u8(r_u32(0x8005B2F8), 3);
    (void)sub_80018C1C(1);
    (void)sub_80020E30(1);
    (void)sub_8005D9EC();
    (void)sub_8005BE40();
    (void)sub_8005D5E4();
    (void)sub_8001C1AC();
    (void)sub_8005D8D0();
    (void)sub_80063E48();
    (void)sub_8005ED94();
    for (index = 0; index < 6; ++index)
        (void)sub_8001E100(0x800D81F0 + 36 * index, 0, 8);
    (void)sub_80063C20();
    (void)sub_8002D2E0();
    (void)sub_80063C00();
    sub_8005D330();
    return sub_8005D500();
}

uint32_t sub_80063D94(const char *name)
{
    char path[128];
    uint32_t bytes;
    uint32_t result;
    uint32_t data;
    uint32_t strings;
    uint32_t index;
    int32_t count;
    int32_t length;

    FUNCTION_MARKER(0x80063D94, "RASHCDI.BIN");
    length = snprintf(path, sizeof(path), "%s%s", (const char *)xport_guest_cptr(0x80052478, 1), name);
    if (length < 0 || (uint32_t)length >= sizeof(path))
    {
        RRJ_DEBUG_FPUTS("Race string-table path is too long\n", stderr);
        abort();
    }
    result = sub_80014654(path, r_u32(0x8005AD5C), xport_guest_ptr(0x8005AE78, 4), &bytes);
    if ((int32_t)result < 0)
        return result;
    data = r_u32(0x8005AE78);
    strings = data + r_u32(data + 16) + 16;
    count = r_s32(data + 32);
    if (count > 0)
    {
        index = 0;
        do
        {
            uint32_t base;
            uint32_t offset;

            ++index;
            base = data + r_u32(data + 16);
            offset = r_u32(strings + 4 * (index - 1));
            w_u32(strings + 4 * (index - 1), offset + base);
        } while ((int32_t)index < r_s32(data + 32));
    }
    w_u32(0x8005B544, strings);
    return 0x80060000;
}

uint32_t sub_800655D4(const char *path)
{
    uint32_t handle;

    FUNCTION_MARKER(0x800655D4, "RASHCDI.BIN");
    handle = sub_8001458C(path, r_u32(0x8005AD5C));
    if ((int32_t)handle < 0)
        return (uint32_t)RRJ_DEBUG_PRINTF("Cannot open %s for reading\n", path);
    (void)sub_80014780(handle, xport_guest_ptr(0x8006B8B8, 12888), 12888);
    return sub_8001460C(handle);
}

uint32_t sub_800674F8(void)
{
    char path[128];
    char language[3];
    int32_t length;

    FUNCTION_MARKER(0x800674F8, "RASHCDI.BIN");
    xport_store_le16(language, r_u16(0x8005B8EC));
    language[2] = (char)r_u8(0x8005B8EE);
    length = snprintf(path, sizeof(path), "%s.%s", (const char *)xport_guest_cptr(0x800524B4, 1), language);
    if (length < 0 || (uint32_t)length >= sizeof(path))
    {
        RRJ_DEBUG_FPUTS("Race localization path is too long\n", stderr);
        abort();
    }
    (void)sub_800655D4(path);
    (void)sub_8001E0B4(xport_guest_ptr(0x800D38E0, 204), xport_guest_cptr(0x8006B8B8, 204), 204);
    return 0x800D38E0;
}

uint32_t sub_80061994(void)
{
    FUNCTION_MARKER(0x80061994, "RASHCDI.BIN");
    w_u32(0x8005B308, 0);
    return 0x80060000;
}

uint32_t sub_800611C4(void)
{
    uint32_t first_red;
    uint32_t first_green;
    uint32_t first_blue;
    uint32_t second_red;
    uint32_t second_green;
    uint32_t second_blue;

    FUNCTION_MARKER(0x800611C4, "RASHCDI.BIN");
    w_u32(0x8005AD28, 1);
    w_u16(0x800523D0, 0);
    first_red = r_u8(0x800523F8);
    second_red = r_u8(0x800523FC);
    w_u16(0x800523DE, 240);
    w_u16(0x800523DA, 240);
    w_u16(0x800523D4, 384);
    w_u16(0x800523D8, 384);
    w_u16(0x800523D2, 0);
    w_u16(0x800523D6, 0);
    w_u16(0x800523DC, 0);
    w_u8(0x800523E0, first_red);
    first_green = r_u8(0x800523F9);
    first_blue = r_u8(0x800523FA);
    second_green = r_u8(0x800523FD);
    second_blue = r_u8(0x800523FE);
    w_u8(0x800523E4, first_red);
    w_u8(0x800523EC, second_red);
    w_u8(0x800523E8, second_red);
    w_u8(0x800523E1, first_green);
    w_u8(0x800523E2, first_blue);
    w_u8(0x800523E5, first_green);
    w_u8(0x800523E6, first_blue);
    w_u8(0x800523ED, second_green);
    w_u8(0x800523EE, second_blue);
    w_u8(0x800523E9, second_green);
    w_u8(0x800523EA, second_blue);
    return 0x800523E0;
}

uint32_t sub_80062384(uint32_t header, uint32_t data)
{
    FUNCTION_MARKER(0x80062384, "RASHCDI.BIN");
    w_u8(0x800523F8, r_u8(data));
    w_u8(0x800523F9, r_u8(data + 1));
    w_u8(0x800523FA, r_u8(data + 2));
    w_u8(0x800523FC, r_u8(data + 4));
    w_u8(0x800523FD, r_u8(data + 5));
    w_u8(0x800523FE, r_u8(data + 6));
    w_u8(0x800523F0, r_u8(data + 8));
    w_u8(0x800523F1, r_u8(data + 9));
    w_u8(0x800523F2, r_u8(data + 10));
    w_u8(0x800523F4, r_u8(data + 12));
    w_u8(0x800523F5, r_u8(data + 13));
    w_u8(0x800523F6, r_u8(data + 14));
    return sub_800611C4();
}

uint32_t sub_80062430(uint32_t header, uint32_t data)
{
    uint32_t width;
    uint32_t blue;
    uint32_t green;
    uint32_t red;
    uint32_t result;

    FUNCTION_MARKER(0x80062430, "RASHCDI.BIN");
    w_u32(0x8005237C, r_u32(data + 1152));
    width = (r_u16(data + 1156) - 256) & 65535;
    w_u16(0x80052380, width);
    w_u32(0x80052384, 256 - width);
    (void)sub_8001E0B4(xport_guest_ptr(0x800D4CA8, 1152), xport_guest_cptr(data, 1152), 1152);
    blue = r_u8(0x800D4CAA);
    green = r_u8(0x800D4CA9);
    red = r_u8(0x800D4CA8);
    result = ((255 - green) / 6) << 8;
    w_u32(0x80052348, (((255 - blue) / 6) << 16) | result | ((255 - red) / 6));
    return result;
}

uint32_t sub_8006250C(uint32_t header, uint32_t data)
{
    uint32_t yaw;
    uint32_t pitch;
    uint32_t quarter;
    uint32_t vector[3];
    uint32_t result;
    uint32_t blue;
    uint32_t green;
    uint32_t red;

    FUNCTION_MARKER(0x8006250C, "RASHCDI.BIN");
    yaw = 11 * r_u32(data + 28);
    pitch = r_u32(data + 32);
    w_u16(0x80052364, 0 - r_u16(0x8005624C + 4 * (yaw & 4095)));
    pitch *= 11;
    w_u16(0x80052368, 0 - r_u16(0x8005624C + (4 * (yaw & 4095) | 2)));
    quarter = (int32_t)pitch < 0 ? pitch + 3 : pitch;
    w_u16(0x80052366, r_u16(0x8005624C + (quarter & 16380)));
    (void)sub_8002EE50(0xFF9C0000, xport_guest_cptr(0x80052364, 6), vector);
    w_u16(0x8005234C, (uint32_t)((int32_t)vector[0] >> 10));
    w_u16(0x8005234E, (uint32_t)((int32_t)vector[1] >> 10));
    w_u16(0x80052350, (uint32_t)((int32_t)vector[2] >> 10));
    result = r_u16(0x8005624C + 4 * (pitch & 4095));
    w_u32(0x80052388, yaw);
    w_u16(0x80052366, result);
    w_u32(0x8005238C, (uint32_t)((int32_t)quarter >> 2));
    w_u32(0x80052354, r_u32(data + 24));
    w_u32(0x80052358, r_u32(data));
    w_u32(0x8005235C, r_u32(data + 4));
    w_u32(0x80052360, r_u32(data + 8));
    result = r_u32(data + 12);
    w_u32(0x8005236C, result);
    result = (uint32_t)sub_8001FC90((int32_t)(65536 - result), r_s32(data));
    w_u32(0x80052370, result);
    result = (uint32_t)sub_8001FC90((int32_t)(65536 - r_u32(0x8005236C)), r_s32(data + 4));
    w_u32(0x80052374, result);
    result = (uint32_t)sub_8001FC90((int32_t)(65536 - r_u32(0x8005236C)), r_s32(data + 8));
    w_u32(0x80052378, result);
    w_u32(0x80052340, r_u32(data + 16));
    w_u32(0x80052344, r_u32(data + 20));
    blue = r_u8(data + 10);
    green = r_u8(data + 6);
    red = r_u8(data + 2);
    result = (blue << 16) | (green << 8) | red;
    w_u32(0x8005233C, result);
    return result;
}

uint32_t sub_800626E4(uint32_t header, uint32_t data)
{
    FUNCTION_MARKER(0x800626E4, "RASHCDI.BIN");
    (void)sub_8001E0B4(xport_guest_ptr(0x800D83B0, 832), xport_guest_cptr(data, 832), 832);
    return 0x800D83B0;
}

uint32_t sub_800627A8(uint32_t header, uint32_t data)
{
    FUNCTION_MARKER(0x800627A8, "RASHCDI.BIN");
    if (r_u16(header + 2) - 4 == 1528)
    {
        (void)sub_8001E0B4(xport_guest_ptr(0x800D5138, 1528), xport_guest_cptr(data, 1528), 1528);
        w_u32(0x8005AD24, 1);
        return 1;
    }
    w_u32(0x8005AD24, 0);
    return 0x80060000;
}

static uint32_t sub_80060780(uint32_t packet, uint32_t depth, const uint32_t vector[4], int32_t clut_x, int32_t clut_y)
{
    uint32_t shift;
    uint32_t value;
    uint32_t width;

    FUNCTION_MARKER(0x80060780, "RASHCDI.BIN");
    shift = (2 - (depth & 7)) & 31;
    w_u16(packet + 6, getTPage(depth & 7, 0, xport_load_le32(vector), xport_load_le32(vector + 1)));
    w_u16(packet + 2, getClut((uint32_t)clut_x, (uint32_t)clut_y));
    value = (xport_load_le32(vector) & 63) << shift;
    w_u8(packet, value);
    value = (xport_load_le32(vector) & 63) << shift;
    w_u8(packet + 10, value);
    width = xport_load_le32(vector + 2);
    value = r_u8(packet);
    w_u8(packet + 4, value + (width << shift) - 1);
    width = xport_load_le32(vector + 2);
    value = r_u8(packet + 10);
    w_u8(packet + 8, value + (width << shift) - 1);
    w_u8(packet + 1, ((const uint8_t *)vector)[4]);
    w_u8(packet + 5, ((const uint8_t *)vector)[4]);
    value = r_u8(packet + 1) + ((const uint8_t *)vector)[12];
    w_u8(packet + 11, value);
    value = r_u8(packet + 5) + ((const uint8_t *)vector)[12];
    w_u8(packet + 9, value);
    return value;
}

uint32_t sub_80061100(void)
{
    uint32_t angle = 0;
    uint32_t row;
    uint32_t vertex;
    uint32_t step;
    uint32_t height;
    uint32_t table;
    uint32_t position;
    uint32_t cosine;

    FUNCTION_MARKER(0x80061100, "RASHCDI.BIN");
    step = r_u32(0x800D43F8);
    for (row = 0; row < 25; ++row)
    {
        angle &= 0xFFF00;
        if (row == 24)
            angle = 0;
        position = 0x800D443C + 16 * row;
        height = r_u32(0x800D43FC);
        table = 0x8005624C + 4 * ((angle >> 8) & 4095);
        for (vertex = 0; vertex < 2; ++vertex)
        {
            w_u16(position, r_u16(table));
            cosine = r_u16(table + 2);
            w_u16(position + 2, height);
            height += step;
            w_u16(position + 4, cosine);
            position += 8;
        }
        angle += 0xAA00;
    }
    return 0;
}

uint32_t sub_80060F58(uint32_t data, uint32_t players)
{
    uint8_t record[20];
    uint32_t vector[4];
    uint32_t pixels_rectangle;
    uint32_t clut_rectangle;
    uint32_t table;
    uint32_t index;
    uint32_t flags;

    FUNCTION_MARKER(0x80060F58, "RASHCDI.BIN");
    sub_80014044(record, data + 20);
    table = 0x800533B4 + 88 * (players - 1);
    flags = r_u8(table + 50);
    pixels_rectangle = xport_load_le32(record + 12);
    w_u16(pixels_rectangle, (flags & 15) << 6);
    flags = r_u8(table + 50);
    w_u16(pixels_rectangle + 2, r_u8(table + 49) + 16 * (flags & 16));
    (void)rrj_gpu_upload(xport_guest_cptr(pixels_rectangle, 8), xport_load_le32(record + 16));
    flags = r_u8(table + 50);
    clut_rectangle = xport_load_le32(record + 4);
    w_u16(clut_rectangle, (flags & 15) << 6);
    flags = r_u8(table + 50);
    w_u16(clut_rectangle + 2, r_u8(table + 49) + 16 * (flags & 16) + 63);
    (void)rrj_gpu_upload(xport_guest_cptr(clut_rectangle, 8), xport_load_le32(record + 8));
    w_u32(0x800D43F8, r_u32(data + 4));
    w_u32(0x800D43FC, r_u32(data + 8));
    w_u32(0x800D4404, r_u32(data + 12));
    w_u32(0x800D4408, r_u32(data + 16));
    for (index = 0; index < 4; ++index)
    {
        vector[2] = index == 3 ? 15 : 16;
        vector[3] = 62;
        vector[0] = ((r_u8(table + 50) & 15) << 6) + 16 * index;
        flags = r_u8(table + 50);
        vector[1] = 16 * (flags & 16) + r_u8(table + 49);
        (void)sub_80060780(0x800D440C + 12 * index, 8, vector, r_s16(clut_rectangle), r_s16(clut_rectangle + 2));
    }
    (void)sub_80061100();
    w_u32(0x8005B308, 1);
    return 1;
}

uint32_t sub_80061D18(uint32_t header, uint32_t data)
{
    uint32_t players;
    uint32_t table;
    uint32_t index;
    uint32_t image;
    uint32_t dimensions;
    uint32_t flags;
    uint32_t layout;
    uint32_t columns;
    uint32_t quotient;
    uint32_t remainder;
    uint16_t rectangle[4];
    uint8_t record[20];
    int32_t type;

    FUNCTION_MARKER(0x80061D18, "RASHCDI.BIN");
    players = r_u32(r_u32(0x8005B2F8) + 48);
    table = 0x800533B4 + 88 * (players - 1);
    for (index = 0; index < 4; ++index)
    {
        image = data + r_u32(data + 4 * index);
        w_u32(data + 4 * index, image);
        sub_80014044(record, image);
        if (index != 3)
        {
            flags = r_u8(table + 30 - 4 * index);
            rectangle[0] = (uint16_t)((flags & 15) << 6);
            flags = r_u8(table + 30 - 4 * index);
            rectangle[1] = (uint16_t)(r_u8(table + 29 - 4 * index) + 16 * (flags & 16));
            dimensions = xport_load_le32(record + 12);
            rectangle[2] = r_u16(dimensions + 4);
            rectangle[3] = r_u16(dimensions + 6);
            (void)rrj_gpu_upload((const uint8_t *)rectangle, xport_load_le32(record + 16));
            if (!index)
                w_u16(0x8005B370, ((rectangle[1] & 256) >> 4) | ((rectangle[0] & 1023) >> 6) | ((rectangle[1] & 512) << 2));
        }
    }
    for (index = 0; index < 12; ++index)
    {
        image = data + r_u32(data + 32 + 4 * index);
        w_u32(data + 32 + 4 * index, image);
        type = r_s16(0x8006B4EC + 4 * index);
        layout = 0x80053254 + 176 * (players - 1) + 8 * (uint32_t)type;
        columns = r_u8(layout + 4);
        remainder = columns ? index % columns : index;
        remainder *= r_u8(layout + 6);
        flags = r_u8(table + 4 * (uint32_t)type + 2);
        rectangle[0] = (uint16_t)(((flags & 15) << 6) + r_u16(layout) + remainder);
        type = r_s16(0x8006B4EC + 4 * index);
        layout = 0x80053254 + 176 * (players - 1) + 8 * (uint32_t)type;
        columns = r_u8(layout + 4);
        quotient = columns ? index / columns : 0xFFFFFFFF;
        flags = r_u8(table + 4 * (uint32_t)type + 2);
        rectangle[2] = 16;
        rectangle[3] = 1;
        rectangle[1] = (uint16_t)(16 * (flags & 16) + r_u16(layout + 2) + quotient);
        (void)rrj_gpu_upload((const uint8_t *)rectangle, image);
        w_u16(0x800D5EC8 + 2 * index, ((uint32_t)rectangle[1] << 6) | ((rectangle[0] >> 4) & 63));
    }
    return 0;
}

uint32_t sub_80061FAC(uint32_t header, uint32_t data)
{
    uint32_t players;
    uint32_t table;
    uint32_t sprite;
    uint32_t clut_index = 0;
    uint32_t image;
    uint32_t dimensions;
    uint32_t destination;
    uint32_t entry;
    uint32_t flags;
    uint32_t index;
    uint32_t layout;
    uint32_t columns;
    uint32_t remainder;
    uint32_t quotient;
    uint16_t rectangle[4];
    uint8_t record[20];
    uint32_t zeros[16] = {0};

    FUNCTION_MARKER(0x80061FAC, "RASHCDI.BIN");
    players = r_u32(r_u32(0x8005B2F8) + 48);
    table = 0x800533B4 + 88 * (players - 1);
    for (sprite = 0; sprite < 11; ++sprite)
    {
        image = data + r_u32(data + 4 * sprite);
        w_u32(data + 4 * sprite, image);
        destination = 0x800D4270 + 28 * sprite;
        sub_80014044(record, image);
        entry = 0x8006B4C0 + 4 * sprite;
        flags = r_u8(table + 54);
        rectangle[0] = (uint16_t)(r_u8(entry) + ((flags & 15) << 6));
        flags = r_u8(table + 54);
        rectangle[1] = (uint16_t)(r_u8(entry + 1) + r_u8(table + 53) + 16 * (flags & 16));
        dimensions = xport_load_le32(record + 12);
        rectangle[2] = r_u16(dimensions + 4);
        rectangle[3] = r_u16(dimensions + 6);
        w_u32(destination, 4 * r_u8(entry));
        flags = r_u8(table + 54);
        w_u32(destination + 4, 16 * (flags & 16) + r_u8(table + 53) + r_u8(entry + 1));
        w_u32(destination + 8, (uint32_t)(int32_t)(int16_t)rectangle[2] << 2);
        w_u32(destination + 12, (uint32_t)r_s16(dimensions + 6));
        (void)rrj_gpu_upload((const uint8_t *)rectangle, xport_load_le32(record + 16));
        w_u16(destination + 16, ((rectangle[1] & 256) >> 4) | ((rectangle[0] & 1023) >> 6) | (sprite ? 0 : 64) | ((rectangle[1] & 512) << 2));
        w_u16(destination + 18, r_u16(entry + 2));
        for (index = 0; index < r_u16(destination + 18); ++index)
        {
            image = data + r_u32(data + 88 + 4 * clut_index);
            w_u32(data + 88 + 4 * clut_index, image);
            layout = 0x80053254 + 176 * (players - 1);
            columns = r_u8(layout + 108);
            remainder = columns ? clut_index % columns : clut_index;
            remainder *= r_u8(layout + 110);
            flags = r_u8(table + 54);
            rectangle[0] = (uint16_t)(((flags & 15) << 6) + r_u16(layout + 104) + remainder);
            columns = r_u8(layout + 108);
            quotient = columns ? clut_index / columns : 0xFFFFFFFF;
            flags = r_u8(table + 54);
            rectangle[2] = 16;
            ++clut_index;
            rectangle[3] = 1;
            rectangle[1] = (uint16_t)(16 * (flags & 16) + r_u16(layout + 106) + quotient);
            (void)rrj_gpu_upload((const uint8_t *)rectangle, image);
            w_u16(destination + 20 + 2 * index, ((uint32_t)rectangle[1] << 6) | ((rectangle[0] >> 4) & 63));
        }
    }
    flags = r_u8(table + 54);
    w_u16(0x8005B398, ((flags & 15) << 6) + 12);
    flags = r_u8(table + 54);
    index = r_u8(table + 53);
    w_u16(0x8005B39C, 32);
    w_u16(0x8005B39E, 1);
    w_u16(0x8005B39A, index + 16 * (flags & 16) + 120);
    return (uint32_t)LoadImagePSX(xport_guest_ptr(0x8005B398, 8), zeros);
}

void sub_8005BDAC(uint32_t data)
{
    int32_t count;
    uint32_t index;
    uint32_t record;
    uint32_t slot;
    uint32_t destination;

    FUNCTION_MARKER(0x8005BDAC, "RASHCDI.BIN");
    count = r_s32(data + 8);
    for (index = 0; (int32_t)index < count; ++index)
    {
        record = data + 12 + 8 * index;
        slot = 0x800CE560 + 8 * (uint32_t)r_s16(record);
        destination = 4 * index + r_u32(slot + 4);
        w_u16(destination, r_u16(record + 4));
        slot = 0x800CE560 + 8 * (uint32_t)r_s16(record);
        destination = 4 * index + r_u32(slot + 4);
        w_u16(destination + 2, r_u16(record + 6));
        slot = 0x800CE560 + 8 * (uint32_t)r_s16(record);
        w_u16(slot, r_u16(slot) + 1);
    }
}

uint32_t sub_8005DA38(uint32_t data)
{
    uint16_t rectangle[4];
    uint32_t players;

    FUNCTION_MARKER(0x8005DA38, "RASHCDI.BIN");
    rectangle[2] = r_u16(data + 12);
    rectangle[3] = r_u16(data + 14);
    players = r_u32(r_u32(0x8005B2F8) + 48);
    rectangle[1] = (uint16_t)(512 - rectangle[3]);
    rectangle[0] = (uint16_t)(r_u16(0x80053254 + 176 * (players - 1) + 112) + 640);
    (void)rrj_gpu_upload((const uint8_t *)rectangle, data + 20);
    w_u32(0x800D6118, (uint32_t)r_s16(data + 8));
    return 0;
}

uint32_t sub_8005DAD0(uint32_t data, uint32_t index)
{
    uint16_t rectangle[4];
    uint32_t layout;
    uint32_t columns;
    uint32_t offset;
    int32_t remainder;

    FUNCTION_MARKER(0x8005DAD0, "RASHCDI.BIN");
    if (index == 0xFFFFFFFF)
    {
        index = r_u32(0x800D6118);
        w_u32(0x800D6118, index + 1);
    }
    rectangle[2] = 128;
    rectangle[3] = 1;
    layout = 0x80053254 + 176 * (r_u32(r_u32(0x8005B2F8) + 48) - 1);
    columns = r_u8(layout + 116);
    remainder = columns ? (int32_t)index % (int32_t)columns : (int32_t)index;
    offset = (uint32_t)remainder * r_u8(layout + 118);
    rectangle[1] = (uint16_t)(511 - (uint32_t)((int32_t)index / 3));
    rectangle[0] = (uint16_t)(r_u16(layout + 112) + offset + 640);
    (void)rrj_gpu_upload((const uint8_t *)rectangle, data);
    return index;
}

uint32_t sub_8005DBB8(uint32_t data)
{
    uint32_t player = 0;
    uint32_t next;
    uint32_t source;
    uint32_t descriptor;
    uint32_t kind;
    uint32_t variant;
    uint32_t palette;
    uint32_t offset;
    uint32_t index;
    int32_t rider;

    FUNCTION_MARKER(0x8005DBB8, "RASHCDI.BIN");
    next = r_u32(0x800D6118);
    w_u8(0x8006B89F, 255);
    w_u8(0x8006B89B, 255);
    source = data + 20;
    if (r_u32(r_u32(0x8005B2F8) + 48))
    {
        do
        {
            rider = r_s32(r_u32(0x8005B2F8) + 72 + 4 * player);
            kind = 0;
            if (rider >= 9)
                kind = (uint32_t)((uint32_t)rider - 9) < 9 ? 1 : 2;
            descriptor = 0x8006B898 + 4 * player;
            w_u8(descriptor, kind);
            if (kind != 2)
            {
                variant = player;
                if (r_u32(r_u32(0x8005B2F8) + 48) != 2)
                    variant = (uint32_t)r_s8(0x800D81E2 + 36 * player);
                offset = kind == 1 ? (6 * variant + 3) << 8 : 1536 * variant;
                palette = source + offset;
                for (index = 0; index < 3; ++index)
                {
                    (void)sub_8005DAD0(palette, next);
                    ++next;
                    palette += 256;
                }
                w_u8(descriptor, kind);
                w_u8(descriptor + 1, variant);
                w_u8(descriptor + 2, 3);
                w_u8(descriptor + 3, next - 3);
            }
            w_u32(0x800D6118, next);
            ++player;
        } while (player < r_u32(r_u32(0x8005B2F8) + 48));
    }
    palette = source + 4608;
    for (index = 0; index < 3; ++index)
    {
        (void)sub_8005DAD0(palette, 43 + index);
        palette += 256;
    }
    return 0;
}

uint32_t sub_8005DDB8(uint32_t data)
{
    uint32_t kind;
    uint32_t players;
    uint32_t table;
    uint32_t layout;
    uint32_t count;
    uint32_t index;
    uint32_t identity;
    uint32_t destination;
    uint32_t flags;
    uint32_t slot;
    uint32_t palette;
    uint32_t columns;
    uint32_t x;
    uint32_t y;
    uint16_t rectangle[4];
    uint8_t record[20];
    int32_t remainder;

    FUNCTION_MARKER(0x8005DDB8, "RASHCDI.BIN");
    kind = r_u8(data + 12);
    players = r_u32(r_u32(0x8005B2F8) + 48);
    table = 0x800533B4 + 88 * (players - 1);
    layout = 0x80053254 + 176 * (players - 1);
    if (kind >= 1 && kind <= 3)
    {
        count = r_u32(0x800D6108);
        if ((int32_t)count > 0)
        {
            identity = r_u16(data + 16);
            for (index = 0; (int32_t)index < (int32_t)count; ++index)
            {
                slot = r_u8(0x800D5F88 + 12 * index);
                if (slot == identity)
                    return slot;
            }
        }
        count = r_u32(0x800D6108);
        if ((int32_t)count >= r_s32(0x8005AE08 + 4 * (players - 1)))
            return count << 1;
        destination = 0x800D5F88 + 12 * count;
        sub_80014044(record, data + 24);
        flags = r_u8(table + 58);
        count = r_u32(0x800D6108);
        slot = count & 3;
        rectangle[0] = (uint16_t)(((flags & 15) << 6) + ((count << 4) & 960));
        flags = r_u8(table + 58);
        rectangle[2] = 64;
        rectangle[3] = 60;
        rectangle[1] = (uint16_t)(r_u8(table + 57) + 16 * (flags & 16) + 60 * slot);
        w_u8(destination, r_u8(data + 16));
        w_u8(destination + 1, slot);
        w_u8(destination + 2, 0);
        w_u8(destination + 3, 60 * slot);
        w_u16(destination + 8, ((rectangle[1] & 256) >> 4) | ((rectangle[0] & 1023) >> 6) | 128 | ((rectangle[1] & 512) << 2));
        w_u16(destination + 6, rectangle[0]);
        (void)rrj_gpu_upload((const uint8_t *)rectangle, xport_load_le32(record + 16));
        w_u32(0x800D6108, r_u32(0x800D6108) + 1);
        identity = r_u8(destination);
        palette = identity == 116 ? 47 : identity == 149 ? 46 : identity == 151 ? 41 : r_u32(0x800D6118);
        columns = r_u8(layout + 116);
        remainder = columns ? (int32_t)palette % (int32_t)columns : (int32_t)palette;
        x = r_u16(layout + 112) + (uint32_t)remainder * r_u8(layout + 118) + 640;
        y = 511 - (uint32_t)((int32_t)palette / 3);
        rectangle[2] = 128;
        rectangle[3] = 1;
        rectangle[1] = (uint16_t)y;
        rectangle[0] = (uint16_t)x;
        w_u16(destination + 10, (y << 6) | ((x >> 4) & 63));
        if (palette - 46 < 2 || palette == 41)
            (void)rrj_gpu_upload((const uint8_t *)rectangle, xport_load_le32(record + 8));
    }
    else if (kind == 4)
    {
        count = r_u32(0x800D610C);
        if ((int32_t)count > 0)
        {
            identity = r_u16(data + 16);
            for (index = 0; (int32_t)index < (int32_t)count; ++index)
            {
                slot = r_u8(0x800D60A8 + 12 * index);
                if (slot == identity)
                    return slot;
            }
        }
        count = r_u32(0x800D610C);
        if ((int32_t)count >= r_s32(0x8005AE10 + 4 * (players - 1)))
            return count << 1;
        destination = 0x800D60A8 + 12 * count;
        sub_80014044(record, data + 24);
        count = r_u32(0x800D610C);
        flags = r_u8(table + 46);
        slot = count & 7;
        rectangle[0] = (uint16_t)(((flags & 15) << 6) + ((count << 3) & 960) + 32 * (count & 1));
        flags = r_u8(table + 46);
        rectangle[2] = 32;
        rectangle[3] = 64;
        rectangle[1] = (uint16_t)(r_u8(table + 45) + 16 * (flags & 16) + ((slot >> 1) << 6));
        w_u8(destination, r_u8(data + 16));
        w_u8(destination + 1, slot);
        w_u8(destination + 2, (count & 1) << 6);
        w_u8(destination + 3, (slot >> 1) << 6);
        w_u16(destination + 8, ((rectangle[1] & 256) >> 4) | ((rectangle[0] & 1023) >> 6) | 128 | ((rectangle[1] & 512) << 2));
        w_u16(destination + 6, rectangle[0]);
        (void)rrj_gpu_upload((const uint8_t *)rectangle, xport_load_le32(record + 16));
        count = r_u32(0x800D610C);
        palette = r_u32(0x800D6118);
        w_u32(0x800D610C, count + 1);
        columns = r_u8(layout + 116);
        remainder = columns ? (int32_t)palette % (int32_t)columns : (int32_t)palette;
        x = r_u16(layout + 112) + (uint32_t)remainder * r_u8(layout + 118) + 640;
        y = 511 - (uint32_t)((int32_t)palette / 3);
        rectangle[2] = 128;
        rectangle[3] = 1;
        rectangle[1] = (uint16_t)y;
        rectangle[0] = (uint16_t)x;
        w_u16(destination + 10, (y << 6) | ((x >> 4) & 63));
        (void)rrj_gpu_upload((const uint8_t *)rectangle, xport_load_le32(record + 8));
        w_u32(0x800D6118, r_u32(0x800D6118) + 1);
    }
    else if (kind == 5 || kind == 6)
    {
        uint32_t field = kind == 5 ? 70 : 18;

        flags = r_u8(table + field);
        x = (flags & 15) << 6;
        rectangle[0] = (uint16_t)x;
        flags = r_u8(table + field);
        y = r_u8(table + field - 1) + 16 * (flags & 16);
        rectangle[2] = 64;
        rectangle[3] = kind == 5 ? 32 : 192;
        rectangle[1] = (uint16_t)y;
        destination = kind == 5 ? 0x800D5F70 : 0x800D5F7C;
        w_u8(destination, r_u8(data + 16));
        w_u16(destination + 8, ((y & 256) >> 4) | (x >> 6) | ((y & 512) << 2));
        w_u8(destination + 1, 0);
        w_u8(destination + 2, 4 * r_u8(table + field - 2));
        index = r_u8(table + field - 1);
        w_u16(destination + 6, x);
        w_u8(destination + 3, index);
        flags = r_u8(table + field);
        index = r_u8(destination + 3) + 16 * (flags & 16) + (kind == 5 ? 31 : 191);
        w_u16(destination + 4, (r_u16(destination + 4) & 0xE004) | ((index & 511) << 4) | 14);
        (void)rrj_gpu_upload((const uint8_t *)rectangle, data + 24);
        w_u32(kind == 5 ? 0x800D6110 : 0x800D6114, 1);
    }
    return (uint32_t)DrawSync(0);
}

uint32_t sub_8005C920(uint32_t data, int32_t bytes)
{
    uint32_t cursor = data;
    uint32_t tag;
    uint32_t length;

    FUNCTION_MARKER(0x8005C920, "RASHCDI.BIN");
    if (bytes <= 0)
        return 0;
    do
    {
        tag = r_u32(cursor);
        length = r_u32(cursor + 4);
        if (tag == 0x504B5443)
            sub_8005BDAC(cursor);
        else if (tag == 0x50424E4B)
            (void)sub_8005DA38(cursor);
        else if (tag == 0x504C5354)
            (void)sub_8005DBB8(cursor);
        else if (tag == 0x5443454C)
            (void)sub_8005DDB8(cursor);
        cursor += length;
    } while ((int32_t)(cursor - data) < bytes);
    return 0;
}

uint32_t sub_8006270C(uint32_t header, uint32_t data)
{
    uint32_t bytes;
    uint32_t flags;

    FUNCTION_MARKER(0x8006270C, "RASHCDI.BIN");
    bytes = r_u16(header + 2);
    flags = r_u32(r_u32(0x8005B2F8) + 4) & 24;
    if (!flags)
        return sub_8005C920(data, (int32_t)(bytes - 4));
    return flags;
}

uint32_t sub_80062750(uint32_t header, uint32_t data)
{
    uint32_t state;

    FUNCTION_MARKER(0x80062750, "RASHCDI.BIN");
    state = r_u32(0x8005B2F8);
    if ((r_u32(state + 4) & 24) != 8 || r_u8(state + 4) == 44)
        return sub_8005C920(data, (int32_t)(r_u16(header + 2) - 4));
    return 44;
}

void sub_80061C24(uint32_t header, uint32_t data)
{
    FUNCTION_MARKER(0x80061C24, "RASHCDI.BIN");
    switch (r_u16(header))
    {
        case 1: (void)sub_80062384(header, data); break;
        case 2: (void)sub_8006250C(header, data); break;
        case 3: (void)sub_80060F58(data, r_u32(r_u32(0x8005B2F8) + 48)); break;
        case 4: (void)sub_80061D18(header, data); break;
        case 5: (void)sub_80061FAC(header, data); break;
        case 6: (void)sub_800626E4(header, data); break;
        case 7: (void)sub_80062430(header, data); break;
        case 8: (void)sub_8006270C(header, data); break;
        case 9: (void)sub_80062750(header, data); break;
        case 10: (void)sub_800627A8(header, data); break;
    }
}

uint32_t sub_800619A0(void)
{
    char path[128];
    uint8_t file_header[8];
    uint8_t section_header[12];
    uint32_t handle;
    uint32_t table;
    uint32_t table_bytes;
    uint32_t offset;
    uint32_t blob;
    uint32_t blob_bytes;
    uint32_t count;
    uint32_t index;
    int32_t track;
    int32_t path_length;

    FUNCTION_MARKER(0x800619A0, "RASHCDI.BIN");
    (void)sub_80061994();
    path_length = snprintf(path, sizeof(path), "data\\gamebin%d.dat", r_s32(r_u32(0x8005B2F8) + 48));
    if (path_length < 0 || (uint32_t)path_length >= sizeof(path))
    {
        RRJ_DEBUG_FPUTS("Gamebin path is too long\n", stderr);
        abort();
    }
    handle = sub_8001458C(path, r_u32(0x8005AD5C));
    if ((int32_t)handle < 0)
        return handle;
    if (sub_80014780(handle, file_header, 8) != 8 || xport_load_le16(file_header) != 259)
        goto close_file;
    count = xport_load_le16(file_header + 2);
    if ((int32_t)count < r_s32(r_u32(0x8005B2F8) + 64))
        goto close_file;
    table_bytes = count << 2;
    table = sub_8001447C(table_bytes, 1);
    if (!table)
        goto close_file;
    if (sub_80014780(handle, xport_guest_ptr(table, table_bytes), table_bytes) != table_bytes)
        goto close_table;
    track = r_s32(r_u32(0x8005B2F8) + 64);
    if (track < 1 || (uint32_t)track > count)
        goto close_table;
    offset = r_u32(table + 4 * ((uint32_t)track - 1));
    if (offset == 0x7FFFFFFF)
        goto close_table;
    (void)sub_80014634(handle, offset, 1);
    if (sub_80014780(handle, section_header, 12) != 12)
        goto close_table;
    (void)sub_800144B8(table);
    if (xport_load_le16(section_header) != 259)
        goto close_file;
    count = xport_load_le16(section_header + 2);
    table_bytes = count << 2;
    table = sub_8001447C(table_bytes, 1);
    if (!table)
        goto close_file;
    if (sub_80014780(handle, xport_guest_ptr(table, table_bytes), table_bytes) != table_bytes)
        goto close_table;
    blob_bytes = xport_load_le32(section_header + 4) - (table_bytes + 12);
    blob = sub_8001447C(blob_bytes, 1);
    if (!blob)
    {
        (void)sub_800144B8(table);
        goto close_file;
    }
    if (sub_80014780(handle, xport_guest_ptr(blob, blob_bytes), blob_bytes) != blob_bytes)
    {
        (void)sub_800144B8(blob);
        goto close_table;
    }
    (void)sub_8001460C(handle);
    for (index = 0; index < xport_load_le16(section_header + 2); ++index)
    {
        offset = blob + r_u32(table + 4 * index);
        sub_80061C24(offset, offset + 4);
    }
    (void)sub_800144B8(blob);
    return sub_800144B8(table);

close_table:
    (void)sub_8001460C(handle);
    return sub_800144B8(table);
close_file:
    return sub_8001460C(handle);
}

int32_t sub_8005C010(uint32_t identity)
{
    uint32_t index;
    FUNCTION_MARKER(0x8005C010, "RASHCDI.BIN");
    for (index = 0; index < 50; ++index)
        if (r_u32(0x800CE1B0 + 16 * index) == identity)
            return (int32_t)index;
    return -1;
}

uint32_t sub_8005C054(uint32_t index)
{
    uint32_t entry = 0x800CE1B0 + 16 * index;
    uint32_t table;
    uint32_t identity;
    uint32_t slot;
    FUNCTION_MARKER(0x8005C054, "RASHCDI.BIN");
    table = r_u32(0x8005B2E4);
    identity = r_u32(r_u32(r_u32(entry + 8)) + 28);
    for (slot = 0; slot < 34; ++slot, table += 12)
        if (r_u8(table) == identity)
            break;
    w_u8(entry + 7, slot < 34 ? slot : 255);
    return entry;
}

int32_t sub_8005BD80(uint32_t kind, uint32_t index)
{
    uint32_t entry = 0x800CE560 + 8 * kind;
    FUNCTION_MARKER(0x8005BD80, "RASHCDI.BIN");
    if (r_s16(entry + 2) == -1)
        w_u16(entry + 2, index);
    return -1;
}

int32_t sub_8005CB9C(uint32_t identity, uint32_t data)
{
    uint32_t index;
    uint32_t entry;
    uint32_t count;
    uint32_t buffer;
    FUNCTION_MARKER(0x8005CB9C, "RASHCDI.BIN");
    for (index = 0; index < 50; ++index)
    {
        entry = 0x800CE1B0 + 16 * index;
        if (r_u32(entry))
            continue;
        w_u32(entry, identity);
        count = r_u8(data + 12);
        w_u8(entry + 4, count);
        buffer = sub_8001447C(12 * count, 0);
        w_u32(entry + 8, buffer);
        (void)sub_8001E100(buffer, 0, 4);
        return (int32_t)index;
    }
    return -1;
}

int32_t sub_8005CC4C(uint32_t identity, uint32_t data, uint32_t part)
{
    uint32_t index;
    uint32_t entry = 0x800CE1B0;
    uint32_t buffer;
    uint32_t optional;
    FUNCTION_MARKER(0x8005CC4C, "RASHCDI.BIN");
    for (index = 0; index < 50; ++index, entry += 16)
    {
        if (r_u32(entry) != identity)
            continue;
        buffer = sub_8001447C(4 * r_u16(data + 24), 0);
        w_u32(r_u32(entry + 8) + 12 * part + 4, buffer);
        w_u32(r_u32(entry + 8) + 12 * part, data);
        w_u32(data + 32, data + r_u32(data + 32));
        optional = r_u32(data + 40);
        w_u32(data + 36, data + r_u32(data + 36));
        w_u32(data + 40, optional ? data + optional : 0);
        optional = r_u32(data + 48);
        w_u32(data + 48, optional ? data + optional : 0);
        optional = r_u32(data + 44);
        w_u32(data + 44, optional ? data + optional : 0);
        return (int32_t)index;
    }
    return -1;
}

uint32_t sub_8005CE5C(uint32_t data)
{
    uint32_t offset;
    uint32_t result;
    FUNCTION_MARKER(0x8005CE5C, "RASHCDI.BIN");
    offset = r_u32(data + 20);
    result = offset ? data + offset : 0;
    w_u32(data + 20, result);
    return result;
}

int32_t sub_8005CD60(uint32_t identity, uint32_t data, uint32_t part, uint32_t piece)
{
    uint32_t index;
    uint32_t entry = 0x800CE1B0;
    uint32_t table;
    FUNCTION_MARKER(0x8005CD60, "RASHCDI.BIN");
    for (index = 0; index < 50; ++index, entry += 16)
    {
        if (r_u32(entry) != identity)
            continue;
        (void)sub_8005CE5C(data);
        table = r_u32(r_u32(entry + 8) + 12 * part + 4);
        w_u32(table + 4 * r_u8(data + 13), data);
        if (part + 1 == r_u8(entry + 4) &&
            piece + 1 == r_u16(r_u32(r_u32(entry + 8) + 12 * part) + 24))
            w_u8(entry + 6, 1);
        return 0;
    }
    return -1;
}

int32_t sub_8005CE78(uint32_t identity, uint32_t data, uint32_t part)
{
    int32_t index;
    FUNCTION_MARKER(0x8005CE78, "RASHCDI.BIN");
    index = sub_8005C010(identity);
    if (index == -1)
        return -1;
    w_u32(r_u32(0x800CE1B0 + 16 * (uint32_t)index + 8) + 12 * part + 8, data);
    return 0;
}

int32_t sub_8005C0C4(uint32_t *identity_out, uint32_t data, int32_t bytes, uint32_t retain)
{
    uint32_t cursor = data;
    uint32_t identity = 0;
    uint32_t seen = 0;
    uint32_t part = 0xFFFFFFFF;
    uint32_t piece = 0;
    int32_t index = 0;
    uint32_t tag;
    uint32_t length;
    uint32_t key;
    FUNCTION_MARKER(0x8005C0C4, "RASHCDI.BIN");
    while ((int32_t)(cursor - data) < bytes)
    {
        tag = r_u32(cursor);
        length = r_u32(cursor + 4);
        key = r_u32(cursor + 8);
        if (tag == 0x33444D52)
        {
            index = sub_8005CB9C(key, cursor);
            seen = 1;
            if (index == -1)
                return -1;
            identity = key;
            cursor += 16;
            continue;
        }
        if (tag == 0x33444F44)
        {
            ++part;
            piece = 0;
            if (seen && sub_8005CC4C(key, cursor, part) == -1)
                return -1;
        }
        else if (tag == 0x33445044)
        {
            if (sub_8005CD60(key, cursor, part, piece++) == -1)
                return -1;
        }
        else if (tag == 0x33444242 && sub_8005CE78(key, cursor, part) == -1)
            return -1;
        cursor += length;
    }
    w_u32(0x800CE1B0 + 16 * (uint32_t)index + 12, retain ? data : 0);
    *identity_out = identity;
    return index;
}

int32_t sub_8005CA10(const char *geometry_path, const char *texture_path)
{
    uint32_t buffer;
    uint32_t bytes;
    uint32_t cursor;
    uint32_t length;
    uint32_t identity;
    uint32_t retain = 1;
    uint32_t entry;
    int32_t index;
    FUNCTION_MARKER(0x8005CA10, "RASHCDI.BIN");
    if (texture_path)
    {
        if ((int32_t)sub_80014654(texture_path, r_u32(0x8005AD5C), &buffer, &bytes) < 0)
            return -1;
        (void)sub_8005C920(buffer, (int32_t)bytes);
        (void)sub_800144B8(buffer);
    }
    if (!geometry_path)
        return 0;
    if ((int32_t)sub_80014654(geometry_path, r_u32(0x8005AD5C), &buffer, &bytes) < 0)
        return -1;
    cursor = buffer;
    while ((int32_t)(cursor - buffer) < (int32_t)bytes)
    {
        length = r_u32(cursor + 4);
        if (r_u32(cursor) == 0x33444D52)
        {
            index = sub_8005C0C4(&identity, cursor, (int32_t)length, retain);
            if (index < 0)
            {
                (void)sub_800144B8(buffer);
                return -1;
            }
            (void)sub_8005C054((uint32_t)index);
            entry = 0x800CE1B0 + 16 * (uint32_t)index;
            (void)sub_8005BD80((r_u16(r_u32(r_u32(entry + 8)) + 14) & 0x78) >> 3, (uint32_t)index);
            (void)sub_800303BC((r_u16(r_u32(r_u32(entry + 8)) + 14) & 0x78) >> 3, identity, (uint32_t)index);
            retain = 0;
        }
        cursor += length;
    }
    return 0;
}

uint32_t sub_80064034(uint32_t kind, uint32_t rider)
{
    FUNCTION_MARKER(0x80064034, "RASHCDI.BIN");
    if (kind == 0)
        return 0x8006B51C + 9 * rider;
    if (kind == 1)
        return 0x8006B5DC + 9 * rider;
    return 0;
}

int32_t sub_8005C30C(const char *path, uint32_t *bytes_out)
{
    uint32_t scratch;
    uint32_t bytes;
    uint32_t buffer;
    uint32_t identity;
    uint32_t entry;
    int32_t index;
    FUNCTION_MARKER(0x8005C30C, "RASHCDI.BIN");
    scratch = r_u32(r_u32(0x8005ACBC) + 2616);
    if ((int32_t)sub_80014680(path, r_u32(0x8005AD5C), xport_guest_ptr(scratch, 1), bytes_out) < 0)
        return -1;
    if (r_u32(scratch) == 0x5443454C)
    {
        bytes = *bytes_out - r_u32(scratch + 4);
        (void)sub_8005DDB8(scratch);
        buffer = sub_8001447C(bytes, 0);
        if (!buffer)
            return -1;
        (void)sub_8001E0B4(xport_guest_ptr(buffer, bytes), xport_guest_cptr(scratch + r_u32(scratch + 4), bytes), bytes);
        index = sub_8005C0C4(&identity, buffer, (int32_t)bytes, 1);
        if (index == -1)
            return -1;
    }
    else
    {
        index = sub_8005C0C4(&identity, scratch, (int32_t)*bytes_out, 1);
        // Reject the original unchecked negative model index
        if (index == -1)
            return -1;
    }
    (void)sub_8005C054((uint32_t)index);
    entry = 0x800CE1B0 + 16 * (uint32_t)index;
    (void)sub_8005BD80((r_u16(r_u32(r_u32(entry + 8)) + 14) & 0x78) >> 3, (uint32_t)index);
    (void)sub_800303BC((r_u16(r_u32(r_u32(entry + 8)) + 14) & 0x78) >> 3, identity, (uint32_t)index);
    return index;
}

int32_t sub_8005C45C(int32_t level)
{
    uint32_t state;
    int32_t players;
    uint32_t name;
    uint32_t rider;
    uint32_t bytes;
    int32_t player;
    int32_t result;
    int length;
    char geometry[104];
    char texture[104];
    FUNCTION_MARKER(0x8005C45C, "RASHCDI.BIN");
    state = r_u32(0x8005B2F8);
    players = r_s32(state + 48);
    if (level >= 3)
        level = 2;
    if (r_u8(state + 4) == 44)
        level = r_s32(state + 72) < 9 ? 3 : 4;
    name = 0x8006B4A4 + 4 * (uint32_t)level;
    length = snprintf(geometry, sizeof(geometry), "%s%s%s", (const char *)xport_guest_cptr(0x80052450, 1),
        (const char *)xport_guest_cptr(r_u32(name), 1), (const char *)xport_guest_cptr(0x8005B630, 1));
    if (length < 0 || length >= (int)sizeof(geometry))
        abort();
    length = snprintf(texture, sizeof(texture), "%s%s%s", (const char *)xport_guest_cptr(0x80052450, 1),
        (const char *)xport_guest_cptr(r_u32(name), 1), (const char *)xport_guest_cptr(0x8005B638, 1));
    if (length < 0 || length >= (int)sizeof(texture))
        abort();
    result = sub_8005CA10(geometry, texture);
    if (result)
        return result;
    for (player = 0; player < players; ++player)
    {
        rider = r_u32(r_u32(0x8005B2F8) + 72 + 4 * (uint32_t)player);
        if ((rider - 6 < 3 || rider - 15 < 3) && sub_8005C010(rider + 100) == -1)
        {
            name = sub_80064034(0, r_u32(r_u32(0x8005B2F8) + 72 + 4 * (uint32_t)player));
            length = snprintf(geometry, sizeof(geometry), "%s%s.%s", (const char *)xport_guest_cptr(0x8005243C, 1),
                (const char *)xport_guest_cptr(name, 1), (const char *)xport_guest_cptr(0x8005B648, 1));
            if (length < 0 || length >= (int)sizeof(geometry))
                abort();
            result = sub_8005C30C(geometry, &bytes);
        }
    }
    return 0;
}

uint32_t sub_8005C630(int32_t level)
{
    int32_t players;
    uint32_t state;
    uint32_t result = 0;
    int length;
    char number[3];
    char geometry[104];
    char texture[104];
    FUNCTION_MARKER(0x8005C630, "RASHCDI.BIN");
    state = r_u32(0x8005B2F8);
    players = r_s32(state + 48);
    number[0] = (char)((uint32_t)(level / 10) + 48);
    number[1] = (char)((uint32_t)(level % 10) + 48);
    number[2] = 0;
    if (!(r_u32(state + 4) & 0x18))
    {
        length = snprintf(geometry, sizeof(geometry), "%s%s", (const char *)xport_guest_cptr(0x80052450, 1),
            (const char *)xport_guest_cptr(0x8005B65C, 1));
        if (length < 0 || length >= (int)sizeof(geometry))
            abort();
        result = sub_8005CA10(geometry, 0) == 0;
    }
    state = r_u32(0x8005B2F8);
    if ((r_u32(state + 4) & 0x18) == 8 && r_u8(state + 4) != 44)
    {
        length = snprintf(geometry, sizeof(geometry), "%s%s", (const char *)xport_guest_cptr(0x80052450, 1),
            (const char *)xport_guest_cptr(0x8005B668, 1));
        if (length < 0 || length >= (int)sizeof(geometry))
            abort();
        length = snprintf(texture, sizeof(texture), "%s%s", (const char *)xport_guest_cptr(0x80052450, 1),
            (const char *)xport_guest_cptr(0x8005B674, 1));
        if (length < 0 || length >= (int)sizeof(texture))
            abort();
        (void)sub_8005CA10(geometry, texture);
        return result;
    }
    length = snprintf(geometry, sizeof(geometry), "%s%s%s%s%s", (const char *)xport_guest_cptr(0x80052450, 1),
        (const char *)xport_guest_cptr(0x8005B68C, 1), number,
        (const char *)xport_guest_cptr(r_u32(0x8006B4B8 + 4 * ((uint32_t)players - 1)), 1),
        (const char *)xport_guest_cptr(0x8005B630, 1));
    if (length < 0 || length >= (int)sizeof(geometry))
        abort();
    if (sub_8005CA10(geometry, 0) == 0)
        ++result;
    return result;
}
