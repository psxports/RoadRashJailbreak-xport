#ifndef RRJ_RACE_BODYLESS_BATCH_001_H
#define RRJ_RACE_BODYLESS_BATCH_001_H

#include "race_bodyless_batch_006.h"
#include "race_native_abi.h"

typedef uint32_t (*RRJNativeFrameCall)(RRJMemory *m, uint32_t target, RRJNativeCallFrame *frame, uint32_t arguments[8]);

uint32_t sub_80047430(uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call);
uint32_t sub_800473F0(uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call);
uint32_t sub_800472B4(uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8]);
uint32_t sub_80047020(uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call);
uint32_t sub_80046DD4(uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call);
uint32_t sub_80046844(uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call);
uint32_t sub_8004687C(uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call);
uint32_t sub_80046A00(RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call);
uint32_t sub_800467F4(RRJNativeCallFrame *frame, uint32_t arguments[8]);
uint32_t sub_80046948(uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call);
uint32_t sub_80046A30(uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call);
uint32_t sub_80047364(uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call);
uint32_t sub_80047248(uint32_t runtime_gp, RRJNativeCallFrame *frame, uint32_t arguments[8]);
uint32_t sub_8001578C(RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call);
uint32_t sub_80015860(RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call);
uint32_t sub_80045BEC(RRJNativeCallFrame *frame, uint32_t arguments[8], RRJNativeFrameCall call);

#endif
