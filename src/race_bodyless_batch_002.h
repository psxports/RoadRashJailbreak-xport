#ifndef RRJ_RACE_BODYLESS_BATCH_002_H
#define RRJ_RACE_BODYLESS_BATCH_002_H

#include "race_leaf.h"
#include "race_bodyless_batch_001.h"

typedef uint32_t (*RRJBodylessBiosCall)(RRJMemory *m, uint32_t table, uint32_t selector, const uint32_t args[8]);
typedef uint32_t (*RRJNativeBiosCall)(RRJMemory *m, uint32_t table, uint32_t selector, RRJNativeCallFrame *frame, uint32_t arguments[8]);

uint32_t sub_80047580(uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8]);
uint32_t sub_80047514(uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call);
uint32_t sub_80015724(uint32_t runtime_gp, RRJNativeCallFrame *frame);
uint32_t sub_80044FE4(RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeBiosCall call);
uint32_t sub_80044FF4(RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeBiosCall call);
uint32_t sub_80040CB4(RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call);
uint32_t sub_80042334(RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call);
uint32_t sub_8001B500(uint32_t *runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call);
uint32_t sub_8001DF64(uint32_t *runtime_gp, RRJNativeCallFrame *frame);
uint32_t sub_8001E0DC(RRJNativeCallFrame *frame, uint32_t arguments[8]);
uint32_t sub_8001B6A8(uint32_t runtime_gp, RRJNativeCallFrame *frame);
uint32_t sub_800201C0(uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call);
uint32_t sub_8002026C(uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call);
uint32_t sub_800202B8(uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8]);
uint32_t rrj_vlc_build_tables(uint32_t long_table, uint32_t short_table, uint32_t values);
uint32_t rrj_vlc_prepare(uint32_t values, uint32_t incoming_result);
uint32_t sub_8001FC84(uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8]);
uint32_t sub_8001EFDC(RRJNativeCallFrame *frame, uint32_t arguments[8]);

#endif
