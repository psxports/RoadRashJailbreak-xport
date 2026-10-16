#include "video_stop.h"
#include "video_preview.h"
#include "psx.h"
#include "menu_video.h"
#include "entry_kind.h"
#include "resource_record.h"

static uint32_t selected(uint32_t menu)
{
    uint32_t index = (uint32_t)r_u16((menu + 4));
    if (index >= 32768)
        index |= 0xffff0000;
    return sub_80072100(r_u32(menu + 16) + 120 * index);
}

uint32_t sub_8006E6F4(uint32_t menu, uint32_t descriptor)
{
    FUNCTION_MARKER(0x8006E6F4u, "RASHCDF.BIN");
    if (!descriptor)
        return 1;
    if (!((uint32_t)r_u16(menu) & 2) || ((int32_t)(uint32_t)(r_u32(0x8005ACAC))) < 151)
        return sub_8006FE6C();
    if ((uint32_t)r_u16(0x8009C5DA) & 1)
        (void)sub_80070018(descriptor);
    else if ((uint32_t)r_u16((menu + 6)) == (uint32_t)r_u16(0x8009C5D0))
        (void)sub_8006FEF4(descriptor);
    return 1;
}

uint32_t sub_8006E4D8(uint32_t menu, uint32_t entry)
{
    FUNCTION_MARKER(0x8006E4D8u, "RASHCDF.BIN");
    uint32_t local = entry + 16, desc = 0, kind, type = (uint32_t)r_u16((entry + 8)), fallback = 0;
    switch (type)
    {
        case 2:
            desc = local;
            break;
        case 3:
            desc = local;
            w_u32(desc, selected(menu) == 3 ? 20 : 11);
            break;
        case 4:
        case 6:
            desc = sub_800641D4();
            if (desc)
            {
                w_u16(desc + 12, (uint16_t)r_u16((local + 12)));
                w_u16(desc + 14, (uint16_t)r_u16((local + 14)));
            }
            else if (type == 6)
            {
                kind = selected(menu);
                fallback = kind != 6 && kind != 4 && kind != 5 && kind != 7;
            }
            break;
        case 5:
            fallback = 1;
            break;
        default:
            break;
    }
    if (fallback)
    {
        desc = local;
        if (selected(menu) == 2)
            w_u32(desc, 19);
        else
            switch (r_u32(0x800D80D8))
            {
                case 1:
                case 17:
                    w_u32(desc, 13);
                    break;
                case 4:
                    w_u32(desc, 14);
                    break;
                case 8:
                case 24:
                    w_u32(desc, 17);
                    break;
                case 16:
                    w_u32(desc, 15);
                    break;
                case 32:
                    w_u32(desc, 12);
                    break;
                default:
                    desc = 0;
                    break;
            }
    }
    return desc ? sub_8006E6F4(menu, desc) : 1;
}
