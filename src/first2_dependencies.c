#include "psx.h"
#include "draft_quick.h"

/* Unverified saved direct dependency for the first2 cluster */

static void draft02_shift_actor(uint32_t actor, uint32_t delta)
{
    sub_8003D7C4(actor + 172, delta);
    sub_8003D7A8(actor, delta);
}

uint32_t sub_8003D39C(uint32_t a1, uint32_t a2)
{
    FUNCTION_MARKER(0x8003D39Cu, "SLUS_010.53");
    uint32_t delta = a1 - a2, actor = 0x800CD898u, count = 0;
    uint32_t list, result, item;
    int32_t remaining;
    while ((int32_t)count < (int32_t)r_u32(r_u32(0x8005B2F8u) + 48))
    {
        if (r_u32(actor + 328) == a2)
            draft02_shift_actor(actor, delta);
        ++count;
        actor += 1132;
    }
    list = 0x800CE4D0u;
    remaining = (int32_t)r_u32(r_u32(list + 12));
    item = r_u32(list);
    while (remaining >= 0)
    {
        uint32_t linked;
        if (r_s16(item + 320) && r_u32(item + 328) == a2)
        {
            draft02_shift_actor(item, delta);
            linked = r_u32(item + 856);
            if (linked && r_u32(item + 1088))
                draft02_shift_actor(linked, delta);
        }
        linked = r_u32(item + 852);
        if (r_u32(linked + 604) - 3 < 2 && r_s16(linked + 320) && r_u32(linked + 328) == a2)
            draft02_shift_actor(linked, delta);
        linked = r_u32(item + 856);
        if (linked && r_u32(item + 1088) && (r_u8(r_u32(item + 852) + 572) & 16))
        {
            linked = r_u32(linked + 852);
            if (r_u32(linked + 604) - 3 < 2 && r_s16(linked + 320) && r_u32(linked + 328) == a2)
                draft02_shift_actor(linked, delta);
        }
        --remaining;
        item += r_u32(list + 4);
    }
    for (list = 0x800CE4F0u; list <= 0x800CE510u; list += 16)
    {
        remaining = (int32_t)r_u32(r_u32(list + 12));
        item = r_u32(list);
        while (remaining >= 0)
        {
            if (r_u16(item + 172) && r_u32(item + 328) == a2)
                draft02_shift_actor(item, delta);
            --remaining;
            item += r_u32(list + 4);
        }
    }
    list = 0x800CE520u;
    remaining = (int32_t)r_u32(r_u32(list + 12));
    item = r_u32(list);
    while (remaining >= 0)
    {
        if (r_u16(item + 172) && r_u32(item + 328) == a2)
            sub_8003D7C4(item + 172, delta);
        --remaining;
        item += r_u32(list + 4);
    }
    list = 0x800CE530u;
    result = r_u32(list + 12);
    remaining = (int32_t)r_u32(result);
    item = r_u32(list);
    while (remaining >= 0)
    {
        if (r_u16(item) && r_u32(item + 156) == a2)
            sub_8003D7C4(item, delta);
        result = r_u32(list + 4);
        --remaining;
        item += result;
    }
    return result;
}

uint32_t sub_8003D7A8(uint32_t a1, uint32_t a2)
{
    FUNCTION_MARKER(0x8003D7A8u, "SLUS_010.53");
    uint32_t result = r_u32(a1 + 492);
    if (result)
        result += a2;
    w_u32(a1 + 492, result);
    return result;
}

uint32_t sub_8003D7C4(uint32_t a1, uint32_t a2)
{
    FUNCTION_MARKER(0x8003D7C4u, "SLUS_010.53");
    uint32_t optional = r_u32(a1 + 200);
    uint32_t first = r_u32(a1 + 156);
    uint32_t second = r_u32(a1 + 160);
    uint32_t third, fourth, result;
    w_u32(a1 + 156, first + a2);
    third = r_u32(a1 + 164);
    w_u32(a1 + 160, second + a2);
    fourth = r_u32(a1 + 168);
    w_u32(a1 + 164, third + a2);
    w_u32(a1 + 168, fourth + a2);
    if (optional)
        optional += a2;
    second = r_u32(a1 + 204);
    w_u32(a1 + 200, optional);
    if (second)
        second += a2;
    third = r_u32(a1 + 208);
    w_u32(a1 + 204, second);
    if (third)
        third += a2;
    result = r_u32(a1 + 212);
    w_u32(a1 + 208, third);
    if (result)
        result += a2;
    w_u32(a1 + 212, result);
    return result;
}
