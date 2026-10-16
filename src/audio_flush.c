#include "psx.h"
/* Pending sound start/stop flush, actual MIPS load/store order. */
#include "audio_flush.h"
#include "spu.h"

uint32_t sub_8001EB7C(uint32_t voice, uint32_t pitch, uint32_t left, uint32_t right)
{
    FUNCTION_MARKER(0x8001EB7Cu, "SLUS_010.53");
    uint32_t sample = r_u32(voice + 8), slot = r_u32(voice + 28);
    RRJVoiceSetup attr;
    attr.mask = 0x60093;
    attr.left = (uint16_t)left;
    attr.right = (uint16_t)right;
    attr.pitch = (uint16_t)pitch;
    attr.voices = 1u << (slot & 31);
    attr.address = r_u32(sample + 8);
    attr.adsr1 = r_u16(sample + 2);
    attr.adsr2 = r_u16(sample + 4);
    return rrj_spu_setup(r_u32(voice + 28), &attr);
}

uint32_t sub_8001EE94(void)
{
    FUNCTION_MARKER(0x8001EE94u, "SLUS_010.53");
    uint32_t mask, count, voice;
    if (r_u32(0x800D6880))
        return 1;
    mask = r_u32(0x800D6888);
    w_u32(0x800D6880, 1);
    if (mask)
    {
        SpuSetKey(SPU_OFF, mask);
        w_u32(0x800D6888, 0);
    }
    if (r_u32(0x800D6884))
    {
        count = r_u32(0x800D6878);
        voice = r_u32(0x800D687C);
        while (count)
        {
            uint32_t slot = r_u32(voice + 28);
            if ((1u << (slot & 31)) & r_u32(0x800D6884))
            {
                uint32_t pitch = r_u32(voice + 32), left = r_u32(voice + 36), right = r_u32(voice + 40);
                (void)sub_8001EB7C(voice, pitch, left, right);
            }
            w_u32(voice + 16, 2);
            --count;
            voice += 44;
        }
        mask = r_u32(0x800D6884);
        if (mask)
        {
            SpuSetKey(SPU_ON, mask);
        }
    }
    w_u32(0x800D6884, 0);
    w_u32(0x800D6880, 0);
    return 0x800D6870;
}
