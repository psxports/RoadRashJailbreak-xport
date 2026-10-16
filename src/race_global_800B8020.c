#include "race_global_800B8020.h"
#include "fixed_math.h"
#include "race_pause.h"
#include "xport.h"

static int8_t global_sbyte(uint32_t address)
{
    return r_s8(address);
}

static int32_t global_clamp_axis(int32_t value)
{
    if (value < -15)
        return -15;
    if (value > 15)
        return 15;
    return value;
}

uint32_t sub_800B8020(uint32_t delta)
{
    uint32_t state = r_u32(0x8005B2F8);
    uint32_t player_count = r_u32(state + 48);
    uint32_t base = r_u32(0x800CE4D0);
    int32_t count = ((int32_t)(uint32_t)(r_u32(r_u32(0x800CE4D8))));
    uint32_t collision_mask = 0;
    uint32_t split_side = 0;
    uint32_t index;
    uint32_t previous_interaction_actor = 0x80060000;

    FUNCTION_MARKER(0x800B8020, "RASHCDG.BIN");
    if (player_count == 2)
    {
        uint32_t first = r_u32(0x8005B38C);
        uint32_t second = r_u32(0x8005B21C);
        int32_t separation = (((int32_t)(uint32_t)(r_u32(first + 324))) >> 12) - (((int32_t)(uint32_t)(r_u32(second + 324))) >> 12);
        uint32_t flags = r_u32(0x800CCAC4);
        split_side = (uint32_t)separation >> 31;
        if (!(flags & 1) && ((int32_t)(((int32_t)(separation)) < 0 ? (int32_t)(0u - (uint32_t)((int32_t)(separation))) : ((int32_t)(separation)))) >= 825)
            flags |= 0x11;
        if (((int32_t)(((int32_t)(separation)) < 0 ? (int32_t)(0u - (uint32_t)((int32_t)(separation))) : ((int32_t)(separation)))) < 825 && (flags & 1))
            flags = (flags & 0xFFFFFFBEu) | 0x40;
        w_u32(0x800CCAC4, flags);
    }

    for (index = 0; index < (uint32_t)(count > 0 ? count : 0); ++index)
    {
        uint32_t actor = base + 1096 * index;
        uint32_t other_index;
        w_u8((actor + 851), (uint8_t)0);
        if ((uint8_t)r_u8((0x8005ADC0 + player_count - 1)) < r_u16(0x800CD540) && (r_u16(actor + 320) & 0x30))
        {
            previous_interaction_actor = base;
            for (other_index = 0; other_index < (uint32_t)count; ++other_index)
            {
                uint32_t other = base + 1096 * other_index;
                if ((r_u16(other + 320) & 0x30) && (uint8_t)r_u8((other + 850)) < (uint8_t)r_u8((actor + 850)))
                    w_u8((actor + 851), (uint8_t)((uint8_t)r_u8((actor + 851)) + 1));
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
        identity = r_u16(actor + 172);
        descriptor = r_u32(actor + 1084);
        if ((identity < player_count || ((uint8_t)r_u8((descriptor + 1)) & 0xF) != 2) && (r_u32(descriptor + 40) || (uint8_t)r_u8((descriptor + 39)) >= 0xF8))
            continue;
        if (r_u32(actor + 856) && r_u32(actor + 1088))
            actor = r_u32(actor + 856);
        for (;;)
        {
            uint32_t mode;
            uint32_t current;
            uint32_t target;
            descriptor = r_u32(actor + 1084);
            current = (uint8_t)r_u8((descriptor + 15));
            target = (uint8_t)r_u8((descriptor + 14));
            if (current < target)
            {
                mode = r_u32(r_u32(actor + 852) + 604);
                if (mode < 3)
                    current += 2;
                else if (mode == 4)
                    current += 4;
                if (current > target)
                    current = target;
                w_u8((descriptor + 15), (uint8_t)current);
            }
            if (r_u32(actor + 1088))
                break;
            actor = r_u32(actor + 856);
        }
        identity = r_u16(actor + 172);
        old_position = (uint8_t)r_u8((descriptor + 39));
        if (!(r_u32(0x8005B2A8) & 1))
        {
            /* Preserve the saved interaction pointer consumed by actor events */
            w_u16(0x801FFEDC, (uint16_t)previous_interaction_actor);
            w_u8((descriptor + 39), (uint8_t)(sub_800138E8(actor, 0)));
            w_u8((actor + 928), (uint8_t)((uint8_t)r_u8((actor + 928)) | 8));
        }
        w_u8(descriptor, (uint8_t)((uint8_t)r_u8(descriptor) & ~8u));

        if (index >= 2 && (r_u32(0x800CCAC4) & 0xF0))
        {
            uint32_t flags = r_u32(0x800CCAC4);
            uint32_t axis = descriptor + (split_side ^ 1) + 16;
            if (flags & 0x10)
            {
                int32_t amount = 15 - global_sbyte(axis);
                if (amount > 8)
                    amount = 8;
                w_u8((0x800CD530 + index - 2), (uint8_t)amount);
                w_u8(axis, (uint8_t)((uint8_t)r_u8(axis) + (uint8_t)amount));
            }
            if (flags & 0x40)
                w_u8(axis, (uint8_t)((uint8_t)r_u8(axis) - (uint8_t)r_u8((0x800CD530 + index - 2))));
            w_u8((descriptor + 16), (uint8_t)(global_clamp_axis(global_sbyte(descriptor + 16))));
            w_u8((descriptor + 17), (uint8_t)(global_clamp_axis(global_sbyte(descriptor + 17))));
        }

        if (!r_u16(actor + 320) || !((identity < player_count) || (((uint8_t)r_u8((descriptor + 1)) & 0xF) != 2) || ((uint8_t)r_u8((actor + 928)) & 0x10)))
            process_actor = 0;
        if (process_actor && ((uint8_t)r_u8((state + 4)) & 1) && identity < player_count)
        {
            (void)sub_80097388(actor);
            if (r_u32(descriptor + 40))
                process_actor = 0;
        }
        if (process_actor && (r_u32(actor + 560) & 0x18000000u) == 0x08000000u && r_u32(0x8005B2A8))
        {
            int32_t timer = ((int32_t)(uint32_t)(r_u32(descriptor + 4) + delta));
            uint32_t period = (uint8_t)r_u8((descriptor + 3)) << 16;
            int32_t steps = period ? (timer >> 16) / (int32_t)r_u8((descriptor + 3)) : 0;
            uint32_t actions;
            int32_t adjustment = 0;
            uint32_t bit;
            uint32_t other_index;

            w_u32(descriptor + 4, (uint32_t)timer);
            event = actor + 8 * global_sbyte(actor + 946) + 948;
            if (steps)
                (void)sub_800BCEEC(descriptor + 2, (uint32_t)steps);
            if (period < (uint32_t)timer)
                w_u32(descriptor + 4, (uint32_t)timer - period);
            actions = sub_800BD2A0(actor, old_position) | (uint8_t)r_u8((descriptor + 68));
            w_u8((descriptor + 68), (uint8_t)0);
            for (bit = 0; bit < 7; ++bit)
                if (actions & (1u << bit))
                    adjustment += ((int32_t)(((int32_t)(uint32_t)(r_u32(((uint32_t)(0x8005ADDC))) << ((uint32_t)(4 * bit)))) >> 28));
            if ((actions & 0x20) && ((uint8_t)r_u8((descriptor + 61)) >> 4) >= 2)
                w_u8((descriptor + 61), (uint8_t)(((uint8_t)r_u8((descriptor + 61)) & 0xF) | ((((uint8_t)r_u8((descriptor + 61)) >> 4) - 1) << 4)));
            if (actions & 0x40)
            {
                uint32_t high = (uint8_t)r_u8((descriptor + 61)) >> 4;
                if (high >= 3)
                    high -= 2;
                else if (high >= 2)
                    --high;
                w_u8((descriptor + 61), (uint8_t)(((uint8_t)r_u8((descriptor + 61)) & 0xF) | (high << 4)));
            }

            for (other_index = 0; other_index < (uint32_t)count; ++other_index)
            {
                uint32_t other = base + 1096 * other_index;
                uint32_t interactions;
                int32_t other_adjustment = 0;
                uint32_t interaction;
                previous_interaction_actor = other;
                if (other_index == index || !other)
                    continue;
                interactions = sub_800BCF6C(actor, other);
                for (interaction = 0; interaction < 13; ++interaction)
                {
                    if (interactions & (1u << interaction))
                    {
                        uint32_t offset = 4 * (interaction / 8);
                        uint32_t shift = 4 * (interaction & 7);
                        adjustment += ((int32_t)(((int32_t)(uint32_t)(r_u32(((uint32_t)(0x8005ADD4 + offset))) << ((uint32_t)(shift)))) >> 28));
                        other_adjustment += ((int32_t)(((int32_t)(uint32_t)(r_u32(((uint32_t)(0x8005ADCC + offset))) << ((uint32_t)(shift)))) >> 28));
                    }
                }
                if (other_adjustment)
                {
                    uint32_t axis = descriptor + global_sbyte(0x800D38B0 + r_u16(other + 172)) + 16;
                    w_u8(axis, (uint8_t)(global_clamp_axis(global_sbyte(axis) + other_adjustment)));
                }
            }
            if (adjustment)
                (void)sub_800BD34C(descriptor + 2, adjustment);
            if ((uint32_t)(r_u16(event) - 10) < 6 && sub_800A8C48(r_u16(event + 2), r_u32(actor + 912)))
                (void)sub_800BD34C(descriptor + 61, 1);
            if (identity >= player_count && ((uint8_t)r_u8((descriptor + 1)) & 0xF) == 2)
            {
                (void)sub_8009DC90(actor);
                continue;
            }
            if ((uint32_t)(r_u16(event) - 3) < 14 && !r_u32(descriptor + 40))
            {
                int32_t position = ((int32_t)(uint32_t)(r_u32(actor + 324)));
                int32_t distance = (position - ((int32_t)(uint32_t)(r_u32(r_u32(0x8005B38C) + 324)))) >> 12;
                int32_t nearest = distance;
                uint32_t threshold;
                if (player_count == 2)
                {
                    int32_t other_distance = (position - ((int32_t)(uint32_t)(r_u32(r_u32(0x8005B21C) + 324)))) >> 12;
                    nearest = ((int32_t)(((int32_t)(distance)) < 0 ? (int32_t)(0u - (uint32_t)((int32_t)(distance))) : ((int32_t)(distance))));
                    if (((int32_t)(((int32_t)(other_distance)) < 0 ? (int32_t)(0u - (uint32_t)((int32_t)(other_distance))) : ((int32_t)(other_distance)))) < nearest)
                        nearest = ((int32_t)(((int32_t)(other_distance)) < 0 ? (int32_t)(0u - (uint32_t)((int32_t)(other_distance))) : ((int32_t)(other_distance))));
                }
                threshold = (uint32_t)(nearest + 63);
                if (threshold >= 75)
                {
                    if ((uint8_t)r_u8((actor + 851)) < (uint8_t)r_u8((0x8005ADC0 + player_count - 1)))
                        continue;
                }
                else
                {
                    if (r_u16(event) != 4 && r_u16(event + 2) < player_count)
                        continue;
                    if ((uint8_t)r_u8((actor + 851)) < (uint8_t)r_u8((0x8005ADC0 + player_count - 1)) || identity < player_count)
                    {
                        collision_mask |= 1u << index;
                        continue;
                    }
                }
                w_u8(descriptor, (uint8_t)((uint8_t)r_u8(descriptor) | 8));
            }
        }
    }

    if (((uint8_t)r_u8((state + 4)) & 1) && (r_u32(0x8005AD48) & 0x1F))
    {
        for (index = 0; index < player_count; ++index)
        {
            uint32_t player = r_u32(0x8005B268 + 4 * index);
            uint32_t descriptor = r_u32(player + 1084);
            if (((uint8_t)r_u8((descriptor + 1)) & 0xF) == 2 && (r_u32(player + 560) & 0x08000000))
                collision_mask = 0;
        }
    }

    for (index = 0; index < (uint32_t)(count > 0 ? count : 0); ++index)
    {
        uint32_t actor = base + 1096 * index;
        uint32_t descriptor = r_u32(actor + 1084);
        int32_t slot = global_sbyte(actor + 946);
        uint32_t event = actor + 8 * slot + 948;
        w_u32(actor + 912, 0);
        w_u16(descriptor + 64, 0);
        w_u16(descriptor + 66, 0);
        w_u8((descriptor + 70), (uint8_t)31);
        w_u8((descriptor + 71), (uint8_t)31);
        w_u16(event + 6, r_u16(event + 6) & 0xBFFF);
    }

    for (index = 0; index < (uint32_t)(count > 0 ? count : 0); ++index)
    {
        uint32_t actor = base + 1096 * index;
        if (collision_mask & (1u << index))
        {
            uint32_t descriptor = r_u32(actor + 1084);
            uint32_t chosen = 19;
            int32_t direction = -15;
            uint32_t scan;
            for (scan = 0; scan < 20; ++scan)
            {
                int32_t value = global_sbyte(descriptor + 16 + scan);
                uint32_t candidate = (uint8_t)r_u8((0x800D38C8 + scan));
                if (direction < value && candidate != r_u16(actor + 172) && candidate != 31 && sub_800BEA30(r_u32(0x8005B3A0) + 1096 * candidate + 172, actor))
                {
                    direction = value;
                    chosen = candidate;
                }
            }
            if (chosen < r_u32(0x8005B1F8))
            {
                uint32_t other = r_u32(0x8005B3A0) + 1096 * chosen;
                uint32_t allow = 1;
                uint32_t other_id = r_u16(other + 172);
                if (other_id < player_count)
                {
                    uint32_t mode = (uint8_t)r_u8((state + 4));
                    if (mode != 36 && mode != 44)
                    {
                        uint16_t mask = r_u16(0x800CCAC0 + 2 * chosen);
                        uint32_t bit = r_u16(actor + 172) - 1;
                        mask &= (uint16_t)~(1u << bit);
                        allow = !mask || !(mask & (mask - 1));
                        w_u16(0x800CCAC0 + 2 * other_id, r_u16(0x800CCAC0 + 2 * other_id) & (uint16_t)~(1u << bit));
                    }
                }
                if (allow)
                    (void)sub_800B8FB0(actor, direction, other);
            }
        }
    }

    for (index = 0; index < player_count; ++index)
    {
        uint32_t player = r_u32(0x8005B268 + 4 * index);
        uint16_t pending = r_u16(0x800CCAC0 + 2 * index);
        if ((uint8_t)r_u8((state + index + 10)) == 2)
        {
            uint32_t body = r_u32(player + 856);
            uint32_t linked = body ? r_u32(body + 856) : 0;
            if (linked && r_u32(r_u32(linked + 852) + 604) < 2)
            {
                uint32_t candidate = 0;
                if (!pending && player_count >= 2)
                {
                    uint32_t alternate = r_u32(0x8005B268 + 4 * (index == 0));
                    int32_t slot = global_sbyte(alternate + 946) - 1;
                    uint32_t prior = alternate + 8 * slot + 956;
                    if (r_u16(prior) == 16 && r_u16(prior + 2) == r_u16(player + 172))
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
                    uint32_t other = r_u32(0x8005B3A0) + 1096 * candidate;
                    int32_t separation;
                    uint32_t road = r_u32(other + 360);
                    if (road == r_u32(linked + 360) && (!(road >> 16) || r_u32(other + 336) == r_u32(linked + 336)))
                    {
                        separation = ((int32_t)(uint32_t)(r_u32(other + 344) - r_u32(linked + 344)));
                        if (((int32_t)(uint32_t)(r_u32(linked + 364))) < 0)
                            separation = -separation;
                    }
                    else
                        separation = ((int32_t)(uint32_t)(sub_800B6AAC(xport_guest_ptr(other + 184, 12), xport_guest_ptr(linked + 432, 6), xport_guest_ptr(linked + 184, 12))));
                    if (separation > 0)
                    {
                        uint8_t packet[8] = {0};
                        (void)sub_800B92C0(r_u32(body + 1084));
                        xport_store_le16(packet, 16);
                        xport_store_le16(packet + 2, (uint16_t)candidate);
                        (void)sub_800BCA68(packet, 2, body);
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
    w_u16(0x800CCAC2, 0);
    w_u16(0x800CCAC0, 0);
    w_u32(0x800CCAC4, r_u32(0x800CCAC4) & 0xFFFFFF0F);
    return r_u32(0x800CCAC4);
}
