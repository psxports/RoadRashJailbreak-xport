#ifndef RRJ_LOCKSTEP_H
#define RRJ_LOCKSTEP_H
#include "menu_loop.h"
#include "vblank.h"

#if defined(LOCKSTEP_DEBUG)
sint32 rrj_lockstep_import_initial_gte(const uint32 registers[64]);
sint32 rrj_lockstep_resume_initial_frame(const char *path);
int rrj_lockstep_run(RRJMemory *memory);
int rrj_lockstep_selftest(void);
uint32_t rrj_lockstep_poll(RRJMemory *memory);
sint32 rrj_game_checkpoint_io(FILE *file, sint32 load);
void rrj_lockstep_object_wait(uint32_t player, uint32_t stage, uint32_t far_count, const uint32_t *far_indices, uint32_t near_count, const uint32_t *near_indices, int32_t near_minimum, int32_t near_maximum);
void rrj_lockstep_object_wait_end(void);
void rrj_lockstep_object_wait_resume(void);
#endif
#endif
