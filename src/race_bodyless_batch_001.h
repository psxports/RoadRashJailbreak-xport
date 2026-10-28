#ifndef RRJ_RACE_BODYLESS_BATCH_001_H
#define RRJ_RACE_BODYLESS_BATCH_001_H

#include "race_bodyless_batch_006.h"

uint32_t sub_80047430(void);
uint32_t sub_800473F0(void);
uint32_t sub_800472B4(void);
uint32_t sub_80047020(uint32_t command, const uint8_t *parameter, uint32_t destination, uint32_t asynchronous);
uint32_t sub_80046DD4(uint32_t mode, uint32_t destination);
uint32_t sub_80046844(uint32_t message);
uint32_t sub_8004687C(void);
uint32_t sub_80046A00(void);
uint32_t sub_80047A88(void);
void sub_800467F4(uint32_t destination, const uint8_t source[8]);
uint32_t sub_80046948(void);
uint32_t sub_80046A30(void);
uint32_t sub_80047364(void);
uint32_t sub_80047248(const uint8_t attenuation[4]);
uint32_t sub_8001578C(uint32_t event, uint32_t response);
uint32_t sub_80015860(uint32_t event, uint32_t response);
uint32_t sub_80045BEC(void *destination, uint32_t words);

#endif
