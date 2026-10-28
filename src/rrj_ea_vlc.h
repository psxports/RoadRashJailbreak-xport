#ifndef RRJ_EA_VLC_H
#define RRJ_EA_VLC_H

#include <stdint.h>

typedef struct RRJEaVlc
{
    uint8_t long_lengths[8192], short_lengths[512];
    uint16_t long_values[8192], short_values[512];
} RRJEaVlc;

int rrj_ea_vlc_build(RRJEaVlc *table, const uint8_t *values, uint32_t bytes);
int rrj_ea_vlc_decode(const RRJEaVlc *table, const uint8_t *input, uint32_t bytes, uint32_t *output, uint32_t capacity);

#endif
