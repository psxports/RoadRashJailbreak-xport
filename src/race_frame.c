/* Resident1432-byte race frame body. Game/SDK callees remain explicit. */
#include "race_frame.h"
#include "race_pause.h"
#include "fixed_math.h"
#include "xport.h"
#include <stdlib.h>

static uint32_t b(RRJMemory *m, uint32_t a)
{
    return r_u8(a);
}

static void wb(RRJMemory *m, uint32_t a, uint32_t v)
{
    w_u8(a, (uint8_t)v);
}

static uint32_t h(RRJMemory *m, uint32_t a)
{
    return (uint32_t)(rrj_s32((uint32_t)rrj_u16(rrj_at(a, 2)) << 16) / 65536);
}

static uint32_t half(RRJMemory *m, uint32_t a)
{
    int32_t n = rrj_s32(h(m, a));
    return (uint32_t)(n >= 0 ? n / 2 : -1 - ((-1 - n) / 2));
}

static uint32_t state(RRJMemory *m)
{
    return rrj_read32(0x8005B2F8);
}

/* 80078AA8..80078B5C inside 80075EE0, not a complete function.
 * delta is the enclosing frame's stack argument at SP+0x88. */
uint32_t rrj_race_movement_block(RRJMemory *m, uint32_t delta)
{
    uint32_t count_pointer = rrj_read32(0x800CE4DC), remaining, actor;
    rrj_write32(0x1F800000, 0x1F800004);
    remaining = rrj_read32(count_pointer);
    actor = rrj_read32(0x800CE4D0);
    if (rrj_s32(remaining) >= 0)
        do
        {
            if (rrj_u16(rrj_at(actor + 0x140, 2)))
            {
                uint32_t x = rrj_read32(actor + 0xB8), y = rrj_read32(actor + 0xBC);
                uint32_t z = rrj_read32(actor + 0xC0), linked = rrj_read32(actor + 0x358);
                rrj_write32(actor + 0x1D4, x);
                rrj_write32(actor + 0x1D8, y);
                rrj_write32(actor + 0x1DC, z);
                if (linked)
                {
                    rrj_write32(linked + 0x1D4, rrj_read32(linked + 0xB8));
                    linked = rrj_read32(actor + 0x358);
                    rrj_write32(linked + 0x1D8, rrj_read32(linked + 0xBC));
                    linked = rrj_read32(actor + 0x358);
                    rrj_write32(linked + 0x1DC, rrj_read32(linked + 0xC0));
                }
                (void)sub_8007F0BC(actor, delta);
            }
            {
                uint32_t stride = rrj_read32(0x800CE4D4);
                --remaining;
                actor += stride;
            }
        } while (rrj_s32(remaining) >= 0);
    return sub_80037338();
}

static uint32_t invoke(RRJMemory *m, RRJRaceFrameCall call, uint32_t fn, uint32_t a, uint32_t bb, uint32_t c, uint32_t d, uint32_t e, uint32_t f, uint32_t g)
{
    uint32_t args[7] = {a, bb, c, d, e, f, g};
    if (!call)
        abort();
    return call(m, fn, args);
}

/* 80078AAC..80078B64; original entry requires a0=1F800000. */
uint32_t rrj_race_movement_contact_block(RRJMemory *m, uint32_t delta)
{
    (void)rrj_race_movement_block(m, delta);
    return sub_8003E150();
}

#define C0(fn) invoke(rrj_host_context(), call, fn, 0, 0, 0, 0, 0, 0, 0)

/* 80078AB0..80078B74: original requires a0=1F800000, v1=800D0000. */
uint32_t rrj_race_movement_route_block(RRJMemory *m, uint32_t delta)
{
    (void)rrj_race_movement_contact_block(m, delta);
    (void)sub_8003AE24();
    return sub_8003B520();
}

/* 80078AB4..80078BC8: entry a0=1F800000, v1=800D0000, s2=800CE4D0. */
uint32_t rrj_race_movement_surface_block(RRJMemory *m, uint32_t delta)
{
    uint32_t cursor = 0x1F800004, end, actor, position;
    (void)rrj_race_movement_route_block(m, delta);
    end = rrj_read32(0x1F800000);
    actor = rrj_read32(cursor);
    while (cursor < end)
    {
        position = actor + ((rrj_read32(actor + 0x238) & 0x600u) ? 0xB8u : 0x1F8u);
        cursor += 4; /* jal delay slot precedes the surface call. */
        (void)sub_8007504C(actor, position);
        end = rrj_read32(0x1F800000);
        actor = rrj_read32(cursor);
    }
    return 0; /* v0 is the final unsigned cursor/end comparison. */
}

#define C1(fn, a) invoke(rrj_host_context(), call, fn, a, 0, 0, 0, 0, 0, 0)

/* 80078AB8..80078C10; entry also supplies v0=*800CE4DC. */
uint32_t rrj_race_movement_placement_block(RRJMemory *m, uint32_t delta)
{
    uint32_t cursor = 0x1F800004, end, actor;
    (void)rrj_race_movement_surface_block(m, delta);
    end = rrj_read32(0x1F800000);
    actor = rrj_read32(cursor);
    while (cursor < end)
    {
        cursor += 4;
        (void)sub_8007FA4C(actor);
        end = rrj_read32(0x1F800000);
        actor = rrj_read32(cursor);
    }
    return 0;
}

#define C2(fn, a, bb) invoke(rrj_host_context(), call, fn, a, bb, 0, 0, 0, 0, 0)

/* 80078ABC..80078C58; includes the independent active-object wheel pass. */
uint32_t rrj_race_movement_wheel_block(RRJMemory *m, uint32_t delta)
{
    uint32_t result, remaining, actor;
    (void)rrj_race_movement_placement_block(m, delta);
    result = rrj_read32(0x800CE4DC);
    remaining = rrj_read32(result);
    actor = rrj_read32(0x800CE4D0);
    if (rrj_s32(remaining) >= 0)
        do
        {
            if (rrj_u16(rrj_at(actor + 320, 2)))
                (void)sub_800807F0(actor, delta);
            result = rrj_read32(0x800CE4D4);
            --remaining;
            actor += result;
        } while (rrj_s32(remaining) >= 0);
    return result; /* Empty table: count pointer; otherwise final live stride. */
}

#define C4(fn, a, bb, c, d) invoke(rrj_host_context(), call, fn, a, bb, c, d, 0, 0, 0)

/* 80078C58..80078C68: both passes reload the live actor table independently.
 * The caller's s4 is reset in the first jal delay slot; it is the subsequent
 * player-loop index, not an argument or a RAM write in this fragment. */
uint32_t rrj_race_activity_block(RRJMemory *m, RRJReverbCall reverb)
{
    (void)sub_80093E6C(reverb);
    return sub_800950E8(reverb);
}

/* 80078AC0..80078C68, including all intervening movement/geometry passes. */
uint32_t rrj_race_movement_activity_block(RRJMemory *m, uint32_t delta, RRJReverbCall reverb)
{
    (void)rrj_race_movement_wheel_block(m, delta);
    return rrj_race_activity_block(m, reverb);
}

uint32_t sub_80011C4C(RRJRaceFrameCall call)
{
    FUNCTION_MARKER(0x80011C4Cu, "SLUS_010.53");
    uint32_t s, count, index = 0, player, child, flags, other, ot, rect, reply;
    (void)C0(0x8002305C);
    s = state(rrj_host_context());
    if (rrj_u16(rrj_at(s + 40, 2)))
    {
        if (rrj_read32(0x8005B220))
        {
            wb(rrj_host_context(), s, 2);
            rrj_put16(rrj_at(state(rrj_host_context()) + 40, 2), 0);
        }
        else if (b(rrj_host_context(), s) != 4)
        {
            wb(rrj_host_context(), s + 1, b(rrj_host_context(), s));
            wb(rrj_host_context(), state(rrj_host_context()), 4);
            s = state(rrj_host_context());
            reply = rrj_read32(s + 12);
            rrj_put16(rrj_at(s + 42, 2), 0);
            rrj_write32(s + 44, reply);
            (void)C1(0x80018C1C, 1);
            (void)C1(0x80020E30, 1);
        }
    }
    else if (b(rrj_host_context(), s) == 4)
    {
        rrj_put16(rrj_at(s + 42, 2), 1);
        if (b(rrj_host_context(), s + 1) == 1)
        {
            wb(rrj_host_context(), s, 3);
            (void)C1(0x80018C1C, 1);
            (void)C1(0x80020E30, 1);
        }
        else
        {
            wb(rrj_host_context(), s, b(rrj_host_context(), s + 1));
            (void)C1(0x80018C1C, b(rrj_host_context(), state(rrj_host_context()) + 1) == 3);
            (void)C1(0x80020E30, b(rrj_host_context(), state(rrj_host_context()) + 1) == 3);
        }
    }
    if (b(rrj_host_context(), state(rrj_host_context())) == 1)
    {
        (void)C0(0x80012524);
        (void)C0(0x8008CFDC);
    }
    (void)C1(0x80018E54, 0x800CE170);
    if (b(rrj_host_context(), state(rrj_host_context())) == 1 || rrj_s32(rrj_read32(0x8005B230)) > 0)
    {
        reply = C0(0x80043E24);
        rrj_write32(state(rrj_host_context()) + 36, reply);
        (void)C1(0x80043DC4, 0x1F8003E4);
        (void)C1(0x8005E1D8, 0x800CE170);
        (void)C1(0x80043DC4, rrj_read32(state(rrj_host_context()) + 36));
        (void)C0(0x8001264C);
    }
    (void)C0(0x800C89A0);
    count = rrj_read32(state(rrj_host_context()) + 48);
    if (count)
        do
        {
            if (count == 2)
            {
                uint32_t v = rrj_read32(0x8005B474), a = v + 8 * index, bb = v + 4 * index;
                uint32_t x = h(rrj_host_context(), a) + half(rrj_host_context(), a + 4) + h(rrj_host_context(), bb + 16), y = h(rrj_host_context(), a + 2) + half(rrj_host_context(), a + 6) + h(rrj_host_context(), bb + 18);
                (void)C2(0x8004D184, x, y);
                if (b(rrj_host_context(), state(rrj_host_context()) + 4) != 24)
                {
                    player = rrj_read32(0x8005B268 + 4 * index);
                    flags = rrj_read32(player + 36);
                    if (flags & 2048)
                    {
                        child = rrj_read32(player + 852);
                        rrj_write32(player + 36, flags ^ 2048);
                        rrj_write32(child + 36, rrj_read32(child + 36) ^ 2048);
                        other = rrj_read32(0x8005B268 + 4 * (index ^ 1));
                        flags = rrj_read32(other + 36);
                        child = rrj_read32(other + 852);
                        rrj_write32(other + 36, flags ^ 2048);
                        rrj_write32(child + 36, rrj_read32(child + 36) ^ 2048);
                    }
                }
            }
            (void)C1(0x8002F2E8, index);
            (void)C0(0x800674C8);
            player = rrj_read32(0x8005B268 + 4 * index);
            if (rrj_u16(rrj_at(player + 172, 2)) < rrj_read32(state(rrj_host_context()) + 48) && b(rrj_host_context(), 0x800D5758 + 72 * index + 39) == 255)
                (void)C4(0x80027778, player, 3, 600, 0);
            (void)C1(0x8008D56C, index);
            (void)C1(0x800358C0, index);
            (void)C1(0x800C8B24, index);
            if (rrj_read32(0x8005B314))
                (void)C1(0x800A2138, index);
            (void)C1(0x80035958, index);
            (void)C1(0x800C8CD4, index);
            if (b(rrj_host_context(), 0x800D8060 + index))
                (void)C2(0x8002C928, index, rrj_read32(0x800CD660));
            count = rrj_read32(state(rrj_host_context()) + 48);
            ++index;
        } while (index < count);
    (void)C2(0x8004D184, 192, 120);
    (void)C0(0x8001E084);
    (void)C4(0x8004CE14, 0x800CD668, 1, 1, 0);
    ot = rrj_read32(0x8005B5AC);
    rrj_write32(0x800CD668, rrj_read32(ot + 44) | 0x01000000);
    rect = rrj_read32(0x800CD660);
    rrj_write32(ot + 44, 0x000CD668);
    (void)invoke(rrj_host_context(), call, 0x8001C304, h(rrj_host_context(), rect), h(rrj_host_context(), rect + 2), 384, 240, 0, 0, ot + 44);
    (void)C0(0x8005E848);
    (void)C0(0x8002CA5C);
    s = state(rrj_host_context());
    if (b(rrj_host_context(), s) == 6)
    {
        if ((b(rrj_host_context(), s + 4) & 1) && rrj_read32(0x8005AD48))
            rrj_write32(0x8005AD48, 0);
        (void)C0(0x800C5918);
    }
    else if (((b(rrj_host_context(), s) - 3) & 255) < 2)
        (void)C0(0x8002D2F4);
    (void)C1(0x80048DB4, rrj_read32(0x8005B5AC) + 44);
    (void)C0(0x8001C3F4);
    return C0(0x80018FAC);
}

/* 80078C68..80078D84: final player loop, before restoring caller registers. */
uint32_t rrj_race_player_tail(RRJMemory *m, uint32_t delta)
{
    uint32_t count = rrj_read32(state(m) + 48), index = 0, player = 0x800CD898, actor, body, driving, value;
    if (!count)
        return 0x800D0000; /* Taken branch's delay slot loads v0 high half. */
    do
    {
        actor = rrj_read32(0x8005B268 + 4 * index);
        body = rrj_read32(actor + 852);
        driving = rrj_read32(body + 604) < 3;
        if (driving)
        {
            value = (uint32_t)sub_8001FC90(rrj_s32(rrj_read32(actor + 480)), rrj_s32(delta));
            value = (value >> 8) | ((value & 0x80000000u) ? 0xff000000u : 0);
            rrj_write32(0x8005B380 + 4 * index, rrj_read32(0x8005B380 + 4 * index) + value);
        }
        body = rrj_read32(actor + 852);
        if ((rrj_read32(body + 552) & 0x80000u) && rrj_read32(player + 540) != 6)
        {
            if (rrj_s32(rrj_read32(player + 788)) > 0x30000 || (driving && rrj_s32(rrj_read32(actor + 44 + 4 * index)) >= 4801))
                (void)sub_80092C7C(actor, 10);
            else
                rrj_write32(player + 788, rrj_read32(player + 788) + delta);
        }
        player += 1132;
        count = rrj_read32(state(m) + 48);
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
uint32_t rrj_race_movement_player_block(RRJMemory *m, uint32_t delta, RRJReverbCall reverb)
{
    (void)rrj_race_movement_activity_block(m, delta, reverb);
    return rrj_race_player_tail(m, delta);
}
