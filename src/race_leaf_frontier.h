#ifndef RRJ_RACE_LEAF_FRONTIER_H
#define RRJ_RACE_LEAF_FRONTIER_H

#include "race_leaf.h"

uint32_t sub_800A13C4(RRJMemory *m, int32_t delta, RRJRaceLeafCall call);
uint32_t sub_80090814(RRJMemory *m, int32_t delta, RRJRaceLeafCall call);
uint32_t sub_800C5078(RRJMemory *m, uint32_t actor, RRJRaceLeafCall call);
uint32_t sub_800C47CC(RRJMemory *m, uint32_t actor, RRJRaceLeafCall call);
uint32_t sub_800C4860(RRJMemory *m, uint32_t actor, RRJRaceLeafCall call);
uint32_t sub_800C4B30(RRJMemory *m, uint32_t actor, RRJRaceLeafCall call);
uint32_t sub_800C4BA0(RRJMemory *m, uint32_t actor, RRJRaceLeafCall call);
uint32_t sub_800C4E18(RRJMemory *m, uint32_t actor, RRJRaceLeafCall call);
uint32_t sub_800881B4(RRJMemory *m, uint32_t actor, int32_t delta, RRJRaceLeafCall call);
uint32_t sub_80087420(RRJMemory *m, uint32_t actor, int32_t delta,
                      RRJRaceLeafCall call);
uint32_t sub_800853E4(RRJMemory *m, uint32_t actor, uint32_t descriptor);
uint32_t sub_80086E1C(RRJMemory *m, uint32_t actor, RRJRaceLeafCall call);
uint32_t sub_80088140(RRJMemory *m, uint32_t actor);
uint32_t sub_8008CFDC(RRJMemory *m, RRJRaceLeafCall call);
uint32_t sub_8008DBCC(RRJMemory *m, uint32_t actor);
uint32_t sub_800667C4(RRJMemory *m, uint32_t actor, uint32_t players);
uint32_t sub_8005E1D8(RRJMemory *m, uint32_t context, RRJRaceLeafCall call);
uint32_t sub_8005D2A8(RRJMemory *m, uint32_t actor, RRJRaceLeafCall call);
uint32_t sub_8005E5A4(RRJMemory *m, uint32_t actor, uint32_t channels, RRJRaceLeafCall call);
uint32_t sub_8005E558(RRJMemory *m, uint32_t stream_pointer, uint32_t bit_pointer, uint32_t width);
uint32_t sub_8005D63C(RRJMemory *m, uint32_t actor, RRJRaceLeafCall call);
uint32_t sub_80066A60(RRJMemory *m, uint32_t object, uint32_t vector);
uint32_t sub_8005CB04(RRJMemory *m, uint32_t actor, uint32_t delta, RRJRaceLeafCall call);
uint32_t sub_8005C58C(RRJMemory *m, uint32_t actor, uint32_t delta, RRJRaceLeafCall call);
uint32_t sub_8005C52C(RRJMemory *m, uint32_t actor);
uint32_t sub_8005C4EC(RRJMemory *m, uint32_t actor, uint32_t delta);

#endif
