#ifndef RRJ_MENU_RENDER_H
#define RRJ_MENU_RENDER_H
#include <stdint.h>

typedef uint32_t (*RRJMenuEntryFunction)(uint32_t menu, uint32_t entry);
RRJMenuEntryFunction rrj_menu_entry_function(uint32_t address);
#endif
