#include <string.h>
#include "gpu.h"
#include "native_game_api.h"
#include "game_main.h"
#include "solo_file_dependencies.h"
#include <stdio.h>
#include <stdlib.h>
#include "psx.h"
#include "sdk_build_stubs.h"
#include "race_bodyless_batch_004.h"
#include "race_bodyless_batch_005.h"

uint32_t sub_8002C9D4(uint32_t index)
{
    FUNCTION_MARKER(0x8002C9D4u, "SLUS_010.53");
    uint32_t address = 0x800d8078 + (((index << 1) + index) << 3), allocation;
    if (r_u8(address))
    {
        allocation = r_u32(address + 4);
        if (allocation)
            (void)(uint32_t)sub_800144B8(allocation);
    }
    address = 0x800d8078 + (((index << 1) + index) << 3);
    w_u8(address, 0);
    w_u8(address + 1, 0);
    w_u8(address + 2, 0);
    w_u32(address + 4, 0);
    return address;
}

uint32_t sub_8001B67C(uint32_t index)
{
    FUNCTION_MARKER(0x8001B67Cu, "SLUS_010.53");
    uint32_t address;
    w_u32(0x800d6c68, index);
    address = 0x800d6c6c + (((index << 1) + index) << 3);
    w_u32(0x8005B474u, address);
    return address;
}

uint32_t sub_8001BCE8(uint32_t mode)
{
    DISPENV display;
    DRAWENV draw;
    PSX_RECT rectangle;
    uint32_t destination_x, destination_y, source_y, result;
    FUNCTION_MARKER(0x8001BCE8u, "SLUS_010.53");
    (void)sub_800494E8(&display);
    (void)sub_80048FBC(&draw);
    destination_x = (uint32_t)(int32_t)draw.clip.x;
    destination_y = (uint32_t)(int32_t)draw.clip.y;
    if (mode == 1)
    {
        w_u32(0x8005B45Cu, 61);
        w_u32(0x8005B460u, 61);
        rectangle.x = 61;
        rectangle.w = 262;
        rectangle.h = 9;
        source_y = (uint16_t)display.disp.y;
        destination_x += 61u;
        destination_y += 198u;
        source_y += 216u;
    }
    else
    {
        w_u32(0x8005B45Cu, 124);
        w_u32(0x8005B460u, 124);
        rectangle.x = 122;
        rectangle.w = 140;
        rectangle.h = 20;
        source_y = (uint16_t)display.disp.y;
        destination_x += 122u;
        destination_y += 160u;
        source_y += 200u;
    }
    xport_store_le16(&rectangle.y, (uint16_t)source_y);
    result = (uint32_t)MoveImage(&rectangle, ((int32_t)(uint32_t)(destination_x)), ((int32_t)(uint32_t)(destination_y)));
    w_u32(0x8005B464u, 0);
    w_u32(0x8005AD88u, mode);
    w_u32(0x8005B468u, 0);
    return result;
}

uint32_t sub_8001B8BC(void)
{
    PSX_RECT source, base, restore, copy;
    DISPENV display;
    DRAWENV draw;
    POLY_F4 primitive = {0};
    uint32_t copied_x, copied_y, difference, filled, excess, bottom;
    FUNCTION_MARKER(0x8001B8BCu, "SLUS_010.53");
    if (r_u32(0x8005AD88u) == 1)
    {
        source.x = 61;
        source.y = 216;
        source.w = 262;
        source.h = 9;
        base.x = 61;
        base.y = 216;
        base.w = 262;
        base.h = 8;
    }
    else
    {
        source.x = 122;
        source.y = 200;
        source.w = 140;
        source.h = 20;
        base.x = 124;
        base.y = 213;
        base.w = 136;
        base.h = 5;
    }
    if (!r_u32(0x8005B468u))
    {
        uint32_t tick = r_u32(r_u32(0x8005B2F8u) + 100u);
        uint32_t rounded = (uint32_t)(((int32_t)(uint32_t)(tick)) / 10) * 10u;
        if (tick != rounded)
            return rounded;
    }
    if (!r_u32(0x8005B468u) && ((int32_t)(uint32_t)(r_u32(0x8005B45Cu))) >= ((int32_t)(uint32_t)(r_u32(0x8005B460u) + (uint32_t)(int32_t)base.w)))
    {
        w_u32(0x8005B45Cu, r_u32(0x8005B460u));
        w_u32(0x8005B464u, (r_u32(0x8005B464u) + 1u) & 3u);
    }
    else
    {
        uint32_t current = r_u32(0x8005B45Cu);
        uint32_t speed = r_u32(0x8005B468u);
        w_u32(0x8005B45Cu, current + 1u + ((0u - speed) & 8u));
    }
    (void)sub_800494E8(&display);
    (void)sub_80048FBC(&draw);
    copied_x = (uint16_t)source.x + (uint32_t)(uint16_t)draw.clip.x;
    copied_y = (uint16_t)source.y + (uint32_t)(uint16_t)draw.clip.y;
    xport_store_le16(&copy.x, (uint16_t)copied_x);
    copy.w = source.w;
    copy.h = source.h;
    restore.x = copy.x;
    restore.w = source.w;
    restore.h = source.h;
    xport_store_le16(&copy.y, (uint16_t)(copied_y - 2u * (uint16_t)source.h));
    xport_store_le16(&restore.y, (uint16_t)copied_y);
    (void)MoveImage(&copy, (int16_t)copied_x, (int16_t)copied_y);
    SetPolyF4(&primitive);
    setRGB0(&primitive, 200, 200, 200);
    SetSemiTrans(&primitive, 1);
    difference = r_u32(0x8005B45Cu) - r_u32(0x8005B460u);
    filled = difference + (((int32_t)(uint32_t)(difference)) < 0 ? 0u - difference : 0u);
    excess = (uint32_t)(int32_t)base.w - difference;
    filled += ((int32_t)(uint32_t)(excess)) < 0 ? excess : 0u;
    bottom = (uint16_t)base.y + (uint32_t)(uint16_t)base.h;
    setXY4(&primitive, base.x, base.y, (uint16_t)base.x + filled, base.y, base.x, bottom, (uint16_t)base.x + filled, bottom);
    DrawPrim(&primitive);
    return (uint32_t)MoveImage(&restore, source.x, (int32_t)source.y + display.disp.y);
}

uint32_t sub_8001C1AC(void)
{
    FUNCTION_MARKER(0x8001C1ACu, "SLUS_010.53");
    uint32_t index, address, mode, count, allocation;
    for (index = 0; index < 4; ++index)
    {
        w_u32(0x800d75c0 + (index << 2), 0);
        w_u32(0x800d75d0 + (index << 6), 0);
    }
    address = r_u32(0x8005b2f8);
    mode = r_u32(address + 48);
    address = r_u32(0x8005b470);
    w_u8(address + 244, mode == 1 ? 2 : 4);
    address = r_u32(0x8005b470);
    count = r_u8(address + 244);
    index = 0;
    if (count)
        do
        {
            allocation = (uint32_t)sub_8001447C(0x1450, 0);
            address = r_u32(0x8005b470);
            w_u32(address + (index << 2) + 248, allocation);
            count = r_u8(address + 244);
            ++index;
        } while (((int32_t)(uint32_t)(index)) < ((int32_t)(uint32_t)(count)));
    address = r_u32(0x8005b470);
    allocation = r_u32(address + 248);
    w_u32(0x8005ae00, 0);
    w_u32(address + 264, allocation);
    return rrj_gpu_clear_ot(allocation, 0x514u);
}

uint32_t sub_8001C498(void)
{
    FUNCTION_MARKER(0x8001C498u, "SLUS_010.53");
    return r_u32(0x800d6c68);
}

uint32_t sub_8002D2E0(void)
{
    FUNCTION_MARKER(0x8002D2E0u, "SLUS_010.53");
    w_u8(0x8005AEB9, 0);
    w_u8(0x8005AEB8, 0);
    w_u16(0x800540DA, 0);
    return 0x80050000u;
}

uint32_t sub_800303BC(uint32_t index, uint32_t key, uint32_t value)
{
    FUNCTION_MARKER(0x800303BCu, "SLUS_010.53");
    uint32_t record = 0x800d4c38 + (index << 4), base, address, result;
    base = r_u32(record + 8);
    key -= base;
    address = r_u32(record + 12);
    address += key << 1;
    result = (uint32_t)r_s16(address);
    if (((int32_t)(uint32_t)(result)) < 0)
    {
        w_u16(address, (uint16_t)value);
        result = r_u32(record + 4) + 1;
        w_u32(record + 4, result);
    }
    return result;
}

uint32_t sub_80016464(uint32_t handle)
{
    FUNCTION_MARKER(0x80016464, "SLUS_010.53");
    if (handle)
    {
        w_u32(0x800d6aa0 + ((handle >> 11) & 0xfe0) + 8, 2);
        return (uint32_t)sub_80030FA0(handle & 255);
    }
    w_u32(0x8005B3E0u, 1);
    return 1;
}

uint32_t sub_80030810(uint32_t *first_output, uint32_t *second_output)
{
    FUNCTION_MARKER(0x80030810u, "SLUS_010.53");
    uint32_t context = r_u32(0x8005acbc), value;
    if (!r_u32(context + 0xa3c))
        return 0;
    value = r_u32(context + 0xa38);
    if (!value)
        return 0;
    xport_store_le32(first_output, value);
    context = r_u32(0x8005acbc);
    value = r_u32(context + 0xa58);
    xport_store_le32(second_output, value << 14);
    return 1;
}

uint32_t sub_800244E0(void)
{
    FUNCTION_MARKER(0x800244E0u, "SLUS_010.53");
    uint32_t context = r_u32(0x8005b2f8), file_name, handle, size, buffer, value;
    file_name = r_u32(context + 48) == 2 ? 0x80010bd8 : 0x80010be4;
    (void)sprintf((char *)xport_guest_ptr(0x800D7E80u, 1), (const char *)xport_guest_cptr(file_name, 1));
    handle = (uint32_t)sub_80023100((const char *)xport_guest_cptr(0x800d7e80, 1));
    size = (uint32_t)sub_800148BC(handle);
    buffer = r_u32(0x8005ae64);
    if (!buffer)
    {
        value = (uint32_t)sub_8001447C(size, 0);
        w_u32(0x8005ae64, value);
    }
    buffer = r_u32(0x8005ae64);
    (void)(uint32_t)sub_80014780(handle, xport_guest_ptr(buffer, 1), size);
    (void)(uint32_t)sub_8001460C(handle);
    buffer = r_u32(0x8005ae64);
    (void)(uint32_t)sub_80024610(buffer);
    buffer = r_u32(0x8005ae64);
    value = r_u32(buffer + 8);
    w_u32(0x8005ae60, value);
    return value;
}

uint32_t sub_80023100(const char *name)
{
    char path[128];
    FUNCTION_MARKER(0x80023100u, "SLUS_010.53");
    path[0] = 0;
    strcat(path, (const char *)xport_guest_cptr(r_u32(0x8005AE2Cu), 1));
    strcat(path, name);
    return sub_8001458C(path, 0u);
}

uint32_t sub_80024610(uint32_t record)
{
    uint32_t first, second;
    FUNCTION_MARKER(0x80024610u, "SLUS_010.53");
    first = r_u32(record + 20);
    second = r_u32(record + 24);
    first += record;
    second += record;
    w_u32(record + 20, first);
    w_u32(record + 24, second);
    return 1;
}

uint32_t sub_80022F78(void)
{
    FUNCTION_MARKER(0x80022F78u, "SLUS_010.53");
    uint32_t context = r_u32(0x8005b2f8);
    if (r_u32(context + 48) == 1)
        (void)sub_80024630();
    (void)sub_80023498();
    return (uint32_t)sub_80022A1C(2);
}

uint32_t sub_80024630(void)
{
    FUNCTION_MARKER(0x80024630u, "SLUS_010.53");
    uint32_t name, first, second, result, flags;
    const uint32_t pairs[] = {1, 16000, 2, 0x2000, 3, 8, 4, 0, 7, 0x80024F78, 5, 0, 6, 0, 0};
    (void)(uint32_t)sub_80024FF8();
    name = r_u32(0x8005AE68);
    w_u32(0x8005b4e8, 0);
    w_u32(0x80053658, 0);
    first = (uint32_t)sub_80023100((const char *)xport_guest_cptr(name, 1));
    if (((int32_t)(uint32_t)(first)) < 0)
        return first;
    name = r_u32(0x8005AE6C);
    second = (uint32_t)sub_80023100((const char *)xport_guest_cptr(name, 1));
    if (((int32_t)(uint32_t)(second)) < 0)
        return second;
    w_u32(0x800cd6a0, UINT32_MAX);
    w_u32(0x800cd6a4, 1);
    w_u32(0x800cd670, first);
    w_u32(0x800cd674, 0);
    w_u32(0x800cd678, 8);
    w_u32(0x800cd67c, 0);
    result = (uint32_t)sub_800148BC(first);
    w_u32(0x800cd680, result);
    w_u32(0x800cd688, first);
    w_u32(0x800cd68c, second);
    w_u32(0x800cd69c, 0);
    flags = r_u32(0x800cd684);
    w_u32(0x8005B1F0, 0);
    w_u32(0x800cd684, flags | 1);
    result = sub_80020BEC(pairs);
    if (((int32_t)(uint32_t)(result)) >= 0)
        w_u32(0x800cd694, 0);
    return result;
}

uint32_t sub_80024FF8(void)
{
    FUNCTION_MARKER(0x80024FF8u, "SLUS_010.53");
    uint32_t index, address = 0x800d7e90;
    w_u32(address + 248, 0);
    w_u32(address + 244, 0);
    w_u32(address + 240, 0);
    for (index = 0; index < 30; ++index)
    {
        w_u32(address, UINT32_MAX);
        w_u32(address + 4, 0);
        address += 8;
    }
    return 0;
}
