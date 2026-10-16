#include "native_game_api.h"
#include "resource_value.h"
#include "menu_transition.h"
#include "draft_quick.h"
#include "psx.h"
/* F6883C: player/mode reset, reconciled with full MIPS switch. */
#include "menu_reset.h"
#include "menu_flags.h"
#include <stdlib.h>

static void set_resource_value(uint32_t slot, uint32_t value)
{
    (void)sub_800630C0(r_u32(slot), value);
}

static uint32_t select_item(uint32_t id)
{
    uint32_t menu = r_u32(r_u32(0x8009C68C) + 4 * id), value = sub_80080A70(menu);
    menu = r_u32(r_u32(0x8009C68C) + 4 * id);
    w_u16((menu + 4), (uint16_t)value);
    return value;
}

uint32_t sub_8006883C(uint32_t id)
{
    FUNCTION_MARKER(0x8006883Cu, "RASHCDF.BIN");
    uint32_t i, p, v, result;
    (void)sub_8002D250();
    w_u8(0x800D80E9, (uint8_t)0);
    w_u8(0x800D81DE, (uint8_t)127);
    w_u8(0x800D8202, (uint8_t)127);
    w_u8(0x800D80DD, (uint8_t)48);
    w_u8(0x800D80ED, (uint8_t)0);
    w_u8(0x800D80EE, (uint8_t)0);
    w_u16(0x8009C5D4, (uint16_t)40);
    w_u8(0x800D80F5, (uint8_t)0);
    for (i = 0, p = 0x800D81D8; i < 6; ++i, p += 36)
    {
        w_u16((p + 12), (uint16_t)0);
        w_u8((p + 14), (uint8_t)0);
        w_u8((p + 15), (uint8_t)0);
        w_u32(p + 16, 0);
        w_u8((p + 20), (uint8_t)0);
        w_u8((p + 22), (uint8_t)0);
    }
    (void)sub_8001E100(0x800D8118, 0, 128);
    (void)sub_8001E100(0x800D80F8, 0, 16);
    (void)sub_8001E100(0x800D8108, 0, 16);
    switch (id)
    {
        case 7:
            w_u32(0x800D80D8, 32);
            w_u8(0x800D80DE, (uint8_t)1);
            w_u8(0x800D81E1, (uint8_t)0);
            set_resource_value(0x8009C4B0, 0);
            w_u8(0x800D81E2, (uint8_t)0);
            set_resource_value(0x8009C4B4, 0);
            set_resource_value(0x8009C4B8, 0);
            (void)select_item(id);
            /* original falls through into mode8 setup */
        case 8:
            w_u32(0x800D80D8, 32);
            w_u8(0x800D80F3, (uint8_t)1);
            w_u8(0x800D80DE, (uint8_t)1);
            w_u8(0x800D80F0, (uint8_t)0);
            w_u8(0x800D80DC, (uint8_t)0);
            v = (uint32_t)r_s8(0x800D81E1);
            w_u8(0x800D80E0, (uint8_t)4);
            if (v == 1)
            {
                w_u8(0x800D81DF, (uint8_t)9);
                if ((uint32_t)r_s8(0x800D81E2) == 1)
                    w_u8(0x800D80F5, (uint8_t)2);
            }
            else
                w_u8(0x800D81DF, (uint8_t)0);
            set_resource_value(0x8009C4BC, (uint32_t)r_s8(0x800D80E0));
            set_resource_value(0x8009C4C0, 0);
            set_resource_value(0x8009C4B8, 9);
            break;
        case 24:
            w_u32(0x800D80D8, 4);
            w_u8(0x8009C5F2, (uint8_t)0);
            w_u8(0x800D80EE, (uint8_t)1);
            w_u16(0x8009C5D4, (uint16_t)24);
            /* fall through */
        case 38:
            w_u32(0x800D80D8, 4);
            w_u8(0x8009C5F2, (uint8_t)0);
            w_u8(0x800D80DE, (uint8_t)1);
            w_u8(0x800D80E0, (uint8_t)56);
            w_u8(0x800D81DF, (uint8_t)0);
            w_u8(0x800D80EB, (uint8_t)0);
            w_u8(0x800D80DC, (uint8_t)0);
            w_u8(0x8009C5F3, (uint8_t)0);
            w_u8(0x800D80E3, (uint8_t)1);
            w_u8(0x800D80E2, (uint8_t)1);
            w_u8(0x800D80E1, (uint8_t)1);
            set_resource_value(0x8009C4D0, 56);
            set_resource_value(0x8009C4CC, (uint32_t)r_s8(0x800D81DF));
            set_resource_value(0x8009C4E4, (uint32_t)r_s8(0x800D80E3));
            set_resource_value(0x8009C4E0, (uint32_t)r_s8(0x800D80E2));
            set_resource_value(0x8009C4DC, (uint32_t)r_s8(0x800D80E1));
            set_resource_value(0x8009C4C8, (uint32_t)r_s8(0x800D80DC));
            set_resource_value(0x8009C4D4, r_u8(0x800D80EA));
            break;
        case 27:
            w_u32(0x800D80D8, 1);
            w_u8(0x800D80F3, (uint8_t)1);
            w_u8(0x800D80DE, (uint8_t)1);
            w_u8(0x800D80F0, (uint8_t)0);
            w_u8(0x800D80DC, (uint8_t)0);
            w_u8(0x800D81E1, (uint8_t)2);
            w_u8(0x800D80E0, (uint8_t)38);
            w_u8(0x800D81DF, (uint8_t)18);
            (void)select_item(id);
            return sub_8002D298();
        case 30:
        case 32:
            w_u32(0x800D80D8, id == 30 ? 16 : 17);
            w_u8(0x800D80DE, (uint8_t)2);
            w_u8(0x800D80E0, (uint8_t)(id == 30 ? 1 : 38));
            w_u8(0x800D80F0, (uint8_t)0);
            w_u8(0x800D81DF, (uint8_t)(id == 30 ? 0 : 18));
            w_u8(0x800D8203, (uint8_t)9);
            set_resource_value(id == 30 ? 0x8009C4EC : 0x8009C4F8, id == 30 ? 1 : 38);
            set_resource_value(id == 30 ? 0x8009C4F0 : 0x8009C4FC, (uint32_t)r_s8(0x800D81DF));
            p = id == 30 ? 0x8009C4F4 : (((int32_t)(uint32_t)((uint32_t)r_s8(0x800D81DF))) < 18 ? 0x8009C500 : 0x8009C504);
            set_resource_value(p, (uint32_t)r_s8(0x800D8203));
            set_resource_value(0x8009C4C8, (uint32_t)r_s8(0x800D80DC));
            break;
        case 34:
            w_u32(0x800D80D8, 24);
            w_u8(0x800D80E0, (uint8_t)1);
            w_u8(0x800D80F0, (uint8_t)0);
            w_u8(0x800D80DE, (uint8_t)2);
            w_u8(0x800D81DF, (uint8_t)7);
            w_u8(0x800D8203, (uint8_t)7);
            set_resource_value(0x8009C4EC, 1);
            set_resource_value(0x8009C510, (uint32_t)r_s8(0x800D81DF));
            set_resource_value(0x8009C514, (uint32_t)r_s8(0x800D8203));
            set_resource_value(0x8009C4C8, (uint32_t)r_s8(0x800D80DC));
            result = select_item(id);
            w_u8(0x800D81ED, (uint8_t)2);
            w_u8(0x800D8211, (uint8_t)2);
            return result;
        case 36:
            w_u32(0x800D80D8, 8);
            w_u8(0x800D80F0, (uint8_t)0);
            w_u8(0x800D80F3, (uint8_t)1);
            w_u8(0x800D80DE, (uint8_t)1);
            w_u8(0x800D80DC, (uint8_t)0);
            w_u8(0x800D80E0, (uint8_t)4);
            w_u8(0x800D81DF, (uint8_t)7);
            w_u8(0x800D8203, (uint8_t)7);
            set_resource_value(0x8009C508, 4);
            set_resource_value(0x8009C50C, (uint32_t)r_s8(0x800D81DF));
            result = select_item(id);
            w_u8(0x800D81ED, (uint8_t)1);
            return result;
        default:
            return id - 7 < 32 ? 0x80068e68 : 0x80060000;
    }
    return select_item(id);
}
