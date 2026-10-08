#ifndef RRJ_RESOURCE_H
#define RRJ_RESOURCE_H
#include "psx_memory.h"
/* LoadImage boundary: copied little-endian RECT and original data address.
 * Required for actual uploads; a missing backend is an error, not success. */
typedef uint32_t (*RRJImageUpload)(RRJMemory *, const uint8_t rect[8], uint32_t pixels);
typedef uint32_t (*RRJQueueCall)(RRJMemory *, uint32_t target, uint32_t a0, uint32_t a1);
void sub_F_80062774(RRJMemory *, uint32_t queue, uint32_t count, RRJQueueCall);
void sub_F_800627BC(RRJMemory *, uint32_t queue, uint32_t count, RRJQueueCall);
void sub_F_800623F8(RRJMemory *);
void sub_F_80062420(RRJMemory *);
uint32_t sub_F_80078BD8(RRJMemory *, uint32_t list, uint32_t wait, RRJQueueCall);
uint32_t sub_F_80078C68(RRJMemory *, uint32_t id, uint32_t force, RRJQueueCall);
uint32_t sub_F_80078E80(RRJMemory *, uint32_t list, RRJQueueCall);
uint32_t sub_F_80065768(RRJMemory *, uint32_t descriptor, RRJImageUpload upload);
uint32_t sub_F_8007A400(RRJMemory *, uint32_t descriptor, uint32_t id, RRJImageUpload upload);
uint32_t sub_F_800705DC(RRJMemory *, uint32_t sprite, uint32_t link, uint32_t command, RRJImageUpload upload);
uint32_t sub_F_80070580(RRJMemory *, uint32_t menu, uint32_t entry, RRJImageUpload upload);
#endif
