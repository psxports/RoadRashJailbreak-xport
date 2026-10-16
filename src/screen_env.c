#include "native_game_api.h"
#include "psx.h"
/* PsyQ default environments, verified against this executable's MIPS. */
#include "screen_env.h"
#include <stdlib.h>

uint32_t sub_8004CC44(uint32_t p, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
    FUNCTION_MARKER(0x8004CC44u, "SLUS_010.53");
    uint32_t mode;
    mode = sub_80048428();
    w_u16(p, (uint16_t)x);
    w_u16((p + 2), (uint16_t)y);
    w_u16((p + 4), (uint16_t)w);
    w_u16((p + 12), (uint16_t)0);
    w_u16((p + 14), (uint16_t)0);
    w_u16((p + 16), (uint16_t)0);
    w_u16((p + 18), (uint16_t)0);
    w_u8((p + 25), (uint8_t)0);
    w_u8((p + 26), (uint8_t)0);
    w_u8((p + 27), (uint8_t)0);
    w_u8((p + 22), (uint8_t)1);
    w_u16((p + 6), (uint16_t)h);
    w_u8((p + 23), (uint8_t)(((int32_t)(uint32_t)(h)) < (mode ? 289 : 257)));
    w_u16((p + 8), (uint16_t)x);
    w_u16((p + 10), (uint16_t)y);
    w_u16((p + 20), (uint16_t)10);
    w_u8((p + 24), (uint8_t)0);
    return p;
}

uint32_t sub_8004CD04(uint32_t p, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
    FUNCTION_MARKER(0x8004CD04u, "SLUS_010.53");
    w_u16(p, (uint16_t)x);
    w_u16((p + 2), (uint16_t)y);
    w_u16((p + 4), (uint16_t)w);
    w_u16((p + 8), (uint16_t)0);
    w_u16((p + 10), (uint16_t)0);
    w_u16((p + 12), (uint16_t)0);
    w_u16((p + 14), (uint16_t)0);
    w_u8((p + 17), (uint8_t)0);
    w_u8((p + 16), (uint8_t)0);
    w_u8((p + 19), (uint8_t)0);
    w_u8((p + 18), (uint8_t)0);
    w_u16((p + 6), (uint16_t)h);
    return p;
}
