#ifndef RRJ_RACE_BODYLESS_BATCH_006_H
#define RRJ_RACE_BODYLESS_BATCH_006_H

#include "race_leaf.h"
#include "race_native_abi.h"

typedef uint32_t (*RRJBodylessRegistersCall)(RRJMemory *m, uint32_t target, uint32_t arguments[8]);

uint32_t sub_8002289C(uint32_t record, uint32_t keys, uint32_t pointers, uint32_t slot);
uint32_t sub_80033EA0(uint32_t flags, uint32_t output, uint32_t record);
uint32_t sub_80022758(uint32_t record, uint32_t keys, uint32_t pointers, uint32_t slot);
uint32_t sub_80033E7C(uint32_t output);
uint32_t sub_80033E5C(RRJNativeCallFrame *frame, uint32_t arguments[8]);
uint32_t sub_80033D70(uint32_t values, uint32_t auxiliary, uint32_t first_index, uint32_t second_index);

uint32_t sub_80023DE4(void);
uint32_t sub_800243BC(uint32_t offset);
uint32_t sub_8002F308(uint32_t source, uint32_t target, uint32_t kind, uint32_t state, uint32_t incoming_sp);
uint32_t sub_80018DC8(uint32_t mode, uint32_t runtime_gp);

uint32_t sub_8002B83C(void);
uint32_t sub_8002201C(uint32_t value, uint32_t incoming_v0);
uint32_t sub_8004D1A4(uint32_t value, uint32_t incoming_v0);
uint32_t sub_80022064(uint32_t x, uint32_t y, uint32_t runtime_gp);
uint32_t sub_800248E4(uint32_t arguments[8], RRJBodylessRegistersCall call);
uint32_t sub_80025038(uint32_t first, uint32_t second);

#endif
