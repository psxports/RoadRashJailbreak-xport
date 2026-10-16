#include "rrj_cd_transport.h"
#include "race_trace_runtime.h"
#include "video_preview.h"
#include "psx.h"
#include "psx_spu.h"
#include "sdk_build_stubs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint32_t stream_enabled;
static uint32_t servicing;
static uint8_t sector_bytes[2352];
static uint32_t sector_offset;
static uint32_t sector_limit;

void rrj_cd_stream_enable(uint32_t enabled)
{
    if (!rrj_trace_runtime_is_live())
        return;
    stream_enabled = enabled != 0;
    sector_offset = sector_limit = 0;
    cd_stream_consumer_native((int32_t)stream_enabled);
}

int rrj_cd_stream_restore(void)
{
    uint32_t context = r_u32(0x8009C2D0);
    uint8_t mode = 0xA0;
    uint8_t response[8] = {0};
    CdlLOC position;
    if (!context || !r_u32(context + 0x41C))
        return 1;
    rrj_cd_stream_enable(1);
    if (!CdControl(14, &mode, response))
        return 0;
    if (r_u32(context + 0x410))
    {
        uint32_t index = r_u32(context + 0x408);
        if (index >= 48 || index % 3)
            return 0;
        uint32_t command = r_u32(context + 8 + 4 * index);
        uint32_t parameter = r_u32(context + 8 + 4 * (index + 1));
        return CdControl((uint8_t)command, parameter ? xport_guest_ptr(parameter, 1) : NULL, response) != 0;
    }
    if (r_u32(context + 0x414) == 2 && r_u32(context))
    {
        uint32_t descriptor = r_u32(context + 0x430);
        uint32_t sector = (uint32_t)CdPosToInt((CdlLOC *)xport_guest_ptr(descriptor + 12, sizeof(CdlLOC)));
        CdIntToPos((int32_t)(sector + r_u32(descriptor + 8)), &position);
        return CdControl(6, (uint8_t *)&position, response) != 0;
    }
    return 1;
}

int rrj_cd_fifo_read(void *destination, uint32_t words)
{
    uint32_t bytes;
    if (!destination || words > 585)
        return 0;
    if (!sector_limit)
        return CdGetSector(destination, (int32_t)words);
    bytes = words * 4;
    if (bytes > sector_limit - sector_offset)
        return 0;
    memcpy(destination, sector_bytes + sector_offset, bytes);
    sector_offset += bytes;
    if (sector_offset == sector_limit)
        sector_offset = sector_limit = 0;
    return 1;
}

void rrj_cd_stream_service(void)
{
    uint32_t context;
    uint8_t response[8] = {0};
    int32_t event;
    if (!stream_enabled || servicing || !rrj_trace_runtime_is_live() || !psx_callbacks_allowed())
        return;
    servicing = 1;
    context = r_u32(0x8009C2D0);
    if (!context || !r_u32(context + 0x41C))
    {
        servicing = 0;
        return;
    }
    if (r_u32(context + 0x410) && r_u32(0x8005AF60) == 0x80061584)
    {
        event = CdSync(1, response);
        if (event == CdlComplete || event == CdlDiskError)
            (void)sub_80061584((uint32_t)event);
    }
    for (;;)
    {
        context = r_u32(0x8009C2D0);
        if (!context || !r_u32(context + 0x41C) || r_u32(context + 0x414) != 2 ||
            !r_u32(context) || r_u32(context + 0x424) >= 2 || r_u32(0x8005AF64) != 0x800611F4)
            break;
        event = cd_stream_pop_sector_native(sector_bytes);
        if (event > 0)
        {
            /* Supply the actual physical sector in the original raw FIFO order */
            sector_offset = 12;
            sector_limit = sizeof(sector_bytes);
            (void)sub_800611F4(CdlDataReady);
            sector_offset = sector_limit = 0;
        }
        else
        {
            if (event < 0)
                (void)sub_800611F4(CdlDiskError);
            break;
        }
    }
    servicing = 0;
}

void rrj_native_wait_events(void)
{
    if (!rrj_trace_runtime_is_live())
        return;
    psx_native_poll_events();
    rrj_spu_completion_service();
    rrj_cd_stream_service();
    /* Yield until the next real display field while native devices complete */
    (void)VSync(0);
}
