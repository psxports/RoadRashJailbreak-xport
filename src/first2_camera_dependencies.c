#include "first2_camera_dependencies.h"
#include "fixed_math.h"
#include "race_pause.h"

static uint32_t camera_asr(uint32_t value, unsigned shift)
{
    return (value >> shift) | ((0u - (value >> 31)) << (32u - shift));
}

uint32_t sub_80086584(uint32_t actor, uint32_t origin, uint32_t target, uint32_t output)
{
    uint32_t difference[3];
    uint32_t partial = 0;
    unsigned row;
    unsigned column;

    FUNCTION_MARKER(0x80086584, "RASHCDG.BIN");
    /* The two-word entry prefix supplies X before falling through 8008658C */
    for (column = 0; column != 3; ++column)
        difference[column] = rrj_read32(target + 4 * column) - rrj_read32(origin + 4 * column);
    for (row = 0; row != 3; ++row)
    {
        uint32_t sum = 0;
        for (column = 0; column != 3; ++column)
        {
            int32_t basis = (int16_t)rrj_u16(rrj_at(actor + 588 + 6 * row + 2 * column, 2));
            uint32_t product = (uint32_t)sub_8001FC90(rrj_s32(difference[column]), basis * 16);
            if (column == 2)
                partial = sum;
            sum += product;
        }
        rrj_write32(output + 4 * row, sum);
    }
    return partial;
}

uint32_t sub_800863EC(uint32_t vector)
{
    uint32_t x = rrj_read32(vector);
    uint32_t z = rrj_read32(vector + 8);
    uint32_t angle;
    uint32_t sign;
    uint32_t absolute;
    uint32_t table;
    uint32_t scalar;
    int32_t basis;
    uint32_t radius;
    uint32_t result;

    FUNCTION_MARKER(0x800863EC, "RASHCDG.BIN");
    angle = sub_80020018(x, z);
    if (rrj_s32(z) < 0)
        angle += rrj_s32(angle) < 0 ? 2048u : 0u - 2048u;
    sign = 0u - (angle >> 31);
    absolute = (angle + sign) ^ sign;
    table = 0x8005624Cu + 4 * (angle & 0xFFFu);
    if (absolute - 513u < 1023u)
        scalar = x;
    else
    {
        scalar = z;
        table += 2;
    }
    basis = (int16_t)rrj_u16(rrj_at(table, 2));
    basis *= 16;
    if (rrj_s32(scalar) > 0)
    {
        if (basis > 0)
            radius = sub_80010028(scalar, (uint32_t)basis);
        else
            radius = 0u - sub_80010028(scalar, 0u - (uint32_t)basis);
    }
    else
    {
        if (basis > 0)
            radius = 0u - sub_80010028(0u - scalar, (uint32_t)basis);
        else
            radius = sub_80010028(0u - scalar, 0u - (uint32_t)basis);
    }
    rrj_write32(vector + 8, radius);
    /* Original shifts and additions wrap before the final arithmetic shift */
    result = camera_asr(angle * 25736u, 8);
    rrj_write32(vector, result);
    return result;
}
