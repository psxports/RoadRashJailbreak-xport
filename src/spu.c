#include "spu.h"
#include "psx_spu.h"
#include "wip.h"

#include <stdlib.h>
#include <string.h>

static uint32 reverb_mask;


uint32 rrj_spu_reverb(RRJMemory *memory, uint32 mode, uint32 mask)
{
    mask &= SPU_ALLCH;
    xport_audio_lock();
    if (mode == 0)
        reverb_mask &= ~mask;
    else if (mode == 1)
        reverb_mask |= mask;
    else
        abort();
    mask = reverb_mask;
    xport_audio_unlock();
    return mask;
}



uint32 rrj_spu_setup(uint32 voice, const RRJVoiceSetup *setup)
{
    SpuVoiceAttr attr;

    if (voice >= SPU_VOICE_COUNT || (setup->mask != 0x60093 && setup->mask != 0x40000))
        abort();
    memset(&attr, 0, sizeof(attr));
    attr.voice = SPU_KEYCH(voice);
    attr.mask = SPU_VOICE_ADSR_ADSR2;
    attr.adsr2 = setup->adsr2;
    if (setup->mask == 0x60093)
    {
        if (setup->address >= SPU_RAM_SIZE)
            abort();
        attr.mask |= SPU_VOICE_VOLL | SPU_VOICE_VOLR | SPU_VOICE_PITCH | SPU_VOICE_WDSA | SPU_VOICE_ADSR_ADSR1;
        attr.volume.left = (sint16)setup->left;
        attr.volume.right = (sint16)setup->right;
        attr.pitch = setup->pitch;
        attr.addr = setup->address;
        attr.adsr1 = setup->adsr1;
    }
    SpuSetVoiceAttr(&attr);
    return 0;
}



void rrj_spu_set_cd_volume(sint16 left, sint16 right)
{
    SsSetSerialVol(SS_SERIAL_A, left, right);
}
