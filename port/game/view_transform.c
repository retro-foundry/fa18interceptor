#include "view_transform.h"

#include "globals.h"

void view_transform(gaddr in, int16_t shift, gaddr out) {
    int count = shift & 63;
    int16_t v[3];
    int row;

    v[0] = rd_s16(in);
    v[1] = (int16_t)(rd_s16(in + 2) - 0x28);
    v[2] = (int16_t)(rd_s16(in + 4) - 0xA3);
    for (row = 0; row < 3; row++) v[row] = (int16_t)(count > 15 ? (v[row] < 0 ? -1 : 0) : v[row] >> count);
    for (row = 0; row < 3; row++) {
        gaddr m = CAMERA_MATRIX + (gaddr)(6 * row);
        int32_t sum = (int32_t)v[2] * rd_s16(m + 4) + (int32_t)v[1] * rd_s16(m + 2) + (int32_t)v[0] * rd_s16(m);
        wr_s16(out + (gaddr)(2 * row), (int16_t)(sum >> 8));
    }
}

void grid_relative_position(gaddr record, int shift, int32_t out[3]) {
    uint16_t cell = rd_u16(record + 0x0E);
    int16_t column = (int16_t)((cell >> 8) - (rd_u16(GRID_ORIGIN_X) & 0xFF));
    int16_t row = (int16_t)((cell & 0xFF) - (rd_u16(GRID_ORIGIN_Z) & 0xFF));
    shift &= 15;
    out[0] = (int32_t)((uint32_t)(int32_t)rd_s16(record + 0x06) << shift) - ((int32_t)((uint32_t)(uint16_t)column << 16) >> 2);
    out[1] = (int32_t)((uint32_t)(int32_t)rd_s16(record + 0x08) << shift);
    out[2] = (int32_t)((uint32_t)(int32_t)rd_s16(record + 0x0A) << shift) - ((int32_t)((uint32_t)(uint16_t)row << 16) >> 2);
}

void append_list_point(int16_t x, int16_t y, int16_t z, int shift, uint16_t tag) {
    gaddr p = rd_u32(LIST_WRITE);
    int count = shift & 63;
    int32_t a, b;
    a = ((int32_t)x * rd_s16(LIST_MATRIX) + (int32_t)y * rd_s16(LIST_MATRIX + 2) + (int32_t)z * rd_s16(LIST_MATRIX + 4)) >> 8;
    b = ((int32_t)x * rd_s16(LIST_MATRIX + 12) + (int32_t)y * rd_s16(LIST_MATRIX + 14) + (int32_t)z * rd_s16(LIST_MATRIX + 16)) >> 8;
    a = count >= 32 ? 0 : (int32_t)((uint32_t)a << count);
    b = count >= 32 ? 0 : (int32_t)((uint32_t)b << count);
    wr_s32(p, a);
    wr_u32(p + 4, 0);
    wr_s32(p + 8, b);
    wr_u16(p + 12, tag);
    p += 16;
    wr_u32(p, 0);
    wr_u32(p + 4, 0);
    wr_u32(p + 8, 0);
    wr_u32(LIST_WRITE, p);
}

void rotate_by_view_matrix(const int16_t v[3], int32_t out[3]) {
    int row, k;
    for (row = 0; row < 3; row++) {
        uint32_t sum = 0;
        for (k = 0; k < 3; k++)
            sum += (uint32_t)((int32_t)rd_s16(VIEW_ANGLE_MATRIX + (gaddr)(6 * row + 2 * k)) * v[k]);
        out[row] = (int32_t)sum >> 8;
    }
    wr_s16(VIEW_DEPTH, (int16_t)out[2]);
}

void transform_ground_points(gaddr src, int16_t count, int16_t shift, const int16_t offset[3], gaddr out) {
    int16_t ox = (int16_t)(offset[0] + rd_s16(BOUND_OFFSET_X)), oz = (int16_t)(offset[2] + rd_s16(BOUND_OFFSET_Z));
    const int16_t *base = offset + 3;
    int count_bits = shift & 63;
    do {
        int16_t x = rd_s16(src), z = rd_s16(src + 2), row;
        src += 4;
        x = (int16_t)((count_bits >= 16 ? (x < 0 ? -1 : 0) : x >> count_bits) + ox);
        z = (int16_t)((count_bits >= 16 ? (z < 0 ? -1 : 0) : z >> count_bits) + oz);
        for (row = 0; row < 3; row++) {
            gaddr m = VIEW_ANGLE_MATRIX + (gaddr)(6 * row);
            int32_t sum = (int32_t)((uint32_t)(x * rd_s16(m)) + (uint32_t)(z * rd_s16(m + 4)));
            wr_s16(out, (int16_t)((sum >> 8) + base[row]));
            out += 2;
        }
    } while (--count > 0);
}
