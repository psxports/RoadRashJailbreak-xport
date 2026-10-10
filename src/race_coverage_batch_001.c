#include "race_leaf.h"
#include <stdio.h>
#include <stdlib.h>

uint32_t sub_80048428(void)
{
    FUNCTION_MARKER(0x80048428, "SLUS_010.53");
    return rrj_read32(0x80055F0C);
}

static uint32_t coverage_probe_call(RRJMemory *m, uint32_t target, const uint32_t args[8])
{
    FILE *stream = (FILE *)m->sdk_user;
    unsigned index;
    if (!stream)
        abort();
    fprintf(stream, "%08X", target);
    for (index = 0; index < 8; ++index)
        fprintf(stream, " %08X", args[index]);
    fputc('\n', stream);
    return 0;
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
            *result = sub_8001447C(args[0], args[1], coverage_probe_call);
            return 1;
        case 0x800142B4:
            *result = sub_800142B4(args[0], args[1], coverage_probe_call);
            return 1;
        default:
            return 0;
    }
}
