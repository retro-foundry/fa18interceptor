/* Glue for faces_all_behind $C27456, called from inside the candidate scan
 * $C26EBE: its inputs are A4 (the face stream), A3 (the record), A2.w and
 * A1 (eye x and y) and the caller's frame words -$5A(A6) (shift) and
 * -$42(A6) (eye z). The caller reads Z and the last face's working
 * registers, which are replayed first (the routine only reads memory). */
#include "glue.h"
#include "ports_glue.h"

#include "memory.h"
#include "plane_tests.h"

#define SEXT(v) ((uint32_t)(int32_t)(int16_t)(v))
#define W(n) ((int16_t)D(n))

static void asr_long_reg(int n, int count) {
    int32_t v = (int32_t)D(n);
    count &= 63;
    D(n) = (uint32_t)(count >= 32 ? (v < 0 ? -1 : 0) : v >> count);
}

static void asr_word_reg(int n, int count) {
    int16_t v = W(n);
    count &= 63;
    SET_W(D(n), (uint16_t)(count >= 16 ? (v < 0 ? -1 : 0) : v >> count));
}

static void muls(int dst, int src) { D(dst) = (uint32_t)((int32_t)W(dst) * W(src)); }

/* $C2745A-$C274F4 for the face at D2; returns the BLT condition. */
static int face_regs(int16_t shift, int16_t eye_z) {
    gaddr a3 = A(3);
    int k;
    int32_t sum;
    int16_t count;

    A(5) = D(2);
    for (k = 0; k < 3; k++) D(2 + k) = SEXT(rd_u16(A(5) + 2 + (gaddr)(2 * k)));
    SET_W(D(4), W(4) & 0xFFF);
    for (k = 2; k <= 4; k++) SET_W(D(k), W(k) + 0xA4);
    A(5) = a3 + SEXT(D(2));
    SET_W(D(0), D(4));
    {
        gaddr q = a3 + SEXT(D(3)), r = a3 + SEXT(D(0));
        for (k = 0; k < 3; k++) D(2 + k) = SEXT(rd_u16(q + (gaddr)(2 * k)));
        for (k = 0; k < 3; k++) SET_W(D(2 + k), W(2 + k) - rd_s16(A(5) + (gaddr)(2 * k)));
        for (k = 0; k < 3; k++) D(5 + k) = SEXT(rd_u16(r + (gaddr)(2 * k)));
        for (k = 0; k < 3; k++) SET_W(D(5 + k), W(5 + k) - rd_s16(A(5) + (gaddr)(2 * k)));
    }
    SET_W(D(0), D(4));
    SET_W(D(1), D(7));
    muls(7, 3);
    muls(4, 6);
    D(7) -= D(4);
    D(4) = 7;
    SET_W(D(4), W(4) + shift);
    count = W(4);
    asr_long_reg(7, count);
    muls(0, 5);
    muls(1, 2);
    D(0) -= D(1);
    asr_long_reg(0, count);
    muls(6, 2);
    muls(5, 3);
    D(6) -= D(5);
    asr_long_reg(6, count);
    SET_W(D(5), D(7));
    SET_W(D(7), D(6));
    SET_W(D(6), D(0));
    for (k = 0; k < 3; k++) D(2 + k) = SEXT(rd_u16(A(5) + (gaddr)(2 * k)));
    D(7) = (D(7) & 0xFFFFu) | ((uint32_t)(uint16_t)shift << 16);  /* SWAP, MOVE.W, SWAP */
    asr_word_reg(2, shift);
    asr_long_reg(3, shift);
    asr_word_reg(4, shift);
    SET_W(D(2), W(2) + rd_s16(a3 + 0xC));
    D(3) += rd_u32(a3 + 0x10);
    SET_W(D(4), W(4) + rd_s16(a3 + 0xE));
    SET_W(D(2), W(2) - (int16_t)A(2));
    D(3) -= A(1);
    SET_W(D(4), W(4) - eye_z);
    for (k = 2; k <= 4; k++) SET_W(D(k), (uint16_t)-W(k));
    muls(5, 2);
    muls(6, 3);
    muls(7, 4);
    D(7) += D(5);
    sum = (int32_t)D(7);
    D(7) += D(6);
    return (int64_t)sum + (int32_t)D(6) < 0;
}

int glue_C27456(void) {
    int16_t shift = rd_s16(A(6) - 0x5A), eye_z = rd_s16(A(6) - 0x42);
    gaddr stream = A(4);
    int behind = faces_all_behind(&stream, A(3), shift, (int16_t)A(2), (int32_t)A(1), eye_z);

    for (;;) {
        D(2) = rd_u32(A(4));
        A(4) += 4;
        if ((int32_t)D(2) < 0) break;
        if (!face_regs(shift, eye_z)) break;
    }
    A(4) = stream;
    D(7) = behind ? 1 : 0;
    flags_logic_l(D(7));
    return glue_return();
}
