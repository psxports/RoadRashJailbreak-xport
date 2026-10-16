#include "rrj_debug.h"
#include "sdk_build_stubs.h"
#include "psx.h"
#include <stdio.h>
#include <stdlib.h>
#include "native_game_api.h"
#include "draft_quick.h"
#include "race_bodyless_batch_002.h"

/* Unverified quick drafts for SLUS_010.53 */
uint32_t sub_800145EC(const char *a1, uint32_t a2)
{
    FUNCTION_MARKER(0x800145ECu, "SLUS_010.53");
    return sub_8001458C(a1, a2);
}

uint32_t sub_80015198(void)
{
    FUNCTION_MARKER(0x80015198u, "SLUS_010.53");
    RRJ_DEBUG_PRINTF((const char *)xport_guest_cptr(0x80010938u, 45));
    return 0xffffffffu;
}

uint32_t sub_800154FC(void)
{
    FUNCTION_MARKER(0x800154FCu, "SLUS_010.53");
    CdReadyCallbackPSX(0);
    w_u32(0x800D6724u, 0xffffffffu);
    w_u32(0x800D6728u, 0);
    return 0x800D6710u;
}

uint32_t *sub_8002F4D8(uint32_t first, uint32_t second, uint32_t third, uint32_t interval, uint32_t next_interval, uint32_t *constant, uint32_t *previous, uint32_t *diagonal, uint32_t *following)
{
    uint32_t numerator, denominator, ratio, magnitude, reciprocal, a, b, c;
    FUNCTION_MARKER(0x8002F4D8, "SLUS_010.53");
    numerator = (int32_t)next_interval <= 0 ? 0 - next_interval : next_interval;
    denominator = (int32_t)interval <= 0 ? 0 - interval : interval;
    ratio = sub_80010028(numerator, denominator);
    if (((int32_t)next_interval > 0) != ((int32_t)interval > 0))
        ratio = 0 - ratio;
    magnitude = (int32_t)ratio < 0 ? 0 - ratio : ratio;
    denominator = (uint32_t)((int32_t)magnitude >> 1) + (uint32_t)((int32_t)(magnitude - 2) >> 31);
    reciprocal = denominator ? 0x80000000 / denominator : UINT32_MAX;
    if ((int32_t)ratio < 0)
        reciprocal = 0 - reciprocal;
    a = (uint32_t)sub_8001FC90((int32_t)first, (int32_t)ratio);
    b = (uint32_t)sub_8001FC90((int32_t)second, (int32_t)(reciprocal - ratio));
    c = (uint32_t)sub_8001FC90((int32_t)third, (int32_t)reciprocal);
    *constant = 3 * (a + b - c);
    *previous = next_interval;
    *diagonal = (interval + next_interval) << 1;
    *following = interval;
    return following;
}

uint32_t sub_8003BA4C(uint32_t owner, uint32_t id, uint32_t output, uint32_t weights, uint32_t limit)
{
    FUNCTION_MARKER(0x8003BA4Cu, "SLUS_010.53");
    uint32_t descriptor = (uint32_t)sub_8003B9A8(owner, 0);
    uint32_t object, metadata, count, index;
    uint32_t *links;
    int32_t capacity;
    if (!descriptor)
        return 0;
    object = r_u32(descriptor + 12);
    if (!object)
        return 0;
    metadata = (uint32_t)sub_80039B60(r_u32(object));
    if (!metadata)
        return 0;
    /* Allocate the complete metadata count to repair the original four-slot overflow */
    capacity = r_s16(metadata + 8);
    links = malloc((size_t)(capacity > 0 ? capacity : 1) * sizeof(*links));
    if (!links)
        abort();
    count = sub_80039BB0(metadata, id, links, limit);
    if ((int32_t)count < 0 || (int32_t)limit < (int32_t)count)
    {
        free(links);
        return 0;
    }
    for (index = 0; (int32_t)index < (int32_t)count; ++index)
    {
        uint32_t link = links[index];
        uint32_t record = (uint32_t)sub_80039C38(link);
        uint32_t section, segment, vertex, weight;
        int32_t vertex_index, direction;
        if (!record)
        {
            free(links);
            return 0;
        }
        section = r_u32(object + 44) + ((uint32_t)(int32_t)r_s16(record + 2) << 5);
        segment = r_u32(object + 48) + 28u * ((uint32_t)(int32_t)r_s16(section + 20) + (uint32_t)(int32_t)r_s16(record + 4));
        vertex_index = r_s16(segment + 8);
        direction = r_s16(record + 8);
        if (r_s16(link + 6) > 0)
        {
            if (direction <= 0)
                vertex_index = (int32_t)((uint32_t)vertex_index - 1u + (uint32_t)(int32_t)r_s16(segment + 10));
            weight = (uint32_t)direction;
        }
        else
        {
            if (direction > 0)
                vertex_index = (int32_t)((uint32_t)vertex_index - 1u + (uint32_t)(int32_t)r_s16(segment + 10));
            weight = 0u - (uint32_t)direction;
        }
        vertex = r_u32(object + 52) + 52u * (uint32_t)vertex_index;
        w_u32(weights, weight);
        w_u32(output + 20, 0);
        w_u32(output + 16, 0);
        w_u32(output, object);
        w_u32(output + 4, section);
        w_u32(output + 8, segment);
        w_u32(output + 12, vertex);
        w_u32(output + 24, metadata);
        w_u32(output + 28, link);
        output += 32;
        weights += 4;
    }
    free(links);
    return count;
}

uint32_t sub_80040714(uint32_t port, int32_t index, uint32_t field)
{
    uint32_t owner, count, row;
    FUNCTION_MARKER(0x80040714, "SLUS_010.53");
    owner = rrj_pad_function(r_u32(0x80054910))(port);
    count = r_u8(owner + 233);
    if (index < 0)
        return count;
    if (index >= (int32_t)count)
        return 0;
    row = r_u32(owner + 4) + 5 * (uint32_t)index;
    if (field - 1 >= 5)
        return 0;
    return r_u8(row + field - 1);
}

uint32_t sub_80040D80(void)
{
    FUNCTION_MARKER(0x80040D80u, "SLUS_010.53");
    sub_80043DA4();
    (uint32_t)ChangeClearRCnt((uint32)(3), (uint32)(1));
    uint32_t result = (uint32_t)SysDeqIntRP(2u, 0x800D9CB0u);
    sub_80043DB4();
    return result;
}



void sub_80045234(void)
{
    FUNCTION_MARKER(0x80045234u, "SLUS_010.53");
    uint32_t object, source;
    sub_80043DA4();
    object = rrj_missing_bios_b0_56();
    object = r_u32(object + 24);
    for (source = 0x800452A4u; source != 0x800452B0u; source += 4)
    {
        w_u32(object + 112, r_u32(source));
        object += 4;
    }
    FlushCache();
    sub_80043DB4();
}

uint32_t sub_80047AB0(uint32_t mask)
{
    uint32_t address, previous;
    FUNCTION_MARKER(0x80047AB0, "SLUS_010.53");
    address = r_u32(0x80055E94);
    previous = r_u16(address);
    w_u16(address, (uint16_t)mask);
    return previous;
}

uint32_t sub_80048340(uint32_t channel, uint32_t callback)
{
    uint32_t slot, previous, control_address, control, bit;
    FUNCTION_MARKER(0x80048340, "SLUS_010.53");
    slot = 0x80055EE0 + 4 * channel;
    previous = r_u32(slot);
    if (callback == previous)
        return previous;
    control_address = r_u32(0x80055EDC);
    w_u32(slot, callback);
    control = r_u32(control_address) & 0xFFFFFF;
    bit = 1u << ((channel + 16) & 31);
    control |= 0x800000;
    if (callback)
        control |= bit;
    else
        control &= ~bit;
    w_u32(control_address, control);
    return previous;
}

typedef uint32_t (*RRJ_DMA_IRQ_SET)(uint32_t channel, uint32_t callback);

static RRJ_DMA_IRQ_SET dma_irq_set_function(uint32_t address)
{
    if (address == 0x80048340)
        return sub_80048340;
    RRJ_DEBUG_FPRINTF(stderr, "RRJ: missing DMA IRQ setter at %08X\n", address);
    abort();
}

uint32_t sub_80047994(uint32_t channel, uint32_t callback)
{
    RRJ_DMA_IRQ_SET function;
    FUNCTION_MARKER(0x80047994, "SLUS_010.53");
    function = dma_irq_set_function(r_u32(r_u32(0x80055E8C) + 4));
    return function(channel, callback);
}

uint32_t sub_8004B324(void)
{
    FUNCTION_MARKER(0x8004B324, "SLUS_010.53");
    return sub_80047994(2, 0x8004A808);
}

uint32_t sub_8004A4BC(uint32_t ordering_table)
{
    uint32_t control;
    FUNCTION_MARKER(0x8004A4BC, "SLUS_010.53");
    w_u32(r_u32(0x80056074), 0x04000002);
    w_u32(r_u32(0x80056078), ordering_table);
    w_u32(r_u32(0x8005607C), 0);
    control = r_u32(0x80056080);
    w_u32(control, 0x01000401);
    return control;
}

typedef uint32_t (*RRJ_GPU_OT_SUBMIT)(uint32_t ordering_table);

static RRJ_GPU_OT_SUBMIT gpu_ot_submit_function(uint32_t address)
{
    if (address == 0x8004A4BC)
        return sub_8004A4BC;
    RRJ_DEBUG_FPRINTF(stderr, "RRJ: missing GPU ordering-table submitter at %08X\n", address);
    abort();
}

uint32_t sub_8004AD28(void)
{
    uint32_t field, deadline, count, gpu_address, head, tail, dma_address_pointer;
    uint32_t dma_address, dma_control_pointer, gpu_status, dma_control, previous_mask, value;
    FUNCTION_MARKER(0x8004AD28, "SLUS_010.53");
    field = (uint32_t)VSync(-1);
    deadline = r_u32(0x800560A8);
    if ((int32_t)deadline >= (int32_t)field)
    {
        count = r_u32(0x800560AC);
        w_u32(0x800560AC, count + 1);
        if ((int32_t)count <= 0xF0000)
            return 0;
    }
    gpu_address = r_u32(0x80056074);
    (void)r_u32(gpu_address);
    head = r_u32(0x80056094);
    dma_address_pointer = r_u32(0x80056078);
    tail = r_u32(0x80056098);
    dma_address = r_u32(dma_address_pointer);
    dma_control_pointer = r_u32(0x80056080);
    gpu_status = r_u32(gpu_address);
    dma_control = r_u32(dma_control_pointer);
    (void)RRJ_DEBUG_PRINTF((const char *)xport_guest_cptr(0x800112A8, 1), (head - tail) & 63, gpu_status, dma_control, dma_address);
    previous_mask = sub_80047AB0(0);
    w_u32(0x80056098, 0);
    value = r_u32(0x80056098);
    w_u32(0x800560A4, previous_mask);
    w_u32(0x80056094, value);
    w_u32(r_u32(0x80056080), 0x401);
    dma_control_pointer = r_u32(0x80056090);
    w_u32(dma_control_pointer, r_u32(dma_control_pointer) | 0x800);
    w_u32(r_u32(0x80056074), 0x02000000);
    w_u32(r_u32(0x80056074), 0x01000000);
    (void)sub_80047AB0(r_u32(0x800560A4));
    return UINT32_MAX;
}

uint32_t sub_8004B228(uint32_t ordering_table)
{
    uint32_t status, control_address;
    RRJ_GPU_OT_SUBMIT submit;
    FUNCTION_MARKER(0x8004B228, "SLUS_010.53");
#ifndef NDEBUG
    if (r_u8(0x80055F6E) >= 2)
    {
        RRJ_SDK_PRINTF print = rrj_sdk_printf_function(r_u32(0x80055F68));
        (void)print((const char *)xport_guest_cptr(0x80011248, 1), ordering_table);
    }
#endif
    status = (uint32_t)VSync(-1);
    control_address = r_u32(0x80056080);
    w_u32(0x800560A8, status + 240);
    w_u32(0x800560AC, 0);
    status = r_u32(control_address);
    for (;;)
    {
        if (!(status & 0x01000000) && (r_u32(r_u32(0x80056074)) & 0x04000000))
            break;
        if (sub_8004AD28())
            return UINT32_MAX;
        status = r_u32(r_u32(0x80056080));
    }
    (void)sub_80047994(2, 0x8004B324);
    submit = gpu_ot_submit_function(r_u32(r_u32(0x80055F64) + 24));
    (void)submit(ordering_table);
    return 0;
}

uint32_t sub_8004EC54(uint32_t source, uint32_t transfer_address, uint32_t blocks)
{
    uint32_t control;
    FUNCTION_MARKER(0x8004EC54, "SLUS_010.53");
    control = r_u32(0x8005A41C);
    w_u16(control + 422, transfer_address);
    rrj_missing_spu_fw1ts();
    control = r_u32(0x8005A41C);
    w_u16(control + 426, r_u16(control + 426) | 0x30);
    rrj_missing_spu_fw1ts();
    rrj_missing_spu_dma_priority();
    w_u32(r_u32(0x8005A420), source);
    w_u32(r_u32(0x8005A424), (blocks << 16) | 0x10);
    w_u32(0x8005A46C, 1);
    w_u32(r_u32(0x8005A428), 0x01000200);
    return 1;
}
