#include "psx.h"
/* Literal RRJ audio bookkeeping. SPU output remains a required SDK binding;
 * this module does not silently discard audio requests. See original MIPS. */
#include "audio.h"

static uint32_t merge_mask(uint32_t a, uint32_t mask)
{
    uint32_t v = r_u32(a) | mask;
    w_u32(a, v);
    return v;
}

/* Verified twin: same seven words, only field offset +0x14/+0x18 changes. */
uint32_t sub_8001EB28(uint32_t mask)
{
    FUNCTION_MARKER(0x8001EB28u, "SLUS_010.53");
    return merge_mask(0x800D6884, mask);
}

uint32_t sub_8001EB44(uint32_t mask)
{
    FUNCTION_MARKER(0x8001EB44u, "SLUS_010.53");
    return merge_mask(0x800D6888, mask);
}

uint32_t sub_8001E86C(uint32_t bank, uint32_t index)
{
    FUNCTION_MARKER(0x8001E86Cu, "SLUS_010.53");
    uint32_t count = r_u8(bank + 4), offset;
    if (((int32_t)(uint32_t)(index)) < 0 || ((int32_t)(uint32_t)(index)) >= (int32_t)count)
        return 0;
    offset = r_u32(bank + 16 + index * 4);
    return offset ? bank + offset : 0;
}

uint32_t sub_8001F9C4(uint32_t reserved)
{
    FUNCTION_MARKER(0x8001F9C4u, "SLUS_010.53");
    uint32_t top = r_u32(0x800D69E4), slot = 0xffffffff, a, next, voice, sequence, i;
    if (((int32_t)(uint32_t)(top)) >= 0)
    {
        a = 0x800D6920 + top * 4;
        slot = r_u32(a);
        w_u32(a, 0xffffffff);
        w_u32(0x800D69E4, r_u32(0x800D69E4) - 1);
    }
    else
    {
        uint32_t head = r_u32(0x800D69E8), tail = r_u32(0x800D69EC);
        if (head != tail)
        {
            a = 0x800D6980 + tail * 4;
            slot = r_u32(a);
            w_u32(a, 0xffffffff);
            next = r_u32(0x800D69EC) + 1;
            w_u32(0x800D69EC, next);
            w_u32(0x800D69EC, ((int32_t)(uint32_t)(next)) < 25 ? next : 0);
            sub_8001EB44(1u << (slot & 31));
        }
    }
    if (slot == 0xffffffff)
        return 0;
    voice = r_u32(0x800D687C) + slot * 44;
    if (reserved)
    {
        for (i = 0; i < 22; ++i)
            if (r_u32(0x800D68C8 + i * 4) == 0xffffffff)
            {
                w_u32(0x800D68C8 + i * 4, slot);
                break;
            }
    }
    else
    {
        next = r_u32(0x800D69E8);
        w_u32(0x800D6980 + next * 4, slot);
        ++next;
        w_u32(0x800D69E8, next);
        w_u32(0x800D69E8, ((int32_t)(uint32_t)(next)) < 25 ? next : 0);
    }
    sequence = (r_u32(0x8005B4A0) + 1) & 0x07ffffff;
    w_u32(0x8005B4A0, sequence);
    if (!sequence)
    {
        sequence = 1;
        w_u32(0x8005B4A0, 1);
    }
    w_u32(voice + 4, sequence);
    w_u32(voice + 16, 1);
    return voice;
}

/* Five original arguments, not IDA's nine. parameters is a packed three-word
 * record: pitch, volume, pan. Read pitch after allocation, preserving aliasing. */
uint32_t sub_8001F174(uint32_t bank, uint32_t sample, uint32_t loop, uint32_t reserved, const void *parameters)
{
    uint32_t scalar_v_2;

    FUNCTION_MARKER(0x8001F174u, "SLUS_010.53");
    const uint8_t *p = (const uint8_t *)parameters;
    uint32_t base, record, left, right, voice, pitch, channel, handle;
    if (((int32_t)(uint32_t)(r_u32(0x800D6870))) < ((int32_t)(uint32_t)(bank)) || ((int32_t)(uint32_t)(bank)) < 0)
        return 0;
    base = r_u32(r_u32(0x800D6874) + bank * 4);
    if (!base)
        return 0;
    record = sub_8001E86C(base, sample);
    if (!record)
        return 0;
    record += 4; /* Nonzero third argument is a flag; original resets its index. */
    if (record & 3)
        return 0;
    if (r_u32(0x800D69F4))
    {
        left = xport_load_le32(p + 4) * r_u8(record);
        right = left;
    }
    else
    {
        uint32_t pan = xport_load_le32(p + 8), folded = ((int32_t)(uint32_t)(pan)) < 129 ? pan : 256 - pan;
        uint32_t volume = xport_load_le32(p + 4) * r_u8(record);
        uint32_t product = (folded - 64) * volume, shifted = ((uint32_t)((((uint32_t)(product)) >> ((unsigned)(6))) | ((((uint32_t)(product)) & 0x80000000) ? (~0u << (32 - ((unsigned)(6)))) : 0)));
        left = (volume - (shifted & ((scalar_v_2 = (0 - shifted)), (((uint32_t)((scalar_v_2 >> ((unsigned)(31))) | ((scalar_v_2 & 0x80000000) ? (~0u << (32 - ((unsigned)(31)))) : 0))))))) ^ ((uint32_t)((((uint32_t)(pan)) >> ((unsigned)(31))) | ((((uint32_t)(pan)) & 0x80000000) ? (~0u << (32 - ((unsigned)(31)))) : 0)));
        if (((int32_t)(uint32_t)(pan)) >= 129)
            left = 0 - left;
        right = volume + (shifted & ((uint32_t)((((uint32_t)(product)) >> ((unsigned)(31))) | ((((uint32_t)(product)) & 0x80000000) ? (~0u << (32 - ((unsigned)(31)))) : 0))));
    }
    voice = sub_8001F9C4(reserved);
    if (!voice)
        return 0;
    w_u32(voice + 8, record);
    pitch = xport_load_le32(p);
    w_u32(voice + 32, pitch == 0xffffffff ? r_u16(record + 6) : pitch);
    w_u32(voice + 36, left);
    w_u32(voice + 40, right);
    if (loop)
        (void)SpuSetReverbVoice(0, 1u << (r_u32(voice + 28) & 31));
    channel = r_u32(voice + 28);
    handle = (channel << 27) + (r_u32(voice + 4) & 0x07ffffff);
    sub_8001EB28(1u << (channel & 31));
    return handle;
}

uint32_t sub_8007EAC0(uint32_t event)
{
    FUNCTION_MARKER(0x8007EAC0u, "RASHCDF.BIN");
    uint8_t parameters[12];
    if (event >= 15)
        return 0;
    xport_store_le32(parameters, 0xffffffff);
    xport_store_le32(parameters + 4, r_u32(0x800D6C0C));
    xport_store_le32(parameters + 8, 64);
    return sub_8001F174(r_u32(0x800A0854), event, 0, 0, parameters);
}
