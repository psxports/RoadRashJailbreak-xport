#ifndef RRJ_RACE_NATIVE_DISPATCH_H
#define RRJ_RACE_NATIVE_DISPATCH_H

#include "race_bodyless_batch_002.h"

struct RRJNativeDispatchContext
{
    uint32_t *runtime_gp;
    RRJNativeFrameCall library_call;
    RRJNativeBiosCall bios_call;
};

uint32_t rrj_native_frame_call(RRJMemory *m, uint32_t target, RRJNativeCallFrame *frame, uint32_t arguments[8]);

#endif
