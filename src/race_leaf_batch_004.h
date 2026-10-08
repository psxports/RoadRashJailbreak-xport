#ifndef RRJ_RACE_LEAF_BATCH_004_H
#define RRJ_RACE_LEAF_BATCH_004_H

#include "race_leaf_batch_003.h"

uint32_t sub_80083864(RRJMemory *m, int32_t first, int32_t second, int32_t first_scale, int32_t second_scale, uint32_t first_output, uint32_t second_output);
uint32_t sub_80084BE8(RRJMemory *m, uint32_t actor, uint32_t current);
uint32_t sub_800B658C(RRJMemory *m, uint32_t actor, uint32_t other, int32_t value, int32_t divisor, int32_t factor);
uint32_t sub_800A8DF0(RRJMemory *m, uint32_t actor, uint32_t delta, uint32_t mark_dirty);
uint32_t sub_80080B10(RRJMemory *m, uint32_t actor, uint32_t normal, uint32_t distance_out, uint32_t input, int32_t distance);
uint32_t sub_800A3CBC(RRJMemory *m, uint32_t actor);
uint32_t sub_800849D8(RRJMemory *m, uint32_t actor);
uint32_t sub_800AD9BC(RRJMemory *m, uint32_t actor, int32_t value, int32_t direction, int32_t active, int32_t blend);
uint32_t sub_800B16F4(RRJMemory *m, uint32_t actor, uint32_t value_pointer);
uint32_t sub_800B3838(RRJMemory *m, uint32_t actor, uint32_t first_axis, uint32_t second_axis, uint32_t third_axis, int32_t value);
uint32_t sub_800A9408(RRJMemory *m, uint32_t actor, int32_t amount, int32_t ratio, int32_t divisor, int32_t mode);
uint32_t sub_80084564(RRJMemory *m, uint32_t first, uint32_t second, uint32_t magnitude_pointer, uint32_t output, int32_t response, int32_t minimum, uint32_t zero_vertical);
uint32_t sub_800B0510(RRJMemory *m, uint32_t actor, uint32_t other, uint32_t output);
uint32_t sub_800B3344(RRJMemory *m, uint32_t actor, uint32_t origin, uint32_t normal, uint32_t mode);
uint32_t sub_800B2F94(RRJMemory *m, uint32_t actor, uint32_t other, uint32_t normal);
uint32_t sub_80083928(RRJMemory *m, uint32_t actor, uint32_t other, uint32_t normal);
uint32_t sub_80083F30(RRJMemory *m, uint32_t actor, uint32_t contact, uint32_t normal, int32_t mode);

#endif
