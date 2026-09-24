#include "race_global_800B8020.h"
#include "fixed_math.h"
#include "race_pause.h"
#include "xport.h"

static uint8_t global_byte(RRJMemory *m, uint32_t address)
{
    return *(uint8_t *)rrj_at(m, address, 1);
}

static int8_t global_sbyte(RRJMemory *m, uint32_t address)
{
    return *(int8_t *)rrj_at(m, address, 1);
}

static void global_put_byte(RRJMemory *m, uint32_t address, uint32_t value)
{
    *(uint8_t *)rrj_at(m, address, 1) = (uint8_t)value;
}

static int32_t global_abs(int32_t value)
{
    return value < 0 ? (int32_t)(0u - (uint32_t)value) : value;
}

static int32_t global_clamp_axis(int32_t value)
{
    if (value < -15)
        return -15;
    if (value > 15)
        return 15;
    return value;
}

static int32_t global_nibble(RRJMemory *m, uint32_t address, uint32_t shift)
{
    return rrj_s32(rrj_read32(m, address) << shift) >> 28;
}

uint32_t sub_800B8020(RRJMemory *m, uint32_t delta, RRJRaceLeafCall call)
{
    uint32_t state = rrj_read32(m, 0x8005B2F8);
    uint32_t player_count = rrj_read32(m, state + 48);
    uint32_t base = rrj_read32(m, 0x800CE4D0);
    int32_t count = rrj_s32(rrj_read32(m, rrj_read32(m, 0x800CE4D8)));
    uint32_t collision_mask = 0;
    uint32_t split_side = 0;
    uint32_t index;

    FUNCTION_MARKER(0x800B8020, "RASHCDG.BIN");
    if (player_count == 2)
    {
        uint32_t first = rrj_read32(m, 0x8005B38C);
        uint32_t second = rrj_read32(m, 0x8005B21C);
        int32_t separation = (rrj_s32(rrj_read32(m, first + 324)) >> 12) -
                             (rrj_s32(rrj_read32(m, second + 324)) >> 12);
        uint32_t flags = rrj_read32(m, 0x800CCAC4);
        split_side = (uint32_t)separation >> 31;
        if (!(flags & 1) && global_abs(separation) >= 825)
            flags |= 0x11;
        if (global_abs(separation) < 825 && (flags & 1))
            flags = (flags & 0xFFFFFFBEu) | 0x40;
        rrj_write32(m, 0x800CCAC4, flags);
    }

    for (index = 0; index < (uint32_t)(count > 0 ? count : 0); ++index)
    {
        uint32_t actor = base + 1096 * index;
        uint32_t other_index;
        global_put_byte(m, actor + 851, 0);
        if (global_byte(m, 0x8005ADC0 + player_count - 1) <
                rrj_u16(rrj_at(m, 0x800CD540, 2)) &&
            (rrj_u16(rrj_at(m, actor + 320, 2)) & 0x30))
        {
            for (other_index = 0; other_index < (uint32_t)count; ++other_index)
            {
                uint32_t other = base + 1096 * other_index;
                if ((rrj_u16(rrj_at(m, other + 320, 2)) & 0x30) &&
                    global_byte(m, other + 850) < global_byte(m, actor + 850))
                    global_put_byte(m, actor + 851,
                                    global_byte(m, actor + 851) + 1);
            }
        }
    }

    for (index = 0; index < (uint32_t)(count > 0 ? count : 0); ++index)
    {
        uint32_t actor = base + 1096 * index;
        uint32_t descriptor;
        uint32_t identity;
        uint32_t old_position;
        uint32_t event;
        int process_actor = 1;

        if (!actor)
            continue;
        identity = rrj_u16(rrj_at(m, actor + 172, 2));
        descriptor = rrj_read32(m, actor + 1084);
        if ((identity < player_count ||
             (global_byte(m, descriptor + 1) & 0xF) != 2) &&
            (rrj_read32(m, descriptor + 40) ||
             global_byte(m, descriptor + 39) >= 0xF8))
            continue;
        if (rrj_read32(m, actor + 856) && rrj_read32(m, actor + 1088))
            actor = rrj_read32(m, actor + 856);
        for (;;)
        {
            uint32_t mode;
            uint32_t current;
            uint32_t target;
            descriptor = rrj_read32(m, actor + 1084);
            current = global_byte(m, descriptor + 15);
            target = global_byte(m, descriptor + 14);
            if (current < target)
            {
                mode = rrj_read32(m, rrj_read32(m, actor + 852) + 604);
                if (mode < 3)
                    current += 2;
                else if (mode == 4)
                    current += 4;
                if (current > target)
                    current = target;
                global_put_byte(m, descriptor + 15, current);
            }
            if (rrj_read32(m, actor + 1088))
                break;
            actor = rrj_read32(m, actor + 856);
        }
        identity = rrj_u16(rrj_at(m, actor + 172, 2));
        old_position = global_byte(m, descriptor + 39);
        if (!(rrj_read32(m, 0x8005B2A8) & 1))
        {
            global_put_byte(m, descriptor + 39,
                            sub_800138E8(m, actor, 0));
            global_put_byte(m, actor + 928,
                            global_byte(m, actor + 928) | 8);
        }
        global_put_byte(m, descriptor,
                        global_byte(m, descriptor) & ~8u);

        if (index >= 2 && (rrj_read32(m, 0x800CCAC4) & 0xF0))
        {
            uint32_t flags = rrj_read32(m, 0x800CCAC4);
            uint32_t axis = descriptor + (split_side ^ 1) + 16;
            if (flags & 0x10)
            {
                int32_t amount = 15 - global_sbyte(m, axis);
                if (amount > 8)
                    amount = 8;
                global_put_byte(m, 0x800CD530 + index - 2, amount);
                global_put_byte(m, axis,
                                global_byte(m, axis) + (uint8_t)amount);
            }
            if (flags & 0x40)
                global_put_byte(m, axis,
                    global_byte(m, axis) - global_byte(m, 0x800CD530 + index - 2));
            global_put_byte(m, descriptor + 16,
                global_clamp_axis(global_sbyte(m, descriptor + 16)));
            global_put_byte(m, descriptor + 17,
                global_clamp_axis(global_sbyte(m, descriptor + 17)));
        }

        if (!rrj_u16(rrj_at(m, actor + 320, 2)) ||
            !((identity < player_count) ||
              ((global_byte(m, descriptor + 1) & 0xF) != 2) ||
              (global_byte(m, actor + 928) & 0x10)))
            process_actor = 0;
        if (process_actor && (global_byte(m, state + 4) & 1) &&
            identity < player_count)
        {
            (void)sub_80097388(m, actor, call);
            if (rrj_read32(m, descriptor + 40))
                process_actor = 0;
        }
        if (process_actor &&
            (rrj_read32(m, actor + 560) & 0x18000000u) == 0x08000000u &&
            rrj_read32(m, 0x8005B2A8))
        {
            int32_t timer = rrj_s32(rrj_read32(m, descriptor + 4) + delta);
            uint32_t period = global_byte(m, descriptor + 3) << 16;
            int32_t steps = period ? (timer >> 16) / (int32_t)global_byte(m, descriptor + 3) : 0;
            uint32_t actions;
            int32_t adjustment = 0;
            uint32_t bit;
            uint32_t other_index;

            rrj_write32(m, descriptor + 4, (uint32_t)timer);
            event = actor + 8 * global_sbyte(m, actor + 946) + 948;
            if (steps)
                (void)sub_800BCEEC(m, descriptor + 2, (uint32_t)steps);
            if (period < (uint32_t)timer)
                rrj_write32(m, descriptor + 4, (uint32_t)timer - period);
            actions = sub_800BD2A0(m, actor, old_position) |
                      global_byte(m, descriptor + 68);
            global_put_byte(m, descriptor + 68, 0);
            for (bit = 0; bit < 7; ++bit)
                if (actions & (1u << bit))
                    adjustment += global_nibble(m, 0x8005ADDC, 4 * bit);
            if ((actions & 0x20) && (global_byte(m, descriptor + 61) >> 4) >= 2)
                global_put_byte(m, descriptor + 61,
                    (global_byte(m, descriptor + 61) & 0xF) |
                    (((global_byte(m, descriptor + 61) >> 4) - 1) << 4));
            if (actions & 0x40)
            {
                uint32_t high = global_byte(m, descriptor + 61) >> 4;
                if (high >= 3)
                    high -= 2;
                else if (high >= 2)
                    --high;
                global_put_byte(m, descriptor + 61,
                    (global_byte(m, descriptor + 61) & 0xF) | (high << 4));
            }

            for (other_index = 0; other_index < (uint32_t)count; ++other_index)
            {
                uint32_t other = base + 1096 * other_index;
                uint32_t interactions;
                int32_t other_adjustment = 0;
                uint32_t interaction;
                if (other_index == index || !other)
                    continue;
                interactions = sub_800BCF6C(m, actor, other);
                for (interaction = 0; interaction < 13; ++interaction)
                {
                    if (interactions & (1u << interaction))
                    {
                        uint32_t offset = 4 * (interaction / 8);
                        uint32_t shift = 4 * (interaction & 7);
                        adjustment += global_nibble(m, 0x8005ADD4 + offset, shift);
                        other_adjustment += global_nibble(m, 0x8005ADCC + offset, shift);
                    }
                }
                if (other_adjustment)
                {
                    uint32_t axis = descriptor +
                        global_sbyte(m, 0x800D38B0 +
                            rrj_u16(rrj_at(m, other + 172, 2))) + 16;
                    global_put_byte(m, axis,
                        global_clamp_axis(global_sbyte(m, axis) + other_adjustment));
                }
            }
            if (adjustment)
                (void)sub_800BD34C(m, descriptor + 2, adjustment);
            if ((uint32_t)(rrj_u16(rrj_at(m, event, 2)) - 10) < 6 &&
                sub_800A8C48(m, rrj_u16(rrj_at(m, event + 2, 2)),
                              rrj_read32(m, actor + 912)))
                (void)sub_800BD34C(m, descriptor + 61, 1);
            if (identity >= player_count &&
                (global_byte(m, descriptor + 1) & 0xF) == 2)
            {
                (void)sub_8009DC90(m, actor, call);
                continue;
            }
            if ((uint32_t)(rrj_u16(rrj_at(m, event, 2)) - 3) < 14 &&
                !rrj_read32(m, descriptor + 40))
            {
                int32_t position = rrj_s32(rrj_read32(m, actor + 324));
                int32_t distance = (position -
                    rrj_s32(rrj_read32(m, rrj_read32(m, 0x8005B38C) + 324))) >> 12;
                int32_t nearest = distance;
                uint32_t threshold;
                if (player_count == 2)
                {
                    int32_t other_distance = (position -
                        rrj_s32(rrj_read32(m, rrj_read32(m, 0x8005B21C) + 324))) >> 12;
                    nearest = global_abs(distance);
                    if (global_abs(other_distance) < nearest)
                        nearest = global_abs(other_distance);
                }
                threshold = (uint32_t)(nearest + 63);
                if (threshold >= 75)
                {
                    if (global_byte(m, actor + 851) <
                        global_byte(m, 0x8005ADC0 + player_count - 1))
                        continue;
                }
                else
                {
                    if (rrj_u16(rrj_at(m, event, 2)) != 4 &&
                        rrj_u16(rrj_at(m, event + 2, 2)) < player_count)
                        continue;
                    if (global_byte(m, actor + 851) <
                            global_byte(m, 0x8005ADC0 + player_count - 1) ||
                        identity < player_count)
                    {
                        collision_mask |= 1u << index;
                        continue;
                    }
                }
                global_put_byte(m, descriptor,
                                global_byte(m, descriptor) | 8);
            }
        }
    }

    if ((global_byte(m, state + 4) & 1) &&
        (rrj_read32(m, 0x8005AD48) & 0x1F))
    {
        for (index = 0; index < player_count; ++index)
        {
            uint32_t player = rrj_read32(m, 0x8005B268 + 4 * index);
            uint32_t descriptor = rrj_read32(m, player + 1084);
            if ((global_byte(m, descriptor + 1) & 0xF) == 2 &&
                (rrj_read32(m, player + 560) & 0x08000000))
                collision_mask = 0;
        }
    }

    for (index = 0; index < (uint32_t)(count > 0 ? count : 0); ++index)
    {
        uint32_t actor = base + 1096 * index;
        uint32_t descriptor = rrj_read32(m, actor + 1084);
        int32_t slot = global_sbyte(m, actor + 946);
        uint32_t event = actor + 8 * slot + 948;
        rrj_write32(m, actor + 912, 0);
        rrj_put16(rrj_at(m, descriptor + 64, 2), 0);
        rrj_put16(rrj_at(m, descriptor + 66, 2), 0);
        global_put_byte(m, descriptor + 70, 31);
        global_put_byte(m, descriptor + 71, 31);
        rrj_put16(rrj_at(m, event + 6, 2),
                  rrj_u16(rrj_at(m, event + 6, 2)) & 0xBFFF);
    }

    for (index = 0; index < (uint32_t)(count > 0 ? count : 0); ++index)
    {
        uint32_t actor = base + 1096 * index;
        if (collision_mask & (1u << index))
        {
            uint32_t descriptor = rrj_read32(m, actor + 1084);
            uint32_t chosen = 19;
            int32_t direction = -15;
            uint32_t scan;
            for (scan = 0; scan < 20; ++scan)
            {
                int32_t value = global_sbyte(m, descriptor + 16 + scan);
                uint32_t candidate = global_byte(m, 0x800D38C8 + scan);
                if (direction < value &&
                    candidate != rrj_u16(rrj_at(m, actor + 172, 2)) &&
                    candidate != 31 &&
                    sub_800BEA30(m,
                        rrj_read32(m, 0x8005B3A0) + 1096 * candidate + 172,
                        actor))
                {
                    direction = value;
                    chosen = candidate;
                }
            }
            if (chosen < rrj_read32(m, 0x8005B1F8))
            {
                uint32_t other = rrj_read32(m, 0x8005B3A0) + 1096 * chosen;
                uint32_t allow = 1;
                uint32_t other_id = rrj_u16(rrj_at(m, other + 172, 2));
                if (other_id < player_count)
                {
                    uint32_t mode = global_byte(m, state + 4);
                    if (mode != 36 && mode != 44)
                    {
                        uint16_t mask = rrj_u16(rrj_at(m, 0x800CCAC0 + 2 * chosen, 2));
                        uint32_t bit = rrj_u16(rrj_at(m, actor + 172, 2)) - 1;
                        mask &= (uint16_t)~(1u << bit);
                        allow = !mask || !(mask & (mask - 1));
                        rrj_put16(rrj_at(m, 0x800CCAC0 + 2 * other_id, 2),
                                  rrj_u16(rrj_at(m, 0x800CCAC0 + 2 * other_id, 2)) &
                                      (uint16_t)~(1u << bit));
                    }
                }
                if (allow)
                    (void)sub_800B8FB0(m, actor, direction, other, call);
            }
        }
    }

    for (index = 0; index < player_count; ++index)
    {
        uint32_t player = rrj_read32(m, 0x8005B268 + 4 * index);
        uint16_t pending = rrj_u16(rrj_at(m, 0x800CCAC0 + 2 * index, 2));
        if (global_byte(m, state + index + 10) == 2)
        {
            uint32_t body = rrj_read32(m, player + 856);
            uint32_t linked = body ? rrj_read32(m, body + 856) : 0;
            if (linked && rrj_read32(m, rrj_read32(m, linked + 852) + 604) < 2)
            {
                uint32_t candidate = 0;
                if (!pending && player_count >= 2)
                {
                    uint32_t alternate = rrj_read32(m, 0x8005B268 + 4 * (index == 0));
                    int32_t slot = global_sbyte(m, alternate + 946) - 1;
                    uint32_t prior = alternate + 8 * slot + 956;
                    if (rrj_u16(rrj_at(m, prior, 2)) == 16 &&
                        rrj_u16(rrj_at(m, prior + 2, 2)) ==
                            rrj_u16(rrj_at(m, player + 172, 2)))
                        candidate = index == 0;
                    else
                        continue;
                }
                else
                {
                    candidate = 1;
                    while (pending && !(pending & (1u << (candidate - 1))))
                        ++candidate;
                }
                while (candidate < 16)
                {
                    uint32_t other = rrj_read32(m, 0x8005B3A0) + 1096 * candidate;
                    int32_t separation;
                    uint32_t road = rrj_read32(m, other + 360);
                    if (road == rrj_read32(m, linked + 360) &&
                        (!(road >> 16) || rrj_read32(m, other + 336) ==
                                             rrj_read32(m, linked + 336)))
                    {
                        separation = rrj_s32(rrj_read32(m, other + 344) -
                                             rrj_read32(m, linked + 344));
                        if (rrj_s32(rrj_read32(m, linked + 364)) < 0)
                            separation = -separation;
                    }
                    else
                        separation = rrj_s32(sub_800B6AAC(
                            m, other + 184, linked + 432, linked + 184));
                    if (separation > 0)
                    {
                        uint8_t packet[8] = {0};
                        (void)sub_800B92C0(m, rrj_read32(m, body + 1084));
                        rrj_put16(packet, 16);
                        rrj_put16(packet + 2, (uint16_t)candidate);
                        (void)rrj_enqueue_actor_event_local(m, packet, 2, body);
                        break;
                    }
                    pending &= (uint16_t)~(1u << (candidate - 1));
                    ++candidate;
                    while (pending && !(pending & (1u << (candidate - 1))))
                        ++candidate;
                    if (!pending)
                        break;
                }
            }
        }
    }
    rrj_put16(rrj_at(m, 0x800CCAC2, 2), 0);
    rrj_put16(rrj_at(m, 0x800CCAC0, 2), 0);
    rrj_write32(m, 0x800CCAC4, rrj_read32(m, 0x800CCAC4) & 0xFFFFFF0F);
    return rrj_read32(m, 0x800CCAC4);
}
