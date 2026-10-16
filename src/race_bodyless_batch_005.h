#ifndef RRJ_RACE_BODYLESS_BATCH_005_H
#define RRJ_RACE_BODYLESS_BATCH_005_H

#include "race_leaf.h"
#include "race_native_abi.h"

uint32_t sub_80020BEC(const uint32_t *pairs);
uint32_t sub_80021058(const uint32_t *pairs, uint32_t key, uint32_t *output, uint32_t *index_output);
uint32_t sub_800210BC(void);
uint32_t sub_800210C4(void);
uint32_t sub_800211F4(void);

uint32_t sub_80023498(void);
uint32_t sub_80023F08(uint32_t output, const char *name);
uint32_t sub_8002428C(const char *name);

uint32_t sub_80024354(void);
uint32_t sub_80023DA4(uint32_t address);
uint32_t sub_80023020(void);

uint32_t sub_800235C8(uint32_t first, uint32_t second, uint32_t third);
uint32_t sub_80023714(void);
uint32_t sub_80024168(const char *name);
uint32_t sub_80023C24(uint32_t address, uint32_t handle, uint32_t offset, uint32_t size);
void sub_80033DAC(uint32_t values[3], uint32_t auxiliary[3]);

#endif
