#include "race_bodyless_batch_003.h"
#include "music_stop.h"
#include "race_leaf.h"
#include "native_game_api.h"
#include "psx.h"
/* Music request/start and CD read bookkeeping, original F MIPS. */
#include "music_start.h"
#include <stdlib.h>

uint32_t sub_8007EF64(void)
{
    FUNCTION_MARKER(0x8007EF64u, "RASHCDF.BIN");
    uint32_t job[8], offset, index;
    job[3] = 0;
    job[1] = r_u32(0x800A084C);
    job[2] = r_u32(0x800A0850);
    job[0] = 512;
    index = r_u32(0x800A0864);
    job[4] = r_u32(0x800A0870 + 4 * (index & 1));
    job[5] = r_u32(0x800A0864);
    job[6] = 0x8007F028;
    job[7] = 0x4000;
    if (sub_80022D20(job) == 0xffffff9c)
    {
        (void)(uint32_t)sub_80022A78(2);
        (void)sub_80022D20(job);
    }
    offset = r_u32(0x800A0850) + 0x4000;
    index = r_u32(0x800A0864);
    w_u32(0x800A0850, offset);
    w_u32(0x800A0864, index + 1);
    return offset;
}

uint32_t sub_8007EEB8(uint32_t track)
{
    FUNCTION_MARKER(0x8007EEB8u, "RASHCDF.BIN");
    uint32_t i, active;
    if (track >= 18)
        return 0;
    if (r_u32(0x800A0830) != track)
    {
        w_u32(0x800A0864, 0);
        w_u32(0x800A0858, 0);
        (void)(uint32_t)sub_8007F158(1);
        active = r_u32(0x800A0878);
        w_u32(0x800A0850, track * 458752);
        if (active)
            (void)(uint32_t)sub_80022A1C(2);
        w_u32(0x800A0878, 1);
        for (i = 0; i < 28; ++i)
            (void)sub_8007EF64();
    }
    w_u32(0x800A0830, track);
    return 0x800a0000;
}

uint32_t sub_8007EDE0(uint32_t track)
{
    FUNCTION_MARKER(0x8007EDE0u, "RASHCDF.BIN");
    uint32_t event[3], handle;
    if (track >= 18)
        return 0;
    if (r_u32(0x800A0830) != track)
    {
        (void)sub_8007EEB8(track);
        w_u32(0x800A0838, 1);
        return 1;
    }
    if (r_u32(0x800A0834))
        return 0x800a0000;
    if (((int32_t)(uint32_t)(r_u32(0x800A0858))) < 12)
    {
        w_u32(0x800A0838, 1);
        return 1;
    }
    event[0] = 1486;
    event[1] = r_u32(0x800D6C14);
    event[2] = 0;
    handle = sub_8001F37C(1, event, 65456);
    w_u32(0x800A0844, handle);
    event[2] = 127;
    handle = sub_8001F37C(1, event, 294832);
    w_u32(0x800A0848, handle);
    (void)sub_8001F544(2, xport_guest_ptr(0x800A0844, 8));
    w_u32(0x800A0834, 1);
    w_u32(0x800A0838, 0);
    return 1;
}

uint32_t sub_8007F028(uint32_t unused0, uint32_t unused1, uint32_t index)
{
    FUNCTION_MARKER(0x8007F028u, "RASHCDF.BIN");
    uint32_t parameters[6];
    parameters[0] = (index << 13) + 0xFFB0u;
    parameters[1] = 0x2000u;
    parameters[2] = r_u32(0x800A0870u + ((index & 1u) << 2));
    parameters[3] = 0;
    /* Original callback-free request leaves this unused token unwritten */
    parameters[4] = 0;
    parameters[5] = 1;
    (void)sub_8001E22C(parameters, 0);
    parameters[3] = 0x8007F0D0u;
    parameters[4] = index;
    parameters[0] += 0x38000u;
    parameters[2] += 0x2000u;
    return sub_8001E22C(parameters, 0);
}

uint32_t sub_8007F0D0(uint32_t index)
{
    FUNCTION_MARKER(0x8007F0D0u, "RASHCDF.BIN");
    uint32_t completed = r_u32(0x800A0858u);
    if (completed != index)
        return completed;
    completed = r_u32(0x800A0858u);
    w_u32(0x800A0858u, completed + 1u);
    if (((int32_t)(uint32_t)(r_u32(0x800A0858u))) >= 12 && r_u32(0x800A0838u))
        (void)sub_8007EDE0(r_u32(0x800A0830u));
    if (((int32_t)(uint32_t)(r_u32(0x800A0858u))) >= 28)
        w_u32(0x800A0878u, 0);
    return 0x800A0000u;
}

uint32_t sub_8007E824(void)
{
    uint32_t record[8], value;
    FUNCTION_MARKER(0x8007E824u, "RASHCDF.BIN");
    sub_8001F080();
    w_u32(0x800A0870u, sub_8001447C(0x4000u, 0u));
    value = sub_8001447C(0x4000u, 0u);
    w_u32(0x800A0874u, value);
    value = sub_8001458C((const char *)xport_guest_cptr(0x8005CA30u, 1), r_u32(0x8005AD5Cu));
    w_u32(0x800A0868u, value);
    w_u32(0x800A0860u, sub_8001447C(316u, 0u));
    w_u32(0x800A0854u, UINT32_MAX);
    record[0] = 512u;
    record[6] = 0x8007E958u;
    record[2] = 0u;
    record[3] = 0u;
    record[5] = 0u;
    record[7] = 61660u;
    record[1] = r_u32(0x800A0868u);
    record[4] = r_u32(0x800810A4u);
    if (sub_80022D20(record) == 0xFFFFFF9Cu)
    {
        sub_80022A78(2u);
        sub_80022D20(record);
    }
    value = sub_8001458C((const char *)xport_guest_cptr(0x8005CA44u, 1), r_u32(0x8005AD5Cu));
    w_u32(0x800A084Cu, value);
    w_u32(0x800A0830u, UINT32_MAX);
    sub_8001EFE8(0u);
    w_u32(0x800A085Cu, 0u);
    sub_80018C1C(0u);
    w_u32(0x800A0878u, 0u);
    return 0x800A0000u;
}

uint32_t sub_8007E958(void)
{
    uint32_t destination, source;
    FUNCTION_MARKER(0x8007E958u, "RASHCDF.BIN");
    destination = r_u32(0x800A0860u);
    source = r_u32(0x800810A4u);
    sub_8001E0B4(xport_guest_ptr(destination, 316u), xport_guest_ptr(source, 1), 316u);
    destination = r_u32(0x800A0860u);
    source = r_u32(0x800810A4u);
    sub_8001ED28(destination, source + 316u, UINT32_MAX, 0x8007E9D0u, 0u, 0u);
    sub_8001460C(r_u32(0x800A0868u));
    w_u32(0x800A0868u, UINT32_MAX);
    return UINT32_MAX;
}

uint32_t sub_8007E9D0(uint32_t handle)
{
    FUNCTION_MARKER(0x8007E9D0u, "RASHCDF.BIN");
    w_u32(0x800A0854u, handle);
    return 0x800A0000u;
}
