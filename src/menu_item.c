/* MIPS-audited choice of menu label color and optional highlight image. */
#include "menu_item.h"
#include "font.h"

static uint32_t byte(RRJMemory *m, uint32_t a)
{
    return r_u8(a);
}

static uint32_t half(RRJMemory *m, uint32_t a)
{
    return rrj_u16(rrj_at(m, a, 2));
}

static uint32_t sbits(uint32_t v)
{
    return v < 128 ? v : v | 0xffffff00;
}

static uint32_t hbits(uint32_t v)
{
    v &= 65535;
    return v < 32768 ? v : v | 0xffff0000;
}

void sub_8002CD78(RRJMemory *m, uint32_t font, uint32_t id, uint32_t x, uint32_t y, uint32_t link, uint32_t color)
{
    uint32_t table = rrj_read32(m, 0x8005B544);
    if (table)
        (void)sub_8002CDC8(m, font, rrj_read32(m, table + 4 * id), hbits(x), hbits(y), link, color);
}

uint32_t sub_F_8006F764(RRJMemory *m, uint32_t menu, uint32_t entry, RRJMenuResource resource)
{
    uint32_t data = entry + 16, color, image, font, record, link, type, flags;
    int eligible = 1;
    if ((half(m, entry + 10) & 32) && !(half(m, menu) & 2))
        return 1;
    if (rrj_read32(m, menu + 16) + 120 * hbits(half(m, menu + 4)) == entry)
    {
        image = rrj_read32(m, data + 24) ? 0x484E5442 : 0;
        color = rrj_read32(m, data + 12);
    }
    else
    {
        type = half(m, entry + 8);
        if (type != 12 && type != 13 && type != 17)
            eligible = 0;
        if (eligible)
        {
            flags = rrj_read32(m, entry);
            if ((flags & 0x40000000) && (byte(m, 0x800D70E1) >> 4) != 8)
                eligible = 0;
            if (eligible && (flags & 0x02000000) && rrj_read32(m, 0x800D742C + 24 * sbits(byte(m, 0x8009C5E6))) != 1)
                eligible = 0;
            if (eligible && (flags & 0x10000000) && !byte(m, 0x800D80F3))
                eligible = 0;
            if (eligible && (flags & 0x08000000) && rrj_read32(m, 0x800D80D8) != 4)
                eligible = 0;
            if (eligible && (flags & 0x04000000) && rrj_read32(m, 0x800D80D8) == 4)
                eligible = 0;
        }
        image = rrj_read32(m, data + 24) ? 0x4E4E5442 : 0;
        color = rrj_read32(m, data + (eligible ? 8 : 16));
    }
    rrj_write32(m, data + 24, image);
    font = rrj_read32(m, 0x8009C5B8);
    record = 0x800D8078 + 24 * font;
    if (byte(m, record))
    {
        link = rrj_read32(m, 0x8009CFC8) + 4 * sbits(byte(m, 0x8009C5E1)) + 8;
        w_u8(record + 3, 0);
        font = rrj_read32(m, 0x8009C5B8);
        sub_8002CD78(m, font, rrj_read32(m, data + 4), hbits(half(m, data + 20)), hbits(half(m, data + 22)), link, color);
    }
    link = rrj_read32(m, 0x8009CFC8) + 4 * sbits(byte(m, 0x8009C5E1)) + 16;
    return sub_F_800700F0(m, menu, data + 24, link, resource);
}

uint32_t sub_F_8006EF30(RRJMemory *m, uint32_t menu, uint32_t entry, RRJMenuResource resource, RRJImageUpload upload)
{
    uint32_t result, slot, table, selected, link, mode, cached, first, second;
    FUNCTION_MARKER(0x8006EF30, "RASHCDF.BIN");
    if ((half(m, entry + 10u) & 0x20u) && !(half(m, menu) & 2u))
        return 1u;
    result = sub_F_8006F764(m, menu, entry, resource);
    if (!result)
        return result;
    slot = sbits(byte(m, 0x8009C5E1u));
    table = rrj_read32(m, 0x8009CFC8u);
    selected = hbits(half(m, menu + 4u));
    link = table + slot * 4u + 12u;
    if (rrj_read32(m, menu + 16u) + selected * 120u != entry)
        return result;
    if (rrj_s32(sbits(byte(m, 0x8009C5E7u))) < 2)
        return result;
    mode = byte(m, 0x8009C5E3u);
    if (mode == 0x40u)
    {
        first = sub_F_800705DC(m, entry + 52u, link, 1u, upload);
        second = sub_F_800705DC(m, entry + 76u, link, 0u, upload);
        return first & second;
    }
    if (mode == 0x80u)
    {
        result = sub_F_800705DC(m, entry + 52u, link, 0u, upload);
        if (!result)
            return result;
        return sub_F_800705DC(m, entry + 76u, link, 1u, upload);
    }
    cached = rrj_read32(m, 0x8009C658u);
    if (cached != entry)
    {
        first = sub_F_800705DC(m, entry + 52u, link, 3u, upload);
        second = sub_F_800705DC(m, entry + 76u, link, 3u, upload);
    }
    else
    {
        first = sub_F_800705DC(m, cached + 52u, link, 0u, upload);
        second = sub_F_800705DC(m, cached + 76u, link, 0u, upload);
    }
    return first & second;
}
