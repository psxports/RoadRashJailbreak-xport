#include "first2_camera_dependencies.h"
#include "fixed_math.h"
#include "race_pause.h"

uint32_t sub_80086584(uint32_t actor, uint32_t origin, uint32_t target, uint32_t output)
{
    uint32_t difference[3];
    uint32_t partial = 0;
    unsigned row;
    unsigned column;

    FUNCTION_MARKER(0x80086584, "RASHCDG.BIN");
    /* The two-word entry prefix supplies X before falling through 8008658C */
    for (column = 0; column != 3; ++column)
        difference[column] = r_u32(target + 4 * column) - r_u32(origin + 4 * column);
    for (row = 0; row != 3; ++row)
    {
        uint32_t sum = 0;
        for (column = 0; column != 3; ++column)
        {
            int32_t basis = (int16_t)r_u16(actor + 588 + 6 * row + 2 * column);
            uint32_t product = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(difference[column])), basis * 16);
            if (column == 2)
                partial = sum;
            sum += product;
        }
        w_u32(output + 4 * row, sum);
    }
    return partial;
}

uint32_t sub_800863EC(uint32_t vector)
{
    uint32_t scalar_value_1;

    uint32_t x = r_u32(vector);
    uint32_t z = r_u32(vector + 8);
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
    if (((int32_t)(uint32_t)(z)) < 0)
        angle += ((int32_t)(uint32_t)(angle)) < 0 ? 2048u : 0u - 2048u;
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
    basis = (int16_t)r_u16(table);
    basis *= 16;
    if (((int32_t)(uint32_t)(scalar)) > 0)
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
    w_u32(vector + 8, radius);
    /* Original shifts and additions wrap before the final arithmetic shift */
    result = ((scalar_value_1 = (angle * 25736u)), (((uint32_t)((scalar_value_1 >> ((unsigned)(8))) | ((0u - (scalar_value_1 >> 31)) << (32u - ((unsigned)(8))))))));
    w_u32(vector, result);
    return result;
}
