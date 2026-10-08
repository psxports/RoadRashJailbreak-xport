#ifndef RRJ_LOCKSTEP_H
#define RRJ_LOCKSTEP_H
#include "menu_loop.h"
#include "vblank.h"

#if defined(LOCKSTEP_DEBUG)
int rrj_lockstep_run(RRJMemory *memory, RRJLoopCall loop, RRJVBlankCall service);
int rrj_lockstep_selftest(void);
uint32_t rrj_lockstep_poll(RRJMemory *memory);
sint32 rrj_game_checkpoint_io(FILE *file, sint32 load);
#endif
#endif
