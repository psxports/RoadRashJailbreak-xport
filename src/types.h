#ifndef RRJ_TYPES_H
#define RRJ_TYPES_H
#include "xport_trace.h"

#ifdef _MSC_VER
    #define RRJ_FORCEINLINE __forceinline
#else
    #define RRJ_FORCEINLINE inline
#endif

/* Fixed PSX word widths; never use host long for an original long/pointer. */
static RRJ_FORCEINLINE sint32 rrj_s32(uint32 bits)
{
    return bits <= INT32_MAX ? (sint32)bits : -1 - (sint32)~bits;
}

#define rrj_u16(pointer) xport_load_le16(pointer)
#define rrj_u32(pointer) xport_load_le32(pointer)
#define rrj_put16(pointer, value) xport_store_le16(pointer, value)
#define rrj_put32(pointer, value) xport_store_le32(pointer, value)
#endif
