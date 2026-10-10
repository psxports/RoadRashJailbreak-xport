#include "gpu.h"
#include "psx_gpu.h"
#include "race_trace_runtime.h"

#include <stdio.h>
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
    data = (uint32 *)rrj_at(pixels, (size_t)rectangle.w * rectangle.h * 2);
    return (uint32)LoadImagePSX(&rectangle, data);
}

void rrj_gpu_clear_ot(RRJMemory *memory, uint32 address, uint32 count)
{
    ClearOTagR((uint32 *)rrj_at(address, (size_t)count * sizeof(uint32)), (sint32)count);
    /* Retain the title-specific resident GPU barrier tail */
    rrj_write32(0x8005602C, 0x04056018);
    rrj_write32(address, 0x0005602C);
}

static int live_packet_failure(uint32 address, uint32 tag, const char *reason)
{
    fprintf(stderr, "rrj_live_gpu_packet_failure address=%08x tag=%08x reason=%s\n", address, tag, reason);
    return 0;
}

static int live_draw_packet(uint32 address, uint32 *packet)
{
    uint32 count = packet[0] >> 24;
    uint32 polygon_words = 0;
    uint32 command;
    uint32 index;
    uint32 split[13];

    if (!count)
        return 1;
    if ((packet[1] >> 24) != 0xE2u || count == 1)
    {
        DrawPrim(packet);
        return 1;
    }
    command = (packet[2] >> 24) & 0xFCu;
    switch (command)
    {
        case 0x24:
            polygon_words = 7;
            break;
        case 0x2C:
            polygon_words = 9;
            break;
        case 0x34:
            polygon_words = 9;
            break;
        case 0x3C:
            polygon_words = 12;
            break;
        default:
            break;
    }
    if (!polygon_words)
    {
        /* Standalone state packets may contain state words and NOP padding */
        for (index = 2; index <= count; ++index)
        {
            uint32 code = packet[index] >> 24;
            if (code != 0 && (code < 0xE1u || code > 0xE6u))
                return live_packet_failure(address, packet[0], "unsupported_compound_command");
        }
        DrawPrim(packet);
        return 1;
    }
    if (count != polygon_words + 2 || (packet[count] >> 24) != 0xE2u)
        return live_packet_failure(address, packet[0], "invalid_texture_window_sandwich");

    /* Submit the original GP0 stream without changing its guest DMA packet */
    split[0] = 0x01FFFFFFu;
    split[1] = packet[1];
    DrawPrim(split);
    split[0] = (polygon_words << 24) | 0x00FFFFFFu;
    memcpy(split + 1, packet + 2, polygon_words * sizeof(uint32));
    DrawPrim(split);
    split[0] = 0x01FFFFFFu;
    split[1] = packet[count];
    DrawPrim(split);
    return 1;
}

int rrj_gpu_draw_ot(RRJMemory *memory, uint32 address, int origin_x, int origin_y)
{
    uint32 *ot = (uint32 *)rrj_at(address, sizeof(uint32));
    uint32 link;
    uint32 visited = 0;

    gpu_register_dma_range(DRAM, PSX_DRAM_SIZE);
    if (!rrj_trace_runtime_is_live())
    {
        DrawOTag(ot);
        return 1;
    }
    link = address & 0x00FFFFFFu;
    while (link != 0x00FFFFFFu)
    {
        uint32 tag;
        uint32 words;
        uint32 *packet;

        if ((link & 3u) || link > PSX_DRAM_SIZE - sizeof(uint32) || visited++ >= 65536u)
            return live_packet_failure(link, 0, "invalid_dma_link");
        tag = rrj_read32(link);
        words = tag >> 24;
        if (words > (PSX_DRAM_SIZE - link - sizeof(uint32)) / sizeof(uint32))
            return live_packet_failure(link, tag, "packet_outside_dram");
        packet = (uint32 *)rrj_at(link, (words + 1u) * sizeof(uint32));
        if (!live_draw_packet(link, packet))
            return 0;
        link = tag & 0x00FFFFFFu;
    }
    return 1;
}

void rrj_gpu_environment(uint32 word, int origin_x, int origin_y)
{
    uint32 packet[2] = {0x01ffffffu, word};

    DrawPrim(packet);
}
