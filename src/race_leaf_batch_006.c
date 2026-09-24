#include "race_leaf_batch_006.h"
#include "race_pause.h"
#include "xport.h"

static uint32_t batch6_animation_set(RRJMemory *m, uint32_t animation)
{
    uint32_t index;

    if (animation < 224)
        index = rrj_read32(m, 0x800541D4u + 8u * animation) & 15u;
    else
        index = rrj_u16(rrj_at(
                    m, 0x800D5F40u + 2u * (animation - 224u), 2)) >>
                12;
    return rrj_read32(m, 0x800CE190u + 4u * index);
}

uint32_t sub_8001B44C(RRJMemory *m, uint32_t x, uint32_t z,
                      uint32_t actor, uint32_t active)
{
    FUNCTION_MARKER(0x8001B44C, "SLUS_010.53");
    if (active)
    {
        uint32_t state = rrj_read32(m, rrj_read32(m, actor + 96));
        uint32_t sound = state - 400u == 30u ? 104u : 103u;

        return sub_80017BA0(m, (int32_t)x, (int32_t)z, sound,
                            rrj_read32(m, 0x8005B404u));
    }
    return 0;
}

uint32_t sub_8003297C(RRJMemory *m, uint32_t packed, uint32_t group)
{
    uint32_t key = ((packed >> 16) & 0x8000u) |
                   ((packed >> 13) & 0x7C00u) | (packed & 0x3FFu);

    FUNCTION_MARKER(0x8003297C, "SLUS_010.53");
    (void)sub_80034D38(m, (int32_t)key, 0, group);
    return 1;
}

static int32_t batch6_distance_component(int32_t value)
{
    int32_t sign = value >> 31;

    return ((value >> 16) + sign) ^ sign;
}

static uint32_t batch6_send_identity(RRJMemory *m, uint32_t actor,
                                     uint16_t identity)
{
    const uint32_t packet = 0x1F8003F0u;
    uint32_t saved = rrj_read32(m, packet);
    uint32_t result;

    rrj_put16(rrj_at(m, packet, 2), 16);
    rrj_put16(rrj_at(m, packet + 2, 2), identity);
    result = sub_800BCA68(m, packet, 2, actor);
    rrj_write32(m, packet, saved);
    return result;
}

uint32_t sub_800C1DD4(RRJMemory *m, uint32_t actor)
{
    uint32_t state = rrj_read32(m, actor + 1084);
    uint16_t identity;
    uint32_t history;

    FUNCTION_MARKER(0x800C1DD4, "RASHCDG.BIN");
    if (rrj_read32(m, state + 40))
        return rrj_read32(m, state + 40);
    if (*(uint8_t *)rrj_at(m, state + 39, 1) >= 248u)
        return 16;

    if (rrj_read32(m, actor + 1088))
    {
        uint32_t relation = rrj_read32(m, actor + 852);

        if ((*(uint8_t *)rrj_at(m, relation + 572, 1) & 0x10u) &&
            (rrj_read32(m, relation + 552) & 0x800000u))
            identity = rrj_u16(rrj_at(
                m, rrj_read32(m, actor + 856) + 172, 2));
        else
        {
            uint32_t actor_identity = rrj_u16(rrj_at(m, actor + 172, 2));
            uint16_t cached = rrj_u16(rrj_at(
                m, 0x800CCB6Cu + 2u * actor_identity, 2));
            uint32_t candidate = 0;

            if (cached != 224u &&
                rrj_read32(m, rrj_read32(m, 0x8005B2F8u) + 16) - 59u <
                    rrj_read32(m, 0x800CCB70u + 4u * actor_identity))
            {
                int32_t x;
                int32_t z;
                int32_t major;
                int32_t minor;
                int32_t diagonal;
                int32_t distance;

                candidate = rrj_read32(m, 0x8005B3A0u) + 1096u * cached;
                x = batch6_distance_component(
                    rrj_s32(rrj_read32(m, actor + 504) -
                            rrj_read32(m, candidate + 504)));
                z = batch6_distance_component(
                    rrj_s32(rrj_read32(m, actor + 512) -
                            rrj_read32(m, candidate + 512)));
                major = x < z ? z : x;
                minor = x < z ? x : z;
                diagonal = minor + (minor >> 1);
                distance = major - (major >> 5) - (major >> 7) +
                           (diagonal >> 2) + (diagonal >> 6);
                if (distance >= 2)
                    candidate = 0;
            }
            identity = candidate
                           ? cached
                           : (uint16_t)sub_8008B428(m, actor + 172, 1);
        }
    }
    else
    {
        uint32_t linked = rrj_read32(m, actor + 856);

        identity = (uint16_t)sub_8008B428(m, linked + 172, 1);
        if (rrj_read32(m, rrj_read32(m, actor + 852) + 552) &
            0x800000u)
            identity = rrj_u16(rrj_at(m, linked + 172, 2));
    }

    history = actor + 948u +
              8u * (uint32_t)(int32_t)(int8_t)*(uint8_t *)rrj_at(
                       m, actor + 946, 1);
    if (rrj_u16(rrj_at(m, history, 2)) != 16u ||
        rrj_u16(rrj_at(m, history + 2, 2)) != identity)
        return batch6_send_identity(m, actor, identity);
    return identity;
}

static int32_t batch6_channel_value(RRJMemory *m, uint32_t input,
                                    uint32_t channel)
{
    uint32_t table = rrj_read32(m, input + 184);
    uint32_t index = rrj_read32(m, table + 4u * channel);

    return rrj_s32(rrj_read32(m, input + 8u * index + 20));
}

uint32_t sub_800C2070(RRJMemory *m, uint32_t input, uint32_t selector)
{
    uint32_t pair = 0x800CCB78u + 4u * selector;
    int32_t value = batch6_channel_value(
        m, input, rrj_u16(rrj_at(m, pair, 2)));
    uint32_t secondary;

    FUNCTION_MARKER(0x800C2070, "RASHCDG.BIN");
    if (value >= 0 || 0u - (uint32_t)value >= 75u)
        return 0;
    secondary = rrj_u16(rrj_at(m, pair + 2, 2));
    if (secondary == 15u)
        return 1;
    return batch6_channel_value(m, input, secondary) > 0;
}

uint32_t sub_800C2100(RRJMemory *m, uint32_t input, uint32_t selector)
{
    uint32_t channel = rrj_u16(rrj_at(
        m, 0x800CCB78u + 4u * selector, 2));

    FUNCTION_MARKER(0x800C2100, "RASHCDG.BIN");
    return batch6_channel_value(m, input, channel) > 0;
}

uint32_t sub_800C213C(RRJMemory *m, uint32_t input, uint32_t selector)
{
    uint32_t channel = rrj_u16(rrj_at(
        m, 0x800CCB78u + 4u * selector, 2));

    FUNCTION_MARKER(0x800C213C, "RASHCDG.BIN");
    return (uint32_t)batch6_channel_value(m, input, channel) >> 31;
}

static void batch6_activate_event(RRJMemory *m, uint32_t state,
                                  uint32_t entry, uint32_t word,
                                  uint32_t index)
{
    uint32_t channel = (word >> 8) & 0x3Fu;

    rrj_write32(m, state + 560, 0);
    *(uint8_t *)rrj_at(m, state + 568, 1) = (uint8_t)channel;
    rrj_write32(m, state + 564, rrj_read32(m, entry + 4));
    *(uint8_t *)rrj_at(m, state + channel + 574, 1) =
        (uint8_t)((word >> 20) & 0x3Fu);
    *(uint8_t *)rrj_at(m, state + 573, 1) |=
        (uint8_t)(1u << (index & 31u));
}

static void batch6_mark_event(RRJMemory *m, uint32_t state,
                              uint32_t index)
{
    *(uint8_t *)rrj_at(m, state + 573, 1) |=
        (uint8_t)(1u << (index & 31u));
}

uint32_t sub_800C258C(RRJMemory *m, uint32_t input, uint32_t actor)
{
    uint32_t state = rrj_read32(m, actor + 852);
    uint32_t config = rrj_read32(m, 0x8005AD4Cu) +
                      12u * *(uint8_t *)rrj_at(m, state + 569, 1);
    uint32_t table = rrj_read32(m, config + 4);
    uint32_t base;
    uint32_t current;
    uint32_t count;
    uint32_t index;
    int active = 1;

    FUNCTION_MARKER(0x800C258C, "RASHCDG.BIN");
    if (*(uint8_t *)rrj_at(m, state + 572, 1) & 2u)
        return 0;
    base = rrj_read32(m, state + 564);
    current = rrj_read32(m, state + 560) +
              rrj_read32(m, rrj_read32(m, 0x8005B2F8u) + 32);
    rrj_write32(m, state + 560, current);
    if (*(uint8_t *)rrj_at(m, state + 568, 1) <
        *(uint8_t *)rrj_at(m, state + 570, 1))
        *(uint8_t *)rrj_at(m, state + 568, 1) =
            *(uint8_t *)rrj_at(m, state + 570, 1);

    count = *(uint8_t *)rrj_at(m, config + 2, 1);
    for (index = 0; active && index < count; ++index)
    {
        uint32_t entry = table + 12u * index;
        uint32_t word = rrj_read32(m, entry + 8);
        uint32_t channel = (word >> 8) & 0x3Fu;
        int32_t event_time = rrj_s32(rrj_read32(m, entry + 4) - base);
        int condition = 0;
        uint8_t marks = *(uint8_t *)rrj_at(m, state + 573, 1);
        uint32_t kind = (*(uint8_t *)rrj_at(m, entry + 8, 1)) >> 4;

        if (*(uint8_t *)rrj_at(m, state + 568, 1) < channel &&
            *(uint8_t *)rrj_at(m, state + channel + 573, 1) ==
                ((word >> 14) & 0x3Fu) &&
            rrj_s32(current) >=
                rrj_s32(rrj_read32(m, entry) - base))
            condition = rrj_s32(current) < event_time;

        if (kind == 0)
        {
            if (condition && sub_800C2070(m, input, word & 15u))
                batch6_activate_event(m, state, entry, word, index);
            else if (!(marks & (1u << (index & 31u))) &&
                     (*(uint8_t *)rrj_at(m, state + 568, 1) >= channel ||
                      event_time < rrj_s32(current)))
                batch6_mark_event(m, state, index);
        }
        else if (kind == 1 || kind == 2)
        {
            if (condition && sub_800C213C(m, input, word & 15u))
                batch6_activate_event(m, state, entry, word, index);
            else if (condition && sub_800C2100(m, input, word & 15u))
                *(uint8_t *)rrj_at(m, state + channel + 574, 1) =
                    (uint8_t)(word >> 26);
            else if (!(marks & (1u << (index & 31u))) &&
                     (*(uint8_t *)rrj_at(m, state + 568, 1) >= channel ||
                      event_time < rrj_s32(current)))
                batch6_mark_event(m, state, index);
        }
        else if (kind == 3)
        {
            if (!(marks & (1u << (index & 31u))))
            {
                if (!sub_800C2100(m, input, word & 15u))
                    batch6_activate_event(m, state, entry, word, index);
                active = 0;
            }
        }
        else if (kind == 4 && !(marks & (1u << (index & 31u))) &&
                 *(uint8_t *)rrj_at(m, state + 568, 1) < channel &&
                 *(uint8_t *)rrj_at(m, state + channel + 573, 1) ==
                     ((word >> 14) & 0x3Fu))
        {
            batch6_activate_event(m, state, entry, word, index);
            base = rrj_read32(m, state + 564);
        }
    }
    return 0;
}

uint32_t sub_80017DA0(RRJMemory *m, uint32_t x, uint32_t z,
                      uint32_t event, uint32_t flags)
{
    uint32_t descriptor = rrj_read32(m, event + 4);
    uint32_t sound;
    uint32_t kind;
    int triggered = 0;

    FUNCTION_MARKER(0x80017DA0, "SLUS_010.53");
    if ((descriptor & 0x01000000u) ||
        ((descriptor & 0x04000000u) && (flags & 0x80u)) ||
        ((descriptor & 0x02000000u) && !(flags & 0x80u)) ||
        ((descriptor & 0x40000000u) && ((flags >> 4) & 3u) != 2u))
        triggered = 1;
    else if ((descriptor & 0x20000000u) &&
             ((flags >> 4) & 3u) == 0 && !(flags & 0x40u))
    {
        uint32_t sample = sub_80043F00(m, 0xF2000002u) & 0xFFu;

        if ((9u * sample) >> 8 == 0)
            triggered = 1;
    }

    if (!triggered)
        return rrj_read32(m, event + 8);

    sound = rrj_u16(rrj_at(m, event + 4, 2));
    kind = descriptor & 0xFF000000u;
    if (kind == 0x14000000u)
        sound = (flags & 15u) + 61u;
    else if (kind == 0x09000000u)
    {
        uint32_t sample = sub_80043F00(m, 0xF2000002u) & 0xFFu;

        sound += (((descriptor >> 16) & 0xFFu) * sample) >> 8;
    }
    else if (kind == 0x40000000u)
    {
        uint32_t side = (sub_80043F00(m, 0xF2000002u) & 0xFFu) >> 7;

        sound = side + ((((flags >> 4) & 3u) == 1u) ? 92u : 90u);
    }

    (void)sub_80017BA0(m, (int32_t)x, (int32_t)z, sound, 0);
    return rrj_read32(m, event + 8);
}

static uint32_t batch6_animation_id(RRJMemory *m, uint32_t animation)
{
    if (animation < 224)
        return (rrj_read32(m, 0x800541D4u + 8u * animation) >> 4) &
               0xFFFu;
    return rrj_u16(rrj_at(
               m, 0x800D5F40u + 2u * (animation - 224u), 2)) &
           0xFFFu;
}

uint32_t sub_800CAB5C(RRJMemory *m, uint32_t actor, RRJRaceLeafCall call)
{
    uint32_t type = *(uint8_t *)rrj_at(m, actor + 553, 1);
    uint32_t variant = *(uint8_t *)rrj_at(m, actor + 554, 1);
    uint32_t animation = rrj_u16(rrj_at(m, 0x800D5730u + 6u * type, 2));
    uint32_t config =
        0x800CCC20u + 16u * *(uint8_t *)rrj_at(m, actor + 566, 1);
    uint32_t speed = rrj_read32(m, config + 12);
    uint32_t object = rrj_read32(m, actor + 540);
    uint32_t flags;

    FUNCTION_MARKER(0x800CAB5C, "RASHCDG.BIN");
    (void)sub_80012858(m, object, batch6_animation_set(m, animation));
    if (type)
    {
        if (type == 2)
        {
            animation = rrj_u16(rrj_at(m, config + 4, 2));
            (void)sub_8001B44C(m, rrj_read32(m, actor + 184),
                               rrj_read32(m, actor + 192), actor, 0);
        }
        else if (type == 1)
            animation = rrj_u16(rrj_at(m, config + 6, 2));
        (void)sub_8005BF6C(m, object,
                           batch6_animation_id(m, animation), variant,
                           speed, 0);
        *(uint8_t *)rrj_at(m, actor + 565, 1) |= 1u;
    }
    else
    {
        animation = 231;
        if (rrj_read32(m, actor + 388) & 1u)
            *(uint8_t *)rrj_at(m, actor + 565, 1) |= 1u;
        else
        {
            *(uint8_t *)rrj_at(m, actor + 565, 1) &= 0xFEu;
            animation = rrj_u16(rrj_at(m, config + 2, 2));
            rrj_write32(m, actor + 480, rrj_read32(m, config + 8));
        }
        (void)sub_80012858(m, object,
                           batch6_animation_set(m, animation));
        (void)sub_8005C0B0(m, object,
                           batch6_animation_id(m, animation), variant,
                           speed, 0);
    }
    rrj_put16(rrj_at(m, actor + 544, 2), (uint16_t)animation);
    flags = rrj_read32(m, actor + 552) & 0xFFFFFFFEu;
    rrj_write32(m, actor + 552, flags);
    return flags;
}
