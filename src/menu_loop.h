#ifndef RRJ_MENU_LOOP_H
#define RRJ_MENU_LOOP_H
#include "psx_memory.h"
/* One complete original iteration; host scheduling may yield only afterwards. */
void rrj_menu_iteration(void);
uint32_t rrj_menu_prepare_iteration(void);
void rrj_menu_finish_iteration(void);
uint32_t sub_80080274(void);
#endif
