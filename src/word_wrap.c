#include "psx.h"
/* Literal word-wrap state and signed truncation from F:80066318 MIPS. */
#include "word_wrap.h"
#include "font.h"

uint32_t sub_80066318(uint32_t font, uint32_t text, const uint16_t rect[4], uint32_t gap, uint32_t link, uint32_t color)
{
    uint32_t scalar_v_19;
    uint32_t scalar_v_20;

    FUNCTION_MARKER(0x80066318u, "RASHCDF.BIN");
    uint32_t record, step, length = 0, glyph, space = 8, last_break = 0, word_width = 0, width = 0, index = 0, start = 0, y, bottom;
    if (font == 0xffffffff)
        return 0xffffffff;
    record = 0x800D8078 + 24 * font;
    step = (uint32_t)r_u16((record + 20)) + gap;
    while ((uint32_t)r_u8((text + length)))
        ++length;
    glyph = sub_8002D1A0(32, r_u32(record + 4), r_u32(record + 8));
    if (glyph)
        space = ((scalar_v_19 = ((uint32_t)r_u8((glyph + 8)))), (((uint32_t)(scalar_v_19 < 128 ? scalar_v_19 : scalar_v_19 | 0xffffff00))));
    y = (uint32_t)(int32_t)(int16_t)xport_load_le16(rect + 1);
    bottom = y + (uint32_t)(int32_t)(int16_t)xport_load_le16(rect + 3);
    while (((int32_t)(uint32_t)(index)) < ((int32_t)(uint32_t)(length)))
    {
        uint32_t character = (uint32_t)r_u8((text + index));
        if (character == 10)
        {
            if (width)
                start = index + 1;
            y += step;
            if (((int32_t)(uint32_t)(bottom)) < ((int32_t)(uint32_t)(((uint32_t)((uint32_t)(int32_t)(int16_t)((uint32_t)(y)))) + ((uint32_t)((uint32_t)(int32_t)(int16_t)((uint32_t)(step)))))))
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
                glyph = sub_8002D1A0(character, r_u32(record + 4), r_u32(record + 8));
                if (glyph)
                {
                    uint32_t advance = ((scalar_v_20 = ((uint32_t)r_u8((glyph + 8)))), (((uint32_t)(scalar_v_20 < 128 ? scalar_v_20 : scalar_v_20 | 0xffffff00))));
                    width += advance;
                    word_width += advance;
                }
            }
            if ((int32_t)(int16_t)xport_load_le16(rect + 2) < ((int32_t)(uint32_t)(width)))
            {
                uint32_t saved;
                if (!last_break)
                    last_break = index;
                saved = (uint32_t)r_u8((text + last_break));
                w_u8(text + last_break, 0);
                (void)sub_8002CDC8(font, xport_guest_cptr(text + start, 1), (uint32_t)(int32_t)(int16_t)xport_load_le16(rect), ((uint32_t)((uint32_t)(int32_t)(int16_t)((uint32_t)(y)))), link, color);
                y += step;
                w_u8(text + last_break, (uint8_t)saved);
                if (((int32_t)(uint32_t)(bottom)) < ((int32_t)(uint32_t)(((uint32_t)((uint32_t)(int32_t)(int16_t)((uint32_t)(y)))) + ((uint32_t)((uint32_t)(int32_t)(int16_t)((uint32_t)(step)))))))
                    break;
                start = last_break + 1;
                width = word_width;
            }
        }
        ++index;
    }
    if (!width)
        return y << 16;
    if (((int32_t)(uint32_t)(bottom)) < ((int32_t)(uint32_t)(((uint32_t)((uint32_t)(int32_t)(int16_t)((uint32_t)(y)))) + ((uint32_t)((uint32_t)(int32_t)(int16_t)((uint32_t)(step)))))))
        return 1;
    return sub_8002CDC8(font, xport_guest_cptr(text + start, 1), (uint32_t)(int32_t)(int16_t)xport_load_le16(rect), ((uint32_t)((uint32_t)(int32_t)(int16_t)((uint32_t)(y)))), link, color);
}
