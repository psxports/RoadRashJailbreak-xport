#include "native_game_api.h"
#include "psx.h"
#include "psx_spu.h"
/* F preview lifecycle, actual nested stop/start; CD/codec are host boundaries. */
#include "video_preview.h"
#include "video_stop.h"
#include "menu_latch.h"
#include "resource.h"
#include <stdlib.h>

static uint32_t stream_call(RRJMemory *m, RRJVideoPhaseCall call, uint32_t target, uint32_t a0, uint32_t a1, uint32_t a2)
{
    uint32_t args[9] = {a0, a1, a2, 0, 0, 0, 0, 0, 0};
    if (!call)
        abort();
    return call(m, target, args);
}

static uint32_t stream_be32(RRJMemory *m, uint32_t address)
{
    uint32_t word = rrj_read32(address);
    return (word >> 24) | ((word >> 8) & 0xff00u) | ((word & 0xff00u) << 8) | (word << 24);
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x800602EC sub_F_800602EC */
uint32_t sub_F_800602EC(void)
{
    FUNCTION_MARKER(0x800602ECu, "RASHCDF.BIN");
    uint32_t context = rrj_read32(0x800810BCu);
    rrj_write32(context + 0x7B20u, 0u);
    return context;
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80062218 sub_F_80062218 */
uint32_t sub_F_80062218(uint32_t context, uint32_t callback, uint32_t handle, uint32_t loop, uint32_t metadata, RRJVideoPhaseCall call)
{
    FUNCTION_MARKER(0x80062218u, "RASHCDF.BIN");
    uint32_t sectors, buffer_bytes, descriptors, descriptor_base, buffer, value, active;
    (void)(uint32_t)sub_800148DC(metadata, handle);
    value = stream_call(rrj_host_context(), call, 0x800148BCu, handle, 0u, 0u);
    rrj_write32(metadata + 4u, value);
    sectors = rrj_read32(0x8009C2E0u);
    buffer_bytes = rrj_read32(0x8009C2E4u);
    descriptors = rrj_read32(0x8009C2E8u);
    (void)stream_call(rrj_host_context(), call, 0x800448B4u, context, buffer_bytes + (sectors << 11) + 0x57cu + 56u * descriptors, 0u);
    descriptor_base = context + 0x57cu;
    descriptors = rrj_read32(0x8009C2E8u);
    rrj_write32(0x8009C2D0u, context);
    rrj_write32(0x8009C2D4u, descriptor_base);
    rrj_write32(context + 0x570u, callback);
    rrj_write32(context + 0x414u, 0u);
    rrj_write32(context + 0x418u, 0u);
    rrj_write32(context + 0x41cu, 0u);
    rrj_write32(context + 0x43cu, 0u);
    rrj_write32(context + 0x434u, 0u);
    rrj_write32(context + 0x430u, 0u);
    rrj_write32(context + 0x42cu, 0u);
    rrj_write32(context + 0x440u, 0u);
    rrj_write32(context + 0x448u, 0u);
    rrj_write32(context + 0x44cu, 0u);
    buffer = descriptor_base + 56u * descriptors;
    buffer_bytes = rrj_read32(0x8009C2E4u);
    rrj_write32(0x8009C2D8u, buffer);
    rrj_write32(0x8009C2DCu, buffer + buffer_bytes);
    (void)sub_8001E0B4(context + 0x588u, metadata, 24u);
    (void)sub_8001E0B4(rrj_read32(0x8009C2D4u) + 0x44u, metadata, 24u);
    active = rrj_read32(0x8009C2D0u);
    rrj_write32(active + 0x438u, loop);
    rrj_write32(active + 0x424u, 0u);
    rrj_write32(active + 0x428u, 0u);
    value = stream_call(rrj_host_context(), call, 0x80045C2Cu, 0u, 0u, 0u);
    active = rrj_read32(0x8009C2D0u);
    rrj_write32(active + 0x574u, value);
    value = stream_call(rrj_host_context(), call, 0x80046614u, 0u, 0u, 0u);
    active = rrj_read32(0x8009C2D0u);
    rrj_write32(active + 0x578u, value);
    (void)(uint32_t)sub_F_80061730();
    (void)stream_call(rrj_host_context(), call, 0x800620D4u, 1u, 0u, 0u);
    return 1u;
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80061730 sub_F_80061730 */
uint32_t sub_F_80061730(void)
{
    FUNCTION_MARKER(0x80061730u, "RASHCDF.BIN");
    uint32_t context = rrj_read32(0x8009C2D0u);
    uint32_t result = rrj_read32(context + 0x43cu);
    uint32_t index, base, current, descriptor;
    if (result)
        return result;
    index = rrj_read32(context + 0x434u);
    base = rrj_read32(0x8009C2D4u);
    current = rrj_read32(context + 0x42cu);
    descriptor = base + 56u * index;
    rrj_write32(context + 0x430u, descriptor);
    if (!current)
        rrj_write32(context + 0x42cu, descriptor);
    context = rrj_read32(0x8009C2D0u);
    descriptor = rrj_read32(context + 0x430u);
    rrj_write32(descriptor, (rrj_read32(descriptor + 16u) + 2047u) >> 11);
    rrj_write32(rrj_read32(context + 0x430u) + 4u, 0u);
    rrj_write32(rrj_read32(context + 0x430u) + 8u, 0u);
    rrj_write32(rrj_read32(context + 0x430u) + 40u, 0u);
    index = rrj_read32(context + 0x434u);
    descriptor = rrj_read32(context + 0x430u);
    rrj_write32(context + 0x444u, 0u);
    rrj_write32(context + 0x440u, 0u);
    rrj_write32(context + 0x448u, 0u);
    rrj_write32(context + 0x44cu, 0u);
    rrj_write32(context + 0x434u, index + 1u);
    rrj_write32(descriptor + 52u, 0u);
    descriptor = rrj_read32(context + 0x430u);
    rrj_write32(descriptor + 44u, rrj_read32(0x8009C2D8u));
    descriptor = rrj_read32(context + 0x430u);
    rrj_write32(descriptor + 48u, rrj_read32(0x8009C2DCu));
    rrj_write32(context + 0x43cu, 0u);
    return 0u;
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x800620D4 sub_F_800620D4 */
uint32_t sub_F_800620D4(uint32_t enabled, RRJVideoPhaseCall call, RRJStreamBytes control)
{
    FUNCTION_MARKER(0x800620D4u, "RASHCDF.BIN");
    uint32_t context, previous;
    if (enabled)
    {
        uint8_t mode = 0xa0u;
        (void)stream_call(rrj_host_context(), call, 0x800457A8u, 0u, 0u, 0u);
        if (!control)
            abort();
        (void)control(rrj_host_context(), 0x80045A80u, 14u, &mode, 1u);
        (void)sub_80043DD4();
        context = rrj_read32(0x8009C2D0u);
        rrj_write32(context + 0x410u, 0u);
        rrj_write32(context + 0x408u, 0u);
        rrj_write32(context + 0x40cu, 0u);
        previous = stream_call(rrj_host_context(), call, 0x800457E8u, 0x80061584u, 0u, 0u);
        rrj_write32(0x800810E0u, previous);
        previous = stream_call(rrj_host_context(), call, 0x800457FCu, 0x800611F4u, 0u, 0u);
        rrj_write32(0x800810E4u, previous);
        context = rrj_read32(0x8009C2D0u);
        rrj_write32(context + 0x420u, 1u);
        rrj_write32(context + 0x41cu, 1u);
    }
    else
    {
        (void)sub_80043DD4();
        context = rrj_read32(0x8009C2D0u);
        previous = rrj_read32(context + 0x420u);
        rrj_write32(context + 0x41cu, 0u);
        rrj_write32(context + 0x410u, 0u);
        rrj_write32(context + 0x408u, 0u);
        rrj_write32(context + 0x40cu, 0u);
        if (previous)
        {
            (void)stream_call(rrj_host_context(), call, 0x800457E8u, rrj_read32(0x800810E0u), 0u, 0u);
            (void)stream_call(rrj_host_context(), call, 0x800457FCu, rrj_read32(0x800810E4u), 0u, 0u);
            context = rrj_read32(0x8009C2D0u);
            rrj_write32(0x800810E0u, 0u);
            rrj_write32(0x800810E4u, 0u);
            rrj_write32(context + 0x420u, 0u);
        }
    }
    return stream_call(rrj_host_context(), call, 0x80043DF4u, 0u, 0u, 0u);
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80060EC4 sub_F_80060EC4 */
uint32_t sub_F_80060EC4(void)
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

/* FUNCTION_MARKER: RASHCDF.BIN:0x8005F36C sub_F_8005F36C */
uint32_t sub_F_8005F36C(const uint32_t arguments[9], RRJVideoPhaseCall call, RRJStreamRead read)
{
    FUNCTION_MARKER(0x8005F36Cu, "RASHCDF.BIN");
    uint32_t result = 1u;
    uint32_t args[9] = {0};
    if (!call || !read)
        abort();
    if (arguments[1])
        sub_F_80062420();
    else
        sub_F_800623F8();
    args[1] = 0x800602ECu;
    args[2] = arguments[0];
    args[3] = arguments[8];
    args[0] = rrj_read32(0x800810ACu);
    args[4] = 2048u;
    if (!call(rrj_host_context(), 0x80062218u, args))
        return result;
    if (arguments[2])
    {
        args[0] = rrj_read32(0x800810B0u);
        args[1] = 0x400000u;
        args[2] = rrj_read32(0x800810B4u);
        args[3] = 0x800000u;
        args[5] = arguments[5];
        args[4] = rrj_read32(0x800810B8u);
        args[6] = 1u;
        args[7] = 0u;
        args[8] = 0u;
        result = call(rrj_host_context(), 0x80060D14u, args);
    }
    if (result)
    {
        uint32_t length = arguments[6] * arguments[7];
        (void)read(rrj_host_context(), &length);
        (void)sub_F_8005F484(arguments[3], arguments[4], arguments[2], arguments[1], 0u, read, call);
    }
    return result;
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80061060 sub_F_80061060 */
uint32_t sub_F_80061060(uint32_t command, uint32_t parameter, uint32_t result, RRJVideoPhaseCall call)
{
    FUNCTION_MARKER(0x80061060u, "RASHCDF.BIN");
    uint32_t context = rrj_read32(0x8009C2D0u);
    uint32_t index = rrj_read32(context + 0x40cu), pending;
    rrj_write32(context + 8u + index * 4u, command & 0xffu);
    rrj_write32(context + 0x40cu, index + 1u);
    index = rrj_read32(context + 0x40cu);
    rrj_write32(context + 8u + index * 4u, parameter);
    rrj_write32(context + 0x40cu, index + 1u);
    index = rrj_read32(context + 0x40cu);
    rrj_write32(context + 8u + index * 4u, result);
    rrj_write32(context + 0x40cu, index + 1u);
    rrj_write32(context + 0x40cu, rrj_read32(context + 0x40cu) % 48u);
    pending = rrj_read32(context + 0x410u) + 1u;
    rrj_write32(context + 0x410u, pending);
    if (rrj_read32(context + 0x410u) == 1u)
    {
        while (!stream_call(rrj_host_context(), call, 0x8004594Cu, command & 0xffu, parameter, 0u))
        {
        }
    }
    return 1u;
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80061180 sub_F_80061180 */
uint32_t sub_F_80061180(uint32_t buffer, uint32_t sectors, uint32_t position, RRJVideoPhaseCall call)
{
    FUNCTION_MARKER(0x80061180u, "RASHCDF.BIN");
    uint32_t context = rrj_read32(0x8009C2D0u);
    rrj_write32(context + 0x450u, buffer);
    rrj_write32(context + 0x454u, sectors);
    return sub_F_80061060(6u, position, 0u, call);
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80061584 sub_F_80061584 */
uint32_t sub_F_80061584(uint32_t event, RRJVideoPhaseCall call)
{
    FUNCTION_MARKER(0x80061584u, "RASHCDF.BIN");
    uint32_t context, result, index, command, parameter;
    if ((event & 0xffu) != 2u)
    {
        uint32_t errors = rrj_read32(0x800810C4u);
        context = rrj_read32(0x8009C2D0u);
        rrj_write32(0x800810C4u, errors + 1u);
        result = rrj_read32(context + 0x424u) + 1u;
        rrj_write32(context + 0x424u, result);
        return result;
    }
    context = rrj_read32(0x8009C2D0u);
    if (rrj_read32(context + 0x424u) >= 2u)
        return 0u;
    rrj_write32(context + 0x424u, 0u);
    result = rrj_read32(context + 0x41cu);
    if (!result)
        return result;
    result = rrj_read32(context + 0x410u);
    if (!result)
        return result;
    if (rrj_read32(context + 0x418u))
    {
        rrj_write32(context + 0x418u, rrj_read32(context + 0x418u) - 1u);
        if (!rrj_read32(context + 0x418u))
        {
            uint32_t buffer = rrj_read32(context + 0x450u);
            uint32_t sectors = rrj_read32(context + 0x454u);
            rrj_write32(context + 4u, buffer);
            rrj_write32(context, sectors);
            rrj_write32(context + 0x414u, 2u);
        }
    }
    context = rrj_read32(0x8009C2D0u);
    rrj_write32(context + 0x410u, rrj_read32(context + 0x410u) - 1u);
    rrj_write32(context + 0x408u, rrj_read32(context + 0x408u) + 3u);
    rrj_write32(context + 0x408u, rrj_read32(context + 0x408u) % 48u);
    result = rrj_read32(context + 0x410u);
    if (!result)
        return result;
    index = rrj_read32(context + 0x408u);
    command = rrj_read32(context + 8u + index * 4u);
    index = rrj_read32(context + 0x408u);
    parameter = rrj_read32(context + 8u + (index + 1u) * 4u);
    index = rrj_read32(context + 0x408u);
    (void)rrj_read32(context + 8u + (index + 2u) * 4u);
    do
    {
        result = stream_call(rrj_host_context(), call, 0x8004594Cu, command & 0xffu, parameter, 0u);
    } while (!result);
    return result;
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80060F44 sub_F_80060F44 */
uint32_t sub_F_80060F44(uint32_t command, uint32_t parameter, uint32_t result)
{
    FUNCTION_MARKER(0x80060F44u, "RASHCDF.BIN");
    uint32_t context, index, pending;
    (void)sub_80043DD4();
    context = rrj_read32(0x8009C2D0u);
    index = rrj_read32(context + 0x40cu);
    rrj_write32(context + 8u + index * 4u, command & 0xffu);
    rrj_write32(context + 0x40cu, index + 1u);
    index = rrj_read32(context + 0x40cu);
    rrj_write32(context + 8u + index * 4u, parameter);
    rrj_write32(context + 0x40cu, index + 1u);
    index = rrj_read32(context + 0x40cu);
    rrj_write32(context + 8u + index * 4u, result);
    rrj_write32(context + 0x40cu, index + 1u);
    rrj_write32(context + 0x40cu, rrj_read32(context + 0x40cu) % 48u);
    pending = rrj_read32(context + 0x410u) + 1u;
    rrj_write32(context + 0x410u, pending);
    pending = rrj_read32(context + 0x410u);
    (void)sub_80043DF4();
    if (pending == 1u)
    {
        while (!CdControl((uint8_t)command, parameter ? (uint8_t *)rrj_at(parameter, 1u) : NULL, result ? (uint8_t *)rrj_at(result, 1u) : NULL))
        {
        }
    }
    return 1u;
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80061148 sub_F_80061148 */
uint32_t sub_F_80061148(uint32_t buffer, uint32_t sectors, uint32_t position)
{
    FUNCTION_MARKER(0x80061148u, "RASHCDF.BIN");
    uint32_t context = rrj_read32(0x8009C2D0u);
    rrj_write32(context + 0x450u, buffer);
    rrj_write32(context + 0x454u, sectors);
    return sub_F_80060F44(6u, position, 0u);
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x800611B8 sub_F_800611B8 */
uint32_t sub_F_800611B8(uint32_t source_position, uint32_t destination_position, uint32_t sectors)
{
    FUNCTION_MARKER(0x800611B8u, "RASHCDF.BIN");
    uint32_t sector = (uint32_t)sub_80046768(source_position);
    CdIntToPos(rrj_s32(sector + sectors), (CdlLOC *)rrj_at(destination_position, sizeof(CdlLOC)));
    return destination_position;
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80061818 sub_F_80061818 */
uint32_t sub_F_80061818(void)
{
    FUNCTION_MARKER(0x80061818u, "RASHCDF.BIN");
    uint32_t context = rrj_read32(0x8009C2D0u);
    uint32_t result = rrj_read32(context + 0x43cu);
    if (result)
        return result;
    if (rrj_read32(context + 0x438u))
    {
        uint32_t current = rrj_read32(context + 0x430u);
        uint32_t next = rrj_read32(0x8009C2D4u), descriptor;
        if (current == next)
            next += 56u;
        rrj_write32(context + 0x430u, next);
        context = rrj_read32(0x8009C2D0u);
        (void)sub_8001E0B4(rrj_read32(context + 0x430u), rrj_read32(context + 0x42cu), 56u);
        context = rrj_read32(0x8009C2D0u);
        descriptor = rrj_read32(context + 0x430u);
        rrj_write32(descriptor, (rrj_read32(descriptor + 16u) + 2047u) >> 11);
        rrj_write32(rrj_read32(context + 0x430u) + 4u, 0u);
        rrj_write32(rrj_read32(context + 0x430u) + 8u, 0u);
        rrj_write32(rrj_read32(context + 0x430u) + 40u, 0u);
        result = rrj_read32(context + 0x434u);
        descriptor = rrj_read32(context + 0x430u);
        rrj_write32(context + 0x434u, result + 1u);
        rrj_write32(descriptor + 52u, 0u);
        descriptor = rrj_read32(context + 0x430u);
        rrj_write32(descriptor + 44u, rrj_read32(0x8009C2D8u));
        descriptor = rrj_read32(context + 0x430u);
        rrj_write32(descriptor + 48u, rrj_read32(0x8009C2DCu));
        rrj_write32(context + 0x43cu, 0u);
    }
    else
        rrj_write32(context + 0x43cu, 2u);
    context = rrj_read32(0x8009C2D0u);
    return rrj_read32(context + 0x43cu);
}

static uint32_t stream_min(uint32_t left, uint32_t right)
{
    return rrj_s32(left) < rrj_s32(right) ? left : right;
}

static uint32_t stream_remainder(uint32_t value, uint32_t divisor)
{
    return divisor ? value % divisor : value;
}

static uint32_t stream_header_sector(const uint8_t header[12])
{
    uint32_t minute = (header[0] >> 4) * 10u + (header[0] & 15u);
    uint32_t second = (header[1] >> 4) * 10u + (header[1] & 15u);
    uint32_t frame = (header[2] >> 4) * 10u + (header[2] & 15u);
    return (minute * 60u + second) * 75u + frame - 150u;
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x800611F4 sub_F_800611F4 */
uint32_t sub_F_800611F4(uint32_t event, RRJVideoPhaseCall call, RRJStreamHeaderRead read_header)
{
    FUNCTION_MARKER(0x800611F4u, "RASHCDF.BIN");
    uint32_t context, descriptor, actual, expected, sectors;
    uint8_t header[12];
    if ((event & 255u) != 1u)
    {
        uint32_t errors = rrj_read32(0x800810C0u);
        context = rrj_read32(0x8009C2D0u);
        rrj_write32(0x800810C0u, errors + 1u);
        errors = rrj_read32(context + 0x424u) + 1u;
        rrj_write32(context + 0x424u, errors);
        return errors;
    }
    context = rrj_read32(0x8009C2D0u);
    if (rrj_read32(context + 0x424u) >= 2u)
        return 0u;
    rrj_write32(context + 0x424u, 0u);
    if (rrj_read32(context + 0x414u) != 2u)
        return 2u;
    if (!rrj_read32(context))
        return 0x80080000u;
    if (rrj_read32(0x800810D4u))
    {
        uint32_t address = rrj_read32(0x800810D8u);
        if (rrj_read32(address) & 0x01000000u)
        {
            uint32_t polls = 1u;
            while (polls != 65537u)
            {
                uint32_t busy = rrj_read32(address) & 0x01000000u;
                ++polls;
                if (!busy)
                    break;
            }
        }
    }
    context = rrj_read32(0x8009C2D0u);
    actual = rrj_read32(context + 4u) & 3u;
    if (actual)
    {
        rrj_write32(context + 0x414u, 0u);
        return actual;
    }
    if (!read_header)
        abort();
    read_header(rrj_host_context(), header);
    (void)stream_call(rrj_host_context(), call, 0x80045C50u, 0u, 0u, 0u);
    actual = stream_header_sector(header);
    context = rrj_read32(0x8009C2D0u);
    descriptor = rrj_read32(context + 0x430u);
    (void)sub_F_800611B8(descriptor + 12u, descriptor + 36u, rrj_read32(descriptor + 8u));
    context = rrj_read32(0x8009C2D0u);
    expected = (uint32_t)sub_80046768(rrj_read32(context + 0x430u) + 36u);
    if (actual != expected)
    {
        uint32_t errors = rrj_read32(0x800810C8u);
        context = rrj_read32(0x8009C2D0u);
        rrj_write32(0x800810C8u, errors + 1u);
    }
    else
        context = rrj_read32(0x8009C2D0u);
    (void)stream_call(rrj_host_context(), call, 0x80045BECu, rrj_read32(context + 4u), 512u, 0u);
    (void)stream_call(rrj_host_context(), call, 0x80045C50u, 0u, 0u, 0u);
    context = rrj_read32(0x8009C2D0u);
    (void)stream_call(rrj_host_context(), call, 0x80045BECu, context + 0x458u, 70u, 0u);
    (void)stream_call(rrj_host_context(), call, 0x80045C50u, 0u, 0u, 0u);
    if (actual != expected)
    {
        context = rrj_read32(0x8009C2D0u);
        descriptor = rrj_read32(context + 0x430u);
        (void)sub_F_800611B8(descriptor + 12u, descriptor + 36u, rrj_read32(descriptor + 8u));
        context = rrj_read32(0x8009C2D0u);
        rrj_write32(context + 0x428u, rrj_read32(context + 0x428u) + 1u);
        rrj_write32(context + 0x418u, 2u);
        rrj_write32(context + 0x414u, 1u);
        rrj_write32(0x800810CCu, rrj_read32(0x800810CCu) + 1u);
        descriptor = rrj_read32(context + 0x430u);
        (void)sub_F_80061060(21u, descriptor + 36u, 0u, call);
        context = rrj_read32(0x8009C2D0u);
        return sub_F_80061180(rrj_read32(context + 4u), rrj_read32(context), rrj_read32(context + 0x430u) + 36u, call);
    }
    context = rrj_read32(0x8009C2D0u);
    rrj_write32(context + 4u, rrj_read32(context + 4u) + 2048u);
    sectors = rrj_read32(context);
    descriptor = rrj_read32(context + 0x430u);
    rrj_write32(context, sectors - 1u);
    rrj_write32(descriptor + 8u, rrj_read32(descriptor + 8u) + 1u);
    rrj_write32(context + 0x448u, rrj_read32(context + 0x448u) + 1u);
    sectors = rrj_read32(context);
    if (sectors)
        return sectors;
    descriptor = rrj_read32(context + 0x430u);
    sectors = rrj_read32(context);
    {
        uint32_t used = rrj_read32(descriptor + 8u);
        uint32_t produced = rrj_read32(context + 0x448u);
        uint32_t capacity = rrj_read32(0x8009C2E0u);
        uint32_t remainder = stream_remainder(produced + sectors, capacity);
        uint32_t remaining = rrj_read32(descriptor) - (used + sectors);
        uint32_t available = rrj_read32(context + 0x440u) + capacity - (produced + sectors);
        sectors = stream_min(stream_min(available, capacity - remainder), remaining);
    }
    context = rrj_read32(0x8009C2D0u);
    if (sectors)
    {
        uint32_t capacity = rrj_read32(0x8009C2E0u);
        uint32_t buffer;
        descriptor = rrj_read32(context + 0x430u);
        buffer = rrj_read32(descriptor + 0x30u);
        if (rrj_read32(context + 4u) == buffer + (capacity << 11))
            rrj_write32(context + 4u, buffer);
        context = rrj_read32(0x8009C2D0u);
        rrj_write32(context, sectors);
    }
    else
    {
        rrj_write32(context, 0u);
        rrj_write32(context + 0x414u, 0u);
    }
    return context;
}

static uint32_t stream_divide(uint32_t value, uint32_t divisor)
{
    if (!divisor)
        return rrj_s32(value) < 0 ? 1u : 0xffffffffu;
    if (value == 0x80000000u && divisor == 0xffffffffu)
        return value;
    return (uint32_t)(rrj_s32(value) / rrj_s32(divisor));
}

static void stream_bar(RRJMemory *m, uint32_t x, uint32_t y, uint32_t width, uint32_t blue, RRJVideoPhaseCall call, RRJStreamBytes draw)
{
    uint8_t packet[16] = {0, 0, 0, 3, 255, 255, (uint8_t)blue, 2};
    rrj_put16(packet + 8, (uint16_t)x);
    rrj_put16(packet + 10, (uint16_t)y);
    rrj_put16(packet + 12, (uint16_t)width);
    rrj_put16(packet + 14, 8);
    (void)stream_call(m, call, 0x800487C0u, 0u, 0u, 0u);
    if (!draw)
        abort();
    /* DrawPrim consumes only byte 3 and the three payload words */
    (void)draw(m, 0x80048D58u, 0u, packet, sizeof packet);
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x800609E4 sub_F_800609E4 */
uint32_t sub_F_800609E4(RRJVideoPhaseCall call, RRJStreamBytes draw)
{
    FUNCTION_MARKER(0x800609E4u, "RASHCDF.BIN");
    uint32_t context = rrj_read32(0x800810BCu);
    uint32_t written = rrj_read32(context + 4u);
    uint32_t read = rrj_read32(context);
    uint32_t available = 15360u - (read - (written - 15360u));
    uint32_t adjusted = rrj_s32(available) < 0 ? available + 63u : available;
    uint32_t width = (uint32_t)(rrj_s32(adjusted) >> 6);
    stream_bar(rrj_host_context(), 24u, 212u, 240u, 255u, call, draw);
    stream_bar(rrj_host_context(), 504u, 212u, 240u, 255u, call, draw);
    stream_bar(rrj_host_context(), 24u, 212u, width, 0u, call, draw);
    stream_bar(rrj_host_context(), 504u, 212u, width, 0u, call, draw);
    return stream_call(rrj_host_context(), call, 0x800487C0u, 0u, 0u, 0u);
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80061C44 sub_F_80061C44 */
uint32_t sub_F_80061C44(uint32_t length, RRJVideoPhaseCall call, RRJStreamBytes draw)
{
    FUNCTION_MARKER(0x80061C44u, "RASHCDF.BIN");
    uint32_t context = rrj_read32(0x8009C2D0u);
    uint32_t descriptor = rrj_read32(context + 0x42cu);
    uint32_t offset = rrj_read32(context + 0x444u);
    uint32_t remaining = 2048u - offset;
    rrj_write32(descriptor + 4u, rrj_read32(descriptor + 4u) + length);
    if (rrj_s32(length) < rrj_s32(remaining))
        rrj_write32(context + 0x444u, rrj_read32(context + 0x444u) + length);
    else
    {
        uint32_t overflow = length - remaining;
        rrj_write32(context + 0x444u, overflow & 0x7ffu);
        rrj_write32(context + 0x440u, rrj_read32(context + 0x440u) + (overflow >> 11) + 1u);
    }
    sub_F_80061938(call);
    context = rrj_read32(0x8009C2D0u);
    descriptor = rrj_read32(context + 0x42cu);
    if (rrj_read32(descriptor + 4u) == rrj_read32(descriptor + 16u) && rrj_read32(context + 0x438u))
    {
        uint32_t next = rrj_read32(context + 0x430u);
        rrj_write32(context + 0x44cu, 0u);
        offset = rrj_read32(context + 0x444u);
        rrj_write32(context + 0x42cu, next);
        if (offset)
            rrj_write32(context + 0x440u, rrj_read32(context + 0x440u) + 1u);
        context = rrj_read32(0x8009C2D0u);
        descriptor = rrj_read32(context + 0x42cu);
        rrj_write32(context + 0x444u, rrj_read32(descriptor + 0x28u));
    }
    if (!rrj_read32(0x800810DCu))
        return 0u;
    {
        uint32_t bytes = rrj_read32(0x8009C2E0u) << 11;
        int64_t product = (int64_t)rrj_s32(bytes) * (int64_t)rrj_s32(0x9c09c09du);
        uint32_t high = (uint32_t)((uint64_t)product >> 32);
        uint32_t scale = (uint32_t)(rrj_s32(high + bytes) >> 8) - (uint32_t)(rrj_s32(bytes) >> 31);
        uint32_t width = stream_divide(bytes, scale), available;
        context = rrj_read32(0x8009C2D0u);
        available = (rrj_read32(context + 0x448u) << 11) - ((rrj_read32(context + 0x440u) << 11) + rrj_read32(context + 0x444u));
        stream_bar(rrj_host_context(), 24u, 200u, width, 255u, call, draw);
        stream_bar(rrj_host_context(), 504u, 200u, width, 255u, call, draw);
        width = stream_divide(available, scale);
        stream_bar(rrj_host_context(), 24u, 200u, width, 0u, call, draw);
        stream_bar(rrj_host_context(), 504u, 200u, width, 0u, call, draw);
    }
    (void)sub_F_800609E4(call, draw);
    return stream_call(rrj_host_context(), call, 0x800487C0u, 0u, 0u, 0u);
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80061938 sub_F_80061938 */
void sub_F_80061938(RRJVideoPhaseCall call)
{
    FUNCTION_MARKER(0x80061938u, "RASHCDF.BIN");
    uint32_t context = rrj_read32(0x8009C2D0u), descriptor, sectors;
    if (rrj_read32(context + 0x424u) >= 2u)
    {
        uint32_t callback = rrj_read32(context + 0x570u);
        uint8_t mode = 0xa0u;
        if (callback)
            (void)stream_call(rrj_host_context(), call, callback, 0u, 0u, 0u);
        (void)sub_80043DD4();
        context = rrj_read32(0x8009C2D0u);
        rrj_write32(context + 0x410u, 0u);
        rrj_write32(context + 0x408u, 0u);
        rrj_write32(context + 0x40cu, 0u);
        rrj_write32(context + 0x41cu, 0u);
        (void)CdSyncCallbackPSX(0u);
        (void)CdReadyCallbackPSX(0u);
        (void)sub_80043DF4();
        (void)CdControl(8u, NULL, NULL);
        while (sub_F_80060EC4())
        {
        }
        (void)CdControlB(14u, &mode, NULL);
        (void)sub_80043DD4();
        (void)CdSyncCallbackPSX(0x80061584u);
        (void)CdReadyCallbackPSX(0x800611F4u);
        context = rrj_read32(0x8009C2D0u);
        rrj_write32(context + 0x41cu, 1u);
        rrj_write32(context + 0x414u, 1u);
        rrj_write32(context + 0x424u, 0u);
        (void)sub_80043DF4();
        context = rrj_read32(0x8009C2D0u);
        if (rrj_read32(context + 0x414u) && rrj_read32(context))
        {
            descriptor = rrj_read32(context + 0x430u);
            (void)sub_F_800611B8(descriptor + 12u, descriptor + 36u, rrj_read32(descriptor + 8u));
            context = rrj_read32(0x8009C2D0u);
            rrj_write32(context + 0x418u, 2u);
            rrj_write32(0x800810CCu, rrj_read32(0x800810CCu) + 1u);
            descriptor = rrj_read32(context + 0x430u);
            if (!sub_F_80060F44(21u, descriptor + 36u, 0u))
            {
                context = rrj_read32(0x8009C2D0u);
                rrj_write32(context + 0x43cu, 1u);
                return;
            }
            context = rrj_read32(0x8009C2D0u);
            sectors = rrj_read32(context);
            (void)sub_F_80061148(rrj_read32(context + 4u), sectors, rrj_read32(context + 0x430u) + 36u);
        }
        else
        {
            context = rrj_read32(0x8009C2D0u);
            rrj_write32(context + 0x414u, 0u);
        }
    }
    context = rrj_read32(0x8009C2D0u);
    if (rrj_read32(context + 0x414u))
        return;
    descriptor = rrj_read32(context + 0x430u);
    sectors = rrj_read32(descriptor) - rrj_read32(descriptor + 8u);
    if (!sectors)
        (void)(uint32_t)sub_F_80061818();
    context = rrj_read32(0x8009C2D0u);
    {
        uint32_t capacity = rrj_read32(0x8009C2E0u);
        uint32_t consumed = rrj_read32(context + 0x440u);
        uint32_t produced = rrj_read32(context + 0x448u);
        uint32_t remainder = stream_remainder(rrj_read32(context + 0x448u), capacity);
        sectors = stream_min(stream_min(consumed + capacity - produced, capacity - remainder), sectors);
    }
    if (!sectors)
        return;
    context = rrj_read32(0x8009C2D0u);
    descriptor = rrj_read32(context + 0x430u);
    (void)sub_F_800611B8(descriptor + 12u, descriptor + 36u, rrj_read32(descriptor + 8u));
    context = rrj_read32(0x8009C2D0u);
    rrj_write32(context + 0x44cu, rrj_read32(context + 0x448u) + sectors);
    rrj_write32(context + 0x418u, 2u);
    rrj_write32(0x800810CCu, rrj_read32(0x800810CCu) + 1u);
    descriptor = rrj_read32(context + 0x430u);
    if (!sub_F_80060F44(21u, descriptor + 36u, 0u))
    {
        context = rrj_read32(0x8009C2D0u);
        rrj_write32(context + 0x43cu, 1u);
        return;
    }
    context = rrj_read32(0x8009C2D0u);
    descriptor = rrj_read32(context + 0x430u);
    {
        uint32_t produced = rrj_read32(context + 0x448u);
        uint32_t displacement = rrj_read32(descriptor + 0x34u);
        uint32_t capacity = rrj_read32(0x8009C2E0u);
        uint32_t position = stream_remainder(produced + displacement, capacity);
        uint32_t buffer = rrj_read32(descriptor + 0x30u);
        rrj_write32(context + 0x414u, 1u);
        (void)sub_F_80061148(buffer + (position << 11), sectors, descriptor + 36u);
    }
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x800602D4 sub_F_800602D4 */
uint32_t sub_F_800602D4(void)
{
    FUNCTION_MARKER(0x800602D4u, "RASHCDF.BIN");
    rrj_write32(rrj_read32(0x800810BCu) + 0x7b20u, 1u);
    return 1u;
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80061F04 sub_F_80061F04 */
uint32_t sub_F_80061F04(uint32_t *length, RRJVideoPhaseCall call)
{
    FUNCTION_MARKER(0x80061F04u, "RASHCDF.BIN");
    uint32_t context = rrj_read32(0x8009C2D0u);
    uint32_t descriptor = rrj_read32(context + 0x42cu);
    uint32_t remaining = rrj_read32(descriptor + 16u) - rrj_read32(descriptor + 4u);
    uint32_t polls = 0u, retries = 0u;
    if (rrj_s32(remaining) < rrj_s32(*length))
        *length = remaining;
    for (;;)
    {
        uint32_t available;
        context = rrj_read32(0x8009C2D0u);
        available = (rrj_read32(context + 0x448u) << 11) - ((rrj_read32(context + 0x440u) << 11) + rrj_read32(context + 0x444u));
        if (rrj_s32(available) < 0)
            available = 0u;
        if (rrj_s32(available) >= rrj_s32(*length))
            break;
        if (rrj_read32(context + 0x43cu))
            return 0u;
        ++polls;
        if (polls > 0x3d0900u)
            rrj_write32(context + 0x424u, rrj_read32(context + 0x424u) + 2u);
        context = rrj_read32(0x8009C2D0u);
        if (rrj_read32(context + 0x414u) && rrj_read32(context + 0x424u) < 2u)
            continue;
        if (++retries >= 3u)
            return 0u;
        polls = 0u;
        /* TODO Bind the complete CD refill owner */
        (void)stream_call(rrj_host_context(), call, 0x80061938u, rrj_read32(context + 0x444u), 0u, 0u);
    }
    {
        uint32_t sector, offset, capacity, position, end, pointer, contiguous;
        context = rrj_read32(0x8009C2D0u);
        sector = rrj_read32(context + 0x440u);
        offset = rrj_read32(context + 0x444u);
        descriptor = rrj_read32(context + 0x42cu);
        position = rrj_read32(context + 0x440u) + rrj_read32(descriptor + 0x34u);
        capacity = rrj_read32(0x8009C2E0u);
        /* MIPS DIVU leaves the dividend in HI when the divisor is zero */
        position = capacity ? position % capacity : position;
        end = capacity ? sector % capacity : sector;
        pointer = rrj_read32(descriptor + 0x30u) + (position << 11) + rrj_read32(context + 0x444u);
        contiguous = ((capacity - 1u - end) << 11) + 2048u - offset;
        if (contiguous < *length)
        {
            (void)(uint32_t)sub_8001E0B4(pointer - (capacity << 11), pointer, contiguous);
            pointer -= rrj_read32(0x8009C2E0u) << 11;
        }
        return pointer;
    }
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x8005F484 sub_F_8005F484 */
uint32_t sub_F_8005F484(uint32_t x, uint32_t y, uint32_t channel, uint32_t mode, uint32_t frame, RRJStreamRead read, RRJVideoPhaseCall call)
{
    FUNCTION_MARKER(0x8005F484u, "RASHCDF.BIN");
    if (!read)
        abort();
    for (;;)
    {
        uint32_t length = 8u, header = read(rrj_host_context(), &length), tag, data;
        if (!header || !length)
            return 0u;
        tag = stream_be32(rrj_host_context(), header);
        length = stream_be32(rrj_host_context(), header + 4u);
        if (length >= 0x4400u)
        {
            (void)stream_call(rrj_host_context(), call, 0x80061C44u, 4u, 0u, 0u);
            continue;
        }
        data = read(rrj_host_context(), &length);
        if (!data || !length)
            return 0u;
        if (tag == 0x4d444332u || tag == 0x4d444543u)
        {
            uint32_t count = rrj_read32(0x8009C2F4u);
            if (rrj_s32(count) < 6)
            {
                uint32_t item = 0x8009C3D0u + 36u * count;
                rrj_write32(item, data);
                rrj_write32(item + 8u, frame ? 2u : 0u);
                rrj_write32(item + 12u, mode);
                rrj_write32(item + 28u, x);
                rrj_write32(item + 32u, y);
                rrj_write32(0x8009C2F4u, count + 1u);
                rrj_write32(item + 16u, length);
            }
            if (frame == 1u && channel)
                (void)stream_call(rrj_host_context(), call, 0x8006013Cu, 0u, 0u, 0u);
            (void)sub_F_800602D4();
            return length;
        }
        if (tag == 0x564c4330u)
            (void)stream_call(rrj_host_context(), call, 0x8002026Cu, data + 8u, 0u, 0u);
        else if ((tag >= 0x41643130u && tag <= 0x41643131u) || (tag >= 0x61643130u && tag <= 0x61643131u) || (tag >= 0x61643230u && tag <= 0x61643231u))
        {
            if (channel)
                (void)stream_call(rrj_host_context(), call, 0x80060304u, data, 0u, 0u);
        }
        else if (tag >= 0x61753030u && tag <= 0x61753031u)
        {
            if (channel)
                (void)stream_call(rrj_host_context(), call, 0x80060780u, data, 0u, 0u);
        }
        else
            length = 4u;
        (void)stream_call(rrj_host_context(), call, 0x80061C44u, length, 0u, 0u);
    }
}

static uint32_t h(RRJMemory *m, uint32_t a)
{
    return rrj_u16(rrj_at(a, 2));
}

static uint32_t sh(uint32_t v)
{
    return v & 0x8000 ? v | 0xffff0000 : v;
}

static void half(RRJMemory *m, uint32_t a, uint32_t v)
{
    rrj_put16(rrj_at(a, 2), (uint16_t)v);
}

uint32_t sub_F_8006FEF4(uint32_t desc, RRJVideoPhaseCall cb, RRJVideoPhaseOpen open, RRJSDKCall stop)
{
    FUNCTION_MARKER(0x8006FEF4u, "RASHCDF.BIN");
    uint32_t args[9] = {0}, file, flags, channel;
    if (!cb)
        abort();
    if (cb(rrj_host_context(), 0x80022A78, args))
        return 0;
    if (h(rrj_host_context(), 0x8009C5DA) & 1)
        return 0;
    if (!open)
        abort();
    file = open(rrj_host_context(), 0x8005BF84, rrj_read32(0x8008973C + 4 * rrj_read32(desc)));
    rrj_write32(0x8009C684, file);
    if (rrj_s32(file) < 0)
        return 1;
    rrj_write32(desc + 16, 0);
    flags = h(rrj_host_context(), 0x8009C5DA);
    args[0] = rrj_read32(0x8009C684);
    half(rrj_host_context(), 0x8009C5DA, flags | 1);
    channel = h(rrj_host_context(), desc + 8);
    half(rrj_host_context(), 0x8009C688, channel);
    args[3] = sh(h(rrj_host_context(), desc + 12));
    args[4] = sh(h(rrj_host_context(), desc + 14));
    args[5] = 2048;
    args[6] = 64;
    args[7] = 1024;
    args[8] = h(rrj_host_context(), desc + 10);
    args[1] = rrj_read32(desc + 4);
    args[2] = sh(channel);
    if (!cb(rrj_host_context(), 0x8005F36C, args))
    {
        (void)sub_F_8006FE6C(stop);
        return 0;
    }
    rrj_write32(desc + 16, rrj_read32(desc + 16) + 1);
    return 1;
}

uint32_t sub_F_80070018(uint32_t desc, RRJVideoPhaseCall cb, RRJVideoPhaseOpen open, RRJSDKCall stop)
{
    FUNCTION_MARKER(0x80070018u, "RASHCDF.BIN");
    uint32_t flags = h(rrj_host_context(), 0x8009C5DA), args[9] = {0}, result = 1;
    if (flags & 4)
        return sub_F_8006FE6C(stop);
    if (!(flags & 1))
    {
        if (flags & 2)
            return sub_F_8006FEF4(desc, cb, open, stop);
        half(rrj_host_context(), 0x8009C5DA, flags | 2);
        return 0;
    }
    args[4] = rrj_read32(desc + 16);
    args[0] = sh(h(rrj_host_context(), desc + 12));
    args[1] = sh(h(rrj_host_context(), desc + 14));
    args[2] = sh(h(rrj_host_context(), 0x8009C688));
    args[3] = rrj_read32(desc + 4);
    if (!cb)
        abort();
    if (!cb(rrj_host_context(), 0x8005F484, args))
    {
        half(rrj_host_context(), 0x8009C5DA, (h(rrj_host_context(), 0x8009C5DA) & 0xfffe) | 2);
        result = 0;
    }
    rrj_write32(desc + 16, rrj_read32(desc + 16) + 1);
    return result;
}
