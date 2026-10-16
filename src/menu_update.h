#ifndef RRJ_MENU_UPDATE_H
#define RRJ_MENU_UPDATE_H
#include "psx_memory.h"

typedef uint32_t (*RRJMenuUpdateFunction)(uint32_t menu);
RRJMenuUpdateFunction rrj_menu_update_function(uint32_t address);
uint32_t sub_800667E4(void);
#endif
