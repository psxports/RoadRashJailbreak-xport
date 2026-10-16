/* Hand translation of seven IDA drafts, checked against RRJ MIPS.
 * See status/decompilation for contracts, table bytes and differential probes.
 * Helpers preserve store order, link address bits, and untouched packet bytes.
 */
#include "flare.h"
#include "packet.h"
#include "psx.h"
#include "xport.h"

static uint32_t packet(uint32_t size)
{
    uint32_t context = r_u32(0x8005B470);
    uint32_t address = r_u32(context + 0x10C);
    if (address + size >= r_u32(0x8005B4D0))
    {
        address = sub_80021C98(address, size);
        context = r_u32(0x8005B470);
        w_u32(context + 0x10C, address);
    }
    context = r_u32(0x8005B470);
    address = r_u32(context + 0x10C);
    w_u32(context + 0x10C, address + size);
    return address;
}

uint32_t sub_8002AF9C(const void *quad, const void *color, void *link)
{
    FUNCTION_MARKER(0x8002AF9Cu, "SLUS_010.53");
    const uint8_t *pt = (const uint8_t *)quad;
    uint32_t address = packet(24), value;
    uint8_t *prim = (uint8_t *)xport_guest_ptr(address, 24);
    xport_store_le32(prim, xport_load_le32(link));
    xport_store_le32(link, address);
    value = xport_load_le32(color);
    w_u8(address + 3, 5);
    xport_store_le32(prim + 4, value);
    w_u8(address + 7, 0x2A);
    xport_store_le32(prim + 8, xport_load_le32(pt));
    xport_store_le32(prim + 12, xport_load_le32(pt + 4));
    xport_store_le32(prim + 16, xport_load_le32(pt + 8));
    value = xport_load_le32(pt + 12);
    xport_store_le32(prim + 20, value);
    return value;
}

uint32_t sub_8002B080(const void *quad, const void *color, void *link)
{
    const uint8_t *pt = (const uint8_t *)quad;
    uint32_t address = packet(24), value;
    uint8_t *prim = (uint8_t *)xport_guest_ptr(address, 24);

    FUNCTION_MARKER(0x8002B080, "SLUS_010.53");
    xport_store_le32(prim, xport_load_le32(link));
    xport_store_le32(link, address);
    value = xport_load_le32(color);
    w_u8(address + 3, 5);
    xport_store_le32(prim + 4, value);
    w_u8(address + 7, 0x28);
    xport_store_le32(prim + 8, xport_load_le32(pt));
    xport_store_le32(prim + 12, xport_load_le32(pt + 4));
    xport_store_le32(prim + 16, xport_load_le32(pt + 8));
    value = xport_load_le32(pt + 12);
    xport_store_le32(prim + 20, value);
    return value;
}

uint32_t sub_8002B164(const void *quad, const void *color, void *link)
{
    FUNCTION_MARKER(0x8002B164u, "SLUS_010.53");
    const uint8_t *pt = (const uint8_t *)quad;
    uint32_t address = packet(36), value;
    uint8_t *prim = (uint8_t *)xport_guest_ptr(address, 36);
    xport_store_le32(prim, xport_load_le32(link));
    xport_store_le32(link, address);
    value = xport_load_le32(color);
    xport_store_le32(prim + 12, 0);
    xport_store_le32(prim + 4, value);
    value = xport_load_le32(color); /* second read: retain even if color aliases packet */
    w_u8(address + 3, 8);
    xport_store_le32(prim + 28, 0);
    w_u8(address + 7, 0x3A);
    xport_store_le32(prim + 20, value);
    xport_store_le32(prim + 8, xport_load_le32(pt));
    xport_store_le32(prim + 16, xport_load_le32(pt + 4));
    xport_store_le32(prim + 24, xport_load_le32(pt + 8));
    value = xport_load_le32(pt + 12);
    xport_store_le32(prim + 32, value);
    return value;
}

uint32_t sub_8002B258(const void *quad, const void *color, uint32_t type, void *link)
{
    FUNCTION_MARKER(0x8002B258u, "SLUS_010.53");
    const uint8_t *pt = (const uint8_t *)quad;
    const uint8_t *texture = (const uint8_t *)xport_guest_cptr(0x800D4270 + type * 28, 24);
    uint32_t address = packet(40), value;
    uint8_t *prim = (uint8_t *)xport_guest_ptr(address, 40);
    xport_store_le32(prim, xport_load_le32(link));
    xport_store_le32(link, address);
    value = xport_load_le32(color);
    w_u8(address + 3, 9);
    xport_store_le32(prim + 4, value);
    w_u8(address + 7, 0x2E);
    xport_store_le32(prim + 8, xport_load_le32(pt));
    xport_store_le32(prim + 16, xport_load_le32(pt + 4));
    xport_store_le32(prim + 24, xport_load_le32(pt + 8));
    xport_store_le32(prim + 32, xport_load_le32(pt + 12));
    w_u8(address + 12, texture[0]);
    w_u8(address + 13, texture[12] - 1 + texture[4]);
    w_u8(address + 20, texture[0]);
    w_u8(address + 21, texture[4]);
    w_u8(address + 28, texture[8] - 1 + texture[0]);
    w_u8(address + 29, texture[12] - 1 + texture[4]);
    w_u8(address + 36, texture[8] - 1 + texture[0]);
    w_u8(address + 37, texture[4]);
    xport_store_le16(prim + 22, (xport_load_le16(texture + 16) & 0xFF9F) | 0x20);
    value = xport_load_le16(texture + 20);
    xport_store_le16(prim + 14, value);
    return value;
}

static void xy_offset(uint8_t *point, const void *xy, uint32_t x, uint32_t y)
{
    xport_store_le16(point, xport_load_le16(xy) + x);
    xport_store_le16(point + 2, xport_load_le16((const uint8_t *)xy + 2) + y);
}

uint32_t sub_8002B3FC(const void *xy, const void *color, uint32_t width, uint32_t height, uint8_t type, void *link)
{
    FUNCTION_MARKER(0x8002B3FCu, "SLUS_010.53");
    uint8_t pt[16];
    xy_offset(pt, xy, 0u - width, height);
    xy_offset(pt + 4, xy, width, height);
    xy_offset(pt + 8, xy, 0u - width, 0u - height);
    xy_offset(pt + 12, xy, width, 0u - height);
    return sub_8002B258(pt, color, type, link);
}

uint32_t sub_8002B4A0(const void *xy, const void *color, uint32_t width, uint32_t height, void *link)
{
    FUNCTION_MARKER(0x8002B4A0u, "SLUS_010.53");
    uint8_t pt[24];
    uint32_t quarter = (uint32_t)(((int32_t)(uint32_t)(width)) / 4), half = (uint32_t)(((int32_t)(uint32_t)(width)) / 2);
    uint32_t h = (uint32_t)(((int32_t)(uint32_t)(height)) / 2);
    xy_offset(pt, xy, 0u - quarter, h);
    xy_offset(pt + 4, xy, quarter, h);
    xy_offset(pt + 8, xy, 0u - half, 0);
    xy_offset(pt + 12, xy, half, 0);
    xy_offset(pt + 16, xy, 0u - quarter, 0u - h);
    xy_offset(pt + 20, xy, quarter, 0u - h);
    sub_8002AF9C(pt, color, link);
    return sub_8002AF9C(pt + 8, color, link);
}

static uint32_t scaled(uint32_t size, const void *table_halfword)
{
    const uint16_t bits = xport_load_le16(table_halfword);
    const int32_t factor = bits < 0x8000 ? bits : (int32_t)bits - 65536;
    /* MULT/MFLO truncates BEFORE signed division, not after a 64-bit divide. */
    return (uint32_t)(((int32_t)(uint32_t)(size * (uint32_t)factor)) / 256);
}

uint32_t sub_8002B5B4(const void *xy, const void *color, uint32_t width, uint32_t height, void *link)
{
    FUNCTION_MARKER(0x8002B5B4u, "SLUS_010.53");
    uint8_t pt[32];
    unsigned i;
    const uint8_t *table = (const uint8_t *)xport_guest_cptr(0x80053A28, 64);
    for (i = 0; i < 8; ++i)
        xy_offset(pt + i * 4, xy, scaled(width, table + i * 8), scaled(height, table + i * 8 + 2));
    sub_8002AF9C(pt, color, link);
    sub_8002AF9C(pt + 8, color, link);
    return sub_8002AF9C(pt + 16, color, link);
}

uint32_t sub_8002B68C(const void *xy, const void *color, uint32_t width, uint32_t height, void *link)
{
    FUNCTION_MARKER(0x8002B68Cu, "SLUS_010.53");
    uint8_t pt[72];
    unsigned i;
    uint32_t result = 0;
    const uint8_t *table = (const uint8_t *)xport_guest_cptr(0x800539E8, 64);
    for (i = 0; i < 9; ++i)
    {
        const uint8_t *record = table + (i % 8) * 8;
        xy_offset(pt + i * 8, xy, scaled(width, record), scaled(height, record + 2));
        xy_offset(pt + i * 8 + 4, xy, scaled(width - 5, record), scaled(height - 5, record + 2));
    }
    for (i = 0; i < 8; ++i)
        result = sub_8002B164(pt + i * 8, color, link);
    return result;
}

uint32_t sub_8002B878(const void *xy, const void *color, void *link)
{
    uint32_t projected[8];
    int index;

    FUNCTION_MARKER(0x8002B878, "SLUS_010.53");
    for (index = 0; index != 8; ++index)
    {
        uint32_t source = 0x800539E8 + 8 * (uint32_t)index;
        SVECTOR vertex;
        sint32 screen;
        sint32 flags;

        vertex.vx = (sint16)r_u16(source);
        vertex.vy = (sint16)r_u16(source + 2);
        vertex.vz = (sint16)r_u16(source + 4);
        vertex.pad = (sint16)r_u16(source + 6);
        (void)gte_project(&vertex, &screen, &flags);
        projected[index] = (uint32_t)screen;
    }
    for (index = 7; index >= 0; --index)
    {
        int16_t first = (int16_t)r_u16(0x8005399E + 2 * (uint32_t)index);
        int16_t second = (int16_t)r_u16(0x800539AE + 2 * (uint32_t)index);
        uint32_t address = packet(28);
        uint8_t *prim = (uint8_t *)xport_guest_ptr(address, 28);

        xport_store_le32(prim, xport_load_le32(link));
        xport_store_le32(link, address);
        xport_store_le32(prim + 4, 0x32000000);
        w_u8(address + 3, 6);
        xport_store_le32(prim + 8, projected[(uint16_t)second]);
        xport_store_le32(prim + 12, xport_load_le32(color));
        xport_store_le32(prim + 16, xport_load_le32(xy));
        xport_store_le32(prim + 20, 0);
        xport_store_le32(prim + 24, projected[(uint16_t)first]);
    }
    return 0xffffffff;
}
