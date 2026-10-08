/* Hand translation from orig/images/SLUS_010.53/pseudocode/*.c,
 * audited against the corresponding .lst and original four-byte MIPS words.
 * NFS4 rmult has a +0x8000 rounding step which is ABSENT in this game.
 */
#include "fixed_math.h"
#include "race_pause.h"
#include "xport.h"

int64_t sub_8001FC90(int32_t a, int32_t b)
{
    const int64_t product = (int64_t)a * b;
    /* MULT, MFHI/MFLO, SRL 16, SLL 16, OR; JR delay: SRA v1,hi,16.
     * Explicit floor avoids an implementation-defined signed right shift. */
    return product / 65536 - (product < 0 && product % 65536 != 0);
}

uint32_t sub_8001FEB4(RRJMemory *m, int32_t angle)
{
    uint32_t index = (uint32_t)angle & 4095u;
    int32_t numerator = (int16_t)rrj_u16(rrj_at(m, 0x8005624Cu + 4u * index, 2)) * 16;
    int32_t denominator = (int16_t)rrj_u16(rrj_at(m, 0x8005624Eu + 4u * index, 2)) * 16;
    uint32_t numerator_magnitude = numerator > 0 ? (uint32_t)numerator : 0u - (uint32_t)numerator;
    uint32_t denominator_magnitude = denominator > 0 ? (uint32_t)denominator : 0u - (uint32_t)denominator;
    uint32_t result;

    FUNCTION_MARKER(0x8001FEB4, "SLUS_010.53");
    result = sub_80010028(numerator_magnitude, denominator_magnitude);
    if ((numerator > 0) != (denominator > 0))
        result = 0u - result;
    return result;
}

uint32_t sub_8002EA20(RRJMemory *m, uint32_t left, uint32_t right, int32_t scale, uint32_t output)
{
    uint32_t i;
    uint32_t result = 0;

    FUNCTION_MARKER(0x8002EA20, "SLUS_010.53");
    for (i = 0; i < 3; ++i)
    {
        int32_t left_value = (int16_t)rrj_u16(rrj_at(m, left + 2u * i, 2)) * 16;
        int32_t right_value = (int16_t)rrj_u16(rrj_at(m, right + 2u * i, 2)) * 16;

        result = (uint32_t)sub_8001FC90(right_value, scale) + (uint32_t)left_value;
        rrj_write32(m, output + 4u * i, result);
    }
    return result;
}

uint32_t rrj_quaternion_to_matrix_values(RRJMemory *m, uint32_t matrix, const int32_t quaternion[4])
{
    int32_t x = quaternion[0];
    int32_t y = quaternion[1];
    int32_t z = quaternion[2];
    int32_t w = quaternion[3];
    uint32_t norm = (uint32_t)sub_8001FC90(x, x) + (uint32_t)sub_8001FC90(y, y) + (uint32_t)sub_8001FC90(z, z) + (uint32_t)sub_8001FC90(w, w);
    uint32_t scale = (uint32_t)(((uint64_t)0x20000 << 16) / norm);
    int32_t sx = (int32_t)sub_8001FC90(x, (int32_t)scale);
    int32_t sy = (int32_t)sub_8001FC90(y, (int32_t)scale);
    int32_t sz = (int32_t)sub_8001FC90(z, (int32_t)scale);
    int32_t wx = (int32_t)sub_8001FC90(w, sx);
    int32_t yz = (int32_t)sub_8001FC90(y, sz);
    int32_t xx = (int32_t)sub_8001FC90(x, sx);
    int32_t yy = (int32_t)sub_8001FC90(y, sy);
    int32_t zz = (int32_t)sub_8001FC90(z, sz);
    int32_t wz = (int32_t)sub_8001FC90(w, sz);
    int32_t xy = (int32_t)sub_8001FC90(x, sy);
    int32_t wy = (int32_t)sub_8001FC90(w, sy);
    int32_t xz = (int32_t)sub_8001FC90(x, sz);

    rrj_put16(rrj_at(m, matrix + 14, 2), (uint16_t)((wx + yz) >> 4));
    rrj_put16(rrj_at(m, matrix + 10, 2), (uint16_t)((yz - wx) >> 4));
    rrj_put16(rrj_at(m, matrix, 2), (uint16_t)((0x10000 - (yy + zz)) >> 4));
    rrj_put16(rrj_at(m, matrix + 8, 2), (uint16_t)((0x10000 - (xx + zz)) >> 4));
    rrj_put16(rrj_at(m, matrix + 16, 2), (uint16_t)((0x10000 - (xx + yy)) >> 4));
    rrj_put16(rrj_at(m, matrix + 2, 2), (uint16_t)((xy - wz) >> 4));
    rrj_put16(rrj_at(m, matrix + 6, 2), (uint16_t)((xy + wz) >> 4));
    rrj_put16(rrj_at(m, matrix + 4, 2), (uint16_t)((xz + wy) >> 4));
    rrj_put16(rrj_at(m, matrix + 12, 2), (uint16_t)((xz - wy) >> 4));
    return (uint32_t)wy;
}

uint32_t sub_8001005C(RRJMemory *m, uint32_t matrix, uint32_t quaternion)
{
    int32_t values[4];
    uint32_t i;

    FUNCTION_MARKER(0x8001005C, "SLUS_010.53");
    for (i = 0; i < 4; ++i)
        values[i] = rrj_s32(rrj_read32(m, quaternion + 4u * i));
    return rrj_quaternion_to_matrix_values(m, matrix, values);
}

int64_t sub_8002E874(const void *left, const void *right, void *out)
{
    const uint8_t *a = (const uint8_t *)left, *b = (const uint8_t *)right;
    uint8_t *r = (uint8_t *)out;
    uint32_t first;
    int64_t last;
    first = (uint32_t)sub_8001FC90(rrj_s32(rrj_u32(a + 4)), rrj_s32(rrj_u32(b + 8)));
    last = sub_8001FC90(rrj_s32(rrj_u32(a + 8)), rrj_s32(rrj_u32(b + 4)));
    rrj_put32(r, first - (uint32_t)last); /* 8002E8B8: before the next input loads */
    first = (uint32_t)sub_8001FC90(rrj_s32(rrj_u32(a + 8)), rrj_s32(rrj_u32(b)));
    last = sub_8001FC90(rrj_s32(rrj_u32(a)), rrj_s32(rrj_u32(b + 8)));
    rrj_put32(r + 4, first - (uint32_t)last); /* 8002E8E0 */
    first = (uint32_t)sub_8001FC90(rrj_s32(rrj_u32(a)), rrj_s32(rrj_u32(b + 4)));
    last = sub_8001FC90(rrj_s32(rrj_u32(a + 4)), rrj_s32(rrj_u32(b)));
    rrj_put32(r + 8, first - (uint32_t)last); /* SUBU wraps at 32 bits */
    return last;                              /* Preserve observed v0/v1, without claiming a meaningful source return. */
}

int64_t sub_8002E928(const void *vector, const void *matrix, void *out)
{
    const uint8_t *v = (const uint8_t *)vector, *m = (const uint8_t *)matrix;
    uint8_t *r = (uint8_t *)out;
    uint32_t first, second;
    int64_t last = 0;
    unsigned column;
    for (column = 0; column != 3; ++column)
    {
        first = (uint32_t)sub_8001FC90(rrj_s32(rrj_u32(v)), rrj_s32(rrj_u32(m + column * 4)));
        second = (uint32_t)sub_8001FC90(rrj_s32(rrj_u32(v + 4)), rrj_s32(rrj_u32(m + 12 + column * 4)));
        last = sub_8001FC90(rrj_s32(rrj_u32(v + 8)), rrj_s32(rrj_u32(m + 24 + column * 4)));
        /* Original unrolled stores: 8002E984, 8002E9C0, 8002E9FC.
         * Do not use NFS4's temporary result vector: that changes overlap. */
        rrj_put32(r + column * 4, first + second + (uint32_t)last);
    }
    return last;
}
