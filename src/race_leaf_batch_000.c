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
    uint32_t input = 0x800D7128 + 192 * rrj_read32(0x8005AD90);
    uint32_t selection = rrj_read32(0x8005B558);

    FUNCTION_MARKER(0x8001DA7C, "SLUS_010.53");
    if (r_s8(input + 122) > 0 && !rrj_read32(0x8005B478))
        return 7;
    if (r_s8(input + 42))
    {
        uint16_t value = (uint16_t)(rrj_u16(rrj_at(selection + 2, 2)) - 1);
        rrj_put16(rrj_at(selection + 2, 2), value);
        if ((int16_t)value < 0)
            rrj_put16(rrj_at(selection + 2, 2), (uint16_t)(rrj_u16(rrj_at(selection, 2)) - 1));
        return 5;
    }
    if (r_s8(input + 50))
    {
        uint16_t value = (uint16_t)(rrj_u16(rrj_at(selection + 2, 2)) + 1);
        rrj_put16(rrj_at(selection + 2, 2), value);
        if ((int16_t)value >= (int16_t)rrj_u16(rrj_at(selection, 2)))
            rrj_put16(rrj_at(selection + 2, 2), 0);
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
    uint32_t input = 0x800D7128 + 192 * rrj_read32(0x8005AD90);
    uint32_t selection = rrj_read32(0x8005B558);

    FUNCTION_MARKER(0x8001DB90, "SLUS_010.53");
    if (r_s8(input + 122) > 0)
        return 7;
    if (r_s8(input + 26))
    {
        uint16_t value = (uint16_t)(rrj_u16(rrj_at(selection + 2, 2)) - 1);
        rrj_put16(rrj_at(selection + 2, 2), value);
        if ((int16_t)value < 0)
            rrj_put16(rrj_at(selection + 2, 2), (uint16_t)(rrj_u16(rrj_at(selection, 2)) - 1));
        return 5;
    }
    if (r_s8(input + 34))
    {
        uint16_t value = (uint16_t)(rrj_u16(rrj_at(selection + 2, 2)) + 1);
        rrj_put16(rrj_at(selection + 2, 2), value);
        if ((int16_t)value >= (int16_t)rrj_u16(rrj_at(selection, 2)))
            rrj_put16(rrj_at(selection + 2, 2), 0);
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
    return sub_8002D0D8(font, rrj_read32(rrj_read32(0x8005B544) + 4 * string_id));
}

uint32_t sub_8003F9D8(uint32_t actor)
{
    uint32_t actor_id;
    uint32_t index;

    FUNCTION_MARKER(0x8003F9D8, "SLUS_010.53");
    if (!actor)
        return 0;
    actor_id = rrj_u16(rrj_at(actor + 172, 2));
    for (index = 0; index < 18; ++index)
        if (rrj_read32(0x800D5DA8 + 16 * index) == actor_id)
            return 1;
    return 0;
}

uint32_t sub_8003F680(uint32_t actor_id, uint32_t flag)
{
    uint32_t actor = rrj_read32(0x8005B3A0) + 1096 * actor_id;
    uint32_t descriptor = rrj_read32(actor + 1084);
    uint32_t rank = r_u8(descriptor + 39);

    FUNCTION_MARKER(0x8003F680, "SLUS_010.53");
    if (rank - 1 < 18)
    {
        uint32_t record = 0x800D5D98 + 16 * rank;
        rrj_write32(record, rrj_u16(rrj_at(actor + 172, 2)));
        rrj_write32(record + 4, rank);
        rrj_write32(record + 8, flag);
        rrj_write32(record + 12, rrj_read32(descriptor + 40));
        return rrj_read32(descriptor + 40);
    }
    return 0x800D0000;
}

uint32_t sub_8003F8D8(void)
{
    uint32_t table = 0x800CE4D0;
    uint32_t actor = rrj_read32(table);
    uint32_t stride = rrj_read32(table + 4);
    int32_t remaining = rrj_s32(rrj_read32(rrj_read32(table + 12)));
    uint32_t best = 0;
    uint32_t best_progress = 0;

    FUNCTION_MARKER(0x8003F8D8, "SLUS_010.53");
    while (remaining >= 0)
    {
        uint32_t descriptor = rrj_read32(actor + 1084);
        uint32_t actor_id = rrj_u16(rrj_at(actor + 172, 2));
        uint32_t mode = r_u8(descriptor + 1) & 15;
        uint32_t progress = rrj_read32(actor + 324);

        if ((actor_id < rrj_read32(rrj_read32(0x8005B2F8) + 48) || mode != 2) && r_u8(descriptor + 39) < 248 && !sub_8003F9D8(actor) && (!best_progress || rrj_s32(progress) < rrj_s32(best_progress)))
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
    uint32_t elapsed = rrj_read32(rrj_read32(0x8005B2F8) + 16);

    FUNCTION_MARKER(0x8003F708, "SLUS_010.53");
    for (slot = rank; slot < 19; ++slot, ++rank)
    {
        uint32_t record = 0x800D5D98 + 16 * rank;

        if (rrj_read32(record) == UINT32_MAX)
            break;
        elapsed = rrj_read32(record + 12);
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
        descriptor = rrj_read32(actor + 1084);
        w_u8(descriptor + 39, (uint8_t)rank);
        numerator = 300 * (rrj_s32(rrj_read32(actor + 324)) >> 4);
        denominator = (58982 * (rrj_s32(rrj_read32(rrj_read32(actor + 556) + 224)) >> 8)) >> 16;
        delta = numerator / denominator;
        rrj_write32(descriptor + 40, delta < 0 ? elapsed : elapsed + (uint32_t)delta);
        ++rank;
        ++slot;
        (void)sub_8003F680(rrj_u16(rrj_at(actor + 172, 2)), 0);
    }
    for (slot = 0; slot < 18; ++slot)
    {
        uint32_t record = 0x800D5DA8 + 16 * slot;

        if (rrj_read32(record) != UINT32_MAX)
        {
            uint32_t actor = rrj_read32(0x8005B3A0) + 1096 * rrj_u16(rrj_at(record, 2));
            uint32_t descriptor = rrj_read32(actor + 1084);

            w_u8(descriptor + 39, r_u8(record + 4));
            rrj_write32(descriptor + 40, rrj_read32(record + 12));
        }
    }
    return 0;
}

static void reset_pause_state(RRJMemory *m, uint8_t game_mode)
{
    uint32_t state = rrj_read32(0x8005B2F8);

    w_u8(0x8005AEB9, 0);
    w_u8(0x8005AEB8, 0);
    rrj_write32(0x8005AD90, 0);
    rrj_put16(rrj_at(state + 42, 2), 0);
    w_u8(state, game_mode);
}

uint32_t sub_8002DC1C(void)
{
    uint32_t result = sub_8001DA7C();
    uint16_t selection = rrj_u16(rrj_at(0x800540DA, 2));

    FUNCTION_MARKER(0x8002DC1C, "SLUS_010.53");
    if (result == 4)
    {
        if (selection == 1)
        {
            w_u8(0x8005AEB9, 1);
            (void)sub_80017BA0(0, 0, 60, 0);
            rrj_put16(rrj_at(0x800540DE, 2), 0);
        }
        else if (selection == 2)
        {
            w_u8(0x8005AEB9, 2);
            (void)sub_80017BA0(0, 0, 60, 0);
            rrj_put16(rrj_at(0x800540E2, 2), 0);
        }
        else
        {
            reset_pause_state(rrj_host_context(), 1);
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
        reset_pause_state(rrj_host_context(), 1);
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
        int16_t selection = (int16_t)rrj_u16(rrj_at(rrj_read32(0x8005B558) + 2, 2));

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

static uint32_t link_pause_packet(RRJMemory *m, uint32_t ordering_slot, uint32_t packet)
{
    uint32_t ordering = rrj_read32(ordering_slot);

    rrj_write32(packet, (rrj_read32(packet) & 0xFF000000) | (ordering & 0x00FFFFFF));
    ordering = (ordering & 0xFF000000) | (packet & 0x00FFFFFF);
    rrj_write32(ordering_slot, ordering);
    return ordering;
}

static uint32_t pause_quad(RRJMemory *m, uint32_t packet, uint32_t ordering_slot, uint32_t red, uint32_t green, uint32_t blue, int32_t x0, int32_t y0, int32_t x1, int32_t y1)
{
    w_u8(packet + 3, 5);
    w_u8(packet + 4, (uint8_t)red);
    w_u8(packet + 5, (uint8_t)green);
    w_u8(packet + 6, (uint8_t)blue);
    w_u8(packet + 7, 40);
    rrj_put16(rrj_at(packet + 8, 2), (uint16_t)x0);
    rrj_put16(rrj_at(packet + 10, 2), (uint16_t)y0);
    rrj_put16(rrj_at(packet + 12, 2), (uint16_t)x1);
    rrj_put16(rrj_at(packet + 14, 2), (uint16_t)y0);
    rrj_put16(rrj_at(packet + 16, 2), (uint16_t)x0);
    rrj_put16(rrj_at(packet + 18, 2), (uint16_t)y1);
    rrj_put16(rrj_at(packet + 20, 2), (uint16_t)x1);
    rrj_put16(rrj_at(packet + 22, 2), (uint16_t)y1);
    (void)sub_8004CDE4(packet, 1);
    return link_pause_packet(m, ordering_slot, packet);
}

uint32_t sub_800400F4(uint32_t red, uint32_t green, uint32_t blue, uint32_t ordering_slot, int32_t left, int32_t top, int32_t right, int32_t bottom)
{
    uint32_t context = rrj_read32(0x8005B470);
    uint32_t packet = rrj_read32(context + 268);
    uint32_t result;

    FUNCTION_MARKER(0x800400F4, "SLUS_010.53");
    if (packet + 96 >= rrj_read32(0x8005B4D0))
        packet = sub_80021C98(packet, 96);
    rrj_write32(rrj_read32(0x8005B470) + 268, packet + 96);
    result = pause_quad(rrj_host_context(), packet, ordering_slot, red, green, blue, left - 4, top - 3, right + 4, top);
    result = pause_quad(rrj_host_context(), packet + 24, ordering_slot, red, green, blue, left - 4, bottom + 3, right + 4, bottom);
    result = pause_quad(rrj_host_context(), packet + 48, ordering_slot, red, green, blue, left - 4, top, left, bottom);
    result = pause_quad(rrj_host_context(), packet + 72, ordering_slot, red, green, blue, right, top, right + 4, bottom);
    return result;
}

uint32_t sub_8002D9E8(uint32_t string_id)
{
    uint32_t font = rrj_read32(0x8005AD2C);
    uint32_t packets = rrj_read32(0x8005B5AC);
    uint32_t link = packets + 12;
    int32_t x;
    int16_t selection;

    FUNCTION_MARKER(0x8002D9E8, "SLUS_010.53");
    rrj_write32(0x800540FC, rrj_read32(packets + 16) | 0x05000000);
    rrj_write32(packets + 16, 0x800540FC & 0x00FFFFFF);
    sub_8002CD78(font, string_id, 96, 155, link, 0x0014506E);
    x = (int16_t)(sub_8002D0A8(font, string_id) + 102);
    selection = (int16_t)rrj_u16(rrj_at(rrj_read32(0x8005B558) + 2, 2));
    if (selection != 0 && selection != 1)
        return 0x002B0000;
    sub_8002CD78(font, 9, (uint32_t)x, 155, link, selection ? 0x002B2B6F : 0x00505A50);
    x = (int16_t)(x + 4 + sub_8002D0A8(font, 9));
    (void)sub_8002CDC8(font, 0x8005AEBC, (uint32_t)x, 155, link, 0x00505A50);
    sub_8002CD78(font, 10, (uint32_t)(int16_t)(x + 4), 155, link, selection ? 0x00505A50 : 0x00348A32);
    return 0;
}

static uint32_t pause_option(RRJMemory *m, uint32_t id, uint32_t rect, uint32_t color, uint32_t mode)
{
    return sub_8002CB08(rrj_read32(0x8005AD2C), id, rect, rrj_read32(0x8005B5AC) + 20, color, mode, rrj_blink_text);
}

uint32_t sub_8002D718(void)
{
    uint32_t packets = rrj_read32(0x8005B5AC);
    uint32_t rect = 0x1F800390;
    int16_t selection = (int16_t)rrj_u16(rrj_at(0x800540DA, 2));
    uint32_t result;

    FUNCTION_MARKER(0x8002D718, "SLUS_010.53");
    rrj_write32(0x800540E4, rrj_read32(packets + 24) | 0x05000000);
    rrj_write32(packets + 24, 0x800540E4 & 0x00FFFFFF);
    rrj_put16(rrj_at(rect, 2), 86);
    rrj_put16(rrj_at(rect + 2, 2), 72);
    rrj_put16(rrj_at(rect + 4, 2), 212);
    rrj_put16(rrj_at(rect + 6, 2), 110);
    result = pause_option(rrj_host_context(), 0, rect, 0x0014506E, 2);
    rrj_put16(rrj_at(rect + 2, 2), 94);
    if (selection == 0)
    {
        result = pause_option(rrj_host_context(), 2, rect, 0x00348A32, 2);
        rrj_put16(rrj_at(rect + 2, 2), 109);
        result = pause_option(rrj_host_context(), 3, rect, 0x00505A50, 2);
        rrj_put16(rrj_at(rect + 2, 2), 124);
        result = pause_option(rrj_host_context(), 4, rect, 0x00505A50, 2);
    }
    else if (selection == 1)
    {
        result = pause_option(rrj_host_context(), 2, rect, 0x00505A50, 2);
        rrj_put16(rrj_at(rect + 2, 2), 109);
        result = pause_option(rrj_host_context(), 3, rect, 0x002B2B6F, 2);
        rrj_put16(rrj_at(rect + 2, 2), 124);
        result = pause_option(rrj_host_context(), 4, rect, 0x00505A50, 2);
    }
    else if (selection == 2)
    {
        result = pause_option(rrj_host_context(), 2, rect, 0x00505A50, 2);
        rrj_put16(rrj_at(rect + 2, 2), 109);
        result = pause_option(rrj_host_context(), 3, rect, 0x00505A50, 2);
        rrj_put16(rrj_at(rect + 2, 2), 124);
        result = pause_option(rrj_host_context(), 4, rect, 0x0014506E, 2);
    }
    return result;
}

static void set_finish_rank(uint32_t descriptor)
{
    uint32_t rank = UINT8_MAX;
    if (rrj_read32(descriptor + 40))
    {
        rank = r_u8(descriptor + 39);
        if (rrj_s32(rrj_read32(0x8005B1FC)) < (int32_t)rank)
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
        rrj_put16(rrj_at(0x800540DA, 2), 0);
        (void)sub_80017BA0(0, 0, 107, 0);
        state = rrj_read32(0x8005B2F8);
        (void)sub_80020E30(r_s8(state) == 3);
        w_u8(0x8005AEB8, 1);
        return 1;
    }
    (void)sub_800400F4(0, 0, 128, rrj_read32(0x8005B5AC) + 24, 86, 65, 298, 175);
    menu_state = r_s8(0x8005AEB9);
    if (menu_state == 0)
    {
        rrj_write32(0x8005B558, 0x800540D8);
        result = sub_8002DC1C();
    }
    else if (menu_state == 1)
    {
        rrj_write32(0x8005B558, 0x800540DC);
        result = sub_8002DD80();
        if (result == 1)
        {
            uint32_t player;
            uint8_t flags;
            result = 3;
            state = rrj_read32(0x8005B2F8);
            player = rrj_read32(0x8005B3A0);
            w_u8(0x8005AEB9, 0);
            w_u8(0x8005AEB8, 0);
            rrj_write32(0x8005AD90, 0);
            w_u8(0x8005AF4C, 1);
            flags = r_u8(state + 4);
            rrj_put16(rrj_at(state + 42, 2), 0);
            if (flags & 0x10)
            {
                set_finish_rank(rrj_read32(player + 1084));
                set_finish_rank(rrj_read32(player + 2180));
            }
            else if ((flags & 4) || (flags & 0x22))
                w_u8(rrj_read32(player + 1084) + 39, UINT8_MAX);
            (void)sub_8003F708();
            state = rrj_read32(0x8005B2F8);
            w_u8(state, 6);
            rrj_write32(0x8005AF58, 0x00101010);
            rrj_write32(0x8005AF5C, 0x00101010);
        }
        else if (!result)
            w_u8(0x8005AEB9, 0);
        (void)sub_8002D9E8(5);
    }
    else if (menu_state == 2)
    {
        rrj_write32(0x8005B558, 0x800540E0);
        result = sub_8002DD80();
        if (result == 1)
        {
            result = 3;
            state = rrj_read32(0x8005B2F8);
            w_u8(0x8005AEB9, 0);
            w_u8(0x8005AEB8, 0);
            rrj_write32(0x8005AD90, 0);
            rrj_put16(rrj_at(state + 42, 2), 0);
            w_u8(state, 5);
        }
        else if (!result)
            w_u8(0x8005AEB9, 0);
        (void)sub_8002D9E8(7);
    }
    (void)sub_8002D718();
    state = rrj_read32(0x8005B2F8);
    if (r_s8(state) == 4)
    {
        if (rrj_s32(rrj_read32(state + 12) - rrj_read32(state + 44)) < 3001)
            sub_8002CD78(rrj_read32(0x8005AD2C), 14, 75, 120, rrj_read32(0x8005B5AC) + 8, 0x002020AA);
        else
        {
            sub_8002CD78(rrj_read32(0x8005AD2C), 11, 65, 120, rrj_read32(0x8005B5AC) + 8, 0x002020AA);
            sub_8002CD78(rrj_read32(0x8005AD2C), 12, 75, 140, rrj_read32(0x8005B5AC) + 8, 0x002020AA);
        }
    }
    state = rrj_read32(0x8005B2F8);
    if (rrj_u16(rrj_at(state + 42, 2)))
        sub_8002CD78(rrj_read32(0x8005AD2C), 13, 100, 160, rrj_read32(0x8005B5AC) + 8, 0x00348A32);
    state = rrj_read32(0x8005B2F8);
    if (r_u8(state + 4) & 0x10)
        (void)sub_8001C304((int16_t)rrj_u16(rrj_at(0x80055F7C, 2)), (int16_t)rrj_u16(rrj_at(0x80055F7E, 2)), 384, 240, rrj_read32(0x8005B5AC) + 24);
    if (result != 3)
        w_u8(0x8005AEB8, 1);
    return 1;
}

uint32_t sub_80086B1C(uint32_t actor, int32_t delta, int32_t target)
{
    uint32_t table = 0x800CD7B8 + 56 * rrj_read32(actor + 540);
    int32_t force;
    int32_t velocity;
    int32_t absolute;
    uint32_t flags;

    FUNCTION_MARKER(0x80086B1C, "RASHCDG.BIN");
    force = -(int32_t)sub_8001FC90(rrj_s32(rrj_read32(table + 48)), target) - (int32_t)sub_8001FC90(rrj_s32(rrj_read32(actor + 720)), rrj_s32(rrj_read32(table + 44)));
    rrj_write32(actor + 708, (uint32_t)force);
    velocity = rrj_s32(rrj_read32(actor + 720)) + (int32_t)sub_8001FC90(force, delta);
    rrj_write32(actor + 720, (uint32_t)velocity);
    rrj_write32(actor + 732, rrj_read32(actor + 732) + (uint32_t)sub_8001FC90(velocity, delta));
    absolute = force < 0 ? -force : force;
    flags = rrj_read32(actor + 548);
    flags = absolute < 656 ? flags | 0x00800000 : flags & 0xFF7FFFFF;
    rrj_write32(actor + 548, flags);
    return flags;
}

uint32_t sub_8002FA28(int32_t phase, uint32_t first, uint32_t second, uint32_t third, uint32_t fourth)
{
    int32_t square;
    int32_t c3;
    int32_t c2;
    int32_t c1;

    FUNCTION_MARKER(0x8002FA28, "SLUS_010.53");
    square = (int32_t)sub_8001FC90(phase, phase);
    c3 = (int32_t)sub_8001FC90(square, phase) - square;
    c2 = c3 - square;
    c1 = -c2 - c3;
    rrj_write32(fourth, (uint32_t)c3);
    rrj_write32(third, (uint32_t)c2);
    rrj_write32(second, (uint32_t)c1);
    rrj_write32(first, 0x10000u - (uint32_t)c1);
    c2 += phase;
    rrj_write32(third, (uint32_t)c2);
    return (uint32_t)c2;
}

uint32_t sub_8008676C(uint32_t actor, int32_t delta)
{
    uint32_t flags = rrj_read32(actor + 548);
    int32_t elapsed;
    int32_t duration;
    int32_t phase;
    int32_t position_delta[3];
    int32_t secondary_delta[3];
    uint32_t coefficients = 0x1F800200;
    unsigned i;

    FUNCTION_MARKER(0x8008676C, "RASHCDG.BIN");
    if (flags & 0x10)
    {
        for (i = 0; i < 3; ++i)
        {
            rrj_write32(actor + 648 + 4 * i, rrj_read32(actor + 624 + 4 * i));
            rrj_write32(actor + 684 + 4 * i, rrj_read32(actor + 660 + 4 * i));
        }
        if ((flags & 0x40) && rrj_s32(rrj_read32(actor + 632) ^ rrj_read32(actor + 644)) < 0)
        {
            rrj_write32(actor + 656, 0u - rrj_read32(actor + 632));
            rrj_write32(actor + 648, rrj_read32(actor + 624) + (rrj_s32(rrj_read32(actor + 624)) <= 0 ? 205887u : 0xFFFCDBC1u));
        }
        rrj_write32(actor + 696, 0);
        flags &= ~0x10u;
        rrj_write32(actor + 548, flags);
    }
    elapsed = rrj_s32(rrj_read32(actor + 696)) + delta;
    duration = rrj_s32(rrj_read32(actor + 1116));
    rrj_write32(actor + 696, (uint32_t)elapsed);
    if (elapsed >= duration)
    {
        for (i = 0; i < 3; ++i)
        {
            rrj_write32(actor + 624 + 4 * i, rrj_read32(actor + 636 + 4 * i));
            rrj_write32(actor + 660 + 4 * i, rrj_read32(actor + 672 + 4 * i));
        }
        rrj_write32(actor + 548, flags & ~8u);
        flags = rrj_read32(actor + 552) & 0xFFFFFC7Fu;
        rrj_write32(actor + 552, flags);
        return flags;
    }
    if (elapsed <= 0)
        phase = duration > 0 ? -(int32_t)sub_80010028(0u - (uint32_t)elapsed, (uint32_t)duration) : (int32_t)sub_80010028(0u - (uint32_t)elapsed, 0u - (uint32_t)duration);
    else
        phase = duration > 0 ? (int32_t)sub_80010028((uint32_t)elapsed, (uint32_t)duration) : -(int32_t)sub_80010028((uint32_t)elapsed, 0u - (uint32_t)duration);
    (void)sub_8002FA28(phase, coefficients, coefficients + 4, coefficients + 8, coefficients + 12);
    for (i = 0; i < 3; ++i)
    {
        position_delta[i] = rrj_s32(rrj_read32(actor + 636 + 4 * i) - rrj_read32(actor + 648 + 4 * i));
        secondary_delta[i] = rrj_s32(rrj_read32(actor + 672 + 4 * i) - rrj_read32(actor + 684 + 4 * i));
    }
    if (flags & 0x40)
    {
        uint32_t options = rrj_read32(actor + 552);

        if ((options & 0x100) && position_delta[0] < 0)
            position_delta[0] += 411774;
        else if ((options & 0x200) && position_delta[0] > 0)
            position_delta[0] -= 411774;
        else if (!(options & 0x300))
        {
            if (position_delta[0] > 205887)
                position_delta[0] -= 411774;
            else if (position_delta[0] < -205887)
                position_delta[0] += 411774;
        }
    }
    for (i = 0; i < 3; ++i)
    {
        rrj_write32(actor + 624 + 4 * i, rrj_read32(actor + 636 + 4 * i) - (uint32_t)sub_8001FC90(position_delta[i], rrj_s32(rrj_read32(coefficients))));
        rrj_write32(actor + 660 + 4 * i, rrj_read32(actor + 672 + 4 * i) - (uint32_t)sub_8001FC90(secondary_delta[i], rrj_s32(rrj_read32(coefficients))));
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
        int64_t left = (int64_t)first_weight * rrj_s32(rrj_u32((const uint8_t *)first + 4u * axis));
        int64_t right = (int64_t)second_weight * rrj_s32(rrj_u32((const uint8_t *)second + 4u * axis));

        result = (uint32_t)((uint64_t)left >> 16) + (uint32_t)((uint64_t)right >> 16);
        rrj_put32((uint8_t *)output + 4u * axis, result);
    }
    return result;
}


uint32_t sub_80086C00(uint32_t value_address, int32_t target, int32_t delta, int32_t smoothing)
{
    int32_t current = rrj_s32(rrj_read32(value_address));
    int32_t difference = target - current;
    int32_t adjustment = difference < -2048 ? -4096 : 0;
    int32_t rate;
    int32_t candidate;
    uint32_t correction = 0;

    FUNCTION_MARKER(0x80086C00, "RASHCDG.BIN");
    if (difference >= 2049)
        adjustment += 4096;
    current += adjustment;
    rrj_write32(value_address, (uint32_t)current);
    difference = target - current;
    if (smoothing)
        rate = (int32_t)sub_8001FC90((int32_t)sub_8001FC90(smoothing, rrj_s32((uint32_t)difference << 16)), delta) >> 16;
    else
        rate = 0;
    if ((rate < 0 ? -rate : rate) < 3)
        candidate = current + (difference > 0 ? 2 : difference < 0 ? -2 : 0);
    else
        candidate = current + rate;
    if ((candidate < target && current >= target) || (target >= current && target < candidate))
        correction = (uint32_t)(target - candidate);
    rrj_write32(value_address, (uint32_t)candidate + correction);
    return correction;
}

uint32_t sub_80086D54(uint32_t value_address, int32_t target, int32_t delta, uint32_t velocity_address, int32_t spring, int32_t damping)
{
    int32_t current = rrj_s32(rrj_read32(value_address));
    int32_t difference = target - current;
    int32_t adjustment = difference < -2048 ? -4096 : 0;
    int32_t velocity;
    int32_t result;

    FUNCTION_MARKER(0x80086D54, "RASHCDG.BIN");
    if (difference >= 2049)
        adjustment += 4096;
    current += adjustment;
    rrj_write32(value_address, (uint32_t)current);
    difference = target - current;
    velocity = rrj_s32(rrj_read32(velocity_address));
    velocity += (int32_t)sub_8001FC90(spring, rrj_s32((uint32_t)difference << 16)) - (int32_t)sub_8001FC90(damping, velocity);
    rrj_write32(velocity_address, (uint32_t)velocity);
    result = (int32_t)sub_8001FC90(velocity, delta) >> 16;
    rrj_write32(value_address, rrj_read32(value_address) + (uint32_t)result);
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
    int32_t time_step = rrj_s32(rrj_read32(0x800CCE38));

    FUNCTION_MARKER(0x800A451C, "RASHCDG.BIN");
    if (mode)
    {
        uint32_t squared;
        unsigned axis;

        for (axis = 0; axis < 3; ++axis)
            rrj_write32(vector + 4 * axis, rrj_read32(actor + 184 + 4 * axis) - rrj_read32(actor + 468 + 4 * axis));
        squared = sub_8002F0F4(vector);
        if (rrj_s32(squared) > 0x3FFEFFFF)
            return 1;
        magnitude = rrj_s32(sub_8004CF74(squared) << 2);
        rrj_write32(actor + 480, (uint32_t)batch_signed_ratio(magnitude, time_step));
        if (magnitude < 132)
            return 1;
        return sub_8002EED8(0x80000000u / ((uint32_t)magnitude >> 1), vector, actor + 450);
    }
    if (rrj_read32(actor + 548) & 0x40000)
        return 0x40000;
    {
        uint32_t state = rrj_read32(actor + 540);
        if (state - 13u < 2u)
            return 1;
    }
    {
        uint32_t owner = rrj_read32(actor + 568);
        int32_t velocity = rrj_s32(rrj_read32(actor + 632));
        magnitude = velocity < 0 ? (int32_t)(0u - (uint32_t)velocity) : velocity;
        if (owner && !(rrj_u16(rrj_at(owner + 172, 2)) >> 5))
        {
            uint32_t body = rrj_read32(owner + 852);
            magnitude += rrj_read32(body + 604) >= 3 ? -32768 : -98304;
        }
        rrj_put16(rrj_at(actor + 450, 2), (uint16_t)-(int16_t)rrj_u16(rrj_at(actor + 600, 2)));
        rrj_put16(rrj_at(actor + 452, 2), (uint16_t)-(int16_t)rrj_u16(rrj_at(actor + 602, 2)));
        rrj_put16(rrj_at(actor + 454, 2), (uint16_t)-(int16_t)rrj_u16(rrj_at(actor + 604, 2)));
        rrj_write32(actor + 480, (uint32_t)batch_signed_ratio(magnitude, time_step));
        return sub_8002EAD8(rrj_at(actor + 184, 12), rrj_at(actor + 450, 6), 0u - (uint32_t)magnitude, rrj_at(actor + 468, 12));
    }
}

uint32_t sub_800A421C(uint32_t actor, RRJRaceLeafCall call)
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
    rrj_write32(actor + 548, rrj_read32(actor + 548) & 0xFFFFF1FFu);
    (void)sub_8008BA18(actor);
    (void)sub_800A451C(actor, 1);
    for (axis = 0; axis < 3; ++axis)
    {
        saved_position[axis] = rrj_read32(actor + 468 + 4 * axis);
        saved_normal[axis] = rrj_u16(rrj_at(actor + 450 + 2 * axis, 2));
    }
    saved_speed = rrj_read32(actor + 480);
    (void)sub_800A451C(actor, 0);
    {
        int32_t count = rrj_s32(rrj_read32(0x800CCF90));
        int32_t index;

        for (index = 0; index < count; ++index)
        {
            uint32_t entry = r_u8(0x800CCF88 + (uint32_t)index);
            uint32_t record = rrj_read32(0x800CD6C4) + 280 * (entry & 31u);
            const uint32_t args[8] = {actor, record, 0, 0, 0, 0, 0, 0};
            if (call(rrj_host_context(), 0x800B2E64, args))
            {
                (void)sub_800A451C(actor, 1);
                break;
            }
        }
    }
    collision_result = sub_800B3AD0(actor, 0, call);
    for (axis = 0; axis < 3; ++axis)
    {
        rrj_write32(actor + 468 + 4 * axis, saved_position[axis]);
        rrj_put16(rrj_at(actor + 450 + 2 * axis, 2), saved_normal[axis]);
    }
    rrj_write32(actor + 480, saved_speed);
    if (rrj_read32(actor + 388) & 1)
    {
        position = contact_vector;
        normal = contact_normal;
        surface_result = rrj_s32(sub_800A7BF8(actor, 0, position, normal, rrj_read32(actor + 536)));
        rrj_write32(actor + 536, (uint32_t)surface_result);
    }
    else
    {
        surface_result = 0;
        w_u8(actor + 534, rrj_read32(actor + 372) ? r_u8(actor + 394) : 1);
        rrj_write32(actor + 536, 0);
    }
    if (surface_result <= 0)
    {
        uint32_t source = rrj_read32(actor + 340);
        position = source + 20;
        normal = source + 8;
        if (surface_result < 0)
        {
            position = actor + 504;
            if (!rrj_read32(actor + 480))
            {
                rrj_write32(actor + 480, 117188);
                (void)sub_8002EE50(117188, rrj_at(actor + 450, 6), rrj_at(actor + 456, 12));
            }
            normal = actor + 522;
        }
    }
    if ((int16_t)rrj_u16(rrj_at(normal + 2, 2)) >= -614)
    {
        position = actor + 504;
        normal = actor + 522;
    }
    else
    {
        for (axis = 0; axis < 3; ++axis)
            rrj_put16(rrj_at(actor + 522 + 2 * axis, 2), rrj_u16(rrj_at(normal + 2 * axis, 2)));
        for (axis = 0; axis < 3; ++axis)
            rrj_write32(actor + 504 + 4 * axis, rrj_read32(position + 4 * axis));
    }
    if (sub_800B6F40(actor + 196, normal, position, distance, 0) >= 8)
        return collision_result;
    (void)sub_8002EAD8(rrj_at(actor + 184, 12), rrj_at(normal, 6), rrj_read32(distance), rrj_at(actor + 184, 12));
    return 1;
}
