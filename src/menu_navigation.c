#include "audio.h"
#include "psx.h"
#include "menu_navigation.h"
#include <stdlib.h>

/* Shared predicate follows the same ordered MIPS block in both functions.
 * IDA's unsigned byte for the player index is wrong: MIPS uses LB. The
 * resource mode is loaded once by the caller, before scanning entries. */
static int selectable(uint32_t entry, uint32_t mode)
{
    uint32_t type = r_u16(entry + 8);
    uint32_t flags;
    if (type != 12 && type != 13 && type != 17)
        return 0;
    flags = r_u32(entry);
    if ((flags & 0x40000000) && (r_u8(0x800D70E1) >> 4) != 8)
        return 0;
    if (flags & 0x02000000)
    {
        uint32_t bits = r_u8(0x8009C5E6);
        int32_t player = bits < 128 ? (int32_t)bits : (int32_t)bits - 256;
        if (r_u32(0x800D742C + (uint32_t)player * 24) != 1)
            return 0;
    }
    if ((flags & 0x10000000) && !r_u8(0x800D80F3))
        return 0;
    if ((flags & 0x08000000) && mode != 4)
        return 0;
    if ((flags & 0x04000000) && mode == 4)
        return 0;
    return 1;
}

/* F:8006C3A4, 436 bytes. The 0x7fff sentinel resets selection before
 * comparison, so it reports the unchanged-selection sound for index zero.
 * The selected halfword is stored AFTER the sound call, as in the original. */
uint32_t sub_8006C3A4(uint32_t menu)
{
    FUNCTION_MARKER(0x8006C3A4u, "RASHCDF.BIN");
    int32_t old = (int32_t)r_s16((menu + 4)), next = old + 1;
    uint32_t result, event = 4;
    if (old == 0x7fff)
    {
        w_u16(menu + 4, 0);
        next = 0;
    }
    else if (next >= (int32_t)r_s16((menu + 8)))
        next = 0;
    old = (int32_t)r_s16((menu + 4));
    if (next != old)
    {
        uint32_t mode = r_u32(0x800D80D8);
        uint32_t offset = (uint32_t)next * 120;
        do
        {
            if (selectable(r_u32(menu + 16) + offset, mode))
                break;
            ++next;
            offset += 120;
            if (next >= (int32_t)r_s16((menu + 8)))
            {
                next = 0;
                offset = 0;
            }
        } while (next != old);
        event = next == (int32_t)r_s16((menu + 4)) ? 4 : 0;
    }
    result = sub_8007EAC0(event);
    w_u16(menu + 4, (uint32_t)next);
    return result;
}

/* F:8006C558, 424 bytes. Unlike the forward scan, the sentinel resets to
 * zero and then decrements/wraps, selecting the last eligible entry. */
uint32_t sub_8006C558(uint32_t menu)
{
    FUNCTION_MARKER(0x8006C558u, "RASHCDF.BIN");
    int32_t old = (int32_t)r_s16((menu + 4)), next = old - 1;
    uint32_t result, event = 4;
    if (old == 0x7fff)
    {
        w_u16(menu + 4, 0);
        old = (int32_t)r_s16((menu + 4));
        next = old - 1;
    }
    if (next < 0)
        next = (int32_t)r_s16((menu + 8)) - 1;
    if (next != old)
    {
        uint32_t mode = r_u32(0x800D80D8);
        do
        {
            uint32_t entry = r_u32(menu + 16) + (uint32_t)next * 120;
            if (selectable(entry, mode))
                break;
            --next;
            if (next < 0)
                next = (int32_t)r_s16((menu + 8)) - 1;
        } while (next != old);
        event = next == (int32_t)r_s16((menu + 4)) ? 4 : 0;
    }
    result = sub_8007EAC0(event);
    w_u16(menu + 4, (uint32_t)next);
    return result;
}
