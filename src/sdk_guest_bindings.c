#include "rrj_debug.h"
#include "psx.h"
#include "sdk_build_stubs.h"
#include "wip.h"
#include "race_bodyless_batch_001.h"
#include "native_game_api.h"
#include "race_audio_frontier.h"
#include <stdio.h>
#include <stdlib.h>

/* Bind reviewed SDK guest slots from SLUS_010.53 */
const uint32 xport_cd_ready_callback_address = 0x8005af64u;
const uint32 xport_cd_sync_callback_address = 0x8005af60u;
const uint32 xport_spu_register_pointer_address = 0x8005a41cu;
const uint32 xport_cd_status_address = 0x8005af6cu;
const uint32 xport_cd_setloc_table_address = 0x80054a14u;

void xport_bind_native_spu_transfer(void)
{
    RRJ_DEBUG_FPUTS("RRJ: missing native SPU transfer adapter\n", stderr);
    abort();
}

uint32 xport_gpu_guest_dispatch(uint32 target, uint32 *registers, uint32 known_register_mask)
{
    RRJ_DEBUG_FPRINTF(stderr, "RRJ: missing native GPU dispatch adapter for %08X, mask %08X\n", target, known_register_mask);
    abort();
}

uint32 rrj_missing_spu_read(uint32 destination, uint32 bytes)
{
    RRJ_DEBUG_FPRINTF(stderr, "RRJ: missing SDK _spu_Fr read adapter, destination %08X, bytes %u\n", destination, bytes);
    abort();
}

uint32_t sub_80050B18(uint32_t enabled)
{
    uint32_t base, count;
    FUNCTION_MARKER(0x80050B18, "SLUS_010.53");
    if (!enabled || enabled == 3)
    {
        base = r_u32(0x8005A41C);
        w_u16(base + 426, r_u16(base + 426) & 0xFFBF);
        if (r_u16(base + 426) & 0x40)
        {
            count = 1;
            for (;;)
            {
                if (count >= 0xF01)
                {
                    (void)RRJ_DEBUG_PRINTF((const char *)xport_guest_cptr(0x800115C8, 1), (const char *)xport_guest_cptr(0x800115D8, 1));
                    return UINT32_MAX;
                }
                if (!(r_u16(r_u32(0x8005A41C) + 426) & 0x40))
                    break;
                ++count;
            }
        }
    }
    if (enabled == 1 || enabled == 3)
    {
        base = r_u32(0x8005A41C);
        w_u16(base + 426, r_u16(base + 426) | 0x40);
        if (!(r_u16(base + 426) & 0x40))
        {
            count = 1;
            for (;;)
            {
                if (count >= 0xF01)
                {
                    (void)RRJ_DEBUG_PRINTF((const char *)xport_guest_cptr(0x800115C8, 1), (const char *)xport_guest_cptr(0x800115E8, 1));
                    return UINT32_MAX;
                }
                if (r_u16(r_u32(0x8005A41C) + 426) & 0x40)
                    break;
                ++count;
            }
        }
    }
    return enabled;
}

uint32_t sub_80050C58(uint32_t address)
{
    FUNCTION_MARKER(0x80050C58, "SLUS_010.53");
    if (address > 0x7FFF8)
        return 0;
    return sub_8004F0A8(210, address);
}

uint32 rrj_missing_pad_init_mtap(uint32 first, uint32 second)
{
    return InitPAD(first, 34, second, 34);
}





void rrj_missing_bios_b0_4a(uint32_t mode)
{
    RRJ_DEBUG_FPRINTF(stderr, "RRJ: missing BIOS B0:4A adapter, mode %u\n", mode);
    abort();
}

void rrj_missing_bios_b0_4b(void)
{
    RRJ_DEBUG_FPUTS("RRJ: missing BIOS B0:4B adapter\n", stderr);
    abort();
}

uint32_t rrj_tim_open(uint32_t data)
{
    w_u32(0x800DC078, data);
    return 0;
}

uint8_t *rrj_tim_read(uint8_t output[20])
{
    uint32_t data = r_u32(0x800DC078);
    uint32_t block;
    uint32_t clut_words;
    uint32_t pixel_words;
    uint32_t mode;
    uint32_t length;

    if (r_u32(data) != 16)
        return NULL;
    block = data + 8;
    xport_store_le32(output, r_u32(data + 4));
    if (r_u8(0x80055F6E) == 2)
        (void)RRJ_DEBUG_PRINTF("id  =%08x\n", 16);
    if (r_u8(0x80055F6E) == 2)
        (void)RRJ_DEBUG_PRINTF("mode=%08x\n", (unsigned int)xport_load_le32(output));
    if (r_u8(0x80055F6E) == 2)
        (void)RRJ_DEBUG_PRINTF("timaddr=%08x\n", (unsigned int)block);
    mode = xport_load_le32(output);
    if (mode & 8)
    {
        length = r_u32(block);
        xport_store_le32(output + 4, block + 4);
        xport_store_le32(output + 8, block + 12);
        clut_words = length >> 2;
        block += clut_words << 2;
    }
    else
    {
        clut_words = 0;
        xport_store_le32(output + 4, 0);
        xport_store_le32(output + 8, 0);
    }
    length = r_u32(block);
    xport_store_le32(output + 12, block + 4);
    xport_store_le32(output + 16, block + 12);
    pixel_words = length >> 2;
    length = clut_words + pixel_words + 2;
    if (length == 0xFFFFFFFF)
        return NULL;
    w_u32(0x800DC078, r_u32(0x800DC078) + (length << 2));
    return output;
}

uint32_t rrj_missing_spu_start(void)
{
    return SpuStart();
}

uint32_t rrj_missing_pc_seek(uint32_t handle, uint32_t offset, uint32_t mode)
{
    RRJ_DEBUG_FPRINTF(stderr, "RRJ: missing BREAK 0x107 file seek adapter, handle %08X, offset %08X, mode %u\n", handle, offset, mode);
    abort();
}

/* Adapt game return values to the interrupt callback ABI */
static void rrj_spu_music_interrupt(void)
{
    w_u16(0x80054E06, 1);
    (void)sub_8002169C();
    w_u16(0x80054E06, 0);
}

static void rrj_spu_stream_interrupt(void)
{
    w_u16(0x80054E06, 1);
    (void)sub_8005FE60();
    w_u16(0x80054E06, 0);
}

/* Preserve the original SDK slot before changing interrupt channel 9 */
uint32_t rrj_spu_irq_callback_psx(uint32_t callback)
{
    uint32_t previous = r_u32(0x8005A458);
    PsxInterruptCallback native_callback;
    switch (callback)
    {
        case 0:
            native_callback = NULL;
            break;
        case 0x8002169C:
            native_callback = rrj_spu_music_interrupt;
            break;
        case 0x8005FE60:
            native_callback = rrj_spu_stream_interrupt;
            break;
        default:
            /* TODO Bind additional original SPU IRQ targets */
            RRJ_DEBUG_FPRINTF(stderr, "RRJ: missing SPU IRQ callback target %08X\n", callback);
            abort();
    }
    if (callback != previous)
        w_u32(0x8005A458, callback);
    (void)InterruptCallback(9, native_callback);
    return previous;
}

/* Missing original physical CD header transfer adapter */
/* Preserve the original CdReadCallback guest slot and previous value */
uint32_t rrj_cd_read_callback_psx(uint32_t callback)
{
    uint32_t previous = r_u32(0x80054A04u);
    w_u32(0x80054A04u, callback);
    return previous;
}

void rrj_missing_init_card(uint32_t mode)
{
    RRJ_WIP3(rrj_host_context(), 0x800116B8, "memory_card", mode, 0, 0);
    (void)xport_bios_init_card(mode);
}

void rrj_missing_card_auto(uint32_t mode)
{
    RRJ_WIP3(rrj_host_context(), 0x800116B8, "memory_card", mode, 0, 0);
}

void rrj_missing_start_card(void)
{
    StartCARD();
}

uint32_t rrj_missing_stop_card(void)
{
    return xport_bios_stop_card();
}

uint32_t rrj_file_buffer_address(const void *buffer)
{
    uintptr_t value = (uintptr_t)buffer;
    uintptr_t dram = (uintptr_t)DRAM;
    uintptr_t scratchpad = (uintptr_t)SCRATCHPAD;
    if (value >= dram && value - dram <= PSX_DRAM_SIZE)
        return 0x80000000u + (uint32_t)(value - dram);
    if (value >= scratchpad && value - scratchpad < 1024u)
        return 0x1F800000u + (uint32_t)(value - scratchpad);
    /* TODO Bind physical CD reads into native local buffers */
    RRJ_DEBUG_FPUTS("RRJ: missing physical CD adapter for native local buffer\n", stderr);
    abort();
}

uint32_t rrj_missing_bios_b0_56(void)
{
    RRJ_DEBUG_FPUTS("RRJ: missing BIOS B0:56 table adapter\n", stderr);
    abort();
}

void rrj_missing_spu_fw1ts(void)
{
#if defined(_DEBUG)
    RRJ_DEBUG_FPUTS("RRJ: missing SDK _spu_Fw1ts adapter\n", stderr);
#endif
    abort();
}

void rrj_missing_spu_dma_priority(void)
{
#if defined(_DEBUG)
    RRJ_DEBUG_FPUTS("RRJ: missing SDK SPU_OBJ_AB0 DMA priority adapter\n", stderr);
#endif
    abort();
}

/* Bind nullable native CD DMA callbacks; nonzero guest targets need an adapter */
void rrj_cd_data_callback_psx(uint32_t callback)
{
    PsxInterruptCallback previous;
    if (callback)
    {
#if defined(_DEBUG)
        RRJ_DEBUG_FPRINTF(stderr, "RRJ: missing native CdDataCallback target %08X\n", callback);
#endif
        abort();
    }
    previous = CdDataCallback(NULL);
    if (previous)
    {
#if defined(_DEBUG)
        RRJ_DEBUG_FPUTS("RRJ: missing previous CdDataCallback address binding\n", stderr);
#endif
        abort();
    }
}

uint32_t sub_80047A88(void)
{
    FUNCTION_MARKER(0x80047A88, "SLUS_010.53");
    return r_u16(0x80054E06);
}

uint32_t sub_80045C74(void)
{
    uint32_t result;
    FUNCTION_MARKER(0x80045C74, "SLUS_010.53");
    result = CdSyncCallbackPSX(r_u32(0x80054ABC));
    w_u32(0x80054AB8, 0);
    return result;
}

uint32_t sub_80046E84(uint32_t mode, uint32_t destination)
{
    uint32_t flag;
    FUNCTION_MARKER(0x80046E84, "SLUS_010.53");
    (void)sub_80046844(0x8005B154);
    do
    {
        if (sub_8004687C())
            return 0xFFFFFFFF;
        (void)sub_80046A00();
        flag = r_u8(0x8005B0DA);
        if (flag)
        {
            w_u8(0x8005B0DA, 0);
            sub_800467F4(destination, xport_guest_cptr(0x8005B5C8, 8));
            return flag;
        }
        flag = r_u8(0x8005B0D9);
        if (flag)
        {
            w_u8(0x8005B0D9, 0);
            sub_800467F4(destination, xport_guest_cptr(0x8005B5C0, 8));
            return flag;
        }
    } while (!mode);
    return 0;
}

uint32_t sub_80045BCC(const uint8_t attenuation[4])
{
    FUNCTION_MARKER(0x80045BCC, "SLUS_010.53");
    (void)sub_80047248(attenuation);
    return 1;
}

/* Mirror the original interrupt handler CheckCallback lifetime */
static void rrj_cd_native_interrupt(void)
{
    w_u16(0x80054E06, 1);
    (void)sub_80046948();
    w_u16(0x80054E06, 0);
}

uint32_t rrj_cd_irq_callback_psx(void)
{
    PsxInterruptCallback previous = InterruptCallback(2, rrj_cd_native_interrupt);
    if (!previous)
        return 0;
    if (previous == rrj_cd_native_interrupt)
        return 0x80046948;
    /* TODO Bind additional previous native CD IRQ handlers */
    RRJ_DEBUG_FPUTS("RRJ: missing previous CD IRQ callback identity adapter\n", stderr);
    abort();
}

uint32_t rrj_missing_sys_deq_int_rp(uint32_t priority, uint32_t node)
{
    RRJ_DEBUG_FPRINTF(stderr, "RRJ: missing BIOS SysDeqIntRP C0:03, priority %u, node %08X\n", priority, node);
    abort();
}

uint32_t rrj_missing_sys_enq_int_rp(uint32_t priority, uint32_t node)
{
    RRJ_DEBUG_FPRINTF(stderr, "RRJ: missing BIOS SysEnqIntRP C0:02, priority %u, node %08X\n", priority, node);
    abort();
}

uint32_t sub_80044E94(uint32_t first_buffer, uint32_t first_size, uint32_t second_buffer, uint32_t second_size)
{
    FUNCTION_MARKER(0x80044E94, "SLUS_010.53");
    return rrj_missing_bios_b0_12(first_buffer, first_size, second_buffer, second_size);
}

uint32_t sub_80044EA4(void)
{
    FUNCTION_MARKER(0x80044EA4, "SLUS_010.53");
    return rrj_missing_bios_b0_13();
}

uint32_t sub_80044EB4(void)
{
    FUNCTION_MARKER(0x80044EB4, "SLUS_010.53");
    return rrj_missing_bios_b0_14();
}

uint32_t sub_80044EC4(uint32_t type, uint32_t button_destination)
{
    FUNCTION_MARKER(0x80044EC4, "SLUS_010.53");
    return rrj_missing_bios_b0_15(type, button_destination);
}

uint32_t sub_80045004(void)
{
    FUNCTION_MARKER(0x80045004, "SLUS_010.53");
    return rrj_missing_bios_b0_4c();
}

uint32_t rrj_missing_bios_b0_12(uint32_t first_buffer, uint32_t first_size, uint32_t second_buffer, uint32_t second_size)
{
    RRJ_DEBUG_FPRINTF(stderr, "RRJ: missing BIOS B0:12 InitPAD2 adapter, buffers %08X/%08X, sizes %u/%u\n", first_buffer, second_buffer, first_size, second_size);
    abort();
}

uint32_t rrj_missing_bios_b0_13(void)
{
    RRJ_DEBUG_FPUTS("RRJ: missing BIOS B0:13 StartPAD2 adapter\n", stderr);
    abort();
}

uint32_t rrj_missing_bios_b0_14(void)
{
    RRJ_DEBUG_FPUTS("RRJ: missing BIOS B0:14 StopPAD2 adapter\n", stderr);
    abort();
}

uint32_t rrj_missing_bios_b0_15(uint32_t type, uint32_t button_destination)
{
    RRJ_DEBUG_FPRINTF(stderr, "RRJ: missing BIOS B0:15 PAD_init2 adapter, type %08X, destination %08X\n", type, button_destination);
    abort();
}

uint32_t rrj_missing_bios_b0_4c(void)
{
    RRJ_DEBUG_FPUTS("RRJ: missing BIOS B0:4C StopCARD2 adapter\n", stderr);
    abort();
}

void *sub_800494E8(void *environment)
{
    uint32_t index;
    uint8_t *output = (uint8_t *)environment;
    FUNCTION_MARKER(0x800494E8, "SLUS_010.53");
    if (output)
        for (index = 0; index < 20; ++index)
            output[index] = r_u8(0x80055FD8 + index);
    return environment;
}

void rrj_missing_bios_b0_19(uint32_t context)
{
    RRJ_DEBUG_FPRINTF(stderr, "RRJ: missing BIOS B0:19 adapter, context %08X\n", context);
    abort();
}
