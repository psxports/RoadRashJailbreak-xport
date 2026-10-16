#include "native_file_callbacks.h"
#include "native_game_api.h"
#include "race_leaf.h"
#include "race_bodyless_batch_004.h"
#include "psx.h"
#include "gpu.h"
#include "race_audio_frontier.h"
#include "input.h"
/* Original resident/menu VBlank counters and display-ready gates. */
#include "vblank.h"
#include "race_trace_runtime.h"
#include <stdlib.h>

typedef char RRJVBlankDrawEnvLayout[(sizeof(DRAWENV) == 92) ? 1 : -1];
typedef char RRJVBlankDispEnvLayout[(sizeof(DISPENV) == 20) ? 1 : -1];

uint32_t sub_800600F8(void)
{
    FUNCTION_MARKER(0x800600F8u, "RASHCDF.BIN");
    uint32_t data = r_u32(0x800810BC), result = r_u32(data + 31520);
    if (result)
    {
        w_u32(data + 31524, r_u32(data + 31524) + 256);
        result = r_u32(data + 31528) + 1;
        w_u32(data + 31528, result);
    }
    return result;
}

uint32_t sub_8001B700(void)
{
    FUNCTION_MARKER(0x8001B700u, "SLUS_010.53");
    uint32_t state = r_u32(0x8005B2F8), frames = r_u32(0x8005B46C), ticks = r_u32(state + 12), count, ctx, result;
    uint32_t cd_callback;
    w_u32(0x8005B46C, frames + 1);
    cd_callback = rrj_trace_vblank_boundary(rrj_host_context());
    if (cd_callback)
        (void)rrj_file_function(cd_callback)();
    count = r_u32(state + 100);
    w_u32(state + 12, ticks + 5);
    ctx = r_u32(0x8005B470);
    w_u32(state + 100, count + 1);
    if (!(uint32_t)r_u8((ctx + 4)) && !(uint32_t)DrawSync(1))
    {
        ctx = r_u32(0x8005B470);
        (void)PutDrawEnv((DRAWENV *)xport_guest_ptr(ctx + 112 * (uint32_t)r_u8((ctx + 6)) + 16, sizeof(DRAWENV)));
        ctx = r_u32(0x8005B470);
        (void)PutDispEnv((DISPENV *)xport_guest_ptr(ctx + 112 * (uint32_t)r_u8((ctx + 5)) + 108, sizeof(DISPENV)));
        ctx = r_u32(0x8005B470);
        w_u8((ctx + 6), (uint8_t)(1 - (uint32_t)r_u8((ctx + 6))));
        ctx = r_u32(0x8005B470);
        w_u8((ctx + 5), (uint8_t)(1 - (uint32_t)r_u8((ctx + 5))));
        w_u8((r_u32(0x8005B470) + 4), (uint8_t)1);
        if (r_u32(0x8005B588) == 1)
        {
            uint32_t ot = r_u32(0x8005B59C);
            w_u32(0x8005B588, 2);
            if (!rrj_gpu_draw_ot(ot + 16))
                abort();
        }
    }
    (void)sub_80019990();
    if (r_u32(r_u32(0x8005B2F8) + 100) & 1)
        rrj_input_read();
    result = r_u32(0x8005AD88);
    if (result)
        result = (uint32_t)sub_8001B8BC();
    return result;
}

uint32_t sub_80064C30(void)
{
    FUNCTION_MARKER(0x80064C30u, "RASHCDF.BIN");
    uint32_t counter = r_u32(0x80088C40), ctx = r_u32(0x8005B470), suppress = 0;
    w_u32(0x80088C40, counter + 1);
    w_u32(0x80088C44, r_u32(0x80088C44) + 1);
    if (!(uint32_t)r_u8((ctx + 4)) && (r_u16(0x8009C5DA) & 1) && r_u32(0x80088C44) < 4)
    {
        w_u8((ctx + 4), (uint8_t)1);
        suppress = 1;
    }
    else
    {
        ctx = r_u32(0x8005B470);
        if (!(uint32_t)r_u8((ctx + 4)) && r_u32(0x80088C44) < 2)
        {
            w_u8((ctx + 4), (uint8_t)1);
            suppress = 1;
        }
    }
    (void)sub_8001B700();
    if (suppress)
        w_u8((r_u32(0x8005B470) + 4), (uint8_t)0);
    w_u32(0x8005ACAC, r_u32(0x8005ACAC) + 1);
    w_u32(0x80088C50, r_u32(0x80088C50) + 1);
    w_u32(0x80088C4C, r_u32(0x80088C4C) + 1);
    if (r_u32(0x80088C48))
        w_u32(0x80088C48, r_u32(0x80088C48) - 1);
    return sub_800600F8();
}

RRJ_VBLANK_FUNCTION rrj_vblank_function(uint32_t address)
{
    switch (address)
    {
        case 0x8001B700:
            return sub_8001B700;
        case 0x80064C30:
            return sub_80064C30;
        default:
            abort();
    }
}

void sub_80064BE4(void)
{
    FUNCTION_MARKER(0x80064BE4, "RASHCDF.BIN");
    w_u32(0x80088C40, 0);
    w_u32(0x80088C44, 0);
    sub_80043DA4();
    (void)sub_8001B564();
    (void)sub_8001B594(0x80064C30);
    sub_80043DB4();
}
