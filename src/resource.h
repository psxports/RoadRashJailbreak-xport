#ifndef RRJ_RESOURCE_H
#define RRJ_RESOURCE_H
#include "psx_memory.h"
void sub_80062774(uint32_t queue, uint32_t count);
void sub_800627BC(uint32_t queue, uint32_t count);
void sub_800623F8(void);
void sub_80062420(void);
uint32_t sub_80062674(void);
uint32_t sub_800624EC(uint32_t state, uint32_t first, uint32_t second, uint32_t encoded, uint32_t decoded, uint32_t enabled);
uint32_t sub_80062440(void);
uint32_t sub_80078BD8(uint32_t list, uint32_t wait);
uint32_t sub_80078C68(uint32_t id, uint32_t force);
uint32_t sub_80078E80(uint32_t list);
uint32_t sub_80065768(uint32_t descriptor);
uint32_t sub_8007A400(uint32_t descriptor, uint32_t id);
uint32_t sub_800705DC(uint32_t sprite, uint32_t link, uint32_t command);
uint32_t sub_80070580(uint32_t menu, uint32_t entry);
void rrj_resource_decoder_output(void);
void live_decoder_restore(void);
#endif
