/* Glue for the coloured face $C099AA, the stored-normal test $C1FB9C, the
 * record steering setters, the view-matrix rotation $C2CE82 and the edge
 * split $C21C2E. */
#include "glue.h"
#include "ports_glue.h"

#include "control_records.h"
#include "faces.h"
#include "globals.h"
#include "memory.h"
#include "plane_tests.h"
#include "view_transform.h"
#include "glue_clip.h"

#define SEXT(v) ((uint32_t)(int32_t)(int16_t)(v))
#define W(n) ((int16_t)D(n))

static void asr_word_reg(int n, int count) {
    int16_t v = W(n);
    count &= 63;
    SET_W(D(n), (uint16_t)(count >= 16 ? (v < 0 ? -1 : 0) : v >> count));
}

static void muls(int dst, int src) { D(dst) = (uint32_t)((int32_t)W(dst) * W(src)); }



/* $C1FB9C: D0-D2 the point, D3-D5 the normal, the eye in the caller's frame
 * words -$26..-$22(A6); D7 = 1 or its low word 0, with those flags. */
int glue_C1FB9C(void) {
    int16_t point[3], normal[3], eye[3];
    int k, toward;
    for (k = 0; k < 3; k++) {
        point[k] = W(k);
        normal[k] = W(3 + k);
        eye[k] = rd_s16(A(6) - 0x26 + (gaddr)(2 * k));
    }
    toward = point_toward_eye(point, normal, eye);
    SET_W(D(7), rd_u16(BOUND_SHIFT));
    for (k = 0; k < 3; k++) asr_word_reg(k, W(7));
    SET_W(D(0), W(0) + rd_s16(BOUND_OFFSET_X));
    SET_W(D(2), W(2) + rd_s16(BOUND_OFFSET_Z));
    for (k = 0; k < 3; k++) SET_W(D(k), W(k) - eye[k]);
    muls(0, 3);
    muls(1, 4);
    muls(2, 5);
    D(2) += D(0);
    D(2) += D(1);
    if (toward) {
        D(7) = 1;
        flags_logic_l(D(7));
    } else {
        SET_W(D(7), 0);
        flags_logic_w(D(7));
    }
    return glue_return();
}

/* ---- steering: D1 the stored byte, D2 the value ------------------------- */

static void controls_regs(void) { SET_B(D(1), rd_u8(A(1) + 0x65)); }

int glue_C2CAA0(void) {
    steer_record_neutral(A(1));
    D(2) = 0;
    controls_regs();
    return glue_return();
}

static void roll_regs(int16_t turn) { D(2) = turn == 0 ? 0 : turn < 0 ? 4 : 8; }

int glue_C2CA92(void) {
    int16_t turn = W(3);
    steer_record_roll(A(1), turn);
    roll_regs(turn);
    controls_regs();
    return glue_return();
}

int glue_C2CA26(void) {
    gaddr r = A(1);
    int16_t turn = W(3), heading = rd_s16(r + 0x6A);
    uint8_t flags = rd_u8(r + 0x64);

    steer_record_turn(r, turn);
    SET_B(D(2), flags & 0x60);
    if ((flags & 0x60) == 0x60) {
        if (turn == 0 || (turn > 0 ? turn <= rd_s16(r + 0x58) : turn >= rd_s16(r + 0x58))) D(2) = 0;
        else {
            SET_B(D(2), turn > 0 ? 0x80 : 0x40);
            SET_W(D(3), (uint16_t)heading);
            if (heading > 0x3840) { if (heading <= 0x7030) SET_B(D(2), (uint8_t)D(2) | 8); }
            else if (heading > 0x50) SET_B(D(2), (uint8_t)D(2) | 4);
        }
    } else {
        int roll = 1;
        if (flags & 0x80) {
            SET_W(D(2), (uint16_t)heading);
            if (heading > 0x3840) { if (heading <= 0x6EF0) { D(2) = 8; roll = 0; } }
            else if (heading >= 0x190) { D(2) = 4; roll = 0; }
        }
        if (roll) roll_regs(turn);
    }
    controls_regs();
    return glue_return();
}

int glue_C2CB86(void) {
    gaddr r = A(1);
    int16_t climb = W(3), limit = rd_s16(r + 0x56);
    steer_record_pitch(r, climb);
    D(2) = (climb >= 0 ? climb > limit : climb < limit) ? (climb >= 0 ? 0x10 : 0x20) : 0;
    D(1) = rd_u8(r + 0x65);
    return glue_return();
}

/* $C2CE82: D3-D5 the vector. Leaves the row products and sums as the
 * MULS / ADD.L / ASR.L sequence does; A0 past the matrix. */
int glue_C2CE82(void) {
    int16_t v[3];
    int32_t out[3];
    gaddr m = VIEW_ANGLE_MATRIX;
    int k;
    for (k = 0; k < 3; k++) v[k] = W(3 + k);
    rotate_by_view_matrix(v, out);
    SET_W(D(6), (uint16_t)out[0]);
    D(0) = (uint32_t)((int32_t)rd_s16(m + 6) * v[0]);
    D(1) = (uint32_t)((int32_t)rd_s16(m + 8) * v[1]);
    D(2) = (uint32_t)out[1];
    D(3) = (uint32_t)((int32_t)rd_s16(m + 12) * v[0]);
    D(4) = (uint32_t)((int32_t)rd_s16(m + 14) * v[1]);
    D(5) = (uint32_t)out[2];
    A(0) = m + 18;
    return glue_return();
}

/* $C21C2E: A2 stream. The second split's registers, then D0 = 0 (MOVEQ). */
