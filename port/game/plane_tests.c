/* Plane-side tests over face streams. */
#include "plane_tests.h"

#include "fixed_math.h"
#include "globals.h"

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

/* The sign of the true sum of the last ADD.L. */
static int sum_not_negative(int32_t first, int32_t second, int32_t third) {
    int32_t sum = (int32_t)((uint32_t)first + (uint32_t)second);
    return (int64_t)sum + third >= 0;
}

int point_toward_eye(const int16_t point[3], const int16_t normal[3], const int16_t eye[3]) {
    int16_t bound = rd_s16(BOUND_SHIFT), q[3];
    int k;
    for (k = 0; k < 3; k++) q[k] = asr_word(point[k], bound);
    q[0] = (int16_t)(q[0] + rd_s16(BOUND_OFFSET_X));
    q[2] = (int16_t)(q[2] + rd_s16(BOUND_OFFSET_Z));
    for (k = 0; k < 3; k++) q[k] = (int16_t)(q[k] - eye[k]);
    return sum_not_negative(q[2] * normal[2], q[0] * normal[0], q[1] * normal[1]);
}

int face_toward_eye(uint16_t kind, gaddr points, gaddr *faces, const int16_t eye[3]) {
    int16_t q[3], n[3], u[3], v[3], p[3];
    int k, shift;

    if (kind & 0x3000) {
        gaddr face = points + (gaddr)(int32_t)rd_s16(*faces);
        *faces += 2;
        for (k = 0; k < 3; k++) {
            q[k] = rd_s16(face + (gaddr)(2 * k));
            n[k] = rd_s16(face + 6 + (gaddr)(2 * k));
        }
        return point_toward_eye(q, n, eye);
    }
    shift = (kind >> 7) & 7;
    for (k = 0; k < 3; k++) {
        p[k] = rd_s16(CLIP_INPUT + 4 + (gaddr)(2 * k));
        u[k] = (int16_t)((int16_t)(rd_s16(CLIP_INPUT + 10 + (gaddr)(2 * k)) - p[k]) << shift);
        v[k] = (int16_t)((int16_t)(rd_s16(CLIP_INPUT + 16 + (gaddr)(2 * k)) - p[k]) << shift);
    }
    n[0] = (int16_t)((int32_t)((uint32_t)(u[1] * v[2]) - (uint32_t)(v[1] * u[2])) >> 8);
    n[1] = (int16_t)((int32_t)((uint32_t)(v[0] * u[2]) - (uint32_t)(u[0] * v[2])) >> 8);
    n[2] = (int16_t)((int32_t)((uint32_t)(u[0] * v[1]) - (uint32_t)(u[1] * v[0])) >> 8);
    return sum_not_negative(n[2] * p[2], n[0] * p[0], n[1] * p[1]);
}

/* The low word of a dot product's absolute value, shifted down by 4. */
static int16_t abs_dot(const int16_t a[3], const int16_t b[3]) {
    int32_t first = (int32_t)((uint32_t)(a[2] * b[2]) + (uint32_t)(a[0] * b[0]));
    int32_t sum = (int32_t)((uint32_t)first + (uint32_t)(a[1] * b[1]));
    if ((int64_t)first + a[1] * b[1] < 0) sum = (int32_t)(0u - (uint32_t)sum);
    return (int16_t)(sum >> 4);
}

static void horizontal_unit(int16_t x, int16_t z, int16_t out[3]) {
    int k;
    normalize_vector(0x100, x, 0, z);
    for (k = 0; k < 3; k++) out[k] = rd_s16(NORMALIZED + (gaddr)(2 * k));
}

int edge_alignment(gaddr edge, int16_t eye_x, int16_t eye_z, int16_t range) {
    int16_t along[3], toward[3], index;
    gaddr table;

    horizontal_unit((int16_t)(rd_s16(edge + 4) - rd_s16(edge)), (int16_t)(rd_s16(edge + 6) - rd_s16(edge + 2)), along);
    horizontal_unit((int16_t)(eye_x - rd_s16(BOUND_OFFSET_X)), (int16_t)(eye_z - rd_s16(BOUND_OFFSET_Z)), toward);
    table = rd_s32(PROJECTION_Y) > -0x80 ? ALIGNMENT_NEAR : ALIGNMENT_FAR;
    index = (int16_t)(range >> 4);
    if (index > 10) index = 11;
    return abs_dot(along, toward) < rd_s16(table + (gaddr)(int32_t)(int16_t)(2 * index)) ? -1 : 0;
}

int edge_alignment_test(gaddr *stream, int16_t eye_x, int16_t eye_z, int16_t range) {
    gaddr edge = rd_u32(BOUND_RECORD) + 0xA + (gaddr)(int32_t)rd_s16(*stream);
    *stream += 2;
    if (rd_s32(PROJECTION_Y) <= -0x140 || rd_u8(ATTITUDE_NEAR)) return 0;
    return edge_alignment(edge, eye_x, eye_z, range);
}
