#include "game_main.h"
#include "packet.h"
#include "native_game_api.h"
#include "menu_hint.h"
#include "menu_sprite.h"
#include "xport.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint32_t sub_80072A60(int32_t player)
{
    FUNCTION_MARKER(0x80072A60, "RASHCDF.BIN");
    return player == 1 ? 252 : 246;
}

uint32_t sub_80072A80(int32_t player, int32_t option)
{
    FUNCTION_MARKER(0x80072A80, "RASHCDF.BIN");
    if (player == 1)
        return option == 1 ? 282 : option == 2 ? 288 : 276;
    return option == 1 ? 264 : option == 2 ? 270 : 258;
}

uint32_t sub_8007219C(uint32_t menu, uint32_t entry)
{
    uint32_t rect = entry + 16, font, link;
    FUNCTION_MARKER(0x8007219C, "RASHCDF.BIN");
    if (!(r_u16(r_u32(menu + 16) + 120 * (uint32_t)r_s16(menu + 4) + 10) & 256))
        return 1;
    font = r_u32(0x8009C5B8);
    w_u8(0x800D8078 + 24 * font + 3, 0);
    font = r_u32(0x8009C5B8);
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    (void)sub_8002CB08(font, r_u8(0x800D80EB) + 18, xport_guest_cptr(rect, 8), link, r_u32(rect + 12), r_u16(rect + 10));
    return 1;
}

uint32_t sub_80072248(uint32_t menu, uint32_t entry)
{
    uint32_t rect = entry + 16, font, link;
    uint16_t shifted[4];
    uint32_t id;
    FUNCTION_MARKER(0x80072248, "RASHCDF.BIN");
    if (!(r_u16(r_u32(menu + 16) + 120 * (uint32_t)r_s16(menu + 4) + 10) & 256))
        return 1;
    font = r_u32(0x8009C5B8);
    w_u8(0x800D8078 + 24 * font + 3, 0);
    font = r_u32(0x8009C5B8);
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    (void)sub_8002CB08(font, 29, xport_guest_cptr(rect, 8), link, r_u32(rect + 12), r_u16(rect + 10));
    shifted[0] = (uint16_t)(r_u16(rect) + 95);
    shifted[1] = r_u16(rect + 2);
    shifted[2] = r_u16(rect + 4);
    shifted[3] = r_u16(rect + 6);
    font = r_u32(0x8009C5B8);
    id = r_u8(0x800D80EA) + 7;
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    (void)sub_8002CB08(font, id, shifted, link, r_u32(rect + 12), r_u16(rect + 10));
    return 1;
}

uint32_t sub_80072388(uint32_t menu, uint32_t entry)
{
    uint32_t rect = entry + 16, font, link;
    uint16_t shifted[4];
    uint32_t id;
    FUNCTION_MARKER(0x80072388, "RASHCDF.BIN");
    if (!(r_u16(r_u32(menu + 16) + 120 * (uint32_t)r_s16(menu + 4) + 10) & 256))
        return 1;
    font = r_u32(0x8009C5B8);
    w_u8(0x800D8078 + 24 * font + 3, 0);
    font = r_u32(0x8009C5B8);
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    (void)sub_8002CB08(font, 30, xport_guest_cptr(rect, 8), link, r_u32(rect + 12), r_u16(rect + 10));
    shifted[0] = (uint16_t)(r_u16(rect) + 95);
    shifted[1] = r_u16(rect + 2);
    shifted[2] = r_u16(rect + 4);
    shifted[3] = r_u16(rect + 6);
    font = r_u32(0x8009C5B8);
    id = 297 + 6 * (uint32_t)r_s8(0x800D80DF);
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    (void)sub_8002CB08(font, id, shifted, link, r_u32(rect + 12), r_u16(rect + 10));
    return 1;
}

uint32_t sub_800724D0(uint32_t menu, uint32_t entry)
{
    uint32_t rect = entry + 16, font, link;
    uint16_t shifted[4];
    char text[64];
    int32_t number;
    FUNCTION_MARKER(0x800724D0, "RASHCDF.BIN");
    if (!(r_u16(r_u32(menu + 16) + 120 * (uint32_t)r_s16(menu + 4) + 10) & 256))
        return 1;
    font = r_u32(0x8009C5B8);
    w_u8(0x800D8078 + 24 * font + 3, 0);
    font = r_u32(0x8009C5B8);
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    (void)sub_8002CB08(font, 1591, xport_guest_cptr(rect, 8), link, r_u32(rect + 12), r_u16(rect + 10));
    shifted[0] = (uint16_t)(r_u16(rect) + 95);
    shifted[1] = r_u16(rect + 2);
    shifted[2] = r_u16(rect + 4);
    shifted[3] = r_u16(rect + 6);
    number = r_s32(0x80099500 + 4 * (uint32_t)r_s8(0x800D80DC));
    (void)snprintf(text, sizeof(text), "%d:%02d", number / 60, number % 60);
    font = r_u32(0x8009C5B8);
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    (void)sub_8002CC74(font, (const uint8_t *)text, shifted, link, r_u32(rect + 12), r_u16(rect + 10));
    return 1;
}

uint32_t sub_80072668(uint32_t menu, uint32_t entry)
{
    uint32_t rect = entry + 16, font, link;
    uint16_t shifted[4];
    char text[64];
    int32_t number;
    FUNCTION_MARKER(0x80072668, "RASHCDF.BIN");
    if (!(r_u16(r_u32(menu + 16) + 120 * (uint32_t)r_s16(menu + 4) + 10) & 256))
        return 1;
    font = r_u32(0x8009C5B8);
    w_u8(0x800D8078 + 24 * font + 3, 0);
    font = r_u32(0x8009C5B8);
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    (void)sub_8002CB08(font, 1592, xport_guest_cptr(rect, 8), link, r_u32(rect + 12), r_u16(rect + 10));
    shifted[0] = (uint16_t)(r_u16(rect) + 95);
    shifted[1] = r_u16(rect + 2);
    shifted[2] = r_u16(rect + 4);
    shifted[3] = r_u16(rect + 6);
    number = r_s16(0x80099530 + 2 * (uint32_t)r_s8(0x800D80DC));
    (void)snprintf(text, sizeof(text), "%02d", number);
    font = r_u32(0x8009C5B8);
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    (void)sub_8002CC74(font, (const uint8_t *)text, shifted, link, r_u32(rect + 12), r_u16(rect + 10));
    return 1;
}

uint32_t sub_800727C8(uint32_t menu, uint32_t entry)
{
    uint32_t rect = entry + 16, font, link;
    uint16_t shifted[4];
    uint32_t id;
    FUNCTION_MARKER(0x800727C8, "RASHCDF.BIN");
    if (!(r_u16(r_u32(menu + 16) + 120 * (uint32_t)r_s16(menu + 4) + 10) & 256))
        return 1;
    font = r_u32(0x8009C5B8);
    w_u8(0x800D8078 + 24 * font + 3, 0);
    font = r_u32(0x8009C5B8);
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    (void)sub_8002CB08(font, 31, xport_guest_cptr(rect, 8), link, r_u32(rect + 12), r_u16(rect + 10));
    shifted[0] = (uint16_t)(r_u16(rect) + 60);
    shifted[1] = r_u16(rect + 2);
    shifted[2] = r_u16(rect + 4);
    shifted[3] = r_u16(rect + 6);
    id = (uint32_t)(int32_t)(int16_t)sub_80072A60(r_s8(0x800D81E1));
    font = r_u32(0x8009C5B8);
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    (void)sub_8002CB08(font, id, shifted, link, r_u32(rect + 12), r_u16(rect + 10));
    return 1;
}

uint32_t sub_80072910(uint32_t menu, uint32_t entry)
{
    uint32_t rect = entry + 16, font, link;
    uint16_t shifted[4];
    uint32_t id;
    FUNCTION_MARKER(0x80072910, "RASHCDF.BIN");
    if (!(r_u16(r_u32(menu + 16) + 120 * (uint32_t)r_s16(menu + 4) + 10) & 256))
        return 1;
    font = r_u32(0x8009C5B8);
    w_u8(0x800D8078 + 24 * font + 3, 0);
    font = r_u32(0x8009C5B8);
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    (void)sub_8002CB08(font, 32, xport_guest_cptr(rect, 8), link, r_u32(rect + 12), r_u16(rect + 10));
    shifted[0] = (uint16_t)(r_u16(rect) + 60);
    shifted[1] = r_u16(rect + 2);
    shifted[2] = r_u16(rect + 4);
    shifted[3] = r_u16(rect + 6);
    id = (uint32_t)(int32_t)(int16_t)sub_80072A80(r_s8(0x800D81E1), r_s8(0x800D81E2));
    font = r_u32(0x8009C5B8);
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    (void)sub_8002CB08(font, id, shifted, link, r_u32(rect + 12), r_u16(rect + 10));
    return 1;
}


uint32_t sub_80071FDC(int32_t rider)
{
    FUNCTION_MARKER(0x80071FDC, "RASHCDF.BIN");
    return (uint32_t)rider < 21 ? 1005 + 3 * (uint32_t)rider : 0xFFFFFFFF;
}

uint32_t sub_800720B4(int32_t track)
{
    uint32_t index = (uint32_t)track - 1;
    FUNCTION_MARKER(0x800720B4, "RASHCDF.BIN");
    if (r_s8(0x800D80DE) != 2)
        return 6 * index + 405;
    if ((int32_t)index >= 19)
        index -= 19;
    return 6 * index + 789;
}

uint32_t sub_800758D4(uint32_t menu, uint32_t entry)
{
    FUNCTION_MARKER(0x800758D4, "RASHCDF.BIN");
    w_u8(0x800D8078 + 24 * r_u32(0x8009C5B8) + 3, 0);
    return 1;
}

uint32_t sub_80075AFC(uint32_t menu, uint32_t entry)
{
    uint32_t rect = entry + 16, desc, link;
    FUNCTION_MARKER(0x80075AFC, "RASHCDF.BIN");
    w_u8(0x800D8078 + 24 * r_u32(0x8009C5C0) + 3, 0);
    desc = sub_80063C7C(r_u16(rect + 8));
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    (void)sub_8002CB08(r_u32(0x8009C5B8), (uint32_t)r_s16(desc), xport_guest_cptr(rect, 8), link, r_u32(rect + 12), r_u16(rect + 10));
    return 1;
}

uint32_t sub_80072AF4(uint32_t menu, uint32_t entry)
{
    uint32_t rect = entry + 16, font, link, id;
    uint16_t shifted[4];
    FUNCTION_MARKER(0x80072AF4, "RASHCDF.BIN");
    if (!(r_u16(r_u32(menu + 16) + 120 * (uint32_t)r_s16(menu + 4) + 10) & 256))
        return 1;
    font = r_u32(0x8009C5B8);
    w_u8(0x800D8078 + 24 * font + 3, 0);
    font = r_u32(0x8009C5B8);
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    id = 24;
    (void)sub_8002CB08(font, id, xport_guest_cptr(rect, 8), link, r_u32(rect + 12), r_u16(rect + 10));
    shifted[0] = (uint16_t)(r_u16(rect) + 95);
    shifted[1] = r_u16(rect + 2);
    shifted[2] = r_u16(rect + 4);
    shifted[3] = r_u16(rect + 6);
    id = (uint32_t)(int32_t)(int16_t)sub_80071F0C((uint32_t)r_s8(0x800D80DC), r_u32(0x800D80D8));
    font = r_u32(0x8009C5B8);
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    (void)sub_8002CB08(font, id, shifted, link, r_u32(rect + 12), r_u16(rect + 10));
    return 1;
}

uint32_t sub_80072C44(uint32_t menu, uint32_t entry)
{
    uint32_t rect = entry + 16, font, link, id;
    uint16_t shifted[4];
    FUNCTION_MARKER(0x80072C44, "RASHCDF.BIN");
    if (!(r_u16(r_u32(menu + 16) + 120 * (uint32_t)r_s16(menu + 4) + 10) & 256))
        return 1;
    font = r_u32(0x8009C5B8);
    w_u8(0x800D8078 + 24 * font + 3, 0);
    font = r_u32(0x8009C5B8);
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    id = 24;
    (void)sub_8002CB08(font, id, xport_guest_cptr(rect, 8), link, r_u32(rect + 12), r_u16(rect + 10));
    shifted[0] = (uint16_t)(r_u16(rect) + 120);
    shifted[1] = r_u16(rect + 2);
    shifted[2] = r_u16(rect + 4);
    shifted[3] = r_u16(rect + 6);
    id = (uint32_t)(int32_t)(int16_t)sub_80071F0C((uint32_t)r_s8(0x800D80DC), r_u32(0x800D80D8));
    font = r_u32(0x8009C5B8);
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    (void)sub_8002CB08(font, id, shifted, link, r_u32(rect + 12), r_u16(rect + 10));
    return 1;
}

uint32_t sub_80072D94(uint32_t menu, uint32_t entry)
{
    uint32_t rect = entry + 16, font, link, id;
    uint16_t shifted[4];
    char text[64];
    FUNCTION_MARKER(0x80072D94, "RASHCDF.BIN");
    if (!(r_u16(r_u32(menu + 16) + 120 * (uint32_t)r_s16(menu + 4) + 10) & 256))
        return 1;
    font = r_u32(0x8009C5B8);
    w_u8(0x800D8078 + 24 * font + 3, 0);
    font = r_u32(0x8009C5B8);
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    id = 222;
    (void)sub_8002CB08(font, id, xport_guest_cptr(rect, 8), link, r_u32(rect + 12), r_u16(rect + 10));
    shifted[0] = (uint16_t)(r_u16(rect) + 120);
    shifted[1] = r_u16(rect + 2);
    shifted[2] = r_u16(rect + 4);
    shifted[3] = r_u16(rect + 6);
    (void)snprintf(text, sizeof(text), "%01d%s%01d", r_u8(0x800D80E9),
        (const char *)xport_guest_cptr(r_u32(r_u32(0x8005B544) + 4), 1), r_u32(0x800D80D8) == 32 ? 9 : 6);
    font = r_u32(0x8009C5B8);
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    (void)sub_8002CC74(font, (const uint8_t *)text, shifted, link, r_u32(rect + 12), r_u16(rect + 10));
    return 1;
}

uint32_t sub_80072F2C(uint32_t menu, uint32_t entry)
{
    uint32_t rect = entry + 16, font, link, id;
    uint16_t shifted[4];
    FUNCTION_MARKER(0x80072F2C, "RASHCDF.BIN");
    if (!(r_u16(r_u32(menu + 16) + 120 * (uint32_t)r_s16(menu + 4) + 10) & 256))
        return 1;
    font = r_u32(0x8009C5B8);
    w_u8(0x800D8078 + 24 * font + 3, 0);
    font = r_u32(0x8009C5B8);
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    id = 25;
    (void)sub_8002CB08(font, id, xport_guest_cptr(rect, 8), link, r_u32(rect + 12), r_u16(rect + 10));
    shifted[0] = (uint16_t)(r_u16(rect) + 95);
    shifted[1] = r_u16(rect + 2);
    shifted[2] = r_u16(rect + 4);
    shifted[3] = r_u16(rect + 6);
    id = (uint32_t)(int32_t)(int16_t)sub_800720B4(r_s8(0x800D80E0));
    font = r_u32(0x8009C5B8);
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    (void)sub_8002CB08(font, id, shifted, link, r_u32(rect + 12), r_u16(rect + 10));
    return 1;
}

uint32_t sub_80073074(uint32_t menu, uint32_t entry, uint32_t selector)
{
    uint32_t rect = entry + 16, font, link, id;
    uint16_t shifted[4];
    uint32_t player;
    FUNCTION_MARKER(0x80073074, "RASHCDF.BIN");
    if (!(r_u16(r_u32(menu + 16) + 120 * (uint32_t)r_s16(menu + 4) + 10) & 256))
        return 1;
    font = r_u32(0x8009C5B8);
    w_u8(0x800D8078 + 24 * font + 3, 0);
    font = r_u32(0x8009C5B8);
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    id = selector == 5 ? 26 : selector == 6 ? 27 : 28;
    (void)sub_8002CB08(font, id, xport_guest_cptr(rect, 8), link, r_u32(rect + 12), r_u16(rect + 10));
    shifted[0] = (uint16_t)(r_u16(rect) + 95);
    shifted[1] = r_u16(rect + 2);
    shifted[2] = r_u16(rect + 4);
    shifted[3] = r_u16(rect + 6);
    player = selector == 5 || selector == 6 ? 0 : 1;
    if (r_u32(0x800D80D8) == 4)
        player = r_u8(0x800D80EB);
    id = (uint32_t)(int32_t)(int16_t)sub_80071FDC(r_s8(0x800D81DF + 36 * player));
    font = r_u32(0x8009C5B8);
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    (void)sub_8002CB08(font, id, shifted, link, r_u32(rect + 12), r_u16(rect + 10));
    return 1;
}

uint32_t sub_80073234(uint32_t menu, uint32_t entry, uint32_t selector)
{
    uint32_t rect = entry + 16, font, link, id;
    uint16_t shifted[4];
    int32_t status;
    FUNCTION_MARKER(0x80073234, "RASHCDF.BIN");
    if (!(r_u16(r_u32(menu + 16) + 120 * (uint32_t)r_s16(menu + 4) + 10) & 256))
        return 1;
    font = r_u32(0x8009C5B8);
    w_u8(0x800D8078 + 24 * font + 3, 0);
    font = r_u32(0x8009C5B8);
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    id = 199;
    (void)sub_8002CB08(font, id, xport_guest_cptr(rect, 8), link, r_u32(rect + 12), r_u16(rect + 10));
    shifted[0] = (uint16_t)(r_u16(rect) + 95);
    shifted[1] = r_u16(rect + 2);
    shifted[2] = r_u16(rect + 4);
    shifted[3] = r_u16(rect + 6);
    status = r_s8(0x800D81ED + 36 * (selector != 8));
    id = status == 1 ? 196 : status == 2 ? 197 : 198;
    font = r_u32(0x8009C5B8);
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    (void)sub_8002CB08(font, id, shifted, link, r_u32(rect + 12), r_u16(rect + 10));
    return 1;
}


uint32_t sub_8007593C(uint32_t menu, uint32_t entry)
{
    uint32_t rect = entry + 16, link, font, label;
    uint16_t labels[4];
    uint8_t image[12];
    FUNCTION_MARKER(0x8007593C, "RASHCDF.BIN");
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    w_u8(0x800D8078 + 24 * r_u32(0x8009C5C0) + 3, 0);
    labels[0] = (uint16_t)(r_u16(rect) + 16);
    labels[1] = r_u16(rect + 2);
    labels[2] = r_u16(rect + 4);
    labels[3] = r_u16(rect + 6);
    (void)sub_8002CB08(r_u32(0x8009C5B8), 211, xport_guest_cptr(rect, 8), link, 0xB9A096, 2);
    xport_store_le32(image, 300);
    xport_store_le32(image + 4, 0x808080);
    labels[1] = (uint16_t)(labels[1] + 14);
    xport_store_le16(image + 8, r_u16(rect));
    xport_store_le16(image + 10, labels[1]);
    for (label = 212; label < 218; ++label)
    {
        xport_store_le32(image, r_u32(0x80088DF4 + 4 * (label - 212)));
        (void)sub_800700F0(menu, image, r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 20);
        font = r_u32(0x8009C5C0);
        (void)sub_8002CB08(font, label, labels, link, r_u32(rect + 12), r_u16(rect + 10));
        labels[1] = (uint16_t)(labels[1] + 12);
        xport_store_le16(image + 10, xport_load_le16(image + 10) + 12);
    }
    return 1;
}

uint32_t sub_80074194(uint32_t menu, uint32_t entry)
{
    uint32_t rect = entry + 16, link, id;
    uint16_t position[4];
    char text[128];
    FUNCTION_MARKER(0x80074194, "RASHCDF.BIN");
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    w_u8(0x800D8078 + 24 * r_u32(0x8009C5B8) + 3, 0);
    position[0] = (uint16_t)(r_u16(rect) + 10);
    position[1] = r_u16(rect + 2);
    position[2] = r_u16(rect + 4);
    position[3] = r_u16(rect + 6);
    (void)sub_8002CB08(r_u32(0x8009C5B8), 1541, position, link, r_u32(rect + 12), r_u16(rect + 10));
    position[0] = (uint16_t)(position[0] + 160);
    id = (uint32_t)(int32_t)(int16_t)sub_80071F0C((uint32_t)r_s8(0x800D80DC), r_u32(0x800D80D8));
    (void)sub_8002CB08(r_u32(0x8009C5B8), id, position, link, r_u32(rect + 12), r_u16(rect + 10));
    if (r_s16(0x8009C634) < (int32_t)r_u8(0x800D80E9))
    {
        w_u8(0x8009C646, r_u8(0x8009C646) + 1);
        if (r_s8(0x8009C646) >= 6)
        {
            w_u8(0x8009C646, 0);
            w_u16(0x8009C634, r_u16(0x8009C634) + 1);
        }
    }
    position[0] = (uint16_t)(r_u16(rect) + 10);
    position[1] = (uint16_t)(position[1] + 14);
    id = (uint32_t)(int32_t)(int16_t)(r_u16(0x8009C634) + 1510);
    (void)sub_8002CB08(r_u32(0x8009C5B8), id, position, link, r_u32(rect + 12), r_u16(rect + 10));
    if (r_s16(0x8009C634) != (int32_t)r_u8(0x800D80E9))
        return 1;
    if (r_s16(0x8009C636) < r_s16(0x800D80F8))
    {
        w_u8(0x8009C646, r_u8(0x8009C646) + 1);
        if (r_s8(0x8009C646) >= 6)
        {
            w_u8(0x8009C646, 0);
            w_u16(0x8009C636, r_u16(0x8009C636) + 1);
        }
    }
    position[1] = (uint16_t)(position[1] + 14);
    (void)sub_8002CB08(r_u32(0x8009C5B8), 1517, position, link, r_u32(rect + 12), r_u16(rect + 10));
    position[0] = (uint16_t)(position[0] + 160);
    (void)snprintf(text, sizeof(text), "%02d", r_s16(0x8009C636));
    (void)sub_8002CC74(r_u32(0x8009C5B8), (const uint8_t *)text, position, link, r_u32(rect + 12), r_u16(rect + 10));
    if (r_s16(0x8009C634) != (int32_t)r_u8(0x800D80E9) || r_s16(0x8009C636) != r_s16(0x800D80F8))
        return 1;
    if (r_s16(0x8009C638) < r_s16(0x800D80FA))
    {
        w_u8(0x8009C646, r_u8(0x8009C646) + 1);
        if (r_s8(0x8009C646) >= 6)
        {
            w_u8(0x8009C646, 0);
            w_u16(0x8009C638, r_u16(0x8009C638) + 1);
        }
    }
    position[0] = (uint16_t)(r_u16(rect) + 10);
    position[1] = (uint16_t)(position[1] + 14);
    (void)sub_8002CB08(r_u32(0x8009C5B8), 1518, position, r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4, r_u32(rect + 12), r_u16(rect + 10));
    position[0] = (uint16_t)(position[0] + 160);
    (void)snprintf(text, sizeof(text), "%02d", r_s16(0x8009C638));
    (void)sub_8002CC74(r_u32(0x8009C5B8), (const uint8_t *)text, position, link, r_u32(rect + 12), r_u16(rect + 10));
    return 1;
}

uint32_t sub_80074C9C(void)
{
    uint32_t link, record, row, column, id;
    int32_t time, minutes, seconds, hundredths;
    uint16_t positions[5][4] = {{31,75,120,120},{53,75,120,120},{189,75,120,120},{340,75,120,120},{419,75,120,120}};
    char text[128];
    FUNCTION_MARKER(0x80074C9C, "RASHCDF.BIN");
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    w_u8(0x800D8078 + 24 * r_u32(0x8009C5C0) + 3, 0);
    for (column = 0; column < 5; ++column)
        (void)sub_8002CB08(r_u32(0x8009C5C0), 225 + column, positions[column], link, 0xB9A096, 0);
    record = 0x80053A88 + 176 * ((uint32_t)r_s8(0x8009C5EF) - 56);
    for (row = 0; row < 8; ++row, record += 20)
    {
        for (column = 0; column < 5; ++column)
            positions[column][1] = (uint16_t)(positions[column][1] + 14);
        (void)snprintf(text, sizeof(text), "%d", row + 1);
        (void)sub_8002CC74(r_u32(0x8009C5C0), (const uint8_t *)text, positions[0], link, 0xB9A096, 0);
        (void)sub_8002CC74(r_u32(0x8009C5C0), xport_guest_cptr(record + 16, 1), positions[1], link, 0x286EB4, 0);
        id = sub_80071FDC(r_s8(record + 32));
        (void)sub_8002CB08(r_u32(0x8009C5C0), id, positions[2], link, 0x286EB4, 0);
        time = r_s32(record + 28);
        if (time == 9999)
            (void)snprintf(text, sizeof(text), "00:00.00");
        else
        {
            minutes = time / 18000;
            seconds = time / 300 - 60 * minutes;
            hundredths = time / 3 - 100 * (time / 300);
            (void)snprintf(text, sizeof(text), "%02d:%02d.%02d", minutes, seconds, hundredths);
        }
        (void)sub_8002CC74(r_u32(0x8009C5C0), (const uint8_t *)text, positions[3], link, 0x286EB4, 0);
        if (r_s8(record + 35))
            (void)sub_8002CB08(r_u32(0x8009C5C0), 230, positions[4], link, 0x286EB4, 0);
        if (r_s8(record + 33))
            (void)sub_8002CB08(r_u32(0x8009C5C0), 231, positions[4], link, 0x286EB4, 0);
        if (r_s8(record + 34))
            (void)sub_8002CB08(r_u32(0x8009C5C0), 232, positions[4], link, 0x286EB4, 0);
    }
    return 1;
}

uint32_t sub_800642DC(uint32_t list)
{
    int32_t original = r_s8(list + 3), index = original + (-1), type;
    uint32_t entry, id, offset, flags;
    FUNCTION_MARKER(0x800642DC, "RASHCDF.BIN");
    if (index < 0) index = r_s8(list) - 1;
    while (index != original)
    {
        entry = r_u32(list + 4) + 12 * (uint32_t)index;
        if (r_u8(entry + 3) & r_u8(0x8009C5E4))
        {
            type = r_s8(entry);
            if (type == 4 || type == 8)
            {
                id = r_u8(entry + 1);
                offset = (uint32_t)((int32_t)(id << 24) >> 27);
                if ((r_u8((type == 4 ? 0x800D81C8 : 0x800D81D4) + offset) >> (id & 7)) & 1)
                    goto next_642DC;
            }
            flags = r_u8(entry + 2);
            if ((flags & 64) && !r_u32(0x800D7138 + 192 * (uint32_t)r_s8(0x8009C5E6)))
                goto next_642DC;
            if ((flags & 32) && r_u32(0x800D7138 + 192 * (uint32_t)r_s8(0x8009C5E6)))
                goto next_642DC;
            if (!(flags & 128) || (r_u8(0x800D70E1) >> 4) == 8)
                break;
        }
next_642DC:
        index += (-1);
        if (index < 0) index = r_s8(list) - 1;
    }
    return (uint32_t)index;
}

uint32_t sub_80064474(uint32_t list)
{
    int32_t original = r_s8(list + 3), index = original + (1), type;
    uint32_t entry, id, offset, flags;
    FUNCTION_MARKER(0x80064474, "RASHCDF.BIN");
    if (index >= r_s8(list)) index = 0;
    while (index != original)
    {
        entry = r_u32(list + 4) + 12 * (uint32_t)index;
        if (r_u8(entry + 3) & r_u8(0x8009C5E4))
        {
            type = r_s8(entry);
            if (type == 4 || type == 8)
            {
                id = r_u8(entry + 1);
                offset = (uint32_t)((int32_t)(id << 24) >> 27);
                if ((r_u8((type == 4 ? 0x800D81C8 : 0x800D81D4) + offset) >> (id & 7)) & 1)
                    goto next_64474;
            }
            flags = r_u8(entry + 2);
            if ((flags & 64) && !r_u32(0x800D7138 + 192 * (uint32_t)r_s8(0x8009C5E6)))
                goto next_64474;
            if ((flags & 32) && r_u32(0x800D7138 + 192 * (uint32_t)r_s8(0x8009C5E6)))
                goto next_64474;
            if (!(flags & 128) || (r_u8(0x800D70E1) >> 4) == 8)
                break;
        }
next_64474:
        index += (1);
        if (index >= r_s8(list)) index = 0;
    }
    return (uint32_t)index;
}

uint32_t sub_80075D0C(uint32_t menu, uint32_t entry)
{
    uint32_t rect = entry + 16, link, image_link, arrow_link, current, desc, row, color, selected;
    int32_t previous, count;
    uint16_t position[4];
    uint8_t image[12], arrow[12];
    FUNCTION_MARKER(0x80075D0C, "RASHCDF.BIN");
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1);
    image_link = link + 16;
    arrow_link = link + 12;
    link += 4;
    xport_store_le32(image + 4, 0x808080);
    xport_store_le16(image + 8, 27);
    xport_store_le32(arrow + 4, 0x808080);
    w_u8(0x800D8078 + 24 * r_u32(0x8009C5BC) + 3, 0);
    position[0] = r_u16(rect);
    position[1] = r_u16(rect + 2);
    position[2] = 40;
    position[3] = r_u16(rect + 6);
    (void)sub_8002CB08(1, 1558, position, link, 0xB9A096, 2);
    position[0] = (uint16_t)(r_u16(rect) + 95);
    position[2] = 160;
    (void)sub_8002CB08(1, 1559, position, link, 0xB9A096, 2);
    position[0] = (uint16_t)(r_u16(rect) + 255);
    position[2] = 220;
    (void)sub_8002CB08(1, 1560, position, link, 0xB9A096, 2);
    position[1] = (uint16_t)(position[1] + 20);
    previous = r_s8(r_u32(0x8009C52C) + 3);
    selected = r_u8(0x8009C5DE);
    count = 0;
    while (count < r_s8(0x8009C5D6))
    {
        current = sub_800642DC(r_u32(0x8009C52C));
        ++count;
        w_u8(r_u32(0x8009C52C) + 3, current);
    }
    w_u8(0x800D8078 + 24 * r_u32(0x8009C5C0) + 3, 0);
    w_u32(0x8009C664, r_u32(0x8009C52C));
    for (row = 0; row < 5; ++row)
    {
        current = r_u32(0x8009C52C);
        w_u8(0x8009C5DE, r_u8(r_u32(current + 4) + 12 * (uint32_t)r_s8(current + 3) + 1));
        position[0] = r_u16(rect);
        position[2] = 40;
        desc = sub_80063C7C(39);
        if (desc)
        {
            xport_store_le16(image + 10, position[1] - 3);
            if (previous == r_s8(r_u32(0x8009C52C) + 3))
            {
                (void)sub_8002CB08(r_u32(0x8009C5C0), (uint32_t)r_s16(desc), position, link, 0xA08C8C, 2);
                xport_store_le32(image, 0x48544253);
                (void)sub_800700F0(menu, image, image_link);
                xport_store_le32(arrow, 0x5752414C);
                xport_store_le16(arrow + 8, 13);
                xport_store_le16(arrow + 10, position[1] - 3);
                (void)sub_800700F0(menu, arrow, arrow_link);
                xport_store_le32(arrow, 0x57524152);
                xport_store_le16(arrow + 8, 68);
                xport_store_le16(arrow + 10, position[1] - 3);
                (void)sub_800700F0(menu, arrow, arrow_link);
            }
            else
            {
                xport_store_le32(image, 0x4E544253);
                (void)sub_800700F0(menu, image, image_link);
                (void)sub_8002CB08(r_u32(0x8009C5C0), (uint32_t)r_s16(desc), position, link, 0x286EB4, 2);
            }
        }
        position[0] = (uint16_t)(r_u16(rect) + 95);
        position[2] = 160;
        desc = sub_80063C7C(57);
        if (desc)
        {
            color = previous == r_s8(r_u32(0x8009C52C) + 3) ? 0xA08C8C : 0x286EB4;
            (void)sub_8002CB08(r_u32(0x8009C5C0), (uint32_t)r_s16(desc), position, link, color, 2);
        }
        position[0] = (uint16_t)(r_u16(rect) + 255);
        position[2] = 220;
        desc = sub_80063C7C(58);
        if (desc)
        {
            color = previous == r_s8(r_u32(0x8009C52C) + 3) ? 0xA08C8C : 0x286EB4;
            (void)sub_8002CB08(r_u32(0x8009C5C0), (uint32_t)r_s16(desc), position, link, color, 2);
        }
        current = sub_80064474(r_u32(0x8009C52C));
        w_u8(r_u32(0x8009C52C) + 3, current);
        position[1] = (uint16_t)(position[1] + 14);
    }
    w_u8(r_u32(0x8009C52C) + 3, previous);
    w_u8(0x8009C5DE, selected);
    desc = sub_80063C7C(59);
    if (desc)
    {
        position[0] = 45;
        position[1] = 150;
        position[2] = 428;
        position[3] = 80;
        (void)sub_8002CB08(r_u32(0x8009C5C0), (uint32_t)r_s16(desc), position, link, 0xA08C8C, 3);
    }
    w_u32(0x8009C664, 0);
    return 1;
}

uint32_t sub_80071F70(uint32_t mode)
{
    FUNCTION_MARKER(0x80071F70, "RASHCDF.BIN");
    switch (mode)
    {
        case 1: return 45;
        case 4: return 47;
        case 8: return 296;
        case 16: return 75;
        case 17: return 77;
        case 24: return 81;
        case 32: return 43;
        default: return 0xFFFFFFFF;
    }
}

uint32_t sub_80070DD4(uint32_t menu, uint32_t rectangle)
{
    uint32_t context, view, link, visible = 0, cursor, bottom, label;
    int32_t offset_x, offset_y;
    uint16_t position[4];
    FUNCTION_MARKER(0x80070DD4, "RASHCDF.BIN");
    context = r_u32(0x8005B470);
    view = context + 112 * r_u8(context + 6);
    offset_x = r_s16(view + 24);
    offset_y = r_s16(view + 26);
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    (void)sub_8001C304(offset_x, offset_y, 512, 240, link);
    if (r_s16(menu + 2) == 32767)
    {
        w_u16(0x8009CCE0, 230);
        w_u8(rectangle + 23, 0);
    }
    else if (r_s8(rectangle + 23) < r_s8(rectangle + 21))
        w_u8(rectangle + 23, r_u8(rectangle + 23) + 1);
    else
    {
        w_u16(0x8009CCE0, r_u16(0x8009CCE0) - (uint32_t)r_s8(rectangle + 22));
        w_u8(rectangle + 23, 0);
    }
    cursor = r_u16(0x8009CCE0);
    position[0] = r_u16(rectangle);
    position[2] = r_u16(rectangle + 4);
    bottom = r_u16(rectangle + 2) + r_u16(rectangle + 6);
    w_u8(0x800D8078 + 24 * r_u32(0x8009C5C0) + 3, 0);
    for (label = 1596; label < 1945; ++label)
    {
        if ((int16_t)cursor >= (int16_t)bottom)
            break;
        if ((int32_t)(int16_t)cursor + r_s8(rectangle + 20) >= r_s16(rectangle + 2))
        {
            ++visible;
            position[1] = (uint16_t)cursor;
            position[3] = (uint16_t)(bottom - cursor);
            (void)sub_8002CB08(r_u32(0x8009C5C0), label, position, link, r_u32(rectangle + 12), r_u16(rectangle + 10));
        }
        cursor += (uint32_t)r_s8(rectangle + 20);
    }
    (void)sub_8001C304(r_s16(rectangle) + offset_x, r_s16(rectangle + 2) + offset_y, r_s16(rectangle + 4), r_s16(rectangle + 6), link);
    if (r_s16(0x8009CCE0) < (int16_t)bottom && !visible)
        w_u16(0x8009C5DA, r_u16(0x8009C5DA) | 8);
    return 1;
}

uint32_t sub_80071AF0(uint32_t rectangle, int32_t id)
{
    uint32_t link, row, column, record, mode, color, total;
    const uint16_t columns[4] = {0,120,220,340};
    int32_t label;
    uint16_t position[4];
    char text[64];
    FUNCTION_MARKER(0x80071AF0, "RASHCDF.BIN");
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    if (id != 15)
        return 1;
    position[0] = r_u16(rectangle);
    position[1] = r_u16(rectangle + 2);
    position[2] = r_u16(rectangle + 4);
    position[3] = r_u16(rectangle + 6);
    w_u8(0x800D8078 + 24 * r_u32(0x8009C5BC) + 3, 0);
    for (column = 0; column < 4; ++column)
    {
        position[0] = (uint16_t)(r_u16(rectangle) + columns[column]);
        (void)sub_8002CB08(r_u32(0x8009C5BC), 172 + column, position, link, r_u32(rectangle + 12), r_u16(rectangle + 10));
    }
    position[1] = (uint16_t)(position[1] + 20);
    for (row = 0; row < 10; ++row)
    {
        record = r_u32(0x8009954C);
        color = row == (uint32_t)r_s16(record + 14) ? 0xB9A096 : 0x286EB4;
        record += 2132 + 484 * row;
        if (r_u32(record))
        {
            for (column = 0; column < 4; ++column)
            {
                position[0] = (uint16_t)(r_u16(rectangle) + columns[column]);
                (void)sub_8002CB08(r_u32(0x8009C5B8), column ? 171 : 170, position, link, color, r_u16(rectangle + 10));
            }
        }
        else
        {
            mode = r_u32(record + 220);
            total = 6;
            label = mode == 1 ? 294 : 295;
            if (mode == 32)
            {
                total = 9;
                label = (int16_t)sub_80072A80(r_s8(record + 13), r_s8(record + 14));
            }
            if (label >= 0)
            {
                position[0] = r_u16(rectangle);
                (void)sub_8002CB08(r_u32(0x8009C5B8), (uint32_t)label, position, link, color, r_u16(rectangle + 10));
            }
            label = (int32_t)sub_80071F70(r_u32(record + 220));
            if (label >= 0)
            {
                position[0] = (uint16_t)(r_u16(rectangle) + 120);
                (void)sub_8002CB08(r_u32(0x8009C5B8), (uint32_t)label, position, link, color, r_u16(rectangle + 10));
            }
            label = (int32_t)sub_80071F0C((uint32_t)r_s8(record + 224), r_u32(record + 220));
            if (label >= 0)
            {
                position[0] = (uint16_t)(r_u16(rectangle) + 220);
                (void)sub_8002CB08(r_u32(0x8009C5B8), (uint32_t)label, position, link, color, r_u16(rectangle + 10));
            }
            (void)snprintf(text, sizeof(text), "%01d%s%01d", r_u8(record + 237),
                (const char *)xport_guest_cptr(r_u32(r_u32(0x8005B544) + 4), 1), total);
            position[0] = (uint16_t)(r_u16(rectangle) + 340);
            (void)sub_8002CC74(r_u32(0x8009C5B8), (const uint8_t *)text, position, link, color, r_u16(rectangle + 10));
        }
        position[1] = (uint16_t)(position[1] + 12);
    }
    return 1;
}

uint32_t sub_80073BAC(uint32_t menu, uint32_t entry)
{
    uint32_t rect = entry + 16, link, row, column, prior, display, target, first, second;
    const uint32_t targets[3] = {0x800D81F0,0x800D81F2,0x800D81F1};
    uint16_t position[4];
    char text[128];
    FUNCTION_MARKER(0x80073BAC, "RASHCDF.BIN");
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    w_u8(0x800D8078 + 24 * r_u32(0x8009C5B8) + 3, 0);
    position[0] = (uint16_t)(r_u16(rect) + 120);
    position[1] = r_u16(rect + 2);
    position[2] = r_u16(rect + 4);
    position[3] = r_u16(rect + 6);
    (void)sub_8002CB08(r_u32(0x8009C5B8), 1519, position, link, r_u32(rect + 12), r_u16(rect + 10));
    position[0] = (uint16_t)(r_u16(rect) + 170);
    (void)sub_8002CB08(r_u32(0x8009C5B8), 1520, position, link, r_u32(rect + 12), r_u16(rect + 10));
    position[0] = (uint16_t)(r_u16(rect) + 10);
    for (row = 0; row < 3; ++row)
    {
        position[1] = (uint16_t)(position[1] + 14);
        (void)sub_8002CB08(r_u32(0x8009C5B8), 1521 + row, position, link, r_u32(rect + 12), r_u16(rect + 10));
    }
    position[1] = (uint16_t)(r_u16(rect + 2) + 14);
    for (row = 0; row < 3; ++row)
    {
        for (prior = 0; prior < row; ++prior)
        {
            display = 0x8009C614 + 4 * prior;
            target = targets[prior];
            first = prior == 1 ? r_u16(target) : r_u8(target);
            second = prior == 1 ? r_u16(target + 4) : r_u8(target + 4);
            if (r_s16(display) != (int32_t)first || r_s16(display + 2) != (int32_t)second)
                return 1;
        }
        display = 0x8009C614 + 4 * row;
        target = targets[row];
        first = row == 1 ? r_u16(target) : r_u8(target);
        second = row == 1 ? r_u16(target + 4) : r_u8(target + 4);
        if (r_s16(display) < (int32_t)first || r_s16(display + 2) < (int32_t)second)
        {
            w_u16(0x8009C622, r_u16(0x8009C622) + 1);
            if (r_s16(0x8009C622) >= 6)
            {
                w_u16(0x8009C622, 0);
                if (r_s16(display) < (int32_t)first)
                    w_u16(display, r_u16(display) + 1);
                second = row == 1 ? r_u16(target + 4) : r_u8(target + 4);
                if (r_s16(display + 2) < (int32_t)second)
                    w_u16(display + 2, r_u16(display + 2) + 1);
            }
        }
        for (column = 0; column < 2; ++column)
        {
            position[0] = (uint16_t)(r_u16(rect) + (column ? 170 : 120));
            (void)snprintf(text, sizeof(text), "%02d", r_s16(display + 2 * column));
            (void)sub_8002CC74(r_u32(0x8009C5B8), (const uint8_t *)text, position, link, 0x1C058E, r_u16(rect + 10));
        }
        position[1] = (uint16_t)(position[1] + 14);
    }
    return 1;
}

uint32_t sub_8006EE20(const uint8_t rectangle[16], uint32_t link)
{
    uint32_t context, packet, replacement, mode;
    FUNCTION_MARKER(0x8006EE20, "RASHCDF.BIN");
    mode = 2 * (xport_load_le16(rectangle + 2) & 1);
    context = r_u32(0x8005B470);
    packet = r_u32(context + 268);
    if (packet + 24 >= r_u32(0x8005B4D0))
    {
        replacement = sub_80021C98(packet, 24);
        w_u32(r_u32(0x8005B470) + 268, replacement);
    }
    context = r_u32(0x8005B470);
    packet = r_u32(context + 268);
    w_u32(context + 268, packet + 24);
    w_u32(packet, r_u32(link) | 0x05000000);
    w_u32(packet + 4, ((mode | 0x28) << 24) | xport_load_le32(rectangle + 4));
    w_u32(packet + 8, (uint32_t)(int32_t)(int16_t)xport_load_le16(rectangle + 8) | ((uint32_t)(int32_t)(int16_t)xport_load_le16(rectangle + 10) << 16));
    w_u32(packet + 12, (uint32_t)(int32_t)(int16_t)xport_load_le16(rectangle + 12) | ((uint32_t)(int32_t)(int16_t)xport_load_le16(rectangle + 10) << 16));
    w_u32(packet + 16, (uint32_t)(int32_t)(int16_t)xport_load_le16(rectangle + 8) | ((uint32_t)(int32_t)(int16_t)xport_load_le16(rectangle + 14) << 16));
    w_u32(packet + 20, (uint32_t)(int32_t)(int16_t)xport_load_le16(rectangle + 12) | ((uint32_t)(int32_t)(int16_t)xport_load_le16(rectangle + 14) << 16));
    w_u32(link, packet);
    return 1;
}

uint32_t sub_8006ECDC(uint32_t menu, const uint8_t frame[34], uint32_t link)
{
    uint8_t rectangle[16] = {0};
    FUNCTION_MARKER(0x8006ECDC, "RASHCDF.BIN");
    xport_store_le16(rectangle + 2, xport_load_le16(frame + 2));
    xport_store_le32(rectangle + 4, xport_load_le32(frame + 4));
    xport_store_le16(rectangle + 8, xport_load_le16(frame + 8));
    xport_store_le16(rectangle + 12, xport_load_le16(frame + 12));
    xport_store_le16(rectangle + 10, xport_load_le16(frame + 10));
    xport_store_le16(rectangle + 14, xport_load_le16(frame + 26));
    (void)sub_8006EE20(rectangle, link);
    xport_store_le16(rectangle + 10, xport_load_le16(frame + 30));
    xport_store_le16(rectangle + 14, xport_load_le16(frame + 14));
    (void)sub_8006EE20(rectangle, link);
    xport_store_le16(rectangle + 10, xport_load_le16(frame + 26));
    xport_store_le16(rectangle + 14, xport_load_le16(frame + 30));
    xport_store_le16(rectangle + 8, xport_load_le16(frame + 8));
    xport_store_le16(rectangle + 12, xport_load_le16(frame + 24));
    (void)sub_8006EE20(rectangle, link);
    xport_store_le16(rectangle + 8, xport_load_le16(frame + 28));
    xport_store_le16(rectangle + 12, xport_load_le16(frame + 12));
    (void)sub_8006EE20(rectangle, link);
    if (!(xport_load_le16(frame + 32) & 1))
        (void)sub_8006EE20(frame + 16, link);
    return 1;
}

uint32_t sub_8006EDE4(uint32_t menu, uint32_t entry)
{
    FUNCTION_MARKER(0x8006EDE4, "RASHCDF.BIN");
    return sub_8006EE20(xport_guest_cptr(entry + 16, 16), r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 20);
}

uint32_t sub_80074554(uint32_t menu, uint32_t entry)
{
    uint32_t link, id, count = 0, item, amount, x, y;
    const uint32_t images[9] = {0x4E414843,0x42554C43,0x45504950,0x444F4F57,0x4B48434E,0x52414243,0x444F5250,0x4E555453,0x59525053};
    uint16_t label[4] = {30,69,107,130}, value[4] = {170,69,110,130};
    uint8_t image[12], halo[12], frame[34] = {0};
    char text[128];
    FUNCTION_MARKER(0x80074554, "RASHCDF.BIN");
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    w_u8(0x800D8078 + 24 * r_u32(0x8009C5B8) + 3, 0);
    w_u32(entry + 28, 0xFF00FF);
    id = (uint32_t)(int32_t)(int16_t)sub_80072A80(r_s8(0x800D81E1), r_s8(0x800D81E2));
    (void)sub_8002CB08(r_u32(0x8009C5B8), 218, label, link, 0x286EB4, 0);
    (void)sub_8002CB08(r_u32(0x8009C5B8), id, value, link, 0xB9A096, 0);
    id = (uint32_t)(int32_t)(int16_t)sub_80072A60(r_s8(0x800D81E1));
    label[1] += 14;
    value[1] += 14;
    (void)sub_8002CB08(r_u32(0x8009C5B8), 219, label, link, 0x286EB4, 0);
    (void)sub_8002CB08(r_u32(0x8009C5B8), id, value, link, 0xB9A096, 0);
    label[1] += 14;
    value[1] += 14;
    (void)sub_8002CB08(r_u32(0x8009C5B8), 220, label, link, 0x286EB4, 0);
    id = sub_80071F0C((uint32_t)r_s8(0x800D80DC), r_u32(0x800D80D8));
    (void)sub_8002CB08(r_u32(0x8009C5B8), id, value, link, 0xB9A096, 0);
    id = sub_80071FDC(r_s8(0x800D81DF));
    label[1] += 14;
    value[1] += 14;
    (void)sub_8002CB08(r_u32(0x8009C5B8), 221, label, link, 0x286EB4, 0);
    (void)sub_8002CB08(r_u32(0x8009C5B8), id, value, link, 0xB9A096, 0);
    label[1] += 14;
    value[1] += 14;
    (void)snprintf(text, sizeof(text), "%1d/9", r_u8(0x800D80E9));
    (void)sub_8002CB08(r_u32(0x8009C5B8), 222, label, link, 0x286EB4, 0);
    (void)sub_8002CC74(r_u32(0x8009C5B8), (const uint8_t *)text, value, link, 0xB9A096, 0);
    label[1] += 14;
    value[1] += 14;
    (void)sub_8002CB08(r_u32(0x8009C5B8), 1584, label, link, 0x286EB4, 0);
    label[0] = 200;
    value[0] = 170;
    (void)snprintf(text, sizeof(text), "%1d", r_u8(0x800D81EC));
    id = r_u8(0x800D81EC) == 1 ? 1585 : 1586;
    (void)sub_8002CB08(r_u32(0x8009C5B8), id, label, link, 0xB9A096, 0);
    (void)sub_8002CC74(r_u32(0x8009C5B8), (const uint8_t *)text, value, link, 0xB9A096, 0);
    label[0] = 295;
    value[0] = 265;
    for (item = 6; item > 0; --item)
        if ((r_u32(0x800D81E8) >> (4 * (item - 1))) & 15)
            ++count;
    (void)snprintf(text, sizeof(text), "%1d", count);
    (void)sub_8002CB08(r_u32(0x8009C5B8), count == 1 ? 1587 : 1588, label, link, 0xB9A096, 0);
    (void)sub_8002CC74(r_u32(0x8009C5B8), (const uint8_t *)text, value, link, 0xB9A096, 0);
    label[0] = 0;
    label[2] = 512;
    label[1] += 14;
    value[1] += 20;
    (void)sub_8002CB08(r_u32(0x8009C5B8), 223, label, link, 0x286EB4, 2);
    xport_store_le32(image + 4, 0x01808080);
    xport_store_le32(halo, 0x4F4C4148);
    xport_store_le32(halo + 4, 0x03808080);
    for (item = 0; item < 9; ++item)
    {
        x = 57 * item + 11;
        y = label[1] + 20;
        xport_store_le16(image + 8, x);
        xport_store_le16(image + 10, y);
        if ((r_u16(0x800D81E4) >> item) & 1)
        {
            xport_store_le32(image, images[item]);
            amount = item < 6 ? (r_u32(0x800D81E8) >> (4 * item)) & 15 : 0;
            if (amount)
            {
                xport_store_le16(halo + 8, x);
                xport_store_le16(halo + 10, y);
                value[0] = (uint16_t)(x + 36);
                value[1] = (uint16_t)(y + 24);
                (void)snprintf(text, sizeof(text), "%1d", amount);
                (void)sub_8002CC74(r_u32(0x8009C5B8), (const uint8_t *)text, value, link, 0xB9A096, 0);
                (void)sub_800700F0(menu, image, link);
                (void)sub_800700F0(menu, halo, link);
            }
            else
                (void)sub_800700F0(menu, image, link);
        }
        xport_store_le16(frame + 2, 0);
        xport_store_le32(frame + 4, 0x1E1414);
        xport_store_le16(frame + 8, x - 4);
        xport_store_le16(frame + 10, y - 2);
        xport_store_le16(frame + 12, x + 50);
        xport_store_le16(frame + 14, y + 34);
        xport_store_le16(frame + 18, 1);
        xport_store_le32(frame + 20, 0x1E1414);
        xport_store_le16(frame + 24, x);
        xport_store_le16(frame + 26, y);
        xport_store_le16(frame + 28, x + 46);
        xport_store_le16(frame + 30, y + 32);
        xport_store_le16(frame + 32, 1);
        (void)sub_8006ECDC(menu, frame, link);
    }
    return 1;
}

uint32_t sub_80075098(uint32_t menu, uint32_t entry)
{
    uint32_t link, packet_link, context, packet, replacement, x, y, width, column, row, color, item;
    uint16_t position[4] = {160,57,24,14}, key[4] = {148,85,24,14};
    uint8_t text[3];
    FUNCTION_MARKER(0x80075098, "RASHCDF.BIN");
    text[0] = r_u8(0x8005C5D4);
    text[1] = r_u8(0x8005C5D5);
    text[2] = r_u8(0x8005C5D6);
    w_u8(0x800D8078 + 24 * r_u32(0x8009C5B8) + 3, 0);
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    x = (uint16_t)(148 + 24 * r_u32(0x8009CCD4));
    y = (uint16_t)(85 + 14 * r_u32(0x8009CCD8));
    context = r_u32(0x8005B470);
    packet = r_u32(context + 268);
    if (packet + 48 >= r_u32(0x8005B4D0))
    {
        replacement = sub_80021C98(packet, 48);
        w_u32(r_u32(0x8005B470) + 268, replacement);
    }
    context = r_u32(0x8005B470);
    packet = r_u32(context + 268);
    w_u32(context + 268, packet + 48);
    packet_link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 20;
    w_u8(packet + 3, 5);
    w_u8(packet + 7, 40);
    if (r_u32(0x8009CCD8) == 4)
    {
        column = r_u32(0x8009CCD4);
        if (column >= 9)
            abort();
        if (column < 2) { x = 148; width = 36; }
        else if (column == 2) { x = 184; width = 36; }
        else if (column < 5) { x = 220; width = 48; }
        else if (column < 7) { x = 268; width = 48; }
        else { x = 316; width = 48; }
        x = (uint16_t)(x + 1);
        y = (uint16_t)(y - 2);
        w_u16(packet + 8, x);
        w_u16(packet + 10, y);
        w_u16(packet + 12, x + width);
        w_u16(packet + 14, y);
        w_u16(packet + 16, x);
        w_u16(packet + 18, y + 14);
        w_u16(packet + 20, x + width);
        w_u16(packet + 22, y + 14);
    }
    else
    {
        x = (uint16_t)(x + 1);
        y = (uint16_t)(y - 2);
        w_u16(packet + 8, x);
        w_u16(packet + 10, y);
        w_u16(packet + 12, x + 25);
        w_u16(packet + 14, y);
        w_u16(packet + 16, x);
        w_u16(packet + 18, y + 14);
        w_u16(packet + 20, x + 24);
        w_u16(packet + 22, y + 14);
    }
    w_u8(packet + 4, 128);
    w_u8(packet + 5, 128);
    w_u8(packet + 6, 255);
    w_u8(packet + 7, r_u8(packet + 7) | 2);
    w_u32(packet, (r_u32(packet) & 0xFF000000) | (r_u32(packet_link) & 0xFFFFFF));
    w_u32(packet_link, (r_u32(packet_link) & 0xFF000000) | (packet & 0xFFFFFF));
    packet += 24;
    x = 161 + 24 * r_u32(0x80088DF0);
    y = 54;
    w_u8(packet + 3, 5);
    w_u8(packet + 7, 40);
    w_u16(packet + 8, x);
    w_u16(packet + 10, y);
    w_u16(packet + 12, x + 24);
    w_u16(packet + 14, y);
    w_u16(packet + 16, x);
    w_u16(packet + 18, y + 14);
    w_u16(packet + 20, x + 24);
    w_u16(packet + 22, y + 14);
    w_u8(packet + 4, 128);
    w_u8(packet + 5, 128);
    w_u8(packet + 6, 255);
    w_u8(packet + 7, r_u8(packet + 7) | 2);
    w_u32(packet, (r_u32(packet) & 0xFF000000) | (r_u32(packet_link) & 0xFFFFFF));
    w_u32(packet_link, (r_u32(packet_link) & 0xFF000000) | (packet & 0xFFFFFF));
    for (column = 0; column < 8; ++column)
    {
        text[0] = r_u8(0x80088C84 + column);
        (void)sub_8002CC74(r_u32(0x8009C5B8), text, position, link, 0xB9A096, 2);
        position[0] = (uint16_t)(position[0] + 24);
    }
    for (row = 0; row < 4; ++row)
    {
        key[1] = (uint16_t)(85 + 14 * row);
        for (column = 0; column < 9; ++column)
        {
            key[0] = (uint16_t)(148 + 24 * column);
            text[0] = r_u8(0x80088C9C + 9 * row + column);
            color = column == r_u32(0x8009CCD4) && row == r_u32(0x8009CCD8) ? 0x40ACFF : 0x606060;
            (void)sub_8002CC74(r_u32(0x8009C5B8), text, key, link, color, 2);
        }
    }
    key[0] = 148;
    key[1] = 141;
    key[2] = 36;
    for (item = 0; item < 5; ++item)
    {
        if (item == 0)
            column = r_u32(0x8009CCD4) < 2;
        else if (item == 1)
            column = r_u32(0x8009CCD4) == 2;
        else
            column = r_u32(0x8009CCD4) - (2 * item - 1) < 2;
        color = column && r_u32(0x8009CCD8) == 4 ? (item == 4 ? 0xC4C4C4 : 0x40ACFF) : 0x606060;
        if (item < 2)
        {
            text[0] = r_u8(0x80088C9C + 36 + item);
            (void)sub_8002CC74(r_u32(0x8009C5B8), text, key, link, color, 2);
        }
        else
        {
            key[2] = 48;
            (void)sub_8002CB08(r_u32(0x8009C5B8), 231 + item, key, link, color, 2);
        }
        key[0] = (uint16_t)(key[0] + key[2]);
    }
    return 1;
}

uint32_t sub_800733BC(uint32_t menu, uint32_t entry)
{
    uint32_t rect = entry + 16, link, row, display, target, next;
    int32_t current, desired, step;
    uint16_t position[4];
    char text[128];
    FUNCTION_MARKER(0x800733BC, "RASHCDF.BIN");
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
    w_u8(0x800D8078 + 24 * r_u32(0x8009C5B8) + 3, 0);
    position[0] = (uint16_t)(r_u16(rect) + 5);
    position[1] = r_u16(rect + 2);
    position[2] = 42;
    position[3] = r_u16(rect + 6);
    for (row = 0; row < 3; ++row)
    {
        (void)sub_8002CB08(r_u32(0x8009C5B8), 1531 + row, position, link, r_u32(rect + 12), r_u16(rect + 10));
        if (row != 2)
            position[1] = (uint16_t)(position[1] + 14);
    }
    position[0] = (uint16_t)(r_u16(rect) + 160);
    position[1] = r_u16(rect + 2);
    for (row = 0; row < 3; ++row)
    {
        if (row == 1 && r_s16(0x8009C624) != r_s16(0x8009C640))
            continue;
        if (row == 2 && r_u32(0x8009C624) != r_u32(0x8009C640))
            return 1;
        if (row)
        {
            position[0] = (uint16_t)(r_u16(rect) + 160);
            position[1] = (uint16_t)(position[1] + 14);
        }
        display = 0x8009C624 + 2 * row;
        target = 0x8009C640 + 2 * row;
        current = r_s16(display);
        desired = r_s16(target);
        step = 0;
        if (row < 2)
        {
            if (current < desired - 90) step = 90;
            else if (current < desired - 15) step = 15;
            else if (current < desired) step = 1;
            else if (desired + 90 < current) step = -90;
            else if (desired < current) step = -1;
        }
        else
        {
            if (current < desired - 15) step = 15;
            else if (current < desired - 450) step = 450;
            else if (current < desired - 90) step = 90;
            else if (current < desired) step = 1;
            else if (desired + 450 < current) step = -450;
            else if (desired + 90 < current) step = -90;
            else if (desired < current) step = -1;
        }
        if (step)
        {
            w_u16(0x8009C632, r_u16(0x8009C632) + 1);
            if (r_s16(0x8009C632) > 0)
            {
                next = r_u16(display) + (uint32_t)step;
                w_u16(display, next);
                w_u16(0x8009C632, 0);
                (void)sub_8007EAC0((int16_t)next == desired ? 14 : 13);
            }
        }
        (void)snprintf(text, sizeof(text), "%d", r_s16(display));
        (void)sub_8002CC74(r_u32(0x8009C5B8), (const uint8_t *)text, position, link, 0x1C058E, 1);
    }
    if (r_u32(0x8009C624) != r_u32(0x8009C640) || r_s16(0x8009C628) != r_s16(0x8009C644) || !r_s16(0x8009C648))
        return 1;
    position[0] = (uint16_t)(r_u16(rect) + 5);
    position[1] = (uint16_t)(position[1] + 28);
    (void)sub_8002CB08(r_u32(0x8009C5B8), 1534, position, link, r_u32(rect + 12), r_u16(rect + 10));
    position[0] = (uint16_t)(r_u16(rect) + 114);
    (void)sub_8002CB08(r_u32(0x8009C5B8), (uint32_t)r_s16(0x8009C648), position, link, 0x1C058E, r_u16(rect + 10));
    if (r_s16(0x8009C64A))
    {
        position[0] = (uint16_t)(r_u16(rect) + 5);
        position[1] = (uint16_t)(position[1] - 14);
        (void)sub_8002CB08(r_u32(0x8009C5B8), 1534, position, link, r_u32(rect + 12), r_u16(rect + 10));
        position[0] = (uint16_t)(r_u16(rect) + 114);
        (void)sub_8002CB08(r_u32(0x8009C5B8), (uint32_t)r_s16(0x8009C64A), position, link, 0x1C058E, r_u16(rect + 10));
    }
    return 1;
}

uint32_t sub_80071064(uint32_t menu, uint32_t rectangle, int32_t id)
{
    uint32_t result = 1, link, row, count = 0, text, font;
    int32_t status, labels[3] = {-1,-1,-1};
    uint16_t position[4];
    uint8_t frame[34] = {0};
    FUNCTION_MARKER(0x80071064, "RASHCDF.BIN");
    position[0] = r_u16(rectangle);
    position[1] = r_u16(rectangle + 2);
    position[2] = r_u16(rectangle + 4);
    position[3] = r_u16(rectangle + 6);
    xport_store_le32(frame + 4, 0x1E1414);
    xport_store_le16(frame + 8, position[0] - 10);
    xport_store_le16(frame + 10, position[1] - 26);
    xport_store_le16(frame + 12, position[0] + position[2] + 10);
    xport_store_le16(frame + 14, position[1] + position[3] - 14);
    xport_store_le16(frame + 18, 1);
    xport_store_le32(frame + 20, 0x1E1414);
    xport_store_le16(frame + 24, position[0]);
    xport_store_le16(frame + 26, position[1] - 20);
    xport_store_le16(frame + 28, position[0] + position[2]);
    xport_store_le16(frame + 30, position[1] + position[3] - 20);
    link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 20;
    w_u8(0x800D8078 + 24 * r_u32(0x8009C5B8) + 3, 0);
    if (id == 0)
    {
        status = r_s16(r_u32(0x8009954C) + 10);
        if (status)
            (void)sub_80071064(menu, rectangle, status);
        return result;
    }
    if (!((id >= 1 && id <= 13) || (id >= 16 && id <= 23) || id == 25))
        return result;
    result = sub_8006ECDC(menu, frame, link);
    switch (id)
    {
        case 1:
            labels[0] = 149; labels[1] = 150; labels[2] = 151; count = 3;
            break;
        case 2:
            labels[0] = 153; labels[1] = 154; count = 2;
            break;
        case 3:
            status = r_s16(menu + 6);
            labels[0] = status == 45 ? 165 : status == 46 ? 169 : -1;
            labels[1] = 166; labels[2] = 151; count = 3;
            break;
        case 4:
        case 5:
        case 6:
        case 23:
            labels[0] = 159; labels[1] = 157; count = 2;
            break;
        case 7:
            labels[0] = 162; labels[1] = 157; count = 2;
            break;
        case 8:
            status = r_s16(menu + 6);
            labels[0] = status == 45 ? 164 : status == 46 ? 168 : -1;
            labels[1] = 157; count = 2;
            break;
        case 9:
            labels[0] = 156; labels[1] = 157; count = 2;
            break;
        case 10:
        case 16:
            labels[0] = 152; count = 1;
            break;
        case 11:
        case 17:
            labels[0] = 155; count = 1;
            break;
        case 12:
        case 18:
            labels[0] = 160; count = 1;
            break;
        case 13:
        case 19:
            status = r_s16(menu + 6);
            labels[0] = status == 45 ? 163 : status == 46 ? 167 : -1;
            count = 1;
            break;
        case 20:
            if ((uint32_t)(r_u16(0x8009C5D0) - 43) < 2)
            {
                font = r_u32(0x8009C5B8);
                link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
                (void)sub_8002CC74(font, xport_guest_cptr(0x8005C288, 1), position, link, r_u32(rectangle + 12), r_u16(rectangle + 10));
                font = r_u32(0x8009C5B8);
                position[1] = (uint16_t)(position[1] + r_u16(0x800D8078 + 24 * font + 20));
                text = 0x8005C2A0;
            }
            else
                text = 0x8005C2B4;
            font = r_u32(0x8009C5B8);
            link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
            (void)sub_8002CC74(font, xport_guest_cptr(text, 1), position, link, r_u32(rectangle + 12), r_u16(rectangle + 10));
            font = r_u32(0x8009C5B8);
            position[1] = (uint16_t)(position[1] + 2 * r_s16(0x800D8078 + 24 * font + 20));
            labels[0] = 157; count = 1;
            break;
        case 21:
            status = r_s16(r_u32(0x8009954C) + 10);
            if (status != 21)
            {
                (void)sub_80071064(menu, rectangle, status);
                return result;
            }
            labels[0] = 147; count = 1;
            break;
        case 22:
            labels[0] = 148; labels[1] = 157; count = 2;
            break;
        case 25:
            labels[0] = 1582; labels[1] = 1583; labels[2] = 157; count = 3;
            break;
    }
    for (row = 0; row < count; ++row)
    {
        if (labels[row] >= 0)
        {
            font = r_u32(0x8009C5B8);
            link = r_u32(0x8009CFC8) + 4 * (uint32_t)r_s8(0x8009C5E1) + 4;
            (void)sub_8002CB08(font, (uint32_t)labels[row], position, link, r_u32(rectangle + 12), r_u16(rectangle + 10));
        }
        if (row + 1 < count)
        {
            font = r_u32(0x8009C5B8);
            position[1] = (uint16_t)(position[1] + 2 * r_s16(0x800D8078 + 24 * font + 20));
        }
    }
    return result;
}
