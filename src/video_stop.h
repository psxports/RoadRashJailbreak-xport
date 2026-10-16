#ifndef RRJ_VIDEO_STOP_H
#define RRJ_VIDEO_STOP_H
#include "psx_memory.h"
/* Shutdown decoder and close CD file are explicit host boundaries. */
uint32_t sub_8006FE6C(void);
uint32_t sub_8006FED4(void);
uint32_t sub_80060BD4(void);
uint32_t sub_80060B68(void);
uint32_t sub_80060C40(void);
uint32_t sub_8006239C(void);
uint32_t sub_8005F7E0(uint32_t channel);
uint32_t sub_8005F9D8(void);
uint32_t sub_8005F81C(void);
uint32_t sub_8005FABC(uint32_t enabled);

#endif
