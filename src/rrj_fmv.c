#include "rrj_fmv.h"

#if !defined(AP_WEB)
#include "rrj_ea_frame.h"
#include "psx_spu.h"
#include <stdlib.h>
#include <string.h>

typedef struct RRJEaMovie
{
    FILE *video, *audio;
    RRJEaFrameDecoder decoder;
    uint8 body[65536], sound[65536];
    uint32 sound_bytes, sound_block, sound_blocks, sound_sample;
    sint16 history1[2], history2[2], samples[2][28];
    uint32 ordinal, has_audio, audio_done, audio_failed, paused;
    uint64 start, pause_start, consumed;
} RRJEaMovie;

static RRJEaMovie *movie;

static uint32 big32(const uint8 *p)
{
    return (uint32)p[0] << 24 | (uint32)p[1] << 16 | (uint32)p[2] << 8 | p[3];
}

static int chunk(FILE *file, uint8 *body, uint32 *tag, uint32 *bytes, int audio)
{
    uint8 header[8];
    for (;;)
    {
        size_t count = xport_fread(header, 1, 8, file);
        uint32 length;
        if (!count)
            return 0;
        if (count != 8)
            return -1;
        *tag = big32(header);
        length = big32(header + 4);
        if (length < 8 || length > 65536)
            return -1;
        *bytes = length - 8;
        if (audio ? (*tag == 0x61753030 || *tag == 0x61753031) : (*tag == 0x4d444543 || *tag == 0x564c4330))
            return xport_fread(body, 1, *bytes, file) == *bytes ? 1 : -1;
        if (fseek(file, *bytes, SEEK_CUR))
            return -1;
    }
}

static void close_movie(void *context)
{
    if (movie)
    {
        if (movie->video)
            fclose(movie->video);
        if (movie->audio)
            fclose(movie->audio);
        free(movie);
        movie = NULL;
    }
}

static sint32 open_movie(void *context, const char *path)
{
    uint32 tag, bytes = 0;
    int found;
    movie = (RRJEaMovie *)calloc(1, sizeof(*movie));
    if (!movie)
        return -1;
    movie->video = xport_fopen(path, "rb");
    movie->audio = xport_fopen(path, "rb");
    if (!movie->video || !movie->audio || !rrj_ea_frame_init(&movie->decoder))
    {
        close_movie(NULL);
        return -1;
    }
    found = chunk(movie->audio, movie->sound, &tag, &bytes, 1);
    if (found < 0)
    {
        close_movie(NULL);
        return -1;
    }
    movie->has_audio = found > 0;
    movie->audio_done = !found;
    movie->sound_bytes = bytes;
    movie->sound_blocks = found && bytes >= 8 ? (bytes - 8) / 30 : 0;
    movie->sound_sample = 56;
    movie->start = xport_timer_get();
    return 1;
}

static uint32 audio_movie(void *context, sint16 *out, uint32 frames)
{
    uint32 produced = 0, channel, index, tag, bytes;
    if (!movie || movie->paused || !movie->has_audio)
        return 0;
    while (produced < frames)
    {
        if (movie->sound_sample >= 56)
        {
            uint8 block[16];
            if (movie->sound_block >= movie->sound_blocks)
            {
                int found = chunk(movie->audio, movie->sound, &tag, &bytes, 1);
                if (found <= 0)
                {
                    movie->audio_done = 1;
                    movie->audio_failed = found < 0;
                    break;
                }
                movie->sound_bytes = bytes;
                movie->sound_blocks = bytes >= 8 ? (bytes - 8) / 30 : 0;
                movie->sound_block = 0;
                if (!movie->sound_blocks)
                    continue;
            }
            for (channel = 0; channel < 2; ++channel)
            {
                const uint8 *packed = movie->sound + 8 + 15 * (channel * movie->sound_blocks + movie->sound_block);
                block[0] = packed[0];
                block[1] = 0;
                memcpy(block + 2, packed + 1, 14);
                spu_decode_adpcm_block(block, movie->history1 + channel, movie->history2 + channel, movie->samples[channel]);
            }
            movie->sound_block++;
            movie->sound_sample = 0;
        }
        index = movie->sound_sample++ / 2;
        out[produced * 2] = movie->samples[0][index];
        out[produced * 2 + 1] = movie->samples[1][index];
        produced++;
    }
    movie->consumed += produced;
    return produced;
}

static sint32 next_movie(void *context, uint8 *rgb, uint32 capacity, XportFmvFrame *frame)
{
    uint32 tag, bytes, done;
    uint64 elapsed;
    int result;
    if (!movie)
        return -1;
    xport_audio_lock();
    elapsed = movie->has_audio ? movie->consumed * 1000000 / 44100 : xport_timer_get() - movie->start;
    done = movie->audio_done;
    result = movie->audio_failed ? -1 : movie->paused != 0;
    xport_audio_unlock();
    if (result < 0)
        return -1;
    if (result || (!done && elapsed * 15 < (uint64)movie->ordinal * 1000000) ||
        (!movie->has_audio && elapsed * 15 < (uint64)movie->ordinal * 1000000))
        return 0;
    for (;;)
    {
        result = chunk(movie->video, movie->body, &tag, &bytes, 0);
        if (result <= 0)
            return result < 0 ? -1 : done ? 2 : 0;
        if (tag == 0x564c4330)
        {
            if (!rrj_ea_vlc_build(&movie->decoder.vlc, movie->body, bytes))
                return -1;
            continue;
        }
        if (!rrj_ea_frame_decode(&movie->decoder, movie->body, bytes, rgb, capacity, &frame->width, &frame->height))
            return -1;
        frame->number = movie->has_audio ? big32(movie->body + 4) : movie->ordinal;
        movie->ordinal++;
        return 1;
    }
}

static void pause_movie(void *context, sint32 paused)
{
    if (!movie)
        return;
    xport_audio_lock();
    if (paused && !movie->paused)
        movie->pause_start = xport_timer_get();
    else if (!paused && movie->paused)
        movie->start += xport_timer_get() - movie->pause_start;
    movie->paused = paused != 0;
    xport_audio_unlock();
}

static sint32 has_audio_movie(void *context)
{
    return movie && movie->has_audio;
}

int rrj_fmv_install(void)
{
    static const XportFmvBackend backend = {open_movie, next_movie, pause_movie, close_movie, audio_movie, has_audio_movie};
    return xport_fmv_set_backend(&backend, NULL);
}
#else
int rrj_fmv_install(void)
{
    return xport_fmv_set_backend(NULL, NULL);
}
#endif
