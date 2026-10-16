#ifndef RRJ_GPU_H
#define RRJ_GPU_H

#include "psx.h"
#include "psx_memory.h"

int rrj_gpu_draw_ot(uint32 address);
uint32 rrj_gpu_clear_ot(uint32 address, uint32 count);
uint32 rrj_gpu_upload(const uint8 rect[8], uint32 pixels);
void rrj_gpu_environment(uint32 word);

#endif
