#include "rrj_debug.h"
#include "menu.h"
#include "game_main.h"
#include <stdio.h>
#include <stdlib.h>
#include "native_game_api.h"
#include "race_pause.h"
#include "race_bodyless_batch_002.h"
#include "psx.h"
#include "xport.h"
#include "screen.h"
#include "menu_loop.h"
#include "resource.h"
#include "resource_apply.h"
#include "resource_next.h"
#include "resource_value.h"
#include "solo_resource_dependencies.h"
#include "draft_quick.h"
#include "resource_groups.h"
#include "menu_transition.h"
#include "race_bodyless_batch_004.h"
#include "video_stop.h"
#include "pad.h"
#include "vblank.h"
#include "gpu.h"
#include "race_leaf.h"

uint32_t sub_8007F20C(uint32_t slot, int32_t value)
{
    uint32_t product;
    int32_t scaled;
    uint32_t clipped;
    uint32_t upper_difference;

    FUNCTION_MARKER(0x8007F20C, "RASHCDF.BIN");
    product = (uint32_t)value * r_u32(0x80052638 + 4 * slot);
    product = (uint32_t)((int32_t)product >> 4) * r_u32(0x80052650);
    scaled = (int32_t)product >> 7;
    if (slot == 5 && scaled < 0)
        scaled = r_s32(0x800D6C14);
    upper_difference = 128 - (uint32_t)scaled;
    clipped = (~(uint32_t)(scaled >> 31) & (uint32_t)scaled) + ((uint32_t)((int32_t)upper_difference >> 31) & upper_difference);
    w_u32(0x800D6C00 + 4 * slot, clipped);
    w_u32(0x800D6C20 + 4 * slot, clipped);
    return 0x800D6C20;
}

uint32_t sub_8007FDA8(void)
{
    uint32_t index;
    uint32_t value;

    FUNCTION_MARKER(0x8007FDA8, "RASHCDF.BIN");
    w_u32(0x800D80D8, 32);
    w_u8(0x800D80DE, 1);
    w_u8(0x800D80DC, 0);
    w_u8(0x800D80E1, 1);
    w_u8(0x800D80E2, 1);
    w_u8(0x800D80E3, 1);
    w_u8(0x800D80EC, 1);
    w_u8(0x800D80EF, 0);
    w_u8(0x800D80F1, 1);
    w_u8(0x800D80F2, 1);
    w_u8(0x800D80F3, 0);
    w_u8(0x800D80F4, 0);
    w_u8(0x800D80F0, 0);
    w_u8(0x800D80EA, 1);
    w_u8(0x800D80F5, 0);
    for (index = 0; index < 6; ++index)
    {
        w_u8(0x800D81EF + 36 * index, 1);
        w_u8(0x800D81E0 + 36 * index, 0);
    }
    for (index = 0; index < 7; ++index)
    {
        value = r_u32(0x80088BDC + 16 * index);
        w_u32(0x800D81AC + 4 * index, value);
        (void)sub_8007F20C(index, (int32_t)value);
    }
    for (index = 18; index != 0; --index)
        w_u8(0x800D8198 + index - 1, 1);
    value = sub_8001FC58() % 6 + 32;
    w_u8(0x800D80E7, value);
    return value;
}

uint32_t sub_800806D0(void)
{
    static const struct
    {
        uint32_t offset;
        uint32_t menu;
    } entries[] = {
        {0, 0x80099064},
        {4, 0x80099078},
        {8, 0x8009908C},
        {12, 0x800990A0},
        {16, 0x800990B4},
        {20, 0x800990C8},
        {28, 0x800990DC},
        {32, 0x800990F0},
        {36, 0x80099104},
        {96, 0x80099118},
        {100, 0x8009912C},
        {204, 0x800993E8},
        {156, 0x80099140},
        {108, 0x80099154},
        {116, 0x80099168},
        {120, 0x8009917C},
        {124, 0x80099190},
        {128, 0x800991A4},
        {132, 0x800991B8},
        {144, 0x800991CC},
        {136, 0x800991E0},
        {140, 0x800991F4},
        {152, 0x80099208},
        {160, 0x8009921C},
        {232, 0x800992E4},
        {24, 0x800992F8},
        {40, 0x8009926C},
        {52, 0x80099280},
        {56, 0x80099294},
        {68, 0x800992A8},
        {72, 0x800992BC},
        {92, 0x800992D0},
        {48, 0x8009930C},
        {44, 0x80099320},
        {64, 0x80099334},
        {60, 0x80099348},
        {80, 0x8009935C},
        {84, 0x80099370},
        {88, 0x80099384},
        {76, 0x80099398},
        {112, 0x800993AC},
        {148, 0x800993C0},
        {168, 0x800993D4},
        {200, 0x80099230},
        {192, 0x80099258},
        {188, 0x80099244},
        {172, 0x800993FC},
        {176, 0x80099410},
        {180, 0x80099424},
        {184, 0x80099438},
        {196, 0x8009944C},
        {104, 0x80099460},
        {164, 0x80099474},
        {212, 0x80099488},
        {216, 0x8009949C},
        {220, 0x800994B0},
        {208, 0x800994C4},
        {224, 0x800994D8},
        {228, 0x800994EC},
    };
    uint32_t index;

    FUNCTION_MARKER(0x800806D0, "RASHCDF.BIN");
    w_u32(0x8009C68C, 0x800A0880);
    for (index = 0; index < sizeof(entries) / sizeof(entries[0]); ++index)
        w_u32(0x800A0880 + entries[index].offset, entries[index].menu);
    return 0x800994EC;
}

uint32_t sub_800809A8(uint32_t id)
{
    uint32_t slot;
    uint32_t menu;

    FUNCTION_MARKER(0x800809A8, "RASHCDF.BIN");
    slot = r_u32(0x8009C68C) + 4 * id;
    menu = r_u32(slot);
    w_u16(menu, r_u16(menu) | 3);
    menu = r_u32(slot);
    w_u16(menu + 2, 0x7FFF);
    return 0x7FFF;
}

uint32_t sub_800809E0(int16_t id)
{
    uint32_t table;
    uint32_t index;
    uint32_t menu;
    uint32_t slot;

    FUNCTION_MARKER(0x800809E0, "RASHCDF.BIN");
    table = r_u32(0x8009C68C);
    for (index = 0; index < 59; ++index)
    {
        slot = table + 4 * index;
        menu = r_u32(slot);
        w_u16(menu, r_u16(menu) & 0xFF00);
        menu = r_u32(slot);
        w_u16(menu + 2, 0);
    }
    w_u16(0x8009C5D0, (uint16_t)id);
    w_u16(0x8009C5D2, (uint16_t)id);
    menu = r_u32(r_u32(0x8009C68C) + 4 * (uint32_t)(int32_t)id);
    w_u32(0x8009C5C8, menu);
    return sub_800809A8((uint32_t)(int32_t)id);
}

uint32_t sub_80080CC8(void)
{
    FUNCTION_MARKER(0x80080CC8, "RASHCDF.BIN");
    (void)sub_8001E100(0x800D81D8, 0, 216);
    return sub_8001E100(0x800D80D8, 0, 256);
}

uint32_t sub_80065B10(void)
{
    uint32_t index;
    uint32_t state;
    uint32_t descriptor;
    uint32_t flags;
    uint32_t value;

    FUNCTION_MARKER(0x80065B10, "RASHCDF.BIN");
    if (!r_u32(0x8005AE74))
    {
        w_u32(0x8005AE74, 1);
        for (index = 0; index < 4; ++index)
        {
            w_u8(0x800D8078 + 24 * index, 0);
            w_u32(0x800D807C + 24 * index, 0);
        }
        w_u32(0x8005B554, 0);
        w_u32(0x8005B540, 0);
        w_u16(0x8005B54C, 0);
        w_u32(0x8005B544, 0);
        w_u32(0x8005AE78, 0);
        state = r_u32(0x8005B2F8);
        descriptor = 0x800533B4 + 88 * (r_u32(state + 48) - 1);
        flags = r_u8(descriptor + 78);
        index = r_u32(state + 48) - 1;
        w_u16(0x8005B54E, (flags & 15) << 6);
        descriptor = 0x800533B4 + 88 * index;
        flags = r_u8(descriptor + 78);
        value = r_u8(descriptor + 77);
        w_u16(0x8005B548, 960);
        w_u16(0x8005B54A, 256);
        w_u16(0x8005B550, value + ((flags & 16) << 4));
    }
    w_u32(0x8009C5C0, 0xFFFFFFFF);
    w_u32(0x8009C5BC, 0xFFFFFFFF);
    w_u32(0x8009C5B8, 0xFFFFFFFF);
    return 0x8009C5B4;
}

uint32_t sub_80063720(void)
{
    uint32_t index;

    FUNCTION_MARKER(0x80063720, "RASHCDF.BIN");
    for (index = 37; index != 0; --index)
        w_u32(0x8009C4B0 + 4 * (index - 1), 0);
    w_u32(0x8009C4B0, 0x800812D0);
    w_u32(0x8009C4B4, 0x80081794);
    w_u32(0x8009C4B8, 0x800817C0);
    w_u32(0x8009C4BC, 0x80086064);
    w_u32(0x8009C4C0, 0x80086C90);
    w_u32(0x8009C4C4, 0x80086C98);
    w_u32(0x8009C4C8, 0x80087098);
    w_u32(0x8009C4D4, 0x80086D80);
    w_u32(0x8009C4DC, 0x800884B0);
    w_u32(0x8009C4E0, 0x800884B8);
    w_u32(0x8009C4E4, 0x800884C0);
    w_u32(0x8009C540, 0x800884C8);
    w_u32(0x8009C4D0, 0x80086FF8);
    w_u32(0x8009C4D8, 0x8008706C);
    w_u32(0x8009C4CC, 0x80086D30);
    w_u32(0x8009C4E8, 0x80087F40);
    w_u32(0x8009C4EC, 0x80088020);
    w_u32(0x8009C4F0, 0x800880B8);
    w_u32(0x8009C4F4, 0x800880C0);
    w_u32(0x8009C4F8, 0x800881A0);
    w_u32(0x8009C4FC, 0x80088310);
    w_u32(0x8009C500, 0x80088320);
    w_u32(0x8009C504, 0x80088318);
    w_u32(0x8009C508, 0x80088460);
    w_u32(0x8009C50C, 0x80088370);
    w_u32(0x8009C510, 0x80088378);
    w_u32(0x8009C514, 0x80088380);
    w_u32(0x8009C518, 0x80088480);
    w_u32(0x8009C51C, 0x80088488);
    w_u32(0x8009C520, 0x800884F8);
    w_u32(0x8009C524, 0x800884F0);
    w_u32(0x8009C528, 0x800884E8);
    w_u32(0x8009C530, 0x80088500);
    w_u32(0x8009C52C, 0x80088BC8);
    w_u32(0x8009C534, 0x80086E28);
    w_u32(0x8009C538, 0x80086F84);
    w_u32(0x8009C53C, 0x800884A8);
    return 0x800884A8;
}

uint32_t sub_80068E88(void)
{
    uint32_t index;

    FUNCTION_MARKER(0x80068E88, "RASHCDF.BIN");
    for (index = 0; index < 81; ++index)
        w_u32(0x8009CC38 - 4 * index, 0xFFFFFFFF);
    w_u32(0x8009CB00, 5);
    w_u32(0x8009CB04, 29);
    w_u32(0x8009CB08, 42);
    w_u32(0x8009CB0C, 6);
    w_u32(0x8009CB14, 24);
    w_u32(0x8009CB10, 27);
    w_u32(0x8009CB64, 30);
    w_u32(0x8009CB68, 32);
    w_u32(0x8009CB6C, 36);
    w_u32(0x8009CB70, 34);
    w_u32(0x8009CB74, 38);
    w_u32(0x8009CBF4, 43);
    w_u32(0x8009CBF8, 44);
    w_u32(0x8009CBFC, 45);
    w_u32(0x8009CC00, 46);
    w_u32(0x8009CC10, 50);
    w_u32(0x8009CC0C, 47);
    w_u32(0x8009CC08, 48);
    w_u32(0x8009CC04, 49);
    w_u32(0x8009CC14, 51);
    w_u32(0x8009CB18, 8);
    w_u32(0x8009CB30, 9);
    w_u32(0x8009CB48, 25);
    w_u32(0x8009CBE8, 39);
    w_u32(0x8009CB78, 31);
    w_u32(0x8009CBD4, 40);
    w_u32(0x8009CB24, 57);
    w_u32(0x8009CB5C, 57);
    w_u32(0x8009CB34, 57);
    w_u32(0x8009CB44, 26);
    w_u32(0x8009CB88, 57);
    w_u32(0x8009CBEC, 57);
    w_u32(0x8009CBE4, 26);
    w_u32(0x8009CB90, 33);
    w_u32(0x8009CBA0, 57);
    w_u32(0x8009CBA8, 57);
    w_u32(0x8009CBB4, 35);
    w_u32(0x8009CBC8, 57);
    for (index = 0; index < 81; ++index)
        w_u32(0x8009CAF0 - 4 * index, 0xFFFFFFFF);
    w_u32(0x8009C9D4, 8);
    w_u32(0x8009C9D8, 8);
    w_u32(0x8009C9E0, 11);
    w_u32(0x8009C9E4, 11);
    w_u32(0x8009CA18, 25);
    w_u32(0x8009C9F0, 15);
    w_u32(0x8009C9F4, 15);
    w_u32(0x8009C9F8, 15);
    w_u32(0x8009CA34, 32);
    w_u32(0x8009CA38, 32);
    w_u32(0x8009CA3C, 32);
    w_u32(0x8009CA44, 36);
    w_u32(0x8009CA4C, 38);
    w_u32(0x8009CA50, 38);
    w_u32(0x8009CA54, 38);
    w_u32(0x8009CA5C, 42);
    w_u32(0x8009CA64, 44);
    w_u32(0x8009CA68, 44);
    w_u32(0x8009CA70, 47);
    w_u32(0x8009CA74, 47);
    w_u32(0x8009CA78, 47);
    w_u32(0x8009CA7C, 47);
    w_u32(0x8009CA84, 52);
    w_u32(0x8009CA88, 52);
    w_u32(0x8009CA90, 55);
    w_u32(0x8009CA94, 55);
    w_u32(0x8009CA98, 55);
    w_u32(0x8009CAA8, 61);
    for (index = 0; index < 6; ++index)
        w_u32(0x8009C8BC - 4 * index, 0xFFFFFFFF);
    for (index = 0; index < 59; ++index)
        w_u32(0x8009C7B0 - 4 * index, 0xFFFFFFFF);
    w_u32(0x8009C6C8, 1);
    w_u32(0x8009C6CC, 3);
    w_u32(0x8009C6D4, 2);
    w_u32(0x8009C6E0, 7);
    w_u32(0x8009C6F8, 13);
    w_u32(0x8009C708, 17);
    w_u32(0x8009C718, 21);
    w_u32(0x8009C71C, 22);
    w_u32(0x8009C6F4, 10);
    w_u32(0x8009C704, 10);
    w_u32(0x8009C714, 10);
    w_u32(0x8009C738, 27);
    w_u32(0x8009C75C, 36);
    w_u32(0x8009C764, 38);
    w_u32(0x8009C6D0, 4);
    w_u32(0x8009C720, 4);
    w_u32(0x8009C6F0, 8);
    w_u32(0x8009C6FC, 8);
    w_u32(0x8009C700, 8);
    w_u32(0x8009C70C, 8);
    w_u32(0x8009C710, 8);
    w_u32(0x8009C724, 8);
    w_u32(0x8009C794, 42);
    w_u32(0x8009C790, 42);
    w_u32(0x8009C788, 42);
    w_u32(0x8009C784, 42);
    w_u32(0x8009C78C, 42);
    w_u32(0x8009C72C, 24);
    w_u32(0x8009C730, 24);
    w_u32(0x8009C76C, 40);
    w_u32(0x8009C6EC, 8);
    w_u32(0x8009C7A8, 42);
    w_u32(0x8009C7B0, 4);
    for (index = 0; index < 59; ++index)
        w_u32(0x8009C8A0 - 4 * index, 0xFFFFFFFF);
    w_u32(0x8009C7D0, 5);
    w_u32(0x8009C7D4, 5);
    w_u32(0x8009C7D8, 5);
    w_u32(0x8009C818, 5);
    w_u32(0x8009C824, 5);
    w_u32(0x8009C834, 30);
    w_u32(0x8009C844, 34);
    w_u32(0x8009C830, 29);
    w_u32(0x8009C848, 29);
    w_u32(0x8009C840, 29);
    w_u32(0x8009C838, 29);
    w_u32(0x8009C850, 29);
    w_u32(0x8009C83C, 32);
    w_u32(0x8009C854, 38);
    w_u32(0x8009C858, 38);
    w_u32(0x8009C7DC, 8);
    w_u32(0x8009C7CC, 4);
    w_u32(0x8009C82C, 4);
    w_u32(0x8009C860, 4);
    w_u32(0x8009C81C, 24);
    w_u32(0x8009C864, 42);
    w_u32(0x8009C868, 42);
    w_u32(0x8009C86C, 42);
    w_u32(0x8009C870, 42);
    w_u32(0x8009C884, 42);
    w_u32(0x8009C880, 42);
    w_u32(0x8009C874, 42);
    w_u32(0x8009C878, 42);
    w_u32(0x8009C87C, 42);
    w_u32(0x8009C820, 24);
    w_u32(0x8009C85C, 40);
    w_u32(0x8009C898, 42);
    w_u32(0x8009C8A0, 4);
    for (index = 0; index < 59; ++index)
        w_u32(0x8009C9A8 - 4 * index, 0x80069418);
    w_u32(0x8009C8C0, 0x8006AAF8);
    w_u32(0x8009C8CC, 0x8006AC80);
    w_u32(0x8009C968, 0x8006AE6C);
    w_u32(0x8009C994, 0x8006AD20);
    w_u32(0x8009C998, 0x8006AB58);
    w_u32(0x8009C99C, 0x8006AB58);
    w_u32(0x8009C96C, 0x8006C06C);
    w_u32(0x8009C970, 0x8006C124);
    w_u32(0x8009C974, 0x8006C1DC);
    w_u32(0x8009C978, 0x8006C290);
    w_u32(0x8009C984, 0x8006A390);
    w_u32(0x8009C98C, 0x8006A518);
    w_u32(0x8009C980, 0x80069FF0);
    w_u32(0x8009C97C, 0x8006A610);
    w_u32(0x8009C988, 0x8006A838);
    w_u32(0x8009C914, 0x8006A838);
    w_u32(0x8009C990, 0x80069A8C);
    w_u32(0x8009C9A0, 0x80069618);
    w_u32(0x8009C928, 0x80069560);
    w_u32(0x8009C9A4, 0x8006C354);
    w_u32(0x8009C8C4, 0x8006A8FC);
    w_u32(0x8009C8C8, 0x8006A8FC);
    w_u32(0x8009C8D8, 0x8006A8FC);
    w_u32(0x8009C8F0, 0x8006A8FC);
    w_u32(0x8009C900, 0x8006A8FC);
    w_u32(0x8009C910, 0x8006A8FC);
    w_u32(0x8009C918, 0x8006A8FC);
    w_u32(0x8009C8EC, 0x8006A8FC);
    w_u32(0x8009C8FC, 0x8006A8FC);
    w_u32(0x8009C90C, 0x8006A8FC);
    w_u32(0x8009C924, 0x80069440);
    w_u32(0x8009C95C, 0x80069440);
    w_u32(0x8009C9A8, 0x8006A8FC);
    for (index = 0; index < 37; ++index)
        w_u32(0x8009CCD0 - 4 * index, 0x8006B7BC);
    w_u32(0x8009CCBC, 0x8006BE84);
    return 0x8006BE84;
}

uint32_t sub_8006D174(void)
{
    uint32_t index;

    FUNCTION_MARKER(0x8006D174, "RASHCDF.BIN");
    for (index = 0; index < 59; ++index)
    {
        w_u32(0x8009D0C0 + 4 * index, 0x8006D5B0);
        w_u32(0x8009CFD0 + 4 * index, 0x8006D630);
    }
    w_u32(0x8009D0D8, 0x8006DB5C);
    w_u32(0x8009CFE8, 0x8006DE5C);
    w_u32(0x8009D0C0, 0x8006DB5C);
    w_u32(0x8009CFD0, 0x8006DE5C);
    w_u32(0x8009D0C4, 0x8006DB5C);
    w_u32(0x8009CFD4, 0x8006DE5C);
    w_u32(0x8009D0C8, 0x8006DB5C);
    w_u32(0x8009CFD8, 0x8006DE5C);
    w_u32(0x8009D0F0, 0x8006DB5C);
    w_u32(0x8009D000, 0x8006DE5C);
    w_u32(0x8009D100, 0x8006DB5C);
    w_u32(0x8009D010, 0x8006DE5C);
    w_u32(0x8009D110, 0x8006DB5C);
    w_u32(0x8009D020, 0x8006DE5C);
    w_u32(0x8009D118, 0x8006DB5C);
    w_u32(0x8009D028, 0x8006DE5C);
    w_u32(0x8009D0EC, 0x8006DB5C);
    w_u32(0x8009CFFC, 0x8006DE5C);
    w_u32(0x8009D0FC, 0x8006DB5C);
    w_u32(0x8009D00C, 0x8006DE5C);
    w_u32(0x8009D10C, 0x8006DB5C);
    w_u32(0x8009D01C, 0x8006DE5C);
    w_u32(0x8009D1A8, 0x8006DB5C);
    w_u32(0x8009D0B8, 0x8006DE5C);
    w_u32(0x8009D1A4, 0x8006E008);
    w_u32(0x8009D0B4, 0x8006E008);
    w_u32(0x8009D18C, 0x8006D8B4);
    w_u32(0x8009D09C, 0x8006D658);
    w_u32(0x8009D124, 0x8006D8B4);
    w_u32(0x8009D034, 0x8006D658);
    w_u32(0x8009D15C, 0x8006D8B4);
    w_u32(0x8009D06C, 0x8006D658);
    w_u32(0x8009D194, 0x8006D8B4);
    w_u32(0x8009D0A4, 0x8006D658);
    w_u32(0x8009D198, 0x8006D8B4);
    w_u32(0x8009D0A8, 0x8006D658);
    w_u32(0x8009D19C, 0x8006D8B4);
    w_u32(0x8009D0AC, 0x8006D658);
    for (index = 0; index < 20; ++index)
    {
        w_u32(0x8009CF78 + 4 * index, 0);
        w_u32(0x8009CF28 + 4 * index, 0);
    }
    w_u32(0x8009CF7C, 0x8006E3BC);
    w_u32(0x8009CF2C, 0x8006E400);
    w_u32(0x8009CF80, 0x8006E4D8);
    w_u32(0x8009CF30, 0x8006E4D8);
    w_u32(0x8009CF84, 0x8006E4D8);
    w_u32(0x8009CF34, 0x8006E4D8);
    w_u32(0x8009CF8C, 0x8006E4D8);
    w_u32(0x8009CF3C, 0x8006E4D8);
    w_u32(0x8009CF88, 0x8006E4D8);
    w_u32(0x8009CF38, 0x8006E4D8);
    w_u32(0x8009CF90, 0x8006E4D8);
    w_u32(0x8009CF40, 0x8006E4D8);
    w_u32(0x8009CF94, 0x8006E894);
    w_u32(0x8009CF44, 0x8006E894);
    w_u32(0x8009CF98, 0x8006E8FC);
    w_u32(0x8009CF48, 0x8006E8FC);
    w_u32(0x8009CF9C, 0x8006E7A4);
    w_u32(0x8009CF4C, 0x8006E7A4);
    w_u32(0x8009CFA0, 0x8006EDE4);
    w_u32(0x8009CF50, 0x8006EDE4);
    w_u32(0x8009CFA4, 0x8006ECA0);
    w_u32(0x8009CF54, 0x8006ECA0);
    w_u32(0x8009CFA8, 0x8006F764);
    w_u32(0x8009CF58, 0x8006F764);
    w_u32(0x8009CFAC, 0x8006EF30);
    w_u32(0x8009CF5C, 0x8006EF30);
    w_u32(0x8009CFB0, 0x8006F9A4);
    w_u32(0x8009CF60, 0x8006F9A4);
    w_u32(0x8009CFB4, 0x8006FA38);
    w_u32(0x8009CF64, 0x8006FA38);
    w_u32(0x8009CFB8, 0x8006FAC8);
    w_u32(0x8009CF68, 0x8006FAC8);
    w_u32(0x8009CFBC, 0x8006F4A0);
    w_u32(0x8009CF6C, 0x8006F4A0);
    w_u32(0x8009CFC0, 0x8006F0B8);
    w_u32(0x8009CF70, 0x8006F0B8);
    w_u32(0x8009CFC4, 0x80070580);
    w_u32(0x8009CF74, 0x80070580);
    return 0x80070580;
}

uint32_t sub_800665E8(void)
{
    uint32_t random;
    uint32_t attempt;
    uint32_t index;
    uint32_t first;
    uint32_t second;

    FUNCTION_MARKER(0x800665E8, "RASHCDF.BIN");
    w_u16(0x8009C5D0, 0);
    w_u32(0x8005ACAC, 0);
    w_u32(0x80088C50, 0);
    w_u8(0x8009C5EF, 56);
    w_u16(0x8009C5D2, 0);
    w_u16(0x8009C5DA, 0);
    w_u8(0x8009C5E3, 0);
    w_u8(0x8009C5E1, 0);
    w_u8(0x8009C5E2, 0);
    w_u32(0x8009C674, 0);
    w_u32(0x8009C678, 0);
    w_u32(0x8009C654, 0);
    w_u32(0x8009C658, 0);
    w_u32(0x8009C664, 0);
    w_u8(0x8009C5E6, 0);
    w_u32(0x8009C684, 0xFFFFFFFF);
    random = sub_80043F00(0xF2000002) & 255;
    sub_8001FC84((3305 * random) >> 6);
    for (attempt = 0; attempt < 200; ++attempt)
    {
        random = sub_8001FC58() % 18;
        w_u8(0x8009C5DE, random);
        if (r_s8(0x800D8198 + random))
            break;
    }
    w_u8(0x8009C5EA, 0xFF);
    w_u8(0x8009C5EB, 0xFF);
    w_u32(0x8009C67C, 300);
    w_u32(0x8009C680, 300);
    w_u8(0x8009C5DF, 0);
    w_u8(0x8009C5E0, 0);
    w_u8(0x8009C5E5, 0);
    w_u8(0x8009C5EE, 0);
    w_u32(0x8009C5F8, 0);
    w_u32(0x8009C5FC, 0);
    w_u16(0x8009C5EC, 0);
    for (index = 0; index < 22; ++index)
        w_u16(0x8009C614 + 2 * index, 0);
    w_u8(0x8009C646, 0);
    first = r_u32(0x80010E3C);
    second = r_u32(0x80010E40);
    w_u32(0x8009C600, first);
    w_u32(0x8009C604, second);
    for (index = 0; index < 3; ++index)
        w_u32(0x8009C610 - 4 * index, 0);
    (void)sub_80068E88();
    return sub_8006D174();
}

uint32_t sub_800805C8(void)
{
    uint32_t context;
    uint32_t ordering_table;

    FUNCTION_MARKER(0x800805C8, "RASHCDF.BIN");
    w_u8(r_u32(0x8005B470) + 5, 0);
    w_u8(r_u32(0x8005B470) + 6, 1);
    w_u8(r_u32(0x8005B470) + 7, 0);
    w_u8(r_u32(0x8005B470) + 4, 1);
    context = r_u32(0x8005B470);
    w_u32(context + 268, r_u32(context + 240) & 0xFFFFFF);
    (void)sub_80080D08();
    (void)sub_8001BE08(0, 0, 512, 240);
    (void)sub_8001BF1C(1, 0, 0, 0);
    (void)sub_8001C3F4();
    (void)sub_8001C408();
    sub_80064BE4();
    SetDispMask(1);
    (void)sub_8001BDEC();
    w_u32(0x800A0B00, 0);
    w_u32(0x800A0B04, 0);
    w_u32(0x8009CFC8, 0x8009CCE8);
    (void)rrj_gpu_clear_ot(0x8009CCE8, 72);
    ordering_table = 0x8009D1B0 + 288 * r_u32(0x800A0B04);
    w_u32(0x8009CCE4, ordering_table);
    return rrj_gpu_clear_ot(ordering_table, 72);
}

uint32_t sub_8007A5EC(void)
{
    FUNCTION_MARKER(0x8007A5EC, "RASHCDF.BIN");
    return sub_8001E0DC(0x800A0810, 0, 32);
}

uint32_t sub_80062438(void)
{
    FUNCTION_MARKER(0x80062438, "RASHCDF.BIN");
    return 308;
}

uint32_t sub_80060D0C(void)
{
    FUNCTION_MARKER(0x80060D0C, "RASHCDF.BIN");
    return 31616;
}

uint32_t sub_800621E0(uint32_t sectors, uint32_t buffer_bytes, uint32_t descriptors)
{
    FUNCTION_MARKER(0x800621E0, "RASHCDF.BIN");
    w_u32(0x8009C2E0, sectors);
    w_u32(0x8009C2E4, buffer_bytes);
    w_u32(0x8009C2E8, descriptors);
    return buffer_bytes + (sectors << 11) + 1404 + 56 * descriptors;
}

uint32_t sub_80076228(void)
{
    uint32_t index;
    uint32_t descriptor;

    FUNCTION_MARKER(0x80076228, "RASHCDF.BIN");
    (void)sub_8001E100(0x8009DDE0, 0, 10800);
    w_u32(0x8009D4DC, 0);
    for (index = 0; index < 100; ++index)
        w_u32(0x800A0AFC - 4 * index, 0);
    for (index = 0; index < 59; ++index)
        w_u32(0x8009D4D8 - 4 * index, 0);
    w_u32(0x8009D3FC, 0x80089CBC);
    w_u32(0x8009D40C, 0x80089CA0);
    w_u32(0x8009D410, 0x80089CA4);
    w_u32(0x8009D45C, 0x80089CC0);
    w_u32(0x8009D450, 0x80089CA8);
    w_u32(0x8009D488, 0x80089CA8);
    w_u32(0x8009D480, 0x80089CAC);
    w_u32(0x8009D468, 0x80089CB0);
    w_u32(0x8009D470, 0x80089CB4);
    w_u32(0x8009D478, 0x80089CB8);
    w_u32(0x8009D4B4, 0x80089CC4);
    w_u32(0x8009D418, 0x80089CC8);
    w_u32(0x8009D424, 0x80089CC8);
    w_u32(0x8009D428, 0x80089CC8);
    w_u32(0x8009D434, 0x80089CC8);
    w_u32(0x8009D438, 0x80089CC8);
    w_u32(0x8009D44C, 0x80089CC8);
    w_u32(0x8009D460, 0x80089CC8);
    w_u32(0x8009D484, 0x80089CC8);
    for (index = 0; index < 37; ++index)
    {
        descriptor = 0x800897E4 + 32 * index;
        w_u16(descriptor, r_u16(descriptor) & 0xFFF1);
    }
    return 0;
}

uint32_t sub_80080544(void)
{
    FUNCTION_MARKER(0x80080544, "RASHCDF.BIN");
    w_u32(r_u32(0x8005B2F8) + 52, 4);
    (void)sub_800805C8();
    (void)sub_8007A5EC();
    (void)sub_80065B10();
    (void)sub_80063720();
    (void)sub_8001B6A8();
    (void)sub_8001C4A8(0x800D6DE0, 0);
    (void)sub_8001C4A8(0x800D6EA0, 0);
    (void)sub_8005FABC(1);
    w_u32(0x8009C2F0, 0);
    w_u32(0x8009C2F4, 0);
    return sub_80076228();
}

uint32_t sub_80080C50(void)
{
    uint32_t index;
    uint32_t menu;
    uint32_t selection;

    FUNCTION_MARKER(0x80080C50, "RASHCDF.BIN");
    for (index = 0; index < 59; ++index)
    {
        menu = r_u32(r_u32(0x8009C68C) + 4 * index);
        w_u32(0x8009C5C8, menu);
        if (menu)
        {
            selection = sub_80080A70(menu);
            menu = r_u32(0x8009C5C8);
            w_u16(menu + 4, selection);
        }
    }
    return 0;
}

uint32_t sub_80063BFC(void)
{
    uint32_t index;
    uint32_t slot;
    uint32_t resource;
    uint32_t selection;

    FUNCTION_MARKER(0x80063BFC, "RASHCDF.BIN");
    for (index = 0; index < 37; ++index)
    {
        slot = 0x8009C4B0 + 4 * index;
        resource = r_u32(slot);
        if (resource && r_u32(resource + 4))
        {
            selection = sub_80062D6C(resource);
            resource = r_u32(slot);
            w_u8(resource + 3, selection);
            sub_8006310C(r_u32(slot));
        }
    }
    return 0;
}

uint32_t sub_80080C04(void)
{
    uint8_t resource_id;
    uint32_t result;

    FUNCTION_MARKER(0x80080C04, "RASHCDF.BIN");
    resource_id = r_u8(0x8009C5DE);
    (void)sub_80080C50();
    (void)sub_80064254(0);
    result = sub_80063BFC();
    w_u8(0x8009C5DE, resource_id);
    return result;
}

uint32_t sub_80065C38(void)
{
    uint32_t index;

    FUNCTION_MARKER(0x80065C38, "RASHCDF.BIN");
    if (r_u32(0x8005AE74))
    {
        for (index = 0; index < 4; ++index)
            (void)sub_8002C9D4(index);
        w_u32(0x8005B554, 0);
        w_u32(0x8005B540, 0);
        w_u32(0x8005B544, 0);
        w_u16(0x8005B54C, 0);
    }
    sub_8002C9A8();
    w_u32(0x8005AE74, 0);
    return 0x80060000;
}

uint32_t sub_80078EF4(void)
{
    uint32_t index;

    FUNCTION_MARKER(0x80078EF4, "RASHCDF.BIN");
    (void)sub_80022A1C(2);
    for (index = 0; index < 37; ++index)
        (void)sub_80078C68(index, 1);
    w_u32(0x8008972C, 0);
    return 0x80090000;
}

uint32_t sub_8005FB1C(void)
{
    FUNCTION_MARKER(0x8005FB1C, "RASHCDF.BIN");
    (void)sub_8005F9D8();
    return sub_80062440();
}

uint32_t sub_80080220(void)
{
    FUNCTION_MARKER(0x80080220, "RASHCDF.BIN");
    (void)sub_80078EF4();
    (void)sub_80065C38();
    (void)sub_8005FB1C();
    (void)sub_8001BDB0();
    (void)sub_8001BE08(0, 0, 384, 240);
    return sub_8001BCE8(1);
}

uint32_t sub_8007F304(void)
{
    uint32_t state;

    FUNCTION_MARKER(0x8007F304, "RASHCDF.BIN");
    state = r_u32(0x8005B2F8);
    w_u8(state + 5, r_u8(state + 5) | 1);
    state = r_u32(0x8005B2F8);
    w_u8(state + 5, r_u8(state + 5) | 2);
    w_u32(0x8005ACC0, 1);
    w_u32(0x8005ACC4, 1);
    w_u8(r_u32(0x8005B2F8) + 56, 3);
    w_u8(r_u32(0x8005B2F8) + 57, 0);
    w_u8(r_u32(0x8005B2F8) + 10, 0);
    state = r_u32(0x8005B2F8);
    w_u8(state + 11, 0);
    return state;
}

uint32_t sub_8007F2A8(int32_t track)
{
    FUNCTION_MARKER(0x8007F2A8, "RASHCDF.BIN");
    switch (track)
    {
        case 38: case 44: case 50: return 9;
        case 39: case 45: case 51: return 10;
        case 40: case 46: case 52: return 3;
        case 41: case 47: case 53: return 4;
        case 42: case 43: case 48: case 49: case 54: case 55: return 1;
        default: return 0;
    }
}

uint32_t sub_8007F37C(void)
{
    uint32_t result;
    uint32_t index;
    uint32_t state;
    uint32_t mode;
    uint32_t descriptor;
    uint32_t destination;
    uint32_t value;
    int32_t kind;
    uint16_t setup_time[2];
    uint8_t setup_first[2];
    uint8_t setup_second[2];
    uint32_t setup_value[2];

    FUNCTION_MARKER(0x8007F37C, "RASHCDF.BIN");
    result = r_u32(0x8005B220);
    if (result)
        return result;
    (void)sub_8007F304();
    for (index = 0; index < 6; ++index)
        (void)sub_8001E100(0x800D81F0 + 36 * index, 0, 8);
    index = 0;
    if (r_u32(r_u32(0x8005B2F8) + 52))
        do
        {
            descriptor = 0x800D81D8 + 36 * index;
            (void)sub_8001C4A8(0x800D6DE0 + 192 * index, (uint32_t)r_s8(descriptor + 8));
            value = (uint32_t)r_s8(descriptor + 23);
            state = r_u32(0x8005B2F8);
            w_u32(0x800D7430 + 24 * index, value);
            ++index;
        } while (index < r_u32(state + 52));
    (void)sub_8001B67C((uint32_t)r_s8(0x800D80EF));
    kind = r_s8(0x800D80DC);
    state = r_u32(0x8005B2F8);
    switch (kind)
    {
        case 0: case 1:
            w_u32(state + 60, 0); w_u16(state + 58, 0); break;
        case 2: case 3:
            w_u32(state + 60, 1); w_u16(state + 58, 1); break;
        case 4:
            w_u32(state + 60, 2); w_u16(state + 58, 2); break;
        case 5:
            w_u16(state + 58, 3); w_u32(state + 60, 2); break;
        default: break;
    }
    w_u8(r_u32(0x8005B2F8) + 4, r_u8(0x800D80D8));
    state = r_u32(0x8005B2F8);
    w_u32(state + 64, (uint32_t)r_s8(0x800D80E0));
    w_u32(state + 68, r_u8(0x800D80E9));
    value = r_u8(state + 5);
    w_u8(state + 5, (r_u8(0x800D80DD) & 16) ? value | 1 : value & 0xFE);
    state = r_u32(0x8005B2F8);
    value = r_u8(state + 5);
    w_u8(state + 5, (r_u8(0x800D80DD) & 32) ? value | 2 : value & 0xFD);
    state = r_u32(0x8005B2F8);
    w_u32(state + 72, (uint32_t)r_s8(0x800D81DF));
    w_u32(state + 76, (uint32_t)r_s8(0x800D8203));
    if (r_s8(0x800D81DF) != r_s8(0x800D81DE))
    {
        w_u8(state + 5, r_u8(state + 5) | 2);
        w_u8(0x800D81DE, r_u8(0x800D81DF));
    }
    value = r_u8(0x800D80F1);
    mode = r_u32(0x800D80D8);
    w_u8(0x800D80F2, value);
    if (mode == 32 && (r_u32(0x8005AE7C) & 32))
        w_u8(0x800D80F5, 4);
    if (mode == 1 || mode == 8 || mode == 32)
    {
        for (index = 0; index < (uint32_t)(mode == 8 ? 2 : 1); ++index)
        {
            descriptor = 0x800D81E4 + 36 * index;
            setup_time[index] = r_u16(descriptor);
            setup_first[index] = r_u8(descriptor + 2);
            setup_second[index] = r_u8(descriptor + 3);
            setup_value[index] = r_u32(descriptor + 4);
        }
        for (index = 0; index < (uint32_t)(mode == 8 ? 2 : 1); ++index)
        {
            destination = 0x800D5784 + 72 * index;
            w_u16(destination, setup_time[index]);
            w_u8(destination + 2, setup_first[index]);
            w_u8(destination + 3, setup_second[index]);
            w_u32(destination + 4, setup_value[index]);
        }
    }
    switch (mode)
    {
        case 1:
            w_u32(r_u32(0x8005B2F8) + 48, 1);
            w_u8(0x800D80F1, 0);
            value = sub_8007F2A8(r_s8(0x800D80E0));
            w_u8(r_u32(0x8005B2F8) + 6, value);
            kind = r_s8(0x800D80DC);
            value = r_u8(0x80099530 + 2 * (uint32_t)kind);
            state = r_u32(0x8005B2F8);
            w_u8(0x800D80E6, value);
            w_u8(state + 7, value);
            kind = r_s8(0x800D80DC);
            state = r_u32(0x8005B2F8);
            w_u32(state + 72, r_u32(0x80099518 + 4 * (uint32_t)kind));
            kind = r_s8(0x800D80DC);
            w_u16(state + 8, r_u16(0x80099500 + 4 * (uint32_t)kind));
            w_u8(0x800D81E2, 2);
            return 2;
        case 4:
            state = r_u32(0x8005B2F8);
            w_u32(state + 48, 1);
            descriptor = 0x800D81D8 + 36 * r_u8(0x800D80EB);
            w_u32(state + 72, (uint32_t)r_s8(descriptor + 7));
            w_u8(0x800D80F1, 0);
            w_u8(state + 5, r_u8(state + 5) | 1);
            state = r_u32(0x8005B2F8);
            w_u8(state + 5, r_u8(state + 5) | 2);
            w_u32(0x8005ACC4, (uint32_t)r_s8(0x800D80E2));
            value = (uint32_t)r_s8(0x800D80E3);
            w_u32(0x8005ACC0, value);
            if (!value)
            {
                state = r_u32(0x8005B2F8);
                w_u8(state + 56, r_u8(state + 56) & 0xFD);
            }
            if (!r_s8(0x800D80E1))
            {
                state = r_u32(0x8005B2F8);
                w_u8(state + 56, r_u8(state + 56) & 0xFE);
            }
            w_u8(0x800D81E2, 2);
            return 2;
        case 8:
            state = r_u32(0x8005B2F8);
            w_u32(state + 48, 1);
            value = r_u8(0x800D81ED);
            w_u8(0x800D80F1, 0);
            w_u8(state + 10, value);
            w_u8(0x800D81E2, 2); w_u8(0x800D8206, 2);
            w_u8(0x800D822A, 2); w_u8(0x800D824E, 2);
            return 2;
        case 16:
            state = r_u32(0x8005B2F8);
            w_u8(state + 5, r_u8(state + 5) | 1);
            destination = r_u32(0x8005B2F8);
            w_u32(state + 48, 2);
            w_u8(destination + 5, r_u8(destination + 5) | 2);
            w_u8(0x800D81E2, 2); w_u8(0x800D8206, 2);
            return 0x800D81D8;
        case 17:
            state = r_u32(0x8005B2F8);
            w_u32(state + 48, 2);
            kind = r_s8(0x800D80DC);
            value = r_u8(0x8009953C + 2 * (uint32_t)kind);
            w_u8(0x800D80E6, value); w_u8(state + 7, value);
            state = r_u32(0x8005B2F8);
            w_u8(state + 5, r_u8(state + 5) | 1);
            state = r_u32(0x8005B2F8);
            w_u8(state + 5, r_u8(state + 5) | 2);
            w_u8(r_u32(0x8005B2F8) + 6, r_s8(0x800D81DF) >= 18);
            w_u8(0x800D81E2, 2); w_u8(0x800D8206, 2);
            return 0x800D81D8;
        case 24:
            state = r_u32(0x8005B2F8);
            w_u8(state + 5, r_u8(state + 5) | 1);
            destination = r_u32(0x8005B2F8);
            w_u32(state + 48, 2);
            w_u8(destination + 5, r_u8(destination + 5) | 2);
            w_u8(r_u32(0x8005B2F8) + 10, r_u8(0x800D81ED));
            w_u8(r_u32(0x8005B2F8) + 11, r_u8(0x800D8211));
            w_u8(0x800D81E2, 2); w_u8(0x800D8206, 2);
            w_u8(0x800D822A, 2); w_u8(0x800D824E, 2);
            return 0x800D81D8;
        case 32:
            for (index = 0; index < 16; ++index)
            {
                descriptor = 0x800D8118 + 8 * index;
                destination = 0x800D5814 + 72 * index;
                w_u16(destination, r_u16(descriptor));
                w_u8(destination + 2, r_u8(descriptor + 2));
                w_u8(destination + 3, r_u8(descriptor + 3));
                w_u32(destination + 4, r_u32(descriptor + 4));
            }
            kind = r_s8(0x800D80DC);
            switch (kind)
            {
                case 0: case 2: case 4:
                    w_u8(r_u32(0x8005B2F8) + 4, 34); break;
                case 1:
                    w_u8(r_u32(0x8005B2F8) + 4, 36);
                    w_u16(r_u32(0x8005B2F8) + 8, 140); break;
                case 3:
                    w_u8(r_u32(0x8005B2F8) + 4, 33); break;
                case 5:
                    w_u8(r_u32(0x8005B2F8) + 4, 44);
                    w_u8(r_u32(0x8005B2F8) + 10, 0);
                    w_u8(r_u32(0x8005B2F8) + 57, 0);
                    value = (uint32_t)r_s8(0x800D81E1);
                    w_u32(0x8005ACC0, 0);
                    if (!value) w_u8(0x800D81DF, 8);
                    else if (value == 1) w_u8(0x800D81DF, 17);
                    w_u32(r_u32(0x8005B2F8) + 72, (uint32_t)r_s8(0x800D81DF));
                    break;
                default: break;
            }
            w_u32(r_u32(0x8005B2F8) + 48, 1);
            w_u8(0x800D80F1, 0);
            return 0x800E0000;
        default:
            return mode - 1 < 32 ? 0x8007FB6C : 0x80060000;
    }
}

uint32_t sub_8007FEDC(void)
{
    FUNCTION_MARKER(0x8007FEDC, "RASHCDF.BIN");
    (void)sub_80011738();
    (void)sub_80080CC8();
    (void)sub_8007FDA8();
    (void)sub_80080544();
    (void)sub_800665E8();
    (void)sub_800806D0();
    (void)sub_800809E0(0);
    (void)sub_80080C04();
    (void)sub_80080274();
    (void)sub_8007F37C();
    return sub_80080220();
}

uint32_t sub_80063908(void)
{
    uint32_t index;
    uint32_t resource;
    uint32_t subtype;
    int32_t value;

    FUNCTION_MARKER(0x80063908, "RASHCDF.BIN");
    for (index = 0; index < 37; ++index)
    {
        resource = r_u32(0x8009C4B0 + 4 * index);
        if (!resource || !r_u32(resource + 4))
            continue;
        switch (r_u8(resource + 1))
        {
            case 1: value = r_s8(0x800D81E1); break;
            case 2: value = r_s8(0x800D81E2); break;
            case 3:
                subtype = r_u8(resource + 2);
                switch (subtype)
                {
                    case 7:
                        value = r_s8(0x800D81DF + 36 * r_u8(0x800D80EB));
                        break;
                    case 17: case 25:
                        value = r_s8(0x800D8203); break;
                    case 20:
                        value = r_s8(0x800D8203);
                        if (value < 18) continue;
                        break;
                    case 21:
                        value = r_s8(0x800D8203);
                        if (value >= 18) continue;
                        break;
                    default: value = r_s8(0x800D81DF); break;
                }
                break;
            case 4: value = r_s8(0x800D80E0); break;
            case 5:
                w_u8(0x8009C5EF, 56); continue;
            case 6: value = r_s8(0x800D80DC); break;
            case 7:
                switch (r_u8(resource + 2))
                {
                    case 11: value = r_s8(0x800D80E1); break;
                    case 12: value = r_s8(0x800D80E2); break;
                    case 13: value = r_s8(0x800D80E3); break;
                    case 28: value = r_s8(0x800D80EC); break;
                    case 29: value = r_s8(0x800D80EF); break;
                    case 30: value = r_s8(0x800D80F1); break;
                    case 32: value = r_s8(0x800D8198 + r_u8(0x8009C5DE)); break;
                    case 35: value = r_s8(0x800D81EF); break;
                    case 36: value = r_s8(0x8009C5E9); break;
                    default: continue;
                }
                break;
            case 8: value = r_s8(0x800D80DF); break;
            case 9: value = r_u8(0x8009C5DE); break;
            case 10:
                subtype = (uint32_t)r_s8(resource + 2);
                if (subtype == 26) value = r_s8(0x800D81ED);
                else if (subtype == 27) value = r_s8(0x800D8211);
                else continue;
                break;
            case 12:
                subtype = (uint32_t)r_s8(resource + 2);
                if (subtype == 33)
                {
                    w_u8(0x8009C5E6, 0);
                    value = 0;
                }
                else if (subtype == 34) value = r_s8(0x800D81E0);
                else continue;
                break;
            default: continue;
        }
        (void)sub_800630C0(resource, (uint32_t)value);
    }
    return 0;
}

uint32_t sub_8007FD20(void)
{
    uint32_t state;
    uint32_t mode;
    uint32_t value;
    uint32_t menu;
    int32_t difficulty;
    int32_t id;

    FUNCTION_MARKER(0x8007FD20, "RASHCDF.BIN");
    state = r_u32(0x8005B2F8);
    mode = r_u8(0x800D80D8);
    w_u32(0x8005B220, 0);
    w_u8(state + 4, mode);
    state = r_u32(0x8005B2F8);
    w_u32(state + 48, (uint32_t)r_s8(0x800D80DE));
    w_u32(state + 64, (uint32_t)r_s8(0x800D80E0));
    difficulty = r_s8(0x800D80DC);
    w_u32(state + 60, (uint32_t)difficulty);
    w_u16(state + 58, (uint16_t)difficulty);
    w_u32(state + 72, (uint32_t)r_s8(0x800D81DF));
    id = r_s8(0x800D80E4);
    mode = r_u32(0x8009C68C);
    value = r_u8(0x800D80E5);
    menu = r_u32(mode + 4 * (uint32_t)id);
    value = (uint32_t)(int32_t)(int8_t)value;
    w_u16(menu + 4, (uint16_t)value);
    return value;
}

uint32_t sub_8007E7BC(int32_t track, int32_t value)
{
    uint32_t address;

    FUNCTION_MARKER(0x8007E7BC, "RASHCDF.BIN");
    address = 0x80053A88 + 176 * ((uint32_t)track - 56) + 168;
    return value < r_s32(address);
}

uint32_t sub_80068688(uint32_t id)
{
    uint32_t index;
    uint32_t descriptor;
    uint32_t resource;
    uint32_t value;
    uint32_t menu;

    FUNCTION_MARKER(0x80068688, "RASHCDF.BIN");
    (void)sub_8002D250();
    w_u8(0x800D80E9, 0);
    w_u8(0x800D81DE, 127);
    w_u8(0x800D8202, 127);
    w_u8(0x800D80DD, 48);
    w_u8(0x800D80ED, 0);
    w_u8(0x800D80EE, 0);
    w_u16(0x8009C5D4, 40);
    w_u8(0x800D80F5, 0);
    w_u8(0x8009C5F2, 0);
    for (index = 0; index < 6; ++index)
    {
        descriptor = 0x800D81D8 + 36 * index;
        w_u16(descriptor + 12, 0);
        w_u8(descriptor + 14, 0); w_u8(descriptor + 15, 0);
        w_u32(descriptor + 16, 0);
        w_u8(descriptor + 20, 0); w_u8(descriptor + 22, 0);
    }
    (void)sub_8001E100(0x800D8118, 0, 128);
    (void)sub_8001E100(0x800D80F8, 0, 16);
    if (id != 38)
    {
        w_u8(0x800D80EE, 1);
        w_u16(0x8009C5D4, 24);
    }
    resource = r_u32(0x8009C4D0);
    value = (uint32_t)r_s8(0x800D80E0);
    w_u8(0x800D80EB, 0);
    w_u8(0x8009C5F3, 0);
    (void)sub_800630C0(resource, value);
    (void)sub_800630C0(r_u32(0x8009C4CC), (uint32_t)r_s8(0x800D81DF));
    (void)sub_800630C0(r_u32(0x8009C4E4), (uint32_t)r_s8(0x800D80E3));
    (void)sub_800630C0(r_u32(0x8009C4E0), (uint32_t)r_s8(0x800D80E2));
    (void)sub_800630C0(r_u32(0x8009C4DC), (uint32_t)r_s8(0x800D80E1));
    (void)sub_800630C0(r_u32(0x8009C4C8), (uint32_t)r_s8(0x800D80DC));
    (void)sub_800630C0(r_u32(0x8009C4D4), r_u8(0x800D80EA));
    menu = r_u32(r_u32(0x8009C68C) + 4 * id);
    value = sub_80080A70(menu);
    menu = r_u32(r_u32(0x8009C68C) + 4 * id);
    w_u16(menu + 4, value);
    return value;
}

uint32_t sub_8007E770(int32_t first, int32_t last)
{
    uint32_t byte;

    FUNCTION_MARKER(0x8007E770, "RASHCDF.BIN");
    if (last < first)
        return 1;
    do
    {
        byte = r_u8(0x800D81D4 + (uint32_t)(first >> 3));
        if (!((byte >> ((uint32_t)first & 7)) & 1))
            return 0;
        first = (int32_t)((uint32_t)first + 1);
    } while (last >= first);
    return 1;
}

uint32_t sub_8007E7EC(int32_t track)
{
    FUNCTION_MARKER(0x8007E7EC, "RASHCDF.BIN");
    return (uint32_t)track < 18 && (track & 1);
}

uint32_t sub_8007E2C8(void)
{
    uint32_t first;
    uint32_t second;
    uint32_t random;
    uint32_t outcome;
    uint32_t bitmap;
    uint32_t count;
    uint32_t days;
    uint32_t setup;
    uint32_t time;
    uint32_t byte_first;
    uint32_t byte_second;
    int32_t track;
    int32_t difficulty;

    FUNCTION_MARKER(0x8007E2C8, "RASHCDF.BIN");
    first = r_u8(0x800D81F1);
    second = r_u8(0x800D81F5);
    w_u16(0x800D8104, first);
    w_u16(0x800D8106, second);
    random = sub_80043F00(0xF2000002) & 255;
    sub_8001FC84((3305 * random) >> 6);
    time = r_u16(0x800D5784);
    byte_first = r_u8(0x800D5786);
    byte_second = r_u8(0x800D5787);
    setup = r_u32(0x800D5788);
    outcome = r_u32(0x800D81F8);
    w_u16(0x800D81E4, time);
    w_u32(0x800D81E8, setup);
    w_u8(0x800D81E6, byte_first);
    w_u8(0x800D81E7, byte_second);
    switch (outcome)
    {
        case 250:
            count = r_u16(0x800D80FE);
            w_u32(0x8009C680, 0x52534F4C);
            w_u16(0x800D80FE, count + 1);
            (void)sub_800782B0(25, 1);
            track = r_s8(0x800D80DF);
            w_u32(0x8009C67C, r_u32(0x80089508 + 4 * (uint32_t)(track % 6)));
            random = sub_8001FC58() & 3;
            track = r_s8(0x800D80DF);
            w_u16(0x8009C5EC, 1370 + 8 * (uint32_t)track + random);
            break;
        case 252: case 253:
            first = r_u8(0x800D80DF);
            bitmap = 0x800D81D4 + (uint32_t)((int32_t)(first << 24) >> 27);
            w_u8(bitmap, r_u8(bitmap) | (1 << (first & 7)));
            w_u32(0x8009C680, 0x524E4E57);
            days = r_u8(0x800D80E9);
            count = r_u16(outcome == 252 ? 0x800D80F8 : 0x800D80FA);
            w_u8(0x800D80E9, days + 1);
            w_u16(outcome == 252 ? 0x800D80F8 : 0x800D80FA, count + 1);
            (void)sub_800782B0(24, 1);
            if (outcome == 252)
            {
                track = r_s8(0x800D80DF);
                w_u32(0x8009C67C, r_u32(0x800894E8 + 4 * (uint32_t)(track % 6)));
                random = sub_8001FC58() & 3;
                track = r_s8(0x800D80DF);
                w_u16(0x8009C5EC, 1366 + 8 * (uint32_t)track + random);
            }
            else
            {
                random = sub_8001FC58() & 3;
                if (sub_8007E7EC(r_s8(0x800D80DF)) == 1)
                {
                    w_u32(0x8009C67C, 0x414B4C4A);
                    w_u16(0x8009C5EC, random + 1362);
                }
                else
                {
                    w_u32(0x8009C67C, 0x53444C4A);
                    w_u16(0x8009C5EC, random + 1358);
                }
            }
            break;
        default:
            count = r_u16(0x800D8102);
            w_u32(0x8009C680, 0x444B5257);
            w_u16(0x800D8102, count + 1);
            (void)sub_800782B0(23, 1);
            first = 4 * (sub_8001FC58() % 5);
            w_u32(0x8009C67C, r_u32(0x80089574 + first));
            random = sub_8001FC58() & 3;
            w_u16(0x8009C5EC, first + 1294 + random);
            break;
    }
    difficulty = r_s8(0x800D80DC);
    if (difficulty == 2)
    {
        if (sub_8007E770(6, 11))
        {
            first = r_u8(0x800D80DD);
            w_u8(0x800D80DC, 4);
            w_u8(0x800D80E9, 0);
            w_u8(0x800D80DD, first | 16);
        }
    }
    else if (difficulty == 0)
    {
        if (sub_8007E770(0, 5))
        {
            first = r_u8(0x800D80DD);
            w_u8(0x800D80DC, 2);
            w_u8(0x800D80E9, 0);
            w_u8(0x800D80DD, first | 16);
        }
    }
    else if (difficulty == 4 && sub_8007E770(12, 17))
    {
        (void)sub_8002D298();
        w_u8(0x800D80DC, 0);
        return 21;
    }
    return 28;
}

uint32_t sub_8007E724(int32_t first, int32_t last)
{
    FUNCTION_MARKER(0x8007E724, "RASHCDF.BIN");
    while (first <= last)
    {
        uint32_t bits = r_u8(0x800D81C8 + (uint32_t)(first >> 3));
        uint32_t bit = (bits >> ((uint32_t)first & 7)) & 1;

        first = (int32_t)((uint32_t)first + 1);
        if (!bit)
            return 0;
    }
    return 1;
}

uint32_t sub_8007DDA8(void)
{
    uint32_t first_time;
    uint32_t first_kind;
    uint32_t first_flags;
    uint32_t first_value;
    uint32_t second_time;
    uint32_t second_kind;
    uint32_t second_flags;
    uint32_t second_value;
    uint32_t second_player;
    int32_t outcome;
    uint32_t result = 37;
    uint32_t offset;
    uint32_t prompt;
    uint32_t random;
    int32_t difficulty;

    FUNCTION_MARKER(0x8007DDA8, "RASHCDF.BIN");
    w_u32(0x8009C680, 300);
    w_u16(0x800D8114, r_u8(0x800D8215));
    w_u16(0x800D8116, r_u8(0x800D8219));
    w_u16(0x800D8104, r_u8(0x800D81F1));
    second_player = r_u8(0x800D81F5);
    first_time = r_u16(0x800D5784);
    first_kind = r_u8(0x800D5786);
    first_flags = r_u8(0x800D5787);
    first_value = r_u32(0x800D5788);
    second_time = r_u16(0x800D57CC);
    second_kind = r_u8(0x800D57CE);
    second_flags = r_u8(0x800D57CF);
    second_value = r_u32(0x800D57D0);
    w_u16(0x800D81E4, first_time);
    w_u8(0x800D81E6, first_kind);
    w_u8(0x800D81E7, first_flags);
    w_u32(0x800D81E8, first_value);
    w_u16(0x800D8208, second_time);
    w_u8(0x800D820A, second_kind);
    w_u8(0x800D820B, second_flags);
    w_u32(0x800D820C, second_value);
    w_u16(0x800D8106, second_player);
    (void)sub_8001FC84((3305 * (sub_80043F00(0xF2000002) & 255)) >> 6);
    outcome = r_s32(0x800D81F8);
    if (outcome == 250 || (outcome >= 4 && (uint32_t)(outcome - 250) >= 6))
    {
        uint32_t count = r_u16(0x800D80FE);

        w_u32(0x8009C680, 0x52534F4C);
        w_u16(0x800D80FE, count + 1);
        (void)sub_800782B0(21, 1);
        offset = 4 * (sub_8001FC58() % 5);
        w_u32(0x8009C67C, r_u32(0x8008954C + offset));
        random = sub_8001FC58() & 3;
        prompt = offset + 1254 + random;
    }
    else if (outcome == 251 || outcome == 252 || outcome == 253 || outcome == 255)
    {
        uint32_t count = r_u16(0x800D8102);

        w_u32(0x8009C680, 0x444B5257);
        w_u16(0x800D8102, count + 1);
        (void)sub_800782B0(23, 1);
        offset = 4 * (sub_8001FC58() % 5);
        w_u32(0x8009C67C, r_u32(0x80089574 + offset));
        random = sub_8001FC58() & 3;
        prompt = offset + 1274 + random;
    }
    else if (outcome == 254)
    {
        uint32_t count = r_u16(0x800D8100);

        w_u32(0x8009C680, 0x44545342);
        w_u16(0x800D8100, count + 1);
        (void)sub_800782B0(20, 1);
        offset = 4 * (sub_8001FC58() % 11);
        w_u32(0x8009C67C, r_u32(0x80089520 + offset));
        random = sub_8001FC58() & 3;
        prompt = offset + 1314 + random;
    }
    else
    {
        uint32_t counter = outcome == 1 ? 0x800D80F8 : outcome == 2 ? 0x800D80FA : 0x800D80FC;
        uint32_t track;
        uint32_t bitmap;
        uint32_t bits;
        uint32_t count;

        w_u16(counter, r_u16(counter) + 1);
        track = r_u8(0x800D80E0);
        bitmap = 0x800D81C8 + (uint32_t)((int32_t)(track << 24) >> 27);
        bits = r_u8(bitmap);
        w_u8(bitmap, bits | (1 << (track & 7)));
        count = r_u8(0x800D80E9);
        w_u32(0x8009C680, 0x524E4E57);
        w_u8(0x800D80E9, count + 1);
        (void)sub_800782B0(22, 1);
        offset = 4 * (sub_8001FC58() % 5);
        w_u32(0x8009C67C, r_u32(0x80089560 + offset));
        random = sub_8001FC58() & 3;
        prompt = offset + 1146 + random;
    }
    w_u16(0x8009C5EC, prompt);
    difficulty = r_s8(0x800D80DC);
    if (difficulty == 4)
    {
        if (sub_8007E724(19, 24))
        {
            uint32_t track = r_u8(0x800D80E0);
            uint32_t bitmap = 0x800D81C8 + (uint32_t)((int32_t)(track << 24) >> 27);

            result = 21;
            w_u8(bitmap, r_u8(bitmap) & ~(1 << (track & 7)));
        }
    }
    else if (sub_8007E724(difficulty == 2 ? 10 : 1, difficulty == 2 ? 15 : 6))
    {
        uint32_t flags = r_u8(0x800D80DD);

        w_u8(0x800D80DC, difficulty == 2 ? 4 : 2);
        w_u8(0x800D80E9, 0);
        w_u8(0x800D80DD, flags | 16);
    }
    return result;
}

void sub_80064610(void)
{
    char path[260];
    uint8_t globals[2732];
    uint32_t handle;
    uint32_t flags;
    uint32_t variant;
    uint32_t state;
    int32_t difficulty;
    int32_t path_length;
    uint32_t offset;

    FUNCTION_MARKER(0x80064610, "RASHCDI.BIN");
    path_length = snprintf(path, sizeof(path), "%sGLOBALS.BI", (const char *)xport_guest_cptr(0x80052464, 1));
    if (path_length < 0 || (uint32_t)path_length >= sizeof(path))
    {
        RRJ_DEBUG_FPUTS("Race globals path is too long\n", stderr);
        abort();
    }
    handle = sub_8001458C(path, r_u32(0x8005AD5C));
    if ((int32_t)handle < 0)
    {
        RRJ_DEBUG_FPRINTF(stderr, "Cannot open %s for reading\n", path);
        abort();
    }
    if (sub_80014780(handle, globals, sizeof(globals)) != sizeof(globals))
    {
        RRJ_DEBUG_FPRINTF(stderr, "Data size mismatch in %s\n", path);
        (void)sub_8001460C(handle);
        abort();
    }
    (void)sub_8001460C(handle);
    (void)sub_8001E0B4(xport_guest_ptr(0x8005ADC0, 4), globals + 0, 4);
    (void)sub_8001E0B4(xport_guest_ptr(0x8005ADC4, 8), globals + 4, 8);
    (void)sub_8001E0B4(xport_guest_ptr(0x80052F50, 80), globals + 12, 80);
    (void)sub_8001E0B4(xport_guest_ptr(0x80052EE4, 108), globals + 92, 108);
    (void)sub_8001E0B4(xport_guest_ptr(0x8005ADE0, 4), globals + 200, 4);
    (void)sub_8001E0B4(xport_guest_ptr(0x8005ADE4, 4), globals + 204, 4);
    (void)sub_8001E0B4(xport_guest_ptr(0x8005ADE8, 4), globals + 208, 4);
    (void)sub_8001E0B4(xport_guest_ptr(0x8005ADEC, 4), globals + 212, 4);
    (void)sub_8001E0B4(xport_guest_ptr(0x8005ADF0, 4), globals + 216, 4);
    (void)sub_8001E0B4(xport_guest_ptr(0x80053174, 20), globals + 220, 20);
    (void)sub_8001E0B4(xport_guest_ptr(0x80053018, 12), globals + 240, 12);
    (void)sub_8001E0B4(xport_guest_ptr(0x80052FAC, 36), globals + 252, 36);
    (void)sub_8001E0B4(xport_guest_ptr(0x80052FD0, 36), globals + 288, 36);
    (void)sub_8001E0B4(xport_guest_ptr(0x80052FF4, 36), globals + 324, 36);
    (void)sub_8001E0B4(xport_guest_ptr(0x80053024, 12), globals + 360, 12);
    (void)sub_8001E0B4(xport_guest_ptr(0x80053188, 12), globals + 372, 12);
    (void)sub_8001E0B4(xport_guest_ptr(0x80053194, 12), globals + 384, 12);
    (void)sub_8001E0B4(xport_guest_ptr(0x800531A0, 12), globals + 396, 12);
    (void)sub_8001E0B4(xport_guest_ptr(0x80053030, 12), globals + 408, 12);
    (void)sub_8001E0B4(xport_guest_ptr(0x8005303C, 12), globals + 420, 12);
    (void)sub_8001E0B4(xport_guest_ptr(0x80053048, 12), globals + 432, 12);
    (void)sub_8001E0B4(xport_guest_ptr(0x80053054, 12), globals + 444, 12);
    (void)sub_8001E0B4(xport_guest_ptr(0x80053060, 12), globals + 456, 12);
    (void)sub_8001E0B4(xport_guest_ptr(0x8005306C, 12), globals + 468, 12);
    (void)sub_8001E0B4(xport_guest_ptr(0x80053078, 12), globals + 480, 12);
    (void)sub_8001E0B4(xport_guest_ptr(0x80053084, 12), globals + 492, 12);
    (void)sub_8001E0B4(xport_guest_ptr(0x80053090, 12), globals + 504, 12);
    (void)sub_8001E0B4(xport_guest_ptr(0x8005309C, 12), globals + 516, 12);
    (void)sub_8001E0B4(xport_guest_ptr(0x800530A8, 24), globals + 528, 24);
    (void)sub_8001E0B4(xport_guest_ptr(0x800530C0, 24), globals + 552, 24);
    (void)sub_8001E0B4(xport_guest_ptr(0x800530FC, 24), globals + 576, 24);
    (void)sub_8001E0B4(xport_guest_ptr(0x80053114, 12), globals + 600, 12);
    (void)sub_8001E0B4(xport_guest_ptr(0x80053120, 24), globals + 612, 24);
    (void)sub_8001E0B4(xport_guest_ptr(0x80053138, 12), globals + 636, 12);
    (void)sub_8001E0B4(xport_guest_ptr(0x800530D8, 12), globals + 648, 12);
    (void)sub_8001E0B4(xport_guest_ptr(0x800530E4, 12), globals + 660, 12);
    (void)sub_8001E0B4(xport_guest_ptr(0x800530F0, 12), globals + 672, 12);
    (void)sub_8001E0B4(xport_guest_ptr(0x80053144, 12), globals + 684, 12);
    (void)sub_8001E0B4(xport_guest_ptr(0x80053150, 12), globals + 696, 12);
    (void)sub_8001E0B4(xport_guest_ptr(0x8005315C, 12), globals + 708, 12);
    (void)sub_8001E0B4(xport_guest_ptr(0x80053168, 12), globals + 720, 12);
    if (r_u32(0x8005B220))
    {
        flags = r_u8(r_u32(0x8005B2F8) + 4);
        variant = ((0 - (flags & 1)) & 0xFFFFFFFB) + 7;
    }
    else
    {
        flags = r_u8(r_u32(0x8005B2F8) + 4);
        if ((flags & 4) && flags != 44)
            variant = 6;
        else
        {
            state = r_u32(0x8005B2F8);
            flags = r_u8(state + 4);
            variant = 2 * (flags & 1);
            if (flags & 8)
                variant += 4;
            variant += r_u32(state + 48) == 2;
        }
    }
    difficulty = r_s16(r_u32(0x8005B2F8) + 58);
    offset = 732 + 60 * ((uint32_t)difficulty + 4 * variant);
    if (offset > sizeof(globals) - 60)
    {
        RRJ_DEBUG_FPUTS("Race globals selection is outside its data table\n", stderr);
        abort();
    }
    (void)sub_8001E0B4(xport_guest_ptr(0x800531AC, 60), globals + offset, 60);
    (void)sub_8001E0B4(xport_guest_ptr(0x8005ADCC, 8), globals + 2652, 8);
    (void)sub_8001E0B4(xport_guest_ptr(0x8005ADD4, 8), globals + 2660, 8);
    (void)sub_8001E0B4(xport_guest_ptr(0x8005ADDC, 4), globals + 2668, 4);
    (void)sub_8001E0B4(xport_guest_ptr(0x800531E8, 40), globals + 2672, 40);
    (void)sub_8001E0B4(xport_guest_ptr(0x80053210, 20), globals + 2712, 20);
}

uint32_t sub_80063C00(void)
{
    FUNCTION_MARKER(0x80063C00, "RASHCDI.BIN");
    w_u32(0x8005B598, 0);
    w_u8(0x8005AF55, 1);
    w_u8(0x8005AF54, 0);
    return 0x80060000;
}

uint32_t sub_80063C20(void)
{
    uint32_t index;
    uint32_t table;
    uint32_t flags;
    uint32_t row;

    FUNCTION_MARKER(0x80063C20, "RASHCDI.BIN");
    if (r_u32(0x8005AE74))
        return 1;
    w_u32(0x8005AE74, 1);
    for (index = 0; index < 4; ++index)
    {
        w_u8(0x800D8078 + 24 * index, 0);
        w_u32(0x800D807C + 24 * index, 0);
    }
    w_u32(0x8005B554, 0);
    w_u32(0x8005B540, 0);
    w_u16(0x8005B54C, 0);
    w_u32(0x8005B544, 0);
    w_u32(0x8005AE78, 0);
    row = r_u32(r_u32(0x8005B2F8) + 48) - 1;
    w_u16(0x8005B54E, (r_u8(0x800533B4 + 88 * row + 78) & 15) << 6);
    row = r_u32(r_u32(0x8005B2F8) + 48) - 1;
    table = 0x800533B4 + 88 * row;
    flags = r_u8(table + 78);
    index = r_u8(table + 77);
    w_u16(0x8005B548, 960);
    w_u16(0x8005B54A, 256);
    w_u16(0x8005B550, index + 16 * (flags & 16));
    return 0x80060000;
}

uint32_t sub_80063E48(void)
{
    uint32_t buffer;
    uint32_t page;
    uint32_t selected;
    uint32_t table;

    FUNCTION_MARKER(0x80063E48, "RASHCDI.BIN");
    w_u8(0x8005B58C, 0);
    w_u32(0x8005B588, 0);
    for (buffer = 0; buffer < 2; ++buffer)
    {
        for (page = 0; page < 2; ++page)
        {
            (void)rrj_gpu_clear_ot(0x800D9BE0 + 20 * buffer + 40 * page, 5);
            (void)rrj_gpu_clear_ot(0x800D9C30 + 4 * buffer + 8 * page, 1);
        }
    }
    selected = (uint32_t)r_s8(0x8005B58C);
    w_u32(0x8005B59C, 0x800D9BE0 + 40 * selected);
    w_u32(0x8005B5A8, 0x800D9C30 + 8 * selected);
    (void)rrj_gpu_clear_ot(0x800D9C50, 12);
    (void)rrj_gpu_clear_ot(0x800D9C80, 12);
    table = 0x800D9C50 + 48 * (uint32_t)r_s8(0x8005B58C);
    w_u32(0x8005B5AC, table);
    w_u32(0x8005B5A0, table + 40);
    w_u32(0x8005B5A4, table + 36);
    w_u32(0x8005B590, table + 32);
    w_u32(0x8005B594, table + 28);
    return table + 28;
}

void sub_8005D960(void)
{
    FUNCTION_MARKER(0x8005D960, "RASHCDI.BIN");
}

void sub_8005D968(void)
{
    FUNCTION_MARKER(0x8005D968, "RASHCDI.BIN");
    sub_8005D960();
}

uint32_t sub_8005D988(void)
{
    uint32_t index;

    FUNCTION_MARKER(0x8005D988, "RASHCDI.BIN");
    w_u32(0x800D8068, 0x800D39B0);
    w_u32(0x800D806C, 0);
    w_u32(0x800D8070, 0);
    w_u32(0x800D8074, 0);
    for (index = 0; index < 20; ++index)
    {
        uint32_t record = 0x800D39B0 + 112 * index;
        uint32_t flags = r_u32(record);

        w_u8(record + 60, 0);
        w_u32(record, (flags & 0xFFFFC000) | 63);
    }
    w_u32(0x8005B360, 0);
    return 0x80060000;
}

uint32_t sub_8005D5E4(void)
{
    FUNCTION_MARKER(0x8005D5E4, "RASHCDI.BIN");
    w_u32(0x8005B280, 0);
    sub_8005D968();
    return sub_8005D988();
}

uint32_t sub_8005D018(void)
{
    int32_t index;

    FUNCTION_MARKER(0x8005D018, "RASHCDI.BIN");
    for (index = 108; index >= 0; --index)
        w_u16(0x800CD6D8 + 2 * (uint32_t)index, 65535);
    w_u32(0x800D4C38, 0);
    w_u32(0x800D4C50, 150);
    w_u32(0x800D4C60, 100);
    w_u32(0x800D4C68, 20);
    w_u32(0x800D4C70, 300);
    w_u32(0x800D4C78, 45);
    w_u32(0x800D4C54, 0x800CD6D8);
    w_u32(0x800D4C80, 400);
    w_u32(0x800D4C90, 800);
    w_u32(0x800D4CA0, 200);
    w_u32(0x800D4C3C, 0);
    w_u32(0x800D4C40, 0);
    w_u32(0x800D4C44, 0);
    w_u32(0x800D4C48, 21);
    w_u32(0x800D4C4C, 0);
    w_u32(0x800D4C58, 21);
    w_u32(0x800D4C5C, 0);
    w_u32(0x800D4C64, 0x800CD702);
    w_u32(0x800D4C6C, 0);
    w_u32(0x800D4C74, 0x800CD72C);
    w_u32(0x800D4C7C, 0);
    w_u32(0x800D4C84, 0x800CD754);
    w_u32(0x800D4C88, 1);
    w_u32(0x800D4C8C, 0);
    w_u32(0x800D4C94, 0x800CD7AE);
    w_u32(0x800D4C98, 1);
    w_u32(0x800D4C9C, 0);
    w_u32(0x800D4CA4, 0x800CD7B0);
    return 0x800CD7B0;
}

uint32_t sub_8005BE40(void)
{
    uint32_t index;
    uint32_t records = 0x800D4B88;

    FUNCTION_MARKER(0x8005BE40, "RASHCDI.BIN");
    (void)sub_8005D018();
    for (index = 0; index < 7; ++index)
    {
        uint32_t slot = 0x800CE560 + 8 * index;

        w_u16(slot + 2, 65535);
        w_u16(slot, 0);
        w_u32(slot + 4, records);
        records += 4 * (uint32_t)r_s8(0x8006B49C + index);
    }
    return sub_8001E100(0x800CE1B0, 0, 800);
}
