#include "psx.h"
/* Literal word-wrap state and signed truncation from F:80066318 MIPS. */
#include "word_wrap.h"
#include "font.h"

static uint32_t b(RRJMemory *m, uint32_t a)
{
    return r_u8(a);
}

static uint32_t h(RRJMemory *m, uint32_t a)
{
    return rrj_u16(rrj_at(a, 2));
}

static uint32_t sxh(uint32_t v)
{
    v &= 65535;
    return v < 32768 ? v : v | 0xffff0000;
}

static uint32_t sxb(uint32_t v)
{
    return v < 128 ? v : v | 0xffffff00;
}

uint32_t sub_F_80066318(uint32_t font, uint32_t text, uint32_t rect, uint32_t gap, uint32_t link, uint32_t color)
{
    FUNCTION_MARKER(0x80066318u, "RASHCDF.BIN");
    uint32_t record, step, length = 0, glyph, space = 8, last_break = 0, word_width = 0, width = 0, index = 0, start = 0, y, bottom;
    if (font == 0xffffffff)
        return 0xffffffff;
    record = 0x800D8078 + 24 * font;
    step = h(rrj_host_context(), record + 20) + gap;
    while (b(rrj_host_context(), text + length))
        ++length;
    glyph = sub_8002D1A0(32, rrj_read32(record + 4), rrj_read32(record + 8));
    if (glyph)
        space = sxb(b(rrj_host_context(), glyph + 8));
    y = sxh(h(rrj_host_context(), rect + 2));
    bottom = y + sxh(h(rrj_host_context(), rect + 6));
    while (rrj_s32(index) < rrj_s32(length))
    {
        uint32_t character = b(rrj_host_context(), text + index);
        if (character == 10)
        {
            if (width)
                start = index + 1;
            y += step;
            if (rrj_s32(bottom) < rrj_s32(sxh(y) + sxh(step)))
                break;
        }
        else
        {
            if (character == 32)
            {
                width += space;
                word_width = 0;
                last_break = index;
            }
            else
            {
                glyph = sub_8002D1A0(character, rrj_read32(record + 4), rrj_read32(record + 8));
                if (glyph)
                {
                    uint32_t advance = sxb(b(rrj_host_context(), glyph + 8));
                    width += advance;
                    word_width += advance;
                }
            }
            if (rrj_s32(sxh(h(rrj_host_context(), rect + 4))) < rrj_s32(width))
            {
                uint32_t saved;
                if (!last_break)
                    last_break = index;
                saved = b(rrj_host_context(), text + last_break);
                w_u8(text + last_break, 0);
                (void)sub_8002CDC8(font, text + start, sxh(h(rrj_host_context(), rect)), sxh(y), link, color);
                y += step;
                w_u8(text + last_break, (uint8_t)saved);
                if (rrj_s32(bottom) < rrj_s32(sxh(y) + sxh(step)))
                    break;
                start = last_break + 1;
                width = word_width;
            }
        }
        ++index;
    }
    if (!width)
        return y << 16;
    if (rrj_s32(bottom) < rrj_s32(sxh(y) + sxh(step)))
        return 1;
    return sub_8002CDC8(font, text + start, sxh(h(rrj_host_context(), rect)), sxh(y), link, color);
}
