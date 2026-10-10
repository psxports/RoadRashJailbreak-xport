#ifndef RRJ_AUDIO_H
#define RRJ_AUDIO_H
#include "psx_memory.h"
uint32_t sub_8001E86C(uint32_t bank, uint32_t index);
uint32_t sub_8001EB28(uint32_t mask);
uint32_t sub_8001EB44(uint32_t mask);
uint32_t sub_8001F9C4(uint32_t reserved);
uint32_t sub_8001F174(uint32_t bank, uint32_t sample, uint32_t loop, uint32_t reserved, const void *parameters);
uint32_t sub_F_8007EAC0(uint32_t event);
#endif
