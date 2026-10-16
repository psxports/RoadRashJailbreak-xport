#include "race_bodyless_batch_002.h"
#include "psx.h"
/* Resource -> game settings, F8006310C. Void ABI: v0 is scratch. */
#include "resource_apply.h"
#include "resource_value.h"
#include "game_main.h"
#include "race_bodyless_batch_003.h"

static uint32_t value(uint32_t r)
{
    uint32_t scalar_v_16;

    return (uint32_t)r_u8((r_u32(r + 4) + 12 * ((scalar_v_16 = ((uint32_t)r_u8((r + 3)))), (((uint32_t)(scalar_v_16 < 128 ? scalar_v_16 : scalar_v_16 | 0xffffff00)))) + 1));
}

void sub_8006310C(uint32_t r)
{
    uint32_t scalar_v_24;

    uint32_t scalar_v_17;
    uint32_t scalar_v_18;
    uint32_t scalar_v_20;
    uint32_t scalar_v_21;
    uint32_t scalar_v_22;
    uint32_t scalar_v_23;

    FUNCTION_MARKER(0x8006310Cu, "RASHCDF.BIN");
    uint32_t kind, dest = 0, v, index, other, changed;
    if (!r || !r_u32(r + 4))
        return;
    kind = (uint32_t)r_u8((r + 2));
    switch (kind)
    {
        case 0:
            v = value(r);
            index = (uint32_t)r_u8(0x8009C5E3);
            w_u8(0x800D81E1, (uint8_t)v);
            if (index & 192)
            {
                other = r_u32(v == 1 ? 0x8009C4B8 : 0x8009C4B4);
                w_u8(0x800D81E2, (uint8_t)0);
                (void)sub_800630C0(other, 0);
            }
            return;
        case 1:
        case 2:
            dest = 0x800D81E2;
            break;
        case 3:
        case 8:
        case 15:
        case 18:
        case 22:
            dest = 0x800D80E0;
            break;
        case 4:
        case 5:
        case 16:
        case 19:
        case 23:
        case 24:
            dest = 0x800D81DF;
            break;
        case 6:
            dest = 0x800D80DC;
            break;
        case 7:
        case 62:
            dest = 0x800D81D8 + 36 * (uint32_t)r_u8(0x800D80EB) + 7;
            break;
        case 9:
            dest = 0x800D80EA;
            break;
        case 10:
            dest = 0x8009C5EF;
            break;
        case 11:
            dest = 0x800D80E1;
            break;
        case 12:
            dest = 0x800D80E2;
            break;
        case 13:
            dest = 0x800D80E3;
            break;
        case 14:
            w_u8(0x800D80DF, (uint8_t)(value(r)));
            w_u8(0x800D80E0, (uint8_t)(value(r) + 38));
            return;
        case 17:
        case 20:
        case 21:
        case 25:
            dest = 0x800D8203;
            break;
        case 26:
            dest = 0x800D81ED;
            break;
        case 27:
            dest = 0x800D8211;
            break;
        case 28:
            v = value(r);
            changed = (uint32_t)r_u8(0x800D80EC) != v;
            w_u8(0x800D80EC, (uint8_t)v);
            (void)sub_8001EFDC(v == 0);
            if (changed)
                (void)sub_8007ED34(((scalar_v_17 = ((uint32_t)r_u8(0x800D80EC))), (((uint32_t)(scalar_v_17 < 128 ? scalar_v_17 : scalar_v_17 | 0xffffff00)))));
            return;
        case 29:
            dest = 0x800D80EF;
            break;
        case 30:
            dest = 0x800D80F1;
            break;
        case 31:
            dest = 0x8009C5DE;
            break;
        case 32:
            index = (uint32_t)r_u8(0x8009C5DE);
            w_u8((0x800D8198 + index), (uint8_t)(value(r)));
            index = (uint32_t)r_u8(0x8009C5DE);
            sub_8002490C(index, ((scalar_v_18 = ((uint32_t)r_u8((0x800D8198 + index)))), (((uint32_t)(scalar_v_18 < 128 ? scalar_v_18 : scalar_v_18 | 0xffffff00)))));
            return;
        case 33:
            other = r_u32(0x8009C538);
            v = value(r);
            w_u8(0x8009C5E6, (uint8_t)v);
            (void)sub_800630C0(other, ((scalar_v_24 = ((uint32_t)r_u8((0x800D81D8 + 36 * ((uint32_t)(((uint32_t)(v)) < 128 ? ((uint32_t)(v)) : ((uint32_t)(v)) | 0xffffff00)) + 8)))), (((uint32_t)(scalar_v_24 < 128 ? scalar_v_24 : scalar_v_24 | 0xffffff00)))));
            index = ((scalar_v_20 = ((uint32_t)r_u8(0x8009C5E6))), (((uint32_t)(scalar_v_20 < 128 ? scalar_v_20 : scalar_v_20 | 0xffffff00))));
            other = r_u32(0x8009C53C);
            (void)sub_800630C0(other, ((scalar_v_21 = ((uint32_t)r_u8((0x800D81D8 + 36 * index + 23)))), (((uint32_t)(scalar_v_21 < 128 ? scalar_v_21 : scalar_v_21 | 0xffffff00)))));
            return;
        case 34:
            dest = 0x800D81D8 + 36 * ((scalar_v_22 = ((uint32_t)r_u8(0x8009C5E6))), (((uint32_t)(scalar_v_22 < 128 ? scalar_v_22 : scalar_v_22 | 0xffffff00)))) + 8;
            break;
        case 35:
            dest = 0x800D81D8 + 36 * ((scalar_v_23 = ((uint32_t)r_u8(0x8009C5E6))), (((uint32_t)(scalar_v_23 < 128 ? scalar_v_23 : scalar_v_23 | 0xffffff00)))) + 23;
            break;
        case 36:
            dest = 0x8009C5E9;
            break;
        default:
            return;
    }
    w_u8(dest, (uint8_t)(value(r)));
}
