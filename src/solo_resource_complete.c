#include "psx.h"
#include "xport.h"
#include "race_bodyless_batch_003.h"
#include "solo_resource_complete.h"
#include "resource_lookup.h"
#include <stdio.h>
#include <stdlib.h>

static uint32_t solo_complete_find(uint32_t tag)
{
    uint32_t index;
    for (index = 0; index < 300u; ++index)
        if (r_u32(0x80088E0Cu + index * 4u) == tag)
            break;
    return index;
}

static uint32_t solo_complete_texture(uint32_t descriptor, uint32_t data, uint32_t tag_address, uint32_t tag, uint32_t flags, uint32_t y)
{
    uint32_t index = solo_complete_find(tag);
    uint32_t entry;
    uint32_t width;
    uint32_t height;
    if (index == 300u)
        return 300u;
    entry = 0x8009DDE0u + index * 36u;
    w_u32(entry + 4u, data);
    w_u32(entry + 8u, r_u32(descriptor + 4u));
    if (tag_address)
        tag = r_u32(tag_address);
    w_u32(entry, flags);
    w_u32(entry + 12u, tag);
    width = (uint16_t)r_u16((data + 8u));
    width = (width >> 8) | ((width & 255u) << 8);
    w_u16((entry + 20u), (uint16_t)width);
    height = (uint16_t)r_u16((data + 10u));
    w_u16((entry + 16u), (uint16_t)512u);
    w_u16((entry + 18u), (uint16_t)y);
    height = (height >> 8) | ((height & 255u) << 8);
    w_u16((entry + 22u), (uint16_t)height);
    return height;
}

static uint32_t solo_complete_rect(uint32_t tag_address, uint32_t tag, uint32_t splash_order)
{
    uint32_t index = solo_complete_find(tag);
    uint32_t entry;
    uint32_t rect;
    uint32_t x;
    uint32_t y;
    uint32_t height;
    if (index == 300u)
        return 300u;
    entry = 0x8009DDE0u + index * 36u;
    if (splash_order)
    {
        w_u32(entry + 12u, tag);
        w_u32(entry + 4u, 0);
        w_u32(entry + 8u, 0);
        w_u32(entry, 0xC0200002u);
    }
    else
    {
        w_u32(entry + 4u, 0);
        w_u32(entry + 8u, 0);
        tag = r_u32(tag_address);
        w_u32(entry, 0xC0200002u);
        w_u32(entry + 12u, tag);
        tag = r_u32(tag_address);
    }
    rect = sub_8007A6C0(tag, 0);
    y = (uint16_t)r_u16((rect + 2u));
    x = (uint16_t)r_u16(rect);
    w_u16((entry + 28u), (uint16_t)(((y & 0x100u) >> 4) | ((x & 0x3FFu) >> 6) | 0x100u | ((y & 0x200u) << 2)));
    x = (uint32_t)(int32_t)r_s16(rect);
    w_u16((entry + 24u), (uint16_t)((uint32_t)((int32_t)x % 64)));
    y = (uint32_t)(int32_t)r_s16((rect + 2u));
    w_u16((entry + 26u), (uint16_t)((uint32_t)((int32_t)y % 256)));
    w_u16((entry + 30u), (uint16_t)0);
    w_u16((entry + 16u), (uint16_t)r_u16(rect));
    w_u16((entry + 18u), (uint16_t)r_u16((rect + 2u)));
    w_u16((entry + 20u), (uint16_t)r_u16((rect + 4u)));
    height = (uint16_t)r_u16((rect + 6u));
    w_u16((entry + 22u), (uint16_t)height);
    return height;
}

uint32_t sub_80076370(uint32_t descriptor)
{
    uint32_t scalar_value_1;

    FUNCTION_MARKER(0x80076370u, "RASHCDF.BIN");
    static const uint32_t tags[24] = {0x800892D8u, 0x800892BCu, 0x800892E8u, 0x800892F8u, 0x800892FCu, 0x8008932Cu, 0, 0x80089388u, 0x800893BCu, 0x800893DCu, 0x80089404u, 0x8008942Cu, 0x80089454u, 0x8008947Cu, 0x800894D0u, 0x80089520u, 0x8008954Cu, 0x80089560u, 0x80089574u, 0x800894E8u, 0x80089508u, 0x80089588u, 0x800895B8u, 0x8008969Cu};
    static const uint32_t counts[24] = {4, 2, 4, 1, 12, 23, 1, 13, 8, 10, 10, 10, 10, 21, 6, 11, 5, 5, 5, 8, 6, 12, 57, 36};
    uint32_t subtype = r_u8(descriptor + 3u);
    uint32_t data;
    uint32_t index;
    uint32_t result;
    uint32_t flags;
    uint32_t y;
    if (subtype >= 24u)
        return 0x80060000u;
    data = r_u32(descriptor + 4u);
    flags = subtype < 5u ? 0xD0200000u : (subtype < 7u ? 0xD0800000u : 0xD0400000u);
    y = subtype < 5u ? 0u : (subtype < 7u ? 480u : 256u);
    if (subtype == 6u)
        return solo_complete_texture(descriptor, data, 0, 0x524C5443u, flags, y);
    result = 300u;
    for (index = 0; index < counts[subtype]; ++index)
    {
        uint32_t tag_address = tags[subtype] + index * 4u;
        if (index)
            data += ((scalar_value_1 = (r_u32(data + 4u))), (((uint32_t)((scalar_value_1 >> 24) | ((scalar_value_1 >> 8) & 0xFF00u) | ((scalar_value_1 & 0xFF00u) << 8) | (scalar_value_1 << 24)))));
        result = solo_complete_texture(descriptor, data, tag_address, r_u32(tag_address), flags, y);
    }
    if (subtype == 0u)
        return solo_complete_rect(0, 0x484C5053u, 1);
    if (subtype == 2u || subtype == 4u)
    {
        uint32_t rect_tags = subtype == 2u ? 0x800892BCu : 0x800892C4u;
        uint32_t rect_count = subtype == 2u ? 2u : 5u;
        for (index = 0; index < rect_count; ++index)
        {
            uint32_t tag_address = rect_tags + index * 4u;
            result = solo_complete_rect(tag_address, r_u32(tag_address), 0);
        }
        return 0;
    }
    if (subtype == 3u)
        return result;
    return 0;
}

uint32_t sub_8007A618(uint32_t tag)
{
    FUNCTION_MARKER(0x8007A618, "RASHCDF.BIN");
    switch (tag)
    {
        case 0x53414853:
        case 0x53414353:
        case 0x5341424A:
        case 0x53414848:
        case 0x53415243:
        case 0x53414F35:
        case 0x53415252:
        case 0x53415454:
            return 1;
        default:
            return 0;
    }
}

uint32_t sub_80065248(uint32_t descriptor)
{
    uint32_t shifted_flags = r_u16(descriptor) >> 11;
    uint32_t data = r_u32(descriptor + 4), upload = shifted_flags & 1;
    uint32_t item = 0;
    FUNCTION_MARKER(0x80065248, "RASHCDF.BIN");
    if (r_s32(data + 8) <= 0)
        return shifted_flags;
    do
    {
        uint32_t record = data + 16 + 8 * item;
        uint32_t image = data + r_u32(record + 4);
        uint32_t tag = r_u32(record), index = 0, entry, rectangle_address, format;
        while (index < 300 && r_u32(0x80088E0C + 4 * index) != tag)
            ++index;
        if (index != 300)
        {
            entry = 0x8009DDE0 + 36 * index;
            w_u32(entry + 12, tag);
            format = r_u8(image);
            w_u32(entry + 8, data);
            w_u32(entry + 4, image);
            w_u16(entry + 32, format);
            rectangle_address = sub_8007A6C0(r_u32(record), 0);
            if (rectangle_address)
            {
                format = r_u8(image);
                if (format == 0x40 || format == 0x41 || format == 0x42)
                {
                    uint32_t first_word = r_u32(rectangle_address);
                    uint32_t second_word = r_u32(rectangle_address + 4);
                    uint32_t width = r_u16(image + 4);
                    int32_t x, y;
                    uint32_t page, flags;
                    w_u32(entry + 16, first_word);
                    w_u32(entry + 20, second_word);
                    x = r_s16(entry + 16);
                    page = r_u16(entry + 18);
                    w_u16(entry + 20, width);
                    page = ((page & 0x100) >> 4) | ((r_u16(entry + 16) & 0x3FF) >> 6) | ((page & 0x200) << 2);
                    if (format == 0x41)
                        page |= 0x80;
                    else if (format == 0x42)
                        page |= 0x100;
                    width = r_u16(image + 6);
                    w_u16(entry + 28, page);
                    if (format == 0x42)
                        w_u16(entry + 30, 0);
                    w_u16(entry + 22, width);
                    x %= 64;
                    y = r_s16(entry + 18);
                    w_u16(entry + 24, (uint32_t)x * (format == 0x40 ? 4 : format == 0x41 ? 2 : 1));
                    flags = r_u32(entry);
                    w_u16(entry + 26, y % 256);
                    if (format != 0x40)
                    {
                        flags |= format == 0x41 ? 1 : 2;
                        w_u32(entry, flags);
                    }
                    if (upload)
                    {
                        PSX_RECT rectangle;
                        rectangle.x = r_s16(entry + 16);
                        rectangle.y = r_s16(entry + 18);
                        rectangle.w = r_s16(entry + 20);
                        if (format == 0x40)
                            rectangle.w >>= 2;
                        else if (format == 0x41)
                            rectangle.w >>= 1;
                        rectangle.h = r_s16(entry + 22);
                        (void)LoadImagePSX(&rectangle, (uint32 *)xport_guest_ptr(image + 16,
                            rectangle.w > 0 && rectangle.h > 0 ? (size_t)rectangle.w * rectangle.h * 2 : 0));
                    }
                    if (format != 0x42)
                    {
                        uint32_t offset = r_u32(image) >> 8;
                        image += offset;
                        if (offset && r_u8(image) == 0x23 && upload)
                        {
                            PSX_RECT rectangle;
                            uint32_t palette;
                            rectangle.x = r_u32(image + 12) & 0xFFF;
                            rectangle.y = r_u16(image + 14) & 0xFFF;
                            rectangle.w = r_s16(image + 4);
                            rectangle.h = r_s16(image + 6);
                            (void)LoadImagePSX(&rectangle, (uint32 *)xport_guest_ptr(image + 16,
                                rectangle.w > 0 && rectangle.h > 0 ? (size_t)rectangle.w * rectangle.h * 2 : 0));
                            palette = r_u32(image + 12);
                            w_u16(entry + 30, (((palette >> 16) & 0xFFF) << 6) | ((palette >> 4) & 63));
                        }
                    }
                }
                {
                    uint32_t flags = r_u32(entry) | 0x10000000;
                    w_u32(entry, flags);
                    w_u32(entry, upload ? flags | 0x20000000 : flags & 0xDFFFFFFF);
                    if (sub_8007A618(r_u32(entry + 12)))
                        w_u32(entry, r_u32(entry) | 0x100000);
                }
            }
        }
        ++item;
    } while ((int32_t)item < r_s32(data + 8));
    return 0;
}

uint32_t sub_80065F4C(uint32_t descriptor)
{
    uint32_t data = r_u32(descriptor + 4);
    uint32_t offset = r_u32(data + 16), entries, index = 0;
    FUNCTION_MARKER(0x80065F4C, "RASHCDF.BIN");
    w_u32(0x8005AE78, data);
    entries = data + offset + 16;
    if (r_s32(data + 32) > 0)
    {
        uint32_t cursor = entries;
        do
        {
            uint32_t base, value;
            ++index;
            base = r_u32(data + 16);
            value = r_u32(cursor);
            w_u32(cursor, value + data + base);
            cursor += 4;
        } while ((int32_t)index < r_s32(data + 32));
    }
    w_u32(0x8005B544, entries);
    return 0x80060000;
}

uint32_t sub_80065FB0(uint32_t image, uint16_t output[16])
{
    FUNCTION_MARKER(0x80065FB0, "RASHCDF.BIN");
    if (r_u8(image) == 0x40)
    {
        PSX_RECT rectangle;
        uint32_t y, width, palette_offset;
        output[8] = r_u32(image + 12) & 0xFFF;
        output[9] = r_u16(image + 14) & 0xFFF;
        y = output[9];
        output[10] = r_u16(image + 4);
        width = r_u16(image + 6);
        output[14] = ((y & 0x100) >> 4) | ((output[8] & 0x3FF) >> 6) | ((y & 0x200) << 2);
        output[11] = (uint16_t)(width);
        output[12] = (r_u32(image + 12) & 63) * 4;
        output[13] = r_u16(image + 14) & 255;
        rectangle.x = (int16_t)output[8];
        rectangle.y = (int16_t)output[9];
        rectangle.w = (int16_t)output[10] >> 2;
        rectangle.h = (int16_t)output[11];
        (void)LoadImagePSX(&rectangle, (uint32 *)xport_guest_ptr(image + 16,
            rectangle.w > 0 && rectangle.h > 0 ? (size_t)rectangle.w * rectangle.h * 2 : 0));
        palette_offset = r_u32(image) >> 8;
        if (palette_offset)
        {
            uint32_t palette = image + (palette_offset << 4);
            if (r_u8(palette) == 0x23)
            {
                uint32_t packed;
                rectangle.x = r_u32(palette + 12) & 0xFFF;
                rectangle.y = r_u16(palette + 14) & 0xFFF;
                rectangle.w = r_s16(palette + 4);
                rectangle.h = r_s16(palette + 6);
                (void)LoadImagePSX(&rectangle, (uint32 *)xport_guest_ptr(palette + 16,
                    rectangle.w > 0 && rectangle.h > 0 ? (size_t)rectangle.w * rectangle.h * 2 : 0));
                packed = r_u32(palette + 12);
                output[15] = (((packed >> 16) & 0xFFF) << 6) | ((packed >> 4) & 63);
            }
        }
    }
    return image;
}

uint32_t sub_80066188(int32_t slot)
{
    uint16_t texture[16] = {0};
    uint32_t entry, image, packed, y;
    FUNCTION_MARKER(0x80066188, "RASHCDF.BIN");
    slot = (int8_t)slot;
    w_u32(0x8005B554, 0x800D8078 + 24 * (uint32_t)slot);
    if (!r_u16(0x8005B54C))
    {
        uint32_t palette_slot = sub_8002CA64((uint32_t *)xport_guest_ptr(0x80053A68, 32),
            r_s16(0x8005B548), r_s16(0x8005B54A));
        w_u8(r_u32(0x8005B554) + 3, palette_slot);
        w_u16(0x8005B54A, r_u16(0x8005B54A) + 1);
    }
    entry = r_u32(0x8005B554);
    packed = r_u16(0x8005B548);
    image = r_u32(entry + 12);
    y = r_u16(0x8005B54A);
    w_u32(image + 12, (r_u32(image + 12) & 0xFFFFF000) | (packed & 0xFFF));
    image = r_u32(entry + 12);
    w_u32(image + 12, (r_u32(image + 12) & 0xF000FFFF) | ((y & 0xFFF) << 16));
    image = r_u32(entry + 12);
    packed = r_u16(image + 6);
    image = r_u32(entry + 12);
    w_u16(0x8005B54A, y + packed);
    /* Initialize semantic output for unsupported original image formats */
    (void)sub_80065FB0(image, texture);
    entry = r_u32(0x8005B554);
    w_u16(entry + 16, texture[14]);
    w_u8(entry + 18, texture[12]);
    w_u8(r_u32(0x8005B554) + 19, texture[13]);
    w_u8(r_u32(0x8005B554) + 3, 0);
    w_u8(r_u32(0x8005B554) + 2, 3);
    return 3;
}

uint32_t sub_80065CA8(uint32_t descriptor)
{
    int32_t selected = -1;
    uint32_t candidate, cursor, data, entry, subtype;
    FUNCTION_MARKER(0x80065CA8, "RASHCDF.BIN");
    if (!r_u16(0x8005B54C))
    {
        uint32_t palette_slot = sub_8002CA64((uint32_t *)xport_guest_ptr(0x80053A68, 32),
            r_s16(0x8005B548), r_s16(0x8005B54A));
        w_u8(r_u32(0x8005B554) + 3, palette_slot);
        w_u16(0x8005B54A, r_u16(0x8005B54A) + 1);
    }
    candidate = r_u32(0x8005B540) + 1;
    if ((int32_t)candidate >= 4)
        candidate = 0;
    if (!r_s8(0x800D8078 + 24 * candidate))
        selected = (int32_t)candidate;
    else
    {
        cursor = 0x800D8078;
        for (candidate = 0; candidate < 4; ++candidate, cursor += 24)
            if (!r_s8(cursor))
            {
                selected = (int32_t)candidate;
                break;
            }
    }
    data = r_u32(descriptor + 4);
    selected = (int8_t)selected;
    if (selected != -1)
    {
        entry = 0x800D8078 + 24 * (uint32_t)selected;
        w_u32(0x8005B540, selected);
        w_u32(0x8005B554, entry);
        w_u8(entry, 1);
        entry = r_u32(0x8005B554);
        w_u32(entry + 4, data);
        w_u32(entry + 8, data + r_u32(data + 20));
        w_u32(entry + 12, data + r_u32(data + 28));
        if (r_u16(descriptor) & 0x800)
            (void)sub_80066188(selected);
    }
    subtype = r_u8(descriptor + 3);
    if (subtype == 1)
        w_u32(0x8009C5B8, selected);
    else if (subtype == 2)
        w_u32(0x8009C5C0, selected);
    else if (subtype == 3)
        w_u32(0x8009C5BC, selected);
    entry = r_u32(0x8005B554);
    if (r_s8(entry) == 1)
    {
        uint32_t header = r_u32(entry + 4);
        w_u16(entry + 20, 0);
        w_u16(entry + 22, 0);
        if (r_u16(header + 10))
        {
            uint32_t index = 0, offset = 0;
            do
            {
                uint32_t item = r_u32(entry + 8) + offset;
                int32_t maximum = r_s16(entry + 20);
                if (maximum < r_u8(item + 3))
                    w_u16(entry + 20, r_u8(item + 3));
                maximum = r_s16(entry + 22);
                if (maximum < r_s8(item + 8))
                    w_u16(entry + 22, r_s8(item + 8));
                header = r_u32(entry + 4);
                ++index;
                offset += 11;
            } while (index < r_u16(header + 10));
        }
    }
    return (uint32_t)selected;
}
