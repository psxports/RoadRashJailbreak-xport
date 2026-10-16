#include "menu_update.h"
#include "resource.h"
#include "race_leaf.h"
#include "psx.h"
/* F80066C34: prepare/draw menus in original table order, then current menu. */
#include "menu_frame.h"
#include <stdlib.h>

void sub_80066C34(void)
{
    uint32_t scalar_v_1;
    uint32_t scalar_v_2;
    uint32_t scalar_v_3;
    uint32_t scalar_v_4;
    uint32_t scalar_v_5;
    uint32_t scalar_v_6;
    uint32_t scalar_v_7;
    uint32_t scalar_v_8;

    FUNCTION_MARKER(0x80066C34u, "RASHCDF.BIN");
    uint32_t index, menu, flags, fn, resource;
    w_u16(0x8009C5D8, (uint16_t)0);
    do
    {
        index = ((scalar_v_1 = ((uint32_t)r_u16(0x8009C5D8))), (((uint32_t)(scalar_v_1 < 32768 ? scalar_v_1 : scalar_v_1 | 0xffff0000))));
        if (index != ((scalar_v_2 = ((uint32_t)r_u16(0x8009C5D0))), (((uint32_t)(scalar_v_2 < 32768 ? scalar_v_2 : scalar_v_2 | 0xffff0000)))))
        {
            menu = r_u32(r_u32(0x8009C68C) + 4 * index);
            if ((uint32_t)r_u16((menu + 2)))
            {
                fn = r_u32(0x8009D0C0 + 4 * index);
                if (fn)
                    (void)rrj_menu_update_function(fn)(menu);
                else
                    w_u16((menu + 2), (uint16_t)0);
            }
            else
            {
                flags = (uint32_t)r_u16(menu);
                if (flags & 2)
                {
                    fn = r_u32(0x8009CFD0 + 4 * index);
                    if (fn)
                        (void)rrj_menu_update_function(fn)(menu);
                }
                else if (flags & 4)
                {
                    resource = ((scalar_v_3 = ((uint32_t)r_u16((menu + 6)))), (((uint32_t)(scalar_v_3 < 32768 ? scalar_v_3 : scalar_v_3 | 0xffff0000))));
                    w_u16(menu, (uint16_t)(flags & 0xfffb));
                    (void)sub_80078E80(r_u32(0x8009D3F0 + 4 * resource));
                }
            }
        }
        w_u16(0x8009C5D8, (uint16_t)((uint32_t)r_u16(0x8009C5D8) + 1));
    } while (((int32_t)(uint32_t)(((scalar_v_4 = ((uint32_t)r_u16(0x8009C5D8))), (((uint32_t)(scalar_v_4 < 32768 ? scalar_v_4 : scalar_v_4 | 0xffff0000)))))) < 59);
    index = ((scalar_v_5 = ((uint32_t)r_u16(0x8009C5D0))), (((uint32_t)(scalar_v_5 < 32768 ? scalar_v_5 : scalar_v_5 | 0xffff0000))));
    w_u16(0x8009C5D8, (uint16_t)index);
    menu = r_u32(r_u32(0x8009C68C) + 4 * index);
    flags = (uint32_t)r_u16(menu);
    if (!(flags & 4))
    {
        resource = r_u32(0x8009D3F0 + 4 * ((scalar_v_6 = ((uint32_t)r_u16((menu + 6)))), (((uint32_t)(scalar_v_6 < 32768 ? scalar_v_6 : scalar_v_6 | 0xffff0000)))));
        if (resource && sub_80078BD8(resource, (flags >> 14) & 1))
            w_u16(menu, (uint16_t)((uint32_t)r_u16(menu) | 4));
    }
    if (!r_u32(0x8008972C))
    {
        flags = (uint32_t)r_u16(menu);
        if ((flags & 0x1000) && sub_80078BD8(0x80089C84, (flags >> 13) & 1))
            w_u32(0x8008972C, 1);
    }
    index = ((scalar_v_7 = ((uint32_t)r_u16(0x8009C5D0))), (((uint32_t)(scalar_v_7 < 32768 ? scalar_v_7 : scalar_v_7 | 0xffff0000))));
    w_u16(0x8009C5D8, (uint16_t)index);
    menu = r_u32(r_u32(0x8009C68C) + 4 * index);
    if ((uint32_t)r_u16((menu + 2)))
    {
        if ((uint32_t)r_u16(menu) & 0x8000)
        {
            (void)(uint32_t)sub_80022A78(2);
            w_u32(0x8005ACAC, 0);
        }
        fn = r_u32(0x8009D0C0 + 4 * ((scalar_v_8 = ((uint32_t)r_u16(0x8009C5D8))), (((uint32_t)(scalar_v_8 < 32768 ? scalar_v_8 : scalar_v_8 | 0xffff0000)))));
        if (fn)
            (void)rrj_menu_update_function(fn)(menu);
        else
            w_u16((menu + 2), (uint16_t)0);
    }
    else if ((uint32_t)r_u16(menu) & 2)
    {
        fn = r_u32(0x8009CFD0 + 4 * index);
        if (fn)
            (void)rrj_menu_update_function(fn)(menu);
    }
}
