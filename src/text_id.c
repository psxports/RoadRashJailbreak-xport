/* Actual MIPS entry 800662CC includes the LUI/LW omitted by IDA. */
#include "text_id.h"
#include "word_wrap.h"

uint32_t sub_800662CC(uint32_t font, uint32_t id, const uint16_t rect[4], uint32_t gap, uint32_t link, uint32_t color)
{
    uint32_t table;
    FUNCTION_MARKER(0x800662CC, "RASHCDF.BIN");
    table = r_u32(0x8005B544);
    if (!table)
        return 0x80060000; /* v0 retains the entry LUI result. */
    gap &= 65535;
    if (gap >= 32768)
        gap |= 0xffff0000;
    return sub_80066318(font, r_u32(table + (id << 2)), rect, gap, link, color);
}
