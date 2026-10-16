#include "psx.h"
/* Controller state and actuator handshake, MIPS 8001DDC4. */
#include "pad_service.h"

uint32_t sub_8001DDC4(uint32_t player, uint32_t multi)
{
    FUNCTION_MARKER(0x8001DDC4u, "SLUS_010.53");
    uint32_t pad, port, i, state, result;
    if (player == 1)
        w_u32(0x800D7440, multi ? 1 : 16);
    pad = 0x800D7428 + 24 * player;
    port = r_u32(pad);
    for (i = 0; i < 2; ++i)
    {
        uint32_t until = r_u32(pad + 16 + 4 * i);
        if (until && ((int32_t)(uint32_t)(until)) < ((int32_t)(uint32_t)(r_u32(r_u32(0x8005B2F8) + 12))))
        {
            w_u8((pad + 12 + i), (uint8_t)0);
            w_u32(pad + 16 + 4 * i, 0);
        }
    }
    state = (uint32_t)PadGetState(((int32_t)(uint32_t)(port)));
    if (!state)
    {
        w_u32(pad + 4, 0);
        return 1;
    }
    if (state == 1)
    {
        w_u32(pad + 4, 0);
        w_u8((pad + 13), (uint8_t)0);
        w_u8((pad + 12), (uint8_t)0);
        w_u32(pad + 20, 0);
        w_u32(pad + 16, 0);
    }
    if (r_u32(pad + 4))
    {
        result = (uint32_t)PadInfoMode(((int32_t)(uint32_t)(port)), 2, 0);
        if (result)
            return result;
        if (r_u32(pad + 16))
        {
            w_u8((pad + 12), (uint8_t)64);
            w_u8((pad + 13), (uint8_t)1);
            return 1;
        }
        w_u8((pad + 12), (uint8_t)64);
        w_u8((pad + 13), (uint8_t)0);
        return 64;
    }
    PadSetAct(((int32_t)(uint32_t)(port)), xport_guest_ptr(pad + 12, 2), 2);
    if (state == 2)
    {
        w_u32(pad + 4, 2);
        return 6;
    }
    if (state != 6)
        return 6;
    result = (uint32_t)PadSetActAlign(((int32_t)(uint32_t)(port)), xport_guest_ptr(0x8005AD94u, 6));
    if (result)
        w_u32(pad + 4, 1);
    return 1;
}
