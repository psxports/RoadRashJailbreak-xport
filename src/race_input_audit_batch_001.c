#include "race_leaf.h"
#include "race_pause.h"
#include "fixed_math.h"
#include "menu_latch.h"
#include "xport.h"
#include <stdio.h>
#include <stdlib.h>

static uint32_t input_call(RRJMemory *m, RRJRaceLeafCall call, uint32_t target, uint32_t a0, uint32_t a1, uint32_t a2)
{
    const uint32_t args[8] = {a0, a1, a2, 0, 0, 0, 0, 0};
    if (!call)
        abort();
    return call(m, target, args);
}

static uint32_t search_magnitude(uint32_t value)
{
    uint32_t sign = (uint32_t)(rrj_s32(value) >> 31);
    return (value + sign) ^ sign;
}

static uint32_t search_upper_bound(uint32_t value)
{
    return rrj_s32(value - 24) < 0 ? value : 24;
}

uint32_t sub_8008B428(uint32_t source, uint32_t mask)
{
    uint32_t first_x = rrj_read32(0x800CCF98);
    uint32_t first_z = rrj_read32(0x800CCFA0);
    uint32_t source_x = rrj_read32(source + 12);
    uint32_t source_z = rrj_read32(source + 20);
    uint32_t source_id = r_u16(source);
    uint32_t column_origin = (uint32_t)(rrj_s32(source_x - first_x) >> 21) + 11;
    uint32_t row_origin = (uint32_t)(rrj_s32(source_z - first_z) >> 21) + 11;
    uint32_t players = rrj_read32(rrj_read32(0x8005B2F8) + 48);
    uint32_t extent = 1, page, best_id = 224, best_distance = 0x7FFF0000;
    FUNCTION_MARKER(0x8008B428, "RASHCDG.BIN");
    if (players != 1 && (column_origin >= 24 || row_origin >= 24))
    {
        uint32_t second_x = rrj_read32(0x800CCF9C);
        uint32_t second_z = rrj_read32(0x800CCFA4);
        column_origin = (uint32_t)(rrj_s32(source_x - second_x) >> 21) + 11;
        row_origin = (uint32_t)(rrj_s32(source_z - second_z) >> 21) + 11;
        row_origin += 24u << ((uint32_t)(rrj_s32(row_origin) >> 31) & 31);
    }
    players = rrj_read32(rrj_read32(0x8005B2F8) + 48);
    page = rrj_s32(row_origin) >= 24 && rrj_s32(row_origin) < rrj_s32(24u << ((players - 1) & 31)) ? 24 : 0;
    row_origin -= page;
    while (column_origin < 24 || row_origin < 24 || column_origin + extent < 24 || row_origin + extent < 24)
    {
        uint32_t first_column = rrj_s32(column_origin) < 0 ? 0 : column_origin;
        uint32_t first_row = rrj_s32(row_origin) < 0 ? 0 : row_origin;
        uint32_t columns = search_upper_bound(column_origin + extent);
        uint32_t rows = search_upper_bound(row_origin + extent);
        uint32_t row;
        for (row = first_row; rrj_s32(row) < rrj_s32(rows); ++row)
        {
            uint32_t column;
            for (column = first_column; rrj_s32(column) < rrj_s32(columns); ++column)
            {
                uint32_t link = r_u8(0x800CD0B0 + 24 * (page + row) + column);
                if (link != 128)
                {
                    uint32_t actor_base = rrj_read32(0x8005B3A0);
                    uint32_t object_base = rrj_read32(0x8005B3A4);
                    do
                    {
                        uint32_t encoded = r_u8(0x800CCFA8 + 2 * link + 1);
                        uint32_t kind = encoded >> 5;
                        if (mask & (1u << kind))
                        {
                            uint32_t candidate, identity, coordinate_offset;
                            if (!kind)
                            {
                                candidate = actor_base + 1096 * encoded;
                                coordinate_offset = 172;
                            }
                            else if (kind == 1)
                            {
                                candidate = object_base + 628 * (encoded & 31);
                                coordinate_offset = 172;
                            }
                            else
                            {
                                uint32_t group = 0x800CE4D0 + 16 * kind;
                                uint32_t stride = rrj_read32(group + 4);
                                uint32_t base = rrj_read32(group);
                                uint32_t offset = rrj_read32(0x800CCA68 + 4 * (kind - 2));
                                candidate = base + stride * (encoded & 31) + offset;
                                coordinate_offset = 0;
                            }
                            identity = r_u16(candidate + coordinate_offset);
                            if (identity != source_id && (kind != 1 || rrj_read32(candidate + 604) == 4))
                            {
                                uint32_t x = rrj_read32(candidate + coordinate_offset + 12);
                                uint32_t origin_x = rrj_read32(source + 12);
                                uint32_t z = rrj_read32(candidate + coordinate_offset + 20);
                                uint32_t origin_z = rrj_read32(source + 20);
                                uint32_t dx = search_magnitude(x - origin_x);
                                uint32_t dz = search_magnitude(z - origin_z);
                                uint32_t minor = rrj_s32(dx) < rrj_s32(dz) ? dx : dz;
                                uint32_t distance = dx + dz - (uint32_t)(rrj_s32(minor) / 2);
                                if (rrj_s32(distance) < rrj_s32(best_distance))
                                {
                                    best_distance = distance;
                                    best_id = identity;
                                }
                            }
                        }
                        link = r_u8(0x800CCFA8 + 2 * link);
                    } while (link != 128);
                }
            }
        }
        extent += 2;
        --column_origin;
        --row_origin;
        if (best_id != 224)
            return best_id;
    }
    return best_id;
}

static uint32_t input_probe_args[6];

static uint32_t input_probe_call(RRJMemory *m, uint32_t target, const uint32_t args[8])
{
    FILE *stream = (FILE *)m->sdk_user;
    unsigned index, count;
    if (!stream)
        abort();
    switch (target)
    {
        case 0x800C2348:
        case 0x800C258C:
        case 0x8008A998:
            count = 2;
            break;
        case 0x800C2218:
            count = 3;
            break;
        default:
            abort();
    }
    fprintf(stream, "%08X", target);
    for (index = 0; index < count; ++index)
        fprintf(stream, " %08X", args[index]);
    fputc('\n', stream);
    if (target == 0x800C2348 || target == 0x800C258C)
    {
        if (input_probe_args[2] & 1)
            rrj_write32(args[1] + 852, input_probe_args[3]);
        if (input_probe_args[2] & 2)
            rrj_write32(0x8005B2F8, input_probe_args[4]);
    }
    else if (target == 0x800C2218)
    {
        if (input_probe_args[2] & 4)
            w_u16(args[1] + 172, 9);
        return input_probe_args[1];
    }
    else
        return sub_8008A998(args[0], args[1]);
    return 0;
}

uint32_t rrj_input_audit_probe(RRJMemory *m, const uint32_t args[6])
{
    unsigned index;
    for (index = 0; index < 6; ++index)
        input_probe_args[index] = args[index];
    m->cpu_status = args[0];
    sub_8001CB3C_race(input_probe_call);
    return m->cpu_status;
}

uint32_t sub_8001CB3C_race(RRJRaceLeafCall call)
{
    uint32_t state, players, player, changed = 0;
    int32_t mode;
    FUNCTION_MARKER(0x8001CB3C, "SLUS_010.53");
    sub_80043DA4();
    sub_8001E0B4(0x800D7128, 0x800D6DE0, 768);
    state = rrj_read32(0x8005B2F8);
    players = rrj_read32(state + 52);
    for (player = 0; rrj_s32(player) < rrj_s32(players); ++player)
    {
        uint32_t button, record = 0x800D6DF4 + 192 * player;
        for (button = 0; button < 19; ++button, record += 8)
        {
            uint32_t held = rrj_read32(record);
            w_u8(record + 6, 0);
            if (rrj_s32(held) < 0)
                held = 0;
            rrj_write32(record, held);
        }
    }
    state = rrj_read32(0x8005B2F8);
    mode = r_s8(state);
    if (mode == 2)
    {
        sub_80043DB4();
        return 0;
    }
    if (mode == 1 && rrj_read32(0x8005B478))
    {
        w_u8(state, 3);
        changed = 1;
    }
    else
    {
        state = rrj_read32(0x8005B2F8);
        players = rrj_read32(state + 52);
        player = 0;
        if (players)
        {
            do
            {
                uint32_t input = 0x800D7128 + 192 * player;
                uint32_t map = rrj_read32(input + 184);
                if (r_s8(input + 8 * rrj_read32(map + 4) + 26))
                {
                    if ((rrj_read32(state) & 0xFF0000FF) == 1)
                    {
                        w_u8(state, 3);
                        rrj_write32(0x8005AD90, player);
                        changed = 1;
                    }
                    else if (r_s8(state) == 6)
                        rrj_write32(0x8005AF50, 1);
                }
                state = rrj_read32(0x8005B2F8);
                players = rrj_read32(state + 52);
                ++player;
            } while (player < players);
        }
    }
    state = rrj_read32(0x8005B2F8);
    if (r_s8(state + 3))
    {
        w_u8(state, 1);
        changed = 1;
    }
    state = rrj_read32(0x8005B2F8);
    mode = r_s8(state);
    if (mode == 1 && !changed)
        rrj_write32(state + 24, rrj_read32(state + 12) - rrj_read32(state + 20));
    else if (mode == 1 && r_s8(state + 2))
    {
        rrj_write32(state + 28, 15);
        rrj_write32(state + 24, 15);
    }
    else
    {
        rrj_write32(state + 28, 0);
        rrj_write32(state + 24, 0);
    }
    state = rrj_read32(0x8005B2F8);
    {
        uint32_t exiting = rrj_read32(0x8005B220);
        rrj_write32(state + 20, rrj_read32(state + 12));
        if (exiting)
        {
            if (rrj_read32(0x800D712C) || rrj_read32(0x800D71EC) || rrj_read32(0x800D72AC) || rrj_read32(0x800D736C))
            {
                state = rrj_read32(0x8005B2F8);
                w_u8(state, 2);
            }
            return 0;
        }
    }
    sub_80043DB4();
    if (changed)
    {
        state = rrj_read32(0x8005B2F8);
        sub_80018C1C(r_s8(state) == 3);
    }
    {
        uint32_t pad_kind = r_u8(0x800D70E1) >> 4;
        state = rrj_read32(0x8005B2F8);
        players = rrj_read32(state + 52);
        if (pad_kind != 8 && rrj_s32(players) >= 3)
            players = 2;
    }
    for (player = 0; rrj_s32(player) < rrj_s32(players); ++player)
    {
        uint32_t input = 0x800D7128 + 192 * player;
        uint32_t entity, enabled, camera = 0;
        uint32_t counter = 0x800D6DD0 + 4 * player;
        if (!player)
        {
            entity = rrj_read32(0x8005B38C);
            enabled = ((rrj_read32(entity + 560) >> 27) ^ 1) & 1;
            camera = 0x800CD898;
        }
        else if (player == 1)
        {
            entity = rrj_read32(0x8005B21C);
            if (entity)
            {
                enabled = ((rrj_read32(entity + 560) >> 27) ^ 1) & 1;
                camera = 0x800CDD04;
            }
            else
            {
                uint32_t primary = rrj_read32(0x8005B38C);
                uint32_t flags = rrj_read32(primary + 560);
                entity = rrj_read32(primary + 856);
                enabled = ((flags >> 27) ^ 1) & 1;
            }
        }
        else if (player == 2)
        {
            uint32_t primary = rrj_read32(0x8005B38C);
            entity = rrj_read32(primary + 856);
            enabled = 0;
            if (!(rrj_read32(primary + 560) & 0x08000000))
            {
                state = rrj_read32(0x8005B2F8);
                enabled = r_u8(state + 10) == 1;
            }
        }
        else
        {
            uint32_t secondary = rrj_read32(0x8005B21C);
            entity = rrj_read32(secondary + 856);
            enabled = 0;
            if (!(rrj_read32(secondary + 560) & 0x08000000))
            {
                state = rrj_read32(0x8005B2F8);
                enabled = r_u8(state + 11) == 1;
            }
        }
        state = rrj_read32(0x8005B2F8);
        if (r_s8(state) != 1 || !enabled)
        {
            sub_8001DD08(player, -1, 0);
            sub_8001DC94(player, -1);
            rrj_write32(counter, 0);
            continue;
        }
        if (rrj_read32(input + 16))
        {
            rrj_write32(0x800CE540 + 8 * player, (uint32_t)sub_8001CA58(r_u8(input + 9), 0x800D3984));
            rrj_write32(0x800CE544 + 8 * player, (uint32_t)sub_8001CA58(r_u8(input + 10), 0x800D3978));
        }
        {
            uint32_t proxy = rrj_read32(entity + 852);
            if (r_s16(proxy + 320))
            {
                if (rrj_read32(proxy + 604) < 2)
                {
                    uint32_t set_flags = 0, clear_flags = 0, pressed;
                    if (rrj_read32(input + 16))
                    {
                        int32_t first = r_s8(input + 146);
                        int32_t second = r_s8(input + 154);
                        set_flags = first > 0;
                        if (second > 0)
                            set_flags |= 0x20;
                        if (r_s8(input + 170) >= 2)
                            set_flags |= 4;
                        pressed = 0;
                        if (first >= 2 && r_s8(entity + 848) > 0)
                        {
                            pressed = 1;
                            set_flags |= 0x0C;
                        }
                        if (r_s8(input + 154) >= 2 && rrj_s32(rrj_read32(input + 164)) > 0)
                            set_flags |= 0x400;
                    }
                    else
                    {
                        uint32_t map = rrj_read32(input + 184);
                        uint32_t flags = rrj_read32(entity + 560);
                        pressed = rrj_s32(rrj_read32(input + 20 + 8 * rrj_read32(map + 12))) > 0;
                        if (!(flags & 2))
                            set_flags = (0u - pressed) & 3;
                        clear_flags = pressed ? 0 : 2;
                        pressed = rrj_s32(rrj_read32(input + 20 + 8 * rrj_read32(map + 16))) > 0;
                        if (!(flags & 0x40) && pressed)
                            set_flags |= 0x60;
                        if (!pressed)
                            clear_flags |= 0x40;
                        if (rrj_read32(input + 180) == 2)
                        {
                            pressed = rrj_s32(rrj_read32(input + 20 + 8 * rrj_read32(map + 20))) > 0;
                            if (pressed && rrj_s32(rrj_read32(input + 44)) > 0)
                                pressed = 0;
                            else if (pressed && rrj_s32(rrj_read32(input + 36)) > 0)
                                pressed = 1;
                            else
                            {
                                map = rrj_read32(input + 184);
                                pressed = rrj_s32(rrj_read32(input + 20 + 8 * rrj_read32(map + 20))) > 0;
                            }
                        }
                        else
                            pressed = rrj_s32(rrj_read32(input + 20 + 8 * rrj_read32(map + 20))) > 0;
                        flags = rrj_read32(entity + 560);
                        if (!(flags & 0x100))
                            set_flags |= pressed ? 0x180 : 0;
                        if (!pressed)
                        {
                            set_flags |= flags & 0x100 ? 0x80 : 0;
                            clear_flags |= 0x100;
                        }
                        if (rrj_read32(input + 180) == 2)
                        {
                            map = rrj_read32(input + 184);
                            pressed = rrj_s32(rrj_read32(input + 20 + 8 * rrj_read32(map + 24))) > 0;
                            if (pressed && rrj_s32(rrj_read32(input + 44)) > 0)
                                pressed = 0;
                            else if (pressed && rrj_s32(rrj_read32(input + 36)) > 0)
                                pressed = 1;
                            else
                            {
                                map = rrj_read32(input + 184);
                                pressed = rrj_s32(rrj_read32(input + 20 + 8 * rrj_read32(map + 24))) > 0;
                            }
                        }
                        else
                        {
                            map = rrj_read32(input + 184);
                            pressed = rrj_s32(rrj_read32(input + 20 + 8 * rrj_read32(map + 24))) > 0;
                        }
                        flags = rrj_read32(entity + 560);
                        if (!(flags & 0x200))
                            set_flags |= pressed ? 0x280 : 0;
                        if (!pressed)
                        {
                            set_flags |= flags & 0x200 ? 0x80 : 0;
                            clear_flags |= 0x200;
                        }
                        map = rrj_read32(input + 184);
                        if (r_s8(input + 26 + 8 * rrj_read32(map + 12)) >= 2)
                            set_flags |= 4;
                        pressed = rrj_s32(rrj_read32(input + 20 + 8 * rrj_read32(map + 28))) > 0;
                        if (pressed && r_s8(input + 26 + 8 * rrj_read32(map + 16)) >= 2)
                            set_flags |= 0x400;
                    }
                    set_flags |= 8 * pressed;
                    if (!pressed)
                        clear_flags |= 8;
                    rrj_write32(entity + 560, (rrj_read32(entity + 560) & ~clear_flags) | set_flags);
                    if (r_u16(entity + 948 + 8 * (uint32_t)r_s8(entity + 946)) == 16)
                        input_call(rrj_host_context(), call, 0x800C258C, input, entity, 0);
                    else
                        input_call(rrj_host_context(), call, 0x800C2348, input, entity, 0);
                    proxy = rrj_read32(entity + 852);
                    if (r_u16(proxy + 544) && !(r_u8(rrj_read32(entity + 1084)) & 0x40))
                    {
                        uint32_t id = rrj_read32(entity + 1088) ? r_u16(entity + 172) : r_u16(rrj_read32(entity + 856) + 172);
                        uint32_t bits = input_call(rrj_host_context(), call, 0x800C2218, input, entity, 0xFFFFFFFF);
                        rrj_write32(0x8005AD18 + 4 * id, rrj_read32(0x8005AD18 + 4 * id) | bits);
                    }
                    state = rrj_read32(0x8005B2F8);
                    {
                        uint32_t score_player = rrj_read32(state + 48) >= 2 ? player != 0 : 0;
                        if ((r_u8(state + 4) & 1) && rrj_s32(rrj_read32(state + 72 + 4 * score_player)) >= 18 && rrj_s32(rrj_read32(input + 44)) > 0 && r_s8(input + 74))
                        {
                            sub_80017BA0(0, 0, 107, 0);
                            sub_80018260();
                            proxy = rrj_read32(entity + 852);
                            if (r_u16(proxy + 544) && !(r_u8(rrj_read32(entity + 1084)) & 0x40))
                            {
                                uint32_t id = rrj_read32(entity + 1088) ? r_u16(entity + 172) : r_u16(rrj_read32(entity + 856) + 172);
                                uint32_t bits = input_call(rrj_host_context(), call, 0x800C2218, input, entity, 1);
                                rrj_write32(0x8005AD18 + 4 * id, rrj_read32(0x8005AD18 + 4 * id) | bits);
                            }
                        }
                    }
                    if (r_s8(input + 26 + 8 * rrj_read32(rrj_read32(input + 184) + 32)) > 0)
                        sub_8001A760(r_u16(entity + 172), 0);
                    pressed = rrj_s32(rrj_read32(input + 20 + 8 * rrj_read32(rrj_read32(input + 184) + 32))) > 0;
                    proxy = rrj_read32(entity + 852);
                    if (pressed)
                        rrj_write32(proxy + 552, rrj_read32(proxy + 552) | 0x00800000);
                    else
                        rrj_write32(proxy + 552, rrj_read32(proxy + 552) & 0xFF7FFFFF);
                }
                else if (!rrj_read32(rrj_read32(entity + 1084) + 40))
                {
                    uint32_t category = r_u16(0x800541D6 + 8 * r_u16(proxy + 544));
                    uint32_t analog = rrj_read32(input + 16);
                    uint32_t flags = 0, active = 0;
                    if (category == 8)
                    {
                        uint32_t first = rrj_read32(input + 36);
                        uint32_t second = rrj_read32(input + 76);
                        flags = (uint32_t)((rrj_s32(first) > 0) | (rrj_s32(second) > 0)) << 8;
                        if (rrj_s32(rrj_read32(input + 20)) > 0)
                            flags |= 0x200;
                        first = rrj_read32(input + 44);
                        second = rrj_read32(input + 52);
                        flags |= (uint32_t)((rrj_s32(first) > 0) | (rrj_s32(second) > 0)) << 7;
                        if (rrj_s32(rrj_read32(input + 28)) > 0)
                            flags |= 0x400;
                        active = flags != 0 || (analog && (rrj_read32(0x800CE540 + 8 * player) || rrj_read32(0x800CE544 + 8 * player)));
                        if (rrj_s32(rrj_read32(input + 68)) <= 0)
                            rrj_write32(proxy + 552, rrj_read32(proxy + 552) & ~0x1000u);
                    }
                    if (r_s8(input + 74) > 0)
                    {
                        flags |= 0x1000;
                        w_u8(0x800D6DE0 + 192 * player + 72, 255);
                        w_u8(0x800D6DE0 + 192 * player + 74, 1);
                    }
                    rrj_write32(proxy + 552, (rrj_read32(proxy + 552) & 0xFFFFF07F) | flags | ((0u - active) & 0x01000800));
                }
                if (camera)
                {
                    if (r_s8(input + 26 + 8 * rrj_read32(rrj_read32(input + 184))) && !rrj_read32(camera + 772))
                    {
                        sub_80017BA0(0, 0, 107, 0);
                        input_call(rrj_host_context(), call, 0x8008A998, camera, (rrj_read32(camera + 540) + 1) & 3, 0);
                    }
                    if (rrj_s32(rrj_read32(input + 20 + 8 * rrj_read32(rrj_read32(input + 184) + 8))) > 0)
                    {
                        if (rrj_read32(camera + 540) < 4)
                        {
                            uint32_t flags = rrj_read32(camera + 548);
                            if (!(flags & 1))
                                rrj_write32(camera + 548, flags | 7);
                        }
                    }
                    else
                    {
                        uint32_t flags = rrj_read32(camera + 548);
                        if (flags & 1)
                            rrj_write32(camera + 548, (flags & ~1u) | 6);
                    }
                }
            }
        }
        if (rrj_read32(0x8005B220))
            continue;
        if (rrj_read32(0x800D743C + 24 * player))
        {
            rrj_write32(counter, 0);
            continue;
        }
        {
            uint32_t ratio = 0, value = 0;
            uint32_t speed = rrj_read32(entity + 480);
            uint32_t acceleration = rrj_read32(entity + 676);
            uint32_t slow = rrj_s32(speed) <= 146485;
            if (acceleration)
            {
                uint32_t sign = (uint32_t)(rrj_s32(acceleration) >> 31);
                uint32_t magnitude = (sign + acceleration) ^ sign;
                uint32_t divisor = rrj_read32(rrj_read32(entity + 556) + 204);
                if (rrj_s32(magnitude) <= 0)
                {
                    magnitude = 0u - magnitude;
                    if (rrj_s32(divisor) > 0)
                        ratio = 0u - sub_80010028(magnitude, divisor);
                    else
                        ratio = sub_80010028(magnitude, 0u - divisor);
                }
                else if (rrj_s32(divisor) <= 0)
                    ratio = 0u - sub_80010028(magnitude, 0u - divisor);
                else
                    ratio = sub_80010028(magnitude, divisor);
            }
            if (rrj_s32(rrj_read32(entity + 700)) > 73727 && rrj_s32(ratio) < 32768)
                ratio = 32768;
            if (r_s16(entity + 320) && rrj_read32(rrj_read32(entity + 852) + 604) < 2 && (ratio || ((rrj_read32(entity + 388) & 1) && !slow) || (rrj_read32(entity + 180) < 9 && slow)))
            {
                if (ratio)
                {
                    value = 110 + (uint32_t)(rrj_s32(145u * ratio) / 65536);
                    rrj_write32(counter, 0);
                }
                else if (slow)
                {
                    uint32_t count = rrj_read32(counter) + 1;
                    rrj_write32(counter, count);
                    value = rrj_s32(count) < 601 ? 50 : 0;
                }
                if (rrj_read32(entity + 388) & 1)
                {
                    int32_t scaled = rrj_s32((uint32_t)sub_8001FC90(rrj_s32(rrj_read32(entity + 480)), 491520)) >> 16;
                    if (scaled >= 151)
                        scaled = 150;
                    rrj_write32(counter, 0);
                    if (scaled >= rrj_s32(value))
                        value = (uint32_t)scaled;
                }
                sub_8001DD08(player, -1, value);
            }
            else
            {
                sub_8001DD08(player, -1, 0);
                rrj_write32(counter, 0);
            }
        }
    }
    return 0;
}
