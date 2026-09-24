/* 80012524..80012648: MIPS-audited update dispatcher; callee proof is separate. */
#include "race_step.h"
#include "xport.h"
#include <stdlib.h>

uint32_t sub_80012524(RRJMemory *m, RRJRaceStepCall call)
{
    uint32_t state = rrj_read32(m, 0x8005B2F8), delta = rrj_read32(m, state + 24);
    uint32_t scaled, player, index = 0, count, elapsed;
    if (!delta)
    {
        rrj_write32(m, state + 28, 0);
        return 1;
    }
    if (rrj_s32(delta) >= 31)
        delta = 30;
    rrj_write32(m, state + 28, delta);
    scaled = 218u * delta;
    elapsed = rrj_read32(m, state + 16);
    rrj_write32(m, 0x8005B580, 0);
    rrj_write32(m, state + 16, elapsed + delta);
    if (!call)
        abort();
    (void)call(m, 0x8008AB00, scaled, 0);
    count = rrj_read32(m, rrj_read32(m, 0x8005B2F8) + 48);
    if (!count)
        return 0;
    player = 0x800CD898;
    do
    {
        uint32_t x = rrj_read32(m, player + 184), y = rrj_read32(m, player + 188), z = rrj_read32(m, player + 192);
        rrj_write32(m, player + 476, z);
        rrj_write32(m, player + 468, x);
        rrj_write32(m, player + 472, y);
        (void)call(m, 0x800881B4, player, scaled);
        if ((rrj_read32(m, player + 548) & 256) && call(m, 0x800A421C, player, 0))
            (void)call(m, 0x80086E1C, player, 0);
        count = rrj_read32(m, rrj_read32(m, 0x8005B2F8) + 48);
        ++index;
        player += 1132;
    } while (index < count);
    return 0;
}

uint32_t sub_8001264C(RRJMemory *m)
{
    uint32_t player = rrj_read32(m, 0x8005B3A4);
    uint32_t end = player + 628 * rrj_read32(m, 0x8005B218);

    FUNCTION_MARKER(0x8001264C, "SLUS_010.53");
    while (player < end)
    {
        uint32_t marker = player + 547;

        if ((int8_t)*(uint8_t *)rrj_at(m, marker, 1) < 0)
        {
            rrj_write32(m, player + 480, 0);
            rrj_write32(m, player + 184, rrj_read32(m, player + 12) << 10);
            rrj_write32(m, player + 552, rrj_read32(m, player + 552) | 2);
            rrj_write32(m, player + 192, rrj_read32(m, player + 20) << 10);
        }
        *(uint8_t *)rrj_at(m, marker, 1) = 0;
        player += 628;
    }
    player = rrj_read32(m, 0x800D4B80);
    if (player)
    {
        end = player + 2288;
        while (player < end)
        {
            uint32_t marker = player + 547;

            if ((int8_t)*(uint8_t *)rrj_at(m, marker, 1) < 0)
            {
                if (!(rrj_read32(m, player + 388) & 1))
                {
                    rrj_write32(m, player + 184, rrj_read32(m, player + 12) << 10);
                    rrj_write32(m, player + 192, rrj_read32(m, player + 20) << 10);
                }
                rrj_write32(m, player + 568, rrj_read32(m, player + 568) | 2);
            }
            *(uint8_t *)rrj_at(m, marker, 1) = 0;
            player += 572;
        }
    }
    return 0;
}
