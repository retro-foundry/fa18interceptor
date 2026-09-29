/* Plane-side tests over face streams. */
#include "plane_tests.h"

#define POINTS 0xA4 /* a record's point table */

/* 68000 ASR by a register count (modulo 64). */
static int32_t asr_long(int32_t v, int count) {
    count &= 63;
    return count >= 32 ? (v < 0 ? -1 : 0) : v >> count;
}

static int16_t asr_word(int16_t v, int count) {
    count &= 63;
    return count >= 16 ? (int16_t)(v < 0 ? -1 : 0) : (int16_t)(v >> count);
}

static int16_t word_at(gaddr record, int16_t offset, int k) {
    return rd_s16(record + (gaddr)(int32_t)(int16_t)(offset + POINTS) + (gaddr)(2 * k));
}

/* n . e for one face: < 0 when the face is behind. */
static int face_behind(gaddr face, gaddr record, int16_t shift,
                       int16_t eye_x, int32_t eye_y, int16_t eye_z) {
    int16_t p = rd_s16(face + 2), q = rd_s16(face + 4), r = (int16_t)(rd_s16(face + 6) & 0xFFF);
    int16_t u[3], v[3], n[3], e[3];
    int k, count = (int16_t)(7 + shift);
    int32_t sum;

    for (k = 0; k < 3; k++) {
        u[k] = (int16_t)(word_at(record, q, k) - word_at(record, p, k));
        v[k] = (int16_t)(word_at(record, r, k) - word_at(record, p, k));
    }
    n[0] = (int16_t)asr_long((int32_t)((uint32_t)(u[1] * v[2]) - (uint32_t)(u[2] * v[1])), count);
    n[1] = (int16_t)asr_long((int32_t)((uint32_t)(u[2] * v[0]) - (uint32_t)(u[0] * v[2])), count);
    n[2] = (int16_t)asr_long((int32_t)((uint32_t)(u[0] * v[1]) - (uint32_t)(u[1] * v[0])), count);

    e[0] = (int16_t)-(int16_t)(asr_word(word_at(record, p, 0), shift) + rd_s16(record + 0xC) - eye_x);
    e[1] = (int16_t)-(int16_t)((uint32_t)asr_long(word_at(record, p, 1), shift) + (uint32_t)rd_s32(record + 0x10)
                               - (uint32_t)eye_y);
    e[2] = (int16_t)-(int16_t)(asr_word(word_at(record, p, 2), shift) + rd_s16(record + 0xE) - eye_z);

    /* The sign of the true sum of the last addition (BLT after ADD.L). */
    sum = (int32_t)((uint32_t)(n[2] * e[2]) + (uint32_t)(n[0] * e[0]));
    return (int64_t)sum + (int32_t)(n[1] * e[1]) < 0;
}

int faces_all_behind(gaddr *stream, gaddr record, int16_t shift,
                     int16_t eye_x, int32_t eye_y, int16_t eye_z) {
    for (;;) {
        int32_t face = rd_s32(*stream);
        *stream += 4;
        if (face < 0) break;
        if (!face_behind((gaddr)face, record, shift, eye_x, eye_y, eye_z)) {
            while (rd_s32(*stream) >= 0) *stream += 4;
            *stream += 2;
            return 0;
        }
    }
    *stream -= 2;
    return 1;
}
