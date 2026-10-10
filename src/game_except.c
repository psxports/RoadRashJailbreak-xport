#include "xport_trace.h"

/* Boot-reserved stack above the heap end, see original-stack-contract-audit.json */
const MEMORY_EXCEPTION game_memory_exceptions[] = {{MEMORY_REGION_DRAM, 0x001FDFF8u, 0x00002008u, MEMORY_EXCEPTION_GUEST_STACK}};
const uint32 game_memory_exception_count = 1u;
