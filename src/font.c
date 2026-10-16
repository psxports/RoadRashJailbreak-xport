#include "psx.h"
/* Resident MIPS font lookup, width and textured packet generation. WIP until
 * live menu integration. The actual 8002CDC8 ABI has SIX arguments. */
#include "font.h"
#include "packet.h"
#include <string.h>

uint32_t sub_8002D1F8(uint32_t ch, uint32_t table, uint32_t count, uint32_t stride)
{
    FUNCTION_MARKER(0x8002D1F8u, "SLUS_010.53");
    ch &= 255;
    while (count)
    {
        uint32_t item = table + ((uint32_t)((((uint32_t)(count)) >> 1) | (((uint32_t)(count)) & 0x80000000))) * stride, c = (uint32_t)r_u8(item);
        if (ch == c)
            return item;
        if (ch > c)
        {
            table = item + stride;
            --count;
        }
        count = ((uint32_t)((((uint32_t)(count)) >> 1) | (((uint32_t)(count)) & 0x80000000)));
    }
    return 0;
}

uint32_t sub_8002D1A0(uint32_t ch, uint32_t header, uint32_t glyphs)
{
    FUNCTION_MARKER(0x8002D1A0u, "SLUS_010.53");
    uint32_t item;
    ch &= 255;
    item = glyphs + (ch - 32) * 11;
    if ((uint32_t)r_u8(item) == ch)
        return item;
    return sub_8002D1F8(ch, glyphs, (uint32_t)r_u16((header + 10)), 11);
}

uint32_t sub_8002D0D8(uint32_t font, const uint8_t *text)
{
    uint32_t scalar_b_21;

    FUNCTION_MARKER(0x8002D0D8u, "SLUS_010.53");
    uint32_t total = 0, record, i, n, glyph;
    if (font == 0xffffffff)
        return 0;
    record = 0x800D8078 + 24 * font;
    n = (uint32_t)strlen((const char *)text);
    for (i = 0; ((int32_t)(uint32_t)(i)) < ((int32_t)(uint32_t)(n)); ++i)
    {
        glyph = sub_8002D1A0((uint32_t)text[i], r_u32(record + 4), r_u32(record + 8));
        if (glyph)
            total += ((scalar_b_21 = ((uint32_t)r_u8((glyph + 8)))), (((uint32_t)(scalar_b_21 < 128 ? scalar_b_21 : scalar_b_21 | 0xffffff00))));
    }
    return ((uint32_t)((uint32_t)(int32_t)(int16_t)((uint32_t)(total))));
}

uint32_t sub_8002CDC8(uint32_t font, const uint8_t *text, uint32_t x, uint32_t y, uint32_t link, uint32_t color)
{
    uint32_t scalar_b_22;
    uint32_t scalar_b_23;
    uint32_t scalar_b_24;
    uint32_t scalar_b_25;
    uint32_t scalar_b_26;

    FUNCTION_MARKER(0x8002CDC8u, "SLUS_010.53");
    uint32_t record, clut, page, n, space = 8, glyph, context, packet, cursor, i, emitted = 0;
    if (font == 0xffffffff)
        return 0xffffffff;
    y &= 65535;
    record = 0x800D8078 + 24 * font;
    clut = (uint32_t)r_u16((0x8005B538 + 2 * ((scalar_b_22 = ((uint32_t)r_u8((record + 3)))), (((uint32_t)(scalar_b_22 < 128 ? scalar_b_22 : scalar_b_22 | 0xffffff00))))));
    page = (uint32_t)r_u16((record + 16));
    n = (uint32_t)strlen((const char *)text);
    glyph = sub_8002D1A0(32, r_u32(record + 4), r_u32(record + 8));
    if (glyph)
        space = ((scalar_b_23 = ((uint32_t)r_u8((glyph + 8)))), (((uint32_t)(scalar_b_23 < 128 ? scalar_b_23 : scalar_b_23 | 0xffffff00))));
    context = r_u32(0x8005B470);
    cursor = r_u32(context + 268);
    if (cursor + 40 * n >= r_u32(0x8005B4D0))
    {
        cursor = sub_80021C98(cursor, 40 * n);
        w_u32(r_u32(0x8005B470) + 268, cursor);
    }
    packet = r_u32(r_u32(0x8005B470) + 268);
    color |= 0x2e000000;
    for (i = 0; ((int32_t)(uint32_t)(i)) < ((int32_t)(uint32_t)(n)); ++i)
    {
        uint32_t ch = (uint32_t)text[i], left, right, top, bottom, u0, u1, v0, v1, w, h, dx, dy;
        if (ch == 32)
        {
            x += space;
            continue;
        }
        glyph = sub_8002D1A0(ch, r_u32(record + 4), r_u32(record + 8));
        if (!glyph)
            continue;
        ++emitted;
        dx = ((scalar_b_24 = ((uint32_t)r_u8((glyph + 9)))), (((uint32_t)(scalar_b_24 < 128 ? scalar_b_24 : scalar_b_24 | 0xffffff00))));
        dy = ((scalar_b_25 = ((uint32_t)r_u8((glyph + 10)))), (((uint32_t)(scalar_b_25 < 128 ? scalar_b_25 : scalar_b_25 | 0xffffff00))));
        u0 = (uint32_t)r_u8((record + 18)) + (uint32_t)r_u8((glyph + 4));
        w = (uint32_t)r_u8((glyph + 2));
        v0 = (uint32_t)r_u8((record + 19));
        h = (uint32_t)r_u8((glyph + 3));
        left = ((uint32_t)((uint32_t)(int32_t)(int16_t)((uint32_t)(x + dx))));
        right = ((uint32_t)((uint32_t)(int32_t)(int16_t)((uint32_t)(x + dx + w))));
        u1 = (u0 + w) & 255;
        u0 &= 255;
        v0 += (uint32_t)r_u8((glyph + 6));
        v1 = ((v0 + h) & 255) << 8;
        v0 = (v0 & 255) << 8;
        /* MIPS ORs SIGN-extended x into packed XY. Do not mask it to 16 bits:
         * negative x intentionally reproduces the original upper-half effect. */
        w_u32(packet, r_u32(link) | 0x09000000);
        w_u32(packet + 4, color);
        top = (y + dy) << 16;
        bottom = (y + dy + h) << 16;
        w_u32(packet + 8, left | top);
        w_u32(packet + 16, right | top);
        w_u32(packet + 12, u0 | v0 | (clut << 16));
        w_u32(packet + 24, left | bottom);
        w_u32(packet + 28, u0 | v1);
        w_u32(packet + 32, right | bottom);
        w_u32(packet + 36, u1 | v1);
        w_u32(packet + 20, u1 | v0 | (page << 16));
        w_u32(link, packet);
        x += ((scalar_b_26 = ((uint32_t)r_u8((glyph + 8)))), (((uint32_t)(scalar_b_26 < 128 ? scalar_b_26 : scalar_b_26 | 0xffffff00))));
        packet += 40;
    }
    context = r_u32(0x8005B470);
    cursor = r_u32(context + 268);
    w_u32(context + 268, cursor + 40 * emitted);
    return 40 * emitted;
}
