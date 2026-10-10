#include "solo_resource_dependencies.h"
#include "solo_resource_complete.h"
#include "xport.h"
#include <stdio.h>
#include <stdlib.h>

static uint32_t solo_resource_call(RRJMemory *m, const RRJSoloResourceServices *services, uint32_t target, uint32_t first, uint32_t second)
{
    if (!services || !services->call)
    {
        fprintf(stderr, "sub_F_800782B0 missing scalar adapter %08X\n", target);
        abort();
    }
    return services->call(m, target, first, second);
}

static uint32_t solo_resource_byte(RRJMemory *m, uint32_t address)
{
    return *(const uint8_t *)rrj_at(address, 1);
}

static void solo_resource_store_byte(RRJMemory *m, uint32_t address, uint32_t value)
{
    *(uint8_t *)rrj_at(address, 1) = (uint8_t)value;
}

static uint32_t solo_resource_flags(RRJMemory *m, uint32_t descriptor)
{
    return rrj_u16(rrj_at(descriptor, 2));
}

static void solo_resource_store_flags(RRJMemory *m, uint32_t descriptor, uint32_t flags)
{
    rrj_put16(rrj_at(descriptor, 2), (uint16_t)flags);
}

static void solo_resource_release_tags(RRJMemory *m, uint32_t tags, uint32_t count)
{
    uint32_t index;
    for (index = 0; index < count; ++index)
    {
        uint32_t tag = rrj_read32(tags + 4u * index);
        uint32_t entry;
        for (entry = 0; entry < 300; ++entry)
        {
            if (rrj_read32(0x80088E0Cu + 4u * entry) == tag)
                break;
        }
        if (entry != 300)
        {
            uint32_t flags = 0x8009DDE0u + 36u * entry;
            rrj_write32(flags, rrj_read32(flags) & ~0x10000000u);
        }
    }
}

static void solo_resource_release_subtype(RRJMemory *m, uint32_t subtype, const RRJSoloResourceServices *services)
{
    uint32_t id;
    if (subtype == 22)
    {
        solo_resource_release_tags(m, 0x800895B8u, 57);
        id = 16;
    }
    else if (subtype == 23)
    {
        solo_resource_release_tags(m, 0x8008969Cu, 36);
        id = 15;
    }
    else
    {
        solo_resource_release_tags(m, 0x800892C4u, 5);
        solo_resource_release_tags(m, 0x800892FCu, 12);
        id = 8;
    }
    (void)solo_resource_call(m, services, 0x80078C68u, id, 1);
}

static void solo_resource_append_name(RRJMemory *m, char path[128], uint32_t *length, uint32_t source)
{
    for (;;)
    {
        uint32_t character = solo_resource_byte(m, source++);
        if (!character)
        {
            path[*length] = '\0';
            return;
        }
        /* TODO The original formatter assumes both strings fit its local buffer */
        if (*length >= 127)
        {
            fputs("sub_F_800782B0 resource path exceeds 127 bytes\n", stderr);
            abort();
        }
        path[(*length)++] = (char)character;
    }
}

uint32_t sub_F_800782B0(int16_t id, uint32_t wait, const RRJSoloResourceServices *services)
{
    uint32_t descriptor, flags, kind, allocation_size = 0, handle, data;
    uint32_t record[8] = {0};
    uint32_t path_length = 0;
    char path[128];

    FUNCTION_MARKER(0x800782B0, "RASHCDF.BIN");
    if (id >= 37)
        return 0;
    descriptor = 0x800897E4u + 32u * (uint32_t)(int32_t)id;
    flags = solo_resource_flags(rrj_host_context(), descriptor);
    if (flags & 0xEu)
        return 1;
    kind = solo_resource_byte(rrj_host_context(), descriptor + 2);
    if (kind == 5)
    {
        (void)solo_resource_call(rrj_host_context(), services, 0x8007E824u, 0, 0);
        solo_resource_store_byte(rrj_host_context(), 0x8009C5DFu, 1);
        solo_resource_store_flags(rrj_host_context(), descriptor, solo_resource_flags(rrj_host_context(), descriptor) | 4u);
        return 1;
    }
    if (kind == 6)
        return 1;
    if (kind == 2)
    {
        uint32_t subtype = solo_resource_byte(rrj_host_context(), descriptor + 3);
        int32_t previous = (int8_t)solo_resource_byte(rrj_host_context(), 0x8009C5EAu);
        switch (subtype)
        {
            case 4:
                if (previous == 22 || previous == 23)
                    solo_resource_release_subtype(rrj_host_context(), (uint32_t)previous, services);
                solo_resource_store_byte(rrj_host_context(), 0x8009C5EAu, 4);
                allocation_size = 250000;
                break;
            case 22:
                if (previous == 23 || previous == 4)
                    solo_resource_release_subtype(rrj_host_context(), (uint32_t)previous, services);
                solo_resource_store_byte(rrj_host_context(), 0x8009C5EAu, 22);
                allocation_size = 250000;
                break;
            case 23:
                if (previous == 22 || previous == 4)
                    solo_resource_release_subtype(rrj_host_context(), (uint32_t)previous, services);
                solo_resource_store_byte(rrj_host_context(), 0x8009C5EAu, 23);
                allocation_size = 250000;
                break;
            case 8:
                if ((int8_t)solo_resource_byte(rrj_host_context(), 0x8009C5EBu) == 14)
                {
                    solo_resource_release_tags(rrj_host_context(), 0x800894D0u, 6);
                    (void)solo_resource_call(rrj_host_context(), services, 0x80078C68u, 19, 1);
                }
                solo_resource_store_byte(rrj_host_context(), 0x8009C5EBu, 8);
                allocation_size = 50000;
                break;
            case 14:
                if ((int8_t)solo_resource_byte(rrj_host_context(), 0x8009C5EBu) == 8)
                {
                    solo_resource_release_tags(rrj_host_context(), 0x800893BCu, 8);
                    (void)solo_resource_call(rrj_host_context(), services, 0x80078C68u, 12, 1);
                }
                solo_resource_store_byte(rrj_host_context(), 0x8009C5EBu, 14);
                allocation_size = 50000;
                break;
            default:
                break;
        }
    }
    solo_resource_append_name(rrj_host_context(), path, &path_length, 0x8005248Cu);
    solo_resource_append_name(rrj_host_context(), path, &path_length, descriptor + 8);
    if (!services || !services->open)
    {
        fputs("sub_F_800782B0 missing host-path 8001458C adapter\n", stderr);
        abort();
    }
    handle = services->open(rrj_host_context(), path, rrj_read32(0x8005AD5Cu));
    rrj_write32(descriptor + 24, handle);
    if (rrj_s32(handle) < 0)
        return 1;
    if (!allocation_size)
        allocation_size = solo_resource_call(rrj_host_context(), services, 0x800148BCu, handle, 0);
    rrj_write32(descriptor + 28, allocation_size);
    flags = solo_resource_flags(rrj_host_context(), descriptor);
    if ((flags & 0x100u) && rrj_read32(0x8009C604u) >= allocation_size)
    {
        data = rrj_read32(0x8009C600u);
        rrj_write32(descriptor + 4, data);
        rrj_write32(0x8009C604u, rrj_read32(0x8009C604u) - allocation_size);
        rrj_write32(0x8009C600u, rrj_read32(0x8009C600u) + rrj_read32(descriptor + 28));
        solo_resource_store_flags(rrj_host_context(), descriptor, solo_resource_flags(rrj_host_context(), descriptor) | 1u);
    }
    else
    {
        data = solo_resource_call(rrj_host_context(), services, 0x8001447Cu, allocation_size, 1);
        rrj_write32(descriptor + 4, data);
    }
    if (!rrj_read32(descriptor + 4))
        return 1;
    record[0] = 512;
    record[1] = rrj_read32(descriptor + 24);
    record[4] = rrj_read32(descriptor + 4);
    record[5] = descriptor;
    record[6] = 0x800781A8u;
    record[7] = rrj_read32(descriptor + 28);
    solo_resource_store_flags(rrj_host_context(), descriptor, solo_resource_flags(rrj_host_context(), descriptor) | 8u);
    if (!services->submit)
    {
        fputs("sub_F_800782B0 missing host-record 80022D20 adapter\n", stderr);
        abort();
    }
    if (services->submit(rrj_host_context(), record) == 0xFFFFFF9Cu)
    {
        if (solo_resource_call(rrj_host_context(), services, 0x80022A78u, 2, 0))
        {
            rrj_write32(0x8005ACACu, 0);
            solo_resource_store_flags(rrj_host_context(), descriptor, solo_resource_flags(rrj_host_context(), descriptor) & 0xFFF7u);
            return 1;
        }
        if (services->submit(rrj_host_context(), record) == 0xFFFFFF9Cu)
        {
            solo_resource_store_flags(rrj_host_context(), descriptor, solo_resource_flags(rrj_host_context(), descriptor) & 0xFFF7u);
            return 1;
        }
    }
    if ((solo_resource_flags(rrj_host_context(), descriptor) & 0x200u) || wait)
    {
        (void)solo_resource_call(rrj_host_context(), services, 0x80022A78u, 2, 0);
        rrj_write32(0x8005ACACu, 0);
    }
    return 1;
}

static uint32_t solo_resource_complete_call(RRJMemory *m, RRJRaceLeafCall call, uint32_t target, uint32_t value)
{
    const uint32_t arguments[8] = {value, 0, 0, 0, 0, 0, 0, 0};
    if (!call)
    {
        fprintf(stderr, "sub_F_800781A8 missing completion adapter %08X\n", target);
        abort();
    }
    return call(m, target, arguments);
}

uint32_t sub_F_800781A8(uint32_t unused0, uint32_t unused1, uint32_t descriptor, RRJRaceLeafCall call)
{
    uint32_t flags = solo_resource_flags(rrj_host_context(), descriptor);
    uint32_t kind, result;

    FUNCTION_MARKER(0x800781A8, "RASHCDF.BIN");
    if (!(flags & 8u))
        return 0;
    (void)solo_resource_complete_call(rrj_host_context(), call, 0x8001460Cu, rrj_read32(descriptor + 24));
    flags = solo_resource_flags(rrj_host_context(), descriptor);
    rrj_write32(descriptor + 24, UINT32_MAX);
    kind = solo_resource_byte(rrj_host_context(), descriptor + 2);
    solo_resource_store_flags(rrj_host_context(), descriptor, (flags | 4u) & 0xFFF7u);
    switch (kind)
    {
        case 1:
            (void)solo_resource_complete_call(rrj_host_context(), call, 0x80065248u, descriptor);
            break;
        case 2:
            (void)sub_F_80076370(descriptor);
            break;
        case 3:
            (void)solo_resource_complete_call(rrj_host_context(), call, 0x80065CA8u, descriptor);
            break;
        case 4:
            (void)solo_resource_complete_call(rrj_host_context(), call, 0x80065F4Cu, descriptor);
            break;
        default:
            break;
    }
    result = solo_resource_flags(rrj_host_context(), descriptor) & 0x400u;
    if (result)
    {
        uint32_t count = rrj_read32(0x8009D4DCu);
        result = 0x800A0970u;
        rrj_write32(result + 4u * count, descriptor);
        rrj_write32(0x8009D4DCu, count + 1u);
    }
    return result;
}
