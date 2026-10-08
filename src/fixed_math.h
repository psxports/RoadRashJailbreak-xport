#ifndef RRJ_MATH_H
#define RRJ_MATH_H
#include "types.h"
#include "psx_memory.h"

/* SLUS_010.53. NFS4 aliases are hypotheses, not original RRJ symbols.
 * Byte buffers retain little-endian layout and permitted input/output overlap.
 * No restrict: the MIPS reloads inputs after each output store.
 */
int64_t sub_8001FC90(int32_t a, int32_t b); /* fixed multiply, floor(product / 65536) */
uint32_t sub_8001FEB4(RRJMemory *m, int32_t angle);
uint32_t sub_8002EA20(RRJMemory *m, uint32_t left, uint32_t right, int32_t scale, uint32_t output);
uint32_t sub_8001005C(RRJMemory *m, uint32_t matrix, uint32_t quaternion);
uint32_t rrj_quaternion_to_matrix_values(RRJMemory *m, uint32_t matrix, const int32_t quaternion[4]);
int64_t sub_8002E874(const void *left, const void *right, void *out);    /* crossproduct */
int64_t sub_8002E928(const void *vector, const void *matrix, void *out); /* transform */
#endif
