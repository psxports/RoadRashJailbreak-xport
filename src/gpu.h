#ifndef RRJ_GPU_H
#define RRJ_GPU_H

#include "psx.h"
#include "psx_memory.h"

int rrj_gpu_draw_ot(RRJMemory *memory, uint32 address, int origin_x, int origin_y);
void rrj_gpu_clear_ot(RRJMemory *memory, uint32 address, uint32 count);
uint32 rrj_gpu_upload(RRJMemory *memory, const uint8 rect[8], uint32 pixels);
void rrj_gpu_environment(uint32 word, int origin_x, int origin_y);

#endif
