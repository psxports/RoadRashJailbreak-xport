#include "race_global.h"
#include "race_leaf_frontier.h"
#include "race_leaf_batch_000.h"
/* 80012524..80012648: MIPS-audited update dispatcher; callee proof is separate. */
#include "race_step.h"
#include "xport.h"
#include <stdlib.h>

uint32_t sub_80012524(void)
{
    FUNCTION_MARKER(0x80012524u, "SLUS_010.53");
    uint32_t state = r_u32(0x8005B2F8), delta = r_u32(state + 24);
    uint32_t scaled, player, index = 0, count, elapsed;
    if (!delta)
    {
        w_u32(state + 28, 0);
        return 1;
    }
    if (((int32_t)(uint32_t)(delta)) >= 31)
        delta = 30;
    w_u32(state + 28, delta);
    scaled = 218 * delta;
    elapsed = r_u32(state + 16);
    w_u32(0x8005B580, 0);
    w_u32(state + 16, elapsed + delta);
    (void)sub_8008AB00(scaled);
    count = r_u32(r_u32(0x8005B2F8) + 48);
    if (!count)
        return 0;
    player = 0x800CD898;
    do
    {
        uint32_t x = r_u32(player + 184), y = r_u32(player + 188), z = r_u32(player + 192);
        w_u32(player + 476, z);
        w_u32(player + 468, x);
        w_u32(player + 472, y);
        (void)sub_800881B4(player, scaled);
        if ((r_u32(player + 548) & 256) && sub_800A421C(player))
            (void)sub_80086E1C(player);
        count = r_u32(r_u32(0x8005B2F8) + 48);
        ++index;
        player += 1132;
    } while (index < count);
    return 0;
}

uint32_t sub_8001264C(void)
{
    uint32_t player = r_u32(0x8005B3A4);
    uint32_t end = player + 628 * r_u32(0x8005B218);

    FUNCTION_MARKER(0x8001264C, "SLUS_010.53");
    while (player < end)
    {
        uint32_t marker = player + 547;

        if ((int8_t)r_u8(marker) < 0)
        {
            w_u32(player + 480, 0);
            w_u32(player + 184, r_u32(player + 12) << 10);
            w_u32(player + 552, r_u32(player + 552) | 2);
            w_u32(player + 192, r_u32(player + 20) << 10);
        }
        w_u8(marker, 0);
        player += 628;
    }
    player = r_u32(0x800D4B80);
    if (player)
    {
        end = player + 2288;
        while (player < end)
        {
            uint32_t marker = player + 547;

            if ((int8_t)r_u8(marker) < 0)
            {
                if (!(r_u32(player + 388) & 1))
                {
                    w_u32(player + 184, r_u32(player + 12) << 10);
                    w_u32(player + 192, r_u32(player + 20) << 10);
                }
                w_u32(player + 568, r_u32(player + 568) | 2);
            }
            w_u8(marker, 0);
            player += 572;
        }
    }
    return 0;
}
