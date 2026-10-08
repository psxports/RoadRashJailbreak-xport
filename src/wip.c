#include "wip.h"
#include "xport_trace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
static XportJournal journal;
static int continue_wip;
static RRJMemory *bound_memory;
static uint32_t current_frame;
static unsigned long sequence, tainted;
static int handoff_wip;

static void close_journal(void)
{
    xport_journal_close(&journal);
}

/* Logging must never call rrj_at: invalid pointers are themselves loggable. */
static uint32_t peek(RRJMemory *m, uint32_t address, unsigned bytes)
{
    uint32_t p = address & 0x1fffffff, value = 0;
    unsigned i;
    if (!m || p > PSX_DRAM_SIZE || bytes > PSX_DRAM_SIZE - p)
        return 0;
    for (i = 0; i < bytes; i++)
        value |= (uint32)((uint8 *)psx_addr(address, bytes))[i] << (8 * i);
    return value;
}

static uint8_t peek_byte(void *user, uint32_t address)
{
    return (uint8_t)peek((RRJMemory *)user, address, 1u);
}

void rrj_wip_frame(uint32_t frame)
{
    current_frame = frame;
}

int rrj_wip_options(int *argc, char **argv, RRJMemory *m)
{
    const char *path = NULL;
    char default_path[4096];
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
            if (++i >= *argc || !argv[i][0])
            {
                fputs("--wip-log requires a file path\n", stderr);
                return 0;
            }
            path = argv[i];
        }
        else
            argv[out++] = argv[i];
    }
    *argc = out;
    argv[out] = NULL;
    /* Old differential probes stay strict, including probes invoked by historical tools. */
    continue_wip = !debug && (explicit_continue || out == 1 || !strcmp(argv[1], "--menu-run"));
    if (!path && !continue_wip)
        return 1;
    if (!path)
    {
        char *slash;
        if (strlen(argv[0]) >= sizeof(default_path))
            return 0;
        strcpy(default_path, argv[0]);
        slash = strrchr(default_path, '\\');
        if (!slash)
            slash = strrchr(default_path, '/');
        if (!slash)
            strcpy(default_path, "wip-branches.jsonl");
        else
        {
            if ((size_t)(slash - default_path + 1) + 20 >= sizeof default_path)
                return 0;
            strcpy(slash + 1, "wip-branches.jsonl");
        }
        path = default_path;
    }
    if (!xport_journal_open(&journal, path, __DATE__ " " __TIME__, argv[0], continue_wip))
    {
        fprintf(stderr, "Cannot open WIP journal: %s\n", path);
        return 0;
    }
    atexit(close_journal);
    if (continue_wip)
        fprintf(stderr, "WIP discovery enabled. Journal: %s\n", path);
    return 1;
}

void rrj_wip_site(RRJMemory *m, uint32_t pc, const char *kind, const char *subsystem, const char *function, const char *file, int line, int recoverable, const char *fallback, const uint32_t *args, unsigned count)
{
    int resume = continue_wip && recoverable;
    if (journal.stream)
    {
        uint32_t context = peek(m, 0x8005B2F8, 4);
        if (!xport_journal_begin(&journal, "wip") || !xport_journal_u64(&journal, "sequence", ++sequence) || !xport_journal_u64(&journal, "time", (uint64_t)time(NULL)) || !xport_journal_u32_hex(&journal, "pc", pc) || !xport_journal_string(&journal, "pc_kind", kind) || !xport_journal_string(&journal, "subsystem", subsystem) || !xport_journal_string(&journal, "function", function) || !xport_journal_string(&journal, "file", file) || !xport_journal_u64(&journal, "line", (uint64_t)line) || !xport_journal_u64(&journal, "iteration", current_frame) || !xport_journal_u64(&journal, "vblank", peek(m, 0x8005B46C, 4)) || !xport_journal_u32_hex(&journal, "game_context", context) || !xport_journal_u64(&journal, "game_state", peek(m, context, 1)) || !xport_journal_u64(&journal, "menu", peek(m, 0x8009C5D0, 2)) || !xport_journal_boolean(&journal, "continued", resume) || !xport_journal_u64(&journal, "prior_skips", tainted) ||
            !xport_journal_string(&journal, "fallback", resume || handoff_wip ? fallback : "abort") || !xport_journal_u32_hex_array(&journal, "args", args, count) || !xport_journal_hex_bytes(&journal, "input_snapshot_hex", 0x800D7128u, 768u, peek_byte, m) || !xport_journal_end(&journal))
        {
            fputs("WIP journal write failed\n", stderr);
            abort();
        }
    }
    if (resume || handoff_wip)
    {
        if (resume)
            ++tainted;
        return;
    }
    fprintf(stderr, "WIP/guard stop %08X %s (%s:%d)\n", pc, function, file, line);
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
