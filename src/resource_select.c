#include "psx.h"
/* IDA draft refined against MIPS; table evidence in resource-select-table.json. */
#include "resource_select.h"

uint32_t sub_800649BC(uint32_t resource)
{
    uint32_t scalar_v_14;
    uint32_t scalar_v_16;
    uint32_t scalar_v_17;

    FUNCTION_MARKER(0x800649BCu, "RASHCDF.BIN");
    int32_t count = ((int32_t)(uint32_t)(((scalar_v_14 = ((uint32_t)r_u8(resource))), (((uint32_t)(scalar_v_14 < 128 ? scalar_v_14 : scalar_v_14 | 0xffffff00))))));
    uint32_t found = 0, i;
    for (i = 0; ((int32_t)(uint32_t)(i)) < count; ++i)
    {
        uint32_t record = r_u32(resource + 4) + 12 * i, kind, flags, bit, index;
        if (!((uint32_t)r_u8((record + 3)) & (uint32_t)r_u8(0x8009C5E4)))
            continue;
        kind = (uint32_t)r_u8(record);
        if (kind == 4 || kind == 8)
        {
            bit = (uint32_t)r_u8((record + 1));
            index = ((uint32_t)(((uint32_t)(bit)) < 128 ? ((uint32_t)(bit)) : ((uint32_t)(bit)) | 0xffffff00));
            index = (index >> 3) | (index & 0x80000000 ? 0xe0000000 : 0);
            if (((uint32_t)r_u8((0x800D80D8 + (kind == 4 ? 240 : 252) + index)) >> (bit & 7)) & 1)
                continue;
        }
        flags = (uint32_t)r_u8((record + 2));
        if ((flags & 64) && !r_u32(0x800D7138 + 192 * ((scalar_v_16 = ((uint32_t)r_u8(0x8009C5E6))), (((uint32_t)(scalar_v_16 < 128 ? scalar_v_16 : scalar_v_16 | 0xffffff00))))))
            continue;
        if ((flags & 32) && r_u32(0x800D7138 + 192 * ((scalar_v_17 = ((uint32_t)r_u8(0x8009C5E6))), (((uint32_t)(scalar_v_17 < 128 ? scalar_v_17 : scalar_v_17 | 0xffffff00))))))
            continue;
        if ((flags & 128) && ((uint32_t)r_u8(0x800D70E1) >> 4) != 8)
            continue;
        ++found;
    }
    return found;
}

uint32_t sub_800680E8(uint32_t menu, uint32_t entry)
{
    uint32_t scalar_v_18;
    uint32_t scalar_v_19;

    FUNCTION_MARKER(0x800680E8u, "RASHCDF.BIN");
    static const uint32_t globals[72] = {0x8009C4B0, 0, 0, 0x8009C4BC, 0, 0, 0, 0x8009C4C8, 0x8009C4CC, 0x8009C4D0, 0, 0, 0x8009C4DC, 0x8009C4E0, 0x8009C4E4, 0x8009C4D8, 0, 0x8009C4E8, 0, 0, 0, 0, 0, 0, 0x8009C4C8, 0x8009C4EC, 0x8009C4F0, 0, 0x8009C4F4, 0, 0x8009C4C8, 0x8009C4F8, 0x8009C4FC, 0, 0, 0, 0x8009C508, 0x8009C50C, 0, 0x8009C4C8, 0x8009C4EC, 0x8009C510, 0x8009C518, 0, 0x8009C514, 0x8009C51C, 0, 0x8009C4C8, 0x8009C4D0, 0x8009C4D4, 0, 0, 0, 0x8009C4CC, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x8009C524, 0x8009C528, 0x8009C52C, 0x8009C520, 0, 0x8009C534, 0x8009C538, 0x8009C53C, 0x8009C540};
    uint32_t result = 0x8009C5D0, address = 0, selector, resource, table;
    w_u32(0x8009C664, 0);
    w_u8(0x8009C5E7, 0);
    if (!entry)
        return result;
    if ((uint32_t)r_u16((entry + 8)) == 13)
    {
        selector = (uint32_t)r_u16((entry + 18));
        if (selector >= 9 && selector <= 80)
            address = globals[selector - 9];
        if (selector == 10 || selector == 13)
        {
            uint32_t mode = (uint32_t)r_u8(0x800D81E1);
            if (mode <= 1)
                address = (selector == 10 ? 0x8009C4B4 : 0x8009C4C0) + 4 * mode;
        }
        if (selector == 43)
            address = ((int32_t)(uint32_t)(((scalar_v_18 = ((uint32_t)r_u8(0x800D81DF))), (((uint32_t)(scalar_v_18 < 128 ? scalar_v_18 : scalar_v_18 | 0xffffff00)))))) < 18 ? 0x8009C500 : 0x8009C504;
        if (address)
            w_u32(0x8009C664, r_u32(address));
    }
    result = r_u32(0x8009C678);
    resource = r_u32(0x8009C664);
    w_u32(0x8009C674, result);
    if (!resource)
        return result;
    table = r_u32(resource + 4);
    if (!table)
        return result;
    w_u32(0x8009C678, table + 12 * ((scalar_v_19 = ((uint32_t)r_u8((resource + 3)))), (((uint32_t)(scalar_v_19 < 128 ? scalar_v_19 : scalar_v_19 | 0xffffff00)))));
    result = sub_800649BC(resource);
    w_u8(0x8009C5E7, (uint8_t)result);
    return result;
}
