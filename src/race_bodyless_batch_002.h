#ifndef RRJ_RACE_BODYLESS_BATCH_002_H
#define RRJ_RACE_BODYLESS_BATCH_002_H

#include "race_leaf.h"
#include "race_bodyless_batch_001.h"

uint32_t sub_80047580(void *destination, uint32_t words);
uint32_t sub_80047514(uint32_t mode);
uint32_t sub_80015724(void);
void sub_80044FE4(uint32_t mode);
void sub_80044FF4(void);
uint32_t sub_80040CB4(void);
uint32_t sub_80042334(uint32_t owner);
uint32_t sub_8001B500(void);
uint32_t sub_8001DF64(void);
uint32_t sub_8001E0DC(uint32_t destination, uint32_t value, uint32_t bytes);
uint32_t sub_8001B6A8(void);
uint32_t sub_800201C0(void);

uint32_t sub_800202B8(uint32_t long_table, uint32_t short_table, uint32_t values);
void sub_8002026C(uint32_t values);
void sub_8001FC84(uint32_t value);
uint32_t sub_8001EFDC(uint32_t value);

typedef uint32_t (*RRJ_PAD_FUNCTION)(uint32_t owner);
RRJ_PAD_FUNCTION rrj_pad_function(uint32_t address);
uint32_t sub_80042B9C(uint32_t owner);
uint32_t sub_80043218(uint32_t owner);
uint32_t sub_80042288(uint32_t owner, uint32_t mode);
uint32_t sub_800422A8(uint32_t owner);
uint32_t sub_800422BC(uint32_t owner, uint32_t mode);
uint32_t sub_800422FC(uint32_t owner, uint32_t mode);
uint32_t sub_80041750(uint32_t owner);
uint32_t sub_80043C94(void);
uint32_t sub_800411E8(uint32_t owner, int32_t command);

uint32_t sub_80043174(uint32_t port);
uint32_t sub_80043BBC(uint32_t owner);
uint32_t sub_8004213C(uint32_t owner, uint32_t mode, uint32_t value);

#endif
