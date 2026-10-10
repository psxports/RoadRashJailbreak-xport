#include "psx.h"
/* F80066C34: prepare/draw menus in original table order, then current menu. */
#include "menu_frame.h"
#include <stdlib.h>

static uint32_t h(RRJMemory *m, uint32_t a)
{
    return rrj_u16(rrj_at(a, 2));
}

static uint32_t sh(uint32_t v)
{
    return v < 32768 ? v : v | 0xffff0000;
}

static void half(RRJMemory *m, uint32_t a, uint32_t v)
{
    rrj_put16(rrj_at(a, 2), (uint16_t)v);
}

static uint32_t call(RRJMemory *m, RRJMenuFrameCall cb, uint32_t fn, uint32_t a, uint32_t b)
{
    if (!cb)
        abort();
    return cb(m, fn, a, b);
}

void sub_F_80066C34(RRJMenuFrameCall cb)
{
    FUNCTION_MARKER(0x80066C34u, "RASHCDF.BIN");
    uint32_t index, menu, flags, fn, resource;
    half(rrj_host_context(), 0x8009C5D8, 0);
    do
    {
        index = sh(h(rrj_host_context(), 0x8009C5D8));
        if (index != sh(h(rrj_host_context(), 0x8009C5D0)))
        {
            menu = rrj_read32(rrj_read32(0x8009C68C) + 4 * index);
            if (h(rrj_host_context(), menu + 2))
            {
                fn = rrj_read32(0x8009D0C0 + 4 * index);
                if (fn)
                    (void)call(rrj_host_context(), cb, fn, menu, 0);
                else
                    half(rrj_host_context(), menu + 2, 0);
            }
            else
            {
                flags = h(rrj_host_context(), menu);
                if (flags & 2)
                {
                    fn = rrj_read32(0x8009CFD0 + 4 * index);
                    if (fn)
                        (void)call(rrj_host_context(), cb, fn, menu, 0);
                }
                else if (flags & 4)
                {
                    resource = sh(h(rrj_host_context(), menu + 6));
                    half(rrj_host_context(), menu, flags & 0xfffb);
                    (void)call(rrj_host_context(), cb, 0x80078E80, rrj_read32(0x8009D3F0 + 4 * resource), 0);
                }
            }
        }
        half(rrj_host_context(), 0x8009C5D8, h(rrj_host_context(), 0x8009C5D8) + 1);
    } while (rrj_s32(sh(h(rrj_host_context(), 0x8009C5D8))) < 59);
    index = sh(h(rrj_host_context(), 0x8009C5D0));
    half(rrj_host_context(), 0x8009C5D8, index);
    menu = rrj_read32(rrj_read32(0x8009C68C) + 4 * index);
    flags = h(rrj_host_context(), menu);
    if (!(flags & 4))
    {
        resource = rrj_read32(0x8009D3F0 + 4 * sh(h(rrj_host_context(), menu + 6)));
        if (resource && call(rrj_host_context(), cb, 0x80078BD8, resource, (flags >> 14) & 1))
            half(rrj_host_context(), menu, h(rrj_host_context(), menu) | 4);
    }
    if (!rrj_read32(0x8008972C))
    {
        flags = h(rrj_host_context(), menu);
        if ((flags & 0x1000) && call(rrj_host_context(), cb, 0x80078BD8, 0x80089C84, (flags >> 13) & 1))
            rrj_write32(0x8008972C, 1);
    }
    index = sh(h(rrj_host_context(), 0x8009C5D0));
    half(rrj_host_context(), 0x8009C5D8, index);
    menu = rrj_read32(rrj_read32(0x8009C68C) + 4 * index);
    if (h(rrj_host_context(), menu + 2))
    {
        if (h(rrj_host_context(), menu) & 0x8000)
        {
            (void)call(rrj_host_context(), cb, 0x80022A78, 2, 0);
            rrj_write32(0x8005ACAC, 0);
        }
        fn = rrj_read32(0x8009D0C0 + 4 * sh(h(rrj_host_context(), 0x8009C5D8)));
        if (fn)
            (void)call(rrj_host_context(), cb, fn, menu, 0);
        else
            half(rrj_host_context(), menu + 2, 0);
    }
    else if (h(rrj_host_context(), menu) & 2)
    {
        fn = rrj_read32(0x8009CFD0 + 4 * index);
        if (fn)
            (void)call(rrj_host_context(), cb, fn, menu, 0);
    }
}
