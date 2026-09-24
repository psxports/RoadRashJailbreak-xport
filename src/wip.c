#include "wip.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
static FILE *journal;
static int continue_wip;
static RRJMemory *bound_memory;
static uint32_t current_frame;
static unsigned long sequence, tainted;
static int handoff_wip;

typedef struct SHA256Context
{
    uint32 state[8];
    uint64 bits;
    uint8 block[64];
    size_t used;
} SHA256Context;

static uint32 sha_rotr(uint32 value, uint32 count)
{
    return (value >> count) | (value << (32 - count));
}

static void sha_transform(SHA256Context *context, const uint8 block[64])
{
    static const uint32 constants[64] = {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};
    uint32 words[64], a, b, c, d, e, f, g, h;
    uint32 i;

    for (i = 0; i < 16; ++i)
        words[i] = ((uint32)block[4 * i] << 24) | ((uint32)block[4 * i + 1] << 16) | ((uint32)block[4 * i + 2] << 8) | block[4 * i + 3];
    for (; i < 64; ++i)
    {
        uint32 s0 = sha_rotr(words[i - 15], 7) ^ sha_rotr(words[i - 15], 18) ^ (words[i - 15] >> 3);
        uint32 s1 = sha_rotr(words[i - 2], 17) ^ sha_rotr(words[i - 2], 19) ^ (words[i - 2] >> 10);
        words[i] = words[i - 16] + s0 + words[i - 7] + s1;
    }
    a = context->state[0];
    b = context->state[1];
    c = context->state[2];
    d = context->state[3];
    e = context->state[4];
    f = context->state[5];
    g = context->state[6];
    h = context->state[7];
    for (i = 0; i < 64; ++i)
    {
        uint32 s1 = sha_rotr(e, 6) ^ sha_rotr(e, 11) ^ sha_rotr(e, 25);
        uint32 choice = (e & f) ^ (~e & g);
        uint32 first = h + s1 + choice + constants[i] + words[i];
        uint32 s0 = sha_rotr(a, 2) ^ sha_rotr(a, 13) ^ sha_rotr(a, 22);
        uint32 majority = (a & b) ^ (a & c) ^ (b & c);
        uint32 second = s0 + majority;
        h = g;
        g = f;
        f = e;
        e = d + first;
        d = c;
        c = b;
        b = a;
        a = first + second;
    }
    context->state[0] += a;
    context->state[1] += b;
    context->state[2] += c;
    context->state[3] += d;
    context->state[4] += e;
    context->state[5] += f;
    context->state[6] += g;
    context->state[7] += h;
}

static void sha_update(SHA256Context *context, const uint8 *data, size_t size)
{
    context->bits += (uint64)size * 8;
    while (size)
    {
        size_t part = size < 64 - context->used ? size : 64 - context->used;
        memcpy(context->block + context->used, data, part);
        context->used += part;
        data += part;
        size -= part;
        if (context->used == 64)
        {
            sha_transform(context, context->block);
            context->used = 0;
        }
    }
}

static void sha_final(SHA256Context *context, uint8 digest[32])
{
    uint32 i;

    context->block[context->used++] = 0x80;
    if (context->used > 56)
    {
        memset(context->block + context->used, 0, 64 - context->used);
        sha_transform(context, context->block);
        context->used = 0;
    }
    memset(context->block + context->used, 0, 56 - context->used);
    for (i = 0; i < 8; ++i)
        context->block[63 - i] = (uint8)(context->bits >> (8 * i));
    sha_transform(context, context->block);
    for (i = 0; i < 8; ++i)
    {
        digest[4 * i] = (uint8)(context->state[i] >> 24);
        digest[4 * i + 1] = (uint8)(context->state[i] >> 16);
        digest[4 * i + 2] = (uint8)(context->state[i] >> 8);
        digest[4 * i + 3] = (uint8)context->state[i];
    }
}

static int executable_hash(const char *path, char out[65])
{
    SHA256Context context = {{0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19}, 0, {0}, 0};
    uint8 data[16384], digest[32];
    size_t count;
    FILE *stream = fopen(path, "rb");
    int i;

    if (!stream)
        return 0;
    while ((count = fread(data, 1, sizeof(data), stream)) != 0)
        sha_update(&context, data, count);
    if (ferror(stream) || fclose(stream))
        return 0;
    sha_final(&context, digest);
    for (i = 0; i < 32; ++i)
        sprintf(out + 2 * i, "%02x", digest[i]);
    return 1;
}

static void close_journal(void)
{
    if (journal)
    {
        fclose(journal);
        journal = NULL;
    }
}

static void quoted(const char *s)
{
    fputc('"', journal);
    for (; *s; s++)
    {
        unsigned char c = (unsigned char)*s;
        if (c == '"' || c == '\\')
            fputc('\\', journal);
        if (c < 32)
            fprintf(journal, "\\u%04x", c);
        else
            fputc(c, journal);
    }
    fputc('"', journal);
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

void rrj_wip_frame(uint32_t frame)
{
    current_frame = frame;
}

int rrj_wip_options(int *argc, char **argv, RRJMemory *m)
{
    const char *path = NULL;
    char hash[65], default_path[4096];
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
    if (!executable_hash(argv[0], hash))
    {
        fputs("Cannot fingerprint RRJ.exe for WIP journal\n", stderr);
        return 0;
    }
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
    journal = fopen(path, "ab");
    if (!journal)
    {
        fprintf(stderr, "Cannot open WIP journal: %s\n", path);
        return 0;
    }
    atexit(close_journal);
    fprintf(journal, "{\"event\":\"session\",\"time\":%lld,\"continue\":%s,\"build\":", (long long)time(NULL), continue_wip ? "true" : "false");
    quoted(__DATE__ " " __TIME__);
    fputs(",\"exe_sha256\":", journal);
    quoted(hash);
    fputs("}\n", journal);
    if (fflush(journal) || ferror(journal))
    {
        fputs("Cannot write WIP journal\n", stderr);
        return 0;
    }
    if (continue_wip)
        fprintf(stderr, "WIP discovery enabled. Journal: %s\n", path);
    return 1;
}

void rrj_wip_site(RRJMemory *m, uint32_t pc, const char *kind, const char *subsystem, const char *function, const char *file, int line, int recoverable, const char *fallback, const uint32_t *args, unsigned count)
{
    unsigned i;
    int resume = continue_wip && recoverable;
    if (journal)
    {
        uint32_t context = peek(m, 0x8005B2F8, 4);
        fprintf(journal, "{\"event\":\"wip\",\"sequence\":%lu,\"time\":%lld,\"pc\":\"%08X\",\"pc_kind\":", ++sequence, (long long)time(NULL), pc);
        quoted(kind);
        fputs(",\"subsystem\":", journal);
        quoted(subsystem);
        fputs(",\"function\":", journal);
        quoted(function);
        fputs(",\"file\":", journal);
        quoted(file);
        fprintf(journal, ",\"line\":%d,\"iteration\":%u,\"vblank\":%u,\"game_context\":\"%08X\",\"game_state\":%u,\"menu\":%u,\"continued\":%s,\"prior_skips\":%lu,\"fallback\":", line, current_frame, peek(m, 0x8005B46C, 4), context, peek(m, context, 1), peek(m, 0x8009C5D0, 2), resume ? "true" : "false", tainted);
        quoted(resume || handoff_wip ? fallback : "abort");
        fputs(",\"args\":[", journal);
        for (i = 0; i < count; i++)
            fprintf(journal, "%s\"%08X\"", i ? "," : "", args[i]);
        fputs("],\"input_snapshot_hex\":\"", journal);
        for (i = 0; i < 768; i++)
            fprintf(journal, "%02X", peek(m, 0x800D7128 + i, 1));
        fputs("\"}\n", journal);
        if (fflush(journal) || ferror(journal))
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
