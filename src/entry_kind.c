#include "psx.h"
/* MIPS jump-table audit: status/menu/entry-kind/audit.json. */
#include "entry_kind.h"

uint32_t sub_80072100(uint32_t entry)
{
    FUNCTION_MARKER(0x80072100u, "RASHCDF.BIN");
    static const uint8_t kinds[80] = {
        0, 1, 9, 9, 3, 9, 9, 9, 0, 7, 7, 1, 6, 4, 9, 1, 9, 4, 6, 2, 0, 9, 9, 9, 6, 1, 7, 9, 9, 9, 9, 9, 0, 9, 6, 4, 1, 5, 0, 9, 6, 4, 1, 5, 1, 6, 4, 0, 9, 6, 4, 9, 1, 5, 9, 0, 9, 6, 9, 2, 9, 1, 4, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 0, 9, 8, 8,
    };
    uint32_t type = r_u16(entry + 8);
    uint32_t subtype;
    if (type != 12 && type != 13)
        return 0xffffffff;
    subtype = r_u16(entry + 18);
    return subtype < 80 ? kinds[subtype] : 9;
}
