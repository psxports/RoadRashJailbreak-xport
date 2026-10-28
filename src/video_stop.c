#include "rrj_fmv.h"
#include "rrj_cd_transport.h"
#include "rrj_data.h"
#include "psx.h"
#include "psx_spu.h"
#include "video_stop.h"
#include "game_main.h"
#include "resource.h"
#include "race_bodyless_batch_002.h"
#include "video_preview.h"
#include "native_game_api.h"
#include "race_leaf.h"
#include "race_bodyless_batch_003.h"
#include "sdk_build_stubs.h"
#include <stdlib.h>

static PsxInterruptCallback stream_cd_callback(uint32_t address)
{
    if (!address)
        return NULL;
    /* TODO Bind additional saved CD callbacks */
    abort();
}

uint32_t sub_80060BD4(void)
{
    uint32_t context, left, right;
    FUNCTION_MARKER(0x80060BD4u, "RASHCDF.BIN");
    context = r_u32(0x800810BCu);
    w_u32(context + 8u, 0u);
    left = r_u32(context + 31588u);
    right = r_u32(context + 31592u);
    SpuSetKey(0, left | right);
    context = r_u32(0x800810BCu);
    SpuSetReverbVoice(r_u32(context + 31596u), r_u32(context + 31588u));
    context = r_u32(0x800810BCu);
    return SpuSetReverbVoice(r_u32(context + 31600u), r_u32(context + 31592u));
}

uint32_t sub_80060B68(void)
{
    uint32_t context, index;
    FUNCTION_MARKER(0x80060B68u, "RASHCDF.BIN");
    sub_80043DD4();
    context = r_u32(0x800810BCu);
    w_u32(context, 0u);
    w_u32(context + 4u, 0u);
    w_u32(context + 12u, 0u);
    for (index = 0u; index < 4u; ++index)
        w_u32(context + 31532u + 4u * index, 0u);
    context = r_u32(0x800810BCu);
    w_u32(context + 31548u, 1u);
    w_u32(context + 31524u, 0u);
    sub_80043DF4();
    return 1u;
}

uint32_t sub_80060C40(void)
{
    uint32_t context, result;
    FUNCTION_MARKER(0x80060C40u, "RASHCDF.BIN");
    sub_80043DD4();
    context = r_u32(0x800810BCu);
    if (r_u32(context + 31516u) == 1u)
    {
        sub_80060BD4();
        sub_80050B18(0u);
        context = r_u32(0x800810BCu);
        rrj_spu_irq_callback_psx(r_u32(context + 31508u));
        sub_80043DF4();
        context = r_u32(0x800810BCu);
        while (r_u32(context + 31560u))
        {
            rrj_native_wait_events();
        }
        sub_80043DD4();
        context = r_u32(0x800810BCu);
        sub_80051058(r_u32(context + 31512u));
    }
    context = r_u32(0x800810BCu);
    w_u32(context + 31520u, 0u);
    w_u32(context + 31524u, 0u);
    sub_80043DF4();
    result = sub_80060B68();
    w_u32(0x800810BCu, 0u);
    return result;
}

uint32_t sub_8006239C(void)
{
    uint32_t context, result;
    FUNCTION_MARKER(0x8006239Cu, "RASHCDF.BIN");
    context = r_u32(0x8009C2D0u);
    result = r_u32(context + 1056u);
    if (result)
    {
        if (rrj_data_enabled())
            return sub_800620D4(0);
        CdDataCallback(stream_cd_callback(r_u32(context + 1396u)));
        context = r_u32(0x8009C2D0u);
        rrj_cd_read_callback_psx(r_u32(context + 1400u));
        return sub_800620D4(0u);
    }
    return result;
}

static void sub_8005F9B0(void)
{
    FUNCTION_MARKER(0x8005F9B0u, "RASHCDF.BIN");
    sub_8002026C(0u);
    DecDCTReset(0);
}

uint32_t sub_8005F7E0(uint32_t channel)
{
    FUNCTION_MARKER(0x8005F7E0u, "RASHCDF.BIN");
    if (channel)
        sub_80060C40();
    sub_8006239C();
    sub_8005F9B0();
    if (rrj_data_enabled())
        return 1;
    return sub_80015530();
}

uint32_t sub_8006FE6C(void)
{
    uint32_t channel, flags;
    FUNCTION_MARKER(0x8006FE6Cu, "RASHCDF.BIN");
    if (rrj_menu_fmv_active())
    {
        rrj_menu_fmv_close();
        w_u32(0x8009C684u, 0xffffffffu);
        w_u16(0x8009C5DAu, (uint16_t)(r_u16(0x8009C5DAu) & 0xfff8u));
        return 1u;
    }
    if (r_u16(0x8009C5DAu) & 1u)
    {
        channel = r_u16(0x8009C688u);
        if (channel & 0x8000u)
            channel |= 0xffff0000u;
        sub_8005F7E0(channel);
        sub_8001460C(r_u32(0x8009C684u));
        flags = r_u16(0x8009C5DAu);
        w_u32(0x8009C684u, 0xffffffffu);
        w_u16(0x8009C5DAu, (uint16_t)(flags & 0xfff8u));
    }
    return 1u;
}

uint32_t sub_8006FED4(void)
{
    FUNCTION_MARKER(0x8006FED4u, "RASHCDF.BIN");
    return sub_8006FE6C();
}

uint32_t sub_8005F9D8(void)
{
    uint32_t value;
    uint32_t result;

    FUNCTION_MARKER(0x8005F9D8, "RASHCDF.BIN");
    value = r_u32(0x800810B8);
    if (value)
        SpuFree(value);
    value = r_u32(0x800810B4);
    if (value)
        SpuFree(value);
    w_u32(0x800810B8, 0);
    w_u32(0x800810B4, 0);
    result = sub_80020228();
    value = r_u32(0x800810B0);
    if (value)
        result = sub_800144B8(value);
    value = r_u32(0x800810AC);
    if (value)
        result = sub_800144B8(value);
    value = r_u32(0x800810A8);
    if (value)
        result = sub_800144B8(value);
    value = r_u32(0x800810A4);
    if (value)
        result = sub_800144B8(value);
    w_u32(0x800810B0, 0);
    w_u32(0x800810AC, 0);
    w_u32(0x800810A8, 0);
    w_u32(0x800810A4, 0);
    return result;
}

uint32_t sub_8005F81C(void)
{
    uint32_t size;

    FUNCTION_MARKER(0x8005F81C, "RASHCDF.BIN");
    if (!r_u32(0x800810A4))
        w_u32(0x800810A4, sub_8001447C(195840, 0));
    if (!r_u32(0x800810A8))
    {
        size = sub_80062438();
        w_u32(0x800810A8, sub_8001447C(size, 0));
    }
    if (!r_u32(0x800810AC))
    {
        size = sub_800621E0(64, 17408, 2);
        w_u32(0x800810AC, sub_8001447C(size, 0));
    }
    if (!r_u32(0x800810B0))
    {
        size = sub_80060D0C();
        w_u32(0x800810B0, sub_8001447C(size, 0));
    }
    (void)sub_800201C0();
    if (!r_u32(0x800810B4))
        w_u32(0x800810B4, 65456);
    if (!r_u32(0x800810B8))
        w_u32(0x800810B8, 294832);
    if (r_u32(0x800810A4) && r_u32(0x800810A8) && r_u32(0x800810AC) && r_u32(0x800810B0) && r_u32(0x800810B4) && r_u32(0x800810B8))
    {
        sub_8005F9B0();
        return 1;
    }
    (void)sub_8005F9D8();
    return 0;
}

uint32_t sub_8005FABC(uint32_t enabled)
{
    uint32_t first;
    uint32_t state;

    FUNCTION_MARKER(0x8005FABC, "RASHCDF.BIN");
    (void)sub_8005F81C();
    first = r_u32(0x800810A4);
    state = r_u32(0x800810A8);
    return sub_800624EC(state, first, first + 61440, first + 122880, first + 184320, enabled);
}
