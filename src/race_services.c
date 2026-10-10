#include "psx.h"
/* Literal G-overlay service sequencing; game callee bindings remain explicit. */
#include "race_services.h"
#include <stdlib.h>

uint32_t sub_G_8008CD88(uint32_t delta, RRJRaceServiceCall call)
{
    FUNCTION_MARKER(0x8008CD88u, "RASHCDG.BIN");
    uint32_t timer = rrj_read32(0x8005B318) + delta, result;
    rrj_write32(0x8005B318, timer);
    if (!call)
        abort();
    if (rrj_s32(timer) >= 16384)
        (void)call(rrj_host_context(), 0x8009C308, 0, 0);
    timer = rrj_read32(0x8005B318);
    if (rrj_s32(timer) <= 65535)
        return 0;
    (void)call(rrj_host_context(), 0x8009B474, 0, timer);
    result = call(rrj_host_context(), 0x8009B474, 3, rrj_read32(0x8005B318));
    rrj_write32(0x8005B318, 0);
    return result;
}

uint32_t sub_G_8008AC80(uint32_t delta, RRJRaceServiceCall call)
{
    FUNCTION_MARKER(0x8008AC80u, "RASHCDG.BIN");
    if (!call)
        abort();
    (void)call(rrj_host_context(), 0x8009A298, delta, 0);
    (void)call(rrj_host_context(), 0x80075EE0, delta, 0);
    (void)call(rrj_host_context(), 0x8008F068, delta, 0);
    (void)call(rrj_host_context(), 0x800A2898, delta, 0);
    if (rrj_read32(0x8005B254))
        (void)call(rrj_host_context(), 0x800CB304, delta, 0);
    (void)call(rrj_host_context(), 0x8009ACA4, 0, 0);
    return call(rrj_host_context(), 0x8009AB60, 0, 0);
}

uint32_t sub_G_8008ACE8(uint32_t delta, RRJRaceServiceCall call)
{
    FUNCTION_MARKER(0x8008ACE8u, "RASHCDG.BIN");
    if (!call)
        abort();
    (void)call(rrj_host_context(), 0x8007B840, delta, 0);
    (void)call(rrj_host_context(), 0x800A2A64, delta, 0);
    if (rrj_read32(0x8005B254))
        (void)call(rrj_host_context(), 0x800CB4F8, delta, 0);
    return call(rrj_host_context(), 0x800A13C4, delta, 0);
}
