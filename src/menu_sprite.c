#include "psx.h"
/* Original F MIPS: resource selection and native sprite-packet producer. */
#include "menu_sprite.h"
#include "packet.h"
#include <stdlib.h>

static uint32_t b(RRJMemory *m, uint32_t a)
{
    return r_u8(a);
}

static void sb(RRJMemory *m, uint32_t a, uint32_t v)
{
    w_u8(a, (uint8_t)v);
}

static uint32_t h(RRJMemory *m, uint32_t a)
{
    return rrj_u16(rrj_at(a, 2));
}

static void sh(RRJMemory *m, uint32_t a, uint32_t v)
{
    rrj_put16(rrj_at(a, 2), v);
}

static uint32_t signed_byte(uint32_t v)
{
    return v < 128 ? v : v | 0xffffff00;
}

static uint32_t load(RRJMemory *m, RRJMenuResource resource, uint32_t function, uint32_t descriptor, uint32_t id)
{
    if (!resource)
        abort();
    return resource(m, function, descriptor, id);
}

/* A local C descriptor and an original RAM descriptor share the producer.
 * RAM reads remain lazy and ordered, so packet/data aliases retain MIPS effects. */
typedef struct
{
    uint32_t address;
    const uint8_t *local;
} ImageSource;

static uint32_t image_word(RRJMemory *m, ImageSource image, uint32_t offset)
{
    return image.local ? rrj_u32(image.local + offset) : rrj_read32(image.address + offset);
}

static uint32_t image_half(RRJMemory *m, ImageSource image, uint32_t offset)
{
    return image.local ? rrj_u16(image.local + offset) : h(m, image.address + offset);
}

static uint32_t draw_image(RRJMemory *m, uint32_t menu, ImageSource image, uint32_t link, RRJMenuResource resource)
{
    uint32_t id = image_word(m, image, 0), index, descriptor, flags, result = 1, cache = 0, packet, context, width;
    int wide;
    for (index = 0; index < 300; ++index)
        if (rrj_read32(0x80088E0C + index * 4) == id)
            break;
    if (index == 300)
        return 1;
    descriptor = 0x8009DDE0 + index * 36;
    width = h(m, descriptor + 20);
    wide = width >= 257 && width < 32768;
    flags = rrj_read32(descriptor);
    if (flags & 0x00100000)
    {
        if (rrj_read32(0x800A0810) != id)
        {
            if (!(flags & 0x10000000))
                result = 0;
            else
                cache = 0x800A0810;
        }
    }
    else if (flags & 0x00200000)
    {
        link = rrj_read32(0x8009CCE4) + signed_byte(b(m, menu + 12)) * 4 + 28;
        if (rrj_read32(0x800A0818) != id)
            result = load(m, resource, 0x80078F44, descriptor, id);
    }
    else if (flags & 0x00400000)
    {
        if (h(m, 0x8009C5DA) & 1)
            link = rrj_read32(0x8009CCE4) + signed_byte(b(m, menu + 12)) * 4 + 20;
        if (rrj_read32(0x800A0814) != id)
            cache = 0x800A0814;
    }
    else if (flags & 0x00800000)
    {
        link = rrj_read32(0x8009CCE4) + signed_byte(b(m, menu + 12)) * 4 + 24;
        if (rrj_read32(0x800A082C) != id)
            cache = 0x800A082C;
    }
    else if (flags & 0x10000000)
        result = load(m, resource, 0x8007A400, descriptor, id);
    else
        result = 0;
    if (cache)
    {
        rrj_write32(descriptor, flags & 0xdfffffff);
        result = load(m, resource, 0x8007A400, descriptor, image_word(m, image, 0));
        if (!result)
            return 0;
        rrj_write32(cache, image_word(m, image, 0));
    }
    if (!result)
        return 0;
    context = rrj_read32(0x8005B470);
    packet = rrj_read32(context + 268);
    if (packet + 48 >= rrj_read32(0x8005B4D0))
    {
        uint32_t replacement = sub_80021C98(packet, 48);
        rrj_write32(rrj_read32(0x8005B470) + 268, replacement);
    }
    context = rrj_read32(0x8005B470);
    packet = rrj_read32(context + 268);
    rrj_write32(context + 268, packet + 48); /* Reserve 48 even for one 24-byte packet. */
    rrj_write32(packet, rrj_read32(link) | 0x05000000);
    rrj_write32(packet + 4, (h(m, descriptor + 28) & 0x9ff) | 0xe1000200);
    sh(m, packet + 18, h(m, descriptor + 30));
    rrj_write32(packet + 8, image_word(m, image, 4) | 0x64000000);
    sh(m, packet + 12, image_half(m, image, 8));
    sh(m, packet + 14, image_half(m, image, 10));
    sb(m, packet + 16, b(m, descriptor + 24));
    sb(m, packet + 17, b(m, descriptor + 26));
    sh(m, packet + 20, wide ? 256 : h(m, descriptor + 20));
    sh(m, packet + 22, h(m, descriptor + 22));
    rrj_write32(link, packet);
    if (wide)
    {
        uint32_t format, x, y;
        packet += 24;
        rrj_write32(packet, rrj_read32(link) | 0x05000000);
        sh(m, packet + 18, h(m, descriptor + 30));
        rrj_write32(packet + 8, image_word(m, image, 4) | 0x64000000);
        sh(m, packet + 12, image_half(m, image, 8) + 256);
        sh(m, packet + 14, image_half(m, image, 10));
        sb(m, packet + 16, b(m, descriptor + 24));
        sb(m, packet + 17, b(m, descriptor + 26));
        sh(m, packet + 20, h(m, descriptor + 20) - 256);
        sh(m, packet + 22, h(m, descriptor + 22));
        format = rrj_read32(descriptor) & 3;
        if (format != 3)
        {
            y = h(m, descriptor + 18);
            x = h(m, descriptor + 16) + (64u << format);
            rrj_write32(packet + 4, 0xe1000200 | (format << 7) | ((y & 256) >> 4) | ((x & 1023) >> 6) | ((y & 512) * 4));
        } /* Format 3 leaves the second page word untouched, as in MIPS. */
        rrj_write32(link, packet);
    }
    return result;
}

uint32_t sub_F_800700F0(uint32_t menu, uint32_t image, uint32_t link, RRJMenuResource resource)
{
    FUNCTION_MARKER(0x800700F0u, "RASHCDF.BIN");
    ImageSource source = {image, NULL};
    return draw_image(rrj_host_context(), menu, source, link, resource);
}

uint32_t rrj_menu_image_local(RRJMemory *m, uint32_t menu, const uint8_t image[12], uint32_t link, RRJMenuResource resource)
{
    ImageSource source = {0, image};
    if (!image)
        abort();
    return draw_image(m, menu, source, link, resource);
}

uint32_t sub_F_8006E894(uint32_t menu, uint32_t entry, RRJMenuResource resource)
{
    FUNCTION_MARKER(0x8006E894u, "RASHCDF.BIN");
    uint32_t link;
    if ((h(rrj_host_context(), entry + 10) & 32) && !(h(rrj_host_context(), menu) & 2))
        return 1;
    link = rrj_read32(0x8009CFC8) + signed_byte(b(rrj_host_context(), 0x8009C5E1)) * 4 + 28;
    return sub_F_800700F0(menu, entry + 16, link, resource);
}
