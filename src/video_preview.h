#ifndef RRJ_VIDEO_PREVIEW_H
#define RRJ_VIDEO_PREVIEW_H
#include "video_phase.h"
typedef uint32_t (*RRJStreamControl)(RRJMemory *, uint32_t command, const uint8_t *parameter, uint8_t *result);
uint32_t sub_800602EC(void);
uint32_t sub_80060EC4(void);
uint32_t sub_8005F36C(uint32_t handle, uint32_t mode, uint32_t channel, uint32_t x, uint32_t y, uint32_t chunk_size, uint32_t preload_width, uint32_t preload_height, uint32_t loop);
uint32_t sub_800611F4(uint32_t event);
uint32_t sub_80061060(uint32_t command, uint32_t parameter, uint32_t result);
uint32_t sub_80061180(uint32_t buffer, uint32_t sectors, uint32_t position);
uint32_t sub_80061584(uint32_t event);
uint32_t sub_80060F44(uint32_t command, uint32_t parameter, uint32_t result);
uint32_t sub_80061148(uint32_t buffer, uint32_t sectors, uint32_t position);
uint32_t sub_800611B8(uint32_t source_position, uint32_t destination_position, uint32_t sectors);
uint32_t sub_80061818(void);
uint32_t sub_80061730(void);
uint32_t sub_80062218(uint32_t context, uint32_t callback, uint32_t handle, uint32_t loop);
uint32_t sub_800620D4(uint32_t enabled);
uint32_t sub_80061C44(uint32_t length);
void sub_80061938(void);
uint32_t sub_800609E4(void);
uint32_t sub_8005F484(uint32_t x, uint32_t y, uint32_t channel, uint32_t mode, uint32_t frame);
uint32_t sub_80061F04(uint32_t *length);
uint32_t sub_800602D4(void);
uint32_t sub_8006FEF4(uint32_t desc);
uint32_t sub_80070018(uint32_t desc);
uint32_t sub_800602BC(uint32_t value);
void sub_80060020(uint32_t voice, uint32_t address, uint32_t pitch);
uint32_t sub_80060D14(uint32_t context, uint32_t first_voice, uint32_t first_address, uint32_t second_voice, uint32_t second_address, uint32_t pitch, uint32_t enabled);
uint32_t sub_8005FBDC(uint32_t source, uint32_t position, uint32_t channel);
uint32_t sub_8005FCE8(void);
uint32_t sub_80060304(uint32_t data);
void sub_8006066C(uint32_t destination, uint32_t source, uint32_t bytes);
uint32_t sub_80060780(uint32_t data);
uint32_t sub_8005FD78(uint32_t bytes);
uint32_t sub_8005FDEC(uint32_t bytes);
uint32_t sub_8005FB44(uint32_t position);
uint32_t sub_8005FE60(void);
void sub_8006013C(void);
#endif
