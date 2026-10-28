#include "menu_transition.h"
#include "rrj_fmv.h"
#include "rrj_runtime.h"
#include "menu_frame.h"
#include "menu_update.h"
#include "resource.h"
#include "native_game_api.h"
#include "gpu.h"
#include "menu.h"
#include "menu_cleanup.h"
#include "psx.h"
/* F80080274: original menu loop. Callee return registers are all discarded. */
#include "menu_loop.h"
#include <stdlib.h>

uint32_t rrj_menu_prepare_iteration(void)
{
    uint32_t sequence, ctx, tick;
    uint8_t delay;
    (void)sub_8001CB3C();
    (void)sub_8001C428();
    sequence = r_u32(0x8005AE00);
    ctx = r_u32(0x8005B470);
    sequence ^= 1;
    w_u32(0x8005AE00, sequence);
    w_u32(0x8005AE04, sequence);
    tick = r_u32(0x8005B46C);
    w_u32(0x800D75C0 + 4 * sequence, tick);
    w_u32(0x800D75B0 + 4 * sequence, r_u32(ctx + 268) - 60);
    sub_800667E4();
    sub_80066C34();
    delay = r_u8(0x8009C5E2);
    if (delay > 0 && delay < 128)
        w_u8(0x8009C5E2, delay - 1);
    else
    {
        (void)sub_8001C3F4();
        return 1u;
    }
    return 0u;
}

void rrj_menu_iteration(void)
{
    if (rrj_menu_prepare_iteration())
    {
        (void)sub_8001C408();
        w_u32(0x80088C44, 0);
    }
    rrj_menu_finish_iteration();
}

void rrj_menu_finish_iteration(void)
{
    uint32_t count;
    count = r_u32(0x8009C2F0);
    if (count)
        sub_80062774(0x8009C2F8, count);
    if (!rrj_gpu_draw_ot(r_u32(0x8009CCE4) + 284))
        abort();
    sub_8006711C();
    (void)DrawSync(0);
    count = r_u32(0x8009C2F4);
    if (count)
        sub_80062774(0x8009C3D0, count);
    rrj_menu_fmv_present();
    (void)sub_80080488();
    w_u32(0x8009C2F0, 0);
    w_u32(0x8009C2F4, 0);
    sub_800803FC();
}

uint32_t sub_80080274(void)
{
    FUNCTION_MARKER(0x80080274u, "RASHCDF.BIN");
    while (r_u8(r_u32(0x8005B2F8)) == 2)
    {
        if (!rrj_runtime_frame())
            return 0;
        rrj_menu_iteration();
    }
    return 2;
}
