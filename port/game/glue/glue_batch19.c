/* Glue for the rotation matrices $C2E47A, $C2E3DE, $C2E514, $C2E38E and the
 * row scaling $C2E5AC. Callers read some of their register leftovers. */
#include "glue.h"
#include "ports_glue.h"

#include "fixed_math.h"
#include "globals.h"
#include "matrix.h"
#include "memory.h"

void sin_cos_pair_registers(void); /* glue_batch14.c */

/* The high word of a 2.14 product as MULS / SWAP / ASR.W #4 leave it: D6. */
static uint32_t swapped_product(int16_t x, int16_t y) {
    uint32_t p = (uint32_t)((int32_t)x * y);
    return p << 16 | (uint16_t)((int16_t)(p >> 16) >> 4);
}

/* The three angles shifted to sin_cos units and the three lookups' register
 * results (D0-D5 words, D7, A0), as $C2E5F6 then $C2E6DA leave them. */
static void three_lookup_registers(void) {
    Fixed14 sine, cosine;
    SET_W(D(0), (uint16_t)D(0) >> 3);
    SET_W(D(2), (uint16_t)D(2) >> 3);
    SET_W(D(4), (uint16_t)D(4) >> 3);
    sin_cos((Angle)(int16_t)D(4), &sine, &cosine);
    sin_cos_pair_registers();
    SET_W(D(4), sine);
    SET_W(D(5), cosine);
}

/* $C2E3DE: D0.w, D2.w, D4.w angles, A1 matrix. Every data register is live
 * after it: D0.w = -(sa >> 6), D1-D5 the lookups, D6 and D7 the last two
 * sums as SWAP / ASR.W #4 leave them. */
void rotation_matrix8_registers(void) {
    int16_t cb_sa;
    int32_t sum;
    three_lookup_registers();
    cb_sa = (int16_t)(((int32_t)(int16_t)D(3) * (int16_t)D(0)) >> 14);
    sum = (int32_t)(int16_t)D(2) * (int16_t)D(4) + (int32_t)cb_sa * (int16_t)D(5);
    D(7) = (uint32_t)sum << 16 | (uint16_t)((int16_t)((uint32_t)sum >> 16) >> 4);
    D(6) = swapped_product((int16_t)D(3), (int16_t)D(1));
    SET_W(D(0), -(int16_t)((int16_t)D(0) >> 6));
    A(1) += 16;
}

int glue_C2E3DE(void) {
    rotation_matrix8((uint16_t)D(0), (uint16_t)D(2), (uint16_t)D(4), A(1));
    rotation_matrix8_registers();
    return glue_return();
}

/* $C2E514: D0.w, D2.w, D4.w angles, A1 matrix. Everything is live after it:
 * D0-D5 the lookups, D6 = (ca*cb) >> 14, D7 = 14, A1 the last word. */
void alternate_rotation_registers(void);
void alternate_rotation_registers(void) {
    uint16_t a = (uint16_t)D(0), b = (uint16_t)D(2), c = (uint16_t)D(4);
    alternate_rotation_matrix(a, b, c, A(1));
    three_lookup_registers();
    D(6) = (uint32_t)(((int32_t)(int16_t)D(1) * (int16_t)D(3)) >> 14);
    D(7) = 14;
    A(1) += 16;
}

int glue_C2E514(void) {
    alternate_rotation_registers();
    return glue_return();
}

/* $C2E47A: D0.w, D2.w, D4.w angles, A1 matrix. The caller reads D3 (the
 * second cosine), D7 (the last shift count, 14) and A0 (the sine table). */
int glue_C2E47A(void) {
    uint16_t a = (uint16_t)D(0), b = (uint16_t)D(2), c = (uint16_t)D(4);
    rotation_matrix(a, b, c, A(1));
    SET_W(D(0), a >> 3);
    SET_W(D(2), b >> 3);
    sin_cos_pair_registers();
    D(7) = 14;
    A(1) += 16;
    return glue_return();
}

/* $C2E38E: D0.w, D2.w angles, A1 matrix. The caller reads D3, D6 (the last
 * product, swapped) and D7/A0 as the sine lookups left them. */
void two_angle_registers(void);
void two_angle_registers(void) {
    SET_W(D(0), (uint16_t)D(0) >> 3);
    SET_W(D(2), (uint16_t)D(2) >> 3);
    sin_cos_pair_registers();
    D(6) = swapped_product((int16_t)D(3), (int16_t)D(1));
}

int glue_C2E38E(void) {
    two_angle_matrix((uint16_t)D(0), (uint16_t)D(2), A(1));
    two_angle_registers();
    A(1) += 16;
    return glue_return();
}

/* $C2E5AC: A1 matrix. Every register is live after it: D0-D2 hold the scale
 * words (sign-extended), D3-D5 the last row's products >> 8, A1 the last
 * row. */
int glue_C2E5AC(void) {
    gaddr last = A(1) + 12;
    int16_t scale = rd_s16(MATRIX_ROW_SCALES + 4);
    int i;
    /* The last row's full products, before the words are overwritten. */
    for (i = 0; i < 3; i++) D(3 + i) = (uint32_t)(((int32_t)rd_s16(last + (gaddr)(2 * i)) * scale) >> 8);
    scale_matrix_rows(A(1), MATRIX_ROW_SCALES);
    for (i = 0; i < 3; i++) D(i) = (uint32_t)(int32_t)rd_s16(MATRIX_ROW_SCALES + (gaddr)(2 * i));
    A(1) = last;
    return glue_return();
}
