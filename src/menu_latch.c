#include "psx.h"
#include "wip.h"
/* Guest word copy used by the canonical input latch */
#include "menu_latch.h"
#include <stdio.h>
#include <stdlib.h>

void *sub_8001E0B4(void *destination, const void *source, uint32_t bytes)
{
    const uint8_t *input = (const uint8_t *)source;
    uint8_t *output = (uint8_t *)destination;

    FUNCTION_MARKER(0x8001E0B4, "SLUS_010.53");
    while (bytes)
    {
        uint32_t value = xport_load_le32(input);
        input += 4;
        bytes -= 4;
        xport_store_le32(output, value);
        output += 4;
    }
    return destination;
}
