#include "race_services.h"
#include "race_leaf_frontier.h"
#include "race_leaf_batch_002.h"
#include "race_leaf_batch_007.h"
#include "psx.h"
/* G8008AB00: global update sequencing. Adjacent eight-byte entries are real
 * fallthrough wrappers, not empty functions; their native bindings are WIP. */
#include "race_global.h"
#include <stdlib.h>

uint32_t sub_8008AD40(uint32_t delta, uint32_t player)
{
    uint32_t scalar_a_5;
    uint32_t scalar_a_6;
    uint32_t scalar_a_7;

    FUNCTION_MARKER(0x8008AD40u, "RASHCDG.BIN");
    uint32_t other, flags, timer, state;
    if (!((uint32_t)r_u8((r_u32(player + 1084))) & 64))
        return 1;
    other = r_u32(0x8005B21C);
    w_u32(player + 720, 0xffff0000);
    if (other)
        w_u32(other + 720, 0xffff0000);
    player = r_u32(0x8005B38Cu);
    if ((uint32_t)r_u8((r_u32(player + 1084))) & 32)
    {
        w_u32(r_u32(0x8005B2F8) + 16, 0);
        flags = r_u32(player + 1084);
        w_u32(0x8005B230, 196608);
        w_u8(flags, (uint8_t)((uint32_t)r_u8(flags) & 223));
        other = r_u32(0x8005B21C);
        if (other)
            ((scalar_a_5 = (r_u32(other + 1084))), (w_u8(scalar_a_5, (uint8_t)((uint32_t)r_u8(scalar_a_5) & 223))));
        return 0;
    }
    timer = r_u32(0x8005B230) - delta;
    w_u32(0x8005B230, timer);
    if (((int32_t)(uint32_t)(timer)) >= 0)
        return 0;
    ((scalar_a_6 = (r_u32(player + 1084))), (w_u8(scalar_a_6, (uint8_t)((uint32_t)r_u8(scalar_a_6) & 191))));
    player = r_u32(0x8005B38Cu);
    other = r_u32(0x8005B21C);
    w_u32(player + 720, 0);
    if (other)
    {
        ((scalar_a_7 = (r_u32(other + 1084))), (w_u8(scalar_a_7, (uint8_t)((uint32_t)r_u8(scalar_a_7) & 191))));
        w_u32(r_u32(0x8005B21C) + 720, 0);
    }
    state = r_u32(0x8005B2F8);
    w_u32(0x8005B30C, 0);
    w_u32(state + 16, 0);
    (void)sub_80090270();
    return 1;
}

uint32_t sub_8008AD38(uint32_t delta)
{
    FUNCTION_MARKER(0x8008AD38u, "RASHCDG.BIN");
    return sub_8008AD40(delta, r_u32(0x8005B38C));
}

uint32_t sub_8008AB00(uint32_t delta)
{
    FUNCTION_MARKER(0x8008AB00u, "RASHCDG.BIN");
    uint32_t elapsed = r_u32(0x8005B30C) + delta;
    uint32_t phase = r_u32(0x8005B2A8);
    w_u32(0x8005B30C, elapsed);
    if (phase)
    {
        if (((int32_t)(uint32_t)(elapsed)) > 32768)
        {
            (void)sub_800B8018(elapsed);
            phase = r_u32(0x8005B2A8);
            w_u32(0x8005B30C, 0);
            w_u32(0x8005B2A8, phase + 1);
        }
    }
    else if (sub_8008AD38(delta))
    {
        elapsed = r_u32(0x8005B30C);
        if (((int32_t)(uint32_t)(elapsed)) > 32768 && ((int32_t)(uint32_t)(elapsed - delta)) <= 32768)
            (void)sub_800B8018(0u);
        else
        {
            uint32_t state = r_u32(0x8005B2F8);
            uint32_t flags = r_u8(state + 4);
            uint32_t index = r_u32(state + 60) + ((flags & 4) ? 6 : ((0u - (flags & 1)) & 3));
            elapsed = r_u32(0x8005B30C);
            if (((int32_t)(uint32_t)(r_u32(0x80052FAC + 4 * index))) < ((int32_t)(uint32_t)(elapsed)))
            {
                phase = r_u32(0x8005B2A8);
                w_u32(0x8005B30C, delta);
                w_u32(0x8005B2A8, phase + 1);
            }
        }
    }
    else if (!r_u8(r_u32(0x8005B2F8) + 3))
        return 0;
    (void)sub_800B9414(delta);
    (void)sub_8008CD88(delta);
    (void)sub_8008AC80(delta);
    (void)sub_800A4774(((int32_t)(uint32_t)(delta)));
    (void)sub_8008ACE8(delta);
    return sub_80090814(((int32_t)(uint32_t)(delta)));
}
