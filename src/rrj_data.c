#include "rrj_data.h"
#include "psx.h"
#include "race_trace_runtime.h"
#include "xport_trace.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef struct RRJDataEntry
{
    uint32_t hash;
    uint32_t size;
    char path[256];
} RRJDataEntry;

static FILE *files[16];
static RRJDataEntry *entries;
static size_t entry_count;
static FILE *stream_file;
static uint32_t stream_base, stream_size;

int rrj_data_enabled(void)
{
#if defined(LOCKSTEP_DEBUG)
    return 0;
#else
    return rrj_trace_runtime_is_live();
#endif
}

static int load_index(void)
{
    FILE *file;
    size_t size;
    if (entries)
        return 1;
    if (!xport_file_size("DATA/rrj-files.idx", &size) || !size || size % sizeof(*entries))
        return 0;
    file = xport_fopen("DATA/rrj-files.idx", "rb");
    if (!file)
        return 0;
    entries = malloc(size);
    if (!entries || xport_fread(entries, 1, size, file) != size)
    {
        free(entries);
        entries = NULL;
        fclose(file);
        return 0;
    }
    fclose(file);
    entry_count = size / sizeof(*entries);
    return 1;
}

static int path_equal(const char *left, const char *right)
{
    while (*left && *right)
    {
        unsigned a = (unsigned char)*left++, b = (unsigned char)*right++;
        if (a >= 'a' && a <= 'z') a -= 'a' - 'A';
        if (b >= 'a' && b <= 'z') b -= 'a' - 'A';
        if (a != b)
            return 0;
    }
    return *left == *right;
}

static const char *data_path(const char *path)
{
    char normalized[256];
    const char *colon;
    size_t length = 0, i;
    if (!path || !load_index())
        return NULL;
    colon = strchr(path, ':');
    if (colon)
        path = colon + 1;
    while (*path == '/' || *path == '\\')
        ++path;
    while (*path && *path != ';')
    {
        if (length + 1 >= sizeof(normalized))
            return NULL;
        normalized[length++] = *path == '\\' ? '/' : *path;
        ++path;
    }
    normalized[length] = 0;
    for (i = 0; i < entry_count; ++i)
        if (path_equal(entries[i].path, normalized) || path_equal(entries[i].path + 5, normalized))
            return entries[i].path;
    return NULL;
}

void rrj_data_reset(void)
{
    unsigned i;
    for (i = 0; i < 16; ++i)
    {
        if (files[i]) fclose(files[i]);
        files[i] = NULL;
    }
    rrj_data_stream_close();
}

int rrj_data_init(void)
{
    rrj_data_reset();
    return load_index();
}

uint32_t rrj_data_open(const char *path)
{
    const char *resolved = data_path(path);
    FILE *file;
    size_t size;
    uint32_t i, record, base = 0, table, rows, hash = 0;
    size_t entry;
    if (!resolved || !xport_file_size(resolved, &size) || size > INT32_MAX)
        return UINT32_MAX;
    for (i = 0; i < 16; ++i)
        if (!r_u32(0x800D65D0 + 20 * i)) break;
    if (i == 16 || !(file = xport_fopen(resolved, "rb")))
        return UINT32_MAX;
    if (files[i]) fclose(files[i]);
    files[i] = file;
    for (entry = 0; entry < entry_count; ++entry)
        if (entries[entry].path == resolved) hash = entries[entry].hash;
    table = r_u32(0x8005AD6C);
    rows = r_u32(0x8005AD70);
    if (rows <= 500)
        for (entry = 0; entry < rows; ++entry)
            if (r_u32(table + 12 * (uint32_t)entry + 8) == hash && r_u32(table + 12 * (uint32_t)entry + 4) == size)
            {
                base = r_u32(table + 12 * (uint32_t)entry);
                break;
            }
    record = 0x800D65D0 + 20 * i;
    w_u32(record + 4, base);
    w_u32(record + 8, 0);
    w_u32(record + 12, (uint32_t)size);
    w_u32(record + 16, 0);
    w_u32(record, 1);
    return i;
}

static const char *original_path(uint32_t base, uint32_t size)
{
    uint32_t table = r_u32(0x8005AD6C), count = r_u32(0x8005AD70);
    uint32_t i;
    size_t j;
    const char *found = NULL;
    if (!load_index() || count > 500)
        return NULL;
    for (i = 0; i < count; ++i)
    {
        uint32_t entry = table + 12 * i;
        if (r_u32(entry) != base || r_u32(entry + 4) != size)
            continue;
        for (j = 0; j < entry_count; ++j)
            if (entries[j].hash == r_u32(entry + 8) && entries[j].size == size)
            {
                if (found) return NULL;
                found = entries[j].path;
            }
        break;
    }
    return found;
}

static FILE *restored_file(uint32_t index)
{
    uint32_t record = 0x800D65D0 + 20 * index;
    const char *path = original_path(r_u32(record + 4), r_u32(record + 12));
    return path ? xport_fopen(path, "rb") : NULL;
}

uint32_t rrj_data_close(uint32_t index)
{
    int result = 0;
    if (index >= 16 || !(r_u32(0x800D65D0 + 20 * index) & 1))
        return 0;
    if (files[index]) result = fclose(files[index]);
    files[index] = NULL;
    w_u32(0x800D65D0 + 20 * index, 0);
    return result ? UINT32_MAX : 0;
}

uint32_t rrj_data_read(uint32_t index, void *destination, uint32_t count)
{
    uint32_t record, position, size, read;
    if (index >= 16 || (count && !destination))
        return UINT32_MAX;
    record = 0x800D65D0 + 20 * index;
    if (!(r_u32(record) & 1))
        return UINT32_MAX;
    position = r_u32(record + 8);
    size = r_u32(record + 12);
    if (position > size || position > INT32_MAX)
        return UINT32_MAX;
    if (count > size - position) count = size - position;
    if (!count)
    {
        if (position == size) w_u32(record, r_u32(record) | 2);
        return 0;
    }
    if (!files[index]) files[index] = restored_file(index);
    if (!files[index] || fseek(files[index], (long)position, SEEK_SET))
        return UINT32_MAX;
    read = (uint32_t)xport_fread(destination, 1, count, files[index]);
    w_u32(record + 8, position + read);
    w_u32(record, (r_u32(record) & ~2u) | (position + read == size ? 2 : 0));
    return read;
}

void rrj_data_stream_close(void)
{
    if (stream_file) fclose(stream_file);
    stream_file = NULL;
}

uint32_t rrj_data_stream_read(uint32_t *count)
{
    uint32_t context = r_u32(0x8009C2D0);
    uint32_t descriptor = r_u32(context + 0x42C);
    uint32_t position = r_u32(descriptor + 4), size = r_u32(descriptor + 16);
    uint32_t packed = r_u32(descriptor + 12), minute, second, frame, base, buffer;
    const char *path;
    if (position == size && size && r_u32(context + 0x438))
    {
        position = 0;
        w_u32(descriptor + 4, 0);
    }
    if (position > size) goto fail;
    if (*count > size - position) *count = size - position;
    if (!*count) return 0;
    if (*count > r_u32(0x8009C2E4)) goto fail;
    minute = ((packed >> 4) & 15) * 10 + (packed & 15);
    second = ((packed >> 12) & 15) * 10 + ((packed >> 8) & 15);
    frame = ((packed >> 20) & 15) * 10 + ((packed >> 16) & 15);
    base = (minute * 60 + second) * 75 + frame - r_u32(0x8005B3D8);
    if (!stream_file || stream_base != base || stream_size != size)
    {
        rrj_data_stream_close();
        path = original_path(base, size);
        if (!path || !(stream_file = xport_fopen(path, "rb"))) goto fail;
        stream_base = base;
        stream_size = size;
    }
    buffer = r_u32(descriptor + 44);
    if (position > INT32_MAX || fseek(stream_file, (long)position, SEEK_SET) ||
        xport_fread(xport_guest_ptr(buffer, *count), 1, *count, stream_file) != *count)
        goto fail;
    return buffer;
fail:
    *count = 0;
    return 0;
}
