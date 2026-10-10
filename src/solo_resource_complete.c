#include "psx.h"
#include "solo_resource_complete.h"
#include "resource_lookup.h"
#include <stdio.h>
#include <stdlib.h>

static uint32_t solo_complete_find(RRJMemory *m, uint32_t tag)
{
    uint32_t index;
    for (index = 0; index < 300u; ++index)
        if (rrj_read32(0x80088E0Cu + index * 4u) == tag)
            break;
    return index;
}

static uint32_t solo_complete_swap32(uint32_t value)
{
    return (value >> 24) | ((value >> 8) & 0xFF00u) | ((value & 0xFF00u) << 8) | (value << 24);
}

static uint16_t solo_complete_half(RRJMemory *m, uint32_t address)
{
    return rrj_u16(rrj_at(address, 2));
}

static void solo_complete_put_half(RRJMemory *m, uint32_t address, uint32_t value)
{
    rrj_put16(rrj_at(address, 2), (uint16_t)value);
}

static uint32_t solo_complete_texture(RRJMemory *m, uint32_t descriptor, uint32_t data, uint32_t tag_address, uint32_t tag, uint32_t flags, uint32_t y)
{
    uint32_t index = solo_complete_find(m, tag);
    uint32_t entry;
    uint32_t width;
    uint32_t height;
    if (index == 300u)
        return 300u;
    entry = 0x8009DDE0u + index * 36u;
    rrj_write32(entry + 4u, data);
    rrj_write32(entry + 8u, rrj_read32(descriptor + 4u));
    if (tag_address)
        tag = rrj_read32(tag_address);
    rrj_write32(entry, flags);
    rrj_write32(entry + 12u, tag);
    width = solo_complete_half(m, data + 8u);
    width = (width >> 8) | ((width & 255u) << 8);
    solo_complete_put_half(m, entry + 20u, width);
    height = solo_complete_half(m, data + 10u);
    solo_complete_put_half(m, entry + 16u, 512u);
    solo_complete_put_half(m, entry + 18u, y);
    height = (height >> 8) | ((height & 255u) << 8);
    solo_complete_put_half(m, entry + 22u, height);
    return height;
}

static uint32_t solo_complete_rect(RRJMemory *m, uint32_t tag_address, uint32_t tag, uint32_t splash_order)
{
    uint32_t index = solo_complete_find(m, tag);
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
        rrj_write32(entry + 12u, tag);
        rrj_write32(entry + 4u, 0);
        rrj_write32(entry + 8u, 0);
        rrj_write32(entry, 0xC0200002u);
    }
    else
    {
        rrj_write32(entry + 4u, 0);
        rrj_write32(entry + 8u, 0);
        tag = rrj_read32(tag_address);
        rrj_write32(entry, 0xC0200002u);
        rrj_write32(entry + 12u, tag);
        tag = rrj_read32(tag_address);
    }
    rect = sub_F_8007A6C0(tag, 0);
    y = solo_complete_half(m, rect + 2u);
    x = solo_complete_half(m, rect);
    solo_complete_put_half(m, entry + 28u, ((y & 0x100u) >> 4) | ((x & 0x3FFu) >> 6) | 0x100u | ((y & 0x200u) << 2));
    x = (uint32_t)(int32_t)(int16_t)solo_complete_half(m, rect);
    solo_complete_put_half(m, entry + 24u, (uint32_t)((int32_t)x % 64));
    y = (uint32_t)(int32_t)(int16_t)solo_complete_half(m, rect + 2u);
    solo_complete_put_half(m, entry + 26u, (uint32_t)((int32_t)y % 256));
    solo_complete_put_half(m, entry + 30u, 0);
    solo_complete_put_half(m, entry + 16u, solo_complete_half(m, rect));
    solo_complete_put_half(m, entry + 18u, solo_complete_half(m, rect + 2u));
    solo_complete_put_half(m, entry + 20u, solo_complete_half(m, rect + 4u));
    height = solo_complete_half(m, rect + 6u);
    solo_complete_put_half(m, entry + 22u, height);
    return height;
}

uint32_t sub_F_80076370(uint32_t descriptor)
{
    FUNCTION_MARKER(0x80076370u, "RASHCDF.BIN");
    static const uint32_t tags[24] = {0x800892D8u, 0x800892BCu, 0x800892E8u, 0x800892F8u, 0x800892FCu, 0x8008932Cu, 0, 0x80089388u, 0x800893BCu, 0x800893DCu, 0x80089404u, 0x8008942Cu, 0x80089454u, 0x8008947Cu, 0x800894D0u, 0x80089520u, 0x8008954Cu, 0x80089560u, 0x80089574u, 0x800894E8u, 0x80089508u, 0x80089588u, 0x800895B8u, 0x8008969Cu};
    static const uint32_t counts[24] = {4, 2, 4, 1, 12, 23, 1, 13, 8, 10, 10, 10, 10, 21, 6, 11, 5, 5, 5, 8, 6, 12, 57, 36};
    uint32_t subtype = *(const uint8_t *)rrj_at(descriptor + 3u, 1);
    uint32_t data;
    uint32_t index;
    uint32_t result;
    uint32_t flags;
    uint32_t y;
    if (subtype >= 24u)
        return 0x80060000u;
    data = rrj_read32(descriptor + 4u);
    flags = subtype < 5u ? 0xD0200000u : (subtype < 7u ? 0xD0800000u : 0xD0400000u);
    y = subtype < 5u ? 0u : (subtype < 7u ? 480u : 256u);
    if (subtype == 6u)
        return solo_complete_texture(rrj_host_context(), descriptor, data, 0, 0x524C5443u, flags, y);
    result = 300u;
    for (index = 0; index < counts[subtype]; ++index)
    {
        uint32_t tag_address = tags[subtype] + index * 4u;
        if (index)
            data += solo_complete_swap32(rrj_read32(data + 4u));
        result = solo_complete_texture(rrj_host_context(), descriptor, data, tag_address, rrj_read32(tag_address), flags, y);
    }
    if (subtype == 0u)
        return solo_complete_rect(rrj_host_context(), 0, 0x484C5053u, 1);
    if (subtype == 2u || subtype == 4u)
    {
        uint32_t rect_tags = subtype == 2u ? 0x800892BCu : 0x800892C4u;
        uint32_t rect_count = subtype == 2u ? 2u : 5u;
        for (index = 0; index < rect_count; ++index)
        {
            uint32_t tag_address = rect_tags + index * 4u;
            result = solo_complete_rect(rrj_host_context(), tag_address, rrj_read32(tag_address), 0);
        }
        return 0;
    }
    if (subtype == 3u)
        return result;
    return 0;
}
