#ifndef RRJ_RACE_AUDIO_FRONTIER_H
#define RRJ_RACE_AUDIO_FRONTIER_H

#include "audio_flush.h"
#include "race_leaf.h"

uint32_t sub_800270F0(RRJMemory *m, uint32_t record);
uint32_t sub_8004F0A8(RRJMemory *m, int32_t index, uint32_t value);
uint32_t sub_80051A38(RRJMemory *m, int32_t first, int32_t second,
                      int32_t third, int32_t fourth);
uint32_t sub_800506A8(RRJMemory *m, uint32_t mode, uint32_t mask,
                      uint32_t first_index, uint32_t second_index);
uint32_t sub_80051088(RRJMemory *m, uint32_t mode, uint32_t mask);
uint32_t sub_8001F900(RRJMemory *m, uint32_t handle, uint32_t enabled);
uint32_t sub_80051C38(RRJMemory *m, uint32_t voice, uint32_t parameters);
uint32_t sub_8001F874(RRJMemory *m, int32_t offset, uint32_t handle);
uint32_t sub_80016768(RRJMemory *m, uint32_t player_index, uint32_t first,
                      uint32_t second, uint32_t third, uint32_t fourth,
                      uint32_t phase);
uint32_t sub_800167A4(RRJMemory *m, uint32_t player_index,
                      RRJReverbCall reverb);
uint32_t sub_8001B244(RRJMemory *m, uint32_t mode, RRJReverbCall reverb);
uint32_t sub_80027778(RRJMemory *m, uint32_t actor, uint32_t kind,
                      uint32_t duration, uint32_t marker);
uint32_t sub_80027974(RRJMemory *m, uint32_t actor, uint32_t unused,
                      uint32_t kind);
uint32_t sub_80027258(RRJMemory *m, uint32_t actor, uint32_t kind);
uint32_t sub_800179D8(RRJMemory *m, uint32_t slot, uint32_t listener,
                      RRJRaceLeafCall call);
uint32_t sub_800169D0(RRJMemory *m, uint32_t player_index,
                      RRJReverbCall reverb);
uint32_t sub_80016E4C(RRJMemory *m, uint32_t player_index,
                      RRJRaceLeafCall call, RRJReverbCall reverb);
uint32_t sub_800184AC(RRJMemory *m, uint32_t player_index,
                      RRJRaceLeafCall call, RRJReverbCall reverb);
uint32_t sub_80018FAC(RRJMemory *m, RRJRaceLeafCall call,
                      RRJReverbCall reverb);
uint32_t sub_80019990(RRJMemory *m, RRJRaceLeafCall call,
                      RRJVoiceSetupCall setup, RRJSDKCall key);

#endif
