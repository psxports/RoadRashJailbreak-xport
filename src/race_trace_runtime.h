#ifndef RRJ_RACE_TRACE_RUNTIME_H
#define RRJ_RACE_TRACE_RUNTIME_H

#include "psx_memory.h"

int rrj_trace_runtime_init(RRJMemory *m, uint32_t phase_base);
int rrj_trace_runtime_init_live(RRJMemory *m);
void rrj_trace_set_live_limit(uint32 limit);
int rrj_trace_runtime_is_live(void);
int rrj_trace_phase_boundary(RRJMemory *m, uint32_t pc);
int rrj_trace_input_packet(RRJMemory *m, uint32_t controller, uint32_t raw);
uint32_t rrj_trace_vblanks_before_aux(RRJMemory *m);
int rrj_trace_input_pending_at_tick(RRJMemory *m);
uint32_t rrj_trace_vblank_boundary(RRJMemory *m);
int rrj_trace_runtime_failed(void);
void rrj_trace_dispatch_target(uint32_t target);
uint32_t rrj_trace_current_dispatch(void);
int rrj_trace_runtime_finish(int result);

#endif
