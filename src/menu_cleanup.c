#include "psx.h"
/* F80080488: deferred resource release, preserving callback-visible reloads. */
#include "menu_cleanup.h"
#include <stdlib.h>

uint32_t sub_F_80080488(RRJCleanupCall call)
{
    FUNCTION_MARKER(0x80080488u, "RASHCDF.BIN");
    uint32_t index = 0, entry = 0x800A0970, descriptor, allocation, count, flags;
    if (!call)
        abort();
    (void)call(rrj_host_context(), 0x800487C0, 0);
    (void)call(rrj_host_context(), 0x80043DA4, 0);
    count = rrj_read32(0x8009D4DC);
    if (rrj_s32(count) > 0)
        do
        {
            descriptor = rrj_read32(entry);
            allocation = rrj_read32(descriptor + 4);
            if (allocation)
            {
                (void)call(rrj_host_context(), 0x800144B8, allocation);
                descriptor = rrj_read32(entry);
                rrj_write32(descriptor + 4, 0);
                descriptor = rrj_read32(entry);
            }
            entry += 4;
            ++index;
            count = rrj_read32(0x8009D4DC);
            flags = rrj_u16(rrj_at(descriptor, 2));
            rrj_put16(rrj_at(descriptor, 2), (uint16_t)(flags & 0xfffb));
        } while (rrj_s32(index) < rrj_s32(count));
    rrj_write32(0x8009D4DC, 0);
    return call(rrj_host_context(), 0x80043DB4, 0);
}
