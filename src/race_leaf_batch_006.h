#ifndef RRJ_RACE_LEAF_BATCH_006_H
#define RRJ_RACE_LEAF_BATCH_006_H

#include "race_pause_frontier.h"

uint32_t rrj_actor_begin_animation(uint32_t actor, uint32_t type, uint32_t variant, uint32_t animation, uint32_t speed);
uint32_t sub_8001B44C(uint32_t x, uint32_t z, uint32_t actor, uint32_t active);
uint32_t sub_8003297C(uint32_t packed, uint32_t group);
uint32_t sub_800C1DD4(uint32_t actor);
uint32_t sub_800C2070(uint32_t input, uint32_t selector);
uint32_t sub_800C2100(uint32_t input, uint32_t selector);
uint32_t sub_800C213C(uint32_t input, uint32_t selector);
uint32_t sub_800C258C(uint32_t input, uint32_t actor);
uint32_t sub_80017DA0(uint32_t x, uint32_t z, uint32_t event, uint32_t flags);

#endif
