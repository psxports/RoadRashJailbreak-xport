#include "psx.h"
/* Original F video-stop bookkeeping, outside the WIP CD/MDEC boundary. */
#include "video_stop.h"
#include <stdlib.h>

uint32_t sub_F_8006FE6C(RRJSDKCall stop)
{
    FUNCTION_MARKER(0x8006FE6Cu, "RASHCDF.BIN");
    uint32_t channel, flags;
    if (rrj_u16(rrj_at(0x8009C5DA, 2)) & 1)
    {
        channel = rrj_u16(rrj_at(0x8009C688, 2));
        if (channel & 0x8000)
            channel |= 0xffff0000;
        if (!stop)
            abort();
        stop(rrj_host_context(), 0x8005F7E0, channel, 0);
        stop(rrj_host_context(), 0x8001460C, rrj_read32(0x8009C684), 0);
        flags = rrj_u16(rrj_at(0x8009C5DA, 2));
        rrj_write32(0x8009C684, 0xffffffff);
        rrj_put16(rrj_at(0x8009C5DA, 2), (uint16_t)(flags & 0xfff8));
    }
    return 1;
}

uint32_t sub_F_8006FED4(RRJSDKCall stop)
{
    FUNCTION_MARKER(0x8006FED4u, "RASHCDF.BIN");
    return sub_F_8006FE6C(stop);
}
