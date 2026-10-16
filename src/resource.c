#include "game_main.h"
#include "solo_resource_dependencies.h"
#include "video_preview.h"
#include "psx_press.h"
#include <string.h>
#include "music_stop.h"
#include "race_leaf.h"
#include "native_game_api.h"
#include "psx.h"
#include "gpu.h"
/* WIP: IDA draft checked against F 80065768/8007A400 MIPS.
 * These functions enqueue work; queue execution is a separate dependency. */
#include "resource.h"
#include "resource_lookup.h"
#include "packet.h"
#include <stdlib.h>

void sub_800623F8(void)
{
    FUNCTION_MARKER(0x800623F8, "RASHCDF.BIN");
    w_u32(0x800810ECu, 1u);
    w_u32(0x800810F0u, 3u);
    w_u32(0x800810D4u, 1u);
}

void sub_80062420(void)
{
    FUNCTION_MARKER(0x80062420, "RASHCDF.BIN");
    w_u32(0x800810ECu, 2u);
    w_u32(0x800810F0u, 2u);
}

/* Keep callback tokens in guest data and native pointers in the SDK */
static DecDCTCallback resource_callback(uint32_t address)
{
    if (!address)
        return NULL;
    if (address == 0x80062674u)
        return rrj_resource_decoder_output;
    if (address == 0x80013AE4)
        return sub_80013AE4;
    abort();
}

static uint32_t resource_callback_address(DecDCTCallback callback)
{
    if (!callback)
        return 0u;
    if (callback == rrj_resource_decoder_output)
        return 0x80062674u;
    if (callback == sub_80013AE4)
        return 0x80013AE4;
    abort();
}

static PsxInterruptCallback resource_draw_callback(uint32_t address)
{
    if (address)
        abort();
    return NULL;
}

static uint32_t resource_draw_callback_address(PsxInterruptCallback callback)
{
    if (callback)
        abort();
    return 0u;
}

void rrj_resource_decoder_output(void)
{
    sub_80062674();
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x800624EC sub_800624EC */
uint32_t sub_800624EC(uint32_t state, uint32_t first, uint32_t second, uint32_t encoded, uint32_t decoded, uint32_t enabled)
{
    FUNCTION_MARKER(0x800624ECu, "RASHCDF.BIN");
    uint32_t context, previous;
    if (!state || !first || !second || !encoded || !decoded)
        return 0u;
    (void)memset(xport_guest_ptr(state, 0x134u), 0, 0x134u);
    w_u32(0x800810E8u, state);
    (void)(uint32_t)DrawSync(0u);
    (void)(uint32_t)DecDCTinSync(((int32_t)(uint32_t)(0u)));
    (void)(uint32_t)DecDCToutSync(((int32_t)(uint32_t)(0u)));
    context = r_u32(0x800810E8u);
    w_u32(context + 0x118u, 0u);
    w_u32(context + 40u, first);
    w_u32(context + 44u, second);
    w_u32(context + 48u, encoded);
    w_u32(context + 36u, decoded);
    (void)DecDCTReset(((int32_t)(uint32_t)(0u)));
    (void)sub_80043DD4();
    context = r_u32(0x800810E8u);
    w_u32(context + 0x130u, enabled);
    previous = resource_callback_address(DecDCToutCallback(enabled ? rrj_resource_decoder_output : NULL));
    w_u32(0x80055EE4u, enabled ? 0x80062674u : 0u);
    context = r_u32(0x800810E8u);
    w_u32(context + 0x124u, previous);
    previous = resource_callback_address(DecDCTinCallback(NULL));
    w_u32(0x80055EE0u, 0u);
    context = r_u32(0x800810E8u);
    w_u32(context + 0x128u, previous);
    previous = resource_draw_callback_address(DrawSyncCallback(NULL));
    w_u32(0x80055F78u, 0u);
    context = r_u32(0x800810E8u);
    if (enabled)
    {
        w_u32(context + 0x12Cu, previous);
        if (r_u32(0x800810ECu) == 1u)
            w_u32(0x800810D4u, 1u);
    }
    else
    {
        w_u32(0x800810D4u, 0u);
        w_u32(context + 0x12Cu, previous);
    }
    (void)sub_80043DF4();
    return 1u;
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80062440 sub_80062440 */
uint32_t sub_80062440(void)
{
    FUNCTION_MARKER(0x80062440u, "RASHCDF.BIN");
    uint32_t context = r_u32(0x800810E8u), previous, result;
    if (!context)
        return 0u;
    (void)(uint32_t)DecDCTinSync(((int32_t)(uint32_t)(0u)));
    (void)(uint32_t)DecDCToutSync(((int32_t)(uint32_t)(0u)));
    (void)(uint32_t)DrawSync(0u);
    (void)sub_80043DD4();
    context = r_u32(0x800810E8u);
    previous = r_u32(context + 0x124u);
    (void)DecDCToutCallback(resource_callback(previous));
    w_u32(0x80055EE4u, previous);
    context = r_u32(0x800810E8u);
    previous = r_u32(context + 0x128u);
    (void)DecDCTinCallback(resource_callback(previous));
    w_u32(0x80055EE0u, previous);
    context = r_u32(0x800810E8u);
    previous = r_u32(context + 0x12Cu);
    (void)DrawSyncCallback(resource_draw_callback(previous));
    w_u32(0x80055F78u, previous);
    context = r_u32(0x800810E8u);
    w_u32(context + 0x124u, 0u);
    w_u32(context + 0x128u, 0u);
    w_u32(context + 0x12Cu, 0u);
    (void)sub_80043DF4();
    DecDCTReset(1);
    /* Original mode-one SDK result */
    result = 0x60000000u;
    w_u32(0x800810E8u, 0u);
    return result;
}

static void queue_wait_decoder(void)
{
    uint32_t context = r_u32(0x800810E8u), index = 0u;
    if (r_u32(context + 0x120u))
        return;
    context = r_u32(0x800810E8u);
    while (index <= 0x30D40u)
    {
        uint32_t ready = r_u32(context + 0x120u);
        ++index;
        if (ready)
            break;
    }
}

static void queue_decoder_output(void)
{
    uint32_t scalar_v_18;
    uint32_t scalar_v_19;

    uint32_t context = r_u32(0x800810E8u);
    uint32_t width = (uint32_t)((scalar_v_18 = (uint32_t)r_u16(((uint32_t)(context + 32u)))), (((int32_t)(scalar_v_18 < 32768 ? (int32_t)scalar_v_18 : (int32_t)scalar_v_18 - 65536))));
    uint32_t height = (uint32_t)((scalar_v_19 = (uint32_t)r_u16(((uint32_t)(context + 34u)))), (((int32_t)(scalar_v_19 < 32768 ? (int32_t)scalar_v_19 : (int32_t)scalar_v_19 - 65536))));
    uint32_t product = width * height;
    uint32_t corrected = product + (product >> 31);
    uint32_t length = (corrected >> 1) | (corrected & 0x80000000u);
    (void)DecDCTout((uint32_t *)xport_guest_ptr(r_u32(context + 36u), 1), ((int32_t)(uint32_t)(length)));
}

/* FUNCTION_MARKER: RASHCDF.BIN:0x80062674 sub_80062674 */
uint32_t sub_80062674(void)
{
    uint32_t scalar_v_20;
    uint32_t scalar_v_21;
    uint32_t scalar_v_22;
    uint32_t scalar_v_23;
    uint32_t scalar_v_24;

    FUNCTION_MARKER(0x80062674u, "RASHCDF.BIN");
    uint32_t context = r_u32(0x800810E8u);
    uint32_t buffer = r_u32(context + 36u), format, x, first, second, right, result;
    (void)rrj_gpu_upload(xport_guest_ptr(context + 28u, 8), buffer);
    context = r_u32(0x800810E8u);
    format = r_u32(0x800810F0u);
    x = (uint32_t)r_u16((context + 28u));
    w_u16((context + 28u), (x + (format << 3)));
    x = (uint32_t)((scalar_v_20 = (uint32_t)r_u16(((uint32_t)(context + 28u)))), (((int32_t)(scalar_v_20 < 32768 ? (int32_t)scalar_v_20 : (int32_t)scalar_v_20 - 65536))));
    first = r_u32(context + 16u);
    second = r_u32(context + 16u);
    right = (uint32_t)((scalar_v_21 = (uint32_t)r_u16(((uint32_t)(context + (first << 3))))), (((int32_t)(scalar_v_21 < 32768 ? (int32_t)scalar_v_21 : (int32_t)scalar_v_21 - 65536))));
    right += (uint32_t)((scalar_v_22 = (uint32_t)r_u16(((uint32_t)(context + (second << 3) + 4u)))), (((int32_t)(scalar_v_22 < 32768 ? (int32_t)scalar_v_22 : (int32_t)scalar_v_22 - 65536))));
    if (((int32_t)(uint32_t)(x)) < ((int32_t)(uint32_t)(right)))
    {
        uint32_t width = (uint32_t)((scalar_v_23 = (uint32_t)r_u16(((uint32_t)(context + 32u)))), (((int32_t)(scalar_v_23 < 32768 ? (int32_t)scalar_v_23 : (int32_t)scalar_v_23 - 65536))));
        uint32_t height = (uint32_t)((scalar_v_24 = (uint32_t)r_u16(((uint32_t)(context + 34u)))), (((int32_t)(scalar_v_24 < 32768 ? (int32_t)scalar_v_24 : (int32_t)scalar_v_24 - 65536))));
        uint32_t product = width * height;
        buffer = r_u32(context + 36u);
        product += product >> 31;
        DecDCTout((uint32_t *)xport_guest_ptr(buffer, 1), ((int32_t)(uint32_t)(product)) >> 1);
        return r_u32(0x8005A370u);
    }
    w_u32(context + 0x120u, 1u);
    first = r_u32(context + 16u);
    result = (uint32_t)r_u16((context + (first << 3)));
    w_u16((context + 28u), result);
    first = r_u32(context + 16u);
    result = (uint32_t)r_u16((context + (first << 3) + 2u));
    w_u16((context + 30u), result);
    return result;
}

void sub_800627BC(uint32_t queue, uint32_t count)
{
    uint32_t scalar_value_79;
    uint32_t scalar_value_80;

    uint32_t scalar_word_75;
    uint32_t scalar_word_76;
    uint32_t scalar_word_77;
    uint32_t scalar_word_78;

    uint32_t scalar_v_25;
    uint32_t scalar_v_26;
    uint32_t scalar_v_27;
    uint32_t scalar_v_28;

    uint32_t index;
    FUNCTION_MARKER(0x800627BC, "RASHCDF.BIN");
    (void)DecDCTReset(((int32_t)(uint32_t)(1u)));
    if (((int32_t)(uint32_t)(count)) <= 0)
        return;
    for (index = 0u; ((int32_t)(uint32_t)(index)) < ((int32_t)(uint32_t)(count)); ++index, queue += 36u)
    {
        uint32_t data = r_u32(queue), x = r_u32(queue + 28u);
        uint32_t mode = r_u32(queue + 12u), y = r_u32(queue + 32u);
        uint32_t payload = data + 8u, tag, size, height, width, kind, context;
        uint32_t offset_x = 0u, offset_y = 0u, format, buffer, value;
        if (mode)
            sub_80062420();
        else
            sub_800623F8();
        tag = ((scalar_word_75 = r_u32(((uint32_t)(data)))), (((uint32_t)((scalar_word_75 >> 24) | ((scalar_word_75 >> 8) & 0xFF00u) | ((scalar_word_75 & 0xFF00u) << 8) | (scalar_word_75 << 24)))));
        size = ((scalar_word_76 = r_u32(((uint32_t)(data + 4u)))), (((uint32_t)((scalar_word_76 >> 24) | ((scalar_word_76 >> 8) & 0xFF00u) | ((scalar_word_76 & 0xFF00u) << 8) | (scalar_word_76 << 24)))));
        while (tag != 0x4D444543u)
        {
            if (tag == 0x564C4330u && !r_u32(queue + 12u))
                sub_8002026C(payload);
            data += size;
            payload = data + 8u;
            tag = ((scalar_word_77 = r_u32(((uint32_t)(data)))), (((uint32_t)((scalar_word_77 >> 24) | ((scalar_word_77 >> 8) & 0xFF00u) | ((scalar_word_77 & 0xFF00u) << 8) | (scalar_word_77 << 24)))));
            size = ((scalar_word_78 = r_u32(((uint32_t)(data + 4u)))), (((uint32_t)((scalar_word_78 >> 24) | ((scalar_word_78 >> 8) & 0xFF00u) | ((scalar_word_78 & 0xFF00u) << 8) | (scalar_word_78 << 24)))));
        }
        height = ((scalar_value_79 = (uint32_t)r_u16(((uint32_t)(payload + 2u)))), (((uint32_t)((scalar_value_79 >> 8) | ((scalar_value_79 & 0xFFu) << 8)))));
        width = ((scalar_value_80 = (uint32_t)r_u16(((uint32_t)(payload)))), (((uint32_t)((scalar_value_80 >> 8) | ((scalar_value_80 & 0xFFu) << 8)))));
        value = (uint32_t)sub_8004DE34(payload + 8u);
        if (r_u32(0x800810F4u) < value)
            w_u32(0x800810F4u, value);
        kind = r_u32(queue + 8u);
        switch (kind)
        {
            case 0u:
                context = r_u32(0x800810E8u);
                buffer = r_u32(context + r_u32(context + 52u) * 4u + 40u);
                (void)(uint32_t)sub_80020400(payload + 8u, buffer);
                context = r_u32(0x800810E8u);
                w_u32(context + 52u, 1u - r_u32(context + 52u));
                break;
            case 1u:
            case 3u:
            case 4u:
                if (kind != 4u)
                {
                    context = r_u32(0x8005B470u);
                    context += r_u8(context + 5u) * 112u;
                    offset_x = (uint32_t)((scalar_v_25 = (uint32_t)r_u16(((uint32_t)(context + 24u)))), (((int32_t)(scalar_v_25 < 32768 ? (int32_t)scalar_v_25 : (int32_t)scalar_v_25 - 65536))));
                    offset_y = (uint32_t)((scalar_v_26 = (uint32_t)r_u16(((uint32_t)(context + 26u)))), (((int32_t)(scalar_v_26 < 32768 ? (int32_t)scalar_v_26 : (int32_t)scalar_v_26 - 65536))));
                }
                x += offset_x;
                format = r_u32(0x800810F0u);
                y += offset_y;
                context = r_u32(0x800810E8u);
                w_u16(context, x);
                w_u16((context + 2u), y);
                w_u16((context + 6u), height);
                w_u16((context + 28u), x);
                w_u16((context + 30u), y);
                w_u16((context + 32u), (format * 8u));
                w_u16((context + 34u), height);
                w_u32(context + 16u, 0u);
                w_u16((context + 4u), ((width * format) >> 1));
                buffer = r_u32(context + 48u);
                (void)(uint32_t)sub_80020400(payload + 8u, buffer);
                value = r_u32(0x800810ECu);
                context = r_u32(0x800810E8u);
                w_u32(context + 0x120u, 0u);
                (void)DecDCTin((uint32_t *)xport_guest_ptr(buffer, 1), ((int32_t)(uint32_t)(value)));
                queue_decoder_output();
                (void)(uint32_t)DecDCToutSync(((int32_t)(uint32_t)(0u)));
                queue_wait_decoder();
                break;
            case 2u:
                context = r_u32(0x8005B470u);
                value = r_u32(0x800810ECu);
                context += r_u8(context + 5u) * 112u;
                offset_x = (uint32_t)((scalar_v_27 = (uint32_t)r_u16(((uint32_t)(context + 24u)))), (((int32_t)(scalar_v_27 < 32768 ? (int32_t)scalar_v_27 : (int32_t)scalar_v_27 - 65536))));
                offset_y = (uint32_t)((scalar_v_28 = (uint32_t)r_u16(((uint32_t)(context + 26u)))), (((int32_t)(scalar_v_28 < 32768 ? (int32_t)scalar_v_28 : (int32_t)scalar_v_28 - 65536))));
                context = r_u32(0x800810E8u);
                format = r_u32(0x800810F0u);
                x += offset_x;
                y += offset_y;
                buffer = r_u32(context + 52u);
                w_u16((context + 6u), height);
                w_u16(context, x);
                w_u16((context + 2u), y);
                w_u16((context + 28u), x);
                w_u16((context + 30u), y);
                w_u16((context + 32u), (format * 8u));
                w_u16((context + 34u), height);
                w_u32(context + 16u, 0u);
                w_u32(context + 0x120u, 0u);
                w_u16((context + 4u), ((width * format) >> 1));
                buffer = r_u32(context + (1u - buffer) * 4u + 40u);
                (void)DecDCTin((uint32_t *)xport_guest_ptr(buffer, 1), ((int32_t)(uint32_t)(value)));
                queue_decoder_output();
                context = r_u32(0x800810E8u);
                buffer = r_u32(context + r_u32(context + 52u) * 4u + 40u);
                (void)(uint32_t)sub_80020400(payload + 8u, buffer);
                (void)(uint32_t)DecDCToutSync(((int32_t)(uint32_t)(0u)));
                queue_wait_decoder();
                context = r_u32(0x800810E8u);
                w_u32(context + 52u, 1u - r_u32(context + 52u));
                break;
            default:
                break;
        }
        if (kind < 5u)
            kind = r_u32(queue + 8u);
        if (kind != 3u)
        {
            value = r_u32(queue + 16u);
            if (value)
                (void)sub_80061C44(value);
        }
        w_u32(queue + 4u, data + size);
    }
}

void sub_80062774(uint32_t queue, uint32_t count)
{
    uint32_t context;
    FUNCTION_MARKER(0x80062774, "RASHCDF.BIN");
    context = r_u32(0x800810E8u);
    if (r_u32(context + 0x130u))
        sub_800627BC(queue, count);
    /* The alternate original branch is the empty nullsub_8 */
}

uint32_t sub_80078BD8(uint32_t list, uint32_t wait)
{
    uint32_t scalar_v_29;
    uint32_t scalar_v_30;

    uint32_t result = 1u, id;
    FUNCTION_MARKER(0x80078BD8, "RASHCDF.BIN");
    id = (uint32_t)((scalar_v_29 = (uint32_t)r_u16(((uint32_t)(list)))), (((int32_t)(scalar_v_29 < 32768 ? (int32_t)scalar_v_29 : (int32_t)scalar_v_29 - 65536))));
    while (id != UINT32_MAX)
    {
        if (!result)
            return result;
        if (!sub_800782B0((int16_t)id, wait))
            result = 0u;
        list += 2u;
        id = (uint32_t)((scalar_v_30 = (uint32_t)r_u16(((uint32_t)(list)))), (((int32_t)(scalar_v_30 < 32768 ? (int32_t)scalar_v_30 : (int32_t)scalar_v_30 - 65536))));
    }
    return result;
}

uint32_t sub_80078C68(uint32_t id, uint32_t force)
{
    uint32_t descriptor, value, flags, data, kind, subtype, slot;
    FUNCTION_MARKER(0x80078C68, "RASHCDF.BIN");
    id = (id & 0xFFFFu) | ((id & 0x8000u) ? 0xFFFF0000u : 0u);
    if (((int32_t)(uint32_t)(id)) >= 37)
        return 1u;
    descriptor = 0x800897E4u + id * 32u;
    if (!force && ((uint32_t)r_u16(descriptor) & 0x1000u))
        return 1u;
    (void)(uint32_t)sub_80022A78(2u);
    value = r_u32(descriptor + 24u);
    w_u32(0x8005ACACu, 0u);
    if (value != UINT32_MAX)
    {
        (void)(uint32_t)sub_8001460C(value);
        w_u32(descriptor + 24u, UINT32_MAX);
    }
    flags = (uint32_t)r_u16(descriptor);
    data = r_u32(descriptor + 4u);
    w_u16(descriptor, (flags & 0xFFF3u));
    if (data)
    {
        if (!(flags & 1u))
            (void)(uint32_t)sub_800144B8(data);
        flags = (uint32_t)r_u16(descriptor);
        w_u32(descriptor + 4u, 0u);
        w_u16(descriptor, (flags & 0xFFFEu));
    }
    kind = r_u8(descriptor + 2u);
    switch (kind)
    {
        case 3u:
            subtype = r_u8(descriptor + 3u);
            if (subtype == 1u || subtype == 2u || subtype == 3u)
            {
                slot = subtype == 1u ? 0x8009C5B8u : subtype == 2u ? 0x8009C5C0u : 0x8009C5BCu;
                value = r_u32(slot);
                w_u32(0x800D807Cu + value * 24u, 0u);
                value = r_u32(slot);
                (void)(uint32_t)sub_8002C9D4(value);
                w_u32(slot, UINT32_MAX);
            }
            break;
        case 4u:
            w_u32(0x8005AE78u, 0u);
            sub_8002C9A8();
            break;
        case 5u:
            w_u8(0x8009C5DFu, 0u);
            (void)sub_8007E9DC();
            break;
        case 6u:
            (void)(uint32_t)sub_8007F158(1u);
            break;
        default:
            break;
    }
    return 1u;
}

uint32_t sub_80078E80(uint32_t list)
{
    uint32_t scalar_v_31;
    uint32_t scalar_v_32;

    uint32_t result = 1u, id;
    FUNCTION_MARKER(0x80078E80, "RASHCDF.BIN");
    id = (uint32_t)((scalar_v_31 = (uint32_t)r_u16(((uint32_t)(list)))), (((int32_t)(scalar_v_31 < 32768 ? (int32_t)scalar_v_31 : (int32_t)scalar_v_31 - 65536))));
    while (id != UINT32_MAX)
    {
        if (!result)
            break;
        result = sub_80078C68(id, 0u);
        list += 2u;
        id = (uint32_t)((scalar_v_32 = (uint32_t)r_u16(((uint32_t)(list)))), (((int32_t)(scalar_v_32 < 32768 ? (int32_t)scalar_v_32 : (int32_t)scalar_v_32 - 65536))));
    }
    return result;
}

uint32_t sub_80065768(uint32_t descriptor)
{
    FUNCTION_MARKER(0x80065768u, "RASHCDF.BIN");
    uint8_t rect[8];
    uint32_t pixels, i;
    for (i = 0; i < 4; ++i)
        xport_store_le16(rect + i * 2, (uint32_t)r_u16((descriptor + 16 + i * 2)));
    pixels = r_u32(descriptor + 4) + 16;
    return rrj_gpu_upload(rect, pixels);
}

/* The count comparison is signed, but address arithmetic wraps at 32 bits.
 * Preserve load/store order even when supplied records alias queue storage. */
static void enqueue(uint32_t descriptor, uint32_t rect, uint32_t count_address, uint32_t base, uint32_t kind)
{
    uint32_t scalar_v_33;
    uint32_t scalar_v_34;

    uint32_t count = r_u32(count_address), q, data, y;
    if (((int32_t)(uint32_t)(count)) >= 6)
        return;
    q = base + count * 36;
    data = r_u32(descriptor + 4);
    w_u32(q + 8, kind);
    w_u32(q + 12, 1);
    w_u32(q + 16, 0);
    w_u32(q, data);
    w_u32(q + 28, (uint32_t)((scalar_v_33 = (uint32_t)r_u16(((uint32_t)(rect)))), (((int32_t)(scalar_v_33 < 32768 ? (int32_t)scalar_v_33 : (int32_t)scalar_v_33 - 65536)))));
    y = (uint32_t)((scalar_v_34 = (uint32_t)r_u16(((uint32_t)(rect + 2)))), (((int32_t)(scalar_v_34 < 32768 ? (int32_t)scalar_v_34 : (int32_t)scalar_v_34 - 65536))));
    w_u32(count_address, count + 1);
    w_u32(q + 32, y);
}

uint32_t sub_8007A400(uint32_t descriptor, uint32_t id)
{
    uint32_t scalar_v_35;
    uint32_t scalar_v_36;

    FUNCTION_MARKER(0x8007A400u, "RASHCDF.BIN");
    uint32_t flags = r_u32(descriptor), rect, x, y;
    if (!(flags & 0x10000000))
        return 0;
    if (flags & 0x40000000)
    {
        if (flags & 0x80000000)
        {
            if (flags & 0x20000000)
                return 1;
            rect = sub_8007A6C0(id, 0);
            enqueue(descriptor, rect, 0x8009C2F0, 0x8009C2F8, 4);
            w_u32(descriptor, r_u32(descriptor) | 0x20000002);
            y = (uint32_t)r_u16((rect + 2));
            x = (uint32_t)r_u16(rect);
            w_u16((descriptor + 28), (((y & 0x100) >> 4) | ((x & 0x3ff) >> 6) | 0x100 | 4 * (y & 0x200)));
            w_u16((descriptor + 24), ((uint32_t)(((scalar_v_35 = (uint32_t)r_u16(((uint32_t)(rect)))), (((int32_t)(scalar_v_35 < 32768 ? (int32_t)scalar_v_35 : (int32_t)scalar_v_35 - 65536)))) % 64)));
            w_u16((descriptor + 26), ((uint32_t)(((scalar_v_36 = (uint32_t)r_u16(((uint32_t)(rect + 2)))), (((int32_t)(scalar_v_36 < 32768 ? (int32_t)scalar_v_36 : (int32_t)scalar_v_36 - 65536)))) % 256)));
            w_u16((descriptor + 30), 0);
            return 1;
        }
        rect = sub_8007A6C0(id, 0);
        enqueue(descriptor, rect, 0x8009C2F4, 0x8009C3D0, 3);
        return 0;
    }
    if (!(flags & 0x20000000))
        (void)sub_80065768(descriptor);
    return 1;
}

static void reset_animation(uint32_t sprite)
{
    w_u8(sprite + 16u, 0u);
    w_u8(sprite + 17u, 0u);
    w_u8(sprite + 18u, 0u);
    w_u8(sprite + 19u, 0u);
}

uint32_t sub_800705DC(uint32_t sprite, uint32_t link, uint32_t command)
{
    uint32_t scalar_v_37;
    uint32_t scalar_v_38;
    uint32_t scalar_v_39;

    uint32_t id, index, descriptor, flags, value, limit;
    uint32_t context, packet, x, y;
    FUNCTION_MARKER(0x800705DC, "RASHCDF.BIN");
    id = r_u32(sprite);
    for (index = 0; index < 300u; ++index)
        if (r_u32(0x80088E0Cu + index * 4u) == id)
            break;
    if (index == 300u)
        return 1u;
    descriptor = 0x8009DDE0u + index * 36u;
    flags = r_u32(descriptor);
    if (!(flags & 0x10000000u))
        return 1u;
    if (flags & 0x00100000u)
    {
        if (r_u32(0x800A0810u) != id)
        {
            w_u32(0x800A0810u, id);
            w_u32(descriptor, r_u32(descriptor) & 0xDFFFFFFFu);
        }
        flags = r_u32(descriptor);
    }
    if (flags & 0x40000000u)
    {
        if (!(flags & 0x80000000u))
        {
            enqueue(descriptor, sprite + 8u, 0x8009C2F4u, 0x8009C3D0u, 3u);
            return 1u;
        }
        if (!(flags & 0x20000000u))
        {
            enqueue(descriptor, descriptor + 16u, 0x8009C2F0u, 0x8009C2F8u, 4u);
            x = (uint32_t)((scalar_v_37 = (uint32_t)r_u16(((uint32_t)(descriptor + 16u)))), (((int32_t)(scalar_v_37 < 32768 ? (int32_t)scalar_v_37 : (int32_t)scalar_v_37 - 65536))));
            value = r_u32(descriptor);
            y = (uint32_t)r_u16((descriptor + 18u));
            w_u32(descriptor, value | 0x20000002u);
            value = (uint32_t)r_u16((descriptor + 16u));
            w_u16((descriptor + 28u), (((y & 0x100u) >> 4) | ((value & 0x3FFu) >> 6) | 0x100u | ((y & 0x200u) << 2)));
            y = (uint32_t)((scalar_v_38 = (uint32_t)r_u16(((uint32_t)(descriptor + 18u)))), (((int32_t)(scalar_v_38 < 32768 ? (int32_t)scalar_v_38 : (int32_t)scalar_v_38 - 65536))));
            w_u16((descriptor + 24u), ((uint32_t)((int32_t)x % 64)));
            w_u16((descriptor + 26u), ((uint32_t)((int32_t)y % 256)));
            w_u16((descriptor + 30u), 0u);
        }
    }
    else if (!(flags & 0x20000000u))
    {
        (void)sub_80065768(descriptor);
        w_u32(descriptor, r_u32(descriptor) | 0x20000000u);
    }
    if (command == 1u || command == 3u)
    {
        value = r_u8(sprite + 20u);
        reset_animation(sprite);
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
                if ((int32_t)value >= ((scalar_v_39 = (uint32_t)r_u16(((uint32_t)(descriptor + 20u)))), (((int32_t)(scalar_v_39 < 32768 ? (int32_t)scalar_v_39 : (int32_t)scalar_v_39 - 65536)))))
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
            reset_animation(sprite);
            w_u8(sprite + 20u, (uint8_t)(flags & 0x5Fu));
        }
        else if (flags & 0x40u)
        {
            value = r_u8(sprite + 20u);
            reset_animation(sprite);
            w_u8(sprite + 20u, (uint8_t)((value & 0x7Fu) | 0x20u));
        }
        else
            return 1u;
    }
    if (!(r_u8(sprite + 20u) & 0x20u))
        w_u8(sprite + 17u, (uint8_t)(r_u8(sprite + 17u) + 1u));
    context = r_u32(0x8005B470u);
    packet = r_u32(context + 268u);
    limit = r_u32(0x8005B4D0u);
    if (packet + 24u >= limit)
    {
        packet = sub_80021C98(packet, 24u);
        context = r_u32(0x8005B470u);
        w_u32(context + 268u, packet);
    }
    context = r_u32(0x8005B470u);
    packet = r_u32(context + 268u);
    w_u32(context + 268u, packet + 24u);
    w_u32(packet, r_u32(link) | 0x05000000u);
    w_u32(packet + 4u, ((uint32_t)r_u16((descriptor + 28u)) & 0x9FFu) | 0xE1000200u);
    w_u16((packet + 18u), ((uint32_t)r_u16((descriptor + 30u))));
    w_u32(packet + 8u, r_u32(sprite + 4u) | 0x64000000u);
    w_u16((packet + 12u), ((uint32_t)r_u16((sprite + 8u))));
    w_u16((packet + 14u), ((uint32_t)r_u16((sprite + 10u))));
    value = r_u8(descriptor + 24u);
    w_u8(packet + 16u, (uint8_t)(value + r_u8(sprite + 18u)));
    value = r_u8(descriptor + 26u);
    w_u8(packet + 17u, (uint8_t)(value + r_u8(sprite + 19u)));
    w_u16((packet + 20u), (r_u8(sprite + 13u)));
    w_u16((packet + 22u), (r_u8(sprite + 14u)));
    w_u32(link, packet);
    return 1u;
}

uint32_t sub_80070580(uint32_t menu, uint32_t entry)
{
    uint32_t scalar_v_40;

    uint32_t slot, table, command;
    FUNCTION_MARKER(0x80070580, "RASHCDF.BIN");
    slot = r_u8(0x8009C5E1u);
    table = r_u32(0x8009CFC8u);
    command = ((scalar_v_40 = (uint32_t)r_u16(((uint32_t)(menu + 2u)))), (((int32_t)(scalar_v_40 < 32768 ? (int32_t)scalar_v_40 : (int32_t)scalar_v_40 - 65536)))) ? 1u : 0u;
    if (slot & 0x80u)
        slot |= 0xFFFFFF00u;
    return sub_800705DC(entry + 16u, table + slot * 4u + 20u, command);
}
