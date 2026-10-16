#include "psx.h"
/* Original F MIPS: resource selection and native sprite-packet producer. */
#include "menu_sprite.h"
#include "packet.h"
#include "game_main.h"
#include "resource.h"
#include <stdlib.h>

uint32_t sub_800700F0(uint32_t menu, const uint8_t image[12], uint32_t link)
{
    FUNCTION_MARKER(0x800700F0, "RASHCDF.BIN");

    uint32_t id = ((uint32_t)(xport_load_le32(image + 0))), index, descriptor, flags, result = 1, cache = 0, packet, context, width;
    int wide;
    for (index = 0; index < 300; ++index)
        if (r_u32(0x80088E0C + index * 4) == id)
            break;
    if (index == 300)
        return 1;
    descriptor = 0x8009DDE0 + index * 36;
    width = (uint32_t)r_u16((descriptor + 20));
    wide = width >= 257 && width < 32768;
    flags = r_u32(descriptor);
    if (flags & 0x00100000)
    {
        if (r_u32(0x800A0810) != id)
        {
            if (!(flags & 0x10000000))
                result = 0;
            else
                cache = 0x800A0810;
        }
    }
    else if (flags & 0x00200000)
    {
        link = r_u32(0x8009CCE4) + (uint32_t)r_s8(menu + 12) * 4 + 28;
        if (r_u32(0x800A0818) != id)
            result = sub_80078F44(descriptor, id);
    }
    else if (flags & 0x00400000)
    {
        if ((uint32_t)r_u16(0x8009C5DA) & 1)
            link = r_u32(0x8009CCE4) + (uint32_t)r_s8(menu + 12) * 4 + 20;
        if (r_u32(0x800A0814) != id)
            cache = 0x800A0814;
    }
    else if (flags & 0x00800000)
    {
        link = r_u32(0x8009CCE4) + (uint32_t)r_s8(menu + 12) * 4 + 24;
        if (r_u32(0x800A082C) != id)
            cache = 0x800A082C;
    }
    else if (flags & 0x10000000)
        result = sub_8007A400(descriptor, id);
    else
        result = 0;
    if (cache)
    {
        w_u32(descriptor, flags & 0xdfffffff);
        result = sub_8007A400(descriptor, ((uint32_t)(xport_load_le32(image + 0))));
        if (!result)
            return 0;
        w_u32(cache, ((uint32_t)(xport_load_le32(image + 0))));
    }
    if (!result)
        return 0;
    context = r_u32(0x8005B470);
    packet = r_u32(context + 268);
    if (packet + 48 >= r_u32(0x8005B4D0))
    {
        uint32_t replacement = sub_80021C98(packet, 48);
        w_u32(r_u32(0x8005B470) + 268, replacement);
    }
    context = r_u32(0x8005B470);
    packet = r_u32(context + 268);
    w_u32(context + 268, packet + 48); /* Reserve 48 even for one 24-byte packet. */
    w_u32(packet, r_u32(link) | 0x05000000);
    w_u32(packet + 4, ((uint32_t)r_u16((descriptor + 28)) & 0x9ff) | 0xe1000200);
    w_u16((packet + 18), ((uint32_t)r_u16((descriptor + 30))));
    w_u32(packet + 8, ((uint32_t)(xport_load_le32(image + 4))) | 0x64000000);
    w_u16((packet + 12), (((uint32_t)(xport_load_le16(image + 8)))));
    w_u16((packet + 14), (((uint32_t)(xport_load_le16(image + 10)))));
    w_u8((packet + 16), (uint8_t)r_u8((descriptor + 24)));
    w_u8((packet + 17), (uint8_t)r_u8((descriptor + 26)));
    w_u16((packet + 20), (wide ? 256 : (uint32_t)r_u16((descriptor + 20))));
    w_u16((packet + 22), ((uint32_t)r_u16((descriptor + 22))));
    w_u32(link, packet);
    if (wide)
    {
        uint32_t format, x, y;
        packet += 24;
        w_u32(packet, r_u32(link) | 0x05000000);
        w_u16((packet + 18), ((uint32_t)r_u16((descriptor + 30))));
        w_u32(packet + 8, ((uint32_t)(xport_load_le32(image + 4))) | 0x64000000);
        w_u16((packet + 12), (((uint32_t)(xport_load_le16(image + 8))) + 256));
        w_u16((packet + 14), (((uint32_t)(xport_load_le16(image + 10)))));
        w_u8((packet + 16), (uint8_t)r_u8((descriptor + 24)));
        w_u8((packet + 17), (uint8_t)r_u8((descriptor + 26)));
        w_u16((packet + 20), ((uint32_t)r_u16((descriptor + 20)) - 256));
        w_u16((packet + 22), ((uint32_t)r_u16((descriptor + 22))));
        format = r_u32(descriptor) & 3;
        if (format != 3)
        {
            y = (uint32_t)r_u16((descriptor + 18));
            x = (uint32_t)r_u16((descriptor + 16)) + (64 << format);
            w_u32(packet + 4, 0xe1000200 | (format << 7) | ((y & 256) >> 4) | ((x & 1023) >> 6) | ((y & 512) * 4));
        } /* Format 3 leaves the second page word untouched, as in MIPS. */
        w_u32(link, packet);
    }
    return result;
}

uint32_t sub_8006E894(uint32_t menu, uint32_t entry)
{
    uint32_t scalar_v_26;

    FUNCTION_MARKER(0x8006E894u, "RASHCDF.BIN");
    uint32_t link;
    if (((uint32_t)r_u16((entry + 10)) & 32) && !((uint32_t)r_u16(menu) & 2))
        return 1;
    link = r_u32(0x8009CFC8) + ((scalar_v_26 = ((uint32_t)r_u8(0x8009C5E1))), (((uint32_t)(scalar_v_26 < 128 ? scalar_v_26 : scalar_v_26 | 0xffffff00)))) * 4 + 28;
    return sub_800700F0(menu, xport_guest_cptr(entry + 16, 12), link);
}
