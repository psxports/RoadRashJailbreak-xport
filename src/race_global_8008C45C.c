#include "race_global_8008C45C.h"
#include "race_pause.h"
#include "spu.h"
#include "xport.h"

static void release_pointer_slot(uint32_t object)
{
    uint32_t pointer = r_u32(object + 4);
    int32_t index;

    if (pointer)
        w_u32(pointer, 0);
    index = ((int32_t)(uint32_t)(pointer - 0x800D1664u)) / 12;
    if (index < ((int32_t)(uint32_t)(r_u32(0x800D1660u))))
        w_u32(0x800D1660u, (uint32_t)index);
}

static void release_forward_pool(uint32_t identity, uint32_t base, uint32_t stride, uint32_t maximum_address, uint32_t minimum_address, uint32_t active_address, uint32_t cursor_increment)
{
    int32_t maximum = ((int32_t)(uint32_t)(r_u32(maximum_address)));
    int32_t i;

    for (i = 0; i <= maximum; ++i)
    {
        uint32_t object = base + stride * (uint32_t)i;
        uint32_t tag = r_u16(object + 172);
        uint32_t index;

        if (!tag || r_u32(object + 176) != identity)
            continue;
        w_u16(object + 320, 0);
        index = tag & 31u;
        if (((int32_t)(uint32_t)(r_u32(active_address))) > 0)
            w_u32(active_address, r_u32(active_address) - 1u);
        if (r_u32(maximum_address) == index)
        {
            int32_t next;

            do
            {
                next = ((int32_t)(uint32_t)(r_u32(maximum_address))) - 1;
                w_u32(maximum_address, (uint32_t)next);
                if (cursor_increment)
                    w_u32(0x800D1814u, r_u32(0x800D1814u) + cursor_increment);
            } while (next >= 0 && !r_u16(base + stride * (uint32_t)next + 172));
        }
        if (index < r_u32(minimum_address))
            w_u32(minimum_address, index);
        release_pointer_slot(object);
        w_u16(object + 172, 0);
    }
}

static void release_reverse_pool(uint32_t identity)
{
    uint32_t base = r_u32(0x800CE5A4u);
    int32_t maximum = ((int32_t)(uint32_t)(r_u32(0x800CE5A0u)));
    int32_t i;

    for (i = 0; i <= maximum; ++i)
    {
        uint32_t object = base - 452u * (uint32_t)i;
        uint32_t tag = r_u16(object + 172);
        uint32_t index;

        if (!tag || r_u32(object + 176) != identity)
            continue;
        w_u16(object + 320, 0);
        index = tag & 31u;
        if (((int32_t)(uint32_t)(r_u32(0x800CE598u))) > 0)
            w_u32(0x800CE598u, r_u32(0x800CE598u) - 1u);
        if (r_u32(0x800CE5A0u) == index)
        {
            int32_t next;

            do
            {
                next = ((int32_t)(uint32_t)(r_u32(0x800CE5A0u))) - 1;
                w_u32(0x800CE5A0u, (uint32_t)next);
                w_u32(0x800D1814u, r_u32(0x800D1814u) + 452u);
            } while (next >= 0 && !r_u16(base - 452u * (uint32_t)next + 172));
        }
        if (index < r_u32(0x800CE59Cu))
            w_u32(0x800CE59Cu, index);
        release_pointer_slot(object);
        w_u16(object + 172, 0);
    }
}

uint32_t sub_8008C45C(uint32_t identity, uint32_t player)
{
    uint32_t state = r_u32(0x8005B2F8u);
    uint32_t binding = 0;
    uint32_t base;
    uint32_t stride;
    int32_t remaining;

    FUNCTION_MARKER(0x8008C45C, "RASHCDG.BIN");
    if (r_u8(state + 4) & 0x10u)
    {
        binding = sub_80013360(identity, player);
        (void)sub_800A3ECC(identity, player);
    }
    if (!binding)
    {
        uint32_t count_pointer = r_u32(0x800CE4DCu);

        remaining = ((int32_t)(uint32_t)(r_u32(count_pointer)));
        base = r_u32(0x800CE4D0u);
        stride = r_u32(0x800CE4D4u);
        while (remaining >= 0)
        {
            if (r_u16(base + 172) >= r_u32(state + 48) && r_u32(base + 176) == identity && r_u16(base + 320))
                sub_80093ED4(base, 1);
            --remaining;
            base += stride;
        }

        remaining = ((int32_t)(uint32_t)(r_u32(count_pointer)));
        base = r_u32(0x800CE4D0u);
        while (remaining >= 0)
        {
            uint32_t tag = r_u16(base + 172);
            uint32_t linked;

            if ((tag >> 5) != 1 || (tag & 31u) >= r_u32(state + 48))
            {
                linked = r_u32(base + 852);
                if ((r_u32(linked + 604) - 3u) < 2u && r_u32(linked + 176) == identity && r_u16(linked + 320))
                    sub_800951B8(linked, 1);
            }
            linked = r_u32(base + 852);
            if (r_u8(linked + 572) & 0x10u)
            {
                uint32_t second = r_u32(r_u32(base + 856) + 852);

                if ((r_u32(second + 604) - 3u) < 2u && r_u32(second + 176) == identity && r_u16(second + 320))
                    sub_800951B8(second, 1);
            }
            --remaining;
            base += stride;
        }

        remaining = ((int32_t)(uint32_t)(r_u32(0x800CF658u)));
        base = 0x800CF660u;
        while (remaining >= 0)
        {
            uint32_t tag = r_u16(base + 172);

            if (tag && r_u32(base + 176) == identity)
            {
                uint32_t index = tag & 31u;

                if (!r_u32(base + 180) && ((r_u32(base + 36) >> 27) & 1u))
                {
                    (void)sub_8002847C(base);
                    (void)sub_8002820C(base);
                }
                w_u16(base + 320, 0);
                if (((int32_t)(uint32_t)(r_u32(0x800CF650u))) > 0)
                    w_u32(0x800CF650u, r_u32(0x800CF650u) - 1u);
                if (r_u32(0x800CF658u) == index)
                {
                    int32_t next;

                    do
                    {
                        next = ((int32_t)(uint32_t)(r_u32(0x800CF658u))) - 1;
                        w_u32(0x800CF658u, (uint32_t)next);
                    } while (next >= 0 && !r_u16(0x800CF660u + 512u * (uint32_t)next + 172));
                }
                if (index < r_u32(0x800CF654u))
                    w_u32(0x800CF654u, index);
            }
            --remaining;
            base += 512;
        }

        base = r_u32(0x800D4B80u);
        if (base)
        {
            remaining = ((int32_t)(uint32_t)(r_u32(0x800D4B78u)));
            while (remaining >= 0)
            {
                uint32_t tag = r_u16(base + 172);

                if (tag && r_u32(base + 176) == identity)
                {
                    uint32_t index = tag & 31u;

                    w_u16(base + 320, 0);
                    (void)sub_800CC0B0(base);
                    if (((int32_t)(uint32_t)(r_u32(0x800D4B70u))) > 0)
                        w_u32(0x800D4B70u, r_u32(0x800D4B70u) - 1u);
                    if (r_u32(0x800D4B78u) == index)
                    {
                        int32_t next;

                        do
                        {
                            next = ((int32_t)(uint32_t)(r_u32(0x800D4B78u))) - 1;
                            w_u32(0x800D4B78u, (uint32_t)next);
                        } while (next >= 0 && !r_u16(r_u32(0x800D4B80u) + 572u * (uint32_t)next + 172));
                    }
                    if (index < r_u32(0x800D4B74u))
                        w_u32(0x800D4B74u, index);
                    w_u16(base + 172, 0);
                }
                --remaining;
                base += 572;
            }
        }
        release_forward_pool(identity, r_u32(0x800CD6D4u), 596, 0x800CD6D0u, 0x800CD6CCu, 0x800CD6C8u, 596);
        release_reverse_pool(identity);
    }

    base = r_u32(0x800CD6C4u);
    if (base)
    {
        remaining = ((int32_t)(uint32_t)(r_u32(0x800CD6B0u)));
        while (remaining >= 0)
        {
            uint32_t tag = r_u16(base);

            if (tag && r_u32(base + 4) == identity)
            {
                uint32_t mask = r_u8(base + 3);
                uint32_t bit = 1u << (player & 31u);

                if (mask & bit)
                {
                    if (mask == (bit & 0xFFu))
                    {
                        uint32_t index = tag & 31u;

                        if (binding)
                            (void)sub_8009C41C(binding, 6, r_u8(base + 2), 0);
                        w_u16(base + 148, 0);
                        if (((int32_t)(uint32_t)(r_u32(0x800CD6A8u))) > 0)
                            w_u32(0x800CD6A8u, r_u32(0x800CD6A8u) - 1u);
                        if (r_u32(0x800CD6B0u) == index)
                        {
                            int32_t next;

                            do
                            {
                                next = ((int32_t)(uint32_t)(r_u32(0x800CD6B0u))) - 1;
                                w_u32(0x800CD6B0u, (uint32_t)next);
                            } while (next >= 0 && !r_u16(r_u32(0x800CD6C4u) + 280u * (uint32_t)next));
                        }
                        if (index < r_u32(0x800CD6ACu))
                            w_u32(0x800CD6ACu, index);
                        w_u16(base, 0);
                    }
                    else
                        w_u8(base + 3, (uint8_t)(mask & (1u << ((player ^ 1u) & 31u))));
                }
            }
            --remaining;
            base += 280;
        }
    }
    return base;
}

uint32_t sub_80032810(uint32_t identity, uint32_t kind, uint32_t group)
{
    int32_t index = ((int32_t)(uint32_t)(group * 12u));
    int32_t limit = ((int32_t)(uint32_t)(group * 12u + 12u));
    uint32_t offset = group * 1344u;

    FUNCTION_MARKER(0x80032810, "SLUS_010.53");
    while (index < limit)
    {
        uint32_t record = 0x800D87E8u + offset;

        if (r_u32(record + 8) == identity)
        {
            uint32_t slot;

            if (kind == 9)
            {
                w_u32(r_u32(record + 4) + 60, 0);
                w_u32(record + 12, r_u32(record + 12) & 0xFFFFFFA9u);
            }
            else
            {
                (void)sub_8008C45C(identity, group);
                w_u32(record + 4, 0);
                w_u32(record + 8, 0xFFFFFFFFu);
                w_u32(record + 12, 0);
                w_u32(record + 56, 0);
                w_u32(record, 0xFFFFFFFFu);
                w_u32(record + 76, 0);
                for (slot = 0; slot < 2; ++slot)
                {
                    uint32_t field = slot * 4u;

                    w_u32(record + field + 88, 0xFFFFFFFFu);
                    w_u32(record + field + 80, 0xFFFFFFFFu);
                    w_u32(record + field + 96, 0);
                    w_u32(record + field + 104, 0);
                }
                w_u32(0x8005AEDCu, r_u32(0x8005AEDCu) - 1u);
                return 1;
            }
        }
        offset += 112;
        ++index;
    }
    return 0;
}
