#include "psx_memory.h"
#include "race_trace_runtime.h"
#include "wip.h"
#include <stdio.h>
#include <stdlib.h>

static uint32_t rrj_mmio_load(const uint8 *bytes, uint32 width)
{
    uint32_t value = bytes[0];
    if (width > 1u)
        value |= (uint32_t)bytes[1] << 8;
    if (width > 2u)
        value |= (uint32_t)bytes[2] << 16 | (uint32_t)bytes[3] << 24;
    return value;
}

static sint32 rrj_mmio_read(void *user, uint32 address, uint32 width, uint32 *value)
{
    RRJMemory *memory = (RRJMemory *)user;
    uint32 offset = address - 0x1f801100u;
    if (!memory || !value || offset > sizeof(memory->root_counters) || width > sizeof(memory->root_counters) - offset || (width != 1u && width != 2u && width != 4u))
        return 0;
    *value = rrj_mmio_load(memory->root_counters + offset, width);
    return 1;
}

static sint32 rrj_mmio_write(void *user, uint32 address, uint32 width, uint32 value, uint32 byte_mask)
{
    RRJMemory *memory = (RRJMemory *)user;
    uint32 offset = address - 0x1f801100u;
    uint32 index;
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
        fprintf(stderr, "Unbound Road Rash host device context\n");
        abort();
    }
    return active_host_context;
}

void rrj_memory_bind(RRJMemory *memory)
{
    if (!memory)
        return;
    active_host_context = memory;
    xport_memory_bind_bios(memory->bios, memory->bios ? 512u * 1024u : 0u);
    xport_memory_bind_mmio(0x1f801100u, sizeof(memory->root_counters), rrj_mmio_read, rrj_mmio_write, memory);
}

void *rrj_at_slow(uint32_t address, size_t bytes)
{
    RRJMemory *memory = rrj_host_context();
    const uint32_t physical = address & 0x1fffffff;
    if (physical >= 0x1f800000 && physical < 0x1f800400 && bytes <= 0x1f800400 - physical)
        return xport_guest_ptr(address, bytes);
    if (physical >= 0x1f801100 && physical < 0x1f801130 && bytes <= 0x1f801130 - physical)
        return memory->root_counters + (physical - 0x1f801100);
    if (physical >= 0x1fc00000 && physical < 0x1fc80000 && memory->bios && bytes <= 0x1fc80000 - physical)
        return (void *)xport_guest_cptr(address, bytes);
    if (physical > PSX_DRAM_SIZE || bytes > PSX_DRAM_SIZE - physical)
    {
        const uint32_t args[2] = {address, (uint32_t)bytes};

        fprintf(stderr, "Unsupported PSX data address: %08X\n", address);
        uint32_t dispatch = rrj_trace_current_dispatch();

        rrj_wip_handoff(memory, dispatch ? dispatch : address, "memory", __func__, __FILE__, __LINE__, "invalid_address", args, 2);
        rrj_wip_stop(__func__, __FILE__, __LINE__);
    }
    return xport_guest_ptr(address, bytes);
}

void rrj_sdk_call(RRJMemory *memory, uint32_t function, uint32_t a0, uint32_t a1)
{
    if (!memory->sdk_call)
    {
        fprintf(stderr, "Unbound SDK boundary %08X; no GPU backend is installed yet.\n", function);
        rrj_wip_stop(__func__, __FILE__, __LINE__);
    }
    memory->sdk_call(memory, function, a0, a1);
}
