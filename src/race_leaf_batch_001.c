#include "native_game_api.h"
#include "race_leaf_batch_001.h"
#include "race_leaf_batch_000.h"
#include "draft_quick.h"
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

uint32_t sub_8003FA18(int32_t count, const uint16_t *source, uint16_t *destination)
{
    uint32_t value = 0;

    FUNCTION_MARKER(0x8003FA18, "SLUS_010.53");
    while (count > 0)
    {
        --count;
        value = xport_load_le16(source);
        ++source;
        xport_store_le16(destination, value);
        ++destination;
    }
    /* The original leaves its return value unspecified for nonpositive counts */
    return value;
}

uint32_t sub_8009FC4C(uint32_t value, uint32_t values, int32_t count)
{
    int32_t index;

    FUNCTION_MARKER(0x8009FC4C, "RASHCDG.BIN");
    for (index = 0; index < count; ++index)
        if (r_u32(values + 4u * (uint32_t)index) == value)
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
    actor = r_u32(0x8005B268u + 4u * player);
    if (r_u16(actor + 362) == 1)
        return 0;
    for (index = 0; index < 4; ++index, candidates += 12)
    {
        int32_t candidate_id = ((int32_t)(uint32_t)(r_u32(candidates)));
        int32_t center;
        int32_t first;
        int32_t second;
        int32_t sign;

        if (candidate_id < 0)
            break;
        if ((uint32_t)candidate_id != r_u16(actor + 360))
            continue;
        center = ((int32_t)(uint32_t)(r_u32(actor + 368))) >> 10;
        first = ((int32_t)(uint32_t)(r_u32(candidates + 4) - (uint32_t)center));
        sign = first >> 31;
        first = ((int32_t)(uint32_t)(((uint32_t)first + (uint32_t)sign) ^ (uint32_t)sign));
        if (first < 19201)
            continue;
        second = ((int32_t)(uint32_t)(r_u32(candidates + 8) - (uint32_t)center));
        sign = second >> 31;
        second = ((int32_t)(uint32_t)(((uint32_t)second + (uint32_t)sign) ^ (uint32_t)sign));
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
    context = r_u32(0x8005B2F8);
    if (!(r_u8(context + 4) & 0x10) || !record)
        return r_u8(context + 4) & 0x10;
    tables = r_u32(r_u32(record + 4) + 32);
    switch (kind)
    {
        case 0:
            target = r_u32(tables + 52) + 64u * (index & 0xFFu);
            break;
        case 2:
            target = r_u32(tables + 40) + 76u * (index & 0xFFu);
            break;
        case 3:
            target = r_u32(tables + 36) + 68u * (index & 0xFFu);
            break;
        case 4:
            target = r_u32(tables + 44) + 64u * (index & 0xFFu);
            break;
        case 6:
            target = r_u32(tables + 48) + 88u * (index & 0xFFu);
            break;
        default:
            return 0x80060000u;
    }
    w_u16(target + 4, (uint16_t)value);
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
    maximum = ((int32_t)(uint32_t)(r_u32(0x8005B31C)));
    for (index = 0; index <= maximum; ++index)
    {
        uint32_t value = r_u32(0x800D4B10u + 16u * (uint32_t)index);

        if (value == 0xFFFFFFFFu)
            continue;
        if (count >= limit)
            return (uint32_t)count;
        w_u32(output + 4u * (uint32_t)count, value);
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
    if (!record || !(road = r_u32(record + 12)))
        return 0;
    count = (int16_t)r_u16(road + 30);
    entries = r_u32(road + 72);
    for (index = 0; index < count; ++index, entries += 20)
        if (r_u32(entries) == identity)
            return entries;
    return 0;
}

uint32_t sub_8008D9E4(uint32_t player)
{
    FUNCTION_MARKER(0x8008D9E4, "RASHCDG.BIN");
    if (r_u32(r_u32(0x8005B2F8) + 48) != 2)
        return 1;
    return ((int32_t)(uint32_t)(r_u32(0x800CD6B8u + 4u * player))) < 16;
}

uint32_t sub_8008DECC(uint32_t player, uint32_t group)
{
    uint32_t state = 0x800D5CF8u + 32u * player;
    uint32_t group_state = state + 4u * group;
    int32_t count = ((int32_t)(uint32_t)(r_u32(group_state + 8)));
    uint32_t identity = 224;

    FUNCTION_MARKER(0x8008DECC, "RASHCDG.BIN");
    if (count > 0)
    {
        int32_t tail = ((int32_t)(uint32_t)(r_u32(group_state + 20)));

        if (tail != -1)
        {
            uint32_t entry = r_u32(state + 4) + 8u * (uint32_t)tail;

            w_u32(group_state + 20, (uint32_t)(int32_t)r_s16(entry + 2));
            w_u16(entry + 2, 0xFFFFu);
            w_u32(entry + 4, 0);
            identity = r_u16(r_u32(0x800CD6C4) + 280u * (uint32_t)tail);
        }
        w_u32(group_state + 8, (uint32_t)(count - 1));
    }
    return identity;
}

uint32_t sub_8008DA20(uint32_t identity, uint32_t player)
{
    uint32_t player_state = 0x800CD898u + 1132u * player;
    int32_t first = (int16_t)r_u16(player_state + 194) - (int16_t)r_u16(identity + 30);
    int32_t second = (int16_t)r_u16(player_state + 186) - (int16_t)r_u16(identity + 22);
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
    if (distance < (((int32_t)(uint32_t)(r_u32(0x8005AD58))) >> 18))
        group = ((int32_t)(uint32_t)(r_u32(0x800D5D08u + 32u * player))) > 0 ? 2 : 1;
    else if (distance < (((int32_t)(uint32_t)(r_u32(0x8005AD58))) >> 16))
        group = 2;
    if (group != -1)
        removed = sub_8008DECC(player, (uint32_t)group);
    if ((uint16_t)(removed - 1u) < 223u)
    {
        uint32_t object = r_u32(0x800CD6C4) + 280u * (removed & 31u);

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
    int32_t count = ((int32_t)(uint32_t)(r_u32(r_u32(0x800CE4DC))));
    uint32_t actor = r_u32(0x800CE4D0);
    uint32_t stride = r_u32(0x800CE4D4);
    uint32_t players = r_u32(r_u32(0x8005B2F8) + 48);

    FUNCTION_MARKER(0x80095848, "RASHCDG.BIN");
    while (count >= 0)
    {
        if (actor && r_u16(actor + 172) >= players && (r_u8(r_u32(actor + 1084) + 1) & 15u) == 2 && !r_u16(actor + 320) && !(r_u8(actor + 928) & 0x10u))
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
    w_u32(scratch, r_u32(record + 8));
    w_u32(scratch + 4, (uint32_t)(int32_t)r_s16(record + 60));
    w_u32(scratch + 8, r_u32(record + 36));
    road = sub_80039DFC(0, ((scratch) ? (const uint32_t[3]){r_u32(scratch), 0, (uint32_t)r_u16((scratch) + 10) << 16} : NULL));
    if (!road)
        return 0;

    selected = ((int32_t)(uint32_t)(r_u32(0x800CD6AC)));
    if (r_u32(0x800CD6C4) && selected < ((int32_t)(uint32_t)(r_u32(0x8005B214))))
    {
        int32_t next = selected + 1;
        uint32_t candidate = r_u32(0x800CD6C4) + 280u * (uint32_t)next;

        while (next < ((int32_t)(uint32_t)(r_u32(0x8005B214))))
        {
            if (!r_u16(candidate))
                break;
            ++next;
            candidate += 280;
        }
        w_u32(0x800CD6AC, (uint32_t)next);
        object = r_u32(0x800CD6C4) + 280u * (uint32_t)selected;
        w_u16(object, (uint16_t)(selected + 192));
        w_u32(0x800CD6A8, r_u32(0x800CD6A8) + 1);
        if (((int32_t)(uint32_t)(r_u32(0x800CD6B0))) < selected)
            w_u32(0x800CD6B0, (uint32_t)selected);
    }
    if (!object)
        return 0;

    w_u16(object + 150, r_u16(record + 62));
    uint32_t geometry_result = 0;
    {
        const uint32_t route_position[3] = {r_u32(scratch), r_u32(scratch + 4), r_u32(scratch + 8)};
        uint32_t route_geometry[8] = {r_u32(scratch + 16), r_u32(scratch + 16 + 4), r_u32(scratch + 16 + 8), r_u32(scratch + 16 + 12), r_u32(scratch + 16 + 16), r_u32(scratch + 16 + 20), r_u32(scratch + 16 + 24), r_u32(scratch + 16 + 28)};
        geometry_result = sub_8003A700(road, route_position, route_geometry);
        w_u32(scratch + 16, route_geometry[0]);
        w_u32(scratch + 16 + 4, route_geometry[1]);
        w_u32(scratch + 16 + 8, route_geometry[2]);
        w_u32(scratch + 16 + 12, route_geometry[3]);
        w_u32(scratch + 16 + 16, route_geometry[4]);
        w_u32(scratch + 16 + 20, route_geometry[5]);
        w_u32(scratch + 16 + 24, route_geometry[6]);
        w_u32(scratch + 16 + 28, route_geometry[7]);
    }
    if (!geometry_result)
        goto cleanup;
    if ((r_u32(scratch) >> 16) == 1)
        (void)sub_8003C758(scratch + 16, record + 20);
    (void)sub_8001E0B4(xport_guest_ptr(object + 156, 32), xport_guest_ptr(scratch + 16, 1), 32);
    basis = r_u32(object + 168);
    w_u32(object + 12, r_u32(record + 20));
    w_u32(object + 16, r_u32(record + 24));
    w_u32(object + 20, r_u32(record + 28));
    sub_80036800(object + 12, basis, xport_guest_ptr(object + 172, 4), xport_guest_ptr(object + 176, 4));
    w_u16(scratch + 48, 0);
    w_u16(scratch + 50, 0);
    w_u16(scratch + 52, 0);
    {
        const uint16_t route_direction[3] = {r_u16(scratch + 48), r_u16(scratch + 48 + 2), r_u16(scratch + 48 + 4)};
        const uint32_t route_record[8] = {r_u32(object + 156), r_u32(object + 156 + 4), r_u32(object + 156 + 8), r_u32(object + 156 + 12), r_u32(object + 156 + 16), r_u32(object + 156 + 20), r_u32(object + 156 + 24), r_u32(object + 156 + 28)};
        uint32_t route_position[3] = {r_u32(object + 188), r_u32(object + 188 + 4), r_u32(object + 188 + 8)};
        uint32_t route_result = sub_8003662C(route_direction, route_record, route_position);
        w_u32(object + 188, route_position[0]);
        w_u32(object + 188 + 4, route_position[1]);
        w_u32(object + 188 + 8, route_position[2]);
        (void)route_result;
    }
    w_u32(object + 192, (uint32_t)(int32_t)r_s16(record + 60));
    sub_8003AF9C(object, 0, player);
    w_u32(object + 152, sub_8003B61C(object));
    w_u32(object + 12, r_u32(record + 20));
    w_u32(object + 16, r_u32(record + 24));
    w_u32(object + 20, r_u32(record + 28));
    w_u32(object + 8, r_u16(record + 2));

    flags = r_u16(record + 12);
    if (flags & 1u)
    {
        w_u16(object + 260, r_u16(basis + 14));
        w_u16(object + 262, r_u16(basis + 16));
        w_u16(object + 264, r_u16(basis + 18));
        w_u16(object + 266, (uint16_t)-(int16_t)r_u16(basis + 8));
        w_u16(object + 268, (uint16_t)-(int16_t)r_u16(basis + 10));
        w_u16(object + 270, (uint16_t)-(int16_t)r_u16(basis + 12));
        w_u16(object + 272, r_u16(basis + 2));
        w_u16(object + 274, r_u16(basis + 4));
        w_u16(object + 276, r_u16(basis + 6));
        if (((int32_t)(uint32_t)(r_u32(object + 172))) >= 0)
            for (index = 272; index <= 276; index += 2)
                w_u16(object + (uint32_t)index, (uint16_t)-(int16_t)r_u16(object + (uint32_t)index));
        else
            for (index = 260; index <= 264; index += 2)
                w_u16(object + (uint32_t)index, (uint16_t)-(int16_t)r_u16(object + (uint32_t)index));
    }
    else
    {
        for (index = 260; index <= 276; index += 2)
            w_u16(object + (uint32_t)index, 0);
        w_u16(object + 260, 4096);
        w_u16(object + 268, 4096);
        w_u16(object + 276, 4096);
    }

    if (r_u32(object + 8) == 1)
    {
        int32_t half_width = ((int32_t)(uint32_t)(r_u32(record + 64)));

        w_u32(object + 132, (uint32_t)half_width);
        w_u32(object + 136, (uint32_t)half_width);
        w_u32(object + 140, r_u32(record + 80));
        w_u32(object + 84, r_u32(record + 76));
        w_u32(object + 88, r_u32(record + 68));
        (void)sub_8002ECB8(xport_guest_ptr(object + 260, 6), xport_guest_ptr(object + 272, 6), xport_guest_ptr(object + 24, 12), (uint32_t)-half_width, 0u - r_u32(object + 136));
        for (index = 0; index < 3; ++index)
            w_u32(object + 48u + 4u * (uint32_t)index, 0u - r_u32(object + 24u + 4u * (uint32_t)index));
        (void)sub_8002ECB8(xport_guest_ptr(object + 260, 6), xport_guest_ptr(object + 272, 6), xport_guest_ptr(object + 36, 12), (uint32_t)half_width, 0u - r_u32(object + 136));
        for (index = 0; index < 3; ++index)
            w_u32(object + 60u + 4u * (uint32_t)index, 0u - r_u32(object + 36u + 4u * (uint32_t)index));
        for (index = 0; index < 4; ++index)
        {
            uint32_t vertex = object + 24u + 12u * (uint32_t)index;

            w_u32(vertex, r_u32(vertex) + r_u32(object + 12));
            w_u32(vertex + 4, r_u32(vertex + 4) + r_u32(object + 16));
            w_u32(vertex + 8, r_u32(vertex + 8) + r_u32(object + 20));
        }
    }
    else
    {
        int32_t half_width = ((int32_t)(uint32_t)(r_u32(record + 76) - r_u32(record + 64))) >> 1;
        int32_t half_length = ((int32_t)(uint32_t)(r_u32(record + 84) - r_u32(record + 72))) >> 1;

        w_u32(object + 132, (uint32_t)half_width);
        w_u32(object + 140, r_u32(record + 80) - r_u32(record + 68));
        w_u32(object + 136, (uint32_t)half_length);
        if (flags & 2u)
        {
            w_u16(object + 272, r_u16(record + 14));
            w_u16(object + 274, r_u16(record + 16));
            w_u16(object + 276, r_u16(record + 18));
            if (flags & 1u)
            {
                int32_t ax = (int16_t)r_u16(object + 266);
                int32_t ay = (int16_t)r_u16(object + 268);
                int32_t az = (int16_t)r_u16(object + 270);
                int32_t bx = (int16_t)r_u16(object + 272);
                int32_t by = (int16_t)r_u16(object + 274);
                int32_t bz = (int16_t)r_u16(object + 276);

                w_u16(object + 260, (uint16_t)((ay * bz - az * by) >> 12));
                w_u16(object + 262, (uint16_t)((az * bx - ax * bz) >> 12));
                w_u16(object + 264, (uint16_t)((ax * by - ay * bx) >> 12));
                (void)sub_8002E468(xport_guest_ptr(object + 260, 6));
            }
            else
            {
                w_u16(object + 260, r_u16(record + 18));
                w_u16(object + 264, (uint16_t)-(int16_t)r_u16(record + 14));
            }
        }
        (void)sub_8002EAD8(xport_guest_ptr(object + 12, 12), xport_guest_ptr(object + 266, 6), 0x8000, xport_guest_ptr(object + 12, 12));
        (void)sub_8002ECB8(xport_guest_ptr(object + 260, 6), xport_guest_ptr(object + 272, 6), xport_guest_ptr(object + 24, 12), (uint32_t)-half_width, (uint32_t)-half_length);
        (void)sub_8002ECB8(xport_guest_ptr(object + 260, 6), xport_guest_ptr(object + 272, 6), xport_guest_ptr(object + 36, 12), (uint32_t)half_width, (uint32_t)-half_length);
        (void)sub_8002ECB8(xport_guest_ptr(object + 260, 6), xport_guest_ptr(object + 272, 6), xport_guest_ptr(object + 48, 12), (uint32_t)half_width, (uint32_t)half_length);
        (void)sub_8002ECB8(xport_guest_ptr(object + 260, 6), xport_guest_ptr(object + 272, 6), xport_guest_ptr(object + 60, 12), (uint32_t)-half_width, (uint32_t)half_length);
        for (index = 0; index < 4; ++index)
        {
            uint32_t vertex = object + 24u + 12u * (uint32_t)index;

            w_u32(vertex, r_u32(vertex) + r_u32(object + 12));
            w_u32(vertex + 4, r_u32(vertex + 4) + r_u32(object + 16));
            w_u32(vertex + 8, r_u32(vertex + 8) + r_u32(object + 20));
            (void)sub_8002EAD8(xport_guest_ptr(vertex, 12), xport_guest_ptr(object + 266, 6), 0xFFFF8000u - r_u32(object + 140), xport_guest_ptr(object + 72u + 12u * (uint32_t)index, 12));
        }
    }
    {
        uint32_t angle = sub_80020018((uint32_t)((int32_t)r_s16(object + 272) << 4), (uint32_t)((int32_t)r_s16(object + 276) << 4));
        int32_t value;

        w_u32(object + 120, angle);
        w_u32(object + 124, (uint32_t)((int32_t)r_s16(0x8005624Eu + 4u * (angle & 0xFFFu)) << 4));
        w_u32(object + 128, (uint32_t)((int32_t)r_s16(0x8005624Cu + 4u * (angle & 0xFFFu)) << 4));
        value = 4 * (int32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(object + 140))), (int32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(object + 136))), ((int32_t)(uint32_t)(r_u32(object + 132)))));
        if (value > 258867)
            value = 258867;
        w_u32(object + 144, (uint32_t)value << 9);
    }
    w_u32(object + 4, 0xFFFFFFFFu);
    w_u16(object + 148, (uint16_t)sub_80039F68(object));
    if (!r_u16(object + 148))
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
    if (!record || !(road = r_u32(record + 12)))
        return 0;
    entry_count = (int16_t)r_u16(road + 30);
    entry = r_u32(road + 72);
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
            int32_t candidate_id = ((int32_t)(uint32_t)(r_u32(candidate)));
            uint32_t bounds;

            if (candidate_id < 0)
                break;
            if (r_u32(entry + 4) != (uint32_t)candidate_id)
                continue;
            bounds = (int16_t)r_u16(road + 16) == 1 ? sub_80039C90(road, r_u32(entry + 4)) : r_u32(road + 44);
            if (bounds && ((int32_t)(uint32_t)(r_u32(candidate + 8))) >= (((int32_t)(uint32_t)(r_u32(bounds + 24))) >> 10) && (((int32_t)(uint32_t)(r_u32(bounds + 28))) >> 10) >= ((int32_t)(uint32_t)(r_u32(candidate + 4))))
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
        uint32_t road_id = r_u32(roads + 4u * (uint32_t)road_index);

        if ((uint8_t)sub_8009F11C(((int32_t)(uint32_t)(road_id)), candidates) && count < limit)
        {
            w_u32(output + 4u * (uint32_t)count, road_id);
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
        uint32_t entry = sub_8003C494(r_u32(record + 8), r_u32(road_ids + 4u * (uint32_t)index));

        if (entry)
        {
            int32_t direction = ((int32_t)(uint32_t)(r_u32(record + 32)));
            int32_t position;

            if (r_u32(entry + 12))
            {
                direction = -direction;
                position = ((int32_t)(uint32_t)((r_u32(entry + 8) + r_u32(entry + 16)) << 10)) - ((int32_t)(uint32_t)(r_u32(record + 36)));
            }
            else
                position = ((int32_t)(uint32_t)(r_u32(entry + 8) << 10)) + ((int32_t)(uint32_t)(r_u32(record + 36)));
            if (position >= 0 && ((int32_t)(uint32_t)((r_u32(entry + 8) + r_u32(entry + 16)) << 10)) >= position)
            {
                w_u32(record + 36, (uint32_t)position);
                w_u32(record + 32, (uint32_t)direction);
                w_u32(record + 8, r_u16(entry + 4));
                return 1;
            }
            {
                uint32_t road = sub_800245DC(r_u32(entry + 4));

                if (road)
                {
                    uint32_t next = r_u16(road + (position < 0 ? 8u : 12u));

                    w_u32(record + 8, next | 0x10000u);
                    w_u32(record + 36, 0);
                    w_u32(record + 32, 0);
                }
            }
        }
    }
    return 0;
}

uint32_t sub_8009FAD8(uint32_t output, uint32_t player)
{
    const uint32_t frame = 0x801FFC00u;
    uint32_t actor = r_u32(0x8005B268u + 4u * player);
    uint32_t linked = r_u32(actor + 852);
    int32_t count = 0;
    int32_t distance;
    int32_t threshold;
    int32_t index;

    FUNCTION_MARKER(0x8009FAD8, "RASHCDG.BIN");
    if (r_u32(linked + 604) >= 3u)
        actor = linked;
    if (r_u32(actor + 176) != 0xFFFFFFFFu)
    {
        w_u32(output, r_u32(actor + 176));
        count = 1;
    }
    if (r_u16(actor + 362))
        return (uint32_t)count;

    {
        uint32_t nearest_id;
        distance = (int32_t)sub_8003A5F4(actor + 360, &nearest_id, 1);
        w_u32(frame + 0x128, nearest_id);
    }
    threshold = 0x960000;
    for (index = 0; index < 2; ++index, threshold += 0x640000)
    {
        uint32_t road;
        int32_t position;

        if (distance < threshold)
            continue;
        w_u16(frame + 0x10, 192);
        w_u32(frame + 0xCC, r_u32(actor + 360));
        w_u32(frame + 0xD0, r_u32(actor + 364));
        position = ((int32_t)(uint32_t)(r_u32(actor + 368)));
        if (((int32_t)(uint32_t)(r_u32(actor + 364))) > 0)
            position += threshold;
        else
            position -= threshold;
        w_u32(frame + 0xD4, (uint32_t)position);
        road = sub_80030410(frame + 0x10, 0);
        w_u32(frame + 0x14, road);
        if (!sub_8009FC4C(road, output, count))
        {
            w_u32(output + 4u * (uint32_t)count, road);
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
    if ((int16_t)r_u16(record + 6) >= 0)
    {
        uint32_t descriptor = sub_80039A08((uint32_t)(int32_t)r_s16(record + 6));

        if (!descriptor)
            return 0;
        road = r_u32(descriptor + 12);
    }
    else
    {
        if ((r_u32(record + 8) & 0xFFFE0000u) && !sub_8009F288(record, candidates, count))
            return 0;
        w_u32(position, r_u32(record + 8));
        w_u32(position + 4, (uint32_t)(int32_t)r_s16(record + 60));
        w_u32(position + 8, r_u32(record + 36));
        road = sub_80039DFC(0, ((position) ? (const uint32_t[3]){r_u32(position), 0, (uint32_t)r_u16((position) + 10) << 16} : NULL));
    }
    if (!road)
        return 0;
    if ((int16_t)r_u16(record + 6) < 0)
        w_u16(record + 6, r_u16(road));
    return sub_80013110(r_u16(record), record + 20, player, 0, 0);
}

uint32_t sub_8009C5E4(int32_t index)
{
    int32_t group = (int16_t)r_u16(0x800CE592);
    uint32_t descriptor;
    uint32_t item;

    FUNCTION_MARKER(0x8009C5E4, "RASHCDG.BIN");
    if (group == -1)
        return 0xFFFFFFFFu;
    descriptor = 0x800CE1B0u + 16u * (uint32_t)group;
    if (index >= r_u8(descriptor + 4))
        return 0xFFFFFFFFu;
    item = r_u32(r_u32(descriptor + 8) + 12u * (uint32_t)index);
    return (r_u16(item + 14) & 0xF80u) >> 7;
}

uint32_t sub_800A1318(uint32_t actor, const uint32_t motion[3], const uint16_t basis[9])
{
    uint32_t *position = xport_guest_ptr(actor + 184, 12);
    uint32_t lateral, forward, height;
    uint32_t position_x, base_x, position_y, base_z, base_y, position_z;

    FUNCTION_MARKER(0x800A1318, "RASHCDG.BIN");
    lateral = motion[0];
    forward = motion[2];
    (void)sub_8002ECB8(basis + 6, basis, position, forward, lateral);
    height = motion[1];
    (void)sub_8002EAD8(position, basis + 3, height, position);
    position_x = r_u32(actor + 184);
    base_x = r_u32(actor + 224);
    w_u32(actor + 176, 0xFFFFFFFF);
    position_y = r_u32(actor + 188);
    base_z = r_u32(actor + 232);
    position_x += base_x;
    base_y = r_u32(actor + 228);
    w_u32(actor + 184, position_x);
    position_z = r_u32(actor + 192);
    position_y += base_y;
    position_z += base_z;
    w_u32(actor + 188, position_y);
    w_u32(actor + 192, position_z);
    return position_y;
}

uint32_t sub_800A0A20(uint32_t selector, uint32_t mode, const uint32_t position[3], const uint16_t rotation[9], uint32_t config)
{
    uint32_t actor = r_u32(0x8005B304);
    int32_t slot = 0;
    uint32_t motion[3], motion_config, active_count;

    FUNCTION_MARKER(0x800A0A20, "RASHCDG.BIN");
    while (r_u16(actor + 172))
    {
        ++slot;
        if (slot >= 3)
            return 0;
        actor += 280;
    }
    w_u16(actor + 172, (uint16_t)(slot + 189));
    w_u32(actor + 4, r_u32(0x8005B250) + 24 * (uint32_t)slot);
    (void)sub_8002FAD4(actor, 6, selector, 0);
    w_u32(actor + 276, config);
    if (r_s8(config) < 0)
    {
        uint32_t direction, random, quotient, speed, angle, derivative;
        uint32_t scaled, divisor, negative_direction;
        int positive_numerator, positive_divisor;

        w_u8(config + 3, 2);
        direction = (sub_8001FC58() & 1) ? 1 : 0xFFFFFFFF;
        random = sub_8001FC58();
        quotient = (uint32_t)(((uint64_t)random * 0xAAAAAAAB) >> 32) >> 17;
        w_u32(config + 144, 0);
        w_u32(config + 116, 0);
        speed = random - quotient * 196608 + 196608;
        w_u32(config + 88, speed);
        random = sub_8001FC58();
        angle = (random & 0xFFFF) + 0x8000;
        speed = r_u32(config + 88);
        w_u32(config + 4, angle);
        scaled = (uint32_t)(sub_8001FC90(108134, speed));
        positive_numerator = (int32_t)scaled > 0;
        positive_divisor = r_s32(config + 4) > 0;
        scaled = (uint32_t)(sub_8001FC90(108134, r_u32(config + 88)));
        divisor = r_u32(config + 4);
        derivative = sub_80010028(positive_numerator ? scaled : 0 - scaled, positive_divisor ? divisor : 0 - divisor);
        if (positive_numerator != positive_divisor)
            derivative = 0 - derivative;
        derivative *= direction;
        speed = direction * r_u32(config + 88);
        negative_direction = 0 - direction;
        w_u32(config + 228, derivative);
        w_u32(config + 172, 0);
        w_u32(config + 200, 0);
        w_u32(config + 92, 0);
        w_u32(config + 120, 0);
        w_u32(config + 148, speed);
        scaled = (uint32_t)(sub_8001FC90(108134, speed));
        positive_numerator = (int32_t)scaled > 0;
        positive_divisor = r_s32(config + 4) > 0;
        scaled = (uint32_t)(sub_8001FC90(108134, r_u32(config + 148)));
        divisor = r_u32(config + 4);
        derivative = sub_80010028(positive_numerator ? scaled : 0 - scaled, positive_divisor ? divisor : 0 - divisor);
        if (positive_numerator != positive_divisor)
            derivative = 0 - derivative;
        w_u32(config + 176, negative_direction * derivative);
        w_u32(config + 232, 0);
        w_u32(config + 204, 0);
        random = sub_8001FC58();
        quotient = (uint32_t)(((uint64_t)(random >> 2) * 0x08FB823F) >> 32) >> 1;
        angle = negative_direction * (random - quotient * 228 + 227);
        w_u32(config + 288, angle);
        w_u32(config + 284, angle);
    }
    w_u32(actor + 224, position[0]);
    w_u32(actor + 228, position[1]);
    w_u32(actor + 232, position[2]);
    if (rotation)
    {
        (void)sub_8003FA18(9, rotation, xport_guest_ptr(actor + 244, 18));
        (void)sub_8003FA18(9, xport_guest_cptr(actor + 244, 18), xport_guest_ptr(actor + 196, 18));
    }
    else
    {
        w_u16(actor + 258, 0);
        w_u16(actor + 256, 0);
        w_u16(actor + 254, 0);
        w_u16(actor + 250, 0);
        w_u16(actor + 248, 0);
        w_u16(actor + 246, 0);
        w_u16(actor + 260, 4096);
        w_u16(actor + 252, 4096);
        w_u16(actor + 244, 4096);
        w_u16(actor + 210, 0);
        w_u16(actor + 208, 0);
        w_u16(actor + 206, 0);
        w_u16(actor + 202, 0);
        w_u16(actor + 200, 0);
        w_u16(actor + 198, 0);
        w_u16(actor + 212, 4096);
        w_u16(actor + 204, 4096);
        w_u16(actor + 196, 4096);
    }
    w_u8(config + 1, (uint8_t)mode);
    if (!mode)
    {
        uint32_t yaw = sub_80020018((uint32_t)(int32_t)r_s16(actor + 256), (uint32_t)(int32_t)r_s16(actor + 260));
        uint32_t pitch = (uint32_t)(int32_t)r_s16(actor + 258);
        w_u32(actor + 236, yaw);
        w_u32(actor + 240, 0 - sub_8001FF3C(pitch << 4));
    }
    motion_config = r_u32(actor + 276);
    w_u8(actor + 262, 0xFF);
    w_u32(actor + 268, 0);
    w_u32(actor + 272, 0);
    w_u32(actor + 264, 0);
    motion[0] = r_u32(motion_config + 88);
    motion[1] = r_u32(r_u32(actor + 276) + 116);
    motion[2] = r_u32(r_u32(actor + 276) + 144);
    (void)sub_800A1318(actor, motion, xport_guest_cptr(actor + 196, 18));
    active_count = r_u32(0x8005B314);
    w_u32(actor + 176, 0xFFFFFFFF);
    w_u32(0x8005B314, active_count + 1);
    return actor;
}

uint32_t sub_800A0EC4(uint32_t event, const uint32_t position[3], const uint16_t rotation[9])
{
    int32_t count = 0;
    int32_t current;
    uint32_t result;

    FUNCTION_MARKER(0x800A0EC4, "RASHCDG.BIN");
    do
    {
        result = (uint32_t)(int32_t)r_s8(event + 9 + (uint32_t)count);
        if ((int32_t)result < 0)
            break;
        ++count;
    } while (count < 3);
    for (current = 0; current < count; ++current)
    {
        uint32_t index = (uint32_t)(int32_t)r_s8(event + 9 + (uint32_t)current);
        uint32_t selector = (uint32_t)(int32_t)r_s8(event + 8);
        uint32_t mode = r_s16(event + 14) != 0;
        uint32_t config = r_u32(0x8005B24C) + 312 * index;
        (void)sub_800A0A20(selector, mode, position, rotation, config);
        result = current + 1 < count;
    }
    w_u8(event + 12, 0);
    return result;
}

uint32_t sub_800A0F9C(uint32_t config)
{
    uint32_t previous_position[3], previous_derivative[3];
    uint32_t changed, other, magnitude, sign, random, quotient;
    uint32_t raw, lower, upper, vertical, horizontal, scale, product, divisor, derivative;
    int positive_product, positive_divisor;

    FUNCTION_MARKER(0x800A0F9C, "RASHCDG.BIN");
    previous_position[0] = r_u32(config + 92);
    previous_position[1] = r_u32(config + 120);
    previous_position[2] = r_u32(config + 148);
    previous_derivative[0] = r_u32(config + 176);
    previous_derivative[1] = r_u32(config + 204);
    previous_derivative[2] = r_u32(config + 232);
    if (r_u32(config + 92))
    {
        changed = config;
        other = config + 56;
    }
    else
    {
        changed = config + 56;
        other = config;
    }
    magnitude = r_u32(changed + 92);
    sign = (uint32_t)((int32_t)magnitude >> 31);
    magnitude = (magnitude + sign) ^ sign;
    if (r_s32(config + 88) > 0)
    {
        random = sub_8001FC58();
        quotient = (uint32_t)(((uint64_t)random * 0x35558AAB) >> 32) >> 14;
        raw = magnitude + random - quotient * 78642 - 39321;
        lower = (uint32_t)((int32_t)(raw - 196608) >> 31) & (196608 - raw);
        upper = (uint32_t)((int32_t)(1310720 - raw) >> 31) & (1310720 - raw);
        magnitude = raw + lower + upper;
    }
    random = sub_8001FC58();
    quotient = (uint32_t)(((uint64_t)random * 0x35558AAB) >> 32) >> 14;
    raw = r_u32(config + 120) + random - quotient * 78642 - 39321;
    lower = (uint32_t)((int32_t)(raw + 131072) >> 31) & (0 - 131072 - raw);
    upper = (uint32_t)((int32_t)(196608 - raw) >> 31) & (196608 - raw);
    vertical = raw + lower + upper;
    horizontal = r_s32(other + 88) < 0 ? magnitude : 0 - magnitude;
    w_u32(other + 92, horizontal);
    w_u32(config + 120, vertical);
    w_u32(changed + 92, 0);
    scale = r_s32(changed + 172) < 0 ? 108134 : (uint32_t)(0 - 108134);
    product = (uint32_t)(sub_8001FC90(scale, magnitude));
    positive_product = (int32_t)product > 0;
    positive_divisor = r_s32(config + 4) > 0;
    scale = r_s32(changed + 172) < 0 ? 108134 : (uint32_t)(0 - 108134);
    product = (uint32_t)(sub_8001FC90(scale, magnitude));
    divisor = r_u32(config + 4);
    derivative = sub_80010028(positive_product ? product : 0 - product, positive_divisor ? divisor : 0 - divisor);
    if (positive_product != positive_divisor)
        derivative = 0 - derivative;
    w_u32(changed + 176, derivative);
    w_u32(other + 176, 0);
    w_u32(config + 204, 0);
    w_u32(config + 88, previous_position[0]);
    w_u32(config + 116, previous_position[1]);
    w_u32(config + 144, previous_position[2]);
    w_u32(config + 172, previous_derivative[0]);
    w_u32(config + 200, previous_derivative[1]);
    w_u32(config + 228, previous_derivative[2]);
    return previous_derivative[2];
}

uint32_t sub_800A2448(uint32_t config, uint32_t other)
{
    const uint32_t pool = 0x800CE598u;
    int32_t current = ((int32_t)(uint32_t)(r_u32(pool + 4)));
    int32_t maximum = ((int32_t)(uint32_t)(r_u32(pool + 8)));
    uint32_t actor = 0;

    FUNCTION_MARKER(0x800A2448, "RASHCDG.BIN");
    if (current < maximum || r_u32(0x800D1814) >= 452u)
    {
        int32_t selected = current;
        int32_t next = current + 1;
        uint32_t candidate = r_u32(pool + 12) - 452u * (uint32_t)next;
        int32_t task_index;
        uint32_t task;

        while (next < maximum + 1)
        {
            if (!r_u16(candidate + 172))
                break;
            ++next;
            candidate -= 452;
        }
        w_u32(pool + 4, (uint32_t)next);
        actor = r_u32(pool + 12) - 452u * (uint32_t)selected;
        w_u16(actor + 172, (uint16_t)(selected + 160));

        task_index = ((int32_t)(uint32_t)(r_u32(0x800D1660))) + 1;
        task = 0x800D1664u + 24u * (r_u32(0x800D1660) + 1u);
        while (task_index < 18)
        {
            if (!r_u32(task))
                break;
            ++task_index;
            task += 24;
        }
        task = 0x800D1664u + 24u * r_u32(0x800D1660);
        w_u32(0x800D1660, (uint32_t)task_index);
        w_u32(actor + 4, task);
        w_u32(pool, r_u32(pool) + 1);
        if (maximum < selected)
        {
            w_u32(pool + 8, (uint32_t)selected);
            w_u32(0x800D1814, r_u32(0x800D1814) - 452);
        }
    }
    if (!actor || !sub_800A07D0(config, actor, 5, other))
        return 0;
    {
        int32_t value = (int32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(actor + 308))), ((int32_t)(uint32_t)(r_u32(actor + 304))));

        value = ((int32_t)(uint32_t)((uint32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(actor + 312))), value) << 10));
        if (value > 0x1000000)
            value = 0x1000000;
        w_u32(actor + 316, (uint32_t)value);
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
    uint32_t used = r_u32(0x8005AD50);
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
    w_u32(0x8005AD50, used | (1u << slot));
    w_u8(actor + 567, (uint8_t)slot);
    if ((int8_t)r_u8(child + 8) != (int32_t)identity)
        (void)sub_8001298C(child, identity);
    (void)sub_80012838(actor, child, kind, 0);
    return 1;
}

uint32_t sub_800A13D4(int32_t delta, uint32_t player, int32_t initial_actor_count)
{
    uint16_t saved_vertical = 0;
    int32_t index;

    FUNCTION_MARKER(0x800A13D4, "RASHCDG.BIN");
    if (!initial_actor_count)
    {
        if (r_s16(player + 320) && r_s32(0x8005B2AC) > 0)
        {
            index = 0;
            do
            {
                uint32_t event = r_u32(0x8005B35C) + 16 * (uint32_t)index;
                if (r_s8(event + 12))
                {
                    uint32_t event_road = r_u32(event);
                    uint32_t player_road = r_u32(player + 360);
                    if (event_road == player_road)
                    {
                        uint32_t width_raw = (uint32_t)(int32_t)r_s16(event + 14) << 16;
                        uint32_t event_position = r_u32(event + 4);
                        uint32_t lower = (uint32_t)((int32_t)(width_raw - 655360) >> 31) & (655360 - width_raw);
                        uint32_t upper = (uint32_t)((int32_t)(13107200 - width_raw) >> 31) & (13107200 - width_raw);
                        uint32_t width = width_raw + lower + upper;
                        uint32_t offset = event_position - r_u32(player + 368);
                        uint32_t sign = (uint32_t)((int32_t)offset >> 31);
                        uint32_t magnitude = (offset + sign) ^ sign;
                        if ((int32_t)magnitude < (int32_t)width && !(r_u8(r_u32(player + 1084)) & 128))
                        {
                            uint32_t track[8];
                            uint16_t rotation[9];
                            const uint32_t *position;
                            const uint16_t *basis;
                            if (width_raw)
                            {
                                uint32_t geometry, lookup;
                                (void)sub_8001E0B4(track, xport_guest_cptr(player + 328, 32), 32);
                                (void)sub_8003775C(0, track, (int32_t)offset < 0 ? 0 - width : width);
                                geometry = track[3];
                                position = xport_guest_cptr(geometry + 20, 12);
                                lookup = sub_8003B4B0(r_u32(player + 428), r_u16(player + 360));
                                rotation[6] = r_u16(geometry + 14);
                                rotation[7] = 0;
                                rotation[8] = r_u16(geometry + 18);
                                if (lookup && r_s32(lookup + 4) < 0)
                                {
                                    rotation[6] = (uint16_t)(0 - rotation[6]);
                                    rotation[8] = (uint16_t)(0 - rotation[8]);
                                }
                                rotation[1] = 0;
                                rotation[3] = 0;
                                rotation[4] = 4096;
                                rotation[5] = 0;
                                rotation[0] = rotation[8];
                                rotation[2] = (uint16_t)(0 - rotation[6]);
                                basis = rotation;
                            }
                            else
                            {
                                position = xport_guest_cptr(player + 504, 12);
                                basis = xport_guest_cptr(player + 516, 18);
                            }
                            (void)sub_800A0EC4(event, position, basis);
                            break;
                        }
                    }
                }
                ++index;
            } while (index < r_s32(0x8005B2AC));
        }
        if (!r_u32(0x8005B314))
            return 0;
    }
    for (index = 0; index < 3; ++index)
    {
        uint32_t actor = r_u32(0x8005B304) + 280 * (uint32_t)index;
        uint32_t config, dx, dz, sign, major, minor, range, threshold;
        uint32_t segment, duration, elapsed, time, phase, coefficients[4], motion[3];
        uint16_t local_rotation[9], angles[3];
        uint16_t *orientation;
        const uint16_t *basis;
        uint32_t numerator, first, second, third, fourth, sum, current, next, axis;
        int numerator_positive, denominator_positive;

        if (!r_u16(actor + 172))
            continue;
        dx = r_u32(actor + 184) - r_u32(player + 184);
        dz = r_u32(actor + 192) - r_u32(player + 192);
        sign = (uint32_t)((int32_t)dx >> 31);
        major = ((uint32_t)((int32_t)dx >> 16) + sign) ^ sign;
        sign = (uint32_t)((int32_t)dz >> 31);
        minor = ((uint32_t)((int32_t)dz >> 16) + sign) ^ sign;
        if ((int32_t)major < (int32_t)minor)
        {
            uint32_t swap = major;
            major = minor;
            minor = swap;
        }
        sum = minor + (uint32_t)((int32_t)minor >> 1);
        range = major - (uint32_t)((int32_t)major >> 5) - (uint32_t)((int32_t)major >> 7) + (uint32_t)((int32_t)sum >> 2) + (uint32_t)((int32_t)sum >> 6);
        threshold = r_u32(actor + 180) ? 301 : 601;
        if ((int32_t)range >= (int32_t)threshold && !(r_u8(actor + 9) & 1))
        {
            (void)sub_80014000(actor);
            continue;
        }
        config = r_u32(actor + 276);
        if (r_u8(config + 3) < 2)
            continue;
        first = r_u8(config + 3);
        if (r_s32(actor + 264) >= (int32_t)first)
        {
            uint32_t step = (uint32_t)(sub_8001FC90(r_u32(actor + 220), (uint32_t)delta));
            (void)sub_8002EAD8(xport_guest_cptr(actor + 184, 12), xport_guest_cptr(actor + 214, 6), step, xport_guest_ptr(actor + 184, 12));
            w_u32(actor + 176, 0xFFFFFFFF);
            continue;
        }
        if (r_s8(config) < 0 && !r_u32(actor + 272))
            (void)sub_800A0F9C(config);
        if (r_u8(config + 1))
        {
            basis = xport_guest_cptr(actor + 244, 18);
            orientation = local_rotation;
            /* Recover the previous side axis in the stored base frame instead of an uninitialized stack row */
            for (axis = 0; axis < 3; ++axis)
            {
                int64_t side = (int64_t)r_s16(actor + 196) * (int16_t)basis[3 * axis] + (int64_t)r_s16(actor + 198) * (int16_t)basis[3 * axis + 1] + (int64_t)r_s16(actor + 200) * (int16_t)basis[3 * axis + 2];
                side >>= 12;
                if (side < -32768)
                    side = -32768;
                if (side > 32767)
                    side = 32767;
                orientation[axis] = (uint16_t)side;
            }
        }
        else
        {
            w_u32(actor + 224, r_u32(player + 504));
            w_u32(actor + 228, r_u32(player + 508));
            w_u32(actor + 232, r_u32(player + 512));
            first = sub_80020018((uint32_t)(int32_t)r_s16(player + 450), (uint32_t)(int32_t)r_s16(player + 454));
            (void)sub_80086C00(actor + 236, (int32_t)first, delta, 458752);
            first = sub_8001FF3C((uint32_t)(int32_t)r_s16(player + 452) << 4);
            (void)sub_80086C00(actor + 240, (int32_t)(0 - first), delta, 458752);
            angles[0] = (uint16_t)(0 - r_u16(actor + 240));
            angles[1] = (uint16_t)(0 - r_u16(actor + 236));
            angles[2] = 0;
            (void)sub_8004D2A4(angles, local_rotation);
            basis = local_rotation;
            orientation = xport_guest_ptr(actor + 244, 18);
        }
        segment = r_u32(actor + 264);
        duration = r_u32(config + 4 * (segment + 1));
        time = r_u32(actor + 272) + (uint32_t)delta;
        elapsed = time - r_u32(actor + 268);
        w_u32(actor + 272, time);
        if ((int32_t)elapsed >= (int32_t)duration)
        {
            uint32_t next_segment = r_u32(actor + 264) + 1;
            uint32_t segments;
            w_u32(actor + 264, next_segment);
            segments = r_u8(config + 3);
            if ((int32_t)next_segment < (int32_t)(segments - 1))
            {
                uint32_t start;
                segment = r_u32(actor + 264);
                start = r_u32(actor + 268) + duration;
                elapsed = r_u32(actor + 272) - start;
                w_u32(actor + 268, start);
                duration = r_u32(config + 4 * (segment + 1));
            }
            else
            {
                if (r_s8(config) <= 0)
                {
                    w_u32(actor + 268, 0);
                    w_u32(actor + 272, 0);
                    w_u32(actor + 264, 0);
                    w_u8(actor + 262, 255);
                }
                else
                {
                    w_u32(actor + 264, segments);
                    w_u32(actor + 220, r_u32(config + 4 * (r_u8(config + 3) + 63)));
                }
                elapsed = duration;
            }
        }
        numerator_positive = (int32_t)elapsed > 0;
        denominator_positive = (int32_t)duration > 0;
        phase = sub_80010028(numerator_positive ? elapsed : 0 - elapsed, denominator_positive ? duration : 0 - duration);
        if (numerator_positive != denominator_positive)
            phase = 0 - phase;
        if (r_s8(config) < 0)
        {
            uint32_t left = r_u32(config + 172) ? config : config + 28;
            uint32_t right = left == config ? config + 28 : config;
            second = r_u32(right + 176);
            first = r_u32(left + 172);
            third = (uint32_t)(sub_8001FC90(phase, second - first));
            w_u32(actor + 220, r_u32(left + 172) + third);
        }
        else
        {
            current = config + 4 * segment;
            next = current + 4;
            first = r_u32(next + 256);
            second = r_u32(current + 256);
            third = (uint32_t)(sub_8001FC90(phase, first - second));
            w_u32(actor + 220, r_u32(current + 256) + third);
            (void)sub_8002FA28((int32_t)phase, coefficients, coefficients + 1, coefficients + 2, coefficients + 3);
            first = (uint32_t)(sub_8001FC90(r_u32(current + 60), coefficients[0]));
            second = (uint32_t)(sub_8001FC90(r_u32(next + 60), coefficients[1]));
            third = (uint32_t)(sub_8001FC90(r_u32(current + 32), coefficients[2]));
            fourth = (uint32_t)(sub_8001FC90(r_u32(next + 32), coefficients[3]));
            sum = (uint32_t)(first + second + sub_8001FC90(duration, third + fourth));
            first = r_u32(next + 60);
            second = r_u32(current + 60);
            numerator = sum - second;
            duration = first - second;
            numerator_positive = (int32_t)numerator > 0;
            denominator_positive = (int32_t)duration > 0;
            phase = sub_80010028(numerator_positive ? numerator : 0 - numerator, denominator_positive ? duration : 0 - duration);
            if (numerator_positive != denominator_positive)
                phase = 0 - phase;
        }
        (void)sub_8002FA28((int32_t)phase, coefficients, coefficients + 1, coefficients + 2, coefficients + 3);
        for (axis = 0; axis < 3; ++axis)
        {
            current = config + 4 * (segment + 7 * axis);
            next = current + 4;
            first = (uint32_t)(sub_8001FC90(r_u32(current + 88), coefficients[0]));
            second = (uint32_t)(sub_8001FC90(r_u32(next + 88), coefficients[1]));
            third = (uint32_t)(sub_8001FC90(r_u32(current + 172), coefficients[2]));
            fourth = (uint32_t)(sub_8001FC90(r_u32(next + 172), coefficients[3]));
            motion[axis] = (uint32_t)(first + second + sub_8001FC90(duration, third + fourth));
        }
        (void)sub_800A1318(actor, motion, basis);
        (void)sub_8002F994((int32_t)phase, coefficients, coefficients + 1, coefficients + 2, coefficients + 3);
        for (axis = 0; axis < 3; ++axis)
        {
            current = config + 4 * (segment + 7 * axis);
            next = current + 4;
            first = (uint32_t)(sub_8001FC90(r_u32(current + 172), coefficients[2]));
            second = (uint32_t)(sub_8001FC90(r_u32(next + 172), coefficients[3]));
            third = (uint32_t)(sub_8001FC90(r_u32(current + 88), coefficients[0]));
            fourth = (uint32_t)(sub_8001FC90(r_u32(next + 88), coefficients[1]));
            numerator_positive = (int32_t)(third + fourth) > 0;
            denominator_positive = (int32_t)duration > 0;
            third = (uint32_t)(sub_8001FC90(r_u32(current + 88), coefficients[0]));
            fourth = (uint32_t)(sub_8001FC90(r_u32(next + 88), coefficients[1]));
            numerator = third + fourth;
            sum = sub_80010028(numerator_positive ? numerator : 0 - numerator, denominator_positive ? duration : 0 - duration);
            if (numerator_positive != denominator_positive)
                sum = 0 - sum;
            motion[axis] = first + sum + second;
        }
        (void)sub_8002E14C(motion);
        orientation[6] = (uint16_t)((int32_t)motion[0] >> 4);
        orientation[7] = (uint16_t)((int32_t)motion[1] >> 4);
        orientation[8] = (uint16_t)((int32_t)motion[2] >> 4);
        if (r_u8(config + 2))
        {
            saved_vertical = orientation[7];
            orientation[7] = 0;
            (void)sub_8002E468(orientation + 6);
        }
        sign = (uint32_t)((int32_t)motion[0] >> 31);
        first = (motion[0] + sign) ^ sign;
        sign = (uint32_t)((int32_t)motion[2] >> 31);
        second = (motion[2] + sign) ^ sign;
        if ((int32_t)first >= 656 || (int32_t)second >= 656)
        {
            int32_t old_sign = r_s8(actor + 262);
            if (r_s8(config) >= 2 && old_sign >= 0)
            {
                first = (uint32_t)(int32_t)(int16_t)orientation[6];
                second = (uint32_t)(int32_t)(int16_t)orientation[7];
                xport_gte_write_control(0, first);
                third = (uint32_t)(int32_t)(int16_t)orientation[8];
                xport_gte_write_control(2, second);
                xport_gte_write_control(4, third);
                first = (uint32_t)(int32_t)(int16_t)orientation[0];
                second = (uint32_t)(int32_t)(int16_t)orientation[1];
                third = (uint32_t)(int32_t)(int16_t)orientation[2];
                xport_gte_write_data(9, first);
                xport_gte_write_data(10, second);
                xport_gte_write_data(11, third);
                xport_gte_execute(0x178000C);
                first = xport_gte_read_data(9);
                second = xport_gte_read_data(10);
                third = xport_gte_read_data(11);
                orientation[3] = (uint16_t)first;
                orientation[4] = (uint16_t)second;
                orientation[5] = (uint16_t)third;
                (void)sub_8002E468(orientation + 3);
                first = (uint32_t)(int32_t)(int16_t)orientation[3];
                second = (uint32_t)(int32_t)(int16_t)orientation[4];
                xport_gte_write_control(0, first);
                third = (uint32_t)(int32_t)(int16_t)orientation[5];
                xport_gte_write_control(2, second);
                xport_gte_write_control(4, third);
                first = (uint32_t)(int32_t)(int16_t)orientation[6];
                second = (uint32_t)(int32_t)(int16_t)orientation[7];
                third = (uint32_t)(int32_t)(int16_t)orientation[8];
                xport_gte_write_data(9, first);
                xport_gte_write_data(10, second);
                xport_gte_write_data(11, third);
                xport_gte_execute(0x178000C);
                first = xport_gte_read_data(9);
                second = xport_gte_read_data(10);
                third = xport_gte_read_data(11);
                orientation[0] = (uint16_t)first;
                orientation[1] = (uint16_t)second;
                orientation[2] = (uint16_t)third;
            }
            else
            {
                int32_t positive_x, positive_z;
                first = orientation[8];
                orientation[1] = 0;
                orientation[0] = (uint16_t)first;
                first = 0 - orientation[6];
                positive_x = (int16_t)orientation[0] > 0;
                orientation[2] = (uint16_t)first;
                positive_z = (int32_t)(first << 16) > 0;
                if (old_sign < 0 || r_s8(actor + 262) == positive_x || r_s8(actor + 263) == positive_z)
                {
                    w_u8(actor + 262, (uint8_t)positive_x);
                    w_u8(actor + 263, (uint8_t)positive_z);
                }
                else
                {
                    orientation[0] = (uint16_t)(0 - orientation[0]);
                    orientation[1] = (uint16_t)(0 - orientation[1]);
                    orientation[2] = (uint16_t)(0 - orientation[2]);
                }
                (void)sub_8002E468(orientation);
                first = (uint32_t)(int32_t)(int16_t)orientation[6];
                second = (uint32_t)(int32_t)(int16_t)orientation[7];
                xport_gte_write_control(0, first);
                third = (uint32_t)(int32_t)(int16_t)orientation[8];
                xport_gte_write_control(2, second);
                xport_gte_write_control(4, third);
                first = (uint32_t)(int32_t)(int16_t)orientation[0];
                second = (uint32_t)(int32_t)(int16_t)orientation[1];
                third = (uint32_t)(int32_t)(int16_t)orientation[2];
                xport_gte_write_data(9, first);
                xport_gte_write_data(10, second);
                xport_gte_write_data(11, third);
                xport_gte_execute(0x178000C);
                first = xport_gte_read_data(9);
                second = xport_gte_read_data(10);
                third = xport_gte_read_data(11);
                orientation[3] = (uint16_t)first;
                orientation[4] = (uint16_t)second;
                orientation[5] = (uint16_t)third;
                current = config + 4 * segment;
                first = (uint32_t)(sub_8001FC90(phase, (r_u32(current + 288) - r_u32(current + 284)) << 16));
                second = r_u32(current + 284) + (uint32_t)((int32_t)first >> 16);
                (void)sub_8002ED94(orientation + 3, orientation, second);
            }
            (void)sub_8003FA40(orientation, basis, xport_guest_ptr(actor + 196, 18));
        }
        if (r_u8(config + 2))
        {
            orientation[7] = saved_vertical;
            (void)sub_8002EFF4(orientation + 6, basis, xport_guest_ptr(actor + 214, 6));
        }
        else
        {
            first = r_u16(actor + 208);
            second = r_u16(actor + 210);
            third = r_u16(actor + 212);
            w_u16(actor + 214, (uint16_t)first);
            w_u16(actor + 216, (uint16_t)second);
            w_u16(actor + 218, (uint16_t)third);
        }
    }
    return 0;
}


uint32_t sub_8007ED64(uint32_t actor, int32_t angle)
{
    uint32_t radius, amplitude, table, scale, coordinate, result = 0;
    unsigned axis;

    FUNCTION_MARKER(0x8007ED64, "RASHCDG.BIN");
    radius = r_u32(actor + 308);
    if (angle > 0)
        radius = 0 - radius;
    amplitude = 3 * radius;
    if ((int32_t)amplitude < 0)
        amplitude += 3;
    amplitude = (uint32_t)((int32_t)amplitude >> 2);
    table = 0x8005624C + 4 * ((uint32_t)angle & 4095);
    scale = (uint32_t)sub_8001FC90((int32_t)amplitude, r_s16(table) * 16);
    for (axis = 0; axis < 3; ++axis)
    {
        int32_t normal = r_s16(actor + 438 + 2 * axis) * 16;
        coordinate = r_u32(actor + 504 + 4 * axis);
        result = (uint32_t)(((int64_t)normal * (int32_t)scale) >> 16) + coordinate;
        w_u32(actor + 184 + 4 * axis, result);
    }
    scale = (uint32_t)sub_8001FC90((int32_t)amplitude, (int32_t)(65536 - (uint32_t)(int32_t)(r_s16(table + 2) * 16)));
    for (axis = 0; axis < 3; ++axis)
    {
        int32_t normal = r_s16(actor + 528 + 2 * axis) * 16;
        coordinate = r_u32(actor + 184 + 4 * axis);
        result = (uint32_t)(((int64_t)normal * (int32_t)scale) >> 16) + coordinate;
        w_u32(actor + 184 + 4 * axis, result);
    }
    return result;
}

uint32_t sub_800871A8(const uint32_t position[3], const uint32_t origin[3], int32_t distance_scale, uint32_t actor, uint32_t output[3])
{
    uint32_t displacement[3];
    uint32_t length, requested = 0xE0000, limit, value, ratio;
    FUNCTION_MARKER(0x800871A8, "RASHCDG.BIN");
    displacement[0] = xport_load_le32(position) - xport_load_le32(origin);
    displacement[1] = xport_load_le32(position + 1) - xport_load_le32(origin + 1);
    displacement[2] = xport_load_le32(position + 2) - xport_load_le32(origin + 2);
    length = sub_8002F0F4(displacement);
    if ((int32_t)length <= 0x3FFEFFFF)
    {
        length = sub_8004CF74(length) << 2;
        requested = (uint32_t)sub_8001FC90(distance_scale, (int32_t)length);
    }
    value = r_u32(actor + 1128);
    limit = (r_u32(actor + 36) & 0x100) ? 0x40000 : 0x70000;
    if (value)
        w_u32(actor + 1128, (int32_t)(value * 7 + limit) / 8);
    else
        w_u32(actor + 1128, limit);
    limit = r_u32(actor + 1128);
    if ((int32_t)requested < (int32_t)limit)
    {
        value = r_u32(actor + 1124);
        if (value && ((int32_t)requested > 0x1FFFF || !(r_u32(actor + 552) & 0x400)))
        {
            value = r_u32(actor + 1124);
            w_u32(actor + 1124, (int32_t)(value + requested * 3) / 4);
            w_u32(actor + 552, r_u32(actor + 552) & 0xFFFFFBFF);
        }
        else
        {
            value = r_u32(actor + 552);
            w_u32(actor + 1124, requested);
            w_u32(actor + 552, value | 0x400);
        }
    }
    else
    {
        value = r_u32(actor + 1124);
        if (value)
            w_u32(actor + 1124, (int32_t)(value * 3 + limit) / 4);
        else
            w_u32(actor + 1124, limit);
    }
    if ((int32_t)length < 6553)
        length = 6553;
    value = r_u32(actor + 1124);
    ratio = (int32_t)value > 0 ? sub_80010028(value, length) : 0 - sub_80010028(0 - value, length);
    (void)sub_8002E810(ratio, displacement, displacement);
    value = xport_load_le32(origin) + displacement[0];
    xport_store_le32(output, value);
    value = xport_load_le32(origin + 1) + displacement[1];
    xport_store_le32(output + 1, value);
    value = xport_load_le32(origin + 2) + displacement[2];
    xport_store_le32(output + 2, value);
    return value;
}
