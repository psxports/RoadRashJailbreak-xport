#include "race_global_8009E8A4.h"
#include "race_pause.h"
#include "race_pause_frontier.h"
#include "spu.h"
#include "xport.h"

static uint8_t traffic_byte(RRJMemory *m, uint32_t address)
{
    return r_u8(address);
}

static int32_t traffic_mode(RRJMemory *m, uint32_t state)
{
    int32_t mode = rrj_s32(rrj_read32(m, state + 60));

    if (rrj_s32(rrj_read32(m, 0x800D86F0u)) > 0)
        mode += 3;
    return mode;
}

static uint32_t traffic_threshold(RRJMemory *m, uint32_t state)
{
    return rrj_read32(m, 0x800530FCu + 4u * (uint32_t)traffic_mode(m, state));
}

static void traffic_place_forced(RRJMemory *m, uint32_t actor, uint32_t spawned, uint32_t state)
{
    int32_t direction = rrj_s32(rrj_read32(m, actor + 364)) > 0 ? 1 : -1;
    uint32_t spacing = rrj_read32(m, 0x80053114u + 4u * rrj_read32(m, state + 60));
    uint32_t position = rrj_read32(m, actor + 368);
    uint32_t route = actor + 328;
    uint32_t limit;
    uint32_t difference;
    uint32_t nonnegative;

    rrj_write32(m, spawned + 360, rrj_read32(m, actor + 360));
    rrj_write32(m, spawned + 364, (uint32_t)direction);
    if (direction > 0)
        position += spacing;
    else
        position -= spacing;
    rrj_write32(m, spawned + 368, position);
    limit = sub_8003C520(m, route);
    nonnegative = (uint32_t)(~(rrj_s32(position) >> 31)) & position;
    difference = sub_8003C520(m, route) - position;
    position = nonnegative + ((uint32_t)(rrj_s32(limit - position) >> 31) & difference);
    rrj_write32(m, spawned + 368, position);
}

static void traffic_activate(RRJMemory *m, uint32_t spawned, uint32_t state)
{
    uint8_t event[8] = {0};
    uint32_t speed = rrj_read32(m, 0x80053138u + 4u * rrj_read32(m, state + 60));
    uint32_t descriptor;

    (void)sub_8002090C(m, spawned);
    rrj_write32(m, spawned + 924, speed);
    rrj_write32(m, spawned + 480, speed);
    (void)sub_80094184(m, spawned);
    rrj_put16(rrj_at(m, spawned + 320, 2), 1);
    rrj_write32(m, rrj_read32(m, spawned + 852) + 604, 1);
    sub_80093F94(m, spawned, 0, rrj_spu_reverb);
    (void)sub_800BCD10(m, spawned);
    rrj_put16(event, 4);
    rrj_put16(event + 2, 224);
    (void)rrj_enqueue_actor_event_local(m, event, 1, spawned);
    descriptor = rrj_read32(m, spawned + 1084);
    rrj_put16(rrj_at(m, descriptor + 62, 2), (uint16_t)(rrj_s32(rrj_read32(m, state + 16)) >> 8));
}

uint32_t sub_8009E8A4(RRJMemory *m, uint32_t delta, uint32_t enabled)
{
    uint32_t active[2] = {0, 0};
    uint32_t state;
    int32_t player_count;
    int32_t force = 0;
    int32_t index;

    FUNCTION_MARKER(0x8009E8A4, "RASHCDG.BIN");
    if (!enabled)
        return 0;
    state = rrj_read32(m, 0x8005B2F8u);
    player_count = rrj_s32(rrj_read32(m, state + 48));
    for (index = 0; index < player_count && index < 2; ++index)
    {
        uint32_t actor = rrj_read32(m, 0x8005B268u + 4u * (uint32_t)index);
        uint32_t descriptor = rrj_read32(m, actor + 1084);

        if (!rrj_read32(m, descriptor + 40) && !(traffic_byte(m, state + 4) & 1u) && !(traffic_byte(m, descriptor) & 0x40u))
        {
            uint32_t linked = rrj_read32(m, actor + 852);

            if ((int16_t)rrj_u16(rrj_at(m, actor + 320, 2)) || (rrj_read32(m, linked + 604) >= 3 && (int16_t)rrj_u16(rrj_at(m, linked + 320, 2))))
                active[index] = rrj_s32(rrj_read32(m, 0x800D86F4u)) > 0;
        }
    }
    (void)sub_800A01CC(m, active);

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
            rrj_write32(m, cooldown, 0xFFFF0000u);
            continue;
        }
        actor = rrj_read32(m, 0x8005B268u + 4u * (uint32_t)index);
        linked = rrj_read32(m, actor + 852);
        descriptor = rrj_read32(m, actor + 1084);
        route_limited = sub_80095410(m, actor);
        high_bit = traffic_byte(m, descriptor) >> 7;
        final_position = traffic_byte(m, descriptor + 39) == 1;
        if ((int32_t)traffic_byte(m, descriptor + 39) >= rrj_s32(rrj_read32(m, 0x8005B1F8u) - rrj_read32(m, 0x800D86F4u)))
            urgent = rrj_s32(rrj_read32(m, actor + 480)) <= 1464849;
        if (traffic_byte(m, state + 4) == 44 && traffic_byte(m, state + 57) == 3)
            force = 1;

        timer = 0x8005B2A0u + 4u * (uint32_t)index;
        rrj_write32(m, timer, rrj_read32(m, timer) + delta);
        state_mode = rrj_read32(m, state + 60);
        if (rrj_s32(rrj_read32(m, timer)) >= rrj_s32(traffic_threshold(m, state)))
        {
            uint32_t chance = sub_8001FC58(m) % 100u;
            uint32_t limit = rrj_read32(m, 0x80053120u + 4u * (uint32_t)traffic_mode(m, state));

            if (chance < limit && rrj_s32(rrj_read32(m, 0x80053114u + 4u * state_mode)) < rrj_s32(sub_8003A5F4(m, actor + 360, 0x1F8003F0u, 1)))
                force = 1;
            rrj_write32(m, timer, 0);
        }

        should_consider = rrj_read32(m, linked + 604) != 1 || rrj_s32(rrj_read32(m, 0x80053138u + 4u * state_mode)) >= rrj_s32(rrj_read32(m, actor + 480)) || high_bit || route_limited || final_position || urgent || force;
        if (!should_consider)
        {
            rrj_write32(m, cooldown, 0xFFFF0000u);
            continue;
        }
        if (rrj_s32(rrj_read32(m, cooldown)) >= 0)
            rrj_write32(m, cooldown, rrj_read32(m, cooldown) + delta);
        else
            rrj_write32(m, cooldown, 0);
        threshold = traffic_threshold(m, state);
        if (!force && rrj_s32(rrj_read32(m, cooldown)) <= rrj_s32(threshold) && (!high_bit || rrj_s32(rrj_read32(m, cooldown)) <= (rrj_s32(threshold) >> 1)) && (!route_limited || rrj_s32(rrj_read32(m, cooldown)) <= (rrj_s32(threshold) >> 1)))
            continue;
        {
            uint32_t spawned = sub_80095848(m);

            if (spawned)
            {
                uint32_t source = actor + 360;
                int32_t offset = -(rrj_s32(rrj_read32(m, 0x80053114u + 4u * state_mode)) >> 1);

                if (rrj_read32(m, linked + 604) >= 3)
                {
                    if (rrj_s32(rrj_read32(m, actor + 364) ^ rrj_read32(m, linked + 364)) < 0)
                        offset = rrj_s32(rrj_read32(m, 0x80053114u + 4u * state_mode)) >> 1;
                    source = linked + 360;
                    (void)sub_80012C1C(m, source, spawned + 360, offset);
                }
                else if (force)
                    traffic_place_forced(m, actor, spawned, state);
                else
                    (void)sub_80012C1C(m, source, spawned + 360, offset);
                if (!rrj_u16(rrj_at(m, spawned + 362, 2)))
                    traffic_activate(m, spawned, state);
            }
        }
        rrj_write32(m, cooldown, 0xFFFF0000u);
    }
    return 0;
}
