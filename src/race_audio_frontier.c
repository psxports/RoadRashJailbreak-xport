#include "race_audio_frontier.h"
#include "race_pause.h"
#include "race_pause_frontier.h"
#include "audio.h"
#include "fixed_math.h"
#include "xport.h"

static int32_t race_audio_mul_asr(int32_t first, int32_t second, uint32_t shift)
{
    uint32_t product = (uint32_t)((int64_t)first * second);

    if (!shift)
        return (int32_t)product;
    return (int32_t)product >> shift;
}

static int32_t race_audio_abs32(int32_t value)
{
    int32_t sign = value >> 31;

    return (value + sign) ^ sign;
}

static int32_t race_audio_clamp127(int32_t value)
{
    if (value < 0)
        return 0;
    return value < 128 ? value : 127;
}

static uint16_t race_audio_spu_registers[0x100];

static uint16_t race_audio_read_half(RRJMemory *m, uint32_t address)
{
    uint32_t physical = address & 0x1FFFFFFFu;

    if (physical >= 0x1F801C00u && physical < 0x1F801E00u)
        return race_audio_spu_registers[(physical - 0x1F801C00u) >> 1];
    return rrj_u16(rrj_at(m, address, 2));
}

static void race_audio_write_half(RRJMemory *m, uint32_t address, uint16_t value)
{
    uint32_t physical = address & 0x1FFFFFFFu;

    if (physical >= 0x1F801C00u && physical < 0x1F801E00u)
        race_audio_spu_registers[(physical - 0x1F801C00u) >> 1] = value;
    else
        rrj_put16(rrj_at(m, address, 2), value);
}

uint32_t sub_80019990(RRJMemory *m, RRJRaceLeafCall call, RRJVoiceSetupCall setup, RRJSDKCall key)
{
    uint32_t selector;

    FUNCTION_MARKER(0x80019990, "SLUS_010.53");
    if (rrj_read32(m, 0x8005ACA8u) & 4u)
    {
        selector = rrj_read32(m, 0x8005B3ECu);
        if (rrj_read32(m, rrj_read32(m, 0x8005B2F8u) + 48) == 2u || !selector)
        {
            uint32_t record = rrj_read32(m, 0x8005B42Cu) + 132u * selector;
            uint32_t step = rrj_read32(m, record + 56);
            uint32_t position = rrj_read32(m, record + 48) + step;
            uint32_t target = rrj_read32(m, record + 52);
            uint32_t modulation_step = rrj_read32(m, record + 104);
            uint32_t modulation = rrj_read32(m, record + 96) + modulation_step;

            rrj_write32(m, record + 48, position);
            if (rrj_s32((position - target) * step) >= 0)
                rrj_write32(m, record + 48, target);
            if (rrj_s32((position - rrj_read32(m, record + 100)) * modulation_step) >= 0)
                rrj_write32(m, record + 96, rrj_read32(m, record + 100));
            else
                rrj_write32(m, record + 96, modulation);
            if (*(uint8_t *)rrj_at(m, record + 4, 1))
            {
                uint32_t event = 0x1F800350u;
                uint32_t owner = rrj_read32(m, record);
                uint32_t mode = (rrj_read32(m, owner + 564) >> 9) & 1u;
                int32_t base = rrj_s32(rrj_read32(m, record + 112));
                int32_t adjusted = rrj_s32(rrj_read32(m, record + 48));
                int32_t volume;
                int32_t pan;
                uint32_t handle;

                adjusted = rrj_s32((uint32_t)adjusted + ((0u - mode) & (uint32_t)(adjusted >> 2)));
                volume = race_audio_mul_asr(adjusted, rrj_s32(rrj_read32(m, record + 120)), 16);
                pan = mode ? base : race_audio_mul_asr(base, rrj_s32(modulation), 7);
                rrj_write32(m, event, (uint32_t)volume);
                rrj_write32(m, event + 4, (uint32_t)pan);
                rrj_put16(rrj_at(m, event + 8, 2), (uint16_t)rrj_read32(m, record + 116));
                (void)sub_80019C54(m, rrj_read32(m, record + 16), event, call);
                if (rrj_read32(m, record + 20))
                {
                    if (mode)
                    {
                        pan = base;
                        volume = (volume >> 1) + (volume >> 2);
                    }
                    else
                    {
                        pan = race_audio_mul_asr(base, 127 - rrj_s32(modulation), 7);
                    }
                    handle = rrj_read32(m, record + 20);
                }
                else
                {
                    int32_t sample = (int32_t)sub_8001F934(m, rrj_s32(rrj_read32(m, record + 8)), 4, 0);

                    pan = base;
                    volume = race_audio_mul_asr(rrj_s32(rrj_read32(m, record + 120)), sample, 16);
                    handle = rrj_read32(m, record + 24);
                }
                rrj_write32(m, event, (uint32_t)volume);
                rrj_write32(m, event + 4, (uint32_t)pan);
                (void)sub_80019C54(m, handle, event, call);
                {
                    int32_t depth = rrj_s32(rrj_read32(m, record + 48)) >> 4;

                    if (depth >= 128)
                        depth = 127;
                    pan = race_audio_mul_asr(base, depth, 7);
                }
                volume = race_audio_mul_asr(volume, rrj_s32(rrj_read32(m, rrj_read32(m, record + 44) + 12)), 16);
                volume = race_audio_mul_asr(volume, rrj_s32(rrj_read32(m, record + 120)), 16);
                rrj_write32(m, event, (uint32_t)volume);
                rrj_write32(m, event + 4, (uint32_t)pan);
                (void)sub_80019C54(m, rrj_read32(m, record + 28), event, call);
                pan = 0;
                volume = rrj_s32(rrj_read32(m, record + 48)) >> (rrj_read32(m, rrj_read32(m, record + 44) + 16) & 31u);
                volume = rrj_s32((uint32_t)volume + rrj_read32(m, record + 64));
                volume = race_audio_mul_asr(volume, rrj_s32(rrj_read32(m, record + 120)), 16);
                rrj_write32(m, event, (uint32_t)volume);
                rrj_write32(m, event + 4, (uint32_t)pan);
                (void)sub_80019C54(m, rrj_read32(m, record + 32), event, call);
            }
        }
        rrj_write32(m, 0x8005B3ECu, selector ^ 1u);
    }
    return sub_8001EE94(m, setup, key);
}

uint32_t sub_800270F0(RRJMemory *m, uint32_t record)
{
    uint32_t sample;
    int32_t value;

    FUNCTION_MARKER(0x800270F0, "SLUS_010.53");
    sample = sub_80043F00(m, 0xF2000002u) & 0xFFu;
    rrj_put16(rrj_at(m, record + 56, 2), (uint16_t)(sample << 4));
    sample = sub_80043F00(m, 0xF2000002u) & 0xFFu;
    value = (int16_t)(101u * sample) >> 8;
    rrj_put16(rrj_at(m, record + 58, 2), (uint16_t)value);
    if (value < 51)
        return 1;
    value = 50 - value;
    rrj_put16(rrj_at(m, record + 58, 2), (uint16_t)value);
    return (uint32_t)value;
}

uint32_t sub_8004F0A8(RRJMemory *m, int32_t index, uint32_t value)
{
    uint32_t alignment;
    uint32_t shifted;

    FUNCTION_MARKER(0x8004F0A8, "SLUS_010.53");
    if (rrj_read32(m, 0x8005A440u))
    {
        alignment = rrj_read32(m, 0x8005A448u);
        if (alignment && value % alignment)
            value = (value + alignment) & ~rrj_read32(m, 0x8005A44Cu);
    }
    shifted = value >> (rrj_read32(m, 0x8005A444u) & 31u);
    if (index == -2)
        return value;
    if (index == -1)
        return shifted & 0xFFFFu;
    race_audio_write_half(m, rrj_read32(m, 0x8005A41Cu) + 2 * (uint32_t)index, (uint16_t)shifted);
    return value;
}

uint32_t sub_80051A38(RRJMemory *m, int32_t first, int32_t second, int32_t third, int32_t fourth)
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
    value = ((uint32_t)rrj_u16(rrj_at(m, 0x8005A88Cu + 2 * (uint32_t)remainder, 2)) * rrj_u16(rrj_at(m, 0x8005A8A4u + 2 * fraction, 2))) >> 16;
    if ((int16_t)octave >= 0)
        return 0x3FFFu;
    {
        uint32_t shift = (uint32_t)(-(int16_t)octave) & 31u;

        value += 1u << ((shift - 1u) & 31u);
        return (value >> shift) & 0xFFFFu;
    }
}

uint32_t sub_800506A8(RRJMemory *m, uint32_t mode, uint32_t mask, uint32_t first_index, uint32_t second_index)
{
    uint32_t buffered = rrj_read32(m, 0x8005A408u) & 1u;
    uint32_t base = buffered ? 0x800DBF20u : rrj_read32(m, 0x8005A41Cu);
    uint32_t first = race_audio_read_half(m, base + 2 * first_index);
    uint32_t second = race_audio_read_half(m, base + 2 * second_index);
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
    race_audio_write_half(m, base + 2 * first_index, (uint16_t)first);
    race_audio_write_half(m, base + 2 * second_index, (uint16_t)second);
    if (buffered)
    {
        uint32_t bit = (first_index - 198u) >> 1;

        rrj_write32(m, 0x8005A3D4u, rrj_read32(m, 0x8005A3D4u) | (1u << (bit & 31u)));
    }
    return packed & 0xFFFFFFu;
}

uint32_t sub_80051088(RRJMemory *m, uint32_t mode, uint32_t mask)
{
    FUNCTION_MARKER(0x80051088, "SLUS_010.53");
    return sub_800506A8(m, mode, mask, 200, 201);
}

uint32_t sub_8001F900(RRJMemory *m, uint32_t handle, uint32_t enabled)
{
    FUNCTION_MARKER(0x8001F900, "SLUS_010.53");
    return sub_80051088(m, enabled != 0, 1u << ((handle >> 27) & 31u));
}

static uint16_t race_audio_volume_word(RRJMemory *m, uint32_t parameters, uint32_t value_offset, uint32_t mode_offset, uint32_t flags, uint32_t mode_flag)
{
    int32_t raw = (int16_t)rrj_u16(rrj_at(m, parameters + value_offset, 2));
    uint32_t value = (uint16_t)raw & 0x7FFFu;
    uint32_t mode = 0;

    if (!flags || (flags & mode_flag))
    {
        uint32_t kind = rrj_u16(rrj_at(m, parameters + mode_offset, 2));

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

uint32_t sub_80051C38(RRJMemory *m, uint32_t voice, uint32_t parameters)
{
    uint32_t flags = rrj_read32(m, parameters + 4);
    uint32_t all = flags == 0;
    uint32_t base = rrj_read32(m, 0x8005A41Cu);
    uint32_t output = base + 16 * voice;
    uint32_t value;

    FUNCTION_MARKER(0x80051C38, "SLUS_010.53");
    if (all || (flags & 0x10u))
        race_audio_write_half(m, output + 4, rrj_u16(rrj_at(m, parameters + 20, 2)));
    if (all || (flags & 0x40u))
        rrj_put16(rrj_at(m, 0x8005A3D8u + 2 * voice, 2), rrj_u16(rrj_at(m, parameters + 24, 2)));
    if (all || (flags & 0x20u))
    {
        uint32_t tuning = rrj_u16(rrj_at(m, 0x8005A3D8u + 2 * voice, 2));
        uint32_t requested = rrj_u16(rrj_at(m, parameters + 22, 2));

        race_audio_write_half(m, output + 4, (uint16_t)sub_80051A38(m, tuning >> 8, tuning & 0xFFu, requested >> 8, requested & 0xFFu));
    }
    if (all || (flags & 1u))
        race_audio_write_half(m, output, race_audio_volume_word(m, parameters, 8, 12, flags, 4));
    if (all || (flags & 2u))
        race_audio_write_half(m, output + 2, race_audio_volume_word(m, parameters, 10, 14, flags, 8));
    if (all || (flags & 0x80u))
        (void)sub_8004F0A8(m, (int32_t)((voice << 3) | 3u), rrj_read32(m, parameters + 28));
    if (all || (flags & 0x10000u))
        (void)sub_8004F0A8(m, (int32_t)((voice << 3) | 7u), rrj_read32(m, parameters + 32));
    if (all || (flags & 0x20000u))
        race_audio_write_half(m, output + 8, rrj_u16(rrj_at(m, parameters + 58, 2)));
    if (all || (flags & 0x40000u))
        race_audio_write_half(m, output + 10, rrj_u16(rrj_at(m, parameters + 60, 2)));
    if (all || (flags & 0x800u))
    {
        uint32_t depth = rrj_u16(rrj_at(m, parameters + 48, 2));
        uint32_t mode = 0;

        if (depth >= 128)
            depth = 127;
        if ((all || (flags & 0x100u)) && rrj_read32(m, parameters + 36) == 5)
            mode = 0x80u;
        value = race_audio_read_half(m, output + 8);
        race_audio_write_half(m, output + 8, (uint16_t)((value & 0xFFu) | ((depth | mode) << 8)));
    }
    if (all || (flags & 0x1000u))
    {
        uint32_t rate = rrj_u16(rrj_at(m, parameters + 50, 2));

        if (rate >= 16)
            rate = 15;
        value = race_audio_read_half(m, output + 8);
        race_audio_write_half(m, output + 8, (uint16_t)((value & 0xFF0Fu) | (rate << 4)));
    }
    if (all || (flags & 0x2000u))
    {
        uint32_t depth = rrj_u16(rrj_at(m, parameters + 52, 2));
        uint32_t mode = 0x100u;
        uint32_t kind;

        if (depth >= 128)
            depth = 127;
        if (all || (flags & 0x200u))
        {
            kind = rrj_read32(m, parameters + 40);
            if (kind == 1)
                mode = 0;
            else if (kind == 5)
                mode = 0x200u;
            else if (kind == 7)
                mode = 0x300u;
        }
        value = race_audio_read_half(m, output + 10);
        race_audio_write_half(m, output + 10, (uint16_t)((value & 0x3Fu) | ((depth | mode) << 6)));
    }
    if (all || (flags & 0x4000u))
    {
        uint32_t rate = rrj_u16(rrj_at(m, parameters + 54, 2));
        uint32_t mode = 0;

        if (rate >= 32)
            rate = 31;
        if ((all || (flags & 0x400u)) && rrj_read32(m, parameters + 44) == 7)
            mode = 0x20u;
        value = race_audio_read_half(m, output + 10);
        race_audio_write_half(m, output + 10, (uint16_t)((value & 0xFFC0u) | rate | mode));
    }
    if (all || (flags & 0x8000u))
    {
        uint32_t rate = rrj_u16(rrj_at(m, parameters + 56, 2));

        if (rate >= 16)
            rate = 15;
        value = race_audio_read_half(m, output + 8);
        race_audio_write_half(m, output + 8, (uint16_t)((value & 0xFFF0u) | rate));
    }
    return 0;
}

uint32_t sub_8001F874(RRJMemory *m, int32_t offset, uint32_t handle)
{
    uint32_t record = rrj_read32(m, 0x800D687Cu) + 44 * ((handle >> 27) & 31u);
    uint32_t parameters = 0x1F8003D0u;
    uint32_t voice;

    FUNCTION_MARKER(0x8001F874, "SLUS_010.53");
    if ((rrj_read32(m, record + 4) & 0x07FFFFFFu) != (handle & 0x07FFFFFFu))
        return 1;
    voice = rrj_read32(m, record + 28);
    rrj_write32(m, parameters, 1u << (voice & 31u));
    rrj_write32(m, parameters + 4, 0x00010000u);
    rrj_write32(m, parameters + 32, rrj_read32(m, rrj_read32(m, record + 8) + 8) + (uint32_t)offset);
    return sub_80051C38(m, voice, parameters);
}

uint32_t sub_80016768(RRJMemory *m, uint32_t player_index, uint32_t first, uint32_t second, uint32_t third, uint32_t fourth, uint32_t phase)
{
    uint32_t record = rrj_read32(m, 0x8005B40Cu) + 72 * player_index;
    uint32_t result = 2048u - phase;

    FUNCTION_MARKER(0x80016768, "SLUS_010.53");
    rrj_write32(m, record, result);
    rrj_write32(m, record + 4, first);
    rrj_write32(m, record + 8, second);
    rrj_write32(m, record + 12, third);
    rrj_write32(m, record + 16, fourth);
    return result;
}

uint32_t sub_800167A4(RRJMemory *m, uint32_t player_index, RRJReverbCall reverb)
{
    uint32_t record = rrj_read32(m, 0x8005B42Cu) + 132 * player_index;
    uint32_t actor;
    uint32_t level_out = 0x1F8003C0u;
    uint32_t pan_out = level_out + 4;
    uint8_t parameters[12];
    uint32_t first_handle;
    uint32_t second_handle;
    uint32_t result;
    int32_t level;
    int32_t pan;

    FUNCTION_MARKER(0x800167A4, "SLUS_010.53");
    result = *(uint8_t *)rrj_at(m, record + 4, 1);
    if (result)
        return result;
    actor = rrj_read32(m, record);
    (void)sub_80019E40(m, player_index, rrj_s32(rrj_read32(m, actor + 184)), rrj_s32(rrj_read32(m, actor + 192)), 0, 0, level_out, pan_out, 0, 0);
    level = race_audio_mul_asr(rrj_s32(rrj_read32(m, level_out)), rrj_s32(rrj_read32(m, 0x800D6C00u)), 7);
    pan = rrj_s32(rrj_read32(m, pan_out));
    rrj_put32(parameters, (uint32_t)race_audio_mul_asr(rrj_s32(rrj_read32(m, record + 48)), rrj_s32(rrj_read32(m, record + 120)), 16));
    rrj_put32(parameters + 4, (uint32_t)race_audio_mul_asr(level, rrj_s32(rrj_read32(m, record + 96)), 7));
    rrj_put32(parameters + 8, (uint32_t)pan);
    first_handle = sub_8001F174(m, rrj_read32(m, record + 8), 0, 0, 1, parameters);
    rrj_write32(m, record + 16, first_handle);
    rrj_put32(parameters + 4, 0);
    second_handle = sub_8001F174(m, rrj_read32(m, record + 8), 3, 0, 1, parameters);
    rrj_write32(m, record + 28, second_handle);
    (void)sub_8001F900(m, second_handle, 1);
    rrj_put32(parameters, 128);
    first_handle = sub_8001F174(m, rrj_read32(m, record + 8), 2, 0, 1, parameters);
    rrj_write32(m, record + 32, first_handle);
    if ((rrj_read32(m, record + 28) >> 27) - (first_handle >> 27) != 1)
    {
        (void)sub_8001F7EC(m, first_handle, reverb);
        (void)sub_8001F7EC(m, rrj_read32(m, record + 28), reverb);
        (void)sub_8001F900(m, rrj_read32(m, record + 28), 0);
        rrj_write32(m, record + 32, UINT32_MAX);
        rrj_write32(m, record + 28, UINT32_MAX);
    }
    rrj_put32(parameters, UINT32_MAX);
    rrj_put32(parameters + 4, (uint32_t)race_audio_mul_asr(level, 127 - rrj_s32(rrj_read32(m, record + 96)), 7));
    result = sub_8001F174(m, rrj_read32(m, record + 8), 4, 0, 1, parameters);
    rrj_write32(m, record + 24, result);
    rrj_write32(m, record + 20, 0);
    rrj_write32(m, record + 56, 0);
    rrj_write32(m, record + 104, 0);
    rrj_write32(m, record + 124, 0);
    rrj_write32(m, record + 76, 0);
    rrj_write32(m, record + 36, 0);
    rrj_write32(m, record + 40, 0);
    rrj_write32(m, record + 84, UINT32_MAX);
    rrj_write32(m, record + 88, UINT32_MAX);
    rrj_write32(m, record + 92, 0);
    *(uint8_t *)rrj_at(m, record + 4, 1) = 1;
    rrj_write32(m, record + 52, rrj_read32(m, record + 48));
    rrj_write32(m, record + 100, rrj_read32(m, record + 96));
    rrj_write32(m, record + 112, (uint32_t)level);
    rrj_write32(m, record + 116, (uint32_t)pan);
    return result;
}

uint32_t sub_8001B244(RRJMemory *m, uint32_t mode, RRJReverbCall reverb)
{
    uint32_t bank;
    uint32_t sample;
    uint32_t result;

    FUNCTION_MARKER(0x8001B244, "SLUS_010.53");
    if (mode == 4)
    {
        if (rrj_read32(m, 0x800D6C58u) != 23)
            return 0x800D0000u;
        if (rrj_read32(m, 0x800D6B68u) != 2)
            return 2;
        bank = rrj_read32(m, 0x800D6B60u);
        sample = 0;
        rrj_write32(m, 0x800D6BCCu, 6);
        rrj_write32(m, 0x800D6B68u, 3);
        rrj_write32(m, 0x800D6C58u, 4);
    }
    else
    {
        bank = rrj_read32(m, 0x8005B3FCu);
        sample = mode;
        rrj_write32(m, 0x800D6BCCu, UINT32_MAX);
    }
    if (mode == 3 && rrj_read32(m, 0x800D6BD0u))
        return rrj_read32(m, 0x800D6BD0u);
    result = 3;
    if (rrj_read32(m, 0x800D6BC8u))
    {
        uint32_t handle = rrj_read32(m, 0x800D6BC0u);

        if (handle)
            (void)sub_8001F7EC(m, handle, reverb);
        rrj_write32(m, 0x800D6BC0u, sub_8001F174(m, bank, sample, 1, 1, NULL));
        rrj_write32(m, 0x800D6BC8u, 0);
        rrj_write32(m, 0x800D6BC4u, rrj_read32(m, rrj_read32(m, 0x8005B2F8u) + 16) + 1500);
    }
    if (mode == 3)
    {
        rrj_write32(m, 0x800D6BD0u, 1);
        rrj_write32(m, 0x800D6BC8u, 1);
        result = rrj_read32(m, rrj_read32(m, 0x8005B2F8u) + 16) + 9000;
        rrj_write32(m, 0x800D6BD4u, result);
    }
    return result;
}

uint32_t sub_80027778(RRJMemory *m, uint32_t actor, uint32_t kind, uint32_t duration, uint32_t marker)
{
    uint32_t context = rrj_read32(m, 0x8005B2F8u);
    uint32_t split = (*(uint8_t *)rrj_at(m, context + 4, 1) >> 4) & 1u;
    uint32_t limit = 20u >> split;
    uint32_t flags = rrj_read32(m, actor + 36);
    int32_t index;
    uint32_t record;
    uint32_t frame;
    uint32_t i;

    FUNCTION_MARKER(0x80027778, "SLUS_010.53");
    if (((flags >> 19) & 15u) >= limit)
        return limit;
    index = (int32_t)sub_80027178(m);
    if (index == -1)
        return UINT32_MAX;
    record = 0x800D39B0u + 112u * (uint32_t)index;
    frame = rrj_read32(m, context + 16);
    if (frame == rrj_read32(m, 0x8005B360u))
        return rrj_read32(m, 0x8005B360u);
    rrj_write32(m, record + 52, duration);
    rrj_write32(m, record, (rrj_read32(m, record) & 0xFFC0003Fu) | 0x5C0u | ((kind & 0xFFu) << 14));
    rrj_write32(m, 0x8005B360u, frame);
    *(uint8_t *)rrj_at(m, record + 60, 1) = (uint8_t)marker;
    flags = (flags & 0xFF87FFFFu) | (((((flags >> 19) & 15u) + 1u) & 15u) << 19);
    rrj_write32(m, actor + 36, flags);
    (void)sub_800289E8(m, actor, 0x80053670u + 12u * kind, record + 20);
    *(uint8_t *)rrj_at(m, record + 108, 1) = 0;
    *(uint8_t *)rrj_at(m, record + 109, 1) = 1;
    rrj_write32(m, record + 96, 300u >> split);
    rrj_write32(m, record + 48, frame);
    rrj_write32(m, record + 100, frame);
    for (i = 0; i < 4; ++i)
        *(uint8_t *)rrj_at(m, record + 104 + i, 1) = (uint8_t)(7u + i);
    (void)sub_800270F0(m, record);
    *(uint8_t *)rrj_at(m, record + 61, 1) = kind != 1u;
    rrj_write32(m, record + 64, 3);
    rrj_write32(m, record + 36, 0);
    rrj_write32(m, record + 40, 0);
    rrj_write32(m, record + 44, 0);
    rrj_put16(rrj_at(m, record + 62, 2), 30);
    return sub_800271CC(m, actor, (uint32_t)index);
}

uint32_t sub_80027974(RRJMemory *m, uint32_t actor, uint32_t unused, uint32_t kind)
{
    uint32_t context = rrj_read32(m, 0x8005B2F8u);
    uint32_t flags = rrj_read32(m, actor + 36);
    int32_t height = (int16_t)rrj_u16(rrj_at(m, actor + 482, 2));
    int32_t index;
    uint32_t record;
    uint32_t frame;
    int32_t duration;

    FUNCTION_MARKER(0x80027974, "SLUS_010.53");
    if (rrj_u16(rrj_at(m, actor + 172, 2)) >= rrj_read32(m, context + 48))
        return rrj_read32(m, context + 48);
    if ((flags >> 5) & 3u)
        return (flags >> 5) & 3u;
    if (!kind)
    {
        if (((flags >> 19) & 15u) >= 4u)
            return 0;
        if ((flags >> 23) & 3u)
            return (flags >> 23) & 3u;
    }
    index = (int32_t)sub_80027178(m);
    if (index == -1)
        return UINT32_MAX;
    frame = rrj_read32(m, context + 16);
    if (frame == rrj_read32(m, 0x8005B360u))
        return rrj_read32(m, 0x8005B360u);
    rrj_write32(m, 0x8005B360u, frame);
    record = 0x800D39B0u + 112u * (uint32_t)index;
    rrj_write32(m, record, (rrj_read32(m, record) & 0xFFC0003Fu) | 0x480u | ((kind & 0xFFu) << 14));
    if (kind)
    {
        flags = (flags & 0xFE7FFFFFu) | (((((flags >> 23) & 3u) + 1u) & 3u) << 23);
        rrj_put16(rrj_at(m, record + 62, 2), 35);
    }
    else
    {
        flags = (flags & 0xFF87FFFFu) | (((((flags >> 19) & 15u) + 1u) & 15u) << 19);
        rrj_put16(rrj_at(m, record + 62, 2), 30);
    }
    rrj_write32(m, actor + 36, flags);
    duration = 150 - 2 * height;
    if (duration < 0)
        duration = 0;
    if (duration > 0x960000)
        duration = 0x960000;
    rrj_write32(m, record + 52, (uint32_t)duration);
    rrj_write32(m, record + 48, frame);
    (void)sub_8002705C(m, record, (uint32_t)height);
    *(uint8_t *)rrj_at(m, record + 61, 1) = 1;
    rrj_write32(m, record + 36, 0);
    rrj_write32(m, record + 40, 0);
    rrj_write32(m, record + 44, 0);
    rrj_write32(m, record + 64, 9);
    return sub_800271CC(m, actor, (uint32_t)index);
}

uint32_t sub_80027258(RRJMemory *m, uint32_t actor, uint32_t kind)
{
    uint32_t context = rrj_read32(m, 0x8005B2F8u);
    uint32_t flags = rrj_read32(m, actor + 36);
    int32_t height = (int16_t)rrj_u16(rrj_at(m, actor + 482, 2));
    int32_t index;
    uint32_t record;
    int32_t duration;

    FUNCTION_MARKER(0x80027258, "SLUS_010.53");
    if (rrj_u16(rrj_at(m, actor + 172, 2)) >= rrj_read32(m, context + 48))
        return rrj_read32(m, context + 48);
    if (((flags >> 25) & 3u) >= 2u)
        return ((flags >> 25) & 3u) < 2u;
    index = (int32_t)sub_80027178(m);
    if (index == -1)
        return UINT32_MAX;
    record = 0x800D39B0u + 112u * (uint32_t)index;
    rrj_write32(m, record, (rrj_read32(m, record) & 0xFFC03C3Fu) | 0x40u | ((kind & 0xFFu) << 14));
    flags = (flags & 0xF9FFFFFFu) | (((((flags >> 25) & 3u) + 1u) & 3u) << 25);
    rrj_write32(m, actor + 36, flags);
    (void)sub_800289E8(m, actor, 0x80053670u + 12u * kind, record + 20);
    duration = 150 - 2 * height;
    if (duration < 0)
        duration = 0;
    if (duration > 9830400)
        duration = 9830400;
    rrj_write32(m, record + 52, (uint32_t)duration);
    rrj_write32(m, record + 48, rrj_read32(m, context + 16));
    (void)sub_8002705C(m, record, (uint32_t)height);
    rrj_write32(m, record + 36, 0);
    rrj_write32(m, record + 40, 0);
    rrj_write32(m, record + 44, 0);
    *(uint8_t *)rrj_at(m, record + 61, 1) = 0;
    rrj_put16(rrj_at(m, record + 62, 2), 30);
    return sub_800271CC(m, actor, (uint32_t)index);
}

uint32_t sub_800179D8(RRJMemory *m, uint32_t slot, uint32_t listener, RRJRaceLeafCall call)
{
    uint32_t record = rrj_read32(m, 0x8005B410u + 4u * listener) + 44u * slot;
    uint32_t level_out = 0x1F8003B0u;
    uint32_t pan_out = level_out + 4;
    uint32_t ratio_out = pan_out + 4;
    uint32_t parameters = ratio_out + 4;
    uint32_t kind = rrj_read32(m, record + 8);
    uint32_t mode;
    uint32_t shift = kind == 3u || kind == 4u || kind == 6u ? 2u : 0u;
    int32_t x;
    int32_t z;
    int32_t level;

    FUNCTION_MARKER(0x800179D8, "SLUS_010.53");
    x = rrj_s32(rrj_read32(m, record + 12)) + (rrj_s32(rrj_read32(m, record + 20)) >> 6);
    z = rrj_s32(rrj_read32(m, record + 16)) + (rrj_s32(rrj_read32(m, record + 24)) >> 6);
    rrj_write32(m, record + 12, (uint32_t)x);
    rrj_write32(m, record + 16, (uint32_t)z);
    (void)sub_80019E40(m, listener, x, z, rrj_s32(rrj_read32(m, record + 20)), rrj_s32(rrj_read32(m, record + 24)), level_out, pan_out, ratio_out, shift);
    if (kind >= 3u && kind <= 5u)
        mode = 2;
    else if (kind == 2u)
        mode = 4;
    else
        mode = 1;
    level = race_audio_mul_asr(rrj_s32(rrj_read32(m, level_out)), rrj_s32(rrj_read32(m, 0x800D6C00u + 4u * mode)), 7);
    if (rrj_read32(m, 0x8005B434u))
        level = 0;
    rrj_write32(m, parameters, (uint32_t)race_audio_mul_asr(rrj_s32(rrj_read32(m, record + 28)), rrj_s32(rrj_read32(m, ratio_out)), 16));
    rrj_write32(m, parameters + 4, (uint32_t)level);
    rrj_write32(m, parameters + 8, rrj_read32(m, pan_out));
    return sub_8001F6A4(m, rrj_read32(m, record + 32), parameters, call);
}

uint32_t sub_800169D0(RRJMemory *m, uint32_t player_index, RRJReverbCall reverb)
{
    uint32_t record = rrj_read32(m, 0x8005B42Cu) + 132u * player_index;
    uint32_t actor = rrj_read32(m, record);
    uint32_t body = rrj_read32(m, actor + 852);
    uint32_t level_out = 0x1F8003A0u;
    uint32_t pan_out = level_out + 4;
    uint32_t ratio_out = pan_out + 4;
    uint8_t parameters[12];
    uint32_t state = rrj_read32(m, body + 604);
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
    engine = (int32_t)sub_8001FC90(rrj_s32(rrj_read32(m, actor + 604)) >> 4, rrj_s32(rrj_read32(m, record + 60)));
    if (state >= 3u || !rrj_u16(rrj_at(m, body + 544, 2)))
        engine = 0;
    effect = 0;
    if (state < 2u)
    {
        int32_t value = (int32_t)sub_8001FC90(rrj_s32(rrj_read32(m, actor + 588)), rrj_s32(rrj_read32(m, 0x800D6BD8u + 4u * (uint32_t)(int8_t)*(uint8_t *)rrj_at(m, actor + 849, 1)))) >> 9;

        effect = value < 127 ? value : 127;
    }
    (void)sub_80019E40(m, player_index, rrj_s32(rrj_read32(m, actor + 184)), rrj_s32(rrj_read32(m, actor + 192)), rrj_s32(rrj_read32(m, actor + 456)), rrj_s32(rrj_read32(m, actor + 464)), level_out, pan_out, ratio_out, 0);
    level = race_audio_mul_asr(rrj_s32(rrj_read32(m, level_out)), rrj_s32(rrj_read32(m, 0x800D6C00u)), 7);
    pan = rrj_s32(rrj_read32(m, pan_out));
    ratio = rrj_s32(rrj_read32(m, ratio_out));
    rrj_write32(m, record + 112, (uint32_t)level);
    rrj_write32(m, record + 116, (uint32_t)pan);
    rrj_write32(m, record + 120, (uint32_t)ratio);
    if (engine < 896)
    {
        engine = 896;
        if (!rrj_read32(m, record + 24))
        {
            rrj_put32(parameters, (uint32_t)race_audio_mul_asr(ratio, (int32_t)sub_8001F934(m, rrj_s32(rrj_read32(m, record + 8)), 4, 0), 16));
            rrj_put32(parameters + 4, (uint32_t)race_audio_mul_asr(level, 127 - rrj_s32(rrj_read32(m, record + 96)), 7));
            rrj_put32(parameters + 8, (uint32_t)pan);
            handle = sub_8001F174(m, rrj_read32(m, record + 8), 4, 0, 1, parameters);
            rrj_write32(m, record + 24, handle);
            (void)sub_8001F7EC(m, rrj_read32(m, record + 20), reverb);
            rrj_write32(m, record + 20, 0);
        }
    }
    else
    {
        if (!rrj_read32(m, record + 20))
        {
            rrj_put32(parameters, (uint32_t)race_audio_mul_asr(rrj_s32(rrj_read32(m, record + 48)), ratio, 16));
            rrj_put32(parameters + 4, (uint32_t)race_audio_mul_asr(level, 127 - rrj_s32(rrj_read32(m, record + 96)), 7));
            rrj_put32(parameters + 8, (uint32_t)pan);
            handle = sub_8001F174(m, rrj_read32(m, record + 8), 1, 0, 1, parameters);
            rrj_write32(m, record + 20, handle);
            (void)sub_8001F7EC(m, rrj_read32(m, record + 24), reverb);
            rrj_write32(m, record + 24, 0);
        }
        if ((uint32_t)(*(uint8_t *)rrj_at(m, actor + 534, 1) - 1u) >= 2u)
        {
            engine += (101 * (int32_t)(sub_80043F00(m, 0xF2000002u) & 0xFFu)) >> 8;
            effect += (11 * (int32_t)(sub_80043F00(m, 0xF2000002u) & 0xFFu)) >> 8;
        }
    }
    table = rrj_read32(m, record + 44);
    if (rrj_s32(rrj_read32(m, actor + 588)) < (rrj_s32(rrj_read32(m, actor + 592)) >> 1) && !(rrj_read32(m, actor + 564) & 0x20u))
    {
        rrj_write32(m, record + 64, rrj_read32(m, table + 8));
        mode = 0;
    }
    else
    {
        rrj_write32(m, record + 64, rrj_read32(m, table + 4));
        mode = 1;
    }
    (void)sub_8001F874(m, (int32_t)(mode << 7), rrj_read32(m, record + 32));
    mode = 0;
    if (rrj_read32(m, actor + 564) & 0x200u)
        mode = 4;
    else if (rrj_read32(m, actor + 564) & 0x20u)
        mode = 2;
    (void)sub_8001F874(m, (int32_t)(mode << 7), rrj_read32(m, record + 28));
    previous_gear = rrj_s32(rrj_read32(m, record + 108));
    gear = (int8_t)*(uint8_t *)rrj_at(m, actor + 849, 1);
    if (previous_gear < gear)
        engine += 512;
    else if (previous_gear > gear)
    {
        engine -= 512;
        effect += 96;
        if (effect >= 127)
            effect = 126;
    }
    rrj_write32(m, record + 108, (uint32_t)gear);
    limit = rrj_s32(rrj_read32(m, table));
    if (engine > limit)
        engine = limit;
    if (!*(uint8_t *)rrj_at(m, record + 4, 1))
        return sub_800167A4(m, player_index, reverb);
    rrj_write32(m, record + 52, (uint32_t)engine);
    rrj_write32(m, record + 100, (uint32_t)effect);
    rrj_write32(m, record + 56, (uint32_t)((engine - rrj_s32(rrj_read32(m, record + 48))) >> 1));
    effect = (effect - rrj_s32(rrj_read32(m, record + 96))) >> 1;
    rrj_write32(m, record + 104, (uint32_t)effect);
    return (uint32_t)effect;
}

uint32_t sub_80016E4C(RRJMemory *m, uint32_t player_index, RRJRaceLeafCall call, RRJReverbCall reverb)
{
    uint32_t record = rrj_read32(m, 0x8005B42Cu) + 132u * player_index;
    uint32_t actor = rrj_read32(m, record);
    uint32_t body = rrj_read32(m, actor + 852);
    uint32_t parameters = 0x1F800300u;
    int32_t primary_volume = 0;
    int32_t secondary_volume = 0;
    int32_t primary_sample = -1;
    int32_t secondary_sample = -1;
    int32_t active = rrj_s32(rrj_read32(m, record + 124));
    uint32_t unusual = (uint32_t)(*(uint8_t *)rrj_at(m, actor + 534, 1) - 1u) >= 2u;
    int32_t level;
    int32_t pan = rrj_s32(rrj_read32(m, record + 116));
    uint32_t handle;
    uint32_t result = 0;

    FUNCTION_MARKER(0x80016E4C, "SLUS_010.53");
    if (active > 0)
    {
        if (active & 1)
        {
            int32_t speed = rrj_s32(rrj_read32(m, actor + 480));
            int32_t delta = rrj_s32(rrj_read32(m, actor + 188)) - rrj_s32(rrj_read32(m, actor + 508));

            primary_volume = speed > 0x7FFFF ? 127 : speed >> 12;
            if (race_audio_abs32(delta) >= 0x4000)
                primary_sample = -1;
            else if (unusual)
                primary_sample = race_audio_abs32(rrj_s32(rrj_read32(m, actor + 652))) >= 0x4001 ? 18 : 16;
            else
                primary_sample = race_audio_abs32(rrj_s32(rrj_read32(m, actor + 652))) >= 0x4001 ? 17 : 12;
            if (speed <= 0x7FFF)
            {
                primary_sample = -1;
                active &= ~1;
                rrj_write32(m, record + 124, (uint32_t)active);
            }
        }
        if (active & 2)
        {
            int32_t speed = rrj_s32(rrj_read32(m, body + 480));
            int32_t alternate = (rrj_read32(m, actor + 388) & 1u) || (int8_t)*(uint8_t *)rrj_at(m, actor + 534, 1) >= 3;

            secondary_volume = speed > 0x7FFFF ? 127 : speed >> 12;
            secondary_sample = 19 + alternate;
            if ((rrj_read32(m, body + 552) & 0x40000000u) && race_audio_abs32(rrj_s32(rrj_read32(m, body + 188)) - rrj_s32(rrj_read32(m, body + 508))) >= 0x4000)
                secondary_sample = -1;
            if (rrj_s32(rrj_read32(m, record + 88)) < 0 && secondary_sample >= 0)
                (void)sub_80017BA0(m, rrj_s32(rrj_read32(m, body + 184)), rrj_s32(rrj_read32(m, body + 192)), *(uint8_t *)rrj_at(m, actor + 534, 1) == 4 ? 102 : 55, 0);
            if (speed <= 0x7FFF)
            {
                secondary_sample = -1;
                active &= ~2;
                rrj_write32(m, record + 124, (uint32_t)active);
            }
        }
    }
    else if (!(rrj_read32(m, actor + 568) & 0x400u))
    {
        uint32_t collision = (rrj_read32(m, actor + 36) >> 25) & 3u;
        int32_t speed = rrj_s32(rrj_read32(m, actor + 700));

        if (collision || rrj_read32(m, record + 92))
        {
            primary_sample = 17;
            if (collision)
                rrj_write32(m, record + 92, 4);
            else
                rrj_write32(m, record + 92, rrj_read32(m, record + 92) - 1u);
            primary_volume = race_audio_clamp127(32 * rrj_s32(rrj_read32(m, record + 92)));
        }
        if (speed > 73727)
        {
            primary_volume = speed >> 10;
            if (race_audio_abs32(rrj_s32(rrj_read32(m, actor + 652))) < 16385)
            {
                primary_sample = 13;
                if (primary_volume >= 128)
                    primary_volume = 127;
            }
            else
            {
                int32_t value = ((int32_t)sub_8001FC90(rrj_s32(rrj_read32(m, record + 68)), race_audio_abs32(rrj_s32(rrj_read32(m, actor + 676)))) >> 9) - 32 + (int32_t)(sub_8001FC58(m) & 63u);
                int32_t threshold = race_audio_abs32(rrj_s32(rrj_read32(m, actor + 676))) - (rrj_s32(rrj_read32(m, rrj_read32(m, actor + 556) + 204)) >> 1);

                primary_sample = 12;
                primary_volume = race_audio_clamp127(value);
                if (primary_volume >= 40)
                    rrj_write32(m, record + 76, 0);
                else
                {
                    uint32_t count = rrj_read32(m, record + 76);

                    rrj_write32(m, record + 76, count + 1u);
                    if (count >= 8u)
                    {
                        rrj_write32(m, record + 76, 0);
                        primary_sample = -1;
                    }
                }
                if (threshold < 0)
                    threshold = 0;
                secondary_sample = 14;
                secondary_volume = race_audio_clamp127(((int32_t)sub_8001FC90(rrj_s32(rrj_read32(m, record + 72)), threshold) >> 9) - 32 + (int32_t)(sub_8001FC58(m) & 63u));
                rrj_write32(m, record + 48, rrj_read32(m, record + 48) + (uint32_t)secondary_volume);
                rrj_write32(m, record + 52, rrj_read32(m, record + 52) + (uint32_t)secondary_volume);
            }
        }
        else if (rrj_s32(rrj_read32(m, actor + 696)) > 51199)
        {
            primary_sample = 14;
            primary_volume = race_audio_clamp127((rrj_s32(rrj_read32(m, actor + 696)) - 51200) >> 7);
        }
        if (unusual)
        {
            if (!rrj_read32(m, record + 92))
            {
                if (primary_sample != 17)
                {
                    primary_sample = 16;
                    if (secondary_volume > primary_volume)
                        primary_volume = secondary_volume;
                }
            }
            secondary_sample = 15;
            secondary_volume = rrj_s32(rrj_read32(m, actor + 480)) > 0xFFFFF ? 127 : rrj_s32(rrj_read32(m, actor + 480)) >> 14;
        }
        if ((uint32_t)(rrj_read32(m, body + 604) - 1u) >= 3u)
            primary_volume = 0;
    }

    level = race_audio_mul_asr(rrj_s32(rrj_read32(m, record + 112)), rrj_s32(rrj_read32(m, 0x800D6C0Cu)), 7);
    if (rrj_read32(m, 0x8005B434u))
        level = 0;
    rrj_write32(m, parameters, (uint32_t)race_audio_mul_asr(rrj_s32(rrj_read32(m, record + 120)), (int32_t)sub_8001F934(m, rrj_s32(rrj_read32(m, 0x8005B420u)), (uint32_t)primary_sample, 0), 16));
    rrj_write32(m, parameters + 4, (uint32_t)race_audio_mul_asr(level, primary_volume, 7));
    rrj_write32(m, parameters + 8, (uint32_t)pan);
    if (primary_sample == rrj_s32(rrj_read32(m, record + 84)))
    {
        handle = rrj_read32(m, record + 36);
        if (handle)
            result = sub_8001F6A4(m, handle, parameters, call);
    }
    else
    {
        handle = rrj_read32(m, record + 36);
        rrj_write32(m, record + 84, (uint32_t)primary_sample);
        if (handle)
            (void)sub_8001F7EC(m, handle, reverb);
        if (primary_sample >= 0)
            rrj_write32(m, record + 36, sub_8001F174(m, rrj_read32(m, 0x8005B420u), (uint32_t)primary_sample, 0, 1, rrj_at(m, parameters, 12)));
    }
    if (active)
    {
        uint32_t level_out = 0x1F800310u;
        uint32_t pan_out = level_out + 4;

        (void)sub_80019E40(m, player_index, rrj_s32(rrj_read32(m, body + 184)), rrj_s32(rrj_read32(m, body + 192)), 0, 0, level_out, pan_out, 0, 0);
        level = race_audio_mul_asr(rrj_s32(rrj_read32(m, level_out)), rrj_s32(rrj_read32(m, 0x800D6C0Cu)), 7);
        if (rrj_read32(m, 0x8005B434u))
            level = 0;
        pan = rrj_s32(rrj_read32(m, pan_out));
    }
    rrj_write32(m, parameters, (uint32_t)race_audio_mul_asr(rrj_s32(rrj_read32(m, record + 120)), (int32_t)sub_8001F934(m, rrj_s32(rrj_read32(m, 0x8005B420u)), (uint32_t)secondary_sample, 0), 16));
    rrj_write32(m, parameters + 4, (uint32_t)race_audio_mul_asr(level, secondary_volume, 7));
    rrj_write32(m, parameters + 8, (uint32_t)pan);
    if (secondary_sample == rrj_s32(rrj_read32(m, record + 88)))
    {
        handle = rrj_read32(m, record + 40);
        if (handle)
            result = sub_8001F6A4(m, handle, parameters, call);
    }
    else
    {
        handle = rrj_read32(m, record + 40);
        rrj_write32(m, record + 88, (uint32_t)secondary_sample);
        if (handle)
            (void)sub_8001F7EC(m, handle, reverb);
        if (secondary_sample >= 0)
            rrj_write32(m, record + 40, sub_8001F174(m, rrj_read32(m, 0x8005B420u), (uint32_t)secondary_sample, 0, 1, rrj_at(m, parameters, 12)));
    }
    if (rrj_read32(m, actor + 564) & 0x18000000u)
        result = sub_80027778(m, actor, 1, 600, 0);
    else if ((rrj_read32(m, actor + 388) & 1u) && (int16_t)rrj_u16(rrj_at(m, actor + 482, 2)) >= 13)
        result = sub_80027974(m, actor, actor + 184, 0);
    else if (rrj_s32(rrj_read32(m, actor + 700)) > 73727 && (rrj_read32(m, actor + 564) & 8u))
        result = sub_80027974(m, actor, actor + 184, 0);
    if (rrj_s32(rrj_read32(m, actor + 696)) > 73727 && (rrj_read32(m, actor + 564) & 4u))
        result = sub_80027974(m, actor, actor + 184, 2);
    return result;
}

static uint32_t race_audio_candidate_actor(RRJMemory *m, uint16_t id)
{
    uint32_t type = id >> 5;
    uint32_t index = id & 31u;

    if (type == 5u && index >= 29u)
        return rrj_read32(m, 0x8005B304u) + 280u * (index - 29u);
    return rrj_read32(m, 0x800CE4D0u + 16u * type) + rrj_read32(m, 0x800CE4D4u + 16u * type) * index;
}

uint32_t sub_800184AC(RRJMemory *m, uint32_t player_index, RRJRaceLeafCall call, RRJReverbCall reverb)
{
    uint32_t candidates = 0x1F800280u;
    uint32_t count_io = candidates + 32;
    uint32_t listener = rrj_read32(m, 0x8005B40Cu) + 72u * player_index;
    uint32_t records = rrj_read32(m, 0x8005B410u + 4u * player_index);
    uint32_t capacity = rrj_read32(m, listener + 36);
    int32_t selected[16];
    int32_t distance[16];
    uint32_t count;
    uint32_t i;
    uint32_t j;

    FUNCTION_MARKER(0x800184AC, "SLUS_010.53");
    if (capacity > 16u)
        capacity = 16u;
    rrj_write32(m, count_io, 10);
    (void)sub_8008AE94(m, candidates, count_io, 0x800CD950u + 1132u * player_index, 0x800000, 9, rrj_u16(rrj_at(m, rrj_read32(m, rrj_read32(m, 0x8005B42Cu) + 132u * player_index) + 172, 2)));
    count = rrj_read32(m, count_io);
    if (rrj_read32(m, 0x8005B314u))
    {
        uint32_t extra = rrj_read32(m, 0x8005B304u);

        for (i = 0; i < 3 && count < 16u; ++i)
        {
            uint16_t id = rrj_u16(rrj_at(m, extra + 280u * i + 172, 2));

            if (id && *(uint8_t *)rrj_at(m, extra + 280u * i + 8, 1) != 9)
            {
                rrj_put16(rrj_at(m, candidates + 2u * count, 2), id);
                ++count;
            }
        }
        rrj_write32(m, count_io, count);
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
        uint16_t id = rrj_u16(rrj_at(m, candidates + 2u * i, 2));
        uint32_t candidate = race_audio_candidate_actor(m, id);
        int32_t dx = rrj_s32(rrj_read32(m, candidate + 184)) - rrj_s32(rrj_read32(m, listener + 4));
        int32_t dz = rrj_s32(rrj_read32(m, candidate + 192)) - rrj_s32(rrj_read32(m, listener + 8));
        int32_t high = race_audio_abs32(dx);
        int32_t low = race_audio_abs32(dz);
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
        if ((*(uint8_t *)rrj_at(m, rrj_read32(m, 0x8005B2F8u) + 4, 1) & 1u) && *(uint8_t *)rrj_at(m, rrj_read32(m, 0x8005B2F8u) + 4, 1) != 33)
        {
            uint32_t local = rrj_read32(m, 0x8005B268u + 4u * player_index);

            if ((*(uint8_t *)rrj_at(m, rrj_read32(m, local + 1084) + 1, 1) & 15u) == 2u && rrj_u16(rrj_at(m, candidate + 172, 2)) == *(uint8_t *)rrj_at(m, rrj_read32(m, 0x8005B2F8u) + 6, 1) && race_audio_abs32(rrj_s32(rrj_read32(m, candidate + 324)) - rrj_s32(rrj_read32(m, local + 324))) <= 204799)
                (void)sub_8001B244(m, 3, reverb);
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
        uint16_t id = rrj_u16(rrj_at(m, records + 44u * i, 2));
        uint32_t found = id == 224;

        for (j = 0; !found && j < capacity; ++j)
            if (selected[j] >= 0 && id == rrj_u16(rrj_at(m, candidates + 2u * (uint32_t)selected[j], 2)))
                found = 1;
        if (!found)
            (void)sub_80017814(m, i, player_index, call);
    }
    for (i = 0; i < capacity; ++i)
    {
        uint32_t found = selected[i] < 0;
        uint16_t id;

        if (found)
            continue;
        id = rrj_u16(rrj_at(m, candidates + 2u * (uint32_t)selected[i], 2));
        for (j = 0; j < capacity; ++j)
            if (rrj_u16(rrj_at(m, records + 44u * j, 2)) == id)
                found = 1;
        if (!found)
        {
            for (j = 0; j < capacity; ++j)
            {
                uint32_t record = records + 44u * j;

                if (rrj_u16(rrj_at(m, record, 2)) == 224)
                {
                    rrj_put16(rrj_at(m, record, 2), id);
                    rrj_write32(m, record + 8, 1);
                    break;
                }
            }
        }
    }
    for (i = 0; i < capacity; ++i)
    {
        uint16_t id = rrj_u16(rrj_at(m, records + 44u * i, 2));

        if (id == 224)
            continue;
        for (j = i + 1u; j < capacity; ++j)
            if (id == rrj_u16(rrj_at(m, records + 44u * j, 2)))
                (void)sub_80017814(m, j, player_index, call);
    }
    return i < capacity;
}

uint32_t sub_80018FAC(RRJMemory *m, RRJRaceLeafCall call, RRJReverbCall reverb)
{
    uint32_t audio_records = rrj_read32(m, 0x8005B42Cu);
    uint32_t listeners = rrj_read32(m, 0x8005B40Cu);
    uint32_t context = rrj_read32(m, 0x8005B2F8u);
    uint32_t players = rrj_read32(m, context + 48);
    uint32_t changed = 0;
    uint32_t player;

    FUNCTION_MARKER(0x80018FAC, "SLUS_010.53");
    if (audio_records && listeners)
    {
        for (player = 0; player < players; ++player)
        {
            uint32_t audio = audio_records + 132u * player;
            uint32_t actor = rrj_read32(m, audio);
            uint32_t body = rrj_read32(m, actor + 852);
            uint32_t camera = 0x800CD898u + 1132u * player;
            uint32_t listener = listeners + 72u * player;
            uint32_t state_flags;
            uint32_t steering_mode;
            uint32_t timer_index;

            if (rrj_read32(m, camera + 552) == 2u && (*(uint8_t *)rrj_at(m, context + 4, 1) & 1u))
                rrj_write32(m, 0x800D6C04u, 0);
            if (rrj_read32(m, camera + 772) || rrj_read32(m, body + 604) >= 3u)
            {
                int32_t speed = rrj_s32(rrj_read32(m, camera + 480)) >> 4;

                (void)sub_80016768(m, player, rrj_read32(m, camera + 184), rrj_read32(m, camera + 192), (uint32_t)race_audio_mul_asr((int16_t)rrj_u16(rrj_at(m, camera + 450, 2)), speed, 8), (uint32_t)race_audio_mul_asr((int16_t)rrj_u16(rrj_at(m, camera + 454, 2)), speed, 8), (uint32_t)(int16_t)rrj_u16(rrj_at(m, rrj_read32(m, 0x8005AEC0u) + 124, 2)));
            }
            else
            {
                (void)sub_80016768(m, player, rrj_read32(m, actor + 184), rrj_read32(m, actor + 192), rrj_read32(m, actor + 456), rrj_read32(m, actor + 464), (uint32_t)(int16_t)rrj_u16(rrj_at(m, rrj_read32(m, 0x8005AEC0u) + 124, 2)));
            }
            state_flags = rrj_read32(m, (rrj_read32(m, body + 604) >= 3u ? body : actor) + 36);
            steering_mode = ((state_flags >> 5) & 3u) == 1u ? 2u : 0u;
            if (steering_mode != rrj_read32(m, listener + 24) && !rrj_read32(m, 0x8005B434u))
            {
                int32_t previous = rrj_s32(rrj_read32(m, listener + 28));
                int32_t step;
                uint32_t phase;

                rrj_write32(m, listener + 24, 1);
                if (previous >= 0)
                    step = race_audio_abs32(previous - (int16_t)rrj_u16(rrj_at(m, actor + 370, 2))) << 11;
                else
                    step = 2048;
                if (!steering_mode)
                    step = -step;
                phase = rrj_read32(m, listener + 32) + (uint32_t)step;
                rrj_write32(m, listener + 32, phase);
                rrj_write32(m, listener + 28, (uint32_t)(int16_t)rrj_u16(rrj_at(m, actor + 370, 2)));
                changed = 1;
                if (phase >= 0x5000u)
                {
                    rrj_write32(m, listener + 24, steering_mode);
                    rrj_write32(m, listener + 32, steering_mode ? 20479u : 0u);
                    rrj_write32(m, listener + 28, UINT32_MAX);
                }
            }
            (void)sub_800169D0(m, player, reverb);
            (void)sub_80016E4C(m, player, call, reverb);
            for (timer_index = 0; timer_index < 2u; ++timer_index)
            {
                uint32_t timer = listener + 4u * timer_index;
                uint32_t source = rrj_read32(m, timer + 56);

                if (source)
                {
                    int32_t remaining = rrj_s32(rrj_read32(m, timer + 64)) - 1;

                    rrj_write32(m, timer + 64, (uint32_t)remaining);
                    if (remaining <= 0)
                    {
                        rrj_write32(m, timer + 64, 0);
                        (void)sub_80017BA0(m, rrj_s32(rrj_read32(m, source + 184)), rrj_s32(rrj_read32(m, source + 192)), rrj_read32(m, timer + 48), 0);
                        rrj_write32(m, timer + 56, 0);
                        rrj_write32(m, timer + 48, 0);
                    }
                }
            }
        }
        if (changed)
        {
            uint32_t second = players == 1u ? rrj_read32(m, listeners + 32) : rrj_read32(m, listeners + 104);

            (void)sub_8001F054(m, rrj_read32(m, listeners + 32), second, call);
        }
    }
    if (listeners)
    {
        uint32_t counter = rrj_read32(m, 0x8005B430u);

        rrj_write32(m, 0x8005B430u, counter + 1u);
        if (!(counter & 3u))
        {
            uint32_t selected = ((counter + 1u) & 4u) >> 2;

            if (!selected || players == 2u)
                (void)sub_800184AC(m, selected, call, reverb);
        }
        for (player = 0; player < players; ++player)
        {
            uint32_t listener = listeners + 72u * player;
            uint32_t records = rrj_read32(m, 0x8005B410u + 4u * player);
            uint32_t count = rrj_read32(m, listener + 36) + rrj_read32(m, listener + 40);
            uint32_t slot;

            for (slot = 0; slot < count; ++slot)
            {
                uint32_t record = records + 44u * slot;
                uint16_t id = rrj_u16(rrj_at(m, record, 2));
                uint32_t actor;
                uint32_t body;
                uint32_t substituted = 0;
                uint32_t kind;

                if (id == 224)
                    continue;
                actor = race_audio_candidate_actor(m, id);
                body = rrj_read32(m, actor + 852);
                if (rrj_read32(m, record + 8) == 2u && rrj_read32(m, body + 604) >= 3u)
                {
                    actor = body;
                    substituted = 1;
                }
                rrj_write32(m, record + 12, rrj_read32(m, actor + 184));
                rrj_write32(m, record + 16, rrj_read32(m, actor + 192));
                if ((id >> 5) == 5u && (id & 31u) >= 29u)
                {
                    int32_t speed = rrj_s32(rrj_read32(m, actor + 220)) >> 4;

                    kind = 6;
                    rrj_write32(m, record + 20, (uint32_t)race_audio_mul_asr((int16_t)rrj_u16(rrj_at(m, actor + 214, 2)), speed, 8));
                    rrj_write32(m, record + 24, (uint32_t)race_audio_mul_asr((int16_t)rrj_u16(rrj_at(m, actor + 218, 2)), speed, 8));
                    rrj_write32(m, record + 8, kind);
                }
                else
                {
                    int32_t speed = rrj_s32(rrj_read32(m, actor + 480)) >> 4;

                    kind = rrj_read32(m, record + 8);
                    rrj_write32(m, record + 20, (uint32_t)race_audio_mul_asr((int16_t)rrj_u16(rrj_at(m, actor + 450, 2)), speed, 8));
                    rrj_write32(m, record + 24, (uint32_t)race_audio_mul_asr((int16_t)rrj_u16(rrj_at(m, actor + 454, 2)), speed, 8));
                }
                if (kind == 1u)
                {
                    int32_t target;

                    if (id >> 5)
                        target = rrj_s32(rrj_read32(m, actor + 480)) >> 10;
                    else
                    {
                        int32_t value = rrj_s32(rrj_read32(m, actor + 604));

                        target = (value >> 16) + (value >> 17);
                    }
                    target += 384;
                    if (target < 704)
                        target = 704;
                    rrj_write32(m, record + 28, (uint32_t)target);
                    if (!rrj_read32(m, record + 4))
                    {
                        uint32_t sample;
                        uint32_t type = rrj_read32(m, actor + 180);

                        if (id >> 5)
                            sample = *(uint8_t *)rrj_at(m, 0x80052630u + type % 6u, 1);
                        else if (type >= 18u)
                            sample = 4;
                        else
                        {
                            uint32_t alternate = (*(uint8_t *)rrj_at(m, rrj_read32(m, actor + 1084) + 1, 1) >> 4) & 1u;

                            sample = type >= 9u ? alternate + 2u : alternate;
                        }
                        (void)sub_8001769C(m, slot, player, rrj_read32(m, 0x8005B420u), sample);
                        rrj_write32(m, record + 8, 1);
                    }
                    if ((rrj_read32(m, actor + 180) >= 18u && rrj_read32(m, body + 604) < 3u) || ((*(uint8_t *)rrj_at(m, context + 4, 1) & 1u) && (id >> 5) == 3u && !rrj_read32(m, actor + 180)))
                        (void)sub_80017F64(m, id, player, call);
                }
                kind = rrj_read32(m, record + 8);
                if (kind == 6u && !rrj_read32(m, record + 4) && rrj_s32(rrj_read32(m, 0x8005B400u)) >= 0)
                {
                    uint32_t sample = rrj_read32(m, 0x8005B400u);

                    rrj_write32(m, record + 28, sub_8001F934(m, rrj_s32(rrj_read32(m, 0x8005B420u)), sample, 0));
                    (void)sub_8001769C(m, slot, player, rrj_read32(m, 0x8005B420u), sample);
                    kind = rrj_read32(m, record + 8);
                }
                if (kind == 2u && rrj_read32(m, record + 36) < rrj_read32(m, context + 12))
                {
                    uint32_t flags_address = substituted ? actor + 572 : body + 572;

                    *(uint8_t *)rrj_at(m, flags_address, 1) &= 0xF3u;
                    (void)sub_80017814(m, slot, player, call);
                }
                if (kind >= 3u && kind <= 5u && rrj_read32(m, actor + 180) >= 18u && rrj_read32(m, body + 604) >= 3u)
                    (void)sub_80017814(m, slot, player, call);
                if (rrj_u16(rrj_at(m, record, 2)) != 224 && rrj_read32(m, record + 4))
                    (void)sub_800179D8(m, slot, player, call);
            }
        }
    }
    if (rrj_read32(m, 0x800D6BC0u) && rrj_read32(m, 0x800D6BC4u) < rrj_read32(m, context + 16))
    {
        (void)sub_8001F7EC(m, rrj_read32(m, 0x800D6BC0u), reverb);
        rrj_write32(m, 0x800D6BC0u, 0);
        if (rrj_s32(rrj_read32(m, 0x800D6BCCu)) >= 0)
        {
            rrj_write32(m, 0x800D6BC8u, 1);
            rrj_write32(m, 0x800D6AA8u + 32u * rrj_read32(m, 0x800D6BCCu), 4);
        }
    }
    if (!rrj_read32(m, 0x800D6BC8u) && rrj_read32(m, 0x8005AD48u) == 8u)
        rrj_write32(m, 0x800D6BC8u, 1);
    if (rrj_read32(m, 0x800D6BD0u) && rrj_read32(m, 0x800D6BD4u) < rrj_read32(m, context + 16))
    {
        rrj_write32(m, 0x800D6BD0u, 0);
        rrj_write32(m, 0x800D6BD4u, 0);
    }
    if (rrj_read32(m, 0x8005B424u))
    {
        if (rrj_read32(m, 0x800D6AA8u) == 2u)
        {
            uint32_t result = sub_80017BA0(m, 0, 0, 0, rrj_read32(m, 0x800D6AA0u));

            rrj_write32(m, 0x8005B424u, 0);
            return result;
        }
        return 2;
    }
    return 0;
}
