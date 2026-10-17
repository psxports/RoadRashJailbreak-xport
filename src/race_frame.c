#include "gpu.h"
#include "menu.h"
#include "native_game_api.h"
#include "game_main.h"
#include "race_audio_frontier.h"
#include "race_leaf_batch_000.h"
#include "race_leaf_frontier.h"
#include "race_pause_frontier.h"
#include "race_step.h"
/* Resident1432-byte race frame body. Game/SDK callees remain explicit. */
#include "race_frame.h"
#include "race_bodyless_batch_008.h"
#include "race_pause.h"
#include "fixed_math.h"
#include "xport.h"
#include <stdlib.h>

/* 80078AA8..80078B5C inside 80075EE0, not a complete function.
 * delta is the enclosing frame's stack argument at SP+0x88. */
uint32_t rrj_race_movement_block(uint32_t delta)
{
    uint32_t count_pointer = r_u32(0x800CE4DC), remaining, actor;
    w_u32(0x1F800000, 0x1F800004);
    remaining = r_u32(count_pointer);
    actor = r_u32(0x800CE4D0);
    if (((int32_t)(uint32_t)(remaining)) >= 0)
        do
        {
            if (r_u16(actor + 0x140))
            {
                uint32_t x = r_u32(actor + 0xB8), y = r_u32(actor + 0xBC);
                uint32_t z = r_u32(actor + 0xC0), linked = r_u32(actor + 0x358);
                w_u32(actor + 0x1D4, x);
                w_u32(actor + 0x1D8, y);
                w_u32(actor + 0x1DC, z);
                if (linked)
                {
                    w_u32(linked + 0x1D4, r_u32(linked + 0xB8));
                    linked = r_u32(actor + 0x358);
                    w_u32(linked + 0x1D8, r_u32(linked + 0xBC));
                    linked = r_u32(actor + 0x358);
                    w_u32(linked + 0x1DC, r_u32(linked + 0xC0));
                }
                (void)sub_8007F0BC(actor, delta);
            }
            {
                uint32_t stride = r_u32(0x800CE4D4);
                --remaining;
                actor += stride;
            }
        } while (((int32_t)(uint32_t)(remaining)) >= 0);
    return sub_80037338();
}

/* 80078AAC..80078B64; original entry requires a0=1F800000. */
uint32_t rrj_race_movement_contact_block(uint32_t delta)
{
    (void)rrj_race_movement_block(delta);
    return sub_8003E150();
}

/* 80078AB0..80078B74: original requires a0=1F800000, v1=800D0000. */
uint32_t rrj_race_movement_route_block(uint32_t delta)
{
    (void)rrj_race_movement_contact_block(delta);
    (void)sub_8003AE24();
    return sub_8003B520();
}

/* 80078AB4..80078BC8: entry a0=1F800000, v1=800D0000, s2=800CE4D0. */
uint32_t rrj_race_movement_surface_block(uint32_t delta)
{
    uint32_t cursor = 0x1F800004, end, actor, position;
    (void)rrj_race_movement_route_block(delta);
    end = r_u32(0x1F800000);
    actor = r_u32(cursor);
    while (cursor < end)
    {
        position = actor + ((r_u32(actor + 0x238) & 0x600u) ? 0xB8u : 0x1F8u);
        cursor += 4; /* jal delay slot precedes the surface call. */
        (void)sub_8007504C(actor, position);
        end = r_u32(0x1F800000);
        actor = r_u32(cursor);
    }
    return 0; /* v0 is the final unsigned cursor/end comparison. */
}

/* 80078AB8..80078C10; entry also supplies v0=*800CE4DC. */
uint32_t rrj_race_movement_placement_block(uint32_t delta)
{
    uint32_t cursor = 0x1F800004, end, actor;
    (void)rrj_race_movement_surface_block(delta);
    end = r_u32(0x1F800000);
    actor = r_u32(cursor);
    while (cursor < end)
    {
        cursor += 4;
        (void)sub_8007FA4C(actor);
        end = r_u32(0x1F800000);
        actor = r_u32(cursor);
    }
    return 0;
}

/* 80078ABC..80078C58; includes the independent active-object wheel pass. */
uint32_t rrj_race_movement_wheel_block(uint32_t delta)
{
    uint32_t result, remaining, actor;
    (void)rrj_race_movement_placement_block(delta);
    result = r_u32(0x800CE4DC);
    remaining = r_u32(result);
    actor = r_u32(0x800CE4D0);
    if (((int32_t)(uint32_t)(remaining)) >= 0)
        do
        {
            if (r_u16(actor + 320))
                (void)sub_800807F0(actor, delta);
            result = r_u32(0x800CE4D4);
            --remaining;
            actor += result;
        } while (((int32_t)(uint32_t)(remaining)) >= 0);
    return result; /* Empty table: count pointer; otherwise final live stride. */
}

/* 80078C58..80078C68: both passes reload the live actor table independently.
 * The caller's s4 is reset in the first jal delay slot; it is the subsequent
 * player-loop index, not an argument or a RAM write in this fragment. */
uint32_t rrj_race_activity_block(void)
{
    (void)sub_80093E6C();
    return sub_800950E8();
}

/* 80078AC0..80078C68, including all intervening movement/geometry passes. */
uint32_t rrj_race_movement_activity_block(uint32_t delta)
{
    (void)rrj_race_movement_wheel_block(delta);
    return rrj_race_activity_block();
}

uint32_t sub_80011C4C(void)
{
    int32_t scalar_n_21;
    int32_t scalar_n_22;

    FUNCTION_MARKER(0x80011C4Cu, "SLUS_010.53");
    uint32_t s, count, index = 0, player, child, flags, other, ot, rect, reply;
    (void)sub_8002305C();
    s = r_u32(0x8005B2F8);
    if (r_u16(s + 40))
    {
        if (r_u32(0x8005B220))
        {
            w_u8(s, (uint8_t)2);
            w_u16(r_u32(0x8005B2F8) + 40, 0);
        }
        else if ((uint32_t)r_u8(s) != 4)
        {
            w_u8((s + 1), (uint8_t)r_u8(s));
            w_u8((r_u32(0x8005B2F8)), (uint8_t)4);
            s = r_u32(0x8005B2F8);
            reply = r_u32(s + 12);
            w_u16(s + 42, 0);
            w_u32(s + 44, reply);
            (void)sub_80018C1C(1);
            (void)sub_80020E30(1);
        }
    }
    else if ((uint32_t)r_u8(s) == 4)
    {
        w_u16(s + 42, 1);
        if ((uint32_t)r_u8((s + 1)) == 1)
        {
            w_u8(s, (uint8_t)3);
            (void)sub_80018C1C(1);
            (void)sub_80020E30(1);
        }
        else
        {
            w_u8(s, (uint8_t)r_u8((s + 1)));
            (void)sub_80018C1C((uint32_t)r_u8((r_u32(0x8005B2F8) + 1)) == 3);
            (void)sub_80020E30((uint32_t)r_u8((r_u32(0x8005B2F8) + 1)) == 3);
        }
    }
    if ((uint32_t)r_u8((r_u32(0x8005B2F8))) == 1)
    {
        (void)sub_80012524();
        (void)sub_8008CFDC();
    }
    (void)sub_80018E54(0x800CE170);
    if ((uint32_t)r_u8((r_u32(0x8005B2F8))) == 1 || ((int32_t)(uint32_t)(r_u32(0x8005B230))) > 0)
    {
        reply = sub_80043E24();
        w_u32(r_u32(0x8005B2F8) + 36, reply);
        (void)sub_80043DC4(0x1F8003E4);
        (void)sub_8005E1D8(0x800CE170);
        (void)sub_80043DC4(r_u32(r_u32(0x8005B2F8) + 36));
        (void)sub_8001264C();
    }
    (void)sub_800C89A0();
    count = r_u32(r_u32(0x8005B2F8) + 48);
    if (count)
        do
        {
            if (count == 2)
            {
                uint32_t v = r_u32(0x8005B474), a = v + 8 * index, bb = v + 4 * index;
                uint32_t x = ((uint32_t)((uint32_t)(((int32_t)(uint32_t)((uint32_t)r_u16(((uint32_t)(a))) << 16)) / 65536))) + ((scalar_n_21 = ((int32_t)(uint32_t)(((uint32_t)((uint32_t)(((int32_t)(uint32_t)((uint32_t)r_u16(((uint32_t)(((uint32_t)(a + 4))))) << 16)) / 65536)))))), (((uint32_t)((uint32_t)(scalar_n_21 >= 0 ? scalar_n_21 / 2 : -1 - ((-1 - scalar_n_21) / 2)))))) + ((uint32_t)((uint32_t)(((int32_t)(uint32_t)((uint32_t)r_u16(((uint32_t)(bb + 16))) << 16)) / 65536))), y = ((uint32_t)((uint32_t)(((int32_t)(uint32_t)((uint32_t)r_u16(((uint32_t)(a + 2))) << 16)) / 65536))) + ((scalar_n_22 = ((int32_t)(uint32_t)(((uint32_t)((uint32_t)(((int32_t)(uint32_t)((uint32_t)r_u16(((uint32_t)(((uint32_t)(a + 6))))) << 16)) / 65536)))))), (((uint32_t)((uint32_t)(scalar_n_22 >= 0 ? scalar_n_22 / 2 : -1 - ((-1 - scalar_n_22) / 2)))))) + ((uint32_t)((uint32_t)(((int32_t)(uint32_t)((uint32_t)r_u16(((uint32_t)(bb + 18))) << 16)) / 65536)));
                (void)sub_8004D184(((int32_t)(uint32_t)(x)), ((int32_t)(uint32_t)(y)));
                if ((uint32_t)r_u8((r_u32(0x8005B2F8) + 4)) != 24)
                {
                    player = r_u32(0x8005B268 + 4 * index);
                    flags = r_u32(player + 36);
                    if (flags & 2048)
                    {
                        child = r_u32(player + 852);
                        w_u32(player + 36, flags ^ 2048);
                        w_u32(child + 36, r_u32(child + 36) ^ 2048);
                        other = r_u32(0x8005B268 + 4 * (index ^ 1));
                        flags = r_u32(other + 36);
                        child = r_u32(other + 852);
                        w_u32(other + 36, flags ^ 2048);
                        w_u32(child + 36, r_u32(child + 36) ^ 2048);
                    }
                }
            }
            (void)sub_8002F2E8(index);
            (void)sub_800674C8();
            player = r_u32(0x8005B268 + 4 * index);
            if (r_u16(player + 172) < r_u32(r_u32(0x8005B2F8) + 48) && (uint32_t)r_u8((0x800D5758 + 72 * index + 39)) == 255)
                (void)sub_80027778(player, 3, 600, 0);
            (void)sub_8008D56C(index);
            (void)sub_800358C0(index);
            (void)sub_800C8B24(index);
            if (r_u32(0x8005B314))
                (void)sub_800A2138(index);
            (void)sub_80035958(index);
            (void)sub_800C8CD4(index);
            if ((uint32_t)r_u8((0x800D8060 + index)))
                (void)sub_8002C928(index, xport_guest_ptr(r_u32(0x800CD660u), 4));
            count = r_u32(r_u32(0x8005B2F8) + 48);
            ++index;
        } while (index < count);
    (void)sub_8004D184(192, 120);
    (void)sub_8001E084();
    (void)SetDrawMode1(xport_guest_ptr(0x800CD668u, 8), 1, 1, 0);
    ot = r_u32(0x8005B5AC);
    w_u32(0x800CD668, r_u32(ot + 44) | 0x01000000);
    rect = r_u32(0x800CD660);
    w_u32(ot + 44, 0x000CD668);
    (void)sub_8001C304(((int32_t)(uint32_t)(((uint32_t)((uint32_t)(((int32_t)(uint32_t)((uint32_t)r_u16(((uint32_t)(rect))) << 16)) / 65536))))), ((int32_t)(uint32_t)(((uint32_t)((uint32_t)(((int32_t)(uint32_t)((uint32_t)r_u16(((uint32_t)(rect + 2))) << 16)) / 65536))))), 384, 240, ot + 44);
    (void)sub_8005E848();
    (void)sub_8002CA5C();
    s = r_u32(0x8005B2F8);
    if ((uint32_t)r_u8(s) == 6)
    {
        if (((uint32_t)r_u8((s + 4)) & 1) && r_u32(0x8005AD48))
            w_u32(0x8005AD48, 0);
        (void)sub_800C5918();
    }
    else if ((((uint32_t)r_u8(s) - 3) & 255) < 2)
        (void)sub_8002D2F4();
    if (!rrj_gpu_draw_ot(r_u32(0x8005B5AC) + 44))
        abort();
    (void)sub_8001C3F4();
    return sub_80018FAC();
}

/* Continue at 80012034 after the current player object pass */
uint32_t rrj_resume_sub_80011C4C(uint32_t index)
{
    uint32_t s, count = 0, player, child, flags, other, ot, rect;
    uint32_t resume_first = 1;

    do
    {
        if (!resume_first)
        {
            if (count == 2)
            {
                uint32_t v = r_u32(0x8005B474), a = v + 8 * index, bb = v + 4 * index;
                uint32_t x = (uint32_t)r_s16(a) + (uint32_t)((int32_t)r_s16(a + 4) >> 1) + (uint32_t)r_s16(bb + 16);
                uint32_t y = (uint32_t)r_s16(a + 2) + (uint32_t)((int32_t)r_s16(a + 6) >> 1) + (uint32_t)r_s16(bb + 18);
                (void)sub_8004D184(((int32_t)(uint32_t)(x)), ((int32_t)(uint32_t)(y)));
                if ((uint32_t)r_u8((r_u32(0x8005B2F8) + 4)) != 24)
                {
                    player = r_u32(0x8005B268 + 4 * index);
                    flags = r_u32(player + 36);
                    if (flags & 2048)
                    {
                        child = r_u32(player + 852);
                        w_u32(player + 36, flags ^ 2048);
                        w_u32(child + 36, r_u32(child + 36) ^ 2048);
                        other = r_u32(0x8005B268 + 4 * (index ^ 1));
                        flags = r_u32(other + 36);
                        child = r_u32(other + 852);
                        w_u32(other + 36, flags ^ 2048);
                        w_u32(child + 36, r_u32(child + 36) ^ 2048);
                    }
                }
            }
            (void)sub_8002F2E8(index);
            (void)sub_800674C8();
            player = r_u32(0x8005B268 + 4 * index);
            if (r_u16(player + 172) < r_u32(r_u32(0x8005B2F8) + 48) && (uint32_t)r_u8((0x800D5758 + 72 * index + 39)) == 255)
                (void)sub_80027778(player, 3, 600, 0);
            (void)sub_8008D56C(index);
            (void)sub_800358C0(index);
            (void)sub_800C8B24(index);
            if (r_u32(0x8005B314))
                (void)sub_800A2138(index);
            (void)sub_80035958(index);
        }
        resume_first = 0;
            (void)sub_800C8CD4(index);
            if ((uint32_t)r_u8((0x800D8060 + index)))
                (void)sub_8002C928(index, xport_guest_ptr(r_u32(0x800CD660u), 4));
            count = r_u32(r_u32(0x8005B2F8) + 48);
            ++index;
        } while (index < count);
    (void)sub_8004D184(192, 120);
    (void)sub_8001E084();
    (void)SetDrawMode1(xport_guest_ptr(0x800CD668u, 8), 1, 1, 0);
    ot = r_u32(0x8005B5AC);
    w_u32(0x800CD668, r_u32(ot + 44) | 0x01000000);
    rect = r_u32(0x800CD660);
    w_u32(ot + 44, 0x000CD668);
    (void)sub_8001C304(r_s16(rect), r_s16(rect + 2), 384, 240, ot + 44);
    (void)sub_8005E848();
    (void)sub_8002CA5C();
    s = r_u32(0x8005B2F8);
    if ((uint32_t)r_u8(s) == 6)
    {
        if (((uint32_t)r_u8((s + 4)) & 1) && r_u32(0x8005AD48))
            w_u32(0x8005AD48, 0);
        (void)sub_800C5918();
    }
    else if ((((uint32_t)r_u8(s) - 3) & 255) < 2)
        (void)sub_8002D2F4();
    if (!rrj_gpu_draw_ot(r_u32(0x8005B5AC) + 44))
        abort();
    (void)sub_8001C3F4();
    return sub_80018FAC();
}

/* 80078C68..80078D84: final player loop, before restoring caller registers. */
uint32_t rrj_race_player_tail(uint32_t delta)
{
    uint32_t count = r_u32(r_u32(0x8005B2F8) + 48), index = 0, player = 0x800CD898, actor, body, driving, value;
    if (!count)
        return 0x800D0000; /* Taken branch's delay slot loads v0 high half. */
    do
    {
        actor = r_u32(0x8005B268 + 4 * index);
        body = r_u32(actor + 852);
        driving = r_u32(body + 604) < 3;
        if (driving)
        {
            value = (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(actor + 480))), ((int32_t)(uint32_t)(delta)));
            value = (value >> 8) | ((value & 0x80000000u) ? 0xff000000u : 0);
            w_u32(0x8005B380 + 4 * index, r_u32(0x8005B380 + 4 * index) + value);
        }
        body = r_u32(actor + 852);
        if ((r_u32(body + 552) & 0x80000u) && r_u32(player + 540) != 6)
        {
            if (((int32_t)(uint32_t)(r_u32(player + 788))) > 0x30000 || (driving && ((int32_t)(uint32_t)(r_u32(actor + 44 + 4 * index))) >= 4801))
                (void)sub_80092C7C(actor, 10);
            else
                w_u32(player + 788, r_u32(player + 788) + delta);
        }
        player += 1132;
        count = r_u32(r_u32(0x8005B2F8) + 48);
        ++index;
    } while (index < count);
    return 0;
}

uint32_t sub_8002CA5C(void)
{
    FUNCTION_MARKER(0x8002CA5C, "SLUS_010.53");
    return 0;
}

/* Final section of 80075EE0, through the player loop before its epilogue. */
uint32_t rrj_race_movement_player_block(uint32_t delta)
{
    (void)rrj_race_movement_activity_block(delta);
    return rrj_race_player_tail(delta);
}
