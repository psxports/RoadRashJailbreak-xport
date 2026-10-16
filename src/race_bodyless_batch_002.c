#include "rrj_debug.h"
#include "vblank.h"
#include <stdlib.h>
#include <stdio.h>
#include "native_game_api.h"
#include "sdk_build_stubs.h"
#include "psx.h"
#include "rrj_cd_transport.h"
#include "race_bodyless_batch_002.h"

/* FUNCTION_MARKER: SLUS_010.53:0x800202B8 sub_800202B8 */
uint32_t sub_800202B8(uint32_t long_table, uint32_t short_table, uint32_t values)
{
    uint32_t row, value, bits, prefix, copies, index, slot;
    FUNCTION_MARKER(0x800202B8, "SLUS_010.53");
    w_u32(0x8005ADAC, long_table);
    w_u32(0x8005ADB0, short_table);
    if (!long_table || !short_table)
        return UINT32_MAX;
    if (!values)
        values = 0x80052C20;
    for (row = 0; row < 96; ++row)
    {
        value = r_u16(values);
        values += 2;
        bits = r_u8(0x800528A0 + row * 4);
        prefix = r_u16(0x800528A2 + row * 4) >> 3;
        copies = 1u << ((13 - bits) & 31);
        if ((int32_t)copies <= 0)
            continue;
        for (index = 0; index < copies; ++index)
        {
            slot = prefix | index;
            w_u8(long_table + slot, bits);
            w_u16(long_table + 0x2000 + (slot << 1), value);
        }
    }
    for (row = 0; row < 128; ++row)
    {
        value = r_u16(values);
        values += 2;
        bits = r_u8(0x80052A20 + row * 4);
        prefix = r_u16(0x80052A22 + row * 4) >> 7;
        copies = 1u << ((9 - bits) & 31);
        if ((int32_t)copies <= 0)
            continue;
        for (index = 0; index < copies; ++index)
        {
            slot = prefix | index;
            w_u8(short_table + slot, bits);
            w_u16(short_table + 512 + (slot << 1), value);
        }
    }
    return 0;
}

/* FUNCTION_MARKER: SLUS_010.53:0x8002026C sub_8002026C */
void sub_8002026C(uint32_t values)
{
    FUNCTION_MARKER(0x8002026Cu, "SLUS_010.53");
    uint32_t long_table = r_u32(0x8005ADB4u), short_table;
    if (!long_table)
        return;
    short_table = r_u32(0x8005ADB8u);
    if (!short_table || !r_u32(0x8005ADBCu))
        return;
    (void)sub_800202B8(long_table, short_table, values);
}


uint32_t sub_80047580(void *destination, uint32_t words)
{
    FUNCTION_MARKER(0x80047580, "SLUS_010.53");
    /* Delegate CD FIFO transfer and completion to the existing host SDK */
    if (words > 585 || !rrj_cd_fifo_read(destination, words))
    {
        RRJ_DEBUG_FPRINTF(stderr, "RRJ CD sector FIFO transfer failed: words=%u\n", words);
        abort();
    }
    return 0;
}

uint32_t sub_80047514(uint32_t mode)
{
    FUNCTION_MARKER(0x80047514u, "SLUS_010.53");
    (void)sub_80046844(0x8005B1E4u);
    for (;;)
    {
        if (sub_8004687C())
            return UINT32_MAX;
        if (!(r_u32(r_u32(0x8005B1E0u)) & 0x01000000u))
            return 0;
        if (mode)
            return 1;
    }
}

uint32_t sub_80015724(void)
{
    FUNCTION_MARKER(0x80015724u, "SLUS_010.53");
    w_u32(0x8005B3CCu, 1);
    return 1;
}

void sub_80044FE4(uint32_t mode)
{
    FUNCTION_MARKER(0x80044FE4u, "SLUS_010.53");
    rrj_missing_bios_b0_4a(mode);
}

void sub_80044FF4(void)
{
    FUNCTION_MARKER(0x80044FF4u, "SLUS_010.53");
    rrj_missing_bios_b0_4b();
}

uint32_t sub_80040CB4(void)
{
    uint32_t interrupt, owner, target;
    RRJ_PAD_FUNCTION function;
    FUNCTION_MARKER(0x80040CB4, "SLUS_010.53");
    w_u32(0x80054934, 0);
    sub_80043DA4();
    (void)rrj_missing_sys_deq_int_rp(2, 0x800D9CB0);
    (void)rrj_missing_sys_enq_int_rp(2, 0x800D9CB0);
    interrupt = r_u32(0x8005495C);
    w_u32(interrupt, 0xFFFFFFFE);
    w_u32(interrupt + 4, r_u32(interrupt + 4) | 1);
    (void)ChangeClearRCnt(3, 0);
    sub_80043DB4();
    owner = r_u32(0x80054930);
    target = r_u32(0x80054900);
    function = rrj_pad_function(target);
    (void)function(owner);
    owner = r_u32(0x80054930);
    target = r_u32(0x80054900);
    function = rrj_pad_function(target);
    (void)function(owner + 240);
    w_u32(0x800D9CC4, 0);
    w_u32(0x800D9CC0, 0);
    w_u32(0x80054934, 1);
    return 1;
}

uint32_t sub_80042334(uint32_t owner)
{
    uint32_t result, buffer;
    RRJ_PAD_FUNCTION function;
    FUNCTION_MARKER(0x80042334, "SLUS_010.53");
    function = rrj_pad_function(r_u32(0x80054914));
    result = function(owner);
    buffer = r_u32(owner + 60);
    w_u32(0x8005497C, result);
    w_u8(buffer, 0);
    return sub_800411E8(owner, -2);
}

uint32_t sub_8001B500(void)
{
    uint32_t target;
    uint32_t result;
    RRJ_VBLANK_FUNCTION handler;

    FUNCTION_MARKER(0x8001B500, "SLUS_010.53");
    target = r_u32(0x8005B448);
    if (target)
    {
        handler = rrj_vblank_function(target);
        (void)handler();
    }
    result = 1 - r_u32(0x8005B44C);
    w_u32(0x8005B44C, result);
    return result;
}

uint32_t sub_8001DF64(void)
{
    FUNCTION_MARKER(0x8001DF64u, "SLUS_010.53");
    if (r_u32(0x800527D8u) == 0xFFFFu)
    {
        w_u32(0x800527D8u, 0);
        w_u32(0x800527DCu, r_u32(0x800527DCu) + 1u);
    }
    else
        w_u32(0x800527D8u, r_u32(0x800527D8u) + 1u);
    return 0;
}

uint32_t sub_8001E0DC(uint32_t destination, uint32_t value, uint32_t bytes)
{
    uint32_t cursor = destination;
    FUNCTION_MARKER(0x8001E0DCu, "SLUS_010.53");
    while (bytes)
    {
        w_u8(cursor, (uint8_t)value);
        --bytes;
        ++cursor;
    }
    return destination;
}

uint32_t sub_8001B6A8(void)
{
    uint32_t state;
    FUNCTION_MARKER(0x8001B6A8u, "SLUS_010.53");
    state = r_u32(0x8005B2F8u);
    w_u32(0x8005B458u, 0);
    w_u32(0x8005AD8Cu, 0);
    w_u32(state + 12u, 0);
    w_u32(state + 16u, 0);
    return state;
}

uint32_t sub_800201C0(void)
{
    FUNCTION_MARKER(0x800201C0u, "SLUS_010.53");
    uint32_t enabled = r_u32(0x8005ADBCu), first, second;
    if (enabled)
        return enabled;
    first = sub_8001447C(0x6000u, 0);
    w_u32(0x8005ADB4u, first);
    second = sub_8001447C(0x600u, 0);
    first = r_u32(0x8005ADB4u);
    w_u32(0x8005ADB8u, second);
    if (!first || !second)
        return sub_80020228();
    w_u32(0x8005ADBCu, 1);
    return 1;
}

void sub_8001FC84(uint32_t value)
{
    FUNCTION_MARKER(0x8001FC84u, "SLUS_010.53");
    w_u32(0x8005B4A8u, value);
}

uint32_t sub_8001EFDC(uint32_t value)
{
    FUNCTION_MARKER(0x8001EFDCu, "SLUS_010.53");
    w_u32(0x800D69F4u, value);
    return 0x800D0000u;
}

RRJ_PAD_FUNCTION rrj_pad_function(uint32_t address)
{
    switch (address)
    {
        case 0x80042B9C:
            return sub_80042B9C;
        case 0x80043218:
            return sub_80043218;
        case 0x80041750:
            return sub_80041750;
        case 0x80043174:
            return sub_80043174;
        case 0x80043BBC:
            return sub_80043BBC;
        default:
            /* TODO Bind additional original PAD callback targets */
            RRJ_DEBUG_FPRINTF(stderr, "RRJ: unresolved PAD callback %08X\n", address);
            abort();
    }
}

uint32_t sub_80042B9C(uint32_t owner)
{
    uint32_t index;
    FUNCTION_MARKER(0x80042B9C, "SLUS_010.53");
    if (!r_u8(owner + 73))
        return 0;
    w_u8(owner + 73, 0);
    w_u8(owner + 70, 0);
    w_u16(owner + 230, 0);
    w_u32(owner + 20, 0);
    w_u32(owner + 24, 0);
    w_u8(owner + 227, 0);
    w_u8(owner + 228, 0);
    w_u16(owner + 230, 0);
    w_u8(owner + 233, 0);
    w_u8(owner + 234, 0);
    w_u32(owner, 0);
    w_u32(owner + 4, 0);
    w_u32(owner + 8, 0);
    w_u8(owner + 55, 0);
    w_u8(owner + 56, 0);
    w_u8(owner + 57, 0);
    for (index = 0; index < 6; ++index)
        w_u8(owner + 93 + index, 255);
    return UINT32_MAX;
}

uint32_t sub_80043218(uint32_t owner)
{
    uint32_t value, child, index, target, state;
    RRJ_PAD_FUNCTION function;
    FUNCTION_MARKER(0x80043218, "SLUS_010.53");
    if (!r_u32(owner + 12))
    {
        value = r_u8(owner + 56);
        if (value)
        {
            if (!r_u32(0x800549A4 + (r_u32(0x8005493C) << 2)))
                return 0;
            if (value == 0x43)
                return 0;
            w_u8(owner + 55, value);
            w_u8(owner + 56, 0);
            return 0;
        }
        child = r_u32(r_u32(owner + 16) + 12) + 44;
        for (index = 0; index < 4; ++index, child += 240)
            if ((r_u8(child + 11) == 0x43 || r_u8(child + 12) == 0x43) && r_u8(r_u32(child)) == 1)
                return 0;
    }
    if (r_u8(r_u32(owner + 60)) == 0xF3)
    {
        if (!r_u8(owner + 232) || (r_u8(owner + 70) == 255 && r_u8(owner + 73) != 2))
        {
            (void)sub_80042288(owner, 0);
            return 0;
        }
        if (r_u8(r_u32(owner + 60)) == 0xF3 && r_u8(owner + 73) == 2 && r_u8(owner + 232) != 8)
        {
            function = rrj_pad_function(r_u32(0x80054900));
            (void)function(owner);
            return 0;
        }
    }
    state = r_u8(owner + 70);
    switch (state)
    {
        case 0:
            return 0;
        case 1:
            (void)sub_80042288(owner, 1);
            return 0;
        case 255:
            if (r_u8(owner + 232) != 8)
                return 0;
            child = r_u32(owner + 12);
            if (!child)
                return 0;
            for (index = 0; index < 4; ++index, child += 240)
                if (r_u8(child + 70) == 1)
                {
                    (void)sub_80043218(child);
                    return 0;
                }
            return 1;
        case 254:
            if (r_u8(owner + 73) == 2)
            {
                w_u8(owner + 55, 0);
                return 0;
            }
            (void)sub_80042288(owner, 0);
            return 0;
        default:
            target = r_u32(owner + 20);
            if (target)
            {
                function = rrj_pad_function(target);
                (void)function(owner);
            }
            else
                (void)sub_80041750(owner);
            return 0;
    }
}

uint32_t sub_80042288(uint32_t owner, uint32_t mode)
{
    FUNCTION_MARKER(0x80042288, "SLUS_010.53");
    w_u8(owner + 55, 0x43);
    w_u32(owner + 44, owner + 36);
    w_u8(owner + 36, mode);
    w_u8(owner + 54, 1);
    return 1;
}

uint32_t sub_800422A8(uint32_t owner)
{
    FUNCTION_MARKER(0x800422A8, "SLUS_010.53");
    w_u8(owner + 55, 0x45);
    w_u32(owner + 44, 0);
    w_u8(owner + 54, 0);
    return 0x45;
}

uint32_t sub_800422BC(uint32_t owner, uint32_t mode)
{
    FUNCTION_MARKER(0x800422BC, "SLUS_010.53");
    w_u8(owner + 55, 0x4C);
    w_u32(owner + 44, owner + 36);
    w_u8(owner + 36, mode);
    w_u8(owner + 54, 1);
    return 1;
}

uint32_t sub_800422FC(uint32_t owner, uint32_t mode)
{
    FUNCTION_MARKER(0x800422FC, "SLUS_010.53");
    w_u8(owner + 55, 0x47);
    w_u32(owner + 44, owner + 36);
    w_u8(owner + 36, mode);
    w_u8(owner + 54, 1);
    return 1;
}

uint32_t sub_80041750(uint32_t owner)
{
    uint32_t state;
    FUNCTION_MARKER(0x80041750, "SLUS_010.53");
    state = r_u8(owner + 70);
    if (state == 3)
        return sub_800422BC(owner, r_u8(owner + 228));
    if (state < 4)
        return state == 2 ? sub_800422A8(owner) : 2;
    return state == 4 ? sub_800422FC(owner, r_u8(owner + 71)) : 4;
}

uint32_t sub_80043C94(void)
{
    uint32_t current, mode, start, limit, elapsed;
    FUNCTION_MARKER(0x80043C94, "SLUS_010.53");
    current = r_u16(0x1F801120);
    if (current < r_u32(0x800DA6D0))
    {
        if (r_u16(0x1F801128))
            current += r_u16(0x1F801128);
        else
            current += 0x10000;
    }
    mode = r_u16(0x1F801124);
    start = r_u32(0x800DA6D0);
    limit = r_u32(0x800DA6D4);
    elapsed = current - start;
    if (!(mode & 0x200))
        elapsed >>= 3;
    return elapsed >= limit;
}

uint32_t sub_800411E8(uint32_t owner, int32_t command)
{
    uint32_t serial, buffer, received, control, status, interrupt, tick, cursor, next;
    FUNCTION_MARKER(0x800411E8, "SLUS_010.53");
    if (command < 0)
    {
        serial = r_u32(0x80054960);
        buffer = r_u32(owner + 64);
        received = r_u8(serial);
        w_u8(owner + 68, 255);
        w_u8(owner + 69, 1);
        w_u8(buffer, (uint8_t)~command);
        serial = r_u32(0x80054960);
        while (!(r_u16(serial + 4) & 1))
            ;
        while (!sub_80043C94())
            ;
        serial = r_u32(0x80054960);
        w_u8(serial, (uint8_t)~command);
        return received;
    }
    control = 0x88;
    if ((r_u8(r_u32(owner + 60)) >> 4) == 8 && r_u8(owner + 68) >= 9)
        control = 0x22;
    serial = r_u32(0x80054960);
    tick = r_u16(0x1F801120);
    status = r_u16(serial + 4);
    w_u32(0x800DA6D4, 430);
    w_u32(0x800DA6D0, tick);
    if (!(status & 2))
        while (!(r_u16(serial + 4) & 2))
            ;
    serial = r_u32(0x80054960);
    interrupt = r_u32(0x8005495C);
    received = r_u8(serial);
    w_u16(serial + 14, control);
    if (!(r_u32(interrupt) & 0x80))
        for (;;)
        {
            if (sub_80043C94())
                return 0xFFFFFFEC;
            interrupt = r_u32(0x8005495C);
            if (r_u32(interrupt) & 0x80)
                break;
        }
    serial = r_u32(0x80054960);
    w_u8(serial, command);
    if (control == 0x22)
    {
        interrupt = r_u32(0x8005495C);
        serial = r_u32(0x80054960);
        w_u32(interrupt, 0xFFFFFF7F);
        w_u16(serial + 10, r_u16(serial + 10) | 0x10);
    }
    next = r_u8(owner + 69);
    cursor = r_u8(owner + 68);
    buffer = r_u32(owner + 60);
    w_u8(owner + 69, next + 1);
    w_u8(buffer + cursor, received);
    w_u8(owner + 68, r_u8(owner + 68) + 1);
    return received;
}

uint32_t sub_80043174(uint32_t port)
{
    uint32_t owner, slot;
    FUNCTION_MARKER(0x80043174, "SLUS_010.53");
    owner = (port & 0xF0) ? 0x800D9E60 : 0x800D9D70;
    if (r_u32(0x80054948))
    {
        slot = port & 3;
        if ((r_u8(owner + 232) == 8 && !(port & 15)) || slot)
            return r_u32(owner + 12) + 240 * slot;
    }
    return owner;
}

uint32_t sub_80043BBC(uint32_t owner)
{
    uint32_t child, index, state;
    FUNCTION_MARKER(0x80043BBC, "SLUS_010.53");
    if (!r_u16(owner + 230))
        return 1;
    if (r_u32(owner + 12))
    {
        if (r_u8(owner + 70) != 255)
            return 1;
        child = r_u32(r_u32(owner + 16) + 12);
        for (index = 0; index < 4; ++index)
        {
            state = r_u8(child + 70);
            if (state != 255 && state)
                return 1;
            child += 240;
        }
        return 0;
    }
    if (r_u8(r_u32(owner + 16) + 70) != 255)
        return 1;
    return r_u8(owner + 70) == 255 ? 0 : 1;
}

uint32_t sub_8004213C(uint32_t owner, uint32_t mode, uint32_t value)
{
    uint32_t old_mode;
    FUNCTION_MARKER(0x8004213C, "SLUS_010.53");
    if (rrj_pad_function(r_u32(0x80054918))(owner))
        return 0;
    old_mode = r_u8(owner + 228);
    w_u8(owner + 70, 1);
    w_u32(owner + 20, 0x800421D4);
    w_u32(owner + 24, 0x80042228);
    w_u8(owner + 81, mode);
    w_u8(owner + 82, value);
    w_u8(owner + 83, (mode & 255) == old_mode);
    return 1;
}
