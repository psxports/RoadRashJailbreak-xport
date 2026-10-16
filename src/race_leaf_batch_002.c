#include "race_global_800B8020.h"
#include "native_game_api.h"
#include "race_leaf_batch_002.h"
#include "race_global_8009E8A4.h"
#include "fixed_math.h"
#include "menu_latch.h"
#include "race_pause.h"
#include "race_pause_frontier.h"
#include "xport.h"
#include <string.h>

uint32_t sub_8009C308(void)
{
    FUNCTION_MARKER(0x8009C308, "RASHCDG.BIN");
    return sub_8009C310();
}

uint32_t sub_8009C310(void)
{
    const uint32_t roads = 0x801FFAE0u;
    int32_t player_count = ((int32_t)(uint32_t)(r_u32(r_u32(0x8005B2F8) + 48)));
    int32_t player;

    FUNCTION_MARKER(0x8009C310, "RASHCDG.BIN");
    for (player = 0; player < player_count; ++player)
    {
        int32_t road_count = (int32_t)sub_8009FAD8(roads, (uint32_t)player);
        uint32_t owner = r_u32(0x8005B268u + 4u * (uint32_t)player);
        uint32_t allow_special = sub_8003B8F4(owner + 172);
        int32_t index;

        for (index = 0; index < road_count; ++index)
        {
            uint32_t road_id = r_u32(roads + 4u * (uint32_t)index);
            uint32_t record = sub_80013204(road_id, (uint32_t)player);

            if (record && r_u32(record + 8) == road_id)
                (void)sub_8009CA88(road_id, r_u32(r_u32(record + 4) + 32), r_u32(record + 76), allow_special, (uint32_t)player);
        }
        player_count = ((int32_t)(uint32_t)(r_u32(r_u32(0x8005B2F8) + 48)));
    }
    return 0;
}

uint32_t sub_8009C654(uint32_t record, uint32_t identity, uint32_t player)
{
    uint32_t result = 0;
    uint32_t kind;

    FUNCTION_MARKER(0x8009C654, "RASHCDG.BIN");
    if ((int16_t)r_u16(record + 4) > 0)
        return 0;
    kind = r_u16(record);
    switch (kind)
    {
        case 0:
            if (((int32_t)(uint32_t)(r_u32(0x800D86F4))) <= 0 && ((int32_t)(uint32_t)((sub_8001FC58() % 11u) << 16)) < ((int32_t)(uint32_t)(r_u32(record + 48))))
            {
                uint32_t actor = sub_80095848();

                if (actor && !(r_u32(record + 8) >> 16))
                {
                    uint32_t linked = r_u32(actor + 852);

                    w_u32(actor + 360, r_u32(record + 8));
                    w_u32(actor + 368, r_u32(record + 36));
                    w_u32(actor + 364, (uint32_t)(int32_t)r_s16(record + 60));
                    w_u32(actor + 344, r_u32(record + 32));
                    (void)sub_8002090C(actor);
                    w_u16(actor + 320, 1);
                    w_u32(actor + 924, 0);
                    w_u32(linked + 604, 1);
                    (void)sub_80093F94(actor, 0);
                    result = 1;
                }
            }
            break;

        case 2:
            if (sub_8008CDF4(2))
            {
                uint32_t owner = r_u32(0x8005B268u + 4u * player);
                int32_t difference = ((int32_t)(uint32_t)(r_u32(owner + 368) - r_u32(record + 36)));
                int32_t sign = difference >> 31;

                difference = ((int32_t)(uint32_t)(((uint32_t)difference + (uint32_t)sign) ^ (uint32_t)sign));
                if (r_u32(record + 8) != r_u32(owner + 360) || !r_u32(r_u32(0x8005B2F8) + 16) || difference > 0)
                    result = sub_800CB8C8(record, 1, owner) != 0;
            }
            break;

        case 4:
        {
            uint32_t selector = r_u16(record + 2);

            if (selector == 50)
            {
                if (r_u8(r_u32(0x8005B2F8) + 4) & 0x10u)
                {
                    (void)sub_80012BA8(record, identity, player);
                    result = 1;
                }
            }
            else if (selector != 9 && selector)
            {
                int32_t material = ((int32_t)(uint32_t)(sub_8009C5E4((int32_t)selector)));
                uint32_t owner;

                if (material < 0)
                    break;
                owner = r_u32(0x8005B268u + 4u * player);
                if ((uint32_t)material < 6)
                {
                    if (sub_8008CDF4(4))
                        result = sub_800A2630(record, owner) != 0;
                }
                else if (sub_8008CDF4(5))
                    result = sub_800A2448(record, owner) != 0;
            }
            else if (!(r_u8(r_u32(0x8005B2F8) + 4) & 0x10u) && ((int32_t)(uint32_t)(r_u32(0x8005B314))) < 3)
            {
                int32_t table_index;

                for (table_index = 0; table_index < 2; ++table_index)
                {
                    uint32_t entry = 0x8005B328u + 4u * (uint32_t)table_index;
                    int32_t entry_selector = r_s8(entry);

                    if (entry_selector < 0)
                        break;
                    if ((uint32_t)entry_selector == selector)
                    {
                        int32_t config_index = r_s8(entry + 1);
                        int32_t sign = config_index >> 31;
                        uint32_t config;

                        config_index = (int32_t)(((uint32_t)config_index + (uint32_t)sign) ^ (uint32_t)sign);
                        if (config_index >= 6)
                        {
                            config_index = ((int32_t)(uint32_t)(r_u32(0x8005B260)));
                            while (config_index < 6)
                            {
                                config = r_u32(0x8005B24C) + 312u * (uint32_t)config_index;
                                if (!r_u8(config))
                                {
                                    w_u8(config, 0xFFu);
                                    break;
                                }
                                ++config_index;
                            }
                        }
                        if (config_index < 6)
                        {
                            config = r_u32(0x8005B24C) + 312u * (uint32_t)config_index;
                            (void)sub_800A0A20(selector, 1, xport_guest_cptr(record + 20, 12), 0, config);
                            result = 1;
                        }
                        break;
                    }
                }
            }
            break;
        }

        case 6:
            if (sub_8008CDF4(6) && sub_8008D9E4(player))
            {
                uint32_t object = sub_8009BB48(record, r_u32(0x8005B268u + 4u * player));

                if (object)
                    result = r_u16(object);
            }
            break;

        default:
            break;
    }
    if (result)
        w_u16(record + 4, (uint16_t)result);
    return result;
}

uint32_t sub_8009CA88(uint32_t road_id, uint32_t table, uint32_t candidates, uint32_t allow_special, uint32_t player)
{
    const uint32_t roads = 0x801FFB00u;
    uint32_t road_count;
    uint32_t destination;
    uint32_t index;

    FUNCTION_MARKER(0x8009CA88, "RASHCDG.BIN");
    if (!table || sub_8009F054(candidates, player))
        return 0;
    destination = sub_80013360(road_id, player);
    road_count = sub_8009EF8C(candidates, roads, 6);
    if (!road_count)
        return 0;

    if (r_u32(r_u32(0x8005B2F8) + 48) == 1)
    {
        uint32_t count = r_u16(table + 2);
        uint32_t record = r_u32(table + 40);

        for (index = 0; index < count; ++index, record += 76)
            if ((int16_t)r_u16(record + 4) <= 0 && sub_8009C4FC(record, roads, (int32_t)road_count, player))
                (void)sub_8009C654(record, r_u32(table + 12), player);
    }

    {
        uint32_t count = r_u16(table);
        uint32_t record = r_u32(table + 36);

        for (index = 0; index < count; ++index, record += 68)
        {
            if ((int16_t)r_u16(record + 4) <= 0 && sub_8009C4FC(record, roads, (int32_t)road_count, player))
            {
                uint32_t identity = sub_8009C654(record, r_u32(table + 12), player);

                if (identity)
                    (void)sub_8009C41C(destination, 3, index, identity);
            }
        }
    }

    {
        uint32_t count = r_u16(table + 6);
        uint32_t record = r_u32(table + 48);

        for (index = 0; index < count; ++index, record += 88)
        {
            if ((int16_t)r_u16(record + 4) > 0)
            {
                if ((r_u8(r_u32(0x8005B2F8) + 4) & 0x10u) && sub_80013294(6, r_u16(record + 4), player))
                    (void)sub_8008D89C();
                continue;
            }
            if (!sub_8009C4FC(record, roads, (int32_t)road_count, player))
                continue;
            {
                uint32_t identity = sub_8009C654(record, r_u32(table + 12), player);

                if ((uint16_t)(identity - 1u) < 223u)
                {
                    uint32_t object = r_u32(0x800CD6C4) + 280u * (identity & 31u);

                    if (object)
                    {
                        w_u8(object + 2, (uint8_t)index);
                        w_u8(object + 3, (uint8_t)(1u << player));
                        w_u32(object + 4, r_u32(table + 12));
                    }
                    (void)sub_8009C41C(destination, 6, index, identity);
                    (void)sub_8008D89C();
                }
                else
                    (void)sub_8008DA20(record, player);
            }
        }
    }

    {
        uint32_t count = r_u16(table + 4);
        uint32_t record = r_u32(table + 44);

        for (index = 0; index < count; ++index, record += 64)
        {
            uint32_t selector;
            uint32_t identity;

            if ((int16_t)r_u16(record + 4) > 0)
                continue;
            selector = r_u16(record + 2);
            if ((!allow_special && selector == 30) || (selector == 50 && !(r_u8(r_u32(0x8005B2F8) + 4) & 0x10u)))
                continue;
            if (selector == 50)
            {
                if ((r_u32(record + 8) & 0xFFFE0000u) && !sub_8009F288(record, roads, (int32_t)road_count))
                    continue;
            }
            else if (!sub_8009C4FC(record, roads, (int32_t)road_count, player))
                continue;
            identity = sub_8009C654(record, r_u32(table + 12), player);
            if (identity && selector != 50)
                (void)sub_8009C41C(destination, 4, index, identity);
        }
    }

    {
        uint32_t count = r_u16(table + 8);
        uint32_t record = r_u32(table + 52);

        for (index = 0; index < count; ++index, record += 64)
        {
            if ((int16_t)r_u16(record + 4) <= 0 && sub_8009C4FC(record, roads, (int32_t)road_count, player))
            {
                uint32_t identity = sub_8009C654(record, r_u32(table + 12), player);

                if (identity)
                    (void)sub_8009C41C(destination, 0, index, identity);
            }
        }
    }
    return 0;
}

uint32_t sub_800C3950(uint32_t actor)
{
    uint32_t owner = r_u32(actor + 596);
    uint32_t animation = r_u32(actor + 540);
    int32_t motion = ((int32_t)(uint32_t)(r_u32(owner + 672)));
    int32_t magnitude = motion < 0 ? (int32_t)(0u - (uint32_t)motion) : motion;
    uint32_t animation_index = r_u32(animation + 12);
    uint32_t animation_entry = r_u32(animation + 4) + 12u * animation_index;
    uint32_t animation_table = r_u32(r_u32(animation + 40) + 4);
    uint32_t animation_record = r_u32(animation_table + 4u * r_u8(animation_entry));
    int32_t multiplier = (int16_t)r_u16(animation_record + 16) - 1;
    uint32_t mirrored = (r_u8(actor + 572) >> 5) & 1u;
    uint32_t reverse = 0;
    int32_t limit = multiplier << 16;
    uint32_t state = r_u16(actor + 544);
    uint32_t state_kind = r_u16(0x800541D6u + 8u * state);

    FUNCTION_MARKER(0x800C3950, "RASHCDG.BIN");
    if (mirrored)
    {
        if (((int32_t)(uint32_t)(r_u32(actor + 76))) <= 0xFFFF && motion < 0)
            limit = 229376;
        if (!magnitude)
        {
            uint32_t linked = r_u32(owner + 856);

            if (!(r_u32(linked + 564) & 2u))
            {
                magnitude = ((int32_t)(uint32_t)(r_u32(linked + 672)));
                motion = ((int32_t)(uint32_t)(r_u32(owner + 636))) < 0 ? -magnitude : magnitude;
                limit = motion > 0 ? 0x40000 : 98304;
            }
        }
    }
    else if (((int32_t)(uint32_t)(r_u32(owner + 636))) > 0)
    {
        reverse = 1;
        if (r_u32(owner + 856) && r_u32(owner + 1088))
            limit = 327680;
    }

    {
        int32_t now = ((int32_t)(uint32_t)(r_u32(r_u32(0x8005B2F8) + 16)));

        if (now - ((int32_t)(uint32_t)(r_u32(actor + 548))) >= 301)
        {
            int32_t speed = ((int32_t)(uint32_t)(r_u32(owner + 480))) >> 16;
            int32_t duration = speed < 0 ? 150 : 150 - speed;

            if (duration < 0)
                duration = 0;
            if (magnitude > 58982)
            {
                w_u32(actor + 548, (uint32_t)now);
                (void)sub_800273EC(actor, ((int32_t)(uint32_t)(r_u32(owner + 636))) > 0 ? 15 : 12, (uint32_t)duration, 3, 2);
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
            uint32_t flags = r_u32(actor + 552);

            w_u32(actor + 552, state_kind == 2 ? flags & 0xFFBFFFFFu : flags | 0x400000u);
            if (mirrored)
                state = state == 79 ? (motion > 0 ? 82u : 81u) : (motion > 0 ? 75u : 74u);
            else
                state = r_u32(owner + 180) >= 9u ? (state == 13 ? 20u : 18u) : 19u;
            w_u16(actor + 544, (uint16_t)state);
            (void)sub_8005C0B0(animation, (r_u32(0x800541D4u + 8u * state) >> 4) & 0xFFFu, reverse, 0, r_u32(0x8005B3E4) ? r_u32(r_u32(0x8005B3E4) + 72) : 0);
            if (r_u16(actor + 608))
                w_u16(actor + 608, 0);
        }
        {
            int32_t product = (int32_t)((uint32_t)magnitude * (uint32_t)multiplier);
            int32_t target = product < limit ? product : limit;
            uint32_t flags = r_u32(actor + 552);

            w_u8(animation_entry + 2, (uint8_t)reverse);
            if (flags & 0x400000u)
            {
                int32_t denominator = ((int32_t)(uint32_t)(r_u32(animation + 24)));
                int32_t current = ((int32_t)(uint32_t)(r_u32(animation + 16) << 16));

                if (denominator)
                    current += ((int32_t)(uint32_t)(r_u32(animation + 32) << 16)) / denominator;
                if (target - current > 0x20000)
                    target = current + 0x20000;
                else if (target - current < -0x20000)
                    target = current - 0x20000;
                else
                    w_u32(actor + 552, flags & 0xFFBFFFFFu);
            }
            w_u32(animation + 36, r_u32(animation + 36) | 8u);
            w_u32(animation + 16, (uint32_t)(target >> 16));
            w_u32(animation + 32, (uint32_t)((int64_t)((int32_t)(uint32_t)(r_u32(animation + 24))) * (target - (((int32_t)(uint32_t)(r_u32(animation + 16))) << 16)) >> 16));
            w_u32(animation + 28, 0);
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
        return sub_800C4550(next, actor, mode) & 0xFFu;
    }
    return 0;
}

uint32_t sub_800CB8C8(uint32_t record, uint32_t mode, uint32_t player)
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
    if (r_u32(0x800CE178) == r_u32(0x800CE17C) || !r_u16(0x800CE580))
        return 0;
    w_u32(scratch, r_u32(record + 8));
    w_u32(scratch + 4, (uint32_t)(int32_t)r_s16(record + 60));
    w_u32(scratch + 8, r_u32(record + 36));
    road = sub_80039DFC(0, ((scratch) ? (const uint32_t[3]){r_u32(scratch), 0, (uint32_t)r_u16((scratch) + 10) << 16} : NULL));
    if (!road)
        return 0;

    selected = ((int32_t)(uint32_t)(r_u32(0x800D4B74)));
    if (r_u32(0x800D4B80) && selected < 4 && ((int32_t)(uint32_t)(r_u32(0x800D4B70))) < ((int32_t)(uint32_t)(r_u32(0x800D8744))))
    {
        int32_t next = selected + 1;
        uint32_t candidate = r_u32(0x800D4B80) + 572u * (uint32_t)next;

        while (next < 4)
        {
            if (!r_u16(candidate + 172) && r_u32(candidate + 4))
                break;
            ++next;
            candidate += 572;
        }
        w_u32(0x800D4B74, (uint32_t)next);
        actor = r_u32(0x800D4B80) + 572u * (uint32_t)selected;
        w_u16(actor + 172, (uint16_t)(selected + 64));
        w_u32(0x800D4B70, r_u32(0x800D4B70) + 1);
        if (((int32_t)(uint32_t)(r_u32(0x800D4B78))) < selected)
            w_u32(0x800D4B78, (uint32_t)selected);
    }
    if (!actor)
        return 0;

    w_u8(actor + 567, 0xFFu);
    w_u32(actor + 56, 0);
    if (r_u32(record + 44))
        w_u32(record + 20, r_u32(record + 20) + (sub_8001FC58() % r_u32(record + 44) << 16));
    if (r_u32(record + 40))
        w_u32(record + 28, r_u32(record + 28) + (sub_8001FC58() % r_u32(record + 40) << 16));
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
    (void)sub_8001E0B4(xport_guest_ptr(actor + 328, 32), xport_guest_ptr(scratch + 16, 1), 32);
    basis = r_u32(actor + 340);
    w_u32(actor + 180, sub_8002FAD4(actor, 4, 0xFFFFu, 0));
    if (r_u32(actor + 180) == 0xFFFFu)
        goto cleanup;

    type = r_u16(record + 2);
    if (type < 19)
    {
        subtype = type;
        w_u8(actor + 566, (uint8_t)subtype);
        if (!r_u16(0x800CCC20u + 16u * subtype))
            w_u8(actor + 564, 0);
        else
            w_u8(actor + 564, (sub_8001FC58() & 1u) ? 1 : 2);
    }
    else if (type == 19 || type == 20)
    {
        subtype = sub_8001FC58() % 13u;
        w_u8(actor + 564, type == 19 ? 1 : 2);
        w_u8(actor + 566, (uint8_t)subtype);
    }
    else if (type == 21)
    {
        subtype = sub_8001FC58() % 13u;
        w_u8(actor + 566, (uint8_t)subtype);
        w_u8(actor + 564, r_u16(0x800CCC20u + 16u * subtype) ? ((sub_8001FC58() & 1u) ? 1 : 2) : 0);
    }
    else if (type == 22)
    {
        w_u8(actor + 564, 0);
        w_u8(actor + 566, (uint8_t)(sub_8001FC58() % 6u + 13u));
    }
    else
    {
        subtype = sub_8001FC58() % 19u;
        w_u8(actor + 566, (uint8_t)subtype);
        w_u8(actor + 564, r_u16(0x800CCC20u + 16u * subtype) ? ((sub_8001FC58() & 1u) ? 1 : 2) : 0);
    }
    (void)sub_80012FC8(actor, 0);

    if (mode)
    {
        w_u32(actor + 184, r_u32(record + 20));
        w_u32(actor + 188, r_u32(record + 24));
        w_u32(actor + 192, r_u32(record + 28));
        sub_80036800(actor + 184, basis, xport_guest_ptr(actor + 344, 4), xport_guest_ptr(actor + 348, 4));
    }
    else
    {
        w_u32(actor + 184, r_u32(basis + 20));
        w_u32(actor + 188, r_u32(basis + 24));
        w_u32(actor + 192, r_u32(basis + 28));
        (void)sub_8002EAD8(xport_guest_ptr(basis + 20, 12), xport_guest_ptr(basis + 14, 6), r_u32(actor + 348), xport_guest_ptr(actor + 184, 12));
    }

    subtype = r_u8(actor + 566);
    w_u32(actor + 480, r_u32(0x800CCC28u + 16u * subtype));
    w_u16(actor + 438, 0);
    w_u16(actor + 440, 4096);
    w_u16(actor + 442, 0);
    w_u16(actor + 444, (uint16_t)-(int16_t)r_u16(basis + 14));
    w_u16(actor + 446, (uint16_t)-(int16_t)r_u16(basis + 16));
    w_u16(actor + 448, (uint16_t)-(int16_t)r_u16(basis + 18));
    w_u32(actor + 188, r_u32(actor + 188) - 0x10000u);
    w_u16(actor + 432, r_u16(actor + 448));
    w_u16(actor + 434, 0);
    w_u16(actor + 436, (uint16_t)-(int16_t)r_u16(actor + 444));
    w_u16(actor + 450, r_u16(actor + 444));
    w_u16(actor + 452, r_u16(actor + 446));
    w_u16(actor + 454, r_u16(actor + 448));
    {
        const uint16_t route_direction[3] = {r_u16(actor + 450), r_u16(actor + 450 + 2), r_u16(actor + 450 + 4)};
        const uint32_t route_record[8] = {r_u32(actor + 328), r_u32(actor + 328 + 4), r_u32(actor + 328 + 8), r_u32(actor + 328 + 12), r_u32(actor + 328 + 16), r_u32(actor + 328 + 20), r_u32(actor + 328 + 24), r_u32(actor + 328 + 28)};
        uint32_t route_position[3] = {r_u32(actor + 360), r_u32(actor + 360 + 4), r_u32(actor + 360 + 8)};
        uint32_t route_result = sub_8003662C(route_direction, route_record, route_position);
        w_u32(actor + 360, route_position[0]);
        w_u32(actor + 360 + 4, route_position[1]);
        w_u32(actor + 360 + 8, route_position[2]);
        (void)route_result;
    }
    w_u32(actor + 364, (uint32_t)(int32_t)r_s16(record + 60));
    sub_8003AF9C(actor + 172, 0, player);
    w_u32(actor + 324, sub_8003B61C(actor + 172));
    w_u16(actor + 322, r_u16(record + 62));
    w_u32(actor + 316, 4915200);
    w_u32(actor + 556, r_u32(record + 64) << 4);
    w_u16(actor + 320, (uint16_t)sub_80039F68(actor + 172));
    if (!r_u16(actor + 320))
        goto cleanup;

    (void)sub_8003DE28(actor, 1, 0, 0xFFFFFFFFu);
    sub_8003DF54(actor, 1, 0xFFFFFFFFu);
    {
        int32_t offset = ((int32_t)(uint32_t)(r_u32(actor + 344)));

        if (offset < 0 && offset < ((int32_t)(uint32_t)(r_u32(actor + 400))))
            (void)sub_8002EAD8(xport_guest_ptr(actor + 184, 12), xport_guest_ptr(basis + 2, 6), r_u32(actor + 400) - (uint32_t)offset + 0x10000u, xport_guest_ptr(actor + 184, 12));
        else if (((int32_t)(uint32_t)(r_u32(actor + 412))) < offset)
            (void)sub_8002EAD8(xport_guest_ptr(actor + 184, 12), xport_guest_ptr(basis + 2, 6), r_u32(actor + 412) - (uint32_t)offset - 0x10000u, xport_guest_ptr(actor + 184, 12));
    }
    if (!mode)
    {
        int32_t offset = ((int32_t)(uint32_t)(r_u32(record + 32)));

        w_u32(actor + 344, (uint32_t)offset);
        if ((((offset >> 31) + offset) ^ (offset >> 31)) == 0x10000)
        {
            uint32_t linked = r_u32(actor + 372);

            if (linked)
            {
                if (offset <= 0)
                    offset = r_u16(linked + 10) ? ((int32_t)(uint32_t)(r_u32(linked + 32))) - 0x10000 : ((int32_t)(uint32_t)(r_u32(linked + 16))) + 0x10000;
                else
                    offset = r_u16(linked + 138) ? ((int32_t)(uint32_t)(r_u32(linked + 160))) + 0x10000 : ((int32_t)(uint32_t)(r_u32(linked + 144))) - 0x10000;
                w_u32(actor + 344, (uint32_t)offset);
            }
        }
        (void)sub_8002EAD8(xport_guest_ptr(actor + 184, 12), xport_guest_ptr(basis + 2, 6), r_u32(actor + 344), xport_guest_ptr(actor + 184, 12));
    }
    w_u32(actor + 552, 1);
    w_u32(actor + 540, 0);
    w_u8(actor + 565, 0);
    w_u32(actor + 560, 0);
    animation = sub_80012884(0x800CE170, actor);
    w_u32(actor + 540, animation);
    w_u8(actor + 72, 3);
    if (!animation)
        goto cleanup;
    (void)sub_800CAAF0(actor);
    (void)sub_800CAAA8(actor);
    (void)sub_800CB02C(actor);
    w_u32(actor + 176, 0xFFFFFFFFu);
    w_u32(actor + 548, r_u32(r_u32(0x8005B2F8) + 16));
    if (!r_u8(actor + 566))
        (void)sub_800CB78C(actor, 10, 9);
    return actor;

cleanup:
    (void)sub_8008C000(actor + 172, 2);
    return 0;
}

uint32_t sub_800B8018(uint32_t delta)
{
    FUNCTION_MARKER(0x800B8018, "RASHCDG.BIN");
    (void)r_u32(0x8005B2F8);
    return sub_800B8020(delta);
}

uint32_t sub_800A8C48(uint32_t value, uint32_t packed)
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

uint32_t sub_800BD34C(uint32_t value, int32_t delta)
{
    int32_t result = (r_u8(value) & 0xF) + delta;

    FUNCTION_MARKER(0x800BD34C, "RASHCDG.BIN");
    if (result < 0)
        result = 0;
    if (result > 15)
        result = 15;
    w_u8(value, (uint8_t)((r_u8(value) & 0xF0) | result));
    return (uint32_t)result;
}

uint32_t sub_8009DB58(uint32_t actor, uint32_t other)
{
    uint32_t descriptor = r_u32(other + 1084);

    FUNCTION_MARKER(0x8009DB58, "RASHCDG.BIN");
    return !r_u32(descriptor + 40) && r_u32(r_u32(actor + 852) + 604) < 3 && r_u8(descriptor + 39) < 0xF7;
}

uint32_t sub_8009E3F0(uint32_t other, uint32_t actor)
{
    uint32_t linked = r_u32(actor + 852);
    int32_t position;

    FUNCTION_MARKER(0x8009E3F0, "RASHCDG.BIN");
    position = ((int32_t)(uint32_t)(r_u32(linked + 604) < 3 ? r_u32(actor + 368) : r_u32(linked + 368)));
    if (((int32_t)(uint32_t)(r_u32(actor + 364))) <= 0)
        return ((int32_t)(uint32_t)(r_u32(other + 368))) < position;
    return position < ((int32_t)(uint32_t)(r_u32(other + 368)));
}

uint32_t sub_8009FC80(uint32_t packed)
{
    uint32_t count = r_u32(0x8005B1F8);

    FUNCTION_MARKER(0x8009FC80, "RASHCDG.BIN");
    packed &= 0xFFFFu;
    while (packed)
    {
        uint32_t value = packed & 0x1Fu;
        if (value && value - 1 < count)
            w_u32(0x800D8720, r_u32(0x800D8720) | (1u << (value - 1)));
        packed >>= 5;
    }
    return 0;
}

uint32_t sub_800BCEEC(uint32_t value, uint32_t step)
{
    uint32_t packed = r_u8(value);
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
    w_u8(value, (uint8_t)packed);
    return packed;
}

uint32_t sub_800BD2A0(uint32_t actor, uint32_t rank)
{
    uint32_t state = r_u32(0x8005B2F8);
    uint32_t descriptor;
    uint32_t position;

    FUNCTION_MARKER(0x800BD2A0, "RASHCDG.BIN");
    descriptor = r_u32(actor + 1084);
    if (r_u16(actor + 172) >= r_u32(state + 48) && ((r_u8(descriptor + 1) & 0xF) == 2))
        return 0;
    if (r_u32(0x8005B2A8) & 1)
        return 0;
    position = r_u8(descriptor + 39);
    if (position > r_u32(0x8005B1F8))
        return 0;
    if (position >= 4 && rank < position)
        return 2;
    if (position < 6 && position < rank)
        return 4;
    return 0;
}

uint32_t sub_8009DBA0(uint32_t actor, uint32_t other, int32_t first, int32_t second)
{
    uint32_t linked = r_u32(other + 852);
    uint32_t linked_mode = r_u32(linked + 604);
    uint32_t state = r_u32(0x8005B2F8);
    int32_t delta = first - second;
    uint32_t magnitude = delta < 0 ? 0u - (uint32_t)delta : (uint32_t)delta;
    uint32_t index;
    int32_t position;

    FUNCTION_MARKER(0x8009DBA0, "RASHCDG.BIN");
    if (linked_mode < 3 && ((int32_t)(uint32_t)(r_u32(other + 480))) > 585944)
        return 0;
    index = r_u32(state + 60) + ((0u - (r_u8(state + 4) & 1u)) & 3u);
    if (((int32_t)(uint32_t)(r_u32(0x800530A8 + 4 * index))) < ((int32_t)(uint32_t)(magnitude)))
        return 0;
    if (r_u32(r_u32(actor + 1084) + 40))
        return 0;
    position = ((int32_t)(uint32_t)(r_u32(actor + 480)));
    if (position <= 585944)
        return 1;
    if (linked_mode >= 3 && (uint32_t)(r_u16(linked + 544) - 72) < 2 && ((int32_t)(uint32_t)(r_u32(linked + 480))) >= position)
        return 1;
    return 0;
}

uint32_t sub_80097470(uint32_t actor, uint32_t other)
{
    int32_t slot;

    FUNCTION_MARKER(0x80097470, "RASHCDG.BIN");
    if (!other || other == actor || !r_u16(other + 320))
        return 0;
    slot = r_s8(other + 946);
    (void)r_u16(other + 8 * (slot - 1) + 956);
    if (!sub_8009DB58(actor, other))
        return 0;
    return sub_8009DBA0(actor, other, ((int32_t)(uint32_t)(sub_8009E444(actor, other))), 0) != 0;
}

uint32_t sub_8009E178(uint32_t actor)
{
    uint32_t state = r_u32(0x8005B2F8);
    int32_t slot = r_s8(actor + 946);
    uint32_t selected = r_u16(actor + 8 * slot + 950);
    uint32_t players[2];
    uint32_t active[2] = {0, 0};
    int32_t score[2] = {0, 0};
    uint32_t count;
    uint32_t index;

    FUNCTION_MARKER(0x8009E178, "RASHCDG.BIN");
    if (selected != 224 && !(selected >> 5))
        return selected;
    if (r_u8(state + 4) & 1)
        return r_u8(state + 6);
    if (!(r_u8(state + 4) & 0x10))
    {
        uint32_t primary = r_u32(0x8005B38C);
        if (selected == r_u16(primary + 172))
            return selected;
        if (r_u32(r_u32(primary + 1084) + 40))
            return selected;
        return r_u16(primary + 172);
    }
    if (!r_u16(actor + 320))
        return selected;

    count = r_u32(state + 48);
    if (count > 2)
        count = 2;
    for (index = 0; index < count; ++index)
    {
        players[index] = r_u32(0x8005B268 + 4 * index);
        active[index] = !r_u32(r_u32(players[index] + 1084) + 40);
    }
    if (!active[0])
        return active[1] ? r_u16(players[1] + 172) : selected;
    if (!active[1])
        return r_u16(players[0] + 172);

    for (index = 0; index < count; ++index)
    {
        int32_t dx = (int16_t)r_u16(actor + 186) - (int16_t)r_u16(players[index] + 186);
        int32_t dz = (int16_t)r_u16(actor + 194) - (int16_t)r_u16(players[index] + 194);
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
    return r_u16(players[index] + 172);
}

uint32_t sub_80097388(uint32_t actor)
{
    uint32_t linked = r_u32(actor + 852);
    uint32_t result;
    uint32_t candidate;
    uint32_t step;
    int32_t remaining;

    FUNCTION_MARKER(0x80097388, "RASHCDG.BIN");
    if (r_u32(linked + 604) >= 3)
        return 0;
    result = r_u32(linked + 552) & 0x40;
    if (result)
        return result;
    if (((int32_t)(uint32_t)(r_u32(actor + 480))) > 585944)
        return 1;
    result = r_u8(r_u32(actor + 1084) + 39);
    if (result == 255)
        return result;

    candidate = r_u32(0x800CE4D0);
    step = r_u32(0x800CE4D4);
    remaining = ((int32_t)(uint32_t)(r_u32(r_u32(0x800CE4DC))));
    while (remaining >= 0)
    {
        if (candidate != actor && sub_80097470(actor, candidate))
            (void)sub_80096F30(actor, candidate, 9);
        candidate += step;
        --remaining;
    }
    return step;
}

uint32_t sub_800205C0(int32_t speed, uint32_t mode)
{
    int32_t limit = ((int32_t)(uint32_t)(r_u32(0x80052EE4 + 36 * mode + 24)));
    int32_t result = (int32_t)sub_8001FC90((int32_t)sub_8001FC90(3276, speed), limit);

    FUNCTION_MARKER(0x800205C0, "SLUS_010.53");
    if (result < 0x300000)
        result = 0x300000;
    if (result > limit)
        result = limit;
    return (uint32_t)result;
}

uint32_t sub_8002064C(int32_t speed, uint32_t mode)
{
    int32_t limit = ((int32_t)(uint32_t)(r_u32(0x80052EE4 + 36 * mode + 28)));
    int32_t result = limit - (int32_t)sub_8001FC90((int32_t)sub_8001FC90(1638, speed), limit);

    FUNCTION_MARKER(0x8002064C, "SLUS_010.53");
    if (result < 0x400000)
        result = 0x400000;
    if (result > limit)
        result = limit;
    return (uint32_t)result;
}

uint32_t sub_800206DC(int32_t speed, uint32_t mode)
{
    int32_t limit = ((int32_t)(uint32_t)(r_u32(0x80052EE4 + 36 * mode + 32)));
    int32_t result = limit - (int32_t)sub_8001FC90((int32_t)sub_8001FC90(2293, speed), limit);

    FUNCTION_MARKER(0x800206DC, "SLUS_010.53");
    if (result < 0x190000)
        result = 0x190000;
    if (result > limit)
        result = limit;
    return (uint32_t)result;
}

uint32_t sub_800BEA30(uint32_t point, uint32_t actor)
{
    uint32_t descriptor = r_u32(actor + 1084);
    uint32_t mode = r_u8(descriptor + 1) & 0xF;
    int32_t speed = ((int32_t)(uint32_t)(r_u32(actor + 480)));
    int32_t front = ((int32_t)(uint32_t)(sub_800205C0(speed, mode)));
    int32_t rear = -((int32_t)(uint32_t)(sub_8002064C(speed, mode)));
    int32_t longitudinal;
    int32_t lateral;
    uint32_t road;
    uint32_t magnitude;

    FUNCTION_MARKER(0x800BEA30, "RASHCDG.BIN");
    if (r_u8(actor + 928) & 1)
    {
        longitudinal = ((int32_t)(uint32_t)((r_u32(actor + 324) - r_u32(point + 152)) << 4));
        if (r_u8(descriptor) & 0x80)
            longitudinal = -longitudinal;
    }
    else
        longitudinal = ((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(point + 12, 12), xport_guest_ptr(actor + 528, 6), xport_guest_ptr(actor + 504, 12))));
    if (longitudinal > front || longitudinal < rear)
        return 0;

    road = r_u32(point + 188);
    if ((r_u8(actor + 928) & 1) && road == r_u32(actor + 360) && (!(road >> 16) || r_u32(point + 164) == r_u32(actor + 336)))
        lateral = ((int32_t)(uint32_t)(r_u32(point + 172) - r_u32(actor + 344)));
    else
        lateral = ((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(point + 12, 12), xport_guest_ptr(actor + 432, 6), xport_guest_ptr(actor + 184, 12))));
    magnitude = lateral < 0 ? 0u - (uint32_t)lateral : (uint32_t)lateral;
    return sub_800206DC(speed, mode) >= magnitude;
}

uint32_t sub_8001B3C8(void)
{
    uint32_t state = r_u32(0x8005B2F8);

    FUNCTION_MARKER(0x8001B3C8, "SLUS_010.53");
    if (r_u32(state + 48) >= 2)
        return 0;
    if (r_u8(state + 4) & 1)
        return 1;
    w_u32(0x800D6C40, 7);
    w_u32(0x800D685C, 0x230000);
    (void)sub_80023148(0x800D6858, 1, 1, 0);
    w_u32(0x8005B424, 1);
    w_u32(0x800D6AA8, 4);
    return 4;
}

uint32_t sub_8009E528(uint32_t source, uint32_t actor)
{
    uint32_t linked = r_u32(actor + 852);
    uint32_t descriptor = r_u32(actor + 1084);
    uint32_t identity = r_u16(actor + 172);
    uint32_t record = 0x800CD898 + 1132 * identity;
    uint32_t result;

    FUNCTION_MARKER(0x8009E528, "RASHCDG.BIN");
    w_u32(actor + 720, 0x20000);
    w_u32(actor + 484, 0);
    w_u32(actor + 576, 0);
    w_u32(actor + 480, 0);
    w_u32(actor + 924, 0);
    w_u32(actor + 464, 0);
    w_u32(actor + 460, 0);
    w_u32(actor + 456, 0);
    w_u32(linked + 456, 0);
    w_u32(linked + 460, 0);
    w_u32(linked + 464, 0);
    w_u32(linked + 480, 0);
    w_u32(linked + 484, 0);
    w_u32(linked + 488, 0);
    w_u32(linked + 552, r_u32(linked + 552) & 0xFFEFF067u);
    w_u8(descriptor + 39, 0xFE);
    (void)sub_8001B3C8();
    if (r_u32(linked + 604) >= 3)
    {
        w_u32(record + 184, r_u32(linked + 184));
        w_u32(record + 188, r_u32(linked + 188));
        w_u32(record + 192, r_u32(linked + 192));
    }
    else
    {
        w_u32(record + 184, r_u32(actor + 504));
        w_u32(record + 188, r_u32(actor + 508));
        w_u32(record + 192, r_u32(actor + 512));
    }
    w_u32(record + 548, r_u32(record + 548) | 6);
    if (!(r_u32(linked + 552) & 0x40))
        return sub_80092C7C(actor, 9);
    result = r_u32(r_u32(0x8005B2F8) + 16);
    w_u32(descriptor + 40, result);
    return result;
}

uint32_t sub_800BCF6C(uint32_t actor, uint32_t other)
{
    int32_t actor_slot = r_s8(actor + 946);
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
    other_slot = r_s8(other + 946);
    if (other_slot > 0)
        other_event = other + 8 * other_slot + 948;
    actor_id = r_u16(actor + 172);
    other_id = r_u16(other + 172);

    if (sub_800A8C48(other_id, r_u16(r_u32(actor + 1084) + 64)))
        flags = 0x40;
    if (other_event && r_u16(other_event) == 16 && r_u16(other_event + 2) == actor_id && (r_u16(other_event + 6) & 0x4000))
    {
        flags |= 1;
        state = r_u32(0x8005B2F8);
        if (other_id < r_u32(state + 48) && !(r_u32(other + 560) & 0x08000000) && !sub_800A8C48(actor_id, r_u16(r_u32(other + 1084) + 64)))
            flags &= ~1u;
        if ((flags & 1) && (r_u16(actor_event) != 16 || r_u16(actor_event + 2) != other_id))
            flags |= 2;
    }
    if (r_u8(r_u32(actor + 1084) + 70) == other_id)
        flags |= 0x1000;
    if (sub_800A8C48(other_id, r_u32(actor + 912)))
        flags |= 0x80;
    if (other_event)
    {
        uint32_t type = r_u16(other_event);
        if ((type == 10 || type == 13 || type == 14 || type == 15) && r_u16(other_event + 2) == actor_id && (r_u16(other_event + 6) & 0x4000))
        {
            flags |= 4;
            type = r_u16(actor_event);
            if (type - 10 >= 7 || r_u16(actor_event + 2) != other_id)
                flags |= 8;
        }
    }
    if (sub_800A8C48(other_id, r_u16(r_u32(actor + 1084) + 66)))
        flags |= 0x100;
    state = r_u16(actor_event);
    if (state == 16)
    {
        if (r_u16(actor_event + 2) == other_id && (r_u16(actor_event + 6) & 0x4000))
            flags |= 0x10;
        state = r_u16(actor_event);
    }
    if ((state == 10 || state == 13 || state == 14 || state == 15) && r_u16(actor_event + 2) == other_id && (r_u16(actor_event + 6) & 0x4000))
        flags |= 0x20;
    if (r_u8(r_u32(actor + 1084) + 71) == other_id)
        flags |= 0x800;
    if (other_event && !r_u16(other_event) && (r_u16(other_event + 6) & 0x4000))
        flags |= 0x400;
    return flags;
}

uint32_t sub_800B8FB0(uint32_t actor, int32_t direction, uint32_t other)
{
    uint32_t descriptor = r_u32(actor + 1084);
    uint32_t config = 0x80052EE4 + 36 * (r_u8(descriptor + 1) & 0xF);
    int32_t delta = ((int32_t)(uint32_t)((r_u32(actor + 324) - r_u32(other + 324)) << 4));
    uint32_t group;
    int32_t slot = r_s8(actor + 946);
    uint32_t event = actor + 8 * slot + 948;
    uint16_t type;
    uint8_t packet[8] = {0};

    FUNCTION_MARKER(0x800B8FB0, "RASHCDG.BIN");
    direction = (int8_t)direction;
    group = direction >= r_s8(config + 4);
    group += direction >= r_s8(config + 5);
    if ((r_u8(descriptor + 2) & 0xF) >= r_u8(config + 17))
        group |= 4;

    switch (group)
    {
        case 0:
        case 4:
        {
            uint32_t magnitude = delta < 0 ? 0u - (uint32_t)delta : (uint32_t)delta;
            if (sub_800BC1EC(actor, other, delta) && magnitude < r_u32(0x80052F70))
                type = 9;
            else
            {
                if (!sub_800BBD44(actor, other, delta))
                    return 0;
                type = 8;
            }
            break;
        }
        case 1:
        case 2:
        case 5:
            if (!sub_800BC1EC(actor, other, delta))
                return 0;
            type = 16;
            (void)sub_800B92C0(descriptor);
            break;
        case 6:
        {
            uint32_t index;
            uint8_t maximum;
            if (!sub_800BC1EC(actor, other, delta))
                return 0;
            if (r_u16(other + 172) < r_u32(r_u32(0x8005B2F8) + 48))
                (void)sub_8001A760(r_u16(actor + 172), 0);
            type = 16;
            if (r_u16(descriptor + 44) & 0x1FF)
            {
                maximum = 0x8F;
                w_u8(descriptor + 60, maximum);
                for (index = 0; index < 8; ++index)
                {
                    uint8_t value = r_u8(descriptor + 52 + index);
                    if (maximum < value)
                    {
                        maximum = value;
                        w_u8(descriptor + 60, value);
                    }
                }
                (void)sub_800B9340(descriptor);
            }
            else
            {
                w_u8(descriptor + 46, 9);
                w_u8(descriptor + 47, 0);
                w_u8(descriptor + 60, 38);
            }
            break;
        }
        default:
            return 0;
    }

    if (r_u16(event + 2) == r_u16(other + 172))
    {
        uint16_t current = r_u16(event);
        if (current == 6)
            return 0;
        if (current == 16 && type != 16)
        {
            uint32_t linked = r_u32(actor + 852);
            uint32_t state = r_u16(linked + 544);
            if (r_u16(0x800541D4 + 8 * state + 2) == 3)
            {
                uint32_t variant = r_u8(linked + 569);
                uint32_t replacement = r_u16(r_u32(0x8005AD4C) + 12 * variant);
                (void)sub_800C4550(replacement, linked, 2);
            }
        }
        w_u16(event, type);
        return 1;
    }
    xport_store_le16(packet, type);
    xport_store_le16(packet + 2, r_u16(other + 172));
    xport_store_le16(packet + 4, r_u16(0x801FFEDC));
    return sub_800BCA68(packet, 2, actor);
}

uint32_t sub_8009DC90(uint32_t actor)
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
    if (r_s8(actor + 946) <= 0)
    {
        xport_store_le16(packet, 4);
        xport_store_le16(packet + 2, 224);
        (void)sub_800BCA68(packet, 1, actor);
    }
    linked = r_u32(actor + 852);
    if (r_u32(linked + 604) >= 3 || (r_u32(linked + 552) & 0x40))
        return 0;
    state = r_u32(0x8005B2F8);
    if (r_u8(state + 4) == 44 && r_u8(state + 57) >= 4)
        return 0;
    for (index = 0; index < r_u32(state + 48); ++index)
    {
        uint32_t player = r_u32(0x8005B268 + 4 * index);
        if (r_u8(r_u32(player + 1084) + 39) == 0xFE)
            w_u32(player + 720, 0x20000);
    }
    descriptor = r_u32(actor + 1084);
    (void)sub_8009FC80(r_u16(descriptor + 64));
    selected = sub_8009E178(actor) & 0xFFFFu;
    if (selected == 224 || (selected >> 5))
        return 0;
    other = r_u32(0x8005B3A0) + 1096 * selected;
    if (!other || other == actor)
        return 0;
    (void)r_u16(other + 8 * (r_s8(other + 946) - 1) + 956);
    longitudinal = ((int32_t)(uint32_t)(sub_8009E444(actor, other)));
    if (!sub_8009DB58(actor, other))
        return 0;
    {
        int32_t speed = ((int32_t)(uint32_t)(r_u32(actor + 480)));
        int32_t numerator = (int32_t)sub_8001FC90(speed, speed);
        int32_t denominator = ((int32_t)(uint32_t)(r_u32(actor + 600) * 2));
        uint32_t quotient;
        if (!denominator)
            ratio = 0;
        else
        {
            uint32_t n = numerator < 0 ? 0u - (uint32_t)numerator : (uint32_t)numerator;
            uint32_t d = denominator < 0 ? 0u - (uint32_t)denominator : (uint32_t)denominator;
            quotient = sub_80010028(n, d);
            ratio = (numerator < 0) != (denominator < 0) ? -((int32_t)(uint32_t)(quotient)) : ((int32_t)(uint32_t)(quotient));
        }
    }
    if (sub_8009DBA0(actor, other, longitudinal, ratio))
    {
        (void)sub_800BCD10(actor);
        memset(packet, 0, sizeof(packet));
        xport_store_le16(packet, 2);
        xport_store_le16(packet + 2, 224);
        (void)sub_800BCA68(packet, 1, actor);
        w_u32(descriptor + 40, r_u32(state + 16));
        return 1;
    }
    {
        uint32_t other_linked = r_u32(other + 852);
        uint32_t other_mode = r_u32(other_linked + 604);
        int32_t ordering = ((int32_t)(uint32_t)(sub_8009E3F0(actor, other)));
        uint32_t other_ready = other_mode >= 3 || ((int32_t)(uint32_t)(r_u32(other + 480))) <= 585944;
        uint32_t same_road = r_u32(other + 360) == r_u32(actor + 360);
        uint32_t table_index = r_u32(state + 60) + ((0u - (r_u8(state + 4) & 1u)) & 3u);
        uint32_t within = ((int32_t)(uint32_t)(r_u32(0x800530A8 + 4 * table_index))) >= longitudinal;
        int32_t slot = r_s8(actor + 946);
        uint32_t previous = r_u16(actor + 8 * (slot - 1) + 956);
        uint16_t event_type = 0;

        if (ordering)
            longitudinal = -longitudinal;
        if (previous == 2)
        {
            if (r_u32(linked + 604) < 3 && within && ((int32_t)(uint32_t)(r_u32(actor + 480))) <= 585944 && r_u8(r_u32(other + 1084) + 39) != 0xFE && other_ready)
            {
                (void)sub_8009E528(actor, other);
                return 0;
            }
            if (within)
                return 0;
            (void)sub_800BCD10(actor);
            w_u32(descriptor + 40, 0);
            selected = 224;
            event_type = 4;
        }
        else if (previous == 4)
        {
            if (other_ready || !same_road)
            {
                if (!sub_800BC120(actor, other, longitudinal))
                    event_type = 17;
            }
            else if (sub_800BB8FC(actor, other, longitudinal))
                event_type = 6;
            else if (sub_800BC1EC(actor, other, longitudinal))
            {
                event_type = 16;
                (void)sub_800B92C0(descriptor);
            }
        }
        if (event_type)
        {
            memset(packet, 0, sizeof(packet));
            xport_store_le16(packet, event_type);
            xport_store_le16(packet + 2, (uint16_t)selected);
            (void)sub_800BCA68(packet, 1, actor);
        }
    }
    return 0;
}

uint32_t sub_8009B474(int32_t mode, uint32_t delta)
{
    FUNCTION_MARKER(0x8009B474, "RASHCDG.BIN");
    if (mode == 3)
        return (uint32_t)sub_8009CFF4(delta);
    if (mode < 4 && !mode)
        return (uint32_t)sub_8009E89C(delta);
    return mode < 4;
}

uint32_t sub_8009E89C(uint32_t delta)
{
    FUNCTION_MARKER(0x8009E89C, "RASHCDG.BIN");
    return sub_8009E8A4(delta, r_u32(0x8005ACC0));
}

uint32_t sub_800A01CC(uint32_t active[2])
{
    uint32_t state = r_u32(0x8005B2F8);
    uint32_t index;

    FUNCTION_MARKER(0x800A01CC, "RASHCDG.BIN");
    if (!(r_u8(state + 4) & 0x10) || !active[0] || !active[1])
        return 0;
    {
        uint32_t first = r_u32(0x8005B38C);
        uint32_t second = r_u32(0x8005B21C);
        int32_t dx = (int16_t)r_u16(first + 186) - (int16_t)r_u16(second + 186);
        int32_t dz = (int16_t)r_u16(first + 194) - (int16_t)r_u16(second + 194);
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
        threshold = (((int32_t)(uint32_t)(r_u32(0x80053114 + 4 * r_u32(state + 60)))) >> 1) + 100;
        if (distance >= threshold)
            return 0;
    }
    {
        uint32_t side[2] = {0, 0};
        for (index = 0; index < r_u32(state + 48) && index < 2; ++index)
        {
            uint32_t player = r_u32(0x8005B268 + 4 * index);
            uint32_t record = 0x800CD898 + 1132 * (index ^ 1);
            int32_t x = ((int32_t)(uint32_t)(r_u32(player + 184) - r_u32(record + 184)));
            int32_t y = ((int32_t)(uint32_t)(r_u32(player + 188) - r_u32(record + 188)));
            int32_t z = ((int32_t)(uint32_t)(r_u32(player + 192) - r_u32(record + 192)));
            int64_t dot = (int64_t)x * ((int16_t)r_u16(record + 444) * 16) + (int64_t)y * ((int16_t)r_u16(record + 446) * 16) + (int64_t)z * ((int16_t)r_u16(record + 448) * 16);
            side[index] = (int32_t)(dot >> 16) >= 0;
        }
        if (side[0] == side[1])
        {
            if (((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(0x800CDA54, 6), xport_guest_ptr(0x800CDEC0, 6)))) < 0)
                for (index = 0; index < r_u32(state + 48) && index < 2; ++index)
                    active[index] = 0;
        }
        else if (side[0])
            active[0] = 0;
        else
            active[1] = 0;
    }
    return 0;
}

uint32_t sub_8003C520(uint32_t object)
{
    uint32_t entry = r_u32(object + 4);
    uint32_t route;

    FUNCTION_MARKER(0x8003C520, "SLUS_010.53");
    if ((int16_t)r_u16(entry + 2) == 1)
        return r_u32(r_u32(object + 8) + 12);
    route = sub_800245DC(r_u32(entry + 12));
    if (route)
        return (r_u32(route + 4) >> 6) << 16;
    return 0;
}

uint32_t sub_80094184(uint32_t actor)
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
    descriptor = r_u32(actor + 1084);
    if ((r_u8(descriptor + 1) & 0xF) != 2 || !r_u8(0x800D8703))
        return 0;
    w_u8(descriptor + 12, r_u8(0x800D86FC));
    w_u8(descriptor + 13, r_u8(0x800D86FD));
    w_u8(descriptor + 15, r_u8(0x800D86FF));
    w_u8(descriptor + 14, r_u8(0x800D86FE));
    w_u8(descriptor + 2, r_u8(0x800D8700));
    w_u8(descriptor + 61, r_u8(0x800D8701));
    w_u8(descriptor + 36, r_u8(0x800D8702));
    w_u8(descriptor + 37, r_u8(0x800D8702));
    w_u16(descriptor + 44, r_u16(0x800D8704));
    w_u8(descriptor + 46, r_u8(0x800D8706));
    w_u8(descriptor + 47, 0);
    w_u32(descriptor + 48, 0);
    w_u32(r_u32(actor + 556) + 224, r_u32(0x800D86F8));
    state = r_u32(0x8005B2F8);
    mode = ((int32_t)(uint32_t)(r_u32(state + 60)));
    base = (mode > 0 ? mode : 0) + (2 - mode < 0 ? 2 - mode : 0);
    base = 8 * base + 66;
    previous = r_u8(descriptor + 38);
    for (attempts = 0; attempts < 8; ++attempts)
    {
        uint8_t value = (uint8_t)(base + (sub_8001FC58() & 7));
        w_u8(descriptor + 38, value);
        if (value != previous)
            break;
    }
    return 0;
}

uint32_t sub_80012C1C(const uint32_t source[3], uint32_t destination[3], int32_t offset)
{
    uint32_t sign = (uint32_t)(offset >> 31);
    uint32_t distance = ((uint32_t)offset + sign) ^ sign;
    uint32_t route_id;

    FUNCTION_MARKER(0x80012C1C, "SLUS_010.53");
    (void)sub_8001E0B4(destination, source, 12);
    if (offset < 0)
        destination[1] = 0 - destination[1];
    route_id = (destination[0] & 0xFFFF);

    while (((int32_t)(uint32_t)(distance)) > 0)
    {
        uint32_t route_word = destination[0];

        if (!(route_word >> 16))
        {
            uint32_t route = sub_800245DC(route_word & 0xFFFF);
            uint32_t length = (r_u32(route + 4) >> 6) << 16;
            int32_t position = ((int32_t)(uint32_t)(destination[2]));

            if (((int32_t)(uint32_t)(destination[1])) <= 0)
                position = ((int32_t)(uint32_t)((uint32_t)position - distance));
            else
                position = ((int32_t)(uint32_t)((uint32_t)position + distance));
            destination[2] = (uint32_t)position;
            if (position < 0)
            {
                route_id = r_u32(route + 8);
                distance = 0 - (uint32_t)position;
            }
            else if (position > (int32_t)length)
            {
                route_id = r_u32(route + 12);
                distance = (uint32_t)position - length;
            }
            else
            {
                distance = 0;
                continue;
            }
            destination[0] = (route_id & 0xFFFF) | 0x10000;
            destination[2] = 0;
        }
        else
        {
            uint32_t junction = sub_800245F4(route_id);
            uint32_t count;
            uint32_t selected = 0;
            uint32_t found = 0;

            if (!junction || (count = r_u32(junction + 4)) < 2)
            {
                distance = 0;
                continue;
            }
            if ((int16_t)r_u16(0x800D6182) != -1)
            {
                uint32_t i;

                if (offset <= 0)
                {
                    for (i = 0; i < count; ++i)
                    {
                        uint32_t entry = junction + 8 + 8 * i;
                        uint32_t link = sub_8003B4B0(0, r_u32(entry));

                        if (link && ((((int32_t)(uint32_t)(r_u32(link + 4))) < 0 && ((int32_t)(uint32_t)(r_u32(entry + 4))) > 0) || (((int32_t)(uint32_t)(r_u32(link + 4))) > 0 && ((int32_t)(uint32_t)(r_u32(entry + 4))) < 0)))
                        {
                            selected = entry;
                            found = 1;
                            break;
                        }
                    }
                }
                else
                {
                    uint32_t connected = sub_8003F408(0, route_id);

                    if (connected)
                    {
                        uint32_t connected_count = r_u32(connected + 16);
                        uint32_t choice = connected_count < 2 ? r_u32(connected + 84) : r_u32(connected + 84 + 4 * (sub_8001FC58() % connected_count));

                        for (i = 0; i < count; ++i)
                        {
                            uint32_t entry = junction + 8 + 8 * i;

                            if (r_u32(entry) == choice)
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
                selected = junction + 8 + 8 * (sub_8001FC58() % count);
            destination[0] = r_u16(selected);
            destination[1] = r_u32(selected + 4);
            if (((int32_t)(uint32_t)(r_u32(selected + 4))) <= 0)
            {
                uint32_t route = sub_800245DC((destination[0] & 0xFFFF));

                destination[2] = (r_u32(route + 4) >> 6) << 16;
            }
            else
                destination[2] = 0;
        }
    }

    destination[1] = 1;
    if (destination[0] == source[0])
        destination[1] = source[1];
    if ((int16_t)r_u16(0x800D6182) == -1)
        return 0xFFFFFFFF;
    if (destination[0] >> 16)
        return destination[0] >> 16;
    {
        uint32_t link = sub_8003B4B0(0, (destination[0] & 0xFFFF));

        if (link)
        {
            uint32_t direction = r_u32(link + 4);

            destination[1] = direction;
            return direction;
        }
    }
    return 0;
}
