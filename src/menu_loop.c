/* F80080274: original menu loop. Callee return registers are all discarded. */
#include "menu_loop.h"
#include <stdlib.h>

uint32_t rrj_menu_prepare_iteration(RRJMemory *m, RRJLoopCall call)
{
    uint32_t sequence, ctx, tick;
    uint8_t delay;
    if (!call)
        abort();
    call(m, 0x8001CB3C, 0, 0);
    call(m, 0x8001C428, 0, 0);
    sequence = rrj_read32(m, 0x8005AE00);
    ctx = rrj_read32(m, 0x8005B470);
    sequence ^= 1;
    rrj_write32(m, 0x8005AE00, sequence);
    rrj_write32(m, 0x8005AE04, sequence);
    tick = rrj_read32(m, 0x8005B46C);
    rrj_write32(m, 0x800D75C0 + 4 * sequence, tick);
    rrj_write32(m, 0x800D75B0 + 4 * sequence, rrj_read32(m, ctx + 268) - 60);
    call(m, 0x800667E4, ctx, tick);
    call(m, 0x80066C34, 0, 0);
    delay = r_u8(0x8009C5E2);
    if (delay > 0 && delay < 128)
        w_u8(0x8009C5E2, delay - 1);
    else
    {
        call(m, 0x8001C3F4, 0, 0);
        return 1u;
    }
    return 0u;
}

void rrj_menu_iteration(RRJMemory *m, RRJLoopCall call)
{
    if (rrj_menu_prepare_iteration(m, call))
    {
        call(m, 0x8001C408, 0, 0);
        rrj_write32(m, 0x80088C44, 0);
    }
    rrj_menu_finish_iteration(m, call);
}

void rrj_menu_finish_iteration(RRJMemory *m, RRJLoopCall call)
{
    uint32_t count;
    if (!call)
        abort();
    count = rrj_read32(m, 0x8009C2F0);
    if (count)
        call(m, 0x80062774, 0x8009C2F8, count);
    call(m, 0x80048DB4, rrj_read32(m, 0x8009CCE4) + 284, 0);
    call(m, 0x8006711C, 0, 0);
    call(m, 0x800487C0, 0, 0);
    count = rrj_read32(m, 0x8009C2F4);
    if (count)
        call(m, 0x80062774, 0x8009C3D0, count);
    call(m, 0x80080488, 0, 0);
    rrj_write32(m, 0x8009C2F0, 0);
    rrj_write32(m, 0x8009C2F4, 0);
    call(m, 0x800803FC, 0, 0);
}

uint32_t sub_F_80080274(RRJMemory *m, RRJLoopCall call)
{
    while (r_u8(rrj_read32(m, 0x8005B2F8)) == 2)
        rrj_menu_iteration(m, call);
    return 2;
}
