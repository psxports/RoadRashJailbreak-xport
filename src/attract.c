#include "audio.h"
#include "psx.h"
/* F8006A8FC: attract/video menu input and return selection. */
#include "attract.h"
#include <stdlib.h>

uint32_t sub_8006A8FC(uint32_t menu)
{
    FUNCTION_MARKER(0x8006A8FCu, "RASHCDF.BIN");
    static const uint8_t offsets[] = {82, 74, 58, 66, 50, 42, 26, 34, 122, 130, 90, 98, 106, 114};
    int32_t index, end;
    uint32_t pad, i, current;
    uint32_t phase = r_u16(menu + 2);
    w_u32(0x8005ACAC, 0);
    if (phase)
        return 0;
    if (!(r_u16(0x8009C5DA) & 1))
    {
        current = r_u16(0x8009C5D0);
        if (current & 0x8000)
            current |= 0xffff0000;
        w_u16(0x8009C5D2, r_u16(0x8009C6C8 + 4 * current));
        return 1;
    }
    index = (int32_t)r_s8((menu + 14));
    end = index + (int32_t)r_s8((menu + 15));
    pad = 0x800D7128 + 192 * (uint32_t)index;
    while (index < end)
    {
        for (i = 0; i < sizeof offsets; ++i)
            if ((int32_t)r_s8((pad + offsets[i])) > 0)
            {
                (void)sub_8007EAC0(2);
                w_u16(0x8009C5DA, (uint16_t)(r_u16(0x8009C5DA) | 4));
                break;
            }
        end = (int32_t)r_s8((menu + 14)) + (int32_t)r_s8((menu + 15));
        ++index;
        pad += 192;
    }
    return 0;
}
