#ifndef RRJ_RACE_FRAME_H
#define RRJ_RACE_FRAME_H
#include "race_leaf.h"
#include "music_stop.h"
uint32_t sub_80011C4C(void);
uint32_t rrj_race_movement_block(uint32_t delta);
uint32_t rrj_race_movement_contact_block(uint32_t delta);
uint32_t rrj_race_movement_route_block(uint32_t delta);
uint32_t rrj_race_movement_surface_block(uint32_t delta);
uint32_t rrj_race_movement_placement_block(uint32_t delta);
uint32_t rrj_race_movement_wheel_block(uint32_t delta);
uint32_t rrj_race_activity_block(void);
uint32_t rrj_race_movement_activity_block(uint32_t delta);
uint32_t rrj_race_player_tail(uint32_t delta);
uint32_t rrj_race_movement_player_block(uint32_t delta);
uint32_t sub_8002CA5C(void);
#endif
