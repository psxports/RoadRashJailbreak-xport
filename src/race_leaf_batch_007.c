#include "race_leaf_batch_007.h"
#include "race_pause.h"
#include "xport.h"

uint32_t sub_800150EC(RRJMemory *m, RRJRaceLeafCall call)
{
    uint32_t callback = rrj_read32(m, 0x8005B3B8u);
    const uint32_t args[8] = {0};

    FUNCTION_MARKER(0x800150EC, "SLUS_010.53");
    return callback ? call(m, callback, args) : 0;
}

uint32_t sub_80022EEC(RRJMemory *m, RRJRaceLeafCall call)
{
    uint32_t record = rrj_read32(m, 0x8005B4ECu);
    uint32_t callback = rrj_read32(m, record + 24u);
    uint32_t args[8] = {0};

    FUNCTION_MARKER(0x80022EEC, "SLUS_010.53");
    rrj_write32(m, 0x8005AE20u, 0);
    rrj_write32(m, 0x8005B4E8u, rrj_read32(m, 0x8005B4E8u) + 1u);
    if (callback)
    {
        args[0] = rrj_read32(m, record + 12u);
        args[1] = rrj_read32(m, record);
        args[2] = rrj_read32(m, record + 20u);
        (void)call(m, callback, args);
    }
    sub_80022B0C(m, 0, call);
    return 0;
}

uint32_t sub_800315E8(RRJMemory *m, uint32_t object)
{
    FUNCTION_MARKER(0x800315E8, "SLUS_010.53");
    rrj_write32(m, object + 8u, 0xFFFFFFFFu);
    rrj_write32(m, object + 20u, 0);
    rrj_write32(m, object + 24u, 0);
    rrj_write32(m, object + 16u, rrj_read32(m, object + 12u));
    return 0xFFFFFFFFu;
}

uint32_t sub_80031540(RRJMemory *m, uint32_t object)
{
    uint32_t base = rrj_read32(m, object + 12u);
    uint32_t kind;
    uint32_t current;
    int32_t offset;

    FUNCTION_MARKER(0x80031540, "SLUS_010.53");
    rrj_write32(m, object + 20u, base);
    rrj_write32(m, object + 8u, rrj_read32(m, base));
    kind = rrj_read32(m, base) >> 28;
    if (kind == 1u || kind == 2u)
    {
        rrj_write32(m, object + 24u, 0);
        rrj_write32(m, object + 16u, base);
        return base;
    }
    current = base + 32u;
    rrj_write32(m, object + 16u, current);
    if (kind != 0u && kind != 8u)
    {
        rrj_write32(m, object + 24u, 0);
        return current;
    }
    offset = (int32_t)(rrj_read32(m, current) * 4u - 48u);
    if (offset > 0)
        rrj_write32(m, object + 24u, current + (uint32_t)offset);
    return current + (uint32_t)offset;
}

uint32_t sub_80031CD4(RRJMemory *m, uint32_t object, RRJRaceLeafCall call)
{
    uint32_t context = rrj_read32(m, 0x8005ACBCu);
    uint32_t index;
    uint32_t count;

    FUNCTION_MARKER(0x80031CD4, "SLUS_010.53");
    count = rrj_read32(m, context + 2356u);
    if (count == 64u)
    {
        const uint32_t args[8] = {0x80010D2Cu, 0, 0, 0, 0, 0, 0, 0};

        return call(m, 0x80044894u, args);
    }
    rrj_write32(m, object, rrj_read32(m, object) | 0x80u);
    index = rrj_read32(m, context + 2348u);
    rrj_write32(m, context + 2360u + 4u * index, object);
    index = (index + 1u) & 63u;
    rrj_write32(m, context + 2348u, index);
    rrj_write32(m, context + 2356u, count + 1u);
    return index;
}

uint32_t sub_80030894(RRJMemory *m, uint32_t index, uint32_t mode, uint32_t value, RRJRaceLeafCall call)
{
    uint32_t context = rrj_read32(m, 0x8005ACBCu);
    uint32_t object = context + 44u + 36u * index;
    uint32_t flags = rrj_read32(m, object);
    uint32_t args[8] = {0};

    FUNCTION_MARKER(0x80030894, "SLUS_010.53");
    if ((flags & 9u) == 9u)
    {
        args[0] = 0x80010D00u;
        (void)call(m, 0x80044894u, args);
    }
    flags = (flags & 0xFFFFFFFAu) | 1u;
    rrj_write32(m, object, flags);
    args[0] = 2;
    args[1] = 1;
    (void)call(m, 0x80032190u, args);
    if (flags & 0x10u)
    {
        rrj_write32(m, context + 4u, rrj_read32(m, context + 4u) + 1u);
        rrj_write32(m, context + 24u, rrj_read32(m, context + 24u) - 1u);
    }
    else if (flags & 8u)
    {
        rrj_write32(m, context + 8u, rrj_read32(m, context + 8u) + 1u);
        rrj_write32(m, context + 28u, rrj_read32(m, context + 28u) - 1u);
    }
    rrj_write32(m, object + 32u, value);
    rrj_write32(m, object, rrj_read32(m, object) | mode);
    if (mode == 8u)
    {
        rrj_write32(m, object, rrj_read32(m, object) | 0x20u);
        args[0] = object;
        (void)call(m, 0x800315E8u, args);
        args[1] = 0;
        return call(m, 0x80031604u, args);
    }
    if (mode == 16u)
    {
        args[0] = object;
        (void)call(m, 0x80031540u, args);
        return call(m, 0x80031CD4u, args);
    }
    return 16;
}

uint32_t sub_800151EC(RRJMemory *m)
{
    FUNCTION_MARKER(0x800151EC, "SLUS_010.53");
    rrj_write32(m, 0x8005B3C0u, 1);
    return 1;
}

uint32_t sub_80090270(RRJMemory *m)
{
    uint32_t count_address = rrj_read32(m, 0x800CE4DCu);
    int32_t remaining = (int32_t)rrj_read32(m, count_address);
    uint32_t object = rrj_read32(m, 0x800CE4D0u);
    uint32_t eligible = 0;
    uint32_t result = count_address;

    FUNCTION_MARKER(0x80090270, "RASHCDG.BIN");
    while (remaining >= 0)
    {
        uint32_t state = rrj_read32(m, 0x8005B2F8u);
        uint32_t type = rrj_read32(m, object + 1084u);

        if ((int16_t)rrj_u16(rrj_at(m, object + 320u, 2)) == 0 && rrj_u16(rrj_at(m, object + 172u, 2)) >= rrj_read32(m, state + 48u) && (r_u8(type + 1u) & 15u) != 2u)
            ++eligible;
        --remaining;
        object += rrj_read32(m, 0x800CE4D4u);
    }

    count_address = rrj_read32(m, 0x800CE4DCu);
    remaining = (int32_t)rrj_read32(m, count_address);
    object = rrj_read32(m, 0x800CE4D0u);
    result = count_address;
    while (remaining >= 0)
    {
        uint32_t state = rrj_read32(m, 0x8005B2F8u);
        uint32_t type = rrj_read32(m, object + 1084u);

        if ((r_u8(type + 1u) & 15u) != 2u || rrj_u16(rrj_at(m, object + 172u, 2)) < rrj_read32(m, state + 48u))
        {
            if ((int16_t)rrj_u16(rrj_at(m, object + 320u, 2)) != 0)
            {
                int32_t rank = (int32_t)r_u8(type + 39u) - (int32_t)eligible - 1;
                uint16_t value;

                if (rank < 0)
                    rank = 0;
                value = rank / 2 ? (uint16_t)(4 * (rank / 2)) : 1u;
                rrj_put16(rrj_at(m, object + 960u, 2), value);
                if (r_u8(rrj_read32(m, object + 852u) + 572u) & 0x10u)
                {
                    uint32_t linked = rrj_read32(m, object + 856u);
                    uint32_t slot = (uint32_t)(int32_t)(int8_t)r_u8(linked + 946u);

                    rrj_put16(rrj_at(m, linked + 952u + slot * 8u, 2), value);
                }
            }
            else
            {
                rrj_put16(rrj_at(m, object + 960u, 2), 1u);
            }
        }
        result = rrj_read32(m, 0x800CE4D4u);
        --remaining;
        object += result;
    }
    return result;
}

uint32_t sub_8003775C(RRJMemory *m, uint32_t actor, uint32_t record, uint32_t delta)
{
    uint32_t candidates[24] = {0};
    uint32_t directions[3] = {0};
    uint32_t current;
    uint32_t direction;
    uint32_t magnitude;
    uint32_t covered;
    uint32_t progress = 0;

    FUNCTION_MARKER(0x8003775C, "SLUS_010.53");
    if (!record)
        return 0;
    current = rrj_read32(m, record + 12u);
    if (!delta)
        return current;
    if (actor)
        direction = rrj_read32(m, actor + 192u);
    else
        direction = (int32_t)delta > 0 ? 1u : 0xFFFFFFFFu;
    if ((int32_t)direction > 0)
        covered = rrj_read32(m, current + 32u) - rrj_read32(m, record + 20u);
    else
        covered = rrj_read32(m, record + 20u);
    magnitude = (delta + (uint32_t)((int32_t)delta >> 31)) ^ (uint32_t)((int32_t)delta >> 31);
    directions[0] = direction;
    if ((int32_t)covered >= (int32_t)magnitude)
    {
        progress = rrj_read32(m, record + 20u);
        progress = (int32_t)direction > 0 ? progress + magnitude : progress - magnitude;
    }
    else
    {
        uint32_t segment = (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, current, 2));
        uint32_t last = (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, rrj_read32(m, record + 8u) + 10u, 2));

        if (segment == 0 || segment == last - 1u)
        {
            uint32_t state[8];
            uint32_t count;
            uint32_t i;

            for (i = 0; i < 8; ++i)
                state[i] = rrj_read32(m, record + 4u * i);
            count = rrj_traverse_track_local(m, actor, state, candidates, directions, 3, (int32_t)direction <= 0);
            current = rrj_commit_track_local(m, actor, candidates, directions, count, state, &direction, NULL, NULL);
            for (i = 0; i < 8; ++i)
                rrj_write32(m, record + 4u * i, state[i]);
        }
        else
        {
            current = (int32_t)direction > 0 ? current + 52u : current - 52u;
            rrj_write32(m, record + 12u, current);
        }
        while ((int32_t)covered < (int32_t)magnitude)
        {
            uint32_t length = rrj_read32(m, current + 32u);
            uint32_t remaining = magnitude - covered;

            if ((int32_t)length >= (int32_t)remaining)
            {
                progress = remaining;
                if ((int32_t)direction <= 0)
                    progress = length - remaining;
                covered = magnitude;
                continue;
            }
            covered += length;
            segment = (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, rrj_read32(m, record + 12u), 2));
            last = (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, rrj_read32(m, record + 8u) + 10u, 2));
            if (segment == 0 || segment == last - 1u)
            {
                uint32_t state[8];
                uint32_t count;
                uint32_t i;

                if (sub_800394F0(m, record, direction))
                    break;
                for (i = 0; i < 8; ++i)
                    state[i] = rrj_read32(m, record + 4u * i);
                count = rrj_traverse_track_local(m, actor, state, candidates, directions, 3, (int32_t)direction <= 0);
                current = rrj_commit_track_local(m, actor, candidates, directions, count, state, &direction, NULL, NULL);
                for (i = 0; i < 8; ++i)
                    rrj_write32(m, record + 4u * i, state[i]);
            }
            else
            {
                current = (int32_t)direction > 0 ? current + 52u : current - 52u;
                rrj_write32(m, record + 12u, current);
            }
        }
    }
    rrj_write32(m, record + 20u, progress);
    return current;
}
