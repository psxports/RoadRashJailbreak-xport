#include "rrj_debug.h"
#include "fixed_math.h"
#include "game_main.h"
#include "race_leaf.h"
#include "race_pause.h"
#include "race_bodyless_batch_002.h"
#include "race_bodyless_batch_004.h"
#include "race_bodyless_batch_005.h"
#include "race_bodyless_batch_006.h"
#include "xport.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint8_t *sub_80064084(uint8_t *output, int32_t limit, uint32_t source)
{
    uint32_t cursor;
    uint32_t value;
    uint32_t length;
    FUNCTION_MARKER(0x80064084, "RASHCDI.BIN");
    if (!source || !r_u8(source))
        return NULL;
    cursor = source;
    if (limit > 0 && r_u8(source) != 13)
        for (;;)
        {
            if (r_u8(cursor) == 10)
                break;
            ++cursor;
            if (!cursor || (int32_t)(cursor - source) >= limit)
                break;
            value = r_u8(cursor);
            if (!value || value == 13)
                break;
        }
    length = cursor - source;
    (void)strncpy((char *)output, (const char *)xport_guest_cptr(source, length + 1), length + 1);
    output[length + 1] = 0;
    return output;
}

const uint8_t *sub_80064144(const uint8_t *source, int32_t *value_out)
{
    int32_t value;
    uint32_t character;
    const uint8_t *cursor;
    FUNCTION_MARKER(0x80064144, "RASHCDI.BIN");
    if (!source || !value_out)
        return NULL;
    value = atoi((const char *)source);
    xport_store_le32(value_out, (uint32_t)value);
    if (!value && !strchr((const char *)source, '0'))
        return NULL;
    cursor = source;
    character = *cursor++;
    if (character - 48 >= 10)
    {
        --cursor;
        while (character != 45 && character - 48 >= 10)
        {
            character = *++cursor;
            if (!character)
                return NULL;
        }
        ++cursor;
    }
    while ((uint32_t)*cursor - 48 < 10)
        ++cursor;
    return cursor;
}

const uint8_t *sub_800643B4(const uint8_t *source, uint8_t *output)
{
    const uint8_t *begin;
    const uint8_t *end;
    size_t length;
    FUNCTION_MARKER(0x800643B4, "RASHCDI.BIN");
    if (!source || !*source)
        return NULL;
    begin = source;
    while (*begin == 13 || *begin == 9 || *begin == 32)
        ++begin;
    if (!*begin)
        return NULL;
    end = begin + 1;
    while (*end && *end != 13 && *end != 9 && *end != 32)
        ++end;
    length = (size_t)(end - begin);
    (void)strncpy((char *)output, (const char *)begin, length);
    output[length] = 0;
    return end + 1;
}

const uint8_t *sub_80069B10(const uint8_t *guard, const uint8_t *source, uint32_t delimiter, int32_t *value_out)
{
    const uint8_t *cursor;
    FUNCTION_MARKER(0x80069B10, "RASHCDI.BIN");
    if (!value_out)
        return guard;
    xport_store_le32(value_out, 0);
    if (!guard)
        return NULL;
    if (!source)
        return guard;
    cursor = source;
    if (delimiter & 255)
    {
        cursor = (const uint8_t *)strchr((const char *)source, delimiter & 255);
        if (!cursor)
            return NULL;
        ++cursor;
    }
    (void)sub_80064144(cursor, value_out);
    return cursor;
}

int32_t sub_80069B8C(uint32_t data, int32_t bytes)
{
    uint8_t line[129];
    uint8_t token[129];
    const uint8_t *delimiter;
    uint32_t cursor = data;
    int32_t value = 0;
    FUNCTION_MARKER(0x80069B8C, "RASHCDI.BIN");
    if (bytes <= 0)
        return value;
    do
    {
        if (!sub_80064084(line, 127, cursor))
            break;
        token[0] = 0;
        (void)sub_800643B4(line, token);
        if (!strncmp((const char *)token, (const char *)xport_guest_cptr(0x8005B958, 13), 13))
        {
            delimiter = (const uint8_t *)strchr((const char *)line, '=');
            if (delimiter)
            {
                (void)sub_80064144(delimiter + 1, &value);
                return value;
            }
        }
        cursor += (uint32_t)strlen((const char *)line) + 1;
    } while ((int32_t)(cursor - data) < bytes);
    return value;
}

uint32_t sub_80069C60(uint32_t data, int32_t bytes, int32_t race, uint32_t *data_out, int32_t *bytes_out)
{
    uint8_t line[129];
    uint8_t token[129];
    const uint8_t *value_cursor;
    uint32_t cursor = data;
    uint32_t begin = 0;
    uint32_t end = 0;
    uint32_t selected = 0;
    int32_t value;
    FUNCTION_MARKER(0x80069C60, "RASHCDI.BIN");
    if (bytes > 0)
        do
        {
            if (!sub_80064084(line, 127, cursor))
                break;
            token[0] = 0;
            value_cursor = sub_800643B4(line, token);
            if (!strncmp((const char *)token, (const char *)xport_guest_cptr(0x8005B968, 7), 7))
                begin = cursor;
            else if (!strncmp((const char *)token, (const char *)xport_guest_cptr(0x8005B970, 8), 8))
            {
                (void)sub_80069B10(value_cursor, line, '=', &value);
                if (value == race)
                    selected = 1;
            }
            else if (!strncmp((const char *)token, (const char *)xport_guest_cptr(0x8005B97C, 5), 5) && selected)
            {
                end = cursor;
                break;
            }
            cursor += (uint32_t)strlen((const char *)line) + 1;
        } while ((int32_t)(cursor - data) < bytes);
    if (!begin || !end)
        return 0;
    xport_store_le32(data_out, begin);
    xport_store_le32(bytes_out, end - begin);
    return 1;
}

void sub_80069DEC(uint32_t data, int32_t bytes)
{
    uint8_t line[129];
    uint8_t token[129];
    const uint8_t *value_cursor;
    uint32_t begin;
    uint32_t cursor;
    uint32_t field;
    int32_t length;
    int32_t entries;
    int32_t value;
    FUNCTION_MARKER(0x80069DEC, "RASHCDI.BIN");
    (void)sub_8001E100(0x800D6170, 0, 40);
    w_u32(0x800D618C, data);
    w_u32(0x800D6190, bytes);
    entries = sub_80069B8C(data, bytes);
    w_u16(0x800D6178, entries);
    if ((int32_t)((uint32_t)entries << 16) > 0)
    {
        uint32_t selected_begin;
        int32_t selected_bytes;
        if (sub_80069C60(data, bytes, r_s32(r_u32(0x8005B2F8) + 64), &selected_begin, &selected_bytes))
        {
            w_u32(0x800D618C, selected_begin);
            w_u32(0x800D6190, selected_bytes);
        }
    }
    begin = r_u32(0x800D618C);
    length = r_s32(0x800D6190);
    cursor = begin;
    if (length <= 0)
        return;
    do
    {
        if (!sub_80064084(line, 127, cursor))
            break;
        token[0] = 0;
        value_cursor = sub_800643B4(line, token);
        if (!strncmp((const char *)token, (const char *)xport_guest_cptr(0x8005B984, 10), 10))
            field = 0x800D6182;
        else if (!strncmp((const char *)token, (const char *)xport_guest_cptr(0x8005B990, 25), 25))
            field = 0x800D617A;
        else if (!strncmp((const char *)token, (const char *)xport_guest_cptr(0x8005B9AC, 22), 22))
            field = 0x800D617C;
        else if (!strncmp((const char *)token, (const char *)xport_guest_cptr(0x8005B9C4, 26), 26))
            field = 0x800D617E;
        else if (!strncmp((const char *)token, (const char *)xport_guest_cptr(0x8005B9E0, 23), 23))
            field = 0x800D6180;
        else
            field = 0;
        if (field)
        {
            (void)sub_80069B10(value_cursor, line, '=', &value);
            w_u16(field, value);
        }
        cursor += (uint32_t)strlen((const char *)line) + 1;
    } while ((int32_t)(cursor - begin) < length);
}

uint32_t sub_8006A014(uint32_t entry)
{
    uint32_t result;
    uint32_t record;
    uint32_t distance;
    FUNCTION_MARKER(0x8006A014, "RASHCDI.BIN");
    result = (uint32_t)r_s16(0x800D6182);
    if (result == 0xFFFFFFFF || r_u32(entry) == 0xFFFFFFFF)
        return result;
    record = sub_800245DC(r_u32(entry));
    distance = r_u32(record + 4);
    w_u16(entry + 12, 0xFFFF);
    w_u16(entry + 14, 0xFFFF);
    w_u32(entry + 8, (distance >> 6) << 16);
    if (sub_800245F4(r_u32(record + 8)))
        w_u16(entry + 12, r_u16(record + 8));
    result = sub_800245F4(r_u32(record + 12));
    if (result)
    {
        result = r_u16(record + 12);
        w_u16(entry + 14, result);
    }
    return result;
}

uint32_t sub_8006A7C0(void)
{
    uint32_t index;
    uint32_t record = 0x800D5DA8;
    FUNCTION_MARKER(0x8006A7C0, "RASHCDI.BIN");
    for (index = 0; index < 18; ++index, record += 16)
    {
        w_u32(record, 0xFFFFFFFF);
        w_u32(record + 4, 0xFFFFFFFF);
        w_u32(record + 8, 0);
        w_u32(record + 12, 0);
    }
    return 0;
}

static uint32_t sub_8006A8FC(void)
{
    uint32_t index;
    uint32_t record = 0x800D4B10;
    FUNCTION_MARKER(0x8006A8FC, "RASHCDI.BIN");
    for (index = 0; index < 6; ++index, record += 16)
    {
        w_u32(record, 0xFFFFFFFF);
        w_u32(record + 4, 0);
        w_u32(record + 8, 0);
        w_u32(record + 12, 0);
    }
    w_u32(0x8005B320, 0);
    w_u32(0x8005B31C, 0xFFFFFFFF);
    return 0xFFFFFFFF;
}

uint32_t sub_8006A98C(void)
{
    FUNCTION_MARKER(0x8006A98C, "RASHCDI.BIN");
    w_u32(0x8005B338, 0);
    w_u32(0x8005B33C, 0);
    return sub_8006A8FC();
}

uint32_t sub_8006A9F4(uint32_t data, uint32_t bytes)
{
    uint32_t base = 0;
    uint32_t cursor = data;
    uint32_t end = data + bytes;
    uint32_t length;
    uint32_t payload;
    uint32_t pointer_offset;
    uint32_t count_offset;
    uint32_t count;
    const char *tag;
    FUNCTION_MARKER(0x8006A9F4, "RASHCDI.BIN");
    if (end < data)
        abort();
    while (cursor < end)
    {
        if (end - cursor < 8)
            abort();
        length = r_u32(cursor + 4);
        if (length < 8 || length > end - cursor)
            abort();
        payload = length - 8;
        tag = (const char *)xport_guest_cptr(cursor, 4);
        if (!strncmp(tag, (const char *)xport_guest_cptr(0x8005BA58, 4), 4))
            base = cursor + 8;
        else
        {
            pointer_offset = 0;
            count_offset = 0;
            count = 0;
            if (!strncmp(tag, (const char *)xport_guest_cptr(0x8005BA60, 4), 4))
            {
                pointer_offset = 28;
                count_offset = 40;
                count = payload >> 5;
            }
            else if (!strncmp(tag, (const char *)xport_guest_cptr(0x8005BA68, 4), 4))
            {
                pointer_offset = 32;
                count_offset = 42;
                count = payload >> 3;
            }
            else if (!strncmp(tag, (const char *)xport_guest_cptr(0x8005BA70, 4), 4))
            {
                pointer_offset = 36;
                count_offset = 44;
                count = payload / 104;
            }
            else if (!strncmp(tag, (const char *)xport_guest_cptr(0x8005BA78, 4), 4))
            {
                pointer_offset = 48;
                count_offset = 60;
                count = payload / 12;
            }
            else if (!strncmp(tag, (const char *)xport_guest_cptr(0x8005BA80, 4), 4))
            {
                pointer_offset = 52;
                count_offset = 62;
                count = payload / 12;
            }
            else if (!strncmp(tag, (const char *)xport_guest_cptr(0x8005BA88, 4), 4))
            {
                pointer_offset = 56;
                count_offset = 64;
                count = payload / 12;
            }
            if (pointer_offset)
            {
                if (!base)
                    abort();
                w_u32(base + pointer_offset, cursor + 8);
                w_u16(base + count_offset, count);
            }
        }
        cursor += r_u32(cursor + 4);
    }
    return base;
}

uint32_t sub_8006ABC8(const char *prefix)
{
    char path[128];
    uint32_t buffer;
    uint32_t bytes;
    int32_t path_length;
    FUNCTION_MARKER(0x8006ABC8, "RASHCDI.BIN");
    if (!prefix)
        abort();
    path_length = snprintf(path, sizeof(path), "%s%d.MAP", prefix, r_s32(r_u32(0x8005B2F8) + 48));
    if (path_length < 0 || (uint32_t)path_length >= sizeof(path))
        abort();
    if ((int32_t)sub_80014654(path, r_u32(0x8005AD5C), &buffer, &bytes) < 0)
        abort();
    w_u32(0x8005B338, buffer);
    w_u32(0x8005B240, sub_8006A9F4(buffer, bytes));
    return 1;
}

uint32_t sub_8006AC4C(const char *prefix)
{
    FUNCTION_MARKER(0x8006AC4C, "RASHCDI.BIN");
    (void)sub_8006ABC8(prefix);
    return 1;
}

void sub_8006A0C8(uint32_t data, int32_t bytes)
{
    uint8_t line[129];
    uint8_t token[129];
    const uint8_t *value_cursor;
    uint32_t selected_begin;
    int32_t selected_bytes;
    uint32_t cursor;
    uint32_t buffer;
    uint32_t allocation;
    uint32_t record = 0;
    uint32_t checkpoint;
    uint32_t slot;
    uint32_t index;
    uint32_t list_cursor;
    int32_t count;
    int32_t remaining = 0;
    int32_t value;
    int32_t identity;
    int32_t parameter;
    uint32_t start_pending = 1;
    uint32_t rows_active = 0;
    FUNCTION_MARKER(0x8006A0C8, "RASHCDI.BIN");
    if (!data)
        return;
    sub_80069DEC(data, bytes);
    selected_begin = r_u32(0x800D618C);
    selected_bytes = r_s32(0x800D6190);
    count = r_s16(0x800D6182);
    if (count < -1)
        abort();
    allocation = count == -1 ? 28 : 120 * ((uint32_t)count + 1) + 56;
    buffer = sub_8001447C(allocation, 0);
    w_u32(0x800D6170, buffer);
    if (!buffer)
        abort();
    w_u32(0x800D6184, buffer);
    (void)sub_8001E100(buffer, 0xFFFFFFFF, 28);
    if (count != -1)
    {
        w_u32(0x800D6188, buffer + 28);
        (void)sub_8001E100(buffer + 28, 0xFFFFFFFF, 28);
        w_u32(0x800D6194, buffer + 56);
    }
    cursor = selected_begin;
    if (selected_bytes <= 0)
        return;
    do
    {
        if (!sub_80064084(line, 127, cursor))
            return;
        token[0] = 0;
        value_cursor = sub_800643B4(line, token);
        if (!strncmp((const char *)token, (const char *)xport_guest_cptr(0x8005B9F8, 7), 7))
        {
            if (!start_pending)
                return;
            checkpoint = r_u32(0x800D6184);
            w_u32(checkpoint, 0);
            value_cursor = sub_80069B10(value_cursor, line, '=', &value);
            w_u32(checkpoint, value);
            value_cursor = sub_80069B10(value_cursor, value_cursor, ' ', &value);
            w_u32(checkpoint + 4, (uint32_t)value << 16);
            w_u32(checkpoint + 8, 0);
            value_cursor = sub_80069B10(value_cursor, value_cursor, ' ', &value);
            w_u32(checkpoint + 8, value);
            w_u32(checkpoint + 12, 0);
            (void)sub_80069B10(value_cursor, value_cursor, ' ', &value);
            w_u32(checkpoint + 12, value);
            if (count == -1)
                return;
            record = r_u32(0x800D6194) + 120 * (uint32_t)count;
            w_u32(record + 4, 0xFFFFF000);
            w_u32(record + 8, 0xFFFFF000);
            w_u32(record, 0xFFFFFFFF);
            w_u32(record + 12, 1);
            w_u32(record + 16, 1);
            w_u32(record + 20, r_u32(checkpoint));
            w_u32(record + 24, r_u32(checkpoint + 8));
            (void)sub_8006A014(record + 20);
            for (index = 0; index < 4; ++index)
            {
                if (index)
                {
                    (void)sub_8001E100(record + 20 + 16 * index, 0, 16);
                    w_u32(record + 20 + 16 * index, 0xFFFFFFFF);
                }
                w_u32(record + 84 + 4 * index, 0xFFFFFFFF);
                w_u32(record + 100 + 4 * index, 0xFFFFFFFF);
            }
            w_u16(record + 116, 0xFFFF);
            w_u16(record + 118, 0);
            w_u32(record + 84, r_u32(checkpoint));
            w_u32(record + 100, r_u32(checkpoint + 12));
            start_pending = 0;
        }
        else if (!strncmp((const char *)token, (const char *)xport_guest_cptr(0x8005BA00, 8), 8))
        {
            checkpoint = r_u32(0x800D6188);
            if (!checkpoint)
                abort();
            w_u32(checkpoint, 0);
            value_cursor = sub_80069B10(value_cursor, line, '=', &value);
            w_u32(checkpoint, value);
            value_cursor = sub_80069B10(value_cursor, value_cursor, ' ', &value);
            w_u32(checkpoint + 4, (uint32_t)value << 16);
            w_u32(checkpoint + 8, 0);
            value_cursor = sub_80069B10(value_cursor, value_cursor, ' ', &value);
            w_u32(checkpoint + 8, value);
            w_u32(checkpoint + 12, 0);
            (void)sub_80069B10(value_cursor, value_cursor, ' ', &value);
            w_u32(checkpoint + 12, value);
        }
        else if (!strncmp((const char *)token, (const char *)xport_guest_cptr(0x8005BA0C, 15), 15))
        {
            checkpoint = r_u32(0x800D6184);
            for (index = 0; index < 3; ++index)
            {
                w_u32(checkpoint + 16 + 4 * index, 0);
                value_cursor = sub_80069B10(value_cursor, index ? value_cursor : line, index ? ' ' : '=', &value);
                w_u32(checkpoint + 16 + 4 * index, value);
            }
        }
        else if (!strncmp((const char *)token, (const char *)xport_guest_cptr(0x8005BA1C, 16), 16))
        {
            checkpoint = r_u32(0x800D6188);
            if (!checkpoint)
                abort();
            for (index = 0; index < 3; ++index)
            {
                w_u32(checkpoint + 16 + 4 * index, 0);
                value_cursor = sub_80069B10(value_cursor, index ? value_cursor : line, index ? ' ' : '=', &value);
                w_u32(checkpoint + 16 + 4 * index, value);
            }
        }
        else if (!strncmp((const char *)token, (const char *)xport_guest_cptr(0x8005BA30, 8), 8))
        {
            w_u32(0x800D6174, 0);
            (void)sub_80069B10(value_cursor, line, '=', &value);
            w_u32(0x800D6174, value);
        }
        else if (!strncmp((const char *)token, (const char *)xport_guest_cptr(0x8005BA3C, 8), 8))
            (void)sub_80069B10(value_cursor, line, '=', &value);
        else if (!strncmp((const char *)token, (const char *)xport_guest_cptr(0x8005B984, 10), 10))
        {
            remaining = r_s16(0x800D6182);
            rows_active = remaining > 0;
            record = r_u32(0x800D6194);
        }
        else if (rows_active == 1)
        {
            (void)sub_8001E100(record, 0xFFFFFFFF, 120);
            w_u32(record, 0);
            value_cursor = sub_80069B10(value_cursor, token, 0, &value);
            w_u32(record, value);
            if (value == -1)
                rows_active = 0;
            else
            {
                value_cursor = sub_80069B10(value_cursor, line, ' ', &value);
                w_u32(record + 4, (uint32_t)value << 12);
                value_cursor = sub_80069B10(value_cursor, value_cursor, ' ', &value);
                w_u32(record + 12, 0);
                w_u32(record + 8, (uint32_t)value << 12);
                for (index = 0; index < 4; ++index)
                {
                    value_cursor = sub_80069B10(value_cursor, value_cursor, ' ', &identity);
                    value_cursor = sub_80069B10(value_cursor, value_cursor, ' ', &parameter);
                    if (identity != -1)
                    {
                        slot = record + 20 + 16 * r_u32(record + 12);
                        w_u32(slot, identity);
                        w_u32(slot + 4, parameter);
                        (void)sub_8006A014(slot);
                        w_u32(record + 12, r_u32(record + 12) + 1);
                    }
                }
                w_u32(record + 16, 0);
                for (index = 0; index < 4; ++index)
                {
                    value_cursor = sub_80069B10(value_cursor, value_cursor, ' ', &value);
                    if (value != -1)
                    {
                        w_u32(record + 84 + 4 * r_u32(record + 16), value);
                        w_u32(record + 16, r_u32(record + 16) + 1);
                    }
                }
                list_cursor = record + 100;
                for (index = 0; index < 4; ++index)
                {
                    value_cursor = sub_80069B10(value_cursor, value_cursor, ' ', &value);
                    if (value != -1)
                    {
                        w_u32(list_cursor, value);
                        list_cursor += 4;
                    }
                }
                w_u16(record + 116, 0);
                w_u16(record + 118, 0);
                record += 120;
                --remaining;
                if (!remaining)
                    rows_active = 0;
            }
        }
        cursor += (uint32_t)strlen((const char *)line) + 1;
    } while ((int32_t)(cursor - selected_begin) < selected_bytes);
}

uint32_t sub_8006A7F8(const char *prefix)
{
    char path[128];
    uint32_t owner;
    uint32_t buffer;
    uint32_t capacity;
    uint32_t handle;
    int32_t bytes;
    int32_t path_length;
    FUNCTION_MARKER(0x8006A7F8, "RASHCDI.BIN");
    owner = r_u32(0x8005ACBC);
    buffer = r_u32(owner + 2616);
    capacity = r_u32(owner + 2648) << 14;
    if (!prefix || !buffer || !capacity)
        abort();
    path_length = snprintf(path, sizeof(path), "%sgrf%1d.txt", prefix, r_s32(r_u32(0x8005B2F8) + 48));
    if (path_length < 0 || (uint32_t)path_length >= sizeof(path))
        abort();
    handle = sub_8001458C(path, r_u32(0x8005AD5C));
    if ((int32_t)handle < 0)
        abort();
    bytes = (int32_t)sub_80014780(handle, xport_guest_ptr(buffer, capacity), capacity);
    (void)sub_8001460C(handle);
    sub_8006A0C8(buffer, bytes);
    (void)sub_8006A7C0();
    return 1;
}

uint32_t sub_8006AC6C(const char *prefix)
{
    FUNCTION_MARKER(0x8006AC6C, "RASHCDI.BIN");
    w_u32(0x8005AD54, 0);
    (void)sub_8006A7F8(prefix);
    (void)sub_8006A98C();
    return (sub_8006AC4C(prefix) & 255) != 0;
}

uint32_t sub_80063670(void)
{
    uint32_t state;
    uint32_t mode;
    uint32_t table;
    uint32_t checkpoint;
    uint32_t actor;
    uint32_t trailer;
    uint32_t actor_count;
    uint32_t display_count;
    uint32_t extra;
    int32_t hazard = 1;
    FUNCTION_MARKER(0x80063670, "RASHCDI.BIN");
    w_u32(0x8005ACD0, 0);
    w_u32(0x8005ACCC, 0);
    state = r_u32(0x8005B2F8);
    w_u32(0x8005ACC8, 0);
    mode = r_u8(state + 4);
    w_u32(state + 16, 0);
    if (mode & 1)
    {
        table = mode & 16 ? 0x80053090 : 0x80053084;
        w_u32(0x8005ACC8, 300 * r_u32(table + 4 * r_u32(state + 60)));
    }
    if (r_u8(state + 4) == 36)
        w_u32(0x8005ACC8, 300 * r_u16(state + 8));
    if (r_u8(state + 4) == 33)
        w_u32(0x8005ACC8, 300 * r_u32(0x8005ADE0));
    if (r_u8(state + 4) == 44)
        w_u32(0x8005ACC8, 300 * r_u32(0x8005ADE4));
    (void)sub_8005D130(0x800CE170);
    w_u32(0x8005AD4C, sub_800653E8((const char *)xport_guest_cptr(0x800524DC, 1)));
    (void)sub_800244E0();
    (void)sub_8006AC6C((const char *)xport_guest_cptr(0x80052400, 1));
    (void)sub_80022F78();
    (void)sub_80023020();
    checkpoint = r_u32(0x800D6184);
    (void)sub_8006ACD0(r_u32(checkpoint), r_u32(checkpoint + 4), r_u32(checkpoint + 8));
    sub_8006982C();
    if (r_u8(r_u32(0x8005B2F8) + 4) & 16)
        (void)sub_80018DC8(0);
    else
    {
        hazard = sub_8006AD4C();
        (void)sub_80018DC8((uint32_t)hazard);
    }
    (void)sub_8005C7F0(hazard);
    actor_count = r_u32(0x8005B218) + 4 * r_u32(0x8005B254);
    mode = r_u8(r_u32(0x8005B2F8) + 4);
    actor_count += mode == 8 ? 2 : 1;
    if (r_u8(r_u32(0x8005B2F8) + 4) == 44)
        actor_count += 4;
    (void)sub_8005D1A0(0x800CE170, (int32_t)actor_count);
    if (r_u32(0x8005B220))
        w_u32(r_u32(0x8005B2F8) + 52, 4);
    else
    {
        actor = r_u32(0x8005B38C);
        extra = 0;
        if (r_u8(r_u32(actor + 852) + 572) & 16)
        {
            trailer = r_u32(actor + 856);
            extra = (r_u8(r_u32(trailer + 852) + 572) & 64) == 0;
        }
        state = r_u32(0x8005B2F8);
        actor = r_u32(0x8005B21C);
        if (actor)
        {
            display_count = extra + 2;
            extra = 0;
            if (r_u8(r_u32(actor + 852) + 572) & 16)
            {
                trailer = r_u32(actor + 856);
                extra = (r_u8(r_u32(trailer + 852) + 572) & 64) == 0;
            }
            display_count += extra;
        }
        else
            display_count = extra + 1;
        w_u32(state + 52, display_count);
        state = r_u32(0x8005B2F8);
        if (r_u32(state + 52) >= 3)
            w_u32(state + 52, 4);
    }
    if (!(r_u8(r_u32(0x8005B2F8) + 4) & 16))
        sub_8006AEF4();
    (void)sub_800235B0(0, r_u32(0x8005B38C));
    (void)sub_800235B0(1, r_u32(0x8005B21C));
    (void)sub_8002B83C();
    (void)sub_8005CEE0(0);
    if (r_u32(r_u32(0x8005B2F8) + 48) >= 2)
        return sub_8005CEE0(1);
    return 1;
}

uint32_t sub_80069000(void)
{
    uint32_t slots;
    uint32_t buffer;
    uint32_t row;
    uint32_t index;
    uint32_t field;
    uint32_t result = 0x800D0000;
    int32_t player;
    FUNCTION_MARKER(0x80069000, "RASHCDI.BIN");
    w_u32(0x800CD6C4, 0);
    w_u32(0x800CD6B4, 0);
    slots = r_u8(r_u32(0x8005B2F8) + 4) & 16 ? 32 : 24;
    w_u32(0x8005B214, slots);
    buffer = sub_8001447C(280 * slots, 0);
    w_u32(0x800CD6C4, buffer);
    if (buffer)
    {
        (void)sub_8001E100(buffer, 0, 280 * r_u32(0x8005B214));
        w_u32(0x800CD6B4, r_u32(0x8005B214) - 1);
    }
    w_u32(0x800D5CF8, 0);
    w_u32(0x800D5CFC, 0);
    w_u32(0x800D5D18, 0);
    w_u32(0x800D5D1C, 0);
    for (player = 0; player < r_s32(r_u32(0x8005B2F8) + 48); ++player)
    {
        row = 0x800D5CF8 + 32 * (uint32_t)player;
        slots = r_u32(0x8005B214);
        w_u32(row, slots);
        buffer = sub_8001447C(8 * slots, 0);
        w_u32(row + 4, buffer);
        if (buffer)
        {
            for (index = 0; (int32_t)index < r_s32(0x8005B214); ++index)
            {
                w_u16(r_u32(row + 4) + 8 * index, index);
                w_u16(r_u32(row + 4) + 8 * index + 2, 0xFFFF);
                w_u32(r_u32(row + 4) + 8 * index + 4, 0xFFFFFFFF);
            }
            for (field = 0; field < 3; ++field)
            {
                w_u32(row + 8 + 4 * field, 0);
                w_u32(row + 20 + 4 * field, 0xFFFFFFFF);
            }
        }
    }
    w_u32(0x800D4B80, 0);
    w_u32(0x800D4B7C, 0);
    if (r_u32(0x8005B254))
    {
        result = sub_8001447C(2288, 0);
        w_u32(0x800D4B80, result);
        if (result)
        {
            (void)sub_8001E100(result, 0, 2288);
            result = 3;
            w_u32(0x800D4B7C, result);
        }
    }
    return result;
}

uint32_t sub_8006968C(void)
{
    FUNCTION_MARKER(0x8006968C, "RASHCDI.BIN");
    if (!r_u32(0x8005AD34))
        w_u32(0x8005AD34, 1);
    return 1;
}

uint32_t sub_800645E4(const uint8_t *data, int32_t offset)
{
    uint32_t index;
    FUNCTION_MARKER(0x800645E4, "RASHCDI.BIN");
    if (!data || offset < 0)
        abort();
    for (index = 0; index < 12; ++index)
        w_u8(0x80052FA0 + index, data[(uint32_t)offset + index]);
    return 0x80052FA0;
}

uint32_t sub_80068D54(void)
{
    uint32_t index;
    uint32_t row;
    uint32_t saved;
    uint32_t state;
    int32_t player;
    FUNCTION_MARKER(0x80068D54, "RASHCDI.BIN");
    for (index = 0; index < 16; ++index)
    {
        row = 0x800CF650 + 512 * index;
        saved = r_u32(row + 20);
        (void)sub_8001E100(row + 16, 0, 512);
        w_u32(row + 20, saved);
    }
    w_u32(0x800CF650, 0);
    state = r_u32(0x8005B2F8);
    w_u32(0x800CF654, 0);
    w_u32(0x800CF658, 0xFFFFFFFF);
    for (player = 0; player < r_s32(state + 48); ++player)
    {
        w_u32(0x800D8714 + 4 * (uint32_t)player, 16);
        w_u32(0x800D871C + 4 * (uint32_t)player, 4);
    }
    w_u32(0x800D8728, 4);
    w_u32(0x800D8724, 16);
    w_u32(0x800D872C, 180);
    w_u32(0x800D8730, 145);
    w_u16(0x800D8734, 100);
    w_u16(0x800D8736, 75);
    w_u16(0x800D8738, 50);
    if (r_u32(0x800D4B80))
        for (index = 0; index < 4; ++index)
        {
            row = r_u32(0x800D4B80) + 572 * index;
            saved = r_u32(row + 4);
            (void)sub_8001E100(row, 0, 572);
            w_u32(r_u32(0x800D4B80) + 572 * index + 4, saved);
        }
    w_u32(0x800D4B70, 0);
    w_u32(0x800D4B74, 0);
    w_u32(0x800D4B78, 0xFFFFFFFF);
    w_u32(0x800D8748, 10);
    w_u32(0x800D8750, 10);
    w_u32(0x800D8754, 150);
    w_u32(0x800D8758, 300);
    w_u32(0x800D8744, 4);
    w_u32(0x800D874C, 4);
    w_u32(0x800D875C, 5);
    w_u32(0x800CD6D4, 0x800D1818);
    w_u32(0x800CD6C8, 0);
    w_u32(0x800CD6CC, 0);
    w_u32(0x800CD6D0, 0xFFFFFFFF);
    w_u32(0x800CE5A4, 0x800D36EC);
    w_u32(0x800CE598, 0);
    w_u32(0x800CE59C, 0);
    w_u32(0x800CE5A0, 0xFFFFFFFF);
    for (index = 0; (int32_t)index < r_s32(0x8005B214); ++index)
        (void)sub_8001E100(r_u32(0x800CD6C4) + 280 * index, 0, 280);
    w_u32(0x800CD6A8, 0);
    w_u32(0x800CD6AC, 0);
    w_u32(0x800CD6B0, 0xFFFFFFFF);
    w_u32(0x800D1814, 8344);
    w_u32(0x8005B1FC, 0);
    w_u32(0x8005B1F8, 0);
    w_u32(0x8005B218, 0);
    w_u32(0x8005AD38, 0xFFFFFFFF);
    w_u32(0x800D1660, 0);
    w_u32(0x8005AD3C, 0xFFFFFFFF);
    (void)sub_8001E100(0x8005B2B8, 0, 4);
    w_u8(0x8005AD32, 0);
    return 0x80060000;
}

uint32_t sub_80068AA4(void)
{
    uint8_t saved[8] = {0};
    uint32_t index;
    uint32_t field;
    uint32_t state;
    uint32_t mode;
    uint32_t reset;
    uint32_t row;
    FUNCTION_MARKER(0x80068AA4, "RASHCDI.BIN");
    if (r_u8(0x8005AD32))
        return 0x80060000;
    state = r_u32(0x8005B2F8);
    mode = r_u8(state + 4);
    reset = 0;
    if ((!(mode & 32) && mode != 1 && mode != 8) || (r_u8(0x800D80DD) & 8))
        reset = 1;
    else if (!r_u32(state + 60) && (r_u8(state + 5) & 1))
        reset = 1;
    for (index = 0; index < 20; ++index)
    {
        row = 0x800D5758 + 72 * index;
        if (!reset)
            for (field = 0; field < 8; ++field)
                saved[field] = r_u8(row + 44 + field);
        (void)sub_8001E100(row, 0, 72);
        if (!reset)
            for (field = 0; field < 8; ++field)
                w_u8(row + 44 + field, saved[field]);
        if (index == 1)
        {
            state = r_u32(0x8005B2F8);
            reset = r_u8(state + 4) == 34 ? (r_u8(state + 5) & 1) != 0 : 1;
        }
        else if (index == 17)
            reset = 1;
    }
    (void)sub_80068D54();
    w_u8(0x8005AD32, 1);
    w_u32(0x800CE4D4, 1096);
    w_u32(0x800CE4E4, 628);
    w_u32(0x800CE4F4, 572);
    w_u32(0x800CE504, 512);
    w_u32(0x800CE514, 596);
    w_u32(0x800CE524, 0xFFFFFE3C);
    w_u32(0x800CE534, 280);
    w_u32(0x800CE4D8, 0x8005B1F8);
    w_u32(0x800CE4E8, 0x8005B218);
    w_u32(0x800CE508, 0x800CF650);
    w_u32(0x800CE4DC, 0x8005AD38);
    w_u32(0x800CE4EC, 0x8005AD3C);
    w_u32(0x800CE500, 0x800CF660);
    w_u32(0x800CE4FC, 0x800D4B78);
    w_u32(0x800CE51C, 0x800CD6D0);
    w_u32(0x800CE4F8, 0x800D4B70);
    w_u32(0x800CE518, 0x800CD6C8);
    w_u32(0x800CE528, 0x800CE598);
    w_u32(0x800CE538, 0x800CD6A8);
    w_u32(0x800CE50C, 0x800CF658);
    w_u32(0x800CE52C, 0x800CE5A0);
    w_u32(0x800CE53C, 0x800CD6B0);
    w_u32(0x800CE4F0, r_u32(0x800D4B80));
    w_u32(0x800CE510, r_u32(0x800CD6D4));
    w_u32(0x800CE520, r_u32(0x800CE5A4));
    w_u32(0x800CE530, r_u32(0x800CD6C4));
    return sub_8001E100(0x800D43C0, 0, 56);
}

uint32_t sub_80067784(void)
{
    uint32_t result;
    uint32_t actor;
    int32_t remaining;
    FUNCTION_MARKER(0x80067784, "RASHCDI.BIN");
    result = r_u32(0x800CE4DC);
    remaining = r_s32(result);
    actor = r_u32(0x800CE4D0);
    while (remaining >= 0)
    {
        w_u32(actor + 832, 0);
        (void)sub_8002090C(actor);
        result = r_u32(0x800CE4D4);
        --remaining;
        actor += result;
    }
    return result;
}

uint32_t sub_80068500(void)
{
    uint32_t index;
    FUNCTION_MARKER(0x80068500, "RASHCDI.BIN");
    (void)sub_8001E0DC(0x800D5F40, 0, 26);
    for (index = 0; index < 13; ++index)
        w_u16(0x800D5F40 + 2 * index, 0x5000 + index);
    w_u16(0x800D5730, 73);
    w_u16(0x800D5736, 230);
    w_u16(0x800D573C, 230);
    w_u16(0x800D5742, 50);
    w_u16(0x800D5744, 4);
    w_u16(0x800D5748, 70);
    w_u16(0x800D5732, 0);
    w_u8(0x800D5734, 0);
    w_u16(0x800D5738, 0);
    w_u8(0x800D573A, 0);
    w_u16(0x800D573E, 0);
    w_u8(0x800D5740, 0);
    w_u8(0x800D5746, 0);
    w_u16(0x800D574A, 0);
    w_u8(0x800D574C, 0);
    w_u16(0x800D574E, 69);
    w_u16(0x800D5750, 0);
    w_u8(0x800D5752, 0);
    return 0x800D5730;
}

uint32_t sub_80069394(void)
{
    uint32_t index;
    uint32_t base;
    uint32_t row;
    uint32_t buffer;
    FUNCTION_MARKER(0x80069394, "RASHCDI.BIN");
    base = r_u32(0x800D4B80);
    if (base)
        for (index = 0; index < 4; ++index)
        {
            row = base + 572 * index;
            buffer = sub_8001447C(408, 0);
            w_u32(row + 4, buffer);
            if (buffer)
            {
                w_u32(row, 0);
                w_u32(row + 96, 0);
                w_u32(row + 540, 0);
                w_u32(0x800D4B7C, r_u32(0x800D4B7C) + 1);
            }
        }
    for (index = 0; index < 16; ++index)
    {
        row = 0x800CF660 + 512 * index;
        buffer = sub_8001447C(24, 0);
        w_u32(row + 4, buffer);
        if (buffer)
        {
            w_u32(row, 0);
            w_u32(row + 96, 0);
            w_u32(0x800CF65C, r_u32(0x800CF65C) + 1);
        }
    }
    w_u32(0x800D1814, 8344);
    w_u32(0x800D1660, 0);
    for (index = 0; index < 18; ++index)
        w_u32(0x800D1664 + 24 * index, 0);
    return 8344;
}

uint32_t sub_80068614(uint32_t kind, uint32_t *attached_out)
{
    uint32_t total = 0;
    uint32_t attached = 0;
    uint32_t actor;
    int32_t remaining;
    FUNCTION_MARKER(0x80068614, "RASHCDI.BIN");
    remaining = r_s32(r_u32(0x800CE4DC));
    actor = r_u32(0x800CE4D0);
    while (remaining >= 0)
    {
        if ((uint32_t)(r_u8(r_u32(actor + 1084) + 1) & 15) == kind)
        {
            ++total;
            if (r_s16(actor + 320))
                ++attached;
        }
        --remaining;
        actor += r_u32(0x800CE4D4);
    }
    xport_store_le32(attached_out, attached);
    return total;
}

uint32_t sub_80068684(void)
{
    uint32_t attached;
    uint32_t result;
    FUNCTION_MARKER(0x80068684, "RASHCDI.BIN");
    result = sub_80068614(2, &attached);
    w_u32(0x800D86F4, result);
    w_u32(0x800D86F0, attached);
    return result;
}

uint32_t sub_800689D0(const uint8_t data[280])
{
    uint32_t result;
    FUNCTION_MARKER(0x800689D0, "RASHCDI.BIN");
    result = r_u8(0x8005AD31);
    if (!result)
    {
        if (r_u32(0x8005B254))
            (void)sub_80068500();
        (void)sub_80069394();
        (void)sub_80068684();
        sub_80068470(data);
        result = 1;
        w_u8(0x8005AD31, result);
    }
    return result;
}

uint32_t sub_80065468(void)
{
    FUNCTION_MARKER(0x80065468, "RASHCDI.BIN");
    w_u32(0x8005B298, 0x8005B298);
    w_u32(0x8005B29C, 0x8005B298);
    w_u32(0x8005B350, 0x8005B350);
    w_u32(0x8005B354, 0x8005B350);
    w_u32(0x8005B2D8, 0x8005B2D8);
    w_u32(0x8005B2DC, 0x8005B2D8);
    w_u32(0x8005B378, 0x8005B378);
    w_u32(0x8005B37C, 0x8005B378);
    w_u32(0x8005B270, 0x8005B270);
    w_u32(0x8005B274, 0x8005B270);
    return 0x8005B270;
}

uint32_t sub_800654BC(uint32_t buffer)
{
    char path[128];
    uint32_t handle;
    uint32_t bytes;
    uint32_t result;
    int32_t length;
    FUNCTION_MARKER(0x800654BC, "RASHCDI.BIN");
    length = snprintf(path, sizeof(path), "%sLEVEL%d.ph", (const char *)xport_guest_cptr(0x8005243C, 1),
        (int32_t)(r_u32(r_u32(0x8005B2F8) + 60) + 1));
    if (!buffer || length < 0 || (uint32_t)length >= sizeof(path))
        abort();
    handle = sub_8001458C(path, r_u32(0x8005AD5C));
    if ((int32_t)handle < 0)
    {
        RRJ_DEBUG_FPRINTF(stderr, "Missing race resource: %s\n", path);
        abort();
    }
    bytes = sub_80014780(handle, xport_guest_ptr(buffer, 1344), 1344);
    result = sub_8001460C(handle);
    if (bytes != 1344)
        abort();
    return result;
}

uint32_t sub_80064B44(uint8_t *buffer, int32_t capacity)
{
    char path[128];
    uint32_t handle;
    uint32_t bytes;
    uint32_t result;
    int32_t length;
    FUNCTION_MARKER(0x80064B44, "RASHCDI.BIN");
    length = snprintf(path, sizeof(path), "%sLEVEL%d.BI", (const char *)xport_guest_cptr(0x80052464, 1),
        (int32_t)(r_u32(r_u32(0x8005B2F8) + 60) + 1));
    if (!buffer || length < 0 || (uint32_t)length >= sizeof(path))
        abort();
    handle = sub_8001458C(path, r_u32(0x8005AD5C));
    if ((int32_t)handle < 0)
    {
        RRJ_DEBUG_FPRINTF(stderr, "Missing race resource: %s\n", path);
        abort();
    }
    if (capacity < 1728)
    {
        (void)sub_8001460C(handle);
        abort();
    }
    bytes = sub_80014780(handle, buffer, 1728);
    result = sub_8001460C(handle);
    if (bytes != 1728)
        abort();
    return result;
}

uint32_t sub_80067B00(const uint8_t *data, int32_t bytes)
{
    uint32_t tuples[18][3];
    uint32_t fallback[3] = {0};
    uint8_t resources[2048] = {0};
    const uint8_t *input;
    uint32_t mask = 0;
    uint32_t state;
    uint32_t players;
    uint32_t groups;
    uint32_t mode;
    uint32_t capacity;
    uint32_t buffer;
    uint32_t actor;
    uint32_t head;
    uint32_t node;
    uint32_t model;
    uint32_t variant;
    uint32_t flags;
    uint32_t temporary;
    uint32_t result;
    int32_t requested = 0;
    int32_t special = 0;
    int32_t removed;
    int32_t limit;
    int32_t index;
    int32_t source_index;
    int32_t target_index;
    int32_t search;
    FUNCTION_MARKER(0x80067B00, "RASHCDI.BIN");
    result = r_u8(0x8005AD30);
    if (result)
        return result;
    w_u32(0x8005B21C, 0);
    w_u32(0x8005B26C, 0);
    w_u32(0x8005B38C, 0);
    w_u32(0x8005B244, 0);
    w_u32(0x8005AD48, 0);
    w_u32(0x8005B268, 0);
    w_u32(0x8005B230, 0);
    (void)sub_8001E100(0x800D38C8, 31, 20);
    (void)sub_8001E0DC(0x800D38B0, 31, 18);
    players = r_u32(r_u32(0x8005B2F8) + 48);
    if (!players || players > 2)
        abort();
    groups = players == 1 ? 4 : 5;
    buffer = sub_8001447C(448 * groups, 0);
    w_u32(0x8005B248, buffer);
    if (!buffer)
        abort();
    (void)sub_80065468();
    (void)sub_800654BC(r_u32(0x8005B248));
    w_u32(0x8005AD40, (1 << ((groups - players) & 31)) - 1);
    (void)sub_80064B44(resources, sizeof(resources));
    input = data;
    w_u32(0x8005AD40, r_u32(0x8005AD40) | (groups << 24));
    if (bytes)
    {
        if (!input || bytes < 4)
            abort();
        requested = (int32_t)xport_load_le32(input);
        input += 4;
    }
    if (requested >= 19)
        requested = 18;
    state = r_u32(0x8005B2F8);
    mode = r_u8(state + 4);
    if (mode & 4)
    {
        if (!r_u8(state + 56))
            requested = mode & 8 ? 2 : 1;
        else if (r_u8(state + 56) == 2)
            requested = mode & 8 ? 4 : 3;
    }
    if (requested < 0 || requested > 18 || (requested && (!input || bytes < 4 + 12 * requested)))
        abort();
    capacity = mode & 8 ? players << 1 : players;
    if ((int32_t)capacity < requested)
        capacity = (uint32_t)requested;
    if (mode & 8)
    {
        limit = 12 + (players == 1 ? (mode == 44 ? 3 : 2) : 0);
        if (requested < limit)
            capacity += players;
        if (capacity >= (uint32_t)limit + players)
            capacity = (uint32_t)limit + players;
    }
    w_u8(0x8005B2B8, capacity);
    w_u8(0x8005B2BA, capacity);
    buffer = sub_8001447C(1096 * capacity, 0);
    w_u32(0x8005B3A0, buffer);
    buffer = sub_8001447C(628 * capacity, 0);
    w_u32(0x8005B3A4, buffer);
    if (!r_u32(0x8005B3A0) || !buffer)
        abort();
    (void)sub_8001E100(r_u32(0x8005B3A0), 0, 1096 * capacity);
    (void)sub_8001E100(r_u32(0x8005B3A4), 0, 628 * capacity);
    w_u32(0x800CE4D0, r_u32(0x8005B3A0));
    w_u32(0x800CE4E0, r_u32(0x8005B3A4));
    memset(tuples, 255, sizeof(tuples));
    for (index = 0; index < requested; ++index)
    {
        tuples[index][2] = xport_load_le32(input);
        input += 4;
        tuples[index][0] = xport_load_le32(input);
        input += 4;
        tuples[index][1] = xport_load_le32(input);
        input += 4;
        special += (int32_t)tuples[index][2] >= 17;
    }
    state = r_u32(0x8005B2F8);
    mode = r_u8(state + 4);
    if (mode == 17)
    {
        if (r_s32(state + 76) >= 18)
            for (index = 0; index < 2; ++index)
            {
                temporary = tuples[0][index];
                tuples[0][index] = tuples[1][index];
                tuples[1][index] = temporary;
            }
    }
    else if ((mode & 4) && r_u8(state + 56) == 2)
    {
        tuples[1][2] = 17;
        tuples[2][2] = 18;
    }
    else if (mode & 8)
    {
        removed = requested - special - 10 - (players == 1 ? (mode == 44 ? 3 : 2) : 0);
        if (removed > 0)
        {
            requested -= removed;
            index = (int32_t)players;
            w_u32(0x8005B1FC, players);
            source_index = index + removed;
            while (index < requested)
            {
                limit = 10 + (players == 1 ? (mode == 44 ? 3 : 2) : 0);
                if (r_s32(0x8005B1FC) >= limit)
                    break;
                if ((int32_t)tuples[index][2] < 17)
                {
                    search = source_index;
                    while (search < 18 && (int32_t)tuples[search][2] >= 17)
                        ++search;
                    if (search >= 18)
                        abort();
                    tuples[index][0] = tuples[search][0];
                    tuples[index][1] = tuples[search][1];
                    w_u32(0x8005B1FC, r_u32(0x8005B1FC) + 1);
                }
                ++index;
                ++source_index;
            }
            target_index = index;
            source_index = index + 1;
            while (source_index < requested + removed)
            {
                if ((int32_t)tuples[source_index][2] >= 17)
                {
                    memcpy(tuples[target_index], tuples[source_index], sizeof(tuples[0]));
                    ++target_index;
                }
                ++source_index;
            }
        }
    }
    w_u32(0x8005B1FC, 0);
    for (index = 0; index < requested; ++index)
    {
        model = tuples[index][2];
        tuples[index][2] = 0;
        flags = (index == 0) | ((players == 2 && index == 1) ? 8 : 0);
        actor = sub_80065A94(tuples[index], resources, model, &mask, flags);
        if (!actor)
            abort();
        if (r_u16(actor + 172) < r_u32(r_u32(0x8005B2F8) + 48) || (r_u8(r_u32(actor + 1084) + 1) & 15) != 2)
            w_u32(0x8005B1FC, r_u32(0x8005B1FC) + 1);
        head = r_s16(actor + 320) ? 0x8005B298 : 0x8005B270;
        node = actor + 1088;
        w_u32(r_u32(head + 4), node);
        w_u32(actor + 1092, r_u32(head + 4));
        w_u32(head + 4, node);
        w_u32(node, head);
    }
    actor = r_u32(0x800CE4D0);
    for (index = 0; index < r_s32(0x8005B1F8); ++index, actor += 1096)
    {
        result = sub_800138E8(actor, 0);
        w_u8(r_u32(actor + 1084) + 39, result);
        (void)sub_800650A0(actor);
    }
    if (!r_u32(0x8005B1F8))
    {
        for (index = 0; (uint32_t)index < r_u32(r_u32(0x8005B2F8) + 48); ++index)
        {
            fallback[0] = (uint32_t)index << 17;
            actor = sub_80065A94(fallback, NULL, (uint32_t)index, &mask, index ? 8 : 1);
            if (!actor)
                abort();
            node = actor + 1088;
            w_u32(r_u32(0x8005B29C), node);
            w_u32(actor + 1092, r_u32(0x8005B29C));
            w_u32(0x8005B29C, node);
            w_u32(node, 0x8005B298);
            w_u8(r_u32(actor + 1084) + 39, index + 1);
        }
        w_u32(0x8005B1FC, r_u32(r_u32(0x8005B2F8) + 48));
    }
    actor = r_u32(0x8005B38C);
    if (!actor)
        abort();
    model = r_u32(actor + 180);
    if (model - 6 < 3 || model - 15 < 3)
    {
        variant = r_u32(r_u32(0x8005B2F8) + 48) == 1 ? 1 : (model < 9 ? 9 : 17);
        (void)sub_800670FC(actor, resources, variant, r_s32(0x8005B1F8));
    }
    actor = r_u32(0x8005B21C);
    if (actor)
    {
        model = r_u32(actor + 180);
        if (model - 6 < 3 || model - 15 < 3)
            (void)sub_800670FC(actor, resources, model < 9 ? 8 : 16, (int32_t)(r_u32(0x8005B1F8) + 1));
    }
    w_u8(0x8005AD30, 1);
    result = (uint32_t)(r_s32(r_u32(0x8005B38C) + 324) >> 12);
    w_u32(0x8005B244, result);
    return result;
}

uint32_t sub_80065974(int32_t slot)
{
    int32_t selected;
    int32_t count;
    uint32_t actor;
    FUNCTION_MARKER(0x80065974, "RASHCDI.BIN");
    if (r_s8(0x8005B2B9) >= r_s8(0x8005B2B8))
        return 0;
    count = r_s32(0x8005B1F8);
    selected = slot < 0 ? count : slot;
    if (selected < 0 || selected >= r_s8(0x8005B2B8) || !r_u32(0x8005B3A0))
        abort();
    actor = r_u32(0x8005B3A0) + 1096 * (uint32_t)selected;
    w_u16(actor + 172, selected);
    if (slot < count)
    {
        w_u32(0x8005B1F8, (uint32_t)count + 1);
        w_u32(0x8005AD38, selected);
    }
    w_u8(0x8005B2B9, r_u8(0x8005B2B9) + 1);
    return actor;
}

uint32_t sub_80065A04(int32_t slot)
{
    uint32_t component;
    FUNCTION_MARKER(0x80065A04, "RASHCDI.BIN");
    if (r_s8(0x8005B2BB) >= r_s8(0x8005B2BA))
        return 0;
    if (slot < 0)
        slot = r_s32(0x8005B218);
    if (slot < 0 || slot >= r_s8(0x8005B2BA) || !r_u32(0x8005B3A4))
        abort();
    w_u32(0x8005AD3C, slot);
    component = r_u32(0x8005B3A4) + 628 * (uint32_t)slot;
    w_u16(component + 172, (uint32_t)slot + 32);
    w_u32(0x8005B218, r_u32(0x8005B218) + 1);
    w_u8(0x8005B2BB, r_u8(0x8005B2BB) + 1);
    return component;
}

uint32_t sub_8005DD9C(uint32_t player)
{
    FUNCTION_MARKER(0x8005DD9C, "RASHCDI.BIN");
    return r_u8(0x8006B898 + 4 * player + 3);
}

uint32_t sub_80064C0C(const uint8_t *resources, uint32_t variant, uint32_t actor)
{
    const uint8_t *record;
    uint32_t state;
    uint32_t mode;
    uint32_t descriptor;
    uint32_t index;
    uint32_t result;
    uint32_t reset = 0;
    FUNCTION_MARKER(0x80064C0C, "RASHCDI.BIN");
    state = r_u32(0x8005B2F8);
    mode = r_u8(state + 4);
    if (variant - 2 < 16)
        reset = mode == 34 ? (r_u8(state + 5) & 1) != 0 : 1;
    else if (variant - 2 < 18)
        reset = 1;
    else if ((!(mode & 32) && mode != 1 && mode != 8) || (r_u8(0x800D80DD) & 8))
        reset = 1;
    else if (!r_u32(state + 60) && (r_u8(state + 5) & 1))
        reset = 1;
    descriptor = r_u32(actor + 1084);
    w_u8(descriptor + 38, 0);
    w_u32(descriptor + 8, 0x10000);
    w_u32(r_u32(actor + 852) + 316, 0x4B0000);
    w_u8(descriptor + 13, 128);
    w_u8(descriptor + 12, 128);
    w_u8(descriptor + 14, 128);
    w_u8(descriptor + 15, 128);
    w_u8(descriptor + 2, 255);
    w_u8(descriptor + 61, 112);
    w_u8(descriptor + 3, 4);
    w_u8(descriptor + 69, 123);
    (void)sub_8001E100(descriptor + 52, 32, 8);
    w_u8(descriptor + 60, 32);
    w_u8(descriptor + 36, 255);
    w_u8(descriptor + 70, 31);
    w_u8(descriptor + 71, 31);
    if (reset)
    {
        w_u8(descriptor + 47, 0);
        w_u8(descriptor + 46, 9);
        w_u16(descriptor + 44, 1536);
        w_u32(descriptor + 48, 0);
    }
    (void)sub_8001E100(descriptor + 16, 0, 20);
    if (resources)
    {
        if (variant >= 32)
            abort();
        record = resources + 64 * variant;
        if ((int32_t)(int8_t)record[0] == (int32_t)variant)
        {
            w_u8(descriptor + 38, record[4]);
            w_u8(descriptor + 1, record[5]);
            w_u8(descriptor + 3, record[7]);
            for (index = 0; index < 4; ++index)
                w_u8(descriptor + 8 + index, record[8 + index]);
            for (index = 0; index < 4; ++index)
                w_u8(r_u32(actor + 852) + 316 + index, record[12 + index]);
            for (index = 0; index < 4; ++index)
                w_u8(descriptor + 12 + index, record[16 + index]);
            w_u8(descriptor + 2, record[6]);
            for (index = 0; index < 20; ++index)
                w_u8(descriptor + 16 + index, record[20 + index]);
            if (reset)
                for (index = 0; index < 2; ++index)
                    w_u8(descriptor + 44 + index, record[40 + index]);
            w_u8(descriptor + 36, record[42]);
            w_u8(descriptor + 69, record[43]);
            for (index = 0; index < 8; ++index)
                w_u8(descriptor + 52 + index, record[44 + index]);
            w_u8(descriptor + 61, record[53]);
            w_u32(actor + 36, (r_u32(actor + 36) & 0xFFFC0FFF) | ((record[54] & 63) << 12));
            result = r_u32(actor + 852);
            w_u32(result + 36, (r_u32(result + 36) & 0xFFFC0FFF) | ((record[55] & 63) << 12));
            for (index = 0; index < 4; ++index)
                w_u8(actor + 76 + index, record[56 + index]);
            for (index = 0; index < 4; ++index)
                w_u8(r_u32(actor + 852) + 76 + index, record[60 + index]);
        }
    }
    w_u32(descriptor + 4, 0);
    w_u32(descriptor + 40, 0);
    w_u8(descriptor + 39, 0);
    w_u8(descriptor, 0);
    w_u8(descriptor + 37, r_u8(descriptor + 36));
    w_u8(descriptor + 15, r_u8(descriptor + 13));
    w_u8(descriptor + 14, r_u8(descriptor + 13));
    w_u8(descriptor + 60, r_u8(descriptor + 52));
    result = r_u16(actor + 172) < r_u32(r_u32(0x8005B2F8) + 48);
    if (!result)
    {
        result = r_u8(descriptor + 1) & 15;
        if (result == 2)
        {
            result = sub_8001FC58() & 7;
            w_u8(descriptor + 38, result);
        }
    }
    return result;
}

uint32_t sub_800650A0(uint32_t actor)
{
    uint32_t state;
    uint32_t descriptor;
    uint32_t kind;
    uint32_t peer;
    uint32_t index;
    uint32_t offset;
    uint32_t result;
    int32_t relation;
    int32_t sum;
    FUNCTION_MARKER(0x800650A0, "RASHCDI.BIN");
    state = r_u32(0x8005B2F8);
    descriptor = r_u32(actor + 1084);
    if (r_u16(actor + 172) < r_u32(state + 48))
    {
        result = r_u8(state + 4) & 1;
        if (result)
            return result;
        index = 8 * (r_u8(descriptor + 1) & 15) + 2;
        if (r_u8(0x800D38C8 + index) == 31)
            return 0x800D0000;
        result = r_u8(0x800D5758 + 72 * index + 36);
        w_u8(descriptor + 36, result);
        w_u8(descriptor + 37, result);
    }
    else
    {
        relation = r_u8(state + 4) & 4 ? 1 : 2 * (r_u8(state + 4) & 1);
        index = 0;
        do
        {
            descriptor = r_u32(actor + 1084);
            kind = r_u8(descriptor + 1) & 15;
            peer = r_u8(0x800D5758 + 72 * index + 1) & 15;
            offset = 0x80052EE4 + 36 * kind;
            if (kind == peer && r_u8(r_u32(0x8005B2F8) + 4) == 44)
                w_u8(descriptor + 16 + index, 0);
            else if ((kind == peer && relation == 1) || (kind != peer && peer != 2 && r_u8(r_u32(0x8005B2F8) + 4) == 44))
                w_u8(descriptor + 16 + index, 15);
            else
            {
                offset += kind == peer ? 7 : (peer == 2 ? 19 : 6);
                sum = r_s8(descriptor + 16 + index) + r_s8(offset);
                w_u8(descriptor + 16 + index, sum < -15 ? -15 : (sum > 15 ? 15 : sum));
            }
            ++index;
        } while (index < r_u32(r_u32(0x8005B2F8) + 48));
        if (relation < 2)
            for (index = 2; index < 18; ++index)
            {
                descriptor = r_u32(actor + 1084);
                if (r_u8(r_u32(0x8005B2F8) + 4) == 44)
                    w_u8(descriptor + 16 + index, (uint32_t)-15);
                else
                {
                    peer = r_u8(0x800D5758 + 72 * index + 1) & 15;
                    offset = 0x80052EE4 + 36 * kind + (uint32_t)relation + (kind == peer ? 22 : 20);
                    sum = r_s8(descriptor + 16 + index) + r_s8(offset);
                    w_u8(descriptor + 16 + index, sum < -15 ? -15 : (sum > 15 ? 15 : sum));
                }
            }
    }
    result = r_u32(actor + 1084) + r_u8(0x800D38B0 + r_u16(actor + 172));
    w_u8(result + 16, 128);
    return result;
}

uint32_t sub_800670FC(uint32_t owner, const uint8_t *resources, uint32_t variant, int32_t slot)
{
    uint32_t actor;
    uint32_t component;
    uint32_t descriptor;
    uint32_t index;
    uint32_t candidate;
    uint32_t maximum;
    uint32_t model;
    uint32_t result;
    uint32_t value;
    uint32_t origin[3];
    uint32_t output[3];
    uint16_t direction[3];
    FUNCTION_MARKER(0x800670FC, "RASHCDI.BIN");
    actor = sub_80065974(slot);
    if (!actor)
        abort();
    w_u32(owner + 856, actor);
    w_u32(actor + 856, owner);
    w_u32(actor + 1088, 0);
    w_u32(actor + 556, r_u32(owner + 556));
    w_u32(actor + 832, 0);
    w_u32(actor + 560, r_u32(owner + 560));
    (void)sub_8002090C(actor);
    component = sub_80065A04(slot);
    if (!component)
        abort();
    descriptor = 0x800D5758 + 72 * variant;
    w_u32(actor + 852, component);
    w_u32(component + 596, actor);
    w_u32(actor + 1084, descriptor);
    w_u8(0x800D38B0 + r_u16(actor + 172), variant);
    w_u8(0x800D38C8 + variant, r_u8(actor + 172));
    (void)sub_80064C0C(resources, (r_u8(r_u32(owner + 1084) + 1) & 15) + 20, actor);
    w_u32(actor + 304, 0x7333);
    w_u32(actor + 308, 0x8000);
    w_u32(actor + 312, 0x1CCCC);
    w_u16(actor + 956, 1);
    w_u16(actor + 958, 224);
    w_u8(actor + 946, 1);
    w_u32(actor + 316, r_u32(owner + 316));
    if (r_u8(r_u32(0x8005B2F8) + 4) == 44)
        w_u32(component + 76, 0x10000);
    result = sub_8005DD9C(owner == r_u32(0x8005B21C));
    if (result != 0xFFFFFFFF)
        w_u32(component + 36, (r_u32(component + 36) & 0xFFFC0FFF) | (((result + 2) & 63) << 12));
    model = r_u8(r_u32(owner + 1084) + 1) & 15;
    w_u32(component + 180, sub_8002FAD4(component, 1, 9 * model + r_u32(r_u32(0x8005B2F8) + 60), 1));
    (void)sub_80012FC8(component, 0);
    w_u8(component + 569, 41);
    w_u32(component + 604, 1);
    w_u8(component + 571, 255);
    w_u32(component + 556, 0);
    w_u32(component + 540, 0);
    w_u8(component + 570, 0);
    w_u8(component + 572, 0);
    w_u32(component + 36, r_u32(component + 36) | 0x40000);
    w_u16(component + 544, 224);
    w_u16(component + 320, r_u16(owner + 320));
    if (r_u8(r_u32(0x8005B2F8) + r_u16(owner + 172) + 10))
    {
        w_u8(component + 572, r_u8(component + 572) | 32);
        descriptor = r_u32(owner + 852);
        w_u8(descriptor + 572, r_u8(descriptor + 572) | 16);
        (void)sub_80012838(owner, component, 2, 1);
    }
    if (r_u8(r_u32(0x8005B2F8) + r_u16(owner + 172) + 10) == 2)
        w_u8(component + 572, r_u8(component + 572) | 64);
    for (index = 0; index < 32; ++index)
        w_u8(actor + 328 + index, r_u8(owner + 328 + index));
    for (index = 0; index < 12; ++index)
        w_u8(actor + 360 + index, r_u8(owner + 360 + index));
    for (index = 0; index < 56; ++index)
        w_u8(actor + 372 + index, r_u8(owner + 372 + index));
    maximum = r_u32(0x8005B1F8);
    w_u32(actor + 324, r_u32(owner + 324));
    w_u16(actor + 320, r_u16(owner + 320));
    if ((int32_t)maximum >= 3)
    {
        index = 2;
        do
        {
            candidate = r_u32(0x8005B3A0) + 1096 * (index & 0xFFFF);
            ++index;
            if ((r_u32(candidate + 180) >= 9) != (r_u32(owner + 180) < 9))
            {
                index = r_u32(0x8005B1F8);
                w_u32(owner + 308, r_u32(candidate + 308));
                w_u32(owner + 304, r_u32(candidate + 304));
                w_u32(owner + 312, r_u32(candidate + 312));
            }
        } while ((int32_t)index < (int32_t)maximum);
    }
    else
    {
        w_u32(owner + 308, 0xC000);
        w_u32(owner + 304, 0x4000);
        w_u32(owner + 312, 0x10000);
    }
    for (index = 0; index < 3; ++index)
    {
        origin[index] = r_u32(owner + 184 + 4 * index);
        direction[index] = r_u16(owner + 432 + 2 * index);
    }
    (void)sub_8002EAD8(origin, direction, r_u32(owner + 304) + r_u32(actor + 304), output);
    for (index = 0; index < 3; ++index)
        w_u32(actor + 184 + 4 * index, output[index]);
    for (index = 0; index < 3; ++index)
        w_u32(actor + 504 + 4 * index, r_u32(actor + 184 + 4 * index));
    for (index = 0; index < 3; ++index)
        w_u32(actor + 468 + 4 * index, r_u32(actor + 184 + 4 * index));
    result = r_u32(owner + 308);
    value = r_u32(actor + 344);
    w_u32(actor + 344, (int32_t)value > 0 ? value + result : value - result);
    return result;
}

uint32_t sub_80065558(const char *path, uint8_t data[448])
{
    uint32_t handle, bytes, result;
    FUNCTION_MARKER(0x80065558, "RASHCDI.BIN");
    handle = sub_8001458C(path, r_u32(0x8005AD5C));
    if ((int32_t)handle < 0)
    {
        RRJ_DEBUG_FPRINTF(stderr, "Missing race resource: %s\n", path);
        abort();
    }
    bytes = sub_80014780(handle, data, 448);
    result = sub_8001460C(handle);
    if (bytes != 448)
    {
        RRJ_DEBUG_FPRINTF(stderr, "Truncated race resource: %s\n", path);
        abort();
    }
    return result;
}

uint32_t sub_80065648(void)
{
    uint32_t owner, shift, divisor;
    int32_t count, quotient, limit;
    FUNCTION_MARKER(0x80065648, "RASHCDI.BIN");
    owner = r_u32(0x8005B2F8);
    shift = ((uint32_t)r_s16(owner + 58) << 2) & 31;
    count = r_s32(owner + 68);
    divisor = (0x8532 >> shift) & 15;
    // Preserve the MIPS DIV result when an out-of-range selection yields zero
    quotient = divisor ? count / (int32_t)divisor : (count >= 0 ? -1 : 1);
    limit = (0x123 >> shift) & 15;
    if (quotient < limit)
        limit = quotient;
    return ((0x148 >> shift) & 15) - (uint32_t)limit;
}

uint32_t sub_800656A0(uint32_t actor, uint32_t piece, uint32_t direction)
{
    uint16_t rotation[9], matrix[9];
    uint16_t axis[3];
    uint32_t i;
    int32_t angle_scale = 0xA0000;
    FUNCTION_MARKER(0x800656A0, "RASHCDI.BIN");
    w_u16(actor + 444, r_u16(piece + 2));
    w_u16(actor + 446, r_u16(piece + 4));
    w_u16(actor + 444, (uint16_t)(0 - r_u16(actor + 444)));
    w_u16(actor + 448, r_u16(piece + 6));
    w_u16(actor + 448, (uint16_t)(0 - r_u16(actor + 448)));
    w_u16(actor + 446, (uint16_t)(0 - r_u16(actor + 446)));
    for (i = 0; i < 3; ++i)
        w_u16(actor + 432 + 2 * i, r_u16(piece + 14 + 2 * i));
    w_u16(actor + 438, r_u16(piece + 8));
    w_u16(actor + 440, r_u16(piece + 10));
    w_u16(actor + 438, (uint16_t)(0 - r_u16(actor + 438)));
    w_u16(actor + 442, r_u16(piece + 12));
    w_u16(actor + 440, (uint16_t)(0 - r_u16(actor + 440)));
    w_u16(actor + 442, (uint16_t)(0 - r_u16(actor + 442)));
    if (r_s32(actor + 344) < 0)
    {
        for (i = 0; i < 3; ++i)
            w_u16(actor + 444 + 2 * i, (uint16_t)(0 - r_u16(actor + 444 + 2 * i)));
        for (i = 0; i < 3; ++i)
            w_u16(actor + 432 + 2 * i, (uint16_t)(0 - r_u16(actor + 432 + 2 * i)));
    }
    if (((int32_t)direction < 0 && r_s32(actor + 344) < 0) || ((int32_t)direction >= 0 && r_s32(actor + 344) > 0))
        angle_scale = -0xA0000;
    for (i = 0; i < 3; ++i)
        axis[i] = r_u16(piece + 8 + 2 * i);
    (void)sub_8003FB34(axis, (uint32_t)sub_8001FC90(angle_scale, 11), rotation);
    for (i = 0; i < 9; ++i)
        matrix[i] = r_u16(actor + 432 + 2 * i);
    (void)sub_8003FA40(matrix, rotation, matrix);
    for (i = 0; i < 9; ++i)
        w_u16(actor + 432 + 2 * i, matrix[i]);
    return actor + 434;
}

uint32_t sub_8006581C(uint32_t actor)
{
    uint32_t random, piece, lateral, resource, owner, result = 0;
    uint32_t position[3], origin[3];
    uint16_t axis[3];
    uint32_t i;
    FUNCTION_MARKER(0x8006581C, "RASHCDI.BIN");
    random = sub_8001FC58();
    piece = r_u32(actor + 340);
    if ((random & 0x1000) || ((random >> 21) & 1))
    {
        lateral = r_u32(actor + 344);
        w_u32(0x8005B2B0, 0);
    }
    else
    {
        resource = r_u32(actor + 372);
        lateral = (uint32_t)(r_s32(resource + 4) >> 1);
        if (r_s32(0x800CF57C) < 0)
            lateral = r_u32(resource + 80) - lateral;
        else
            lateral += r_u32(resource + 208);
        w_u32(0x8005B2B0, 1);
    }
    for (i = 0; i < 3; ++i)
    {
        origin[i] = r_u32(piece + 20 + 4 * i);
        axis[i] = r_u16(piece + 14 + 2 * i);
    }
    (void)sub_8002EAD8(origin, axis, r_u32(actor + 348), position);
    for (i = 0; i < 3; ++i)
        w_u32(actor + 184 + 4 * i, position[i]);
    for (i = 0; i < 3; ++i)
        axis[i] = r_u16(piece + 2 + 2 * i);
    (void)sub_8002EAD8(position, axis, lateral, position);
    for (i = 0; i < 3; ++i)
        w_u32(actor + 184 + 4 * i, position[i]);
    w_u32(actor + 344, lateral);
    for (i = 0; i < 3; ++i)
        w_u32(actor + 504 + 4 * i, position[i]);
    if (!r_u32(0x8005B2B0))
    {
        result = 1;
        (void)sub_800656A0(actor, piece, r_u32(0x800CF57C));
    }
    owner = r_u32(0x8005B2F8);
    w_u32(0x8005AD44, r_u8(owner + 7));
    w_u32(0x800D8708, r_u32(0x800D8708) | (1u << (r_u8(owner + 6) & 31)));
    return result;
}
