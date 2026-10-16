#include "race_global_8009E8A4.h"
#include "race_pause.h"
#include "race_pause_frontier.h"
#include "spu.h"
#include "xport.h"

static int32_t traffic_mode(uint32_t state)
{
    int32_t mode = ((int32_t)(uint32_t)(r_u32(state + 60)));

    if (((int32_t)(uint32_t)(r_u32(0x800D86F0u))) > 0)
        mode += 3;
    return mode;
}

static void traffic_place_forced(uint32_t actor, uint32_t spawned, uint32_t state)
{
    int32_t direction = ((int32_t)(uint32_t)(r_u32(actor + 364))) > 0 ? 1 : -1;
    uint32_t spacing = r_u32(0x80053114u + 4u * r_u32(state + 60));
    uint32_t position = r_u32(actor + 368);
    uint32_t route = actor + 328;
    uint32_t limit;
    uint32_t difference;
    uint32_t nonnegative;

    w_u32(spawned + 360, r_u32(actor + 360));
    w_u32(spawned + 364, (uint32_t)direction);
    if (direction > 0)
        position += spacing;
    else
        position -= spacing;
    w_u32(spawned + 368, position);
    limit = sub_8003C520(route);
    nonnegative = (uint32_t)(~(((int32_t)(uint32_t)(position)) >> 31)) & position;
    difference = sub_8003C520(route) - position;
    position = nonnegative + ((uint32_t)(((int32_t)(uint32_t)(limit - position)) >> 31) & difference);
    w_u32(spawned + 368, position);
}

static void traffic_activate(uint32_t spawned, uint32_t state)
{
    uint8_t event[8] = {0};
    uint32_t speed = r_u32(0x80053138u + 4u * r_u32(state + 60));
    uint32_t descriptor;

    (void)sub_8002090C(spawned);
    w_u32(spawned + 924, speed);
    w_u32(spawned + 480, speed);
    (void)sub_80094184(spawned);
    w_u16(spawned + 320, 1);
    w_u32(r_u32(spawned + 852) + 604, 1);
    sub_80093F94(spawned, 0);
    (void)sub_800BCD10(spawned);
    xport_store_le16(event, 4);
    xport_store_le16(event + 2, 224);
    (void)sub_800BCA68(event, 1, spawned);
    descriptor = r_u32(spawned + 1084);
    w_u16(descriptor + 62, (uint16_t)(((int32_t)(uint32_t)(r_u32(state + 16))) >> 8));
}

uint32_t sub_8009E8A4(uint32_t delta, uint32_t enabled)
{
    uint32_t active[2] = {0, 0};
    uint32_t state;
    int32_t player_count;
    int32_t force = 0;
    int32_t index;

    FUNCTION_MARKER(0x8009E8A4, "RASHCDG.BIN");
    if (!enabled)
        return 0;
    state = r_u32(0x8005B2F8u);
    player_count = ((int32_t)(uint32_t)(r_u32(state + 48)));
    for (index = 0; index < player_count && index < 2; ++index)
    {
        uint32_t actor = r_u32(0x8005B268u + 4u * (uint32_t)index);
        uint32_t descriptor = r_u32(actor + 1084);

        if (!r_u32(descriptor + 40) && !((uint8_t)r_u8((state + 4)) & 1u) && !((uint8_t)r_u8(descriptor) & 0x40u))
        {
            uint32_t linked = r_u32(actor + 852);

            if ((int16_t)r_u16(actor + 320) || (r_u32(linked + 604) >= 3 && (int16_t)r_u16(linked + 320)))
                active[index] = ((int32_t)(uint32_t)(r_u32(0x800D86F4u))) > 0;
        }
    }
    /* Preserve the callee RA spill in the aliased passing specification */
    w_u32(0x1F8002DCu, 0x8009E9D8u);
    (void)sub_800A01CC(active);

    for (index = 0; index < player_count && index < 2; ++index)
    {
        uint32_t cooldown = 0x8005B368u + 4u * (uint32_t)index;
        uint32_t actor;
        uint32_t linked;
        uint32_t descriptor;
        uint32_t timer;
        uint32_t state_mode;
        uint32_t high_bit;
        uint32_t final_position;
        uint32_t route_limited;
        uint32_t urgent = 0;
        uint32_t should_consider;
        uint32_t threshold;

        if (!active[index])
        {
            w_u32(cooldown, 0xFFFF0000u);
            continue;
        }
        actor = r_u32(0x8005B268u + 4u * (uint32_t)index);
        linked = r_u32(actor + 852);
        descriptor = r_u32(actor + 1084);
        route_limited = sub_80095410(actor);
        high_bit = (uint8_t)r_u8(descriptor) >> 7;
        final_position = (uint8_t)r_u8((descriptor + 39)) == 1;
        if ((int32_t)r_u8((descriptor + 39)) >= ((int32_t)(uint32_t)(r_u32(0x8005B1F8u) - r_u32(0x800D86F4u))))
            urgent = ((int32_t)(uint32_t)(r_u32(actor + 480))) <= 1464849;
        if ((uint8_t)r_u8((state + 4)) == 44 && (uint8_t)r_u8((state + 57)) == 3)
            force = 1;

        timer = 0x8005B2A0u + 4u * (uint32_t)index;
        w_u32(timer, r_u32(timer) + delta);
        state_mode = r_u32(state + 60);
        if (((int32_t)(uint32_t)(r_u32(timer))) >= ((int32_t)(uint32_t)(r_u32(0x800530FCu + 4u * (uint32_t)traffic_mode(state)))))
        {
            uint32_t chance = sub_8001FC58() % 100u;
            uint32_t limit = r_u32(0x80053120u + 4u * (uint32_t)traffic_mode(state));

            if (chance < limit)
            {
                uint32_t nearest_id;
                int32_t limit_distance = r_s32(0x80053114 + 4 * state_mode);
                int32_t distance = (int32_t)sub_8003A5F4(actor + 360, &nearest_id, 1);
                if (limit_distance < distance)
                    force = 1;
            }
            w_u32(timer, 0);
        }

        should_consider = r_u32(linked + 604) != 1 || ((int32_t)(uint32_t)(r_u32(0x80053138u + 4u * state_mode))) >= ((int32_t)(uint32_t)(r_u32(actor + 480))) || high_bit || route_limited || final_position || urgent || force;
        if (!should_consider)
        {
            w_u32(cooldown, 0xFFFF0000u);
            continue;
        }
        if (((int32_t)(uint32_t)(r_u32(cooldown))) >= 0)
            w_u32(cooldown, r_u32(cooldown) + delta);
        else
            w_u32(cooldown, 0);
        threshold = r_u32(0x800530FCu + 4u * (uint32_t)traffic_mode(state));
        if (!force && ((int32_t)(uint32_t)(r_u32(cooldown))) <= ((int32_t)(uint32_t)(threshold)) && (!high_bit || ((int32_t)(uint32_t)(r_u32(cooldown))) <= (((int32_t)(uint32_t)(threshold)) >> 1)) && (!route_limited || ((int32_t)(uint32_t)(r_u32(cooldown))) <= (((int32_t)(uint32_t)(threshold)) >> 1)))
            continue;
        {
            uint32_t spawned = sub_80095848();

            if (spawned)
            {
                uint32_t source = actor + 360;
                int32_t offset = -(((int32_t)(uint32_t)(r_u32(0x80053114u + 4u * state_mode))) >> 1);

                {
                    uint32_t linked_route = r_u32(linked + 604) >= 3;
                    if (linked_route)
                    {
                        if ((int32_t)(r_u32(actor + 364) ^ r_u32(linked + 364)) < 0)
                            offset = r_s32(0x80053114 + 4 * state_mode) >> 1;
                        source = linked + 360;
                    }
                    if (!linked_route && force)
                    {
                        int32_t direction = ((int32_t)(uint32_t)(r_u32(actor + 364))) > 0 ? 1 : -1;
                        uint32_t spacing = r_u32(0x80053114u + 4u * r_u32(state + 60));
                        uint32_t position = r_u32(actor + 368);
                        uint32_t route = actor + 328;
                        uint32_t limit;
                        uint32_t difference;
                        uint32_t nonnegative;
                        
                        w_u32(spawned + 360, r_u32(actor + 360));
                        w_u32(spawned + 364, (uint32_t)direction);
                        if (direction > 0)
                            position += spacing;
                        else
                            position -= spacing;
                        w_u32(spawned + 368, position);
                        w_u32(0x1F8002DCu, 0x8009EE14u);
                        limit = sub_8003C520(route);
                        nonnegative = (uint32_t)(~(((int32_t)(uint32_t)(position)) >> 31)) & position;
                        w_u32(0x1F8002DCu, 0x8009EE20u);
                        difference = sub_8003C520(route) - position;
                        position = nonnegative + ((uint32_t)(((int32_t)(uint32_t)(limit - position)) >> 31) & difference);
                        w_u32(spawned + 368, position);
                    }
                    else
                    {
                        uint32_t source_record[3];
                        uint32_t destination_record[3];
                        uint32_t axis;
                        for (axis = 0; axis < 3; ++axis)
                        {
                            source_record[axis] = r_u32(source + 4 * axis);
                            destination_record[axis] = source_record[axis];
                        }
                        w_u32(0x1F8002DCu, 0x8009EE78u);
                        (void)sub_80012C1C(source == spawned + 360 ? destination_record : source_record, destination_record, offset);
                        for (axis = 0; axis < 3; ++axis)
                            w_u32(spawned + 360 + 4 * axis, destination_record[axis]);
                    }
                }
                if (!r_u16(spawned + 362))
                {
                    uint8_t event[8] = {0};
                    uint32_t speed = r_u32(0x80053138u + 4u * r_u32(state + 60));
                    uint32_t spawned_descriptor;
                    
                    (void)sub_8002090C(spawned);
                    w_u32(spawned + 924, speed);
                    w_u32(spawned + 480, speed);
                    (void)sub_80094184(spawned);
                    w_u16(spawned + 320, 1);
                    w_u32(r_u32(spawned + 852) + 604, 1);
                    sub_80093F94(spawned, 0);
                    w_u32(0x1F8002DCu, 0x8009EEFCu);
                    (void)sub_800BCD10(spawned);
                    xport_store_le16(event, 4);
                    xport_store_le16(event + 2, 224);
                    (void)sub_800BCA68(event, 1, spawned);
                    spawned_descriptor = r_u32(spawned + 1084);
                    w_u16(spawned_descriptor + 62, (uint16_t)(((int32_t)(uint32_t)(r_u32(state + 16))) >> 8));
                }
            }
        }
        w_u32(cooldown, 0xFFFF0000u);
    }
    return 0;
}
