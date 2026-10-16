/* Windows digital-pad boundary backed by the reused AA PsyQ PadRead.
 * Keyboard devices have no analog mode or vibration actuators. */
#include "input.h"
#include "pad.h"
#include "native_game_api.h"
#include "psx.h"
#include <string.h>

void rrj_input_read(void)
{
    if (!pad_replay_is_bound())
    {
        uint32_t buttons = PadRead(0) ^ 0xffff;
        w_u8(0x800D70E0, 0);
        w_u8(0x800D70E1, 0x41);
        w_u8(0x800D70E2, buttons >> 8);
        w_u8(0x800D70E3, buttons);
        xport_guest_fill(0x800D70E4, 0, 4);
    }
    (void)sub_8001C5F8();
}

void rrj_input_latch(void)
{
    (void)sub_8001CB3C();
}
