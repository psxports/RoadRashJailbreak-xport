#include "race_leaf_frontier.h"
#include "fixed_math.h"
#include "race_leaf_batch_000.h"
#include "race_leaf_batch_002.h"
#include "race_pause.h"
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

uint32_t sub_80087420(RRJMemory *m, uint32_t actor, int32_t delta, RRJRaceLeafCall call)
{
    uint32_t owner = rrj_read32(m, actor + 568);
    uint32_t target = rrj_read32(m, owner + 852);
    uint32_t flags = rrj_read32(m, actor + 548);
    uint32_t table = 0x800CD7B8 + 56 * rrj_read32(m, actor + 540);
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
    blocked = (rrj_read32(m, owner + 568) & 0x400) || (rrj_read32(m, owner + 564) & 0x8800);
    target_mode = rrj_read32(m, target + 604) < 3;
    alternate_axis = (flags & 0x8000) != 0;
    if ((rrj_u16(rrj_at(m, owner + 172, 2)) >> 5) || blocked || (rrj_read32(m, owner + 568) & 0x100) || alternate_axis || (flags & 0x400000) || !target_mode)
        return leaf_frontier_call3(m, call, 0x80087424, actor, (uint32_t)delta, 0);
    flags &= ~0x02000000u;
    flags &= ~0x20000000u;
    rrj_write32(m, actor + 548, flags);
    phase_step = (flags & 2) ? 65536000 : 0x20000;
    phase_step = -phase_step;
    if ((flags & 0x10000000) || rrj_read32(m, actor + 1120))
        return leaf_frontier_call3(m, call, 0x80087424, actor, (uint32_t)delta, 0);
    rrj_write32(m, source, rrj_read32(m, owner + 504));
    rrj_write32(m, source + 4, rrj_read32(m, owner + 508));
    rrj_write32(m, source + 8, rrj_read32(m, owner + 512));
    phase = rrj_s32(rrj_read32(m, actor + 584)) + (int32_t)sub_8001FC90(phase_step, delta);
    if (phase < 0)
        phase = 0;
    if (phase > 0x10000)
        phase = 0x10000;
    rrj_write32(m, actor + 584, (uint32_t)phase);
    if ((flags & 0x110000) != 0x100000)
        (void)sub_8002E6F8(m, source, target + 184, actor + 572, 0x10000 - phase, phase);
    if (rrj_read32(m, owner + 568) & 0x400)
        return leaf_frontier_call3(m, call, 0x80087A48, actor, (uint32_t)delta, 0);
    if ((int16_t)rrj_u16(rrj_at(m, owner + 450, 2)) >= -408 && (int16_t)rrj_u16(rrj_at(m, owner + 450, 2)) <= 408 && (int16_t)rrj_u16(rrj_at(m, owner + 454, 2)) >= -408 && (int16_t)rrj_u16(rrj_at(m, owner + 454, 2)) <= 408)
    {
        desired_yaw = rrj_s32(rrj_read32(m, actor + 608));
        desired_pitch = rrj_s32(rrj_read32(m, actor + 612));
    }
    else
    {
        desired_yaw = rrj_s32(sub_80020018(m, (uint32_t)(int32_t)r_s16(owner + 450), (uint32_t)(int32_t)r_s16(owner + 454)));
        desired_pitch = -rrj_s32(sub_8001FF3C(m, (uint32_t)((int32_t)r_s16(owner + 452) * 16)));
    }
    if ((flags & 6) == 6)
    {
        rrj_write32(m, actor + 608, (uint32_t)desired_yaw);
        rrj_write32(m, actor + 612, (uint32_t)desired_pitch);
        rrj_write32(m, actor + 616, 0);
        rrj_write32(m, actor + 620, rrj_read32(m, table));
    }
    else
    {
        (void)sub_80086C00(m, actor + 608, desired_yaw, delta, rrj_s32(rrj_read32(m, table)));
        (void)sub_80086D54(m, actor + 612, desired_pitch, delta, actor + 616, 100007, 30015);
    }
    if (rrj_read32(m, target + 604) >= 2)
    {
        int32_t accumulated = rrj_s32(rrj_read32(m, actor + 484)) + rrj_s32(rrj_read32(m, actor + 608)) - desired_yaw;
        uint32_t elapsed = rrj_read32(m, actor + 316) + (uint32_t)delta;

        rrj_write32(m, actor + 484, (uint32_t)accumulated);
        rrj_write32(m, actor + 316, elapsed);
        if (elapsed > 0x10000)
        {
            int32_t average = accumulated < 0 ? -(int32_t)sub_80010028(0u - ((uint32_t)accumulated << 16), elapsed) : (int32_t)sub_80010028((uint32_t)accumulated << 16, elapsed);

            rrj_write32(m, actor + 456, (uint32_t)average);
            rrj_write32(m, actor + 316, 0);
            rrj_write32(m, actor + 484, 0);
        }
    }
    rrj_put16(rrj_at(m, angles, 2), (uint16_t)-(int16_t)rrj_u16(rrj_at(m, actor + 612, 2)));
    rrj_put16(rrj_at(m, angles + 2, 2), (uint16_t)-(int16_t)rrj_u16(rrj_at(m, actor + 608, 2)));
    rrj_put16(rrj_at(m, angles + 4, 2), 0);
    return sub_8004D2A4(m, angles, actor + 588);
}

uint32_t sub_800A13C4(RRJMemory *m, int32_t delta, RRJRaceLeafCall call)
{
    const uint32_t args[8] = {(uint32_t)delta, 0, 0, 0, 0, 0, 0, 0};

    FUNCTION_MARKER(0x800A13C4, "RASHCDG.BIN");
    if (rrj_read32(m, 0x8005B314) == 0 && rrj_read32(m, 0x8005B2AC) == 0)
        return 0;
    return call(m, 0x800A13D4, args);
}

uint32_t sub_80090814(RRJMemory *m, int32_t delta, RRJRaceLeafCall call)
{
    uint32_t actor = rrj_read32(m, 0x800CE4D0);
    uint32_t stride = rrj_read32(m, 0x800CE4D4);
    uint32_t count_pointer = rrj_read32(m, 0x800CE4DC);
    int32_t count = rrj_s32(rrj_read32(m, count_pointer));
    uint32_t result = count_pointer;

    FUNCTION_MARKER(0x80090814, "RASHCDG.BIN");
    while (count-- >= 0)
    {
        uint32_t config = rrj_read32(m, 0x8005B2F8);
        uint32_t player = rrj_u16(rrj_at(m, actor + 172, 2));
        uint32_t state = rrj_read32(m, actor + 1084);
        if (player < rrj_read32(m, config + 48) || (r_u8(state + 1) & 15) != 2 || (r_u8(actor + 928) & 0x10))
        {
            if (rrj_u16(rrj_at(m, actor + 320, 2)))
            {
                uint32_t mode = r_u8(config + 4);
                if ((player < rrj_read32(m, config + 48) && player == 0 && (rrj_read32(m, config + 4) & 0x18) == 0x10) || (!(mode & 0x10) && (mode & 1) && player == r_u8(config + 6)))
                {
                    uint32_t timer = rrj_read32(m, 0x8005B2FC) + (uint32_t)delta;
                    int32_t sequence = rrj_s32(rrj_read32(m, 0x800CCA7C));
                    uint32_t index = rrj_read32(m, config + 60);
                    rrj_write32(m, 0x8005B2FC, timer);
                    if (sequence < 0)
                    {
                        if (rrj_s32(timer) >= rrj_s32(rrj_read32(m, 0x8005306C + 4 * index)))
                        {
                            uint32_t body = rrj_read32(m, actor + 852);
                            rrj_write32(m, actor + 36, rrj_read32(m, actor + 36) | 0x800);
                            rrj_write32(m, body + 36, rrj_read32(m, body + 36) | 0x800);
                            rrj_write32(m, 0x800CCA7C, 1);
                            rrj_write32(m, 0x8005B2FC, 0);
                        }
                    }
                    else if (rrj_s32(timer) >= rrj_s32(rrj_read32(m, 0x80053060 + 4 * index)))
                    {
                        uint32_t body = rrj_read32(m, actor + 852);
                        uint32_t flags = rrj_read32(m, actor + 36);
                        uint32_t body_flags = rrj_read32(m, body + 36);
                        flags = (flags & ~0x800u) | (((flags >> 11) ^ 1u) << 11);
                        body_flags = (body_flags & ~0x800u) | (((body_flags >> 11) ^ 1u) << 11);
                        rrj_write32(m, actor + 36, flags);
                        rrj_write32(m, body + 36, body_flags);
                        if (flags & 0x800)
                            ++sequence;
                        if (sequence >= rrj_s32(rrj_read32(m, 0x80053078 + 4 * index)) && !(flags & 0x800))
                            sequence = -1;
                        rrj_write32(m, 0x800CCA7C, (uint32_t)sequence);
                        rrj_write32(m, 0x8005B2FC, 0);
                    }
                }
                if (rrj_read32(m, actor + 856))
                {
                    uint32_t shadow = rrj_read32(m, actor + 856);
                    unsigned i;
                    for (i = 0; i < 9; ++i)
                    {
                        rrj_put16(rrj_at(m, shadow + 432 + 2 * i, 2), rrj_u16(rrj_at(m, actor + 432 + 2 * i, 2)));
                        rrj_put16(rrj_at(m, shadow + 516 + 2 * i, 2), rrj_u16(rrj_at(m, actor + 516 + 2 * i, 2)));
                    }
                    rrj_put16(rrj_at(m, shadow + 450, 2), rrj_u16(rrj_at(m, actor + 450, 2)));
                    rrj_put16(rrj_at(m, shadow + 452, 2), rrj_u16(rrj_at(m, actor + 452, 2)));
                    rrj_put16(rrj_at(m, shadow + 454, 2), rrj_u16(rrj_at(m, actor + 454, 2)));
                    rrj_write32(m, shadow + 568, rrj_read32(m, actor + 568));
                    rrj_write32(m, shadow + 564, rrj_read32(m, actor + 564));
                    rrj_write32(m, shadow + 480, rrj_read32(m, actor + 480));
                    rrj_write32(m, shadow + 636, rrj_read32(m, actor + 636));
                    rrj_write32(m, shadow + 652, rrj_read32(m, actor + 652));
                    rrj_write32(m, shadow + 616, rrj_read32(m, actor + 616));
                    rrj_write32(m, shadow + 620, rrj_read32(m, actor + 620));
                    rrj_write32(m, shadow + 676, rrj_read32(m, actor + 676));
                    rrj_put16(rrj_at(m, shadow + 320, 2), rrj_u16(rrj_at(m, actor + 320, 2)));
                }
                {
                    uint32_t body = rrj_read32(m, actor + 852);
                    uint32_t remaining = (r_u8(body + 572) >> 4) & 1;
                    do
                    {
                        uint32_t flags = rrj_read32(m, body + 552);
                        uint32_t event;
                        if (flags & 0x4000)
                        {
                            uint32_t owner = rrj_read32(m, body + 596);
                            rrj_write32(m, body + 552, flags | 0x8000);
                            rrj_write32(m, owner + 568, rrj_read32(m, owner + 568) | 8);
                        }
                        flags = rrj_read32(m, body + 552);
                        if (flags & 0x8000)
                        {
                            uint32_t body_mode = rrj_read32(m, body + 604);
                            if (body_mode - 1 < 2 || (!body_mode && rrj_u16(rrj_at(m, body + 544, 2))))
                                (void)leaf_frontier_call(m, call, 0x80090D84, body);
                        }
                        flags = rrj_read32(m, body + 552);
                        if (flags & 0x4000)
                        {
                            uint32_t owner = rrj_read32(m, body + 596);
                            rrj_write32(m, body + 552, flags & ~0x4000u);
                            rrj_write32(m, owner + 568, rrj_read32(m, owner + 568) & ~8u);
                        }
                        if (rrj_read32(m, body + 604) != 2)
                        {
                            if (!(rrj_read32(m, body + 552) & 0x02000000))
                                (void)leaf_frontier_call(m, call, 0x800C5078, body);
                        }
                        else
                        {
                            event = rrj_u16(rrj_at(m, body + 544, 2));
                            if (event == 88 || event == 40)
                                (void)leaf_frontier_call(m, call, 0x800C341C, body);
                            else if (event == 38 || ((event == 39 || event == 89) && ((r_u8(body + 572) & 0x20) && (rrj_read32(m, actor + 568) & 0x40))))
                                (void)leaf_frontier_call(m, call, 0x800C3630, body);
                            else if (rrj_read32(m, actor + 568) & 0x200)
                                (void)leaf_frontier_call(m, call, 0x80091468, body);
                            else if (event != 39 && event != 89 && leaf_frontier_call(m, call, 0x8005BE58, rrj_read32(m, body + 540)))
                                (void)leaf_frontier_call(m, call, 0x80091468, body);
                            else if (event == 39 || event == 89)
                                (void)leaf_frontier_call(m, call, 0x800C31CC, body);
                        }
                        if (remaining)
                            body = rrj_read32(m, rrj_read32(m, actor + 856) + 852);
                    } while (remaining-- > 0);
                }
            }
            rrj_write32(m, actor + 560, rrj_read32(m, actor + 560) & 0xFFFFFFDEu);
            xport_update_u8(actor + 928, XPORT_MEMORY_UPDATE_AND, 0xFD);
        }
        result = stride;
        actor += stride;
    }
    return result;
}

uint32_t sub_800C5078(RRJMemory *m, uint32_t actor, RRJRaceLeafCall call)
{
    static const uint32_t handlers[] = {0x800C47CC, 0x800C4860, 0x800C4B30, 0x800C4BA0, 0x800C4E18};
    uint32_t event = rrj_u16(rrj_at(m, actor + 544, 2));
    uint32_t result;
    unsigned i;

    FUNCTION_MARKER(0x800C5078, "RASHCDG.BIN");
    if (rrj_u16(rrj_at(m, 0x800541D6 + 8 * event, 2)) == 3)
        return 3;
    for (i = 0; i < sizeof(handlers) / sizeof(handlers[0]); ++i)
    {
        result = leaf_frontier_call(m, call, handlers[i], actor);
        if (result)
            return result;
    }
    if (rrj_u16(rrj_at(m, 0x800541D6 + 8 * event, 2)) == 2 || event == 39)
        return 39;
    result = leaf_frontier_call(m, call, 0x8005BE58, rrj_read32(m, actor + 540));
    if (result)
    {
        uint32_t output = 0x1F8003F0;
        uint32_t selected = leaf_frontier_call3(m, call, 0x800C45D8, actor, output, 0);
        return leaf_frontier_call3(m, call, 0x800C4550, selected & 0xFFFF, actor, rrj_read32(m, output));
    }
    return result;
}

uint32_t sub_800C47CC(RRJMemory *m, uint32_t actor, RRJRaceLeafCall call)
{
    uint32_t count = rrj_u16(rrj_at(m, actor + 608, 2));
    uint32_t flags;
    uint32_t options;

    FUNCTION_MARKER(0x800C47CC, "RASHCDG.BIN");
    if (!count)
        return 0;
    flags = rrj_read32(m, actor + 552);
    options = (flags & 0x10000000) ? 34 : 36;
    if (flags & 0x08000000)
        options |= 0x100;
    if (leaf_frontier_call3(m, call, 0x800C4550, rrj_u16(rrj_at(m, actor + 610 + 2 * (count - 1), 2)), actor, options))
        rrj_put16(rrj_at(m, actor + 608, 2), (uint16_t)(count - 1));
    return 1;
}

uint32_t sub_800C4860(RRJMemory *m, uint32_t actor, RRJRaceLeafCall call)
{
    uint32_t owner = rrj_read32(m, actor + 596);
    uint32_t mode = rrj_read32(m, actor + 604);
    int32_t selector = (int8_t)r_u8(owner + 946);
    uint32_t event;
    uint32_t type;
    uint32_t link;

    FUNCTION_MARKER(0x800C4860, "RASHCDG.BIN");
    rrj_write32(m, actor + 552, rrj_read32(m, actor + 552) & 0xEFFFFFFFu);
    if (!(r_u8(actor + 572) & 0x20) && mode == 0 && rrj_u16(rrj_at(m, owner + 948 + 8 * selector, 2)) >= 3 && rrj_s32(rrj_read32(m, owner + 480)) > 0x8000)
    {
        event = rrj_u16(rrj_at(m, actor + 544, 2));
        if (event == 4)
            return sub_800C4550(m, 3, actor, 0x10);
        if (event == 6)
            return sub_800C4550(m, 7, actor, 0x10);
        return 0;
    }
    if (mode < 2 && (rrj_read32(m, owner + 564) & 0x08000000) && !(r_u8(actor + 572) & 0x20))
    {
        event = rrj_u16(rrj_at(m, actor + 544, 2));
        if (event == 9)
            return 1;
        link = rrj_read32(m, 0x8005B3E4);
        (void)sub_8005C018(m, rrj_read32(m, actor + 540), (rrj_read32(m, 0x8005421C) >> 4) & 0xFFF, 0, 0, rrj_read32(m, owner + 856) ? 4 : 13, 0, link ? rrj_read32(m, link + 36) : 0);
        (void)sub_800C2FF4(m, 9, actor, 0);
        return 1;
    }
    if (mode != 1)
        return 0;
    event = rrj_u16(rrj_at(m, actor + 544, 2));
    if (event != 9 && event != 10 && event != 76 && rrj_s32(rrj_read32(m, owner + 480)) > 0x8000 && rrj_read32(m, owner + 600) < 3 * rrj_read32(m, owner + 596))
    {
        type = (r_u8(actor + 572) & 0x20) ? 76 : 10;
        return sub_800C4550(m, type, actor, 10);
    }
    if ((r_u8(actor + 572) & 0x20) || rrj_s32(rrj_read32(m, owner + 480)) > 0x7FFF || (rrj_read32(m, owner + 568) & 15) || (rrj_read32(m, owner + 564) & 0x0021D000))
        return 0;
    type = 5;
    if (rrj_u16(rrj_at(m, owner + 172, 2)) < rrj_read32(m, rrj_read32(m, 0x8005B2F8) + 48))
    {
        link = rrj_read32(m, owner + 1084);
        if (rrj_read32(m, link + 40) && r_u8(link + 39) < 4 && r_u8(rrj_read32(m, 0x8005B2F8) + 57))
            type = 1;
    }
    return sub_800C4550(m, type, actor, 12);
}

uint32_t sub_800C4B30(RRJMemory *m, uint32_t actor, RRJRaceLeafCall call)
{
    uint32_t flags = r_u8(actor + 572);
    uint32_t result = 0;

    FUNCTION_MARKER(0x800C4B30, "RASHCDG.BIN");
    if (!(flags & 0x20) && (flags & 0x0C))
    {
        result = leaf_frontier_call3(m, call, 0x800C4550, 0x10, actor, (flags << 5) & 0x100);
        if (result)
            xport_update_u8(actor + 572, XPORT_MEMORY_UPDATE_AND, 0xF3);
    }
    return result;
}

uint32_t sub_800C4BA0(RRJMemory *m, uint32_t actor, RRJRaceLeafCall call)
{
    uint32_t owner = rrj_read32(m, actor + 596);
    uint32_t state = rrj_u16(rrj_at(m, actor + 544, 2));
    uint32_t result = 0;

    FUNCTION_MARKER(0x800C4BA0, "RASHCDG.BIN");
    if (rrj_read32(m, actor + 604) != 1)
        return 0;
    if (rrj_u16(rrj_at(m, 0x800541D6u + 8u * state, 2)) != 2 && (uint32_t)(state - 77u) >= 3u && (uint32_t)(state - 11u) >= 3u)
        return 0;

    result = sub_800C3950(m, actor) & 0xFFu;
    if (!result && (rrj_read32(m, owner + 560) & 1u))
        result = sub_800C4550(m, (r_u8(actor + 572) & 0x20u) ? 77u : 8u, actor, 4);
    if (!result)
    {
        int32_t speed = rrj_s32(rrj_read32(m, owner + 480));

        state = rrj_u16(rrj_at(m, actor + 544, 2));
        if ((state == 11 || state == 14 || state == 77 || state == 80) && speed > 2050790)
            result = sub_800C4550(m, (r_u8(actor + 572) & 0x20u) ? 78u : 12u, actor, 20);
        else if ((state == 12 || state == 13 || state == 78 || state == 79) && speed <= 1757819)
            result = sub_800C4550(m, (r_u8(actor + 572) & 0x20u) ? 80u : 14u, actor, 20);
    }
    if (!(r_u8(actor + 572) & 0x20u) && !result)
    {
        int32_t elapsed = rrj_s32(rrj_read32(m, rrj_read32(m, 0x8005B2F8) + 16)) - rrj_s32(rrj_read32(m, actor + 548));

        if (rrj_s32(rrj_read32(m, 0x800CCB9C)) < elapsed)
        {
            uint32_t type;

            state = rrj_u16(rrj_at(m, actor + 544, 2));
            if (state == 11)
                type = rrj_s32(rrj_read32(m, owner + 616)) > 0 ? 25u : 24u;
            else if (state == 13)
                type = 23;
            else
                return 0;
            result = sub_800C4550(m, type, actor, 4);
            rrj_write32(m, 0x800CCB9C, 300u * (sub_8001FC58(m) % 6u + 3u));
        }
    }
    return result;
}

uint32_t sub_800C4E18(RRJMemory *m, uint32_t actor, RRJRaceLeafCall call)
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
    if ((r_u8(actor + 572) & 0x20) || rrj_read32(m, actor + 604) != 1)
        return 0;

    owner = rrj_read32(m, actor + 596);
    owner_state = owner + 8 * (int32_t)r_s8(owner + 946) + 956;
    owner_state = rrj_u16(rrj_at(m, owner_state - 8, 2));
    if (owner_state < 4 || owner_state >= 13)
        return 0;

    index = sub_8008B428(m, owner + 172, 1) & 0xFFFFu;
    if (index == 224)
        return 0;
    other = rrj_read32(m, 0x8005B3A0) + 1096 * index;
    other_state = other + 8 * (int32_t)r_s8(other + 946) + 956;
    other_state = rrj_u16(rrj_at(m, other_state - 8, 2));
    collision = owner_state == 16 || owner_state == 6 || other_state == 16 || other_state == 6;
    distance = (rrj_read32(m, other + 324) - rrj_read32(m, owner + 324)) << 4;

    if (distance - 0xA0000u <= 0x13FFFFu)
    {
        if (collision)
            target_state = 15;
    }
    else if (distance <= 0x9FFFFu && collision)
    {
        target_state = 16;
        if (rrj_read32(m, actor + 544) == 13 || rrj_read32(m, actor + 544) == 20)
            target_state = 17;
    }

    if (target_state == 224 || target_state == rrj_read32(m, actor + 544))
        return 0;

    road = rrj_read32(m, other + 360);
    if (road == rrj_read32(m, owner + 360) && (!(road >> 16) || rrj_read32(m, other + 336) == rrj_read32(m, owner + 336)))
        separation = rrj_s32(rrj_read32(m, other + 344) - rrj_read32(m, owner + 344));
    else
        separation = rrj_s32(sub_800B6AAC(m, other + 184, owner + 432, owner + 184));
    if (rrj_s32(rrj_read32(m, owner + 364)) < 0 && road == rrj_read32(m, owner + 360) && (!(road >> 16) || rrj_read32(m, other + 336) == rrj_read32(m, owner + 336)))
        separation = -separation;

    magnitude = separation < 0 ? 0u - (uint32_t)separation : (uint32_t)separation;
    if (magnitude > 983039)
        return 0;
    return sub_800C4550(m, target_state, actor, separation < 0 ? 260 : 4);
}

uint32_t sub_800881B4(RRJMemory *m, uint32_t actor, int32_t delta, RRJRaceLeafCall call)
{
    uint32_t type = rrj_read32(m, actor + 540);
    uint32_t flags = rrj_read32(m, actor + 552);
    uint32_t mode;
    uint32_t descriptor;

    FUNCTION_MARKER(0x800881B4, "RASHCDG.BIN");
    if (!rrj_read32(m, actor + 772) || type - 7 >= 4)
        goto common_tail;
    if (flags & 1)
    {
        rrj_write32(m, actor + 772, 0);
        rrj_write32(m, actor + 540, rrj_read32(m, actor + 544));
        rrj_write32(m, actor + 552, flags & ~1u);
        rrj_write32(m, actor + 548, rrj_read32(m, actor + 548) | 0x102);
        rrj_write32(m, actor + 548, rrj_read32(m, actor + 548) & 0xFF9FFFFFu);
        goto common_tail;
    }
    if (flags & 0xC)
    {
        rrj_write32(m, actor + 552, (flags & ~0xCu) | 8);
        rrj_write32(m, actor + 540, 6);
        rrj_write32(m, actor + 548, rrj_read32(m, actor + 548) & 0xFF9FFFFFu);
        goto common_tail;
    }
    flags &= ~0x10u;
    if (flags & 2)
    {
        uint32_t config = rrj_read32(m, 0x8005B2F8);
        mode = rrj_read32(m, actor + 776) + 1;
        rrj_write32(m, actor + 776, mode);
        if (r_u8(config + 4) & 1)
            return leaf_frontier_call3(m, call, 0x80088970, actor, (uint32_t)delta, 0);
    }
    else
    {
        uint32_t config = rrj_read32(m, 0x8005B2F8);
        if ((r_u8(config + 4) & 1) && type == 7)
            mode = rrj_read32(m, 0x8005B2B0) == 1 ? 6 : 4;
        else
        {
            mode = type - 7;
            flags |= 4;
            if (type == 7)
                flags |= 1;
        }
        rrj_write32(m, actor + 776, mode);
    }
    rrj_write32(m, actor + 552, flags);
    descriptor = 0x800D83B4 + 26 * mode;
    rrj_write32(m, actor + 792, (flags & 7) ? (uint32_t)rrj_u16(rrj_at(m, descriptor, 2)) << 16 : 0);
    if (r_u8(descriptor + 2) >= 2)
        return leaf_frontier_call3(m, call, 0x800889F4, actor, (uint32_t)delta, descriptor);
    (void)leaf_frontier_call3(m, call, 0x800853E4, actor, descriptor + 3, 0);
    rrj_write32(m, actor + 780, 0);
    rrj_write32(m, actor + 784, 0);
    rrj_write32(m, actor + 700, 0);
    rrj_write32(m, actor + 548, rrj_read32(m, actor + 548) & 0xFF9FFFFFu);
common_tail:
    if (rrj_s32(rrj_read32(m, actor + 700)) <= 655359999)
        rrj_write32(m, actor + 700, rrj_read32(m, actor + 700) + (uint32_t)delta);
    type = rrj_read32(m, actor + 540);
    if ((type < 4 || type - 7 < 6) && !rrj_read32(m, actor + 772))
    {
        uint32_t owner = rrj_read32(m, actor + 568);
        uint32_t target = rrj_read32(m, owner + 852);
        uint32_t table = 0x800CD7B8 + 56 * type;
        int32_t acceleration;
        int32_t speed;
        int32_t next;

        rrj_write32(m, actor + 672, 0);
        rrj_write32(m, actor + 636, 0);
        if ((rrj_read32(m, actor + 548) & 0x48000) && rrj_read32(m, target + 604) >= 3 && !(rrj_read32(m, target + 552) & 0x80000))
            return leaf_frontier_call3(m, call, 0x80088BA0, actor, (uint32_t)delta, 0);
        if ((rrj_read32(m, actor + 548) & 0x800000) && (rrj_read32(m, owner + 388) & 1) && rrj_s32(rrj_read32(m, owner + 480)) > 146486)
            return leaf_frontier_call3(m, call, 0x80088BA0, actor, (uint32_t)delta, 0);
        (void)sub_80086B1C(m, actor, delta, rrj_read32(m, owner + 564) & 0x800 ? rrj_s32(rrj_read32(m, actor + 740)) : rrj_s32(rrj_read32(m, actor + 732)));
        rrj_write32(m, actor + 676, 0);
        rrj_write32(m, actor + 640, rrj_read32(m, table + 12 + 4 * (rrj_read32(m, actor + 548) & 1)) - rrj_read32(m, actor + 732));
        if (rrj_read32(m, actor + 548) & 1)
            next = rrj_s32(rrj_read32(m, table + 4 * ((rrj_read32(m, actor + 548) & 1) + 1)));
        else
        {
            uint32_t damping_offset = rrj_s32(rrj_read32(m, owner + 484)) <= 0 ? 40 : 32;
            uint32_t spring_offset = rrj_s32(rrj_read32(m, owner + 484)) <= 0 ? 36 : 28;

            acceleration = -(int32_t)sub_8001FC90(rrj_s32(rrj_read32(m, table + damping_offset)), rrj_s32(rrj_read32(m, actor + 736))) - (int32_t)sub_8001FC90(rrj_s32(rrj_read32(m, actor + 724) - rrj_read32(m, owner + 480)), rrj_s32(rrj_read32(m, table + spring_offset)));
            if (rrj_s32(rrj_read32(m, actor + 736)) > 0)
                acceleration -= (int32_t)sub_8001FC90(rrj_s32(rrj_read32(m, actor + 736)), 2 * rrj_s32(rrj_read32(m, table + damping_offset)));
            rrj_write32(m, actor + 712, (uint32_t)acceleration);
            next = rrj_s32(rrj_read32(m, actor + 724)) + (int32_t)sub_8001FC90(acceleration, delta);
            rrj_write32(m, actor + 724, (uint32_t)next);
            speed = rrj_s32(rrj_read32(m, actor + 736)) + (int32_t)sub_8001FC90(next - rrj_s32(rrj_read32(m, owner + 480)), delta);
            if (speed < -6553600)
                speed = -6553600;
            if (speed > 52428)
                speed = 52428;
            rrj_write32(m, actor + 736, (uint32_t)speed);
            next = rrj_s32(rrj_read32(m, table + 4)) + speed;
        }
        rrj_write32(m, actor + 644, (uint32_t)next);
        rrj_write32(m, actor + 680, rrj_read32(m, table + 20 + 4 * (rrj_read32(m, actor + 548) & 1)));
        flags = rrj_read32(m, actor + 548);
        if (flags & 2)
            return leaf_frontier_call3(m, call, 0x80088BA0, actor, (uint32_t)delta, 0);
        if (flags & 8)
        {
            if (flags & 0x40)
                rrj_write32(m, actor + 636, 0);
            (void)sub_8008676C(m, actor, delta);
        }
        else
        {
            rrj_write32(m, actor + 624, rrj_read32(m, actor + 636));
            rrj_write32(m, actor + 628, rrj_read32(m, actor + 640));
            rrj_write32(m, actor + 632, rrj_read32(m, actor + 644));
            rrj_write32(m, actor + 660, rrj_read32(m, actor + 672));
            rrj_write32(m, actor + 664, rrj_read32(m, actor + 676));
            rrj_write32(m, actor + 668, rrj_read32(m, actor + 680));
        }
        return sub_80087420(m, actor, delta, call);
    }
    if (!(type < 4 || type - 7 < 6))
        return leaf_frontier_call3(m, call, 0x80088BA0, actor, (uint32_t)delta, 0);
    rrj_write32(m, actor + 548, rrj_read32(m, actor + 548) & ~6u);
    if (type != 6)
        (void)leaf_frontier_call(m, call, 0x80086E1C, actor);
    (void)leaf_frontier_call3(m, call, 0x800374D4, actor, 1, 0);
    (void)leaf_frontier_call(m, call, 0x80088140, actor);
    if (rrj_read32(m, actor + 548) & 0x80000)
        return leaf_frontier_call3(m, call, 0x8008A6B0, actor, (uint32_t)delta, 0);
    flags = rrj_read32(m, actor + 548) & ~0x4000u;
    rrj_write32(m, actor + 548, flags);
    return flags;
}

static void leaf_frontier_load_point(RRJMemory *m, uint32_t actor, uint32_t slot, int secondary, int32_t kind, int32_t value, unsigned shift)
{
    int32_t base = (int16_t)rrj_u16(rrj_at(m, 0x800D85BC + 2 * kind, 2));
    int32_t index = base + (kind < 2 ? 2 * value : value);
    uint32_t point = 0x800D85C4 + 6 * index;
    unsigned i;

    for (i = 0; i < 3; ++i)
    {
        int32_t coordinate = (int16_t)rrj_u16(rrj_at(m, point + 2 * i, 2));
        int32_t tangent = (int16_t)rrj_u16(rrj_at(m, point + 6 + 2 * i, 2));
        uint32_t coordinate_base = secondary ? 972 : 828;
        uint32_t tangent_base = secondary ? 1044 : 900;
        rrj_write32(m, actor + coordinate_base + 24 * i + 4 * slot, (uint32_t)(coordinate << shift));
        rrj_write32(m, actor + tangent_base + 24 * i + 4 * slot, kind < 2 ? (uint32_t)(tangent << 8) : 0);
    }
}

uint32_t sub_800853E4(RRJMemory *m, uint32_t actor, uint32_t descriptor)
{
    int32_t mode = (int8_t)r_u8(descriptor + 1);
    uint32_t cursor = descriptor + 2;
    uint32_t slot = 0;

    FUNCTION_MARKER(0x800853E4, "RASHCDG.BIN");
    rrj_write32(m, actor + 548, rrj_read32(m, actor + 548) | 6);
    if (mode != 0)
        return 0;
    {
        int32_t kind = (int8_t)r_u8(cursor);
        if (kind == 0 || kind == 2)
        {
            int32_t value = (int8_t)r_u8(cursor + 1);
            leaf_frontier_load_point(m, actor, 0, 0, kind, value, 8);
            cursor += 2;
        }
        else
            rrj_write32(m, actor + 548, rrj_read32(m, actor + 548) & ~4u);
    }
    {
        int32_t kind = (int8_t)r_u8(cursor);
        if (kind == 1 || kind == 3)
        {
            int32_t value = (int8_t)r_u8(cursor + 1);
            leaf_frontier_load_point(m, actor, 0, 1, kind, value, 8);
            cursor += 2;
        }
        else
        {
            unsigned i;
            for (i = 0; i < 6; ++i)
                rrj_write32(m, actor + 972 + 24 * i, 0);
        }
    }
    if ((int8_t)r_u8(cursor) == 5)
    {
        rrj_write32(m, actor + 804, (uint32_t)(int8_t)r_u8(cursor + 1) << 16);
        cursor += 2;
    }
    else
        rrj_write32(m, actor + 804, 0);
    while (r_u8(cursor) == 0 || r_u8(cursor) == 2)
    {
        int32_t kind = (int8_t)r_u8(cursor);
        int32_t value = (int8_t)r_u8(cursor + 1);
        ++slot;
        leaf_frontier_load_point(m, actor, slot, 0, kind, value, 8);
        cursor += 2;
        if ((int8_t)r_u8(cursor) == 5)
        {
            rrj_write32(m, actor + 804 + 4 * slot, (uint32_t)(int8_t)r_u8(cursor + 1) << 16);
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
            leaf_frontier_load_point(m, actor, tail, 1, kind, value, 8);
            cursor += 2;
        }
        else
        {
            unsigned i;
            for (i = 0; i < 6; ++i)
                rrj_write32(m, actor + 972 + 24 * i + 4 * tail, 0);
        }
        kind = (int8_t)r_u8(cursor);
        if (kind != 0 && kind != 2)
        {
            uint32_t defaults = 0x800CD7B8 + 56 * rrj_read32(m, actor + 544);
            rrj_write32(m, actor + 828 + 4 * tail, 0);
            rrj_write32(m, actor + 900 + 4 * tail, 0);
            rrj_write32(m, actor + 924 + 4 * tail, 0);
            rrj_write32(m, actor + 852 + 4 * tail, rrj_read32(m, defaults + 12));
            rrj_write32(m, actor + 876 + 4 * tail, rrj_read32(m, defaults + 4));
            rrj_write32(m, actor + 948 + 4 * tail, rrj_read32(m, defaults + 20));
        }
        slot = tail;
    }
    rrj_write32(m, actor + 800, slot + 1);
    rrj_write32(m, actor + 540, 11);
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

uint32_t sub_80086E1C(RRJMemory *m, uint32_t actor, RRJRaceLeafCall call)
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
    flags = rrj_read32(m, actor + 548);
    if (!rrj_read32(m, actor + 772) && (flags & 0x4000))
        return leaf_frontier_call(m, call, 0x80086E5C, actor);
    dx = rrj_s32(rrj_read32(m, actor + 184) - rrj_read32(m, actor + 556));
    dy = rrj_s32(rrj_read32(m, actor + 188) - rrj_read32(m, actor + 560));
    dz = rrj_s32(rrj_read32(m, actor + 192) - rrj_read32(m, actor + 564));
    yaw = rrj_s32(sub_80020018(m, (uint32_t)-dx, rrj_read32(m, actor + 564) - rrj_read32(m, actor + 192)));
    horizontal = rrj_s32(sub_8004CF74(m, (uint32_t)((int32_t)sub_8001FC90(dx, dx) + (int32_t)sub_8001FC90(dz, dz))));
    pitch = rrj_s32(sub_80020018(m, (uint32_t)dy, (uint32_t)(4 * horizontal)));
    if (flags & 0x1000)
    {
        int32_t previous = rrj_s32(rrj_read32(m, actor + 748));
        previous = leaf_frontier_angle_delta(pitch, previous);
        rrj_write32(m, actor + 748, (uint32_t)previous);
        if ((((pitch - previous) >> 31) + pitch - previous ^ ((pitch - previous) >> 31)) < 3)
        {
            rrj_write32(m, actor + 748, (uint32_t)pitch);
            flags &= ~0x1000u;
        }
        else
            rrj_write32(m, actor + 748, (uint32_t)((3 * previous + pitch) / 4));
    }
    else
        rrj_write32(m, actor + 748, (uint32_t)pitch);
    if (flags & 0x2000)
    {
        int32_t previous = rrj_s32(rrj_read32(m, actor + 744));
        previous = leaf_frontier_angle_delta(yaw, previous);
        rrj_write32(m, actor + 744, (uint32_t)previous);
        if ((((yaw - previous) >> 31) + yaw - previous ^ ((yaw - previous) >> 31)) < 23)
        {
            rrj_write32(m, actor + 744, (uint32_t)yaw);
            flags &= ~0x2000u;
        }
        else if (flags & 8)
            rrj_write32(m, actor + 744, (uint32_t)((3 * previous + yaw) / 4));
        else
            rrj_write32(m, actor + 744, (uint32_t)((previous + yaw) / 2));
    }
    else
        rrj_write32(m, actor + 744, (uint32_t)yaw);
    rrj_write32(m, actor + 548, flags);
    rrj_put16(rrj_at(m, angles, 2), (uint16_t)-rrj_u16(rrj_at(m, actor + 748, 2)));
    rrj_put16(rrj_at(m, angles + 2, 2), (uint16_t)-rrj_u16(rrj_at(m, actor + 744, 2)));
    rrj_put16(rrj_at(m, angles + 4, 2), 0);
    return sub_8004D2A4(m, angles, actor + 432);
}

uint32_t sub_80088140(RRJMemory *m, uint32_t actor)
{
    uint32_t prior = (rrj_read32(m, actor + 36) >> 8) & 1;
    uint32_t flags;
    uint32_t result;

    FUNCTION_MARKER(0x80088140, "RASHCDG.BIN");
    rrj_write32(m, actor + 344, 0u - rrj_read32(m, actor + 344));
    (void)sub_8003A9D8(m, actor);
    flags = rrj_read32(m, actor + 36);
    result = (((flags >> 8) & 1) | prior) << 8;
    rrj_write32(m, actor + 344, 0u - rrj_read32(m, actor + 344));
    rrj_write32(m, actor + 36, (flags & ~0x100u) | result);
    return result;
}

uint32_t sub_8008CFDC(RRJMemory *m, RRJRaceLeafCall call)
{
    uint32_t actor = rrj_read32(m, 0x800CE4D0);
    uint32_t stride = rrj_read32(m, 0x800CE4D4);
    uint32_t count_pointer = rrj_read32(m, 0x800CE4DC);
    int32_t count = rrj_s32(rrj_read32(m, count_pointer));
    uint32_t players = rrj_read32(m, rrj_read32(m, 0x8005B2F8) + 48);
    uint16_t prior_second = rrj_u16(rrj_at(m, 0x800CD542, 2));
    uint16_t prior_first = (uint16_t)(rrj_u16(rrj_at(m, 0x800CD540, 2)) - prior_second);

    FUNCTION_MARKER(0x8008CFDC, "RASHCDG.BIN");
    rrj_put16(rrj_at(m, 0x800CD540, 2), 0);
    rrj_put16(rrj_at(m, 0x800CD542, 2), 0);
    while (count-- >= 0)
    {
        if (rrj_u16(rrj_at(m, actor + 320, 2)))
        {
            unsigned player_index;
            (void)leaf_frontier_call(m, call, 0x8008DBCC, actor);
            (void)leaf_frontier_call3(m, call, 0x800667C4, actor, players, 0);
            for (player_index = 0; player_index < players; ++player_index)
            {
                uint32_t player = 0x800CD898 + 1132 * player_index;
                uint32_t counter = 0x800CD540 + 2 * player_index;
                uint16_t actor_flags = rrj_u16(rrj_at(m, actor + 320, 2));
                if (rrj_read32(m, player + 548) & 1)
                    rrj_put16(rrj_at(m, counter, 2), player_index ? prior_second : prior_first);
                else
                {
                    int32_t distance = rrj_s32(rrj_read32(m, actor + 324) - rrj_read32(m, player + 324));
                    uint32_t close = distance < 8192;
                    int32_t visibility = rrj_s32(rrj_read32(m, actor + 44 + 4 * player_index));
                    actor_flags = (uint16_t)(actor_flags & ~(16u << player_index));
                    rrj_put16(rrj_at(m, actor + 320, 2), actor_flags);
                    if (visibility < (close ? 4096 : 768))
                    {
                        uint8_t level;
                        if (!close)
                            visibility += 4096;
                        level = (uint8_t)(visibility >> 6);
                        if ((actor_flags & 0x10) && r_u8(actor + 850) < level)
                            level = r_u8(actor + 850);
                        w_u8(actor + 850, level);
                        rrj_put16(rrj_at(m, counter, 2), (uint16_t)(rrj_u16(rrj_at(m, counter, 2)) + 1));
                        rrj_put16(rrj_at(m, actor + 320, 2), (uint16_t)(rrj_u16(rrj_at(m, actor + 320, 2)) | (16u << player_index)));
                    }
                }
            }
        }
        {
            uint32_t body = rrj_read32(m, actor + 852);
            if (rrj_u16(rrj_at(m, body + 320, 2)))
            {
                if (rrj_read32(m, body + 604) >= 3)
                    (void)leaf_frontier_call(m, call, 0x8008DBCC, body);
                else
                {
                    rrj_write32(m, body + 44, rrj_read32(m, actor + 44));
                    rrj_write32(m, body + 48, rrj_read32(m, actor + 48));
                }
                (void)leaf_frontier_call3(m, call, 0x800667C4, body, players, 0);
            }
        }
        actor += stride;
    }
    rrj_put16(rrj_at(m, 0x800CD540, 2), (uint16_t)(rrj_u16(rrj_at(m, 0x800CD540, 2)) + rrj_u16(rrj_at(m, 0x800CD542, 2))));
    {
        static const uint32_t list_bases[] = {0x800CE4F0, 0x800CE500, 0x800CE510, 0x800CE520};
        static const uint32_t list_strides[] = {0x800CE4F4, 0x800CE504, 0x800CE514, 0x800CE524};
        static const uint32_t list_counts[] = {0x800CE4FC, 0x800CE50C, 0x800CE51C, 0x800CE52C};
        unsigned list_index;
        for (list_index = 0; list_index < 4; ++list_index)
        {
            uint32_t item = rrj_read32(m, list_bases[list_index]);
            uint32_t item_stride = rrj_read32(m, list_strides[list_index]);
            int32_t item_count = rrj_s32(rrj_read32(m, rrj_read32(m, list_counts[list_index])));
            while (item_count-- >= 0)
            {
                if (rrj_u16(rrj_at(m, item + 172, 2)) && rrj_u16(rrj_at(m, item + 320, 2)))
                {
                    (void)leaf_frontier_call(m, call, 0x8008DBCC, item);
                    if (list_index < 2)
                        (void)leaf_frontier_call3(m, call, 0x800667C4, item, players, 0);
                }
                item += item_stride;
            }
        }
    }
    if (rrj_read32(m, 0x8005B314))
    {
        unsigned i;
        uint32_t item = rrj_read32(m, 0x8005B304);
        for (i = 0; i < 3; ++i, item += 280)
        {
            if (rrj_u16(rrj_at(m, item + 172, 2)))
                (void)leaf_frontier_call(m, call, 0x8008DBCC, item);
        }
    }
    return 0;
}

uint32_t sub_8008DBCC(RRJMemory *m, uint32_t actor)
{
    uint32_t players = rrj_read32(m, rrj_read32(m, 0x8005B2F8) + 48);
    uint32_t player = 0x800CD898;
    unsigned i;

    FUNCTION_MARKER(0x8008DBCC, "RASHCDG.BIN");
    rrj_write32(m, actor + 48, 0x7FFFFFFF);
    for (i = 0; i < players; ++i)
    {
        int32_t dx = rrj_s32(rrj_read32(m, actor + 184) - rrj_read32(m, player + 184)) >> 10;
        int32_t dy = rrj_s32(rrj_read32(m, actor + 188) - rrj_read32(m, player + 188)) >> 10;
        int32_t dz = rrj_s32(rrj_read32(m, actor + 192) - rrj_read32(m, player + 192)) >> 10;
        int32_t distance = rrj_s32(sub_8001FCB0((uint32_t)dx, (uint32_t)dy, (uint32_t)dz));
        rrj_write32(m, actor + 44 + 4 * i, distance < 0 ? 0x7FFFFFFF : (uint32_t)distance);
        player += 1132;
    }
    return i < players;
}

uint32_t sub_800667C4(RRJMemory *m, uint32_t actor, uint32_t players)
{
    uint32_t shape = rrj_read32(m, actor);
    uint32_t mode = (rrj_u16(rrj_at(m, shape + 14, 2)) & 0x78) >> 3;
    unsigned i;
    int32_t selected;

    FUNCTION_MARKER(0x800667C4, "RASHCDG.BIN");
    for (i = 0; i < players; ++i)
    {
        uint32_t ranges = rrj_read32(m, actor + 100);
        int32_t index = (int8_t)r_u8(actor + 10 + i);
        int32_t distance = rrj_s32(rrj_read32(m, actor + 44 + 4 * i));
        if (ranges)
        {
            uint32_t descriptor = rrj_read32(m, actor + 96);
            int32_t last = (int32_t)r_u8(descriptor + 4) - 1;
            if (index < last && rrj_s32(rrj_read32(m, ranges + 8 * index)) < distance)
            {
                do
                    ++index;
                while (rrj_s32(rrj_read32(m, ranges + 8 * index)) < distance);
            }
            else if (index)
            {
                while (distance < rrj_s32(rrj_read32(m, ranges + 8 * index + 4)))
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
        if (!selected || (selected == 1 && mode == 1 && ((uint32_t)(rrj_u16(rrj_at(m, actor + 172, 2)) & 0x1F) < players || (r_u8(actor + 572) & 0x20))))
            visible = 1;
        body = rrj_read32(m, actor + 540);
        rrj_write32(m, body + 1760, rrj_read32(m, 0x80052390 + 4 * (mode == 4 ? selected + 1 : selected)));
        rrj_write32(m, body + 36, (rrj_read32(m, body + 36) & ~4u) | 4 * visible);
    }
    {
        uint32_t result = r_u8(actor + 9) & 0xF7;
        w_u8(actor + 9, (uint8_t)result);
        return result;
    }
}

uint32_t sub_8005E1D8(RRJMemory *m, uint32_t context, RRJRaceLeafCall call)
{
    uint32_t enabled = rrj_read32(m, context + 8);
    int32_t count = rrj_s32(rrj_read32(m, context + 12));
    uint32_t actor = rrj_read32(m, context);
    int32_t i;
    uint32_t result = enabled;

    FUNCTION_MARKER(0x8005E1D8, "RASHCDG.BIN");
    if (!enabled || count <= 0)
        return result;
    for (i = 0; i < count; ++i, actor += 2108)
    {
        uint32_t flags = rrj_read32(m, actor + 36);
        if (flags & 2)
        {
            uint32_t shape = rrj_read32(m, actor);
            if ((r_u8(shape + 9) & 3) || !(flags & 0x10))
                (void)leaf_frontier_call(m, call, 0x8005D2A8, actor);
            if (!(rrj_read32(m, actor + 36) & 8))
                (void)leaf_frontier_call3(m, call, 0x8005CB04, actor, rrj_read32(m, rrj_read32(m, 0x8005B2F8) + 28), 0);
        }
        result = (uint32_t)(i + 1 < count);
    }
    return result;
}

uint32_t sub_8005D2A8(RRJMemory *m, uint32_t actor, RRJRaceLeafCall call)
{
    uint32_t descriptor = rrj_read32(m, actor + 4) + 12 * rrj_read32(m, actor + 12);
    uint32_t result;

    FUNCTION_MARKER(0x8005D2A8, "RASHCDG.BIN");
    if (r_u8(descriptor + 1) == 3)
        (void)leaf_frontier_call(m, call, 0x8005D36C, actor);
    else
    {
        (void)leaf_frontier_call3(m, call, 0x8005E5A4, actor, actor + 48, 0);
        (void)leaf_frontier_call(m, call, 0x8005D63C, actor);
    }
    rrj_write32(m, actor + 36, r_u8(actor + 36) | 0x10);
    result = r_u8(descriptor + 2) & 0x10;
    if (result)
    {
        uint32_t shape = rrj_read32(m, actor);
        w_u8(shape + 547, (uint8_t)(0u - r_u8(shape + 547)));
        result = r_u8(descriptor + 2) & 0xEF;
        w_u8(descriptor + 2, (uint8_t)result);
    }
    return result;
}

uint32_t sub_8005E5A4(RRJMemory *m, uint32_t actor, uint32_t channels, RRJRaceLeafCall call)
{
    int32_t frame = rrj_s32(rrj_read32(m, actor + 16));
    int32_t next = frame + 1;
    uint32_t source = rrj_read32(m, actor + 44);
    int32_t count = (int16_t)rrj_u16(rrj_at(m, channels + 2, 2));
    int32_t i;
    uint32_t result = (uint32_t)count;

    FUNCTION_MARKER(0x8005E5A4, "RASHCDG.BIN");
    if ((int32_t)rrj_u16(rrj_at(m, source + 16, 2)) - 1 < next)
        next = frame;
    for (i = 0; i < count; ++i)
    {
        uint32_t record = channels + 24 * (uint32_t)i;
        uint32_t value = record + 10;
        uint32_t flags = r_u8(value + 14);
        if (!(flags & 2))
        {
            uint32_t samples = rrj_read32(m, record + 4);
            rrj_put16(rrj_at(m, value - 2, 2), rrj_u16(rrj_at(m, samples + 2 * frame, 2)));
            rrj_put16(rrj_at(m, value, 2), rrj_u16(rrj_at(m, samples + 2 * next, 2)));
        }
        else if (flags & 1)
        {
            int32_t previous = (int16_t)rrj_u16(rrj_at(m, channels, 2));
            int32_t step;
            for (step = previous + 1; step < next; ++step)
            {
                uint32_t width = r_u8(value + 16);
                int32_t decoded;
                if (flags & 4)
                {
                    int32_t remaining = rrj_s32(rrj_read32(m, value + 6)) - 1;
                    decoded = (int8_t)r_u8(value + 12);
                    w_u8(value + 15, (uint8_t)decoded);
                    flags &= ~4u;
                    rrj_write32(m, value + 6, (uint32_t)remaining);
                    if (remaining > 0)
                        flags |= 4;
                    w_u8(value + 14, (uint8_t)flags);
                }
                else
                {
                    decoded = (int8_t)sub_8005E558(m, record + 4, record + 27, width);
                    w_u8(value + 15, (uint8_t)decoded);
                    if (decoded == (int8_t)r_u8(value + 13))
                    {
                        int32_t remaining = 0;
                        int32_t limit = 1 << width;
                        do
                        {
                            decoded = (int8_t)sub_8005E558(m, record + 4, record + 27, width);
                            w_u8(value + 15, (uint8_t)decoded);
                            if (decoded != limit - 1)
                                break;
                            remaining = remaining - 1 + limit;
                        } while (1);
                        remaining += 4 + decoded;
                        decoded = (int8_t)sub_8005E558(m, record + 4, record + 27, width);
                        w_u8(value + 12, (uint8_t)decoded);
                        w_u8(value + 15, (uint8_t)decoded);
                        rrj_write32(m, value + 6, (uint32_t)(remaining - 1));
                        flags |= 4;
                        w_u8(value + 14, (uint8_t)flags);
                    }
                }
                decoded = (int8_t)r_u8(value + 15);
                if ((decoded >> (width - 1)) & 1)
                    decoded -= 1 << width;
                rrj_write32(m, value + 2, rrj_read32(m, value + 2) + (uint32_t)(16 * decoded));
                rrj_put16(rrj_at(m, value - 2, 2), rrj_u16(rrj_at(m, value, 2)));
                rrj_put16(rrj_at(m, value, 2), (uint16_t)((rrj_s32(rrj_read32(m, value + 2)) * (int32_t)rrj_u16(rrj_at(m, value + 10, 2))) >> 9));
            }
            if (next == frame)
                rrj_put16(rrj_at(m, value - 2, 2), rrj_u16(rrj_at(m, value, 2)));
        }
        result = (uint32_t)(i + 1 < count);
    }
    rrj_put16(rrj_at(m, channels, 2), (uint16_t)frame);
    return result;
}

uint32_t sub_8005E558(RRJMemory *m, uint32_t stream_pointer, uint32_t bit_pointer, uint32_t width)
{
    uint32_t bit = r_u8(bit_pointer);
    uint32_t stream = rrj_read32(m, stream_pointer);
    uint32_t packed = ((uint32_t)rrj_u16(rrj_at(m, stream, 2)) << 16) | rrj_u16(rrj_at(m, stream + 2, 2));

    FUNCTION_MARKER(0x8005E558, "RASHCDG.BIN");
    rrj_write32(m, stream_pointer, stream + (((bit + width) >> 3) & 0x1E));
    w_u8(bit_pointer, (uint8_t)((bit + width) & 15));
    return ((packed << bit) >> (32 - width)) & 0xFF;
}

uint32_t sub_8005D63C(RRJMemory *m, uint32_t actor, RRJRaceLeafCall call)
{
    uint32_t object = rrj_read32(m, actor);
    uint32_t descriptor = rrj_read32(m, actor + 4) + 12 * rrj_read32(m, actor + 12);
    uint32_t linked = rrj_read32(m, object + 52);
    int32_t blend = 0;
    uint32_t subtype = 0;
    uint32_t vector = 0x1F8003D0;
    uint32_t quaternion = 0x1F8003E0;
    int32_t x = (int16_t)rrj_u16(rrj_at(m, actor + 56, 2));
    int32_t y = (int16_t)rrj_u16(rrj_at(m, actor + 80, 2));
    int32_t z = (int16_t)rrj_u16(rrj_at(m, actor + 104, 2));

    FUNCTION_MARKER(0x8005D63C, "RASHCDG.BIN");
    if (((rrj_read32(m, object + 36) >> 18) & 1) && linked)
        return leaf_frontier_call(m, call, 0x8005D708, actor);
    if (((rrj_u16(rrj_at(m, rrj_read32(m, object) + 14, 2)) & 0x78) >> 3) == 1 && linked)
        subtype = (rrj_u16(rrj_at(m, rrj_read32(m, linked) + 14, 2)) & 0xF80) >> 7;
    if (rrj_read32(m, actor + 36) & 4)
    {
        int32_t duration = rrj_s32(rrj_read32(m, actor + 24));
        int32_t elapsed = rrj_s32(rrj_read32(m, actor + 32));

        blend = (elapsed << 16) / duration;
        x = (int16_t)((int32_t)sub_8001FC90(0x10000 - blend, x) + (int32_t)sub_8001FC90(blend, (int16_t)rrj_u16(rrj_at(m, actor + 58, 2))));
        y = (int16_t)((int32_t)sub_8001FC90(0x10000 - blend, y) + (int32_t)sub_8001FC90(blend, (int16_t)rrj_u16(rrj_at(m, actor + 82, 2))));
        z = (int16_t)((int32_t)sub_8001FC90(0x10000 - blend, z) + (int32_t)sub_8001FC90(blend, (int16_t)rrj_u16(rrj_at(m, actor + 106, 2))));
    }
    if (subtype == 1)
    {
        x += (int16_t)rrj_u16(rrj_at(m, 0x800CC1C8, 2));
        y += (int16_t)rrj_u16(rrj_at(m, 0x800CC1CA, 2));
        z += (int16_t)rrj_u16(rrj_at(m, 0x800CC1CC, 2));
    }
    if (r_u8(descriptor + 2) & 1)
        x = -x;
    rrj_put16(rrj_at(m, vector, 2), (uint16_t)x);
    rrj_put16(rrj_at(m, vector + 2, 2), (uint16_t)y);
    rrj_put16(rrj_at(m, vector + 4, 2), (uint16_t)z);
    (void)leaf_frontier_call3(m, call, 0x80066A60, object, vector, 0);
    {
        uint32_t model = rrj_read32(m, actor + 44);
        uint32_t mask = (r_u8(descriptor + 2) & 0x40) ? UINT32_MAX : rrj_read32(m, actor + 1760);
        uint32_t matrix = rrj_read32(m, object + 4);
        uint32_t mode = (rrj_u16(rrj_at(m, rrj_read32(m, object) + 14, 2)) & 0x78) >> 3;
        uint32_t count = r_u8(model + 15);
        uint32_t index;
        int32_t fraction = (int16_t)(blend >> 2);
        int32_t inverse = 0x4000 - fraction;

        if ((r_u8(descriptor + 2) & 1) || (mode != 1 && mode != 4))
            return leaf_frontier_call(m, call, 0x8005D8DC, actor);
        for (index = 0; index < count; ++index)
        {
            uint32_t component;
            int32_t values[4];

            if (((mask >> index) & 1) == 0)
                continue;
            for (component = 0; component != 4; ++component)
            {
                int32_t current = (int16_t)rrj_u16(rrj_at(m, actor + 128 + component * 24 + index * 96, 2));
                int32_t value = current;

                if (fraction)
                {
                    int32_t next = (int16_t)rrj_u16(rrj_at(m, actor + 130 + component * 24 + index * 96, 2));
                    value = (inverse * current >> 14) + (fraction * next >> 14);
                }
                values[component] = value;
                rrj_put16(rrj_at(m, quaternion + component * 2, 2), (uint16_t)value);
            }
            if (subtype == 1 && ((rrj_read32(m, 0x800CC1C4) >> index) & 1))
                (void)leaf_frontier_call3(m, call, 0x800714FC, 0x800CC1CE + 8 * index, quaternion, quaternion);
            for (component = 0; component != 4; ++component)
                values[component] = (int16_t)rrj_u16(rrj_at(m, quaternion + component * 2, 2));
            for (component = 0; component != 4; ++component)
                rrj_write32(m, quaternion + component * 4, (uint32_t)(values[component] * 4));
            (void)leaf_frontier_call3(m, call, 0x8001005C, matrix + 24 * index + 4, quaternion, 0);
        }
    }
    if (rrj_u16(rrj_at(m, object + 544, 2)) != 42)
        return 42;
    return leaf_frontier_call(m, call, 0x8005DED4, actor);
}

uint32_t sub_80066A60(RRJMemory *m, uint32_t object, uint32_t vector)
{
    uint32_t result;

    FUNCTION_MARKER(0x80066A60, "RASHCDG.BIN");
    rrj_put16(rrj_at(m, object + 28, 2), rrj_u16(rrj_at(m, vector, 2)));
    rrj_put16(rrj_at(m, object + 30, 2), rrj_u16(rrj_at(m, vector + 2, 2)));
    result = rrj_u16(rrj_at(m, vector + 4, 2));
    rrj_put16(rrj_at(m, object + 32, 2), (uint16_t)result);
    return result;
}

uint32_t sub_8005CB04(RRJMemory *m, uint32_t actor, uint32_t delta, RRJRaceLeafCall call)
{
    uint32_t result;

    FUNCTION_MARKER(0x8005CB04, "RASHCDG.BIN");
    result = leaf_frontier_call3(m, call, 0x8005C58C, actor, delta, 0);
    if (result && (rrj_read32(m, actor + 36) & 2))
    {
        uint32_t frame = rrj_read32(m, actor + 12) + 1;

        rrj_write32(m, actor + 12, frame == rrj_read32(m, actor + 8) ? 0 : frame);
        result = leaf_frontier_call(m, call, 0x8005C418, actor);
        rrj_write32(m, actor + 12, result);
        return leaf_frontier_call(m, call, 0x8005C8F4, actor);
    }
    return result;
}

uint32_t sub_8005C52C(RRJMemory *m, uint32_t actor)
{
    uint32_t descriptor = rrj_read32(m, actor + 4) + 12 * rrj_read32(m, actor + 12);
    uint32_t result;

    FUNCTION_MARKER(0x8005C52C, "RASHCDG.BIN");
    (void)sub_8005C39C(m, actor, r_u8(descriptor));
    rrj_write32(m, actor + 16, 0);
    rrj_write32(m, actor + 28, 0);
    result = rrj_read32(m, descriptor + 8);
    rrj_write32(m, actor + 1756, result);
    return result;
}

uint32_t sub_8005C58C(RRJMemory *m, uint32_t actor, uint32_t delta, RRJRaceLeafCall call)
{
    uint32_t base = rrj_read32(m, actor + 4);
    uint32_t descriptor = base + 12 * rrj_read32(m, actor + 12);
    uint32_t model = rrj_read32(m, actor + 44);
    uint32_t mode = r_u8(descriptor + 1);
    int32_t progress;

    FUNCTION_MARKER(0x8005C58C, "RASHCDG.BIN");
    if (mode == 0)
    {
        (void)leaf_frontier_call3(m, call, 0x8005C4EC, actor, delta, 0);
        progress = rrj_s32(rrj_read32(m, actor + 16));
        if (progress >= (int32_t)rrj_u16(rrj_at(m, model + 16, 2)) - 1)
            rrj_write32(m, actor + 32, 0);
        if (progress < (int32_t)rrj_u16(rrj_at(m, model + 16, 2)))
            return 0;
        if (rrj_read32(m, actor + 20) != 0xFFFF)
            rrj_write32(m, actor + 20, rrj_read32(m, actor + 20) - 1);
        if (rrj_read32(m, actor + 20))
        {
            (void)leaf_frontier_call(m, call, 0x8005C52C, actor);
            return 0;
        }
        return 1;
    }
    if (mode == 1)
    {
        (void)leaf_frontier_call3(m, call, 0x8005C4EC, actor, delta, 0);
        progress = rrj_s32(rrj_read32(m, actor + 16));
        if (progress >= (int16_t)rrj_u16(rrj_at(m, descriptor + 6, 2)))
            rrj_write32(m, actor + 32, 0);
        return progress > (int16_t)rrj_u16(rrj_at(m, descriptor + 6, 2));
    }
    if (mode == 5)
        return 1;
    if (mode != 3)
        return 0;
    (void)leaf_frontier_call3(m, call, 0x8005C4EC, actor, delta, 0);
    if (rrj_s32(rrj_read32(m, actor + 16)) < (int16_t)rrj_u16(rrj_at(m, descriptor + 4, 2)))
        return 0;
    rrj_write32(m, actor + 16, 0);
    rrj_write32(m, actor + 32, 0);
    if (r_u8(descriptor + 2) & 4)
    {
        uint8_t flags;

        rrj_write32(m, actor + 12, rrj_read32(m, actor + 8) - 1);
        w_u8(base + 2, r_u8(descriptor + 26));
        rrj_write32(m, base + 8, rrj_read32(m, descriptor + 32));
        w_u8(base, r_u8(descriptor + 28));
        flags = r_u8(base + 2);
        w_u8(base + 3, (flags & 8) ? r_u8(descriptor + 27) : 0);
        if (r_u8(descriptor) == 1)
        {
            w_u8(base + 1, 1);
            rrj_put16(rrj_at(m, base + 4, 2), 0);
            rrj_put16(rrj_at(m, base + 6, 2), rrj_u16(rrj_at(m, model + 16, 2)) - 1);
            w_u8(base + 13, 5);
        }
        else
        {
            w_u8(base + 1, 0);
            rrj_put16(rrj_at(m, base + 4, 2), UINT16_MAX);
            rrj_put16(rrj_at(m, base + 6, 2), 0);
            w_u8(base + 13, 2);
        }
        w_u8(base + 14, flags);
        rrj_write32(m, base + 20, rrj_read32(m, base + 8));
        w_u8(base + 12, r_u8(base));
        w_u8(base + 15, r_u8(base + 3));
        rrj_put16(rrj_at(m, base + 16, 2), 0);
        rrj_put16(rrj_at(m, base + 18, 2), 0);
    }
    return 1;
}

uint32_t sub_8005C4EC(RRJMemory *m, uint32_t actor, uint32_t delta)
{
    int32_t period = rrj_s32(rrj_read32(m, actor + 24));
    int32_t remainder = rrj_s32(delta + rrj_read32(m, actor + 28));

    FUNCTION_MARKER(0x8005C4EC, "RASHCDG.BIN");
    while (remainder >= period)
    {
        remainder -= period;
        rrj_write32(m, actor + 16, rrj_read32(m, actor + 16) + 1);
    }
    rrj_write32(m, actor + 32, (uint32_t)remainder);
    rrj_write32(m, actor + 28, (uint32_t)remainder);
    return remainder < period;
}
