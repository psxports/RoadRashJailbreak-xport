#ifndef RRJ_NATIVE_FILE_CALLBACKS_H
#define RRJ_NATIVE_FILE_CALLBACKS_H
#include <stdint.h>

typedef uint32_t (*RRJ_CD_CALLBACK)(uint32_t event, uint32_t response);
typedef uint32_t (*RRJFileFunction)(void);
typedef uint32_t (*RRJReadCompleteFunction)(uint32_t first, uint32_t mode, uint32_t token);
typedef uint32_t (*RRJTransferCompleteFunction)(uint32_t token);
RRJFileFunction rrj_file_function(uint32_t address);
RRJReadCompleteFunction rrj_read_complete_function(uint32_t address);
RRJTransferCompleteFunction rrj_transfer_complete_function(uint32_t address);
RRJ_CD_CALLBACK rrj_cd_callback(uint32_t address);
#endif
