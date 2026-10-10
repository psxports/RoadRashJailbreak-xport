#include "draft_quick.h"

/* Unverified draft translations of SLUS_010.53 */
uint32_t rrj_draft_80013F8C(void)
{
    uint32_t p, result = 0x800D0000u;
    if (rrj_read32(0x8005AD24u))
    {
        int32_t half;
        p = rrj_read32(0x8005B474u);
        half = r_s16(p + 4);
        w_u16(0x800D5EECu, (uint16_t)(half / 64 + 1));
        w_u16(0x800D5EEEu, (uint16_t)(r_u16(p) + (half >> 1)));
        result = (uint32_t)((int32_t)r_s16(p + 12) >> 1);
        w_u16(0x800D5EF0u, (uint16_t)(r_u16(p + 8) + result));
    }
    return result;
}

uint32_t rrj_draft_80014508(uint32_t a1)
{
    return rrj_draft_call(rrj_host_context(), 0x800144B8u, &a1, 1);
}

uint32_t rrj_draft_8001C17C(uint32_t incoming_v0)
{
    uint32_t p = rrj_read32(incoming_v0 + 240);
    return p ? rrj_draft_call(rrj_host_context(), 0x800144B8u, &p, 1) : incoming_v0;
}

uint32_t rrj_draft_8001E928(void)
{
    rrj_write32(0x8005B4A4u, 1);
    return 1;
}

uint32_t rrj_draft_80027028(uint32_t a1, uint32_t a2)
{
    uint32_t p = 0x800537DAu + 6 * rrj_read32(a1 + 180) + 2 * a2;
    return r_s8(p) != -1;
}

uint32_t rrj_draft_8003D7C4(uint32_t a1, uint32_t a2)
{
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

uint32_t rrj_draft_800153F0(uint32_t a1, uint32_t a2, uint32_t a3)
{
    if ((a1 >> 16) == 1)
        return 1;
    return rrj_draft_8001556C(a2 + (uint32_t)((int32_t)a3 / 2048));
}

uint32_t rrj_draft_80014F30(uint32_t a1, uint32_t a2, uint32_t a3)
{
    uint32_t args[3] = {a1, a2, a3};
    uint32_t result = rrj_draft_call(rrj_host_context(), 0x80014E5Cu, args, 3);
    if (result != UINT32_MAX)
    {
        uint32_t record = 0x800D65D0u + 20 * a1;
        rrj_draft_800153F0(rrj_read32(record + 16), rrj_read32(record + 4), rrj_read32(record + 8));
    }
    return result;
}

uint32_t rrj_draft_80050EC8(uint32_t a1, uint32_t a2, uint32_t a3)
{
    uint32_t limit = a2 > 0x7EFF0u ? 0x7EFF0u : a2;
    uint32_t args[3] = {a1, limit, a3};
    rrj_draft_call(rrj_host_context(), 0x8004F000u, args, 3);
    if (!rrj_read32(0x8005A454u))
        rrj_write32(0x8005A450u, 0);
    return limit;
}

uint32_t rrj_draft_80047A60(uint32_t incoming_v0)
{
    /* TODO The indirect callee argument carriers need integration binding */
    return rrj_draft_indirect(rrj_host_context(), rrj_read32(incoming_v0 + 24), 0, 0);
}

uint32_t rrj_draft_80049524(uint32_t incoming_v0)
{
    /* TODO The indirect callee argument carriers need integration binding */
    return rrj_draft_indirect(rrj_host_context(), rrj_read32(incoming_v0 + 56), 0, 0) >> 31;
}

uint32_t rrj_draft_800408D0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t incoming_v0)
{
    uint32_t args[3] = {a1, a2, a3};
    uint32_t value = rrj_draft_indirect(rrj_host_context(), incoming_v0, args, 3);
    args[0] = value;
    args[1] = a2 & 255;
    args[2] = a3 & 255;
    return rrj_draft_call(rrj_host_context(), 0x8004213Cu, args, 3);
}

uint32_t rrj_draft_80044EB4(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
    uint32_t args[4] = {a0, a1, a2, a3};
    return rrj_draft_bios(rrj_host_context(), 0xB0, 0x14, args, 4);
}

uint32_t rrj_draft_80045004(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
    uint32_t args[4] = {a0, a1, a2, a3};
    return rrj_draft_bios(rrj_host_context(), 0xB0, 0x4C, args, 4);
}

uint32_t rrj_draft_8002F994(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t fifth_output)
{
    uint32_t args[2] = {a1, a1};
    uint32_t twice = a1 * 2;
    uint32_t value = (uint32_t)rrj_draft_call64(rrj_host_context(), 0x8001FC90u, args, 2);
    uint32_t weight = 3 * value - twice;
    uint32_t result;
    rrj_write32(fifth_output, weight);
    rrj_write32(a4, weight - twice + 0x10000u);
    result = 2 * rrj_read32(fifth_output) - twice;
    rrj_write32(a2, result);
    result = 0u - result;
    rrj_write32(a3, result);
    return result;
}

uint32_t rrj_draft_80017CF0(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
{
    uint32_t difference = rrj_read32(0x8005B41Cu) - rrj_read32(rrj_read32(0x8005B2F8u) + 12);
    uint32_t sign = (uint32_t)((int32_t)difference >> 31);
    uint32_t result = (int32_t)((difference + sign) ^ sign) < 76;
    if (!result)
    {
        uint32_t table = rrj_read32(0x8005B40Cu);
        uint32_t args[4];
        if (table)
        {
            uint32_t p = table + 72 * a1;
            uint32_t x = rrj_read32(p + 4), z = rrj_read32(p + 8);
            a3 = x + (uint32_t)((int32_t)(a3 - x) >> 2);
            a4 = z + (uint32_t)((int32_t)(a4 - z) >> 2);
        }
        args[0] = a3;
        args[1] = a4;
        args[2] = a2 + 98;
        args[3] = 0;
        rrj_draft_call(rrj_host_context(), 0x80017BA0u, args, 4);
        result = rrj_read32(rrj_read32(0x8005B2F8u) + 12);
        rrj_write32(0x8005B41Cu, result);
    }
    return result;
}

uint32_t rrj_draft_80015A18(uint32_t a1, uint32_t a2)
{
    uint32_t args[2] = {0, 0};
    uint32_t mode;
    rrj_write32(0x800D6724u, UINT32_MAX);
    rrj_write32(0x8005AD68u, rrj_read32(0x8005AD68u) + 1);
    rrj_draft_call(rrj_host_context(), 0x800457E8u, args, 1);
    if (r_u8(a2) & 16)
        rrj_draft_80015B60();
    else
        rrj_draft_call(rrj_host_context(), 0x800457A8u, args, 2);
    mode = rrj_read32(0x800D6728u);
    switch (mode)
    {
        case 0:
        case 3:
        case 4:
            args[0] = rrj_read32(0x800D6720u) - rrj_read32(0x8005B3D8u);
            rrj_write32(0x800D6728u, 0);
            return rrj_draft_8001556C(args[0]);
        case 1:
        case 2:
        case 5:
            args[0] = rrj_read32(0x800D6714u);
            args[1] = rrj_read32(0x800D671Cu);
            rrj_write32(0x800D6728u, 0);
            return rrj_draft_call(rrj_host_context(), 0x8001564Cu, args, 2);
        default:
            return 0x80010000u;
    }
}

uint32_t rrj_draft_80046E84(uint32_t a1, uint32_t a2)
{
    uint32_t args[2] = {0x8005B154u, 0};
    rrj_draft_call(rrj_host_context(), 0x80046844u, args, 1);
    do
    {
        uint32_t flag;
        if (rrj_draft_call(rrj_host_context(), 0x8004687Cu, 0, 0))
            return UINT32_MAX;
        rrj_draft_call(rrj_host_context(), 0x80046A00u, 0, 0);
        flag = r_u8(0x8005B0DAu);
        if (flag)
        {
            w_u8(0x8005B0DAu, 0);
            args[0] = a2;
            args[1] = 0x8005B5C8u;
            rrj_draft_call(rrj_host_context(), 0x800467F4u, args, 2);
            return flag;
        }
        flag = r_u8(0x8005B0D9u);
        if (flag)
        {
            w_u8(0x8005B0D9u, 0);
            args[0] = a2;
            args[1] = 0x8005B5C0u;
            rrj_draft_call(rrj_host_context(), 0x800467F4u, args, 2);
            return flag;
        }
    } while (!a1);
    return 0;
}

uint32_t rrj_draft_80047654(uint32_t a1, uint32_t a2)
{
    uint32_t p;
    w_u8(rrj_read32(0x8005B0C0u), 0);
    w_u8(rrj_read32(0x8005B0CCu), 128);
    rrj_write32(rrj_read32(0x8005B1D0u), 0x21020843u);
    rrj_write32(rrj_read32(0x8005B0D0u), 0x1325u);
    p = rrj_read32(0x8005B1D4u);
    rrj_write32(p, rrj_read32(p) | 0x8000u);
    rrj_write32(rrj_read32(0x8005B1D8u), a1);
    rrj_write32(rrj_read32(0x8005B1DCu), a2 | 0x10000u);
    p = rrj_read32(0x8005B0C0u);
    while (!(r_u8(p) & 64))
    {
    }
    rrj_write32(rrj_read32(0x8005B1E0u), 0x11400100u);
    /* Preserve the final hardware read omitted by pseudocode */
    rrj_read32(rrj_read32(0x8005B1E0u));
    return 0;
}

uint32_t rrj_draft_80022F40(void)
{
    uint32_t local = rrj_draft_local_alloc(rrj_host_context(), 8);
    uint32_t args[3] = {14, local, 0};
    uint32_t result;
    /* TODO Local allocation adapter must preserve uninitialized bytes */
    w_u8(local, 128);
    rrj_draft_call(rrj_host_context(), 0x80045810u, args, 3);
    args[0] = 3;
    result = rrj_draft_call(rrj_host_context(), 0x80047724u, args, 1);
    rrj_draft_local_free(rrj_host_context(), local);
    return result;
}

uint32_t rrj_draft_80040974(uint32_t a1, uint32_t incoming_v0, uint32_t incoming_v1)
{
    uint32_t result = (incoming_v1 << 1) | (incoming_v0 == 0);
    if (result != a1)
    {
        uint32_t arg;
        rrj_write32(0x80054934u, 0);
        if (a1 & 1)
        {
            rrj_write32(0x8005494Cu, 0);
            if ((int32_t)rrj_read32(0x800D9CC0u) >= 150)
            {
                arg = rrj_read32(0x80054930u);
                rrj_draft_indirect(rrj_host_context(), rrj_read32(0x80054900u), &arg, 1);
            }
            rrj_write32(0x800D9CC0u, 0);
        }
        else
            rrj_write32(0x8005494Cu, 1);
        if (a1 & 2)
        {
            rrj_write32(0x80054950u, 1);
            if ((int32_t)rrj_read32(0x800D9CC4u) >= 150)
            {
                arg = rrj_read32(0x80054930u) + 240;
                rrj_draft_indirect(rrj_host_context(), rrj_read32(0x80054900u), &arg, 1);
            }
            rrj_write32(0x800D9CC4u, 0);
        }
        else
            rrj_write32(0x80054950u, 0);
        rrj_write32(0x80054934u, 1);
    }
    return result;
}

uint32_t rrj_draft_80027540(uint32_t a1, uint32_t a2, uint32_t a3)
{
    uint32_t result = rrj_read32(rrj_read32(0x8005B2F8u) + 48);
    uint32_t id, p, bits, source, indices, vertex, args[2];
    if (r_u16(a1 + 172) >= result)
        return result;
    result = (uint32_t)(int32_t)r_s8(a1 + 8);
    if ((int32_t)result > 0)
        return result;
    result = (rrj_read32(a1 + 36) >> 30) < 2;
    if (!result)
        return result;
    id = rrj_draft_call(rrj_host_context(), 0x80027178u, 0, 0);
    if (id == UINT32_MAX)
        return UINT32_MAX;
    bits = rrj_read32(a1 + 36);
    rrj_write32(a1 + 36, (bits & 0x3FFFFFFFu) | (((bits >> 30) + 1) << 30));
    p = 0x800D39B0u + 112 * id;
    bits = rrj_read32(p);
    w_u8(p + 60, 0);
    bits = (bits & 0xFFFFFC3Fu) | 0x100u;
    rrj_write32(p, bits);
    bits = (bits & 0xFFC03FFFu) | ((a2 & 255) << 14);
    bits = (bits & 0xFFFFC3FFu) | 0x400u;
    rrj_write32(p, bits);
    source = rrj_read32(rrj_read32(0x8005B2F8u) + 16);
    w_u8(p + 61, 0);
    rrj_write32(p + 52, 900);
    w_u16(p + 62, 30);
    rrj_write32(p + 96, 9);
    rrj_write32(p + 36, 0);
    rrj_write32(p + 44, 0);
    rrj_write32(p + 40, 0);
    rrj_write32(p + 48, source);
    source = rrj_read32(rrj_read32(0x8005B2F8u) + 16);
    w_u8(p + 108, (uint8_t)a3);
    w_u8(p + 109, 4);
    rrj_write32(p + 100, source);
    for (uint32_t i = 0; i < 4; ++i)
        w_u8(p + 104 + i, (uint8_t)(i + 1));
    source = 0x800537DAu + 6 * rrj_read32(a1 + 180) + ((rrj_read32(p) >> 13) & 0x1FEu);
    indices = 0x800536F0u + 4 * (uint32_t)(int32_t)r_s8(source + 1);
    vertex = rrj_read32(rrj_read32(24 * (uint32_t)r_u16(rrj_read32(a1) + 24) + rrj_read32(a1 + 4) - 24) + 20) + 20 * (uint32_t)(int32_t)r_s8(source) + 4;
    w_u16(p + 76, r_u16(vertex + 2 * (uint32_t)(int32_t)r_s8(indices + 2) + 12));
    w_u16(p + 78, r_u16(vertex + 2 * (uint32_t)(int32_t)r_s8(indices + 3) + 12));
    args[0] = a1;
    args[1] = id;
    return rrj_draft_call(rrj_host_context(), 0x800271CCu, args, 2);
}

uint32_t rrj_draft_80038550(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t output)
{
    uint32_t track = rrj_read32(a4 + 12);
    uint32_t local, args[5], result, delta, quotient, clamped;
    int32_t marker = r_s16(track);
    if (!marker || marker == (int32_t)r_s16(rrj_read32(a4 + 8) + 10) - 1)
    {
        args[0] = a4;
        args[1] = 1;
        if (rrj_draft_call(rrj_host_context(), 0x800394F0u, args, 2))
            return 0;
    }
    local = rrj_draft_local_alloc(rrj_host_context(), 0xA8);
    /* TODO Bind local guest buffers without inventing initialized bytes */
    rrj_write32(local + 0x54, 0);
    args[0] = local + 0x18;
    args[1] = a4;
    args[2] = 32;
    rrj_draft_call(rrj_host_context(), 0x8001E0B4u, args, 3);
    args[0] = a1 + 172;
    args[1] = local + 0x18;
    args[2] = local + 0x38;
    args[3] = local + 0x98;
    args[4] = 3;
    result = rrj_draft_call(rrj_host_context(), 0x80037A30u, args, 5);
    if (!result)
    {
        rrj_draft_local_free(rrj_host_context(), local);
        return 0;
    }
    args[0] = (uint32_t)(int32_t)r_s16(track + 36);
    result = rrj_draft_call(rrj_host_context(), 0x8001FEB4u, args, 1);
    args[0] = a3;
    args[1] = result;
    result = (uint32_t)rrj_draft_call64(rrj_host_context(), 0x8001FC90u, args, 2);
    delta = rrj_read32(track + 32) - result;
    args[0] = (int32_t)a2 <= 0 ? 0u - a2 : a2;
    args[1] = (int32_t)delta <= 0 ? 0u - delta : delta;
    quotient = rrj_draft_call(rrj_host_context(), 0x80010028u, args, 2);
    if (((int32_t)a2 > 0) != ((int32_t)delta > 0))
        quotient = 0u - quotient;
    clamped = (~(uint32_t)((int32_t)quotient >> 31) & quotient) + ((uint32_t)((int32_t)(0x10000u - quotient) >> 31) & (0x10000u - quotient));
    args[0] = track + 8;
    args[1] = rrj_read32(local + 0x44) + 8;
    args[2] = output;
    args[3] = 0x10000u - clamped;
    args[4] = clamped;
    rrj_draft_call64(rrj_host_context(), 0x8002EB78u, args, 5);
    args[0] = output;
    rrj_draft_call(rrj_host_context(), 0x8002E468u, args, 1);
    rrj_draft_local_free(rrj_host_context(), local);
    return 1;
}

static void draft02_shift_actor(RRJMemory *m, uint32_t actor, uint32_t delta)
{
    rrj_draft_8003D7C4(actor + 172, delta);
    rrj_draft_8003D7A8(actor, delta);
}

uint32_t rrj_draft_8003D39C(uint32_t a1, uint32_t a2)
{
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
