#include "game_main.h"
#include "psx.h"
#include "wip.h"
/* Information string descriptor selection, checked against F MIPS. */
#include "menu_info.h"
#include "entry_kind.h"
#include "resource_record.h"
#include <stdlib.h>

static uint32_t result(uint32_t a, uint32_t value)
{
    w_u16(a, value);
    return a;
}

uint32_t sub_80063C7C(uint32_t selector)
{
    uint32_t scalar_v_16;
    uint32_t scalar_v_17;

    FUNCTION_MARKER(0x80063C7Cu, "RASHCDF.BIN");
    uint32_t entry, value, mode;
    switch (selector & 65535)
    {
        case 14:
            if ((uint32_t)r_u16(0x8009C5D8) != (uint32_t)r_u16(0x8009C5D0))
                return 0;
            entry = r_u32(0x8009C654);
            if (!entry || (uint32_t)r_u16((entry + 8)) != 12)
                return 0;
            value = 0;
            if (sub_80072100(entry) == 1)
            {
                mode = (uint32_t)r_u8(0x800D80DC);
                if (mode == 1)
                    value = 1545;
                else if (mode == 3)
                    value = 1546;
                else if (mode == 5)
                    value = 1547;
            }
            if (!value)
                value = (uint32_t)r_u16((r_u32(0x8009C654) + 20)) + 1;
            (void)result(0x8009C4A8, value);
            w_u8(0x8009C4AA, 3);
            w_u32(0x8009C4AC, 0xA08C8C);
            return 0x8009C4A8;
        case 32:
            value = (uint32_t)r_u8(0x800D80EF);
            if (value != 0 && value != 2)
                return 0;
            return result(0x80081120, value == 2 ? 201 : 200);
        case 33:
            return result(0x80081120, (uint32_t)r_u8(0x800D80F1) ? 6 : 7);
        case 34:
            value = sub_80071F0C(((scalar_v_16 = ((uint32_t)r_u8(0x800D80DC))), (((uint32_t)(scalar_v_16 < 128 ? scalar_v_16 : scalar_v_16 | 0xffffff00)))), r_u32(0x800D80D8));
            return result(0x800810F8, value);
        case 35:
            return result(0x80081100, (uint32_t)r_u8(0x800D80E1) ? 6 : 7);
        case 36:
            return result(0x80081108, (uint32_t)r_u8(0x800D80E2) ? 6 : 7);
        case 37:
            return result(0x80081110, (uint32_t)r_u8(0x800D80E3) ? 6 : 7);
        case 38:
            value = (uint32_t)r_u8(0x800D80EA);
            return result(0x80081118, value >= 2 && value <= 6 ? value + 7 : 8);
        case 39:
            return result(0x80081120, (uint32_t)r_u8((0x800D8198 + (uint32_t)r_u8(0x8009C5DE))) ? 6 : 7);
        case 41:
        case 42:
            value = (uint32_t)r_u8(((selector & 65535) == 41 ? 0x800D81ED : 0x800D8211));
            return result(0x80081120, value == 1 ? 196 : value == 2 ? 197 : 198);
        case 43:
            return result(0x80081108, (uint32_t)r_u8(0x800D80EC) ? 184 : 183);
        case 44:
            return result(0x80081120, (uint32_t)r_u8((0x800D81EF + 36 * ((scalar_v_17 = ((uint32_t)r_u8(0x8009C5E6))), (((uint32_t)(scalar_v_17 < 128 ? scalar_v_17 : scalar_v_17 | 0xffffff00)))))) ? 6 : 7);
        case 56:
            return result(0x80081108, (uint32_t)r_u8(0x8009C5E9) ? 4 : 5);
        default:
            return sub_80064154(selector);
    }
}
