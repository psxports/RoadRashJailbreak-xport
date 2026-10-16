#include "psx.h"
/* Select first signed-byte value in a resource directory. */
#include "resource_value.h"

uint32_t sub_800630C0(uint32_t resource, uint32_t value)
{
    FUNCTION_MARKER(0x800630C0u, "RASHCDF.BIN");
    uint32_t count = (uint32_t)r_s8(resource), table, index;
    if (((int32_t)(uint32_t)(count)) <= 0)
        return count;
    table = r_u32(resource + 4);
    for (index = 0; index < count; ++index, table += 12)
    {
        uint32_t current = (uint32_t)r_s8((table + 1));
        if (current == value)
        {
            w_u8(resource + 3, (uint8_t)index);
            return current;
        }
    }
    return 0;
}
