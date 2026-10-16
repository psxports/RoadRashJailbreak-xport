#ifndef RRJ_SDK_BUILD_STUBS_H
#define RRJ_SDK_BUILD_STUBS_H
#include "psx.h"

/* Existing shared SDK entry */
uint32 _SpuInit(uint32 mode);
uint32 _bu_init(void);

/* Missing project SDK adapters */
uint32 rrj_missing_spu_read(uint32 destination, uint32 bytes);
uint32_t sub_80050B18(uint32_t enabled);
uint32_t sub_80050C58(uint32_t address);
uint32 rrj_missing_pad_init_mtap(uint32 first, uint32 second);

void *sub_800494E8(void *environment);

void rrj_missing_bios_b0_19(uint32_t context);
void rrj_missing_bios_b0_4a(uint32_t mode);
void rrj_missing_bios_b0_4b(void);
uint32_t rrj_tim_open(uint32_t data);
uint8_t *rrj_tim_read(uint8_t output[20]);
uint32_t rrj_missing_spu_start(void);
uint32_t rrj_missing_pc_seek(uint32_t handle, uint32_t offset, uint32_t mode);
uint32_t rrj_spu_irq_callback_psx(uint32_t callback);
void rrj_spu_completion_service(void);
uint32_t rrj_cd_read_callback_psx(uint32_t callback);
void rrj_missing_init_card(uint32_t mode);
void rrj_missing_card_auto(uint32_t mode);
void rrj_missing_start_card(void);
uint32_t rrj_missing_stop_card(void);
uint32_t rrj_file_buffer_address(const void *buffer);
uint32_t rrj_missing_bios_b0_56(void);
void rrj_missing_spu_fw1ts(void);
void rrj_missing_spu_dma_priority(void);
void rrj_cd_data_callback_psx(uint32_t callback);

uint32_t rrj_cd_irq_callback_psx(void);

uint32_t rrj_missing_sys_deq_int_rp(uint32_t priority, uint32_t node);
uint32_t rrj_missing_sys_enq_int_rp(uint32_t priority, uint32_t node);

uint32_t sub_80044E94(uint32_t first_buffer, uint32_t first_size, uint32_t second_buffer, uint32_t second_size);

uint32_t sub_80044EA4(void);

uint32_t sub_80044EB4(void);

uint32_t sub_80044EC4(uint32_t type, uint32_t button_destination);

uint32_t sub_80045004(void);

uint32_t rrj_missing_bios_b0_12(uint32_t first_buffer, uint32_t first_size, uint32_t second_buffer, uint32_t second_size);
uint32_t rrj_missing_bios_b0_13(void);
uint32_t rrj_missing_bios_b0_14(void);
uint32_t rrj_missing_bios_b0_15(uint32_t type, uint32_t button_destination);
uint32_t rrj_missing_bios_b0_4c(void);

#endif
