#include "psx.h"
#include "wip.h"
/* Menu input snapshot and event consumption, audited MIPS branch. */
#include "menu_latch.h"
#include <stdio.h>
#include <stdlib.h>

uint32_t sub_8001E0B4(uint32_t dst, uint32_t src, uint32_t bytes)
{
    FUNCTION_MARKER(0x8001E0B4u, "SLUS_010.53");
    uint32_t result = dst;
    while (bytes)
    {
        uint32_t value = rrj_read32(src);
        src += 4;
        bytes -= 4;
        rrj_write32(dst, value);
        dst += 4;
    }
    return result;
}

void sub_8001CB3C_menu(RRJSDKCall critical)
{
    uint32_t count, player, button, record;
    if (!critical)
        abort();
    critical(rrj_host_context(), 0x80043DA4, 0, 0);
    (void)sub_8001E0B4(0x800D7128, 0x800D6DE0, 768);
    count = rrj_read32(rrj_read32(0x8005B2F8) + 52);
    for (player = 0; rrj_s32(player) < rrj_s32(count); ++player)
    {
        record = 0x800D6DF4 + 192 * player;
        for (button = 0; button < 19; ++button, record += 8)
        {
            uint32_t held = rrj_read32(record);
            w_u8(record + 6, 0);
            if (rrj_s32(held) < 0)
                held = 0;
            rrj_write32(record, held);
        }
    }
    if (r_u8(rrj_read32(0x8005B2F8)) != 2)
    {
        RRJ_WIP(rrj_host_context(), 0x8001CB3C, "function", "input_latch", "skip_non_menu_tail_continue_critical_exit", NULL, 0);
    }
    critical(rrj_host_context(), 0x80043DB4, 0, 0);
}
