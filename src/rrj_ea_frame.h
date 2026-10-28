#ifndef RRJ_EA_FRAME_H
#define RRJ_EA_FRAME_H

#include "rrj_ea_vlc.h"
#include "psx_press.h"

typedef struct RRJEaFrameDecoder
{
    RRJEaVlc vlc;
    uint32_t coefficients[65536];
    uint8_t encoded[65544];
    DECDCTENV environment;
} RRJEaFrameDecoder;

int rrj_ea_frame_init(RRJEaFrameDecoder *decoder);
int rrj_ea_frame_decode(RRJEaFrameDecoder *decoder, const uint8_t *body, uint32_t bytes, uint8_t *rgb, uint32_t capacity, uint32_t *width, uint32_t *height);

#endif
