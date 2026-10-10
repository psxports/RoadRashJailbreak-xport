#include "psx.h"
/* Resident MIPS font lookup, width and textured packet generation. WIP until
 * live menu integration. The actual 8002CDC8 ABI has SIX arguments. */
#include "font.h"
#include "packet.h"

static uint32_t byte(RRJMemory *m, uint32_t a)
{
    return r_u8(a);
}

static uint32_t half(RRJMemory *m, uint32_t a)
{
    return rrj_u16(rrj_at(a, 2));
}

static uint32_t sbits(uint32_t b)
{
    return b < 128 ? b : b | 0xffffff00;
}

static uint32_t hbits(uint32_t h)
{
    h &= 65535;
    return h < 32768 ? h : h | 0xffff0000;
}

static uint32_t asr1(uint32_t n)
{
    return (n >> 1) | (n & 0x80000000);
}

/* PsyQ strlen boundary, operating on original RAM data rather than casting an
 * unbounded original pointer to a host string. No SDK function is registered. */
static uint32_t text_length(RRJMemory *m, uint32_t s)
{
    uint32_t n = 0;
    while (byte(m, s + n))
        ++n;
    return n;
}

uint32_t sub_8002D1F8(uint32_t ch, uint32_t table, uint32_t count, uint32_t stride)
{
    FUNCTION_MARKER(0x8002D1F8u, "SLUS_010.53");
    ch &= 255;
    while (count)
    {
        uint32_t item = table + asr1(count) * stride, c = byte(rrj_host_context(), item);
        if (ch == c)
            return item;
        if (ch > c)
        {
            table = item + stride;
            --count;
        }
        count = asr1(count);
    }
    return 0;
}

uint32_t sub_8002D1A0(uint32_t ch, uint32_t header, uint32_t glyphs)
{
    FUNCTION_MARKER(0x8002D1A0u, "SLUS_010.53");
    uint32_t item;
    ch &= 255;
    item = glyphs + (ch - 32) * 11;
    if (byte(rrj_host_context(), item) == ch)
        return item;
    return sub_8002D1F8(ch, glyphs, half(rrj_host_context(), header + 10), 11);
}

uint32_t sub_8002D0D8(uint32_t font, uint32_t text)
{
    FUNCTION_MARKER(0x8002D0D8u, "SLUS_010.53");
    uint32_t total = 0, record, i, n, glyph;
    if (font == 0xffffffff)
        return 0;
    record = 0x800D8078 + 24 * font;
    n = text_length(rrj_host_context(), text);
    for (i = 0; rrj_s32(i) < rrj_s32(n); ++i)
    {
        glyph = sub_8002D1A0(byte(rrj_host_context(), text + i), rrj_read32(record + 4), rrj_read32(record + 8));
        if (glyph)
            total += sbits(byte(rrj_host_context(), glyph + 8));
    }
    return hbits(total);
}

uint32_t sub_8002CDC8(uint32_t font, uint32_t text, uint32_t x, uint32_t y, uint32_t link, uint32_t color)
{
    FUNCTION_MARKER(0x8002CDC8u, "SLUS_010.53");
    uint32_t record, clut, page, n, space = 8, glyph, context, packet, cursor, i, emitted = 0;
    if (font == 0xffffffff)
        return 0xffffffff;
    y &= 65535;
    record = 0x800D8078 + 24 * font;
    clut = half(rrj_host_context(), 0x8005B538 + 2 * sbits(byte(rrj_host_context(), record + 3)));
    page = half(rrj_host_context(), record + 16);
    n = text_length(rrj_host_context(), text);
    glyph = sub_8002D1A0(32, rrj_read32(record + 4), rrj_read32(record + 8));
    if (glyph)
        space = sbits(byte(rrj_host_context(), glyph + 8));
    context = rrj_read32(0x8005B470);
    cursor = rrj_read32(context + 268);
    if (cursor + 40 * n >= rrj_read32(0x8005B4D0))
    {
        cursor = sub_80021C98(cursor, 40 * n);
        rrj_write32(rrj_read32(0x8005B470) + 268, cursor);
    }
    packet = rrj_read32(rrj_read32(0x8005B470) + 268);
    color |= 0x2e000000;
    for (i = 0; rrj_s32(i) < rrj_s32(n); ++i)
    {
        uint32_t ch = byte(rrj_host_context(), text + i), left, right, top, bottom, u0, u1, v0, v1, w, h, dx, dy;
        if (ch == 32)
        {
            x += space;
            continue;
        }
        glyph = sub_8002D1A0(ch, rrj_read32(record + 4), rrj_read32(record + 8));
        if (!glyph)
            continue;
        ++emitted;
        dx = sbits(byte(rrj_host_context(), glyph + 9));
        dy = sbits(byte(rrj_host_context(), glyph + 10));
        u0 = byte(rrj_host_context(), record + 18) + byte(rrj_host_context(), glyph + 4);
        w = byte(rrj_host_context(), glyph + 2);
        v0 = byte(rrj_host_context(), record + 19);
        h = byte(rrj_host_context(), glyph + 3);
        left = hbits(x + dx);
        right = hbits(x + dx + w);
        u1 = (u0 + w) & 255;
        u0 &= 255;
        v0 += byte(rrj_host_context(), glyph + 6);
        v1 = ((v0 + h) & 255) << 8;
        v0 = (v0 & 255) << 8;
        /* MIPS ORs SIGN-extended x into packed XY. Do not mask it to 16 bits:
         * negative x intentionally reproduces the original upper-half effect. */
        rrj_write32(packet, rrj_read32(link) | 0x09000000);
        rrj_write32(packet + 4, color);
        top = (y + dy) << 16;
        bottom = (y + dy + h) << 16;
        rrj_write32(packet + 8, left | top);
        rrj_write32(packet + 16, right | top);
        rrj_write32(packet + 12, u0 | v0 | (clut << 16));
        rrj_write32(packet + 24, left | bottom);
        rrj_write32(packet + 28, u0 | v1);
        rrj_write32(packet + 32, right | bottom);
        rrj_write32(packet + 36, u1 | v1);
        rrj_write32(packet + 20, u1 | v0 | (page << 16));
        rrj_write32(link, packet);
        x += sbits(byte(rrj_host_context(), glyph + 8));
        packet += 40;
    }
    context = rrj_read32(0x8005B470);
    cursor = rrj_read32(context + 268);
    rrj_write32(context + 268, cursor + 40 * emitted);
    return 40 * emitted;
}
