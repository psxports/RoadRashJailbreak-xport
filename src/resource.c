#include "native_game_api.h"
#include "psx.h"
/* WIP: IDA draft checked against F 80065768/8007A400 MIPS.
 * These functions enqueue work; queue execution is a separate dependency. */
#include "resource.h"
#include "resource_lookup.h"
#include "packet.h"
#include <stdlib.h>

static uint32_t half(RRJMemory *m, uint32_t a)
{
    return rrj_u16(rrj_at(a, 2));
}

static int32_t signed_half(RRJMemory *m, uint32_t a)
{
    uint32_t v = half(m, a);
    return v < 32768 ? (int32_t)v : (int32_t)v - 65536;
}

static void store_half(RRJMemory *m, uint32_t a, uint32_t v)
{
    rrj_put16(rrj_at(a, 2), v);
}

void sub_F_800623F8(void)
{
    FUNCTION_MARKER(0x800623F8, "RASHCDF.BIN");
    rrj_write32(0x800810ECu, 1u);
    rrj_write32(0x800810F0u, 3u);
    rrj_write32(0x800810D4u, 1u);
}

void sub_F_80062420(void)
{
    FUNCTION_MARKER(0x80062420, "RASHCDF.BIN");
    rrj_write32(0x800810ECu, 2u);
    rrj_write32(0x800810F0u, 2u);
}

static uint32_t queue_call(RRJMemory *m, RRJQueueCall call, uint32_t target, uint32_t a0, uint32_t a1)
{
    if (!call)
        abort();
    return call(m, target, a0, a1);
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x800624EC sub_F_800624EC */
uint32_t sub_F_800624EC(const uint32_t arguments[6], RRJQueueCall call)
{
    FUNCTION_MARKER(0x800624ECu, "RASHCDF.BIN");
    uint32_t context, previous, enabled = arguments[5];
    if (!arguments[0] || !arguments[1] || !arguments[2] || !arguments[3] || !arguments[4])
        return 0u;
    (void)queue_call(rrj_host_context(), call, 0x800448B4u, arguments[0], 0x134u);
    rrj_write32(0x800810E8u, arguments[0]);
    (void)queue_call(rrj_host_context(), call, 0x800487C0u, 0u, 0u);
    (void)queue_call(rrj_host_context(), call, 0x8004D9A8u, 0u, 0u);
    (void)queue_call(rrj_host_context(), call, 0x8004D9E4u, 0u, 0u);
    context = rrj_read32(0x800810E8u);
    rrj_write32(context + 0x118u, 0u);
    rrj_write32(context + 40u, arguments[1]);
    rrj_write32(context + 44u, arguments[2]);
    rrj_write32(context + 48u, arguments[3]);
    rrj_write32(context + 36u, arguments[4]);
    (void)queue_call(rrj_host_context(), call, 0x8004D7B4u, 0u, 0u);
    (void)sub_80043DD4();
    context = rrj_read32(0x800810E8u);
    rrj_write32(context + 0x130u, enabled);
    previous = queue_call(rrj_host_context(), call, 0x8004DA44u, enabled ? 0x80062674u : 0u, 0u);
    context = rrj_read32(0x800810E8u);
    rrj_write32(context + 0x124u, previous);
    previous = queue_call(rrj_host_context(), call, 0x8004DA20u, 0u, 0u);
    context = rrj_read32(0x800810E8u);
    rrj_write32(context + 0x128u, previous);
    previous = queue_call(rrj_host_context(), call, 0x800486C8u, 0u, 0u);
    context = rrj_read32(0x800810E8u);
    if (enabled)
    {
        rrj_write32(context + 0x12Cu, previous);
        if (rrj_read32(0x800810ECu) == 1u)
            rrj_write32(0x800810D4u, 1u);
    }
    else
    {
        rrj_write32(0x800810D4u, 0u);
        rrj_write32(context + 0x12Cu, previous);
    }
    (void)sub_80043DF4();
    return 1u;
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80062440 sub_F_80062440 */
uint32_t sub_F_80062440(RRJQueueCall call)
{
    FUNCTION_MARKER(0x80062440u, "RASHCDF.BIN");
    uint32_t context = rrj_read32(0x800810E8u), previous, result;
    if (!context)
        return 0u;
    (void)queue_call(rrj_host_context(), call, 0x8004D9A8u, 0u, 0u);
    (void)queue_call(rrj_host_context(), call, 0x8004D9E4u, 0u, 0u);
    (void)queue_call(rrj_host_context(), call, 0x800487C0u, 0u, 0u);
    (void)sub_80043DD4();
    context = rrj_read32(0x800810E8u);
    previous = rrj_read32(context + 0x124u);
    (void)queue_call(rrj_host_context(), call, 0x8004DA44u, previous, 0u);
    context = rrj_read32(0x800810E8u);
    previous = rrj_read32(context + 0x128u);
    (void)queue_call(rrj_host_context(), call, 0x8004DA20u, previous, 0u);
    context = rrj_read32(0x800810E8u);
    previous = rrj_read32(context + 0x12Cu);
    (void)queue_call(rrj_host_context(), call, 0x800486C8u, previous, 0u);
    context = rrj_read32(0x800810E8u);
    rrj_write32(context + 0x124u, 0u);
    rrj_write32(context + 0x128u, 0u);
    rrj_write32(context + 0x12Cu, 0u);
    (void)sub_80043DF4();
    result = queue_call(rrj_host_context(), call, 0x8004D7B4u, 1u, 0u);
    rrj_write32(0x800810E8u, 0u);
    return result;
}

static uint32_t queue_big_word(RRJMemory *m, uint32_t address)
{
    uint32_t word = rrj_read32(address);
    return (word >> 24) | ((word >> 8) & 0xFF00u) | ((word & 0xFF00u) << 8) | (word << 24);
}

static uint32_t queue_big_half(RRJMemory *m, uint32_t address)
{
    uint32_t value = half(m, address);
    return (value >> 8) | ((value & 0xFFu) << 8);
}

static void queue_wait_decoder(RRJMemory *m)
{
    uint32_t context = rrj_read32(0x800810E8u), index = 0u;
    if (rrj_read32(context + 0x120u))
        return;
    context = rrj_read32(0x800810E8u);
    while (index <= 0x30D40u)
    {
        uint32_t ready = rrj_read32(context + 0x120u);
        ++index;
        if (ready)
            break;
    }
}

static void queue_decoder_output(RRJMemory *m, RRJQueueCall call)
{
    uint32_t context = rrj_read32(0x800810E8u);
    uint32_t width = (uint32_t)signed_half(m, context + 32u);
    uint32_t height = (uint32_t)signed_half(m, context + 34u);
    uint32_t product = width * height;
    uint32_t corrected = product + (product >> 31);
    uint32_t length = (corrected >> 1) | (corrected & 0x80000000u);
    (void)queue_call(m, call, 0x8004D988u, rrj_read32(context + 36u), length);
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80062674 sub_F_80062674 */
uint32_t sub_F_80062674(RRJQueueCall call)
{
    FUNCTION_MARKER(0x80062674u, "RASHCDF.BIN");
    uint32_t context = rrj_read32(0x800810E8u);
    uint32_t buffer = rrj_read32(context + 36u), format, x, first, second, right, result;
    (void)queue_call(rrj_host_context(), call, 0x80048A6Cu, context + 28u, buffer);
    context = rrj_read32(0x800810E8u);
    format = rrj_read32(0x800810F0u);
    x = half(rrj_host_context(), context + 28u);
    store_half(rrj_host_context(), context + 28u, x + (format << 3));
    x = (uint32_t)signed_half(rrj_host_context(), context + 28u);
    first = rrj_read32(context + 16u);
    second = rrj_read32(context + 16u);
    right = (uint32_t)signed_half(rrj_host_context(), context + (first << 3));
    right += (uint32_t)signed_half(rrj_host_context(), context + (second << 3) + 4u);
    if (rrj_s32(x) < rrj_s32(right))
    {
        uint32_t width = (uint32_t)signed_half(rrj_host_context(), context + 32u);
        uint32_t height = (uint32_t)signed_half(rrj_host_context(), context + 34u);
        uint32_t product = width * height;
        buffer = rrj_read32(context + 36u);
        product += product >> 31;
        return queue_call(rrj_host_context(), call, 0x8004D988u, buffer, (uint32_t)(rrj_s32(product) >> 1));
    }
    rrj_write32(context + 0x120u, 1u);
    first = rrj_read32(context + 16u);
    result = half(rrj_host_context(), context + (first << 3));
    store_half(rrj_host_context(), context + 28u, result);
    first = rrj_read32(context + 16u);
    result = half(rrj_host_context(), context + (first << 3) + 2u);
    store_half(rrj_host_context(), context + 30u, result);
    return result;
}

void sub_F_800627BC(uint32_t queue, uint32_t count, RRJQueueCall call)
{
    uint32_t index;
    FUNCTION_MARKER(0x800627BC, "RASHCDF.BIN");
    (void)queue_call(rrj_host_context(), call, 0x8004D7B4u, 1u, 0u);
    if (rrj_s32(count) <= 0)
        return;
    for (index = 0u; rrj_s32(index) < rrj_s32(count); ++index, queue += 36u)
    {
        uint32_t data = rrj_read32(queue), x = rrj_read32(queue + 28u);
        uint32_t mode = rrj_read32(queue + 12u), y = rrj_read32(queue + 32u);
        uint32_t payload = data + 8u, tag, size, height, width, kind, context;
        uint32_t offset_x = 0u, offset_y = 0u, format, buffer, value;
        if (mode)
            sub_F_80062420();
        else
            sub_F_800623F8();
        tag = queue_big_word(rrj_host_context(), data);
        size = queue_big_word(rrj_host_context(), data + 4u);
        while (tag != 0x4D444543u)
        {
            if (tag == 0x564C4330u && !rrj_read32(queue + 12u))
                (void)queue_call(rrj_host_context(), call, 0x8002026Cu, payload, 0u);
            data += size;
            payload = data + 8u;
            tag = queue_big_word(rrj_host_context(), data);
            size = queue_big_word(rrj_host_context(), data + 4u);
        }
        height = queue_big_half(rrj_host_context(), payload + 2u);
        width = queue_big_half(rrj_host_context(), payload);
        value = (uint32_t)sub_8004DE34(payload + 8u);
        if (rrj_read32(0x800810F4u) < value)
            rrj_write32(0x800810F4u, value);
        kind = rrj_read32(queue + 8u);
        switch (kind)
        {
            case 0u:
                context = rrj_read32(0x800810E8u);
                buffer = rrj_read32(context + rrj_read32(context + 52u) * 4u + 40u);
                (void)(uint32_t)sub_80020400(payload + 8u, buffer);
                context = rrj_read32(0x800810E8u);
                rrj_write32(context + 52u, 1u - rrj_read32(context + 52u));
                break;
            case 1u:
            case 3u:
            case 4u:
                if (kind != 4u)
                {
                    context = rrj_read32(0x8005B470u);
                    context += r_u8(context + 5u) * 112u;
                    offset_x = (uint32_t)signed_half(rrj_host_context(), context + 24u);
                    offset_y = (uint32_t)signed_half(rrj_host_context(), context + 26u);
                }
                x += offset_x;
                format = rrj_read32(0x800810F0u);
                y += offset_y;
                context = rrj_read32(0x800810E8u);
                store_half(rrj_host_context(), context, x);
                store_half(rrj_host_context(), context + 2u, y);
                store_half(rrj_host_context(), context + 6u, height);
                store_half(rrj_host_context(), context + 28u, x);
                store_half(rrj_host_context(), context + 30u, y);
                store_half(rrj_host_context(), context + 32u, format * 8u);
                store_half(rrj_host_context(), context + 34u, height);
                rrj_write32(context + 16u, 0u);
                store_half(rrj_host_context(), context + 4u, (width * format) >> 1);
                buffer = rrj_read32(context + 48u);
                (void)(uint32_t)sub_80020400(payload + 8u, buffer);
                value = rrj_read32(0x800810ECu);
                context = rrj_read32(0x800810E8u);
                rrj_write32(context + 0x120u, 0u);
                (void)queue_call(rrj_host_context(), call, 0x8004D90Cu, buffer, value);
                queue_decoder_output(rrj_host_context(), call);
                (void)queue_call(rrj_host_context(), call, 0x8004D9E4u, 0u, 0u);
                queue_wait_decoder(rrj_host_context());
                break;
            case 2u:
                context = rrj_read32(0x8005B470u);
                value = rrj_read32(0x800810ECu);
                context += r_u8(context + 5u) * 112u;
                offset_x = (uint32_t)signed_half(rrj_host_context(), context + 24u);
                offset_y = (uint32_t)signed_half(rrj_host_context(), context + 26u);
                context = rrj_read32(0x800810E8u);
                format = rrj_read32(0x800810F0u);
                x += offset_x;
                y += offset_y;
                buffer = rrj_read32(context + 52u);
                store_half(rrj_host_context(), context + 6u, height);
                store_half(rrj_host_context(), context, x);
                store_half(rrj_host_context(), context + 2u, y);
                store_half(rrj_host_context(), context + 28u, x);
                store_half(rrj_host_context(), context + 30u, y);
                store_half(rrj_host_context(), context + 32u, format * 8u);
                store_half(rrj_host_context(), context + 34u, height);
                rrj_write32(context + 16u, 0u);
                rrj_write32(context + 0x120u, 0u);
                store_half(rrj_host_context(), context + 4u, (width * format) >> 1);
                buffer = rrj_read32(context + (1u - buffer) * 4u + 40u);
                (void)queue_call(rrj_host_context(), call, 0x8004D90Cu, buffer, value);
                queue_decoder_output(rrj_host_context(), call);
                context = rrj_read32(0x800810E8u);
                buffer = rrj_read32(context + rrj_read32(context + 52u) * 4u + 40u);
                (void)(uint32_t)sub_80020400(payload + 8u, buffer);
                (void)queue_call(rrj_host_context(), call, 0x8004D9E4u, 0u, 0u);
                queue_wait_decoder(rrj_host_context());
                context = rrj_read32(0x800810E8u);
                rrj_write32(context + 52u, 1u - rrj_read32(context + 52u));
                break;
            default:
                break;
        }
        if (kind < 5u)
            kind = rrj_read32(queue + 8u);
        if (kind != 3u)
        {
            value = rrj_read32(queue + 16u);
            if (value)
                (void)queue_call(rrj_host_context(), call, 0x80061C44u, value, 0u);
        }
        rrj_write32(queue + 4u, data + size);
    }
}

void sub_F_80062774(uint32_t queue, uint32_t count, RRJQueueCall call)
{
    uint32_t context;
    FUNCTION_MARKER(0x80062774, "RASHCDF.BIN");
    context = rrj_read32(0x800810E8u);
    if (rrj_read32(context + 0x130u))
        sub_F_800627BC(queue, count, call);
    /* The alternate original branch is the empty nullsub_8 */
}

uint32_t sub_F_80078BD8(uint32_t list, uint32_t wait, RRJQueueCall call)
{
    uint32_t result = 1u, id;
    FUNCTION_MARKER(0x80078BD8, "RASHCDF.BIN");
    id = (uint32_t)signed_half(rrj_host_context(), list);
    while (id != UINT32_MAX)
    {
        if (!result)
            return result;
        if (!queue_call(rrj_host_context(), call, 0x800782B0u, id, wait))
            result = 0u;
        list += 2u;
        id = (uint32_t)signed_half(rrj_host_context(), list);
    }
    return result;
}

uint32_t sub_F_80078C68(uint32_t id, uint32_t force, RRJQueueCall call)
{
    uint32_t descriptor, value, flags, data, kind, subtype, slot;
    FUNCTION_MARKER(0x80078C68, "RASHCDF.BIN");
    id = (id & 0xFFFFu) | ((id & 0x8000u) ? 0xFFFF0000u : 0u);
    if (rrj_s32(id) >= 37)
        return 1u;
    descriptor = 0x800897E4u + id * 32u;
    if (!force && (half(rrj_host_context(), descriptor) & 0x1000u))
        return 1u;
    (void)queue_call(rrj_host_context(), call, 0x80022A78u, 2u, 0u);
    value = rrj_read32(descriptor + 24u);
    rrj_write32(0x8005ACACu, 0u);
    if (value != UINT32_MAX)
    {
        (void)queue_call(rrj_host_context(), call, 0x8001460Cu, value, 0u);
        rrj_write32(descriptor + 24u, UINT32_MAX);
    }
    flags = half(rrj_host_context(), descriptor);
    data = rrj_read32(descriptor + 4u);
    store_half(rrj_host_context(), descriptor, flags & 0xFFF3u);
    if (data)
    {
        if (!(flags & 1u))
            (void)(uint32_t)sub_800144B8(data);
        flags = half(rrj_host_context(), descriptor);
        rrj_write32(descriptor + 4u, 0u);
        store_half(rrj_host_context(), descriptor, flags & 0xFFFEu);
    }
    kind = r_u8(descriptor + 2u);
    switch (kind)
    {
        case 3u:
            subtype = r_u8(descriptor + 3u);
            if (subtype == 1u || subtype == 2u || subtype == 3u)
            {
                slot = subtype == 1u ? 0x8009C5B8u : subtype == 2u ? 0x8009C5C0u : 0x8009C5BCu;
                value = rrj_read32(slot);
                rrj_write32(0x800D807Cu + value * 24u, 0u);
                value = rrj_read32(slot);
                (void)(uint32_t)sub_8002C9D4(value);
                rrj_write32(slot, UINT32_MAX);
            }
            break;
        case 4u:
            rrj_write32(0x8005AE78u, 0u);
            (void)queue_call(rrj_host_context(), call, 0x8002C9A8u, 0u, 0u);
            break;
        case 5u:
            w_u8(0x8009C5DFu, 0u);
            (void)queue_call(rrj_host_context(), call, 0x8007E9DCu, 0u, 0u);
            break;
        case 6u:
            (void)queue_call(rrj_host_context(), call, 0x8007F158u, 1u, 0u);
            break;
        default:
            break;
    }
    return 1u;
}

uint32_t sub_F_80078E80(uint32_t list, RRJQueueCall call)
{
    uint32_t result = 1u, id;
    FUNCTION_MARKER(0x80078E80, "RASHCDF.BIN");
    id = (uint32_t)signed_half(rrj_host_context(), list);
    while (id != UINT32_MAX)
    {
        if (!result)
            break;
        result = sub_F_80078C68(id, 0u, call);
        list += 2u;
        id = (uint32_t)signed_half(rrj_host_context(), list);
    }
    return result;
}

uint32_t sub_F_80065768(uint32_t descriptor, RRJImageUpload upload)
{
    FUNCTION_MARKER(0x80065768u, "RASHCDF.BIN");
    uint8_t rect[8];
    uint32_t pixels, i;
    for (i = 0; i < 4; ++i)
        rrj_put16(rect + i * 2, half(rrj_host_context(), descriptor + 16 + i * 2));
    pixels = rrj_read32(descriptor + 4) + 16;
    if (!upload)
        abort();
    return upload(rrj_host_context(), rect, pixels);
}

/* The count comparison is signed, but address arithmetic wraps at 32 bits.
 * Preserve load/store order even when supplied records alias queue storage. */
static void enqueue(RRJMemory *m, uint32_t descriptor, uint32_t rect, uint32_t count_address, uint32_t base, uint32_t kind)
{
    uint32_t count = rrj_read32(count_address), q, data, y;
    if (rrj_s32(count) >= 6)
        return;
    q = base + count * 36;
    data = rrj_read32(descriptor + 4);
    rrj_write32(q + 8, kind);
    rrj_write32(q + 12, 1);
    rrj_write32(q + 16, 0);
    rrj_write32(q, data);
    rrj_write32(q + 28, (uint32_t)signed_half(m, rect));
    y = (uint32_t)signed_half(m, rect + 2);
    rrj_write32(count_address, count + 1);
    rrj_write32(q + 32, y);
}

uint32_t sub_F_8007A400(uint32_t descriptor, uint32_t id, RRJImageUpload upload)
{
    FUNCTION_MARKER(0x8007A400u, "RASHCDF.BIN");
    uint32_t flags = rrj_read32(descriptor), rect, x, y;
    if (!(flags & 0x10000000))
        return 0;
    if (flags & 0x40000000)
    {
        if (flags & 0x80000000)
        {
            if (flags & 0x20000000)
                return 1;
            rect = sub_F_8007A6C0(id, 0);
            enqueue(rrj_host_context(), descriptor, rect, 0x8009C2F0, 0x8009C2F8, 4);
            rrj_write32(descriptor, rrj_read32(descriptor) | 0x20000002);
            y = half(rrj_host_context(), rect + 2);
            x = half(rrj_host_context(), rect);
            store_half(rrj_host_context(), descriptor + 28, ((y & 0x100) >> 4) | ((x & 0x3ff) >> 6) | 0x100 | 4 * (y & 0x200));
            store_half(rrj_host_context(), descriptor + 24, (uint32_t)(signed_half(rrj_host_context(), rect) % 64));
            store_half(rrj_host_context(), descriptor + 26, (uint32_t)(signed_half(rrj_host_context(), rect + 2) % 256));
            store_half(rrj_host_context(), descriptor + 30, 0);
            return 1;
        }
        rect = sub_F_8007A6C0(id, 0);
        enqueue(rrj_host_context(), descriptor, rect, 0x8009C2F4, 0x8009C3D0, 3);
        return 0;
    }
    if (!(flags & 0x20000000))
        (void)sub_F_80065768(descriptor, upload);
    return 1;
}

static void reset_animation(RRJMemory *m, uint32_t sprite)
{
    w_u8(sprite + 16u, 0u);
    w_u8(sprite + 17u, 0u);
    w_u8(sprite + 18u, 0u);
    w_u8(sprite + 19u, 0u);
}

uint32_t sub_F_800705DC(uint32_t sprite, uint32_t link, uint32_t command, RRJImageUpload upload)
{
    uint32_t id, index, descriptor, flags, value, limit;
    uint32_t context, packet, x, y;
    FUNCTION_MARKER(0x800705DC, "RASHCDF.BIN");
    id = rrj_read32(sprite);
    for (index = 0; index < 300u; ++index)
        if (rrj_read32(0x80088E0Cu + index * 4u) == id)
            break;
    if (index == 300u)
        return 1u;
    descriptor = 0x8009DDE0u + index * 36u;
    flags = rrj_read32(descriptor);
    if (!(flags & 0x10000000u))
        return 1u;
    if (flags & 0x00100000u)
    {
        if (rrj_read32(0x800A0810u) != id)
        {
            rrj_write32(0x800A0810u, id);
            rrj_write32(descriptor, rrj_read32(descriptor) & 0xDFFFFFFFu);
        }
        flags = rrj_read32(descriptor);
    }
    if (flags & 0x40000000u)
    {
        if (!(flags & 0x80000000u))
        {
            enqueue(rrj_host_context(), descriptor, sprite + 8u, 0x8009C2F4u, 0x8009C3D0u, 3u);
            return 1u;
        }
        if (!(flags & 0x20000000u))
        {
            enqueue(rrj_host_context(), descriptor, descriptor + 16u, 0x8009C2F0u, 0x8009C2F8u, 4u);
            x = (uint32_t)signed_half(rrj_host_context(), descriptor + 16u);
            value = rrj_read32(descriptor);
            y = half(rrj_host_context(), descriptor + 18u);
            rrj_write32(descriptor, value | 0x20000002u);
            value = half(rrj_host_context(), descriptor + 16u);
            store_half(rrj_host_context(), descriptor + 28u, ((y & 0x100u) >> 4) | ((value & 0x3FFu) >> 6) | 0x100u | ((y & 0x200u) << 2));
            y = (uint32_t)signed_half(rrj_host_context(), descriptor + 18u);
            store_half(rrj_host_context(), descriptor + 24u, (uint32_t)((int32_t)x % 64));
            store_half(rrj_host_context(), descriptor + 26u, (uint32_t)((int32_t)y % 256));
            store_half(rrj_host_context(), descriptor + 30u, 0u);
        }
    }
    else if (!(flags & 0x20000000u))
    {
        (void)sub_F_80065768(descriptor, upload);
        rrj_write32(descriptor, rrj_read32(descriptor) | 0x20000000u);
    }
    if (command == 1u || command == 3u)
    {
        value = r_u8(sprite + 20u);
        reset_animation(rrj_host_context(), sprite);
        w_u8(sprite + 20u, (uint8_t)((value & 0x5Fu) | (command == 3u ? 0x20u : 0u)));
    }
    else if (command == 2u)
        w_u8(sprite + 20u, (uint8_t)(r_u8(sprite + 20u) | 0x20u));
    if (!(r_u8(sprite + 20u) & 0x20u))
    {
        value = r_u8(sprite + 17u);
        if (r_u8(sprite + 15u) < value)
        {
            value = r_u8(sprite + 16u);
            limit = r_u8(sprite + 12u);
            w_u8(sprite + 17u, 0u);
            value = (value + 1u) & 0xFFu;
            w_u8(sprite + 16u, (uint8_t)value);
            if (value >= limit)
                w_u8(sprite + 20u, (uint8_t)(r_u8(sprite + 20u) | 0xA0u));
            else
            {
                value = r_u8(sprite + 18u);
                value = (value + r_u8(sprite + 13u)) & 0xFFu;
                w_u8(sprite + 18u, (uint8_t)value);
                if ((int32_t)value >= signed_half(rrj_host_context(), descriptor + 20u))
                {
                    value = r_u8(sprite + 19u);
                    value += r_u8(sprite + 14u);
                    w_u8(sprite + 18u, 0u);
                    w_u8(sprite + 19u, (uint8_t)value);
                }
            }
        }
    }
    flags = r_u8(sprite + 20u);
    if (flags & 0x80u)
    {
        if (flags & 0x10u)
        {
            reset_animation(rrj_host_context(), sprite);
            w_u8(sprite + 20u, (uint8_t)(flags & 0x5Fu));
        }
        else if (flags & 0x40u)
        {
            value = r_u8(sprite + 20u);
            reset_animation(rrj_host_context(), sprite);
            w_u8(sprite + 20u, (uint8_t)((value & 0x7Fu) | 0x20u));
        }
        else
            return 1u;
    }
    if (!(r_u8(sprite + 20u) & 0x20u))
        w_u8(sprite + 17u, (uint8_t)(r_u8(sprite + 17u) + 1u));
    context = rrj_read32(0x8005B470u);
    packet = rrj_read32(context + 268u);
    limit = rrj_read32(0x8005B4D0u);
    if (packet + 24u >= limit)
    {
        packet = sub_80021C98(packet, 24u);
        context = rrj_read32(0x8005B470u);
        rrj_write32(context + 268u, packet);
    }
    context = rrj_read32(0x8005B470u);
    packet = rrj_read32(context + 268u);
    rrj_write32(context + 268u, packet + 24u);
    rrj_write32(packet, rrj_read32(link) | 0x05000000u);
    rrj_write32(packet + 4u, (half(rrj_host_context(), descriptor + 28u) & 0x9FFu) | 0xE1000200u);
    store_half(rrj_host_context(), packet + 18u, half(rrj_host_context(), descriptor + 30u));
    rrj_write32(packet + 8u, rrj_read32(sprite + 4u) | 0x64000000u);
    store_half(rrj_host_context(), packet + 12u, half(rrj_host_context(), sprite + 8u));
    store_half(rrj_host_context(), packet + 14u, half(rrj_host_context(), sprite + 10u));
    value = r_u8(descriptor + 24u);
    w_u8(packet + 16u, (uint8_t)(value + r_u8(sprite + 18u)));
    value = r_u8(descriptor + 26u);
    w_u8(packet + 17u, (uint8_t)(value + r_u8(sprite + 19u)));
    store_half(rrj_host_context(), packet + 20u, r_u8(sprite + 13u));
    store_half(rrj_host_context(), packet + 22u, r_u8(sprite + 14u));
    rrj_write32(link, packet);
    return 1u;
}

uint32_t sub_F_80070580(uint32_t menu, uint32_t entry, RRJImageUpload upload)
{
    uint32_t slot, table, command;
    FUNCTION_MARKER(0x80070580, "RASHCDF.BIN");
    slot = r_u8(0x8009C5E1u);
    table = rrj_read32(0x8009CFC8u);
    command = signed_half(rrj_host_context(), menu + 2u) ? 1u : 0u;
    if (slot & 0x80u)
        slot |= 0xFFFFFF00u;
    return sub_F_800705DC(entry + 16u, table + slot * 4u + 20u, command, upload);
}
