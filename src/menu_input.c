#include "music_start.h"
#include "psx.h"
/* IDA drafts reconciled with F overlay MIPS; controller is still WIP
 * until its frame caller and music replay dependency are integrated. */
#include "menu_input.h"
#include "menu_navigation.h"
#include "audio.h"
#include <stdlib.h>

uint32_t sub_8006C700(uint32_t menu, uint32_t subtype)
{
    FUNCTION_MARKER(0x8006C700u, "RASHCDF.BIN");
    int32_t count = (int32_t)r_s16((menu + 8)), wanted = ((int32_t)((int32_t)(int32_t)(int16_t)((uint32_t)(subtype)))), i;
    uint32_t entry;
    if (count <= 0)
        return 0xffffffff;
    entry = r_u32(menu + 16);
    for (i = 0; i < count; ++i, entry += 120)
    {
        int32_t type = (int32_t)r_s16((entry + 8));
        if (type >= 12 && type < 14 && (int32_t)(uint32_t)r_u16((entry + 18)) == wanted)
            return (uint32_t)i;
    }
    return 0xffffffff;
}

/* The two back-button blocks have identical data/control effects. */
static uint32_t back(uint32_t menu)
{
    uint32_t current = (uint32_t)r_u16(0x8009C5D0);
    uint32_t entry = 0x8009C7B8 + (uint32_t)((int32_t)((int32_t)(int32_t)(int16_t)((uint32_t)(current)))) * 4;
    if (r_u32(entry) == 0xffffffff)
    {
        sub_8007EAC0(4);
        return 2;
    }
    if (((uint32_t)r_u16(menu) & 0x400) && (uint32_t)r_u8(0x800D80F0))
    {
        uint32_t target = (uint32_t)r_u8((r_u32(0x8005B2F8) + 4)) == 4 ? 55 : 54;
        uint32_t table;
        w_u16(0x8009C5D2, target);
        table = r_u32(0x8009C68C);
        w_u8(0x8009C5E8, (uint8_t)r_u8(entry));
        w_u16((r_u32(table + target * 4) + 10), current);
        w_u8((r_u32(table + target * 4) + 14), (uint8_t)r_u8((menu + 14)));
        w_u8((r_u32(r_u32(0x8009C68C) + target * 4) + 15), (uint8_t)r_u8((menu + 15)));
        w_u8(0x8009C5E3, (uint8_t)((uint32_t)r_u8(0x8009C5E3) | 4));
        sub_8007EAC0(3);
        return 1;
    }
    entry = 0x8009C7B8 + (uint32_t)(int32_t)r_s16(0x8009C5D0) * 4;
    w_u16(0x8009C5D2, ((uint32_t)r_u16(entry)));
    sub_8007EAC0(3);
    w_u8(0x8009C5E3, (uint8_t)((uint32_t)r_u8(0x8009C5E3) | 4));
    return 1;
}

uint32_t sub_8006B03C(uint32_t menu, uint32_t first, uint32_t end)
{
    FUNCTION_MARKER(0x8006B03Cu, "RASHCDF.BIN");
    uint32_t player;
    if (!r_u32(menu + 16))
        return 0;
    /* Back from either physical pad has priority over the configured pad range. */
    for (player = 0; player < 2; ++player)
        if ((int32_t)r_s8((0x800D7128 + 192 * player + 74)) > 0)
            return back(menu);
    for (player = first; ((int32_t)(uint32_t)(player)) < ((int32_t)(uint32_t)(end)); ++player)
    {
        uint32_t pad = 0x800D7128 + 192 * player, slider, table, entry, value;
        int32_t type;
        if ((uint32_t)r_u8((pad + 42)))
        {
            w_u8(0x8009C5E3, (uint8_t)((uint32_t)r_u8(0x8009C5E3) | 16));
            sub_8006C558(menu);
            return 2;
        }
        if ((uint32_t)r_u8((pad + 50)))
        {
            w_u8(0x8009C5E3, (uint8_t)((uint32_t)r_u8(0x8009C5E3) | 32));
            sub_8006C3A4(menu);
            return 2;
        }
        if ((uint32_t)r_u8((pad + 26)) || (uint32_t)r_u8((pad + 34)))
        {
            int left = (uint32_t)r_u8((pad + 26)) != 0;
            slider = r_u32(0x8009C64C);
            w_u8(0x8009C5E3, (uint8_t)((uint32_t)r_u8(0x8009C5E3) | (left ? 64 : 128)));
            if (slider)
            {
                uint32_t step = (uint32_t)(int32_t)r_s16((slider + 8));
                value = r_u32(slider + 12);
                value = left ? value - step : value + step;
                w_u32(slider + 12, value);
                slider = r_u32(0x8009C64C);
                value = r_u32(slider + 12);
                table = r_u32(slider + (left ? 0 : 4));
                if (left ? ((int32_t)(uint32_t)(value)) < ((int32_t)(uint32_t)(table)) : ((int32_t)(uint32_t)(table)) < ((int32_t)(uint32_t)(value)))
                    w_u32(slider + 12, table);
                table = r_u32(0x8009C650);
                if (table)
                    w_u32(table, r_u32(r_u32(0x8009C64C) + 12));
            }
            return 2;
        }
        if ((int32_t)r_s8((pad + 82)) > 0)
        {
            w_u8(0x8009C5E3, (uint8_t)((uint32_t)r_u8(0x8009C5E3) | 1));
            table = 0x8009C6C8 + (uint32_t)(int32_t)r_s16(0x8009C5D0) * 4;
            if (r_u32(table) != 0xffffffff)
            {
                w_u16(0x8009C5D2, ((uint32_t)r_u16(table)));
                sub_8007EAC0(2);
                return 1;
            }
            entry = r_u32(menu + 16) + (uint32_t)(int32_t)r_s16((menu + 4)) * 120;
            type = (int32_t)r_s16((entry + 8));
            if (type >= 12 && type < 14)
            {
                uint32_t offset = (uint32_t)r_u16((entry + 18)) * 4;
                table = 0x8009CAF8 + offset;
                if (r_u32(table) != 0xffffffff)
                {
                    w_u16(0x8009C5D2, ((uint32_t)r_u16(table)));
                    sub_8007EAC0(2);
                    return 1;
                }
                table = 0x8009C9B0 + offset;
            }
            else if (type == 17)
                table = 0x8009C8A8 + (uint32_t)r_u16((entry + 100)) * 4;
            else
            {
                sub_8007EAC0(4);
                return 2;
            }
            if (r_u32(table) == 0xffffffff)
            {
                sub_8007EAC0(4);
                return 2;
            }
            w_u16((menu + 4), (sub_8006C700(menu, (uint32_t)(int32_t)r_s16(table))));
            sub_8007EAC0(2);
            return 2;
        }
        if ((int32_t)r_s8((pad + 74)) > 0)
            return back(menu);
        if ((int32_t)r_s8((pad + 58)) > 0)
        {
            if ((uint32_t)r_u16(menu) & 0x100)
            {
                if (!(uint32_t)r_u8(0x8009C5EE))
                    sub_8007EAC0(4);
                else if ((uint32_t)r_u8(0x8009C5DF))
                {
                    (void)sub_8007EDE0((uint32_t)r_u8(0x8009C5DE));
                    w_u8(0x8009C5E0, (uint8_t)1);
                }
                return 2;
            }
            value = (uint32_t)r_u8(0x8009C5D0);
            w_u8(0x800D80E8, (uint8_t)value);
            table = r_u32(0x8009C68C) + (uint32_t)((int32_t)((int32_t)(int32_t)(int8_t)((uint32_t)(value)))) * 4;
            value = (uint32_t)r_u8((r_u32(table) + 4));
            w_u16(0x8009C5D2, 42);
            w_u8(0x800D80E5, (uint8_t)value);
            sub_8007EAC0(2);
            w_u8(0x8009C5E3, (uint8_t)((uint32_t)r_u8(0x8009C5E3) | 8));
            return 1;
        }
        if ((int32_t)r_s8((pad + 66)) > 0)
        {
            if ((uint32_t)r_u16(menu) & 0x200)
            {
                sub_8007EAC0(4);
                return 2;
            }
            sub_8007EAC0(2);
            w_u16(0x8009C5D2, 53);
            table = r_u32(0x8009C68C);
            w_u8(0x8009C5E3, (uint8_t)((uint32_t)r_u8(0x8009C5E3) | 2));
            w_u16((r_u32(table + 212) + 10), ((uint32_t)r_u16((menu + 6))));
            w_u8((r_u32(table + 212) + 14), (uint8_t)r_u8((menu + 14)));
            w_u8((r_u32(r_u32(0x8009C68C) + 212) + 15), (uint8_t)r_u8((menu + 15)));
            return 2; /* Original deliberately returns handled, not transition. */
        }
    }
    return 0;
}

uint32_t sub_80069418(uint32_t menu)
{
    FUNCTION_MARKER(0x80069418u, "RASHCDF.BIN");
    int32_t first = (int32_t)r_s8((menu + 14)), end = first + (int32_t)r_s8((menu + 15));
    return sub_8006B03C(menu, (uint32_t)first, (uint32_t)end);
}
