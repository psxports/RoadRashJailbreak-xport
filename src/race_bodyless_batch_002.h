#ifndef RRJ_RACE_BODYLESS_BATCH_002_H
#define RRJ_RACE_BODYLESS_BATCH_002_H

#include "race_leaf.h"
#include "race_bodyless_batch_001.h"

typedef uint32_t (*RRJBodylessBiosCall)(RRJMemory *m, uint32_t table, uint32_t selector, const uint32_t args[8]);
typedef uint32_t (*RRJNativeBiosCall)(RRJMemory *m, uint32_t table, uint32_t selector, RRJNativeCallFrame *frame, uint32_t arguments[8]);

uint32_t sub_80047580(RRJMemory *m, uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8]);
uint32_t sub_80047514(RRJMemory *m, uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call);
uint32_t sub_80015724(RRJMemory *m, uint32_t runtime_gp, RRJNativeCallFrame *frame);
uint32_t sub_80044FE4(RRJMemory *m, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeBiosCall call);
uint32_t sub_80044FF4(RRJMemory *m, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeBiosCall call);
uint32_t sub_80040CB4(RRJMemory *m, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call);
uint32_t sub_80042334(RRJMemory *m, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call);
uint32_t sub_8001B500(RRJMemory *m, uint32_t *runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call);
uint32_t sub_8001DF64(RRJMemory *m, uint32_t *runtime_gp, RRJNativeCallFrame *frame);
uint32_t sub_8001E0DC(RRJMemory *m, RRJNativeCallFrame *frame, uint32_t arguments[8]);
uint32_t sub_8001B6A8(RRJMemory *m, uint32_t runtime_gp, RRJNativeCallFrame *frame);
uint32_t sub_800201C0(RRJMemory *m, uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call);
uint32_t sub_8002026C(RRJMemory *m, uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call);
uint32_t sub_800202B8(RRJMemory *m, uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8]);
uint32_t sub_8001FC84(RRJMemory *m, uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8]);
uint32_t sub_8001EFDC(RRJMemory *m, RRJNativeCallFrame *frame, uint32_t arguments[8]);

#endif
