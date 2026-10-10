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
    int32_t mode = rrj_s32(rrj_read32(state + 60));

    if (rrj_s32(rrj_read32(0x800D86F0u)) > 0)
        mode += 3;
    return mode;
}

static uint32_t traffic_threshold(RRJMemory *m, uint32_t state)
{
    return rrj_read32(0x800530FCu + 4u * (uint32_t)traffic_mode(m, state));
}

static void traffic_place_forced(RRJMemory *m, uint32_t actor, uint32_t spawned, uint32_t state)
{
    int32_t direction = rrj_s32(rrj_read32(actor + 364)) > 0 ? 1 : -1;
    uint32_t spacing = rrj_read32(0x80053114u + 4u * rrj_read32(state + 60));
    uint32_t position = rrj_read32(actor + 368);
    uint32_t route = actor + 328;
    uint32_t limit;
    uint32_t difference;
    uint32_t nonnegative;

    rrj_write32(spawned + 360, rrj_read32(actor + 360));
    rrj_write32(spawned + 364, (uint32_t)direction);
    if (direction > 0)
        position += spacing;
    else
        position -= spacing;
    rrj_write32(spawned + 368, position);
    limit = sub_8003C520(route);
    nonnegative = (uint32_t)(~(rrj_s32(position) >> 31)) & position;
    difference = sub_8003C520(route) - position;
    position = nonnegative + ((uint32_t)(rrj_s32(limit - position) >> 31) & difference);
    rrj_write32(spawned + 368, position);
}

static void traffic_activate(RRJMemory *m, uint32_t spawned, uint32_t state)
{
    uint8_t event[8] = {0};
    uint32_t speed = rrj_read32(0x80053138u + 4u * rrj_read32(state + 60));
    uint32_t descriptor;

    (void)sub_8002090C(spawned);
    rrj_write32(spawned + 924, speed);
    rrj_write32(spawned + 480, speed);
    (void)sub_80094184(spawned);
    rrj_put16(rrj_at(spawned + 320, 2), 1);
    rrj_write32(rrj_read32(spawned + 852) + 604, 1);
    sub_80093F94(spawned, 0, rrj_spu_reverb);
    (void)sub_800BCD10(spawned);
    rrj_put16(event, 4);
    rrj_put16(event + 2, 224);
    (void)rrj_enqueue_actor_event_local(m, event, 1, spawned);
    descriptor = rrj_read32(spawned + 1084);
    rrj_put16(rrj_at(descriptor + 62, 2), (uint16_t)(rrj_s32(rrj_read32(state + 16)) >> 8));
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
    state = rrj_read32(0x8005B2F8u);
    player_count = rrj_s32(rrj_read32(state + 48));
    for (index = 0; index < player_count && index < 2; ++index)
    {
        uint32_t actor = rrj_read32(0x8005B268u + 4u * (uint32_t)index);
        uint32_t descriptor = rrj_read32(actor + 1084);

        if (!rrj_read32(descriptor + 40) && !(traffic_byte(rrj_host_context(), state + 4) & 1u) && !(traffic_byte(rrj_host_context(), descriptor) & 0x40u))
        {
            uint32_t linked = rrj_read32(actor + 852);

            if ((int16_t)rrj_u16(rrj_at(actor + 320, 2)) || (rrj_read32(linked + 604) >= 3 && (int16_t)rrj_u16(rrj_at(linked + 320, 2))))
                active[index] = rrj_s32(rrj_read32(0x800D86F4u)) > 0;
        }
    }
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
            rrj_write32(cooldown, 0xFFFF0000u);
            continue;
        }
        actor = rrj_read32(0x8005B268u + 4u * (uint32_t)index);
        linked = rrj_read32(actor + 852);
        descriptor = rrj_read32(actor + 1084);
        route_limited = sub_80095410(actor);
        high_bit = traffic_byte(rrj_host_context(), descriptor) >> 7;
        final_position = traffic_byte(rrj_host_context(), descriptor + 39) == 1;
        if ((int32_t)traffic_byte(rrj_host_context(), descriptor + 39) >= rrj_s32(rrj_read32(0x8005B1F8u) - rrj_read32(0x800D86F4u)))
            urgent = rrj_s32(rrj_read32(actor + 480)) <= 1464849;
        if (traffic_byte(rrj_host_context(), state + 4) == 44 && traffic_byte(rrj_host_context(), state + 57) == 3)
            force = 1;

        timer = 0x8005B2A0u + 4u * (uint32_t)index;
        rrj_write32(timer, rrj_read32(timer) + delta);
        state_mode = rrj_read32(state + 60);
        if (rrj_s32(rrj_read32(timer)) >= rrj_s32(traffic_threshold(rrj_host_context(), state)))
        {
            uint32_t chance = sub_8001FC58() % 100u;
            uint32_t limit = rrj_read32(0x80053120u + 4u * (uint32_t)traffic_mode(rrj_host_context(), state));

            if (chance < limit && rrj_s32(rrj_read32(0x80053114u + 4u * state_mode)) < rrj_s32(sub_8003A5F4(actor + 360, 0x1F8003F0u, 1)))
                force = 1;
            rrj_write32(timer, 0);
        }

        should_consider = rrj_read32(linked + 604) != 1 || rrj_s32(rrj_read32(0x80053138u + 4u * state_mode)) >= rrj_s32(rrj_read32(actor + 480)) || high_bit || route_limited || final_position || urgent || force;
        if (!should_consider)
        {
            rrj_write32(cooldown, 0xFFFF0000u);
            continue;
        }
        if (rrj_s32(rrj_read32(cooldown)) >= 0)
            rrj_write32(cooldown, rrj_read32(cooldown) + delta);
        else
            rrj_write32(cooldown, 0);
        threshold = traffic_threshold(rrj_host_context(), state);
        if (!force && rrj_s32(rrj_read32(cooldown)) <= rrj_s32(threshold) && (!high_bit || rrj_s32(rrj_read32(cooldown)) <= (rrj_s32(threshold) >> 1)) && (!route_limited || rrj_s32(rrj_read32(cooldown)) <= (rrj_s32(threshold) >> 1)))
            continue;
        {
            uint32_t spawned = sub_80095848();

            if (spawned)
            {
                uint32_t source = actor + 360;
                int32_t offset = -(rrj_s32(rrj_read32(0x80053114u + 4u * state_mode)) >> 1);

                if (rrj_read32(linked + 604) >= 3)
                {
                    if (rrj_s32(rrj_read32(actor + 364) ^ rrj_read32(linked + 364)) < 0)
                        offset = rrj_s32(rrj_read32(0x80053114u + 4u * state_mode)) >> 1;
                    source = linked + 360;
                    (void)sub_80012C1C(source, spawned + 360, offset);
                }
                else if (force)
                    traffic_place_forced(rrj_host_context(), actor, spawned, state);
                else
                    (void)sub_80012C1C(source, spawned + 360, offset);
                if (!rrj_u16(rrj_at(spawned + 362, 2)))
                    traffic_activate(rrj_host_context(), spawned, state);
            }
        }
        rrj_write32(cooldown, 0xFFFF0000u);
    }
    return 0;
}
