#include "psx.h"
#include "first2_runtime_dependencies.h"

/* Unverified native dependency reconstructed from the complete original gap */
uint32_t first2_800C2218(uint32_t input, uint32_t entity, int32_t direction)
{
    FUNCTION_MARKER(0x800C2218u, "RASHCDG.BIN");
    uint32_t proxy = rrj_read32(entity + 852u);
    uint32_t mapping, button, equipment, index, step, flags, value;
    int32_t total;
    if (r_u8(proxy + 569u) != 41u)
        return 0;
    mapping = rrj_read32(input + 184u);
    button = rrj_read32(mapping + 4u * r_u16(0x800CCB78u));
    if (!r_s8(input + 26u + 8u * button) && direction != 1)
        return 0;
    step = (uint32_t)direction + 10u;
    do
    {
        equipment = rrj_read32(entity + 1084u);
        total = (int32_t)((uint32_t)r_u8(equipment + 46u) + step);
        w_u8(equipment + 46u, (uint8_t)(total % 10));
        proxy = rrj_read32(entity + 852u);
        flags = r_u8(proxy + 572u);
        w_u8(proxy + 572u, flags | 1u);
        equipment = rrj_read32(entity + 1084u);
        value = r_u16(equipment + 44u);
        index = r_u8(equipment + 46u);
    } while (!((value >> (index & 31u)) & 1u));
    value = index < 8u ? (rrj_read32(equipment + 48u) >> ((index * 4u) & 31u)) & 15u : 0;
    w_u8(equipment + 47u, value);
    equipment = rrj_read32(entity + 1084u);
    index = r_u8(equipment + 46u);
    w_u8(equipment + 60u, index < 9u ? 142u : 32u);
    return 1;
}
