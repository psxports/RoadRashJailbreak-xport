#include "first2_camera_surface.h"

uint32_t sub_8008AAB0(uint32_t actor)
{
    uint32_t flags;

    FUNCTION_MARKER(0x8008AAB0, "RASHCDG.BIN");
    flags = r_u32(actor + 0x224u);
    if ((flags & 0x8000u) != 0)
    {
        w_u32(actor + 0x224u, flags | 0x10006u);
        flags = r_u32(actor + 0x224u);
    }
    if ((flags & 0x40000u) != 0)
        w_u32(actor + 0x224u, flags | 6u);
    flags = r_u32(actor + 0x224u) & 0xFFF17FFFu;
    w_u32(actor + 0x224u, flags);
    return flags;
}
