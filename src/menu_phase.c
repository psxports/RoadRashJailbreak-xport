#include "psx.h"
/* F8006D5B0: transition-phase draw, preserving phase changes around renderer. */
#include "menu_phase.h"

uint32_t sub_8006D5B0(uint32_t menu)
{
    FUNCTION_MARKER(0x8006D5B0u, "RASHCDF.BIN");
    if (r_u16(menu) & 2)
    {
        w_u8(0x8009C5E1, r_u8(menu + 12));
        (void)sub_8006D3E0(menu);
        w_u16(menu + 2, 0);
    }
    else if (r_u16(menu + 2) == 32766)
    {
        uint8_t value = r_u8(menu + 13);
        w_u16(menu + 2, 1);
        w_u8(0x8009C5E1, value);
        (void)sub_8006D3E0(menu);
    }
    else
        w_u16(menu + 2, 0);
    return 0;
}
