#include "rrj_debug.h"
#include "first2_runtime_dependencies.h"
#include "race_leaf_batch_006.h"
#include "race_leaf.h"
#include "race_pause.h"
#include "fixed_math.h"
#include "menu_latch.h"
#include "xport.h"
#include <stdio.h>
#include <stdlib.h>

uint32_t sub_8008B428(uint32_t source, uint32_t mask)
{
    uint32_t scalar_value_3;
    uint32_t scalar_value_4;

    uint32_t scalar_value_1;
    uint32_t scalar_sign_1;
    uint32_t scalar_value_2;
    uint32_t scalar_sign_2;

    uint32_t first_x = r_u32(0x800CCF98);
    uint32_t first_z = r_u32(0x800CCFA0);
    uint32_t source_x = r_u32(source + 12);
    uint32_t source_z = r_u32(source + 20);
    uint32_t source_id = r_u16(source);
    uint32_t column_origin = (uint32_t)(((int32_t)(uint32_t)(source_x - first_x)) >> 21) + 11;
    uint32_t row_origin = (uint32_t)(((int32_t)(uint32_t)(source_z - first_z)) >> 21) + 11;
    uint32_t players = r_u32(r_u32(0x8005B2F8) + 48);
    uint32_t extent = 1, page, best_id = 224, best_distance = 0x7FFF0000;
    FUNCTION_MARKER(0x8008B428, "RASHCDG.BIN");
    if (players != 1 && (column_origin >= 24 || row_origin >= 24))
    {
        uint32_t second_x = r_u32(0x800CCF9C);
        uint32_t second_z = r_u32(0x800CCFA4);
        column_origin = (uint32_t)(((int32_t)(uint32_t)(source_x - second_x)) >> 21) + 11;
        row_origin = (uint32_t)(((int32_t)(uint32_t)(source_z - second_z)) >> 21) + 11;
        row_origin += 24u << ((uint32_t)(((int32_t)(uint32_t)(row_origin)) >> 31) & 31);
    }
    players = r_u32(r_u32(0x8005B2F8) + 48);
    page = ((int32_t)(uint32_t)(row_origin)) >= 24 && ((int32_t)(uint32_t)(row_origin)) < ((int32_t)(uint32_t)(24u << ((players - 1) & 31))) ? 24 : 0;
    row_origin -= page;
    while (column_origin < 24 || row_origin < 24 || column_origin + extent < 24 || row_origin + extent < 24)
    {
        uint32_t first_column = ((int32_t)(uint32_t)(column_origin)) < 0 ? 0 : column_origin;
        uint32_t first_row = ((int32_t)(uint32_t)(row_origin)) < 0 ? 0 : row_origin;
        uint32_t columns = ((scalar_value_3 = (column_origin + extent)), (((uint32_t)(((int32_t)(uint32_t)(scalar_value_3 - 24)) < 0 ? scalar_value_3 : 24))));
        uint32_t rows = ((scalar_value_4 = (row_origin + extent)), (((uint32_t)(((int32_t)(uint32_t)(scalar_value_4 - 24)) < 0 ? scalar_value_4 : 24))));
        uint32_t row;
        for (row = first_row; ((int32_t)(uint32_t)(row)) < ((int32_t)(uint32_t)(rows)); ++row)
        {
            uint32_t column;
            for (column = first_column; ((int32_t)(uint32_t)(column)) < ((int32_t)(uint32_t)(columns)); ++column)
            {
                uint32_t link = r_u8(0x800CD0B0 + 24 * (page + row) + column);
                if (link != 128)
                {
                    uint32_t actor_base = r_u32(0x8005B3A0);
                    uint32_t object_base = r_u32(0x8005B3A4);
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
                                uint32_t stride = r_u32(group + 4);
                                uint32_t base = r_u32(group);
                                uint32_t offset = r_u32(0x800CCA68 + 4 * (kind - 2));
                                candidate = base + stride * (encoded & 31) + offset;
                                coordinate_offset = 0;
                            }
                            identity = r_u16(candidate + coordinate_offset);
                            if (identity != source_id && (kind != 1 || r_u32(candidate + 604) == 4))
                            {
                                uint32_t x = r_u32(candidate + coordinate_offset + 12);
                                uint32_t origin_x = r_u32(source + 12);
                                uint32_t z = r_u32(candidate + coordinate_offset + 20);
                                uint32_t origin_z = r_u32(source + 20);
                                uint32_t dx = ((scalar_value_1 = (x - origin_x)), (scalar_sign_1 = (uint32_t)(((int32_t)(uint32_t)(scalar_value_1)) >> 31)), (((uint32_t)((scalar_value_1 + scalar_sign_1) ^ scalar_sign_1))));
                                uint32_t dz = ((scalar_value_2 = (z - origin_z)), (scalar_sign_2 = (uint32_t)(((int32_t)(uint32_t)(scalar_value_2)) >> 31)), (((uint32_t)((scalar_value_2 + scalar_sign_2) ^ scalar_sign_2))));
                                uint32_t minor = ((int32_t)(uint32_t)(dx)) < ((int32_t)(uint32_t)(dz)) ? dx : dz;
                                uint32_t distance = dx + dz - (uint32_t)(((int32_t)(uint32_t)(minor)) / 2);
                                if (((int32_t)(uint32_t)(distance)) < ((int32_t)(uint32_t)(best_distance)))
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

/* Optional host fixture hooks are inactive during native gameplay */
typedef struct InputProbeHooks
{
    void (*primary)(uint32_t input, uint32_t entity);
    void (*secondary)(uint32_t input, uint32_t entity);
    uint32_t (*bits)(uint32_t input, uint32_t entity, int32_t direction);
    void (*camera)(uint32_t camera, uint32_t mode);
} InputProbeHooks;

static InputProbeHooks input_probe_hooks;
static uint32_t input_probe_args[6];

static FILE *input_probe_stream(void)
{
    FILE *stream = (FILE *)rrj_host_context()->sdk_user;
    if (!stream)
        abort();
    return stream;
}

static void input_probe_actor(uint32_t address, uint32_t input, uint32_t entity)
{
    RRJ_DEBUG_FPRINTF(input_probe_stream(), "%08X %08X %08X\n", address, input, entity);
    if (input_probe_args[2] & 1)
        w_u32(entity + 852, input_probe_args[3]);
    if (input_probe_args[2] & 2)
        w_u32(0x8005B2F8, input_probe_args[4]);
}

static void input_probe_primary(uint32_t input, uint32_t entity)
{
    input_probe_actor(0x800C2348, input, entity);
}

static void input_probe_secondary(uint32_t input, uint32_t entity)
{
    input_probe_actor(0x800C258C, input, entity);
}

static uint32_t input_probe_bits(uint32_t input, uint32_t entity, int32_t direction)
{
    RRJ_DEBUG_FPRINTF(input_probe_stream(), "%08X %08X %08X %08X\n", 0x800C2218u, input, entity, (uint32_t)direction);
    if (input_probe_args[2] & 4)
        w_u16(entity + 172, 9);
    return input_probe_args[1];
}

static void input_probe_camera(uint32_t camera, uint32_t mode)
{
    RRJ_DEBUG_FPRINTF(input_probe_stream(), "%08X %08X %08X\n", 0x8008A998u, camera, mode);
    (void)sub_8008A998(camera, mode);
}

uint32_t rrj_input_audit_probe(RRJMemory *m, const uint32_t args[6])
{
    InputProbeHooks saved = input_probe_hooks;
    unsigned index;
    for (index = 0; index < 6; ++index)
        input_probe_args[index] = args[index];
    m->cpu_status = args[0];
    input_probe_hooks.primary = input_probe_primary;
    input_probe_hooks.secondary = input_probe_secondary;
    input_probe_hooks.bits = input_probe_bits;
    input_probe_hooks.camera = input_probe_camera;
    (void)sub_8001CB3C();
    input_probe_hooks = saved;
    return m->cpu_status;
}

uint32_t sub_8001CB3C(void)
{
    uint32_t state, players, player, changed = 0;
    int32_t mode;
    FUNCTION_MARKER(0x8001CB3C, "SLUS_010.53");
    sub_80043DA4();
    sub_8001E0B4(xport_guest_ptr(0x800D7128, 768), xport_guest_ptr(0x800D6DE0, 1), 768);
    state = r_u32(0x8005B2F8);
    players = r_u32(state + 52);
    for (player = 0; ((int32_t)(uint32_t)(player)) < ((int32_t)(uint32_t)(players)); ++player)
    {
        uint32_t button, record = 0x800D6DF4 + 192 * player;
        for (button = 0; button < 19; ++button, record += 8)
        {
            uint32_t held = r_u32(record);
            w_u8(record + 6, 0);
            if (((int32_t)(uint32_t)(held)) < 0)
                held = 0;
            w_u32(record, held);
        }
    }
    state = r_u32(0x8005B2F8);
    mode = r_s8(state);
    if (mode == 2)
    {
        sub_80043DB4();
        return 0;
    }
    if (mode == 1 && r_u32(0x8005B478))
    {
        w_u8(state, 3);
        changed = 1;
    }
    else
    {
        state = r_u32(0x8005B2F8);
        players = r_u32(state + 52);
        player = 0;
        if (players)
        {
            do
            {
                uint32_t input = 0x800D7128 + 192 * player;
                uint32_t map = r_u32(input + 184);
                if (r_s8(input + 8 * r_u32(map + 4) + 26))
                {
                    if ((r_u32(state) & 0xFF0000FF) == 1)
                    {
                        w_u8(state, 3);
                        w_u32(0x8005AD90, player);
                        changed = 1;
                    }
                    else if (r_s8(state) == 6)
                        w_u32(0x8005AF50, 1);
                }
                state = r_u32(0x8005B2F8);
                players = r_u32(state + 52);
                ++player;
            } while (player < players);
        }
    }
    state = r_u32(0x8005B2F8);
    if (r_s8(state + 3))
    {
        w_u8(state, 1);
        changed = 1;
    }
    state = r_u32(0x8005B2F8);
    mode = r_s8(state);
    if (mode == 1 && !changed)
        w_u32(state + 24, r_u32(state + 12) - r_u32(state + 20));
    else if (mode == 1 && r_s8(state + 2))
    {
        w_u32(state + 28, 15);
        w_u32(state + 24, 15);
    }
    else
    {
        w_u32(state + 28, 0);
        w_u32(state + 24, 0);
    }
    state = r_u32(0x8005B2F8);
    {
        uint32_t exiting = r_u32(0x8005B220);
        w_u32(state + 20, r_u32(state + 12));
        if (exiting)
        {
            if (r_u32(0x800D712C) || r_u32(0x800D71EC) || r_u32(0x800D72AC) || r_u32(0x800D736C))
            {
                state = r_u32(0x8005B2F8);
                w_u8(state, 2);
            }
            return 0;
        }
    }
    sub_80043DB4();
    if (changed)
    {
        state = r_u32(0x8005B2F8);
        sub_80018C1C(r_s8(state) == 3);
    }
    {
        uint32_t pad_kind = r_u8(0x800D70E1) >> 4;
        state = r_u32(0x8005B2F8);
        players = r_u32(state + 52);
        if (pad_kind != 8 && ((int32_t)(uint32_t)(players)) >= 3)
            players = 2;
    }
    for (player = 0; ((int32_t)(uint32_t)(player)) < ((int32_t)(uint32_t)(players)); ++player)
    {
        uint32_t input = 0x800D7128 + 192 * player;
        uint32_t entity, enabled, camera = 0;
        uint32_t counter = 0x800D6DD0 + 4 * player;
        if (!player)
        {
            entity = r_u32(0x8005B38C);
            enabled = ((r_u32(entity + 560) >> 27) ^ 1) & 1;
            camera = 0x800CD898;
        }
        else if (player == 1)
        {
            entity = r_u32(0x8005B21C);
            if (entity)
            {
                enabled = ((r_u32(entity + 560) >> 27) ^ 1) & 1;
                camera = 0x800CDD04;
            }
            else
            {
                uint32_t primary = r_u32(0x8005B38C);
                uint32_t flags = r_u32(primary + 560);
                entity = r_u32(primary + 856);
                enabled = ((flags >> 27) ^ 1) & 1;
            }
        }
        else if (player == 2)
        {
            uint32_t primary = r_u32(0x8005B38C);
            entity = r_u32(primary + 856);
            enabled = 0;
            if (!(r_u32(primary + 560) & 0x08000000))
            {
                state = r_u32(0x8005B2F8);
                enabled = r_u8(state + 10) == 1;
            }
        }
        else
        {
            uint32_t secondary = r_u32(0x8005B21C);
            entity = r_u32(secondary + 856);
            enabled = 0;
            if (!(r_u32(secondary + 560) & 0x08000000))
            {
                state = r_u32(0x8005B2F8);
                enabled = r_u8(state + 11) == 1;
            }
        }
        state = r_u32(0x8005B2F8);
        if (r_s8(state) != 1 || !enabled)
        {
            sub_8001DD08(player, -1, 0);
            sub_8001DC94(player, -1);
            w_u32(counter, 0);
            continue;
        }
        if (r_u32(input + 16))
        {
            w_u32(0x800CE540 + 8 * player, (uint32_t)sub_8001CA58(r_u8(input + 9), 0x800D3984));
            w_u32(0x800CE544 + 8 * player, (uint32_t)sub_8001CA58(r_u8(input + 10), 0x800D3978));
        }
        {
            uint32_t proxy = r_u32(entity + 852);
            if (r_s16(proxy + 320))
            {
                if (r_u32(proxy + 604) < 2)
                {
                    uint32_t set_flags = 0, clear_flags = 0, pressed;
                    if (r_u32(input + 16))
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
                        if (r_s8(input + 154) >= 2 && ((int32_t)(uint32_t)(r_u32(input + 164))) > 0)
                            set_flags |= 0x400;
                    }
                    else
                    {
                        uint32_t map = r_u32(input + 184);
                        uint32_t flags = r_u32(entity + 560);
                        pressed = ((int32_t)(uint32_t)(r_u32(input + 20 + 8 * r_u32(map + 12)))) > 0;
                        if (!(flags & 2))
                            set_flags = (0u - pressed) & 3;
                        clear_flags = pressed ? 0 : 2;
                        pressed = ((int32_t)(uint32_t)(r_u32(input + 20 + 8 * r_u32(map + 16)))) > 0;
                        if (!(flags & 0x40) && pressed)
                            set_flags |= 0x60;
                        if (!pressed)
                            clear_flags |= 0x40;
                        if (r_u32(input + 180) == 2)
                        {
                            pressed = ((int32_t)(uint32_t)(r_u32(input + 20 + 8 * r_u32(map + 20)))) > 0;
                            if (pressed && ((int32_t)(uint32_t)(r_u32(input + 44))) > 0)
                                pressed = 0;
                            else if (pressed && ((int32_t)(uint32_t)(r_u32(input + 36))) > 0)
                                pressed = 1;
                            else
                            {
                                map = r_u32(input + 184);
                                pressed = ((int32_t)(uint32_t)(r_u32(input + 20 + 8 * r_u32(map + 20)))) > 0;
                            }
                        }
                        else
                            pressed = ((int32_t)(uint32_t)(r_u32(input + 20 + 8 * r_u32(map + 20)))) > 0;
                        flags = r_u32(entity + 560);
                        if (!(flags & 0x100))
                            set_flags |= pressed ? 0x180 : 0;
                        if (!pressed)
                        {
                            set_flags |= flags & 0x100 ? 0x80 : 0;
                            clear_flags |= 0x100;
                        }
                        if (r_u32(input + 180) == 2)
                        {
                            map = r_u32(input + 184);
                            pressed = ((int32_t)(uint32_t)(r_u32(input + 20 + 8 * r_u32(map + 24)))) > 0;
                            if (pressed && ((int32_t)(uint32_t)(r_u32(input + 44))) > 0)
                                pressed = 0;
                            else if (pressed && ((int32_t)(uint32_t)(r_u32(input + 36))) > 0)
                                pressed = 1;
                            else
                            {
                                map = r_u32(input + 184);
                                pressed = ((int32_t)(uint32_t)(r_u32(input + 20 + 8 * r_u32(map + 24)))) > 0;
                            }
                        }
                        else
                        {
                            map = r_u32(input + 184);
                            pressed = ((int32_t)(uint32_t)(r_u32(input + 20 + 8 * r_u32(map + 24)))) > 0;
                        }
                        flags = r_u32(entity + 560);
                        if (!(flags & 0x200))
                            set_flags |= pressed ? 0x280 : 0;
                        if (!pressed)
                        {
                            set_flags |= flags & 0x200 ? 0x80 : 0;
                            clear_flags |= 0x200;
                        }
                        map = r_u32(input + 184);
                        if (r_s8(input + 26 + 8 * r_u32(map + 12)) >= 2)
                            set_flags |= 4;
                        pressed = ((int32_t)(uint32_t)(r_u32(input + 20 + 8 * r_u32(map + 28)))) > 0;
                        if (pressed && r_s8(input + 26 + 8 * r_u32(map + 16)) >= 2)
                            set_flags |= 0x400;
                    }
                    set_flags |= 8 * pressed;
                    if (!pressed)
                        clear_flags |= 8;
                    w_u32(entity + 560, (r_u32(entity + 560) & ~clear_flags) | set_flags);
                    if (r_u16(entity + 948 + 8 * (uint32_t)r_s8(entity + 946)) == 16)
                    {
                        if (input_probe_hooks.secondary)
                            input_probe_hooks.secondary(input, entity);
                        else
                            (void)sub_800C258C(input, entity);
                    }
                    else
                    {
                        if (input_probe_hooks.primary)
                            input_probe_hooks.primary(input, entity);
                        else
                            (void)sub_800C2348(input, entity);
                    }
                    proxy = r_u32(entity + 852);
                    if (r_u16(proxy + 544) && !(r_u8(r_u32(entity + 1084)) & 0x40))
                    {
                        uint32_t id = r_u32(entity + 1088) ? r_u16(entity + 172) : r_u16(r_u32(entity + 856) + 172);
                        uint32_t bits = input_probe_hooks.bits ? input_probe_hooks.bits(input, entity, -1) : sub_800C2218(input, entity, -1);
                        w_u32(0x8005AD18 + 4 * id, r_u32(0x8005AD18 + 4 * id) | bits);
                    }
                    state = r_u32(0x8005B2F8);
                    {
                        uint32_t score_player = r_u32(state + 48) >= 2 ? player != 0 : 0;
                        if ((r_u8(state + 4) & 1) && ((int32_t)(uint32_t)(r_u32(state + 72 + 4 * score_player))) >= 18 && ((int32_t)(uint32_t)(r_u32(input + 44))) > 0 && r_s8(input + 74))
                        {
                            sub_80017BA0(0, 0, 107, 0);
                            sub_80018260();
                            proxy = r_u32(entity + 852);
                            if (r_u16(proxy + 544) && !(r_u8(r_u32(entity + 1084)) & 0x40))
                            {
                                uint32_t id = r_u32(entity + 1088) ? r_u16(entity + 172) : r_u16(r_u32(entity + 856) + 172);
                                uint32_t bits = input_probe_hooks.bits ? input_probe_hooks.bits(input, entity, 1) : sub_800C2218(input, entity, 1);
                                w_u32(0x8005AD18 + 4 * id, r_u32(0x8005AD18 + 4 * id) | bits);
                            }
                        }
                    }
                    if (r_s8(input + 26 + 8 * r_u32(r_u32(input + 184) + 32)) > 0)
                        sub_8001A760(r_u16(entity + 172), 0);
                    pressed = ((int32_t)(uint32_t)(r_u32(input + 20 + 8 * r_u32(r_u32(input + 184) + 32)))) > 0;
                    proxy = r_u32(entity + 852);
                    if (pressed)
                        w_u32(proxy + 552, r_u32(proxy + 552) | 0x00800000);
                    else
                        w_u32(proxy + 552, r_u32(proxy + 552) & 0xFF7FFFFF);
                }
                else if (!r_u32(r_u32(entity + 1084) + 40))
                {
                    uint32_t category = r_u16(0x800541D6 + 8 * r_u16(proxy + 544));
                    uint32_t analog = r_u32(input + 16);
                    uint32_t flags = 0, active = 0;
                    if (category == 8)
                    {
                        uint32_t first = r_u32(input + 36);
                        uint32_t second = r_u32(input + 76);
                        flags = (uint32_t)((((int32_t)(uint32_t)(first)) > 0) | (((int32_t)(uint32_t)(second)) > 0)) << 8;
                        if (((int32_t)(uint32_t)(r_u32(input + 20))) > 0)
                            flags |= 0x200;
                        first = r_u32(input + 44);
                        second = r_u32(input + 52);
                        flags |= (uint32_t)((((int32_t)(uint32_t)(first)) > 0) | (((int32_t)(uint32_t)(second)) > 0)) << 7;
                        if (((int32_t)(uint32_t)(r_u32(input + 28))) > 0)
                            flags |= 0x400;
                        active = flags != 0 || (analog && (r_u32(0x800CE540 + 8 * player) || r_u32(0x800CE544 + 8 * player)));
                        if (((int32_t)(uint32_t)(r_u32(input + 68))) <= 0)
                            w_u32(proxy + 552, r_u32(proxy + 552) & ~0x1000u);
                    }
                    if (r_s8(input + 74) > 0)
                    {
                        flags |= 0x1000;
                        w_u8(0x800D6DE0 + 192 * player + 72, 255);
                        w_u8(0x800D6DE0 + 192 * player + 74, 1);
                    }
                    w_u32(proxy + 552, (r_u32(proxy + 552) & 0xFFFFF07F) | flags | ((0u - active) & 0x01000800));
                }
                if (camera)
                {
                    if (r_s8(input + 26 + 8 * r_u32(r_u32(input + 184))) && !r_u32(camera + 772))
                    {
                        sub_80017BA0(0, 0, 107, 0);
                        if (input_probe_hooks.camera)
                            input_probe_hooks.camera(camera, (r_u32(camera + 540) + 1) & 3);
                        else
                            (void)sub_8008A998(camera, (r_u32(camera + 540) + 1) & 3);
                    }
                    if (((int32_t)(uint32_t)(r_u32(input + 20 + 8 * r_u32(r_u32(input + 184) + 8)))) > 0)
                    {
                        if (r_u32(camera + 540) < 4)
                        {
                            uint32_t flags = r_u32(camera + 548);
                            if (!(flags & 1))
                                w_u32(camera + 548, flags | 7);
                        }
                    }
                    else
                    {
                        uint32_t flags = r_u32(camera + 548);
                        if (flags & 1)
                            w_u32(camera + 548, (flags & ~1u) | 6);
                    }
                }
            }
        }
        if (r_u32(0x8005B220))
            continue;
        if (r_u32(0x800D743C + 24 * player))
        {
            w_u32(counter, 0);
            continue;
        }
        {
            uint32_t ratio = 0, value = 0;
            uint32_t speed = r_u32(entity + 480);
            uint32_t acceleration = r_u32(entity + 676);
            uint32_t slow = ((int32_t)(uint32_t)(speed)) <= 146485;
            if (acceleration)
            {
                uint32_t sign = (uint32_t)(((int32_t)(uint32_t)(acceleration)) >> 31);
                uint32_t magnitude = (sign + acceleration) ^ sign;
                uint32_t divisor = r_u32(r_u32(entity + 556) + 204);
                if (((int32_t)(uint32_t)(magnitude)) <= 0)
                {
                    magnitude = 0u - magnitude;
                    if (((int32_t)(uint32_t)(divisor)) > 0)
                        ratio = 0u - sub_80010028(magnitude, divisor);
                    else
                        ratio = sub_80010028(magnitude, 0u - divisor);
                }
                else if (((int32_t)(uint32_t)(divisor)) <= 0)
                    ratio = 0u - sub_80010028(magnitude, 0u - divisor);
                else
                    ratio = sub_80010028(magnitude, divisor);
            }
            if (((int32_t)(uint32_t)(r_u32(entity + 700))) > 73727 && ((int32_t)(uint32_t)(ratio)) < 32768)
                ratio = 32768;
            if (r_s16(entity + 320) && r_u32(r_u32(entity + 852) + 604) < 2 && (ratio || ((r_u32(entity + 388) & 1) && !slow) || (r_u32(entity + 180) < 9 && slow)))
            {
                if (ratio)
                {
                    value = 110 + (uint32_t)(((int32_t)(uint32_t)(145u * ratio)) / 65536);
                    w_u32(counter, 0);
                }
                else if (slow)
                {
                    uint32_t count = r_u32(counter) + 1;
                    w_u32(counter, count);
                    value = ((int32_t)(uint32_t)(count)) < 601 ? 50 : 0;
                }
                if (r_u32(entity + 388) & 1)
                {
                    int32_t scaled = ((int32_t)(uint32_t)((uint32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(entity + 480))), 491520))) >> 16;
                    if (scaled >= 151)
                        scaled = 150;
                    w_u32(counter, 0);
                    if (scaled >= ((int32_t)(uint32_t)(value)))
                        value = (uint32_t)scaled;
                }
                sub_8001DD08(player, -1, value);
            }
            else
            {
                sub_8001DD08(player, -1, 0);
                w_u32(counter, 0);
            }
        }
    }
    return 0;
}
