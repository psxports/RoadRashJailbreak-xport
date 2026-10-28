#ifndef RRJ_CD_TRANSPORT_H
#define RRJ_CD_TRANSPORT_H
#include <stdint.h>

void rrj_cd_stream_enable(uint32_t enabled);
void rrj_cd_stream_service(void);
void rrj_native_wait_events(void);
int rrj_cd_fifo_read(void *destination, uint32_t words);

#endif
