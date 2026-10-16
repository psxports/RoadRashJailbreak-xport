#include "psx.h"
#include "wip.h"
#include "menu_center.h"
#include "entry_kind.h"
#include "resource_record.h"
#include <stdlib.h>
#include "resource_select.h"
#include "game_main.h"

static uint32_t selected(uint32_t menu)
{
    uint32_t scalar_v_4;

    return r_u32(menu + 16) + 120 * ((scalar_v_4 = ((uint32_t)r_u16((menu + 4)))), (((uint32_t)(scalar_v_4 < 32768 ? scalar_v_4 : scalar_v_4 | 0xffff0000))));
}

static void position(uint32_t data, uint8_t image[12])
{
    xport_store_le16(image + 8, (uint32_t)r_u16(data));
    xport_store_le32(image + 4, 0x808080);
    xport_store_le16(image + 10, (uint32_t)r_u16((data + 2)));
}

uint32_t sub_8006E8FC(uint32_t menu, uint32_t entry)
{
    uint32_t scalar_v_5;

    FUNCTION_MARKER(0x8006E8FCu, "RASHCDF.BIN");
    uint32_t data = entry + 16, id = 0, source = 0, kind, link, mode;
    uint8_t local[12];
    if (((uint32_t)r_u16((entry + 10)) & 32) && !((uint32_t)r_u16(menu) & 2))
        return 1;
    switch ((uint32_t)r_u16((data + 4)))
    {
        case 48:
        {
            uint32_t choice = selected(menu), saved1 = r_u32(0x8009C664), saved2 = r_u32(0x8009C678), saved3 = (uint32_t)r_u8(0x8009C5E7);
            (void)sub_800680E8(menu, choice);
            source = sub_80064154((uint32_t)r_u16((data + 4)));
            if (source)
            {
                w_u16(source + 8, (uint32_t)r_u16(data));
                w_u16(source + 10, (uint32_t)r_u16((data + 2)));
            }
            w_u32(0x8009C664, saved1);
            w_u32(0x8009C678, saved2);
            w_u8(0x8009C5E7, (uint8_t)saved3);
            if (!source)
                return 1;
            break;
        }
        case 49:
            if (((uint32_t)r_u16((data + 6)) & 0x1000) && !((uint32_t)r_u16((selected(menu) + 10)) & 0x100))
                return 1;
            position(data, local);
            kind = sub_80072100(selected(menu));
            if (kind == 2)
                id = 0x474C5954;
            else
            {
                mode = r_u32(0x800D80D8);
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
                        switch ((uint32_t)r_u8(0x800D80DC))
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
            return sub_80075BD0(menu, entry, data);
        case 51:
            kind = sub_80072100(selected(menu));
            id = kind == 3 ? 0x474C4E43 : 0x474C5252;
            position(data, local);
            break;
        case 52:
        case 53:
            id = r_u32((uint32_t)r_u16((data + 4)) == 52 ? 0x8009C67C : 0x8009C680);
            if (id == 300)
                return 1;
            position(data, local);
            break;
        default:
            return 1;
    }
    link = r_u32(0x8009CFC8) + 4 * ((scalar_v_5 = ((uint32_t)r_u8(0x8009C5E1))), (((uint32_t)(scalar_v_5 < 128 ? scalar_v_5 : scalar_v_5 | 0xffffff00)))) + 24;
    if (source)
        return sub_800700F0(menu, xport_guest_cptr(source, 12), link);
    xport_store_le32(local, id);
    return sub_800700F0(menu, local, link);
}
