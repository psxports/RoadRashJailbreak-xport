#ifndef RRJ_RACE_BODYLESS_BATCH_006_H
#define RRJ_RACE_BODYLESS_BATCH_006_H

#include "race_leaf.h"
#include "race_native_abi.h"

uint32_t sub_8002289C(uint8_t record[9], const uint32_t keys[3], const uint32_t pointers[3], uint32_t slot);
uint32_t sub_80033EA0(uint32_t flags, uint32_t output, const uint8_t record[9]);
uint32_t sub_80022758(uint8_t record[9], const uint32_t keys[3], const uint32_t pointers[3], uint32_t slot);
uint32_t sub_80033E7C(uint32_t output);
uint32_t sub_80033E5C(uint32_t output);
uint32_t sub_80033D70(uint32_t values[3], uint32_t auxiliary[3], uint32_t first_index, uint32_t second_index);

uint32_t sub_80023DE4(void);
uint32_t sub_800243BC(uint32_t offset);
uint32_t sub_8002F308(uint32_t source, uint32_t target, uint32_t kind, uint32_t state, uint32_t option);
uint32_t sub_80018DC8(uint32_t mode);

uint32_t sub_8002B83C(void);
void sub_8002201C(uint32_t value);
void sub_8004D1A4(uint32_t value);
uint32_t sub_80022064(uint32_t x, uint32_t y);
uint32_t sub_800248E4(uint32_t first, uint32_t second);
uint32_t sub_80025038(uint32_t first, uint32_t second);

#endif
