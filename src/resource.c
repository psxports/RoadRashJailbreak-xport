/* WIP: IDA draft checked against F 80065768/8007A400 MIPS.
 * These functions enqueue work; queue execution is a separate dependency. */
#include "resource.h"
#include "resource_lookup.h"
#include "packet.h"
#include <stdlib.h>

static uint32_t half(RRJMemory *m, uint32_t a)
{
    return rrj_u16(rrj_at(m, a, 2));
}

static int32_t signed_half(RRJMemory *m, uint32_t a)
{
    uint32_t v = half(m, a);
    return v < 32768 ? (int32_t)v : (int32_t)v - 65536;
}

static void store_half(RRJMemory *m, uint32_t a, uint32_t v)
{
    rrj_put16(rrj_at(m, a, 2), v);
}

void sub_F_800623F8(RRJMemory *m)
{
    FUNCTION_MARKER(0x800623F8, "RASHCDF.BIN");
    rrj_write32(m, 0x800810ECu, 1u);
    rrj_write32(m, 0x800810F0u, 3u);
    rrj_write32(m, 0x800810D4u, 1u);
}

void sub_F_80062420(RRJMemory *m)
{
    FUNCTION_MARKER(0x80062420, "RASHCDF.BIN");
    rrj_write32(m, 0x800810ECu, 2u);
    rrj_write32(m, 0x800810F0u, 2u);
}

static uint32_t queue_call(RRJMemory *m, RRJQueueCall call, uint32_t target, uint32_t a0, uint32_t a1)
{
    if (!call)
        abort();
    return call(m, target, a0, a1);
}

static uint32_t queue_big_word(RRJMemory *m, uint32_t address)
{
    uint32_t word = rrj_read32(m, address);
    return (word >> 24) | ((word >> 8) & 0xFF00u) | ((word & 0xFF00u) << 8) | (word << 24);
}

static uint32_t queue_big_half(RRJMemory *m, uint32_t address)
{
    uint32_t value = half(m, address);
    return (value >> 8) | ((value & 0xFFu) << 8);
}

static void queue_wait_decoder(RRJMemory *m)
{
    uint32_t context = rrj_read32(m, 0x800810E8u), index = 0u;
    if (rrj_read32(m, context + 0x120u))
        return;
    context = rrj_read32(m, 0x800810E8u);
    while (index <= 0x30D40u)
    {
        uint32_t ready = rrj_read32(m, context + 0x120u);
        ++index;
        if (ready)
            break;
    }
}

static void queue_decoder_output(RRJMemory *m, RRJQueueCall call)
{
    uint32_t context = rrj_read32(m, 0x800810E8u);
    uint32_t width = (uint32_t)signed_half(m, context + 32u);
    uint32_t height = (uint32_t)signed_half(m, context + 34u);
    uint32_t product = width * height;
    uint32_t corrected = product + (product >> 31);
    uint32_t length = (corrected >> 1) | (corrected & 0x80000000u);
    (void)queue_call(m, call, 0x8004D988u, rrj_read32(m, context + 36u), length);
}

void sub_F_800627BC(RRJMemory *m, uint32_t queue, uint32_t count, RRJQueueCall call)
{
    uint32_t index;
    FUNCTION_MARKER(0x800627BC, "RASHCDF.BIN");
    (void)queue_call(m, call, 0x8004D7B4u, 1u, 0u);
    if (rrj_s32(count) <= 0)
        return;
    for (index = 0u; rrj_s32(index) < rrj_s32(count); ++index, queue += 36u)
    {
        uint32_t data = rrj_read32(m, queue), x = rrj_read32(m, queue + 28u);
        uint32_t mode = rrj_read32(m, queue + 12u), y = rrj_read32(m, queue + 32u);
        uint32_t payload = data + 8u, tag, size, height, width, kind, context;
        uint32_t offset_x = 0u, offset_y = 0u, format, buffer, value;
        if (mode)
            sub_F_80062420(m);
        else
            sub_F_800623F8(m);
        tag = queue_big_word(m, data);
        size = queue_big_word(m, data + 4u);
        while (tag != 0x4D444543u)
        {
            if (tag == 0x564C4330u && !rrj_read32(m, queue + 12u))
                (void)queue_call(m, call, 0x8002026Cu, payload, 0u);
            data += size;
            payload = data + 8u;
            tag = queue_big_word(m, data);
            size = queue_big_word(m, data + 4u);
        }
        height = queue_big_half(m, payload + 2u);
        width = queue_big_half(m, payload);
        value = queue_call(m, call, 0x8004DE34u, payload + 8u, 0u);
        if (rrj_read32(m, 0x800810F4u) < value)
            rrj_write32(m, 0x800810F4u, value);
        kind = rrj_read32(m, queue + 8u);
        switch (kind)
        {
            case 0u:
                context = rrj_read32(m, 0x800810E8u);
                buffer = rrj_read32(m, context + rrj_read32(m, context + 52u) * 4u + 40u);
                (void)queue_call(m, call, 0x80020400u, payload + 8u, buffer);
                context = rrj_read32(m, 0x800810E8u);
                rrj_write32(m, context + 52u, 1u - rrj_read32(m, context + 52u));
                break;
            case 1u:
            case 3u:
            case 4u:
                if (kind != 4u)
                {
                    context = rrj_read32(m, 0x8005B470u);
                    context += r_u8(context + 5u) * 112u;
                    offset_x = (uint32_t)signed_half(m, context + 24u);
                    offset_y = (uint32_t)signed_half(m, context + 26u);
                }
                x += offset_x;
                format = rrj_read32(m, 0x800810F0u);
                y += offset_y;
                context = rrj_read32(m, 0x800810E8u);
                store_half(m, context, x);
                store_half(m, context + 2u, y);
                store_half(m, context + 6u, height);
                store_half(m, context + 28u, x);
                store_half(m, context + 30u, y);
                store_half(m, context + 32u, format * 8u);
                store_half(m, context + 34u, height);
                rrj_write32(m, context + 16u, 0u);
                store_half(m, context + 4u, (width * format) >> 1);
                buffer = rrj_read32(m, context + 48u);
                (void)queue_call(m, call, 0x80020400u, payload + 8u, buffer);
                value = rrj_read32(m, 0x800810ECu);
                context = rrj_read32(m, 0x800810E8u);
                rrj_write32(m, context + 0x120u, 0u);
                (void)queue_call(m, call, 0x8004D90Cu, buffer, value);
                queue_decoder_output(m, call);
                (void)queue_call(m, call, 0x8004D9E4u, 0u, 0u);
                queue_wait_decoder(m);
                break;
            case 2u:
                context = rrj_read32(m, 0x8005B470u);
                value = rrj_read32(m, 0x800810ECu);
                context += r_u8(context + 5u) * 112u;
                offset_x = (uint32_t)signed_half(m, context + 24u);
                offset_y = (uint32_t)signed_half(m, context + 26u);
                context = rrj_read32(m, 0x800810E8u);
                format = rrj_read32(m, 0x800810F0u);
                x += offset_x;
                y += offset_y;
                buffer = rrj_read32(m, context + 52u);
                store_half(m, context + 6u, height);
                store_half(m, context, x);
                store_half(m, context + 2u, y);
                store_half(m, context + 28u, x);
                store_half(m, context + 30u, y);
                store_half(m, context + 32u, format * 8u);
                store_half(m, context + 34u, height);
                rrj_write32(m, context + 16u, 0u);
                rrj_write32(m, context + 0x120u, 0u);
                store_half(m, context + 4u, (width * format) >> 1);
                buffer = rrj_read32(m, context + (1u - buffer) * 4u + 40u);
                (void)queue_call(m, call, 0x8004D90Cu, buffer, value);
                queue_decoder_output(m, call);
                context = rrj_read32(m, 0x800810E8u);
                buffer = rrj_read32(m, context + rrj_read32(m, context + 52u) * 4u + 40u);
                (void)queue_call(m, call, 0x80020400u, payload + 8u, buffer);
                (void)queue_call(m, call, 0x8004D9E4u, 0u, 0u);
                queue_wait_decoder(m);
                context = rrj_read32(m, 0x800810E8u);
                rrj_write32(m, context + 52u, 1u - rrj_read32(m, context + 52u));
                break;
            default:
                break;
        }
        if (kind < 5u)
            kind = rrj_read32(m, queue + 8u);
        if (kind != 3u)
        {
            value = rrj_read32(m, queue + 16u);
            if (value)
                (void)queue_call(m, call, 0x80061C44u, value, 0u);
        }
        rrj_write32(m, queue + 4u, data + size);
    }
}

void sub_F_80062774(RRJMemory *m, uint32_t queue, uint32_t count, RRJQueueCall call)
{
    uint32_t context;
    FUNCTION_MARKER(0x80062774, "RASHCDF.BIN");
    context = rrj_read32(m, 0x800810E8u);
    if (rrj_read32(m, context + 0x130u))
        sub_F_800627BC(m, queue, count, call);
    /* The alternate original branch is the empty nullsub_8 */
}

uint32_t sub_F_80078BD8(RRJMemory *m, uint32_t list, uint32_t wait, RRJQueueCall call)
{
    uint32_t result = 1u, id;
    FUNCTION_MARKER(0x80078BD8, "RASHCDF.BIN");
    id = (uint32_t)signed_half(m, list);
    while (id != UINT32_MAX)
    {
        if (!result)
            return result;
        if (!queue_call(m, call, 0x800782B0u, id, wait))
            result = 0u;
        list += 2u;
        id = (uint32_t)signed_half(m, list);
    }
    return result;
}

uint32_t sub_F_80078C68(RRJMemory *m, uint32_t id, uint32_t force, RRJQueueCall call)
{
    uint32_t descriptor, value, flags, data, kind, subtype, slot;
    FUNCTION_MARKER(0x80078C68, "RASHCDF.BIN");
    id = (id & 0xFFFFu) | ((id & 0x8000u) ? 0xFFFF0000u : 0u);
    if (rrj_s32(id) >= 37)
        return 1u;
    descriptor = 0x800897E4u + id * 32u;
    if (!force && (half(m, descriptor) & 0x1000u))
        return 1u;
    (void)queue_call(m, call, 0x80022A78u, 2u, 0u);
    value = rrj_read32(m, descriptor + 24u);
    rrj_write32(m, 0x8005ACACu, 0u);
    if (value != UINT32_MAX)
    {
        (void)queue_call(m, call, 0x8001460Cu, value, 0u);
        rrj_write32(m, descriptor + 24u, UINT32_MAX);
    }
    flags = half(m, descriptor);
    data = rrj_read32(m, descriptor + 4u);
    store_half(m, descriptor, flags & 0xFFF3u);
    if (data)
    {
        if (!(flags & 1u))
            (void)queue_call(m, call, 0x800144B8u, data, 0u);
        flags = half(m, descriptor);
        rrj_write32(m, descriptor + 4u, 0u);
        store_half(m, descriptor, flags & 0xFFFEu);
    }
    kind = r_u8(descriptor + 2u);
    switch (kind)
    {
        case 3u:
            subtype = r_u8(descriptor + 3u);
            if (subtype == 1u || subtype == 2u || subtype == 3u)
            {
                slot = subtype == 1u ? 0x8009C5B8u : subtype == 2u ? 0x8009C5C0u : 0x8009C5BCu;
                value = rrj_read32(m, slot);
                rrj_write32(m, 0x800D807Cu + value * 24u, 0u);
                value = rrj_read32(m, slot);
                (void)queue_call(m, call, 0x8002C9D4u, value, 0u);
                rrj_write32(m, slot, UINT32_MAX);
            }
            break;
        case 4u:
            rrj_write32(m, 0x8005AE78u, 0u);
            (void)queue_call(m, call, 0x8002C9A8u, 0u, 0u);
            break;
        case 5u:
            w_u8(0x8009C5DFu, 0u);
            (void)queue_call(m, call, 0x8007E9DCu, 0u, 0u);
            break;
        case 6u:
            (void)queue_call(m, call, 0x8007F158u, 1u, 0u);
            break;
        default:
            break;
    }
    return 1u;
}

uint32_t sub_F_80078E80(RRJMemory *m, uint32_t list, RRJQueueCall call)
{
    uint32_t result = 1u, id;
    FUNCTION_MARKER(0x80078E80, "RASHCDF.BIN");
    id = (uint32_t)signed_half(m, list);
    while (id != UINT32_MAX)
    {
        if (!result)
            break;
        result = sub_F_80078C68(m, id, 0u, call);
        list += 2u;
        id = (uint32_t)signed_half(m, list);
    }
    return result;
}

uint32_t sub_F_80065768(RRJMemory *m, uint32_t descriptor, RRJImageUpload upload)
{
    uint8_t rect[8];
    uint32_t pixels, i;
    for (i = 0; i < 4; ++i)
        rrj_put16(rect + i * 2, half(m, descriptor + 16 + i * 2));
    pixels = rrj_read32(m, descriptor + 4) + 16;
    if (!upload)
        abort();
    return upload(m, rect, pixels);
}

/* The count comparison is signed, but address arithmetic wraps at 32 bits.
 * Preserve load/store order even when supplied records alias queue storage. */
static void enqueue(RRJMemory *m, uint32_t descriptor, uint32_t rect, uint32_t count_address, uint32_t base, uint32_t kind)
{
    uint32_t count = rrj_read32(m, count_address), q, data, y;
    if (rrj_s32(count) >= 6)
        return;
    q = base + count * 36;
    data = rrj_read32(m, descriptor + 4);
    rrj_write32(m, q + 8, kind);
    rrj_write32(m, q + 12, 1);
    rrj_write32(m, q + 16, 0);
    rrj_write32(m, q, data);
    rrj_write32(m, q + 28, (uint32_t)signed_half(m, rect));
    y = (uint32_t)signed_half(m, rect + 2);
    rrj_write32(m, count_address, count + 1);
    rrj_write32(m, q + 32, y);
}

uint32_t sub_F_8007A400(RRJMemory *m, uint32_t descriptor, uint32_t id, RRJImageUpload upload)
{
    uint32_t flags = rrj_read32(m, descriptor), rect, x, y;
    if (!(flags & 0x10000000))
        return 0;
    if (flags & 0x40000000)
    {
        if (flags & 0x80000000)
        {
            if (flags & 0x20000000)
                return 1;
            rect = sub_F_8007A6C0(id, 0);
            enqueue(m, descriptor, rect, 0x8009C2F0, 0x8009C2F8, 4);
            rrj_write32(m, descriptor, rrj_read32(m, descriptor) | 0x20000002);
            y = half(m, rect + 2);
            x = half(m, rect);
            store_half(m, descriptor + 28, ((y & 0x100) >> 4) | ((x & 0x3ff) >> 6) | 0x100 | 4 * (y & 0x200));
            store_half(m, descriptor + 24, (uint32_t)(signed_half(m, rect) % 64));
            store_half(m, descriptor + 26, (uint32_t)(signed_half(m, rect + 2) % 256));
            store_half(m, descriptor + 30, 0);
            return 1;
        }
        rect = sub_F_8007A6C0(id, 0);
        enqueue(m, descriptor, rect, 0x8009C2F4, 0x8009C3D0, 3);
        return 0;
    }
    if (!(flags & 0x20000000))
        (void)sub_F_80065768(m, descriptor, upload);
    return 1;
}

static void reset_animation(RRJMemory *m, uint32_t sprite)
{
    w_u8(sprite + 16u, 0u);
    w_u8(sprite + 17u, 0u);
    w_u8(sprite + 18u, 0u);
    w_u8(sprite + 19u, 0u);
}

uint32_t sub_F_800705DC(RRJMemory *m, uint32_t sprite, uint32_t link, uint32_t command, RRJImageUpload upload)
{
    uint32_t id, index, descriptor, flags, value, limit;
    uint32_t context, packet, x, y;
    FUNCTION_MARKER(0x800705DC, "RASHCDF.BIN");
    id = rrj_read32(m, sprite);
    for (index = 0; index < 300u; ++index)
        if (rrj_read32(m, 0x80088E0Cu + index * 4u) == id)
            break;
    if (index == 300u)
        return 1u;
    descriptor = 0x8009DDE0u + index * 36u;
    flags = rrj_read32(m, descriptor);
    if (!(flags & 0x10000000u))
        return 1u;
    if (flags & 0x00100000u)
    {
        if (rrj_read32(m, 0x800A0810u) != id)
        {
            rrj_write32(m, 0x800A0810u, id);
            rrj_write32(m, descriptor, rrj_read32(m, descriptor) & 0xDFFFFFFFu);
        }
        flags = rrj_read32(m, descriptor);
    }
    if (flags & 0x40000000u)
    {
        if (!(flags & 0x80000000u))
        {
            enqueue(m, descriptor, sprite + 8u, 0x8009C2F4u, 0x8009C3D0u, 3u);
            return 1u;
        }
        if (!(flags & 0x20000000u))
        {
            enqueue(m, descriptor, descriptor + 16u, 0x8009C2F0u, 0x8009C2F8u, 4u);
            x = (uint32_t)signed_half(m, descriptor + 16u);
            value = rrj_read32(m, descriptor);
            y = half(m, descriptor + 18u);
            rrj_write32(m, descriptor, value | 0x20000002u);
            value = half(m, descriptor + 16u);
            store_half(m, descriptor + 28u, ((y & 0x100u) >> 4) |
                       ((value & 0x3FFu) >> 6) | 0x100u | ((y & 0x200u) << 2));
            y = (uint32_t)signed_half(m, descriptor + 18u);
            store_half(m, descriptor + 24u, (uint32_t)((int32_t)x % 64));
            store_half(m, descriptor + 26u, (uint32_t)((int32_t)y % 256));
            store_half(m, descriptor + 30u, 0u);
        }
    }
    else if (!(flags & 0x20000000u))
    {
        (void)sub_F_80065768(m, descriptor, upload);
        rrj_write32(m, descriptor, rrj_read32(m, descriptor) | 0x20000000u);
    }
    if (command == 1u || command == 3u)
    {
        value = r_u8(sprite + 20u);
        reset_animation(m, sprite);
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
                if ((int32_t)value >= signed_half(m, descriptor + 20u))
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
            reset_animation(m, sprite);
            w_u8(sprite + 20u, (uint8_t)(flags & 0x5Fu));
        }
        else if (flags & 0x40u)
        {
            value = r_u8(sprite + 20u);
            reset_animation(m, sprite);
            w_u8(sprite + 20u, (uint8_t)((value & 0x7Fu) | 0x20u));
        }
        else
            return 1u;
    }
    if (!(r_u8(sprite + 20u) & 0x20u))
        w_u8(sprite + 17u, (uint8_t)(r_u8(sprite + 17u) + 1u));
    context = rrj_read32(m, 0x8005B470u);
    packet = rrj_read32(m, context + 268u);
    limit = rrj_read32(m, 0x8005B4D0u);
    if (packet + 24u >= limit)
    {
        packet = sub_80021C98(m, packet, 24u);
        context = rrj_read32(m, 0x8005B470u);
        rrj_write32(m, context + 268u, packet);
    }
    context = rrj_read32(m, 0x8005B470u);
    packet = rrj_read32(m, context + 268u);
    rrj_write32(m, context + 268u, packet + 24u);
    rrj_write32(m, packet, rrj_read32(m, link) | 0x05000000u);
    rrj_write32(m, packet + 4u, (half(m, descriptor + 28u) & 0x9FFu) | 0xE1000200u);
    store_half(m, packet + 18u, half(m, descriptor + 30u));
    rrj_write32(m, packet + 8u, rrj_read32(m, sprite + 4u) | 0x64000000u);
    store_half(m, packet + 12u, half(m, sprite + 8u));
    store_half(m, packet + 14u, half(m, sprite + 10u));
    value = r_u8(descriptor + 24u);
    w_u8(packet + 16u, (uint8_t)(value + r_u8(sprite + 18u)));
    value = r_u8(descriptor + 26u);
    w_u8(packet + 17u, (uint8_t)(value + r_u8(sprite + 19u)));
    store_half(m, packet + 20u, r_u8(sprite + 13u));
    store_half(m, packet + 22u, r_u8(sprite + 14u));
    rrj_write32(m, link, packet);
    return 1u;
}

uint32_t sub_F_80070580(RRJMemory *m, uint32_t menu, uint32_t entry, RRJImageUpload upload)
{
    uint32_t slot, table, command;
    FUNCTION_MARKER(0x80070580, "RASHCDF.BIN");
    slot = r_u8(0x8009C5E1u);
    table = rrj_read32(m, 0x8009CFC8u);
    command = signed_half(m, menu + 2u) ? 1u : 0u;
    if (slot & 0x80u)
        slot |= 0xFFFFFF00u;
    return sub_F_800705DC(m, entry + 16u, table + slot * 4u + 20u, command, upload);
}
