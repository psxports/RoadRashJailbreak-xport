#include "race_leaf_batch_001.h"
#include "fixed_math.h"
#include "menu_latch.h"
#include "race_pause.h"
#include "xport.h"

static int32_t batch_signed_ratio(int32_t numerator, int32_t denominator)
{
    uint32_t magnitude = numerator > 0 ? (uint32_t)numerator : 0u - (uint32_t)numerator;
    uint32_t divisor = denominator > 0 ? (uint32_t)denominator : 0u - (uint32_t)denominator;
    int32_t result = (int32_t)sub_80010028(magnitude, divisor);

    return (numerator > 0) == (denominator > 0) ? result : -result;
}

uint32_t sub_8003FA18(int32_t count, uint32_t source, uint32_t destination)
{
    uint32_t value = 0;

    FUNCTION_MARKER(0x8003FA18, "SLUS_010.53");
    while (count-- > 0)
    {
        value = rrj_u16(rrj_at(source, 2));
        rrj_put16(rrj_at(destination, 2), (uint16_t)value);
        source += 2;
        destination += 2;
    }
    return value;
}

uint32_t sub_8009FC4C(uint32_t value, uint32_t values, int32_t count)
{
    int32_t index;

    FUNCTION_MARKER(0x8009FC4C, "RASHCDG.BIN");
    for (index = 0; index < count; ++index)
        if (rrj_read32(values + 4u * (uint32_t)index) == value)
            return 1;
    return 0;
}

uint32_t sub_8009F054(uint32_t candidates, uint32_t player)
{
    uint32_t actor;
    int32_t index;

    FUNCTION_MARKER(0x8009F054, "RASHCDG.BIN");
    if (!candidates)
        return 1;
    actor = rrj_read32(0x8005B268u + 4u * player);
    if (rrj_u16(rrj_at(actor + 362, 2)) == 1)
        return 0;
    for (index = 0; index < 4; ++index, candidates += 12)
    {
        int32_t candidate_id = rrj_s32(rrj_read32(candidates));
        int32_t center;
        int32_t first;
        int32_t second;
        int32_t sign;

        if (candidate_id < 0)
            break;
        if ((uint32_t)candidate_id != rrj_u16(rrj_at(actor + 360, 2)))
            continue;
        center = rrj_s32(rrj_read32(actor + 368)) >> 10;
        first = rrj_s32(rrj_read32(candidates + 4) - (uint32_t)center);
        sign = first >> 31;
        first = rrj_s32(((uint32_t)first + (uint32_t)sign) ^ (uint32_t)sign);
        if (first < 19201)
            continue;
        second = rrj_s32(rrj_read32(candidates + 8) - (uint32_t)center);
        sign = second >> 31;
        second = rrj_s32(((uint32_t)second + (uint32_t)sign) ^ (uint32_t)sign);
        if (second >= 19201)
            return 1;
    }
    return 0;
}

uint32_t sub_8009C41C(uint32_t record, uint32_t kind, uint32_t index, uint32_t value)
{
    uint32_t context;
    uint32_t tables;
    uint32_t target;

    FUNCTION_MARKER(0x8009C41C, "RASHCDG.BIN");
    context = rrj_read32(0x8005B2F8);
    if (!(r_u8(context + 4) & 0x10) || !record)
        return r_u8(context + 4) & 0x10;
    tables = rrj_read32(rrj_read32(record + 4) + 32);
    switch (kind)
    {
        case 0:
            target = rrj_read32(tables + 52) + 64u * (index & 0xFFu);
            break;
        case 2:
            target = rrj_read32(tables + 40) + 76u * (index & 0xFFu);
            break;
        case 3:
            target = rrj_read32(tables + 36) + 68u * (index & 0xFFu);
            break;
        case 4:
            target = rrj_read32(tables + 44) + 64u * (index & 0xFFu);
            break;
        case 6:
            target = rrj_read32(tables + 48) + 88u * (index & 0xFFu);
            break;
        default:
            return 0x80060000u;
    }
    rrj_put16(rrj_at(target + 4, 2), (uint16_t)value);
    return target;
}

uint32_t sub_8003C42C(uint32_t output, int32_t limit)
{
    int32_t maximum;
    int32_t index;
    int32_t count = 0;

    FUNCTION_MARKER(0x8003C42C, "SLUS_010.53");
    if (!output)
        return 0;
    maximum = rrj_s32(rrj_read32(0x8005B31C));
    for (index = 0; index <= maximum; ++index)
    {
        uint32_t value = rrj_read32(0x800D4B10u + 16u * (uint32_t)index);

        if (value == 0xFFFFFFFFu)
            continue;
        if (count >= limit)
            return (uint32_t)count;
        rrj_write32(output + 4u * (uint32_t)count, value);
        ++count;
    }
    return (uint32_t)count;
}

uint32_t sub_8003C494(uint32_t identity, uint32_t road_id)
{
    uint32_t record;
    uint32_t road;
    uint32_t entries;
    int32_t count;
    int32_t index;

    FUNCTION_MARKER(0x8003C494, "SLUS_010.53");
    record = sub_80039A08(road_id);
    if (!record || !(road = rrj_read32(record + 12)))
        return 0;
    count = (int16_t)rrj_u16(rrj_at(road + 30, 2));
    entries = rrj_read32(road + 72);
    for (index = 0; index < count; ++index, entries += 20)
        if (rrj_read32(entries) == identity)
            return entries;
    return 0;
}

uint32_t sub_8008D9E4(uint32_t player)
{
    FUNCTION_MARKER(0x8008D9E4, "RASHCDG.BIN");
    if (rrj_read32(rrj_read32(0x8005B2F8) + 48) != 2)
        return 1;
    return rrj_s32(rrj_read32(0x800CD6B8u + 4u * player)) < 16;
}

uint32_t sub_8008DECC(uint32_t player, uint32_t group)
{
    uint32_t state = 0x800D5CF8u + 32u * player;
    uint32_t group_state = state + 4u * group;
    int32_t count = rrj_s32(rrj_read32(group_state + 8));
    uint32_t identity = 224;

    FUNCTION_MARKER(0x8008DECC, "RASHCDG.BIN");
    if (count > 0)
    {
        int32_t tail = rrj_s32(rrj_read32(group_state + 20));

        if (tail != -1)
        {
            uint32_t entry = rrj_read32(state + 4) + 8u * (uint32_t)tail;

            rrj_write32(group_state + 20, (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(entry + 2, 2)));
            rrj_put16(rrj_at(entry + 2, 2), 0xFFFFu);
            rrj_write32(entry + 4, 0);
            identity = rrj_u16(rrj_at(rrj_read32(0x800CD6C4) + 280u * (uint32_t)tail, 2));
        }
        rrj_write32(group_state + 8, (uint32_t)(count - 1));
    }
    return identity;
}

uint32_t sub_8008DA20(uint32_t identity, uint32_t player)
{
    uint32_t player_state = 0x800CD898u + 1132u * player;
    int32_t first = (int16_t)rrj_u16(rrj_at(player_state + 194, 2)) - (int16_t)rrj_u16(rrj_at(identity + 30, 2));
    int32_t second = (int16_t)rrj_u16(rrj_at(player_state + 186, 2)) - (int16_t)rrj_u16(rrj_at(identity + 22, 2));
    int32_t large = first < 0 ? (int32_t)(0u - (uint32_t)first) : first;
    int32_t small = second < 0 ? (int32_t)(0u - (uint32_t)second) : second;
    int32_t distance;
    int32_t group = -1;
    uint32_t removed = 224;

    FUNCTION_MARKER(0x8008DA20, "RASHCDG.BIN");
    if (large < small)
    {
        int32_t swap = large;

        large = small;
        small = swap;
    }
    distance = large - (large >> 5) - (large >> 7) + ((small + (small >> 1)) >> 2) + ((small + (small >> 1)) >> 6);
    if (distance < (rrj_s32(rrj_read32(0x8005AD58)) >> 18))
        group = rrj_s32(rrj_read32(0x800D5D08u + 32u * player)) > 0 ? 2 : 1;
    else if (distance < (rrj_s32(rrj_read32(0x8005AD58)) >> 16))
        group = 2;
    if (group != -1)
        removed = sub_8008DECC(player, (uint32_t)group);
    if ((uint16_t)(removed - 1u) < 223u)
    {
        uint32_t object = rrj_read32(0x800CD6C4) + 280u * (removed & 31u);

        if (object)
        {
            sub_8009F3D0(object, player);
            (void)sub_8008C000(object, 6);
            return sub_8008D89C();
        }
        return 280u * (removed & 31u);
    }
    return 0;
}

uint32_t sub_80095848(void)
{
    int32_t count = rrj_s32(rrj_read32(rrj_read32(0x800CE4DC)));
    uint32_t actor = rrj_read32(0x800CE4D0);
    uint32_t stride = rrj_read32(0x800CE4D4);
    uint32_t players = rrj_read32(rrj_read32(0x8005B2F8) + 48);

    FUNCTION_MARKER(0x80095848, "RASHCDG.BIN");
    while (count >= 0)
    {
        if (actor && rrj_u16(rrj_at(actor + 172, 2)) >= players && (r_u8(rrj_read32(actor + 1084) + 1) & 15u) == 2 && !rrj_u16(rrj_at(actor + 320, 2)) && !(r_u8(actor + 928) & 0x10u))
            return actor;
        --count;
        actor += stride;
    }
    return 0;
}

uint32_t sub_8009BB48(uint32_t record, uint32_t player)
{
    const uint32_t scratch = 0x801FFB80u;
    uint32_t road;
    uint32_t object = 0;
    int32_t selected;
    uint32_t basis;
    uint32_t flags;
    int32_t index;

    FUNCTION_MARKER(0x8009BB48, "RASHCDG.BIN");
    rrj_write32(scratch, rrj_read32(record + 8));
    rrj_write32(scratch + 4, (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(record + 60, 2)));
    rrj_write32(scratch + 8, rrj_read32(record + 36));
    road = sub_80039DFC(0, scratch);
    if (!road)
        return 0;

    selected = rrj_s32(rrj_read32(0x800CD6AC));
    if (rrj_read32(0x800CD6C4) && selected < rrj_s32(rrj_read32(0x8005B214)))
    {
        int32_t next = selected + 1;
        uint32_t candidate = rrj_read32(0x800CD6C4) + 280u * (uint32_t)next;

        while (next < rrj_s32(rrj_read32(0x8005B214)))
        {
            if (!rrj_u16(rrj_at(candidate, 2)))
                break;
            ++next;
            candidate += 280;
        }
        rrj_write32(0x800CD6AC, (uint32_t)next);
        object = rrj_read32(0x800CD6C4) + 280u * (uint32_t)selected;
        rrj_put16(rrj_at(object, 2), (uint16_t)(selected + 192));
        rrj_write32(0x800CD6A8, rrj_read32(0x800CD6A8) + 1);
        if (rrj_s32(rrj_read32(0x800CD6B0)) < selected)
            rrj_write32(0x800CD6B0, (uint32_t)selected);
    }
    if (!object)
        return 0;

    rrj_put16(rrj_at(object + 150, 2), rrj_u16(rrj_at(record + 62, 2)));
    if (!sub_8003A700(road, scratch, scratch + 16))
        goto cleanup;
    if ((rrj_read32(scratch) >> 16) == 1)
        (void)sub_8003C758(scratch + 16, record + 20);
    (void)sub_8001E0B4(object + 156, scratch + 16, 32);
    basis = rrj_read32(object + 168);
    rrj_write32(object + 12, rrj_read32(record + 20));
    rrj_write32(object + 16, rrj_read32(record + 24));
    rrj_write32(object + 20, rrj_read32(record + 28));
    sub_80036800(object + 12, basis, object + 172, object + 176);
    rrj_put16(rrj_at(scratch + 48, 2), 0);
    rrj_put16(rrj_at(scratch + 50, 2), 0);
    rrj_put16(rrj_at(scratch + 52, 2), 0);
    (void)sub_8003662C(scratch + 48, object + 156, object + 188);
    rrj_write32(object + 192, (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(record + 60, 2)));
    sub_8003AF9C(object, 0, player);
    rrj_write32(object + 152, sub_8003B61C(object));
    rrj_write32(object + 12, rrj_read32(record + 20));
    rrj_write32(object + 16, rrj_read32(record + 24));
    rrj_write32(object + 20, rrj_read32(record + 28));
    rrj_write32(object + 8, rrj_u16(rrj_at(record + 2, 2)));

    flags = rrj_u16(rrj_at(record + 12, 2));
    if (flags & 1u)
    {
        rrj_put16(rrj_at(object + 260, 2), rrj_u16(rrj_at(basis + 14, 2)));
        rrj_put16(rrj_at(object + 262, 2), rrj_u16(rrj_at(basis + 16, 2)));
        rrj_put16(rrj_at(object + 264, 2), rrj_u16(rrj_at(basis + 18, 2)));
        rrj_put16(rrj_at(object + 266, 2), (uint16_t)-(int16_t)rrj_u16(rrj_at(basis + 8, 2)));
        rrj_put16(rrj_at(object + 268, 2), (uint16_t)-(int16_t)rrj_u16(rrj_at(basis + 10, 2)));
        rrj_put16(rrj_at(object + 270, 2), (uint16_t)-(int16_t)rrj_u16(rrj_at(basis + 12, 2)));
        rrj_put16(rrj_at(object + 272, 2), rrj_u16(rrj_at(basis + 2, 2)));
        rrj_put16(rrj_at(object + 274, 2), rrj_u16(rrj_at(basis + 4, 2)));
        rrj_put16(rrj_at(object + 276, 2), rrj_u16(rrj_at(basis + 6, 2)));
        if (rrj_s32(rrj_read32(object + 172)) >= 0)
            for (index = 272; index <= 276; index += 2)
                rrj_put16(rrj_at(object + (uint32_t)index, 2), (uint16_t)-(int16_t)rrj_u16(rrj_at(object + (uint32_t)index, 2)));
        else
            for (index = 260; index <= 264; index += 2)
                rrj_put16(rrj_at(object + (uint32_t)index, 2), (uint16_t)-(int16_t)rrj_u16(rrj_at(object + (uint32_t)index, 2)));
    }
    else
    {
        for (index = 260; index <= 276; index += 2)
            rrj_put16(rrj_at(object + (uint32_t)index, 2), 0);
        rrj_put16(rrj_at(object + 260, 2), 4096);
        rrj_put16(rrj_at(object + 268, 2), 4096);
        rrj_put16(rrj_at(object + 276, 2), 4096);
    }

    if (rrj_read32(object + 8) == 1)
    {
        int32_t half_width = rrj_s32(rrj_read32(record + 64));

        rrj_write32(object + 132, (uint32_t)half_width);
        rrj_write32(object + 136, (uint32_t)half_width);
        rrj_write32(object + 140, rrj_read32(record + 80));
        rrj_write32(object + 84, rrj_read32(record + 76));
        rrj_write32(object + 88, rrj_read32(record + 68));
        (void)sub_8002ECB8(rrj_at(object + 260, 6), rrj_at(object + 272, 6), rrj_at(object + 24, 12), (uint32_t)-half_width, 0u - rrj_read32(object + 136));
        for (index = 0; index < 3; ++index)
            rrj_write32(object + 48u + 4u * (uint32_t)index, 0u - rrj_read32(object + 24u + 4u * (uint32_t)index));
        (void)sub_8002ECB8(rrj_at(object + 260, 6), rrj_at(object + 272, 6), rrj_at(object + 36, 12), (uint32_t)half_width, 0u - rrj_read32(object + 136));
        for (index = 0; index < 3; ++index)
            rrj_write32(object + 60u + 4u * (uint32_t)index, 0u - rrj_read32(object + 36u + 4u * (uint32_t)index));
        for (index = 0; index < 4; ++index)
        {
            uint32_t vertex = object + 24u + 12u * (uint32_t)index;

            rrj_write32(vertex, rrj_read32(vertex) + rrj_read32(object + 12));
            rrj_write32(vertex + 4, rrj_read32(vertex + 4) + rrj_read32(object + 16));
            rrj_write32(vertex + 8, rrj_read32(vertex + 8) + rrj_read32(object + 20));
        }
    }
    else
    {
        int32_t half_width = rrj_s32(rrj_read32(record + 76) - rrj_read32(record + 64)) >> 1;
        int32_t half_length = rrj_s32(rrj_read32(record + 84) - rrj_read32(record + 72)) >> 1;

        rrj_write32(object + 132, (uint32_t)half_width);
        rrj_write32(object + 140, rrj_read32(record + 80) - rrj_read32(record + 68));
        rrj_write32(object + 136, (uint32_t)half_length);
        if (flags & 2u)
        {
            rrj_put16(rrj_at(object + 272, 2), rrj_u16(rrj_at(record + 14, 2)));
            rrj_put16(rrj_at(object + 274, 2), rrj_u16(rrj_at(record + 16, 2)));
            rrj_put16(rrj_at(object + 276, 2), rrj_u16(rrj_at(record + 18, 2)));
            if (flags & 1u)
            {
                int32_t ax = (int16_t)rrj_u16(rrj_at(object + 266, 2));
                int32_t ay = (int16_t)rrj_u16(rrj_at(object + 268, 2));
                int32_t az = (int16_t)rrj_u16(rrj_at(object + 270, 2));
                int32_t bx = (int16_t)rrj_u16(rrj_at(object + 272, 2));
                int32_t by = (int16_t)rrj_u16(rrj_at(object + 274, 2));
                int32_t bz = (int16_t)rrj_u16(rrj_at(object + 276, 2));

                rrj_put16(rrj_at(object + 260, 2), (uint16_t)((ay * bz - az * by) >> 12));
                rrj_put16(rrj_at(object + 262, 2), (uint16_t)((az * bx - ax * bz) >> 12));
                rrj_put16(rrj_at(object + 264, 2), (uint16_t)((ax * by - ay * bx) >> 12));
                (void)sub_8002E468(object + 260);
            }
            else
            {
                rrj_put16(rrj_at(object + 260, 2), rrj_u16(rrj_at(record + 18, 2)));
                rrj_put16(rrj_at(object + 264, 2), (uint16_t)-(int16_t)rrj_u16(rrj_at(record + 14, 2)));
            }
        }
        (void)sub_8002EAD8(rrj_at(object + 12, 12), rrj_at(object + 266, 6), 0x8000, rrj_at(object + 12, 12));
        (void)sub_8002ECB8(rrj_at(object + 260, 6), rrj_at(object + 272, 6), rrj_at(object + 24, 12), (uint32_t)-half_width, (uint32_t)-half_length);
        (void)sub_8002ECB8(rrj_at(object + 260, 6), rrj_at(object + 272, 6), rrj_at(object + 36, 12), (uint32_t)half_width, (uint32_t)-half_length);
        (void)sub_8002ECB8(rrj_at(object + 260, 6), rrj_at(object + 272, 6), rrj_at(object + 48, 12), (uint32_t)half_width, (uint32_t)half_length);
        (void)sub_8002ECB8(rrj_at(object + 260, 6), rrj_at(object + 272, 6), rrj_at(object + 60, 12), (uint32_t)-half_width, (uint32_t)half_length);
        for (index = 0; index < 4; ++index)
        {
            uint32_t vertex = object + 24u + 12u * (uint32_t)index;

            rrj_write32(vertex, rrj_read32(vertex) + rrj_read32(object + 12));
            rrj_write32(vertex + 4, rrj_read32(vertex + 4) + rrj_read32(object + 16));
            rrj_write32(vertex + 8, rrj_read32(vertex + 8) + rrj_read32(object + 20));
            (void)sub_8002EAD8(rrj_at(vertex, 12), rrj_at(object + 266, 6), 0xFFFF8000u - rrj_read32(object + 140), rrj_at(object + 72u + 12u * (uint32_t)index, 12));
        }
    }
    {
        uint32_t angle = sub_80020018((uint32_t)((int32_t)(int16_t)rrj_u16(rrj_at(object + 272, 2)) << 4), (uint32_t)((int32_t)(int16_t)rrj_u16(rrj_at(object + 276, 2)) << 4));
        int32_t value;

        rrj_write32(object + 120, angle);
        rrj_write32(object + 124, (uint32_t)((int32_t)(int16_t)rrj_u16(rrj_at(0x8005624Eu + 4u * (angle & 0xFFFu), 2)) << 4));
        rrj_write32(object + 128, (uint32_t)((int32_t)(int16_t)rrj_u16(rrj_at(0x8005624Cu + 4u * (angle & 0xFFFu), 2)) << 4));
        value = 4 * (int32_t)sub_8001FC90(rrj_s32(rrj_read32(object + 140)), (int32_t)sub_8001FC90(rrj_s32(rrj_read32(object + 136)), rrj_s32(rrj_read32(object + 132))));
        if (value > 258867)
            value = 258867;
        rrj_write32(object + 144, (uint32_t)value << 9);
    }
    rrj_write32(object + 4, 0xFFFFFFFFu);
    rrj_put16(rrj_at(object + 148, 2), (uint16_t)sub_80039F68(object));
    if (!rrj_u16(rrj_at(object + 148, 2)))
        goto cleanup;
    return object;

cleanup:
    (void)sub_8008C000(object, 6);
    return 0;
}

uint32_t sub_8009F11C(int32_t road_id, uint32_t candidates)
{
    uint32_t record;
    uint32_t road;
    uint32_t entry;
    int32_t entry_count;
    int32_t entry_index;

    FUNCTION_MARKER(0x8009F11C, "RASHCDG.BIN");
    if (road_id < 0 || !candidates)
        return 0;
    record = sub_80039A08((uint32_t)road_id);
    if (!record || !(road = rrj_read32(record + 12)))
        return 0;
    entry_count = (int16_t)rrj_u16(rrj_at(road + 30, 2));
    entry = rrj_read32(road + 72);
    if (entry_count <= 0 || !entry)
        return 0;
    for (entry_index = 0; entry_index < entry_count; ++entry_index, entry += 20)
    {
        uint32_t candidate = candidates;
        int32_t candidate_index;

        if (!entry)
            break;
        for (candidate_index = 0; candidate_index < 4; ++candidate_index, candidate += 12)
        {
            int32_t candidate_id = rrj_s32(rrj_read32(candidate));
            uint32_t bounds;

            if (candidate_id < 0)
                break;
            if (rrj_read32(entry + 4) != (uint32_t)candidate_id)
                continue;
            bounds = (int16_t)rrj_u16(rrj_at(road + 16, 2)) == 1 ? sub_80039C90(road, rrj_read32(entry + 4)) : rrj_read32(road + 44);
            if (bounds && rrj_s32(rrj_read32(candidate + 8)) >= (rrj_s32(rrj_read32(bounds + 24)) >> 10) && (rrj_s32(rrj_read32(bounds + 28)) >> 10) >= rrj_s32(rrj_read32(candidate + 4)))
                return 1;
        }
    }
    return 0;
}

uint32_t sub_8009EF8C(uint32_t candidates, uint32_t output, int32_t limit)
{
    const uint32_t roads = 0x801FFE00u;
    int32_t road_count;
    int32_t road_index;
    int32_t count = 0;

    FUNCTION_MARKER(0x8009EF8C, "RASHCDG.BIN");
    if (!candidates || !output)
        return 0;
    road_count = (int32_t)sub_8003C42C(roads, 6);
    for (road_index = 0; road_index < road_count; ++road_index)
    {
        uint32_t road_id = rrj_read32(roads + 4u * (uint32_t)road_index);

        if ((uint8_t)sub_8009F11C(rrj_s32(road_id), candidates) && count < limit)
        {
            rrj_write32(output + 4u * (uint32_t)count, road_id);
            ++count;
        }
    }
    return (uint32_t)count;
}

uint32_t sub_8009F288(uint32_t record, uint32_t road_ids, int32_t count)
{
    int32_t index;

    FUNCTION_MARKER(0x8009F288, "RASHCDG.BIN");
    for (index = 0; index < count; ++index)
    {
        uint32_t entry = sub_8003C494(rrj_read32(record + 8), rrj_read32(road_ids + 4u * (uint32_t)index));

        if (entry)
        {
            int32_t direction = rrj_s32(rrj_read32(record + 32));
            int32_t position;

            if (rrj_read32(entry + 12))
            {
                direction = -direction;
                position = rrj_s32((rrj_read32(entry + 8) + rrj_read32(entry + 16)) << 10) - rrj_s32(rrj_read32(record + 36));
            }
            else
                position = rrj_s32(rrj_read32(entry + 8) << 10) + rrj_s32(rrj_read32(record + 36));
            if (position >= 0 && rrj_s32((rrj_read32(entry + 8) + rrj_read32(entry + 16)) << 10) >= position)
            {
                rrj_write32(record + 36, (uint32_t)position);
                rrj_write32(record + 32, (uint32_t)direction);
                rrj_write32(record + 8, rrj_u16(rrj_at(entry + 4, 2)));
                return 1;
            }
            {
                uint32_t road = sub_800245DC(rrj_read32(entry + 4));

                if (road)
                {
                    uint32_t next = rrj_u16(rrj_at(road + (position < 0 ? 8u : 12u), 2));

                    rrj_write32(record + 8, next | 0x10000u);
                    rrj_write32(record + 36, 0);
                    rrj_write32(record + 32, 0);
                }
            }
        }
    }
    return 0;
}

uint32_t sub_8009FAD8(uint32_t output, uint32_t player)
{
    const uint32_t frame = 0x801FFC00u;
    uint32_t actor = rrj_read32(0x8005B268u + 4u * player);
    uint32_t linked = rrj_read32(actor + 852);
    int32_t count = 0;
    int32_t distance;
    int32_t threshold;
    int32_t index;

    FUNCTION_MARKER(0x8009FAD8, "RASHCDG.BIN");
    if (rrj_read32(linked + 604) >= 3u)
        actor = linked;
    if (rrj_read32(actor + 176) != 0xFFFFFFFFu)
    {
        rrj_write32(output, rrj_read32(actor + 176));
        count = 1;
    }
    if (rrj_u16(rrj_at(actor + 362, 2)))
        return (uint32_t)count;

    distance = rrj_s32(sub_8003A5F4(actor + 360, frame + 0x128, 1));
    threshold = 0x960000;
    for (index = 0; index < 2; ++index, threshold += 0x640000)
    {
        uint32_t road;
        int32_t position;

        if (distance < threshold)
            continue;
        rrj_put16(rrj_at(frame + 0x10, 2), 192);
        rrj_write32(frame + 0xCC, rrj_read32(actor + 360));
        rrj_write32(frame + 0xD0, rrj_read32(actor + 364));
        position = rrj_s32(rrj_read32(actor + 368));
        if (rrj_s32(rrj_read32(actor + 364)) > 0)
            position += threshold;
        else
            position -= threshold;
        rrj_write32(frame + 0xD4, (uint32_t)position);
        road = sub_80030410(frame + 0x10, 0);
        rrj_write32(frame + 0x14, road);
        if (!sub_8009FC4C(road, output, count))
        {
            rrj_write32(output + 4u * (uint32_t)count, road);
            ++count;
        }
    }
    return (uint32_t)count;
}

uint32_t sub_8009C4FC(uint32_t record, uint32_t candidates, int32_t count, uint32_t player)
{
    const uint32_t position = 0x801FFE20u;
    uint32_t road;

    FUNCTION_MARKER(0x8009C4FC, "RASHCDG.BIN");
    if ((int16_t)rrj_u16(rrj_at(record + 6, 2)) >= 0)
    {
        uint32_t descriptor = sub_80039A08((uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(record + 6, 2)));

        if (!descriptor)
            return 0;
        road = rrj_read32(descriptor + 12);
    }
    else
    {
        if ((rrj_read32(record + 8) & 0xFFFE0000u) && !sub_8009F288(record, candidates, count))
            return 0;
        rrj_write32(position, rrj_read32(record + 8));
        rrj_write32(position + 4, (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(record + 60, 2)));
        rrj_write32(position + 8, rrj_read32(record + 36));
        road = sub_80039DFC(0, position);
    }
    if (!road)
        return 0;
    if ((int16_t)rrj_u16(rrj_at(record + 6, 2)) < 0)
        rrj_put16(rrj_at(record + 6, 2), rrj_u16(rrj_at(road, 2)));
    return sub_80013110(rrj_u16(rrj_at(record, 2)), record + 20, player, 0, 0);
}

uint32_t sub_8009C5E4(int32_t index)
{
    int32_t group = (int16_t)rrj_u16(rrj_at(0x800CE592, 2));
    uint32_t descriptor;
    uint32_t item;

    FUNCTION_MARKER(0x8009C5E4, "RASHCDG.BIN");
    if (group == -1)
        return 0xFFFFFFFFu;
    descriptor = 0x800CE1B0u + 16u * (uint32_t)group;
    if (index >= r_u8(descriptor + 4))
        return 0xFFFFFFFFu;
    item = rrj_read32(rrj_read32(descriptor + 8) + 12u * (uint32_t)index);
    return (rrj_u16(rrj_at(item + 14, 2)) & 0xF80u) >> 7;
}

uint32_t sub_800A1318(uint32_t actor, uint32_t motion, uint32_t basis)
{
    uint32_t position = actor + 184;
    uint32_t result;

    FUNCTION_MARKER(0x800A1318, "RASHCDG.BIN");
    (void)sub_8002ECB8(rrj_at(basis + 12, 6), rrj_at(basis, 6), rrj_at(position, 12), rrj_read32(motion + 8), rrj_read32(motion));
    (void)sub_8002EAD8(rrj_at(position, 12), rrj_at(basis + 6, 6), rrj_read32(motion + 4), rrj_at(position, 12));
    rrj_write32(actor + 176, 0xFFFFFFFFu);
    rrj_write32(position, rrj_read32(position) + rrj_read32(actor + 224));
    result = rrj_read32(position + 4) + rrj_read32(actor + 228);
    rrj_write32(position + 4, result);
    rrj_write32(position + 8, rrj_read32(position + 8) + rrj_read32(actor + 232));
    return result;
}

uint32_t sub_800A0A20(uint32_t selector, uint32_t mode, uint32_t position, uint32_t rotation, uint32_t config, RRJRaceLeafCall call)
{
    uint32_t actor = rrj_read32(0x8005B304);
    int32_t slot;

    FUNCTION_MARKER(0x800A0A20, "RASHCDG.BIN");
    for (slot = 0; slot < 3; ++slot, actor += 280)
        if (!rrj_u16(rrj_at(actor + 172, 2)))
            break;
    if (slot >= 3)
        return 0;

    rrj_put16(rrj_at(actor + 172, 2), (uint16_t)(slot + 189));
    rrj_write32(actor + 4, rrj_read32(0x8005B250) + 24u * (uint32_t)slot);
    (void)sub_8002FAD4(actor, 6, selector, 0, call);
    rrj_write32(actor + 276, config);
    if (r_s8(config) < 0)
    {
        int32_t direction = (sub_8001FC58() & 1u) ? 1 : -1;
        int32_t speed = (int32_t)(sub_8001FC58() % 0x30000u) + 196608;
        int32_t angle = (int32_t)(sub_8001FC58() & 0xFFFFu) + 0x8000;
        int32_t scaled;
        int32_t ratio;

        w_u8(config + 3, 2);
        rrj_write32(config + 0x90, 0);
        rrj_write32(config + 0x74, 0);
        rrj_write32(config + 0x58, (uint32_t)speed);
        rrj_write32(config + 4, (uint32_t)angle);
        scaled = (int32_t)sub_8001FC90(108134, speed);
        ratio = batch_signed_ratio(scaled, angle);
        rrj_write32(config + 0xE4, (uint32_t)(direction * ratio));
        rrj_write32(config + 0xAC, 0);
        rrj_write32(config + 0xC8, 0);
        rrj_write32(config + 0x5C, 0);
        rrj_write32(config + 0x78, 0);
        rrj_write32(config + 0x94, (uint32_t)(direction * speed));
        scaled = (int32_t)sub_8001FC90(108134, direction * speed);
        ratio = batch_signed_ratio(scaled, angle);
        rrj_write32(config + 0xB0, (uint32_t)(-direction * ratio));
        rrj_write32(config + 0xE8, 0);
        rrj_write32(config + 0xCC, 0);
        scaled = -direction * ((int32_t)(sub_8001FC58() % 0xE4u) + 227);
        rrj_write32(config + 0x120, (uint32_t)scaled);
        rrj_write32(config + 0x11C, (uint32_t)scaled);
    }

    rrj_write32(actor + 224, rrj_read32(position));
    rrj_write32(actor + 228, rrj_read32(position + 4));
    rrj_write32(actor + 232, rrj_read32(position + 8));
    if (rotation)
    {
        (void)sub_8003FA18(9, rotation, actor + 244);
        (void)sub_8003FA18(9, actor + 244, actor + 196);
    }
    else
    {
        int32_t index;

        for (index = 0; index < 18; index += 2)
        {
            rrj_put16(rrj_at(actor + 196 + (uint32_t)index, 2), 0);
            rrj_put16(rrj_at(actor + 244 + (uint32_t)index, 2), 0);
        }
        rrj_put16(rrj_at(actor + 196, 2), 4096);
        rrj_put16(rrj_at(actor + 204, 2), 4096);
        rrj_put16(rrj_at(actor + 212, 2), 4096);
        rrj_put16(rrj_at(actor + 244, 2), 4096);
        rrj_put16(rrj_at(actor + 252, 2), 4096);
        rrj_put16(rrj_at(actor + 260, 2), 4096);
    }
    w_u8(config + 1, (uint8_t)mode);
    if (!mode)
    {
        rrj_write32(actor + 236, sub_80020018((uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(actor + 256, 2)), (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(actor + 260, 2))));
        rrj_write32(actor + 240, 0u - sub_8001FF3C((uint32_t)((int32_t)(int16_t)rrj_u16(rrj_at(actor + 258, 2)) << 4)));
    }
    w_u8(actor + 262, 0xFFu);
    rrj_write32(actor + 264, 0);
    rrj_write32(actor + 268, 0);
    rrj_write32(actor + 272, 0);
    {
        const uint32_t motion = 0x801FFBE0u;

        rrj_write32(motion, rrj_read32(config + 88));
        rrj_write32(motion + 4, rrj_read32(config + 116));
        rrj_write32(motion + 8, rrj_read32(config + 144));
        (void)sub_800A1318(actor, motion, actor + 196);
    }
    rrj_write32(actor + 176, 0xFFFFFFFFu);
    rrj_write32(0x8005B314, rrj_read32(0x8005B314) + 1);
    return actor;
}

uint32_t sub_800A2448(uint32_t config, uint32_t other, RRJRaceLeafCall call)
{
    const uint32_t pool = 0x800CE598u;
    int32_t current = rrj_s32(rrj_read32(pool + 4));
    int32_t maximum = rrj_s32(rrj_read32(pool + 8));
    uint32_t actor = 0;

    FUNCTION_MARKER(0x800A2448, "RASHCDG.BIN");
    if (current < maximum || rrj_read32(0x800D1814) >= 452u)
    {
        int32_t selected = current;
        int32_t next = current + 1;
        uint32_t candidate = rrj_read32(pool + 12) - 452u * (uint32_t)next;
        int32_t task_index;
        uint32_t task;

        while (next < maximum + 1)
        {
            if (!rrj_u16(rrj_at(candidate + 172, 2)))
                break;
            ++next;
            candidate -= 452;
        }
        rrj_write32(pool + 4, (uint32_t)next);
        actor = rrj_read32(pool + 12) - 452u * (uint32_t)selected;
        rrj_put16(rrj_at(actor + 172, 2), (uint16_t)(selected + 160));

        task_index = rrj_s32(rrj_read32(0x800D1660)) + 1;
        task = 0x800D1664u + 24u * (rrj_read32(0x800D1660) + 1u);
        while (task_index < 18)
        {
            if (!rrj_read32(task))
                break;
            ++task_index;
            task += 24;
        }
        task = 0x800D1664u + 24u * rrj_read32(0x800D1660);
        rrj_write32(0x800D1660, (uint32_t)task_index);
        rrj_write32(actor + 4, task);
        rrj_write32(pool, rrj_read32(pool) + 1);
        if (maximum < selected)
        {
            rrj_write32(pool + 8, (uint32_t)selected);
            rrj_write32(0x800D1814, rrj_read32(0x800D1814) - 452);
        }
    }
    if (!actor || !sub_800A07D0(config, actor, 5, other, call))
        return 0;
    {
        int32_t value = (int32_t)sub_8001FC90(rrj_s32(rrj_read32(actor + 308)), rrj_s32(rrj_read32(actor + 304)));

        value = rrj_s32((uint32_t)sub_8001FC90(rrj_s32(rrj_read32(actor + 312)), value) << 10);
        if (value > 0x1000000)
            value = 0x1000000;
        rrj_write32(actor + 316, (uint32_t)value);
    }
    return actor;
}

uint32_t sub_800CAAA8(uint32_t actor)
{
    FUNCTION_MARKER(0x800CAAA8, "RASHCDG.BIN");
    if (r_u8(actor + 564) == 2)
        return sub_800CA7C4(actor, 0);
    return sub_800CA564(actor, 0);
}

uint32_t sub_800CB78C(uint32_t actor, uint32_t kind, uint32_t identity)
{
    uint32_t used = rrj_read32(0x8005AD50);
    uint32_t slot = 0;
    uint32_t child = 0x800CF018u;

    FUNCTION_MARKER(0x800CB78C, "RASHCDG.BIN");
    if (used == 0xFFu)
        return 0;
    while (used & (1u << slot))
    {
        ++slot;
        child += 172;
        if (slot >= 8)
            return 0;
    }
    rrj_write32(0x8005AD50, used | (1u << slot));
    w_u8(actor + 567, (uint8_t)slot);
    if ((int8_t)r_u8(child + 8) != (int32_t)identity)
        (void)sub_8001298C(child, identity);
    (void)sub_80012838(actor, child, kind, 0);
    return 1;
}
