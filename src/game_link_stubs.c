#include "psx.h"
#include <stdlib.h>

#define XPORT_LINK_STUB(symbol, address, image) \
    uint32 symbol() \
    { \
        FUNCTION_MARKER(address, image); \
        abort(); \
    }
