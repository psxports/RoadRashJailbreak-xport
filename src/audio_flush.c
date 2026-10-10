#include "psx.h"
/* Pending sound start/stop flush, actual MIPS load/store order. */
#include "audio_flush.h"
#include <stdlib.h>

uint32_t sub_8001EB7C(uint32_t voice, uint32_t pitch, uint32_t left, uint32_t right, RRJVoiceSetupCall setup)
{
    FUNCTION_MARKER(0x8001EB7Cu, "SLUS_010.53");
    uint32_t sample = rrj_read32(voice + 8), slot = rrj_read32(voice + 28);
    RRJVoiceSetup attr;
    if (!setup)
        abort();
    attr.mask = 0x60093;
    attr.left = (uint16_t)left;
    attr.right = (uint16_t)right;
    attr.pitch = (uint16_t)pitch;
    attr.voices = 1u << (slot & 31);
    attr.address = rrj_read32(sample + 8);
    attr.adsr1 = rrj_u16(rrj_at(sample + 2, 2));
    attr.adsr2 = rrj_u16(rrj_at(sample + 4, 2));
    return setup(rrj_host_context(), rrj_read32(voice + 28), &attr);
}

uint32_t sub_8001EE94(RRJVoiceSetupCall setup, RRJSDKCall key)
{
    FUNCTION_MARKER(0x8001EE94u, "SLUS_010.53");
    uint32_t mask, count, voice;
    if (rrj_read32(0x800D6880))
        return 1;
    mask = rrj_read32(0x800D6888);
    rrj_write32(0x800D6880, 1);
    if (mask)
    {
        if (!key)
            abort();
        key(rrj_host_context(), 0x80050D08, 0, mask);
        rrj_write32(0x800D6888, 0);
    }
    if (rrj_read32(0x800D6884))
    {
        count = rrj_read32(0x800D6878);
        voice = rrj_read32(0x800D687C);
        while (count)
        {
            uint32_t slot = rrj_read32(voice + 28);
            if ((1u << (slot & 31)) & rrj_read32(0x800D6884))
            {
                uint32_t pitch = rrj_read32(voice + 32), left = rrj_read32(voice + 36), right = rrj_read32(voice + 40);
                (void)sub_8001EB7C(voice, pitch, left, right, setup);
            }
            rrj_write32(voice + 16, 2);
            --count;
            voice += 44;
        }
        mask = rrj_read32(0x800D6884);
        if (mask)
        {
            if (!key)
                abort();
            key(rrj_host_context(), 0x80050D08, 1, mask);
        }
    }
    rrj_write32(0x800D6884, 0);
    rrj_write32(0x800D6880, 0);
    return 0x800D6870;
}
