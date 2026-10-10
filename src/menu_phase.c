#include "psx.h"
/* F8006D5B0: transition-phase draw, preserving phase changes around renderer. */
#include "menu_phase.h"

uint32_t sub_F_8006D5B0(uint32_t menu, RRJMenuDraw draw)
{
    FUNCTION_MARKER(0x8006D5B0u, "RASHCDF.BIN");
    if (rrj_u16(rrj_at(menu, 2)) & 2)
    {
        w_u8(0x8009C5E1, r_u8(menu + 12));
        (void)sub_F_8006D3E0(menu, draw);
        rrj_put16(rrj_at(menu + 2, 2), 0);
    }
    else if (rrj_u16(rrj_at(menu + 2, 2)) == 32766)
    {
        uint8_t value = r_u8(menu + 13);
        rrj_put16(rrj_at(menu + 2, 2), 1);
        w_u8(0x8009C5E1, value);
        (void)sub_F_8006D3E0(menu, draw);
    }
    else
        rrj_put16(rrj_at(menu + 2, 2), 0);
    return 0;
}
