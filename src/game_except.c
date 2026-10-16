#include "xport_trace.h"

/* SDK interrupt stack, see status/repairs/lockstep-race/sdk-interrupt-stack.json */
/* Boot stack above the heap end, see original-stack-contract-audit.json */
const MEMORY_EXCEPTION game_memory_exceptions[] = {
    /* Timer2 interrupt epoch, excluded by the requested game-data scope */
    {MEMORY_REGION_DRAM, 0x000527D8, 8, MEMORY_EXCEPTION_TRUSTED_DEVICE_STATE},
    {MEMORY_REGION_DRAM, 0x00054E6Cu, 0x00001000u, MEMORY_EXCEPTION_GUEST_STACK},
    /* PsyQ VSync epoch, maintained by the SDK VBlank dispatcher */
    {MEMORY_REGION_DRAM, 0x00055ECC, 4, MEMORY_EXCEPTION_TRUSTED_DEVICE_STATE},
    /* PsyQ DMA2 callback slot, owned by the trusted GPU wrapper */
    {MEMORY_REGION_DRAM, 0x00055EE8u, 0x00000004u, MEMORY_EXCEPTION_TRUSTED_DEVICE_STATE},
    /* PsyQ GPU queue cursors, interrupt mask and timeout bookkeeping */
    {MEMORY_REGION_DRAM, 0x00056094u, 0x0000001Cu, MEMORY_EXCEPTION_TRUSTED_DEVICE_STATE},
    {MEMORY_REGION_DRAM, 0x001FDFF8u, 0x00002008u, MEMORY_EXCEPTION_GUEST_STACK},
    /* User-approved blind spot, see approved-stack-audit-frame729.json */
    {MEMORY_REGION_SCRATCHPAD, 0x000003C0, 0x00000024, MEMORY_EXCEPTION_GUEST_STACK}
};
const uint32 game_memory_exception_count = 7;
