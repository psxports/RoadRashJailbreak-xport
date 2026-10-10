#include "psx.h"
#include "wip.h"
/* Information-panel dispatcher, audited against F8006FAC8 MIPS. */
#include "info_panel.h"
#include "menu_hint.h"
#include "text_id.h"
#include <stdlib.h>

static uint32_t h(RRJMemory *m, uint32_t a)
{
    return rrj_u16(rrj_at(a, 2));
}

static uint32_t b(RRJMemory *m, uint32_t a)
{
    return r_u8(a);
}

static uint32_t sh(uint32_t v)
{
    return v < 32768 ? v : v | 0xffff0000;
}

static uint32_t sb(uint32_t v)
{
    return v < 128 ? v : v | 0xffffff00;
}

uint32_t sub_F_8006FAC8(uint32_t menu, uint32_t entry, RRJModeLabel label, RRJSpecialPanel special)
{
    FUNCTION_MARKER(0x8006FAC8u, "RASHCDF.BIN");
    uint32_t font, rect = entry + 16, id, color, mode, link, desc, selector, fn = 0, a0 = menu, a1 = entry, a2 = menu;
    if ((h(rrj_host_context(), entry + 10) & 32) && !(h(rrj_host_context(), menu) & 2))
        return 1;
    if (!b(rrj_host_context(), 0x800D8078 + 24 * rrj_read32(0x8009C5C0)))
        return 1;
    if (!b(rrj_host_context(), 0x800D8078 + 24 * rrj_read32(0x8009C5BC)))
        return 1;
    selector = h(rrj_host_context(), entry + 24);
    if (h(rrj_host_context(), entry + 10) & 8192)
    {
        switch (selector)
        {
            case 1:
                fn = 0x80072AF4;
                break;
            case 2:
                fn = 0x80072C44;
                break;
            case 3:
                fn = 0x80072D94;
                break;
            case 4:
                fn = 0x80072F2C;
                break;
            case 5:
            case 6:
            case 7:
                fn = 0x80073074;
                a2 = selector;
                break;
            case 8:
            case 9:
                fn = 0x80073234;
                a2 = selector;
                break;
            case 10:
                fn = 0x8007593C;
                break;
            case 15:
                fn = 0x8007219C;
                break;
            case 16:
                fn = 0x80072248;
                break;
            case 17:
                fn = 0x80072388;
                break;
            case 18:
                fn = 0x800724D0;
                break;
            case 19:
                fn = 0x80072668;
                break;
            case 20:
                fn = 0x800727C8;
                break;
            case 21:
                fn = 0x80072910;
                break;
            case 26:
                break;
            case 27:
                fn = 0x800733BC;
                break;
            case 28:
                fn = 0x80073BAC;
                break;
            case 29:
                fn = 0x80074194;
                break;
            case 30:
                fn = 0x80074554;
                break;
            case 31:
                fn = 0x800758D4;
                break;
            case 40:
                fn = 0x80075D0C;
                break;
            case 45:
                fn = 0x80071064;
                a1 = rect;
                a2 = sh(h(rrj_host_context(), rrj_read32(0x8009954C) + 8));
                break;
            case 46:
                fn = 0x80071AF0;
                a0 = rect;
                a1 = sh(h(rrj_host_context(), rrj_read32(0x8009954C) + 8));
                break;
            case 47:
                fn = 0x80070DD4;
                a1 = rect;
                break;
            case 54:
                fn = 0x80074C9C;
                break;
            case 55:
                fn = 0x80075098;
                break;
            case 56:
                fn = 0x80075AFC;
                break;
            default:
                return 1;
        }
        if (fn)
        {
            if (!special)
            {
                RRJ_WIP(rrj_host_context(), fn, "callee", "info_panel", "skip_panel_return_one", ((const uint32_t[]){a0, a1, a2}), 3);
                return 1;
            }
            special(rrj_host_context(), fn, a0, a1, a2);
            return 1;
        }
        if (!h(rrj_host_context(), 0x8009C5EC))
            return 1;
        font = rrj_read32(0x8009C5B8);
        w_u8(0x800D8078 + 24 * font + 3, 0);
        font = rrj_read32(0x8009C5B8);
        color = rrj_read32(rect + 12);
        id = sh(h(rrj_host_context(), 0x8009C5EC));
        mode = h(rrj_host_context(), rect + 10);
    }
    else
    {
        desc = sub_F_80063C7C(selector, label);
        if (!desc)
            return 1;
        font = rrj_read32(0x8009C5B8);
        w_u8(0x800D8078 + 24 * font + 3, 0);
        font = rrj_read32(0x8009C5B8);
        id = sh(h(rrj_host_context(), desc));
        color = rrj_read32(desc + 4);
        mode = sb(b(rrj_host_context(), desc + 2));
    }
    link = rrj_read32(0x8009CFC8) + 4 * sb(b(rrj_host_context(), 0x8009C5E1)) + 4;
    (void)sub_8002CB08(font, id, rect, link, color, mode, rrj_blink_text);
    return 1;
}
