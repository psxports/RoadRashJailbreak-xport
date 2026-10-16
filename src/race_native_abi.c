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
    decoded.frame.return_value = xport_load_le32(prefix + 24);
    decoded.frame.secondary_result = xport_load_le32(prefix + 28);
    decoded.arguments[0] = xport_load_le32(prefix + 32);
    decoded.arguments[1] = xport_load_le32(prefix + 36);
    decoded.arguments[2] = xport_load_le32(prefix + 40);
    decoded.arguments[3] = xport_load_le32(prefix + 44);
    decoded.frame.preserved_s0 = xport_load_le32(prefix + 80);
    decoded.frame.preserved_s1 = xport_load_le32(prefix + 84);
    decoded.frame.preserved_s2 = xport_load_le32(prefix + 88);
    decoded.frame.preserved_s3 = xport_load_le32(prefix + 92);
    decoded.frame.preserved_s4 = xport_load_le32(prefix + 96);
    decoded.preserved_s5 = xport_load_le32(prefix + 100);
    decoded.preserved_s6 = xport_load_le32(prefix + 104);
    decoded.preserved_s7 = xport_load_le32(prefix + 108);
    decoded.global_pointer = xport_load_le32(prefix + 128);
    decoded.frame.stack_pointer = xport_load_le32(prefix + 132);
    decoded.frame_pointer = xport_load_le32(prefix + 136);
    decoded.frame.return_address = xport_load_le32(prefix + 140);
    decoded.multiply_high = xport_load_le32(prefix + 144);
    decoded.multiply_low = xport_load_le32(prefix + 148);
    decoded.checkpoint_pc = xport_load_le32(prefix + 152);
    decoded.checkpoint_npc = xport_load_le32(prefix + 156);
    decoded.cpu_status = xport_load_le32(prefix + 192);
    memcpy(decoded.pipeline_flags, prefix + 216, sizeof(decoded.pipeline_flags));
    *abi = decoded;
    return 1;
}
