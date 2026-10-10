#ifndef RRJ_RACE_BODYLESS_BATCH_004_H
#define RRJ_RACE_BODYLESS_BATCH_004_H

#include "race_leaf.h"
#include "race_native_abi.h"

uint32_t sub_8002C9D4(uint32_t index);
uint32_t sub_8001B67C(uint32_t index);
uint32_t sub_8001BCE8(uint32_t mode, uint32_t incoming_sp, RRJRaceLeafCall call);
uint32_t sub_8001B8BC(uint32_t incoming_sp, RRJRaceLeafCall call);
uint32_t sub_8001C1AC(RRJRaceLeafCall call);
uint32_t sub_8001C498(void);
uint32_t sub_8002D2E0(uint32_t runtime_gp, RRJNativeCallFrame *frame);
uint32_t sub_800303BC(uint32_t index, uint32_t key, uint32_t value);
uint32_t sub_80016464(uint32_t handle, RRJRaceLeafCall call);
uint32_t sub_80030810(uint32_t first_output, uint32_t second_output);
uint32_t sub_800244E0(RRJRaceLeafCall call);
uint32_t sub_80023100(uint32_t name);

uint32_t sub_80024610(RRJNativeCallFrame *frame, uint32_t arguments[8]);
uint32_t sub_80022F78(RRJRaceLeafCall call);
uint32_t sub_80024630(uint32_t runtime_gp, uint32_t incoming_sp, RRJRaceLeafCall call);
uint32_t sub_80024FF8(void);

#endif
