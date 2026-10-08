#include "race_native_abi.h"
#include "types.h"
#include <string.h>

int rrj_native_checkpoint_decode(RRJNativeCheckpointABI *abi, const uint8_t *prefix, uint32_t size)
{
    RRJNativeCheckpointABI decoded;
    unsigned i;

    if (!abi || !prefix || size != 1260)
        return 0;
    for (i = 0; i < 6; ++i)
        if (prefix[216 + i] > 1)
            return 0;
    decoded.frame.return_value = rrj_u32(prefix + 24);
    decoded.frame.secondary_result = rrj_u32(prefix + 28);
    decoded.arguments[0] = rrj_u32(prefix + 32);
    decoded.arguments[1] = rrj_u32(prefix + 36);
    decoded.arguments[2] = rrj_u32(prefix + 40);
    decoded.arguments[3] = rrj_u32(prefix + 44);
    decoded.frame.preserved_s0 = rrj_u32(prefix + 80);
    decoded.frame.preserved_s1 = rrj_u32(prefix + 84);
    decoded.frame.preserved_s2 = rrj_u32(prefix + 88);
    decoded.frame.preserved_s3 = rrj_u32(prefix + 92);
    decoded.frame.preserved_s4 = rrj_u32(prefix + 96);
    decoded.preserved_s5 = rrj_u32(prefix + 100);
    decoded.preserved_s6 = rrj_u32(prefix + 104);
    decoded.preserved_s7 = rrj_u32(prefix + 108);
    decoded.global_pointer = rrj_u32(prefix + 128);
    decoded.frame.stack_pointer = rrj_u32(prefix + 132);
    decoded.frame_pointer = rrj_u32(prefix + 136);
    decoded.frame.return_address = rrj_u32(prefix + 140);
    decoded.multiply_high = rrj_u32(prefix + 144);
    decoded.multiply_low = rrj_u32(prefix + 148);
    decoded.checkpoint_pc = rrj_u32(prefix + 152);
    decoded.checkpoint_npc = rrj_u32(prefix + 156);
    decoded.cpu_status = rrj_u32(prefix + 192);
    memcpy(decoded.pipeline_flags, prefix + 216, sizeof(decoded.pipeline_flags));
    *abi = decoded;
    return 1;
}
