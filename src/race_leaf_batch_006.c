#include "race_leaf_batch_006.h"
#include "race_pause.h"
#include "xport.h"

static uint32_t batch6_animation_set(uint32_t animation)
{
    uint32_t index;

    if (animation < 224)
        index = rrj_read32(0x800541D4u + 8u * animation) & 15u;
    else
        index = rrj_u16(rrj_at(0x800D5F40u + 2u * (animation - 224u), 2)) >> 12;
    return rrj_read32(0x800CE190u + 4u * index);
}

uint32_t sub_8001B44C(uint32_t x, uint32_t z, uint32_t actor, uint32_t active)
{
    FUNCTION_MARKER(0x8001B44C, "SLUS_010.53");
    if (active)
    {
        uint32_t state = rrj_read32(rrj_read32(actor + 96));
        uint32_t sound = state - 400u == 30u ? 104u : 103u;

        return sub_80017BA0((int32_t)x, (int32_t)z, sound, rrj_read32(0x8005B404u));
    }
    return 0;
}

uint32_t sub_8003297C(uint32_t packed, uint32_t group)
{
    uint32_t key = ((packed >> 16) & 0x8000u) | ((packed >> 13) & 0x7C00u) | (packed & 0x3FFu);

    FUNCTION_MARKER(0x8003297C, "SLUS_010.53");
    (void)sub_80034D38((int32_t)key, 0, group);
    return 1;
}

static int32_t batch6_distance_component(int32_t value)
{
    int32_t sign = value >> 31;

    return ((value >> 16) + sign) ^ sign;
}

static uint32_t batch6_send_identity(RRJMemory *m, uint32_t actor, uint16_t identity)
{
    const uint32_t packet = 0x1F8003F0u;
    uint32_t saved = rrj_read32(packet);
    uint32_t result;

    rrj_put16(rrj_at(packet, 2), 16);
    rrj_put16(rrj_at(packet + 2, 2), identity);
    result = sub_800BCA68(packet, 2, actor);
    rrj_write32(packet, saved);
    return result;
}

uint32_t sub_800C1DD4(uint32_t actor)
{
    uint32_t state = rrj_read32(actor + 1084);
    uint16_t identity;
    uint32_t history;

    FUNCTION_MARKER(0x800C1DD4, "RASHCDG.BIN");
    history = rrj_read32(state + 40);
    if (history)
        return history;
    if (r_u8(state + 39) >= 248u)
        return 16;

    if (rrj_read32(actor + 1088))
    {
        uint32_t relation = rrj_read32(actor + 852);

        if ((r_u8(relation + 572) & 0x10u) && (rrj_read32(relation + 552) & 0x800000u))
            identity = rrj_u16(rrj_at(rrj_read32(actor + 856) + 172, 2));
        else
        {
            uint32_t actor_identity = rrj_u16(rrj_at(actor + 172, 2));
            uint16_t cached = rrj_u16(rrj_at(0x800CCB6Cu + 2u * actor_identity, 2));
            uint32_t candidate = 0;
            uint32_t within_time = 0;

            if (cached != 224u)
            {
                uint32_t clock = rrj_read32(rrj_read32(0x8005B2F8u) + 16);
                uint32_t timestamp = rrj_read32(0x800CCB70u + 4u * actor_identity);
                within_time = rrj_s32(clock - 59u) < rrj_s32(timestamp);
            }
            if (within_time)
            {
                uint32_t actor_x, candidate_x, candidate_z, actor_z;
                int32_t x;
                int32_t z;
                int32_t major;
                int32_t minor;
                int32_t diagonal;
                int32_t distance;

                cached = rrj_u16(rrj_at(0x800CCB6Cu + 2u * actor_identity, 2));
                candidate = rrj_read32(0x8005B3A0u) + 1096u * cached;
                actor_x = rrj_read32(actor + 504);
                candidate_x = rrj_read32(candidate + 504);
                candidate_z = rrj_read32(candidate + 512);
                actor_z = rrj_read32(actor + 512);
                x = batch6_distance_component(rrj_s32(actor_x - candidate_x));
                z = batch6_distance_component(rrj_s32(actor_z - candidate_z));
                major = x < z ? z : x;
                minor = x < z ? x : z;
                diagonal = minor + (minor >> 1);
                distance = major - (major >> 5) - (major >> 7) + (diagonal >> 2) + (diagonal >> 6);
                if (distance >= 2)
                    candidate = 0;
            }
            identity = candidate ? rrj_u16(rrj_at(0x800CCB6Cu + 2u * rrj_u16(rrj_at(actor + 172, 2)), 2)) : (uint16_t)sub_8008B428(actor + 172, 1);
        }
    }
    else
    {
        uint32_t linked = rrj_read32(actor + 856);

        identity = (uint16_t)sub_8008B428(linked + 172, 1);
        if (rrj_read32(rrj_read32(actor + 852) + 552) & 0x800000u)
            identity = rrj_u16(rrj_at(rrj_read32(actor + 856) + 172, 2));
    }

    history = actor + 948u + 8u * (uint32_t)(int32_t)(int8_t)r_u8(actor + 946);
    if (rrj_u16(rrj_at(history, 2)) != 16u || rrj_u16(rrj_at(history + 2, 2)) != identity)
        return batch6_send_identity(rrj_host_context(), actor, identity);
    return identity;
}

static int32_t batch6_channel_value(RRJMemory *m, uint32_t input, uint32_t channel)
{
    uint32_t table = rrj_read32(input + 184);
    uint32_t index = rrj_read32(table + 4u * channel);

    return rrj_s32(rrj_read32(input + 8u * index + 20));
}

uint32_t sub_800C2070(uint32_t input, uint32_t selector)
{
    uint32_t pair = 0x800CCB78u + 4u * selector;
    int32_t value = batch6_channel_value(rrj_host_context(), input, rrj_u16(rrj_at(pair, 2)));
    uint32_t secondary;

    FUNCTION_MARKER(0x800C2070, "RASHCDG.BIN");
    if (value >= 0 || 0u - (uint32_t)value >= 75u)
        return 0;
    secondary = rrj_u16(rrj_at(pair + 2, 2));
    if (secondary == 15u)
        return 1;
    return batch6_channel_value(rrj_host_context(), input, secondary) > 0;
}

uint32_t sub_800C2100(uint32_t input, uint32_t selector)
{
    uint32_t channel = rrj_u16(rrj_at(0x800CCB78u + 4u * selector, 2));

    FUNCTION_MARKER(0x800C2100, "RASHCDG.BIN");
    return batch6_channel_value(rrj_host_context(), input, channel) > 0;
}

uint32_t sub_800C213C(uint32_t input, uint32_t selector)
{
    uint32_t channel = rrj_u16(rrj_at(0x800CCB78u + 4u * selector, 2));

    FUNCTION_MARKER(0x800C213C, "RASHCDG.BIN");
    return (uint32_t)batch6_channel_value(rrj_host_context(), input, channel) >> 31;
}

static void batch6_activate_event(RRJMemory *m, uint32_t state, uint32_t entry, uint32_t word, uint32_t index)
{
    uint32_t channel = (word >> 8) & 0x3Fu;

    rrj_write32(state + 560, 0);
    w_u8(state + 568, (uint8_t)channel);
    rrj_write32(state + 564, rrj_read32(entry + 4));
    w_u8(state + channel + 574, (uint8_t)((word >> 20) & 0x3Fu));
    xport_update_u8(state + 573, XPORT_MEMORY_UPDATE_OR, (uint8_t)(1u << (index & 31u)));
}

static void batch6_mark_event(RRJMemory *m, uint32_t state, uint32_t index)
{
    xport_update_u8(state + 573, XPORT_MEMORY_UPDATE_OR, (uint8_t)(1u << (index & 31u)));
}

uint32_t sub_800C258C(uint32_t input, uint32_t actor)
{
    uint32_t state = rrj_read32(actor + 852);
    uint32_t config = rrj_read32(0x8005AD4Cu) + 12u * r_u8(state + 569);
    uint32_t table = rrj_read32(config + 4);
    uint32_t base;
    uint32_t current;
    uint32_t count;
    uint32_t index;
    int active = 1;

    FUNCTION_MARKER(0x800C258C, "RASHCDG.BIN");
    if (r_u8(state + 572) & 2u)
        return 0;
    base = rrj_read32(state + 564);
    current = rrj_read32(state + 560) + rrj_read32(rrj_read32(0x8005B2F8u) + 32);
    rrj_write32(state + 560, current);
    if (r_u8(state + 568) < r_u8(state + 570))
        w_u8(state + 568, r_u8(state + 570));

    count = r_u8(config + 2);
    for (index = 0; active && index < count; ++index)
    {
        uint32_t entry = table + 12u * index;
        uint32_t word = rrj_read32(entry + 8);
        uint32_t channel = (word >> 8) & 0x3Fu;
        int32_t event_time = rrj_s32(rrj_read32(entry + 4) - base);
        int condition = 0;
        uint8_t marks = r_u8(state + 573);
        uint32_t kind = (r_u8(entry + 8)) >> 4;

        if (r_u8(state + 568) < channel && r_u8(state + channel + 573) == ((word >> 14) & 0x3Fu) && rrj_s32(current) >= rrj_s32(rrj_read32(entry) - base))
            condition = rrj_s32(current) < event_time;

        if (kind == 0)
        {
            if (condition && sub_800C2070(input, word & 15u))
                batch6_activate_event(rrj_host_context(), state, entry, word, index);
            else if (!(marks & (1u << (index & 31u))) && (r_u8(state + 568) >= channel || event_time < rrj_s32(current)))
                batch6_mark_event(rrj_host_context(), state, index);
        }
        else if (kind == 1 || kind == 2)
        {
            if (condition && sub_800C213C(input, word & 15u))
                batch6_activate_event(rrj_host_context(), state, entry, word, index);
            else if (condition && sub_800C2100(input, word & 15u))
                w_u8(state + channel + 574, (uint8_t)(word >> 26));
            else if (!(marks & (1u << (index & 31u))) && (r_u8(state + 568) >= channel || event_time < rrj_s32(current)))
                batch6_mark_event(rrj_host_context(), state, index);
        }
        else if (kind == 3)
        {
            if (!(marks & (1u << (index & 31u))))
            {
                if (!sub_800C2100(input, word & 15u))
                    batch6_activate_event(rrj_host_context(), state, entry, word, index);
                active = 0;
            }
        }
        else if (kind == 4 && !(marks & (1u << (index & 31u))) && r_u8(state + 568) < channel && r_u8(state + channel + 573) == ((word >> 14) & 0x3Fu))
        {
            batch6_activate_event(rrj_host_context(), state, entry, word, index);
            base = rrj_read32(state + 564);
        }
    }
    return 0;
}

uint32_t sub_80017DA0(uint32_t x, uint32_t z, uint32_t event, uint32_t flags)
{
    uint32_t descriptor = rrj_read32(event + 4);
    uint32_t sound;
    uint32_t kind;
    int triggered = 0;

    FUNCTION_MARKER(0x80017DA0, "SLUS_010.53");
    if ((descriptor & 0x01000000u) || ((descriptor & 0x04000000u) && (flags & 0x80u)) || ((descriptor & 0x02000000u) && !(flags & 0x80u)) || ((descriptor & 0x40000000u) && ((flags >> 4) & 3u) != 2u))
        triggered = 1;
    else if ((descriptor & 0x20000000u) && ((flags >> 4) & 3u) == 0 && !(flags & 0x40u))
    {
        uint32_t sample = sub_80043F00(0xF2000002u) & 0xFFu;

        if ((9u * sample) >> 8 == 0)
            triggered = 1;
    }

    if (!triggered)
        return rrj_read32(event + 8);

    sound = rrj_u16(rrj_at(event + 4, 2));
    kind = descriptor & 0xFF000000u;
    if (kind == 0x14000000u)
        sound = (flags & 15u) + 61u;
    else if (kind == 0x09000000u)
    {
        uint32_t sample = sub_80043F00(0xF2000002u) & 0xFFu;

        sound += (((descriptor >> 16) & 0xFFu) * sample) >> 8;
    }
    else if (kind == 0x40000000u)
    {
        uint32_t side = (sub_80043F00(0xF2000002u) & 0xFFu) >> 7;

        sound = side + ((((flags >> 4) & 3u) == 1u) ? 92u : 90u);
    }

    (void)sub_80017BA0((int32_t)x, (int32_t)z, sound, 0);
    return rrj_read32(event + 8);
}

static uint32_t batch6_animation_id(uint32_t animation)
{
    if (animation < 224)
        return (rrj_read32(0x800541D4u + 8u * animation) >> 4) & 0xFFFu;
    return rrj_u16(rrj_at(0x800D5F40u + 2u * (animation - 224u), 2)) & 0xFFFu;
}

uint32_t rrj_actor_begin_animation(uint32_t actor, uint32_t type, uint32_t variant,
                                   uint32_t animation, uint32_t speed)
{
    uint32_t object, set, identity, flags;

    /* Internal branch of RASHCDG 800CAAF0 */
    set = batch6_animation_set(animation);
    object = rrj_read32(actor + 540);
    (void)sub_80012858(object, set);
    if (type)
    {
        if (type == 2)
        {
            uint32_t x = rrj_read32(actor + 184);
            uint32_t config = 0x800CCC20u + 16u * r_u8(actor + 566);
            uint32_t z = rrj_read32(actor + 192);
            animation = rrj_u16(rrj_at(config + 4, 2));
            (void)sub_8001B44C(x, z, actor, 0);
        }
        else if (type == 1)
            animation = rrj_u16(rrj_at(0x800CCC20u + 16u * r_u8(actor + 566) + 6, 2));
        identity = batch6_animation_id(animation);
        object = rrj_read32(actor + 540);
        (void)sub_8005BF6C(object, identity, variant, speed, 0);
        xport_update_u8(actor + 565, XPORT_MEMORY_UPDATE_OR, 1u);
    }
    else
    {
        animation = 231;
        if (rrj_read32(actor + 388) & 1u)
            xport_update_u8(actor + 565, XPORT_MEMORY_UPDATE_OR, 1u);
        else
        {
            uint32_t config, selected_speed;
            xport_update_u8(actor + 565, XPORT_MEMORY_UPDATE_AND, 0xFEu);
            config = 0x800CCC20u + 16u * r_u8(actor + 566);
            animation = rrj_u16(rrj_at(config + 2, 2));
            selected_speed = rrj_read32(config + 8);
            rrj_write32(actor + 480, selected_speed);
        }
        set = batch6_animation_set(animation);
        object = rrj_read32(actor + 540);
        (void)sub_80012858(object, set);
        identity = batch6_animation_id(animation);
        object = rrj_read32(actor + 540);
        (void)sub_8005C0B0(object, identity, variant, speed, 0);
    }
    flags = rrj_read32(actor + 552);
    rrj_put16(rrj_at(actor + 544, 2), (uint16_t)animation);
    flags &= 0xFFFFFFFEu;
    rrj_write32(actor + 552, flags);
    return flags;
}
