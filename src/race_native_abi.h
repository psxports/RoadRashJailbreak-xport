#ifndef RRJ_RACE_NATIVE_ABI_H
#define RRJ_RACE_NATIVE_ABI_H

#include <stdint.h>

typedef struct RRJNativeCallFrame
{
    uint32_t stack_pointer;
    uint32_t return_address;
    uint32_t return_value;
    uint32_t secondary_result;
    uint32_t preserved_s0;
    uint32_t preserved_s1;
    uint32_t preserved_s2;
    uint32_t preserved_s3;
    uint32_t preserved_s4;
} RRJNativeCallFrame;

typedef struct RRJNativeCheckpointABI
{
    RRJNativeCallFrame frame;
    uint32_t arguments[4];
    uint32_t global_pointer;
    uint32_t frame_pointer;
    uint32_t multiply_high;
    uint32_t multiply_low;
    uint32_t preserved_s5;
    uint32_t preserved_s6;
    uint32_t preserved_s7;
    uint32_t cpu_status;
    uint32_t checkpoint_pc;
    uint32_t checkpoint_npc;
    uint8_t pipeline_flags[6];
} RRJNativeCheckpointABI;

int rrj_native_checkpoint_decode(RRJNativeCheckpointABI *abi, const uint8_t *prefix, uint32_t size);

#endif
