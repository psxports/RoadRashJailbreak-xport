#include "race_leaf_batch_002.h"
#include "fixed_math.h"
#include "menu_latch.h"
#include "race_pause.h"
#include "race_pause_frontier.h"
#include "xport.h"
#include <string.h>

static uint32_t batch_002_call(RRJMemory *m, RRJRaceLeafCall call, uint32_t target, uint32_t a0, uint32_t a1)
{
    uint32_t args[8] = {a0, a1, 0, 0, 0, 0, 0, 0};

    return call ? call(m, target, args) : 0;
}

uint32_t sub_8009C308(RRJMemory *m, RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x8009C308, "RASHCDG.BIN");
    return sub_8009C310(m, call);
}

uint32_t sub_8009C310(RRJMemory *m, RRJRaceLeafCall call)
{
    const uint32_t roads = 0x801FFAE0u;
    int32_t player_count = rrj_s32(rrj_read32(m, rrj_read32(m, 0x8005B2F8) + 48));
    int32_t player;

    FUNCTION_MARKER(0x8009C310, "RASHCDG.BIN");
    for (player = 0; player < player_count; ++player)
    {
        int32_t road_count = (int32_t)sub_8009FAD8(m, roads, (uint32_t)player);
        uint32_t owner = rrj_read32(m, 0x8005B268u + 4u * (uint32_t)player);
        uint32_t allow_special = sub_8003B8F4(m, owner + 172);
        int32_t index;

        for (index = 0; index < road_count; ++index)
        {
            uint32_t road_id = rrj_read32(m, roads + 4u * (uint32_t)index);
            uint32_t record = sub_80013204(m, road_id, (uint32_t)player);

            if (record && rrj_read32(m, record + 8) == road_id)
                (void)sub_8009CA88(m, road_id, rrj_read32(m, rrj_read32(m, record + 4) + 32), rrj_read32(m, record + 76), allow_special, (uint32_t)player, call);
        }
        player_count = rrj_s32(rrj_read32(m, rrj_read32(m, 0x8005B2F8) + 48));
    }
    return 0;
}

uint32_t sub_8009C654(RRJMemory *m, uint32_t record, uint32_t identity, uint32_t player, RRJRaceLeafCall call)
{
    uint32_t result = 0;
    uint32_t kind;

    FUNCTION_MARKER(0x8009C654, "RASHCDG.BIN");
    if ((int16_t)rrj_u16(rrj_at(m, record + 4, 2)) > 0)
        return 0;
    kind = rrj_u16(rrj_at(m, record, 2));
    switch (kind)
    {
        case 0:
            if (rrj_s32(rrj_read32(m, 0x800D86F4)) <= 0 && rrj_s32((sub_8001FC58(m) % 11u) << 16) < rrj_s32(rrj_read32(m, record + 48)))
            {
                uint32_t actor = sub_80095848(m);

                if (actor && !(rrj_read32(m, record + 8) >> 16))
                {
                    uint32_t linked = rrj_read32(m, actor + 852);

                    rrj_write32(m, actor + 360, rrj_read32(m, record + 8));
                    rrj_write32(m, actor + 368, rrj_read32(m, record + 36));
                    rrj_write32(m, actor + 364, (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, record + 60, 2)));
                    rrj_write32(m, actor + 344, rrj_read32(m, record + 32));
                    (void)sub_8002090C(m, actor);
                    rrj_put16(rrj_at(m, actor + 320, 2), 1);
                    rrj_write32(m, actor + 924, 0);
                    rrj_write32(m, linked + 604, 1);
                    (void)batch_002_call(m, call, 0x80093F94u, actor, 0);
                    result = 1;
                }
            }
            break;

        case 2:
            if (sub_8008CDF4(m, 2))
            {
                uint32_t owner = rrj_read32(m, 0x8005B268u + 4u * player);
                int32_t difference = rrj_s32(rrj_read32(m, owner + 368) - rrj_read32(m, record + 36));
                int32_t sign = difference >> 31;

                difference = rrj_s32(((uint32_t)difference + (uint32_t)sign) ^ (uint32_t)sign);
                if (rrj_read32(m, record + 8) != rrj_read32(m, owner + 360) || !rrj_read32(m, rrj_read32(m, 0x8005B2F8) + 16) || difference > 0)
                    result = sub_800CB8C8(m, record, 1, owner, call) != 0;
            }
            break;

        case 4:
        {
            uint32_t selector = rrj_u16(rrj_at(m, record + 2, 2));

            if (selector == 50)
            {
                if (*(uint8_t *)rrj_at(m, rrj_read32(m, 0x8005B2F8) + 4, 1) & 0x10u)
                {
                    (void)sub_80012BA8(m, record, identity, player);
                    result = 1;
                }
            }
            else if (selector != 9 && selector)
            {
                int32_t material = rrj_s32(sub_8009C5E4(m, (int32_t)selector));
                uint32_t owner;

                if (material < 0)
                    break;
                owner = rrj_read32(m, 0x8005B268u + 4u * player);
                if ((uint32_t)material < 6)
                {
                    if (sub_8008CDF4(m, 4))
                        result = sub_800A2630(m, record, owner, call) != 0;
                }
                else if (sub_8008CDF4(m, 5))
                    result = sub_800A2448(m, record, owner, call) != 0;
            }
            else if (!(*(uint8_t *)rrj_at(m, rrj_read32(m, 0x8005B2F8) + 4, 1) & 0x10u) && rrj_s32(rrj_read32(m, 0x8005B314)) < 3)
            {
                int32_t table_index;

                for (table_index = 0; table_index < 2; ++table_index)
                {
                    uint32_t entry = 0x8005B328u + 4u * (uint32_t)table_index;
                    int32_t entry_selector = *(int8_t *)rrj_at(m, entry, 1);

                    if (entry_selector < 0)
                        break;
                    if ((uint32_t)entry_selector == selector)
                    {
                        int32_t config_index = *(int8_t *)rrj_at(m, entry + 1, 1);
                        int32_t sign = config_index >> 31;
                        uint32_t config;

                        config_index = (int32_t)(((uint32_t)config_index + (uint32_t)sign) ^ (uint32_t)sign);
                        if (config_index >= 6)
                        {
                            config_index = rrj_s32(rrj_read32(m, 0x8005B260));
                            while (config_index < 6)
                            {
                                config = rrj_read32(m, 0x8005B24C) + 312u * (uint32_t)config_index;
                                if (!*(uint8_t *)rrj_at(m, config, 1))
                                {
                                    *(uint8_t *)rrj_at(m, config, 1) = 0xFFu;
                                    break;
                                }
                                ++config_index;
                            }
                        }
                        if (config_index < 6)
                        {
                            config = rrj_read32(m, 0x8005B24C) + 312u * (uint32_t)config_index;
                            (void)sub_800A0A20(m, selector, 1, record + 20, 0, config, call);
                            result = 1;
                        }
                        break;
                    }
                }
            }
            break;
        }

        case 6:
            if (sub_8008CDF4(m, 6) && sub_8008D9E4(m, player))
            {
                uint32_t object = sub_8009BB48(m, record, rrj_read32(m, 0x8005B268u + 4u * player));

                if (object)
                    result = rrj_u16(rrj_at(m, object, 2));
            }
            break;

        default:
            break;
    }
    if (result)
        rrj_put16(rrj_at(m, record + 4, 2), (uint16_t)result);
    return result;
}

uint32_t sub_8009CA88(RRJMemory *m, uint32_t road_id, uint32_t table, uint32_t candidates, uint32_t allow_special, uint32_t player, RRJRaceLeafCall call)
{
    const uint32_t roads = 0x801FFB00u;
    uint32_t road_count;
    uint32_t destination;
    uint32_t index;

    FUNCTION_MARKER(0x8009CA88, "RASHCDG.BIN");
    if (!table || sub_8009F054(m, candidates, player))
        return 0;
    destination = sub_80013360(m, road_id, player);
    road_count = sub_8009EF8C(m, candidates, roads, 6);
    if (!road_count)
        return 0;

    if (rrj_read32(m, rrj_read32(m, 0x8005B2F8) + 48) == 1)
    {
        uint32_t count = rrj_u16(rrj_at(m, table + 2, 2));
        uint32_t record = rrj_read32(m, table + 40);

        for (index = 0; index < count; ++index, record += 76)
            if ((int16_t)rrj_u16(rrj_at(m, record + 4, 2)) <= 0 && sub_8009C4FC(m, record, roads, (int32_t)road_count, player))
                (void)sub_8009C654(m, record, rrj_read32(m, table + 12), player, call);
    }

    {
        uint32_t count = rrj_u16(rrj_at(m, table, 2));
        uint32_t record = rrj_read32(m, table + 36);

        for (index = 0; index < count; ++index, record += 68)
        {
            if ((int16_t)rrj_u16(rrj_at(m, record + 4, 2)) <= 0 && sub_8009C4FC(m, record, roads, (int32_t)road_count, player))
            {
                uint32_t identity = sub_8009C654(m, record, rrj_read32(m, table + 12), player, call);

                if (identity)
                    (void)sub_8009C41C(m, destination, 3, index, identity);
            }
        }
    }

    {
        uint32_t count = rrj_u16(rrj_at(m, table + 6, 2));
        uint32_t record = rrj_read32(m, table + 48);

        for (index = 0; index < count; ++index, record += 88)
        {
            if ((int16_t)rrj_u16(rrj_at(m, record + 4, 2)) > 0)
            {
                if ((*(uint8_t *)rrj_at(m, rrj_read32(m, 0x8005B2F8) + 4, 1) & 0x10u) && sub_80013294(m, 6, rrj_u16(rrj_at(m, record + 4, 2)), player))
                    (void)sub_8008D89C(m);
                continue;
            }
            if (!sub_8009C4FC(m, record, roads, (int32_t)road_count, player))
                continue;
            {
                uint32_t identity = sub_8009C654(m, record, rrj_read32(m, table + 12), player, call);

                if ((uint16_t)(identity - 1u) < 223u)
                {
                    uint32_t object = rrj_read32(m, 0x800CD6C4) + 280u * (identity & 31u);

                    if (object)
                    {
                        *(uint8_t *)rrj_at(m, object + 2, 1) = (uint8_t)index;
                        *(uint8_t *)rrj_at(m, object + 3, 1) = (uint8_t)(1u << player);
                        rrj_write32(m, object + 4, rrj_read32(m, table + 12));
                    }
                    (void)sub_8009C41C(m, destination, 6, index, identity);
                    (void)sub_8008D89C(m);
                }
                else
                    (void)sub_8008DA20(m, record, player);
            }
        }
    }

    {
        uint32_t count = rrj_u16(rrj_at(m, table + 4, 2));
        uint32_t record = rrj_read32(m, table + 44);

        for (index = 0; index < count; ++index, record += 64)
        {
            uint32_t selector;
            uint32_t identity;

            if ((int16_t)rrj_u16(rrj_at(m, record + 4, 2)) > 0)
                continue;
            selector = rrj_u16(rrj_at(m, record + 2, 2));
            if ((!allow_special && selector == 30) || (selector == 50 && !(*(uint8_t *)rrj_at(m, rrj_read32(m, 0x8005B2F8) + 4, 1) & 0x10u)))
                continue;
            if (selector == 50)
            {
                if ((rrj_read32(m, record + 8) & 0xFFFE0000u) && !sub_8009F288(m, record, roads, (int32_t)road_count))
                    continue;
            }
            else if (!sub_8009C4FC(m, record, roads, (int32_t)road_count, player))
                continue;
            identity = sub_8009C654(m, record, rrj_read32(m, table + 12), player, call);
            if (identity && selector != 50)
                (void)sub_8009C41C(m, destination, 4, index, identity);
        }
    }

    {
        uint32_t count = rrj_u16(rrj_at(m, table + 8, 2));
        uint32_t record = rrj_read32(m, table + 52);

        for (index = 0; index < count; ++index, record += 64)
        {
            if ((int16_t)rrj_u16(rrj_at(m, record + 4, 2)) <= 0 && sub_8009C4FC(m, record, roads, (int32_t)road_count, player))
            {
                uint32_t identity = sub_8009C654(m, record, rrj_read32(m, table + 12), player, call);

                if (identity)
                    (void)sub_8009C41C(m, destination, 0, index, identity);
            }
        }
    }
    return 0;
}

uint32_t sub_800C3950(RRJMemory *m, uint32_t actor)
{
    uint32_t owner = rrj_read32(m, actor + 596);
    uint32_t animation = rrj_read32(m, actor + 540);
    int32_t motion = rrj_s32(rrj_read32(m, owner + 672));
    int32_t magnitude = motion < 0 ? (int32_t)(0u - (uint32_t)motion) : motion;
    uint32_t animation_index = rrj_read32(m, animation + 12);
    uint32_t animation_entry = rrj_read32(m, animation + 4) + 12u * animation_index;
    uint32_t animation_table = rrj_read32(m, rrj_read32(m, animation + 40) + 4);
    uint32_t animation_record = rrj_read32(m, animation_table + 4u * *(uint8_t *)rrj_at(m, animation_entry, 1));
    int32_t multiplier = (int16_t)rrj_u16(rrj_at(m, animation_record + 16, 2)) - 1;
    uint32_t mirrored = (*(uint8_t *)rrj_at(m, actor + 572, 1) >> 5) & 1u;
    uint32_t reverse = 0;
    int32_t limit = multiplier << 16;
    uint32_t state = rrj_u16(rrj_at(m, actor + 544, 2));
    uint32_t state_kind = rrj_u16(rrj_at(m, 0x800541D6u + 8u * state, 2));

    FUNCTION_MARKER(0x800C3950, "RASHCDG.BIN");
    if (mirrored)
    {
        if (rrj_s32(rrj_read32(m, actor + 76)) <= 0xFFFF && motion < 0)
            limit = 229376;
        if (!magnitude)
        {
            uint32_t linked = rrj_read32(m, owner + 856);

            if (!(rrj_read32(m, linked + 564) & 2u))
            {
                magnitude = rrj_s32(rrj_read32(m, linked + 672));
                motion = rrj_s32(rrj_read32(m, owner + 636)) < 0 ? -magnitude : magnitude;
                limit = motion > 0 ? 0x40000 : 98304;
            }
        }
    }
    else if (rrj_s32(rrj_read32(m, owner + 636)) > 0)
    {
        reverse = 1;
        if (rrj_read32(m, owner + 856) && rrj_read32(m, owner + 1088))
            limit = 327680;
    }

    {
        int32_t now = rrj_s32(rrj_read32(m, rrj_read32(m, 0x8005B2F8) + 16));

        if (now - rrj_s32(rrj_read32(m, actor + 548)) >= 301)
        {
            int32_t speed = rrj_s32(rrj_read32(m, owner + 480)) >> 16;
            int32_t duration = speed < 0 ? 150 : 150 - speed;

            if (duration < 0)
                duration = 0;
            if (magnitude > 58982)
            {
                rrj_write32(m, actor + 548, (uint32_t)now);
                (void)sub_800273EC(m, actor, rrj_s32(rrj_read32(m, owner + 636)) > 0 ? 15 : 12, (uint32_t)duration, 3, 2);
            }
        }
    }

    if (magnitude)
    {
        uint32_t valid;

        if (mirrored)
            valid = !((motion > 0 && (state == 75 || state == 82)) || (motion < 0 && (state == 74 || state == 81)));
        else
            valid = state_kind != 2;
        if (valid)
        {
            uint32_t flags = rrj_read32(m, actor + 552);

            rrj_write32(m, actor + 552, state_kind == 2 ? flags & 0xFFBFFFFFu : flags | 0x400000u);
            if (mirrored)
                state = state == 79 ? (motion > 0 ? 82u : 81u) : (motion > 0 ? 75u : 74u);
            else
                state = rrj_read32(m, owner + 180) >= 9u ? (state == 13 ? 20u : 18u) : 19u;
            rrj_put16(rrj_at(m, actor + 544, 2), (uint16_t)state);
            (void)sub_8005C0B0(m, animation, (rrj_read32(m, 0x800541D4u + 8u * state) >> 4) & 0xFFFu, reverse, 0, rrj_read32(m, 0x8005B3E4) ? rrj_read32(m, rrj_read32(m, 0x8005B3E4) + 72) : 0);
            if (rrj_u16(rrj_at(m, actor + 608, 2)))
                rrj_put16(rrj_at(m, actor + 608, 2), 0);
        }
        {
            int32_t product = (int32_t)((uint32_t)magnitude * (uint32_t)multiplier);
            int32_t target = product < limit ? product : limit;
            uint32_t flags = rrj_read32(m, actor + 552);

            *(uint8_t *)rrj_at(m, animation_entry + 2, 1) = (uint8_t)reverse;
            if (flags & 0x400000u)
            {
                int32_t denominator = rrj_s32(rrj_read32(m, animation + 24));
                int32_t current = rrj_s32(rrj_read32(m, animation + 16) << 16);

                if (denominator)
                    current += rrj_s32(rrj_read32(m, animation + 32) << 16) / denominator;
                if (target - current > 0x20000)
                    target = current + 0x20000;
                else if (target - current < -0x20000)
                    target = current - 0x20000;
                else
                    rrj_write32(m, actor + 552, flags & 0xFFBFFFFFu);
            }
            rrj_write32(m, animation + 36, rrj_read32(m, animation + 36) | 8u);
            rrj_write32(m, animation + 16, (uint32_t)(target >> 16));
            rrj_write32(m, animation + 32, (uint32_t)((int64_t)rrj_s32(rrj_read32(m, animation + 24)) * (target - (rrj_s32(rrj_read32(m, animation + 16)) << 16)) >> 16));
            rrj_write32(m, animation + 28, 0);
        }
        return 1;
    }

    if (state_kind == 2)
    {
        uint32_t next = 0;
        uint32_t mode = 0;

        switch (state)
        {
            case 18:
            case 19:
                next = 11;
                break;
            case 20:
                next = 13;
                mode = 1;
                break;
            case 74:
            case 75:
                next = 77;
                break;
            case 81:
            case 82:
                next = 79;
                mode = 1;
                break;
            default:
                return 0;
        }
        return sub_800C4550(m, next, actor, mode) & 0xFFu;
    }
    return 0;
}

uint32_t sub_800CB8C8(RRJMemory *m, uint32_t record, uint32_t mode, uint32_t player, RRJRaceLeafCall call)
{
    const uint32_t scratch = 0x801FFB40u;
    uint32_t road;
    uint32_t actor = 0;
    uint32_t basis;
    uint32_t type;
    uint32_t subtype;
    uint32_t animation;
    int32_t selected;

    FUNCTION_MARKER(0x800CB8C8, "RASHCDG.BIN");
    if (rrj_read32(m, 0x800CE178) == rrj_read32(m, 0x800CE17C) || !rrj_u16(rrj_at(m, 0x800CE580, 2)))
        return 0;
    rrj_write32(m, scratch, rrj_read32(m, record + 8));
    rrj_write32(m, scratch + 4, (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, record + 60, 2)));
    rrj_write32(m, scratch + 8, rrj_read32(m, record + 36));
    road = sub_80039DFC(m, 0, scratch);
    if (!road)
        return 0;

    selected = rrj_s32(rrj_read32(m, 0x800D4B74));
    if (rrj_read32(m, 0x800D4B80) && selected < 4 && rrj_s32(rrj_read32(m, 0x800D4B70)) < rrj_s32(rrj_read32(m, 0x800D8744)))
    {
        int32_t next = selected + 1;
        uint32_t candidate = rrj_read32(m, 0x800D4B80) + 572u * (uint32_t)next;

        while (next < 4)
        {
            if (!rrj_u16(rrj_at(m, candidate + 172, 2)) && rrj_read32(m, candidate + 4))
                break;
            ++next;
            candidate += 572;
        }
        rrj_write32(m, 0x800D4B74, (uint32_t)next);
        actor = rrj_read32(m, 0x800D4B80) + 572u * (uint32_t)selected;
        rrj_put16(rrj_at(m, actor + 172, 2), (uint16_t)(selected + 64));
        rrj_write32(m, 0x800D4B70, rrj_read32(m, 0x800D4B70) + 1);
        if (rrj_s32(rrj_read32(m, 0x800D4B78)) < selected)
            rrj_write32(m, 0x800D4B78, (uint32_t)selected);
    }
    if (!actor)
        return 0;

    *(uint8_t *)rrj_at(m, actor + 567, 1) = 0xFFu;
    rrj_write32(m, actor + 56, 0);
    if (rrj_read32(m, record + 44))
        rrj_write32(m, record + 20, rrj_read32(m, record + 20) + (sub_8001FC58(m) % rrj_read32(m, record + 44) << 16));
    if (rrj_read32(m, record + 40))
        rrj_write32(m, record + 28, rrj_read32(m, record + 28) + (sub_8001FC58(m) % rrj_read32(m, record + 40) << 16));
    if (!sub_8003A700(m, road, scratch, scratch + 16))
        goto cleanup;
    if ((rrj_read32(m, scratch) >> 16) == 1)
        (void)sub_8003C758(m, scratch + 16, record + 20);
    (void)sub_8001E0B4(m, actor + 328, scratch + 16, 32);
    basis = rrj_read32(m, actor + 340);
    rrj_write32(m, actor + 180, sub_8002FAD4(m, actor, 4, 0xFFFFu, 0, call));
    if (rrj_read32(m, actor + 180) == 0xFFFFu)
        goto cleanup;

    type = rrj_u16(rrj_at(m, record + 2, 2));
    if (type < 19)
    {
        subtype = type;
        *(uint8_t *)rrj_at(m, actor + 566, 1) = (uint8_t)subtype;
        if (!rrj_u16(rrj_at(m, 0x800CCC20u + 16u * subtype, 2)))
            *(uint8_t *)rrj_at(m, actor + 564, 1) = 0;
        else
            *(uint8_t *)rrj_at(m, actor + 564, 1) = (sub_8001FC58(m) & 1u) ? 1 : 2;
    }
    else if (type == 19 || type == 20)
    {
        subtype = sub_8001FC58(m) % 13u;
        *(uint8_t *)rrj_at(m, actor + 564, 1) = type == 19 ? 1 : 2;
        *(uint8_t *)rrj_at(m, actor + 566, 1) = (uint8_t)subtype;
    }
    else if (type == 21)
    {
        subtype = sub_8001FC58(m) % 13u;
        *(uint8_t *)rrj_at(m, actor + 566, 1) = (uint8_t)subtype;
        *(uint8_t *)rrj_at(m, actor + 564, 1) = rrj_u16(rrj_at(m, 0x800CCC20u + 16u * subtype, 2)) ? ((sub_8001FC58(m) & 1u) ? 1 : 2) : 0;
    }
    else if (type == 22)
    {
        *(uint8_t *)rrj_at(m, actor + 564, 1) = 0;
        *(uint8_t *)rrj_at(m, actor + 566, 1) = (uint8_t)(sub_8001FC58(m) % 6u + 13u);
    }
    else
    {
        subtype = sub_8001FC58(m) % 19u;
        *(uint8_t *)rrj_at(m, actor + 566, 1) = (uint8_t)subtype;
        *(uint8_t *)rrj_at(m, actor + 564, 1) = rrj_u16(rrj_at(m, 0x800CCC20u + 16u * subtype, 2)) ? ((sub_8001FC58(m) & 1u) ? 1 : 2) : 0;
    }
    (void)sub_80012FC8(m, actor, 0);

    if (mode)
    {
        rrj_write32(m, actor + 184, rrj_read32(m, record + 20));
        rrj_write32(m, actor + 188, rrj_read32(m, record + 24));
        rrj_write32(m, actor + 192, rrj_read32(m, record + 28));
        sub_80036800(m, actor + 184, basis, actor + 344, actor + 348);
    }
    else
    {
        rrj_write32(m, actor + 184, rrj_read32(m, basis + 20));
        rrj_write32(m, actor + 188, rrj_read32(m, basis + 24));
        rrj_write32(m, actor + 192, rrj_read32(m, basis + 28));
        (void)sub_8002EAD8(m, basis + 20, basis + 14, rrj_read32(m, actor + 348), actor + 184);
    }

    subtype = *(uint8_t *)rrj_at(m, actor + 566, 1);
    rrj_write32(m, actor + 480, rrj_read32(m, 0x800CCC28u + 16u * subtype));
    rrj_put16(rrj_at(m, actor + 438, 2), 0);
    rrj_put16(rrj_at(m, actor + 440, 2), 4096);
    rrj_put16(rrj_at(m, actor + 442, 2), 0);
    rrj_put16(rrj_at(m, actor + 444, 2), (uint16_t)-(int16_t)rrj_u16(rrj_at(m, basis + 14, 2)));
    rrj_put16(rrj_at(m, actor + 446, 2), (uint16_t)-(int16_t)rrj_u16(rrj_at(m, basis + 16, 2)));
    rrj_put16(rrj_at(m, actor + 448, 2), (uint16_t)-(int16_t)rrj_u16(rrj_at(m, basis + 18, 2)));
    rrj_write32(m, actor + 188, rrj_read32(m, actor + 188) - 0x10000u);
    rrj_put16(rrj_at(m, actor + 432, 2), rrj_u16(rrj_at(m, actor + 448, 2)));
    rrj_put16(rrj_at(m, actor + 434, 2), 0);
    rrj_put16(rrj_at(m, actor + 436, 2), (uint16_t)-(int16_t)rrj_u16(rrj_at(m, actor + 444, 2)));
    rrj_put16(rrj_at(m, actor + 450, 2), rrj_u16(rrj_at(m, actor + 444, 2)));
    rrj_put16(rrj_at(m, actor + 452, 2), rrj_u16(rrj_at(m, actor + 446, 2)));
    rrj_put16(rrj_at(m, actor + 454, 2), rrj_u16(rrj_at(m, actor + 448, 2)));
    (void)sub_8003662C(m, actor + 450, actor + 328, actor + 360);
    rrj_write32(m, actor + 364, (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, record + 60, 2)));
    sub_8003AF9C(m, actor + 172, 0, player);
    rrj_write32(m, actor + 324, sub_8003B61C(m, actor + 172));
    rrj_put16(rrj_at(m, actor + 322, 2), rrj_u16(rrj_at(m, record + 62, 2)));
    rrj_write32(m, actor + 316, 4915200);
    rrj_write32(m, actor + 556, rrj_read32(m, record + 64) << 4);
    rrj_put16(rrj_at(m, actor + 320, 2), (uint16_t)sub_80039F68(m, actor + 172));
    if (!rrj_u16(rrj_at(m, actor + 320, 2)))
        goto cleanup;

    (void)sub_8003DE28(m, actor, 1, 0, 0xFFFFFFFFu);
    sub_8003DF54(m, actor, 1, 0xFFFFFFFFu);
    {
        int32_t offset = rrj_s32(rrj_read32(m, actor + 344));

        if (offset < 0 && offset < rrj_s32(rrj_read32(m, actor + 400)))
            (void)sub_8002EAD8(m, actor + 184, basis + 2, rrj_read32(m, actor + 400) - (uint32_t)offset + 0x10000u, actor + 184);
        else if (rrj_s32(rrj_read32(m, actor + 412)) < offset)
            (void)sub_8002EAD8(m, actor + 184, basis + 2, rrj_read32(m, actor + 412) - (uint32_t)offset - 0x10000u, actor + 184);
    }
    if (!mode)
    {
        int32_t offset = rrj_s32(rrj_read32(m, record + 32));

        rrj_write32(m, actor + 344, (uint32_t)offset);
        if ((((offset >> 31) + offset) ^ (offset >> 31)) == 0x10000)
        {
            uint32_t linked = rrj_read32(m, actor + 372);

            if (linked)
            {
                if (offset <= 0)
                    offset = rrj_u16(rrj_at(m, linked + 10, 2)) ? rrj_s32(rrj_read32(m, linked + 32)) - 0x10000 : rrj_s32(rrj_read32(m, linked + 16)) + 0x10000;
                else
                    offset = rrj_u16(rrj_at(m, linked + 138, 2)) ? rrj_s32(rrj_read32(m, linked + 160)) + 0x10000 : rrj_s32(rrj_read32(m, linked + 144)) - 0x10000;
                rrj_write32(m, actor + 344, (uint32_t)offset);
            }
        }
        (void)sub_8002EAD8(m, actor + 184, basis + 2, rrj_read32(m, actor + 344), actor + 184);
    }
    rrj_write32(m, actor + 552, 1);
    rrj_write32(m, actor + 540, 0);
    *(uint8_t *)rrj_at(m, actor + 565, 1) = 0;
    rrj_write32(m, actor + 560, 0);
    animation = sub_80012884(m, 0x800CE170, actor);
    rrj_write32(m, actor + 540, animation);
    *(uint8_t *)rrj_at(m, actor + 72, 1) = 3;
    if (!animation)
        goto cleanup;
    (void)sub_800CAAF0(m, actor, call);
    (void)sub_800CAAA8(m, actor);
    (void)sub_800CB02C(m, actor);
    rrj_write32(m, actor + 176, 0xFFFFFFFFu);
    rrj_write32(m, actor + 548, rrj_read32(m, rrj_read32(m, 0x8005B2F8) + 16));
    if (!*(uint8_t *)rrj_at(m, actor + 566, 1))
        (void)sub_800CB78C(m, actor, 10, 9);
    return actor;

cleanup:
    (void)sub_8008C000(m, actor + 172, 2);
    return 0;
}

uint32_t sub_800B8018(RRJMemory *m, uint32_t delta, RRJRaceGlobalCall call)
{
    FUNCTION_MARKER(0x800B8018, "RASHCDG.BIN");
    (void)rrj_read32(m, 0x8005B2F8);
    return call(m, 0x800B8020, delta);
}

uint32_t sub_800A8C48(RRJMemory *m, uint32_t value, uint32_t packed)
{
    FUNCTION_MARKER(0x800A8C48, "RASHCDG.BIN");
    value = (value & 0xFFFFu) + 1;
    while (packed)
    {
        if ((packed & 0x1Fu) == value)
            return 1;
        packed >>= 5;
    }
    return 0;
}

uint32_t sub_800BD34C(RRJMemory *m, uint32_t value, int32_t delta)
{
    int32_t result = (*(uint8_t *)rrj_at(m, value, 1) & 0xF) + delta;

    FUNCTION_MARKER(0x800BD34C, "RASHCDG.BIN");
    if (result < 0)
        result = 0;
    if (result > 15)
        result = 15;
    *(uint8_t *)rrj_at(m, value, 1) = (uint8_t)((*(uint8_t *)rrj_at(m, value, 1) & 0xF0) | result);
    return (uint32_t)result;
}

uint32_t sub_8009DB58(RRJMemory *m, uint32_t actor, uint32_t other)
{
    uint32_t descriptor = rrj_read32(m, other + 1084);

    FUNCTION_MARKER(0x8009DB58, "RASHCDG.BIN");
    return !rrj_read32(m, descriptor + 40) && rrj_read32(m, rrj_read32(m, actor + 852) + 604) < 3 && *(uint8_t *)rrj_at(m, descriptor + 39, 1) < 0xF7;
}

uint32_t sub_8009E3F0(RRJMemory *m, uint32_t other, uint32_t actor)
{
    uint32_t linked = rrj_read32(m, actor + 852);
    int32_t position;

    FUNCTION_MARKER(0x8009E3F0, "RASHCDG.BIN");
    position = rrj_s32(rrj_read32(m, linked + 604) < 3 ? rrj_read32(m, actor + 368) : rrj_read32(m, linked + 368));
    if (rrj_s32(rrj_read32(m, actor + 364)) <= 0)
        return rrj_s32(rrj_read32(m, other + 368)) < position;
    return position < rrj_s32(rrj_read32(m, other + 368));
}

uint32_t sub_8009FC80(RRJMemory *m, uint32_t packed)
{
    uint32_t count = rrj_read32(m, 0x8005B1F8);

    FUNCTION_MARKER(0x8009FC80, "RASHCDG.BIN");
    packed &= 0xFFFFu;
    while (packed)
    {
        uint32_t value = packed & 0x1Fu;
        if (value && value - 1 < count)
            rrj_write32(m, 0x800D8720, rrj_read32(m, 0x800D8720) | (1u << (value - 1)));
        packed >>= 5;
    }
    return 0;
}

uint32_t sub_800BCEEC(RRJMemory *m, uint32_t value, uint32_t step)
{
    uint32_t packed = *(uint8_t *)rrj_at(m, value, 1);
    int32_t current = packed & 0xF;
    int32_t target = packed >> 4;
    int32_t delta = current - target;

    FUNCTION_MARKER(0x800BCEEC, "RASHCDG.BIN");
    if ((uint32_t)(delta < 0 ? -delta : delta) < step)
        current = target;
    else if (delta < 0)
        current += (int32_t)step;
    else
        current -= (int32_t)step;
    if (current < 0)
        current = 0;
    if (current > 15)
        current = 15;
    packed = (packed & 0xF0) | (uint32_t)current;
    *(uint8_t *)rrj_at(m, value, 1) = (uint8_t)packed;
    return packed;
}

uint32_t sub_800BD2A0(RRJMemory *m, uint32_t actor, uint32_t rank)
{
    uint32_t state = rrj_read32(m, 0x8005B2F8);
    uint32_t descriptor;
    uint32_t position;

    FUNCTION_MARKER(0x800BD2A0, "RASHCDG.BIN");
    descriptor = rrj_read32(m, actor + 1084);
    if (rrj_u16(rrj_at(m, actor + 172, 2)) >= rrj_read32(m, state + 48) && ((*(uint8_t *)rrj_at(m, descriptor + 1, 1) & 0xF) == 2))
        return 0;
    if (rrj_read32(m, 0x8005B2A8) & 1)
        return 0;
    position = *(uint8_t *)rrj_at(m, descriptor + 39, 1);
    if (position > rrj_read32(m, 0x8005B1F8))
        return 0;
    if (position >= 4 && rank < position)
        return 2;
    if (position < 6 && position < rank)
        return 4;
    return 0;
}

uint32_t sub_8009DBA0(RRJMemory *m, uint32_t actor, uint32_t other, int32_t first, int32_t second)
{
    uint32_t linked = rrj_read32(m, other + 852);
    uint32_t linked_mode = rrj_read32(m, linked + 604);
    uint32_t state = rrj_read32(m, 0x8005B2F8);
    int32_t delta = first - second;
    uint32_t magnitude = delta < 0 ? 0u - (uint32_t)delta : (uint32_t)delta;
    uint32_t index;
    int32_t position;

    FUNCTION_MARKER(0x8009DBA0, "RASHCDG.BIN");
    if (linked_mode < 3 && rrj_s32(rrj_read32(m, other + 480)) > 585944)
        return 0;
    index = rrj_read32(m, state + 60) + ((0u - (*(uint8_t *)rrj_at(m, state + 4, 1) & 1u)) & 3u);
    if (rrj_s32(rrj_read32(m, 0x800530A8 + 4 * index)) < rrj_s32(magnitude))
        return 0;
    if (rrj_read32(m, rrj_read32(m, actor + 1084) + 40))
        return 0;
    position = rrj_s32(rrj_read32(m, actor + 480));
    if (position <= 585944)
        return 1;
    if (linked_mode >= 3 && (uint32_t)(rrj_u16(rrj_at(m, linked + 544, 2)) - 72) < 2 && rrj_s32(rrj_read32(m, linked + 480)) >= position)
        return 1;
    return 0;
}

uint32_t sub_80097470(RRJMemory *m, uint32_t actor, uint32_t other)
{
    int32_t slot;

    FUNCTION_MARKER(0x80097470, "RASHCDG.BIN");
    if (!other || other == actor || !rrj_u16(rrj_at(m, other + 320, 2)))
        return 0;
    slot = *(int8_t *)rrj_at(m, other + 946, 1);
    (void)rrj_u16(rrj_at(m, other + 8 * (slot - 1) + 956, 2));
    if (!sub_8009DB58(m, actor, other))
        return 0;
    return sub_8009DBA0(m, actor, other, rrj_s32(sub_8009E444(m, actor, other)), 0) != 0;
}

uint32_t sub_8009E178(RRJMemory *m, uint32_t actor)
{
    uint32_t state = rrj_read32(m, 0x8005B2F8);
    int32_t slot = *(int8_t *)rrj_at(m, actor + 946, 1);
    uint32_t selected = rrj_u16(rrj_at(m, actor + 8 * slot + 950, 2));
    uint32_t players[2];
    uint32_t active[2] = {0, 0};
    int32_t score[2] = {0, 0};
    uint32_t count;
    uint32_t index;

    FUNCTION_MARKER(0x8009E178, "RASHCDG.BIN");
    if (selected != 224 && !(selected >> 5))
        return selected;
    if (*(uint8_t *)rrj_at(m, state + 4, 1) & 1)
        return *(uint8_t *)rrj_at(m, state + 6, 1);
    if (!(*(uint8_t *)rrj_at(m, state + 4, 1) & 0x10))
    {
        uint32_t primary = rrj_read32(m, 0x8005B38C);
        if (selected == rrj_u16(rrj_at(m, primary + 172, 2)))
            return selected;
        if (rrj_read32(m, rrj_read32(m, primary + 1084) + 40))
            return selected;
        return rrj_u16(rrj_at(m, primary + 172, 2));
    }
    if (!rrj_u16(rrj_at(m, actor + 320, 2)))
        return selected;

    count = rrj_read32(m, state + 48);
    if (count > 2)
        count = 2;
    for (index = 0; index < count; ++index)
    {
        players[index] = rrj_read32(m, 0x8005B268 + 4 * index);
        active[index] = !rrj_read32(m, rrj_read32(m, players[index] + 1084) + 40);
    }
    if (!active[0])
        return active[1] ? rrj_u16(rrj_at(m, players[1] + 172, 2)) : selected;
    if (!active[1])
        return rrj_u16(rrj_at(m, players[0] + 172, 2));

    for (index = 0; index < count; ++index)
    {
        int32_t dx = (int16_t)rrj_u16(rrj_at(m, actor + 186, 2)) - (int16_t)rrj_u16(rrj_at(m, players[index] + 186, 2));
        int32_t dz = (int16_t)rrj_u16(rrj_at(m, actor + 194, 2)) - (int16_t)rrj_u16(rrj_at(m, players[index] + 194, 2));
        int32_t major;
        int32_t minor;
        if (dx < 0)
            dx = -dx;
        if (dz < 0)
            dz = -dz;
        major = dx >= dz ? dx : dz;
        minor = dx >= dz ? dz : dx;
        score[index] = major - (major >> 5) - (major >> 7) + ((minor + (minor >> 1)) >> 2) + ((minor + (minor >> 1)) >> 6);
        if (score[index] > 300)
            score[index] = 300;
    }
    index = score[1] < score[0];
    return rrj_u16(rrj_at(m, players[index] + 172, 2));
}

uint32_t sub_80097388(RRJMemory *m, uint32_t actor, RRJRaceLeafCall call)
{
    uint32_t linked = rrj_read32(m, actor + 852);
    uint32_t result;
    uint32_t candidate;
    uint32_t step;
    int32_t remaining;

    FUNCTION_MARKER(0x80097388, "RASHCDG.BIN");
    if (rrj_read32(m, linked + 604) >= 3)
        return 0;
    result = rrj_read32(m, linked + 552) & 0x40;
    if (result)
        return result;
    if (rrj_s32(rrj_read32(m, actor + 480)) > 585944)
        return 1;
    result = *(uint8_t *)rrj_at(m, rrj_read32(m, actor + 1084) + 39, 1);
    if (result == 255)
        return result;

    candidate = rrj_read32(m, 0x800CE4D0);
    step = rrj_read32(m, 0x800CE4D4);
    remaining = rrj_s32(rrj_read32(m, rrj_read32(m, 0x800CE4DC)));
    while (remaining >= 0)
    {
        if (candidate != actor && sub_80097470(m, actor, candidate))
            (void)sub_80096F30(m, actor, candidate, 9, call);
        candidate += step;
        --remaining;
    }
    return step;
}

uint32_t sub_800205C0(RRJMemory *m, int32_t speed, uint32_t mode)
{
    int32_t limit = rrj_s32(rrj_read32(m, 0x80052EE4 + 36 * mode + 24));
    int32_t result = (int32_t)sub_8001FC90((int32_t)sub_8001FC90(3276, speed), limit);

    FUNCTION_MARKER(0x800205C0, "SLUS_010.53");
    if (result < 0x300000)
        result = 0x300000;
    if (result > limit)
        result = limit;
    return (uint32_t)result;
}

uint32_t sub_8002064C(RRJMemory *m, int32_t speed, uint32_t mode)
{
    int32_t limit = rrj_s32(rrj_read32(m, 0x80052EE4 + 36 * mode + 28));
    int32_t result = limit - (int32_t)sub_8001FC90((int32_t)sub_8001FC90(1638, speed), limit);

    FUNCTION_MARKER(0x8002064C, "SLUS_010.53");
    if (result < 0x400000)
        result = 0x400000;
    if (result > limit)
        result = limit;
    return (uint32_t)result;
}

uint32_t sub_800206DC(RRJMemory *m, int32_t speed, uint32_t mode)
{
    int32_t limit = rrj_s32(rrj_read32(m, 0x80052EE4 + 36 * mode + 32));
    int32_t result = limit - (int32_t)sub_8001FC90((int32_t)sub_8001FC90(2293, speed), limit);

    FUNCTION_MARKER(0x800206DC, "SLUS_010.53");
    if (result < 0x190000)
        result = 0x190000;
    if (result > limit)
        result = limit;
    return (uint32_t)result;
}

uint32_t sub_800BEA30(RRJMemory *m, uint32_t point, uint32_t actor)
{
    uint32_t descriptor = rrj_read32(m, actor + 1084);
    uint32_t mode = *(uint8_t *)rrj_at(m, descriptor + 1, 1) & 0xF;
    int32_t speed = rrj_s32(rrj_read32(m, actor + 480));
    int32_t front = rrj_s32(sub_800205C0(m, speed, mode));
    int32_t rear = -rrj_s32(sub_8002064C(m, speed, mode));
    int32_t longitudinal;
    int32_t lateral;
    uint32_t road;
    uint32_t magnitude;

    FUNCTION_MARKER(0x800BEA30, "RASHCDG.BIN");
    if (*(uint8_t *)rrj_at(m, actor + 928, 1) & 1)
    {
        longitudinal = rrj_s32((rrj_read32(m, actor + 324) - rrj_read32(m, point + 152)) << 4);
        if (*(uint8_t *)rrj_at(m, descriptor, 1) & 0x80)
            longitudinal = -longitudinal;
    }
    else
        longitudinal = rrj_s32(sub_800B6AAC(m, point + 12, actor + 528, actor + 504));
    if (longitudinal > front || longitudinal < rear)
        return 0;

    road = rrj_read32(m, point + 188);
    if ((*(uint8_t *)rrj_at(m, actor + 928, 1) & 1) && road == rrj_read32(m, actor + 360) && (!(road >> 16) || rrj_read32(m, point + 164) == rrj_read32(m, actor + 336)))
        lateral = rrj_s32(rrj_read32(m, point + 172) - rrj_read32(m, actor + 344));
    else
        lateral = rrj_s32(sub_800B6AAC(m, point + 12, actor + 432, actor + 184));
    magnitude = lateral < 0 ? 0u - (uint32_t)lateral : (uint32_t)lateral;
    return sub_800206DC(m, speed, mode) >= magnitude;
}

uint32_t sub_8001B3C8(RRJMemory *m, RRJRaceLeafCall call)
{
    uint32_t state = rrj_read32(m, 0x8005B2F8);

    FUNCTION_MARKER(0x8001B3C8, "SLUS_010.53");
    if (rrj_read32(m, state + 48) >= 2)
        return 0;
    if (*(uint8_t *)rrj_at(m, state + 4, 1) & 1)
        return 1;
    rrj_write32(m, 0x800D6C40, 7);
    rrj_write32(m, 0x800D685C, 0x230000);
    (void)sub_80023148(m, 0x800D6858, 1, 1, 0, call);
    rrj_write32(m, 0x8005B424, 1);
    rrj_write32(m, 0x800D6AA8, 4);
    return 4;
}

uint32_t sub_8009E528(RRJMemory *m, uint32_t source, uint32_t actor, RRJRaceLeafCall call)
{
    uint32_t linked = rrj_read32(m, actor + 852);
    uint32_t descriptor = rrj_read32(m, actor + 1084);
    uint32_t identity = rrj_u16(rrj_at(m, actor + 172, 2));
    uint32_t record = 0x800CD898 + 1132 * identity;
    uint32_t result;

    FUNCTION_MARKER(0x8009E528, "RASHCDG.BIN");
    rrj_write32(m, actor + 720, 0x20000);
    rrj_write32(m, actor + 484, 0);
    rrj_write32(m, actor + 576, 0);
    rrj_write32(m, actor + 480, 0);
    rrj_write32(m, actor + 924, 0);
    rrj_write32(m, actor + 464, 0);
    rrj_write32(m, actor + 460, 0);
    rrj_write32(m, actor + 456, 0);
    rrj_write32(m, linked + 456, 0);
    rrj_write32(m, linked + 460, 0);
    rrj_write32(m, linked + 464, 0);
    rrj_write32(m, linked + 480, 0);
    rrj_write32(m, linked + 484, 0);
    rrj_write32(m, linked + 488, 0);
    rrj_write32(m, linked + 552, rrj_read32(m, linked + 552) & 0xFFEFF067u);
    *(uint8_t *)rrj_at(m, descriptor + 39, 1) = 0xFE;
    (void)sub_8001B3C8(m, call);
    if (rrj_read32(m, linked + 604) >= 3)
    {
        rrj_write32(m, record + 184, rrj_read32(m, linked + 184));
        rrj_write32(m, record + 188, rrj_read32(m, linked + 188));
        rrj_write32(m, record + 192, rrj_read32(m, linked + 192));
    }
    else
    {
        rrj_write32(m, record + 184, rrj_read32(m, actor + 504));
        rrj_write32(m, record + 188, rrj_read32(m, actor + 508));
        rrj_write32(m, record + 192, rrj_read32(m, actor + 512));
    }
    rrj_write32(m, record + 548, rrj_read32(m, record + 548) | 6);
    if (!(rrj_read32(m, linked + 552) & 0x40))
        return sub_80092C7C(m, actor, 9);
    result = rrj_read32(m, rrj_read32(m, 0x8005B2F8) + 16);
    rrj_write32(m, descriptor + 40, result);
    return result;
}

uint32_t sub_800BCF6C(RRJMemory *m, uint32_t actor, uint32_t other)
{
    int32_t actor_slot = *(int8_t *)rrj_at(m, actor + 946, 1);
    int32_t other_slot;
    uint32_t actor_event;
    uint32_t other_event = 0;
    uint32_t actor_id;
    uint32_t other_id;
    uint32_t flags = 0;
    uint32_t state;

    FUNCTION_MARKER(0x800BCF6C, "RASHCDG.BIN");
    if (actor_slot <= 0)
        return 0;
    actor_event = actor + 8 * actor_slot + 948;
    other_slot = *(int8_t *)rrj_at(m, other + 946, 1);
    if (other_slot > 0)
        other_event = other + 8 * other_slot + 948;
    actor_id = rrj_u16(rrj_at(m, actor + 172, 2));
    other_id = rrj_u16(rrj_at(m, other + 172, 2));

    if (sub_800A8C48(m, other_id, rrj_u16(rrj_at(m, rrj_read32(m, actor + 1084) + 64, 2))))
        flags = 0x40;
    if (other_event && rrj_u16(rrj_at(m, other_event, 2)) == 16 && rrj_u16(rrj_at(m, other_event + 2, 2)) == actor_id && (rrj_u16(rrj_at(m, other_event + 6, 2)) & 0x4000))
    {
        flags |= 1;
        state = rrj_read32(m, 0x8005B2F8);
        if (other_id < rrj_read32(m, state + 48) && !(rrj_read32(m, other + 560) & 0x08000000) && !sub_800A8C48(m, actor_id, rrj_u16(rrj_at(m, rrj_read32(m, other + 1084) + 64, 2))))
            flags &= ~1u;
        if ((flags & 1) && (rrj_u16(rrj_at(m, actor_event, 2)) != 16 || rrj_u16(rrj_at(m, actor_event + 2, 2)) != other_id))
            flags |= 2;
    }
    if (*(uint8_t *)rrj_at(m, rrj_read32(m, actor + 1084) + 70, 1) == other_id)
        flags |= 0x1000;
    if (sub_800A8C48(m, other_id, rrj_read32(m, actor + 912)))
        flags |= 0x80;
    if (other_event)
    {
        uint32_t type = rrj_u16(rrj_at(m, other_event, 2));
        if ((type == 10 || type == 13 || type == 14 || type == 15) && rrj_u16(rrj_at(m, other_event + 2, 2)) == actor_id && (rrj_u16(rrj_at(m, other_event + 6, 2)) & 0x4000))
        {
            flags |= 4;
            type = rrj_u16(rrj_at(m, actor_event, 2));
            if (type - 10 >= 7 || rrj_u16(rrj_at(m, actor_event + 2, 2)) != other_id)
                flags |= 8;
        }
    }
    if (sub_800A8C48(m, other_id, rrj_u16(rrj_at(m, rrj_read32(m, actor + 1084) + 66, 2))))
        flags |= 0x100;
    state = rrj_u16(rrj_at(m, actor_event, 2));
    if (state == 16)
    {
        if (rrj_u16(rrj_at(m, actor_event + 2, 2)) == other_id && (rrj_u16(rrj_at(m, actor_event + 6, 2)) & 0x4000))
            flags |= 0x10;
        state = rrj_u16(rrj_at(m, actor_event, 2));
    }
    if ((state == 10 || state == 13 || state == 14 || state == 15) && rrj_u16(rrj_at(m, actor_event + 2, 2)) == other_id && (rrj_u16(rrj_at(m, actor_event + 6, 2)) & 0x4000))
        flags |= 0x20;
    if (*(uint8_t *)rrj_at(m, rrj_read32(m, actor + 1084) + 71, 1) == other_id)
        flags |= 0x800;
    if (other_event && !rrj_u16(rrj_at(m, other_event, 2)) && (rrj_u16(rrj_at(m, other_event + 6, 2)) & 0x4000))
        flags |= 0x400;
    return flags;
}

uint32_t sub_800B8FB0(RRJMemory *m, uint32_t actor, int32_t direction, uint32_t other, RRJRaceLeafCall call)
{
    uint32_t descriptor = rrj_read32(m, actor + 1084);
    uint32_t config = 0x80052EE4 + 36 * (*(uint8_t *)rrj_at(m, descriptor + 1, 1) & 0xF);
    int32_t delta = rrj_s32((rrj_read32(m, actor + 324) - rrj_read32(m, other + 324)) << 4);
    uint32_t group;
    int32_t slot = *(int8_t *)rrj_at(m, actor + 946, 1);
    uint32_t event = actor + 8 * slot + 948;
    uint16_t type;
    uint8_t packet[8] = {0};

    FUNCTION_MARKER(0x800B8FB0, "RASHCDG.BIN");
    direction = (int8_t)direction;
    group = direction >= *(int8_t *)rrj_at(m, config + 4, 1);
    group += direction >= *(int8_t *)rrj_at(m, config + 5, 1);
    if ((*(uint8_t *)rrj_at(m, descriptor + 2, 1) & 0xF) >= *(uint8_t *)rrj_at(m, config + 17, 1))
        group |= 4;

    switch (group)
    {
        case 0:
        case 4:
        {
            uint32_t magnitude = delta < 0 ? 0u - (uint32_t)delta : (uint32_t)delta;
            if (sub_800BC1EC(m, actor, other, delta) && magnitude < rrj_read32(m, 0x80052F70))
                type = 9;
            else
            {
                if (!sub_800BBD44(m, actor, other, delta))
                    return 0;
                type = 8;
            }
            break;
        }
        case 1:
        case 2:
        case 5:
            if (!sub_800BC1EC(m, actor, other, delta))
                return 0;
            type = 16;
            (void)sub_800B92C0(m, descriptor);
            break;
        case 6:
        {
            uint32_t index;
            uint8_t maximum;
            if (!sub_800BC1EC(m, actor, other, delta))
                return 0;
            if (rrj_u16(rrj_at(m, other + 172, 2)) < rrj_read32(m, rrj_read32(m, 0x8005B2F8) + 48))
                (void)sub_8001A760(m, rrj_u16(rrj_at(m, actor + 172, 2)), 0, call);
            type = 16;
            if (rrj_u16(rrj_at(m, descriptor + 44, 2)) & 0x1FF)
            {
                maximum = 0x8F;
                *(uint8_t *)rrj_at(m, descriptor + 60, 1) = maximum;
                for (index = 0; index < 8; ++index)
                {
                    uint8_t value = *(uint8_t *)rrj_at(m, descriptor + 52 + index, 1);
                    if (maximum < value)
                    {
                        maximum = value;
                        *(uint8_t *)rrj_at(m, descriptor + 60, 1) = value;
                    }
                }
                (void)sub_800B9340(m, descriptor);
            }
            else
            {
                *(uint8_t *)rrj_at(m, descriptor + 46, 1) = 9;
                *(uint8_t *)rrj_at(m, descriptor + 47, 1) = 0;
                *(uint8_t *)rrj_at(m, descriptor + 60, 1) = 38;
            }
            break;
        }
        default:
            return 0;
    }

    if (rrj_u16(rrj_at(m, event + 2, 2)) == rrj_u16(rrj_at(m, other + 172, 2)))
    {
        uint16_t current = rrj_u16(rrj_at(m, event, 2));
        if (current == 6)
            return 0;
        if (current == 16 && type != 16)
        {
            uint32_t linked = rrj_read32(m, actor + 852);
            uint32_t state = rrj_u16(rrj_at(m, linked + 544, 2));
            if (rrj_u16(rrj_at(m, 0x800541D4 + 8 * state + 2, 2)) == 3)
            {
                uint32_t variant = *(uint8_t *)rrj_at(m, linked + 569, 1);
                uint32_t replacement = rrj_u16(rrj_at(m, rrj_read32(m, 0x8005AD4C) + 12 * variant, 2));
                (void)sub_800C4550(m, replacement, linked, 2);
            }
        }
        rrj_put16(rrj_at(m, event, 2), type);
        return 1;
    }
    rrj_put16(packet, type);
    rrj_put16(packet + 2, rrj_u16(rrj_at(m, other + 172, 2)));
    return rrj_enqueue_actor_event_local(m, packet, 2, actor);
}

uint32_t sub_8009DC90(RRJMemory *m, uint32_t actor, RRJRaceLeafCall call)
{
    uint32_t linked;
    uint32_t descriptor;
    uint32_t selected;
    uint32_t other;
    uint32_t state;
    uint32_t index;
    int32_t longitudinal;
    int32_t ratio;
    uint8_t packet[8] = {0};

    FUNCTION_MARKER(0x8009DC90, "RASHCDG.BIN");
    if (*(int8_t *)rrj_at(m, actor + 946, 1) <= 0)
    {
        rrj_put16(packet, 4);
        rrj_put16(packet + 2, 224);
        (void)rrj_enqueue_actor_event_local(m, packet, 1, actor);
    }
    linked = rrj_read32(m, actor + 852);
    if (rrj_read32(m, linked + 604) >= 3 || (rrj_read32(m, linked + 552) & 0x40))
        return 0;
    state = rrj_read32(m, 0x8005B2F8);
    if (*(uint8_t *)rrj_at(m, state + 4, 1) == 44 && *(uint8_t *)rrj_at(m, state + 57, 1) >= 4)
        return 0;
    for (index = 0; index < rrj_read32(m, state + 48); ++index)
    {
        uint32_t player = rrj_read32(m, 0x8005B268 + 4 * index);
        if (*(uint8_t *)rrj_at(m, rrj_read32(m, player + 1084) + 39, 1) == 0xFE)
            rrj_write32(m, player + 720, 0x20000);
    }
    descriptor = rrj_read32(m, actor + 1084);
    (void)sub_8009FC80(m, rrj_u16(rrj_at(m, descriptor + 64, 2)));
    selected = sub_8009E178(m, actor) & 0xFFFFu;
    if (selected == 224 || (selected >> 5))
        return 0;
    other = rrj_read32(m, 0x8005B3A0) + 1096 * selected;
    if (!other || other == actor)
        return 0;
    (void)rrj_u16(rrj_at(m, other + 8 * (*(int8_t *)rrj_at(m, other + 946, 1) - 1) + 956, 2));
    longitudinal = rrj_s32(sub_8009E444(m, actor, other));
    if (!sub_8009DB58(m, actor, other))
        return 0;
    {
        int32_t speed = rrj_s32(rrj_read32(m, actor + 480));
        int32_t numerator = (int32_t)sub_8001FC90(speed, speed);
        int32_t denominator = rrj_s32(rrj_read32(m, actor + 600) * 2);
        uint32_t quotient;
        if (!denominator)
            ratio = 0;
        else
        {
            uint32_t n = numerator < 0 ? 0u - (uint32_t)numerator : (uint32_t)numerator;
            uint32_t d = denominator < 0 ? 0u - (uint32_t)denominator : (uint32_t)denominator;
            quotient = sub_80010028(n, d);
            ratio = (numerator < 0) != (denominator < 0) ? -rrj_s32(quotient) : rrj_s32(quotient);
        }
    }
    if (sub_8009DBA0(m, actor, other, longitudinal, ratio))
    {
        (void)sub_800BCD10(m, actor);
        memset(packet, 0, sizeof(packet));
        rrj_put16(packet, 2);
        rrj_put16(packet + 2, 224);
        (void)rrj_enqueue_actor_event_local(m, packet, 1, actor);
        rrj_write32(m, descriptor + 40, rrj_read32(m, state + 16));
        return 1;
    }
    {
        uint32_t other_linked = rrj_read32(m, other + 852);
        uint32_t other_mode = rrj_read32(m, other_linked + 604);
        int32_t ordering = rrj_s32(sub_8009E3F0(m, actor, other));
        uint32_t other_ready = other_mode >= 3 || rrj_s32(rrj_read32(m, other + 480)) <= 585944;
        uint32_t same_road = rrj_read32(m, other + 360) == rrj_read32(m, actor + 360);
        uint32_t table_index = rrj_read32(m, state + 60) + ((0u - (*(uint8_t *)rrj_at(m, state + 4, 1) & 1u)) & 3u);
        uint32_t within = rrj_s32(rrj_read32(m, 0x800530A8 + 4 * table_index)) >= longitudinal;
        int32_t slot = *(int8_t *)rrj_at(m, actor + 946, 1);
        uint32_t previous = rrj_u16(rrj_at(m, actor + 8 * (slot - 1) + 956, 2));
        uint16_t event_type = 0;

        if (ordering)
            longitudinal = -longitudinal;
        if (previous == 2)
        {
            if (rrj_read32(m, linked + 604) < 3 && within && rrj_s32(rrj_read32(m, actor + 480)) <= 585944 && *(uint8_t *)rrj_at(m, rrj_read32(m, other + 1084) + 39, 1) != 0xFE && other_ready)
            {
                (void)sub_8009E528(m, actor, other, call);
                return 0;
            }
            if (within)
                return 0;
            (void)sub_800BCD10(m, actor);
            rrj_write32(m, descriptor + 40, 0);
            selected = 224;
            event_type = 4;
        }
        else if (previous == 4)
        {
            if (other_ready || !same_road)
            {
                if (!sub_800BC120(m, actor, other, longitudinal))
                    event_type = 17;
            }
            else if (sub_800BB8FC(m, actor, other, longitudinal))
                event_type = 6;
            else if (sub_800BC1EC(m, actor, other, longitudinal))
            {
                event_type = 16;
                (void)sub_800B92C0(m, descriptor);
            }
        }
        if (event_type)
        {
            memset(packet, 0, sizeof(packet));
            rrj_put16(packet, event_type);
            rrj_put16(packet + 2, (uint16_t)selected);
            (void)rrj_enqueue_actor_event_local(m, packet, 1, actor);
        }
    }
    return 0;
}

uint32_t sub_8009B474(RRJMemory *m, int32_t mode, uint32_t delta, RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x8009B474, "RASHCDG.BIN");
    if (mode == 3)
        return batch_002_call(m, call, 0x8009CFF4, delta, 0);
    if (mode < 4 && !mode)
        return batch_002_call(m, call, 0x8009E89C, delta, 0);
    return mode < 4;
}

uint32_t sub_8009E89C(RRJMemory *m, uint32_t delta, RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x8009E89C, "RASHCDG.BIN");
    return batch_002_call(m, call, 0x8009E8A4, delta, rrj_read32(m, 0x8005ACC0));
}

uint32_t sub_800A01CC(RRJMemory *m, uint32_t active[2])
{
    uint32_t state = rrj_read32(m, 0x8005B2F8);
    uint32_t index;

    FUNCTION_MARKER(0x800A01CC, "RASHCDG.BIN");
    if (!(*(uint8_t *)rrj_at(m, state + 4, 1) & 0x10) || !active[0] || !active[1])
        return 0;
    {
        uint32_t first = rrj_read32(m, 0x8005B38C);
        uint32_t second = rrj_read32(m, 0x8005B21C);
        int32_t dx = (int16_t)rrj_u16(rrj_at(m, first + 186, 2)) - (int16_t)rrj_u16(rrj_at(m, second + 186, 2));
        int32_t dz = (int16_t)rrj_u16(rrj_at(m, first + 194, 2)) - (int16_t)rrj_u16(rrj_at(m, second + 194, 2));
        int32_t major;
        int32_t minor;
        int32_t distance;
        int32_t threshold;
        if (dx < 0)
            dx = -dx;
        if (dz < 0)
            dz = -dz;
        major = dx >= dz ? dx : dz;
        minor = dx >= dz ? dz : dx;
        distance = major - (major >> 5) - (major >> 7) + ((minor + (minor >> 1)) >> 2) + ((minor + (minor >> 1)) >> 6);
        threshold = (rrj_s32(rrj_read32(m, 0x80053114 + 4 * rrj_read32(m, state + 60))) >> 1) + 100;
        if (distance >= threshold)
            return 0;
    }
    {
        uint32_t side[2] = {0, 0};
        for (index = 0; index < rrj_read32(m, state + 48) && index < 2; ++index)
        {
            uint32_t player = rrj_read32(m, 0x8005B268 + 4 * index);
            uint32_t record = 0x800CD898 + 1132 * (index ^ 1);
            int32_t x = rrj_s32(rrj_read32(m, player + 184) - rrj_read32(m, record + 184));
            int32_t y = rrj_s32(rrj_read32(m, player + 188) - rrj_read32(m, record + 188));
            int32_t z = rrj_s32(rrj_read32(m, player + 192) - rrj_read32(m, record + 192));
            int64_t dot = (int64_t)x * ((int16_t)rrj_u16(rrj_at(m, record + 444, 2)) * 16) + (int64_t)y * ((int16_t)rrj_u16(rrj_at(m, record + 446, 2)) * 16) + (int64_t)z * ((int16_t)rrj_u16(rrj_at(m, record + 448, 2)) * 16);
            side[index] = (int32_t)(dot >> 16) >= 0;
        }
        if (side[0] == side[1])
        {
            if (rrj_s32(sub_8002E698(m, 0x800CDA54, 0x800CDEC0)) < 0)
                for (index = 0; index < rrj_read32(m, state + 48) && index < 2; ++index)
                    active[index] = 0;
        }
        else if (side[0])
            active[0] = 0;
        else
            active[1] = 0;
    }
    return 0;
}

uint32_t sub_8003C520(RRJMemory *m, uint32_t object)
{
    uint32_t entry = rrj_read32(m, object + 4);
    uint32_t route;

    FUNCTION_MARKER(0x8003C520, "SLUS_010.53");
    if ((int16_t)rrj_u16(rrj_at(m, entry + 2, 2)) == 1)
        return rrj_read32(m, rrj_read32(m, object + 8) + 12);
    route = sub_800245DC(m, rrj_read32(m, entry + 12));
    if (route)
        return (rrj_read32(m, route + 4) >> 6) << 16;
    return 0;
}

uint32_t sub_80094184(RRJMemory *m, uint32_t actor)
{
    uint32_t descriptor;
    uint32_t state;
    uint32_t attempts;
    uint8_t previous;
    int32_t mode;
    int32_t base;

    FUNCTION_MARKER(0x80094184, "RASHCDG.BIN");
    if (!actor)
        return 0;
    descriptor = rrj_read32(m, actor + 1084);
    if ((*(uint8_t *)rrj_at(m, descriptor + 1, 1) & 0xF) != 2 || !*(uint8_t *)rrj_at(m, 0x800D8703, 1))
        return 0;
    *(uint8_t *)rrj_at(m, descriptor + 12, 1) = *(uint8_t *)rrj_at(m, 0x800D86FC, 1);
    *(uint8_t *)rrj_at(m, descriptor + 13, 1) = *(uint8_t *)rrj_at(m, 0x800D86FD, 1);
    *(uint8_t *)rrj_at(m, descriptor + 15, 1) = *(uint8_t *)rrj_at(m, 0x800D86FF, 1);
    *(uint8_t *)rrj_at(m, descriptor + 14, 1) = *(uint8_t *)rrj_at(m, 0x800D86FE, 1);
    *(uint8_t *)rrj_at(m, descriptor + 2, 1) = *(uint8_t *)rrj_at(m, 0x800D8700, 1);
    *(uint8_t *)rrj_at(m, descriptor + 61, 1) = *(uint8_t *)rrj_at(m, 0x800D8701, 1);
    *(uint8_t *)rrj_at(m, descriptor + 36, 1) = *(uint8_t *)rrj_at(m, 0x800D8702, 1);
    *(uint8_t *)rrj_at(m, descriptor + 37, 1) = *(uint8_t *)rrj_at(m, 0x800D8702, 1);
    rrj_put16(rrj_at(m, descriptor + 44, 2), rrj_u16(rrj_at(m, 0x800D8704, 2)));
    *(uint8_t *)rrj_at(m, descriptor + 46, 1) = *(uint8_t *)rrj_at(m, 0x800D8706, 1);
    *(uint8_t *)rrj_at(m, descriptor + 47, 1) = 0;
    rrj_write32(m, descriptor + 48, 0);
    rrj_write32(m, rrj_read32(m, actor + 556) + 224, rrj_read32(m, 0x800D86F8));
    state = rrj_read32(m, 0x8005B2F8);
    mode = rrj_s32(rrj_read32(m, state + 60));
    base = (mode > 0 ? mode : 0) + (2 - mode < 0 ? 2 - mode : 0);
    base = 8 * base + 66;
    previous = *(uint8_t *)rrj_at(m, descriptor + 38, 1);
    for (attempts = 0; attempts < 8; ++attempts)
    {
        uint8_t value = (uint8_t)(base + (sub_8001FC58(m) & 7));
        *(uint8_t *)rrj_at(m, descriptor + 38, 1) = value;
        if (value != previous)
            break;
    }
    return 0;
}

uint32_t sub_80012C1C(RRJMemory *m, uint32_t source, uint32_t destination, int32_t offset)
{
    uint32_t sign = (uint32_t)offset >> 31;
    uint32_t distance = ((uint32_t)offset + sign) ^ sign;
    uint32_t route_id;

    FUNCTION_MARKER(0x80012C1C, "SLUS_010.53");
    (void)sub_8001E0B4(m, destination, source, 12);
    if (offset < 0)
        rrj_write32(m, destination + 4, 0u - rrj_read32(m, destination + 4));
    route_id = rrj_u16(rrj_at(m, destination, 2));

    while (rrj_s32(distance) > 0)
    {
        uint32_t route_word = rrj_read32(m, destination);

        if (!(route_word >> 16))
        {
            uint32_t route = sub_800245DC(m, route_word & 0xFFFFu);
            uint32_t length = (rrj_read32(m, route + 4) >> 6) << 16;
            int32_t position = rrj_s32(rrj_read32(m, destination + 8));

            if (rrj_s32(rrj_read32(m, destination + 4)) <= 0)
                position = rrj_s32((uint32_t)position - distance);
            else
                position = rrj_s32((uint32_t)position + distance);
            rrj_write32(m, destination + 8, (uint32_t)position);
            if (position < 0)
            {
                route_id = rrj_read32(m, route + 8);
                distance = 0u - (uint32_t)position;
            }
            else if ((uint32_t)position > length)
            {
                route_id = rrj_read32(m, route + 12);
                distance = (uint32_t)position - length;
            }
            else
            {
                distance = 0;
                continue;
            }
            rrj_write32(m, destination, (route_id & 0xFFFFu) | 0x10000u);
            rrj_write32(m, destination + 8, 0);
        }
        else
        {
            uint32_t junction = sub_800245F4(m, route_id);
            uint32_t count;
            uint32_t selected = 0;
            uint32_t found = 0;

            if (!junction || (count = rrj_read32(m, junction + 4)) < 2)
            {
                distance = 0;
                continue;
            }
            if ((int16_t)rrj_u16(rrj_at(m, 0x800D6182u, 2)) != -1)
            {
                uint32_t i;

                if (offset <= 0)
                {
                    for (i = 0; i < count; ++i)
                    {
                        uint32_t entry = junction + 8 + 8 * i;
                        uint32_t link = sub_8003B4B0(m, 0, rrj_read32(m, entry));

                        if (link && ((rrj_s32(rrj_read32(m, link + 4)) < 0 && rrj_s32(rrj_read32(m, entry + 4)) > 0) || (rrj_s32(rrj_read32(m, link + 4)) > 0 && rrj_s32(rrj_read32(m, entry + 4)) < 0)))
                        {
                            selected = entry;
                            found = 1;
                            break;
                        }
                    }
                }
                else
                {
                    uint32_t connected = sub_8003F408(m, 0, route_id);

                    if (connected)
                    {
                        uint32_t connected_count = rrj_read32(m, connected + 16);
                        uint32_t choice = connected_count < 2 ? rrj_read32(m, connected + 84) : rrj_read32(m, connected + 84 + 4 * (sub_8001FC58(m) % connected_count));

                        for (i = 0; i < count; ++i)
                        {
                            uint32_t entry = junction + 8 + 8 * i;

                            if (rrj_read32(m, entry) == choice)
                            {
                                selected = entry;
                                found = 1;
                                break;
                            }
                        }
                    }
                }
            }
            if (!found)
                selected = junction + 8 + 8 * (sub_8001FC58(m) % count);
            rrj_write32(m, destination, rrj_u16(rrj_at(m, selected, 2)));
            rrj_write32(m, destination + 4, rrj_read32(m, selected + 4));
            if (rrj_s32(rrj_read32(m, selected + 4)) <= 0)
            {
                uint32_t route = sub_800245DC(m, rrj_u16(rrj_at(m, destination, 2)));

                rrj_write32(m, destination + 8, (rrj_read32(m, route + 4) >> 6) << 16);
            }
            else
                rrj_write32(m, destination + 8, 0);
        }
    }

    rrj_write32(m, destination + 4, 1);
    if (rrj_read32(m, destination) == rrj_read32(m, source))
        rrj_write32(m, destination + 4, rrj_read32(m, source + 4));
    if ((int16_t)rrj_u16(rrj_at(m, 0x800D6182u, 2)) == -1)
        return 0xFFFFFFFFu;
    if (rrj_read32(m, destination) >> 16)
        return rrj_read32(m, destination) >> 16;
    {
        uint32_t link = sub_8003B4B0(m, 0, rrj_u16(rrj_at(m, destination, 2)));

        if (link)
        {
            uint32_t direction = rrj_read32(m, link + 4);

            rrj_write32(m, destination + 4, direction);
            return direction;
        }
    }
    return 0;
}
