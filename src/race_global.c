#include "psx.h"
/* G8008AB00: global update sequencing. Adjacent eight-byte entries are real
 * fallthrough wrappers, not empty functions; their native bindings are WIP. */
#include "race_global.h"
#include <stdlib.h>

static uint32_t read_byte(RRJMemory *m, uint32_t a)
{
    return r_u8(a);
}

static void clear_flags(RRJMemory *m, uint32_t a, uint32_t mask)
{
    w_u8(a, (uint8_t)(read_byte(m, a) & mask));
}

uint32_t sub_G_8008AD40(uint32_t delta, uint32_t player, uint32_t base, RRJRaceGlobalCall call)
{
    FUNCTION_MARKER(0x8008AD40u, "RASHCDG.BIN");
    uint32_t other, flags, timer, state;
    if (!(read_byte(rrj_host_context(), rrj_read32(player + 1084)) & 64))
        return 1;
    other = rrj_read32(0x8005B21C);
    rrj_write32(player + 720, 0xffff0000);
    if (other)
        rrj_write32(other + 720, 0xffff0000);
    player = rrj_read32(base - 19572);
    if (read_byte(rrj_host_context(), rrj_read32(player + 1084)) & 32)
    {
        rrj_write32(rrj_read32(0x8005B2F8) + 16, 0);
        flags = rrj_read32(player + 1084);
        rrj_write32(0x8005B230, 196608);
        clear_flags(rrj_host_context(), flags, 223);
        other = rrj_read32(0x8005B21C);
        if (other)
            clear_flags(rrj_host_context(), rrj_read32(other + 1084), 223);
        return 0;
    }
    timer = rrj_read32(0x8005B230) - delta;
    rrj_write32(0x8005B230, timer);
    if (rrj_s32(timer) >= 0)
        return 0;
    clear_flags(rrj_host_context(), rrj_read32(player + 1084), 191);
    player = rrj_read32(base - 19572);
    other = rrj_read32(0x8005B21C);
    rrj_write32(player + 720, 0);
    if (other)
    {
        clear_flags(rrj_host_context(), rrj_read32(other + 1084), 191);
        rrj_write32(rrj_read32(0x8005B21C) + 720, 0);
    }
    state = rrj_read32(0x8005B2F8);
    rrj_write32(0x8005B30C, 0);
    rrj_write32(state + 16, 0);
    if (!call)
        abort();
    (void)call(rrj_host_context(), 0x80090270, 0);
    return 1;
}

uint32_t sub_G_8008AD38(uint32_t delta, RRJRaceGlobalCall call)
{
    FUNCTION_MARKER(0x8008AD38u, "RASHCDG.BIN");
    return sub_G_8008AD40(delta, rrj_read32(0x8005B38C), 0x80060000, call);
}

uint32_t sub_G_8008AB00(uint32_t delta, RRJRaceGlobalCall call)
{
    FUNCTION_MARKER(0x8008AB00u, "RASHCDG.BIN");
    uint32_t elapsed = rrj_read32(0x8005B30C) + delta;
    uint32_t phase = rrj_read32(0x8005B2A8);
    if (!call)
        abort();
    rrj_write32(0x8005B30C, elapsed);
    if (phase)
    {
        if (rrj_s32(elapsed) > 32768)
        {
            (void)call(rrj_host_context(), 0x800B8018, elapsed);
            phase = rrj_read32(0x8005B2A8);
            rrj_write32(0x8005B30C, 0);
            rrj_write32(0x8005B2A8, phase + 1);
        }
    }
    else if (call(rrj_host_context(), 0x8008AD38, delta))
    {
        elapsed = rrj_read32(0x8005B30C);
        if (rrj_s32(elapsed) > 32768 && rrj_s32(elapsed - delta) <= 32768)
            (void)call(rrj_host_context(), 0x800B8018, 0);
        else
        {
            uint32_t state = rrj_read32(0x8005B2F8);
            uint32_t flags = r_u8(state + 4);
            uint32_t index = rrj_read32(state + 60) + ((flags & 4) ? 6 : ((0u - (flags & 1)) & 3));
            elapsed = rrj_read32(0x8005B30C);
            if (rrj_s32(rrj_read32(0x80052FAC + 4 * index)) < rrj_s32(elapsed))
            {
                phase = rrj_read32(0x8005B2A8);
                rrj_write32(0x8005B30C, delta);
                rrj_write32(0x8005B2A8, phase + 1);
            }
        }
    }
    else if (!r_u8(rrj_read32(0x8005B2F8) + 3))
        return 0;
    (void)call(rrj_host_context(), 0x800B9414, delta);
    (void)call(rrj_host_context(), 0x8008CD88, delta);
    (void)call(rrj_host_context(), 0x8008AC80, delta);
    (void)call(rrj_host_context(), 0x800A4774, delta);
    (void)call(rrj_host_context(), 0x8008ACE8, delta);
    return call(rrj_host_context(), 0x80090814, delta);
}
