#ifndef RRJ_MENU_ITEM_H
#define RRJ_MENU_ITEM_H
#include "menu_sprite.h"
#include "resource.h"
/* Return from the resident convenience wrapper is unused by menu callers.
 * Its original no-string-table path leaves v0 undefined. */
void sub_8002CD78(uint32_t font, uint32_t string_id, uint32_t x, uint32_t y, uint32_t link, uint32_t color);
uint32_t sub_8006F764(uint32_t menu, uint32_t entry);
uint32_t sub_8006EF30(uint32_t menu, uint32_t entry);
uint32_t sub_8006F0B8(uint32_t menu, uint32_t entry);
#endif
