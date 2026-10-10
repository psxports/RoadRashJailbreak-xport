/* UNVERIFIED DRAFT Ordinary C derived from complete IDA and MIPS */
#include "draft_quick.h"

static uint8_t q03_read8(RRJMemory *m, uint32_t p)
{
    return *(uint8_t *)rrj_at(p, 1);
}

static uint16_t q03_read16(RRJMemory *m, uint32_t p)
{
    uint8_t *b = (uint8_t *)rrj_at(p, 2);
    return (uint16_t)(b[0] | ((uint16_t)b[1] << 8));
}

static void q03_write8(RRJMemory *m, uint32_t p, uint8_t v)
{
    *(uint8_t *)rrj_at(p, 1) = v;
}

static void q03_write16(RRJMemory *m, uint32_t p, uint16_t v)
{
    uint8_t *b = (uint8_t *)rrj_at(p, 2);
    b[0] = (uint8_t)v;
    b[1] = (uint8_t)(v >> 8);
}

static uint32_t q03_sra(uint32_t v, uint32_t n)
{
    n &= 31;
    if (!n)
        return v;
    return (v >> n) | ((v & 0x80000000u) ? (~0u << (32 - n)) : 0);
}

static uint32_t q03_call1(RRJMemory *m, uint32_t target, uint32_t a)
{
    return rrj_draft_call(m, target, &a, 1);
}

static uint32_t q03_call2(RRJMemory *m, uint32_t target, uint32_t a, uint32_t b)
{
    uint32_t args[2] = {a, b};
    return rrj_draft_call(m, target, args, 2);
}

static uint32_t q03_mul(RRJMemory *m, uint32_t a, uint32_t b)
{
    return q03_call2(m, 0x8001FC90u, a, b);
}

uint32_t rrj_draft_80014000(uint32_t a1)
{
    uint32_t result = q03_read16(rrj_host_context(), a1 + 172);
    if (result)
    {
        uint32_t p = rrj_read32(a1 + 276);
        q03_write16(rrj_host_context(), a1 + 172, 0);
        if ((int8_t)q03_read8(rrj_host_context(), p) < 0)
            q03_write8(rrj_host_context(), p, 0);
        result = rrj_read32(0x8005B314u) - 1;
        rrj_write32(0x8005B314u, result);
    }
    return result;
}

uint32_t rrj_draft_80014550(void)
{
    uint32_t result = rrj_draft_call(rrj_host_context(), 0x8001458Cu, 0, 0);
    if ((int32_t)result < 0)
        return (uint32_t)-5;
    q03_call1(rrj_host_context(), 0x8001460Cu, result);
    return 0;
}

uint32_t rrj_draft_800144E8(uint32_t a1)
{
    return q03_call2(rrj_host_context(), 0x8001447Cu, a1, 1);
}

uint64_t rrj_draft_80015B04(void)
{
    uint32_t count = 0, low, high;
    low = (uint32_t)(1499999 < (int32_t)count);
    ++count;
    if (low)
        return ((uint64_t)count << 32) | low;
    do
    {
        low = count + 1;
        high = (uint32_t)(1499999 < (int32_t)count);
        count = low;
    } while (!high);
    return ((uint64_t)high << 32) | low;
}

uint32_t rrj_draft_80017B6C(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4)
{
    uint32_t result = 72u * a4 + rrj_read32(0x8005B40Cu) + 4u * (a2 == 0x6D);
    rrj_write32(result + 48, a2);
    rrj_write32(result + 56, a1);
    rrj_write32(result + 64, a3);
    return result;
}

uint32_t rrj_draft_80031BC8(uint32_t a1, uint32_t a2)
{
    uint32_t args[3] = {rrj_read32(a1) >> 28, a1 + 4, a2};
    return rrj_draft_call(rrj_host_context(), 0x80023900u, args, 3) == 0;
}

uint32_t rrj_draft_80032938(uint32_t a1, uint32_t a2)
{
    uint32_t args[3] = {(((a1 | 0x80000000u) >> 13) & 0x7C00u) | 0x8000u | (a1 & 0x3FFu), 1, a2};
    rrj_draft_call(rrj_host_context(), 0x80034D38u, args, 3);
    return 1;
}

uint32_t rrj_draft_8003D7A8(uint32_t a1, uint32_t a2)
{
    uint32_t result = rrj_read32(a1 + 492);
    if (result)
        result += a2;
    rrj_write32(a1 + 492, result);
    return result;
}

uint32_t rrj_draft_80040444(uint32_t incoming_t0, uint32_t incoming_v0)
{
    /* The immutable callback count is zero on both entry paths */
    return incoming_v0;
}

uint32_t rrj_draft_80044E94(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
    uint32_t args[4] = {a0, a1, a2, a3};
    /* TODO Bind B0 service 12 with a fail-fast BIOS adapter */
    return rrj_draft_bios(rrj_host_context(), 0xB0, 0x12, args, 4);
}

uint32_t rrj_draft_80044EC4(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
    uint32_t args[4] = {a0, a1, a2, a3};
    /* TODO Bind B0 service 15 with a fail-fast BIOS adapter */
    return rrj_draft_bios(rrj_host_context(), 0xB0, 0x15, args, 4);
}

uint32_t rrj_draft_80045C0C(uint32_t a0, uint32_t a1)
{
    return rrj_draft_80047654(a0, a1) == 0;
}

uint32_t rrj_draft_80047A00(uint32_t incoming_v0, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
    uint32_t args[4] = {a0, a1, a2, a3};
    return rrj_draft_indirect(rrj_host_context(), rrj_read32(incoming_v0 + 20), args, 4);
}

uint32_t rrj_draft_800407F0(uint32_t a1, int32_t a2, int32_t a3, uint32_t incoming_v0)
{
    uint32_t args[3] = {a1, (uint32_t)a2, (uint32_t)a3};
    uint32_t owner = rrj_draft_indirect(rrj_host_context(), incoming_v0, args, 3), entry;
    if (a2 < 0)
        return q03_read8(rrj_host_context(), owner + 234);
    if (a2 >= q03_read8(rrj_host_context(), owner + 234))
        return 0;
    entry = rrj_read32(owner + 8) + 8u * (uint32_t)a2;
    if (a3 < 0)
        return q03_read8(rrj_host_context(), entry);
    if (a3 < q03_read8(rrj_host_context(), entry))
        return q03_read8(rrj_host_context(), rrj_read32(entry + 4) + (uint32_t)a3);
    return 0;
}

uint32_t rrj_draft_80048BEC(uint32_t a1, uint32_t a2, uint32_t incoming_v0)
{
    uint32_t p = a1, remaining = a2;
    if (incoming_v0 >= 2)
    {
        uint32_t args[3] = {0x80011218u, a1, a2};
        rrj_draft_indirect(rrj_host_context(), rrj_read32(0x80055F68u), args, 3);
    }
    --remaining;
    while (remaining)
    {
        --remaining;
        q03_write8(rrj_host_context(), p + 3, 0);
        rrj_write32(p, (rrj_read32(p) & 0xFF000000u) | ((p + 4) & 0xFFFFFFu));
        p += 4;
    }
    rrj_write32(0x8005602Cu, (0x80056018u & 0xFFFFFFu) | 0x4000000u);
    rrj_write32(p, 0x8005602Cu & 0xFFFFFFu);
    return p;
}

uint32_t rrj_draft_8001C298(void)
{
    uint32_t owner = rrj_read32(0x8005B470u), index = 0, result;
    result = q03_read8(rrj_host_context(), owner + 244);
    if (!result)
        return result;
    do
    {
        uint32_t p = rrj_read32(owner + 248 + index * 4);
        if (p)
            q03_call1(rrj_host_context(), 0x800144B8u, p);
        owner = rrj_read32(0x8005B470u);
        result = q03_read8(rrj_host_context(), owner + 244);
        ++index;
        result = (uint32_t)((int32_t)index < (int32_t)result);
        /* The branch delay slot overwrites the comparison result */
        if (!result)
            return index * 4;
    } while (1);
}

uint32_t rrj_draft_80035E60(uint32_t a1, uint32_t a2)
{
    uint32_t count = 0, triple = a2 * 3, index = triple * 4, end = index + 12;
    uint32_t entry = 0x800D87E8u + triple * 448;
    if ((int32_t)index < (int32_t)end)
    {
        do
        {
            if (rrj_read32(entry) != 0xFFFFFFFFu && rrj_read32(entry + 8) != 0xFFFFFFFFu && rrj_read32(entry + 4))
            {
                if (q03_call1(rrj_host_context(), 0x800363F0u, entry) & 255)
                {
                    rrj_write32(a1, index);
                    a1 += 4;
                    ++count;
                }
            }
            ++index;
            entry += 112;
        } while ((int32_t)index < (int32_t)end);
    }
    return count;
}

uint64_t rrj_draft_8002EF48(uint32_t a1, uint32_t a2, uint32_t a3)
{
    uint32_t local = rrj_draft_local_alloc(rrj_host_context(), 36), i;
    uint32_t args[3] = {a1, local, a3};
    uint64_t result;
    /* TODO Supply temporary guest-layout allocation during integration */
    for (i = 0; i < 9; ++i)
        rrj_write32(local + 4 * i, (uint32_t)(int32_t)(int16_t)q03_read16(rrj_host_context(), a2 + 2 * i) << 4);
    result = rrj_draft_call64(rrj_host_context(), 0x8002E928u, args, 3);
    rrj_draft_local_free(rrj_host_context(), local);
    return result;
}

uint32_t rrj_draft_8001BBC0(void)
{
    uint32_t local = rrj_draft_local_alloc(rrj_host_context(), 36), result, image;
    uint32_t args[3] = {0x80010AC8u, local + 32, local + 8};
    rrj_write32(local + 32, 0);
    result = rrj_draft_call(rrj_host_context(), 0x8001408Cu, args, 3);
    if (!result)
    {
        image = rrj_read32(local + 24);
        q03_write16(rrj_host_context(), local, 122);
        q03_write16(rrj_host_context(), local + 2, 200);
        q03_write16(rrj_host_context(), local + 4, 140);
        q03_write16(rrj_host_context(), local + 6, 20);
        q03_call2(rrj_host_context(), 0x80048A6Cu, local, image);
        image = rrj_read32(local + 24);
        q03_write16(rrj_host_context(), local + 2, 456);
        q03_call2(rrj_host_context(), 0x80048A6Cu, local, image);
    }
    result = q03_call1(rrj_host_context(), 0x800487C0u, 0);
    image = rrj_read32(local + 32);
    if (image)
        result = q03_call1(rrj_host_context(), 0x800144B8u, image);
    rrj_draft_local_free(rrj_host_context(), local);
    return result;
}

uint32_t rrj_draft_8003E338(uint32_t a1)
{
    uint32_t owner, p, local, candidate, left, right, i;
    int32_t found = -1, count;
    if (!a1 || !q03_read16(rrj_host_context(), a1 + 320))
        return 0;
    owner = rrj_read32(a1 + 328);
    if (!owner || !rrj_read32(owner + 104))
        return 0;
    p = q03_call1(rrj_host_context(), 0x80039AFCu, rrj_read32(owner));
    if (!p)
        return 0;
    local = rrj_draft_local_alloc(rrj_host_context(), 8);
    {
        uint32_t args[5] = {a1 + 184, p, local, local + 4, 0xFFFFFFFFu};
        found = (int32_t)rrj_draft_call(rrj_host_context(), 0x8003EB58u, args, 5);
    }
    if (found < 0)
    {
        rrj_draft_local_free(rrj_host_context(), local);
        return 0;
    }
    p = rrj_read32(rrj_read32(a1 + 328) + 104);
    count = (int16_t)q03_read16(rrj_host_context(), p + 2);
    if (count <= 0)
    {
        rrj_draft_local_free(rrj_host_context(), local);
        return 0;
    }
    left = rrj_read32(local);
    right = rrj_read32(local + 4);
    candidate = p + 4;
    left = (uint32_t)(int32_t)(int16_t)q03_read16(rrj_host_context(), left + 4);
    for (i = 0; (int32_t)i < count; ++i, candidate += 24)
    {
        uint32_t first = (uint32_t)(int32_t)(int16_t)q03_read16(rrj_host_context(), candidate);
        if (first == left && (int16_t)q03_read16(rrj_host_context(), candidate + 2) == (int16_t)q03_read16(rrj_host_context(), right + 4))
        {
            rrj_draft_local_free(rrj_host_context(), local);
            return candidate;
        }
        if ((int16_t)q03_read16(rrj_host_context(), candidate + 2) == (int32_t)left && first == (uint32_t)(int32_t)(int16_t)q03_read16(rrj_host_context(), right + 4))
        {
            rrj_draft_local_free(rrj_host_context(), local);
            return candidate;
        }
    }
    rrj_draft_local_free(rrj_host_context(), local);
    return 0;
}

uint32_t rrj_draft_8002E388(uint32_t a1)
{
    uint32_t x = rrj_read32(a1), y = rrj_read32(a1 + 4), z = rrj_read32(a1 + 8);
    uint32_t result, leading, table, entry, scale;
    int32_t shift;
    /* TODO Bind the exact GTE command and data register adapters */
    rrj_draft_gte_write(rrj_host_context(), 9, x);
    rrj_draft_gte_write(rrj_host_context(), 10, y);
    rrj_draft_gte_write(rrj_host_context(), 11, z);
    rrj_draft_gte_command(rrj_host_context(), 0xA00428u);
    result = rrj_draft_gte_read(rrj_host_context(), 25);
    result += rrj_draft_gte_read(rrj_host_context(), 26);
    result += rrj_draft_gte_read(rrj_host_context(), 27);
    /* TODO MIPS ADD overflow traps require an explicit exception policy */
    rrj_draft_gte_write(rrj_host_context(), 30, result);
    leading = rrj_draft_gte_read(rrj_host_context(), 31);
    table = rrj_read32(0x8005B560u);
    shift = (int32_t)(22u - (leading & 0xFFFFFFFEu));
    if (shift <= 0)
        shift = 0;
    entry = q03_read16(rrj_host_context(), table + (q03_sra(result, (uint32_t)shift) << 1));
    scale = q03_sra((entry >> 5) << (entry & 31), (uint32_t)shift >> 1);
    rrj_write32(a1, q03_sra(x * scale, 12));
    rrj_write32(a1 + 4, q03_sra(y * scale, 12));
    rrj_write32(a1 + 8, q03_sra(z * scale, 12));
    return result;
}

int32_t rrj_draft_8003FDF0(int16_t a1, int16_t a2, int16_t a3, uint32_t a4)
{
    uint32_t first = (uint32_t)(int32_t)(int16_t)q03_read16(rrj_host_context(), 0x8005624Cu + 4u * ((uint16_t)a1 & 4095)) << 4;
    uint32_t third = (uint32_t)(int32_t)(int16_t)q03_read16(rrj_host_context(), 0x8005624Cu + 4u * ((uint16_t)a3 & 4095)) << 4;
    uint32_t first_c = (uint32_t)(int32_t)(int16_t)q03_read16(rrj_host_context(), 0x8005624Eu + 4u * ((uint16_t)a1 & 4095)) << 4;
    uint32_t second = (uint32_t)(int32_t)(int16_t)q03_read16(rrj_host_context(), 0x8005624Cu + 4u * ((uint16_t)a2 & 4095)) << 4;
    uint32_t second_c = (uint32_t)(int32_t)(int16_t)q03_read16(rrj_host_context(), 0x8005624Eu + 4u * ((uint16_t)a2 & 4095)) << 4;
    uint32_t third_c = (uint32_t)(int32_t)(int16_t)q03_read16(rrj_host_context(), 0x8005624Eu + 4u * ((uint16_t)a3 & 4095)) << 4;
    uint32_t combined = q03_mul(rrj_host_context(), third, first), combined_c = q03_mul(rrj_host_context(), first, third_c);
    uint32_t elements[9], t, result, i;
    t = q03_mul(rrj_host_context(), third_c, second_c);
    elements[0] = t + q03_mul(rrj_host_context(), combined, second);
    elements[1] = q03_mul(rrj_host_context(), third, first_c);
    t = q03_mul(rrj_host_context(), third_c, second);
    elements[2] = q03_mul(rrj_host_context(), combined, second_c) - t;
    t = q03_mul(rrj_host_context(), third, second_c);
    elements[3] = q03_mul(rrj_host_context(), combined_c, second) - t;
    elements[4] = q03_mul(rrj_host_context(), third_c, first_c);
    t = q03_mul(rrj_host_context(), third, second);
    elements[5] = t + q03_mul(rrj_host_context(), combined_c, second_c);
    elements[6] = q03_mul(rrj_host_context(), first_c, second);
    elements[7] = 0u - first;
    result = q03_mul(rrj_host_context(), first_c, second_c);
    for (i = 0; i < 7; ++i)
        q03_write16(rrj_host_context(), a4 + i * 2, (uint16_t)q03_sra(elements[i], 4));
    result = q03_sra(result, 4);
    q03_write16(rrj_host_context(), a4 + 16, (uint16_t)result);
    q03_write16(rrj_host_context(), a4 + 14, (uint16_t)q03_sra(elements[7], 4));
    return (int32_t)result;
}

uint32_t rrj_draft_800461DC(uint32_t incoming_v0)
{
    uint32_t result, start;
    if (incoming_v0 & 1)
        q03_call1(rrj_host_context(), 0x80045C50u, 0);
    rrj_write32(0x80054AA8u, 0);
    result = 1;
    if (rrj_read32(0x80054AB8u))
    {
        start = q03_call1(rrj_host_context(), 0x80047724u, 0xFFFFFFFFu);
        if (!rrj_read32(0x80054AB8u))
            result = 0;
        else
        {
            do
            {
                result = q03_call1(rrj_host_context(), 0x80047724u, 0xFFFFFFFFu) - start;
                if (result >= 121)
                {
                    q03_call1(rrj_host_context(), 0x800457E8u, rrj_read32(0x80054ABCu));
                    result = 0xFFFFFFFFu;
                    rrj_write32(0x80054AB8u, 0);
                    break;
                }
                result = rrj_read32(0x80054AB8u);
            } while (result);
        }
    }
    if (!result)
        return result;
    q03_call1(rrj_host_context(), 0x800457FCu, rrj_read32(0x80054AC0u));
    if (rrj_read32(0x80054A08u) & 1)
        q03_call1(rrj_host_context(), 0x80045C2Cu, rrj_read32(0x80054AC4u));
    q03_call1(rrj_host_context(), 0x800457E8u, 0x80045C74u);
    q03_call2(rrj_host_context(), 0x8004594Cu, 9, 0);
    rrj_write32(0x80054AB8u, 1);
    result = rrj_read32(0x80054AB8u);
    if (!result)
        return result;
    start = q03_call1(rrj_host_context(), 0x80047724u, 0xFFFFFFFFu);
    result = start;
    if (!rrj_read32(0x80054AB8u))
        return result;
    do
    {
        result = (q03_call1(rrj_host_context(), 0x80047724u, 0xFFFFFFFFu) - start) < 121;
        if (!result)
        {
            result = q03_call1(rrj_host_context(), 0x800457E8u, rrj_read32(0x80054ABCu));
            rrj_write32(0x80054AB8u, 0);
            return result;
        }
        result = rrj_read32(0x80054AB8u);
    } while (result);
    return result;
}

uint32_t rrj_draft_8002F634(uint32_t a1, uint32_t a2, uint32_t a3, int32_t a4)
{
    uint32_t local = rrj_draft_local_alloc(rrj_host_context(), 152);
    uint32_t value = 0, index = 1, offset = 0, next = 8;
    uint32_t right = a2 + 4, left = a1 + 4, output = a3 + 4;
    uint32_t coef = local + 44, constant = local + 84;
    uint32_t result, i;
    rrj_write32(local + 44, 0x10000);
    rrj_write32(local + 84, 0);
    rrj_write32(local + 80, 0);
    rrj_write32(local + 40, 0);
    if (a4 > 1)
    {
        do
        {
            uint32_t args[9], slope, intercept, factor, mixed;
            args[4] = rrj_read32(left);
            args[5] = local + 120;
            args[6] = local + 124;
            args[7] = local + 128;
            args[8] = local + 132;
            args[0] = rrj_read32(right - 4);
            args[1] = rrj_read32(right);
            args[2] = rrj_read32(right + 4);
            args[3] = rrj_read32(left - 4);
            rrj_draft_8002F4D8(args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8]);
            slope = q03_mul(rrj_host_context(), rrj_read32(local + 124), rrj_read32(local + 40 + offset));
            slope += q03_mul(rrj_host_context(), rrj_read32(local + 128), rrj_read32(coef));
            intercept = q03_mul(rrj_host_context(), rrj_read32(local + 124), rrj_read32(local + 80 + offset));
            mixed = q03_mul(rrj_host_context(), rrj_read32(local + 128), rrj_read32(constant));
            intercept = rrj_read32(local + 120) + intercept + mixed;
            if (index == (uint32_t)a4 - 1)
            {
                mixed = intercept + q03_mul(rrj_host_context(), rrj_read32(local + 132), rrj_read32(output + 4));
                if ((int32_t)mixed > 0)
                {
                    if ((int32_t)slope > 0)
                    {
                        mixed = intercept + q03_mul(rrj_host_context(), rrj_read32(local + 132), rrj_read32(output + 4));
                        value = 0u - q03_call2(rrj_host_context(), 0x80010028u, mixed, slope);
                    }
                    else
                    {
                        mixed = intercept + q03_mul(rrj_host_context(), rrj_read32(local + 132), rrj_read32(output + 4));
                        value = q03_call2(rrj_host_context(), 0x80010028u, mixed, 0u - slope);
                    }
                }
                else
                {
                    if ((int32_t)slope > 0)
                    {
                        mixed = intercept + q03_mul(rrj_host_context(), rrj_read32(local + 132), rrj_read32(output + 4));
                        value = q03_call2(rrj_host_context(), 0x80010028u, 0u - mixed, slope);
                    }
                    else
                    {
                        mixed = intercept + q03_mul(rrj_host_context(), rrj_read32(local + 132), rrj_read32(output + 4));
                        value = 0u - q03_call2(rrj_host_context(), 0x80010028u, 0u - mixed, 0u - slope);
                    }
                }
                next += 4;
            }
            else
            {
                uint32_t denominator;
                factor = rrj_read32(local + 132);
                if ((int32_t)factor < 0)
                {
                    factor = 0u - factor;
                    denominator = q03_sra(factor, 1) + q03_sra(factor - 2, 31);
                    factor = denominator ? 0x80000000u / denominator : 0xFFFFFFFFu;
                    factor = 0u - factor;
                }
                else
                {
                    denominator = q03_sra(factor, 1) + q03_sra(factor - 2, 31);
                    factor = denominator ? 0x80000000u / denominator : 0xFFFFFFFFu;
                }
                rrj_write32(local + 132, factor);
                mixed = 0u - q03_mul(rrj_host_context(), slope, factor);
                rrj_write32(local + 40 + next, mixed);
                mixed = 0u - q03_mul(rrj_host_context(), intercept, rrj_read32(local + 132));
                rrj_write32(local + 80 + next, mixed);
                next += 4;
            }
            output += 4;
            offset += 4;
            right += 4;
            constant += 4;
            coef += 4;
            ++index;
            left += 4;
        } while ((int32_t)index < a4);
    }
    rrj_write32(a3 + 4, value);
    result = a4 > 2;
    for (i = 2; (int32_t)i < a4; ++i)
    {
        uint32_t item = q03_mul(rrj_host_context(), rrj_read32(local + 40 + i * 4), value);
        item += rrj_read32(local + 80 + i * 4);
        rrj_write32(a3 + i * 4, item);
        result = (int32_t)(i + 1) < a4;
    }
    /* TODO Local array bounds follow the original caller contract */
    rrj_draft_local_free(rrj_host_context(), local);
    return result;
}
