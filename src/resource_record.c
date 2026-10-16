#include "psx.h"
/* Shared traversal verified separately against both 128-byte MIPS functions.
 * 64154 compares record+2 to an input halfword; 641D4 compares record+0 to 4. */
#include "resource_record.h"

static uint32_t find_record(uint32_t field, uint32_t value)
{
    uint32_t object = r_u32(0x8009C664), table, index, record, count, i;
    if (!object)
        return 0;
    table = r_u32(object + 4);
    if (!table)
        return 0;
    index = r_u8(object + 3);
    if (index >= 128)
        index |= 0xffffff00;
    table += 12 * index;
    count = r_u16(table + 6);
    record = r_u32(table + 8);
    if (count == 0 || count >= 32768)
        return 0;
    for (i = 0; i < count; ++i, record += 28)
        if (r_u16(record + field) == value)
            return record + 4;
    return 0;
}

uint32_t sub_80064154(uint32_t subtype)
{
    FUNCTION_MARKER(0x80064154u, "RASHCDF.BIN");
    return find_record(2, subtype & 65535);
}

uint32_t sub_800641D4(void)
{
    FUNCTION_MARKER(0x800641D4u, "RASHCDF.BIN");
    return find_record(0, 4);
}
