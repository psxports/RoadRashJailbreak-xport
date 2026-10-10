#include "psx.h"
/* Select first signed-byte value in a resource directory. */
#include "resource_value.h"

static uint32_t signed_byte(RRJMemory *m, uint32_t address)
{
    uint32_t value = r_u8(address);
    return value < 128 ? value : value | 0xffffff00;
}

uint32_t sub_F_800630C0(uint32_t resource, uint32_t value)
{
    FUNCTION_MARKER(0x800630C0u, "RASHCDF.BIN");
    uint32_t count = signed_byte(rrj_host_context(), resource), table, index;
    if (rrj_s32(count) <= 0)
        return count;
    table = rrj_read32(resource + 4);
    for (index = 0; index < count; ++index, table += 12)
    {
        uint32_t current = signed_byte(rrj_host_context(), table + 1);
        if (current == value)
        {
            w_u8(resource + 3, (uint8_t)index);
            return current;
        }
    }
    return 0;
}
