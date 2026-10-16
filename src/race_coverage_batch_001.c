#include "rrj_debug.h"
#include "race_leaf.h"
#include <stdio.h>
#include <stdlib.h>

uint32_t sub_80048428(void)
{
    FUNCTION_MARKER(0x80048428, "SLUS_010.53");
    return r_u32(0x80055F0C);
}

static FILE *coverage_heap_stream;

static void coverage_heap_failure_probe(uint32_t requested)
{
    if (!coverage_heap_stream)
        abort();
    RRJ_DEBUG_FPRINTF(coverage_heap_stream, "%08X %08X %08X %08X 00000000 00000000 00000000 00000000 00000000\n", 0x80044894u, 0x800107F0u, requested, requested);
}

int rrj_coverage_probe(RRJMemory *m, uint32_t target, const uint32_t args[6], uint32_t *result)
{
    switch (target)
    {
        case 0x800140E8:
            *result = sub_800140E8();
            return 1;
        case 0x8001444C:
            *result = sub_8001444C();
            return 1;
        case 0x8001426C:
            *result = sub_8001426C(args[0], args[1], args[2]);
            return 1;
        case 0x80011738:
            *result = sub_80011738();
            return 1;
        case 0x80048428:
            *result = sub_80048428();
            return 1;
        case 0x80048414:
            *result = sub_80048414(args[0]);
            return 1;
        case 0x8001BDEC:
            *result = sub_8001BDEC();
            return 1;
        case 0x8001447C:
        {
            RRJHeapFailureProbe previous = rrj_heap_set_failure_probe(coverage_heap_failure_probe);
            FILE *previous_stream = coverage_heap_stream;
            coverage_heap_stream = (FILE *)m->sdk_user;
            *result = sub_8001447C(args[0], args[1]);
            (void)rrj_heap_set_failure_probe(previous);
            coverage_heap_stream = previous_stream;
            return 1;
        }
        case 0x800142B4:
        {
            RRJHeapFailureProbe previous = rrj_heap_set_failure_probe(coverage_heap_failure_probe);
            FILE *previous_stream = coverage_heap_stream;
            coverage_heap_stream = (FILE *)m->sdk_user;
            *result = sub_800142B4(args[0], args[1]);
            (void)rrj_heap_set_failure_probe(previous);
            coverage_heap_stream = previous_stream;
            return 1;
        }
        default:
            return 0;
    }
}
