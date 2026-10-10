#include "psx.h"
#include "wip.h"
/* Information string descriptor selection, checked against F MIPS. */
#include "menu_info.h"
#include "entry_kind.h"
#include "resource_record.h"
#include <stdlib.h>

static uint32_t b(RRJMemory *m, uint32_t a)
{
    return r_u8(a);
}

static uint32_t h(RRJMemory *m, uint32_t a)
{
    return rrj_u16(rrj_at(a, 2));
}

static uint32_t sx(uint32_t v)
{
    return v < 128 ? v : v | 0xffffff00;
}

static uint32_t result(RRJMemory *m, uint32_t a, uint32_t value)
{
    rrj_put16(rrj_at(a, 2), value);
    return a;
}

uint32_t sub_F_80063C7C(uint32_t selector, RRJModeLabel mode_label)
{
    FUNCTION_MARKER(0x80063C7Cu, "RASHCDF.BIN");
    uint32_t entry, value, mode;
    switch (selector & 65535)
    {
        case 14:
            if (h(rrj_host_context(), 0x8009C5D8) != h(rrj_host_context(), 0x8009C5D0))
                return 0;
            entry = rrj_read32(0x8009C654);
            if (!entry || h(rrj_host_context(), entry + 8) != 12)
                return 0;
            value = 0;
            if (sub_F_80072100(entry) == 1)
            {
                mode = b(rrj_host_context(), 0x800D80DC);
                if (mode == 1)
                    value = 1545;
                else if (mode == 3)
                    value = 1546;
                else if (mode == 5)
                    value = 1547;
            }
            if (!value)
                value = h(rrj_host_context(), rrj_read32(0x8009C654) + 20) + 1;
            (void)result(rrj_host_context(), 0x8009C4A8, value);
            w_u8(0x8009C4AA, 3);
            rrj_write32(0x8009C4AC, 0xA08C8C);
            return 0x8009C4A8;
        case 32:
            value = b(rrj_host_context(), 0x800D80EF);
            if (value != 0 && value != 2)
                return 0;
            return result(rrj_host_context(), 0x80081120, value == 2 ? 201 : 200);
        case 33:
            return result(rrj_host_context(), 0x80081120, b(rrj_host_context(), 0x800D80F1) ? 6 : 7);
        case 34:
            if (!mode_label)
            {
                RRJ_WIP(rrj_host_context(), 0, "unknown", "mode_label", "return_null_descriptor", &selector, 1);
                return 0;
            }
            value = mode_label(rrj_host_context(), sx(b(rrj_host_context(), 0x800D80DC)), rrj_read32(0x800D80D8), 0);
            return result(rrj_host_context(), 0x800810F8, value);
        case 35:
            return result(rrj_host_context(), 0x80081100, b(rrj_host_context(), 0x800D80E1) ? 6 : 7);
        case 36:
            return result(rrj_host_context(), 0x80081108, b(rrj_host_context(), 0x800D80E2) ? 6 : 7);
        case 37:
            return result(rrj_host_context(), 0x80081110, b(rrj_host_context(), 0x800D80E3) ? 6 : 7);
        case 38:
            value = b(rrj_host_context(), 0x800D80EA);
            return result(rrj_host_context(), 0x80081118, value >= 2 && value <= 6 ? value + 7 : 8);
        case 39:
            return result(rrj_host_context(), 0x80081120, b(rrj_host_context(), 0x800D8198 + b(rrj_host_context(), 0x8009C5DE)) ? 6 : 7);
        case 41:
        case 42:
            value = b(rrj_host_context(), (selector & 65535) == 41 ? 0x800D81ED : 0x800D8211);
            return result(rrj_host_context(), 0x80081120, value == 1 ? 196 : value == 2 ? 197 : 198);
        case 43:
            return result(rrj_host_context(), 0x80081108, b(rrj_host_context(), 0x800D80EC) ? 184 : 183);
        case 44:
            return result(rrj_host_context(), 0x80081120, b(rrj_host_context(), 0x800D81EF + 36 * sx(b(rrj_host_context(), 0x8009C5E6))) ? 6 : 7);
        case 56:
            return result(rrj_host_context(), 0x80081108, b(rrj_host_context(), 0x8009C5E9) ? 4 : 5);
        default:
            return sub_F_80064154(selector);
    }
}
