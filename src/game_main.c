#include "game_main.h"
#include "psx_memory.h"
#include "race_pause.h"
#include "race_leaf_batch_004.h"
#include "xport.h"
#include "fixed_math.h"
#include "native_game_api.h"
#include "race_leaf.h"

uint32_t sub_800A9868(uint32_t actor, const uint16_t normal[3], int32_t impact_speed,
    uint32_t other_direction, uint32_t other_identity, uint32_t force)
{
    uint32_t rider;
    uint32_t stop = 0;
    uint32_t strong = 0;
    uint32_t accepted = 0;
    uint32_t flags;
    int32_t original_speed;

    FUNCTION_MARKER(0x800A9868, "RASHCDG.BIN");
    rider = (r_u16(actor + 172) >> 5) == 1 ? actor : 0;
    flags = rrj_read32(actor + 552);
    original_speed = rrj_s32(rrj_read32(actor + 480));
    if (!(flags & 0x40000000u) && impact_speed <= 146486 && !force)
    {
        uint32_t state;
        int32_t dot;
        uint32_t x;
        uint32_t y;
        uint32_t z;
        uint32_t ratio;

        if (!rider || (flags & 0x20000000u))
        {
            stop = 1;
            goto finish;
        }
        state = rrj_read32(rider + 552);
        if (state & 0x800u)
        {
            rrj_write32(rider + 552, (state & 0xFFEFFEFFu) | 0x80u);
            stop = 1;
            goto finish;
        }
        if ((uint32_t)(r_u16(rider + 544) - 69u) < 2 || (state & 0x100018u))
            goto finish;
        dot = rrj_s32(sub_8002E698(normal, rrj_at(rider + 450, 6)));
        if (dot >= 0)
            goto finish;
        rrj_write32(rider + 552, rrj_read32(rider + 552) | 0x100001u);
        if (other_identity == rider + 172)
        {
            ratio = sub_80010028(0x40000u, 0x80000u);
        }
        else
        {
            uint32_t sum = rrj_read32(actor + 304);
            int32_t half;

            sum += rrj_read32(actor + 308);
            half = rrj_s32(sum + (sum >> 31)) >> 1;
            ratio = half > 0 ? sub_80010028((uint32_t)half, 0x80000u) :
                0u - sub_80010028(0u - (uint32_t)half, 0x80000u);
        }
        rrj_write32(rider + 600, ratio);
        dot = rrj_s32(sub_8002E698(normal, rrj_at(rider + 516, 6)));
        x = rrj_u16(normal);
        w_u16(rider + 516, (uint16_t)x);
        y = rrj_u16((const uint8_t *)normal + 2);
        w_u16(rider + 518, (uint16_t)y);
        state = rrj_read32(rider + 552);
        z = rrj_u16((const uint8_t *)normal + 4);
        state &= 0xFFF9FFFFu;
        rrj_write32(rider + 552, state);
        w_u16(rider + 520, (uint16_t)z);
        if (dot < 0)
        {
            x = r_u16(rider + 516);
            w_u16(rider + 516, (uint16_t)(0u - x));
            y = r_u16(rider + 518);
            w_u16(rider + 520, (uint16_t)(0u - z));
            w_u16(rider + 518, (uint16_t)(0u - y));
            state = rrj_read32(rider + 552) | 0x40000u;
        }
        else
        {
            state |= 0x20000u;
        }
        rrj_write32(rider + 552, state);
        rrj_write32(rider + 488, 0);
        goto finish;
    }
    if ((uint32_t)(r_u16(actor + 544) - 60u) < 4 && !force)
        goto finish;
    if (r_u16(0x800541D6u + 8u * r_u16(actor + 544)) == 5)
        (void)sub_80091468(rider);
    accepted = 1;
    if (rrj_read32(actor + 552) & 0x40000000u)
    {
        uint32_t contact = sub_800A966C(actor, normal, 0x80000);

        if (contact == 2)
        {
            if (r_u16(actor + 544) != 43)
                (void)sub_800C4550(43, actor, 3);
        }
        else if (!contact)
            accepted = 0;
    }
    else
    {
        uint32_t animation = rrj_read32(actor + 540);
        uint32_t frame_index = rrj_read32(animation + 12);
        uint32_t animation_table = rrj_read32(animation + 40);
        uint32_t frames = rrj_read32(animation + 4);
        uint32_t frame_kind = r_u8(frames + 12u * frame_index);
        uint32_t sequences = rrj_read32(animation_table + 4);
        uint32_t sequence = rrj_read32(sequences + 4u * frame_kind);
        uint32_t frame_count = r_u16(sequence + 16);
        int32_t frame = rrj_s32(rrj_read32(animation + 16));
        uint32_t event = 0;
        int32_t signed_count = (int16_t)(uint16_t)(frame_count - 1u);
        int32_t halfway = (signed_count + (signed_count < 0)) >> 1;

        if (!force)
            event = r_u16(actor + 544);

        if (force || event == 67 || event == 68 ||
            ((uint32_t)(event - 69u) < 2 && frame < halfway))
        {
            if ((uint32_t)(r_u16(actor + 544) - 67u) >= 2 ||
                sub_8005BE58(rrj_read32(actor + 540)))
            {
                event = r_u16(actor + 544);
                event = 67u + (event == 70 || event == 51 || event == 45 ||
                    event == 61 || event == 62 || event == 68 || event == 57 || event == 53);
                (void)sub_800C4550(event, actor, 2);
            }
            stop = 1;
        }
        else if (impact_speed > 585944)
        {
            strong = 1;
        }
        else
        {
            stop = 1;
            if (!(rrj_read32(actor + 552) & 0x20000000u))
            {
                int32_t dot = rrj_s32(sub_8002E698(rrj_at(other_direction, 6), rrj_at(actor + 444, 6)));
                int32_t magnitude = rrj_s32(dot < 0 ? 0u - (uint32_t)dot : (uint32_t)dot);

                event = dot > 0 ? (magnitude < 0x4000 ? 61u : 60u) :
                    (magnitude < 0x4000 ? 63u : 62u);
                (void)sub_800C4550(event, actor, 2);
            }
        }
    }
    if (strong)
    {
        uint32_t forward = rrj_s32(sub_8002E698(rrj_at(other_direction, 6), rrj_at(actor + 444, 6))) > 0;
        uint32_t random;
        int32_t rotation;
        int32_t component;
        uint32_t product;
        uint32_t x;
        uint32_t y;
        uint32_t z;
        uint32_t angle;
        uint32_t speed;
        int32_t cosine;
        uint32_t event;

        if (!(rrj_read32(actor + 552) & 0x20000000u))
            (void)sub_8002EAD8(rrj_at(actor + 184, 12), rrj_at(actor + 438, 6), 0xFFFF0000u, rrj_at(actor + 184, 12));
        rrj_write32(actor + 480, (uint32_t)sub_8001FC90(72089, impact_speed));
        random = sub_8001FC58();
        rotation = (int16_t)(uint16_t)((random % 714u) - 357u);
        component = (int16_t)r_u16(other_direction + 4);
        product = (uint32_t)((int64_t)rotation * component);
        x = r_u16(other_direction);
        w_u16(actor + 450, (uint16_t)(x + (uint32_t)(rrj_s32(product) >> 12)));
        y = r_u16(other_direction + 2);
        w_u16(actor + 452, (uint16_t)y);
        component = (int16_t)r_u16(other_direction);
        product = (uint32_t)((int64_t)rotation * component);
        y = (uint32_t)(int32_t)(int16_t)r_u16(actor + 452) << 4;
        z = r_u16(other_direction + 4);
        w_u16(actor + 454, (uint16_t)(z - (uint32_t)(rrj_s32(product) >> 12)));
        angle = sub_8001FF3C(y);
        speed = rrj_read32(actor + 480);
        cosine = (int16_t)r_u16(0x8005624Eu + 4u * ((1365u - angle) & 0xFFFu));
        (void)sub_8007E868(actor + 450, rrj_s32(speed), cosine, 580648);
        (void)sub_8002EE50(rrj_read32(actor + 480), rrj_at(actor + 450, 6), rrj_at(actor + 456, 12));
        x = r_u16(actor + 450);
        y = r_u16(actor + 452);
        w_u16(actor + 434, 0);
        w_u16(actor + 444, (uint16_t)(forward ? x : 0u - x));
        z = r_u16(actor + 454);
        w_u16(actor + 446, (uint16_t)(forward ? y : 0u - y));
        x = r_u16(actor + 444);
        z = forward ? z : 0u - z;
        w_u16(actor + 448, (uint16_t)z);
        w_u16(actor + 432, (uint16_t)z);
        w_u16(actor + 436, (uint16_t)(0u - x));
        (void)sub_8002E468(actor + 432);
        x = (uint32_t)(int32_t)(int16_t)r_u16(actor + 444);
        y = (uint32_t)(int32_t)(int16_t)r_u16(actor + 446);
        xport_gte_write_control(0u, x);
        z = (uint32_t)(int32_t)(int16_t)r_u16(actor + 448);
        xport_gte_write_control(2u, y);
        xport_gte_write_control(4u, z);
        x = (uint32_t)(int32_t)(int16_t)r_u16(actor + 432);
        y = (uint32_t)(int32_t)(int16_t)r_u16(actor + 434);
        z = (uint32_t)(int32_t)(int16_t)r_u16(actor + 436);
        xport_gte_write_data(9u, x);
        xport_gte_write_data(10u, y);
        xport_gte_write_data(11u, z);
        xport_gte_execute(0x178000Cu);
        x = xport_gte_read_data(9u);
        y = xport_gte_read_data(10u);
        z = xport_gte_read_data(11u);
        w_u16(actor + 438, (uint16_t)x);
        w_u16(actor + 440, (uint16_t)y);
        w_u16(actor + 442, (uint16_t)z);
        rrj_write32(actor + 552, rrj_read32(actor + 552) | 0xC0000000u);
        event = 43;
        if (forward)
        {
            random = sub_8001FC58() % 3u;
            event = random == 0 ? 49u : 46u + (random == 1);
        }
        if (event != r_u16(actor + 544))
            (void)sub_800C4550(event, actor, 3);
        if (rider)
        {
            uint32_t state = rrj_read32(rider + 552);
            uint32_t tag = r_u16(rider + 172);
            uint32_t session = rrj_read32(0x8005B2F8);
            int32_t players;

            rrj_write32(rider + 552, state | 0x20u);
            rrj_write32(rider + 580, rrj_read32(0x800D3964));
            players = rrj_s32(rrj_read32(session + 48));
            if ((tag >> 5) == 1 && rrj_s32(tag & 31u) < players)
            {
                uint32_t body = rrj_read32(rider + 596);
                uint32_t player = 0x800CD898u + 1132u * r_u16(body + 172);

                if (!rrj_read32(player + 772))
                {
                    state = rrj_read32(player + 548);
                    if (!(state & 0x8000u))
                    {
                        x = rrj_read32(player + 572);
                        y = rrj_read32(player + 576);
                        z = rrj_read32(player + 580);
                        rrj_write32(player + 548, state | 0x600000u);
                        rrj_write32(player + 804, x);
                        rrj_write32(player + 808, y);
                        rrj_write32(player + 812, z);
                    }
                }
                rrj_write32(player + 548, rrj_read32(player + 548) & 0xFFE1FFFFu);
            }
        }
    }
finish:
    if (stop)
    {
        rrj_write32(actor + 464, 0);
        rrj_write32(actor + 460, 0);
        rrj_write32(actor + 456, 0);
        rrj_write32(actor + 480, 0);
        if (rider)
        {
            w_u16(rider + 622, rrj_u16(normal));
            w_u16(rider + 624, rrj_u16((const uint8_t *)normal + 2));
            w_u16(rider + 626, rrj_u16((const uint8_t *)normal + 4));
        }
    }
    if (accepted && rider)
    {
        uint32_t state = rrj_read32(rider + 552);
        uint32_t body = rrj_read32(rider + 596);
        uint32_t tag;
        uint32_t session;
        uint32_t player;
        int32_t players;

        rrj_write32(rider + 552, state & 0xFFEFFFE7u);
        w_u8(rrj_read32(body + 1084) + 15, 0);
        tag = r_u16(rider + 172);
        session = rrj_read32(0x8005B2F8);
        rrj_write32(rider + 488, 0);
        players = rrj_s32(rrj_read32(session + 48));
        player = (tag >> 5) == 1 && rrj_s32(tag & 31u) < players;
        if ((player || (rrj_read32(rider + 572) & 0x60u) == 0x20u) && !rrj_read32(0x8005B220))
        {
            int32_t magnitude = impact_speed < original_speed ? original_speed : impact_speed;

            (void)sub_800B658C(rrj_read32(rider + 596), rider, magnitude, 1464860, 2);
        }
    }
    return accepted;
}

uint32_t sub_800B2844(uint32_t actor, uint32_t candidate)
{
    uint16_t normal[3];
    uint32_t translation[3];
    uint32_t actor_face;
    uint32_t candidate_face;

    FUNCTION_MARKER(0x800B2844, "RASHCDG.BIN");
    if (sub_800A8C78(actor, 0x140000, 0x1C0000))
    {
        uint32_t face;
        uint32_t depth;
        uint32_t vertex = sub_800B7030(actor + 196, candidate + 432,
            candidate + 196, 16, &face, &depth);

        if (vertex == 8)
            return 8;
        (void)sub_800B675C(candidate + 196, candidate + 432, face, normal, NULL);
        depth += 0x2000u;
        (void)sub_8002EE50(depth, normal, translation);
        (void)sub_800A8DF0(actor, translation, 1);
        actor_face = vertex | 0x100u;
        candidate_face = face | 0x200u;
    }
    else
    {
        uint32_t movable = sub_800AA140(actor, candidate, &actor_face,
            &candidate_face, translation);

        if (movable)
            (void)sub_800A8DF0(movable, translation, 1);
        normal[0] = (uint16_t)(0u - r_u16(candidate + 438));
        normal[1] = (uint16_t)(0u - r_u16(candidate + 440));
        normal[2] = (uint16_t)(0u - r_u16(candidate + 442));
    }
    if (actor_face)
    {
        int32_t actor_axis = rrj_s32(rrj_read32(actor + 296));
        int32_t candidate_axis = rrj_s32(rrj_read32(candidate + 296));
        uint32_t alignment = (uint32_t)sub_8001FC90(actor_axis, candidate_axis);
        uint32_t candidate_speed;
        uint32_t projected_speed;
        uint32_t difference;
        uint32_t actor_position;
        int32_t half;
        int32_t direction;
        uint32_t separation;
        int32_t relative_speed;

        actor_axis = rrj_s32(rrj_read32(actor + 300));
        candidate_axis = rrj_s32(rrj_read32(candidate + 300));
        alignment += (uint32_t)sub_8001FC90(actor_axis, candidate_axis);
        candidate_speed = rrj_read32(candidate + 480);
        projected_speed = (uint32_t)sub_8001FC90(rrj_s32(alignment), rrj_s32(candidate_speed));
        difference = rrj_read32(candidate + 184);
        actor_position = rrj_read32(actor + 184);
        difference -= actor_position;
        half = rrj_s32(difference + (difference >> 31)) >> 1;
        direction = (int16_t)r_u16(actor + 450);
        separation = (uint32_t)((int64_t)direction * half);
        difference = rrj_read32(candidate + 192);
        actor_position = rrj_read32(actor + 192);
        difference -= actor_position;
        half = rrj_s32(difference + (difference >> 31)) >> 1;
        direction = (int16_t)r_u16(actor + 454);
        relative_speed = rrj_s32(rrj_read32(actor + 480) - projected_speed);
        separation += (uint32_t)((int64_t)direction * half);
        if ((rrj_s32(separation) > 0 && relative_speed <= 0) ||
            (rrj_s32(separation) <= 0 && relative_speed >= 0))
            candidate_face = 0;
        if (actor_face && sub_800A9868(actor, normal,
            rrj_s32(rrj_read32(candidate + 480)), candidate + 450, candidate + 172, 0))
        {
            if (rrj_s32(rrj_read32(actor + 480)) > 65918 ||
                rrj_s32(rrj_read32(candidate + 480)) > 65918)
            {
                int32_t x = rrj_s32(rrj_read32(actor + 184));
                int32_t z = rrj_s32(rrj_read32(actor + 192));

                (void)sub_80017BA0(x, z, 19, 0);
            }
        }
    }
    if ((uint8_t)candidate_face == 3)
    {
        uint32_t state = r_u8(candidate + 509);

        if (!(state & 0x10u))
            w_u8(candidate + 509, (uint8_t)(state | 0x80u));
        state = r_u8(candidate + 509) | 0x10u;
        w_u8(candidate + 509, (uint8_t)state);
        return state;
    }
    return 3;
}

uint32_t sub_800A966C(uint32_t actor, const uint16_t normal[3], int32_t minimum)
{
    int32_t dot;
    uint32_t magnitude;
    uint32_t near;
    uint32_t backwards;
    uint32_t zero_vertical = 0;
    int32_t contact;

    FUNCTION_MARKER(0x800A966C, "RASHCDG.BIN");
    dot = rrj_s32(sub_8002E698(rrj_at(actor + 432, 6), normal));
    magnitude = dot < 0 ? 0u - (uint32_t)dot : (uint32_t)dot;
    near = rrj_s32(magnitude) <= 45874;
    backwards = sub_8002E698(rrj_at(actor + 450, 6), rrj_at(actor + 444, 6)) >> 31;
    if (near)
        zero_vertical = (r_u16(actor + 172) >> 5) == 1;
    contact = rrj_s32(sub_80084564(normal, actor + 450, actor + 480,
        actor + 456, 5, minimum, zero_vertical));
    if (contact < 0)
        return 0;
    if (contact)
    {
        uint32_t x = r_u16(actor + 450);
        uint32_t y = r_u16(actor + 452);
        uint32_t z = r_u16(actor + 454);

        w_u16(actor + 444, (uint16_t)x);
        w_u16(actor + 446, (uint16_t)y);
        w_u16(actor + 448, (uint16_t)z);
        if (backwards)
        {
            w_u16(actor + 444, (uint16_t)(0u - x));
            w_u16(actor + 446, (uint16_t)(0u - y));
            w_u16(actor + 448, (uint16_t)(0u - z));
        }
        if (zero_vertical)
        {
            x = r_u16(actor + 444);
            z = r_u16(actor + 448);
            w_u16(actor + 444, (uint16_t)(0u - x));
            y = r_u16(actor + 446);
            w_u16(actor + 448, (uint16_t)(0u - z));
            w_u16(actor + 446, (uint16_t)(0u - y));
        }
        z = r_u16(actor + 448);
        x = r_u16(actor + 444);
        w_u16(actor + 434, 0);
        w_u16(actor + 432, (uint16_t)z);
        w_u16(actor + 436, (uint16_t)(0u - x));
        if (!sub_8002E468(actor + 432))
        {
            w_u16(actor + 432, 4096);
            w_u16(actor + 436, 0);
        }
        x = (uint32_t)(int32_t)(int16_t)r_u16(actor + 444);
        y = (uint32_t)(int32_t)(int16_t)r_u16(actor + 446);
        xport_gte_write_control(0u, x);
        z = (uint32_t)(int32_t)(int16_t)r_u16(actor + 448);
        xport_gte_write_control(2u, y);
        xport_gte_write_control(4u, z);
        x = (uint32_t)(int32_t)(int16_t)r_u16(actor + 432);
        y = (uint32_t)(int32_t)(int16_t)r_u16(actor + 434);
        z = (uint32_t)(int32_t)(int16_t)r_u16(actor + 436);
        xport_gte_write_data(9u, x);
        xport_gte_write_data(10u, y);
        xport_gte_write_data(11u, z);
        xport_gte_execute(0x178000Cu);
        x = xport_gte_read_data(9u);
        y = xport_gte_read_data(10u);
        z = xport_gte_read_data(11u);
        w_u16(actor + 438, (uint16_t)x);
        w_u16(actor + 440, (uint16_t)y);
        w_u16(actor + 442, (uint16_t)z);
    }
    return ((uint32_t)contact & near) ? 2u : 1u;
}

uint32_t sub_800B675C(uint32_t box, uint32_t basis, uint32_t face,
    uint16_t normal[3], uint32_t corner[3])
{
    uint8_t axes[6];
    uint32_t table_word;
    uint32_t result;
    uint32_t origin;
    uint32_t axis;
    int negate;
    FUNCTION_MARKER(0x800B675C, "RASHCDG.BIN");
    table_word = rrj_read32(0x8005B970);
    axes[0] = (uint8_t)table_word;
    axes[1] = (uint8_t)(table_word >> 8);
    axes[2] = (uint8_t)(table_word >> 16);
    axes[3] = (uint8_t)(table_word >> 24);
    axes[4] = r_u8(0x8005B974);
    axes[5] = r_u8(0x8005B975);
    basis += 6u * (uint32_t)(int32_t)(int8_t)axes[face];
    negate = face < 2 || face == 5;
    for (axis = 0; axis < 3; ++axis)
    {
        result = r_u16(basis + 2u * axis);
        if (negate)
            result = 0u - result;
        normal[axis] = (uint16_t)result;
    }
    origin = box + (negate ? 48u : 24u);
    if (corner)
    {
        for (axis = 0; axis < 3; ++axis)
        {
            result = rrj_read32(origin + 4u * axis);
            corner[axis] = result;
        }
    }
    return result;
}

uint32_t sub_800B7030(uint32_t points, uint32_t basis, uint32_t box,
    uint32_t excluded_faces, uint32_t *face, uint32_t *depth)
{
    int32_t minimum = 0x7FFF0000;
    uint32_t vertex;
    FUNCTION_MARKER(0x800B7030, "RASHCDG.BIN");
    *face = 6;
    for (vertex = 0; vertex < 8; ++vertex)
    {
        uint32_t plane;
        for (plane = 0; plane < 6; ++plane)
        {
            int32_t signed_axis = rrj_s32(0x002EDF31u << (28u - 4u * plane)) >> 28;
            uint32_t axis = (uint32_t)(signed_axis < 0 ? -signed_axis : signed_axis) - 1u;
            uint32_t origin_index = (0x00606600u >> (4u * plane)) & 15u;
            uint32_t distance = sub_800B6AAC(rrj_at(points + 12u * vertex, 12), rrj_at(basis + 6u * axis, 6), rrj_at(box + 12u * origin_index, 12));
            if (signed_axis < 0)
                distance = 0u - distance;
            if (rrj_s32(distance) < 0)
                break;
            if (!((excluded_faces >> plane) & 1u) && rrj_s32(distance) < minimum)
            {
                *face = plane;
                minimum = rrj_s32(distance);
            }
        }
        if (plane == 6)
        {
            if (depth)
                *depth = (uint32_t)minimum;
            return vertex;
        }
    }
    return 8;
}

uint32_t sub_800AA34C(uint32_t points, uint32_t basis, uint32_t bounds[4])
{
    uint32_t order = 0x7520;
    uint32_t vertex;
    FUNCTION_MARKER(0x800AA34C, "RASHCDG.BIN");
    bounds[1] = 0x3FFF0000;
    bounds[0] = 0x3FFF0000;
    bounds[3] = 0xC0010000;
    bounds[2] = 0xC0010000;
    for (vertex = 0; vertex < 4; ++vertex)
    {
        uint32_t point = points + 12u * (order & 3u);
        uint32_t first = sub_800B6AAC(rrj_at(point, 12), rrj_at(basis + 2u, 6), rrj_at(basis + 20u, 12));
        uint32_t second = sub_800B6AAC(rrj_at(point, 12), rrj_at(basis + 14u, 6), rrj_at(basis + 20u, 12));
        uint32_t minimum = bounds[0];
        uint32_t maximum = bounds[2];
        if (rrj_s32(first) < rrj_s32(minimum))
            minimum = first;
        bounds[0] = minimum;
        if (rrj_s32(maximum) < rrj_s32(first))
            maximum = first;
        minimum = bounds[1];
        bounds[2] = maximum;
        if (rrj_s32(second) < rrj_s32(minimum))
            minimum = second;
        maximum = bounds[3];
        bounds[1] = minimum;
        if (rrj_s32(maximum) < rrj_s32(second))
            maximum = second;
        bounds[3] = maximum;
        order >>= 4;
    }
    return 0;
}

uint32_t sub_800AA140(uint32_t actor, uint32_t other, uint32_t *actor_face,
    uint32_t *other_face, uint32_t translation[3])
{
    uint32_t basis;
    uint32_t actor_bounds[4];
    uint32_t other_bounds[4];
    uint32_t first_near;
    uint32_t first_far;
    uint32_t second_near;
    uint32_t second_far;
    uint32_t first_depth;
    uint32_t second_depth;
    uint32_t penetration;
    uint32_t face;
    uint32_t normal;
    uint32_t angle;
    uint32_t quadrant;
    int first_negative;
    int second_negative;
    FUNCTION_MARKER(0x800AA140, "RASHCDG.BIN");
    basis = rrj_read32(actor + 340);
    (void)sub_800AA34C(actor + 196, basis, actor_bounds);
    (void)sub_800AA34C(other + 196, basis, other_bounds);
    first_near = other_bounds[2] - actor_bounds[0];
    first_far = actor_bounds[2] - other_bounds[0];
    if (rrj_s32(first_near) < 0 || rrj_s32(first_far) < 0)
    {
        *other_face = 0;
        *actor_face = 0;
        return 0;
    }
    second_near = other_bounds[3] - actor_bounds[1];
    second_far = actor_bounds[3] - other_bounds[1];
    if (rrj_s32(second_near) < 0 || rrj_s32(second_far) < 0)
    {
        *other_face = 0;
        *actor_face = 0;
        return 0;
    }
    first_negative = rrj_s32(first_near) < rrj_s32(first_far);
    second_negative = rrj_s32(second_near) < rrj_s32(second_far);
    first_depth = first_negative ? first_near : first_far;
    second_depth = second_negative ? second_near : second_far;
    if (rrj_s32(first_depth) < rrj_s32(second_depth))
    {
        penetration = first_negative ? 0u - first_depth : first_depth;
        face = 2u * (uint32_t)first_negative;
        normal = rrj_read32(actor + 340) + 2;
    }
    else
    {
        penetration = second_negative ? 0u - second_depth : second_depth;
        face = second_negative ? 3u : 1u;
        normal = rrj_read32(actor + 340) + 14;
    }
    (void)sub_8002EE50(penetration, rrj_at(normal, 6), translation);
    angle = sub_80020018((uint32_t)((int32_t)(int16_t)r_u16(basis + 14) * 16),
        (uint32_t)((int32_t)(int16_t)r_u16(basis + 18) * 16));
    quadrant = (((rrj_read32(other + 292) - angle + 0x2000u) & 0xFFFu) + 0x300u) >> 10;
    *other_face = ((face + quadrant) & 3u) | 0x200u;
    quadrant = (((rrj_read32(actor + 292) - angle + 0x2000u) & 0xFFFu) + 0x300u) >> 10;
    *actor_face = (((face ^ 2u) + quadrant) & 3u) | 0x200u;
    return other;
}

static uint32_t contact_abs32(uint32_t value)
{
    uint32_t sign = (uint32_t)(rrj_s32(value) >> 31);
    return (value + sign) ^ sign;
}

static uint32_t contact_mul16(uint32_t left, uint32_t right)
{
    int64_t product = (int64_t)rrj_s32(left) * rrj_s32(right);
    return (uint32_t)((uint64_t)product >> 16);
}

uint32_t sub_800ABE78(uint32_t actor, uint32_t other, uint32_t *linked,
    uint32_t *angle, uint32_t *lateral, uint32_t *longitudinal)
{
    uint32_t difference, state, value, limit, half, magnitude, mask, width;

    FUNCTION_MARKER(0x800ABE78u, "RASHCDG.BIN");
    difference = rrj_read32(actor + 188);
    difference -= rrj_read32(other + 188);
    state = rrj_s32(contact_abs32(difference)) < rrj_s32(rrj_read32(actor + 312)) ? 4u : 0u;
    if (state)
    {
        value = rrj_read32(actor + 292);
        value = (value - rrj_read32(other + 292) + 0x1000u) & 0xFFFu;
        difference = value + ((0x1000u - (value << 1)) & (uint32_t)(rrj_s32(0x800u - value) >> 31));
        if (rrj_s32(difference) < 0x180)
            state += 8;
        rrj_put32(angle, value);
    }
    if (state == 12)
    {
        value = sub_800B6AAC(rrj_at(other + 504, 12), rrj_at(actor + 516, 6), rrj_at(actor + 504, 12));
        rrj_put32(longitudinal, value);
        mask = rrj_read32(actor + 856) && rrj_s32(value) > 0;
        mask |= (rrj_read32(other + 856) ? value >> 31 : 0u) << 1;
        rrj_put32(linked, mask);
        limit = rrj_read32(actor + 308) << 1;
        width = rrj_read32(actor + 304);
        limit += width << 2;
        half = (uint32_t)(rrj_s32(width + (width >> 31)) >> 1);
        magnitude = contact_abs32(rrj_u32(longitudinal));
        if (mask & 1u)
        {
            value = rrj_read32(rrj_read32(actor + 856) + 304);
            limit += value << 1;
            half += value + width;
        }
        else if (mask & 2u)
        {
            width = rrj_read32(rrj_read32(other + 856) + 304);
            value = rrj_read32(other + 304);
            limit += width << 1;
            half += value + width;
        }
        if (rrj_s32(magnitude) >= rrj_s32(limit))
            return UINT32_MAX;
        if (rrj_s32(half) < rrj_s32(magnitude))
            state = 13;
    }
    if (state == 13)
    {
        value = sub_800B6AAC(rrj_at(other + 504, 12), rrj_at(actor + 528, 6), rrj_at(actor + 504, 12));
        rrj_put32(lateral, value);
        mask = rrj_u32(linked);
        if (mask & 3u)
        {
            uint32_t partner = rrj_read32((mask & 1u ? actor : other) + 856);
            width = rrj_read32(other + 308);
            width += rrj_read32(partner + 308);
            value = (width << 3) - width;
            if (rrj_s32(value) < 0)
                value += 7;
            limit = (uint32_t)(rrj_s32(value) >> 3);
        }
        else
        {
            width = rrj_read32(actor + 308);
            width += rrj_read32(other + 308);
            value = (width << 3) + width;
            if (rrj_s32(value) < 0)
                value += 15;
            limit = (uint32_t)(rrj_s32(value) >> 4);
        }
        if (rrj_s32(contact_abs32(rrj_u32(lateral))) < rrj_s32(limit))
            state = 15;
    }
    if (state == 15)
    {
        difference = rrj_read32(actor + 480);
        difference -= rrj_read32(other + 480);
        if (rrj_s32(contact_abs32(difference)) <= 0xEFFFF)
            state = 31;
    }
    return state;
}

uint32_t sub_800B5B48(uint32_t actor, uint32_t other, uint16_t output[3])
{
    uint32_t ratio, scaled_speed, vector[3], axis, root, denominator, reciprocal, result;

    FUNCTION_MARKER(0x800B5B48u, "RASHCDG.BIN");
    ratio = rrj_read32(other + 316);
    denominator = rrj_read32(actor + 316);
    ratio = sub_80010028(ratio, denominator);
    scaled_speed = contact_mul16(rrj_read32(other + 480), ratio);
    for (axis = 0; axis < 3; ++axis)
    {
        uint32_t first_direction = (uint32_t)((int32_t)(int16_t)r_u16(actor + 450 + 2u * axis) * 16);
        uint32_t first_product = contact_mul16(rrj_read32(actor + 480), first_direction);
        uint32_t second_direction = (uint32_t)((int32_t)(int16_t)r_u16(other + 450 + 2u * axis) * 16);
        uint32_t second_product = contact_mul16(scaled_speed, second_direction);
        vector[axis] = first_product + second_product;
    }
    while (rrj_s32(contact_abs32(vector[0])) > 0x5A8000 ||
           rrj_s32(contact_abs32(vector[1])) > 0x5A8000 ||
           rrj_s32(contact_abs32(vector[2])) > 0x5A8000)
    {
        vector[0] = (uint32_t)(rrj_s32(vector[0]) >> 1);
        vector[1] = (uint32_t)(rrj_s32(vector[1]) >> 1);
        vector[2] = (uint32_t)(rrj_s32(vector[2]) >> 1);
    }
    root = contact_mul16(vector[0], vector[0]) + contact_mul16(vector[1], vector[1]);
    root += contact_mul16(vector[2], vector[2]);
    root = sub_8004CF74(root);
    result = root << 2;
    denominator = (uint32_t)(rrj_s32(result) >> 1) + (uint32_t)(rrj_s32(result - 2u) >> 31);
    reciprocal = denominator ? 0x80000000u / denominator : UINT32_MAX;
    for (axis = 0; axis < 3; ++axis)
    {
        result = (uint32_t)(rrj_s32(contact_mul16(reciprocal, vector[axis])) >> 4);
        rrj_put16((uint8_t *)output + 2u * axis, (uint16_t)result);
    }
    return result;
}

static uint32_t contact_pair_index(uint32_t actor, uint32_t other, uint32_t count)
{
    uint32_t index = 0;
    if (rrj_s32(count) > 0)
    {
        uint32_t identity = r_u16(actor + 172);
        do
        {
            uint32_t pair = 0x800CCE48u + 36u * index;
            uint32_t first = r_u16(pair);
            if (first == identity)
            {
                uint32_t second = r_u16(pair + 2);
                uint32_t other_identity = r_u16(other + 172);
                if (second == other_identity)
                    break;
            }
            if (first == r_u16(other + 172))
            {
                if (r_u16(pair + 2) == identity)
                    break;
            }
            ++index;
        } while (rrj_s32(index) < rrj_s32(count));
    }
    return index;
}

static void contact_direction(uint32_t actor)
{
    uint32_t x = (uint32_t)(int32_t)(int16_t)r_u16(actor + 450) << 4;
    uint32_t z = (uint32_t)(int32_t)(int16_t)r_u16(actor + 454) << 4;
    uint32_t angle = sub_80020018(x, z);
    uint32_t cosine;
    uint32_t sine_address;
    rrj_write32(actor + 292, angle);
    cosine = (uint32_t)(int32_t)(int16_t)r_u16(0x8005624Cu + (((angle & 0xFFFu) << 2) | 2u)) << 4;
    sine_address = 0x8005624Cu + ((rrj_read32(actor + 292) & 0xFFFu) << 2);
    rrj_write32(actor + 296, cosine);
    rrj_write32(actor + 300, (uint32_t)(int32_t)(int16_t)r_u16(sine_address) << 4);
}

uint32_t sub_800AB7A0(uint32_t actor, uint32_t other, uint32_t propagated)
{
    uint32_t linked = 0;
    uint32_t angle = 0;
    uint32_t lateral = 0;
    uint32_t longitudinal = 0;
    uint32_t actor_face;
    uint32_t other_face;
    uint32_t translation[3];
    uint32_t power = 0;
    uint32_t selected;
    uint32_t partner;
    uint32_t selected_face;
    uint32_t partner_face;
    uint32_t count;
    uint32_t index;
    uint32_t kind;
    uint32_t direction_flags;

    FUNCTION_MARKER(0x800AB7A0, "RASHCDG.BIN");
    direction_flags = sub_800A8FE8(actor);
    direction_flags |= sub_800A8FE8(other) << 1;
    if (sub_800A8C78(actor, 0x140000, 0x1C0000))
    {
        uint32_t session;
        uint32_t identity;
        (void)w_u16(other + 320, (uint16_t)(r_u16(other + 320) | 12u));
        session = rrj_read32(0x8005B2F8);
        identity = r_u16(other + 172);
        if (identity < rrj_read32(session + 48))
        {
            uint32_t linked_player = (identity == 1u) & ((rrj_read32(0x800CCF78) >> 2) & 1u);
            uint32_t player = rrj_read32(0x8005B21C);
            uint32_t encoded = identity - (linked_player - 1u);
            uint32_t actor_identity = r_u16(actor + 172);
            uint32_t address = 0x800CCF78u + 4u * (actor_identity >> 4);
            uint32_t flags = rrj_read32(address);
            uint32_t player_bit = other == player && !linked_player;
            rrj_write32(address, flags | (encoded << (2u * (actor_identity & 15u))));
            flags = rrj_read32(0x800CCF78);
            rrj_write32(0x800CCF78, flags | (player_bit << 3));
        }
        identity = r_u16(actor + 172);
        count = rrj_read32(0x800CCF70u + 4u * (identity >> 4));
        count = (count >> (2u * (identity & 15u))) & 3u;
        index = rrj_read32(0x800CCF80);
        if (count)
            index |= 1u << (identity & 31u);
        rrj_write32(0x800CCF80, index);
        identity = r_u16(other + 172);
        count = rrj_read32(0x800CCF70u + 4u * (identity >> 4));
        count = (count >> (2u * (identity & 15u))) & 3u;
        index = rrj_read32(0x800CCF80);
        if (count)
            index |= 1u << (identity & 31u);
        rrj_write32(0x800CCF80, index);
        if (!propagated)
        {
            count = rrj_read32(0x800CCF68);
            if (rrj_s32(count) > 0 && contact_pair_index(actor, other, count) < count)
                return 0;
        }
        index = sub_800ABE78(actor, other, &linked, &angle, &lateral, &longitudinal);
        kind = (index == 31u) + 1u;
        if (index == UINT32_MAX)
            return 0;
    }
    else
    {
        if (direction_flags & 1u)
            contact_direction(actor);
        if (direction_flags & 2u)
            contact_direction(other);
        kind = 0;
    }
    if (kind == 2u)
        selected = sub_800AA474(actor, other, linked, angle, lateral, longitudinal,
            &actor_face, &other_face, translation);
    else if (kind == 1u)
    {
        selected = sub_800AAD30(other, actor, &other_face, &actor_face, translation, &power);
        if (rrj_read32(actor + 856) && !selected)
            selected = sub_800AAD30(actor, other, &actor_face, &other_face, translation, &power);
    }
    else
        selected = sub_800AA140(other, actor, &other_face, &actor_face, translation);
    if (!actor_face && !other_face)
        return 0;
    if (!rrj_read32(selected + 1088))
        selected = rrj_read32(selected + 856);
    if (selected == actor)
    {
        partner = other;
        partner_face = other_face;
        selected_face = actor_face;
    }
    else
    {
        partner = actor;
        selected_face = other_face;
        partner_face = actor_face;
    }
    if (!propagated)
        (void)sub_800A8DF0(selected, translation, 1);
    if (!kind)
    {
        (void)sub_800AC130(selected, partner, selected_face, partner_face);
        return 0;
    }
    if (rrj_s32(power) > 0)
    {
        uint16_t normal[3];
        uint32_t delta[3];
        uint32_t scale;
        uint32_t maximum;
        (void)sub_800B5B48(partner, selected, normal);
        scale = (uint32_t)sub_8001FC90(rrj_s32(rrj_read32(partner + 576)), rrj_s32(power));
        (void)sub_8002EE50(scale, normal, delta);
        (void)sub_800A8DF0(actor, delta, 1);
        (void)sub_800A8DF0(other, delta, 1);
        maximum = rrj_read32(actor + 552);
        rrj_write32(actor + 552, power < maximum ? maximum : power);
        maximum = rrj_read32(other + 552);
        rrj_write32(other + 552, power < maximum ? maximum : power);
    }
    count = rrj_read32(0x800CCF68);
    index = contact_pair_index(selected, partner, count);
    if (index >= 8u)
    {
        (void)sub_800AC130(selected, partner, selected_face, partner_face);
        return 1;
    }
    if (propagated && rrj_s32(index) < rrj_s32(rrj_read32(0x800CCF68)))
        return 1;
    {
        uint32_t pair = 0x800CCE48u + 36u * index;
        uint32_t flags;
        uint32_t identity;
        uint32_t partner_identity;
        (void)w_u16(pair, (uint16_t)r_u16(selected + 172));
        rrj_write32(pair + 16, translation[0]);
        rrj_write32(pair + 20, translation[1]);
        count = rrj_read32(0x800CCF68);
        rrj_write32(0x800CCF68, count + 1u);
        rrj_write32(pair + 24, translation[2]);
        partner_identity = r_u16(partner + 172);
        rrj_write32(pair + 28, selected_face | (partner_face << 16));
        flags = rrj_read32(0x800CCF80);
        (void)w_u16(pair + 2, (uint16_t)partner_identity);
        identity = r_u16(selected + 172);
        if (flags & (1u << (identity & 31u)))
            (void)sub_800A7AB4(selected, translation, r_u16(partner + 172));
    }
    return 1;
}

uint32_t sub_800A7AB4(uint32_t actor, const uint32_t delta[3], uint32_t excluded)
{
    FUNCTION_MARKER(0x800A7AB4, "RASHCDG.BIN");
    return sub_800A7ABC(actor, delta, excluded, rrj_read32(0x8005B1F8));
}

uint32_t sub_800A7ABC(uint32_t actor, const uint32_t delta[3], uint32_t excluded, uint32_t initial_count)
{
    uint32_t index = 0;
    FUNCTION_MARKER(0x800A7ABC, "RASHCDG.BIN");
    if (rrj_s32(initial_count) <= 0)
        return initial_count;
    do
    {
        uint32_t identity = r_u16(actor + 172);
        if (index != identity && index != (excluded & 0xFFFFu))
        {
            uint32_t flags = rrj_read32(0x800CCF70u + 4u * (index >> 4));
            uint32_t actor_flags;
            identity = r_u16(actor + 172);
            flags >>= 2u * (index & 15u);
            actor_flags = rrj_read32(0x800CCF70u + 4u * (identity >> 4));
            actor_flags = (actor_flags >> (2u * (identity & 15u))) & 3u;
            if (flags & actor_flags)
            {
                uint32_t candidate = rrj_read32(0x8005B3A0) + 1096u * index;
                uint32_t result;
                if (index < identity)
                    result = sub_800AB7A0(actor, candidate, 1);
                else
                    result = sub_800AB7A0(candidate, actor, 1);
                if (result)
                    (void)sub_800A8DF0(candidate, delta, 1);
            }
        }
        index = (index + 1u) & 0xFFFFu;
    } while (rrj_s32(index) < rrj_s32(rrj_read32(0x8005B1F8)));
    return 0;
}

uint32_t sub_800AC56C(uint32_t alignment, uint32_t first_side, uint32_t other_mode)
{
    FUNCTION_MARKER(0x800AC56C, "RASHCDG.BIN");
    if (rrj_s32(alignment) > 56754)
        return other_mode < 4u ? other_mode ^ 2u : 4u;
    if (rrj_s32(alignment) < -56754)
        return other_mode;
    return first_side & 2u ? 3u : 1u;
}

static void contact_player_feedback(uint32_t actor, uint32_t difference)
{
    uint32_t session = rrj_read32(0x8005B2F8);
    uint32_t identity = r_u16(actor + 172);
    uint32_t flags;
    uint32_t value;
    if (identity >= rrj_read32(session + 48))
        return;
    if (rrj_read32(rrj_read32(actor + 852) + 604) >= 2u)
        return;
    if (rrj_read32(0x8005B220))
        return;
    flags = rrj_read32(actor + 568);
    value = rrj_read32(actor + 480);
    if (flags & 0x20Du)
        value = difference;
    (void)sub_800B658C(actor, 0, rrj_s32(value), 0x165A1C, 1);
}

uint32_t sub_800AC130(uint32_t actor, uint32_t other, uint32_t actor_face, uint32_t other_face)
{
    uint32_t actor_side = 8;
    uint32_t other_side = 8;
    uint32_t actor_mode = 6;
    uint32_t other_mode = 6;
    uint32_t high_speed;
    uint32_t alignment;
    uint32_t projected;
    uint32_t difference;
    uint32_t first;
    uint32_t second;
    uint32_t response;
    uint16_t normal[3];

    FUNCTION_MARKER(0x800AC130, "RASHCDG.BIN");
    if (!(rrj_read32(actor + 568) & 1u) && !(rrj_read32(other + 568) & 1u))
    {
        if (rrj_read32(actor + 828) == other + 172u)
            return actor + 172u;
        if (rrj_read32(other + 828) == actor + 172u)
            return actor + 172u;
    }
    high_speed = rrj_s32(rrj_read32(actor + 480)) > 65918;
    if (!high_speed)
        high_speed = rrj_s32(rrj_read32(other + 480)) > 65918;
    if (!r_u16(rrj_read32(actor + 852) + 544) || (rrj_read32(actor + 568) & 14u)
        || !r_u16(rrj_read32(other + 852) + 544) || (rrj_read32(other + 568) & 14u))
        return other_face & 0x200u;
    if (other_face & 0x200u)
    {
        if (actor_face & 0x200u)
            actor_mode = actor_face & 255u;
        else
            actor_side = actor_face & 255u;
        other_mode = other_face & 255u;
        (void)sub_800B675C(other + 196, other + 432, other_mode, normal, NULL);
    }
    else
    {
        actor_mode = actor_face & 255u;
        (void)sub_800B675C(actor + 196, actor + 432, actor_mode, normal, NULL);
        other_side = other_face & 255u;
        first = rrj_u16((uint8_t *)normal);
        second = rrj_u16((uint8_t *)normal + 4);
        rrj_put16((uint8_t *)normal, (uint16_t)(0u - first));
        first = rrj_u16((uint8_t *)normal + 2);
        rrj_put16((uint8_t *)normal + 4, (uint16_t)(0u - second));
        rrj_put16((uint8_t *)normal + 2, (uint16_t)(0u - first));
    }
    if (!sub_80081D7C(actor, other, actor_face, other_face, normal))
        return 0;
    first = rrj_read32(actor + 296);
    second = rrj_read32(other + 296);
    alignment = (uint32_t)sub_8001FC90(rrj_s32(first), rrj_s32(second));
    first = rrj_read32(actor + 300);
    second = rrj_read32(other + 300);
    alignment += (uint32_t)sub_8001FC90(rrj_s32(first), rrj_s32(second));
    projected = (uint32_t)sub_8001FC90(rrj_s32(alignment), rrj_s32(rrj_read32(other + 480)));
    difference = contact_abs32(rrj_read32(actor + 480) - projected);
    if (actor_mode == 6u)
        actor_mode = sub_800AC56C(alignment, actor_side, other_mode);
    first = rrj_read32(other + 316);
    second = rrj_read32(actor + 316);
    (void)sub_800A9408(actor, rrj_s32(difference), rrj_s32(first), rrj_s32(second), rrj_s32(actor_mode));
    if (high_speed)
        contact_player_feedback(actor, difference);
    projected = (uint32_t)sub_8001FC90(rrj_s32(alignment), rrj_s32(rrj_read32(actor + 480)));
    difference = contact_abs32(rrj_read32(other + 480) - projected);
    if (other_mode == 6u)
        other_mode = sub_800AC56C(alignment, other_side, actor_mode);
    first = rrj_read32(actor + 316);
    second = rrj_read32(other + 316);
    response = sub_800A9408(other, rrj_s32(difference), rrj_s32(first), rrj_s32(second), rrj_s32(other_mode));
    if (high_speed)
    {
        uint32_t sound;
        if (rrj_s32(response) >= 3)
            sound = 48;
        else if (rrj_s32(response) >= 2)
            sound = 18;
        else
            sound = ((5u * (sub_80043F00(0xF2000002) & 255u)) >> 8) + 50u;
        first = rrj_read32(other + 184);
        second = rrj_read32(other + 192);
        (void)sub_80017BA0(rrj_s32(first), rrj_s32(second), sound, 0);
        contact_player_feedback(other, difference);
    }
    (void)sub_800A8BE0((uint16_t)r_u16(other + 172), actor + 912);
    return sub_800A8BE0((uint16_t)r_u16(actor + 172), other + 912);
}

uint32_t sub_800B71AC(uint32_t points, uint32_t direction, int32_t distance, uint32_t box,
    uint32_t basis, uint32_t excluded_faces, uint32_t *face, uint32_t *depth)
{
    uint8_t vertices[8];
    uint8_t faces[8];
    uint32_t distances[8];
    uint16_t reverse[3];
    uint32_t count = 0;
    uint32_t vertex;
    uint32_t index;
    uint32_t best = 0;

    FUNCTION_MARKER(0x800B71AC, "RASHCDG.BIN");
    for (vertex = 0; vertex < 8; ++vertex)
    {
        uint32_t side;
        for (side = 0; side < 6; ++side)
        {
            if (!(excluded_faces & (1u << side)))
            {
                int32_t axis = rrj_s32(0x2EDF31u << (28u - 4u * side)) >> 28;
                uint32_t normal = basis + 6u * (contact_abs32((uint32_t)axis) - 1u);
                uint32_t corner = box + 12u * ((0x606600u >> (4u * side)) & 15u);
                uint32_t value = sub_800B6AAC(rrj_at(points + 12u * vertex, 12), rrj_at(normal, 6), rrj_at(corner, 12));
                if (axis < 0)
                    value = 0u - value;
                if (rrj_s32(value) < 0)
                    break;
            }
        }
        if (side == 6)
            vertices[count++] = (uint8_t)vertex;
    }
    if (!count)
        return 8;
    for (index = 0; index < 3; ++index)
    {
        uint32_t value = (uint32_t)(int32_t)(int16_t)r_u16(direction + 2u * index);
        if (distance >= 0)
            value = 0u - value;
        rrj_put16((uint8_t *)reverse + 2u * index, (uint16_t)value);
    }
    for (index = 0; index < 8; ++index)
        faces[index] = 6;
    for (index = 0; index < count; ++index)
    {
        uint32_t side;
        distances[index] = 0x100000;
        for (side = 0; side < 6; ++side)
        {
            if (!(excluded_faces & (1u << side)))
            {
                int32_t axis = rrj_s32(0x2EDF31u << (28u - 4u * side)) >> 28;
                uint32_t normal = basis + 6u * (contact_abs32((uint32_t)axis) - 1u);
                uint32_t corner = box + 12u * ((0x606600u >> (4u * side)) & 15u);
                uint32_t value = sub_800B6BD0(points + 12u * vertices[index], reverse, rrj_at(normal, 6), corner);
                if (rrj_s32(value) > -2048)
                {
                    if (rrj_s32(value) <= 2048)
                        value = 2048;
                    if (rrj_s32(value) < rrj_s32(distances[index]))
                    {
                        distances[index] = value;
                        faces[index] = (uint8_t)side;
                    }
                }
            }
        }
    }
    for (index = 1; index < count; ++index)
        if (rrj_s32(distances[best]) < rrj_s32(distances[index]))
            best = index;
    *face = faces[best];
    *depth = distances[best];
    return vertices[best];
}

uint32_t sub_800B7810(uint32_t points, uint32_t basis, uint32_t face, uint32_t output[12], uint16_t planes[12])
{
    static const uint8_t vertices[6][4] = {
        {3, 0, 4, 7}, {0, 1, 5, 4}, {1, 2, 6, 5},
        {2, 3, 7, 6}, {1, 0, 3, 2}, {4, 5, 6, 7}
    };
    uint32_t first_axis;
    uint32_t second_axis;
    uint32_t negative_first;
    uint32_t negative_second;
    uint32_t index;
    uint32_t first_y;
    uint32_t first_x;
    uint32_t first_z;
    uint32_t second_x;
    uint32_t second_y;
    uint32_t second_z;
    uint32_t last;

    FUNCTION_MARKER(0x800B7810, "RASHCDG.BIN");
    if (face >= 6u)
        return 0x80060000u;
    for (index = 0; index < 12; ++index)
    {
        uint32_t address = points + 12u * vertices[face][index / 3u] + 4u * (index % 3u);
        rrj_put32((uint8_t *)output + 4u * index, rrj_read32(address));
    }
    first_axis = face < 4u ? 1u : 2u;
    second_axis = face == 0u || face == 2u ? 2u : 0u;
    for (index = 0; index < 3; ++index)
        rrj_put16((uint8_t *)planes + 2u * index, (uint16_t)r_u16(basis + 6u * first_axis + 2u * index));
    rrj_put16((uint8_t *)planes + 12, (uint16_t)r_u16(basis + 6u * first_axis));
    rrj_put16((uint8_t *)planes + 14, (uint16_t)r_u16(basis + 6u * first_axis + 2));
    first_y = rrj_u16((uint8_t *)planes + (face < 4u ? 2u : 14u));
    rrj_put16((uint8_t *)planes + 16, (uint16_t)r_u16(basis + 6u * first_axis + 4));
    for (index = 0; index < 3; ++index)
        rrj_put16((uint8_t *)planes + 6u + 2u * index, (uint16_t)r_u16(basis + 6u * second_axis + 2u * index));
    rrj_put16((uint8_t *)planes + 18, (uint16_t)r_u16(basis + 6u * second_axis));
    rrj_put16((uint8_t *)planes + 20, (uint16_t)r_u16(basis + 6u * second_axis + 2));
    last = r_u16(basis + 6u * second_axis + 4);
    negative_first = face < 4u ? 0u : 6u;
    negative_second = face == 0u || face == 3u || face == 4u ? 9u : 3u;
    first_x = rrj_u16((uint8_t *)planes + 2u * negative_first);
    rrj_put16((uint8_t *)planes + 2u * (negative_first + 1u), (uint16_t)(0u - first_y));
    second_x = rrj_u16((uint8_t *)planes + 2u * negative_second);
    rrj_put16((uint8_t *)planes + 2u * negative_first, (uint16_t)(0u - first_x));
    first_z = rrj_u16((uint8_t *)planes + 2u * (negative_first + 2u));
    rrj_put16((uint8_t *)planes + 2u * negative_second, (uint16_t)(0u - second_x));
    second_z = negative_second == 9u ? last : rrj_u16((uint8_t *)planes + 2u * (negative_second + 2u));
    rrj_put16((uint8_t *)planes + 22, (uint16_t)last);
    rrj_put16((uint8_t *)planes + 2u * (negative_first + 2u), (uint16_t)(0u - first_z));
    second_y = rrj_u16((uint8_t *)planes + 2u * (negative_second + 1u));
    rrj_put16((uint8_t *)planes + 2u * (negative_second + 2u), (uint16_t)(0u - second_z));
    rrj_put16((uint8_t *)planes + 2u * (negative_second + 1u), (uint16_t)(0u - second_y));
    return 0u - second_y;
}

uint32_t sub_800B6D70(const uint32_t point[3], const uint32_t vertices[], const uint16_t planes[], int32_t count, uint32_t tolerance)
{
    int32_t index;
    uint32_t limit = 0u - tolerance;
    FUNCTION_MARKER(0x800B6D70, "RASHCDG.BIN");
    for (index = 0; index < count; ++index)
    {
        uint32_t value = sub_800B6AAC(point, planes, vertices);
        if (rrj_s32(value) < rrj_s32(limit))
            return 0;
        vertices += 3;
        planes += 3;
    }
    return 1;
}

uint32_t sub_800B74F0(uint32_t points, uint32_t direction, int32_t distance, uint32_t box, uint32_t basis, uint32_t excluded_faces, uint32_t *face, uint32_t *depth)
{
    uint32_t vertices[6][12];
    uint16_t planes[6][12];
    uint16_t reverse[3];
    uint16_t outward[3];
    uint32_t projected[3];
    uint32_t current_face;
    uint32_t running_vertex = 8;
    uint32_t running_depth = 0;
    uint32_t saved_vertex = 8;
    uint32_t saved_face = 6;
    uint32_t saved_depth = 0;
    uint32_t magnitude = distance < 0 ? 0u - (uint32_t)distance : (uint32_t)distance;
    uint32_t axis;
    FUNCTION_MARKER(0x800B74F0, "RASHCDG.BIN");
    for (axis = 0; axis < 3; ++axis)
    {
        uint16_t value = rrj_u16(rrj_at(direction + 2u * axis, 2));
        rrj_put16((uint8_t *)reverse + 2u * axis,
            distance < 0 ? value : (uint16_t)(0u - (uint32_t)value));
    }
    for (current_face = 0; current_face < 6; ++current_face)
    {
        uint32_t corner;
        int32_t vertex;
        if ((excluded_faces >> current_face) & 1u)
            continue;
        (void)sub_800B7810(box, basis, current_face, vertices[current_face], planes[current_face]);
        (void)sub_800B675C(box, basis, current_face, outward, NULL);
        corner = box + 12u * ((0x606600u >> (4u * current_face)) & 15u);
        rrj_put16((uint8_t *)outward, (uint16_t)(0u - (uint32_t)rrj_u16(outward)));
        rrj_put16((uint8_t *)outward + 4, (uint16_t)(0u - (uint32_t)rrj_u16((uint8_t *)outward + 4)));
        rrj_put16((uint8_t *)outward + 2, (uint16_t)(0u - (uint32_t)rrj_u16((uint8_t *)outward + 2)));
        for (vertex = 7; vertex >= 0; --vertex)
        {
            uint32_t position = points + 12u * (uint32_t)vertex;
            uint32_t contact_depth;
            if (rrj_s32(sub_800B6AAC(rrj_at(position, 12), outward, rrj_at(corner, 12))) < 0)
                continue;
            contact_depth = sub_800B6BD0(position, reverse, outward, corner);
            if (rrj_s32(contact_depth) < -2048 || rrj_s32(magnitude + 2048u) < rrj_s32(contact_depth))
                continue;
            (void)sub_8002EAD8(rrj_at(position, 12), reverse, 0u - contact_depth, projected);
            if (!sub_800B6D70(projected, vertices[current_face], planes[current_face], 4, 2048))
                continue;
            if (rrj_s32(contact_depth) <= 2048)
                contact_depth = 2048;
            if (rrj_s32(running_depth) < rrj_s32(contact_depth))
            {
                running_depth = contact_depth;
                running_vertex = (uint32_t)vertex;
            }
        }
        if (running_vertex < 8)
        {
            if (rrj_s32(sub_8002E698(outward, reverse)) < 4097)
            {
                rrj_put32(face, current_face);
                rrj_put32(depth, running_depth);
                return running_vertex;
            }
            saved_face = current_face;
            saved_vertex = running_vertex;
            saved_depth = running_depth;
        }
    }
    rrj_put32(face, saved_face);
    rrj_put32(depth, saved_depth);
    return saved_vertex;
}

uint32_t sub_800AAD30(uint32_t actor, uint32_t other, uint32_t *actor_face,
    uint32_t *other_face, uint32_t translation[3], uint32_t *power)
{
    uint32_t groups[4];
    uint16_t normal[3];
    uint32_t saved_translation[3];
    uint32_t relative_actor;
    uint32_t relative_other;
    uint32_t actor_distance;
    uint32_t other_distance;
    uint32_t step_time;
    uint32_t selected = 0;
    uint32_t selected_vertex = 8;
    uint32_t selected_face = 6;
    uint32_t saved_vertex = 8;
    uint32_t saved_face = 6;
    uint32_t saved_actor = 0;
    uint32_t saved_other = 0;
    uint32_t saved_depth = 0;
    uint32_t depth = 0;
    uint32_t pass_count;
    uint32_t step = 0;
    uint32_t kind;
    uint32_t dot;
    uint32_t product;
    uint32_t first_speed;
    uint32_t second_speed;
    uint32_t speed;
    uint32_t time;
    int32_t steps;
    uint32_t axis;
    int searching = 1;

    FUNCTION_MARKER(0x800AAD30, "RASHCDG.BIN");
    rrj_put32(other_face, 0);
    rrj_put32(actor_face, 0);
    first_speed = rrj_read32(actor + 188);
    second_speed = rrj_read32(other + 188);
    if (rrj_s32(contact_abs32(first_speed - second_speed)) > 0x140000)
        return 0;
    groups[0] = actor;
    groups[1] = other;
    groups[2] = rrj_read32(actor + 856);
    groups[3] = other;
    pass_count = 2u << (rrj_read32(actor + 856) != 0 && rrj_read32(actor + 1088) != 0);
    dot = sub_8002E698(rrj_at(actor + 450, 6), rrj_at(other + 450, 6));
    product = (uint32_t)sub_8001FC90(rrj_s32(dot), rrj_s32(rrj_read32(other + 480)));
    first_speed = rrj_read32(actor + 480);
    second_speed = rrj_read32(other + 480);
    relative_actor = first_speed - product;
    speed = relative_actor;
    if ((rrj_s32(second_speed) < rrj_s32(first_speed) && rrj_s32(relative_actor) < rrj_s32(first_speed)) ||
        (rrj_s32(second_speed) >= rrj_s32(first_speed) && rrj_s32(relative_actor) < rrj_s32(second_speed)))
    {
        first_speed = rrj_read32(actor + 480);
        second_speed = rrj_read32(other + 480);
        speed = rrj_s32(second_speed) < rrj_s32(first_speed) ? first_speed : second_speed;
    }
    time = rrj_read32(0x800CCE38);
    product = (uint32_t)sub_8001FC90(rrj_s32(time), rrj_s32(speed));
    steps = rrj_s32((uint32_t)(rrj_s32(product + 0x8000u) >> 16) * 3u) / 4;
    if (steps < 1)
        steps = 1;
    if (steps > 4)
        steps = 4;
    step_time = rrj_read32(0x800CCE38);
    if (steps >= 2)
    {
        time = step_time;
        step_time = (uint32_t)(rrj_s32(time) / steps);
        first_speed = rrj_read32(actor + 480);
        product = (uint32_t)sub_8001FC90(rrj_s32(first_speed), rrj_s32(step_time - time));
        (void)sub_8002EE50(product, rrj_at(actor + 450, 6), translation);
        (void)sub_800A8DF0(actor, translation, 0);
        time = rrj_read32(0x800CCE38);
        second_speed = rrj_read32(other + 480);
        product = (uint32_t)sub_8001FC90(rrj_s32(second_speed), rrj_s32(step_time - time));
        (void)sub_8002EE50(product, rrj_at(other + 450, 6), translation);
        (void)sub_800A8DF0(other, translation, 0);
    }
    actor_distance = (uint32_t)sub_8001FC90(rrj_s32(step_time), rrj_s32(relative_actor));
    dot = sub_8002E698(rrj_at(other + 450, 6), rrj_at(actor + 450, 6));
    first_speed = rrj_read32(actor + 480);
    product = (uint32_t)sub_8001FC90(rrj_s32(dot), rrj_s32(first_speed));
    relative_other = rrj_read32(other + 480) - product;
    other_distance = (uint32_t)sub_8001FC90(rrj_s32(step_time), rrj_s32(relative_other));
    for (;;)
    {
        uint32_t pass = 0;
        do
        {
            uint32_t candidate = groups[(pass + 3u) & 3u];
            uint32_t distance = (pass & 1u) ? other_distance : actor_distance;
            uint32_t relative = (pass & 1u) ? relative_other : relative_actor;
            uint32_t excluded = 0;
            uint32_t width;
            uint32_t length;
            uint32_t threshold;
            uint32_t sweep;
            uint32_t allowance;
            uint32_t alignment;

            selected = groups[pass];
            kind = r_u16(candidate + 172) >> 5;
            if (kind == 3)
            {
                excluded = 0x10;
                if (!(r_u16(selected + 172) >> 5) && !(rrj_read32(selected + 568) & 0x600u))
                    excluded = 0x30;
            }
            length = rrj_read32(candidate + 308);
            width = rrj_read32(candidate + 304);
            threshold = (kind == 0 || kind == 3) ?
                (rrj_s32(width) < rrj_s32(length) ? width : length) * 20u : 0;
            if (rrj_s32(contact_abs32(relative)) < rrj_s32(threshold))
            {
                uint32_t target = rrj_read32(candidate + 468);
                uint32_t source = rrj_read32(selected + 468);
                uint32_t factor = rrj_read32(selected + 300);
                uint32_t first = (uint32_t)sub_8001FC90(rrj_s32(factor), rrj_s32(target - source));
                target = rrj_read32(candidate + 476);
                source = rrj_read32(selected + 476);
                factor = rrj_read32(selected + 296);
                sweep = first + (uint32_t)sub_8001FC90(rrj_s32(factor), rrj_s32(target - source));
                selected_vertex = sub_800B71AC(selected + 196, selected + 450, rrj_s32(sweep),
                    candidate + 196, candidate + 432, excluded, &selected_face, &depth);
            }
            else
            {
                sweep = distance;
                selected_vertex = sub_800B74F0(selected + 196, selected + 450, rrj_s32(sweep),
                    candidate + 196, candidate + 432, excluded, &selected_face, &depth);
            }
            if (selected_vertex < 8)
            {
                searching = 0;
                (void)sub_800B675C(candidate + 196, candidate + 432, selected_face, normal, NULL);
                alignment = sub_8002E698(normal, rrj_at(selected + 450, 6));
                if (rrj_s32(sweep) < 0)
                    alignment = 0u - alignment;
                allowance = rrj_read32(selected + 308);
                if (rrj_s32(allowance) >= rrj_s32(distance))
                {
                    uint32_t numerator = distance;
                    uint32_t denominator = allowance;
                    int negate = 0;
                    if (rrj_s32(distance) <= 0)
                    {
                        numerator = 0u - distance;
                        if (rrj_s32(allowance) <= 0)
                            denominator = 0u - allowance;
                        else
                            negate = 1;
                    }
                    else if (rrj_s32(allowance) <= 0)
                    {
                        denominator = 0u - allowance;
                        negate = 1;
                    }
                    allowance = sub_80010028(numerator, denominator);
                    if (negate)
                        allowance = 0u - allowance;
                }
                if (rrj_s32(alignment) < -3275)
                {
                    depth += 8192u + allowance;
                    (void)sub_8002EE50(depth, normal, translation);
                }
                else
                {
                    depth += allowance;
                    (void)sub_8002ECB8(rrj_at(selected + 450, 6), normal, translation, 0u - depth, 8192);
                }
                kind = r_u16(other + 172) >> 5;
                if ((kind == 0 || kind == 3) &&
                    (((selected_vertex & 2u) && selected_face == 3) ||
                     (!(selected_vertex & 2u) && selected_face == 1)))
                {
                    dot = sub_8002E698(rrj_at(actor + 450, 6), rrj_at(other + 450, 6));
                    if (rrj_s32(dot) > 0)
                    {
                        depth = sub_800B6AAC(rrj_at(selected + 468, 12),
                            rrj_at(candidate + 432, 6), rrj_at(candidate + 468, 12));
                        if (rrj_s32(depth) < 0)
                        {
                            selected_face = 0;
                            selected_vertex = (selected_vertex & 2u) ? 2u : 1u;
                            width = rrj_read32(candidate + 304);
                            length = rrj_read32(selected + 304);
                            product = 0u - width - length;
                        }
                        else
                        {
                            selected_face = 2;
                            selected_vertex = (selected_vertex & 2u) ? 3u : 0u;
                            width = rrj_read32(candidate + 304);
                            length = rrj_read32(selected + 304);
                            product = width + length;
                        }
                        (void)sub_8002EE50(product, rrj_at(candidate + 432, 6), translation);
                        width = rrj_read32(candidate + 304);
                        length = rrj_read32(selected + 304);
                        depth = width + length;
                    }
                }
                if (pass + 2u < pass_count &&
                    ((candidate == actor && selected_face == 2) ||
                     (selected == actor && ((selected_vertex & 1u) != ((selected_vertex & 2u) >> 1)))))
                {
                    saved_vertex = selected_vertex;
                    saved_actor = selected;
                    saved_face = selected_face;
                    saved_other = candidate;
                    for (axis = 0; axis < 3; ++axis)
                        saved_translation[axis] = rrj_u32((uint8_t *)translation + 4u * axis);
                    pass = 1;
                    searching = 1;
                    saved_depth = depth;
                }
                else if (saved_vertex < 8)
                {
                    if (candidate != saved_other)
                    {
                        saved_translation[0] = 0u - saved_translation[0];
                        saved_translation[2] = 0u - saved_translation[2];
                        saved_translation[1] = 0u - saved_translation[1];
                    }
                    if (candidate != saved_other || saved_face != selected_face)
                    {
                        for (axis = 0; axis < 3; ++axis)
                        {
                            uint32_t value = rrj_u32((uint8_t *)translation + 4u * axis);
                            rrj_put32((uint8_t *)translation + 4u * axis, value + saved_translation[axis]);
                        }
                    }
                    else if (rrj_s32(depth) < rrj_s32(saved_depth))
                    {
                        for (axis = 0; axis < 3; ++axis)
                            rrj_put32((uint8_t *)translation + 4u * axis, saved_translation[axis]);
                    }
                }
            }
            if (!searching)
                break;
            ++pass;
        } while (pass < pass_count);
        if (saved_vertex < 8)
            searching = 0;
        if (!searching)
            break;
        selected = 0;
        if (step >= (uint32_t)(steps - 1))
            break;
        product = (uint32_t)sub_8001FC90(rrj_s32(rrj_read32(actor + 480)), rrj_s32(step_time));
        (void)sub_8002EE50(product, rrj_at(actor + 450, 6), translation);
        (void)sub_800A8DF0(actor, translation, 0);
        product = (uint32_t)sub_8001FC90(rrj_s32(rrj_read32(other + 480)), rrj_s32(step_time));
        (void)sub_8002EE50(product, rrj_at(other + 450, 6), translation);
        (void)sub_800A8DF0(other, translation, 0);
        kind = r_u16(other + 172) >> 5;
        ++step;
        if (!kind && saved_vertex == 8)
        {
            uint32_t linked = 0;
            uint32_t angle = 0;
            uint32_t lateral = 0;
            uint32_t longitudinal = 0;
            uint32_t result = sub_800ABE78(actor, other, &linked, &angle, &lateral, &longitudinal);
            if (rrj_s32(result) >= 15)
                selected = sub_800AA474(actor, other, linked, angle, lateral, longitudinal,
                    actor_face, other_face, translation);
            searching = 1;
            if (selected)
                goto remaining_power;
        }
    }
    if (saved_vertex < 8 && selected_vertex == 8)
    {
        selected_face = saved_face;
        for (axis = 0; axis < 3; ++axis)
            rrj_put32((uint8_t *)translation + 4u * axis, saved_translation[axis]);
        selected_vertex = saved_vertex;
        selected = saved_actor;
    }
    if (selected_vertex >= 8)
        return selected;
    if (selected == other)
    {
        rrj_put32(other_face, selected_vertex | 0x100u);
        rrj_put32(actor_face, selected_face | 0x200u);
    }
    else
    {
        rrj_put32(actor_face, selected_vertex | 0x100u);
        rrj_put32(other_face, selected_face | 0x200u);
    }
remaining_power:
    if (power)
    {
        uint32_t remaining = 0;
        if (step < (uint32_t)(steps - 1))
            remaining = rrj_read32(0x800CCE38) - (step + 1u) * step_time;
        rrj_put32(power, remaining);
    }
    return selected;
}
