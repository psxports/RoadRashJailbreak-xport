#include "rrj_debug.h"
#include "race_bodyless_batch_003.h"
#include <stdio.h>
#include "native_file_callbacks.h"
#include <string.h>
#include "race_bodyless_batch_001.h"
#include "sdk_build_stubs.h"
#include "native_game_api.h"
#include "psx.h"
#include "psx_spu.h"
/* F preview lifecycle, actual nested stop/start; CD/codec are host boundaries. */
#include "video_preview.h"
#include "rrj_cd_transport.h"
#include "rrj_data.h"
#include "race_trace_runtime.h"
#include "video_stop.h"
#include "menu_latch.h"
#include "resource.h"
#include <stdlib.h>

/* FUNCTION_MARKER: RASHCDF.BIN:0x800602EC sub_800602EC */
uint32_t sub_800602EC(void)
{
    FUNCTION_MARKER(0x800602ECu, "RASHCDF.BIN");
    uint32_t context = r_u32(0x800810BCu);
    w_u32(context + 0x7B20u, 0u);
    return context;
}

static uint32_t stream_disable_cd_data_callback(void)
{
    PsxInterruptCallback previous = CdDataCallback(NULL);
    if (previous)
    {
        RRJ_DEBUG_FPUTS("RRJ: unresolved previous CD data callback\n", stderr);
        abort();
    }
    return 0u;
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80062218 sub_80062218 */
uint32_t sub_80062218(uint32_t context, uint32_t callback, uint32_t handle, uint32_t loop)
{
    FUNCTION_MARKER(0x80062218u, "RASHCDF.BIN");
    uint32_t sectors, buffer_bytes, descriptors, descriptor_base, buffer, value, active;
    /* Initialize the original unwritten metadata tail */
    uint8_t metadata[24] = {0};
    (void)(uint32_t)sub_800148DC(metadata, handle);
    value = (uint32_t)sub_800148BC(handle);
    xport_store_le32(metadata + 4u, value);
    sectors = r_u32(0x8009C2E0u);
    buffer_bytes = r_u32(0x8009C2E4u);
    descriptors = r_u32(0x8009C2E8u);
    (void)memset(xport_guest_ptr(context, buffer_bytes + (sectors << 11) + 0x57cu + 56u * descriptors), 0, buffer_bytes + (sectors << 11) + 0x57cu + 56u * descriptors);
    descriptor_base = context + 0x57cu;
    descriptors = r_u32(0x8009C2E8u);
    w_u32(0x8009C2D0u, context);
    w_u32(0x8009C2D4u, descriptor_base);
    w_u32(context + 0x570u, callback);
    w_u32(context + 0x414u, 0u);
    w_u32(context + 0x418u, 0u);
    w_u32(context + 0x41cu, 0u);
    w_u32(context + 0x43cu, 0u);
    w_u32(context + 0x434u, 0u);
    w_u32(context + 0x430u, 0u);
    w_u32(context + 0x42cu, 0u);
    w_u32(context + 0x440u, 0u);
    w_u32(context + 0x448u, 0u);
    w_u32(context + 0x44cu, 0u);
    buffer = descriptor_base + 56u * descriptors;
    buffer_bytes = r_u32(0x8009C2E4u);
    w_u32(0x8009C2D8u, buffer);
    w_u32(0x8009C2DCu, buffer + buffer_bytes);
    (void)sub_8001E0B4(xport_guest_ptr(context + 0x588u, 24u), metadata, 24u);
    (void)sub_8001E0B4(xport_guest_ptr(r_u32(0x8009C2D4u) + 0x44u, 24u), metadata, 24u);
    active = r_u32(0x8009C2D0u);
    w_u32(active + 0x438u, loop);
    w_u32(active + 0x424u, 0u);
    w_u32(active + 0x428u, 0u);
    value = stream_disable_cd_data_callback();
    active = r_u32(0x8009C2D0u);
    w_u32(active + 0x574u, value);
    value = rrj_cd_read_callback_psx(0u);
    active = r_u32(0x8009C2D0u);
    w_u32(active + 0x578u, value);
    (void)(uint32_t)sub_80061730();
    (void)(uint32_t)sub_800620D4(1u);
    return 1u;
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80061730 sub_80061730 */
uint32_t sub_80061730(void)
{
    FUNCTION_MARKER(0x80061730u, "RASHCDF.BIN");
    uint32_t context = r_u32(0x8009C2D0u);
    uint32_t result = r_u32(context + 0x43cu);
    uint32_t index, base, current, descriptor;
    if (result)
        return result;
    index = r_u32(context + 0x434u);
    base = r_u32(0x8009C2D4u);
    current = r_u32(context + 0x42cu);
    descriptor = base + 56u * index;
    w_u32(context + 0x430u, descriptor);
    if (!current)
        w_u32(context + 0x42cu, descriptor);
    context = r_u32(0x8009C2D0u);
    descriptor = r_u32(context + 0x430u);
    w_u32(descriptor, (r_u32(descriptor + 16u) + 2047u) >> 11);
    w_u32(r_u32(context + 0x430u) + 4u, 0u);
    w_u32(r_u32(context + 0x430u) + 8u, 0u);
    w_u32(r_u32(context + 0x430u) + 40u, 0u);
    index = r_u32(context + 0x434u);
    descriptor = r_u32(context + 0x430u);
    w_u32(context + 0x444u, 0u);
    w_u32(context + 0x440u, 0u);
    w_u32(context + 0x448u, 0u);
    w_u32(context + 0x44cu, 0u);
    w_u32(context + 0x434u, index + 1u);
    w_u32(descriptor + 52u, 0u);
    descriptor = r_u32(context + 0x430u);
    w_u32(descriptor + 44u, r_u32(0x8009C2D8u));
    descriptor = r_u32(context + 0x430u);
    w_u32(descriptor + 48u, r_u32(0x8009C2DCu));
    w_u32(context + 0x43cu, 0u);
    return 0u;
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x800620D4 sub_800620D4 */
uint32_t sub_800620D4(uint32_t enabled)
{
    FUNCTION_MARKER(0x800620D4u, "RASHCDF.BIN");
    uint32_t context, previous;
    if (rrj_data_enabled())
    {
        context = r_u32(0x8009C2D0);
        w_u32(context + 0x408, 0);
        w_u32(context + 0x40C, 0);
        w_u32(context + 0x410, 0);
        w_u32(context + 0x414, 0);
        w_u32(context + 0x420, enabled != 0);
        w_u32(context + 0x41C, enabled != 0);
        if (!enabled) rrj_data_stream_close();
        return enabled ? 1 : context;
    }
    if (enabled)
    {
        uint8_t mode = 0xa0u;
        (void)(uint32_t)CdSync(((int32_t)(uint32_t)(0u)), NULL);
        (void)CdControlB(14u, &mode, NULL);
        (void)sub_80043DD4();
        context = r_u32(0x8009C2D0u);
        w_u32(context + 0x410u, 0u);
        w_u32(context + 0x408u, 0u);
        w_u32(context + 0x40cu, 0u);
        previous = CdSyncCallbackPSX(0x80061584u);
        w_u32(0x800810E0u, previous);
        previous = CdReadyCallbackPSX(0x800611F4u);
        w_u32(0x800810E4u, previous);
        context = r_u32(0x8009C2D0u);
        w_u32(context + 0x420u, 1u);
        w_u32(context + 0x41cu, 1u);
    }
    else
    {
        (void)sub_80043DD4();
        context = r_u32(0x8009C2D0u);
        previous = r_u32(context + 0x420u);
        w_u32(context + 0x41cu, 0u);
        w_u32(context + 0x410u, 0u);
        w_u32(context + 0x408u, 0u);
        w_u32(context + 0x40cu, 0u);
        if (previous)
        {
            (void)CdSyncCallbackPSX(r_u32(0x800810E0u));
            (void)CdReadyCallbackPSX(r_u32(0x800810E4u));
            context = r_u32(0x8009C2D0u);
            w_u32(0x800810E0u, 0u);
            w_u32(0x800810E4u, 0u);
            w_u32(context + 0x420u, 0u);
        }
    }
    sub_80043DF4();
    rrj_cd_stream_enable(enabled);
    return enabled ? 1u : context;
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80060EC4 sub_80060EC4 */
uint32_t sub_80060EC4(void)
{
    FUNCTION_MARKER(0x80060EC4u, "RASHCDF.BIN");
    uint8_t parameter[32] = {0}, result[32] = {0};
    if (!CdControl(1u, parameter, result))
    {
        (void)CdControl(10u, NULL, NULL);
        return 1u;
    }
    return result[0] != 0u && result[0] != 2u;
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x8005F36C sub_8005F36C */
uint32_t sub_8005F36C(uint32_t handle, uint32_t mode, uint32_t channel, uint32_t x, uint32_t y, uint32_t chunk_size, uint32_t preload_width, uint32_t preload_height, uint32_t loop)
{
    uint32_t result = 1u;
    FUNCTION_MARKER(0x8005F36Cu, "RASHCDF.BIN");
    if (mode)
        sub_80062420();
    else
        sub_800623F8();
    if (!sub_80062218(r_u32(0x800810ACu), 0x800602ECu, handle, loop))
        return result;
    if (channel)
    {
        result = sub_80060D14(r_u32(0x800810B0u), 0x400000u, r_u32(0x800810B4u), 0x800000u, r_u32(0x800810B8u), chunk_size, 1u);
    }
    if (result)
    {
        uint32_t length = preload_width * preload_height;
        sub_80061F04(&length);
        sub_8005F484(x, y, channel, mode, 0u);
    }
    return result;
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80061060 sub_80061060 */
uint32_t sub_80061060(uint32_t command, uint32_t parameter, uint32_t result)
{
    FUNCTION_MARKER(0x80061060u, "RASHCDF.BIN");
    uint32_t context = r_u32(0x8009C2D0u);
    uint32_t index = r_u32(context + 0x40cu), pending;
    w_u32(context + 8u + index * 4u, command & 0xffu);
    w_u32(context + 0x40cu, index + 1u);
    index = r_u32(context + 0x40cu);
    w_u32(context + 8u + index * 4u, parameter);
    w_u32(context + 0x40cu, index + 1u);
    index = r_u32(context + 0x40cu);
    w_u32(context + 8u + index * 4u, result);
    w_u32(context + 0x40cu, index + 1u);
    w_u32(context + 0x40cu, r_u32(context + 0x40cu) % 48u);
    pending = r_u32(context + 0x410u) + 1u;
    w_u32(context + 0x410u, pending);
    if (r_u32(context + 0x410u) == 1u)
    {
        while (!(uint32_t)CdControlF((uint8_t)(command & 0xffu), (parameter ? xport_guest_ptr(parameter, 1) : NULL)))
        {
        }
    }
    return 1u;
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80061180 sub_80061180 */
uint32_t sub_80061180(uint32_t buffer, uint32_t sectors, uint32_t position)
{
    FUNCTION_MARKER(0x80061180u, "RASHCDF.BIN");
    uint32_t context = r_u32(0x8009C2D0u);
    w_u32(context + 0x450u, buffer);
    w_u32(context + 0x454u, sectors);
    return sub_80061060(6u, position, 0u);
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80061584 sub_80061584 */
uint32_t sub_80061584(uint32_t event)
{
    FUNCTION_MARKER(0x80061584u, "RASHCDF.BIN");
    uint32_t context, result, index, command, parameter;
    if ((event & 0xffu) != 2u)
    {
        uint32_t errors = r_u32(0x800810C4u);
        context = r_u32(0x8009C2D0u);
        w_u32(0x800810C4u, errors + 1u);
        result = r_u32(context + 0x424u) + 1u;
        w_u32(context + 0x424u, result);
        return result;
    }
    context = r_u32(0x8009C2D0u);
    if (r_u32(context + 0x424u) >= 2u)
        return 0u;
    w_u32(context + 0x424u, 0u);
    result = r_u32(context + 0x41cu);
    if (!result)
        return result;
    result = r_u32(context + 0x410u);
    if (!result)
        return result;
    if (r_u32(context + 0x418u))
    {
        w_u32(context + 0x418u, r_u32(context + 0x418u) - 1u);
        if (!r_u32(context + 0x418u))
        {
            uint32_t buffer = r_u32(context + 0x450u);
            uint32_t sectors = r_u32(context + 0x454u);
            w_u32(context + 4u, buffer);
            w_u32(context, sectors);
            w_u32(context + 0x414u, 2u);
        }
    }
    context = r_u32(0x8009C2D0u);
    w_u32(context + 0x410u, r_u32(context + 0x410u) - 1u);
    w_u32(context + 0x408u, r_u32(context + 0x408u) + 3u);
    w_u32(context + 0x408u, r_u32(context + 0x408u) % 48u);
    result = r_u32(context + 0x410u);
    if (!result)
        return result;
    index = r_u32(context + 0x408u);
    command = r_u32(context + 8u + index * 4u);
    index = r_u32(context + 0x408u);
    parameter = r_u32(context + 8u + (index + 1u) * 4u);
    index = r_u32(context + 0x408u);
    (void)r_u32(context + 8u + (index + 2u) * 4u);
    do
    {
        result = (uint32_t)CdControlF((uint8_t)(command & 0xffu), (parameter ? xport_guest_ptr(parameter, 1) : NULL));
    } while (!result);
    return result;
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80060F44 sub_80060F44 */
uint32_t sub_80060F44(uint32_t command, uint32_t parameter, uint32_t result)
{
    FUNCTION_MARKER(0x80060F44u, "RASHCDF.BIN");
    uint32_t context, index, pending;
    (void)sub_80043DD4();
    context = r_u32(0x8009C2D0u);
    index = r_u32(context + 0x40cu);
    w_u32(context + 8u + index * 4u, command & 0xffu);
    w_u32(context + 0x40cu, index + 1u);
    index = r_u32(context + 0x40cu);
    w_u32(context + 8u + index * 4u, parameter);
    w_u32(context + 0x40cu, index + 1u);
    index = r_u32(context + 0x40cu);
    w_u32(context + 8u + index * 4u, result);
    w_u32(context + 0x40cu, index + 1u);
    w_u32(context + 0x40cu, r_u32(context + 0x40cu) % 48u);
    pending = r_u32(context + 0x410u) + 1u;
    w_u32(context + 0x410u, pending);
    pending = r_u32(context + 0x410u);
    (void)sub_80043DF4();
    if (pending == 1u)
    {
        while (!CdControl((uint8_t)command, parameter ? (uint8_t *)xport_guest_ptr(parameter, 1u) : NULL, result ? (uint8_t *)xport_guest_ptr(result, 1u) : NULL))
        {
        }
    }
    return 1u;
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80061148 sub_80061148 */
uint32_t sub_80061148(uint32_t buffer, uint32_t sectors, uint32_t position)
{
    FUNCTION_MARKER(0x80061148u, "RASHCDF.BIN");
    uint32_t context = r_u32(0x8009C2D0u);
    w_u32(context + 0x450u, buffer);
    w_u32(context + 0x454u, sectors);
    return sub_80060F44(6u, position, 0u);
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x800611B8 sub_800611B8 */
uint32_t sub_800611B8(uint32_t source_position, uint32_t destination_position, uint32_t sectors)
{
    FUNCTION_MARKER(0x800611B8u, "RASHCDF.BIN");
    uint32_t sector = (uint32_t)sub_80046768(xport_guest_cptr(source_position, 3));
    CdIntToPos(((int32_t)(uint32_t)(sector + sectors)), (CdlLOC *)xport_guest_ptr(destination_position, sizeof(CdlLOC)));
    return destination_position;
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80061818 sub_80061818 */
uint32_t sub_80061818(void)
{
    FUNCTION_MARKER(0x80061818u, "RASHCDF.BIN");
    uint32_t context = r_u32(0x8009C2D0u);
    uint32_t result = r_u32(context + 0x43cu);
    if (result)
        return result;
    if (r_u32(context + 0x438u))
    {
        uint32_t current = r_u32(context + 0x430u);
        uint32_t next = r_u32(0x8009C2D4u), descriptor;
        if (current == next)
            next += 56u;
        w_u32(context + 0x430u, next);
        context = r_u32(0x8009C2D0u);
        (void)sub_8001E0B4(xport_guest_ptr(r_u32(context + 0x430u), 56u), xport_guest_ptr(r_u32(context + 0x42cu), 1), 56u);
        context = r_u32(0x8009C2D0u);
        descriptor = r_u32(context + 0x430u);
        w_u32(descriptor, (r_u32(descriptor + 16u) + 2047u) >> 11);
        w_u32(r_u32(context + 0x430u) + 4u, 0u);
        w_u32(r_u32(context + 0x430u) + 8u, 0u);
        w_u32(r_u32(context + 0x430u) + 40u, 0u);
        result = r_u32(context + 0x434u);
        descriptor = r_u32(context + 0x430u);
        w_u32(context + 0x434u, result + 1u);
        w_u32(descriptor + 52u, 0u);
        descriptor = r_u32(context + 0x430u);
        w_u32(descriptor + 44u, r_u32(0x8009C2D8u));
        descriptor = r_u32(context + 0x430u);
        w_u32(descriptor + 48u, r_u32(0x8009C2DCu));
        w_u32(context + 0x43cu, 0u);
    }
    else
        w_u32(context + 0x43cu, 2u);
    context = r_u32(0x8009C2D0u);
    return r_u32(context + 0x43cu);
}


uint32_t sub_800611F4(uint32_t event)
{
    uint32_t context, descriptor, actual, expected, sectors, count, value, capacity;
    uint32_t remaining, available, contiguous, buffer, produced;
    uint8_t header[12];
    FUNCTION_MARKER(0x800611F4, "RASHCDF.BIN");
    if ((event & 255) != 1)
    {
        count = r_u32(0x800810C0);
        context = r_u32(0x8009C2D0);
        w_u32(0x800810C0, count + 1);
        count = r_u32(context + 0x424) + 1;
        w_u32(context + 0x424, count);
        return count;
    }
    context = r_u32(0x8009C2D0);
    if (r_u32(context + 0x424) >= 2)
        return 0;
    w_u32(context + 0x424, 0);
    if (r_u32(context + 0x414) != 2)
        return 2;
    if (!r_u32(context))
        return 0x80080000;
    if (r_u32(0x800810D4))
    {
        uint32_t port = r_u32(0x800810D8);
        if (r_u32(port) & 0x01000000)
            for (count = 1; count != 65537; ++count)
                if (!(r_u32(port) & 0x01000000))
                    break;
    }
    context = r_u32(0x8009C2D0);
    actual = r_u32(context + 4) & 3;
    if (actual)
    {
        w_u32(context + 0x414, 0);
        return actual;
    }
    (void)sub_80045BEC(header, 3);
    (void)CdDataSync(0);
    actual = sub_80046768(header);
    context = r_u32(0x8009C2D0);
    descriptor = r_u32(context + 0x430);
    (void)sub_800611B8(descriptor + 12, descriptor + 36, r_u32(descriptor + 8));
    context = r_u32(0x8009C2D0);
    expected = sub_80046768(xport_guest_cptr(r_u32(context + 0x430) + 36, 3));
    if (actual != expected)
    {
        count = r_u32(0x800810C8);
        context = r_u32(0x8009C2D0);
        w_u32(0x800810C8, count + 1);
    }
    else
        context = r_u32(0x8009C2D0);
    (void)sub_80045BEC(xport_guest_ptr(r_u32(context + 4), 2048), 512);
    (void)CdDataSync(0);
    context = r_u32(0x8009C2D0);
    (void)sub_80045BEC(xport_guest_ptr(context + 0x458, 280), 70);
    (void)CdDataSync(0);
    if (actual != expected)
    {
        context = r_u32(0x8009C2D0);
        descriptor = r_u32(context + 0x430);
        (void)sub_800611B8(descriptor + 12, descriptor + 36, r_u32(descriptor + 8));
        context = r_u32(0x8009C2D0);
        w_u32(context + 0x428, r_u32(context + 0x428) + 1);
        w_u32(context + 0x418, 2);
        w_u32(context + 0x414, 1);
        w_u32(0x800810CC, r_u32(0x800810CC) + 1);
        descriptor = r_u32(context + 0x430);
        (void)sub_80061060(21, descriptor + 36, 0);
        context = r_u32(0x8009C2D0);
        return sub_80061180(r_u32(context + 4), r_u32(context), r_u32(context + 0x430) + 36);
    }
    context = r_u32(0x8009C2D0);
    w_u32(context + 4, r_u32(context + 4) + 2048);
    count = r_u32(context);
    descriptor = r_u32(context + 0x430);
    w_u32(context, count - 1);
    w_u32(descriptor + 8, r_u32(descriptor + 8) + 1);
    w_u32(context + 0x448, r_u32(context + 0x448) + 1);
    count = r_u32(context);
    if (count)
        return count;
    descriptor = r_u32(context + 0x430);
    count = r_u32(context);
    remaining = r_u32(descriptor) - r_u32(descriptor + 8) - count;
    produced = r_u32(context + 0x448) + count;
    capacity = r_u32(0x8009C2E0);
    value = capacity ? produced % capacity : produced;
    available = r_u32(context + 0x440) + capacity - produced;
    contiguous = capacity - value;
    sectors = (int32_t)available < (int32_t)contiguous ? available : contiguous;
    if ((int32_t)remaining < (int32_t)sectors)
        sectors = remaining;
    if (sectors)
    {
        context = r_u32(0x8009C2D0);
        capacity = r_u32(0x8009C2E0);
        descriptor = r_u32(context + 0x430);
        buffer = r_u32(descriptor + 0x30);
        if (r_u32(context + 4) == buffer + (capacity << 11))
            w_u32(context + 4, buffer);
        context = r_u32(0x8009C2D0);
        w_u32(context, sectors);
    }
    else
    {
        context = r_u32(0x8009C2D0);
        w_u32(context, 0);
        w_u32(context + 0x414, 0);
    }
    return context;
}

static uint32_t stream_divide(uint32_t value, uint32_t divisor)
{
    if (!divisor)
        return ((int32_t)(uint32_t)(value)) < 0 ? 1u : 0xffffffffu;
    if (value == 0x80000000u && divisor == 0xffffffffu)
        return value;
    return (uint32_t)(((int32_t)(uint32_t)(value)) / ((int32_t)(uint32_t)(divisor)));
}

static void stream_bar(uint32_t x, uint32_t y, uint32_t width, uint32_t blue)
{
    uint32_t words[4];
    uint8_t *packet = (uint8_t *)words;
    xport_store_le32(packet, 0x03000000u);
    xport_store_le32(packet + 4, 0x02FFFFFFu);
    xport_store_u8(packet + 6, (uint8_t)blue);
    xport_store_le16(packet + 8, (uint16_t)x);
    xport_store_le16(packet + 10, (uint16_t)y);
    xport_store_le16(packet + 12, (uint16_t)width);
    xport_store_le16(packet + 14, 8);
    (void)DrawSync(0);
    DrawPrim(packet);
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x800609E4 sub_800609E4 */
uint32_t sub_800609E4(void)
{
    FUNCTION_MARKER(0x800609E4u, "RASHCDF.BIN");
    uint32_t context = r_u32(0x800810BCu);
    uint32_t written = r_u32(context + 4u);
    uint32_t read = r_u32(context);
    uint32_t available = 15360u - (read - (written - 15360u));
    uint32_t adjusted = ((int32_t)(uint32_t)(available)) < 0 ? available + 63u : available;
    uint32_t width = (uint32_t)(((int32_t)(uint32_t)(adjusted)) >> 6);
    stream_bar(24u, 212u, 240u, 255u);
    stream_bar(504u, 212u, 240u, 255u);
    stream_bar(24u, 212u, width, 0u);
    stream_bar(504u, 212u, width, 0u);
    return (uint32_t)DrawSync(0u);
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80061C44 sub_80061C44 */
uint32_t sub_80061C44(uint32_t length)
{
    FUNCTION_MARKER(0x80061C44u, "RASHCDF.BIN");
    uint32_t context = r_u32(0x8009C2D0u);
    uint32_t descriptor = r_u32(context + 0x42cu);
    uint32_t offset = r_u32(context + 0x444u);
    uint32_t remaining = 2048u - offset;
    w_u32(descriptor + 4u, r_u32(descriptor + 4u) + length);
    if (((int32_t)(uint32_t)(length)) < ((int32_t)(uint32_t)(remaining)))
        w_u32(context + 0x444u, r_u32(context + 0x444u) + length);
    else
    {
        uint32_t overflow = length - remaining;
        w_u32(context + 0x444u, overflow & 0x7ffu);
        w_u32(context + 0x440u, r_u32(context + 0x440u) + (overflow >> 11) + 1u);
    }
    sub_80061938();
    context = r_u32(0x8009C2D0u);
    descriptor = r_u32(context + 0x42cu);
    if (r_u32(descriptor + 4u) == r_u32(descriptor + 16u) && r_u32(context + 0x438u))
    {
        uint32_t next = r_u32(context + 0x430u);
        w_u32(context + 0x44cu, 0u);
        offset = r_u32(context + 0x444u);
        w_u32(context + 0x42cu, next);
        if (offset)
            w_u32(context + 0x440u, r_u32(context + 0x440u) + 1u);
        context = r_u32(0x8009C2D0u);
        descriptor = r_u32(context + 0x42cu);
        w_u32(context + 0x444u, r_u32(descriptor + 0x28u));
    }
    if (!r_u32(0x800810DCu))
        return 0u;
    {
        uint32_t bytes = r_u32(0x8009C2E0u) << 11;
        int64_t product = (int64_t)((int32_t)(uint32_t)(bytes)) * (int64_t)((int32_t)(uint32_t)(0x9c09c09du));
        uint32_t high = (uint32_t)((uint64_t)product >> 32);
        uint32_t scale = (uint32_t)(((int32_t)(uint32_t)(high + bytes)) >> 8) - (uint32_t)(((int32_t)(uint32_t)(bytes)) >> 31);
        uint32_t width = stream_divide(bytes, scale), available;
        context = r_u32(0x8009C2D0u);
        available = (r_u32(context + 0x448u) << 11) - ((r_u32(context + 0x440u) << 11) + r_u32(context + 0x444u));
        stream_bar(24u, 200u, width, 255u);
        stream_bar(504u, 200u, width, 255u);
        width = stream_divide(available, scale);
        stream_bar(24u, 200u, width, 0u);
        stream_bar(504u, 200u, width, 0u);
    }
    (void)sub_800609E4();
    return (uint32_t)DrawSync(0u);
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80061938 sub_80061938 */
void sub_80061938(void)
{
    if (rrj_data_enabled())
        return;
    uint32_t scalar_value_12;
    uint32_t scalar_value_13;

    uint32_t scalar_left_10;

    uint32_t scalar_left_8;
    uint32_t scalar_right_8;

    FUNCTION_MARKER(0x80061938u, "RASHCDF.BIN");
    uint32_t context = r_u32(0x8009C2D0u), descriptor, sectors;
    if (r_u32(context + 0x424u) >= 2u)
    {
        uint32_t callback = r_u32(context + 0x570u);
        uint8_t mode = 0xa0u;
        if (callback)
            (void)rrj_file_function(callback)();
        (void)sub_80043DD4();
        context = r_u32(0x8009C2D0u);
        w_u32(context + 0x410u, 0u);
        w_u32(context + 0x408u, 0u);
        w_u32(context + 0x40cu, 0u);
        w_u32(context + 0x41cu, 0u);
        (void)CdSyncCallbackPSX(0u);
        (void)CdReadyCallbackPSX(0u);
        (void)sub_80043DF4();
        (void)CdControl(8u, NULL, NULL);
        while (sub_80060EC4())
        {
        }
        (void)CdControlB(14u, &mode, NULL);
        (void)sub_80043DD4();
        (void)CdSyncCallbackPSX(0x80061584u);
        (void)CdReadyCallbackPSX(0x800611F4u);
        context = r_u32(0x8009C2D0u);
        w_u32(context + 0x41cu, 1u);
        w_u32(context + 0x414u, 1u);
        w_u32(context + 0x424u, 0u);
        (void)sub_80043DF4();
        context = r_u32(0x8009C2D0u);
        if (r_u32(context + 0x414u) && r_u32(context))
        {
            descriptor = r_u32(context + 0x430u);
            (void)sub_800611B8(descriptor + 12u, descriptor + 36u, r_u32(descriptor + 8u));
            context = r_u32(0x8009C2D0u);
            w_u32(context + 0x418u, 2u);
            w_u32(0x800810CCu, r_u32(0x800810CCu) + 1u);
            descriptor = r_u32(context + 0x430u);
            if (!sub_80060F44(21u, descriptor + 36u, 0u))
            {
                context = r_u32(0x8009C2D0u);
                w_u32(context + 0x43cu, 1u);
                return;
            }
            context = r_u32(0x8009C2D0u);
            sectors = r_u32(context);
            (void)sub_80061148(r_u32(context + 4u), sectors, r_u32(context + 0x430u) + 36u);
        }
        else
        {
            context = r_u32(0x8009C2D0u);
            w_u32(context + 0x414u, 0u);
        }
    }
    context = r_u32(0x8009C2D0u);
    if (r_u32(context + 0x414u))
        return;
    descriptor = r_u32(context + 0x430u);
    sectors = r_u32(descriptor) - r_u32(descriptor + 8u);
    if (!sectors)
        (void)(uint32_t)sub_80061818();
    context = r_u32(0x8009C2D0u);
    {
        uint32_t capacity = r_u32(0x8009C2E0u);
        uint32_t consumed = r_u32(context + 0x440u);
        uint32_t produced = r_u32(context + 0x448u);
        uint32_t remainder = ((scalar_value_12 = (r_u32(context + 0x448u))), (((uint32_t)(((uint32_t)(capacity)) ? scalar_value_12 % ((uint32_t)(capacity)) : scalar_value_12))));
        sectors = ((scalar_left_10 = (((scalar_left_8 = (consumed + capacity - produced)), (scalar_right_8 = (capacity - remainder)), (((uint32_t)(((int32_t)(uint32_t)(scalar_left_8)) < ((int32_t)(uint32_t)(scalar_right_8)) ? scalar_left_8 : scalar_right_8)))))), (((uint32_t)(((int32_t)(uint32_t)(scalar_left_10)) < ((int32_t)(uint32_t)(((uint32_t)(sectors)))) ? scalar_left_10 : ((uint32_t)(sectors))))));
    }
    if (!sectors)
        return;
    context = r_u32(0x8009C2D0u);
    descriptor = r_u32(context + 0x430u);
    (void)sub_800611B8(descriptor + 12u, descriptor + 36u, r_u32(descriptor + 8u));
    context = r_u32(0x8009C2D0u);
    w_u32(context + 0x44cu, r_u32(context + 0x448u) + sectors);
    w_u32(context + 0x418u, 2u);
    w_u32(0x800810CCu, r_u32(0x800810CCu) + 1u);
    descriptor = r_u32(context + 0x430u);
    if (!sub_80060F44(21u, descriptor + 36u, 0u))
    {
        context = r_u32(0x8009C2D0u);
        w_u32(context + 0x43cu, 1u);
        return;
    }
    context = r_u32(0x8009C2D0u);
    descriptor = r_u32(context + 0x430u);
    {
        uint32_t produced = r_u32(context + 0x448u);
        uint32_t displacement = r_u32(descriptor + 0x34u);
        uint32_t capacity = r_u32(0x8009C2E0u);
        uint32_t position = ((scalar_value_13 = (produced + displacement)), (((uint32_t)(((uint32_t)(capacity)) ? scalar_value_13 % ((uint32_t)(capacity)) : scalar_value_13))));
        uint32_t buffer = r_u32(descriptor + 0x30u);
        w_u32(context + 0x414u, 1u);
        (void)sub_80061148(buffer + (position << 11), sectors, descriptor + 36u);
    }
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x800602D4 sub_800602D4 */
uint32_t sub_800602D4(void)
{
    FUNCTION_MARKER(0x800602D4u, "RASHCDF.BIN");
    w_u32(r_u32(0x800810BCu) + 0x7b20u, 1u);
    return 1u;
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80061F04 sub_80061F04 */
uint32_t sub_80061F04(uint32_t *length)
{
    FUNCTION_MARKER(0x80061F04u, "RASHCDF.BIN");
    if (rrj_data_enabled())
        return rrj_data_stream_read(length);
    uint32_t context = r_u32(0x8009C2D0u);
    uint32_t descriptor = r_u32(context + 0x42cu);
    uint32_t remaining = r_u32(descriptor + 16u) - r_u32(descriptor + 4u);
    uint32_t polls = 0u, retries = 0u;
    if (((int32_t)(uint32_t)(remaining)) < ((int32_t)(uint32_t)(*length)))
        *length = remaining;
    for (;;)
    {
        uint32_t available;
        if (rrj_trace_runtime_is_live())
        {
            psx_native_poll_events();
            rrj_cd_stream_service();
        }
        context = r_u32(0x8009C2D0u);
        available = (r_u32(context + 0x448u) << 11) - ((r_u32(context + 0x440u) << 11) + r_u32(context + 0x444u));
        if (((int32_t)(uint32_t)(available)) < 0)
            available = 0u;
        if (((int32_t)(uint32_t)(available)) >= ((int32_t)(uint32_t)(*length)))
            break;
        if (r_u32(context + 0x43cu))
            return 0u;
        ++polls;
        if (polls > 0x3d0900u)
            w_u32(context + 0x424u, r_u32(context + 0x424u) + 2u);
        context = r_u32(0x8009C2D0u);
        if (r_u32(context + 0x414u) && r_u32(context + 0x424u) < 2u)
            continue;
        if (++retries >= 3u)
            return 0u;
        polls = 0u;
        (void)sub_80061938();
    }
    {
        uint32_t sector, offset, capacity, position, end, pointer, contiguous;
        context = r_u32(0x8009C2D0u);
        sector = r_u32(context + 0x440u);
        offset = r_u32(context + 0x444u);
        descriptor = r_u32(context + 0x42cu);
        position = r_u32(context + 0x440u) + r_u32(descriptor + 0x34u);
        capacity = r_u32(0x8009C2E0u);
        /* MIPS DIVU leaves the dividend in HI when the divisor is zero */
        position = capacity ? position % capacity : position;
        end = capacity ? sector % capacity : sector;
        pointer = r_u32(descriptor + 0x30u) + (position << 11) + r_u32(context + 0x444u);
        contiguous = ((capacity - 1u - end) << 11) + 2048u - offset;
        if (contiguous < *length)
        {
            (void)sub_8001E0B4(xport_guest_ptr(pointer - (capacity << 11), contiguous), xport_guest_ptr(pointer, 1), contiguous);
            pointer -= r_u32(0x8009C2E0u) << 11;
        }
        return pointer;
    }
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x8005F484 sub_8005F484 */
uint32_t sub_8005F484(uint32_t x, uint32_t y, uint32_t channel, uint32_t mode, uint32_t frame)
{
    uint32_t scalar_word_1;
    uint32_t scalar_word_2;

    FUNCTION_MARKER(0x8005F484u, "RASHCDF.BIN");
    for (;;)
    {
        uint32_t length = 8u, header = sub_80061F04(&length), tag, data;
        if (!header || !length)
            return 0u;
        tag = ((scalar_word_1 = r_u32(((uint32_t)(header)))), (((uint32_t)((scalar_word_1 >> 24) | ((scalar_word_1 >> 8) & 0xff00u) | ((scalar_word_1 & 0xff00u) << 8) | (scalar_word_1 << 24)))));
        length = ((scalar_word_2 = r_u32(((uint32_t)(header + 4u)))), (((uint32_t)((scalar_word_2 >> 24) | ((scalar_word_2 >> 8) & 0xff00u) | ((scalar_word_2 & 0xff00u) << 8) | (scalar_word_2 << 24)))));
        if (length >= 0x4400u)
        {
            (void)(uint32_t)sub_80061C44(4u);
            continue;
        }
        data = sub_80061F04(&length);
        if (!data || !length)
            return 0u;
        if (tag == 0x4d444332u || tag == 0x4d444543u)
        {
            uint32_t count = r_u32(0x8009C2F4u);
            if (((int32_t)(uint32_t)(count)) < 6)
            {
                uint32_t item = 0x8009C3D0u + 36u * count;
                w_u32(item, data);
                w_u32(item + 8u, frame ? 2u : 0u);
                w_u32(item + 12u, mode);
                w_u32(item + 28u, x);
                w_u32(item + 32u, y);
                w_u32(0x8009C2F4u, count + 1u);
                w_u32(item + 16u, length);
            }
            if (frame == 1u && channel)
                (void)sub_8006013C();
            (void)sub_800602D4();
            return length;
        }
        if (tag == 0x564c4330u)
            sub_8002026C(data + 8u);
        else if ((tag >= 0x41643130u && tag <= 0x41643131u) || (tag >= 0x61643130u && tag <= 0x61643131u) || (tag >= 0x61643230u && tag <= 0x61643231u))
        {
            if (channel)
                (void)sub_80060304(data);
        }
        else if (tag >= 0x61753030u && tag <= 0x61753031u)
        {
            if (channel)
                (void)sub_80060780(data);
        }
        else
            length = 4u;
        (void)(uint32_t)sub_80061C44(length);
    }
}

uint32_t sub_8006FEF4(uint32_t desc)
{
    uint32_t scalar_v_14;
    uint32_t scalar_v_15;

    char path[128];
    uint32_t file, flags, channel, x, y, mode, loop;
    FUNCTION_MARKER(0x8006FEF4u, "RASHCDF.BIN");
    if (sub_80022A78(0u))
        return 0u;
    if ((uint32_t)r_u16(0x8009C5DAu) & 1u)
        return 0u;
    sprintf(path, (const char *)xport_guest_cptr(0x8005BF84u, 1), (const char *)xport_guest_cptr(r_u32(0x8008973Cu + 4u * r_u32(desc)), 1));
    file = sub_8001458C(path, 0u);
    w_u32(0x8009C684u, file);
    if (((int32_t)(uint32_t)(file)) < 0)
        return 1u;
    w_u32(desc + 16u, 0u);
    flags = (uint32_t)r_u16(0x8009C5DAu);
    file = r_u32(0x8009C684u);
    w_u16(0x8009C5DAu, (uint16_t)(flags | 1u));
    channel = (uint32_t)r_u16((desc + 8u));
    w_u16(0x8009C688u, (uint16_t)channel);
    x = ((scalar_v_14 = ((uint32_t)r_u16((desc + 12u)))), (((uint32_t)(scalar_v_14 & 0x8000 ? scalar_v_14 | 0xffff0000 : scalar_v_14))));
    y = ((scalar_v_15 = ((uint32_t)r_u16((desc + 14u)))), (((uint32_t)(scalar_v_15 & 0x8000 ? scalar_v_15 | 0xffff0000 : scalar_v_15))));
    loop = (uint32_t)r_u16((desc + 10u));
    mode = r_u32(desc + 4u);
    if (!sub_8005F36C(file, mode, ((uint32_t)(((uint32_t)(channel)) & 0x8000 ? ((uint32_t)(channel)) | 0xffff0000 : ((uint32_t)(channel)))), x, y, 2048u, 64u, 1024u, loop))
    {
        sub_8006FE6C();
        return 0u;
    }
    w_u32(desc + 16u, r_u32(desc + 16u) + 1u);
    return 1u;
}

uint32_t sub_80070018(uint32_t desc)
{
    uint32_t scalar_v_17;
    uint32_t scalar_v_18;
    uint32_t scalar_v_19;

    uint32_t flags, frame, x, y, channel, mode, result = 1u;
    FUNCTION_MARKER(0x80070018u, "RASHCDF.BIN");
    flags = (uint32_t)r_u16(0x8009C5DAu);
    if (flags & 4u)
        return sub_8006FE6C();
    if (!(flags & 1u))
    {
        if (flags & 2u)
            return sub_8006FEF4(desc);
        w_u16(0x8009C5DAu, (uint16_t)(flags | 2u));
        return 0u;
    }
    frame = r_u32(desc + 16u);
    x = ((scalar_v_17 = ((uint32_t)r_u16((desc + 12u)))), (((uint32_t)(scalar_v_17 & 0x8000 ? scalar_v_17 | 0xffff0000 : scalar_v_17))));
    y = ((scalar_v_18 = ((uint32_t)r_u16((desc + 14u)))), (((uint32_t)(scalar_v_18 & 0x8000 ? scalar_v_18 | 0xffff0000 : scalar_v_18))));
    channel = ((scalar_v_19 = ((uint32_t)r_u16(0x8009C688u))), (((uint32_t)(scalar_v_19 & 0x8000 ? scalar_v_19 | 0xffff0000 : scalar_v_19))));
    mode = r_u32(desc + 4u);
    if (!sub_8005F484(x, y, channel, mode, frame))
    {
        w_u16(0x8009C5DAu, (uint16_t)(((uint32_t)r_u16(0x8009C5DAu) & 0xfffeu) | 2u));
        result = 0u;
    }
    w_u32(desc + 16u, r_u32(desc + 16u) + 1u);
    return result;
}

uint32_t sub_800602BC(uint32_t value)
{
    uint32_t context;
    FUNCTION_MARKER(0x800602BCu, "RASHCDF.BIN");
    context = r_u32(0x800810BCu);
    w_u32(context + 31604u, value);
    return context;
}

void sub_80060020(uint32_t voice, uint32_t address, uint32_t pitch)
{
    SpuCommonAttr common = {0};
    SpuVoiceAttr attributes = {0};
    uint32_t context;
    FUNCTION_MARKER(0x80060020u, "RASHCDF.BIN");
    common.mask = 3u;
    common.mvol.left = 0x3FFF;
    common.mvol.right = 0x3FFF;
    SpuSetCommonAttr(&common);
    context = r_u32(0x800810BCu);
    attributes.voice = voice;
    attributes.mask = 0xFF93u;
    attributes.volume.left = voice == r_u32(context + 31588u) ? 9216 : 0;
    attributes.volume.right = voice == r_u32(context + 31592u) ? 9216 : 0;
    attributes.pitch = (uint16_t)pitch;
    attributes.addr = address;
    attributes.a_mode = 1;
    attributes.s_mode = 1;
    attributes.r_mode = 3;
    attributes.ar = 0;
    attributes.dr = 0;
    attributes.sr = 0;
    attributes.rr = 0;
    attributes.sl = 15;
    SpuSetVoiceAttr(&attributes);
}

uint32_t sub_80060D14(uint32_t context, uint32_t first_voice, uint32_t first_address, uint32_t second_voice, uint32_t second_address, uint32_t pitch, uint32_t enabled)
{
    uint32_t active, offset, previous;
    FUNCTION_MARKER(0x80060D14u, "RASHCDF.BIN");
    if (!context)
        return 0u;
    w_u32(0x800810BCu, context);
    w_u32(context + 31520u, 0u);
    memset(xport_guest_ptr(context, 31616u), 0, 31616u);
    sub_800602BC(0u);
    active = r_u32(0x800810BCu);
    w_u32(active + 31548u, 1u);
    w_u32(active + 31576u, 0u);
    if (enabled == 1u && first_address && second_address && first_voice && second_voice)
    {
        for (offset = 0u; offset < 768u; offset += 16u)
            w_u8(r_u32(0x800810BCu) + 30736u + offset + 1u, 2u);
        active = r_u32(0x800810BCu);
        w_u32(active + 31580u, first_address);
        w_u32(active + 31584u, second_address);
        w_u32(active + 31588u, first_voice);
        w_u32(active + 31592u, second_voice);
        w_u32(active + 31612u, pitch);
        sub_80060020(r_u32(active + 31588u), r_u32(active + 31580u), pitch);
        active = r_u32(0x800810BCu);
        sub_80060020(r_u32(active + 31592u), r_u32(active + 31584u), pitch);
    }
    sub_80043DD4();
    if (enabled == 1u)
    {
        previous = rrj_spu_irq_callback_psx(0);
        active = r_u32(0x800810BCu);
        w_u32(active + 31508u, previous);
        previous = sub_80051058(0x8005FCE8u);
        active = r_u32(0x800810BCu);
        w_u32(active + 31512u, previous);
    }
    active = r_u32(0x800810BCu);
    w_u32(active + 31516u, enabled);
    sub_80043DF4();
    return 1u;
}

uint32_t sub_8005FBDC(uint32_t source, uint32_t position, uint32_t channel)
{
    uint32_t offset, context, slot, address;
    FUNCTION_MARKER(0x8005FBDCu, "RASHCDF.BIN");
    for (offset = 0u; offset < 768u; offset += 16u)
        w_u8(source + offset + 1u, 2u);
    context = r_u32(0x800810BCu);
    slot = r_u32(context + 31556u) & 3u;
    if (!slot)
        w_u8(source + 1u, 6u);
    else if (!((r_u32(context + 31556u) + 1u) & 3u))
        w_u8(source + 753u, 3u);
    context = r_u32(0x800810BCu);
    slot = r_u32(context + 31556u) & 3u;
    w_u32(context + 31532u + 4u * slot, position);
    slot = r_u32(context + 31556u) & 3u;
    address = r_u32(context + 31580u + 4u * channel) + 768u * slot;
    if (channel == 1u)
        w_u32(context + 31556u, r_u32(context + 31556u) + 1u);
    SpuSetTransferMode(0);
    SpuSetTransferStartAddr(address);
    return sub_80050F28(source, 768u);
}

uint32_t sub_8005FCE8(void)
{
    uint32_t context, state, result = 2u;
    FUNCTION_MARKER(0x8005FCE8u, "RASHCDF.BIN");
    context = r_u32(0x800810BCu);
    state = r_u32(context + 31560u);
    if (state == 1u)
    {
        sub_8005FBDC(r_u32(context + 31568u), r_u32(context + 31572u), 1u);
        context = r_u32(0x800810BCu);
        w_u32(context + 31560u, 2u);
        return 2u;
    }
    state = r_u32(context + 31560u);
    if (state == 2u)
    {
        w_u32(context + 31560u, 0u);
        result = r_u32(context + 31564u);
        if (result)
        {
            result = r_u32(context) + 768u;
            w_u32(context, result);
            w_u32(context + 31564u, 0u);
        }
    }
    return result;
}

uint32_t sub_80060304(uint32_t data)
{
    uint32_t scalar_word_3;
    uint32_t scalar_word_4;
    uint32_t scalar_word_5;

    uint32_t tag, first, second, bytes, space, context, position, result;
    uint32_t destination, source, remaining;
    int stereo;
    FUNCTION_MARKER(0x80060304u, "RASHCDF.BIN");
    tag = ((scalar_word_3 = r_u32(((uint32_t)(data)))), (((uint32_t)((scalar_word_3 >> 24) | ((scalar_word_3 >> 8) & 0xff00u) | ((scalar_word_3 & 0xff00u) << 8) | (scalar_word_3 << 24)))));
    stereo = (tag & 0xFFFFFF00u) == 0x61643200u;
    if ((tag & 0xFFFF0000u) == 0x61640000u)
    {
        first = data + 16u;
        bytes = ((scalar_word_4 = r_u32(((uint32_t)(data + 4u)))), (((uint32_t)((scalar_word_4 >> 24) | ((scalar_word_4 >> 8) & 0xff00u) | ((scalar_word_4 & 0xff00u) << 8) | (scalar_word_4 << 24))))) - 16u;
    }
    else
    {
        first = data + 12u;
        bytes = ((scalar_word_5 = r_u32(((uint32_t)(data + 4u)))), (((uint32_t)((scalar_word_5 >> 24) | ((scalar_word_5 >> 8) & 0xff00u) | ((scalar_word_5 & 0xff00u) << 8) | (scalar_word_5 << 24))))) - 12u;
    }
    second = first;
    if (stereo)
    {
        bytes >>= 1;
        second += bytes;
    }
    context = r_u32(0x800810BCu);
    for (;;)
    {
        uint32_t written = r_u32(context + 4u);
        uint32_t consumed = r_u32(context);
        if (consumed - (written - 15360u) >= bytes)
            break;
        rrj_native_wait_events();
    }
    context = r_u32(0x800810BCu);
    position = r_u32(context + 4u) % 15360u;
    space = 15360u - position;
    if (space < bytes)
    {
        position = r_u32(context + 4u) % 15360u;
        sub_8001E0B4(xport_guest_ptr(context + position + 16u, space), xport_guest_ptr(first, 1), space);
        context = r_u32(0x800810BCu);
        remaining = bytes - space;
        sub_8001E0B4(xport_guest_ptr(context + 16u, remaining), xport_guest_ptr(first + space, 1), remaining);
        context = r_u32(0x800810BCu);
        position = r_u32(context + 4u) % 15360u;
        source = stereo ? second : first;
        sub_8001E0B4(xport_guest_ptr(context + position + 15376u, space), xport_guest_ptr(source, 1), space);
        context = r_u32(0x800810BCu);
        destination = context + 15376u;
        source += space;
    }
    else
    {
        position = r_u32(context + 4u) % 15360u;
        sub_8001E0B4(xport_guest_ptr(context + position + 16u, bytes), xport_guest_ptr(first, 1), bytes);
        context = r_u32(0x800810BCu);
        position = r_u32(context + 4u) % 15360u;
        source = stereo ? second : first;
        remaining = bytes;
        destination = context + position + 15376u;
    }
    sub_8001E0B4(xport_guest_ptr(destination, remaining), xport_guest_ptr(source, 1), remaining);
    context = r_u32(0x800810BCu);
    result = r_u32(context + 4u) + bytes;
    w_u32(context + 4u, result);
    return result;
}

void sub_8006066C(uint32_t destination, uint32_t source, uint32_t bytes)
{
    uint32_t copied, index, value;
    FUNCTION_MARKER(0x8006066Cu, "RASHCDF.BIN");
    for (copied = 0u; copied < bytes; copied += 15u)
    {
        value = r_u8(source++);
        w_u8(destination++, (uint8_t)value);
        w_u8(destination++, 0u);
        for (index = 0u; index < 14u; ++index)
        {
            value = r_u8(source++);
            w_u8(destination++, (uint8_t)value);
        }
    }
}

uint32_t sub_80060780(uint32_t data)
{
    uint32_t scalar_word_6;

    uint32_t input_bytes, output_bytes, first, second, context, position, space;
    uint32_t remaining, destination, source, result;
    FUNCTION_MARKER(0x80060780u, "RASHCDF.BIN");
    input_bytes = 15u * (((((scalar_word_6 = r_u32(((uint32_t)(data + 4u)))), (((uint32_t)((scalar_word_6 >> 24) | ((scalar_word_6 >> 8) & 0xff00u) | ((scalar_word_6 & 0xff00u) << 8) | (scalar_word_6 << 24))))) - 16u) >> 1) / 15u);
    first = data + 16u;
    second = first + input_bytes;
    output_bytes = 16u * (input_bytes / 15u);
    context = r_u32(0x800810BCu);
    for (;;)
    {
        uint32_t written = r_u32(context + 4u);
        uint32_t consumed = r_u32(context);
        if (consumed - (written - 15360u) >= output_bytes)
            break;
        rrj_native_wait_events();
    }
    context = r_u32(0x800810BCu);
    position = r_u32(context + 4u) % 15360u;
    space = 15u * ((15360u - position) >> 4);
    if (space < input_bytes)
    {
        position = r_u32(context + 4u) % 15360u;
        sub_8006066C(context + position + 16u, first, space);
        context = r_u32(0x800810BCu);
        remaining = input_bytes - space;
        sub_8006066C(context + 16u, first + space, remaining);
        context = r_u32(0x800810BCu);
        position = r_u32(context + 4u) % 15360u;
        sub_8006066C(context + position + 15376u, second, space);
        context = r_u32(0x800810BCu);
        source = second + space;
        destination = context + 15376u;
    }
    else
    {
        position = r_u32(context + 4u) % 15360u;
        sub_8006066C(context + position + 16u, first, input_bytes);
        context = r_u32(0x800810BCu);
        position = r_u32(context + 4u) % 15360u;
        source = second;
        remaining = input_bytes;
        destination = context + position + 15376u;
    }
    sub_8006066C(destination, source, remaining);
    context = r_u32(0x800810BCu);
    result = r_u32(context + 4u) + output_bytes;
    w_u32(context + 4u, result);
    return result;
}

uint32_t sub_8005FD78(uint32_t bytes)
{
    uint32_t context, written, consumed;
    FUNCTION_MARKER(0x8005FD78u, "RASHCDF.BIN");
    context = r_u32(0x800810BCu);
    written = r_u32(context + 4u);
    consumed = r_u32(context);
    if (bytes >= written - consumed || r_u32(context + 31608u))
        return 0u;
    consumed = r_u32(context);
    return context + consumed % 15360u + 16u;
}

uint32_t sub_8005FDEC(uint32_t bytes)
{
    uint32_t context, written, consumed;
    FUNCTION_MARKER(0x8005FDECu, "RASHCDF.BIN");
    context = r_u32(0x800810BCu);
    written = r_u32(context + 4u);
    consumed = r_u32(context);
    if (bytes >= written - consumed || r_u32(context + 31608u))
        return 0u;
    consumed = r_u32(context);
    return context + consumed % 15360u + 15376u;
}

uint32_t sub_8005FB44(uint32_t position)
{
    uint32_t context, mode, factor, divisor;
    FUNCTION_MARKER(0x8005FB44u, "RASHCDF.BIN");
    context = r_u32(0x800810BCu);
    mode = r_u32(context + 31604u);
    if (!mode)
    {
        if (r_u32(context + 31612u) == 2048u)
        {
            factor = 5328u;
            divisor = 4375u;
        }
        else
        {
            factor = 447552u;
            divisor = 535459u;
        }
    }
    else if (mode == 1u)
    {
        factor = 64u;
        divisor = 63u;
    }
    else
        return 0u;
    return (uint32_t)(((uint64_t)position * factor) / divisor);
}

uint32_t sub_8005FE60(void)
{
    uint32_t context, result, first, second, position, next, voice;
    FUNCTION_MARKER(0x8005FE60u, "RASHCDF.BIN");
    context = r_u32(0x800810BCu);
    result = r_u32(context + 8u);
    if (!result)
        return result;
    sub_80050B18(0u);
    rrj_spu_irq_callback_psx(0);
    context = r_u32(0x800810BCu);
    if (!r_u32(context + 31560u) && ((int32_t)(uint32_t)(2u * r_u32(context + 31556u) - r_u32(context + 31552u))) < 6)
    {
        first = sub_8005FD78(768u);
        if (first)
        {
            second = sub_8005FDEC(768u);
            context = r_u32(0x800810BCu);
            w_u32(context + 31576u, 1u);
            w_u32(context + 31564u, 1u);
            w_u32(context + 31560u, 1u);
            w_u32(context + 31568u, second);
            position = r_u32(context + 31548u);
            w_u32(context + 31572u, position);
            sub_8005FBDC(first, r_u32(context + 31548u), 0u);
            context = r_u32(0x800810BCu);
            w_u32(context + 31548u, r_u32(context + 31548u) + 768u);
        }
        else
        {
            context = r_u32(0x800810BCu);
            first = context + 30736u;
            w_u32(context + 31576u, 0u);
            w_u32(context + 31564u, 0u);
            w_u32(context + 31560u, 1u);
            w_u32(context + 31568u, first);
            w_u32(context + 31572u, 0u);
            sub_8005FBDC(first, 0u, 0u);
        }
    }
    context = r_u32(0x800810BCu);
    if (!(r_u32(context + 31552u) & 1u))
    {
        position = r_u32(context + 31532u + ((2u * r_u32(context + 31552u)) & 12u));
        if (position)
        {
            result = sub_8005FB44(position);
            context = r_u32(0x800810BCu);
            w_u32(context + 31524u, result);
        }
    }
    context = r_u32(0x800810BCu);
    next = r_u32(context + 31552u) + 1u;
    w_u32(context + 31552u, next);
    next = r_u32(context + 31552u) & 7u;
    voice = r_u32(context + 31580u);
    sub_80050C58(voice + 384u * next);
    rrj_spu_irq_callback_psx(0x8005FE60);
    return sub_80050B18(1u);
}

void sub_8006013C(void)
{
    uint32_t context, index, first, second, result, left, right;
    FUNCTION_MARKER(0x8006013Cu, "RASHCDF.BIN");
    context = r_u32(0x800810BCu);
    if (!r_u32(context + 8u))
    {
        w_u32(context + 31548u, 768u);
        for (index = 0u; index < 3u; ++index)
        {
            first = sub_8005FD78(768u);
            if (first)
                second = sub_8005FDEC(768u);
            else
            {
                first = r_u32(0x800810BCu) + 30736u;
                second = first;
            }
            context = r_u32(0x800810BCu);
            w_u32(context + 31560u, 1u);
            w_u32(context + 31564u, 1u);
            w_u32(context + 31568u, second);
            w_u32(context + 31572u, 1u);
            sub_8005FBDC(first, r_u32(context + 31548u), 0u);
            context = r_u32(0x800810BCu);
            w_u32(context + 31548u, r_u32(context + 31548u) + 768u);
            if (r_u32(context + 31560u))
            {
                context = r_u32(0x800810BCu);
                while (r_u32(context + 31560u))
                {
                    /* Dispatch completed native DMA while the original waits for its interrupt */
                    rrj_spu_completion_service();
                }
            }
        }
        context = r_u32(0x800810BCu);
        w_u32(context + 31560u, 3u);
        w_u32(context + 8u, 1u);
        sub_8005FE60();
        w_u32(r_u32(0x800810BCu) + 31560u, 0u);
    }
    context = r_u32(0x800810BCu);
    result = SpuSetReverbVoice(0, r_u32(context + 31588u));
    context = r_u32(0x800810BCu);
    w_u32(context + 31596u, result);
    result = SpuSetReverbVoice(0, r_u32(context + 31592u));
    context = r_u32(0x800810BCu);
    w_u32(context + 31600u, result);
    left = r_u32(context + 31588u);
    right = r_u32(context + 31592u);
    SpuSetKey(1, left | right);
}
