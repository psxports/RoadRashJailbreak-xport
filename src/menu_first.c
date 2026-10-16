#include "psx.h"
/* First eligible menu entry. MIPS caches list, count and resource mode. */
#include "menu_first.h"

uint32_t sub_80080ACC(uint32_t menu)
{
    uint32_t scalar_v_4;

    FUNCTION_MARKER(0x80080ACCu, "RASHCDF.BIN");
    uint32_t list = r_u32(menu + 16), count, mode, index;
    if (!list)
        return 0;
    count = r_u16(menu + 8);
    if (!count || count >= 32768)
        return 0;
    mode = r_u32(0x800D80D8);
    for (index = 0; index < count; ++index, list += 120)
    {
        uint32_t type = r_u16(list + 8), flags;
        if (type != 12 && type != 13 && type != 17)
            continue;
        flags = r_u32(list);
        if ((flags & 0x40000000) && ((uint32_t)r_u8(0x800D70E1) >> 4) != 8)
            continue;
        if ((flags & 0x02000000) && r_u32(0x800D742C + 24 * ((scalar_v_4 = ((uint32_t)r_u8(0x8009C5E6))), (((uint32_t)(scalar_v_4 < 128 ? scalar_v_4 : scalar_v_4 | 0xffffff00))))) != 1)
            continue;
        if ((flags & 0x10000000) && !(uint32_t)r_u8(0x800D80F3))
            continue;
        if ((flags & 0x08000000) && mode != 4)
            continue;
        if ((flags & 0x04000000) && mode == 4)
            continue;
        return index;
    }
    return 0;
}
