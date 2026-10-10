#ifndef RRJ_VIDEO_PREVIEW_H
#define RRJ_VIDEO_PREVIEW_H
#include "video_phase.h"
typedef uint32_t (*RRJStreamRead)(RRJMemory *, uint32_t *length);
typedef void (*RRJStreamHeaderRead)(RRJMemory *, uint8_t header[12]);
typedef uint32_t (*RRJStreamControl)(RRJMemory *, uint32_t command, const uint8_t *parameter, uint8_t *result);
uint32_t sub_F_800602EC(void);
uint32_t sub_F_80060EC4(void);
uint32_t sub_F_8005F36C(const uint32_t arguments[9], RRJVideoPhaseCall, RRJStreamRead);
uint32_t sub_F_800611F4(uint32_t event, RRJVideoPhaseCall, RRJStreamHeaderRead);
uint32_t sub_F_80061060(uint32_t command, uint32_t parameter, uint32_t result, RRJVideoPhaseCall);
uint32_t sub_F_80061180(uint32_t buffer, uint32_t sectors, uint32_t position, RRJVideoPhaseCall);
uint32_t sub_F_80061584(uint32_t event, RRJVideoPhaseCall);
uint32_t sub_F_80060F44(uint32_t command, uint32_t parameter, uint32_t result);
uint32_t sub_F_80061148(uint32_t buffer, uint32_t sectors, uint32_t position);
uint32_t sub_F_800611B8(uint32_t source_position, uint32_t destination_position, uint32_t sectors);
uint32_t sub_F_80061818(void);
uint32_t sub_F_80061730(void);
/* Metadata is the original 24-byte local region at entry SP minus 0x38 */
uint32_t sub_F_80062218(uint32_t context, uint32_t callback, uint32_t handle, uint32_t loop, uint32_t metadata, RRJVideoPhaseCall);
typedef uint32_t (*RRJStreamBytes)(RRJMemory *, uint32_t target, uint32_t argument, const uint8_t *bytes, uint32_t size);
uint32_t sub_F_800620D4(uint32_t enabled, RRJVideoPhaseCall, RRJStreamBytes);
uint32_t sub_F_80061C44(uint32_t length, RRJVideoPhaseCall, RRJStreamBytes);
void sub_F_80061938(RRJVideoPhaseCall);
uint32_t sub_F_800609E4(RRJVideoPhaseCall, RRJStreamBytes);
uint32_t sub_F_8005F484(uint32_t x, uint32_t y, uint32_t channel, uint32_t mode, uint32_t frame, RRJStreamRead, RRJVideoPhaseCall);
uint32_t sub_F_80061F04(uint32_t *length, RRJVideoPhaseCall);
uint32_t sub_F_800602D4(void);
uint32_t sub_F_8006FEF4(uint32_t, RRJVideoPhaseCall, RRJVideoPhaseOpen, RRJSDKCall);
uint32_t sub_F_80070018(uint32_t, RRJVideoPhaseCall, RRJVideoPhaseOpen, RRJSDKCall);
#endif
