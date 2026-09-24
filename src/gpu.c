#include "gpu.h"
#include "psx_gpu.h"

#include <string.h>

static int valid_upload(const PSX_RECT *rectangle)
{
    return rectangle->x >= 0 && rectangle->y >= 0 && rectangle->w > 0 && rectangle->h > 0 && rectangle->x + rectangle->w <= GPU_VRAM_WIDTH && rectangle->y + rectangle->h <= GPU_VRAM_HEIGHT;
}

uint32 rrj_gpu_upload(RRJMemory *memory, const uint8 rect[8], uint32 pixels)
{
    PSX_RECT rectangle;
    uint32 *data;

    memcpy(&rectangle, rect, sizeof(rectangle));
    if (!valid_upload(&rectangle))
        return (uint32)-1;
    data = (uint32 *)rrj_at(memory, pixels, (size_t)rectangle.w * rectangle.h * 2);
    return (uint32)LoadImagePSX(&rectangle, data);
}

void rrj_gpu_clear_ot(RRJMemory *memory, uint32 address, uint32 count)
{
    ClearOTagR((uint32 *)rrj_at(memory, address, (size_t)count * sizeof(uint32)), (sint32)count);
    /* Retain the title-specific resident GPU barrier tail */
    rrj_write32(memory, 0x8005602C, 0x04056018);
    rrj_write32(memory, address, 0x0005602C);
}

int rrj_gpu_draw_ot(RRJMemory *memory, uint32 address, int origin_x, int origin_y)
{
    uint32 *ot = (uint32 *)rrj_at(memory, address, sizeof(uint32));

    gpu_register_dma_range(DRAM, PSX_DRAM_SIZE);
    DrawOTag(ot);
    return 1;
}

void rrj_gpu_environment(uint32 word, int origin_x, int origin_y)
{
    uint32 packet[2] = {0x01ffffffu, word};

    gpu_packet(packet);
}
