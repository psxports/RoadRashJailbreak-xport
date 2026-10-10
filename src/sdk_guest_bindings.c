#include "psx.h"
#include "sdk_build_stubs.h"
#include <stdio.h>
#include <stdlib.h>

/* Bind reviewed SDK guest slots from SLUS_010.53 */
const uint32 xport_cd_ready_callback_address = 0x8005af64u;
const uint32 xport_cd_sync_callback_address = 0x8005af60u;
const uint32 xport_spu_register_pointer_address = 0x8005a41cu;
const uint32 xport_cd_status_address = 0x8005af6cu;
const uint32 xport_cd_setloc_table_address = 0x80054a14u;

void xport_bind_native_spu_transfer(void)
{
    fputs("RRJ: missing native SPU transfer adapter\n", stderr);
    abort();
}
