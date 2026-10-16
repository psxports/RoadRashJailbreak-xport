#include "rrj_debug.h"
#include "solo_file_dependencies.h"
#include "music_start.h"
#include "solo_resource_dependencies.h"
#include "solo_resource_complete.h"
#include "xport.h"
#include <stdio.h>
#include <stdlib.h>

static void solo_resource_release_tags(uint32_t tags, uint32_t count)
{
    uint32_t index;
    for (index = 0; index < count; ++index)
    {
        uint32_t tag = r_u32(tags + 4u * index);
        uint32_t entry;
        for (entry = 0; entry < 300; ++entry)
        {
            if (r_u32(0x80088E0Cu + 4u * entry) == tag)
                break;
        }
        if (entry != 300)
        {
            uint32_t flags = 0x8009DDE0u + 36u * entry;
            w_u32(flags, r_u32(flags) & ~0x10000000u);
        }
    }
}

static void solo_resource_release_subtype(uint32_t subtype)
{
    uint32_t id;
    if (subtype == 22)
    {
        solo_resource_release_tags(0x800895B8u, 57);
        id = 16;
    }
    else if (subtype == 23)
    {
        solo_resource_release_tags(0x8008969Cu, 36);
        id = 15;
    }
    else
    {
        solo_resource_release_tags(0x800892C4u, 5);
        solo_resource_release_tags(0x800892FCu, 12);
        id = 8;
    }
    (void)sub_80078C68(id, 1);
}

static void solo_resource_append_name(char path[128], uint32_t *length, uint32_t source)
{
    for (;;)
    {
        uint32_t character = (uint8_t)r_u8((source++));
        if (!character)
        {
            path[*length] = '\0';
            return;
        }
        /* TODO The original formatter assumes both strings fit its local buffer */
        if (*length >= 127)
        {
            RRJ_DEBUG_FPUTS("sub_800782B0 resource path exceeds 127 bytes\n", stderr);
            abort();
        }
        path[(*length)++] = (char)character;
    }
}

uint32_t sub_800782B0(int16_t id, uint32_t wait)
{
    uint32_t descriptor, flags, kind, allocation_size = 0, handle, data;
    uint32_t record[8] = {0};
    uint32_t path_length = 0;
    char path[128];

    FUNCTION_MARKER(0x800782B0, "RASHCDF.BIN");
    if (id >= 37)
        return 0;
    descriptor = 0x800897E4u + 32u * (uint32_t)(int32_t)id;
    flags = (uint32_t)r_u16(descriptor);
    if (flags & 0xEu)
        return 1;
    kind = (uint8_t)r_u8((descriptor + 2));
    if (kind == 5)
    {
        (void)sub_8007E824();
        w_u8(0x8009C5DFu, ((uint8_t)(1)));
        w_u16(descriptor, (uint16_t)((uint32_t)r_u16(descriptor) | 4u));
        return 1;
    }
    if (kind == 6)
        return 1;
    if (kind == 2)
    {
        uint32_t subtype = (uint8_t)r_u8((descriptor + 3));
        int32_t previous = (int8_t)r_s8(0x8009C5EAu);
        switch (subtype)
        {
            case 4:
                if (previous == 22 || previous == 23)
                    solo_resource_release_subtype((uint32_t)previous);
                w_u8(0x8009C5EAu, ((uint8_t)(4)));
                allocation_size = 250000;
                break;
            case 22:
                if (previous == 23 || previous == 4)
                    solo_resource_release_subtype((uint32_t)previous);
                w_u8(0x8009C5EAu, ((uint8_t)(22)));
                allocation_size = 250000;
                break;
            case 23:
                if (previous == 22 || previous == 4)
                    solo_resource_release_subtype((uint32_t)previous);
                w_u8(0x8009C5EAu, ((uint8_t)(23)));
                allocation_size = 250000;
                break;
            case 8:
                if ((int8_t)r_s8(0x8009C5EBu) == 14)
                {
                    solo_resource_release_tags(0x800894D0u, 6);
                    (void)sub_80078C68(19, 1);
                }
                w_u8(0x8009C5EBu, ((uint8_t)(8)));
                allocation_size = 50000;
                break;
            case 14:
                if ((int8_t)r_s8(0x8009C5EBu) == 8)
                {
                    solo_resource_release_tags(0x800893BCu, 8);
                    (void)sub_80078C68(12, 1);
                }
                w_u8(0x8009C5EBu, ((uint8_t)(14)));
                allocation_size = 50000;
                break;
            default:
                break;
        }
    }
    solo_resource_append_name(path, &path_length, 0x8005248Cu);
    solo_resource_append_name(path, &path_length, descriptor + 8);

    handle = rrj_solo_open_path(path, r_u32(0x8005AD5Cu));
    w_u32(descriptor + 24, handle);
    if (((int32_t)(uint32_t)(handle)) < 0)
        return 1;
    if (!allocation_size)
        allocation_size = sub_800148BC(handle);
    w_u32(descriptor + 28, allocation_size);
    flags = (uint32_t)r_u16(descriptor);
    if ((flags & 0x100u) && r_u32(0x8009C604u) >= allocation_size)
    {
        data = r_u32(0x8009C600u);
        w_u32(descriptor + 4, data);
        w_u32(0x8009C604u, r_u32(0x8009C604u) - allocation_size);
        w_u32(0x8009C600u, r_u32(0x8009C600u) + r_u32(descriptor + 28));
        w_u16(descriptor, (uint16_t)((uint32_t)r_u16(descriptor) | 1u));
    }
    else
    {
        data = sub_8001447C(allocation_size, 1);
        w_u32(descriptor + 4, data);
    }
    if (!r_u32(descriptor + 4))
        return 1;
    record[0] = 512;
    record[1] = r_u32(descriptor + 24);
    record[4] = r_u32(descriptor + 4);
    record[5] = descriptor;
    record[6] = 0x800781A8u;
    record[7] = r_u32(descriptor + 28);
    w_u16(descriptor, (uint16_t)((uint32_t)r_u16(descriptor) | 8u));

    if (sub_80022D20(record) == 0xFFFFFF9Cu)
    {
        if (sub_80022A78(2))
        {
            w_u32(0x8005ACACu, 0);
            w_u16(descriptor, (uint16_t)((uint32_t)r_u16(descriptor) & 0xFFF7u));
            return 1;
        }
        if (sub_80022D20(record) == 0xFFFFFF9Cu)
        {
            w_u16(descriptor, (uint16_t)((uint32_t)r_u16(descriptor) & 0xFFF7u));
            return 1;
        }
    }
    if (((uint32_t)r_u16(descriptor) & 0x200u) || wait)
    {
        (void)sub_80022A78(2);
        w_u32(0x8005ACACu, 0);
    }
    return 1;
}

uint32_t sub_800781A8(uint32_t descriptor)
{
    uint32_t flags = (uint32_t)r_u16(descriptor);
    uint32_t kind, result;

    FUNCTION_MARKER(0x800781A8, "RASHCDF.BIN");
    if (!(flags & 8u))
        return 0;
    (void)sub_8001460C(r_u32(descriptor + 24));
    flags = (uint32_t)r_u16(descriptor);
    w_u32(descriptor + 24, UINT32_MAX);
    kind = (uint8_t)r_u8((descriptor + 2));
    w_u16(descriptor, (uint16_t)((flags | 4u) & 0xFFF7u));
    switch (kind)
    {
        case 1:
            (void)sub_80065248(descriptor);
            break;
        case 2:
            (void)sub_80076370(descriptor);
            break;
        case 3:
            (void)sub_80065CA8(descriptor);
            break;
        case 4:
            (void)sub_80065F4C(descriptor);
            break;
        default:
            break;
    }
    result = (uint32_t)r_u16(descriptor) & 0x400u;
    if (result)
    {
        uint32_t count = r_u32(0x8009D4DCu);
        result = 0x800A0970u;
        w_u32(result + 4u * count, descriptor);
        w_u32(0x8009D4DCu, count + 1u);
    }
    return result;
}
