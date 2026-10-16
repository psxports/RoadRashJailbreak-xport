#ifndef RRJ_DRAFT_QUICK_H
#define RRJ_DRAFT_QUICK_H
#include "psx_memory.h"
#include <stdint.h>

#ifndef NDEBUG
typedef int (*RRJ_SDK_PRINTF)(const char *format, ...);
RRJ_SDK_PRINTF rrj_sdk_printf_function(uint32_t address);
#endif

/* Unverified drafts with a shared ordinary C interface */
uint32_t sub_80013F8C(void);
uint32_t sub_80014000(uint32_t a1);
void sub_80014264(void);
uint32_t sub_800144E8(uint32_t a1);
uint32_t sub_80014508(uint32_t a1);
uint32_t sub_80014550(const char *path, uint32_t device);
uint32_t sub_800145EC(const char *a1, uint32_t a2);
uint32_t sub_80014910(uint32_t a1, uint32_t a2);
uint32_t sub_80014F30(uint32_t a1, uint32_t a2, uint32_t a3);
uint32_t sub_80015198(void);
uint32_t sub_800153F0(uint32_t a1, uint32_t a2, uint32_t a3);
uint32_t sub_800154FC(void);
uint32_t sub_80017B6C(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
uint32_t sub_80017CF0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4);
uint32_t sub_8001BBC0(void);
uint32_t sub_8001C178(void);
uint32_t sub_8001C298(void);
uint32_t sub_8001E928(void);
uint32_t sub_80022F40(void);
void sub_800245B4(void);
uint32_t sub_80027028(uint32_t a1, uint32_t a2);
uint32_t sub_80027540(uint32_t a1, uint32_t a2, uint32_t a3);
uint32_t sub_8002D298(void);
uint32_t sub_8002DF14(uint32_t a1[2]);
uint32_t sub_8002DFC0(const uint32_t a1[2], const uint32_t a2[2]);
uint32_t sub_8002E010(const uint32_t a1[2]);
uint32_t sub_8002E388(uint32_t vector[3]);
uint64_t sub_8002EF48(uint32_t a1, uint32_t a2, uint32_t a3);
uint32_t *sub_8002F4D8(uint32_t first, uint32_t second, uint32_t third, uint32_t interval, uint32_t next_interval, uint32_t *constant, uint32_t *previous, uint32_t *diagonal, uint32_t *following);
uint32_t sub_8002F634(uint32_t intervals, uint32_t samples, uint32_t output, int32_t count);
uint32_t sub_8002F994(int32_t phase, uint32_t *first, uint32_t *second, uint32_t *third, uint32_t *fourth);
uint32_t sub_80031BC8(uint32_t a1, uint32_t a2);
uint32_t sub_80032938(uint32_t a1, uint32_t a2);

uint32_t sub_80038550(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t output);
uint32_t sub_8003BA4C(uint32_t owner, uint32_t id, uint32_t output, uint32_t weights, uint32_t limit);
uint32_t sub_8003CB6C(uint32_t a1, uint32_t a2);
uint32_t sub_8003D39C(uint32_t a1, uint32_t a2);
uint32_t sub_8003D7A8(uint32_t a1, uint32_t a2);
uint32_t sub_8003D7C4(uint32_t a1, uint32_t a2);
uint32_t sub_8003D844(uint32_t a1, uint32_t a2);
uint32_t sub_8003E338(uint32_t a1);
int32_t sub_8003FDF0(int16_t a1, int16_t a2, int16_t a3, uint32_t a4);
void sub_80040444(void);
uint32_t sub_80040714(uint32_t port, int32_t index, uint32_t field);
uint32_t sub_800407E8(uint32_t port, int32_t index, int32_t field);
uint32_t sub_800408C8(uint32_t port, uint32_t mode, uint32_t value);
uint32_t sub_80040964(uint32_t a1);
uint32_t sub_80040D80(void);
uint32_t sub_80044E94(uint32_t first_buffer, uint32_t first_size, uint32_t second_buffer, uint32_t second_size);
uint32_t sub_80044EA4(void);
uint32_t sub_80044EB4(void);
uint32_t sub_80044EC4(uint32_t type, uint32_t button_destination);
uint32_t sub_80045004(void);
void sub_80045234(void);
uint32_t sub_80045BCC(const uint8_t attenuation[4]);
uint32_t sub_80045C0C(uint32_t a0, uint32_t a1);
uint32_t sub_80045C74(void);
uint32_t sub_800461D4(void);
uint32_t sub_80046E84(uint32_t mode, uint32_t destination);
uint32_t sub_80047654(uint32_t a1, uint32_t a2);
uint32_t sub_800479F8(uint32_t channel, uint32_t callback);
uint32_t sub_80048118(uint32_t channel, uint32_t callback);
uint32_t sub_80047A58(void);
uint32_t sub_80047F58(void);
uint32_t sub_80048BE4(uint32_t ordering_table, uint32_t count);
uint32_t sub_8004951C(void);
uint32_t sub_80049C60(void);
uint32_t sub_8004B228(uint32_t ordering_table);
uint32_t sub_8004B324(void);
uint32_t sub_8004AD28(void);
uint32_t sub_80047AB0(uint32_t mask);
uint32_t sub_80048340(uint32_t channel, uint32_t callback);
uint32_t sub_80047994(uint32_t channel, uint32_t callback);
uint32_t sub_8004A4BC(uint32_t ordering_table);
uint32_t sub_8004EC54(uint32_t source, uint32_t transfer_address, uint32_t blocks);

#endif
