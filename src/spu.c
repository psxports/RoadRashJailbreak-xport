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

void rrj_spu_initialize(const void *ram)
{
    SpuCommonAttr common;

    SpuInit();
    reverb_mask = 0;
    if (!spu_upload(0, ram, SPU_RAM_SIZE))
        abort();
    memset(&common, 0, sizeof(common));
    common.mask = SPU_COMMON_MVOLL | SPU_COMMON_MVOLR;
    common.mvol.left = 0x3fff;
    common.mvol.right = 0x3fff;
    SpuSetCommonAttr(&common);
}

uint32 rrj_spu_setup(RRJMemory *memory, uint32 voice, const RRJVoiceSetup *setup)
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

void rrj_spu_command(RRJMemory *memory, uint32 function, uint32 mode, uint32 mask)
{
    if (function == 0x80050D08)
    {
        if (mode != 0 && mode != 1)
            abort();
        SpuSetKey(mode == 0 ? SPU_OFF : SPU_ON, mask & SPU_ALLCH);
        return;
    }
    if (function == 0x80050678)
    {
        (void)rrj_spu_reverb(memory, mode, mask);
        return;
    }
    RRJ_WIP3(memory, function, "spu_command", mode, mask, 0);
}

void rrj_spu_set_cd_volume(sint16 left, sint16 right)
{
    SsSetSerialVol(SS_SERIAL_A, left, right);
}
