#include "rrj_debug.h"
#include "game_main.h"
#include "rrj_lockstep.h"
#include "vblank.h"
#include "native_game_api.h"
#include "music_stop.h"
#include "race_pause_frontier.h"
#include "race_leaf_batch_006.h"
#include "race_bodyless_batch_006.h"
#include "fixed_math.h"
#include "flare.h"
#include "gpu.h"
#include "menu_item.h"
#include "packet.h"
#include "psx_gpu.h"
#include "xport.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void frontier_copy_words(uint32_t output, uint32_t input, uint32_t bytes);
static uint32_t frontier_asr(uint32_t value, uint32_t shift);

static uint32_t frontier_prepare_object_indices(const uint32_t *indices, int32_t count, uint32_t player_index)
{
    int32_t index;

    w_u16(0x1F80000Au, 2048);
    w_u16(0x1F80000Cu, 4096);
    w_u16(0x1F800012u, 512);
    w_u16(0x1F800014u, 768);
    w_u8(0x1F800018u, (uint32_t)-3);
    w_u8(0x1F800019u, (uint32_t)-2);
    w_u16(0x1F800008u, 0);
    w_u16(0x1F800010u, 0);
    w_u8(0x1F80001Au, 0);
    w_u32(0x1F800004u, r_u32(0x8005B4D4u));
    w_u32(0x1F800000u, r_u32(0x8005B4D8u));
    w_u32(0x1F80001Cu, r_u32(0x8005ADFCu) - 2);
    w_u32(0x1F800208u, r_u32(0x8005ACB4u));
    frontier_copy_words(0x1F800020u, 0x800D50A8u, 128);
    for (index = 0; index < count; ++index)
    {
        uint32_t owner = r_u32(0x800D87F0u + 112 * indices[index]);

        (void)sub_80067690(owner, player_index);
    }
    for (index = 0; index < count; ++index)
    {
        uint32_t owner = r_u32(0x800D87F0u + 112 * indices[index]);

        (void)sub_80067770(owner, player_index);
    }
    return 0;
}

static void frontier_configure_model_scratch(void)
{
    w_u16(0x1F80000Au, 2048);
    w_u16(0x1F80000Cu, 4096);
    w_u16(0x1F800012u, 512);
    w_u16(0x1F800014u, 768);
    w_u8(0x1F800018u, (uint32_t)-3);
    w_u8(0x1F800019u, (uint32_t)-2);
    w_u16(0x1F800008u, 0);
    w_u16(0x1F800010u, 0);
    w_u8(0x1F80001Au, 0);
    w_u32(0x1F800004u, r_u32(0x8005B4D4u));
    w_u32(0x1F80001Cu, r_u32(0x8005ADFCu) - 2);
    w_u32(0x1F800000u, r_u32(0x8005B4D8u));
}

static void frontier_set_flare_matrix(int32_t angle, int32_t x_scale, int32_t y_scale)
{
    uint32_t packed = r_u32(0x8005624Cu + 4 * ((uint32_t)(angle < 0 ? -angle : angle) & 0xFFFu));
    int32_t sine = (int16_t)packed;
    int32_t cosine = (int16_t)(packed >> 16);
    MATRIX matrix;

    if (angle < 0)
        sine = -sine;
    memset(&matrix, 0, sizeof(matrix));
    matrix.m[0][0] = (int16_t)((int32_t)(uint32_t)(frontier_asr((uint32_t)(x_scale * cosine), 12)));
    matrix.m[0][1] = (int16_t)((int32_t)(uint32_t)(frontier_asr((uint32_t)(-x_scale * sine), 12)));
    matrix.m[1][0] = (int16_t)((int32_t)(uint32_t)(frontier_asr((uint32_t)(y_scale * sine), 12)));
    matrix.m[1][1] = (int16_t)((int32_t)(uint32_t)(frontier_asr((uint32_t)(y_scale * cosine), 12)));
    SetRotMatrix(&matrix);
}

static uint32_t frontier_asr(uint32_t value, uint32_t shift)
{
    if (!shift)
        return value;
    return (value >> shift) | ((0u - (value >> 31)) << (32 - shift));
}

static uint32_t frontier_project_translation(int32_t x, int32_t y, int32_t z)
{
    MATRIX matrix;
    SVECTOR vertex;
    sint32 screen;
    sint32 flags;

    memset(&matrix, 0, sizeof(matrix));
    memset(&vertex, 0, sizeof(vertex));
    matrix.t[0] = x;
    matrix.t[1] = y;
    matrix.t[2] = z;
    PushMatrix();
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);
    (void)gte_project(&vertex, &screen, &flags);
    PopMatrix();
    return (uint32_t)screen;
}

typedef struct FrontierVertex
{
    int32_t x;
    int32_t y;
    int32_t z;
    uint32_t reserved;
    uint32_t uv;
    int32_t shade;
    uint32_t screen;
    uint32_t color_clip;
} FrontierVertex;

static uint8_t frontier_classify_projected(int32_t z, uint32_t screen, int32_t far_depth)
{
    uint32_t clip;
    int32_t screen_y = ((int32_t)(uint32_t)(screen)) >> 16;
    uint32_t sign = (screen & 0x8000u) >> 15;

    if (z > far_depth)
        clip = 1u << 5;
    else if (z > 51199)
        clip = 1u << 6;
    else
        clip = 1u << 7;
    if (z < 10240)
        clip |= 0x10u;
    if (screen_y < 0)
        clip |= 8u;
    if (screen_y >= 241)
        clip |= 4u;
    if ((uint16_t)screen < 385u)
        clip |= sign;
    else
        clip |= sign + 1;
    return (uint8_t)clip;
}

static void frontier_transform_vertex(const MATRIX *matrix, uint32_t source, FrontierVertex *vertex)
{
    int16_t input[3];
    int32_t output[3];
    uint32_t row;

    input[0] = (int16_t)r_u16(source);
    input[1] = (int16_t)r_u16(source + 2);
    input[2] = (int16_t)r_u16(source + 4);
    for (row = 0; row != 3; ++row)
    {
        int64_t value = (int64_t)matrix->m[row][0] * input[0] + (int64_t)matrix->m[row][1] * input[1] + (int64_t)matrix->m[row][2] * input[2];
        output[row] = (int32_t)(value >> 12) + matrix->t[row];
    }
    vertex->x = ((int32_t)(uint32_t)((uint32_t)output[0] << 8));
    vertex->y = ((int32_t)(uint32_t)((uint32_t)output[1] << 8));
    vertex->z = ((int32_t)(uint32_t)((uint32_t)output[2] << 8));
    vertex->screen = frontier_project_translation(vertex->x >> 5, vertex->y >> 5, vertex->z >> 5);
}

static void frontier_project_scratch_quad(void)
{
    uint32_t index;

    for (index = 0; index != 4; ++index)
    {
        uint32_t record = 0x1F800100u + 32 * index;
        int32_t x = ((int32_t)(uint32_t)(r_u32(record)));
        int32_t y = ((int32_t)(uint32_t)(r_u32(record + 4)));
        int32_t z = ((int32_t)(uint32_t)(r_u32(record + 8)));
        uint32_t screen = frontier_project_translation(x >> 5, y >> 5, z >> 5);

        w_u32(record + 24, screen);
        w_u8(record + 31, frontier_classify_projected(z, screen, 102399));
    }
}

static void frontier_render_flat_quad(int32_t depth, uint32_t color)
{
    if (depth < 1600)
    {
        (void)sub_8006A25C(4, 0x01030200u, color);
    }
    else
    {
        uint32_t cursor = r_u32(0x1F800020u);
        uint32_t packet;

        if (cursor + 24 >= r_u32(0x8005B4D0u))
            cursor = sub_80021C98(cursor, 24);
        packet = cursor;
        w_u32(packet, r_u32(0x1F800028u) | 0x05000000u);
        w_u32(packet + 4, color | 0x28000000u);
        w_u32(packet + 8, r_u32(0x1F800118u));
        w_u32(packet + 12, r_u32(0x1F800138u));
        w_u32(packet + 16, r_u32(0x1F800158u));
        w_u32(packet + 20, r_u32(0x1F800178u));
        w_u32(0x1F800028u, packet);
        w_u32(0x1F800020u, packet + 24);
    }
}

static void frontier_render_wrapped_subdivision(uint32_t slot, uint32_t flags, int quad)
{
    uint32_t cursor = r_u32(0x1F800020u);
    uint32_t packet = cursor;

    w_u32(packet, (r_u32(slot) & 0xFFFFFFu) | 0x02000000u);
    w_u32(packet + 4, r_u32(0x1F8000B4u));
    w_u32(packet + 8, 0);
    w_u32(0x1F800028u, packet);
    if (packet + 12 >= r_u32(0x8005B4D0u))
        cursor = sub_80021C98(packet, 12);
    cursor += 12;
    w_u32(0x1F800020u, cursor);
    if (quad)
        (void)sub_80069784(4, 0x03020100u, 5u - ((flags >> 16) & 0x0Fu));
    else
        (void)sub_8006929C(3, 0x00020100u, 5u - ((flags >> 16) & 0x0Fu));
    packet = r_u32(0x1F800020u);
    w_u32(packet, (r_u32(0x1F800028u) & 0xFFFFFFu) | 0x02000000u);
    w_u32(packet + 4, r_u32(0x1F800078u + 4 * ((flags & 0xFFu) >> 4)));
    w_u32(packet + 8, 0);
    w_u32(0x1F800028u, packet);
    if (packet + 12 >= r_u32(0x8005B4D0u))
        cursor = sub_80021C98(packet, 12);
    else
        cursor = packet;
    cursor += 12;
    w_u32(0x1F800020u, cursor);
    w_u32(slot, r_u32(0x1F800028u));
}

static void frontier_build_midpoint(uint32_t output_index, uint32_t first_index, uint32_t second_index, uint32_t rounding_bias, int adjust)
{
    uint32_t first = 0x1F800100u + 32 * first_index;
    uint32_t second = 0x1F800100u + 32 * second_index;
    uint32_t output = 0x1F800100u + 32 * output_index;
    uint32_t first_color = r_u32(first + 16);
    uint32_t second_color = r_u32(second + 16);
    uint32_t depth;
    int32_t z;
    uint32_t screen;
    uint32_t clip;
    int32_t screen_y;

    w_u32(output + 16, (((first_color & 0xFFFFFEFFu) + (second_color & 0xFFFFFEFFu)) >> 1) + ((first_color & second_color) & 0x100u));
    depth = frontier_asr(r_u32(first + 20) + r_u32(second + 20), 1);
    w_u32(output + 20, depth);
    w_u32(output + 28, (r_u32(output + 28) & 0xFF000000u) | (r_u32(0x800D4CA8u + 4 * ((uint32_t)depth & 0xFFu)) & 0xFFFFFFu));
    w_u32(output, frontier_asr(r_u32(first) + r_u32(second), 1));
    w_u32(output + 4, frontier_asr(r_u32(first + 4) + r_u32(second + 4), 1));
    z = (int32_t)frontier_asr(r_u32(first + 8) + r_u32(second + 8), 1);
    w_u32(output + 8, (uint32_t)z);
    if (adjust)
    {
        int32_t scale = ((int32_t)(uint32_t)(frontier_asr((uint32_t)z + rounding_bias, 8)));
        uint32_t first_uv = r_u32(first + 24);
        uint32_t second_uv = r_u32(second + 24);
        int32_t delta_x = (int16_t)first_uv - (int16_t)second_uv;
        int32_t delta_y = (int16_t)(first_uv >> 16) - (int16_t)(second_uv >> 16);
        uint32_t sign_x = 0u - ((uint32_t)delta_x >> 31);
        uint32_t sign_y = 0u - ((uint32_t)delta_y >> 31);
        uint32_t abs_x = ((uint32_t)delta_x + sign_x) ^ sign_x;
        uint32_t abs_y = ((uint32_t)delta_y + sign_y) ^ sign_y;
        uint32_t y_mask = frontier_asr(abs_y - 2u * abs_x, 31);
        uint32_t x_mask = frontier_asr(abs_x - 2u * abs_y, 31);
        uint32_t y_adjust = (((2u * (uint32_t)scale) & sign_x) - (uint32_t)scale) & y_mask;
        uint32_t x_adjust = ((uint32_t)scale + ((0u - 2u * (uint32_t)scale) & sign_y)) & x_mask;
        uint32_t y = r_u32(output + 4);
        uint32_t x = r_u32(output);
        w_u32(output + 4, y + y_adjust);
        w_u32(output, x + x_adjust);
    }
    xport_gte_write_control(5, frontier_asr(r_u32(output), 5));
    xport_gte_write_control(6, frontier_asr(r_u32(output + 4), 5));
    xport_gte_write_control(7, frontier_asr(r_u32(output + 8), 5));
    xport_gte_execute(0x198001u);
    screen = xport_gte_read_data(14);
    w_u32(output + 24, screen);
    z = r_s32(output + 8);
    if (z > 102399)
        clip = 1u << 5;
    else if (z > 51199)
        clip = 1u << 6;
    else
        clip = 1u << 7;
    if (z < 10240)
        clip |= 0x10u;
    screen_y = ((int32_t)(uint32_t)(screen)) >> 16;
    if (screen_y < 0)
        clip |= 8u;
    if (screen_y >= 241)
        clip |= 4u;
    if ((uint16_t)screen < 385u)
        clip |= (screen >> 15) & 1u;
    else
        clip |= ((screen >> 15) & 1u) + 1u;
    w_u8(output + 31, (uint8_t)clip);
}

static int32_t frontier_depth_bucket(int32_t depth)
{
    int32_t bucket;
    int32_t origin = (int32_t)r_u32(0x1F800004);

    if (origin < 4096)
    {
        uint32_t selector = ((depth >> 12) > 0) + ((depth >> 11) > 0);
        int32_t exponent = (int32_t)(r_u32(0x1F800000) + (uint32_t)(int32_t)r_s8(0x1F800018 + selector));
        int32_t upper = (int32_t)(3 - (uint32_t)exponent);
        uint32_t table_index = (exponent >= 0 ? (uint32_t)exponent : 0) + (upper < 0 ? (uint32_t)upper : 0);
        uint32_t base = r_u16(0x1F800006 + 2 * table_index) + (uint32_t)origin;
        int32_t distance = (int32_t)((uint32_t)depth - base);

        bucket = (int32_t)(r_u16(0x1F80000E + 2 * table_index) + (uint32_t)(distance >> (exponent & 31)));
    }
    else
    {
        bucket = (int32_t)((uint32_t)depth - (uint32_t)origin) >> (r_u32(0x1F800000) & 31);
    }
    {
        int32_t upper = (int32_t)(r_u32(0x1F80001C) - (uint32_t)bucket);
        return (int32_t)((bucket >= 0 ? (uint32_t)bucket : 0) + (upper < 0 ? (uint32_t)upper : 0));
    }
}

static int32_t frontier_matrix_mul32(uint32_t matrix, const int32_t input[3], int32_t output[3])
{
    unsigned row;
    int32_t result = 0;

    for (row = 0; row != 3; ++row)
    {
        uint32_t first = (uint32_t)(int32_t)r_s16(matrix + 6 * row) * (uint32_t)input[0];
        uint32_t second = (uint32_t)(int32_t)r_s16(matrix + 6 * row + 2) * (uint32_t)input[1];
        uint32_t third = (uint32_t)(int32_t)r_s16(matrix + 6 * row + 4) * (uint32_t)input[2];

        result = (int32_t)(frontier_asr(first, 12) + frontier_asr(second, 12) + frontier_asr(third, 12));
        output[row] = result;
    }
    return result;
}

uint32_t sub_800106DC(uint32_t input, uint32_t output, uint32_t matrix)
{
    int32_t source[3];
    int32_t transformed[3];
    unsigned index;

    FUNCTION_MARKER(0x800106DC, "SLUS_010.53");
    for (index = 0; index != 3; ++index)
        source[index] = ((int32_t)(uint32_t)(r_u32(input + 4 * index)));
    (void)frontier_matrix_mul32(matrix, source, transformed);
    for (index = 0; index != 3; ++index)
        w_u32(output + 4 * index, (uint32_t)transformed[index]);
    return (uint32_t)transformed[2];
}

uint32_t sub_8001064C(void)
{
    PsxGteSnapshot state;
    uint32_t index;

    FUNCTION_MARKER(0x8001064C, "SLUS_010.53");
    psx_gte_snapshot(&state);
    for (index = 0; index != 4; ++index)
    {
        uint32_t component = 2 * index;
        uint32_t word = (uint16_t)state.rotation.m[component / 3][component % 3];
        ++component;
        word |= (uint32_t)(uint16_t)state.rotation.m[component / 3][component % 3] << 16;
        xport_gte_write_control(8 + index, word);
    }
    xport_gte_write_control(12, (uint32_t)(int32_t)state.rotation.m[2][2]);
    for (index = 0; index != 3; ++index)
        xport_gte_write_control(13 + index, (uint32_t)state.translation[index]);
    for (index = 0; index != 5; ++index)
        xport_gte_write_control(index, 0);
    for (index = 0; index != 3; ++index)
        xport_gte_write_data(9 + index, 0);
    return 0;
}

uint32_t sub_80023A14(void)
{
    uint32_t record = r_u32(0x8005AE34);
    uint32_t source = r_u32(record + 4);
    uint32_t blocked = r_u16(source + 362);
    int32_t direction;
    int32_t position;

    FUNCTION_MARKER(0x80023A14, "SLUS_010.53");
    if (blocked)
        return blocked;
    position = (int16_t)r_u16(source + 370);
    w_u32(record + 68, (uint32_t)position);
    w_u32(record + 72, r_u16(source + 360));
    direction = ((int32_t)(uint32_t)(r_u32(source + 364)));
    direction = direction > 0 ? 1 : direction < 0 ? -1 : 0;
    w_u32(record + 64, (uint32_t)direction);
    w_u32(record + 100, direction <= 0 ? r_u32(record + 68) - r_u32(record + 104) : r_u32(record + 104) - r_u32(record + 68));
    w_u32(record + 44, r_u32(record + 72) != r_u32(record + 80));
    w_u32(record + 48, r_u32(record + 88) != (uint32_t)direction);
    if (r_u32(record + 44) || r_u32(record + 48))
    {
        uint32_t route = (uint32_t)sub_800245DC(r_u32(record + 72));

        w_u32(record + 92, route);
        w_u32(record + 96, r_u32(route + 4) >> 6);
        w_u32(record + 52, r_u32(route + (direction <= 0 ? 8 : 12)));
    }
    if (r_u32(record + 60))
    {
        if (position >= 41 && position < ((int32_t)(uint32_t)(r_u32(record + 96))) - 40)
        {
            w_u32(record + 60, 0);
            w_u32(record + 36, 1);
        }
    }
    else if ((position < 41 || position >= ((int32_t)(uint32_t)(r_u32(record + 96))) - 40) && (r_u32(record + 44) || r_u32(record + 48) || (position - ((int32_t)(uint32_t)(r_u32(record + 84)))) * direction >= 0))
    {
        w_u32(record + 60, 1);
        w_u32(record + 40, 1);
    }
    return (uint32_t)sub_80023DB8(record + 80);
}

uint32_t sub_80023DB8(uint32_t output)
{
    uint32_t record = r_u32(0x8005AE34);
    uint32_t result;

    FUNCTION_MARKER(0x80023DB8, "SLUS_010.53");
    w_u32(output + 8, r_u32(record + 64));
    w_u32(output, r_u32(record + 72));
    result = r_u32(record + 68);
    w_u32(output + 4, result);
    return result;
}

uint32_t sub_80023FBC(void)
{
    uint32_t record = r_u32(0x8005AE34);
    uint32_t state;

    FUNCTION_MARKER(0x80023FBC, "SLUS_010.53");
    if (!r_u32(record + 44) && !r_u32(record + 48) && !r_u32(record + 40) && !r_u32(record + 36))
        return 0;
    record = r_u32(0x8005AE34u);
    state = r_u32(record + 56);
    if (state == 0)
    {
        if (r_u32(record + 48))
        {
            uint32_t route;

            (void)sub_80023DE4();
            record = r_u32(0x8005AE34u);
            uint32_t offset = ((int32_t)(uint32_t)(r_u32(record + 64))) <= 0 ? 8u : 4u;
            route = r_u32(0x8005B508u + 4u * r_u32(record));
            route = r_u32(route + offset);
            (void)(uint32_t)sub_800243BC(route);
        }
        else if (r_u32(record + 40))
            w_u32(record + 56, 1);
        else if (r_u32(record + 44))
        {
            w_u32(record + 56, 1);
            w_u32(record + 60, 1);
        }
    }
    else if (state == 1)
    {
        if (r_u32(record + 40))
        {
            w_u32(record + 56, 0);
            return sub_80023DE4();
        }
        if (r_u32(record + 36))
        {
            if (!(r_u32(record + 28) & 2))
            {
                w_u32(record + 60, state);
                return 0;
            }
            w_u32(record + 56, 0);
            (void)sub_80023DE4();
        }
    }
    record = r_u32(0x8005AE34u);
    w_u32(record + 36, 0);
    w_u32(record + 40, 0);
    w_u32(record + 48, 0);
    w_u32(record + 44, 0);
    return record;
}

uint32_t sub_80023CAC(void)
{
    uint32_t current = r_u32(0x8005B500);
    uint32_t state = 0x80053478 + (current << 7);
    uint32_t raw;
    uint32_t delta;
    uint32_t restored;
    uint32_t count;
    uint32_t result;

    FUNCTION_MARKER(0x80023CAC, "SLUS_010.53");
    if (r_u32(state + 108) == 0 && r_u32(0x80053478 + ((current ^ 1) << 7) + 108) != 0)
    {
        current ^= 1;
        w_u32(0x8005B500, current);
    }
    raw = r_u32(r_u32(0x8005ACBC) + 24);
    delta = 5 - raw;
    restored = 5 - delta;
    count = (delta & (0u - ((delta >> 31) ^ 1u))) + (restored & (0u - (restored >> 31)));
    /* Preserve the allocator index spill later copied into actor events */
    if (!(r_u32(0x80053480 + (current << 7) + 20) & 2))
        w_u16(0x801FFEDC, 0);
    (void)sub_80023148(0x80053480 + (current << 7), count, 0, current << 16);
    current = r_u32(0x8005B500);
    result = r_u32(0x80053480 + (current << 7) + 4);
    if (result)
        w_u32(0x8005B514, result);
    return result;
}

uint32_t sub_80030E58(uint32_t player_index)
{
    uint32_t base = r_u32(0x8005ACBC);
    uint32_t first = r_u32(base + 0xA40 + 8 * player_index);
    uint32_t last = r_u32(base + 0xA44 + 8 * player_index);
    uint32_t index;

    FUNCTION_MARKER(0x80030E58, "SLUS_010.53");
    w_u32(base + 32, 0);
    w_u32(base + 36, 0);
    w_u32(base + 40, 0);
    for (index = first; index <= last; ++index)
    {
        uint32_t record = base + 44 + 36 * index;

        if ((r_u32(record) & 0x91) == 0x11)
        {
            uint32_t payload = r_u32(record + 20);
            int32_t result = (int32_t)(uint32_t)sub_80023900(r_u32(payload) >> 28, payload + 4, player_index);

            if (result < 0)
                (void)(uint32_t)sub_800318E8(record, player_index);
            if (result < 0)
                w_u32(base + 32, r_u32(base + 32) + 1);
            else if (result > 0)
                w_u32(base + 40, r_u32(base + 40) + 1);
            else
                w_u32(base + 36, r_u32(base + 36) + 1);
        }
    }
    return (uint32_t)sub_80033198(player_index);
}

uint32_t sub_80033198(uint32_t player_index)
{
    uint32_t record = 0x800D9268 + 1152 * player_index;
    uint32_t index;

    FUNCTION_MARKER(0x80033198, "SLUS_010.53");
    for (index = 0; index != 24; ++index, record += 48)
    {
        uint32_t value = r_u32(record + 8);

        if (((int32_t)(uint32_t)(value)) >= 0 && ((int32_t)(uint32_t)((uint32_t)sub_80023900(value, record + 24, player_index))) < 0)
        {
            uint32_t id = ((value & 0x8000) << 16) | ((value & 0x7C00) << 13) | (value & 0x3FF);

            if (r_u32(record + 4) == 0)
                (void)(uint32_t)sub_8003297C(id, player_index);
            else
                (void)(uint32_t)sub_80032938(id, player_index);
        }
    }
    return 0;
}

uint32_t sub_80023900(uint32_t type, uint32_t payload, uint32_t player_index)
{
    uint32_t previous = r_u32(r_u32(0x8005AE34));
    uint32_t result;

    FUNCTION_MARKER(0x80023900, "SLUS_010.53");
    (void)sub_8002379C(player_index);
    result = (uint32_t)sub_800243EC(type, payload);
    (void)sub_8002379C(previous);
    return result;
}

uint32_t sub_800243EC(uint32_t type, uint32_t ranges)
{
    uint32_t record = r_u32(0x8005AE34);
    uint32_t index;
    int32_t route = ((int32_t)(uint32_t)(r_u32(record + 72)));
    int32_t position = ((int32_t)(uint32_t)(r_u32(record + 68)));

    FUNCTION_MARKER(0x800243EC, "SLUS_010.53");
    for (index = 0; index != 4; ++index, ranges += 6)
    {
        int32_t candidate = (int16_t)r_u16(ranges);

        if (candidate == -1)
            break;
        if (candidate == route)
        {
            int32_t first = (int16_t)r_u16(ranges + 2);
            int32_t last = (int16_t)r_u16(ranges + 4);

            if (position >= first && position <= last)
                return 0;
            if (((int32_t)(uint32_t)(r_u32(record + 64))) > 0)
                return position < first ? 1 : UINT32_MAX;
            return last < position ? 1 : UINT32_MAX;
        }
    }
    return UINT32_MAX;
}

uint32_t sub_800318E8(uint32_t record, uint32_t player_index)
{
    uint32_t payload = r_u32(record + 20);
    uint32_t word = r_u32(payload);
    uint32_t type = word >> 28;
    uint32_t id = word & 0x0FFFFFFF;
    uint32_t extra = r_u32(record + 16);
    uint32_t handled = 1;

    FUNCTION_MARKER(0x800318E8, "SLUS_010.53");
    if (r_u32(record) & 2)
    {
        if (type == 0 || type == 8 || type == 9)
            (void)(uint32_t)sub_80032810(id, type, player_index);
        else if (type == 1)
            (void)(uint32_t)sub_8003297C(id, player_index);
        else if (type == 2)
            (void)(uint32_t)sub_80032938(id, player_index);
        else if (type == 3)
            handled = (uint32_t)sub_8003D844(id & 0xFFFF, player_index);
        else if (type == 4)
            handled = (uint32_t)sub_80013828(extra);
    }
    if (handled == 1)
    {
        if (r_u32(record) & 0x10)
            (void)(uint32_t)sub_80023868(r_u32(payload + 28) >> 1);
        return (uint32_t)sub_800313EC(record);
    }
    return 1;
}

uint32_t sub_800312D0(uint32_t player_index)
{
    uint32_t base = r_u32(0x8005ACBC);
    uint32_t first = r_u32(base + 0xA40 + 8 * player_index);
    uint32_t last = r_u32(base + 0xA44 + 8 * player_index);
    uint32_t record = base + 44 + 36 * first;
    uint32_t skipped = 0;
    uint32_t count = 0;

    FUNCTION_MARKER(0x800312D0, "SLUS_010.53");
    while (first <= last)
    {
        if (r_u32(record) == 0)
        {
            if (r_u32(r_u32(0x8005B2F8) + 48) == 1 && skipped < 2)
                ++skipped;
            else
                ++count;
        }
        ++first;
        record += 36;
    }
    return count;
}

uint32_t sub_80018E54(uint32_t group)
{
    uint32_t enabled = r_u32(group + 8);
    uint32_t count;
    uint32_t index;

    FUNCTION_MARKER(0x80018E54, "SLUS_010.53");
    if (!enabled)
        return enabled;
    count = r_u32(group + 12);
    if (((int32_t)(uint32_t)(count)) <= 0)
        return count;
    for (index = 0; ((int32_t)(uint32_t)(index)) < ((int32_t)(uint32_t)(r_u32(group + 12))); ++index)
    {
        uint32_t entry = r_u32(group) + 2108 * index;
        uint32_t cursor;

        if (!(r_u32(entry + 36) & 2) || !(cursor = r_u32(entry + 1756)))
            continue;
        while (r_u32(entry + 16) >= r_u32(cursor))
        {
            uint32_t object = r_u32(entry);
            uint32_t source = object;
            uint32_t flags = r_u8(entry + 36) & 0x80;

            if ((r_u16(object + 172) >> 5) == 1 && r_u32(object + 604) < 3)
            {
                uint32_t descriptor;

                source = r_u32(object + 596);
                descriptor = r_u32(source + 1084);
                flags |= (r_u8(descriptor + 46) & 15) | ((r_u8(descriptor + 1) & 3) << 4);
                if (r_u16(source + 172) < r_u32(r_u32(0x8005B2F8) + 48))
                    flags |= 0x40;
            }
            cursor = (uint32_t)sub_80017DA0(r_u32(source + 184), r_u32(source + 192), cursor, flags);
            w_u32(entry + 1756, cursor);
            if (!cursor)
                break;
        }
    }
    return 0;
}

uint32_t sub_80043E24(void)
{
    FUNCTION_MARKER(0x80043E24u, "SLUS_010.53");
    return 0x801FFF98;
}

uint32_t sub_80043DC4(uint32_t stack)
{
    FUNCTION_MARKER(0x80043DC4u, "SLUS_010.53");
    /* Native execution uses the host stack */
    return 0;
}

uint32_t sub_800C89A0(void)
{
    uint32_t state = r_u32(0x8005B2F8);
    uint32_t current = r_u8(0x8005B58C) ^ 1;
    uint32_t index;
    uint32_t table;

    FUNCTION_MARKER(0x800C89B8, "RASHCDG.BIN");
    w_u32(0x8005B588, 0);
    w_u8(0x8005B58C, (uint8_t)current);
    for (index = 0; index < r_u32(state + 48); ++index)
    {
        rrj_gpu_clear_ot(0x800D9BE0 + 40 * current + 20 * index, 5);
        rrj_gpu_clear_ot(0x800D9C30 + 8 * current + 4 * index, 1);
    }
    w_u32(0x8005B588, 0);
    w_u32(0x8005B59C, 0x800D9BE0 + 40 * current);
    w_u32(0x8005B5A8, 0x800D9C30 + 8 * current);
    table = 0x800D9C50 + 48 * current;
    rrj_gpu_clear_ot(table, 12);
    w_u32(0x8005B5AC, table);
    w_u32(0x8005B5A0, table + 40);
    w_u32(0x8005B5A4, table + 36);
    w_u32(0x8005B590, table + 32);
    w_u32(0x8005B594, table + 28);
    w_u32(0x8005B598, table);
    return table;
}

uint32_t sub_8002F2E8(uint32_t argument)
{
    FUNCTION_MARKER(0x8002F2E8, "SLUS_010.53");
    return (uint32_t)sub_8002F17C(argument);
}

uint32_t sub_8002F17C(uint32_t player_index)
{
    uint32_t player = 0x800CD898 + 1132 * player_index;
    uint32_t record = r_u32(0x8005AEC0 + 4 * player_index);
    uint32_t index;
    uint32_t result;

    FUNCTION_MARKER(0x8002F17C, "SLUS_010.53");
    (void)sub_80070F9C(player + 432, record + 60);
    (void)TransposeMatrix((MATRIX *)xport_guest_ptr(record + 60, sizeof(MATRIX)), (MATRIX *)xport_guest_ptr(record + 92, sizeof(MATRIX)));
    for (index = 0; index != 3; ++index)
    {
        int32_t value = (int16_t)r_u16(record + 98 + 2 * index);
        uint32_t scaled = frontier_asr((uint32_t)value * 3413u, 12);

        w_u16(record + 98 + 2 * index, (uint16_t)scaled);
    }
    w_u32(record + 16, r_u32(player + 184));
    w_u32(record + 20, r_u32(player + 188));
    w_u32(record + 24, r_u32(player + 192));
    w_u32(record + 28, frontier_asr(r_u32(record + 16), 10));
    w_u32(record + 32, frontier_asr(r_u32(record + 20), 10));
    w_u32(record + 36, frontier_asr(r_u32(record + 24), 10));
    result = r_u16(player + 744);
    w_u16(record + 124, (uint16_t)result);
    return result;
}

uint32_t sub_80070F9C(uint32_t source, uint32_t output)
{
    uint32_t value;

    FUNCTION_MARKER(0x80070F9C, "RASHCDG.BIN");
    value = r_u16(source);
    w_u16(output, (uint16_t)value);
    value = r_u16(source + 2);
    w_u16(output + 6, (uint16_t)value);
    value = r_u16(source + 4);
    w_u16(output + 12, (uint16_t)value);
    value = r_u16(source + 6);
    w_u16(output + 2, (uint16_t)value);
    value = r_u16(source + 8);
    w_u16(output + 8, (uint16_t)value);
    value = r_u16(source + 10);
    w_u16(output + 14, (uint16_t)value);
    value = r_u16(source + 12);
    w_u16(output + 4, (uint16_t)value);
    value = r_u16(source + 14);
    w_u16(output + 10, (uint16_t)value);
    value = r_u16(source + 16);
    w_u16(output + 16, (uint16_t)value);
    return (uint32_t)(int32_t)(int16_t)value;
}

uint32_t sub_800674C8(void)
{
    FUNCTION_MARKER(0x800674C8, "RASHCDG.BIN");
    w_u32(0x8005B280, 0);
    return 0x80060000;
}

static void frontier_update_linked_actor(uint32_t root, uint32_t player_index)
{
    uint32_t child;
    uint32_t actor;

    if (!root)
        return;
    child = r_u32(root + 852);
    if (!(r_u8(child + 572) & 0x10))
        return;
    child = r_u32(root + 856);
    actor = r_u32(child + 852);
    if (r_u32(actor + 604) < 3)
        return;
    (void)sub_8008B99C(actor);
    (void)sub_80085224(actor, player_index);
}

uint32_t sub_8008D56C(uint32_t player_index)
{
    uint32_t table = 0x800CE4D0;
    uint32_t actor;
    int32_t remaining;
    uint32_t group;

    FUNCTION_MARKER(0x8008D56C, "RASHCDG.BIN");
    (void)(uint32_t)sub_8008B99C(0x800CD898 + 1132 * player_index);
    actor = r_u32(table);
    remaining = ((int32_t)(uint32_t)(r_u32(r_u32(table + 12))));
    while (remaining >= 0)
    {
        uint32_t linked;

        if ((int16_t)r_u16(actor + 320) != 0 && ((int32_t)(uint32_t)((uint32_t)sub_8008B99C(actor))) > 0)
            (void)(uint32_t)sub_80084E10(actor, player_index);
        linked = r_u32(actor + 852);
        if (r_u32(linked + 604) < 3)
            w_u32(linked + 176, r_u32(actor + 176));
        else if ((int16_t)r_u16(linked + 320) != 0 && ((int32_t)(uint32_t)((uint32_t)sub_8008B99C(linked))) > 0)
            (void)sub_80085224(linked, player_index);
        actor += r_u32(table + 4);
        --remaining;
    }
    frontier_update_linked_actor(r_u32(0x8005B38C), player_index);
    frontier_update_linked_actor(r_u32(0x8005B21C), player_index);
    for (group = 2; group != 6; ++group)
    {
        uint32_t entry = table + 16 * group;

        actor = r_u32(entry);
        remaining = ((int32_t)(uint32_t)(r_u32(r_u32(entry + 12))));
        while (remaining >= 0)
        {
            if (r_u16(actor + 172) != 0 && ((int32_t)(uint32_t)((uint32_t)sub_8008B99C(actor))) > 0)
            {
                uint32_t output = actor + 104;

                w_u32(actor + 12, frontier_asr(r_u32(actor + 184), 10));
                w_u32(actor + 16, frontier_asr(r_u32(actor + 188), 10));
                w_u32(actor + 20, frontier_asr(r_u32(actor + 192), 10));
                if (r_s8(actor + 72) != 3)
                    output = r_u32(actor + 4) + 4;
                (void)sub_80070F9C(actor + 432, output);
                (void)(uint32_t)sub_80067AC4(actor, player_index);
            }
            actor += r_u32(entry + 4);
            --remaining;
        }
    }
    return 0;
}

static int32_t frontier_clamp(int32_t value, int32_t minimum, int32_t maximum)
{
    if (value < minimum)
        return minimum;
    if (value > maximum)
        return maximum;
    return value;
}

uint32_t sub_80084E10(uint32_t actor, uint32_t player_index)
{
    uint32_t specification = r_u32(actor + 556);
    uint32_t output = actor + 104;

    FUNCTION_MARKER(0x80084E10, "RASHCDG.BIN");
    w_u32(actor + 12, frontier_asr(r_u32(actor + 184), 10));
    w_u32(actor + 16, frontier_asr(r_u32(actor + 188), 10));
    w_u32(actor + 20, frontier_asr(r_u32(actor + 192), 10));
    if (r_s8(actor + 72) != 3)
        output = r_u32(actor + 4) + 4;
    (void)sub_80070F9C(actor + 432, output);
    if (r_s8(r_u32(0x8005B2F8)) != 3 && r_s8(actor + 8) < 2)
    {
        int32_t target = 0;
        int32_t steering;
        int32_t roll = 0;
        uint32_t linked;

        (void)(uint32_t)sub_8001FD24(0, (uint32_t)(int32_t)r_s16(actor + 826), 0, r_u32(actor + 4) + 28);
        if (!(r_u32(actor + 568) & 0x600))
        {
            if (r_u32(actor + 612))
            {
                int32_t state = ((int32_t)(uint32_t)(r_u32(actor + 616)));
                int32_t angle = (int16_t)r_u16(actor + 846);

                if (state > 0 || angle > 0)
                    target = r_u16(specification + 438);
                else if (state < 0 || angle < 0)
                    target = r_u16(specification + 436);
            }
            else
            {
                int32_t lower = r_u16(specification + 436);
                int32_t upper = r_u16(specification + 438);
                int32_t phase = (int32_t)sub_8001FF3C((uint32_t)((int32_t)r_s16(actor + 530) * 16));
                int32_t curve = 2 * (phase - (int16_t)r_u16(actor + 842));
                int32_t desired;
                int32_t velocity = ((int32_t)(uint32_t)(r_u32(actor + 608)));

                curve = frontier_clamp(curve, -lower, upper);
                if (r_u32(actor + 564) & 0x20)
                    desired = upper;
                else if (r_u32(actor + 564) & 0x40)
                    desired = -lower;
                else if (velocity > 0)
                    desired = ((int32_t)((int32_t)sub_8001FC90(((int32_t)(velocity)), ((int32_t)(upper << 16))))) >> 16;
                else if (velocity < 0)
                    desired = ((int32_t)((int32_t)sub_8001FC90(((int32_t)(velocity)), ((int32_t)(lower << 16))))) >> 16;
                else
                    desired = 0;
                target = desired;
                if ((curve ^ desired) >= 0)
                    target = desired <= 0 ? (curve < desired ? curve : desired) : (desired >= curve ? desired : curve);
            }
        }
        steering = r_u16(actor + 844);
        steering += (((int32_t)((int32_t)sub_8001FC90(((int32_t)(245760)), ((int32_t)((target - (int16_t)r_u16(actor + 846)) << 16))))) - ((int32_t)((int32_t)sub_8001FC90(((int32_t)(22937)), ((int32_t)((int16_t)r_u16(actor + 844) << 16)))))) >> 16;
        w_u16(actor + 844, (uint16_t)steering);
        w_u16(actor + 846, (uint16_t)((int16_t)r_u16(actor + 846) + (int16_t)steering / 20));
        linked = r_u32(actor + 856);
        if (linked)
        {
            int32_t target_roll = 0;
            int32_t velocity;
            int32_t old_velocity;

            if (!(r_u32(actor + 568) & 0x700))
            {
                velocity = ((int32_t)(uint32_t)(r_u32(actor + 636)));
                if (velocity <= 0)
                    target_roll = -velocity / 3;
                else
                    target_roll = ((int32_t)(uint32_t)(r_u32(actor + 488))) / 4;
            }
            old_velocity = ((int32_t)(uint32_t)(r_u32(linked + 660)));
            velocity = old_velocity + ((int32_t)((int32_t)sub_8001FC90(((int32_t)(245760)), ((int32_t)(target_roll - ((int32_t)(uint32_t)(r_u32(linked + 656)))))))) - ((int32_t)((int32_t)sub_8001FC90(((int32_t)(0x4000)), ((int32_t)(old_velocity)))));
            w_u32(linked + 660, (uint32_t)velocity);
            roll = frontier_clamp(((int32_t)(uint32_t)(r_u32(linked + 656))) + velocity / 20, -13725, 13725);
            w_u32(linked + 656, (uint32_t)roll);
            roll = -(int32_t)frontier_asr((uint32_t)(652 * roll), 16);
        }
        (void)(uint32_t)sub_8001FD24((uint32_t)(int32_t)r_s16(actor + 846), 0, (uint32_t)roll, r_u32(actor + 4) + 52);
    }
    return (uint32_t)sub_80067AC4(actor, player_index);
}

static void frontier_copy_words(uint32_t output, uint32_t input, uint32_t bytes)
{
    uint32_t offset;

    for (offset = 0; offset != bytes; offset += 4)
        w_u32(output + offset, r_u32(input + offset));
}

static void frontier_rotate_values(const int16_t matrix[3][3], const int16_t input[3], int32_t output[3])
{
    uint32_t row;

    for (row = 0; row != 3; ++row)
    {
        int64_t value = 0;
        uint32_t column;

        for (column = 0; column != 3; ++column)
            value += matrix[row][column] * input[column];
        value >>= 12;
        if (value < -32768)
            value = -32768;
        else if (value > 32767)
            value = 32767;
        output[row] = (int32_t)value;
    }
}

static void frontier_rotate_vector(uint32_t matrix_address, const int16_t input[3], int32_t output[3])
{
    int16_t matrix[3][3];
    uint32_t row;
    uint32_t column;

    for (row = 0; row != 3; ++row)
        for (column = 0; column != 3; ++column)
            matrix[row][column] = (int16_t)r_u16(matrix_address + 2 * (3 * row + column));
    frontier_rotate_values(matrix, input, output);
}

static uint32_t frontier_clear_player_visibility(uint32_t actor, uint32_t player_index)
{
    uint8_t bit = (uint8_t)(1u << player_index);
    uint8_t flags = r_u8(actor + 9);

    flags &= (uint8_t)~bit;
    w_u8(actor + 9, flags);
    return flags & bit;
}

static void frontier_67AC4_rotate(uint32_t matrix, const int16_t input[3], int32_t output[3])
{
    uint32_t index;

    for (index = 0; index != 5; ++index)
        xport_gte_write_control(index, r_u32(matrix + 4 * index));
    xport_gte_write_data(0, (uint16_t)input[0] | ((uint32_t)(uint16_t)input[1] << 16));
    xport_gte_write_data(1, (uint32_t)(int32_t)input[2]);
    xport_gte_execute(0x486012);
    for (index = 0; index != 3; ++index)
        output[index] = ((int32_t)(uint32_t)(xport_gte_read_data(25 + index)));
}

uint32_t sub_80067AC4(uint32_t actor, uint32_t player_index)
{
    uint32_t player = r_u32(0x8005AEC0 + 4 * player_index);
    uint32_t camera = player + 92;
    uint32_t specification = r_u32(actor);
    uint32_t type = (r_u16(specification + 14) & 0x78) >> 3;
    uint32_t shift;
    uint8_t bit = (uint8_t)(1u << player_index);
    uint8_t flags;
    int16_t input[3];
    int32_t transformed[3];
    uint32_t index;

    FUNCTION_MARKER(0x80067AC4, "RASHCDG.BIN");
    if (!r_u32(actor + 52) && r_u32(actor + 176) == UINT32_MAX)
        return 0;
    w_u32(0x1F8002BCu, 0x1F8002E0u);
    if (r_s8(actor + 72) == 1)
    {
        flags = r_u8(actor + 9) | bit;
        w_u8(actor + 9, flags);
        shift = r_u16(specification + 14) >> 12;
        frontier_copy_words(actor + 136, actor + 104, 32);
        if (type == 5)
        {
            for (index = 0; index != 3; ++index)
            {
                w_u32(actor + 80 + 4 * index, r_u32(actor + 156 + 4 * index));
                w_u32(actor + 156 + 4 * index, 0);
            }
        }
        else
        {
            for (index = 0; index != 3; ++index)
            {
                int32_t position = ((int32_t)(uint32_t)(r_u32(player + 16 + 4 * index)));
                input[index] = (int16_t)(r_u16(actor + 12 + 4 * index) - frontier_asr((uint32_t)position, 10 - shift));
            }
            frontier_67AC4_rotate(camera, input, transformed);
            for (index = 0; index != 3; ++index)
                w_u32(actor + 80 + 4 * index, (uint32_t)transformed[index]);
        }
        goto visible;
    }
    {
        int32_t threshold = ((int32_t)(uint32_t)(r_u32(0x800CC6A4 + 4 * type)));
        int32_t distance = ((int32_t)(uint32_t)(r_u32(actor + 44 + 4 * player_index)));
        int outside = threshold < distance;

        if (type == 6 && r_s8(actor + 8) == 0)
            outside = ((int32_t)(uint32_t)((uint32_t)threshold + 32000u)) < distance;
        if (outside || distance < 0)
            return frontier_clear_player_visibility(actor, player_index);
    }
    if (r_u32(actor + 100))
    {
        (void)(uint32_t)sub_8001298C(actor, (uint32_t)(int32_t)r_s8(actor + 10 + player_index));
        w_u32(actor + 36, r_u32(actor + 36) & ~0x80u);
    }
    for (index = 0; index != 2; ++index)
    {
        uint32_t child = r_u32(actor + 56 + 8 * index);

        if (child && r_u32(child + 100))
        {
            (void)(uint32_t)sub_8001298C(child, (uint32_t)(int32_t)r_s8(child + 10 + player_index));
            w_u32(child + 36, r_u32(child + 36) & ~0x80u);
        }
    }
    specification = r_u32(actor);
    {
        uint32_t actor_flags = r_u32(actor + 36);
        int32_t distance = ((int32_t)(uint32_t)(r_u32(actor + 44 + 4 * player_index)));

        if (((actor_flags >> 5) & 3) != 1 && ((uint32_t)distance - 129u) < 0x27f)
        {
            if (r_u32(r_u32(0x8005B2F8) + 48) != 1 || type == 3)
                w_u32(actor + 36, (actor_flags & ~0x80u) | ((type - 1 < 2) << 7));
            else
            {
                uint32_t active = 0;

                if (r_u32(specification + 44))
                {
                    actor_flags |= 0x80;
                    w_u32(actor + 36, actor_flags);
                    active = 1;
                }
                for (index = 0; index != 2; ++index)
                {
                    uint32_t child = r_u32(actor + 56 + 8 * index);

                    if (child && r_u32(child + 100) && r_u32(r_u32(child) + 44))
                    {
                        ++active;
                        w_u32(child + 36, r_u32(child + 36) | 0x80);
                    }
                }
                flags = r_u8(actor + 9);
                w_u8(actor + 9, (uint8_t)((16 * active) | (flags & 0xcf)));
            }
        }
    }
    shift = r_u16(specification + 14) >> 12;
    if (r_s8(actor + 72) == 3)
    {
        int32_t world[3];

        for (index = 0; index != 3; ++index)
            world[index] = ((int32_t)(uint32_t)(r_u32(actor + 12 + 4 * index) << shift));
        if (r_u8(actor + 9) & 4)
        {
            uint32_t correction_shift = 4 - shift;

            for (index = 0; index != 3; ++index)
            {
                int32_t value = (int16_t)r_u16(actor + 28 + 2 * index);
                input[index] = (int16_t)(shift == 4 ? value : ((int32_t)(uint32_t)(frontier_asr((uint32_t)value, correction_shift))));
            }
            frontier_67AC4_rotate(actor + 104, input, transformed);
            for (index = 0; index != 3; ++index)
            {
                world[index] = ((int32_t)(uint32_t)((uint32_t)world[index] + (uint32_t)transformed[index]));
                w_u32(actor + 12 + 4 * index, r_u32(actor + 12 + 4 * index) + frontier_asr((uint32_t)transformed[index], shift));
            }
        }
        for (index = 0; index != 3; ++index)
        {
            int32_t position = ((int32_t)(uint32_t)(r_u32(player + 16 + 4 * index)));
            input[index] = (int16_t)((uint32_t)world[index] - frontier_asr((uint32_t)position, 10 - shift));
        }
        w_u32(0x1F8002BCu, r_u32(0x1F8002BCu) + 32);
        frontier_copy_words(r_u32(0x1F8002BCu), camera, 32);
        frontier_67AC4_rotate(r_u32(0x1F8002BCu), input, transformed);
    }
    else
    {
        for (index = 0; index != 3; ++index)
            input[index] = (int16_t)frontier_asr(r_u32(actor + 184 + 4 * index) - r_u32(player + 16 + 4 * index), 10);
        w_u32(0x1F8002BCu, r_u32(0x1F8002BCu) + 32);
        frontier_copy_words(r_u32(0x1F8002BCu), camera, 32);
        frontier_67AC4_rotate(r_u32(0x1F8002BCu), input, transformed);
        for (index = 0; index != 3; ++index)
            transformed[index] = (int32_t)((uint32_t)transformed[index] << shift);
    }
    for (index = 0; index != 3; ++index)
        w_u32(actor + 80 + 4 * index, (uint32_t)transformed[index]);
    {
        int32_t x = transformed[0];
        int32_t y = transformed[1];
        int32_t z = transformed[2];
        int32_t radius = ((int32_t)(uint32_t)(r_u32(actor + 40)));

        if (type == 3)
        {
            x = ((int32_t)(uint32_t)(frontier_asr((uint32_t)x, shift)));
            y = ((int32_t)(uint32_t)(frontier_asr((uint32_t)y, shift)));
        }
        else
        {
            x = ((int32_t)(uint32_t)(frontier_asr((x < 0 ? (uint32_t)x + (uint32_t)radius : (uint32_t)x - (uint32_t)radius), shift)));
            y = ((int32_t)(uint32_t)(frontier_asr((y < 0 ? (uint32_t)y + (uint32_t)radius : (uint32_t)y - (uint32_t)radius), shift)));
            z = ((int32_t)(uint32_t)((uint32_t)z + (uint32_t)radius));
        }
        z = ((int32_t)(uint32_t)(frontier_asr((uint32_t)z, shift)));
        if (z < x || z < ((int32_t)(uint32_t)(0u - (uint32_t)x)) || z < y || z < ((int32_t)(uint32_t)(0u - (uint32_t)y)) || z < 40)
        {
            w_u32(0x1F8002BCu, r_u32(0x1F8002BCu) - 32);
            return frontier_clear_player_visibility(actor, player_index);
        }
    }
    flags = r_u8(actor + 9) | bit;
    w_u8(actor + 9, flags);
    frontier_copy_words(actor + 136, r_u32(0x1F8002BCu), 32);
    w_u32(0x1F8002BCu, r_u32(0x1F8002BCu) - 32);

visible:
    if (!r_u32(actor + 52))
    {
        uint32_t head = r_u32(0x8005B280);

        w_u32(actor + 168, head);
        w_u32(0x8005B280, actor);
    }
    return r_u8(actor + 9) & bit;
}

uint32_t sub_800358C0(uint32_t player_index)
{
    uint32_t count_address = 0x8005B568 + 4 * player_index;
    int32_t count;
    uint32_t records;

    FUNCTION_MARKER(0x800358C0, "SLUS_010.53");
    (void)(uint32_t)sub_80035F48(0);
    count = ((int32_t)(uint32_t)(r_u32(count_address)));
    if (count <= 0)
        return count_address;
    records = 0x800D9B80 + 48 * player_index;
    (void)(uint32_t)sub_800353C4(0x8005B578 + 4 * player_index, (uint32_t)count, records, player_index);
    (void)(uint32_t)sub_80036438((uint32_t)count, records, player_index);
    return (uint32_t)sub_80035680((uint32_t)count, records, player_index);
}

uint32_t sub_80035F48(uint32_t player_index)
{
    uint32_t selected_address = 0x8005B508 + 4 * player_index;
    uint32_t output = 0x800D9B80 + 48 * player_index;
    uint32_t table = r_u32(0x8005B518);
    uint32_t actor;
    uint32_t selected;
    uint32_t packed;
    uint32_t id;
    int32_t value;
    uint32_t index;

    FUNCTION_MARKER(0x80035F48, "SLUS_010.53");
    if (!table)
    {
        uint32_t count = sub_80035E60(output, player_index);

        w_u32(0x8005B568 + 4 * player_index, count);
        w_u32(0x8005B578 + 4 * player_index, count);
        table = r_u32(0x8005B518);
    }
    actor = r_u32(0x8005B268 + 4 * player_index);
    selected = r_u32(actor + 852);
    if ((uint32_t)(r_u32(selected + 604) - 3) >= 2)
        selected = actor;
    packed = r_u32(selected + 360);
    if (!(packed >> 16))
    {
        uint32_t current = r_u32(selected_address);

        id = packed & 0xffff;
        value = (int16_t)r_u16(selected + 370);
        if (!current || !((int)((int16_t)r_u16(((uint32_t)(current)) + 12) == (int32_t)((uint32_t)(id)) && ((int32_t)(value)) >= (int16_t)r_u16(((uint32_t)(current)) + 14) && ((int32_t)(value)) <= (int16_t)r_u16(((uint32_t)(current)) + 16))))
        {
            if (current && (int16_t)r_u16(current + 12) == (int32_t)id)
            {
                current += 32 * r_u32(selected + 364);
                w_u32(selected_address, current);
            }
            current = r_u32(selected_address);
            if (!current || !((int)((int16_t)r_u16(((uint32_t)(current)) + 12) == (int32_t)((uint32_t)(id)) && ((int32_t)(value)) >= (int16_t)r_u16(((uint32_t)(current)) + 14) && ((int32_t)(value)) <= (int16_t)r_u16(((uint32_t)(current)) + 16))))
            {
                uint32_t record;

                table = r_u32(0x8005B518);
                w_u32(selected_address, 0);
                record = r_u32(table + 4 * id);
                for (index = 0; index != 140; ++index, record += 32)
                {
                    if ((int16_t)r_u16(record + 12) != (int32_t)id)
                        break;
                    if (((int)((int16_t)r_u16(((uint32_t)(record)) + 12) == (int32_t)((uint32_t)(id)) && ((int32_t)(value)) >= (int16_t)r_u16(((uint32_t)(record)) + 14) && ((int32_t)(value)) <= (int16_t)r_u16(((uint32_t)(record)) + 16))))
                    {
                        w_u32(selected_address, record);
                        break;
                    }
                }
            }
        }
    }
    selected = r_u32(selected_address);
    if (!selected)
        return 0;
    for (index = 0; index != 12; ++index)
        w_u32(output + 4 * index, UINT32_MAX);
    for (index = 6; index != 0; --index)
        w_u32(0x800D9B68 + 4 * (index - 1), UINT32_MAX);
    w_u32(0x8005B568 + 4 * player_index, 0);
    w_u32(0x8005B570, 0);
    w_u32(0x8005B574, 0);
    for (index = 0; index != 6; ++index)
    {
        uint32_t encoded = r_u16(r_u32(selected_address) + 18 + 2 * index);
        uint32_t decoded;
        uint32_t actor_index;
        uint32_t record;

        if (encoded == 0xffff)
            break;
        decoded = ((encoded & 0x8000) << 16) | ((encoded & 0x7c00) << 13) | (encoded & 0x3ff);
        if (decoded == UINT32_MAX)
            continue;
        actor_index = sub_800329BC(decoded & 0x0fffffff, player_index);
        if (actor_index != UINT32_MAX)
        {
            record = 0x800D87E8 + 112 * actor_index;
            if (r_u32(record + 8) != UINT32_MAX && r_u32(record + 4) && (sub_800363F0(record) & 255))
            {
                uint32_t count = r_u32(0x8005B568 + 4 * player_index);

                w_u32(output + 4 * count, actor_index);
                w_u32(0x8005B568 + 4 * player_index, count + 1);
                continue;
            }
        }
        {
            uint32_t deferred = r_u32(0x8005B574);

            w_u32(0x800D9B68 + 4 * deferred, decoded);
            w_u32(0x8005B574, deferred + 1);
        }
    }
    {
        uint32_t count = r_u32(0x8005B568 + 4 * player_index);

        w_u32(0x8005B578 + 4 * player_index, count);
        w_u32(0x8005B570, count + r_u32(0x8005B574));
        return count;
    }
}

uint32_t sub_800363F0(uint32_t record)
{
    uint32_t index;

    FUNCTION_MARKER(0x800363F0, "SLUS_010.53");
    for (index = 0; index != 2; ++index)
        if (r_u32(record + 88 + 4 * index) != UINT32_MAX && !r_u32(record + 96 + 4 * index))
            return 0;
    return 1;
}

uint32_t sub_80035040(uint32_t record, uint32_t vertices, int32_t count)
{
    uint32_t common_flags = 0xff;
    uint32_t combined_flags = 0;
    int32_t index;

    FUNCTION_MARKER(0x80035040, "SLUS_010.53");
    for (index = 0; index < 8; ++index)
        xport_gte_write_control((uint32_t)index, r_u32(record + 12 + 4 * (uint32_t)index));
    w_u32(record + 44, (uint32_t)-641);
    w_u32(record + 48, 0xffff);
    for (index = 0; index < count; ++index)
    {
        int16_t vertex_index = (int16_t)r_u16(vertices + 2 * (uint32_t)index);
        uint32_t model_vertices = r_u32(r_u32(record) + 52);
        int32_t transformed[3];
        uint32_t far_flags;
        uint32_t near_flags;
        uint32_t scaled_z;
        uint32_t absolute_z;
        uint32_t sign;
        int32_t negative_absolute_z;
        int32_t wide_x;
        int32_t narrow_x;
        unsigned row;

        uint32_t vertex = model_vertices + 8 * (uint32_t)(int32_t)vertex_index + 4;
        xport_gte_write_data(0, r_u32(vertex));
        xport_gte_write_data(1, (uint32_t)(int32_t)r_s16(vertex + 4));
        xport_gte_execute(0x480012);
        for (row = 0; row != 3; ++row)
            transformed[row] = (int32_t)xport_gte_read_data(25 + row);
        if (transformed[2] < ((int32_t)(uint32_t)(r_u32(record + 48))))
            w_u32(record + 48, (uint32_t)transformed[2]);
        if (transformed[2] > ((int32_t)(uint32_t)(r_u32(record + 44))))
            w_u32(record + 44, (uint32_t)transformed[2]);
        scaled_z = (uint32_t)transformed[2] << 4;
        sign = 0u - (scaled_z >> 31);
        absolute_z = (scaled_z + sign) ^ sign;
        negative_absolute_z = (int32_t)(0u - absolute_z);
        wide_x = (int32_t)((uint32_t)transformed[0] * 21u);
        narrow_x = (int32_t)((uint32_t)transformed[0] << 4);
        far_flags = (int32_t)scaled_z < -640 ? 4u : 0u;
        if ((int32_t)absolute_z < wide_x)
            far_flags |= 2;
        if (wide_x < negative_absolute_z)
            far_flags |= 1;
        common_flags &= far_flags;
        near_flags = (int32_t)scaled_z < -640 ? 4u : 0u;
        if ((int32_t)absolute_z < narrow_x)
            near_flags |= 2;
        if (narrow_x < negative_absolute_z)
            near_flags |= 1;
        combined_flags |= near_flags;
    }
    if (common_flags)
        return 0;
    return combined_flags ? 1 : 2;
}

static uint32_t frontier_classify_segment(uint32_t record, uint32_t vertices, int32_t count, int32_t *average, int32_t *minimum, int32_t *maximum)
{
    uint32_t common_flags = 7;
    uint32_t combined_flags = 0;
    uint32_t scratch = r_u32(0x8005ACB0);
    uint32_t sum = 0;
    int32_t index;

    for (index = 0; index < 8; ++index)
        xport_gte_write_control((uint32_t)index, r_u32(record + 12 + 4 * (uint32_t)index));
    *minimum = 0xffffff;
    *maximum = -641;
    for (index = 0; index < count; ++index)
    {
        int16_t vertex_index = (int16_t)r_u16(vertices + 2 * (uint32_t)index);
        uint32_t model_vertices = r_u32(r_u32(record) + 52);
        int32_t transformed[3];
        uint32_t far_flags;
        uint32_t near_flags;
        uint32_t scaled_z;
        uint32_t absolute_z;
        uint32_t sign;
        int32_t negative_absolute_z;
        int32_t wide_x;
        int32_t narrow_x;
        unsigned row;

        uint32_t vertex = model_vertices + 8 * (uint32_t)(int32_t)vertex_index + 4;
        xport_gte_write_data(0, r_u32(vertex));
        xport_gte_write_data(1, r_u32(vertex + 4));
        xport_gte_execute(0x480012);
        for (row = 0; row != 3; ++row)
        {
            transformed[row] = (int32_t)xport_gte_read_data(25 + row);
            w_u32(scratch + 16 * (uint32_t)index + 4 * row, (uint32_t)transformed[row]);
        }
        if (transformed[2] < *minimum)
            *minimum = transformed[2];
        if (transformed[2] > *maximum)
            *maximum = transformed[2];
        if (index < 4)
            sum += (uint32_t)transformed[2];
        scaled_z = (uint32_t)transformed[2] << 4;
        sign = 0u - (scaled_z >> 31);
        absolute_z = (scaled_z + sign) ^ sign;
        negative_absolute_z = (int32_t)(0u - absolute_z);
        wide_x = (int32_t)((uint32_t)transformed[0] * 21u);
        narrow_x = (int32_t)((uint32_t)transformed[0] << 4);
        far_flags = (int32_t)scaled_z < -640 ? 4u : 0u;
        if ((int32_t)absolute_z < wide_x)
            far_flags |= 2;
        if (wide_x < negative_absolute_z)
            far_flags |= 1;
        common_flags &= far_flags;
        near_flags = (int32_t)scaled_z < -640 ? 4u : 0u;
        if ((int32_t)absolute_z < narrow_x)
            near_flags |= 2;
        if (narrow_x < negative_absolute_z)
            near_flags |= 1;
        combined_flags |= near_flags;
    }
    *average = (int32_t)frontier_asr(sum, 2);
    if (common_flags)
        return 0;
    return combined_flags ? 1 : 2;
}

uint32_t sub_800351EC(uint32_t record, uint32_t vertices, int32_t count, uint32_t average_output, uint32_t minimum_output, uint32_t maximum_output)
{
    int32_t average;
    int32_t minimum;
    int32_t maximum;
    uint32_t result;

    FUNCTION_MARKER(0x800351EC, "SLUS_010.53");
    result = frontier_classify_segment(record, vertices, count, &average, &minimum, &maximum);
    w_u32(average_output, (uint32_t)average);
    w_u32(minimum_output, (uint32_t)minimum);
    w_u32(maximum_output, (uint32_t)maximum);
    return result;
}

uint32_t sub_800353C4(uint32_t count_pointer, int32_t count, uint32_t indices, uint32_t player_index)
{
    uint32_t player = r_u32(0x8005AEC0 + 4 * player_index);
    uint32_t special_actor = r_u32(0x800CD898 + 1132 * player_index + 176);
    int32_t special_slot = -1;
    int32_t slot;

    FUNCTION_MARKER(0x800353C4, "SLUS_010.53");
    w_u32(0x8005AEE0, UINT32_MAX);
    for (slot = 0; slot < count; ++slot)
    {
        uint32_t slot_address = indices + 4 * (uint32_t)slot;
        uint32_t actor_index = r_u32(slot_address);
        uint32_t record;
        uint32_t model;
        uint32_t table;
        int32_t delta[3];
        int32_t transformed[3];
        uint32_t visible;
        unsigned index;

        if (actor_index == UINT32_MAX)
            continue;
        record = 0x800D87EC + 112 * actor_index;
        for (index = 0; index != 8; ++index)
            w_u32(record + 12 + 4 * index, r_u32(player + 92 + 4 * index));
        model = r_u32(record);
        for (index = 0; index != 3; ++index)
            delta[index] = ((int32_t)(uint32_t)(r_u32(model + 8 + 4 * index) - r_u32(player + 28 + 4 * index)));
        (void)frontier_matrix_mul32(record + 12, delta, transformed);
        for (index = 0; index != 3; ++index)
            w_u32(record + 32 + 4 * index, (uint32_t)transformed[index]);
        table = r_u32(model + 44);
        {
            int32_t first = (int16_t)r_u16(table);
            int32_t last = (int16_t)r_u16(table + 2);

            visible = (uint32_t)sub_80035040(record, table + 2 * (uint32_t)first, (uint32_t)(last - first)) & 0xff;
        }
        if (r_u32(0x800D87E8 + 112 * actor_index + 8) == special_actor)
        {
            if (((int32_t)(uint32_t)(r_u32(record + 44))) < 0)
                w_u32(record + 44, 1280);
            w_u32(0x8005AEE0, actor_index);
            special_slot = slot;
        }
        else if (!visible)
        {
            w_u32(slot_address, UINT32_MAX);
            w_u32(count_pointer, r_u32(count_pointer) - 1);
        }
    }
    if (r_u32(0x8005AEE0) != UINT32_MAX && special_slot != -1)
    {
        uint32_t special_address = indices + 4 * (uint32_t)special_slot;

        w_u32(special_address, r_u32(indices));
        w_u32(indices, r_u32(0x8005AEE0));
        return special_address;
    }
    return UINT32_MAX;
}

uint32_t sub_80036438(int32_t count, uint32_t indices, uint32_t player_index)
{
    uint32_t player = 0x800CD898 + 1132 * player_index;
    uint32_t special;
    int32_t last_index;
    uint32_t swaps;
    uint32_t result = 0x800E0000;

    FUNCTION_MARKER(0x80036438, "SLUS_010.53");
    (void)sub_8002E698(xport_guest_ptr(r_u32(player + 340) + 14, 6), xport_guest_ptr(player + 444, 6));
    special = r_u32(0x8005AEE0);
    if (special != UINT32_MAX)
    {
        w_u32(indices, special);
        indices += 4;
        last_index = count - 2;
    }
    else
    {
        last_index = count - 1;
    }
    do
    {
        int32_t index;

        swaps = 0;
        for (index = 0; index < last_index; ++index)
        {
            uint32_t current_address = indices + 4 * (uint32_t)index;
            uint32_t current_index = r_u32(current_address);
            int32_t next_index = index + 1;
            uint32_t next_address;
            uint32_t next;
            uint32_t current_record;
            uint32_t next_record;
            int32_t current_depth;
            int32_t next_depth;
            uint32_t should_swap;

            if (current_index == UINT32_MAX)
                continue;
            while (next_index < last_index && r_u32(indices + 4 * (uint32_t)next_index) == UINT32_MAX)
                ++next_index;
            next_address = indices + 4 * (uint32_t)next_index;
            next = r_u32(next_address);
            if (next == UINT32_MAX)
                continue;
            current_record = 0x800D87E8 + 112 * current_index;
            next_record = 0x800D87E8 + 112 * next;
            current_depth = ((int32_t)(uint32_t)(r_u32(current_record + 48)));
            next_depth = ((int32_t)(uint32_t)(r_u32(next_record + 48)));
            should_swap = next_depth < current_depth;
            {
                int32_t difference = (int32_t)((uint32_t)current_depth - (uint32_t)next_depth);
                uint32_t sign = (uint32_t)(difference >> 31);
                int32_t magnitude = (int32_t)(((uint32_t)difference + sign) ^ sign);

                if (magnitude < 640)
                    should_swap = ((int32_t)(uint32_t)(r_u32(next_record + 64))) < ((int32_t)(uint32_t)(r_u32(current_record + 64)));
            }
            if (should_swap)
            {
                w_u32(current_address, next);
                w_u32(next_address, current_index);
                ++swaps;
            }
        }
        if (last_index > 0)
            result = 0;
    } while (swaps);
    return result;
}

uint32_t sub_80036614(uint32_t record, uint32_t slot, uint32_t flags)
{
    uint32_t result;

    FUNCTION_MARKER(0x80036614, "SLUS_010.53");
    result = r_u32(record + 52) | (flags << ((slot << 2) & 31));
    w_u32(record + 52, result);
    return result;
}

uint32_t sub_80035680(int32_t count, uint32_t indices, uint32_t player_index)
{
    int32_t threshold = r_u32(r_u32(0x8005B2F8) + 48) == 2 ? 4800 : 9600;
    uint32_t priority_count = 0;
    int32_t actor_slot;

    FUNCTION_MARKER(0x80035680, "SLUS_010.53");
    for (actor_slot = 0; actor_slot < count; ++actor_slot)
    {
        uint32_t actor_index = r_u32(indices + 4 * (uint32_t)actor_slot);
        uint32_t record;
        uint32_t model;
        uint32_t table;
        uint32_t segment_count;
        uint32_t priority = 0;
        uint32_t segment;

        if (actor_index == UINT32_MAX)
            continue;
        record = 0x800D87EC + 112 * actor_index;
        model = r_u32(record);
        table = r_u32(model + 44);
        segment_count = r_u16(model + 6);
        w_u32(record + 52, 0);
        for (segment = 0; segment < segment_count; ++segment)
        {
            int32_t first = (int16_t)r_u16(table + 2 * segment + 2);
            int32_t last = (int16_t)r_u16(table + 2 * segment + 4);
            int32_t average;
            int32_t minimum;
            int32_t maximum;
            uint32_t classification;
            uint32_t flags;

            classification = frontier_classify_segment(record, table + 2 * (uint32_t)first, last - first, &average, &minimum, &maximum);
            if (segment == 1)
                w_u32(record + 60, (uint32_t)average);
            if (!classification)
            {
                (void)sub_80036614(record, segment, 0);
                continue;
            }
            else if (r_u32(model + 60) && minimum < threshold && priority_count < 4)
            {
                priority = 1;
                flags = classification == 1 || average < 2048 ? 7 : 3;
            }
            else
            {
                flags = classification == 1 || average < 2048 ? 5 : 1;
            }
            (void)sub_80036614(record, segment, flags);
            if (average < threshold)
                (void)sub_80036614(record, segment, 12);
        }
        if (priority)
            ++priority_count;
    }
    return count > 0 ? 0 : 2;
}

uint32_t sub_80013828(uint32_t entry)
{
    uint32_t owner;
    uint32_t slot;

    FUNCTION_MARKER(0x80013828, "SLUS_010.53");
    if (r_u32(0x8005B2D0) || r_u32(entry + 12) != 0x50414E4F)
        return 2;
    owner = r_u32(0x8005B278);
    for (slot = 0; slot != 20; ++slot)
    {
        uint32_t record = owner + 740 + 12 * slot;

        if (r_u32(record) == entry)
        {
            if (slot == (uint32_t)(int32_t)r_s16(owner + 16) || slot == (uint32_t)(int32_t)r_s16(owner + 18))
                return 2;
            w_u32(record, 0);
            w_u32(record + 4, 0);
            w_u32(record + 8, 0);
            w_u16(owner + 34, r_u16(owner + 34) - 1);
            return 1;
        }
    }
    return 2;
}

uint32_t sub_80023868(uint32_t unused)
{
    FUNCTION_MARKER(0x80023868, "SLUS_010.53");
    return 0;
}

uint32_t sub_800CB02C(uint32_t actor)
{
    uint32_t player = r_u32(0x8005B38C);
    uint32_t reference = player;
    uint32_t body = r_u32(player + 852);
    uint32_t type = r_u8(actor + 553);
    uint32_t projection = 0;
    uint32_t mismatch;
    uint32_t speed;
    uint32_t absolute_projection;
    int32_t difference;
    int32_t sign;
    int32_t direction_dot;
    uint32_t index;

    FUNCTION_MARKER(0x800CB02C, "RASHCDG.BIN");
    if (r_u32(body + 604) >= 3)
        reference = body;
    difference = (int32_t)(r_u32(actor + 480) - r_u32(reference + 480));
    sign = difference >> 31;
    speed = ((uint32_t)difference + (uint32_t)sign) ^ (uint32_t)sign;
    mismatch = (r_u32(actor + 364) >> 31) != (uint32_t)((int32_t)r_u32(actor + 344) < (int32_t)r_u32(reference + 344));
    for (index = 0; index < 3; ++index)
    {
        int32_t delta = (int32_t)(r_u32(actor + 184 + 4 * index) - r_u32(reference + 184 + 4 * index));
        int32_t axis = (int32_t)r_s16(actor + 444 + 2 * index) * 16;
        projection += (uint32_t)sub_8001FC90(delta, axis);
    }
    direction_dot = (int32_t)sub_8002E698(xport_guest_ptr(r_u32(0x8005B38C) + 444, 6), xport_guest_ptr(actor + 444, 6));
    sign = (int32_t)projection >> 31;
    absolute_projection = (projection + (uint32_t)sign) ^ (uint32_t)sign;
    if ((int32_t)projection >= 0 || (int32_t)absolute_projection >= (int32_t)sub_8001FC90((int32_t)speed, 457800) || direction_dot >= -58982)
    {
        uint32_t selected = type - 1 < 2;
        if (selected)
        {
            uint32_t flags = (mismatch << 16) | (r_u32(actor + 552) & 0xFF0000FF) | 1;
            w_u32(actor + 552, flags);
            return flags;
        }
        return selected;
    }
    if (type - 1 >= 2)
    {
        int32_t elapsed = (int32_t)(r_u32(r_u32(0x8005B2F8) + 16) - r_u32(actor + 548));
        uint32_t selected = (int32_t)r_u32(0x800CCC1C) < elapsed;
        if (selected)
        {
            uint32_t flags = (mismatch << 16) | (r_u32(actor + 552) & 0xFF00FFFF);
            int32_t scaled;
            w_u32(actor + 552, flags);
            scaled = (int32_t)sub_8001FC90((int32_t)speed, 261600);
            flags = (r_u32(actor + 552) & 0xFFFF00FF) | (scaled >= (int32_t)absolute_projection ? 0x201 : 0x101);
            w_u32(actor + 552, flags);
            w_u32(actor + 548, r_u32(r_u32(0x8005B2F8) + 16));
            selected = sub_8001FC58() % 900 + 300;
            w_u32(0x800CCC1C, selected);
        }
        return selected;
    }
    return 0x80060000;
}

uint32_t sub_800CAAF0(uint32_t actor)
{
    uint32_t type, flags, variant, animation, speed, transition, complete;

    FUNCTION_MARKER(0x800CAAF0, "RASHCDG.BIN");
    type = r_u8(actor + 553);
    flags = r_u32(actor + 552);
    variant = r_u8(actor + 554);
    animation = r_u16(0x800D5730u + 6u * type);
    speed = r_u32(0x800CCC20u + 16u * r_u8(actor + 566) + 12);
    if (flags & 1u)
        return rrj_actor_begin_animation(actor, type, variant, animation, speed);

    transition = (flags >> 29) & 1u;
    if (!transition)
        complete = sub_8005BE58(r_u32(actor + 540));
    else if (flags & 0x40000000u)
        complete = 0;
    else
    {
        complete = sub_8008FD5C(actor);
        if (complete)
        {
            uint32_t config = 0x800CCC20u + 16u * r_u8(actor + 566);
            uint32_t state = r_u8(actor + 565);
            uint32_t selected_speed = r_u32(config + 8);
            w_u8(actor + 565, state & 0xFBu);
            w_u32(actor + 480, selected_speed);
            animation = r_u16(actor + 544);
            w_u16(0x800D5744u, (uint16_t)((animation == 51 || animation == 45 || animation == 61 || animation == 62 || animation == 68 || animation == 57 || animation == 53) ? 4 : 5));
            animation = r_u16(actor + 544);
            state = r_u8(0x800D5746u);
            w_u8(0x800D5746u, (state & 0xEFu) | (((animation - 60u < 4u) || animation == 44) ? 16u : 0u));
            w_u32(actor + 552, r_u32(actor + 552) & 0xDFFFFFFFu);
        }
    }
    if (complete)
    {
        animation = r_u16(actor + 544);
        if (animation == 63)
        {
            uint32_t x = r_u16(actor + 450);
            uint32_t y = r_u16(actor + 452);
            uint32_t state = r_u8(actor + 565);
            uint32_t z, reversed_x, reversed_y, reversed_z;
            w_u32(actor + 560, 0);
            w_u16(actor + 434, 0);
            w_u16(actor + 450, (uint16_t)(0u - x));
            z = r_u16(actor + 454);
            w_u16(actor + 452, (uint16_t)(0u - y));
            reversed_x = r_u16(actor + 450);
            reversed_y = r_u16(actor + 452);
            w_u16(actor + 454, (uint16_t)(0u - z));
            reversed_z = r_u16(actor + 454);
            w_u16(actor + 444, (uint16_t)reversed_x);
            w_u8(actor + 565, (state & 0xFDu) | ((state & 2u) ? 0u : 2u));
            w_u16(actor + 446, (uint16_t)reversed_y);
            w_u16(actor + 436, (uint16_t)(0u - reversed_x));
            w_u16(actor + 448, (uint16_t)reversed_z);
            w_u16(actor + 432, (uint16_t)reversed_z);
        }
        {
            uint32_t entry = 0x800D5730u + 6u * type;
            uint32_t next_type = r_u16(entry + 2);
            uint32_t next_flags = r_u32(actor + 552);
            next_flags = (next_type << 8) | (next_flags & 0xFFFF00FFu) | 1u;
            w_u32(actor + 552, next_flags);
            next_flags = (next_flags & 0xFF00FFFFu) | (r_u8(entry + 4) << 16);
            w_u32(actor + 552, next_flags);
        }
    }
    if (!transition && type - 4u >= 2u)
        (void)sub_800CB02C(actor);
    flags = r_u32(actor + 552) & 1u;
    if (flags)
        return sub_800CAAF0(actor);
    return flags;
}

uint32_t sub_800CB4F8(int32_t delta)
{
    uint32_t count_pointer = r_u32(0x800CE4FC);
    int32_t remaining = ((int32_t)(uint32_t)(r_u32(count_pointer)));
    uint32_t actor = r_u32(0x800CE4F0);
    uint32_t stride = r_u32(0x800CE4F4);

    FUNCTION_MARKER(0x800CB4F8, "RASHCDG.BIN");
    while (remaining >= 0)
    {
        if (r_u16(actor + 172))
        {
            if (r_u16(actor + 320))
            {
                uint32_t flags;
                uint8_t state;
                if ((r_u8(actor + 9) & 3) || r_u8(actor + 553))
                {
                    state = r_u8(actor + 565);
                    if (state & 8)
                    {
                        (void)sub_8005BE44(r_u32(actor + 540));
                        w_u8(actor + 565, state & 0xF7);
                    }
                    (void)(uint32_t)sub_800CAAF0(actor);
                }
                else if (!(r_u8(actor + 565) & 8))
                {
                    (void)sub_8005BE20(r_u32(actor + 540));
                    xport_update_u8(actor + 565, XPORT_MEMORY_UPDATE_OR, 8);
                }
                flags = r_u32(actor + 568);
                if (flags & 2)
                {
                    (void)sub_80037450(actor);
                    (void)sub_8009246C(actor);
                    w_u32(actor + 568, (flags | 1) & ~2u);
                }
                flags = r_u32(actor + 552);
                if (flags & 0x20000000)
                {
                    if (flags & 0x40000000)
                    {
                        int32_t scale = (int32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(0x800D3964))), delta);
                        int32_t value = (int32_t)sub_8001FC90(scale, ((int32_t)(uint32_t)(r_u32(actor + 456))));
                        w_u32(actor + 456, r_u32(actor + 456) - (uint32_t)value);
                        value = (int32_t)sub_8001FC90(scale, ((int32_t)(uint32_t)(r_u32(actor + 460)))) - (int32_t)sub_8001FC90(643207, delta);
                        w_u32(actor + 460, r_u32(actor + 460) - (uint32_t)value);
                        value = (int32_t)sub_8001FC90(scale, ((int32_t)(uint32_t)(r_u32(actor + 464))));
                        w_u32(actor + 464, r_u32(actor + 464) - (uint32_t)value);
                        value = ((int32_t)(uint32_t)(sub_8002E548(xport_guest_ptr(actor + 456, 12))));
                        w_u32(actor + 480, (uint32_t)value);
                        if (value)
                        {
                            uint32_t magnitude = value < 0 ? (uint32_t)(0u - (uint32_t)value) : (uint32_t)value;
                            uint32_t denominator = (magnitude >> 1) + ((magnitude - 2) >> 31);
                            int32_t reciprocal = (int32_t)(0x80000000u / denominator);
                            (void)sub_8002EED8(value < 0 ? (uint32_t)-reciprocal : (uint32_t)reciprocal, xport_guest_ptr(actor + 456, 12), xport_guest_ptr(actor + 450, 6));
                        }
                        w_u32(actor + 552, flags & 0x7FFFFFFF);
                    }
                    else if (!(flags & 0x04000000))
                    {
                        (void)sub_8008F754(actor, delta, actor + 488);
                    }
                }
            }
            else
            {
                (void)sub_8008C000(actor + 172, 2);
            }
        }
        actor += stride;
        --remaining;
    }
    return stride;
}

void sub_800C8B24(uint32_t player_index)
{
    int32_t view = r_s8(0x8005B58C);
    uint32_t output = 0x800D9BE0 + 40 * (uint32_t)view + 20 * player_index;
    uint32_t players = r_u32(0x8005B470);
    uint32_t selected;

    FUNCTION_MARKER(0x800C8B24, "RASHCDG.BIN");
    w_u32(0x8005B588, 0);
    w_u32(0x8005B59C, output);
    if (player_index || r_u8(players + 4))
        selected = r_u8(players + 5);
    else
        selected = r_u8(players + 6);
    selected = players + 112 * selected + 16;
    w_u32(0x800CD660, selected);
    if (r_u32(r_u32(0x8005B2F8) + 48) >= 2)
    {
        uint32_t offsets = r_u32(0x8005B474) + 8 * player_index;

        (void)(uint32_t)sub_8001C304((uint32_t)((int16_t)r_u16(selected) + (int16_t)r_u16(offsets)), (uint32_t)((int16_t)r_u16(selected + 2) + (int16_t)r_u16(offsets + 2)), (uint32_t)(int32_t)r_s16(offsets + 4), (uint32_t)(int32_t)r_s16(offsets + 6), r_u32(0x8005B59Cu) + 16u);
    }
    (void)(uint32_t)sub_80064B9C(player_index);
    (void)(uint32_t)sub_8002C4F8(0x8005234C, r_u32(0x8005B59Cu) + 8u, player_index);
    if (r_u8(r_u32(0x8005B470u) + 4u))
    {
        w_u32(0x8005B588, 2);
        DrawOTag((uint32_t *)xport_guest_ptr(r_u32(0x8005B59Cu) + 16u, 4));
        return;
    }
    w_u32(0x8005B588, 1);
}

void sub_800C8CD4(uint32_t player_index)
{
    int32_t view = r_s8(0x8005B58Cu);
    uint32_t ordering_slot = 0x800D9C30u + 8 * (uint32_t)view + 4 * player_index;

    FUNCTION_MARKER(0x800C8CD4, "RASHCDG.BIN");
    w_u32(0x8005B5A8u, ordering_slot);
    if (r_u8(0x800D8060u + player_index))
        (void)sub_8002BE14(0x800D7FE8u + 4 * player_index, ordering_slot, player_index);
    DrawOTag((uint32_t *)xport_guest_ptr(r_u32(0x8005B5A8u), 4));
}

uint32_t sub_8004D184(int32_t x, int32_t y)
{
    FUNCTION_MARKER(0x8004D184, "SLUS_010.53");
    SetGeomOffset(x, y);
    return 0;
}

uint32_t sub_8001E084(void)
{
    FUNCTION_MARKER(0x8001E084, "SLUS_010.53");
    return 0;
}

static uint32_t frontier_draw_area_word(int32_t x, int32_t y, uint32_t command)
{
    int32_t width = (int16_t)r_u16(0x80055F70u);
    int32_t height = (int16_t)r_u16(0x80055F72u);

    if (x < 0)
        x = 0;
    else if (x > width - 1)
        x = width - 1;
    if (y < 0)
        y = 0;
    else if (y > height - 1)
        y = height - 1;
    return command | ((uint32_t)y & 0x3FFu) << 10 | ((uint32_t)x & 0x3FFu);
}

uint32_t sub_8001C304(int32_t x, int32_t y, int32_t width, int32_t height, uint32_t ordering_slot)
{
    uint32_t context = r_u32(0x8005B470u);
    uint32_t packet = r_u32(context + 268);
    uint32_t ordering_word;
    int32_t right;
    int32_t bottom;

    FUNCTION_MARKER(0x8001C304, "SLUS_010.53");
    if (packet + 12 >= r_u32(0x8005B4D0u))
        packet = sub_80021C98(packet, 12);
    context = r_u32(0x8005B470u);
    w_u32(context + 268, packet + 12);
    right = (int16_t)((uint16_t)x + (uint16_t)width - 1);
    bottom = (int16_t)((uint16_t)y + (uint16_t)height - 1);
    w_u8(packet + 3, 2);
    w_u32(packet + 4, frontier_draw_area_word((int16_t)x, (int16_t)y, 0xE3000000u));
    w_u32(packet + 8, frontier_draw_area_word(right, bottom, 0xE4000000u));
    ordering_word = r_u32(ordering_slot);
    w_u32(packet, (r_u32(packet) & 0xFF000000u) | (ordering_word & 0x00FFFFFFu));
    ordering_word = (ordering_word & 0xFF000000u) | (packet & 0x00FFFFFFu);
    w_u32(ordering_slot, ordering_word);
    return ordering_word;
}

uint32_t sub_8005FA68(uint32_t ordering_slot, uint32_t packets, uint32_t next_index, uint32_t packet_index)
{
    uint32_t packet = packets + 36 * packet_index;
    int32_t type = r_s8(packet + 32);
    uint32_t result = 36 * next_index;

    FUNCTION_MARKER(0x8005FA68, "RASHCDG.BIN");
    w_u32(packet, (r_u32(0x800CC68Cu + 4 * (uint32_t)type) << 24) | r_u32(ordering_slot));
    w_u32(ordering_slot, (packets + result) & 0x00FFFFFFu);
    return result;
}

uint32_t sub_800C5618(uint32_t player_index)
{
    int32_t x = (int16_t)r_u16(0x80055F7Cu);
    int32_t y = (int16_t)r_u16(0x80055F7Eu);
    uint32_t ordering_slot = r_u32(0x8005B590u + 4 * player_index);

    FUNCTION_MARKER(0x800C5618, "RASHCDG.BIN");
    return sub_8001C304(x, y, 384, 240, ordering_slot);
}

uint32_t sub_8005FAC4(uint32_t packet_base, uint32_t state, uint32_t player_index)
{
    uint32_t game_state = r_u32(0x8005B2F8u);
    uint32_t selected = r_u8(game_state + 6);
    uint32_t player = r_u32(0x8005B3A0u) + 1096 * selected;
    uint32_t descriptor = r_u32(player + 1084);

    FUNCTION_MARKER(0x8005FAC4, "RASHCDG.BIN");
    sub_8002CD78(r_u32(0x8005AD2Cu), r_u8(descriptor + 38), (uint32_t)(int32_t)r_s16(state + 24), (uint32_t)(int32_t)r_s16(state + 26), r_u32(0x8005B590u + 4 * player_index), 0x00808080u);
    return 0;
}

uint32_t sub_80095410(uint32_t actor)
{
    uint32_t record = r_u32(actor + 428);
    uint32_t route = r_u32(actor + 360);

    FUNCTION_MARKER(0x80095410, "RASHCDG.BIN");
    if (record && !((r_u16(record + 118) >> (r_u16(actor + 172) & 31)) & 1u))
        return 1;
    if ((route >> 16) == 1)
        return sub_8003F408(record, route & 0xFFFFu) == 0;
    return sub_8003B4B0(record, route & 0xFFFFu) == 0;
}

uint32_t sub_8008B84C(uint32_t actor)
{
    int32_t count = ((int32_t)(uint32_t)(r_u32(0x8005B1F8u)));
    uint32_t game_state = r_u32(0x8005B2F8u);
    uint32_t candidate = r_u32(0x8005B3A0u);
    uint32_t actor_id = r_u16(actor + 172);
    uint32_t route_limited = sub_80095410(actor);
    uint32_t best_distance = 0x7FFF0000u;
    int32_t best_index = -1;
    int32_t index;

    FUNCTION_MARKER(0x8008B84C, "RASHCDG.BIN");
    for (index = 0; index < count; ++index, candidate += 1096)
    {
        uint32_t candidate_id = r_u16(candidate + 172);
        uint32_t descriptor = r_u32(candidate + 1084);

        if (candidate_id != actor_id && (candidate_id < r_u32(game_state + 48) || (r_u8(descriptor + 1) & 0xFu) != 2 || (r_u8(candidate + 928) & 0x10u)) && (!route_limited || r_u32(candidate + 360) == r_u32(actor + 360)))
        {
            uint32_t delta = (r_u32(candidate + 324) - r_u32(actor + 324)) << 4;
            uint32_t distance = ((int32_t)(uint32_t)(delta)) < 0 ? 0u - delta : delta;

            if (distance < best_distance)
            {
                best_distance = distance;
                best_index = index;
            }
        }
    }
    return best_index < 0 ? 0 : r_u32(0x8005B3A0u) + 1096 * (uint32_t)best_index;
}

uint32_t sub_8005F9B0(uint32_t packets, int32_t delta, int32_t first, int32_t last)
{
    uint32_t record = packets + 36 * (uint32_t)first + 8;
    uint32_t result = last < first;

    FUNCTION_MARKER(0x8005F9B0, "RASHCDG.BIN");
    while (first <= last)
    {
        int32_t mode = r_s8(record + 24);
        uint16_t value = (uint16_t)(r_u16(record + 18) + delta);

        w_u16(record + 18, value);
        if (!mode)
        {
            uint32_t source = r_u32(record + 12);
            uint32_t packed = r_u8(source + 2) | ((uint32_t)r_u8(source + 3) << 16);

            w_u32(record, r_u32(record + 16));
            w_u32(record + 8, packed);
            w_u32(record + 20, packed);
        }
        if (r_s8(record + 24) == 1)
            w_u32(record, r_u16(record + 16) | ((uint32_t)r_u16(record + 18) << 16));
        ++first;
        result = last < first;
        record += 36;
    }
    return result;
}

uint32_t sub_800C5558(uint32_t player_index)
{
    uint32_t players = r_u32(0x8005B470u);
    uint32_t selected = r_u8(players + 5);
    uint32_t player = players + 112 * selected;
    uint32_t area = 0x800D63E8u + 12 * player_index;
    int32_t x = (int16_t)(r_u16(area) + r_u16(player + 24));
    int32_t y = (int16_t)(r_u16(area + 2) + r_u16(player + 26));

    FUNCTION_MARKER(0x800C5558, "RASHCDG.BIN");
    return sub_8001C304(x, y, (int16_t)r_u16(area + 4), (int16_t)r_u16(area + 6), r_u32(0x8005B590u + 4 * player_index));
}

uint32_t sub_8005F834(uint32_t packets, uint32_t state, uint32_t reverse)
{
    int32_t mode = ((int32_t)(uint32_t)(r_u32(state)));
    int32_t progress = ((int32_t)(uint32_t)(r_u32(state + 4)));
    int32_t step;
    uint32_t result = 3;

    FUNCTION_MARKER(0x8005F834, "RASHCDG.BIN");
    if (mode == 0)
    {
        progress = reverse ? 0 : progress + 1;
        w_u32(state + 4, (uint32_t)progress);
        if (progress != ((int32_t)(uint32_t)(r_u32(state + 8))))
            return 1;
        w_u32(state, 1);
        w_u32(state + 4, 0);
        return 1;
    }
    if (mode == 1)
    {
        if (reverse)
        {
            w_u32(state, 3);
            return 3;
        }
        step = ((int32_t)(uint32_t)(r_u32(state + 20))) - progress;
        if (((int32_t)(uint32_t)(r_u32(state + 16))) < step)
            step = ((int32_t)(uint32_t)(r_u32(state + 16)));
        (void)sub_8005F9B0(packets, (int16_t)((uint32_t)step * r_u32(state + 24)), ((int32_t)(uint32_t)(r_u32(state + 28))), ((int32_t)(uint32_t)(r_u32(state + 32))));
        progress += step;
        w_u32(state + 4, (uint32_t)progress);
        if (progress != ((int32_t)(uint32_t)(r_u32(state + 20))))
            return (uint32_t)progress;
        w_u32(state, 2);
        w_u32(state + 4, 0);
        return 2;
    }
    if (mode == 2)
    {
        if (reverse)
        {
            w_u32(state, 3);
            w_u32(state + 4, r_u32(state + 20));
        }
        return 3;
    }
    if (mode == 3)
    {
        step = progress;
        if (((int32_t)(uint32_t)(r_u32(state + 12))) < step)
            step = ((int32_t)(uint32_t)(r_u32(state + 12)));
        (void)sub_8005F9B0(packets, (int16_t)(0u - (uint32_t)step * r_u32(state + 24)), ((int32_t)(uint32_t)(r_u32(state + 28))), ((int32_t)(uint32_t)(r_u32(state + 32))));
        progress -= step;
        w_u32(state + 4, (uint32_t)progress);
        result = (uint32_t)progress;
        if (!progress)
        {
            w_u32(state, 0);
            w_u32(state + 4, 0);
        }
    }
    return result;
}

uint32_t sub_80063408(uint32_t actor, uint32_t packets, uint32_t animation, uint32_t player_index, uint32_t hide_second)
{
    uint32_t game_state = r_u32(0x8005B2F8u);
    uint32_t player_flag = 0x8005AD18u + 4 * player_index;
    uint32_t hidden;

    FUNCTION_MARKER(0x80063408, "RASHCDG.BIN");
    if (r_s8(game_state) != 1 || (r_u8(r_u32(actor + 1084)) & 0x40u))
        return r_u8(game_state);
    hidden = r_u32(player_flag) || r_u32(0x8005B220u) || r_u32(0x8005B288u + 4 * player_index);
    (void)sub_8005F834(packets, animation, hidden);
    hidden = hide_second || r_u32(0x8005B220u) || r_u32(0x8005B288u + 4 * player_index);
    (void)sub_8005F834(packets, animation + 36, hidden);
    w_u32(player_flag, 0);
    return player_flag;
}

static uint32_t frontier_emit_flat_quad(const uint16_t vertices[8], uint32_t color, uint32_t mode, uint32_t player_index)
{
    uint32_t context = r_u32(0x8005B470u);
    uint32_t packet = r_u32(context + 268);
    uint32_t ordering_slot = r_u32(0x8005B590u + 4 * player_index);
    uint32_t index;
    static const uint8_t order[8] = {0, 1, 2, 3, 6, 7, 4, 5};

    if (packet + 24 >= r_u32(0x8005B4D0u))
        packet = sub_80021C98(packet, 24);
    w_u32(packet, r_u32(ordering_slot) | 0x05000000u);
    w_u32(packet + 4, ((mode | 0x29u) << 24) | ((uint32_t)r_u8(color + 2) << 16) | ((uint32_t)r_u8(color + 1) << 8) | r_u8(color));
    w_u32(ordering_slot, packet);
    for (index = 0; index < 8; ++index)
        w_u16(packet + 8 + 2 * index, vertices[order[index]]);
    context = r_u32(0x8005B470u);
    w_u32(context + 268, packet + 24);
    return context;
}

uint32_t sub_800C5168(uint32_t vertices, uint32_t color, uint32_t mode, uint32_t player_index)
{
    uint16_t values[8];
    uint32_t index;

    FUNCTION_MARKER(0x800C5168, "RASHCDG.BIN");
    for (index = 0; index < 8; ++index)
        values[index] = r_u16(vertices + 2 * index);
    return frontier_emit_flat_quad(values, color, mode, player_index);
}

uint32_t sub_8001FE80(int32_t angle, uint32_t sine_output, uint32_t cosine_output)
{
    uint32_t packed = r_u32(0x8005624Cu + 4 * ((uint32_t)angle & 0xFFFu));
    int32_t sine = (int16_t)packed * 16;
    int32_t cosine = (int16_t)(packed >> 16) * 16;

    FUNCTION_MARKER(0x8001FE80, "SLUS_010.53");
    w_u32(sine_output, (uint32_t)sine);
    w_u32(cosine_output, (uint32_t)cosine);
    return (uint32_t)cosine;
}

uint32_t sub_8002DEC8(uint32_t vector, int32_t scale)
{
    uint32_t first;
    uint32_t second;

    FUNCTION_MARKER(0x8002DEC8, "SLUS_010.53");
    first = (uint32_t)sub_8001FC90(scale, ((int32_t)(uint32_t)(r_u32(vector))));
    second = (uint32_t)sub_8001FC90(scale, ((int32_t)(uint32_t)(r_u32(vector + 4))));
    w_u32(vector, first);
    w_u32(vector + 4, second);
    return second;
}

uint32_t sub_8002DE40(int32_t angle, uint32_t vector)
{
    uint32_t packed = r_u32(0x8005624Cu + 4 * ((uint32_t)angle & 0xFFFu));
    int32_t sine = (int16_t)packed * 16;
    int32_t cosine = (int16_t)(packed >> 16) * 16;
    int32_t x = ((int32_t)(uint32_t)(r_u32(vector)));
    int32_t y = ((int32_t)(uint32_t)(r_u32(vector + 4)));
    uint32_t result;

    FUNCTION_MARKER(0x8002DE40, "SLUS_010.53");
    w_u32(vector, (uint32_t)sub_8001FC90(cosine, x) - (uint32_t)sub_8001FC90(sine, y));
    result = (uint32_t)sub_8001FC90(cosine, y);
    w_u32(vector + 4, (uint32_t)sub_8001FC90(sine, x) + result);
    return result;
}

static uint32_t frontier_minimap_marker(int32_t position_x, int32_t position_z, int32_t angle, uint32_t player_index, uint32_t marker_type)
{
    int32_t x = (int32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(0x8005B234u))), position_x);
    int32_t y = (int32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(0x8005B234u))), position_z);
    uint32_t packed = r_u32(0x8005624Cu + 4 * ((uint32_t)angle & 0xFFFu));
    int32_t sine = (int16_t)packed * 16;
    int32_t cosine = (int16_t)(packed >> 16) * 16;
    int32_t rotated_x = (int32_t)((uint32_t)sub_8001FC90(cosine, x) - (uint32_t)sub_8001FC90(sine, y));
    int32_t rotated_y = (int32_t)((uint32_t)sub_8001FC90(sine, x) + (uint32_t)sub_8001FC90(cosine, y));
    uint32_t area = 0x800D63E8u + 12 * player_index;
    int32_t center_x = (int16_t)(r_u16(area + 8) + (uint32_t)(rotated_x >> 16));
    int32_t center_y = (int16_t)(r_u16(area + 10) - (uint32_t)(rotated_y >> 16));
    int16_t vertices[8];
    int32_t half_width;
    int32_t half_height;

    if (center_x < (int16_t)r_u16(area) || center_x > (int16_t)r_u16(area) + (int16_t)r_u16(area + 4) - 1 || center_y < (int16_t)r_u16(area + 2) || center_y > (int16_t)r_u16(area + 2) + (int16_t)r_u16(area + 6) - 1)
        return 1;
    half_width = marker_type ? (marker_type == 5 ? 2 : 1) : 3;
    half_height = marker_type ? (marker_type == 5 ? 3 : 2) : 3;
    vertices[0] = (int16_t)(marker_type ? center_x - half_width : center_x);
    vertices[1] = (int16_t)(center_y - half_height);
    vertices[2] = (int16_t)(marker_type ? center_x + half_width : center_x);
    vertices[3] = (int16_t)(center_y - half_height);
    vertices[4] = (int16_t)(center_x + half_width);
    vertices[5] = (int16_t)(center_y + half_height);
    vertices[6] = (int16_t)(center_x - half_width);
    vertices[7] = (int16_t)(center_y + half_height);
    return frontier_emit_flat_quad((const uint16_t *)vertices, 0x800CCBE0u + 4 * marker_type, 0, player_index);
}

uint32_t sub_800C5380(uint32_t position, int32_t angle, uint32_t player_index, uint32_t marker_type)
{
    FUNCTION_MARKER(0x800C5380, "RASHCDG.BIN");
    return frontier_minimap_marker(((int32_t)(uint32_t)(r_u32(position))), ((int32_t)(uint32_t)(r_u32(position + 8))), angle, player_index, marker_type);
}

uint32_t sub_80013E64(uint32_t state)
{
    uint32_t game_state = r_u32(0x8005B2F8u);
    uint32_t interval = r_u32(state + (r_u32(state + 16) ? 4 : 8));
    uint32_t current = r_u32(game_state + 16);
    uint32_t anchor = r_u32(state + 20);
    int32_t delta = anchor ? ((int32_t)(uint32_t)(current - anchor)) : 0;
    uint32_t in_or_after = 1;
    uint32_t outside = 0;
    uint32_t mode = r_u8(game_state);

    FUNCTION_MARKER(0x80013E64, "SLUS_010.53");
    if ((mode - 3u) < 2u)
        return 1;
    if (delta)
    {
        int32_t start = ((int32_t)(uint32_t)(r_u32(state + 24)));
        uint32_t reaches = 0;

        in_or_after = start >= delta;
        if (start < delta)
            reaches = ((int32_t)(uint32_t)((uint32_t)start + r_u32(state + 28))) >= delta;
        if (!in_or_after)
            outside = !reaches;
    }
    if (in_or_after && ((int32_t)(uint32_t)(r_u32(state) + interval)) < ((int32_t)(uint32_t)(current)))
    {
        int32_t count = ((int32_t)(uint32_t)(r_u32(state + 12))) + 1;

        w_u32(state + 16, r_u32(state + 16) ^ 1u);
        w_u32(state + 12, (uint32_t)count);
        w_u32(state, current);
        if (count >= 17)
        {
            w_u32(state + 12, (uint32_t)-60);
            return (uint32_t)-60;
        }
        return 1;
    }
    if (anchor)
    {
        uint32_t result = !outside;

        if (!in_or_after)
        {
            w_u32(state + 16, result);
            if (outside)
                w_u32(state + 20, 0);
        }
        return result;
    }
    return 0;
}

uint32_t sub_80013AF8(uint32_t packet, uint32_t template_data)
{
    int32_t state = r_s8(packet + 32);
    uint32_t packed;

    FUNCTION_MARKER(0x80013AF8, "SLUS_010.53");
    if (state == 1)
    {
        w_u16(packet + 28, r_u8(template_data + 2));
        w_u16(packet + 30, r_u8(template_data + 3));
        return r_u8(template_data + 3);
    }
    if (state != 0)
        return 1;
    w_u32(packet + 20, template_data);
    w_u16(packet + 28, r_u8(template_data + 2));
    w_u16(packet + 30, r_u8(template_data + 3));
    packed = (uint32_t)r_u8(template_data + 2) | ((uint32_t)r_u8(template_data + 3) << 16);
    w_u32(packet + 8, r_u32(packet + 24));
    w_u32(packet + 16, packed);
    w_u32(packet + 28, packed);
    return packed;
}

uint32_t sub_800C569C(uint32_t subject, uint32_t type_filter, int32_t angle, uint32_t player_index)
{
    uint32_t actor = r_u32(0x800CE4D0u);
    int32_t remaining = ((int32_t)(uint32_t)(r_u32(r_u32(0x800CE4DCu))));
    uint32_t marker_state = 0x800D6198u + 224 * player_index;

    FUNCTION_MARKER(0x800C569C, "RASHCDG.BIN");
    while (remaining >= 0)
    {
        uint32_t owner = r_u16(actor + 172);

        if (r_u16(actor + 320) && owner != player_index)
        {
            uint32_t body = r_u32(actor + 852);
            uint32_t source = r_u32(body + 604) < 3 ? actor : body;
            int32_t relative_x = ((int32_t)(uint32_t)(r_u32(source + 184) - r_u32(subject + 184)));
            int32_t relative_z = ((int32_t)(uint32_t)(r_u32(source + 192) - r_u32(subject + 192)));
            uint32_t game_state = r_u32(0x8005B2F8u);
            uint32_t marker_type = 0;
            uint32_t flags = r_u8(game_state + 4);
            int draw = 1;

            if (owner >= r_u32(game_state + 48) || player_index != owner)
            {
                uint32_t type = r_u8(r_u32(actor + 1084) + 1) & 0xFu;

                if (type == (type_filter & 0xFu))
                    marker_type = 2;
                else if (type == 2)
                    marker_type = 4;
                else
                    marker_type = flags == 44 ? 4 : 3;
            }
            if ((flags & 0x10u) && owner == (player_index ^ 1u))
                draw = 0;
            else if ((flags & 1u) && owner != player_index && owner == r_u8(game_state + 6))
                draw = 0;
            if (!draw)
            {
                (void)sub_80013E64(marker_state + 64);
                marker_type = 5;
                draw = r_u32(marker_state + 80) != 0;
            }
            if (draw)
                (void)frontier_minimap_marker(relative_x, relative_z, angle, player_index, marker_type);
        }
        --remaining;
        actor += r_u32(0x800CE4D4u);
    }
    return frontier_minimap_marker(0, 0, angle, player_index, 0);
}

uint32_t sub_800C52A8(uint32_t actor, uint32_t player_index, uint32_t inhibited)
{
    uint32_t body;
    uint32_t subject;
    uint32_t active;
    int32_t angle;

    FUNCTION_MARKER(0x800C52A8, "RASHCDG.BIN");
    if (inhibited)
        return inhibited;
    inhibited = r_u32(0x8005B288u + 4 * player_index);
    if (inhibited)
        return inhibited;
    body = r_u32(actor + 852);
    active = r_u32(body + 604) < 3;
    subject = active ? actor : body;
    if (!subject)
        return active;
    active = r_u16(subject + 320);
    if (!active)
        return active;
    (void)sub_800C5618(player_index);
    angle = ((int32_t)(uint32_t)(sub_80020018((uint32_t)((int32_t)r_s16(subject + 444) * 16), (uint32_t)((int32_t)r_s16(subject + 448) * 16))));
    if (angle < 0)
        angle += 4096;
    (void)sub_800C569C(subject, r_u8(r_u32(actor + 1084) + 1), angle, player_index);
    return sub_800C5558(player_index);
}

uint32_t sub_800C52A0(uint32_t actor, uint32_t player_index)
{
    FUNCTION_MARKER(0x800C52A0, "RASHCDG.BIN");
    return sub_800C52A8(actor, player_index, r_u32(0x8005B220u));
}

uint32_t sub_80061E50(uint32_t state, uint32_t player_index, uint32_t configuration)
{
    uint32_t ordering_slot = r_u32(0x8005B590u + 4 * player_index);
    uint32_t selected;
    uint32_t other;

    FUNCTION_MARKER(0x80061E50, "RASHCDG.BIN");
    if (r_u32(configuration) != 2)
    {
        selected = r_u32(0x8005B38Cu) + 1096 * player_index;
        sub_8002CD78(r_u32(0x8005AD2Cu), r_u8(r_u32(selected + 1084) + 38), (uint32_t)(int32_t)r_s16(state + 636), (uint32_t)(int32_t)r_s16(state + 638), ordering_slot, 0x00808080u);
    }
    other = r_u32(0x8005AD0Cu + 4 * player_index);
    if (other && r_u32(configuration + 36) != 2)
    {
        sub_8002CD78(r_u32(0x8005AD2Cu), r_u8(r_u32(other + 1084) + 38), (uint32_t)(int32_t)r_s16(state + 1140), (uint32_t)(int32_t)r_s16(state + 1142), ordering_slot, 0x00808080u);
    }
    return other;
}

static uint32_t frontier_link_race_packet(uint32_t ordering_slot, uint32_t packet)
{
    int32_t type = r_s8(packet + 32);
    uint32_t tag = (r_u32(0x800CC68Cu + 4 * (uint32_t)type) << 24) | r_u32(ordering_slot);

    w_u32(packet, tag);
    w_u32(ordering_slot, packet & 0xFFFFFFu);
    return tag;
}

static void frontier_initialize_template_packet(uint32_t packet, uint32_t entry)
{
    uint32_t packed;

    if (r_s8(packet + 32) != 0)
        return;
    packed = (uint32_t)r_u8(entry + 2) | ((uint32_t)r_u8(entry + 3) << 16);
    w_u32(packet + 12, r_u32(entry + 4));
    w_u32(packet + 8, r_u32(packet + 24));
    w_u32(packet + 16, packed);
    w_u32(packet + 28, packed);
}

static void frontier_initialize_digit_packet(uint32_t packet, uint32_t digit)
{
    frontier_initialize_template_packet(packet, 0x800D4620u + 16 * digit);
}

uint32_t sub_80013B90(int32_t value, uint32_t packets, uint32_t table)
{
    int32_t first;
    int32_t second;
    int32_t third;
    int32_t fourth;
    int32_t fifth;
    int32_t remainder;

    FUNCTION_MARKER(0x80013B90, "SLUS_010.53");
    if (value > 0x176F0000)
        return 0;
    first = (value / 600) >> 8;
    remainder = value - 153600 * first;
    second = (remainder / 60) >> 8;
    remainder -= 15360 * second;
    third = (remainder / 10) >> 8;
    remainder -= 2560 * third;
    fourth = remainder >> 8;
    fifth = (10 * (remainder - (fourth << 8))) >> 8;
    if (fifth >= 10)
        fifth = 0;
    frontier_initialize_template_packet(packets, table + 16 * (uint32_t)first);
    frontier_initialize_template_packet(packets + 36, table + 16 * (uint32_t)second);
    frontier_initialize_template_packet(packets + 72, table + 16 * (uint32_t)third);
    frontier_initialize_template_packet(packets + 108, table + 16 * (uint32_t)fourth);
    frontier_initialize_template_packet(packets + 144, table + 16 * (uint32_t)fifth);
    return (uint32_t)fourth & 1u;
}

uint32_t sub_8005FE58(uint32_t ordering_slot, uint32_t packets, uint32_t state, uint32_t actor)
{
    uint32_t result;

    FUNCTION_MARKER(0x8005FE58, "RASHCDG.BIN");
    if (r_u32(actor + 180) < 18)
    {
        result = r_u32(state + 140);
        if (((int32_t)(uint32_t)(result)) <= 0)
        {
            w_u32(state + 140, 0);
            return result;
        }
        (void)sub_80013E64(state + 128);
        result = r_u32(state + 144);
        if (result)
            result = frontier_link_race_packet(ordering_slot, packets + 144);
        return result;
    }
    result = ((int32_t)(uint32_t)(r_u32(state + 108))) < 3 ? r_u32(state + 112) : 1;
    if (!result)
        return 0x80060000u;
    result = r_u32(0x8005AD48u) < 2;
    if (result)
        return result;
    result = r_u32(0x8005AD48u) < 9;
    if (result)
        return frontier_link_race_packet(ordering_slot, packets + 36);
    return result;
}

uint32_t sub_80016528(void)
{
    uint32_t result;

    FUNCTION_MARKER(0x80016528, "SLUS_010.53");
    result = sub_8001F7EC(r_u32(0x8005B438u));
    w_u32(0x8005B438u, 0);
    return result;
}

uint32_t sub_8005FF84(uint32_t ordering_slot, uint32_t packets, uint32_t state, uint32_t actor)
{
    uint32_t packet = packets + 1872;
    uint32_t descriptor = r_u32(actor + 1084);

    FUNCTION_MARKER(0x8005FF84, "RASHCDG.BIN");
    if ((r_u32(descriptor) & 0x60u) == 0x40u)
    {
        int32_t value = ((int32_t)(uint32_t)(r_u32(0x8005B230u)));
        int32_t high = value >> 16;
        int32_t excess = 2 - high;
        int32_t variant = high + (value < 0 ? -high : 0) + 1;
        uint32_t entry;

        if (excess < 0)
            variant += excess;
        entry = 0x800D4960u + 16 * (uint32_t)variant;
        if (r_s8(packet + 32) == 0)
        {
            uint32_t template_word = r_u32(entry + 4);
            uint32_t coordinate = r_u32(packet + 24);
            uint32_t u, v, packed;
            int32_t packet_type;
            w_u32(packet + 12, template_word);
            u = r_u8(entry + 2);
            v = r_u8(entry + 3);
            packet_type = r_s8(packet + 32);
            w_u32(packet + 8, coordinate);
            packed = u | (v << 16);
            w_u32(packet + (packet_type ? 12u : 16u), packed);
            w_u32(packet + 28, packed);
        }
        if (r_u32(state + 84) != (uint32_t)variant)
        {
            (void)sub_80017BA0(0, 0, 76, 0);
            (void)sub_80017BA0(0, 0, 75, 0);
            (void)sub_80017BA0(0, 0, 29, 0);
        }
        w_u32(state + 84, (uint32_t)variant);
        return frontier_link_race_packet(ordering_slot, packet);
    }
    uint32_t result = 0x80060000u;
    if (r_u32(state + 84))
    {
        if ((r_u8(r_u32(0x8005B2F8u) + 4) & 1u) == 0)
        {
            (void)sub_80017BA0(0, 0, 65, 0);
            (void)sub_80017BA0(0, 0, 106, 0);
            (void)sub_80017BA0(0, 0, 80, 0);
        }
        result = sub_80016528();
        w_u32(state + 84, 0);
    }
    return result;
}

uint32_t sub_80060178(uint32_t ordering_slot, uint32_t packets, uint32_t state, uint32_t actor)
{
    uint32_t body = r_u32(actor + 852);
    int32_t value = (r_u32(body + 604) - 3u) < 2u ? 0 : ((int32_t)(uint32_t)(r_u32(actor + 480))) >> 8;
    uint32_t first_packet = packets + 360;
    uint32_t last_packet = packets + 432;
    uint32_t result;

    FUNCTION_MARKER(0x80060178, "RASHCDG.BIN");
    if (r_u32(state + 8) != (uint32_t)value)
    {
        int32_t scaled = (int32_t)sub_8001FC90(value, 146599);
        uint32_t first;
        uint32_t second;
        uint32_t third;

        w_u32(state + 8, (uint32_t)value);
        if (scaled < 0)
            scaled = 0;
        first = (uint32_t)scaled / 25600u;
        scaled -= (int32_t)(25600u * first);
        second = (uint32_t)scaled / 2560u;
        third = ((uint32_t)scaled - 2560u * second) >> 8;
        frontier_initialize_digit_packet(first_packet, first);
        frontier_initialize_digit_packet(packets + 396, second);
        frontier_initialize_digit_packet(last_packet, third);
    }
    result = (r_u32(0x800CC68Cu + 4 * (uint32_t)r_s8(last_packet + 32)) << 24) | r_u32(ordering_slot);
    w_u32(last_packet, result);
    result = first_packet & 0xFFFFFFu;
    w_u32(ordering_slot, result);
    return result;
}

uint32_t sub_800603E4(uint32_t ordering_slot, uint32_t packets, uint32_t state, uint32_t player_index)
{
    uint32_t first_packet = packets + 468;
    uint32_t last_packet = packets + 576;
    uint32_t result;

    FUNCTION_MARKER(0x800603E4, "RASHCDG.BIN");
    if ((r_u32(0x8005ACDCu) & 7u) == 7u)
    {
        int32_t value = (int32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(0x8005B380u + 4 * player_index))), 40);

        if (r_u32(state + 12) != (uint32_t)value)
        {
            uint32_t first;
            uint32_t second;
            uint32_t third;
            int32_t remainder;

            w_u32(state + 12, (uint32_t)value);
            if (value >= 25600)
                value %= 25600;
            first = (uint32_t)(value / 10) >> 8;
            remainder = value - (int32_t)(2560u * first);
            second = (uint32_t)(remainder >> 8);
            third = (uint32_t)((10 * (remainder - ((int32_t)second << 8))) >> 8);
            if (first >= 10)
                first = 0;
            if (second >= 10)
                second = 0;
            if (third >= 10)
                third = 0;
            frontier_initialize_template_packet(first_packet, 0x800D4710u + 16 * first);
            frontier_initialize_template_packet(packets + 504, 0x800D4710u + 16 * second);
            frontier_initialize_template_packet(packets + 540, 0x800D4920u);
            frontier_initialize_template_packet(last_packet, 0x800D4710u + 16 * third);
        }
    }
    result = (r_u32(0x800CC68Cu + 4 * (uint32_t)r_s8(last_packet + 32)) << 24) | r_u32(ordering_slot);
    w_u32(last_packet, result);
    result = first_packet & 0xFFFFFFu;
    w_u32(ordering_slot, result);
    return result;
}

uint32_t sub_800606F8(uint32_t ordering_slot, uint32_t packets, uint32_t state, uint32_t actor, uint32_t flags, uint32_t player_index, uint32_t blink_state, uint32_t game_state)
{
    uint32_t descriptor;
    uint32_t rank;
    uint32_t result = 0x80060000u;

    FUNCTION_MARKER(0x800606F8, "RASHCDG.BIN");
    if (r_u8(game_state + 4) & 4u)
        return result;
    if (r_u32(0x8005B220u))
        return result;
    descriptor = r_u32(actor + 1084);
    if (r_u32(0x8005B288u + 4 * player_index) && r_u8(descriptor + 39) != 0xFFu)
        return result;
    rank = r_u32(state);
    if (r_u32(0x8005ACDCu) & 1u)
    {
        uint32_t nearest;
        uint32_t distance = sub_8003A5F4(actor + 360, &nearest, 0);
        uint32_t next_rank = rank;

        if (r_u16(actor + 362) == 0 && distance > 0xA0000u)
            next_rank = sub_800138E8(actor, 0);
        if (next_rank != rank && ((int32_t)(uint32_t)(next_rank)) < 20)
        {
            uint32_t digit = next_rank;
            uint32_t tens = 0;
            uint32_t other = r_u32(0x8005B1FCu);
            uint32_t other_tens = 0;

            w_u32(state, next_rank);
            rank = next_rank;
            if (digit >= 10)
            {
                digit -= 10;
                tens = 1;
            }
            if (tens)
                frontier_initialize_template_packet(packets + 1620, 0x800D4620u + 16 * tens);
            frontier_initialize_template_packet(packets + 1656, 0x800D4620u + 16 * digit);
            frontier_initialize_template_packet(packets + 1692, 0x800D4910u);
            if (other >= 10)
            {
                other -= 10;
                other_tens = 1;
            }
            if (other_tens)
                frontier_initialize_template_packet(packets + 1728, 0x800D4710u + 16 * other_tens);
            frontier_initialize_template_packet(packets + 1764, 0x800D4710u + 16 * other);
        }
    }
    {
        uint32_t linked = r_u32(actor + 428);
        uint32_t low_speed = (int32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(actor + 324))), 40) < 4096;
        uint32_t enabled = linked ? (r_u16(linked + 118) >> (r_u16(actor + 172) & 31u)) & 1u : 0;

        if (low_speed && enabled)
        {
            if (!r_u32(0x8005B220u) && !(flags & 2u) && !r_u32(0x8005B288u + 4 * player_index))
                (void)sub_80013E64(blink_state + 64);
        }
        if (!(low_speed && enabled) || r_u32(blink_state + 80))
        {
            uint32_t packet = packets + 1656;
            uint32_t first = rank < 10 ? packet : packets + 1620;
            int32_t type = r_s8(packet + 32);

            w_u32(packet, (r_u32(0x800CC68Cu + 4 * (uint32_t)type) << 24) | r_u32(ordering_slot));
            w_u32(ordering_slot, first & 0xFFFFFFu);
        }
    }
    {
        uint32_t packet = packets + 1764;
        uint32_t first = r_u32(0x8005B1FCu) < 10 ? packet : packets + 1728;
        int32_t type = r_s8(packet + 32);

        w_u32(packet, (r_u32(0x800CC68Cu + 4 * (uint32_t)type) << 24) | r_u32(ordering_slot));
        w_u32(ordering_slot, first & 0xFFFFFFu);
        result = frontier_link_race_packet(ordering_slot, packets + 1692);
    }
    return result;
}

uint32_t sub_800606F0(uint32_t ordering_slot, uint32_t packets, uint32_t state, uint32_t actor, uint32_t flags, uint32_t player_index, uint32_t blink_state)
{
    FUNCTION_MARKER(0x800606F0, "RASHCDG.BIN");
    return sub_800606F8(ordering_slot, packets, state, actor, flags, player_index, blink_state, r_u32(0x8005B2F8u));
}

static int32_t frontier_clamp_meter(int32_t value)
{
    if (value < 2)
        return 2;
    if (value > 127)
        return 127;
    return value;
}

static void frontier_update_meter_packet(uint32_t packet, int32_t value, uint32_t blink_state, uint32_t game_state)
{
    int32_t phase;
    uint32_t width;
    uint32_t color;

    if (value < 33)
    {
        uint32_t mode = r_u8(game_state);

        if ((mode - 3u) >= 2u)
            (void)sub_80013E64(blink_state + 192);
        phase = 1 - ((int32_t)(uint32_t)(r_u32(blink_state + 208)));
    }
    else if (value < 65)
    {
        phase = 1;
    }
    else
    {
        phase = value < 97 ? 2 : 3;
    }
    width = phase ? (uint32_t)(value >> 2) : 0;
    if (phase && width < 4)
        width = 4;
    w_u16(packet + 28, (uint16_t)width);
    if (phase > 0)
        --phase;
    color = 0x800CC698u + 4 * (uint32_t)phase;
    w_u32(packet + 4, ((uint32_t)(int32_t)r_s16(packet + 34) << 25) | ((uint32_t)r_u8(color + 2) << 16) | ((uint32_t)r_u8(color + 1) << 8) | r_u8(color) | 0x60000000u);
    w_u32(packet + 8, r_u16(packet + 24) | ((uint32_t)r_u16(packet + 26) << 16));
    w_u32(packet + 12, r_u16(packet + 28) | ((uint32_t)r_u16(packet + 30) << 16));
}

static uint32_t frontier_link_packet_range(uint32_t ordering_slot, uint32_t tag_packet, uint32_t first_packet)
{
    int32_t type = r_s8(tag_packet + 32);
    uint32_t tag = (r_u32(0x800CC68Cu + 4 * (uint32_t)type) << 24) | r_u32(ordering_slot);

    w_u32(tag_packet, tag);
    w_u32(ordering_slot, first_packet & 0xFFFFFFu);
    return tag;
}

uint32_t sub_80060C18(uint32_t ordering_slot, uint32_t packets, uint32_t state, uint32_t actor, uint32_t player_index, uint32_t configuration, uint32_t blink_state, uint32_t display_flags)
{
    uint32_t descriptor = r_u32(actor + 1084);
    uint32_t game_state = r_u32(0x8005B2F8u);
    uint32_t other = r_u32(actor + 856);
    int32_t primary = 0;
    int32_t secondary = 0;
    uint32_t result = 2;

    FUNCTION_MARKER(0x80060C18, "RASHCDG.BIN");
    if (display_flags & 1u)
    {
        uint32_t glyph = r_u8(descriptor + 46);

        if (glyph != r_u32(state + 20))
        {
            w_u32(state + 20, glyph);
            frontier_initialize_template_packet(packets + 864, 0x800D4860u + 16 * glyph);
            w_u32(0x8005AD18u + 4 * player_index, 1);
        }
        primary = ((int32_t)r_u8(descriptor + 15) << 7) / ((int32_t)r_u8(descriptor + 13) + 1);
        secondary = ((int32_t)r_u8(descriptor + 37) << 7) / ((int32_t)r_u8(descriptor + 36) + 1);
        if (r_u8(descriptor + 15))
            primary = frontier_clamp_meter(primary);
        if (primary != ((int32_t)(uint32_t)(r_u32(state + 24))))
        {
            w_u32(state + 24, (uint32_t)primary);
            w_u32(0x8005AD18u + 4 * player_index, 1);
        }
        if (r_u8(descriptor + 37))
            secondary = frontier_clamp_meter(secondary);
        else
            secondary = 2 * (r_u32(r_u32(actor + 852) + 604) < 3);
        if (secondary != ((int32_t)(uint32_t)(r_u32(state + 28))))
        {
            w_u32(state + 28, (uint32_t)secondary);
            w_u32(0x8005AD18u + 4 * player_index, 1);
        }
        if ((r_u8(game_state + 4) & 8u) && other && r_u32(actor + 1088) && r_u8(game_state + player_index + 10))
        {
            uint32_t other_descriptor = r_u32(other + 1084);
            int32_t other_meter = ((int32_t)r_u8(other_descriptor + 15) << 7) / ((int32_t)r_u8(other_descriptor + 13) + 1);

            if (r_u8(other_descriptor + 15))
                other_meter = frontier_clamp_meter(other_meter);
            if (other_meter != ((int32_t)(uint32_t)(r_u32(state + 36))))
            {
                w_u32(state + 36, (uint32_t)other_meter);
                w_u32(0x8005AD18u + 4 * player_index, 1);
                frontier_update_meter_packet(packets + 1008, other_meter, blink_state, game_state);
            }
            glyph = r_u8(other_descriptor + 46);
            if (glyph != r_u32(state + 32))
            {
                w_u32(state + 32, glyph);
                frontier_initialize_template_packet(packets + 1044, 0x800D4860u + 16 * glyph);
                w_u32(0x8005AD18u + 4 * player_index, 1);
            }
        }
        frontier_update_meter_packet(packets + 720, primary, blink_state, game_state);
        frontier_update_meter_packet(packets + 828, secondary, blink_state, game_state);
    }
    if (r_u32(configuration) != 2)
    {
        result = frontier_link_packet_range(ordering_slot, packets + 864, packets + 648);
        if (r_u8(descriptor + 47) && r_u8(descriptor + 46) != 9)
            result = frontier_link_race_packet(ordering_slot, packets + 900);
        if (other && r_u32(actor + 1088) && r_u8(game_state + player_index + 10))
        {
            uint32_t other_descriptor = r_u32(other + 1084);

            result = frontier_link_packet_range(ordering_slot, packets + 1044, packets + 936);
            if (r_u8(other_descriptor + 47) && r_u8(other_descriptor + 46) != 9)
                result = frontier_link_race_packet(ordering_slot, packets + 1080);
        }
    }
    return result;
}

uint32_t sub_80060C10(uint32_t ordering_slot, uint32_t packets, uint32_t state, uint32_t actor, uint32_t player_index, uint32_t configuration, uint32_t blink_state)
{
    FUNCTION_MARKER(0x80060C10, "RASHCDG.BIN");
    return sub_80060C18(ordering_slot, packets, state, actor, player_index, configuration, blink_state, r_u32(0x8005ACDCu));
}

uint32_t sub_80061494(uint32_t ordering_slot, uint32_t packets, uint32_t state, uint32_t actor, uint32_t player_index, uint32_t configuration, uint32_t blink_state, uint32_t display_flags)
{
    uint32_t game_state = r_u32(0x8005B2F8u);
    uint32_t selected = 0;
    uint32_t changed = 0;

    FUNCTION_MARKER(0x80061494, "RASHCDG.BIN");
    if (display_flags & 1u)
    {
        int32_t relation = r_s8(actor + 946) - 1;
        uint32_t candidate_id = r_u16(actor + 958 + 8 * (uint32_t)relation);

        if (candidate_id < r_u32(0x8005B1F8u) && candidate_id != player_index)
        {
            selected = r_u32(0x8005B3A0u) + 1096 * candidate_id;
            changed = 1;
            w_u32(0x8005AD18u + 4 * player_index, 1);
        }
        else if (r_u8(game_state + 4) != 44 || r_u8(game_state + 57))
        {
            selected = sub_8008B84C(actor);
        }
        if (selected)
        {
            int32_t selected_relation = r_s8(selected + 946) - 1;
            uint32_t relation_id = r_u16(selected + 958 + 8 * (uint32_t)selected_relation);
            uint32_t descriptor = r_u32(selected + 1084);

            if (relation_id == r_u16(actor + 172))
            {
                changed = 1;
                w_u32(0x8005AD18u + 4 * player_index, 1);
            }
            if (r_u32(state + 60) != 5 || (r_u8(descriptor + 1) & 0xFu) == 2)
            {
                int32_t primary = ((int32_t)r_u8(descriptor + 15) << 7) / ((int32_t)r_u8(descriptor + 13) + 1);
                int32_t secondary = ((int32_t)r_u8(descriptor + 37) << 7) / ((int32_t)r_u8(descriptor + 36) + 1);
                uint32_t glyph = r_u8(descriptor + 46);

                w_u32(0x8005AD0Cu + 4 * player_index, selected);
                if (r_u8(descriptor + 15))
                    primary = frontier_clamp_meter(primary);
                if (r_u8(descriptor + 37))
                    secondary = frontier_clamp_meter(secondary);
                else
                    secondary = 2 * (r_u32(r_u32(selected + 852) + 604) < 3);
                if (selected != r_u32(state + 64))
                {
                    w_u32(state + 64, selected);
                    (void)sub_80013AF8(packets + 1116, 0x800D4850u);
                    frontier_initialize_template_packet(packets + 1116, 0x800D4850u);
                    changed = 1;
                }
                if (primary != ((int32_t)(uint32_t)(r_u32(state + 40))))
                {
                    w_u32(state + 40, (uint32_t)primary);
                    changed = 1;
                }
                if (secondary != ((int32_t)(uint32_t)(r_u32(state + 44))))
                {
                    w_u32(state + 44, (uint32_t)secondary);
                    changed = 1;
                }
                if (glyph != r_u32(state + 48))
                {
                    w_u32(state + 48, glyph);
                    frontier_initialize_template_packet(packets + 1368, 0x800D4860u + 16 * glyph);
                    changed = 1;
                }
            }
            else
            {
                w_u32(0x8005AD0Cu + 4 * player_index, 0);
            }
        }
        else
        {
            w_u32(0x8005AD0Cu + 4 * player_index, 0);
        }
    }
    selected = r_u32(state + 64);
    if (!selected)
        return changed;
    if (r_u32(configuration + 36) != 2)
    {
        uint32_t descriptor = r_u32(selected + 1084);
        uint32_t linked;

        frontier_update_meter_packet(packets + 1224, ((int32_t)(uint32_t)(r_u32(state + 40))), blink_state, game_state);
        frontier_update_meter_packet(packets + 1332, ((int32_t)(uint32_t)(r_u32(state + 44))), blink_state, game_state);
        linked = r_u32(selected + 856);
        if (linked && r_u32(selected + 1088) && r_u8(game_state + r_u16(selected + 172) + 10))
        {
            uint32_t linked_descriptor = r_u32(linked + 1084);
            int32_t linked_meter = ((int32_t)r_u8(linked_descriptor + 15) << 7) / ((int32_t)r_u8(linked_descriptor + 13) + 1);
            uint32_t linked_glyph = r_u8(linked_descriptor + 46);

            if (r_u8(linked_descriptor + 15))
                linked_meter = frontier_clamp_meter(linked_meter);
            if (linked_meter != ((int32_t)(uint32_t)(r_u32(state + 56))))
            {
                w_u32(state + 56, (uint32_t)linked_meter);
                frontier_update_meter_packet(packets + 1512, linked_meter, blink_state, game_state);
                changed = 1;
            }
            if (linked_glyph != r_u32(state + 52))
            {
                w_u32(state + 52, linked_glyph);
                frontier_initialize_template_packet(packets + 1548, 0x800D4860u + 16 * linked_glyph);
                changed = 1;
            }
            (void)frontier_link_packet_range(ordering_slot, packets + 1548, packets + 1440);
            if (r_u8(linked_descriptor + 47) && linked_glyph != 9)
                (void)frontier_link_race_packet(ordering_slot, packets + 1584);
        }
        (void)frontier_link_packet_range(ordering_slot, packets + 1368, packets + 1152);
        if (r_u8(descriptor + 47) && r_u8(descriptor + 46) != 9)
            (void)frontier_link_race_packet(ordering_slot, packets + 1404);
    }
    return changed;
}

uint32_t sub_8006148C(uint32_t ordering_slot, uint32_t packets, uint32_t state, uint32_t actor, uint32_t player_index, uint32_t configuration, uint32_t blink_state)
{
    FUNCTION_MARKER(0x8006148C, "RASHCDG.BIN");
    return sub_80061494(ordering_slot, packets, state, actor, player_index, configuration, blink_state, r_u32(0x8005ACDCu));
}

uint32_t sub_80061F6C(uint32_t ordering_slot, uint32_t packets, uint32_t state, uint32_t actor, uint32_t player_index)
{
    uint32_t game_state = r_u32(0x8005B2F8u);
    uint32_t flags = r_u8(game_state + 4);
    uint32_t target;

    FUNCTION_MARKER(0x80061F6C, "RASHCDG.BIN");
    if (flags & 0x10u)
        target = r_u32(0x8005B3A0u) + 1096 * (player_index ^ 1u);
    else if (flags & 1u)
        target = r_u32(0x8005B3A0u) + 1096 * r_u8(game_state + 6);
    else
        target = r_u32(0x8005AD0Cu + 4 * player_index);
    if (target)
    {
        uint32_t difference = (r_u32(target + 324) - r_u32(actor + 324)) << 4;
        uint32_t arrow;
        int32_t distance;

        if (sub_80095410(actor) && ((int32_t)(uint32_t)(r_u32(actor + 364))) > 0)
            arrow = ((int32_t)(uint32_t)(difference)) > 0 ? 31 : 32;
        else
            arrow = ((int32_t)(uint32_t)(difference)) > 0 ? 32 : 31;
        if (((int32_t)(uint32_t)(difference)) < 0)
            difference = 0u - difference;
        distance = (int32_t)sub_8001FC90((int32_t)difference, 40);
        if (distance <= 0x9FFFF && r_u32(state + 16) != (uint32_t)distance)
        {
            uint32_t whole = (uint32_t)distance >> 16;
            uint32_t remainder = (uint32_t)distance - (whole << 16);
            uint32_t tenth = (10 * remainder) >> 16;
            uint32_t hundredth = (100 * (remainder - 6553 * tenth)) >> 16;

            w_u32(state + 16, (uint32_t)distance);
            if (hundredth >= 10)
                hundredth = 0;
            frontier_initialize_template_packet(packets + 3420, 0x800D45D0u + 16 * arrow);
            frontier_initialize_template_packet(packets + 3564, 0x800D4920u);
            frontier_initialize_template_packet(packets + 3456, 0x800D4710u + 16 * whole);
            frontier_initialize_template_packet(packets + 3492, 0x800D4710u + 16 * tenth);
            frontier_initialize_template_packet(packets + 3528, 0x800D4710u + 16 * hundredth);
        }
        (void)frontier_link_packet_range(ordering_slot, packets + 3564, packets + 3420);
        return r_u32(ordering_slot);
    }
    return target;
}

uint32_t sub_80062368(uint32_t ordering_slot, uint32_t packets, uint32_t state, uint32_t actor, uint32_t animation)
{
    uint32_t actor_id = r_u16(actor + 172);
    int32_t current = (int32_t)((r_u32(actor + 564) >> 9) & 1u) + r_s8(actor + 848);
    int32_t capacity = r_u8(0x800D81D8u + 36 * actor_id + 20);
    int32_t base = r_u8(r_u32(actor + 556) + 445) + r_s8(0x800D80F5u);
    int32_t total;

    FUNCTION_MARKER(0x80062368, "RASHCDG.BIN");
    if (current < capacity)
        capacity = current;
    total = base + capacity;
    if (total <= 0)
        return total < 9;
    if (total >= 9)
        total = 8;
    if (r_u32(state + 4) != (uint32_t)current)
    {
        int32_t index;

        w_u32(state + 4, (uint32_t)current);
        for (index = 0; index < total; ++index)
        {
            uint32_t icon = index < total - current ? 38 : 39;

            frontier_initialize_template_packet(packets + 3024 + 36 * (uint32_t)index, 0x800D45D0u + 16 * icon);
        }
    }
    if (r_u32(actor + 564) & 0x200u)
    {
        int32_t slot = r_u8(r_u32(actor + 556) + 445);
        int32_t limit = r_u8(0x800D81D8u + 36 * actor_id + 20);

        if (limit < current)
            slot -= current - limit;
        (void)sub_80013E64(animation + 32);
        frontier_initialize_template_packet(packets + 3024 + 36 * (uint32_t)slot, 0x800D45D0u + 16 * (38 + (r_u32(animation + 48) & 1u)));
    }
    else if (r_u32(animation + 48))
    {
        w_u32(state + 4, UINT32_MAX);
        w_u32(animation + 48, 0);
    }
    (void)frontier_link_packet_range(ordering_slot, packets + 36 * (uint32_t)(total + 83), packets + 3024);
    return r_u32(ordering_slot);
}

static uint32_t frontier_allocate_gpu_packet(uint32_t bytes)
{
    uint32_t context = r_u32(0x8005B470u);
    uint32_t packet = r_u32(context + 268);

    if (packet + bytes >= r_u32(0x8005B4D0u))
        packet = sub_80021C98(packet, bytes);
    w_u32(context + 268, packet + bytes);
    return packet;
}

static uint32_t frontier_emit_panel_quad(uint32_t ordering_slot, int32_t first_x, int32_t first_y, int32_t second_x, int32_t second_y)
{
    uint32_t packet = frontier_allocate_gpu_packet(28);
    uint32_t old = r_u32(ordering_slot);

    w_u32(packet, old | 0x06000000u);
    w_u32(packet + 4, 0x4C808080u);
    w_u32(packet + 8, (uint16_t)first_x | ((uint32_t)(uint16_t)first_y << 16));
    w_u32(packet + 12, (uint16_t)second_x | ((uint32_t)(uint16_t)first_y << 16));
    w_u32(packet + 16, (uint16_t)second_x | ((uint32_t)(uint16_t)second_y << 16));
    w_u32(packet + 20, (uint16_t)first_x | ((uint32_t)(uint16_t)second_y << 16));
    w_u32(packet + 24, 0x55555555u);
    w_u32(ordering_slot, packet & 0xFFFFFFu);
    return packet;
}

uint32_t sub_80062618(uint32_t ordering_slot, uint32_t packets, uint32_t suppressed, uint32_t game_state)
{
    uint32_t result = r_u8(game_state + 4) & 0x10u;

    FUNCTION_MARKER(0x80062618, "RASHCDG.BIN");
    if (result && r_u32(0x8005AD14u) == 2)
    {
        int32_t value = (int32_t)r_u8(game_state + 7) - ((int32_t)(uint32_t)(r_u32(0x8005AD44u)));
        uint32_t tens = 0;
        uint32_t digit;

        (void)frontier_link_race_packet(ordering_slot, packets + 72);
        if (value >= 10)
        {
            value -= 10;
            tens = 1;
        }
        digit = (uint32_t)value;
        if (tens)
            frontier_initialize_template_packet(packets + 216, 0x800D4710u + 16 * tens);
        frontier_initialize_template_packet(packets + 252, 0x800D4710u + 16 * digit);
        (void)frontier_link_packet_range(ordering_slot, packets + 252, tens ? packets + 216 : packets + 252);
        return r_u32(ordering_slot);
    }
    if (suppressed)
        return result;
    {
        uint32_t count = r_u8(game_state + 7);
        int32_t width = r_u16(packets + 136);
        int32_t first_x = r_u16(packets + 132) - (int32_t)(count >> 1) * width;
        int32_t second_x = r_u16(packets + 96) - (int32_t)(count >> 1) * width;
        int32_t last_x = first_x + width * (int32_t)count - 1;
        int32_t first_y = r_u16(packets + 134);
        int32_t second_y = r_u16(packets + 98);
        uint32_t source = packets + 72;
        uint32_t index;

        for (index = 0; index < count; ++index)
        {
            if (index < count - r_u32(0x8005AD44u))
            {
                uint32_t packet = frontier_allocate_gpu_packet(20);
                uint32_t mode = (int16_t)r_u16(source + 34) ? 0x64000000u : 0x65000000u;

                w_u32(packet, r_u32(ordering_slot) | 0x04000000u);
                w_u32(packet + 4, ((uint32_t)(int32_t)r_s16(source + 34) << 25) | mode);
                w_u32(packet + 8, (uint16_t)second_x | ((uint32_t)(uint16_t)second_y << 16));
                w_u32(packet + 12, r_u8(source + 12) | ((uint32_t)r_u8(source + 13) << 8) | ((uint32_t)r_u16(source + 14) << 16));
                w_u32(packet + 16, (uint16_t)r_u16(source + 16) | ((uint32_t)r_u16(source + 18) << 16));
                w_u32(ordering_slot, packet & 0xFFFFFFu);
            }
            {
                uint32_t packet = frontier_allocate_gpu_packet(32);
                int32_t end_y = first_y + r_u16(packets + 138) - 1;

                w_u32(packet, r_u32(ordering_slot) | 0x07000000u);
                w_u32(packet + 4, 0x4C808080u);
                w_u32(packet + 8, (uint16_t)first_x | ((uint32_t)(uint16_t)first_y << 16));
                w_u32(packet + 12, (uint16_t)(first_x + width - 1) | ((uint32_t)(uint16_t)first_y << 16));
                w_u32(packet + 16, (uint16_t)(first_x + width - 1) | ((uint32_t)(uint16_t)end_y << 16));
                w_u32(packet + 20, (uint16_t)first_x | ((uint32_t)(uint16_t)end_y << 16));
                w_u32(packet + 24, (uint16_t)first_x | ((uint32_t)(uint16_t)first_y << 16));
                w_u32(packet + 28, 0x55555555u);
                w_u32(ordering_slot, packet & 0xFFFFFFu);
            }
            first_x += width;
            second_x += width;
        }
        {
            int32_t edge = r_u16(packets + 1860) + r_u16(packets + 1864);
            int32_t y = r_u16(packets + 1862);

            (void)frontier_emit_panel_quad(ordering_slot, edge - 3, y + 1, r_u16(packets + 132) - (int32_t)(count >> 1) * width, y + 4);
        }
        {
            int32_t edge = r_u16(packets + 1824);
            int32_t y = r_u16(packets + 1862);

            result = frontier_emit_panel_quad(ordering_slot, edge + 2, y + 1, last_x, y + 4) & 0xFFFFFFu;
        }
    }
    w_u32(ordering_slot, result);
    return result;
}

uint32_t sub_80062610(uint32_t ordering_slot, uint32_t packets, uint32_t unused, uint32_t suppressed)
{
    FUNCTION_MARKER(0x80062610, "RASHCDG.BIN");
    return sub_80062618(ordering_slot, packets, suppressed, r_u32(0x8005B2F8u));
}

uint32_t sub_80062C40(uint32_t ordering_slot, uint32_t packets, uint32_t state, uint32_t cache, uint32_t actor)
{
    uint32_t context = r_u32(0x8005B2F8u);
    uint32_t active = 0;
    uint32_t result;

    FUNCTION_MARKER(0x80062C40, "RASHCDG.BIN");
    if (r_s8(context) == 1 && (r_u8(r_u32(actor + 1084)) & 0x40u) == 0)
    {
        int32_t elapsed = ((int32_t)(uint32_t)(r_u32(0x8005ACC8u) - (r_u32(context + 16) - r_u32(0x8005ACD0u))));
        int32_t value = (int32_t)(((int64_t)elapsed * 256) / 300);

        if (value < 0)
            value = 0;
        if (r_u32(cache + 68) != (uint32_t)value)
        {
            w_u32(cache + 68, (uint32_t)value);
            (void)sub_80013B90(value + 12, packets + 1908, 0x800D4620u);
        }
        if ((value >> 8) < 31)
        {
            active = 1;
            (void)sub_80013E64(state + 64);
        }
    }
    if (active)
    {
        result = r_u32(state + 80);
        if (result != 0)
            return result;
    }
    {
        int32_t index = r_s8(packets + 2156);
        uint32_t color = r_u32(0x800CC68Cu + 4 * (uint32_t)index);

        w_u32(packets + 2124, (color << 24) | r_u32(ordering_slot));
        result = (packets + 1908) & 0xFFFFFFu;
        w_u32(ordering_slot, result);
    }
    return result;
}

uint32_t sub_80062D9C(uint32_t packets, uint32_t state, uint32_t actor, uint32_t player_index)
{
    uint32_t resource = r_u32(actor + 1084);
    uint32_t result;
    int32_t string_id = -1;

    FUNCTION_MARKER(0x80062D9C, "RASHCDG.BIN");
    (void)sub_80013E64(state + 64);
    result = r_u32(state + 80);
    if (result == 0)
        return result;
    if (r_u8(resource + 39) == 254)
        string_id = 28;
    else if (r_u8(resource + 39) == 255)
        string_id = 29;
    if (string_id != -1)
    {
        int32_t width = ((int32_t)(uint32_t)(r_u32(0x800D6120u + 4 * (uint32_t)(string_id - 15)))) >> 1;
        int16_t x = (int16_t)(r_u16(packets + 3696) - width);
        int16_t y = (int16_t)r_u16(packets + 3698);

        sub_8002CD78(r_u32(0x8005AD2Cu), (uint32_t)string_id, (uint32_t)(int32_t)x, (uint32_t)(int32_t)y, r_u32(0x8005B590u + 4 * player_index), 0x70u);
        return 0;
    }
    string_id = ((int32_t)(uint32_t)(r_u32(0x8005B220u))) > 0 ? 26 : 27;
    if (string_id == 27)
    {
        result = r_u32(resource + 40);
        if (result != 0)
            return result;
    }
    {
        uint32_t link = r_u32(0x8005B590u + 4 * player_index);

        sub_8002CD78(r_u32(0x8005AD2Cu), (uint32_t)string_id, (uint32_t)(int32_t)r_s16(packets + 3624), (uint32_t)(int32_t)r_s16(packets + 3626), link, 0x00808080u);
        sub_8002CD78(r_u32(0x8005AD2Cu), (uint32_t)string_id, (uint32_t)(int32_t)r_s16(packets + 3660), (uint32_t)(int32_t)r_s16(packets + 3662), link, 0x00808080u);
    }
    return 0;
}

uint32_t sub_8004CE44(uint32_t packet, uint32_t dither, uint32_t draw_to_display, uint32_t page, uint32_t texture_window)
{
    uint32_t mode = 0xE1000000u | (page & 0x9FFu);

    FUNCTION_MARKER(0x8004CE44, "SLUS_010.53");
    w_u8(packet + 3, 2);
    if (draw_to_display)
        mode |= 0x200u;
    if (dither)
        mode |= 0x400u;
    w_u32(packet + 4, mode);
    if (texture_window)
    {
        uint32_t command = 0xE2000000u | ((uint32_t)(r_u8(texture_window + 2) >> 3) << 15) | ((uint32_t)(r_u8(texture_window) >> 3) << 10) | (((uint32_t)(0 - (int16_t)r_u16(texture_window + 6)) << 2) & 0x3E0u) | (((uint32_t)(0 - (int16_t)r_u16(texture_window + 4)) & 0xFFu) >> 3);

        w_u32(packet + 8, command);
        return command;
    }
    w_u32(packet + 8, 0);
    return mode;
}

uint32_t sub_80062F34(uint32_t packets, uint32_t actor, uint32_t state, uint32_t player_index)
{
    uint32_t resource = r_u32(actor + 1084);
    uint32_t result = r_u8(resource + 1) & 0xFu;
    uint32_t flags;
    uint32_t table;
    uint32_t ordering_slot;
    uint32_t anchor;
    int32_t string_id;
    int32_t first_width;
    int32_t second_width;
    int32_t maximum_width;
    int32_t center_x;
    int32_t center_y;
    int32_t height;
    int32_t left;
    int32_t top;
    int32_t bottom;

    FUNCTION_MARKER(0x80062F34, "RASHCDG.BIN");
    if (result != 2)
        return result;
    flags = r_u32(0x8005AD48u);
    result = flags & 0x1Eu;
    if (result == 0)
        return result;
    (void)sub_80013E64(state + 96);
    result = r_u32(state + 112);
    if (result == 0)
        return result;
    switch (r_u8(resource + 39))
    {
        case 252:
        case 248:
            string_id = r_u8(r_u32(0x8005B2F8u) + 4) == 33 ? 24 : 18;
            break;
        case 253:
            string_id = 20;
            break;
        default:
            string_id = flags & 8u ? 15 : 22;
            break;
    }
    table = r_u32(0x8005B544u);
    if (table == 0)
        return table;
    anchor = packets + 3708;
    center_x = r_u16(anchor + 24);
    center_y = (int16_t)r_u16(anchor + 26);
    first_width = ((int32_t)(uint32_t)(r_u32(0x800D6120u + 4 * (uint32_t)(string_id - 15))));
    second_width = ((int32_t)(uint32_t)(r_u32(0x800D6120u + 4 * (uint32_t)(string_id - 14))));
    ordering_slot = r_u32(0x8005B590u + 4 * player_index);
    height = (int16_t)r_u16(0x800D808Cu + 24 * r_u32(0x8005AD2Cu));
    sub_8002CD78(r_u32(0x8005AD2Cu), (uint32_t)string_id, (uint32_t)(int32_t)(int16_t)(center_x - (first_width >> 1)), (uint32_t)center_y, ordering_slot, 0x00808080u);
    sub_8002CD78(r_u32(0x8005AD2Cu), (uint32_t)(string_id + 1), (uint32_t)(int32_t)(int16_t)(center_x - (second_width >> 1)), (uint32_t)(int32_t)(int16_t)(center_y + 2 * height), ordering_slot, 0x00808080u);
    if (string_id == 15)
    {
        int32_t third_width = ((int32_t)(uint32_t)(r_u32(0x800D6128u)));

        sub_8002CD78(r_u32(0x8005AD2Cu), 17, (uint32_t)(int32_t)(int16_t)(center_x - (third_width >> 1)), (uint32_t)(int32_t)(int16_t)(center_y + 3 * height), ordering_slot, 0x00808080u);
    }
    w_u32(0x800CC674u, r_u32(ordering_slot) | 0x05000000u);
    w_u32(ordering_slot, 0x00CC674u);
    maximum_width = first_width < second_width ? second_width : first_width;
    left = center_x - (maximum_width >> 1);
    top = center_y - height;
    bottom = top + (string_id == 15 ? 6 : 5) * height;
    w_u16(0x800CC67Cu, (uint16_t)left);
    w_u16(0x800CC67Eu, (uint16_t)top);
    w_u16(0x800CC680u, (uint16_t)(left + maximum_width));
    w_u16(0x800CC682u, (uint16_t)top);
    w_u16(0x800CC684u, (uint16_t)left);
    w_u16(0x800CC686u, (uint16_t)bottom);
    w_u16(0x800CC688u, (uint16_t)(left + maximum_width));
    w_u16(0x800CC68Au, (uint16_t)bottom);
    (void)sub_8004CE44(0x800CCD50u, 1, 1, 15, 0);
    w_u32(0x800CCD50u, r_u32(ordering_slot) | 0x02000000u);
    w_u32(ordering_slot, 0x00CCD50u);
    return ordering_slot;
}

uint32_t sub_80063530(uint32_t ordering_slot, uint32_t packets, uint32_t cache, uint32_t state, uint32_t actor, uint32_t countdown, uint32_t suppressed)
{
    uint32_t resource = r_u32(actor + 1084);
    uint32_t context = r_u32(0x8005B2F8u);
    uint32_t result = r_u32(resource + 40);
    uint32_t active = 0;

    FUNCTION_MARKER(0x80063530, "RASHCDG.BIN");
    if (result != 0)
        return result;
    result = 1;
    if (r_s8(context) == 1)
    {
        result = r_u8(resource) & 0x40u;
        if (result == 0)
        {
            int32_t elapsed;
            int32_t value;

            if (countdown)
            {
                elapsed = ((int32_t)(uint32_t)(r_u32(0x8005ACC8u) - r_u32(context + 16)));
                value = (int32_t)(((int64_t)elapsed * 256) / 300);
                if (value < 0)
                    value = 0;
                if ((value >> 8) < 31 && value > 0)
                {
                    active = 1;
                    (void)sub_80013E64(state + 64);
                }
            }
            else
            {
                value = (int32_t)(((int64_t)((int32_t)(uint32_t)(r_u32(context + 16))) * 256) / 300);
            }
            result = r_u32(cache + 68);
            if ((uint32_t)value != result)
            {
                w_u32(cache + 68, (uint32_t)value);
                result = sub_80013B90(value + 12, packets + 1908, 0x800D4620u);
            }
        }
    }
    if (!suppressed)
    {
        result = r_u32(state + 140);
        if (((int32_t)(uint32_t)(result)) <= 0)
        {
            if (active)
            {
                result = r_u32(state + 80);
                if (result == 0)
                    return result;
            }
            {
                int32_t index = r_s8(packets + 2156);
                uint32_t color = r_u32(0x800CC68Cu + 4 * (uint32_t)index);

                w_u32(packets + 2124, (color << 24) | r_u32(ordering_slot));
                result = (packets + 1908) & 0xFFFFFFu;
                w_u32(ordering_slot, result);
            }
        }
    }
    return result;
}

uint32_t sub_800636F0(uint32_t ordering_slot, uint32_t packets, uint32_t state, uint32_t actor, uint32_t suppressed)
{
    uint32_t checkpoint = r_u32(0x8005ACD4u);
    uint32_t result;

    FUNCTION_MARKER(0x800636F0, "RASHCDG.BIN");
    if (!suppressed && ((int32_t)(uint32_t)(r_u32(state + 140))) <= 0)
    {
        int32_t index = r_s8(packets + 2408);
        uint32_t color = r_u32(0x800CC68Cu + 4 * (uint32_t)index);

        w_u32(packets + 2376, (color << 24) | r_u32(ordering_slot));
        w_u32(ordering_slot, (packets + 2160) & 0xFFFFFFu);
    }
    if (((int32_t)(uint32_t)(checkpoint)) < 3)
    {
        uint32_t marker = 0x800D9C40u + 4 * checkpoint;
        int32_t threshold = ((int32_t)(uint32_t)(r_u32(0x800D5F60u + 4 * checkpoint))) << 12;

        if (r_u32(marker) == 0 && ((int32_t)(uint32_t)(r_u32(actor + 324))) < threshold)
        {
            uint32_t context = r_u32(0x8005B2F8u);
            int32_t current_time = ((int32_t)(uint32_t)(r_u32(context + 16)));
            int32_t course = ((int32_t)(uint32_t)(r_u32(context + 64))) - 56;
            int32_t target_time = ((int32_t)(uint32_t)(r_u32(0x80053A88u + 176 * (uint32_t)course + 4 * checkpoint)));

            w_u32(state + 180, (uint32_t)current_time);
            w_u32(marker, (uint32_t)current_time);
            (void)sub_80013B90((int32_t)(((int64_t)current_time * 256) / 300) + 12, packets + 2412, 0x800D4620u);
            (void)sub_80013B90((int32_t)(((int64_t)target_time * 256) / 300), packets + 2664, 0x800D4620u);
            checkpoint++;
            w_u32(0x8005ACD4u, checkpoint);
        }
    }
    result = r_u32(state + 180);
    if (result)
    {
        result = sub_80013E64(state + 160);
        if (!suppressed)
        {
            result = r_u32(state + 140);
            if (((int32_t)(uint32_t)(result)) <= 0)
            {
                result = r_u32(state + 176);
                if (result)
                {
                    int32_t first_index = r_s8(packets + 2660);
                    int32_t second_index = r_s8(packets + 2912);
                    uint32_t first = (packets + 2412) & 0xFFFFFFu;

                    w_u32(packets + 2628, (r_u32(0x800CC68Cu + 4 * (uint32_t)first_index) << 24) | r_u32(ordering_slot));
                    w_u32(ordering_slot, first);
                    w_u32(packets + 2880, (r_u32(0x800CC68Cu + 4 * (uint32_t)second_index) << 24) | first);
                    result = (packets + 2664) & 0xFFFFFFu;
                    w_u32(ordering_slot, result);
                }
            }
        }
    }
    return result;
}

static uint32_t frontier_emit_line(uint32_t ordering_slot, int32_t first_x, int32_t first_y, int32_t second_x, int32_t second_y)
{
    uint32_t packet = frontier_allocate_gpu_packet(16);

    w_u32(packet, r_u32(ordering_slot) | 0x03000000u);
    w_u32(packet + 4, 0x40808080u);
    w_u32(packet + 8, (uint16_t)first_x | ((uint32_t)(uint16_t)first_y << 16));
    w_u32(packet + 12, (uint16_t)second_x | ((uint32_t)(uint16_t)second_y << 16));
    w_u32(ordering_slot, packet & 0xFFFFFFu);
    return packet & 0xFFFFFFu;
}

static uint32_t frontier_link_count_digits(uint32_t ordering_slot, uint32_t packets, int32_t count)
{
    uint32_t tens;
    uint32_t digit;

    (void)frontier_link_race_packet(ordering_slot, packets + 180);
    if (count < 20)
    {
        tens = count >= 10 ? 1u : 0u;
        digit = (uint32_t)(count - (tens ? 10 : 0));
    }
    else
    {
        tens = 2;
        digit = (uint32_t)(count - 20);
    }
    if (tens)
        frontier_initialize_template_packet(packets + 216, 0x800D4710u + 16 * tens);
    frontier_initialize_template_packet(packets + 252, 0x800D4710u + 16 * digit);
    (void)frontier_link_packet_range(ordering_slot, packets + 252, tens ? packets + 216 : packets + 252);
    return r_u32(ordering_slot);
}

uint32_t sub_8005F030(uint32_t ordering_slot, uint32_t packets, uint32_t state, uint32_t actor, uint32_t player_index, uint32_t suppressed)
{
    uint32_t context = r_u32(0x8005B2F8u);
    uint32_t related = 0;
    uint32_t result;
    int32_t count;

    FUNCTION_MARKER(0x8005F030, "RASHCDG.BIN");
    if (r_u32(actor + 856) && r_u32(actor + 1088))
        related = 0x800D8238u + 36 * r_u16(actor + 172);
    count = r_u8(0x800D81F1u + 36 * player_index);
    if (related)
        count += r_u8(related + 1);
    result = r_u8(context + 4) & 0x10u;
    if (result)
    {
        result = 2;
        if (r_u32(0x8005AD14u) == 2)
            return frontier_link_count_digits(ordering_slot, packets, count);
    }
    if (suppressed)
        return result;
    result = r_u32(state + 140);
    if (((int32_t)(uint32_t)(result)) > 0 || count <= 0)
        return result;
    if (count >= 6)
        return frontier_link_count_digits(ordering_slot, packets, count);
    {
        int32_t width = r_u16(packets + 136);
        int32_t half_width = (count >> 1) * width;
        int32_t sprite_x = r_u16(packets + 204) - half_width;
        int32_t left = r_u16(packets + 132) - half_width;
        int32_t right = left + width * count - 1;
        int32_t sprite_y = r_u16(packets + 206);
        uint32_t source = packets + 180;
        int32_t index;

        for (index = 0; index < count; ++index)
        {
            uint32_t packet = frontier_allocate_gpu_packet(20);
            int16_t mode_value = (int16_t)r_u16(source + 34);
            uint32_t mode = mode_value ? 0x64000000u : 0x65000000u;

            w_u32(packet, r_u32(ordering_slot) | 0x04000000u);
            w_u32(packet + 4, ((uint32_t)(int32_t)mode_value << 25) | mode);
            w_u32(packet + 8, (uint16_t)sprite_x | ((uint32_t)(uint16_t)sprite_y << 16));
            w_u32(packet + 12, r_u8(source + 12) | ((uint32_t)r_u8(source + 13) << 8) | ((uint32_t)r_u16(source + 14) << 16));
            w_u32(packet + 16, (uint16_t)r_u16(source + 16) | ((uint32_t)r_u16(source + 18) << 16));
            w_u32(ordering_slot, packet & 0xFFFFFFu);
            sprite_x += width;
        }
        {
            int32_t edge = r_u16(packets + 348) + r_u16(packets + 352);
            int32_t y = r_u16(packets + 350);
            uint32_t reference = packets + (r_u8(context + 4) == 33 ? 1800 : 288);
            int32_t reference_x = r_u16(reference + 24);

            (void)frontier_emit_line(ordering_slot, edge - 3, y + 1, left, y + 1);
            (void)frontier_emit_line(ordering_slot, left, y + 4, edge - 1, y + 4);
            (void)frontier_emit_line(ordering_slot, reference_x + 2, y + 1, right, y + 1);
            result = frontier_emit_line(ordering_slot, right, y + 4, reference_x, y + 4);
        }
    }
    return result;
}

uint32_t sub_8005FB4C(uint32_t ordering_slot, uint32_t packets, uint32_t state, uint32_t actor, uint32_t input_flags, uint32_t animation, uint32_t player_index)
{
    uint32_t context = r_u32(0x8005B2F8u);
    uint32_t descriptor = r_u32(actor + 1084);
    uint32_t status;
    uint32_t result;

    FUNCTION_MARKER(0x8005FB4C, "RASHCDG.BIN");
    if (r_s8(context) == 1 && (r_u8(descriptor) & 0x40u) == 0)
    {
        status = 0;
        w_u32(0, status);
        if (r_u32(r_u32(actor + 852) + 604) < 2 && r_u32(0x8005B220u) == 0 && r_u32(0x8005B288u + 4 * player_index) == 0)
        {
            if (input_flags & 1u)
                status = 4;
            else if (input_flags & 0x10u)
                status = 3;
            else if (input_flags & 0x40u)
                status = 1;
            w_u32(0, status);
        }
        if (status != r_u32(state + 60))
        {
            w_u32(state + 60, status);
            w_u32(animation, 0);
            w_u32(animation + 16, 0);
            w_u32(animation + 12, 0);
            if (status == 1)
                frontier_initialize_template_packet(packets + 2952, 0x800D45F0u);
            else if (status == 3)
                frontier_initialize_template_packet(packets + 2988, 0x800D4600u);
            else if (status == 4)
            {
                frontier_initialize_template_packet(packets + 2924, 0x800D4940u);
                w_u32(animation + 12, 0xFFFFFFF8u);
            }
        }
    }
    status = r_u32(state + 60);
    result = status - 4;
    if (status)
    {
        uint32_t packet_index = 81;
        uint32_t visible = 1;

        if (result >= 2)
        {
            packet_index = 83;
            if (status == 1)
                packet_index = 82;
        }
        (void)sub_80013E64(animation);
        if (((int32_t)(uint32_t)(r_u32(animation + 12))) < 0)
            visible = packet_index != 81;
        result = r_u32(animation + 16);
        if (result && visible)
            result = frontier_link_race_packet(ordering_slot, packets + 36 * packet_index);
    }
    return result;
}

uint32_t sub_8003B9A8(int32_t owner, uint32_t kind)
{
    uint32_t manager;
    int32_t count;
    int32_t index;
    uint32_t record;

    FUNCTION_MARKER(0x8003B9A8, "SLUS_010.53");
    if (owner < 0)
        return 0;
    manager = r_u32(0x8005B240u);
    count = (int16_t)r_u16(manager + 40);
    if (count <= 0 || (int16_t)r_u16(manager + 44) <= 0)
        return 0;
    index = (int16_t)r_u16(manager + 42);
    record = r_u32(manager + 28) + 32 * (uint32_t)index;
    while (index < count)
    {
        if ((int16_t)r_u16(record + 4) == 1 && r_u32(record + 16) == (uint32_t)owner && r_u32(record + 28) == kind)
            return record;
        ++index;
        record += 32;
    }
    return 0;
}

uint32_t sub_8003BC48(int32_t owner, int32_t id, uint32_t *output, int32_t limit)
{
    uint32_t segment, links, metadata;
    uint32_t *candidates;
    int32_t count, index, capacity;
    FUNCTION_MARKER(0x8003BC48, "SLUS_010.53");
    if (owner < 0 || id < 0)
        return 0;
    segment = sub_8003B9A8(owner, 0);
    if (!segment)
        return 0;
    links = r_u32(segment + 12);
    if (!links)
        return 0;
    metadata = sub_80039B60(r_u32(links));
    if (!metadata)
        return 0;
    /* Allocate all metadata entries to repair the original four-slot overflow */
    capacity = r_s16(metadata + 8);
    candidates = malloc((size_t)(capacity > 0 ? capacity : 1) * sizeof(*candidates));
    if (!candidates)
        abort();
    count = (int32_t)sub_80039BB0(metadata, (uint32_t)id, candidates, (uint32_t)limit);
    if (count < 0 || limit < count)
    {
        free(candidates);
        return 0;
    }
    for (index = 0; index < count; ++index)
        output[index] = (uint32_t)(int32_t)r_s16(candidates[index] + 10);
    free(candidates);
    return (uint32_t)count;
}

uint32_t sub_8003C590(uint32_t actor)
{
    uint32_t flags = r_u16(actor + 362) == 1 ? 0x80u : 0;

    FUNCTION_MARKER(0x8003C590, "SLUS_010.53");
    if (sub_80095410(actor) == 0 && (r_u8(r_u32(actor + 1084)) & 0x80u))
    {
        int32_t first = (int32_t)sub_8002E698(xport_guest_ptr(actor + 450, 6), xport_guest_ptr(r_u32(actor + 340) + 14, 6));
        uint32_t second = sub_8002E698(xport_guest_ptr(actor + 450, 6), xport_guest_ptr(r_u32(actor + 340) + 14, 6));
        int32_t third = (int32_t)sub_8002E698(xport_guest_ptr(actor + 450, 6), xport_guest_ptr(r_u32(actor + 340) + 14, 6));
        uint32_t magnitude = ((uint32_t)(first >> 31) + second) ^ (uint32_t)(third >> 31);

        if (((int32_t)(uint32_t)(magnitude)) > 0xC000)
            flags |= 1u;
    }
    if ((flags & 1u) == 0 && sub_80095410(actor) != 0)
        flags |= 2u;
    if ((flags & 3u) == 0 && r_u16(actor + 362) == 0)
    {
        uint32_t nearest;
        uint32_t roads[4];
        uint32_t distance = sub_8003A5F4(actor + 360, &nearest, 1);

        if (nearest == UINT32_MAX)
            return flags;
        if (((int32_t)(uint32_t)(r_u32(0x800531A0u + 4 * r_u32(r_u32(0x8005B2F8u) + 60)))) < ((int32_t)(uint32_t)(distance)))
            return flags;
        {
            uint32_t record = sub_8003F408(r_u32(actor + 428), nearest);

            if (record == 0)
                return flags;
            if (sub_8003BC48((int32_t)nearest, (int32_t)r_u16(actor + 360), roads, 3) == 2)
            {
                if (sub_8003F580(record, roads[0]))
                    flags |= 0x10u;
                if (sub_8003F580(record, roads[1]))
                    flags |= 0x40u;
            }
        }
    }
    return flags;
}

uint32_t sub_8005E850(uint32_t enabled)
{
    uint32_t context;
    int32_t player_count;
    int32_t player_index;
    uint32_t result = enabled;

    FUNCTION_MARKER(0x8005E850, "RASHCDG.BIN");
    if (!enabled)
        return result;

    w_u32(0x8005ACDCu, r_u32(0x8005ACDCu) + 1);
    context = r_u32(0x8005B2F8u);
    if ((r_u8(context + 4) & 0x10u) != 0 && r_u32(0x8005AD14u) != 1)
    {
        uint32_t packet = 0x800523B8u;
        uint32_t viewport = r_u32(0x8005B474u);
        uint32_t ordering_slot = r_u32(0x8005B590u);
        int32_t first_y = (int16_t)r_u16(viewport + 6);
        int32_t second_y = (int16_t)r_u16(viewport + 10);
        int32_t difference = second_y - first_y;
        int32_t sign = difference >> 31;
        int32_t last_y = first_y + ((difference + sign) ^ sign);

        w_u8(packet + 3, 5);
        w_u8(packet + 7, 40);
        w_u16(packet + 8, 0);
        w_u16(packet + 10, (uint16_t)first_y);
        w_u16(packet + 12, 384);
        w_u16(packet + 14, (uint16_t)first_y);
        w_u16(packet + 16, 0);
        w_u16(packet + 18, (uint16_t)last_y);
        w_u16(packet + 20, 384);
        w_u16(packet + 22, (uint16_t)last_y);
        w_u32(packet, (r_u32(packet) & 0xFF000000u) | (r_u32(ordering_slot) & 0xFFFFFFu));
        w_u32(ordering_slot, (r_u32(ordering_slot) & 0xFF000000u) | (packet & 0xFFFFFFu));
    }

    player_count = ((int32_t)(uint32_t)(r_u32(context + 48)));
    for (player_index = 0; player_index < player_count; ++player_index)
    {
        uint32_t actor = r_u32(0x8005B268u + 4 * (uint32_t)player_index);
        uint32_t ordering_slot = r_u32(0x8005B590u + 4 * (uint32_t)player_index);
        uint32_t packets = r_u32(0x8005ACECu + 4 * (uint32_t)player_index);
        uint32_t display = 0x800D4A60u + 88 * (uint32_t)player_index;
        uint32_t state = 0x800D6198u + 224 * (uint32_t)player_index;
        uint32_t animation = 0x800D6358u + 72 * (uint32_t)player_index;
        uint32_t status_flags = sub_8003C590(actor);
        uint32_t mode = r_u8(context + 4);

        if (r_s8(context) == 6)
        {
            if (!(mode & 0x10u))
                continue;
            if (r_u32(0x8005AD14u) == 2)
                (void)sub_8005FA68(ordering_slot, packets, 105, 108);
            (void)sub_8005FA68(ordering_slot, packets, 0, 0);
            continue;
        }
        {
            uint32_t descriptor = r_u32(actor + 1084);
            uint32_t suppressed = (((mode & 1u) != 0 || mode == 44) && (r_u32(actor + 560) & 0x08000000u) != 0);
            uint32_t skip_main;

            w_u32(0x8005B288u + 4 * (uint32_t)player_index, suppressed);
            if ((mode & 1u) != 0 && !r_u32(0x8005B220u) && mode != 44 && (r_u8(descriptor + 1) & 0xFu) == 2 && (mode != 33 || r_u8(descriptor + 39) == 248))
            {
                (void)sub_80062F34(packets, actor, state, (uint32_t)player_index);
                (void)sub_8005FE58(ordering_slot, packets, state, actor);
            }

            skip_main = r_u32(0x8005B220u) || suppressed || r_u32(descriptor + 40);
            if (skip_main)
            {
                (void)sub_80062D9C(packets, state, actor, (uint32_t)player_index);
            }
            else
            {
                uint32_t hide_second;

                (void)sub_8005FF84(ordering_slot, packets, display, actor);
                (void)sub_80062368(ordering_slot, packets, display, actor, state);
                if (!(mode & 1u) || mode == 33 || (r_u8(descriptor + 1) & 0xFu) != 2)
                {
                    (void)sub_80060178(ordering_slot, packets, display, actor);
                    (void)sub_800603E4(ordering_slot, packets, display, (uint32_t)player_index);
                    (void)sub_8005FA68(ordering_slot, packets, 9, 9);
                }
                else
                {
                    (void)sub_80062C40(ordering_slot, packets, state, display, actor);
                    (void)sub_8005FA68(ordering_slot, packets, 51, 51);
                }
                if (!(mode & 4u))
                    (void)sub_80061F6C(ordering_slot, packets, display, actor, (uint32_t)player_index);

                if ((mode & 1u) != 0 && (r_u8(descriptor + 1) & 0xFu) == 2)
                {
                    (void)sub_8005FAC4(ordering_slot, packets + 3384, (uint32_t)player_index);
                    (void)sub_800C52A0(actor, (uint32_t)player_index);
                    (void)sub_8005FA68(ordering_slot, packets, 50, 50);
                    (void)sub_8005FA68(ordering_slot, packets, 104, 104);
                    if (mode == 33)
                        (void)sub_8005F030(ordering_slot, packets, state, actor, (uint32_t)player_index, status_flags);
                    else
                        (void)sub_80062610(ordering_slot, packets, (uint32_t)player_index, status_flags);
                }
                else if (((mode & 4u) != 0 && mode != 44) || (mode == 44 && r_u8(context + 57) < 2))
                {
                    uint32_t countdown = mode == 44 || mode == 36;

                    (void)sub_80063530(ordering_slot, packets, display, state, actor, countdown, status_flags);
                    if (mode == 4)
                        (void)sub_800636F0(ordering_slot, packets, state, actor, status_flags);
                }
                else
                {
                    if (!(mode & 4u))
                        (void)sub_8005F030(ordering_slot, packets, state, actor, (uint32_t)player_index, status_flags);
                    (void)sub_800606F0(ordering_slot, packets, display, actor, status_flags, (uint32_t)player_index, state);
                    (void)sub_800C52A0(actor, (uint32_t)player_index);
                    (void)sub_8005FA68(ordering_slot, packets, 8, 8);
                    (void)sub_8005FA68(ordering_slot, packets, 104, 104);
                }

                hide_second = sub_8006148C(ordering_slot, packets, display, actor, (uint32_t)player_index, animation, state);
                (void)sub_80063408(actor, packets, animation, (uint32_t)player_index, hide_second);
                (void)sub_80060C10(ordering_slot, packets, display, actor, (uint32_t)player_index, animation, state);
                (void)sub_80061E50(packets, (uint32_t)player_index, animation);
                (void)sub_8005FE58(ordering_slot, packets, state, actor);
                (void)sub_8005FB4C(ordering_slot, packets, display, actor, status_flags, state, (uint32_t)player_index);
            }
        }

        if ((mode & 0x10u) != 0 && r_u32(0x8005AD14u) == 2)
            (void)sub_8005FA68(ordering_slot, packets, 105, 108);
        (void)sub_8005FA68(ordering_slot, packets, 0, 0);
    }

    w_u8(0x800CCD63u, 1);
    w_u32(0x800CCD64u, 0xE1000600u);
    {
        uint32_t ordering_slot = r_u32(0x8005B5A0u);

        result = r_u32(ordering_slot) | 0x01000000u;
        w_u32(0x800CCD60u, result);
        w_u32(ordering_slot, 0x00CCD60u);
    }
    return result;
}

uint32_t sub_8005E848(void)
{
    FUNCTION_MARKER(0x8005E848u, "RASHCDG.BIN");
    return sub_8005E850(r_u32(0x8005ACD8u));
}

uint32_t sub_80064B9C(uint32_t player_index)
{
    uint32_t mode;
    uint32_t result;
    MATRIX translation = {0};

    FUNCTION_MARKER(0x80064B9C, "RASHCDG.BIN");
    SetTransMatrix(&translation);
    (void)SetRotMatrix((MATRIX *)xport_guest_ptr(r_u32(0x8005AEC0 + 4 * player_index) + 92, sizeof(MATRIX)));
    (void)(uint32_t)sub_800650D0(player_index);
    mode = r_u32(r_u32(0x8005B2F8) + 48);
    if (mode == 1 && !r_u32(0x8005B2D0) && (int16_t)r_u16(r_u32(0x8005B278) + 20) != -1)
        (void)(uint32_t)sub_800644F4();
    mode = r_u32(r_u32(0x8005B2F8) + 48);
    if ((mode == 2 || r_u32(0x8005B2D0)) && r_u32(0x8005AD24))
        sub_80064CC8(player_index);
    if (r_u32(0x8005B308))
        (void)(uint32_t)sub_8006396C();
    result = r_u32(0x8005AD28);
    if (result)
        return (uint32_t)sub_80063C5C(player_index);
    return result;
}

uint32_t sub_800650D0(uint32_t player_index)
{
    uint32_t player = r_u32(0x8005AEC0 + 4 * player_index);
    uint32_t angle = ((uint32_t)(int32_t)r_s16(player + 124) - 443) & 0xfff;
    uint32_t offset = 110 * angle / 4096;
    uint32_t mode;

    FUNCTION_MARKER(0x800650D0, "RASHCDG.BIN");
    w_u16(0x800D5EE8, (uint16_t)offset);
    w_u16(0x800D5EEA, (uint16_t)(offset + 24));
    mode = r_u32(r_u32(0x8005B2F8) + 48);
    if (mode == 1 && !r_u32(0x8005B2D0))
        return (uint32_t)sub_80065174();
    return 0x80060000;
}

uint32_t sub_800654B4(int32_t route, int32_t position)
{
    int32_t scalar_value_8;
    uint32_t scalar_sign_8;

    uint32_t manager = r_u32(0x8005B278);
    int32_t selected = -1;
    int32_t index;

    FUNCTION_MARKER(0x800654B4, "RASHCDG.BIN");
    if (!(int16_t)r_u16(manager + 34))
        return UINT32_MAX;
    index = (int16_t)r_u16(manager + 18);
    if (index != -1 && r_u32(manager + 740 + 12 * (uint32_t)index + 8) == r_u32(r_u32(0x8005B508)))
        return (uint32_t)index;
    for (index = 0; index != 20; ++index)
    {
        uint32_t record = manager + 740 + 12 * (uint32_t)index;

        if (r_u32(record + 8) == r_u32(r_u32(0x8005B508)))
        {
            selected = index;
            break;
        }
    }
    if (selected == -1)
    {
        uint32_t closest = 0xffff;

        for (index = 0; index != 20; ++index)
        {
            uint32_t record = manager + 740 + 12 * (uint32_t)index;
            uint32_t segments;
            int32_t segment;

            if (!r_u32(record))
                continue;
            segments = r_u32(record + 4);
            for (segment = 0; segment != 4; ++segment)
            {
                uint32_t entry = segments + 6 * (uint32_t)segment;

                if ((int16_t)r_u16(entry) == route)
                {
                    int32_t total = (int16_t)r_u16(entry + 2) + (int16_t)r_u16(entry + 4);
                    int32_t midpoint = total / 2;
                    uint32_t distance = ((scalar_value_8 = (((int32_t)(uint32_t)((uint32_t)midpoint - (uint32_t)position)))), (scalar_sign_8 = (uint32_t)scalar_value_8 >> 31), (((uint32_t)(((uint32_t)scalar_value_8 + scalar_sign_8) ^ (0u - scalar_sign_8)))));

                    if (distance < closest)
                    {
                        closest = distance;
                        selected = index;
                    }
                    break;
                }
            }
        }
    }
    if (selected != -1)
    {
        uint32_t record = manager + 740 + 12 * (uint32_t)selected;
        uint32_t source = r_u32(record) + 12;

        for (index = 1; index < 60; ++index)
        {
            w_u32(manager + 984 + 4 * (uint32_t)(index - 1), source);
            source += r_u32(source + 4);
        }
        w_u32(manager + 4, r_u32(r_u32(manager + 984) + 20));
        return (uint32_t)selected;
    }
    return (uint32_t)(int32_t)r_s16(manager + 18);
}

uint32_t sub_800656A8(int32_t point, int32_t first_start, int32_t second_start, int32_t second_end)
{
    int32_t first_end = first_start;
    int32_t start = second_start;
    int32_t end = second_end;
    int32_t distance;
    int32_t span;

    FUNCTION_MARKER(0x800656A8, "RASHCDG.BIN");
    point = (int16_t)point;
    if ((int16_t)first_start < point)
    {
        first_end = first_start + 110;
        if ((int16_t)second_start < (int16_t)first_start)
        {
            start = second_start + 110;
            end = second_end + 110;
        }
    }
    start = (int16_t)start;
    if ((int16_t)end < start)
        end += 110;
    distance = point - start;
    if (start >= point)
    {
        if ((int16_t)first_end >= (int16_t)end)
            return 0;
        span = (int16_t)end - (int16_t)first_end;
        if ((int16_t)first_end >= start)
            distance = point - (int16_t)first_end + 110;
        else
            distance = point - start + 110;
    }
    else
    {
        if ((int16_t)end >= point)
            span = point - (int16_t)first_end + 110;
        else
            span = (int16_t)end - (int16_t)first_end + 110;
    }
    return (uint32_t)(distance >= span ? span : -distance);
}

uint32_t sub_800662BC(void)
{
    FUNCTION_MARKER(0x800662BC, "RASHCDG.BIN");
    return sub_800662C4(r_u32(0x8005B278));
}

uint32_t sub_80065174(void)
{
    uint32_t manager = r_u32(0x8005B278);
    uint32_t control = r_u32(0x8005AE34);
    uint32_t angle;
    int32_t first;
    int32_t route;
    int32_t position;
    int32_t selected;

    FUNCTION_MARKER(0x80065174, "RASHCDG.BIN");
    if (r_u32(manager + 24))
        return 0x80060000;
    angle = ((uint32_t)(int32_t)r_s16(r_u32(0x8005AEC0) + 124) - 443) & 0xfff;
    first = (int32_t)(110 * angle / 4096);
    w_u16(manager + 48, (uint16_t)first);
    w_u16(manager + 50, (uint16_t)(first + 24 >= 110 ? first - 86 : first + 24));
    route = ((int32_t)(uint32_t)(r_u32(control + 72)));
    position = ((int32_t)(uint32_t)(r_u32(control + 68) << 6));
    selected = (int16_t)sub_800654B4(route, position);
    w_u16(manager + 16, (uint16_t)selected);
    if (selected == -1)
    {
        w_u16(manager + 18, 0xffff);
        return UINT32_MAX;
    }
    w_u32(manager + 8, (uint32_t)route);
    w_u32(manager + 12, (uint32_t)position);
    if (selected != (int16_t)r_u16(manager + 18))
    {
        uint32_t packed = r_u32(manager + 988) + 8;
        uint32_t twos = 0;
        uint32_t width = 0;
        uint32_t column;

        w_u16(manager + 18, (uint16_t)selected);
        for (column = 0; column != 110; ++column)
        {
            uint32_t group = r_u32(manager + 1000 + 4 * (column / 2));

            if (!(column & 1))
                width = 0;
            w_u16(manager + 292 + 2 * column, (uint16_t)width);
            w_u16(manager + 512 + 2 * column, (uint16_t)twos);
            if (group)
            {
                uint32_t count = 0;
                uint32_t item;
                uint32_t first_item = 1;

                w_u8(manager + 72 + column, 0);
                for (item = 0; item != 8; ++item)
                {
                    uint32_t value = (r_u8(packed + item / 4) >> (6 - 2 * (item & 3))) & 3;

                    if (value)
                    {
                        ++count;
                        if (value == 2)
                            ++twos;
                        if (first_item)
                        {
                            first_item = 0;
                            w_u8(manager + 72 + column, (uint8_t)item);
                        }
                    }
                }
                if (r_u32(group + 12) || !count)
                {
                    width += count << 9;
                    w_u8(manager + 182 + column, (uint8_t)count);
                }
            }
            packed += 2;
        }
        {
            int32_t even = (int16_t)r_u16(manager + 48);
            uint16_t origin = r_u16(0x8005B390);
            uint16_t height = r_u16(0x8005B394);

            even = (even / 2) * 2;
            w_u16(manager + 30, 26);
            w_u32(manager + 36, 1);
            w_u16(manager + 32, 1);
            w_u16(manager + 28, 1);
            w_u16(manager + 58, height);
            w_u16(manager + 62, height + 16);
            w_u16(manager + 20, 0xffff);
            w_u16(manager + 52, (uint16_t)even);
            w_u16(manager + 54, (uint16_t)(even - 1));
            w_u16(manager + 44, 1);
            w_u16(manager + 56, origin);
            w_u16(manager + 60, origin);
            w_u16(manager + 46, 0);
            w_u16(manager + 42, r_u16(manager + 48));
            return sub_800662BC();
        }
    }
    if ((int16_t)r_u16(manager + 48) != (int16_t)r_u16(manager + 42))
    {
        int32_t delta = (int16_t)sub_800656A8((int16_t)r_u16(manager + 52), (int16_t)r_u16(manager + 54), (int16_t)r_u16(manager + 48), (int16_t)r_u16(manager + 50));

        w_u16(manager + 30, (uint16_t)delta);
        if (delta)
        {
            if (delta >= 0)
                w_u16(manager + 28, 1);
            else
            {
                w_u16(manager + 30, (uint16_t)-delta);
                w_u16(manager + 28, 0);
            }
            (void)sub_800662BC();
        }
        w_u16(manager + 42, r_u16(manager + 48));
    }
    return r_u16(manager + 48);
}

uint32_t sub_800662C4(uint32_t manager)
{
    int32_t first_start = (int16_t)r_u16(manager + 52);
    int32_t first_end = (int16_t)r_u16(manager + 54);
    int32_t first_half = first_start / 2;
    int32_t second_half = first_end / 2;
    int32_t direction = (int16_t)r_u16(manager + 28);
    int32_t candidate;
    int32_t index;
    uint32_t entry;
    uint32_t payload;
    int32_t skipped = 0;
    int32_t adjustment = 0;

    FUNCTION_MARKER(0x800662C4, "RASHCDG.BIN");
    if (!direction)
    {
        candidate = first_start - 1;
        if (candidate < 0)
            candidate = 109;
    }
    else if (direction == 1)
    {
        candidate = first_end + 1;
        if (candidate >= 110)
            candidate = 0;
    }
    else
    {
        w_u32(manager + 24, 0);
        w_u16(manager + 30, 0);
        return 1;
    }
    index = candidate / 2;
    if (index == (int16_t)r_u16(manager + 20))
        return (uint32_t)sub_800657A8();
    entry = r_u32(manager + 1000 + 4 * (uint32_t)index);
    if (!entry)
    {
        w_u32(manager + 24, 0);
        w_u16(manager + 30, 0);
        return 0;
    }
    payload = entry + 8;
    if (!r_u32(payload + 4))
    {
        int32_t boundary_mode = (int16_t)r_u16(manager + 32);
        int32_t attempts = 1;

        for (;;)
        {
            if (attempts == 55)
            {
                w_u16(manager + 52, 0);
                w_u16(manager + 54, 109);
                w_u32(manager + 24, 0);
                w_u16(manager + 30, 0);
                return manager;
            }
            if (direction == 1)
            {
                if (boundary_mode)
                {
                    int32_t old_end = (int16_t)r_u16(manager + 54);

                    boundary_mode = 0;
                    w_u16(manager + 54, (uint16_t)(2 * index - 1));
                    w_u16(manager + 52, (uint16_t)(2 * index));
                    adjustment = old_end - (2 * index - 1);
                    ++index;
                }
                else if (index++ == first_half)
                {
                    int32_t value = (int16_t)r_u16(manager + 52) - 1;

                    w_u32(manager + 24, 0);
                    w_u16(manager + 30, 0);
                    w_u16(manager + 54, (uint16_t)(value < 0 ? 109 : value));
                    return 109;
                }
                if (index >= 55)
                    index = 0;
            }
            else
            {
                int32_t next = index + 1;

                if (boundary_mode)
                {
                    int32_t old_start = (int16_t)r_u16(manager + 52);

                    boundary_mode = 0;
                    w_u16(manager + 52, (uint16_t)(2 * next));
                    w_u16(manager + 54, (uint16_t)(2 * next - 1));
                    adjustment = 2 * next - old_start;
                    --index;
                }
                else if (index-- == second_half)
                {
                    uint32_t old_end = r_u16(manager + 54);
                    int32_t old_signed = (int16_t)old_end;

                    w_u32(manager + 24, 0);
                    w_u16(manager + 30, 0);
                    w_u16(manager + 52, (uint16_t)(old_end + 1));
                    if (old_signed >= 110)
                        w_u16(manager + 54, 0);
                    return old_end + 1;
                }
                if (index < 0)
                    index = 54;
            }
            entry = r_u32(manager + 1000 + 4 * (uint32_t)index);
            if (!entry)
            {
                w_u32(manager + 24, 0);
                w_u16(manager + 30, 0);
                return 0;
            }
            payload = entry + 8;
            ++attempts;
            if (r_u32(payload + 4))
            {
                skipped = attempts - 1;
                break;
            }
        }
    }
    if (skipped)
    {
        int32_t amount = 2 * skipped;
        int32_t progress;

        if (direction == 1)
        {
            int32_t value = (int16_t)r_u16(manager + 54) + amount;
            w_u16(manager + 54, (uint16_t)(value >= 111 ? value - 110 : value));
        }
        else
        {
            int32_t value = (int16_t)r_u16(manager + 52) - amount;
            w_u16(manager + 52, (uint16_t)(value < 0 ? value + 110 : value));
        }
        progress = (int32_t)r_u16(manager + 30) - amount + adjustment;
        w_u16(manager + 30, (uint16_t)progress);
        if ((int16_t)progress <= 0)
        {
            w_u16(manager + 30, 0);
            w_u32(manager + 24, 0);
            return (uint32_t)progress << 16;
        }
    }
    {
        uint32_t product = r_u32(payload) * r_u32(payload + 4);

        if (product && product <= 4096)
        {
            uint32_t destination = manager + 9920 - 4 * r_u16(payload + 8);

            (void)(uint32_t)DecDCTinSync(((int32_t)(uint32_t)(0)));
            (void)(uint32_t)DecDCToutSync(((int32_t)(uint32_t)(0)));
            w_u32(manager + 24, 2);
            if (!(uint32_t)sub_80020400(payload + 8, destination))
            {
                w_u32(manager + 732, payload);
                w_u16(manager + 20, (uint16_t)index);
                w_u32(manager + 24, 1);
                DecDCTin((uint32_t *)xport_guest_ptr(destination, 1), (int16_t)r_u16(manager + 22));
                DecDCTout((uint32_t *)xport_guest_ptr(manager + 1220u, 1), (int32_t)(product / 2u));
                return r_u32(0x8005A370u);
            }
        }
    }
    w_u32(manager + 24, 0);
    w_u16(manager + 30, 0);
    return manager;
}

uint32_t sub_80020400(uint32_t input, uint32_t output)
{
    uint32_t bit_count = 0;
    uint32_t output_start = output;
    uint32_t lengths_a = r_u32(0x8005ADAC);
    uint32_t lengths_b = r_u32(0x8005ADB0);
    uint32_t values_a = lengths_a + 0x2000;
    uint32_t values_b = lengths_b + 512;
    uint32_t prefix;
    uint32_t bits;

    FUNCTION_MARKER(0x80020400, "SLUS_010.53");
    w_u16(output, r_u16(input));
    output += 2;
    w_u16(output, r_u16(input + 2));
    output += 2;
    prefix = (uint32_t)r_u16(input + 4) << 10;
    bits = (uint32_t)r_u16(input + 8) << 16;
    bits |= r_u16(input + 10);
    input += 12;
    for (;;)
    {
        uint32_t code = bits >> 22;

        bit_count += 10;
        if (code == 511)
            break;
        bits <<= 10;
        if (bit_count & 16)
        {
            bit_count &= 15;
            bits |= (uint32_t)r_u16(input) << bit_count;
            input += 2;
        }
        bit_count &= 15;
        w_u16(output, (uint16_t)(code | prefix));
        output += 2;
        for (;;)
        {
            uint32_t index = bits >> 19;
            uint32_t length;
            uint16_t value;

            if (index >= 32)
            {
                value = r_u16(values_a + 2 * index);
                length = r_u8(lengths_a + index);
            }
            else
            {
                bit_count += 8;
                bits <<= 8;
                if (bit_count & 16)
                {
                    bit_count &= 15;
                    bits |= (uint32_t)r_u16(input) << bit_count;
                    input += 2;
                }
                bit_count &= 15;
                index = bits >> 23;
                value = r_u16(values_b + 2 * index);
                length = r_u8(lengths_b + index);
            }
            bits <<= length & 31u;
            bit_count += length;
            if (bit_count & 16)
            {
                bit_count &= 15;
                bits |= (uint32_t)r_u16(input) << bit_count;
                input += 2;
            }
            bit_count &= 15;
            if (value == 0x7c1f)
            {
                w_u16(output, (uint16_t)(bits >> 16));
                output += 2;
                bits = (bits << 16) | ((uint32_t)r_u16(input) << bit_count);
                input += 2;
                continue;
            }
            w_u16(output, value);
            output += 2;
            if (value == 0xfe00)
                break;
        }
    }
    {
        uint32_t used = output - output_start;
        uint32_t padded = used - 4;

        if (padded & 0x7f)
        {
            if (output & 2)
            {
                w_u16(output, 0xfe00);
                output += 2;
                padded = used - 2;
            }
            while (padded & 0x7f)
            {
                w_u32(output, 0xfe00fe00);
                output += 4;
                padded += 4;
            }
        }
    }
    return 0;
}

uint32_t sub_800644F4(void)
{
    FUNCTION_MARKER(0x800644F4, "RASHCDG.BIN");
    return sub_800644FC(r_u32(0x8005B278));
}

static void frontier_panorama_project(uint32_t output, int32_t angle_index, int32_t first_row, int32_t last_row)
{
    SVECTOR vertex;

    vertex.vx = (sint16)r_u16(0x8005624C + 4 * ((uint32_t)angle_index & 0xfff));
    vertex.vz = (sint16)r_u16(0x8005624E + 4 * ((uint32_t)angle_index & 0xfff));
    vertex.pad = 0;
    while (first_row <= last_row)
    {
        sint32 screen;
        sint32 flags;
        sint32 depth;

        vertex.vy = (sint16)(279 * first_row - 2232);
        depth = gte_project(&vertex, &screen, &flags);
        w_u32(output + 4 * (uint32_t)first_row, (uint32_t)screen);
        w_u32(0x1F800034, (uint32_t)depth);
        ++first_row;
    }
}

uint32_t sub_800644FC(uint32_t manager)
{
    uint32_t packet;
    uint32_t first_buffer = 0x1F800038;
    uint32_t second_buffer = 0x1F800080;
    int32_t current_column;
    int32_t texture_row;
    int32_t low;
    int32_t high;
    int32_t strip;
    uint32_t texture;
    uint32_t ot_head;
    uint32_t descriptor;
    unsigned iteration;

    FUNCTION_MARKER(0x800644FC, "RASHCDG.BIN");
    if ((int16_t)r_u16(manager + 18) == -1)
        return 0xffffffff;
    w_u32(0x1F800028, first_buffer);
    w_u32(0x1F80002C, second_buffer);
    if (r_u32(manager + 24))
    {
        unsigned timeout = 0;
        do
        {
            ++timeout;
            if (r_u32(r_u32(0x8005B278) + 24) == 2)
            {
                (void)(uint32_t)sub_800657A8();
                timeout = 1;
            }
            if (timeout >= 10000)
            {
                uint32_t current_manager = r_u32(0x8005B278);
                w_u32(current_manager + 24, 0);
                w_u16(current_manager + 30, 0);
                DecDCTReset(1);
                return 0x60000000;
            }
        } while (r_u32(r_u32(0x8005B278) + 24));
    }
    manager = r_u32(0x8005B278);
    packet = r_u32(r_u32(0x8005B470) + 268);
    current_column = (int16_t)r_u16(manager + 52);
    texture_row = (int16_t)r_u16(manager + 36);
    while (current_column != (int16_t)r_u16(manager + 48))
    {
        texture_row += r_u8(manager + 182 + (uint32_t)current_column);
        ++current_column;
        if (current_column >= 110)
            current_column = 0;
    }
    if (texture_row >= (int16_t)r_u16(manager + 38))
        texture_row -= (int16_t)r_u16(manager + 38);
    if (texture_row > (int16_t)r_u16(manager + 46) && texture_row < (int16_t)r_u16(manager + 44))
        texture_row = (int16_t)r_u16(manager + 44);
    descriptor = manager + 10052 + 12 * (uint32_t)texture_row;
    texture = r_u32(manager + 992) + 8;
    current_column = (int16_t)r_u16(manager + 48);
    low = r_u8(texture + (uint32_t)current_column) + r_u8(manager + 72 + (uint32_t)current_column);
    high = low + r_u8(manager + 182 + (uint32_t)current_column);
    w_u32(0x1F800024, texture);
    w_u32(0x1F800004, (uint32_t)current_column);
    w_u32(0x1F80000C, (uint32_t)low);
    w_u32(0x1F800018, (uint32_t)high);
    frontier_panorama_project(first_buffer, (2383 * current_column) >> 6, low, high);
    current_column = current_column == 109 ? 0 : current_column + 1;
    w_u32(0x1F800004, (uint32_t)current_column);
    strip = texture_row + 1;
    w_u32(0x1F800020, (uint32_t)((int16_t)r_u16(manager + 46) < texture_row ? (int16_t)r_u16(manager + 40) : (int16_t)r_u16(manager + 46) + 1));
    ot_head = r_u32(r_u32(0x8005B59C));
    w_u32(0x1F800000, 0);
    w_u32(0x1F800030, ot_head);
    for (iteration = 0; iteration != 25; ++iteration)
    {
        manager = r_u32(0x8005B278);
        int32_t next_low = r_u8(texture + (uint32_t)current_column) + r_u8(manager + 72 + (uint32_t)current_column);
        int32_t next_high = next_low + r_u8(manager + 182 + (uint32_t)current_column);
        int32_t overlap_low = low < next_low ? low : next_low;
        int32_t overlap_high = high > next_high ? high : next_high;
        int32_t count = high - low;
        int32_t index;

        w_u32(0x1F800010, (uint32_t)next_low);
        w_u32(0x1F80001C, (uint32_t)next_high);
        w_u32(0x1F800008, (uint32_t)overlap_low);
        w_u32(0x1F800014, (uint32_t)overlap_high);
        frontier_panorama_project(second_buffer, (2383 * current_column) >> 6, overlap_low, overlap_high);
        if (count > 0)
        {
            if (packet + 40 * (uint32_t)count >= r_u32(0x8005B4D0))
                packet = sub_80021C98(packet, 40 * (uint32_t)count);
            uint32_t wrap_manager = r_u32(0x8005B278);
            for (index = 0; index != count; ++index)
            {
                uint32_t first_uv = r_u32(descriptor);
                uint32_t second_uv = r_u32(descriptor + 4);
                uint32_t packed_uv = r_u32(descriptor + 8);
                descriptor += 12;
                uint32_t row = (uint32_t)(low + index);

                w_u32(packet, r_u32(0x1F800030) | 0x09000000);
                ot_head = packet;
                w_u32(0x1F800030, ot_head);
                w_u32(packet + 4, r_u32(0x8005237C) | 0x2c000000);
                w_u32(packet + 8, r_u32(first_buffer + 4 * row));
                w_u32(packet + 12, first_uv);
                w_u32(packet + 16, r_u32(second_buffer + 4 * row));
                w_u32(packet + 20, second_uv);
                w_u32(packet + 24, r_u32(first_buffer + 4 * (row + 1)));
                w_u32(packet + 28, packed_uv >> 16);
                w_u32(packet + 32, r_u32(second_buffer + 4 * (row + 1)));
                w_u32(packet + 36, packed_uv);
                packet += 40;
                if (strip >= ((int32_t)(uint32_t)(r_u32(0x1F800020))))
                {
                    if (strip == (int16_t)r_u16(wrap_manager + 46) + 1)
                    {
                        strip = (int16_t)r_u16(wrap_manager + 44);
                        descriptor = wrap_manager + 10052 + 12 * (uint32_t)strip;
                        w_u32(0x1F800020, (uint32_t)(int32_t)(int16_t)r_u16(wrap_manager + 40));
                    }
                    else if (strip >= (int16_t)r_u16(wrap_manager + 40))
                    {
                        strip = 0;
                        descriptor = wrap_manager + 10052;
                        w_u32(0x1F800020, (uint32_t)((int16_t)r_u16(wrap_manager + 46) + 1));
                    }
                }
                ++strip;
            }
            texture_row = strip - 1;
        }
        {
            uint32_t swap = first_buffer;
            first_buffer = second_buffer;
            second_buffer = swap;
        }
        low = next_low;
        high = next_high;
        current_column = current_column == 109 ? 0 : current_column + 1;
        w_u32(0x1F800028, first_buffer);
        w_u32(0x1F80002C, second_buffer);
        w_u32(0x1F800004, (uint32_t)current_column);
        w_u32(0x1F80000C, (uint32_t)low);
        w_u32(0x1F800018, (uint32_t)high);
        w_u32(0x1F800000, iteration + 1);
    }
    w_u32(r_u32(0x8005B59C), r_u32(0x1F800030));
    w_u32(r_u32(0x8005B470) + 268, packet);
    return r_u32(0x8005B470);
}

static uint32_t frontier_project_vertex(uint32_t address)
{
    SVECTOR vertex;
    sint32 screen;
    sint32 flags;

    vertex.vx = (sint16)r_u16(address);
    vertex.vy = (sint16)r_u16(address + 2);
    vertex.vz = (sint16)r_u16(address + 4);
    vertex.pad = (sint16)r_u16(address + 6);
    (void)gte_project(&vertex, &screen, &flags);
    return (uint32_t)screen;
}

static uint32_t sub_800642F8(uint32_t ot, uint32_t first, uint32_t second, uint32_t third, uint32_t fourth, uint32_t first_color, uint32_t second_color, uint32_t third_color, uint32_t fourth_color)
{
    uint32_t display = r_u32(0x8005B470);
    uint32_t packet = r_u32(display + 268);
    unsigned index;
    const uint32_t positions[4] = {first, second, third, fourth};
    const uint32_t colors[4] = {first_color, second_color, third_color, fourth_color};

    FUNCTION_MARKER(0x800642F8, "RASHCDG.BIN");
    if (packet + 36 >= r_u32(0x8005B4D0))
        packet = sub_80021C98(packet, 36);
    w_u32(display + 268, packet + 36);
    w_u8(packet + 3, 8);
    w_u8(packet + 7, 56);
    for (index = 0; index != 4; ++index)
    {
        w_u32(packet + 8 + 8 * index, positions[index]);
        w_u8(packet + 4 + 8 * index, (uint8_t)colors[index]);
        w_u8(packet + 5 + 8 * index, (uint8_t)(colors[index] >> 8));
        w_u8(packet + 6 + 8 * index, (uint8_t)(colors[index] >> 16));
    }
    w_u32(packet, (r_u32(packet) & 0xff000000) | (r_u32(ot) & 0x00ffffff));
    w_u32(ot, (r_u32(ot) & 0xff000000) | (packet & 0x00ffffff));
    return r_u32(ot);
}

uint32_t sub_8006396C(void)
{
    int32_t segment = 24 * (int16_t)r_u16(0x800D5EE8) / 110;
    uint32_t display = r_u32(0x8005B470);
    uint32_t packet = r_u32(display + 268);
    uint32_t ot = r_u32(0x8005B59C) + 4;
    uint32_t source = 0x800D443C + 16 * (uint32_t)segment;
    uint32_t previous_first;
    uint32_t previous_second;
    uint32_t previous_clip;
    unsigned index;

    FUNCTION_MARKER(0x8006396C, "RASHCDG.BIN");
    if (packet + 320 >= r_u32(0x8005B4D0))
    {
        packet = sub_80021C98(packet, 320);
        w_u32(display + 268, packet);
    }
    previous_first = frontier_project_vertex(source);
    previous_second = frontier_project_vertex(source + 8);
    previous_clip = ((uint32_t)(((((uint32_t)(previous_first)) & ((uint32_t)(previous_second))) & 0x8000) | ((int16_t)(((uint32_t)(previous_first)) & 0xffff) >= 385 && (int16_t)(((uint32_t)(previous_second)) & 0xffff) >= 385)));
    source += 16;
    for (index = 0; index != 8; ++index, ++segment)
    {
        uint32_t current_first;
        uint32_t current_second;
        uint32_t current_clip;

        if (segment == 23)
            source = 0x800D443C;
        current_first = frontier_project_vertex(source);
        current_second = frontier_project_vertex(source + 8);
        current_clip = ((uint32_t)(((((uint32_t)(current_first)) & ((uint32_t)(current_second))) & 0x8000) | ((int16_t)(((uint32_t)(current_first)) & 0xffff) >= 385 && (int16_t)(((uint32_t)(current_second)) & 0xffff) >= 385)));
        source += 16;
        if (!(previous_clip & current_clip))
        {
            uint32_t descriptor = 0x800D440C + 12 * ((uint32_t)segment & 3);

            w_u32(packet, r_u32(ot) | 0x09000000);
            w_u32(packet + 4, 0x2f000000);
            w_u32(packet + 8, previous_first);
            w_u32(packet + 12, r_u32(descriptor));
            w_u32(packet + 16, current_first);
            w_u32(packet + 20, r_u32(descriptor + 4));
            w_u32(packet + 24, previous_second);
            w_u32(packet + 28, r_u16(descriptor + 10));
            w_u32(packet + 32, current_second);
            w_u32(packet + 36, r_u32(descriptor + 8));
            w_u32(ot, packet);
            packet += 40;
        }
        previous_first = current_first;
        previous_second = current_second;
        previous_clip = current_clip;
    }
    w_u32(display + 268, packet);
    return display;
}

static uint32_t frontier_pack_pair(int16_t x, int16_t y)
{
    return (uint16_t)x | ((uint32_t)(uint16_t)y << 16);
}

static uint32_t frontier_blend_color(uint32_t first, uint32_t second, int32_t weight)
{
    uint32_t result = 0;
    unsigned component;

    for (component = 0; component != 3; ++component)
    {
        int32_t a = r_u8(first + component);
        int32_t b = r_u8(second + component);
        uint32_t value = (uint32_t)sub_8001FC90(weight, b) + (uint32_t)sub_8001FC90(0x10000 - weight, a);

        result |= (value & 0xff) << (8 * component);
    }
    return result;
}

static uint32_t frontier_project_direction(int32_t horizontal, int32_t vertical)
{
    SVECTOR vector;
    sint32 screen;
    sint32 flags;

    horizontal &= 0xfff;
    vertical &= 0xfff;
    vector.vx = (int16_t)r_u16(0x8005624C + 4 * (uint32_t)horizontal);
    vector.vy = -(int16_t)r_u16(0x8005624C + 4 * (uint32_t)vertical);
    vector.vz = (int16_t)r_u16(0x8005624E + 4 * (uint32_t)horizontal);
    vector.pad = 0;
    (void)gte_project(&vector, &screen, &flags);
    return (uint32_t)screen;
}

uint32_t sub_80063C5C(uint32_t player_index)
{
    uint32_t actor = r_u32(0x8005AEC0 + 4 * player_index);
    int32_t heading = (int16_t)r_u16(actor + 124);
    int32_t relative = heading - ((int32_t)(uint32_t)(r_u32(0x80052388)));
    int32_t left = relative - 426;
    int32_t right = relative + 426;
    uint32_t center_screen;
    uint32_t horizon_screen;
    uint32_t selected_color = 0;
    uint32_t mode = 0;
    uint32_t top_left;
    uint32_t top_right;
    uint32_t bottom_left;
    uint32_t bottom_right;
    uint32_t left_horizon;
    uint32_t right_horizon;
    uint32_t left_color;
    uint32_t right_color;
    uint32_t upper_color = ((uint32_t)(r_u32(((uint32_t)(0x800523F8))) & 0x00ffffff));
    uint32_t lower_color = ((uint32_t)(r_u32(((uint32_t)(0x800523FC))) & 0x00ffffff));
    uint32_t ot = r_u32(0x8005B59C) + 12;

    FUNCTION_MARKER(0x80063C5C, "RASHCDG.BIN");
    w_u32(0x800CCD68, (uint32_t)heading);
    center_screen = frontier_project_direction(heading, 0);
    if (left < 0)
        left += 4096;
    if (right < 0)
        right += 4096;
    else if (right >= 4097)
        right -= 4096;
    if (left >= 3073 && right < 1024)
    {
        mode = 1;
        selected_color = 0x800523F0;
        horizon_screen = frontier_project_direction(((int32_t)(uint32_t)(r_u32(0x80052388))), ((int32_t)(uint32_t)(r_u32(0x8005238C))));
    }
    else if (left < 2048 && right >= 2049)
    {
        mode = 2;
        selected_color = 0x800523F4;
        horizon_screen = frontier_project_direction(((int32_t)(uint32_t)(r_u32(0x80052388))) + 2048, ((int32_t)(uint32_t)(r_u32(0x8005238C))));
    }
    else
    {
        horizon_screen = frontier_project_direction(heading - 426, ((int32_t)(uint32_t)(r_u32(0x8005238C))));
    }
    if (left >= 2049)
        left = 4096 - left;
    if (right >= 2049)
        right = 4096 - right;
    left_color = frontier_blend_color(0x800523F0, 0x800523F4, left << 5);
    right_color = frontier_blend_color(0x800523F0, 0x800523F4, right << 5);
    if (r_u32(r_u32(0x8005B2F8) + 48) < 2)
    {
        int16_t bottom = (int16_t)(center_screen >> 16) + 10;

        if (r_u32(0x8005AD24))
            bottom -= (int16_t)r_u16(0x800D513C);
        w_u16(0x800523DA, (uint16_t)bottom);
        w_u16(0x800523DE, (uint16_t)bottom);
    }
    else
    {
        uint32_t viewport = r_u32(0x8005B474) + 8 * player_index;
        int16_t x = (int16_t)r_u16(viewport);
        int16_t y = (int16_t)r_u16(viewport + 2);
        int16_t width = (int16_t)r_u16(viewport + 4);
        int16_t bottom = (int16_t)(center_screen >> 16);

        w_u16(0x800523D0, (uint16_t)x);
        w_u16(0x800523D2, (uint16_t)y);
        w_u16(0x800523D4, (uint16_t)(x + width));
        w_u16(0x800523D6, (uint16_t)y);
        w_u16(0x800523D8, (uint16_t)(x + width));
        w_u16(0x800523DC, (uint16_t)x);
        if (r_u32(0x8005AD24))
            bottom -= (int16_t)r_u16(0x800D513C);
        w_u16(0x800523DA, (uint16_t)bottom);
        w_u16(0x800523DE, (uint16_t)bottom);
    }
    top_left = frontier_pack_pair((int16_t)r_u16(0x800523D0), (int16_t)r_u16(0x800523D2));
    top_right = frontier_pack_pair((int16_t)r_u16(0x800523D4), (int16_t)r_u16(0x800523D6));
    bottom_right = frontier_pack_pair((int16_t)r_u16(0x800523D8), (int16_t)r_u16(0x800523DA));
    bottom_left = frontier_pack_pair((int16_t)r_u16(0x800523DC), (int16_t)r_u16(0x800523DE));
    left_horizon = frontier_pack_pair((int16_t)r_u16(0x800523D0), (int16_t)(horizon_screen >> 16));
    right_horizon = frontier_pack_pair((int16_t)r_u16(0x800523D4), (int16_t)(horizon_screen >> 16));
    if (mode)
    {
        uint32_t selected_upper = ((uint32_t)(r_u32(((uint32_t)(selected_color))) & 0x00ffffff));
        uint32_t left_top = frontier_pack_pair((int16_t)horizon_screen, (int16_t)r_u16(0x800523D2));
        uint32_t left_bottom = frontier_pack_pair((int16_t)horizon_screen, (int16_t)r_u16(0x800523DE));

        (void)sub_800642F8(ot, left_top, top_left, horizon_screen, left_horizon, upper_color, upper_color, selected_upper, left_color);
        (void)sub_800642F8(ot, top_right, left_top, right_horizon, horizon_screen, upper_color, upper_color, right_color, selected_upper);
        (void)sub_800642F8(ot, left_horizon, horizon_screen, bottom_left, left_bottom, left_color, selected_upper, lower_color, lower_color);
        return sub_800642F8(ot, right_horizon, horizon_screen, bottom_right, left_bottom, right_color, selected_upper, lower_color, lower_color);
    }
    (void)sub_800642F8(ot, top_right, top_left, right_horizon, left_horizon, upper_color, upper_color, right_color, left_color);
    return sub_800642F8(ot, left_horizon, right_horizon, bottom_left, bottom_right, left_color, right_color, lower_color, lower_color);
}

static uint32_t frontier_draw_mode_packet(uint32_t link, int dtd)
{
    uint32_t display = r_u32(0x8005B470);
    uint32_t packet = r_u32(display + 268);
    DR_MODE *mode;

    if (packet + 12 >= r_u32(0x8005B4D0))
        packet = sub_80021C98(packet, 12);
    w_u32(display + 268, packet + 12);
    w_u32(packet, r_u32(link));
    w_u32(link, packet);
    mode = (DR_MODE *)xport_guest_ptr(packet, 12);
    SetDrawMode(mode, 0, dtd, 0x120, 0);
    return packet;
}

uint32_t sub_8002C4F8(uint32_t position, uint32_t link, uint32_t player_index)
{
    uint32_t actor = r_u32(0x8005AEC0 + 4 * player_index);
    MATRIX matrix;
    SVECTOR vertex;
    VECTOR transformed;
    sint32 screen;
    sint32 flags;
    int32_t center_x = 192;
    int32_t center_y = 120;
    int32_t offset_x;
    int32_t offset_y;
    int32_t scale;
    uint32_t state = 0x800D7FC8;

    FUNCTION_MARKER(0x8002C4F8, "SLUS_010.53");
    memset(&matrix, 0, sizeof(matrix));
    memcpy(matrix.m, xport_guest_ptr(actor + 92, 18), 18);
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);
    w_u8(state + player_index + 152, 0);
    vertex.vx = (int16_t)r_u16(position);
    vertex.vy = (int16_t)r_u16(position + 2);
    vertex.vz = (int16_t)r_u16(position + 4);
    vertex.pad = (int16_t)r_u16(position + 6);
    gte_transform(&vertex, &transformed, &flags);
    if (transformed.vz < transformed.vx || transformed.vz < -transformed.vx)
        return (uint32_t)-transformed.vx;
    (void)gte_project(&vertex, &screen, &flags);
    if (r_u32(r_u32(0x8005B2F8) + 48) >= 2)
    {
        uint32_t viewport = r_u32(0x8005B474) + 8 * player_index;

        center_x = (int16_t)r_u16(viewport) + (int16_t)r_u16(viewport + 4) / 2;
        center_y = (int16_t)r_u16(viewport + 2) + (int16_t)r_u16(viewport + 6) / 2;
    }
    offset_x = 4 * ((int16_t)screen - center_x);
    offset_y = 4 * ((int16_t)(screen >> 16) - center_y);
    scale = 4 * ((int32_t)(uint32_t)(r_u32(actor + 4)));
    if (scale >= 2897)
        scale = 2896;
    w_u32(state + 32 + 4 * player_index, (uint32_t)screen);
    w_u32(state + 16 * player_index, (uint32_t)offset_x);
    w_u32(state + 16 * player_index + 4, (uint32_t)offset_y);
    w_u32(state + 16 * player_index + 8, (uint32_t)scale);
    if ((int16_t)screen < 381 && !(r_u32(0x80052354) & 4))
    {
        uint8_t quad[16];
        uint8_t color[4];
        int16_t x = (int16_t)screen;
        int16_t y = (int16_t)(screen >> 16);

        xport_store_le32(color, 0x00ffffff);
        xport_store_le32(quad, frontier_pack_pair(x - 2, y - 2));
        xport_store_le32(quad + 4, frontier_pack_pair(x + 3, y - 2));
        xport_store_le32(quad + 8, frontier_pack_pair(x - 2, y + 3));
        xport_store_le32(quad + 12, frontier_pack_pair(x + 3, y + 3));
        (void)sub_8002B080(quad, color, xport_guest_ptr(link, 4));
        w_u8(state + player_index + 152, 1);
    }
    (void)frontier_draw_mode_packet(link, 1);
    if (r_u32(0x80052354) & 1)
    {
        uint8_t point[4];

        xport_store_le32(point, (uint32_t)screen);
        (void)sub_8002B3FC(point, xport_guest_ptr(0x8005233C, 4), 16, 13, 7, xport_guest_ptr(link, 4));
    }
    else
    {
        uint8_t point[4];

        memset(&matrix, 0, sizeof(matrix));
        matrix.m[0][0] = 1024;
        matrix.m[1][1] = 1024;
        matrix.m[2][2] = 1024;
        matrix.t[0] = offset_x;
        matrix.t[1] = offset_y;
        matrix.t[2] = scale;
        SetRotMatrix(&matrix);
        SetTransMatrix(&matrix);
        xport_store_le32(point, (uint32_t)screen);
        (void)sub_8002B878(point, xport_guest_ptr(0x8005233C, 4), xport_guest_ptr(link, 4));
    }
    return frontier_draw_mode_packet(link, 0);
}

uint32_t sub_80021988(int32_t minimum, int32_t maximum)
{
    uint32_t display = r_u32(0x8005B470);
    uint32_t slot = r_u32(0x8005AE00) + 1;
    uint32_t slot_count = r_u8(display + 244);
    uint32_t next;
    int32_t span;
    uint32_t shift;
    uint32_t count;
    uint32_t ot;

    FUNCTION_MARKER(0x80021988, "SLUS_010.53");
    if (slot >= slot_count)
        slot = 0;
    while (r_u32(0x800D75D0 + 64 * slot))
    {
        if (r_u32(0x8005B46C) - r_u32(0x800D75C0 + 4 * slot) >= 61)
            w_u32(0x800D75D0 + 64 * slot, 0);
        else
            rrj_display_wait_step();
    }
    if (maximum < 0)
    {
        shift = r_u32(0x80053224);
        w_u32(0x8005B4D4, 0);
        span = 1920;
    }
    else if (minimum < 0)
    {
        shift = r_u32(0x80053224);
        w_u32(0x8005B4D4, 0);
        span = maximum + 1920;
    }
    else
    {
        int32_t table_index = minimum >> 11;

        if (table_index >= 10)
            table_index = 9;
        shift = r_u32(0x80053224 + 4 * (uint32_t)table_index);
        w_u32(0x8005B4D4, (uint32_t)(minimum - 960));
        span = maximum - minimum + 1920;
    }
    w_u32(0x8005B4D8, shift);
    count = ((uint32_t)span + (1u << shift)) >> shift;
    if (((int32_t)(uint32_t)(r_u32(0x8005B4D4))) < 4096)
        count += 768;
    w_u32(0x8005ADFC, count);
    w_u32(0x8005AE00, slot);
    next = slot + 1;
    if (next >= slot_count)
        next = 0;
    w_u32(0x8005AE04, next);
    w_u32(0x800D75B0 + 4 * slot, r_u32(display + 268) - 60);
    while (!r_u32(0x800D75D0 + 64 * next) && next != slot)
    {
        ++next;
        if (next >= slot_count)
            next = 0;
        w_u32(0x8005AE04, next);
    }
    w_u32(0x800D75D0 + 64 * slot, 1);
    w_u32(0x800D75C0 + 4 * slot, r_u32(0x8005B46C));
    ot = r_u32(display + 248 + 4 * slot);
    w_u32(display + 264, ot);
    if (count >= 1300)
    {
        count = 1299;
        w_u32(0x8005ADFC, count);
    }
    rrj_gpu_clear_ot(ot, count);
    return 0;
}

uint32_t sub_8002AF80(uint32_t matrix)
{
    FUNCTION_MARKER(0x8002AF80, "SLUS_010.53");
    w_u32(matrix, 4096);
    w_u32(matrix + 4, 0);
    w_u32(matrix + 8, 4096);
    w_u32(matrix + 12, 0);
    w_u32(matrix + 16, 4096);
    return 4096;
}

uint32_t sub_8002BAE8(uint32_t first_value, uint32_t second_value, uint32_t ordering_slot)
{
    uint32_t screens[12];
    uint32_t index;

    FUNCTION_MARKER(0x8002BAE8, "SLUS_010.53");
    for (index = 0; index != 12; ++index)
    {
        uint32_t source = 0x800539C8u + 8 * index;
        SVECTOR vertex;
        sint32 screen;
        sint32 flags;

        vertex.vx = (int16_t)r_u16(source);
        vertex.vy = (int16_t)r_u16(source + 2);
        vertex.vz = (int16_t)r_u16(source + 4);
        vertex.pad = (int16_t)r_u16(source + 6);
        (void)gte_project(&vertex, &screen, &flags);
        screens[index] = (uint32_t)screen;
    }
    for (index = 8; index != 0; --index)
    {
        uint32_t item = index - 1;
        uint32_t context = r_u32(0x8005B470u);
        uint32_t packet = r_u32(context + 268);
        uint32_t second_color;
        int32_t vertex_index;

        if (packet + 36u >= r_u32(0x8005B4D0u))
        {
            uint32_t allocation = sub_80021C98(packet, 36);
            context = r_u32(0x8005B470u);
            w_u32(context + 268, allocation);
        }
        context = r_u32(0x8005B470u);
        packet = r_u32(context + 268);
        w_u32(context + 268, packet + 36u);
        w_u32(packet, r_u32(ordering_slot));
        w_u32(ordering_slot, packet);
        w_u32(packet + 4, 0x3A000000u);
        second_color = r_u32(second_value);
        w_u32(packet + 20, 0);
        w_u32(packet + 28, 0);
        w_u8(packet + 3, 8);
        w_u32(packet + 12, second_color);
        vertex_index = r_s16(0x80053960u + 2 * item);
        w_u32(packet + 8, screens[vertex_index]);
        w_u32(packet + 16, r_u32(first_value));
        vertex_index = r_s16(0x80053980u + 2 * item);
        w_u32(packet + 24, screens[vertex_index]);
        vertex_index = r_s16(0x80053970u + 2 * item);
        w_u32(packet + 32, screens[vertex_index]);
    }
    return UINT32_MAX;
}

uint32_t sub_8002BE14(uint32_t point, uint32_t ordering_slot, uint32_t player_index)
{
    int32_t x = (int16_t)r_u16(point);
    int32_t y = (int16_t)r_u16(point + 2);
    int32_t target_x;
    int32_t target_y;
    uint32_t count = 0;
    uint32_t index;
    MATRIX translation;

    FUNCTION_MARKER(0x8002BE14, "SLUS_010.53");
    if (r_u32(r_u32(0x8005B2F8u) + 48) < 2)
    {
        target_x = 384 - x;
        target_y = 240 - y;
    }
    else
    {
        uint32_t viewport = r_u32(0x8005B474u) + 8 * player_index;

        target_x = 2 * (int16_t)r_u16(viewport) + (int16_t)r_u16(viewport + 4) - x;
        target_y = 2 * (int16_t)r_u16(viewport + 2) + (int16_t)r_u16(viewport + 6) - y;
    }
    for (index = 0; index != 25; ++index)
        if ((r_u16(0x800D7FF0u + 52 * player_index + 2 * index) & 0x7FFFu) == 0x7FFFu)
            ++count;
    if (!count)
        return 16 * player_index;

    memset(&translation, 0, sizeof(translation));
    translation.t[0] = ((int32_t)(uint32_t)(r_u32(0x800D7FC8u + 16 * player_index)));
    translation.t[1] = ((int32_t)(uint32_t)(r_u32(0x800D7FCCu + 16 * player_index)));
    translation.t[2] = ((int32_t)(uint32_t)(r_u32(0x800D7FD0u + 16 * player_index)));
    SetTransMatrix(&translation);

    frontier_set_flare_matrix(8 * (x + y), (int32_t)count << 7, (int32_t)count << 6);
    (void)sub_8002BAE8(point, 0x80052340u, ordering_slot);
    frontier_set_flare_matrix(6 * (x + y), (int32_t)count << 6, (int32_t)count << 6);
    (void)sub_8002BAE8(point, 0x80052340u, ordering_slot);

    index = 1;
    if ((r_u32(0x80052354u) & 2u) != 0)
    {
        index = 0;
        w_u32(r_u32(0x800D805Cu) + 8, r_u32(0x80052344u));
    }
    while (index < 9)
    {
        uint32_t record = r_u32(0x800D805Cu) + 16 * index;
        int32_t weight = ((int32_t)(uint32_t)(r_u32(record)));
        int32_t width = ((int32_t)(uint32_t)(r_u32(0x800D8058u) * r_u32(record + 4))) / 65536;
        int32_t height = ((int32_t)(uint32_t)((uint32_t)(3413 * width))) >> 12;

        if (width >= 4)
        {
            uint8_t xy[4];
            uint8_t color[4] = {0, 0, 0, 0};
            int32_t screen_x = ((0x10000 - weight) * x + weight * target_x) / 65536;
            int32_t screen_y = ((0x10000 - weight) * y + weight * target_y) / 65536;
            uint32_t kind = r_u8(record + 12);
            void *link = xport_guest_ptr(ordering_slot, 4);

            xport_store_le16(xy, (uint16_t)screen_x);
            xport_store_le16(xy + 2, (uint16_t)screen_y);
            color[0] = (uint8_t)(r_u8(record + 8) * count / 25);
            color[1] = (uint8_t)(r_u8(record + 9) * count / 25);
            color[2] = (uint8_t)(r_u8(record + 10) * count / 25);
            if (kind == 0)
                (void)sub_8002B3FC(xy, color, (uint32_t)width, (uint32_t)height, 7, link);
            else if (kind == 1)
                (void)sub_8002B3FC(xy, color, (uint32_t)width, (uint32_t)height, 6, link);
            else if (kind == 2)
                (void)sub_8002B4A0(xy, color, (uint32_t)width, (uint32_t)height, link);
            else if (kind == 3)
                (void)sub_8002B5B4(xy, color, (uint32_t)width, (uint32_t)height, link);
            else if (kind == 4)
                (void)sub_8002B68C(xy, color, (uint32_t)width, (uint32_t)height, link);
        }
        ++index;
    }
    {
        uint32_t context = r_u32(0x8005B470u);
        uint32_t packet = r_u32(context + 268);
        uint32_t link;

        if (packet + 12 >= r_u32(0x8005B4D0u))
        {
            packet = sub_80021C98(packet, 12);
            w_u32(context + 268, packet);
        }
        w_u32(context + 268, packet + 12);
        link = r_u32(ordering_slot);
        w_u32(packet, (link & 0xFFFFFFu) | 0x02000000u);
        w_u32(ordering_slot, packet);
        w_u32(packet + 4, 0xE1000120u);
        w_u32(packet + 8, 0);
        return 0xE1000120u;
    }
}

uint32_t sub_8001034C(uint32_t matrix_address, uint32_t count, uint32_t vertices, uint32_t records, uint32_t screens, uint32_t clips)
{
    FUNCTION_MARKER(0x8001034C, "SLUS_010.53");
    uint32_t first = r_u32(matrix_address);
    uint32_t second;
    uint32_t index;
    uint32_t end;
    if (!count)
        return 0;
    for (index = 0; index < 8; index += 2)
    {
        second = r_u32(matrix_address + 4 * index + 4);
        xport_gte_write_control(index, first);
        xport_gte_write_control(index + 1, second);
        if (index != 6)
            first = r_u32(matrix_address + 4 * index + 8);
    }
    first = r_u32(vertices);
    second = r_u32(vertices + 4);
    end = clips + count;
    do
    {
        uint32_t depth;
        uint32_t screen;
        uint32_t classification;
        int32_t x;
        int32_t y;
        xport_gte_write_data(0, first);
        xport_gte_write_data(1, second);
        vertices += 8;
        screens += 4;
        xport_gte_execute(0x180001);
        first = r_u32(vertices);
        second = r_u32(vertices + 4);
        records += 16;
        depth = xport_gte_read_data(27);
        screen = xport_gte_read_data(14);
        w_u32(records - 8, depth);
        w_u32(screens - 4, screen);
        classification = ((int32_t)depth < 1024 ? 32 : 0) | ((int32_t)depth < 40 ? 16 : 0);
        y = (int32_t)screen >> 16;
        x = (int16_t)screen;
        classification |= y > 240 ? 8 : 0;
        classification |= ((uint32_t)x > 384) + ((uint32_t)x >> 31);
        ++clips;
        w_u8(clips - 1, (uint8_t)classification);
    } while (clips != end);
    return 0;
}

uint32_t sub_800104C8(uint32_t matrix_address, uint32_t count, uint32_t vertices, uint32_t records, uint32_t screens)
{
    MATRIX matrix;
    uint32_t index;

    FUNCTION_MARKER(0x800104C8, "SLUS_010.53");
    if (!count)
        return 0;
    memcpy(&matrix, xport_guest_ptr(matrix_address, sizeof(matrix)), sizeof(matrix));
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);
    for (index = 0; index != count; ++index)
    {
        uint32_t source = vertices + 8 * index;
        SVECTOR vertex;
        VECTOR transformed;
        sint32 screen;
        sint32 flags;

        vertex.vx = (int16_t)r_u16(source);
        vertex.vy = (int16_t)r_u16(source + 2);
        vertex.vz = (int16_t)r_u16(source + 4);
        vertex.pad = (int16_t)r_u16(source + 6);
        gte_transform(&vertex, &transformed, &flags);
        (void)gte_project(&vertex, &screen, &flags);
        w_u32(screens + 4 * index, (uint32_t)screen);
        w_u32(records + 16 * index + 8, (uint32_t)transformed.vz);
    }
    return 0;
}

uint32_t sub_80068D50(uint32_t object, uint32_t entry_index)
{
    uint32_t container = r_u32(object);
    uint32_t entry = r_u32(container + 36) + 8 * entry_index;
    uint32_t start = r_u32(entry);
    uint32_t count = r_u32(entry + 4);
    uint32_t index = start >> 3;
    uint32_t records = r_u32(0x8005ACB0) + 16 * index;
    uint32_t screens = r_u32(0x8005ACB4) + 4 * index;
    uint32_t clips = r_u32(0x8005ACB8) + index;
    uint32_t vertices = r_u32(container + 52) + 8 * index + 4;
    uint32_t result;

    FUNCTION_MARKER(0x80068D50, "RASHCDG.BIN");
    w_u32(0x800CCDB0, start);
    w_u32(0x800CCDB4, count);
    w_u32(0x800CCDB8, records);
    w_u32(0x800CCDAC, screens);
    w_u32(0x800CCDBC, clips);
    w_u32(0x800CCDC0, vertices);
    (void)sub_8001034C(r_u32(0x800CC870), count, vertices, records, screens, clips);
    result = r_u32(0x800CCDA8) + count;
    w_u32(0x800CCDA8, result);
    return result;
}

uint32_t sub_80068E2C(uint32_t object, uint32_t entry_index)
{
    uint32_t container = r_u32(object);
    uint32_t entry = r_u32(container + 36) + 8 * entry_index;
    uint32_t start = r_u32(entry);
    uint32_t count = r_u32(entry + 4);
    uint32_t index = start >> 3;
    uint32_t vertices = r_u32(container + 52) + 8 * index + 4;
    uint32_t records = r_u32(0x8005ACB0) + 16 * index;
    uint32_t screens = r_u32(0x8005ACB4) + 4 * index;
    uint32_t result;

    FUNCTION_MARKER(0x80068E2C, "RASHCDG.BIN");
    (void)sub_800104C8(r_u32(0x800CC870), count, vertices, records, screens);
    result = r_u32(0x800CCDA8) + count;
    w_u32(0x800CCDA8, result);
    return result;
}

uint32_t sub_80068EB8(uint32_t object)
{
    uint32_t entry_index = 1;
    uint32_t shift = 0;
    uint32_t flags = r_u32(object + 52);
    uint32_t entry_count = r_u8(object + 68);
    int32_t maximum;
    int32_t total;

    FUNCTION_MARKER(0x80068EB8, "RASHCDG.BIN");
    w_u32(0x800CCDA8, 0);
    (void)sub_80068D50(object, 0);
    if ((flags & 0x22222222u) != 0 && entry_count > 1)
    {
        while (entry_index < entry_count)
        {
            uint32_t mode = (flags >> (shift & 31)) & (r_u32(0x800CC86C) != 0 ? 0xDu : 0xFu);
            if ((mode & 2) != 0)
            {
                if ((mode & 4) != 0)
                    (void)sub_80068D50(object, entry_index);
                else
                    (void)sub_80068E2C(object, entry_index);
            }
            ++entry_index;
            shift += 4;
        }
    }
    maximum = (int32_t)r_u32(0x8005B300);
    total = (int32_t)r_u32(0x800CCDA8);
    if (maximum < total)
        w_u32(0x8005B300, (uint32_t)total);
    return maximum < total;
}

uint32_t sub_80069200(uint32_t object)
{
    uint32_t matrix = r_u32(0x800CC870u) + 32;
    uint32_t index;

    FUNCTION_MARKER(0x80069200, "RASHCDG.BIN");
    w_u32(0x800CC870u, matrix);
    for (index = 0; index != 8; ++index)
        w_u32(matrix + 4 * index, r_u32(object + 12 + 4 * index));
    (void)sub_80068EB8(object);
    w_u32(0x800CC870u, r_u32(0x800CC870u) - 32);
    (void)sub_80068FCC(object);
    return 1;
}

uint32_t sub_80035958(uint32_t player_index)
{
    uint32_t far_indices[12];
    uint32_t near_indices[12];
    uint32_t far_count = 0;
    uint32_t near_count = 0;
    int32_t far_minimum = 0x4000;
    int32_t near_minimum = 0x4000;
    int32_t far_maximum = 0;
    int32_t near_maximum = 0;
    uint32_t object_count = r_u32(0x8005B568u + 4 * player_index);
    uint32_t synchronized = 0;
    uint32_t index;
    uint32_t result = object_count;

    FUNCTION_MARKER(0x80035958, "SLUS_010.53");
    if (!object_count)
        return result;
    (void)sub_80032D24(player_index);
    /* Reload the prepared count as in the MIPS load at 800359C4 */
    object_count = r_u32(0x8005B568u + 4 * player_index);
    result = object_count;
    for (index = object_count; index != 0; --index)
    {
        int32_t object_index = ((int32_t)(uint32_t)(r_u32(0x800D9B80u + 48 * player_index + 4 * (index - 1))));
        uint32_t object;
        int32_t depth;
        int32_t distance;

        if (object_index == -1)
            continue;
        object = 0x800D87E8u + 112 * (uint32_t)object_index;
        depth = ((int32_t)(uint32_t)(r_u32(object + 48)));
        if (depth <= 0)
            continue;
        distance = ((int32_t)(uint32_t)(r_u32(object + 52)));
        if ((r_u32(object + 56) & 0x22222222u) != 0 && distance < 2048 && depth < 0x4000)
        {
            near_indices[near_count++] = (uint32_t)object_index;
            if (distance < near_minimum)
                near_minimum = distance;
            if (depth > near_maximum)
                near_maximum = depth;
        }
        else
        {
            far_indices[far_count++] = (uint32_t)object_index;
            if (distance < far_minimum)
                far_minimum = distance;
            if (depth > far_maximum)
                far_maximum = depth;
        }
    }
    if (far_count != 0)
    {
        (void)sub_80021988(far_minimum, far_maximum);
        (void)frontier_prepare_object_indices(far_indices, (int32_t)far_count, player_index);
        for (index = 0; index != far_count; ++index)
        {
            frontier_configure_model_scratch();
            (void)sub_80069200(0x800D87ECu + 112 * far_indices[index]);
        }
        if (player_index == 0)
        {
#if defined(LOCKSTEP_DEBUG)
            rrj_lockstep_object_wait(player_index, 1u, far_count, far_indices, near_count, near_indices, near_minimum, near_maximum);
#endif
            (void)sub_8001C408();
#if defined(LOCKSTEP_DEBUG)
            rrj_lockstep_object_wait_end();
#endif
            synchronized = 1;
            if (r_u32(0x8005B588u) == 1)
            {
                w_u32(0x8005B588u, 2);
                (void)DrawOTag((uint32 *)xport_guest_ptr(r_u32(0x8005B59Cu) + 16, sizeof(uint32)));
            }
        }
        (void)DrawOTag((uint32 *)xport_guest_ptr(r_u32(r_u32(0x8005B470u) + 264) + 4 * r_u32(0x8005ADFCu) - 4, sizeof(uint32)));
        {
            PSX_RECT *rectangle = (PSX_RECT *)xport_guest_ptr(0x8005B398u, sizeof(PSX_RECT));
            uint32_t destination = 0x800D75D0u + 64u * r_u32(0x8005AE00u);
            size_t bytes = rectangle->w > 0 && rectangle->h > 0 ? (size_t)rectangle->w * rectangle->h * 2u : 0;
            result = (uint32_t)StoreImage(rectangle, (uint32 *)xport_guest_ptr(destination, bytes));
        }
    }
    if (near_count != 0)
    {
        (void)sub_80021988(near_minimum, near_maximum);
        (void)frontier_prepare_object_indices(near_indices, (int32_t)near_count, player_index);
        for (index = 0; index != near_count; ++index)
        {
            frontier_configure_model_scratch();
            (void)sub_80069200(0x800D87ECu + 112 * near_indices[index]);
        }
        if (player_index == 0 && !synchronized)
        {
#if defined(LOCKSTEP_DEBUG)
            rrj_lockstep_object_wait(player_index, 2u, far_count, far_indices, near_count, near_indices, near_minimum, near_maximum);
#endif
            (void)sub_8001C408();
#if defined(LOCKSTEP_DEBUG)
            rrj_lockstep_object_wait_end();
#endif
            if (r_u32(0x8005B588u) == 1)
            {
                w_u32(0x8005B588u, 2);
                (void)DrawOTag((uint32 *)xport_guest_ptr(r_u32(0x8005B59Cu) + 16, sizeof(uint32)));
            }
        }
        (void)DrawOTag((uint32 *)xport_guest_ptr(r_u32(r_u32(0x8005B470u) + 264) + 4 * r_u32(0x8005ADFCu) - 4, sizeof(uint32)));
        {
            PSX_RECT *rectangle = (PSX_RECT *)xport_guest_ptr(0x8005B398u, sizeof(PSX_RECT));
            uint32_t destination = 0x800D75D0u + 64u * r_u32(0x8005AE00u);
            size_t bytes = rectangle->w > 0 && rectangle->h > 0 ? (size_t)rectangle->w * rectangle->h * 2u : 0;
            result = (uint32_t)StoreImage(rectangle, (uint32 *)xport_guest_ptr(destination, bytes));
        }
    }
    return result;
}

/* Continue at 80035C0C after the current far model returns */
uint32_t rrj_resume_sub_80035958(const RRJObjectContinuation *phase)
{
    uint32_t player_index;
    uint32_t far_count;
    uint32_t near_count;
    uint32_t synchronized;
    const uint32_t *far_indices;
    const uint32_t *near_indices;
    int32_t near_minimum;
    int32_t near_maximum;
    uint32_t index;
    uint32_t result = 0;

    if (phase == NULL || phase->far_count == 0 || phase->far_count > 12 || phase->near_count > 12 || phase->next_far_index > phase->far_count)
        abort();
    player_index = phase->player_index;
    far_count = phase->far_count;
    near_count = phase->near_count;
    synchronized = phase->synchronized;
    far_indices = phase->far_indices;
    near_indices = phase->near_indices;
    near_minimum = phase->near_minimum;
    near_maximum = phase->near_maximum;
    for (index = phase->next_far_index; index != far_count; ++index)
    {
        frontier_configure_model_scratch();
        (void)sub_80069200(0x800D87ECu + 112 * far_indices[index]);
    }
    if (player_index == 0 && !synchronized)
    {
#if defined(LOCKSTEP_DEBUG)
        rrj_lockstep_object_wait(player_index, 1u, far_count, far_indices, near_count, near_indices, near_minimum, near_maximum);
#endif
        (void)sub_8001C408();
#if defined(LOCKSTEP_DEBUG)
        rrj_lockstep_object_wait_end();
#endif
        synchronized = 1;
        if (r_u32(0x8005B588u) == 1)
        {
            w_u32(0x8005B588u, 2);
            (void)DrawOTag((uint32 *)xport_guest_ptr(r_u32(0x8005B59Cu) + 16, sizeof(uint32)));
        }
    }
    (void)DrawOTag((uint32 *)xport_guest_ptr(r_u32(r_u32(0x8005B470u) + 264) + 4 * r_u32(0x8005ADFCu) - 4, sizeof(uint32)));
    {
        PSX_RECT *rectangle = (PSX_RECT *)xport_guest_ptr(0x8005B398u, sizeof(PSX_RECT));
        uint32_t destination = 0x800D75D0u + 64u * r_u32(0x8005AE00u);
        size_t bytes = rectangle->w > 0 && rectangle->h > 0 ? (size_t)rectangle->w * rectangle->h * 2u : 0;
        result = (uint32_t)StoreImage(rectangle, (uint32 *)xport_guest_ptr(destination, bytes));
    }

    if (near_count != 0)
    {
        (void)sub_80021988(near_minimum, near_maximum);
        (void)frontier_prepare_object_indices(near_indices, (int32_t)near_count, player_index);
        for (index = 0; index != near_count; ++index)
        {
            frontier_configure_model_scratch();
            (void)sub_80069200(0x800D87ECu + 112 * near_indices[index]);
        }
        if (player_index == 0 && !synchronized)
        {
#if defined(LOCKSTEP_DEBUG)
            rrj_lockstep_object_wait(player_index, 2u, far_count, far_indices, near_count, near_indices, near_minimum, near_maximum);
#endif
            (void)sub_8001C408();
#if defined(LOCKSTEP_DEBUG)
            rrj_lockstep_object_wait_end();
#endif
            if (r_u32(0x8005B588u) == 1)
            {
                w_u32(0x8005B588u, 2);
                (void)DrawOTag((uint32 *)xport_guest_ptr(r_u32(0x8005B59Cu) + 16, sizeof(uint32)));
            }
        }
        (void)DrawOTag((uint32 *)xport_guest_ptr(r_u32(r_u32(0x8005B470u) + 264) + 4 * r_u32(0x8005ADFCu) - 4, sizeof(uint32)));
        {
            PSX_RECT *rectangle = (PSX_RECT *)xport_guest_ptr(0x8005B398u, sizeof(PSX_RECT));
            uint32_t destination = 0x800D75D0u + 64u * r_u32(0x8005AE00u);
            size_t bytes = rectangle->w > 0 && rectangle->h > 0 ? (size_t)rectangle->w * rectangle->h * 2u : 0;
            result = (uint32_t)StoreImage(rectangle, (uint32 *)xport_guest_ptr(destination, bytes));
        }
    }
    return result;
}

#if defined(LOCKSTEP_DEBUG)
uint32_t rrj_resume_sub_80035958_wait(const RRJObjectContinuation *phase, uint32_t stage)
{
    uint32_t player_index;
    uint32_t far_count;
    uint32_t near_count;
    uint32_t synchronized;
    const uint32_t *far_indices;
    const uint32_t *near_indices;
    int32_t near_minimum;
    int32_t near_maximum;
    uint32_t index;
    uint32_t result = 0;

    if (!phase || phase->far_count > 12 || phase->near_count > 12 || (stage != 1 && stage != 2) || (stage == 2 && !phase->near_count))
        abort();
    player_index = phase->player_index;
    far_count = phase->far_count;
    near_count = phase->near_count;
    synchronized = phase->synchronized;
    far_indices = phase->far_indices;
    near_indices = phase->near_indices;
    near_minimum = phase->near_minimum;
    near_maximum = phase->near_maximum;
    if (stage == 2)
        goto near_wait;
    if (player_index == 0 && !synchronized)
    {
    #if defined(LOCKSTEP_DEBUG)
        rrj_lockstep_object_wait(player_index, 1u, far_count, far_indices, near_count, near_indices, near_minimum, near_maximum);
    #endif
        rrj_lockstep_object_wait_resume();
    #if defined(LOCKSTEP_DEBUG)
        rrj_lockstep_object_wait_end();
    #endif
        synchronized = 1;
        if (r_u32(0x8005B588u) == 1)
        {
            w_u32(0x8005B588u, 2);
            (void)DrawOTag((uint32 *)xport_guest_ptr(r_u32(0x8005B59Cu) + 16, sizeof(uint32)));
        }
    }
    (void)DrawOTag((uint32 *)xport_guest_ptr(r_u32(r_u32(0x8005B470u) + 264) + 4 * r_u32(0x8005ADFCu) - 4, sizeof(uint32)));
    {
        PSX_RECT *rectangle = (PSX_RECT *)xport_guest_ptr(0x8005B398u, sizeof(PSX_RECT));
        uint32_t destination = 0x800D75D0u + 64u * r_u32(0x8005AE00u);
        size_t bytes = rectangle->w > 0 && rectangle->h > 0 ? (size_t)rectangle->w * rectangle->h * 2u : 0;
        result = (uint32_t)StoreImage(rectangle, (uint32 *)xport_guest_ptr(destination, bytes));
    }

    if (near_count != 0)
    {
        (void)sub_80021988(near_minimum, near_maximum);
        (void)frontier_prepare_object_indices(near_indices, (int32_t)near_count, player_index);
        for (index = 0; index != near_count; ++index)
        {
            frontier_configure_model_scratch();
            (void)sub_80069200(0x800D87ECu + 112 * near_indices[index]);
        }
    near_wait:
        if (player_index == 0 && !synchronized)
        {
    #if defined(LOCKSTEP_DEBUG)
            rrj_lockstep_object_wait(player_index, 2u, far_count, far_indices, near_count, near_indices, near_minimum, near_maximum);
    #endif
            rrj_lockstep_object_wait_resume();
    #if defined(LOCKSTEP_DEBUG)
            rrj_lockstep_object_wait_end();
    #endif
            if (r_u32(0x8005B588u) == 1)
            {
                w_u32(0x8005B588u, 2);
                (void)DrawOTag((uint32 *)xport_guest_ptr(r_u32(0x8005B59Cu) + 16, sizeof(uint32)));
            }
        }
        (void)DrawOTag((uint32 *)xport_guest_ptr(r_u32(r_u32(0x8005B470u) + 264) + 4 * r_u32(0x8005ADFCu) - 4, sizeof(uint32)));
        {
            PSX_RECT *rectangle = (PSX_RECT *)xport_guest_ptr(0x8005B398u, sizeof(PSX_RECT));
            uint32_t destination = 0x800D75D0u + 64u * r_u32(0x8005AE00u);
            size_t bytes = rectangle->w > 0 && rectangle->h > 0 ? (size_t)rectangle->w * rectangle->h * 2u : 0;
            result = (uint32_t)StoreImage(rectangle, (uint32 *)xport_guest_ptr(destination, bytes));
        }
    }
    return result;
}
#endif

uint32_t sub_8006929C(uint32_t base_index, uint32_t packed_indices, uint32_t depth_shift)
{
    static const uint32_t first_shifts[3] = {8, 16, 0};
    static const uint32_t second_shifts[3] = {0, 8, 16};
    uint8_t indices[6];
    uint32_t midpoint, triangle;

    FUNCTION_MARKER(0x8006929C, "RASHCDG.BIN");
    for (midpoint = 0; midpoint != 3; ++midpoint)
    {
        uint32_t first = 0x1F800100 + 32 * ((packed_indices >> first_shifts[midpoint]) & 255);
        uint32_t second = 0x1F800100 + 32 * ((packed_indices >> second_shifts[midpoint]) & 255);
        uint32_t output = 0x1F800100 + 32 * (base_index + midpoint);
        uint32_t first_color = r_u32(first + 16), second_color = r_u32(second + 16);
        uint32_t depth, x, y, z, scale, first_screen, second_screen;
        uint32_t delta_x, delta_y, sign_x, sign_y, abs_x, abs_y, twice_scale;
        uint32_t x_adjust, y_adjust, screen, clip;
        int32_t screen_y;

        w_u32(output + 16, (((first_color & 0xFFFFFEFF) + (second_color & 0xFFFFFEFF)) >> 1) +
              ((first_color & second_color) & 256));
        depth = frontier_asr(r_u32(first + 20) + r_u32(second + 20), 1);
        w_u32(output + 20, depth);
        w_u32(output + 28, (r_u32(output + 28) & 0xFF000000) |
              (r_u32(0x800D4CA8 + 4 * (depth & 255)) & 0xFFFFFF));
        x = frontier_asr(r_u32(first) + r_u32(second), 1);
        w_u32(output, x);
        y = frontier_asr(r_u32(first + 4) + r_u32(second + 4), 1);
        w_u32(output + 4, y);
        z = frontier_asr(r_u32(first + 8) + r_u32(second + 8), 1);
        w_u32(output + 8, z);
        scale = frontier_asr(z + 128, 8);
        twice_scale = scale << 1;
        first_screen = r_u32(first + 24);
        second_screen = r_u32(second + 24);
        delta_x = (uint32_t)((int32_t)(int16_t)first_screen - (int32_t)(int16_t)second_screen);
        delta_y = (uint32_t)((int32_t)(int16_t)(first_screen >> 16) -
                             (int32_t)(int16_t)(second_screen >> 16));
        sign_x = frontier_asr(delta_x, 31);
        sign_y = frontier_asr(delta_y, 31);
        abs_x = (delta_x + sign_x) ^ sign_x;
        abs_y = (delta_y + sign_y) ^ sign_y;
        y_adjust = ((twice_scale & sign_x) - scale) & frontier_asr(abs_y - (abs_x << 1), 31);
        x_adjust = (scale + ((0 - twice_scale) & sign_y)) & frontier_asr(abs_x - (abs_y << 1), 31);
        y = r_u32(output + 4) + y_adjust;
        z = r_u32(output + 8);
        w_u32(output + 4, y);
        x = r_u32(output) + x_adjust;
        w_u32(output, x);
        xport_gte_write_control(5, frontier_asr(x, 5));
        xport_gte_write_control(6, frontier_asr(y, 5));
        xport_gte_write_control(7, frontier_asr(z, 5));
        xport_gte_execute(0x198001);
        w_u32(output + 24, xport_gte_read_data(14));
        screen = r_u32(output + 24);
        z = r_u32(output + 8);
        clip = (int32_t)z > 102399 ? 32 : (int32_t)z > 51199 ? 64 : 128;
        if ((int32_t)z < 10240)
            clip |= 16;
        screen_y = (int32_t)screen >> 16;
        if (screen_y < 0)
            clip |= 8;
        if (screen_y >= 241)
            clip |= 4;
        clip |= (screen & 65535) < 385 ? (screen >> 15) & 1 : ((screen >> 15) & 1) + 1;
        w_u8(output + 31, (uint8_t)clip);
    }

    indices[0] = (uint8_t)packed_indices;
    indices[1] = (uint8_t)(packed_indices >> 8);
    indices[2] = (uint8_t)(packed_indices >> 16);
    indices[3] = (uint8_t)base_index;
    indices[4] = (uint8_t)(base_index + 1);
    indices[5] = (uint8_t)(base_index + 2);
    ++depth_shift;
    for (triangle = 0; triangle != 4; ++triangle)
    {
        uint32_t selectors = r_u32(0x800CCA30 + 4 * triangle);
        uint32_t first_index = indices[selectors >> 8];
        uint32_t second_index = indices[(selectors >> 4) & 15];
        uint32_t third_index = indices[selectors & 15];
        uint32_t first = 0x1F800100 + 32 * first_index;
        uint32_t second = 0x1F800100 + 32 * second_index;
        uint32_t third = 0x1F800100 + 32 * third_index;
        uint32_t first_clip = r_u8(first + 31);
        uint32_t second_clip = r_u8(second + 31);
        uint32_t third_clip = r_u8(third + 31);
        uint32_t combined = first_clip | second_clip | third_clip;

        if (first_clip & second_clip & third_clip & 31)
            continue;
        if ((combined >> (depth_shift & 31)) || ((int32_t)base_index < 12 && (combined & 16)))
            (void)sub_8006929C(base_index + 3, first_index | (second_index << 8) |
                              (third_index << 16), depth_shift);
        else
        {
            uint32_t packet = r_u32(0x1F800020);
            uint32_t screen, color;
            if (packet + 40 >= r_u32(0x8005B4D0))
            {
                packet = sub_80021C98(packet, 40);
                w_u32(0x1F800020, packet);
            }
            w_u32(packet, r_u32(0x1F800028) | 0x09000000);
            w_u32(packet + 4, (r_u32(first + 28) & 0xFFFFFF) | 0x34000000);
            w_u32(packet + 8, r_u32(first + 24));
            w_u32(packet + 12, r_u32(first + 16) | r_u32(0x1F800034));
            w_u32(packet + 16, r_u32(second + 28) & 0xFFFFFF);
            w_u32(packet + 20, r_u32(second + 24));
            w_u32(packet + 24, r_u32(second + 16) | r_u32(0x1F800038));
            w_u32(packet + 28, r_u32(third + 28) & 0xFFFFFF);
            screen = r_u32(third + 24);
            w_u32(0x1F800028, packet);
            w_u32(packet + 32, screen);
            color = r_u32(third + 16);
            w_u32(0x1F800020, packet + 40);
            w_u32(packet + 36, color);
        }
    }
    return 0;
}

uint32_t sub_80069784(uint32_t base_index, uint32_t packed_indices, uint32_t depth_shift)
{
    static const uint8_t first_selectors[5] = {0, 3, 2, 1, 4};
    static const uint8_t second_selectors[5] = {3, 2, 1, 0, 5};
    uint8_t indices[9];
    int32_t midpoint;
    uint32_t triangle;

    FUNCTION_MARKER(0x80069784, "RASHCDG.BIN");
    indices[0] = (uint8_t)packed_indices;
    indices[1] = (uint8_t)(packed_indices >> 8);
    indices[2] = (uint8_t)(packed_indices >> 16);
    indices[3] = (uint8_t)(packed_indices >> 24);
    indices[4] = (uint8_t)(base_index + 1);
    indices[5] = (uint8_t)(base_index + 3);
    for (midpoint = 4; midpoint >= 0; --midpoint)
        frontier_build_midpoint(base_index + (uint32_t)midpoint, indices[first_selectors[4 - midpoint]], indices[second_selectors[4 - midpoint]], 128, midpoint != 0);

    indices[4] = (uint8_t)base_index;
    indices[5] = (uint8_t)(base_index + 1);
    indices[6] = (uint8_t)(base_index + 2);
    indices[7] = (uint8_t)(base_index + 3);
    indices[8] = (uint8_t)(base_index + 4);
    ++depth_shift;
    for (triangle = 0; triangle != 4; ++triangle)
    {
        uint32_t selectors = r_u32(0x800CCA40u + 4 * triangle);
        uint32_t first_index = indices[selectors >> 12];
        uint32_t second_index = indices[(selectors >> 8) & 0xFu];
        uint32_t third_index = indices[(selectors >> 4) & 0xFu];
        uint32_t fourth_index = indices[selectors & 0xFu];
        uint32_t first = 0x1F800100u + 32 * first_index;
        uint32_t second = 0x1F800100u + 32 * second_index;
        uint32_t third = 0x1F800100u + 32 * third_index;
        uint32_t fourth = 0x1F800100u + 32 * fourth_index;
        uint32_t first_clip = r_u8(first + 31);
        uint32_t second_clip = r_u8(second + 31);
        uint32_t third_clip = r_u8(third + 31);
        uint32_t fourth_clip = r_u8(fourth + 31);
        uint32_t combined = first_clip | second_clip | third_clip | fourth_clip;

        if ((first_clip & second_clip & third_clip & fourth_clip & 0x1Fu) != 0)
            continue;
        if ((combined >> (depth_shift & 31u)) != 0 || (((int32_t)(uint32_t)(base_index)) < 19 && (combined & 0x10u) != 0))
        {
            (void)sub_80069784(base_index + 5, first_index | (second_index << 8) | (third_index << 16) | (fourth_index << 24), depth_shift);
        }
        else
        {
            uint32_t packet = r_u32(0x1F800020u);
            if (packet + 52u >= r_u32(0x8005B4D0u))
            {
                packet = sub_80021C98(packet, 52);
                w_u32(0x1F800020u, packet);
            }
            w_u32(packet, r_u32(0x1F800028u) | 0x0C000000u);
            w_u32(packet + 4, (r_u32(first + 28) & 0xFFFFFFu) | 0x3C000000u);
            w_u32(packet + 8, r_u32(first + 24));
            w_u32(packet + 12, r_u32(first + 16) | r_u32(0x1F800034u));
            w_u32(packet + 16, r_u32(second + 28) & 0xFFFFFFu);
            w_u32(packet + 20, r_u32(second + 24));
            w_u32(packet + 24, r_u32(second + 16) | r_u32(0x1F800038u));
            w_u32(packet + 28, r_u32(fourth + 28) & 0xFFFFFFu);
            w_u32(packet + 32, r_u32(fourth + 24));
            w_u32(packet + 36, r_u32(fourth + 16));
            w_u32(packet + 40, r_u32(third + 28) & 0xFFFFFFu);
            {
                uint32_t screen = r_u32(third + 24);
                uint32_t uv;
                w_u32(0x1F800028u, packet);
                w_u32(packet + 44, screen);
                uv = r_u32(third + 16);
                w_u32(0x1F800020u, packet + 52);
                w_u32(packet + 48, uv);
            }
        }
    }
    return 0;
}

uint32_t sub_80069CF0(uint32_t base_index, uint32_t packed_indices, uint32_t depth_shift)
{
    static const uint8_t first_selectors[5] = {0, 3, 2, 1, 4};
    static const uint8_t second_selectors[5] = {3, 2, 1, 0, 5};
    uint8_t indices[9];
    int32_t midpoint;
    uint32_t triangle;

    FUNCTION_MARKER(0x80069CF0, "RASHCDG.BIN");
    indices[0] = (uint8_t)packed_indices;
    indices[1] = (uint8_t)(packed_indices >> 8);
    indices[2] = (uint8_t)(packed_indices >> 16);
    indices[3] = (uint8_t)(packed_indices >> 24);
    indices[4] = (uint8_t)(base_index + 1);
    indices[5] = (uint8_t)(base_index + 3);
    for (midpoint = 4; midpoint >= 0; --midpoint)
        frontier_build_midpoint(base_index + (uint32_t)midpoint, indices[first_selectors[4 - midpoint]], indices[second_selectors[4 - midpoint]], 64, midpoint != 0);

    indices[4] = (uint8_t)base_index;
    indices[5] = (uint8_t)(base_index + 1);
    indices[6] = (uint8_t)(base_index + 2);
    indices[7] = (uint8_t)(base_index + 3);
    indices[8] = (uint8_t)(base_index + 4);
    ++depth_shift;
    for (triangle = 0; triangle != 4; ++triangle)
    {
        uint32_t selectors = r_u32(0x800CCA40u + 4 * triangle);
        uint32_t first_index = indices[selectors >> 12];
        uint32_t second_index = indices[(selectors >> 8) & 0xFu];
        uint32_t third_index = indices[(selectors >> 4) & 0xFu];
        uint32_t fourth_index = indices[selectors & 0xFu];
        uint32_t first = 0x1F800100u + 32 * first_index;
        uint32_t second = 0x1F800100u + 32 * second_index;
        uint32_t third = 0x1F800100u + 32 * third_index;
        uint32_t fourth = 0x1F800100u + 32 * fourth_index;
        uint32_t first_clip = r_u8(first + 31);
        uint32_t second_clip = r_u8(second + 31);
        uint32_t third_clip = r_u8(third + 31);
        uint32_t fourth_clip = r_u8(fourth + 31);
        uint32_t combined = first_clip | second_clip | third_clip | fourth_clip;

        if ((first_clip & second_clip & third_clip & fourth_clip & 0x1Fu) != 0)
            continue;
        if ((combined >> (depth_shift & 31u)) != 0 || (((int32_t)(uint32_t)(base_index)) < 14 && (combined & 0x10u) != 0))
        {
            (void)sub_80069CF0(base_index + 5, first_index | (second_index << 8) | (third_index << 16) | (fourth_index << 24), depth_shift);
        }
        else
        {
            uint32_t packet = r_u32(0x1F800020u);
            if (packet + 52u >= r_u32(0x8005B4D0u))
            {
                packet = sub_80021C98(packet, 52);
                w_u32(0x1F800020u, packet);
            }
            w_u32(packet, r_u32(0x1F800028u) | 0x0C000000u);
            w_u32(packet + 4, (r_u32(first + 28) & 0xFFFFFFu) | 0x3C000000u);
            w_u32(packet + 8, r_u32(first + 24));
            w_u32(packet + 12, r_u32(first + 16) | r_u32(0x1F800034u));
            w_u32(packet + 16, r_u32(second + 28) & 0xFFFFFFu);
            w_u32(packet + 20, r_u32(second + 24));
            w_u32(packet + 24, r_u32(second + 16) | r_u32(0x1F800038u));
            w_u32(packet + 28, r_u32(fourth + 28) & 0xFFFFFFu);
            w_u32(packet + 32, r_u32(fourth + 24));
            w_u32(packet + 36, r_u32(fourth + 16));
            w_u32(packet + 40, r_u32(third + 28) & 0xFFFFFFu);
            {
                uint32_t screen = r_u32(third + 24);
                uint32_t uv;
                w_u32(0x1F800028u, packet);
                w_u32(packet + 44, screen);
                uv = r_u32(third + 16);
                w_u32(0x1F800020u, packet + 52);
                w_u32(packet + 48, uv);
            }
        }
    }
    return 0;
}

/* Continue the accepted quad at 8006A0B0 with the saved pre-IRQ clip flags */
void rrj_resume_sub_80069CF0(uint32_t base_index, uint32_t depth_shift, uint32_t first_quad, const uint8_t indices[9], uint32_t current_clip_or)
{
    uint32_t triangle;

    if (first_quad >= 4)
        abort();
    for (triangle = first_quad; triangle != 4; ++triangle)
    {
        uint32_t selectors = r_u32(0x800CCA40u + 4 * triangle);
        uint32_t first_index = indices[selectors >> 12];
        uint32_t second_index = indices[(selectors >> 8) & 0xFu];
        uint32_t third_index = indices[(selectors >> 4) & 0xFu];
        uint32_t fourth_index = indices[selectors & 0xFu];
        uint32_t first = 0x1F800100u + 32 * first_index;
        uint32_t second = 0x1F800100u + 32 * second_index;
        uint32_t third = 0x1F800100u + 32 * third_index;
        uint32_t fourth = 0x1F800100u + 32 * fourth_index;
        uint32_t first_clip = triangle == first_quad ? 0 : r_u8(first + 31);
        uint32_t second_clip = triangle == first_quad ? 0 : r_u8(second + 31);
        uint32_t third_clip = triangle == first_quad ? 0 : r_u8(third + 31);
        uint32_t fourth_clip = triangle == first_quad ? 0 : r_u8(fourth + 31);
        uint32_t combined = first_clip | second_clip | third_clip | fourth_clip;

        if (triangle == first_quad)
            combined = current_clip_or;
        else if ((first_clip & second_clip & third_clip & fourth_clip & 0x1Fu) != 0)
            continue;
        if ((combined >> (depth_shift & 31u)) != 0 || (((int32_t)(uint32_t)(base_index)) < 14 && (combined & 0x10u) != 0))
        {
            (void)sub_80069CF0(base_index + 5, first_index | (second_index << 8) | (third_index << 16) | (fourth_index << 24), depth_shift);
        }
        else
        {
            uint32_t packet = r_u32(0x1F800020u);
            if (packet + 52u >= r_u32(0x8005B4D0u))
            {
                packet = sub_80021C98(packet, 52);
                w_u32(0x1F800020u, packet);
            }
            w_u32(packet, r_u32(0x1F800028u) | 0x0C000000u);
            w_u32(packet + 4, (r_u32(first + 28) & 0xFFFFFFu) | 0x3C000000u);
            w_u32(packet + 8, r_u32(first + 24));
            w_u32(packet + 12, r_u32(first + 16) | r_u32(0x1F800034u));
            w_u32(packet + 16, r_u32(second + 28) & 0xFFFFFFu);
            w_u32(packet + 20, r_u32(second + 24));
            w_u32(packet + 24, r_u32(second + 16) | r_u32(0x1F800038u));
            w_u32(packet + 28, r_u32(fourth + 28) & 0xFFFFFFu);
            w_u32(packet + 32, r_u32(fourth + 24));
            w_u32(packet + 36, r_u32(fourth + 16));
            w_u32(packet + 40, r_u32(third + 28) & 0xFFFFFFu);
            {
                uint32_t screen = r_u32(third + 24);
                uint32_t uv;
                w_u32(0x1F800028u, packet);
                w_u32(packet + 44, screen);
                uv = r_u32(third + 16);
                w_u32(0x1F800020u, packet + 52);
                w_u32(packet + 48, uv);
            }
        }
    }
}

uint32_t sub_8006A25C(uint32_t base_index, uint32_t packed_indices, uint32_t packet_color)
{
    static const uint32_t first_shifts[2] = {8, 24};
    static const uint32_t second_shifts[2] = {16, 0};
    uint8_t indices[6];
    uint32_t midpoint;
    uint32_t quad;

    FUNCTION_MARKER(0x8006A25C, "RASHCDG.BIN");
    for (midpoint = 0; midpoint != 2; ++midpoint)
    {
        uint32_t first_index = (packed_indices >> first_shifts[midpoint]) & 0xFFu;
        uint32_t second_index = (packed_indices >> second_shifts[midpoint]) & 0xFFu;
        uint32_t first = 0x1F800100u + 32 * first_index;
        uint32_t second = 0x1F800100u + 32 * second_index;
        uint32_t output = 0x1F800100u + 32 * (base_index + midpoint);
        uint32_t z_sum = r_u32(first + 8) + r_u32(second + 8);
        int32_t depth = ((int32_t)(uint32_t)(frontier_asr(r_u32(first + 20) + r_u32(second + 20), 1)));
        int32_t x = ((int32_t)(uint32_t)(frontier_asr(r_u32(first) + r_u32(second), 1)));
        int32_t y = ((int32_t)(uint32_t)(frontier_asr(r_u32(first + 4) + r_u32(second + 4), 1)));
        int32_t z = ((int32_t)(uint32_t)(frontier_asr(z_sum, 1)));
        uint32_t screen;
        uint32_t clip;
        int32_t screen_y;

        w_u32(output + 20, (uint32_t)depth);
        w_u32(output + 28, (r_u32(output + 28) & 0xFF000000u) | (r_u32(0x800D4CA8u + 4 * ((uint32_t)depth & 0xFFu)) & 0xFFFFFFu));
        w_u32(output, (uint32_t)x);
        w_u32(output + 4, (uint32_t)y);
        w_u32(output + 8, (uint32_t)z);
        screen = frontier_project_translation(x >> 5, y >> 5, ((int32_t)(uint32_t)(frontier_asr(z_sum, 6))));
        w_u32(output + 24, screen);
        if (z > 102399)
            clip = 1u << 5;
        else if (z > 51199)
            clip = 1u << 6;
        else
            clip = 1u << 7;
        if (z < 10240)
            clip |= 0x10u;
        screen_y = ((int32_t)(uint32_t)(screen)) >> 16;
        if (screen_y < 0)
            clip |= 8u;
        if (screen_y >= 241)
            clip |= 4u;
        if ((uint16_t)screen < 385u)
            clip |= (screen >> 15) & 1u;
        else
            clip |= ((screen >> 15) & 1u) + 1u;
        w_u8(output + 31, (uint8_t)clip);
    }

    indices[0] = (uint8_t)packed_indices;
    indices[1] = (uint8_t)(packed_indices >> 8);
    indices[2] = (uint8_t)(packed_indices >> 16);
    indices[3] = (uint8_t)(packed_indices >> 24);
    indices[4] = (uint8_t)base_index;
    indices[5] = (uint8_t)(base_index + 1);
    for (quad = 0; quad != 2; ++quad)
    {
        uint32_t selectors = r_u32(0x800CCA50u + 4 * quad);
        uint32_t first_index = indices[selectors >> 12];
        uint32_t second_index = indices[(selectors >> 8) & 0xFu];
        uint32_t third_index = indices[(selectors >> 4) & 0xFu];
        uint32_t fourth_index = indices[selectors & 0xFu];
        uint32_t first = 0x1F800100u + 32 * first_index;
        uint32_t second = 0x1F800100u + 32 * second_index;
        uint32_t third = 0x1F800100u + 32 * third_index;
        uint32_t fourth = 0x1F800100u + 32 * fourth_index;
        uint32_t first_clip = r_u8(first + 31);
        uint32_t second_clip = r_u8(second + 31);
        uint32_t third_clip = r_u8(third + 31);
        uint32_t fourth_clip = r_u8(fourth + 31);
        uint32_t combined = first_clip | second_clip | third_clip | fourth_clip;

        if ((first_clip & second_clip & third_clip & fourth_clip & 0x1Fu) != 0)
            continue;
        if ((combined & 4u) != 0 && ((int32_t)(uint32_t)(base_index)) < 14)
        {
            (void)sub_8006A25C(base_index + 2, first_index | (second_index << 8) | (third_index << 16) | (fourth_index << 24), packet_color);
        }
        else
        {
            uint32_t packet = r_u32(0x1F800020u);
            if (packet + 24u >= r_u32(0x8005B4D0u))
                packet = sub_80021C98(packet, 24);
            w_u32(packet, r_u32(0x1F800028u) | 0x05000000u);
            w_u32(packet + 4, packet_color | 0x28000000u);
            w_u32(packet + 8, r_u32(first + 24));
            w_u32(packet + 12, r_u32(second + 24));
            w_u32(packet + 16, r_u32(fourth + 24));
            w_u32(0x1F800028u, packet);
            w_u32(packet + 20, r_u32(third + 24));
            w_u32(0x1F800020u, packet + 24);
        }
    }
    return 0;
}

static uint32_t frontier_emit_textured_faces(uint32_t object, uint32_t group_index, int reject_common_clip)
{
    uint32_t descriptor = r_u32(object);
    uint32_t group = r_u32(descriptor + 40) + 12 * group_index;
    uint32_t triangle_count = r_u32(group + 4);
    uint32_t face = r_u32(descriptor + 56) + r_u32(group);
    uint32_t context = r_u32(0x8005B470u);
    uint32_t ordering_table = r_u32(context + 264);
    uint32_t cursor = r_u32(context + 268);
    uint32_t index;

    w_u32(0x800CCE2Cu, ordering_table);
    w_u32(0x800CCE34u, r_u32(descriptor + 52) + 4);
    w_u32(0x1F800020u, cursor);
    if (cursor + 40 * triangle_count >= r_u32(0x8005B4D0u))
    {
        cursor = sub_80021C98(cursor, 40 * triangle_count);
        w_u32(0x1F800020u, cursor);
    }
    for (index = 0; index != triangle_count; ++index, face += 20)
    {
        uint32_t flags = r_u32(face);
        uint32_t packed_indices = r_u32(face + 16);
        uint32_t first_index = r_u32(face + 12) >> 16;
        uint32_t second_index = packed_indices & 0xFFFFu;
        uint32_t third_index = packed_indices >> 16;
        uint32_t clips = r_u32(0x8005ACB8u);
        uint32_t screens = r_u32(0x8005ACB4u);
        uint32_t depths = r_u32(0x8005ACB0u);
        int32_t depth;
        int32_t other;
        int32_t bucket;
        uint32_t slot;
        uint32_t packet;

        w_u32(0x1F80003Cu, flags);
        w_u32(0x1F800048u, r_u32(face + 12));
        w_u32(0x1F80004Cu, packed_indices);
        if (reject_common_clip && (r_u8(clips + first_index) & r_u8(clips + second_index) & r_u8(clips + third_index) & 0x1Fu) != 0)
            continue;
        w_u32(0x1F800040u, r_u32(face + 4));
        w_u32(0x1F800044u, r_u32(face + 8));
        if ((flags & 4u) == 0 && NormalClip((sint32)r_u32(screens + 4 * first_index), (sint32)r_u32(screens + 4 * second_index), (sint32)r_u32(screens + 4 * third_index)) < 0)
            continue;
        depth = ((int32_t)(uint32_t)(r_u32(depths + 16 * first_index + 8)));
        other = ((int32_t)(uint32_t)(r_u32(depths + 16 * second_index + 8)));
        if (other > depth)
            depth = other;
        other = ((int32_t)(uint32_t)(r_u32(depths + 16 * third_index + 8)));
        if (other > depth)
            depth = other;
        bucket = frontier_depth_bucket(depth);
        slot = ordering_table + 4 * (uint32_t)bucket;
        packet = r_u32(0x1F800020u);
        w_u32(packet, r_u32(slot) | 0x09000000u);
        w_u32(packet + 4, r_u32(0x800CC994u + 4 * ((flags & 0xFFu) >> 3)));
        w_u32(packet + 8, r_u32(0x800D4CA8u + 4 * ((flags >> 16) & 0xFFu)) | 0x24000000u);
        w_u32(packet + 12, r_u32(screens + 4 * first_index));
        w_u32(packet + 16, r_u32(0x1F800040u));
        w_u32(packet + 20, r_u32(screens + 4 * second_index));
        w_u32(packet + 24, r_u32(0x1F800044u));
        w_u32(packet + 28, r_u32(screens + 4 * third_index));
        w_u32(packet + 32, r_u16(0x1F800048u));
        w_u32(packet + 36, 0xE2000000u);
        w_u32(0x1F800020u, packet + 40);
        w_u32(slot, packet);
    }

    {
        uint32_t quad_count = r_u32(group + 8);
        cursor = r_u32(0x1F800020u);
        if (cursor + 48 * quad_count >= r_u32(0x8005B4D0u))
        {
            cursor = sub_80021C98(cursor, 48 * quad_count);
            w_u32(0x1F800020u, cursor);
        }
        for (index = 0; index != quad_count; ++index, face += 24)
        {
            uint32_t flags = r_u32(face);
            uint32_t packed_uv = r_u32(face + 12);
            uint32_t first_pair = r_u32(face + 16);
            uint32_t second_pair = r_u32(face + 20);
            uint32_t first_index = first_pair & 0xFFFFu;
            uint32_t second_index = first_pair >> 16;
            uint32_t third_index = second_pair & 0xFFFFu;
            uint32_t fourth_index = second_pair >> 16;
            uint32_t clips = r_u32(0x8005ACB8u);
            uint32_t screens = r_u32(0x8005ACB4u);
            uint32_t depths = r_u32(0x8005ACB0u);
            int32_t depth;
            int32_t other;
            int32_t bucket;
            uint32_t slot;
            uint32_t packet;

            w_u32(0x1F80003Cu, flags);
            w_u32(0x1F80004Cu, first_pair);
            w_u32(0x1F800050u, second_pair);
            if (reject_common_clip && (r_u8(clips + first_index) & r_u8(clips + second_index) & r_u8(clips + third_index) & r_u8(clips + fourth_index) & 0x1Fu) != 0)
                continue;
            w_u32(0x1F800040u, r_u32(face + 4));
            w_u32(0x1F800044u, r_u32(face + 8));
            w_u32(0x1F800048u, packed_uv);
            if ((flags & 4u) == 0)
            {
                int32_t first_clip = NormalClip((sint32)r_u32(screens + 4 * first_index), (sint32)r_u32(screens + 4 * second_index), (sint32)r_u32(screens + 4 * third_index));
                xport_gte_write_data(13, r_u32(screens + 4 * fourth_index));
                if (first_clip < 0 && NormalClip((sint32)r_u32(screens + 4 * first_index), (sint32)r_u32(screens + 4 * fourth_index), (sint32)r_u32(screens + 4 * third_index)) > 0)
                    continue;
            }
            depth = ((int32_t)(uint32_t)(r_u32(depths + 16 * first_index + 8)));
            other = ((int32_t)(uint32_t)(r_u32(depths + 16 * second_index + 8)));
            if (other > depth)
                depth = other;
            other = ((int32_t)(uint32_t)(r_u32(depths + 16 * third_index + 8)));
            if (other > depth)
                depth = other;
            other = ((int32_t)(uint32_t)(r_u32(depths + 16 * fourth_index + 8)));
            if (other > depth)
                depth = other;
            bucket = frontier_depth_bucket(depth);
            slot = ordering_table + 4 * (uint32_t)bucket;
            packet = r_u32(0x1F800020u);
            w_u32(packet, r_u32(slot) | 0x0B000000u);
            w_u32(packet + 4, r_u32(0x800CC994u + 4 * ((flags & 0xFFu) >> 3)));
            w_u32(packet + 8, r_u32(0x800D4CA8u + 4 * ((flags >> 16) & 0xFFu)) | 0x2C000000u);
            w_u32(packet + 12, r_u32(screens + 4 * first_index));
            w_u32(packet + 16, r_u32(0x1F800040u));
            w_u32(packet + 20, r_u32(screens + 4 * second_index));
            w_u32(packet + 24, r_u32(0x1F800044u));
            w_u32(packet + 28, r_u32(screens + 4 * fourth_index));
            w_u32(packet + 32, packed_uv >> 16);
            w_u32(packet + 36, r_u32(screens + 4 * third_index));
            w_u32(packet + 40, packed_uv & 0xFFFFu);
            w_u32(packet + 44, 0xE2000000u);
            w_u32(0x1F800020u, packet + 48);
            w_u32(slot, packet);
        }
    }
    cursor = r_u32(0x1F800020u);
    w_u32(context + 268, cursor);
    return cursor;
}

uint32_t sub_8006D350(uint32_t object, uint32_t group_index)
{
    FUNCTION_MARKER(0x8006D350, "RASHCDG.BIN");
    return frontier_emit_textured_faces(object, group_index, 1);
}

uint32_t sub_8006DC20(uint32_t object, uint32_t group_index)
{
    FUNCTION_MARKER(0x8006DC20, "RASHCDG.BIN");
    return frontier_emit_textured_faces(object, group_index, 0);
}

static uint32_t frontier_road_gte_project(int32_t x, int32_t y, int32_t z)
{
    xport_gte_write_control(5, (uint32_t)x);
    xport_gte_write_control(6, (uint32_t)y);
    xport_gte_write_control(7, (uint32_t)z);
    xport_gte_execute(0x198001u);
    return xport_gte_read_data(14);
}

static void frontier_road_gte_transform(uint32_t source, FrontierVertex *vertex)
{
    xport_gte_write_data(0, r_u32(source));
    xport_gte_write_data(1, r_u32(source + 4));
    xport_gte_execute(0x4A2012u);
    vertex->x = ((int32_t)(uint32_t)(xport_gte_read_data(25) << 8));
    vertex->y = ((int32_t)(uint32_t)(xport_gte_read_data(26) << 8));
    vertex->z = ((int32_t)(uint32_t)(xport_gte_read_data(27) << 8));
    vertex->screen = frontier_road_gte_project(vertex->x >> 5, vertex->y >> 5, vertex->z >> 5);
}

static void frontier_road_gte_scratch(void)
{
    uint32_t index;
    for (index = 0; index != 4; ++index)
    {
        uint32_t record = 0x1F800100u + 32 * index;
        int32_t x = ((int32_t)(uint32_t)(r_u32(record)));
        int32_t y = ((int32_t)(uint32_t)(r_u32(record + 4)));
        int32_t z = ((int32_t)(uint32_t)(r_u32(record + 8)));
        uint32_t screen = frontier_road_gte_project(x >> 5, y >> 5, z >> 5);
        w_u32(record + 24, screen);
        w_u8(record + 31, frontier_classify_projected(z, screen, 102399));
    }
}

static int32_t frontier_road_gte_bucket(int32_t depth)
{
    int32_t base = ((int32_t)(uint32_t)(r_u32(0x1F800004u)));
    int32_t exponent = ((int32_t)(uint32_t)(r_u32(0x1F800000u)));
    int32_t bucket;
    if (base < 4096)
    {
        uint32_t lookup = 0x1F800018u + ((depth >> 12) > 0) + ((depth >> 11) > 0);
        int32_t table_index;
        exponent = ((int32_t)(uint32_t)((uint32_t)exponent + (uint32_t)(int32_t)r_s8(lookup)));
        table_index = exponent < 0 ? 0 : exponent > 3 ? 3 : exponent;
        base = ((int32_t)(uint32_t)((uint32_t)base + r_u16(0x1F800006u + 2 * (uint32_t)table_index)));
        bucket = ((int32_t)(uint32_t)(frontier_asr((uint32_t)depth - (uint32_t)base, (uint32_t)exponent & 31u)));
        bucket = ((int32_t)(uint32_t)((uint32_t)bucket + r_u16(0x1F80000Eu + 2 * (uint32_t)table_index)));
    }
    else
        bucket = ((int32_t)(uint32_t)(frontier_asr((uint32_t)depth - (uint32_t)base, (uint32_t)exponent & 31u)));
    if (bucket < 0)
        bucket = 0;
    if (bucket > ((int32_t)(uint32_t)(r_u32(0x1F80001Cu))))
        bucket = ((int32_t)(uint32_t)(r_u32(0x1F80001Cu)));
    return bucket;
}

static void frontier_road_gte_prepare(uint32_t record_index, uint32_t vertex_index, uint32_t uv)
{
    FrontierVertex vertex;
    uint32_t record = 0x1F800100u + 32 * record_index;
    uint32_t color_table = r_u32(0x800CCE34u);
    uint32_t source_table = r_u32(0x800CCE28u);
    uint32_t clips = r_u32(0x8005ACB8u);
    uint32_t shade;
    uint32_t depth_clip;

    memset(&vertex, 0, sizeof(vertex));
    frontier_road_gte_transform(source_table + 8 * vertex_index, &vertex);
    shade = r_u8(color_table + 8 * vertex_index + 6);
    if (vertex.z > 204799)
        depth_clip = 1u << 5;
    else if (vertex.z > 51199)
        depth_clip = 1u << 6;
    else
        depth_clip = 1u << 7;
    w_u32(record, (uint32_t)vertex.x);
    w_u32(record + 4, (uint32_t)vertex.y);
    w_u32(record + 8, (uint32_t)vertex.z);
    w_u32(record + 16, uv);
    w_u32(record + 20, shade);
    w_u32(record + 24, vertex.screen);
    w_u32(record + 28, (r_u32(0x800D4CA8u + 4 * shade) & 0xFFFFFFu) | ((uint32_t)(r_u8(clips + vertex_index) | depth_clip) << 24));
}

uint32_t sub_8006A630(uint32_t object, uint32_t group_index)
{
    uint32_t descriptor = r_u32(object);
    uint32_t group = r_u32(descriptor + 40) + 12 * group_index;
    uint32_t triangle_count = r_u32(group + 4);
    uint32_t face = r_u32(descriptor + 60) + r_u32(group);
    uint32_t context = r_u32(0x8005B470u);
    uint32_t ordering_table = r_u32(context + 264);
    uint32_t cursor = r_u32(context + 268);
    uint32_t color_table = r_u32(descriptor + 52) + 4;
    uint32_t screens = r_u32(0x8005ACB4u);
    uint32_t depths = r_u32(0x8005ACB0u);
    uint32_t clips = r_u32(0x8005ACB8u);
    uint32_t item;
    uint32_t index;

    FUNCTION_MARKER(0x8006A630, "RASHCDG.BIN");
    w_u32(0x1F800020u, cursor);
    w_u32(0x800CCE2Cu, ordering_table);
    w_u32(0x800CCE28u, color_table);
    w_u32(0x800CCE34u, color_table);
    for (item = 0; item != triangle_count; ++item, face += 20)
    {
        uint32_t flags = r_u32(face);
        uint32_t packed_indices = r_u32(face + 16);
        uint32_t indices[3];
        uint32_t clip_union;
        uint32_t clip_intersection;
        int32_t depth;
        int32_t bucket;
        uint32_t slot;

        w_u32(0x1F80003Cu, flags);
        w_u32(0x1F800040u, r_u32(face + 4));
        w_u32(0x1F800044u, r_u32(face + 8));
        w_u32(0x1F800048u, r_u32(face + 12));
        w_u32(0x1F80004Cu, packed_indices);
        indices[0] = r_u32(face + 12) >> 16;
        indices[1] = packed_indices & 0xFFFFu;
        indices[2] = packed_indices >> 16;
        clip_union = r_u8(clips + indices[0]) | r_u8(clips + indices[1]) | r_u8(clips + indices[2]);
        clip_intersection = r_u8(clips + indices[0]) & r_u8(clips + indices[1]) & r_u8(clips + indices[2]);
        if ((clip_intersection & 0x1Fu) != 0)
            continue;
        if ((flags & 4u) == 0 && NormalClip((sint32)r_u32(screens + 4 * indices[0]), (sint32)r_u32(screens + 4 * indices[1]), (sint32)r_u32(screens + 4 * indices[2])) < 0)
            continue;
        depth = ((int32_t)(uint32_t)(r_u32(depths + 16 * indices[0] + 8)));
        if (((int32_t)(uint32_t)(r_u32(depths + 16 * indices[1] + 8))) > depth)
            depth = ((int32_t)(uint32_t)(r_u32(depths + 16 * indices[1] + 8)));
        if (((int32_t)(uint32_t)(r_u32(depths + 16 * indices[2] + 8))) > depth)
            depth = ((int32_t)(uint32_t)(r_u32(depths + 16 * indices[2] + 8)));
        bucket = frontier_road_gte_bucket(depth);
        slot = ordering_table + 4 * (uint32_t)bucket;
        if ((clip_union & 0x20u) != 0)
        {
            frontier_road_gte_prepare(0, indices[0], r_u16(0x1F800040u));
            frontier_road_gte_prepare(1, indices[1], r_u16(0x1F800044u));
            frontier_road_gte_prepare(2, indices[2], r_u16(0x1F800048u));
            w_u32(0x1F800034u, r_u16(0x1F800042u) << 16);
            w_u32(0x1F800038u, r_u16(0x1F800046u) << 16);
            frontier_render_wrapped_subdivision(slot, flags, 0);
            cursor = r_u32(0x1F800020u);
        }
        else
        {
            uint32_t packet;
            uint32_t shade;

            if (cursor + 48 >= r_u32(0x8005B4D0u))
                cursor = sub_80021C98(cursor, 48);
            packet = cursor;
            w_u32(packet, r_u32(slot) | 0x0B000000u);
            w_u32(packet + 4, r_u32(0x1F800078u + 4 * ((flags & 0xFFu) >> 4)));
            shade = r_u8(color_table + 8 * indices[0] + 6);
            w_u32(packet + 8, r_u32(0x800D4CA8u + 4 * shade) | 0x34000000u);
            w_u32(packet + 12, r_u32(screens + 4 * indices[0]));
            w_u32(packet + 16, r_u32(0x1F800040u));
            shade = r_u8(color_table + 8 * indices[1] + 6);
            w_u32(packet + 20, r_u32(0x800D4CA8u + 4 * shade));
            w_u32(packet + 24, r_u32(screens + 4 * indices[1]));
            w_u32(packet + 28, r_u32(0x1F800044u));
            shade = r_u8(color_table + 8 * indices[2] + 6);
            w_u32(packet + 32, r_u32(0x800D4CA8u + 4 * shade));
            w_u32(packet + 36, r_u32(screens + 4 * indices[2]));
            w_u32(packet + 40, r_u16(0x1F800048u));
            w_u32(packet + 44, 0xE2000000u);
            cursor = packet + 48;
            w_u32(0x1F800020u, cursor);
            w_u32(slot, packet);
        }
    }

    {
        uint32_t quad_count = r_u32(group + 8);

        if (quad_count != 0)
            face += 24 * (quad_count - 1);
        for (item = 0; item != quad_count; ++item, face -= 24)
        {
            uint32_t flags = r_u32(face);
            uint32_t first_pair = r_u32(face + 16);
            uint32_t second_pair = r_u32(face + 20);
            uint32_t indices[4];
            uint32_t clip_values[4];
            uint32_t clip_union;
            uint32_t clip_intersection;
            int32_t depth;
            int32_t bucket;
            uint32_t slot;

            w_u32(0x1F80003Cu, flags);
            w_u32(0x1F800040u, r_u32(face + 4));
            w_u32(0x1F800044u, r_u32(face + 8));
            w_u32(0x1F800048u, r_u32(face + 12));
            w_u32(0x1F80004Cu, first_pair);
            w_u32(0x1F800050u, second_pair);
            indices[0] = first_pair & 0xFFFFu;
            indices[1] = first_pair >> 16;
            indices[2] = second_pair & 0xFFFFu;
            indices[3] = second_pair >> 16;
            clip_values[0] = r_u8(clips + indices[0]);
            clip_values[1] = r_u8(clips + indices[1]);
            clip_values[2] = r_u8(clips + indices[2]);
            clip_values[3] = r_u8(clips + indices[3]);
            clip_union = clip_values[0] | clip_values[1] | clip_values[2] | clip_values[3];
            clip_intersection = clip_values[0] & clip_values[1] & clip_values[2] & clip_values[3];
            if ((clip_intersection & 0x1Fu) != 0)
                continue;
            if ((flags & 4u) == 0 && (clip_union & 0x10u) == 0)
            {
                int32_t first_clip = NormalClip((sint32)r_u32(screens + 4 * indices[0]), (sint32)r_u32(screens + 4 * indices[1]), (sint32)r_u32(screens + 4 * indices[2]));
                xport_gte_write_data(13, r_u32(screens + 4 * indices[3]));
                if (first_clip < 0 && NormalClip((sint32)r_u32(screens + 4 * indices[0]), (sint32)r_u32(screens + 4 * indices[3]), (sint32)r_u32(screens + 4 * indices[2])) > 0)
                    continue;
            }
            depth = ((int32_t)(uint32_t)(r_u32(depths + 16 * indices[0] + 8)));
            for (index = 1; index != 4; ++index)
                if (((int32_t)(uint32_t)(r_u32(depths + 16 * indices[index] + 8))) > depth)
                    depth = ((int32_t)(uint32_t)(r_u32(depths + 16 * indices[index] + 8)));
            bucket = frontier_road_gte_bucket(depth);
            slot = ordering_table + 4 * (uint32_t)bucket;
            if ((clip_union & 0x20u) != 0)
            {
                for (index = 0; index != 4; ++index)
                    frontier_road_gte_prepare(index, indices[index], r_u16(0x1F800040u + 4 * index));
                w_u32(0x1F800034u, r_u16(0x1F800042u) << 16);
                w_u32(0x1F800170u, r_u16(0x1F80004Au));
                w_u32(0x1F800038u, r_u16(0x1F800046u) << 16);
                frontier_render_wrapped_subdivision(slot, flags, 1);
                cursor = r_u32(0x1F800020u);
            }
            else
            {
                uint32_t packet;
                uint32_t shade;

                if (cursor + 60 >= r_u32(0x8005B4D0u))
                    cursor = sub_80021C98(cursor, 60);
                packet = cursor;
                w_u32(packet, r_u32(slot) | 0x0E000000u);
                w_u32(packet + 4, r_u32(0x1F800078u + 4 * ((flags & 0xFFu) >> 4)));
                shade = r_u8(color_table + 8 * indices[0] + 6);
                w_u32(packet + 8, r_u32(0x800D4CA8u + 4 * shade) | 0x3C000000u);
                w_u32(packet + 12, r_u32(screens + 4 * indices[0]));
                w_u32(packet + 16, r_u32(0x1F800040u));
                shade = r_u8(color_table + 8 * indices[1] + 6);
                w_u32(packet + 20, r_u32(0x800D4CA8u + 4 * shade));
                w_u32(packet + 24, r_u32(screens + 4 * indices[1]));
                w_u32(packet + 28, r_u32(0x1F800044u));
                shade = r_u8(color_table + 8 * indices[3] + 6);
                w_u32(packet + 32, r_u32(0x800D4CA8u + 4 * shade));
                w_u32(packet + 36, r_u32(screens + 4 * indices[3]));
                w_u32(packet + 40, r_u32(0x1F800048u) >> 16);
                shade = r_u8(color_table + 8 * indices[2] + 6);
                w_u32(packet + 44, r_u32(0x800D4CA8u + 4 * shade));
                w_u32(packet + 48, r_u32(screens + 4 * indices[2]));
                w_u32(packet + 52, r_u32(0x1F800048u));
                w_u32(packet + 56, 0xE2000000u);
                cursor = packet + 60;
                w_u32(0x1F800020u, cursor);
                w_u32(slot, packet);
            }
        }
    }
    w_u32(context + 268, cursor);
    return cursor;
}

uint32_t sub_8006C888(uint32_t object, uint32_t group_index)
{
    uint32_t descriptor = r_u32(object);
    uint32_t group = r_u32(descriptor + 40) + 12 * group_index;
    uint32_t face = r_u32(descriptor + 60) + r_u32(group);
    uint32_t context = r_u32(0x8005B470u);
    uint32_t ordering_table = r_u32(context + 264);
    uint32_t cursor = r_u32(context + 268);
    uint32_t screens = r_u32(0x8005ACB4u);
    uint32_t depths = r_u32(0x8005ACB0u);
    uint32_t color_table = r_u32(descriptor + 52) + 4;
    uint32_t count = r_u32(group + 4);
    uint32_t item;

    FUNCTION_MARKER(0x8006C888, "RASHCDG.BIN");
    w_u32(0x1F800020u, cursor);
    w_u32(0x800CCE2Cu, ordering_table);
    w_u32(0x800CCE28u, color_table);
    w_u32(0x800CCE34u, color_table);
    for (item = 0; item != count; ++item, face += 20)
    {
        uint32_t flags = r_u32(face);
        uint32_t packed_indices = r_u32(face + 16);
        uint32_t first_index = r_u32(face + 12) >> 16;
        uint32_t second_index = packed_indices & 0xFFFFu;
        uint32_t third_index = packed_indices >> 16;
        int32_t depth;
        int32_t candidate;
        int32_t bucket;
        uint32_t slot;
        uint32_t packet;
        uint32_t shade;

        w_u32(0x1F80003Cu, flags);
        w_u32(0x1F800040u, r_u32(face + 4));
        w_u32(0x1F800044u, r_u32(face + 8));
        w_u32(0x1F800048u, r_u32(face + 12));
        w_u32(0x1F80004Cu, packed_indices);
        if ((flags & 4u) == 0 && NormalClip((sint32)r_u32(screens + 4 * first_index), (sint32)r_u32(screens + 4 * second_index), (sint32)r_u32(screens + 4 * third_index)) < 0)
            continue;
        depth = ((int32_t)(uint32_t)(r_u32(depths + 16 * first_index + 8)));
        candidate = ((int32_t)(uint32_t)(r_u32(depths + 16 * second_index + 8)));
        if (candidate > depth)
            depth = candidate;
        candidate = ((int32_t)(uint32_t)(r_u32(depths + 16 * third_index + 8)));
        if (candidate > depth)
            depth = candidate;
        bucket = frontier_depth_bucket(depth);
        slot = ordering_table + 4 * (uint32_t)bucket;
        if (cursor + 48 >= r_u32(0x8005B4D0u))
            cursor = sub_80021C98(cursor, 48);
        packet = cursor;
        w_u32(packet, r_u32(slot) | 0x0B000000u);
        w_u32(packet + 4, r_u32(0x1F800078u + 4 * ((flags & 0xFFu) >> 4)));
        shade = r_u8(color_table + 8 * first_index + 6);
        w_u32(packet + 8, r_u32(0x800D4CA8u + 4 * shade) | 0x34000000u);
        w_u32(packet + 12, r_u32(screens + 4 * first_index));
        w_u32(packet + 16, r_u32(0x1F800040u));
        shade = r_u8(color_table + 8 * second_index + 6);
        w_u32(packet + 20, r_u32(0x800D4CA8u + 4 * shade));
        w_u32(packet + 24, r_u32(screens + 4 * second_index));
        w_u32(packet + 28, r_u32(0x1F800044u));
        shade = r_u8(color_table + 8 * third_index + 6);
        w_u32(packet + 32, r_u32(0x800D4CA8u + 4 * shade));
        w_u32(packet + 36, r_u32(screens + 4 * third_index));
        w_u32(packet + 40, r_u16(0x1F800048u));
        w_u32(packet + 44, 0xE2000000u);
        cursor = packet + 48;
        w_u32(0x1F800020u, cursor);
        w_u32(slot, packet);
    }

    count = r_u32(group + 8);
    for (item = 0; item != count; ++item, face += 24)
    {
        uint32_t flags = r_u32(face);
        uint32_t first_pair = r_u32(face + 16);
        uint32_t second_pair = r_u32(face + 20);
        uint32_t first_index = first_pair & 0xFFFFu;
        uint32_t second_index = first_pair >> 16;
        uint32_t third_index = second_pair & 0xFFFFu;
        uint32_t fourth_index = second_pair >> 16;
        int32_t depth;
        int32_t candidate;
        int32_t bucket;
        uint32_t slot;
        uint32_t packet;
        uint32_t shade;

        w_u32(0x1F80003Cu, flags);
        w_u32(0x1F800040u, r_u32(face + 4));
        w_u32(0x1F800044u, r_u32(face + 8));
        w_u32(0x1F800048u, r_u32(face + 12));
        w_u32(0x1F80004Cu, first_pair);
        w_u32(0x1F800050u, second_pair);
        if ((flags & 4u) == 0)
        {
            int32_t first_clip = NormalClip((sint32)r_u32(screens + 4 * first_index), (sint32)r_u32(screens + 4 * second_index), (sint32)r_u32(screens + 4 * third_index));
            xport_gte_write_data(13, r_u32(screens + 4 * fourth_index));
            if (first_clip < 0 && NormalClip((sint32)r_u32(screens + 4 * first_index), (sint32)r_u32(screens + 4 * fourth_index), (sint32)r_u32(screens + 4 * third_index)) > 0)
                continue;
        }
        depth = ((int32_t)(uint32_t)(r_u32(depths + 16 * first_index + 8)));
        candidate = ((int32_t)(uint32_t)(r_u32(depths + 16 * second_index + 8)));
        if (candidate > depth)
            depth = candidate;
        candidate = ((int32_t)(uint32_t)(r_u32(depths + 16 * third_index + 8)));
        if (candidate > depth)
            depth = candidate;
        candidate = ((int32_t)(uint32_t)(r_u32(depths + 16 * fourth_index + 8)));
        if (candidate > depth)
            depth = candidate;
        bucket = frontier_depth_bucket(depth);
        slot = ordering_table + 4 * (uint32_t)bucket;
        if (cursor + 60 >= r_u32(0x8005B4D0u))
            cursor = sub_80021C98(cursor, 60);
        packet = cursor;
        w_u32(packet, r_u32(slot) | 0x0E000000u);
        w_u32(packet + 4, r_u32(0x1F800078u + 4 * ((flags & 0xFFu) >> 4)));
        shade = r_u8(color_table + 8 * first_index + 6);
        w_u32(packet + 8, r_u32(0x800D4CA8u + 4 * shade) | 0x3C000000u);
        w_u32(packet + 12, r_u32(screens + 4 * first_index));
        w_u32(packet + 16, r_u32(0x1F800040u));
        shade = r_u8(color_table + 8 * second_index + 6);
        w_u32(packet + 20, r_u32(0x800D4CA8u + 4 * shade));
        w_u32(packet + 24, r_u32(screens + 4 * second_index));
        w_u32(packet + 28, r_u32(0x1F800044u));
        shade = r_u8(color_table + 8 * fourth_index + 6);
        w_u32(packet + 32, r_u32(0x800D4CA8u + 4 * shade));
        w_u32(packet + 36, r_u32(screens + 4 * fourth_index));
        w_u32(packet + 40, r_u32(0x1F800048u) >> 16);
        shade = r_u8(color_table + 8 * third_index + 6);
        w_u32(packet + 44, r_u32(0x800D4CA8u + 4 * shade));
        w_u32(packet + 48, r_u32(screens + 4 * third_index));
        w_u32(packet + 52, r_u32(0x1F800048u));
        w_u32(packet + 56, 0xE2000000u);
        cursor = packet + 60;
        w_u32(0x1F800020u, cursor);
        w_u32(slot, packet);
    }
    w_u32(context + 268, cursor);
    return cursor;
}

static uint32_t frontier_emit_road_quads(uint32_t object, uint32_t group_index, int reject_common_clip)
{
    uint32_t descriptor = r_u32(object);
    uint32_t group = r_u32(descriptor + 40) + 12 * group_index;
    uint32_t count = r_u32(group + 8);
    uint32_t face = r_u32(descriptor + 60) + r_u32(group);
    uint32_t context = r_u32(0x8005B470u);
    uint32_t ordering_table = r_u32(context + 264);
    uint32_t cursor = r_u32(context + 268);
    uint32_t color_table = r_u32(descriptor + 52) + 4;
    uint32_t depths = r_u32(0x8005ACB0u);
    uint32_t screens = r_u32(0x8005ACB4u);
    uint32_t clips = r_u32(0x8005ACB8u);
    uint32_t item;

    w_u32(0x1F800020u, cursor);
    w_u32(0x800CCE2Cu, ordering_table);
    w_u32(0x800CCE34u, color_table);
    w_u32(0x800CCE28u, color_table);
    for (item = 0; item != count; ++item, face += 24)
    {
        uint32_t first_pair = r_u32(face + 16);
        uint32_t second_pair = r_u32(face + 20);
        uint32_t indices[4];
        uint32_t index;
        uint32_t clip = 0x0Fu;
        int32_t depth;
        int32_t bucket;
        uint32_t slot;

        indices[0] = first_pair & 0xFFFFu;
        indices[1] = first_pair >> 16;
        indices[2] = second_pair & 0xFFFFu;
        indices[3] = second_pair >> 16;
        w_u32(0x1F80004Cu, first_pair);
        w_u32(0x1F800050u, second_pair);
        for (index = 0; index != 4; ++index)
            clip &= r_u8(clips + indices[index]);
        if (reject_common_clip && clip != 0)
            continue;
        for (index = 0; index != 4; ++index)
            w_u32(0x1F80003Cu + 4 * index, r_u32(face + 4 * index));
        depth = ((int32_t)(uint32_t)(r_u32(depths + 16 * indices[0] + 8)));
        for (index = 1; index != 4; ++index)
        {
            int32_t candidate = ((int32_t)(uint32_t)(r_u32(depths + 16 * indices[index] + 8)));
            if (candidate > depth)
                depth = candidate;
        }
        if (depth >= 4096)
        {
            uint32_t flags = r_u16(0x1F80003Eu);
            uint32_t packet;
            uint32_t shade;

            w_u16(0x1F800042u, r_u16(0x800D5EC8u + ((flags >> 1) & 0x18u)));
            w_u16(0x1F800046u, r_u16(0x8005B370u));
            if (cursor + 52 >= r_u32(0x8005B4D0u))
                cursor = sub_80021C98(cursor, 52);
            bucket = frontier_road_gte_bucket(depth);
            slot = ordering_table + 4 * (uint32_t)bucket;
            packet = cursor;
            w_u32(packet, r_u32(slot) | 0x0C000000u);
            shade = r_u8(color_table + 8 * indices[0] + 6);
            w_u32(packet + 4, r_u32(0x800D4CA8u + 4 * shade) | 0x3C000000u);
            w_u32(packet + 8, r_u32(screens + 4 * indices[0]));
            w_u32(packet + 12, r_u32(0x1F800040u));
            shade = r_u8(color_table + 8 * indices[1] + 6);
            w_u32(packet + 16, r_u32(0x800D4CA8u + 4 * shade));
            w_u32(packet + 20, r_u32(screens + 4 * indices[1]));
            w_u32(packet + 24, r_u32(0x1F800044u));
            shade = r_u8(color_table + 8 * indices[3] + 6);
            w_u32(packet + 28, r_u32(0x800D4CA8u + 4 * shade));
            w_u32(packet + 32, r_u32(screens + 4 * indices[3]));
            w_u32(packet + 36, r_u16(0x1F80004Au));
            shade = r_u8(color_table + 8 * indices[2] + 6);
            w_u32(packet + 40, r_u32(0x800D4CA8u + 4 * shade));
            w_u32(packet + 44, r_u32(screens + 4 * indices[2]));
            w_u32(packet + 48, r_u16(0x1F800048u));
            cursor = packet + 52;
            w_u32(0x1F800020u, cursor);
            w_u32(slot, packet);
        }
        else
        {
            FrontierVertex vertices[18];
            int32_t delta[2][3];
            int16_t shade_delta[2];
            uint32_t flags = r_u16(0x1F80003Eu);
            uint32_t table_offset = 4 * (flags & 0x0Fu);
            uint32_t pattern = r_u32(0x800CC914u + table_offset);
            uint32_t segment_count = pattern >> 28;
            uint32_t texture_pattern = r_u32(0x800CC874u + table_offset);
            uint32_t texture_group = (flags >> 2) & 0x0Cu;
            uint32_t depth_band = r_u8(0x800CCA58u + (((uint32_t)depth ^ (depth < 0 ? 0xFFFFFFFFu : 0u)) + (depth < 0 ? 1u : 0u)) / 512);
            uint32_t original_slots[4];
            int32_t step = ((int32_t)(uint32_t)(r_u32(0x800CCA10u + 4 * segment_count)));
            uint32_t pair;
            uint32_t position;
            uint32_t pattern_shift = 0;

            memset(vertices, 0, sizeof(vertices));
            original_slots[0] = 0;
            original_slots[1] = 2 * segment_count;
            original_slots[2] = 2 * segment_count + 1;
            original_slots[3] = 1;
            bucket = frontier_road_gte_bucket(depth);
            slot = ordering_table + 4 * (uint32_t)bucket;
            for (index = 0; index != 4; ++index)
            {
                FrontierVertex *vertex = &vertices[original_slots[index]];
                uint32_t shade;
                uint32_t depth_clip;

                frontier_road_gte_transform(color_table + 8 * indices[index], vertex);
                shade = r_u8(color_table + 8 * indices[index] + 6);
                vertex->shade = (int32_t)shade;
                vertex->color_clip = r_u32(0x800D4CA8u + 4 * shade) & 0xFFFFFFu;
                if (vertex->z > 204799)
                    depth_clip = 1u << 5;
                else if (vertex->z > 51199)
                    depth_clip = 1u << 6;
                else
                    depth_clip = 1u << 7;
                vertex->color_clip |= (uint32_t)(r_u8(clips + indices[index]) | depth_clip) << 24;
            }
            for (pair = 0; pair != 2; ++pair)
            {
                FrontierVertex *start = &vertices[pair];
                FrontierVertex *end = &vertices[2 * segment_count + pair];

                delta[pair][0] = (int32_t)(frontier_asr(((uint32_t)end->x - (uint32_t)start->x) * (uint32_t)step, 12));
                delta[pair][1] = (int32_t)(frontier_asr(((uint32_t)end->y - (uint32_t)start->y) * (uint32_t)step, 12));
                delta[pair][2] = (int32_t)(frontier_asr(((uint32_t)end->z - (uint32_t)start->z) * (uint32_t)step, 12));
                shade_delta[pair] = (int16_t)(frontier_asr(((uint32_t)end->shade - (uint32_t)start->shade) * (uint32_t)step, 12));
                w_u32(0x1F800054u + 16 * pair, (uint32_t)delta[pair][0]);
                w_u32(0x1F800058u + 16 * pair, (uint32_t)delta[pair][1]);
                w_u32(0x1F80005Cu + 16 * pair, (uint32_t)delta[pair][2]);
                w_u16(0x1F800074u + 2 * pair, (uint16_t)shade_delta[pair]);
            }
            for (position = 2; position < 2 * segment_count; position += 2)
            {
                for (pair = 0; pair != 2; ++pair)
                {
                    FrontierVertex *previous = &vertices[position - 2 + pair];
                    FrontierVertex *vertex = &vertices[position + pair];
                    uint32_t shade;

                    vertex->x = ((int32_t)(uint32_t)((uint32_t)previous->x + (uint32_t)delta[pair][0]));
                    vertex->y = ((int32_t)(uint32_t)((uint32_t)previous->y + (uint32_t)delta[pair][1]));
                    vertex->z = ((int32_t)(uint32_t)((uint32_t)previous->z + (uint32_t)delta[pair][2]));
                    vertex->screen = frontier_road_gte_project(vertex->x >> 5, vertex->y >> 5, vertex->z >> 5);
                    vertex->shade = previous->shade + shade_delta[pair];
                    shade = (uint8_t)vertex->shade;
                    vertex->color_clip = (r_u32(0x800D4CA8u + 4 * shade) & 0xFFFFFFu) | ((uint32_t)frontier_classify_projected(vertex->z, vertex->screen, 102399) << 24);
                }
            }
            w_u32(0x1F800028u, r_u32(slot));
            for (position = 0; position < 2 * segment_count; position += 2)
            {
                uint32_t side_code = (pattern >> pattern_shift) & 3u;

                if (side_code != 0)
                {
                    uint32_t color = r_u32(0x800523ACu + 4 * side_code);

                    for (pair = 0; pair != 2; ++pair)
                    {
                        FrontierVertex *source = &vertices[position + pair];
                        uint32_t near_record = 0x1F800100u + 32 * pair;
                        uint32_t far_record = 0x1F800140u + 32 * pair;
                        int32_t x = ((int32_t)(uint32_t)((uint32_t)source->x + (uint32_t)(delta[pair][0] >> 6)));
                        int32_t y = ((int32_t)(uint32_t)((uint32_t)source->y + (uint32_t)(delta[pair][1] >> 6)));
                        int32_t z = ((int32_t)(uint32_t)((uint32_t)source->z + (uint32_t)(delta[pair][2] >> 6)));

                        w_u32(near_record, (uint32_t)x);
                        w_u32(near_record + 4, (uint32_t)y);
                        w_u32(near_record + 8, (uint32_t)z);
                        w_u32(far_record, (uint32_t)((int32_t)(uint32_t)((uint32_t)x + (uint32_t)(delta[pair][0] >> 5))));
                        w_u32(far_record + 4, (uint32_t)((int32_t)(uint32_t)((uint32_t)y + (uint32_t)(delta[pair][1] >> 5))));
                        w_u32(far_record + 8, (uint32_t)((int32_t)(uint32_t)((uint32_t)z + (uint32_t)(delta[pair][2] >> 5))));
                    }
                    frontier_road_gte_scratch();
                    frontier_render_flat_quad(depth, color);
                }
                side_code = (pattern >> (pattern_shift + 2)) & 3u;
                if (side_code != 0)
                {
                    uint32_t color = r_u32(0x800523ACu + 4 * side_code);

                    for (pair = 0; pair != 2; ++pair)
                    {
                        FrontierVertex *source = &vertices[position + 2 + pair];
                        uint32_t far_record = 0x1F800140u + 32 * pair;
                        uint32_t near_record = 0x1F800100u + 32 * pair;
                        int32_t x = ((int32_t)(uint32_t)((uint32_t)source->x - (uint32_t)(delta[pair][0] >> 6)));
                        int32_t y = ((int32_t)(uint32_t)((uint32_t)source->y - (uint32_t)(delta[pair][1] >> 6)));
                        int32_t z = ((int32_t)(uint32_t)((uint32_t)source->z - (uint32_t)(delta[pair][2] >> 6)));

                        w_u32(far_record, (uint32_t)x);
                        w_u32(far_record + 4, (uint32_t)y);
                        w_u32(far_record + 8, (uint32_t)z);
                        w_u32(near_record, (uint32_t)((int32_t)(uint32_t)((uint32_t)x - (uint32_t)(delta[pair][0] >> 5))));
                        w_u32(near_record + 4, (uint32_t)((int32_t)(uint32_t)((uint32_t)y - (uint32_t)(delta[pair][1] >> 5))));
                        w_u32(near_record + 8, (uint32_t)((int32_t)(uint32_t)((uint32_t)z - (uint32_t)(delta[pair][2] >> 5))));
                    }
                    frontier_road_gte_scratch();
                    frontier_render_flat_quad(depth, color);
                }
                pattern_shift += 4;
            }
            pattern_shift = 0;
            for (position = 0; position < 2 * segment_count; position += 2)
            {
                uint32_t texture_code = (texture_pattern >> pattern_shift) & 3u;
                uint32_t clut_code = (texture_pattern >> (pattern_shift + 2)) & 3u;
                uint32_t uv_table = 0x800CC8B4u + 32 * texture_code + 16 * depth_band;
                uint32_t scratch_index;

                for (scratch_index = 0; scratch_index != 4; ++scratch_index)
                {
                    const FrontierVertex *vertex = &vertices[position + scratch_index];
                    uint32_t record = 0x1F800100u + 32 * scratch_index;

                    w_u32(record, (uint32_t)vertex->x);
                    w_u32(record + 4, (uint32_t)vertex->y);
                    w_u32(record + 8, (uint32_t)vertex->z);
                    w_u32(record + 12, vertex->reserved);
                    w_u32(record + 16, r_u16(uv_table + 4 * scratch_index));
                    w_u32(record + 20, (uint32_t)vertex->shade);
                    w_u32(record + 24, vertex->screen);
                    w_u32(record + 28, vertex->color_clip);
                }
                w_u32(0x1F800034u, r_u16(0x800D5EC8u + 2 * (clut_code + 1 + texture_group)) << 16);
                w_u32(0x1F800038u, r_u16(0x8005B370u) << 16);
                (void)sub_80069CF0(4, 0x01030200u, 5);
                pattern_shift += 5;
            }
            w_u32(slot, r_u32(0x1F800028u));
            cursor = r_u32(0x1F800020u);
        }
    }
    w_u32(context + 268, cursor);
    return cursor;
}

uint32_t sub_8006E474(uint32_t object, uint32_t group_index)
{
    uint32_t descriptor = r_u32(object);
    uint32_t group = r_u32(descriptor + 40) + 12 * group_index;
    uint32_t count = r_u32(group + 8);
    uint32_t face = r_u32(descriptor + 60) + r_u32(group);
    uint32_t context = r_u32(0x8005B470u);
    uint32_t ordering_table = r_u32(context + 264);
    uint32_t cursor = r_u32(context + 268);
    uint32_t color_table = r_u32(descriptor + 52) + 4;
    uint32_t depths = r_u32(0x8005ACB0u);
    uint32_t screens = r_u32(0x8005ACB4u);
    uint32_t clips = r_u32(0x8005ACB8u);
    uint32_t item;

    w_u32(0x1F800020u, cursor);
    w_u32(0x800CCE2Cu, ordering_table);
    w_u32(0x800CCE34u, color_table);
    w_u32(0x800CCE28u, color_table);
    FUNCTION_MARKER(0x8006E474, "RASHCDG.BIN");
    for (item = 0; item != count; ++item, face += 24)
    {
        uint32_t first_pair = r_u32(face + 16);
        uint32_t second_pair = r_u32(face + 20);
        uint32_t indices[4];
        uint32_t index;
        uint32_t clip = 0x0Fu;
        int32_t depth;
        int32_t bucket;
        uint32_t slot;

        indices[0] = first_pair & 0xFFFFu;
        indices[1] = first_pair >> 16;
        indices[2] = second_pair & 0xFFFFu;
        indices[3] = second_pair >> 16;
        w_u32(0x1F80004Cu, first_pair);
        w_u32(0x1F800050u, second_pair);
        for (index = 0; index != 4; ++index)
            clip &= r_u8(clips + indices[index]);
        if (clip != 0)
            continue;
        for (index = 0; index != 4; ++index)
            w_u32(0x1F80003Cu + 4 * index, r_u32(face + 4 * index));
        depth = ((int32_t)(uint32_t)(r_u32(depths + 16 * indices[0] + 8)));
        for (index = 1; index != 4; ++index)
        {
            int32_t candidate = ((int32_t)(uint32_t)(r_u32(depths + 16 * indices[index] + 8)));
            if (candidate > depth)
                depth = candidate;
        }
        if (depth >= 4096)
        {
            uint32_t flags = r_u16(0x1F80003Eu);
            uint32_t packet;
            uint32_t shade;

            w_u16(0x1F800042u, r_u16(0x800D5EC8u + ((flags >> 1) & 0x18u)));
            w_u16(0x1F800046u, r_u16(0x8005B370u));
            if (cursor + 52 >= r_u32(0x8005B4D0u))
                cursor = sub_80021C98(cursor, 52);
            bucket = frontier_road_gte_bucket(depth);
            slot = ordering_table + 4 * (uint32_t)bucket;
            packet = cursor;
            w_u32(packet, r_u32(slot) | 0x0C000000u);
            shade = r_u8(color_table + 8 * indices[0] + 6);
            w_u32(packet + 4, r_u32(0x800D4CA8u + 4 * shade) | 0x3C000000u);
            w_u32(packet + 8, r_u32(screens + 4 * indices[0]));
            w_u32(packet + 12, r_u32(0x1F800040u));
            shade = r_u8(color_table + 8 * indices[1] + 6);
            w_u32(packet + 16, r_u32(0x800D4CA8u + 4 * shade));
            w_u32(packet + 20, r_u32(screens + 4 * indices[1]));
            w_u32(packet + 24, r_u32(0x1F800044u));
            shade = r_u8(color_table + 8 * indices[3] + 6);
            w_u32(packet + 28, r_u32(0x800D4CA8u + 4 * shade));
            w_u32(packet + 32, r_u32(screens + 4 * indices[3]));
            w_u32(packet + 36, r_u16(0x1F80004Au));
            shade = r_u8(color_table + 8 * indices[2] + 6);
            w_u32(packet + 40, r_u32(0x800D4CA8u + 4 * shade));
            w_u32(packet + 44, r_u32(screens + 4 * indices[2]));
            w_u32(packet + 48, r_u16(0x1F800048u));
            cursor = packet + 52;
            w_u32(0x1F800020u, cursor);
            w_u32(slot, packet);
        }
        else
        {
            FrontierVertex vertices[18];
            int32_t delta[2][3];
            int16_t shade_delta[2];
            uint32_t flags = r_u16(0x1F80003Eu);
            uint32_t table_offset = 4 * (flags & 0x0Fu);
            uint32_t pattern = r_u32(0x800CC914u + table_offset);
            uint32_t segment_count = pattern >> 28;
            uint32_t texture_pattern = r_u32(0x800CC874u + table_offset);
            uint32_t texture_group = (flags >> 2) & 0x0Cu;
            uint32_t depth_band = r_u8(0x800CCA58u + (depth < 0 ? 0u - (uint32_t)depth : (uint32_t)depth) / 512);
            uint32_t original_slots[4];
            int32_t step = ((int32_t)(uint32_t)(r_u32(0x800CCA10u + 4 * segment_count)));
            uint32_t pair;
            uint32_t position;
            uint32_t pattern_shift = 0;

            memset(vertices, 0, sizeof(vertices));
            original_slots[0] = 0;
            original_slots[1] = 2 * segment_count;
            original_slots[2] = 2 * segment_count + 1;
            original_slots[3] = 1;
            bucket = frontier_road_gte_bucket(depth);
            slot = ordering_table + 4 * (uint32_t)bucket;
            for (index = 0; index != 4; ++index)
            {
                FrontierVertex *vertex = &vertices[original_slots[index]];
                uint32_t shade;
                uint32_t depth_clip;

                frontier_road_gte_transform(color_table + 8 * indices[index], vertex);
                shade = r_u8(color_table + 8 * indices[index] + 6);
                vertex->shade = (int32_t)shade;
                vertex->color_clip = r_u32(0x800D4CA8u + 4 * shade) & 0xFFFFFFu;
                if (vertex->z > 204799)
                    depth_clip = 1u << 5;
                else if (vertex->z > 51199)
                    depth_clip = 1u << 6;
                else
                    depth_clip = 1u << 7;
                vertex->color_clip |= (uint32_t)(r_u8(clips + indices[index]) | depth_clip) << 24;
            }
            for (pair = 0; pair != 2; ++pair)
            {
                FrontierVertex *start = &vertices[pair];
                FrontierVertex *end = &vertices[2 * segment_count + pair];

                delta[pair][0] = (int32_t)(frontier_asr(((uint32_t)end->x - (uint32_t)start->x) * (uint32_t)step, 12));
                delta[pair][1] = (int32_t)(frontier_asr(((uint32_t)end->y - (uint32_t)start->y) * (uint32_t)step, 12));
                delta[pair][2] = (int32_t)(frontier_asr(((uint32_t)end->z - (uint32_t)start->z) * (uint32_t)step, 12));
                shade_delta[pair] = (int16_t)(frontier_asr(((uint32_t)end->shade - (uint32_t)start->shade) * (uint32_t)step, 12));
                w_u32(0x1F800054u + 16 * pair, (uint32_t)delta[pair][0]);
                w_u32(0x1F800058u + 16 * pair, (uint32_t)delta[pair][1]);
                w_u32(0x1F80005Cu + 16 * pair, (uint32_t)delta[pair][2]);
                w_u16(0x1F800074u + 2 * pair, (uint16_t)shade_delta[pair]);
            }
            for (position = 2; position < 2 * segment_count; position += 2)
            {
                for (pair = 0; pair != 2; ++pair)
                {
                    FrontierVertex *previous = &vertices[position - 2 + pair];
                    FrontierVertex *vertex = &vertices[position + pair];
                    uint32_t shade;

                    vertex->x = ((int32_t)(uint32_t)((uint32_t)previous->x + (uint32_t)delta[pair][0]));
                    vertex->y = ((int32_t)(uint32_t)((uint32_t)previous->y + (uint32_t)delta[pair][1]));
                    vertex->z = ((int32_t)(uint32_t)((uint32_t)previous->z + (uint32_t)delta[pair][2]));
                    vertex->screen = frontier_road_gte_project(vertex->x >> 5, vertex->y >> 5, vertex->z >> 5);
                    vertex->shade = previous->shade + shade_delta[pair];
                    shade = (uint8_t)vertex->shade;
                    vertex->color_clip = (r_u32(0x800D4CA8u + 4 * shade) & 0xFFFFFFu) | ((uint32_t)frontier_classify_projected(vertex->z, vertex->screen, 102399) << 24);
                }
            }
            w_u32(0x1F800028u, r_u32(slot));
            for (position = 0; position < 2 * segment_count; position += 2)
            {
                uint32_t side_code = (pattern >> pattern_shift) & 3u;

                if (side_code != 0)
                {
                    uint32_t color = r_u32(0x800523ACu + 4 * side_code);

                    for (pair = 0; pair != 2; ++pair)
                    {
                        FrontierVertex *source = &vertices[position + pair];
                        uint32_t near_record = 0x1F800100u + 32 * pair;
                        uint32_t far_record = 0x1F800140u + 32 * pair;
                        int32_t x = ((int32_t)(uint32_t)((uint32_t)source->x + (uint32_t)(delta[pair][0] >> 6)));
                        int32_t y = ((int32_t)(uint32_t)((uint32_t)source->y + (uint32_t)(delta[pair][1] >> 6)));
                        int32_t z = ((int32_t)(uint32_t)((uint32_t)source->z + (uint32_t)(delta[pair][2] >> 6)));

                        w_u32(near_record, (uint32_t)x);
                        w_u32(near_record + 4, (uint32_t)y);
                        w_u32(near_record + 8, (uint32_t)z);
                        w_u32(far_record, (uint32_t)((int32_t)(uint32_t)((uint32_t)x + (uint32_t)(delta[pair][0] >> 5))));
                        w_u32(far_record + 4, (uint32_t)((int32_t)(uint32_t)((uint32_t)y + (uint32_t)(delta[pair][1] >> 5))));
                        w_u32(far_record + 8, (uint32_t)((int32_t)(uint32_t)((uint32_t)z + (uint32_t)(delta[pair][2] >> 5))));
                    }
                    frontier_road_gte_scratch();
                    frontier_render_flat_quad(depth, color);
                }
                side_code = (pattern >> (pattern_shift + 2)) & 3u;
                if (side_code != 0)
                {
                    uint32_t color = r_u32(0x800523ACu + 4 * side_code);

                    for (pair = 0; pair != 2; ++pair)
                    {
                        FrontierVertex *source = &vertices[position + 2 + pair];
                        uint32_t far_record = 0x1F800140u + 32 * pair;
                        uint32_t near_record = 0x1F800100u + 32 * pair;
                        int32_t x = ((int32_t)(uint32_t)((uint32_t)source->x - (uint32_t)(delta[pair][0] >> 6)));
                        int32_t y = ((int32_t)(uint32_t)((uint32_t)source->y - (uint32_t)(delta[pair][1] >> 6)));
                        int32_t z = ((int32_t)(uint32_t)((uint32_t)source->z - (uint32_t)(delta[pair][2] >> 6)));

                        w_u32(far_record, (uint32_t)x);
                        w_u32(far_record + 4, (uint32_t)y);
                        w_u32(far_record + 8, (uint32_t)z);
                        w_u32(near_record, (uint32_t)((int32_t)(uint32_t)((uint32_t)x - (uint32_t)(delta[pair][0] >> 5))));
                        w_u32(near_record + 4, (uint32_t)((int32_t)(uint32_t)((uint32_t)y - (uint32_t)(delta[pair][1] >> 5))));
                        w_u32(near_record + 8, (uint32_t)((int32_t)(uint32_t)((uint32_t)z - (uint32_t)(delta[pair][2] >> 5))));
                    }
                    frontier_road_gte_scratch();
                    frontier_render_flat_quad(depth, color);
                }
                pattern_shift += 4;
            }
            pattern_shift = 0;
            for (position = 0; position < 2 * segment_count; position += 2)
            {
                uint32_t texture_code = (texture_pattern >> pattern_shift) & 3u;
                uint32_t clut_code = (texture_pattern >> (pattern_shift + 2)) & 3u;
                uint32_t uv_table = 0x800CC8B4u + 32 * texture_code + 16 * depth_band;
                uint32_t scratch_index;

                for (scratch_index = 0; scratch_index != 4; ++scratch_index)
                {
                    const FrontierVertex *vertex = &vertices[position + scratch_index];
                    uint32_t record = 0x1F800100u + 32 * scratch_index;

                    w_u32(record, (uint32_t)vertex->x);
                    w_u32(record + 4, (uint32_t)vertex->y);
                    w_u32(record + 8, (uint32_t)vertex->z);
                    w_u32(record + 12, vertex->reserved);
                    w_u32(record + 16, r_u16(uv_table + 4 * scratch_index));
                    w_u32(record + 20, (uint32_t)vertex->shade);
                    w_u32(record + 24, vertex->screen);
                    w_u32(record + 28, vertex->color_clip);
                }
                w_u32(0x1F800034u, r_u16(0x800D5EC8u + 2 * (clut_code + 1 + texture_group)) << 16);
                w_u32(0x1F800038u, r_u16(0x8005B370u) << 16);
                (void)sub_80069CF0(4, 0x01030200u, 5);
                pattern_shift += 5;
            }
            w_u32(slot, r_u32(0x1F800028u));
            cursor = r_u32(0x1F800020u);
        }
    }
    w_u32(context + 268, cursor);
    return cursor;
}

uint32_t sub_8006F5D0(uint32_t object, uint32_t group_index)
{
    FUNCTION_MARKER(0x8006F5D0, "RASHCDG.BIN");
    return frontier_emit_road_quads(object, group_index, 0);
}

/* Continue unprocessed faces using the existing road state */
static uint32_t frontier_resume_road_faces(uint32_t face, uint32_t count)
{
    uint32_t ordering_table = r_u32(0x800CCE2Cu);
    uint32_t cursor = r_u32(0x1F800020u);
    uint32_t color_table = r_u32(0x800CCE34u);
    uint32_t depths = r_u32(0x8005ACB0u);
    uint32_t screens = r_u32(0x8005ACB4u);
    uint32_t clips = r_u32(0x8005ACB8u);
    uint32_t item;

    for (item = 0; item != count; ++item, face += 24)
    {
        uint32_t first_pair = r_u32(face + 16);
        uint32_t second_pair = r_u32(face + 20);
        uint32_t indices[4];
        uint32_t index;
        uint32_t clip = 0x0Fu;
        int32_t depth;
        int32_t bucket;
        uint32_t slot;

        indices[0] = first_pair & 0xFFFFu;
        indices[1] = first_pair >> 16;
        indices[2] = second_pair & 0xFFFFu;
        indices[3] = second_pair >> 16;
        w_u32(0x1F80004Cu, first_pair);
        w_u32(0x1F800050u, second_pair);
        for (index = 0; index != 4; ++index)
            clip &= r_u8(clips + indices[index]);
        if (clip != 0)
            continue;
        for (index = 0; index != 4; ++index)
            w_u32(0x1F80003Cu + 4 * index, r_u32(face + 4 * index));
        depth = ((int32_t)(uint32_t)(r_u32(depths + 16 * indices[0] + 8)));
        for (index = 1; index != 4; ++index)
        {
            int32_t candidate = ((int32_t)(uint32_t)(r_u32(depths + 16 * indices[index] + 8)));
            if (candidate > depth)
                depth = candidate;
        }
        if (depth >= 4096)
        {
            uint32_t flags = r_u16(0x1F80003Eu);
            uint32_t packet;
            uint32_t shade;

            w_u16(0x1F800042u, r_u16(0x800D5EC8u + ((flags >> 1) & 0x18u)));
            w_u16(0x1F800046u, r_u16(0x8005B370u));
            if (cursor + 52 >= r_u32(0x8005B4D0u))
            {
                cursor = sub_80021C98(cursor, 52);
                w_u32(0x1F800020u, cursor);
            }
            bucket = frontier_road_gte_bucket(depth);
            slot = ordering_table + 4 * (uint32_t)bucket;
            packet = cursor;
            w_u32(packet, r_u32(slot) | 0x0C000000u);
            shade = r_u8(color_table + 8 * indices[0] + 6);
            w_u32(packet + 4, r_u32(0x800D4CA8u + 4 * shade) | 0x3C000000u);
            {
                uint32_t screen = r_u32(screens + 4 * indices[0]);
                w_u32(packet + 12, r_u32(0x1F800040u));
                w_u32(packet + 8, screen);
            }
            shade = r_u8(color_table + 8 * indices[1] + 6);
            w_u32(packet + 16, r_u32(0x800D4CA8u + 4 * shade));
            {
                uint32_t screen = r_u32(screens + 4 * indices[1]);
                w_u32(packet + 24, r_u32(0x1F800044u));
                w_u32(packet + 20, screen);
            }
            shade = r_u8(color_table + 8 * indices[3] + 6);
            w_u32(packet + 28, r_u32(0x800D4CA8u + 4 * shade));
            w_u32(packet + 32, r_u32(screens + 4 * indices[3]));
            w_u32(packet + 36, r_u16(0x1F80004Au));
            shade = r_u8(color_table + 8 * indices[2] + 6);
            w_u32(packet + 40, r_u32(0x800D4CA8u + 4 * shade));
            {
                uint32_t uv = r_u16(0x1F800048u);
                uint32_t screen = r_u32(screens + 4 * indices[2]);
                cursor = packet + 52;
                w_u32(0x1F800020u, cursor);
                w_u32(packet + 48, uv);
                w_u32(packet + 44, screen);
                w_u32(slot, packet);
            }
        }
        else
        {
            FrontierVertex vertices[18];
            int32_t delta[2][3];
            int16_t shade_delta[2];
            uint32_t flags = r_u16(0x1F80003Eu);
            uint32_t table_offset = 4 * (flags & 0x0Fu);
            uint32_t pattern = r_u32(0x800CC914u + table_offset);
            uint32_t segment_count = pattern >> 28;
            uint32_t texture_pattern = r_u32(0x800CC874u + table_offset);
            uint32_t texture_group = (flags >> 2) & 0x0Cu;
            uint32_t depth_band = r_u8(0x800CCA58u + (depth < 0 ? 0u - (uint32_t)depth : (uint32_t)depth) / 512);
            uint32_t original_slots[4];
            int32_t step = ((int32_t)(uint32_t)(r_u32(0x800CCA10u + 4 * segment_count)));
            uint32_t pair;
            uint32_t position;
            uint32_t pattern_shift = 0;

            memset(vertices, 0, sizeof(vertices));
            original_slots[0] = 0;
            original_slots[1] = 2 * segment_count;
            original_slots[2] = 2 * segment_count + 1;
            original_slots[3] = 1;
            bucket = frontier_road_gte_bucket(depth);
            slot = ordering_table + 4 * (uint32_t)bucket;
            for (index = 0; index != 4; ++index)
            {
                FrontierVertex *vertex = &vertices[original_slots[index]];
                uint32_t shade;
                uint32_t depth_clip;

                frontier_road_gte_transform(color_table + 8 * indices[index], vertex);
                shade = r_u8(color_table + 8 * indices[index] + 6);
                vertex->shade = (int32_t)shade;
                vertex->color_clip = r_u32(0x800D4CA8u + 4 * shade) & 0xFFFFFFu;
                if (vertex->z > 204799)
                    depth_clip = 1u << 5;
                else if (vertex->z > 51199)
                    depth_clip = 1u << 6;
                else
                    depth_clip = 1u << 7;
                vertex->color_clip |= (uint32_t)(r_u8(clips + indices[index]) | depth_clip) << 24;
            }
            for (pair = 0; pair != 2; ++pair)
            {
                FrontierVertex *start = &vertices[pair];
                FrontierVertex *end = &vertices[2 * segment_count + pair];

                delta[pair][0] = (int32_t)(frontier_asr(((uint32_t)end->x - (uint32_t)start->x) * (uint32_t)step, 12));
                delta[pair][1] = (int32_t)(frontier_asr(((uint32_t)end->y - (uint32_t)start->y) * (uint32_t)step, 12));
                delta[pair][2] = (int32_t)(frontier_asr(((uint32_t)end->z - (uint32_t)start->z) * (uint32_t)step, 12));
                shade_delta[pair] = (int16_t)(frontier_asr(((uint32_t)end->shade - (uint32_t)start->shade) * (uint32_t)step, 12));
                w_u32(0x1F800054u + 16 * pair, (uint32_t)delta[pair][0]);
                w_u32(0x1F800058u + 16 * pair, (uint32_t)delta[pair][1]);
                w_u32(0x1F80005Cu + 16 * pair, (uint32_t)delta[pair][2]);
                w_u16(0x1F800074u + 2 * pair, (uint16_t)shade_delta[pair]);
            }
            for (position = 2; position < 2 * segment_count; position += 2)
            {
                for (pair = 0; pair != 2; ++pair)
                {
                    FrontierVertex *previous = &vertices[position - 2 + pair];
                    FrontierVertex *vertex = &vertices[position + pair];
                    uint32_t shade;

                    vertex->x = ((int32_t)(uint32_t)((uint32_t)previous->x + (uint32_t)delta[pair][0]));
                    vertex->y = ((int32_t)(uint32_t)((uint32_t)previous->y + (uint32_t)delta[pair][1]));
                    vertex->z = ((int32_t)(uint32_t)((uint32_t)previous->z + (uint32_t)delta[pair][2]));
                    vertex->screen = frontier_road_gte_project(vertex->x >> 5, vertex->y >> 5, vertex->z >> 5);
                    vertex->shade = (int32_t)((uint32_t)previous->shade + (uint32_t)(int32_t)shade_delta[pair]);
                    shade = (uint8_t)vertex->shade;
                    vertex->color_clip = (r_u32(0x800D4CA8u + 4 * shade) & 0xFFFFFFu) | ((uint32_t)frontier_classify_projected(vertex->z, vertex->screen, 102399) << 24);
                }
            }
            w_u32(0x1F800028u, r_u32(slot));
            for (position = 0; position < 2 * segment_count; position += 2)
            {
                uint32_t side_code = (pattern >> pattern_shift) & 3u;

                if (side_code != 0)
                {
                    uint32_t color = r_u32(0x800523ACu + 4 * side_code);

                    for (pair = 0; pair != 2; ++pair)
                    {
                        FrontierVertex *source = &vertices[position + pair];
                        uint32_t near_record = 0x1F800100u + 32 * pair;
                        uint32_t far_record = 0x1F800140u + 32 * pair;
                        int32_t x = ((int32_t)(uint32_t)((uint32_t)source->x + (uint32_t)(delta[pair][0] >> 6)));
                        int32_t y = ((int32_t)(uint32_t)((uint32_t)source->y + (uint32_t)(delta[pair][1] >> 6)));
                        int32_t z = ((int32_t)(uint32_t)((uint32_t)source->z + (uint32_t)(delta[pair][2] >> 6)));

                        w_u32(near_record, (uint32_t)x);
                        w_u32(near_record + 4, (uint32_t)y);
                        w_u32(near_record + 8, (uint32_t)z);
                        w_u32(far_record, (uint32_t)((int32_t)(uint32_t)((uint32_t)x + (uint32_t)(delta[pair][0] >> 5))));
                        w_u32(far_record + 4, (uint32_t)((int32_t)(uint32_t)((uint32_t)y + (uint32_t)(delta[pair][1] >> 5))));
                        w_u32(far_record + 8, (uint32_t)((int32_t)(uint32_t)((uint32_t)z + (uint32_t)(delta[pair][2] >> 5))));
                    }
                    frontier_road_gte_scratch();
                    frontier_render_flat_quad(depth, color);
                }
                side_code = (pattern >> (pattern_shift + 2)) & 3u;
                if (side_code != 0)
                {
                    uint32_t color = r_u32(0x800523ACu + 4 * side_code);

                    for (pair = 0; pair != 2; ++pair)
                    {
                        FrontierVertex *source = &vertices[position + 2 + pair];
                        uint32_t far_record = 0x1F800140u + 32 * pair;
                        uint32_t near_record = 0x1F800100u + 32 * pair;
                        int32_t x = ((int32_t)(uint32_t)((uint32_t)source->x - (uint32_t)(delta[pair][0] >> 6)));
                        int32_t y = ((int32_t)(uint32_t)((uint32_t)source->y - (uint32_t)(delta[pair][1] >> 6)));
                        int32_t z = ((int32_t)(uint32_t)((uint32_t)source->z - (uint32_t)(delta[pair][2] >> 6)));

                        w_u32(far_record, (uint32_t)x);
                        w_u32(far_record + 4, (uint32_t)y);
                        w_u32(far_record + 8, (uint32_t)z);
                        w_u32(near_record, (uint32_t)((int32_t)(uint32_t)((uint32_t)x - (uint32_t)(delta[pair][0] >> 5))));
                        w_u32(near_record + 4, (uint32_t)((int32_t)(uint32_t)((uint32_t)y - (uint32_t)(delta[pair][1] >> 5))));
                        w_u32(near_record + 8, (uint32_t)((int32_t)(uint32_t)((uint32_t)z - (uint32_t)(delta[pair][2] >> 5))));
                    }
                    frontier_road_gte_scratch();
                    frontier_render_flat_quad(depth, color);
                }
                pattern_shift += 4;
            }
            pattern_shift = 0;
            for (position = 0; position < 2 * segment_count; position += 2)
            {
                uint32_t texture_code = (texture_pattern >> pattern_shift) & 3u;
                uint32_t clut_code = (texture_pattern >> (pattern_shift + 2)) & 3u;
                uint32_t uv_table = 0x800CC8B4u + 32 * texture_code + 16 * depth_band;
                uint32_t scratch_index;

                for (scratch_index = 0; scratch_index != 4; ++scratch_index)
                {
                    const FrontierVertex *vertex = &vertices[position + scratch_index];
                    uint32_t record = 0x1F800100u + 32 * scratch_index;

                    w_u32(record, (uint32_t)vertex->x);
                    w_u32(record + 4, (uint32_t)vertex->y);
                    w_u32(record + 8, (uint32_t)vertex->z);
                    w_u32(record + 12, vertex->reserved);
                    w_u32(record + 16, r_u16(uv_table + 4 * scratch_index));
                    w_u32(record + 20, (uint32_t)vertex->shade);
                    w_u32(record + 24, vertex->screen);
                    w_u32(record + 28, vertex->color_clip);
                }
                w_u32(0x1F800034u, r_u16(0x800D5EC8u + 2 * (clut_code + 1 + texture_group)) << 16);
                w_u32(0x1F800038u, r_u16(0x8005B370u) << 16);
                (void)sub_80069CF0(4, 0x01030200u, 5);
                pattern_shift += 5;
            }
            w_u32(slot, r_u32(0x1F800028u));
            cursor = r_u32(0x1F800020u);
        }
    }
    cursor = r_u32(0x1F800020u);
    w_u32(r_u32(0x8005B470u) + 268, cursor);
    return cursor;
}

/* Continue at 8006F53C after the interrupted clip call returns */
uint32_t rrj_resume_sub_8006E474(const RRJRoadContinuation *phase)
{
    uint32_t position;
    uint32_t pattern_shift;
    uint32_t cursor;

    if (phase == NULL || phase->remaining_faces == 0 || phase->segment_count > 8 || (phase->next_vertex_position & 1u) != 0 || phase->next_vertex_position > 2 * phase->segment_count)
        abort();
    pattern_shift = phase->pattern_shift;
    for (position = phase->next_vertex_position; position < 2 * phase->segment_count; position += 2)
    {
        uint32_t texture_code = (phase->texture_pattern >> (pattern_shift & 31u)) & 3u;
        uint32_t clut_code = (phase->texture_pattern >> ((pattern_shift + 2) & 31u)) & 3u;
        uint32_t uv_table = 0x800CC8B4u + 32 * texture_code + 16 * phase->depth_band;
        uint32_t clut = r_u16(0x800D5EC8u + 2 * (clut_code + 1 + phase->texture_group));
        uint32_t index;

        for (index = 0; index != 4; ++index)
        {
            const uint32_t *vertex = phase->vertices[position + index];
            uint32_t record = 0x1F800100u + 32 * index;
            uint32_t word;
            for (word = 0; word != 8; ++word)
                w_u32(record + 4 * word, vertex[word]);
            w_u32(record + 16, r_u16(uv_table + 4 * index));
        }
        w_u32(0x1F800034u, clut << 16);
        w_u32(0x1F800038u, r_u16(0x8005B370u) << 16);
        pattern_shift += 5;
        (void)sub_80069CF0(4, 0x01030200u, 5);
    }
    w_u32(r_u32(0x800CCE2Cu) + 4 * phase->ordering_bucket, r_u32(0x1F800028u));
    if (phase->remaining_faces > 1)
        return frontier_resume_road_faces(phase->face + 24, phase->remaining_faces - 1);
    cursor = r_u32(0x1F800020u);
    w_u32(r_u32(0x8005B470u) + 268, cursor);
    return cursor;
}

uint32_t sub_80068FCC(uint32_t object)
{
    uint32_t group;
    uint32_t end;
    uint32_t shift = 0;
    uint32_t state;

    FUNCTION_MARKER(0x80068FCC, "RASHCDG.BIN");
    (void)sub_8001064C();
    for (group = 0; group != 16; ++group)
        w_u32(0x1F800078u + 4 * group, r_u32(0x800CC954u + 4 * group));
    group = 0;
    if (r_u16(r_u32(object) + 4) != 0)
    {
        do
        {
            (void)sub_8006D350(object, group);
            ++group;
        } while ((int32_t)group < r_u16(r_u32(object) + 4));
    }
    group = r_u16(r_u32(object) + 4);
    end = group + r_u16(r_u32(object) + 6);
    state = r_u32(0x8005B310u);
    if (state != 0 && r_u32(state + 16) == r_u32(object + 4))
    {
        uint32_t filtered = r_u32(0x800CC86Cu);
        uint32_t row = r_u32(state + 20);
        uint32_t flags = r_u32(object + 52);
        uint32_t mode = (flags >> ((4 * row) & 31u)) & (filtered != 0 ? 0x0Du : 0x0Fu);

        if ((mode & 2u) != 0)
            (void)sub_800706A4(object);
    }
    for (; (int32_t)group < (int32_t)end; ++group, shift += 4)
    {
        uint32_t filtered = r_u32(0x800CC86Cu);
        uint32_t flags = r_u32(object + 52);
        uint32_t mode = (flags >> (shift & 31u)) & (filtered != 0 ? 0x0Du : 0x0Fu);

        if ((mode & 1u) == 0)
            continue;
        if ((mode & 2u) != 0)
        {
            if ((mode & 4u) != 0)
            {
                (void)sub_8006E474(object, group + 2 * r_u16(r_u32(object) + 6));
                (void)sub_8006A630(object, group + r_u16(r_u32(object) + 6));
            }
            else
            {
                (void)sub_8006F5D0(object, group + 2 * r_u16(r_u32(object) + 6));
                (void)sub_8006C888(object, group + r_u16(r_u32(object) + 6));
            }
        }
        else if ((mode & 4u) != 0)
            (void)sub_8006D350(object, group);
        else
            (void)sub_8006DC20(object, group);
    }
    return 0;
}

/* Resume the row dispatcher at 80069158 after the textured-road call */
void rrj_resume_sub_80068FCC(uint32_t object, uint32_t group, uint32_t end, uint32_t shift)
{
    (void)sub_8006A630(object, group + r_u16(r_u32(object) + 6));
    ++group;
    shift += 4;
    for (; (int32_t)group < (int32_t)end; ++group, shift += 4)
    {
        uint32_t filtered = r_u32(0x800CC86Cu);
        uint32_t flags = r_u32(object + 52);
        uint32_t mode = (flags >> (shift & 31u)) & (filtered != 0 ? 0x0Du : 0x0Fu);

        if ((mode & 1u) == 0)
            continue;
        if ((mode & 2u) != 0)
        {
            if ((mode & 4u) != 0)
            {
                (void)sub_8006E474(object, group + 2 * r_u16(r_u32(object) + 6));
                (void)sub_8006A630(object, group + r_u16(r_u32(object) + 6));
            }
            else
            {
                (void)sub_8006F5D0(object, group + 2 * r_u16(r_u32(object) + 6));
                (void)sub_8006C888(object, group + r_u16(r_u32(object) + 6));
            }
        }
        else if ((mode & 4u) != 0)
            (void)sub_8006D350(object, group);
        else
            (void)sub_8006DC20(object, group);
    }
}

uint32_t sub_800706A4(uint32_t object)
{
    uint32_t descriptor = r_u32(object);
    uint32_t row_count = r_u16(descriptor + 6);
    uint32_t state = r_u32(0x8005B310u);
    uint32_t row = r_u32(state + 20);
    uint32_t group_index;
    uint32_t group;
    uint32_t quad_count;
    uint32_t quad;
    uint32_t face;
    uint32_t context = r_u32(0x8005B470u);
    uint32_t ordering_table = r_u32(context + 264);
    uint32_t cursor = r_u32(context + 268);
    uint32_t clips = r_u32(0x8005ACB8u);
    uint32_t screens = r_u32(0x8005ACB4u);
    uint32_t depths = r_u32(0x8005ACB0u);
    uint32_t indices[4];
    uint32_t index;
    uint32_t texture;
    uint32_t texture_x;
    int32_t depth;
    int32_t bucket;
    uint32_t slot;

    FUNCTION_MARKER(0x800706A4, "RASHCDG.BIN");
    if (row >= row_count)
    {
        row = row_count - 1;
        w_u32(state + 20, row);
    }
    group_index = row + 2 * row_count + r_u16(descriptor + 4);
    group = r_u32(descriptor + 40) + 12 * group_index;
    quad_count = r_u32(group + 8);
    quad = r_u32(state + 24);
    if (quad >= quad_count)
    {
        quad = quad_count - 1;
        w_u32(state + 24, quad);
    }
    face = r_u32(descriptor + 60) + r_u32(group) + 24 * quad;
    w_u32(0x800CCE2Cu, ordering_table);
    w_u32(0x800CCE34u, r_u32(descriptor + 52) + 4);
    w_u32(0x800CCE28u, r_u32(descriptor + 52) + 4);
    w_u32(0x1F800020u, cursor);
    w_u32(0x1F80004Cu, r_u32(face + 16));
    w_u32(0x1F800050u, r_u32(face + 20));
    for (index = 0; index != 4; ++index)
        indices[index] = r_u16(0x1F80004Cu + 2 * index);
    if ((r_u8(clips + indices[0]) & r_u8(clips + indices[1]) & r_u8(clips + indices[2]) & r_u8(clips + indices[3]) & 0x0Fu) != 0)
        return 0;

    for (index = 0; index != 4; ++index)
        w_u32(0x1F80003Cu + 4 * index, r_u32(face + 4 * index));
    texture = 0x800533B4u + 88 * (r_u32(r_u32(0x8005B2F8u) + 48) - 1);
    texture_x = r_u8(texture + 85) + ((r_u8(texture + 86) & 0x10u) << 4);
    w_u16(0x1F800042u, (uint16_t)((texture_x << 6) | ((r_u8(texture + 86) & 0x0Fu) << 2) | 1u));
    w_u8(0x1F800040u, 32);
    w_u16(0x1F800046u, (uint16_t)(((texture_x & 0x100u) >> 4) | (r_u8(texture + 86) & 0x0Fu)));
    w_u8(0x1F800044u, 63);
    w_u8(0x1F800041u, (uint8_t)(texture_x + 2));
    w_u8(0x1F800048u, 63);
    w_u8(0x1F800045u, (uint8_t)(texture_x + 2));
    w_u8(0x1F80004Au, 32);
    w_u8(0x1F800049u, (uint8_t)(texture_x + 34));
    w_u16(0x1F80003Eu, 3);
    w_u8(0x1F80004Bu, (uint8_t)(texture_x + 34));

    depth = ((int32_t)(uint32_t)(r_u32(depths + 16 * indices[0] + 8)));
    if (depth >= 4096)
    {
        uint32_t packet;

        if (cursor + 52 >= r_u32(0x8005B4D0u))
            cursor = sub_80021C98(cursor, 52);
        bucket = frontier_depth_bucket(depth);
        slot = ordering_table + 4 * (uint32_t)bucket;
        packet = cursor;
        w_u32(packet, r_u32(slot) | 0x0C000000u);
        for (index = 0; index != 4; ++index)
        {
            static const uint8_t packet_order[4] = {0, 1, 3, 2};
            static const uint8_t packet_offsets[4] = {4, 16, 28, 40};
            static const uint8_t uv_offsets[4] = {0x40, 0x44, 0x4A, 0x48};
            uint32_t vertex = indices[packet_order[index]];
            uint32_t shade = r_u8(r_u32(0x800CCE34u) + 8 * vertex + 6);
            uint32_t offset = packet_offsets[index];
            uint32_t color = r_u32(0x800D4CA8u + 4 * shade) & 0xFFFFFFu;

            if (index == 0)
                color |= 0x3C000000u;
            w_u32(packet + offset, color);
            w_u32(packet + offset + 4, r_u32(screens + 4 * vertex));
            w_u32(packet + offset + 8, r_u16(0x1F800000u + uv_offsets[index]));
        }
        cursor = packet + 52;
        w_u32(0x1F800020u, cursor);
        w_u32(slot, packet);
    }
    else
    {
        MATRIX matrix;

        for (index = 1; index != 4; ++index)
        {
            int32_t candidate = ((int32_t)(uint32_t)(r_u32(depths + 16 * indices[index] + 8)));
            if (candidate > depth)
                depth = candidate;
        }
        bucket = frontier_depth_bucket(depth);
        slot = ordering_table + 4 * (uint32_t)bucket;
        ReadRotMatrix(&matrix);
        for (index = 0; index != 4; ++index)
        {
            uint32_t record = 0x1F800100u + 32 * index;
            uint32_t source = r_u32(0x800CCE28u) + 8 * indices[index];
            int32_t x = (int32_t)(((int64_t)matrix.m[0][0] * (int16_t)r_u16(source) + (int64_t)matrix.m[0][1] * (int16_t)r_u16(source + 2) + (int64_t)matrix.m[0][2] * (int16_t)r_u16(source + 4)) >> 12);
            int32_t y = (int32_t)(((int64_t)matrix.m[1][0] * (int16_t)r_u16(source) + (int64_t)matrix.m[1][1] * (int16_t)r_u16(source + 2) + (int64_t)matrix.m[1][2] * (int16_t)r_u16(source + 4)) >> 12);
            int32_t z = (int32_t)(((int64_t)matrix.m[2][0] * (int16_t)r_u16(source) + (int64_t)matrix.m[2][1] * (int16_t)r_u16(source + 2) + (int64_t)matrix.m[2][2] * (int16_t)r_u16(source + 4)) >> 12);
            uint32_t shade = r_u8(r_u32(0x800CCE34u) + 8 * indices[index] + 6);
            uint32_t clip;

            x = ((int32_t)(uint32_t)(((uint32_t)x + (uint32_t)matrix.t[0]) << 8));
            y = ((int32_t)(uint32_t)(((uint32_t)y + (uint32_t)matrix.t[1]) << 8));
            z = ((int32_t)(uint32_t)(((uint32_t)z + (uint32_t)matrix.t[2]) << 8));
            w_u32(record, (uint32_t)x);
            w_u32(record + 4, (uint32_t)y);
            w_u32(record + 8, (uint32_t)z);
            w_u32(record + 16, r_u16(0x1F800040u + 4 * index));
            w_u32(record + 20, shade);
            w_u32(record + 24, frontier_project_translation(x >> 5, y >> 5, z >> 5));
            w_u32(record + 28, (r_u32(record + 28) & 0xFF000000u) | (r_u32(0x800D4CA8u + 4 * shade) & 0xFFFFFFu));
            if (z > 204799)
                clip = 1u << 5;
            else if (z > 51199)
                clip = 1u << 6;
            else
                clip = 1u << 7;
            w_u8(record + 31, (uint8_t)(r_u8(clips + indices[index]) | clip));
        }
        w_u32(0x1F800170u, r_u16(0x1F80004Au));
        w_u32(0x1F800034u, r_u16(0x1F800042u) << 16);
        w_u32(0x1F800038u, r_u16(0x1F800046u) << 16);
        w_u32(0x1F800028u, r_u32(slot));
        (void)sub_80069784(4, 0x03020100u, 4);
        w_u32(slot, r_u32(0x1F800028u));
        cursor = r_u32(0x1F800020u);
    }
    w_u32(context + 268, cursor);
    return cursor;
}

int32_t sub_800669E8(uint32_t object, uint32_t output)
{
    uint32_t descriptor = r_u32(object);
    uint32_t shift = r_u16(descriptor + 14) >> 12;
    int32_t x = (int32_t)r_u32(object + 12);
    int32_t y = (int32_t)r_u32(object + 16);
    int32_t z = (int32_t)r_u32(object + 20);

    FUNCTION_MARKER(0x800669E8, "RASHCDG.BIN");
    if (r_s8(object + 72) == 1)
    {
        x >>= shift;
        y >>= shift;
        z >>= shift;
    }
    w_u32(output, (uint32_t)x);
    w_u32(output + 4, (uint32_t)y);
    w_u32(output + 8, (uint32_t)z);
    return z;
}

uint32_t sub_800220A4(uint32_t matrix_address, uint32_t count, uint32_t vertices, uint32_t records, uint32_t screens, uint32_t clips)
{
    uint32_t index;
    uint32_t result = 0;

    FUNCTION_MARKER(0x800220A4, "SLUS_010.53");
    for (index = 0; index != 3; ++index)
        xport_gte_write_control(5 + index, r_u32(matrix_address + 20 + 4 * index));
    for (index = 0; index != 5; ++index)
        xport_gte_write_control(index, r_u32(matrix_address + 4 * index));
    if (count == 0)
    {
        /* TODO: The zero-count flush requires incoming MIPS v1 and t2 carriers */
        RRJ_DEBUG_FPRINTF(stderr, "sub_800220A4: Missing zero-count v1/t2 carriers\n");
        abort();
    }
    for (index = 0; index < count; ++index)
    {
        uint32_t source = vertices + 8 * index;
        uint32_t previous_screen = xport_gte_read_data(14);
        uint32_t screen;
        uint32_t depth;
        uint32_t x_bits;
        uint32_t y_bits;
        uint32_t classification;

        xport_gte_write_data(0, r_u32(source));
        xport_gte_write_data(1, r_u32(source + 4));
        xport_gte_execute(0x180001u);
        w_u32(records + 16 * index, xport_gte_read_data(25));
        w_u32(records + 16 * index + 4, xport_gte_read_data(26));
        depth = xport_gte_read_data(27);
        w_u32(records + 16 * index + 8, depth);
        screen = xport_gte_read_data(14);
        w_u32(screens + 4 * index, screen);
        /* The terminal flush retains the preceding screen X in t2 */
        x_bits = (uint32_t)(int32_t)(int16_t)(index + 1 == count ? previous_screen : screen);
        y_bits = (uint32_t)(int32_t)(int16_t)(screen >> 16);
        result = (((384u - x_bits) >> 31) << 1) | (x_bits >> 31);
        classification = (((depth - 1024u) >> 31) << 5) | (((depth - 40u) >> 31) << 4) | (((240u - y_bits) >> 31) << 3) | ((y_bits >> 31) << 2) | result;
        w_u8(clips + index, (uint8_t)classification);
    }
    return result;
}

uint32_t sub_80030B7C(uint32_t first, uint32_t second)
{
    uint32_t array = r_u32(0x8005ACBC);
    uint32_t entry = array + 36 * first;
    uint32_t value;

    FUNCTION_MARKER(0x80030B7C, "SLUS_010.53");
    value = r_u32(r_u32(entry + 64));
    w_u32(entry + 52, value);
    entry = array + 36 * second;
    value = r_u32(r_u32(entry + 64));
    w_u32(entry + 52, value);
    return value;
}

uint32_t sub_80030C8C(uint32_t index)
{
    uint32_t entry = r_u32(0x8005ACBC) + 36 * index;
    uint32_t value;

    FUNCTION_MARKER(0x80030C8C, "SLUS_010.53");
    value = r_u32(r_u32(entry + 64));
    w_u32(entry + 52, value);
    return value;
}

void sub_80030CB8(int32_t value)
{
    FUNCTION_MARKER(0x80030CB8, "SLUS_010.53");
    (void)(value >> 1);
}

static uint32_t frontier_store_vram(uint32_t output, uint32_t x, uint32_t y)
{
    uint32_t row;
    const uint8 *vram = (const uint8 *)VRAM;
    for (row = 0; row < 128; ++row)
        xport_guest_copy(xport_guest_ref(output + row * 128), xport_host_ref((void *)(vram + ((y + row) * 1024 + x) * 2)), 128);
    return 0;
}

static uint32_t frontier_move_vram(uint32_t source_x, uint32_t source_y, uint32_t height, uint32_t destination_x)
{
    uint32_t row;
    uint8 *vram = (uint8 *)VRAM;
    for (row = 0; row < height; ++row)
        memmove(vram + (row * 1024 + destination_x) * 2, vram + ((source_y + row) * 1024 + source_x) * 2, 128);
    return 0;
}

uint32_t sub_80022484(uint32_t page, uint32_t mode, uint32_t first_output, uint32_t second_output, uint32_t table_index)
{
    uint32_t state = r_u32(0x8005B2F8);
    uint32_t table_page = r_u32(state + 48);
    uint32_t kind = mode >> 15;
    uint32_t entry;
    uint32_t x;
    uint32_t y;

    FUNCTION_MARKER(0x80022484, "SLUS_010.53");
    if (kind == 0)
    {
        entry = 0x800533B4u + 88 * (table_page - 1) + 4 * table_index;
        x = ((page & 15) + (r_u8(entry + 2) & 15)) << 6;
        (void)frontier_store_vram(first_output, x, 0);
        return frontier_store_vram(second_output, x, 128);
    }
    if (kind == 1)
    {
        entry = 0x800533B4u + 88 * (table_page - 1) + 8 + 4 * table_index;
        x = ((r_u8(entry + 2) & 15) << 6) + ((page << 5) & 0x3C0);
        y = r_u8(entry + 1) + ((r_u8(entry + 2) & 0x10) << 4) + ((page & 1) << 7);
        return frontier_store_vram(first_output, x, y);
    }
    return 1;
}

uint32_t sub_800225E0(uint32_t page, uint32_t mode, uint32_t table_index)
{
    uint32_t state = r_u32(0x8005B2F8);
    uint32_t table_page = r_u32(state + 48);
    uint32_t table_base = 0x800533B4u + 88 * (table_page - 1);
    uint32_t destination_x = ((r_u8(table_base + 2) & 15) << 6) + 256;
    uint32_t kind = mode >> 15;
    uint32_t entry;
    uint32_t source_x;
    uint32_t source_y;

    FUNCTION_MARKER(0x800225E0, "SLUS_010.53");
    if (kind == 0)
    {
        w_u32(0x800D7710u + 24 * table_index + 8 * page, 0xFFFFFFFFu);
        entry = table_base + 4 * table_index;
        source_x = ((page & 15) + (r_u8(entry + 2) & 15)) << 6;
        return frontier_move_vram(source_x, 0, 256, destination_x);
    }
    if (kind == 1)
    {
        w_u32(0x800D76D0u + 32 * table_index + 8 * page, 0xFFFFFFFFu);
        entry = table_base + 8 + 4 * table_index;
        source_x = ((r_u8(entry + 2) & 15) << 6) + ((page << 5) & 0x3C0);
        source_y = r_u8(entry + 1) + ((r_u8(entry + 2) & 0x10) << 4) + ((page & 1) << 7);
        return frontier_move_vram(source_x, source_y, 128, destination_x);
    }
    return 1;
}

uint32_t sub_80033634(int32_t index, uint32_t mode, uint32_t group)
{
    uint32_t base = mode != 0 ? 0x800D76D0u : 0x800D7710u;
    uint32_t entry = base + (mode != 0 ? 32 : 24) * group;

    FUNCTION_MARKER(0x80033634, "SLUS_010.53");
    if (index >= 0)
    {
        entry += 8 * (uint32_t)index;
        w_u32(entry, 0xFFFFFFFFu);
        return entry;
    }
    return base;
}

uint32_t sub_800309E4(uint32_t first_value, uint32_t second_value, uint32_t first_record, uint32_t second_record, uint32_t kind)
{
    const uint32_t scratch = 0x1F8003F8u;
    uint32_t saved_first = r_u32(scratch);
    uint32_t saved_second = r_u32(scratch + 4);
    uint32_t selected;
    uint32_t first;
    uint32_t second;

    FUNCTION_MARKER(0x800309E4, "SLUS_010.53");
    selected = sub_80031170(scratch, 2, 1, kind);
    first = r_u32(scratch);
    second = r_u32(scratch + 4);
    w_u32(scratch, saved_first);
    w_u32(scratch + 4, saved_second);
    if (selected == 2)
    {
        w_u32(first, 51);
        w_u32(first + 16, r_u32(first + 12));
        w_u32(second, 51);
        w_u32(second + 16, r_u32(second + 12));
        w_u32(first + 20, r_u32(second + 12) + 0x3F60);
        w_u32(second + 20, r_u32(second + 12) + 0x3FE0);
        w_u32(first_value, r_u32(first + 16));
        w_u32(second_value, r_u32(second + 16));
        w_u32(first_record, r_u32(first + 4));
        w_u32(second_record, r_u32(second + 4));
        (void)sub_800322D0(3, 1);
        (void)sub_800322D0(3, 1);
        first = r_u32(0x8005ACBC);
        w_u32(first + 4, r_u32(first + 4) + 2);
        return 1;
    }
    if (selected == 1)
    {
        w_u32(first, 0);
        (void)sub_800322D0(3, 0);
    }
    w_u32(first_value, 0);
    w_u32(second_value, 0);
    w_u32(first_record, 0xFFFFFFFFu);
    w_u32(second_record, 0xFFFFFFFFu);
    return 0;
}

uint32_t sub_80030BCC(uint32_t value_output, uint32_t record_output, uint32_t kind)
{
    const uint32_t scratch = 0x1F8003FCu;
    uint32_t saved = r_u32(scratch);
    uint32_t selected;
    uint32_t record;
    uint32_t value;
    uint32_t base;

    FUNCTION_MARKER(0x80030BCC, "SLUS_010.53");
    selected = sub_80031170(scratch, 1, 1, kind);
    record = r_u32(scratch);
    w_u32(scratch, saved);
    if (selected != 0)
    {
        value = r_u32(record + 12);
        w_u32(record, 51);
        w_u32(record + 16, value);
        w_u32(record + 20, value + 0x3FE0);
        w_u32(value_output, value);
        w_u32(record_output, r_u32(record + 4));
        (void)sub_800322D0(3, 1);
        base = r_u32(0x8005ACBC);
        w_u32(base + 4, r_u32(base + 4) + 1);
    }
    else
    {
        w_u32(value_output, 0);
        w_u32(record_output, 0xFFFFFFFFu);
    }
    return selected;
}

static void frontier_remap_packet(uint32_t packet, uint32_t size, uint32_t mode)
{
    uint16_t clut = r_u16(packet + 10);
    uint16_t special = r_u16(0x800D6160);
    if (clut == special)
    {
        uint8_t delta = r_u8(0x800D6168);
        w_u16(packet + 10, r_u16(0x800D6162));
        if (mode == 0)
        {
            xport_update_u8(packet + 5, XPORT_MEMORY_UPDATE_SUBTRACT, delta);
            xport_update_u8(packet + 9, XPORT_MEMORY_UPDATE_SUBTRACT, delta);
            xport_update_u8(packet + 13, XPORT_MEMORY_UPDATE_SUBTRACT, delta);
            if (size == 24)
                xport_update_u8(packet + 15, XPORT_MEMORY_UPDATE_SUBTRACT, delta);
        }
        return;
    }
    {
        uint32_t mapped = r_u32(0x800D8768u + 4 * (clut & 31));
        uint8_t command;
        if (mode != 0)
            mapped >>= (clut & 0x800) >> 7;
        w_u16(packet + 10, (uint16_t)mapped);
        if (mode == 0)
            return;
        command = r_u8(packet);
        if ((command >> 3) < 15)
            command = (uint8_t)(2 * (command & 0xF8) | (command & 7));
        else
            command = (uint8_t)(16 * ((command >> 3) - 15) | (command & 7));
        w_u8(packet, command);
        xport_update_u8(packet + 5, XPORT_MEMORY_UPDATE_AND, 0x7F);
        xport_update_u8(packet + 9, XPORT_MEMORY_UPDATE_AND, 0x7F);
        xport_update_u8(packet + 13, XPORT_MEMORY_UPDATE_AND, 0x7F);
        if (size == 24)
            xport_update_u8(packet + 15, XPORT_MEMORY_UPDATE_AND, 0x7F);
    }
}

int32_t sub_800348CC(uint32_t record, uint32_t mode)
{
    uint32_t descriptor;
    uint32_t packet;
    uint32_t table;
    uint32_t flags;
    uint32_t first_count;
    uint32_t second_count;
    uint32_t segment;
    uint32_t segment_end;

    FUNCTION_MARKER(0x800348CC, "SLUS_010.53");
    if (r_u32(record + 8) == 0xFFFFFFFFu)
        return -1;
    descriptor = r_u32(record + 4);
    if (descriptor == 0)
        return 0;
    packet = r_u32(descriptor + (mode != 0 ? 56 : 60));
    if (packet == 0)
        return 0;
    flags = r_u32(record + 12);
    if ((flags & (mode != 0 ? 8u : 4u)) == 0)
        return mode != 0 ? -9 : -5;
    flags &= ~(mode != 0 ? 8u : 4u);
    w_u32(record + 12, flags);
    first_count = r_u16(descriptor + 4);
    second_count = r_u16(descriptor + 6);
    table = r_u32(descriptor + 40);
    segment = mode != 0 ? 0 : first_count + second_count;
    segment_end = mode != 0 ? first_count + second_count : first_count + 2 * second_count;
    while (segment < segment_end)
    {
        uint32_t entry = table + 12 * segment;
        uint32_t count = r_u32(entry + 4);
        while ((int32_t)count > 0)
        {
            frontier_remap_packet(packet, 20, mode);
            packet += 20;
            --count;
        }
        count = r_u32(entry + 8);
        while ((int32_t)count > 0)
        {
            frontier_remap_packet(packet, 24, mode);
            packet += 24;
            --count;
        }
        ++segment;
    }
    if (mode != 0)
    {
        flags = r_u32(record + 12) & ~0x40u;
        w_u32(record + 12, flags);
        return (int32_t)flags;
    }
    return 0;
}

uint32_t sub_80033304(uint32_t owner, uint32_t mode, uint32_t group)
{
    uint32_t index;

    FUNCTION_MARKER(0x80033304, "SLUS_010.53");
    for (index = 0; index < 12; ++index)
    {
        uint32_t record = 0x800D87E8u + 1344 * group + 112 * index;
        uint32_t candidate;
        for (candidate = 0; candidate < 2; ++candidate)
        {
            uint32_t offset = mode == 0 ? 80 : 88;
            if (mode <= 1 && r_u32(record + offset + 4 * candidate) == owner)
            {
                (void)sub_800348CC(record, mode);
                break;
            }
        }
    }
    return 0;
}

uint32_t sub_800326BC(int32_t key, uint32_t group)
{
    int32_t mode = key >> 15;
    uint32_t index;

    FUNCTION_MARKER(0x800326BC, "SLUS_010.53");
    for (index = 0; index < 12; ++index)
    {
        uint32_t record = 0x800D87E8u + 1344 * group + 112 * index;
        uint32_t candidate;
        uint32_t offset = mode != 0 ? 96 : 104;
        for (candidate = 0; candidate < 2; ++candidate)
        {
            uint32_t slot = record + offset + 4 * candidate;
            uint32_t linked = r_u32(slot);
            if (linked != 0 && r_u32(linked + 8) == (uint32_t)key)
            {
                (void)sub_800348CC(record, (uint32_t)mode);
                w_u32(slot, 0);
            }
        }
    }
    return 1;
}

int32_t sub_80034D38(int32_t key, uint32_t unused, uint32_t group)
{
    uint32_t index;
    uint32_t record;

    FUNCTION_MARKER(0x80034D38, "SLUS_010.53");
    index = sub_80022218((uint32_t)key, group);
    if (index == 0xFFFFFFFFu)
        return -1;
    (void)sub_800326BC(key, group);
    record = 0x800D9268u + 48 * index;
    (void)sub_80033634((int32_t)r_u32(record), r_u32(record + 4), group);
    w_u32(record + 8, 0xFFFFFFFFu);
    w_u32(record + 12, 0);
    w_u32(record + 16, 0);
    w_u32(record, 0xFFFFFFFFu);
    w_u32(record + 20, 0xFFFFFFFFu);
    (void)sub_8001E100(record + 24, 0xFFFFFFFFu, 24);
    return 1;
}

int32_t sub_80033770(uint32_t page, uint32_t mode, uint32_t record_index, uint32_t group)
{
    const uint32_t outputs = 0x1F8003E0u;
    uint32_t saved[4];
    uint32_t slot;
    uint32_t key;
    uint32_t record = 0x800D9268u + 48 * record_index;
    uint32_t success;
    uint32_t values[4];
    uint32_t index;

    FUNCTION_MARKER(0x80033770, "SLUS_010.53");
    slot = (mode != 0 ? 0x800D76D0u + 32 * group : 0x800D7710u + 24 * group) + 8 * page;
    key = r_u32(slot);
    if (key == 0xFFFFFFFFu)
        return 1;
    (void)sub_80033304(key, mode != 0, group);
    for (index = 0; index < 4; ++index)
        saved[index] = r_u32(outputs + 4 * index);
    if (mode != 0)
        success = sub_80030BCC(outputs, outputs + 8, group);
    else
        success = sub_800309E4(outputs, outputs + 4, outputs + 8, outputs + 12, group);
    for (index = 0; index < 4; ++index)
    {
        values[index] = r_u32(outputs + 4 * index);
        w_u32(outputs + 4 * index, saved[index]);
    }
    w_u32(record, 0xFFFFFFFFu);
    if (success != 0)
    {
        if (mode != 0)
        {
            (void)sub_80022484(page, key, values[0], 0, group);
            w_u32(slot, 0xFFFFFFFFu);
            w_u32(record + 12, values[0]);
            (void)sub_80030C8C(values[2]);
        }
        else
        {
            (void)sub_80022484(page, key, values[0], values[1], group);
            w_u32(slot, 0xFFFFFFFFu);
            w_u32(record + 16, values[1]);
            w_u32(record + 12, values[0]);
            (void)sub_80030B7C(values[2], values[3]);
        }
        return 1;
    }
    if (r_u32(r_u32(0x8005B2F8) + 48) == 2)
    {
        w_u32(0x800D7720, key);
        (void)sub_800225E0(page, key, group);
        return -1;
    }
    index = sub_80022218(key, group);
    sub_80030CB8((int32_t)r_u32(0x800D9268u + 48 * index + 20));
    (void)sub_80034D38((int32_t)key, mode, group);
    return -1;
}

int32_t sub_800339F8(uint32_t mode, uint32_t group)
{
    const uint32_t outputs = 0x1F8003E0u;
    uint32_t saved[4];
    uint32_t values[4];
    uint32_t key = r_u32(0x800D7720);
    uint32_t record_index = sub_80022218(key, group);
    uint32_t selected;
    uint32_t record;
    uint32_t index;

    FUNCTION_MARKER(0x800339F8, "SLUS_010.53");
    if ((int32_t)record_index < 0)
        return -1;
    if (key == 0xFFFFFFFFu)
        return 1;
    for (index = 0; index < 4; ++index)
        saved[index] = r_u32(outputs + 4 * index);
    if (mode != 0)
        selected = sub_80030BCC(outputs, outputs + 8, group);
    else
        selected = sub_800309E4(outputs, outputs + 4, outputs + 8, outputs + 12, group);
    for (index = 0; index < 4; ++index)
    {
        values[index] = r_u32(outputs + 4 * index);
        w_u32(outputs + 4 * index, saved[index]);
    }
    if (selected == 0)
        return -1;
    record = 0x800D9268u + 48 * record_index;
    if (mode != 0)
    {
        (void)sub_80022484(4, key, values[0], 0, 0);
        w_u32(0x800D7720, 0xFFFFFFFFu);
        w_u32(record, 0xFFFFFFFFu);
        w_u32(record + 12, values[0]);
        (void)sub_80030C8C(values[2]);
    }
    else
    {
        (void)sub_80022484(4, key, values[0], values[1], 0);
        w_u32(0x800D7720, 0xFFFFFFFFu);
        w_u32(record, 0xFFFFFFFFu);
        w_u32(record + 16, values[1]);
        w_u32(record + 12, values[0]);
        (void)sub_80030B7C(values[2], values[3]);
    }
    return 1;
}

uint32_t sub_8003367C(uint32_t owner, uint32_t indices, int32_t count, uint32_t mode)
{
    int32_t index;

    FUNCTION_MARKER(0x8003367C, "SLUS_010.53");
    for (index = 0; index < count; ++index)
    {
        uint32_t record = 0x800D87E8u + 112 * r_u32(indices + 4 * (uint32_t)index);
        uint32_t candidate;
        for (candidate = 0; candidate < 2; ++candidate)
        {
            if (r_u32(record + (mode != 0 ? 88 : 80) + 4 * candidate) == owner && (mode != 0 || (r_u32(record + 56) & 0x22222222u) != 0))
                return 1;
        }
    }
    return 0;
}

int32_t sub_8003348C(uint32_t mode, uint32_t group)
{
    int32_t slot_count = (int32_t)r_u32(mode != 0 ? 0x8005AEE8 : 0x8005AEE4);
    uint32_t slots = (mode != 0 ? 0x800D76D0u + 32 * group : 0x800D7710u + 24 * group);
    int32_t slot;

    FUNCTION_MARKER(0x8003348C, "SLUS_010.53");
    for (slot = 0; slot < slot_count; ++slot)
    {
        if (r_u32(slots + 8 * (uint32_t)slot) == 0xFFFFFFFFu)
            return slot;
    }
    for (slot = 0; slot < slot_count; ++slot)
    {
        uint32_t owner = r_u32(slots + 8 * (uint32_t)slot);
        if (owner != 0xFFFFFFFFu && sub_8003367C(owner, 0x800D9B80u + 48 * group, (int32_t)r_u32(0x8005B568u + 4 * group), mode) == 0)
            return slot;
    }
    return -1;
}

int32_t sub_80032F78(uint32_t object, uint32_t mode, uint32_t group)
{
    uint32_t slots;
    int32_t slot_count;
    uint32_t sources;
    uint32_t source_index;
    int32_t transfer_result = 0;

    FUNCTION_MARKER(0x80032F78, "SLUS_010.53");
    if (mode == 0 && (r_u32(object + 56) & 0x22222222u) == 0)
        return 1;
    slots = mode != 0 ? 0x800D76D0u + 32 * group : 0x800D7710u + 24 * group;
    slot_count = (int32_t)r_u32(mode != 0 ? 0x8005AEE8 : 0x8005AEE4);
    sources = object + (mode != 0 ? 88 : 80);
    for (source_index = 0; source_index < 2; ++source_index)
    {
        uint32_t key = r_u32(sources + 4 * source_index);
        int32_t slot_index;
        uint32_t record_index;
        uint32_t record;
        int32_t scan;
        uint32_t found = 0;
        if (key == 0xFFFFFFFFu)
            continue;
        for (scan = 0; scan < slot_count; ++scan)
        {
            if (r_u32(slots + 8 * (uint32_t)scan) == key)
                found = 1;
        }
        if (found != 0)
            continue;
        slot_index = sub_8003348C(mode, group);
        if (slot_index < 0)
            return -1;
        record_index = sub_80022218(key, group);
        if (record_index == 0xFFFFFFFFu)
            return -1;
        record = 0x800D9268u + 48 * record_index;
        if (r_u32(record + 12) == 0 || (mode == 0 && r_u32(record + 16) == 0))
            return -1;
        {
            uint32_t replaced = r_u32(slots + 8 * (uint32_t)slot_index);
            if (replaced != 0xFFFFFFFFu)
            {
                uint32_t replaced_index = sub_80022218(replaced, group);
                if (replaced_index != 0xFFFFFFFFu)
                    transfer_result = sub_80033770((uint32_t)slot_index, mode, replaced_index, group);
            }
        }
        w_u32(slots + 8 * (uint32_t)slot_index, key);
        (void)(uint32_t)sub_80033B50((uint32_t)slot_index, record_index, mode, group);
        if (transfer_result < 0)
            (void)sub_800339F8(mode, group);
    }
    return 1;
}

uint32_t sub_80032D24(uint32_t group)
{
    int32_t entry;
    int32_t active;
    uint32_t list = 0x800D9B80u + 48 * group;

    FUNCTION_MARKER(0x80032D24, "SLUS_010.53");
    active = 0;
    for (entry = 0; entry < ((int32_t)(uint32_t)(r_u32(0x8005B568u + 4 * group))); ++entry)
    {
        int32_t object_index = ((int32_t)(uint32_t)(r_u32(list + 4 * (uint32_t)entry)));
        uint32_t object;
        uint32_t flags;

        if (object_index == -1)
            continue;
        object = 0x800D87E8u + 112 * (uint32_t)object_index;
        flags = r_u32(object + 56);
        if ((flags & 0x22222222u) == 0)
            continue;
        if (active < 4)
        {
            if (sub_80032F78(object, 0, group) == -1)
                w_u32(object + 56, flags & 0xDDDDDDDDu);
            ++active;
        }
        else
        {
            w_u32(object + 56, flags & 0xDDDDDDDDu);
        }
    }

    active = 0;
    for (entry = 0; entry < ((int32_t)(uint32_t)(r_u32(0x8005B568u + 4 * group))); ++entry)
    {
        uint32_t list_entry = list + 4 * (uint32_t)entry;
        int32_t object_index;

        if (active >= 6)
        {
            (void)RRJ_DEBUG_PRINTF((const char *)xport_guest_cptr(0x80010DCCu, 48));
            continue;
        }
        object_index = ((int32_t)(uint32_t)(r_u32(list_entry)));
        if (object_index == -1)
            continue;
        if (sub_80032F78(0x800D87E8u + 112 * (uint32_t)object_index, 1, group) == -1)
            w_u32(list_entry, 0xFFFFFFFFu);
        ++active;
    }

    for (active = 0; active < 2; ++active)
    {
        uint32_t channel = sub_800335B4((uint32_t)active, 0xFFFFFFFFu, group);

        if (channel != 0xFFFFFFFFu)
        {
            int32_t slot = sub_8003328C((uint32_t)active, (int32_t)group);

            if (slot != -1)
                (void)(uint32_t)sub_80033B50(channel, (uint32_t)slot, (uint32_t)active, group);
        }
    }
    return 1;
}

uint32_t sub_80066A84(uint32_t object)
{
    int32_t selector = r_s8(object + 8);
    uint32_t table = r_u32(r_u32(object + 96) + 8);
    uint32_t descriptor = r_u32(table + 12 * (uint32_t)selector);
    uint32_t type = r_u16(descriptor + 24);
    uint32_t slot;
    uint32_t record;

    FUNCTION_MARKER(0x80066A84, "RASHCDG.BIN");
    if (type == 3)
        slot = 2;
    else if (type == 5)
        slot = 3;
    else if (type == 6)
        slot = 4;
    else
        return 0;
    record = r_u32(r_u32(object + 4));
    return r_u32(r_u32(object) + 36) + 8 * r_u32(record + 16) + 4 + 8 * slot;
}

int32_t sub_8001FD24(uint32_t first, uint32_t second, uint32_t third, uint32_t output)
{
    uint32_t first_entry = 0x8005624Cu + 4 * (first & 0xFFFu);
    uint32_t second_entry = 0x8005624Cu + 4 * (second & 0xFFFu);
    uint32_t third_entry = 0x8005624Cu + 4 * (third & 0xFFFu);
    int32_t first_x = (int16_t)r_u16(first_entry);
    int32_t first_y = (int16_t)r_u16(first_entry + 2);
    int32_t second_x = (int16_t)r_u16(second_entry);
    int32_t second_y = (int16_t)r_u16(second_entry + 2);
    int32_t third_x = (int16_t)r_u16(third_entry);
    int32_t third_y = (int16_t)r_u16(third_entry + 2);
    int32_t first_x_second_x = ((int32_t)((int32_t)frontier_asr((uint32_t)((int32_t)(first_x)) * (uint32_t)((int32_t)(second_x)), 12)));
    int32_t third_y_first_y = ((int32_t)((int32_t)frontier_asr((uint32_t)((int32_t)(third_y)) * (uint32_t)((int32_t)(first_y)), 12)));
    int32_t result;

    FUNCTION_MARKER(0x8001FD24, "SLUS_010.53");
    w_u16(output + 12, (uint16_t)(-second_x));
    w_u16(output + 14, (uint16_t)((int32_t)((int32_t)frontier_asr((uint32_t)((int32_t)(first_x)) * (uint32_t)((int32_t)(second_y)), 12))));
    w_u16(output + 16, (uint16_t)((int32_t)((int32_t)frontier_asr((uint32_t)((int32_t)(first_y)) * (uint32_t)((int32_t)(second_y)), 12))));
    w_u16(output + 2, (uint16_t)(((int32_t)((int32_t)frontier_asr((uint32_t)((int32_t)(first_x_second_x)) * (uint32_t)((int32_t)(third_y)), 12))) - ((int32_t)((int32_t)frontier_asr((uint32_t)((int32_t)(third_x)) * (uint32_t)((int32_t)(first_y)), 12)))));
    w_u16(output, (uint16_t)((int32_t)((int32_t)frontier_asr((uint32_t)((int32_t)(third_y)) * (uint32_t)((int32_t)(second_y)), 12))));
    w_u16(output + 8, (uint16_t)(third_y_first_y + ((int32_t)((int32_t)frontier_asr((uint32_t)((int32_t)(first_x_second_x)) * (uint32_t)((int32_t)(third_x)), 12)))));
    w_u16(output + 6, (uint16_t)((int32_t)((int32_t)frontier_asr((uint32_t)((int32_t)(second_y)) * (uint32_t)((int32_t)(third_x)), 12))));
    w_u16(output + 4, (uint16_t)(((int32_t)((int32_t)frontier_asr((uint32_t)((int32_t)(first_x)) * (uint32_t)((int32_t)(third_x)), 12))) + ((int32_t)((int32_t)frontier_asr((uint32_t)((int32_t)(third_y_first_y)) * (uint32_t)((int32_t)(second_x)), 12)))));
    result = ((int32_t)((int32_t)frontier_asr((uint32_t)((int32_t)(((int32_t)((int32_t)frontier_asr((uint32_t)((int32_t)(third_x)) * (uint32_t)((int32_t)(first_y)), 12))))) * (uint32_t)((int32_t)(second_x)), 12))) - ((int32_t)((int32_t)frontier_asr((uint32_t)((int32_t)(first_x)) * (uint32_t)((int32_t)(third_y)), 12)));
    w_u16(output + 10, (uint16_t)result);
    return result;
}

uint32_t sub_80066B98(uint32_t object, uint32_t child_index)
{
    uint32_t child = r_u32(object + 56 + 8 * child_index);
    uint32_t specification = r_u32(child);
    uint32_t scale = r_u16(specification + 14) >> 12;
    uint32_t parent = r_u32(object + 4);
    uint32_t attachment = sub_80066A84(object);
    uint32_t attachment_scale;
    int16_t input[3];
    int32_t transformed[3];
    int32_t difference;
    uint32_t index;

    FUNCTION_MARKER(0x80066B98, "RASHCDG.BIN");
    for (index = 0; index != 3; ++index)
        input[index] = (int16_t)frontier_asr((uint32_t)(int32_t)r_s16(child + 28 + 2 * index), (4u - scale) & 31u);
    frontier_rotate_vector(parent + 52, input, transformed);
    for (index = 0; index != 3; ++index)
        w_u32(child + 12 + 4 * index, (uint32_t)transformed[index]);

    if (attachment != 0)
    {
        attachment_scale = r_u16(r_u32(object) + 14) >> 12;
    }
    else
    {
        uint32_t descriptor = r_u32(r_u32(r_u32(object + 96) + 8));
        uint32_t record = r_u32(parent);
        uint32_t base = r_u32(descriptor + 36) + 4 + 8 * r_u32(record + 16);

        attachment_scale = r_u16(descriptor + 14) >> 12;
        attachment = base + (r_u16(descriptor + 24) < 6 ? 24 : 32);
    }

    difference = (int32_t)scale - (int32_t)attachment_scale;
    for (index = 0; index != 3; ++index)
    {
        int32_t value = (int16_t)r_u16(attachment + 2 * index);
        uint32_t adjusted = difference < 0 ? frontier_asr((uint32_t)value, (uint32_t)(-difference) & 31u) : (uint32_t)value << ((uint32_t)difference & 31u);
        uint32_t base = r_u16(child + 12 + 4 * index);

        input[index] = (int16_t)(base + adjusted);
    }
    if (r_s8(object + 72) == 3)
        frontier_rotate_vector(object + 104, input, transformed);
    else
        frontier_rotate_vector(parent + 4, input, transformed);
    for (index = 0; index != 3; ++index)
    {
        uint32_t value = (uint32_t)transformed[index] + (r_u32(object + 12 + 4 * index) << (scale & 31u));
        w_u32(child + 12 + 4 * index, value);
    }
    frontier_copy_words(child + 104, r_u32(0x1F8002BCu), 32);
    return child + 104;
}

uint32_t sub_80067064(uint32_t mode, uint32_t object)
{
    uint32_t configuration = 0x800CC854u + 3 * mode;
    int32_t inner_count = r_u8(configuration + 1);
    uint32_t stream = 0x800CC790u + 4 * r_u8(configuration);
    int32_t outer_count = r_u8(configuration + 2);
    uint32_t root = r_u32(object + 4);
    uint32_t descriptor_entry = root + 24;
    uint32_t result = 0x800CC790u;

    FUNCTION_MARKER(0x80067064, "RASHCDG.BIN");
    while (outer_count > 0)
    {
        int32_t remaining = inner_count;

        while (remaining > 0)
        {
            uint32_t command = r_u32(stream);
            uint32_t scratch = r_u32(0x1F8002BCu) + 32 * (command & 3u);
            uint32_t model_entry = root + 24 * ((command >> 13) & 31u);
            uint32_t model = r_u32(model_entry);
            uint32_t model_index = r_u32(model + 16);
            uint32_t source = r_u32(0x8005ACB0u) + 16 * model_index + ((command >> 6) & 0x70u);
            uint32_t matrix_input;
            uint32_t matrix_output;
            uint32_t descriptor;
            int16_t rotation[3][3];
            uint32_t row;
            uint32_t column;

            w_u32(scratch + 20, r_u32(source));
            w_u32(scratch + 24, r_u32(source + 4));
            w_u32(scratch + 28, r_u32(source + 8));
            stream += 4;
            matrix_input = scratch + ((command << 3) & 0x60u);
            w_u32(0x1F8002BCu, matrix_input);
            frontier_copy_words(matrix_input, root + 24 * ((command >> 23) & 31u) + 4, 20);
            matrix_output = matrix_input - ((command >> 3) & 0x60u);
            for (row = 0; row != 3; ++row)
                for (column = 0; column != 3; ++column)
                    rotation[row][column] = (int16_t)r_u16(matrix_input - 32 + 2 * (3 * row + column));
            for (column = 0; column != 3; ++column)
            {
                int16_t input[3];
                int32_t transformed[3];

                for (row = 0; row != 3; ++row)
                    input[row] = (int16_t)r_u16(matrix_input + 2 * (3 * row + column));
                frontier_rotate_values(rotation, input, transformed);
                for (row = 0; row != 3; ++row)
                    w_u16(matrix_output + 2 * (3 * row + column), (uint16_t)transformed[row]);
            }

            descriptor = r_u32(descriptor_entry);
            descriptor_entry += 24;
            --remaining;
            inner_count = (int32_t)r_u32(descriptor + 16);
            scratch = r_u32(0x1F8002BCu) - ((command << 1) & 0x60u);
            w_u32(0x1F8002BCu, scratch);
            (void)sub_800220A4(scratch, r_u16(descriptor + 14), r_u32(r_u32(object) + 36) + 8 * (uint32_t)inner_count + 4, r_u32(0x8005ACB0u) + 16 * (uint32_t)inner_count, r_u32(0x8005ACB4u) + 4 * (uint32_t)inner_count, r_u32(0x8005ACB8u) + (uint32_t)inner_count);
            w_u32(0x1F8002BCu, r_u32(0x1F8002BCu) - ((command >> 1) & 0x60u));
        }

        result = 3;
        {
            uint32_t command = r_u32(stream);

            if ((command & 3u) == 3)
            {
                uint32_t linked = r_u32(object + 56);

                stream += 4;
                if (linked != 0 && r_u32(object + 60) == ((command >> 18) & 31u))
                    frontier_copy_words(linked + 104, r_u32(0x1F8002BCu), 32);
                inner_count = (int32_t)((command >> 13) & 31u);
                result = (command >> 1) & 0x60u;
                w_u32(0x1F8002BCu, r_u32(0x1F8002BCu) - result);
            }
        }
        --outer_count;
    }
    return result;
}

uint32_t sub_80066B28(uint32_t object)
{
    uint32_t type = r_u16(r_u32(object) + 24);
    uint32_t scratch;

    FUNCTION_MARKER(0x80066B28, "RASHCDG.BIN");
    if (type == 4)
        return sub_80067064(6, object);
    if (type == 3)
        return sub_80067064(7, object);
    scratch = r_u32(0x1F8002BCu) - 32;
    w_u32(0x1F8002BCu, scratch);
    return scratch;
}

uint32_t sub_8006745C(uint32_t object)
{
    uint32_t type = r_u16(r_u32(object) + 24);

    FUNCTION_MARKER(0x8006745C, "RASHCDG.BIN");
    if (type == 12)
        return sub_80067064(1, object);
    if (type >= 13)
        return type == 17 ? sub_80067064(0, object) : 17;
    if (type == 4)
        return sub_80067064(2, object);
    return 4;
}

uint32_t sub_80066ECC(uint32_t object)
{
    uint32_t context = r_u32(0x8005B2F8u);
    uint32_t scratch = r_u32(0x1F8002BCu);
    uint32_t type = r_u16(r_u32(object) + 24);
    uint32_t parent = r_u32(object + 4);
    uint32_t child_index;

    FUNCTION_MARKER(0x80066ECC, "RASHCDG.BIN");
    if (r_u16(object + 172) < r_u32(context + 48))
        frontier_copy_words(0x800CCD80u, scratch, 32);
    if (type >= 5)
    {
        uint32_t extended = type >= 6;

        (void)sub_8001FD24((uint32_t)(int32_t)r_s16(object + 836), 0, 0, parent + 76);
        (void)sub_8001FD24((uint32_t)(int32_t)r_s16(object + 838), 0, 0, parent + 100);
        if (extended)
        {
            uint32_t linked = r_u32(object + 856);

            (void)sub_8001FD24((uint32_t)(int32_t)r_s16(linked + 836), 0, 0, parent + 124);
        }
        (void)sub_80067064(4 - extended, object);
    }
    else if (type != 1)
    {
        (void)sub_80067064(5, object);
    }
    for (child_index = 0; child_index != 2; ++child_index)
    {
        if (r_u32(object + 56 + 8 * child_index) != 0)
            (void)sub_80066B98(object, child_index);
    }
    if (type != 1)
    {
        scratch = r_u32(0x1F8002BCu) - 32;
        w_u32(0x1F8002BCu, scratch);
    }
    scratch = r_u32(0x1F8002BCu) - 32;
    w_u32(0x1F8002BCu, scratch);
    return scratch;
}

uint32_t sub_80066EC4(uint32_t object)
{
    FUNCTION_MARKER(0x80066EC4, "RASHCDG.BIN");
    return sub_80066ECC(object);
}

uint32_t sub_80029048(uint8_t output[32], const uint8_t center[6], uint32_t angle, int32_t radius)
{
    uint32_t table = 0x8005624C + 4 * (angle & 0xFFF);
    int32_t scaled_radius = (int32_t)((uint32_t)radius << 11);
    int32_t x_offset = (int32_t)(uint32_t)((uint64_t)((int64_t)(int16_t)r_u16(table) * scaled_radius) >> 12) >> 12;
    int32_t y_offset = (int32_t)(uint32_t)((uint64_t)((int64_t)(int16_t)r_u16(table + 2) * scaled_radius) >> 12) >> 12;
    uint32_t z;

    FUNCTION_MARKER(0x80029048, "SLUS_010.53");
    xport_store_le16(output, (uint16_t)(xport_load_le16(center) - (uint32_t)x_offset));
    xport_store_le16(output + 2, (uint16_t)(xport_load_le16(center + 2) + (uint32_t)y_offset));
    xport_store_le16(output + 4, xport_load_le16(center + 4));
    xport_store_le16(output + 8, (uint16_t)(xport_load_le16(center) + (uint32_t)y_offset));
    xport_store_le16(output + 10, (uint16_t)(xport_load_le16(center + 2) + (uint32_t)x_offset));
    xport_store_le16(output + 12, xport_load_le16(center + 4));
    xport_store_le16(output + 24, (uint16_t)(xport_load_le16(center) + (uint32_t)x_offset));
    xport_store_le16(output + 26, (uint16_t)(xport_load_le16(center + 2) - (uint32_t)y_offset));
    xport_store_le16(output + 28, xport_load_le16(center + 4));
    xport_store_le16(output + 16, (uint16_t)(xport_load_le16(center) - (uint32_t)y_offset));
    xport_store_le16(output + 18, (uint16_t)(xport_load_le16(center + 2) - (uint32_t)x_offset));
    z = xport_load_le16(center + 4);
    xport_store_le16(output + 20, (uint16_t)z);
    return z;
}

uint32_t sub_8002990C(uint32_t object, uint32_t output)
{
    uint32_t command = r_u32(output);
    uint32_t type = (command >> 6) & 15u;
    uint32_t vertex_base = r_u32(0x8005ACB0u);

    FUNCTION_MARKER(0x8002990C, "SLUS_010.53");
    if (type == 4)
    {
        uint32_t first = vertex_base + 16 * r_u16(output + 76);
        uint32_t second = vertex_base + 16 * r_u16(output + 78);
        uint32_t index;
        uint32_t result = 0;

        for (index = 0; index != 3; ++index)
        {
            result = (uint32_t)(((int32_t)(uint32_t)(r_u32(first + 4 * index) + r_u32(second + 4 * index))) / 2);
            w_u32(output + 80 + 4 * index, result);
        }
        return result;
    }
    if (type == 3 || type == 6)
    {
        uint32_t root = r_u32(object + 4);
        uint32_t record_index = type == 3 ? (command >> 14) & 0xFFu : r_u16(r_u32(object) + 24) - 1u;
        uint32_t record = r_u32(root + 24 * record_index);
        uint32_t vertex_index = r_u16(record + 16) + (type == 3);
        uint32_t source = vertex_base + 16 * vertex_index;

        w_u16(output + 76, (uint16_t)vertex_index);
        frontier_copy_words(output + 80, source, 16);
        return source;
    }
    if (type != 5)
        return type < 5 ? 3 : 6;
    {
        uint32_t specification = r_u32(object);
        uint32_t scale = r_u16(specification + 14) >> 12;
        int32_t object_part = r_s8(object + 8);
        uint32_t configuration = 0x80053728u + 2 * (uint32_t)object_part + ((command >> 11) & 0x7F8u);
        int32_t model_part = r_s8(configuration);
        uint32_t screen_base = r_u32(0x8005ACB4u);
        uint32_t route;
        int32_t record_index;
        uint32_t model;
        uint32_t part;
        int32_t visible;
        uint32_t first_vertex;
        int32_t extent;

        if (model_part == -1)
        {
            w_u32(output + 92, 0xFFFFFFFFu);
            return 0x80053728u;
        }
        route = 0x800536F0u + 4 * (uint32_t)(int32_t)r_s8(configuration + 1);
        record_index = (int32_t)r_u16(specification + 24) - 1;
        if (r_s8(route) != -1)
            record_index = r_s8(route);
        model = r_u32(r_u32(object + 4) + 24 * (uint32_t)record_index);
        part = r_u32(model + 20) + 20 * (uint32_t)model_part + 4;
        visible = (int32_t)frontier_asr(r_u32(vertex_base + 16 * r_u16(part + 12) + 8), scale & 31u);
        w_u32(output + 92, (uint32_t)visible);
        if (visible <= 0)
            return (uint32_t)visible;
        first_vertex = screen_base + 4 * r_u16(part + 12 + 2 * (uint32_t)(int32_t)r_s8(route + 2));
        if (r_s8(route + 1) == 1)
        {
            w_u16(output + 80, r_u16(first_vertex));
            w_u16(output + 82, r_u16(first_vertex + 2));
        }
        else
        {
            uint32_t second_vertex = screen_base + 4 * r_u16(part + 12 + 2 * (uint32_t)(int32_t)r_s8(route + 3));
            int32_t x = (int16_t)r_u16(first_vertex) + (int16_t)r_u16(second_vertex);
            int32_t y = (int16_t)r_u16(first_vertex + 2) + (int16_t)r_u16(second_vertex + 2);

            w_u16(output + 80, (uint16_t)(x / 2));
            w_u16(output + 82, (uint16_t)(y / 2));
        }
        if (object_part >= 2)
        {
            extent = object_part == 2 ? 6 : 4;
            w_u32(output + 88, (uint32_t)extent);
            return 2;
        }
        {
            uint32_t second_vertex = screen_base + 4 * r_u16(part + 16);
            uint32_t base_vertex = screen_base + 4 * r_u16(part + 12);
            int32_t x = (int16_t)r_u16(base_vertex) - (int16_t)r_u16(second_vertex);
            int32_t y = (int16_t)r_u16(base_vertex + 2) - (int16_t)r_u16(second_vertex + 2);
            int32_t half_x = (x < 0 ? -x : x) >> 1;
            int32_t half_y = (y < 0 ? -y : y) >> 1;

            extent = half_y < half_x ? half_x : half_y;
            w_u32(output + 88, (uint32_t)extent);
            return half_y < half_x;
        }
    }
}

int32_t sub_80029CA4(uint32_t object)
{
    int32_t index = r_s8(object + 73);
    int32_t result = -1;

    FUNCTION_MARKER(0x80029CA4, "SLUS_010.53");
    if (index == -1)
        return result;
    if (r_s8(object + 8) > 0)
    {
        uint32_t entry = 0x800D39B0u + 112 * (uint32_t)index;

        result = ((r_u32(entry) >> 6) & 15u) < 5u;
        if (result != 0)
            return result;
    }
    while (index != -1)
    {
        uint32_t entry = 0x800D39B0u + 112 * (uint32_t)index;
        uint32_t command = r_u32(entry);

        index = ((int32_t)(uint32_t)(command << 26)) >> 26;
        (void)sub_8002990C(object, entry);
        result = 8 * index;
    }
    return result;
}

uint32_t sub_80028534(uint32_t object)
{
    uint32_t specification = r_u32(object);
    uint32_t configuration = 0x800537D8u + 6 * r_u32(object + 180);
    uint32_t root = r_u32(object + 4);
    uint32_t type = r_u16(specification + 24);
    uint32_t record = r_u32(root + 24 * (type - 1));
    uint32_t part = r_u32(record + 20) + 20 * (uint32_t)(int32_t)r_s8(configuration) + 4;
    uint32_t vertex_base = r_u32(0x8005ACB0u);
    uint32_t screen_base = r_u32(0x8005ACB4u);
    uint32_t route = 0x800536F0u + 4 * (uint32_t)(int32_t)r_s8(configuration + 1);
    uint32_t scale = r_u16(specification + 14) >> 12;
    int32_t depth = (int32_t)frontier_asr(r_u32(vertex_base + 16 * r_u16(part + 12) + 8), scale & 31u);

    FUNCTION_MARKER(0x80028534, "SLUS_010.53");
    if (depth > 0)
    {
        uint32_t first = screen_base + 4 * r_u16(part + 12 + 2 * (uint32_t)(int32_t)r_s8(route + 2));
        uint32_t second = screen_base + 4 * r_u16(part + 12 + 2 * (uint32_t)(int32_t)r_s8(route + 3));
        int32_t first_x = (int16_t)r_u16(first);
        int32_t first_y = (int16_t)r_u16(first + 2);
        int32_t second_x = (int16_t)r_u16(second);
        int32_t second_y = (int16_t)r_u16(second + 2);
        int32_t center_x = (first_x + second_x) / 2;
        int32_t center_y = (first_y + second_y) / 2;
        int32_t delta_x = first_x - second_x;
        int32_t delta_y = first_y - second_y;
        int32_t radius;
        uint8_t center[6];
        uint8_t quad[32];
        uint32_t display;
        uint32_t packet;
        uint32_t ordering_table;
        int32_t bucket;
        uint32_t entry;
        uint8_t u0;
        uint8_t v0;
        uint8_t u1;
        uint8_t v1;
        uint16_t tpage;
        uint16_t clut;
        uint32_t previous_tag;

        if (delta_x < 0)
            delta_x = -delta_x;
        if (delta_y < 0)
            delta_y = -delta_y;
        radius = 2 * (delta_y >= delta_x ? delta_y : delta_x);
        if ((((int32_t)(uint32_t)(r_u32(0x80053250u))) >> 2) < depth)
        {
            int32_t quotient = (int32_t)((uint32_t)radius << 12) / depth;
            int32_t scale_value = (int32_t)(r_u32(0x80053250u) << 12);

            radius = (int32_t)(uint32_t)((uint64_t)((int64_t)quotient * scale_value) >> 12) >> 12;
        }
        xport_store_le16(center, (uint16_t)center_x);
        xport_store_le16(center + 2, (uint16_t)center_y);
        xport_store_le16(center + 4, 0);
        (void)sub_80029048(quad, center, (uint32_t)(int32_t)(int16_t)((center_x + center_y) * 8), radius);

        u0 = (uint8_t)r_u32(0x800D42FC);
        v0 = (uint8_t)r_u32(0x800D4300);
        u1 = (uint8_t)(u0 + r_u32(0x800D4304) - 1);
        v1 = (uint8_t)(v0 + r_u32(0x800D4308) - 1);
        tpage = r_u16(0x800D430C);
        clut = r_u16(0x800D4310);
        display = r_u32(0x8005B470u);
        packet = r_u32(display + 268);
        ordering_table = r_u32(display + 264);
        if (packet + 40 >= r_u32(0x8005B4D0u))
            packet = sub_80021C98(packet, 40);
        if (((int32_t)(uint32_t)(r_u32(0x1F800004u))) < 4096)
        {
            int32_t exponent = (int32_t)(r_u32(0x1F800000) + (uint32_t)r_s8(((depth >> 12) > 0 ? 0x1F800019 : 0x1F800018) + ((depth >> 11) > 0)));
            int32_t table_index = exponent;
            int32_t base;

            int32_t distance = (int32_t)(3 - (uint32_t)exponent);

            table_index = (int32_t)((uint32_t)(~(exponent >> 31) & exponent) + (uint32_t)((distance >> 31) & distance));
            base = (int32_t)(r_u16(0x1F800006u + 2 * (uint32_t)table_index) + r_u32(0x1F800004u));
            bucket = r_u16(0x1F80000Eu + 2 * (uint32_t)table_index) + (((int32_t)(uint32_t)((uint32_t)depth - (uint32_t)base)) >> (exponent & 31));
        }
        else
        {
            bucket = ((int32_t)(uint32_t)((uint32_t)depth - r_u32(0x1F800004u))) >> (r_u32(0x1F800000u) & 31u);
        }
        {
            int32_t distance = (int32_t)(r_u32(0x1F80001C) - (uint32_t)bucket);

            bucket = (int32_t)((uint32_t)(~(bucket >> 31) & bucket) + (uint32_t)((distance >> 31) & distance));
        }
        if (bucket >= 3)
            bucket -= 3;
        entry = ordering_table + 4 * (uint32_t)bucket;
        previous_tag = r_u32(entry);
        w_u32(packet + 4, 0x2E000060u);
        w_u32(packet, previous_tag | 0x09000000u);
        w_u32(entry, packet);
        w_u16(packet + 8, xport_load_le16(quad));
        w_u16(packet + 10, xport_load_le16(quad + 2));
        w_u16(packet + 16, xport_load_le16(quad + 8));
        w_u16(packet + 18, xport_load_le16(quad + 10));
        w_u16(packet + 24, xport_load_le16(quad + 16));
        w_u16(packet + 26, xport_load_le16(quad + 18));
        w_u16(packet + 32, xport_load_le16(quad + 24));
        w_u16(packet + 34, xport_load_le16(quad + 26));
        w_u8(packet + 12, u0);
        w_u8(packet + 13, v0);
        w_u8(packet + 20, u1);
        w_u8(packet + 21, v0);
        w_u8(packet + 28, u0);
        w_u8(packet + 29, v1);
        w_u8(packet + 36, u1);
        w_u8(packet + 37, v1);
        w_u16(packet + 22, tpage | 0x20);
        display = r_u32(0x8005B470);
        w_u16(packet + 14, clut);
        w_u32(display + 268, packet + 40);
        return display;
    }
    return 0x800536F0u;
}

uint32_t sub_80027B80(uint32_t object)
{
    int32_t object_part = r_s8(object + 8);
    uint32_t scale = r_u16(r_u32(object) + 14) >> 12;
    uint32_t screen = r_u32(0x8005ACB4u);
    uint32_t texture;
    uint32_t texture_index;
    int32_t x;
    int32_t y;
    int32_t height;
    int32_t width;
    int32_t depth;
    uint32_t result;

    FUNCTION_MARKER(0x80027B80, "SLUS_010.53");
    if (object_part == 6)
    {
        int32_t horizontal = (int16_t)r_u16(screen + 56) - (int16_t)r_u16(screen + 32);
        int32_t vertical = (int16_t)r_u16(screen + 54) - (int16_t)r_u16(screen + 58);

        texture = 0x800D42C4u;
        texture_index = 14;
        x = (int16_t)r_u16(screen + 56);
        y = (int16_t)(r_u16(screen + 58) + 6);
        height = -10;
        if (vertical < 0)
            vertical = -vertical;
        width = horizontal * vertical;
    }
    else if (object_part == 7)
    {
        int32_t horizontal = (int16_t)r_u16(screen + 16) - (int16_t)r_u16(screen + 4);
        int32_t vertical = (int16_t)r_u16(screen + 14) - (int16_t)r_u16(screen + 18);

        texture = 0x800D4388u;
        texture_index = 4;
        x = (int16_t)r_u16(screen + 16);
        y = (int16_t)(r_u16(screen + 18) + 4);
        height = -10;
        if (vertical < 0)
            vertical = -vertical;
        width = 2 * horizontal * vertical;
    }
    else if (object_part == 8)
    {
        int32_t horizontal = (int16_t)r_u16(screen + 4) - (int16_t)r_u16(screen + 16);
        int32_t vertical = (int16_t)r_u16(screen + 10) - (int16_t)r_u16(screen + 6);

        texture = 0x800D42E0u;
        texture_index = 1;
        x = (int16_t)r_u16(screen + 4);
        y = (int16_t)(r_u16(screen + 6) - 4);
        height = 10;
        if (vertical < 0)
            vertical = -vertical;
        width = horizontal * (vertical + 3);
    }
    else
    {
        return 0x800D0000u;
    }
    w_u32(0x8005B530u, texture_index);
    depth = (int32_t)frontier_asr(r_u32(r_u32(0x8005ACB0u) + 16 * texture_index + 8), scale & 31u);
    result = width < 201;
    if (depth > 0)
    {
        uint32_t display;
        uint32_t packet;
        uint32_t ordering_table;
        int32_t bucket;
        uint32_t entry;
        uint8_t u0 = r_u8(texture);
        uint8_t v0 = r_u8(texture + 4);
        uint8_t u1 = (uint8_t)(u0 + r_u32(texture + 8) - 1);
        uint8_t v1 = (uint8_t)(v0 + r_u32(texture + 12) - 1);

        if (width >= 201)
            width = 200;
        if ((((int32_t)(uint32_t)(r_u32(0x80053250u))) >> 2) >= depth)
        {
            if (width < -200)
                width = -200;
        }
        else
        {
            int32_t quotient = (int32_t)((uint32_t)width << 12) / depth;
            int32_t scale_value = (int32_t)(r_u32(0x80053250u) << 12);

            width = (int32_t)(((int64_t)quotient * scale_value) >> 24);
        }
        display = r_u32(0x8005B470u);
        packet = r_u32(display + 268);
        ordering_table = r_u32(display + 264);
        if (packet + 40 >= r_u32(0x8005B4D0u))
            packet = sub_80021C98(packet, 40);
        if (((int32_t)(uint32_t)(r_u32(0x1F800004u))) < 4096)
        {
            int32_t exponent = ((int32_t)(uint32_t)(r_u32(0x1F800000u))) + r_s8((depth >> 12) > 0 ? 0x1F800019u : 0x1F800018u) + ((depth >> 11) > 0);
            int32_t table_index = exponent;
            int32_t base;

            if (table_index < 0)
                table_index = 0;
            if (table_index > 3)
                table_index = 3;
            base = r_u16(0x1F800006u + 2 * (uint32_t)table_index) + ((int32_t)(uint32_t)(r_u32(0x1F800004u)));
            bucket = r_u16(0x1F80000Eu + 2 * (uint32_t)table_index) + (((int32_t)(uint32_t)((uint32_t)(depth - base))) >> (exponent & 31));
        }
        else
        {
            bucket = ((int32_t)(uint32_t)((uint32_t)(depth - ((int32_t)(uint32_t)(r_u32(0x1F800004u)))))) >> (r_u32(0x1F800000u) & 31u);
        }
        if (bucket < 0)
            bucket = 0;
        if (bucket > ((int32_t)(uint32_t)(r_u32(0x1F80001Cu))))
            bucket = ((int32_t)(uint32_t)(r_u32(0x1F80001Cu)));
        entry = ordering_table + 4 * (uint32_t)bucket;
        w_u32(packet + 4, 0x2F000000u);
        w_u32(packet, r_u32(entry) | 0x09000000u);
        w_u32(entry, packet);
        w_u16(packet + 8, (uint16_t)x);
        w_u16(packet + 10, (uint16_t)y);
        w_u16(packet + 16, (uint16_t)(x + width));
        w_u16(packet + 18, (uint16_t)y);
        w_u16(packet + 24, (uint16_t)x);
        w_u16(packet + 26, (uint16_t)(y + height));
        w_u16(packet + 32, (uint16_t)(x + width));
        w_u16(packet + 34, (uint16_t)(y + height));
        w_u8(packet + 12, u0);
        w_u8(packet + 13, v0);
        w_u8(packet + 20, u1);
        w_u8(packet + 21, v0);
        w_u8(packet + 28, u0);
        w_u8(packet + 29, v1);
        w_u8(packet + 36, u1);
        w_u8(packet + 37, v1);
        w_u16(packet + 22, r_u16(texture + 16) | 0x20u);
        w_u16(packet + 14, r_u16(texture + 20));
        w_u32(display + 268, packet + 40);
        return display;
    }
    return result;
}

uint32_t sub_80026960(uint32_t object, uint32_t player_index, uint32_t position, uint32_t direction)
{
    uint32_t temporary = 0x1F800300u;
    uint32_t saved_first = r_u32(temporary);
    uint32_t saved_second = r_u32(temporary + 4);
    int32_t dot = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(direction, 6), xport_guest_ptr(0x80052364u, 6))));
    int16_t plane[3];
    int32_t reciprocal;
    int32_t world[4][3];
    uint32_t player = r_u32(0x8005AEC0u + 4 * player_index);
    uint32_t corner;
    MATRIX matrix;
    sint32 screens[4];
    sint32 depths[4];
    sint32 flags[4];
    int32_t average_depth;
    uint32_t result;

    FUNCTION_MARKER(0x80026960, "SLUS_010.53");
    plane[0] = (int16_t)r_u16(0x80052364u);
    plane[1] = (int16_t)r_u16(0x80052366u);
    plane[2] = (int16_t)r_u16(0x80052368u);
    if ((dot < 0 ? -dot : dot) <= 46339)
    {
        int16_t axis[3];
        int16_t cross[3];
        int16_t adjusted[3];
        uint32_t component;

        for (component = 0; component != 3; ++component)
            axis[component] = (int16_t)r_u16(direction + 2 * component);
        cross[0] = (int16_t)(((int32_t)axis[1] * plane[2] - (int32_t)axis[2] * plane[1]) >> 12);
        cross[1] = (int16_t)(((int32_t)axis[2] * plane[0] - (int32_t)axis[0] * plane[2]) >> 12);
        cross[2] = (int16_t)(((int32_t)axis[0] * plane[1] - (int32_t)axis[1] * plane[0]) >> 12);
        adjusted[0] = (int16_t)(((int32_t)cross[1] * axis[2] - (int32_t)cross[2] * axis[1]) >> 12);
        adjusted[1] = (int16_t)(((int32_t)cross[2] * axis[0] - (int32_t)cross[0] * axis[2]) >> 12);
        adjusted[2] = (int16_t)(((int32_t)cross[0] * axis[1] - (int32_t)cross[1] * axis[0]) >> 12);
        for (component = 0; component != 3; ++component)
            w_u16(temporary + 2 * component, (uint16_t)adjusted[component]);
        (void)sub_8002E468(xport_guest_ptr(temporary, 6));
        (void)sub_8002EB78(xport_guest_ptr(direction, 6), xport_guest_ptr(temporary, 6), xport_guest_ptr(temporary, 6), 46340, 46340);
        for (component = 0; component != 3; ++component)
            plane[component] = (int16_t)r_u16(temporary + 2 * component);
        dot = ((int32_t)(uint32_t)(sub_8002E698(xport_guest_ptr(direction, 6), xport_guest_ptr(temporary, 6))));
    }
    if (dot >= 0)
    {
        int32_t denominator = (dot >> 1) + ((dot - 2) >> 31);
        reciprocal = (int32_t)(0x80000000u / (uint32_t)denominator);
    }
    else
    {
        int32_t magnitude = -dot;
        int32_t denominator = (magnitude >> 1) + ((magnitude - 2) >> 31);
        reciprocal = -(int32_t)(0x80000000u / (uint32_t)denominator);
    }
    for (corner = 0; corner != 4; ++corner)
    {
        uint32_t origin = object + 196 + 12 * (3 - corner);
        uint32_t component;
        uint32_t sum = 0;
        int32_t ratio;

        for (component = 0; component != 3; ++component)
        {
            uint32_t delta = r_u32(position + 4 * component) - r_u32(origin + 4 * component);
            sum += (uint32_t)sub_8001FC90(((int32_t)(uint32_t)(delta)), (int32_t)r_s16(direction + 2 * component) * 16);
        }
        ratio = (int32_t)(((int64_t)((int32_t)(uint32_t)(sum)) * reciprocal) >> 16);
        for (component = 0; component != 3; ++component)
        {
            int32_t offset = (int32_t)sub_8001FC90((int32_t)plane[component] * 16, ratio);
            world[corner][component] = ((int32_t)(uint32_t)(r_u32(origin + 4 * component))) + offset;
        }
    }
    memset(&matrix, 0, sizeof(matrix));
    memcpy(matrix.m, xport_guest_ptr(player + 92, 18), 18);
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);
    for (corner = 0; corner != 4; ++corner)
    {
        uint32_t source_corner = corner < 2 ? corner : 5 - corner;
        SVECTOR vertex;

        vertex.vx = (int16_t)((world[source_corner][0] >> 10) - (int16_t)r_u16(player + 28));
        vertex.vy = (int16_t)((world[source_corner][1] >> 10) - (int16_t)r_u16(player + 32));
        vertex.vz = (int16_t)((world[source_corner][2] >> 10) - (int16_t)r_u16(player + 36));
        vertex.pad = 0;
        depths[corner] = gte_project(&vertex, &screens[corner], &flags[corner]);
    }
    average_depth = (depths[0] + depths[1] + depths[2] + depths[3]) / 4;
    result = (uint32_t)average_depth;
    if ((result & 0x20000000u) == 0)
    {
        uint32_t texture = 0x800D4270u;
        uint32_t display = r_u32(0x8005B470u);
        uint32_t packet = r_u32(display + 268);
        uint32_t ordering_table = r_u32(display + 264);
        int32_t depth = average_depth * 4;
        int32_t bucket;
        uint32_t entry;
        uint8_t u0 = r_u8(texture);
        uint8_t v0 = r_u8(texture + 4);
        uint8_t u1 = (uint8_t)(u0 + r_u32(texture + 8) - 1);
        uint8_t v1 = (uint8_t)(v0 + r_u32(texture + 12) - 1);

        if (packet + 40 >= r_u32(0x8005B4D0u))
            packet = sub_80021C98(packet, 40);
        if (((int32_t)(uint32_t)(r_u32(0x1F800004u))) < 4096)
        {
            int32_t exponent = ((int32_t)(uint32_t)(r_u32(0x1F800000u))) + r_s8((depth >> 12) > 0 ? 0x1F800019u : 0x1F800018u) + ((depth >> 11) > 0);
            int32_t table_index = exponent;
            int32_t base;

            if (table_index < 0)
                table_index = 0;
            if (table_index > 3)
                table_index = 3;
            base = r_u16(0x1F800006u + 2 * (uint32_t)table_index) + ((int32_t)(uint32_t)(r_u32(0x1F800004u)));
            bucket = r_u16(0x1F80000Eu + 2 * (uint32_t)table_index) + (((int32_t)(uint32_t)((uint32_t)(depth - base))) >> (exponent & 31));
        }
        else
        {
            bucket = ((int32_t)(uint32_t)((uint32_t)(depth - ((int32_t)(uint32_t)(r_u32(0x1F800004u)))))) >> (r_u32(0x1F800000u) & 31u);
        }
        if (bucket < 0)
            bucket = 0;
        if (bucket > ((int32_t)(uint32_t)(r_u32(0x1F80001Cu))))
            bucket = ((int32_t)(uint32_t)(r_u32(0x1F80001Cu)));
        entry = ordering_table + 4 * (uint32_t)bucket;
        w_u32(packet + 4, 0x2F000000u);
        w_u32(packet, r_u32(entry) | 0x09000000u);
        w_u32(entry, packet);
        w_u32(packet + 8, (uint32_t)screens[0]);
        w_u32(packet + 16, (uint32_t)screens[1]);
        w_u32(packet + 24, (uint32_t)screens[2]);
        w_u32(packet + 32, (uint32_t)screens[3]);
        w_u8(packet + 12, u0);
        w_u8(packet + 13, v0);
        w_u8(packet + 20, u1);
        w_u8(packet + 21, v0);
        w_u8(packet + 28, u0);
        w_u8(packet + 29, v1);
        w_u8(packet + 36, u1);
        w_u8(packet + 37, v1);
        w_u16(packet + 22, r_u16(texture + 16));
        w_u16(packet + 14, r_u16(texture + 20));
        w_u32(display + 268, packet + 40);
        result = display;
    }
    w_u32(temporary, saved_first);
    w_u32(temporary + 4, saved_second);
    return result;
}

uint32_t sub_80025EE0(uint32_t object, uint32_t player_index, uint32_t position, uint32_t direction)
{
    uint32_t specification = r_u32(object);
    uint32_t vertex_base = r_u32(0x8005ACB0u);
    uint32_t scale = r_u16(specification + 14) >> 12;
    uint32_t list = r_u32(specification + 44);
    uint32_t count = r_u32(list);
    uint32_t display = r_u32(0x8005B470u);
    uint32_t packet = r_u32(display + 268);
    uint32_t refresh = r_u32(0x800CCD78u);
    uint32_t result = list;

    FUNCTION_MARKER(0x80025EE0, "SLUS_010.53");
    if (packet + 36 * count >= r_u32(0x8005B4D0u))
    {
        packet = sub_80021C98(packet, 36 * count);
        w_u32(display + 268, packet);
    }
    if (r_u32(0x800CCDA0u) == refresh)
    {
        uint32_t player = r_u32(0x8005AEC0u + 4 * player_index);
        int16_t input_direction[3];
        int16_t fixed_direction[3];
        int16_t delta[3];
        int32_t rotated_direction32[3];
        int32_t rotated_fixed32[3];
        int32_t rotated_delta[3];
        int16_t rotated_direction[3];
        int16_t rotated_fixed[3];
        int32_t dot_sum = 0;
        int32_t reciprocal;
        int32_t projection;
        uint32_t component;

        for (component = 0; component != 3; ++component)
        {
            input_direction[component] = (int16_t)r_u16(direction + 2 * component);
            fixed_direction[component] = (int16_t)r_u16(0x80052364u + 2 * component);
            delta[component] = (int16_t)((((int32_t)(uint32_t)(r_u32(position + 4 * component))) >> 10) - (int16_t)r_u16(player + 28 + 4 * component));
        }
        frontier_rotate_vector(player + 92, input_direction, rotated_direction32);
        frontier_rotate_vector(player + 92, fixed_direction, rotated_fixed32);
        frontier_rotate_vector(player + 92, delta, rotated_delta);
        for (component = 0; component != 3; ++component)
        {
            rotated_direction[component] = (int16_t)rotated_direction32[component];
            rotated_fixed[component] = (int16_t)rotated_fixed32[component];
            dot_sum += rotated_direction[component] * rotated_fixed[component];
        }
        dot_sum >>= 8;
        w_u32(0x8005B528u, (uint32_t)dot_sum);
        if ((dot_sum < 0 ? -dot_sum : dot_sum) <= 46339)
        {
            int16_t cross[3];
            int16_t adjusted[3];
            uint32_t temporary = 0x1F800300u;
            uint32_t saved_first = r_u32(temporary);
            uint32_t saved_second = r_u32(temporary + 4);

            cross[0] = (int16_t)(((int32_t)rotated_direction[1] * rotated_fixed[2] - (int32_t)rotated_direction[2] * rotated_fixed[1]) >> 12);
            cross[1] = (int16_t)(((int32_t)rotated_direction[2] * rotated_fixed[0] - (int32_t)rotated_direction[0] * rotated_fixed[2]) >> 12);
            cross[2] = (int16_t)(((int32_t)rotated_direction[0] * rotated_fixed[1] - (int32_t)rotated_direction[1] * rotated_fixed[0]) >> 12);
            adjusted[0] = (int16_t)(((int32_t)cross[1] * rotated_direction[2] - (int32_t)cross[2] * rotated_direction[1]) >> 12);
            adjusted[1] = (int16_t)(((int32_t)cross[2] * rotated_direction[0] - (int32_t)cross[0] * rotated_direction[2]) >> 12);
            adjusted[2] = (int16_t)(((int32_t)cross[0] * rotated_direction[1] - (int32_t)cross[1] * rotated_direction[0]) >> 12);
            for (component = 0; component != 3; ++component)
                w_u16(temporary + 2 * component, (uint16_t)adjusted[component]);
            (void)sub_8002E468(xport_guest_ptr(temporary, 6));
            for (component = 0; component != 3; ++component)
                rotated_fixed[component] = (int16_t)r_u16(temporary + 2 * component);
            for (component = 0; component != 3; ++component)
            {
                int32_t first = (int32_t)(((int64_t)rotated_direction[component] * 16 * 46340) >> 16);
                int32_t second = (int32_t)(((int64_t)rotated_fixed[component] * 16 * 46340) >> 16);
                rotated_fixed[component] = (int16_t)((first + second) >> 4);
            }
            dot_sum = 0;
            for (component = 0; component != 3; ++component)
                dot_sum += rotated_direction[component] * rotated_fixed[component];
            dot_sum >>= 8;
            w_u32(temporary, saved_first);
            w_u32(temporary + 4, saved_second);
            w_u32(0x8005B528u, (uint32_t)dot_sum);
        }
        if (dot_sum >= 0)
        {
            int32_t denominator = (dot_sum >> 1) + ((dot_sum - 2) >> 31);
            reciprocal = (int32_t)(0x80000000u / (uint32_t)denominator);
        }
        else
        {
            int32_t magnitude = -dot_sum;
            int32_t denominator = (magnitude >> 1) + ((magnitude - 2) >> 31);
            reciprocal = -(int32_t)(0x80000000u / (uint32_t)denominator);
        }
        w_u32(0x8005B528u, (uint32_t)reciprocal);
        {
            uint32_t scalar = 0;

            for (component = 0; component != 3; ++component)
                scalar += (uint32_t)sub_8001FC90(rotated_delta[component], (int32_t)rotated_direction[component] * 16);
            projection = (int32_t)(((int64_t)((int32_t)(uint32_t)(scalar)) * reciprocal) >> 16);
        }
        for (component = 0; component != 3; ++component)
        {
            int32_t translation = (int32_t)sub_8001FC90(projection, (int32_t)rotated_fixed[component] * 16);
            w_u32(0x800D7FBCu + 4 * component, (uint32_t)translation);
        }
        {
            int32_t diagonal[3];

            for (component = 0; component != 3; ++component)
                diagonal[component] = ((int32_t)((int32_t)frontier_asr((uint32_t)((int32_t)(rotated_direction[component])) * (uint32_t)((int32_t)(rotated_fixed[component])), 12)));
            w_u16(0x800D7FA8u, (uint16_t)(diagonal[1] + diagonal[2]));
            w_u16(0x800D7FAAu, (uint16_t)-((int32_t)((int32_t)frontier_asr((uint32_t)((int32_t)(rotated_fixed[0])) * (uint32_t)((int32_t)(rotated_direction[1])), 12))));
            w_u16(0x800D7FACu, (uint16_t)-((int32_t)((int32_t)frontier_asr((uint32_t)((int32_t)(rotated_fixed[0])) * (uint32_t)((int32_t)(rotated_direction[2])), 12))));
            w_u16(0x800D7FAEu, (uint16_t)-((int32_t)((int32_t)frontier_asr((uint32_t)((int32_t)(rotated_fixed[1])) * (uint32_t)((int32_t)(rotated_direction[0])), 12))));
            w_u16(0x800D7FB0u, (uint16_t)(diagonal[0] + diagonal[2]));
            w_u16(0x800D7FB2u, (uint16_t)-((int32_t)((int32_t)frontier_asr((uint32_t)((int32_t)(rotated_fixed[1])) * (uint32_t)((int32_t)(rotated_direction[2])), 12))));
            w_u16(0x800D7FB4u, (uint16_t)-((int32_t)((int32_t)frontier_asr((uint32_t)((int32_t)(rotated_fixed[2])) * (uint32_t)((int32_t)(rotated_direction[0])), 12))));
            w_u16(0x800D7FB6u, (uint16_t)-((int32_t)((int32_t)frontier_asr((uint32_t)((int32_t)(rotated_fixed[2])) * (uint32_t)((int32_t)(rotated_direction[1])), 12))));
            w_u16(0x800D7FB8u, (uint16_t)(diagonal[0] + diagonal[1]));
        }
        refresh = r_u32(0x800CCD78u);
    }
    w_u32(0x800CCD78u, refresh - 1);
    {
        MATRIX matrix;
        uint32_t indices = list + 4;
        uint32_t reciprocal = r_u32(0x8005B528u);
        uint32_t primitive;

        memset(&matrix, 0, sizeof(matrix));
        memcpy(matrix.m, xport_guest_ptr(0x800D7FA8u, 18), 18);
        matrix.t[0] = ((int32_t)(uint32_t)(r_u32(0x800D7FBCu)));
        matrix.t[1] = ((int32_t)(uint32_t)(r_u32(0x800D7FC0u)));
        matrix.t[2] = ((int32_t)(uint32_t)(r_u32(0x800D7FC4u)));
        SetRotMatrix(&matrix);
        SetTransMatrix(&matrix);
        for (primitive = 0; primitive != count; ++primitive, indices += 8)
        {
            sint32 screens[4];
            sint32 depths[4];
            sint32 flags[4];
            uint32_t vertex;
            int32_t average_depth;
            int32_t bucket;
            uint32_t ordering_table;
            uint32_t entry;

            for (vertex = 0; vertex != 4; ++vertex)
            {
                uint32_t source = vertex_base + 16 * r_u16(indices + 2 * vertex);
                SVECTOR input;
                uint32_t component;
                int16_t *values = &input.vx;

                for (component = 0; component != 3; ++component)
                {
                    int32_t value = ((int32_t)(uint32_t)(r_u32(source + 4 * component)));
                    value >>= scale & 31u;
                    values[component] = (int16_t)(((int64_t)value * ((int32_t)(uint32_t)(reciprocal))) >> 16);
                }
                input.pad = 0;
                depths[vertex] = gte_project_full_depth(&input, &screens[vertex], &flags[vertex]);
            }
            average_depth = AverageZ4(depths[0], depths[1], depths[2], depths[3]);
            average_depth = (int32_t)((uint32_t)average_depth << 2);
            if (average_depth < 0)
                continue;
            bucket = frontier_depth_bucket(average_depth);
            ordering_table = r_u32(display + 264);
            entry = ordering_table + 4 * (uint32_t)bucket;
            packet = r_u32(display + 268);
            w_u32(display + 268, packet + 36);
            w_u32(packet, 0x08000000u);
            w_u32(packet + 4, 0xE1000740u);
            w_u32(packet + 8, 0xE6000003u);
            w_u32(packet + 12, r_u32(0x80052348u) | 0x2A000000u);
            w_u32(packet + 16, (uint32_t)screens[0]);
            w_u32(packet + 20, (uint32_t)screens[1]);
            w_u32(packet + 24, (uint32_t)screens[3]);
            w_u32(packet + 28, (uint32_t)screens[2]);
            w_u32(packet + 32, 0xE6000000u);
            w_u32(packet, (r_u32(packet) & 0xFF000000u) | (r_u32(entry) & 0x00FFFFFFu));
            result = (r_u32(entry) & 0xFF000000u) | (packet & 0x00FFFFFFu);
            w_u32(entry, result);
        }
    }
    return result;
}

uint32_t sub_800251E4(uint32_t object, uint32_t player_index)
{
    uint32_t specification = r_u32(object);
    uint32_t kind = (r_u16(specification + 14) & 0x78u) >> 3;
    uint32_t flags = r_u32(object + 36);
    uint32_t mapping = r_u32(specification + 48);
    uint32_t root;
    uint32_t screen_base;
    uint32_t vertex_base;
    uint32_t clip_base;
    uint32_t display;
    uint32_t packet;
    uint32_t ordering_table;
    uint32_t scale;
    uint32_t group_count;
    uint32_t group;
    uint32_t texture_high = 0;
    uint32_t texture_low = 0;
    uint32_t alternate_texture_high[2] = {0, 0};
    uint32_t alternate_texture_low[2] = {0, 0};
    uint32_t texture_shift;
    uint32_t use_gouraud;
    uint32_t command_base;
    uint32_t result;

    FUNCTION_MARKER(0x800251E4, "SLUS_010.53");
    if (mapping != 0)
        mapping += 4;
    if (kind == 5 && r_s8(object + 8) >= 6)
    {
        uint32_t linked = r_u32(object + 52);
        uint32_t first_index = r_u8(linked + 569);
        uint32_t second_index = r_u8(linked + 570);
        uint32_t table = r_u32(r_u32(0x8005AD4Cu) + 12 * first_index + 8);
        uint32_t descriptor = table + 12 * r_u8(linked + second_index + 574);

        if (r_u16(descriptor + 8) != 0 && ((int32_t)(uint32_t)(r_u32(r_u32(linked + 540) + 16))) >= 9)
            (void)sub_80027B80(object);
    }
    {
        int32_t primitive = r_s8(object + 73);

        if (primitive != -1)
        {
            uint32_t primitive_kind = (r_u32(0x800D39B0u + 112 * (uint32_t)primitive) >> 6) & 15u;

            if (r_s8(object + 8) < 2 || primitive_kind == 5 || primitive_kind == 6)
                (void)sub_80029CA4(object);
        }
    }
    if (kind == 2 && ((flags >> 18) & 1u) != 0 && r_u8(object + 8) == 0)
        (void)sub_80028534(object);
    {
        uint32_t texture = r_u32(0x8005B2E4u) + 12 * (uint32_t)(int32_t)r_s16(object + 74);

        if (((flags >> 10) & 1u) != 0)
        {
            uint32_t context = r_u32(0x8005B2F8u);
            uint32_t configuration = 0x80053254u + 176 * r_u32(context + 48) - 176;
            int32_t selector;
            int32_t offset;
            int32_t row;

            if (((flags >> 11) & 1u) != 0)
            {
                int32_t adjust = kind == 2 ? -1 : 0;
                uint32_t animation = r_u32(object + 180);

                selector = animation - 9u >= 9u ? 44 + adjust : 45 + 2 * adjust;
            }
            else
            {
                selector = (int32_t)((flags >> 12) & 0x3Fu);
            }
            offset = selector % r_u8(configuration + 116) * r_u8(configuration + 118);
            row = 511 - selector / 3;
            w_u32(0x1F8000A0u, (uint32_t)(((row << 6) | (((int16_t)r_u16(configuration + 112) + offset + 640) >> 4)) << 16));
            texture_high = r_u16(texture + 8) << 16;
            texture_low = r_u8(texture + 2) | ((uint32_t)r_u8(texture + 3) << 8);
            if (kind == 2)
            {
                alternate_texture_high[0] = texture_high;
                alternate_texture_low[0] = texture_low;
                alternate_texture_low[1] = 0;
                alternate_texture_high[1] = r_u16(r_u32(0x8005B348u) + 8) << 16;
            }
        }
        else
        {
            uint32_t index;
            uint32_t packed = r_u16(texture + 4);
            uint32_t texture_v = r_u16(texture + 6);
            uint32_t shift = packed & 3u;

            for (index = 0; index != 40; ++index)
            {
                uint32_t value = (((((packed >> 4) & 0x1FFu) - (index >> shift)) << 6) | ((texture_v + 16 * (index & (packed >> 2) & 3u)) >> 4)) << 16;
                w_u32(0x1F8000A0u + 4 * index, value);
            }
            texture_high = r_u16(texture + 8) << 16;
            texture_low = r_u8(texture + 2) | ((uint32_t)r_u8(texture + 3) << 8);
        }
    }
    texture_shift = 0;
    if (((flags >> 5) & 3u) != 0)
        texture_shift = ((flags >> 11) & 1u) == 0;
    w_u32(0x1F80020Cu, texture_shift);
    use_gouraud = 0;
    if ((r_u8(r_u32(0x8005B2F8u) + 4) & 0x10u) == 0 && mapping != 0)
        use_gouraud = ((flags >> 11) & 1u) == 0;
    {
        uint32_t command_index = r_u16(0x80052380u);

        if (kind == 3)
            command_index += 8;
        command_index >>= texture_shift;
        command_base = r_u32(0x1F800020u + 4 * command_index);
    }
    display = r_u32(0x8005B470u);
    root = r_u32(object + 4);
    screen_base = r_u32(0x8005ACB4u);
    vertex_base = r_u32(0x8005ACB0u);
    specification = r_u32(object);
    scale = r_u16(specification + 14) >> 12;
    packet = r_u32(display + 268);
    ordering_table = r_u32(display + 264);
    group_count = r_u16(specification + 24);
    for (group = 0; group < group_count; ++group)
    {
        uint32_t model = r_u32(root + 24 * group);
        uint32_t face_list = r_u32(model + 20);
        uint32_t face_count = r_u32(face_list);
        uint32_t face;

        if (packet + 52 * face_count >= r_u32(0x8005B4D0u))
            packet = sub_80021C98(packet, 52 * face_count);
        face_list = r_u32(model + 20);
        for (face = 0; face != face_count; ++face)
        {
            uint32_t record = face_list + 20 * face;
            uint32_t face_flags = r_u32(record + 4);
            uint32_t indices[4];
            uint32_t positions[4];
            uint8_t clips[4];
            uint32_t index;
            uint32_t depth_sum;
            int32_t depth;
            int32_t bucket;
            uint32_t entry;
            uint32_t selected_high = texture_high;
            uint32_t selected_low = texture_low;
            uint32_t uv_base;
            uint32_t uv_pair;
            uint32_t texture_word;

            indices[0] = r_u16(record + 16);
            indices[1] = r_u16(record + 18);
            indices[2] = r_u16(record + 20);
            indices[3] = r_u16(record + 22);
            if (kind == 2)
            {
                uint32_t selection = (face_flags >> 21) & 4u;

                selected_high = alternate_texture_high[selection >> 2];
                selected_low = alternate_texture_low[selection >> 2];
            }
            for (index = 0; index != 4; ++index)
            {
                positions[index] = r_u32(screen_base + 4 * indices[index]);
            }
            depth_sum = r_u32(vertex_base + 16 * indices[0] + 8) + r_u32(vertex_base + 16 * indices[1] + 8) + 2 * r_u32(vertex_base + 16 * indices[2] + 8);
            depth = ((int32_t)(uint32_t)(depth_sum)) >> ((scale + 2) & 31u);
            {
                int32_t near_depth = ((int32_t)(uint32_t)(r_u32(0x1F800004u)));
                int32_t exponent = ((int32_t)(uint32_t)(r_u32(0x1F800000u)));

                if (near_depth < 4096)
                {
                    uint32_t selector = ((depth >> 12) > 0) + ((depth >> 11) > 0);
                    int32_t table_index;
                    uint32_t base;

                    exponent = ((int32_t)(uint32_t)((uint32_t)exponent + (uint32_t)r_s8(0x1F800018u + selector)));
                    table_index = exponent < 0 ? 0 : (exponent > 3 ? 3 : exponent);
                    base = r_u16(0x1F800006u + 2 * (uint32_t)table_index) + (uint32_t)near_depth;
                    bucket = ((int32_t)(uint32_t)((uint32_t)r_u16(0x1F80000Eu + 2 * (uint32_t)table_index) + (uint32_t)(((int32_t)(uint32_t)((uint32_t)depth - base)) >> (exponent & 31))));
                }
                else
                    bucket = ((int32_t)(uint32_t)((uint32_t)depth - (uint32_t)near_depth)) >> (exponent & 31);
                if (bucket < 0)
                    bucket = 0;
                if (bucket > ((int32_t)(uint32_t)(r_u32(0x1F80001Cu))))
                    bucket = ((int32_t)(uint32_t)(r_u32(0x1F80001Cu)));
            }
            entry = ordering_table + 4 * (uint32_t)bucket;
            if (((int32_t)(uint32_t)(face_flags)) >= 0)
            {
                xport_gte_write_data(12, positions[0]);
                xport_gte_write_data(14, positions[2]);
                xport_gte_write_data(13, positions[1]);
                xport_gte_execute(0x1400006);
                if (((int32_t)(uint32_t)(xport_gte_read_data(24))) < 0)
                    continue;
            }
            clip_base = r_u32(0x8005ACB8u);
            for (index = 0; index != 4; ++index)
                clips[index] = r_u8(clip_base + indices[index]);
            if ((face_flags & 0x20000000u) != 0)
            {
                if ((clips[0] & clips[1] & clips[2] & clips[3] & 0xFu) != 0)
                    continue;
            }
            else if ((clips[0] & clips[1] & clips[2] & 0xFu) != 0)
            {
                continue;
            }
            texture_word = r_u32(0x1F8000A0u + 4 * r_u16(record + 10));
            uv_base = r_u16(record + 8) + selected_low;
            uv_pair = r_u32(record + 12);
            if ((face_flags & 0x20000000u) != 0)
            {
                if (!use_gouraud)
                {
                    w_u32(packet, r_u32(entry) | 0x09000000u);
                    w_u32(entry, packet);
                    w_u32(packet + 4, command_base | 0x2C000000u);
                    w_u32(packet + 8, positions[0]);
                    w_u32(packet + 12, uv_base | texture_word);
                    w_u32(packet + 16, positions[1]);
                    w_u32(packet + 20, ((uint16_t)face_flags + selected_low) | selected_high);
                    w_u32(packet + 24, positions[3]);
                    w_u16(packet + 28, (uint16_t)((uv_pair >> 16) + selected_low));
                    w_u32(packet + 32, positions[2]);
                    w_u16(packet + 36, (uint16_t)((uint16_t)uv_pair + selected_low));
                    packet += 40;
                }
                else
                {
                    uint32_t colors[4];

                    for (index = 0; index != 4; ++index)
                    {
                        uint32_t color_index = r_u8(0x1F800140u + r_u16(mapping + 2 * indices[index]));
                        colors[index] = r_u32(0x1F800020u + 4 * (color_index >> texture_shift));
                    }
                    w_u32(packet, r_u32(entry) | 0x0C000000u);
                    w_u32(entry, packet);
                    w_u32(packet + 4, colors[0] | 0x3C000000u);
                    w_u32(packet + 8, positions[0]);
                    w_u32(packet + 12, uv_base | texture_word);
                    w_u32(packet + 16, colors[1]);
                    w_u32(packet + 20, positions[1]);
                    w_u32(packet + 24, ((uint16_t)face_flags + selected_low) | selected_high);
                    w_u32(packet + 28, colors[3]);
                    w_u32(packet + 32, positions[3]);
                    w_u16(packet + 36, (uint16_t)((uv_pair >> 16) + selected_low));
                    w_u32(packet + 40, colors[2]);
                    w_u32(packet + 44, positions[2]);
                    w_u16(packet + 48, (uint16_t)((uint16_t)uv_pair + selected_low));
                    packet += 52;
                }
            }
            else if (use_gouraud)
            {
                uint32_t colors[3];

                for (index = 0; index != 3; ++index)
                {
                    uint32_t color_index = r_u8(0x1F800140u + r_u16(mapping + 2 * indices[index]));
                    colors[index] = r_u32(0x1F800020u + 4 * (color_index >> texture_shift));
                }
                w_u32(packet, r_u32(entry) | 0x09000000u);
                w_u32(entry, packet);
                w_u32(packet + 4, colors[0] | 0x34000000u);
                w_u32(packet + 8, positions[0]);
                w_u32(packet + 12, uv_base | texture_word);
                w_u32(packet + 16, colors[1]);
                w_u32(packet + 20, positions[1]);
                w_u32(packet + 24, ((uint16_t)face_flags + selected_low) | selected_high);
                w_u32(packet + 28, colors[2]);
                w_u32(packet + 32, positions[2]);
                w_u16(packet + 36, (uint16_t)((uint16_t)uv_pair + selected_low));
                packet += 40;
            }
            else
            {
                w_u32(packet, r_u32(entry) | 0x07000000u);
                w_u32(entry, packet);
                w_u32(packet + 4, command_base | 0x24000000u);
                w_u32(packet + 8, positions[0]);
                w_u32(packet + 12, uv_base | texture_word);
                w_u32(packet + 16, positions[1]);
                w_u32(packet + 20, ((uint16_t)face_flags + selected_low) | selected_high);
                w_u32(packet + 24, positions[2]);
                w_u16(packet + 28, (uint16_t)((uint16_t)uv_pair + selected_low));
                packet += 32;
            }
        }
        group_count = r_u16(r_u32(object) + 24);
    }
    w_u32(r_u32(0x8005B470u) + 268, packet);
    result = (flags >> 7) & 1u;
    if (result != 0)
    {
        uint32_t linked = r_u32(object + 52);
        uint32_t position;
        uint32_t direction;

        if (linked != 0)
        {
            position = linked + 504;
            direction = linked + 522;
        }
        else
        {
            direction = object + 522;
            position = object + 504;
            if (kind == 3 || ((kind == 1 || kind == 4) && (r_u32(object + 552) & 0x40000000u) == 0))
                position = object + 184;
        }
        if (r_u32(r_u32(0x8005B2F8u) + 48) == 1)
            return sub_80025EE0(object, player_index, position, direction);
        return sub_80026960(object, player_index, position, direction);
    }
    return result;
}

uint32_t sub_80068468(uint32_t object, uint32_t player_index)
{
    uint32_t specification = r_u32(object);
    uint32_t type = (r_u16(specification + 14) & 0x78u) >> 3;
    uint32_t scratch = 0x1F800300u;
    uint32_t root = r_u32(object + 4);
    uint32_t current;
    uint32_t index;

    FUNCTION_MARKER(0x80068468, "RASHCDG.BIN");
    w_u32(0x1F8002BCu, scratch);
    frontier_copy_words(scratch, object + 136, 32);
    scratch += 32;
    w_u32(0x1F8002BCu, scratch);
    for (index = 0; index != 3; ++index)
        w_u32(scratch + 20 + 4 * index, r_u32(object + 80 + 4 * index));

    if (r_s8(object + 72) == 3)
    {
        for (index = 0; index != 5; ++index)
            xport_gte_write_control(index, r_u32(object + 104 + 4 * index));
        for (index = 0; index != 3; ++index)
        {
            uint32_t column = r_u32(object + 4) + 4 + 2 * index;
            uint32_t row;

            for (row = 0; row != 3; ++row)
                xport_gte_write_data(9 + row, r_u16(column + 6 * row));
            xport_gte_execute(0x49E012);
            for (row = 0; row != 3; ++row)
                w_u16(object + 104 + 6 * row + 2 * index, (uint16_t)xport_gte_read_data(9 + row));
        }
        frontier_copy_words(scratch, object + 104, 20);
        if ((type == 1 || type == 4) && r_u32(object + 52) == 0 && ((int32_t)(uint32_t)(r_u32(object + 44 + 4 * player_index))) < 1280)
        {
            uint32_t output = 0x1F800240u;

            (void)sub_800669E8(object, output);
            for (index = 0; index != 3; ++index)
                w_u32(object + 244 + 4 * index, (r_u32(output + 4 * index) << 10) - r_u32(object + 184 + 4 * index));
            w_u16(object + 262, (uint16_t)-(int16_t)r_u16(object + 104));
            w_u16(object + 264, (uint16_t)-(int16_t)r_u16(object + 110));
            w_u16(object + 266, (uint16_t)-(int16_t)r_u16(object + 116));
            w_u16(object + 268, (uint16_t)-(int16_t)r_u16(object + 106));
            w_u16(object + 270, (uint16_t)-(int16_t)r_u16(object + 112));
            w_u16(object + 272, (uint16_t)-(int16_t)r_u16(object + 118));
            w_u16(object + 256, r_u16(object + 108));
            w_u16(object + 258, r_u16(object + 114));
            w_u16(object + 260, r_u16(object + 120));
            w_u8(object + 547, 1);
        }
    }
    else
    {
        frontier_copy_words(scratch, root + 4, 20);
    }

    scratch = r_u32(0x1F8002BCu);
    for (index = 0; index != 5; ++index)
        xport_gte_write_control(index, r_u32(scratch - 32 + 4 * index));
    for (index = 0; index != 3; ++index)
    {
        uint32_t column = r_u32(0x1F8002BCu) + 2 * index;
        uint32_t row;

        for (row = 0; row != 3; ++row)
            xport_gte_write_data(9 + row, r_u16(column + 6 * row));
        xport_gte_execute(0x49E012);
        for (row = 0; row != 3; ++row)
            w_u16(column + 6 * row, (uint16_t)xport_gte_read_data(9 + row));
    }
    if (r_u32(object + 76) != 0x10000u)
    {
        int32_t x_scale = (int16_t)(r_u32(object + 76) >> 4);
        uint32_t row;

        for (row = 0; row != 3; ++row)
        {
            int32_t value = (int16_t)r_u16(scratch + 6 * row);

            w_u16(scratch + 6 * row, (uint16_t)((int32_t)((int32_t)frontier_asr((uint32_t)((int32_t)(value)) * (uint32_t)((int32_t)(x_scale)), 12))));
        }
    }
    root = r_u32(object + 4);
    specification = r_u32(object);
    current = r_u32(root);
    (void)sub_800220A4(scratch, r_u16(current + 14), r_u32(specification + 36) + 8 * r_u32(current + 16) + 4, r_u32(0x8005ACB0u) + 16 * r_u32(current + 16), r_u32(0x8005ACB4u) + 4 * r_u32(current + 16), r_u32(0x8005ACB8u) + r_u32(current + 16));

    specification = r_u32(object);
    if ((r_u8(r_u32(0x8005B2F8u) + 4) & 0x10u) == 0 && r_u32(specification + 40) != 0)
    {
        uint32_t camera = r_u32(0x8005AEC0u + 4 * player_index) + 92;
        uint32_t transposed = 0x1F800240u;
        uint32_t normals;
        int32_t count;
        int16_t light[3];
        int32_t shade;
        uint32_t normal_index;

        (void)sub_80070F9C(camera, transposed);
        for (index = 0; index != 5; ++index)
            xport_gte_write_control(index, r_u32(transposed + 4 * index));
        xport_gte_write_data(0, r_u32(0x80052364u));
        xport_gte_write_data(1, r_u32(0x80052368u));
        xport_gte_execute(0x486012);
        for (index = 0; index != 3; ++index)
            light[index] = (int16_t)xport_gte_read_data(25 + index);
        normals = r_u32(r_u32(object) + 40);
        for (index = 0; index != 3; ++index)
            xport_gte_write_data(9 + index, r_u16(normals + 4 + 2 * index));
        xport_gte_write_control(16, (uint16_t)light[0] | ((uint32_t)(uint16_t)light[1] << 16));
        xport_gte_write_control(17, (uint32_t)(int32_t)light[2]);
        xport_gte_execute(0x45E012);
        shade = ((int32_t)(uint32_t)(xport_gte_read_data(25))) >> 8;
        count = ((int32_t)(uint32_t)(r_u32(r_u32(r_u32(object) + 40))));
        for (normal_index = 1; ((int32_t)(uint32_t)(normal_index)) < count; ++normal_index)
        {
            int32_t value;

            normals = r_u32(r_u32(object) + 40);
            for (index = 0; index != 3; ++index)
                xport_gte_write_data(9 + index, r_u16(normals + 4 + 8 * normal_index + 2 * index));
            xport_gte_write_control(16, (uint16_t)light[0] | ((uint32_t)(uint16_t)light[1] << 16));
            xport_gte_write_control(17, (uint32_t)(int32_t)light[2]);
            xport_gte_execute(0x45E012);
            value = shade < 0 ? 0 : shade >> 11;
            if (value >= 32)
                value = 31;
            w_u8(0x1F80013Fu + normal_index, (uint8_t)value);
            shade = ((int32_t)(uint32_t)(xport_gte_read_data(25))) >> 8;
        }
        {
            int32_t value = shade < 0 ? 0 : shade >> 11;

            if (value >= 32)
                value = 31;
            w_u8(0x1F80013Fu + normal_index, (uint8_t)value);
        }
    }
    switch (type)
    {
        case 1:
        case 4:
            (void)sub_8006745C(object);
            break;
        case 2:
        {
            uint32_t limit = r_u32(object + 596);
            uint32_t flags = r_u32(object + 36);

            if (limit == 0 || ((r_u32(object + 560) & 0x08000000u) != 0 && ((int32_t)(uint32_t)(r_u32(object + 600))) >= ((int32_t)(uint32_t)(limit << 1))))
                flags &= ~0x00040000u;
            else
                flags |= 0x00040000u;
            w_u32(object + 36, flags);
            (void)sub_80066EC4(object);
            break;
        }
        case 3:
        case 5:
        case 6:
            (void)sub_80066B28(object);
            break;
        default:
            break;
    }
    w_u32(0x1F8002BCu, r_u32(0x1F8002BCu) - 32);
    (void)sub_800251E4(object, player_index);
    for (index = 0; index != 2; ++index)
    {
        uint32_t child = r_u32(object + 56 + 8 * index);

        if (child != 0 && ((type != 1 && type != 4) || r_u16(r_u32(object) + 24) == 17))
        {
            (void)sub_80067AC4(child, player_index);
            (void)sub_80068468(child, player_index);
        }
    }
    return 0;
}

uint32_t sub_8006780C(uint32_t object, uint32_t player_index)
{
    uint32_t render_slot = sub_80066A84(object);
    uint16_t saved_x = 0;
    uint16_t saved_y = 0;

    FUNCTION_MARKER(0x8006780C, "RASHCDG.BIN");
    if (render_slot != 0)
    {
        uint32_t configuration = r_u32(object + 556);
        int32_t timer = ((int32_t)(uint32_t)(r_u32(object + 612)));
        int32_t angle = (int16_t)r_u16(object + 846);
        uint32_t table_angle = (uint32_t)angle & 0xFFFu;
        int32_t selection = (int32_t)((uint32_t)angle << 16) > 0 ? -1 : 0;
        int32_t lateral_x;
        int32_t lateral_y;
        int32_t cosine;
        int32_t sine;
        int32_t x_offset;
        int32_t y_offset;
        uint32_t scale = r_u16(r_u32(object) + 14) >> 12;

        saved_x = r_u16(render_slot + 4);
        saved_y = r_u16(render_slot + 2);
        if ((r_u32(object + 568) & 0x600u) != 0)
        {
            timer += 655;
        }
        else if (timer != 0)
        {
            uint32_t limit;
            uint32_t phase;
            uint32_t magnitude;

            if (angle <= 0)
            {
                limit = r_u16(configuration + 436);
                if (angle <= -(int32_t)limit)
                {
                    timer = 0;
                    goto timer_ready;
                }
                magnitude = (uint32_t)-angle;
            }
            else
            {
                limit = r_u16(configuration + 438);
                if (angle >= (int32_t)limit)
                {
                    timer = 0;
                    goto timer_ready;
                }
                magnitude = (uint32_t)angle;
            }
            if (limit != 0)
                phase = 0x10000u - sub_80010028(magnitude, limit);
            else
                phase = sub_80010028(magnitude, 0) + 0x10000u;
            timer = (int32_t)sub_8001FC90(3932, (int32_t)phase);
        }
    timer_ready:
        if (timer < 0)
            timer = 0;
        if (timer > 3932)
            timer = 3932;
        w_u32(object + 612, (uint32_t)timer);

        lateral_x = (selection & (((int32_t)(uint32_t)(r_u32(configuration + 256))) + ((int32_t)(uint32_t)(r_u32(configuration + 248))))) - ((int32_t)(uint32_t)(r_u32(configuration + 248)));
        lateral_y = (selection & (((int32_t)(uint32_t)(r_u32(configuration + 260))) + ((int32_t)(uint32_t)(r_u32(configuration + 252))))) - ((int32_t)(uint32_t)(r_u32(configuration + 252)));
        cosine = 0x10000 - ((int32_t)r_s16(0x8005624Eu + 4 * table_angle) << 4);
        sine = (int32_t)r_s16(0x8005624Cu + 4 * table_angle) << 4;
        x_offset = ((int32_t)sub_8001FC90(lateral_x, cosine) - (int32_t)sub_8001FC90(lateral_y, sine)) >> 10;
        y_offset = ((int32_t)sub_8001FC90(lateral_x, sine) + (int32_t)sub_8001FC90(lateral_y, cosine) + timer) >> 10;
        w_u16(render_slot + 4, (uint16_t)(saved_x - ((uint32_t)x_offset << scale)));
        w_u16(render_slot + 2, (uint16_t)(saved_y - ((uint32_t)y_offset << scale)));
    }
    (void)sub_80068468(object, player_index);
    if (render_slot != 0)
    {
        w_u16(render_slot + 4, saved_x);
        w_u16(render_slot + 2, saved_y);
    }
    return 1;
}

uint32_t sub_80067690(uint32_t owner, uint32_t player_index)
{
    uint32_t object = r_u32(0x8005B280u);
    uint32_t result = 0x80060000u;

    FUNCTION_MARKER(0x80067690, "RASHCDG.BIN");
    while (object != 0)
    {
        if (r_u32(object + 176) == owner)
        {
            uint32_t group = (r_u8(object + 9) & 0x30u) >> 4;

            w_u32(0x800CCD78u, group);
            w_u32(0x800CCDA0u, group);
            result = sub_80068468(object, player_index);
        }
        object = r_u32(object + 168);
    }
    return result;
}

static void frontier_transform_effect_position(uint32_t position, uint16_t output[3], uint32_t player_index)
{
    uint32_t player = r_u32(0x8005AEC0u + 4 * player_index);
    int16_t input[3];
    MATRIX identity;
    uint32_t row;

    FUNCTION_MARKER(0x80028E8C, "SLUS_010.53");
    for (row = 0; row != 3; ++row)
        input[row] = (int16_t)(r_u16(position + 4 * row) - r_u16(player + 28 + 4 * row));
    for (row = 0; row != 3; ++row)
    {
        int64_t value = 0;
        uint32_t column;

        for (column = 0; column != 3; ++column)
            value += (int16_t)r_u16(player + 92 + 6 * row + 2 * column) * input[column];
        output[row] = (uint16_t)(value >> 12);
    }
    memset(&identity, 0, sizeof(identity));
    identity.m[0][0] = 4096;
    identity.m[1][1] = 4096;
    identity.m[2][2] = 4096;
    SetRotMatrix(&identity);
    SetTransMatrix(&identity);
}

uint32_t sub_80028E8C(uint32_t position, uint32_t output, uint32_t player_index)
{
    frontier_transform_effect_position(position, (uint16_t *)xport_guest_ptr(output, 6), player_index);
    return output;
}

uint32_t sub_80029174(uint32_t vertices, uint32_t position, uint32_t center, uint32_t player_index)
{
    uint16_t transformed[3];
    int16_t depth;

    FUNCTION_MARKER(0x80029174, "SLUS_010.53");
    frontier_transform_effect_position(position, transformed, player_index);
    depth = (int16_t)transformed[2];
    if (depth < 64)
        depth = 64;
    w_u16(vertices + 8, r_u16(center) + 13);
    w_u16(vertices + 10, r_u16(center + 2));
    w_u16(vertices + 12, r_u16(center + 4));
    w_u16(vertices, r_u16(center) - 13);
    w_u16(vertices + 2, r_u16(center + 2));
    w_u16(vertices + 4, r_u16(center + 4));
    w_u16(vertices + 24, transformed[0] + 13);
    w_u16(vertices + 26, transformed[1]);
    w_u16(vertices + 28, (uint16_t)depth);
    w_u16(vertices + 16, transformed[0] - 13);
    w_u16(vertices + 18, transformed[1]);
    w_u16(vertices + 20, (uint16_t)depth);
    return 0;
}

static int32_t frontier_project_effect_quad(uint32_t vertices, int32_t screens[4])
{
    int32_t total = 0;
    uint32_t index;

    for (index = 0; index != 4; ++index)
    {
        uint32_t source = vertices + 8 * index;
        SVECTOR vertex;
        sint32 flags;

        vertex.vx = (int16_t)r_u16(source);
        vertex.vy = (int16_t)r_u16(source + 2);
        vertex.vz = (int16_t)r_u16(source + 4);
        vertex.pad = 0;
        total += gte_project(&vertex, &screens[index], &flags);
    }
    return (total / 4) * 4;
}

uint32_t sub_8002926C(uint32_t vertices, uint32_t flags, uint32_t texture_index, uint32_t clut_index, uint32_t command)
{
    uint32_t texture = 0x800D4270u + 28 * texture_index;
    uint32_t display = r_u32(0x8005B470u);
    uint32_t packet = r_u32(display + 268);
    uint32_t ordering_table = r_u32(display + 264);
    uint32_t vertex;
    int32_t depth;
    int32_t bucket;
    uint32_t entry;
    uint8_t u0;
    uint8_t v0;
    uint8_t u1;
    uint8_t v1;

    FUNCTION_MARKER(0x8002926C, "SLUS_010.53");
    if (packet + 40 >= r_u32(0x8005B4D0u))
        packet = sub_80021C98(packet, 40);
    xport_gte_write_data(0u, r_u32(vertices));
    xport_gte_write_data(1u, r_u32(vertices + 4u));
    xport_gte_execute(0x180001u);
    w_u32(packet + 8u, xport_gte_read_data(14u));
    for (vertex = 0u; vertex != 3u; ++vertex)
    {
        xport_gte_write_data(2u * vertex, r_u32(vertices + 8u * (vertex + 1u)));
        xport_gte_write_data(2u * vertex + 1u, r_u32(vertices + 8u * (vertex + 1u) + 4u));
    }
    xport_gte_execute(0x280030u);
    w_u32(packet + 16u, xport_gte_read_data(12u));
    w_u32(packet + 24u, xport_gte_read_data(13u));
    w_u32(packet + 32u, xport_gte_read_data(14u));
    xport_gte_execute(0x168002Eu);
    depth = (int32_t)(xport_gte_read_data(7u) << 2u);
    bucket = frontier_depth_bucket(depth);
    if (bucket < 0)
        bucket = 0;
    if (bucket >= ((int32_t)(uint32_t)(r_u32(0x8005ADFCu))))
        bucket = ((int32_t)(uint32_t)(r_u32(0x8005ADFCu))) - 1;
    entry = ordering_table + 4 * (uint32_t)bucket;
    w_u32(packet, r_u32(entry) | 0x09000000u);
    w_u32(entry, packet);
    w_u32(packet + 4, (flags & 1u) != 0 ? command | 0x2E000000u : 0x2F000000u);
    u0 = r_u8(texture);
    v0 = r_u8(texture + 4);
    u1 = (uint8_t)(u0 + r_u8(texture + 8) - 1);
    v1 = (uint8_t)(v0 + r_u8(texture + 12) - 1);
    w_u8(packet + 12, u0);
    w_u8(packet + 13, v1);
    w_u8(packet + 20, u0);
    w_u8(packet + 21, v0);
    w_u8(packet + 28, u1);
    w_u8(packet + 29, v1);
    w_u8(packet + 36, u1);
    w_u8(packet + 37, v0);
    w_u16(packet + 22, (flags & 2u) != 0 ? (r_u16(texture + 16) & 0xFF9Fu) | 0x20u : r_u16(texture + 16) | 0x60u);
    w_u16(packet + 14, r_u16(texture + 20 + 2 * clut_index));
    w_u32(display + 268, packet + 40);
    return display;
}

uint32_t sub_800295AC(uint32_t object, uint32_t vertices, uint32_t flags, uint32_t texture_index, uint32_t uv_variant)
{
    uint32_t texture = 0x800D4270u + 28 * texture_index;
    uint32_t offsets = 0x80053858u + 8 * uv_variant;
    uint32_t display = r_u32(0x8005B470u);
    uint32_t packet = r_u32(display + 268);
    uint32_t ordering_table = r_u32(display + 264);
    int32_t screens[4];
    int32_t bucket;
    uint32_t entry;
    uint32_t index;

    FUNCTION_MARKER(0x800295AC, "SLUS_010.53");
    if (packet + 40 >= r_u32(0x8005B4D0u))
        packet = sub_80021C98(packet, 40);
    bucket = frontier_depth_bucket(frontier_project_effect_quad(vertices, screens));
    if (bucket < 0)
        bucket = 0;
    if (bucket >= ((int32_t)(uint32_t)(r_u32(0x8005ADFCu))))
        bucket = ((int32_t)(uint32_t)(r_u32(0x8005ADFCu))) - 1;
    entry = ordering_table + 4 * (uint32_t)bucket;
    w_u32(packet, r_u32(entry) | 0x09000000u);
    w_u32(entry, packet);
    w_u32(packet + 4, (flags & 1u) != 0 ? 0x2E000000u : 0x2F000000u);
    for (index = 0; index != 4; ++index)
    {
        w_u32(packet + 8 + 8 * index, (uint32_t)screens[index]);
        w_u8(packet + 12 + 8 * index, (uint8_t)(r_u8(texture) + r_u8(offsets + 2 * index)));
        w_u8(packet + 13 + 8 * index, (uint8_t)(r_u8(texture + 4) + r_u8(offsets + 2 * index + 1)));
    }
    w_u16(packet + 22, (flags & 2u) != 0 ? (r_u16(texture + 16) & 0xFF9Fu) | 0x20u : r_u16(texture + 16) | 0x60u);
    w_u16(packet + 14, r_u16(texture + 20));
    w_u32(display + 268, packet + 40);
    return display;
}

uint32_t sub_8002AB14(uint32_t object, uint32_t record)
{
    uint32_t mode = (r_u32(record) >> 10) & 0xFu;
    int32_t depth = ((int32_t)(uint32_t)(r_u32(record + 92)));
    int32_t radius;
    uint32_t quad = 0x1F800200u;
    uint32_t texture;
    uint32_t display;
    uint32_t packet;
    uint32_t ordering_table;
    int32_t bucket;
    uint32_t entry;
    uint8_t u0;
    uint8_t v0;
    uint8_t u1;
    uint8_t v1;

    FUNCTION_MARKER(0x8002AB14, "SLUS_010.53");
    if (mode == 0 || depth <= 0)
        return depth > 0 ? mode : (uint32_t)depth;
    if (r_s8(object + 8) < 2)
    {
        uint32_t diameter = 2 * r_u16(record + 88);

        w_u16(record + 62, (uint16_t)diameter);
        if ((((int32_t)(uint32_t)(r_u32(0x80053250u))) >> 2) < depth)
        {
            int32_t quotient = (int32_t)((r_u32(record + 88) << 12) / (uint32_t)depth);

            w_u32(record + 88, (uint32_t)(((int64_t)quotient * ((int64_t)((int32_t)(uint32_t)(r_u32(0x80053250u))) << 12)) >> 24));
        }
        radius = ((int32_t)(uint32_t)(r_u32(record + 88)));
        diameter = r_u16(record + 62);
        if ((int32_t)diameter >= radius)
        {
            if (diameter < 6)
                radius = 12;
            else
                radius = 2 * (int32_t)diameter;
        }
        else if (radius < 6)
        {
            radius = 12;
        }
        else
        {
            radius *= 2;
        }
        w_u32(record + 88, (uint32_t)radius);
    }
    if (r_s8(record + 97) != 0)
        w_u32(record + 88, r_u32(record + 88) << 1);
    w_u16(record + 84, 0);
    (void)sub_80029048((uint8_t *)xport_guest_ptr(quad, 32), (const uint8_t *)xport_guest_cptr(record + 80, 6), (uint32_t)(8 * ((int16_t)r_u16(record + 80) + (int16_t)r_u16(record + 82))), ((int32_t)(uint32_t)(r_u32(record + 88))));

    texture = 0x800D4270u + 28 * (uint32_t)(int32_t)r_s8(record + 96);
    display = r_u32(0x8005B470u);
    packet = r_u32(display + 268);
    ordering_table = r_u32(display + 264);
    if (packet + 40 >= r_u32(0x8005B4D0u))
        packet = sub_80021C98(packet, 40);
    bucket = frontier_depth_bucket(depth);
    if (bucket >= 3)
        bucket -= 3;
    entry = ordering_table + 4 * (uint32_t)bucket;
    w_u32(packet + 4, r_u32(0x800536C4u + 4 * (uint32_t)(int32_t)r_s8(record + 97)) | 0x2E000000u);
    w_u32(packet, r_u32(entry) | 0x09000000u);
    w_u32(entry, packet);
    w_u32(packet + 8, r_u32(quad));
    w_u32(packet + 16, r_u32(quad + 8));
    w_u32(packet + 24, r_u32(quad + 16));
    w_u32(packet + 32, r_u32(quad + 24));
    u0 = r_u8(texture);
    v0 = r_u8(texture + 4);
    u1 = (uint8_t)(u0 + r_u8(texture + 8) - 1);
    v1 = (uint8_t)(v0 + r_u8(texture + 12) - 1);
    w_u8(packet + 12, u0);
    w_u8(packet + 13, v0);
    w_u8(packet + 20, u1);
    w_u8(packet + 21, v0);
    w_u8(packet + 28, u0);
    w_u8(packet + 29, v1);
    w_u8(packet + 36, u1);
    w_u8(packet + 37, v1);
    w_u16(packet + 22, r_u16(texture + 16) | 0x20u);
    w_u16(packet + 14, r_u16(texture + 20));
    w_u32(display + 268, packet + 40);
    return display;
}

uint32_t sub_8002A2C8(uint32_t object, uint32_t record, uint32_t age, uint32_t player_index, uint32_t output_flags)
{
    uint32_t type = (r_u32(record) >> 6) & 0xFu;
    uint32_t center = 0x1F800240u;
    uint32_t position = 0x1F800260u;
    uint32_t quad = 0x1F800200u;
    int32_t variant;
    uint32_t component;
    uint32_t result = type;

    FUNCTION_MARKER(0x8002A2C8, "SLUS_010.53");
    variant = (int16_t)r_u16(0x800538B0u + 2 * (uint32_t)(int32_t)r_s8(object + 534));
    switch (type)
    {
        case 1:
            (void)sub_80028E8C(record + 4, center, player_index);
            for (component = 0; component != 3; ++component)
                w_u32(position + 4 * component, r_u32(record + 20 + 4 * component) - r_u32(record + 36 + 4 * component));
            if (sub_80029174(quad, position, center, player_index) == 0)
                result = sub_8002926C(quad, 2, 3, 0, 25700);
            else
                result = 25700;
            break;
        case 2:
        {
            uint32_t required = ((r_u32(record) >> 14) & 0xFFu) == 2 ? 4u : 8u;

            if ((r_u32(object + 564) & required) != 0 && (r_u32(output_flags) & 4u) == 0)
            {
                uint32_t context = r_u32(0x8005B2F8u);
                uint32_t duration = r_u32(record + 52);
                uint32_t scale = duration != 0 ? ((r_u32(context + 16) - r_u32(record + 48)) << 17) / duration : 0;

                (void)sub_8002EE50(scale, xport_guest_ptr(object + 450, 6), xport_guest_ptr(record + 36, 12));
                for (component = 0; component != 3; ++component)
                    w_u32(position + 4 * component, r_u32(record + 4 + 4 * component) - (uint32_t)(((int32_t)(uint32_t)(r_u32(record + 36 + 4 * component))) >> 10));
                w_u32(output_flags, r_u32(output_flags) | 4u);
                (void)sub_80028E8C(record + 4, center, player_index);
                if (sub_80029174(quad, position, center, player_index) == 0)
                    (void)sub_8002926C(quad, 0, 1, variant == 2, 25650);
            }
            result = age < 76;
            if (((r_u32(record) >> 14) & 0xFFu) == 0)
            {
                (void)sub_80029048((uint8_t *)xport_guest_ptr(quad, 32), (const uint8_t *)xport_guest_cptr(record + 68, 6), (uint32_t)(int32_t)r_s16(record + 56), r_u16(record + 62) + (int32_t)age);
                result = sub_8002926C(quad, 0, 2, (uint32_t)(variant + (age >= 76)), 25650);
            }
            break;
        }
        case 3:
            (void)sub_80028E8C(record + 4, center, player_index);
            for (component = 0; component != 3; ++component)
                w_u32(position + 4 * component, r_u32(record + 20 + 4 * component) - r_u32(record + 36 + 4 * component));
            result = sub_80029174(quad, position, center, player_index);
            if (result == 0)
                result = sub_8002926C(quad, 0, 2, (uint32_t)variant, 0);
            break;
        case 4:
            (void)sub_80028E8C(record + 4, center, player_index);
            for (component = 0; component != 3; ++component)
                w_u32(position + 4 * component, r_u32(record + 4 + 4 * component) - r_u32(record + 36 + 4 * component));
            result = sub_80029174(quad, position, center, player_index);
            if (result == 0)
                result = sub_800295AC(record, quad, 2, 8, r_u8(record + 104 + r_u8(record + 108)));
            break;
        case 6:
            (void)sub_80029048((uint8_t *)xport_guest_ptr(quad, 32), (const uint8_t *)xport_guest_cptr(record + 68, 6), (uint32_t)(int32_t)r_s16(record + 56), r_u16(record + 62) + 25);
            result = sub_8002926C(quad, 0, 9, age & 1u, 25650);
            break;
        case 7:
            if ((r_u8(record + 60) & 1u) != 0)
                variant += r_u32(object + 480) <= 0x7FFFFu;
            else
                variant = (r_u32(record + 52) >> 1) < age;
            if ((int16_t)r_u16(record + 72) < 0)
                w_u32(record, (r_u32(record) & 0xFFFFC3FFu) | 0x1000u);
            (void)sub_80029048((uint8_t *)xport_guest_ptr(quad, 32), (const uint8_t *)xport_guest_cptr(record + 68, 6), (uint32_t)(int32_t)r_s16(record + 56), r_u16(record + 62) + (int32_t)(age >> 1));
            result = sub_8002926C(quad, 0, 2, (uint32_t)variant, 0);
            break;
        default:
            break;
    }
    return result;
}

uint32_t sub_80028E74(uint32_t record)
{
    uint32_t result = r_u16(record + 56) + r_u16(record + 58);

    FUNCTION_MARKER(0x80028E74, "SLUS_010.53");
    w_u16(record + 56, (uint16_t)result);
    return result;
}

uint32_t sub_800289E8(uint32_t object, uint32_t scale, uint32_t output)
{
    int32_t scaled[3];
    uint32_t component;
    int32_t result = 0;

    FUNCTION_MARKER(0x800289E8, "SLUS_010.53");
    scaled[0] = (int32_t)(((int64_t)((int32_t)(uint32_t)(r_u32(object + 304))) * ((int32_t)(uint32_t)(r_u32(scale)))) >> 16);
    scaled[1] = (int32_t)(((int64_t)((int32_t)(uint32_t)(r_u32(object + 312))) * ((int32_t)(uint32_t)(r_u32(scale + 4)))) >> 16);
    scaled[2] = (int32_t)(((int64_t)((int32_t)(uint32_t)(r_u32(object + 308))) * ((int32_t)(uint32_t)(r_u32(scale + 8)))) >> 16);
    for (component = 0; component != 3; ++component)
    {
        uint32_t row_offset = 2 * component;
        int64_t value = ((int32_t)(uint32_t)(r_u32(object + 184 + 4 * component)));

        value += ((int64_t)(int16_t)r_u16(object + 432 + row_offset) * scaled[0]) >> 12;
        value += ((int64_t)(int16_t)r_u16(object + 438 + row_offset) * scaled[1]) >> 12;
        value += ((int64_t)(int16_t)r_u16(object + 444 + row_offset) * scaled[2]) >> 12;
        result = (int32_t)value >> 10;
        w_u32(output + 4 * component, (uint32_t)result);
    }
    return (uint32_t)result;
}

uint32_t sub_80028C78(uint32_t object, uint32_t output, uint32_t input, uint32_t player_index)
{
    uint32_t player = r_u32(0x8005AEC0u + 4 * player_index);
    uint32_t shift = r_u16(r_u32(object) + 14) >> 12;
    int16_t vector[3];
    MATRIX matrix;
    uint32_t row;
    uint32_t column;
    int32_t result = 0;

    FUNCTION_MARKER(0x80028C78, "SLUS_010.53");
    vector[0] = (int16_t)frontier_asr(r_u32(input), shift);
    vector[1] = (int16_t)(((int32_t)(frontier_asr(r_u32(input + 4), shift) << 16) >> 4) / 3413);
    vector[2] = (int16_t)frontier_asr(r_u32(input + 8), shift);
    memset(&matrix, 0, sizeof(matrix));
    for (row = 0; row != 3; ++row)
        for (column = 0; column != 3; ++column)
            matrix.m[row][column] = (int16_t)r_u16(player + 92 + 6 * column + 2 * row);
    for (row = 0; row != 3; ++row)
        matrix.m[row][1] = (int16_t)(((int32_t)matrix.m[row][1] << 12) / 3413);
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);
    for (row = 0; row != 3; ++row)
    {
        int64_t value = 0;

        for (column = 0; column != 3; ++column)
            value += matrix.m[row][column] * vector[column];
        result = (int32_t)(value >> 12) + ((int32_t)(uint32_t)(r_u32(player + 28 + 4 * row)));
        w_u32(output + 4 * row, (uint32_t)result);
    }
    return (uint32_t)result;
}

uint32_t sub_8002A8E4(uint32_t age, uint32_t record)
{
    int32_t state = r_s8(record + 97);
    uint32_t result = r_u32(0x800536D0u + 4 * (uint32_t)state) < age;

    FUNCTION_MARKER(0x8002A8E4, "SLUS_010.53");
    if (result != 0)
    {
        w_u32(record + 48, r_u32(r_u32(0x8005B2F8u) + 16));
        w_u32(record, (r_u32(record) & 0xFFFFC3FFu) | 0x800u);
        if (state != 0)
        {
            uint8_t next = (uint8_t)(r_u8(record + 97) - 1);

            w_u8(record + 97, next != 0 ? next : 2);
            result = 2;
        }
        else
        {
            result = r_u32(record);
        }
    }
    return result;
}

uint32_t sub_80017F64(uint32_t actor_id, uint32_t listener)
{
    uint32_t table = r_u32(0x8005B410u + 4 * listener);
    uint32_t metadata = r_u32(0x8005B40Cu) + 72 * listener;
    uint32_t first;
    uint32_t end;
    uint32_t slot;
    uint32_t selected = UINT32_MAX;
    uint32_t protected_count = 0;
    uint32_t record;
    uint32_t actor_entry;
    uint32_t actor;
    uint32_t sound;

    FUNCTION_MARKER(0x80017F64, "SLUS_010.53");
    if (table == 0)
        return 0;
    first = r_u32(metadata + 36);
    end = first + r_u32(metadata + 40);
    for (slot = first; slot < end; ++slot)
    {
        record = table + 44 * slot;
        if (r_u16(record) == actor_id && ((int32_t)(uint32_t)(r_u32(record + 8))) >= 3 && r_u32(record + 4) != 0)
            return r_u32(record + 4);
    }
    for (slot = first; slot < end; ++slot)
    {
        record = table + 44 * slot;
        if (r_u32(record + 4) == 0)
        {
            selected = slot;
            break;
        }
        if (r_u8(r_u32(0x8005B2F8u) + 4) == 33 && ((int32_t)(uint32_t)(r_u32(record + 8))) >= 3 && ++protected_count >= 2)
            return 0;
    }
    if (selected == UINT32_MAX)
        selected = first;
    record = table + 44 * selected;
    if (r_u32(record + 4) != 0)
        (void)sub_80017814(selected, listener);
    w_u16(record, (uint16_t)actor_id);
    actor_entry = 0x800CE4D0u + 16 * (uint32_t)((int32_t)actor_id >> 5);
    actor = r_u32(actor_entry) + r_u32(actor_entry + 4) * (actor_id & 31u);
    w_u32(record + 12, r_u32(actor + 184));
    w_u32(record + 16, r_u32(actor + 192));
    w_u32(record + 20, (uint32_t)(((int64_t)(int16_t)r_u16(actor + 450) * (((int32_t)(uint32_t)(r_u32(actor + 480))) >> 4)) >> 8));
    w_u32(record + 24, (uint32_t)(((int64_t)(int16_t)r_u16(actor + 454) * (((int32_t)(uint32_t)(r_u32(actor + 480))) >> 4)) >> 8));
    sound = r_u32(0x8005B420u);
    w_u32(record + 28, sub_8001F934((int32_t)sound, 11, 0));
    w_u32(record + 8, r_u32(actor + 180) != 0 ? (actor_id & 1u) + 3u : 5u);
    return sub_8001769C(selected, listener, sound, 11);
}

uint32_t sub_800182B0(uint32_t listener)
{
    uint32_t metadata = r_u32(0x8005B40Cu) + 72 * listener;
    uint32_t first = r_u32(metadata + 36);
    uint32_t end = first + r_u32(metadata + 40);
    uint32_t table = r_u32(0x8005B410u + 4 * listener);
    uint32_t slot;

    FUNCTION_MARKER(0x800182B0, "SLUS_010.53");
    if (table != 0)
    {
        for (slot = first; slot < end; ++slot)
        {
            uint32_t record = table + 44 * slot;

            if (((int32_t)(uint32_t)(r_u32(record + 8))) >= 3 && r_u16(record) == listener)
                return listener;
        }
    }
    return sub_80017F64(listener, listener);
}

uint32_t sub_8001836C(uint32_t listener)
{
    uint32_t metadata = r_u32(0x8005B40Cu) + 72 * listener;
    uint32_t first = r_u32(metadata + 36);
    uint32_t end = first + r_u32(metadata + 40);
    uint32_t table = r_u32(0x8005B410u + 4 * listener);
    uint32_t slot;

    FUNCTION_MARKER(0x8001836C, "SLUS_010.53");
    if (table == 0)
        return 0;
    for (slot = first; slot < end; ++slot)
    {
        uint32_t record = table + 44 * slot;

        if (((int32_t)(uint32_t)(r_u32(record + 8))) >= 3 && r_u16(record) == listener)
            (void)sub_80017814(slot, listener);
    }
    return 0;
}

uint32_t sub_8002A974(uint32_t object, uint32_t record, uint32_t age, uint32_t player_index)
{
    uint32_t type = (r_u16(r_u32(object) + 14) & 0x78u) >> 3;
    uint32_t linked;
    uint32_t listener;
    uint32_t context;

    FUNCTION_MARKER(0x8002A974, "SLUS_010.53");
    (void)sub_8002A8E4(age, record);
    if (type != 2)
        return 1;
    linked = r_u32(object + 852);
    listener = r_u16(object + 172);
    context = r_u32(0x8005B2F8u);
    if (r_u32(linked + 604) >= 3 || (r_u16(0x800541D4u + 8 * r_u16(linked + 544) + 2) == 0 && r_u16(linked + 544) == 0))
    {
        w_u32(record, r_u32(record) & 0xFFFFC3FFu);
        w_u32(object + 36, r_u32(object + 36) & 0xCFFFFFFFu);
        if (listener < r_u32(context + 48))
            (void)sub_8001836C(listener);
    }
    if ((int16_t)(r_u32(object + 44 + 4 * player_index) >> 6) >= 36)
        w_u32(record, r_u32(record) & 0xFFFFC3FFu);
    if (((r_u32(record) >> 10) & 0xFu) == 0)
        return 0;
    w_u32(object + 36, r_u32(object + 36) | 0x30000000u);
    if (listener < r_u32(context + 48))
        (void)sub_800182B0(listener);
    return 1;
}

uint32_t sub_80029D88(uint32_t object, uint32_t record, uint32_t player_index, uint32_t age)
{
    uint32_t command = r_u32(record);
    uint32_t type = (command >> 6) & 0xFu;
    uint32_t context = r_u32(0x8005B2F8u);
    uint32_t current_time = r_u32(context + 16);
    uint32_t reset = 0;
    uint32_t result;

    FUNCTION_MARKER(0x80029D88, "SLUS_010.53");
    if (type != 5 && r_u32(record + 52) < age)
    {
        w_u32(record, (command & 0xFFFFC3FFu) | 0x1000u);
        return r_u32(record);
    }
    if (r_u8(record + 109) != 0 && type != 5 && r_u32(record + 96) < current_time - r_u32(record + 100))
    {
        uint8_t frame = r_u8(record + 108);
        uint8_t count = r_u8(record + 109);

        w_u8(record + 108, frame == count - 1 ? 0 : frame + 1);
        w_u32(record + 100, current_time);
    }
    switch (type)
    {
        case 1:
        {
            uint32_t subtype = (r_u32(record) >> 14) & 0xFFu;
            uint32_t target = object;
            uint32_t scale = 0x80053670u + 12 * subtype;

            if (subtype == 3 && r_u32(object + 856) != 0 && r_u32(object + 1088) != 0)
            {
                target = r_u32(object + 856);
                scale = 0x800536B8u;
            }
            (void)sub_800289E8(target, scale, record + 4);
            if (((r_u32(record) >> 10) & 0xFu) == 1)
            {
                w_u32(record, (r_u32(record) & 0xFFFFC3FFu) | 0x800u);
                w_u32(record + 20, r_u32(record + 4));
                w_u32(record + 24, r_u32(record + 8));
                w_u32(record + 28, r_u32(record + 12));
            }
            w_u32(record + 36, r_u32(record + 4) - r_u32(record + 20));
            w_u32(record + 40, r_u32(record + 8) - r_u32(record + 24));
            w_u32(record + 44, r_u32(record + 12) - r_u32(record + 28));
            (void)sub_8002E810(13107, xport_guest_cptr(record + 36, 12), xport_guest_ptr(record + 36, 12));
            reset = (r_u32(object + 568) & 0x600u) != 0;
            break;
        }
        case 2:
        {
            uint32_t subtype = (r_u32(record) >> 14) & 0xFFu;

            (void)sub_800289E8(object, 0x80053670u + 12 * subtype, record + 4);
            if (((r_u32(record) >> 10) & 0xFu) == 1)
            {
                w_u32(record, (r_u32(record) & 0xFFFFC3FFu) | 0x800u);
                w_u32(record + 20, r_u32(record + 4));
                w_u32(record + 24, r_u32(record + 8));
                w_u32(record + 28, r_u32(record + 12));
            }
            w_u32(record + 36, r_u32(record + 4) - r_u32(record + 20));
            w_u32(record + 40, r_u32(record + 8) - r_u32(record + 24));
            w_u32(record + 44, r_u32(record + 12) - r_u32(record + 28));
            (void)sub_8002E810(19660, xport_guest_cptr(record + 36, 12), xport_guest_ptr(record + 36, 12));
            reset = (r_u32(object + 568) & 0x600u) != 0;
            (void)sub_80028E74(record);
            break;
        }
        case 3:
            (void)sub_80028C78(object, record + 4, record + 80, player_index);
            if (((r_u32(record) >> 10) & 0xFu) == 1)
            {
                w_u32(record, (r_u32(record) & 0xFFFFC3FFu) | 0x800u);
                w_u32(record + 20, r_u32(record + 4));
                w_u32(record + 24, r_u32(record + 8));
                w_u32(record + 28, r_u32(record + 12));
            }
            w_u32(record + 36, r_u32(record + 4) - r_u32(record + 20));
            w_u32(record + 40, r_u32(record + 8) - r_u32(record + 24));
            w_u32(record + 44, r_u32(record + 12) - r_u32(record + 28));
            (void)sub_8002E810(0x8000u, xport_guest_cptr(record + 36, 12), xport_guest_ptr(record + 36, 12));
            reset = (r_u32(object + 568) & 0x600u) != 0;
            break;
        case 4:
        {
            uint32_t duration = r_u32(record + 52);
            int32_t phase = duration != 0 ? (int32_t)(((current_time - r_u32(record + 48)) << 17) / duration) : 0;

            (void)sub_80028C78(object, record + 4, record + 80, player_index);
            if (((r_u32(record) >> 10) & 0xFu) == 1)
                w_u32(record, (r_u32(record) & 0xFFFFC3FFu) | 0x800u);
            if (phase > 0x10000)
                phase = 0x20000 - phase;
            (void)sub_8002EE50((uint32_t)phase, xport_guest_ptr(object + 450, 6), xport_guest_ptr(record + 36, 12));
            w_u32(record + 36, (uint32_t)(((int32_t)(uint32_t)(r_u32(record + 36))) >> 10));
            w_u32(record + 40, (uint32_t)(((int32_t)(uint32_t)(r_u32(record + 40))) >> 10));
            w_u32(record + 44, (uint32_t)(((int32_t)(uint32_t)(r_u32(record + 44))) >> 10));
            break;
        }
        case 5:
            (void)sub_8002A974(object, record, age, player_index);
            break;
        case 6:
            (void)sub_80028C78(object, record + 20, record + 80, player_index);
            (void)sub_80028E74(record);
            break;
        case 7:
            w_u32(record + 36, 0);
            w_u32(record + 44, 0);
            w_u32(record, (r_u32(record) & 0xFFFFC3FFu) | 0x800u);
            break;
        default:
            break;
    }
    result = (r_u32(record) >> 6) & 0xFu;
    if (result != 5)
    {
        if (r_u8(record + 61) != 0)
        {
            uint32_t angle = r_u32(0x800D8074u) & 0xFFFu;
            int32_t amplitude = ((int32_t)(uint32_t)(r_u32(record + 64)));
            int32_t cosine = (int16_t)r_u16(0x8005624Eu + 4 * angle);
            int32_t sine = (int16_t)r_u16(0x8005624Cu + 4 * angle);

            w_u32(record + 36, r_u32(record + 36) + (uint32_t)(((int64_t)(cosine << 4) * amplitude) >> 16));
            w_u32(record + 44, r_u32(record + 44) + (uint32_t)(((int64_t)(sine << 4) * amplitude) >> 16));
        }
        w_u32(record + 20, r_u32(record + 20) + r_u32(record + 36));
        w_u32(record + 24, r_u32(record + 24) + r_u32(record + 40));
        w_u32(record + 28, r_u32(record + 28) + r_u32(record + 44));
        result = r_u32(record + 24);
    }
    if (reset != 0)
    {
        w_u32(record, (r_u32(record) & 0xFFFFC3FFu) | 0x1000u);
        result = r_u32(record);
    }
    return result;
}

uint32_t sub_8002823C(uint32_t object, uint32_t player_index)
{
    int32_t effect_index = r_s8(object + 73);
    uint32_t local_flags = 0x1F800230u;
    uint32_t child_index;

    FUNCTION_MARKER(0x8002823C, "SLUS_010.53");
    w_u32(local_flags, 0);
    if (effect_index != -1)
    {
        uint32_t first = 0x800D39B0u + 112 * (uint32_t)effect_index;
        uint32_t first_type = (r_u32(first) >> 6) & 0xFu;

        if (r_s8(object + 8) <= 0 || first_type >= 5)
        {
            uint32_t counter = r_u32(0x800D8074u) + 1;
            uint32_t entry = first;
            uint32_t previous = 0;

            if (counter >= 4097)
                counter = 0;
            w_u32(0x800D8074u, counter);
            while (entry != 0)
            {
                uint32_t command = r_u32(entry);
                int32_t next = ((int32_t)(uint32_t)(command << 26)) >> 26;
                uint32_t type = (command >> 6) & 0xFu;
                uint32_t age = r_u32(r_u32(0x8005B2F8u) + 16) - r_u32(entry + 48);

                if ((r_u8(object + 9) & 8u) == 0 || type == 5 || type == 6)
                    (void)sub_80029D88(object, entry, player_index, age);
                command = r_u32(entry);
                type = (command >> 6) & 0xFu;
                if (((command >> 10) & 0xFu) == 4)
                {
                    (void)sub_8002A738(object, previous, entry, (uint32_t)next);
                }
                else
                {
                    if (type == 5)
                    {
                        (void)sub_8002AB14(object, entry);
                    }
                    else
                    {
                        (void)sub_80028E8C(entry + 20, entry + 68, player_index);
                        (void)sub_8002A2C8(object, entry, age, player_index, local_flags);
                    }
                    previous = entry;
                }
                entry = next == -1 ? 0 : 0x800D39B0u + 112 * (uint32_t)next;
            }
            xport_update_u8(object + 9, XPORT_MEMORY_UPDATE_OR, 8u);
        }
    }
    for (child_index = 0; child_index != 2; ++child_index)
    {
        uint32_t child = r_u32(object + 56 + 8 * child_index);

        if (child != 0)
            (void)sub_8002823C(child, player_index);
    }
    return 0;
}

uint32_t sub_80067770(uint32_t owner, uint32_t player_index)
{
    uint32_t object = r_u32(0x8005B280u);
    uint32_t previous = 0;
    uint32_t result = 0x80060000u;

    FUNCTION_MARKER(0x80067770, "RASHCDG.BIN");
    while (object != 0)
    {
        result = r_u32(object + 176);
        if (result == owner)
        {
            uint32_t next;

            result = sub_8002823C(object, player_index);
            next = r_u32(object + 168);
            object = next;
            if (previous != 0)
                w_u32(previous + 168, next);
            else
                w_u32(0x8005B280u, next);
        }
        else
        {
            object = r_u32(object + 168);
        }
        previous = object;
    }
    return result;
}

uint32_t sub_800674D4(uint32_t indices, int32_t count, uint32_t player_index)
{
    uint32_t local_indices[12];
    int32_t index;

    FUNCTION_MARKER(0x800674D4, "RASHCDG.BIN");
    for (index = 0; index < count; ++index)
        local_indices[index] = r_u32(indices + 4 * (uint32_t)index);
    return frontier_prepare_object_indices(local_indices, count, player_index);
}

int32_t sub_8003328C(uint32_t owner, int32_t group)
{
    int32_t index = 24 * group;
    int32_t end = index + 24;
    uint32_t entry = 0x800D9268u + 1152 * (uint32_t)group;

    FUNCTION_MARKER(0x8003328C, "SLUS_010.53");
    while (index < end)
    {
        if (r_u32(entry + 4) == owner && r_u32(entry) == 0xFFFFFFFFu)
            return index;
        ++index;
        entry += 48;
    }
    return -1;
}
