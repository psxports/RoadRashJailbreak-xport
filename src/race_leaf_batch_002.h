#ifndef RRJ_RACE_LEAF_BATCH_002_H
#define RRJ_RACE_LEAF_BATCH_002_H

#include "race_leaf_batch_001.h"
#include "race_global.h"

uint32_t sub_8009C308(void);
uint32_t sub_8009C310(void);
uint32_t sub_800CB8C8(uint32_t record, uint32_t mode, uint32_t player);
uint32_t sub_8009C654(uint32_t record, uint32_t identity, uint32_t player);
uint32_t sub_8009CA88(uint32_t road_id, uint32_t table, uint32_t candidates, uint32_t allow_special, uint32_t player);
uint32_t sub_800C3950(uint32_t actor);
uint32_t sub_800B8018(uint32_t delta);
uint32_t sub_800A8C48(uint32_t value, uint32_t packed);
uint32_t sub_800BD34C(uint32_t value, int32_t delta);
uint32_t sub_8009DB58(uint32_t actor, uint32_t other);
uint32_t sub_8009E3F0(uint32_t other, uint32_t actor);
uint32_t sub_8009FC80(uint32_t packed);
uint32_t sub_800BCEEC(uint32_t value, uint32_t step);
uint32_t sub_800BD2A0(uint32_t actor, uint32_t rank);
uint32_t sub_8009DBA0(uint32_t actor, uint32_t other, int32_t first, int32_t second);
uint32_t sub_80097470(uint32_t actor, uint32_t other);
uint32_t sub_8009E178(uint32_t actor);
uint32_t sub_80097388(uint32_t actor);
uint32_t sub_800205C0(int32_t speed, uint32_t mode);
uint32_t sub_8002064C(int32_t speed, uint32_t mode);
uint32_t sub_800206DC(int32_t speed, uint32_t mode);
uint32_t sub_800BEA30(uint32_t point, uint32_t actor);
uint32_t sub_8001B3C8(void);
uint32_t sub_8009E528(uint32_t source, uint32_t actor);
uint32_t sub_800BCF6C(uint32_t actor, uint32_t other);
uint32_t sub_800B8FB0(uint32_t actor, int32_t direction, uint32_t other);
uint32_t sub_8009DC90(uint32_t actor);
uint32_t sub_8009B474(int32_t mode, uint32_t delta);
uint32_t sub_8009E89C(uint32_t delta);
uint32_t sub_800A01CC(uint32_t active[2]);
uint32_t sub_8003C520(uint32_t object);
uint32_t sub_80094184(uint32_t actor);
uint32_t sub_80012C1C(const uint32_t source[3], uint32_t destination[3], int32_t offset);

#endif
