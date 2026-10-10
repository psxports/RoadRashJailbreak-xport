#ifndef RRJ_GAME_MAIN_H
#define RRJ_GAME_MAIN_H

#include <stdint.h>

uint32_t sub_800A966C(uint32_t actor, const uint16_t normal[3], int32_t minimum);
uint32_t sub_800B2844(uint32_t actor, uint32_t candidate);
uint32_t sub_800A9868(uint32_t actor, const uint16_t normal[3], int32_t impact_speed,
    uint32_t other_direction, uint32_t other_identity, uint32_t force);

uint32_t sub_800B675C(uint32_t box, uint32_t basis, uint32_t face,
    uint16_t normal[3], uint32_t corner[3]);
uint32_t sub_800B7030(uint32_t points, uint32_t basis, uint32_t box,
    uint32_t excluded_faces, uint32_t *face, uint32_t *depth);
uint32_t sub_800AA34C(uint32_t points, uint32_t basis, uint32_t bounds[4]);
uint32_t sub_800AA140(uint32_t actor, uint32_t other, uint32_t *actor_face,
    uint32_t *other_face, uint32_t translation[3]);

uint32_t sub_800ABE78(uint32_t actor, uint32_t other, uint32_t *linked, uint32_t *angle, uint32_t *lateral, uint32_t *longitudinal);
uint32_t sub_800B5B48(uint32_t actor, uint32_t other, uint16_t output[3]);

uint32_t sub_800AB7A0(uint32_t actor, uint32_t other, uint32_t propagated);
uint32_t sub_800A7AB4(uint32_t actor, const uint32_t delta[3], uint32_t excluded);
uint32_t sub_800A7ABC(uint32_t actor, const uint32_t delta[3], uint32_t excluded, uint32_t initial_count);
uint32_t sub_800AC56C(uint32_t alignment, uint32_t first_side, uint32_t other_mode);
uint32_t sub_800AA474(uint32_t actor, uint32_t other, uint32_t linked, uint32_t angle, uint32_t lateral, uint32_t longitudinal, uint32_t *actor_face, uint32_t *other_face, uint32_t translation[3]);
uint32_t sub_800AAD30(uint32_t actor, uint32_t other, uint32_t *actor_face, uint32_t *other_face, uint32_t translation[3], uint32_t *power);
uint32_t sub_800AC130(uint32_t actor, uint32_t other, uint32_t actor_face, uint32_t other_face);

uint32_t sub_800B71AC(uint32_t points, uint32_t direction, int32_t distance, uint32_t box, uint32_t basis, uint32_t excluded_faces, uint32_t *face, uint32_t *depth);
uint32_t sub_80081D7C(uint32_t actor, uint32_t other, uint32_t actor_face, uint32_t other_face, uint16_t normal[3]);

uint32_t sub_800B7810(uint32_t points, uint32_t basis, uint32_t face, uint32_t output[12], uint16_t planes[12]);

uint32_t sub_800B6D70(const uint32_t point[3], const uint32_t vertices[], const uint16_t planes[], int32_t count, uint32_t tolerance);
uint32_t sub_800B74F0(uint32_t points, uint32_t direction, int32_t distance, uint32_t box, uint32_t basis, uint32_t excluded_faces, uint32_t *face, uint32_t *depth);

#endif
