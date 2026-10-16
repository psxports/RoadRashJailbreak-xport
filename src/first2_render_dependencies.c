#include "psx.h"
#include "gpu.h"
#include "first2_render_dependencies.h"
#include "race_bodyless_batch_007.h"
#include "race_pause_frontier.h"

uint32_t sub_800657A8(void)
{
    int32_t scalar_column_151;
    int32_t scalar_column_152;
    int32_t scalar_column_153;
    int32_t scalar_column_154;
    int32_t scalar_column_155;

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
    w_u16(rectangle + 4, 16);
    w_u16(rectangle + 6, 16);
    if ((int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(28))))) == 1)
    {
        row = (int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(54))))) + 1;
        if (row >= 110)
            row = 0;
        budget = 2 - row % 2;
        if ((int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(30))))) < budget)
            budget = (int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(30)))));
        --budget;
        while (budget != -1)
        {
            manager = r_u32(0x8005B278u);
            remaining = r_u8(manager + 182 + (uint32_t)row);
            if (remaining)
            {
                w_u16(rectangle, (uint16_t)r_u16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(60))))));
                w_u16(rectangle + 2, (uint16_t)r_u16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(62))))));
                height = ((scalar_column_151 = ((int32_t)(uint32_t)((uint32_t)(int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(60))))) - r_u32(0x8005B390u))) / 64), (int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + 64 + 2 * (uint32_t)scalar_column_151))));
                bits = 6;
                codes = r_u32(manager + 988) + 8 + 2 * (uint32_t)row;
                pixels = manager + 1220 + (uint32_t)(int32_t)r_s16(((uint32_t)(manager + 292 + 2 * (uint32_t)row)));
                while (((r_u8(codes) >> bits) & 3u) == 0)
                {
                    bits -= 2;
                    if (bits < 0)
                    {
                        bits = 6;
                        ++codes;
                    }
                }
                masks = r_u32(manager + 996) + 72 + 8 * (uint32_t)(int32_t)r_s16(((uint32_t)(manager + 512 + 2 * (uint32_t)row)));
                while (remaining)
                {
                    w_u16(r_u32(0x8005B278u) + 46, (uint16_t)((uint32_t)(int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(46))))) + 1));
                    if ((int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(40))))) - 1 < (int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(46))))))
                        w_u16(r_u32(0x8005B278u) + 46, (uint16_t)0);
                    if (!(int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(32))))) && (int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(46))))) == (int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(44))))))
                    {
                        skipped = 0;
                        while (skipped < remaining)
                        {
                            skipped += r_u8(r_u32(0x8005B278u) + 182 + (uint32_t)(int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(52))))));
                            w_u16(r_u32(0x8005B278u) + 52, (uint16_t)((uint32_t)(int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(52))))) + 1));
                            if ((int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(52))))) == 110)
                                w_u16(r_u32(0x8005B278u) + 52, (uint16_t)0);
                        }
                        int32_t reclaim_height = ((scalar_column_152 = ((int32_t)(uint32_t)((uint32_t)(int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(56))))) - r_u32(0x8005B390u))) / 64), (int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + 64 + 2 * (uint32_t)scalar_column_152))));
                        w_u16(r_u32(0x8005B278u) + 58, (uint16_t)((uint32_t)(int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(58))))) + (uint32_t)skipped * 16u));
                        while ((int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(58))))) >= reclaim_height)
                        {
                            w_u16(r_u32(0x8005B278u) + 58, (uint16_t)((uint32_t)(int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(58))))) - (uint32_t)reclaim_height + (uint32_t)r_u16(0x8005B394u)));
                            w_u16(r_u32(0x8005B278u) + 56, (uint16_t)((uint32_t)(int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(56))))) + 16));
                            if ((int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(56))))) >= (int16_t)((uint32_t)r_u16(0x8005B390u) + 192))
                                w_u16(r_u32(0x8005B278u) + 56, (uint16_t)r_u16(0x8005B390u));
                        }
                        w_u16(r_u32(0x8005B278u) + 36, (uint16_t)((uint32_t)(int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(36))))) + (uint32_t)skipped));
                        if ((int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(36))))) >= (int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(38))))))
                            w_u16(r_u32(0x8005B278u) + 36, (uint16_t)((uint32_t)(int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(36))))) - (uint32_t)(int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(38)))))));
                        w_u16(r_u32(0x8005B278u) + 44, (uint16_t)((uint32_t)(int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(44))))) + (uint32_t)skipped));
                        if ((int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(44))))) >= (int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(38))))))
                            w_u16(r_u32(0x8005B278u) + 44, (uint16_t)((uint32_t)(int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(44))))) - (uint32_t)(int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(38)))))));
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
                        (void)sub_800102A4(pixels, masks);
                        masks += 8;
                    }
                    (void)rrj_gpu_upload(xport_guest_ptr(rectangle, 8), pixels);
                    w_u16(r_u32(0x8005B278u) + 38, (uint16_t)((uint32_t)(int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(38))))) + 1));
                    if ((int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(40))))) < (int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(38))))))
                        w_u16(r_u32(0x8005B278u) + 38, (uint16_t)r_u16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(40))))));
                    pixels += 512;
                    w_u16(rectangle + 2, (uint16_t)((int32_t)r_s16(((uint32_t)(rectangle + 2))) + 16));
                    if ((int32_t)r_s16(((uint32_t)(rectangle + 2))) >= height)
                    {
                        w_u16(rectangle + 2, (uint16_t)r_u16(0x8005B394u));
                        w_u16(rectangle, (uint16_t)((uint32_t)r_u16(rectangle) + 16));
                        if ((int32_t)r_s16(((uint32_t)(rectangle))) >= (int16_t)((uint32_t)r_u16(0x8005B390u) + 192))
                            w_u16(rectangle, (uint16_t)r_u16(0x8005B390u));
                        height = ((scalar_column_153 = ((int32_t)(uint32_t)((uint32_t)(int32_t)r_s16(((uint32_t)(rectangle))) - r_u32(0x8005B390u))) / 64), (int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + 64 + 2 * (uint32_t)scalar_column_153))));
                    }
                    --remaining;
                }
                w_u16(r_u32(0x8005B278u) + 60, (uint16_t)r_u16(rectangle));
                w_u16(r_u32(0x8005B278u) + 32, (uint16_t)0);
                w_u16(r_u32(0x8005B278u) + 62, (uint16_t)r_u16((rectangle + 2)));
            }
            w_u16(r_u32(0x8005B278u) + 54, (uint16_t)((uint32_t)row));
            w_u16(r_u32(0x8005B278u) + 30, (uint16_t)((uint32_t)(int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(30))))) - 1));
            ++row;
            if ((int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(30))))) <= 0)
                goto complete;
            if (row >= 110)
                row = 0;
            --budget;
        }
    }
    else
    {
        row = (int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(52))))) - 1;
        if (row < 0)
            row = 109;
        budget = row % 2 + 1;
        if ((int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(30))))) < budget)
            budget = (int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(30)))));
        --budget;
        while (budget != -1)
        {
            manager = r_u32(0x8005B278u);
            remaining = r_u8(manager + 182 + (uint32_t)row);
            if (remaining)
            {
                w_u16(rectangle, (uint16_t)r_u16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(56))))));
                w_u16(rectangle + 2, (uint16_t)r_u16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(58))))));
                bits = 0;
                codes = r_u32(manager + 988) + 9 + 2 * (uint32_t)row;
                pixels = manager + 1220 + (uint32_t)(int32_t)r_s16(((uint32_t)(manager + 292 + 2 * (uint32_t)row))) + ((uint32_t)remaining - 1) * 512;
                while (((r_u8(codes) >> bits) & 3u) == 0)
                {
                    bits += 2;
                    if (bits >= 7)
                    {
                        bits = 0;
                        --codes;
                    }
                }
                masks = r_u32(manager + 996) + 72;
                if (row < 109)
                    masks += 8 * (uint32_t)((int32_t)r_s16(((uint32_t)(manager + 512 + 2 * (uint32_t)(row + 1)))) - 1);
                else
                {
                    int32_t look_bits = bits + 2;
                    uint32_t look_codes = codes;
                    masks += 8 * (uint32_t)(int32_t)r_s16(((uint32_t)(manager + 512 + 2 * (uint32_t)row)));
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
                    w_u16(r_u32(0x8005B278u) + 44, (uint16_t)((uint32_t)(int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(44))))) - 1));
                    if ((int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(44))))) < 0)
                        w_u16(r_u32(0x8005B278u) + 44, (uint16_t)((uint32_t)(int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(40))))) - 1));
                    if (!(int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(32))))) && (int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(46))))) == (int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(44))))))
                    {
                        skipped = 0;
                        while (skipped < remaining)
                        {
                            skipped += r_u8(r_u32(0x8005B278u) + 182 + (uint32_t)(int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(54))))));
                            w_u16(r_u32(0x8005B278u) + 54, (uint16_t)((uint32_t)(int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(54))))) - 1));
                            if ((int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(54))))) < 0)
                                w_u16(r_u32(0x8005B278u) + 54, (uint16_t)109);
                        }
                        w_u16(r_u32(0x8005B278u) + 62, (uint16_t)((uint32_t)(int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(62))))) - (uint32_t)skipped * 16u));
                        while ((int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(62))))) < ((int32_t)(uint32_t)(r_u32(0x8005B394u))))
                        {
                            w_u16(r_u32(0x8005B278u) + 60, (uint16_t)((uint32_t)(int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(60))))) - 16));
                            if ((int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(60))))) < (int32_t)r_s16(((uint32_t)(0x8005B390u))))
                                w_u16(r_u32(0x8005B278u) + 60, (uint16_t)((uint32_t)r_u16(0x8005B390u) + 176));
                            height = ((scalar_column_154 = ((int32_t)(uint32_t)((uint32_t)(int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(60))))) - r_u32(0x8005B390u))) / 64), (int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + 64 + 2 * (uint32_t)scalar_column_154))));
                            w_u16(r_u32(0x8005B278u) + 62, (uint16_t)((uint32_t)(int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(62))))) + (uint32_t)height - (uint32_t)r_u16(0x8005B394u)));
                        }
                        w_u16(r_u32(0x8005B278u) + 46, (uint16_t)((uint32_t)(int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(46))))) - (uint32_t)skipped));
                        if ((int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(46))))) < 0)
                            w_u16(r_u32(0x8005B278u) + 46, (uint16_t)((uint32_t)(int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(46))))) + (uint32_t)(int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(38)))))));
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
                        (void)sub_800102A4(pixels, masks);
                        masks -= 8;
                    }
                    (void)rrj_gpu_upload(xport_guest_ptr(rectangle, 8), pixels);
                    pixels -= 512;
                    w_u16(rectangle + 2, (uint16_t)((int32_t)r_s16(((uint32_t)(rectangle + 2))) - 16));
                    if ((int32_t)r_s16(((uint32_t)(rectangle + 2))) < ((int32_t)(uint32_t)(r_u32(0x8005B394u))))
                    {
                        w_u16(rectangle, (uint16_t)((uint32_t)r_u16(rectangle) - 16));
                        if ((int32_t)r_s16(((uint32_t)(rectangle))) < (int32_t)r_s16(((uint32_t)(0x8005B390u))))
                        {
                            w_u16(rectangle, (uint16_t)((uint32_t)r_u16(0x8005B390u) + 176));
                            w_u16(r_u32(0x8005B278u) + 38, (uint16_t)r_u16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(40))))));
                        }
                        height = ((scalar_column_155 = ((int32_t)(uint32_t)((uint32_t)(int32_t)r_s16(((uint32_t)(rectangle))) - r_u32(0x8005B390u))) / 64), (int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + 64 + 2 * (uint32_t)scalar_column_155))));
                        w_u16(rectangle + 2, (uint16_t)(height - 16));
                    }
                    w_u16(r_u32(0x8005B278u) + 36, (uint16_t)((uint32_t)(int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(36))))) - 1));
                    if ((int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(36))))) < 0)
                        w_u16(r_u32(0x8005B278u) + 36, (uint16_t)((uint32_t)(int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(40))))) - 1));
                    --remaining;
                }
                w_u16(r_u32(0x8005B278u) + 32, (uint16_t)0);
                w_u16(r_u32(0x8005B278u) + 56, (uint16_t)r_u16(rectangle));
                w_u16(r_u32(0x8005B278u) + 58, (uint16_t)r_u16((rectangle + 2)));
            }
            if ((int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(40))))) < (int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(38))))))
                w_u16(r_u32(0x8005B278u) + 38, (uint16_t)r_u16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(40))))));
            w_u16(r_u32(0x8005B278u) + 52, (uint16_t)((uint32_t)row));
            w_u16(r_u32(0x8005B278u) + 30, (uint16_t)((uint32_t)(int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(30))))) - 1));
            --row;
            if ((int32_t)r_s16(((uint32_t)(r_u32(0x8005B278u) + ((uint32_t)(30))))) <= 0)
                goto complete;
            if (row < 0)
                row = 109;
            --budget;
        }
    }
    while ((uint32_t)DrawSync(1))
    {
        /* Original 800662B4 is a return-only wait hook */
    }
    (void)sub_800662BC();
    return 0;
complete:
    w_u32(r_u32(0x8005B278u) + 24, 0);
    w_u16(r_u32(0x8005B278u) + 30, (uint16_t)0);
    return 1;
}
