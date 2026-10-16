#include "psx.h"
/* Resource refresh pass, derived from IDA and F8006738C MIPS. */
#include "resource_refresh.h"
#include "resource_groups.h"
#include "resource_next.h"
#include "resource_apply.h"
#include "resource_value.h"
#include "menu_first.h"

static uint32_t unavailable(uint32_t root)
{
    uint32_t scalar_v_23;
    uint32_t scalar_v_25;
    uint32_t scalar_v_26;

    uint32_t record = r_u32(root + 4) + 12 * ((scalar_v_23 = ((uint32_t)r_u8((root + 3)))), (((uint32_t)(scalar_v_23 < 128 ? scalar_v_23 : scalar_v_23 | 0xffffff00)))), kind, bit, index, flags;
    if (!((uint32_t)r_u8((record + 3)) & (uint32_t)r_u8(0x8009C5E4)))
        return 1;
    kind = (uint32_t)r_u8(record);
    if (kind == 4 || kind == 8)
    {
        bit = (uint32_t)r_u8((record + 1));
        index = ((uint32_t)(((uint32_t)(bit)) < 128 ? ((uint32_t)(bit)) : ((uint32_t)(bit)) | 0xffffff00));
        index = (index >> 3) | (index & 0x80000000 ? 0xe0000000 : 0);
        if (((uint32_t)r_u8((0x800D80D8 + (kind == 4 ? 240 : 252) + index)) >> (bit & 7)) & 1)
            return 1;
    }
    flags = (uint32_t)r_u8((record + 2));
    if ((flags & 64) && !r_u32(0x800D7138 + 192 * ((scalar_v_25 = ((uint32_t)r_u8(0x8009C5E6))), (((uint32_t)(scalar_v_25 < 128 ? scalar_v_25 : scalar_v_25 | 0xffffff00))))))
        return 1;
    if ((flags & 32) && r_u32(0x800D7138 + 192 * ((scalar_v_26 = ((uint32_t)r_u8(0x8009C5E6))), (((uint32_t)(scalar_v_26 < 128 ? scalar_v_26 : scalar_v_26 | 0xffffff00))))))
        return 1;
    return (flags & 128) && ((uint32_t)r_u8(0x800D70E1) >> 4) != 8;
}

static void refresh(uint32_t slot, uint32_t apply)
{
    uint32_t root = r_u32(slot);
    if (!root || !r_u32(root + 4))
        return;
    if (unavailable(root))
        w_u8((root + 3), (uint8_t)(sub_80062D6C(r_u32(slot))));
    if (apply)
        sub_8006310C(root);
}

static uint32_t menu_ineligible(uint32_t entry)
{
    uint32_t scalar_v_27;

    uint32_t type = (uint32_t)r_u16((entry + 8)), flags;
    if (type != 12 && type != 13 && type != 17)
        return 1;
    flags = r_u32(entry);
    if ((flags & 0x40000000) && ((uint32_t)r_u8(0x800D70E1) >> 4) != 8)
        return 1;
    if ((flags & 0x02000000) && r_u32(0x800D742C + 24 * ((scalar_v_27 = ((uint32_t)r_u8(0x8009C5E6))), (((uint32_t)(scalar_v_27 < 128 ? scalar_v_27 : scalar_v_27 | 0xffffff00))))) != 1)
        return 1;
    if ((flags & 0x10000000) && !(uint32_t)r_u8(0x800D80F3))
        return 1;
    if ((flags & 0x08000000) && r_u32(0x800D80D8) != 4)
        return 1;
    return (flags & 0x04000000) && r_u32(0x800D80D8) == 4;
}

void sub_8006738C(uint32_t context)
{
    uint32_t scalar_v_32;
    uint32_t scalar_v_33;

    uint32_t scalar_v_28;
    uint32_t scalar_v_29;
    uint32_t scalar_v_30;
    uint32_t scalar_v_31;

    FUNCTION_MARKER(0x8006738Cu, "RASHCDF.BIN");
    uint32_t mode = (uint32_t)r_u8(0x800D80DC), menu, entry, i, root, index;
    if (!(r_u32(0x800D80D8) & 32) && mode != 0 && mode != 2 && mode != 4)
        w_u8(0x800D80DC, (uint8_t)0);
    (void)sub_80064254(((scalar_v_28 = ((uint32_t)r_u8(0x800D80DC))), (((uint32_t)(scalar_v_28 < 128 ? scalar_v_28 : scalar_v_28 | 0xffffff00)))));
    (void)sub_80068448();
    for (i = 0; i < 3; ++i)
        refresh(0x8009C668 + 4 * i, 1);
    if (r_u32(0x800D80D8) == 1)
        refresh(0x8009C4E8, 1);
    refresh(0x8009C664, 0);
    if (((uint32_t)r_u8(0x800D70E1) >> 4) != 8)
    {
        if (r_u32(0x800D80D8) == 8)
            w_u8(0x800D81ED, (uint8_t)1);
        else
        {
            if ((uint32_t)r_u8(0x800D81ED) == 1)
            {
                w_u8(0x800D81ED, (uint8_t)2);
                (void)sub_800630C0(r_u32(0x8009C518), 2);
            }
            if ((uint32_t)r_u8(0x800D8211) == 1)
            {
                w_u8(0x800D8211, (uint8_t)2);
                (void)sub_800630C0(r_u32(0x8009C51C), 2);
            }
        }
    }
    menu = r_u32(r_u32(0x8009C68C) + 4 * ((scalar_v_32 = ((uint32_t)r_u16(0x8009C5D0))), (((uint32_t)(scalar_v_32 < 32768 ? scalar_v_32 : scalar_v_32 | 0xffff0000)))));
    w_u32(0x8009C5C8, menu);
    entry = r_u32(menu + 16) + 120 * ((scalar_v_33 = ((uint32_t)r_u16((menu + 4)))), (((uint32_t)(scalar_v_33 < 32768 ? scalar_v_33 : scalar_v_33 | 0xffff0000))));
    if (menu_ineligible(entry))
    {
        uint32_t first = sub_80080ACC(r_u32(0x8009C5C8));
        w_u16(r_u32(0x8009C5C8) + 4, (uint16_t)first);
    }
    for (i = 0; i < 4; ++i)
    {
        uint32_t address = 0x800D81D8 + 36 * i + 8, value = (uint32_t)r_u8(address);
        if (r_u32(0x800D7138 + 192 * i))
        {
            if (value - 3 >= 2)
                w_u8(address, (uint8_t)3);
        }
        else if (value - 3 < 2)
            w_u8(address, (uint8_t)0);
    }
    root = r_u32(0x8009C534);
    if (root && r_u32(root + 4) && unavailable(root))
    {
        w_u8((root + 3), (uint8_t)(sub_80062D6C(r_u32(0x8009C534))));
        (void)sub_800630C0(root, ((scalar_v_29 = ((uint32_t)r_u8(0x8009C5E6))), (((uint32_t)(scalar_v_29 < 128 ? scalar_v_29 : scalar_v_29 | 0xffffff00)))));
    }
    root = r_u32(0x8009C538);
    if (root && r_u32(root + 4) && unavailable(root))
    {
        w_u8((root + 3), (uint8_t)(sub_80062D6C(r_u32(0x8009C538))));
        index = ((scalar_v_30 = ((uint32_t)r_u8(0x8009C5E6))), (((uint32_t)(scalar_v_30 < 128 ? scalar_v_30 : scalar_v_30 | 0xffffff00))));
        (void)sub_800630C0(root, ((scalar_v_31 = ((uint32_t)r_u8((0x800D81D8 + 36 * index + 8)))), (((uint32_t)(scalar_v_31 < 128 ? scalar_v_31 : scalar_v_31 | 0xffffff00)))));
    }
}
