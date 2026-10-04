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

/* Complete record steering owners are in glue_record_steering.c. */

/* $C21C2E: A2 stream. The second split's registers, then D0 = 0 (MOVEQ). */
