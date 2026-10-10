#ifndef RRJ_RACE_BODYLESS_BATCH_003_H
#define RRJ_RACE_BODYLESS_BATCH_003_H

#include "race_leaf.h"

uint32_t sub_8002490C(uint32_t index, uint32_t enabled, uint32_t incoming_v0);
uint32_t sub_800148DC(uint32_t first, uint32_t second);
uint32_t sub_8001500C(uint32_t index);
uint32_t sub_80046768(uint32_t position);
uint32_t sub_8004DE34(uint32_t address);
uint32_t sub_80050F28(const uint32_t incoming_args[8], RRJRaceLeafCall call);
uint32_t sub_80015530(uint32_t runtime_gp, RRJRaceLeafCall call);
uint32_t sub_8001EFE8(uint32_t enabled, uint32_t incoming_sp, RRJRaceLeafCall call);
uint32_t sub_8001ED28(uint32_t context, uint32_t length, uint32_t slot, uint32_t callback, uint32_t token, uint32_t mode, RRJRaceLeafCall call);
uint32_t sub_8001E938(uint32_t context, uint32_t length, uint32_t callback, uint32_t argument, uint32_t mode, RRJRaceLeafCall call);
uint32_t sub_8001E22C(uint32_t parameters, uint32_t locked, RRJRaceLeafCall call);
uint32_t sub_8001E22C_values(const uint32_t parameters[6], uint32_t locked, RRJRaceLeafCall call);
uint32_t sub_8001E418_values(const uint32_t parameters[6], RRJRaceLeafCall call);
uint32_t sub_8001E418(uint32_t parameters, RRJRaceLeafCall call);
uint32_t sub_8001EAA4(uint32_t data, uint32_t base);
uint32_t sub_8001E328(RRJRaceLeafCall call);
uint32_t sub_8002CA64(uint32_t source, uint32_t x, uint32_t y, uint32_t runtime_gp, uint32_t incoming_sp, RRJRaceLeafCall call);
uint32_t sub_8002C9A8(uint32_t incoming_v0);

#endif
