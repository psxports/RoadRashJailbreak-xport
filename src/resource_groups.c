#include "psx.h"
/* Resource-group selection and mask, actual F-overlay MIPS. */
#include "resource_groups.h"

uint32_t sub_80064254(uint32_t mode)
{
    FUNCTION_MARKER(0x80064254u, "RASHCDF.BIN");
    uint32_t mask = mode < 6 ? 1u << mode : 1;
    w_u8(0x8009C5E4, (uint8_t)mask);
    return mask; /* v1=800A0000 is a scratch register, not a 64-bit result. */
}

uint32_t sub_80068448(void)
{
    FUNCTION_MARKER(0x80068448u, "RASHCDF.BIN");
    uint32_t mode = r_u32(0x800D80D8), first, second, last, index;
    switch (mode)
    {
        case 32:
            first = r_u32(0x8009C4BC);
            index = (uint32_t)r_u8(0x800D81E1);
            w_u32(0x8009C668, first);
            if (index > 1)
            {
                w_u32(0x8009C66C, 0);
                w_u32(0x8009C670, 0);
                return 0x8009C5D0;
            }
            last = r_u32(0x8009C4C0 + 4 * index);
            w_u32(0x8009C670, 0);
            w_u32(0x8009C66C, last);
            return last;
        case 4:
        case 8:
            first = r_u32(mode == 4 ? 0x8009C4D0 : 0x8009C508);
            last = r_u32(mode == 4 ? 0x8009C4CC : 0x8009C50C);
            w_u32(0x8009C670, 0);
            w_u32(0x8009C668, first);
            w_u32(0x8009C66C, last);
            return last;
        case 16:
        case 24:
            first = r_u32(0x8009C4EC);
            second = r_u32(mode == 16 ? 0x8009C4F0 : 0x8009C510);
            last = r_u32(mode == 16 ? 0x8009C4F4 : 0x8009C514);
            w_u32(0x8009C668, first);
            w_u32(0x8009C66C, second);
            w_u32(0x8009C670, last);
            return last;
        case 17:
            first = r_u32(0x8009C4F8);
            index = (uint32_t)r_u8(0x800D81DF);
            second = r_u32(0x8009C4FC);
            w_u32(0x8009C668, first);
            w_u32(0x8009C66C, second);
            last = r_u32((index < 18 || index >= 128) ? 0x8009C500 : 0x8009C504);
            w_u32(0x8009C670, last);
            return last;
        default:
            w_u32(0x8009C668, 0);
            w_u32(0x8009C66C, 0);
            w_u32(0x8009C670, 0);
            return 0x8009C5D0;
    }
}
