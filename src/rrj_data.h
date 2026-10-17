#ifndef RRJ_DATA_H
#define RRJ_DATA_H

#include <stdint.h>

int rrj_data_enabled(void);
int rrj_data_init(void);
void rrj_data_reset(void);
uint32_t rrj_data_open(const char *path);
uint32_t rrj_data_close(uint32_t index);
uint32_t rrj_data_read(uint32_t index, void *destination, uint32_t count);
uint32_t rrj_data_stream_read(uint32_t *count);
void rrj_data_stream_close(void);

#endif
