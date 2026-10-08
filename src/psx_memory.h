#ifndef RRJ_MEMORY_H
#define RRJ_MEMORY_H
#include "types.h"
#include "psx.h"
#include "race_native_abi.h"

/* Transitional storage for original 32-bit addresses and packed data. This is
 * a byte-addressed data arena, not a CPU emulator. Native functions use ordinary
 * C arguments/locals. Keep physical/KSEG aliases in packet links unchanged. */
typedef struct RRJMemory RRJMemory;
typedef struct RRJNativeDispatchContext RRJNativeDispatchContext;
typedef void (*RRJSDKCall)(RRJMemory *memory, uint32_t function, uint32_t a0, uint32_t a1);

struct RRJMemory
{
    RRJSDKCall sdk_call;
    void *sdk_user;
    uint32_t cpu_status;
    /* Optional immutable BIOS image used by exact trace replay */
    uint8 *bios;
    /* Explicit root-counter MMIO registers */
    uint8 root_counters[48];
    RRJNativeCheckpointABI checkpoint_abi;
    int checkpoint_abi_valid;
    RRJNativeDispatchContext *native_dispatch;
};

void *rrj_at_slow(RRJMemory *memory, uint32_t address, size_t bytes);
void rrj_memory_bind(RRJMemory *memory);

static RRJ_FORCEINLINE void *rrj_at(RRJMemory *memory, uint32_t address, size_t bytes)
{
    const uint32_t physical = address & 0x1fffffff;
    if (physical >= 0x1f800000 && physical < 0x1f800400 && bytes <= 0x1f800400 - physical)
        return xport_guest_ptr(address, bytes);
    if (physical >= 0x1f801100 && physical < 0x1f801130 && bytes <= 0x1f801130 - physical)
        return memory->root_counters + (physical - 0x1f801100);
    if (physical >= 0x1fc00000 && physical < 0x1fc80000 && memory->bios && bytes <= 0x1fc80000 - physical)
        return (void *)xport_guest_cptr(address, bytes);
    if (physical <= PSX_DRAM_SIZE && bytes <= PSX_DRAM_SIZE - physical)
        return xport_guest_ptr(address, bytes);
    return rrj_at_slow(memory, address, bytes);
}

static RRJ_FORCEINLINE uint32_t rrj_read32(RRJMemory *memory, uint32_t address)
{
    return r_u32(address);
}

static RRJ_FORCEINLINE void rrj_write32(RRJMemory *memory, uint32_t address, uint32_t value)
{
    (void)w_u32(address, value);
}

void rrj_sdk_call(RRJMemory *memory, uint32_t function, uint32_t a0, uint32_t a1);
#endif
