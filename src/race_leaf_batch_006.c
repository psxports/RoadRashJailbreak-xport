#include "race_leaf_batch_006.h"
#include "race_pause.h"
#include "xport.h"

static uint32_t batch6_animation_set(uint32_t animation)
{
    uint32_t index;

    if (animation < 224)
        index = r_u32(0x800541D4u + 8u * animation) & 15u;
    else
        index = r_u16(0x800D5F40u + 2u * (animation - 224u)) >> 12;
    return r_u32(0x800CE190u + 4u * index);
}

uint32_t sub_8001B44C(uint32_t x, uint32_t z, uint32_t actor, uint32_t active)
{
    FUNCTION_MARKER(0x8001B44C, "SLUS_010.53");
    if (active)
    {
        uint32_t state = r_u32(r_u32(actor + 96));
        uint32_t sound = state - 400u == 30u ? 104u : 103u;

        return sub_80017BA0((int32_t)x, (int32_t)z, sound, r_u32(0x8005B404u));
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

static uint32_t batch6_send_identity(uint32_t actor, uint16_t identity)
{
    const uint32_t packet = 0x1F8003F0u;
    uint32_t saved = r_u32(packet);
    uint32_t result;

    w_u16(packet, 16);
    w_u16(packet + 2, identity);
    result = sub_800BCA68(xport_guest_ptr(packet, 8), 2, actor);
    w_u32(packet, saved);
    return result;
}

uint32_t sub_800C1DD4(uint32_t actor)
{
    int32_t scalar_value_1;
    int32_t scalar_sign_1;
    int32_t scalar_value_2;
    int32_t scalar_sign_2;

    uint32_t state = r_u32(actor + 1084);
    uint16_t identity;
    uint32_t history;

    FUNCTION_MARKER(0x800C1DD4, "RASHCDG.BIN");
    history = r_u32(state + 40);
    if (history)
        return history;
    if (r_u8(state + 39) >= 248u)
        return 16;

    if (r_u32(actor + 1088))
    {
        uint32_t relation = r_u32(actor + 852);

        if ((r_u8(relation + 572) & 0x10u) && (r_u32(relation + 552) & 0x800000u))
            identity = r_u16(r_u32(actor + 856) + 172);
        else
        {
            uint32_t actor_identity = r_u16(actor + 172);
            uint16_t cached = r_u16(0x800CCB6Cu + 2u * actor_identity);
            uint32_t candidate = 0;
            uint32_t within_time = 0;

            if (cached != 224u)
            {
                uint32_t clock = r_u32(r_u32(0x8005B2F8u) + 16);
                uint32_t timestamp = r_u32(0x800CCB70u + 4u * actor_identity);
                within_time = ((int32_t)(uint32_t)(clock - 59u)) < ((int32_t)(uint32_t)(timestamp));
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

                cached = r_u16(0x800CCB6Cu + 2u * actor_identity);
                candidate = r_u32(0x8005B3A0u) + 1096u * cached;
                actor_x = r_u32(actor + 504);
                candidate_x = r_u32(candidate + 504);
                candidate_z = r_u32(candidate + 512);
                actor_z = r_u32(actor + 512);
                x = ((scalar_value_1 = (((int32_t)(uint32_t)(actor_x - candidate_x)))), (scalar_sign_1 = scalar_value_1 >> 31), (((int32_t)(((scalar_value_1 >> 16) + scalar_sign_1) ^ scalar_sign_1))));
                z = ((scalar_value_2 = (((int32_t)(uint32_t)(actor_z - candidate_z)))), (scalar_sign_2 = scalar_value_2 >> 31), (((int32_t)(((scalar_value_2 >> 16) + scalar_sign_2) ^ scalar_sign_2))));
                major = x < z ? z : x;
                minor = x < z ? x : z;
                diagonal = minor + (minor >> 1);
                distance = major - (major >> 5) - (major >> 7) + (diagonal >> 2) + (diagonal >> 6);
                if (distance >= 2)
                    candidate = 0;
            }
            identity = candidate ? r_u16(0x800CCB6Cu + 2u * r_u16(actor + 172)) : (uint16_t)sub_8008B428(actor + 172, 1);
        }
    }
    else
    {
        uint32_t linked = r_u32(actor + 856);

        identity = (uint16_t)sub_8008B428(linked + 172, 1);
        if (r_u32(r_u32(actor + 852) + 552) & 0x800000u)
            identity = r_u16(r_u32(actor + 856) + 172);
    }

    history = actor + 948u + 8u * (uint32_t)(int32_t)r_s8(actor + 946);
    if (r_u16(history) != 16u || r_u16(history + 2) != identity)
        return batch6_send_identity(actor, identity);
    return identity;
}

uint32_t sub_800C2070(uint32_t input, uint32_t selector)
{
    uint32_t scalar_table_3;
    uint32_t scalar_index_3;
    uint32_t scalar_table_4;
    uint32_t scalar_index_4;

    uint32_t pair = 0x800CCB78u + 4u * selector;
    int32_t value = ((scalar_table_3 = r_u32(((uint32_t)(input)) + 184)), (scalar_index_3 = r_u32(scalar_table_3 + 4u * ((uint32_t)(r_u16(pair))))), (((int32_t)(((int32_t)(uint32_t)(r_u32(((uint32_t)(input)) + 8u * scalar_index_3 + 20)))))));
    uint32_t secondary;

    FUNCTION_MARKER(0x800C2070, "RASHCDG.BIN");
    if (value >= 0 || 0u - (uint32_t)value >= 75u)
        return 0;
    secondary = r_u16(pair + 2);
    if (secondary == 15u)
        return 1;
    return ((scalar_table_4 = r_u32(((uint32_t)(input)) + 184)), (scalar_index_4 = r_u32(scalar_table_4 + 4u * ((uint32_t)(secondary)))), (((int32_t)(((int32_t)(uint32_t)(r_u32(((uint32_t)(input)) + 8u * scalar_index_4 + 20))))))) > 0;
}

uint32_t sub_800C2100(uint32_t input, uint32_t selector)
{
    uint32_t scalar_table_5;
    uint32_t scalar_index_5;

    uint32_t channel = r_u16(0x800CCB78u + 4u * selector);

    FUNCTION_MARKER(0x800C2100, "RASHCDG.BIN");
    return ((scalar_table_5 = r_u32(((uint32_t)(input)) + 184)), (scalar_index_5 = r_u32(scalar_table_5 + 4u * ((uint32_t)(channel)))), (((int32_t)(((int32_t)(uint32_t)(r_u32(((uint32_t)(input)) + 8u * scalar_index_5 + 20))))))) > 0;
}

uint32_t sub_800C213C(uint32_t input, uint32_t selector)
{
    uint32_t scalar_table_6;
    uint32_t scalar_index_6;

    uint32_t channel = r_u16(0x800CCB78u + 4u * selector);

    FUNCTION_MARKER(0x800C213C, "RASHCDG.BIN");
    return (uint32_t)((scalar_table_6 = r_u32(((uint32_t)(input)) + 184)), (scalar_index_6 = r_u32(scalar_table_6 + 4u * ((uint32_t)(channel)))), (((int32_t)(((int32_t)(uint32_t)(r_u32(((uint32_t)(input)) + 8u * scalar_index_6 + 20))))))) >> 31;
}

static void batch6_activate_event(uint32_t state, uint32_t entry, uint32_t word, uint32_t index)
{
    uint32_t channel = (word >> 8) & 0x3Fu;

    w_u32(state + 560, 0);
    w_u8(state + 568, (uint8_t)channel);
    w_u32(state + 564, r_u32(entry + 4));
    w_u8(state + channel + 574, (uint8_t)((word >> 20) & 0x3Fu));
    xport_update_u8(state + 573, XPORT_MEMORY_UPDATE_OR, (uint8_t)(1u << (index & 31u)));
}

static void batch6_mark_event(uint32_t state, uint32_t index)
{
    xport_update_u8(state + 573, XPORT_MEMORY_UPDATE_OR, (uint8_t)(1u << (index & 31u)));
}

uint32_t sub_800C258C(uint32_t input, uint32_t actor)
{
    uint32_t state = r_u32(actor + 852);
    uint32_t config = r_u32(0x8005AD4Cu) + 12u * r_u8(state + 569);
    uint32_t table = r_u32(config + 4);
    uint32_t base;
    uint32_t current;
    uint32_t count;
    uint32_t index;
    int active = 1;

    FUNCTION_MARKER(0x800C258C, "RASHCDG.BIN");
    if (r_u8(state + 572) & 2u)
        return 0;
    base = r_u32(state + 564);
    current = r_u32(state + 560) + r_u32(r_u32(0x8005B2F8u) + 32);
    w_u32(state + 560, current);
    if (r_u8(state + 568) < r_u8(state + 570))
        w_u8(state + 568, r_u8(state + 570));

    count = r_u8(config + 2);
    for (index = 0; active && index < count; ++index)
    {
        uint32_t entry = table + 12u * index;
        uint32_t word = r_u32(entry + 8);
        uint32_t channel = (word >> 8) & 0x3Fu;
        int32_t event_time = ((int32_t)(uint32_t)(r_u32(entry + 4) - base));
        int condition = 0;
        uint8_t marks = r_u8(state + 573);
        uint32_t kind = (r_u8(entry + 8)) >> 4;

        if (r_u8(state + 568) < channel && r_u8(state + channel + 573) == ((word >> 14) & 0x3Fu) && ((int32_t)(uint32_t)(current)) >= ((int32_t)(uint32_t)(r_u32(entry) - base)))
            condition = ((int32_t)(uint32_t)(current)) < event_time;

        if (kind == 0)
        {
            if (condition && sub_800C2070(input, word & 15u))
                batch6_activate_event(state, entry, word, index);
            else if (!(marks & (1u << (index & 31u))) && (r_u8(state + 568) >= channel || event_time < ((int32_t)(uint32_t)(current))))
                batch6_mark_event(state, index);
        }
        else if (kind == 1 || kind == 2)
        {
            if (condition && sub_800C213C(input, word & 15u))
                batch6_activate_event(state, entry, word, index);
            else if (condition && sub_800C2100(input, word & 15u))
                w_u8(state + channel + 574, (uint8_t)(word >> 26));
            else if (!(marks & (1u << (index & 31u))) && (r_u8(state + 568) >= channel || event_time < ((int32_t)(uint32_t)(current))))
                batch6_mark_event(state, index);
        }
        else if (kind == 3)
        {
            if (!(marks & (1u << (index & 31u))))
            {
                if (!sub_800C2100(input, word & 15u))
                    batch6_activate_event(state, entry, word, index);
                active = 0;
            }
        }
        else if (kind == 4 && !(marks & (1u << (index & 31u))) && r_u8(state + 568) < channel && r_u8(state + channel + 573) == ((word >> 14) & 0x3Fu))
        {
            batch6_activate_event(state, entry, word, index);
            base = r_u32(state + 564);
        }
    }
    return 0;
}

uint32_t sub_80017DA0(uint32_t x, uint32_t z, uint32_t event, uint32_t flags)
{
    uint32_t descriptor = r_u32(event + 4);
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
        return r_u32(event + 8);

    sound = r_u16(event + 4);
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
    return r_u32(event + 8);
}

uint32_t rrj_actor_begin_animation(uint32_t actor, uint32_t type, uint32_t variant, uint32_t animation, uint32_t speed)
{
    uint32_t object, set, identity, flags;

    /* Internal branch of RASHCDG 800CAAF0 */
    set = batch6_animation_set(animation);
    object = r_u32(actor + 540);
    (void)sub_80012858(object, set);
    if (type)
    {
        if (type == 2)
        {
            uint32_t x = r_u32(actor + 184);
            uint32_t config = 0x800CCC20u + 16u * r_u8(actor + 566);
            uint32_t z = r_u32(actor + 192);
            animation = r_u16(config + 4);
            (void)sub_8001B44C(x, z, actor, 0);
        }
        else if (type == 1)
            animation = r_u16(0x800CCC20u + 16u * r_u8(actor + 566) + 6);
        identity = ((uint32_t)((((uint32_t)(animation)) < 224) ? ((r_u32(0x800541D4u + 8u * ((uint32_t)(animation))) >> 4) & 0xFFFu) : (r_u16(0x800D5F40u + 2u * (((uint32_t)(animation)) - 224u)) & 0xFFFu)));
        object = r_u32(actor + 540);
        (void)sub_8005BF6C(object, identity, variant, speed, 0);
        xport_update_u8(actor + 565, XPORT_MEMORY_UPDATE_OR, 1u);
    }
    else
    {
        animation = 231;
        if (r_u32(actor + 388) & 1u)
            xport_update_u8(actor + 565, XPORT_MEMORY_UPDATE_OR, 1u);
        else
        {
            uint32_t config, selected_speed;
            xport_update_u8(actor + 565, XPORT_MEMORY_UPDATE_AND, 0xFEu);
            config = 0x800CCC20u + 16u * r_u8(actor + 566);
            animation = r_u16(config + 2);
            selected_speed = r_u32(config + 8);
            w_u32(actor + 480, selected_speed);
        }
        set = batch6_animation_set(animation);
        object = r_u32(actor + 540);
        (void)sub_80012858(object, set);
        identity = ((uint32_t)((((uint32_t)(animation)) < 224) ? ((r_u32(0x800541D4u + 8u * ((uint32_t)(animation))) >> 4) & 0xFFFu) : (r_u16(0x800D5F40u + 2u * (((uint32_t)(animation)) - 224u)) & 0xFFFu)));
        object = r_u32(actor + 540);
        (void)sub_8005C0B0(object, identity, variant, speed, 0);
    }
    flags = r_u32(actor + 552);
    w_u16(actor + 544, (uint16_t)animation);
    flags &= 0xFFFFFFFEu;
    w_u32(actor + 552, flags);
    return flags;
}
