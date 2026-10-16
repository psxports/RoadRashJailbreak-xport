#ifndef RRJ_VBLANK_H
#define RRJ_VBLANK_H
#include "psx_memory.h"
/* All remaining callees here have zero or one semantic game argument. */
typedef uint32_t (*RRJ_VBLANK_FUNCTION)(void);
RRJ_VBLANK_FUNCTION rrj_vblank_function(uint32_t address);
void sub_80064BE4(void);
uint32_t sub_8001B700(void);
uint32_t sub_80064C30(void);
uint32_t sub_800600F8(void);
#endif
