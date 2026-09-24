#ifndef RRJ_RACE_LEAF_BATCH_003_H
#define RRJ_RACE_LEAF_BATCH_003_H

#include "race_leaf_batch_002.h"

uint32_t sub_8008DBA8(RRJMemory *m, uint32_t player);
int32_t sub_8003A98C(RRJMemory *m, int32_t direction, uint32_t actor);
uint32_t sub_8009F578(RRJMemory *m, uint32_t packet);
uint32_t sub_8009F478(RRJMemory *m, uint32_t player, uint32_t actor,
                      uint32_t route);
uint32_t sub_8009F44C(RRJMemory *m, uint32_t player);
uint32_t sub_8009FF2C(RRJMemory *m, uint32_t active[2], uint32_t state);
uint32_t sub_8009FF24(RRJMemory *m, uint32_t active[2]);
uint32_t sub_800C45D8(RRJMemory *m, uint32_t object, uint32_t flags_out);
uint32_t sub_800A3ECC(RRJMemory *m, uint32_t object, uint32_t index);
uint32_t sub_800710C0(RRJMemory *m, uint32_t amount, uint32_t first,
                      uint32_t second, uint32_t output);
uint32_t sub_8005D36C(RRJMemory *m, uint32_t actor);
uint32_t sub_800B6BD0(RRJMemory *m, uint32_t first, uint32_t normal,
                      uint32_t plane, uint32_t edge);

#endif
