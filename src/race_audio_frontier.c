#include "rrj_debug.h"
#include "race_audio_frontier.h"
#include "race_pause.h"
#include "race_pause_frontier.h"
#include "audio.h"
#include "fixed_math.h"
#include "xport.h"
#include <stdio.h>
#include <stdlib.h>

static int32_t race_audio_mul_asr(int32_t first, int32_t second, uint32_t shift)
{
    uint32_t product = (uint32_t)((int64_t)first * second);

    if (!shift)
        return (int32_t)product;
    return (int32_t)product >> shift;
}

#if defined(LOCKSTEP_DEBUG)
static uint16_t race_audio_spu_registers[0x100];

sint32 rrj_audio_checkpoint_io(FILE *file, sint32 load)
{
    return file && (load ? fread(race_audio_spu_registers, sizeof(race_audio_spu_registers), 1, file) == 1 : fwrite(race_audio_spu_registers, sizeof(race_audio_spu_registers), 1, file) == 1);
}
#endif

static uint16_t race_audio_read_half(uint32_t address)
{
    return r_u16(address);
}

static void race_audio_write_half(uint32_t address, uint16_t value)
{
    w_u16(address, value);
}

uint32_t sub_80019990(void)
{
    FUNCTION_MARKER(0x80019990, "SLUS_010.53");
    if (r_u32(0x8005ACA8u) & 4u)
    {
        uint32_t players = r_u32(r_u32(0x8005B2F8u) + 48);
        uint32_t selector;
        uint32_t record;
        uint32_t step;
        uint32_t position;
        uint32_t target;
        uint32_t raw_position;
        uint32_t selected_position;
        uint32_t modulation_target;
        uint32_t modulation_step;
        uint32_t modulation;
        uint32_t selected_modulation;
        uint32_t active;

        if (players != 2u)
        {
            selector = r_u32(0x8005B3ECu);
            if (selector)
            {
                w_u32(0x8005B3ECu, selector ^ 1u);
                return sub_8001EE94();
            }
        }
        selector = r_u32(0x8005B3ECu);
        record = r_u32(0x8005B42Cu) + 132u * selector;
        step = r_u32(record + 56);
        position = r_u32(record + 48);
        target = r_u32(record + 52);
        raw_position = position + step;
        selected_position = raw_position;
        w_u32(record + 48, raw_position);
        if (((int32_t)(uint32_t)((raw_position - target) * step)) >= 0)
            selected_position = target;
        modulation_target = r_u32(record + 100);
        modulation_step = r_u32(record + 104);
        modulation = r_u32(record + 96);
        w_u32(record + 48, selected_position);
        modulation += modulation_step;
        selected_modulation = modulation;
        w_u32(record + 96, modulation);
        if (((int32_t)(uint32_t)((raw_position - modulation_target) * modulation_step)) >= 0)
            selected_modulation = modulation_target;
        active = r_u8(record + 4);
        w_u32(record + 96, selected_modulation);
        if (active)
        {
            uint8_t event[12];
            uint32_t owner = r_u32(record);
            uint32_t event_pan = r_u32(record + 116);
            uint32_t mode = (r_u32(owner + 564) >> 9) & 1u;
            uint32_t adjusted;
            uint32_t gain;
            uint32_t base;
            uint32_t handle;
            uint32_t factor;
            uint32_t config;
            uint32_t shift;
            uint32_t offset;
            uint32_t value;
            int32_t depth;

            xport_store_le32(event + 8, event_pan);
            position = r_u32(record + 48);
            adjusted = position + ((0u - mode) & (uint32_t)(((int32_t)(uint32_t)(position)) >> 2));
            xport_store_le32(event, adjusted);
            gain = r_u32(record + 120);
            xport_store_le32(event, (uint32_t)(((int32_t)(uint32_t)(adjusted * gain)) >> 16));
            base = r_u32(record + 112);
            xport_store_le32(event + 4, mode ? base : (uint32_t)(((int32_t)(uint32_t)(base * modulation)) >> 7));
            (void)sub_80019C54(r_u32(record + 16), event);
            if (r_u32(record + 20))
            {
                if (mode)
                {
                    int32_t current = ((int32_t)(uint32_t)(xport_load_le32(event)));

                    xport_store_le32(event + 4, base);
                    xport_store_le32(event, (uint32_t)(current >> 1) + (uint32_t)(current >> 2));
                }
                else
                    xport_store_le32(event + 4, (uint32_t)(((int32_t)(uint32_t)(base * (127u - modulation))) >> 7));
                handle = r_u32(record + 20);
            }
            else
            {
                uint32_t sample = sub_8001F934(((int32_t)(uint32_t)(r_u32(record + 8))), 4, 0);

                gain = r_u32(record + 120);
                xport_store_le32(event + 4, base);
                xport_store_le32(event, (uint32_t)(((int32_t)(uint32_t)(gain * sample)) >> 16));
                handle = r_u32(record + 24);
            }
            (void)sub_80019C54(handle, event);
            depth = ((int32_t)(uint32_t)(r_u32(record + 48))) >> 4;
            if (depth >= 128)
                depth = 127;
            xport_store_le32(event + 4, (uint32_t)(((int32_t)(uint32_t)(base * (uint32_t)depth)) >> 7));
            config = r_u32(record + 44);
            value = xport_load_le32(event);
            factor = r_u32(config + 12);
            value = (uint32_t)(((int32_t)(uint32_t)(value * factor)) >> 16);
            xport_store_le32(event, value);
            gain = r_u32(record + 120);
            xport_store_le32(event, (uint32_t)(((int32_t)(uint32_t)(value * gain)) >> 16));
            (void)sub_80019C54(r_u32(record + 28), event);
            xport_store_le32(event + 4, 0);
            config = r_u32(record + 44);
            position = r_u32(record + 48);
            shift = r_u32(config + 16);
            offset = r_u32(record + 64);
            value = (uint32_t)(((int32_t)(uint32_t)(position)) >> (shift & 31u)) + offset;
            xport_store_le32(event, value);
            gain = r_u32(record + 120);
            xport_store_le32(event, (uint32_t)(((int32_t)(uint32_t)(value * gain)) >> 16));
            (void)sub_80019C54(r_u32(record + 32), event);
        }
        w_u32(0x8005B3ECu, r_u32(0x8005B3ECu) ^ 1u);
    }
    return sub_8001EE94();
}

uint32_t sub_800270F0(uint32_t record)
{
    uint32_t sample;
    int32_t value;

    FUNCTION_MARKER(0x800270F0, "SLUS_010.53");
    sample = sub_80043F00(0xF2000002u) & 0xFFu;
    w_u16(record + 56, (uint16_t)(sample << 4));
    sample = sub_80043F00(0xF2000002u) & 0xFFu;
    value = (int16_t)(101u * sample) >> 8;
    w_u16(record + 58, (uint16_t)value);
    if (value < 51)
        return 1;
    value = 50 - value;
    w_u16(record + 58, (uint16_t)value);
    return (uint32_t)value;
}

uint32_t sub_8004F0A8(int32_t index, uint32_t value)
{
    uint32_t alignment, shifted;
    FUNCTION_MARKER(0x8004F0A8, "SLUS_010.53");
    if (r_u32(0x8005A440))
    {
        alignment = r_u32(0x8005A448);
        if (!alignment)
        {
            RRJ_DEBUG_FPUTS("RRJ: original BREAK 7 in SPU address alignment\n", stderr);
            abort();
        }
        if (value % alignment)
            value = (value + alignment) & ~r_u32(0x8005A44C);
    }
    shifted = value >> (r_u32(0x8005A444) & 31);
    if (index == -2)
        return value;
    if (index == -1)
        return shifted & 0xFFFF;
    w_u16(r_u32(0x8005A41C) + ((uint32_t)index << 1), (uint16_t)shifted);
    return value;
}

uint32_t sub_80051A38(int32_t first, int32_t second, int32_t third, int32_t fourth)
{
    uint32_t fraction;
    int32_t pitch;
    int32_t quotient;
    int32_t octave;
    int32_t remainder;
    uint32_t value;

    FUNCTION_MARKER(0x80051A38, "SLUS_010.53");
    fraction = ((uint16_t)fourth + (uint16_t)second) & 0xFFFFu;
    pitch = (int16_t)((uint16_t)third + (fraction >> 7) - (uint16_t)first);
    fraction &= 0x7Fu;
    quotient = pitch / 12;
    octave = quotient - 2;
    remainder = pitch % 12;
    if (remainder < 0)
    {
        remainder += 12;
        octave = quotient - 3;
    }
    value = ((uint32_t)r_u16(0x8005A88Cu + 2 * (uint32_t)remainder) * r_u16(0x8005A8A4u + 2 * fraction)) >> 16;
    if ((int16_t)octave >= 0)
        return 0x3FFFu;
    {
        uint32_t shift = (uint32_t)(-(int16_t)octave) & 31u;

        value += 1u << ((shift - 1u) & 31u);
        return (value >> shift) & 0xFFFFu;
    }
}

uint32_t sub_800506A8(uint32_t mode, uint32_t mask, uint32_t first_index, uint32_t second_index)
{
    uint32_t buffered = r_u32(0x8005A408u) & 1u;
    uint32_t base = buffered ? 0x800DBF20u : r_u32(0x8005A41Cu);
    uint32_t first = race_audio_read_half(base + 2 * first_index);
    uint32_t second = race_audio_read_half(base + 2 * second_index);
    uint32_t packed = first | ((second & 0xFFu) << 16);
    uint32_t first_mask = mask & 0xFFFFu;
    uint32_t second_mask = (mask >> 16) & 0xFFu;

    FUNCTION_MARKER(0x800506A8, "SLUS_010.53");
    if (mode == 1)
    {
        first |= first_mask;
        second |= second_mask;
        packed |= mask & 0xFFFFFFu;
    }
    else if (mode == 0)
    {
        first &= ~first_mask;
        second &= ~second_mask;
        packed &= ~(mask & 0xFFFFFFu);
    }
    else if (mode == 8)
    {
        first = first_mask;
        second = second_mask;
        packed = mask & 0xFFFFFFu;
    }
    else
    {
        return packed & 0xFFFFFFu;
    }
    race_audio_write_half(base + 2 * first_index, (uint16_t)first);
    race_audio_write_half(base + 2 * second_index, (uint16_t)second);
    if (buffered)
    {
        uint32_t bit = (first_index - 198u) >> 1;

        w_u32(0x8005A3D4u, r_u32(0x8005A3D4u) | (1u << (bit & 31u)));
    }
    return packed & 0xFFFFFFu;
}

uint32_t sub_80051088(uint32_t mode, uint32_t mask)
{
    FUNCTION_MARKER(0x80051088, "SLUS_010.53");
    return sub_800506A8(mode, mask, 200, 201);
}

uint32_t sub_8001F900(uint32_t handle, uint32_t enabled)
{
    FUNCTION_MARKER(0x8001F900, "SLUS_010.53");
    return sub_80051088(enabled != 0, 1u << ((handle >> 27) & 31u));
}

static uint16_t race_audio_volume_word(const uint8_t *parameters, uint32_t value_offset, uint32_t mode_offset, uint32_t flags, uint32_t mode_flag)
{
    int32_t raw = (int16_t)xport_load_le16(parameters + value_offset);
    uint32_t value = (uint16_t)raw & 0x7FFFu;
    uint32_t mode = 0;

    if (!flags || (flags & mode_flag))
    {
        uint32_t kind = xport_load_le16(parameters + mode_offset);

        if (kind >= 1 && kind <= 7)
            mode = 0x7000u + 0x1000u * kind;
    }
    if (mode)
    {
        if (raw < 0)
            value = 0;
        else if (raw >= 128)
            value = 127;
    }
    return (uint16_t)(value | mode);
}

uint32_t sub_80051C38(uint32_t voice, const uint8_t *parameters)
{
    uint32_t flags = xport_load_le32(parameters + 4);
    uint32_t all = flags == 0;
    uint32_t base = r_u32(0x8005A41Cu);
    uint32_t output = base + 16 * voice;
    uint32_t value;

    FUNCTION_MARKER(0x80051C38, "SLUS_010.53");
    if (all || (flags & 0x10u))
        race_audio_write_half(output + 4, xport_load_le16(parameters + 20));
    if (all || (flags & 0x40u))
        w_u16(0x8005A3D8u + 2 * voice, xport_load_le16(parameters + 24));
    if (all || (flags & 0x20u))
    {
        uint32_t tuning = r_u16(0x8005A3D8u + 2 * voice);
        uint32_t requested = xport_load_le16(parameters + 22);

        race_audio_write_half(output + 4, (uint16_t)sub_80051A38(tuning >> 8, tuning & 0xFFu, requested >> 8, requested & 0xFFu));
    }
    if (all || (flags & 1u))
        race_audio_write_half(output, race_audio_volume_word(parameters, 8, 12, flags, 4));
    if (all || (flags & 2u))
        race_audio_write_half(output + 2, race_audio_volume_word(parameters, 10, 14, flags, 8));
    if (all || (flags & 0x80u))
        (void)sub_8004F0A8((int32_t)((voice << 3) | 3u), xport_load_le32(parameters + 28));
    if (all || (flags & 0x10000u))
        (void)sub_8004F0A8((int32_t)((voice << 3) | 7u), xport_load_le32(parameters + 32));
    if (all || (flags & 0x20000u))
        race_audio_write_half(output + 8, xport_load_le16(parameters + 58));
    if (all || (flags & 0x40000u))
        race_audio_write_half(output + 10, xport_load_le16(parameters + 60));
    if (all || (flags & 0x800u))
    {
        uint32_t depth = xport_load_le16(parameters + 48);
        uint32_t mode = 0;

        if (depth >= 128)
            depth = 127;
        if ((all || (flags & 0x100u)) && xport_load_le32(parameters + 36) == 5)
            mode = 0x80u;
        value = race_audio_read_half(output + 8);
        race_audio_write_half(output + 8, (uint16_t)((value & 0xFFu) | ((depth | mode) << 8)));
    }
    if (all || (flags & 0x1000u))
    {
        uint32_t rate = xport_load_le16(parameters + 50);

        if (rate >= 16)
            rate = 15;
        value = race_audio_read_half(output + 8);
        race_audio_write_half(output + 8, (uint16_t)((value & 0xFF0Fu) | (rate << 4)));
    }
    if (all || (flags & 0x2000u))
    {
        uint32_t depth = xport_load_le16(parameters + 52);
        uint32_t mode = 0x100u;
        uint32_t kind;

        if (depth >= 128)
            depth = 127;
        if (all || (flags & 0x200u))
        {
            kind = xport_load_le32(parameters + 40);
            if (kind == 1)
                mode = 0;
            else if (kind == 5)
                mode = 0x200u;
            else if (kind == 7)
                mode = 0x300u;
        }
        value = race_audio_read_half(output + 10);
        race_audio_write_half(output + 10, (uint16_t)((value & 0x3Fu) | ((depth | mode) << 6)));
    }
    if (all || (flags & 0x4000u))
    {
        uint32_t rate = xport_load_le16(parameters + 54);
        uint32_t mode = 0;

        if (rate >= 32)
            rate = 31;
        if ((all || (flags & 0x400u)) && xport_load_le32(parameters + 44) == 7)
            mode = 0x20u;
        value = race_audio_read_half(output + 10);
        race_audio_write_half(output + 10, (uint16_t)((value & 0xFFC0u) | rate | mode));
    }
    if (all || (flags & 0x8000u))
    {
        uint32_t rate = xport_load_le16(parameters + 56);

        if (rate >= 16)
            rate = 15;
        value = race_audio_read_half(output + 8);
        race_audio_write_half(output + 8, (uint16_t)((value & 0xFFF0u) | rate));
    }
    return 0;
}

uint32_t sub_8001F874(int32_t offset, uint32_t handle)
{
    uint32_t record = r_u32(0x800D687Cu) + 44 * (handle >> 27);
    uint8_t parameters[64];
    uint32_t sample;
    uint32_t voice;
    uint32_t address;

    FUNCTION_MARKER(0x8001F874, "SLUS_010.53");
    if ((r_u32(record + 4) & 0x07FFFFFFu) != (handle & 0x07FFFFFFu))
        return 1;
    sample = r_u32(record + 8);
    voice = r_u32(record + 28);
    address = r_u32(sample + 8);
    xport_store_le32(parameters, 1u << (voice & 31u));
    xport_store_le32(parameters + 4, 0x00010000u);
    xport_store_le32(parameters + 32, address + (uint32_t)offset);
    voice = r_u32(record + 28);
    return sub_80051C38(voice, parameters);
}

uint32_t sub_80016768(uint32_t player_index, uint32_t first, uint32_t second, uint32_t third, uint32_t fourth, uint32_t phase)
{
    uint32_t record = r_u32(0x8005B40Cu) + 72 * player_index;
    uint32_t result = 2048u - phase;

    FUNCTION_MARKER(0x80016768, "SLUS_010.53");
    w_u32(record, result);
    w_u32(record + 4, first);
    w_u32(record + 8, second);
    w_u32(record + 12, third);
    w_u32(record + 16, fourth);
    return result;
}

uint32_t sub_800167A4(uint32_t player_index)
{
    uint32_t record = r_u32(0x8005B42Cu) + 132 * player_index;
    uint32_t actor;
    uint32_t level_out;
    uint32_t pan_out;
    uint8_t parameters[12];
    uint32_t first_handle;
    uint32_t second_handle;
    uint32_t result;
    int32_t level;
    int32_t pan;

    FUNCTION_MARKER(0x800167A4, "SLUS_010.53");
    result = r_u8(record + 4);
    if (result)
        return result;
    actor = r_u32(record);
    (void)sub_80019E40(player_index, ((int32_t)(uint32_t)(r_u32(actor + 184))), ((int32_t)(uint32_t)(r_u32(actor + 192))), 0, 0, &level_out, &pan_out, NULL, 0);
    level = race_audio_mul_asr(((int32_t)(uint32_t)(xport_load_le32(&level_out))), ((int32_t)(uint32_t)(r_u32(0x800D6C00u))), 7);
    pan = ((int32_t)(uint32_t)(xport_load_le32(&pan_out)));
    xport_store_le32(parameters, (uint32_t)race_audio_mul_asr(((int32_t)(uint32_t)(r_u32(record + 48))), ((int32_t)(uint32_t)(r_u32(record + 120))), 16));
    xport_store_le32(parameters + 4, (uint32_t)race_audio_mul_asr(level, ((int32_t)(uint32_t)(r_u32(record + 96))), 7));
    xport_store_le32(parameters + 8, (uint32_t)pan);
    first_handle = sub_8001F174(r_u32(record + 8), 0, 0, 1, parameters);
    w_u32(record + 16, first_handle);
    xport_store_le32(parameters + 4, 0);
    second_handle = sub_8001F174(r_u32(record + 8), 3, 0, 1, parameters);
    w_u32(record + 28, second_handle);
    (void)sub_8001F900(second_handle, 1);
    xport_store_le32(parameters, 128);
    first_handle = sub_8001F174(r_u32(record + 8), 2, 0, 1, parameters);
    w_u32(record + 32, first_handle);
    if ((r_u32(record + 28) >> 27) - (first_handle >> 27) != 1)
    {
        (void)sub_8001F7EC(first_handle);
        (void)sub_8001F7EC(r_u32(record + 28));
        (void)sub_8001F900(r_u32(record + 28), 0);
        w_u32(record + 32, UINT32_MAX);
        w_u32(record + 28, UINT32_MAX);
    }
    xport_store_le32(parameters, UINT32_MAX);
    xport_store_le32(parameters + 4, (uint32_t)race_audio_mul_asr(level, 127 - ((int32_t)(uint32_t)(r_u32(record + 96))), 7));
    result = sub_8001F174(r_u32(record + 8), 4, 0, 1, parameters);
    w_u32(record + 24, result);
    w_u32(record + 20, 0);
    w_u32(record + 56, 0);
    w_u32(record + 104, 0);
    w_u32(record + 124, 0);
    w_u32(record + 76, 0);
    w_u32(record + 36, 0);
    w_u32(record + 40, 0);
    w_u32(record + 84, UINT32_MAX);
    w_u32(record + 88, UINT32_MAX);
    w_u32(record + 92, 0);
    w_u8(record + 4, 1);
    w_u32(record + 52, r_u32(record + 48));
    w_u32(record + 100, r_u32(record + 96));
    w_u32(record + 112, (uint32_t)level);
    w_u32(record + 116, (uint32_t)pan);
    return result;
}

uint32_t sub_8001B244(uint32_t mode)
{
    uint32_t bank;
    uint32_t sample;
    uint32_t result;

    FUNCTION_MARKER(0x8001B244, "SLUS_010.53");
    if (mode == 4)
    {
        if (r_u32(0x800D6C58u) != 23)
            return 0x800D0000u;
        if (r_u32(0x800D6B68u) != 2)
            return 2;
        bank = r_u32(0x800D6B60u);
        sample = 0;
        w_u32(0x800D6BCCu, 6);
        w_u32(0x800D6B68u, 3);
        w_u32(0x800D6C58u, 4);
    }
    else
    {
        bank = r_u32(0x8005B3FCu);
        sample = mode;
        w_u32(0x800D6BCCu, UINT32_MAX);
    }
    if (mode == 3 && r_u32(0x800D6BD0u))
        return r_u32(0x800D6BD0u);
    result = 3;
    if (r_u32(0x800D6BC8u))
    {
        uint32_t handle = r_u32(0x800D6BC0u);

        if (handle)
            (void)sub_8001F7EC(handle);
        w_u32(0x800D6BC0u, sub_8001F174(bank, sample, 1, 1, NULL));
        w_u32(0x800D6BC8u, 0);
        w_u32(0x800D6BC4u, r_u32(r_u32(0x8005B2F8u) + 16) + 1500);
    }
    if (mode == 3)
    {
        w_u32(0x800D6BD0u, 1);
        w_u32(0x800D6BC8u, 1);
        result = r_u32(r_u32(0x8005B2F8u) + 16) + 9000;
        w_u32(0x800D6BD4u, result);
    }
    return result;
}

uint32_t sub_80027778(uint32_t actor, uint32_t kind, uint32_t duration, uint32_t marker)
{
    uint32_t context = r_u32(0x8005B2F8u);
    uint32_t split = (r_u8(context + 4) >> 4) & 1u;
    uint32_t limit = 20u >> split;
    uint32_t flags = r_u32(actor + 36);
    int32_t index;
    uint32_t record;
    uint32_t frame;
    uint32_t i;

    FUNCTION_MARKER(0x80027778, "SLUS_010.53");
    if (((flags >> 19) & 15u) >= limit)
        return limit;
    index = (int32_t)sub_80027178();
    if (index == -1)
        return UINT32_MAX;
    record = 0x800D39B0u + 112u * (uint32_t)index;
    frame = r_u32(context + 16);
    if (frame == r_u32(0x8005B360u))
        return r_u32(0x8005B360u);
    w_u32(record + 52, duration);
    w_u32(record, (r_u32(record) & 0xFFC0003Fu) | 0x5C0u | ((kind & 0xFFu) << 14));
    w_u32(0x8005B360u, frame);
    w_u8(record + 60, (uint8_t)marker);
    flags = (flags & 0xFF87FFFFu) | (((((flags >> 19) & 15u) + 1u) & 15u) << 19);
    w_u32(actor + 36, flags);
    (void)sub_800289E8(actor, 0x80053670u + 12u * kind, record + 20);
    w_u8(record + 108, 0);
    w_u8(record + 109, 1);
    w_u32(record + 96, 300u >> split);
    w_u32(record + 48, frame);
    w_u32(record + 100, frame);
    for (i = 0; i < 4; ++i)
        w_u8(record + 104 + i, (uint8_t)(7u + i));
    (void)sub_800270F0(record);
    w_u8(record + 61, kind != 1u);
    w_u32(record + 64, 3);
    w_u32(record + 36, 0);
    w_u32(record + 40, 0);
    w_u32(record + 44, 0);
    w_u16(record + 62, 30);
    return sub_800271CC(actor, (uint32_t)index);
}

uint32_t sub_80027974(uint32_t actor, uint32_t unused, uint32_t kind)
{
    uint32_t context = r_u32(0x8005B2F8u);
    uint32_t flags = r_u32(actor + 36);
    int32_t height = (int16_t)r_u16(actor + 482);
    int32_t index;
    uint32_t record;
    uint32_t frame;
    int32_t duration;

    FUNCTION_MARKER(0x80027974, "SLUS_010.53");
    if (r_u16(actor + 172) >= r_u32(context + 48))
        return r_u32(context + 48);
    if ((flags >> 5) & 3u)
        return (flags >> 5) & 3u;
    if (!kind)
    {
        if (((flags >> 19) & 15u) >= 4u)
            return 0;
        if ((flags >> 23) & 3u)
            return (flags >> 23) & 3u;
    }
    index = (int32_t)sub_80027178();
    if (index == -1)
        return UINT32_MAX;
    frame = r_u32(context + 16);
    if (frame == r_u32(0x8005B360u))
        return r_u32(0x8005B360u);
    w_u32(0x8005B360u, frame);
    record = 0x800D39B0u + 112u * (uint32_t)index;
    w_u32(record, (r_u32(record) & 0xFFC0003Fu) | 0x480u | ((kind & 0xFFu) << 14));
    if (kind)
    {
        flags = (flags & 0xFE7FFFFFu) | (((((flags >> 23) & 3u) + 1u) & 3u) << 23);
        w_u16(record + 62, 35);
    }
    else
    {
        flags = (flags & 0xFF87FFFFu) | (((((flags >> 19) & 15u) + 1u) & 15u) << 19);
        w_u16(record + 62, 30);
    }
    w_u32(actor + 36, flags);
    duration = 150 - 2 * height;
    if (duration < 0)
        duration = 0;
    if (duration > 0x960000)
        duration = 0x960000;
    w_u32(record + 52, (uint32_t)duration);
    w_u32(record + 48, frame);
    (void)sub_8002705C(record, (uint32_t)height);
    w_u8(record + 61, 1);
    w_u32(record + 36, 0);
    w_u32(record + 40, 0);
    w_u32(record + 44, 0);
    w_u32(record + 64, 9);
    return sub_800271CC(actor, (uint32_t)index);
}

uint32_t sub_80027258(uint32_t actor, uint32_t kind)
{
    uint32_t context = r_u32(0x8005B2F8u);
    uint32_t flags = r_u32(actor + 36);
    int32_t height = (int16_t)r_u16(actor + 482);
    int32_t index;
    uint32_t record;
    int32_t duration;

    FUNCTION_MARKER(0x80027258, "SLUS_010.53");
    if (r_u16(actor + 172) >= r_u32(context + 48))
        return r_u32(context + 48);
    if (((flags >> 25) & 3u) >= 2u)
        return ((flags >> 25) & 3u) < 2u;
    index = (int32_t)sub_80027178();
    if (index == -1)
        return UINT32_MAX;
    record = 0x800D39B0u + 112u * (uint32_t)index;
    w_u32(record, (r_u32(record) & 0xFFC03C3Fu) | 0x40u | ((kind & 0xFFu) << 14));
    flags = (flags & 0xF9FFFFFFu) | (((((flags >> 25) & 3u) + 1u) & 3u) << 25);
    w_u32(actor + 36, flags);
    (void)sub_800289E8(actor, 0x80053670u + 12u * kind, record + 20);
    duration = 150 - 2 * height;
    if (duration < 0)
        duration = 0;
    if (duration > 9830400)
        duration = 9830400;
    w_u32(record + 52, (uint32_t)duration);
    w_u32(record + 48, r_u32(context + 16));
    (void)sub_8002705C(record, (uint32_t)height);
    w_u32(record + 36, 0);
    w_u32(record + 40, 0);
    w_u32(record + 44, 0);
    w_u8(record + 61, 0);
    w_u16(record + 62, 30);
    return sub_800271CC(actor, (uint32_t)index);
}

uint32_t sub_800179D8(uint32_t slot, uint32_t listener)
{
    uint32_t record = r_u32(0x8005B410u + 4u * listener) + 44u * slot;
    uint32_t level_out;
    uint32_t pan_out;
    uint32_t ratio_out = 0x1F8003B8u;
    /* TODO: Resolve untouched ratio on the no-table path before moving its storage */
    uint8_t parameters[12];
    uint32_t kind = r_u32(record + 8);
    uint32_t mode;
    uint32_t shift = kind == 3u || kind == 4u || kind == 6u ? 2u : 0u;
    int32_t x;
    int32_t z;
    int32_t level;
    uint32_t position_x, position_z, velocity_x, velocity_z, pitch, ratio;

    FUNCTION_MARKER(0x800179D8, "SLUS_010.53");
    position_x = r_u32(record + 12);
    velocity_x = r_u32(record + 20);
    x = ((int32_t)(uint32_t)(position_x + (uint32_t)(((int32_t)(uint32_t)(velocity_x)) >> 6)));
    velocity_z = r_u32(record + 24);
    position_z = r_u32(record + 16);
    w_u32(record + 12, (uint32_t)x);
    z = ((int32_t)(uint32_t)(position_z + (uint32_t)(((int32_t)(uint32_t)(velocity_z)) >> 6)));
    w_u32(record + 16, (uint32_t)z);
    velocity_z = r_u32(record + 24);
    velocity_x = r_u32(record + 20);
    (void)sub_80019E40(listener, x, z, ((int32_t)(uint32_t)(velocity_x)), ((int32_t)(uint32_t)(velocity_z)), &level_out, &pan_out, xport_guest_ptr(ratio_out, 4), shift);
    kind = r_u32(record + 8);
    if (kind >= 3u && kind <= 5u)
        mode = 2;
    else if (kind == 2u)
        mode = 4;
    else
        mode = 1;
    level = race_audio_mul_asr(((int32_t)(uint32_t)(xport_load_le32(&level_out))), ((int32_t)(uint32_t)(r_u32(0x800D6C00u + 4u * mode))), 7);
    if (r_u32(0x8005B434u))
        level = 0;
    xport_store_le32(&level_out, (uint32_t)level);
    level = ((int32_t)(uint32_t)(xport_load_le32(&level_out)));
    velocity_z = xport_load_le32(&pan_out);
    xport_store_le32(parameters + 4, (uint32_t)level);
    xport_store_le32(parameters + 8, velocity_z);
    pitch = r_u32(record + 28);
    ratio = r_u32(ratio_out);
    xport_store_le32(parameters, (uint32_t)race_audio_mul_asr(((int32_t)(uint32_t)(pitch)), ((int32_t)(uint32_t)(ratio)), 16));
    return sub_8001F6A4(r_u32(record + 32), parameters);
}

uint32_t sub_800169D0(uint32_t player_index)
{
    uint32_t record = r_u32(0x8005B42Cu) + 132u * player_index;
    uint32_t actor = r_u32(record);
    uint32_t body = r_u32(actor + 852);
    uint32_t level_out;
    uint32_t pan_out;
    uint32_t ratio_out = 0x1F8003A8u;
    /* TODO: Resolve untouched ratio on the no-table path before moving its storage */
    uint8_t parameters[12];
    uint32_t state = r_u32(body + 604);
    int32_t engine;
    int32_t effect;
    int32_t level;
    int32_t pan;
    int32_t ratio;
    int32_t gear;
    int32_t previous_gear;
    int32_t limit;
    uint32_t handle;
    uint32_t table;
    uint32_t mode;

    FUNCTION_MARKER(0x800169D0, "SLUS_010.53");
    engine = (int32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(actor + 604))) >> 4, ((int32_t)(uint32_t)(r_u32(record + 60))));
    if (state >= 3u || !r_u16(body + 544))
        engine = 0;
    effect = 0;
    if (state < 2u)
    {
        int32_t value = (int32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(actor + 588))), ((int32_t)(uint32_t)(r_u32(0x800D6BD8u + 4u * (uint32_t)(int32_t)r_s8(actor + 849))))) >> 9;

        effect = value < 127 ? value : 127;
    }
    (void)sub_80019E40(player_index, ((int32_t)(uint32_t)(r_u32(actor + 184))), ((int32_t)(uint32_t)(r_u32(actor + 192))), ((int32_t)(uint32_t)(r_u32(actor + 456))), ((int32_t)(uint32_t)(r_u32(actor + 464))), &level_out, &pan_out, xport_guest_ptr(ratio_out, 4), 0);
    level = race_audio_mul_asr(((int32_t)(uint32_t)(xport_load_le32(&level_out))), ((int32_t)(uint32_t)(r_u32(0x800D6C00u))), 7);
    pan = ((int32_t)(uint32_t)(xport_load_le32(&pan_out)));
    ratio = ((int32_t)(uint32_t)(r_u32(ratio_out)));
    w_u32(record + 112, (uint32_t)level);
    w_u32(record + 116, (uint32_t)pan);
    w_u32(record + 120, (uint32_t)ratio);
    if (engine < 896)
    {
        engine = 896;
        if (!r_u32(record + 24))
        {
            xport_store_le32(parameters, (uint32_t)race_audio_mul_asr(ratio, (int32_t)sub_8001F934(((int32_t)(uint32_t)(r_u32(record + 8))), 4, 0), 16));
            xport_store_le32(parameters + 4, (uint32_t)race_audio_mul_asr(level, 127 - ((int32_t)(uint32_t)(r_u32(record + 96))), 7));
            xport_store_le32(parameters + 8, (uint32_t)pan);
            handle = sub_8001F174(r_u32(record + 8), 4, 0, 1, parameters);
            w_u32(record + 24, handle);
            (void)sub_8001F7EC(r_u32(record + 20));
            w_u32(record + 20, 0);
        }
    }
    else
    {
        if (!r_u32(record + 20))
        {
            xport_store_le32(parameters, (uint32_t)race_audio_mul_asr(((int32_t)(uint32_t)(r_u32(record + 48))), ratio, 16));
            xport_store_le32(parameters + 4, (uint32_t)race_audio_mul_asr(level, 127 - ((int32_t)(uint32_t)(r_u32(record + 96))), 7));
            xport_store_le32(parameters + 8, (uint32_t)pan);
            handle = sub_8001F174(r_u32(record + 8), 1, 0, 1, parameters);
            w_u32(record + 20, handle);
            (void)sub_8001F7EC(r_u32(record + 24));
            w_u32(record + 24, 0);
        }
        if ((uint32_t)(r_u8(actor + 534) - 1u) >= 2u)
        {
            engine += (101 * (int32_t)(sub_80043F00(0xF2000002u) & 0xFFu)) >> 8;
            effect += (11 * (int32_t)(sub_80043F00(0xF2000002u) & 0xFFu)) >> 8;
        }
    }
    table = r_u32(record + 44);
    if (((int32_t)(uint32_t)(r_u32(actor + 588))) < (((int32_t)(uint32_t)(r_u32(actor + 592))) >> 1) && !(r_u32(actor + 564) & 0x20u))
    {
        w_u32(record + 64, r_u32(table + 8));
        mode = 0;
    }
    else
    {
        w_u32(record + 64, r_u32(table + 4));
        mode = 1;
    }
    (void)sub_8001F874((int32_t)(mode << 7), r_u32(record + 32));
    mode = 0;
    if (r_u32(actor + 564) & 0x200u)
        mode = 4;
    else if (r_u32(actor + 564) & 0x20u)
        mode = 2;
    (void)sub_8001F874((int32_t)(mode << 7), r_u32(record + 28));
    previous_gear = ((int32_t)(uint32_t)(r_u32(record + 108)));
    gear = (int8_t)r_u8(actor + 849);
    if (previous_gear < gear)
        engine += 512;
    else if (previous_gear > gear)
    {
        engine -= 512;
        effect += 96;
        if (effect >= 127)
            effect = 126;
    }
    w_u32(record + 108, (uint32_t)gear);
    limit = ((int32_t)(uint32_t)(r_u32(table)));
    if (engine > limit)
        engine = limit;
    if (!r_u8(record + 4))
        return sub_800167A4(player_index);
    w_u32(record + 52, (uint32_t)engine);
    w_u32(record + 100, (uint32_t)effect);
    w_u32(record + 56, (uint32_t)((engine - ((int32_t)(uint32_t)(r_u32(record + 48)))) >> 1));
    effect = (effect - ((int32_t)(uint32_t)(r_u32(record + 96)))) >> 1;
    w_u32(record + 104, (uint32_t)effect);
    return (uint32_t)effect;
}

uint32_t sub_80016E4C(uint32_t player_index)
{
    int32_t scalar_value_12;
    int32_t scalar_value_14;
    int32_t scalar_value_15;

    int32_t scalar_sign_1;
    int32_t scalar_value_2;
    int32_t scalar_sign_2;
    int32_t scalar_value_3;
    int32_t scalar_sign_3;
    int32_t scalar_value_4;
    int32_t scalar_sign_4;
    int32_t scalar_value_5;
    int32_t scalar_sign_5;
    int32_t scalar_value_6;
    int32_t scalar_sign_6;
    int32_t scalar_value_7;
    int32_t scalar_sign_7;

    uint32_t record = r_u32(0x8005B42Cu) + 132u * player_index;
    uint32_t actor = r_u32(record);
    uint32_t body = r_u32(actor + 852);
    uint8_t parameters[12];
    int32_t primary_volume = 0;
    int32_t secondary_volume = 0;
    int32_t primary_sample = -1;
    int32_t secondary_sample = -1;
    int32_t active = ((int32_t)(uint32_t)(r_u32(record + 124)));
    uint32_t unusual = (uint32_t)(r_u8(actor + 534) - 1u) >= 2u;
    int32_t level;
    int32_t pan = ((int32_t)(uint32_t)(r_u32(record + 116)));
    uint32_t handle;
    uint32_t result = 0;

    FUNCTION_MARKER(0x80016E4C, "SLUS_010.53");
    if (active > 0)
    {
        if (active & 1)
        {
            int32_t speed = ((int32_t)(uint32_t)(r_u32(actor + 480)));
            int32_t delta = ((int32_t)(uint32_t)(r_u32(actor + 188))) - ((int32_t)(uint32_t)(r_u32(actor + 508)));

            primary_volume = speed > 0x7FFFF ? 127 : speed >> 12;
            if (((scalar_sign_1 = ((int32_t)(delta)) >> 31), (((int32_t)((((int32_t)(delta)) + scalar_sign_1) ^ scalar_sign_1)))) >= 0x4000)
                primary_sample = -1;
            else if (unusual)
                primary_sample = ((scalar_value_2 = (((int32_t)(uint32_t)(r_u32(actor + 652))))), (scalar_sign_2 = scalar_value_2 >> 31), (((int32_t)((scalar_value_2 + scalar_sign_2) ^ scalar_sign_2)))) >= 0x4001 ? 18 : 16;
            else
                primary_sample = ((scalar_value_3 = (((int32_t)(uint32_t)(r_u32(actor + 652))))), (scalar_sign_3 = scalar_value_3 >> 31), (((int32_t)((scalar_value_3 + scalar_sign_3) ^ scalar_sign_3)))) >= 0x4001 ? 17 : 12;
            if (speed <= 0x7FFF)
            {
                primary_sample = -1;
                active &= ~1;
                w_u32(record + 124, (uint32_t)active);
            }
        }
        if (active & 2)
        {
            int32_t speed = ((int32_t)(uint32_t)(r_u32(body + 480)));
            int32_t alternate = (r_u32(actor + 388) & 1u) || (int8_t)r_u8(actor + 534) >= 3;

            secondary_volume = speed > 0x7FFFF ? 127 : speed >> 12;
            secondary_sample = 19 + alternate;
            if ((r_u32(body + 552) & 0x40000000u) && ((scalar_value_4 = (((int32_t)(uint32_t)(r_u32(body + 188))) - ((int32_t)(uint32_t)(r_u32(body + 508))))), (scalar_sign_4 = scalar_value_4 >> 31), (((int32_t)((scalar_value_4 + scalar_sign_4) ^ scalar_sign_4)))) >= 0x4000)
                secondary_sample = -1;
            if (((int32_t)(uint32_t)(r_u32(record + 88))) < 0 && secondary_sample >= 0)
                (void)sub_80017BA0(((int32_t)(uint32_t)(r_u32(body + 184))), ((int32_t)(uint32_t)(r_u32(body + 192))), r_u8(actor + 534) == 4 ? 102 : 55, 0);
            if (speed <= 0x7FFF)
            {
                secondary_sample = -1;
                active &= ~2;
                w_u32(record + 124, (uint32_t)active);
            }
        }
    }
    else if (!(r_u32(actor + 568) & 0x400u))
    {
        uint32_t collision = (r_u32(actor + 36) >> 25) & 3u;
        int32_t speed = ((int32_t)(uint32_t)(r_u32(actor + 700)));

        if (collision || r_u32(record + 92))
        {
            primary_sample = 17;
            if (collision)
                w_u32(record + 92, 4);
            else
                w_u32(record + 92, r_u32(record + 92) - 1u);
            primary_volume = ((scalar_value_12 = (32 * ((int32_t)(uint32_t)(r_u32(record + 92))))), (((int32_t)((scalar_value_12 < 0) ? (0) : (scalar_value_12 < 128 ? scalar_value_12 : 127)))));
        }
        if (speed > 73727)
        {
            primary_volume = speed >> 10;
            if (((scalar_value_5 = (((int32_t)(uint32_t)(r_u32(actor + 652))))), (scalar_sign_5 = scalar_value_5 >> 31), (((int32_t)((scalar_value_5 + scalar_sign_5) ^ scalar_sign_5)))) < 16385)
            {
                primary_sample = 13;
                if (primary_volume >= 128)
                    primary_volume = 127;
            }
            else
            {
                int32_t value = ((int32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(record + 68))), ((scalar_value_6 = (((int32_t)(uint32_t)(r_u32(actor + 676))))), (scalar_sign_6 = scalar_value_6 >> 31), (((int32_t)((scalar_value_6 + scalar_sign_6) ^ scalar_sign_6))))) >> 9) - 32 + (int32_t)(sub_8001FC58() & 63u);
                int32_t threshold = ((scalar_value_7 = (((int32_t)(uint32_t)(r_u32(actor + 676))))), (scalar_sign_7 = scalar_value_7 >> 31), (((int32_t)((scalar_value_7 + scalar_sign_7) ^ scalar_sign_7)))) - (((int32_t)(uint32_t)(r_u32(r_u32(actor + 556) + 204))) >> 1);

                primary_sample = 12;
                primary_volume = ((int32_t)((((int32_t)(value)) < 0) ? (0) : (((int32_t)(value)) < 128 ? ((int32_t)(value)) : 127)));
                if (primary_volume >= 40)
                    w_u32(record + 76, 0);
                else
                {
                    uint32_t count = r_u32(record + 76);

                    w_u32(record + 76, count + 1u);
                    if (count >= 8u)
                    {
                        w_u32(record + 76, 0);
                        primary_sample = -1;
                    }
                }
                if (threshold < 0)
                    threshold = 0;
                secondary_sample = 14;
                secondary_volume = ((scalar_value_14 = (((int32_t)sub_8001FC90(((int32_t)(uint32_t)(r_u32(record + 72))), threshold) >> 9) - 32 + (int32_t)(sub_8001FC58() & 63u))), (((int32_t)((scalar_value_14 < 0) ? (0) : (scalar_value_14 < 128 ? scalar_value_14 : 127)))));
                w_u32(record + 48, r_u32(record + 48) + (uint32_t)secondary_volume);
                w_u32(record + 52, r_u32(record + 52) + (uint32_t)secondary_volume);
            }
        }
        else if (((int32_t)(uint32_t)(r_u32(actor + 696))) > 51199)
        {
            primary_sample = 14;
            primary_volume = ((scalar_value_15 = ((((int32_t)(uint32_t)(r_u32(actor + 696))) - 51200) >> 7)), (((int32_t)((scalar_value_15 < 0) ? (0) : (scalar_value_15 < 128 ? scalar_value_15 : 127)))));
        }
        if (unusual)
        {
            if (!r_u32(record + 92))
            {
                if (primary_sample != 17)
                {
                    primary_sample = 16;
                    if (secondary_volume > primary_volume)
                        primary_volume = secondary_volume;
                }
            }
            secondary_sample = 15;
            secondary_volume = ((int32_t)(uint32_t)(r_u32(actor + 480))) > 0xFFFFF ? 127 : ((int32_t)(uint32_t)(r_u32(actor + 480))) >> 14;
        }
        if ((uint32_t)(r_u32(body + 604) - 1u) >= 3u)
            primary_volume = 0;
    }

    level = race_audio_mul_asr(((int32_t)(uint32_t)(r_u32(record + 112))), ((int32_t)(uint32_t)(r_u32(0x800D6C0Cu))), 7);
    if (r_u32(0x8005B434u))
        level = 0;
    xport_store_le32(parameters, (uint32_t)race_audio_mul_asr(((int32_t)(uint32_t)(r_u32(record + 120))), (int32_t)sub_8001F934(((int32_t)(uint32_t)(r_u32(0x8005B420u))), (uint32_t)primary_sample, 0), 16));
    xport_store_le32(parameters + 4, (uint32_t)race_audio_mul_asr(level, primary_volume, 7));
    xport_store_le32(parameters + 8, (uint32_t)pan);
    if (primary_sample == ((int32_t)(uint32_t)(r_u32(record + 84))))
    {
        handle = r_u32(record + 36);
        if (handle)
            result = sub_8001F6A4(handle, parameters);
    }
    else
    {
        handle = r_u32(record + 36);
        w_u32(record + 84, (uint32_t)primary_sample);
        if (handle)
            (void)sub_8001F7EC(handle);
        if (primary_sample >= 0)
            w_u32(record + 36, sub_8001F174(r_u32(0x8005B420u), (uint32_t)primary_sample, 0, 1, parameters));
    }
    if (active)
    {
        uint32_t level_out;
        uint32_t pan_out;

        (void)sub_80019E40(player_index, ((int32_t)(uint32_t)(r_u32(body + 184))), ((int32_t)(uint32_t)(r_u32(body + 192))), 0, 0, &level_out, &pan_out, NULL, 0);
        level = race_audio_mul_asr(((int32_t)(uint32_t)(xport_load_le32(&level_out))), ((int32_t)(uint32_t)(r_u32(0x800D6C0Cu))), 7);
        if (r_u32(0x8005B434u))
            level = 0;
        pan = ((int32_t)(uint32_t)(xport_load_le32(&pan_out)));
    }
    xport_store_le32(parameters, (uint32_t)race_audio_mul_asr(((int32_t)(uint32_t)(r_u32(record + 120))), (int32_t)sub_8001F934(((int32_t)(uint32_t)(r_u32(0x8005B420u))), (uint32_t)secondary_sample, 0), 16));
    xport_store_le32(parameters + 4, (uint32_t)race_audio_mul_asr(level, secondary_volume, 7));
    xport_store_le32(parameters + 8, (uint32_t)pan);
    if (secondary_sample == ((int32_t)(uint32_t)(r_u32(record + 88))))
    {
        handle = r_u32(record + 40);
        if (handle)
            result = sub_8001F6A4(handle, parameters);
    }
    else
    {
        handle = r_u32(record + 40);
        w_u32(record + 88, (uint32_t)secondary_sample);
        if (handle)
            (void)sub_8001F7EC(handle);
        if (secondary_sample >= 0)
            w_u32(record + 40, sub_8001F174(r_u32(0x8005B420u), (uint32_t)secondary_sample, 0, 1, parameters));
    }
    if (r_u32(actor + 564) & 0x18000000u)
        result = sub_80027778(actor, 1, 600, 0);
    else if ((r_u32(actor + 388) & 1u) && (int16_t)r_u16(actor + 482) >= 13)
        result = sub_80027974(actor, actor + 184, 0);
    else if (((int32_t)(uint32_t)(r_u32(actor + 700))) > 73727 && (r_u32(actor + 564) & 8u))
        result = sub_80027974(actor, actor + 184, 0);
    if (((int32_t)(uint32_t)(r_u32(actor + 696))) > 73727 && (r_u32(actor + 564) & 4u))
        result = sub_80027974(actor, actor + 184, 2);
    return result;
}

static uint32_t race_audio_candidate_actor(uint16_t id)
{
    uint32_t type = id >> 5;
    uint32_t index = id & 31u;

    if (type == 5u && index >= 29u)
        return r_u32(0x8005B304u) + 280u * (index - 29u);
    return r_u32(0x800CE4D0u + 16u * type) + r_u32(0x800CE4D4u + 16u * type) * index;
}

uint32_t sub_800184AC(uint32_t player_index)
{
    int32_t scalar_sign_8;
    int32_t scalar_sign_9;
    int32_t scalar_value_10;
    int32_t scalar_sign_10;

    uint32_t candidates = 0x1F800280u;
    uint32_t count_io = candidates + 32;
    uint32_t listener = r_u32(0x8005B40Cu) + 72u * player_index;
    uint32_t records = r_u32(0x8005B410u + 4u * player_index);
    uint32_t capacity = r_u32(listener + 36);
    int32_t selected[16];
    int32_t distance[16];
    uint32_t count;
    uint32_t i;
    uint32_t j;

    FUNCTION_MARKER(0x800184AC, "SLUS_010.53");
    if (capacity > 16u)
        capacity = 16u;
    w_u32(count_io, 10);
    (void)sub_8008AE94(candidates, count_io, 0x800CD950u + 1132u * player_index, 0x800000, 9, r_u16(r_u32(r_u32(0x8005B42Cu) + 132u * player_index) + 172));
    count = r_u32(count_io);
    if (r_u32(0x8005B314u))
    {
        uint32_t extra = r_u32(0x8005B304u);

        for (i = 0; i < 3 && count < 16u; ++i)
        {
            uint16_t id = r_u16(extra + 280u * i + 172);

            if (id && r_u8(extra + 280u * i + 8) != 9)
            {
                w_u16(candidates + 2u * count, id);
                ++count;
            }
        }
        w_u32(count_io, count);
    }
    if (count > 16u)
        count = 16u;
    for (i = 0; i < 16u; ++i)
    {
        selected[i] = -1;
        distance[i] = INT32_MAX;
    }
    for (i = 0; i < count; ++i)
    {
        uint16_t id = r_u16(candidates + 2u * i);
        uint32_t candidate = race_audio_candidate_actor(id);
        int32_t dx = ((int32_t)(uint32_t)(r_u32(candidate + 184))) - ((int32_t)(uint32_t)(r_u32(listener + 4)));
        int32_t dz = ((int32_t)(uint32_t)(r_u32(candidate + 192))) - ((int32_t)(uint32_t)(r_u32(listener + 8)));
        int32_t high = ((scalar_sign_8 = ((int32_t)(dx)) >> 31), (((int32_t)((((int32_t)(dx)) + scalar_sign_8) ^ scalar_sign_8))));
        int32_t low = ((scalar_sign_9 = ((int32_t)(dz)) >> 31), (((int32_t)((((int32_t)(dz)) + scalar_sign_9) ^ scalar_sign_9))));
        int32_t mixed;
        int32_t metric;

        if (high < low)
        {
            int32_t swap = high;

            high = low;
            low = swap;
        }
        mixed = low + (low >> 1);
        metric = high - (high >> 5) - (high >> 7) + (mixed >> 2) + (mixed >> 6);
        distance[i] = metric;
        if ((r_u8(r_u32(0x8005B2F8u) + 4) & 1u) && r_u8(r_u32(0x8005B2F8u) + 4) != 33)
        {
            uint32_t local = r_u32(0x8005B268u + 4u * player_index);

            if ((r_u8(r_u32(local + 1084) + 1) & 15u) == 2u && r_u16(candidate + 172) == r_u8(r_u32(0x8005B2F8u) + 6) && ((scalar_value_10 = (((int32_t)(uint32_t)(r_u32(candidate + 324))) - ((int32_t)(uint32_t)(r_u32(local + 324))))), (scalar_sign_10 = scalar_value_10 >> 31), (((int32_t)((scalar_value_10 + scalar_sign_10) ^ scalar_sign_10)))) <= 204799)
                (void)sub_8001B244(3);
        }
        for (j = 0; j < capacity; ++j)
        {
            if (selected[j] < 0 || metric < distance[selected[j]])
            {
                uint32_t move = capacity - 1u;

                while (move > j)
                {
                    selected[move] = selected[move - 1u];
                    --move;
                }
                selected[j] = (int32_t)i;
                break;
            }
        }
    }
    for (i = 0; i < capacity; ++i)
    {
        uint16_t id = r_u16(records + 44u * i);
        uint32_t found = id == 224;

        for (j = 0; !found && j < capacity; ++j)
            if (selected[j] >= 0 && id == r_u16(candidates + 2u * (uint32_t)selected[j]))
                found = 1;
        if (!found)
            (void)sub_80017814(i, player_index);
    }
    for (i = 0; i < capacity; ++i)
    {
        uint32_t found = selected[i] < 0;
        uint16_t id;

        if (found)
            continue;
        id = r_u16(candidates + 2u * (uint32_t)selected[i]);
        for (j = 0; j < count; ++j)
            if (r_u16(records + 44u * j) == id)
                found = 1;
        if (!found)
        {
            for (j = 0; j < capacity; ++j)
            {
                uint32_t record = records + 44u * j;

                if (r_u16(record) == 224)
                {
                    w_u16(record, id);
                    w_u32(record + 8, 1);
                    break;
                }
            }
        }
    }
    for (i = 0; i < capacity; ++i)
    {
        uint16_t id = r_u16(records + 44u * i);

        if (id == 224)
            continue;
        for (j = i + 1u; j < capacity; ++j)
            if (id == r_u16(records + 44u * j))
                (void)sub_80017814(j, player_index);
    }
    return i < capacity;
}

uint32_t sub_80018FAC(void)
{
    uint32_t changed = 0;
    uint32_t player;

    FUNCTION_MARKER(0x80018FAC, "SLUS_010.53");
    if (r_u32(0x8005B42C))
    {
        if (!r_u32(0x8005B40C))
            goto audio_tail;
        for (player = 0; player < r_u32(r_u32(0x8005B2F8) + 48); ++player)
        {
            uint32_t actor = r_u32(r_u32(0x8005B42C) + 132 * player);
            uint32_t camera = 0x800CD898 + 1132 * player;
            uint32_t context = r_u32(0x8005B2F8);
            uint32_t body;
            uint32_t listener;
            uint32_t state_flags;
            uint32_t steering_mode;
            uint32_t timer_index;

            if (r_u32(camera + 552) == 2 && (r_u8(context + 4) & 1))
                w_u32(0x800D6C04, 0);
            if (r_u32(camera + 772) || r_u32(r_u32(actor + 852) + 604) >= 3)
            {
                int32_t speed = (int32_t)r_u32(camera + 480) >> 4;
                int32_t velocity_z = race_audio_mul_asr(r_s16(camera + 454), speed, 8);
                int32_t velocity_x = race_audio_mul_asr(r_s16(camera + 450), speed, 8);
                (void)sub_80016768(player, r_u32(camera + 184), r_u32(camera + 192), (uint32_t)velocity_x, (uint32_t)velocity_z, (uint32_t)(int32_t)r_s16(r_u32(0x8005AEC0) + 124));
            }
            else
                (void)sub_80016768(player, r_u32(actor + 184), r_u32(actor + 192), r_u32(actor + 456), r_u32(actor + 464), (uint32_t)(int32_t)r_s16(r_u32(0x8005AEC0) + 124));
            body = r_u32(actor + 852);
            state_flags = r_u32((r_u32(body + 604) >= 3 ? body : actor) + 36);
            steering_mode = ((state_flags >> 5) & 3) == 1 ? 2 : 0;
            listener = r_u32(0x8005B40C) + 72 * player;
            if (steering_mode != r_u32(listener + 24) && !r_u32(0x8005B434))
            {
                int32_t previous;
                uint32_t step;
                uint32_t phase;

                w_u32(listener + 24, 1);
                previous = (int32_t)r_u32(listener + 28);
                if (previous >= 0)
                {
                    int32_t difference = (int32_t)((uint32_t)previous - (uint32_t)(int32_t)r_s16(actor + 370));
                    uint32_t sign = (uint32_t)(difference >> 31);
                    step = (((uint32_t)difference + sign) ^ sign) << 11;
                }
                else
                    step = 2048;
                if (!steering_mode)
                    step = 0 - step;
                changed = 1;
                listener = r_u32(0x8005B40C) + 72 * player;
                phase = r_u32(listener + 32) + step;
                w_u32(listener + 32, phase);
                w_u32(listener + 28, (uint32_t)(int32_t)r_s16(actor + 370));
                if (phase >= 0x5000)
                {
                    w_u32(listener + 24, steering_mode);
                    w_u32(listener + 32, steering_mode ? 20479 : 0);
                    w_u32(listener + 28, UINT32_MAX);
                }
            }
            (void)sub_800169D0(player);
            (void)sub_80016E4C(player);
            for (timer_index = 0; timer_index < 2; ++timer_index)
            {
                uint32_t timer = r_u32(0x8005B40C) + 72 * player + 4 * timer_index;
                if (r_u32(timer + 56))
                {
                    int32_t remaining = (int32_t)(r_u32(timer + 64) - 1);
                    w_u32(timer + 64, (uint32_t)remaining);
                    if (remaining <= 0)
                    {
                        uint32_t sample = r_u32(timer + 48);
                        uint32_t source = r_u32(timer + 56);
                        w_u32(timer + 64, 0);
                        (void)sub_80017BA0((int32_t)r_u32(source + 184), (int32_t)r_u32(source + 192), sample, 0);
                        timer = r_u32(0x8005B40C) + 72 * player + 4 * timer_index;
                        w_u32(timer + 56, 0);
                        w_u32(timer + 48, 0);
                    }
                }
            }
        }
        if (changed)
        {
            uint32_t listeners = r_u32(0x8005B40C);
            uint32_t second = r_u32(r_u32(0x8005B2F8) + 48) == 1 ? r_u32(listeners + 32) : r_u32(listeners + 104);
            (void)sub_8001F054(r_u32(listeners + 32), second);
        }
    }
    if (r_u32(0x8005B40C))
    {
        uint32_t counter = r_u32(0x8005B430);
        w_u32(0x8005B430, counter + 1);
        if (!(counter & 3))
        {
            uint32_t selected = ((counter + 1) & 4) >> 2;
            if (!selected || r_u32(r_u32(0x8005B2F8) + 48) == 2)
                (void)sub_800184AC(selected);
        }
        for (player = 0; player < r_u32(r_u32(0x8005B2F8) + 48); ++player)
        {
            uint32_t listener = r_u32(0x8005B40C) + 72 * player;
            int32_t count = (int32_t)(r_u32(listener + 36) + r_u32(listener + 40));
            uint32_t slot = 0;
            if (count <= 0)
                continue;
            do
            {
                uint32_t record = r_u32(0x8005B410 + 4 * player) + 44 * slot;
                uint16_t id = r_u16(record);
                uint32_t actor;
                uint32_t substituted = 0;
                uint32_t direction;
                uint32_t kind;
                int32_t speed;

                if (id == 224)
                    goto next_audio_record;
                actor = race_audio_candidate_actor(id);
                if (r_u32(record + 8) == 2)
                {
                    uint32_t body = r_u32(actor + 852);
                    if (r_u32(body + 604) >= 3)
                    {
                        actor = body;
                        substituted = 1;
                    }
                }
                w_u32(record + 12, r_u32(actor + 184));
                id = r_u16(record);
                w_u32(record + 16, r_u32(actor + 192));
                if ((id >> 5) == 5 && (id & 31) >= 29)
                {
                    direction = actor + 214;
                    speed = (int32_t)r_u32(actor + 220) >> 4;
                    w_u32(record + 8, 6);
                }
                else
                {
                    direction = actor + 450;
                    speed = (int32_t)r_u32(actor + 480) >> 4;
                }
                w_u32(record + 20, (uint32_t)race_audio_mul_asr(r_s16(direction), speed, 8));
                kind = r_u32(record + 8);
                w_u32(record + 24, (uint32_t)race_audio_mul_asr(r_s16(direction + 4), speed, 8));
                if (kind == 1)
                {
                    int32_t target;
                    if (r_u16(actor + 172) >> 5)
                        target = (int32_t)r_u32(actor + 480) >> 10;
                    else
                    {
                        int32_t value = (int32_t)r_u32(actor + 604);
                        target = (int32_t)((uint32_t)(value >> 16) + (uint32_t)(value >> 17));
                    }
                    target = (int32_t)((uint32_t)target + 384);
                    if (target < 704)
                        target = 704;
                    if (!r_u32(record + 4))
                    {
                        uint32_t sample;
                        uint32_t type = r_u32(actor + 180);
                        if (r_u16(actor + 172) >> 5)
                            sample = (uint32_t)(int32_t)r_s8(0x80052630 + type % 6);
                        else if (type >= 18)
                            sample = 4;
                        else
                        {
                            uint32_t alternate = (r_u8(r_u32(actor + 1084) + 1) >> 4) & 1;
                            sample = type >= 9 ? alternate + 2 : alternate;
                        }
                        w_u32(record + 28, (uint32_t)target);
                        (void)sub_8001769C(slot, player, r_u32(0x8005B420), sample);
                        w_u32(record + 8, 1);
                    }
                    else
                        w_u32(record + 28, (uint32_t)target);
                    {
                        uint32_t type = r_u32(actor + 180);
                        if ((type >= 18 && r_u32(r_u32(actor + 852) + 604) < 3) || ((r_u8(r_u32(0x8005B2F8) + 4) & 1) && (r_u16(actor + 172) >> 5) == 3 && !type))
                            (void)sub_80017F64(r_u16(record), player);
                    }
                }
                kind = r_u32(record + 8);
                if (kind == 6 && !r_u32(record + 4) && (int32_t)r_u32(0x8005B400) >= 0)
                {
                    uint32_t pitch = sub_8001F934((int32_t)r_u32(0x8005B420), r_u32(0x8005B400), 0);
                    uint32_t bank = r_u32(0x8005B420);
                    uint32_t sample = r_u32(0x8005B400);
                    w_u32(record + 28, pitch);
                    (void)sub_8001769C(slot, player, bank, sample);
                    kind = r_u32(record + 8);
                }
                if (kind == 2 && (int32_t)r_u32(record + 36) < (int32_t)r_u32(r_u32(0x8005B2F8) + 12))
                {
                    uint32_t body = substituted ? actor : r_u32(actor + 852);
                    w_u8(body + 572, r_u8(body + 572) & 0xF3);
                    (void)sub_80017814(slot, player);
                }
                kind = r_u32(record + 8);
                if (kind - 3 < 3 && r_u32(actor + 180) >= 18 && r_u32(r_u32(actor + 852) + 604) >= 3)
                    (void)sub_80017814(slot, player);
                if (r_u16(record) != 224 && r_u32(record + 4))
                    (void)sub_800179D8(slot, player);
            next_audio_record:
                listener = r_u32(0x8005B40C) + 72 * player;
                ++slot;
                count = (int32_t)(r_u32(listener + 36) + r_u32(listener + 40));
            } while ((int32_t)slot < count);
        }
    }
audio_tail:
    if (r_u32(0x800D6BC0) && (int32_t)r_u32(0x800D6BC4) < (int32_t)r_u32(r_u32(0x8005B2F8) + 16))
    {
        (void)sub_8001F7EC(r_u32(0x800D6BC0));
        {
            int32_t index = (int32_t)r_u32(0x800D6BCC);
            w_u32(0x800D6BC0, 0);
            if (index >= 0)
            {
                w_u32(0x800D6BC8, 1);
                w_u32(0x800D6AA8 + 32 * (uint32_t)index, 4);
            }
        }
    }
    if (!r_u32(0x800D6BC8) && r_u32(0x8005AD48) == 8)
        w_u32(0x800D6BC8, 1);
    if (r_u32(0x800D6BD0) && (int32_t)r_u32(0x800D6BD4) < (int32_t)r_u32(r_u32(0x8005B2F8) + 16))
    {
        w_u32(0x800D6BD0, 0);
        w_u32(0x800D6BD4, 0);
    }
    if (r_u32(0x8005B424))
    {
        if (r_u32(0x800D6AA8) == 2)
        {
            uint32_t result = sub_80017BA0(0, 0, 0, r_u32(0x800D6AA0));
            w_u32(0x8005B424, 0);
            return result;
        }
        return 2;
    }
    return 0;
}
