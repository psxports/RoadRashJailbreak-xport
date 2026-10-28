#include "rrj_runtime.h"
#include "rrj_data.h"
#include "psx_gpu.h"
#include <stdio.h>
#include <string.h>

static uint8_t bios[524288];
static int frame_started;

static int load_data(const char *path, void *data, size_t size)
{
    FILE *file = xport_fopen(path, "rb");
    int ok;
    if (!file)
        return 0;
    ok = xport_fread(data, 1, size, file) == size;
    fclose(file);
    return ok;
}

int rrj_runtime_init(RRJMemory *memory)
{
    const GpuPsyqStateBinding gpu_state = {
        0x80055FD8, 0x80055F7C, 0x80055F6C, 0x80055F6F, 0x80055F0C, 0
    };
    memset(DRAM, 0, sizeof(DRAM));
    memset(SCRATCHPAD, 0, sizeof(SCRATCHPAD));
    if (!load_data("DATA/RRJ_RESIDENT.BIN", DRAM + 0x10000, 0x4B800) ||
        !load_data("../tools/duckstation/data/user/bios/scph5502.bin", bios, sizeof(bios)))
        return 0;
    memory->bios = bios;
    rrj_memory_bind(memory);
    /* Original CRT initializes BSS and the game heap before main */
    memset(DRAM + 0x5B1F0, 0, 0xDC0C8 - 0x5B1F0);
    w_u32(0x800548D4, 0x800DC0C8);
    w_u32(0x800548D8, r_u32(0x8005AC98) - 8 - r_u32(0x8005AC9C) - 0xDC0C8);
    frame_started = 0;
    return gpu_bind_psyq_state(&gpu_state) && rrj_data_init();
}

void rrj_runtime_input(uint32_t controller, uint32_t raw)
{
    uint32_t buttons = PadRead((sint32)controller);
    uint8_t *packet = xport_guest_ptr(raw, 4);
    packet[0] = 0;
    packet[1] = 0x41;
    packet[2] = (uint8_t)(~buttons >> 8);
    packet[3] = (uint8_t)~buttons;
}

int rrj_runtime_frame(void)
{
    if (frame_started)
    {
        if (!gpu_present())
            return 0;
        VSync(0);
    }
    gpu_begin();
    frame_started = 1;
    return 1;
}
