/* F8006D5B0: transition-phase draw, preserving phase changes around renderer. */
#include "menu_phase.h"

uint32_t sub_F_8006D5B0(RRJMemory *m, uint32_t menu, RRJMenuDraw draw)
{
    if (rrj_u16(rrj_at(m, menu, 2)) & 2)
    {
        w_u8(0x8009C5E1, r_u8(menu + 12));
        (void)sub_F_8006D3E0(m, menu, draw);
        rrj_put16(rrj_at(m, menu + 2, 2), 0);
    }
    else if (rrj_u16(rrj_at(m, menu + 2, 2)) == 32766)
    {
        uint8_t value = r_u8(menu + 13);
        rrj_put16(rrj_at(m, menu + 2, 2), 1);
        w_u8(0x8009C5E1, value);
        (void)sub_F_8006D3E0(m, menu, draw);
    }
    else
        rrj_put16(rrj_at(m, menu + 2, 2), 0);
    return 0;
}
