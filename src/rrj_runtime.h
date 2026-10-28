#ifndef RRJ_RUNTIME_H
#define RRJ_RUNTIME_H
#include "psx_memory.h"
int rrj_runtime_init(RRJMemory *memory);
void rrj_runtime_input(uint32_t controller, uint32_t raw);
int rrj_runtime_frame(void);
void rrj_spu_completion_service(void);
#endif
