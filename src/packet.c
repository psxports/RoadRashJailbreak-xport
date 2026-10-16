#include "rrj_debug.h"
#include "gpu.h"
#include <stdio.h>
#include <stdlib.h>
#include "psx.h"
/* IDA draft + MIPS SLUS_010.53:80021BE8..80022018.
 * DAT names/offsets are retained. Queue field names are provisional.
 * All pointer comparisons in the allocator are unsigned, as SLTU dictates.
 */
#include "packet.h"
#define CONTEXT 0x8005B470u
#define LIMIT 0x8005B4D0u
#define END 0x8005B4DCu
#define LAST 0x8005AE00u
#define CURRENT 0x8005AE04u
#define POINTERS 0x800D75B0u
#define TIMES 0x800D75C0u
#define ACTIVE 0x800D75D0u

uint32_t sub_80021BE8(uint32_t cursor, uint32_t bytes)
{
    FUNCTION_MARKER(0x80021BE8u, "SLUS_010.53");
    uint32_t context, slot, start;
    RRJ_DEBUG_PRINTF((const char *)xport_guest_cptr(0x80010B2Cu, 64), (int)((int32_t)(uint32_t)(bytes)));
    context = r_u32(CONTEXT);
    if (!rrj_gpu_draw_ot(r_u32(context + 0x108) + r_u32(0x8005ADFC) * 4 - 4))
        abort();
    (uint32_t)DrawSync(0); /* DrawSync(0) */
    RRJ_DEBUG_PRINTF((const char *)xport_guest_cptr(0x80010B6C, 40));
    context = r_u32(CONTEXT);
    (void)rrj_gpu_clear_ot(r_u32(context + 0x108), r_u32(0x8005ADFC)); /* ClearOTagR */
    (uint32_t)DrawSync(0);
    context = r_u32(CONTEXT);
    start = r_u32(context + 0xF0) & 0xFFFFFF;
    w_u32(LIMIT, r_u32(END));
    slot = POINTERS + r_u32(LAST) * 4;
    w_u32(slot, start);
    return slot; /* residual v0 at 80021C80, not a source-level semantic result */
}

/* Both original scanning blocks start from LAST+1 on each outer iteration.
 * Do not "repair" that into CURRENT+1. No artificial retry count in game code. */
static void advance_queue(uint32_t context, uint32_t last, uint32_t next)
{
    uint32_t index = r_u32(CURRENT);
    if (r_u32(0x8005B46C) - r_u32(TIMES + index * 4) >= 61)
        w_u32(ACTIVE + index * 64, 0);
    w_u32(CURRENT, next);
    if (((int32_t)(uint32_t)(next)) >= r_u8(context + 0xF4))
        w_u32(CURRENT, 0);
    index = r_u32(CURRENT);
    while (r_u32(ACTIVE + index * 64) == 0)
    {
        if (index == last)
            break;
        ++index;
        w_u32(CURRENT, index);
        if (((int32_t)(uint32_t)(index)) >= r_u8(context + 0xF4))
            w_u32(CURRENT, 0);
        index = r_u32(CURRENT);
    }
}

uint32_t sub_80021C98(uint32_t cursor, uint32_t bytes)
{
    FUNCTION_MARKER(0x80021C98u, "SLUS_010.53");
    uint32_t context, last, next, end, candidate, request_end;
    int fits = 0, fits_end;
    end = r_u32(END);
    if (r_u32(LIMIT) == end)
        goto wrap_buffer;
    if (r_u32(CURRENT) == r_u32(LAST))
        goto final_check;
    last = r_u32(LAST);
    next = last + 1;
    context = r_u32(CONTEXT);
    request_end = cursor + bytes;
    fits_end = request_end < end;
    for (;;)
    {
        advance_queue(context, last, next); /* 80021E64..80021F18 */
        candidate = r_u32(POINTERS + r_u32(CURRENT) * 4);
        if (r_u32(LIMIT) != candidate)
        {
            if (cursor < candidate)
                w_u32(LIMIT, candidate);
            else
            {
                w_u32(LIMIT, end);
                if (!fits_end)
                    goto wrap_buffer;
                fits = 1;
                goto final_check;
            }
        }
        if (request_end < r_u32(LIMIT))
        {
            fits = 1;
            goto final_check;
        }
        if (r_u32(CURRENT) == last)
            break;
    }
final_check: /* 80021F80 */
    if (fits)
        return cursor;
    candidate = r_u32(POINTERS + r_u32(CURRENT) * 4);
    if (r_u32(LIMIT) != candidate)
    {
        if (cursor < candidate)
            w_u32(LIMIT, candidate);
        else
        {
            end = r_u32(END);
            w_u32(LIMIT, end);
            if (cursor + bytes >= end)
                goto wrap_buffer;
            fits = 1;
        }
    }
    if (cursor + bytes < r_u32(LIMIT))
        fits = 1;
    goto finish;
wrap_buffer: /* 80021CBC; keep 24-bit physical address, not KSEG0 */
    context = r_u32(CONTEXT);
    cursor = r_u32(context + 0xF0) & 0xFFFFFF;
    request_end = cursor + bytes;
    candidate = r_u32(POINTERS + r_u32(CURRENT) * 4);
    if (request_end < candidate)
    {
        w_u32(LIMIT, candidate);
        return cursor;
    }
    if (r_u32(CURRENT) != r_u32(LAST))
    {
        last = r_u32(LAST);
        next = last + 1;
        for (;;)
        {
            advance_queue(context, last, next); /* 80021D34..80021DE8 */
            candidate = r_u32(POINTERS + r_u32(CURRENT) * 4);
            if (request_end < candidate)
            {
                w_u32(LIMIT, candidate);
                fits = 1;
                break;
            }
            if (r_u32(CURRENT) == last)
                break;
        }
    }
finish: /* 80021FF8 */
    if (!fits)
        sub_80021BE8(cursor, bytes);
    return cursor;
}
