#include "race_leaf.h"
#include "spu.h"
#include "psx.h"
/* Original MIPS voice release and stream/music stop, including memory reloads. */
#include "music_stop.h"
#include "audio.h"
#include <stdlib.h>

uint32_t sub_8001FB58(uint32_t voice)
{
    FUNCTION_MARKER(0x8001FB58u, "SLUS_010.53");
    uint32_t slot = r_u32(voice + 28), i, tail, next, result;
    w_u32(voice + 4, 0);
    w_u32(voice + 16, 0);
    for (i = 0; i < 22; ++i)
        if (r_u32(0x800D68C8 + 4 * i) == slot)
        {
            w_u32(0x800D68C8 + 4 * i, 0xffffffff);
            tail = r_u32(0x800D69E8);
            next = tail + 1;
            w_u32(0x800D6980 + 4 * tail, slot);
            result = ((int32_t)(uint32_t)(next)) < 25;
            w_u32(0x800D69E8, next);
            w_u32(0x800D69E8, result ? next : 0);
            return result;
        }
    return 0;
}

uint32_t sub_8001F7EC(uint32_t handle)
{
    FUNCTION_MARKER(0x8001F7ECu, "SLUS_010.53");
    uint32_t voice = r_u32(0x800D687C) + 44 * (handle >> 27);
    uint32_t generation = r_u32(voice + 4) & 0x7ffffff;
    if (generation != (handle & 0x7ffffff))
        return generation;
    (void)sub_8001EB44(1u << (r_u32(voice + 28) & 31));
    (void)sub_8001FB58(voice);
    return SpuSetReverbVoice(1, 1u << (r_u32(voice + 28) & 31));
}

uint32_t sub_8001F5D4(uint32_t handle, uint32_t release)
{
    FUNCTION_MARKER(0x8001F5D4u, "SLUS_010.53");
    uint32_t i, stream = 0x800D6870, mask = 1u << (handle >> 27);
    for (i = 0; i < 3; ++i, stream += 20)
        if (r_u32(stream + 44) == handle)
        {
            if (release)
            {
                RRJVoiceSetup attr = {0};
                attr.voices = mask;
                attr.mask = 0x40000;
                attr.adsr2 = 0x1fc0;
                (void)rrj_spu_setup(r_u32(stream + 44) >> 27, &attr);
            }
            (void)sub_8001F7EC(handle);
            (void)SpuSetReverbVoice(1, 1u << (r_u32(stream + 44) >> 27));
            w_u32(stream + 28, 0);
            w_u32(stream + 44, 0);
        }
    return 0;
}

uint32_t sub_8007F158(uint32_t reset)
{
    FUNCTION_MARKER(0x8007F158u, "RASHCDF.BIN");
    uint32_t restore, release;
    if (r_u32(0x800A0838))
        w_u32(0x800A0838, 0);
    if (r_u32(0x800A0834))
    {
        release = r_u16(0x8009C5D0) == 47;
        (void)sub_8001F5D4(r_u32(0x800A0844), release);
        (void)sub_8001F5D4(r_u32(0x800A0848), release);
    }
    restore = r_u32(0x800A0840);
    w_u32(0x800A0834, 0);
    if (restore)
    {
        w_u32(0x800A0840, 0);
        w_u32(0x800D6C14, r_u32(0x800D6C34));
    }
    if (reset)
        w_u32(0x800A0830, 0xffffffff);
    return 0xffffffff;
}

uint32_t sub_8007E9DC(void)
{
    uint32_t buffer, handle;
    FUNCTION_MARKER(0x8007E9DCu, "RASHCDF.BIN");
    buffer = r_u32(0x800A0870u);
    if (buffer)
    {
        sub_800144B8(buffer);
        sub_800144B8(r_u32(0x800A0874u));
        w_u32(0x800A0870u, 0u);
        w_u32(0x800A0874u, 0u);
    }
    handle = r_u32(0x800A0854u);
    if (((int32_t)(uint32_t)(handle)) >= 0)
    {
        sub_8001EE28(handle);
        w_u32(0x800A0854u, UINT32_MAX);
    }
    buffer = r_u32(0x800A0860u);
    if (buffer)
    {
        sub_800144B8(buffer);
        w_u32(0x800A0860u, 0u);
    }
    handle = r_u32(0x800A084Cu);
    if (((int32_t)(uint32_t)(handle)) >= 0)
    {
        sub_8001460C(handle);
        w_u32(0x800A084Cu, UINT32_MAX);
    }
    handle = r_u32(0x800A0868u);
    if (((int32_t)(uint32_t)(handle)) >= 0)
    {
        sub_8001460C(handle);
        w_u32(0x800A0868u, UINT32_MAX);
    }
    w_u32(0x800A0830u, UINT32_MAX);
    return UINT32_MAX;
}
