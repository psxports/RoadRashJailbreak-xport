#ifndef RRJ_RACE_BODYLESS_BATCH_008_H
#define RRJ_RACE_BODYLESS_BATCH_008_H

#include "race_bodyless_batch_006.h"

uint32_t sub_8001A2E4(uint32_t identifier);
uint32_t sub_800386DC(uint32_t actor, uint32_t target_distance, uint32_t initial_distance, uint32_t input_direction, uint32_t input_record[8], uint32_t position_output[3], uint32_t copy_output[8]);
uint32_t sub_8003A3CC(uint32_t arguments[8]);
uint32_t sub_8002C928(uint32_t incoming_sp, uint32_t arguments[8], RRJBodylessRegistersCall call);
uint32_t sub_8002CC74(uint32_t incoming_sp, uint32_t arguments[8]);

#endif
