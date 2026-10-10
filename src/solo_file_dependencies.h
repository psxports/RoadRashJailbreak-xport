#ifndef RRJ_SOLO_FILE_DEPENDENCIES_H
#define RRJ_SOLO_FILE_DEPENDENCIES_H
#include "race_leaf.h"

uint32_t rrj_solo_open_path(RRJMemory *m, const char *path, uint32_t device);
uint32_t rrj_solo_bios_toupper(RRJMemory *m, uint32_t value);

#endif
