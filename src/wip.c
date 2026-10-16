#include "rrj_debug.h"
#include "wip.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int continue_wip;
static RRJMemory *bound_memory;
static int handoff_wip;

void rrj_wip_frame(uint32_t frame)
{
}

int rrj_wip_options(int *argc, char **argv, RRJMemory *m)
{
    int i, out = 1, debug = 0, explicit_continue = 0;
    bound_memory = m;
    for (i = 1; i < *argc; i++)
    {
        if (!strcmp(argv[i], "--wip-continue"))
            explicit_continue = 1;
        else if (!strcmp(argv[i], "--debug"))
            debug = 1;
        else if (!strcmp(argv[i], "--wip-log"))
        {
            RRJ_DEBUG_FPUTS("--wip-log is obsolete and unsupported\n", stderr);
            return 0;
        }
        else
            argv[out++] = argv[i];
    }
    *argc = out;
    argv[out] = NULL;
    continue_wip = !debug && (explicit_continue || out == 1 || !strcmp(argv[1], "--menu-run"));
    return 1;
}

void rrj_wip_site(RRJMemory *m, uint32_t pc, const char *kind, const char *subsystem, const char *function, const char *file, int line, int recoverable, const char *fallback, const uint32_t *args, unsigned count)
{
    if ((continue_wip && recoverable) || handoff_wip)
        return;
    RRJ_DEBUG_FPRINTF(stderr, "WIP/guard stop %08X %s (%s:%d)\n", pc, function, file, line);
    abort();
}

void rrj_wip_handoff(RRJMemory *m, uint32_t pc, const char *subsystem, const char *function, const char *file, int line, const char *fallback, const uint32_t *args, unsigned count)
{
    handoff_wip = 1;
    rrj_wip_site(m, pc, "function", subsystem, function, file, line, 0, fallback, args, count);
    handoff_wip = 0;
}

_Noreturn void rrj_wip_stop(const char *function, const char *file, int line)
{
    rrj_wip_site(bound_memory, 0, "unknown", "guard_or_unclassified", function, file, line, 0, "abort", NULL, 0);
    abort();
}
