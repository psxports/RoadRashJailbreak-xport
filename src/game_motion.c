#include "first2_camera_surface.h"
#include "psx.h"
#include "game_main.h"
#include "fixed_math.h"
#include "draft_quick.h"
#include "race_pause.h"
#include "race_leaf.h"
#include "race_audio_frontier.h"
#include "native_game_api.h"

uint32_t sub_80097BCC(uint32_t actor, int32_t delta)
{
    uint32_t table, category, normal_address = 0, wall, record;
    uint32_t x, z, larger, smaller, approximation, sign, dot, value, other, flags;
    uint32_t angle, numerator, denominator, quotient, outcome, displacement[3];
    int32_t distance, facing, normal_y;
    int nearby = 0, barrier = 0, positive, y_positive;
    uint16_t reversed[3];
    const uint16_t *normal = NULL;
    unsigned axis;
    int64_t products[3];
    FUNCTION_MARKER(0x80097BCCu, "RASHCDG.BIN");
    if (!(r_u32(actor + 552u) & 0x18u))
    {
        category = r_u16(actor + 172u);
        if ((category >> 5) == 1u && (int32_t)(category & 31u) < r_s32(r_u32(0x8005B2F8u) + 48u))
        {
            table = 0x800D43C0u + 28u * r_u16(r_u32(actor + 596u) + 172u);
            if (r_s16(table + 22u) && r_u32(table) == r_u32(actor + 360u))
            {
                x = r_u32(table + 4u) - r_u32(actor + 184u);
                z = r_u32(table + 12u) - r_u32(actor + 192u);
                sign = (uint32_t)((int32_t)x >> 31);
                larger = (sign + (uint32_t)((int32_t)x >> 16)) ^ sign;
                sign = (uint32_t)((int32_t)z >> 31);
                smaller = (sign + (uint32_t)((int32_t)z >> 16)) ^ sign;
                if ((int32_t)larger < (int32_t)smaller)
                {
                    value = larger;
                    larger = smaller;
                    smaller = value;
                }
                value = smaller + (uint32_t)((int32_t)smaller >> 1);
                approximation = larger - (uint32_t)((int32_t)larger >> 5) - (uint32_t)((int32_t)larger >> 7);
                approximation += (uint32_t)((int32_t)value >> 2) + (uint32_t)((int32_t)value >> 6);
                if ((int32_t)approximation < 50)
                {
                    if ((int32_t)(uint32_t)sub_8002E698(xport_guest_cptr(actor + 450u, 6), xport_guest_cptr(table + 16u, 6)) <= 0)
                    {
                        x = r_u32(table + 4u) - r_u32(actor + 184u);
                        dot = (uint32_t)r_s16(table + 16u) * (uint32_t)((int32_t)x >> 16);
                        z = r_u32(table + 12u) - r_u32(actor + 192u);
                        dot += (uint32_t)r_s16(table + 20u) * (uint32_t)((int32_t)z >> 16);
                        nearby = (int32_t)dot > 0;
                    }
                    normal_address = table + 16u;
                }
            }
        }
        if ((r_u32(actor + 388u) & 1u) && r_u32(actor + 372u) && !(r_u32(actor + 36u) & 0x200u))
        {
            wall = r_u32(actor + 340u);
            distance = r_s32(actor + 344u);
            value = r_u8(wall + 49u);
            if ((distance > 0 && r_u8(wall + 48u)) || (distance <= 0 && value))
            {
                facing = (int32_t)(uint32_t)sub_8002E698(xport_guest_cptr(actor + 450u, 6), xport_guest_cptr(r_u32(actor + 340u) + 2u, 6));
                wall = r_u32(actor + 340u);
                barrier = ((int32_t)(r_u8(wall + 48u) << 15) < distance && facing > 0) || (distance < -(int32_t)(r_u8(wall + 49u) << 15) && facing < 0);
                if (barrier)
                {
                    normal_address = r_u32(actor + 340u) + 2u;
                    if (facing > 0)
                    {
                        xport_store_le16(reversed, (uint16_t)(0u - r_u16(normal_address)));
                        xport_store_le16((uint8_t *)reversed + 2u, (uint16_t)(0u - r_u16(normal_address + 2u)));
                        xport_store_le16((uint8_t *)reversed + 4u, (uint16_t)(0u - r_u16(normal_address + 4u)));
                        normal = reversed;
                    }
                }
            }
            else
            {
                record = sub_8003E338(actor);
                if (record)
                {
                    if ((int32_t)(uint32_t)sub_8002E698(xport_guest_cptr(actor + 450u, 6), xport_guest_cptr(record + 16u, 6)) <= 0)
                    {
                        x = r_u32(record + 4u) - r_u32(actor + 184u);
                        dot = (uint32_t)r_s16(record + 16u) * (uint32_t)((int32_t)x >> 16);
                        z = r_u32(record + 12u) - r_u32(actor + 192u);
                        dot += (uint32_t)r_s16(record + 20u) * (uint32_t)((int32_t)z >> 16);
                        barrier = (int32_t)dot > 0;
                    }
                    if (barrier)
                        normal_address = record + 16u;
                }
            }
        }
        if (barrier || nearby)
        {
            if (!normal)
                normal = xport_guest_cptr(normal_address, 6);
            w_u32(actor + 552u, r_u32(actor + 552u) | 2u);
            value = xport_load_le16(normal);
            flags = r_u32(actor + 552u);
            w_u16(actor + 622u, (uint16_t)value);
            w_u16(actor + 624u, xport_load_le16((const uint8_t *)normal + 2u));
            value = xport_load_le16((const uint8_t *)normal + 4u);
            w_u32(actor + 480u, 0u);
            w_u32(actor + 464u, 0u);
            w_u32(actor + 460u, 0u);
            w_u32(actor + 456u, 0u);
            w_u16(actor + 626u, (uint16_t)value);
            if (flags & 0x800u)
                w_u32(actor + 552u, (flags & 0xFFEFFEFFu) | 0x80u);
            else if (!(flags & 0x100000u))
            {
                w_u32(actor + 552u, flags | 0x100001u);
                value = sub_80010028(0x40000u, 0x80000u);
                w_u32(actor + 600u, value);
                dot = (uint32_t)sub_8002E698(normal, xport_guest_cptr(actor + 516u, 6));
                w_u16(actor + 516u, xport_load_le16(normal));
                w_u16(actor + 518u, xport_load_le16((const uint8_t *)normal + 2u));
                flags = r_u32(actor + 552u);
                value = xport_load_le16((const uint8_t *)normal + 4u);
                flags &= 0xFFF9FFFFu;
                w_u32(actor + 552u, flags);
                w_u16(actor + 520u, (uint16_t)value);
                if ((int32_t)dot < 0)
                {
                    w_u16(actor + 516u, (uint16_t)(0u - r_u16(actor + 516u)));
                    other = r_u16(actor + 518u);
                    w_u16(actor + 520u, (uint16_t)(0u - value));
                    w_u16(actor + 518u, (uint16_t)(0u - other));
                    flags = r_u32(actor + 552u) | 0x40000u;
                }
                else
                    flags |= 0x20000u;
                w_u32(actor + 552u, flags);
                w_u32(actor + 488u, 0u);
            }
        }
    }
    if (r_u32(actor + 604u) == 3u)
    {
        category = r_u16(actor + 172u);
        if (((category >> 5) == 1u && (int32_t)(category & 31u) < r_s32(r_u32(0x8005B2F8u) + 48u) || (r_u8(actor + 572u) & 0x20u)) && r_s32(actor + 480u) > 196608)
            (void)sub_80027778(actor, 3u, 300u, 1u);
        if (r_u32(actor + 552u) & 1u)
            (void)sub_8002EE50(r_u32(actor + 480u), xport_guest_cptr(actor + 450u, 6), xport_guest_ptr(actor + 456u, 12));
        (void)sub_8008F754(actor, delta, actor + 600u);
        (void)sub_8003FA18(9, xport_guest_cptr(actor + 432u, 18), xport_guest_ptr(actor + 516u, 18));
        outcome = sub_8008FD5C(actor);
        if (r_u32(actor + 552u) & 0x80000u)
            outcome = 0u;
    }
    else
    {
        value = r_u32(actor + 488u);
        if (value)
        {
            angle = ((163u * (uint32_t)sub_8001FC90((int32_t)value, delta)) >> 14) & 4095u;
            numerator = 0u - (uint32_t)((int32_t)r_s16(0x8005624Cu + 4u * angle) * 16);
            denominator = (uint32_t)((int32_t)r_s16(0x8005624Eu + 4u * angle) * 16);
            (void)sub_8002EB78(xport_guest_cptr(actor + 528u, 6), xport_guest_cptr(actor + 516u, 6), xport_guest_ptr(actor + 516u, 6), numerator, denominator);
        }
        if (r_u32(actor + 552u) & 1u)
        {
            for (axis = 0; axis < 3; ++axis)
                xport_gte_write_control(2u * axis, (uint32_t)(int32_t)r_s16(actor + 516u + 2u * axis));
            for (axis = 0; axis < 3; ++axis)
                xport_gte_write_data(9u + axis, (uint32_t)(int32_t)r_s16(actor + 522u + 2u * axis));
            xport_gte_execute(0x178000Cu);
            for (axis = 0; axis < 3; ++axis)
                w_u16(actor + 528u + 2u * axis, (uint16_t)xport_gte_read_data(9u + axis));
            if (!sub_8002E468(xport_guest_ptr(actor + 528u, 6)))
                (void)sub_8008CF74(actor);
            for (axis = 0; axis < 3; ++axis)
                xport_gte_write_control(2u * axis, (uint32_t)(int32_t)r_s16(actor + 522u + 2u * axis));
            for (axis = 0; axis < 3; ++axis)
                xport_gte_write_data(9u + axis, (uint32_t)(int32_t)r_s16(actor + 528u + 2u * axis));
            xport_gte_execute(0x178000Cu);
            for (axis = 0; axis < 3; ++axis)
                w_u16(actor + 516u + 2u * axis, (uint16_t)xport_gte_read_data(9u + axis));
            value = (uint32_t)sub_8001FC90((int32_t)r_s16(actor + 450u) * 16, (int32_t)r_s16(actor + 522u) * 16);
            value += (uint32_t)sub_8001FC90((int32_t)r_s16(actor + 454u) * 16, (int32_t)r_s16(actor + 526u) * 16);
            positive = (int32_t)value > 0;
            normal_y = (int32_t)r_s16(actor + 524u) * 16;
            y_positive = normal_y > 0;
            value = (uint32_t)sub_8001FC90((int32_t)r_s16(actor + 450u) * 16, (int32_t)r_s16(actor + 522u) * 16);
            value += (uint32_t)sub_8001FC90((int32_t)r_s16(actor + 454u) * 16, (int32_t)r_s16(actor + 526u) * 16);
            numerator = positive ? value : 0u - value;
            normal_y = (int32_t)r_s16(actor + 524u) * 16;
            denominator = y_positive ? (uint32_t)normal_y : 0u - (uint32_t)normal_y;
            quotient = sub_80010028(numerator, denominator);
            if (positive == y_positive)
                quotient = 0u - quotient;
            w_u16(actor + 452u, (uint16_t)(quotient >> 4));
            if (!sub_8002E468(xport_guest_ptr(actor + 450u, 6)))
                (void)sub_8008CF74(actor);
        }
        else if (r_u32(actor + 488u))
        {
            if (!sub_8002E468(xport_guest_ptr(actor + 516u, 6)))
                (void)sub_8008CF74(actor);
            for (axis = 0; axis < 3; ++axis)
                xport_gte_write_control(2u * axis, (uint32_t)(int32_t)r_s16(actor + 516u + 2u * axis));
            for (axis = 0; axis < 3; ++axis)
                xport_gte_write_data(9u + axis, (uint32_t)(int32_t)r_s16(actor + 522u + 2u * axis));
            xport_gte_execute(0x178000Cu);
            for (axis = 0; axis < 3; ++axis)
                w_u16(actor + 528u + 2u * axis, (uint16_t)xport_gte_read_data(9u + axis));
        }
        flags = r_u32(actor + 552u);
        if (flags & 0x10u)
            (void)sub_8002EAD8(xport_guest_cptr(actor + 456u, 12), xport_guest_cptr(actor + 516u, 6), r_u32(actor + 484u), xport_guest_ptr(actor + 184u, 12));
        else
        {
            if (flags & 0x60000u)
            {
                flags &= 0xFFFFF9FFu;
                w_u32(actor + 552u, flags);
                w_u32(actor + 552u, flags | ((flags & 0x20000u) ? 0x400u : 0x200u));
                for (axis = 0; axis < 3; ++axis)
                    displacement[axis] = r_u32(r_u32(actor + 596u) + 184u + 4u * axis) - r_u32(actor + 184u + 4u * axis);
                flags = r_u32(actor + 552u);
                dot = 0u;
                if (!(flags & 0x800u))
                {
                    for (axis = 0; axis < 3; ++axis)
                        products[axis] = (int64_t)(int32_t)displacement[axis] * ((int32_t)r_s16(actor + 528u + 2u * axis) * 16);
                    for (axis = 0; axis < 3; ++axis)
                        dot += (uint32_t)((uint64_t)products[axis] >> 16);
                }
                if ((flags & 0x800u) || (int32_t)dot > 0)
                    w_u32(actor + 552u, flags & 0xFFF9FFFFu);
            }
            if (r_u16(0x800541D6u + 8u * r_u16(actor + 544u)) == 8u)
                (void)sub_8009926C(actor, delta);
        }
        outcome = sub_800986D0(actor);
    }
    if (outcome)
        (void)sub_8009A038(r_u32(actor + 596u));
    if (r_u32(actor + 604u) == 4u || outcome)
    {
        x = r_u16(actor + 528u);
        value = r_u16(actor + 530u);
        z = r_u16(actor + 532u);
        w_u16(actor + 434u, 0u);
        w_u16(actor + 450u, (uint16_t)x);
        w_u16(actor + 452u, (uint16_t)value);
        w_u16(actor + 454u, (uint16_t)z);
        w_u16(actor + 432u, (uint16_t)z);
        w_u16(actor + 436u, (uint16_t)(0u - x));
        if (!sub_8002E468(xport_guest_ptr(actor + 432u, 6)))
        {
            w_u16(actor + 432u, 4096u);
            w_u16(actor + 436u, 0u);
        }
        w_u16(actor + 440u, 4096u);
        value = r_u16(actor + 436u);
        other = r_u16(actor + 432u);
        w_u16(actor + 446u, 0u);
        w_u16(actor + 438u, 0u);
        w_u16(actor + 442u, 0u);
        w_u16(actor + 444u, (uint16_t)(0u - value));
        w_u16(actor + 448u, (uint16_t)other);
    }
    value = r_u32(actor + 552u) & 0xFFFFFFFEu;
    w_u32(actor + 552u, value);
    return value;
}

uint32_t sub_8009A038(uint32_t actor)
{
    uint32_t entry, rider, identity, clock, state, event, flags, player;
    uint16_t x, y, z, forward_x, forward_y, forward_z;
    FUNCTION_MARKER(0x8009A038u, "RASHCDG.BIN");
    entry = actor + 948u + 8u * (uint32_t)(int32_t)r_s8(actor + 946u);
    w_u16(entry, 18u);
    identity = r_u16(actor + 172u);
    clock = r_u32(0x8005B2F8u);
    w_u16(entry + 6u, 0u);
    w_u16(entry + 2u, (uint16_t)identity);
    clock = ((2180u * r_u32(clock + 16u) + 0x8000u) >> 16) | 0xC000u;
    w_u16(entry + 6u, (uint16_t)clock);
    rider = r_u32(actor + 852u);
    if (r_u16(rider + 544u) == 44u)
    {
        if (r_u32(rider + 552u) & 0x08000000u)
        {
            forward_x = r_u16(rider + 444u);
            x = r_u16(rider + 432u);
            y = r_u16(rider + 434u);
            forward_y = r_u16(rider + 446u);
            z = r_u16(rider + 436u);
            forward_z = r_u16(rider + 448u);
            w_u16(rider + 444u, x);
            w_u16(rider + 432u, (uint16_t)(0u - forward_x));
            w_u16(rider + 446u, y);
            w_u16(rider + 434u, (uint16_t)(0u - forward_y));
            w_u16(rider + 448u, z);
            w_u16(rider + 436u, (uint16_t)(0u - forward_z));
        }
        else
        {
            forward_x = r_u16(rider + 444u);
            x = r_u16(rider + 432u);
            y = r_u16(rider + 434u);
            w_u16(rider + 432u, forward_x);
            forward_y = r_u16(rider + 446u);
            w_u16(rider + 446u, (uint16_t)(0u - y));
            z = r_u16(rider + 436u);
            w_u16(rider + 444u, (uint16_t)(0u - x));
            w_u16(rider + 434u, forward_y);
            forward_z = r_u16(rider + 448u);
            w_u16(rider + 448u, (uint16_t)(0u - z));
            w_u16(rider + 436u, forward_z);
        }
    }
    x = r_u16(rider + 444u);
    y = r_u16(rider + 446u);
    z = r_u16(rider + 448u);
    w_u16(rider + 450u, x);
    w_u16(rider + 452u, y);
    w_u16(rider + 454u, z);
    (void)sub_8003FA18(9, xport_guest_cptr(rider + 432u, 18), xport_guest_ptr(rider + 516u, 18));
    state = r_u16(rider + 544u);
    event = state == 51u || state == 45u || state == 61u || state == 62u || state == 68u || state == 57u || state == 53u ? 70u : 69u;
    state = r_u16(rider + 544u);
    flags = (state - 60u < 4u || state == 44u) ? 4096u : 0u;
    (void)sub_800C4550(event, rider, flags);
    clock = r_u32(0x8005B2F8u);
    w_u32(rider + 488u, 0u);
    w_u32(rider + 480u, 0u);
    identity = r_u16(actor + 172u);
    if (identity < r_u32(clock + 48u))
    {
        player = 0x800CD898u + 1132u * identity;
        (void)sub_8008AAB0(player);
        w_u32(player + 548u, (r_u32(player + 548u) | 0x100000u) & 0xF5FFFFFFu);
    }
    flags = (r_u32(rider + 552u) & 0xFEFFF07Fu) | 0x80u;
    w_u32(rider + 552u, flags);
    return flags;
}

uint32_t sub_800986D0(uint32_t actor)
{
    uint32_t state, category, value, first, second, selected, desired;
    uint32_t player, flags, x, y, z, record, timer, product, result;
    int recovery, state_group, active, first_group, second_group;
    int32_t speed;
    FUNCTION_MARKER(0x800986D0u, "RASHCDG.BIN");
    state = r_u16(actor + 544u);
    recovery = state - 69u < 2u;
    state_group = r_u16(0x800541D6u + 8u * state) == 8u;
    if (recovery || state_group)
    {
        result = 0u;
        value = sub_8005BE58(r_u32(actor + 540u));
        if (value || state_group)
        {
            active = 0;
            if (r_u8(r_u32(r_u32(actor + 596u) + 1084u) + 39u) != 254u)
            {
                if (recovery && !(r_u32(actor + 552u) & 0x800u))
                    active = 1;
                else if (r_s32(actor + 480u) > 0 || (r_u32(actor + 552u) & 0x10u))
                    active = 1;
            }
            first = r_u16(actor + 544u);
            first_group = recovery || first - 72u < 2u;
            second = r_u16(actor + 544u);
            first += (0u - (uint32_t)first_group) & (71u - second);
            second_group = recovery || second - 72u < 2u;
            value = r_u16(actor + 544u);
            selected = second + ((0u - (uint32_t)second_group) & (71u - value));
            desired = r_s32(actor + 480u) > 163839 ? 73u : 72u;
            selected = (selected + ((0u - (uint32_t)active) & (desired - first))) & 0xFFFFu;
            if (selected != r_u16(actor + 544u))
            {
                (void)sub_800C4550(selected, actor, ((0u - (uint32_t)recovery) & 0x1000u) | 0x83u);
                category = r_u16(actor + 172u);
                if ((category >> 5) == 1u && (int32_t)(category & 31u) < r_s32(r_u32(0x8005B2F8u) + 48u) && recovery)
                {
                    player = 0x800CD898u + 1132u * r_u16(r_u32(actor + 596u) + 172u);
                    if (!r_u32(player + 772u))
                    {
                        flags = r_u32(player + 548u);
                        x = r_u32(player + 572u);
                        y = r_u32(player + 576u);
                        z = r_u32(player + 580u);
                        w_u32(player + 548u, flags | 0x600000u);
                        w_u32(player + 804u, x);
                        w_u32(player + 808u, y);
                        w_u32(player + 812u, z);
                    }
                    w_u32(player + 548u, r_u32(player + 548u) & 0xFFEFFFFFu);
                }
            }
        }
    }
    else
        result = sub_8005BE58(r_u32(actor + 540u));
    if (r_u16(actor + 544u) - 72u < 2u)
    {
        record = r_u32(r_u32(actor + 540u) + 4u);
        if (r_u8(record + 1u) == 3u || r_u8(record + 13u) == 3u)
            timer = 15u;
        else
        {
            speed = r_s32(actor + 480u);
            if (speed > 524287)
                timer = 22u;
            else if (speed > 163839)
            {
                product = 0u - 11u * (uint32_t)sub_8001FC90(11915, (int32_t)((uint32_t)speed - 524288u));
                timer = (uint32_t)((int32_t)product >> 16) + 33u;
            }
            else if (speed > 65535)
            {
                product = 0u - 10u * (uint32_t)sub_8001FC90(43690, (int32_t)((uint32_t)speed - 163840u));
                timer = (uint32_t)((int32_t)product >> 16) + 50u;
            }
            else
                timer = 50u;
        }
        w_u32(r_u32(actor + 540u) + 24u, timer ? timer : 10u);
    }
    return result;
}

uint32_t sub_8008FD5C(uint32_t actor)
{
    int32_t speed, frames;
    uint32_t state, stopped = 0, timer, product, animation, index, dictionary, sequence;
    uint32_t frame_count, current_frame, value, flags, event, random, output_flags;
    FUNCTION_MARKER(0x8008FD5Cu, "RASHCDG.BIN");
    speed = r_s32(actor + 480u);
    if (!speed)
    {
        state = r_u16(actor + 544u);
        if (state - 60u >= 4u)
            stopped = state - 67u >= 2u;
    }
    state = r_u16(actor + 544u);
    if (state == 44u)
        return sub_8005BE58(r_u32(actor + 540u));
    if (state == 55u)
    {
        if (sub_8005BE58(r_u32(actor + 540u)))
        {
            value = (r_u32(0x80054354u) >> 4) & 4095u;
            sequence = r_u32(0x8005B3E4u);
            sequence = sequence ? r_u32(sequence + 192u) : 0u;
            (void)sub_8005C018(r_u32(actor + 540u), value, 0u, 4u, 15u, 0u, sequence);
            (void)sub_800C2FF4(48u, actor, 0u);
        }
        return stopped;
    }
    if (state >= 48u && state <= 51u)
    {
        if (speed > 1464860)
            timer = 4u;
        else if (speed > 102539)
        {
            product = (uint32_t)sub_8001FC90(3152, (int32_t)((uint32_t)speed - 102540u));
            if (state >= 50u)
                product = 0u - 26u * product;
            else
                product = 0u - 16u * product;
            timer = (uint32_t)((int32_t)product >> 16) + (state >= 50u ? 30u : 20u);
        }
        else
            timer = state >= 50u ? 30u : 20u;
        w_u32(r_u32(actor + 540u) + 24u, timer ? timer : 10u);
        if (state >= 50u)
            return speed <= 87890;
        if (state == 48u)
        {
            animation = r_u32(actor + 540u);
            index = r_u32(animation + 12u);
            dictionary = r_u32(animation + 40u);
            sequence = r_u32(animation + 4u);
            index = r_u8(sequence + 12u * index);
            dictionary = r_u32(dictionary + 4u);
            sequence = r_u32(dictionary + 4u * index);
            frame_count = r_u16(sequence + 16u);
            current_frame = r_u32(animation + 16u);
            frames = 3 * (int32_t)(int16_t)(uint16_t)(frame_count - 1u);
            if (stopped || (speed <= 117187 && frames / 4 < (int32_t)current_frame))
            {
                flags = (r_u32(actor + 552u) & 0x08000000u) ? 2u : 258u;
                (void)sub_800C4550(44u, actor, flags);
                return 0u;
            }
            if (sub_8005BE58(r_u32(actor + 540u)))
                (void)sub_800C4550(48u, actor, 1u);
            return stopped;
        }
        if (!sub_8005BE58(r_u32(actor + 540u)))
            return stopped;
        value = r_u8(actor + 535u) + 1u;
        w_u8(actor + 535u, (uint8_t)value);
        event = 49u;
        flags = 0u;
        if ((int8_t)(uint8_t)value > 0)
        {
            random = sub_8001FC58() % 3u;
            event = (random ? 55u : 57u) + (random == 1u);
            flags = 2u;
        }
        (void)sub_800C4550(event, actor, flags);
        return stopped;
    }
    if (!sub_8005BE58(r_u32(actor + 540u)))
        return stopped;
    event = sub_800C45D8(actor, &output_flags);
    stopped = stopped || (uint16_t)(event - 69u) < 2u;
    if (!stopped)
        (void)sub_800C4550(event & 0xFFFFu, actor, output_flags);
    return stopped;
}

uint32_t sub_8009926C(uint32_t actor, int32_t delta)
{
    uint32_t rate = 174762u, cap = 524288u, held, attached, category, player, index;
    uint32_t flags, value, magnitude, left, right, neutral, positive, neutral_positive, neutral_negative, boost;
    uint32_t distance, numerator, divisor, ratio, change, acceleration, speed, gap, turn, lower, result;
    int numerator_positive, divisor_positive;
    FUNCTION_MARKER(0x8009926Cu, "RASHCDG.BIN");
    held = (r_u32(actor + 552u) >> 11) & 1u;
    attached = (r_u8(actor + 572u) >> 5) & 1u;
    if (held)
    {
        category = r_u16(actor + 172u);
        if (((category >> 5) == 1u && (int32_t)(category & 31u) < r_s32(r_u32(0x8005B2F8u) + 48u)) || attached)
        {
            player = r_u32(actor + 596u);
            if (attached)
                player = r_u32(player + 856u);
            value = r_u32(r_u32(0x8005B2F8u) + 48u);
            index = r_u16(player + 172u) + attached;
            if (value >= 2u)
                index += attached;
            if (r_u32(0x800D7138u + 192u * index))
            {
                value = r_u32(0x800CE544u + 8u * index);
                if (value)
                {
                    flags = r_u32(actor + 552u) & 0xFFFFF9FFu;
                    w_u32(actor + 552u, flags);
                    w_u32(actor + 552u, flags | ((int32_t)value > 0 ? 0x400u : 0x200u));
                    magnitude = (int32_t)value > 0 ? value : 0u - value;
                    rate = (uint32_t)sub_8001FC90((int32_t)rate, (int32_t)magnitude);
                }
                flags = r_u32(actor + 552u);
                if (!(flags & 0x80u))
                {
                    value = r_u32(0x800CE540u + 8u * index);
                    if (value)
                    {
                        flags &= 0xFFFFFE7Fu;
                        w_u32(actor + 552u, flags);
                        if ((int32_t)value >= 0)
                            w_u32(actor + 552u, flags | 0x80u);
                        else
                        {
                            w_u32(actor + 552u, flags | 0x100u);
                            cap = (uint32_t)sub_8001FC90((int32_t)cap, (int32_t)(0u - value));
                            w_u32(actor + 480u, cap);
                        }
                    }
                }
            }
        }
    }
    flags = r_u32(actor + 552u);
    left = (flags >> 9) & 1u;
    right = (flags >> 10) & 1u;
    neutral = (left | right) == 0u;
    positive = r_s32(actor + 488u) > 0;
    neutral_positive = neutral & positive;
    neutral_negative = neutral & (positive ^ 1u);
    boost = neutral | (left & positive) | (positive ? 0u : right);
    if (boost)
        rate *= 3u;
    if (!held)
    {
        value = r_u32(actor + 484u);
        distance = (uint32_t)((int32_t)(value + ((int32_t)value < 0 ? 3u : 0u)) >> 2);
        if ((int32_t)distance > 196608)
            distance = 196608u;
        if ((int32_t)distance > 98303)
        {
            numerator_positive = (int32_t)cap > 0;
            divisor_positive = (int32_t)distance > 0;
            numerator = numerator_positive ? cap : 0u - cap;
            divisor = divisor_positive ? distance : 0u - distance;
            ratio = sub_80010028(numerator, divisor);
            if (numerator_positive != divisor_positive)
                ratio = 0u - ratio;
            if ((int32_t)rate >= (int32_t)ratio)
                rate = ratio;
            else
                cap = (uint32_t)sub_8001FC90((int32_t)rate, (int32_t)distance);
        }
        else
        {
            value = (uint32_t)sub_8001FC90((int32_t)rate, (int32_t)distance);
            if ((int32_t)cap < (int32_t)value)
            {
                numerator_positive = (int32_t)cap > 0;
                divisor_positive = (int32_t)distance > 0;
                numerator = numerator_positive ? cap : 0u - cap;
                divisor = divisor_positive ? distance : 0u - distance;
                rate = sub_80010028(numerator, divisor);
                if (numerator_positive != divisor_positive)
                    rate = 0u - rate;
            }
            else
                cap = (int32_t)value < 262144 ? 262144u : value;
        }
    }
    if (r_u32(actor + 552u) & 0x80u)
        cap = 0u;
    change = (uint32_t)sub_8001FC90(524288, delta);
    if (!(r_u32(actor + 552u) & 0x100u) && (!held || neutral))
        change = 0u - 3u * change;
    acceleration = ((0u - (left | neutral_positive)) & 0xFFFAAAABu) + ((0u - (right | neutral_negative)) & 0x55555u);
    acceleration += (0u - boost) & (2u * acceleration);
    speed = r_u32(actor + 480u) + change;
    gap = cap - speed;
    value = (~(uint32_t)((int32_t)speed >> 31) & speed) + ((uint32_t)((int32_t)gap >> 31) & gap);
    w_u32(actor + 480u, value);
    value = (uint32_t)sub_8001FC90((int32_t)acceleration, delta);
    turn = r_u32(actor + 488u) + value;
    w_u32(actor + 488u, turn);
    lower = neutral_positive ? 0u : 0u - rate;
    if (neutral_negative)
        rate = 0u;
    gap = rate - turn;
    result = (uint32_t)((int32_t)gap >> 31) & gap;
    value = turn + ((uint32_t)((int32_t)(turn - lower) >> 31) & (lower - turn)) + result;
    w_u32(actor + 488u, value);
    return result;
}

uint32_t sub_8008F754(uint32_t actor, int32_t delta, uint32_t blend)
{
    uint32_t state, x, z, sign, absolute_x, absolute_z, first, second, value, result;
    uint32_t vector[3], dot, speed, magnitude, denominator, reciprocal, old_speed;
    uint32_t lower, damping, animation, record, flags;
    uint16_t previous[3], direction[3], up[3];
    int steep, aligned = 1, protected_state;
    unsigned axis;
    int64_t products[3];
    FUNCTION_MARKER(0x8008F754u, "RASHCDG.BIN");
    steep = r_s16(actor + 524u) < 3547;
    state = r_u16(actor + 544u);
    if (state == 45u || state == 50u || state == 51u)
    {
        x = r_u16(actor + 450u);
        z = r_u16(actor + 454u);
        sign = (uint32_t)((int32_t)(int16_t)x >> 31);
        absolute_x = (sign + (uint32_t)(int32_t)(int16_t)x) ^ sign;
        sign = (uint32_t)((int32_t)(int16_t)z >> 31);
        absolute_z = (sign + (uint32_t)(int32_t)(int16_t)z) ^ sign;
        value = (int32_t)absolute_z < (int32_t)absolute_x ? x ^ r_u16(actor + 444u) : z ^ r_u16(actor + 448u);
        aligned = !(value & 0x8000u);
    }
    first = (uint32_t)sub_8001FC90((int32_t)r_s16(actor + 446u) * 16, 643207);
    second = (uint32_t)sub_8001FC90((int32_t)r_s16(actor + 434u) * 16, 643207);
    (void)sub_8002ECB8(xport_guest_cptr(actor + 444u, 6), xport_guest_cptr(actor + 432u, 6), vector, first, second);
    result = r_u32(actor + 480u);
    if (!result && steep)
    {
        if ((r_u16(actor + 172u) >> 5) == 1u)
        {
            for (axis = 0; axis < 3; ++axis)
                products[axis] = (int64_t)(int32_t)vector[axis] * ((int32_t)r_s16(actor + 622u + 2u * axis) * 16);
            dot = 0u;
            for (axis = 0; axis < 3; ++axis)
                dot += (uint32_t)((uint64_t)products[axis] >> 16);
            if ((int32_t)dot > 0)
                w_u32(actor + 480u, 65536u);
        }
        result = r_u32(actor + 480u);
    }
    if (!result)
        return result;
    state = r_u16(actor + 544u);
    protected_state = state == 45u || state == 55u || state == 56u || state == 57u || state == 52u || state == 53u || state == 58u || state == 59u || state == 54u || state == 49u;
    value = r_u32(blend) - (uint32_t)delta;
    w_u32(blend, value);
    if ((int32_t)value < 0)
        value = 0u;
    w_u32(blend, value);
    lower = (int32_t)value < 65536 ? 65536u : value;
    value += (0u - (uint32_t)steep) & (lower - value);
    w_u32(blend, value);
    state = r_u16(actor + 544u);
    lower = state - 50u < 2u ? 0xFFFF0000u : 0xFFFF8000u;
    damping = (uint32_t)sub_8001FC90((int32_t)value, delta);
    second = (uint32_t)sub_8001FC90((int32_t)((0u - (uint32_t)(!steep && !protected_state)) & lower), delta);
    (void)sub_8002E6F8(vector, xport_guest_cptr(actor + 456u, 12), vector, (int32_t)damping, (int32_t)second);
    w_u32(actor + 456u, r_u32(actor + 456u) + vector[0]);
    w_u32(actor + 460u, r_u32(actor + 460u) + vector[1]);
    value = r_u32(actor + 464u) + vector[2];
    old_speed = r_u32(actor + 480u);
    w_u32(actor + 464u, value);
    speed = sub_8002E548(xport_guest_cptr(actor + 456u, 12));
    w_u32(actor + 480u, speed);
    value = r_u32(blend);
    w_u32(blend, (int32_t)value > 0 ? value - (uint32_t)delta : 0u);
    for (axis = 0; axis < 3; ++axis)
        xport_store_le16((uint8_t *)previous + 2u * axis, r_u16(actor + 450u + 2u * axis));
    speed = r_u32(actor + 480u);
    if (speed)
    {
        magnitude = (int32_t)speed < 0 ? 0u - speed : speed;
        denominator = (uint32_t)((int32_t)magnitude >> 1) + (uint32_t)((int32_t)(magnitude - 2u) >> 31);
        reciprocal = denominator ? 0x80000000u / denominator : 0xFFFFFFFFu;
        if ((int32_t)speed < 0)
            reciprocal = 0u - reciprocal;
        (void)sub_8002EED8(reciprocal, xport_guest_cptr(actor + 456u, 12), xport_guest_ptr(actor + 450u, 6));
    }
    if ((int32_t)(uint32_t)sub_8002E698(xport_guest_cptr(actor + 450u, 6), previous) < 0)
    {
        animation = r_u32(actor + 540u);
        value = r_u32(animation + 12u);
        record = r_u32(animation + 4u) + 12u * value;
        value = r_u8(record + 2u);
        if (value & 1u)
        {
            w_u8(record + 2u, (uint8_t)(value & 0xFEu));
            flags = r_u32(actor + 552u) & 0xF7FFFFFFu;
        }
        else
        {
            w_u8(record + 2u, (uint8_t)(value | 1u));
            flags = r_u32(actor + 552u) | 0x08000000u;
        }
        w_u32(actor + 552u, flags);
    }
    speed = r_u32(actor + 480u);
    if ((int32_t)speed < (int32_t)old_speed && (protected_state || (steep && (int32_t)speed <= 87890)))
    {
        w_u32(actor + 480u, old_speed);
        (void)sub_8002EE50(old_speed, xport_guest_cptr(actor + 450u, 6), xport_guest_ptr(actor + 456u, 12));
    }
    for (axis = 0; axis < 3; ++axis)
        direction[axis] = r_u16(actor + 450u + 2u * axis);
    for (axis = 0; axis < 3; ++axis)
        w_u16(actor + 444u + 2u * axis, direction[axis]);
    if (!aligned)
        for (axis = 0; axis < 3; ++axis)
            w_u16(actor + 444u + 2u * axis, (uint16_t)(0u - direction[axis]));
    for (axis = 0; axis < 3; ++axis)
        up[axis] = r_u16(actor + 522u + 2u * axis);
    for (axis = 0; axis < 3; ++axis)
        w_u16(actor + 438u + 2u * axis, up[axis]);
    for (axis = 0; axis < 3; ++axis)
        xport_gte_write_control(2u * axis, (uint32_t)(int32_t)r_s16(actor + 438u + 2u * axis));
    for (axis = 0; axis < 3; ++axis)
        xport_gte_write_data(9u + axis, (uint32_t)(int32_t)r_s16(actor + 444u + 2u * axis));
    xport_gte_execute(0x178000Cu);
    for (axis = 0; axis < 3; ++axis)
        w_u16(actor + 432u + 2u * axis, (uint16_t)xport_gte_read_data(9u + axis));
    if (!sub_8002E468(xport_guest_ptr(actor + 432u, 6)))
        (void)sub_8008CF74(actor);
    for (axis = 0; axis < 3; ++axis)
        xport_gte_write_control(2u * axis, (uint32_t)(int32_t)r_s16(actor + 432u + 2u * axis));
    for (axis = 0; axis < 3; ++axis)
        xport_gte_write_data(9u + axis, (uint32_t)(int32_t)r_s16(actor + 438u + 2u * axis));
    xport_gte_execute(0x178000Cu);
    for (axis = 0; axis < 3; ++axis)
        w_u16(actor + 444u + 2u * axis, (uint16_t)xport_gte_read_data(9u + axis));
    result = r_u16(actor + 444u);
    value = r_u16(actor + 446u);
    z = r_u16(actor + 448u);
    w_u16(actor + 450u, (uint16_t)result);
    w_u16(actor + 452u, (uint16_t)value);
    w_u16(actor + 454u, (uint16_t)z);
    if (!aligned)
    {
        w_u16(actor + 450u, (uint16_t)(0u - result));
        result = 0u - value;
        w_u16(actor + 452u, (uint16_t)result);
        w_u16(actor + 454u, (uint16_t)(0u - z));
    }
    return result;
}

uint32_t sub_800C341C(uint32_t actor)
{
    uint32_t owner, animation, frames, table, frame, value, fraction, product, sign, complement;
    int32_t speed, last, numerator, denominator;
    FUNCTION_MARKER(0x800C341C, "RASHCDG.BIN");
    owner = r_u32(actor + 596);
    speed = r_s32(owner + 616);
    if (r_u8(actor + 572) & 0x20)
        owner = r_u32(owner + 856);
    animation = r_u32(actor + 540);
    frame = r_u32(animation + 12);
    frames = r_u32(animation + 4);
    table = r_u32(animation + 40);
    frame = r_u8(frames + 12 * frame);
    table = r_u32(table + 4);
    frame = r_u32(table + 4 * frame);
    last = (int16_t)(uint16_t)(r_u16(frame + 16) - 1);
    w_u32(actor + 580, r_u32(owner + 184) - r_u32(owner + 504));
    w_u32(actor + 584, r_u32(owner + 188) - r_u32(owner + 508));
    w_u32(actor + 588, r_u32(owner + 192) - r_u32(owner + 512));
    (void)sub_8003FA18(9, xport_guest_cptr(owner + 432, 18), xport_guest_ptr(actor + 432, 18));
    if (r_s32(owner + 620) < 0 && speed >= -45752)
    {
        sign = (uint32_t)(speed >> 31);
        value = (uint32_t)(sub_8001FC90(((uint32_t)speed + sign) ^ sign, 93873));
        complement = 65536 - value;
        value = (value & ~(uint32_t)((int32_t)value >> 31)) + (complement & (uint32_t)((int32_t)complement >> 31));
        value *= (uint32_t)last;
        animation = r_u32(actor + 540);
        w_u32(animation + 36, r_u32(animation + 36) | 8);
        animation = r_u32(actor + 540);
        w_u32(animation + 16, (uint32_t)((int32_t)value >> 16));
        animation = r_u32(actor + 540);
        fraction = value - (r_u32(animation + 16) << 16);
        product = r_u32(animation + 24) * fraction;
        w_u32(animation + 32, (uint32_t)((int32_t)product >> 16));
        animation = r_u32(actor + 540);
        w_u32(animation + 28, 0);
        return animation;
    }
    animation = r_u32(actor + 540);
    numerator = (int32_t)(r_u32(animation + 32) << 16);
    denominator = r_s32(animation + 24);
    if (!denominator)
        fraction = numerator < 0 ? 1 : 0xFFFFFFFF;
    else if (numerator == INT32_MIN && denominator == -1)
        fraction = 0x80000000;
    else
        fraction = (uint32_t)(numerator / denominator);
    value = r_u32(animation + 16) << 16;
    w_u32(animation + 36, r_u32(animation + 36) | 8);
    value += fraction + ((uint32_t)last << 16);
    animation = r_u32(actor + 540);
    w_u32(animation + 16, (uint32_t)((int32_t)value >> 16));
    animation = r_u32(actor + 540);
    fraction = value - (r_u32(animation + 16) << 16);
    product = r_u32(animation + 24) * fraction;
    w_u32(animation + 32, (uint32_t)((int32_t)product >> 16));
    w_u32(r_u32(actor + 540) + 28, 0);
    return sub_80091468(actor);
}

uint32_t sub_800C3630(uint32_t actor)
{
    uint32_t owner, animation, frames, table, frame, magnitude, sign, value, complement, product, fraction;
    int32_t speed, last;
    FUNCTION_MARKER(0x800C3630, "RASHCDG.BIN");
    owner = r_u32(actor + 596);
    animation = r_u32(actor + 540);
    speed = r_s32(owner + 652);
    frame = r_u32(animation + 12);
    table = r_u32(animation + 40);
    frames = r_u32(animation + 4);
    sign = (uint32_t)(speed >> 31);
    frame = r_u8(frames + 12 * frame);
    table = r_u32(table + 4);
    frame = r_u32(table + 4 * frame);
    magnitude = ((uint32_t)speed + sign) ^ sign;
    last = (int16_t)(uint16_t)(r_u16(frame + 16) - 1);
    if ((int32_t)magnitude > 80066)
    {
        w_u32(animation + 36, r_u32(animation + 36) | 8);
        animation = r_u32(actor + 540);
        w_u32(animation + 16, (uint32_t)last);
        animation = r_u32(actor + 540);
        fraction = ((uint32_t)last - r_u32(animation + 16)) << 16;
        product = r_u32(animation + 24) * fraction;
        w_u32(animation + 32, (uint32_t)((int32_t)product >> 16));
        w_u32(r_u32(actor + 540) + 28, 0);
        return sub_80091468(actor);
    }
    value = (uint32_t)(sub_8001FC90(magnitude - 45752, 125164));
    complement = 65536 - value;
    value = (value & ~(uint32_t)((int32_t)value >> 31)) + (complement & (uint32_t)((int32_t)complement >> 31));
    value *= (uint32_t)last;
    animation = r_u32(actor + 540);
    w_u32(animation + 36, r_u32(animation + 36) | 8);
    animation = r_u32(actor + 540);
    w_u32(animation + 16, (uint32_t)((int32_t)value >> 16));
    animation = r_u32(actor + 540);
    fraction = value - (r_u32(animation + 16) << 16);
    product = r_u32(animation + 24) * fraction;
    w_u32(animation + 32, (uint32_t)((int32_t)product >> 16));
    animation = r_u32(actor + 540);
    w_u32(animation + 28, 0);
    return animation;
}

uint32_t sub_800C31CC(uint32_t actor)
{
    uint32_t owner, animation, frame, frames, table, negative = 0, motion, value, sign, magnitude, product, fraction;
    int32_t speed, last;
    int finish;
    FUNCTION_MARKER(0x800C31CC, "RASHCDG.BIN");
    value = (r_u32(actor + 552) >> 27) & 1;
    owner = r_u32(actor + 596);
    motion = (r_u32(owner + 568) >> 5) & 1;
    speed = r_s32(owner + 636);
    if (r_u32(owner + 856) && !value)
        negative = (uint32_t)speed >> 31;
    animation = r_u32(actor + 540);
    frame = r_u32(animation + 12);
    frames = r_u32(animation + 4);
    table = r_u32(animation + 40);
    frame = r_u8(frames + 12 * frame);
    table = r_u32(table + 4);
    frame = r_u32(table + 4 * frame);
    last = (int16_t)(uint16_t)(r_u16(frame + 16) - 1);
    if (!motion)
    {
        finish = sub_8005BE58(r_u32(actor + 540)) != 0;
        if (!finish)
        {
            sign = (uint32_t)(speed >> 31);
            magnitude = ((uint32_t)speed + sign) ^ sign;
            if ((int32_t)magnitude <= 51471)
                return 0;
        }
        finish = 1;
    }
    else if (negative)
        finish = speed <= -34314;
    else
        finish = !speed || (last ? speed > 0 : speed < 0);
    if (finish)
    {
        animation = r_u32(actor + 540);
        w_u32(animation + 36, r_u32(animation + 36) | 8);
        animation = r_u32(actor + 540);
        w_u32(animation + 16, (uint32_t)last);
        animation = r_u32(actor + 540);
        fraction = ((uint32_t)last - r_u32(animation + 16)) << 16;
        product = r_u32(animation + 24) * fraction;
        w_u32(animation + 32, (uint32_t)((int32_t)product >> 16));
        w_u32(r_u32(actor + 540) + 28, 0);
        return sub_80091468(actor);
    }
    value = (uint32_t)speed + ((0 - (uint32_t)last) & (0 - (uint32_t)speed - (uint32_t)speed));
    value = (uint32_t)(0 - sub_8001FC90(value, 125164));
    if (!negative)
        value += 65536;
    value *= (uint32_t)last;
    animation = r_u32(actor + 540);
    w_u32(animation + 36, r_u32(animation + 36) | 8);
    animation = r_u32(actor + 540);
    w_u32(animation + 16, (uint32_t)((int32_t)value >> 16));
    animation = r_u32(actor + 540);
    fraction = value - (r_u32(animation + 16) << 16);
    product = r_u32(animation + 24) * fraction;
    w_u32(animation + 32, (uint32_t)((int32_t)product >> 16));
    w_u32(r_u32(actor + 540) + 28, 0);
    animation = r_u32(actor + 540);
    w_u32(animation + 32, 0);
    return animation;
}
