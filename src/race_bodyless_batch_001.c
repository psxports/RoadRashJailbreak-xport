#include "rrj_debug.h"
#include <stdio.h>
#include "native_file_callbacks.h"
#include "sdk_build_stubs.h"
#include "native_game_api.h"
#include "psx.h"
#include "race_bodyless_batch_001.h"
#include "race_bodyless_batch_002.h"

uint32_t sub_80047430(void)
{
    FUNCTION_MARKER(0x80047430, "SLUS_010.53");
    (void)RRJ_DEBUG_PUTS((const char *)xport_guest_cptr(0x8005B1B8, 1));
    (void)RRJ_DEBUG_PRINTF((const char *)xport_guest_cptr(0x8005B1C4, 1), (unsigned int)0x80054D74);
    w_u8(0x8005AF7D, 0);
    w_u8(0x8005AF7C, 0);
    (void)sub_800473F0();
    (void)sub_800472B4();
    (void)sub_80047020(1, NULL, 0, 0);
    if (r_u32(0x8005AF6C) & 0x10)
        (void)sub_80047020(1, NULL, 0, 0);
    if (sub_80047020(10, NULL, 0, 0))
        return UINT32_MAX;
    if (sub_80047020(12, NULL, 0, 0))
        return UINT32_MAX;
    return sub_80046DD4(0, 0) == 2 ? 0 : UINT32_MAX;
}

uint32_t sub_800473F0(void)
{
    FUNCTION_MARKER(0x800473F0, "SLUS_010.53");
    w_u32(0x8005AF64, 0);
    w_u32(0x8005AF60, 0);
    w_u32(0x8005AF70, 0);
    w_u32(0x8005AF6C, 0);
    (void)ResetCallback();
    return rrj_cd_irq_callback_psx();
}

uint32_t sub_800472B4(void)
{
    uint32_t port, value;
    FUNCTION_MARKER(0x800472B4u, "SLUS_010.53");
    w_u8(r_u32(0x8005B0C0u), 1);
    value = r_u8(r_u32(0x8005B0CCu)) & 7u;
    while (value)
    {
        w_u8(r_u32(0x8005B0C0u), 1);
        w_u8(r_u32(0x8005B0CCu), 7);
        w_u8(r_u32(0x8005B0C8u), 7);
        value = r_u8(r_u32(0x8005B0CCu)) & 7u;
    }
    w_u8(0x8005B0DAu, 0);
    value = r_u8(0x8005B0DAu);
    port = r_u32(0x8005B0C0u);
    w_u8(0x8005B0D9u, value);
    w_u8(0x8005B0D8u, 2);
    w_u8(port, 0);
    w_u8(r_u32(0x8005B0CCu), 0);
    w_u32(r_u32(0x8005B0D0u), 0x1325u);
    return 0x1325u;
}

uint32_t sub_80047020(uint32_t command, const uint8_t *parameter, uint32_t destination, uint32_t asynchronous)
{
    FUNCTION_MARKER(0x80047020, "SLUS_010.53");
    return (uint32_t)CD_cw((uint8_t)command, parameter,
                          destination ? (uint8_t *)xport_guest_ptr(destination, 8) : NULL, asynchronous);
}

uint32_t sub_80046DD4(uint32_t mode, uint32_t destination)
{
    uint32_t event;
    FUNCTION_MARKER(0x80046DD4, "SLUS_010.53");
    (void)sub_80046844(0x8005B14C);
    do
    {
        if (sub_8004687C())
            return UINT32_MAX;
        (void)sub_80046A00();
        event = r_u8(0x8005B0D8);
        if (event == 2 || event == 5)
        {
            w_u8(0x8005B0D8, 2);
            sub_800467F4(destination, xport_guest_cptr(0x8005B5B8, 8));
            return event;
        }
    } while (!mode);
    return 0;
}

uint32_t sub_80046844(uint32_t message)
{
    uint32_t result;
    FUNCTION_MARKER(0x80046844u, "SLUS_010.53");
    result = (uint32_t)VSync(-1);
    w_u32(0x8005B5E0u, message);
    result += 960u;
    w_u32(0x8005B5D8u, result);
    w_u32(0x8005B5DCu, 0);
    return result;
}

uint32_t sub_8004687C(void)
{
    uint32_t now, previous, sync_index, ready_index, command_index;
    uint32_t ready, message, command, sync;
    FUNCTION_MARKER(0x8004687Cu, "SLUS_010.53");
    now = (uint32_t)VSync(-1);
    if (((int32_t)(uint32_t)(r_u32(0x8005B5D8u))) >= ((int32_t)(uint32_t)(now)))
    {
        previous = r_u32(0x8005B5DCu);
        w_u32(0x8005B5DCu, previous + 1u);
        if (((int32_t)(uint32_t)(previous)) <= 0x3C0000)
            return 0;
    }
    (void)RRJ_DEBUG_FPUTS((const char *)xport_guest_cptr(0x8005B0DCu, 13), stdout);
    sync_index = r_u8(0x8005B0D8u);
    ready_index = r_u8(0x8005B0D9u);
    command_index = r_u8(0x8005AF7Du);
    ready = r_u32(0x80054B54u + (ready_index << 2));
    message = r_u32(0x8005B5E0u);
    command = r_u32(0x80054AD4u + (command_index << 2));
    sync = r_u32(0x80054B54u + (sync_index << 2));
    (void)RRJ_DEBUG_PRINTF((const char *)xport_guest_cptr(0x8005B0ECu, 27), (const char *)xport_guest_cptr(message, 1), (const char *)xport_guest_cptr(command, 1), (const char *)xport_guest_cptr(sync, 1), (const char *)xport_guest_cptr(ready, 1));
    (void)sub_800472B4();
    return UINT32_MAX;
}

uint32_t sub_80046A00(void)
{
    uint32_t result;
    FUNCTION_MARKER(0x80046A00, "SLUS_010.53");
    result = sub_80047A88();
    if (result)
        result = sub_80046948();
    return result;
}

void sub_800467F4(uint32_t destination, const uint8_t source[8])
{
    uint32_t index;
    FUNCTION_MARKER(0x800467F4u, "SLUS_010.53");
    if (destination)
        for (index = 0; index < 8; ++index)
            w_u8(destination + index, source[index]);
}

uint32_t sub_80046948(void)
{
    uint32_t saved_bank, flags, target, port;
    RRJ_CD_CALLBACK callback;
    FUNCTION_MARKER(0x80046948, "SLUS_010.53");
    saved_bank = r_u8(r_u32(0x8005B0C0)) & 3;
    for (;;)
    {
        flags = sub_80046A30();
        if (!flags)
            break;
        if (flags & 4)
        {
            target = r_u32(0x8005AF64);
            if (target)
            {
                callback = rrj_cd_callback(target);
                (void)callback(r_u8(0x8005B0D9), 0x8005B5C0);
            }
        }
        if (flags & 2)
        {
            target = r_u32(0x8005AF60);
            if (target)
            {
                callback = rrj_cd_callback(target);
                (void)callback(r_u8(0x8005B0D8), 0x8005B5B8);
            }
        }
    }
    port = r_u32(0x8005B0C0);
    w_u8(port, saved_bank);
    return port;
}

uint32_t sub_80046A30(void)
{
    uint32_t interrupt_port, interrupt, observed, count = 0, index, error = 0;
    uint32_t command, status, code;
    uint8_t response[8];
    FUNCTION_MARKER(0x80046A30, "SLUS_010.53");
    w_u8(r_u32(0x8005B0C0), 1);
    interrupt_port = r_u32(0x8005B0CC);
    interrupt = r_u8(interrupt_port) & 7;
    if (!interrupt)
        return 0;
    for (;;)
    {
        observed = r_u8(interrupt_port) & 7;
        if (interrupt == observed)
            break;
        interrupt = r_u8(interrupt_port) & 7;
    }
    do
    {
        if (!(r_u8(r_u32(0x8005B0C0)) & 0x20))
            break;
        response[count++] = r_u8(r_u32(0x8005B0C4));
    } while (count < 8);
    for (index = count; index < 8; ++index)
        response[index] = 0;
    w_u8(r_u32(0x8005B0C0), 1);
    w_u8(r_u32(0x8005B0CC), 7);
    w_u8(r_u32(0x8005B0C8), 7);
    if (interrupt != 3 || r_u32(0x80054B74 + 256 + 4 * r_u8(0x8005AF7D)))
    {
        if (!(r_u32(0x8005AF6C) & 0x10) && (response[0] & 0x10))
            w_u32(0x8005AF74, r_u32(0x8005AF74) + 1);
        error = response[0] & 0x1D;
        w_u32(0x8005AF6C, response[0]);
        w_u32(0x8005AF70, response[1]);
    }
    if (interrupt == 5 && r_s32(0x8005AF68) >= 3)
    {
        (void)RRJ_DEBUG_PRINTF((const char *)xport_guest_cptr(0x8005B108, 1));
        if (r_s32(0x8005AF68) >= 3)
        {
            index = r_u8(0x8005AF7D);
            status = r_u32(0x8005AF6C);
            code = r_u32(0x8005AF70);
            command = r_u32(0x80054AD4 + 4 * index);
            (void)RRJ_DEBUG_PRINTF((const char *)xport_guest_cptr(0x8005B114, 1), (const char *)xport_guest_cptr(command, 1), status, code);
        }
    }
    switch (interrupt)
    {
        case 1:
            if (error && count == 1)
                error = 0;
            w_u8(0x8005B0D9, error ? 5 : 1);
            sub_800467F4(0x8005B5C0, response);
            w_u8(r_u32(0x8005B0C0), 0);
            w_u8(r_u32(0x8005B0CC), 0);
            return 4;
        case 2:
            w_u8(0x8005B0D8, error ? 5 : 2);
            sub_800467F4(0x8005B5B8, response);
            return 2;
        case 3:
            if (error)
                w_u8(0x8005B0D8, 5);
            else if (r_u32(0x80054B74 + 4 * r_u8(0x8005AF7D)))
            {
                w_u8(0x8005B0D8, 3);
                sub_800467F4(0x8005B5B8, response);
                return 1;
            }
            else
                w_u8(0x8005B0D8, 2);
            sub_800467F4(0x8005B5B8, response);
            return 2;
        case 4:
            w_u8(0x8005B0DA, 4);
            w_u8(0x8005B0D9, r_u8(0x8005B0DA));
            sub_800467F4(0x8005B5C8, response);
            sub_800467F4(0x8005B5C0, response);
            return 4;
        case 5:
            w_u8(0x8005B0D9, 5);
            w_u8(0x8005B0D8, r_u8(0x8005B0D9));
            sub_800467F4(0x8005B5B8, response);
            sub_800467F4(0x8005B5C0, response);
            return 6;
        default:
            (void)RRJ_DEBUG_PUTS((const char *)xport_guest_cptr(0x8005B130, 1));
            (void)RRJ_DEBUG_PRINTF((const char *)xport_guest_cptr(0x8005B144, 1), interrupt);
            return 0;
    }
}

uint32_t sub_80047364(void)
{
    uint32_t registers;
    uint8_t attenuation[4];
    FUNCTION_MARKER(0x80047364, "SLUS_010.53");
    registers = r_u32(0x8005B0D4);
    if (!r_u16(registers + 440) && !r_u16(registers + 442))
    {
        w_u16(registers + 384, 0x3FFF);
        w_u16(registers + 386, 0x3FFF);
    }
    registers = r_u32(0x8005B0D4);
    w_u16(registers + 432, 0x3FFF);
    w_u16(registers + 434, 0x3FFF);
    w_u16(registers + 426, 0xC001);
    attenuation[2] = 128;
    attenuation[0] = 128;
    attenuation[3] = 0;
    attenuation[1] = 0;
    return sub_80047248(attenuation) ? 0xFFFFFFFF : 0;
}

uint32_t sub_80047248(const uint8_t attenuation[4])
{
    uint32_t port;
    uint8_t value;
    FUNCTION_MARKER(0x80047248, "SLUS_010.53");
    port = r_u32(0x8005B0C0);
    w_u8(port, 2);
    port = r_u32(0x8005B0C8);
    value = attenuation[0];
    w_u8(port, value);
    port = r_u32(0x8005B0CC);
    value = attenuation[1];
    w_u8(port, value);
    port = r_u32(0x8005B0C0);
    w_u8(port, 3);
    port = r_u32(0x8005B0C4);
    value = attenuation[2];
    w_u8(port, value);
    port = r_u32(0x8005B0C8);
    value = attenuation[3];
    w_u8(port, value);
    port = r_u32(0x8005B0CC);
    w_u8(port, 32);
    return 0;
}

uint32_t sub_8001578C(uint32_t event, uint32_t response)
{
    uint32_t state, sector;
    FUNCTION_MARKER(0x8001578Cu, "SLUS_010.53");
    event &= 255u;
    if (event != 2u)
        return sub_80015A18(event, xport_guest_ptr(response, 1));
    state = r_u32(0x800D6728u);
    if (state == 2u)
    {
        sector = r_u32(0x800D6720u);
        w_u32(0x800D6728u, 5u);
        w_u32(0x800D6724u, sector);
        return CdSyncCallbackPSX(0u);
    }
    if (state < 3u)
    {
        if (state != 1u)
            return 1u;
        w_u32(0x800D6728u, 2u);
        return (uint32_t)CdControlF(6u, NULL);
    }
    if (state == 3u)
    {
        w_u32(0x800D6728u, 4u);
        return (uint32_t)CdControlF(6u, NULL);
    }
    if (state == 4u)
    {
        sector = r_u32(0x800D6720u);
        w_u32(0x800D6728u, 0u);
        w_u32(0x800D672Cu, sector);
        return CdSyncCallbackPSX(0u);
    }
    return 4u;
}

uint32_t sub_80015860(uint32_t event, uint32_t response)
{
    uint32_t state, sector, expected, chunk, destination, remaining, result, target;
    uint8_t tail[4];
    FUNCTION_MARKER(0x80015860, "SLUS_010.53");
    event &= 255;
    if (event != 1)
    {
        if (event == 5)
            return sub_80015A18(5, xport_guest_ptr(response, 1));
        return 5;
    }
    state = r_u32(0x800D6728);
    if (state < 5)
    {
        if (!state)
        {
            result = r_u32(0x800D6724) + 1;
            w_u32(0x800D6724, result);
            return result;
        }
        return 5;
    }
    if (state != 5)
        return 5;
    (void)sub_80045BEC(xport_guest_ptr(0x800D6730, 12), 3);
    (void)CdDataSync(0);
    sector = sub_80015C08(xport_guest_cptr(0x800D6730, 3));
    expected = r_u32(0x800D6720);
    w_u32(0x800D6724, sector + 1);
    if (sector != expected)
    {
        destination = r_u32(0x800D6714);
        remaining = r_u32(0x800D671C);
        w_u32(0x800D6728, 0);
        return sub_8001564C(xport_guest_ptr(destination, 1), remaining);
    }
    remaining = r_u32(0x800D671C);
    if ((int32_t)remaining >= 2049)
        remaining = 2048;
    chunk = remaining & 0xFFFFFFFC;
    (void)sub_80045BEC(xport_guest_ptr(r_u32(0x800D6714), chunk), (uint32_t)((int32_t)chunk >> 2));
    destination = r_u32(0x800D6714);
    remaining = r_u32(0x800D671C);
    w_u32(0x800D6714, destination + chunk);
    remaining -= chunk;
    w_u32(0x800D671C, remaining);
    if ((int32_t)chunk < 2048 && remaining)
    {
        /* TODO Bind the physical CD transfer into this native local buffer */
        (void)sub_80045BEC(tail, 1);
        (void)CdDataSync(0);
        destination = r_u32(0x800D6714);
        remaining = r_u32(0x800D671C);
        (void)sub_8001E08C(xport_guest_ptr(destination, 1), tail, remaining);
        destination = r_u32(0x800D6714);
        remaining = r_u32(0x800D671C);
        w_u32(0x800D671C, 0);
        w_u32(0x800D6714, destination + remaining);
    }
    w_u32(0x800D6718, r_u32(0x800D6718) - 1);
    result = r_u32(0x800D6720) + 1;
    remaining = r_u32(0x800D6718);
    w_u32(0x800D6720, result);
    if (!remaining)
    {
        (void)CdDataSync(0);
        result = r_u32(0x800D6724);
        target = r_u32(0x800D6710);
        w_u32(0x800D6728, 0);
        w_u32(0x800D672C, result);
        if (target)
            return rrj_file_function(target)();
    }
    return result;
}

uint32_t sub_80045BEC(void *destination, uint32_t words)
{
    FUNCTION_MARKER(0x80045BECu, "SLUS_010.53");
    return sub_80047580(destination, words) < 1u;
}
