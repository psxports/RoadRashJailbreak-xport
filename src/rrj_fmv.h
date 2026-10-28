#ifndef RRJ_FMV_H
#define RRJ_FMV_H

#include "xport.h"

int rrj_fmv_install(void);
int rrj_menu_fmv_open(uint32 descriptor);
int rrj_menu_fmv_active(void);
uint32 rrj_menu_fmv_next(uint32 x, uint32 y, uint32 mode);
void rrj_menu_fmv_present(void);
void rrj_menu_fmv_close(void);
int rrj_fmv_path(uint32 descriptor, char *path, uint32 capacity);

#endif
