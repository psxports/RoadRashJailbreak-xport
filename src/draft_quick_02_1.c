#include "rrj_debug.h"
#include "fixed_math.h"
#include "sdk_build_stubs.h"
#include <stdio.h>
#include <stdlib.h>
#include "psx.h"
#include "native_game_api.h"
#include "draft_quick.h"
#include "race_bodyless_batch_002.h"

/* Unverified draft translations of SLUS_010.53 */
uint32_t sub_8001C178(void)
{
    uint32_t context, resource;
    FUNCTION_MARKER(0x8001C178, "SLUS_010.53");
    context = r_u32(0x8005B470);
    resource = r_u32(context + 240);
    return resource ? sub_800144B8(resource) : context;
}

uint32_t sub_800153F0(uint32_t a1, uint32_t a2, uint32_t a3)
{
    FUNCTION_MARKER(0x800153F0u, "SLUS_010.53");
    if ((a1 >> 16) == 1)
        return 1;
    return sub_8001556C(a2 + (uint32_t)((int32_t)a3 / 2048));
}

uint32_t sub_80014F30(uint32_t a1, uint32_t a2, uint32_t a3)
{
    FUNCTION_MARKER(0x80014F30u, "SLUS_010.53");
    uint32_t result = sub_80014E5C(a1, a2, a3);
    if (result != UINT32_MAX)
    {
        uint32_t record = 0x800D65D0u + 20 * a1;
        sub_800153F0(r_u32(record + 16), r_u32(record + 4), r_u32(record + 8));
    }
    return result;
}

uint32_t sub_80047F58(void)
{
    uint32_t interrupt_mask, dma_control;
    uint16_t saved_mask;
    FUNCTION_MARKER(0x80047F58, "SLUS_010.53");
    if (r_u16(0x80054E04))
        return 0;
    rrj_missing_bios_b0_19(0x80054E3C);
    interrupt_mask = r_u32(0x80055E94);
    saved_mask = r_u16(0x80054E36);
    w_u16(0x80054E04, 1);
    w_u16(interrupt_mask, saved_mask);
    dma_control = r_u32(0x80055E98);
    w_u32(dma_control, r_u32(0x80054E38));
    sub_80043DB4();
    return 0x80054E04;
}

typedef uint32_t (*RRJ_RESTART_CALLBACK)(void);

static RRJ_RESTART_CALLBACK restart_callback_function(uint32_t address)
{
    if (address == 0x80047F58)
        return sub_80047F58;
    RRJ_DEBUG_FPRINTF(stderr, "RRJ: missing restart callback target %08X\n", address);
    abort();
}

uint32_t sub_80047A58(void)
{
    RRJ_RESTART_CALLBACK function;
    FUNCTION_MARKER(0x80047A58, "SLUS_010.53");
    function = restart_callback_function(r_u32(r_u32(0x80055E8C) + 24));
    return function();
}

uint32_t sub_80049C60(void)
{
    FUNCTION_MARKER(0x80049C60, "SLUS_010.53");
    return r_u32(r_u32(0x80056074));
}

typedef uint32_t (*RRJ_GPU_STATUS_READ)(void);

static RRJ_GPU_STATUS_READ gpu_status_read_function(uint32_t address)
{
    if (address == 0x80049C60)
        return sub_80049C60;
    RRJ_DEBUG_FPRINTF(stderr, "RRJ: missing GPU status reader at %08X\n", address);
    abort();
}

uint32_t sub_8004951C(void)
{
    RRJ_GPU_STATUS_READ function;
    FUNCTION_MARKER(0x8004951C, "SLUS_010.53");
    function = gpu_status_read_function(r_u32(r_u32(0x80055F64) + 56));
    return function() >> 31;
}

uint32_t sub_800408C8(uint32_t port, uint32_t mode, uint32_t value)
{
    uint32_t owner;
    FUNCTION_MARKER(0x800408C8, "SLUS_010.53");
    owner = rrj_pad_function(r_u32(0x80054910))(port);
    return sub_8004213C(owner, mode & 255, value & 255);
}

uint32_t sub_80047654(uint32_t a1, uint32_t a2)
{
    FUNCTION_MARKER(0x80047654u, "SLUS_010.53");
    uint32_t p;
    w_u8(r_u32(0x8005B0C0u), 0);
    w_u8(r_u32(0x8005B0CCu), 128);
    w_u32(r_u32(0x8005B1D0u), 0x21020843u);
    w_u32(r_u32(0x8005B0D0u), 0x1325u);
    p = r_u32(0x8005B1D4u);
    w_u32(p, r_u32(p) | 0x8000u);
    w_u32(r_u32(0x8005B1D8u), a1);
    w_u32(r_u32(0x8005B1DCu), a2 | 0x10000u);
    p = r_u32(0x8005B0C0u);
    while (!(r_u8(p) & 64))
    {
    }
    w_u32(r_u32(0x8005B1E0u), 0x11400100u);
    /* Preserve the final hardware read omitted by pseudocode */
    r_u32(r_u32(0x8005B1E0u));
    return 0;
}

uint32_t sub_80022F40(void)
{
    FUNCTION_MARKER(0x80022F40u, "SLUS_010.53");
    uint8_t mode = 0x80;
    (void)CdControl(14, &mode, NULL);
    return (uint32_t)VSync(3);
}

uint32_t sub_80040964(uint32_t a1)
{
    FUNCTION_MARKER(0x80040964, "SLUS_010.53");
    uint32_t second = r_u32(0x80054950);
    uint32_t first = r_u32(0x8005494C);
    uint32_t result = (second << 1) | (first == 0);
    if (result != a1)
    {
        uint32_t arg;
        w_u32(0x80054934u, 0);
        if (a1 & 1)
        {
            w_u32(0x8005494Cu, 0);
            if ((int32_t)r_u32(0x800D9CC0u) >= 150)
            {
                arg = r_u32(0x80054930u);
                (void)rrj_pad_function(r_u32(0x80054900))(arg);
            }
            w_u32(0x800D9CC0u, 0);
        }
        else
            w_u32(0x8005494Cu, 1);
        if (a1 & 2)
        {
            w_u32(0x80054950u, 1);
            if ((int32_t)r_u32(0x800D9CC4u) >= 150)
            {
                arg = r_u32(0x80054930u) + 240;
                (void)rrj_pad_function(r_u32(0x80054900))(arg);
            }
            w_u32(0x800D9CC4u, 0);
        }
        else
            w_u32(0x80054950u, 0);
        w_u32(0x80054934u, 1);
    }
    return result;
}
