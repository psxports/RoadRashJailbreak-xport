#include "platform_smoke.h"

#include "psx.h"
#include "psx_gpu.h"

#include <stdio.h>
#include <string.h>

int rrj_platform_smoke(unsigned frames, int headless)
{
    POLY_F4 quad;
    unsigned frame;
    unsigned colored = 0;
    uint32 checksum = 2166136261u;
    sint32 ticks;

    xport_set_headless(headless);
    ResetGraph(0);
    PadInit(0);
    gpu_set_display(0, 0, 320, 240);
    memset(&quad, 0, sizeof(quad));
    setPolyF4(&quad);
    setRGB0(&quad, 180, 50, 20);
    setXY4(&quad, 20, 20, 180, 20, 20, 100, 180, 100);
    for (frame = 0; frame < frames && !xport_isquit(); ++frame)
    {
        gpu_begin();
        DrawPrim(&quad);
        if (!gpu_present())
            return 2;
        (void)PadRead(0);
        ticks = VSync(-1);
        if (VSync(1) != 0 || VSync(-1) != ticks || VSync(0) != 1 || VSync(-1) != ticks + 1)
        {
            fprintf(stderr, "platform smoke: VSync tick contract failed\n");
            return 2;
        }
    }
    if (frame != frames)
        return 2;
    for (frame = 0; frame < GPU_VRAM_WIDTH * GPU_VRAM_HEIGHT; ++frame)
    {
        colored += VRAM[frame] != 0;
        checksum = (checksum ^ VRAM[frame]) * 16777619u;
    }
    printf("platform smoke: vblank=%d, colored_pixels=%u, headless=%d, framebuffer_hash=%08X\n", VSync(-1), colored, headless, checksum);
    return colored > 0 && (unsigned)VSync(-1) == frames ? 0 : 2;
}
