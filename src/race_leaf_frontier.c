#include "native_game_api.h"
#include "race_leaf_frontier.h"
#include "fixed_math.h"
#include "race_leaf_batch_000.h"
#include "race_leaf_batch_002.h"
#include "race_pause.h"
#include "first2_camera_dependencies.h"
#include "first2_camera_surface.h"
#include <stdio.h>
#include <stdlib.h>
#include "xport.h"

static uint32_t leaf_frontier_call(RRJMemory *m, RRJRaceLeafCall call, uint32_t target, uint32_t a0)
{
    const uint32_t args[8] = {a0, 0, 0, 0, 0, 0, 0, 0};
    return call(m, target, args);
}

static uint32_t leaf_frontier_call3(RRJMemory *m, RRJRaceLeafCall call, uint32_t target, uint32_t a0, uint32_t a1, uint32_t a2)
{
    const uint32_t args[8] = {a0, a1, a2, 0, 0, 0, 0, 0};
    return call(m, target, args);
}

uint32_t sub_80087420(uint32_t actor, int32_t delta, RRJRaceLeafCall call)
{
    uint32_t owner = rrj_read32(actor + 568);
    uint32_t target = rrj_read32(owner + 852);
    uint32_t flags = rrj_read32(actor + 548);
    uint32_t table = 0x800CD7B8 + 56 * rrj_read32(actor + 540);
    uint32_t source = 0x1F800240;
    uint32_t angles = 0x1F800250;
    int blocked;
    int target_mode;
    int alternate_axis;
    int32_t phase_step;
    int32_t phase;
    int32_t desired_yaw;
    int32_t desired_pitch;

    FUNCTION_MARKER(0x80087420, "RASHCDG.BIN");
    blocked = (rrj_read32(owner + 568) & 0x400) || (rrj_read32(owner + 564) & 0x8800);
    target_mode = rrj_read32(target + 604) < 3;
    alternate_axis = (flags & 0x8000) != 0;
    if ((rrj_u16(rrj_at(owner + 172, 2)) >> 5) || blocked || (rrj_read32(owner + 568) & 0x100) || alternate_axis || (flags & 0x400000) || !target_mode)
        return leaf_frontier_call3(rrj_host_context(), call, 0x80087424, actor, (uint32_t)delta, 0);
    flags &= ~0x02000000u;
    flags &= ~0x20000000u;
    rrj_write32(actor + 548, flags);
    phase_step = (flags & 2) ? 65536000 : 0x20000;
    phase_step = -phase_step;
    if ((flags & 0x10000000) || rrj_read32(actor + 1120))
        return leaf_frontier_call3(rrj_host_context(), call, 0x80087424, actor, (uint32_t)delta, 0);
    rrj_write32(source, rrj_read32(owner + 504));
    rrj_write32(source + 4, rrj_read32(owner + 508));
    rrj_write32(source + 8, rrj_read32(owner + 512));
    phase = rrj_s32(rrj_read32(actor + 584)) + (int32_t)sub_8001FC90(phase_step, delta);
    if (phase < 0)
        phase = 0;
    if (phase > 0x10000)
        phase = 0x10000;
    rrj_write32(actor + 584, (uint32_t)phase);
    if ((flags & 0x110000) != 0x100000)
        (void)sub_8002E6F8(rrj_at(source, 12), rrj_at(target + 184, 12), rrj_at(actor + 572, 12), 0x10000 - phase, phase);
    if (rrj_read32(owner + 568) & 0x400)
        return leaf_frontier_call3(rrj_host_context(), call, 0x80087A48, actor, (uint32_t)delta, 0);
    if ((int16_t)rrj_u16(rrj_at(owner + 450, 2)) >= -408 && (int16_t)rrj_u16(rrj_at(owner + 450, 2)) <= 408 && (int16_t)rrj_u16(rrj_at(owner + 454, 2)) >= -408 && (int16_t)rrj_u16(rrj_at(owner + 454, 2)) <= 408)
    {
        desired_yaw = rrj_s32(rrj_read32(actor + 608));
        desired_pitch = rrj_s32(rrj_read32(actor + 612));
    }
    else
    {
        desired_yaw = rrj_s32(sub_80020018((uint32_t)(int32_t)r_s16(owner + 450), (uint32_t)(int32_t)r_s16(owner + 454)));
        desired_pitch = -rrj_s32(sub_8001FF3C((uint32_t)((int32_t)r_s16(owner + 452) * 16)));
    }
    if ((flags & 6) == 6)
    {
        rrj_write32(actor + 608, (uint32_t)desired_yaw);
        rrj_write32(actor + 612, (uint32_t)desired_pitch);
        rrj_write32(actor + 616, 0);
        rrj_write32(actor + 620, rrj_read32(table));
    }
    else
    {
        (void)sub_80086C00(actor + 608, desired_yaw, delta, rrj_s32(rrj_read32(table)));
        (void)sub_80086D54(actor + 612, desired_pitch, delta, actor + 616, 100007, 30015);
    }
    if (rrj_read32(target + 604) >= 2)
    {
        int32_t accumulated = rrj_s32(rrj_read32(actor + 484)) + rrj_s32(rrj_read32(actor + 608)) - desired_yaw;
        uint32_t elapsed = rrj_read32(actor + 316) + (uint32_t)delta;

        rrj_write32(actor + 484, (uint32_t)accumulated);
        rrj_write32(actor + 316, elapsed);
        if (elapsed > 0x10000)
        {
            int32_t average = accumulated < 0 ? -(int32_t)sub_80010028(0u - ((uint32_t)accumulated << 16), elapsed) : (int32_t)sub_80010028((uint32_t)accumulated << 16, elapsed);

            rrj_write32(actor + 456, (uint32_t)average);
            rrj_write32(actor + 316, 0);
            rrj_write32(actor + 484, 0);
        }
    }
    rrj_put16(rrj_at(angles, 2), (uint16_t)-(int16_t)rrj_u16(rrj_at(actor + 612, 2)));
    rrj_put16(rrj_at(angles + 2, 2), (uint16_t)-(int16_t)rrj_u16(rrj_at(actor + 608, 2)));
    rrj_put16(rrj_at(angles + 4, 2), 0);
    return sub_8004D2A4(angles, actor + 588);
}

uint32_t sub_800A13C4(int32_t delta, RRJRaceLeafCall call)
{
    const uint32_t args[8] = {(uint32_t)delta, 0, 0, 0, 0, 0, 0, 0};

    FUNCTION_MARKER(0x800A13C4, "RASHCDG.BIN");
    if (rrj_read32(0x8005B314) == 0 && rrj_read32(0x8005B2AC) == 0)
        return 0;
    return call(rrj_host_context(), 0x800A13D4, args);
}

uint32_t sub_80090814(int32_t delta, RRJRaceLeafCall call)
{
    uint32_t actor = rrj_read32(0x800CE4D0);
    uint32_t stride = rrj_read32(0x800CE4D4);
    uint32_t count_pointer = rrj_read32(0x800CE4DC);
    int32_t count = rrj_s32(rrj_read32(count_pointer));
    uint32_t result = count_pointer;

    FUNCTION_MARKER(0x80090814, "RASHCDG.BIN");
    while (count-- >= 0)
    {
        uint32_t config = rrj_read32(0x8005B2F8);
        uint32_t player = rrj_u16(rrj_at(actor + 172, 2));
        uint32_t state = rrj_read32(actor + 1084);
        if (player < rrj_read32(config + 48) || (r_u8(state + 1) & 15) != 2 || (r_u8(actor + 928) & 0x10))
        {
            if (rrj_u16(rrj_at(actor + 320, 2)))
            {
                uint32_t mode = r_u8(config + 4);
                if ((player < rrj_read32(config + 48) && player == 0 && (rrj_read32(config + 4) & 0x18) == 0x10) || (!(mode & 0x10) && (mode & 1) && player == r_u8(config + 6)))
                {
                    uint32_t timer = rrj_read32(0x8005B2FC) + (uint32_t)delta;
                    int32_t sequence = rrj_s32(rrj_read32(0x800CCA7C));
                    uint32_t index = rrj_read32(config + 60);
                    rrj_write32(0x8005B2FC, timer);
                    if (sequence < 0)
                    {
                        if (rrj_s32(timer) >= rrj_s32(rrj_read32(0x8005306C + 4 * index)))
                        {
                            uint32_t body = rrj_read32(actor + 852);
                            rrj_write32(actor + 36, rrj_read32(actor + 36) | 0x800);
                            rrj_write32(body + 36, rrj_read32(body + 36) | 0x800);
                            rrj_write32(0x800CCA7C, 1);
                            rrj_write32(0x8005B2FC, 0);
                        }
                    }
                    else if (rrj_s32(timer) >= rrj_s32(rrj_read32(0x80053060 + 4 * index)))
                    {
                        uint32_t body = rrj_read32(actor + 852);
                        uint32_t flags = rrj_read32(actor + 36);
                        uint32_t body_flags = rrj_read32(body + 36);
                        flags = (flags & ~0x800u) | (((flags >> 11) ^ 1u) << 11);
                        body_flags = (body_flags & ~0x800u) | (((body_flags >> 11) ^ 1u) << 11);
                        rrj_write32(actor + 36, flags);
                        rrj_write32(body + 36, body_flags);
                        if (flags & 0x800)
                            ++sequence;
                        if (sequence >= rrj_s32(rrj_read32(0x80053078 + 4 * index)) && !(flags & 0x800))
                            sequence = -1;
                        rrj_write32(0x800CCA7C, (uint32_t)sequence);
                        rrj_write32(0x8005B2FC, 0);
                    }
                }
                if (rrj_read32(actor + 856))
                {
                    uint32_t shadow = rrj_read32(actor + 856);
                    unsigned i;
                    for (i = 0; i < 9; ++i)
                    {
                        rrj_put16(rrj_at(shadow + 432 + 2 * i, 2), rrj_u16(rrj_at(actor + 432 + 2 * i, 2)));
                        rrj_put16(rrj_at(shadow + 516 + 2 * i, 2), rrj_u16(rrj_at(actor + 516 + 2 * i, 2)));
                    }
                    rrj_put16(rrj_at(shadow + 450, 2), rrj_u16(rrj_at(actor + 450, 2)));
                    rrj_put16(rrj_at(shadow + 452, 2), rrj_u16(rrj_at(actor + 452, 2)));
                    rrj_put16(rrj_at(shadow + 454, 2), rrj_u16(rrj_at(actor + 454, 2)));
                    rrj_write32(shadow + 568, rrj_read32(actor + 568));
                    rrj_write32(shadow + 564, rrj_read32(actor + 564));
                    rrj_write32(shadow + 480, rrj_read32(actor + 480));
                    rrj_write32(shadow + 636, rrj_read32(actor + 636));
                    rrj_write32(shadow + 652, rrj_read32(actor + 652));
                    rrj_write32(shadow + 616, rrj_read32(actor + 616));
                    rrj_write32(shadow + 620, rrj_read32(actor + 620));
                    rrj_write32(shadow + 676, rrj_read32(actor + 676));
                    rrj_put16(rrj_at(shadow + 320, 2), rrj_u16(rrj_at(actor + 320, 2)));
                }
                {
                    uint32_t body = rrj_read32(actor + 852);
                    uint32_t remaining = (r_u8(body + 572) >> 4) & 1;
                    do
                    {
                        uint32_t flags = rrj_read32(body + 552);
                        uint32_t event;
                        if (flags & 0x4000)
                        {
                            uint32_t owner = rrj_read32(body + 596);
                            rrj_write32(body + 552, flags | 0x8000);
                            rrj_write32(owner + 568, rrj_read32(owner + 568) | 8);
                        }
                        flags = rrj_read32(body + 552);
                        if (flags & 0x8000)
                        {
                            uint32_t body_mode = rrj_read32(body + 604);
                            if (body_mode - 1 < 2 || (!body_mode && rrj_u16(rrj_at(body + 544, 2))))
                                (void)leaf_frontier_call(rrj_host_context(), call, 0x80090D84, body);
                        }
                        flags = rrj_read32(body + 552);
                        if (flags & 0x4000)
                        {
                            uint32_t owner = rrj_read32(body + 596);
                            rrj_write32(body + 552, flags & ~0x4000u);
                            rrj_write32(owner + 568, rrj_read32(owner + 568) & ~8u);
                        }
                        if (rrj_read32(body + 604) != 2)
                        {
                            if (!(rrj_read32(body + 552) & 0x02000000))
                                (void)(uint32_t)sub_800C5078(body, call);
                        }
                        else
                        {
                            event = rrj_u16(rrj_at(body + 544, 2));
                            if (event == 88 || event == 40)
                                (void)leaf_frontier_call(rrj_host_context(), call, 0x800C341C, body);
                            else if (event == 38 || ((event == 39 || event == 89) && ((r_u8(body + 572) & 0x20) && (rrj_read32(actor + 568) & 0x40))))
                                (void)leaf_frontier_call(rrj_host_context(), call, 0x800C3630, body);
                            else if (rrj_read32(actor + 568) & 0x200)
                                (void)(uint32_t)sub_80091468(body);
                            else if (event != 39 && event != 89 && (uint32_t)sub_8005BE58(rrj_read32(body + 540)))
                                (void)(uint32_t)sub_80091468(body);
                            else if (event == 39 || event == 89)
                                (void)leaf_frontier_call(rrj_host_context(), call, 0x800C31CC, body);
                        }
                        if (remaining)
                            body = rrj_read32(rrj_read32(actor + 856) + 852);
                    } while (remaining-- > 0);
                }
            }
            rrj_write32(actor + 560, rrj_read32(actor + 560) & 0xFFFFFFDEu);
            xport_update_u8(actor + 928, XPORT_MEMORY_UPDATE_AND, 0xFD);
        }
        result = stride;
        actor += stride;
    }
    return result;
}

uint32_t sub_800C5078(uint32_t actor, RRJRaceLeafCall call)
{
    static const uint32_t handlers[] = {0x800C47CC, 0x800C4860, 0x800C4B30, 0x800C4BA0, 0x800C4E18};
    uint32_t event = rrj_u16(rrj_at(actor + 544, 2));
    uint32_t result;
    unsigned i;

    FUNCTION_MARKER(0x800C5078, "RASHCDG.BIN");
    if (rrj_u16(rrj_at(0x800541D6 + 8 * event, 2)) == 3)
        return 3;
    for (i = 0; i < sizeof(handlers) / sizeof(handlers[0]); ++i)
    {
        result = leaf_frontier_call(rrj_host_context(), call, handlers[i], actor);
        if (result)
            return result;
    }
    if (rrj_u16(rrj_at(0x800541D6 + 8 * event, 2)) == 2 || event == 39)
        return 39;
    result = (uint32_t)sub_8005BE58(rrj_read32(actor + 540));
    if (result)
    {
        uint32_t output = 0x1F8003F0;
        uint32_t selected = (uint32_t)sub_800C45D8(actor, output);
        return (uint32_t)sub_800C4550(selected & 0xFFFF, actor, rrj_read32(output));
    }
    return result;
}

uint32_t sub_800C47CC(uint32_t actor)
{
    uint32_t count = rrj_u16(rrj_at(actor + 608, 2));
    uint32_t flags;
    uint32_t options;

    FUNCTION_MARKER(0x800C47CC, "RASHCDG.BIN");
    if (!count)
        return 0;
    flags = rrj_read32(actor + 552);
    options = (flags & 0x10000000) ? 34 : 36;
    if (flags & 0x08000000)
        options |= 0x100;
    if ((uint32_t)sub_800C4550(rrj_u16(rrj_at(actor + 610 + 2 * (count - 1), 2)), actor, options))
        rrj_put16(rrj_at(actor + 608, 2), (uint16_t)(count - 1));
    return 1;
}

uint32_t sub_800C4860(uint32_t actor)
{
    uint32_t owner = rrj_read32(actor + 596);
    uint32_t mode = rrj_read32(actor + 604);
    int32_t selector = (int8_t)r_u8(owner + 946);
    uint32_t event;
    uint32_t type;
    uint32_t link;

    FUNCTION_MARKER(0x800C4860, "RASHCDG.BIN");
    rrj_write32(actor + 552, rrj_read32(actor + 552) & 0xEFFFFFFFu);
    if (!(r_u8(actor + 572) & 0x20) && mode == 0 && rrj_u16(rrj_at(owner + 948 + 8 * selector, 2)) >= 3 && rrj_s32(rrj_read32(owner + 480)) > 0x8000)
    {
        event = rrj_u16(rrj_at(actor + 544, 2));
        if (event == 4)
            return sub_800C4550(3, actor, 0x10);
        if (event == 6)
            return sub_800C4550(7, actor, 0x10);
        return 0;
    }
    if (mode < 2 && (rrj_read32(owner + 564) & 0x08000000) && !(r_u8(actor + 572) & 0x20))
    {
        event = rrj_u16(rrj_at(actor + 544, 2));
        if (event == 9)
            return 1;
        link = rrj_read32(0x8005B3E4);
        (void)sub_8005C018(rrj_read32(actor + 540), (rrj_read32(0x8005421C) >> 4) & 0xFFF, 0, 0, rrj_read32(owner + 856) ? 4 : 13, 0, link ? rrj_read32(link + 36) : 0);
        (void)sub_800C2FF4(9, actor, 0);
        return 1;
    }
    if (mode != 1)
        return 0;
    event = rrj_u16(rrj_at(actor + 544, 2));
    if (event != 9 && event != 10 && event != 76 && rrj_s32(rrj_read32(owner + 480)) > 0x8000 && rrj_read32(owner + 600) < 3 * rrj_read32(owner + 596))
    {
        type = (r_u8(actor + 572) & 0x20) ? 76 : 10;
        return sub_800C4550(type, actor, 10);
    }
    if ((r_u8(actor + 572) & 0x20) || rrj_s32(rrj_read32(owner + 480)) > 0x7FFF || (rrj_read32(owner + 568) & 15) || (rrj_read32(owner + 564) & 0x0021D000))
        return 0;
    type = 5;
    if (rrj_u16(rrj_at(owner + 172, 2)) < rrj_read32(rrj_read32(0x8005B2F8) + 48))
    {
        link = rrj_read32(owner + 1084);
        if (rrj_read32(link + 40) && r_u8(link + 39) < 4 && r_u8(rrj_read32(0x8005B2F8) + 57))
            type = 1;
    }
    return sub_800C4550(type, actor, 12);
}

uint32_t sub_800C4B30(uint32_t actor)
{
    uint32_t flags = r_u8(actor + 572);
    uint32_t result = 0;

    FUNCTION_MARKER(0x800C4B30, "RASHCDG.BIN");
    if (!(flags & 0x20) && (flags & 0x0C))
    {
        result = (uint32_t)sub_800C4550(0x10, actor, (flags << 5) & 0x100);
        if (result)
            xport_update_u8(actor + 572, XPORT_MEMORY_UPDATE_AND, 0xF3);
    }
    return result;
}

uint32_t sub_800C4BA0(uint32_t actor)
{
    uint32_t owner = rrj_read32(actor + 596);
    uint32_t state = rrj_u16(rrj_at(actor + 544, 2));
    uint32_t result = 0;

    FUNCTION_MARKER(0x800C4BA0, "RASHCDG.BIN");
    if (rrj_read32(actor + 604) != 1)
        return 0;
    if (rrj_u16(rrj_at(0x800541D6u + 8u * state, 2)) != 2 && (uint32_t)(state - 77u) >= 3u && (uint32_t)(state - 11u) >= 3u)
        return 0;

    result = sub_800C3950(actor) & 0xFFu;
    if (!result && (rrj_read32(owner + 560) & 1u))
        result = sub_800C4550((r_u8(actor + 572) & 0x20u) ? 77u : 8u, actor, 4);
    if (!result)
    {
        int32_t speed = rrj_s32(rrj_read32(owner + 480));

        state = rrj_u16(rrj_at(actor + 544, 2));
        if ((state == 11 || state == 14 || state == 77 || state == 80) && speed > 2050790)
            result = sub_800C4550((r_u8(actor + 572) & 0x20u) ? 78u : 12u, actor, 20);
        else if ((state == 12 || state == 13 || state == 78 || state == 79) && speed <= 1757819)
            result = sub_800C4550((r_u8(actor + 572) & 0x20u) ? 80u : 14u, actor, 20);
    }
    if (!(r_u8(actor + 572) & 0x20u) && !result)
    {
        int32_t elapsed = rrj_s32(rrj_read32(rrj_read32(0x8005B2F8) + 16)) - rrj_s32(rrj_read32(actor + 548));

        if (rrj_s32(rrj_read32(0x800CCB9C)) < elapsed)
        {
            uint32_t type;

            state = rrj_u16(rrj_at(actor + 544, 2));
            if (state == 11)
                type = rrj_s32(rrj_read32(owner + 616)) > 0 ? 25u : 24u;
            else if (state == 13)
                type = 23;
            else
                return 0;
            result = sub_800C4550(type, actor, 4);
            rrj_write32(0x800CCB9C, 300u * (sub_8001FC58() % 6u + 3u));
        }
    }
    return result;
}

uint32_t sub_800C4E18(uint32_t actor)
{
    uint32_t owner;
    uint32_t owner_state;
    uint32_t other;
    uint32_t other_state;
    uint32_t distance;
    uint32_t target_state = 224;
    int32_t separation;
    uint32_t magnitude;
    uint32_t road;
    uint32_t index;
    int collision;

    FUNCTION_MARKER(0x800C4E18, "RASHCDG.BIN");
    if ((r_u8(actor + 572) & 0x20) || rrj_read32(actor + 604) != 1)
        return 0;

    owner = rrj_read32(actor + 596);
    owner_state = owner + 8 * (int32_t)r_s8(owner + 946) + 956;
    owner_state = rrj_u16(rrj_at(owner_state - 8, 2));
    if (owner_state < 4 || owner_state >= 13)
        return 0;

    index = sub_8008B428(owner + 172, 1) & 0xFFFFu;
    if (index == 224)
        return 0;
    other = rrj_read32(0x8005B3A0) + 1096 * index;
    other_state = other + 8 * (int32_t)r_s8(other + 946) + 956;
    other_state = rrj_u16(rrj_at(other_state - 8, 2));
    collision = owner_state == 16 || owner_state == 6 || other_state == 16 || other_state == 6;
    distance = (rrj_read32(other + 324) - rrj_read32(owner + 324)) << 4;

    if (distance - 0xA0000u <= 0x13FFFFu)
    {
        if (collision)
            target_state = 15;
    }
    else if (distance <= 0x9FFFFu && collision)
    {
        target_state = 16;
        if (rrj_read32(actor + 544) == 13 || rrj_read32(actor + 544) == 20)
            target_state = 17;
    }

    if (target_state == 224 || target_state == rrj_read32(actor + 544))
        return 0;

    road = rrj_read32(other + 360);
    if (road == rrj_read32(owner + 360) && (!(road >> 16) || rrj_read32(other + 336) == rrj_read32(owner + 336)))
        separation = rrj_s32(rrj_read32(other + 344) - rrj_read32(owner + 344));
    else
        separation = rrj_s32(sub_800B6AAC(rrj_at(other + 184, 12), rrj_at(owner + 432, 6), rrj_at(owner + 184, 12)));
    if (rrj_s32(rrj_read32(owner + 364)) < 0 && road == rrj_read32(owner + 360) && (!(road >> 16) || rrj_read32(other + 336) == rrj_read32(owner + 336)))
        separation = -separation;

    magnitude = separation < 0 ? 0u - (uint32_t)separation : (uint32_t)separation;
    if (magnitude > 983039)
        return 0;
    return sub_800C4550(target_state, actor, separation < 0 ? 260 : 4);
}

/* Ordinary local spline and coefficient storage for this camera owner */
static int32_t camera_881B4_mul(int32_t left, int32_t right)
{
    return (int32_t)sub_8001FC90(left, right);
}

static int32_t camera_881B4_div(uint32_t numerator, uint32_t denominator)
{
    return (int32_t)sub_80010028(numerator, denominator);
}

static uint32_t camera_881B4_reciprocal(uint32_t input)
{
    uint32_t magnitude = (int32_t)input < 0 ? 0u - input : input;
    uint32_t denominator = (uint32_t)((int32_t)magnitude >> 1) + (uint32_t)((int32_t)(magnitude - 2u) >> 31);
    uint32_t result = denominator ? 0x80000000u / denominator : 0xFFFFFFFFu;
    return (int32_t)input < 0 ? 0u - result : result;
}

static void camera_881B4_coefficients(int32_t phase, uint32_t out[4])
{
    uint32_t square = (uint32_t)camera_881B4_mul(phase, phase);
    uint32_t fourth = (uint32_t)camera_881B4_mul((int32_t)square, phase) - square;
    uint32_t third = fourth - square;
    uint32_t second = 0u - third - fourth;
    out[3] = fourth;
    out[2] = third;
    out[1] = second;
    out[0] = 0x10000u - second;
    out[2] += (uint32_t)phase;
}

static uint32_t camera_881B4_spline(RRJMemory *m, const int32_t *durations, uint32_t guest_durations, uint32_t values, uint32_t output, int32_t count)
{
    uint32_t coefficient[10] = {0};
    uint32_t constant[10] = {0};
    uint32_t value = 0;
    int32_t index;

    /* TODO Other callers may have a different spline array bound */
    if (count > 5)
    {
        fprintf(stderr, "sub_800881B4 unsupported spline count %d\n", count);
        abort();
    }
    coefficient[1] = 0x10000u;
    for (index = 1; index < count; ++index)
    {
        uint32_t left = durations ? (uint32_t)durations[index - 1] : rrj_read32(guest_durations + 4u * (uint32_t)(index - 1));
        uint32_t right = durations ? (uint32_t)durations[index] : rrj_read32(guest_durations + 4u * (uint32_t)index);
        uint32_t ratio = (uint32_t)camera_881B4_div((int32_t)right <= 0 ? 0u - right : right, (int32_t)left <= 0 ? 0u - left : left);
        uint32_t reciprocal, first, second, third, row0, row2, slope, intercept;
        uint32_t offset = (uint32_t)index * 4u;
        if (((int32_t)right > 0) != ((int32_t)left > 0))
            ratio = 0u - ratio;
        reciprocal = camera_881B4_reciprocal(ratio);
        first = (uint32_t)camera_881B4_mul(rrj_s32(rrj_read32(values + offset - 4)), (int32_t)ratio);
        second = (uint32_t)camera_881B4_mul(rrj_s32(rrj_read32(values + offset)), (int32_t)(reciprocal - ratio));
        third = (uint32_t)camera_881B4_mul(rrj_s32(rrj_read32(values + offset + 4)), (int32_t)reciprocal);
        row0 = 3u * (first + second - third);
        row2 = (left + right) << 1;
        slope = (uint32_t)camera_881B4_mul((int32_t)right, (int32_t)coefficient[index - 1]);
        slope += (uint32_t)camera_881B4_mul((int32_t)row2, (int32_t)coefficient[index]);
        intercept = row0 + (uint32_t)camera_881B4_mul((int32_t)right, (int32_t)constant[index - 1]);
        intercept += (uint32_t)camera_881B4_mul((int32_t)row2, (int32_t)constant[index]);
        if (index == count - 1)
        {
            uint32_t mixed = intercept + (uint32_t)camera_881B4_mul((int32_t)left, rrj_s32(rrj_read32(output + offset + 4)));
            uint32_t quotient = (uint32_t)camera_881B4_div((int32_t)mixed <= 0 ? 0u - mixed : mixed, (int32_t)slope <= 0 ? 0u - slope : slope);
            value = (((int32_t)mixed > 0) == ((int32_t)slope > 0)) ? 0u - quotient : quotient;
        }
        else
        {
            uint32_t factor = camera_881B4_reciprocal(left);
            coefficient[index + 1] = 0u - (uint32_t)camera_881B4_mul((int32_t)slope, (int32_t)factor);
            constant[index + 1] = 0u - (uint32_t)camera_881B4_mul((int32_t)intercept, (int32_t)factor);
        }
    }
    rrj_write32(output + 4, value);
    for (index = 2; index < count; ++index)
        rrj_write32(output + 4u * (uint32_t)index, (uint32_t)camera_881B4_mul((int32_t)coefficient[index], (int32_t)value) + constant[index]);
    return 0;
}

#define CAM32(address) (*(int32_t *)rrj_at((uint32_t)(address), 4))
#define CAM16(address) (*(int16_t *)rrj_at((uint32_t)(address), 2))
#define CAMU16(address) (*(uint16_t *)rrj_at((uint32_t)(address), 2))
#define CAM8(address) (*(int8_t *)rrj_at((uint32_t)(address), 1))
#define CAMU8(address) (*(uint8_t *)rrj_at((uint32_t)(address), 1))

uint32_t sub_800881B4(uint32_t actor, int32_t delta, RRJRaceLeafCall call)
{
    int32_t v3;
    int32_t v4;
    int32_t v5;
    int32_t v6;
    int32_t v7;
    uint32_t v8;
    int32_t v9;
    int32_t v10;
    int16_t v11;
    int32_t v12;
    int32_t v13;
    int32_t v14;
    int32_t v15;
    uint32_t v16;
    int32_t v17;
    int32_t v18;
    int32_t v19;
    int32_t v20;
    int32_t v21;
    int32_t v22;
    int32_t v23;
    int32_t v24;
    int32_t v25;
    uint32_t v26;
    int32_t v27;
    int32_t v28;
    int32_t v29;
    int32_t v30;
    int32_t v31;
    int32_t v32;
    int32_t v33;
    int32_t v34;
    int32_t v35;
    int32_t v36;
    int32_t v37;
    int32_t v38;
    int32_t v39;
    int32_t v40;
    int32_t v41;
    int32_t v42;
    int32_t v43;
    int32_t v44;
    int32_t v45;
    uint32_t v46;
    uint32_t v47;
    int32_t v48;
    int32_t v49;
    int32_t v50;
    int32_t v51;
    int32_t v52;
    uint32_t v53;
    int32_t v54;
    int32_t v55;
    int32_t v56;
    int32_t v57;
    int32_t v58;
    uint32_t v59;
    int32_t v60;
    int32_t v61;
    int32_t v62;
    uint32_t v63;
    int32_t v64;
    int32_t v65;
    uint32_t v66;
    int32_t v67;
    int32_t v68;
    int32_t v69;
    int32_t v70;
    int32_t v71;
    int32_t v72;
    int32_t v73;
    uint32_t v74;
    int32_t v75;
    int32_t v76;
    int32_t v77;
    uint32_t v78;
    int64_t v79;
    int64_t v80;
    int64_t v81;
    int64_t v82;
    int64_t v83;
    int32_t v84;
    int32_t v85;
    int32_t v86;
    uint32_t v87;
    int32_t v88;
    int32_t v89;
    int32_t v90;
    int32_t v91;
    int32_t v92;
    int32_t v93;
    uint32_t v94;
    uint32_t v95;
    int32_t v96;
    uint32_t v97;
    int32_t v98;
    int32_t v99;
    uint32_t v100;
    int32_t v101;
    int32_t v102;
    int32_t v103;
    int32_t v104;
    int32_t v105;
    int32_t v106;
    int32_t v107;
    uint32_t v108;
    int32_t v109;
    int32_t v110;
    int32_t v111;
    int32_t v112;
    int32_t v113;
    int32_t v114;
    int32_t v115;
    int32_t v116;
    int32_t v117;
    int32_t v118;
    int32_t v119;
    int32_t v120;
    int32_t v121;
    int32_t v122;
    int32_t v123;
    int32_t v124;
    int32_t v125;
    int32_t v126;
    int32_t v127;
    int32_t v128;
    int32_t v129;
    int32_t v130;
    int32_t v131;
    int32_t v132;
    int32_t v133;
    int32_t v134;
    int32_t v135;
    int32_t v136;
    uint32_t v137;
    int32_t v138;
    int32_t v139;
    int32_t v140;
    int32_t v141;
    int32_t v142;
    int32_t v143;
    int32_t v144;
    int32_t v145;
    int32_t v146;
    int32_t v147;
    int32_t v148;
    int32_t v149;
    int32_t v150;
    int32_t v151;
    int32_t v152;
    int32_t v153;
    int32_t v154;
    int32_t v155;
    int32_t v156;
    int32_t v157;
    int32_t v158;
    int32_t v159;
    int32_t v160;
    int32_t v161;
    int32_t v162;
    int32_t v163;
    int32_t v164;
    int32_t v165;
    int32_t v166;
    int32_t v167;
    int32_t v168;
    int32_t v169;
    int32_t v170;
    int32_t v171;
    int32_t v172;
    int32_t v173;
    int32_t v174;
    int32_t v175;
    int32_t v176;
    int32_t v177;
    int32_t v178;
    int32_t v179;
    int32_t v180;
    int32_t v181;
    int32_t v182;
    int32_t v183;
    uint32_t v184;
    int32_t v185;
    int32_t v186;
    uint32_t v188;
    uint32_t v189;
    int32_t v191;
    int32_t v192;
    int32_t v193;
    int32_t v194;
    uint32_t v195;
    int32_t v196;
    uint8_t v197;
    int32_t v198;
    uint32_t v199;
    int32_t v200;
    int32_t v201;
    int32_t v202;
    int32_t v203;
    int32_t v204;
    int32_t v205;
    int32_t v206;
    int32_t v207;
    int32_t v208;
    int32_t v209;
    int32_t v210;
    int32_t v211;
    uint32_t v212;
    int32_t v213;
    int32_t v214;
    int32_t v215;
    int32_t v216;
    int32_t v217;
    int32_t v218;
    int32_t v219;
    int32_t v220;
    int32_t v221;
    int32_t v222;
    int32_t v223;
    int32_t v224;
    int32_t v225;
    int32_t v226;
    int32_t v227;
    int32_t v228;
    int32_t v229;
    int32_t v230;
    int32_t v231;
    uint32_t v232;
    int32_t v233;
    int32_t v234;
    uint32_t v235;
    int32_t v236;
    int32_t v237;
    int32_t v238;
    int32_t v239;
    int32_t v240;
    int32_t v241;
    int32_t v242;
    int32_t v243;
    uint32_t v244;
    int32_t v245;
    int32_t v246;
    int32_t v247;
    uint32_t v248;
    int32_t v249;
    uint32_t v250;
    int32_t v251;
    int32_t v252;
    int32_t v253;
    int32_t v254;
    int32_t v255;
    int32_t v256;
    int32_t v257;
    int32_t v258;
    int32_t v259;
    int32_t v260;
    int32_t v261;
    uint32_t v262;
    int32_t v263;
    int32_t v264;
    int32_t v265;
    int32_t v266;
    int32_t v267;
    int32_t v268;
    int32_t v269;
    int32_t v270;
    int32_t v271;
    int32_t v272;
    int32_t v273;
    int32_t v274;
    uint32_t v275;
    int32_t v276;
    int32_t v277;
    int32_t v278;
    int32_t v279;
    int32_t v280;
    int32_t v281;
    int32_t v282;
    int32_t v283;
    int16_t v284;
    int16_t v285;
    int32_t v286;
    int32_t v287;
    int32_t v288;
    int32_t v289;
    int32_t v290;
    int32_t v291;
    int32_t v292;
    int32_t v293;
    int32_t v294;
    int32_t v295;
    int32_t v296;
    int32_t v297;
    int32_t v298;
    int32_t v299;
    int32_t v300;
    int32_t v301;
    int32_t v302;
    int32_t v303;
    int32_t v304;
    int32_t v305;
    int32_t v306;
    int32_t v307;
    int32_t v308;
    int32_t v309;
    int32_t v310;
    int32_t v311;
    int32_t v312;
    int32_t v313;
    int32_t v314;
    int32_t v315;
    int32_t v316;
    int32_t v317;
    int32_t v318;
    int32_t v319;
    int32_t v320;
    int32_t v321;
    int32_t v322;
    int32_t v323;
    int32_t v324;
    int32_t v325;
    int32_t v326;
    int32_t v327;
    int32_t v328;
    int32_t v329;
    int32_t v330;
    int32_t v331;
    int32_t v332;
    int32_t v333;
    int32_t v334;
    int32_t v335;
    int32_t v336;
    int32_t v337;
    int32_t v338;
    int32_t v339;
    int32_t v340;
    uint32_t v341;
    int32_t v342;
    int32_t v343;
    int64_t v344;
    int32_t v345;
    int32_t v346;
    int32_t v347;
    int32_t v348;
    int32_t v349;
    int32_t v350;
    uint32_t result;
    int32_t *v372;
    int32_t v373;
    int32_t v374;
    int32_t v375;
    int32_t v376;
    int32_t v377;
    int32_t v378;
    int32_t v379;
    int64_t v380;
    int32_t camera_position[3];
    int32_t camera_vector[3];
    int32_t camera_delta[6];
    uint32_t camera_coeff_a[4], camera_coeff_b[4], camera_coeff_c[4];
    uint16_t camera_normal[3];
    int32_t camera_sine;

    FUNCTION_MARKER(0x800881B4, "RASHCDG.BIN");
    v3 = 1;
    if (CAM32(actor + 772))
    {
        v4 = 0;
        v5 = CAM32(actor + 776);
        v6 = CAM32(actor + 540);
        v7 = CAM32(actor + 568);
        v8 = 0x800D83B4u + 26 * v5;
        v9 = CAM16(v7 + 370);
        v10 = (uint32_t)(v6 - 7) < 4;
        v11 = CAM16(v7 + 370);
        if ((uint32_t)(v6 - 7) >= 4)
        {
            v3 = 0;
            if ((uint32_t)(v6 - 13) >= 2)
            {
                if (v6 == 12)
                {
                    if (CAMU8(CAM32(0x8005B2F8u) + 4) == 44)
                    {
                        v13 = CAM32(actor + 792);
                        if (v13 > 0 && v13 >= CAM32(actor + 700))
                        {
                            v14 = 0;
                        }
                        else if (v5 == 5)
                        {
                            v14 = (CAM32(actor + 552) >> 4) & 1;
                        }
                        else if (v5 >= 6)
                        {
                            if (v5 == 6)
                                v14 = ((CAM32(CAM32(0x8005B38Cu) + 560) >> 27) ^ 1) & 1;
                            else
                                v14 = 0;
                        }
                        else
                        {
                            v14 = 0;
                            if (v5 == 4)
                                v14 = CAM32(CAM32(0x8005B38Cu) + 480) == 0;
                        }
                    }
                    else
                    {
                        v15 = CAM32(actor + 792);
                        v14 = 0;
                        if (v15)
                            v14 = v15 < CAM32(actor + 700);
                    }
                    v16 = CAM32(actor + 756);
                    if (v16 && !CAM16(v16) || v14)
                    {
                        v6 = 1;
                        if (CAMU8(CAM32(0x8005B2F8u) + 4) == 44)
                        {
                            v17 = CAM32(actor + 776);
                            v6 = 0;
                            if (v17 == 4)
                            {
                                v18 = CAM32(CAM32(CAM32(0x8005B38Cu) + 856) + 852);
                                CAM32(actor + 752) = 0x800CE5A8u;
                                CAM32(actor + 756) = 0;
                                CAM32(actor + 568) = v18;
                            }
                            else if (v17 == 5)
                            {
                                v19 = CAM32(0x8005B330u) + 184;
                                v20 = CAM32(0x8005B330u) + 320;
                                CAM32(actor + 568) = CAM32(0x8005B38Cu);
                                CAM32(actor + 752) = v19;
                                CAM32(actor + 756) = v20;
                            }
                            else
                            {
                                v6 = 1;
                                CAM32(actor + 548) |= 4u;
                                CAM32(actor + 552) &= ~2u;
                            }
                        }
                        if (v6)
                        {
                            sub_8008A998(actor, CAM32(actor + 544));
                            sub_80086AF8(actor);
                            CAM32(actor + 552) &= ~0x10u;
                        }
                        else
                        {
                            v3 = 1;
                            CAM32(actor + 552) |= 2u;
                        }
                    }
                }
                else
                {
                    v6 = CAM32(actor + 552);
                    if ((v6 & 7) != 0)
                    {
                        if (CAMU8(CAM32(0x8005B2F8u) + 4) != 44 || CAMU8(CAM32(0x8005B2F8u) + 57) < 4u)
                            v3 = ((uint32_t)v6 >> 5) & 1;
                    }
                    else
                    {
                        v21 = CAMU16(v8);
                        v3 = v21 < v9;
                        v6 = v9 << 16;
                        if (v21 >= v9)
                        {
                            if (v5 > 0)
                                v3 = (int16_t)v9 < (int)CAMU16(0x800D83B0u + 2 * (13 * v5 - 11));
                        }
                        else
                        {
                            v3 = v5 < CAM32(0x800D83B0u) - 1;
                        }
                        if (v3)
                            goto LABEL_51;
                        if (CAM8(v8 + 1 * (4)) == 2)
                        {
                            v22 = CAM8(v8 + 1 * (5));
                            v23 = CAM8(v8 + 1 * (6));
                            v24 = CAM16(0x800D83B0u + 2 * (v22 + 262));
                            if (v22 >= 2)
                                v25 = v24 + v23;
                            else
                                v25 = v24 + 2 * v23;
                            v26 = (0x800D85C4u + 6 * v25);
                            v27 = (CAM16(v26) << 16) - CAM32(v7 + 184);
                            v28 = (CAM16(v26 + 2 * (2)) << 16) - CAM32(v7 + 192);
                            v29 = ((v27 >> 31) + (v27 >> 16)) ^ (v27 >> 31);
                            v6 = ((v28 >> 31) + (v28 >> 16)) ^ (v28 >> 31);
                            v30 = v29;
                            if (v29 < v6)
                            {
                                v29 = ((v28 >> 31) + (v28 >> 16)) ^ (v28 >> 31);
                                v6 = v30;
                            }
                            v4 = 1;
                            v3 = (v29 - (v29 >> 5) - (v29 >> 7) + ((v6 + (v6 >> 1)) >> 2) + ((v6 + (v6 >> 1)) >> 6)) << 16 <= 3932159;
                        }
                    }
                }
            }
            else
            {
                camera_position[0] = CAM32(v7 + 184) - CAM32(actor + 184);
                camera_position[1] = CAM32(v7 + 188) - CAM32(actor + 188);
                camera_position[2] = CAM32(v7 + 192) - CAM32(actor + 192);
                v12 = (int32_t)(((camera_position[1] * (int64_t)(16 * CAM16(actor + 602))) >> 16) + ((camera_position[0] * (int64_t)(16 * CAM16(actor + 600))) >> 16));
                v380 = camera_position[2] * (int64_t)(16 * CAM16(actor + 604));
                v6 = ((int32_t)(v380 >> 32)) >> 16;
                v3 = (int)((v380 >> 16) + v12) > 3932160;
            }
        }
        if (!v3 && (CAM32(actor + 552) & 0x10) == 0)
            goto LABEL_101;
    LABEL_51:
        v31 = CAM32(actor + 552);
        if ((v31 & 1) != 0)
        {
            v32 = CAM32(actor + 544);
            CAM32(actor + 772) = 0;
            CAM32(actor + 540) = v32;
            v33 = v31;
            v34 = CAM32(actor + 548);
            CAM32(actor + 552) = v33 & 0xFFFFFFFE;
            CAM32(actor + 548) = v34 | 0x102;
        LABEL_101:
            CAM32(actor + 548) &= 0xFF9FFFFF;
            goto LABEL_102;
        }
        if ((v31 & 0xC) != 0)
        {
            CAM32(actor + 552) = v31 & 0xFFFFFFF3 | 8;
            CAM32(actor + 540) = 6;
            goto LABEL_101;
        }
        CAM32(actor + 552) = v31 & 0xFFFFFFEF;
        if ((v31 & 2) != 0)
        {
            v35 = CAM32(0x8005B2F8u);
            ++CAM32(actor + 776);
            if ((CAMU8(v35 + 4) & 1) != 0)
            {
                CAM32(actor + 552) = CAM32(actor + 552) & 0xFFFFFFFC | 1;
                sub_80018C1C(0);
                v36 = CAM32(actor + 568);
                CAM32(actor + 752) = v36 + 504;
                CAM32(actor + 756) = v36 + 320;
                if (actor == 0x800CD898u)
                    CAM32(actor + 568) = CAM32(0x8005B38Cu);
                else
                    CAM32(actor + 568) = CAM32(0x8005B21Cu);
            }
        }
        else if (v10)
        {
            if ((CAMU8(CAM32(0x8005B2F8u) + 4) & 1) != 0 && CAM32(actor + 540) == 7)
            {
                v37 = 4;
                if (CAM32(0x8005B2B0u) == 1)
                    v37 = 6;
                v38 = CAM32(actor + 552);
                CAM32(actor + 776) = v37;
                v39 = v38 | 2;
            }
            else
            {
                CAM32(actor + 776) = CAM32(actor + 540) - 7;
                v40 = CAM32(actor + 552);
                v39 = v40 | 4;
                if (CAM32(actor + 540) == 7)
                    v39 = v40 | 1;
            }
            CAM32(actor + 552) = v39;
        }
        else if (CAMU16(v8) >= v11)
        {
            v44 = CAM32(actor + 776);
            if (v44 <= 0)
                goto LABEL_78;
            if (v11 < (int)CAMU16(0x800D83B0u + 2 * (13 * v44 - 11)))
            {
                do
                {
                    v45 = CAM32(actor + 776);
                    CAM32(actor + 776) = v45 - 1;
                } while (v45 - 1 > 0 && v11 < (int)CAMU16(0x800D83B0u + 2 * (13 * v45 - 24)));
            }
        }
        else
        {
            do
            {
                v41 = CAM32(actor + 776);
                v42 = v41 < CAM32(0x800D83B0u) - 1;
                v43 = v41 + v42;
                CAM32(actor + 776) = v43;
            } while (v42 && CAMU16(0x800D83B0u + 2 * (13 * v43 + 2)) < v11);
        }
        v44 = CAM32(actor + 776);
    LABEL_78:
        v46 = 0x800D83B4u + 26 * v44;
        if ((CAM32(actor + 552) & 7) != 0)
            CAM32(actor + 792) = CAMU16(v46) << 16;
        else
            CAM32(actor + 792) = 0;
        v47 = v46 + 3;
        v48 = 0;
        if ((uint8_t)CAM8(v46 + 1 * (2)) >= 2u)
        {
            if (CAM8(v46 + 1 * (4)) == 2)
            {
                v49 = CAM8(v46 + 1 * (5));
                v50 = CAM8(v46 + 1 * (6));
                v51 = CAM16(0x800D83B0u + 2 * (v49 + 262));
                if (v49 >= 2)
                    v52 = v51 + v50;
                else
                    v52 = v51 + 2 * v50;
                v53 = (0x800D85C4u + 6 * v52);
                v54 = (CAM16(v53) << 16) - CAM32(v7 + 184);
                v55 = (CAM16(v53 + 2 * (2)) << 16) - CAM32(v7 + 192);
                v56 = ((v54 >> 31) + (v54 >> 16)) ^ (v54 >> 31);
                v57 = ((v55 >> 31) + (v55 >> 16)) ^ (v55 >> 31);
                v58 = v56;
                if (v56 < v57)
                {
                    v56 = ((v55 >> 31) + (v55 >> 16)) ^ (v55 >> 31);
                    v57 = v58;
                }
                v4 = 1;
                if (CAM8(v46 + 1 * (4)) == 2)
                {
                    v59 = actor;
                    if ((v56 - (v56 >> 5) - (v56 >> 7) + ((v57 + (v57 >> 1)) >> 2) + ((v57 + (v57 >> 1)) >> 6)) << 16 <= 3932159)
                        goto LABEL_100;
                }
            }
            v60 = (uint8_t)CAM8(v46 + 1 * (2)) - v4;
            v61 = v60 + 1;
            if (v60 == 1)
            {
                v48 = 1;
            }
            else
            {
                do
                    v48 = v4 + ((v61 * (uint8_t)(uint32_t)sub_80043F00(0xF2000002u)) >> 8);
                while (v48 == CAM32(actor + 784));
            }
            v62 = 0;
            if (v48 > 0)
            {
                v63 = v46 + 5;
                do
                {
                    v64 = v62 < v48;
                    if (CAM8(v63) == 7)
                        v64 = ++v62 < v48;
                    v63 += 2;
                } while (v64);
                v47 = v63 - 2;
            }
        }
        v59 = actor;
    LABEL_100:
        sub_800853E4(v59, v47);
        CAM32(actor + 780) = (v47 - 3 - v46) >> 1;
        CAM32(actor + 784) = v48;
        CAM32(actor + 700) = 0;
        goto LABEL_101;
    }
LABEL_102:
    v65 = CAM32(actor + 700);
    if (v65 <= 655359999)
        v65 += delta;
    v66 = CAM32(actor + 540);
    CAM32(actor + 700) = v65;
    if (v66 < 4 || v66 - 7 < 6)
    {
        if (!CAM32(actor + 772))
        {
            v188 = CAM32(actor + 568);
            v189 = CAM32(v188 + 4 * (213));
            if ((CAM32(actor + 548) & 0x48000) != 0 && CAM32(v189 + 4 * (151)) >= 3u && (CAM32(v189 + 4 * (138)) & 0x80000) == 0 && sub_8001FCB0((CAM32(v189 + 4 * (46)) - CAM32(actor + 184)) >> 16, (CAM32(v189 + 4 * (47)) - CAM32(actor + 188)) >> 16, (CAM32(v189 + 4 * (48)) - CAM32(actor + 192)) >> 16) >= 61)
            {
                if ((CAM32(actor + 548) & 0x40000) != 0)
                {
                    camera_delta[0] = CAM32(v189 + 4 * (46)) - CAM32(actor + 184);
                    camera_delta[1] = CAM32(v189 + 4 * (47)) - CAM32(actor + 188);
                    camera_delta[2] = CAM32(v189 + 4 * (48)) - CAM32(actor + 192);
                    v191 = (int32_t)(((camera_delta[1] * (int64_t)(16 * CAM16(actor + 446))) >> 16) + ((camera_delta[0] * (int64_t)(16 * CAM16(actor + 444))) >> 16));
                    v380 = camera_delta[2] * (int64_t)(16 * CAM16(actor + 448));
                    v192 = (int)((v380 >> 16) + v191) > 0;
                }
                else
                {
                    v192 = 1;
                }
                if (v192)
                    sub_8008AAB0(actor);
            }
            v193 = CAM32(actor + 540);
            v194 = CAM32(actor + 548);
            CAM32(actor + 672) = 0;
            CAM32(actor + 636) = 0;
            v195 = (0x800CD7B8u + 4 * (14 * v193));
            v196 = v194 & 1;
            if ((v194 & 0x800000) != 0 && (CAM32(v188 + 4 * (97)) & 1) != 0 && (int)CAM32(v188 + 4 * (120)) > 146486)
            {
                v197 = (uint8_t)(uint32_t)sub_80043F00(0xF2000002u);
                v198 = CAM32(v188 + 4 * (120));
                v199 = (16843009 * (uint32_t)v197) >> 24;
                if (v198 <= 0)
                    v200 = -camera_881B4_div(-v198, 3932160);
                else
                    v200 = camera_881B4_div(v198, 3932160);
                v201 = camera_881B4_mul(v200, CAM32(0x800CD7B8u + 4 * (14 * CAM32(actor + 540) + 13)));
                v203 = camera_881B4_mul(v199, v201);
                v202 = camera_881B4_mul(v200, CAM32(0x800CD7B8u + 4 * (14 * CAM32(actor + 540) + 13)));
                v205 = camera_881B4_mul(v199, v202);
                v204 = camera_881B4_mul(v200, CAM32(0x800CD7B8u + 4 * (14 * CAM32(actor + 540) + 13)));
                v206 = camera_881B4_mul(v199, v204);
                v207 = CAM32(v195 + 4 * (v196 + 3));
                v208 = ((v203 >> 31) + v205) ^ (v206 >> 31);
                CAM32(actor + 676) = v208;
                CAM32(actor + 640) = v207 + v208;
            }
            else
            {
                if ((CAM32(v188 + 4 * (141)) & 0x800) != 0)
                    v209 = CAM32(actor + 740);
                else
                    v209 = CAM32(actor + 732);
                sub_80086B1C(actor, delta, v209);
                v210 = CAM32(v195 + 4 * (v196 + 3));
                v211 = CAM32(actor + 732);
                CAM32(actor + 676) = 0;
                CAM32(actor + 640) = v210 - v211;
            }
            if ((CAM32(actor + 548) & 1) != 0)
            {
                v224 = CAM32(v195 + 4 * (v196 + 1));
            }
            else
            {
                v212 = (0x800CD7B8u + 4 * (14 * CAM32(actor + 540)));
                if ((int)CAM32(v188 + 4 * (121)) <= 0)
                {
                    v213 = CAM32(v212 + 4 * (10));
                    v214 = CAM32(v212 + 4 * (9));
                }
                else
                {
                    v213 = CAM32(v212 + 4 * (8));
                    v214 = CAM32(v212 + 4 * (7));
                }
                v216 = CAM32(actor + 724) - CAM32(v188 + 4 * (120));
                v215 = camera_881B4_mul(v213, CAM32(actor + 736));
                v217 = v216;
                v219 = v215;
                v218 = camera_881B4_mul(v217, v214);
                v220 = CAM32(actor + 736);
                CAM32(actor + 712) = -v219 - v218;
                if (v220 > 0)
                    CAM32(actor + 712) -= camera_881B4_mul(v220, 2 * v213);
                v221 = CAM32(actor + 724) + camera_881B4_mul(CAM32(actor + 712), delta);
                CAM32(actor + 724) = v221;
                v222 = camera_881B4_mul(v221 - CAM32(v188 + 4 * (120)), delta);
                v223 = CAM32(actor + 736) + v222 + (((CAM32(actor + 736) + v222 + 6553600) >> 31) & (-6553600 - (CAM32(actor + 736) + v222))) + (((52428 - (CAM32(actor + 736) + v222)) >> 31) & (52428 - (CAM32(actor + 736) + v222)));
                CAM32(actor + 736) = v223;
                v224 = CAM32(v195 + 4 * (v196 + 1)) + v223;
            }
            CAM32(actor + 644) = v224;
            v225 = CAM32(actor + 548);
            CAM32(actor + 680) = CAM32(v195 + 4 * (v196 + 5));
            if ((v225 & 2) != 0)
            {
                if ((v225 & 4) != 0)
                {
                    v226 = CAM32(actor + 640);
                    v227 = CAM32(actor + 644);
                    v228 = CAM32(actor + 672);
                    v229 = CAM32(actor + 676);
                    v230 = CAM32(actor + 680);
                    CAM32(actor + 624) = CAM32(actor + 636);
                    v231 = CAM32(actor + 548);
                    CAM32(actor + 628) = v226;
                    CAM32(actor + 632) = v227;
                    CAM32(actor + 660) = v228;
                    CAM32(actor + 664) = v229;
                    CAM32(actor + 668) = v230;
                    v232 = v231 & 0xFFFFEFA7 | 0x20;
                }
                else
                {
                    v233 = CAM32(actor + 552);
                    if ((v233 & 0x80) != 0)
                    {
                        v234 = CAM32(actor + 624);
                        v235 = v233 & 0xFFFFFCFF;
                        CAM32(actor + 552) = v235;
                        if ((((v234 >> 31) + v234) ^ (v234 >> 31)) > 102942)
                        {
                            v236 = v235 | 0x100;
                            if (CAM32(actor + 760) >= v234)
                                v236 = v235 | 0x200;
                        }
                        else
                        {
                            v236 = v235 | 0x200;
                            if (v234 <= 0)
                                v236 = v235 | 0x100;
                        }
                        CAM32(actor + 552) = v236;
                        camera_delta[0] = CAM32(CAM32(actor + 752)) - CAM32(actor + 572);
                        camera_delta[1] = CAM32(CAM32(actor + 752) + 4) - CAM32(actor + 576);
                        camera_delta[2] = CAM32(CAM32(actor + 752) + 8) - CAM32(actor + 580);
                        CAM32(actor + 660) = 0;
                        CAM32(actor + 668) = sub_8001FCB0(((int16_t)((uint32_t)camera_delta[0] >> 16)), ((int16_t)((uint32_t)camera_delta[1] >> 16)), ((int16_t)((uint32_t)camera_delta[2] >> 16))) << 16;
                    }
                    else if ((v225 & 0x40) == 0)
                    {
                        sub_80086584(actor, actor + 572, actor + 184, actor + 624);
                        sub_80086584(actor, actor + 572, actor + 556, actor + 660);
                        sub_800863EC((actor + 624));
                        CAM32(actor + 548) = CAM32(actor + 548) & 0xFFFFFF9F | 0x40;
                    }
                    v232 = CAM32(actor + 548) | 0x3018;
                }
                CAM32(actor + 548) = v232;
                CAM32(actor + 548) &= ~0x1000000u;
            }
            v237 = CAM32(actor + 548);
            if ((v237 & 8) != 0)
            {
                if ((v237 & 0x40) != 0)
                    CAM32(actor + 636) = 0;
                sub_8008676C(actor, delta);
            }
            else
            {
                v238 = CAM32(actor + 640);
                v239 = CAM32(actor + 644);
                v240 = CAM32(actor + 672);
                v241 = CAM32(actor + 676);
                v242 = CAM32(actor + 680);
                CAM32(actor + 624) = CAM32(actor + 636);
                CAM32(actor + 628) = v238;
                CAM32(actor + 632) = v239;
                CAM32(actor + 660) = v240;
                CAM32(actor + 664) = v241;
                CAM32(actor + 668) = v242;
            }
            if ((CAM32(actor + 548) & 0x20) != 0)
            {
                camera_vector[0] = CAM32(actor + 624);
                camera_vector[2] = CAM32(actor + 632);
            }
            else
            {
                v243 = CAM32(actor + 624);
                if (v243)
                {
                    v244 = ((uint32_t)(163 * v243) >> 14) & 0xFFF;
                    camera_vector[0] = camera_881B4_mul(CAM32(actor + 632), 16 * (int16_t)CAM16(0x8005624Cu + 2 * (2 * v244)));
                    camera_vector[2] = camera_881B4_mul(CAM32(actor + 632), 16 * (int16_t)CAM16(0x8005624Cu + 2 * (2 * v244 + 1)));
                }
                else
                {
                    camera_vector[0] = 0;
                    camera_vector[2] = CAM32(actor + 632);
                }
                v245 = CAM32(actor + 548);
                if ((v245 & 8) == 0)
                    CAM32(actor + 548) = v245 & 0xFFFFFF9F | 0x20;
            }
            sub_80087420(actor, delta, call);
        LABEL_246:
            v246 = CAM32(actor + 548);
            if ((v246 & 0x40004) != 0x40000)
            {
                if ((v246 & 0x1000000) != 0)
                    camera_vector[0] = -camera_vector[0];
                sub_8002ECB8(rrj_at(actor + 600, 6), rrj_at(actor + 588, 6), rrj_at(actor + 184, 12), camera_vector[2], camera_vector[0]);
                sub_8002EAD8(rrj_at(actor + 184, 12), rrj_at(actor + 594, 6), CAM32(actor + 628), (uint32_t *)camera_vector);
                CAM32(actor + 184) = CAM32(actor + 572) + camera_vector[0];
                CAM32(actor + 188) = CAM32(actor + 576) + camera_vector[1];
                CAM32(actor + 192) = CAM32(actor + 580) + camera_vector[2];
            }
            if ((CAM32(actor + 548) & 8) != 0 && ((CAM32(actor + 552) & 0x80) != 0 || CAM32(actor + 540) == 12))
            {
                v247 = CAM32(actor + 624);
                if (v247)
                {
                    v248 = ((uint32_t)(163 * v247) >> 14) & 0xFFF;
                    camera_vector[0] = camera_881B4_mul(CAM32(actor + 668), 16 * (int16_t)CAM16(0x8005624Cu + 2 * (2 * v248)));
                    camera_vector[2] = camera_881B4_mul(CAM32(actor + 668), 16 * (int16_t)CAM16(0x8005624Cu + 2 * (2 * v248 + 1)));
                    goto LABEL_258;
                }
                camera_vector[0] = 0;
            }
            else
            {
                camera_vector[0] = CAM32(actor + 660);
            }
            camera_vector[2] = CAM32(actor + 668);
        LABEL_258:
            if ((CAM32(actor + 548) & 0x1000000) != 0)
                camera_vector[0] = -camera_vector[0];
            sub_8002ECB8(rrj_at(actor + 600, 6), rrj_at(actor + 588, 6), rrj_at(actor + 556, 12), camera_vector[2], camera_vector[0]);
            v249 = CAM32(actor + 664);
            if (v249)
                sub_8002EAD8(rrj_at(actor + 556, 12), rrj_at(actor + 594, 6), v249, rrj_at(actor + 556, 12));
            if (CAM32(actor + 540) == 12 && (CAM32(actor + 548) & 8) == 0)
            {
                v250 = CAM32(actor + 752);
                CAM32(actor + 556) += CAM32(v250);
                v251 = CAM32(actor + 560) + CAM32(v250 + 4 * (1));
                v252 = CAM32(actor + 752);
                CAM32(actor + 560) = v251;
                CAM32(actor + 564) += CAM32(v252 + 8);
                goto LABEL_303;
            }
            goto LABEL_302;
        }
        v372 = camera_vector;
        if (v66 == 12)
        {
            v67 = CAM32(actor + 548);
            if ((v67 & 2) != 0)
            {
                if ((v67 & 4) != 0)
                {
                    v68 = CAM32(actor + 852);
                    v69 = CAM32(actor + 876);
                    v70 = CAM32(actor + 900);
                    v71 = CAM32(actor + 924);
                    v72 = CAM32(actor + 948);
                    CAM32(actor + 624) = CAM32(actor + 828);
                    v73 = CAM32(actor + 548);
                    CAM32(actor + 628) = v68;
                    CAM32(actor + 632) = v69;
                    CAM32(actor + 660) = v70;
                    CAM32(actor + 664) = v71;
                    CAM32(actor + 668) = v72;
                    v74 = v73 & 0xFFFFEFE7;
                }
                else
                {
                    sub_80086584(actor, actor + 572, actor + 556, actor + 660);
                    if ((CAM32(actor + 548) & 0x40) == 0)
                    {
                        sub_80086584(actor, actor + 572, actor + 184, actor + 624);
                        sub_800863EC((actor + 624));
                    }
                    v74 = CAM32(actor + 548) | 0x3018;
                }
                CAM32(actor + 548) = v74;
                v75 = CAM32(actor + 800);
                CAM32(actor + 548) = CAM32(actor + 548) & 0xFFFFFF9F | 0x40;
                if (v75 >= 3)
                {
                    v76 = 0;
                    if (v75 - 1 > 0)
                    {
                        do
                        {
                            v77 = 4 * v76++;
                            camera_delta[v77 / 4] = CAM32(actor + 4 * v76 + 828) - CAM32(actor + v77 + 828);
                        } while (v76 < CAM32(actor + 800) - 1);
                    }
                    camera_881B4_spline(rrj_host_context(), camera_delta, 0, actor + 852, actor + 996, CAM32(actor + 800) - 1);
                    camera_881B4_spline(rrj_host_context(), camera_delta, 0, actor + 876, actor + 1020, CAM32(actor + 800) - 1);
                }
                CAM32(actor + 552) &= ~0x20u;
            }
            sub_80087420(actor, delta, call);
            v78 = CAM32(actor + 752);
            if (v78)
            {
                camera_delta[0] = CAM32(v78) - CAM32(actor + 572);
                camera_delta[1] = CAM32(CAM32(actor + 752) + 4) - CAM32(actor + 576);
                camera_delta[2] = CAM32(CAM32(actor + 752) + 8) - CAM32(actor + 580);
                v79 = camera_delta[0] * (int64_t)(16 * CAM16(actor + 588));
                v80 = camera_delta[1] * (int64_t)(16 * CAM16(actor + 590));
                v81 = camera_delta[2] * (int64_t)(16 * CAM16(actor + 592));
                v82 = camera_delta[0] * (int64_t)(16 * CAM16(actor + 600));
                v83 = camera_delta[1] * (int64_t)(16 * CAM16(actor + 602));
                v380 = camera_delta[2] * (int64_t)(16 * CAM16(actor + 604));
                v84 = (int32_t)(25736u * sub_80020018((uint32_t)((v81 >> 16) + (v80 >> 16) + (v79 >> 16)), (uint32_t)((v380 >> 16) + (v83 >> 16) + (v82 >> 16)))) >> 8;
            }
            else
            {
                v84 = 0;
            }
            v85 = (CAM32(actor + 548) >> 3) & 1;
            CAM32(actor + (-v85 & 0xC) + 624) = v84;
            if ((CAM32(actor + 548) & 2) != 0)
                CAM32(actor + 760) = v84;
            v86 = CAM32(actor + 800);
            v87 = (actor + 828);
            if (v86 >= 2)
            {
                v88 = CAM32(actor + 828);
                v89 = CAM32(actor + 796);
                v90 = v86 - 1;
                if (v84 >= v88)
                {
                    v92 = v84 < CAM32(v87 + 4 * (v89));
                    if (v89 == v90)
                    {
                        v89 = 0;
                        v92 = v84 < v88;
                    }
                    if (v92)
                    {
                        v91 = v89 - 1;
                        if (v91 >= 0)
                        {
                            v95 = (v87 + 4 * (v91));
                            while (1)
                            {
                                v96 = v91;
                                if (v84 >= CAM32(v95))
                                    break;
                                ++v91;
                                (v95 += 4);
                                if (v91 < 0)
                                    goto LABEL_139;
                            }
                        LABEL_140:
                            v97 = (v87 + 4 * (v96));
                            CAM32(actor + 796) = v91;
                            v98 = CAM32(v87 + 4 * (v96));
                            v99 = v84 - v98;
                            if (v91 >= v90)
                            {
                                v101 = CAM32(actor + 852);
                                v102 = CAM32(actor + 996);
                                v103 = CAM32(actor + 876);
                                v373 = CAM32(actor + 1020);
                                v104 = CAM32(v87) - (v98 - 411774);
                            }
                            else
                            {
                                v100 = (actor + 4 * (v91 + 1));
                                v101 = CAM32(v100 + 4 * (213));
                                v102 = CAM32(v100 + 4 * (249));
                                v103 = CAM32(v100 + 4 * (219));
                                v104 = CAM32(v97 + 4 * (1)) - v98;
                                v373 = CAM32(v100 + 4 * (255));
                            }
                            if (v99 <= 0)
                            {
                                if (v104 > 0)
                                {
                                    v99 = v98 - v84;
                                    v106 = v104;
                                    goto LABEL_148;
                                }
                                v99 = v98 - v84;
                                v105 = -v104;
                            }
                            else
                            {
                                v105 = v104;
                                if (v104 <= 0)
                                {
                                    v106 = -v104;
                                LABEL_148:
                                    v107 = -camera_881B4_div(v99, v106);
                                LABEL_151:
                                    camera_881B4_coefficients(v107, camera_coeff_a);
                                    v108 = (actor + 4 * v91);
                                    v110 = actor + (-v85 & 0xC) + 624;
                                    v109 = camera_881B4_mul(CAM32(v108 + 4 * (213)), camera_coeff_a[0]);
                                    v111 = v101;
                                    v112 = v109;
                                    v114 = camera_881B4_mul(v111, camera_coeff_a[1]);
                                    v113 = camera_881B4_mul(CAM32(v108 + 4 * (249)), camera_coeff_a[2]);
                                    v115 = v102;
                                    v117 = v113;
                                    v116 = camera_881B4_mul(v115, camera_coeff_a[3]);
                                    CAM32(v110 + 4) = v112 + v114 + camera_881B4_mul(v104, v117 + v116);
                                    v119 = camera_881B4_mul(CAM32(v108 + 4 * (219)), camera_coeff_a[0]);
                                    v118 = camera_881B4_mul(v103, camera_coeff_a[1]);
                                    v120 = CAM32(v108 + 4 * (255));
                                    v121 = v118;
                                    v123 = camera_881B4_mul(v120, camera_coeff_a[2]);
                                    v122 = camera_881B4_mul(v373, camera_coeff_a[3]);
                                    CAM32(v110 + 8) = v119 + v121 + camera_881B4_mul(v104, v123 + v122);
                                    if (!v85)
                                    {
                                        v124 = CAM32(actor + 924);
                                        v125 = CAM32(actor + 948);
                                        CAM32(actor + 660) = CAM32(actor + 900);
                                        CAM32(actor + 664) = v124;
                                        CAM32(actor + 668) = v125;
                                    LABEL_157:
                                        if (v85)
                                        {
                                            sub_8008676C(actor, delta);
                                            if ((CAM32(actor + 548) & 8) == 0)
                                            {
                                                v128 = CAM32(actor + 924);
                                                v129 = CAM32(actor + 948);
                                                CAM32(actor + 660) = CAM32(actor + 900);
                                                CAM32(actor + 664) = v128;
                                                CAM32(actor + 668) = v129;
                                            }
                                        }
                                        goto LABEL_188;
                                    }
                                    CAM32(actor + 676) = 0;
                                    CAM32(actor + 672) = 0;
                                LABEL_156:
                                    CAM32(actor + 680) = sub_8001FCB0(((int16_t)((uint32_t)camera_delta[0] >> 16)), ((int16_t)((uint32_t)camera_delta[1] >> 16)), ((int16_t)((uint32_t)camera_delta[2] >> 16))) << 16;
                                    goto LABEL_157;
                                }
                            }
                            v107 = camera_881B4_div(v99, v105);
                            goto LABEL_151;
                        }
                    }
                    else
                    {
                        v93 = v89 + 1;
                        if (v93 < v90)
                        {
                            v94 = (v87 + 4 * (v93));
                            do
                            {
                                if (v84 < CAM32(v94))
                                    break;
                                ++v93;
                                (v94 += 4);
                            } while (v93 < v90);
                        }
                        v91 = v93 - 1;
                    }
                }
                else
                {
                    v84 += 411774;
                    v91 = v86 - 1;
                }
            LABEL_139:
                v96 = v91;
                goto LABEL_140;
            }
            if (v85)
            {
                v126 = CAM32(actor + 852);
                v127 = CAM32(actor + 876);
                CAM32(actor + 676) = 0;
                CAM32(actor + 672) = 0;
                CAM32(actor + 640) = v126;
                CAM32(actor + 644) = v127;
                goto LABEL_156;
            }
        LABEL_188:
            v183 = CAM32(actor + 624);
            if (v183)
            {
                v184 = ((uint32_t)(163 * v183) >> 14) & 0xFFF;
                v185 = camera_881B4_mul(CAM32(actor + 632), 16 * (int16_t)CAM16(0x8005624Cu + 2 * (2 * v184)));
                *v372 = v185;
                v186 = camera_881B4_mul(CAM32(actor + 632), 16 * (int16_t)CAM16(0x8005624Cu + 2 * (2 * v184 + 1)));
                v372[2] = v186;
            }
            else
            {
                *v372 = 0;
                camera_vector[2] = CAM32(actor + 632);
            }
            goto LABEL_246;
        }
        v130 = CAM32(actor + 548);
        if ((v130 & 2) != 0)
        {
            if ((v130 & 4) != 0)
            {
                v131 = CAM32(actor + 852);
                v132 = CAM32(actor + 876);
                v133 = CAM32(actor + 900);
                v134 = CAM32(actor + 924);
                v135 = CAM32(actor + 948);
                CAM32(actor + 624) = CAM32(actor + 828);
                v136 = CAM32(actor + 548);
                CAM32(actor + 628) = v131;
                CAM32(actor + 632) = v132;
                CAM32(actor + 660) = v133;
                CAM32(actor + 664) = v134;
                CAM32(actor + 668) = v135;
                v137 = v136 & 0xFFFFEFE7;
            }
            else
            {
                if ((v130 & 0x40) == 0)
                {
                    sub_80086584(actor, actor + 572, actor + 184, actor + 624);
                    sub_80086584(actor, actor + 572, actor + 556, actor + 660);
                    sub_800863EC((actor + 624));
                }
                v138 = CAM32(actor + 628);
                v139 = CAM32(actor + 632);
                v140 = CAM32(actor + 660);
                v141 = CAM32(actor + 664);
                v142 = CAM32(actor + 668);
                v137 = CAM32(actor + 548) | 0x3018;
                CAM32(actor + 828) = CAM32(actor + 624);
                CAM32(actor + 852) = v138;
                CAM32(actor + 876) = v139;
                CAM32(actor + 900) = v140;
                CAM32(actor + 924) = v141;
                CAM32(actor + 948) = v142;
            }
            CAM32(actor + 548) = v137;
            v143 = CAM32(actor + 800) < 3;
            CAM32(actor + 548) = CAM32(actor + 548) & 0xFFFFFF9F | 0x40;
            if (!v143)
            {
                v144 = 0;
                v145 = 972;
                v146 = 828;
                v147 = actor + 804;
                do
                {
                    v148 = actor + v146;
                    v149 = actor + v145;
                    v145 += 24;
                    v146 += 24;
                    ++v144;
                    camera_881B4_spline(rrj_host_context(), NULL, v147, v148, v149, CAM32(actor + 800) - 1);
                    v147 = actor + 804;
                } while (v144 < 6);
            }
            v150 = CAM32(actor + 552);
            CAM32(actor + 796) = 0;
            CAM32(actor + 788) = 0;
            CAM32(actor + 552) = v150 & 0xFFFFFFDF;
        }
        if (CAM32(actor + 800) < 2)
        {
        LABEL_187:
            sub_80087420(actor, delta, call);
            goto LABEL_188;
        }
        v151 = CAM32(actor + 796);
        v152 = CAM32(actor + 4 * v151 + 804);
        v153 = CAM32(actor + 700) - CAM32(actor + 788);
        v154 = v151 + 1;
        if (v153 >= v152)
        {
            v155 = CAM32(actor + 800);
            CAM32(actor + 796) = v154;
            v153 = v152;
            if (v154 < v155 - 1)
            {
                v151 = CAM32(actor + 796);
                v156 = CAM32(actor + 788) + v152;
                v153 = CAM32(actor + 700) - v156;
                CAM32(actor + 788) = v156;
                v152 = CAM32(actor + 4 * v151 + 804);
            }
            else
            {
                v143 = CAM32(actor + 792) >= CAM32(actor + 700);
                CAM32(actor + 796) = v155 - 2;
                if (!v143)
                    CAM32(actor + 552) |= 0x20u;
            }
        }
        if (v153 <= 0)
        {
            v157 = -v153;
            if (v152 <= 0)
            {
                v158 = -v152;
                goto LABEL_184;
            }
            v159 = v152;
        }
        else
        {
            v157 = v153;
            if (v152 > 0)
            {
                v158 = v152;
            LABEL_184:
                v160 = camera_881B4_div(v157, v158);
            LABEL_185:
                camera_881B4_coefficients(v160, camera_coeff_b);
                v161 = 72;
                v162 = actor;
                v163 = 4 * (v151 + 1);
                v164 = 4 * v151;
                v374 = 0;
                v376 = v163;
                v375 = 4 * v151;
                do
                {
                    v165 = actor + v164;
                    v164 += 24;
                    v166 = CAM32(v165 + 828);
                    ++v374;
                    v168 = camera_881B4_mul(v166, camera_coeff_b[0]);
                    v167 = camera_881B4_mul(CAM32(actor + v163 + 828), camera_coeff_b[1]);
                    v169 = CAM32(v165 + 972);
                    v170 = v167;
                    v172 = camera_881B4_mul(v169, camera_coeff_b[2]);
                    v171 = camera_881B4_mul(CAM32(actor + v163 + 972), camera_coeff_b[3]);
                    CAM32(v162 + 624) = v168 + v170 + camera_881B4_mul(v152, v172 + v171);
                    v173 = actor + v375 + v161;
                    v163 += 24;
                    v174 = camera_881B4_mul(CAM32(v173 + 828), camera_coeff_b[0]);
                    v175 = actor + v376 + v161;
                    v161 += 24;
                    v176 = camera_881B4_mul(CAM32(v175 + 828), camera_coeff_b[1]);
                    v177 = CAM32(v173 + 972);
                    v179 = v176;
                    v178 = camera_881B4_mul(v177, camera_coeff_b[2]);
                    v180 = CAM32(v175 + 972);
                    v182 = v178;
                    v181 = camera_881B4_mul(v180, camera_coeff_b[3]);
                    CAM32(v162 + 660) = v174 + v179 + camera_881B4_mul(v152, v182 + v181);
                    v162 += 4;
                } while (v374 < 3);
                goto LABEL_187;
            }
            v159 = -v152;
        }
        v160 = -camera_881B4_div(v157, v159);
        goto LABEL_185;
    }
    if (v66 - 13 >= 2)
        goto LABEL_303;
    v253 = CAM32(actor + 548);
    v254 = CAM32(actor + 568);
    if ((v253 & 2) != 0)
    {
        if (v66 == 13)
        {
            v255 = v253 | 4;
            CAM32(actor + 548) = v255;
            if ((v255 & 4) != 0)
            {
                v256 = CAM32(actor + 852);
                v257 = CAM32(actor + 876);
                v258 = CAM32(actor + 900);
                v259 = CAM32(actor + 924);
                v260 = CAM32(actor + 948);
                CAM32(actor + 624) = CAM32(actor + 828);
                v261 = CAM32(actor + 548);
                CAM32(actor + 628) = v256;
                CAM32(actor + 632) = v257;
                CAM32(actor + 660) = v258;
                CAM32(actor + 664) = v259;
                CAM32(actor + 668) = v260;
                v262 = v261 & 0xFFFFEFE7;
            }
            else
            {
                if ((v255 & 0x40) == 0)
                {
                    sub_80086584(actor, actor + 572, actor + 184, actor + 624);
                    sub_80086584(actor, actor + 572, actor + 556, actor + 660);
                    sub_800863EC((actor + 624));
                }
                v263 = CAM32(actor + 628);
                v264 = CAM32(actor + 632);
                v265 = CAM32(actor + 660);
                v266 = CAM32(actor + 664);
                v267 = CAM32(actor + 668);
                v262 = CAM32(actor + 548) | 0x3018;
                CAM32(actor + 828) = CAM32(actor + 624);
                CAM32(actor + 852) = v263;
                CAM32(actor + 876) = v264;
                CAM32(actor + 900) = v265;
                CAM32(actor + 924) = v266;
                CAM32(actor + 948) = v267;
            }
            CAM32(actor + 548) = v262;
            v143 = CAM32(actor + 800) < 3;
            CAM32(actor + 548) = CAM32(actor + 548) & 0xFFFFFF9F | 0x40;
            if (!v143)
            {
                v268 = 0;
                v269 = 972;
                v270 = 828;
                v271 = actor + 804;
                do
                {
                    v272 = actor + v270;
                    v273 = actor + v269;
                    v269 += 24;
                    v270 += 24;
                    ++v268;
                    camera_881B4_spline(rrj_host_context(), NULL, v271, v272, v273, CAM32(actor + 800) - 1);
                    v271 = actor + 804;
                } while (v268 < 6);
            }
            v274 = CAM32(actor + 552);
            CAM32(actor + 796) = 0;
            CAM32(actor + 788) = 0;
            CAM32(actor + 552) = v274 & 0xFFFFFFDF;
        }
        else
        {
            v275 = ((uint32_t)(163 * CAM32(actor + 828)) >> 14) & 0xFFF;
            v276 = camera_881B4_mul(CAM32(actor + 876), 16 * CAM16(0x8005624Cu + ((4 * v275) | 2)));
            camera_sine = camera_881B4_mul(CAM32(actor + 876), 16 * (int16_t)CAM16(0x8005624Cu + 2 * (2 * v275)));
            sub_8002ECB8(rrj_at(v254 + 450, 6), rrj_at(v254 + 814, 6), rrj_at(actor + 184, 12), v276, camera_sine);
            sub_8002EAD8(rrj_at(actor + 184, 12), rrj_at(v254 + 522, 6), CAM32(actor + 852), rrj_at(actor + 184, 12));
            v277 = 0;
            v278 = actor + 72;
            v279 = actor;
            v280 = CAM32(actor + 576);
            v281 = CAM32(actor + 580);
            CAM32(actor + 184) += CAM32(actor + 572);
            v282 = CAM32(actor + 192) + v281;
            CAM32(actor + 188) += v280;
            CAM32(actor + 192) = v282;
            do
            {
                ++v277;
                v283 = CAM32(v278 + 828);
                v278 += 24;
                CAM32(v279 + 660) = v283;
                v279 += 4;
            } while (v277 < 3);
        }
        CAM16(actor + 600) = CAM16(CAM32(v254 + 340) + 14);
        CAM16(actor + 602) = CAM16(CAM32(v254 + 340) + 16);
        CAM16(actor + 604) = CAM16(CAM32(v254 + 340) + 18);
        if (CAM32(v254 + 364) < 0)
        {
            v284 = CAM16(actor + 604);
            CAM16(actor + 600) = -CAM16(actor + 600);
            v285 = CAM16(actor + 602);
            CAM16(actor + 604) = -v284;
            CAM16(actor + 602) = -v285;
        }
        CAM32(actor + 548) &= 0xFFFFFF9F;
    }
    sub_80087420(actor, delta, call);
    v286 = actor + 600;
    if (CAM32(actor + 540) == 13)
    {
        if (CAM32(actor + 800) < 2)
        {
        LABEL_300:
            v319 = actor + 4 * CAM32(actor + 800);
            CAM32(actor + 184) = CAM32(v319 + 828) + CAM32(actor + 624);
            v320 = CAM32(v319 + 852);
            v321 = actor + 4 * CAM32(actor + 800);
            CAM32(actor + 188) = v320 + CAM32(actor + 628);
            CAM32(actor + 192) = CAM32(v321 + 876) + CAM32(actor + 632);
            v286 = actor + 600;
            goto LABEL_301;
        }
        v287 = CAM32(actor + 796);
        v288 = CAM32(actor + 4 * v287 + 804);
        v289 = CAM32(actor + 700) - CAM32(actor + 788);
        v290 = v287 + 1;
        if (v289 >= v288)
        {
            v291 = CAM32(actor + 800);
            CAM32(actor + 796) = v290;
            v289 = v288;
            if (v290 < v291 - 1)
            {
                v287 = CAM32(actor + 796);
                v292 = CAM32(actor + 788) + v288;
                v289 = CAM32(actor + 700) - v292;
                CAM32(actor + 788) = v292;
                v288 = CAM32(actor + 4 * v287 + 804);
            }
            else
            {
                v143 = CAM32(actor + 792) >= CAM32(actor + 700);
                CAM32(actor + 796) = v291 - 2;
                if (!v143)
                    CAM32(actor + 552) |= 0x20u;
            }
        }
        if (v289 <= 0)
        {
            v293 = -v289;
            if (v288 <= 0)
            {
                v294 = -v288;
                goto LABEL_297;
            }
            v295 = v288;
        }
        else
        {
            v293 = v289;
            if (v288 > 0)
            {
                v294 = v288;
            LABEL_297:
                v296 = camera_881B4_div(v293, v294);
            LABEL_298:
                camera_881B4_coefficients(v296, camera_coeff_c);
                v297 = 72;
                v298 = actor;
                v299 = 4 * (v287 + 1);
                v300 = 4 * v287;
                v377 = 0;
                v379 = v299;
                v378 = 4 * v287;
                do
                {
                    v301 = actor + v300;
                    v300 += 24;
                    v302 = CAM32(v301 + 828);
                    ++v377;
                    v304 = camera_881B4_mul(v302, camera_coeff_c[0]);
                    v303 = camera_881B4_mul(CAM32(actor + v299 + 828), camera_coeff_c[1]);
                    v305 = CAM32(v301 + 972);
                    v306 = v303;
                    v308 = camera_881B4_mul(v305, camera_coeff_c[2]);
                    v307 = camera_881B4_mul(CAM32(actor + v299 + 972), camera_coeff_c[3]);
                    CAM32(v298 + 624) = v304 + v306 + camera_881B4_mul(v288, v308 + v307);
                    v309 = actor + v378 + v297;
                    v299 += 24;
                    v310 = camera_881B4_mul(CAM32(v309 + 828), camera_coeff_c[0]);
                    v311 = actor + v379 + v297;
                    v297 += 24;
                    v312 = camera_881B4_mul(CAM32(v311 + 828), camera_coeff_c[1]);
                    v313 = CAM32(v309 + 972);
                    v315 = v312;
                    v314 = camera_881B4_mul(v313, camera_coeff_c[2]);
                    v316 = CAM32(v311 + 972);
                    v318 = v314;
                    v317 = camera_881B4_mul(v316, camera_coeff_c[3]);
                    CAM32(v298 + 660) = v310 + v315 + camera_881B4_mul(v288, v318 + v317);
                    v298 += 4;
                } while (v377 < 3);
                goto LABEL_300;
            }
            v295 = -v288;
        }
        v296 = -camera_881B4_div(v293, v295);
        goto LABEL_298;
    }
LABEL_301:
    sub_8002ECB8(rrj_at(v286, 6), rrj_at(actor + 588, 6), rrj_at(actor + 556, 12), CAM32(actor + 668), CAM32(actor + 660));
    sub_8002EAD8(rrj_at(actor + 556, 12), rrj_at(actor + 594, 6), CAM32(actor + 664), rrj_at(actor + 556, 12));
LABEL_302:
    v322 = CAM32(actor + 576);
    v323 = CAM32(actor + 580);
    CAM32(actor + 556) += CAM32(actor + 572);
    v324 = CAM32(actor + 564) + v323;
    CAM32(actor + 560) += v322;
    CAM32(actor + 564) = v324;
LABEL_303:
    v325 = CAM32(actor + 540);
    CAM32(actor + 548) &= 0xFFFFFFF9;
    if (v325 != 6)
        sub_80086E1C(actor, call);
    sub_800374D4(actor, 1);
    sub_80088140(actor);
    if ((CAM32(actor + 548) & 0x80000) != 0)
    {
        v326 = 0x20000;
        if (((CAM32(actor + 36) >> 8) & 1) == 0)
            v326 = (sub_8001FC58() & 0x1FFFF) + 0x20000;
        v327 = 0;
        if (((CAM32(actor + 36) >> 8) & 1) != 0)
            v327 = CAM32(actor + 388) & 1;
        v328 = 0;
        if (CAM32(actor + 344) <= 0)
            v329 = CAM32(actor + 400);
        else
            v329 = CAM32(actor + 412);
        v330 = -v326;
        while (1)
        {
            if (v327)
            {
                v331 = camera_881B4_mul(16 * CAM16(CAM32(actor + 340) + 14), CAM32(actor + 348));
                v332 = CAM32(actor + 340);
                CAM32(actor + 184) = CAM32(v332 + 20) + v331;
                v333 = camera_881B4_mul(16 * CAM16(v332 + 18), CAM32(actor + 348));
                v334 = CAM32(actor + 340);
                v335 = CAM32(v334 + 28);
                CAM32(actor + 344) = v329;
                CAM32(actor + 192) = v335 + v333;
                v336 = camera_881B4_mul(16 * CAM16(v334 + 2), v329);
                v337 = CAM32(actor + 340);
                CAM32(actor + 184) += v336;
                CAM32(actor + 192) += camera_881B4_mul(16 * CAM16(v337 + 6), CAM32(actor + 344));
            }
            v338 = CAM32(actor + 184) - CAM32(CAM32(actor + 340) + 20);
            camera_position[0] = v338;
            v339 = -CAM32(CAM32(actor + 340) + 24);
            camera_position[1] = v339;
            v340 = CAM32(actor + 192) - CAM32(CAM32(actor + 340) + 28);
            camera_position[2] = v340;
            v341 = CAM32(actor + 340);
            v342 = CAM16(v341 + 2 * (5));
            v343 = 16 * v342;
            if (16 * v342 >= -6552)
                v343 = -6553;
            v344 = v338 * (int64_t)(16 * CAM16(v341 + 2 * (4)));
            v380 = v340 * (int64_t)(16 * CAM16(v341 + 2 * (6)));
            v345 = (int32_t)((v380 >> 16) + ((v339 * (int64_t)(16 * v342)) >> 16) + (v344 >> 16));
            if (v345 <= 0)
            {
                v347 = v343;
                if (v343 <= 0)
                {
                    v345 = -v345;
                    v346 = -v343;
                LABEL_325:
                    v348 = v330 - camera_881B4_div(v345, v346);
                    goto LABEL_326;
                }
                v345 = -v345;
            }
            else
            {
                v346 = v343;
                if (v343 > 0)
                    goto LABEL_325;
                v347 = -v343;
            }
            v348 = v330 + camera_881B4_div(v345, v347);
        LABEL_326:
            CAM32(actor + 188) = v348;
            sub_800374D4(actor, 1);
            sub_80088140(actor);
            if ((CAM32(actor + 388) & 1) != 0)
            {
                v349 = rrj_surface_hit_local(rrj_host_context(), actor, 0, (uint32_t *)camera_position, camera_normal, rrj_read32(actor + 536));
                CAM32(actor + 536) = v349;
            }
            else
            {
                v349 = 0;
                if (CAM32(actor + 372))
                    CAMU8(actor + 534) = CAMU8(actor + 394);
                else
                    CAMU8(actor + 534) = 1;
                CAM32(actor + 536) = 0;
            }
            if (v349 < 0 && !CAM32(actor + 480))
            {
                CAM32(actor + 480) = 117188;
                sub_8002EE50(117188, rrj_at(actor + 450, 6), rrj_at(actor + 456, 12));
            }
            v350 = 0x10000;
            if (v329 > 0)
                v350 = -65536;
            v329 += v350;
            v327 = 1;
            ++v328;
            if (v349 >= 0 || v328 >= 10)
            {
                sub_80086E1C(actor, call);
                CAM32(actor + 548) &= ~0x80000u;
                break;
            }
        }
    }
    result = CAM32(actor + 548) & 0xFFFFBFFF;
    CAM32(actor + 548) = result;
    return result;
}

#undef CAM32
#undef CAM16
#undef CAMU16
#undef CAM8
#undef CAMU8

static void leaf_frontier_load_point(RRJMemory *m, uint32_t actor, uint32_t slot, int secondary, int32_t kind, int32_t value, unsigned shift)
{
    int32_t base = (int16_t)rrj_u16(rrj_at(0x800D85BC + 2 * kind, 2));
    int32_t index = base + (kind < 2 ? 2 * value : value);
    uint32_t point = 0x800D85C4 + 6 * index;
    unsigned i;

    for (i = 0; i < 3; ++i)
    {
        int32_t coordinate = (int16_t)rrj_u16(rrj_at(point + 2 * i, 2));
        int32_t tangent = (int16_t)rrj_u16(rrj_at(point + 6 + 2 * i, 2));
        uint32_t coordinate_base = secondary ? 972 : 828;
        uint32_t tangent_base = secondary ? 1044 : 900;
        rrj_write32(actor + coordinate_base + 24 * i + 4 * slot, (uint32_t)(coordinate << shift));
        rrj_write32(actor + tangent_base + 24 * i + 4 * slot, kind < 2 ? (uint32_t)(tangent << 8) : 0);
    }
}

uint32_t sub_800853E4(uint32_t actor, uint32_t descriptor)
{
    int32_t mode = (int8_t)r_u8(descriptor + 1);
    uint32_t cursor = descriptor + 2;
    uint32_t slot = 0;

    FUNCTION_MARKER(0x800853E4, "RASHCDG.BIN");
    rrj_write32(actor + 548, rrj_read32(actor + 548) | 6);
    if (mode != 0)
        return 0;
    {
        int32_t kind = (int8_t)r_u8(cursor);
        if (kind == 0 || kind == 2)
        {
            int32_t value = (int8_t)r_u8(cursor + 1);
            leaf_frontier_load_point(rrj_host_context(), actor, 0, 0, kind, value, 8);
            cursor += 2;
        }
        else
            rrj_write32(actor + 548, rrj_read32(actor + 548) & ~4u);
    }
    {
        int32_t kind = (int8_t)r_u8(cursor);
        if (kind == 1 || kind == 3)
        {
            int32_t value = (int8_t)r_u8(cursor + 1);
            leaf_frontier_load_point(rrj_host_context(), actor, 0, 1, kind, value, 8);
            cursor += 2;
        }
        else
        {
            unsigned i;
            for (i = 0; i < 6; ++i)
                rrj_write32(actor + 972 + 24 * i, 0);
        }
    }
    if ((int8_t)r_u8(cursor) == 5)
    {
        rrj_write32(actor + 804, (uint32_t)(int8_t)r_u8(cursor + 1) << 16);
        cursor += 2;
    }
    else
        rrj_write32(actor + 804, 0);
    while (r_u8(cursor) == 0 || r_u8(cursor) == 2)
    {
        int32_t kind = (int8_t)r_u8(cursor);
        int32_t value = (int8_t)r_u8(cursor + 1);
        ++slot;
        leaf_frontier_load_point(rrj_host_context(), actor, slot, 0, kind, value, 8);
        cursor += 2;
        if ((int8_t)r_u8(cursor) == 5)
        {
            rrj_write32(actor + 804 + 4 * slot, (uint32_t)(int8_t)r_u8(cursor + 1) << 16);
            cursor += 2;
        }
    }
    if ((int8_t)r_u8(cursor - 2) == 5)
    {
        uint32_t tail = slot + 1;
        int32_t kind = (int8_t)r_u8(cursor);
        if (kind == 1 || kind == 3)
        {
            int32_t value = (int8_t)r_u8(cursor + 1);
            leaf_frontier_load_point(rrj_host_context(), actor, tail, 1, kind, value, 8);
            cursor += 2;
        }
        else
        {
            unsigned i;
            for (i = 0; i < 6; ++i)
                rrj_write32(actor + 972 + 24 * i + 4 * tail, 0);
        }
        kind = (int8_t)r_u8(cursor);
        if (kind != 0 && kind != 2)
        {
            uint32_t defaults = 0x800CD7B8 + 56 * rrj_read32(actor + 544);
            rrj_write32(actor + 828 + 4 * tail, 0);
            rrj_write32(actor + 900 + 4 * tail, 0);
            rrj_write32(actor + 924 + 4 * tail, 0);
            rrj_write32(actor + 852 + 4 * tail, rrj_read32(defaults + 12));
            rrj_write32(actor + 876 + 4 * tail, rrj_read32(defaults + 4));
            rrj_write32(actor + 948 + 4 * tail, rrj_read32(defaults + 20));
        }
        slot = tail;
    }
    rrj_write32(actor + 800, slot + 1);
    rrj_write32(actor + 540, 11);
    return 11;
}

static int32_t leaf_frontier_angle_delta(int32_t value, int32_t previous)
{
    int32_t delta = value - previous;
    if (delta >= 2049)
        return previous + 4096;
    if (delta < -2048)
        return previous - 4096;
    return previous;
}

uint32_t sub_80086E1C(uint32_t actor, RRJRaceLeafCall call)
{
    int32_t dx;
    int32_t dy;
    int32_t dz;
    int32_t horizontal;
    int32_t pitch;
    int32_t yaw;
    uint32_t flags;
    uint32_t angles = 0x1F8003E0;

    FUNCTION_MARKER(0x80086E1C, "RASHCDG.BIN");
    flags = rrj_read32(actor + 548);
    if (!rrj_read32(actor + 772) && (flags & 0x4000))
        return leaf_frontier_call(rrj_host_context(), call, 0x80086E5C, actor);
    dx = rrj_s32(rrj_read32(actor + 184) - rrj_read32(actor + 556));
    dy = rrj_s32(rrj_read32(actor + 188) - rrj_read32(actor + 560));
    dz = rrj_s32(rrj_read32(actor + 192) - rrj_read32(actor + 564));
    yaw = rrj_s32(sub_80020018((uint32_t)-dx, rrj_read32(actor + 564) - rrj_read32(actor + 192)));
    horizontal = rrj_s32(sub_8004CF74((uint32_t)((int32_t)sub_8001FC90(dx, dx) + (int32_t)sub_8001FC90(dz, dz))));
    pitch = rrj_s32(sub_80020018((uint32_t)dy, (uint32_t)(4 * horizontal)));
    if (flags & 0x1000)
    {
        int32_t previous = rrj_s32(rrj_read32(actor + 748));
        previous = leaf_frontier_angle_delta(pitch, previous);
        rrj_write32(actor + 748, (uint32_t)previous);
        if ((((pitch - previous) >> 31) + pitch - previous ^ ((pitch - previous) >> 31)) < 3)
        {
            rrj_write32(actor + 748, (uint32_t)pitch);
            flags &= ~0x1000u;
        }
        else
            rrj_write32(actor + 748, (uint32_t)((3 * previous + pitch) / 4));
    }
    else
        rrj_write32(actor + 748, (uint32_t)pitch);
    if (flags & 0x2000)
    {
        int32_t previous = rrj_s32(rrj_read32(actor + 744));
        previous = leaf_frontier_angle_delta(yaw, previous);
        rrj_write32(actor + 744, (uint32_t)previous);
        if ((((yaw - previous) >> 31) + yaw - previous ^ ((yaw - previous) >> 31)) < 23)
        {
            rrj_write32(actor + 744, (uint32_t)yaw);
            flags &= ~0x2000u;
        }
        else if (flags & 8)
            rrj_write32(actor + 744, (uint32_t)((3 * previous + yaw) / 4));
        else
            rrj_write32(actor + 744, (uint32_t)((previous + yaw) / 2));
    }
    else
        rrj_write32(actor + 744, (uint32_t)yaw);
    rrj_write32(actor + 548, flags);
    rrj_put16(rrj_at(angles, 2), (uint16_t)-rrj_u16(rrj_at(actor + 748, 2)));
    rrj_put16(rrj_at(angles + 2, 2), (uint16_t)-rrj_u16(rrj_at(actor + 744, 2)));
    rrj_put16(rrj_at(angles + 4, 2), 0);
    return sub_8004D2A4(angles, actor + 432);
}

uint32_t sub_80088140(uint32_t actor)
{
    uint32_t prior = (rrj_read32(actor + 36) >> 8) & 1;
    uint32_t flags;
    uint32_t result;

    FUNCTION_MARKER(0x80088140, "RASHCDG.BIN");
    rrj_write32(actor + 344, 0u - rrj_read32(actor + 344));
    (void)sub_8003A9D8(actor);
    flags = rrj_read32(actor + 36);
    result = (((flags >> 8) & 1) | prior) << 8;
    rrj_write32(actor + 344, 0u - rrj_read32(actor + 344));
    rrj_write32(actor + 36, (flags & ~0x100u) | result);
    return result;
}

uint32_t sub_8008CFDC(void)
{
    uint32_t actor = rrj_read32(0x800CE4D0);
    uint32_t stride = rrj_read32(0x800CE4D4);
    uint32_t count_pointer = rrj_read32(0x800CE4DC);
    int32_t count = rrj_s32(rrj_read32(count_pointer));
    uint32_t players = rrj_read32(rrj_read32(0x8005B2F8) + 48);
    uint16_t prior_second = rrj_u16(rrj_at(0x800CD542, 2));
    uint16_t prior_first = (uint16_t)(rrj_u16(rrj_at(0x800CD540, 2)) - prior_second);

    FUNCTION_MARKER(0x8008CFDC, "RASHCDG.BIN");
    rrj_put16(rrj_at(0x800CD540, 2), 0);
    rrj_put16(rrj_at(0x800CD542, 2), 0);
    while (count-- >= 0)
    {
        if (rrj_u16(rrj_at(actor + 320, 2)))
        {
            unsigned player_index;
            (void)(uint32_t)sub_8008DBCC(actor);
            (void)(uint32_t)sub_800667C4(actor, players);
            for (player_index = 0; player_index < players; ++player_index)
            {
                uint32_t player = 0x800CD898 + 1132 * player_index;
                uint32_t counter = 0x800CD540 + 2 * player_index;
                uint16_t actor_flags = rrj_u16(rrj_at(actor + 320, 2));
                if (rrj_read32(player + 548) & 1)
                    rrj_put16(rrj_at(counter, 2), player_index ? prior_second : prior_first);
                else
                {
                    int32_t distance = rrj_s32(rrj_read32(actor + 324) - rrj_read32(player + 324));
                    uint32_t close = distance < 8192;
                    int32_t visibility = rrj_s32(rrj_read32(actor + 44 + 4 * player_index));
                    actor_flags = (uint16_t)(actor_flags & ~(16u << player_index));
                    rrj_put16(rrj_at(actor + 320, 2), actor_flags);
                    if (visibility < (close ? 4096 : 768))
                    {
                        uint8_t level;
                        if (!close)
                            visibility += 4096;
                        level = (uint8_t)(visibility >> 6);
                        if ((actor_flags & 0x10) && r_u8(actor + 850) < level)
                            level = r_u8(actor + 850);
                        w_u8(actor + 850, level);
                        rrj_put16(rrj_at(counter, 2), (uint16_t)(rrj_u16(rrj_at(counter, 2)) + 1));
                        rrj_put16(rrj_at(actor + 320, 2), (uint16_t)(rrj_u16(rrj_at(actor + 320, 2)) | (16u << player_index)));
                    }
                }
            }
        }
        {
            uint32_t body = rrj_read32(actor + 852);
            if (rrj_u16(rrj_at(body + 320, 2)))
            {
                if (rrj_read32(body + 604) >= 3)
                    (void)(uint32_t)sub_8008DBCC(body);
                else
                {
                    rrj_write32(body + 44, rrj_read32(actor + 44));
                    rrj_write32(body + 48, rrj_read32(actor + 48));
                }
                (void)(uint32_t)sub_800667C4(body, players);
            }
        }
        actor += stride;
    }
    rrj_put16(rrj_at(0x800CD540, 2), (uint16_t)(rrj_u16(rrj_at(0x800CD540, 2)) + rrj_u16(rrj_at(0x800CD542, 2))));
    {
        static const uint32_t list_bases[] = {0x800CE4F0, 0x800CE500, 0x800CE510, 0x800CE520};
        static const uint32_t list_strides[] = {0x800CE4F4, 0x800CE504, 0x800CE514, 0x800CE524};
        static const uint32_t list_counts[] = {0x800CE4FC, 0x800CE50C, 0x800CE51C, 0x800CE52C};
        unsigned list_index;
        for (list_index = 0; list_index < 4; ++list_index)
        {
            uint32_t item = rrj_read32(list_bases[list_index]);
            uint32_t item_stride = rrj_read32(list_strides[list_index]);
            int32_t item_count = rrj_s32(rrj_read32(rrj_read32(list_counts[list_index])));
            while (item_count-- >= 0)
            {
                if (rrj_u16(rrj_at(item + 172, 2)) && rrj_u16(rrj_at(item + 320, 2)))
                {
                    (void)(uint32_t)sub_8008DBCC(item);
                    if (list_index < 2)
                        (void)(uint32_t)sub_800667C4(item, players);
                }
                item += item_stride;
            }
        }
    }
    if (rrj_read32(0x8005B314))
    {
        unsigned i;
        uint32_t item = rrj_read32(0x8005B304);
        for (i = 0; i < 3; ++i, item += 280)
        {
            if (rrj_u16(rrj_at(item + 172, 2)))
                (void)(uint32_t)sub_8008DBCC(item);
        }
    }
    return 0;
}

uint32_t sub_8008DBCC(uint32_t actor)
{
    uint32_t players = rrj_read32(rrj_read32(0x8005B2F8) + 48);
    uint32_t player = 0x800CD898;
    unsigned i;

    FUNCTION_MARKER(0x8008DBCC, "RASHCDG.BIN");
    rrj_write32(actor + 48, 0x7FFFFFFF);
    for (i = 0; i < players; ++i)
    {
        int32_t dx = rrj_s32(rrj_read32(actor + 184) - rrj_read32(player + 184)) >> 10;
        int32_t dy = rrj_s32(rrj_read32(actor + 188) - rrj_read32(player + 188)) >> 10;
        int32_t dz = rrj_s32(rrj_read32(actor + 192) - rrj_read32(player + 192)) >> 10;
        int32_t distance = rrj_s32(sub_8001FCB0((uint32_t)dx, (uint32_t)dy, (uint32_t)dz));
        rrj_write32(actor + 44 + 4 * i, distance < 0 ? 0x7FFFFFFF : (uint32_t)distance);
        player += 1132;
    }
    return i < players;
}

uint32_t sub_800667C4(uint32_t actor, uint32_t players)
{
    uint32_t shape = rrj_read32(actor);
    uint32_t mode = (rrj_u16(rrj_at(shape + 14, 2)) & 0x78) >> 3;
    unsigned i;
    int32_t selected;

    FUNCTION_MARKER(0x800667C4, "RASHCDG.BIN");
    for (i = 0; i < players; ++i)
    {
        uint32_t ranges = rrj_read32(actor + 100);
        int32_t index = (int8_t)r_u8(actor + 10 + i);
        int32_t distance = rrj_s32(rrj_read32(actor + 44 + 4 * i));
        if (ranges)
        {
            uint32_t descriptor = rrj_read32(actor + 96);
            int32_t last = (int32_t)r_u8(descriptor + 4) - 1;
            if (index < last && rrj_s32(rrj_read32(ranges + 8 * index)) < distance)
            {
                do
                    ++index;
                while (rrj_s32(rrj_read32(ranges + 8 * index)) < distance);
            }
            else if (index)
            {
                while (distance < rrj_s32(rrj_read32(ranges + 8 * index + 4)))
                    --index;
            }
            if (index > last)
                index = last;
            w_u8(actor + 10 + i, (uint8_t)index);
        }
    }
    selected = (int8_t)r_u8(actor + 10);
    if (players >= 2)
    {
        int32_t second = (int8_t)r_u8(actor + 11);
        if (selected >= second)
            selected = second;
    }
    if (mode == 1 || mode == 4)
    {
        uint32_t visible = 0;
        uint32_t body;
        if (!selected || (selected == 1 && mode == 1 && ((uint32_t)(rrj_u16(rrj_at(actor + 172, 2)) & 0x1F) < players || (r_u8(actor + 572) & 0x20))))
            visible = 1;
        body = rrj_read32(actor + 540);
        rrj_write32(body + 1760, rrj_read32(0x80052390 + 4 * (mode == 4 ? selected + 1 : selected)));
        rrj_write32(body + 36, (rrj_read32(body + 36) & ~4u) | 4 * visible);
    }
    {
        uint32_t result = r_u8(actor + 9) & 0xF7;
        w_u8(actor + 9, (uint8_t)result);
        return result;
    }
}

uint32_t sub_8005E1D8(uint32_t context)
{
    uint32_t enabled = rrj_read32(context + 8);
    int32_t count = rrj_s32(rrj_read32(context + 12));
    uint32_t actor = rrj_read32(context);
    int32_t i;
    uint32_t result = enabled;

    FUNCTION_MARKER(0x8005E1D8, "RASHCDG.BIN");
    if (!enabled || count <= 0)
        return result;
    for (i = 0; i < count; ++i, actor += 2108)
    {
        uint32_t flags = rrj_read32(actor + 36);
        if (flags & 2)
        {
            uint32_t shape = rrj_read32(actor);
            if ((r_u8(shape + 9) & 3) || !(flags & 0x10))
                (void)(uint32_t)sub_8005D2A8(actor);
            if (!(rrj_read32(actor + 36) & 8))
                (void)(uint32_t)sub_8005CB04(actor, rrj_read32(rrj_read32(0x8005B2F8) + 28));
        }
        result = (uint32_t)(i + 1 < count);
    }
    return result;
}

uint32_t sub_8005D2A8(uint32_t actor)
{
    uint32_t descriptor = rrj_read32(actor + 4) + 12 * rrj_read32(actor + 12);
    uint32_t result;

    FUNCTION_MARKER(0x8005D2A8, "RASHCDG.BIN");
    if (r_u8(descriptor + 1) == 3)
        (void)(uint32_t)sub_8005D36C(actor);
    else
    {
        (void)(uint32_t)sub_8005E5A4(actor, actor + 48);
        (void)(uint32_t)sub_8005D63C(actor);
    }
    rrj_write32(actor + 36, r_u8(actor + 36) | 0x10);
    result = r_u8(descriptor + 2) & 0x10;
    if (result)
    {
        uint32_t shape = rrj_read32(actor);
        w_u8(shape + 547, (uint8_t)(0u - r_u8(shape + 547)));
        result = r_u8(descriptor + 2) & 0xEF;
        w_u8(descriptor + 2, (uint8_t)result);
    }
    return result;
}

uint32_t sub_8005E5A4(uint32_t actor, uint32_t channels)
{
    int32_t frame = rrj_s32(rrj_read32(actor + 16));
    int32_t next = frame + 1;
    uint32_t source = rrj_read32(actor + 44);
    int32_t count = (int16_t)rrj_u16(rrj_at(channels + 2, 2));
    int32_t i;
    uint32_t result = (uint32_t)count;

    FUNCTION_MARKER(0x8005E5A4, "RASHCDG.BIN");
    if ((int32_t)rrj_u16(rrj_at(source + 16, 2)) - 1 < next)
        next = frame;
    for (i = 0; i < count; ++i)
    {
        uint32_t record = channels + 24 * (uint32_t)i;
        uint32_t value = record + 10;
        uint32_t flags = r_u8(value + 14);
        if (!(flags & 2))
        {
            uint32_t samples = rrj_read32(record + 4);
            rrj_put16(rrj_at(value - 2, 2), rrj_u16(rrj_at(samples + 2 * frame, 2)));
            rrj_put16(rrj_at(value, 2), rrj_u16(rrj_at(samples + 2 * next, 2)));
        }
        else if (flags & 1)
        {
            int32_t previous = (int16_t)rrj_u16(rrj_at(channels, 2));
            int32_t step;
            for (step = previous + 1; step < next; ++step)
            {
                uint32_t width = r_u8(value + 16);
                int32_t decoded;
                if (flags & 4)
                {
                    int32_t remaining = rrj_s32(rrj_read32(value + 6)) - 1;
                    decoded = (int8_t)r_u8(value + 12);
                    w_u8(value + 15, (uint8_t)decoded);
                    flags &= ~4u;
                    rrj_write32(value + 6, (uint32_t)remaining);
                    if (remaining > 0)
                        flags |= 4;
                    w_u8(value + 14, (uint8_t)flags);
                }
                else
                {
                    decoded = (int8_t)sub_8005E558(record + 4, record + 27, width);
                    w_u8(value + 15, (uint8_t)decoded);
                    if (decoded == (int8_t)r_u8(value + 13))
                    {
                        int32_t remaining = 0;
                        int32_t limit = 1 << width;
                        do
                        {
                            decoded = (int8_t)sub_8005E558(record + 4, record + 27, width);
                            w_u8(value + 15, (uint8_t)decoded);
                            if (decoded != limit - 1)
                                break;
                            remaining = remaining - 1 + limit;
                        } while (1);
                        remaining += 4 + decoded;
                        decoded = (int8_t)sub_8005E558(record + 4, record + 27, width);
                        w_u8(value + 12, (uint8_t)decoded);
                        w_u8(value + 15, (uint8_t)decoded);
                        rrj_write32(value + 6, (uint32_t)(remaining - 1));
                        flags |= 4;
                        w_u8(value + 14, (uint8_t)flags);
                    }
                }
                decoded = (int8_t)r_u8(value + 15);
                if ((decoded >> (width - 1)) & 1)
                    decoded -= 1 << width;
                rrj_write32(value + 2, rrj_read32(value + 2) + (uint32_t)(16 * decoded));
                rrj_put16(rrj_at(value - 2, 2), rrj_u16(rrj_at(value, 2)));
                rrj_put16(rrj_at(value, 2), (uint16_t)((rrj_s32(rrj_read32(value + 2)) * (int32_t)rrj_u16(rrj_at(value + 10, 2))) >> 9));
            }
            if (next == frame)
                rrj_put16(rrj_at(value - 2, 2), rrj_u16(rrj_at(value, 2)));
        }
        result = (uint32_t)(i + 1 < count);
    }
    rrj_put16(rrj_at(channels, 2), (uint16_t)frame);
    return result;
}

uint32_t sub_8005E558(uint32_t stream_pointer, uint32_t bit_pointer, uint32_t width)
{
    uint32_t bit = r_u8(bit_pointer);
    uint32_t stream = rrj_read32(stream_pointer);
    uint32_t packed = ((uint32_t)rrj_u16(rrj_at(stream, 2)) << 16) | rrj_u16(rrj_at(stream + 2, 2));

    FUNCTION_MARKER(0x8005E558, "RASHCDG.BIN");
    rrj_write32(stream_pointer, stream + (((bit + width) >> 3) & 0x1E));
    w_u8(bit_pointer, (uint8_t)((bit + width) & 15));
    return ((packed << bit) >> (32 - width)) & 0xFF;
}

uint32_t sub_8005D63C(uint32_t actor)
{
    uint32_t descriptor = rrj_read32(actor + 4) + 12 * rrj_read32(actor + 12);
    uint32_t flags = r_u8(descriptor + 2);
    uint32_t mirror = flags & 1;
    uint32_t mask = (flags & 0x40) ? UINT32_MAX : rrj_read32(actor + 1760);
    uint32_t model = rrj_read32(actor + 44);
    uint32_t object;
    uint32_t subtype = 0;
    uint32_t vector = 0x1F8003D0;
    uint32_t quaternion = 0x1F8003E0;
    int32_t blend = 0;
    int32_t x_offset = 0;
    int32_t y_offset = 0;
    int32_t root[3];
    uint32_t index;

    FUNCTION_MARKER(0x8005D63C, "RASHCDG.BIN");
    if (rrj_read32(actor + 36) & 4)
    {
        int32_t numerator = rrj_s32(rrj_read32(actor + 32) << 16);
        int32_t denominator = rrj_s32(rrj_read32(actor + 24));

        blend = denominator == 0 ? (numerator < 0 ? 1 : -1) : (numerator == INT32_MIN && denominator == -1 ? INT32_MIN : numerator / denominator);
    }
    object = rrj_read32(actor);
    if (((rrj_read32(object + 36) >> 18) & 1) && rrj_read32(object + 52))
    {
        x_offset = 1148;
        y_offset = rrj_s32(rrj_read32(object + 76)) > 65535 ? 10 : 140;
    }
    else if (((rrj_u16(rrj_at(rrj_read32(object) + 14, 2)) & 0x78) >> 3) == 1)
    {
        uint32_t linked = rrj_read32(object + 52);

        if (linked)
            subtype = (rrj_u16(rrj_at(rrj_read32(linked) + 14, 2)) & 0xF80) >> 7;
    }
    for (index = 0; index < 3; ++index)
        root[index] = (int16_t)rrj_u16(rrj_at(actor + 56 + 24 * index, 2));
    if (blend)
    {
        int32_t next[3];
        int32_t inverse = rrj_s32(0x10000u - (uint32_t)blend);

        for (index = 0; index < 3; ++index)
            next[index] = (int16_t)rrj_u16(rrj_at(actor + 58 + 24 * index, 2));
        for (index = 0; index < 3; ++index)
        {
            uint32_t current_part = (uint32_t)sub_8001FC90(inverse, root[index]);
            uint32_t next_part = (uint32_t)sub_8001FC90(blend, next[index]);

            root[index] = (int16_t)(current_part + next_part);
        }
    }
    if (subtype == 1)
        for (index = 0; index < 3; ++index)
            root[index] = (int16_t)(root[index] + (int16_t)rrj_u16(rrj_at(0x800CC1C8 + 2 * index, 2)));
    if (mirror)
        root[0] = (int16_t)(x_offset - root[0]);
    root[1] = (int16_t)(root[1] + y_offset);
    for (index = 0; index < 3; ++index)
        rrj_put16(rrj_at(vector + 2 * index, 2), (uint16_t)root[index]);
    (void)(uint32_t)sub_80066A60(object, vector);
    for (index = 0; index < r_u8(model + 15); ++index)
    {
        uint32_t destination = mirror ? r_u8(0x800CC1B0 + index) : index;
        uint32_t mode;
        uint32_t component;
        int32_t values[4];
        int32_t fraction = (int16_t)(blend >> 2);
        int32_t inverse = 0x4000 - fraction;

        if (((mask >> (destination & 31)) & 1) == 0)
            continue;
        object = rrj_read32(actor);
        mode = (rrj_u16(rrj_at(rrj_read32(object) + 14, 2)) & 0x78) >> 3;
        for (component = 0; component < 4; ++component)
        {
            uint32_t source = actor + 128 + 96 * index + 24 * component;
            int32_t current = (int16_t)rrj_u16(rrj_at(source, 2));
            int32_t value = current;

            if (fraction)
            {
                int32_t next = (int16_t)rrj_u16(rrj_at(source + 2, 2));
                int32_t current_part = rrj_s32((uint32_t)inverse * (uint32_t)current) >> 14;
                int32_t next_part = rrj_s32((uint32_t)fraction * (uint32_t)next) >> 14;

                value = (int16_t)(current_part + next_part);
            }
            values[component] = value;
            rrj_put16(rrj_at(quaternion + 2 * component, 2), (uint16_t)value);
        }
        if ((mode == 1 || mode == 4) && subtype == 1 && ((rrj_read32(0x800CC1C4) >> (index & 31)) & 1))
        {
            (void)(uint32_t)sub_800714FC(0x800CC1CE + 8 * index, quaternion, quaternion);
            for (component = 0; component < 4; ++component)
                values[component] = (int16_t)rrj_u16(rrj_at(quaternion + 2 * component, 2));
        }
        if (mirror)
        {
            if ((mode == 1 || mode == 4) && index == 0)
            {
                int32_t first = values[0];
                int32_t second = values[1];

                values[0] = (int16_t)-values[2];
                values[1] = (int16_t)-values[3];
                values[2] = (int16_t)-first;
                values[3] = (int16_t)-second;
            }
            else if (mode == 1 || mode == 4 || mode == 5)
            {
                values[0] = (int16_t)-values[0];
                values[1] = (int16_t)-values[1];
            }
            else
            {
                values[1] = (int16_t)-values[1];
                values[2] = (int16_t)-values[2];
            }
        }
        for (component = 0; component < 4; ++component)
            rrj_write32(quaternion + 4 * component, (uint32_t)(values[component] * 4));
        object = rrj_read32(actor);
        (void)(uint32_t)sub_8001005C(rrj_read32(object + 4) + 24 * destination + 4, quaternion);
    }
    object = rrj_read32(actor);
    if (rrj_u16(rrj_at(object + 544, 2)) != 42)
        return 42;
    {
        int32_t numerator = rrj_s32(rrj_read32(actor + 32) << 16);
        int32_t denominator = rrj_s32(rrj_read32(actor + 24));
        int32_t elapsed = denominator == 0 ? (numerator < 0 ? 1 : -1) : (numerator == INT32_MIN && denominator == -1 ? INT32_MIN : numerator / denominator);
        int32_t progress = rrj_s32((rrj_read32(actor + 16) << 16) + (uint32_t)elapsed);
        uint32_t animation_table = rrj_read32(rrj_read32(actor + 40) + 4);
        uint32_t animation;
        int32_t last;
        uint32_t scale;
        int32_t angle;
        int32_t cosine;
        int32_t sine;
        uint32_t rotation[5];
        uint32_t position;

        descriptor = rrj_read32(actor + 4) + 12 * rrj_read32(actor + 12);
        animation = rrj_read32(animation_table + 4 * r_u8(descriptor));
        last = (int16_t)(rrj_u16(rrj_at(animation + 16, 2)) - 1);
        scale = rrj_read32(rrj_read32(object + 596) + 652);
        if (progress < rrj_s32((uint32_t)(last / 2) << 16))
        {
            int32_t doubled = rrj_s32((uint32_t)progress << 1);
            int32_t ratio = last == 0 ? (doubled < 0 ? 1 : -1) : (doubled == INT32_MIN && last == -1 ? INT32_MIN : doubled / last);

            scale = (uint32_t)sub_8001FC90(rrj_s32(scale), ratio);
        }
        angle = rrj_s32(652u * scale) >> 16;
        cosine = (int16_t)rrj_u16(rrj_at(0x8005624C + 4 * ((uint32_t)angle & 0xFFF) + 2, 2));
        sine = (int16_t)rrj_u16(rrj_at(0x8005624C + 4 * ((uint32_t)angle & 0xFFF), 2));
        rotation[0] = (uint16_t)cosine | ((uint32_t)(uint16_t)sine << 16);
        rotation[1] = (uint32_t)(uint16_t)-sine << 16;
        rotation[2] = (uint16_t)cosine;
        rotation[3] = 0;
        rotation[4] = 4096;
        for (index = 0; index < 5; ++index)
            xport_gte_write_control(index, rotation[index]);
        for (index = 0; index < 3; ++index)
        {
            uint32_t column = rrj_read32(rrj_read32(actor) + 4) + 4 + 2 * index;

            for (uint32_t row = 0; row < 3; ++row)
                xport_gte_write_data(9 + row, rrj_u16(rrj_at(column + 6 * row, 2)));
            xport_gte_execute(0x49E012);
            for (uint32_t row = 0; row < 3; ++row)
                rrj_put16(rrj_at(column + 6 * row, 2), (uint16_t)xport_gte_read_data(9 + row));
        }
        for (index = 0; index < 5; ++index)
            xport_gte_write_control(index, rotation[index]);
        position = rrj_read32(actor) + 28;
        xport_gte_write_data(0, rrj_read32(position));
        xport_gte_write_data(1, rrj_read32(position + 4));
        xport_gte_execute(0x486012);
        for (index = 0; index < 3; ++index)
            rrj_put16(rrj_at(position + 2 * index, 2), (uint16_t)xport_gte_read_data(9 + index));
        return position;
    }
}

uint32_t sub_80066A60(uint32_t object, uint32_t vector)
{
    uint32_t result;

    FUNCTION_MARKER(0x80066A60, "RASHCDG.BIN");
    rrj_put16(rrj_at(object + 28, 2), rrj_u16(rrj_at(vector, 2)));
    rrj_put16(rrj_at(object + 30, 2), rrj_u16(rrj_at(vector + 2, 2)));
    result = rrj_u16(rrj_at(vector + 4, 2));
    rrj_put16(rrj_at(object + 32, 2), (uint16_t)result);
    return result;
}

uint32_t sub_8005CB04(uint32_t actor, uint32_t delta)
{
    uint32_t result;

    FUNCTION_MARKER(0x8005CB04, "RASHCDG.BIN");
    result = (uint32_t)sub_8005C58C(actor, delta);
    if (result && (rrj_read32(actor + 36) & 2))
    {
        uint32_t frame = rrj_read32(actor + 12) + 1;

        rrj_write32(actor + 12, frame == rrj_read32(actor + 8) ? 0 : frame);
        result = (uint32_t)sub_8005C418(actor);
        rrj_write32(actor + 12, result);
        return (uint32_t)sub_8005C8F4(actor);
    }
    return result;
}

uint32_t sub_8005C52C(uint32_t actor)
{
    uint32_t descriptor = rrj_read32(actor + 4) + 12 * rrj_read32(actor + 12);
    uint32_t result;

    FUNCTION_MARKER(0x8005C52C, "RASHCDG.BIN");
    (void)sub_8005C39C(actor, r_u8(descriptor));
    rrj_write32(actor + 16, 0);
    rrj_write32(actor + 28, 0);
    result = rrj_read32(descriptor + 8);
    rrj_write32(actor + 1756, result);
    return result;
}

uint32_t sub_8005C58C(uint32_t actor, uint32_t delta)
{
    uint32_t base = rrj_read32(actor + 4);
    uint32_t descriptor = base + 12 * rrj_read32(actor + 12);
    uint32_t model = rrj_read32(actor + 44);
    uint32_t mode = r_u8(descriptor + 1);
    int32_t progress;

    FUNCTION_MARKER(0x8005C58C, "RASHCDG.BIN");
    if (mode == 0)
    {
        (void)(uint32_t)sub_8005C4EC(actor, delta);
        progress = rrj_s32(rrj_read32(actor + 16));
        if (progress >= (int32_t)rrj_u16(rrj_at(model + 16, 2)) - 1)
            rrj_write32(actor + 32, 0);
        if (progress < (int32_t)rrj_u16(rrj_at(model + 16, 2)))
            return 0;
        if (rrj_read32(actor + 20) != 0xFFFF)
            rrj_write32(actor + 20, rrj_read32(actor + 20) - 1);
        if (rrj_read32(actor + 20))
        {
            (void)(uint32_t)sub_8005C52C(actor);
            return 0;
        }
        return 1;
    }
    if (mode == 1)
    {
        (void)(uint32_t)sub_8005C4EC(actor, delta);
        progress = rrj_s32(rrj_read32(actor + 16));
        if (progress >= (int16_t)rrj_u16(rrj_at(descriptor + 6, 2)))
            rrj_write32(actor + 32, 0);
        return progress > (int16_t)rrj_u16(rrj_at(descriptor + 6, 2));
    }
    if (mode == 5)
        return 1;
    if (mode != 3)
        return 0;
    (void)(uint32_t)sub_8005C4EC(actor, delta);
    if (rrj_s32(rrj_read32(actor + 16)) < (int16_t)rrj_u16(rrj_at(descriptor + 4, 2)))
        return 0;
    rrj_write32(actor + 16, 0);
    rrj_write32(actor + 32, 0);
    if (r_u8(descriptor + 2) & 4)
    {
        uint8_t flags;

        rrj_write32(actor + 12, rrj_read32(actor + 8) - 1);
        w_u8(base + 2, r_u8(descriptor + 26));
        rrj_write32(base + 8, rrj_read32(descriptor + 32));
        w_u8(base, r_u8(descriptor + 28));
        flags = r_u8(base + 2);
        w_u8(base + 3, (flags & 8) ? r_u8(descriptor + 27) : 0);
        if (r_u8(descriptor) == 1)
        {
            w_u8(base + 1, 1);
            rrj_put16(rrj_at(base + 4, 2), 0);
            rrj_put16(rrj_at(base + 6, 2), rrj_u16(rrj_at(model + 16, 2)) - 1);
            w_u8(base + 13, 5);
        }
        else
        {
            w_u8(base + 1, 0);
            rrj_put16(rrj_at(base + 4, 2), UINT16_MAX);
            rrj_put16(rrj_at(base + 6, 2), 0);
            w_u8(base + 13, 2);
        }
        w_u8(base + 14, flags);
        rrj_write32(base + 20, rrj_read32(base + 8));
        w_u8(base + 12, r_u8(base));
        w_u8(base + 15, r_u8(base + 3));
        rrj_put16(rrj_at(base + 16, 2), 0);
        rrj_put16(rrj_at(base + 18, 2), 0);
    }
    return 1;
}

uint32_t sub_8005C4EC(uint32_t actor, uint32_t delta)
{
    int32_t period = rrj_s32(rrj_read32(actor + 24));
    int32_t remainder = rrj_s32(delta + rrj_read32(actor + 28));

    FUNCTION_MARKER(0x8005C4EC, "RASHCDG.BIN");
    while (remainder >= period)
    {
        remainder -= period;
        rrj_write32(actor + 16, rrj_read32(actor + 16) + 1);
    }
    rrj_write32(actor + 32, (uint32_t)remainder);
    rrj_write32(actor + 28, (uint32_t)remainder);
    return remainder < period;
}
