#include "rrj_debug.h"
#include "native_game_api.h"
#include "solo_file_dependencies.h"
#include <stdio.h>
#include <stdlib.h>

uint32_t rrj_solo_bios_toupper(uint32_t value)
{
    uint32_t character = value & 255u;
    if (!rrj_host_context()->bios)
    {
        RRJ_DEBUG_FPUTS("rrj_solo_bios_toupper: missing BIOS image\n", stderr);
        abort();
    }
    if (rrj_host_context()->bios[0xDDB1u + character] & 2u)
        character = (character - 32u) & 255u;
    return (uint32_t)(int32_t)(int8_t)character;
}

uint32_t rrj_solo_open_path(const char *path, uint32_t device)
{
    return sub_8001458C(path, device);
}
