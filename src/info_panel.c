#include "psx.h"
#include "info_panel.h"
#include "game_main.h"
#include "native_game_api.h"
#include "menu_hint.h"

uint32_t sub_8006FAC8(uint32_t menu, uint32_t entry)
{
    uint32_t font, rect = entry + 16, id, color, mode, link, desc, selector;
    FUNCTION_MARKER(0x8006FAC8, "RASHCDF.BIN");
    if ((r_u16(entry + 10) & 32) && !(r_u16(menu) & 2))
        return 1;
    if (!r_s8(0x800D8078 + 24 * r_u32(0x8009C5C0)))
        return 1;
    if (!r_s8(0x800D8078 + 24 * r_u32(0x8009C5BC)))
        return 1;
    selector = r_u16(entry + 24);
    if (r_u16(entry + 10) & 8192)
    {
        switch (selector)
        {
            case 1: (void)sub_80072AF4(menu, entry); return 1;
            case 2: (void)sub_80072C44(menu, entry); return 1;
            case 3: (void)sub_80072D94(menu, entry); return 1;
            case 4: (void)sub_80072F2C(menu, entry); return 1;
            case 10: (void)sub_8007593C(menu, entry); return 1;
            case 15: (void)sub_8007219C(menu, entry); return 1;
            case 16: (void)sub_80072248(menu, entry); return 1;
            case 17: (void)sub_80072388(menu, entry); return 1;
            case 18: (void)sub_800724D0(menu, entry); return 1;
            case 19: (void)sub_80072668(menu, entry); return 1;
            case 20: (void)sub_800727C8(menu, entry); return 1;
            case 21: (void)sub_80072910(menu, entry); return 1;
            case 27: (void)sub_800733BC(menu, entry); return 1;
            case 28: (void)sub_80073BAC(menu, entry); return 1;
            case 29: (void)sub_80074194(menu, entry); return 1;
            case 30: (void)sub_80074554(menu, entry); return 1;
            case 40: (void)sub_80075D0C(menu, entry); return 1;
            case 55: (void)sub_80075098(menu, entry); return 1;
            case 56: (void)sub_80075AFC(menu, entry); return 1;
            case 5:
            case 6:
            case 7: (void)sub_80073074(menu, entry, selector); return 1;
            case 8:
            case 9: (void)sub_80073234(menu, entry, selector); return 1;
            case 31: (void)sub_800758D4(menu, entry); return 1;
            case 45: (void)sub_80071064(menu, rect, r_s16(r_u32(0x8009954C) + 8)); return 1;
            case 46: (void)sub_80071AF0(rect, r_s16(r_u32(0x8009954C) + 8)); return 1;
            case 47: (void)sub_80070DD4(menu, rect); return 1;
            case 54: (void)sub_80074C9C(); return 1;
            case 26: break;
            default: return 1;
        }
        if (!r_s16(0x8009C5EC))
            return 1;
        font = r_u32(0x8009C5B8);
        w_u8(0x800D8078 + 24 * font + 3, 0);
        font = r_u32(0x8009C5B8);
        color = r_u32(rect + 12);
        id = (uint32_t)r_s16(0x8009C5EC);
        mode = r_u16(rect + 10);
    }
    else
    {
        desc = sub_80063C7C(selector);
        if (!desc)
            return 1;
        font = r_u32(0x8009C5B8);
        w_u8(0x800D8078 + 24 * font + 3, 0);
        font = r_u32(0x8009C5B8);
        id = (uint32_t)r_s16(desc);
        color = r_u32(desc + 4);
        mode = (uint32_t)r_s8(desc + 2);
    }
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    (void)sub_8002CB08(font, id, xport_guest_cptr(rect, 8), link, color, mode);
    return 1;
}
