#include "native_game_api.h"
#include "race_leaf_batch_000.h"
#include "fixed_math.h"
#include "font.h"
#include "packet.h"
#include "menu_item.h"
#include "menu_hint.h"
#include "text_id.h"
#include "race_pause.h"
#include "race_pause_frontier.h"
#include "xport.h"

uint32_t sub_8001DA7C(void)
{
    uint32_t input = 0x800D7128 + 192 * r_u32(0x8005AD90);
    uint32_t selection = r_u32(0x8005B558);

    FUNCTION_MARKER(0x8001DA7C, "SLUS_010.53");
    if (r_s8(input + 122) > 0 && !r_u32(0x8005B478))
        return 7;
    if (r_s8(input + 42))
    {
        uint16_t value = (uint16_t)(r_u16(selection + 2) - 1);
        w_u16(selection + 2, value);
        if ((int16_t)value < 0)
            w_u16(selection + 2, (uint16_t)(r_u16(selection) - 1));
        return 5;
    }
    if (r_s8(input + 50))
    {
        uint16_t value = (uint16_t)(r_u16(selection + 2) + 1);
        w_u16(selection + 2, value);
        if ((int16_t)value >= (int16_t)r_u16(selection))
            w_u16(selection + 2, 0);
        return 5;
    }
    if (r_s8(input + 82) > 0)
        return 4;
    if (r_s8(input + 74) > 0)
        return 6;
    return 2;
}

uint32_t sub_8001DB90(void)
{
    uint32_t input = 0x800D7128 + 192 * r_u32(0x8005AD90);
    uint32_t selection = r_u32(0x8005B558);

    FUNCTION_MARKER(0x8001DB90, "SLUS_010.53");
    if (r_s8(input + 122) > 0)
        return 7;
    if (r_s8(input + 26))
    {
        uint16_t value = (uint16_t)(r_u16(selection + 2) - 1);
        w_u16(selection + 2, value);
        if ((int16_t)value < 0)
            w_u16(selection + 2, (uint16_t)(r_u16(selection) - 1));
        return 5;
    }
    if (r_s8(input + 34))
    {
        uint16_t value = (uint16_t)(r_u16(selection + 2) + 1);
        w_u16(selection + 2, value);
        if ((int16_t)value >= (int16_t)r_u16(selection))
            w_u16(selection + 2, 0);
        return 5;
    }
    if (r_s8(input + 82) > 0)
        return 4;
    if (r_s8(input + 74) > 0)
        return 6;
    return 2;
}

uint32_t sub_8002D0A8(uint32_t font, uint32_t string_id)
{
    FUNCTION_MARKER(0x8002D0A8, "SLUS_010.53");
    return sub_8002D0D8(font, xport_guest_cptr(r_u32(r_u32(0x8005B544) + 4 * string_id), 1));
}

uint32_t sub_8003F9D8(uint32_t actor)
{
    uint32_t actor_id;
    uint32_t index;

    FUNCTION_MARKER(0x8003F9D8, "SLUS_010.53");
    if (!actor)
        return 0;
    actor_id = r_u16(actor + 172);
    for (index = 0; index < 18; ++index)
        if (r_u32(0x800D5DA8 + 16 * index) == actor_id)
            return 1;
    return 0;
}

uint32_t sub_8003F680(uint32_t actor_id, uint32_t flag)
{
    uint32_t actor = r_u32(0x8005B3A0) + 1096 * actor_id;
    uint32_t descriptor = r_u32(actor + 1084);
    uint32_t rank = r_u8(descriptor + 39);

    FUNCTION_MARKER(0x8003F680, "SLUS_010.53");
    if (rank - 1 < 18)
    {
        uint32_t record = 0x800D5D98 + 16 * rank;
        w_u32(record, r_u16(actor + 172));
        w_u32(record + 4, rank);
        w_u32(record + 8, flag);
        w_u32(record + 12, r_u32(descriptor + 40));
        return r_u32(descriptor + 40);
    }
    return 0x800D0000;
}

uint32_t sub_8003F8D8(void)
{
    uint32_t table = 0x800CE4D0;
    uint32_t actor = r_u32(table);
    uint32_t stride = r_u32(table + 4);
    int32_t remaining = ((int32_t)(uint32_t)(r_u32(r_u32(table + 12))));
    uint32_t best = 0;
    uint32_t best_progress = 0;

    FUNCTION_MARKER(0x8003F8D8, "SLUS_010.53");
    while (remaining >= 0)
    {
        uint32_t descriptor = r_u32(actor + 1084);
        uint32_t actor_id = r_u16(actor + 172);
        uint32_t mode = r_u8(descriptor + 1) & 15;
        uint32_t progress = r_u32(actor + 324);

        if ((actor_id < r_u32(r_u32(0x8005B2F8) + 48) || mode != 2) && r_u8(descriptor + 39) < 248 && !sub_8003F9D8(actor) && (!best_progress || ((int32_t)(uint32_t)(progress)) < ((int32_t)(uint32_t)(best_progress))))
        {
            best_progress = progress;
            best = actor;
        }
        --remaining;
        actor += stride;
    }
    return best;
}

uint32_t sub_8003F708(void)
{
    uint32_t rank = 1;
    uint32_t slot;
    uint32_t elapsed = r_u32(r_u32(0x8005B2F8) + 16);

    FUNCTION_MARKER(0x8003F708, "SLUS_010.53");
    for (slot = rank; slot < 19; ++slot, ++rank)
    {
        uint32_t record = 0x800D5D98 + 16 * rank;

        if (r_u32(record) == UINT32_MAX)
            break;
        elapsed = r_u32(record + 12);
    }
    while (slot < 19)
    {
        uint32_t actor = sub_8003F8D8();
        uint32_t descriptor;
        int32_t numerator;
        int32_t denominator;
        int32_t delta;

        if (!actor)
            break;
        descriptor = r_u32(actor + 1084);
        w_u8(descriptor + 39, (uint8_t)rank);
        numerator = 300 * (((int32_t)(uint32_t)(r_u32(actor + 324))) >> 4);
        denominator = (58982 * (((int32_t)(uint32_t)(r_u32(r_u32(actor + 556) + 224))) >> 8)) >> 16;
        delta = numerator / denominator;
        w_u32(descriptor + 40, delta < 0 ? elapsed : elapsed + (uint32_t)delta);
        ++rank;
        ++slot;
        (void)sub_8003F680(r_u16(actor + 172), 0);
    }
    for (slot = 0; slot < 18; ++slot)
    {
        uint32_t record = 0x800D5DA8 + 16 * slot;

        if (r_u32(record) != UINT32_MAX)
        {
            uint32_t actor = r_u32(0x8005B3A0) + 1096 * r_u16(record);
            uint32_t descriptor = r_u32(actor + 1084);

            w_u8(descriptor + 39, r_u8(record + 4));
            w_u32(descriptor + 40, r_u32(record + 12));
        }
    }
    return 0;
}

static void reset_pause_state(uint8_t game_mode)
{
    uint32_t state = r_u32(0x8005B2F8);

    w_u8(0x8005AEB9, 0);
    w_u8(0x8005AEB8, 0);
    w_u32(0x8005AD90, 0);
    w_u16(state + 42, 0);
    w_u8(state, game_mode);
}

uint32_t sub_8002DC1C(void)
{
    uint32_t result = sub_8001DA7C();
    uint16_t selection = r_u16(0x800540DA);

    FUNCTION_MARKER(0x8002DC1C, "SLUS_010.53");
    if (result == 4)
    {
        if (selection == 1)
        {
            w_u8(0x8005AEB9, 1);
            (void)sub_80017BA0(0, 0, 60, 0);
            w_u16(0x800540DE, 0);
        }
        else if (selection == 2)
        {
            w_u8(0x8005AEB9, 2);
            (void)sub_80017BA0(0, 0, 60, 0);
            w_u16(0x800540E2, 0);
        }
        else
        {
            reset_pause_state(1);
            (void)sub_80017BA0(0, 0, 107, 0);
            (void)sub_80018C1C(0);
            (void)sub_80020E30(0);
            return 3;
        }
    }
    else if (result == 5)
        (void)sub_80017BA0(0, 0, 107, 0);
    else if (result == 6 || result == 7)
    {
        reset_pause_state(1);
        (void)sub_80017BA0(0, 0, 107, 0);
        (void)sub_80018C1C(0);
        (void)sub_80020E30(0);
        return 3;
    }
    return result;
}

uint32_t sub_8002DD80(void)
{
    uint32_t result = sub_8001DB90();
    uint32_t sound;

    FUNCTION_MARKER(0x8002DD80, "SLUS_010.53");
    if (result == 4)
    {
        int16_t selection = (int16_t)r_u16(r_u32(0x8005B558) + 2);

        if (selection == 0)
        {
            result = 0;
            sound = 73;
        }
        else if (selection == 1)
        {
            result = 1;
            sound = 66;
        }
        else
            return result;
    }
    else if (result == 5)
        sound = 107;
    else if (result == 6)
    {
        w_u8(0x8005AEB9, 0);
        sound = 73;
    }
    else
        return result;
    (void)sub_80017BA0(0, 0, sound, 0);
    return result;
}

uint32_t sub_8004CDE4(uint32_t packet, uint32_t enabled)
{
    uint8_t value = r_u8(packet + 7);

    FUNCTION_MARKER(0x8004CDE4, "SLUS_010.53");
    value = enabled ? (uint8_t)(value | 2) : (uint8_t)(value & 0xFD);
    w_u8(packet + 7, value);
    return value;
}

static uint32_t link_pause_packet(uint32_t ordering_slot, uint32_t packet)
{
    uint32_t ordering = r_u32(ordering_slot);

    w_u32(packet, (r_u32(packet) & 0xFF000000) | (ordering & 0x00FFFFFF));
    ordering = (ordering & 0xFF000000) | (packet & 0x00FFFFFF);
    w_u32(ordering_slot, ordering);
    return ordering;
}

static uint32_t pause_quad(uint32_t packet, uint32_t ordering_slot, uint32_t red, uint32_t green, uint32_t blue, int32_t x0, int32_t y0, int32_t x1, int32_t y1)
{
    w_u8(packet + 3, 5);
    w_u8(packet + 4, (uint8_t)red);
    w_u8(packet + 5, (uint8_t)green);
    w_u8(packet + 6, (uint8_t)blue);
    w_u8(packet + 7, 40);
    w_u16(packet + 8, (uint16_t)x0);
    w_u16(packet + 10, (uint16_t)y0);
    w_u16(packet + 12, (uint16_t)x1);
    w_u16(packet + 14, (uint16_t)y0);
    w_u16(packet + 16, (uint16_t)x0);
    w_u16(packet + 18, (uint16_t)y1);
    w_u16(packet + 20, (uint16_t)x1);
    w_u16(packet + 22, (uint16_t)y1);
    (void)sub_8004CDE4(packet, 1);
    return link_pause_packet(ordering_slot, packet);
}

uint32_t sub_800400F4(uint32_t red, uint32_t green, uint32_t blue, uint32_t ordering_slot, int32_t left, int32_t top, int32_t right, int32_t bottom)
{
    uint32_t context = r_u32(0x8005B470);
    uint32_t packet = r_u32(context + 268);
    uint32_t result;

    FUNCTION_MARKER(0x800400F4, "SLUS_010.53");
    if (packet + 96 >= r_u32(0x8005B4D0))
        packet = sub_80021C98(packet, 96);
    w_u32(r_u32(0x8005B470) + 268, packet + 96);
    result = pause_quad(packet, ordering_slot, red, green, blue, left - 4, top - 3, right + 4, top);
    result = pause_quad(packet + 24, ordering_slot, red, green, blue, left - 4, bottom + 3, right + 4, bottom);
    result = pause_quad(packet + 48, ordering_slot, red, green, blue, left - 4, top, left, bottom);
    result = pause_quad(packet + 72, ordering_slot, red, green, blue, right, top, right + 4, bottom);
    return result;
}

uint32_t sub_8002D9E8(uint32_t string_id)
{
    uint32_t font = r_u32(0x8005AD2C);
    uint32_t packets = r_u32(0x8005B5AC);
    uint32_t link = packets + 12;
    int32_t x;
    int16_t selection;

    FUNCTION_MARKER(0x8002D9E8, "SLUS_010.53");
    w_u32(0x800540FC, r_u32(packets + 16) | 0x05000000);
    w_u32(packets + 16, 0x800540FC & 0x00FFFFFF);
    sub_8002CD78(font, string_id, 96, 155, link, 0x0014506E);
    x = (int16_t)(sub_8002D0A8(font, string_id) + 102);
    selection = (int16_t)r_u16(r_u32(0x8005B558) + 2);
    if (selection != 0 && selection != 1)
        return 0x002B0000;
    sub_8002CD78(font, 9, (uint32_t)x, 155, link, selection ? 0x002B2B6F : 0x00505A50);
    x = (int16_t)(x + 4 + sub_8002D0A8(font, 9));
    (void)sub_8002CDC8(font, xport_guest_cptr(0x8005AEBC, 1), (uint32_t)x, 155, link, 0x00505A50);
    sub_8002CD78(font, 10, (uint32_t)(int16_t)(x + 4), 155, link, selection ? 0x00505A50 : 0x00348A32);
    return 0;
}

uint32_t sub_8002D718(void)
{
    uint32_t packets = r_u32(0x8005B5AC);
    uint32_t rect = 0x1F800390;
    int16_t selection = (int16_t)r_u16(0x800540DA);
    uint32_t result;

    FUNCTION_MARKER(0x8002D718, "SLUS_010.53");
    w_u32(0x800540E4, r_u32(packets + 24) | 0x05000000);
    w_u32(packets + 24, 0x800540E4 & 0x00FFFFFF);
    w_u16(rect, 86);
    w_u16(rect + 2, 72);
    w_u16(rect + 4, 212);
    w_u16(rect + 6, 110);
    result = ((uint32_t)(sub_8002CB08(r_u32(0x8005AD2C), ((uint32_t)(0)), xport_guest_cptr(((uint32_t)(rect)), 8), r_u32(0x8005B5AC) + 20, ((uint32_t)(0x0014506E)), ((uint32_t)(2)))));
    w_u16(rect + 2, 94);
    if (selection == 0)
    {
        result = ((uint32_t)(sub_8002CB08(r_u32(0x8005AD2C), ((uint32_t)(2)), xport_guest_cptr(((uint32_t)(rect)), 8), r_u32(0x8005B5AC) + 20, ((uint32_t)(0x00348A32)), ((uint32_t)(2)))));
        w_u16(rect + 2, 109);
        result = ((uint32_t)(sub_8002CB08(r_u32(0x8005AD2C), ((uint32_t)(3)), xport_guest_cptr(((uint32_t)(rect)), 8), r_u32(0x8005B5AC) + 20, ((uint32_t)(0x00505A50)), ((uint32_t)(2)))));
        w_u16(rect + 2, 124);
        result = ((uint32_t)(sub_8002CB08(r_u32(0x8005AD2C), ((uint32_t)(4)), xport_guest_cptr(((uint32_t)(rect)), 8), r_u32(0x8005B5AC) + 20, ((uint32_t)(0x00505A50)), ((uint32_t)(2)))));
    }
    else if (selection == 1)
    {
        result = ((uint32_t)(sub_8002CB08(r_u32(0x8005AD2C), ((uint32_t)(2)), xport_guest_cptr(((uint32_t)(rect)), 8), r_u32(0x8005B5AC) + 20, ((uint32_t)(0x00505A50)), ((uint32_t)(2)))));
        w_u16(rect + 2, 109);
        result = ((uint32_t)(sub_8002CB08(r_u32(0x8005AD2C), ((uint32_t)(3)), xport_guest_cptr(((uint32_t)(rect)), 8), r_u32(0x8005B5AC) + 20, ((uint32_t)(0x002B2B6F)), ((uint32_t)(2)))));
        w_u16(rect + 2, 124);
        result = ((uint32_t)(sub_8002CB08(r_u32(0x8005AD2C), ((uint32_t)(4)), xport_guest_cptr(((uint32_t)(rect)), 8), r_u32(0x8005B5AC) + 20, ((uint32_t)(0x00505A50)), ((uint32_t)(2)))));
    }
    else if (selection == 2)
    {
        result = ((uint32_t)(sub_8002CB08(r_u32(0x8005AD2C), ((uint32_t)(2)), xport_guest_cptr(((uint32_t)(rect)), 8), r_u32(0x8005B5AC) + 20, ((uint32_t)(0x00505A50)), ((uint32_t)(2)))));
        w_u16(rect + 2, 109);
        result = ((uint32_t)(sub_8002CB08(r_u32(0x8005AD2C), ((uint32_t)(3)), xport_guest_cptr(((uint32_t)(rect)), 8), r_u32(0x8005B5AC) + 20, ((uint32_t)(0x00505A50)), ((uint32_t)(2)))));
        w_u16(rect + 2, 124);
        result = ((uint32_t)(sub_8002CB08(r_u32(0x8005AD2C), ((uint32_t)(4)), xport_guest_cptr(((uint32_t)(rect)), 8), r_u32(0x8005B5AC) + 20, ((uint32_t)(0x0014506E)), ((uint32_t)(2)))));
    }
    return result;
}

static void set_finish_rank(uint32_t descriptor)
{
    uint32_t rank = UINT8_MAX;
    if (r_u32(descriptor + 40))
    {
        rank = r_u8(descriptor + 39);
        if (((int32_t)(uint32_t)(r_u32(0x8005B1FC))) < (int32_t)rank)
            rank = UINT8_MAX;
    }
    w_u8(descriptor + 39, rank);
}

uint32_t sub_8002D2F4(void)
{
    uint32_t state;
    uint32_t result = 2;
    int8_t menu_state;

    FUNCTION_MARKER(0x8002D2F4, "SLUS_010.53");
    if (!r_u8(0x8005AEB8))
    {
        w_u8(0x8005AEB9, 0);
        w_u8(0x8005AEB8, 0);
        w_u16(0x800540DA, 0);
        (void)sub_80017BA0(0, 0, 107, 0);
        state = r_u32(0x8005B2F8);
        (void)sub_80020E30(r_s8(state) == 3);
        w_u8(0x8005AEB8, 1);
        return 1;
    }
    (void)sub_800400F4(0, 0, 128, r_u32(0x8005B5AC) + 24, 86, 65, 298, 175);
    menu_state = r_s8(0x8005AEB9);
    if (menu_state == 0)
    {
        w_u32(0x8005B558, 0x800540D8);
        result = sub_8002DC1C();
    }
    else if (menu_state == 1)
    {
        w_u32(0x8005B558, 0x800540DC);
        result = sub_8002DD80();
        if (result == 1)
        {
            uint32_t player;
            uint8_t flags;
            result = 3;
            state = r_u32(0x8005B2F8);
            player = r_u32(0x8005B3A0);
            w_u8(0x8005AEB9, 0);
            w_u8(0x8005AEB8, 0);
            w_u32(0x8005AD90, 0);
            w_u8(0x8005AF4C, 1);
            flags = r_u8(state + 4);
            w_u16(state + 42, 0);
            if (flags & 0x10)
            {
                set_finish_rank(r_u32(player + 1084));
                set_finish_rank(r_u32(player + 2180));
            }
            else if ((flags & 4) || (flags & 0x22))
                w_u8(r_u32(player + 1084) + 39, UINT8_MAX);
            (void)sub_8003F708();
            state = r_u32(0x8005B2F8);
            w_u8(state, 6);
            w_u32(0x8005AF58, 0x00101010);
            w_u32(0x8005AF5C, 0x00101010);
        }
        else if (!result)
            w_u8(0x8005AEB9, 0);
        (void)sub_8002D9E8(5);
    }
    else if (menu_state == 2)
    {
        w_u32(0x8005B558, 0x800540E0);
        result = sub_8002DD80();
        if (result == 1)
        {
            result = 3;
            state = r_u32(0x8005B2F8);
            w_u8(0x8005AEB9, 0);
            w_u8(0x8005AEB8, 0);
            w_u32(0x8005AD90, 0);
            w_u16(state + 42, 0);
            w_u8(state, 5);
        }
        else if (!result)
            w_u8(0x8005AEB9, 0);
        (void)sub_8002D9E8(7);
    }
    (void)sub_8002D718();
    state = r_u32(0x8005B2F8);
    if (r_s8(state) == 4)
    {
        if (((int32_t)(uint32_t)(r_u32(state + 12) - r_u32(state + 44))) < 3001)
            sub_8002CD78(r_u32(0x8005AD2C), 14, 75, 120, r_u32(0x8005B5AC) + 8, 0x002020AA);
        else
        {
            sub_8002CD78(r_u32(0x8005AD2C), 11, 65, 120, r_u32(0x8005B5AC) + 8, 0x002020AA);
            sub_8002CD78(r_u32(0x8005AD2C), 12, 75, 140, r_u32(0x8005B5AC) + 8, 0x002020AA);
        }
    }
    state = r_u32(0x8005B2F8);
    if (r_u16(state + 42))
        sub_8002CD78(r_u32(0x8005AD2C), 13, 100, 160, r_u32(0x8005B5AC) + 8, 0x00348A32);
    state = r_u32(0x8005B2F8);
    if (r_u8(state + 4) & 0x10)
        (void)sub_8001C304((int16_t)r_u16(0x80055F7C), (int16_t)r_u16(0x80055F7E), 384, 240, r_u32(0x8005B5AC) + 24);
    if (result != 3)
        w_u8(0x8005AEB8, 1);
    return 1;
}

uint32_t sub_80086B1C(uint32_t actor, int32_t delta, int32_t target)
{
    uint32_t table = 0x800CD7B8 + 56 * r_u32(actor + 540);
    int32_t force;
    int32_t velocity;
    int32_t absolute;
    uint32_t flags;

    FUNCTION_MARKER(0x80086B1C, "RASHCDG.BIN");
    force = -(int32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(table + 48))), target) - (int32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(actor + 720))), ((int32_t)(uint32_t)(r_u32(table + 44))));
    w_u32(actor + 708, (uint32_t)force);
    velocity = ((int32_t)(uint32_t)(r_u32(actor + 720))) + (int32_t)sub_8001FC90(force, delta);
    w_u32(actor + 720, (uint32_t)velocity);
    w_u32(actor + 732, r_u32(actor + 732) + (uint32_t)sub_8001FC90(velocity, delta));
    absolute = force < 0 ? -force : force;
    flags = r_u32(actor + 548);
    flags = absolute < 656 ? flags | 0x00800000 : flags & 0xFF7FFFFF;
    w_u32(actor + 548, flags);
    return flags;
}

uint32_t sub_8002FA28(int32_t phase, uint32_t *first, uint32_t *second, uint32_t *third, uint32_t *fourth)
{
    uint32_t square, result;
    FUNCTION_MARKER(0x8002FA28, "SLUS_010.53");
    square = (uint32_t)(sub_8001FC90(phase, phase));
    result = (uint32_t)(sub_8001FC90((int32_t)square, phase) - square);
    *fourth = result;
    result -= square;
    *third = result;
    result = 0 - result - *fourth;
    *second = result;
    *first = 0x10000 - result;
    result = *third + (uint32_t)phase;
    *third = result;
    return result;
}

uint32_t sub_8002F994(int32_t phase, uint32_t *first, uint32_t *second, uint32_t *third, uint32_t *fourth)
{
    uint32_t twice, square, weight, result;
    FUNCTION_MARKER(0x8002F994, "SLUS_010.53");
    twice = (uint32_t)phase << 1;
    square = (uint32_t)(sub_8001FC90(phase, phase));
    weight = 3 * square - twice;
    *fourth = weight;
    weight = weight - twice + 0x10000;
    *third = weight;
    result = (*fourth << 1) - twice;
    *first = result;
    result = 0 - result;
    *second = result;
    return result;
}

uint32_t sub_8008676C(uint32_t actor, int32_t delta)
{
    uint32_t flags, elapsed, duration, phase, coefficients[4];
    uint32_t start_position[3], start_secondary[3], position_delta[3], secondary_delta[3];
    uint32_t first, second, third, fourth, scaled, value, options, adjustment;
    uint32_t index;
    FUNCTION_MARKER(0x8008676C, "RASHCDG.BIN");
    flags = r_u32(actor + 548);
    if (flags & 0x10)
    {
        start_position[0] = r_u32(actor + 624);
        start_position[1] = r_u32(actor + 628);
        start_position[2] = r_u32(actor + 632);
        start_secondary[0] = r_u32(actor + 660);
        start_secondary[1] = r_u32(actor + 664);
        flags = r_u32(actor + 548);
        start_secondary[2] = r_u32(actor + 668);
        w_u32(actor + 648, start_position[0]);
        w_u32(actor + 652, start_position[1]);
        w_u32(actor + 656, start_position[2]);
        w_u32(actor + 684, start_secondary[0]);
        w_u32(actor + 688, start_secondary[1]);
        w_u32(actor + 692, start_secondary[2]);
        if ((flags & 0x40) && (int32_t)(start_position[2] ^ r_u32(actor + 644)) < 0)
        {
            w_u32(actor + 656, 0 - start_position[2]);
            value = (int32_t)start_position[0] > 0 ? 0xFFFCDBC1 : 205887;
            w_u32(actor + 648, start_position[0] + value);
        }
        flags = r_u32(actor + 548);
        w_u32(actor + 696, 0);
        w_u32(actor + 548, flags & 0xFFFFFFEF);
    }
    elapsed = r_u32(actor + 696);
    duration = r_u32(actor + 1116);
    elapsed += (uint32_t)delta;
    w_u32(actor + 696, elapsed);
    if ((int32_t)elapsed >= (int32_t)duration)
    {
        start_position[0] = r_u32(actor + 636);
        start_position[1] = r_u32(actor + 640);
        start_position[2] = r_u32(actor + 644);
        start_secondary[0] = r_u32(actor + 672);
        start_secondary[1] = r_u32(actor + 676);
        start_secondary[2] = r_u32(actor + 680);
        w_u32(actor + 624, start_position[0]);
        flags = r_u32(actor + 548);
        w_u32(actor + 628, start_position[1]);
        w_u32(actor + 632, start_position[2]);
        w_u32(actor + 660, start_secondary[0]);
        w_u32(actor + 664, start_secondary[1]);
        w_u32(actor + 668, start_secondary[2]);
        w_u32(actor + 548, flags & 0xFFFFFFF7);
        flags = r_u32(actor + 552) & 0xFFFFFC7F;
        w_u32(actor + 552, flags);
        return flags;
    }
    if ((int32_t)elapsed > 0)
        phase = (int32_t)duration > 0 ? sub_80010028(elapsed, duration) : 0 - sub_80010028(elapsed, 0 - duration);
    else
        phase = (int32_t)duration > 0 ? 0 - sub_80010028(0 - elapsed, duration) : sub_80010028(0 - elapsed, 0 - duration);
    (void)sub_8002FA28((int32_t)phase, coefficients, coefficients + 1, coefficients + 2, coefficients + 3);
    for (index = 0; index < 3; ++index)
    {
        position_delta[index] = r_u32(actor + 636 + index * 4) - r_u32(actor + 648 + index * 4);
        secondary_delta[index] = r_u32(actor + 672 + index * 4) - r_u32(actor + 684 + index * 4);
    }
    flags = r_u32(actor + 548);
    if (flags & 0x40)
    {
        options = r_u32(actor + 552);
        adjustment = 0;
        if (options & 0x100)
        {
            if ((int32_t)position_delta[0] < 0)
                adjustment = 411774;
        }
        else if (options & 0x200)
        {
            if ((int32_t)position_delta[0] > 0)
                adjustment = 0 - (uint32_t)411774;
        }
        else if ((int32_t)position_delta[0] > 205887)
            adjustment = 0 - (uint32_t)411774;
        else if ((int32_t)position_delta[0] < -205887)
            adjustment = 411774;
        position_delta[0] += adjustment;
    }
    for (index = 0; index < 3; ++index)
    {
        first = (uint32_t)(sub_8001FC90((int32_t)position_delta[index], (int32_t)coefficients[0]));
        second = (uint32_t)(sub_8001FC90(0, (int32_t)coefficients[1]));
        third = (uint32_t)(sub_8001FC90(0, (int32_t)coefficients[2]));
        fourth = (uint32_t)(sub_8001FC90(0, (int32_t)coefficients[3]));
        scaled = (uint32_t)(sub_8001FC90((int32_t)r_u32(actor + 1116), (int32_t)(third + fourth)));
        value = r_u32(actor + 636 + index * 4) - (first + second + scaled);
        w_u32(actor + 624 + index * 4, value);
        first = (uint32_t)(sub_8001FC90((int32_t)secondary_delta[index], (int32_t)coefficients[0]));
        second = (uint32_t)(sub_8001FC90(0, (int32_t)coefficients[1]));
        third = (uint32_t)(sub_8001FC90(0, (int32_t)coefficients[2]));
        fourth = (uint32_t)(sub_8001FC90(0, (int32_t)coefficients[3]));
        scaled = (uint32_t)(sub_8001FC90((int32_t)r_u32(actor + 1116), (int32_t)(third + fourth)));
        value = r_u32(actor + 672 + index * 4) - (first + second + scaled);
        w_u32(actor + 660 + index * 4, value);
    }
    return 0;
}

uint32_t sub_8002E6F8(const uint32_t first[3], const uint32_t second[3], uint32_t output[3], int32_t first_weight, int32_t second_weight)
{
    uint32_t axis;
    uint32_t result = 0;

    FUNCTION_MARKER(0x8002E6F8u, "SLUS_010.53");
    for (axis = 0; axis < 3; ++axis)
    {
        int64_t left = (int64_t)first_weight * ((int32_t)(uint32_t)(xport_load_le32((const uint8_t *)first + 4u * axis)));
        int64_t right = (int64_t)second_weight * ((int32_t)(uint32_t)(xport_load_le32((const uint8_t *)second + 4u * axis)));

        result = (uint32_t)((uint64_t)left >> 16) + (uint32_t)((uint64_t)right >> 16);
        xport_store_le32((uint8_t *)output + 4u * axis, result);
    }
    return result;
}

uint32_t sub_80086C00(uint32_t value_address, int32_t target, int32_t delta, int32_t smoothing)
{
    int32_t current = ((int32_t)(uint32_t)(r_u32(value_address)));
    int32_t difference = target - current;
    int32_t adjustment = difference < -2048 ? -4096 : 0;
    int32_t rate;
    int32_t candidate;
    uint32_t correction = 0;

    FUNCTION_MARKER(0x80086C00, "RASHCDG.BIN");
    if (difference >= 2049)
        adjustment += 4096;
    current += adjustment;
    w_u32(value_address, (uint32_t)current);
    difference = target - current;
    if (smoothing)
        rate = (int32_t)sub_8001FC90((int32_t)sub_8001FC90(smoothing, ((int32_t)(uint32_t)((uint32_t)difference << 16))), delta) >> 16;
    else
        rate = 0;
    if ((rate < 0 ? -rate : rate) < 3)
        candidate = current + (difference > 0 ? 2 : difference < 0 ? -2 : 0);
    else
        candidate = current + rate;
    if ((candidate < target && current >= target) || (target >= current && target < candidate))
        correction = (uint32_t)(target - candidate);
    w_u32(value_address, (uint32_t)candidate + correction);
    return correction;
}

uint32_t sub_80086D54(uint32_t value_address, int32_t target, int32_t delta, uint32_t velocity_address, int32_t spring, int32_t damping)
{
    int32_t current = ((int32_t)(uint32_t)(r_u32(value_address)));
    int32_t difference = target - current;
    int32_t adjustment = difference < -2048 ? -4096 : 0;
    int32_t velocity;
    int32_t result;

    FUNCTION_MARKER(0x80086D54, "RASHCDG.BIN");
    if (difference >= 2049)
        adjustment += 4096;
    current += adjustment;
    w_u32(value_address, (uint32_t)current);
    difference = target - current;
    velocity = ((int32_t)(uint32_t)(r_u32(velocity_address)));
    velocity += (int32_t)sub_8001FC90(spring, ((int32_t)(uint32_t)((uint32_t)difference << 16))) - (int32_t)sub_8001FC90(damping, velocity);
    w_u32(velocity_address, (uint32_t)velocity);
    result = (int32_t)sub_8001FC90(velocity, delta) >> 16;
    w_u32(value_address, r_u32(value_address) + (uint32_t)result);
    return (uint32_t)result;
}

static int32_t batch_signed_ratio(int32_t numerator, int32_t denominator)
{
    uint32_t numerator_abs = numerator < 0 ? 0u - (uint32_t)numerator : (uint32_t)numerator;
    uint32_t denominator_abs = denominator < 0 ? 0u - (uint32_t)denominator : (uint32_t)denominator;
    int32_t result = (int32_t)sub_80010028(numerator_abs, denominator_abs);

    return (numerator < 0) != (denominator < 0) ? -result : result;
}

uint32_t sub_800A451C(uint32_t actor, uint32_t mode)
{
    const uint32_t vector = 0x1F8003A0;
    int32_t magnitude;
    int32_t time_step = ((int32_t)(uint32_t)(r_u32(0x800CCE38)));

    FUNCTION_MARKER(0x800A451C, "RASHCDG.BIN");
    if (mode)
    {
        uint32_t squared;
        unsigned axis;

        for (axis = 0; axis < 3; ++axis)
            w_u32(vector + 4 * axis, r_u32(actor + 184 + 4 * axis) - r_u32(actor + 468 + 4 * axis));
        squared = sub_8002F0F4(xport_guest_ptr(vector, 12));
        if (((int32_t)(uint32_t)(squared)) > 0x3FFEFFFF)
            return 1;
        magnitude = ((int32_t)(uint32_t)(sub_8004CF74(squared) << 2));
        w_u32(actor + 480, (uint32_t)batch_signed_ratio(magnitude, time_step));
        if (magnitude < 132)
            return 1;
        return sub_8002EED8(0x80000000u / ((uint32_t)magnitude >> 1), xport_guest_ptr(vector, 12), xport_guest_ptr(actor + 450, 6));
    }
    if (r_u32(actor + 548) & 0x40000)
        return 0x40000;
    {
        uint32_t state = r_u32(actor + 540);
        if (state - 13u < 2u)
            return 1;
    }
    {
        uint32_t owner = r_u32(actor + 568);
        int32_t velocity = ((int32_t)(uint32_t)(r_u32(actor + 632)));
        magnitude = velocity < 0 ? (int32_t)(0u - (uint32_t)velocity) : velocity;
        if (owner && !(r_u16(owner + 172) >> 5))
        {
            uint32_t body = r_u32(owner + 852);
            magnitude += r_u32(body + 604) >= 3 ? -32768 : -98304;
        }
        w_u16(actor + 450, (uint16_t)-(int16_t)r_u16(actor + 600));
        w_u16(actor + 452, (uint16_t)-(int16_t)r_u16(actor + 602));
        w_u16(actor + 454, (uint16_t)-(int16_t)r_u16(actor + 604));
        w_u32(actor + 480, (uint32_t)batch_signed_ratio(magnitude, time_step));
        return sub_8002EAD8(xport_guest_ptr(actor + 184, 12), xport_guest_ptr(actor + 450, 6), 0u - (uint32_t)magnitude, xport_guest_ptr(actor + 468, 12));
    }
}

uint32_t sub_800A421C(uint32_t actor)
{
    const uint32_t contact_vector = 0x1F8003B0;
    const uint32_t contact_normal = 0x1F8003C0;
    const uint32_t distance = 0x1F8003D0;
    uint32_t saved_position[3];
    uint16_t saved_normal[3];
    uint32_t saved_speed;
    uint32_t position = 0;
    uint32_t normal = 0;
    uint32_t collision_result;
    int32_t surface_result;
    unsigned axis;

    FUNCTION_MARKER(0x800A421C, "RASHCDG.BIN");
    w_u32(actor + 548, r_u32(actor + 548) & 0xFFFFF1FFu);
    (void)sub_8008BA18(actor);
    (void)sub_800A451C(actor, 1);
    for (axis = 0; axis < 3; ++axis)
    {
        saved_position[axis] = r_u32(actor + 468 + 4 * axis);
        saved_normal[axis] = r_u16(actor + 450 + 2 * axis);
    }
    saved_speed = r_u32(actor + 480);
    (void)sub_800A451C(actor, 0);
    {
        int32_t count = ((int32_t)(uint32_t)(r_u32(0x800CCF90)));
        int32_t index;

        for (index = 0; index < count; ++index)
        {
            uint32_t entry = r_u8(0x800CCF88 + (uint32_t)index);
            uint32_t record = r_u32(0x800CD6C4) + 280 * (entry & 31u);
            if (sub_800B2E64(actor, record))
            {
                (void)sub_800A451C(actor, 1);
                break;
            }
        }
    }
    collision_result = sub_800B3AD0(actor, 0);
    for (axis = 0; axis < 3; ++axis)
    {
        w_u32(actor + 468 + 4 * axis, saved_position[axis]);
        w_u16(actor + 450 + 2 * axis, saved_normal[axis]);
    }
    w_u32(actor + 480, saved_speed);
    if (r_u32(actor + 388) & 1)
    {
        position = contact_vector;
        normal = contact_normal;
        surface_result = ((int32_t)(uint32_t)(sub_800A7BF8(actor, 0, (uint32_t *)xport_guest_ptr(position, 12), (uint16_t *)xport_guest_ptr(normal, 6), r_u32(actor + 536))));
        w_u32(actor + 536, (uint32_t)surface_result);
    }
    else
    {
        surface_result = 0;
        w_u8(actor + 534, r_u32(actor + 372) ? r_u8(actor + 394) : 1);
        w_u32(actor + 536, 0);
    }
    if (surface_result <= 0)
    {
        uint32_t source = r_u32(actor + 340);
        position = source + 20;
        normal = source + 8;
        if (surface_result < 0)
        {
            position = actor + 504;
            if (!r_u32(actor + 480))
            {
                w_u32(actor + 480, 117188);
                (void)sub_8002EE50(117188, xport_guest_ptr(actor + 450, 6), xport_guest_ptr(actor + 456, 12));
            }
            normal = actor + 522;
        }
    }
    if ((int16_t)r_u16(normal + 2) >= -614)
    {
        position = actor + 504;
        normal = actor + 522;
    }
    else
    {
        for (axis = 0; axis < 3; ++axis)
            w_u16(actor + 522 + 2 * axis, r_u16(normal + 2 * axis));
        for (axis = 0; axis < 3; ++axis)
            w_u32(actor + 504 + 4 * axis, r_u32(position + 4 * axis));
    }
    {
        uint32_t contact_depth;
        if (sub_800B6F40(actor + 196, xport_guest_cptr(normal, 6), position, &contact_depth, NULL) >= 8)
            return collision_result;
        w_u32(distance, contact_depth);
    }
    (void)sub_8002EAD8(xport_guest_ptr(actor + 184, 12), xport_guest_ptr(normal, 6), r_u32(distance), xport_guest_ptr(actor + 184, 12));
    return 1;
}
