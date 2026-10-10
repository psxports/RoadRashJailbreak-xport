#ifndef RRJ_RACE_AUDIO_FRONTIER_H
#define RRJ_RACE_AUDIO_FRONTIER_H

#include "audio_flush.h"
#include "race_leaf.h"
#if defined(LOCKSTEP_DEBUG)
sint32 rrj_audio_checkpoint_io(FILE *file, sint32 load);
#endif

uint32_t sub_800270F0(uint32_t record);
uint32_t sub_8004F0A8(int32_t index, uint32_t value);
uint32_t sub_80051A38(int32_t first, int32_t second, int32_t third, int32_t fourth);
uint32_t sub_800506A8(uint32_t mode, uint32_t mask, uint32_t first_index, uint32_t second_index);
uint32_t sub_80051088(uint32_t mode, uint32_t mask);
uint32_t sub_8001F900(uint32_t handle, uint32_t enabled);
uint32_t sub_80051C38(uint32_t voice, uint32_t parameters);
uint32_t sub_8001F874(int32_t offset, uint32_t handle);
uint32_t sub_80016768(uint32_t player_index, uint32_t first, uint32_t second, uint32_t third, uint32_t fourth, uint32_t phase);
uint32_t sub_800167A4(uint32_t player_index);
uint32_t sub_8001B244(uint32_t mode);
uint32_t sub_80027778(uint32_t actor, uint32_t kind, uint32_t duration, uint32_t marker);
uint32_t sub_80027974(uint32_t actor, uint32_t unused, uint32_t kind);
uint32_t sub_80027258(uint32_t actor, uint32_t kind);
uint32_t sub_800179D8(uint32_t slot, uint32_t listener);
uint32_t sub_800169D0(uint32_t player_index, RRJReverbCall reverb);
uint32_t sub_80016E4C(uint32_t player_index, RRJRaceLeafCall call);
uint32_t sub_800184AC(uint32_t player_index, RRJRaceLeafCall call, RRJReverbCall reverb);
uint32_t sub_80018FAC(RRJRaceLeafCall call, RRJReverbCall reverb);
uint32_t sub_80019990(RRJRaceLeafCall call, RRJVoiceSetupCall setup, RRJSDKCall key);

#endif
