#include "first2_render_dependencies.h"
#include "race_bodyless_batch_007.h"
#include "race_pause_frontier.h"

static uint32_t render_manager(RRJMemory *m)
{
    return rrj_read32(0x8005B278u);
}

static int32_t render_half(RRJMemory *m, uint32_t address)
{
    return (int16_t)rrj_u16(rrj_at(address, 2));
}

static uint32_t render_unsigned_half(RRJMemory *m, uint32_t address)
{
    return rrj_u16(rrj_at(address, 2));
}

static int32_t manager_half(RRJMemory *m, uint32_t offset)
{
    return render_half(m, render_manager(m) + offset);
}

static void manager_put(RRJMemory *m, uint32_t offset, uint32_t value)
{
    rrj_put16(rrj_at(render_manager(m) + offset, 2), (uint16_t)value);
}

static uint32_t render_call(RRJMemory *m, RRJRaceLeafCall call, uint32_t target, uint32_t first, uint32_t second)
{
    const uint32_t args[8] = {first, second, 0, 0, 0, 0, 0, 0};
    return call(m, target, args);
}

static int32_t render_column_height(RRJMemory *m, int32_t x)
{
    int32_t column = rrj_s32((uint32_t)x - rrj_read32(0x8005B390u)) / 64;
    return render_half(m, render_manager(m) + 64 + 2 * (uint32_t)column);
}

uint32_t sub_800657A8(RRJRaceLeafCall call)
{
    const uint32_t rectangle = 0x800CCD70u;
    int32_t row;
    int32_t budget;
    int32_t remaining;
    int32_t bits;
    int32_t height;
    uint32_t manager;
    uint32_t codes;
    uint32_t pixels;
    uint32_t masks;
    uint32_t code;
    int32_t skipped;

    FUNCTION_MARKER(0x800657A8, "RASHCDG.BIN");
    rrj_put16(rrj_at(rectangle + 4, 2), 16);
    rrj_put16(rrj_at(rectangle + 6, 2), 16);
    if (manager_half(rrj_host_context(), 28) == 1)
    {
        row = manager_half(rrj_host_context(), 54) + 1;
        if (row >= 110)
            row = 0;
        budget = 2 - row % 2;
        if (manager_half(rrj_host_context(), 30) < budget)
            budget = manager_half(rrj_host_context(), 30);
        --budget;
        while (budget != -1)
        {
            manager = render_manager(rrj_host_context());
            remaining = r_u8(manager + 182 + (uint32_t)row);
            if (remaining)
            {
                rrj_put16(rrj_at(rectangle, 2), (uint16_t)manager_half(rrj_host_context(), 60));
                rrj_put16(rrj_at(rectangle + 2, 2), (uint16_t)manager_half(rrj_host_context(), 62));
                height = render_column_height(rrj_host_context(), manager_half(rrj_host_context(), 60));
                bits = 6;
                codes = rrj_read32(manager + 988) + 8 + 2 * (uint32_t)row;
                pixels = manager + 1220 + (uint32_t)render_half(rrj_host_context(), manager + 292 + 2 * (uint32_t)row);
                while (((r_u8(codes) >> bits) & 3u) == 0)
                {
                    bits -= 2;
                    if (bits < 0)
                    {
                        bits = 6;
                        ++codes;
                    }
                }
                masks = rrj_read32(manager + 996) + 72 + 8 * (uint32_t)render_half(rrj_host_context(), manager + 512 + 2 * (uint32_t)row);
                while (remaining)
                {
                    manager_put(rrj_host_context(), 46, (uint32_t)manager_half(rrj_host_context(), 46) + 1);
                    if (manager_half(rrj_host_context(), 40) - 1 < manager_half(rrj_host_context(), 46))
                        manager_put(rrj_host_context(), 46, 0);
                    if (!manager_half(rrj_host_context(), 32) && manager_half(rrj_host_context(), 46) == manager_half(rrj_host_context(), 44))
                    {
                        skipped = 0;
                        while (skipped < remaining)
                        {
                            skipped += r_u8(render_manager(rrj_host_context()) + 182 + (uint32_t)manager_half(rrj_host_context(), 52));
                            manager_put(rrj_host_context(), 52, (uint32_t)manager_half(rrj_host_context(), 52) + 1);
                            if (manager_half(rrj_host_context(), 52) == 110)
                                manager_put(rrj_host_context(), 52, 0);
                        }
                        int32_t reclaim_height = render_column_height(rrj_host_context(), manager_half(rrj_host_context(), 56));
                        manager_put(rrj_host_context(), 58, (uint32_t)manager_half(rrj_host_context(), 58) + (uint32_t)skipped * 16u);
                        while (manager_half(rrj_host_context(), 58) >= reclaim_height)
                        {
                            manager_put(rrj_host_context(), 58, (uint32_t)manager_half(rrj_host_context(), 58) - (uint32_t)reclaim_height + render_unsigned_half(rrj_host_context(), 0x8005B394u));
                            manager_put(rrj_host_context(), 56, (uint32_t)manager_half(rrj_host_context(), 56) + 16);
                            if (manager_half(rrj_host_context(), 56) >= (int16_t)(render_unsigned_half(rrj_host_context(), 0x8005B390u) + 192))
                                manager_put(rrj_host_context(), 56, render_unsigned_half(rrj_host_context(), 0x8005B390u));
                        }
                        manager_put(rrj_host_context(), 36, (uint32_t)manager_half(rrj_host_context(), 36) + (uint32_t)skipped);
                        if (manager_half(rrj_host_context(), 36) >= manager_half(rrj_host_context(), 38))
                            manager_put(rrj_host_context(), 36, (uint32_t)manager_half(rrj_host_context(), 36) - (uint32_t)manager_half(rrj_host_context(), 38));
                        manager_put(rrj_host_context(), 44, (uint32_t)manager_half(rrj_host_context(), 44) + (uint32_t)skipped);
                        if (manager_half(rrj_host_context(), 44) >= manager_half(rrj_host_context(), 38))
                            manager_put(rrj_host_context(), 44, (uint32_t)manager_half(rrj_host_context(), 44) - (uint32_t)manager_half(rrj_host_context(), 38));
                    }
                    code = (r_u8(codes) >> bits) & 3u;
                    bits -= 2;
                    if (bits < 0)
                    {
                        bits = 6;
                        ++codes;
                    }
                    if (code == 2)
                    {
                        (void)sub_800102A4(pixels, masks, 2);
                        masks += 8;
                    }
                    (void)render_call(rrj_host_context(), call, 0x80048A6Cu, rectangle, pixels);
                    manager_put(rrj_host_context(), 38, (uint32_t)manager_half(rrj_host_context(), 38) + 1);
                    if (manager_half(rrj_host_context(), 40) < manager_half(rrj_host_context(), 38))
                        manager_put(rrj_host_context(), 38, (uint32_t)manager_half(rrj_host_context(), 40));
                    pixels += 512;
                    rrj_put16(rrj_at(rectangle + 2, 2), (uint16_t)(render_half(rrj_host_context(), rectangle + 2) + 16));
                    if (render_half(rrj_host_context(), rectangle + 2) >= height)
                    {
                        rrj_put16(rrj_at(rectangle + 2, 2), (uint16_t)render_unsigned_half(rrj_host_context(), 0x8005B394u));
                        rrj_put16(rrj_at(rectangle, 2), (uint16_t)(render_unsigned_half(rrj_host_context(), rectangle) + 16));
                        if (render_half(rrj_host_context(), rectangle) >= (int16_t)(render_unsigned_half(rrj_host_context(), 0x8005B390u) + 192))
                            rrj_put16(rrj_at(rectangle, 2), (uint16_t)render_unsigned_half(rrj_host_context(), 0x8005B390u));
                        height = render_column_height(rrj_host_context(), render_half(rrj_host_context(), rectangle));
                    }
                    --remaining;
                }
                manager_put(rrj_host_context(), 60, render_unsigned_half(rrj_host_context(), rectangle));
                manager_put(rrj_host_context(), 32, 0);
                manager_put(rrj_host_context(), 62, render_unsigned_half(rrj_host_context(), rectangle + 2));
            }
            manager_put(rrj_host_context(), 54, (uint32_t)row);
            manager_put(rrj_host_context(), 30, (uint32_t)manager_half(rrj_host_context(), 30) - 1);
            ++row;
            if (manager_half(rrj_host_context(), 30) <= 0)
                goto complete;
            if (row >= 110)
                row = 0;
            --budget;
        }
    }
    else
    {
        row = manager_half(rrj_host_context(), 52) - 1;
        if (row < 0)
            row = 109;
        budget = row % 2 + 1;
        if (manager_half(rrj_host_context(), 30) < budget)
            budget = manager_half(rrj_host_context(), 30);
        --budget;
        while (budget != -1)
        {
            manager = render_manager(rrj_host_context());
            remaining = r_u8(manager + 182 + (uint32_t)row);
            if (remaining)
            {
                rrj_put16(rrj_at(rectangle, 2), (uint16_t)manager_half(rrj_host_context(), 56));
                rrj_put16(rrj_at(rectangle + 2, 2), (uint16_t)manager_half(rrj_host_context(), 58));
                bits = 0;
                codes = rrj_read32(manager + 988) + 9 + 2 * (uint32_t)row;
                pixels = manager + 1220 + (uint32_t)render_half(rrj_host_context(), manager + 292 + 2 * (uint32_t)row) + ((uint32_t)remaining - 1) * 512;
                while (((r_u8(codes) >> bits) & 3u) == 0)
                {
                    bits += 2;
                    if (bits >= 7)
                    {
                        bits = 0;
                        --codes;
                    }
                }
                masks = rrj_read32(manager + 996) + 72;
                if (row < 109)
                    masks += 8 * (uint32_t)(render_half(rrj_host_context(), manager + 512 + 2 * (uint32_t)(row + 1)) - 1);
                else
                {
                    int32_t look_bits = bits + 2;
                    uint32_t look_codes = codes;
                    masks += 8 * (uint32_t)render_half(rrj_host_context(), manager + 512 + 2 * (uint32_t)row);
                    if (look_bits >= 7)
                    {
                        look_bits = 0;
                        --look_codes;
                    }
                    if (((r_u8(look_codes) >> look_bits) & 3u) == 2)
                    {
                        do
                        {
                            look_bits += 2;
                            if (look_bits >= 7)
                            {
                                look_bits = 0;
                                --look_codes;
                            }
                        } while (((r_u8(look_codes) >> look_bits) & 3u) == 2);
                    }
                }
                while (remaining)
                {
                    manager_put(rrj_host_context(), 44, (uint32_t)manager_half(rrj_host_context(), 44) - 1);
                    if (manager_half(rrj_host_context(), 44) < 0)
                        manager_put(rrj_host_context(), 44, (uint32_t)manager_half(rrj_host_context(), 40) - 1);
                    if (!manager_half(rrj_host_context(), 32) && manager_half(rrj_host_context(), 46) == manager_half(rrj_host_context(), 44))
                    {
                        skipped = 0;
                        while (skipped < remaining)
                        {
                            skipped += r_u8(render_manager(rrj_host_context()) + 182 + (uint32_t)manager_half(rrj_host_context(), 54));
                            manager_put(rrj_host_context(), 54, (uint32_t)manager_half(rrj_host_context(), 54) - 1);
                            if (manager_half(rrj_host_context(), 54) < 0)
                                manager_put(rrj_host_context(), 54, 109);
                        }
                        manager_put(rrj_host_context(), 62, (uint32_t)manager_half(rrj_host_context(), 62) - (uint32_t)skipped * 16u);
                        while (manager_half(rrj_host_context(), 62) < rrj_s32(rrj_read32(0x8005B394u)))
                        {
                            manager_put(rrj_host_context(), 60, (uint32_t)manager_half(rrj_host_context(), 60) - 16);
                            if (manager_half(rrj_host_context(), 60) < render_half(rrj_host_context(), 0x8005B390u))
                                manager_put(rrj_host_context(), 60, render_unsigned_half(rrj_host_context(), 0x8005B390u) + 176);
                            height = render_column_height(rrj_host_context(), manager_half(rrj_host_context(), 60));
                            manager_put(rrj_host_context(), 62, (uint32_t)manager_half(rrj_host_context(), 62) + (uint32_t)height - render_unsigned_half(rrj_host_context(), 0x8005B394u));
                        }
                        manager_put(rrj_host_context(), 46, (uint32_t)manager_half(rrj_host_context(), 46) - (uint32_t)skipped);
                        if (manager_half(rrj_host_context(), 46) < 0)
                            manager_put(rrj_host_context(), 46, (uint32_t)manager_half(rrj_host_context(), 46) + (uint32_t)manager_half(rrj_host_context(), 38));
                    }
                    code = (r_u8(codes) >> bits) & 3u;
                    bits += 2;
                    if (bits >= 7)
                    {
                        bits = 0;
                        --codes;
                    }
                    if (code == 2)
                    {
                        (void)sub_800102A4(pixels, masks, 2);
                        masks -= 8;
                    }
                    (void)render_call(rrj_host_context(), call, 0x80048A6Cu, rectangle, pixels);
                    pixels -= 512;
                    rrj_put16(rrj_at(rectangle + 2, 2), (uint16_t)(render_half(rrj_host_context(), rectangle + 2) - 16));
                    if (render_half(rrj_host_context(), rectangle + 2) < rrj_s32(rrj_read32(0x8005B394u)))
                    {
                        rrj_put16(rrj_at(rectangle, 2), (uint16_t)(render_unsigned_half(rrj_host_context(), rectangle) - 16));
                        if (render_half(rrj_host_context(), rectangle) < render_half(rrj_host_context(), 0x8005B390u))
                        {
                            rrj_put16(rrj_at(rectangle, 2), (uint16_t)(render_unsigned_half(rrj_host_context(), 0x8005B390u) + 176));
                            manager_put(rrj_host_context(), 38, (uint32_t)manager_half(rrj_host_context(), 40));
                        }
                        height = render_column_height(rrj_host_context(), render_half(rrj_host_context(), rectangle));
                        rrj_put16(rrj_at(rectangle + 2, 2), (uint16_t)(height - 16));
                    }
                    manager_put(rrj_host_context(), 36, (uint32_t)manager_half(rrj_host_context(), 36) - 1);
                    if (manager_half(rrj_host_context(), 36) < 0)
                        manager_put(rrj_host_context(), 36, (uint32_t)manager_half(rrj_host_context(), 40) - 1);
                    --remaining;
                }
                manager_put(rrj_host_context(), 32, 0);
                manager_put(rrj_host_context(), 56, render_unsigned_half(rrj_host_context(), rectangle));
                manager_put(rrj_host_context(), 58, render_unsigned_half(rrj_host_context(), rectangle + 2));
            }
            if (manager_half(rrj_host_context(), 40) < manager_half(rrj_host_context(), 38))
                manager_put(rrj_host_context(), 38, (uint32_t)manager_half(rrj_host_context(), 40));
            manager_put(rrj_host_context(), 52, (uint32_t)row);
            manager_put(rrj_host_context(), 30, (uint32_t)manager_half(rrj_host_context(), 30) - 1);
            --row;
            if (manager_half(rrj_host_context(), 30) <= 0)
                goto complete;
            if (row < 0)
                row = 109;
            --budget;
        }
    }
    while (render_call(rrj_host_context(), call, 0x800487C0u, 1, 0))
    {
        /* Original 800662B4 is a return-only wait hook */
    }
    (void)sub_800662BC(call);
    return 0;
complete:
    rrj_write32(render_manager(rrj_host_context()) + 24, 0);
    manager_put(rrj_host_context(), 30, 0);
    return 1;
}
