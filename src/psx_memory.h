#ifndef RRJ_MEMORY_H
#define RRJ_MEMORY_H
#include "types.h"
#include "psx.h"
#include "race_native_abi.h"

/* Transitional storage for original 32-bit addresses and packed data. This is
 * a byte-addressed data arena, not a CPU emulator. Native functions use ordinary
 * C arguments/locals. Keep physical/KSEG aliases in packet links unchanged. */
typedef struct RRJMemory RRJMemory;
typedef void (*RRJDisplayWaitService)(void);

struct RRJMemory
{
    void *sdk_user;
    uint32_t cpu_status;
    /* Original SP at the resident menu continuation */
    uint32_t menu_stack_pointer;
    /* Optional immutable BIOS image used by exact trace replay */
    uint8 *bios;
    /* Explicit root-counter MMIO registers */
    uint8 root_counters[48];
    RRJNativeCheckpointABI checkpoint_abi;
    int checkpoint_abi_valid;
    RRJDisplayWaitService display_wait_service;
};

void rrj_memory_bind(RRJMemory *memory);

/* Host device state is separate from original game arguments */
RRJMemory *rrj_host_context(void);
void rrj_display_wait_step(void);

#endif
