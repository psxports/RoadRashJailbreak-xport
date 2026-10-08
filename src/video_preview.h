#ifndef RRJ_VIDEO_PREVIEW_H
#define RRJ_VIDEO_PREVIEW_H
#include "video_phase.h"
typedef uint32_t (*RRJStreamRead)(RRJMemory *, uint32_t *length);
typedef void (*RRJStreamHeaderRead)(RRJMemory *, uint8_t header[12]);
uint32_t sub_F_800611F4(RRJMemory *, uint32_t event, RRJVideoPhaseCall, RRJStreamHeaderRead);
uint32_t sub_F_80061060(RRJMemory *, uint32_t command, uint32_t parameter, uint32_t result, RRJVideoPhaseCall);
uint32_t sub_F_80061180(RRJMemory *, uint32_t buffer, uint32_t sectors, uint32_t position, RRJVideoPhaseCall);
uint32_t sub_F_80061584(RRJMemory *, uint32_t event, RRJVideoPhaseCall);
uint32_t sub_F_80060F44(RRJMemory *, uint32_t command, uint32_t parameter, uint32_t result, RRJVideoPhaseCall);
uint32_t sub_F_80061148(RRJMemory *, uint32_t buffer, uint32_t sectors, uint32_t position, RRJVideoPhaseCall);
uint32_t sub_F_800611B8(RRJMemory *, uint32_t source_position, uint32_t destination_position, uint32_t sectors, RRJVideoPhaseCall);
uint32_t sub_F_80061818(RRJMemory *);
typedef uint32_t (*RRJStreamBytes)(RRJMemory *, uint32_t target, uint32_t argument, const uint8_t *bytes, uint32_t size);
uint32_t sub_F_80061C44(RRJMemory *, uint32_t length, RRJVideoPhaseCall, RRJStreamBytes);
void sub_F_80061938(RRJMemory *, RRJVideoPhaseCall, RRJStreamBytes);
uint32_t sub_F_800609E4(RRJMemory *, RRJVideoPhaseCall, RRJStreamBytes);
uint32_t sub_F_8005F484(RRJMemory *, uint32_t x, uint32_t y, uint32_t channel, uint32_t mode, uint32_t frame, RRJStreamRead, RRJVideoPhaseCall);
uint32_t sub_F_80061F04(RRJMemory *, uint32_t *length, RRJVideoPhaseCall);
uint32_t sub_F_800602D4(RRJMemory *);
uint32_t sub_F_8006FEF4(RRJMemory *, uint32_t, RRJVideoPhaseCall, RRJVideoPhaseOpen, RRJSDKCall);
uint32_t sub_F_80070018(RRJMemory *, uint32_t, RRJVideoPhaseCall, RRJVideoPhaseOpen, RRJSDKCall);
#endif
