#include "solo_file_dependencies.h"
#include <stdio.h>
#include <stdlib.h>

static void solo_path_overflow(void)
{
    fputs("rrj_solo_open_path: original 128-byte expanded path capacity exceeded\n", stderr);
    abort();
}

uint32_t rrj_solo_bios_toupper(RRJMemory *m, uint32_t value)
{
    uint32_t character = value & 255u;
    if (!m->bios)
    {
        fputs("rrj_solo_bios_toupper: missing BIOS image\n", stderr);
        abort();
    }
    if (m->bios[0xDDB1u + character] & 2u)
        character = (character - 32u) & 255u;
    return (uint32_t)(int32_t)(int8_t)character;
}

static uint32_t solo_path_hash(RRJMemory *m, const char *path)
{
    const char *start = path;
    const char *cursor;
    uint32_t hash = 0;
    uint32_t shift = 0;
    for (cursor = path; *cursor; ++cursor)
        if (*cursor == '\\')
            start = cursor;
    while (*start == '\\')
        ++start;
    while (*start)
    {
        uint32_t value = rrj_solo_bios_toupper(m, (uint8_t)*start++);
        hash = ((hash ^ (value << shift)) + value * value) ^ (value << ((24 - shift) & 31));
        shift = (shift + 1) % 24;
    }
    return hash;
}

uint32_t rrj_solo_open_path(RRJMemory *m, const char *path, uint32_t device)
{
    char expanded[128];
    uint32_t index;
    uint32_t record;
    uint32_t table;
    uint32_t count;
    uint32_t hash;
    uint32_t cursor = 0;
    uint32_t device_table = rrj_read32(0x8005B3B0u);
    if (rrj_s32(device) < 0 || rrj_s32(device) >= rrj_s32(rrj_read32(device_table + 32)))
        return 0xFFFFFFF8u;
    for (index = 0; index < 16; ++index)
        if (!rrj_read32(0x800D65D0u + 20 * index))
            break;
    if (index == 16)
        return 0xFFFFFFFFu;
    record = 0x800D65D0u + 20 * index;
    /* Both device branches pass an unused flag to the same path resolver */
    if (path[0] != '\\' && path[1] != ':')
    {
        while (r_u8(0x8005AD60u + cursor))
        {
            if (cursor == sizeof expanded - 1)
                solo_path_overflow();
            expanded[cursor] = (char)r_u8(0x8005AD60u + cursor);
            ++cursor;
        }
    }
    while (*path)
    {
        if (cursor == sizeof expanded - 1)
            solo_path_overflow();
        expanded[cursor++] = *path++;
    }
    expanded[cursor] = 0;
    table = rrj_read32(0x8005AD6Cu);
    count = rrj_read32(0x8005AD70u);
    if (table)
    {
        hash = solo_path_hash(m, expanded);
        for (cursor = 0; rrj_s32(cursor) < rrj_s32(count); ++cursor)
        {
            uint32_t entry = table + 12 * cursor;
            if (rrj_read32(entry + 8) == hash)
            {
                rrj_write32(record + 4, rrj_read32(entry));
                rrj_write32(record + 12, rrj_read32(entry + 4));
                rrj_write32(record + 8, 0);
                rrj_write32(record + 16, 0);
                rrj_write32(record, rrj_read32(record) | 1);
                return index;
            }
        }
    }
    rrj_write32(record + 12, 0);
    rrj_write32(record + 4, 0);
    return 0xFFFFFFFFu;
}
