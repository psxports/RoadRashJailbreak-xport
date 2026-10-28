#include "rrj_ea_vlc.h"
#include <string.h>

#include "rrj_ea_vlc.inc"

static uint16_t ea_read16(const uint8_t *bytes)
{
    return (uint16_t)(bytes[0] | (uint16_t)bytes[1] << 8);
}

/* Host adaptation of SLUS_010.53:800202B8 */
int rrj_ea_vlc_build(RRJEaVlc *table, const uint8_t *values, uint32_t bytes)
{
    uint32_t row, index, bits, prefix, count;
    if (!table || (values && bytes < 448))
        return 0;
    if (!values)
        values = ea_vlc_default_values;
    memset(table, 0, sizeof(*table));
    for (row = 0; row < 224; ++row)
    {
        const uint8_t *code = ea_vlc_codes + 4 * row;
        uint16_t value = ea_read16(values + 2 * row);
        bits = code[0];
        prefix = ea_read16(code + 2) >> (row < 96 ? 3 : 7);
        if (!bits || bits > (row < 96 ? 13u : 9u))
            return 0;
        count = 1 << ((row < 96 ? 13 : 9) - bits);
        for (index = 0; index < count; ++index)
        {
            uint32_t slot = prefix | index;
            if (row < 96)
            {
                if (slot >= 8192) return 0;
                table->long_lengths[slot] = (uint8_t)bits;
                table->long_values[slot] = value;
            }
            else
            {
                if (slot >= 512) return 0;
                table->short_lengths[slot] = (uint8_t)bits;
                table->short_values[slot] = value;
            }
        }
    }
    return 1;
}

typedef struct EaBits
{
    const uint8_t *cursor, *end;
    uint32_t bits, count;
} EaBits;

static int ea_shift(EaBits *stream, uint32_t length)
{
    stream->bits <<= length;
    stream->count += length;
    if (stream->count & 16)
    {
        stream->count &= 15;
        if (stream->end - stream->cursor < 2)
            return 0;
        stream->bits |= (uint32_t)ea_read16(stream->cursor) << stream->count;
        stream->cursor += 2;
    }
    stream->count &= 15;
    return 1;
}

/* Host adaptation of SLUS_010.53:80020400 with bounded input and output */
int rrj_ea_vlc_decode(const RRJEaVlc *table, const uint8_t *input, uint32_t bytes, uint32_t *output, uint32_t capacity)
{
    EaBits stream;
    uint32_t prefix, used = 2, words;
    uint16_t *decoded = (uint16_t *)output;
    if (!table || !input || !output || bytes < 12 || !capacity)
        return 0;
    words = (uint32_t)ea_read16(input) + 1;
    if (words > capacity || words < 2)
        return 0;
    output[0] = (uint32_t)ea_read16(input) | (uint32_t)ea_read16(input + 2) << 16;
    prefix = (uint32_t)ea_read16(input + 4) << 10;
    stream.bits = (uint32_t)ea_read16(input + 8) << 16 | ea_read16(input + 10);
    stream.count = 0;
    stream.cursor = input + 12;
    stream.end = input + bytes;
    for (;;)
    {
        uint32_t code = stream.bits >> 22;
        if (code == 511)
            break;
        if (used >= words * 2 || !ea_shift(&stream, 10))
            return 0;
        decoded[used++] = (uint16_t)(code | prefix);
        for (;;)
        {
            uint32_t index = stream.bits >> 19, length;
            uint16_t value;
            if (index >= 32)
            {
                length = table->long_lengths[index];
                value = table->long_values[index];
            }
            else
            {
                if (!ea_shift(&stream, 8)) return 0;
                index = stream.bits >> 23;
                length = table->short_lengths[index];
                value = table->short_values[index];
            }
            if (!length || !ea_shift(&stream, length) || used >= words * 2)
                return 0;
            if (value == 0x7C1F)
            {
                decoded[used++] = (uint16_t)(stream.bits >> 16);
                if (stream.end - stream.cursor < 2) return 0;
                stream.bits = stream.bits << 16 | (uint32_t)ea_read16(stream.cursor) << stream.count;
                stream.cursor += 2;
            }
            else
            {
                decoded[used++] = value;
                if (value == 0xFE00) break;
            }
        }
    }
    while (used < words * 2)
        decoded[used++] = 0xFE00;
    return (int)words;
}
