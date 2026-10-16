#include "text_id.h"
#include "psx.h"
/* MIPS rectangle alignment and button-label/image renderer. */
#include "menu_hint.h"
#include "font.h"
#include <stdlib.h>

uint32_t sub_8002CB08(uint32_t font, uint32_t id, const uint16_t rect[4], uint32_t link, uint32_t color, uint32_t mode)
{
    uint32_t scalar_v_12;

    FUNCTION_MARKER(0x8002CB08u, "SLUS_010.53");
    uint32_t text, x, y, width, box;
    if (mode > 3)
        return 3;
    if (mode == 3)
    {
        if (!(r_u32(0x8005ACA8) & 16))
            return 0;
        return sub_800662CC(font, id, rect, 0, link, color);
    }
    if (mode == 0)
    {
        x = (uint32_t)(int32_t)(int16_t)xport_load_le16(rect);
        y = (uint32_t)(int32_t)(int16_t)xport_load_le16(rect + 1);
        text = r_u32(r_u32(0x8005B544) + 4 * id);
    }
    else
    {
        text = r_u32(r_u32(0x8005B544) + 4 * id);
        width = sub_8002D0D8(font, xport_guest_cptr(text, 1));
        x = (uint32_t)xport_load_le16(rect);
        box = (uint32_t)xport_load_le16(rect + 2);
        y = (uint32_t)(int32_t)(int16_t)xport_load_le16(rect + 1);
        if (mode == 2)
        {
            width = ((uint32_t)((((uint32_t)(width)) >> 1) | (((uint32_t)(width)) & 0x80000000)));
            box = ((scalar_v_12 = (((uint32_t)((uint32_t)(int32_t)(int16_t)((uint32_t)(box)))))), (((uint32_t)((scalar_v_12 >> 1) | (scalar_v_12 & 0x80000000)))));
        }
        x = ((uint32_t)((uint32_t)(int32_t)(int16_t)((uint32_t)(x + box - width))));
        text = r_u32(r_u32(0x8005B544) + 4 * id);
    }
    return sub_8002CDC8(font, xport_guest_cptr(text, 1), x, y, link, color);
}

uint32_t sub_8006E7A4(uint32_t menu, uint32_t entry)
{
    uint32_t scalar_v_9;
    uint32_t scalar_v_10;

    FUNCTION_MARKER(0x8006E7A4u, "RASHCDF.BIN");
    uint32_t font, link, color, id, mode;
    if (((uint32_t)r_u16((entry + 10)) & 32) && !((uint32_t)r_u16(menu) & 2))
        return 1;
    font = r_u32(0x8009C5C0);
    if ((uint32_t)r_u8((0x800D8078 + 24 * font)))
    {
        color = r_u32(entry + 40);
        id = (uint32_t)r_u16((entry + 28));
        mode = (uint32_t)r_u16((entry + 30));
        link = r_u32(0x8009CFC8) + 4 * ((scalar_v_9 = ((uint32_t)r_u8(0x8009C5E1))), (((uint32_t)(scalar_v_9 < 128 ? scalar_v_9 : scalar_v_9 | 0xffffff00)))) + 4;
        (void)sub_8002CB08(font, id, xport_guest_cptr(entry + 32, 8), link, color, mode);
    }
    link = r_u32(0x8009CFC8) + 4 * ((scalar_v_10 = ((uint32_t)r_u8(0x8009C5E1))), (((uint32_t)(scalar_v_10 < 128 ? scalar_v_10 : scalar_v_10 | 0xffffff00)))) + 20;
    return sub_800700F0(menu, xport_guest_cptr(entry + 16, 12), link);
}
