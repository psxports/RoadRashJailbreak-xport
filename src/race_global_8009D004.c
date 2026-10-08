#include "race_global_8009D004.h"
#include "race_leaf.h"
#include "race_pause.h"
#include "xport.h"

static uint8_t passing_byte(RRJMemory *m, uint32_t address)
{
    return r_u8(address);
}

static uint32_t passing_mode(RRJMemory *m, uint32_t state)
{
    uint32_t mode = rrj_read32(m, state + 60);
    uint32_t flags = passing_byte(m, state + 4);

    if (flags & 4u)
        return mode + 6u;
    return mode + ((0u - (flags & 1u)) & 3u);
}

static int32_t passing_abs(int32_t value)
{
    uint32_t sign = (uint32_t)(value >> 31);

    return rrj_s32(((uint32_t)value + sign) ^ sign);
}

static int32_t passing_direction(RRJMemory *m, uint32_t actor, uint32_t route, int32_t movement, uint32_t late, uint32_t state)
{
    uint32_t current = rrj_read32(m, actor + 360);
    uint32_t record = rrj_read32(m, actor + 428);
    uint32_t link;
    int32_t direction;

    if (!(current >> 16))
    {
        if (current == route)
            direction = rrj_s32(rrj_read32(m, actor + 364)) > 0 ? 1 : -1;
        else
        {
            link = sub_8003B4B0(m, record, route & 0xFFFFu);
            direction = link ? rrj_s32(rrj_read32(m, link + 4)) : 1;
        }
        if (movement > 0)
        {
            if (passing_byte(m, state + 4) == 44)
            {
                if ((sub_8001FC58(m) % 100u) < 76u)
                    direction = (int32_t)(0u - (uint32_t)direction);
            }
            else if (late)
                direction = (int32_t)(0u - (uint32_t)direction);
            else
                direction = sub_8003A98C(m, direction, actor);
        }
        return direction;
    }
    if (late)
        return 0;
    link = sub_8003B4B0(m, record, route & 0xFFFFu);
    if (!link)
        return rrj_s32(rrj_read32(m, actor + 364));
    direction = rrj_s32(rrj_read32(m, link + 4));
    if (sub_8003F580(m, record, route & 0xFFFFu))
        direction = (int32_t)(0u - (uint32_t)direction);
    return direction;
}

uint32_t sub_8009D004(RRJMemory *m, uint32_t delta, uint32_t step, uint32_t enabled, RRJRaceLeafCall call)
{
    const uint32_t route_state = 0x1F800280u;
    const uint32_t specification = 0x1F8002A0u;
    uint32_t active[2] = {0, 0};
    uint32_t state;
    uint32_t reset = 0;
    uint32_t late;
    uint32_t mode;
    uint32_t scale;
    int32_t count;
    int32_t index;
    int32_t movement = rrj_s32(step << 16);

    FUNCTION_MARKER(0x8009D004, "RASHCDG.BIN");
    if (!enabled)
        return 0;
    state = rrj_read32(m, 0x8005B2F8u);
    count = rrj_s32(rrj_read32(m, state + 48));
    for (index = 0; index < count && index < 2; ++index)
    {
        uint32_t actor = rrj_read32(m, 0x8005B268u + 4u * (uint32_t)index);
        uint32_t descriptor = rrj_read32(m, actor + 1084);

        active[index] = 1;
        (void)sub_8009F44C(m, (uint32_t)index);
        if (!sub_8008CDF4(m, 3) || !sub_8008DBA8(m, (uint32_t)index) || rrj_read32(m, descriptor + 40) || (passing_byte(m, descriptor) & 0x40u))
            active[index] = 0;
    }
    if ((count == 2 && !active[0] && !active[1]) || (count == 1 && !active[0]))
        return 0;

    rrj_write32(m, 0x8005B210u, rrj_read32(m, 0x8005B210u) + delta);
    mode = passing_mode(m, state);
    scale = rrj_read32(m, 0x80052FACu + 4u * mode);
    if (rrj_s32((300u * scale) >> 16) >= rrj_s32(rrj_read32(m, state + 16)))
        return 0;
    late = rrj_s32((600u * scale) >> 16) >= rrj_s32(rrj_read32(m, state + 16));
    (void)sub_8009FF24(m, active);

    for (index = 0; index < count && index < 2; ++index)
    {
        uint32_t actor;
        uint32_t kind;
        uint32_t route;
        int32_t direction;
        uint32_t conflict;

        if (!active[index])
            continue;
        actor = rrj_read32(m, 0x8005B268u + 4u * (uint32_t)index);
        if ((rrj_read32(m, 0x800D871Cu + 4u * (uint32_t)index) << 16) >= rrj_read32(m, 0x8005B210u))
            continue;
        kind = active[index];
        movement = rrj_s32(step << 16);
        if (kind == 1)
        {
            uint32_t random = sub_8001FC58(m) % 100u;
            int32_t speed = rrj_s32(rrj_read32(m, actor + 480));
            int32_t probability;

            if (speed > 1318365)
                probability = (int16_t)rrj_u16(rrj_at(m, 0x800D8734u, 2));
            else if (passing_byte(m, state + 4) == 44)
                probability = 90;
            else if (speed > 327680)
                probability = (int16_t)rrj_u16(rrj_at(m, 0x800D8736u, 2));
            else
                probability = (int16_t)rrj_u16(rrj_at(m, 0x800D8738u, 2));
            if (probability > 100)
                probability = 100;
            if (100 - probability > 0 && (uint32_t)(100 - probability) >= random)
                movement = (int32_t)(0u - (uint32_t)movement);
        }
        else if (kind == 3)
            movement = (int32_t)(0u - (uint32_t)movement);
        if (movement < 0)
            movement = (int32_t)(0u - (rrj_read32(m, 0x800D8730u) << 16));

        (void)sub_80012C1C(m, actor + 360, route_state, movement);
        route = rrj_read32(m, route_state);
        if (route >> 16)
            continue;
        if ((passing_byte(m, state + 4) & 0x10u) && (route == 11u || route == 12u || route == 20u || route == 26u))
            continue;
        direction = passing_direction(m, actor, route, movement, late, state);
        if ((rrj_read32(m, actor + 360) >> 16) && late)
            continue;

        rrj_write32(m, specification + 8, (uint32_t)(int32_t)(int16_t)(uint16_t)route);
        rrj_write32(m, specification + 36, rrj_read32(m, route_state + 8));
        rrj_put16(rrj_at(m, specification + 2, 2), 0xFFFFu);
        rrj_put16(rrj_at(m, specification + 60, 2), (uint16_t)direction);
        rrj_put16(rrj_at(m, specification + 64, 2), 4);
        conflict = sub_8009F578(m, specification);
        if ((route != rrj_read32(m, actor + 360) || passing_abs(rrj_s32(rrj_read32(m, actor + 368) - rrj_read32(m, route_state + 8))) > 0x00780000) && conflict != 1)
        {
            uint32_t spawned = sub_8009AD48(m, specification, actor, call);

            if (sub_8009FE90(m, spawned))
                reset = 1;
        }
    }
    if (reset)
        rrj_write32(m, 0x8005B210u, 0);
    return 0;
}

uint32_t sub_8009CFF4(RRJMemory *m, uint32_t delta, RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x8009CFF4, "RASHCDG.BIN");
    return sub_8009D004(m, delta, rrj_read32(m, 0x800D872Cu), rrj_read32(m, 0x8005ACC4u), call);
}
