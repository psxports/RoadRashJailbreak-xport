#ifndef RRJ_RACE_BODYLESS_BATCH_005_H
#define RRJ_RACE_BODYLESS_BATCH_005_H

#include "race_leaf.h"
#include "race_native_abi.h"

uint32_t sub_80020BEC(RRJMemory *m, const uint32_t arguments[4], uint32_t runtime_gp, uint32_t incoming_sp, RRJRaceLeafCall call);
uint32_t sub_80021058(RRJMemory *m, uint32_t pairs, uint32_t key, uint32_t output, uint32_t index_output);
uint32_t sub_800210BC(void);
uint32_t sub_800210C4(RRJMemory *m, RRJRaceLeafCall call);
uint32_t sub_800211F4(RRJMemory *m, uint32_t runtime_gp);

uint32_t sub_80023498(RRJMemory *m, uint32_t runtime_gp, uint32_t incoming_sp, RRJRaceLeafCall call);
uint32_t sub_80023F08(RRJMemory *m, uint32_t output, uint32_t name, RRJRaceLeafCall call);
uint32_t sub_8002428C(RRJMemory *m, uint32_t name, uint32_t runtime_gp, RRJRaceLeafCall call);

uint32_t sub_80024354(RRJMemory *m, uint32_t runtime_gp, RRJRaceLeafCall call);
uint32_t sub_80023DA4(RRJMemory *m, uint32_t address, RRJNativeCallFrame *frame);
uint32_t sub_80023020(RRJMemory *m, RRJRaceLeafCall call);

uint32_t sub_800235C8(RRJMemory *m, uint32_t first, uint32_t second, uint32_t third, uint32_t runtime_gp, RRJRaceLeafCall call);
uint32_t sub_80023714(RRJMemory *m, uint32_t incoming_sp, RRJRaceLeafCall call);
uint32_t sub_80024168(RRJMemory *m, uint32_t name, uint32_t runtime_gp, uint32_t incoming_sp, RRJRaceLeafCall call);
uint32_t sub_80023C24(RRJMemory *m, uint32_t address, uint32_t handle, uint32_t offset, uint32_t size);
uint32_t sub_80033DAC(RRJMemory *m, uint32_t values, uint32_t auxiliary, uint32_t incoming_v0, RRJRaceLeafCall call);

#endif
