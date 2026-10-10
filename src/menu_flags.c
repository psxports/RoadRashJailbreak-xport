#include "psx.h"
/* SLUS8002D250: clear bits0..64, preserve upper seven bits of ninth byte. */
#include "menu_flags.h"

uint32_t sub_8002D250(void)
{
    FUNCTION_MARKER(0x8002D250u, "SLUS_010.53");
    uint32_t i;
    for (i = 0; i < 65; ++i)
    {
        uint32_t address = 0x800D81C8 + (i >> 3);
        w_u8(address, r_u8(address) & ~(1u << (i & 7)));
    }
    return 0;
}
