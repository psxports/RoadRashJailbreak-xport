/* Hand translation from orig/images/SLUS_010.53/pseudocode/*.c,
 * audited against the corresponding .lst and original four-byte MIPS words.
 * NFS4 rmult has a +0x8000 rounding step which is ABSENT in this game.
 */
#include "fixed_math.h"
#include "race_pause.h"
#include "xport.h"

int64_t sub_8001FC90(int32_t a, int32_t b)
{
    FUNCTION_MARKER(0x8001FC90u, "SLUS_010.53");
    const int64_t product = (int64_t)a * b;
    /* MULT, MFHI/MFLO, SRL 16, SLL 16, OR; JR delay: SRA v1,hi,16.
     * Explicit floor avoids an implementation-defined signed right shift. */
    return product / 65536 - (product < 0 && product % 65536 != 0);
}

uint32_t sub_8001FEB4(int32_t angle)
{
    uint32_t index = (uint32_t)angle & 4095u;
    int32_t numerator = (int16_t)r_u16(0x8005624Cu + 4u * index) * 16;
    int32_t denominator = (int16_t)r_u16(0x8005624Eu + 4u * index) * 16;
    uint32_t numerator_magnitude = numerator > 0 ? (uint32_t)numerator : 0u - (uint32_t)numerator;
    uint32_t denominator_magnitude = denominator > 0 ? (uint32_t)denominator : 0u - (uint32_t)denominator;
    uint32_t result;

    FUNCTION_MARKER(0x8001FEB4, "SLUS_010.53");
    result = sub_80010028(numerator_magnitude, denominator_magnitude);
    if ((numerator > 0) != (denominator > 0))
        result = 0u - result;
    return result;
}

uint32_t sub_8002EA20(const uint16_t left[3], const uint16_t right[3], int32_t scale, uint32_t output[3])
{
    uint32_t i;
    uint32_t result = 0;

    FUNCTION_MARKER(0x8002EA20, "SLUS_010.53");
    for (i = 0; i < 3; ++i)
    {
        int32_t right_value = (int16_t)xport_load_le16((const uint8_t *)right + 2u * i) * 16;
        int64_t product = (int64_t)right_value * scale;
        int32_t left_value = (int16_t)xport_load_le16((const uint8_t *)left + 2u * i) * 16;
        int64_t scaled = product / 65536 - (product < 0 && product % 65536 != 0);

        result = (uint32_t)scaled + (uint32_t)left_value;
        xport_store_le32((uint8_t *)output + 4u * i, result);
    }
    return result;
}

uint32_t sub_8001005C(uint16_t matrix[9], const int32_t quaternion[4])
{
    uint32_t x, y, z, w, norm, quotient, remainder, scale, sx, sy, sz;
    uint32_t wx, yz, xx, yy, zz, wz, xy, wy, xz, value;
    FUNCTION_MARKER(0x8001005C, "SLUS_010.53");
    x = xport_load_le32(quaternion);
    y = xport_load_le32(quaternion + 1);
    z = xport_load_le32(quaternion + 2);
    w = xport_load_le32(quaternion + 3);
    norm = (uint32_t)(((int64_t)(int32_t)x * (int32_t)x) >> 16);
    norm += (uint32_t)(((int64_t)(int32_t)y * (int32_t)y) >> 16);
    norm += (uint32_t)(((int64_t)(int32_t)z * (int32_t)z) >> 16);
    norm += (uint32_t)(((int64_t)(int32_t)w * (int32_t)w) >> 16);
    quotient = norm ? 0x20000 / norm : 0xFFFFFFFF;
    remainder = norm ? 0x20000 % norm : 0x20000;
    value = remainder << 16;
    scale = (quotient << 16) | ((norm ? value / norm : 0xFFFFFFFF) & 0xFFFF);
    sx = (uint32_t)(((int64_t)(int32_t)x * (int32_t)scale) >> 16);
    sy = (uint32_t)(((int64_t)(int32_t)y * (int32_t)scale) >> 16);
    sz = (uint32_t)(((int64_t)(int32_t)z * (int32_t)scale) >> 16);
    wx = (uint32_t)(((int64_t)(int32_t)w * (int32_t)sx) >> 16);
    yz = (uint32_t)(((int64_t)(int32_t)y * (int32_t)sz) >> 16);
    xx = (uint32_t)(((int64_t)(int32_t)x * (int32_t)sx) >> 16);
    value = wx + yz;
    xport_store_le16(matrix + 7, (uint16_t)((int32_t)value >> 4));
    value = yz - wx;
    xport_store_le16(matrix + 5, (uint16_t)((int32_t)value >> 4));
    yy = (uint32_t)(((int64_t)(int32_t)y * (int32_t)sy) >> 16);
    zz = (uint32_t)(((int64_t)(int32_t)z * (int32_t)sz) >> 16);
    value = 65536 - (yy + zz);
    xport_store_le16(matrix, (uint16_t)((int32_t)value >> 4));
    value = 65536 - (xx + zz);
    xport_store_le16(matrix + 4, (uint16_t)((int32_t)value >> 4));
    value = 65536 - (xx + yy);
    xport_store_le16(matrix + 8, (uint16_t)((int32_t)value >> 4));
    wz = (uint32_t)(((int64_t)(int32_t)w * (int32_t)sz) >> 16);
    xy = (uint32_t)(((int64_t)(int32_t)x * (int32_t)sy) >> 16);
    wy = (uint32_t)(((int64_t)(int32_t)w * (int32_t)sy) >> 16);
    xz = (uint32_t)(((int64_t)(int32_t)x * (int32_t)sz) >> 16);
    value = xy - wz;
    xport_store_le16(matrix + 1, (uint16_t)((int32_t)value >> 4));
    value = xy + wz;
    xport_store_le16(matrix + 3, (uint16_t)((int32_t)value >> 4));
    value = xz + wy;
    xport_store_le16(matrix + 2, (uint16_t)((int32_t)value >> 4));
    value = xz - wy;
    xport_store_le16(matrix + 6, (uint16_t)((int32_t)value >> 4));
    return wy;
}

int64_t sub_8002E874(const void *left, const void *right, void *out)
{
    FUNCTION_MARKER(0x8002E874u, "SLUS_010.53");
    const uint8_t *a = (const uint8_t *)left, *b = (const uint8_t *)right;
    uint8_t *r = (uint8_t *)out;
    uint32_t first;
    int64_t last;
    first = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(xport_load_le32(a + 4))), ((int32_t)(uint32_t)(xport_load_le32(b + 8))));
    last = sub_8001FC90(((int32_t)(uint32_t)(xport_load_le32(a + 8))), ((int32_t)(uint32_t)(xport_load_le32(b + 4))));
    xport_store_le32(r, first - (uint32_t)last); /* 8002E8B8: before the next input loads */
    first = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(xport_load_le32(a + 8))), ((int32_t)(uint32_t)(xport_load_le32(b))));
    last = sub_8001FC90(((int32_t)(uint32_t)(xport_load_le32(a))), ((int32_t)(uint32_t)(xport_load_le32(b + 8))));
    xport_store_le32(r + 4, first - (uint32_t)last); /* 8002E8E0 */
    first = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(xport_load_le32(a))), ((int32_t)(uint32_t)(xport_load_le32(b + 4))));
    last = sub_8001FC90(((int32_t)(uint32_t)(xport_load_le32(a + 4))), ((int32_t)(uint32_t)(xport_load_le32(b))));
    xport_store_le32(r + 8, first - (uint32_t)last); /* SUBU wraps at 32 bits */
    return last;                                     /* Preserve observed v0/v1, without claiming a meaningful source return. */
}

int64_t sub_8002E928(const void *vector, const void *matrix, void *out)
{
    FUNCTION_MARKER(0x8002E928u, "SLUS_010.53");
    const uint8_t *v = (const uint8_t *)vector, *m = (const uint8_t *)matrix;
    uint8_t *r = (uint8_t *)out;
    uint32_t first, second;
    int64_t last = 0;
    unsigned column;
    for (column = 0; column != 3; ++column)
    {
        first = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(xport_load_le32(v))), ((int32_t)(uint32_t)(xport_load_le32(m + column * 4))));
        second = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(xport_load_le32(v + 4))), ((int32_t)(uint32_t)(xport_load_le32(m + 12 + column * 4))));
        last = sub_8001FC90(((int32_t)(uint32_t)(xport_load_le32(v + 8))), ((int32_t)(uint32_t)(xport_load_le32(m + 24 + column * 4))));
        /* Original unrolled stores: 8002E984, 8002E9C0, 8002E9FC.
         * Do not use NFS4's temporary result vector: that changes overlap. */
        xport_store_le32(r + column * 4, first + second + (uint32_t)last);
    }
    return last;
}
