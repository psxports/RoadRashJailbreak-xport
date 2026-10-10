#include "psx.h"
#include "wip.h"
#include "menu_center.h"
#include "entry_kind.h"
#include "resource_record.h"
#include <stdlib.h>

static uint32_t h(RRJMemory *m, uint32_t a)
{
    return rrj_u16(rrj_at(a, 2));
}

static uint32_t b(RRJMemory *m, uint32_t a)
{
    return r_u8(a);
}

static uint32_t sxh(uint32_t v)
{
    return v < 32768 ? v : v | 0xffff0000;
}

static uint32_t sxb(uint32_t v)
{
    return v < 128 ? v : v | 0xffffff00;
}

static uint32_t selected(RRJMemory *m, uint32_t menu)
{
    return rrj_read32(menu + 16) + 120 * sxh(h(m, menu + 4));
}

static void position(RRJMemory *m, uint32_t data, uint8_t image[12])
{
    rrj_put16(image + 8, h(m, data));
    rrj_put32(image + 4, 0x808080);
    rrj_put16(image + 10, h(m, data + 2));
}

uint32_t sub_F_8006E8FC(uint32_t menu, uint32_t entry, RRJMenuResource resource, RRJImageSelect select, RRJImageSpecial special)
{
    FUNCTION_MARKER(0x8006E8FCu, "RASHCDF.BIN");
    uint32_t data = entry + 16, id = 0, source = 0, kind, link, mode;
    uint8_t local[12];
    if ((h(rrj_host_context(), entry + 10) & 32) && !(h(rrj_host_context(), menu) & 2))
        return 1;
    switch (h(rrj_host_context(), data + 4))
    {
        case 48:
        {
            uint32_t choice = selected(rrj_host_context(), menu), saved1 = rrj_read32(0x8009C664), saved2 = rrj_read32(0x8009C678), saved3 = b(rrj_host_context(), 0x8009C5E7);
            if (!select)
                abort();
            select(rrj_host_context(), menu, choice);
            source = sub_F_80064154(h(rrj_host_context(), data + 4));
            if (source)
            {
                rrj_put16(rrj_at(source + 8, 2), h(rrj_host_context(), data));
                rrj_put16(rrj_at(source + 10, 2), h(rrj_host_context(), data + 2));
            }
            rrj_write32(0x8009C664, saved1);
            rrj_write32(0x8009C678, saved2);
            w_u8(0x8009C5E7, (uint8_t)saved3);
            if (!source)
                return 1;
            break;
        }
        case 49:
            if ((h(rrj_host_context(), data + 6) & 0x1000) && !(h(rrj_host_context(), selected(rrj_host_context(), menu) + 10) & 0x100))
                return 1;
            position(rrj_host_context(), data, local);
            kind = sub_F_80072100(selected(rrj_host_context(), menu));
            if (kind == 2)
                id = 0x474C5954;
            else
            {
                mode = rrj_read32(0x800D80D8);
                switch (mode)
                {
                    case 1:
                    case 17:
                        id = 0x474C4F35;
                        break;
                    case 4:
                        id = 0x474C5454;
                        break;
                    case 8:
                    case 24:
                        id = 0x474C4353;
                        break;
                    case 16:
                        id = 0x474C4848;
                        break;
                    case 32:
                        switch (b(rrj_host_context(), 0x800D80DC))
                        {
                            case 1:
                                id = 0x504C5447;
                                break;
                            case 3:
                                id = 0x504C5444;
                                break;
                            case 5:
                                id = 0x504C424A;
                                break;
                            default:
                                id = 0x474C424A;
                                break;
                        }
                        break;
                    default:
                        return 1;
                }
            }
            break;
        case 50:
            if (!special)
            {
                RRJ_WIP(rrj_host_context(), 0, "unknown", "menu_center", "skip_image_return_one", ((const uint32_t[]){menu, entry, data}), 3);
                return 1;
            }
            return special(rrj_host_context(), menu, entry, data);
        case 51:
            kind = sub_F_80072100(selected(rrj_host_context(), menu));
            id = kind == 3 ? 0x474C4E43 : 0x474C5252;
            position(rrj_host_context(), data, local);
            break;
        case 52:
        case 53:
            id = rrj_read32(h(rrj_host_context(), data + 4) == 52 ? 0x8009C67C : 0x8009C680);
            if (id == 300)
                return 1;
            position(rrj_host_context(), data, local);
            break;
        default:
            return 1;
    }
    link = rrj_read32(0x8009CFC8) + 4 * sxb(b(rrj_host_context(), 0x8009C5E1)) + 24;
    if (source)
        return sub_F_800700F0(menu, source, link, resource);
    rrj_put32(local, id);
    return rrj_menu_image_local(rrj_host_context(), menu, local, link, resource);
}
