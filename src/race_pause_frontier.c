#include "race_pause_frontier.h"
#include "race_leaf_batch_006.h"
#include "fixed_math.h"
#include "flare.h"
#include "gpu.h"
#include "menu_item.h"
#include "packet.h"
#include "psx_gpu.h"
#include "xport.h"
#include <string.h>

static void frontier_copy_words(RRJMemory *m, uint32_t output, uint32_t input, uint32_t bytes);
static uint32_t frontier_asr(uint32_t value, uint32_t shift);

static uint32_t frontier_call(RRJMemory *m, RRJRaceLeafCall call, uint32_t target, uint32_t a0, uint32_t a1, uint32_t a2)
{
    const uint32_t args[8] = {a0, a1, a2, 0, 0, 0, 0, 0};
    return call(m, target, args);
}

static uint32_t frontier_call4(RRJMemory *m, RRJRaceLeafCall call, uint32_t target, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
    const uint32_t args[8] = {a0, a1, a2, a3, 0, 0, 0, 0};
    return call(m, target, args);
}

static uint32_t frontier_call7(RRJMemory *m, RRJRaceLeafCall call, uint32_t target, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6)
{
    const uint32_t args[8] = {a0, a1, a2, a3, a4, a5, a6, 0};
    return call(m, target, args);
}

static uint32_t frontier_prepare_object_indices(RRJMemory *m, const uint32_t *indices, int32_t count, uint32_t player_index, RRJRaceLeafCall call)
{
    int32_t index;

    rrj_put16(rrj_at(m, 0x1F80000Au, 2), 2048);
    rrj_put16(rrj_at(m, 0x1F80000Cu, 2), 4096);
    rrj_put16(rrj_at(m, 0x1F800012u, 2), 512);
    rrj_put16(rrj_at(m, 0x1F800014u, 2), 768);
    *(int8_t *)rrj_at(m, 0x1F800018u, 1) = -3;
    *(int8_t *)rrj_at(m, 0x1F800019u, 1) = -2;
    rrj_put16(rrj_at(m, 0x1F800008u, 2), 0);
    rrj_put16(rrj_at(m, 0x1F800010u, 2), 0);
    *(uint8_t *)rrj_at(m, 0x1F80001Au, 1) = 0;
    rrj_write32(m, 0x1F800004u, rrj_read32(m, 0x8005B4D4u));
    rrj_write32(m, 0x1F800000u, rrj_read32(m, 0x8005B4D8u));
    rrj_write32(m, 0x1F80001Cu, rrj_read32(m, 0x8005ADFCu) - 2);
    rrj_write32(m, 0x1F800208u, rrj_read32(m, 0x8005ACB4u));
    frontier_copy_words(m, 0x1F800020u, 0x800D50A8u, 128);
    for (index = 0; index < count; ++index)
    {
        uint32_t owner = rrj_read32(m, 0x800D87F0u + 112 * indices[index]);

        (void)sub_80067690(m, owner, player_index, call);
    }
    for (index = 0; index < count; ++index)
    {
        uint32_t owner = rrj_read32(m, 0x800D87F0u + 112 * indices[index]);

        (void)sub_80067770(m, owner, player_index, call);
    }
    return 0;
}

static void frontier_configure_model_scratch(RRJMemory *m)
{
    rrj_put16(rrj_at(m, 0x1F80000Au, 2), 2048);
    rrj_put16(rrj_at(m, 0x1F80000Cu, 2), 4096);
    rrj_put16(rrj_at(m, 0x1F800012u, 2), 512);
    rrj_put16(rrj_at(m, 0x1F800014u, 2), 768);
    *(int8_t *)rrj_at(m, 0x1F800018u, 1) = -3;
    *(int8_t *)rrj_at(m, 0x1F800019u, 1) = -2;
    rrj_put16(rrj_at(m, 0x1F800008u, 2), 0);
    rrj_put16(rrj_at(m, 0x1F800010u, 2), 0);
    *(uint8_t *)rrj_at(m, 0x1F80001Au, 1) = 0;
    rrj_write32(m, 0x1F800004u, rrj_read32(m, 0x8005B4D4u));
    rrj_write32(m, 0x1F80001Cu, rrj_read32(m, 0x8005ADFCu) - 2);
    rrj_write32(m, 0x1F800000u, rrj_read32(m, 0x8005B4D8u));
}

static void frontier_set_flare_matrix(RRJMemory *m, int32_t angle, int32_t x_scale, int32_t y_scale)
{
    uint32_t packed = rrj_read32(m, 0x8005624Cu + 4 * ((uint32_t)(angle < 0 ? -angle : angle) & 0xFFFu));
    int32_t sine = (int16_t)packed;
    int32_t cosine = (int16_t)(packed >> 16);
    MATRIX matrix;

    if (angle < 0)
        sine = -sine;
    memset(&matrix, 0, sizeof(matrix));
    matrix.m[0][0] = (int16_t)rrj_s32(frontier_asr((uint32_t)(x_scale * cosine), 12));
    matrix.m[0][1] = (int16_t)rrj_s32(frontier_asr((uint32_t)(-x_scale * sine), 12));
    matrix.m[1][0] = (int16_t)rrj_s32(frontier_asr((uint32_t)(y_scale * sine), 12));
    matrix.m[1][1] = (int16_t)rrj_s32(frontier_asr((uint32_t)(y_scale * cosine), 12));
    SetRotMatrix(&matrix);
}

static uint32_t frontier_abs(int32_t value)
{
    uint32_t sign = (uint32_t)value >> 31;
    return ((uint32_t)value + sign) ^ (0u - sign);
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
    int32_t screen_y = rrj_s32(screen) >> 16;
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

static void frontier_transform_vertex(const MATRIX *matrix, RRJMemory *m, uint32_t source, FrontierVertex *vertex)
{
    int16_t input[3];
    int32_t output[3];
    uint32_t row;

    input[0] = (int16_t)rrj_u16(rrj_at(m, source, 2));
    input[1] = (int16_t)rrj_u16(rrj_at(m, source + 2, 2));
    input[2] = (int16_t)rrj_u16(rrj_at(m, source + 4, 2));
    for (row = 0; row != 3; ++row)
    {
        int64_t value = (int64_t)matrix->m[row][0] * input[0] + (int64_t)matrix->m[row][1] * input[1] + (int64_t)matrix->m[row][2] * input[2];
        output[row] = (int32_t)(value >> 12) + matrix->t[row];
    }
    vertex->x = rrj_s32((uint32_t)output[0] << 8);
    vertex->y = rrj_s32((uint32_t)output[1] << 8);
    vertex->z = rrj_s32((uint32_t)output[2] << 8);
    vertex->screen = frontier_project_translation(vertex->x >> 5, vertex->y >> 5, vertex->z >> 5);
}

static void frontier_project_scratch_quad(RRJMemory *m)
{
    uint32_t index;

    for (index = 0; index != 4; ++index)
    {
        uint32_t record = 0x1F800100u + 32 * index;
        int32_t x = rrj_s32(rrj_read32(m, record));
        int32_t y = rrj_s32(rrj_read32(m, record + 4));
        int32_t z = rrj_s32(rrj_read32(m, record + 8));
        uint32_t screen = frontier_project_translation(x >> 5, y >> 5, z >> 5);

        rrj_write32(m, record + 24, screen);
        *(uint8_t *)rrj_at(m, record + 31, 1) = frontier_classify_projected(z, screen, 102399);
    }
}

static void frontier_render_flat_quad(RRJMemory *m, int32_t depth, uint32_t color)
{
    if (depth < 1600)
    {
        (void)sub_8006A25C(m, 4, 0x01030200u, color);
    }
    else
    {
        uint32_t cursor = rrj_read32(m, 0x1F800020u);
        uint32_t packet;

        if (cursor + 24 >= rrj_read32(m, 0x8005B4D0u))
            cursor = sub_80021C98(m, cursor, 24);
        packet = cursor;
        rrj_write32(m, packet, rrj_read32(m, 0x1F800028u) | 0x05000000u);
        rrj_write32(m, packet + 4, color | 0x28000000u);
        rrj_write32(m, packet + 8, rrj_read32(m, 0x1F800118u));
        rrj_write32(m, packet + 12, rrj_read32(m, 0x1F800138u));
        rrj_write32(m, packet + 16, rrj_read32(m, 0x1F800158u));
        rrj_write32(m, packet + 20, rrj_read32(m, 0x1F800178u));
        rrj_write32(m, 0x1F800028u, packet);
        rrj_write32(m, 0x1F800020u, packet + 24);
    }
}

static void frontier_prepare_source_record(RRJMemory *m, const MATRIX *matrix, uint32_t record_index, uint32_t vertex_index, uint32_t uv)
{
    FrontierVertex vertex;
    uint32_t record = 0x1F800100u + 32 * record_index;
    uint32_t color_table = rrj_read32(m, 0x800CCE34u);
    uint32_t source_table = rrj_read32(m, 0x800CCE28u);
    uint32_t clips = rrj_read32(m, 0x8005ACB8u);
    uint32_t shade;
    uint32_t depth_clip;

    memset(&vertex, 0, sizeof(vertex));
    frontier_transform_vertex(matrix, m, source_table + 8 * vertex_index, &vertex);
    shade = *(uint8_t *)rrj_at(m, color_table + 8 * vertex_index + 6, 1);
    if (vertex.z > 204799)
        depth_clip = 1u << 5;
    else if (vertex.z > 51199)
        depth_clip = 1u << 6;
    else
        depth_clip = 1u << 7;
    rrj_write32(m, record, (uint32_t)vertex.x);
    rrj_write32(m, record + 4, (uint32_t)vertex.y);
    rrj_write32(m, record + 8, (uint32_t)vertex.z);
    rrj_write32(m, record + 16, uv);
    rrj_write32(m, record + 20, shade);
    rrj_write32(m, record + 24, vertex.screen);
    rrj_write32(m, record + 28, (rrj_read32(m, 0x800D4CA8u + 4 * shade) & 0xFFFFFFu) | ((uint32_t)(*(uint8_t *)rrj_at(m, clips + vertex_index, 1) | depth_clip) << 24));
}

static void frontier_render_wrapped_subdivision(RRJMemory *m, uint32_t slot, uint32_t flags, int quad)
{
    uint32_t cursor = rrj_read32(m, 0x1F800020u);
    uint32_t packet = cursor;

    rrj_write32(m, packet, (rrj_read32(m, slot) & 0xFFFFFFu) | 0x02000000u);
    rrj_write32(m, packet + 4, rrj_read32(m, 0x1F8000B4u));
    rrj_write32(m, packet + 8, 0);
    rrj_write32(m, 0x1F800028u, packet);
    if (packet + 12 >= rrj_read32(m, 0x8005B4D0u))
        cursor = sub_80021C98(m, packet, 12);
    cursor += 12;
    rrj_write32(m, 0x1F800020u, cursor);
    if (quad)
        (void)sub_80069784(m, 4, 0x03020100u, 5u - ((flags >> 16) & 0x0Fu));
    else
        (void)sub_8006929C(m, 3, 0x00020100u, 5u - ((flags >> 16) & 0x0Fu));
    packet = rrj_read32(m, 0x1F800020u);
    rrj_write32(m, packet, (rrj_read32(m, 0x1F800028u) & 0xFFFFFFu) | 0x02000000u);
    rrj_write32(m, packet + 4, rrj_read32(m, 0x1F800078u + 4 * ((flags & 0xFFu) >> 4)));
    rrj_write32(m, packet + 8, 0);
    rrj_write32(m, 0x1F800028u, packet);
    if (packet + 12 >= rrj_read32(m, 0x8005B4D0u))
        cursor = sub_80021C98(m, packet, 12);
    else
        cursor = packet;
    cursor += 12;
    rrj_write32(m, 0x1F800020u, cursor);
    rrj_write32(m, slot, rrj_read32(m, 0x1F800028u));
}

static void frontier_build_midpoint(RRJMemory *m, uint32_t output_index, uint32_t first_index, uint32_t second_index, uint32_t rounding_bias, int adjust)
{
    uint32_t first = 0x1F800100u + 32 * first_index;
    uint32_t second = 0x1F800100u + 32 * second_index;
    uint32_t output = 0x1F800100u + 32 * output_index;
    uint32_t first_color = rrj_read32(m, first + 16);
    uint32_t second_color = rrj_read32(m, second + 16);
    int32_t depth = rrj_s32(frontier_asr(rrj_read32(m, first + 20) + rrj_read32(m, second + 20), 1));
    int32_t x = rrj_s32(frontier_asr(rrj_read32(m, first) + rrj_read32(m, second), 1));
    int32_t y = rrj_s32(frontier_asr(rrj_read32(m, first + 4) + rrj_read32(m, second + 4), 1));
    int32_t z = rrj_s32(frontier_asr(rrj_read32(m, first + 8) + rrj_read32(m, second + 8), 1));
    uint32_t screen;
    uint32_t clip;
    int32_t screen_y;

    rrj_write32(m, output + 16, (((first_color & 0xFFFFFEFFu) + (second_color & 0xFFFFFEFFu)) >> 1) + ((first_color & second_color) & 0x100u));
    rrj_write32(m, output + 20, (uint32_t)depth);
    rrj_write32(m, output + 28, (rrj_read32(m, output + 28) & 0xFF000000u) | (rrj_read32(m, 0x800D4CA8u + 4 * ((uint32_t)depth & 0xFFu)) & 0xFFFFFFu));
    if (adjust)
    {
        int32_t scale = rrj_s32(frontier_asr((uint32_t)z + rounding_bias, 8));
        uint32_t first_uv = rrj_read32(m, first + 24);
        uint32_t second_uv = rrj_read32(m, second + 24);
        int32_t delta_x = (int16_t)first_uv - (int16_t)second_uv;
        int32_t delta_y = (first_uv >> 16) - (second_uv >> 16);
        uint32_t abs_x = frontier_abs(delta_x);
        uint32_t abs_y = frontier_abs(delta_y);
        uint32_t sign_x = 0u - ((uint32_t)delta_x >> 31);
        uint32_t sign_y = 0u - ((uint32_t)delta_y >> 31);
        uint32_t y_mask = frontier_asr(abs_y - 2u * abs_x, 31);
        uint32_t x_mask = frontier_asr(abs_x - 2u * abs_y, 31);
        uint32_t y_adjust = (((2u * (uint32_t)scale) & sign_x) - (uint32_t)scale) & y_mask;
        uint32_t x_adjust = ((uint32_t)scale + ((0u - 2u * (uint32_t)scale) & sign_y)) & x_mask;
        x = rrj_s32((uint32_t)x + x_adjust);
        y = rrj_s32((uint32_t)y + y_adjust);
    }
    rrj_write32(m, output, (uint32_t)x);
    rrj_write32(m, output + 4, (uint32_t)y);
    rrj_write32(m, output + 8, (uint32_t)z);
    screen = frontier_project_translation(x >> 5, y >> 5, z >> 5);
    rrj_write32(m, output + 24, screen);
    if (z > 102399)
        clip = 1u << 5;
    else if (z > 51199)
        clip = 1u << 6;
    else
        clip = 1u << 7;
    if (z < 10240)
        clip |= 0x10u;
    screen_y = rrj_s32(screen) >> 16;
    if (screen_y < 0)
        clip |= 8u;
    if (screen_y >= 241)
        clip |= 4u;
    if ((uint16_t)screen < 385u)
        clip |= (screen >> 15) & 1u;
    else
        clip |= ((screen >> 15) & 1u) + 1u;
    *(uint8_t *)rrj_at(m, output + 31, 1) = (uint8_t)clip;
}

static int32_t frontier_mul_shift12(int32_t left, int32_t right)
{
    return (int32_t)frontier_asr((uint32_t)left * (uint32_t)right, 12);
}

static int32_t frontier_depth_bucket(RRJMemory *m, int32_t depth)
{
    int32_t bucket;

    if (rrj_s32(rrj_read32(m, 0x1F800004u)) < 4096)
    {
        int32_t exponent = rrj_s32(rrj_read32(m, 0x1F800000u)) + *(int8_t *)rrj_at(m, (depth >> 12) > 0 ? 0x1F800019u : 0x1F800018u, 1) + ((depth >> 11) > 0);
        int32_t table_index = exponent;
        int32_t base;

        if (table_index < 0)
            table_index = 0;
        if (table_index > 3)
            table_index = 3;
        base = rrj_u16(rrj_at(m, 0x1F800006u + 2 * (uint32_t)table_index, 2)) + rrj_s32(rrj_read32(m, 0x1F800004u));
        bucket = rrj_u16(rrj_at(m, 0x1F80000Eu + 2 * (uint32_t)table_index, 2)) + (rrj_s32((uint32_t)depth - (uint32_t)base) >> (exponent & 31));
    }
    else
    {
        bucket = rrj_s32((uint32_t)depth - rrj_read32(m, 0x1F800004u)) >> (rrj_read32(m, 0x1F800000u) & 31u);
    }
    if (bucket < 0)
        bucket = 0;
    if (bucket > rrj_s32(rrj_read32(m, 0x1F80001Cu)))
        bucket = rrj_s32(rrj_read32(m, 0x1F80001Cu));
    return bucket;
}

static int32_t frontier_matrix_mul32(RRJMemory *m, uint32_t matrix, const int32_t input[3], int32_t output[3])
{
    unsigned row;
    int32_t result = 0;

    for (row = 0; row != 3; ++row)
    {
        uint32_t first = (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, matrix + 6 * row, 2)) * (uint32_t)input[0];
        uint32_t second = (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, matrix + 6 * row + 2, 2)) * (uint32_t)input[1];
        uint32_t third = (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, matrix + 6 * row + 4, 2)) * (uint32_t)input[2];

        result = (int32_t)(frontier_asr(first, 12) + frontier_asr(second, 12) + frontier_asr(third, 12));
        output[row] = result;
    }
    return result;
}

uint32_t sub_800106DC(RRJMemory *m, uint32_t input, uint32_t output, uint32_t matrix)
{
    int32_t source[3];
    int32_t transformed[3];
    unsigned index;

    FUNCTION_MARKER(0x800106DC, "SLUS_010.53");
    for (index = 0; index != 3; ++index)
        source[index] = rrj_s32(rrj_read32(m, input + 4 * index));
    (void)frontier_matrix_mul32(m, matrix, source, transformed);
    for (index = 0; index != 3; ++index)
        rrj_write32(m, output + 4 * index, (uint32_t)transformed[index]);
    return (uint32_t)transformed[2];
}

uint32_t sub_8001064C(RRJMemory *m)
{
    FUNCTION_MARKER(0x8001064C, "SLUS_010.53");
    return 0;
}

uint32_t sub_80023A14(RRJMemory *m, RRJRaceLeafCall call)
{
    uint32_t record = rrj_read32(m, 0x8005AE34);
    uint32_t source = rrj_read32(m, record + 4);
    uint32_t blocked = rrj_u16(rrj_at(m, source + 362, 2));
    int32_t direction;
    int32_t position;

    FUNCTION_MARKER(0x80023A14, "SLUS_010.53");
    if (blocked)
        return blocked;
    position = (int16_t)rrj_u16(rrj_at(m, source + 370, 2));
    rrj_write32(m, record + 68, (uint32_t)position);
    rrj_write32(m, record + 72, rrj_u16(rrj_at(m, source + 360, 2)));
    direction = rrj_s32(rrj_read32(m, source + 364));
    direction = direction > 0 ? 1 : direction < 0 ? -1 : 0;
    rrj_write32(m, record + 64, (uint32_t)direction);
    rrj_write32(m, record + 100, direction <= 0 ? rrj_read32(m, record + 68) - rrj_read32(m, record + 104) : rrj_read32(m, record + 104) - rrj_read32(m, record + 68));
    rrj_write32(m, record + 44, rrj_read32(m, record + 72) != rrj_read32(m, record + 80));
    rrj_write32(m, record + 48, rrj_read32(m, record + 88) != (uint32_t)direction);
    if (rrj_read32(m, record + 44) || rrj_read32(m, record + 48))
    {
        uint32_t route = frontier_call(m, call, 0x800245DC, rrj_read32(m, record + 72), 0, 0);

        rrj_write32(m, record + 92, route);
        rrj_write32(m, record + 96, rrj_read32(m, route + 4) >> 6);
        rrj_write32(m, record + 52, rrj_read32(m, route + (direction <= 0 ? 8 : 12)));
    }
    if (rrj_read32(m, record + 60))
    {
        if (position >= 41 && position < rrj_s32(rrj_read32(m, record + 96)) - 40)
        {
            rrj_write32(m, record + 60, 0);
            rrj_write32(m, record + 36, 1);
        }
    }
    else if ((position < 41 || position >= rrj_s32(rrj_read32(m, record + 96)) - 40) && (rrj_read32(m, record + 44) || rrj_read32(m, record + 48) || (position - rrj_s32(rrj_read32(m, record + 84))) * direction >= 0))
    {
        rrj_write32(m, record + 60, 1);
        rrj_write32(m, record + 40, 1);
    }
    return frontier_call(m, call, 0x80023DB8, record + 80, 0, 0);
}

uint32_t sub_80023DB8(RRJMemory *m, uint32_t output)
{
    uint32_t record = rrj_read32(m, 0x8005AE34);
    uint32_t result;

    FUNCTION_MARKER(0x80023DB8, "SLUS_010.53");
    rrj_write32(m, output + 8, rrj_read32(m, record + 64));
    rrj_write32(m, output, rrj_read32(m, record + 72));
    result = rrj_read32(m, record + 68);
    rrj_write32(m, output + 4, result);
    return result;
}

uint32_t sub_80023FBC(RRJMemory *m, RRJRaceLeafCall call)
{
    uint32_t record = rrj_read32(m, 0x8005AE34);
    uint32_t state;

    FUNCTION_MARKER(0x80023FBC, "SLUS_010.53");
    if (!rrj_read32(m, record + 44) && !rrj_read32(m, record + 48) && !rrj_read32(m, record + 40) && !rrj_read32(m, record + 36))
        return record;
    state = rrj_read32(m, record + 56);
    if (state == 0)
    {
        if (rrj_read32(m, record + 48))
        {
            uint32_t route;

            (void)frontier_call(m, call, 0x80023DE4, 0, 0, 0);
            route = rrj_read32(m, 0x8005B508 + 4 * rrj_read32(m, record));
            route = rrj_read32(m, route + (rrj_s32(rrj_read32(m, record + 64)) <= 0 ? 8 : 4));
            (void)frontier_call(m, call, 0x800243BC, route, 0, 0);
        }
        else if (rrj_read32(m, record + 40))
            rrj_write32(m, record + 56, 1);
        else if (rrj_read32(m, record + 44))
        {
            rrj_write32(m, record + 56, 1);
            rrj_write32(m, record + 60, 1);
        }
    }
    else if (state == 1)
    {
        if (rrj_read32(m, record + 40))
        {
            rrj_write32(m, record + 56, 0);
            return frontier_call(m, call, 0x80023DE4, 0, 0, 0);
        }
        if (rrj_read32(m, record + 36))
        {
            if (!(rrj_read32(m, record + 28) & 2))
            {
                rrj_write32(m, record + 60, state);
                return record;
            }
            rrj_write32(m, record + 56, 0);
            (void)frontier_call(m, call, 0x80023DE4, 0, 0, 0);
        }
    }
    rrj_write32(m, record + 36, 0);
    rrj_write32(m, record + 40, 0);
    rrj_write32(m, record + 48, 0);
    rrj_write32(m, record + 44, 0);
    return record;
}

uint32_t sub_80023CAC(RRJMemory *m, RRJRaceLeafCall call)
{
    uint32_t current = rrj_read32(m, 0x8005B500);
    uint32_t state = 0x80053478 + (current << 7);
    uint32_t raw;
    uint32_t delta;
    uint32_t restored;
    uint32_t count;
    uint32_t result;

    FUNCTION_MARKER(0x80023CAC, "SLUS_010.53");
    if (rrj_read32(m, state + 108) == 0 && rrj_read32(m, 0x80053478 + ((current ^ 1) << 7) + 108) != 0)
    {
        current ^= 1;
        rrj_write32(m, 0x8005B500, current);
    }
    raw = rrj_read32(m, rrj_read32(m, 0x8005ACBC) + 24);
    delta = 5 - raw;
    restored = 5 - delta;
    count = (delta & (0u - ((delta >> 31) ^ 1u))) + (restored & (0u - (restored >> 31)));
    (void)sub_80023148(m, 0x80053480 + (current << 7), count, 0, current << 16, call);
    current = rrj_read32(m, 0x8005B500);
    result = rrj_read32(m, 0x80053480 + (current << 7) + 4);
    if (result)
        rrj_write32(m, 0x8005B514, result);
    return result;
}

uint32_t sub_80030E58(RRJMemory *m, uint32_t player_index, RRJRaceLeafCall call)
{
    uint32_t base = rrj_read32(m, 0x8005ACBC);
    uint32_t first = rrj_read32(m, base + 0xA40 + 8 * player_index);
    uint32_t last = rrj_read32(m, base + 0xA44 + 8 * player_index);
    uint32_t index;

    FUNCTION_MARKER(0x80030E58, "SLUS_010.53");
    rrj_write32(m, base + 32, 0);
    rrj_write32(m, base + 36, 0);
    rrj_write32(m, base + 40, 0);
    for (index = first; index <= last; ++index)
    {
        uint32_t record = base + 44 + 36 * index;

        if ((rrj_read32(m, record) & 0x91) == 0x11)
        {
            uint32_t payload = rrj_read32(m, record + 20);
            int32_t result = (int32_t)frontier_call(m, call, 0x80023900, rrj_read32(m, payload) >> 28, payload + 4, player_index);

            if (result < 0)
                (void)frontier_call(m, call, 0x800318E8, record, player_index, 0);
            if (result < 0)
                rrj_write32(m, base + 32, rrj_read32(m, base + 32) + 1);
            else if (result > 0)
                rrj_write32(m, base + 40, rrj_read32(m, base + 40) + 1);
            else
                rrj_write32(m, base + 36, rrj_read32(m, base + 36) + 1);
        }
    }
    return frontier_call(m, call, 0x80033198, player_index, 0, 0);
}

uint32_t sub_80033198(RRJMemory *m, uint32_t player_index, RRJRaceLeafCall call)
{
    uint32_t record = 0x800D9268 + 1152 * player_index;
    uint32_t index;

    FUNCTION_MARKER(0x80033198, "SLUS_010.53");
    for (index = 0; index != 24; ++index, record += 48)
    {
        uint32_t value = rrj_read32(m, record + 8);

        if (rrj_s32(value) >= 0 && rrj_s32(frontier_call(m, call, 0x80023900, value, record + 24, player_index)) < 0)
        {
            uint32_t id = ((value & 0x8000) << 16) | ((value & 0x7C00) << 13) | (value & 0x3FF);

            if (rrj_read32(m, record + 4) == 0)
                (void)frontier_call(m, call, 0x8003297C, id, player_index, 0);
            else
                (void)frontier_call(m, call, 0x80032938, id, player_index, 0);
        }
    }
    return 0;
}

uint32_t sub_80023900(RRJMemory *m, uint32_t type, uint32_t payload, uint32_t player_index, RRJRaceLeafCall call)
{
    uint32_t previous = rrj_read32(m, rrj_read32(m, 0x8005AE34));
    uint32_t result;

    FUNCTION_MARKER(0x80023900, "SLUS_010.53");
    (void)sub_8002379C(m, player_index);
    result = frontier_call(m, call, 0x800243EC, type, payload, 0);
    (void)sub_8002379C(m, previous);
    return result;
}

uint32_t sub_800243EC(RRJMemory *m, uint32_t type, uint32_t ranges)
{
    uint32_t record = rrj_read32(m, 0x8005AE34);
    uint32_t index;
    int32_t route = rrj_s32(rrj_read32(m, record + 72));
    int32_t position = rrj_s32(rrj_read32(m, record + 68));

    FUNCTION_MARKER(0x800243EC, "SLUS_010.53");
    for (index = 0; index != 4; ++index, ranges += 6)
    {
        int32_t candidate = (int16_t)rrj_u16(rrj_at(m, ranges, 2));

        if (candidate == -1)
            break;
        if (candidate == route)
        {
            int32_t first = (int16_t)rrj_u16(rrj_at(m, ranges + 2, 2));
            int32_t last = (int16_t)rrj_u16(rrj_at(m, ranges + 4, 2));

            if (position >= first && position <= last)
                return 0;
            if (rrj_s32(rrj_read32(m, record + 64)) > 0)
                return position < first ? 1 : UINT32_MAX;
            return last < position ? 1 : UINT32_MAX;
        }
    }
    return UINT32_MAX;
}

uint32_t sub_800318E8(RRJMemory *m, uint32_t record, uint32_t player_index, RRJRaceLeafCall call)
{
    uint32_t payload = rrj_read32(m, record + 20);
    uint32_t word = rrj_read32(m, payload);
    uint32_t type = word >> 28;
    uint32_t id = word & 0x0FFFFFFF;
    uint32_t extra = rrj_read32(m, record + 16);
    uint32_t handled = 1;

    FUNCTION_MARKER(0x800318E8, "SLUS_010.53");
    if (rrj_read32(m, record) & 2)
    {
        if (type == 0 || type == 8 || type == 9)
            (void)frontier_call(m, call, 0x80032810, id, type, player_index);
        else if (type == 1)
            (void)frontier_call(m, call, 0x8003297C, id, player_index, extra);
        else if (type == 2)
            (void)frontier_call(m, call, 0x80032938, id, player_index, extra);
        else if (type == 3)
            handled = frontier_call(m, call, 0x8003D844, id & 0xFFFF, player_index, extra);
        else if (type == 4)
            handled = frontier_call(m, call, 0x80013828, extra, 0, 0);
    }
    if (handled == 1)
    {
        if (rrj_read32(m, record) & 0x10)
            (void)frontier_call(m, call, 0x80023868, rrj_read32(m, payload + 28) >> 1, 0, 0);
        return frontier_call(m, call, 0x800313EC, record, player_index, 0);
    }
    return 1;
}

uint32_t sub_800312D0(RRJMemory *m, uint32_t player_index)
{
    uint32_t base = rrj_read32(m, 0x8005ACBC);
    uint32_t first = rrj_read32(m, base + 0xA40 + 8 * player_index);
    uint32_t last = rrj_read32(m, base + 0xA44 + 8 * player_index);
    uint32_t record = base + 44 + 36 * first;
    uint32_t skipped = 0;
    uint32_t count = 0;

    FUNCTION_MARKER(0x800312D0, "SLUS_010.53");
    while (first <= last)
    {
        if (rrj_read32(m, record) == 0)
        {
            if (rrj_read32(m, rrj_read32(m, 0x8005B2F8) + 48) == 1 && skipped < 2)
                ++skipped;
            else
                ++count;
        }
        ++first;
        record += 36;
    }
    return count;
}

uint32_t sub_80018E54(RRJMemory *m, uint32_t group, RRJRaceLeafCall call)
{
    uint32_t enabled = rrj_read32(m, group + 8);
    uint32_t count;
    uint32_t index;

    FUNCTION_MARKER(0x80018E54, "SLUS_010.53");
    if (!enabled)
        return enabled;
    count = rrj_read32(m, group + 12);
    if (rrj_s32(count) <= 0)
        return count;
    for (index = 0; rrj_s32(index) < rrj_s32(rrj_read32(m, group + 12)); ++index)
    {
        uint32_t entry = rrj_read32(m, group) + 2108 * index;
        uint32_t cursor;

        if (!(rrj_read32(m, entry + 36) & 2) || !(cursor = rrj_read32(m, entry + 1756)))
            continue;
        while (rrj_read32(m, entry + 16) >= rrj_read32(m, cursor))
        {
            uint32_t object = rrj_read32(m, entry);
            uint32_t source = object;
            uint32_t flags = *(uint8_t *)rrj_at(m, entry + 36, 1) & 0x80;

            if ((rrj_u16(rrj_at(m, object + 172, 2)) >> 5) == 1 && rrj_read32(m, object + 604) < 3)
            {
                uint32_t descriptor;

                source = rrj_read32(m, object + 596);
                descriptor = rrj_read32(m, source + 1084);
                flags |= (*(uint8_t *)rrj_at(m, descriptor + 46, 1) & 15) | ((*(uint8_t *)rrj_at(m, descriptor + 1, 1) & 3) << 4);
                if (rrj_u16(rrj_at(m, source + 172, 2)) < rrj_read32(m, rrj_read32(m, 0x8005B2F8) + 48))
                    flags |= 0x40;
            }
            cursor = frontier_call4(m, call, 0x80017DA0, rrj_read32(m, source + 184), rrj_read32(m, source + 192), cursor, flags);
            rrj_write32(m, entry + 1756, cursor);
            if (!cursor)
                break;
        }
    }
    return 0;
}

uint32_t sub_80043E24(RRJMemory *m)
{
    return 0x801FFF98;
}

uint32_t sub_80043DC4(RRJMemory *m, uint32_t stack)
{
    /* Native execution uses the host stack */
    return 0;
}

uint32_t sub_800C89A0(RRJMemory *m)
{
    uint32_t state = rrj_read32(m, 0x8005B2F8);
    uint32_t current = *(uint8_t *)rrj_at(m, 0x8005B58C, 1) ^ 1;
    uint32_t index;
    uint32_t table;

    FUNCTION_MARKER(0x800C89B8, "RASHCDG.BIN");
    rrj_write32(m, 0x8005B588, 0);
    *(uint8_t *)rrj_at(m, 0x8005B58C, 1) = (uint8_t)current;
    for (index = 0; index < rrj_read32(m, state + 48); ++index)
    {
        rrj_gpu_clear_ot(m, 0x800D9BE0 + 40 * current + 20 * index, 5);
        rrj_gpu_clear_ot(m, 0x800D9C30 + 8 * current + 4 * index, 1);
    }
    rrj_write32(m, 0x8005B588, 0);
    rrj_write32(m, 0x8005B59C, 0x800D9BE0 + 40 * current);
    rrj_write32(m, 0x8005B5A8, 0x800D9C30 + 8 * current);
    table = 0x800D9C50 + 48 * current;
    rrj_gpu_clear_ot(m, table, 12);
    rrj_write32(m, 0x8005B5AC, table);
    rrj_write32(m, 0x8005B5A0, table + 40);
    rrj_write32(m, 0x8005B5A4, table + 36);
    rrj_write32(m, 0x8005B590, table + 32);
    rrj_write32(m, 0x8005B594, table + 28);
    rrj_write32(m, 0x8005B598, table);
    return table;
}

uint32_t sub_8002F2E8(RRJMemory *m, uint32_t argument, RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x8002F2E8, "SLUS_010.53");
    return frontier_call(m, call, 0x8002F17C, argument, 0, 0);
}

uint32_t sub_8002F17C(RRJMemory *m, uint32_t player_index, RRJRaceLeafCall call)
{
    uint32_t player = 0x800CD898 + 1132 * player_index;
    uint32_t record = rrj_read32(m, 0x8005AEC0 + 4 * player_index);
    uint32_t index;
    uint32_t result;

    FUNCTION_MARKER(0x8002F17C, "SLUS_010.53");
    (void)frontier_call(m, call, 0x80070F9C, player + 432, record + 60, 0);
    (void)frontier_call(m, call, 0x8004D264, record + 60, record + 92, 0);
    for (index = 0; index != 3; ++index)
    {
        int32_t value = (int16_t)rrj_u16(rrj_at(m, record + 98 + 2 * index, 2));
        uint32_t scaled = frontier_asr((uint32_t)value * 3413u, 12);

        rrj_put16(rrj_at(m, record + 98 + 2 * index, 2), (uint16_t)scaled);
    }
    rrj_write32(m, record + 16, rrj_read32(m, player + 184));
    rrj_write32(m, record + 20, rrj_read32(m, player + 188));
    rrj_write32(m, record + 24, rrj_read32(m, player + 192));
    rrj_write32(m, record + 28, frontier_asr(rrj_read32(m, record + 16), 10));
    rrj_write32(m, record + 32, frontier_asr(rrj_read32(m, record + 20), 10));
    rrj_write32(m, record + 36, frontier_asr(rrj_read32(m, record + 24), 10));
    result = rrj_u16(rrj_at(m, player + 744, 2));
    rrj_put16(rrj_at(m, record + 124, 2), (uint16_t)result);
    return result;
}

uint32_t sub_80070F9C(RRJMemory *m, uint32_t source, uint32_t output)
{
    uint32_t value;

    FUNCTION_MARKER(0x80070F9C, "RASHCDG.BIN");
    value = rrj_u16(rrj_at(m, source, 2));
    rrj_put16(rrj_at(m, output, 2), (uint16_t)value);
    value = rrj_u16(rrj_at(m, source + 2, 2));
    rrj_put16(rrj_at(m, output + 6, 2), (uint16_t)value);
    value = rrj_u16(rrj_at(m, source + 4, 2));
    rrj_put16(rrj_at(m, output + 12, 2), (uint16_t)value);
    value = rrj_u16(rrj_at(m, source + 6, 2));
    rrj_put16(rrj_at(m, output + 2, 2), (uint16_t)value);
    value = rrj_u16(rrj_at(m, source + 8, 2));
    rrj_put16(rrj_at(m, output + 8, 2), (uint16_t)value);
    value = rrj_u16(rrj_at(m, source + 10, 2));
    rrj_put16(rrj_at(m, output + 14, 2), (uint16_t)value);
    value = rrj_u16(rrj_at(m, source + 12, 2));
    rrj_put16(rrj_at(m, output + 4, 2), (uint16_t)value);
    value = rrj_u16(rrj_at(m, source + 14, 2));
    rrj_put16(rrj_at(m, output + 10, 2), (uint16_t)value);
    value = rrj_u16(rrj_at(m, source + 16, 2));
    rrj_put16(rrj_at(m, output + 16, 2), (uint16_t)value);
    return (uint32_t)(int32_t)(int16_t)value;
}

uint32_t sub_800674C8(RRJMemory *m)
{
    FUNCTION_MARKER(0x800674C8, "RASHCDG.BIN");
    rrj_write32(m, 0x8005B280, 0);
    return 0x80060000;
}

static void frontier_update_linked_actor(RRJMemory *m, uint32_t root, uint32_t player_index, RRJRaceLeafCall call)
{
    uint32_t child;
    uint32_t actor;

    if (!root)
        return;
    child = rrj_read32(m, root + 852);
    if (!(*(uint8_t *)rrj_at(m, child + 572, 1) & 0x10))
        return;
    child = rrj_read32(m, root + 856);
    actor = rrj_read32(m, child + 852);
    if (rrj_read32(m, actor + 604) < 3)
        return;
    (void)frontier_call(m, call, 0x8008B99C, actor, 0, 0);
    (void)frontier_call(m, call, 0x80085224, actor, player_index, 0);
}

uint32_t sub_8008D56C(RRJMemory *m, uint32_t player_index, RRJRaceLeafCall call)
{
    uint32_t table = 0x800CE4D0;
    uint32_t actor;
    int32_t remaining;
    uint32_t group;

    FUNCTION_MARKER(0x8008D56C, "RASHCDG.BIN");
    (void)frontier_call(m, call, 0x8008B99C, 0x800CD898 + 1132 * player_index, 0, 0);
    actor = rrj_read32(m, table);
    remaining = rrj_s32(rrj_read32(m, rrj_read32(m, table + 12)));
    while (remaining >= 0)
    {
        uint32_t linked;

        if ((int16_t)rrj_u16(rrj_at(m, actor + 320, 2)) != 0 && rrj_s32(frontier_call(m, call, 0x8008B99C, actor, 0, 0)) > 0)
            (void)frontier_call(m, call, 0x80084E10, actor, player_index, 0);
        linked = rrj_read32(m, actor + 852);
        if (rrj_read32(m, linked + 604) < 3)
            rrj_write32(m, linked + 176, rrj_read32(m, actor + 176));
        else if ((int16_t)rrj_u16(rrj_at(m, linked + 320, 2)) != 0 && rrj_s32(frontier_call(m, call, 0x8008B99C, linked, 0, 0)) > 0)
            (void)frontier_call(m, call, 0x80085224, linked, player_index, 0);
        actor += rrj_read32(m, table + 4);
        --remaining;
    }
    frontier_update_linked_actor(m, rrj_read32(m, 0x8005B38C), player_index, call);
    frontier_update_linked_actor(m, rrj_read32(m, 0x8005B21C), player_index, call);
    for (group = 2; group != 6; ++group)
    {
        uint32_t entry = table + 16 * group;

        actor = rrj_read32(m, entry);
        remaining = rrj_s32(rrj_read32(m, rrj_read32(m, entry + 12)));
        while (remaining >= 0)
        {
            if (rrj_u16(rrj_at(m, actor + 172, 2)) != 0 && rrj_s32(frontier_call(m, call, 0x8008B99C, actor, 0, 0)) > 0)
            {
                uint32_t output = actor + 104;

                rrj_write32(m, actor + 12, frontier_asr(rrj_read32(m, actor + 184), 10));
                rrj_write32(m, actor + 16, frontier_asr(rrj_read32(m, actor + 188), 10));
                rrj_write32(m, actor + 20, frontier_asr(rrj_read32(m, actor + 192), 10));
                if (*(int8_t *)rrj_at(m, actor + 72, 1) != 3)
                    output = rrj_read32(m, actor + 4) + 4;
                (void)sub_80070F9C(m, actor + 432, output);
                (void)frontier_call(m, call, 0x80067AC4, actor, player_index, 0);
            }
            actor += rrj_read32(m, entry + 4);
            --remaining;
        }
    }
    return 0;
}

static int32_t frontier_fixed_mul(int32_t left, int32_t right)
{
    return (int32_t)sub_8001FC90(left, right);
}

static int32_t frontier_clamp(int32_t value, int32_t minimum, int32_t maximum)
{
    if (value < minimum)
        return minimum;
    if (value > maximum)
        return maximum;
    return value;
}

uint32_t sub_80084E10(RRJMemory *m, uint32_t actor, uint32_t player_index, RRJRaceLeafCall call)
{
    uint32_t specification = rrj_read32(m, actor + 556);
    uint32_t output = actor + 104;

    FUNCTION_MARKER(0x80084E10, "RASHCDG.BIN");
    rrj_write32(m, actor + 12, frontier_asr(rrj_read32(m, actor + 184), 10));
    rrj_write32(m, actor + 16, frontier_asr(rrj_read32(m, actor + 188), 10));
    rrj_write32(m, actor + 20, frontier_asr(rrj_read32(m, actor + 192), 10));
    if (*(int8_t *)rrj_at(m, actor + 72, 1) != 3)
        output = rrj_read32(m, actor + 4) + 4;
    (void)sub_80070F9C(m, actor + 432, output);
    if (*(int8_t *)rrj_at(m, rrj_read32(m, 0x8005B2F8), 1) != 3 && *(int8_t *)rrj_at(m, actor + 8, 1) < 2)
    {
        int32_t target = 0;
        int32_t steering;
        int32_t roll = 0;
        uint32_t linked;

        (void)frontier_call4(m, call, 0x8001FD24, 0, (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, actor + 826, 2)), 0, rrj_read32(m, actor + 4) + 28);
        if (!(rrj_read32(m, actor + 568) & 0x600))
        {
            if (rrj_read32(m, actor + 612))
            {
                int32_t state = rrj_s32(rrj_read32(m, actor + 616));
                int32_t angle = (int16_t)rrj_u16(rrj_at(m, actor + 846, 2));

                if (state > 0 || angle > 0)
                    target = rrj_u16(rrj_at(m, specification + 438, 2));
                else if (state < 0 || angle < 0)
                    target = rrj_u16(rrj_at(m, specification + 436, 2));
            }
            else
            {
                int32_t lower = rrj_u16(rrj_at(m, specification + 436, 2));
                int32_t upper = rrj_u16(rrj_at(m, specification + 438, 2));
                int32_t phase = (int32_t)sub_8001FF3C(m, (uint32_t)((int32_t)(int16_t)rrj_u16(rrj_at(m, actor + 530, 2)) * 16));
                int32_t curve = 2 * (phase - (int16_t)rrj_u16(rrj_at(m, actor + 842, 2)));
                int32_t desired;
                int32_t velocity = rrj_s32(rrj_read32(m, actor + 608));

                curve = frontier_clamp(curve, -lower, upper);
                if (rrj_read32(m, actor + 564) & 0x20)
                    desired = upper;
                else if (rrj_read32(m, actor + 564) & 0x40)
                    desired = -lower;
                else if (velocity > 0)
                    desired = frontier_fixed_mul(velocity, upper << 16) >> 16;
                else if (velocity < 0)
                    desired = frontier_fixed_mul(velocity, lower << 16) >> 16;
                else
                    desired = 0;
                target = desired;
                if ((curve ^ desired) >= 0)
                    target = desired <= 0 ? (curve < desired ? curve : desired) : (desired >= curve ? desired : curve);
            }
        }
        steering = rrj_u16(rrj_at(m, actor + 844, 2));
        steering += (frontier_fixed_mul(245760, (target - (int16_t)rrj_u16(rrj_at(m, actor + 846, 2))) << 16) - frontier_fixed_mul(22937, (int16_t)rrj_u16(rrj_at(m, actor + 844, 2)) << 16)) >> 16;
        rrj_put16(rrj_at(m, actor + 844, 2), (uint16_t)steering);
        rrj_put16(rrj_at(m, actor + 846, 2), (uint16_t)((int16_t)rrj_u16(rrj_at(m, actor + 846, 2)) + (int16_t)steering / 20));
        linked = rrj_read32(m, actor + 856);
        if (linked)
        {
            int32_t target_roll = 0;
            int32_t velocity;
            int32_t old_velocity;

            if (!(rrj_read32(m, actor + 568) & 0x700))
            {
                velocity = rrj_s32(rrj_read32(m, actor + 636));
                if (velocity <= 0)
                    target_roll = -velocity / 3;
                else
                    target_roll = rrj_s32(rrj_read32(m, actor + 488)) / 4;
            }
            old_velocity = rrj_s32(rrj_read32(m, linked + 660));
            velocity = old_velocity + frontier_fixed_mul(245760, target_roll - rrj_s32(rrj_read32(m, linked + 656))) - frontier_fixed_mul(0x4000, old_velocity);
            rrj_write32(m, linked + 660, (uint32_t)velocity);
            roll = frontier_clamp(rrj_s32(rrj_read32(m, linked + 656)) + velocity / 20, -13725, 13725);
            rrj_write32(m, linked + 656, (uint32_t)roll);
            roll = -(int32_t)frontier_asr((uint32_t)(652 * roll), 16);
        }
        (void)frontier_call4(m, call, 0x8001FD24, (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, actor + 846, 2)), 0, (uint32_t)roll, rrj_read32(m, actor + 4) + 52);
    }
    return frontier_call(m, call, 0x80067AC4, actor, player_index, 0);
}

static void frontier_copy_words(RRJMemory *m, uint32_t output, uint32_t input, uint32_t bytes)
{
    uint32_t offset;

    for (offset = 0; offset != bytes; offset += 4)
        rrj_write32(m, output + offset, rrj_read32(m, input + offset));
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

static void frontier_rotate_vector(RRJMemory *m, uint32_t matrix_address, const int16_t input[3], int32_t output[3])
{
    int16_t matrix[3][3];
    uint32_t row;
    uint32_t column;

    for (row = 0; row != 3; ++row)
        for (column = 0; column != 3; ++column)
            matrix[row][column] = (int16_t)rrj_u16(rrj_at(m, matrix_address + 2 * (3 * row + column), 2));
    frontier_rotate_values(matrix, input, output);
}

static uint32_t frontier_clear_player_visibility(RRJMemory *m, uint32_t actor, uint32_t player_index)
{
    uint8_t bit = (uint8_t)(1u << player_index);
    uint8_t flags = *(uint8_t *)rrj_at(m, actor + 9, 1);

    flags &= (uint8_t)~bit;
    *(uint8_t *)rrj_at(m, actor + 9, 1) = flags;
    return flags & bit;
}

uint32_t sub_80067AC4(RRJMemory *m, uint32_t actor, uint32_t player_index, RRJRaceLeafCall call)
{
    uint32_t player = rrj_read32(m, 0x8005AEC0 + 4 * player_index);
    uint32_t camera = player + 92;
    uint32_t specification = rrj_read32(m, actor);
    uint32_t type = (rrj_u16(rrj_at(m, specification + 14, 2)) & 0x78) >> 3;
    uint32_t shift;
    uint8_t bit = (uint8_t)(1u << player_index);
    uint8_t flags;
    int16_t input[3];
    int32_t transformed[3];
    uint32_t index;

    FUNCTION_MARKER(0x80067AC4, "RASHCDG.BIN");
    if (!rrj_read32(m, actor + 52) && rrj_read32(m, actor + 176) == UINT32_MAX)
        return 0;
    if (*(int8_t *)rrj_at(m, actor + 72, 1) == 1)
    {
        flags = *(uint8_t *)rrj_at(m, actor + 9, 1) | bit;
        *(uint8_t *)rrj_at(m, actor + 9, 1) = flags;
        shift = rrj_u16(rrj_at(m, specification + 14, 2)) >> 12;
        frontier_copy_words(m, actor + 136, actor + 104, 32);
        if (type == 5)
        {
            for (index = 0; index != 3; ++index)
            {
                rrj_write32(m, actor + 80 + 4 * index, rrj_read32(m, actor + 156 + 4 * index));
                rrj_write32(m, actor + 156 + 4 * index, 0);
            }
        }
        else
        {
            for (index = 0; index != 3; ++index)
            {
                int32_t position = rrj_s32(rrj_read32(m, player + 16 + 4 * index));
                input[index] = (int16_t)(rrj_u16(rrj_at(m, actor + 12 + 4 * index, 2)) - frontier_asr((uint32_t)position, 10 - shift));
            }
            frontier_rotate_vector(m, camera, input, transformed);
            for (index = 0; index != 3; ++index)
                rrj_write32(m, actor + 80 + 4 * index, (uint32_t)transformed[index]);
        }
        goto visible;
    }
    {
        int32_t threshold = rrj_s32(rrj_read32(m, 0x800CC6A4 + 4 * type));
        int32_t distance = rrj_s32(rrj_read32(m, actor + 44 + 4 * player_index));
        int outside = threshold < distance;

        if (type == 6 && *(int8_t *)rrj_at(m, actor + 8, 1) == 0)
            outside = threshold + 32000 < distance;
        if (outside || distance < 0)
            return frontier_clear_player_visibility(m, actor, player_index);
    }
    if (rrj_read32(m, actor + 100))
    {
        (void)frontier_call(m, call, 0x8001298C, actor, (uint32_t)(int32_t)*(int8_t *)rrj_at(m, actor + 10 + player_index, 1), 0);
        rrj_write32(m, actor + 36, rrj_read32(m, actor + 36) & ~0x80u);
    }
    for (index = 0; index != 2; ++index)
    {
        uint32_t child = rrj_read32(m, actor + 56 + 8 * index);

        if (child && rrj_read32(m, child + 100))
        {
            (void)frontier_call(m, call, 0x8001298C, child, (uint32_t)(int32_t)*(int8_t *)rrj_at(m, child + 10 + player_index, 1), 0);
            rrj_write32(m, child + 36, rrj_read32(m, child + 36) & ~0x80u);
        }
    }
    {
        uint32_t actor_flags = rrj_read32(m, actor + 36);
        int32_t distance = rrj_s32(rrj_read32(m, actor + 44 + 4 * player_index));

        if (((actor_flags >> 5) & 3) != 1 && (uint32_t)(distance - 129) < 0x27f)
        {
            if (rrj_read32(m, rrj_read32(m, 0x8005B2F8) + 48) != 1 || type == 3)
                rrj_write32(m, actor + 36, (actor_flags & ~0x80u) | ((type - 1 < 2) << 7));
            else
            {
                uint32_t active = 0;

                if (rrj_read32(m, specification + 44))
                {
                    actor_flags |= 0x80;
                    rrj_write32(m, actor + 36, actor_flags);
                    active = 1;
                }
                for (index = 0; index != 2; ++index)
                {
                    uint32_t child = rrj_read32(m, actor + 56 + 8 * index);

                    if (child && rrj_read32(m, child + 100) && rrj_read32(m, rrj_read32(m, child) + 44))
                    {
                        ++active;
                        rrj_write32(m, child + 36, rrj_read32(m, child + 36) | 0x80);
                    }
                }
                flags = *(uint8_t *)rrj_at(m, actor + 9, 1);
                *(uint8_t *)rrj_at(m, actor + 9, 1) = (uint8_t)((16 * active) | (flags & 0xcf));
            }
        }
    }
    shift = rrj_u16(rrj_at(m, specification + 14, 2)) >> 12;
    if (*(int8_t *)rrj_at(m, actor + 72, 1) == 3)
    {
        int32_t world[3];

        for (index = 0; index != 3; ++index)
            world[index] = rrj_s32(rrj_read32(m, actor + 12 + 4 * index) << shift);
        if (*(uint8_t *)rrj_at(m, actor + 9, 1) & 4)
        {
            uint32_t correction_shift = 4 - shift;

            for (index = 0; index != 3; ++index)
            {
                int32_t value = (int16_t)rrj_u16(rrj_at(m, actor + 28 + 2 * index, 2));
                input[index] = (int16_t)(shift == 4 ? value : rrj_s32(frontier_asr((uint32_t)value, correction_shift)));
            }
            frontier_rotate_vector(m, actor + 104, input, transformed);
            for (index = 0; index != 3; ++index)
            {
                world[index] += transformed[index];
                rrj_write32(m, actor + 12 + 4 * index, rrj_read32(m, actor + 12 + 4 * index) + frontier_asr((uint32_t)transformed[index], shift));
            }
        }
        for (index = 0; index != 3; ++index)
        {
            int32_t position = rrj_s32(rrj_read32(m, player + 16 + 4 * index));
            input[index] = (int16_t)(world[index] - rrj_s32(frontier_asr((uint32_t)position, 10 - shift)));
        }
        frontier_rotate_vector(m, camera, input, transformed);
    }
    else
    {
        for (index = 0; index != 3; ++index)
            input[index] = (int16_t)frontier_asr(rrj_read32(m, actor + 184 + 4 * index) - rrj_read32(m, player + 16 + 4 * index), 10);
        frontier_rotate_vector(m, camera, input, transformed);
        for (index = 0; index != 3; ++index)
            transformed[index] = (int32_t)((uint32_t)transformed[index] << shift);
    }
    for (index = 0; index != 3; ++index)
        rrj_write32(m, actor + 80 + 4 * index, (uint32_t)transformed[index]);
    {
        int32_t x = transformed[0];
        int32_t y = transformed[1];
        int32_t z = transformed[2];
        int32_t radius = rrj_s32(rrj_read32(m, actor + 40));

        if (type == 3)
        {
            x = rrj_s32(frontier_asr((uint32_t)x, shift));
            y = rrj_s32(frontier_asr((uint32_t)y, shift));
        }
        else
        {
            x = rrj_s32(frontier_asr((uint32_t)(x < 0 ? x + radius : x - radius), shift));
            y = rrj_s32(frontier_asr((uint32_t)(y < 0 ? y + radius : y - radius), shift));
            z += radius;
        }
        z = rrj_s32(frontier_asr((uint32_t)z, shift));
        if (z < x || z < -x || z < y || z < -y || z < 40)
            return frontier_clear_player_visibility(m, actor, player_index);
    }
    flags = *(uint8_t *)rrj_at(m, actor + 9, 1) | bit;
    *(uint8_t *)rrj_at(m, actor + 9, 1) = flags;
    frontier_copy_words(m, actor + 136, camera, 32);

visible:
    if (!rrj_read32(m, actor + 52))
    {
        uint32_t head = rrj_read32(m, 0x8005B280);

        rrj_write32(m, actor + 168, head);
        rrj_write32(m, 0x8005B280, actor);
    }
    return *(uint8_t *)rrj_at(m, actor + 9, 1) & bit;
}

uint32_t sub_800358C0(RRJMemory *m, uint32_t player_index, RRJRaceLeafCall call)
{
    uint32_t count_address = 0x8005B568 + 4 * player_index;
    int32_t count;
    uint32_t records;

    FUNCTION_MARKER(0x800358C0, "SLUS_010.53");
    (void)frontier_call(m, call, 0x80035F48, 0, 0, 0);
    count = rrj_s32(rrj_read32(m, count_address));
    if (count <= 0)
        return count_address;
    records = 0x800D9B80 + 48 * player_index;
    (void)frontier_call4(m, call, 0x800353C4, 0x8005B578 + 4 * player_index, (uint32_t)count, records, player_index);
    (void)frontier_call(m, call, 0x80036438, (uint32_t)count, records, player_index);
    return frontier_call(m, call, 0x80035680, (uint32_t)count, records, player_index);
}

static int frontier_render_range_matches(RRJMemory *m, uint32_t record, uint32_t id, int32_t value)
{
    return (int16_t)rrj_u16(rrj_at(m, record + 12, 2)) == (int32_t)id && value >= (int16_t)rrj_u16(rrj_at(m, record + 14, 2)) && value <= (int16_t)rrj_u16(rrj_at(m, record + 16, 2));
}

uint32_t sub_80035F48(RRJMemory *m, uint32_t player_index, RRJRaceLeafCall call)
{
    uint32_t selected_address = 0x8005B508 + 4 * player_index;
    uint32_t output = 0x800D9B80 + 48 * player_index;
    uint32_t table = rrj_read32(m, 0x8005B518);
    uint32_t actor;
    uint32_t selected;
    uint32_t packed;
    uint32_t id;
    int32_t value;
    uint32_t index;

    FUNCTION_MARKER(0x80035F48, "SLUS_010.53");
    if (!table)
    {
        uint32_t count = frontier_call(m, call, 0x80035E60, output, player_index, 0);

        rrj_write32(m, 0x8005B568 + 4 * player_index, count);
        rrj_write32(m, 0x8005B578 + 4 * player_index, count);
        table = rrj_read32(m, 0x8005B518);
    }
    actor = rrj_read32(m, 0x8005B268 + 4 * player_index);
    selected = rrj_read32(m, actor + 852);
    if ((uint32_t)(rrj_read32(m, selected + 604) - 3) >= 2)
        selected = actor;
    packed = rrj_read32(m, selected + 360);
    if (!(packed >> 16))
    {
        uint32_t current = rrj_read32(m, selected_address);

        id = packed & 0xffff;
        value = (int16_t)rrj_u16(rrj_at(m, selected + 370, 2));
        if (!current || !frontier_render_range_matches(m, current, id, value))
        {
            if (current && (int16_t)rrj_u16(rrj_at(m, current + 12, 2)) == (int32_t)id)
                current += 32 * rrj_read32(m, selected + 364);
            if (!current || !frontier_render_range_matches(m, current, id, value))
            {
                uint32_t record = rrj_read32(m, table + 4 * id);

                rrj_write32(m, selected_address, 0);
                for (index = 0; index != 140; ++index, record += 32)
                {
                    if ((int16_t)rrj_u16(rrj_at(m, record + 12, 2)) != (int32_t)id)
                        break;
                    if (frontier_render_range_matches(m, record, id, value))
                    {
                        rrj_write32(m, selected_address, record);
                        break;
                    }
                }
            }
        }
    }
    selected = rrj_read32(m, selected_address);
    if (!selected)
        return 0;
    for (index = 0; index != 12; ++index)
        rrj_write32(m, output + 4 * index, UINT32_MAX);
    for (index = 0; index != 6; ++index)
        rrj_write32(m, 0x800D9B68 + 4 * index, UINT32_MAX);
    rrj_write32(m, 0x8005B568 + 4 * player_index, 0);
    rrj_write32(m, 0x8005B570, 0);
    rrj_write32(m, 0x8005B574, 0);
    for (index = 0; index != 6; ++index)
    {
        uint32_t encoded = rrj_u16(rrj_at(m, selected + 18 + 2 * index, 2));
        uint32_t decoded;
        uint32_t actor_index;
        uint32_t record;

        if (encoded == 0xffff)
            break;
        decoded = ((encoded & 0x8000) << 16) | ((encoded & 0x7c00) << 13) | (encoded & 0x3ff);
        if (decoded == UINT32_MAX)
            continue;
        actor_index = sub_800329BC(m, decoded & 0x0fffffff, player_index);
        if (actor_index != UINT32_MAX)
        {
            record = 0x800D87E8 + 112 * actor_index;
            if (rrj_read32(m, record + 8) != UINT32_MAX && rrj_read32(m, record + 4) && sub_800363F0(m, record))
            {
                uint32_t count = rrj_read32(m, 0x8005B568 + 4 * player_index);

                rrj_write32(m, output + 4 * count, actor_index);
                rrj_write32(m, 0x8005B568 + 4 * player_index, count + 1);
                continue;
            }
        }
        {
            uint32_t deferred = rrj_read32(m, 0x8005B574);

            rrj_write32(m, 0x800D9B68 + 4 * deferred, decoded);
            rrj_write32(m, 0x8005B574, deferred + 1);
        }
    }
    {
        uint32_t count = rrj_read32(m, 0x8005B568 + 4 * player_index);

        rrj_write32(m, 0x8005B578 + 4 * player_index, count);
        rrj_write32(m, 0x8005B570, count + rrj_read32(m, 0x8005B574));
        return count;
    }
}

uint32_t sub_800363F0(RRJMemory *m, uint32_t record)
{
    uint32_t index;

    FUNCTION_MARKER(0x800363F0, "SLUS_010.53");
    for (index = 0; index != 2; ++index)
        if (rrj_read32(m, record + 88 + 4 * index) != UINT32_MAX && !rrj_read32(m, record + 96 + 4 * index))
            return 0;
    return 1;
}

uint32_t sub_80035040(RRJMemory *m, uint32_t record, uint32_t vertices, int32_t count)
{
    uint32_t common_flags = 0xff;
    uint32_t combined_flags = 0;
    int32_t index;

    FUNCTION_MARKER(0x80035040, "SLUS_010.53");
    rrj_write32(m, record + 44, (uint32_t)-641);
    rrj_write32(m, record + 48, 0xffff);
    for (index = 0; index < count; ++index)
    {
        int16_t vertex_index = (int16_t)rrj_u16(rrj_at(m, vertices + 2 * (uint32_t)index, 2));
        uint32_t model_vertices = rrj_read32(m, rrj_read32(m, record) + 52);
        int16_t input[3];
        int32_t transformed[3];
        uint32_t far_flags;
        uint32_t near_flags;
        uint32_t scaled_z;
        uint32_t absolute_z;
        int32_t negative_absolute_z;
        int32_t wide_x;
        int32_t narrow_x;
        unsigned row;

        for (row = 0; row != 3; ++row)
            input[row] = (int16_t)rrj_u16(rrj_at(m, model_vertices + 8 * (uint32_t)(int32_t)vertex_index + 4 + 2 * row, 2));
        for (row = 0; row != 3; ++row)
        {
            int64_t value = (int64_t)rrj_s32(rrj_read32(m, record + 32 + 4 * row)) << 12;
            unsigned column;

            for (column = 0; column != 3; ++column)
                value += (int16_t)rrj_u16(rrj_at(m, record + 12 + 2 * (3 * row + column), 2)) * input[column];
            transformed[row] = (int32_t)(value >> 12);
        }
        if (transformed[2] < rrj_s32(rrj_read32(m, record + 48)))
            rrj_write32(m, record + 48, (uint32_t)transformed[2]);
        if (transformed[2] > rrj_s32(rrj_read32(m, record + 44)))
            rrj_write32(m, record + 44, (uint32_t)transformed[2]);
        scaled_z = (uint32_t)transformed[2] << 4;
        absolute_z = frontier_abs((int32_t)scaled_z);
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

static uint32_t frontier_classify_segment(RRJMemory *m, uint32_t record, uint32_t vertices, int32_t count, int32_t *average, int32_t *minimum, int32_t *maximum)
{
    uint32_t common_flags = 7;
    uint32_t combined_flags = 0;
    uint32_t scratch = rrj_read32(m, 0x8005ACB0);
    uint32_t sum = 0;
    int32_t index;

    *minimum = 0xffffff;
    *maximum = -641;
    for (index = 0; index < count; ++index)
    {
        int16_t vertex_index = (int16_t)rrj_u16(rrj_at(m, vertices + 2 * (uint32_t)index, 2));
        uint32_t model_vertices = rrj_read32(m, rrj_read32(m, record) + 52);
        int16_t input[3];
        int32_t transformed[3];
        uint32_t far_flags;
        uint32_t near_flags;
        uint32_t scaled_z;
        uint32_t absolute_z;
        int32_t negative_absolute_z;
        int32_t wide_x;
        int32_t narrow_x;
        unsigned row;

        for (row = 0; row != 3; ++row)
            input[row] = (int16_t)rrj_u16(rrj_at(m, model_vertices + 8 * (uint32_t)(int32_t)vertex_index + 4 + 2 * row, 2));
        for (row = 0; row != 3; ++row)
        {
            int64_t value = (int64_t)rrj_s32(rrj_read32(m, record + 32 + 4 * row)) << 12;
            unsigned column;

            for (column = 0; column != 3; ++column)
                value += (int16_t)rrj_u16(rrj_at(m, record + 12 + 2 * (3 * row + column), 2)) * input[column];
            transformed[row] = (int32_t)(value >> 12);
            rrj_write32(m, scratch + 16 * (uint32_t)index + 4 * row, (uint32_t)transformed[row]);
        }
        if (transformed[2] < *minimum)
            *minimum = transformed[2];
        if (transformed[2] > *maximum)
            *maximum = transformed[2];
        if (index < 4)
            sum += (uint32_t)transformed[2];
        scaled_z = (uint32_t)transformed[2] << 4;
        absolute_z = frontier_abs((int32_t)scaled_z);
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

uint32_t sub_800351EC(RRJMemory *m, uint32_t record, uint32_t vertices, int32_t count, uint32_t average_output, uint32_t minimum_output, uint32_t maximum_output)
{
    int32_t average;
    int32_t minimum;
    int32_t maximum;
    uint32_t result;

    FUNCTION_MARKER(0x800351EC, "SLUS_010.53");
    result = frontier_classify_segment(m, record, vertices, count, &average, &minimum, &maximum);
    rrj_write32(m, average_output, (uint32_t)average);
    rrj_write32(m, minimum_output, (uint32_t)minimum);
    rrj_write32(m, maximum_output, (uint32_t)maximum);
    return result;
}

uint32_t sub_800353C4(RRJMemory *m, uint32_t count_pointer, int32_t count, uint32_t indices, uint32_t player_index, RRJRaceLeafCall call)
{
    uint32_t player = rrj_read32(m, 0x8005AEC0 + 4 * player_index);
    uint32_t special_actor = rrj_read32(m, 0x800CD898 + 1132 * player_index + 176);
    int32_t special_slot = -1;
    int32_t slot;

    FUNCTION_MARKER(0x800353C4, "SLUS_010.53");
    rrj_write32(m, 0x8005AEE0, UINT32_MAX);
    for (slot = 0; slot < count; ++slot)
    {
        uint32_t slot_address = indices + 4 * (uint32_t)slot;
        uint32_t actor_index = rrj_read32(m, slot_address);
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
            rrj_write32(m, record + 12 + 4 * index, rrj_read32(m, player + 92 + 4 * index));
        model = rrj_read32(m, record);
        for (index = 0; index != 3; ++index)
            delta[index] = rrj_s32(rrj_read32(m, model + 8 + 4 * index) - rrj_read32(m, player + 28 + 4 * index));
        (void)frontier_matrix_mul32(m, record + 12, delta, transformed);
        for (index = 0; index != 3; ++index)
            rrj_write32(m, record + 32 + 4 * index, (uint32_t)transformed[index]);
        table = rrj_read32(m, model + 44);
        {
            int32_t first = (int16_t)rrj_u16(rrj_at(m, table, 2));
            int32_t last = (int16_t)rrj_u16(rrj_at(m, table + 2, 2));

            visible = frontier_call4(m, call, 0x80035040, record, table + 2 * (uint32_t)first, (uint32_t)(last - first), player_index) & 0xff;
        }
        if (rrj_read32(m, 0x800D87E8 + 112 * actor_index + 8) == special_actor)
        {
            if (rrj_s32(rrj_read32(m, record + 44)) < 0)
                rrj_write32(m, record + 44, 1280);
            rrj_write32(m, 0x8005AEE0, actor_index);
            special_slot = slot;
        }
        else if (!visible)
        {
            rrj_write32(m, slot_address, UINT32_MAX);
            rrj_write32(m, count_pointer, rrj_read32(m, count_pointer) - 1);
        }
    }
    if (rrj_read32(m, 0x8005AEE0) != UINT32_MAX && special_slot != -1)
    {
        uint32_t special_address = indices + 4 * (uint32_t)special_slot;

        rrj_write32(m, special_address, rrj_read32(m, indices));
        rrj_write32(m, indices, rrj_read32(m, 0x8005AEE0));
        return special_address;
    }
    return UINT32_MAX;
}

uint32_t sub_80036438(RRJMemory *m, int32_t count, uint32_t indices, uint32_t player_index)
{
    uint32_t player = 0x800CD898 + 1132 * player_index;
    uint32_t special = rrj_read32(m, 0x8005AEE0);
    int32_t last_index;
    uint32_t swaps;
    uint32_t result = 0x800E0000;

    FUNCTION_MARKER(0x80036438, "SLUS_010.53");
    (void)sub_8002E698(m, rrj_read32(m, player + 340) + 14, player + 444);
    if (special != UINT32_MAX)
    {
        rrj_write32(m, indices, special);
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
            uint32_t current_index = rrj_read32(m, current_address);
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
            while (next_index < last_index && rrj_read32(m, indices + 4 * (uint32_t)next_index) == UINT32_MAX)
                ++next_index;
            next_address = indices + 4 * (uint32_t)next_index;
            next = rrj_read32(m, next_address);
            if (next == UINT32_MAX)
                continue;
            current_record = 0x800D87E8 + 112 * current_index;
            next_record = 0x800D87E8 + 112 * next;
            current_depth = rrj_s32(rrj_read32(m, current_record + 48));
            next_depth = rrj_s32(rrj_read32(m, next_record + 48));
            should_swap = next_depth < current_depth;
            if (frontier_abs(rrj_s32((uint32_t)current_depth - (uint32_t)next_depth)) < 640)
                should_swap = rrj_s32(rrj_read32(m, next_record + 64)) < rrj_s32(rrj_read32(m, current_record + 64));
            result = should_swap;
            if (should_swap)
            {
                rrj_write32(m, current_address, next);
                rrj_write32(m, next_address, current_index);
                ++swaps;
            }
        }
    } while (swaps);
    return result;
}

uint32_t sub_80036614(RRJMemory *m, uint32_t record, uint32_t slot, uint32_t flags)
{
    uint32_t result;

    FUNCTION_MARKER(0x80036614, "SLUS_010.53");
    result = rrj_read32(m, record + 52) | (flags << ((slot << 2) & 31));
    rrj_write32(m, record + 52, result);
    return result;
}

uint32_t sub_80035680(RRJMemory *m, int32_t count, uint32_t indices, uint32_t player_index)
{
    int32_t threshold = rrj_read32(m, rrj_read32(m, 0x8005B2F8) + 48) == 2 ? 4800 : 9600;
    uint32_t priority_count = 0;
    int32_t actor_slot;

    FUNCTION_MARKER(0x80035680, "SLUS_010.53");
    for (actor_slot = 0; actor_slot < count; ++actor_slot)
    {
        uint32_t actor_index = rrj_read32(m, indices + 4 * (uint32_t)actor_slot);
        uint32_t record;
        uint32_t model;
        uint32_t table;
        uint32_t segment_count;
        uint32_t priority = 0;
        uint32_t segment;

        if (actor_index == UINT32_MAX)
            continue;
        record = 0x800D87EC + 112 * actor_index;
        model = rrj_read32(m, record);
        table = rrj_read32(m, model + 44);
        segment_count = rrj_u16(rrj_at(m, model + 6, 2));
        rrj_write32(m, record + 52, 0);
        for (segment = 0; segment < segment_count; ++segment)
        {
            int32_t first = (int16_t)rrj_u16(rrj_at(m, table + 2 * segment + 2, 2));
            int32_t last = (int16_t)rrj_u16(rrj_at(m, table + 2 * segment + 4, 2));
            int32_t average;
            int32_t minimum;
            int32_t maximum;
            uint32_t classification;
            uint32_t flags;

            classification = frontier_classify_segment(m, record, table + 2 * (uint32_t)first, last - first, &average, &minimum, &maximum);
            if (segment == 1)
                rrj_write32(m, record + 60, (uint32_t)average);
            if (!classification)
            {
                flags = 0;
            }
            else if (rrj_read32(m, model + 60) && minimum < threshold && priority_count < 4)
            {
                priority = 1;
                flags = classification == 1 || average < 2048 ? 7 : 3;
            }
            else
            {
                flags = classification == 1 || average < 2048 ? 5 : 1;
            }
            (void)sub_80036614(m, record, segment, flags);
            if (minimum < threshold)
                (void)sub_80036614(m, record, segment, 12);
        }
        if (priority)
            ++priority_count;
    }
    return count > 0 ? 0 : 2;
}

uint32_t sub_80013828(RRJMemory *m, uint32_t entry)
{
    uint32_t owner;
    uint32_t slot;

    FUNCTION_MARKER(0x80013828, "SLUS_010.53");
    if (rrj_read32(m, 0x8005B2D0) || rrj_read32(m, entry + 12) != 0x50414E4F)
        return 2;
    owner = rrj_read32(m, 0x8005B278);
    for (slot = 0; slot != 20; ++slot)
    {
        uint32_t record = owner + 740 + 12 * slot;

        if (rrj_read32(m, record) == entry)
        {
            if (slot == (uint32_t)(int16_t)rrj_u16(rrj_at(m, owner + 16, 2)) || slot == (uint32_t)(int16_t)rrj_u16(rrj_at(m, owner + 18, 2)))
                return 2;
            rrj_write32(m, record, 0);
            rrj_write32(m, record + 4, 0);
            rrj_write32(m, record + 8, 0);
            rrj_put16(rrj_at(m, owner + 34, 2), rrj_u16(rrj_at(m, owner + 34, 2)) - 1);
            return 1;
        }
    }
    return 2;
}

uint32_t sub_80023868(RRJMemory *m, uint32_t unused)
{
    FUNCTION_MARKER(0x80023868, "SLUS_010.53");
    return 0;
}

uint32_t sub_800CB02C(RRJMemory *m, uint32_t actor)
{
    uint32_t reference = rrj_read32(m, 0x8005B38C);
    uint32_t body = rrj_read32(m, reference + 852);
    uint32_t type = *(uint8_t *)rrj_at(m, actor + 553, 1);
    uint32_t mismatch;
    uint32_t speed;
    uint32_t absolute_projection;
    uint32_t projection = 0;
    int32_t direction_dot;
    unsigned i;

    FUNCTION_MARKER(0x800CB02C, "RASHCDG.BIN");
    if (rrj_read32(m, body + 604) >= 3)
        reference = body;
    speed = frontier_abs(rrj_s32(rrj_read32(m, actor + 480) - rrj_read32(m, reference + 480)));
    mismatch = ((rrj_read32(m, actor + 364) >> 31) != (uint32_t)(rrj_s32(rrj_read32(m, actor + 344)) < rrj_s32(rrj_read32(m, reference + 344))));
    for (i = 0; i < 3; ++i)
    {
        int32_t delta = rrj_s32(rrj_read32(m, actor + 184 + 4 * i) - rrj_read32(m, reference + 184 + 4 * i));
        int32_t axis = (int32_t)*(int16_t *)rrj_at(m, actor + 444 + 2 * i, 2) << 4;
        projection += (uint32_t)sub_8001FC90(delta, axis);
    }
    absolute_projection = frontier_abs(rrj_s32(projection));
    direction_dot = rrj_s32(sub_8002E698(m, reference + 444, actor + 444));
    if (rrj_s32(projection) >= 0 || absolute_projection >= (uint32_t)sub_8001FC90((int32_t)speed, 457800) || direction_dot >= -58982)
    {
        uint32_t selected = type - 1u < 2u;
        if (selected)
        {
            uint32_t flags = rrj_read32(m, actor + 552);
            flags = (mismatch << 16) | (flags & 0xFF0000FF) | 1;
            rrj_write32(m, actor + 552, flags);
            return flags;
        }
        return selected;
    }
    if (type - 1u >= 2u)
    {
        uint32_t clock = rrj_read32(m, rrj_read32(m, 0x8005B2F8) + 16);
        if (rrj_read32(m, 0x800CCC1C) < clock - rrj_read32(m, actor + 548))
        {
            uint32_t flags = (mismatch << 16) | (rrj_read32(m, actor + 552) & 0xFF00FFFF);
            uint32_t scaled = (uint32_t)sub_8001FC90((int32_t)speed, 261600);
            flags = (flags & 0xFFFF00FE) | (scaled >= absolute_projection ? 0x201u : 0x101u);
            rrj_write32(m, actor + 552, flags);
            rrj_write32(m, actor + 548, clock);
            flags = sub_8001FC58(m) % 900u + 300u;
            rrj_write32(m, 0x800CCC1C, flags);
            return flags;
        }
    }
    return 0x80060000;
}

uint32_t sub_800CAAF0(RRJMemory *m, uint32_t actor, RRJRaceLeafCall call)
{
    uint32_t flags = rrj_read32(m, actor + 552);
    uint32_t type = *(uint8_t *)rrj_at(m, actor + 553, 1);
    uint32_t transition = (flags >> 29) & 1;
    uint32_t complete;

    FUNCTION_MARKER(0x800CAAF0, "RASHCDG.BIN");
    if (flags & 1)
        return sub_800CAB5C(m, actor, call);
    if (!transition)
        complete = frontier_call(m, call, 0x8005BE58, rrj_read32(m, actor + 540), 0, 0);
    else if (flags & 0x40000000)
        complete = 0;
    else
        complete = frontier_call(m, call, 0x8008FD5C, actor, 0, 0);
    if (complete)
        return frontier_call(m, call, 0x800CAEF8, actor, complete, 0);
    if (!transition && type - 4u >= 2u)
        (void)sub_800CB02C(m, actor);
    flags = rrj_read32(m, actor + 552) & 1;
    if (flags)
        return sub_800CAAF0(m, actor, call);
    return flags;
}

uint32_t sub_800CB4F8(RRJMemory *m, int32_t delta, RRJRaceLeafCall call)
{
    uint32_t count_pointer = rrj_read32(m, 0x800CE4FC);
    int32_t remaining = rrj_s32(rrj_read32(m, count_pointer));
    uint32_t actor = rrj_read32(m, 0x800CE4F0);
    uint32_t stride = rrj_read32(m, 0x800CE4F4);

    FUNCTION_MARKER(0x800CB4F8, "RASHCDG.BIN");
    while (remaining >= 0)
    {
        if (*(uint16_t *)rrj_at(m, actor + 172, 2))
        {
            if (*(uint16_t *)rrj_at(m, actor + 320, 2))
            {
                uint32_t flags;
                uint8_t state;
                if ((*(uint8_t *)rrj_at(m, actor + 9, 1) & 3) || *(uint8_t *)rrj_at(m, actor + 553, 1))
                {
                    state = *(uint8_t *)rrj_at(m, actor + 565, 1);
                    if (state & 8)
                    {
                        (void)frontier_call(m, call, 0x8005BE44, rrj_read32(m, actor + 540), 0, 0);
                        *(uint8_t *)rrj_at(m, actor + 565, 1) = state & 0xF7;
                    }
                    (void)frontier_call(m, call, 0x800CAAF0, actor, 0, 0);
                }
                else if (!(*(uint8_t *)rrj_at(m, actor + 565, 1) & 8))
                {
                    (void)frontier_call(m, call, 0x8005BE20, rrj_read32(m, actor + 540), 0, 0);
                    *(uint8_t *)rrj_at(m, actor + 565, 1) |= 8;
                }
                flags = rrj_read32(m, actor + 568);
                if (flags & 2)
                {
                    (void)sub_80037450(m, actor);
                    (void)sub_8009246C(m, actor);
                    rrj_write32(m, actor + 568, (flags | 1) & ~2u);
                }
                flags = rrj_read32(m, actor + 552);
                if (flags & 0x20000000)
                {
                    if (flags & 0x40000000)
                    {
                        int32_t scale = (int32_t)sub_8001FC90(rrj_s32(rrj_read32(m, 0x800D3964)), delta);
                        int32_t value = (int32_t)sub_8001FC90(scale, rrj_s32(rrj_read32(m, actor + 456)));
                        rrj_write32(m, actor + 456, rrj_read32(m, actor + 456) - (uint32_t)value);
                        value = (int32_t)sub_8001FC90(scale, rrj_s32(rrj_read32(m, actor + 460))) - (int32_t)sub_8001FC90(643207, delta);
                        rrj_write32(m, actor + 460, rrj_read32(m, actor + 460) - (uint32_t)value);
                        value = (int32_t)sub_8001FC90(scale, rrj_s32(rrj_read32(m, actor + 464)));
                        rrj_write32(m, actor + 464, rrj_read32(m, actor + 464) - (uint32_t)value);
                        value = rrj_s32(sub_8002E548(m, actor + 456));
                        rrj_write32(m, actor + 480, (uint32_t)value);
                        if (value)
                        {
                            uint32_t magnitude = value < 0 ? (uint32_t)(0u - (uint32_t)value) : (uint32_t)value;
                            uint32_t denominator = (magnitude >> 1) + ((magnitude - 2) >> 31);
                            int32_t reciprocal = (int32_t)(0x80000000u / denominator);
                            (void)sub_8002EED8(m, value < 0 ? (uint32_t)-reciprocal : (uint32_t)reciprocal, actor + 456, actor + 450);
                        }
                        rrj_write32(m, actor + 552, flags & 0x7FFFFFFF);
                    }
                    else if (!(flags & 0x04000000))
                    {
                        (void)frontier_call(m, call, 0x8008F754, actor, (uint32_t)delta, actor + 488);
                    }
                }
            }
            else
            {
                (void)sub_8008C000(m, actor + 172, 2);
            }
        }
        actor += stride;
        --remaining;
    }
    return stride;
}

uint32_t sub_800C8B24(RRJMemory *m, uint32_t player_index, RRJRaceLeafCall call)
{
    int32_t view = *(int8_t *)rrj_at(m, 0x8005B58C, 1);
    uint32_t output = 0x800D9BE0 + 40 * (uint32_t)view + 20 * player_index;
    uint32_t players = rrj_read32(m, 0x8005B470);
    uint32_t selected;

    FUNCTION_MARKER(0x800C8B24, "RASHCDG.BIN");
    rrj_write32(m, 0x8005B588, 0);
    rrj_write32(m, 0x8005B59C, output);
    if (player_index || *(uint8_t *)rrj_at(m, players + 4, 1))
        selected = *(uint8_t *)rrj_at(m, players + 5, 1);
    else
        selected = *(uint8_t *)rrj_at(m, players + 6, 1);
    selected = players + 112 * selected + 16;
    rrj_write32(m, 0x800CD660, selected);
    if (rrj_read32(m, rrj_read32(m, 0x8005B2F8) + 48) >= 2)
    {
        uint32_t offsets = rrj_read32(m, 0x8005B474) + 8 * player_index;

        (void)frontier_call7(m, call, 0x8001C304, (uint32_t)((int16_t)rrj_u16(rrj_at(m, selected, 2)) + (int16_t)rrj_u16(rrj_at(m, offsets, 2))), (uint32_t)((int16_t)rrj_u16(rrj_at(m, selected + 2, 2)) + (int16_t)rrj_u16(rrj_at(m, offsets + 2, 2))), (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, offsets + 4, 2)), (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, offsets + 6, 2)), 0, 0, output + 16);
    }
    (void)frontier_call(m, call, 0x80064B9C, player_index, 0, 0);
    (void)frontier_call(m, call, 0x8002C4F8, 0x8005234C, output + 8, player_index);
    if (*(uint8_t *)rrj_at(m, players + 4, 1))
    {
        rrj_write32(m, 0x8005B588, 2);
        return frontier_call(m, call, 0x80048DB4, output + 16, 0, 0);
    }
    rrj_write32(m, 0x8005B588, 1);
    return 1;
}

uint32_t sub_800C8CD4(RRJMemory *m, uint32_t player_index, RRJRaceLeafCall call)
{
    int32_t view = *(int8_t *)rrj_at(m, 0x8005B58Cu, 1);
    uint32_t ordering_slot = 0x800D9C30u + 8 * (uint32_t)view + 4 * player_index;

    FUNCTION_MARKER(0x800C8CD4, "RASHCDG.BIN");
    rrj_write32(m, 0x8005B5A8u, ordering_slot);
    if (*(uint8_t *)rrj_at(m, 0x800D8060u + player_index, 1))
        (void)sub_8002BE14(m, 0x800D7FE8u + 4 * player_index, ordering_slot, player_index);
    return frontier_call(m, call, 0x80048DB4u, ordering_slot, 0, 0);
}

uint32_t sub_8004D184(RRJMemory *m, int32_t x, int32_t y)
{
    FUNCTION_MARKER(0x8004D184, "SLUS_010.53");
    SetGeomOffset(x, y);
    return 0;
}

uint32_t sub_8001E084(RRJMemory *m)
{
    FUNCTION_MARKER(0x8001E084, "SLUS_010.53");
    return 0;
}

static uint32_t frontier_draw_area_word(RRJMemory *m, int32_t x, int32_t y, uint32_t command)
{
    int32_t width = (int16_t)rrj_u16(rrj_at(m, 0x80055F70u, 2));
    int32_t height = (int16_t)rrj_u16(rrj_at(m, 0x80055F72u, 2));

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

uint32_t sub_8001C304(RRJMemory *m, int32_t x, int32_t y, int32_t width, int32_t height, uint32_t ordering_slot)
{
    uint32_t context = rrj_read32(m, 0x8005B470u);
    uint32_t packet = rrj_read32(m, context + 268);
    uint32_t ordering_word;
    int32_t right;
    int32_t bottom;

    FUNCTION_MARKER(0x8001C304, "SLUS_010.53");
    if (packet + 12 >= rrj_read32(m, 0x8005B4D0u))
        packet = sub_80021C98(m, packet, 12);
    context = rrj_read32(m, 0x8005B470u);
    rrj_write32(m, context + 268, packet + 12);
    right = (int16_t)((uint16_t)x + (uint16_t)width - 1);
    bottom = (int16_t)((uint16_t)y + (uint16_t)height - 1);
    *(uint8_t *)rrj_at(m, packet + 3, 1) = 2;
    rrj_write32(m, packet + 4, frontier_draw_area_word(m, (int16_t)x, (int16_t)y, 0xE3000000u));
    rrj_write32(m, packet + 8, frontier_draw_area_word(m, right, bottom, 0xE4000000u));
    ordering_word = rrj_read32(m, ordering_slot);
    rrj_write32(m, packet, (rrj_read32(m, packet) & 0xFF000000u) | (ordering_word & 0x00FFFFFFu));
    ordering_word = (ordering_word & 0xFF000000u) | (packet & 0x00FFFFFFu);
    rrj_write32(m, ordering_slot, ordering_word);
    return ordering_word;
}

uint32_t sub_8005FA68(RRJMemory *m, uint32_t ordering_slot, uint32_t packets, uint32_t next_index, uint32_t packet_index)
{
    uint32_t packet = packets + 36 * packet_index;
    int32_t type = *(int8_t *)rrj_at(m, packet + 32, 1);
    uint32_t result = 36 * next_index;

    FUNCTION_MARKER(0x8005FA68, "RASHCDG.BIN");
    rrj_write32(m, packet, (rrj_read32(m, 0x800CC68Cu + 4 * (uint32_t)type) << 24) | rrj_read32(m, ordering_slot));
    rrj_write32(m, ordering_slot, (packets + result) & 0x00FFFFFFu);
    return result;
}

uint32_t sub_800C5618(RRJMemory *m, uint32_t player_index)
{
    int32_t x = (int16_t)rrj_u16(rrj_at(m, 0x80055F7Cu, 2));
    int32_t y = (int16_t)rrj_u16(rrj_at(m, 0x80055F7Eu, 2));
    uint32_t ordering_slot = rrj_read32(m, 0x8005B590u + 4 * player_index);

    FUNCTION_MARKER(0x800C5618, "RASHCDG.BIN");
    return sub_8001C304(m, x, y, 384, 240, ordering_slot);
}

uint32_t sub_8005FAC4(RRJMemory *m, uint32_t packet_base, uint32_t state, uint32_t player_index)
{
    uint32_t game_state = rrj_read32(m, 0x8005B2F8u);
    uint32_t selected = *(uint8_t *)rrj_at(m, game_state + 6, 1);
    uint32_t player = rrj_read32(m, 0x8005B3A0u) + 1096 * selected;
    uint32_t descriptor = rrj_read32(m, player + 1084);

    FUNCTION_MARKER(0x8005FAC4, "RASHCDG.BIN");
    sub_8002CD78(m, rrj_read32(m, 0x8005AD2Cu), *(uint8_t *)rrj_at(m, descriptor + 38, 1), (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, state + 24, 2)), (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, state + 26, 2)), rrj_read32(m, 0x8005B590u + 4 * player_index), 0x00808080u);
    return 0;
}

uint32_t sub_80095410(RRJMemory *m, uint32_t actor)
{
    uint32_t record = rrj_read32(m, actor + 428);
    uint32_t route = rrj_read32(m, actor + 360);

    FUNCTION_MARKER(0x80095410, "RASHCDG.BIN");
    if (record && !((rrj_u16(rrj_at(m, record + 118, 2)) >> (rrj_u16(rrj_at(m, actor + 172, 2)) & 31)) & 1u))
        return 1;
    if ((route >> 16) == 1)
        return sub_8003F408(m, record, route & 0xFFFFu) == 0;
    return sub_8003B4B0(m, record, route & 0xFFFFu) == 0;
}

uint32_t sub_8008B84C(RRJMemory *m, uint32_t actor)
{
    int32_t count = rrj_s32(rrj_read32(m, 0x8005B1F8u));
    uint32_t game_state = rrj_read32(m, 0x8005B2F8u);
    uint32_t candidate = rrj_read32(m, 0x8005B3A0u);
    uint32_t actor_id = rrj_u16(rrj_at(m, actor + 172, 2));
    uint32_t route_limited = sub_80095410(m, actor);
    uint32_t best_distance = 0x7FFF0000u;
    int32_t best_index = -1;
    int32_t index;

    FUNCTION_MARKER(0x8008B84C, "RASHCDG.BIN");
    for (index = 0; index < count; ++index, candidate += 1096)
    {
        uint32_t candidate_id = rrj_u16(rrj_at(m, candidate + 172, 2));
        uint32_t descriptor = rrj_read32(m, candidate + 1084);

        if (candidate_id != actor_id && (candidate_id < rrj_read32(m, game_state + 48) || (*(uint8_t *)rrj_at(m, descriptor + 1, 1) & 0xFu) != 2 || (*(uint8_t *)rrj_at(m, candidate + 928, 1) & 0x10u)) && (!route_limited || rrj_read32(m, candidate + 360) == rrj_read32(m, actor + 360)))
        {
            uint32_t delta = (rrj_read32(m, candidate + 324) - rrj_read32(m, actor + 324)) << 4;
            uint32_t distance = rrj_s32(delta) < 0 ? 0u - delta : delta;

            if (distance < best_distance)
            {
                best_distance = distance;
                best_index = index;
            }
        }
    }
    return best_index < 0 ? 0 : rrj_read32(m, 0x8005B3A0u) + 1096 * (uint32_t)best_index;
}

uint32_t sub_8005F9B0(RRJMemory *m, uint32_t packets, int32_t delta, int32_t first, int32_t last)
{
    uint32_t record = packets + 36 * (uint32_t)first + 8;
    uint32_t result = last < first;

    FUNCTION_MARKER(0x8005F9B0, "RASHCDG.BIN");
    while (first <= last)
    {
        int32_t mode = *(int8_t *)rrj_at(m, record + 24, 1);
        uint16_t value = (uint16_t)(rrj_u16(rrj_at(m, record + 18, 2)) + delta);

        rrj_put16(rrj_at(m, record + 18, 2), value);
        if (!mode)
        {
            uint32_t source = rrj_read32(m, record + 12);
            uint32_t packed = *(uint8_t *)rrj_at(m, source + 2, 1) | ((uint32_t)*(uint8_t *)rrj_at(m, source + 3, 1) << 16);

            rrj_write32(m, record, rrj_read32(m, record + 16));
            rrj_write32(m, record + 8, packed);
            rrj_write32(m, record + 20, packed);
        }
        if (*(int8_t *)rrj_at(m, record + 24, 1) == 1)
            rrj_write32(m, record, rrj_u16(rrj_at(m, record + 16, 2)) | ((uint32_t)rrj_u16(rrj_at(m, record + 18, 2)) << 16));
        ++first;
        result = last < first;
        record += 36;
    }
    return result;
}

uint32_t sub_800C5558(RRJMemory *m, uint32_t player_index)
{
    uint32_t players = rrj_read32(m, 0x8005B470u);
    uint32_t selected = *(uint8_t *)rrj_at(m, players + 5, 1);
    uint32_t player = players + 112 * selected;
    uint32_t area = 0x800D63E8u + 12 * player_index;
    int32_t x = (int16_t)(rrj_u16(rrj_at(m, area, 2)) + rrj_u16(rrj_at(m, player + 24, 2)));
    int32_t y = (int16_t)(rrj_u16(rrj_at(m, area + 2, 2)) + rrj_u16(rrj_at(m, player + 26, 2)));

    FUNCTION_MARKER(0x800C5558, "RASHCDG.BIN");
    return sub_8001C304(m, x, y, (int16_t)rrj_u16(rrj_at(m, area + 4, 2)), (int16_t)rrj_u16(rrj_at(m, area + 6, 2)), rrj_read32(m, 0x8005B590u + 4 * player_index));
}

uint32_t sub_8005F834(RRJMemory *m, uint32_t packets, uint32_t state, uint32_t reverse)
{
    int32_t mode = rrj_s32(rrj_read32(m, state));
    int32_t progress = rrj_s32(rrj_read32(m, state + 4));
    int32_t step;
    uint32_t result = 3;

    FUNCTION_MARKER(0x8005F834, "RASHCDG.BIN");
    if (mode == 0)
    {
        progress = reverse ? 0 : progress + 1;
        rrj_write32(m, state + 4, (uint32_t)progress);
        if (progress != rrj_s32(rrj_read32(m, state + 8)))
            return 1;
        rrj_write32(m, state, 1);
        rrj_write32(m, state + 4, 0);
        return 1;
    }
    if (mode == 1)
    {
        if (reverse)
        {
            rrj_write32(m, state, 3);
            return 3;
        }
        step = rrj_s32(rrj_read32(m, state + 20)) - progress;
        if (rrj_s32(rrj_read32(m, state + 16)) < step)
            step = rrj_s32(rrj_read32(m, state + 16));
        (void)sub_8005F9B0(m, packets, (int16_t)((uint32_t)step * rrj_read32(m, state + 24)), rrj_s32(rrj_read32(m, state + 28)), rrj_s32(rrj_read32(m, state + 32)));
        progress += step;
        rrj_write32(m, state + 4, (uint32_t)progress);
        if (progress != rrj_s32(rrj_read32(m, state + 20)))
            return (uint32_t)progress;
        rrj_write32(m, state, 2);
        rrj_write32(m, state + 4, 0);
        return 2;
    }
    if (mode == 2)
    {
        if (reverse)
        {
            rrj_write32(m, state, 3);
            rrj_write32(m, state + 4, rrj_read32(m, state + 20));
        }
        return 3;
    }
    if (mode == 3)
    {
        step = progress;
        if (rrj_s32(rrj_read32(m, state + 12)) < step)
            step = rrj_s32(rrj_read32(m, state + 12));
        (void)sub_8005F9B0(m, packets, (int16_t)(0u - (uint32_t)step * rrj_read32(m, state + 24)), rrj_s32(rrj_read32(m, state + 28)), rrj_s32(rrj_read32(m, state + 32)));
        progress -= step;
        rrj_write32(m, state + 4, (uint32_t)progress);
        result = (uint32_t)progress;
        if (!progress)
        {
            rrj_write32(m, state, 0);
            rrj_write32(m, state + 4, 0);
        }
    }
    return result;
}

uint32_t sub_80063408(RRJMemory *m, uint32_t actor, uint32_t packets, uint32_t animation, uint32_t player_index, uint32_t hide_second)
{
    uint32_t game_state = rrj_read32(m, 0x8005B2F8u);
    uint32_t player_flag = 0x8005AD18u + 4 * player_index;
    uint32_t hidden;

    FUNCTION_MARKER(0x80063408, "RASHCDG.BIN");
    if (*(int8_t *)rrj_at(m, game_state, 1) != 1 || (*(uint8_t *)rrj_at(m, rrj_read32(m, actor + 1084), 1) & 0x40u))
        return *(uint8_t *)rrj_at(m, game_state, 1);
    hidden = rrj_read32(m, player_flag) || rrj_read32(m, 0x8005B220u) || rrj_read32(m, 0x8005B288u + 4 * player_index);
    (void)sub_8005F834(m, packets, animation, hidden);
    hidden = hide_second || rrj_read32(m, 0x8005B220u) || rrj_read32(m, 0x8005B288u + 4 * player_index);
    (void)sub_8005F834(m, packets, animation + 36, hidden);
    rrj_write32(m, player_flag, 0);
    return player_flag;
}

static uint32_t frontier_emit_flat_quad(RRJMemory *m, const uint16_t vertices[8], uint32_t color, uint32_t mode, uint32_t player_index)
{
    uint32_t context = rrj_read32(m, 0x8005B470u);
    uint32_t packet = rrj_read32(m, context + 268);
    uint32_t ordering_slot = rrj_read32(m, 0x8005B590u + 4 * player_index);
    uint32_t index;
    static const uint8_t order[8] = {0, 1, 2, 3, 6, 7, 4, 5};

    if (packet + 24 >= rrj_read32(m, 0x8005B4D0u))
        packet = sub_80021C98(m, packet, 24);
    rrj_write32(m, packet, rrj_read32(m, ordering_slot) | 0x05000000u);
    rrj_write32(m, packet + 4, ((mode | 0x29u) << 24) | ((uint32_t)*(uint8_t *)rrj_at(m, color + 2, 1) << 16) | ((uint32_t)*(uint8_t *)rrj_at(m, color + 1, 1) << 8) | *(uint8_t *)rrj_at(m, color, 1));
    rrj_write32(m, ordering_slot, packet);
    for (index = 0; index < 8; ++index)
        rrj_put16(rrj_at(m, packet + 8 + 2 * index, 2), vertices[order[index]]);
    context = rrj_read32(m, 0x8005B470u);
    rrj_write32(m, context + 268, packet + 24);
    return context;
}

uint32_t sub_800C5168(RRJMemory *m, uint32_t vertices, uint32_t color, uint32_t mode, uint32_t player_index)
{
    uint16_t values[8];
    uint32_t index;

    FUNCTION_MARKER(0x800C5168, "RASHCDG.BIN");
    for (index = 0; index < 8; ++index)
        values[index] = rrj_u16(rrj_at(m, vertices + 2 * index, 2));
    return frontier_emit_flat_quad(m, values, color, mode, player_index);
}

uint32_t sub_8001FE80(RRJMemory *m, int32_t angle, uint32_t sine_output, uint32_t cosine_output)
{
    uint32_t packed = rrj_read32(m, 0x8005624Cu + 4 * ((uint32_t)angle & 0xFFFu));
    int32_t sine = (int16_t)packed * 16;
    int32_t cosine = (int16_t)(packed >> 16) * 16;

    FUNCTION_MARKER(0x8001FE80, "SLUS_010.53");
    rrj_write32(m, sine_output, (uint32_t)sine);
    rrj_write32(m, cosine_output, (uint32_t)cosine);
    return (uint32_t)cosine;
}

uint32_t sub_8002DEC8(RRJMemory *m, uint32_t vector, int32_t scale)
{
    uint32_t first;
    uint32_t second;

    FUNCTION_MARKER(0x8002DEC8, "SLUS_010.53");
    first = (uint32_t)sub_8001FC90(scale, rrj_s32(rrj_read32(m, vector)));
    second = (uint32_t)sub_8001FC90(scale, rrj_s32(rrj_read32(m, vector + 4)));
    rrj_write32(m, vector, first);
    rrj_write32(m, vector + 4, second);
    return second;
}

uint32_t sub_8002DE40(RRJMemory *m, int32_t angle, uint32_t vector)
{
    uint32_t packed = rrj_read32(m, 0x8005624Cu + 4 * ((uint32_t)angle & 0xFFFu));
    int32_t sine = (int16_t)packed * 16;
    int32_t cosine = (int16_t)(packed >> 16) * 16;
    int32_t x = rrj_s32(rrj_read32(m, vector));
    int32_t y = rrj_s32(rrj_read32(m, vector + 4));
    uint32_t result;

    FUNCTION_MARKER(0x8002DE40, "SLUS_010.53");
    rrj_write32(m, vector, (uint32_t)sub_8001FC90(cosine, x) - (uint32_t)sub_8001FC90(sine, y));
    result = (uint32_t)sub_8001FC90(cosine, y);
    rrj_write32(m, vector + 4, (uint32_t)sub_8001FC90(sine, x) + result);
    return result;
}

static uint32_t frontier_minimap_marker(RRJMemory *m, int32_t position_x, int32_t position_z, int32_t angle, uint32_t player_index, uint32_t marker_type)
{
    int32_t x = (int32_t)sub_8001FC90(rrj_s32(rrj_read32(m, 0x8005B234u)), position_x);
    int32_t y = (int32_t)sub_8001FC90(rrj_s32(rrj_read32(m, 0x8005B234u)), position_z);
    uint32_t packed = rrj_read32(m, 0x8005624Cu + 4 * ((uint32_t)angle & 0xFFFu));
    int32_t sine = (int16_t)packed * 16;
    int32_t cosine = (int16_t)(packed >> 16) * 16;
    int32_t rotated_x = (int32_t)((uint32_t)sub_8001FC90(cosine, x) - (uint32_t)sub_8001FC90(sine, y));
    int32_t rotated_y = (int32_t)((uint32_t)sub_8001FC90(sine, x) + (uint32_t)sub_8001FC90(cosine, y));
    uint32_t area = 0x800D63E8u + 12 * player_index;
    int32_t center_x = (int16_t)(rrj_u16(rrj_at(m, area + 8, 2)) + (uint32_t)(rotated_x >> 16));
    int32_t center_y = (int16_t)(rrj_u16(rrj_at(m, area + 10, 2)) - (uint32_t)(rotated_y >> 16));
    int16_t vertices[8];
    int32_t half_width;
    int32_t half_height;

    if (center_x < (int16_t)rrj_u16(rrj_at(m, area, 2)) || center_x > (int16_t)rrj_u16(rrj_at(m, area, 2)) + (int16_t)rrj_u16(rrj_at(m, area + 4, 2)) - 1 || center_y < (int16_t)rrj_u16(rrj_at(m, area + 2, 2)) || center_y > (int16_t)rrj_u16(rrj_at(m, area + 2, 2)) + (int16_t)rrj_u16(rrj_at(m, area + 6, 2)) - 1)
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
    return frontier_emit_flat_quad(m, (const uint16_t *)vertices, 0x800CCBE0u + 4 * marker_type, 0, player_index);
}

uint32_t sub_800C5380(RRJMemory *m, uint32_t position, int32_t angle, uint32_t player_index, uint32_t marker_type)
{
    FUNCTION_MARKER(0x800C5380, "RASHCDG.BIN");
    return frontier_minimap_marker(m, rrj_s32(rrj_read32(m, position)), rrj_s32(rrj_read32(m, position + 8)), angle, player_index, marker_type);
}

uint32_t sub_80013E64(RRJMemory *m, uint32_t state)
{
    uint32_t game_state = rrj_read32(m, 0x8005B2F8u);
    uint32_t interval = rrj_read32(m, state + (rrj_read32(m, state + 16) ? 4 : 8));
    uint32_t current = rrj_read32(m, game_state + 16);
    uint32_t anchor = rrj_read32(m, state + 20);
    int32_t delta = anchor ? rrj_s32(current - anchor) : 0;
    uint32_t in_or_after = 1;
    uint32_t outside = 0;
    uint32_t mode = *(uint8_t *)rrj_at(m, game_state, 1);

    FUNCTION_MARKER(0x80013E64, "SLUS_010.53");
    if ((mode - 3u) < 2u)
        return 1;
    if (delta)
    {
        int32_t start = rrj_s32(rrj_read32(m, state + 24));
        uint32_t reaches = 0;

        in_or_after = start >= delta;
        if (start < delta)
            reaches = rrj_s32((uint32_t)start + rrj_read32(m, state + 28)) >= delta;
        if (!in_or_after)
            outside = !reaches;
    }
    if (in_or_after && rrj_s32(rrj_read32(m, state) + interval) < rrj_s32(current))
    {
        int32_t count = rrj_s32(rrj_read32(m, state + 12)) + 1;

        rrj_write32(m, state + 16, rrj_read32(m, state + 16) ^ 1u);
        rrj_write32(m, state + 12, (uint32_t)count);
        rrj_write32(m, state, current);
        if (count >= 17)
        {
            rrj_write32(m, state + 12, (uint32_t)-60);
            return (uint32_t)-60;
        }
        return 1;
    }
    if (anchor)
    {
        uint32_t result = !outside;

        if (!in_or_after)
        {
            rrj_write32(m, state + 16, result);
            if (outside)
                rrj_write32(m, state + 20, 0);
        }
        return result;
    }
    return 0;
}

uint32_t sub_80013AF8(RRJMemory *m, uint32_t packet, uint32_t template_data)
{
    int32_t state = *(int8_t *)rrj_at(m, packet + 32, 1);
    uint32_t packed;

    FUNCTION_MARKER(0x80013AF8, "SLUS_010.53");
    if (state == 1)
    {
        rrj_put16(rrj_at(m, packet + 28, 2), *(uint8_t *)rrj_at(m, template_data + 2, 1));
        rrj_put16(rrj_at(m, packet + 30, 2), *(uint8_t *)rrj_at(m, template_data + 3, 1));
        return *(uint8_t *)rrj_at(m, template_data + 3, 1);
    }
    if (state != 0)
        return 1;
    rrj_write32(m, packet + 20, template_data);
    rrj_put16(rrj_at(m, packet + 28, 2), *(uint8_t *)rrj_at(m, template_data + 2, 1));
    rrj_put16(rrj_at(m, packet + 30, 2), *(uint8_t *)rrj_at(m, template_data + 3, 1));
    packed = (uint32_t)*(uint8_t *)rrj_at(m, template_data + 2, 1) | ((uint32_t)*(uint8_t *)rrj_at(m, template_data + 3, 1) << 16);
    rrj_write32(m, packet + 8, rrj_read32(m, packet + 24));
    rrj_write32(m, packet + 16, packed);
    rrj_write32(m, packet + 28, packed);
    return packed;
}

uint32_t sub_800C569C(RRJMemory *m, uint32_t subject, uint32_t type_filter, int32_t angle, uint32_t player_index)
{
    uint32_t actor = rrj_read32(m, 0x800CE4D0u);
    int32_t remaining = rrj_s32(rrj_read32(m, rrj_read32(m, 0x800CE4DCu)));
    uint32_t marker_state = 0x800D6198u + 224 * player_index;

    FUNCTION_MARKER(0x800C569C, "RASHCDG.BIN");
    while (remaining >= 0)
    {
        uint32_t owner = rrj_u16(rrj_at(m, actor + 172, 2));

        if (rrj_u16(rrj_at(m, actor + 320, 2)) && owner != player_index)
        {
            uint32_t body = rrj_read32(m, actor + 852);
            uint32_t source = rrj_read32(m, body + 604) < 3 ? actor : body;
            int32_t relative_x = rrj_s32(rrj_read32(m, source + 184) - rrj_read32(m, subject + 184));
            int32_t relative_z = rrj_s32(rrj_read32(m, source + 192) - rrj_read32(m, subject + 192));
            uint32_t game_state = rrj_read32(m, 0x8005B2F8u);
            uint32_t marker_type = 0;
            uint32_t flags = *(uint8_t *)rrj_at(m, game_state + 4, 1);
            int draw = 1;

            if (owner >= rrj_read32(m, game_state + 48) || player_index != owner)
            {
                uint32_t type = *(uint8_t *)rrj_at(m, rrj_read32(m, actor + 1084) + 1, 1) & 0xFu;

                if (type == (type_filter & 0xFu))
                    marker_type = 2;
                else if (type == 2)
                    marker_type = 4;
                else
                    marker_type = flags == 44 ? 4 : 3;
            }
            if ((flags & 0x10u) && owner == (player_index ^ 1u))
                draw = 0;
            else if ((flags & 1u) && owner != player_index && owner == *(uint8_t *)rrj_at(m, game_state + 6, 1))
                draw = 0;
            if (!draw)
            {
                (void)sub_80013E64(m, marker_state + 64);
                marker_type = 5;
                draw = rrj_read32(m, marker_state + 80) != 0;
            }
            if (draw)
                (void)frontier_minimap_marker(m, relative_x, relative_z, angle, player_index, marker_type);
        }
        --remaining;
        actor += rrj_read32(m, 0x800CE4D4u);
    }
    return frontier_minimap_marker(m, 0, 0, angle, player_index, 0);
}

uint32_t sub_800C52A8(RRJMemory *m, uint32_t actor, uint32_t player_index, uint32_t inhibited)
{
    uint32_t body;
    uint32_t subject;
    uint32_t active;
    int32_t angle;

    FUNCTION_MARKER(0x800C52A8, "RASHCDG.BIN");
    if (inhibited)
        return inhibited;
    inhibited = rrj_read32(m, 0x8005B288u + 4 * player_index);
    if (inhibited)
        return inhibited;
    body = rrj_read32(m, actor + 852);
    active = rrj_read32(m, body + 604) < 3;
    subject = active ? actor : body;
    if (!subject)
        return active;
    active = rrj_u16(rrj_at(m, subject + 320, 2));
    if (!active)
        return active;
    (void)sub_800C5618(m, player_index);
    angle = rrj_s32(sub_80020018(m, (uint32_t)((int32_t)(int16_t)rrj_u16(rrj_at(m, subject + 444, 2)) * 16), (uint32_t)((int32_t)(int16_t)rrj_u16(rrj_at(m, subject + 448, 2)) * 16)));
    if (angle < 0)
        angle += 4096;
    (void)sub_800C569C(m, subject, *(uint8_t *)rrj_at(m, rrj_read32(m, actor + 1084) + 1, 1), angle, player_index);
    return sub_800C5558(m, player_index);
}

uint32_t sub_800C52A0(RRJMemory *m, uint32_t actor, uint32_t player_index)
{
    FUNCTION_MARKER(0x800C52A0, "RASHCDG.BIN");
    return sub_800C52A8(m, actor, player_index, rrj_read32(m, 0x8005B220u));
}

uint32_t sub_80061E50(RRJMemory *m, uint32_t state, uint32_t player_index, uint32_t configuration)
{
    uint32_t ordering_slot = rrj_read32(m, 0x8005B590u + 4 * player_index);
    uint32_t selected;
    uint32_t other;

    FUNCTION_MARKER(0x80061E50, "RASHCDG.BIN");
    if (rrj_read32(m, configuration) != 2)
    {
        selected = rrj_read32(m, 0x8005B38Cu) + 1096 * player_index;
        sub_8002CD78(m, rrj_read32(m, 0x8005AD2Cu), *(uint8_t *)rrj_at(m, rrj_read32(m, selected + 1084) + 38, 1), (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, state + 636, 2)), (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, state + 638, 2)), ordering_slot, 0x00808080u);
    }
    other = rrj_read32(m, 0x8005AD0Cu + 4 * player_index);
    if (other && rrj_read32(m, configuration + 36) != 2)
    {
        sub_8002CD78(m, rrj_read32(m, 0x8005AD2Cu), *(uint8_t *)rrj_at(m, rrj_read32(m, other + 1084) + 38, 1), (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, state + 1140, 2)), (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, state + 1142, 2)), ordering_slot, 0x00808080u);
    }
    return other;
}

static uint32_t frontier_link_race_packet(RRJMemory *m, uint32_t ordering_slot, uint32_t packet)
{
    int32_t type = *(int8_t *)rrj_at(m, packet + 32, 1);
    uint32_t tag = (rrj_read32(m, 0x800CC68Cu + 4 * (uint32_t)type) << 24) | rrj_read32(m, ordering_slot);

    rrj_write32(m, packet, tag);
    rrj_write32(m, ordering_slot, packet & 0xFFFFFFu);
    return tag;
}

static void frontier_initialize_template_packet(RRJMemory *m, uint32_t packet, uint32_t entry)
{
    uint32_t packed;

    if (*(int8_t *)rrj_at(m, packet + 32, 1) != 0)
        return;
    packed = (uint32_t)*(uint8_t *)rrj_at(m, entry + 2, 1) | ((uint32_t)*(uint8_t *)rrj_at(m, entry + 3, 1) << 16);
    rrj_write32(m, packet + 12, rrj_read32(m, entry + 4));
    rrj_write32(m, packet + 8, rrj_read32(m, packet + 24));
    rrj_write32(m, packet + 16, packed);
    rrj_write32(m, packet + 28, packed);
}

static void frontier_initialize_digit_packet(RRJMemory *m, uint32_t packet, uint32_t digit)
{
    frontier_initialize_template_packet(m, packet, 0x800D4620u + 16 * digit);
}

uint32_t sub_80013B90(RRJMemory *m, int32_t value, uint32_t packets, uint32_t table)
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
    frontier_initialize_template_packet(m, packets, table + 16 * (uint32_t)first);
    frontier_initialize_template_packet(m, packets + 36, table + 16 * (uint32_t)second);
    frontier_initialize_template_packet(m, packets + 72, table + 16 * (uint32_t)third);
    frontier_initialize_template_packet(m, packets + 108, table + 16 * (uint32_t)fourth);
    frontier_initialize_template_packet(m, packets + 144, table + 16 * (uint32_t)fifth);
    return (uint32_t)fourth & 1u;
}

uint32_t sub_8005FE58(RRJMemory *m, uint32_t ordering_slot, uint32_t packets, uint32_t state, uint32_t actor)
{
    uint32_t result;

    FUNCTION_MARKER(0x8005FE58, "RASHCDG.BIN");
    if (rrj_read32(m, actor + 180) < 18)
    {
        result = rrj_read32(m, state + 140);
        if (rrj_s32(result) <= 0)
        {
            rrj_write32(m, state + 140, 0);
            return result;
        }
        (void)sub_80013E64(m, state + 128);
        result = rrj_read32(m, state + 144);
        if (result)
            result = frontier_link_race_packet(m, ordering_slot, packets + 144);
        return result;
    }
    result = rrj_s32(rrj_read32(m, state + 108)) < 3 ? rrj_read32(m, state + 112) : 1;
    if (!result)
        return 0x80060000u;
    result = rrj_read32(m, 0x8005AD48u) < 2;
    if (result)
        return result;
    result = rrj_read32(m, 0x8005AD48u) < 9;
    if (result)
        return frontier_link_race_packet(m, ordering_slot, packets + 36);
    return result;
}

uint32_t sub_80016528(RRJMemory *m, RRJRaceLeafCall call)
{
    uint32_t result;

    FUNCTION_MARKER(0x80016528, "SLUS_010.53");
    result = frontier_call(m, call, 0x8001F7EC, rrj_read32(m, 0x8005B438u), 0, 0);
    rrj_write32(m, 0x8005B438u, 0);
    return result;
}

uint32_t sub_8005FF84(RRJMemory *m, uint32_t ordering_slot, uint32_t packets, uint32_t state, uint32_t actor, RRJRaceLeafCall call)
{
    uint32_t packet = packets + 1872;
    uint32_t descriptor = rrj_read32(m, actor + 1084);

    FUNCTION_MARKER(0x8005FF84, "RASHCDG.BIN");
    if ((rrj_read32(m, descriptor) & 0x60u) == 0x40u)
    {
        int32_t value = rrj_s32(rrj_read32(m, 0x8005B230u));
        int32_t high = value >> 16;
        int32_t excess = 2 - high;
        int32_t variant = high + (value < 0 ? -high : 0) + 1;
        uint32_t entry;

        if (excess < 0)
            variant += excess;
        entry = 0x800D4960u + 16 * (uint32_t)variant;
        if (*(int8_t *)rrj_at(m, packet + 32, 1) == 0)
        {
            uint32_t packed = (uint32_t)*(uint8_t *)rrj_at(m, entry + 2, 1) | ((uint32_t)*(uint8_t *)rrj_at(m, entry + 3, 1) << 16);

            rrj_write32(m, packet + 12, rrj_read32(m, entry + 4));
            rrj_write32(m, packet + 8, rrj_read32(m, packet + 24));
            rrj_write32(m, packet + 16, packed);
            rrj_write32(m, packet + 28, packed);
        }
        if (rrj_read32(m, state + 84) != (uint32_t)variant)
        {
            (void)sub_80017BA0(m, 0, 0, 76, 0);
            (void)sub_80017BA0(m, 0, 0, 75, 0);
            (void)sub_80017BA0(m, 0, 0, 29, 0);
        }
        rrj_write32(m, state + 84, (uint32_t)variant);
        return frontier_link_race_packet(m, ordering_slot, packet);
    }
    if (rrj_read32(m, state + 84))
    {
        if ((*(uint8_t *)rrj_at(m, rrj_read32(m, 0x8005B2F8u) + 4, 1) & 1u) == 0)
        {
            (void)sub_80017BA0(m, 0, 0, 65, 0);
            (void)sub_80017BA0(m, 0, 0, 106, 0);
            (void)sub_80017BA0(m, 0, 0, 80, 0);
        }
        (void)sub_80016528(m, call);
        rrj_write32(m, state + 84, 0);
    }
    return 0x80060000u;
}

uint32_t sub_80060178(RRJMemory *m, uint32_t ordering_slot, uint32_t packets, uint32_t state, uint32_t actor)
{
    uint32_t body = rrj_read32(m, actor + 852);
    int32_t value = (rrj_read32(m, body + 604) - 3u) < 2u ? 0 : rrj_s32(rrj_read32(m, actor + 480)) >> 8;
    uint32_t first_packet = packets + 360;
    uint32_t last_packet = packets + 432;
    uint32_t result;

    FUNCTION_MARKER(0x80060178, "RASHCDG.BIN");
    if (rrj_read32(m, state + 8) != (uint32_t)value)
    {
        int32_t scaled = (int32_t)sub_8001FC90(value, 146599);
        uint32_t first;
        uint32_t second;
        uint32_t third;

        rrj_write32(m, state + 8, (uint32_t)value);
        if (scaled < 0)
            scaled = 0;
        first = (uint32_t)scaled / 25600u;
        scaled -= (int32_t)(25600u * first);
        second = (uint32_t)scaled / 2560u;
        third = ((uint32_t)scaled - 2560u * second) >> 8;
        frontier_initialize_digit_packet(m, first_packet, first);
        frontier_initialize_digit_packet(m, packets + 396, second);
        frontier_initialize_digit_packet(m, last_packet, third);
    }
    result = (rrj_read32(m, 0x800CC68Cu + 4 * (uint32_t)*(int8_t *)rrj_at(m, last_packet + 32, 1)) << 24) | rrj_read32(m, ordering_slot);
    rrj_write32(m, last_packet, result);
    result = first_packet & 0xFFFFFFu;
    rrj_write32(m, ordering_slot, result);
    return result;
}

uint32_t sub_800603E4(RRJMemory *m, uint32_t ordering_slot, uint32_t packets, uint32_t state, uint32_t player_index)
{
    uint32_t first_packet = packets + 468;
    uint32_t last_packet = packets + 576;
    uint32_t result;

    FUNCTION_MARKER(0x800603E4, "RASHCDG.BIN");
    if ((rrj_read32(m, 0x8005ACDCu) & 7u) == 7u)
    {
        int32_t value = (int32_t)sub_8001FC90(rrj_s32(rrj_read32(m, 0x8005B380u + 4 * player_index)), 40);

        if (rrj_read32(m, state + 12) != (uint32_t)value)
        {
            uint32_t first;
            uint32_t second;
            uint32_t third;
            int32_t remainder;

            rrj_write32(m, state + 12, (uint32_t)value);
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
            frontier_initialize_template_packet(m, first_packet, 0x800D4710u + 16 * first);
            frontier_initialize_template_packet(m, packets + 504, 0x800D4710u + 16 * second);
            frontier_initialize_template_packet(m, packets + 540, 0x800D4920u);
            frontier_initialize_template_packet(m, last_packet, 0x800D4710u + 16 * third);
        }
    }
    result = (rrj_read32(m, 0x800CC68Cu + 4 * (uint32_t)*(int8_t *)rrj_at(m, last_packet + 32, 1)) << 24) | rrj_read32(m, ordering_slot);
    rrj_write32(m, last_packet, result);
    result = first_packet & 0xFFFFFFu;
    rrj_write32(m, ordering_slot, result);
    return result;
}

uint32_t sub_800606F8(RRJMemory *m, uint32_t ordering_slot, uint32_t packets, uint32_t state, uint32_t actor, uint32_t flags, uint32_t player_index, uint32_t blink_state, uint32_t game_state)
{
    uint32_t descriptor;
    uint32_t rank;
    uint32_t result = 0x80060000u;

    FUNCTION_MARKER(0x800606F8, "RASHCDG.BIN");
    if (*(uint8_t *)rrj_at(m, game_state + 4, 1) & 4u)
        return result;
    if (rrj_read32(m, 0x8005B220u))
        return result;
    descriptor = rrj_read32(m, actor + 1084);
    if (rrj_read32(m, 0x8005B288u + 4 * player_index) && *(uint8_t *)rrj_at(m, descriptor + 39, 1) != 0xFFu)
        return result;
    rank = rrj_read32(m, state);
    if (rrj_read32(m, 0x8005ACDCu) & 1u)
    {
        uint32_t nearest;
        uint32_t distance = rrj_nearest_junction_local(m, actor + 360, &nearest, 0);
        uint32_t next_rank = rank;

        if (rrj_u16(rrj_at(m, actor + 362, 2)) == 0 && distance > 0xA0000u)
            next_rank = sub_800138E8(m, actor, 0);
        if (next_rank != rank && rrj_s32(next_rank) < 20)
        {
            uint32_t digit = next_rank;
            uint32_t tens = 0;
            uint32_t other = rrj_read32(m, 0x8005B1FCu);
            uint32_t other_tens = 0;

            rrj_write32(m, state, next_rank);
            rank = next_rank;
            if (digit >= 10)
            {
                digit -= 10;
                tens = 1;
            }
            if (tens)
                frontier_initialize_template_packet(m, packets + 1620, 0x800D4620u + 16 * tens);
            frontier_initialize_template_packet(m, packets + 1656, 0x800D4620u + 16 * digit);
            frontier_initialize_template_packet(m, packets + 1692, 0x800D4910u);
            if (other >= 10)
            {
                other -= 10;
                other_tens = 1;
            }
            if (other_tens)
                frontier_initialize_template_packet(m, packets + 1728, 0x800D4710u + 16 * other_tens);
            frontier_initialize_template_packet(m, packets + 1764, 0x800D4710u + 16 * other);
        }
    }
    {
        uint32_t linked = rrj_read32(m, actor + 428);
        uint32_t low_speed = (int32_t)sub_8001FC90(rrj_s32(rrj_read32(m, actor + 324)), 40) < 4096;
        uint32_t enabled = linked ? (rrj_u16(rrj_at(m, linked + 118, 2)) >> (rrj_u16(rrj_at(m, actor + 172, 2)) & 31u)) & 1u : 0;

        if (low_speed && enabled)
        {
            if (!rrj_read32(m, 0x8005B220u) && !(flags & 2u) && !rrj_read32(m, 0x8005B288u + 4 * player_index))
                (void)sub_80013E64(m, blink_state + 64);
        }
        if (!(low_speed && enabled) || rrj_read32(m, blink_state + 80))
        {
            uint32_t packet = packets + 1656;
            uint32_t first = rank < 10 ? packet : packets + 1620;
            int32_t type = *(int8_t *)rrj_at(m, packet + 32, 1);

            rrj_write32(m, packet, (rrj_read32(m, 0x800CC68Cu + 4 * (uint32_t)type) << 24) | rrj_read32(m, ordering_slot));
            rrj_write32(m, ordering_slot, first & 0xFFFFFFu);
        }
    }
    {
        uint32_t packet = packets + 1764;
        uint32_t first = rrj_read32(m, 0x8005B1FCu) < 10 ? packet : packets + 1728;
        int32_t type = *(int8_t *)rrj_at(m, packet + 32, 1);

        rrj_write32(m, packet, (rrj_read32(m, 0x800CC68Cu + 4 * (uint32_t)type) << 24) | rrj_read32(m, ordering_slot));
        rrj_write32(m, ordering_slot, first & 0xFFFFFFu);
        result = frontier_link_race_packet(m, ordering_slot, packets + 1692);
    }
    return result;
}

uint32_t sub_800606F0(RRJMemory *m, uint32_t ordering_slot, uint32_t packets, uint32_t state, uint32_t actor, uint32_t flags, uint32_t player_index, uint32_t blink_state)
{
    FUNCTION_MARKER(0x800606F0, "RASHCDG.BIN");
    return sub_800606F8(m, ordering_slot, packets, state, actor, flags, player_index, blink_state, rrj_read32(m, 0x8005B2F8u));
}

static int32_t frontier_clamp_meter(int32_t value)
{
    if (value < 2)
        return 2;
    if (value > 127)
        return 127;
    return value;
}

static void frontier_update_meter_packet(RRJMemory *m, uint32_t packet, int32_t value, uint32_t blink_state, uint32_t game_state)
{
    int32_t phase;
    uint32_t width;
    uint32_t color;

    if (value < 33)
    {
        uint32_t mode = *(uint8_t *)rrj_at(m, game_state, 1);

        if ((mode - 3u) >= 2u)
            (void)sub_80013E64(m, blink_state + 192);
        phase = 1 - rrj_s32(rrj_read32(m, blink_state + 208));
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
    rrj_put16(rrj_at(m, packet + 28, 2), (uint16_t)width);
    if (phase > 0)
        --phase;
    color = 0x800CC698u + 4 * (uint32_t)phase;
    rrj_write32(m, packet + 4, ((uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, packet + 34, 2)) << 25) | ((uint32_t)*(uint8_t *)rrj_at(m, color + 2, 1) << 16) | ((uint32_t)*(uint8_t *)rrj_at(m, color + 1, 1) << 8) | *(uint8_t *)rrj_at(m, color, 1) | 0x60000000u);
    rrj_write32(m, packet + 8, rrj_u16(rrj_at(m, packet + 24, 2)) | ((uint32_t)rrj_u16(rrj_at(m, packet + 26, 2)) << 16));
    rrj_write32(m, packet + 12, rrj_u16(rrj_at(m, packet + 28, 2)) | ((uint32_t)rrj_u16(rrj_at(m, packet + 30, 2)) << 16));
}

static uint32_t frontier_link_packet_range(RRJMemory *m, uint32_t ordering_slot, uint32_t tag_packet, uint32_t first_packet)
{
    int32_t type = *(int8_t *)rrj_at(m, tag_packet + 32, 1);
    uint32_t tag = (rrj_read32(m, 0x800CC68Cu + 4 * (uint32_t)type) << 24) | rrj_read32(m, ordering_slot);

    rrj_write32(m, tag_packet, tag);
    rrj_write32(m, ordering_slot, first_packet & 0xFFFFFFu);
    return tag;
}

uint32_t sub_80060C18(RRJMemory *m, uint32_t ordering_slot, uint32_t packets, uint32_t state, uint32_t actor, uint32_t player_index, uint32_t configuration, uint32_t blink_state, uint32_t display_flags)
{
    uint32_t descriptor = rrj_read32(m, actor + 1084);
    uint32_t game_state = rrj_read32(m, 0x8005B2F8u);
    uint32_t other = rrj_read32(m, actor + 856);
    int32_t primary = 0;
    int32_t secondary = 0;
    uint32_t result = 2;

    FUNCTION_MARKER(0x80060C18, "RASHCDG.BIN");
    if (display_flags & 1u)
    {
        uint32_t glyph = *(uint8_t *)rrj_at(m, descriptor + 46, 1);

        if (glyph != rrj_read32(m, state + 20))
        {
            rrj_write32(m, state + 20, glyph);
            frontier_initialize_template_packet(m, packets + 864, 0x800D4860u + 16 * glyph);
            rrj_write32(m, 0x8005AD18u + 4 * player_index, 1);
        }
        primary = ((int32_t)*(uint8_t *)rrj_at(m, descriptor + 15, 1) << 7) / ((int32_t)*(uint8_t *)rrj_at(m, descriptor + 13, 1) + 1);
        secondary = ((int32_t)*(uint8_t *)rrj_at(m, descriptor + 37, 1) << 7) / ((int32_t)*(uint8_t *)rrj_at(m, descriptor + 36, 1) + 1);
        if (*(uint8_t *)rrj_at(m, descriptor + 15, 1))
            primary = frontier_clamp_meter(primary);
        if (primary != rrj_s32(rrj_read32(m, state + 24)))
        {
            rrj_write32(m, state + 24, (uint32_t)primary);
            rrj_write32(m, 0x8005AD18u + 4 * player_index, 1);
        }
        if (*(uint8_t *)rrj_at(m, descriptor + 37, 1))
            secondary = frontier_clamp_meter(secondary);
        else
            secondary = 2 * (rrj_read32(m, rrj_read32(m, actor + 852) + 604) < 3);
        if (secondary != rrj_s32(rrj_read32(m, state + 28)))
        {
            rrj_write32(m, state + 28, (uint32_t)secondary);
            rrj_write32(m, 0x8005AD18u + 4 * player_index, 1);
        }
        if ((*(uint8_t *)rrj_at(m, game_state + 4, 1) & 8u) && other && rrj_read32(m, actor + 1088) && *(uint8_t *)rrj_at(m, game_state + player_index + 10, 1))
        {
            uint32_t other_descriptor = rrj_read32(m, other + 1084);
            int32_t other_meter = ((int32_t)*(uint8_t *)rrj_at(m, other_descriptor + 15, 1) << 7) / ((int32_t)*(uint8_t *)rrj_at(m, other_descriptor + 13, 1) + 1);

            if (*(uint8_t *)rrj_at(m, other_descriptor + 15, 1))
                other_meter = frontier_clamp_meter(other_meter);
            if (other_meter != rrj_s32(rrj_read32(m, state + 36)))
            {
                rrj_write32(m, state + 36, (uint32_t)other_meter);
                rrj_write32(m, 0x8005AD18u + 4 * player_index, 1);
                frontier_update_meter_packet(m, packets + 1008, other_meter, blink_state, game_state);
            }
            glyph = *(uint8_t *)rrj_at(m, other_descriptor + 46, 1);
            if (glyph != rrj_read32(m, state + 32))
            {
                rrj_write32(m, state + 32, glyph);
                frontier_initialize_template_packet(m, packets + 1044, 0x800D4860u + 16 * glyph);
                rrj_write32(m, 0x8005AD18u + 4 * player_index, 1);
            }
        }
        frontier_update_meter_packet(m, packets + 720, primary, blink_state, game_state);
        frontier_update_meter_packet(m, packets + 828, secondary, blink_state, game_state);
    }
    if (rrj_read32(m, configuration) != 2)
    {
        result = frontier_link_packet_range(m, ordering_slot, packets + 864, packets + 648);
        if (*(uint8_t *)rrj_at(m, descriptor + 47, 1) && *(uint8_t *)rrj_at(m, descriptor + 46, 1) != 9)
            result = frontier_link_race_packet(m, ordering_slot, packets + 900);
        if (other && rrj_read32(m, actor + 1088) && *(uint8_t *)rrj_at(m, game_state + player_index + 10, 1))
        {
            uint32_t other_descriptor = rrj_read32(m, other + 1084);

            result = frontier_link_packet_range(m, ordering_slot, packets + 1044, packets + 936);
            if (*(uint8_t *)rrj_at(m, other_descriptor + 47, 1) && *(uint8_t *)rrj_at(m, other_descriptor + 46, 1) != 9)
                result = frontier_link_race_packet(m, ordering_slot, packets + 1080);
        }
    }
    return result;
}

uint32_t sub_80060C10(RRJMemory *m, uint32_t ordering_slot, uint32_t packets, uint32_t state, uint32_t actor, uint32_t player_index, uint32_t configuration, uint32_t blink_state)
{
    FUNCTION_MARKER(0x80060C10, "RASHCDG.BIN");
    return sub_80060C18(m, ordering_slot, packets, state, actor, player_index, configuration, blink_state, rrj_read32(m, 0x8005ACDCu));
}

uint32_t sub_80061494(RRJMemory *m, uint32_t ordering_slot, uint32_t packets, uint32_t state, uint32_t actor, uint32_t player_index, uint32_t configuration, uint32_t blink_state, uint32_t display_flags)
{
    uint32_t game_state = rrj_read32(m, 0x8005B2F8u);
    uint32_t selected = 0;
    uint32_t changed = 0;

    FUNCTION_MARKER(0x80061494, "RASHCDG.BIN");
    if (display_flags & 1u)
    {
        int32_t relation = *(int8_t *)rrj_at(m, actor + 946, 1) - 1;
        uint32_t candidate_id = rrj_u16(rrj_at(m, actor + 958 + 8 * (uint32_t)relation, 2));

        if (candidate_id < rrj_read32(m, 0x8005B1F8u) && candidate_id != player_index)
        {
            selected = rrj_read32(m, 0x8005B3A0u) + 1096 * candidate_id;
            changed = 1;
            rrj_write32(m, 0x8005AD18u + 4 * player_index, 1);
        }
        else if (*(uint8_t *)rrj_at(m, game_state + 4, 1) != 44 || *(uint8_t *)rrj_at(m, game_state + 57, 1))
        {
            selected = sub_8008B84C(m, actor);
        }
        if (selected)
        {
            int32_t selected_relation = *(int8_t *)rrj_at(m, selected + 946, 1) - 1;
            uint32_t relation_id = rrj_u16(rrj_at(m, selected + 958 + 8 * (uint32_t)selected_relation, 2));
            uint32_t descriptor = rrj_read32(m, selected + 1084);

            if (relation_id == rrj_u16(rrj_at(m, actor + 172, 2)))
            {
                changed = 1;
                rrj_write32(m, 0x8005AD18u + 4 * player_index, 1);
            }
            if (rrj_read32(m, state + 60) != 5 || (*(uint8_t *)rrj_at(m, descriptor + 1, 1) & 0xFu) == 2)
            {
                int32_t primary = ((int32_t)*(uint8_t *)rrj_at(m, descriptor + 15, 1) << 7) / ((int32_t)*(uint8_t *)rrj_at(m, descriptor + 13, 1) + 1);
                int32_t secondary = ((int32_t)*(uint8_t *)rrj_at(m, descriptor + 37, 1) << 7) / ((int32_t)*(uint8_t *)rrj_at(m, descriptor + 36, 1) + 1);
                uint32_t glyph = *(uint8_t *)rrj_at(m, descriptor + 46, 1);

                rrj_write32(m, 0x8005AD0Cu + 4 * player_index, selected);
                if (*(uint8_t *)rrj_at(m, descriptor + 15, 1))
                    primary = frontier_clamp_meter(primary);
                if (*(uint8_t *)rrj_at(m, descriptor + 37, 1))
                    secondary = frontier_clamp_meter(secondary);
                else
                    secondary = 2 * (rrj_read32(m, rrj_read32(m, selected + 852) + 604) < 3);
                if (selected != rrj_read32(m, state + 64))
                {
                    rrj_write32(m, state + 64, selected);
                    (void)sub_80013AF8(m, packets + 1116, 0x800D4850u);
                    frontier_initialize_template_packet(m, packets + 1116, 0x800D4850u);
                    changed = 1;
                }
                if (primary != rrj_s32(rrj_read32(m, state + 40)))
                {
                    rrj_write32(m, state + 40, (uint32_t)primary);
                    changed = 1;
                }
                if (secondary != rrj_s32(rrj_read32(m, state + 44)))
                {
                    rrj_write32(m, state + 44, (uint32_t)secondary);
                    changed = 1;
                }
                if (glyph != rrj_read32(m, state + 48))
                {
                    rrj_write32(m, state + 48, glyph);
                    frontier_initialize_template_packet(m, packets + 1368, 0x800D4860u + 16 * glyph);
                    changed = 1;
                }
            }
            else
            {
                rrj_write32(m, 0x8005AD0Cu + 4 * player_index, 0);
            }
        }
        else
        {
            rrj_write32(m, 0x8005AD0Cu + 4 * player_index, 0);
        }
    }
    selected = rrj_read32(m, state + 64);
    if (!selected)
        return changed;
    if (rrj_read32(m, configuration + 36) != 2)
    {
        uint32_t descriptor = rrj_read32(m, selected + 1084);
        uint32_t linked;

        frontier_update_meter_packet(m, packets + 1224, rrj_s32(rrj_read32(m, state + 40)), blink_state, game_state);
        frontier_update_meter_packet(m, packets + 1332, rrj_s32(rrj_read32(m, state + 44)), blink_state, game_state);
        linked = rrj_read32(m, selected + 856);
        if (linked && rrj_read32(m, selected + 1088) && *(uint8_t *)rrj_at(m, game_state + rrj_u16(rrj_at(m, selected + 172, 2)) + 10, 1))
        {
            uint32_t linked_descriptor = rrj_read32(m, linked + 1084);
            int32_t linked_meter = ((int32_t)*(uint8_t *)rrj_at(m, linked_descriptor + 15, 1) << 7) / ((int32_t)*(uint8_t *)rrj_at(m, linked_descriptor + 13, 1) + 1);
            uint32_t linked_glyph = *(uint8_t *)rrj_at(m, linked_descriptor + 46, 1);

            if (*(uint8_t *)rrj_at(m, linked_descriptor + 15, 1))
                linked_meter = frontier_clamp_meter(linked_meter);
            if (linked_meter != rrj_s32(rrj_read32(m, state + 56)))
            {
                rrj_write32(m, state + 56, (uint32_t)linked_meter);
                frontier_update_meter_packet(m, packets + 1512, linked_meter, blink_state, game_state);
                changed = 1;
            }
            if (linked_glyph != rrj_read32(m, state + 52))
            {
                rrj_write32(m, state + 52, linked_glyph);
                frontier_initialize_template_packet(m, packets + 1548, 0x800D4860u + 16 * linked_glyph);
                changed = 1;
            }
            (void)frontier_link_packet_range(m, ordering_slot, packets + 1548, packets + 1440);
            if (*(uint8_t *)rrj_at(m, linked_descriptor + 47, 1) && linked_glyph != 9)
                (void)frontier_link_race_packet(m, ordering_slot, packets + 1584);
        }
        (void)frontier_link_packet_range(m, ordering_slot, packets + 1368, packets + 1152);
        if (*(uint8_t *)rrj_at(m, descriptor + 47, 1) && *(uint8_t *)rrj_at(m, descriptor + 46, 1) != 9)
            (void)frontier_link_race_packet(m, ordering_slot, packets + 1404);
    }
    return changed;
}

uint32_t sub_8006148C(RRJMemory *m, uint32_t ordering_slot, uint32_t packets, uint32_t state, uint32_t actor, uint32_t player_index, uint32_t configuration, uint32_t blink_state)
{
    FUNCTION_MARKER(0x8006148C, "RASHCDG.BIN");
    return sub_80061494(m, ordering_slot, packets, state, actor, player_index, configuration, blink_state, rrj_read32(m, 0x8005ACDCu));
}

uint32_t sub_80061F6C(RRJMemory *m, uint32_t ordering_slot, uint32_t packets, uint32_t state, uint32_t actor, uint32_t player_index)
{
    uint32_t game_state = rrj_read32(m, 0x8005B2F8u);
    uint32_t flags = *(uint8_t *)rrj_at(m, game_state + 4, 1);
    uint32_t target;

    FUNCTION_MARKER(0x80061F6C, "RASHCDG.BIN");
    if (flags & 0x10u)
        target = rrj_read32(m, 0x8005B3A0u) + 1096 * (player_index ^ 1u);
    else if (flags & 1u)
        target = rrj_read32(m, 0x8005B3A0u) + 1096 * *(uint8_t *)rrj_at(m, game_state + 6, 1);
    else
        target = rrj_read32(m, 0x8005AD0Cu + 4 * player_index);
    if (target)
    {
        uint32_t difference = (rrj_read32(m, target + 324) - rrj_read32(m, actor + 324)) << 4;
        uint32_t arrow;
        int32_t distance;

        if (sub_80095410(m, actor) && rrj_s32(rrj_read32(m, actor + 364)) > 0)
            arrow = rrj_s32(difference) > 0 ? 31 : 32;
        else
            arrow = rrj_s32(difference) > 0 ? 32 : 31;
        if (rrj_s32(difference) < 0)
            difference = 0u - difference;
        distance = (int32_t)sub_8001FC90((int32_t)difference, 40);
        if (distance <= 0x9FFFF && rrj_read32(m, state + 16) != (uint32_t)distance)
        {
            uint32_t whole = (uint32_t)distance >> 16;
            uint32_t remainder = (uint32_t)distance - (whole << 16);
            uint32_t tenth = (10 * remainder) >> 16;
            uint32_t hundredth = (100 * (remainder - 6553 * tenth)) >> 16;

            rrj_write32(m, state + 16, (uint32_t)distance);
            if (hundredth >= 10)
                hundredth = 0;
            frontier_initialize_template_packet(m, packets + 3420, 0x800D45D0u + 16 * arrow);
            frontier_initialize_template_packet(m, packets + 3564, 0x800D4920u);
            frontier_initialize_template_packet(m, packets + 3456, 0x800D4710u + 16 * whole);
            frontier_initialize_template_packet(m, packets + 3492, 0x800D4710u + 16 * tenth);
            frontier_initialize_template_packet(m, packets + 3528, 0x800D4710u + 16 * hundredth);
        }
        (void)frontier_link_packet_range(m, ordering_slot, packets + 3564, packets + 3420);
        return rrj_read32(m, ordering_slot);
    }
    return target;
}

uint32_t sub_80062368(RRJMemory *m, uint32_t ordering_slot, uint32_t packets, uint32_t state, uint32_t actor, uint32_t animation)
{
    uint32_t actor_id = rrj_u16(rrj_at(m, actor + 172, 2));
    int32_t current = (int32_t)((rrj_read32(m, actor + 564) >> 9) & 1u) + *(int8_t *)rrj_at(m, actor + 848, 1);
    int32_t capacity = *(uint8_t *)rrj_at(m, 0x800D81D8u + 36 * actor_id + 20, 1);
    int32_t base = *(uint8_t *)rrj_at(m, rrj_read32(m, actor + 556) + 445, 1) + *(int8_t *)rrj_at(m, 0x800D80F5u, 1);
    int32_t total;

    FUNCTION_MARKER(0x80062368, "RASHCDG.BIN");
    if (current < capacity)
        capacity = current;
    total = base + capacity;
    if (total <= 0)
        return total < 9;
    if (total >= 9)
        total = 8;
    if (rrj_read32(m, state + 4) != (uint32_t)current)
    {
        int32_t index;

        rrj_write32(m, state + 4, (uint32_t)current);
        for (index = 0; index < total; ++index)
        {
            uint32_t icon = index < total - current ? 38 : 39;

            frontier_initialize_template_packet(m, packets + 3024 + 36 * (uint32_t)index, 0x800D45D0u + 16 * icon);
        }
    }
    if (rrj_read32(m, actor + 564) & 0x200u)
    {
        int32_t slot = *(uint8_t *)rrj_at(m, rrj_read32(m, actor + 556) + 445, 1);
        int32_t limit = *(uint8_t *)rrj_at(m, 0x800D81D8u + 36 * actor_id + 20, 1);

        if (limit < current)
            slot -= current - limit;
        (void)sub_80013E64(m, animation + 32);
        frontier_initialize_template_packet(m, packets + 3024 + 36 * (uint32_t)slot, 0x800D45D0u + 16 * (38 + (rrj_read32(m, animation + 48) & 1u)));
    }
    else if (rrj_read32(m, animation + 48))
    {
        rrj_write32(m, state + 4, UINT32_MAX);
        rrj_write32(m, animation + 48, 0);
    }
    (void)frontier_link_packet_range(m, ordering_slot, packets + 36 * (uint32_t)(total + 83), packets + 3024);
    return rrj_read32(m, ordering_slot);
}

static uint32_t frontier_allocate_gpu_packet(RRJMemory *m, uint32_t bytes)
{
    uint32_t context = rrj_read32(m, 0x8005B470u);
    uint32_t packet = rrj_read32(m, context + 268);

    if (packet + bytes >= rrj_read32(m, 0x8005B4D0u))
        packet = sub_80021C98(m, packet, bytes);
    rrj_write32(m, context + 268, packet + bytes);
    return packet;
}

static uint32_t frontier_emit_panel_quad(RRJMemory *m, uint32_t ordering_slot, int32_t first_x, int32_t first_y, int32_t second_x, int32_t second_y)
{
    uint32_t packet = frontier_allocate_gpu_packet(m, 28);
    uint32_t old = rrj_read32(m, ordering_slot);

    rrj_write32(m, packet, old | 0x06000000u);
    rrj_write32(m, packet + 4, 0x4C808080u);
    rrj_write32(m, packet + 8, (uint16_t)first_x | ((uint32_t)(uint16_t)first_y << 16));
    rrj_write32(m, packet + 12, (uint16_t)second_x | ((uint32_t)(uint16_t)first_y << 16));
    rrj_write32(m, packet + 16, (uint16_t)second_x | ((uint32_t)(uint16_t)second_y << 16));
    rrj_write32(m, packet + 20, (uint16_t)first_x | ((uint32_t)(uint16_t)second_y << 16));
    rrj_write32(m, packet + 24, 0x55555555u);
    rrj_write32(m, ordering_slot, packet & 0xFFFFFFu);
    return packet;
}

uint32_t sub_80062618(RRJMemory *m, uint32_t ordering_slot, uint32_t packets, uint32_t suppressed, uint32_t game_state)
{
    uint32_t result = *(uint8_t *)rrj_at(m, game_state + 4, 1) & 0x10u;

    FUNCTION_MARKER(0x80062618, "RASHCDG.BIN");
    if (result && rrj_read32(m, 0x8005AD14u) == 2)
    {
        int32_t value = (int32_t)*(uint8_t *)rrj_at(m, game_state + 7, 1) - rrj_s32(rrj_read32(m, 0x8005AD44u));
        uint32_t tens = 0;
        uint32_t digit;

        (void)frontier_link_race_packet(m, ordering_slot, packets + 72);
        if (value >= 10)
        {
            value -= 10;
            tens = 1;
        }
        digit = (uint32_t)value;
        if (tens)
            frontier_initialize_template_packet(m, packets + 216, 0x800D4710u + 16 * tens);
        frontier_initialize_template_packet(m, packets + 252, 0x800D4710u + 16 * digit);
        (void)frontier_link_packet_range(m, ordering_slot, packets + 252, tens ? packets + 216 : packets + 252);
        return rrj_read32(m, ordering_slot);
    }
    if (suppressed)
        return result;
    {
        uint32_t count = *(uint8_t *)rrj_at(m, game_state + 7, 1);
        int32_t width = rrj_u16(rrj_at(m, packets + 136, 2));
        int32_t first_x = rrj_u16(rrj_at(m, packets + 132, 2)) - (int32_t)(count >> 1) * width;
        int32_t second_x = rrj_u16(rrj_at(m, packets + 96, 2)) - (int32_t)(count >> 1) * width;
        int32_t last_x = first_x + width * (int32_t)count - 1;
        int32_t first_y = rrj_u16(rrj_at(m, packets + 134, 2));
        int32_t second_y = rrj_u16(rrj_at(m, packets + 98, 2));
        uint32_t source = packets + 72;
        uint32_t index;

        for (index = 0; index < count; ++index)
        {
            if (index < count - rrj_read32(m, 0x8005AD44u))
            {
                uint32_t packet = frontier_allocate_gpu_packet(m, 20);
                uint32_t mode = (int16_t)rrj_u16(rrj_at(m, source + 34, 2)) ? 0x64000000u : 0x65000000u;

                rrj_write32(m, packet, rrj_read32(m, ordering_slot) | 0x04000000u);
                rrj_write32(m, packet + 4, ((uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, source + 34, 2)) << 25) | mode);
                rrj_write32(m, packet + 8, (uint16_t)second_x | ((uint32_t)(uint16_t)second_y << 16));
                rrj_write32(m, packet + 12, *(uint8_t *)rrj_at(m, source + 12, 1) | ((uint32_t)*(uint8_t *)rrj_at(m, source + 13, 1) << 8) | ((uint32_t)rrj_u16(rrj_at(m, source + 14, 2)) << 16));
                rrj_write32(m, packet + 16, (uint16_t)(int16_t)rrj_u16(rrj_at(m, source + 16, 2)) | ((uint32_t)(uint16_t)(int16_t)rrj_u16(rrj_at(m, source + 18, 2)) << 16));
                rrj_write32(m, ordering_slot, packet & 0xFFFFFFu);
            }
            {
                uint32_t packet = frontier_allocate_gpu_packet(m, 32);
                int32_t end_y = first_y + rrj_u16(rrj_at(m, packets + 138, 2)) - 1;

                rrj_write32(m, packet, rrj_read32(m, ordering_slot) | 0x07000000u);
                rrj_write32(m, packet + 4, 0x4C808080u);
                rrj_write32(m, packet + 8, (uint16_t)first_x | ((uint32_t)(uint16_t)first_y << 16));
                rrj_write32(m, packet + 12, (uint16_t)(first_x + width - 1) | ((uint32_t)(uint16_t)first_y << 16));
                rrj_write32(m, packet + 16, (uint16_t)(first_x + width - 1) | ((uint32_t)(uint16_t)end_y << 16));
                rrj_write32(m, packet + 20, (uint16_t)first_x | ((uint32_t)(uint16_t)end_y << 16));
                rrj_write32(m, packet + 24, (uint16_t)first_x | ((uint32_t)(uint16_t)first_y << 16));
                rrj_write32(m, packet + 28, 0x55555555u);
                rrj_write32(m, ordering_slot, packet & 0xFFFFFFu);
            }
            first_x += width;
            second_x += width;
        }
        {
            int32_t edge = rrj_u16(rrj_at(m, packets + 1860, 2)) + rrj_u16(rrj_at(m, packets + 1864, 2));
            int32_t y = rrj_u16(rrj_at(m, packets + 1862, 2));

            (void)frontier_emit_panel_quad(m, ordering_slot, edge - 3, y + 1, rrj_u16(rrj_at(m, packets + 132, 2)) - (int32_t)(count >> 1) * width, y + 4);
        }
        {
            int32_t edge = rrj_u16(rrj_at(m, packets + 1824, 2));
            int32_t y = rrj_u16(rrj_at(m, packets + 1862, 2));

            result = frontier_emit_panel_quad(m, ordering_slot, edge + 2, y + 1, last_x, y + 4) & 0xFFFFFFu;
        }
    }
    rrj_write32(m, ordering_slot, result);
    return result;
}

uint32_t sub_80062610(RRJMemory *m, uint32_t ordering_slot, uint32_t packets, uint32_t unused, uint32_t suppressed)
{
    FUNCTION_MARKER(0x80062610, "RASHCDG.BIN");
    return sub_80062618(m, ordering_slot, packets, suppressed, rrj_read32(m, 0x8005B2F8u));
}

uint32_t sub_80062C40(RRJMemory *m, uint32_t ordering_slot, uint32_t packets, uint32_t state, uint32_t cache, uint32_t actor)
{
    uint32_t context = rrj_read32(m, 0x8005B2F8u);
    uint32_t active = 0;
    uint32_t result;

    FUNCTION_MARKER(0x80062C40, "RASHCDG.BIN");
    if (*(int8_t *)rrj_at(m, context, 1) == 1 && (*(uint8_t *)rrj_at(m, rrj_read32(m, actor + 1084), 1) & 0x40u) == 0)
    {
        int32_t elapsed = rrj_s32(rrj_read32(m, 0x8005ACC8u) - (rrj_read32(m, context + 16) - rrj_read32(m, 0x8005ACD0u)));
        int32_t value = (int32_t)(((int64_t)elapsed * 256) / 300);

        if (value < 0)
            value = 0;
        if (rrj_read32(m, cache + 68) != (uint32_t)value)
        {
            rrj_write32(m, cache + 68, (uint32_t)value);
            (void)sub_80013B90(m, value + 12, packets + 1908, 0x800D4620u);
        }
        if ((value >> 8) < 31)
        {
            active = 1;
            (void)sub_80013E64(m, state + 64);
        }
    }
    if (active)
    {
        result = rrj_read32(m, state + 80);
        if (result != 0)
            return result;
    }
    {
        int32_t index = *(int8_t *)rrj_at(m, packets + 2156, 1);
        uint32_t color = rrj_read32(m, 0x800CC68Cu + 4 * (uint32_t)index);

        rrj_write32(m, packets + 2124, (color << 24) | rrj_read32(m, ordering_slot));
        result = (packets + 1908) & 0xFFFFFFu;
        rrj_write32(m, ordering_slot, result);
    }
    return result;
}

uint32_t sub_80062D9C(RRJMemory *m, uint32_t packets, uint32_t state, uint32_t actor, uint32_t player_index)
{
    uint32_t resource = rrj_read32(m, actor + 1084);
    uint32_t result;
    int32_t string_id = -1;

    FUNCTION_MARKER(0x80062D9C, "RASHCDG.BIN");
    (void)sub_80013E64(m, state + 64);
    result = rrj_read32(m, state + 80);
    if (result == 0)
        return result;
    if (*(uint8_t *)rrj_at(m, resource + 39, 1) == 254)
        string_id = 28;
    else if (*(uint8_t *)rrj_at(m, resource + 39, 1) == 255)
        string_id = 29;
    if (string_id != -1)
    {
        int32_t width = rrj_s32(rrj_read32(m, 0x800D6120u + 4 * (uint32_t)(string_id - 15))) >> 1;
        int16_t x = (int16_t)(rrj_u16(rrj_at(m, packets + 3696, 2)) - width);
        int16_t y = (int16_t)rrj_u16(rrj_at(m, packets + 3698, 2));

        sub_8002CD78(m, rrj_read32(m, 0x8005AD2Cu), (uint32_t)string_id, (uint32_t)(int32_t)x, (uint32_t)(int32_t)y, rrj_read32(m, 0x8005B590u + 4 * player_index), 0x70u);
        return 0;
    }
    string_id = rrj_s32(rrj_read32(m, 0x8005B220u)) > 0 ? 26 : 27;
    if (string_id == 27)
    {
        result = rrj_read32(m, resource + 40);
        if (result != 0)
            return result;
    }
    {
        uint32_t link = rrj_read32(m, 0x8005B590u + 4 * player_index);

        sub_8002CD78(m, rrj_read32(m, 0x8005AD2Cu), (uint32_t)string_id, (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, packets + 3624, 2)), (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, packets + 3626, 2)), link, 0x00808080u);
        sub_8002CD78(m, rrj_read32(m, 0x8005AD2Cu), (uint32_t)string_id, (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, packets + 3660, 2)), (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, packets + 3662, 2)), link, 0x00808080u);
    }
    return 0;
}

uint32_t sub_8004CE44(RRJMemory *m, uint32_t packet, uint32_t dither, uint32_t draw_to_display, uint32_t page, uint32_t texture_window)
{
    uint32_t mode = 0xE1000000u | (page & 0x9FFu);

    FUNCTION_MARKER(0x8004CE44, "SLUS_010.53");
    *(uint8_t *)rrj_at(m, packet + 3, 1) = 2;
    if (draw_to_display)
        mode |= 0x200u;
    if (dither)
        mode |= 0x400u;
    rrj_write32(m, packet + 4, mode);
    if (texture_window)
    {
        uint32_t command = 0xE2000000u | ((uint32_t)(*(uint8_t *)rrj_at(m, texture_window + 2, 1) >> 3) << 15) | ((uint32_t)(*(uint8_t *)rrj_at(m, texture_window, 1) >> 3) << 10) | (((uint32_t)(0 - (int16_t)rrj_u16(rrj_at(m, texture_window + 6, 2))) << 2) & 0x3E0u) | (((uint32_t)(0 - (int16_t)rrj_u16(rrj_at(m, texture_window + 4, 2))) & 0xFFu) >> 3);

        rrj_write32(m, packet + 8, command);
        return command;
    }
    rrj_write32(m, packet + 8, 0);
    return mode;
}

uint32_t sub_80062F34(RRJMemory *m, uint32_t packets, uint32_t actor, uint32_t state, uint32_t player_index)
{
    uint32_t resource = rrj_read32(m, actor + 1084);
    uint32_t result = *(uint8_t *)rrj_at(m, resource + 1, 1) & 0xFu;
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
    flags = rrj_read32(m, 0x8005AD48u);
    result = flags & 0x1Eu;
    if (result == 0)
        return result;
    (void)sub_80013E64(m, state + 96);
    result = rrj_read32(m, state + 112);
    if (result == 0)
        return result;
    switch (*(uint8_t *)rrj_at(m, resource + 39, 1))
    {
        case 252:
        case 248:
            string_id = *(uint8_t *)rrj_at(m, rrj_read32(m, 0x8005B2F8u) + 4, 1) == 33 ? 24 : 18;
            break;
        case 253:
            string_id = 20;
            break;
        default:
            string_id = flags & 8u ? 15 : 22;
            break;
    }
    table = rrj_read32(m, 0x8005B544u);
    if (table == 0)
        return table;
    anchor = packets + 3708;
    center_x = rrj_u16(rrj_at(m, anchor + 24, 2));
    center_y = (int16_t)rrj_u16(rrj_at(m, anchor + 26, 2));
    first_width = rrj_s32(rrj_read32(m, 0x800D6120u + 4 * (uint32_t)(string_id - 15)));
    second_width = rrj_s32(rrj_read32(m, 0x800D6120u + 4 * (uint32_t)(string_id - 14)));
    ordering_slot = rrj_read32(m, 0x8005B590u + 4 * player_index);
    height = (int16_t)rrj_u16(rrj_at(m, 0x800D808Cu + 24 * rrj_read32(m, 0x8005AD2Cu), 2));
    sub_8002CD78(m, rrj_read32(m, 0x8005AD2Cu), (uint32_t)string_id, (uint32_t)(int32_t)(int16_t)(center_x - (first_width >> 1)), (uint32_t)center_y, ordering_slot, 0x00808080u);
    sub_8002CD78(m, rrj_read32(m, 0x8005AD2Cu), (uint32_t)(string_id + 1), (uint32_t)(int32_t)(int16_t)(center_x - (second_width >> 1)), (uint32_t)(int32_t)(int16_t)(center_y + 2 * height), ordering_slot, 0x00808080u);
    if (string_id == 15)
    {
        int32_t third_width = rrj_s32(rrj_read32(m, 0x800D6128u));

        sub_8002CD78(m, rrj_read32(m, 0x8005AD2Cu), 17, (uint32_t)(int32_t)(int16_t)(center_x - (third_width >> 1)), (uint32_t)(int32_t)(int16_t)(center_y + 3 * height), ordering_slot, 0x00808080u);
    }
    rrj_write32(m, 0x800CC674u, rrj_read32(m, ordering_slot) | 0x05000000u);
    rrj_write32(m, ordering_slot, 0x00CC674u);
    maximum_width = first_width < second_width ? second_width : first_width;
    left = center_x - (maximum_width >> 1);
    top = center_y - height;
    bottom = top + (string_id == 15 ? 6 : 5) * height;
    *(uint16_t *)rrj_at(m, 0x800CC67Cu, 2) = (uint16_t)left;
    *(uint16_t *)rrj_at(m, 0x800CC67Eu, 2) = (uint16_t)top;
    *(uint16_t *)rrj_at(m, 0x800CC680u, 2) = (uint16_t)(left + maximum_width);
    *(uint16_t *)rrj_at(m, 0x800CC682u, 2) = (uint16_t)top;
    *(uint16_t *)rrj_at(m, 0x800CC684u, 2) = (uint16_t)left;
    *(uint16_t *)rrj_at(m, 0x800CC686u, 2) = (uint16_t)bottom;
    *(uint16_t *)rrj_at(m, 0x800CC688u, 2) = (uint16_t)(left + maximum_width);
    *(uint16_t *)rrj_at(m, 0x800CC68Au, 2) = (uint16_t)bottom;
    (void)sub_8004CE44(m, 0x800CCD50u, 1, 1, 15, 0);
    rrj_write32(m, 0x800CCD50u, rrj_read32(m, ordering_slot) | 0x02000000u);
    rrj_write32(m, ordering_slot, 0x00CCD50u);
    return ordering_slot;
}

uint32_t sub_80063530(RRJMemory *m, uint32_t ordering_slot, uint32_t packets, uint32_t cache, uint32_t state, uint32_t actor, uint32_t countdown, uint32_t suppressed)
{
    uint32_t resource = rrj_read32(m, actor + 1084);
    uint32_t context = rrj_read32(m, 0x8005B2F8u);
    uint32_t result = rrj_read32(m, resource + 40);
    uint32_t active = 0;

    FUNCTION_MARKER(0x80063530, "RASHCDG.BIN");
    if (result != 0)
        return result;
    result = 1;
    if (*(int8_t *)rrj_at(m, context, 1) == 1)
    {
        result = *(uint8_t *)rrj_at(m, resource, 1) & 0x40u;
        if (result == 0)
        {
            int32_t elapsed;
            int32_t value;

            if (countdown)
            {
                elapsed = rrj_s32(rrj_read32(m, 0x8005ACC8u) - rrj_read32(m, context + 16));
                value = (int32_t)(((int64_t)elapsed * 256) / 300);
                if (value < 0)
                    value = 0;
                if ((value >> 8) < 31 && value > 0)
                {
                    active = 1;
                    (void)sub_80013E64(m, state + 64);
                }
            }
            else
            {
                value = (int32_t)(((int64_t)rrj_s32(rrj_read32(m, context + 16)) * 256) / 300);
            }
            result = rrj_read32(m, cache + 68);
            if ((uint32_t)value != result)
            {
                rrj_write32(m, cache + 68, (uint32_t)value);
                result = sub_80013B90(m, value + 12, packets + 1908, 0x800D4620u);
            }
        }
    }
    if (!suppressed)
    {
        result = rrj_read32(m, state + 140);
        if (rrj_s32(result) <= 0)
        {
            if (active)
            {
                result = rrj_read32(m, state + 80);
                if (result == 0)
                    return result;
            }
            {
                int32_t index = *(int8_t *)rrj_at(m, packets + 2156, 1);
                uint32_t color = rrj_read32(m, 0x800CC68Cu + 4 * (uint32_t)index);

                rrj_write32(m, packets + 2124, (color << 24) | rrj_read32(m, ordering_slot));
                result = (packets + 1908) & 0xFFFFFFu;
                rrj_write32(m, ordering_slot, result);
            }
        }
    }
    return result;
}

uint32_t sub_800636F0(RRJMemory *m, uint32_t ordering_slot, uint32_t packets, uint32_t state, uint32_t actor, uint32_t suppressed)
{
    uint32_t checkpoint = rrj_read32(m, 0x8005ACD4u);
    uint32_t result;

    FUNCTION_MARKER(0x800636F0, "RASHCDG.BIN");
    if (!suppressed && rrj_s32(rrj_read32(m, state + 140)) <= 0)
    {
        int32_t index = *(int8_t *)rrj_at(m, packets + 2408, 1);
        uint32_t color = rrj_read32(m, 0x800CC68Cu + 4 * (uint32_t)index);

        rrj_write32(m, packets + 2376, (color << 24) | rrj_read32(m, ordering_slot));
        rrj_write32(m, ordering_slot, (packets + 2160) & 0xFFFFFFu);
    }
    if (rrj_s32(checkpoint) < 3)
    {
        uint32_t marker = 0x800D9C40u + 4 * checkpoint;
        int32_t threshold = rrj_s32(rrj_read32(m, 0x800D5F60u + 4 * checkpoint)) << 12;

        if (rrj_read32(m, marker) == 0 && rrj_s32(rrj_read32(m, actor + 324)) < threshold)
        {
            uint32_t context = rrj_read32(m, 0x8005B2F8u);
            int32_t current_time = rrj_s32(rrj_read32(m, context + 16));
            int32_t course = rrj_s32(rrj_read32(m, context + 64)) - 56;
            int32_t target_time = rrj_s32(rrj_read32(m, 0x80053A88u + 176 * (uint32_t)course + 4 * checkpoint));

            rrj_write32(m, state + 180, (uint32_t)current_time);
            rrj_write32(m, marker, (uint32_t)current_time);
            (void)sub_80013B90(m, (int32_t)(((int64_t)current_time * 256) / 300) + 12, packets + 2412, 0x800D4620u);
            (void)sub_80013B90(m, (int32_t)(((int64_t)target_time * 256) / 300), packets + 2664, 0x800D4620u);
            checkpoint++;
            rrj_write32(m, 0x8005ACD4u, checkpoint);
        }
    }
    result = rrj_read32(m, state + 180);
    if (result)
    {
        result = sub_80013E64(m, state + 160);
        if (!suppressed)
        {
            result = rrj_read32(m, state + 140);
            if (rrj_s32(result) <= 0)
            {
                result = rrj_read32(m, state + 176);
                if (result)
                {
                    int32_t first_index = *(int8_t *)rrj_at(m, packets + 2660, 1);
                    int32_t second_index = *(int8_t *)rrj_at(m, packets + 2912, 1);
                    uint32_t first = (packets + 2412) & 0xFFFFFFu;

                    rrj_write32(m, packets + 2628, (rrj_read32(m, 0x800CC68Cu + 4 * (uint32_t)first_index) << 24) | rrj_read32(m, ordering_slot));
                    rrj_write32(m, ordering_slot, first);
                    rrj_write32(m, packets + 2880, (rrj_read32(m, 0x800CC68Cu + 4 * (uint32_t)second_index) << 24) | first);
                    result = (packets + 2664) & 0xFFFFFFu;
                    rrj_write32(m, ordering_slot, result);
                }
            }
        }
    }
    return result;
}

static uint32_t frontier_emit_line(RRJMemory *m, uint32_t ordering_slot, int32_t first_x, int32_t first_y, int32_t second_x, int32_t second_y)
{
    uint32_t packet = frontier_allocate_gpu_packet(m, 16);

    rrj_write32(m, packet, rrj_read32(m, ordering_slot) | 0x03000000u);
    rrj_write32(m, packet + 4, 0x40808080u);
    rrj_write32(m, packet + 8, (uint16_t)first_x | ((uint32_t)(uint16_t)first_y << 16));
    rrj_write32(m, packet + 12, (uint16_t)second_x | ((uint32_t)(uint16_t)second_y << 16));
    rrj_write32(m, ordering_slot, packet & 0xFFFFFFu);
    return packet & 0xFFFFFFu;
}

static uint32_t frontier_link_count_digits(RRJMemory *m, uint32_t ordering_slot, uint32_t packets, int32_t count)
{
    uint32_t tens;
    uint32_t digit;

    (void)frontier_link_race_packet(m, ordering_slot, packets + 180);
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
        frontier_initialize_template_packet(m, packets + 216, 0x800D4710u + 16 * tens);
    frontier_initialize_template_packet(m, packets + 252, 0x800D4710u + 16 * digit);
    (void)frontier_link_packet_range(m, ordering_slot, packets + 252, tens ? packets + 216 : packets + 252);
    return rrj_read32(m, ordering_slot);
}

uint32_t sub_8005F030(RRJMemory *m, uint32_t ordering_slot, uint32_t packets, uint32_t state, uint32_t actor, uint32_t player_index, uint32_t suppressed)
{
    uint32_t context = rrj_read32(m, 0x8005B2F8u);
    uint32_t descriptor = rrj_read32(m, actor + 1084);
    uint32_t related = 0;
    uint32_t result;
    int32_t count;

    FUNCTION_MARKER(0x8005F030, "RASHCDG.BIN");
    if (rrj_read32(m, actor + 856) && rrj_read32(m, actor + 1088))
        related = 0x800D8238u + 36 * rrj_u16(rrj_at(m, actor + 172, 2));
    count = *(uint8_t *)rrj_at(m, 0x800D81F1u + 36 * player_index, 1);
    if (related)
        count += *(uint8_t *)rrj_at(m, related + 1, 1);
    result = *(uint8_t *)rrj_at(m, context + 4, 1) & 0x10u;
    if (result)
    {
        result = 2;
        if (rrj_read32(m, 0x8005AD14u) == 2)
            return frontier_link_count_digits(m, ordering_slot, packets, count);
    }
    if (suppressed)
        return result;
    result = rrj_read32(m, state + 140);
    if (rrj_s32(result) > 0 || count <= 0)
        return result;
    if (count >= 6)
        return frontier_link_count_digits(m, ordering_slot, packets, count);
    {
        int32_t width = rrj_u16(rrj_at(m, packets + 136, 2));
        int32_t half_width = (count >> 1) * width;
        int32_t sprite_x = rrj_u16(rrj_at(m, packets + 204, 2)) - half_width;
        int32_t left = rrj_u16(rrj_at(m, packets + 132, 2)) - half_width;
        int32_t right = left + width * count - 1;
        int32_t sprite_y = rrj_u16(rrj_at(m, packets + 206, 2));
        uint32_t source = packets + 180;
        int32_t index;

        for (index = 0; index < count; ++index)
        {
            uint32_t packet = frontier_allocate_gpu_packet(m, 20);
            int16_t mode_value = (int16_t)rrj_u16(rrj_at(m, source + 34, 2));
            uint32_t mode = mode_value ? 0x64000000u : 0x65000000u;

            rrj_write32(m, packet, rrj_read32(m, ordering_slot) | 0x04000000u);
            rrj_write32(m, packet + 4, ((uint32_t)(int32_t)mode_value << 25) | mode);
            rrj_write32(m, packet + 8, (uint16_t)sprite_x | ((uint32_t)(uint16_t)sprite_y << 16));
            rrj_write32(m, packet + 12, *(uint8_t *)rrj_at(m, source + 12, 1) | ((uint32_t)*(uint8_t *)rrj_at(m, source + 13, 1) << 8) | ((uint32_t)rrj_u16(rrj_at(m, source + 14, 2)) << 16));
            rrj_write32(m, packet + 16, (uint16_t)(int16_t)rrj_u16(rrj_at(m, source + 16, 2)) | ((uint32_t)(uint16_t)(int16_t)rrj_u16(rrj_at(m, source + 18, 2)) << 16));
            rrj_write32(m, ordering_slot, packet & 0xFFFFFFu);
            sprite_x += width;
        }
        {
            int32_t edge = rrj_u16(rrj_at(m, packets + 348, 2)) + rrj_u16(rrj_at(m, packets + 352, 2));
            int32_t y = rrj_u16(rrj_at(m, packets + 350, 2));
            uint32_t reference = packets + (*(uint8_t *)rrj_at(m, context + 4, 1) == 33 ? 1800 : 288);
            int32_t reference_x = rrj_u16(rrj_at(m, reference + 24, 2));

            (void)frontier_emit_line(m, ordering_slot, edge - 3, y + 1, left, y + 1);
            (void)frontier_emit_line(m, ordering_slot, left, y + 4, edge - 1, y + 4);
            (void)frontier_emit_line(m, ordering_slot, reference_x + 2, y + 1, right, y + 1);
            result = frontier_emit_line(m, ordering_slot, right, y + 4, reference_x, y + 4);
        }
    }
    return result;
}

uint32_t sub_8005FB4C(RRJMemory *m, uint32_t ordering_slot, uint32_t packets, uint32_t state, uint32_t actor, uint32_t input_flags, uint32_t animation, uint32_t player_index)
{
    uint32_t context = rrj_read32(m, 0x8005B2F8u);
    uint32_t descriptor = rrj_read32(m, actor + 1084);
    uint32_t status;
    uint32_t result;

    FUNCTION_MARKER(0x8005FB4C, "RASHCDG.BIN");
    if (*(int8_t *)rrj_at(m, context, 1) == 1 && (*(uint8_t *)rrj_at(m, descriptor, 1) & 0x40u) == 0)
    {
        status = 0;
        rrj_write32(m, 0, status);
        if (rrj_read32(m, rrj_read32(m, actor + 852) + 604) < 2 && rrj_read32(m, 0x8005B220u) == 0 && rrj_read32(m, 0x8005B288u + 4 * player_index) == 0)
        {
            if (input_flags & 1u)
                status = 4;
            else if (input_flags & 0x10u)
                status = 3;
            else if (input_flags & 0x40u)
                status = 1;
            rrj_write32(m, 0, status);
        }
        if (status != rrj_read32(m, state + 60))
        {
            rrj_write32(m, state + 60, status);
            rrj_write32(m, animation, 0);
            rrj_write32(m, animation + 16, 0);
            rrj_write32(m, animation + 12, 0);
            if (status == 1)
                frontier_initialize_template_packet(m, packets + 2952, 0x800D45F0u);
            else if (status == 3)
                frontier_initialize_template_packet(m, packets + 2988, 0x800D4600u);
            else if (status == 4)
            {
                frontier_initialize_template_packet(m, packets + 2924, 0x800D4940u);
                rrj_write32(m, animation + 12, 0xFFFFFFF8u);
            }
        }
    }
    status = rrj_read32(m, state + 60);
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
        (void)sub_80013E64(m, animation);
        if (rrj_s32(rrj_read32(m, animation + 12)) < 0)
            visible = packet_index != 81;
        result = rrj_read32(m, animation + 16);
        if (result && visible)
            result = frontier_link_race_packet(m, ordering_slot, packets + 36 * packet_index);
    }
    return result;
}

uint32_t sub_8003B9A8(RRJMemory *m, int32_t owner, uint32_t kind)
{
    uint32_t manager;
    int32_t count;
    int32_t index;
    uint32_t record;

    FUNCTION_MARKER(0x8003B9A8, "SLUS_010.53");
    if (owner < 0)
        return 0;
    manager = rrj_read32(m, 0x8005B240u);
    count = (int16_t)rrj_u16(rrj_at(m, manager + 40, 2));
    if (count <= 0 || (int16_t)rrj_u16(rrj_at(m, manager + 44, 2)) <= 0)
        return 0;
    index = (int16_t)rrj_u16(rrj_at(m, manager + 42, 2));
    record = rrj_read32(m, manager + 28) + 32 * (uint32_t)index;
    while (index < count)
    {
        if ((int16_t)rrj_u16(rrj_at(m, record + 4, 2)) == 1 && rrj_read32(m, record + 16) == (uint32_t)owner && rrj_read32(m, record + 28) == kind)
            return record;
        ++index;
        record += 32;
    }
    return 0;
}

static int32_t frontier_collect_connected_roads(RRJMemory *m, int32_t owner, int32_t id, uint32_t *output, int32_t limit)
{
    uint32_t candidates[4];
    uint32_t segment;
    uint32_t links;
    uint32_t metadata;
    int32_t count;
    int32_t index;

    if (owner < 0 || id < 0)
        return 0;
    segment = sub_8003B9A8(m, owner, 0);
    if (segment == 0)
        return 0;
    links = rrj_read32(m, segment + 12);
    if (links == 0)
        return 0;
    metadata = sub_80039B60(m, rrj_read32(m, links));
    if (metadata == 0)
        return 0;
    count = (int32_t)rrj_collect_route_links_local(m, metadata, (uint32_t)id, candidates, 4, (uint32_t)limit);
    if (count < 0 || limit < count)
        return 0;
    for (index = 0; index < count; ++index)
        output[index] = (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, candidates[index] + 10, 2));
    return count;
}

uint32_t sub_8003BC48(RRJMemory *m, int32_t owner, int32_t id, uint32_t output, int32_t limit)
{
    uint32_t roads[4];
    int32_t count;
    int32_t index;

    FUNCTION_MARKER(0x8003BC48, "SLUS_010.53");
    count = frontier_collect_connected_roads(m, owner, id, roads, limit);
    for (index = 0; index < count; ++index)
        rrj_write32(m, output + 4 * (uint32_t)index, roads[index]);
    return (uint32_t)count;
}

uint32_t sub_8003C590(RRJMemory *m, uint32_t actor)
{
    uint32_t flags = rrj_u16(rrj_at(m, actor + 362, 2)) == 1 ? 0x80u : 0;

    FUNCTION_MARKER(0x8003C590, "SLUS_010.53");
    if (sub_80095410(m, actor) == 0 && (*(uint8_t *)rrj_at(m, rrj_read32(m, actor + 1084), 1) & 0x80u))
    {
        int32_t dot = rrj_s32(sub_8002E698(m, actor + 450, rrj_read32(m, actor + 340) + 14));
        uint32_t magnitude = ((uint32_t)(dot >> 31) + (uint32_t)dot) ^ (uint32_t)(dot >> 31);

        if (rrj_s32(magnitude) > 0xC000)
            flags |= 1u;
    }
    if ((flags & 1u) == 0 && sub_80095410(m, actor) != 0)
        flags |= 2u;
    if ((flags & 3u) == 0 && rrj_u16(rrj_at(m, actor + 362, 2)) == 0)
    {
        uint32_t nearest;
        uint32_t roads[4];
        uint32_t distance = rrj_nearest_junction_local(m, actor + 360, &nearest, 1);

        if (nearest == UINT32_MAX)
            return flags;
        if (rrj_s32(rrj_read32(m, 0x800531A0u + 4 * rrj_read32(m, rrj_read32(m, 0x8005B2F8u) + 60))) < rrj_s32(distance))
            return flags;
        {
            uint32_t record = sub_8003F408(m, rrj_read32(m, actor + 428), nearest);

            if (record == 0)
                return flags;
            if (frontier_collect_connected_roads(m, (int32_t)nearest, (int32_t)rrj_u16(rrj_at(m, actor + 360, 2)), roads, 3) == 2)
            {
                if (sub_8003F580(m, record, roads[0]))
                    flags |= 0x10u;
                if (sub_8003F580(m, record, roads[1]))
                    flags |= 0x40u;
            }
        }
    }
    return flags;
}

uint32_t sub_8005E850(RRJMemory *m, uint32_t enabled, RRJRaceLeafCall call)
{
    uint32_t context;
    int32_t player_count;
    int32_t player_index;
    uint32_t result = enabled;

    FUNCTION_MARKER(0x8005E850, "RASHCDG.BIN");
    if (!enabled)
        return result;

    rrj_write32(m, 0x8005ACDCu, rrj_read32(m, 0x8005ACDCu) + 1);
    context = rrj_read32(m, 0x8005B2F8u);
    if ((*(uint8_t *)rrj_at(m, context + 4, 1) & 0x10u) != 0 && rrj_read32(m, 0x8005AD14u) != 1)
    {
        uint32_t packet = 0x800523B8u;
        uint32_t viewport = rrj_read32(m, 0x8005B474u);
        uint32_t ordering_slot = rrj_read32(m, 0x8005B590u);
        int32_t first_y = (int16_t)rrj_u16(rrj_at(m, viewport + 6, 2));
        int32_t second_y = (int16_t)rrj_u16(rrj_at(m, viewport + 10, 2));
        int32_t difference = second_y - first_y;
        int32_t sign = difference >> 31;
        int32_t last_y = first_y + ((difference + sign) ^ sign);

        *(uint8_t *)rrj_at(m, packet + 3, 1) = 5;
        *(uint8_t *)rrj_at(m, packet + 7, 1) = 40;
        rrj_put16(rrj_at(m, packet + 8, 2), 0);
        rrj_put16(rrj_at(m, packet + 10, 2), (uint16_t)first_y);
        rrj_put16(rrj_at(m, packet + 12, 2), 384);
        rrj_put16(rrj_at(m, packet + 14, 2), (uint16_t)first_y);
        rrj_put16(rrj_at(m, packet + 16, 2), 0);
        rrj_put16(rrj_at(m, packet + 18, 2), (uint16_t)last_y);
        rrj_put16(rrj_at(m, packet + 20, 2), 384);
        rrj_put16(rrj_at(m, packet + 22, 2), (uint16_t)last_y);
        rrj_write32(m, packet, (rrj_read32(m, packet) & 0xFF000000u) | (rrj_read32(m, ordering_slot) & 0xFFFFFFu));
        rrj_write32(m, ordering_slot, (rrj_read32(m, ordering_slot) & 0xFF000000u) | (packet & 0xFFFFFFu));
    }

    player_count = rrj_s32(rrj_read32(m, context + 48));
    for (player_index = 0; player_index < player_count; ++player_index)
    {
        uint32_t actor = rrj_read32(m, 0x8005B268u + 4 * (uint32_t)player_index);
        uint32_t ordering_slot = rrj_read32(m, 0x8005B590u + 4 * (uint32_t)player_index);
        uint32_t packets = rrj_read32(m, 0x8005ACECu + 4 * (uint32_t)player_index);
        uint32_t display = 0x800D4A60u + 88 * (uint32_t)player_index;
        uint32_t state = 0x800D6198u + 224 * (uint32_t)player_index;
        uint32_t animation = 0x800D6358u + 72 * (uint32_t)player_index;
        uint32_t status_flags = sub_8003C590(m, actor);
        uint32_t mode = *(uint8_t *)rrj_at(m, context + 4, 1);

        if (*(int8_t *)rrj_at(m, context, 1) == 6)
        {
            if (!(mode & 0x10u))
                continue;
            if (rrj_read32(m, 0x8005AD14u) == 2)
                (void)sub_8005FA68(m, ordering_slot, packets, 105, 108);
            (void)sub_8005FA68(m, ordering_slot, packets, 0, 0);
            continue;
        }
        {
            uint32_t descriptor = rrj_read32(m, actor + 1084);
            uint32_t suppressed = (((mode & 1u) != 0 || mode == 44) && (rrj_read32(m, actor + 560) & 0x08000000u) != 0);
            uint32_t skip_main;

            rrj_write32(m, 0x8005B288u + 4 * (uint32_t)player_index, suppressed);
            if ((mode & 1u) != 0 && !rrj_read32(m, 0x8005B220u) && mode != 44 && (*(uint8_t *)rrj_at(m, descriptor + 1, 1) & 0xFu) == 2 && (mode != 33 || *(uint8_t *)rrj_at(m, descriptor + 39, 1) == 248))
            {
                (void)sub_80062F34(m, packets, actor, state, (uint32_t)player_index);
                (void)sub_8005FE58(m, ordering_slot, packets, state, actor);
            }

            skip_main = rrj_read32(m, 0x8005B220u) || suppressed || rrj_read32(m, descriptor + 40);
            if (skip_main)
            {
                (void)sub_80062D9C(m, packets, state, actor, (uint32_t)player_index);
            }
            else
            {
                uint32_t hide_second;

                (void)sub_8005FF84(m, ordering_slot, packets, display, actor, call);
                (void)sub_80062368(m, ordering_slot, packets, display, actor, state);
                if (!(mode & 1u) || mode == 33 || (*(uint8_t *)rrj_at(m, descriptor + 1, 1) & 0xFu) != 2)
                {
                    (void)sub_80060178(m, ordering_slot, packets, display, actor);
                    (void)sub_800603E4(m, ordering_slot, packets, display, (uint32_t)player_index);
                    (void)sub_8005FA68(m, ordering_slot, packets, 9, 9);
                }
                else
                {
                    (void)sub_80062C40(m, ordering_slot, packets, state, display, actor);
                    (void)sub_8005FA68(m, ordering_slot, packets, 51, 51);
                }
                if (!(mode & 4u))
                    (void)sub_80061F6C(m, ordering_slot, packets, display, actor, (uint32_t)player_index);

                if ((mode & 1u) != 0 && (*(uint8_t *)rrj_at(m, descriptor + 1, 1) & 0xFu) == 2)
                {
                    (void)sub_8005FAC4(m, ordering_slot, packets + 3384, (uint32_t)player_index);
                    (void)sub_800C52A0(m, actor, (uint32_t)player_index);
                    (void)sub_8005FA68(m, ordering_slot, packets, 50, 50);
                    (void)sub_8005FA68(m, ordering_slot, packets, 104, 104);
                    if (mode == 33)
                        (void)sub_8005F030(m, ordering_slot, packets, state, actor, (uint32_t)player_index, status_flags);
                    else
                        (void)sub_80062610(m, ordering_slot, packets, (uint32_t)player_index, status_flags);
                }
                else if (((mode & 4u) != 0 && mode != 44) || (mode == 44 && *(uint8_t *)rrj_at(m, context + 57, 1) < 2))
                {
                    uint32_t countdown = mode == 44 || mode == 36;

                    (void)sub_80063530(m, ordering_slot, packets, display, state, actor, countdown, status_flags);
                    if (mode == 4)
                        (void)sub_800636F0(m, ordering_slot, packets, state, actor, status_flags);
                }
                else
                {
                    if (!(mode & 4u))
                        (void)sub_8005F030(m, ordering_slot, packets, state, actor, (uint32_t)player_index, status_flags);
                    (void)sub_800606F0(m, ordering_slot, packets, display, actor, status_flags, (uint32_t)player_index, state);
                    (void)sub_800C52A0(m, actor, (uint32_t)player_index);
                    (void)sub_8005FA68(m, ordering_slot, packets, 8, 8);
                    (void)sub_8005FA68(m, ordering_slot, packets, 104, 104);
                }

                hide_second = sub_8006148C(m, ordering_slot, packets, display, actor, (uint32_t)player_index, animation, state);
                (void)sub_80063408(m, actor, packets, animation, (uint32_t)player_index, hide_second);
                (void)sub_80060C10(m, ordering_slot, packets, display, actor, (uint32_t)player_index, animation, state);
                (void)sub_80061E50(m, packets, (uint32_t)player_index, animation);
                (void)sub_8005FE58(m, ordering_slot, packets, state, actor);
                (void)sub_8005FB4C(m, ordering_slot, packets, display, actor, status_flags, state, (uint32_t)player_index);
            }
        }

        if ((mode & 0x10u) != 0 && rrj_read32(m, 0x8005AD14u) == 2)
            (void)sub_8005FA68(m, ordering_slot, packets, 105, 108);
        (void)sub_8005FA68(m, ordering_slot, packets, 0, 0);
    }

    *(uint8_t *)rrj_at(m, 0x800CCD63u, 1) = 1;
    rrj_write32(m, 0x800CCD64u, 0xE1000600u);
    {
        uint32_t ordering_slot = rrj_read32(m, 0x8005B5A0u);

        result = rrj_read32(m, ordering_slot) | 0x01000000u;
        rrj_write32(m, 0x800CCD60u, result);
        rrj_write32(m, ordering_slot, 0x00CCD60u);
    }
    return result;
}

uint32_t sub_8005E848(RRJMemory *m, RRJRaceLeafCall call)
{
    return sub_8005E850(m, rrj_read32(m, 0x8005ACD8u), call);
}

uint32_t sub_80064B9C(RRJMemory *m, uint32_t player_index, RRJRaceLeafCall call)
{
    uint32_t mode;
    uint32_t result;

    FUNCTION_MARKER(0x80064B9C, "RASHCDG.BIN");
    (void)frontier_call(m, call, 0x8004D154, rrj_read32(m, 0x8005AEC0 + 4 * player_index) + 92, 0, 0);
    (void)frontier_call(m, call, 0x800650D0, player_index, 0, 0);
    mode = rrj_read32(m, rrj_read32(m, 0x8005B2F8) + 48);
    if (mode == 1 && !rrj_read32(m, 0x8005B2D0) && (int16_t)rrj_u16(rrj_at(m, rrj_read32(m, 0x8005B278) + 20, 2)) != -1)
        (void)frontier_call(m, call, 0x800644F4, 0, 0, 0);
    mode = rrj_read32(m, rrj_read32(m, 0x8005B2F8) + 48);
    if ((mode == 2 || rrj_read32(m, 0x8005B2D0)) && rrj_read32(m, 0x8005AD24))
        (void)frontier_call(m, call, 0x80064CC8, player_index, 0, 0);
    if (rrj_read32(m, 0x8005B308))
        (void)frontier_call(m, call, 0x8006396C, player_index, 0, 0);
    result = rrj_read32(m, 0x8005AD28);
    if (result)
        return frontier_call(m, call, 0x80063C5C, player_index, 0, 0);
    return result;
}

uint32_t sub_800650D0(RRJMemory *m, uint32_t player_index, RRJRaceLeafCall call)
{
    uint32_t player = rrj_read32(m, 0x8005AEC0 + 4 * player_index);
    uint32_t angle = ((uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, player + 124, 2)) - 443) & 0xfff;
    uint32_t offset = 110 * angle / 4096;
    uint32_t mode;

    FUNCTION_MARKER(0x800650D0, "RASHCDG.BIN");
    rrj_put16(rrj_at(m, 0x800D5EE8, 2), (uint16_t)offset);
    rrj_put16(rrj_at(m, 0x800D5EEA, 2), (uint16_t)(offset + 24));
    mode = rrj_read32(m, rrj_read32(m, 0x8005B2F8) + 48);
    if (mode == 1 && !rrj_read32(m, 0x8005B2D0))
        return frontier_call(m, call, 0x80065174, 0x800D5EE8, 0, 0);
    return 0x80060000;
}

uint32_t sub_800654B4(RRJMemory *m, int32_t route, int32_t position)
{
    uint32_t manager = rrj_read32(m, 0x8005B278);
    int32_t selected = -1;
    int32_t index;

    FUNCTION_MARKER(0x800654B4, "RASHCDG.BIN");
    if (!(int16_t)rrj_u16(rrj_at(m, manager + 34, 2)))
        return UINT32_MAX;
    index = (int16_t)rrj_u16(rrj_at(m, manager + 18, 2));
    if (index != -1 && rrj_read32(m, manager + 740 + 12 * (uint32_t)index + 8) == rrj_read32(m, rrj_read32(m, 0x8005B508)))
        return (uint32_t)index;
    for (index = 0; index != 20; ++index)
    {
        uint32_t record = manager + 740 + 12 * (uint32_t)index;

        if (rrj_read32(m, record + 8) == rrj_read32(m, rrj_read32(m, 0x8005B508)))
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

            if (!rrj_read32(m, record))
                continue;
            segments = rrj_read32(m, record + 4);
            for (segment = 0; segment != 4; ++segment)
            {
                uint32_t entry = segments + 6 * (uint32_t)segment;

                if ((int16_t)rrj_u16(rrj_at(m, entry, 2)) == route)
                {
                    int32_t total = (int16_t)rrj_u16(rrj_at(m, entry + 2, 2)) + (int16_t)rrj_u16(rrj_at(m, entry + 4, 2));
                    int32_t midpoint = total / 2;
                    uint32_t distance = frontier_abs(rrj_s32((uint32_t)midpoint - (uint32_t)position));

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
        uint32_t source = rrj_read32(m, record) + 12;

        for (index = 1; index < 60; ++index)
        {
            rrj_write32(m, manager + 984 + 4 * (uint32_t)(index - 1), source);
            source += rrj_read32(m, source + 4);
        }
        rrj_write32(m, manager + 4, rrj_read32(m, rrj_read32(m, manager + 984) + 20));
        return (uint32_t)selected;
    }
    return (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, manager + 18, 2));
}

uint32_t sub_800656A8(RRJMemory *m, int32_t point, int32_t first_start, int32_t second_start, int32_t second_end)
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

uint32_t sub_800662BC(RRJMemory *m, RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x800662BC, "RASHCDG.BIN");
    return frontier_call(m, call, 0x800662C4, rrj_read32(m, 0x8005B278), 0, 0);
}

uint32_t sub_80065174(RRJMemory *m, RRJRaceLeafCall call)
{
    uint32_t manager = rrj_read32(m, 0x8005B278);
    uint32_t control = rrj_read32(m, 0x8005AE34);
    uint32_t angle;
    int32_t first;
    int32_t route;
    int32_t position;
    int32_t selected;

    FUNCTION_MARKER(0x80065174, "RASHCDG.BIN");
    if (rrj_read32(m, manager + 24))
        return 0x80060000;
    angle = ((uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, rrj_read32(m, 0x8005AEC0) + 124, 2)) - 443) & 0xfff;
    first = (int32_t)(110 * angle / 4096);
    rrj_put16(rrj_at(m, manager + 48, 2), (uint16_t)first);
    rrj_put16(rrj_at(m, manager + 50, 2), (uint16_t)(first + 24 >= 110 ? first - 86 : first + 24));
    route = rrj_s32(rrj_read32(m, control + 72));
    position = rrj_s32(rrj_read32(m, control + 68) << 6);
    selected = (int16_t)sub_800654B4(m, route, position);
    rrj_put16(rrj_at(m, manager + 16, 2), (uint16_t)selected);
    if (selected == -1)
    {
        rrj_put16(rrj_at(m, manager + 18, 2), 0xffff);
        return UINT32_MAX;
    }
    rrj_write32(m, manager + 8, (uint32_t)route);
    rrj_write32(m, manager + 12, (uint32_t)position);
    if (selected != (int16_t)rrj_u16(rrj_at(m, manager + 18, 2)))
    {
        uint32_t packed = rrj_read32(m, manager + 988) + 8;
        uint32_t twos = 0;
        uint32_t width = 0;
        uint32_t column;

        rrj_put16(rrj_at(m, manager + 18, 2), (uint16_t)selected);
        for (column = 0; column != 110; ++column)
        {
            uint32_t group = rrj_read32(m, manager + 1000 + 4 * (column / 2));

            if (!(column & 1))
                width = 0;
            rrj_put16(rrj_at(m, manager + 292 + 2 * column, 2), (uint16_t)width);
            rrj_put16(rrj_at(m, manager + 512 + 2 * column, 2), (uint16_t)twos);
            if (group)
            {
                uint32_t count = 0;
                uint32_t item;
                uint32_t first_item = 1;

                *(uint8_t *)rrj_at(m, manager + 72 + column, 1) = 0;
                for (item = 0; item != 8; ++item)
                {
                    uint32_t value = (*(uint8_t *)rrj_at(m, packed + item / 4, 1) >> (6 - 2 * (item & 3))) & 3;

                    if (value)
                    {
                        ++count;
                        if (value == 2)
                            ++twos;
                        if (first_item)
                        {
                            first_item = 0;
                            *(uint8_t *)rrj_at(m, manager + 72 + column, 1) = (uint8_t)item;
                        }
                    }
                }
                if (rrj_read32(m, group + 12) || !count)
                {
                    width += count << 9;
                    *(uint8_t *)rrj_at(m, manager + 182 + column, 1) = (uint8_t)count;
                }
            }
            packed += 2;
        }
        {
            int32_t even = (int16_t)rrj_u16(rrj_at(m, manager + 48, 2));
            uint16_t origin = rrj_u16(rrj_at(m, 0x8005B390, 2));
            uint16_t height = rrj_u16(rrj_at(m, 0x8005B394, 2));

            even = (even / 2) * 2;
            rrj_put16(rrj_at(m, manager + 30, 2), 26);
            rrj_write32(m, manager + 36, 1);
            rrj_put16(rrj_at(m, manager + 32, 2), 1);
            rrj_put16(rrj_at(m, manager + 28, 2), 1);
            rrj_put16(rrj_at(m, manager + 58, 2), height);
            rrj_put16(rrj_at(m, manager + 62, 2), height + 16);
            rrj_put16(rrj_at(m, manager + 20, 2), 0xffff);
            rrj_put16(rrj_at(m, manager + 52, 2), (uint16_t)even);
            rrj_put16(rrj_at(m, manager + 54, 2), (uint16_t)(even - 1));
            rrj_put16(rrj_at(m, manager + 44, 2), 1);
            rrj_put16(rrj_at(m, manager + 56, 2), origin);
            rrj_put16(rrj_at(m, manager + 60, 2), origin);
            rrj_put16(rrj_at(m, manager + 46, 2), 0);
            rrj_put16(rrj_at(m, manager + 42, 2), rrj_u16(rrj_at(m, manager + 48, 2)));
            return sub_800662BC(m, call);
        }
    }
    if ((int16_t)rrj_u16(rrj_at(m, manager + 48, 2)) != (int16_t)rrj_u16(rrj_at(m, manager + 42, 2)))
    {
        int32_t delta = (int16_t)sub_800656A8(m, (int16_t)rrj_u16(rrj_at(m, manager + 52, 2)), (int16_t)rrj_u16(rrj_at(m, manager + 54, 2)), (int16_t)rrj_u16(rrj_at(m, manager + 48, 2)), (int16_t)rrj_u16(rrj_at(m, manager + 50, 2)));

        rrj_put16(rrj_at(m, manager + 30, 2), (uint16_t)delta);
        if (delta)
        {
            if (delta >= 0)
                rrj_put16(rrj_at(m, manager + 28, 2), 1);
            else
            {
                rrj_put16(rrj_at(m, manager + 30, 2), (uint16_t)-delta);
                rrj_put16(rrj_at(m, manager + 28, 2), 0);
            }
            (void)sub_800662BC(m, call);
        }
        rrj_put16(rrj_at(m, manager + 42, 2), rrj_u16(rrj_at(m, manager + 48, 2)));
    }
    return rrj_u16(rrj_at(m, manager + 48, 2));
}

uint32_t sub_800662C4(RRJMemory *m, uint32_t manager, RRJRaceLeafCall call)
{
    int32_t first_start = (int16_t)rrj_u16(rrj_at(m, manager + 52, 2));
    int32_t first_end = (int16_t)rrj_u16(rrj_at(m, manager + 54, 2));
    int32_t first_half = first_start / 2;
    int32_t second_half = first_end / 2;
    int32_t direction = (int16_t)rrj_u16(rrj_at(m, manager + 28, 2));
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
        rrj_write32(m, manager + 24, 0);
        rrj_put16(rrj_at(m, manager + 30, 2), 0);
        return 1;
    }
    index = candidate / 2;
    if (index == (int16_t)rrj_u16(rrj_at(m, manager + 20, 2)))
        return frontier_call(m, call, 0x800657A8, 0, 0, 0);
    entry = rrj_read32(m, manager + 1000 + 4 * (uint32_t)index);
    if (!entry)
    {
        rrj_write32(m, manager + 24, 0);
        rrj_put16(rrj_at(m, manager + 30, 2), 0);
        return 0;
    }
    payload = entry + 8;
    if (!rrj_read32(m, payload + 4))
    {
        int32_t boundary_mode = (int16_t)rrj_u16(rrj_at(m, manager + 32, 2));
        int32_t attempts = 1;

        for (;;)
        {
            if (attempts == 55)
            {
                rrj_put16(rrj_at(m, manager + 52, 2), 0);
                rrj_put16(rrj_at(m, manager + 54, 2), 109);
                rrj_write32(m, manager + 24, 0);
                rrj_put16(rrj_at(m, manager + 30, 2), 0);
                return manager;
            }
            if (direction == 1)
            {
                if (boundary_mode)
                {
                    int32_t old_end = (int16_t)rrj_u16(rrj_at(m, manager + 54, 2));

                    boundary_mode = 0;
                    rrj_put16(rrj_at(m, manager + 54, 2), (uint16_t)(2 * index - 1));
                    rrj_put16(rrj_at(m, manager + 52, 2), (uint16_t)(2 * index));
                    adjustment = old_end - (2 * index - 1);
                    ++index;
                }
                else if (index++ == first_half)
                {
                    int32_t value = (int16_t)rrj_u16(rrj_at(m, manager + 52, 2)) - 1;

                    rrj_write32(m, manager + 24, 0);
                    rrj_put16(rrj_at(m, manager + 30, 2), 0);
                    rrj_put16(rrj_at(m, manager + 54, 2), (uint16_t)(value < 0 ? 109 : value));
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
                    int32_t old_start = (int16_t)rrj_u16(rrj_at(m, manager + 52, 2));

                    boundary_mode = 0;
                    rrj_put16(rrj_at(m, manager + 52, 2), (uint16_t)(2 * next));
                    rrj_put16(rrj_at(m, manager + 54, 2), (uint16_t)(2 * next - 1));
                    adjustment = 2 * next - old_start;
                    --index;
                }
                else if (index-- == second_half)
                {
                    uint32_t old_end = rrj_u16(rrj_at(m, manager + 54, 2));
                    int32_t old_signed = (int16_t)old_end;

                    rrj_write32(m, manager + 24, 0);
                    rrj_put16(rrj_at(m, manager + 30, 2), 0);
                    rrj_put16(rrj_at(m, manager + 52, 2), (uint16_t)(old_end + 1));
                    if (old_signed >= 110)
                        rrj_put16(rrj_at(m, manager + 54, 2), 0);
                    return old_end + 1;
                }
                if (index < 0)
                    index = 54;
            }
            entry = rrj_read32(m, manager + 1000 + 4 * (uint32_t)index);
            if (!entry)
            {
                rrj_write32(m, manager + 24, 0);
                rrj_put16(rrj_at(m, manager + 30, 2), 0);
                return 0;
            }
            payload = entry + 8;
            ++attempts;
            if (rrj_read32(m, payload + 4))
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
            int32_t value = (int16_t)rrj_u16(rrj_at(m, manager + 54, 2)) + amount;
            rrj_put16(rrj_at(m, manager + 54, 2), (uint16_t)(value >= 111 ? value - 110 : value));
        }
        else
        {
            int32_t value = (int16_t)rrj_u16(rrj_at(m, manager + 52, 2)) - amount;
            rrj_put16(rrj_at(m, manager + 52, 2), (uint16_t)(value < 0 ? value + 110 : value));
        }
        progress = (int32_t)rrj_u16(rrj_at(m, manager + 30, 2)) - amount + adjustment;
        rrj_put16(rrj_at(m, manager + 30, 2), (uint16_t)progress);
        if ((int16_t)progress <= 0)
        {
            rrj_put16(rrj_at(m, manager + 30, 2), 0);
            rrj_write32(m, manager + 24, 0);
            return (uint32_t)progress << 16;
        }
    }
    {
        uint32_t product = rrj_read32(m, payload) * rrj_read32(m, payload + 4);

        if (product && product <= 4096)
        {
            uint32_t destination = manager + 9920 - 4 * rrj_u16(rrj_at(m, payload + 8, 2));

            (void)frontier_call(m, call, 0x8004D9A8, 0, 0, 0);
            (void)frontier_call(m, call, 0x8004D9E4, 0, 0, 0);
            rrj_write32(m, manager + 24, 2);
            if (!frontier_call(m, call, 0x80020400, payload + 8, destination, 0))
            {
                rrj_write32(m, manager + 732, payload);
                rrj_put16(rrj_at(m, manager + 20, 2), (uint16_t)index);
                rrj_write32(m, manager + 24, 1);
                (void)frontier_call(m, call, 0x8004D90C, destination, (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, manager + 22, 2)), 0);
                return frontier_call(m, call, 0x8004D988, manager + 1220, product / 2, 0);
            }
        }
    }
    rrj_write32(m, manager + 24, 0);
    rrj_put16(rrj_at(m, manager + 30, 2), 0);
    return manager;
}

uint32_t sub_80020400(RRJMemory *m, uint32_t input, uint32_t output)
{
    uint32_t bit_count = 0;
    uint32_t output_start = output;
    uint32_t lengths_a = rrj_read32(m, 0x8005ADAC);
    uint32_t lengths_b = rrj_read32(m, 0x8005ADB0);
    uint32_t values_a = lengths_a + 0x2000;
    uint32_t values_b = lengths_b + 512;
    uint32_t prefix;
    uint32_t bits;

    FUNCTION_MARKER(0x80020400, "SLUS_010.53");
    rrj_put16(rrj_at(m, output, 2), rrj_u16(rrj_at(m, input, 2)));
    output += 2;
    rrj_put16(rrj_at(m, output, 2), rrj_u16(rrj_at(m, input + 2, 2)));
    output += 2;
    prefix = (uint32_t)rrj_u16(rrj_at(m, input + 4, 2)) << 10;
    bits = ((uint32_t)rrj_u16(rrj_at(m, input + 8, 2)) << 16) | rrj_u16(rrj_at(m, input + 10, 2));
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
            bits |= (uint32_t)rrj_u16(rrj_at(m, input, 2)) << bit_count;
            input += 2;
        }
        rrj_put16(rrj_at(m, output, 2), (uint16_t)(code | prefix));
        output += 2;
        for (;;)
        {
            uint32_t index = bits >> 19;
            uint32_t length;
            uint16_t value;

            if (index >= 32)
            {
                length = *(uint8_t *)rrj_at(m, lengths_a + index, 1);
                value = rrj_u16(rrj_at(m, values_a + 2 * index, 2));
            }
            else
            {
                bit_count += 8;
                bits <<= 8;
                if (bit_count & 16)
                {
                    bit_count &= 15;
                    bits |= (uint32_t)rrj_u16(rrj_at(m, input, 2)) << bit_count;
                    input += 2;
                }
                index = bits >> 23;
                length = *(uint8_t *)rrj_at(m, lengths_b + index, 1);
                value = rrj_u16(rrj_at(m, values_b + 2 * index, 2));
            }
            bits <<= length;
            bit_count += length;
            if (bit_count & 16)
            {
                bit_count &= 15;
                bits |= (uint32_t)rrj_u16(rrj_at(m, input, 2)) << bit_count;
                input += 2;
            }
            if (value == 0x7c1f)
            {
                rrj_put16(rrj_at(m, output, 2), (uint16_t)(bits >> 16));
                output += 2;
                bits = (bits << 16) | ((uint32_t)rrj_u16(rrj_at(m, input, 2)) << bit_count);
                input += 2;
                continue;
            }
            rrj_put16(rrj_at(m, output, 2), value);
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
                rrj_put16(rrj_at(m, output, 2), 0xfe00);
                output += 2;
                padded = used - 2;
            }
            while (padded & 0x7f)
            {
                rrj_write32(m, output, 0xfe00fe00);
                output += 4;
                padded += 4;
            }
        }
    }
    return 0;
}

uint32_t sub_800644F4(RRJMemory *m, RRJRaceLeafCall call)
{
    FUNCTION_MARKER(0x800644F4, "RASHCDG.BIN");
    return frontier_call(m, call, 0x800644FC, rrj_read32(m, 0x8005B278), 0x80060000, 0);
}

static void frontier_panorama_project(RRJMemory *m, uint32_t output, int32_t angle_index, int32_t first_row, int32_t last_row)
{
    SVECTOR vertex;

    vertex.vx = (sint16)rrj_u16(rrj_at(m, 0x8005624C + 4 * ((uint32_t)angle_index & 0xfff), 2));
    vertex.vz = (sint16)rrj_u16(rrj_at(m, 0x8005624E + 4 * ((uint32_t)angle_index & 0xfff), 2));
    vertex.pad = 0;
    while (first_row <= last_row)
    {
        sint32 screen;
        sint32 flags;
        sint32 depth;

        vertex.vy = (sint16)(279 * first_row - 2232);
        depth = gte_project(&vertex, &screen, &flags);
        rrj_write32(m, output + 4 * (uint32_t)first_row, (uint32_t)screen);
        rrj_write32(m, 0x1F800034, (uint32_t)depth);
        ++first_row;
    }
}

uint32_t sub_800644FC(RRJMemory *m, uint32_t manager, RRJRaceLeafCall call)
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
    unsigned iteration;

    FUNCTION_MARKER(0x800644FC, "RASHCDG.BIN");
    if ((int16_t)rrj_u16(rrj_at(m, manager + 18, 2)) == -1)
        return 0xffffffff;
    rrj_write32(m, 0x1F800028, first_buffer);
    rrj_write32(m, 0x1F80002C, second_buffer);
    if (rrj_read32(m, manager + 24))
    {
        unsigned timeout = 0;

        while (rrj_read32(m, manager + 24) && ++timeout < 10000)
        {
            if (rrj_read32(m, manager + 24) == 2)
            {
                (void)frontier_call(m, call, 0x800657A8, 0, 0, 0);
                timeout = 1;
            }
        }
        if (rrj_read32(m, manager + 24))
        {
            rrj_write32(m, manager + 24, 0);
            rrj_put16(rrj_at(m, manager + 30, 2), 0);
            return frontier_call(m, call, 0x8004D7B4, 1, 0, 0);
        }
    }
    packet = rrj_read32(m, rrj_read32(m, 0x8005B470) + 268);
    current_column = (int16_t)rrj_u16(rrj_at(m, manager + 52, 2));
    texture_row = (int16_t)rrj_u16(rrj_at(m, manager + 36, 2));
    while (current_column != (int16_t)rrj_u16(rrj_at(m, manager + 48, 2)))
    {
        texture_row += *(uint8_t *)rrj_at(m, manager + 182 + (uint32_t)current_column, 1);
        current_column = current_column == 109 ? 0 : current_column + 1;
    }
    if (texture_row >= (int16_t)rrj_u16(rrj_at(m, manager + 38, 2)))
        texture_row -= (int16_t)rrj_u16(rrj_at(m, manager + 38, 2));
    if (texture_row > (int16_t)rrj_u16(rrj_at(m, manager + 46, 2)) && texture_row < (int16_t)rrj_u16(rrj_at(m, manager + 44, 2)))
        texture_row = (int16_t)rrj_u16(rrj_at(m, manager + 44, 2));
    texture = rrj_read32(m, manager + 992) + 8;
    current_column = (int16_t)rrj_u16(rrj_at(m, manager + 48, 2));
    low = *(uint8_t *)rrj_at(m, texture + (uint32_t)current_column, 1) + *(uint8_t *)rrj_at(m, manager + 72 + (uint32_t)current_column, 1);
    high = low + *(uint8_t *)rrj_at(m, manager + 182 + (uint32_t)current_column, 1);
    rrj_write32(m, 0x1F800024, texture);
    rrj_write32(m, 0x1F800004, (uint32_t)current_column);
    rrj_write32(m, 0x1F80000C, (uint32_t)low);
    rrj_write32(m, 0x1F800018, (uint32_t)high);
    frontier_panorama_project(m, first_buffer, (2383 * current_column) >> 6, low - 1, high);
    current_column = current_column == 109 ? 0 : current_column + 1;
    strip = texture_row + 1;
    rrj_write32(m, 0x1F800020, (uint32_t)((int16_t)rrj_u16(rrj_at(m, manager + 46, 2)) < texture_row ? (int16_t)rrj_u16(rrj_at(m, manager + 40, 2)) : (int16_t)rrj_u16(rrj_at(m, manager + 46, 2)) + 1));
    ot_head = rrj_read32(m, rrj_read32(m, 0x8005B59C));
    rrj_write32(m, 0x1F800000, 0);
    rrj_write32(m, 0x1F800030, ot_head);
    for (iteration = 0; iteration != 25; ++iteration)
    {
        int32_t next_low = *(uint8_t *)rrj_at(m, texture + (uint32_t)current_column, 1) + *(uint8_t *)rrj_at(m, manager + 72 + (uint32_t)current_column, 1);
        int32_t next_high = next_low + *(uint8_t *)rrj_at(m, manager + 182 + (uint32_t)current_column, 1);
        int32_t overlap_low = low < next_low ? low : next_low;
        int32_t overlap_high = high > next_high ? high : next_high;
        int32_t count = high - low;
        int32_t index;

        rrj_write32(m, 0x1F800010, (uint32_t)next_low);
        rrj_write32(m, 0x1F80001C, (uint32_t)next_high);
        rrj_write32(m, 0x1F800008, (uint32_t)overlap_low);
        rrj_write32(m, 0x1F800014, (uint32_t)overlap_high);
        frontier_panorama_project(m, second_buffer, (2383 * current_column) >> 6, overlap_low - 1, overlap_high);
        if (count > 0)
        {
            if (packet + 40 * (uint32_t)count >= rrj_read32(m, 0x8005B4D0))
                packet = sub_80021C98(m, packet, 40 * (uint32_t)count);
            for (index = 0; index != count; ++index)
            {
                uint32_t descriptor = manager + 10052 + 12 * (uint32_t)strip;
                uint32_t row = (uint32_t)(low + index);

                rrj_write32(m, packet, ot_head | 0x09000000);
                ot_head = packet;
                rrj_write32(m, packet + 4, rrj_read32(m, 0x800523BC) | 0x2c000000);
                rrj_write32(m, packet + 8, rrj_read32(m, first_buffer + 4 * row));
                rrj_write32(m, packet + 12, rrj_read32(m, descriptor));
                rrj_write32(m, packet + 16, rrj_read32(m, second_buffer + 4 * row));
                rrj_write32(m, packet + 20, rrj_read32(m, descriptor + 4));
                rrj_write32(m, packet + 24, rrj_read32(m, first_buffer + 4 * (row + 1)));
                rrj_write32(m, packet + 28, rrj_read32(m, descriptor + 8) >> 16);
                rrj_write32(m, packet + 32, rrj_read32(m, second_buffer + 4 * (row + 1)));
                rrj_write32(m, packet + 36, rrj_read32(m, descriptor + 8));
                packet += 40;
                ++strip;
                if (strip >= rrj_s32(rrj_read32(m, 0x1F800020)))
                {
                    if (strip == (int16_t)rrj_u16(rrj_at(m, manager + 46, 2)) + 1)
                    {
                        strip = (int16_t)rrj_u16(rrj_at(m, manager + 44, 2));
                        rrj_write32(m, 0x1F800020, rrj_u16(rrj_at(m, manager + 40, 2)));
                    }
                    else if (strip >= (int16_t)rrj_u16(rrj_at(m, manager + 40, 2)))
                    {
                        strip = 0;
                        rrj_write32(m, 0x1F800020, rrj_u16(rrj_at(m, manager + 40, 2)));
                    }
                }
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
        rrj_write32(m, 0x1F800028, first_buffer);
        rrj_write32(m, 0x1F80002C, second_buffer);
        rrj_write32(m, 0x1F800004, (uint32_t)current_column);
        rrj_write32(m, 0x1F80000C, (uint32_t)low);
        rrj_write32(m, 0x1F800018, (uint32_t)high);
        rrj_write32(m, 0x1F800000, iteration + 1);
    }
    rrj_write32(m, rrj_read32(m, 0x8005B59C), ot_head);
    rrj_write32(m, rrj_read32(m, 0x8005B470) + 268, packet);
    return rrj_read32(m, 0x8005B470);
}

static uint32_t frontier_project_vertex(RRJMemory *m, uint32_t address)
{
    SVECTOR vertex;
    sint32 screen;
    sint32 flags;

    vertex.vx = (sint16)rrj_u16(rrj_at(m, address, 2));
    vertex.vy = (sint16)rrj_u16(rrj_at(m, address + 2, 2));
    vertex.vz = (sint16)rrj_u16(rrj_at(m, address + 4, 2));
    vertex.pad = (sint16)rrj_u16(rrj_at(m, address + 6, 2));
    (void)gte_project(&vertex, &screen, &flags);
    return (uint32_t)screen;
}

static uint32_t frontier_panorama_clip(uint32_t first, uint32_t second)
{
    return ((first & second) & 0x8000) | ((int16_t)(first & 0xffff) >= 385 && (int16_t)(second & 0xffff) >= 385);
}

static uint32_t sub_800642F8(RRJMemory *m, uint32_t ot, uint32_t first, uint32_t second, uint32_t third, uint32_t fourth, uint32_t first_color, uint32_t second_color, uint32_t third_color, uint32_t fourth_color)
{
    uint32_t display = rrj_read32(m, 0x8005B470);
    uint32_t packet = rrj_read32(m, display + 268);
    unsigned index;
    const uint32_t positions[4] = {first, second, third, fourth};
    const uint32_t colors[4] = {first_color, second_color, third_color, fourth_color};

    FUNCTION_MARKER(0x800642F8, "RASHCDG.BIN");
    if (packet + 36 >= rrj_read32(m, 0x8005B4D0))
        packet = sub_80021C98(m, packet, 36);
    rrj_write32(m, display + 268, packet + 36);
    *(uint8_t *)rrj_at(m, packet + 3, 1) = 8;
    *(uint8_t *)rrj_at(m, packet + 7, 1) = 56;
    for (index = 0; index != 4; ++index)
    {
        rrj_write32(m, packet + 8 + 8 * index, positions[index]);
        *(uint8_t *)rrj_at(m, packet + 4 + 8 * index, 1) = (uint8_t)colors[index];
        *(uint8_t *)rrj_at(m, packet + 5 + 8 * index, 1) = (uint8_t)(colors[index] >> 8);
        *(uint8_t *)rrj_at(m, packet + 6 + 8 * index, 1) = (uint8_t)(colors[index] >> 16);
    }
    rrj_write32(m, packet, (rrj_read32(m, packet) & 0xff000000) | (rrj_read32(m, ot) & 0x00ffffff));
    rrj_write32(m, ot, (rrj_read32(m, ot) & 0xff000000) | (packet & 0x00ffffff));
    return rrj_read32(m, ot);
}

uint32_t sub_8006396C(RRJMemory *m)
{
    int32_t segment = 24 * (int16_t)rrj_u16(rrj_at(m, 0x800D5EE8, 2)) / 110;
    uint32_t display = rrj_read32(m, 0x8005B470);
    uint32_t packet = rrj_read32(m, display + 268);
    uint32_t ot = rrj_read32(m, 0x8005B59C) + 4;
    uint32_t source = 0x800D443C + 16 * (uint32_t)segment;
    uint32_t previous_first;
    uint32_t previous_second;
    uint32_t previous_clip;
    unsigned index;

    FUNCTION_MARKER(0x8006396C, "RASHCDG.BIN");
    if (packet + 320 >= rrj_read32(m, 0x8005B4D0))
    {
        packet = sub_80021C98(m, packet, 320);
        rrj_write32(m, display + 268, packet);
    }
    previous_first = frontier_project_vertex(m, source);
    previous_second = frontier_project_vertex(m, source + 8);
    previous_clip = frontier_panorama_clip(previous_first, previous_second);
    source += 16;
    for (index = 0; index != 8; ++index, ++segment)
    {
        uint32_t current_first;
        uint32_t current_second;
        uint32_t current_clip;

        if (segment == 23)
            source = 0x800D443C;
        current_first = frontier_project_vertex(m, source);
        current_second = frontier_project_vertex(m, source + 8);
        current_clip = frontier_panorama_clip(current_first, current_second);
        source += 16;
        if (!(previous_clip & current_clip))
        {
            uint32_t descriptor = 0x800D440C + 12 * ((uint32_t)segment & 3);

            rrj_write32(m, packet, rrj_read32(m, ot) | 0x09000000);
            rrj_write32(m, packet + 4, 0x2f000000);
            rrj_write32(m, packet + 8, previous_first);
            rrj_write32(m, packet + 12, rrj_read32(m, descriptor));
            rrj_write32(m, packet + 16, current_first);
            rrj_write32(m, packet + 20, rrj_read32(m, descriptor + 4));
            rrj_write32(m, packet + 24, previous_second);
            rrj_write32(m, packet + 28, rrj_u16(rrj_at(m, descriptor + 10, 2)));
            rrj_write32(m, packet + 32, current_second);
            rrj_write32(m, packet + 36, rrj_read32(m, descriptor + 8));
            rrj_write32(m, ot, packet);
            packet += 40;
        }
        previous_first = current_first;
        previous_second = current_second;
        previous_clip = current_clip;
    }
    rrj_write32(m, display + 268, packet);
    return display;
}

static uint32_t frontier_pack_pair(int16_t x, int16_t y)
{
    return (uint16_t)x | ((uint32_t)(uint16_t)y << 16);
}

static uint32_t frontier_read_color(RRJMemory *m, uint32_t address)
{
    return rrj_read32(m, address) & 0x00ffffff;
}

static uint32_t frontier_blend_color(RRJMemory *m, uint32_t first, uint32_t second, int32_t weight)
{
    uint32_t result = 0;
    unsigned component;

    for (component = 0; component != 3; ++component)
    {
        int32_t a = *(uint8_t *)rrj_at(m, first + component, 1);
        int32_t b = *(uint8_t *)rrj_at(m, second + component, 1);
        uint32_t value = (uint32_t)sub_8001FC90(weight, b) + (uint32_t)sub_8001FC90(0x10000 - weight, a);

        result |= (value & 0xff) << (8 * component);
    }
    return result;
}

static uint32_t frontier_project_direction(RRJMemory *m, int32_t horizontal, int32_t vertical)
{
    SVECTOR vector;
    sint32 screen;
    sint32 flags;

    horizontal &= 0xfff;
    vertical &= 0xfff;
    vector.vx = (int16_t)rrj_u16(rrj_at(m, 0x8005624C + 4 * (uint32_t)horizontal, 2));
    vector.vy = -(int16_t)rrj_u16(rrj_at(m, 0x8005624C + 4 * (uint32_t)vertical, 2));
    vector.vz = (int16_t)rrj_u16(rrj_at(m, 0x8005624E + 4 * (uint32_t)horizontal, 2));
    vector.pad = 0;
    (void)gte_project(&vector, &screen, &flags);
    return (uint32_t)screen;
}

uint32_t sub_80063C5C(RRJMemory *m, uint32_t player_index)
{
    uint32_t actor = rrj_read32(m, 0x8005AEC0 + 4 * player_index);
    int32_t heading = (int16_t)rrj_u16(rrj_at(m, actor + 124, 2));
    int32_t relative = heading - rrj_s32(rrj_read32(m, 0x80052388));
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
    uint32_t upper_color = frontier_read_color(m, 0x800523F8);
    uint32_t lower_color = frontier_read_color(m, 0x800523FC);
    uint32_t ot = rrj_read32(m, 0x8005B59C) + 12;

    FUNCTION_MARKER(0x80063C5C, "RASHCDG.BIN");
    rrj_write32(m, 0x800CCD68, (uint32_t)heading);
    center_screen = frontier_project_direction(m, heading, 0);
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
        horizon_screen = frontier_project_direction(m, rrj_s32(rrj_read32(m, 0x80052388)), rrj_s32(rrj_read32(m, 0x8005238C)));
    }
    else if (left < 2048 && right >= 2049)
    {
        mode = 2;
        selected_color = 0x800523F4;
        horizon_screen = frontier_project_direction(m, rrj_s32(rrj_read32(m, 0x80052388)) + 2048, rrj_s32(rrj_read32(m, 0x8005238C)));
    }
    else
    {
        horizon_screen = frontier_project_direction(m, heading - 426, rrj_s32(rrj_read32(m, 0x8005238C)));
    }
    if (left >= 2049)
        left = 4096 - left;
    if (right >= 2049)
        right = 4096 - right;
    left_color = frontier_blend_color(m, 0x800523F0, 0x800523F4, left << 5);
    right_color = frontier_blend_color(m, 0x800523F0, 0x800523F4, right << 5);
    if (rrj_read32(m, rrj_read32(m, 0x8005B2F8) + 48) < 2)
    {
        int16_t bottom = (int16_t)(center_screen >> 16) + 10;

        if (rrj_read32(m, 0x8005AD24))
            bottom -= (int16_t)rrj_u16(rrj_at(m, 0x800D513C, 2));
        rrj_put16(rrj_at(m, 0x800523DA, 2), (uint16_t)bottom);
        rrj_put16(rrj_at(m, 0x800523DE, 2), (uint16_t)bottom);
    }
    else
    {
        uint32_t viewport = rrj_read32(m, 0x8005B474) + 8 * player_index;
        int16_t x = (int16_t)rrj_u16(rrj_at(m, viewport, 2));
        int16_t y = (int16_t)rrj_u16(rrj_at(m, viewport + 2, 2));
        int16_t width = (int16_t)rrj_u16(rrj_at(m, viewport + 4, 2));
        int16_t bottom = (int16_t)(center_screen >> 16);

        rrj_put16(rrj_at(m, 0x800523D0, 2), (uint16_t)x);
        rrj_put16(rrj_at(m, 0x800523D2, 2), (uint16_t)y);
        rrj_put16(rrj_at(m, 0x800523D4, 2), (uint16_t)(x + width));
        rrj_put16(rrj_at(m, 0x800523D6, 2), (uint16_t)y);
        rrj_put16(rrj_at(m, 0x800523D8, 2), (uint16_t)(x + width));
        rrj_put16(rrj_at(m, 0x800523DC, 2), (uint16_t)x);
        if (rrj_read32(m, 0x8005AD24))
            bottom -= (int16_t)rrj_u16(rrj_at(m, 0x800D513C, 2));
        rrj_put16(rrj_at(m, 0x800523DA, 2), (uint16_t)bottom);
        rrj_put16(rrj_at(m, 0x800523DE, 2), (uint16_t)bottom);
    }
    top_left = frontier_pack_pair((int16_t)rrj_u16(rrj_at(m, 0x800523D0, 2)), (int16_t)rrj_u16(rrj_at(m, 0x800523D2, 2)));
    top_right = frontier_pack_pair((int16_t)rrj_u16(rrj_at(m, 0x800523D4, 2)), (int16_t)rrj_u16(rrj_at(m, 0x800523D6, 2)));
    bottom_right = frontier_pack_pair((int16_t)rrj_u16(rrj_at(m, 0x800523D8, 2)), (int16_t)rrj_u16(rrj_at(m, 0x800523DA, 2)));
    bottom_left = frontier_pack_pair((int16_t)rrj_u16(rrj_at(m, 0x800523DC, 2)), (int16_t)rrj_u16(rrj_at(m, 0x800523DE, 2)));
    left_horizon = frontier_pack_pair((int16_t)horizon_screen, (int16_t)(horizon_screen >> 16));
    right_horizon = frontier_pack_pair((int16_t)rrj_u16(rrj_at(m, 0x800523D4, 2)), (int16_t)(horizon_screen >> 16));
    if (mode)
    {
        uint32_t selected_upper = frontier_read_color(m, selected_color);
        uint32_t selected_lower = frontier_read_color(m, selected_color + 4);
        uint32_t left_top = frontier_pack_pair((int16_t)horizon_screen, (int16_t)rrj_u16(rrj_at(m, 0x800523D2, 2)));
        uint32_t left_bottom = frontier_pack_pair((int16_t)horizon_screen, (int16_t)rrj_u16(rrj_at(m, 0x800523DE, 2)));

        (void)sub_800642F8(m, ot, left_top, top_left, horizon_screen, left_horizon, upper_color, upper_color, selected_upper, left_color);
        (void)sub_800642F8(m, ot, top_right, left_top, right_horizon, horizon_screen, upper_color, upper_color, right_color, selected_upper);
        (void)sub_800642F8(m, ot, left_horizon, horizon_screen, bottom_left, left_bottom, left_color, selected_lower, selected_lower, lower_color);
        return sub_800642F8(m, ot, right_horizon, horizon_screen, bottom_right, left_bottom, right_color, selected_lower, lower_color, lower_color);
    }
    (void)sub_800642F8(m, ot, top_right, top_left, right_horizon, left_horizon, upper_color, upper_color, left_color, left_color);
    return sub_800642F8(m, ot, left_horizon, right_horizon, bottom_left, bottom_right, left_color, right_color, lower_color, lower_color);
}

static uint32_t frontier_draw_mode_packet(RRJMemory *m, uint32_t link, int dtd)
{
    uint32_t display = rrj_read32(m, 0x8005B470);
    uint32_t packet = rrj_read32(m, display + 268);
    DR_MODE *mode;

    if (packet + 12 >= rrj_read32(m, 0x8005B4D0))
        packet = sub_80021C98(m, packet, 12);
    rrj_write32(m, display + 268, packet + 12);
    rrj_write32(m, packet, rrj_read32(m, link));
    rrj_write32(m, link, packet);
    mode = (DR_MODE *)rrj_at(m, packet, 12);
    SetDrawMode(mode, 0, dtd, 0x120, 0);
    return packet;
}

uint32_t sub_8002C4F8(RRJMemory *m, uint32_t position, uint32_t link, uint32_t player_index)
{
    uint32_t actor = rrj_read32(m, 0x8005AEC0 + 4 * player_index);
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
    memcpy(matrix.m, rrj_at(m, actor + 92, 18), 18);
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);
    *(uint8_t *)rrj_at(m, state + player_index + 152, 1) = 0;
    vertex.vx = (int16_t)rrj_u16(rrj_at(m, position, 2));
    vertex.vy = (int16_t)rrj_u16(rrj_at(m, position + 2, 2));
    vertex.vz = (int16_t)rrj_u16(rrj_at(m, position + 4, 2));
    vertex.pad = (int16_t)rrj_u16(rrj_at(m, position + 6, 2));
    gte_transform(&vertex, &transformed, &flags);
    if (transformed.vz < transformed.vx || transformed.vz < -transformed.vx)
        return (uint32_t)-transformed.vx;
    (void)gte_project(&vertex, &screen, &flags);
    if (rrj_read32(m, rrj_read32(m, 0x8005B2F8) + 48) >= 2)
    {
        uint32_t viewport = rrj_read32(m, 0x8005B474) + 8 * player_index;

        center_x = (int16_t)rrj_u16(rrj_at(m, viewport, 2)) + (int16_t)rrj_u16(rrj_at(m, viewport + 4, 2)) / 2;
        center_y = (int16_t)rrj_u16(rrj_at(m, viewport + 2, 2)) + (int16_t)rrj_u16(rrj_at(m, viewport + 6, 2)) / 2;
    }
    offset_x = 4 * ((int16_t)screen - center_x);
    offset_y = 4 * ((int16_t)(screen >> 16) - center_y);
    scale = 4 * rrj_s32(rrj_read32(m, actor + 4));
    if (scale >= 2897)
        scale = 2896;
    rrj_write32(m, state + 32 + 4 * player_index, (uint32_t)screen);
    rrj_write32(m, state + 16 * player_index, (uint32_t)offset_x);
    rrj_write32(m, state + 16 * player_index + 4, (uint32_t)offset_y);
    rrj_write32(m, state + 16 * player_index + 8, (uint32_t)scale);
    if ((int16_t)screen < 381 && !(rrj_read32(m, 0x80052354) & 4))
    {
        uint8_t quad[16];
        uint8_t color[4];
        int16_t x = (int16_t)screen;
        int16_t y = (int16_t)(screen >> 16);

        rrj_put32(color, 0x00ffffff);
        rrj_put32(quad, frontier_pack_pair(x - 2, y - 2));
        rrj_put32(quad + 4, frontier_pack_pair(x + 3, y - 2));
        rrj_put32(quad + 8, frontier_pack_pair(x - 2, y + 3));
        rrj_put32(quad + 12, frontier_pack_pair(x + 3, y + 3));
        (void)sub_8002B080(m, quad, color, rrj_at(m, link, 4));
        *(uint8_t *)rrj_at(m, state + player_index + 152, 1) = 1;
    }
    (void)frontier_draw_mode_packet(m, link, 1);
    if (rrj_read32(m, 0x80052354) & 1)
    {
        uint8_t point[4];

        rrj_put32(point, (uint32_t)screen);
        (void)sub_8002B3FC(m, point, rrj_at(m, 0x8005233C, 4), 16, 13, 7, rrj_at(m, link, 4));
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
        rrj_put32(point, (uint32_t)screen);
        (void)sub_8002B878(m, point, rrj_at(m, 0x8005233C, 4), rrj_at(m, link, 4));
    }
    return frontier_draw_mode_packet(m, link, 0);
}

uint32_t sub_80021988(RRJMemory *m, int32_t minimum, int32_t maximum, RRJRaceLeafCall call)
{
    uint32_t display = rrj_read32(m, 0x8005B470);
    uint32_t slot = rrj_read32(m, 0x8005AE00) + 1;
    uint32_t slot_count = *(uint8_t *)rrj_at(m, display + 244, 1);
    uint32_t next;
    int32_t span;
    uint32_t shift;
    uint32_t count;
    uint32_t ot;

    FUNCTION_MARKER(0x80021988, "SLUS_010.53");
    if (slot >= slot_count)
        slot = 0;
    while (rrj_read32(m, 0x800D75D0 + 64 * slot))
    {
        if (rrj_read32(m, 0x8005B46C) - rrj_read32(m, 0x800D75C0 + 4 * slot) >= 61)
            rrj_write32(m, 0x800D75D0 + 64 * slot, 0);
        else
            (void)frontier_call(m, call, 0x8001B700u, 0, 0, 0);
    }
    if (maximum < 0)
    {
        shift = rrj_read32(m, 0x80053224);
        rrj_write32(m, 0x8005B4D4, 0);
        span = 1920;
    }
    else if (minimum < 0)
    {
        shift = rrj_read32(m, 0x80053224);
        rrj_write32(m, 0x8005B4D4, 0);
        span = maximum + 1920;
    }
    else
    {
        int32_t table_index = minimum >> 11;

        if (table_index >= 10)
            table_index = 9;
        shift = rrj_read32(m, 0x80053224 + 4 * (uint32_t)table_index);
        rrj_write32(m, 0x8005B4D4, (uint32_t)(minimum - 960));
        span = maximum - minimum + 1920;
    }
    rrj_write32(m, 0x8005B4D8, shift);
    count = ((uint32_t)span + (1u << shift)) >> shift;
    if (rrj_s32(rrj_read32(m, 0x8005B4D4)) < 4096)
        count += 768;
    rrj_write32(m, 0x8005ADFC, count);
    rrj_write32(m, 0x8005AE00, slot);
    next = slot + 1;
    if (next >= slot_count)
        next = 0;
    rrj_write32(m, 0x8005AE04, next);
    rrj_write32(m, 0x800D75B0 + 4 * slot, rrj_read32(m, display + 268) - 60);
    while (!rrj_read32(m, 0x800D75D0 + 64 * next) && next != slot)
    {
        ++next;
        if (next >= slot_count)
            next = 0;
        rrj_write32(m, 0x8005AE04, next);
    }
    rrj_write32(m, 0x800D75D0 + 64 * slot, 1);
    rrj_write32(m, 0x800D75C0 + 4 * slot, rrj_read32(m, 0x8005B46C));
    ot = rrj_read32(m, display + 248 + 4 * slot);
    rrj_write32(m, display + 264, ot);
    if (count >= 1300)
    {
        count = 1299;
        rrj_write32(m, 0x8005ADFC, count);
    }
    rrj_gpu_clear_ot(m, ot, count);
    return 0;
}

uint32_t sub_8002AF80(RRJMemory *m, uint32_t matrix)
{
    FUNCTION_MARKER(0x8002AF80, "SLUS_010.53");
    rrj_write32(m, matrix, 4096);
    rrj_write32(m, matrix + 4, 0);
    rrj_write32(m, matrix + 8, 4096);
    rrj_write32(m, matrix + 12, 0);
    rrj_write32(m, matrix + 16, 4096);
    return 4096;
}

uint32_t sub_8002BAE8(RRJMemory *m, uint32_t first_value, uint32_t second_value, uint32_t ordering_slot)
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

        vertex.vx = (int16_t)rrj_u16(rrj_at(m, source, 2));
        vertex.vy = (int16_t)rrj_u16(rrj_at(m, source + 2, 2));
        vertex.vz = (int16_t)rrj_u16(rrj_at(m, source + 4, 2));
        vertex.pad = (int16_t)rrj_u16(rrj_at(m, source + 6, 2));
        (void)gte_project(&vertex, &screen, &flags);
        screens[index] = (uint32_t)screen;
    }
    for (index = 8; index != 0; --index)
    {
        uint32_t item = index - 1;
        uint32_t context = rrj_read32(m, 0x8005B470u);
        uint32_t packet = rrj_read32(m, context + 268);
        uint32_t first_index;
        uint32_t second_index;
        uint32_t third_index;

        if (packet + 36 >= rrj_read32(m, 0x8005B4D0u))
        {
            packet = sub_80021C98(m, packet, 36);
            rrj_write32(m, context + 268, packet);
        }
        rrj_write32(m, context + 268, packet + 36);
        rrj_write32(m, packet, rrj_read32(m, ordering_slot));
        rrj_write32(m, ordering_slot, packet);
        rrj_write32(m, packet + 4, 0x08000000u);
        rrj_write32(m, packet + 12, rrj_read32(m, second_value));
        rrj_write32(m, packet + 16, rrj_read32(m, first_value));
        rrj_write32(m, packet + 20, 0);
        rrj_write32(m, packet + 28, 0);
        first_index = (uint32_t)(int16_t)rrj_u16(rrj_at(m, 0x80053960u + 2 * item, 2));
        second_index = (uint32_t)(int16_t)rrj_u16(rrj_at(m, 0x80053980u + 2 * item, 2));
        third_index = (uint32_t)(int16_t)rrj_u16(rrj_at(m, 0x80053970u + 2 * item, 2));
        rrj_write32(m, packet + 8, screens[first_index]);
        rrj_write32(m, packet + 24, screens[second_index]);
        rrj_write32(m, packet + 32, screens[third_index]);
    }
    return UINT32_MAX;
}

uint32_t sub_8002BE14(RRJMemory *m, uint32_t point, uint32_t ordering_slot, uint32_t player_index)
{
    int32_t x = (int16_t)rrj_u16(rrj_at(m, point, 2));
    int32_t y = (int16_t)rrj_u16(rrj_at(m, point + 2, 2));
    int32_t target_x;
    int32_t target_y;
    uint32_t count = 0;
    uint32_t index;
    MATRIX translation;

    FUNCTION_MARKER(0x8002BE14, "SLUS_010.53");
    if (rrj_read32(m, rrj_read32(m, 0x8005B2F8u) + 48) < 2)
    {
        target_x = 384 - x;
        target_y = 240 - y;
    }
    else
    {
        uint32_t viewport = rrj_read32(m, 0x8005B474u) + 8 * player_index;

        target_x = 2 * (int16_t)rrj_u16(rrj_at(m, viewport, 2)) + (int16_t)rrj_u16(rrj_at(m, viewport + 4, 2)) - x;
        target_y = 2 * (int16_t)rrj_u16(rrj_at(m, viewport + 2, 2)) + (int16_t)rrj_u16(rrj_at(m, viewport + 6, 2)) - y;
    }
    for (index = 0; index != 25; ++index)
        if ((rrj_u16(rrj_at(m, 0x800D7FF0u + 52 * player_index + 2 * index, 2)) & 0x7FFFu) == 0x7FFFu)
            ++count;
    if (!count)
        return 16 * player_index;

    memset(&translation, 0, sizeof(translation));
    translation.t[0] = rrj_s32(rrj_read32(m, 0x800D7FC8u + 16 * player_index));
    translation.t[1] = rrj_s32(rrj_read32(m, 0x800D7FCCu + 16 * player_index));
    translation.t[2] = rrj_s32(rrj_read32(m, 0x800D7FD0u + 16 * player_index));
    SetTransMatrix(&translation);

    frontier_set_flare_matrix(m, 8 * (x + y), (int32_t)count << 7, (int32_t)count << 6);
    (void)sub_8002BAE8(m, point, 0x80052340u, ordering_slot);
    frontier_set_flare_matrix(m, 6 * (x + y), (int32_t)count << 6, (int32_t)count << 6);
    (void)sub_8002BAE8(m, point, 0x80052340u, ordering_slot);

    index = 1;
    if ((rrj_read32(m, 0x80052354u) & 2u) != 0)
    {
        index = 0;
        rrj_write32(m, rrj_read32(m, 0x800D805Cu) + 8, rrj_read32(m, 0x80052344u));
    }
    while (index < 9)
    {
        uint32_t record = rrj_read32(m, 0x800D805Cu) + 16 * index;
        int32_t weight = rrj_s32(rrj_read32(m, record));
        int32_t width = rrj_s32(rrj_read32(m, 0x800D8058u) * rrj_read32(m, record + 4)) / 65536;
        int32_t height = rrj_s32((uint32_t)(3413 * width)) >> 12;

        if (width >= 4)
        {
            uint8_t xy[4];
            uint8_t color[4] = {0, 0, 0, 0};
            int32_t screen_x = ((0x10000 - weight) * x + weight * target_x) / 65536;
            int32_t screen_y = ((0x10000 - weight) * y + weight * target_y) / 65536;
            uint32_t kind = *(uint8_t *)rrj_at(m, record + 12, 1);
            void *link = rrj_at(m, ordering_slot, 4);

            rrj_put16(xy, (uint16_t)screen_x);
            rrj_put16(xy + 2, (uint16_t)screen_y);
            color[0] = (uint8_t)(*(uint8_t *)rrj_at(m, record + 8, 1) * count / 25);
            color[1] = (uint8_t)(*(uint8_t *)rrj_at(m, record + 9, 1) * count / 25);
            color[2] = (uint8_t)(*(uint8_t *)rrj_at(m, record + 10, 1) * count / 25);
            if (kind == 0)
                (void)sub_8002B3FC(m, xy, color, (uint32_t)width, (uint32_t)height, 7, link);
            else if (kind == 1)
                (void)sub_8002B3FC(m, xy, color, (uint32_t)width, (uint32_t)height, 6, link);
            else if (kind == 2)
                (void)sub_8002B4A0(m, xy, color, (uint32_t)width, (uint32_t)height, link);
            else if (kind == 3)
                (void)sub_8002B5B4(m, xy, color, (uint32_t)width, (uint32_t)height, link);
            else if (kind == 4)
                (void)sub_8002B68C(m, xy, color, (uint32_t)width, (uint32_t)height, link);
        }
        ++index;
    }
    {
        uint32_t context = rrj_read32(m, 0x8005B470u);
        uint32_t packet = rrj_read32(m, context + 268);
        uint32_t link;

        if (packet + 12 >= rrj_read32(m, 0x8005B4D0u))
        {
            packet = sub_80021C98(m, packet, 12);
            rrj_write32(m, context + 268, packet);
        }
        rrj_write32(m, context + 268, packet + 12);
        link = rrj_read32(m, ordering_slot);
        rrj_write32(m, packet, (link & 0xFFFFFFu) | 0x02000000u);
        rrj_write32(m, ordering_slot, packet);
        rrj_write32(m, packet + 4, 0xE1000120u);
        rrj_write32(m, packet + 8, 0);
        return 0xE1000120u;
    }
}

uint32_t sub_8001034C(RRJMemory *m, uint32_t matrix_address, uint32_t count, uint32_t vertices, uint32_t records, uint32_t screens, uint32_t clips)
{
    MATRIX matrix;
    uint32_t index;

    FUNCTION_MARKER(0x8001034C, "SLUS_010.53");
    if (!count)
        return 0;
    memcpy(&matrix, rrj_at(m, matrix_address, sizeof(matrix)), sizeof(matrix));
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);
    for (index = 0; index != count; ++index)
    {
        uint32_t source = vertices + 8 * index;
        SVECTOR vertex;
        VECTOR transformed;
        sint32 screen;
        sint32 flags;
        uint8_t classification;

        vertex.vx = (int16_t)rrj_u16(rrj_at(m, source, 2));
        vertex.vy = (int16_t)rrj_u16(rrj_at(m, source + 2, 2));
        vertex.vz = (int16_t)rrj_u16(rrj_at(m, source + 4, 2));
        vertex.pad = (int16_t)rrj_u16(rrj_at(m, source + 6, 2));
        gte_transform(&vertex, &transformed, &flags);
        (void)gte_project(&vertex, &screen, &flags);
        rrj_write32(m, records + 16 * index + 8, (uint32_t)transformed.vz);
        rrj_write32(m, screens + 4 * index, (uint32_t)screen);
        classification = (uint8_t)((transformed.vz < 1024 ? 32 : 0) | (transformed.vz < 40 ? 16 : 0) | ((int16_t)(screen >> 16) > 240 ? 8 : 0) | ((uint16_t)screen > 384) + ((screen & 0x8000) != 0));
        *(uint8_t *)rrj_at(m, clips + index, 1) = classification;
    }
    return 0;
}

uint32_t sub_800104C8(RRJMemory *m, uint32_t matrix_address, uint32_t count, uint32_t vertices, uint32_t records, uint32_t screens)
{
    MATRIX matrix;
    uint32_t index;

    FUNCTION_MARKER(0x800104C8, "SLUS_010.53");
    if (!count)
        return 0;
    memcpy(&matrix, rrj_at(m, matrix_address, sizeof(matrix)), sizeof(matrix));
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);
    for (index = 0; index != count; ++index)
    {
        uint32_t source = vertices + 8 * index;
        SVECTOR vertex;
        VECTOR transformed;
        sint32 screen;
        sint32 flags;

        vertex.vx = (int16_t)rrj_u16(rrj_at(m, source, 2));
        vertex.vy = (int16_t)rrj_u16(rrj_at(m, source + 2, 2));
        vertex.vz = (int16_t)rrj_u16(rrj_at(m, source + 4, 2));
        vertex.pad = (int16_t)rrj_u16(rrj_at(m, source + 6, 2));
        gte_transform(&vertex, &transformed, &flags);
        (void)gte_project(&vertex, &screen, &flags);
        rrj_write32(m, screens + 4 * index, (uint32_t)screen);
        rrj_write32(m, records + 16 * index + 8, (uint32_t)transformed.vz);
    }
    return 0;
}

uint32_t sub_80068D50(RRJMemory *m, uint32_t object, uint32_t entry_index)
{
    uint32_t container = rrj_read32(m, object);
    uint32_t entry = rrj_read32(m, container + 36) + 8 * entry_index;
    uint32_t start = rrj_read32(m, entry);
    uint32_t count = rrj_read32(m, entry + 4);
    uint32_t index = start >> 3;
    uint32_t records = rrj_read32(m, 0x8005ACB0) + 16 * index;
    uint32_t screens = rrj_read32(m, 0x8005ACB4) + 4 * index;
    uint32_t clips = rrj_read32(m, 0x8005ACB8) + index;
    uint32_t vertices = rrj_read32(m, container + 52) + 8 * index + 4;
    uint32_t result;

    FUNCTION_MARKER(0x80068D50, "RASHCDG.BIN");
    rrj_write32(m, 0x800CCDB0, start);
    rrj_write32(m, 0x800CCDB4, count);
    rrj_write32(m, 0x800CCDB8, records);
    rrj_write32(m, 0x800CCDAC, screens);
    rrj_write32(m, 0x800CCDBC, clips);
    rrj_write32(m, 0x800CCDC0, vertices);
    (void)sub_8001034C(m, rrj_read32(m, 0x800CC870), count, vertices, records, screens, clips);
    result = rrj_read32(m, 0x800CCDA8) + count;
    rrj_write32(m, 0x800CCDA8, result);
    return result;
}

uint32_t sub_80068E2C(RRJMemory *m, uint32_t object, uint32_t entry_index)
{
    uint32_t container = rrj_read32(m, object);
    uint32_t entry = rrj_read32(m, container + 36) + 8 * entry_index;
    uint32_t start = rrj_read32(m, entry);
    uint32_t count = rrj_read32(m, entry + 4);
    uint32_t index = start >> 3;
    uint32_t vertices = rrj_read32(m, container + 52) + 8 * index + 4;
    uint32_t records = rrj_read32(m, 0x8005ACB0) + 16 * index;
    uint32_t screens = rrj_read32(m, 0x8005ACB4) + 4 * index;
    uint32_t result;

    FUNCTION_MARKER(0x80068E2C, "RASHCDG.BIN");
    (void)sub_800104C8(m, rrj_read32(m, 0x800CC870), count, vertices, records, screens);
    result = rrj_read32(m, 0x800CCDA8) + count;
    rrj_write32(m, 0x800CCDA8, result);
    return result;
}

uint32_t sub_80068EB8(RRJMemory *m, uint32_t object)
{
    uint32_t entry_index = 1;
    uint32_t shift = 0;
    uint32_t flags = rrj_read32(m, object + 52);
    uint32_t entry_count = *(uint8_t *)rrj_at(m, object + 68, 1);
    int32_t maximum;
    int32_t total;

    FUNCTION_MARKER(0x80068EB8, "RASHCDG.BIN");
    rrj_write32(m, 0x800CCDA8, 0);
    (void)sub_80068D50(m, object, 0);
    if ((flags & 0x22222222u) != 0 && entry_count > 1)
    {
        while (entry_index < entry_count)
        {
            uint32_t mode = (flags >> (shift & 31)) & (rrj_read32(m, 0x800CC86C) != 0 ? 0xDu : 0xFu);
            if ((mode & 2) != 0)
            {
                if ((mode & 4) != 0)
                    (void)sub_80068D50(m, object, entry_index);
                else
                    (void)sub_80068E2C(m, object, entry_index);
            }
            ++entry_index;
            shift += 4;
        }
    }
    maximum = (int32_t)rrj_read32(m, 0x8005B300);
    total = (int32_t)rrj_read32(m, 0x800CCDA8);
    if (maximum < total)
        rrj_write32(m, 0x8005B300, (uint32_t)total);
    return maximum < total;
}

uint32_t sub_80069200(RRJMemory *m, uint32_t object)
{
    uint32_t matrix = rrj_read32(m, 0x800CC870u) + 32;
    uint32_t index;

    FUNCTION_MARKER(0x80069200, "RASHCDG.BIN");
    rrj_write32(m, 0x800CC870u, matrix);
    for (index = 0; index != 8; ++index)
        rrj_write32(m, matrix + 4 * index, rrj_read32(m, object + 12 + 4 * index));
    (void)sub_80068EB8(m, object);
    rrj_write32(m, 0x800CC870u, matrix - 32);
    (void)sub_80068FCC(m, object);
    return 1;
}

uint32_t sub_80035958(RRJMemory *m, uint32_t player_index, RRJRaceLeafCall call)
{
    uint32_t far_indices[12];
    uint32_t near_indices[12];
    uint32_t far_count = 0;
    uint32_t near_count = 0;
    int32_t far_minimum = 0x4000;
    int32_t near_minimum = 0x4000;
    int32_t far_maximum = 0;
    int32_t near_maximum = 0;
    uint32_t object_count = rrj_read32(m, 0x8005B568u + 4 * player_index);
    uint32_t synchronized = 0;
    uint32_t index;
    uint32_t result = object_count;

    FUNCTION_MARKER(0x80035958, "SLUS_010.53");
    if (!object_count)
        return result;
    (void)sub_80032D24(m, player_index, call);
    for (index = object_count; index != 0; --index)
    {
        int32_t object_index = rrj_s32(rrj_read32(m, 0x800D9B80u + 48 * player_index + 4 * (index - 1)));
        uint32_t object;
        int32_t depth;
        int32_t distance;

        if (object_index == -1)
            continue;
        object = 0x800D87E8u + 112 * (uint32_t)object_index;
        depth = rrj_s32(rrj_read32(m, object + 48));
        if (depth <= 0)
            continue;
        distance = rrj_s32(rrj_read32(m, object + 52));
        if ((rrj_read32(m, object + 56) & 0x22222222u) != 0 && distance < 2048 && depth < 0x4000)
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
        (void)sub_80021988(m, far_minimum, far_maximum, call);
        (void)frontier_prepare_object_indices(m, far_indices, (int32_t)far_count, player_index, call);
        for (index = 0; index != far_count; ++index)
        {
            frontier_configure_model_scratch(m);
            (void)sub_80069200(m, 0x800D87ECu + 112 * far_indices[index]);
        }
        if (player_index == 0)
        {
            (void)frontier_call(m, call, 0x8001C408u, 0, 0, 0);
            synchronized = 1;
            if (rrj_read32(m, 0x8005B588u) == 1)
            {
                rrj_write32(m, 0x8005B588u, 2);
                (void)frontier_call(m, call, 0x80048DB4u, rrj_read32(m, 0x8005B59Cu) + 16, 0, 0);
            }
        }
        (void)frontier_call(m, call, 0x80048DB4u, rrj_read32(m, rrj_read32(m, 0x8005B470u) + 264) + 4 * rrj_read32(m, 0x8005ADFCu) - 4, 0, 0);
        result = frontier_call(m, call, 0x80048ACCu, 0x8005B398u, 0x800D75D0u + 64 * rrj_read32(m, 0x8005AE00u), 0);
    }
    if (near_count != 0)
    {
        (void)sub_80021988(m, near_minimum, near_maximum, call);
        (void)frontier_prepare_object_indices(m, near_indices, (int32_t)near_count, player_index, call);
        for (index = 0; index != near_count; ++index)
        {
            frontier_configure_model_scratch(m);
            (void)sub_80069200(m, 0x800D87ECu + 112 * near_indices[index]);
        }
        if (player_index == 0 && !synchronized)
        {
            (void)frontier_call(m, call, 0x8001C408u, 0, 0, 0);
            if (rrj_read32(m, 0x8005B588u) == 1)
            {
                rrj_write32(m, 0x8005B588u, 2);
                (void)frontier_call(m, call, 0x80048DB4u, rrj_read32(m, 0x8005B59Cu) + 16, 0, 0);
            }
        }
        (void)frontier_call(m, call, 0x80048DB4u, rrj_read32(m, rrj_read32(m, 0x8005B470u) + 264) + 4 * rrj_read32(m, 0x8005ADFCu) - 4, 0, 0);
        result = frontier_call(m, call, 0x80048ACCu, 0x8005B398u, 0x800D75D0u + 64 * rrj_read32(m, 0x8005AE00u), 0);
    }
    return result;
}

uint32_t sub_8006929C(RRJMemory *m, uint32_t base_index, uint32_t packed_indices, uint32_t depth_shift)
{
    static const uint32_t first_shifts[3] = {8, 16, 0};
    static const uint32_t second_shifts[3] = {0, 8, 16};
    uint8_t indices[6];
    uint32_t midpoint;
    uint32_t triangle;

    FUNCTION_MARKER(0x8006929C, "RASHCDG.BIN");
    for (midpoint = 0; midpoint != 3; ++midpoint)
    {
        uint32_t first_index = (packed_indices >> first_shifts[midpoint]) & 0xFFu;
        uint32_t second_index = (packed_indices >> second_shifts[midpoint]) & 0xFFu;
        uint32_t first = 0x1F800100u + 32 * first_index;
        uint32_t second = 0x1F800100u + 32 * second_index;
        uint32_t output = 0x1F800100u + 32 * (base_index + midpoint);
        uint32_t first_color = rrj_read32(m, first + 16);
        uint32_t second_color = rrj_read32(m, second + 16);
        int32_t depth = rrj_s32(frontier_asr(rrj_read32(m, first + 20) + rrj_read32(m, second + 20), 1));
        int32_t x = rrj_s32(frontier_asr(rrj_read32(m, first) + rrj_read32(m, second), 1));
        int32_t y = rrj_s32(frontier_asr(rrj_read32(m, first + 4) + rrj_read32(m, second + 4), 1));
        int32_t z = rrj_s32(frontier_asr(rrj_read32(m, first + 8) + rrj_read32(m, second + 8), 1));
        int32_t scale = rrj_s32(frontier_asr((uint32_t)z + 128u, 8));
        uint32_t first_uv = rrj_read32(m, first + 24);
        uint32_t second_uv = rrj_read32(m, second + 24);
        int32_t delta_x = (int16_t)first_uv - (int16_t)second_uv;
        int32_t delta_y = (first_uv >> 16) - (second_uv >> 16);
        uint32_t abs_x = frontier_abs(delta_x);
        uint32_t abs_y = frontier_abs(delta_y);
        uint32_t sign_x = 0u - ((uint32_t)delta_x >> 31);
        uint32_t sign_y = 0u - ((uint32_t)delta_y >> 31);
        uint32_t y_mask = frontier_asr(abs_y - 2u * abs_x, 31);
        uint32_t x_mask = frontier_asr(abs_x - 2u * abs_y, 31);
        uint32_t y_adjust = (((2u * (uint32_t)scale) & sign_x) - (uint32_t)scale) & y_mask;
        uint32_t x_adjust = ((uint32_t)scale + ((0u - 2u * (uint32_t)scale) & sign_y)) & x_mask;
        uint32_t screen;
        uint32_t clip;
        int32_t screen_y;

        rrj_write32(m, output + 16, (((first_color & 0xFFFFFEFFu) + (second_color & 0xFFFFFEFFu)) >> 1) + ((first_color & second_color) & 0x100u));
        rrj_write32(m, output + 20, (uint32_t)depth);
        rrj_write32(m, output + 28, (rrj_read32(m, output + 28) & 0xFF000000u) | (rrj_read32(m, 0x800D4CA8u + 4 * ((uint32_t)depth & 0xFFu)) & 0xFFFFFFu));
        x = rrj_s32((uint32_t)x + x_adjust);
        y = rrj_s32((uint32_t)y + y_adjust);
        rrj_write32(m, output, (uint32_t)x);
        rrj_write32(m, output + 4, (uint32_t)y);
        rrj_write32(m, output + 8, (uint32_t)z);
        screen = frontier_project_translation(x >> 5, y >> 5, z >> 5);
        rrj_write32(m, output + 24, screen);
        if (z > 102399)
            clip = 1u << 5;
        else if (z > 51199)
            clip = 1u << 6;
        else
            clip = 1u << 7;
        if (z < 10240)
            clip |= 0x10u;
        screen_y = rrj_s32(screen) >> 16;
        if (screen_y < 0)
            clip |= 8u;
        if (screen_y >= 241)
            clip |= 4u;
        if ((uint16_t)screen < 385u)
            clip |= (screen >> 15) & 1u;
        else
            clip |= ((screen >> 15) & 1u) + 1u;
        *(uint8_t *)rrj_at(m, output + 31, 1) = (uint8_t)clip;
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
        uint32_t selectors = rrj_read32(m, 0x800CCA30u + 4 * triangle);
        uint32_t first_index = indices[selectors >> 8];
        uint32_t second_index = indices[(selectors >> 4) & 0xFu];
        uint32_t third_index = indices[selectors & 0xFu];
        uint32_t first = 0x1F800100u + 32 * first_index;
        uint32_t second = 0x1F800100u + 32 * second_index;
        uint32_t third = 0x1F800100u + 32 * third_index;
        uint32_t first_clip = *(uint8_t *)rrj_at(m, first + 31, 1);
        uint32_t second_clip = *(uint8_t *)rrj_at(m, second + 31, 1);
        uint32_t third_clip = *(uint8_t *)rrj_at(m, third + 31, 1);
        uint32_t combined = first_clip | second_clip | third_clip;

        if ((first_clip & second_clip & third_clip & 0x1Fu) != 0)
            continue;
        if ((combined >> (depth_shift & 31u)) != 0 || (rrj_s32(base_index) < 12 && (combined & 0x10u) != 0))
        {
            (void)sub_8006929C(m, base_index + 3, first_index | (second_index << 8) | (third_index << 16), depth_shift);
        }
        else
        {
            uint32_t packet = rrj_read32(m, 0x1F800020u);
            if (packet + 40u >= rrj_read32(m, 0x8005B4D0u))
                packet = sub_80021C98(m, packet, 40);
            rrj_write32(m, packet, rrj_read32(m, 0x1F800028u) | 0x09000000u);
            rrj_write32(m, packet + 4, (rrj_read32(m, first + 28) & 0xFFFFFFu) | 0x34000000u);
            rrj_write32(m, packet + 8, rrj_read32(m, first + 24));
            rrj_write32(m, packet + 12, rrj_read32(m, first + 16) | rrj_read32(m, 0x1F800034u));
            rrj_write32(m, packet + 16, rrj_read32(m, second + 28) & 0xFFFFFFu);
            rrj_write32(m, packet + 20, rrj_read32(m, second + 24));
            rrj_write32(m, packet + 24, rrj_read32(m, second + 16) | rrj_read32(m, 0x1F800038u));
            rrj_write32(m, packet + 28, rrj_read32(m, third + 28) & 0xFFFFFFu);
            rrj_write32(m, packet + 32, rrj_read32(m, third + 24));
            rrj_write32(m, 0x1F800028u, packet);
            rrj_write32(m, packet + 36, rrj_read32(m, third + 16));
            rrj_write32(m, 0x1F800020u, packet + 40);
        }
    }
    return 0;
}

uint32_t sub_80069784(RRJMemory *m, uint32_t base_index, uint32_t packed_indices, uint32_t depth_shift)
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
        frontier_build_midpoint(m, base_index + (uint32_t)midpoint, indices[first_selectors[4 - midpoint]], indices[second_selectors[4 - midpoint]], 128, midpoint != 0);

    indices[4] = (uint8_t)base_index;
    indices[5] = (uint8_t)(base_index + 1);
    indices[6] = (uint8_t)(base_index + 2);
    indices[7] = (uint8_t)(base_index + 3);
    indices[8] = (uint8_t)(base_index + 4);
    ++depth_shift;
    for (triangle = 0; triangle != 4; ++triangle)
    {
        uint32_t selectors = rrj_read32(m, 0x800CCA40u + 4 * triangle);
        uint32_t first_index = indices[selectors >> 12];
        uint32_t second_index = indices[(selectors >> 8) & 0xFu];
        uint32_t third_index = indices[(selectors >> 4) & 0xFu];
        uint32_t fourth_index = indices[selectors & 0xFu];
        uint32_t first = 0x1F800100u + 32 * first_index;
        uint32_t second = 0x1F800100u + 32 * second_index;
        uint32_t third = 0x1F800100u + 32 * third_index;
        uint32_t fourth = 0x1F800100u + 32 * fourth_index;
        uint32_t first_clip = *(uint8_t *)rrj_at(m, first + 31, 1);
        uint32_t second_clip = *(uint8_t *)rrj_at(m, second + 31, 1);
        uint32_t third_clip = *(uint8_t *)rrj_at(m, third + 31, 1);
        uint32_t fourth_clip = *(uint8_t *)rrj_at(m, fourth + 31, 1);
        uint32_t combined = first_clip | second_clip | third_clip | fourth_clip;

        if ((first_clip & second_clip & third_clip & fourth_clip & 0x1Fu) != 0)
            continue;
        if ((combined >> (depth_shift & 31u)) != 0 || (rrj_s32(base_index) < 19 && (combined & 0x10u) != 0))
        {
            (void)sub_80069784(m, base_index + 5, first_index | (second_index << 8) | (third_index << 16) | (fourth_index << 24), depth_shift);
        }
        else
        {
            uint32_t packet = rrj_read32(m, 0x1F800020u);
            if (packet + 52u >= rrj_read32(m, 0x8005B4D0u))
                packet = sub_80021C98(m, packet, 52);
            rrj_write32(m, packet, rrj_read32(m, 0x1F800028u) | 0x0C000000u);
            rrj_write32(m, packet + 4, (rrj_read32(m, first + 28) & 0xFFFFFFu) | 0x3C000000u);
            rrj_write32(m, packet + 8, rrj_read32(m, first + 24));
            rrj_write32(m, packet + 12, rrj_read32(m, first + 16) | rrj_read32(m, 0x1F800034u));
            rrj_write32(m, packet + 16, rrj_read32(m, second + 28) & 0xFFFFFFu);
            rrj_write32(m, packet + 20, rrj_read32(m, second + 24));
            rrj_write32(m, packet + 24, rrj_read32(m, second + 16) | rrj_read32(m, 0x1F800038u));
            rrj_write32(m, packet + 28, rrj_read32(m, fourth + 28) & 0xFFFFFFu);
            rrj_write32(m, packet + 32, rrj_read32(m, fourth + 24));
            rrj_write32(m, packet + 36, rrj_read32(m, fourth + 16));
            rrj_write32(m, packet + 40, rrj_read32(m, third + 28) & 0xFFFFFFu);
            rrj_write32(m, packet + 44, rrj_read32(m, third + 24));
            rrj_write32(m, 0x1F800028u, packet);
            rrj_write32(m, packet + 48, rrj_read32(m, third + 16));
            rrj_write32(m, 0x1F800020u, packet + 52);
        }
    }
    return 0;
}

uint32_t sub_80069CF0(RRJMemory *m, uint32_t base_index, uint32_t packed_indices, uint32_t depth_shift)
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
        frontier_build_midpoint(m, base_index + (uint32_t)midpoint, indices[first_selectors[4 - midpoint]], indices[second_selectors[4 - midpoint]], 64, midpoint != 0);

    indices[4] = (uint8_t)base_index;
    indices[5] = (uint8_t)(base_index + 1);
    indices[6] = (uint8_t)(base_index + 2);
    indices[7] = (uint8_t)(base_index + 3);
    indices[8] = (uint8_t)(base_index + 4);
    ++depth_shift;
    for (triangle = 0; triangle != 4; ++triangle)
    {
        uint32_t selectors = rrj_read32(m, 0x800CCA40u + 4 * triangle);
        uint32_t first_index = indices[selectors >> 12];
        uint32_t second_index = indices[(selectors >> 8) & 0xFu];
        uint32_t third_index = indices[(selectors >> 4) & 0xFu];
        uint32_t fourth_index = indices[selectors & 0xFu];
        uint32_t first = 0x1F800100u + 32 * first_index;
        uint32_t second = 0x1F800100u + 32 * second_index;
        uint32_t third = 0x1F800100u + 32 * third_index;
        uint32_t fourth = 0x1F800100u + 32 * fourth_index;
        uint32_t first_clip = *(uint8_t *)rrj_at(m, first + 31, 1);
        uint32_t second_clip = *(uint8_t *)rrj_at(m, second + 31, 1);
        uint32_t third_clip = *(uint8_t *)rrj_at(m, third + 31, 1);
        uint32_t fourth_clip = *(uint8_t *)rrj_at(m, fourth + 31, 1);
        uint32_t combined = first_clip | second_clip | third_clip | fourth_clip;

        if ((first_clip & second_clip & third_clip & fourth_clip & 0x1Fu) != 0)
            continue;
        if ((combined >> (depth_shift & 31u)) != 0 || (rrj_s32(base_index) < 14 && (combined & 0x10u) != 0))
        {
            (void)sub_80069CF0(m, base_index + 5, first_index | (second_index << 8) | (third_index << 16) | (fourth_index << 24), depth_shift);
        }
        else
        {
            uint32_t packet = rrj_read32(m, 0x1F800020u);
            if (packet + 52u >= rrj_read32(m, 0x8005B4D0u))
                packet = sub_80021C98(m, packet, 52);
            rrj_write32(m, packet, rrj_read32(m, 0x1F800028u) | 0x0C000000u);
            rrj_write32(m, packet + 4, (rrj_read32(m, first + 28) & 0xFFFFFFu) | 0x3C000000u);
            rrj_write32(m, packet + 8, rrj_read32(m, first + 24));
            rrj_write32(m, packet + 12, rrj_read32(m, first + 16) | rrj_read32(m, 0x1F800034u));
            rrj_write32(m, packet + 16, rrj_read32(m, second + 28) & 0xFFFFFFu);
            rrj_write32(m, packet + 20, rrj_read32(m, second + 24));
            rrj_write32(m, packet + 24, rrj_read32(m, second + 16) | rrj_read32(m, 0x1F800038u));
            rrj_write32(m, packet + 28, rrj_read32(m, fourth + 28) & 0xFFFFFFu);
            rrj_write32(m, packet + 32, rrj_read32(m, fourth + 24));
            rrj_write32(m, packet + 36, rrj_read32(m, fourth + 16));
            rrj_write32(m, packet + 40, rrj_read32(m, third + 28) & 0xFFFFFFu);
            rrj_write32(m, packet + 44, rrj_read32(m, third + 24));
            rrj_write32(m, 0x1F800028u, packet);
            rrj_write32(m, packet + 48, rrj_read32(m, third + 16));
            rrj_write32(m, 0x1F800020u, packet + 52);
        }
    }
    return 0;
}

uint32_t sub_8006A25C(RRJMemory *m, uint32_t base_index, uint32_t packed_indices, uint32_t packet_color)
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
        uint32_t z_sum = rrj_read32(m, first + 8) + rrj_read32(m, second + 8);
        int32_t depth = rrj_s32(frontier_asr(rrj_read32(m, first + 20) + rrj_read32(m, second + 20), 1));
        int32_t x = rrj_s32(frontier_asr(rrj_read32(m, first) + rrj_read32(m, second), 1));
        int32_t y = rrj_s32(frontier_asr(rrj_read32(m, first + 4) + rrj_read32(m, second + 4), 1));
        int32_t z = rrj_s32(frontier_asr(z_sum, 1));
        uint32_t screen;
        uint32_t clip;
        int32_t screen_y;

        rrj_write32(m, output + 20, (uint32_t)depth);
        rrj_write32(m, output + 28, (rrj_read32(m, output + 28) & 0xFF000000u) | (rrj_read32(m, 0x800D4CA8u + 4 * ((uint32_t)depth & 0xFFu)) & 0xFFFFFFu));
        rrj_write32(m, output, (uint32_t)x);
        rrj_write32(m, output + 4, (uint32_t)y);
        rrj_write32(m, output + 8, (uint32_t)z);
        screen = frontier_project_translation(x >> 5, y >> 5, rrj_s32(frontier_asr(z_sum, 6)));
        rrj_write32(m, output + 24, screen);
        if (z > 102399)
            clip = 1u << 5;
        else if (z > 51199)
            clip = 1u << 6;
        else
            clip = 1u << 7;
        if (z < 10240)
            clip |= 0x10u;
        screen_y = rrj_s32(screen) >> 16;
        if (screen_y < 0)
            clip |= 8u;
        if (screen_y >= 241)
            clip |= 4u;
        if ((uint16_t)screen < 385u)
            clip |= (screen >> 15) & 1u;
        else
            clip |= ((screen >> 15) & 1u) + 1u;
        *(uint8_t *)rrj_at(m, output + 31, 1) = (uint8_t)clip;
    }

    indices[0] = (uint8_t)packed_indices;
    indices[1] = (uint8_t)(packed_indices >> 8);
    indices[2] = (uint8_t)(packed_indices >> 16);
    indices[3] = (uint8_t)(packed_indices >> 24);
    indices[4] = (uint8_t)base_index;
    indices[5] = (uint8_t)(base_index + 1);
    for (quad = 0; quad != 2; ++quad)
    {
        uint32_t selectors = rrj_read32(m, 0x800CCA50u + 4 * quad);
        uint32_t first_index = indices[selectors >> 12];
        uint32_t second_index = indices[(selectors >> 8) & 0xFu];
        uint32_t third_index = indices[(selectors >> 4) & 0xFu];
        uint32_t fourth_index = indices[selectors & 0xFu];
        uint32_t first = 0x1F800100u + 32 * first_index;
        uint32_t second = 0x1F800100u + 32 * second_index;
        uint32_t third = 0x1F800100u + 32 * third_index;
        uint32_t fourth = 0x1F800100u + 32 * fourth_index;
        uint32_t first_clip = *(uint8_t *)rrj_at(m, first + 31, 1);
        uint32_t second_clip = *(uint8_t *)rrj_at(m, second + 31, 1);
        uint32_t third_clip = *(uint8_t *)rrj_at(m, third + 31, 1);
        uint32_t fourth_clip = *(uint8_t *)rrj_at(m, fourth + 31, 1);
        uint32_t combined = first_clip | second_clip | third_clip | fourth_clip;

        if ((first_clip & second_clip & third_clip & fourth_clip & 0x1Fu) != 0)
            continue;
        if ((combined & 4u) != 0 && rrj_s32(base_index) < 14)
        {
            (void)sub_8006A25C(m, base_index + 2, first_index | (second_index << 8) | (third_index << 16) | (fourth_index << 24), packet_color);
        }
        else
        {
            uint32_t packet = rrj_read32(m, 0x1F800020u);
            if (packet + 24u >= rrj_read32(m, 0x8005B4D0u))
                packet = sub_80021C98(m, packet, 24);
            rrj_write32(m, packet, rrj_read32(m, 0x1F800028u) | 0x05000000u);
            rrj_write32(m, packet + 4, packet_color | 0x28000000u);
            rrj_write32(m, packet + 8, rrj_read32(m, first + 24));
            rrj_write32(m, packet + 12, rrj_read32(m, second + 24));
            rrj_write32(m, packet + 16, rrj_read32(m, fourth + 24));
            rrj_write32(m, 0x1F800028u, packet);
            rrj_write32(m, packet + 20, rrj_read32(m, third + 24));
            rrj_write32(m, 0x1F800020u, packet + 24);
        }
    }
    return 0;
}

static uint32_t frontier_emit_textured_faces(RRJMemory *m, uint32_t object, uint32_t group_index, int reject_common_clip)
{
    uint32_t descriptor = rrj_read32(m, object);
    uint32_t group = rrj_read32(m, descriptor + 40) + 12 * group_index;
    uint32_t triangle_count = rrj_read32(m, group + 4);
    uint32_t face = rrj_read32(m, descriptor + 56) + rrj_read32(m, group);
    uint32_t context = rrj_read32(m, 0x8005B470u);
    uint32_t ordering_table = rrj_read32(m, context + 264);
    uint32_t cursor = rrj_read32(m, context + 268);
    uint32_t index;

    FUNCTION_MARKER(0x8006D350, "RASHCDG.BIN");
    rrj_write32(m, 0x800CCE2Cu, ordering_table);
    rrj_write32(m, 0x800CCE34u, rrj_read32(m, descriptor + 52) + 4);
    rrj_write32(m, 0x1F800020u, cursor);
    if (cursor + 40 * triangle_count >= rrj_read32(m, 0x8005B4D0u))
    {
        cursor = sub_80021C98(m, cursor, 40 * triangle_count);
        rrj_write32(m, 0x1F800020u, cursor);
    }
    for (index = 0; index != triangle_count; ++index, face += 20)
    {
        uint32_t flags = rrj_read32(m, face);
        uint32_t packed_indices = rrj_read32(m, face + 16);
        uint32_t first_index = rrj_read32(m, face + 12) >> 16;
        uint32_t second_index = packed_indices & 0xFFFFu;
        uint32_t third_index = packed_indices >> 16;
        uint32_t clips = rrj_read32(m, 0x8005ACB8u);
        uint32_t screens = rrj_read32(m, 0x8005ACB4u);
        uint32_t depths = rrj_read32(m, 0x8005ACB0u);
        int32_t depth;
        int32_t other;
        int32_t bucket;
        uint32_t slot;
        uint32_t packet;

        rrj_write32(m, 0x1F80003Cu, flags);
        rrj_write32(m, 0x1F800048u, rrj_read32(m, face + 12));
        rrj_write32(m, 0x1F80004Cu, packed_indices);
        if (reject_common_clip && (*(uint8_t *)rrj_at(m, clips + first_index, 1) & *(uint8_t *)rrj_at(m, clips + second_index, 1) & *(uint8_t *)rrj_at(m, clips + third_index, 1) & 0x1Fu) != 0)
            continue;
        rrj_write32(m, 0x1F800040u, rrj_read32(m, face + 4));
        rrj_write32(m, 0x1F800044u, rrj_read32(m, face + 8));
        if ((flags & 4u) == 0 && NormalClip((sint32)rrj_read32(m, screens + 4 * first_index), (sint32)rrj_read32(m, screens + 4 * second_index), (sint32)rrj_read32(m, screens + 4 * third_index)) < 0)
            continue;
        depth = rrj_s32(rrj_read32(m, depths + 16 * first_index + 8));
        other = rrj_s32(rrj_read32(m, depths + 16 * second_index + 8));
        if (other > depth)
            depth = other;
        other = rrj_s32(rrj_read32(m, depths + 16 * third_index + 8));
        if (other > depth)
            depth = other;
        bucket = frontier_depth_bucket(m, depth);
        slot = ordering_table + 4 * (uint32_t)bucket;
        packet = rrj_read32(m, 0x1F800020u);
        rrj_write32(m, packet, rrj_read32(m, slot) | 0x09000000u);
        rrj_write32(m, packet + 4, rrj_read32(m, 0x800CC994u + 4 * ((flags & 0xFFu) >> 3)));
        rrj_write32(m, packet + 8, rrj_read32(m, 0x800D4CA8u + 4 * ((flags >> 16) & 0xFFu)) | 0x24000000u);
        rrj_write32(m, packet + 12, rrj_read32(m, screens + 4 * first_index));
        rrj_write32(m, packet + 16, rrj_read32(m, 0x1F800040u));
        rrj_write32(m, packet + 20, rrj_read32(m, screens + 4 * second_index));
        rrj_write32(m, packet + 24, rrj_read32(m, 0x1F800044u));
        rrj_write32(m, packet + 28, rrj_read32(m, screens + 4 * third_index));
        rrj_write32(m, packet + 32, rrj_read32(m, 0x1F800048u));
        rrj_write32(m, packet + 36, 0xE2000000u);
        rrj_write32(m, 0x1F800020u, packet + 40);
        rrj_write32(m, slot, packet);
    }

    {
        uint32_t quad_count = rrj_read32(m, group + 8);
        cursor = rrj_read32(m, 0x1F800020u);
        if (cursor + 48 * quad_count >= rrj_read32(m, 0x8005B4D0u))
        {
            cursor = sub_80021C98(m, cursor, 48 * quad_count);
            rrj_write32(m, 0x1F800020u, cursor);
        }
        for (index = 0; index != quad_count; ++index, face += 24)
        {
            uint32_t flags = rrj_read32(m, face);
            uint32_t packed_uv = rrj_read32(m, face + 12);
            uint32_t first_pair = rrj_read32(m, face + 16);
            uint32_t second_pair = rrj_read32(m, face + 20);
            uint32_t first_index = first_pair & 0xFFFFu;
            uint32_t second_index = first_pair >> 16;
            uint32_t third_index = second_pair & 0xFFFFu;
            uint32_t fourth_index = second_pair >> 16;
            uint32_t clips = rrj_read32(m, 0x8005ACB8u);
            uint32_t screens = rrj_read32(m, 0x8005ACB4u);
            uint32_t depths = rrj_read32(m, 0x8005ACB0u);
            int32_t depth;
            int32_t other;
            int32_t bucket;
            uint32_t slot;
            uint32_t packet;

            rrj_write32(m, 0x1F80003Cu, flags);
            rrj_write32(m, 0x1F80004Cu, first_pair);
            rrj_write32(m, 0x1F800050u, second_pair);
            if (reject_common_clip && (*(uint8_t *)rrj_at(m, clips + first_index, 1) & *(uint8_t *)rrj_at(m, clips + second_index, 1) & *(uint8_t *)rrj_at(m, clips + third_index, 1) & *(uint8_t *)rrj_at(m, clips + fourth_index, 1) & 0x1Fu) != 0)
                continue;
            rrj_write32(m, 0x1F800040u, rrj_read32(m, face + 4));
            rrj_write32(m, 0x1F800044u, rrj_read32(m, face + 8));
            rrj_write32(m, 0x1F800048u, packed_uv);
            if ((flags & 4u) == 0)
            {
                int32_t first_clip = NormalClip((sint32)rrj_read32(m, screens + 4 * first_index), (sint32)rrj_read32(m, screens + 4 * third_index), (sint32)rrj_read32(m, screens + 4 * second_index));
                if (first_clip < 0 && NormalClip((sint32)rrj_read32(m, screens + 4 * first_index), (sint32)rrj_read32(m, screens + 4 * fourth_index), (sint32)rrj_read32(m, screens + 4 * second_index)) > 0)
                    continue;
            }
            depth = rrj_s32(rrj_read32(m, depths + 16 * first_index + 8));
            other = rrj_s32(rrj_read32(m, depths + 16 * second_index + 8));
            if (other > depth)
                depth = other;
            other = rrj_s32(rrj_read32(m, depths + 16 * third_index + 8));
            if (other > depth)
                depth = other;
            other = rrj_s32(rrj_read32(m, depths + 16 * fourth_index + 8));
            if (other > depth)
                depth = other;
            bucket = frontier_depth_bucket(m, depth);
            slot = ordering_table + 4 * (uint32_t)bucket;
            packet = rrj_read32(m, 0x1F800020u);
            rrj_write32(m, packet, rrj_read32(m, slot) | 0x0B000000u);
            rrj_write32(m, packet + 4, rrj_read32(m, 0x800CC994u + 4 * ((flags & 0xFFu) >> 3)));
            rrj_write32(m, packet + 8, rrj_read32(m, 0x800D4CA8u + 4 * ((flags >> 16) & 0xFFu)) | 0x2C000000u);
            rrj_write32(m, packet + 12, rrj_read32(m, screens + 4 * first_index));
            rrj_write32(m, packet + 16, rrj_read32(m, 0x1F800040u));
            rrj_write32(m, packet + 20, rrj_read32(m, screens + 4 * second_index));
            rrj_write32(m, packet + 24, rrj_read32(m, 0x1F800044u));
            rrj_write32(m, packet + 28, rrj_read32(m, screens + 4 * fourth_index));
            rrj_write32(m, packet + 32, packed_uv >> 16);
            rrj_write32(m, packet + 36, rrj_read32(m, screens + 4 * third_index));
            rrj_write32(m, packet + 40, packed_uv & 0xFFFFu);
            rrj_write32(m, packet + 44, 0xE2000000u);
            rrj_write32(m, 0x1F800020u, packet + 48);
            rrj_write32(m, slot, packet);
        }
    }
    cursor = rrj_read32(m, 0x1F800020u);
    rrj_write32(m, context + 268, cursor);
    return cursor;
}

uint32_t sub_8006D350(RRJMemory *m, uint32_t object, uint32_t group_index)
{
    return frontier_emit_textured_faces(m, object, group_index, 1);
}

uint32_t sub_8006DC20(RRJMemory *m, uint32_t object, uint32_t group_index)
{
    FUNCTION_MARKER(0x8006DC20, "RASHCDG.BIN");
    return frontier_emit_textured_faces(m, object, group_index, 0);
}

uint32_t sub_8006A630(RRJMemory *m, uint32_t object, uint32_t group_index)
{
    uint32_t descriptor = rrj_read32(m, object);
    uint32_t group = rrj_read32(m, descriptor + 40) + 12 * group_index;
    uint32_t triangle_count = rrj_read32(m, group + 4);
    uint32_t face = rrj_read32(m, descriptor + 60) + rrj_read32(m, group);
    uint32_t context = rrj_read32(m, 0x8005B470u);
    uint32_t ordering_table = rrj_read32(m, context + 264);
    uint32_t cursor = rrj_read32(m, context + 268);
    uint32_t color_table = rrj_read32(m, descriptor + 52) + 4;
    uint32_t screens = rrj_read32(m, 0x8005ACB4u);
    uint32_t depths = rrj_read32(m, 0x8005ACB0u);
    uint32_t clips = rrj_read32(m, 0x8005ACB8u);
    MATRIX matrix;
    uint32_t item;
    uint32_t index;

    FUNCTION_MARKER(0x8006A630, "RASHCDG.BIN");
    rrj_write32(m, 0x1F800020u, cursor);
    rrj_write32(m, 0x800CCE2Cu, ordering_table);
    rrj_write32(m, 0x800CCE28u, color_table);
    rrj_write32(m, 0x800CCE34u, color_table);
    ReadRotMatrix(&matrix);
    for (item = 0; item != triangle_count; ++item, face += 20)
    {
        uint32_t flags = rrj_read32(m, face);
        uint32_t packed_indices = rrj_read32(m, face + 16);
        uint32_t indices[3];
        uint32_t clip_union;
        uint32_t clip_intersection;
        int32_t depth;
        int32_t bucket;
        uint32_t slot;

        rrj_write32(m, 0x1F80003Cu, flags);
        rrj_write32(m, 0x1F800040u, rrj_read32(m, face + 4));
        rrj_write32(m, 0x1F800044u, rrj_read32(m, face + 8));
        rrj_write32(m, 0x1F800048u, rrj_read32(m, face + 12));
        rrj_write32(m, 0x1F80004Cu, packed_indices);
        indices[0] = rrj_read32(m, face + 12) >> 16;
        indices[1] = packed_indices & 0xFFFFu;
        indices[2] = packed_indices >> 16;
        clip_union = *(uint8_t *)rrj_at(m, clips + indices[0], 1) | *(uint8_t *)rrj_at(m, clips + indices[1], 1) | *(uint8_t *)rrj_at(m, clips + indices[2], 1);
        clip_intersection = *(uint8_t *)rrj_at(m, clips + indices[0], 1) & *(uint8_t *)rrj_at(m, clips + indices[1], 1) & *(uint8_t *)rrj_at(m, clips + indices[2], 1);
        if ((clip_intersection & 0x1Fu) != 0)
            continue;
        if ((flags & 4u) == 0 && NormalClip((sint32)rrj_read32(m, screens + 4 * indices[0]), (sint32)rrj_read32(m, screens + 4 * indices[1]), (sint32)rrj_read32(m, screens + 4 * indices[2])) < 0)
            continue;
        depth = rrj_s32(rrj_read32(m, depths + 16 * indices[0] + 8));
        if (rrj_s32(rrj_read32(m, depths + 16 * indices[1] + 8)) > depth)
            depth = rrj_s32(rrj_read32(m, depths + 16 * indices[1] + 8));
        if (rrj_s32(rrj_read32(m, depths + 16 * indices[2] + 8)) > depth)
            depth = rrj_s32(rrj_read32(m, depths + 16 * indices[2] + 8));
        bucket = frontier_depth_bucket(m, depth);
        slot = ordering_table + 4 * (uint32_t)bucket;
        if ((clip_union & 0x20u) != 0)
        {
            frontier_prepare_source_record(m, &matrix, 0, indices[0], rrj_u16(rrj_at(m, 0x1F800040u, 2)));
            frontier_prepare_source_record(m, &matrix, 1, indices[1], rrj_u16(rrj_at(m, 0x1F800044u, 2)));
            frontier_prepare_source_record(m, &matrix, 2, indices[2], rrj_u16(rrj_at(m, 0x1F800048u, 2)));
            rrj_write32(m, 0x1F800034u, rrj_u16(rrj_at(m, 0x1F800042u, 2)) << 16);
            rrj_write32(m, 0x1F800038u, rrj_u16(rrj_at(m, 0x1F800046u, 2)) << 16);
            frontier_render_wrapped_subdivision(m, slot, flags, 0);
            cursor = rrj_read32(m, 0x1F800020u);
        }
        else
        {
            uint32_t packet;
            uint32_t shade;

            if (cursor + 48 >= rrj_read32(m, 0x8005B4D0u))
                cursor = sub_80021C98(m, cursor, 48);
            packet = cursor;
            rrj_write32(m, packet, rrj_read32(m, slot) | 0x0B000000u);
            rrj_write32(m, packet + 4, rrj_read32(m, 0x1F800078u + 4 * ((flags & 0xFFu) >> 4)));
            shade = *(uint8_t *)rrj_at(m, color_table + 8 * indices[0] + 6, 1);
            rrj_write32(m, packet + 8, rrj_read32(m, 0x800D4CA8u + 4 * shade) | 0x34000000u);
            rrj_write32(m, packet + 12, rrj_read32(m, screens + 4 * indices[0]));
            rrj_write32(m, packet + 16, rrj_read32(m, 0x1F800040u));
            shade = *(uint8_t *)rrj_at(m, color_table + 8 * indices[1] + 6, 1);
            rrj_write32(m, packet + 20, rrj_read32(m, 0x800D4CA8u + 4 * shade));
            rrj_write32(m, packet + 24, rrj_read32(m, screens + 4 * indices[1]));
            rrj_write32(m, packet + 28, rrj_read32(m, 0x1F800044u));
            shade = *(uint8_t *)rrj_at(m, color_table + 8 * indices[2] + 6, 1);
            rrj_write32(m, packet + 32, rrj_read32(m, 0x800D4CA8u + 4 * shade));
            rrj_write32(m, packet + 36, rrj_read32(m, screens + 4 * indices[2]));
            rrj_write32(m, packet + 40, rrj_read32(m, 0x1F800048u));
            rrj_write32(m, packet + 44, 0xE2000000u);
            cursor = packet + 48;
            rrj_write32(m, 0x1F800020u, cursor);
            rrj_write32(m, slot, packet);
        }
    }

    {
        uint32_t quad_count = rrj_read32(m, group + 8);

        if (quad_count != 0)
            face += 24 * (quad_count - 1);
        for (item = 0; item != quad_count; ++item, face -= 24)
        {
            uint32_t flags = rrj_read32(m, face);
            uint32_t first_pair = rrj_read32(m, face + 16);
            uint32_t second_pair = rrj_read32(m, face + 20);
            uint32_t indices[4];
            uint32_t clip_values[4];
            uint32_t clip_union;
            uint32_t clip_intersection;
            int32_t depth;
            int32_t bucket;
            uint32_t slot;

            rrj_write32(m, 0x1F80003Cu, flags);
            rrj_write32(m, 0x1F800040u, rrj_read32(m, face + 4));
            rrj_write32(m, 0x1F800044u, rrj_read32(m, face + 8));
            rrj_write32(m, 0x1F800048u, rrj_read32(m, face + 12));
            rrj_write32(m, 0x1F80004Cu, first_pair);
            rrj_write32(m, 0x1F800050u, second_pair);
            indices[0] = first_pair & 0xFFFFu;
            indices[1] = first_pair >> 16;
            indices[2] = second_pair & 0xFFFFu;
            indices[3] = second_pair >> 16;
            clip_values[0] = *(uint8_t *)rrj_at(m, clips + indices[0], 1);
            clip_values[1] = *(uint8_t *)rrj_at(m, clips + indices[1], 1);
            clip_values[2] = *(uint8_t *)rrj_at(m, clips + indices[2], 1);
            clip_values[3] = *(uint8_t *)rrj_at(m, clips + indices[3], 1);
            clip_union = clip_values[0] | clip_values[1] | clip_values[2] | clip_values[3];
            clip_intersection = clip_values[0] & clip_values[1] & clip_values[2] & clip_values[3];
            if ((clip_intersection & 0x1Fu) != 0)
                continue;
            if ((flags & 4u) == 0 && (clip_union & 0x10u) == 0)
            {
                int32_t first_clip = NormalClip((sint32)rrj_read32(m, screens + 4 * indices[0]), (sint32)rrj_read32(m, screens + 4 * indices[1]), (sint32)rrj_read32(m, screens + 4 * indices[2]));
                if (first_clip < 0 && NormalClip((sint32)rrj_read32(m, screens + 4 * indices[0]), (sint32)rrj_read32(m, screens + 4 * indices[3]), (sint32)rrj_read32(m, screens + 4 * indices[2])) > 0)
                    continue;
            }
            depth = rrj_s32(rrj_read32(m, depths + 16 * indices[0] + 8));
            for (index = 1; index != 4; ++index)
                if (rrj_s32(rrj_read32(m, depths + 16 * indices[index] + 8)) > depth)
                    depth = rrj_s32(rrj_read32(m, depths + 16 * indices[index] + 8));
            bucket = frontier_depth_bucket(m, depth);
            slot = ordering_table + 4 * (uint32_t)bucket;
            if ((clip_union & 0x20u) != 0)
            {
                for (index = 0; index != 4; ++index)
                    frontier_prepare_source_record(m, &matrix, index, indices[index], rrj_u16(rrj_at(m, 0x1F800040u + 4 * index, 2)));
                rrj_write32(m, 0x1F800034u, rrj_u16(rrj_at(m, 0x1F800042u, 2)) << 16);
                rrj_write32(m, 0x1F800170u, rrj_u16(rrj_at(m, 0x1F80004Au, 2)));
                rrj_write32(m, 0x1F800038u, rrj_u16(rrj_at(m, 0x1F800046u, 2)) << 16);
                frontier_render_wrapped_subdivision(m, slot, flags, 1);
                cursor = rrj_read32(m, 0x1F800020u);
            }
            else
            {
                uint32_t packet;
                uint32_t shade;

                if (cursor + 60 >= rrj_read32(m, 0x8005B4D0u))
                    cursor = sub_80021C98(m, cursor, 60);
                packet = cursor;
                rrj_write32(m, packet, rrj_read32(m, slot) | 0x0E000000u);
                rrj_write32(m, packet + 4, rrj_read32(m, 0x1F800078u + 4 * ((flags & 0xFFu) >> 4)));
                shade = *(uint8_t *)rrj_at(m, color_table + 8 * indices[0] + 6, 1);
                rrj_write32(m, packet + 8, rrj_read32(m, 0x800D4CA8u + 4 * shade) | 0x3C000000u);
                rrj_write32(m, packet + 12, rrj_read32(m, screens + 4 * indices[0]));
                rrj_write32(m, packet + 16, rrj_read32(m, 0x1F800040u));
                shade = *(uint8_t *)rrj_at(m, color_table + 8 * indices[1] + 6, 1);
                rrj_write32(m, packet + 20, rrj_read32(m, 0x800D4CA8u + 4 * shade));
                rrj_write32(m, packet + 24, rrj_read32(m, screens + 4 * indices[1]));
                rrj_write32(m, packet + 28, rrj_read32(m, 0x1F800044u));
                shade = *(uint8_t *)rrj_at(m, color_table + 8 * indices[3] + 6, 1);
                rrj_write32(m, packet + 32, rrj_read32(m, 0x800D4CA8u + 4 * shade));
                rrj_write32(m, packet + 36, rrj_read32(m, screens + 4 * indices[3]));
                rrj_write32(m, packet + 40, rrj_read32(m, 0x1F800048u) >> 16);
                shade = *(uint8_t *)rrj_at(m, color_table + 8 * indices[2] + 6, 1);
                rrj_write32(m, packet + 44, rrj_read32(m, 0x800D4CA8u + 4 * shade));
                rrj_write32(m, packet + 48, rrj_read32(m, screens + 4 * indices[2]));
                rrj_write32(m, packet + 52, rrj_read32(m, 0x1F800048u));
                rrj_write32(m, packet + 56, 0xE2000000u);
                cursor = packet + 60;
                rrj_write32(m, 0x1F800020u, cursor);
                rrj_write32(m, slot, packet);
            }
        }
    }
    rrj_write32(m, context + 268, cursor);
    return cursor;
}

uint32_t sub_8006C888(RRJMemory *m, uint32_t object, uint32_t group_index)
{
    uint32_t descriptor = rrj_read32(m, object);
    uint32_t group = rrj_read32(m, descriptor + 40) + 12 * group_index;
    uint32_t face = rrj_read32(m, descriptor + 60) + rrj_read32(m, group);
    uint32_t context = rrj_read32(m, 0x8005B470u);
    uint32_t ordering_table = rrj_read32(m, context + 264);
    uint32_t cursor = rrj_read32(m, context + 268);
    uint32_t screens = rrj_read32(m, 0x8005ACB4u);
    uint32_t depths = rrj_read32(m, 0x8005ACB0u);
    uint32_t color_table = rrj_read32(m, descriptor + 52) + 4;
    uint32_t count = rrj_read32(m, group + 4);
    uint32_t item;

    FUNCTION_MARKER(0x8006C888, "RASHCDG.BIN");
    rrj_write32(m, 0x1F800020u, cursor);
    rrj_write32(m, 0x800CCE2Cu, ordering_table);
    rrj_write32(m, 0x800CCE28u, color_table);
    rrj_write32(m, 0x800CCE34u, color_table);
    for (item = 0; item != count; ++item, face += 20)
    {
        uint32_t flags = rrj_read32(m, face);
        uint32_t packed_indices = rrj_read32(m, face + 16);
        uint32_t first_index = rrj_read32(m, face + 12) >> 16;
        uint32_t second_index = packed_indices & 0xFFFFu;
        uint32_t third_index = packed_indices >> 16;
        int32_t depth;
        int32_t candidate;
        int32_t bucket;
        uint32_t slot;
        uint32_t packet;
        uint32_t shade;

        rrj_write32(m, 0x1F80003Cu, flags);
        rrj_write32(m, 0x1F800040u, rrj_read32(m, face + 4));
        rrj_write32(m, 0x1F800044u, rrj_read32(m, face + 8));
        rrj_write32(m, 0x1F800048u, rrj_read32(m, face + 12));
        rrj_write32(m, 0x1F80004Cu, packed_indices);
        if ((flags & 4u) == 0 && NormalClip((sint32)rrj_read32(m, screens + 4 * first_index), (sint32)rrj_read32(m, screens + 4 * second_index), (sint32)rrj_read32(m, screens + 4 * third_index)) < 0)
            continue;
        depth = rrj_s32(rrj_read32(m, depths + 16 * first_index + 8));
        candidate = rrj_s32(rrj_read32(m, depths + 16 * second_index + 8));
        if (candidate > depth)
            depth = candidate;
        candidate = rrj_s32(rrj_read32(m, depths + 16 * third_index + 8));
        if (candidate > depth)
            depth = candidate;
        bucket = frontier_depth_bucket(m, depth);
        slot = ordering_table + 4 * (uint32_t)bucket;
        if (cursor + 48 >= rrj_read32(m, 0x8005B4D0u))
            cursor = sub_80021C98(m, cursor, 48);
        packet = cursor;
        rrj_write32(m, packet, rrj_read32(m, slot) | 0x0B000000u);
        rrj_write32(m, packet + 4, rrj_read32(m, 0x1F800078u + 4 * ((flags & 0xFFu) >> 4)));
        shade = *(uint8_t *)rrj_at(m, color_table + 8 * first_index + 6, 1);
        rrj_write32(m, packet + 8, (rrj_read32(m, 0x800D4CA8u + 4 * shade) & 0xFFFFFFu) | 0x34000000u);
        rrj_write32(m, packet + 12, rrj_read32(m, screens + 4 * first_index));
        rrj_write32(m, packet + 16, rrj_read32(m, 0x1F800040u));
        shade = *(uint8_t *)rrj_at(m, color_table + 8 * second_index + 6, 1);
        rrj_write32(m, packet + 20, rrj_read32(m, 0x800D4CA8u + 4 * shade) & 0xFFFFFFu);
        rrj_write32(m, packet + 24, rrj_read32(m, screens + 4 * second_index));
        rrj_write32(m, packet + 28, rrj_read32(m, 0x1F800044u));
        shade = *(uint8_t *)rrj_at(m, color_table + 8 * third_index + 6, 1);
        rrj_write32(m, packet + 32, rrj_read32(m, 0x800D4CA8u + 4 * shade) & 0xFFFFFFu);
        rrj_write32(m, packet + 36, rrj_read32(m, screens + 4 * third_index));
        rrj_write32(m, packet + 40, rrj_u16(rrj_at(m, 0x1F800048u, 2)));
        rrj_write32(m, packet + 44, 0xE2000000u);
        cursor = packet + 48;
        rrj_write32(m, 0x1F800020u, cursor);
        rrj_write32(m, slot, packet);
    }

    count = rrj_read32(m, group + 8);
    for (item = 0; item != count; ++item, face += 24)
    {
        uint32_t flags = rrj_read32(m, face);
        uint32_t first_pair = rrj_read32(m, face + 16);
        uint32_t second_pair = rrj_read32(m, face + 20);
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

        rrj_write32(m, 0x1F80003Cu, flags);
        rrj_write32(m, 0x1F800040u, rrj_read32(m, face + 4));
        rrj_write32(m, 0x1F800044u, rrj_read32(m, face + 8));
        rrj_write32(m, 0x1F800048u, rrj_read32(m, face + 12));
        rrj_write32(m, 0x1F80004Cu, first_pair);
        rrj_write32(m, 0x1F800050u, second_pair);
        if ((flags & 4u) == 0)
        {
            int32_t first_clip = NormalClip((sint32)rrj_read32(m, screens + 4 * first_index), (sint32)rrj_read32(m, screens + 4 * second_index), (sint32)rrj_read32(m, screens + 4 * third_index));
            if (first_clip < 0 && NormalClip((sint32)rrj_read32(m, screens + 4 * first_index), (sint32)rrj_read32(m, screens + 4 * fourth_index), (sint32)rrj_read32(m, screens + 4 * third_index)) > 0)
                continue;
        }
        depth = rrj_s32(rrj_read32(m, depths + 16 * first_index + 8));
        candidate = rrj_s32(rrj_read32(m, depths + 16 * second_index + 8));
        if (candidate > depth)
            depth = candidate;
        candidate = rrj_s32(rrj_read32(m, depths + 16 * third_index + 8));
        if (candidate > depth)
            depth = candidate;
        candidate = rrj_s32(rrj_read32(m, depths + 16 * fourth_index + 8));
        if (candidate > depth)
            depth = candidate;
        bucket = frontier_depth_bucket(m, depth);
        slot = ordering_table + 4 * (uint32_t)bucket;
        if (cursor + 60 >= rrj_read32(m, 0x8005B4D0u))
            cursor = sub_80021C98(m, cursor, 60);
        packet = cursor;
        rrj_write32(m, packet, rrj_read32(m, slot) | 0x0E000000u);
        rrj_write32(m, packet + 4, rrj_read32(m, 0x1F800078u + 4 * ((flags & 0xFFu) >> 4)));
        shade = *(uint8_t *)rrj_at(m, color_table + 8 * first_index + 6, 1);
        rrj_write32(m, packet + 8, (rrj_read32(m, 0x800D4CA8u + 4 * shade) & 0xFFFFFFu) | 0x3C000000u);
        rrj_write32(m, packet + 12, rrj_read32(m, screens + 4 * first_index));
        rrj_write32(m, packet + 16, rrj_read32(m, 0x1F800040u));
        shade = *(uint8_t *)rrj_at(m, color_table + 8 * second_index + 6, 1);
        rrj_write32(m, packet + 20, rrj_read32(m, 0x800D4CA8u + 4 * shade) & 0xFFFFFFu);
        rrj_write32(m, packet + 24, rrj_read32(m, screens + 4 * second_index));
        rrj_write32(m, packet + 28, rrj_read32(m, 0x1F800044u));
        shade = *(uint8_t *)rrj_at(m, color_table + 8 * fourth_index + 6, 1);
        rrj_write32(m, packet + 32, rrj_read32(m, 0x800D4CA8u + 4 * shade) & 0xFFFFFFu);
        rrj_write32(m, packet + 36, rrj_read32(m, screens + 4 * fourth_index));
        rrj_write32(m, packet + 40, rrj_read32(m, 0x1F800048u) >> 16);
        shade = *(uint8_t *)rrj_at(m, color_table + 8 * third_index + 6, 1);
        rrj_write32(m, packet + 44, rrj_read32(m, 0x800D4CA8u + 4 * shade) & 0xFFFFFFu);
        rrj_write32(m, packet + 48, rrj_read32(m, screens + 4 * third_index));
        rrj_write32(m, packet + 52, rrj_read32(m, 0x1F800048u));
        rrj_write32(m, packet + 56, 0xE2000000u);
        cursor = packet + 60;
        rrj_write32(m, 0x1F800020u, cursor);
        rrj_write32(m, slot, packet);
    }
    rrj_write32(m, context + 268, cursor);
    return cursor;
}

static uint32_t frontier_emit_road_quads(RRJMemory *m, uint32_t object, uint32_t group_index, int reject_common_clip)
{
    uint32_t descriptor = rrj_read32(m, object);
    uint32_t group = rrj_read32(m, descriptor + 40) + 12 * group_index;
    uint32_t count = rrj_read32(m, group + 8);
    uint32_t face = rrj_read32(m, descriptor + 60) + rrj_read32(m, group);
    uint32_t context = rrj_read32(m, 0x8005B470u);
    uint32_t ordering_table = rrj_read32(m, context + 264);
    uint32_t cursor = rrj_read32(m, context + 268);
    uint32_t color_table = rrj_read32(m, descriptor + 52) + 4;
    uint32_t depths = rrj_read32(m, 0x8005ACB0u);
    uint32_t screens = rrj_read32(m, 0x8005ACB4u);
    uint32_t clips = rrj_read32(m, 0x8005ACB8u);
    MATRIX matrix;
    uint32_t item;

    rrj_write32(m, 0x1F800020u, cursor);
    rrj_write32(m, 0x800CCE2Cu, ordering_table);
    rrj_write32(m, 0x800CCE34u, color_table);
    rrj_write32(m, 0x800CCE28u, color_table);
    ReadRotMatrix(&matrix);
    for (item = 0; item != count; ++item, face += 24)
    {
        uint32_t first_pair = rrj_read32(m, face + 16);
        uint32_t second_pair = rrj_read32(m, face + 20);
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
        rrj_write32(m, 0x1F80004Cu, first_pair);
        rrj_write32(m, 0x1F800050u, second_pair);
        for (index = 0; index != 4; ++index)
            clip &= *(uint8_t *)rrj_at(m, clips + indices[index], 1);
        if (reject_common_clip && clip != 0)
            continue;
        for (index = 0; index != 4; ++index)
            rrj_write32(m, 0x1F80003Cu + 4 * index, rrj_read32(m, face + 4 * index));
        depth = rrj_s32(rrj_read32(m, depths + 16 * indices[0] + 8));
        for (index = 1; index != 4; ++index)
        {
            int32_t candidate = rrj_s32(rrj_read32(m, depths + 16 * indices[index] + 8));
            if (candidate > depth)
                depth = candidate;
        }
        if (depth >= 4096)
        {
            uint32_t flags = rrj_u16(rrj_at(m, 0x1F80003Eu, 2));
            uint32_t packet;
            uint32_t shade;

            rrj_put16(rrj_at(m, 0x1F800042u, 2), rrj_u16(rrj_at(m, 0x800D5EC8u + ((flags >> 1) & 0x18u), 2)));
            rrj_put16(rrj_at(m, 0x1F800046u, 2), rrj_u16(rrj_at(m, 0x8005B370u, 2)));
            if (cursor + 52 >= rrj_read32(m, 0x8005B4D0u))
                cursor = sub_80021C98(m, cursor, 52);
            bucket = frontier_depth_bucket(m, depth);
            slot = ordering_table + 4 * (uint32_t)bucket;
            packet = cursor;
            rrj_write32(m, packet, rrj_read32(m, slot) | 0x0C000000u);
            shade = *(uint8_t *)rrj_at(m, color_table + 8 * indices[0] + 6, 1);
            rrj_write32(m, packet + 4, rrj_read32(m, 0x800D4CA8u + 4 * shade) | 0x3C000000u);
            rrj_write32(m, packet + 8, rrj_read32(m, screens + 4 * indices[0]));
            rrj_write32(m, packet + 12, rrj_read32(m, 0x1F800040u));
            shade = *(uint8_t *)rrj_at(m, color_table + 8 * indices[1] + 6, 1);
            rrj_write32(m, packet + 16, rrj_read32(m, 0x800D4CA8u + 4 * shade));
            rrj_write32(m, packet + 20, rrj_read32(m, screens + 4 * indices[1]));
            rrj_write32(m, packet + 24, rrj_read32(m, 0x1F800044u));
            shade = *(uint8_t *)rrj_at(m, color_table + 8 * indices[3] + 6, 1);
            rrj_write32(m, packet + 28, rrj_read32(m, 0x800D4CA8u + 4 * shade));
            rrj_write32(m, packet + 32, rrj_read32(m, screens + 4 * indices[3]));
            rrj_write32(m, packet + 36, rrj_u16(rrj_at(m, 0x1F80004Au, 2)));
            shade = *(uint8_t *)rrj_at(m, color_table + 8 * indices[2] + 6, 1);
            rrj_write32(m, packet + 40, rrj_read32(m, 0x800D4CA8u + 4 * shade));
            rrj_write32(m, packet + 44, rrj_read32(m, screens + 4 * indices[2]));
            rrj_write32(m, packet + 48, rrj_u16(rrj_at(m, 0x1F800048u, 2)));
            cursor = packet + 52;
            rrj_write32(m, 0x1F800020u, cursor);
            rrj_write32(m, slot, packet);
        }
        else
        {
            FrontierVertex vertices[18];
            int32_t delta[2][3];
            int16_t shade_delta[2];
            uint32_t flags = rrj_u16(rrj_at(m, 0x1F80003Eu, 2));
            uint32_t table_offset = 4 * (flags & 0x0Fu);
            uint32_t pattern = rrj_read32(m, 0x800CC914u + table_offset);
            uint32_t segment_count = pattern >> 28;
            uint32_t texture_pattern = rrj_read32(m, 0x800CC874u + table_offset);
            uint32_t texture_group = (flags >> 2) & 0x0Cu;
            uint32_t depth_band = *(uint8_t *)rrj_at(m, 0x800CCA58u + frontier_abs(depth) / 512, 1);
            uint32_t original_slots[4];
            int32_t step = rrj_s32(rrj_read32(m, 0x800CCA10u + 4 * segment_count));
            uint32_t pair;
            uint32_t position;
            uint32_t pattern_shift = 0;

            memset(vertices, 0, sizeof(vertices));
            original_slots[0] = 0;
            original_slots[1] = 2 * segment_count;
            original_slots[2] = 2 * segment_count + 1;
            original_slots[3] = 1;
            bucket = frontier_depth_bucket(m, depth);
            slot = ordering_table + 4 * (uint32_t)bucket;
            for (index = 0; index != 4; ++index)
            {
                FrontierVertex *vertex = &vertices[original_slots[index]];
                uint32_t shade;
                uint32_t depth_clip;

                frontier_transform_vertex(&matrix, m, color_table + 8 * indices[index], vertex);
                shade = *(uint8_t *)rrj_at(m, color_table + 8 * indices[index] + 6, 1);
                vertex->shade = (int32_t)shade;
                vertex->color_clip = rrj_read32(m, 0x800D4CA8u + 4 * shade) & 0xFFFFFFu;
                if (vertex->z > 204799)
                    depth_clip = 1u << 5;
                else if (vertex->z > 51199)
                    depth_clip = 1u << 6;
                else
                    depth_clip = 1u << 7;
                vertex->color_clip |= (uint32_t)(*(uint8_t *)rrj_at(m, clips + indices[index], 1) | depth_clip) << 24;
            }
            for (pair = 0; pair != 2; ++pair)
            {
                FrontierVertex *start = &vertices[pair];
                FrontierVertex *end = &vertices[2 * segment_count + pair];

                delta[pair][0] = (int32_t)(((int64_t)(end->x - start->x) * step) >> 12);
                delta[pair][1] = (int32_t)(((int64_t)(end->y - start->y) * step) >> 12);
                delta[pair][2] = (int32_t)(((int64_t)(end->z - start->z) * step) >> 12);
                shade_delta[pair] = (int16_t)(((int64_t)(end->shade - start->shade) * step) >> 12);
            }
            for (position = 2; position < 2 * segment_count; position += 2)
            {
                for (pair = 0; pair != 2; ++pair)
                {
                    FrontierVertex *previous = &vertices[position - 2 + pair];
                    FrontierVertex *vertex = &vertices[position + pair];
                    uint32_t shade;

                    vertex->x = rrj_s32((uint32_t)previous->x + (uint32_t)delta[pair][0]);
                    vertex->y = rrj_s32((uint32_t)previous->y + (uint32_t)delta[pair][1]);
                    vertex->z = rrj_s32((uint32_t)previous->z + (uint32_t)delta[pair][2]);
                    vertex->screen = frontier_project_translation(vertex->x >> 5, vertex->y >> 5, vertex->z >> 5);
                    vertex->shade = previous->shade + shade_delta[pair];
                    shade = (uint8_t)vertex->shade;
                    vertex->color_clip = (rrj_read32(m, 0x800D4CA8u + 4 * shade) & 0xFFFFFFu) | ((uint32_t)frontier_classify_projected(vertex->z, vertex->screen, 102399) << 24);
                }
            }
            rrj_write32(m, 0x1F800028u, rrj_read32(m, slot));
            for (position = 0; position < 2 * segment_count; position += 2)
            {
                uint32_t side_code = (pattern >> pattern_shift) & 3u;

                if (side_code != 0)
                {
                    uint32_t color = rrj_read32(m, 0x800523ACu + 4 * side_code);

                    for (pair = 0; pair != 2; ++pair)
                    {
                        FrontierVertex *source = &vertices[position + pair];
                        uint32_t near_record = 0x1F800100u + 32 * pair;
                        uint32_t far_record = 0x1F800140u + 32 * pair;
                        int32_t x = rrj_s32((uint32_t)source->x + (uint32_t)(delta[pair][0] >> 6));
                        int32_t y = rrj_s32((uint32_t)source->y + (uint32_t)(delta[pair][1] >> 6));
                        int32_t z = rrj_s32((uint32_t)source->z + (uint32_t)(delta[pair][2] >> 6));

                        rrj_write32(m, near_record, (uint32_t)x);
                        rrj_write32(m, near_record + 4, (uint32_t)y);
                        rrj_write32(m, near_record + 8, (uint32_t)z);
                        rrj_write32(m, far_record, (uint32_t)rrj_s32((uint32_t)x + (uint32_t)(delta[pair][0] >> 5)));
                        rrj_write32(m, far_record + 4, (uint32_t)rrj_s32((uint32_t)y + (uint32_t)(delta[pair][1] >> 5)));
                        rrj_write32(m, far_record + 8, (uint32_t)rrj_s32((uint32_t)z + (uint32_t)(delta[pair][2] >> 5)));
                    }
                    frontier_project_scratch_quad(m);
                    frontier_render_flat_quad(m, depth, color);
                }
                side_code = (pattern >> (pattern_shift + 2)) & 3u;
                if (side_code != 0)
                {
                    uint32_t color = rrj_read32(m, 0x800523ACu + 4 * side_code);

                    for (pair = 0; pair != 2; ++pair)
                    {
                        FrontierVertex *source = &vertices[position + 2 + pair];
                        uint32_t far_record = 0x1F800140u + 32 * pair;
                        uint32_t near_record = 0x1F800100u + 32 * pair;
                        int32_t x = rrj_s32((uint32_t)source->x - (uint32_t)(delta[pair][0] >> 6));
                        int32_t y = rrj_s32((uint32_t)source->y - (uint32_t)(delta[pair][1] >> 6));
                        int32_t z = rrj_s32((uint32_t)source->z - (uint32_t)(delta[pair][2] >> 6));

                        rrj_write32(m, far_record, (uint32_t)x);
                        rrj_write32(m, far_record + 4, (uint32_t)y);
                        rrj_write32(m, far_record + 8, (uint32_t)z);
                        rrj_write32(m, near_record, (uint32_t)rrj_s32((uint32_t)x - (uint32_t)(delta[pair][0] >> 5)));
                        rrj_write32(m, near_record + 4, (uint32_t)rrj_s32((uint32_t)y - (uint32_t)(delta[pair][1] >> 5)));
                        rrj_write32(m, near_record + 8, (uint32_t)rrj_s32((uint32_t)z - (uint32_t)(delta[pair][2] >> 5)));
                    }
                    frontier_project_scratch_quad(m);
                    frontier_render_flat_quad(m, depth, color);
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

                    rrj_write32(m, record, (uint32_t)vertex->x);
                    rrj_write32(m, record + 4, (uint32_t)vertex->y);
                    rrj_write32(m, record + 8, (uint32_t)vertex->z);
                    rrj_write32(m, record + 12, vertex->reserved);
                    rrj_write32(m, record + 16, rrj_u16(rrj_at(m, uv_table + 2 * scratch_index, 2)));
                    rrj_write32(m, record + 20, (uint32_t)vertex->shade);
                    rrj_write32(m, record + 24, vertex->screen);
                    rrj_write32(m, record + 28, vertex->color_clip);
                }
                rrj_write32(m, 0x1F800034u, rrj_u16(rrj_at(m, 0x800D5EC8u + 2 * (clut_code + 1 + texture_group), 2)) << 16);
                rrj_write32(m, 0x1F800038u, rrj_u16(rrj_at(m, 0x8005B370u, 2)) << 16);
                (void)sub_80069CF0(m, 4, 0x01030200u, 5);
                pattern_shift += 5;
            }
            rrj_write32(m, slot, rrj_read32(m, 0x1F800028u));
            cursor = rrj_read32(m, 0x1F800020u);
        }
    }
    rrj_write32(m, context + 268, cursor);
    return cursor;
}

uint32_t sub_8006E474(RRJMemory *m, uint32_t object, uint32_t group_index)
{
    FUNCTION_MARKER(0x8006E474, "RASHCDG.BIN");
    return frontier_emit_road_quads(m, object, group_index, 1);
}

uint32_t sub_8006F5D0(RRJMemory *m, uint32_t object, uint32_t group_index)
{
    FUNCTION_MARKER(0x8006F5D0, "RASHCDG.BIN");
    return frontier_emit_road_quads(m, object, group_index, 0);
}

uint32_t sub_80068FCC(RRJMemory *m, uint32_t object)
{
    uint32_t descriptor = rrj_read32(m, object);
    uint32_t base_count = rrj_u16(rrj_at(m, descriptor + 4, 2));
    uint32_t row_count = rrj_u16(rrj_at(m, descriptor + 6, 2));
    uint32_t end = base_count + row_count;
    uint32_t group;
    uint32_t shift = 0;
    uint32_t state;
    uint32_t filtered = rrj_read32(m, 0x800CC86Cu);
    uint32_t flags = rrj_read32(m, object + 52);

    FUNCTION_MARKER(0x80068FCC, "RASHCDG.BIN");
    (void)sub_8001064C(m);
    for (group = 0; group != 16; ++group)
        rrj_write32(m, 0x1F800078u + 4 * group, rrj_read32(m, 0x800CC954u + 4 * group));
    for (group = 0; group < base_count; ++group)
        (void)sub_8006D350(m, object, group);
    state = rrj_read32(m, 0x8005B310u);
    if (state != 0 && rrj_read32(m, state + 16) == rrj_read32(m, object + 4))
    {
        uint32_t row = rrj_read32(m, state + 20);
        uint32_t mode = (flags >> ((4 * row) & 31u)) & (filtered != 0 ? 0x0Du : 0x0Fu);

        if ((mode & 2u) != 0)
            (void)sub_800706A4(m, object);
    }
    for (group = base_count; group < end; ++group, shift += 4)
    {
        uint32_t mode = (flags >> (shift & 31u)) & (filtered != 0 ? 0x0Du : 0x0Fu);

        if ((mode & 1u) == 0)
            continue;
        if ((mode & 2u) != 0)
        {
            if ((mode & 4u) != 0)
            {
                (void)sub_8006E474(m, object, group + 2 * row_count);
                (void)sub_8006A630(m, object, group + row_count);
            }
            else
            {
                (void)sub_8006F5D0(m, object, group + 2 * row_count);
                (void)sub_8006C888(m, object, group + row_count);
            }
        }
        else if ((mode & 4u) != 0)
        {
            (void)sub_8006D350(m, object, group);
        }
        else
        {
            (void)sub_8006DC20(m, object, group);
        }
    }
    return 0;
}

uint32_t sub_800706A4(RRJMemory *m, uint32_t object)
{
    uint32_t descriptor = rrj_read32(m, object);
    uint32_t row_count = rrj_u16(rrj_at(m, descriptor + 6, 2));
    uint32_t state = rrj_read32(m, 0x8005B310u);
    uint32_t row = rrj_read32(m, state + 20);
    uint32_t group_index;
    uint32_t group;
    uint32_t quad_count;
    uint32_t quad;
    uint32_t face;
    uint32_t context = rrj_read32(m, 0x8005B470u);
    uint32_t ordering_table = rrj_read32(m, context + 264);
    uint32_t cursor = rrj_read32(m, context + 268);
    uint32_t clips = rrj_read32(m, 0x8005ACB8u);
    uint32_t screens = rrj_read32(m, 0x8005ACB4u);
    uint32_t depths = rrj_read32(m, 0x8005ACB0u);
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
        rrj_write32(m, state + 20, row);
    }
    group_index = row + 2 * row_count + rrj_u16(rrj_at(m, descriptor + 4, 2));
    group = rrj_read32(m, descriptor + 40) + 12 * group_index;
    quad_count = rrj_read32(m, group + 8);
    quad = rrj_read32(m, state + 24);
    if (quad >= quad_count)
    {
        quad = quad_count - 1;
        rrj_write32(m, state + 24, quad);
    }
    face = rrj_read32(m, descriptor + 60) + rrj_read32(m, group) + 24 * quad;
    rrj_write32(m, 0x800CCE2Cu, ordering_table);
    rrj_write32(m, 0x800CCE34u, rrj_read32(m, descriptor + 52) + 4);
    rrj_write32(m, 0x800CCE28u, rrj_read32(m, descriptor + 52) + 4);
    rrj_write32(m, 0x1F800020u, cursor);
    rrj_write32(m, 0x1F80004Cu, rrj_read32(m, face + 16));
    rrj_write32(m, 0x1F800050u, rrj_read32(m, face + 20));
    for (index = 0; index != 4; ++index)
        indices[index] = rrj_u16(rrj_at(m, 0x1F80004Cu + 2 * index, 2));
    if ((*(uint8_t *)rrj_at(m, clips + indices[0], 1) & *(uint8_t *)rrj_at(m, clips + indices[1], 1) & *(uint8_t *)rrj_at(m, clips + indices[2], 1) & *(uint8_t *)rrj_at(m, clips + indices[3], 1) & 0x0Fu) != 0)
        return 0;

    for (index = 0; index != 4; ++index)
        rrj_write32(m, 0x1F80003Cu + 4 * index, rrj_read32(m, face + 4 * index));
    texture = 0x800533B4u + 88 * (rrj_read32(m, rrj_read32(m, 0x8005B2F8u) + 48) - 1);
    texture_x = *(uint8_t *)rrj_at(m, texture + 85, 1) + ((*(uint8_t *)rrj_at(m, texture + 86, 1) & 0x10u) << 4);
    rrj_put16(rrj_at(m, 0x1F800042u, 2), (uint16_t)((texture_x << 6) | ((*(uint8_t *)rrj_at(m, texture + 86, 1) & 0x0Fu) << 2) | 1u));
    *(uint8_t *)rrj_at(m, 0x1F800040u, 1) = 32;
    rrj_put16(rrj_at(m, 0x1F800046u, 2), (uint16_t)(((texture_x & 0x100u) >> 4) | (*(uint8_t *)rrj_at(m, texture + 86, 1) & 0x0Fu)));
    *(uint8_t *)rrj_at(m, 0x1F800044u, 1) = 63;
    *(uint8_t *)rrj_at(m, 0x1F800041u, 1) = (uint8_t)(texture_x + 2);
    *(uint8_t *)rrj_at(m, 0x1F800048u, 1) = 63;
    *(uint8_t *)rrj_at(m, 0x1F800045u, 1) = (uint8_t)(texture_x + 2);
    *(uint8_t *)rrj_at(m, 0x1F80004Au, 1) = 32;
    *(uint8_t *)rrj_at(m, 0x1F800049u, 1) = (uint8_t)(texture_x + 34);
    rrj_put16(rrj_at(m, 0x1F80003Eu, 2), 3);
    *(uint8_t *)rrj_at(m, 0x1F80004Bu, 1) = (uint8_t)(texture_x + 34);

    depth = rrj_s32(rrj_read32(m, depths + 16 * indices[0] + 8));
    if (depth >= 4096)
    {
        uint32_t packet;

        if (cursor + 52 >= rrj_read32(m, 0x8005B4D0u))
            cursor = sub_80021C98(m, cursor, 52);
        bucket = frontier_depth_bucket(m, depth);
        slot = ordering_table + 4 * (uint32_t)bucket;
        packet = cursor;
        rrj_write32(m, packet, rrj_read32(m, slot) | 0x0C000000u);
        for (index = 0; index != 4; ++index)
        {
            static const uint8_t packet_order[4] = {0, 1, 3, 2};
            static const uint8_t packet_offsets[4] = {4, 16, 28, 40};
            static const uint8_t uv_offsets[4] = {0x40, 0x44, 0x4A, 0x48};
            uint32_t vertex = indices[packet_order[index]];
            uint32_t shade = *(uint8_t *)rrj_at(m, rrj_read32(m, 0x800CCE34u) + 8 * vertex + 6, 1);
            uint32_t offset = packet_offsets[index];
            uint32_t color = rrj_read32(m, 0x800D4CA8u + 4 * shade) & 0xFFFFFFu;

            if (index == 0)
                color |= 0x3C000000u;
            rrj_write32(m, packet + offset, color);
            rrj_write32(m, packet + offset + 4, rrj_read32(m, screens + 4 * vertex));
            rrj_write32(m, packet + offset + 8, rrj_u16(rrj_at(m, 0x1F800000u + uv_offsets[index], 2)));
        }
        cursor = packet + 52;
        rrj_write32(m, 0x1F800020u, cursor);
        rrj_write32(m, slot, packet);
    }
    else
    {
        MATRIX matrix;

        for (index = 1; index != 4; ++index)
        {
            int32_t candidate = rrj_s32(rrj_read32(m, depths + 16 * indices[index] + 8));
            if (candidate > depth)
                depth = candidate;
        }
        bucket = frontier_depth_bucket(m, depth);
        slot = ordering_table + 4 * (uint32_t)bucket;
        ReadRotMatrix(&matrix);
        for (index = 0; index != 4; ++index)
        {
            uint32_t record = 0x1F800100u + 32 * index;
            uint32_t source = rrj_read32(m, 0x800CCE28u) + 8 * indices[index];
            int32_t x = (int32_t)(((int64_t)matrix.m[0][0] * (int16_t)rrj_u16(rrj_at(m, source, 2)) + (int64_t)matrix.m[0][1] * (int16_t)rrj_u16(rrj_at(m, source + 2, 2)) + (int64_t)matrix.m[0][2] * (int16_t)rrj_u16(rrj_at(m, source + 4, 2))) >> 12);
            int32_t y = (int32_t)(((int64_t)matrix.m[1][0] * (int16_t)rrj_u16(rrj_at(m, source, 2)) + (int64_t)matrix.m[1][1] * (int16_t)rrj_u16(rrj_at(m, source + 2, 2)) + (int64_t)matrix.m[1][2] * (int16_t)rrj_u16(rrj_at(m, source + 4, 2))) >> 12);
            int32_t z = (int32_t)(((int64_t)matrix.m[2][0] * (int16_t)rrj_u16(rrj_at(m, source, 2)) + (int64_t)matrix.m[2][1] * (int16_t)rrj_u16(rrj_at(m, source + 2, 2)) + (int64_t)matrix.m[2][2] * (int16_t)rrj_u16(rrj_at(m, source + 4, 2))) >> 12);
            uint32_t shade = *(uint8_t *)rrj_at(m, rrj_read32(m, 0x800CCE34u) + 8 * indices[index] + 6, 1);
            uint32_t clip;

            x = rrj_s32(((uint32_t)x + (uint32_t)matrix.t[0]) << 8);
            y = rrj_s32(((uint32_t)y + (uint32_t)matrix.t[1]) << 8);
            z = rrj_s32(((uint32_t)z + (uint32_t)matrix.t[2]) << 8);
            rrj_write32(m, record, (uint32_t)x);
            rrj_write32(m, record + 4, (uint32_t)y);
            rrj_write32(m, record + 8, (uint32_t)z);
            rrj_write32(m, record + 16, rrj_u16(rrj_at(m, 0x1F800040u + 4 * index, 2)));
            rrj_write32(m, record + 20, shade);
            rrj_write32(m, record + 24, frontier_project_translation(x >> 5, y >> 5, z >> 5));
            rrj_write32(m, record + 28, (rrj_read32(m, record + 28) & 0xFF000000u) | (rrj_read32(m, 0x800D4CA8u + 4 * shade) & 0xFFFFFFu));
            if (z > 204799)
                clip = 1u << 5;
            else if (z > 51199)
                clip = 1u << 6;
            else
                clip = 1u << 7;
            *(uint8_t *)rrj_at(m, record + 31, 1) = (uint8_t)(*(uint8_t *)rrj_at(m, clips + indices[index], 1) | clip);
        }
        rrj_write32(m, 0x1F800170u, rrj_u16(rrj_at(m, 0x1F80004Au, 2)));
        rrj_write32(m, 0x1F800034u, rrj_u16(rrj_at(m, 0x1F800042u, 2)) << 16);
        rrj_write32(m, 0x1F800038u, rrj_u16(rrj_at(m, 0x1F800046u, 2)) << 16);
        rrj_write32(m, 0x1F800028u, rrj_read32(m, slot));
        (void)sub_80069784(m, 4, 0x03020100u, 4);
        rrj_write32(m, slot, rrj_read32(m, 0x1F800028u));
        cursor = rrj_read32(m, 0x1F800020u);
    }
    rrj_write32(m, context + 268, cursor);
    return cursor;
}

int32_t sub_800669E8(RRJMemory *m, uint32_t object, uint32_t output)
{
    uint32_t descriptor = rrj_read32(m, object);
    uint32_t shift = rrj_u16(rrj_at(m, descriptor + 14, 2)) >> 12;
    int32_t x = (int32_t)rrj_read32(m, object + 12);
    int32_t y = (int32_t)rrj_read32(m, object + 16);
    int32_t z = (int32_t)rrj_read32(m, object + 20);

    FUNCTION_MARKER(0x800669E8, "RASHCDG.BIN");
    if (*(int8_t *)rrj_at(m, object + 72, 1) == 1)
    {
        x >>= shift;
        y >>= shift;
        z >>= shift;
    }
    rrj_write32(m, output, (uint32_t)x);
    rrj_write32(m, output + 4, (uint32_t)y);
    rrj_write32(m, output + 8, (uint32_t)z);
    return z;
}

uint32_t sub_800220A4(RRJMemory *m, uint32_t matrix_address, uint32_t count, uint32_t vertices, uint32_t records, uint32_t screens, uint32_t clips)
{
    MATRIX matrix;
    uint32_t index;
    uint32_t result = 0;

    FUNCTION_MARKER(0x800220A4, "SLUS_010.53");
    memcpy(&matrix, rrj_at(m, matrix_address, sizeof(matrix)), sizeof(matrix));
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);
    for (index = 0; index < count; ++index)
    {
        uint32_t source = vertices + 8 * index;
        SVECTOR vertex;
        VECTOR transformed;
        sint32 screen;
        sint32 flags;
        int16_t x;
        int16_t y;
        uint8_t classification;

        vertex.vx = (int16_t)rrj_u16(rrj_at(m, source, 2));
        vertex.vy = (int16_t)rrj_u16(rrj_at(m, source + 2, 2));
        vertex.vz = (int16_t)rrj_u16(rrj_at(m, source + 4, 2));
        vertex.pad = (int16_t)rrj_u16(rrj_at(m, source + 6, 2));
        gte_transform(&vertex, &transformed, &flags);
        (void)gte_project(&vertex, &screen, &flags);
        x = (int16_t)screen;
        y = (int16_t)(screen >> 16);
        classification = (uint8_t)((transformed.vz < 1024 ? 32 : 0) | (transformed.vz < 40 ? 16 : 0) | (y > 240 ? 8 : 0) | (y < 0 ? 4 : 0) | (x > 384 ? 2 : 0) | (x < 0 ? 1 : 0));
        rrj_write32(m, records + 16 * index, (uint32_t)transformed.vx);
        rrj_write32(m, records + 16 * index + 4, (uint32_t)transformed.vy);
        rrj_write32(m, records + 16 * index + 8, (uint32_t)transformed.vz);
        rrj_write32(m, screens + 4 * index, (uint32_t)screen);
        *(uint8_t *)rrj_at(m, clips + index, 1) = classification;
        result = classification & 3;
    }
    return result;
}

uint32_t sub_80030B7C(RRJMemory *m, uint32_t first, uint32_t second)
{
    uint32_t array = rrj_read32(m, 0x8005ACBC);
    uint32_t entry = array + 36 * first;
    uint32_t value;

    FUNCTION_MARKER(0x80030B7C, "SLUS_010.53");
    value = rrj_read32(m, rrj_read32(m, entry + 64));
    rrj_write32(m, entry + 52, value);
    entry = array + 36 * second;
    value = rrj_read32(m, rrj_read32(m, entry + 64));
    rrj_write32(m, entry + 52, value);
    return value;
}

uint32_t sub_80030C8C(RRJMemory *m, uint32_t index)
{
    uint32_t entry = rrj_read32(m, 0x8005ACBC) + 36 * index;
    uint32_t value;

    FUNCTION_MARKER(0x80030C8C, "SLUS_010.53");
    value = rrj_read32(m, rrj_read32(m, entry + 64));
    rrj_write32(m, entry + 52, value);
    return value;
}

void sub_80030CB8(RRJMemory *m, int32_t value)
{
    FUNCTION_MARKER(0x80030CB8, "SLUS_010.53");
    (void)(value >> 1);
}

static uint32_t frontier_store_vram(RRJMemory *m, uint32_t output, uint32_t x, uint32_t y)
{
    uint32_t row;
    uint8_t *destination = rrj_at(m, output, 64 * 128 * 2);
    const uint8 *vram = (const uint8 *)VRAM;
    for (row = 0; row < 128; ++row)
        memcpy(destination + row * 128, vram + ((y + row) * 1024 + x) * 2, 128);
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

uint32_t sub_80022484(RRJMemory *m, uint32_t page, uint32_t mode, uint32_t first_output, uint32_t second_output, uint32_t table_index)
{
    uint32_t state = rrj_read32(m, 0x8005B2F8);
    uint32_t table_page = rrj_read32(m, state + 48);
    uint32_t kind = mode >> 15;
    uint32_t entry;
    uint32_t x;
    uint32_t y;

    FUNCTION_MARKER(0x80022484, "SLUS_010.53");
    if (kind == 0)
    {
        entry = 0x800533B4u + 88 * (table_page - 1) + 4 * table_index;
        x = ((page & 15) + (*(uint8_t *)rrj_at(m, entry + 2, 1) & 15)) << 6;
        (void)frontier_store_vram(m, first_output, x, 0);
        return frontier_store_vram(m, second_output, x, 128);
    }
    if (kind == 1)
    {
        entry = 0x800533B4u + 88 * (table_page - 1) + 8 + 4 * table_index;
        x = ((*(uint8_t *)rrj_at(m, entry + 2, 1) & 15) << 6) + ((page << 5) & 0x3C0);
        y = *(uint8_t *)rrj_at(m, entry + 1, 1) + ((*(uint8_t *)rrj_at(m, entry + 2, 1) & 0x10) << 4) + ((page & 1) << 7);
        return frontier_store_vram(m, first_output, x, y);
    }
    return 1;
}

uint32_t sub_800225E0(RRJMemory *m, uint32_t page, uint32_t mode, uint32_t table_index)
{
    uint32_t state = rrj_read32(m, 0x8005B2F8);
    uint32_t table_page = rrj_read32(m, state + 48);
    uint32_t table_base = 0x800533B4u + 88 * (table_page - 1);
    uint32_t destination_x = ((*(uint8_t *)rrj_at(m, table_base + 2, 1) & 15) << 6) + 256;
    uint32_t kind = mode >> 15;
    uint32_t entry;
    uint32_t source_x;
    uint32_t source_y;

    FUNCTION_MARKER(0x800225E0, "SLUS_010.53");
    if (kind == 0)
    {
        rrj_write32(m, 0x800D7710u + 24 * table_index + 8 * page, 0xFFFFFFFFu);
        entry = table_base + 4 * table_index;
        source_x = ((page & 15) + (*(uint8_t *)rrj_at(m, entry + 2, 1) & 15)) << 6;
        return frontier_move_vram(source_x, 0, 256, destination_x);
    }
    if (kind == 1)
    {
        rrj_write32(m, 0x800D76D0u + 32 * table_index + 8 * page, 0xFFFFFFFFu);
        entry = table_base + 8 + 4 * table_index;
        source_x = ((*(uint8_t *)rrj_at(m, entry + 2, 1) & 15) << 6) + ((page << 5) & 0x3C0);
        source_y = *(uint8_t *)rrj_at(m, entry + 1, 1) + ((*(uint8_t *)rrj_at(m, entry + 2, 1) & 0x10) << 4) + ((page & 1) << 7);
        return frontier_move_vram(source_x, source_y, 128, destination_x);
    }
    return 1;
}

uint32_t sub_80033634(RRJMemory *m, int32_t index, uint32_t mode, uint32_t group)
{
    uint32_t base = mode != 0 ? 0x800D76D0u : 0x800D7710u;
    uint32_t entry = base + (mode != 0 ? 32 : 24) * group;

    FUNCTION_MARKER(0x80033634, "SLUS_010.53");
    if (index >= 0)
    {
        entry += 8 * (uint32_t)index;
        rrj_write32(m, entry, 0xFFFFFFFFu);
        return entry;
    }
    return base;
}

uint32_t sub_800309E4(RRJMemory *m, uint32_t first_value, uint32_t second_value, uint32_t first_record, uint32_t second_record, uint32_t kind, RRJRaceLeafCall call)
{
    const uint32_t scratch = 0x1F8003F8u;
    uint32_t saved_first = rrj_read32(m, scratch);
    uint32_t saved_second = rrj_read32(m, scratch + 4);
    uint32_t selected;
    uint32_t first;
    uint32_t second;

    FUNCTION_MARKER(0x800309E4, "SLUS_010.53");
    selected = sub_80031170(m, scratch, 2, 1, kind, call);
    first = rrj_read32(m, scratch);
    second = rrj_read32(m, scratch + 4);
    rrj_write32(m, scratch, saved_first);
    rrj_write32(m, scratch + 4, saved_second);
    if (selected == 2)
    {
        rrj_write32(m, first, 51);
        rrj_write32(m, first + 16, rrj_read32(m, first + 12));
        rrj_write32(m, second, 51);
        rrj_write32(m, second + 16, rrj_read32(m, second + 12));
        rrj_write32(m, first + 20, rrj_read32(m, second + 12) + 0x3F60);
        rrj_write32(m, second + 20, rrj_read32(m, second + 12) + 0x3FE0);
        rrj_write32(m, first_value, rrj_read32(m, first + 16));
        rrj_write32(m, second_value, rrj_read32(m, second + 16));
        rrj_write32(m, first_record, rrj_read32(m, first + 4));
        rrj_write32(m, second_record, rrj_read32(m, second + 4));
        (void)frontier_call(m, call, 0x800322D0, 3, 1, 0);
        (void)frontier_call(m, call, 0x800322D0, 3, 1, 0);
        first = rrj_read32(m, 0x8005ACBC);
        rrj_write32(m, first + 4, rrj_read32(m, first + 4) + 2);
        return 1;
    }
    if (selected == 1)
    {
        rrj_write32(m, first, 0);
        (void)frontier_call(m, call, 0x800322D0, 3, 0, 0);
    }
    rrj_write32(m, first_value, 0);
    rrj_write32(m, second_value, 0);
    rrj_write32(m, first_record, 0xFFFFFFFFu);
    rrj_write32(m, second_record, 0xFFFFFFFFu);
    return 0;
}

uint32_t sub_80030BCC(RRJMemory *m, uint32_t value_output, uint32_t record_output, uint32_t kind, RRJRaceLeafCall call)
{
    const uint32_t scratch = 0x1F8003FCu;
    uint32_t saved = rrj_read32(m, scratch);
    uint32_t selected;
    uint32_t record;
    uint32_t value;
    uint32_t base;

    FUNCTION_MARKER(0x80030BCC, "SLUS_010.53");
    selected = sub_80031170(m, scratch, 1, 1, kind, call);
    record = rrj_read32(m, scratch);
    rrj_write32(m, scratch, saved);
    if (selected != 0)
    {
        value = rrj_read32(m, record + 12);
        rrj_write32(m, record, 51);
        rrj_write32(m, record + 16, value);
        rrj_write32(m, record + 20, value + 0x3FE0);
        rrj_write32(m, value_output, value);
        rrj_write32(m, record_output, rrj_read32(m, record + 4));
        (void)frontier_call(m, call, 0x800322D0, 3, 1, 0);
        base = rrj_read32(m, 0x8005ACBC);
        rrj_write32(m, base + 4, rrj_read32(m, base + 4) + 1);
    }
    else
    {
        rrj_write32(m, value_output, 0);
        rrj_write32(m, record_output, 0xFFFFFFFFu);
    }
    return selected;
}

static void frontier_remap_packet(RRJMemory *m, uint32_t packet, uint32_t size, uint32_t mode)
{
    uint16_t clut = rrj_u16(rrj_at(m, packet + 10, 2));
    uint16_t special = rrj_u16(rrj_at(m, 0x800D6160, 2));
    if (clut == special)
    {
        uint8_t delta = *(uint8_t *)rrj_at(m, 0x800D6168, 1);
        rrj_put16(rrj_at(m, packet + 10, 2), rrj_u16(rrj_at(m, 0x800D6162, 2)));
        if (mode == 0)
        {
            *(uint8_t *)rrj_at(m, packet + 5, 1) -= delta;
            *(uint8_t *)rrj_at(m, packet + 9, 1) -= delta;
            *(uint8_t *)rrj_at(m, packet + 13, 1) -= delta;
            if (size == 24)
                *(uint8_t *)rrj_at(m, packet + 15, 1) -= delta;
        }
        return;
    }
    {
        uint32_t mapped = rrj_read32(m, 0x800D8768u + 4 * (clut & 31));
        uint8_t command;
        if (mode != 0)
            mapped >>= (clut & 0x800) >> 7;
        rrj_put16(rrj_at(m, packet + 10, 2), (uint16_t)mapped);
        if (mode == 0)
            return;
        command = *(uint8_t *)rrj_at(m, packet, 1);
        if ((command >> 3) < 15)
            command = (uint8_t)(2 * (command & 0xF8) | (command & 7));
        else
            command = (uint8_t)(16 * ((command >> 3) - 15) | (command & 7));
        *(uint8_t *)rrj_at(m, packet, 1) = command;
        *(uint8_t *)rrj_at(m, packet + 5, 1) &= 0x7F;
        *(uint8_t *)rrj_at(m, packet + 9, 1) &= 0x7F;
        *(uint8_t *)rrj_at(m, packet + 13, 1) &= 0x7F;
        if (size == 24)
            *(uint8_t *)rrj_at(m, packet + 15, 1) &= 0x7F;
    }
}

int32_t sub_800348CC(RRJMemory *m, uint32_t record, uint32_t mode)
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
    if (rrj_read32(m, record + 8) == 0xFFFFFFFFu)
        return -1;
    descriptor = rrj_read32(m, record + 4);
    if (descriptor == 0)
        return 0;
    packet = rrj_read32(m, descriptor + (mode != 0 ? 56 : 60));
    if (packet == 0)
        return 0;
    flags = rrj_read32(m, record + 12);
    if ((flags & (mode != 0 ? 8u : 4u)) == 0)
        return mode != 0 ? -9 : -5;
    flags &= ~(mode != 0 ? 8u : 4u);
    rrj_write32(m, record + 12, flags);
    first_count = rrj_u16(rrj_at(m, descriptor + 4, 2));
    second_count = rrj_u16(rrj_at(m, descriptor + 6, 2));
    table = rrj_read32(m, descriptor + 40);
    segment = mode != 0 ? 0 : first_count + second_count;
    segment_end = mode != 0 ? first_count + second_count : first_count + 2 * second_count;
    while (segment < segment_end)
    {
        uint32_t entry = table + 12 * segment;
        uint32_t count = rrj_read32(m, entry + 4);
        while ((int32_t)count > 0)
        {
            frontier_remap_packet(m, packet, 20, mode);
            packet += 20;
            --count;
        }
        count = rrj_read32(m, entry + 8);
        while ((int32_t)count > 0)
        {
            frontier_remap_packet(m, packet, 24, mode);
            packet += 24;
            --count;
        }
        ++segment;
    }
    if (mode != 0)
    {
        flags = rrj_read32(m, record + 12) & ~0x40u;
        rrj_write32(m, record + 12, flags);
        return (int32_t)flags;
    }
    return 0;
}

uint32_t sub_80033304(RRJMemory *m, uint32_t owner, uint32_t mode, uint32_t group)
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
            if (mode <= 1 && rrj_read32(m, record + offset + 4 * candidate) == owner)
            {
                (void)sub_800348CC(m, record, mode);
                break;
            }
        }
    }
    return 0;
}

uint32_t sub_800326BC(RRJMemory *m, int32_t key, uint32_t group)
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
            uint32_t linked = rrj_read32(m, slot);
            if (linked != 0 && rrj_read32(m, linked + 8) == (uint32_t)key)
            {
                (void)sub_800348CC(m, record, (uint32_t)mode);
                rrj_write32(m, slot, 0);
            }
        }
    }
    return 1;
}

int32_t sub_80034D38(RRJMemory *m, int32_t key, uint32_t unused, uint32_t group)
{
    uint32_t index;
    uint32_t record;

    FUNCTION_MARKER(0x80034D38, "SLUS_010.53");
    index = sub_80022218(m, (uint32_t)key, group);
    if (index == 0xFFFFFFFFu)
        return -1;
    (void)sub_800326BC(m, key, group);
    record = 0x800D9268u + 48 * index;
    (void)sub_80033634(m, (int32_t)rrj_read32(m, record), rrj_read32(m, record + 4), group);
    rrj_write32(m, record + 8, 0xFFFFFFFFu);
    rrj_write32(m, record + 12, 0);
    rrj_write32(m, record + 16, 0);
    rrj_write32(m, record, 0xFFFFFFFFu);
    rrj_write32(m, record + 20, 0xFFFFFFFFu);
    (void)sub_8001E100(m, record + 24, 0xFFFFFFFFu, 24);
    return 1;
}

int32_t sub_80033770(RRJMemory *m, uint32_t page, uint32_t mode, uint32_t record_index, uint32_t group, RRJRaceLeafCall call)
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
    key = rrj_read32(m, slot);
    if (key == 0xFFFFFFFFu)
        return 1;
    (void)sub_80033304(m, key, mode != 0, group);
    for (index = 0; index < 4; ++index)
        saved[index] = rrj_read32(m, outputs + 4 * index);
    if (mode != 0)
        success = sub_80030BCC(m, outputs, outputs + 8, group, call);
    else
        success = sub_800309E4(m, outputs, outputs + 4, outputs + 8, outputs + 12, group, call);
    for (index = 0; index < 4; ++index)
    {
        values[index] = rrj_read32(m, outputs + 4 * index);
        rrj_write32(m, outputs + 4 * index, saved[index]);
    }
    rrj_write32(m, record, 0xFFFFFFFFu);
    if (success != 0)
    {
        if (mode != 0)
        {
            (void)sub_80022484(m, page, key, values[0], 0, group);
            rrj_write32(m, slot, 0xFFFFFFFFu);
            rrj_write32(m, record + 12, values[0]);
            (void)sub_80030C8C(m, values[2]);
        }
        else
        {
            (void)sub_80022484(m, page, key, values[0], values[1], group);
            rrj_write32(m, slot, 0xFFFFFFFFu);
            rrj_write32(m, record + 16, values[1]);
            rrj_write32(m, record + 12, values[0]);
            (void)sub_80030B7C(m, values[2], values[3]);
        }
        return 1;
    }
    if (rrj_read32(m, rrj_read32(m, 0x8005B2F8) + 48) == 2)
    {
        rrj_write32(m, 0x800D7720, key);
        (void)sub_800225E0(m, page, key, group);
        return -1;
    }
    index = sub_80022218(m, key, group);
    sub_80030CB8(m, (int32_t)rrj_read32(m, 0x800D9268u + 48 * index + 20));
    (void)sub_80034D38(m, (int32_t)key, mode, group);
    return -1;
}

int32_t sub_800339F8(RRJMemory *m, uint32_t mode, uint32_t group, RRJRaceLeafCall call)
{
    const uint32_t outputs = 0x1F8003E0u;
    uint32_t saved[4];
    uint32_t values[4];
    uint32_t key = rrj_read32(m, 0x800D7720);
    uint32_t record_index = sub_80022218(m, key, group);
    uint32_t selected;
    uint32_t record;
    uint32_t index;

    FUNCTION_MARKER(0x800339F8, "SLUS_010.53");
    if ((int32_t)record_index < 0)
        return -1;
    if (key == 0xFFFFFFFFu)
        return 1;
    for (index = 0; index < 4; ++index)
        saved[index] = rrj_read32(m, outputs + 4 * index);
    if (mode != 0)
        selected = sub_80030BCC(m, outputs, outputs + 8, group, call);
    else
        selected = sub_800309E4(m, outputs, outputs + 4, outputs + 8, outputs + 12, group, call);
    for (index = 0; index < 4; ++index)
    {
        values[index] = rrj_read32(m, outputs + 4 * index);
        rrj_write32(m, outputs + 4 * index, saved[index]);
    }
    if (selected == 0)
        return -1;
    record = 0x800D9268u + 48 * record_index;
    if (mode != 0)
    {
        (void)sub_80022484(m, 4, key, values[0], 0, 0);
        rrj_write32(m, 0x800D7720, 0xFFFFFFFFu);
        rrj_write32(m, record, 0xFFFFFFFFu);
        rrj_write32(m, record + 12, values[0]);
        (void)sub_80030C8C(m, values[2]);
    }
    else
    {
        (void)sub_80022484(m, 4, key, values[0], values[1], 0);
        rrj_write32(m, 0x800D7720, 0xFFFFFFFFu);
        rrj_write32(m, record, 0xFFFFFFFFu);
        rrj_write32(m, record + 16, values[1]);
        rrj_write32(m, record + 12, values[0]);
        (void)sub_80030B7C(m, values[2], values[3]);
    }
    return 1;
}

uint32_t sub_8003367C(RRJMemory *m, uint32_t owner, uint32_t indices, int32_t count, uint32_t mode)
{
    int32_t index;

    FUNCTION_MARKER(0x8003367C, "SLUS_010.53");
    for (index = 0; index < count; ++index)
    {
        uint32_t record = 0x800D87E8u + 112 * rrj_read32(m, indices + 4 * (uint32_t)index);
        uint32_t candidate;
        for (candidate = 0; candidate < 2; ++candidate)
        {
            if (rrj_read32(m, record + (mode != 0 ? 88 : 80) + 4 * candidate) == owner && (mode != 0 || (rrj_read32(m, record + 56) & 0x22222222u) != 0))
                return 1;
        }
    }
    return 0;
}

int32_t sub_8003348C(RRJMemory *m, uint32_t mode, uint32_t group)
{
    int32_t slot_count = (int32_t)rrj_read32(m, mode != 0 ? 0x8005AEE8 : 0x8005AEE4);
    uint32_t slots = (mode != 0 ? 0x800D76D0u + 32 * group : 0x800D7710u + 24 * group);
    int32_t slot;

    FUNCTION_MARKER(0x8003348C, "SLUS_010.53");
    for (slot = 0; slot < slot_count; ++slot)
    {
        if (rrj_read32(m, slots + 8 * (uint32_t)slot) == 0xFFFFFFFFu)
            return slot;
    }
    for (slot = 0; slot < slot_count; ++slot)
    {
        uint32_t owner = rrj_read32(m, slots + 8 * (uint32_t)slot);
        if (owner != 0xFFFFFFFFu && sub_8003367C(m, owner, 0x800D9B80u + 48 * group, (int32_t)rrj_read32(m, 0x8005B568u + 4 * group), mode) == 0)
            return slot;
    }
    return -1;
}

int32_t sub_80032F78(RRJMemory *m, uint32_t object, uint32_t mode, uint32_t group, RRJRaceLeafCall call)
{
    uint32_t slots;
    int32_t slot_count;
    uint32_t sources;
    uint32_t source_index;
    int32_t transfer_result = 0;

    FUNCTION_MARKER(0x80032F78, "SLUS_010.53");
    if (mode == 0 && (rrj_read32(m, object + 56) & 0x22222222u) == 0)
        return 1;
    slots = mode != 0 ? 0x800D76D0u + 32 * group : 0x800D7710u + 24 * group;
    slot_count = (int32_t)rrj_read32(m, mode != 0 ? 0x8005AEE8 : 0x8005AEE4);
    sources = object + (mode != 0 ? 88 : 80);
    for (source_index = 0; source_index < 2; ++source_index)
    {
        uint32_t key = rrj_read32(m, sources + 4 * source_index);
        int32_t slot_index;
        uint32_t record_index;
        uint32_t record;
        int32_t scan;
        uint32_t found = 0;
        if (key == 0xFFFFFFFFu)
            continue;
        for (scan = 0; scan < slot_count; ++scan)
        {
            if (rrj_read32(m, slots + 8 * (uint32_t)scan) == key)
                found = 1;
        }
        if (found != 0)
            continue;
        slot_index = sub_8003348C(m, mode, group);
        if (slot_index < 0)
            return -1;
        record_index = sub_80022218(m, key, group);
        if (record_index == 0xFFFFFFFFu)
            return -1;
        record = 0x800D9268u + 48 * record_index;
        if (rrj_read32(m, record + 12) == 0 || (mode == 0 && rrj_read32(m, record + 16) == 0))
            return -1;
        {
            uint32_t replaced = rrj_read32(m, slots + 8 * (uint32_t)slot_index);
            if (replaced != 0xFFFFFFFFu)
            {
                uint32_t replaced_index = sub_80022218(m, replaced, group);
                if (replaced_index != 0xFFFFFFFFu)
                    transfer_result = sub_80033770(m, (uint32_t)slot_index, mode, replaced_index, group, call);
            }
        }
        rrj_write32(m, slots + 8 * (uint32_t)slot_index, key);
        (void)frontier_call4(m, call, 0x80033B50, (uint32_t)slot_index, record_index, mode, group);
        if (transfer_result < 0)
            (void)sub_800339F8(m, mode, group, call);
    }
    return 1;
}

uint32_t sub_80032D24(RRJMemory *m, uint32_t group, RRJRaceLeafCall call)
{
    int32_t entry;
    int32_t active;
    uint32_t list = 0x800D9B80u + 48 * group;

    FUNCTION_MARKER(0x80032D24, "SLUS_010.53");
    active = 0;
    for (entry = 0; entry < rrj_s32(rrj_read32(m, 0x8005B568u + 4 * group)); ++entry)
    {
        int32_t object_index = rrj_s32(rrj_read32(m, list + 4 * (uint32_t)entry));
        uint32_t object;
        uint32_t flags;

        if (object_index == -1)
            continue;
        object = 0x800D87E8u + 112 * (uint32_t)object_index;
        flags = rrj_read32(m, object + 56);
        if ((flags & 0x22222222u) == 0)
            continue;
        if (active < 4)
        {
            if (sub_80032F78(m, object, 0, group, call) == -1)
                rrj_write32(m, object + 56, flags & 0xDDDDDDDDu);
            ++active;
        }
        else
        {
            rrj_write32(m, object + 56, flags & 0xDDDDDDDDu);
        }
    }

    active = 0;
    for (entry = 0; entry < rrj_s32(rrj_read32(m, 0x8005B568u + 4 * group)); ++entry)
    {
        uint32_t list_entry = list + 4 * (uint32_t)entry;
        int32_t object_index;

        if (active >= 6)
        {
            (void)frontier_call(m, call, 0x80044894u, 0x80010DCCu, 0, 0);
            continue;
        }
        object_index = rrj_s32(rrj_read32(m, list_entry));
        if (object_index == -1)
            continue;
        if (sub_80032F78(m, 0x800D87E8u + 112 * (uint32_t)object_index, 1, group, call) == -1)
            rrj_write32(m, list_entry, 0xFFFFFFFFu);
        ++active;
    }

    for (active = 0; active < 2; ++active)
    {
        uint32_t channel = sub_800335B4(m, (uint32_t)active, 0xFFFFFFFFu, group);

        if (channel != 0xFFFFFFFFu)
        {
            int32_t slot = sub_8003328C(m, (uint32_t)active, (int32_t)group);

            if (slot != -1)
                (void)frontier_call4(m, call, 0x80033B50u, channel, (uint32_t)slot, (uint32_t)active, group);
        }
    }
    return 1;
}

uint32_t sub_80066A84(RRJMemory *m, uint32_t object)
{
    int32_t selector = *(int8_t *)rrj_at(m, object + 8, 1);
    uint32_t table = rrj_read32(m, rrj_read32(m, object + 96) + 8);
    uint32_t descriptor = rrj_read32(m, table + 12 * (uint32_t)selector);
    uint32_t type = rrj_u16(rrj_at(m, descriptor + 24, 2));
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
    record = rrj_read32(m, rrj_read32(m, object + 4));
    return rrj_read32(m, rrj_read32(m, object) + 36) + 8 * rrj_read32(m, record + 16) + 4 + 8 * slot;
}

int32_t sub_8001FD24(RRJMemory *m, uint32_t first, uint32_t second, uint32_t third, uint32_t output)
{
    uint32_t first_entry = 0x8005624Cu + 4 * (first & 0xFFFu);
    uint32_t second_entry = 0x8005624Cu + 4 * (second & 0xFFFu);
    uint32_t third_entry = 0x8005624Cu + 4 * (third & 0xFFFu);
    int32_t first_x = (int16_t)rrj_u16(rrj_at(m, first_entry, 2));
    int32_t first_y = (int16_t)rrj_u16(rrj_at(m, first_entry + 2, 2));
    int32_t second_x = (int16_t)rrj_u16(rrj_at(m, second_entry, 2));
    int32_t second_y = (int16_t)rrj_u16(rrj_at(m, second_entry + 2, 2));
    int32_t third_x = (int16_t)rrj_u16(rrj_at(m, third_entry, 2));
    int32_t third_y = (int16_t)rrj_u16(rrj_at(m, third_entry + 2, 2));
    int32_t first_x_second_x = frontier_mul_shift12(first_x, second_x);
    int32_t third_y_first_y = frontier_mul_shift12(third_y, first_y);
    int32_t result;

    FUNCTION_MARKER(0x8001FD24, "SLUS_010.53");
    rrj_put16(rrj_at(m, output + 12, 2), (uint16_t)(-second_x));
    rrj_put16(rrj_at(m, output + 14, 2), (uint16_t)frontier_mul_shift12(first_x, second_y));
    rrj_put16(rrj_at(m, output + 16, 2), (uint16_t)frontier_mul_shift12(first_y, second_y));
    rrj_put16(rrj_at(m, output + 2, 2), (uint16_t)(frontier_mul_shift12(first_x_second_x, third_y) - frontier_mul_shift12(third_x, first_y)));
    rrj_put16(rrj_at(m, output, 2), (uint16_t)frontier_mul_shift12(third_y, second_y));
    rrj_put16(rrj_at(m, output + 8, 2), (uint16_t)(third_y_first_y + frontier_mul_shift12(first_x_second_x, third_x)));
    rrj_put16(rrj_at(m, output + 6, 2), (uint16_t)frontier_mul_shift12(second_y, third_x));
    rrj_put16(rrj_at(m, output + 4, 2), (uint16_t)(frontier_mul_shift12(first_x, third_x) + frontier_mul_shift12(third_y_first_y, second_x)));
    result = frontier_mul_shift12(frontier_mul_shift12(third_x, first_y), second_x) - frontier_mul_shift12(first_x, third_y);
    rrj_put16(rrj_at(m, output + 10, 2), (uint16_t)result);
    return result;
}

uint32_t sub_80066B98(RRJMemory *m, uint32_t object, uint32_t child_index)
{
    uint32_t child = rrj_read32(m, object + 56 + 8 * child_index);
    uint32_t specification = rrj_read32(m, child);
    uint32_t scale = rrj_u16(rrj_at(m, specification + 14, 2)) >> 12;
    uint32_t parent = rrj_read32(m, object + 4);
    uint32_t attachment = sub_80066A84(m, object);
    uint32_t attachment_scale;
    int16_t input[3];
    int32_t transformed[3];
    int32_t difference;
    uint32_t index;

    FUNCTION_MARKER(0x80066B98, "RASHCDG.BIN");
    for (index = 0; index != 3; ++index)
        input[index] = (int16_t)frontier_asr((uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, child + 28 + 2 * index, 2)), (4u - scale) & 31u);
    frontier_rotate_vector(m, parent + 52, input, transformed);
    for (index = 0; index != 3; ++index)
        rrj_write32(m, child + 12 + 4 * index, (uint32_t)transformed[index]);

    if (attachment != 0)
    {
        attachment_scale = rrj_u16(rrj_at(m, rrj_read32(m, object) + 14, 2)) >> 12;
    }
    else
    {
        uint32_t descriptor = rrj_read32(m, rrj_read32(m, rrj_read32(m, object + 96) + 8));
        uint32_t record = rrj_read32(m, parent);
        uint32_t base = rrj_read32(m, descriptor + 36) + 4 + 8 * rrj_read32(m, record + 16);

        attachment_scale = rrj_u16(rrj_at(m, descriptor + 14, 2)) >> 12;
        attachment = base + (rrj_u16(rrj_at(m, descriptor + 24, 2)) < 6 ? 24 : 32);
    }

    difference = (int32_t)scale - (int32_t)attachment_scale;
    for (index = 0; index != 3; ++index)
    {
        int32_t value = (int16_t)rrj_u16(rrj_at(m, attachment + 2 * index, 2));
        uint32_t adjusted = difference < 0 ? frontier_asr((uint32_t)value, (uint32_t)(-difference) & 31u) : (uint32_t)value << ((uint32_t)difference & 31u);
        uint32_t base = rrj_u16(rrj_at(m, child + 12 + 4 * index, 2));

        input[index] = (int16_t)(base + adjusted);
    }
    if (*(int8_t *)rrj_at(m, object + 72, 1) == 3)
        frontier_rotate_vector(m, object + 104, input, transformed);
    else
        frontier_rotate_vector(m, parent + 4, input, transformed);
    for (index = 0; index != 3; ++index)
    {
        uint32_t value = (uint32_t)transformed[index] + (rrj_read32(m, object + 12 + 4 * index) << (scale & 31u));
        rrj_write32(m, child + 12 + 4 * index, value);
    }
    frontier_copy_words(m, child + 104, rrj_read32(m, 0x1F8002BCu), 32);
    return child + 104;
}

uint32_t sub_80067064(RRJMemory *m, uint32_t mode, uint32_t object)
{
    uint32_t configuration = 0x800CC854u + 3 * mode;
    int32_t inner_count = *(uint8_t *)rrj_at(m, configuration + 1, 1);
    uint32_t stream = 0x800CC790u + 4 * *(uint8_t *)rrj_at(m, configuration, 1);
    int32_t outer_count = *(uint8_t *)rrj_at(m, configuration + 2, 1);
    uint32_t root = rrj_read32(m, object + 4);
    uint32_t descriptor_entry = root + 24;
    uint32_t result = 0x800CC790u;

    FUNCTION_MARKER(0x80067064, "RASHCDG.BIN");
    while (outer_count > 0)
    {
        int32_t remaining = inner_count;

        while (remaining > 0)
        {
            uint32_t command = rrj_read32(m, stream);
            uint32_t scratch = rrj_read32(m, 0x1F8002BCu) + 32 * (command & 3u);
            uint32_t model_entry = root + 24 * ((command >> 13) & 31u);
            uint32_t model = rrj_read32(m, model_entry);
            uint32_t model_index = rrj_read32(m, model + 16);
            uint32_t source = rrj_read32(m, 0x8005ACB0u) + 16 * model_index + ((command >> 6) & 0x70u);
            uint32_t matrix_input;
            uint32_t matrix_output;
            uint32_t descriptor;
            int16_t rotation[3][3];
            uint32_t row;
            uint32_t column;

            rrj_write32(m, scratch + 20, rrj_read32(m, source));
            rrj_write32(m, scratch + 24, rrj_read32(m, source + 4));
            rrj_write32(m, scratch + 28, rrj_read32(m, source + 8));
            stream += 4;
            matrix_input = scratch + ((command << 3) & 0x60u);
            rrj_write32(m, 0x1F8002BCu, matrix_input);
            frontier_copy_words(m, matrix_input, root + 24 * ((command >> 23) & 31u) + 4, 20);
            matrix_output = matrix_input - ((command >> 3) & 0x60u);
            for (row = 0; row != 3; ++row)
                for (column = 0; column != 3; ++column)
                    rotation[row][column] = (int16_t)rrj_u16(rrj_at(m, matrix_input - 32 + 2 * (3 * row + column), 2));
            for (column = 0; column != 3; ++column)
            {
                int16_t input[3];
                int32_t transformed[3];

                for (row = 0; row != 3; ++row)
                    input[row] = (int16_t)rrj_u16(rrj_at(m, matrix_input + 2 * (3 * row + column), 2));
                frontier_rotate_values(rotation, input, transformed);
                for (row = 0; row != 3; ++row)
                    rrj_put16(rrj_at(m, matrix_output + 2 * (3 * row + column), 2), (uint16_t)transformed[row]);
            }

            descriptor = rrj_read32(m, descriptor_entry);
            descriptor_entry += 24;
            --remaining;
            inner_count = (int32_t)rrj_read32(m, descriptor + 16);
            scratch = rrj_read32(m, 0x1F8002BCu) - ((command << 1) & 0x60u);
            rrj_write32(m, 0x1F8002BCu, scratch);
            (void)sub_800220A4(m, scratch, rrj_u16(rrj_at(m, descriptor + 14, 2)), rrj_read32(m, rrj_read32(m, object) + 36) + 8 * (uint32_t)inner_count + 4, rrj_read32(m, 0x8005ACB0u) + 16 * (uint32_t)inner_count, rrj_read32(m, 0x8005ACB4u) + 4 * (uint32_t)inner_count, rrj_read32(m, 0x8005ACB8u) + (uint32_t)inner_count);
            rrj_write32(m, 0x1F8002BCu, rrj_read32(m, 0x1F8002BCu) - ((command >> 1) & 0x60u));
        }

        result = 3;
        {
            uint32_t command = rrj_read32(m, stream);

            if ((command & 3u) == 3)
            {
                uint32_t linked = rrj_read32(m, object + 56);

                stream += 4;
                if (linked != 0 && rrj_read32(m, object + 60) == ((command >> 18) & 31u))
                    frontier_copy_words(m, linked + 104, rrj_read32(m, 0x1F8002BCu), 32);
                inner_count = (int32_t)((command >> 13) & 31u);
                result = (command >> 1) & 0x60u;
                rrj_write32(m, 0x1F8002BCu, rrj_read32(m, 0x1F8002BCu) - result);
            }
        }
        --outer_count;
    }
    return result;
}

uint32_t sub_80066B28(RRJMemory *m, uint32_t object)
{
    uint32_t type = rrj_u16(rrj_at(m, rrj_read32(m, object) + 24, 2));
    uint32_t scratch;

    FUNCTION_MARKER(0x80066B28, "RASHCDG.BIN");
    if (type == 4)
        return sub_80067064(m, 6, object);
    if (type == 3)
        return sub_80067064(m, 7, object);
    scratch = rrj_read32(m, 0x1F8002BCu) - 32;
    rrj_write32(m, 0x1F8002BCu, scratch);
    return scratch;
}

uint32_t sub_8006745C(RRJMemory *m, uint32_t object)
{
    uint32_t type = rrj_u16(rrj_at(m, rrj_read32(m, object) + 24, 2));

    FUNCTION_MARKER(0x8006745C, "RASHCDG.BIN");
    if (type == 12)
        return sub_80067064(m, 1, object);
    if (type >= 13)
        return type == 17 ? sub_80067064(m, 0, object) : 17;
    if (type == 4)
        return sub_80067064(m, 2, object);
    return 4;
}

uint32_t sub_80066ECC(RRJMemory *m, uint32_t object)
{
    uint32_t context = rrj_read32(m, 0x8005B2F8u);
    uint32_t scratch = rrj_read32(m, 0x1F8002BCu);
    uint32_t type = rrj_u16(rrj_at(m, rrj_read32(m, object) + 24, 2));
    uint32_t parent = rrj_read32(m, object + 4);
    uint32_t child_index;

    FUNCTION_MARKER(0x80066ECC, "RASHCDG.BIN");
    if (rrj_u16(rrj_at(m, object + 172, 2)) < rrj_read32(m, context + 48))
        frontier_copy_words(m, 0x800CCD80u, scratch, 32);
    if (type >= 5)
    {
        uint32_t extended = type >= 6;

        (void)sub_8001FD24(m, (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, object + 836, 2)), 0, 0, parent + 76);
        (void)sub_8001FD24(m, (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, object + 838, 2)), 0, 0, parent + 100);
        if (extended)
        {
            uint32_t linked = rrj_read32(m, object + 856);

            (void)sub_8001FD24(m, (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, linked + 836, 2)), 0, 0, parent + 124);
        }
        (void)sub_80067064(m, 4 - extended, object);
    }
    else if (type != 1)
    {
        (void)sub_80067064(m, 5, object);
    }
    for (child_index = 0; child_index != 2; ++child_index)
    {
        if (rrj_read32(m, object + 56 + 8 * child_index) != 0)
            (void)sub_80066B98(m, object, child_index);
    }
    if (type != 1)
    {
        scratch = rrj_read32(m, 0x1F8002BCu) - 32;
        rrj_write32(m, 0x1F8002BCu, scratch);
    }
    scratch = rrj_read32(m, 0x1F8002BCu) - 32;
    rrj_write32(m, 0x1F8002BCu, scratch);
    return scratch;
}

uint32_t sub_80066EC4(RRJMemory *m, uint32_t object)
{
    FUNCTION_MARKER(0x80066EC4, "RASHCDG.BIN");
    return sub_80066ECC(m, object);
}

uint32_t sub_80029048(RRJMemory *m, uint32_t output, uint32_t center, uint32_t angle, int32_t radius)
{
    uint32_t table = 0x8005624Cu + 4 * (angle & 0xFFFu);
    int32_t scaled_radius = (int32_t)((uint32_t)radius << 11);
    int32_t x_offset = (int32_t)(((int64_t)(int16_t)rrj_u16(rrj_at(m, table, 2)) * scaled_radius) >> 24);
    int32_t y_offset = (int32_t)(((int64_t)(int16_t)rrj_u16(rrj_at(m, table + 2, 2)) * scaled_radius) >> 24);
    uint32_t x = rrj_u16(rrj_at(m, center, 2));
    uint32_t y = rrj_u16(rrj_at(m, center + 2, 2));
    uint32_t z = rrj_u16(rrj_at(m, center + 4, 2));

    FUNCTION_MARKER(0x80029048, "SLUS_010.53");
    rrj_put16(rrj_at(m, output, 2), (uint16_t)(x - (uint32_t)x_offset));
    rrj_put16(rrj_at(m, output + 2, 2), (uint16_t)(y + (uint32_t)y_offset));
    rrj_put16(rrj_at(m, output + 4, 2), (uint16_t)z);
    rrj_put16(rrj_at(m, output + 8, 2), (uint16_t)(x + (uint32_t)y_offset));
    rrj_put16(rrj_at(m, output + 10, 2), (uint16_t)(y + (uint32_t)x_offset));
    rrj_put16(rrj_at(m, output + 12, 2), (uint16_t)z);
    rrj_put16(rrj_at(m, output + 24, 2), (uint16_t)(x + (uint32_t)x_offset));
    rrj_put16(rrj_at(m, output + 26, 2), (uint16_t)(y - (uint32_t)y_offset));
    rrj_put16(rrj_at(m, output + 28, 2), (uint16_t)z);
    rrj_put16(rrj_at(m, output + 16, 2), (uint16_t)(x - (uint32_t)y_offset));
    rrj_put16(rrj_at(m, output + 18, 2), (uint16_t)(y - (uint32_t)x_offset));
    rrj_put16(rrj_at(m, output + 20, 2), (uint16_t)z);
    return z;
}

uint32_t sub_8002990C(RRJMemory *m, uint32_t object, uint32_t output)
{
    uint32_t command = rrj_read32(m, output);
    uint32_t type = (command >> 6) & 15u;
    uint32_t vertex_base = rrj_read32(m, 0x8005ACB0u);

    FUNCTION_MARKER(0x8002990C, "SLUS_010.53");
    if (type == 4)
    {
        uint32_t first = vertex_base + 16 * rrj_u16(rrj_at(m, output + 76, 2));
        uint32_t second = vertex_base + 16 * rrj_u16(rrj_at(m, output + 78, 2));
        uint32_t index;
        uint32_t result = 0;

        for (index = 0; index != 3; ++index)
        {
            result = (uint32_t)(rrj_s32(rrj_read32(m, first + 4 * index) + rrj_read32(m, second + 4 * index)) / 2);
            rrj_write32(m, output + 80 + 4 * index, result);
        }
        return result;
    }
    if (type == 3 || type == 6)
    {
        uint32_t root = rrj_read32(m, object + 4);
        uint32_t record_index = type == 3 ? (command >> 14) & 0xFFu : rrj_u16(rrj_at(m, rrj_read32(m, object) + 24, 2)) - 1u;
        uint32_t record = rrj_read32(m, root + 24 * record_index);
        uint32_t vertex_index = rrj_u16(rrj_at(m, record + 16, 2)) + (type == 3);
        uint32_t source = vertex_base + 16 * vertex_index;

        rrj_put16(rrj_at(m, output + 76, 2), (uint16_t)vertex_index);
        frontier_copy_words(m, output + 80, source, 16);
        return source;
    }
    if (type != 5)
        return type < 5 ? 3 : 6;
    {
        uint32_t specification = rrj_read32(m, object);
        uint32_t scale = rrj_u16(rrj_at(m, specification + 14, 2)) >> 12;
        int32_t object_part = *(int8_t *)rrj_at(m, object + 8, 1);
        uint32_t configuration = 0x80053728u + 2 * (uint32_t)object_part + ((command >> 11) & 0x7F8u);
        int32_t model_part = *(int8_t *)rrj_at(m, configuration, 1);
        uint32_t screen_base = rrj_read32(m, 0x8005ACB4u);
        uint32_t route;
        int32_t record_index;
        uint32_t model;
        uint32_t part;
        int32_t visible;
        uint32_t first_vertex;
        int32_t extent;

        if (model_part == -1)
        {
            rrj_write32(m, output + 92, 0xFFFFFFFFu);
            return 0x80053728u;
        }
        route = 0x800536F0u + 4 * (uint32_t)(int32_t)*(int8_t *)rrj_at(m, configuration + 1, 1);
        record_index = (int32_t)rrj_u16(rrj_at(m, specification + 24, 2)) - 1;
        if (*(int8_t *)rrj_at(m, route, 1) != -1)
            record_index = *(int8_t *)rrj_at(m, route, 1);
        model = rrj_read32(m, rrj_read32(m, object + 4) + 24 * (uint32_t)record_index);
        part = rrj_read32(m, model + 20) + 20 * (uint32_t)model_part + 4;
        visible = (int32_t)frontier_asr(rrj_read32(m, vertex_base + 16 * rrj_u16(rrj_at(m, part + 12, 2)) + 8), scale & 31u);
        rrj_write32(m, output + 92, (uint32_t)visible);
        if (visible <= 0)
            return (uint32_t)visible;
        first_vertex = screen_base + 4 * rrj_u16(rrj_at(m, part + 12 + 2 * (uint32_t)(int32_t)*(int8_t *)rrj_at(m, route + 2, 1), 2));
        if (*(int8_t *)rrj_at(m, route + 1, 1) == 1)
        {
            rrj_put16(rrj_at(m, output + 80, 2), rrj_u16(rrj_at(m, first_vertex, 2)));
            rrj_put16(rrj_at(m, output + 82, 2), rrj_u16(rrj_at(m, first_vertex + 2, 2)));
        }
        else
        {
            uint32_t second_vertex = screen_base + 4 * rrj_u16(rrj_at(m, part + 12 + 2 * (uint32_t)(int32_t)*(int8_t *)rrj_at(m, route + 3, 1), 2));
            int32_t x = (int16_t)rrj_u16(rrj_at(m, first_vertex, 2)) + (int16_t)rrj_u16(rrj_at(m, second_vertex, 2));
            int32_t y = (int16_t)rrj_u16(rrj_at(m, first_vertex + 2, 2)) + (int16_t)rrj_u16(rrj_at(m, second_vertex + 2, 2));

            rrj_put16(rrj_at(m, output + 80, 2), (uint16_t)(x / 2));
            rrj_put16(rrj_at(m, output + 82, 2), (uint16_t)(y / 2));
        }
        if (object_part >= 2)
        {
            extent = object_part == 2 ? 6 : 4;
            rrj_write32(m, output + 88, (uint32_t)extent);
            return 2;
        }
        {
            uint32_t second_vertex = screen_base + 4 * rrj_u16(rrj_at(m, part + 16, 2));
            uint32_t base_vertex = screen_base + 4 * rrj_u16(rrj_at(m, part + 12, 2));
            int32_t x = (int16_t)rrj_u16(rrj_at(m, base_vertex, 2)) - (int16_t)rrj_u16(rrj_at(m, second_vertex, 2));
            int32_t y = (int16_t)rrj_u16(rrj_at(m, base_vertex + 2, 2)) - (int16_t)rrj_u16(rrj_at(m, second_vertex + 2, 2));
            int32_t half_x = (x < 0 ? -x : x) >> 1;
            int32_t half_y = (y < 0 ? -y : y) >> 1;

            extent = half_y < half_x ? half_x : half_y;
            rrj_write32(m, output + 88, (uint32_t)extent);
            return half_y < half_x;
        }
    }
}

int32_t sub_80029CA4(RRJMemory *m, uint32_t object)
{
    int32_t index = *(int8_t *)rrj_at(m, object + 73, 1);
    int32_t result = -1;

    FUNCTION_MARKER(0x80029CA4, "SLUS_010.53");
    if (index == -1)
        return result;
    if (*(int8_t *)rrj_at(m, object + 8, 1) > 0)
    {
        uint32_t entry = 0x800D39B0u + 112 * (uint32_t)index;

        result = ((rrj_read32(m, entry) >> 6) & 15u) < 5u;
        if (result != 0)
            return result;
    }
    while (index != -1)
    {
        uint32_t entry = 0x800D39B0u + 112 * (uint32_t)index;
        uint32_t command = rrj_read32(m, entry);

        index = rrj_s32(command << 26) >> 26;
        (void)sub_8002990C(m, object, entry);
        result = 8 * index;
    }
    return result;
}

uint32_t sub_80028534(RRJMemory *m, uint32_t object)
{
    uint32_t specification = rrj_read32(m, object);
    uint32_t configuration = 0x800537D8u + 6 * rrj_read32(m, object + 180);
    uint32_t root = rrj_read32(m, object + 4);
    uint32_t type = rrj_u16(rrj_at(m, specification + 24, 2));
    uint32_t record = rrj_read32(m, root + 24 * (type - 1));
    uint32_t part = rrj_read32(m, record + 20) + 20 * (uint32_t)(int32_t)*(int8_t *)rrj_at(m, configuration, 1) + 4;
    uint32_t vertex_base = rrj_read32(m, 0x8005ACB0u);
    uint32_t screen_base = rrj_read32(m, 0x8005ACB4u);
    uint32_t route = 0x800536F0u + 4 * (uint32_t)(int32_t)*(int8_t *)rrj_at(m, configuration + 1, 1);
    uint32_t scale = rrj_u16(rrj_at(m, specification + 14, 2)) >> 12;
    int32_t depth = (int32_t)frontier_asr(rrj_read32(m, vertex_base + 16 * rrj_u16(rrj_at(m, part + 12, 2)) + 8), scale & 31u);

    FUNCTION_MARKER(0x80028534, "SLUS_010.53");
    if (depth > 0)
    {
        uint32_t first = screen_base + 4 * rrj_u16(rrj_at(m, part + 12 + 2 * (uint32_t)(int32_t)*(int8_t *)rrj_at(m, route + 2, 1), 2));
        uint32_t second = screen_base + 4 * rrj_u16(rrj_at(m, part + 12 + 2 * (uint32_t)(int32_t)*(int8_t *)rrj_at(m, route + 3, 1), 2));
        int32_t first_x = (int16_t)rrj_u16(rrj_at(m, first, 2));
        int32_t first_y = (int16_t)rrj_u16(rrj_at(m, first + 2, 2));
        int32_t second_x = (int16_t)rrj_u16(rrj_at(m, second, 2));
        int32_t second_y = (int16_t)rrj_u16(rrj_at(m, second + 2, 2));
        int32_t center_x = (first_x + second_x) / 2;
        int32_t center_y = (first_y + second_y) / 2;
        int32_t delta_x = first_x - second_x;
        int32_t delta_y = first_y - second_y;
        int32_t radius;
        uint32_t center = 0x1F800030u;
        uint32_t quad = 0x1F800010u;
        uint32_t display;
        uint32_t packet;
        uint32_t ordering_table;
        int32_t bucket;
        uint32_t entry;
        uint8_t u0;
        uint8_t v0;
        uint8_t u1;
        uint8_t v1;

        if (delta_x < 0)
            delta_x = -delta_x;
        if (delta_y < 0)
            delta_y = -delta_y;
        radius = 2 * (delta_y >= delta_x ? delta_y : delta_x);
        if ((rrj_s32(rrj_read32(m, 0x80053250u)) >> 2) < depth)
        {
            int32_t quotient = (int32_t)((uint32_t)radius << 12) / depth;
            int32_t scale_value = (int32_t)(rrj_read32(m, 0x80053250u) << 12);

            radius = (int32_t)(((int64_t)quotient * scale_value) >> 24);
        }
        rrj_put16(rrj_at(m, center, 2), (uint16_t)center_x);
        rrj_put16(rrj_at(m, center + 2, 2), (uint16_t)center_y);
        rrj_put16(rrj_at(m, center + 4, 2), 0);
        (void)sub_80029048(m, quad, center, (uint32_t)((center_x + center_y) * 8), radius);

        display = rrj_read32(m, 0x8005B470u);
        packet = rrj_read32(m, display + 268);
        ordering_table = rrj_read32(m, display + 264);
        if (packet + 40 >= rrj_read32(m, 0x8005B4D0u))
            packet = sub_80021C98(m, packet, 40);
        if (rrj_s32(rrj_read32(m, 0x1F800004u)) < 4096)
        {
            int32_t exponent = rrj_s32(rrj_read32(m, 0x1F800000u)) + *(int8_t *)rrj_at(m, (depth >> 12) > 0 ? 0x1F800019u : 0x1F800018u, 1) + ((depth >> 11) > 0);
            int32_t table_index = exponent;
            int32_t base;

            if (table_index < 0)
                table_index = 0;
            if (table_index > 3)
                table_index = 3;
            base = rrj_u16(rrj_at(m, 0x1F800006u + 2 * (uint32_t)table_index, 2)) + rrj_s32(rrj_read32(m, 0x1F800004u));
            bucket = rrj_u16(rrj_at(m, 0x1F80000Eu + 2 * (uint32_t)table_index, 2)) + (rrj_s32((uint32_t)(depth - base)) >> (exponent & 31));
        }
        else
        {
            bucket = rrj_s32((uint32_t)(depth - rrj_s32(rrj_read32(m, 0x1F800004u)))) >> (rrj_read32(m, 0x1F800000u) & 31u);
        }
        if (bucket < 0)
            bucket = 0;
        if (bucket > rrj_s32(rrj_read32(m, 0x1F80001Cu)))
            bucket = rrj_s32(rrj_read32(m, 0x1F80001Cu));
        if (bucket >= 3)
            bucket -= 3;
        entry = ordering_table + 4 * (uint32_t)bucket;
        rrj_write32(m, packet + 4, 0x2E000060u);
        rrj_write32(m, packet, rrj_read32(m, entry) | 0x09000000u);
        rrj_write32(m, entry, packet);
        rrj_put16(rrj_at(m, packet + 8, 2), rrj_u16(rrj_at(m, quad, 2)));
        rrj_put16(rrj_at(m, packet + 10, 2), rrj_u16(rrj_at(m, quad + 2, 2)));
        rrj_put16(rrj_at(m, packet + 16, 2), rrj_u16(rrj_at(m, quad + 8, 2)));
        rrj_put16(rrj_at(m, packet + 18, 2), rrj_u16(rrj_at(m, quad + 10, 2)));
        rrj_put16(rrj_at(m, packet + 24, 2), rrj_u16(rrj_at(m, quad + 16, 2)));
        rrj_put16(rrj_at(m, packet + 26, 2), rrj_u16(rrj_at(m, quad + 18, 2)));
        rrj_put16(rrj_at(m, packet + 32, 2), rrj_u16(rrj_at(m, quad + 24, 2)));
        rrj_put16(rrj_at(m, packet + 34, 2), rrj_u16(rrj_at(m, quad + 26, 2)));
        u0 = *(uint8_t *)rrj_at(m, 0x800D42FCu, 1);
        v0 = *(uint8_t *)rrj_at(m, 0x800D4300u, 1);
        u1 = (uint8_t)(u0 + *(uint8_t *)rrj_at(m, 0x800D4304u, 1) - 1);
        v1 = (uint8_t)(v0 + *(uint8_t *)rrj_at(m, 0x800D4308u, 1) - 1);
        *(uint8_t *)rrj_at(m, packet + 12, 1) = u0;
        *(uint8_t *)rrj_at(m, packet + 13, 1) = v0;
        *(uint8_t *)rrj_at(m, packet + 20, 1) = u1;
        *(uint8_t *)rrj_at(m, packet + 21, 1) = v0;
        *(uint8_t *)rrj_at(m, packet + 28, 1) = u0;
        *(uint8_t *)rrj_at(m, packet + 29, 1) = v1;
        *(uint8_t *)rrj_at(m, packet + 36, 1) = u1;
        *(uint8_t *)rrj_at(m, packet + 37, 1) = v1;
        rrj_put16(rrj_at(m, packet + 22, 2), rrj_u16(rrj_at(m, 0x800D430Cu, 2)) | 0x20u);
        rrj_put16(rrj_at(m, packet + 14, 2), rrj_u16(rrj_at(m, 0x800D4310u, 2)));
        rrj_write32(m, display + 268, packet + 40);
        return display;
    }
    return 0x800536F0u;
}

uint32_t sub_80027B80(RRJMemory *m, uint32_t object)
{
    int32_t object_part = *(int8_t *)rrj_at(m, object + 8, 1);
    uint32_t scale = rrj_u16(rrj_at(m, rrj_read32(m, object) + 14, 2)) >> 12;
    uint32_t screen = rrj_read32(m, 0x8005ACB4u);
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
        int32_t horizontal = (int16_t)rrj_u16(rrj_at(m, screen + 56, 2)) - (int16_t)rrj_u16(rrj_at(m, screen + 32, 2));
        int32_t vertical = (int16_t)rrj_u16(rrj_at(m, screen + 54, 2)) - (int16_t)rrj_u16(rrj_at(m, screen + 58, 2));

        texture = 0x800D42C4u;
        texture_index = 14;
        x = (int16_t)rrj_u16(rrj_at(m, screen + 56, 2));
        y = (int16_t)(rrj_u16(rrj_at(m, screen + 58, 2)) + 6);
        height = -10;
        if (vertical < 0)
            vertical = -vertical;
        width = horizontal * vertical;
    }
    else if (object_part == 7)
    {
        int32_t horizontal = (int16_t)rrj_u16(rrj_at(m, screen + 16, 2)) - (int16_t)rrj_u16(rrj_at(m, screen + 4, 2));
        int32_t vertical = (int16_t)rrj_u16(rrj_at(m, screen + 14, 2)) - (int16_t)rrj_u16(rrj_at(m, screen + 18, 2));

        texture = 0x800D4388u;
        texture_index = 4;
        x = (int16_t)rrj_u16(rrj_at(m, screen + 16, 2));
        y = (int16_t)(rrj_u16(rrj_at(m, screen + 18, 2)) + 4);
        height = -10;
        if (vertical < 0)
            vertical = -vertical;
        width = 2 * horizontal * vertical;
    }
    else if (object_part == 8)
    {
        int32_t horizontal = (int16_t)rrj_u16(rrj_at(m, screen + 4, 2)) - (int16_t)rrj_u16(rrj_at(m, screen + 16, 2));
        int32_t vertical = (int16_t)rrj_u16(rrj_at(m, screen + 10, 2)) - (int16_t)rrj_u16(rrj_at(m, screen + 6, 2));

        texture = 0x800D42E0u;
        texture_index = 1;
        x = (int16_t)rrj_u16(rrj_at(m, screen + 4, 2));
        y = (int16_t)(rrj_u16(rrj_at(m, screen + 6, 2)) - 4);
        height = 10;
        if (vertical < 0)
            vertical = -vertical;
        width = horizontal * (vertical + 3);
    }
    else
    {
        return 0x800D0000u;
    }
    rrj_write32(m, 0x8005B530u, texture_index);
    depth = (int32_t)frontier_asr(rrj_read32(m, rrj_read32(m, 0x8005ACB0u) + 16 * texture_index + 8), scale & 31u);
    result = width < 201;
    if (depth > 0)
    {
        uint32_t display;
        uint32_t packet;
        uint32_t ordering_table;
        int32_t bucket;
        uint32_t entry;
        uint8_t u0 = *(uint8_t *)rrj_at(m, texture, 1);
        uint8_t v0 = *(uint8_t *)rrj_at(m, texture + 4, 1);
        uint8_t u1 = (uint8_t)(u0 + rrj_read32(m, texture + 8) - 1);
        uint8_t v1 = (uint8_t)(v0 + rrj_read32(m, texture + 12) - 1);

        if (width >= 201)
            width = 200;
        if ((rrj_s32(rrj_read32(m, 0x80053250u)) >> 2) >= depth)
        {
            if (width < -200)
                width = -200;
        }
        else
        {
            int32_t quotient = (int32_t)((uint32_t)width << 12) / depth;
            int32_t scale_value = (int32_t)(rrj_read32(m, 0x80053250u) << 12);

            width = (int32_t)(((int64_t)quotient * scale_value) >> 24);
        }
        display = rrj_read32(m, 0x8005B470u);
        packet = rrj_read32(m, display + 268);
        ordering_table = rrj_read32(m, display + 264);
        if (packet + 40 >= rrj_read32(m, 0x8005B4D0u))
            packet = sub_80021C98(m, packet, 40);
        if (rrj_s32(rrj_read32(m, 0x1F800004u)) < 4096)
        {
            int32_t exponent = rrj_s32(rrj_read32(m, 0x1F800000u)) + *(int8_t *)rrj_at(m, (depth >> 12) > 0 ? 0x1F800019u : 0x1F800018u, 1) + ((depth >> 11) > 0);
            int32_t table_index = exponent;
            int32_t base;

            if (table_index < 0)
                table_index = 0;
            if (table_index > 3)
                table_index = 3;
            base = rrj_u16(rrj_at(m, 0x1F800006u + 2 * (uint32_t)table_index, 2)) + rrj_s32(rrj_read32(m, 0x1F800004u));
            bucket = rrj_u16(rrj_at(m, 0x1F80000Eu + 2 * (uint32_t)table_index, 2)) + (rrj_s32((uint32_t)(depth - base)) >> (exponent & 31));
        }
        else
        {
            bucket = rrj_s32((uint32_t)(depth - rrj_s32(rrj_read32(m, 0x1F800004u)))) >> (rrj_read32(m, 0x1F800000u) & 31u);
        }
        if (bucket < 0)
            bucket = 0;
        if (bucket > rrj_s32(rrj_read32(m, 0x1F80001Cu)))
            bucket = rrj_s32(rrj_read32(m, 0x1F80001Cu));
        entry = ordering_table + 4 * (uint32_t)bucket;
        rrj_write32(m, packet + 4, 0x2F000000u);
        rrj_write32(m, packet, rrj_read32(m, entry) | 0x09000000u);
        rrj_write32(m, entry, packet);
        rrj_put16(rrj_at(m, packet + 8, 2), (uint16_t)x);
        rrj_put16(rrj_at(m, packet + 10, 2), (uint16_t)y);
        rrj_put16(rrj_at(m, packet + 16, 2), (uint16_t)(x + width));
        rrj_put16(rrj_at(m, packet + 18, 2), (uint16_t)y);
        rrj_put16(rrj_at(m, packet + 24, 2), (uint16_t)x);
        rrj_put16(rrj_at(m, packet + 26, 2), (uint16_t)(y + height));
        rrj_put16(rrj_at(m, packet + 32, 2), (uint16_t)(x + width));
        rrj_put16(rrj_at(m, packet + 34, 2), (uint16_t)(y + height));
        *(uint8_t *)rrj_at(m, packet + 12, 1) = u0;
        *(uint8_t *)rrj_at(m, packet + 13, 1) = v0;
        *(uint8_t *)rrj_at(m, packet + 20, 1) = u1;
        *(uint8_t *)rrj_at(m, packet + 21, 1) = v0;
        *(uint8_t *)rrj_at(m, packet + 28, 1) = u0;
        *(uint8_t *)rrj_at(m, packet + 29, 1) = v1;
        *(uint8_t *)rrj_at(m, packet + 36, 1) = u1;
        *(uint8_t *)rrj_at(m, packet + 37, 1) = v1;
        rrj_put16(rrj_at(m, packet + 22, 2), rrj_u16(rrj_at(m, texture + 16, 2)) | 0x20u);
        rrj_put16(rrj_at(m, packet + 14, 2), rrj_u16(rrj_at(m, texture + 20, 2)));
        rrj_write32(m, display + 268, packet + 40);
        return display;
    }
    return result;
}

uint32_t sub_80026960(RRJMemory *m, uint32_t object, uint32_t player_index, uint32_t position, uint32_t direction)
{
    uint32_t temporary = 0x1F800300u;
    uint32_t saved_first = rrj_read32(m, temporary);
    uint32_t saved_second = rrj_read32(m, temporary + 4);
    int32_t dot = rrj_s32(sub_8002E698(m, direction, 0x80052364u));
    int16_t plane[3];
    int32_t reciprocal;
    int32_t world[4][3];
    uint32_t player = rrj_read32(m, 0x8005AEC0u + 4 * player_index);
    uint32_t corner;
    MATRIX matrix;
    sint32 screens[4];
    sint32 depths[4];
    sint32 flags[4];
    int32_t average_depth;
    uint32_t result;

    FUNCTION_MARKER(0x80026960, "SLUS_010.53");
    plane[0] = (int16_t)rrj_u16(rrj_at(m, 0x80052364u, 2));
    plane[1] = (int16_t)rrj_u16(rrj_at(m, 0x80052366u, 2));
    plane[2] = (int16_t)rrj_u16(rrj_at(m, 0x80052368u, 2));
    if ((dot < 0 ? -dot : dot) <= 46339)
    {
        int16_t axis[3];
        int16_t cross[3];
        int16_t adjusted[3];
        uint32_t component;

        for (component = 0; component != 3; ++component)
            axis[component] = (int16_t)rrj_u16(rrj_at(m, direction + 2 * component, 2));
        cross[0] = (int16_t)(((int32_t)axis[1] * plane[2] - (int32_t)axis[2] * plane[1]) >> 12);
        cross[1] = (int16_t)(((int32_t)axis[2] * plane[0] - (int32_t)axis[0] * plane[2]) >> 12);
        cross[2] = (int16_t)(((int32_t)axis[0] * plane[1] - (int32_t)axis[1] * plane[0]) >> 12);
        adjusted[0] = (int16_t)(((int32_t)cross[1] * axis[2] - (int32_t)cross[2] * axis[1]) >> 12);
        adjusted[1] = (int16_t)(((int32_t)cross[2] * axis[0] - (int32_t)cross[0] * axis[2]) >> 12);
        adjusted[2] = (int16_t)(((int32_t)cross[0] * axis[1] - (int32_t)cross[1] * axis[0]) >> 12);
        for (component = 0; component != 3; ++component)
            rrj_put16(rrj_at(m, temporary + 2 * component, 2), (uint16_t)adjusted[component]);
        (void)sub_8002E468(m, temporary);
        (void)sub_8002EB78(m, direction, temporary, temporary, 46340, 46340);
        for (component = 0; component != 3; ++component)
            plane[component] = (int16_t)rrj_u16(rrj_at(m, temporary + 2 * component, 2));
        dot = rrj_s32(sub_8002E698(m, direction, temporary));
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
            uint32_t delta = rrj_read32(m, position + 4 * component) - rrj_read32(m, origin + 4 * component);
            sum += (uint32_t)sub_8001FC90(rrj_s32(delta), (int32_t)(int16_t)rrj_u16(rrj_at(m, direction + 2 * component, 2)) * 16);
        }
        ratio = (int32_t)(((int64_t)rrj_s32(sum) * reciprocal) >> 16);
        for (component = 0; component != 3; ++component)
        {
            int32_t offset = (int32_t)sub_8001FC90((int32_t)plane[component] * 16, ratio);
            world[corner][component] = rrj_s32(rrj_read32(m, origin + 4 * component)) + offset;
        }
    }
    memset(&matrix, 0, sizeof(matrix));
    memcpy(matrix.m, rrj_at(m, player + 92, 18), 18);
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);
    for (corner = 0; corner != 4; ++corner)
    {
        uint32_t source_corner = corner < 2 ? corner : 5 - corner;
        SVECTOR vertex;

        vertex.vx = (int16_t)((world[source_corner][0] >> 10) - (int16_t)rrj_u16(rrj_at(m, player + 28, 2)));
        vertex.vy = (int16_t)((world[source_corner][1] >> 10) - (int16_t)rrj_u16(rrj_at(m, player + 32, 2)));
        vertex.vz = (int16_t)((world[source_corner][2] >> 10) - (int16_t)rrj_u16(rrj_at(m, player + 36, 2)));
        vertex.pad = 0;
        depths[corner] = gte_project(&vertex, &screens[corner], &flags[corner]);
    }
    average_depth = (depths[0] + depths[1] + depths[2] + depths[3]) / 4;
    result = (uint32_t)average_depth;
    if ((result & 0x20000000u) == 0)
    {
        uint32_t texture = 0x800D4270u;
        uint32_t display = rrj_read32(m, 0x8005B470u);
        uint32_t packet = rrj_read32(m, display + 268);
        uint32_t ordering_table = rrj_read32(m, display + 264);
        int32_t depth = average_depth * 4;
        int32_t bucket;
        uint32_t entry;
        uint8_t u0 = *(uint8_t *)rrj_at(m, texture, 1);
        uint8_t v0 = *(uint8_t *)rrj_at(m, texture + 4, 1);
        uint8_t u1 = (uint8_t)(u0 + rrj_read32(m, texture + 8) - 1);
        uint8_t v1 = (uint8_t)(v0 + rrj_read32(m, texture + 12) - 1);

        if (packet + 40 >= rrj_read32(m, 0x8005B4D0u))
            packet = sub_80021C98(m, packet, 40);
        if (rrj_s32(rrj_read32(m, 0x1F800004u)) < 4096)
        {
            int32_t exponent = rrj_s32(rrj_read32(m, 0x1F800000u)) + *(int8_t *)rrj_at(m, (depth >> 12) > 0 ? 0x1F800019u : 0x1F800018u, 1) + ((depth >> 11) > 0);
            int32_t table_index = exponent;
            int32_t base;

            if (table_index < 0)
                table_index = 0;
            if (table_index > 3)
                table_index = 3;
            base = rrj_u16(rrj_at(m, 0x1F800006u + 2 * (uint32_t)table_index, 2)) + rrj_s32(rrj_read32(m, 0x1F800004u));
            bucket = rrj_u16(rrj_at(m, 0x1F80000Eu + 2 * (uint32_t)table_index, 2)) + (rrj_s32((uint32_t)(depth - base)) >> (exponent & 31));
        }
        else
        {
            bucket = rrj_s32((uint32_t)(depth - rrj_s32(rrj_read32(m, 0x1F800004u)))) >> (rrj_read32(m, 0x1F800000u) & 31u);
        }
        if (bucket < 0)
            bucket = 0;
        if (bucket > rrj_s32(rrj_read32(m, 0x1F80001Cu)))
            bucket = rrj_s32(rrj_read32(m, 0x1F80001Cu));
        entry = ordering_table + 4 * (uint32_t)bucket;
        rrj_write32(m, packet + 4, 0x2F000000u);
        rrj_write32(m, packet, rrj_read32(m, entry) | 0x09000000u);
        rrj_write32(m, entry, packet);
        rrj_write32(m, packet + 8, (uint32_t)screens[0]);
        rrj_write32(m, packet + 16, (uint32_t)screens[1]);
        rrj_write32(m, packet + 24, (uint32_t)screens[2]);
        rrj_write32(m, packet + 32, (uint32_t)screens[3]);
        *(uint8_t *)rrj_at(m, packet + 12, 1) = u0;
        *(uint8_t *)rrj_at(m, packet + 13, 1) = v0;
        *(uint8_t *)rrj_at(m, packet + 20, 1) = u1;
        *(uint8_t *)rrj_at(m, packet + 21, 1) = v0;
        *(uint8_t *)rrj_at(m, packet + 28, 1) = u0;
        *(uint8_t *)rrj_at(m, packet + 29, 1) = v1;
        *(uint8_t *)rrj_at(m, packet + 36, 1) = u1;
        *(uint8_t *)rrj_at(m, packet + 37, 1) = v1;
        rrj_put16(rrj_at(m, packet + 22, 2), rrj_u16(rrj_at(m, texture + 16, 2)));
        rrj_put16(rrj_at(m, packet + 14, 2), rrj_u16(rrj_at(m, texture + 20, 2)));
        rrj_write32(m, display + 268, packet + 40);
        result = display;
    }
    rrj_write32(m, temporary, saved_first);
    rrj_write32(m, temporary + 4, saved_second);
    return result;
}

uint32_t sub_80025EE0(RRJMemory *m, uint32_t object, uint32_t player_index, uint32_t position, uint32_t direction)
{
    uint32_t specification = rrj_read32(m, object);
    uint32_t vertex_base = rrj_read32(m, 0x8005ACB0u);
    uint32_t scale = rrj_u16(rrj_at(m, specification + 14, 2)) >> 12;
    uint32_t list = rrj_read32(m, specification + 44);
    uint32_t count = rrj_read32(m, list);
    uint32_t display = rrj_read32(m, 0x8005B470u);
    uint32_t packet = rrj_read32(m, display + 268);
    uint32_t refresh = rrj_read32(m, 0x800CCD78u);
    uint32_t result = list;

    FUNCTION_MARKER(0x80025EE0, "SLUS_010.53");
    if (packet + 36 * count >= rrj_read32(m, 0x8005B4D0u))
    {
        packet = sub_80021C98(m, packet, 36 * count);
        rrj_write32(m, display + 268, packet);
    }
    if (rrj_read32(m, 0x800CCDA0u) == refresh)
    {
        uint32_t player = rrj_read32(m, 0x8005AEC0u + 4 * player_index);
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
            input_direction[component] = (int16_t)rrj_u16(rrj_at(m, direction + 2 * component, 2));
            fixed_direction[component] = (int16_t)rrj_u16(rrj_at(m, 0x80052364u + 2 * component, 2));
            delta[component] = (int16_t)((rrj_s32(rrj_read32(m, position + 4 * component)) >> 10) - (int16_t)rrj_u16(rrj_at(m, player + 28 + 4 * component, 2)));
        }
        frontier_rotate_vector(m, player + 92, input_direction, rotated_direction32);
        frontier_rotate_vector(m, player + 92, fixed_direction, rotated_fixed32);
        frontier_rotate_vector(m, player + 92, delta, rotated_delta);
        for (component = 0; component != 3; ++component)
        {
            rotated_direction[component] = (int16_t)rotated_direction32[component];
            rotated_fixed[component] = (int16_t)rotated_fixed32[component];
            dot_sum += rotated_direction[component] * rotated_fixed[component];
        }
        dot_sum >>= 8;
        rrj_write32(m, 0x8005B528u, (uint32_t)dot_sum);
        if ((dot_sum < 0 ? -dot_sum : dot_sum) <= 46339)
        {
            int16_t cross[3];
            int16_t adjusted[3];
            uint32_t temporary = 0x1F800300u;
            uint32_t saved_first = rrj_read32(m, temporary);
            uint32_t saved_second = rrj_read32(m, temporary + 4);

            cross[0] = (int16_t)(((int32_t)rotated_direction[1] * rotated_fixed[2] - (int32_t)rotated_direction[2] * rotated_fixed[1]) >> 12);
            cross[1] = (int16_t)(((int32_t)rotated_direction[2] * rotated_fixed[0] - (int32_t)rotated_direction[0] * rotated_fixed[2]) >> 12);
            cross[2] = (int16_t)(((int32_t)rotated_direction[0] * rotated_fixed[1] - (int32_t)rotated_direction[1] * rotated_fixed[0]) >> 12);
            adjusted[0] = (int16_t)(((int32_t)cross[1] * rotated_direction[2] - (int32_t)cross[2] * rotated_direction[1]) >> 12);
            adjusted[1] = (int16_t)(((int32_t)cross[2] * rotated_direction[0] - (int32_t)cross[0] * rotated_direction[2]) >> 12);
            adjusted[2] = (int16_t)(((int32_t)cross[0] * rotated_direction[1] - (int32_t)cross[1] * rotated_direction[0]) >> 12);
            for (component = 0; component != 3; ++component)
                rrj_put16(rrj_at(m, temporary + 2 * component, 2), (uint16_t)adjusted[component]);
            (void)sub_8002E468(m, temporary);
            for (component = 0; component != 3; ++component)
                rotated_fixed[component] = (int16_t)rrj_u16(rrj_at(m, temporary + 2 * component, 2));
            for (component = 0; component != 3; ++component)
                rotated_fixed[component] = (int16_t)(((int32_t)rotated_direction[component] + rotated_fixed[component]) * 46340 >> 16);
            dot_sum = 0;
            for (component = 0; component != 3; ++component)
                dot_sum += rotated_direction[component] * rotated_fixed[component];
            dot_sum >>= 8;
            rrj_write32(m, temporary, saved_first);
            rrj_write32(m, temporary + 4, saved_second);
            rrj_write32(m, 0x8005B528u, (uint32_t)dot_sum);
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
        rrj_write32(m, 0x8005B528u, (uint32_t)reciprocal);
        {
            uint32_t scalar = 0;

            for (component = 0; component != 3; ++component)
                scalar += (uint32_t)sub_8001FC90(rotated_delta[component], (int32_t)rotated_direction[component] * 16);
            projection = (int32_t)(((int64_t)rrj_s32(scalar) * reciprocal) >> 16);
        }
        for (component = 0; component != 3; ++component)
        {
            int32_t translation = (int32_t)sub_8001FC90(projection, (int32_t)rotated_fixed[component] * 16);
            rrj_write32(m, 0x800D7FBCu + 4 * component, (uint32_t)translation);
        }
        {
            int32_t diagonal[3];

            for (component = 0; component != 3; ++component)
                diagonal[component] = frontier_mul_shift12(rotated_direction[component], rotated_fixed[component]);
            rrj_put16(rrj_at(m, 0x800D7FA8u, 2), (uint16_t)(diagonal[1] + diagonal[2]));
            rrj_put16(rrj_at(m, 0x800D7FAAu, 2), (uint16_t)-frontier_mul_shift12(rotated_fixed[0], rotated_direction[1]));
            rrj_put16(rrj_at(m, 0x800D7FACu, 2), (uint16_t)-frontier_mul_shift12(rotated_fixed[0], rotated_direction[2]));
            rrj_put16(rrj_at(m, 0x800D7FAEu, 2), (uint16_t)-frontier_mul_shift12(rotated_fixed[1], rotated_direction[0]));
            rrj_put16(rrj_at(m, 0x800D7FB0u, 2), (uint16_t)(diagonal[0] + diagonal[2]));
            rrj_put16(rrj_at(m, 0x800D7FB2u, 2), (uint16_t)-frontier_mul_shift12(rotated_fixed[1], rotated_direction[2]));
            rrj_put16(rrj_at(m, 0x800D7FB4u, 2), (uint16_t)-frontier_mul_shift12(rotated_fixed[2], rotated_direction[0]));
            rrj_put16(rrj_at(m, 0x800D7FB6u, 2), (uint16_t)-frontier_mul_shift12(rotated_fixed[2], rotated_direction[1]));
            rrj_put16(rrj_at(m, 0x800D7FB8u, 2), (uint16_t)(diagonal[0] + diagonal[1]));
        }
        refresh = rrj_read32(m, 0x800CCD78u);
    }
    rrj_write32(m, 0x800CCD78u, refresh - 1);
    {
        MATRIX matrix;
        uint32_t indices = list + 4;
        uint32_t reciprocal = rrj_read32(m, 0x8005B528u);
        uint32_t primitive;

        memset(&matrix, 0, sizeof(matrix));
        memcpy(matrix.m, rrj_at(m, 0x800D7FA8u, 18), 18);
        matrix.t[0] = rrj_s32(rrj_read32(m, 0x800D7FBCu));
        matrix.t[1] = rrj_s32(rrj_read32(m, 0x800D7FC0u));
        matrix.t[2] = rrj_s32(rrj_read32(m, 0x800D7FC4u));
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
                uint32_t source = vertex_base + 16 * rrj_u16(rrj_at(m, indices + 2 * vertex, 2));
                SVECTOR input;
                uint32_t component;
                int16_t *values = &input.vx;

                for (component = 0; component != 3; ++component)
                {
                    int32_t value = rrj_s32(rrj_read32(m, source + 4 * component));
                    value >>= scale & 31u;
                    values[component] = (int16_t)(((int64_t)value * rrj_s32(reciprocal)) >> 16);
                }
                input.pad = 0;
                depths[vertex] = gte_project(&input, &screens[vertex], &flags[vertex]);
            }
            average_depth = (depths[0] + depths[1] + depths[2] + depths[3]) / 4;
            if (average_depth < 0)
                continue;
            average_depth *= 4;
            if (rrj_s32(rrj_read32(m, 0x1F800004u)) < 4096)
            {
                int32_t exponent = rrj_s32(rrj_read32(m, 0x1F800000u)) + *(int8_t *)rrj_at(m, (average_depth >> 12) > 0 ? 0x1F800019u : 0x1F800018u, 1) + ((average_depth >> 11) > 0);
                int32_t table_index = exponent;
                int32_t base;

                if (table_index < 0)
                    table_index = 0;
                if (table_index > 3)
                    table_index = 3;
                base = rrj_u16(rrj_at(m, 0x1F800006u + 2 * (uint32_t)table_index, 2)) + rrj_s32(rrj_read32(m, 0x1F800004u));
                bucket = rrj_u16(rrj_at(m, 0x1F80000Eu + 2 * (uint32_t)table_index, 2)) + (rrj_s32((uint32_t)(average_depth - base)) >> (exponent & 31));
            }
            else
            {
                bucket = rrj_s32((uint32_t)(average_depth - rrj_s32(rrj_read32(m, 0x1F800004u)))) >> (rrj_read32(m, 0x1F800000u) & 31u);
            }
            if (bucket < 0)
                bucket = 0;
            if (bucket > rrj_s32(rrj_read32(m, 0x1F80001Cu)))
                bucket = rrj_s32(rrj_read32(m, 0x1F80001Cu));
            ordering_table = rrj_read32(m, display + 264);
            entry = ordering_table + 4 * (uint32_t)bucket;
            packet = rrj_read32(m, display + 268);
            rrj_write32(m, display + 268, packet + 36);
            rrj_write32(m, packet, 0x08000000u);
            rrj_write32(m, packet + 4, 0xE1000740u);
            rrj_write32(m, packet + 8, 0xE6000003u);
            rrj_write32(m, packet + 12, rrj_read32(m, 0x80052348u) | 0x2A000000u);
            rrj_write32(m, packet + 16, (uint32_t)screens[0]);
            rrj_write32(m, packet + 20, (uint32_t)screens[1]);
            rrj_write32(m, packet + 24, (uint32_t)screens[3]);
            rrj_write32(m, packet + 28, (uint32_t)screens[2]);
            rrj_write32(m, packet + 32, 0xE6000000u);
            rrj_write32(m, packet, (rrj_read32(m, packet) & 0xFF000000u) | (rrj_read32(m, entry) & 0x00FFFFFFu));
            result = (rrj_read32(m, entry) & 0xFF000000u) | (packet & 0x00FFFFFFu);
            rrj_write32(m, entry, result);
        }
    }
    return result;
}

uint32_t sub_800251E4(RRJMemory *m, uint32_t object, uint32_t player_index)
{
    uint32_t specification = rrj_read32(m, object);
    uint32_t kind = (rrj_u16(rrj_at(m, specification + 14, 2)) & 0x78u) >> 3;
    uint32_t flags = rrj_read32(m, object + 36);
    uint32_t mapping = rrj_read32(m, specification + 48);
    uint32_t root = rrj_read32(m, object + 4);
    uint32_t screen_base = rrj_read32(m, 0x8005ACB4u);
    uint32_t vertex_base = rrj_read32(m, 0x8005ACB0u);
    uint32_t clip_base = rrj_read32(m, 0x8005ACB8u);
    uint32_t display = rrj_read32(m, 0x8005B470u);
    uint32_t packet = rrj_read32(m, display + 268);
    uint32_t ordering_table = rrj_read32(m, display + 264);
    uint32_t scale = rrj_u16(rrj_at(m, specification + 14, 2)) >> 12;
    uint32_t group_count = rrj_u16(rrj_at(m, specification + 24, 2));
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
    if (kind == 5 && *(int8_t *)rrj_at(m, object + 8, 1) >= 6)
    {
        uint32_t linked = rrj_read32(m, object + 52);
        uint32_t first_index = *(uint8_t *)rrj_at(m, linked + 569, 1);
        uint32_t second_index = *(uint8_t *)rrj_at(m, linked + 570, 1);
        uint32_t table = rrj_read32(m, 0x8005AD4Cu + 12 * first_index + 8);
        uint32_t descriptor = table + 12 * *(uint8_t *)rrj_at(m, linked + second_index + 574, 1);

        if (rrj_u16(rrj_at(m, descriptor + 8, 2)) != 0 && rrj_s32(rrj_read32(m, rrj_read32(m, linked + 540) + 16)) >= 9)
            (void)sub_80027B80(m, object);
    }
    {
        int32_t primitive = *(int8_t *)rrj_at(m, object + 73, 1);

        if (primitive != -1)
        {
            uint32_t primitive_kind = (rrj_read32(m, 0x800D39B0u + 112 * (uint32_t)primitive) >> 6) & 15u;

            if (*(int8_t *)rrj_at(m, object + 8, 1) < 2 || primitive_kind == 5 || primitive_kind == 6)
                (void)sub_80029CA4(m, object);
        }
    }
    if (kind == 2 && ((flags >> 18) & 1u) != 0 && *(uint8_t *)rrj_at(m, object + 8, 1) == 0)
        (void)sub_80028534(m, object);
    {
        uint32_t texture = rrj_read32(m, 0x8005B2E4u) + 12 * (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, object + 74, 2));

        if (((flags >> 10) & 1u) != 0)
        {
            uint32_t context = rrj_read32(m, 0x8005B2F8u);
            uint32_t configuration = 0x80053254u + 176 * rrj_read32(m, context + 48) - 176;
            int32_t selector;
            int32_t offset;
            int32_t row;

            if (((flags >> 11) & 1u) != 0)
            {
                int32_t adjust = kind == 2 ? -1 : 0;
                uint32_t animation = rrj_read32(m, object + 180);

                selector = animation - 9u >= 9u ? 44 + adjust : 45 + 2 * adjust;
            }
            else
            {
                selector = (int32_t)((flags >> 12) & 0x3Fu);
            }
            offset = selector % *(uint8_t *)rrj_at(m, configuration + 116, 1) * *(uint8_t *)rrj_at(m, configuration + 118, 1);
            row = 511 - selector / 3;
            rrj_write32(m, 0x1F8000A0u, (uint32_t)(((row << 6) | (((int16_t)rrj_u16(rrj_at(m, configuration + 112, 2)) + offset + 640) >> 4)) << 16));
            texture_high = rrj_u16(rrj_at(m, texture + 8, 2)) << 16;
            texture_low = *(uint8_t *)rrj_at(m, texture + 2, 1) | ((uint32_t)*(uint8_t *)rrj_at(m, texture + 3, 1) << 8);
            if (kind == 2)
            {
                alternate_texture_high[0] = texture_high;
                alternate_texture_low[0] = texture_low;
                alternate_texture_low[1] = 0;
                alternate_texture_high[1] = rrj_u16(rrj_at(m, rrj_read32(m, 0x8005B348u) + 8, 2)) << 16;
            }
        }
        else
        {
            uint32_t index;
            uint32_t packed = rrj_u16(rrj_at(m, texture + 4, 2));
            uint32_t texture_v = rrj_u16(rrj_at(m, texture + 6, 2));
            uint32_t shift = packed & 3u;

            for (index = 0; index != 40; ++index)
            {
                uint32_t value = (((((packed >> 4) & 0x1FFu) - (index >> shift)) << 6) | ((texture_v + 16 * (index & (packed >> 2) & 3u)) >> 4)) << 16;
                rrj_write32(m, 0x1F8000A0u + 4 * index, value);
            }
            texture_high = rrj_u16(rrj_at(m, texture + 8, 2)) << 16;
            texture_low = *(uint8_t *)rrj_at(m, texture + 2, 1) | ((uint32_t)*(uint8_t *)rrj_at(m, texture + 3, 1) << 8);
        }
    }
    texture_shift = 0;
    if (((flags >> 5) & 3u) != 0)
        texture_shift = ((flags >> 11) & 1u) == 0;
    rrj_write32(m, 0x1F80020Cu, texture_shift);
    use_gouraud = 0;
    if ((*(uint8_t *)rrj_at(m, rrj_read32(m, 0x8005B2F8u) + 4, 1) & 0x10u) == 0 && mapping != 0)
        use_gouraud = ((flags >> 11) & 1u) == 0;
    {
        uint32_t command_index = rrj_u16(rrj_at(m, 0x80052380u, 2));

        if (kind == 3)
            command_index += 8;
        command_index >>= texture_shift;
        command_base = rrj_read32(m, 0x1F800020u + 4 * command_index);
    }
    for (group = 0; group != group_count; ++group)
    {
        uint32_t model = rrj_read32(m, root + 24 * group);
        uint32_t face_list = rrj_read32(m, model + 20);
        uint32_t face_count = rrj_read32(m, face_list);
        uint32_t face;

        if (packet + 52 * face_count >= rrj_read32(m, 0x8005B4D0u))
            packet = sub_80021C98(m, packet, 52 * face_count);
        for (face = 0; face != face_count; ++face)
        {
            uint32_t record = face_list + 20 * face;
            uint32_t face_flags = rrj_read32(m, record + 4);
            uint32_t indices[4];
            uint32_t positions[4];
            uint8_t clips[4];
            uint32_t index;
            uint32_t depth_sum;
            int32_t depth;
            int32_t bucket;
            uint32_t entry;
            int64_t area;
            uint32_t selected_high = texture_high;
            uint32_t selected_low = texture_low;
            uint32_t uv_base;
            uint32_t uv_pair;
            uint32_t texture_word;

            indices[0] = rrj_u16(rrj_at(m, record + 16, 2));
            indices[1] = rrj_u16(rrj_at(m, record + 18, 2));
            indices[2] = rrj_u16(rrj_at(m, record + 20, 2));
            indices[3] = rrj_u16(rrj_at(m, record + 22, 2));
            if (kind == 2)
            {
                uint32_t selection = (face_flags >> 21) & 4u;

                selected_high = alternate_texture_high[selection >> 2];
                selected_low = alternate_texture_low[selection >> 2];
            }
            for (index = 0; index != 4; ++index)
            {
                positions[index] = rrj_read32(m, screen_base + 4 * indices[index]);
                clips[index] = *(uint8_t *)rrj_at(m, clip_base + indices[index], 1);
            }
            depth_sum = rrj_read32(m, vertex_base + 16 * indices[0] + 8) + rrj_read32(m, vertex_base + 16 * indices[1] + 8) + 2 * rrj_read32(m, vertex_base + 16 * indices[2] + 8);
            depth = rrj_s32(depth_sum) >> ((scale + 2) & 31u);
            bucket = frontier_depth_bucket(m, depth);
            entry = ordering_table + 4 * (uint32_t)bucket;
            if (rrj_s32(face_flags) >= 0)
            {
                int32_t x0 = (int16_t)(positions[0] & 0xFFFFu);
                int32_t y0 = (int16_t)(positions[0] >> 16);
                int32_t x1 = (int16_t)(positions[1] & 0xFFFFu);
                int32_t y1 = (int16_t)(positions[1] >> 16);
                int32_t x2 = (int16_t)(positions[2] & 0xFFFFu);
                int32_t y2 = (int16_t)(positions[2] >> 16);

                area = (int64_t)x0 * (y1 - y2) + (int64_t)x1 * (y2 - y0) + (int64_t)x2 * (y0 - y1);
                if (area < 0)
                    continue;
            }
            if ((face_flags & 0x20000000u) != 0)
            {
                if ((clips[0] & clips[1] & clips[2] & clips[3] & 0xFu) != 0)
                    continue;
            }
            else if ((clips[0] & clips[1] & clips[2] & 0xFu) != 0)
            {
                continue;
            }
            texture_word = rrj_read32(m, 0x1F8000A0u + 4 * rrj_u16(rrj_at(m, record + 10, 2)));
            uv_base = rrj_u16(rrj_at(m, record + 8, 2)) + selected_low;
            uv_pair = rrj_read32(m, record + 12);
            if ((face_flags & 0x20000000u) != 0)
            {
                if (!use_gouraud)
                {
                    rrj_write32(m, packet, rrj_read32(m, entry) | 0x09000000u);
                    rrj_write32(m, entry, packet);
                    rrj_write32(m, packet + 4, command_base | 0x2C000000u);
                    rrj_write32(m, packet + 8, positions[0]);
                    rrj_write32(m, packet + 12, uv_base | texture_word);
                    rrj_write32(m, packet + 16, positions[1]);
                    rrj_write32(m, packet + 20, ((uint16_t)face_flags + selected_low) | selected_high);
                    rrj_write32(m, packet + 24, positions[3]);
                    rrj_put16(rrj_at(m, packet + 28, 2), (uint16_t)((uv_pair >> 16) + selected_low));
                    rrj_write32(m, packet + 32, positions[2]);
                    rrj_put16(rrj_at(m, packet + 36, 2), (uint16_t)((uint16_t)uv_pair + selected_low));
                    packet += 40;
                }
                else
                {
                    uint32_t colors[4];

                    for (index = 0; index != 4; ++index)
                    {
                        uint32_t color_index = *(uint8_t *)rrj_at(m, 0x1F800140u + rrj_u16(rrj_at(m, mapping + 2 * indices[index], 2)), 1);
                        colors[index] = rrj_read32(m, 0x1F800020u + 4 * (color_index >> texture_shift));
                    }
                    rrj_write32(m, packet, rrj_read32(m, entry) | 0x0C000000u);
                    rrj_write32(m, entry, packet);
                    rrj_write32(m, packet + 4, colors[0] | 0x3C000000u);
                    rrj_write32(m, packet + 8, positions[0]);
                    rrj_write32(m, packet + 12, uv_base | texture_word);
                    rrj_write32(m, packet + 16, colors[1]);
                    rrj_write32(m, packet + 20, positions[1]);
                    rrj_write32(m, packet + 24, ((uint16_t)face_flags + selected_low) | selected_high);
                    rrj_write32(m, packet + 28, colors[3]);
                    rrj_write32(m, packet + 32, positions[3]);
                    rrj_put16(rrj_at(m, packet + 36, 2), (uint16_t)((uv_pair >> 16) + selected_low));
                    rrj_write32(m, packet + 40, colors[2]);
                    rrj_write32(m, packet + 44, positions[2]);
                    rrj_put16(rrj_at(m, packet + 48, 2), (uint16_t)((uint16_t)uv_pair + selected_low));
                    packet += 52;
                }
            }
            else if (use_gouraud)
            {
                uint32_t colors[3];

                for (index = 0; index != 3; ++index)
                {
                    uint32_t color_index = *(uint8_t *)rrj_at(m, 0x1F800140u + rrj_u16(rrj_at(m, mapping + 2 * indices[index], 2)), 1);
                    colors[index] = rrj_read32(m, 0x1F800020u + 4 * (color_index >> texture_shift));
                }
                rrj_write32(m, packet, rrj_read32(m, entry) | 0x09000000u);
                rrj_write32(m, entry, packet);
                rrj_write32(m, packet + 4, colors[0] | 0x34000000u);
                rrj_write32(m, packet + 8, positions[0]);
                rrj_write32(m, packet + 12, uv_base | texture_word);
                rrj_write32(m, packet + 16, colors[1]);
                rrj_write32(m, packet + 20, positions[1]);
                rrj_write32(m, packet + 24, ((uint16_t)face_flags + selected_low) | selected_high);
                rrj_write32(m, packet + 28, colors[2]);
                rrj_write32(m, packet + 32, positions[2]);
                rrj_put16(rrj_at(m, packet + 36, 2), (uint16_t)((uint16_t)uv_pair + selected_low));
                packet += 40;
            }
            else
            {
                rrj_write32(m, packet, rrj_read32(m, entry) | 0x07000000u);
                rrj_write32(m, entry, packet);
                rrj_write32(m, packet + 4, command_base | 0x24000000u);
                rrj_write32(m, packet + 8, positions[0]);
                rrj_write32(m, packet + 12, uv_base | texture_word);
                rrj_write32(m, packet + 16, positions[1]);
                rrj_write32(m, packet + 20, ((uint16_t)face_flags + selected_low) | selected_high);
                rrj_write32(m, packet + 24, positions[2]);
                rrj_put16(rrj_at(m, packet + 28, 2), (uint16_t)((uint16_t)uv_pair + selected_low));
                packet += 32;
            }
        }
    }
    rrj_write32(m, display + 268, packet);
    result = (flags >> 7) & 1u;
    if (result != 0)
    {
        uint32_t linked = rrj_read32(m, object + 52);
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
            if (kind == 3 || ((kind == 1 || kind == 4) && (rrj_read32(m, object + 552) & 0x40000000u) == 0))
                position = object + 184;
        }
        if (rrj_read32(m, rrj_read32(m, 0x8005B2F8u) + 48) == 1)
            return sub_80025EE0(m, object, player_index, position, direction);
        return sub_80026960(m, object, player_index, position, direction);
    }
    return result;
}

uint32_t sub_80068468(RRJMemory *m, uint32_t object, uint32_t player_index, RRJRaceLeafCall call)
{
    uint32_t specification = rrj_read32(m, object);
    uint32_t type = (rrj_u16(rrj_at(m, specification + 14, 2)) & 0x78u) >> 3;
    uint32_t scratch = 0x1F800300u;
    uint32_t root = rrj_read32(m, object + 4);
    uint32_t current;
    uint32_t index;

    FUNCTION_MARKER(0x80068468, "RASHCDG.BIN");
    rrj_write32(m, 0x1F8002BCu, scratch);
    frontier_copy_words(m, scratch, object + 136, 32);
    scratch += 32;
    rrj_write32(m, 0x1F8002BCu, scratch);
    for (index = 0; index != 3; ++index)
        rrj_write32(m, scratch + 20 + 4 * index, rrj_read32(m, object + 80 + 4 * index));

    if (*(int8_t *)rrj_at(m, object + 72, 1) == 3)
    {
        for (index = 0; index != 3; ++index)
        {
            int16_t input[3];
            int32_t transformed[3];
            uint32_t row;

            for (row = 0; row != 3; ++row)
                input[row] = (int16_t)rrj_u16(rrj_at(m, root + 4 + 6 * row + 2 * index, 2));
            frontier_rotate_vector(m, object + 104, input, transformed);
            for (row = 0; row != 3; ++row)
                rrj_put16(rrj_at(m, object + 104 + 6 * row + 2 * index, 2), (uint16_t)transformed[row]);
        }
        frontier_copy_words(m, scratch, object + 104, 20);
        if ((type == 1 || type == 4) && rrj_read32(m, object + 52) == 0 && rrj_s32(rrj_read32(m, object + 44 + 4 * player_index)) < 1280)
        {
            uint32_t output = 0x1F800240u;

            (void)sub_800669E8(m, object, output);
            for (index = 0; index != 3; ++index)
                rrj_write32(m, object + 244 + 4 * index, (rrj_read32(m, output + 4 * index) << 10) - rrj_read32(m, object + 184 + 4 * index));
            rrj_put16(rrj_at(m, object + 262, 2), (uint16_t)-(int16_t)rrj_u16(rrj_at(m, object + 104, 2)));
            rrj_put16(rrj_at(m, object + 264, 2), (uint16_t)-(int16_t)rrj_u16(rrj_at(m, object + 110, 2)));
            rrj_put16(rrj_at(m, object + 266, 2), (uint16_t)-(int16_t)rrj_u16(rrj_at(m, object + 116, 2)));
            rrj_put16(rrj_at(m, object + 268, 2), (uint16_t)-(int16_t)rrj_u16(rrj_at(m, object + 106, 2)));
            rrj_put16(rrj_at(m, object + 270, 2), (uint16_t)-(int16_t)rrj_u16(rrj_at(m, object + 112, 2)));
            rrj_put16(rrj_at(m, object + 272, 2), (uint16_t)-(int16_t)rrj_u16(rrj_at(m, object + 118, 2)));
            rrj_put16(rrj_at(m, object + 256, 2), rrj_u16(rrj_at(m, object + 108, 2)));
            rrj_put16(rrj_at(m, object + 258, 2), rrj_u16(rrj_at(m, object + 114, 2)));
            rrj_put16(rrj_at(m, object + 260, 2), rrj_u16(rrj_at(m, object + 120, 2)));
            *(uint8_t *)rrj_at(m, object + 547, 1) = 1;
        }
    }
    else
    {
        frontier_copy_words(m, scratch, root + 4, 20);
    }

    for (index = 0; index != 3; ++index)
    {
        int16_t input[3];
        int32_t transformed[3];
        uint32_t row;

        for (row = 0; row != 3; ++row)
            input[row] = (int16_t)rrj_u16(rrj_at(m, scratch + 6 * row + 2 * index, 2));
        frontier_rotate_vector(m, scratch - 32, input, transformed);
        for (row = 0; row != 3; ++row)
            rrj_put16(rrj_at(m, scratch + 6 * row + 2 * index, 2), (uint16_t)transformed[row]);
    }
    if (rrj_read32(m, object + 76) != 0x10000u)
    {
        int32_t x_scale = (int16_t)(rrj_read32(m, object + 76) >> 4);
        uint32_t row;

        for (row = 0; row != 3; ++row)
        {
            int32_t value = (int16_t)rrj_u16(rrj_at(m, scratch + 6 * row, 2));

            rrj_put16(rrj_at(m, scratch + 6 * row, 2), (uint16_t)frontier_mul_shift12(value, x_scale));
        }
    }
    current = rrj_read32(m, root);
    (void)sub_800220A4(m, scratch, rrj_u16(rrj_at(m, current + 14, 2)), rrj_read32(m, specification + 36) + 8 * rrj_read32(m, current + 16) + 4, rrj_read32(m, 0x8005ACB0u) + 16 * rrj_read32(m, current + 16), rrj_read32(m, 0x8005ACB4u) + 4 * rrj_read32(m, current + 16), rrj_read32(m, 0x8005ACB8u) + rrj_read32(m, current + 16));

    if ((*(uint8_t *)rrj_at(m, rrj_read32(m, 0x8005B2F8u) + 4, 1) & 0x10u) == 0 && rrj_read32(m, specification + 40) != 0)
    {
        uint32_t camera = rrj_read32(m, 0x8005AEC0u + 4 * player_index) + 92;
        uint32_t transposed = 0x1F800240u;
        uint32_t normals = rrj_read32(m, specification + 40);
        uint32_t count = rrj_read32(m, normals);
        int16_t direction[3];
        int32_t light[3];

        (void)sub_80070F9C(m, camera, transposed);
        for (index = 0; index != 3; ++index)
            direction[index] = (int16_t)rrj_u16(rrj_at(m, 0x80052364u + 2 * index, 2));
        frontier_rotate_vector(m, transposed, direction, light);
        for (index = 0; index < count; ++index)
        {
            uint32_t normal = normals + 4 + 8 * index;
            int64_t shade = 0;
            uint32_t component;
            int32_t value;

            for (component = 0; component != 3; ++component)
                shade += (int16_t)rrj_u16(rrj_at(m, normal + 2 * component, 2)) * (int16_t)light[component];
            value = (int32_t)(shade >> 8);
            if (value < 0)
                value = 0;
            value >>= 11;
            if (value > 31)
                value = 31;
            *(uint8_t *)rrj_at(m, 0x1F800140u + index, 1) = (uint8_t)value;
        }
    }

    switch (type)
    {
        case 1:
        case 4:
            (void)sub_8006745C(m, object);
            break;
        case 2:
        {
            uint32_t limit = rrj_read32(m, object + 596);
            uint32_t flags = rrj_read32(m, object + 36);

            if (limit == 0 || ((rrj_read32(m, object + 560) & 0x08000000u) != 0 && rrj_s32(rrj_read32(m, object + 600)) >= rrj_s32(limit << 1)))
                flags &= ~0x00040000u;
            else
                flags |= 0x00040000u;
            rrj_write32(m, object + 36, flags);
            (void)sub_80066EC4(m, object);
            break;
        }
        case 3:
        case 5:
        case 6:
            (void)sub_80066B28(m, object);
            break;
        default:
            break;
    }
    rrj_write32(m, 0x1F8002BCu, rrj_read32(m, 0x1F8002BCu) - 32);
    (void)sub_800251E4(m, object, player_index);
    for (index = 0; index != 2; ++index)
    {
        uint32_t child = rrj_read32(m, object + 56 + 8 * index);

        if (child != 0 && ((type != 1 && type != 4) || rrj_u16(rrj_at(m, specification + 24, 2)) == 17))
        {
            (void)sub_80067AC4(m, child, player_index, call);
            (void)sub_80068468(m, child, player_index, call);
        }
    }
    return 0;
}

uint32_t sub_8006780C(RRJMemory *m, uint32_t object, uint32_t player_index, RRJRaceLeafCall call)
{
    uint32_t render_slot = sub_80066A84(m, object);
    uint16_t saved_x = 0;
    uint16_t saved_y = 0;

    FUNCTION_MARKER(0x8006780C, "RASHCDG.BIN");
    if (render_slot != 0)
    {
        uint32_t configuration = rrj_read32(m, object + 556);
        int32_t timer = rrj_s32(rrj_read32(m, object + 612));
        int32_t angle = (int16_t)rrj_u16(rrj_at(m, object + 846, 2));
        uint32_t table_angle = (uint32_t)angle & 0xFFFu;
        int32_t selection = (int32_t)((uint32_t)angle << 16) > 0 ? -1 : 0;
        int32_t lateral_x;
        int32_t lateral_y;
        int32_t cosine;
        int32_t sine;
        int32_t x_offset;
        int32_t y_offset;
        uint32_t scale = rrj_u16(rrj_at(m, rrj_read32(m, object) + 14, 2)) >> 12;

        saved_x = rrj_u16(rrj_at(m, render_slot + 4, 2));
        saved_y = rrj_u16(rrj_at(m, render_slot + 2, 2));
        if ((rrj_read32(m, object + 568) & 0x600u) != 0)
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
                limit = rrj_u16(rrj_at(m, configuration + 436, 2));
                if (angle <= -(int32_t)limit)
                {
                    timer = 0;
                    goto timer_ready;
                }
                magnitude = (uint32_t)-angle;
            }
            else
            {
                limit = rrj_u16(rrj_at(m, configuration + 438, 2));
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
        rrj_write32(m, object + 612, (uint32_t)timer);

        lateral_x = (selection & (rrj_s32(rrj_read32(m, configuration + 256)) + rrj_s32(rrj_read32(m, configuration + 248)))) - rrj_s32(rrj_read32(m, configuration + 248));
        lateral_y = (selection & (rrj_s32(rrj_read32(m, configuration + 260)) + rrj_s32(rrj_read32(m, configuration + 252)))) - rrj_s32(rrj_read32(m, configuration + 252));
        cosine = 0x10000 - ((int32_t)(int16_t)rrj_u16(rrj_at(m, 0x8005624Eu + 4 * table_angle, 2)) << 4);
        sine = (int32_t)(int16_t)rrj_u16(rrj_at(m, 0x8005624Cu + 4 * table_angle, 2)) << 4;
        x_offset = ((int32_t)sub_8001FC90(lateral_x, cosine) - (int32_t)sub_8001FC90(lateral_y, sine)) >> 10;
        y_offset = ((int32_t)sub_8001FC90(lateral_x, sine) + (int32_t)sub_8001FC90(lateral_y, cosine) + timer) >> 10;
        rrj_put16(rrj_at(m, render_slot + 4, 2), (uint16_t)(saved_x - ((uint32_t)x_offset << scale)));
        rrj_put16(rrj_at(m, render_slot + 2, 2), (uint16_t)(saved_y - ((uint32_t)y_offset << scale)));
    }
    (void)sub_80068468(m, object, player_index, call);
    if (render_slot != 0)
    {
        rrj_put16(rrj_at(m, render_slot + 4, 2), saved_x);
        rrj_put16(rrj_at(m, render_slot + 2, 2), saved_y);
    }
    return 1;
}

uint32_t sub_80067690(RRJMemory *m, uint32_t owner, uint32_t player_index, RRJRaceLeafCall call)
{
    uint32_t object = rrj_read32(m, 0x8005B280u);
    uint32_t result = 0x80060000u;

    FUNCTION_MARKER(0x80067690, "RASHCDG.BIN");
    while (object != 0)
    {
        if (rrj_read32(m, object + 176) == owner)
        {
            uint32_t group = (*(uint8_t *)rrj_at(m, object + 9, 1) & 0x30u) >> 4;

            rrj_write32(m, 0x800CCD78u, group);
            rrj_write32(m, 0x800CCDA0u, group);
            result = sub_80068468(m, object, player_index, call);
        }
        object = rrj_read32(m, object + 168);
    }
    return result;
}

uint32_t sub_80028E8C(RRJMemory *m, uint32_t position, uint32_t output, uint32_t player_index)
{
    uint32_t player = rrj_read32(m, 0x8005AEC0u + 4 * player_index);
    int16_t input[3];
    MATRIX identity;
    uint32_t row;

    FUNCTION_MARKER(0x80028E8C, "SLUS_010.53");
    for (row = 0; row != 3; ++row)
        input[row] = (int16_t)(rrj_u16(rrj_at(m, position + 4 * row, 2)) - rrj_u16(rrj_at(m, player + 28 + 4 * row, 2)));
    for (row = 0; row != 3; ++row)
    {
        int64_t value = 0;
        uint32_t column;

        for (column = 0; column != 3; ++column)
            value += (int16_t)rrj_u16(rrj_at(m, player + 92 + 6 * row + 2 * column, 2)) * input[column];
        rrj_put16(rrj_at(m, output + 2 * row, 2), (uint16_t)(value >> 12));
    }
    memset(&identity, 0, sizeof(identity));
    identity.m[0][0] = 4096;
    identity.m[1][1] = 4096;
    identity.m[2][2] = 4096;
    SetRotMatrix(&identity);
    SetTransMatrix(&identity);
    return output;
}

uint32_t sub_80029174(RRJMemory *m, uint32_t vertices, uint32_t position, uint32_t center, uint32_t player_index)
{
    uint32_t transformed = 0x1F800240u;
    int16_t depth;

    FUNCTION_MARKER(0x80029174, "SLUS_010.53");
    (void)sub_80028E8C(m, position, transformed, player_index);
    depth = (int16_t)rrj_u16(rrj_at(m, transformed + 4, 2));
    if (depth < 64)
        depth = 64;
    rrj_put16(rrj_at(m, vertices, 2), rrj_u16(rrj_at(m, center, 2)) - 13);
    rrj_put16(rrj_at(m, vertices + 2, 2), rrj_u16(rrj_at(m, center + 2, 2)));
    rrj_put16(rrj_at(m, vertices + 4, 2), rrj_u16(rrj_at(m, center + 4, 2)));
    rrj_put16(rrj_at(m, vertices + 8, 2), rrj_u16(rrj_at(m, center, 2)) + 13);
    rrj_put16(rrj_at(m, vertices + 10, 2), rrj_u16(rrj_at(m, center + 2, 2)));
    rrj_put16(rrj_at(m, vertices + 12, 2), rrj_u16(rrj_at(m, center + 4, 2)));
    rrj_put16(rrj_at(m, vertices + 16, 2), rrj_u16(rrj_at(m, transformed, 2)) - 13);
    rrj_put16(rrj_at(m, vertices + 18, 2), rrj_u16(rrj_at(m, transformed + 2, 2)));
    rrj_put16(rrj_at(m, vertices + 20, 2), (uint16_t)depth);
    rrj_put16(rrj_at(m, vertices + 24, 2), rrj_u16(rrj_at(m, transformed, 2)) + 13);
    rrj_put16(rrj_at(m, vertices + 26, 2), rrj_u16(rrj_at(m, transformed + 2, 2)));
    rrj_put16(rrj_at(m, vertices + 28, 2), (uint16_t)depth);
    return 0;
}

static int32_t frontier_project_effect_quad(RRJMemory *m, uint32_t vertices, int32_t screens[4])
{
    int32_t total = 0;
    uint32_t index;

    for (index = 0; index != 4; ++index)
    {
        uint32_t source = vertices + 8 * index;
        SVECTOR vertex;
        sint32 flags;

        vertex.vx = (int16_t)rrj_u16(rrj_at(m, source, 2));
        vertex.vy = (int16_t)rrj_u16(rrj_at(m, source + 2, 2));
        vertex.vz = (int16_t)rrj_u16(rrj_at(m, source + 4, 2));
        vertex.pad = 0;
        total += gte_project(&vertex, &screens[index], &flags);
    }
    return (total / 4) * 4;
}

uint32_t sub_8002926C(RRJMemory *m, uint32_t vertices, uint32_t flags, uint32_t texture_index, uint32_t clut_index, uint32_t command)
{
    uint32_t texture = 0x800D4270u + 28 * texture_index;
    uint32_t display = rrj_read32(m, 0x8005B470u);
    uint32_t packet = rrj_read32(m, display + 268);
    uint32_t ordering_table = rrj_read32(m, display + 264);
    int32_t screens[4];
    int32_t depth;
    int32_t bucket;
    uint32_t entry;
    uint8_t u0;
    uint8_t v0;
    uint8_t u1;
    uint8_t v1;

    FUNCTION_MARKER(0x8002926C, "SLUS_010.53");
    if (packet + 40 >= rrj_read32(m, 0x8005B4D0u))
        packet = sub_80021C98(m, packet, 40);
    depth = frontier_project_effect_quad(m, vertices, screens);
    bucket = frontier_depth_bucket(m, depth);
    if (bucket < 0)
        bucket = 0;
    if (bucket >= rrj_s32(rrj_read32(m, 0x8005ADFCu)))
        bucket = rrj_s32(rrj_read32(m, 0x8005ADFCu)) - 1;
    entry = ordering_table + 4 * (uint32_t)bucket;
    rrj_write32(m, packet, rrj_read32(m, entry) | 0x09000000u);
    rrj_write32(m, entry, packet);
    rrj_write32(m, packet + 4, (flags & 1u) != 0 ? command | 0x2E000000u : 0x2F000000u);
    rrj_write32(m, packet + 8, (uint32_t)screens[0]);
    rrj_write32(m, packet + 16, (uint32_t)screens[1]);
    rrj_write32(m, packet + 24, (uint32_t)screens[2]);
    rrj_write32(m, packet + 32, (uint32_t)screens[3]);
    u0 = *(uint8_t *)rrj_at(m, texture, 1);
    v0 = *(uint8_t *)rrj_at(m, texture + 4, 1);
    u1 = (uint8_t)(u0 + *(uint8_t *)rrj_at(m, texture + 8, 1) - 1);
    v1 = (uint8_t)(v0 + *(uint8_t *)rrj_at(m, texture + 12, 1) - 1);
    *(uint8_t *)rrj_at(m, packet + 12, 1) = u0;
    *(uint8_t *)rrj_at(m, packet + 13, 1) = v1;
    *(uint8_t *)rrj_at(m, packet + 20, 1) = u0;
    *(uint8_t *)rrj_at(m, packet + 21, 1) = v0;
    *(uint8_t *)rrj_at(m, packet + 28, 1) = u1;
    *(uint8_t *)rrj_at(m, packet + 29, 1) = v1;
    *(uint8_t *)rrj_at(m, packet + 36, 1) = u1;
    *(uint8_t *)rrj_at(m, packet + 37, 1) = v0;
    rrj_put16(rrj_at(m, packet + 22, 2), (flags & 2u) != 0 ? (rrj_u16(rrj_at(m, texture + 16, 2)) & 0xFF9Fu) | 0x20u : rrj_u16(rrj_at(m, texture + 16, 2)) | 0x60u);
    rrj_put16(rrj_at(m, packet + 14, 2), rrj_u16(rrj_at(m, texture + 20 + 2 * clut_index, 2)));
    rrj_write32(m, display + 268, packet + 40);
    return display;
}

uint32_t sub_800295AC(RRJMemory *m, uint32_t object, uint32_t vertices, uint32_t flags, uint32_t texture_index, uint32_t uv_variant)
{
    uint32_t texture = 0x800D4270u + 28 * texture_index;
    uint32_t offsets = 0x80053858u + 8 * uv_variant;
    uint32_t display = rrj_read32(m, 0x8005B470u);
    uint32_t packet = rrj_read32(m, display + 268);
    uint32_t ordering_table = rrj_read32(m, display + 264);
    int32_t screens[4];
    int32_t bucket;
    uint32_t entry;
    uint32_t index;

    FUNCTION_MARKER(0x800295AC, "SLUS_010.53");
    if (packet + 40 >= rrj_read32(m, 0x8005B4D0u))
        packet = sub_80021C98(m, packet, 40);
    bucket = frontier_depth_bucket(m, frontier_project_effect_quad(m, vertices, screens));
    if (bucket < 0)
        bucket = 0;
    if (bucket >= rrj_s32(rrj_read32(m, 0x8005ADFCu)))
        bucket = rrj_s32(rrj_read32(m, 0x8005ADFCu)) - 1;
    entry = ordering_table + 4 * (uint32_t)bucket;
    rrj_write32(m, packet, rrj_read32(m, entry) | 0x09000000u);
    rrj_write32(m, entry, packet);
    rrj_write32(m, packet + 4, (flags & 1u) != 0 ? 0x2E000000u : 0x2F000000u);
    for (index = 0; index != 4; ++index)
    {
        rrj_write32(m, packet + 8 + 8 * index, (uint32_t)screens[index]);
        *(uint8_t *)rrj_at(m, packet + 12 + 8 * index, 1) = (uint8_t)(*(uint8_t *)rrj_at(m, texture, 1) + *(uint8_t *)rrj_at(m, offsets + 2 * index, 1));
        *(uint8_t *)rrj_at(m, packet + 13 + 8 * index, 1) = (uint8_t)(*(uint8_t *)rrj_at(m, texture + 4, 1) + *(uint8_t *)rrj_at(m, offsets + 2 * index + 1, 1));
    }
    rrj_put16(rrj_at(m, packet + 22, 2), (flags & 2u) != 0 ? (rrj_u16(rrj_at(m, texture + 16, 2)) & 0xFF9Fu) | 0x20u : rrj_u16(rrj_at(m, texture + 16, 2)) | 0x60u);
    rrj_put16(rrj_at(m, packet + 14, 2), rrj_u16(rrj_at(m, texture + 20, 2)));
    rrj_write32(m, display + 268, packet + 40);
    return display;
}

uint32_t sub_8002AB14(RRJMemory *m, uint32_t object, uint32_t record)
{
    uint32_t mode = (rrj_read32(m, record) >> 10) & 0xFu;
    int32_t depth = rrj_s32(rrj_read32(m, record + 92));
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
    if (*(int8_t *)rrj_at(m, object + 8, 1) < 2)
    {
        uint32_t diameter = 2 * rrj_u16(rrj_at(m, record + 88, 2));

        rrj_put16(rrj_at(m, record + 62, 2), (uint16_t)diameter);
        if ((rrj_s32(rrj_read32(m, 0x80053250u)) >> 2) < depth)
        {
            int32_t quotient = (int32_t)((rrj_read32(m, record + 88) << 12) / (uint32_t)depth);

            rrj_write32(m, record + 88, (uint32_t)(((int64_t)quotient * ((int64_t)rrj_s32(rrj_read32(m, 0x80053250u)) << 12)) >> 24));
        }
        radius = rrj_s32(rrj_read32(m, record + 88));
        diameter = rrj_u16(rrj_at(m, record + 62, 2));
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
        rrj_write32(m, record + 88, (uint32_t)radius);
    }
    if (*(int8_t *)rrj_at(m, record + 97, 1) != 0)
        rrj_write32(m, record + 88, rrj_read32(m, record + 88) << 1);
    rrj_put16(rrj_at(m, record + 84, 2), 0);
    (void)sub_80029048(m, quad, record + 80, (uint32_t)(8 * ((int16_t)rrj_u16(rrj_at(m, record + 80, 2)) + (int16_t)rrj_u16(rrj_at(m, record + 82, 2)))), rrj_s32(rrj_read32(m, record + 88)));

    texture = 0x800D4270u + 28 * (uint32_t)(int32_t)*(int8_t *)rrj_at(m, record + 96, 1);
    display = rrj_read32(m, 0x8005B470u);
    packet = rrj_read32(m, display + 268);
    ordering_table = rrj_read32(m, display + 264);
    if (packet + 40 >= rrj_read32(m, 0x8005B4D0u))
        packet = sub_80021C98(m, packet, 40);
    bucket = frontier_depth_bucket(m, depth);
    if (bucket >= 3)
        bucket -= 3;
    entry = ordering_table + 4 * (uint32_t)bucket;
    rrj_write32(m, packet + 4, rrj_read32(m, 0x800536C4u + 4 * (uint32_t)(int32_t)*(int8_t *)rrj_at(m, record + 97, 1)) | 0x2E000000u);
    rrj_write32(m, packet, rrj_read32(m, entry) | 0x09000000u);
    rrj_write32(m, entry, packet);
    rrj_write32(m, packet + 8, rrj_read32(m, quad));
    rrj_write32(m, packet + 16, rrj_read32(m, quad + 8));
    rrj_write32(m, packet + 24, rrj_read32(m, quad + 16));
    rrj_write32(m, packet + 32, rrj_read32(m, quad + 24));
    u0 = *(uint8_t *)rrj_at(m, texture, 1);
    v0 = *(uint8_t *)rrj_at(m, texture + 4, 1);
    u1 = (uint8_t)(u0 + *(uint8_t *)rrj_at(m, texture + 8, 1) - 1);
    v1 = (uint8_t)(v0 + *(uint8_t *)rrj_at(m, texture + 12, 1) - 1);
    *(uint8_t *)rrj_at(m, packet + 12, 1) = u0;
    *(uint8_t *)rrj_at(m, packet + 13, 1) = v0;
    *(uint8_t *)rrj_at(m, packet + 20, 1) = u1;
    *(uint8_t *)rrj_at(m, packet + 21, 1) = v0;
    *(uint8_t *)rrj_at(m, packet + 28, 1) = u0;
    *(uint8_t *)rrj_at(m, packet + 29, 1) = v1;
    *(uint8_t *)rrj_at(m, packet + 36, 1) = u1;
    *(uint8_t *)rrj_at(m, packet + 37, 1) = v1;
    rrj_put16(rrj_at(m, packet + 22, 2), rrj_u16(rrj_at(m, texture + 16, 2)) | 0x20u);
    rrj_put16(rrj_at(m, packet + 14, 2), rrj_u16(rrj_at(m, texture + 20, 2)));
    rrj_write32(m, display + 268, packet + 40);
    return display;
}

uint32_t sub_8002A2C8(RRJMemory *m, uint32_t object, uint32_t record, uint32_t age, uint32_t player_index, uint32_t output_flags)
{
    uint32_t type = (rrj_read32(m, record) >> 6) & 0xFu;
    uint32_t center = 0x1F800240u;
    uint32_t position = 0x1F800260u;
    uint32_t quad = 0x1F800200u;
    int32_t variant;
    uint32_t component;
    uint32_t result = type;

    FUNCTION_MARKER(0x8002A2C8, "SLUS_010.53");
    variant = (int16_t)rrj_u16(rrj_at(m, 0x800538B0u + 2 * (uint32_t)(int32_t)*(int8_t *)rrj_at(m, object + 534, 1), 2));
    switch (type)
    {
        case 1:
            (void)sub_80028E8C(m, record + 4, center, player_index);
            for (component = 0; component != 3; ++component)
                rrj_write32(m, position + 4 * component, rrj_read32(m, record + 20 + 4 * component) - rrj_read32(m, record + 36 + 4 * component));
            if (sub_80029174(m, quad, position, center, player_index) == 0)
                result = sub_8002926C(m, quad, 2, 3, 0, 25700);
            else
                result = 25700;
            break;
        case 2:
        {
            uint32_t required = ((rrj_read32(m, record) >> 14) & 0xFFu) == 2 ? 4u : 8u;

            if ((rrj_read32(m, object + 564) & required) != 0 && (rrj_read32(m, output_flags) & 4u) == 0)
            {
                uint32_t context = rrj_read32(m, 0x8005B2F8u);
                uint32_t duration = rrj_read32(m, record + 52);
                uint32_t scale = duration != 0 ? ((rrj_read32(m, context + 16) - rrj_read32(m, record + 48)) << 17) / duration : 0;

                (void)sub_8002EE50(m, scale, object + 450, record + 36);
                for (component = 0; component != 3; ++component)
                    rrj_write32(m, position + 4 * component, rrj_read32(m, record + 4 + 4 * component) - (uint32_t)(rrj_s32(rrj_read32(m, record + 36 + 4 * component)) >> 10));
                rrj_write32(m, output_flags, rrj_read32(m, output_flags) | 4u);
                (void)sub_80028E8C(m, record + 4, center, player_index);
                if (sub_80029174(m, quad, position, center, player_index) == 0)
                    (void)sub_8002926C(m, quad, 0, 1, variant == 2, 25650);
            }
            result = age < 76;
            if (((rrj_read32(m, record) >> 14) & 0xFFu) == 0)
            {
                (void)sub_80029048(m, quad, record + 68, (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, record + 56, 2)), rrj_u16(rrj_at(m, record + 62, 2)) + (int32_t)age);
                result = sub_8002926C(m, quad, 0, 2, (uint32_t)(variant + (age >= 76)), 25650);
            }
            break;
        }
        case 3:
            (void)sub_80028E8C(m, record + 4, center, player_index);
            for (component = 0; component != 3; ++component)
                rrj_write32(m, position + 4 * component, rrj_read32(m, record + 20 + 4 * component) - rrj_read32(m, record + 36 + 4 * component));
            result = sub_80029174(m, quad, position, center, player_index);
            if (result == 0)
                result = sub_8002926C(m, quad, 0, 2, (uint32_t)variant, 0);
            break;
        case 4:
            (void)sub_80028E8C(m, record + 4, center, player_index);
            for (component = 0; component != 3; ++component)
                rrj_write32(m, position + 4 * component, rrj_read32(m, record + 4 + 4 * component) - rrj_read32(m, record + 36 + 4 * component));
            result = sub_80029174(m, quad, position, center, player_index);
            if (result == 0)
                result = sub_800295AC(m, record, quad, 2, 8, *(uint8_t *)rrj_at(m, record + 104 + *(uint8_t *)rrj_at(m, record + 108, 1), 1));
            break;
        case 6:
            (void)sub_80029048(m, quad, record + 68, (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, record + 56, 2)), rrj_u16(rrj_at(m, record + 62, 2)) + 25);
            result = sub_8002926C(m, quad, 0, 9, age & 1u, 25650);
            break;
        case 7:
            if ((*(uint8_t *)rrj_at(m, record + 60, 1) & 1u) != 0)
                variant += rrj_read32(m, object + 480) <= 0x7FFFFu;
            else
                variant = (rrj_read32(m, record + 52) >> 1) < age;
            if ((int16_t)rrj_u16(rrj_at(m, record + 72, 2)) < 0)
                rrj_write32(m, record, (rrj_read32(m, record) & 0xFFFFC3FFu) | 0x1000u);
            (void)sub_80029048(m, quad, record + 68, (uint32_t)(int32_t)(int16_t)rrj_u16(rrj_at(m, record + 56, 2)), rrj_u16(rrj_at(m, record + 62, 2)) + (int32_t)(age >> 1));
            result = sub_8002926C(m, quad, 0, 2, (uint32_t)variant, 0);
            break;
        default:
            break;
    }
    return result;
}

uint32_t sub_80028E74(RRJMemory *m, uint32_t record)
{
    uint32_t result = rrj_u16(rrj_at(m, record + 56, 2)) + rrj_u16(rrj_at(m, record + 58, 2));

    FUNCTION_MARKER(0x80028E74, "SLUS_010.53");
    rrj_put16(rrj_at(m, record + 56, 2), (uint16_t)result);
    return result;
}

uint32_t sub_800289E8(RRJMemory *m, uint32_t object, uint32_t scale, uint32_t output)
{
    int32_t scaled[3];
    uint32_t component;
    int32_t result = 0;

    FUNCTION_MARKER(0x800289E8, "SLUS_010.53");
    scaled[0] = (int32_t)(((int64_t)rrj_s32(rrj_read32(m, object + 304)) * rrj_s32(rrj_read32(m, scale))) >> 16);
    scaled[1] = (int32_t)(((int64_t)rrj_s32(rrj_read32(m, object + 312)) * rrj_s32(rrj_read32(m, scale + 4))) >> 16);
    scaled[2] = (int32_t)(((int64_t)rrj_s32(rrj_read32(m, object + 308)) * rrj_s32(rrj_read32(m, scale + 8))) >> 16);
    for (component = 0; component != 3; ++component)
    {
        uint32_t row_offset = 2 * component;
        int64_t value = rrj_s32(rrj_read32(m, object + 184 + 4 * component));

        value += ((int64_t)(int16_t)rrj_u16(rrj_at(m, object + 432 + row_offset, 2)) * scaled[0]) >> 12;
        value += ((int64_t)(int16_t)rrj_u16(rrj_at(m, object + 438 + row_offset, 2)) * scaled[1]) >> 12;
        value += ((int64_t)(int16_t)rrj_u16(rrj_at(m, object + 444 + row_offset, 2)) * scaled[2]) >> 12;
        result = (int32_t)value >> 10;
        rrj_write32(m, output + 4 * component, (uint32_t)result);
    }
    return (uint32_t)result;
}

uint32_t sub_80028C78(RRJMemory *m, uint32_t object, uint32_t output, uint32_t input, uint32_t player_index)
{
    uint32_t player = rrj_read32(m, 0x8005AEC0u + 4 * player_index);
    uint32_t shift = rrj_u16(rrj_at(m, rrj_read32(m, object) + 14, 2)) >> 12;
    int16_t vector[3];
    MATRIX matrix;
    uint32_t row;
    uint32_t column;
    int32_t result = 0;

    FUNCTION_MARKER(0x80028C78, "SLUS_010.53");
    vector[0] = (int16_t)frontier_asr(rrj_read32(m, input), shift);
    vector[1] = (int16_t)(((int32_t)(frontier_asr(rrj_read32(m, input + 4), shift) << 16) >> 4) / 3413);
    vector[2] = (int16_t)frontier_asr(rrj_read32(m, input + 8), shift);
    memset(&matrix, 0, sizeof(matrix));
    for (row = 0; row != 3; ++row)
        for (column = 0; column != 3; ++column)
            matrix.m[row][column] = (int16_t)rrj_u16(rrj_at(m, player + 92 + 6 * column + 2 * row, 2));
    for (row = 0; row != 3; ++row)
        matrix.m[row][1] = (int16_t)(((int32_t)matrix.m[row][1] << 12) / 3413);
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);
    for (row = 0; row != 3; ++row)
    {
        int64_t value = 0;

        for (column = 0; column != 3; ++column)
            value += matrix.m[row][column] * vector[column];
        result = (int32_t)(value >> 12) + rrj_s32(rrj_read32(m, player + 28 + 4 * row));
        rrj_write32(m, output + 4 * row, (uint32_t)result);
    }
    return (uint32_t)result;
}

uint32_t sub_8002A8E4(RRJMemory *m, uint32_t age, uint32_t record)
{
    int32_t state = *(int8_t *)rrj_at(m, record + 97, 1);
    uint32_t result = rrj_read32(m, 0x800536D0u + 4 * (uint32_t)state) < age;

    FUNCTION_MARKER(0x8002A8E4, "SLUS_010.53");
    if (result != 0)
    {
        rrj_write32(m, record + 48, rrj_read32(m, rrj_read32(m, 0x8005B2F8u) + 16));
        rrj_write32(m, record, (rrj_read32(m, record) & 0xFFFFC3FFu) | 0x800u);
        if (state != 0)
        {
            uint8_t next = (uint8_t)(*(uint8_t *)rrj_at(m, record + 97, 1) - 1);

            *(uint8_t *)rrj_at(m, record + 97, 1) = next != 0 ? next : 2;
            result = 2;
        }
        else
        {
            result = rrj_read32(m, record);
        }
    }
    return result;
}

uint32_t sub_80017F64(RRJMemory *m, uint32_t actor_id, uint32_t listener, RRJRaceLeafCall call)
{
    uint32_t table = rrj_read32(m, 0x8005B410u + 4 * listener);
    uint32_t metadata = rrj_read32(m, 0x8005B40Cu) + 72 * listener;
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
    first = rrj_read32(m, metadata + 36);
    end = first + rrj_read32(m, metadata + 40);
    for (slot = first; slot < end; ++slot)
    {
        record = table + 44 * slot;
        if (rrj_u16(rrj_at(m, record, 2)) == actor_id && rrj_s32(rrj_read32(m, record + 8)) >= 3 && rrj_read32(m, record + 4) != 0)
            return rrj_read32(m, record + 4);
    }
    for (slot = first; slot < end; ++slot)
    {
        record = table + 44 * slot;
        if (rrj_read32(m, record + 4) == 0)
        {
            selected = slot;
            break;
        }
        if (*(uint8_t *)rrj_at(m, rrj_read32(m, 0x8005B2F8u) + 4, 1) == 33 && rrj_s32(rrj_read32(m, record + 8)) >= 3 && ++protected_count >= 2)
            return 0;
    }
    if (selected == UINT32_MAX)
        selected = first;
    record = table + 44 * selected;
    if (rrj_read32(m, record + 4) != 0)
        (void)sub_80017814(m, selected, listener, call);
    rrj_put16(rrj_at(m, record, 2), (uint16_t)actor_id);
    actor_entry = 0x800CE4D0u + 16 * (uint32_t)((int32_t)actor_id >> 5);
    actor = rrj_read32(m, actor_entry) + rrj_read32(m, actor_entry + 4) * (actor_id & 31u);
    rrj_write32(m, record + 12, rrj_read32(m, actor + 184));
    rrj_write32(m, record + 16, rrj_read32(m, actor + 192));
    rrj_write32(m, record + 20, (uint32_t)(((int64_t)(int16_t)rrj_u16(rrj_at(m, actor + 450, 2)) * (rrj_s32(rrj_read32(m, actor + 480)) >> 4)) >> 8));
    rrj_write32(m, record + 24, (uint32_t)(((int64_t)(int16_t)rrj_u16(rrj_at(m, actor + 454, 2)) * (rrj_s32(rrj_read32(m, actor + 480)) >> 4)) >> 8));
    sound = rrj_read32(m, 0x8005B420u);
    rrj_write32(m, record + 28, sub_8001F934(m, (int32_t)sound, 11, 0));
    rrj_write32(m, record + 8, rrj_read32(m, actor + 180) != 0 ? (actor_id & 1u) + 3u : 5u);
    return sub_8001769C(m, selected, listener, sound, 11);
}

uint32_t sub_800182B0(RRJMemory *m, uint32_t listener, RRJRaceLeafCall call)
{
    uint32_t metadata = rrj_read32(m, 0x8005B40Cu) + 72 * listener;
    uint32_t first = rrj_read32(m, metadata + 36);
    uint32_t end = first + rrj_read32(m, metadata + 40);
    uint32_t table = rrj_read32(m, 0x8005B410u + 4 * listener);
    uint32_t slot;

    FUNCTION_MARKER(0x800182B0, "SLUS_010.53");
    if (table != 0)
    {
        for (slot = first; slot < end; ++slot)
        {
            uint32_t record = table + 44 * slot;

            if (rrj_s32(rrj_read32(m, record + 8)) >= 3 && rrj_u16(rrj_at(m, record, 2)) == listener)
                return listener;
        }
    }
    return sub_80017F64(m, listener, listener, call);
}

uint32_t sub_8001836C(RRJMemory *m, uint32_t listener, RRJRaceLeafCall call)
{
    uint32_t metadata = rrj_read32(m, 0x8005B40Cu) + 72 * listener;
    uint32_t first = rrj_read32(m, metadata + 36);
    uint32_t end = first + rrj_read32(m, metadata + 40);
    uint32_t table = rrj_read32(m, 0x8005B410u + 4 * listener);
    uint32_t slot;

    FUNCTION_MARKER(0x8001836C, "SLUS_010.53");
    if (table == 0)
        return 0;
    for (slot = first; slot < end; ++slot)
    {
        uint32_t record = table + 44 * slot;

        if (rrj_s32(rrj_read32(m, record + 8)) >= 3 && rrj_u16(rrj_at(m, record, 2)) == listener)
            (void)sub_80017814(m, slot, listener, call);
    }
    return 0;
}

uint32_t sub_8002A974(RRJMemory *m, uint32_t object, uint32_t record, uint32_t age, uint32_t player_index, RRJRaceLeafCall call)
{
    uint32_t type = (rrj_u16(rrj_at(m, rrj_read32(m, object) + 14, 2)) & 0x78u) >> 3;
    uint32_t linked;
    uint32_t listener;
    uint32_t context;

    FUNCTION_MARKER(0x8002A974, "SLUS_010.53");
    (void)sub_8002A8E4(m, age, record);
    if (type != 2)
        return 1;
    linked = rrj_read32(m, object + 852);
    listener = rrj_u16(rrj_at(m, object + 172, 2));
    context = rrj_read32(m, 0x8005B2F8u);
    if (rrj_read32(m, linked + 604) >= 3 || (rrj_u16(rrj_at(m, 0x800541D4u + 8 * rrj_u16(rrj_at(m, linked + 544, 2)) + 2, 2)) == 0 && rrj_u16(rrj_at(m, linked + 544, 2)) == 0))
    {
        rrj_write32(m, record, rrj_read32(m, record) & 0xFFFFC3FFu);
        rrj_write32(m, object + 36, rrj_read32(m, object + 36) & 0xCFFFFFFFu);
        if (listener < rrj_read32(m, context + 48))
            (void)sub_8001836C(m, listener, call);
    }
    if ((int16_t)(rrj_read32(m, object + 44 + 4 * player_index) >> 6) >= 36)
        rrj_write32(m, record, rrj_read32(m, record) & 0xFFFFC3FFu);
    if (((rrj_read32(m, record) >> 10) & 0xFu) == 0)
        return 0;
    rrj_write32(m, object + 36, rrj_read32(m, object + 36) | 0x30000000u);
    if (listener < rrj_read32(m, context + 48))
        (void)sub_800182B0(m, listener, call);
    return 1;
}

uint32_t sub_80029D88(RRJMemory *m, uint32_t object, uint32_t record, uint32_t player_index, uint32_t age, RRJRaceLeafCall call)
{
    uint32_t command = rrj_read32(m, record);
    uint32_t type = (command >> 6) & 0xFu;
    uint32_t context = rrj_read32(m, 0x8005B2F8u);
    uint32_t current_time = rrj_read32(m, context + 16);
    uint32_t reset = 0;
    uint32_t result;

    FUNCTION_MARKER(0x80029D88, "SLUS_010.53");
    if (type != 5 && rrj_read32(m, record + 52) < age)
    {
        rrj_write32(m, record, (command & 0xFFFFC3FFu) | 0x1000u);
        return rrj_read32(m, record);
    }
    if (*(uint8_t *)rrj_at(m, record + 109, 1) != 0 && type != 5 && rrj_read32(m, record + 96) < current_time - rrj_read32(m, record + 100))
    {
        uint8_t frame = *(uint8_t *)rrj_at(m, record + 108, 1);
        uint8_t count = *(uint8_t *)rrj_at(m, record + 109, 1);

        *(uint8_t *)rrj_at(m, record + 108, 1) = frame == count - 1 ? 0 : frame + 1;
        rrj_write32(m, record + 100, current_time);
    }
    switch (type)
    {
        case 1:
        {
            uint32_t subtype = (rrj_read32(m, record) >> 14) & 0xFFu;
            uint32_t target = object;
            uint32_t scale = 0x80053670u + 12 * subtype;

            if (subtype == 3 && rrj_read32(m, object + 856) != 0 && rrj_read32(m, object + 1088) != 0)
            {
                target = rrj_read32(m, object + 856);
                scale = 0x800536B8u;
            }
            (void)sub_800289E8(m, target, scale, record + 4);
            if (((rrj_read32(m, record) >> 10) & 0xFu) == 1)
            {
                rrj_write32(m, record, (rrj_read32(m, record) & 0xFFFFC3FFu) | 0x800u);
                rrj_write32(m, record + 20, rrj_read32(m, record + 4));
                rrj_write32(m, record + 24, rrj_read32(m, record + 8));
                rrj_write32(m, record + 28, rrj_read32(m, record + 12));
            }
            rrj_write32(m, record + 36, rrj_read32(m, record + 4) - rrj_read32(m, record + 20));
            rrj_write32(m, record + 40, rrj_read32(m, record + 8) - rrj_read32(m, record + 24));
            rrj_write32(m, record + 44, rrj_read32(m, record + 12) - rrj_read32(m, record + 28));
            (void)sub_8002E810(m, 13107, record + 36, record + 36);
            reset = (rrj_read32(m, object + 568) & 0x600u) != 0;
            break;
        }
        case 2:
        {
            uint32_t subtype = (rrj_read32(m, record) >> 14) & 0xFFu;

            (void)sub_800289E8(m, object, 0x80053670u + 12 * subtype, record + 4);
            if (((rrj_read32(m, record) >> 10) & 0xFu) == 1)
            {
                rrj_write32(m, record, (rrj_read32(m, record) & 0xFFFFC3FFu) | 0x800u);
                rrj_write32(m, record + 20, rrj_read32(m, record + 4));
                rrj_write32(m, record + 24, rrj_read32(m, record + 8));
                rrj_write32(m, record + 28, rrj_read32(m, record + 12));
            }
            rrj_write32(m, record + 36, rrj_read32(m, record + 4) - rrj_read32(m, record + 20));
            rrj_write32(m, record + 40, rrj_read32(m, record + 8) - rrj_read32(m, record + 24));
            rrj_write32(m, record + 44, rrj_read32(m, record + 12) - rrj_read32(m, record + 28));
            (void)sub_8002E810(m, 19660, record + 36, record + 36);
            reset = (rrj_read32(m, object + 568) & 0x600u) != 0;
            (void)sub_80028E74(m, record);
            break;
        }
        case 3:
            (void)sub_80028C78(m, object, record + 4, record + 80, player_index);
            if (((rrj_read32(m, record) >> 10) & 0xFu) == 1)
            {
                rrj_write32(m, record, (rrj_read32(m, record) & 0xFFFFC3FFu) | 0x800u);
                rrj_write32(m, record + 20, rrj_read32(m, record + 4));
                rrj_write32(m, record + 24, rrj_read32(m, record + 8));
                rrj_write32(m, record + 28, rrj_read32(m, record + 12));
            }
            rrj_write32(m, record + 36, rrj_read32(m, record + 4) - rrj_read32(m, record + 20));
            rrj_write32(m, record + 40, rrj_read32(m, record + 8) - rrj_read32(m, record + 24));
            rrj_write32(m, record + 44, rrj_read32(m, record + 12) - rrj_read32(m, record + 28));
            (void)sub_8002E810(m, 0x8000u, record + 36, record + 36);
            reset = (rrj_read32(m, object + 568) & 0x600u) != 0;
            break;
        case 4:
        {
            uint32_t duration = rrj_read32(m, record + 52);
            int32_t phase = duration != 0 ? (int32_t)(((current_time - rrj_read32(m, record + 48)) << 17) / duration) : 0;

            (void)sub_80028C78(m, object, record + 4, record + 80, player_index);
            if (((rrj_read32(m, record) >> 10) & 0xFu) == 1)
                rrj_write32(m, record, (rrj_read32(m, record) & 0xFFFFC3FFu) | 0x800u);
            if (phase > 0x10000)
                phase = 0x20000 - phase;
            (void)sub_8002EE50(m, (uint32_t)phase, object + 450, record + 36);
            rrj_write32(m, record + 36, (uint32_t)(rrj_s32(rrj_read32(m, record + 36)) >> 10));
            rrj_write32(m, record + 40, (uint32_t)(rrj_s32(rrj_read32(m, record + 40)) >> 10));
            rrj_write32(m, record + 44, (uint32_t)(rrj_s32(rrj_read32(m, record + 44)) >> 10));
            break;
        }
        case 5:
            (void)sub_8002A974(m, object, record, age, player_index, call);
            break;
        case 6:
            (void)sub_80028C78(m, object, record + 20, record + 80, player_index);
            (void)sub_80028E74(m, record);
            break;
        case 7:
            rrj_write32(m, record + 36, 0);
            rrj_write32(m, record + 44, 0);
            rrj_write32(m, record, (rrj_read32(m, record) & 0xFFFFC3FFu) | 0x800u);
            break;
        default:
            break;
    }
    result = (rrj_read32(m, record) >> 6) & 0xFu;
    if (result != 5)
    {
        if (*(uint8_t *)rrj_at(m, record + 61, 1) != 0)
        {
            uint32_t angle = rrj_read32(m, 0x800D8074u) & 0xFFFu;
            int32_t amplitude = rrj_s32(rrj_read32(m, record + 64));
            int32_t cosine = (int16_t)rrj_u16(rrj_at(m, 0x8005624Eu + 4 * angle, 2));
            int32_t sine = (int16_t)rrj_u16(rrj_at(m, 0x8005624Cu + 4 * angle, 2));

            rrj_write32(m, record + 36, rrj_read32(m, record + 36) + (uint32_t)(((int64_t)(cosine << 4) * amplitude) >> 16));
            rrj_write32(m, record + 44, rrj_read32(m, record + 44) + (uint32_t)(((int64_t)(sine << 4) * amplitude) >> 16));
        }
        rrj_write32(m, record + 20, rrj_read32(m, record + 20) + rrj_read32(m, record + 36));
        rrj_write32(m, record + 24, rrj_read32(m, record + 24) + rrj_read32(m, record + 40));
        rrj_write32(m, record + 28, rrj_read32(m, record + 28) + rrj_read32(m, record + 44));
        result = rrj_read32(m, record + 24);
    }
    if (reset != 0)
    {
        rrj_write32(m, record, (rrj_read32(m, record) & 0xFFFFC3FFu) | 0x1000u);
        result = rrj_read32(m, record);
    }
    return result;
}

uint32_t sub_8002823C(RRJMemory *m, uint32_t object, uint32_t player_index, RRJRaceLeafCall call)
{
    int32_t effect_index = *(int8_t *)rrj_at(m, object + 73, 1);
    uint32_t local_flags = 0x1F800230u;
    uint32_t child_index;

    FUNCTION_MARKER(0x8002823C, "SLUS_010.53");
    rrj_write32(m, local_flags, 0);
    if (effect_index != -1)
    {
        uint32_t first = 0x800D39B0u + 112 * (uint32_t)effect_index;
        uint32_t first_type = (rrj_read32(m, first) >> 6) & 0xFu;

        if (*(int8_t *)rrj_at(m, object + 8, 1) <= 0 || first_type >= 5)
        {
            uint32_t counter = rrj_read32(m, 0x800D8074u) + 1;
            uint32_t entry = first;
            uint32_t previous = 0;

            if (counter >= 4097)
                counter = 0;
            rrj_write32(m, 0x800D8074u, counter);
            while (entry != 0)
            {
                uint32_t command = rrj_read32(m, entry);
                int32_t next = rrj_s32(command << 26) >> 26;
                uint32_t type = (command >> 6) & 0xFu;
                uint32_t age = rrj_read32(m, rrj_read32(m, 0x8005B2F8u) + 16) - rrj_read32(m, entry + 48);

                if ((*(uint8_t *)rrj_at(m, object + 9, 1) & 8u) == 0 || type == 5 || type == 6)
                    (void)sub_80029D88(m, object, entry, player_index, age, call);
                if (((command >> 10) & 0xFu) == 4)
                {
                    (void)sub_8002A738(m, object, previous, entry, (uint32_t)next);
                }
                else
                {
                    if (type == 5)
                    {
                        (void)sub_8002AB14(m, object, entry);
                    }
                    else
                    {
                        (void)sub_80028E8C(m, entry + 20, entry + 68, player_index);
                        (void)sub_8002A2C8(m, object, entry, age, player_index, local_flags);
                    }
                    previous = entry;
                }
                entry = next == -1 ? 0 : 0x800D39B0u + 112 * (uint32_t)next;
            }
            *(uint8_t *)rrj_at(m, object + 9, 1) |= 8u;
        }
    }
    for (child_index = 0; child_index != 2; ++child_index)
    {
        uint32_t child = rrj_read32(m, object + 56 + 8 * child_index);

        if (child != 0)
            (void)sub_8002823C(m, child, player_index, call);
    }
    return 0;
}

uint32_t sub_80067770(RRJMemory *m, uint32_t owner, uint32_t player_index, RRJRaceLeafCall call)
{
    uint32_t object = rrj_read32(m, 0x8005B280u);
    uint32_t previous = 0;
    uint32_t result = 0x80060000u;

    FUNCTION_MARKER(0x80067770, "RASHCDG.BIN");
    while (object != 0)
    {
        result = rrj_read32(m, object + 176);
        if (result == owner)
        {
            uint32_t next;

            result = sub_8002823C(m, object, player_index, call);
            next = rrj_read32(m, object + 168);
            object = next;
            if (previous != 0)
                rrj_write32(m, previous + 168, next);
            else
                rrj_write32(m, 0x8005B280u, next);
        }
        else
        {
            object = rrj_read32(m, object + 168);
        }
        previous = object;
    }
    return result;
}

uint32_t sub_800674D4(RRJMemory *m, uint32_t indices, int32_t count, uint32_t player_index, RRJRaceLeafCall call)
{
    uint32_t local_indices[12];
    int32_t index;

    FUNCTION_MARKER(0x800674D4, "RASHCDG.BIN");
    for (index = 0; index < count; ++index)
        local_indices[index] = rrj_read32(m, indices + 4 * (uint32_t)index);
    return frontier_prepare_object_indices(m, local_indices, count, player_index, call);
}

int32_t sub_8003328C(RRJMemory *m, uint32_t owner, int32_t group)
{
    int32_t index = 24 * group;
    int32_t end = index + 24;
    uint32_t entry = 0x800D9268u + 1152 * (uint32_t)group;

    FUNCTION_MARKER(0x8003328C, "SLUS_010.53");
    while (index < end)
    {
        if (rrj_read32(m, entry + 4) == owner && rrj_read32(m, entry) == 0xFFFFFFFFu)
            return index;
        ++index;
        entry += 48;
    }
    return -1;
}
