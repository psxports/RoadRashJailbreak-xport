#ifndef RRJ_TYPES_H
#define RRJ_TYPES_H
#include "xport.h"

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

static RRJ_FORCEINLINE uint16 rrj_u16(const void *p)
{
    const uint8 *b = (const uint8 *)p;
    return (uint16)((uint32)b[0] | ((uint32)b[1] << 8));
}

static RRJ_FORCEINLINE uint32 rrj_u32(const void *p)
{
    const uint8 *b = (const uint8 *)p;
    return b[0] | ((uint32)b[1] << 8) | ((uint32)b[2] << 16) | ((uint32)b[3] << 24);
}

static RRJ_FORCEINLINE void rrj_put16(void *p, uint32 v)
{
    uint8 *b = (uint8 *)p;
    b[0] = (uint8)v;
    b[1] = (uint8)(v >> 8);
}

static RRJ_FORCEINLINE void rrj_put32(void *p, uint32 v)
{
    uint8 *b = (uint8 *)p;
    b[0] = (uint8)v;
    b[1] = (uint8)(v >> 8);
    b[2] = (uint8)(v >> 16);
    b[3] = (uint8)(v >> 24);
}
#endif
