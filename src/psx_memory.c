#include "psx_memory.h"
#include "race_trace_runtime.h"
#include "wip.h"
#include <stdio.h>
#include <stdlib.h>

void *rrj_at_slow(RRJMemory *memory, uint32_t address, size_t bytes)
{
    const uint32_t physical = address & 0x1fffffff;
    if (physical >= 0x1f800000 && physical < 0x1f800400 && bytes <= 0x1f800400 - physical)
        return psx_addr(address, bytes);
    if (physical >= 0x1f801100 && physical < 0x1f801130 && bytes <= 0x1f801130 - physical)
        return memory->root_counters + (physical - 0x1f801100);
    if (physical >= 0x1fc00000 && physical < 0x1fc80000 && memory->bios && bytes <= 0x1fc80000 - physical)
        return memory->bios + (physical - 0x1fc00000);
    if (physical > PSX_DRAM_SIZE || bytes > PSX_DRAM_SIZE - physical)
    {
        const uint32_t args[2] = {address, (uint32_t)bytes};

        fprintf(stderr, "Unsupported PSX data address: %08X\n", address);
        uint32_t dispatch = rrj_trace_current_dispatch();

        rrj_wip_handoff(memory, dispatch ? dispatch : address, "memory", __func__, __FILE__, __LINE__, "invalid_address", args, 2);
        rrj_wip_stop(__func__, __FILE__, __LINE__);
    }
    return psx_addr(address, bytes);
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
