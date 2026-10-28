#include "rrj_fmv.h"
#include "psx.h"
#include "psx_gpu.h"
#include <stdlib.h>

static struct
{
    uint8 *rgb;
    uint16 *pixels;
    XportFmvFrame frame;
    sint32 x, y;
    uint32 mode, ready, session, descriptor;
} preview;

int rrj_fmv_path(uint32 descriptor, char *path, uint32 capacity)
{
    const char *name = (const char *)xport_guest_cptr(r_u32(0x8008973Cu + 4 * r_u32(descriptor)), 1);
    int length = snprintf(path, capacity, "DATA/FE/%s", name);
    return length > 0 && (uint32)length < capacity;
}

int rrj_menu_fmv_active(void)
{
    return preview.rgb != NULL;
}

void rrj_menu_fmv_close(void)
{
    if (preview.session)
        xport_fmv_close();
    free(preview.rgb);
    free(preview.pixels);
    preview.rgb = NULL;
    preview.pixels = NULL;
    preview.ready = 0;
    preview.session = 0;
}

int rrj_menu_fmv_open(uint32 descriptor)
{
    char path[128];
    if (preview.session || !preview.rgb || preview.descriptor != descriptor)
        rrj_menu_fmv_close();
    if (!rrj_fmv_path(descriptor, path, sizeof(path)) || !rrj_fmv_install())
        return 0;
    if (!preview.rgb)
    {
        preview.rgb = (uint8 *)malloc(640 * 256 * 3);
        preview.pixels = (uint16 *)malloc(640 * 256 * 2);
    }
    if (!preview.rgb || !preview.pixels || xport_fmv_open(path) <= 0)
    {
        rrj_menu_fmv_close();
        return 0;
    }
    preview.x = r_s16(descriptor + 12);
    preview.y = r_s16(descriptor + 14);
    preview.mode = r_u32(descriptor + 4);
    preview.descriptor = descriptor;
    preview.session = 1;
    return 1;
}

uint32 rrj_menu_fmv_next(uint32 x, uint32 y, uint32 mode)
{
    int result = xport_fmv_next(preview.rgb, 640 * 256 * 3, &preview.frame);
    if (result < 0)
    {
        rrj_menu_fmv_close();
        abort();
    }
    if (result == 2)
    {
        xport_fmv_close();
        preview.session = 0;
        return 0;
    }
    preview.x = (sint32)x;
    preview.y = (sint32)y;
    preview.mode = mode;
    if (result == 1)
    {
        uint32 index;
        for (index = 0; index < preview.frame.width * preview.frame.height; ++index)
        {
            const uint8 *pixel = preview.rgb + index * 3;
            preview.pixels[index] = (uint16)((pixel[0] >> 3) | (pixel[1] >> 3) << 5 | (pixel[2] >> 3) << 10);
        }
        preview.ready = 1;
    }
    return 1;
}

void rrj_menu_fmv_present(void)
{
    uint32 context;
    PSX_RECT rectangle;
    if (!preview.ready)
        return;
    context = r_u32(0x8005B470);
    context += r_u8(context + 5) * 112;
    rectangle.x = (sint16)(preview.x + r_s16(context + 24));
    rectangle.y = (sint16)(preview.y + r_s16(context + 26));
    rectangle.w = (sint16)preview.frame.width;
    rectangle.h = (sint16)preview.frame.height;
    if (preview.mode)
        LoadImagePSX(&rectangle, (uint32 *)preview.pixels);
    else
    {
        rectangle.x = (sint16)(rectangle.x * 3 / 2);
        rectangle.w = (sint16)(rectangle.w * 3 / 2);
        LoadImagePSX(&rectangle, (uint32 *)preview.rgb);
    }
}
