/* F preview lifecycle, actual nested stop/start; CD/codec are host boundaries. */
#include "video_preview.h"
#include "video_stop.h"
#include "menu_latch.h"
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
    uint32_t word = rrj_read32(m, address);
    return (word >> 24) | ((word >> 8) & 0xff00u) | ((word & 0xff00u) << 8) | (word << 24);
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80061060 sub_F_80061060 */
uint32_t sub_F_80061060(RRJMemory *m, uint32_t command, uint32_t parameter, uint32_t result, RRJVideoPhaseCall call)
{
    uint32_t context = rrj_read32(m, 0x8009C2D0u);
    uint32_t index = rrj_read32(m, context + 0x40cu), pending;
    rrj_write32(m, context + 8u + index * 4u, command & 0xffu);
    rrj_write32(m, context + 0x40cu, index + 1u);
    index = rrj_read32(m, context + 0x40cu);
    rrj_write32(m, context + 8u + index * 4u, parameter);
    rrj_write32(m, context + 0x40cu, index + 1u);
    index = rrj_read32(m, context + 0x40cu);
    rrj_write32(m, context + 8u + index * 4u, result);
    rrj_write32(m, context + 0x40cu, index + 1u);
    rrj_write32(m, context + 0x40cu, rrj_read32(m, context + 0x40cu) % 48u);
    pending = rrj_read32(m, context + 0x410u) + 1u;
    rrj_write32(m, context + 0x410u, pending);
    if (rrj_read32(m, context + 0x410u) == 1u)
    {
        while (!stream_call(m, call, 0x8004594Cu, command & 0xffu, parameter, 0u))
        {
        }
    }
    return 1u;
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80061180 sub_F_80061180 */
uint32_t sub_F_80061180(RRJMemory *m, uint32_t buffer, uint32_t sectors, uint32_t position, RRJVideoPhaseCall call)
{
    uint32_t context = rrj_read32(m, 0x8009C2D0u);
    rrj_write32(m, context + 0x450u, buffer);
    rrj_write32(m, context + 0x454u, sectors);
    return sub_F_80061060(m, 6u, position, 0u, call);
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80061584 sub_F_80061584 */
uint32_t sub_F_80061584(RRJMemory *m, uint32_t event, RRJVideoPhaseCall call)
{
    uint32_t context, result, index, command, parameter;
    if ((event & 0xffu) != 2u)
    {
        uint32_t errors = rrj_read32(m, 0x800810C4u);
        context = rrj_read32(m, 0x8009C2D0u);
        rrj_write32(m, 0x800810C4u, errors + 1u);
        result = rrj_read32(m, context + 0x424u) + 1u;
        rrj_write32(m, context + 0x424u, result);
        return result;
    }
    context = rrj_read32(m, 0x8009C2D0u);
    if (rrj_read32(m, context + 0x424u) >= 2u)
        return 0u;
    rrj_write32(m, context + 0x424u, 0u);
    result = rrj_read32(m, context + 0x41cu);
    if (!result)
        return result;
    result = rrj_read32(m, context + 0x410u);
    if (!result)
        return result;
    if (rrj_read32(m, context + 0x418u))
    {
        rrj_write32(m, context + 0x418u, rrj_read32(m, context + 0x418u) - 1u);
        if (!rrj_read32(m, context + 0x418u))
        {
            uint32_t buffer = rrj_read32(m, context + 0x450u);
            uint32_t sectors = rrj_read32(m, context + 0x454u);
            rrj_write32(m, context + 4u, buffer);
            rrj_write32(m, context, sectors);
            rrj_write32(m, context + 0x414u, 2u);
        }
    }
    context = rrj_read32(m, 0x8009C2D0u);
    rrj_write32(m, context + 0x410u, rrj_read32(m, context + 0x410u) - 1u);
    rrj_write32(m, context + 0x408u, rrj_read32(m, context + 0x408u) + 3u);
    rrj_write32(m, context + 0x408u, rrj_read32(m, context + 0x408u) % 48u);
    result = rrj_read32(m, context + 0x410u);
    if (!result)
        return result;
    index = rrj_read32(m, context + 0x408u);
    command = rrj_read32(m, context + 8u + index * 4u);
    index = rrj_read32(m, context + 0x408u);
    parameter = rrj_read32(m, context + 8u + (index + 1u) * 4u);
    index = rrj_read32(m, context + 0x408u);
    (void)rrj_read32(m, context + 8u + (index + 2u) * 4u);
    do
    {
        result = stream_call(m, call, 0x8004594Cu, command & 0xffu, parameter, 0u);
    } while (!result);
    return result;
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80060F44 sub_F_80060F44 */
uint32_t sub_F_80060F44(RRJMemory *m, uint32_t command, uint32_t parameter, uint32_t result, RRJVideoPhaseCall call)
{
    uint32_t context, index, pending;
    (void)stream_call(m, call, 0x80043DD4u, 0u, 0u, 0u);
    context = rrj_read32(m, 0x8009C2D0u);
    index = rrj_read32(m, context + 0x40cu);
    rrj_write32(m, context + 8u + index * 4u, command & 0xffu);
    rrj_write32(m, context + 0x40cu, index + 1u);
    index = rrj_read32(m, context + 0x40cu);
    rrj_write32(m, context + 8u + index * 4u, parameter);
    rrj_write32(m, context + 0x40cu, index + 1u);
    index = rrj_read32(m, context + 0x40cu);
    rrj_write32(m, context + 8u + index * 4u, result);
    rrj_write32(m, context + 0x40cu, index + 1u);
    rrj_write32(m, context + 0x40cu, rrj_read32(m, context + 0x40cu) % 48u);
    pending = rrj_read32(m, context + 0x410u) + 1u;
    rrj_write32(m, context + 0x410u, pending);
    pending = rrj_read32(m, context + 0x410u);
    (void)stream_call(m, call, 0x80043DF4u, 0u, 0u, 0u);
    if (pending == 1u)
    {
        while (!stream_call(m, call, 0x80045810u, command & 0xffu, parameter, result))
        {
        }
    }
    return 1u;
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80061148 sub_F_80061148 */
uint32_t sub_F_80061148(RRJMemory *m, uint32_t buffer, uint32_t sectors, uint32_t position, RRJVideoPhaseCall call)
{
    uint32_t context = rrj_read32(m, 0x8009C2D0u);
    rrj_write32(m, context + 0x450u, buffer);
    rrj_write32(m, context + 0x454u, sectors);
    return sub_F_80060F44(m, 6u, position, 0u, call);
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x800611B8 sub_F_800611B8 */
uint32_t sub_F_800611B8(RRJMemory *m, uint32_t source_position, uint32_t destination_position, uint32_t sectors, RRJVideoPhaseCall call)
{
    uint32_t sector = stream_call(m, call, 0x80046768u, source_position, 0u, 0u);
    return stream_call(m, call, 0x80046664u, sector + sectors, destination_position, 0u);
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80061818 sub_F_80061818 */
uint32_t sub_F_80061818(RRJMemory *m)
{
    uint32_t context = rrj_read32(m, 0x8009C2D0u);
    uint32_t result = rrj_read32(m, context + 0x43cu);
    if (result)
        return result;
    if (rrj_read32(m, context + 0x438u))
    {
        uint32_t current = rrj_read32(m, context + 0x430u);
        uint32_t next = rrj_read32(m, 0x8009C2D4u), descriptor;
        if (current == next)
            next += 56u;
        rrj_write32(m, context + 0x430u, next);
        context = rrj_read32(m, 0x8009C2D0u);
        (void)sub_8001E0B4(m, rrj_read32(m, context + 0x430u), rrj_read32(m, context + 0x42cu), 56u);
        context = rrj_read32(m, 0x8009C2D0u);
        descriptor = rrj_read32(m, context + 0x430u);
        rrj_write32(m, descriptor, (rrj_read32(m, descriptor + 16u) + 2047u) >> 11);
        rrj_write32(m, rrj_read32(m, context + 0x430u) + 4u, 0u);
        rrj_write32(m, rrj_read32(m, context + 0x430u) + 8u, 0u);
        rrj_write32(m, rrj_read32(m, context + 0x430u) + 40u, 0u);
        result = rrj_read32(m, context + 0x434u);
        descriptor = rrj_read32(m, context + 0x430u);
        rrj_write32(m, context + 0x434u, result + 1u);
        rrj_write32(m, descriptor + 52u, 0u);
        descriptor = rrj_read32(m, context + 0x430u);
        rrj_write32(m, descriptor + 44u, rrj_read32(m, 0x8009C2D8u));
        descriptor = rrj_read32(m, context + 0x430u);
        rrj_write32(m, descriptor + 48u, rrj_read32(m, 0x8009C2DCu));
        rrj_write32(m, context + 0x43cu, 0u);
    }
    else
        rrj_write32(m, context + 0x43cu, 2u);
    context = rrj_read32(m, 0x8009C2D0u);
    return rrj_read32(m, context + 0x43cu);
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
uint32_t sub_F_800611F4(RRJMemory *m, uint32_t event, RRJVideoPhaseCall call, RRJStreamHeaderRead read_header)
{
    uint32_t context, descriptor, actual, expected, sectors;
    uint8_t header[12];
    if ((event & 255u) != 1u)
    {
        uint32_t errors = rrj_read32(m, 0x800810C0u);
        context = rrj_read32(m, 0x8009C2D0u);
        rrj_write32(m, 0x800810C0u, errors + 1u);
        errors = rrj_read32(m, context + 0x424u) + 1u;
        rrj_write32(m, context + 0x424u, errors);
        return errors;
    }
    context = rrj_read32(m, 0x8009C2D0u);
    if (rrj_read32(m, context + 0x424u) >= 2u)
        return 0u;
    rrj_write32(m, context + 0x424u, 0u);
    if (rrj_read32(m, context + 0x414u) != 2u)
        return 2u;
    if (!rrj_read32(m, context))
        return 0x80080000u;
    if (rrj_read32(m, 0x800810D4u))
    {
        uint32_t address = rrj_read32(m, 0x800810D8u);
        if (rrj_read32(m, address) & 0x01000000u)
        {
            uint32_t polls = 1u;
            while (polls != 65537u)
            {
                uint32_t busy = rrj_read32(m, address) & 0x01000000u;
                ++polls;
                if (!busy)
                    break;
            }
        }
    }
    context = rrj_read32(m, 0x8009C2D0u);
    actual = rrj_read32(m, context + 4u) & 3u;
    if (actual)
    {
        rrj_write32(m, context + 0x414u, 0u);
        return actual;
    }
    if (!read_header)
        abort();
    read_header(m, header);
    (void)stream_call(m, call, 0x80045C50u, 0u, 0u, 0u);
    actual = stream_header_sector(header);
    context = rrj_read32(m, 0x8009C2D0u);
    descriptor = rrj_read32(m, context + 0x430u);
    (void)sub_F_800611B8(m, descriptor + 12u, descriptor + 36u, rrj_read32(m, descriptor + 8u), call);
    context = rrj_read32(m, 0x8009C2D0u);
    expected = stream_call(m, call, 0x80046768u, rrj_read32(m, context + 0x430u) + 36u, 0u, 0u);
    if (actual != expected)
    {
        uint32_t errors = rrj_read32(m, 0x800810C8u);
        context = rrj_read32(m, 0x8009C2D0u);
        rrj_write32(m, 0x800810C8u, errors + 1u);
    }
    else
        context = rrj_read32(m, 0x8009C2D0u);
    (void)stream_call(m, call, 0x80045BECu, rrj_read32(m, context + 4u), 512u, 0u);
    (void)stream_call(m, call, 0x80045C50u, 0u, 0u, 0u);
    context = rrj_read32(m, 0x8009C2D0u);
    (void)stream_call(m, call, 0x80045BECu, context + 0x458u, 70u, 0u);
    (void)stream_call(m, call, 0x80045C50u, 0u, 0u, 0u);
    if (actual != expected)
    {
        context = rrj_read32(m, 0x8009C2D0u);
        descriptor = rrj_read32(m, context + 0x430u);
        (void)sub_F_800611B8(m, descriptor + 12u, descriptor + 36u, rrj_read32(m, descriptor + 8u), call);
        context = rrj_read32(m, 0x8009C2D0u);
        rrj_write32(m, context + 0x428u, rrj_read32(m, context + 0x428u) + 1u);
        rrj_write32(m, context + 0x418u, 2u);
        rrj_write32(m, context + 0x414u, 1u);
        rrj_write32(m, 0x800810CCu, rrj_read32(m, 0x800810CCu) + 1u);
        descriptor = rrj_read32(m, context + 0x430u);
        (void)sub_F_80061060(m, 21u, descriptor + 36u, 0u, call);
        context = rrj_read32(m, 0x8009C2D0u);
        return sub_F_80061180(m, rrj_read32(m, context + 4u), rrj_read32(m, context), rrj_read32(m, context + 0x430u) + 36u, call);
    }
    context = rrj_read32(m, 0x8009C2D0u);
    rrj_write32(m, context + 4u, rrj_read32(m, context + 4u) + 2048u);
    sectors = rrj_read32(m, context);
    descriptor = rrj_read32(m, context + 0x430u);
    rrj_write32(m, context, sectors - 1u);
    rrj_write32(m, descriptor + 8u, rrj_read32(m, descriptor + 8u) + 1u);
    rrj_write32(m, context + 0x448u, rrj_read32(m, context + 0x448u) + 1u);
    sectors = rrj_read32(m, context);
    if (sectors)
        return sectors;
    descriptor = rrj_read32(m, context + 0x430u);
    sectors = rrj_read32(m, context);
    {
        uint32_t used = rrj_read32(m, descriptor + 8u);
        uint32_t produced = rrj_read32(m, context + 0x448u);
        uint32_t capacity = rrj_read32(m, 0x8009C2E0u);
        uint32_t remainder = stream_remainder(produced + sectors, capacity);
        uint32_t remaining = rrj_read32(m, descriptor) - (used + sectors);
        uint32_t available = rrj_read32(m, context + 0x440u) + capacity - (produced + sectors);
        sectors = stream_min(stream_min(available, capacity - remainder), remaining);
    }
    context = rrj_read32(m, 0x8009C2D0u);
    if (sectors)
    {
        uint32_t capacity = rrj_read32(m, 0x8009C2E0u);
        uint32_t buffer;
        descriptor = rrj_read32(m, context + 0x430u);
        buffer = rrj_read32(m, descriptor + 0x30u);
        if (rrj_read32(m, context + 4u) == buffer + (capacity << 11))
            rrj_write32(m, context + 4u, buffer);
        context = rrj_read32(m, 0x8009C2D0u);
        rrj_write32(m, context, sectors);
    }
    else
    {
        rrj_write32(m, context, 0u);
        rrj_write32(m, context + 0x414u, 0u);
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
uint32_t sub_F_800609E4(RRJMemory *m, RRJVideoPhaseCall call, RRJStreamBytes draw)
{
    uint32_t context = rrj_read32(m, 0x800810BCu);
    uint32_t written = rrj_read32(m, context + 4u);
    uint32_t read = rrj_read32(m, context);
    uint32_t available = 15360u - (read - (written - 15360u));
    uint32_t adjusted = rrj_s32(available) < 0 ? available + 63u : available;
    uint32_t width = (uint32_t)(rrj_s32(adjusted) >> 6);
    stream_bar(m, 24u, 212u, 240u, 255u, call, draw);
    stream_bar(m, 504u, 212u, 240u, 255u, call, draw);
    stream_bar(m, 24u, 212u, width, 0u, call, draw);
    stream_bar(m, 504u, 212u, width, 0u, call, draw);
    return stream_call(m, call, 0x800487C0u, 0u, 0u, 0u);
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80061C44 sub_F_80061C44 */
uint32_t sub_F_80061C44(RRJMemory *m, uint32_t length, RRJVideoPhaseCall call, RRJStreamBytes draw)
{
    uint32_t context = rrj_read32(m, 0x8009C2D0u);
    uint32_t descriptor = rrj_read32(m, context + 0x42cu);
    uint32_t offset = rrj_read32(m, context + 0x444u);
    uint32_t remaining = 2048u - offset;
    rrj_write32(m, descriptor + 4u, rrj_read32(m, descriptor + 4u) + length);
    if (rrj_s32(length) < rrj_s32(remaining))
        rrj_write32(m, context + 0x444u, rrj_read32(m, context + 0x444u) + length);
    else
    {
        uint32_t overflow = length - remaining;
        rrj_write32(m, context + 0x444u, overflow & 0x7ffu);
        rrj_write32(m, context + 0x440u, rrj_read32(m, context + 0x440u) + (overflow >> 11) + 1u);
    }
    sub_F_80061938(m, call, draw);
    context = rrj_read32(m, 0x8009C2D0u);
    descriptor = rrj_read32(m, context + 0x42cu);
    if (rrj_read32(m, descriptor + 4u) == rrj_read32(m, descriptor + 16u) && rrj_read32(m, context + 0x438u))
    {
        uint32_t next = rrj_read32(m, context + 0x430u);
        rrj_write32(m, context + 0x44cu, 0u);
        offset = rrj_read32(m, context + 0x444u);
        rrj_write32(m, context + 0x42cu, next);
        if (offset)
            rrj_write32(m, context + 0x440u, rrj_read32(m, context + 0x440u) + 1u);
        context = rrj_read32(m, 0x8009C2D0u);
        descriptor = rrj_read32(m, context + 0x42cu);
        rrj_write32(m, context + 0x444u, rrj_read32(m, descriptor + 0x28u));
    }
    if (!rrj_read32(m, 0x800810DCu))
        return 0u;
    {
        uint32_t bytes = rrj_read32(m, 0x8009C2E0u) << 11;
        int64_t product = (int64_t)rrj_s32(bytes) * (int64_t)rrj_s32(0x9c09c09du);
        uint32_t high = (uint32_t)((uint64_t)product >> 32);
        uint32_t scale = (uint32_t)(rrj_s32(high + bytes) >> 8) - (uint32_t)(rrj_s32(bytes) >> 31);
        uint32_t width = stream_divide(bytes, scale), available;
        context = rrj_read32(m, 0x8009C2D0u);
        available = (rrj_read32(m, context + 0x448u) << 11) - ((rrj_read32(m, context + 0x440u) << 11) + rrj_read32(m, context + 0x444u));
        stream_bar(m, 24u, 200u, width, 255u, call, draw);
        stream_bar(m, 504u, 200u, width, 255u, call, draw);
        width = stream_divide(available, scale);
        stream_bar(m, 24u, 200u, width, 0u, call, draw);
        stream_bar(m, 504u, 200u, width, 0u, call, draw);
    }
    (void)sub_F_800609E4(m, call, draw);
    return stream_call(m, call, 0x800487C0u, 0u, 0u, 0u);
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80061938 sub_F_80061938 */
void sub_F_80061938(RRJMemory *m, RRJVideoPhaseCall call, RRJStreamBytes bytes_call)
{
    uint32_t context = rrj_read32(m, 0x8009C2D0u), descriptor, sectors;
    if (rrj_read32(m, context + 0x424u) >= 2u)
    {
        uint32_t callback = rrj_read32(m, context + 0x570u);
        uint8_t mode = 0xa0u;
        if (callback)
            (void)stream_call(m, call, callback, 0u, 0u, 0u);
        (void)stream_call(m, call, 0x80043DD4u, 0u, 0u, 0u);
        context = rrj_read32(m, 0x8009C2D0u);
        rrj_write32(m, context + 0x410u, 0u);
        rrj_write32(m, context + 0x408u, 0u);
        rrj_write32(m, context + 0x40cu, 0u);
        rrj_write32(m, context + 0x41cu, 0u);
        (void)stream_call(m, call, 0x800457E8u, 0u, 0u, 0u);
        (void)stream_call(m, call, 0x800457FCu, 0u, 0u, 0u);
        (void)stream_call(m, call, 0x80043DF4u, 0u, 0u, 0u);
        (void)stream_call(m, call, 0x80045810u, 8u, 0u, 0u);
        while (stream_call(m, call, 0x80060EC4u, 0u, 0u, 0u))
        {
        }
        if (!bytes_call)
            abort();
        (void)bytes_call(m, 0x80045A80u, 14u, &mode, 1u);
        (void)stream_call(m, call, 0x80043DD4u, 0u, 0u, 0u);
        (void)stream_call(m, call, 0x800457E8u, 0x80061584u, 0u, 0u);
        (void)stream_call(m, call, 0x800457FCu, 0x800611F4u, 0u, 0u);
        context = rrj_read32(m, 0x8009C2D0u);
        rrj_write32(m, context + 0x41cu, 1u);
        rrj_write32(m, context + 0x414u, 1u);
        rrj_write32(m, context + 0x424u, 0u);
        (void)stream_call(m, call, 0x80043DF4u, 0u, 0u, 0u);
        context = rrj_read32(m, 0x8009C2D0u);
        if (rrj_read32(m, context + 0x414u) && rrj_read32(m, context))
        {
            descriptor = rrj_read32(m, context + 0x430u);
            (void)stream_call(m, call, 0x800611B8u, descriptor + 12u, descriptor + 36u, rrj_read32(m, descriptor + 8u));
            context = rrj_read32(m, 0x8009C2D0u);
            rrj_write32(m, context + 0x418u, 2u);
            rrj_write32(m, 0x800810CCu, rrj_read32(m, 0x800810CCu) + 1u);
            descriptor = rrj_read32(m, context + 0x430u);
            if (!stream_call(m, call, 0x80060F44u, 21u, descriptor + 36u, 0u))
            {
                context = rrj_read32(m, 0x8009C2D0u);
                rrj_write32(m, context + 0x43cu, 1u);
                return;
            }
            context = rrj_read32(m, 0x8009C2D0u);
            sectors = rrj_read32(m, context);
            (void)stream_call(m, call, 0x80061148u, rrj_read32(m, context + 4u), sectors, rrj_read32(m, context + 0x430u) + 36u);
        }
        else
        {
            context = rrj_read32(m, 0x8009C2D0u);
            rrj_write32(m, context + 0x414u, 0u);
        }
    }
    context = rrj_read32(m, 0x8009C2D0u);
    if (rrj_read32(m, context + 0x414u))
        return;
    descriptor = rrj_read32(m, context + 0x430u);
    sectors = rrj_read32(m, descriptor) - rrj_read32(m, descriptor + 8u);
    if (!sectors)
        (void)stream_call(m, call, 0x80061818u, 0u, 0u, 0u);
    context = rrj_read32(m, 0x8009C2D0u);
    {
        uint32_t capacity = rrj_read32(m, 0x8009C2E0u);
        uint32_t consumed = rrj_read32(m, context + 0x440u);
        uint32_t produced = rrj_read32(m, context + 0x448u);
        uint32_t remainder = stream_remainder(rrj_read32(m, context + 0x448u), capacity);
        sectors = stream_min(stream_min(consumed + capacity - produced, capacity - remainder), sectors);
    }
    if (!sectors)
        return;
    context = rrj_read32(m, 0x8009C2D0u);
    descriptor = rrj_read32(m, context + 0x430u);
    (void)stream_call(m, call, 0x800611B8u, descriptor + 12u, descriptor + 36u, rrj_read32(m, descriptor + 8u));
    context = rrj_read32(m, 0x8009C2D0u);
    rrj_write32(m, context + 0x44cu, rrj_read32(m, context + 0x448u) + sectors);
    rrj_write32(m, context + 0x418u, 2u);
    rrj_write32(m, 0x800810CCu, rrj_read32(m, 0x800810CCu) + 1u);
    descriptor = rrj_read32(m, context + 0x430u);
    if (!stream_call(m, call, 0x80060F44u, 21u, descriptor + 36u, 0u))
    {
        context = rrj_read32(m, 0x8009C2D0u);
        rrj_write32(m, context + 0x43cu, 1u);
        return;
    }
    context = rrj_read32(m, 0x8009C2D0u);
    descriptor = rrj_read32(m, context + 0x430u);
    {
        uint32_t produced = rrj_read32(m, context + 0x448u);
        uint32_t displacement = rrj_read32(m, descriptor + 0x34u);
        uint32_t capacity = rrj_read32(m, 0x8009C2E0u);
        uint32_t position = stream_remainder(produced + displacement, capacity);
        uint32_t buffer = rrj_read32(m, descriptor + 0x30u);
        rrj_write32(m, context + 0x414u, 1u);
        (void)stream_call(m, call, 0x80061148u, buffer + (position << 11), sectors, descriptor + 36u);
    }
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x800602D4 sub_F_800602D4 */
uint32_t sub_F_800602D4(RRJMemory *m)
{
    rrj_write32(m, rrj_read32(m, 0x800810BCu) + 0x7b20u, 1u);
    return 1u;
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80061F04 sub_F_80061F04 */
uint32_t sub_F_80061F04(RRJMemory *m, uint32_t *length, RRJVideoPhaseCall call)
{
    uint32_t context = rrj_read32(m, 0x8009C2D0u);
    uint32_t descriptor = rrj_read32(m, context + 0x42cu);
    uint32_t remaining = rrj_read32(m, descriptor + 16u) - rrj_read32(m, descriptor + 4u);
    uint32_t polls = 0u, retries = 0u;
    if (rrj_s32(remaining) < rrj_s32(*length))
        *length = remaining;
    for (;;)
    {
        uint32_t available;
        context = rrj_read32(m, 0x8009C2D0u);
        available = (rrj_read32(m, context + 0x448u) << 11) - ((rrj_read32(m, context + 0x440u) << 11) + rrj_read32(m, context + 0x444u));
        if (rrj_s32(available) < 0)
            available = 0u;
        if (rrj_s32(available) >= rrj_s32(*length))
            break;
        if (rrj_read32(m, context + 0x43cu))
            return 0u;
        ++polls;
        if (polls > 0x3d0900u)
            rrj_write32(m, context + 0x424u, rrj_read32(m, context + 0x424u) + 2u);
        context = rrj_read32(m, 0x8009C2D0u);
        if (rrj_read32(m, context + 0x414u) && rrj_read32(m, context + 0x424u) < 2u)
            continue;
        if (++retries >= 3u)
            return 0u;
        polls = 0u;
        /* TODO Bind the complete CD refill owner */
        (void)stream_call(m, call, 0x80061938u, rrj_read32(m, context + 0x444u), 0u, 0u);
    }
    {
        uint32_t sector, offset, capacity, position, end, pointer, contiguous;
        context = rrj_read32(m, 0x8009C2D0u);
        sector = rrj_read32(m, context + 0x440u);
        offset = rrj_read32(m, context + 0x444u);
        descriptor = rrj_read32(m, context + 0x42cu);
        position = rrj_read32(m, context + 0x440u) + rrj_read32(m, descriptor + 0x34u);
        capacity = rrj_read32(m, 0x8009C2E0u);
        /* MIPS DIVU leaves the dividend in HI when the divisor is zero */
        position = capacity ? position % capacity : position;
        end = capacity ? sector % capacity : sector;
        pointer = rrj_read32(m, descriptor + 0x30u) + (position << 11) + rrj_read32(m, context + 0x444u);
        contiguous = ((capacity - 1u - end) << 11) + 2048u - offset;
        if (contiguous < *length)
        {
            (void)stream_call(m, call, 0x8001E0B4u, pointer - (capacity << 11), pointer, contiguous);
            pointer -= rrj_read32(m, 0x8009C2E0u) << 11;
        }
        return pointer;
    }
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x8005F484 sub_F_8005F484 */
uint32_t sub_F_8005F484(RRJMemory *m, uint32_t x, uint32_t y, uint32_t channel, uint32_t mode, uint32_t frame, RRJStreamRead read, RRJVideoPhaseCall call)
{
    if (!read)
        abort();
    for (;;)
    {
        uint32_t length = 8u, header = read(m, &length), tag, data;
        if (!header || !length)
            return 0u;
        tag = stream_be32(m, header);
        length = stream_be32(m, header + 4u);
        if (length >= 0x4400u)
        {
            (void)stream_call(m, call, 0x80061C44u, 4u, 0u, 0u);
            continue;
        }
        data = read(m, &length);
        if (!data || !length)
            return 0u;
        if (tag == 0x4d444332u || tag == 0x4d444543u)
        {
            uint32_t count = rrj_read32(m, 0x8009C2F4u);
            if (rrj_s32(count) < 6)
            {
                uint32_t item = 0x8009C3D0u + 36u * count;
                rrj_write32(m, item, data);
                rrj_write32(m, item + 8u, frame ? 2u : 0u);
                rrj_write32(m, item + 12u, mode);
                rrj_write32(m, item + 28u, x);
                rrj_write32(m, item + 32u, y);
                rrj_write32(m, 0x8009C2F4u, count + 1u);
                rrj_write32(m, item + 16u, length);
            }
            if (frame == 1u && channel)
                (void)stream_call(m, call, 0x8006013Cu, 0u, 0u, 0u);
            (void)sub_F_800602D4(m);
            return length;
        }
        if (tag == 0x564c4330u)
            (void)stream_call(m, call, 0x8002026Cu, data + 8u, 0u, 0u);
        else if ((tag >= 0x41643130u && tag <= 0x41643131u) || (tag >= 0x61643130u && tag <= 0x61643131u) || (tag >= 0x61643230u && tag <= 0x61643231u))
        {
            if (channel)
                (void)stream_call(m, call, 0x80060304u, data, 0u, 0u);
        }
        else if (tag >= 0x61753030u && tag <= 0x61753031u)
        {
            if (channel)
                (void)stream_call(m, call, 0x80060780u, data, 0u, 0u);
        }
        else
            length = 4u;
        (void)stream_call(m, call, 0x80061C44u, length, 0u, 0u);
    }
}

static uint32_t h(RRJMemory *m, uint32_t a)
{
    return rrj_u16(rrj_at(m, a, 2));
}

static uint32_t sh(uint32_t v)
{
    return v & 0x8000 ? v | 0xffff0000 : v;
}

static void half(RRJMemory *m, uint32_t a, uint32_t v)
{
    rrj_put16(rrj_at(m, a, 2), (uint16_t)v);
}

uint32_t sub_F_8006FEF4(RRJMemory *m, uint32_t desc, RRJVideoPhaseCall cb, RRJVideoPhaseOpen open, RRJSDKCall stop)
{
    uint32_t args[9] = {0}, file, flags, channel;
    if (!cb)
        abort();
    if (cb(m, 0x80022A78, args))
        return 0;
    if (h(m, 0x8009C5DA) & 1)
        return 0;
    if (!open)
        abort();
    file = open(m, 0x8005BF84, rrj_read32(m, 0x8008973C + 4 * rrj_read32(m, desc)));
    rrj_write32(m, 0x8009C684, file);
    if (rrj_s32(file) < 0)
        return 1;
    rrj_write32(m, desc + 16, 0);
    flags = h(m, 0x8009C5DA);
    args[0] = rrj_read32(m, 0x8009C684);
    half(m, 0x8009C5DA, flags | 1);
    channel = h(m, desc + 8);
    half(m, 0x8009C688, channel);
    args[3] = sh(h(m, desc + 12));
    args[4] = sh(h(m, desc + 14));
    args[5] = 2048;
    args[6] = 64;
    args[7] = 1024;
    args[8] = h(m, desc + 10);
    args[1] = rrj_read32(m, desc + 4);
    args[2] = sh(channel);
    if (!cb(m, 0x8005F36C, args))
    {
        (void)sub_F_8006FE6C(m, stop);
        return 0;
    }
    rrj_write32(m, desc + 16, rrj_read32(m, desc + 16) + 1);
    return 1;
}

uint32_t sub_F_80070018(RRJMemory *m, uint32_t desc, RRJVideoPhaseCall cb, RRJVideoPhaseOpen open, RRJSDKCall stop)
{
    uint32_t flags = h(m, 0x8009C5DA), args[9] = {0}, result = 1;
    if (flags & 4)
        return sub_F_8006FE6C(m, stop);
    if (!(flags & 1))
    {
        if (flags & 2)
            return sub_F_8006FEF4(m, desc, cb, open, stop);
        half(m, 0x8009C5DA, flags | 2);
        return 0;
    }
    args[4] = rrj_read32(m, desc + 16);
    args[0] = sh(h(m, desc + 12));
    args[1] = sh(h(m, desc + 14));
    args[2] = sh(h(m, 0x8009C688));
    args[3] = rrj_read32(m, desc + 4);
    if (!cb)
        abort();
    if (!cb(m, 0x8005F484, args))
    {
        half(m, 0x8009C5DA, (h(m, 0x8009C5DA) & 0xfffe) | 2);
        result = 0;
    }
    rrj_write32(m, desc + 16, rrj_read32(m, desc + 16) + 1);
    return result;
}
