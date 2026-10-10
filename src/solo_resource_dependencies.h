#ifndef RRJ_SOLO_RESOURCE_DEPENDENCIES_H
#define RRJ_SOLO_RESOURCE_DEPENDENCIES_H
#include "resource.h"
#include "race_leaf.h"

typedef uint32_t (*RRJSoloResourceOpen)(RRJMemory *, const char *, uint32_t);
typedef uint32_t (*RRJSoloResourceSubmit)(RRJMemory *, const uint32_t[8]);

typedef struct RRJSoloResourceServices
{
    RRJQueueCall call;
    RRJSoloResourceOpen open;
    RRJSoloResourceSubmit submit;
} RRJSoloResourceServices;

uint32_t sub_F_800782B0(int16_t id, uint32_t wait, const RRJSoloResourceServices *);
uint32_t sub_F_800781A8(uint32_t unused0, uint32_t unused1, uint32_t descriptor, RRJRaceLeafCall);
#endif
