#ifndef RRJ_RACE_BODYLESS_BATCH_004_H
#define RRJ_RACE_BODYLESS_BATCH_004_H

#include "race_leaf.h"
#include "race_native_abi.h"

uint32_t sub_8002C9D4(RRJMemory *m, uint32_t index, RRJRaceLeafCall call);
uint32_t sub_8001B67C(RRJMemory *m, uint32_t index, uint32_t runtime_gp);
uint32_t sub_8001BCE8(RRJMemory *m, uint32_t mode, uint32_t incoming_sp, RRJRaceLeafCall call);
uint32_t sub_8001B8BC(RRJMemory *m, uint32_t incoming_sp, RRJRaceLeafCall call);
uint32_t sub_8001C1AC(RRJMemory *m, RRJRaceLeafCall call);
uint32_t sub_8001C498(RRJMemory *m);
uint32_t sub_8002D2E0(RRJMemory *m, uint32_t runtime_gp, RRJNativeCallFrame *frame);
uint32_t sub_800303BC(RRJMemory *m, uint32_t index, uint32_t key, uint32_t value);
uint32_t sub_80016464(RRJMemory *m, uint32_t handle, uint32_t runtime_gp, RRJRaceLeafCall call);
uint32_t sub_80030810(RRJMemory *m, uint32_t first_output, uint32_t second_output);
uint32_t sub_800244E0(RRJMemory *m, RRJRaceLeafCall call);
uint32_t sub_80023100(RRJMemory *m, uint32_t name, uint32_t runtime_gp, uint32_t incoming_sp, RRJRaceLeafCall call);

uint32_t sub_80024610(RRJMemory *m, RRJNativeCallFrame *frame, uint32_t arguments[8]);
uint32_t sub_80022F78(RRJMemory *m, RRJRaceLeafCall call);
uint32_t sub_80024630(RRJMemory *m, uint32_t runtime_gp, uint32_t incoming_sp, RRJRaceLeafCall call);
uint32_t sub_80024FF8(RRJMemory *m);

#endif
