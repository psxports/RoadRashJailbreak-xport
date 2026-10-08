#ifndef RRJ_RACE_BODYLESS_BATCH_003_H
#define RRJ_RACE_BODYLESS_BATCH_003_H

#include "race_leaf.h"

uint32_t sub_8002490C(RRJMemory *m, uint32_t index, uint32_t enabled, uint32_t incoming_v0);
uint32_t sub_800148DC(RRJMemory *m, uint32_t first, uint32_t second, RRJRaceLeafCall call);
uint32_t sub_8001500C(RRJMemory *m, uint32_t index);
uint32_t sub_80046768(RRJMemory *m, uint32_t position);
uint32_t sub_8004DE34(RRJMemory *m, uint32_t address);
uint32_t sub_80050F28(RRJMemory *m, const uint32_t incoming_args[8], RRJRaceLeafCall call);
uint32_t sub_80015530(RRJMemory *m, uint32_t runtime_gp, RRJRaceLeafCall call);
uint32_t sub_8001EFE8(RRJMemory *m, uint32_t enabled, uint32_t incoming_sp, RRJRaceLeafCall call);
uint32_t sub_8001ED28(RRJMemory *m, uint32_t context, uint32_t length, uint32_t slot, uint32_t callback, uint32_t incoming_sp, RRJRaceLeafCall call);
uint32_t sub_8001E938(RRJMemory *m, uint32_t context, uint32_t length, uint32_t callback, uint32_t argument, uint32_t incoming_sp, RRJRaceLeafCall call);
uint32_t sub_8001E22C(RRJMemory *m, uint32_t parameters, uint32_t locked, RRJRaceLeafCall call);
uint32_t sub_8001E418(RRJMemory *m, uint32_t parameters, RRJRaceLeafCall call);
uint32_t sub_8001EAA4(RRJMemory *m, uint32_t data, uint32_t base);
uint32_t sub_8001E328(RRJMemory *m, uint32_t incoming_sp, RRJRaceLeafCall call);
uint32_t sub_8002CA64(RRJMemory *m, uint32_t source, uint32_t x, uint32_t y, uint32_t runtime_gp, uint32_t incoming_sp, RRJRaceLeafCall call);
uint32_t sub_8002C9A8(RRJMemory *m, uint32_t incoming_v0, RRJRaceLeafCall call);

#endif
