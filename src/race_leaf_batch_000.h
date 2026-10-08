#ifndef RRJ_RACE_LEAF_BATCH_000_H
#define RRJ_RACE_LEAF_BATCH_000_H

#include "race_leaf.h"

uint32_t sub_8001DA7C(RRJMemory *m);
uint32_t sub_8001DB90(RRJMemory *m);
uint32_t sub_8002D0A8(RRJMemory *m, uint32_t font, uint32_t string_id);
uint32_t sub_8003F9D8(RRJMemory *m, uint32_t actor);
uint32_t sub_8003F680(RRJMemory *m, uint32_t actor_id, uint32_t flag);
uint32_t sub_8003F8D8(RRJMemory *m);
uint32_t sub_8003F708(RRJMemory *m);
uint32_t sub_8002DC1C(RRJMemory *m, RRJRaceLeafCall call);
uint32_t sub_8002DD80(RRJMemory *m);
uint32_t sub_8004CDE4(RRJMemory *m, uint32_t packet, uint32_t enabled);
uint32_t sub_800400F4(RRJMemory *m, uint32_t red, uint32_t green, uint32_t blue, uint32_t ordering_slot, int32_t left, int32_t top, int32_t right, int32_t bottom);
uint32_t sub_8002D9E8(RRJMemory *m, uint32_t string_id);
uint32_t sub_8002D718(RRJMemory *m);
uint32_t sub_8002D2F4(RRJMemory *m, RRJRaceLeafCall call);
uint32_t sub_80086B1C(RRJMemory *m, uint32_t actor, int32_t delta, int32_t target);
uint32_t sub_8002FA28(int32_t phase, uint32_t first, uint32_t second, uint32_t third, uint32_t fourth, RRJMemory *m);
uint32_t sub_8008676C(RRJMemory *m, uint32_t actor, int32_t delta);
uint32_t sub_8002E6F8(RRJMemory *m, uint32_t first, uint32_t second, uint32_t output, int32_t first_weight, int32_t second_weight);
uint32_t sub_80086C00(RRJMemory *m, uint32_t value_address, int32_t target, int32_t delta, int32_t smoothing);
uint32_t sub_80086D54(RRJMemory *m, uint32_t value_address, int32_t target, int32_t delta, uint32_t velocity_address, int32_t spring, int32_t damping);
uint32_t sub_800A451C(RRJMemory *m, uint32_t actor, uint32_t mode);
uint32_t sub_800A421C(RRJMemory *m, uint32_t actor, RRJRaceLeafCall call);

#endif
