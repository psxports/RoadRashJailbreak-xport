#include "psx.h"
/* Original menu table calls use the two game arguments */
#include "menu_draw.h"
#include "menu_render.h"
#include <stdlib.h>

uint32_t sub_8006D3E0(uint32_t menu)
{
    uint32_t scalar_v_10;
    uint32_t scalar_v_11;

    FUNCTION_MARKER(0x8006D3E0u, "RASHCDF.BIN");
    uint32_t table = (int32_t)r_s16((menu + 2)) ? 0x8009CF78 : 0x8009CF28;
    uint32_t list = r_u32(menu + 16), index = 0, offset = 0;
    int32_t count;
    if (!list)
        return 0;
    count = (int32_t)r_s16((menu + 8));
    if (count <= 0)
        return (uint32_t)count;
    do
    {
        uint32_t entry = r_u32(menu + 16) + offset;
        uint32_t function = r_u32(table + (uint32_t)(int32_t)r_s16((entry + 8)) * 4);
        if (function)
        {
            uint32_t flags = r_u32(entry + 4);
            uint32_t active = 1u << ((uint32_t)((scalar_v_10 = (uint32_t)r_u8(((uint32_t)(0x800D80DC)))), (((int32_t)(scalar_v_10 < 128 ? (int32_t)scalar_v_10 : (int32_t)scalar_v_10 - 256)))) & 31);
            if (((flags & 0x3f00) >> 8) & active)
            {
                if ((flags & 0x02000000) && r_u32(0x800D742C + (uint32_t)((scalar_v_11 = (uint32_t)r_u8(((uint32_t)(0x8009C5E6)))), (((int32_t)(scalar_v_11 < 128 ? (int32_t)scalar_v_11 : (int32_t)scalar_v_11 - 256)))) * 24) != 1)
                    goto next;
                if ((flags & 0x08000000) && r_u32(0x800D80D8) != 4)
                    goto next;
                if ((flags & 0x04000000) && r_u32(0x800D80D8) == 4)
                    goto next;
                if ((flags & 0x10000000) && !(uint32_t)r_u8(0x800D80F3))
                    goto next;
                if ((flags & 0x80000000) && (int32_t)r_s16(0x8009C5D0) != (int32_t)r_s16(0x8009C5D8))
                    goto next;
                RRJMenuEntryFunction render = rrj_menu_entry_function(function);
                (void)render(menu, entry);
            }
        }
    next:
        count = (int32_t)r_s16((menu + 8));
        ++index;
        offset += 120;
    } while (((int32_t)(uint32_t)(index)) < count);
    return 0; /* Final SLT overwrites any renderer return. */
}

uint32_t sub_8006D630(uint32_t menu)
{
    FUNCTION_MARKER(0x8006D630u, "RASHCDF.BIN");
    w_u8(0x8009C5E1, (uint8_t)r_u8((menu + 12)));
    sub_8006D3E0(menu);
    return 1;
}
