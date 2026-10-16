#include "rrj_cd_transport.h"
#include "native_game_api.h"
#include "game_main.h"
#include "race_leaf.h"
#include "race_bodyless_batch_003.h"
#include "solo_resource_complete.h"
#include "xport.h"
#include "gpu.h"
#include "psx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "draft_quick.h"
#include "race_bodyless_batch_002.h"
#include "psx_press.h"
#include "race_pause.h"
#include "race_pause_frontier.h"
#include "race_bodyless_batch_004.h"
#include "race_bodyless_batch_006.h"

uint32_t sub_80061258(uint32_t image, uint16_t output[16])
{
    uint32_t palette;
    uint32_t packed;
    PSX_RECT rectangle;
    FUNCTION_MARKER(0x80061258, "RASHCDI.BIN");
    if (r_u8(image) != 64)
        return image;
    output[8] = r_u32(image + 12) & 0xFFF;
    output[9] = r_u16(image + 14) & 0xFFF;
    output[10] = r_u16(image + 4);
    output[14] = ((output[9] & 256) >> 4) | ((output[8] & 1023) >> 6) | (4 * (output[9] & 512));
    output[11] = r_u16(image + 6);
    packed = r_u32(image + 12) & 0xFFF;
    output[12] = (uint16_t)(4 * (packed - (packed & 0x1FC0)));
    packed = r_u16(image + 14) & 0xFFF;
    output[13] = (uint16_t)(packed - (packed & 0x1F00));
    rectangle.x = output[8];
    rectangle.y = output[9];
    rectangle.w = (int16_t)output[10] >> 2;
    rectangle.h = output[11];
    (void)LoadImagePSX(&rectangle, (uint32 *)xport_guest_ptr(image + 16,
        rectangle.w > 0 && rectangle.h > 0 ? (size_t)rectangle.w * rectangle.h * 2 : 0));
    packed = r_u32(image) >> 8;
    if (!packed)
        return image;
    palette = image + (packed << 4);
    if (r_u8(palette) != 35)
        return image;
    rectangle.x = r_u32(palette + 12) & 0xFFF;
    rectangle.y = r_u16(palette + 14) & 0xFFF;
    rectangle.w = r_s16(palette + 4);
    rectangle.h = r_s16(palette + 6);
    (void)LoadImagePSX(&rectangle, (uint32 *)xport_guest_ptr(palette + 16,
        rectangle.w > 0 && rectangle.h > 0 ? (size_t)rectangle.w * rectangle.h * 2 : 0));
    packed = r_u32(palette + 12);
    output[15] = (((packed >> 16) & 0xFFF) << 6) | ((packed >> 4) & 63);
    return image;
}

uint32_t sub_80061430(int32_t slot)
{
    uint16_t texture[16] = {0};
    uint32_t entry;
    uint32_t image;
    uint32_t y;
    uint32_t height;
    FUNCTION_MARKER(0x80061430, "RASHCDI.BIN");
    slot = (int8_t)slot;
    entry = 0x800D8078 + 24 * (uint32_t)slot;
    image = r_u32(entry + 12);
    w_u32(image + 12, (r_u32(image + 12) & 0xFFFFF000) | (r_u16(0x8005B54E) & 0xFFF));
    y = r_u16(0x8005B550);
    image = r_u32(entry + 12);
    w_u32(image + 12, (r_u32(image + 12) & 0xF000FFFF) | ((y & 0xFFF) << 16));
    height = r_u16(r_u32(entry + 12) + 6);
    w_u32(0x8005B554, entry);
    w_u16(0x8005B550, y + height);
    (void)sub_80061258(r_u32(entry + 12), texture);
    entry = r_u32(0x8005B554);
    w_u16(entry + 16, texture[14]);
    w_u8(entry + 18, texture[12]);
    w_u8(r_u32(0x8005B554) + 19, texture[13]);
    w_u8(r_u32(0x8005B554) + 3, 0);
    w_u8(r_u32(0x8005B554) + 2, 3);
    return 3;
}

int32_t sub_80061528(const char *name, uint32_t upload, uint32_t split)
{
    int32_t slot = -1;
    int32_t candidate;
    uint32_t entry;
    uint32_t handle;
    uint32_t header = 0;
    uint32_t pixels = 0;
    uint32_t bytes;
    uint32_t header_bytes;
    uint32_t pixel_bytes;
    uint32_t index;
    uint32_t glyph;
    uint8_t file_header[32];
    int length;
    char path[128];
    FUNCTION_MARKER(0x80061528, "RASHCDI.BIN");
    if (!r_u16(0x8005B54C))
    {
        uint32_t palette = sub_8002CA64((uint32_t *)xport_guest_ptr(0x80053A68, 32),
            (uint32_t)r_s16(0x8005B54E), (uint32_t)r_s16(0x8005B550));
        w_u8(r_u32(0x8005B554) + 3, palette);
        w_u16(0x8005B550, r_u16(0x8005B550) + 1);
    }
    candidate = (int32_t)(r_u32(0x8005B540) + 1);
    if (candidate < 0 || candidate >= 4)
        candidate = 0;
    if (!r_s8(0x800D8078 + 24 * (uint32_t)candidate))
        slot = candidate;
    else
        for (candidate = 0; candidate < 4; ++candidate)
            if (!r_s8(0x800D8078 + 24 * (uint32_t)candidate))
            {
                slot = candidate;
                break;
            }
    // Reject a full table before replacing the previously selected font
    if (slot == -1)
        return -1;
    length = snprintf(path, sizeof(path), "%s%s", (const char *)xport_guest_cptr(0x80052478, 1), name);
    if (length < 0 || length >= (int)sizeof(path))
        abort();
    if (split)
    {
        handle = sub_8001458C(path, r_u32(0x8005AD5C));
        if ((int32_t)handle < 0)
            return -1;
        if (sub_80014780(handle, file_header, 32) != 32)
            goto failed_read;
        header_bytes = xport_load_le32(file_header + 28);
        pixel_bytes = xport_load_le32(file_header + 4) - header_bytes;
        if (header_bytes < 32 || (int32_t)pixel_bytes < 0)
            goto failed_read;
        header = sub_8001447C(header_bytes, 1);
        if (!header)
            goto failed_read;
        for (index = 0; index < 8; ++index)
            w_u32(header + 4 * index, xport_load_le32(file_header + 4 * index));
        bytes = header_bytes - 32;
        if (sub_80014780(handle, xport_guest_ptr(header + 32, bytes), bytes) != bytes)
            goto failed_read;
        pixels = sub_8001447C(pixel_bytes, 1);
        if (!pixels || sub_80014780(handle, xport_guest_ptr(pixels, pixel_bytes), pixel_bytes) != pixel_bytes)
            goto failed_read;
        (void)sub_8001460C(handle);
    }
    else
    {
        if ((int32_t)sub_80014654(path, r_u32(0x8005AD5C), &header, &bytes) < 0)
            return 0;
        pixels = header + r_u32(header + 28);
    }
    entry = 0x800D8078 + 24 * (uint32_t)slot;
    w_u32(0x8005B540, slot);
    w_u32(0x8005B554, entry);
    w_u8(entry, 1);
    w_u32(entry + 4, header);
    if (split)
    {
        w_u32(entry + 12, pixels);
        w_u32(entry + 8, header + r_u32(header + 20));
    }
    else
    {
        w_u32(entry + 8, header + r_u32(header + 20));
        w_u32(entry + 12, pixels);
    }
    if (split || upload)
    {
        if (r_s8(r_u32(0x8005B2F8)) == 2)
            (void)sub_80066188(slot);
        else
            (void)sub_80061430(slot);
        if (split)
            (void)sub_800144B8(pixels);
    }
    entry = r_u32(0x8005B554);
    if (r_s8(entry) == 1)
    {
        w_u16(entry + 20, 0);
        w_u16(entry + 22, 0);
        for (index = 0; index < r_u16(r_u32(entry + 4) + 10); ++index)
        {
            glyph = r_u32(entry + 8) + 11 * index;
            if (r_s16(entry + 20) < r_u8(glyph + 3))
                w_u16(entry + 20, r_u8(glyph + 3));
            if (r_s16(entry + 22) < r_s8(glyph + 8))
                w_u16(entry + 22, (uint32_t)r_s8(glyph + 8));
        }
    }
    return (int8_t)slot;

failed_read:
    (void)sub_8001460C(handle);
    if (pixels)
        (void)sub_800144B8(pixels);
    if (header)
        (void)sub_800144B8(header);
    return -1;
}

uint32_t sub_80060C78(uint32_t players)
{
    uint32_t index;
    uint32_t texture_count = 0;
    uint32_t palette_count = 0;
    uint32_t entry;
    uint32_t layout;
    uint32_t buffer;
    uint32_t rectangle;
    uint32_t y;
    uint32_t source;
    uint32_t shift;
    uint8_t record[20];
    char path[96];
    const char *name;
    int length;
    FUNCTION_MARKER(0x80060C78, "RASHCDI.BIN");
    for (index = 0; index < 32; ++index)
    {
        entry = 0x800D5144 + 44 * index;
        name = (const char *)xport_guest_cptr(entry + 12, 1);
        if (!strlen(name) || !strncmp(name, (const char *)xport_guest_cptr(0x8005B73C, 5), 5))
            continue;
        length = snprintf(path, sizeof(path), "%s%s", (const char *)xport_guest_cptr(0x800524A0, 1), name);
        if (length < 0 || length >= (int)sizeof(path))
            abort();
        if (sub_8001408C(path, &buffer, record))
            continue;
        layout = 0x800533B4 + 88 * (players - 1);
        rectangle = xport_load_le32(record + 12);
        w_u16(rectangle, ((r_u8(layout + 82) & 15) << 6) + 32 * (texture_count >> 3));
        w_u16(rectangle + 2, r_u8(layout + 81) + 16 * (r_u8(layout + 82) & 16) + 32 * (texture_count & 7));
        (void)rrj_gpu_upload(xport_guest_cptr(rectangle, 8), xport_load_le32(record + 16));
        shift = r_u8(layout + 82);
        y = 16 * (shift & 16) + r_u8(layout + 81);
        w_u32(entry + 4, ((y & 256) >> 4) | (shift & 15) | 128 | (4 * (y & 512)) | 0xE1000200);
        source = r_u32(entry + 8);
        w_u8(entry, 2 * r_u8(rectangle));
        ++texture_count;
        w_u8(entry + 1, r_u8(rectangle + 2));
        if (source == index)
        {
            shift = 32 * palette_count++;
            rectangle = xport_load_le32(record + 4);
            w_u16(rectangle, (r_u8(layout + 82) & 15) << 6);
            y = r_u8(layout + 81) + 16 * (r_u8(layout + 82) & 16) + shift + 31;
            w_u16(rectangle + 4, 128);
            w_u16(rectangle + 2, y);
            (void)rrj_gpu_upload(xport_guest_cptr(rectangle, 8), xport_load_le32(record + 8));
            w_u16(entry + 2, (r_u16(rectangle + 2) << 6) | ((r_u16(rectangle) >> 4) & 63));
        }
        (void)sub_800144B8(buffer);
    }
    for (index = 0; index < 32; ++index)
    {
        entry = 0x800D5144 + 44 * index;
        source = r_u32(entry + 8);
        if (source != index)
            w_u16(entry + 2, r_u16(0x800D5144 + 44 * source + 2));
    }
    return 0;
}

uint32_t sub_80060EFC(uint32_t players)
{
    uint32_t index;
    FUNCTION_MARKER(0x80060EFC, "RASHCDI.BIN");
    if (!r_u32(0x8005AD24))
        return 0;
    (void)sub_80060C78(players);
    for (index = 0; index < 27; ++index)
        w_u16(0x800D5EF2 + 2 * index, 64 * index);
    return sub_80013F8C();
}

uint32_t sub_80060894(uint32_t packet, uint32_t depth, const uint32_t vector[4], uint32_t clut_x, uint32_t clut_y)
{
    uint32_t shift;
    uint32_t result;
    FUNCTION_MARKER(0x80060894, "RASHCDI.BIN");
    shift = (2 - (depth & 7)) & 31;
    w_u16(packet + 6, getTPage(depth & 7, 0, vector[0], vector[1]));
    w_u16(packet + 2, getClut(clut_x, clut_y));
    w_u8(packet + 8, (vector[0] & 63) << shift);
    w_u8(packet + 10, (vector[0] & 63) << shift);
    w_u8(packet + 4, r_u8(packet + 8) + (vector[2] << shift) - 1);
    w_u8(packet, r_u8(packet + 10) + (vector[2] << shift) - 1);
    w_u8(packet + 1, vector[1]);
    w_u8(packet + 11, vector[1]);
    w_u8(packet + 5, r_u8(packet + 1) + (vector[3] & 255) - 1);
    result = r_u8(packet + 11) + (vector[3] & 255) - 1;
    w_u8(packet + 9, result);
    return result;
}

uint32_t sub_80060AA8(void)
{
    uint32_t vector[4];
    uint32_t packet;
    uint32_t columns;
    uint32_t index;
    uint32_t count = 0;
    uint32_t row;
    int32_t column;
    int32_t difference;
    int32_t rows;
    FUNCTION_MARKER(0x80060AA8, "RASHCDI.BIN");
    vector[2] = 16;
    vector[3] = 16;
    packet = r_u32(0x8005B278) + 10052;
    vector[0] = r_u32(0x8005B390);
    for (column = 0; column < 12; ++column)
    {
        difference = (int32_t)(vector[0] - r_u32(0x8005B390));
        vector[1] = r_u32(0x8005B394);
        columns = (uint32_t)(difference / 64);
        difference = (int32_t)((uint32_t)r_s16(r_u32(0x8005B278) + 64 + 2 * columns) - vector[1]);
        rows = difference / 16;
        for (row = 0; (int32_t)row < rows; ++row)
        {
            (void)sub_80060894(packet, 2, vector, 0, 0);
            packet += 12;
            vector[1] += vector[3];
            ++count;
        }
        vector[0] += vector[2];
    }
    index = r_u32(0x8005B278);
    w_u16(index + 40, count);
    return index;
}

uint32_t sub_80063448(uint32_t player)
{
    uint32_t voices;
    uint32_t settings;
    uint32_t table;
    uint32_t index = 0;
    uint32_t offset = 0;
    uint32_t result;
    FUNCTION_MARKER(0x80063448, "RASHCDI.BIN");
    settings = r_u32(0x8005B40C);
    voices = r_u32(0x8005B42C) + 132 * player;
    w_u32(voices + 16, 0xFFFFFFFF);
    w_u32(voices + 20, 0xFFFFFFFF);
    w_u32(voices + 24, 0xFFFFFFFF);
    w_u32(voices + 36, 0xFFFFFFFF);
    w_u32(voices + 40, 0xFFFFFFFF);
    w_u32(voices + 8, 0xFFFFFFFF);
    settings += 72 * player;
    w_u32(settings + 28, 0xFFFFFFFF);
    table = 0x8005B410 + 4 * player;
    result = r_u32(settings + 36) + r_u32(settings + 40);
    if ((int32_t)result <= 0)
        return result;
    do
    {
        w_u16(r_u32(table) + offset, 224);
        ++index;
        result = (int32_t)index < (int32_t)(r_u32(settings + 36) + r_u32(settings + 40));
        offset += 44;
    } while (result);
    return result;
}

void sub_80013AE4(void)
{
    FUNCTION_MARKER(0x80013AE4, "SLUS_010.53");
    w_u32(r_u32(0x8005B278) + 24, 2);
}

void sub_800609B0(void)
{
    uint32_t buffer;
    uint32_t x;
    uint32_t y;
    FUNCTION_MARKER(0x800609B0, "RASHCDI.BIN");
    buffer = sub_8001447C(12360, 1);
    w_u32(0x8005B278, buffer);
    if (!buffer)
        abort();
    (void)memset(xport_guest_ptr(buffer, 12360), 0, 12360);
    buffer = r_u32(0x8005B278);
    y = r_u16(0x8005B394);
    w_u32(buffer, 12360);
    w_u16(buffer + 18, 0xFFFF);
    w_u16(buffer + 32, 1);
    x = r_u16(0x8005B390);
    w_u16(buffer + 58, y);
    w_u16(buffer + 62, y);
    y += 256;
    w_u16(buffer + 64, y);
    w_u16(buffer + 66, y);
    w_u16(buffer + 68, y);
    w_u16(buffer + 70, y);
    w_u32(buffer + 24, 0);
    w_u16(buffer + 16, 0);
    w_u16(buffer + 20, 0);
    w_u32(buffer + 4, 0xFFFFFFFF);
    w_u16(buffer + 34, 0);
    w_u32(buffer + 8, 0xFFFFFFFF);
    w_u32(buffer + 12, 0xFFFFFFFF);
    w_u16(buffer + 36, 0);
    w_u16(buffer + 56, x);
    w_u16(buffer + 60, x);
    (void)sub_8001E100(buffer + 740, 0, 240);
    w_u16(r_u32(0x8005B278) + 22, 2);
    (void)DecDCTinSync(0);
    (void)DecDCToutSync(0);
    sub_80043DA4();
    (void)DecDCToutCallback(sub_80013AE4);
    w_u32(0x80055EE4, 0x80013AE4);
    sub_80043DB4();
}

uint32_t sub_80060BE8(uint32_t players)
{
    uint32_t layout;
    FUNCTION_MARKER(0x80060BE8, "RASHCDI.BIN");
    layout = 0x800533B4 + 88 * (players - 1);
    w_u32(0x8005B390, (r_u8(layout + 38) & 15) << 6);
    w_u32(0x8005B394, 16 * (r_u8(layout + 38) & 16) + r_u8(layout + 37));
    (void)sub_800201C0();
    sub_8002026C(0);
    DecDCTReset(0);
    sub_800609B0();
    return sub_80060AA8();
}

uint32_t sub_8006068C(void)
{
    uint32_t players;
    FUNCTION_MARKER(0x8006068C, "RASHCDI.BIN");
    players = r_u32(r_u32(0x8005B2F8) + 48);
    if (players == 1)
    {
        if (r_u32(0x8005B2D0))
            return sub_80060EFC(players);
        (void)sub_80060BE8(1);
    }
    if (r_u32(0x8005B2D0) || players == 2)
        return sub_80060EFC(players);
    return 2;
}

int32_t sub_80062D84(uint32_t handle, const uint32_t bank[3], const uint32_t *buffer_pointer, uint32_t scratch)
{
    uint32_t bytes;
    uint32_t buffer;
    int32_t result;
    FUNCTION_MARKER(0x80062D84, "RASHCDI.BIN");
    bytes = bank[1] + bank[2];
    buffer = xport_load_le32(buffer_pointer);
    if (!buffer)
        return -1;
    (void)sub_80014634(handle, bank[0] + 96, 0);
    if ((int32_t)sub_80014780(handle, xport_guest_ptr(scratch, bytes), bytes) < (int32_t)bytes)
        return -1;
    buffer = xport_load_le32(buffer_pointer);
    (void)sub_8001E0B4(xport_guest_ptr(buffer, bank[1]), xport_guest_cptr(scratch, bank[1]), bank[1]);
    w_u32(0x8005B3E0, 0);
    result = (int32_t)sub_8001ED28(xport_load_le32(buffer_pointer), scratch + bank[1], 0xFFFFFFFF, 0x80016464, 0, 1);
    if (result > 0)
        while (!r_u32(0x8005B3E0))
            rrj_native_wait_events();
    return result;
}

uint32_t sub_800627F8(void)
{
    uint8_t header_bytes[96];
    uint32_t banks[24];
    int32_t rider;
    int32_t second_rider = 0;
    uint32_t random_bank;
    uint32_t state;
    uint32_t buffer;
    uint32_t handle;
    uint32_t scratch;
    uint32_t index;
    uint32_t result;
    FUNCTION_MARKER(0x800627F8, "RASHCDI.BIN");
    rider = r_s32(r_u32(0x8005B2F8) + 72) / 9;
    (void)sub_8001F054(0, 0);
    sub_8001F080();
    (void)sub_8001F0D4();
    (void)sub_8001FBD4();
    if (!r_u32(0x8005B40C))
    {
        buffer = sub_8001447C(72 * r_u32(r_u32(0x8005B2F8) + 48), 0);
        w_u32(0x8005B40C, buffer);
        (void)sub_8001E100(buffer, 0, 72 * r_u32(r_u32(0x8005B2F8) + 48));
    }
    if (!r_u32(0x8005B42C))
    {
        buffer = sub_8001447C(132 * r_u32(r_u32(0x8005B2F8) + 48), 0);
        w_u32(0x8005B42C, buffer);
        (void)sub_8001E100(buffer, 0, 132 * r_u32(r_u32(0x8005B2F8) + 48));
    }
    if (!r_u32(0x8005B410))
    {
        buffer = sub_8001447C(352, 0);
        w_u32(0x8005B410, buffer);
        (void)sub_8001E100(buffer, 0, 352);
    }
    state = r_u32(0x8005B2F8);
    if (r_u32(state + 48) == 1)
    {
        buffer = r_u32(0x8005B40C);
        w_u32(buffer + 20, 64);
        w_u32(buffer + 36, 5);
        w_u32(buffer + 40, 3);
        if (r_u8(state + 4) == 33)
        {
            result = r_u32(buffer + 40) + 1;
            w_u32(buffer + 36, r_u32(buffer + 36) - 1);
            w_u32(buffer + 40, result);
        }
        (void)sub_80063448(0);
    }
    else
    {
        if (!r_u32(0x8005B414))
        {
            buffer = sub_8001447C(352, 0);
            w_u32(0x8005B414, buffer);
            (void)sub_8001E100(buffer, 0, 352);
        }
        buffer = r_u32(0x8005B40C);
        w_u32(buffer + 92, 127);
        w_u32(buffer + 20, 0);
        w_u32(buffer + 36, 3);
        w_u32(buffer + 40, 2);
        w_u32(buffer + 108, 3);
        w_u32(buffer + 112, 2);
        (void)sub_80063448(0);
        (void)sub_80063448(1);
    }
    handle = sub_8001458C((const char *)xport_guest_cptr(0x8005B790, 1), r_u32(0x8005AD5C));
    if ((int32_t)handle < 0)
        return handle;
    if (sub_80014780(handle, header_bytes, 96) != 96 || rider < 0 || rider >= 3)
    {
        (void)sub_8001460C(handle);
        return 0xFFFFFFFF;
    }
    for (index = 0; index < 24; ++index)
        banks[index] = xport_load_le32(header_bytes + 4 * index);
    result = r_u32(r_u32(0x8005B2F8) + 48) == 1 ? banks[1] : banks[4];
    w_u32(0x8005B3F8, sub_8001447C(result, 0));
    w_u32(0x8005B418, sub_8001447C(banks[7], 0));
    buffer = sub_8001447C(banks[3 * (uint32_t)rider + 10], 0);
    w_u32(r_u32(0x8005B42C) + 12, buffer);
    state = r_u32(0x8005B2F8);
    if (r_u32(state + 48) == 2)
    {
        second_rider = r_s32(state + 76) / 9;
        if (second_rider < 0 || second_rider >= 3)
        {
            (void)sub_8001460C(handle);
            return 0xFFFFFFFF;
        }
        buffer = sub_8001447C(banks[3 * (uint32_t)second_rider + 10], 0);
        w_u32(r_u32(0x8005B42C) + 144, buffer);
    }
    random_bank = ((sub_80043F00(0xF2000002) & 255) >> 7) + 6;
    if (r_u8(r_u32(0x8005B2F8) + 4) & 1)
        w_u32(0x8005B3F0, sub_8001447C(banks[3 * random_bank + 1], 0));
    scratch = sub_8001447C(0x40000, 0);
    if (!scratch)
    {
        (void)sub_8001460C(handle);
        return 0xFFFFFFFF;
    }
    index = r_u32(r_u32(0x8005B2F8) + 48) == 1 ? 0 : 3;
    result = (uint32_t)sub_80062D84(handle, banks + index, xport_guest_cptr(0x8005B3F8, 4), scratch);
    w_u32(0x8005B404, result);
    result = (uint32_t)sub_80062D84(handle, banks + 6, xport_guest_cptr(0x8005B418, 4), scratch);
    w_u32(0x8005B420, result);
    result = (uint32_t)sub_80062D84(handle, banks + 3 * (uint32_t)rider + 9,
        xport_guest_cptr(r_u32(0x8005B42C) + 12, 4), scratch);
    buffer = r_u32(0x8005B42C);
    state = r_u32(0x8005B2F8);
    w_u32(buffer + 8, result);
    if (r_u32(state + 48) == 2)
    {
        result = (uint32_t)sub_80062D84(handle, banks + 3 * (uint32_t)second_rider + 9, xport_guest_cptr(buffer + 144, 4), scratch);
        w_u32(r_u32(0x8005B42C) + 140, result);
    }
    if (r_u8(r_u32(0x8005B2F8) + 4) & 1)
    {
        result = (uint32_t)sub_80062D84(handle, banks + 3 * random_bank, xport_guest_cptr(0x8005B3F0, 4), scratch);
        w_u32(0x8005B3FC, result);
    }
    buffer = sub_8001458C((const char *)xport_guest_cptr(0x8005B7A4, 1), r_u32(0x8005AD5C));
    w_u32(0x800D6858, buffer);
    w_u32(0x800D685C, 0);
    w_u32(0x800D6860, 16);
    w_u32(0x800D6864, 0);
    w_u32(0x800D6868, sub_800148BC(buffer));
    w_u32(0x800D686C, 1);
    for (index = 0; index < 9; ++index)
    {
        w_u32(0x800D6AA0 + 32 * index, 0xFFFFFFFF);
        w_u32(0x800D6AA4 + 32 * index, 0xFFFFFFFF);
        w_u32(0x800D6AA8 + 32 * index, 0);
    }
    (void)sub_800144B8(scratch);
    (void)sub_8001EFE8(1);
    (void)sub_8001460C(handle);
    w_u32(0x8005B438, 0);
    w_u32(0x8005B3E8, 2);
    result = (uint32_t)((int32_t)(7 * r_u32(0x80052640)) >> 4);
    w_u32(0x800D6C08, result);
    return result;
}

uint32_t sub_8005F104(uint32_t data, uint16_t output[5])
{
    uint32_t fields[5];
    uint32_t count = 0;
    uint32_t length = 0;
    uint32_t index;
    uint32_t value;
    FUNCTION_MARKER(0x8005F104, "RASHCDI.BIN");
    value = r_u8(data);
    while (value != 13 && length < 80)
    {
        if (value == 44)
        {
            if (count < 5)
                fields[count] = data + 1;
            ++count;
            w_u8(data, 0);
        }
        ++data;
        value = r_u8(data);
        ++length;
    }
    for (index = 0; index < 5; ++index)
        output[index] = index < count ? (uint16_t)atoi((const char *)xport_guest_cptr(fields[index], 1)) : 0;
    return length + 2;
}

uint32_t sub_8005F1E4(const char *path)
{
    uint32_t buffer = 0;
    uint32_t bytes = 0;
    uint32_t cursor;
    uint32_t entry;
    uint32_t base;
    uint32_t index;
    uint32_t field;
    uint32_t group;
    uint32_t row;
    int32_t player;
    uint16_t values[5];
    FUNCTION_MARKER(0x8005F1E4, "RASHCDI.BIN");
    if ((int32_t)sub_80014654(path, r_u32(0x8005AD5C), &buffer, &bytes) <= 0 || !buffer)
        return 0;
    cursor = buffer;
    cursor += sub_8005F104(cursor, values);
    w_u16(0x8005B2B4, values[0]);
    w_u16(0x8005B2BC, values[1]);
    cursor += sub_8005F104(cursor, values);
    for (player = 0; player < r_s32(r_u32(0x8005B2F8) + 48); ++player)
    {
        base = 0x800D6198 + 224 * (uint32_t)player;
        for (group = 0; group < 3; ++group)
        {
            entry = base + (group == 0 ? 0 : group == 1 ? 128 : 192);
            (void)sub_8001E100(entry, 0, 32);
            w_u32(entry + 4, values[0]);
            w_u32(entry + 8, values[1]);
            w_u32(entry + 24, values[2]);
            w_u32(entry + 28, values[3]);
        }
    }
    cursor += sub_8005F104(cursor, values);
    for (player = 0; player < r_s32(r_u32(0x8005B2F8) + 48); ++player)
    {
        entry = 0x800D6198 + 224 * (uint32_t)player + 32;
        (void)sub_8001E100(entry, 0, 32);
        w_u32(entry + 4, values[0]);
        w_u32(entry + 8, values[1]);
        w_u32(entry + 24, values[2]);
        w_u32(entry + 28, values[3]);
    }
    cursor += sub_8005F104(cursor, values);
    for (player = 0; player < r_s32(r_u32(0x8005B2F8) + 48); ++player)
    {
        entry = 0x800D6198 + 224 * (uint32_t)player + 64;
        (void)sub_8001E100(entry, 0, 32);
        w_u32(entry + 4, values[0]);
        w_u32(entry + 8, values[1]);
        w_u32(entry + 24, values[2]);
        w_u32(entry + 28, values[3]);
    }
    cursor += sub_8005F104(cursor, values);
    for (player = 0; player < r_s32(r_u32(0x8005B2F8) + 48); ++player)
    {
        entry = 0x800D6198 + 224 * (uint32_t)player + 160;
        (void)sub_8001E100(entry, 0, 32);
        w_u32(entry + 4, values[0]);
        w_u32(entry + 8, values[1]);
        w_u32(entry + 24, values[2]);
        w_u32(entry + 28, values[3]);
    }
    cursor += sub_8005F104(cursor, values);
    for (player = 0; player < r_s32(r_u32(0x8005B2F8) + 48); ++player)
    {
        entry = 0x800D6198 + 224 * (uint32_t)player + 96;
        (void)sub_8001E100(entry, 0, 32);
        w_u32(entry + 4, values[0]);
        w_u32(entry + 8, values[1]);
        w_u32(entry + 24, values[2]);
        w_u32(entry + 28, values[3]);
    }
    cursor += sub_8005F104(cursor, values);
    for (player = 0; player < r_s32(r_u32(0x8005B2F8) + 48); ++player)
        for (field = 0; field < 5; ++field)
            w_u32(0x800D6358 + 72 * (uint32_t)player + 8 + 4 * field, values[field]);
    cursor += sub_8005F104(cursor, values);
    for (player = 0; player < r_s32(r_u32(0x8005B2F8) + 48); ++player)
        for (field = 0; field < 5; ++field)
            w_u32(0x800D6358 + 72 * (uint32_t)player + 44 + 4 * field, values[field]);
    for (player = 0; player < r_s32(r_u32(0x8005B2F8) + 48); ++player)
    {
        cursor += sub_8005F104(cursor, values);
        w_u16(0x8005B258 + 4 * (uint32_t)player, values[0]);
        w_u16(0x8005B258 + 4 * (uint32_t)player + 2, values[1]);
        cursor += sub_8005F104(cursor, values);
        w_u16(0x8005B2C8 + 4 * (uint32_t)player, values[0]);
        w_u16(0x8005B2C8 + 4 * (uint32_t)player + 2, values[1]);
        cursor += sub_8005F104(cursor, values);
        w_u16(0x8005B2C0 + 4 * (uint32_t)player, values[0]);
        w_u16(0x8005B2C0 + 4 * (uint32_t)player + 2, values[1]);
        cursor += sub_8005F104(cursor, values);
        w_u16(0x8005B238 + 4 * (uint32_t)player, values[0]);
        w_u16(0x8005B238 + 4 * (uint32_t)player + 2, values[1]);
    }
    cursor += sub_8005F104(cursor, values);
    w_u32(0x8005B388, values[0]);
    for (index = 0; (int32_t)index < r_s32(0x8005B388); ++index)
    {
        cursor += sub_8005F104(cursor, values);
        entry = 0x800D49B0 + 8 * index;
        w_u8(entry + 4, values[0]);
        w_u8(entry + 5, values[1]);
        w_u16(entry + 6, values[2]);
    }
    for (index = 0; index < 62; ++index)
    {
        cursor += sub_8005F104(cursor, values);
        entry = 0x800D45D0 + 16 * index;
        w_u8(entry + 4, values[0]);
        w_u8(entry + 5, values[1]);
        w_u8(entry + 8, values[0] + values[2] - 1);
        w_u8(entry + 9, values[1]);
        w_u8(entry + 12, values[0]);
        w_u8(entry + 13, values[1] + values[3] - 1);
        w_u8(entry + 14, values[0] + values[2] - 1);
        w_u8(entry + 15, values[1] + values[3] - 1);
        w_u8(entry + 2, values[2]);
        w_u8(entry + 3, values[3]);
        w_u16(entry, values[4]);
    }
    for (player = 0; player < r_s32(r_u32(0x8005B2F8) + 48); ++player)
    {
        base = r_u32(0x8005ACEC + 4 * (uint32_t)player);
        for (row = 0; row < 110; ++row)
        {
            cursor += sub_8005F104(cursor, values);
            entry = base + 36 * row;
            w_u16(entry + 24, values[0]);
            w_u8(entry + 33, 255);
            w_u16(entry + 34, 0);
            w_u16(entry + 26, values[1]);
            if (!values[4])
                w_u8(entry + 33, values[2]);
            if (values[4] != 2)
                w_u16(entry + 34, values[3] & 1);
            w_u8(entry + 32, values[4]);
        }
    }
    (void)sub_800144B8(buffer);
    return 1;
}

uint32_t sub_8005EF48(void)
{
    char path[128];
    uint32_t file_name;
    uint32_t buffer;
    uint32_t capacity;
    uint32_t bytes;
    int length;
    PSX_RECT rectangle;
    FUNCTION_MARKER(0x8005EF48, "RASHCDI.BIN");
    file_name = r_u32(r_u32(0x8005B2F8) + 48) == 1 ? 0x8005B6EC : 0x8005B6E0;
    length = snprintf(path, sizeof(path), "%s\\%s", (const char *)xport_guest_cptr(0x8005B700, 1),
        (const char *)xport_guest_cptr(file_name, 1));
    if (length < 0 || length >= (int)sizeof(path))
        abort();
    rectangle.x = 960;
    rectangle.y = 0;
    rectangle.w = 64;
    rectangle.h = 256;
    if (!sub_80030810(&buffer, &capacity))
        return 0xFFFFFFFF;
    if ((int32_t)sub_80014680(path, r_u32(0x8005AD5C), xport_guest_ptr(buffer, capacity), &bytes) < 0)
        return 0xFFFFFFFF;
    rectangle.h = 216;
    (void)LoadImagePSX(&rectangle, (uint32 *)xport_guest_ptr(buffer, 64 * 216 * 2));
    return (uint32_t)DrawSync(0);
}

uint32_t sub_8005F9EC(uint32_t palette, int32_t index)
{
    uint32_t x;
    uint32_t y;
    uint32_t result;
    FUNCTION_MARKER(0x8005F9EC, "RASHCDI.BIN");
    x = r_u16(0x8005B2B4) + (((uint32_t)index & 3) << 6);
    w_u16(palette, (((r_u8(palette + 4) >> 2) + 960) & 1023) >> 6);
    x = (uint32_t)(int32_t)(int16_t)(x / 4 + 960);
    y = (uint32_t)(int32_t)(int16_t)(r_u16(0x8005B2BC) + (uint32_t)(index >> 2));
    result = getClut(x, y);
    w_u16(palette + 2, result);
    return result;
}

uint32_t sub_8005EFFC(uint32_t packet, uint32_t palette)
{
    uint32_t result;
    FUNCTION_MARKER(0x8005EFFC, "RASHCDI.BIN");
    w_u16(packet + 10, r_u16(palette));
    w_u16(packet + 6, r_u16(palette + 2));
    w_u8(packet + 4, r_u8(packet + 4) + r_u8(palette + 4));
    w_u8(packet + 5, r_u8(packet + 5) + r_u8(palette + 5));
    w_u8(packet + 8, r_u8(packet + 8) + r_u8(palette + 4));
    w_u8(packet + 9, r_u8(packet + 9) + r_u8(palette + 5));
    w_u8(packet + 12, r_u8(packet + 12) + r_u8(palette + 4));
    w_u8(packet + 13, r_u8(packet + 13) + r_u8(palette + 5));
    w_u8(packet + 14, r_u8(packet + 14) + r_u8(palette + 4));
    result = r_u8(packet + 15) + r_u8(palette + 5);
    w_u8(packet + 15, result);
    return result;
}

uint32_t sub_8005FA88(void)
{
    uint32_t name;
    uint32_t entry;
    uint32_t base;
    uint32_t index;
    uint32_t state;
    uint32_t offset;
    uint32_t x;
    uint32_t y;
    uint32_t packet_x;
    uint32_t packet_y;
    int32_t player;
    int32_t sprite;
    char path[136];
    int length;
    FUNCTION_MARKER(0x8005FA88, "RASHCDI.BIN");
    if (!r_u32(0x8005ACD8))
        return 0;
    if (r_u32(r_u32(0x8005B2F8) + 48) == 1)
        name = 0x8005B708;
    else if (r_u32(0x8005AD14) == 2)
        name = 0x8005B714;
    else
        name = r_u32(0x8005AD14) ? 0x8005B72C : 0x8005B720;
    length = snprintf(path, sizeof(path), "%s\\%s", (const char *)xport_guest_cptr(0x8005B700, 1),
        (const char *)xport_guest_cptr(name, 1));
    if (length < 0 || length >= (int)sizeof(path))
        abort();
    (void)sub_8005F1E4(path);
    (void)sub_8005EF48();
    for (index = 0; (int32_t)index < r_s32(0x8005B388); ++index)
        (void)sub_8005F9EC(0x800D49B0 + 8 * index, (int32_t)index);
    for (index = 0; index < 62; ++index)
    {
        entry = 0x800D45D0 + 16 * index;
        (void)sub_8005EFFC(entry, 0x800D49B0 + 8 * r_u16(entry));
    }
    for (player = 0; player < r_s32(r_u32(0x8005B2F8) + 48); ++player)
    {
        offset = 4 * (uint32_t)player;
        base = r_u32(0x8005ACEC + offset);
        for (index = 0; index < 110; ++index)
        {
            entry = base + 36 * index;
            sprite = r_s8(entry + 33);
            if (sprite >= 0)
                (void)sub_80013AF8(entry, 0x800D45D0 + 16 * (uint32_t)sprite);
        }
        state = r_u32(0x8005B2F8);
        name = r_u8(state + 4);
        if (name == 33 || ((name & 1) && r_s32(state + offset + 72) >= 18))
        {
            x = (uint32_t)r_s16(0x8005B258 + offset) - r_u16(base + 3444);
            y = (uint32_t)r_s16(0x8005B258 + offset + 2) - r_u16(base + 3446);
            for (index = 95; index < 100; ++index)
            {
                entry = base + 36 * index;
                packet_x = r_u16(entry + 24) + x;
                packet_y = r_u16(entry + 26) + y;
                w_u16(entry + 24, packet_x);
                w_u16(entry + 26, packet_y);
            }
            if (r_u8(r_u32(0x8005B2F8) + 4) != 33)
            {
                x = (uint32_t)r_s16(0x8005B2C8 + offset) - r_u16(base + 3048);
                y = (uint32_t)r_s16(0x8005B2C8 + offset + 2) - r_u16(base + 3050);
                for (index = 84; index < 94; ++index)
                {
                    entry = base + 36 * index;
                    packet_x = r_u16(entry + 24) + x;
                    packet_y = r_u16(entry + 26) + y;
                    w_u16(entry + 24, packet_x);
                    w_u16(entry + 26, packet_y);
                }
                x = (uint32_t)r_s16(0x8005B2C0 + offset) - r_u16(base + 1932);
                y = (uint32_t)r_s16(0x8005B2C0 + offset + 2) - r_u16(base + 1934);
                for (index = 53; index < 60; ++index)
                {
                    entry = base + 36 * index;
                    packet_x = r_u16(entry + 24) + x;
                    packet_y = r_u16(entry + 26) + y;
                    w_u16(entry + 24, packet_x);
                    w_u16(entry + 26, packet_y);
                }
            }
            y = (uint32_t)r_s16(0x8005B238 + offset + 2);
            w_u16(base + 3768, (uint32_t)r_s16(0x8005B238 + offset));
            w_u16(base + 3770, y);
        }
    }
    w_u32(0x8005ACE0, 1);
    return 1;
}

uint32_t sub_800635D0(void)
{
    FUNCTION_MARKER(0x800635D0, "RASHCDI.BIN");
    (void)sub_8005C45C(r_s32(r_u32(0x8005B2F8) + 60));
    (void)sub_800619A0();
    (void)sub_8005C630(r_s32(r_u32(0x8005B2F8) + 64));
    (void)sub_8006068C();
    (void)sub_800627F8();
    (void)sub_8005FA88();
    w_u32(0x8005AD2C, (uint32_t)(int32_t)(int8_t)sub_80061528((const char *)xport_guest_cptr(0x8005B7CC, 1), 1, 1));
    (void)sub_80063D94((const char *)xport_guest_cptr(0x8005B7DC, 1));
    return sub_800674F8();
}

uint32_t sub_8005D130(uint32_t owner)
{
    uint32_t index = 0;
    uint32_t offset = 0;
    uint32_t entry;
    uint32_t result = 0x800CF5D8;
    FUNCTION_MARKER(0x8005D130, "RASHCDI.BIN");
    w_u32(owner + 24, 10);
    w_u32(owner, 0);
    w_u32(owner + 4, 0);
    w_u32(owner + 12, 0);
    w_u32(owner + 8, 0);
    w_u32(owner + 16, result);
    w_u32(owner + 20, 0);
    if (r_s32(owner + 24) <= 0)
        return result;
    do
    {
        entry = r_u32(owner + 16) + offset;
        ++index;
        w_u8(entry + 1, 0);
        w_u16(entry + 2, 0);
        w_u32(entry + 4, 0);
        w_u8(entry, 0);
        result = (int32_t)index < r_s32(owner + 24);
        offset += 12;
    } while (result);
    return result;
}

uint32_t sub_8005D1A0(uint32_t owner, int32_t count)
{
    uint32_t objects;
    uint32_t controls;
    uint32_t index = 0;
    uint32_t object_offset = 0;
    uint32_t control_offset = 0;
    uint32_t entry;
    uint32_t control;
    uint32_t result;
    FUNCTION_MARKER(0x8005D1A0, "RASHCDI.BIN");
    objects = sub_8001447C(2108 * (uint32_t)count, 0);
    w_u32(owner, objects);
    controls = sub_8001447C(60 * (uint32_t)count, 0);
    w_u32(owner + 12, count);
    w_u32(owner + 4, controls);
    w_u32(owner + 8, 0);
    result = controls;
    if (count <= 0)
        return result;
    if (!objects || !controls)
        abort();
    do
    {
        ++index;
        control = r_u32(owner + 4) + control_offset;
        entry = r_u32(owner) + object_offset;
        control_offset += 60;
        w_u32(entry + 36, 0);
        w_u8(control + 1, 5);
        w_u32(entry + 4, control);
        w_u32(entry + 8, 5);
        w_u32(entry + 1760, 0xFFFFFFFF);
        w_u32(entry + 1756, 0);
        w_u32(entry + 44, 0);
        result = (int32_t)index < r_s32(owner + 12);
        object_offset += 2108;
    } while (result);
    return result;
}

uint32_t sub_800653E8(const char *name)
{
    char path[24];
    uint32_t buffer = 0;
    uint32_t bytes;
    uint32_t index;
    uint32_t entry;
    uint32_t first;
    uint32_t second;
    int length;
    FUNCTION_MARKER(0x800653E8, "RASHCDI.BIN");
    length = snprintf(path, sizeof(path), "%s.bin", name);
    if (length < 0 || length >= (int)sizeof(path))
        abort();
    if ((int32_t)sub_80014654(path, r_u32(0x8005AD5C), &buffer, &bytes) < 0 || !buffer)
        return 0;
    if (bytes < 480)
    {
        (void)sub_800144B8(buffer);
        return 0;
    }
    for (index = 0; index < 40; ++index)
    {
        entry = buffer + 12 * index;
        first = r_u32(entry + 4) + buffer;
        second = r_u32(entry + 8) + buffer;
        w_u32(entry + 4, first);
        w_u32(entry + 8, second);
    }
    return buffer;
}

uint32_t sub_8005C7F0(int32_t selection)
{
    char digit[2];
    char geometry[104];
    char texture[104];
    int length;
    FUNCTION_MARKER(0x8005C7F0, "RASHCDI.BIN");
    if ((r_u32(r_u32(0x8005B2F8) + 4) & 0x18) == 8)
        selection = 0;
    digit[0] = (char)((uint32_t)(selection % 10) + 48);
    digit[1] = 0;
    length = snprintf(geometry, sizeof(geometry), "%s%s%s%s", (const char *)xport_guest_cptr(0x80052450, 1),
        (const char *)xport_guest_cptr(0x8005B69C, 1), digit, (const char *)xport_guest_cptr(0x8005B630, 1));
    if (length < 0 || length >= (int)sizeof(geometry))
        abort();
    length = snprintf(texture, sizeof(texture), "%s%s%s%s", (const char *)xport_guest_cptr(0x80052450, 1),
        (const char *)xport_guest_cptr(0x8005B69C, 1), digit, (const char *)xport_guest_cptr(0x8005B638, 1));
    if (length < 0 || length >= (int)sizeof(texture))
        abort();
    return sub_8005CA10(geometry, texture) == 0;
}

uint32_t sub_8005CEE0(uint32_t player)
{
    uint32_t slot;
    uint32_t entry;
    uint32_t index;
    static const uint32_t zero_offsets[6] = {94, 96, 98, 102, 104, 106};
    FUNCTION_MARKER(0x8005CEE0, "RASHCDI.BIN");
    slot = 0x8005AEC0 + 4 * player;
    w_u32(slot, 0x800D82B0);
    w_u32(0x8005AEC4, 0x800D8330);
    w_u32(r_u32(slot), 0);
    w_u32(r_u32(slot) + 4, 237);
    w_u32(r_u32(slot) + 8, 40);
    w_u32(r_u32(slot) + 12, 32767);
    entry = r_u32(slot);
    w_u32(entry + 52, 64);
    w_u32(entry + 48, 64);
    w_u32(entry + 44, 64);
    for (index = 0; index < 6; ++index)
        w_u16(r_u32(slot) + zero_offsets[index], 0);
    w_u16(r_u32(slot) + 92, 4096);
    w_u16(r_u32(slot) + 100, 4096);
    w_u16(r_u32(slot) + 108, 4096);
    w_u32(r_u32(slot) + 112, 0);
    w_u32(r_u32(slot) + 116, 0);
    w_u32(r_u32(slot) + 120, 0);
    sub_8002201C(r_u32(r_u32(slot) + 4));
    return sub_80022064(384, 240);
}

int32_t sub_8006AD4C(void)
{
    uint32_t random;
    uint32_t count;
    uint32_t selected;
    uint32_t candidate;
    uint32_t preferences = 0;
    uint32_t index;
    uint32_t variant;
    uint32_t matches;
    int32_t value;
    FUNCTION_MARKER(0x8006AD4C, "RASHCDI.BIN");
    if (r_s8(0x8006B984))
    {
        random = sub_8001FC58();
        count = (uint32_t)r_s8(0x8006B984);
        selected = count ? random % count : random;
    }
    else
        selected = 0;
    w_u32(0x8006EB40, selected);
    if (r_s8(0x8005B328) >= 0)
    {
        while (preferences < 2 && r_s8(0x8005B328 + 4 * preferences) >= 0)
            ++preferences;
        selected = r_u32(0x8006EB40);
        candidate = selected + 1;
        if (candidate == (uint32_t)r_s8(0x8006B984))
            candidate = 0;
        while (candidate != r_u32(0x8006EB40))
        {
            matches = 0;
            for (index = 0; index < preferences; ++index)
                for (variant = 0; variant < 2; ++variant)
                    if (r_s8(0x8005B328 + 4 * index) == r_s8(0x8006B985 + 2 * candidate + variant))
                    {
                        ++matches;
                        break;
                    }
            if (matches == preferences)
            {
                w_u32(0x8006EB40, candidate);
                break;
            }
            ++candidate;
            if (candidate == (uint32_t)r_s8(0x8006B984))
                candidate = 0;
        }
    }
    value = r_s8(0x8006B985 + 2 * r_u32(0x8006EB40));
    if (value < 1)
        return 1;
    if (value > 5)
        return 5;
    return value;
}

uint32_t sub_8006ACD0(uint32_t route, uint32_t distance, uint32_t offset)
{
    uint32_t segment;
    FUNCTION_MARKER(0x8006ACD0, "RASHCDI.BIN");
    w_u32(0x800CF580, distance);
    w_u32(0x800CF578, route & 65535);
    w_u32(0x800CF57C, offset);
    segment = sub_80039DFC(0, ((0x800CF578) ? (const uint32_t[3]){r_u32(0x800CF578), 0, (uint32_t)r_u16((0x800CF578) + 10) << 16} : NULL));
    uint32_t geometry_result = 0;
    {
        const uint32_t route_position[3] = {r_u32(0x800CF578), r_u32(0x800CF578 + 4), r_u32(0x800CF578 + 8)};
        uint32_t route_geometry[8] = {r_u32(0x800CF588), r_u32(0x800CF588 + 4), r_u32(0x800CF588 + 8), r_u32(0x800CF588 + 12), r_u32(0x800CF588 + 16), r_u32(0x800CF588 + 20), r_u32(0x800CF588 + 24), r_u32(0x800CF588 + 28)};
        geometry_result = sub_8003A700(segment, route_position, route_geometry);
        w_u32(0x800CF588, route_geometry[0]);
        w_u32(0x800CF588 + 4, route_geometry[1]);
        w_u32(0x800CF588 + 8, route_geometry[2]);
        w_u32(0x800CF588 + 12, route_geometry[3]);
        w_u32(0x800CF588 + 16, route_geometry[4]);
        w_u32(0x800CF588 + 20, route_geometry[5]);
        w_u32(0x800CF588 + 24, route_geometry[6]);
        w_u32(0x800CF588 + 28, route_geometry[7]);
    }
    return geometry_result;
}

uint32_t sub_80068470(const uint8_t data[280])
{
    uint32_t index;
    FUNCTION_MARKER(0x80068470, "RASHCDI.BIN");
    (void)sub_8001E0B4(xport_guest_ptr(0x8005B328, 8), data + 220, 8);
    (void)sub_8001E0B4(xport_guest_ptr(0x8006B8B4, 4), data + 228, 4);
    for (index = 0; index < 3; ++index)
        (void)sub_8001E0B4(xport_guest_ptr(0x8006EB10 + 16 * index, 16), data + 232 + 16 * index, 16);
    return 0;
}

uint32_t sub_8006AEF4(void)
{
    uint32_t model_ids[18];
    uint32_t preference;
    uint32_t group;
    uint32_t kind;
    uint32_t forced;
    uint32_t descriptor;
    uint32_t object;
    uint32_t profile;
    uint32_t index;
    uint32_t entry;
    uint32_t random;
    uint32_t start;
    uint32_t divisor;
    uint32_t remainder;
    uint32_t models;
    uint32_t capacity;
    uint32_t buffer;
    uint32_t source;
    uint32_t variant;
    uint32_t result;
    int32_t quantity;
    int32_t selection;
    int32_t descriptor_count;
    int32_t variants;
    int32_t model;
    int32_t height;
    FUNCTION_MARKER(0x8006AEF4, "RASHCDI.BIN");
    for (preference = 0; preference < 2; ++preference)
        if (r_s8(0x8005B328 + 4 * preference) < 0)
            break;
    if (preference < 2)
    {
        entry = 0x8005B328 + 4 * preference;
        w_u8(entry, 9);
        w_u8(entry + 1, 156);
        w_u16(entry + 2, 0);
    }
    w_u32(0x8005B260, 0);
    descriptor_count = r_s32(0x8006B8B4);
    if (descriptor_count > 3)
        abort();
    capacity = descriptor_count > 0 ? 32 * (uint32_t)descriptor_count : 0;
    if (capacity < 48)
        capacity = 48;
    buffer = sub_8001447C(capacity, 0);
    if (!buffer)
        abort();
    w_u32(0x8005B35C, buffer);
    w_u32(0x8005B2AC, 0);
    for (group = 0; group < 2; ++group)
    {
        kind = (uint32_t)r_s8(0x8006B985 + 2 * r_u32(0x8006EB40) + group);
        if (kind >= 6)
            break;
        height = 0;
        forced = 0;
        selection = -1;
        for (preference = 0; preference < 2; ++preference)
        {
            entry = 0x8005B328 + 4 * preference;
            if (r_s8(entry) < 0)
                break;
            if ((uint32_t)r_s8(entry) == kind)
            {
                quantity = r_s8(entry + 1);
                forced = quantity < 0;
                selection = quantity < 0 ? -quantity : quantity;
                height = r_s16(entry + 2);
                break;
            }
        }
        if (forced)
        {
            if (selection < 6)
            {
                models = r_u32(0x8005B260);
                if (models >= 18)
                    abort();
                model_ids[models] = (uint32_t)selection;
                w_u8(0x8005B328 + 4 * preference + 1, r_u8(0x8005B260));
                w_u32(0x8005B260, r_u32(0x8005B260) + 1);
            }
            continue;
        }
        descriptor_count = r_s32(0x8006B8B4);
        if (descriptor_count > 3)
            abort();
        for (descriptor = 0; (int32_t)descriptor < descriptor_count; ++descriptor)
        {
            index = r_u32(0x8005B2AC);
            if (16 * index >= capacity)
                abort();
            object = r_u32(0x8005B35C) + 16 * index;
            profile = r_u32(0x8006B8B4) ? 0x8006EB10 + 16 * descriptor : 0;
            w_u8(object + 8, kind ? 0 : 9);
            w_u8(object + 12, 1);
            quantity = height;
            if (profile && r_s16(profile + 14) >= 0)
                quantity = r_u16(profile + 14);
            w_u16(object + 14, quantity);
            if (profile)
            {
                w_u32(object, r_u32(profile));
                random = sub_8001FC58();
                start = r_u32(profile + 4);
                divisor = (uint32_t)((int32_t)(r_u32(profile + 8) - start) >> 16);
                remainder = divisor ? random % divisor : random;
                w_u32(object + 4, ((uint32_t)((int32_t)start >> 16) + remainder) << 16);
            }
            else
            {
                entry = r_u32(0x8005B38C);
                w_u32(object, r_u32(entry + 360));
                w_u32(object + 4, r_u32(entry + 368));
            }
            variants = r_s8(0x8006B995 + kind);
            if (profile && r_s16(profile + 12) >= 0)
                selection = r_s16(profile + 12);
            if (selection < 0 || selection >= variants)
            {
                random = sub_8001FC58();
                divisor = (uint32_t)variants;
                selection = (int32_t)(divisor ? random % divisor : random);
            }
            for (variant = 0; variant < 3; ++variant)
            {
                source = 0x8006B99B + 30 * kind + 3 * (uint32_t)selection + variant;
                model = r_s8(source);
                if (model < 0)
                {
                    w_u8(object + variant + 9, 255);
                    break;
                }
                models = r_u32(0x8005B260);
                if (models >= 18)
                    abort();
                w_u8(object + variant + 9, r_u8(0x8005B260));
                model_ids[models] = (uint32_t)r_s8(source);
                w_u32(0x8005B260, models + 1);
            }
            w_u32(0x8005B2AC, r_u32(0x8005B2AC) + 1);
        }
    }
    models = r_u32(0x8005B260);
    buffer = sub_8001447C(312 * (models < 6 ? 6 : models), 0);
    if (!buffer)
        abort();
    w_u32(0x8005B24C, buffer);
    for (index = 0; (int32_t)index < r_s32(0x8005B260); ++index)
    {
        model = (int32_t)model_ids[index];
        entry = r_u32(0x8005B24C) + 312 * index;
        if (model >= 40)
            w_u8(entry, 255);
        else
            (void)sub_8001E0B4(xport_guest_ptr(entry, 312), xport_guest_cptr(0x8006BA50 + 312 * (uint32_t)model, 312), 312);
    }
    for (; index < 6; ++index)
        w_u8(r_u32(0x8005B24C) + 312 * index, 0);
    w_u32(0x8005B314, 0);
    buffer = sub_8001447C(840, 0);
    w_u32(0x8005B304, buffer);
    result = sub_8001447C(72, 0);
    w_u32(0x8005B250, result);
    if (!buffer || !result)
        abort();
    entry = r_u32(0x8005B304) + 560;
    for (index = 0; index < 3; ++index, entry -= 280)
        w_u16(entry + 172, 0);
    return result;
}
