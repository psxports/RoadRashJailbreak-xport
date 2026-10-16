#ifndef RRJ_RACE_LEAF_BATCH_001_H
#define RRJ_RACE_LEAF_BATCH_001_H

#include "race_leaf.h"

uint32_t sub_8003FA18(int32_t count, const uint16_t *source, uint16_t *destination);
uint32_t sub_8003C42C(uint32_t output, int32_t limit);
uint32_t sub_8003C494(uint32_t identity, uint32_t road_id);
uint32_t sub_8008D9E4(uint32_t player);
uint32_t sub_8008DA20(uint32_t identity, uint32_t player);
uint32_t sub_8008DECC(uint32_t player, uint32_t group);
uint32_t sub_80095848(void);
uint32_t sub_8009BB48(uint32_t record, uint32_t player);
uint32_t sub_8009C41C(uint32_t record, uint32_t kind, uint32_t index, uint32_t value);
uint32_t sub_8009C4FC(uint32_t record, uint32_t candidates, int32_t count, uint32_t player);
uint32_t sub_8009C5E4(int32_t index);
uint32_t sub_8009EF8C(uint32_t candidates, uint32_t output, int32_t limit);
uint32_t sub_8009F054(uint32_t candidates, uint32_t player);
uint32_t sub_8009F11C(int32_t road_id, uint32_t candidates);
uint32_t sub_8009F288(uint32_t record, uint32_t road_ids, int32_t count);
uint32_t sub_8009FAD8(uint32_t output, uint32_t player);
uint32_t sub_8009FC4C(uint32_t value, uint32_t values, int32_t count);
uint32_t sub_800A1318(uint32_t actor, const uint32_t motion[3], const uint16_t basis[9]);
uint32_t sub_800A0A20(uint32_t selector, uint32_t mode, const uint32_t position[3], const uint16_t rotation[9], uint32_t config);
uint32_t sub_800A0EC4(uint32_t event, const uint32_t position[3], const uint16_t rotation[9]);
uint32_t sub_800A0F9C(uint32_t config);
uint32_t sub_800A2448(uint32_t config, uint32_t other);
uint32_t sub_800CAAA8(uint32_t actor);
uint32_t sub_800CB78C(uint32_t actor, uint32_t kind, uint32_t identity);

uint32_t sub_800A13D4(int32_t delta, uint32_t player, int32_t initial_actor_count);

uint32_t sub_8007ED64(uint32_t actor, int32_t angle);

uint32_t sub_800871A8(const uint32_t position[3], const uint32_t origin[3], int32_t distance_scale, uint32_t actor, uint32_t output[3]);

#endif
