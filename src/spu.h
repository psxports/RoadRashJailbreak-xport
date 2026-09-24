#ifndef RRJ_SPU_H
#define RRJ_SPU_H

#include "audio_flush.h"

void rrj_spu_initialize(const void *ram);
uint32 rrj_spu_setup(RRJMemory *memory, uint32 voice, const RRJVoiceSetup *setup);
void rrj_spu_command(RRJMemory *memory, uint32 function, uint32 mode, uint32 mask);
void rrj_spu_set_cd_volume(sint16 left, sint16 right);
uint32 rrj_spu_reverb(RRJMemory *memory, uint32 mode, uint32 mask);

#endif
