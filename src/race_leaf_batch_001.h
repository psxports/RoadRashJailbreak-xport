#ifndef RRJ_RACE_LEAF_BATCH_001_H
#define RRJ_RACE_LEAF_BATCH_001_H

#include "race_leaf.h"

uint32_t sub_8003FA18(RRJMemory *m, int32_t count, uint32_t source, uint32_t destination);
uint32_t sub_8003C42C(RRJMemory *m, uint32_t output, int32_t limit);
uint32_t sub_8003C494(RRJMemory *m, uint32_t identity, uint32_t road_id);
uint32_t sub_8008D9E4(RRJMemory *m, uint32_t player);
uint32_t sub_8008DA20(RRJMemory *m, uint32_t identity, uint32_t player);
uint32_t sub_8008DECC(RRJMemory *m, uint32_t player, uint32_t group);
uint32_t sub_80095848(RRJMemory *m);
uint32_t sub_8009BB48(RRJMemory *m, uint32_t record, uint32_t player);
uint32_t sub_8009C41C(RRJMemory *m, uint32_t record, uint32_t kind, uint32_t index, uint32_t value);
uint32_t sub_8009C4FC(RRJMemory *m, uint32_t record, uint32_t candidates, int32_t count, uint32_t player);
uint32_t sub_8009C5E4(RRJMemory *m, int32_t index);
uint32_t sub_8009EF8C(RRJMemory *m, uint32_t candidates, uint32_t output, int32_t limit);
uint32_t sub_8009F054(RRJMemory *m, uint32_t candidates, uint32_t player);
uint32_t sub_8009F11C(RRJMemory *m, int32_t road_id, uint32_t candidates);
uint32_t sub_8009F288(RRJMemory *m, uint32_t record, uint32_t road_ids, int32_t count);
uint32_t sub_8009FAD8(RRJMemory *m, uint32_t output, uint32_t player);
uint32_t sub_8009FC4C(RRJMemory *m, uint32_t value, uint32_t values, int32_t count);
uint32_t sub_800A1318(RRJMemory *m, uint32_t actor, uint32_t motion, uint32_t basis);
uint32_t sub_800A0A20(RRJMemory *m, uint32_t selector, uint32_t mode, uint32_t position, uint32_t rotation, uint32_t config, RRJRaceLeafCall call);
uint32_t sub_800A2448(RRJMemory *m, uint32_t config, uint32_t other, RRJRaceLeafCall call);
uint32_t sub_800CAAA8(RRJMemory *m, uint32_t actor);
uint32_t sub_800CB78C(RRJMemory *m, uint32_t actor, uint32_t kind, uint32_t identity);

#endif
