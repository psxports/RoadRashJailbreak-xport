#include "race_global_8009D004.h"
#include "race_leaf.h"
#include "race_leaf_batch_002.h"
#include "race_pause.h"
#include "xport.h"

static uint32_t passing_mode(uint32_t state)
{
    uint32_t mode = r_u32(state + 60);
    uint32_t flags = (uint8_t)r_u8((state + 4));

    if (flags & 4u)
        return mode + 6u;
    return mode + ((0u - (flags & 1u)) & 3u);
}

static int32_t passing_direction(uint32_t actor, uint32_t route, int32_t movement, uint32_t late, uint32_t state)
{
    uint32_t current = r_u32(actor + 360);
    uint32_t record = r_u32(actor + 428);
    uint32_t link;
    int32_t direction;

    if (!(current >> 16))
    {
        if (current == route)
            direction = ((int32_t)(uint32_t)(r_u32(actor + 364))) > 0 ? 1 : -1;
        else
        {
            link = sub_8003B4B0(record, route & 0xFFFFu);
            direction = link ? ((int32_t)(uint32_t)(r_u32(link + 4))) : 1;
        }
        if (movement > 0)
        {
            if ((uint8_t)r_u8((state + 4)) == 44)
            {
                if ((sub_8001FC58() % 100u) < 76u)
                    direction = (int32_t)(0u - (uint32_t)direction);
            }
            else if (late)
                direction = (int32_t)(0u - (uint32_t)direction);
            else
                direction = sub_8003A98C(direction, actor);
        }
        return direction;
    }
    if (late)
        return 0;
    link = sub_8003B4B0(record, route & 0xFFFFu);
    if (!link)
        return ((int32_t)(uint32_t)(r_u32(actor + 364)));
    direction = ((int32_t)(uint32_t)(r_u32(link + 4)));
    if (sub_8003F580(record, route & 0xFFFFu))
        direction = (int32_t)(0u - (uint32_t)direction);
    return direction;
}

uint32_t sub_8009D004(uint32_t delta, uint32_t step, uint32_t enabled)
{
    int32_t scalar_value_6;
    uint32_t scalar_sign_6;

    uint32_t route_state[3];
    uint32_t source_record[3];
    uint32_t axis;
    const uint32_t specification = 0x1F8002A0u;
    uint32_t active[2] = {0, 0};
    uint32_t state;
    uint32_t reset = 0;
    uint32_t late;
    uint32_t mode;
    uint32_t scale;
    int32_t count;
    int32_t index;
    int32_t movement = ((int32_t)(uint32_t)(step << 16));

    FUNCTION_MARKER(0x8009D004, "RASHCDG.BIN");
    if (!enabled)
        return 0;
    state = r_u32(0x8005B2F8u);
    count = ((int32_t)(uint32_t)(r_u32(state + 48)));
    for (index = 0; index < count && index < 2; ++index)
    {
        uint32_t actor = r_u32(0x8005B268u + 4u * (uint32_t)index);
        uint32_t descriptor = r_u32(actor + 1084);

        active[index] = 1;
        (void)sub_8009F44C((uint32_t)index);
        if (!sub_8008CDF4(3) || !sub_8008DBA8((uint32_t)index) || r_u32(descriptor + 40) || ((uint8_t)r_u8(descriptor) & 0x40u))
            active[index] = 0;
    }
    if ((count == 2 && !active[0] && !active[1]) || (count == 1 && !active[0]))
        return 0;

    w_u32(0x8005B210u, r_u32(0x8005B210u) + delta);
    mode = passing_mode(state);
    scale = r_u32(0x80052FACu + 4u * mode);
    if (((int32_t)(300 * scale) >> 16) >= ((int32_t)(uint32_t)(r_u32(state + 16))))
        return 0;
    late = ((int32_t)(600 * scale) >> 16) >= ((int32_t)(uint32_t)(r_u32(state + 16)));
    (void)sub_8009FF24(active);

    for (index = 0; index < count && index < 2; ++index)
    {
        uint32_t actor;
        uint32_t kind;
        uint32_t route;
        int32_t direction;
        uint32_t conflict;

        if (!active[index])
            continue;
        actor = r_u32(0x8005B268u + 4u * (uint32_t)index);
        if ((int32_t)(r_u32(0x800D871C + 4 * (uint32_t)index) << 16) >= r_s32(0x8005B210))
            continue;
        kind = active[index];
        if (kind == 1)
        {
            uint32_t random = sub_8001FC58() % 100u;
            int32_t speed = ((int32_t)(uint32_t)(r_u32(actor + 480)));
            int32_t probability;

            if (speed > 1318365)
                probability = (int16_t)r_u16(0x800D8734u);
            else if ((uint8_t)r_u8((state + 4)) == 44)
                probability = 90;
            else if (speed > 327680)
                probability = (int16_t)r_u16(0x800D8736u);
            else
                probability = (int16_t)r_u16(0x800D8738u);
            if (probability > 100)
                probability = 100;
            if (100 - probability > 0 && (uint32_t)(100 - probability) >= random)
                movement = (int32_t)(0u - (uint32_t)movement);
        }
        else if (kind == 3)
            movement = (int32_t)(0u - (uint32_t)movement);
        if (movement < 0)
            movement = (int32_t)(0u - (r_u32(0x800D8730u) << 16));

        for (axis = 0; axis < 3; ++axis)
            source_record[axis] = r_u32(actor + 360 + 4 * axis);
        (void)sub_80012C1C(source_record, route_state, movement);
        route = route_state[0];
        if (route >> 16)
            continue;
        if (((uint8_t)r_u8((state + 4)) & 0x10u) && (route == 11u || route == 12u || route == 20u || route == 26u))
            continue;
        direction = passing_direction(actor, route, movement, late, state);
        if ((r_u32(actor + 360) >> 16) && late)
            continue;

        w_u32(specification + 8, (uint32_t)(int32_t)(int16_t)(uint16_t)route);
        w_u32(specification + 36, route_state[2]);
        w_u16(specification + 2, 0xFFFFu);
        w_u16(specification + 60, (uint16_t)direction);
        w_u16(specification + 64, 4);
        conflict = sub_8009F578(specification);
        if ((route != r_u32(actor + 360) || ((scalar_value_6 = (((int32_t)(uint32_t)(r_u32(actor + 368) - route_state[2])))), (scalar_sign_6 = (uint32_t)(scalar_value_6 >> 31)), (((int32_t)(((int32_t)(uint32_t)(((uint32_t)scalar_value_6 + scalar_sign_6) ^ scalar_sign_6)))))) > 0x00780000) && conflict != 1)
        {
            uint32_t spawned = sub_8009AD48(specification, actor);

            if (sub_8009FE90(spawned))
                reset = 1;
        }
    }
    if (reset)
        w_u32(0x8005B210u, 0);
    return 0;
}

uint32_t sub_8009CFF4(uint32_t delta)
{
    FUNCTION_MARKER(0x8009CFF4, "RASHCDG.BIN");
    return sub_8009D004(delta, r_u32(0x800D872Cu), r_u32(0x8005ACC4u));
}
