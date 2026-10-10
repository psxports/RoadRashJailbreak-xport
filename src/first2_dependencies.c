#include "psx.h"
#include "draft_quick.h"

/* Unverified saved direct dependency for the first2 cluster */

static void draft02_shift_actor(RRJMemory *m, uint32_t actor, uint32_t delta)
{
    rrj_draft_8003D7C4(actor + 172, delta);
    rrj_draft_8003D7A8(actor, delta);
}

uint32_t rrj_draft_8003D39C(uint32_t a1, uint32_t a2)
{
    FUNCTION_MARKER(0x8003D39Cu, "SLUS_010.53");
    uint32_t delta = a1 - a2, actor = 0x800CD898u, count = 0;
    uint32_t list, result, item;
    int32_t remaining;
    while ((int32_t)count < (int32_t)rrj_read32(rrj_read32(0x8005B2F8u) + 48))
    {
        if (rrj_read32(actor + 328) == a2)
            draft02_shift_actor(rrj_host_context(), actor, delta);
        ++count;
        actor += 1132;
    }
    list = 0x800CE4D0u;
    remaining = (int32_t)rrj_read32(rrj_read32(list + 12));
    item = rrj_read32(list);
    while (remaining >= 0)
    {
        uint32_t linked;
        if (r_s16(item + 320) && rrj_read32(item + 328) == a2)
        {
            draft02_shift_actor(rrj_host_context(), item, delta);
            linked = rrj_read32(item + 856);
            if (linked && rrj_read32(item + 1088))
                draft02_shift_actor(rrj_host_context(), linked, delta);
        }
        linked = rrj_read32(item + 852);
        if (rrj_read32(linked + 604) - 3 < 2 && r_s16(linked + 320) && rrj_read32(linked + 328) == a2)
            draft02_shift_actor(rrj_host_context(), linked, delta);
        linked = rrj_read32(item + 856);
        if (linked && rrj_read32(item + 1088) && (r_u8(rrj_read32(item + 852) + 572) & 16))
        {
            linked = rrj_read32(linked + 852);
            if (rrj_read32(linked + 604) - 3 < 2 && r_s16(linked + 320) && rrj_read32(linked + 328) == a2)
                draft02_shift_actor(rrj_host_context(), linked, delta);
        }
        --remaining;
        item += rrj_read32(list + 4);
    }
    for (list = 0x800CE4F0u; list <= 0x800CE510u; list += 16)
    {
        remaining = (int32_t)rrj_read32(rrj_read32(list + 12));
        item = rrj_read32(list);
        while (remaining >= 0)
        {
            if (r_u16(item + 172) && rrj_read32(item + 328) == a2)
                draft02_shift_actor(rrj_host_context(), item, delta);
            --remaining;
            item += rrj_read32(list + 4);
        }
    }
    list = 0x800CE520u;
    remaining = (int32_t)rrj_read32(rrj_read32(list + 12));
    item = rrj_read32(list);
    while (remaining >= 0)
    {
        if (r_u16(item + 172) && rrj_read32(item + 328) == a2)
            rrj_draft_8003D7C4(item + 172, delta);
        --remaining;
        item += rrj_read32(list + 4);
    }
    list = 0x800CE530u;
    result = rrj_read32(list + 12);
    remaining = (int32_t)rrj_read32(result);
    item = rrj_read32(list);
    while (remaining >= 0)
    {
        if (r_u16(item) && rrj_read32(item + 156) == a2)
            rrj_draft_8003D7C4(item, delta);
        result = rrj_read32(list + 4);
        --remaining;
        item += result;
    }
    return result;
}

uint32_t rrj_draft_8003D7A8(uint32_t a1, uint32_t a2)
{
    FUNCTION_MARKER(0x8003D7A8u, "SLUS_010.53");
    uint32_t result = rrj_read32(a1 + 492);
    if (result)
        result += a2;
    rrj_write32(a1 + 492, result);
    return result;
}

uint32_t rrj_draft_8003D7C4(uint32_t a1, uint32_t a2)
{
    FUNCTION_MARKER(0x8003D7C4u, "SLUS_010.53");
    uint32_t optional = rrj_read32(a1 + 200);
    uint32_t first = rrj_read32(a1 + 156);
    uint32_t second = rrj_read32(a1 + 160);
    uint32_t third, fourth, result;
    rrj_write32(a1 + 156, first + a2);
    third = rrj_read32(a1 + 164);
    rrj_write32(a1 + 160, second + a2);
    fourth = rrj_read32(a1 + 168);
    rrj_write32(a1 + 164, third + a2);
    rrj_write32(a1 + 168, fourth + a2);
    if (optional)
        optional += a2;
    second = rrj_read32(a1 + 204);
    rrj_write32(a1 + 200, optional);
    if (second)
        second += a2;
    third = rrj_read32(a1 + 208);
    rrj_write32(a1 + 204, second);
    if (third)
        third += a2;
    result = rrj_read32(a1 + 212);
    rrj_write32(a1 + 208, third);
    if (result)
        result += a2;
    rrj_write32(a1 + 212, result);
    return result;
}
