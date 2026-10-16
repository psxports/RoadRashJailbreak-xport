#include "psx.h"
/* MIPS-audited choice of menu label color and optional highlight image. */
#include "menu_item.h"
#include "font.h"

void sub_8002CD78(uint32_t font, uint32_t id, uint32_t x, uint32_t y, uint32_t link, uint32_t color)
{
    FUNCTION_MARKER(0x8002CD78u, "SLUS_010.53");
    uint32_t table = r_u32(0x8005B544);
    if (table)
        (void)sub_8002CDC8(font, xport_guest_cptr(r_u32(table + 4 * id), 1), ((uint32_t)((uint32_t)(int32_t)(int16_t)((uint32_t)(x)))), ((uint32_t)((uint32_t)(int32_t)(int16_t)((uint32_t)(y)))), link, color);
}

uint32_t sub_8006F764(uint32_t menu, uint32_t entry)
{
    uint32_t scalar_v_19;
    uint32_t scalar_v_20;
    uint32_t scalar_v_21;

    FUNCTION_MARKER(0x8006F764u, "RASHCDF.BIN");
    uint32_t data = entry + 16, color, image, font, record, link, type, flags;
    int eligible = 1;
    if (((uint32_t)r_u16((entry + 10)) & 32) && !((uint32_t)r_u16(menu) & 2))
        return 1;
    if (r_u32(menu + 16) + 120 * (uint32_t)(int32_t)r_s16((menu + 4)) == entry)
    {
        image = r_u32(data + 24) ? 0x484E5442 : 0;
        color = r_u32(data + 12);
    }
    else
    {
        type = (uint32_t)r_u16((entry + 8));
        if (type != 12 && type != 13 && type != 17)
            eligible = 0;
        if (eligible)
        {
            flags = r_u32(entry);
            if ((flags & 0x40000000) && ((uint32_t)r_u8(0x800D70E1) >> 4) != 8)
                eligible = 0;
            if (eligible && (flags & 0x02000000) && r_u32(0x800D742C + 24 * ((scalar_v_19 = ((uint32_t)r_u8(0x8009C5E6))), (((uint32_t)(scalar_v_19 < 128 ? scalar_v_19 : scalar_v_19 | 0xffffff00))))) != 1)
                eligible = 0;
            if (eligible && (flags & 0x10000000) && !(uint32_t)r_u8(0x800D80F3))
                eligible = 0;
            if (eligible && (flags & 0x08000000) && r_u32(0x800D80D8) != 4)
                eligible = 0;
            if (eligible && (flags & 0x04000000) && r_u32(0x800D80D8) == 4)
                eligible = 0;
        }
        image = r_u32(data + 24) ? 0x4E4E5442 : 0;
        color = r_u32(data + (eligible ? 8 : 16));
    }
    w_u32(data + 24, image);
    font = r_u32(0x8009C5B8);
    record = 0x800D8078 + 24 * font;
    if ((uint32_t)r_u8(record))
    {
        link = r_u32(0x8009CFC8) + 4 * ((scalar_v_20 = ((uint32_t)r_u8(0x8009C5E1))), (((uint32_t)(scalar_v_20 < 128 ? scalar_v_20 : scalar_v_20 | 0xffffff00)))) + 8;
        w_u8(record + 3, 0);
        font = r_u32(0x8009C5B8);
        sub_8002CD78(font, r_u32(data + 4), (uint32_t)(int32_t)r_s16((data + 20)), (uint32_t)(int32_t)r_s16((data + 22)), link, color);
    }
    link = r_u32(0x8009CFC8) + 4 * ((scalar_v_21 = ((uint32_t)r_u8(0x8009C5E1))), (((uint32_t)(scalar_v_21 < 128 ? scalar_v_21 : scalar_v_21 | 0xffffff00)))) + 16;
    return sub_800700F0(menu, xport_guest_cptr(data + 24, 12), link);
}

uint32_t sub_8006EF30(uint32_t menu, uint32_t entry)
{
    uint32_t scalar_v_22;
    uint32_t scalar_v_23;

    uint32_t result, slot, table, selected, link, mode, cached, first, second;
    FUNCTION_MARKER(0x8006EF30, "RASHCDF.BIN");
    if (((uint32_t)r_u16((entry + 10u)) & 0x20u) && !((uint32_t)r_u16(menu) & 2u))
        return 1u;
    result = sub_8006F764(menu, entry);
    if (!result)
        return result;
    slot = ((scalar_v_22 = ((uint32_t)r_u8(0x8009C5E1u))), (((uint32_t)(scalar_v_22 < 128 ? scalar_v_22 : scalar_v_22 | 0xffffff00))));
    table = r_u32(0x8009CFC8u);
    selected = (uint32_t)(int32_t)r_s16((menu + 4u));
    link = table + slot * 4u + 12u;
    if (r_u32(menu + 16u) + selected * 120u != entry)
        return result;
    if (((int32_t)(uint32_t)(((scalar_v_23 = ((uint32_t)r_u8(0x8009C5E7u))), (((uint32_t)(scalar_v_23 < 128 ? scalar_v_23 : scalar_v_23 | 0xffffff00)))))) < 2)
        return result;
    mode = (uint32_t)r_u8(0x8009C5E3u);
    if (mode == 0x40u)
    {
        first = sub_800705DC(entry + 52u, link, 1u);
        second = sub_800705DC(entry + 76u, link, 0u);
        return first & second;
    }
    if (mode == 0x80u)
    {
        result = sub_800705DC(entry + 52u, link, 0u);
        if (!result)
            return result;
        return sub_800705DC(entry + 76u, link, 1u);
    }
    cached = r_u32(0x8009C658u);
    if (cached != entry)
    {
        first = sub_800705DC(entry + 52u, link, 3u);
        second = sub_800705DC(entry + 76u, link, 3u);
    }
    else
    {
        first = sub_800705DC(cached + 52u, link, 0u);
        second = sub_800705DC(cached + 76u, link, 0u);
    }
    return first & second;
}
