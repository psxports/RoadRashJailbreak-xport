#include "rrj_debug.h"
#include "psx_memory.h"
#include "psx_spu.h"
#include "wip.h"
#include <stdio.h>
#include <stdlib.h>

static sint32 rrj_mmio_read(void *user, uint32 address, uint32 width, uint32 *value)
{
    RRJMemory *memory = (RRJMemory *)user;
    uint32 offset = address - 0x1f801100u;
    if (address >= 0x1F801C00 && address < 0x1F801E00)
        return spu_mmio_read(NULL, address, width, value);
    if (!memory || !value || offset > sizeof(memory->root_counters) || width > sizeof(memory->root_counters) - offset || (width != 1u && width != 2u && width != 4u))
        return 0;
    if ((offset & 15) < 4 && width <= 4 - (offset & 15))
    {
        uint32 counter = (uint32)GetRCnt(offset >> 4);
        uint32 register_byte = offset & 15;
        *value = counter >> (8 * register_byte);
        if (width == 1)
            *value &= 255;
        else if (width == 2)
            *value &= 65535;
        return 1;
    }
    *value = memory->root_counters[offset];
    if (width > 1)
        *value |= (uint32)memory->root_counters[offset + 1] << 8;
    if (width > 2)
        *value |= (uint32)memory->root_counters[offset + 2] << 16 | (uint32)memory->root_counters[offset + 3] << 24;
    return 1;
}

static sint32 rrj_mmio_write(void *user, uint32 address, uint32 width, uint32 value, uint32 byte_mask)
{
    RRJMemory *memory = (RRJMemory *)user;
    uint32 offset = address - 0x1f801100u;
    uint32 index;
    if (address >= 0x1F801C00 && address < 0x1F801E00)
        return spu_mmio_write(NULL, address, width, value, byte_mask);
    if (!memory || offset > sizeof(memory->root_counters) || width > sizeof(memory->root_counters) - offset || (width != 1u && width != 2u && width != 4u))
        return 0;
    for (index = 0; index < width; ++index)
        if (byte_mask & (1u << index))
            memory->root_counters[offset + index] = (uint8)(value >> (index * 8u));
    return 1;
}

static RRJMemory *active_host_context;

RRJMemory *rrj_host_context(void)
{
    if (!active_host_context)
    {
        RRJ_DEBUG_FPRINTF(stderr, "Unbound Road Rash host device context\n");
        abort();
    }
    return active_host_context;
}

void rrj_display_wait_step(void)
{
    RRJDisplayWaitService service = rrj_host_context()->display_wait_service;
    if (!service)
    {
        RRJ_DEBUG_FPUTS("RRJ: missing host display-wait service\n", stderr);
        abort();
    }
    service();
}

extern CdGuestCallback rrj_cd_callback(uint32 address);

void rrj_memory_bind(RRJMemory *memory)
{
    static const CD_GUEST_BINDING cd_binding = {
        0x8005AF60, 0x8005B0D8, 0x8005B5B8, 0x8005B0DC,
        0x80054B74, 0x80054AD4, 0x80054B54, 0x80054E06, rrj_cd_callback
    };
    if (!memory)
        return;
    active_host_context = memory;
    cd_bind_guest(&cd_binding);
    spu_bind_key_guest(0x8005A3A8u, 0x8005A408u, 0x8005A3D4u, 0x8005A3D0u, 0x800DC0A8u);
    xport_memory_bind_bios(memory->bios, memory->bios ? 512u * 1024u : 0u);
    xport_memory_bind_mmio(0x1f801100u, 0xD00, rrj_mmio_read, rrj_mmio_write, memory);
}
