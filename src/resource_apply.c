#include "psx.h"
/* Resource -> game settings, F8006310C. Void ABI: v0 is scratch. */
#include "resource_apply.h"
#include "resource_value.h"
#include <stdlib.h>

static uint32_t b(RRJMemory *m, uint32_t a)
{
    return r_u8(a);
}

static uint32_t sb(uint32_t v)
{
    return v < 128 ? v : v | 0xffffff00;
}

static void put(RRJMemory *m, uint32_t a, uint32_t v)
{
    w_u8(a, (uint8_t)v);
}

static uint32_t value(RRJMemory *m, uint32_t r)
{
    return b(m, rrj_read32(r + 4) + 12 * sb(b(m, r + 3)) + 1);
}

static void call(RRJMemory *m, RRJSDKCall effect, uint32_t fn, uint32_t a, uint32_t c)
{
    if (!effect)
        abort();
    effect(m, fn, a, c);
}

void sub_F_8006310C(uint32_t r, RRJSDKCall effect)
{
    FUNCTION_MARKER(0x8006310Cu, "RASHCDF.BIN");
    uint32_t kind, dest = 0, v, index, other, changed;
    if (!r || !rrj_read32(r + 4))
        return;
    kind = b(rrj_host_context(), r + 2);
    switch (kind)
    {
        case 0:
            v = value(rrj_host_context(), r);
            index = b(rrj_host_context(), 0x8009C5E3);
            put(rrj_host_context(), 0x800D81E1, v);
            if (index & 192)
            {
                other = rrj_read32(v == 1 ? 0x8009C4B8 : 0x8009C4B4);
                put(rrj_host_context(), 0x800D81E2, 0);
                (void)sub_F_800630C0(other, 0);
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
            dest = 0x800D81D8 + 36 * b(rrj_host_context(), 0x800D80EB) + 7;
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
            put(rrj_host_context(), 0x800D80DF, value(rrj_host_context(), r));
            put(rrj_host_context(), 0x800D80E0, value(rrj_host_context(), r) + 38);
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
            v = value(rrj_host_context(), r);
            changed = b(rrj_host_context(), 0x800D80EC) != v;
            put(rrj_host_context(), 0x800D80EC, v);
            call(rrj_host_context(), effect, 0x8001EFDC, v == 0, 0);
            if (changed)
                call(rrj_host_context(), effect, 0x8007ED34, sb(b(rrj_host_context(), 0x800D80EC)), 0);
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
            index = b(rrj_host_context(), 0x8009C5DE);
            put(rrj_host_context(), 0x800D8198 + index, value(rrj_host_context(), r));
            index = b(rrj_host_context(), 0x8009C5DE);
            call(rrj_host_context(), effect, 0x8002490C, index, sb(b(rrj_host_context(), 0x800D8198 + index)));
            return;
        case 33:
            other = rrj_read32(0x8009C538);
            v = value(rrj_host_context(), r);
            put(rrj_host_context(), 0x8009C5E6, v);
            (void)sub_F_800630C0(other, sb(b(rrj_host_context(), 0x800D81D8 + 36 * sb(v) + 8)));
            index = sb(b(rrj_host_context(), 0x8009C5E6));
            other = rrj_read32(0x8009C53C);
            (void)sub_F_800630C0(other, sb(b(rrj_host_context(), 0x800D81D8 + 36 * index + 23)));
            return;
        case 34:
            dest = 0x800D81D8 + 36 * sb(b(rrj_host_context(), 0x8009C5E6)) + 8;
            break;
        case 35:
            dest = 0x800D81D8 + 36 * sb(b(rrj_host_context(), 0x8009C5E6)) + 23;
            break;
        case 36:
            dest = 0x8009C5E9;
            break;
        default:
            return;
    }
    put(rrj_host_context(), dest, value(rrj_host_context(), r));
}
