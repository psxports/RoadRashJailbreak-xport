#include "rrj_debug.h"
#include "psx.h"
#include "menu_render.h"
#include "menu_item.h"
#include "menu_hint.h"
#include "menu_center.h"
#include "info_panel.h"
#include "resource_select.h"
#include "menu_video.h"
#include "video_preview.h"
#include "video_stop.h"
#include "game_main.h"
#include "packet.h"
#include <stdio.h>
#include <stdlib.h>

uint32_t sub_8006F9A4(uint32_t menu, uint32_t entry)
{
    FUNCTION_MARKER(0x8006F9A4, "RASHCDF.BIN");
    uint32_t status = 0x800D8078 + 24 * r_u32(0x8009C5C0);
    if (r_s8(status))
    {
        w_u8(status + 3, 0);
        uint32_t link = r_u32(0x8009CFC8) + 4 * (uint32_t)(int32_t)r_s8(0x8009C5E1) + 4;
        sub_8002CD78(r_u32(0x8009C5C0), r_u16(entry + 16),
            (uint32_t)(int32_t)r_s16(entry + 20), (uint32_t)(int32_t)r_s16(entry + 22),
            link, r_u32(entry + 24));
    }
    return 1;
}

uint32_t sub_8006E3BC(uint32_t menu, uint32_t entry)
{
    FUNCTION_MARKER(0x8006E3BC, "RASHCDF.BIN");
    if (r_u16(menu) & 2)
        return sub_8006FEF4(entry + 16);
    return sub_8006FE6C();
}

uint32_t sub_8006E400(uint32_t menu, uint32_t entry)
{
    FUNCTION_MARKER(0x8006E400, "RASHCDF.BIN");
    uint32_t descriptor = entry + 16;
    uint32_t flags = r_u16(0x8009C5DA);
    if (flags & 4)
        return sub_8006FE6C();
    if (!(flags & 1))
    {
        if (flags & 2)
            return sub_8006FEF4(descriptor);
        w_u16(0x8009C5DA, (uint16_t)(flags | 2));
        return 0;
    }
    uint32_t result = 1;
    if (!sub_8005F484((uint32_t)(int32_t)r_s16(descriptor + 12),
        (uint32_t)(int32_t)r_s16(descriptor + 14), (uint32_t)(int32_t)r_s16(0x8009C688),
        r_u32(descriptor + 4), r_u32(descriptor + 16)))
    {
        w_u16(0x8009C5DA, (uint16_t)((r_u16(0x8009C5DA) & 0xFFFE) | 2));
        result = 0;
    }
    w_u32(descriptor + 16, r_u32(descriptor + 16) + 1);
    return result;
}

uint32_t sub_8006ECA0(uint32_t menu, uint32_t entry)
{
    FUNCTION_MARKER(0x8006ECA0, "RASHCDF.BIN");
    uint32_t link = r_u32(0x8009CFC8) + 4 * (uint32_t)(int32_t)r_s8(0x8009C5E1) + 20;
    return sub_8006ECDC(menu, xport_guest_cptr(entry + 16, 34), link);
}

uint32_t sub_8006FA38(uint32_t menu, uint32_t entry)
{
    FUNCTION_MARKER(0x8006FA38, "RASHCDF.BIN");
    uint32_t status = 0x800D8078 + 24 * r_u32(0x8009C5C0);
    if (r_s8(status))
    {
        w_u8(status + 3, 0);
        uint32_t color = r_u32(entry + 28);
        uint32_t mode = r_u16(entry + 18);
        uint32_t link = r_u32(0x8009CFC8) + 4 * (uint32_t)(int32_t)r_s8(0x8009C5E1) + 4;
        sub_8002CB08(r_u32(0x8009C5C0), r_u16(entry + 16),
            xport_guest_cptr(entry + 20, 8), link, color, mode);
    }
    return 1;
}

uint32_t sub_80064BB0(uint32_t descriptor)
{
    FUNCTION_MARKER(0x80064BB0, "RASHCDF.BIN");
    return r_u16(descriptor + 84) < 6 ? 0x8009C548 + 16 * r_u8(descriptor + 86) : 0;
}

uint32_t sub_8006F4A0(uint32_t menu, uint32_t entry)
{
    FUNCTION_MARKER(0x8006F4A0, "RASHCDF.BIN");
    uint32_t descriptor = entry + 16;
    uint32_t result = 1;
    uint32_t filled = 0;
    if (r_u8(descriptor + 87) & 1)
        result = sub_8006EF30(menu, entry);
    uint32_t link = r_u32(0x8009CFC8) + 4 * (uint32_t)(int32_t)r_s8(0x8009C5E1) + 20;
    uint32_t progress = sub_80064BB0(descriptor);
    if (progress)
    {
        uint32_t delta = (uint16_t)(r_u16(progress + 12) - r_u16(progress));
        int32_t numerator = (int32_t)(delta * (uint32_t)(int32_t)r_s16(descriptor + 92));
        int32_t divisor = (int32_t)(r_u32(progress + 4) - r_u32(progress));
        if (!divisor)
            filled = numerator < 0 ? 1 : UINT32_MAX;
        else if (numerator == INT32_MIN && divisor == -1)
            filled = (uint32_t)INT32_MIN;
        else
            filled = (uint32_t)(numerator / divisor);
    }
    uint32_t context = r_u32(0x8005B470);
    uint32_t packet = r_u32(context + 268);
    if (packet + 32 >= r_u32(0x8005B4D0))
    {
        packet = sub_80021C98(packet, 32);
        w_u32(r_u32(0x8005B470) + 268, packet);
    }
    context = r_u32(0x8005B470);
    packet = r_u32(context + 268);
    w_u32(context + 268, packet + 32);
    w_u32(packet + 4, 0x4C010101);
    w_u32(packet, r_u32(link) | 0x07000000);
    uint32_t x = (uint32_t)(int32_t)r_s16(descriptor + 88);
    uint32_t y = (uint32_t)(int32_t)r_s16(descriptor + 90);
    w_u32(packet + 8, x | (y << 16));
    x = (uint32_t)(int32_t)r_s16(descriptor + 88) + (uint32_t)(int32_t)r_s16(descriptor + 92);
    y = (uint32_t)(int32_t)r_s16(descriptor + 90);
    w_u32(packet + 12, x | (y << 16));
    x = (uint32_t)(int32_t)r_s16(descriptor + 88) + (uint32_t)(int32_t)r_s16(descriptor + 92);
    y = (uint32_t)(int32_t)r_s16(descriptor + 90) + (uint32_t)(int32_t)r_s16(descriptor + 94);
    w_u32(packet + 16, x | (y << 16));
    y = (uint32_t)(int32_t)r_s16(descriptor + 90) + (uint32_t)(int32_t)r_s16(descriptor + 94);
    x = (uint32_t)(int32_t)r_s16(descriptor + 88);
    w_u32(packet + 20, x | (y << 16));
    x = (uint32_t)(int32_t)r_s16(descriptor + 88);
    y = (uint32_t)(int32_t)r_s16(descriptor + 90);
    w_u32(packet + 28, 0x55555555);
    w_u32(packet + 24, x | (y << 16));
    w_u32(link, packet);
    context = r_u32(0x8005B470);
    packet = r_u32(context + 268);
    if (packet + 32 >= r_u32(0x8005B4D0))
    {
        packet = sub_80021C98(packet, 32);
        w_u32(r_u32(0x8005B470) + 268, packet);
    }
    context = r_u32(0x8005B470);
    packet = r_u32(context + 268);
    w_u32(context + 268, packet + 32);
    w_u32(packet, r_u32(link) | 0x03000000);
    w_u32(packet + 4, r_u32(descriptor + 100) | 0x62000000);
    w_u16(packet + 8, r_u16(descriptor + 88));
    y = r_u16(descriptor + 90);
    w_u16(packet + 12, (uint16_t)filled);
    w_u16(packet + 10, (uint16_t)y);
    w_u16(packet + 14, r_u16(descriptor + 94));
    w_u32(link, packet);
    w_u32(packet + 16, packet | 0x03000000);
    w_u32(packet + 20, r_u32(descriptor + 96) | 0x62000000);
    w_u16(packet + 24, (uint16_t)(r_u16(descriptor + 88) + filled));
    w_u16(packet + 26, r_u16(descriptor + 90));
    w_u16(packet + 28, (uint16_t)(r_u16(descriptor + 92) - filled));
    w_u16(packet + 30, r_u16(descriptor + 94));
    w_u32(link, packet + 16);
    return result;
}

RRJMenuEntryFunction rrj_menu_entry_function(uint32_t address)
{
    switch (address)
    {
        case 0x8006E3BC:
            return sub_8006E3BC;
        case 0x8006E400:
            return sub_8006E400;
        case 0x8006ECA0:
            return sub_8006ECA0;
        case 0x8006FA38:
            return sub_8006FA38;
        case 0x8006F4A0:
            return sub_8006F4A0;
        case 0x8006EDE4:
            return sub_8006EDE4;
        case 0x8006F9A4:
            return sub_8006F9A4;
        case 0x80070580u:
            return sub_80070580;
        case 0x8006F0B8:
            return sub_8006F0B8;
        case 0x8006EF30u:
            return sub_8006EF30;
        case 0x8006E894u:
            return sub_8006E894;
        case 0x8006F764u:
            return sub_8006F764;
        case 0x8006E7A4u:
            return sub_8006E7A4;
        case 0x8006E8FCu:
            return sub_8006E8FC;
        case 0x8006FAC8u:
            return sub_8006FAC8;
        case 0x8006E4D8u:
            return sub_8006E4D8;
        default:
            RRJ_DEBUG_FPRINTF(stderr, "RRJ: unresolved menu entry function %08X\n", address);
            abort();
    }
}
