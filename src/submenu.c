#include "menu_input.h"
#include "audio.h"
#include "psx.h"
/* F8006AE6C generic submenu input wrapper, actual MIPS ordering. */
#include "submenu.h"
#include <stdlib.h>

uint32_t sub_8006AE6C(uint32_t menu)
{
    uint32_t scalar_v_12;
    uint32_t scalar_v_13;
    uint32_t scalar_v_14;
    uint32_t scalar_v_15;
    uint32_t scalar_v_16;
    uint32_t scalar_v_17;
    uint32_t scalar_v_18;
    uint32_t scalar_v_19;
    uint32_t scalar_v_20;

    uint32_t scalar_v_10;
    uint32_t scalar_v_11;

    FUNCTION_MARKER(0x8006AE6Cu, "RASHCDF.BIN");
    uint32_t item = r_u32(menu + 16), selected, index, end, done = 0;
    if (item)
    {
        selected = item + 120 * ((scalar_v_10 = ((uint32_t)r_u16((menu + 4)))), (((uint32_t)(scalar_v_10 & 0x8000 ? scalar_v_10 | 0xffff0000 : scalar_v_10))));
        if (((int32_t)(uint32_t)(r_u32(0x800D717C))) > 0 && ((int32_t)(uint32_t)(r_u32(0x800D718C))) > 0 && ((int32_t)(uint32_t)(r_u32(0x800D7194))) > 0 && ((int32_t)(uint32_t)(r_u32(0x800D713C))) > 0 && ((int32_t)(uint32_t)(((scalar_v_12 = ((uint32_t)r_u8(0x800D717A))), (((uint32_t)(scalar_v_12 & 128 ? scalar_v_12 | 0xffffff00 : scalar_v_12)))))) > 0 && (uint32_t)r_u16((selected + 8)) == 12 && (uint32_t)r_u16((selected + 18)) == 71)
        {
            w_u16(0x8009C5D2, 56);
            done = 1;
        }
        index = ((scalar_v_13 = ((uint32_t)r_u8((menu + 14)))), (((uint32_t)(scalar_v_13 & 128 ? scalar_v_13 | 0xffffff00 : scalar_v_13))));
        end = index + ((scalar_v_14 = ((uint32_t)r_u8((menu + 15)))), (((uint32_t)(scalar_v_14 & 128 ? scalar_v_14 | 0xffffff00 : scalar_v_14))));
        while (((int32_t)(uint32_t)(index)) < ((int32_t)(uint32_t)(end)) && !done)
        {
            if (((int32_t)(uint32_t)(((scalar_v_15 = ((uint32_t)r_u8((0x800D7128 + 192 * index + 74)))), (((uint32_t)(scalar_v_15 & 128 ? scalar_v_15 | 0xffffff00 : scalar_v_15)))))) > 0 && r_u32(0x8009C7B8 + 4 * ((scalar_v_11 = ((uint32_t)r_u16(0x8009C5D0))), (((uint32_t)(scalar_v_11 & 0x8000 ? scalar_v_11 | 0xffff0000 : scalar_v_11))))) != 0xffffffff)
            {
                (void)sub_8007EAC0(3);
                done = 1;
                w_u16(0x8009C5D2, (uint16_t)((scalar_v_16 = ((uint32_t)r_u8(0x800D80E8))), (((uint32_t)(scalar_v_16 & 128 ? scalar_v_16 | 0xffffff00 : scalar_v_16)))));
            }
            end = ((scalar_v_17 = ((uint32_t)r_u8((menu + 14)))), (((uint32_t)(scalar_v_17 & 128 ? scalar_v_17 | 0xffffff00 : scalar_v_17)))) + ((scalar_v_18 = ((uint32_t)r_u8((menu + 15)))), (((uint32_t)(scalar_v_18 & 128 ? scalar_v_18 | 0xffffff00 : scalar_v_18))));
            ++index;
        }
    }
    if (done)
        return done;
    index = ((scalar_v_19 = ((uint32_t)r_u8((menu + 14)))), (((uint32_t)(scalar_v_19 & 128 ? scalar_v_19 | 0xffffff00 : scalar_v_19))));
    end = index + ((scalar_v_20 = ((uint32_t)r_u8((menu + 15)))), (((uint32_t)(scalar_v_20 & 128 ? scalar_v_20 | 0xffffff00 : scalar_v_20))));
    return sub_8006B03C(menu, index, end);
}
