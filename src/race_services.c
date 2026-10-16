#include "race_leaf_batch_002.h"
#include "race_leaf_frontier.h"
#include "race_pause_frontier.h"
#include "native_game_api.h"
#include "psx.h"
/* Literal G-overlay service sequencing; game callee bindings remain explicit. */
#include "race_services.h"
#include <stdlib.h>

uint32_t sub_8008CD88(uint32_t delta)
{
    uint32_t timer, result;
    FUNCTION_MARKER(0x8008CD88u, "RASHCDG.BIN");
    timer = r_u32(0x8005B318u) + delta;
    w_u32(0x8005B318u, timer);
    if ((int32_t)timer >= 16384)
        (void)sub_8009C308();
    timer = r_u32(0x8005B318u);
    result = (int32_t)timer > 65535;
    if (result)
    {
        (void)sub_8009B474(0u, timer);
        result = sub_8009B474(3u, r_u32(0x8005B318u));
        w_u32(0x8005B318u, 0u);
    }
    return result;
}

uint32_t sub_8008AC80(uint32_t delta)
{
    FUNCTION_MARKER(0x8008AC80u, "RASHCDG.BIN");
    (void)sub_8009A298(delta);
    (void)sub_80075EE0((int32_t)delta);
    (void)sub_8008F068(((int32_t)(uint32_t)(delta)));
    (void)sub_800A2898(((int32_t)(uint32_t)(delta)));
    if (r_u32(0x8005B254))
        (void)sub_800CB304(((int32_t)(uint32_t)(delta)));
    (void)sub_8009ACA4();
    return sub_8009AB60();
}

uint32_t sub_8008ACE8(uint32_t delta)
{
    FUNCTION_MARKER(0x8008ACE8, "RASHCDG.BIN");
    (void)sub_8007B840((int32_t)delta);
    (void)sub_800A2A64((int32_t)delta);
    if (r_u32(0x8005B254))
        (void)sub_800CB4F8((int32_t)delta);
    return sub_800A13C4((int32_t)delta);
}
