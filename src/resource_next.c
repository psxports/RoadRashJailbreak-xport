#include "psx.h"
/* Two-pass circular resource choice, F80062D6C actual MIPS. */
#include "resource_next.h"

static uint32_t blocked(uint32_t record, uint32_t kind)
{
    uint32_t bit = (uint32_t)r_u8((record + 1)), index = ((uint32_t)(((uint32_t)(bit)) < 128 ? ((uint32_t)(bit)) : ((uint32_t)(bit)) | 0xffffff00));
    index = (index >> 3) | (index & 0x80000000 ? 0xe0000000 : 0);
    return ((uint32_t)r_u8((0x800D80D8 + (kind == 4 ? 240 : 252) + index)) >> (bit & 7)) & 1;
}

static uint32_t eligible(uint32_t record, uint32_t pass, uint32_t flags)
{
    uint32_t scalar_v_15;
    uint32_t scalar_v_16;

    uint32_t kind;
    if (!((uint32_t)r_u8((record + 3)) & (uint32_t)r_u8(0x8009C5E4)))
        return 0;
    kind = (uint32_t)r_u8(record);
    if ((kind == 4 || kind == 8) && blocked(record, kind))
        return 0;
    if (pass)
        flags = (uint32_t)r_u8((record + 2));
    if ((flags & 64) && !r_u32(0x800D7138 + 192 * ((scalar_v_15 = ((uint32_t)r_u8(0x8009C5E6))), (((uint32_t)(scalar_v_15 < 128 ? scalar_v_15 : scalar_v_15 | 0xffffff00))))))
        return 0;
    if ((flags & 32) && r_u32(0x800D7138 + 192 * ((scalar_v_16 = ((uint32_t)r_u8(0x8009C5E6))), (((uint32_t)(scalar_v_16 < 128 ? scalar_v_16 : scalar_v_16 | 0xffffff00))))))
        return 0;
    if ((flags & 128) && ((uint32_t)r_u8(0x800D70E1) >> 4) != 8)
        return 0;
    return 1;
}

uint32_t sub_80062D6C(uint32_t resource)
{
    uint32_t scalar_v_17;
    uint32_t scalar_v_18;
    uint32_t scalar_v_19;

    FUNCTION_MARKER(0x80062D6Cu, "RASHCDF.BIN");
    uint32_t pass, seen = 0;
    for (pass = 0; pass < 2; ++pass)
    {
        uint32_t count = ((scalar_v_17 = ((uint32_t)r_u8(resource))), (((uint32_t)(scalar_v_17 < 128 ? scalar_v_17 : scalar_v_17 | 0xffffff00)))), index = ((scalar_v_18 = ((uint32_t)r_u8((resource + 3)))), (((uint32_t)(scalar_v_18 < 128 ? scalar_v_18 : scalar_v_18 | 0xffffff00)))), visited = 0;
        if (((int32_t)(uint32_t)(count)) <= 0)
            continue;
        do
        {
            uint32_t record = r_u32(resource + 4) + 12 * index, flags = pass ? 0 : (uint32_t)r_u8((record + 2));
            if (pass || (flags & 16) || seen)
            {
                if (eligible(record, pass, flags))
                    return index;
                seen = 1;
            }
            count = ((scalar_v_19 = ((uint32_t)r_u8(resource))), (((uint32_t)(scalar_v_19 < 128 ? scalar_v_19 : scalar_v_19 | 0xffffff00))));
            ++index;
            if (((int32_t)(uint32_t)(index)) >= ((int32_t)(uint32_t)(count)))
                index = 0;
            ++visited;
        } while (((int32_t)(uint32_t)(visited)) < ((int32_t)(uint32_t)(count)));
    }
    return 0xffffffff;
}
