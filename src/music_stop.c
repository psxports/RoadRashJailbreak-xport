#include "psx.h"
/* Original MIPS voice release and stream/music stop, including memory reloads. */
#include "music_stop.h"
#include "audio.h"
#include <stdlib.h>

uint32_t sub_8001FB58(uint32_t voice)
{
    FUNCTION_MARKER(0x8001FB58u, "SLUS_010.53");
    uint32_t slot = rrj_read32(voice + 28), i, tail, next, result;
    rrj_write32(voice + 4, 0);
    rrj_write32(voice + 16, 0);
    for (i = 0; i < 22; ++i)
        if (rrj_read32(0x800D68C8 + 4 * i) == slot)
        {
            rrj_write32(0x800D68C8 + 4 * i, 0xffffffff);
            tail = rrj_read32(0x800D69E8);
            next = tail + 1;
            rrj_write32(0x800D6980 + 4 * tail, slot);
            result = rrj_s32(next) < 25;
            rrj_write32(0x800D69E8, next);
            rrj_write32(0x800D69E8, result ? next : 0);
            return result;
        }
    return 0;
}

uint32_t sub_8001F7EC(uint32_t handle)
{
    FUNCTION_MARKER(0x8001F7ECu, "SLUS_010.53");
    uint32_t voice = rrj_read32(0x800D687C) + 44 * (handle >> 27);
    uint32_t generation = rrj_read32(voice + 4) & 0x7ffffff;
    if (generation != (handle & 0x7ffffff))
        return generation;
    (void)sub_8001EB44(1u << (rrj_read32(voice + 28) & 31));
    (void)sub_8001FB58(voice);
    return SpuSetReverbVoice(1, 1u << (rrj_read32(voice + 28) & 31));
}

uint32_t sub_8001F5D4(uint32_t handle, uint32_t release, RRJVoiceSetupCall setup)
{
    FUNCTION_MARKER(0x8001F5D4u, "SLUS_010.53");
    uint32_t i, stream = 0x800D6870, mask = 1u << (handle >> 27);
    for (i = 0; i < 3; ++i, stream += 20)
        if (rrj_read32(stream + 44) == handle)
        {
            if (release)
            {
                RRJVoiceSetup attr = {0};
                attr.voices = mask;
                attr.mask = 0x40000;
                attr.adsr2 = 0x1fc0;
                if (!setup)
                    abort();
                (void)setup(rrj_host_context(), rrj_read32(stream + 44) >> 27, &attr);
            }
            (void)sub_8001F7EC(handle);
            (void)SpuSetReverbVoice(1, 1u << (rrj_read32(stream + 44) >> 27));
            rrj_write32(stream + 28, 0);
            rrj_write32(stream + 44, 0);
        }
    return 0;
}

uint32_t sub_F_8007F158(uint32_t reset, RRJVoiceSetupCall setup)
{
    FUNCTION_MARKER(0x8007F158u, "RASHCDF.BIN");
    uint32_t restore, release;
    if (rrj_read32(0x800A0838))
        rrj_write32(0x800A0838, 0);
    if (rrj_read32(0x800A0834))
    {
        release = rrj_u16(rrj_at(0x8009C5D0, 2)) == 47;
        (void)sub_8001F5D4(rrj_read32(0x800A0844), release, setup);
        (void)sub_8001F5D4(rrj_read32(0x800A0848), release, setup);
    }
    restore = rrj_read32(0x800A0840);
    rrj_write32(0x800A0834, 0);
    if (restore)
    {
        rrj_write32(0x800A0840, 0);
        rrj_write32(0x800D6C14, rrj_read32(0x800D6C34));
    }
    if (reset)
        rrj_write32(0x800A0830, 0xffffffff);
    return 0xffffffff;
}
