#include "psx.h"
/* Deferred resource release with original reload order */
#include "menu_cleanup.h"
#include "native_game_api.h"
#include <stdlib.h>

uint32_t sub_80080488(void)
{
    FUNCTION_MARKER(0x80080488u, "RASHCDF.BIN");
    uint32_t index = 0, entry = 0x800A0970, descriptor, allocation, count, flags;
    (void)(uint32_t)DrawSync(0);
    sub_80043DA4();
    count = r_u32(0x8009D4DC);
    if (((int32_t)(uint32_t)(count)) > 0)
        do
        {
            descriptor = r_u32(entry);
            allocation = r_u32(descriptor + 4);
            if (allocation)
            {
                (void)sub_800144B8(allocation);
                descriptor = r_u32(entry);
                w_u32(descriptor + 4, 0);
                descriptor = r_u32(entry);
            }
            entry += 4;
            ++index;
            count = r_u32(0x8009D4DC);
            flags = r_u16(descriptor);
            w_u16(descriptor, (uint16_t)(flags & 0xfffb));
        } while (((int32_t)(uint32_t)(index)) < ((int32_t)(uint32_t)(count)));
    w_u32(0x8009D4DC, 0);
    sub_80043DB4();
    return 0x800A0000u;
}
