#include "psx.h"
#include <stdlib.h>

#define XPORT_LINK_STUB(symbol, address, image) \
    uint32 symbol() \
    { \
        FUNCTION_MARKER(address, image); \
        abort(); \
    }

XPORT_LINK_STUB(sub_80015A18, 0x80015A18u, "SLUS_010.53")
XPORT_LINK_STUB(sub_80035E60, 0x80035E60u, "SLUS_010.53")
XPORT_LINK_STUB(sub_8003D39C, 0x8003D39Cu, "SLUS_010.53")
XPORT_LINK_STUB(sub_80050EC8, 0x80050EC8u, "SLUS_010.53")
