#include "rrj_ea_frame.h"
#include "psx_mdec.h"
#include <string.h>

int rrj_ea_frame_init(RRJEaFrameDecoder *decoder)
{
    if (!decoder)
        return 0;
    decoder->environment = mdec_default_environment;
    return rrj_ea_vlc_build(&decoder->vlc, NULL, 0);
}

int rrj_ea_frame_decode(RRJEaFrameDecoder *decoder, const uint8_t *body, uint32_t bytes, uint8_t *rgb, uint32_t capacity, uint32_t *width, uint32_t *height)
{
    uint32_t w, h, words, available, consumed, x, y, block, row, column;
    const uint16_t *input;
    sint16 blocks[384];
    uint32 pixels[256];
    if (!decoder || !body || !rgb || !width || !height || bytes < 20 || bytes - 8 > sizeof(decoder->encoded) - 8)
        return 0;
    w = (uint32_t)body[0] << 8 | body[1];
    h = (uint32_t)body[2] << 8 | body[3];
    if (!w || w > 640 || (w & 15) || !h || h > 256 || capacity < w * h * 3)
        return 0;
    memcpy(decoder->encoded, body + 8, bytes - 8);
    memset(decoder->encoded + bytes - 8, 0, 8);
    words = rrj_ea_vlc_decode(&decoder->vlc, decoder->encoded, bytes, decoder->coefficients, 65536);
    if (!words)
        return 0;
    input = (const uint16_t *)(decoder->coefficients + 1);
    available = (words - 1) * 2;
    for (x = 0; x < w; x += 16)
        for (y = 0; y < h; y += 16)
        {
            for (block = 0; block < 6; ++block)
            {
                MdecRleBlock rle;
                mdec_rle_begin(&rle);
                if (mdec_rle_decode(&rle, input, available, &consumed, block < 2 ? decoder->environment.iq_c : decoder->environment.iq_y) != 1)
                    return 0;
                input += consumed;
                available -= consumed;
                memcpy(blocks + block * 64, rle.coefficients, sizeof(rle.coefficients));
                if (mdec_idct(blocks + block * 64, decoder->environment.dct))
                    return 0;
            }
            if (mdec_color(pixels, blocks, 0))
                return 0;
            for (row = 0; row < 16 && y + row < h; ++row)
                for (column = 0; column < 16; ++column)
                {
                    uint32_t pixel = pixels[row * 16 + column];
                    uint8_t *out = rgb + ((y + row) * w + x + column) * 3;
                    out[0] = (uint8_t)pixel;
                    out[1] = (uint8_t)(pixel >> 8);
                    out[2] = (uint8_t)(pixel >> 16);
                }
        }
    *width = w;
    *height = h;
    return 1;
}
