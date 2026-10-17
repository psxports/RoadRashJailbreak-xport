#ifndef RRJ_MATH_H
#define RRJ_MATH_H
#include "types.h"
#include "psx_memory.h"

/* SLUS_010.53. NFS4 aliases are hypotheses, not original RRJ symbols.
 * Byte buffers retain little-endian layout and permitted input/output overlap.
 * No restrict: the MIPS reloads inputs after each output store.
 */
int64_t sub_8001FC90(int32_t a, int32_t b); /* fixed multiply, floor(product / 65536) */
uint32_t sub_8002DF14(uint32_t vector[2]);
uint32_t sub_8002DFC0(const uint32_t left[2], const uint32_t right[2]);
uint32_t sub_8002E010(const uint32_t vector[2]);
uint32_t sub_8001FEB4(int32_t angle);
uint32_t sub_8002EA20(const uint16_t left[3], const uint16_t right[3], int32_t scale, uint32_t output[3]);
uint32_t sub_8001005C(uint16_t matrix[9], const int32_t quaternion[4]);
int64_t sub_8002E874(const void *left, const void *right, void *out);    /* crossproduct */
int64_t sub_8002E928(const void *vector, const void *matrix, void *out); /* transform */
#endif
