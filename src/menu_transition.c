#include "menu_reset.h"
#include "native_game_api.h"
#include "psx.h"
/* Menu hierarchy transition. Original flags, parent order and reloads retained. */
#include "menu_transition.h"
#include <stdlib.h>

uint32_t sub_80080A70(uint32_t menu)
{
    FUNCTION_MARKER(0x80080A70u, "RASHCDF.BIN");
    uint32_t list = r_u32(menu + 16), count, index;
    if (!list)
        return 0;
    count = (uint32_t)r_u16((menu + 8));
    if (!count || count >= 32768)
        return 0;
    for (index = 0; index < count; ++index, list += 120)
        if ((uint32_t)r_u16((list + 10)) & 0x1000)
            return index;
    return 0;
}

uint32_t sub_80066EF8(void)
{
    uint32_t scalar_v_1;
    uint32_t scalar_v_2;
    uint32_t scalar_v_3;
    uint32_t scalar_v_4;
    uint32_t scalar_v_5;
    uint32_t scalar_v_6;
    uint32_t scalar_v_7;
    uint32_t scalar_v_8;
    uint32_t scalar_v_9;
    uint32_t scalar_v_10;
    uint32_t scalar_v_11;

    FUNCTION_MARKER(0x80066EF8u, "RASHCDF.BIN");
    uint32_t current = ((scalar_v_1 = ((uint32_t)r_u16(0x8009C5D0))), (((uint32_t)(scalar_v_1 < 32768 ? scalar_v_1 : scalar_v_1 | 0xffff0000)))), next = ((scalar_v_2 = ((uint32_t)r_u16(0x8009C5D2))), (((uint32_t)(scalar_v_2 < 32768 ? scalar_v_2 : scalar_v_2 | 0xffff0000)))), shared = 0, table, menu, ancestor, value;
    if (current == next)
        return next;
    w_u32(0x8005ACAC, 0);
    w_u32(0x80088C50, 0);
    if (current != 0xffffffff)
    {
        table = r_u32(0x8009C68C);
        do
        {
            next = ((scalar_v_3 = ((uint32_t)r_u16(0x8009C5D2))), (((uint32_t)(scalar_v_3 < 32768 ? scalar_v_3 : scalar_v_3 | 0xffff0000))));
            menu = r_u32(table + 4 * current);
            while (next != 0xffffffff)
            {
                ancestor = r_u32(table + 4 * next);
                if (current == next)
                {
                    shared = 1;
                    break;
                }
                next = ((scalar_v_4 = ((uint32_t)r_u16((ancestor + 10)))), (((uint32_t)(scalar_v_4 < 32768 ? scalar_v_4 : scalar_v_4 | 0xffff0000))));
            }
            if (!shared)
            {
                value = (uint32_t)r_u16((menu + 2));
                w_u16(menu, (uint16_t)((uint32_t)r_u16(menu) & 0xfffc));
                if (!value)
                    w_u16((menu + 2), (uint16_t)0x7ffe);
            }
            current = ((scalar_v_5 = ((uint32_t)r_u16((menu + 10)))), (((uint32_t)(scalar_v_5 < 32768 ? scalar_v_5 : scalar_v_5 | 0xffff0000))));
        } while (current != 0xffffffff);
    }
    next = ((scalar_v_6 = ((uint32_t)r_u16(0x8009C5D2))), (((uint32_t)(scalar_v_6 < 32768 ? scalar_v_6 : scalar_v_6 | 0xffff0000))));
    menu = r_u32(r_u32(0x8009C68C) + 4 * next);
    if (!((uint32_t)r_u16(menu) & 3) && !(uint32_t)r_u16((menu + 2)))
        w_u16((menu + 2), (uint16_t)0x7fff);
    current = ((scalar_v_7 = ((uint32_t)r_u16(0x8009C5D0))), (((uint32_t)(scalar_v_7 < 32768 ? scalar_v_7 : scalar_v_7 | 0xffff0000))));
    if (current == 4)
    {
        w_u8(0x800D80E8, (uint8_t)current);
        current = ((scalar_v_8 = ((uint32_t)r_u16(0x8009C5D0))), (((uint32_t)(scalar_v_8 < 32768 ? scalar_v_8 : scalar_v_8 | 0xffff0000))));
    }
    if (((int32_t)(uint32_t)(current)) >= 5 && (((int32_t)(uint32_t)(current)) < 8 || current == 29))
        sub_8006883C(((scalar_v_9 = ((uint32_t)r_u16(0x8009C5D2))), (((uint32_t)(scalar_v_9 < 32768 ? scalar_v_9 : scalar_v_9 | 0xffff0000)))));
    if ((uint32_t)r_u16(0x8009C5D0) == 4)
    {
        menu = r_u32(r_u32(0x8009C68C) + 4 * ((scalar_v_10 = ((uint32_t)r_u16(0x8009C5D2))), (((uint32_t)(scalar_v_10 < 32768 ? scalar_v_10 : scalar_v_10 | 0xffff0000)))));
        value = sub_80080A70(menu);
        menu = r_u32(r_u32(0x8009C68C) + 4 * ((scalar_v_11 = ((uint32_t)r_u16(0x8009C5D2))), (((uint32_t)(scalar_v_11 < 32768 ? scalar_v_11 : scalar_v_11 | 0xffff0000)))));
        w_u16((menu + 4), (uint16_t)value);
    }
    next = (uint32_t)r_u16(0x8009C5D2);
    table = r_u32(0x8009C68C);
    w_u16(0x8009C5D0, (uint16_t)next);
    menu = r_u32(table + 4 * ((uint32_t)(((uint32_t)(next)) < 32768 ? ((uint32_t)(next)) : ((uint32_t)(next)) | 0xffff0000)));
    value = (uint32_t)r_u16(menu) | 3;
    w_u32(0x8009C5C8, menu);
    w_u16(menu, (uint16_t)value);
    return value;
}

uint32_t sub_8006711C(void)
{
    FUNCTION_MARKER(0x8006711Cu, "RASHCDF.BIN");
    uint32_t current;
    w_u32(0x8009C658, r_u32(0x8009C654));
    current = (uint32_t)r_u16(0x8009C5D0);
    if (current == 5 || current == 29)
    {
        w_u8(0x800D80F0, (uint8_t)0);
        w_u8(0x800D80EA, (uint8_t)1);
        w_u8(0x800D80DC, (uint8_t)0);
        (uint32_t)sub_8002D250();
    }
    return sub_80066EF8();
}
