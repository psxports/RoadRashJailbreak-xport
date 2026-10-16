#include "rrj_debug.h"
#include "game_main.h"
#include "native_game_api.h"
#include "race_bodyless_batch_002.h"
#include "race_leaf.h"
#include "race_leaf_batch_004.h"
#include "draft_quick.h"
#include "race_audio_frontier.h"
#include "race_bodyless_batch_004.h"
#include "race_pause.h"
#include "race_pause_frontier.h"
#include "xport.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint32_t *sub_8006AD24(uint32_t output[8])
{
    uint32_t i;
    FUNCTION_MARKER(0x8006AD24, "RASHCDI.BIN");
    for (i = 0; i < 8; ++i)
        output[i] = r_u32(0x800CF588 + 4 * i);
    return output;
}

uint32_t sub_800644C8(uint32_t actor)
{
    uint32_t result = 0x80060000;
    FUNCTION_MARKER(0x800644C8, "RASHCDI.BIN");
    if (!actor)
        return result;
    result = r_u16(actor + 172) < r_u32(r_u32(0x8005B2F8) + 48);
    if (result)
        return result;
    result = r_u8(r_u32(actor + 1084) + 1) & 15;
    if (result != 2)
        return result;
    w_u8(0x800D8703, 1);
    w_u8(0x800D86FC, r_u8(r_u32(actor + 1084) + 12));
    w_u8(0x800D86FD, r_u8(r_u32(actor + 1084) + 13));
    w_u8(0x800D86FF, r_u8(r_u32(actor + 1084) + 15));
    w_u8(0x800D86FE, r_u8(r_u32(actor + 1084) + 14));
    w_u8(0x800D8700, r_u8(r_u32(actor + 1084) + 2));
    w_u8(0x800D8701, r_u8(r_u32(actor + 1084) + 61));
    w_u8(0x800D8702, r_u8(r_u32(actor + 1084) + 36));
    w_u8(0x800D8706, r_u8(r_u32(actor + 1084) + 46));
    w_u16(0x800D8704, r_u16(r_u32(actor + 1084) + 44));
    result = r_u32(r_u32(actor + 556) + 224);
    w_u32(0x800D8708, 0);
    w_u32(0x800D86F8, result);
    return result;
}

uint32_t sub_80063158(void)
{
    uint32_t result, data, bytes, cursor, end, pointer;
    FUNCTION_MARKER(0x80063158, "RASHCDI.BIN");
    result = r_u32(0x8005B428);
    if (result)
        return result;
    result = sub_80014654("DATA\\ANIMNOIZ.DAT", r_u32(0x8005AD5C), &data, &bytes);
    if ((int32_t)result <= 0)
        return result;
    cursor = data + 8;
    end = data + r_u32(data + 4) + 8;
    w_u32(0x8005B428, data);
    w_u32(0x8005B408, cursor);
    while ((int32_t)cursor < (int32_t)end)
    {
        pointer = r_u32(cursor + 8);
        if (pointer)
            w_u32(cursor + 8, pointer + data);
        cursor += 12;
    }
    end = cursor + r_u32(cursor + 4) + 8;
    cursor += 8;
    w_u32(0x8005B3E4, cursor);
    result = (int32_t)cursor < (int32_t)end;
    while (result)
    {
        pointer = r_u32(cursor);
        if (pointer)
            w_u32(cursor, pointer + r_u32(0x8005B428));
        cursor += 4;
        result = (int32_t)cursor < (int32_t)end;
    }
    return result;
}

uint32_t sub_80069618(const char *path)
{
    uint8_t data[224];
    uint32_t handle, bytes, result, i;
    FUNCTION_MARKER(0x80069618, "RASHCDI.BIN");
    handle = sub_8001458C(path, r_u32(0x8005AD5C));
    if ((int32_t)handle < 0)
    {
        RRJ_DEBUG_FPRINTF(stderr, "Missing camera resource: %s\n", path);
        abort();
    }
    bytes = sub_80014780(handle, data, sizeof(data));
    result = sub_8001460C(handle);
    if (bytes != sizeof(data))
    {
        RRJ_DEBUG_FPRINTF(stderr, "Truncated camera resource: %s\n", path);
        abort();
    }
    for (i = 0; i < sizeof(data); ++i)
        w_u8(0x800CD7B8 + i, data[i]);
    return result;
}

uint32_t sub_80067564(void)
{
    char basename[128], path[128];
    uint32_t suffix, kind, owner, player = 0, option = 0, secondary, result, i;
    int32_t length;
    FUNCTION_MARKER(0x80067564, "RASHCDI.BIN");
    w_u32(0x800CDAB4, r_u32(0x800CDAB8));
    owner = r_u32(0x8005B2F8);
    kind = r_u32(r_u32(0x8005B38C) + 180);
    if (r_u32(owner + 48) == 2)
        suffix = kind - 6 < 3 || kind - 15 < 3 ? 0x8005B900 : 0x8005B904;
    else
        suffix = kind - 6 < 3 || kind - 15 < 3 ? 0x8005B908 : 0x8005B90C;
    length = snprintf(basename, sizeof(basename), "%s%s", (const char *)xport_guest_cptr(0x800524C8, 1), (const char *)xport_guest_cptr(suffix, 1));
    if (length < 0 || (uint32_t)length >= sizeof(basename))
        abort();
    for (i = 0; i <= (uint32_t)length; ++i)
        w_u8(0x800D4C20 + i, (uint8_t)basename[i]);
    length = snprintf(path, sizeof(path), "%s.ca", basename);
    if (length < 0 || (uint32_t)length >= sizeof(path))
        abort();
    (void)sub_80069618(path);
    w_u32(0x800CDBA0, 0);
    if (r_u32(0x8005B220))
        option = r_u32(0x800D83B0) != 0;
    owner = r_u32(0x8005B2F8);
    if (r_u8(owner + 4) & 1)
        player = r_u32(0x8005B3A0) + 1096 * r_u8(owner + 6);
    (void)sub_8002F308(player ? player : r_u32(0x8005B38C), 0x800CD898, 159, 0, option);
    secondary = r_u32(0x8005B21C);
    w_u32(0x800CD8C4, 0);
    result = secondary;
    if (secondary)
    {
        (void)sub_8002F308(player ? player : secondary, 0x800CDD04, 158, 0, option);
        w_u32(0x800CD8C8, 0x8000);
        w_u32(0x800CDD30, 0x8000);
        w_u32(0x800CDD34, 0);
        result = r_u32(0x800CDF28) | 0x1000000;
        w_u32(0x800CDF28, result);
    }
    return result;
}

uint32_t sub_800696B8(void)
{
    uint32_t result, actor, owner, speed, table, i;
    uint16_t direction[3];
    uint32_t velocity[3];
    int32_t remaining;
    FUNCTION_MARKER(0x800696B8, "RASHCDI.BIN");
    result = r_u32(0x800CE4DC);
    remaining = r_s32(result);
    actor = r_u32(0x800CE4D0);
    if (remaining < 0)
        return result;
    do
    {
        owner = r_u32(0x8005B2F8);
        if (r_u16(actor + 172) < r_u32(owner + 48) && (r_u8(r_u32(actor + 1084) + 1) & 15) == 2)
            speed = r_u32(0x8005B2B0) ? r_u32(0x80053048 + 4 * r_u32(owner + 60)) : 0;
        else
        {
            table = r_u32(0x8005B2B0) ? 0x80053054 : 0x8005303C;
            speed = r_u32(table + 4 * r_u32(r_u32(0x8005B2F8) + 60));
        }
        w_u32(actor + 924, speed);
        speed = r_u32(actor + 924);
        w_u32(actor + 480, speed);
        w_u32(actor + 576, speed);
        for (i = 0; i < 3; ++i)
            direction[i] = r_u16(actor + 450 + 2 * i);
        (void)sub_8002EE50(speed, direction, velocity);
        for (i = 0; i < 3; ++i)
            w_u32(actor + 456 + 4 * i, velocity[i]);
        result = r_u32(0x800CE4D4);
        --remaining;
        actor += result;
    } while (remaining >= 0);
    return result;
}

uint32_t sub_80068740(void)
{
    char path[128];
    char *extension;
    uint8_t *data;
    uint32_t guest_data, bytes, load_result, cursor = 0, counts[5];
    uint32_t stage, group, total, roster, record, i;
    int32_t length;
    FUNCTION_MARKER(0x80068740, "RASHCDI.BIN");
    w_u32(0x8005B344, 0);
    w_u32(0x8005B340, 0);
    w_u32(0x8005B20C, 0);
    w_u32(0x8005B208, 0);
    if (r_u8(r_u32(0x8005B2F8) + 4) != 44)
        return 44;
    length = snprintf(path, sizeof(path), "%s", (const char *)xport_guest_cptr(0x80052414, 1));
    if (length < 0 || (uint32_t)length >= sizeof(path))
        abort();
    extension = strchr(path, '.');
    if (!extension || extension - path < 3)
        abort();
    extension -= 3;
    extension[0] = 'J';
    extension[1] = 'B';
    extension[2] = 'A';
    RRJ_DEBUG_FPRINTF(stdout, "Opening %s to read escape scene positions\n", path);
    load_result = sub_80014654(path, r_u32(0x8005AD5C), &guest_data, &bytes);
    if ((int32_t)load_result < 0 || !guest_data || !bytes)
    {
        RRJ_DEBUG_FPRINTF(stderr, "Missing escape positions: %s\n", path);
        abort();
    }
    data = malloc(bytes);
    if (!data)
        abort();
    for (i = 0; i < bytes; ++i)
        data[i] = r_u8(guest_data + i);
    (void)sub_800144B8(guest_data);
    for (stage = 0; stage < 2; ++stage)
    {
        extension[2] = (char)('A' + stage);
        total = 0;
        if (cursor > bytes || bytes - cursor < 20)
            abort();
        for (group = 0; group < 5; ++group)
        {
            counts[group] = xport_load_le32(data + cursor);
            cursor += 4;
            if ((int32_t)counts[group] < 0 || counts[group] > (0xFFFFFFFF / 12) - total)
                abort();
            total += counts[group];
            w_u32(0x8005B208 + 4 * stage, r_u32(0x8005B208 + 4 * stage) + counts[group]);
        }
        if (cursor > bytes || total > (bytes - cursor) / 12)
            abort();
        if (total)
        {
            roster = sub_8001447C(12 * total, 0);
            if (!roster)
                abort();
            w_u32(0x8005B340 + 4 * stage, roster);
            (void)sub_8001E100(roster, 0, 12 * r_u32(0x8005B208 + 4 * stage));
        }
        record = 0;
        for (group = 0; group < 5; ++group)
        {
            for (i = 0; i < counts[group]; ++i)
            {
                roster = r_u32(0x8005B340 + 4 * stage);
                w_u32(roster + 12 * record + 8, xport_load_le32(data + cursor));
                cursor += 4;
                roster = r_u32(0x8005B340 + 4 * stage);
                w_u32(roster + 12 * record, xport_load_le32(data + cursor));
                cursor += 4;
                roster = r_u32(0x8005B340 + 4 * stage);
                w_u32(roster + 12 * record + 4, xport_load_le32(data + cursor));
                cursor += 4;
                ++record;
            }
        }
    }
    free(data);
    return 0;
}

void sub_8006982C(void)
{
    uint8_t configuration[292];
    char path[128];
    uint32_t owner, extension, i, guest_data = 0, bytes = 0, result, selection, offset;
    uint32_t counter;
    FUNCTION_MARKER(0x8006982C, "RASHCDI.BIN");
    owner = r_u32(0x8005B2F8);
    if (r_u8(owner + 4) == 44)
    {
        w_u8(owner + 57, 0);
        w_u32(0x8005B2E8, r_u16(0x8005317A) < r_u16(0x80053176) ? 0xFFFFFFFF : 1);
        w_u8(r_u32(0x8005B2F8) + 10, 0);
        w_u32(0x8005ACC0, 0);
    }
    (void)sub_80069000();
    (void)sub_80068AA4();
    (void)sub_8006968C();
    extension = 0;
    for (i = 0; i < sizeof(path); ++i)
    {
        path[i] = (char)r_u8(0x80052414 + i);
        if (!path[i])
            break;
        if (path[i] == '.' && !extension)
            extension = i;
    }
    if (i == sizeof(path) || !extension || extension + 4 >= sizeof(path))
        abort();
    path[extension] = '.';
    path[extension + 1] = 'B';
    path[extension + 2] = 'I';
    path[extension + 3] = 'N';
    path[extension + 4] = 0;
    path[extension - 1] = r_u32(r_u32(0x8005B2F8) + 48) < 2 ? 'A' : 'B';
    for (i = 0; i <= extension + 4; ++i)
        w_u8(0x80052414 + i, (uint8_t)path[i]);
    result = sub_80014654(path, r_u32(0x8005AD5C), &guest_data, &bytes);
    selection = r_u32(r_u32(0x8005B2F8) + 64);
    if ((int32_t)result < 0 || !guest_data || !selection || selection > bytes / sizeof(configuration))
    {
        RRJ_DEBUG_FPRINTF(stderr, "Missing or truncated race configuration: %s\n", path);
        abort();
    }
    offset = (selection - 1) * sizeof(configuration);
    for (i = 0; i < sizeof(configuration); ++i)
        configuration[i] = r_u8(guest_data + offset + i);
    (void)sub_800144B8(guest_data);
    counter = sub_80043F00(0xF2000002) & 255;
    sub_8001FC84(3305 * counter >> 6);
    (void)sub_80067B00(configuration, sizeof(configuration));
    (void)sub_80067784();
    (void)sub_80067564();
    if (r_u8(r_u32(0x8005B2F8) + 4) & 1)
        (void)sub_800696B8();
    (void)sub_800689D0(configuration);
    (void)sub_800645E4(configuration, sizeof(configuration) - 12);
    (void)sub_80068740();
    w_u32(0x8005B2A8, 0);
    w_u32(0x8005B2A4, 0);
    w_u32(0x8005B2E0, 0);
    w_u32(0x8005B290, 0);
    w_u32(0x8005B2EC, 0);
    w_u32(0x8005B2FC, 0);
    w_u32(0x8005B210, 0);
    w_u32(0x8005B318, 0);
    w_u32(0x8005B2A0, 0);
    w_u32(0x8005B30C, 0);
    w_u32(0x8005B36C, 0xFFFF0000);
    w_u32(0x8005B368, 0xFFFF0000);
    w_u32(0x8005B284, 0xFFFFFFFF);
    w_u8(0x8005B358, 0);
}

uint32_t sub_80066E1C(uint32_t actor)
{
    uint32_t resource, variant, index, owner, players, mode, kind, reference, base;
    uint32_t level, difference, random;
    FUNCTION_MARKER(0x80066E1C, "RASHCDI.BIN");
    resource = r_u32(actor + 1084);
    variant = r_u8(resource + 38);
    if (variant >= 9)
        variant -= 8;
    owner = r_u32(0x8005B2F8);
    index = r_u16(actor + 172);
    players = r_u32(owner + 48);
    if (index < players)
    {
        mode = r_u8(owner + 4);
        if (mode & 32)
        {
            reference = 0x800D81D8 + 36 * index;
            return (uint32_t)r_s8(reference + 10) + (r_s8(reference + 9) ? 113 : 110);
        }
        if (mode & 16)
            return index + (mode & 8 ? 108 : 30);
        return 92;
    }
    if ((r_u8(resource + 1) & 15) == 2)
    {
        random = sub_8001FC58() & 7;
        level = r_u32(r_u32(0x8005B2F8) + 60);
        difference = 2 - level;
        base = (~(uint32_t)((int32_t)level >> 31) & level) + ((uint32_t)((int32_t)difference >> 31) & difference);
        return (8 * base + 66 + random) & 255;
    }
    mode = r_u8(owner + 4);
    if (mode == 44)
    {
        kind = r_u8(r_u32(r_u32(0x8005B38C) + 1084) + 1) & 15;
        base = kind == r_u8(0x8006B8A0 + index) ? (kind ? 42 : 34) : 82;
        return base - 1 + variant;
    }
    if (mode == 33)
    {
        reference = r_u32(r_u32(0x8005B38C) + 180) < 9;
        return variant + (r_u8(0x8006B8A0 + index) == reference ? 73 : 81);
    }
    if (mode == 36 || (mode & 4))
    {
        kind = r_u8(r_u32(r_u32(0x8005B38C) + 1084) + 1) & 15;
        base = kind ? 42 : 34;
        return base + variant + (r_u8(0x8006B8A0 + index) == kind ? 0xFFFFFFFF : 15);
    }
    if (mode & 1)
    {
        kind = r_u8(r_u32(r_u32(0x8005B38C) + 1084) + 1) & 15;
        reference = kind == 2 ? 1 : players < 2;
        kind = r_u8(r_u32(r_u32(0x8005B3A0) + 1096 * reference + 1084) + 1) & 15;
        base = kind ? 42 : 34;
        return base + variant + (r_u8(0x8006B8A0 + index) == kind ? 0xFFFFFFFF : 15);
    }
    return variant + (r_u8(0x8006B8A0 + index) ? 41 : 33);
}

uint32_t sub_8005BB50(uint32_t chunk)
{
    uint32_t offset, result;
    FUNCTION_MARKER(0x8005BB50, "RASHCDI.BIN");
    offset = r_u32(chunk + 20);
    result = offset ? chunk + offset : 0;
    w_u32(chunk + 20, result);
    return result;
}

uint32_t sub_8005BAF8(uint32_t chunk)
{
    uint32_t index;
    FUNCTION_MARKER(0x8005BAF8, "RASHCDI.BIN");
    (void)sub_8005BB50(chunk);
    index = r_u8(0x8006B890);
    if (index >= (0x8006B890 - 0x8006B6A0) / 4)
    {
        RRJ_DEBUG_FPRINTF(stderr, "Animation pointer table overflow\n");
        abort();
    }
    w_u8(chunk + 14, index);
    w_u32(0x8006B6A0 + 4 * index, chunk);
    w_u32(0x8006B890, r_u32(0x8006B890) + 1);
    return 0;
}

uint32_t sub_8005BB6C(uint32_t entry, uint32_t data, uint32_t bytes)
{
    uint32_t cursor = data, span, count, pointers, i;
    FUNCTION_MARKER(0x8005BB6C, "RASHCDI.BIN");
    w_u32(0x8006B890, 0);
    if (bytes)
    {
        do
        {
            if (bytes - (cursor - data) < 8)
                abort();
            span = r_u32(cursor + 4);
            if (span < 8 || span > bytes - (cursor - data))
                abort();
            if (r_u32(cursor) == 0x33444D44)
            {
                if (span < 24)
                    abort();
                if (sub_8005BAF8(cursor) == 0xFFFFFFFF)
                    return 0xFFFFFFFF;
            }
            cursor += span;
        } while (cursor - data < bytes);
    }
    w_u8(entry + 1, r_u8(entry + 1) | 1);
    count = r_u32(0x8006B890);
    w_u32(entry + 8, data);
    if ((int32_t)count > 0)
    {
        w_u16(entry + 2, r_u16(0x8006B890));
        pointers = sub_8001447C(4 * count, 0);
        if (!pointers)
            abort();
        w_u32(entry + 4, pointers);
        for (i = 0; i < r_u16(entry + 2); ++i)
            w_u32(r_u32(entry + 4) + 4 * i, r_u32(0x8006B6A0 + 4 * i));
    }
    else
    {
        w_u16(entry + 2, 0);
        w_u32(entry + 4, 0);
    }
    return 0;
}

uint32_t sub_8005BCDC(uint32_t owner, const char *path)
{
    uint32_t entry = 0, cursor, data = 0, bytes = 0, i;
    int32_t count;
    FUNCTION_MARKER(0x8005BCDC, "RASHCDI.BIN");
    count = r_s32(owner + 24);
    cursor = r_u32(owner + 16);
    for (i = 0; (int32_t)i < count; ++i, cursor += 12)
    {
        if (!(r_u8(cursor + 1) & 1))
        {
            entry = cursor;
            w_u32(owner + 20, r_u32(owner + 20) + 1);
            break;
        }
    }
    if (!entry)
    {
        RRJ_DEBUG_FPRINTF(stderr, "No animation entry for %s\n", path);
        abort();
    }
    (void)sub_80014654(path, r_u32(0x8005AD5C), &data, &bytes);
    if (!data || !bytes)
    {
        RRJ_DEBUG_FPRINTF(stderr, "Missing animation resource: %s\n", path);
        abort();
    }
    (void)sub_8005BB6C(entry, data, bytes);
    return entry;
}

uint32_t sub_80065A94(uint32_t record[3], const uint8_t *resources, uint32_t selection, uint32_t *mask_out, uint32_t flags)
{
    uint32_t actor, component, owner, descriptor, physics, physics_group, model, variant;
    uint32_t rank, team, parameter_variant, opposing = 0, reference, value, index, i;
    uint32_t raw_choice, effective_kind, extra, point[3], origin[3], position[3];
    uint32_t geometry[8], initial_geometry[8], segment, piece, special_start = 0;
    uint16_t direction[3];
    uint8_t physics_data[448];
    char path[160] = {0};
    char extension[3] = "ph";
    char animation_path[128];
    int32_t slot = -1, quotient, length;
    FUNCTION_MARKER(0x80065A94, "RASHCDI.BIN");
    if (flags & 4)
        slot = (int32_t)flags >> 4;
    actor = sub_80065974(slot);
    if (!actor)
        return 0;
    w_u32(actor + 1088, actor + 1088);
    if (flags & 1)
    {
        w_u32(0x8005B38C, actor);
        w_u32(0x8005B268, actor);
    }
    if (flags & 8)
    {
        w_u32(0x8005B21C, actor);
        w_u32(0x8005B26C, actor);
    }
    component = sub_80065A04(slot);
    if (!component)
        return 0;
    w_u32(component + 596, actor);
    w_u32(actor + 852, component);
    owner = r_u32(0x8005B2F8);
    if (flags & 9)
    {
        variant = (flags >> 3) & 1;
        quotient = r_s32(owner + 72 + 4 * variant) / 9;
        value = (uint32_t)r_s8(0x800D81D8 + 36 * variant + 10);
        parameter_variant = quotient == 2 ? 26 : (int32_t)value > 0 ? 2 * (value - 1) + (uint32_t)quotient + 22 : (uint32_t)quotient;
    }
    else if (selection >= 17)
        parameter_variant = variant = selection + 1;
    else
    {
        value = *mask_out;
        if ((value & (0x1FF | (0x1FF << 10))) == (0x1FF | (0x1FF << 10)))
            abort();
        for (;;)
        {
            rank = (selection + 1) >> 1;
            team = (selection + 1) & 1;
            while (value & (1u << ((rank + 10 * team) & 31)))
                ++rank;
            if ((int32_t)rank < 9)
                break;
            ++selection;
        }
        *mask_out |= 1u << ((rank + 10 * team) & 31);
        w_u8(0x8006B8A0 + r_u16(actor + 172), team);
        variant = rank + 8 * team + 1;
        owner = r_u32(0x8005B2F8);
        value = r_u8(owner + 4);
        if (value == 33)
            team = r_u32(r_u32(0x8005B38C) + 180) < 9;
        else if ((value & 1) && r_s32(0x8005B1F8) >= 3)
        {
            reference = (r_u8(r_u32(r_u32(0x8005B38C) + 1084) + 1) & 15) == 2 ? 1 : r_u32(owner + 48) < 2;
            team = r_u8(r_u32(r_u32(0x8005B3A0) + 1096 * reference + 1084) + 1) & 15;
        }
        else if ((value & 4) && value != 44)
            team = r_u8(r_u32(r_u32(0x8005B38C) + 1084) + 1) & 15;
        parameter_variant = rank + 8 * team + 1;
    }
    w_u8(0x800D38B0 + r_u16(actor + 172), variant);
    w_u8(0x800D38C8 + variant, r_u8(actor + 172));
    descriptor = 0x800D5758 + 72 * variant;
    w_u32(actor + 1084, descriptor);
    (void)sub_80064C0C(resources, parameter_variant, actor);
    value = r_u8(r_u32(0x8005B2F8) + 4);
    if ((value == 33 || value == 44) && (r_u8(descriptor + 1) & 15) != (r_u8(r_u32(r_u32(0x8005B38C) + 1084) + 1) & 15))
    {
        opposing = 1;
        w_u32(actor + 36, (r_u32(actor + 36) & 0xFFFC0FFF) | ((resources[1206] & 63) << 12));
        w_u32(r_u32(actor + 852) + 36, (r_u32(r_u32(actor + 852) + 36) & 0xFFFC0FFF) | ((resources[1207] & 63) << 12));
        for (i = 0; i < 4; ++i)
            w_u8(actor + 76 + i, resources[1208 + i]);
        for (i = 0; i < 4; ++i)
            w_u8(r_u32(actor + 852) + 76 + i, resources[1212 + i]);
    }
    w_u8(descriptor + 38, sub_80066E1C(actor));
    if (flags & 9)
    {
        value = r_u8(r_u32(0x8005B2F8) + 4);
        if (!(value & 1) || value == 33)
        {
            value = sub_8005DD9C((flags ^ 1) & 1);
            if (value != 0xFFFFFFFF)
            {
                w_u32(actor + 36, (r_u32(actor + 36) & 0xFFFC0FFF) | ((value & 63) << 12));
                w_u32(r_u32(actor + 852) + 36, (r_u32(r_u32(actor + 852) + 36) & 0xFFFC0FFF) | (((value + 1) & 63) << 12));
            }
        }
    }
    if (r_u8(r_u32(0x8005B2F8) + 4) & 8)
    {
        w_u16(descriptor + 44, r_u16(descriptor + 44) & 0xFECD);
        value = r_u8(descriptor + 46);
        if (value - 4 < 2 || value == 1 || value == 8)
        {
            w_u8(descriptor + 46, 9);
            w_u8(descriptor + 47, 0);
        }
    }
    if (flags & 9)
    {
        raw_choice = r_u32(r_u32(0x8005B2F8) + 72 + 4 * !(flags & 1));
        w_u8(descriptor + 1, raw_choice / 9);
        rank = sub_80065648();
        w_u8(descriptor + 1, r_u8(descriptor + 1) | (rank << 4));
        if ((int32_t)rank < 2)
            rank = 8;
        if ((flags & 8) && (r_u8(descriptor + 1) & 15) == (r_u8(r_u32(r_u32(0x8005B38C) + 1084) + 1) & 15))
            rank = rank == 5 || rank == 2 ? rank + 1 : rank - 1;
        *mask_out |= 1u << ((rank + 10 * r_u8(descriptor + 1)) & 31);
        physics_group = flags & 8 ? 4 : 3;
        value = raw_choice;
        owner = r_u32(0x8005B2F8);
        if (raw_choice >= 9 && (r_u8(owner + 4) & 8) && r_u8(owner + r_u16(actor + 172) + 10) != 1)
            value -= 9;
        length = snprintf(path, sizeof(path), "%s%s", (const char *)xport_guest_cptr(0x8005243C, 1), (const char *)xport_guest_cptr(sub_80064034(0, value), 1));
        if (length < 0 || (uint32_t)length >= sizeof(path))
            abort();
        model = raw_choice;
    }
    else
    {
        value = r_u8(descriptor + 1);
        physics_group = value & 15;
        effective_kind = physics_group + ((0 - opposing) & (2 - physics_group));
        extra = (int32_t)effective_kind < 2 && (value >> 4) >= 2;
        model = 9 * effective_kind + 3 * extra + r_u32(r_u32(0x8005B2F8) + 60);
    }
    physics = r_u32(0x8005B248) + 448 * physics_group;
    w_u32(actor + 556, physics);
    if (!(r_u32(0x8005AD40) & (1u << (physics_group & 31))))
    {
        if (!(flags & 9))
        {
            RRJ_DEBUG_FPRINTF(stderr, "Missing initialized NPC physics group %u\n", physics_group);
            abort();
        }
        index = (uint32_t)strlen(path);
        if (index + 3 >= sizeof(path))
            abort();
        memcpy(path + index, ".ph", 4);
        (void)sub_80065558(path, physics_data);
        for (i = 0; i < sizeof(physics_data); ++i)
            w_u8(physics + i, physics_data[i]);
    }
    if (flags & 9)
    {
        if (r_u8(r_u32(0x8005B2F8) + 4) & 1)
            w_u32(actor + 560, r_u32(actor + 560) | 0x8000000);
        if (r_u8(r_u32(0x8005B2F8) + 4) & 32)
            w_u8(physics + 445, 0);
        value = r_u8(physics + 445);
        w_u8(actor + 848, value);
        w_u8(actor + 848, value + r_u8(0x800D81D8 + (flags & 8 ? 36 : 0) + 20) + r_u8(0x800D80F5));
    }
    else
    {
        value = r_u32(actor + 560) | 0x8000000;
        w_u32(actor + 560, value);
        if (flags & 4)
            w_u32(actor + 560, value | 0x10000000);
    }
    w_u32(0x8005AD40, r_u32(0x8005AD40) | (1u << (physics_group & 31)));
    w_u32(actor + 180, sub_8002FAD4(actor, 2, model, 1));
    (void)sub_80012FC8(actor, 0);
    w_u32(actor + 540, 0);
    w_u32(actor + 316, r_u32(physics) + r_u32(r_u32(actor + 852) + 316));
    if (r_u16(actor + 172) < r_u32(r_u32(0x8005B2F8) + 48))
        opposing = 0;
    value = r_u8(descriptor + 1);
    effective_kind = (value & 15) + ((0 - opposing) & (2 - (value & 15)));
    extra = 2 - ((0x1112222 >> ((32 - 4 * (value >> 4)) & 31)) & 15);
    if ((int32_t)effective_kind >= 2)
        extra = 0;
    model = 9 * effective_kind + 3 * extra + r_u32(r_u32(0x8005B2F8) + 60);
    w_u32(component + 180, sub_8002FAD4(component, 1, model, 1));
    (void)sub_80012FC8(component, 0);
    (void)sub_80012838(actor, component, 2, 0);
    if (flags & 1)
    {
        w_u32(0x800CE194, sub_8005BCDC(0x800CE170, (const char *)xport_guest_cptr(0x80052E44, 1)));
        value = r_u8(r_u32(0x8005B2F8) + 4) == 44 ? 0x80052E6C : 0x80052E58;
        w_u32(0x800CE198, sub_8005BCDC(0x800CE170, (const char *)xport_guest_cptr(value, 1)));
        if (r_u8(r_u32(0x8005B2F8) + 4) & 8)
        {
            w_u32(0x800CE1A0, sub_8005BCDC(0x800CE170, (const char *)xport_guest_cptr(0x80052E94, 1)));
            w_u32(0x800CE190, sub_8005BCDC(0x800CE170, (const char *)xport_guest_cptr(0x80052E30, 1)));
            w_u32(0x800CE19C, sub_8005BCDC(0x800CE170, (const char *)xport_guest_cptr(0x80052E08, 1)));
            value = r_u8(r_u32(0x8005B2F8) + 4) == 44 ? 0x80052ED0 : 0x80052EBC;
            w_u32(0x800CE1A8, sub_8005BCDC(0x800CE170, (const char *)xport_guest_cptr(value, 1)));
        }
        else
        {
            w_u32(0x800CE1A0, sub_8005BCDC(0x800CE170, (const char *)xport_guest_cptr(0x80052E80, 1)));
            w_u32(0x800CE190, sub_8005BCDC(0x800CE170, (const char *)xport_guest_cptr(0x80052E1C, 1)));
            w_u32(0x800CE19C, sub_8005BCDC(0x800CE170, (const char *)xport_guest_cptr(0x80052DF4, 1)));
            w_u32(0x800CE1A8, 0);
        }
        w_u32(0x800CE1A4, r_u32(0x8005B254) ? sub_8005BCDC(0x800CE170, (const char *)xport_guest_cptr(0x80052EA8, 1)) : 0);
        length = snprintf(animation_path, sizeof(animation_path), "%s", (const char *)xport_guest_cptr(0x80052DE0, 1));
        if (length < 0 || (uint32_t)length >= sizeof(animation_path))
            abort();
        {
            char *dot = strchr(animation_path, '.');
            index = dot ? (uint32_t)(dot - animation_path + 1) : (uint32_t)length;
            if (!dot)
                animation_path[index++] = '.';
            if (index + sizeof(extension) > sizeof(animation_path))
                abort();
            memcpy(animation_path + index, extension, sizeof(extension));
            for (i = 0; i <= index + 2; ++i)
                w_u8(0x80052DE0 + i, (uint8_t)animation_path[i]);
        }
        for (i = 0; i < 8; ++i)
            (void)sub_8002FAD4(0x800CF018 + 172 * i, 5, 0, 1);
        (void)sub_80063158();
    }
    if ((flags & 9) && !(r_u8(r_u32(0x8005B2F8) + 4) & 1))
        w_u8(descriptor, r_u8(descriptor) | 0x60);
    owner = r_u32(0x8005B2F8);
    if (r_u8(owner + 4) == 33 && r_u16(actor + 172) < r_u32(owner + 48))
        w_u8(descriptor + 1, (r_u8(descriptor + 1) & 0xF0) | 2);
    w_u32(actor + 948, 0);
    w_u32(actor + 952, 0);
    value = record[1];
    index = (uint32_t)((int32_t)value >> 31);
    if ((int32_t)((index + value) ^ index) < 131)
        record[1] = 131;
    (void)sub_8006AD24(geometry);
    for (i = 0; i < 8; ++i)
        w_u32(actor + 328 + 4 * i, geometry[i]);
    for (i = 0; i < 3; ++i)
        point[i] = r_u32(0x800CF578 + 4 * i);
    for (i = 0; i < 3; ++i)
        w_u32(actor + 360 + 4 * i, point[i]);
    w_u8(actor + 534, 1);
    value = record[1] - (uint32_t)((int32_t)record[1] >> 7) - (uint32_t)((int32_t)record[1] >> 6);
    (void)sub_80012C1C(point, position, (int32_t)value);
    segment = sub_80039DFC(0, position);
    if (segment)
    {
        if (r_u32(0x800CF578) == position[0])
        {
            record[0] = 0 - record[0];
            record[1] = 0 - record[1];
        }
        if (record[2] == 1)
        {
            record[0] = 0 - record[0];
            record[1] = 0 - record[1];
        }
        value = sub_8003A700(segment, position, geometry);
        for (i = 0; i < 8; ++i)
            w_u32(actor + 328 + 4 * i, geometry[i]);
        if (!value)
        {
            w_u32(actor + 348, 0);
            w_u16(actor + 320, 0);
            goto inactive_position;
        }
        piece = r_u32(actor + 340);
        record[0] = record[0] - (uint32_t)((int32_t)record[0] >> 7) - (uint32_t)((int32_t)record[0] >> 6);
        for (i = 0; i < 3; ++i)
        {
            origin[i] = r_u32(piece + 20 + 4 * i);
            direction[i] = r_u16(piece + 14 + 2 * i);
        }
        (void)sub_8002EAD8(origin, direction, r_u32(actor + 348), point);
        for (i = 0; i < 3; ++i)
            w_u32(actor + 184 + 4 * i, point[i]);
        for (i = 0; i < 3; ++i)
            direction[i] = r_u16(piece + 2 + 2 * i);
        (void)sub_8002EAD8(point, direction, record[0], point);
        for (i = 0; i < 3; ++i)
            w_u32(actor + 184 + 4 * i, point[i]);
        w_u32(actor + 340, piece);
        w_u16(actor + 320, 1);
        w_u32(actor + 344, record[0]);
        for (i = 0; i < 3; ++i)
        {
            w_u32(actor + 504 + 4 * i, point[i]);
            w_u32(actor + 468 + 4 * i, point[i]);
        }
        (void)sub_8006AD24(initial_geometry);
        owner = r_u32(0x8005B2F8);
        if ((r_u8(owner + 4) & 1) && r_u16(actor + 172) < r_u32(owner + 48) && (r_u8(descriptor + 1) & 15) == 2)
        {
            w_u32(0x8005AD48, r_u32(0x8005AD48) | 1);
            for (i = 0; i < 3; ++i)
            {
                direction[i] = r_u16(actor + 450 + 2 * i);
                point[i] = r_u32(actor + 360 + 4 * i);
            }
            (void)sub_8003662C(direction, geometry, point);
            for (i = 0; i < 3; ++i)
                w_u32(actor + 360 + 4 * i, point[i]);
            (void)sub_8003DE28(actor, 1, 0, 0xFFFFFFFF);
            special_start = sub_8006581C(actor);
        }
        if (!special_start)
        {
            for (i = 0; i < 3; ++i)
                w_u16(actor + 432 + 2 * i, r_u16(piece + 2 + 2 * i));
            for (i = 0; i < 3; ++i)
                w_u16(actor + 438 + 2 * i, (uint16_t)(0 - r_u16(piece + 8 + 2 * i)));
            for (i = 0; i < 3; ++i)
                w_u16(actor + 444 + 2 * i, r_u16(piece + 14 + 2 * i));
            for (index = 0; index < 2; ++index)
            {
                if ((index == 0 && r_u32(0x800CF578) == position[0] && r_s32(0x800CF57C) < 0) || (index == 1 && record[2] == 1))
                {
                    for (i = 0; i < 3; ++i)
                        w_u16(actor + 444 + 2 * i, (uint16_t)(0 - r_u16(actor + 444 + 2 * i)));
                    for (i = 0; i < 3; ++i)
                        w_u16(actor + 432 + 2 * i, (uint16_t)(0 - r_u16(actor + 432 + 2 * i)));
                }
            }
        }
        for (i = 0; i < 9; ++i)
            w_u16(actor + 516 + 2 * i, r_u16(actor + 432 + 2 * i));
        value = sub_80020018((uint32_t)r_s16(actor + 444) << 4, (uint32_t)r_s16(actor + 448) << 4);
        w_u32(actor + 292, value);
        w_u32(actor + 296, (uint32_t)r_s16(0x8005624C + 4 * (value & 4095) + 2) << 4);
        w_u32(actor + 300, (uint32_t)r_s16(0x8005624C + 4 * (r_u32(actor + 292) & 4095)) << 4);
        for (i = 0; i < 3; ++i)
        {
            origin[i] = r_u32(actor + 184 + 4 * i);
            direction[i] = r_u16(actor + 528 + 2 * i);
        }
        (void)sub_8002EAD8(origin, direction, 2 * r_u32(actor + 308), point);
        for (i = 0; i < 3; ++i)
            w_u32(actor + 880 + 4 * i, point[i]);
        for (i = 0; i < 3; ++i)
        {
            w_u16(actor + 450 + 2 * i, r_u16(actor + 444 + 2 * i));
            w_u16(actor + 814 + 2 * i, r_u16(actor + 432 + 2 * i));
            direction[i] = r_u16(actor + 450 + 2 * i);
            point[i] = r_u32(actor + 360 + 4 * i);
        }
        for (i = 0; i < 8; ++i)
            geometry[i] = r_u32(actor + 328 + 4 * i);
        (void)sub_8003662C(direction, geometry, point);
        for (i = 0; i < 3; ++i)
            w_u32(actor + 360 + 4 * i, point[i]);
        (void)sub_8003DE28(actor, 1, 0, 0xFFFFFFFF);
        sub_8003DF54(actor, 1, 0xFFFFFFFF);
        goto finish_position;
    }
    for (i = 0; i < 3; ++i)
        w_u32(actor + 360 + 4 * i, position[i]);
    w_u16(actor + 320, 0);
    (void)sub_8001E100(actor + 328, 0, 32);
    w_u32(actor + 348, 0);
    w_u32(actor + 492, 0);
    w_u32(actor + 496, 0);
    w_u32(actor + 372, 0);
    w_u32(actor + 388, 0);
inactive_position:
    w_u32(actor + 344, record[0] - (uint32_t)((int32_t)record[0] >> 7) - (uint32_t)((int32_t)record[0] >> 6));
finish_position:
    w_u32(actor + 924, 0);
    sub_8003AF9C(actor + 172, 0, 0);
    w_u32(actor + 324, sub_8003B61C(actor + 172));
    w_u16(actor + 956, (r_u8(r_u32(0x8005B2F8) + 4) & 1) && !special_start ? 4 : 1);
    w_u16(actor + 958, 224);
    w_u8(actor + 946, 1);
    if (r_s16(actor + 320))
    {
        for (i = 0; i < 8; ++i)
            w_u32(component + 328 + 4 * i, r_u32(actor + 328 + 4 * i));
        for (i = 0; i < 3; ++i)
            w_u32(component + 184 + 4 * i, r_u32(actor + 184 + 4 * i));
        for (i = 0; i < 3; ++i)
            w_u32(component + 468 + 4 * i, r_u32(component + 184 + 4 * i));
    }
    w_u16(component + 320, r_u16(actor + 320));
    for (i = 0; i < 3; ++i)
        w_u16(component + 444 + 2 * i, r_u16(actor + 444 + 2 * i));
    for (i = 0; i < 3; ++i)
        w_u16(component + 438 + 2 * i, r_u16(actor + 438 + 2 * i));
    for (i = 0; i < 3; ++i)
        w_u16(component + 432 + 2 * i, r_u16(actor + 432 + 2 * i));
    w_u8(component + 571, 255);
    w_u8(component + 569, 41);
    w_u8(component + 570, 0);
    w_u8(component + 572, 0);
    w_u32(component + 556, 0);
    w_u32(component + 540, 0);
    w_u32(component + 604, (r_u8(r_u32(0x8005B2F8) + 4) & 1) != 0);
    w_u16(component + 544, 224);
    w_u32(actor + 176, 0xFFFFFFFF);
    w_u32(component + 176, 0xFFFFFFFF);
    if (r_u8(r_u32(0x8005B2F8) + 4) != 33 && (r_u8(descriptor + 1) & 15) == 2 && (r_u16(descriptor + 44) & 4))
        w_u8(descriptor + 46, 2);
    if (r_u16(actor + 172) >= r_u32(r_u32(0x8005B2F8) + 48) && (r_u8(descriptor + 1) & 15) == 2)
    {
        (void)sub_800644C8(actor);
        w_u16(actor + 320, 0);
        w_u8(actor + 928, r_u8(actor + 928) | 0x30);
    }
    w_u16(actor + 870, 0);
    return actor;
}

uint32_t sub_80060028(uint32_t packet, const uint8_t color[3])
{
    uint32_t command, coordinates, high;
    FUNCTION_MARKER(0x80060028, "RASHCDI.BIN");
    command = ((uint32_t)r_s16(packet + 34) << 25) | ((uint32_t)color[2] << 16) | ((uint32_t)color[1] << 8) | color[0] | 0x60000000;
    coordinates = r_u16(packet + 24) | ((uint32_t)r_u16(packet + 26) << 16);
    high = (uint32_t)r_u16(packet + 30) << 16;
    w_u32(packet + 4, command);
    w_u32(packet + 8, coordinates);
    w_u32(packet + 12, r_u16(packet + 28) | high);
    return high;
}

uint32_t sub_8005FE24(void)
{
    uint32_t result, divisor, player, packet, ordering, data, i;
    int32_t remaining;
    FUNCTION_MARKER(0x8005FE24, "RASHCDI.BIN");
    result = r_u32(0x8005ACD8);
    if (!result)
        return result;
    w_u32(0x8005ACD4, 0);
    divisor = (uint32_t)(r_s32(0x8005B244) / 4);
    w_u32(0x800D9C4C, 0);
    w_u32(0x800D5F6C, 0);
    for (i = 0; i < 3; ++i)
    {
        w_u32(0x800D5F60 + 4 * i, divisor + (2 - i) * divisor);
        w_u32(0x800D9C40 + 4 * i, 0);
    }
    result = r_u32(0x8005AD20);
    if (!result)
        return result;
    result = r_u32(r_u32(0x8005B2F8) + 48);
    if ((int32_t)result <= 0)
        return result;
    player = 0;
    do
    {
        packet = (r_u32(0x8005ACEC + 4 * player) + 3924) & 0xFFFFFF;
        ordering = 0x8005B5A0 + 4 * player;
        for (remaining = 109; remaining >= 0; --remaining, packet -= 36)
        {
            if (!r_s8(packet + 32))
            {
                w_u32(packet, r_u32(r_u32(ordering)) | 0x4000000);
                w_u32(packet + 4, ((uint32_t)r_s16(packet + 34) << 25) | 0x64808080);
                data = r_u32(packet + 20);
                w_u32(packet + 12, r_u32(data + 4));
            }
            else if (r_s8(packet + 32) == 1)
            {
                w_u32(packet, r_u32(r_u32(ordering)) | 0x3000000);
                w_u32(packet + 4, ((uint32_t)r_s16(packet + 34) << 25) | 0x60808080);
            }
            else
            {
                (void)SetDrawMode(xport_guest_ptr(packet, 12), 1, 1, 15, NULL);
                w_u32(packet, r_u32(r_u32(ordering)) | 0x2000000);
            }
            w_u32(r_u32(ordering), packet);
        }
        ++player;
        result = (int32_t)player < r_s32(r_u32(0x8005B2F8) + 48);
    } while (result);
    return result;
}

uint32_t sub_8005ED4C(uint32_t value)
{
    uint32_t result;
    FUNCTION_MARKER(0x8005ED4C, "RASHCDI.BIN");
    result = (int32_t)value > 0 ? sub_80010028(value, 0xC80000) : 0 - sub_80010028(0 - value, 0xC80000);
    w_u32(0x8005B234, result);
    return result;
}

uint32_t sub_8005EC20(void)
{
    uint32_t owner, player, packet, output, i;
    FUNCTION_MARKER(0x8005EC20, "RASHCDI.BIN");
    owner = r_u32(0x8005B2F8);
    for (player = 0; (int32_t)player < r_s32(owner + 48); ++player)
    {
        packet = r_u32(0x8005ACEC + 4 * player) + 3744;
        output = 0x800D63E8 + 12 * player;
        for (i = 0; i < 4; ++i)
            w_u16(output + 2 * i, r_u16(packet + 24 + 2 * i));
        w_u16(output + 8, r_u16(packet + 24) + (r_u16(packet + 28) >> 1));
        w_u16(output + 10, r_u16(packet + 26) + (r_u16(packet + 30) >> 1));
    }
    (void)sub_8005ED4C((uint32_t)r_u16(r_u32(0x8005ACEC) + 3774) << 16);
    for (i = 0; i < 15; ++i)
        w_u32(0x800D6120 + 4 * i, sub_8002D0D8(r_u32(0x8005AD2C), xport_guest_cptr(r_u32(r_u32(0x8005B544) + 60 + 4 * i), 1)));
    return 0;
}

uint32_t sub_80060088(void)
{
    static const uint16_t statistic_offsets[12] = {0, 16, 12, 96, 112, 108, 128, 144, 140, 32, 48, 44};
    static const uint16_t state_offsets[16] = {0, 4, 8, 12, 16, 20, 48, 32, 52, 24, 60, 36, 56, 28, 40, 44};
    static const uint16_t bars[4] = {1908, 2160, 2412, 2664};
    static const uint16_t separators[6] = {2088, 2124, 2340, 2376, 2592, 2628};
    static const uint16_t labels[4] = {900, 1080, 1404, 1584};
    static const uint16_t panels[6] = {684, 792, 972, 1188, 1296, 1476};
    static const uint16_t linked_uv[6] = {1038, 858, 750, 1542, 1362, 1254};
    static const uint16_t gray_packets[4] = {2592, 2628, 2844, 2880};
    uint8_t color[4];
    uint32_t player, packets, base, i, j, owner, mode, table, value, offset;
    FUNCTION_MARKER(0x80060088, "RASHCDI.BIN");
    value = r_u32(0x8005B738);
    for (i = 0; i < 4; ++i)
        color[i] = (uint8_t)(value >> (8 * i));
    for (player = 0; (int32_t)player < r_s32(r_u32(0x8005B2F8) + 48); ++player)
    {
        packets = r_u32(0x8005ACEC + 4 * player);
        base = 0x800D6198 + 224 * player;
        for (i = 0; i < 12; ++i)
            w_u32(base + statistic_offsets[i], 0);
        base = 0x800D6358 + 72 * player;
        w_u32(base + 28, 17);
        w_u32(base + 32, 30);
        w_u32(base + 64, 31);
        w_u32(base, 0);
        w_u32(base + 4, 0);
        w_u32(base + 36, 0);
        w_u32(base + 40, 0);
        w_u32(base + 68, 44);
        base = 0x800D4A60 + 88 * player;
        for (i = 0; i < 4; ++i)
            w_u32(base + 4 * i, 0);
        w_u32(0x8005B380 + 4 * player, 0);
        for (i = 4; i < 16; ++i)
            w_u32(base + state_offsets[i], 0);
        w_u32(base + 84, 0xFFFFFFFF);
        w_u32(base + 64, 0);
        for (i = 68; i <= 80; i += 4)
            w_u32(base + i, 0xFFFFFFFF);
        for (i = 0; i < 5; ++i)
            for (j = 0; j < 4; ++j)
                (void)sub_80013AF8(packets + bars[j] + 36 * i, 0x800D4620);
        for (i = 0; i < 6; ++i)
            (void)sub_80013AF8(packets + separators[i], 0x800D46C0);
        for (i = 0; i < 4; ++i)
            (void)sub_80013AF8(packets + labels[i], 0x800D49A0);
        (void)sub_80013AF8(packets + 3744, 0x800D46F0);
        for (i = 0; i < 6; ++i)
            (void)sub_80013AF8(packets + panels[i], 0x800D46E0);
    }
    (void)sub_8005FE24();
    for (player = 0; (int32_t)player < r_s32(r_u32(0x8005B2F8) + 48); ++player)
    {
        packets = r_u32(0x8005ACEC + 4 * player);
        for (i = 0; i < 6; ++i)
            (void)sub_80060028(packets + panels[i], color);
        value = r_u16(packets + 714);
        for (i = 0; i < 6; ++i)
            w_u16(packets + linked_uv[i], value);
        owner = r_u32(0x8005B2F8);
        mode = r_u8(owner + 4);
        if ((mode & 4) && mode != 36)
        {
            offset = 176 * (r_u32(owner + 64) - 56);
            table = 0x80053A88 + offset;
            value = (uint32_t)((int32_t)(r_u32(table + 12) << 8) / 300);
            (void)sub_80013B90((int32_t)value, packets + 2160, 0x800D4620);
            table = 0x80053A88 + 176 * (r_u32(r_u32(0x8005B2F8) + 64) - 56);
            value = (uint32_t)((int32_t)(r_u32(table) << 8) / 300);
            (void)sub_80013B90((int32_t)value, packets + 2412, 0x800D4620);
            table = 0x80053A88 + 176 * (r_u32(r_u32(0x8005B2F8) + 64) - 56);
            value = (uint32_t)((int32_t)(r_u32(table) << 8) / 300);
            (void)sub_80013B90((int32_t)value, packets + 2664, 0x800D4620);
        }
        mode = r_u8(r_u32(0x8005B2F8) + 4);
        if (mode == 44 || mode == 36)
            (void)sub_80013B90((int32_t)(r_u32(0x8005ACC8) << 8) / 300, packets + 1908, 0x800D4620);
        for (i = 0; i < 5; ++i)
        {
            w_u8(packets + 2416 + 36 * i, 128);
            w_u8(packets + 2417 + 36 * i, 128);
            w_u8(packets + 2418 + 36 * i, 0);
            w_u8(packets + 2668 + 36 * i, 129);
            w_u8(packets + 2669 + 36 * i, 128);
            w_u8(packets + 2670 + 36 * i, 0);
        }
        for (i = 0; i < 4; ++i)
        {
            w_u8(packets + gray_packets[i] + 4, 128);
            w_u8(packets + gray_packets[i] + 5, 128);
            w_u8(packets + gray_packets[i] + 6, 0);
        }
        w_u32(0x8005ACE4 + 4 * player, 0);
    }
    return sub_8005EC20();
}

uint32_t sub_80063B90(void)
{
    FUNCTION_MARKER(0x80063B90, "RASHCDI.BIN");
    w_u8(r_u32(0x8005B2F8) + 3, 1);
    w_u32(0x8005B254, !(r_u32(r_u32(0x8005B2F8) + 4) & 0x18));
    (void)sub_8001B6A8();
    (void)sub_80063500();
    (void)sub_800635D0();
    (void)sub_80063670();
    (void)sub_80060088();
    return sub_8001BDB0();
}

uint32_t sub_8006A944(void)
{
    uint32_t i;
    FUNCTION_MARKER(0x8006A944, "RASHCDI.BIN");
    for (i = 0; i < 6; ++i)
    {
        w_u32(0x800D4B10 + 16 * i, 0xFFFFFFFF);
        w_u32(0x800D4B14 + 16 * i, 0);
        w_u32(0x800D4B18 + 16 * i, 0);
        w_u32(0x800D4B1C + 16 * i, 0);
    }
    w_u32(0x8005B320, 0);
    w_u32(0x8005B31C, 0xFFFFFFFF);
    return 0xFFFFFFFF;
}

uint32_t sub_8006A9B8(void)
{
    uint32_t data;
    FUNCTION_MARKER(0x8006A9B8, "RASHCDI.BIN");
    (void)sub_8006A944();
    data = r_u32(0x8005B338);
    return data ? sub_800144B8(data) : 0x80060000;
}

uint32_t sub_8006ACB0(void)
{
    FUNCTION_MARKER(0x8006ACB0, "RASHCDI.BIN");
    return sub_8006A9B8();
}

uint32_t sub_8006A8D0(void)
{
    uint32_t data;
    FUNCTION_MARKER(0x8006A8D0, "RASHCDI.BIN");
    data = r_u32(0x800D6170);
    return data ? sub_800144B8(data) : 0x800D0000;
}

uint32_t sub_8006B3FC(void)
{
    static const uint32_t pointers[4] = {0x8005B35C, 0x8005B24C, 0x8005B304, 0x8005B250};
    uint32_t i, data;
    FUNCTION_MARKER(0x8006B3FC, "RASHCDI.BIN");
    for (i = 0; i < 4; ++i)
    {
        data = r_u32(pointers[i]);
        if (data)
        {
            (void)sub_800144B8(data);
            w_u32(pointers[i], 0);
        }
    }
    w_u32(0x8005B314, 0);
    return 0x80060000;
}

uint32_t sub_800686B8(void)
{
    uint32_t data;
    FUNCTION_MARKER(0x800686B8, "RASHCDI.BIN");
    if (r_u8(r_u32(0x8005B2F8) + 4) != 44)
        return 44;
    data = r_u32(0x8005B344);
    if (data)
        (void)sub_800144B8(data);
    data = r_u32(0x8005B340);
    if (data)
        (void)sub_800144B8(data);
    w_u32(0x8005B344, 0);
    w_u32(0x8005B340, 0);
    w_u32(0x8005B20C, 0);
    w_u32(0x8005B208, 0);
    return 0x8005B208;
}

uint32_t sub_8005D618(uint32_t object)
{
    FUNCTION_MARKER(0x8005D618, "RASHCDI.BIN");
    return sub_800144B8(r_u32(object + 4));
}

uint32_t sub_800694B0(void)
{
    uint32_t actor, animation, i;
    FUNCTION_MARKER(0x800694B0, "RASHCDI.BIN");
    actor = r_u32(0x800D4B80);
    if (actor)
    {
        for (i = 0; i < 4; ++i, actor += 572)
        {
            if (r_u32(actor + 4))
            {
                w_u32(0x800D4B7C, r_u32(0x800D4B7C) - 1);
                (void)sub_8005D618(actor);
            }
            animation = r_u32(actor + 540);
            if (animation)
            {
                w_u32(animation + 36, 0);
                w_u32(0x800CE178, r_u32(0x800CE178) - 1);
            }
            w_u32(actor + 540, 0);
        }
        (void)sub_8001E100(r_u32(0x800D4B80), 0, 2288);
    }
    for (i = 0; i < 16; ++i)
    {
        actor = 0x800CF660 + 512 * i;
        if (r_u32(actor + 4))
        {
            w_u32(0x800CF65C, r_u32(0x800CF65C) - 1);
            (void)sub_8005D618(actor);
        }
    }
    (void)sub_8001E100(0x800CF660, 0, 8192);
    w_u32(0x800CD6B4, 0);
    w_u32(0x800D1814, 8344);
    w_u32(0x800D1660, 0);
    for (i = 0; i < 18; ++i)
        w_u32(0x800D1664 + 24 * i, 0);
    return 8344;
}

void sub_8006860C(void)
{
    FUNCTION_MARKER(0x8006860C, "RASHCDI.BIN");
}

void sub_80068A50(void)
{
    FUNCTION_MARKER(0x80068A50, "RASHCDI.BIN");
    if (r_u8(0x8005AD31))
    {
        (void)sub_800694B0();
        if (r_u32(0x8005B254))
            sub_8006860C();
        w_u8(0x8005AD31, 0);
    }
}

uint32_t sub_800677EC(void)
{
    uint32_t owner, player, linked, component, actor, animation, i;
    int32_t remaining, value;
    FUNCTION_MARKER(0x800677EC, "RASHCDI.BIN");
    if (!r_u8(0x8005AD30))
        return 0;
    owner = r_u32(0x8005B2F8);
    if ((r_u8(owner + 4) & 32) && r_s8(owner) != 5)
    {
        value = r_s8(r_u32(0x8005B38C) + 848);
        if ((int32_t)r_u8(0x800D81EC) < value)
            value = r_u8(0x800D81EC);
        w_u8(0x800D81EC, (uint8_t)value);
    }
    for (i = 0; i < 2; ++i)
    {
        player = r_u32(i ? 0x8005B21C : 0x8005B38C);
        if (!player)
        {
            if (!i)
                abort();
            continue;
        }
        linked = r_u32(player + 856);
        if (linked && r_u32(player + 1088))
        {
            (void)sub_8005D618(r_u32(linked + 852));
            player = r_u32(i ? 0x8005B21C : 0x8005B38C);
            component = r_u32(r_u32(player + 856) + 852);
            w_u32(component + 4, 0);
            animation = r_u32(r_u32(r_u32(player + 856) + 852) + 540);
            if (animation)
            {
                w_u32(animation + 36, 0);
                w_u32(0x800CE178, r_u32(0x800CE178) - 1);
            }
        }
    }
    remaining = r_s32(r_u32(0x800CE4DC));
    actor = r_u32(0x800CE4D0);
    while (remaining >= 0)
    {
        (void)sub_8005D618(actor);
        component = r_u32(actor + 852);
        w_u32(actor + 4, 0);
        (void)sub_8005D618(component);
        w_u32(r_u32(actor + 852) + 4, 0);
        animation = r_u32(actor + 540);
        if (animation)
        {
            w_u32(animation + 36, 0);
            w_u32(0x800CE178, r_u32(0x800CE178) - 1);
        }
        animation = r_u32(r_u32(actor + 852) + 540);
        if (animation)
        {
            w_u32(animation + 36, 0);
            w_u32(0x800CE178, r_u32(0x800CE178) - 1);
        }
        --remaining;
        actor += r_u32(0x800CE4D4);
    }
    for (i = 0; i < 8; ++i)
        (void)sub_8005D618(0x800CF018 + 172 * i);
    (void)sub_800144B8(r_u32(0x8005B3A4));
    (void)sub_800144B8(r_u32(0x8005B3A0));
    (void)sub_800144B8(r_u32(0x8005B248));
    (void)sub_8001E100(0x8005B2B8, 0, 4);
    w_u32(0x8005B1F8, 0);
    w_u32(0x8005B1FC, 0);
    w_u32(0x8005AD40, 0);
    w_u32(0x8005AD50, 0);
    w_u8(0x8005AD30, 0);
    w_u32(0x8005B21C, 0);
    w_u32(0x8005B26C, 0);
    w_u32(0x8005B38C, 0);
    w_u32(0x8005B268, 0);
    return 0x80060000;
}

uint32_t sub_800696AC(void)
{
    FUNCTION_MARKER(0x800696AC, "RASHCDI.BIN");
    w_u32(0x8005AD34, 0);
    return 0x80060000;
}

uint32_t sub_80069238(void)
{
    uint32_t actor, state, player, i, result = 0x800D0000;
    FUNCTION_MARKER(0x80069238, "RASHCDI.BIN");
    actor = r_u32(0x800CD6C4);
    w_u32(0x800CD6B4, 0);
    if (actor)
    {
        (void)sub_8001E100(actor, 0, 280 * r_u32(0x8005B214));
        (void)sub_800144B8(r_u32(0x800CD6C4));
        w_u32(0x800CD6C4, 0);
    }
    for (player = 0; (int32_t)player < r_s32(r_u32(0x8005B2F8) + 48); ++player)
    {
        state = 0x800D5CF8 + 32 * player;
        actor = r_u32(state + 4);
        w_u32(state, r_u32(0x8005B214));
        if (actor)
        {
            (void)sub_800144B8(actor);
            w_u32(state + 4, 0);
            w_u32(state, 0);
        }
        for (i = 0; i < 3; ++i)
        {
            w_u32(state + 8 + 4 * i, 0);
            w_u32(state + 20 + 4 * i, 0xFFFFFFFF);
        }
    }
    actor = r_u32(0x800D4B80);
    w_u32(0x800D4B7C, 0);
    if (actor)
    {
        (void)sub_8001E100(actor, 0, 2288);
        result = sub_800144B8(r_u32(0x800D4B80));
        w_u32(0x800D4B80, 0);
    }
    return result;
}

uint32_t sub_80069AC8(void)
{
    FUNCTION_MARKER(0x80069AC8, "RASHCDI.BIN");
    (void)sub_800686B8();
    sub_80068A50();
    (void)sub_800677EC();
    (void)sub_800696AC();
    (void)sub_80068D54();
    return sub_80069238();
}

uint32_t sub_8005D520(void)
{
    uint32_t data, owner;
    FUNCTION_MARKER(0x8005D520, "RASHCDI.BIN");
    data = r_u32(r_u32(0x8005ACBC) + 2616);
    if (data)
        (void)sub_800144B8(data);
    owner = r_u32(0x8005ACBC);
    w_u32(owner + 2620, 0);
    return owner;
}

void sub_8005D010(void)
{
    FUNCTION_MARKER(0x8005D010, "RASHCDI.BIN");
}

uint32_t sub_8005BA9C(uint32_t entry)
{
    uint32_t result;
    FUNCTION_MARKER(0x8005BA9C, "RASHCDI.BIN");
    result = r_u8(entry + 1) & 1;
    if (result)
    {
        (void)sub_800144B8(r_u32(entry + 8));
        result = sub_800144B8(r_u32(entry + 4));
        w_u8(entry + 1, 0);
        w_u16(entry + 2, 0);
        w_u32(entry + 4, 0);
        w_u8(entry, 0);
    }
    return result;
}

uint32_t sub_8005BCA0(uint32_t owner, uint32_t entry)
{
    FUNCTION_MARKER(0x8005BCA0, "RASHCDI.BIN");
    (void)sub_8005BA9C(entry);
    w_u32(owner + 20, r_u32(owner + 20) - 1);
    return 0;
}

uint32_t sub_8005D274(uint32_t owner)
{
    uint32_t result, entry, index = 0;
    FUNCTION_MARKER(0x8005D274, "RASHCDI.BIN");
    (void)sub_800144B8(r_u32(owner));
    (void)sub_800144B8(r_u32(owner + 4));
    result = r_u32(owner + 24);
    w_u32(owner, 0);
    w_u32(owner + 4, 0);
    if ((int32_t)result > 0)
    {
        do
        {
            entry = r_u32(owner + 16) + 12 * index;
            if (r_u8(entry + 1) & 1)
                (void)sub_8005BCA0(owner, entry);
            ++index;
            result = (int32_t)index < r_s32(owner + 24);
        } while (result);
    }
    return result;
}

uint32_t sub_8005BF2C(uint32_t slot)
{
    uint32_t entry, i, data;
    FUNCTION_MARKER(0x8005BF2C, "RASHCDI.BIN");
    entry = 0x800CE1B0 + 16 * slot;
    for (i = 0; i < r_u8(entry + 4); ++i)
        (void)sub_800144B8(r_u32(r_u32(entry + 8) + 12 * i + 4));
    (void)sub_800144B8(r_u32(entry + 8));
    data = r_u32(entry + 12);
    if (data)
        (void)sub_800144B8(data);
    return sub_8001E100(entry, 0, 16);
}

uint32_t sub_8005BEC0(void)
{
    uint32_t i;
    FUNCTION_MARKER(0x8005BEC0, "RASHCDI.BIN");
    for (i = 0; i < 50; ++i)
        if (r_u32(0x800CE1B0 + 16 * i))
            (void)sub_8005BF2C(i);
    return sub_8001E100(0x800CE1B0, 0, 800);
}

uint32_t sub_8005D5C4(uint32_t data)
{
    FUNCTION_MARKER(0x8005D5C4, "RASHCDI.BIN");
    return sub_800144B8(data);
}

uint32_t sub_8005D920(void)
{
    FUNCTION_MARKER(0x8005D920, "RASHCDI.BIN");
    (void)sub_8005D5C4(r_u32(0x8005ACB0));
    w_u32(0x8005ACB0, 0);
    w_u32(0x8005ACB4, 0);
    w_u32(0x8005ACB8, 0);
    return 0x80060000;
}

uint32_t sub_8005F0B8(void)
{
    uint32_t data;
    FUNCTION_MARKER(0x8005F0B8, "RASHCDI.BIN");
    data = r_u32(0x8005B2F0);
    w_u32(0x8005ACD8, 0);
    if (!data)
        return 0x80060000;
    (void)sub_800144B8(data);
    w_u32(0x8005ACEC, 0);
    w_u32(0x8005B2F0, 0);
    w_u32(0x8005ACF0, 0);
    return 0x8005ACEC;
}

uint32_t sub_800606FC(void)
{
    uint32_t data;
    FUNCTION_MARKER(0x800606FC, "RASHCDI.BIN");
    if (r_u32(r_u32(0x8005B2F8) + 48) != 1)
        return 0x80060000;
    data = r_u32(0x8005B2D0);
    if (data)
        return data;
    (void)sub_80020228();
    return sub_800144B8(r_u32(0x8005B278));
}

uint32_t sub_80060754(void)
{
    FUNCTION_MARKER(0x80060754, "RASHCDI.BIN");
    (void)sub_800606FC();
    w_u32(0x8005AD24, 0);
    w_u32(0x8005AD28, 0);
    return 0x80060000;
}

uint32_t sub_80063D24(void)
{
    uint32_t i;
    FUNCTION_MARKER(0x80063D24, "RASHCDI.BIN");
    if (r_u32(0x8005AE74))
    {
        for (i = 0; i < 4; ++i)
            (void)sub_8002C9D4(i);
        w_u32(0x8005B554, 0);
        w_u32(0x8005B540, 0);
        w_u32(0x8005B544, 0);
        w_u16(0x8005B54C, 0);
    }
    (void)sub_8002C9A8();
    w_u32(0x8005AE74, 0);
    return 0x80060000;
}

uint32_t sub_800632B4(uint32_t player)
{
    uint32_t block, handle;
    FUNCTION_MARKER(0x800632B4, "RASHCDI.BIN");
    block = r_u32(0x8005B42C) + 132 * player;
    if (!r_u8(block + 4))
        return 0;
    (void)sub_8001F7EC(r_u32(block + 20));
    (void)sub_8001F7EC(r_u32(block + 24));
    (void)sub_8001F7EC(r_u32(block + 32));
    (void)sub_8001F7EC(r_u32(block + 28));
    (void)sub_8001F7EC(r_u32(block + 16));
    (void)sub_8001F7EC(r_u32(block + 36));
    (void)sub_8001F7EC(r_u32(block + 40));
    handle = r_u32(block + 28);
    w_u8(block + 4, 0);
    return sub_8001F900(handle, 0);
}

uint32_t sub_8006326C(void)
{
    uint32_t data;
    FUNCTION_MARKER(0x8006326C, "RASHCDI.BIN");
    data = r_u32(0x8005B428);
    if (data)
    {
        (void)sub_800144B8(data);
        w_u32(0x8005B428, 0);
    }
    w_u32(0x8005B3E4, 0);
    w_u32(0x8005B408, 0);
    return 0x80060000;
}

uint32_t sub_8006335C(void)
{
    uint32_t slot;
    FUNCTION_MARKER(0x8006335C, "RASHCDI.BIN");
    if (r_u32(0x8005B42C))
    {
        (void)sub_800632B4(0);
        if (r_u32(r_u32(0x8005B2F8) + 48) == 2)
            (void)sub_800632B4(1);
    }
    for (slot = 0; slot < 8; ++slot)
    {
        if (r_u32(0x8005B410))
        {
            (void)sub_80017814(slot, 0);
            if (r_u32(r_u32(0x8005B2F8) + 48) == 2)
                (void)sub_80017814(slot, 1);
        }
    }
    w_u32(0x8005B41C, r_u32(r_u32(0x8005B2F8) + 12));
    (void)sub_8001F0D4();
    (void)sub_8001FBD4();
    return sub_8006326C();
}

uint32_t sub_80062E9C(void)
{
    uint32_t player, slot, block, entry, data;
    FUNCTION_MARKER(0x80062E9C, "RASHCDI.BIN");
    if (r_u32(0x8005B42C))
    {
        (void)sub_8001EE28(r_u32(r_u32(0x8005B42C) + 8));
        (void)sub_800144B8(r_u32(r_u32(0x8005B42C) + 12));
        if (r_u32(r_u32(0x8005B2F8) + 48) == 2)
        {
            (void)sub_8001EE28(r_u32(r_u32(0x8005B42C) + 140));
            (void)sub_800144B8(r_u32(r_u32(0x8005B42C) + 144));
        }
        (void)sub_800632B4(0);
        if (r_u32(r_u32(0x8005B2F8) + 48) == 2)
            (void)sub_800632B4(1);
        (void)sub_800144B8(r_u32(0x8005B42C));
        w_u32(0x8005B42C, 0);
    }
    for (player = 0; player < 2; ++player)
    {
        entry = 0x8005B410 + 4 * player;
        if (r_u32(entry) && r_u32(0x8005B40C))
        {
            slot = 0;
            block = r_u32(0x8005B40C) + 72 * player;
            while ((int32_t)slot < (int32_t)(r_u32(block + 36) + r_u32(block + 40)))
            {
                (void)sub_80017814(slot, player);
                ++slot;
                block = r_u32(0x8005B40C) + 72 * player;
            }
            (void)sub_800144B8(r_u32(entry));
            w_u32(entry, 0);
        }
    }
    data = r_u32(0x8005B40C);
    if (data)
    {
        (void)sub_800144B8(data);
        w_u32(0x8005B40C, 0);
    }
    for (entry = 0x800D6AA0; entry < 0x800D6BC0; entry += 32)
    {
        data = r_u32(entry);
        if (data != r_u32(0x8005B404))
        {
            (void)sub_8001EE28(data);
            data = r_u32(entry + 28);
            if (data)
                (void)sub_800144B8(data);
        }
        w_u32(entry, 0xFFFFFFFF);
        w_u32(entry + 4, 0xFFFFFFFF);
        w_u32(entry + 8, 0);
        w_u32(entry + 28, 0);
    }
    (void)sub_8001EE28(r_u32(0x8005B404));
    (void)sub_8001EE28(r_u32(0x8005B420));
    data = r_u32(0x8005B3FC);
    if ((int32_t)data >= 0)
        (void)sub_8001EE28(data);
    data = r_u32(0x8005B3F8);
    w_u32(0x8005B404, 0xFFFFFFFF);
    w_u32(0x8005B420, 0xFFFFFFFF);
    w_u32(0x8005B3FC, 0xFFFFFFFF);
    if (data)
    {
        (void)sub_800144B8(data);
        w_u32(0x8005B3F8, 0);
    }
    data = r_u32(0x8005B418);
    if (data)
    {
        (void)sub_800144B8(data);
        w_u32(0x8005B418, 0);
    }
    data = r_u32(0x8005B3F0);
    if (data)
    {
        (void)sub_800144B8(data);
        w_u32(0x8005B3F0, 0);
    }
    return sub_8001460C(r_u32(0x800D6858));
}

void sub_8005D610(void)
{
    FUNCTION_MARKER(0x8005D610, "RASHCDI.BIN");
}

void sub_8005DA30(void)
{
    FUNCTION_MARKER(0x8005DA30, "RASHCDI.BIN");
}

uint32_t sub_80063A20(void)
{
    uint32_t player, object;
    int32_t mode;
    FUNCTION_MARKER(0x80063A20, "RASHCDI.BIN");
    for (player = 0; player < r_u32(r_u32(0x8005B2F8) + 52); ++player)
    {
        (void)sub_8001DD08(player, -1, 0);
        (void)sub_8001DC94(player, -1);
    }
    (void)sub_8005D520();
    sub_8005D010();
    (void)sub_800144B8(r_u32(0x8005AD4C));
    (void)sub_8006ACB0();
    (void)sub_8006A8D0();
    (void)sub_8006B3FC();
    (void)sub_80069AC8();
    (void)sub_8005D274(0x800CE170);
    (void)sub_8005BEC0();
    (void)sub_8005D920();
    (void)sub_8001C298();
    (void)sub_8005F0B8();
    (void)sub_80060754();
    sub_8005D610();
    sub_8005DA30();
    (void)sub_80063D24();
    (void)sub_8002C9A8();
    (void)sub_8006335C();
    (void)sub_80062E9C();
    sub_800245B4();
    mode = r_s8(r_u32(0x8005B2F8));
    if (mode != 5 && mode != 2)
        return 2;
    (void)sub_8001BBC0();
    (void)sub_8001BCE8(2);
    object = r_u32(0x8005B278);
    w_u32(object + 8, 0xFFFFFFFF);
    w_u32(object + 12, 0xFFFFFFFF);
    return 0xFFFFFFFF;
}

void sub_800B2AF8(void)
{
    FUNCTION_MARKER(0x800B2AF8, "RASHCDG.BIN");
}

uint32_t sub_800B2C98(uint32_t actor, uint32_t other)
{
    uint32_t result, count, index, identity;
    FUNCTION_MARKER(0x800B2C98, "RASHCDG.BIN");
    if (r_s32(0x800CCF90) >= 8)
        return 0;
    result = sub_800A8C78(actor, 0x140000, 0x1C0000);
    if (!result)
        return result;
    count = r_u32(0x800CCF90);
    identity = r_u8(other);
    for (index = 0; (int32_t)index < (int32_t)count; ++index)
    {
        result = r_u8(0x800CCF88 + index);
        if (result == identity)
            return result;
    }
    count = r_u32(0x800CCF90);
    if (count >= 8)
    {
        RRJ_DEBUG_FPUTS("Collision identity count is outside its table\n", stderr);
        abort();
    }
    result = 0x800CCF88 + count;
    w_u8(result, r_u8(other));
    w_u32(0x800CCF90, count + 1);
    return result;
}

uint32_t sub_800B2D44(uint32_t actor, uint32_t other)
{
    uint32_t actor_face, other_face, translation[3], movable;
    FUNCTION_MARKER(0x800B2D44, "RASHCDG.BIN");
    movable = (sub_800AA140(actor, xport_guest_cptr(other, 512), &actor_face, &other_face, translation) ? other : 0);
    if (movable)
        return sub_800A8DF0(xport_guest_ptr(movable, 1), translation, 1);
    return movable;
}

uint32_t sub_800B2D88(uint32_t bounds, uint32_t actor)
{
    uint32_t result, face, depth, corner[3], translation[3];
    uint16_t normal[3];
    FUNCTION_MARKER(0x800B2D88, "RASHCDG.BIN");
    result = sub_800A8C78(actor, 0x280000, 0x380000);
    if (!result)
        return result;
    result = sub_800B7030(xport_guest_cptr(actor + 196, 96), xport_guest_cptr(bounds + 432, 18), xport_guest_cptr(bounds + 196, 96), 16, &face, &depth);
    if (result == 8)
        return result;
    (void)sub_800B675C(xport_guest_cptr(bounds + 196, 96), xport_guest_cptr(bounds + 432, 18), face, normal, corner);
    (void)sub_8002EE50(depth + 0x2000, normal, translation);
    (void)sub_800A8DF0(xport_guest_ptr(actor, 1), translation, 1);
    /* Preserve the actor instead of the original incidental v0 result */
    return sub_800B2F94(actor, bounds, normal);
}

uint32_t sub_800B2B00(uint32_t actor, uint32_t bounds)
{
    uint32_t result, accepted = 1, face, depth, translation[3];
    uint32_t speed, sign, magnitude, level, difference, kind;
    uint16_t normal[3];
    int32_t quotient;
    FUNCTION_MARKER(0x800B2B00, "RASHCDG.BIN");
    if ((r_u16(bounds) >> 5) == 6 && r_s32(bounds + 140) > 0x20000 && r_u32(bounds + 8) != 1)
        (void)sub_800B2C98(actor, bounds);
    result = sub_800B7030(xport_guest_cptr(actor + 196, 96), xport_guest_cptr(bounds + 260, 18), xport_guest_cptr(bounds + 24, 96), 16, &face, &depth);
    if (result == 8)
        return result;
    (void)sub_800B675C(xport_guest_cptr(bounds + 24, 96), xport_guest_cptr(bounds + 260, 18), face, normal, NULL);
    depth += 0x2000;
    (void)sub_8002EE50(depth, normal, translation);
    (void)sub_800A8DF0(xport_guest_ptr(actor, 1), translation, 1);
    speed = r_u32(actor + 480);
    sign = (uint32_t)((int32_t)speed >> 31);
    magnitude = (speed + sign) ^ sign;
    if ((int32_t)magnitude < 0)
        magnitude += 0x1FFFF;
    quotient = (int32_t)magnitude >> 17;
    level = ~(uint32_t)((int32_t)magnitude >> 31) & (uint32_t)quotient;
    difference = 4 - (uint32_t)quotient;
    level += (uint32_t)((int32_t)difference >> 31) & difference;
    kind = r_u16(actor + 172) >> 5;
    result = kind;
    if (kind == 1 || (int32_t)level > 0)
    {
        result = sub_800A9868(actor, normal, 0, normal, bounds, 0);
        accepted = result;
    }
    if (accepted)
        return sub_80017BA0(r_s32(actor + 184), r_s32(actor + 192), 19, 0);
    return result;
}
