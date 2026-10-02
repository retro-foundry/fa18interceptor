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
    a = (int32_t)((uint32_t)((int32_t)x * rd_s16(LIST_MATRIX)) + (uint32_t)((int32_t)y * rd_s16(LIST_MATRIX + 2)) + (uint32_t)((int32_t)z * rd_s16(LIST_MATRIX + 4))) >> 8;
    b = (int32_t)((uint32_t)((int32_t)x * rd_s16(LIST_MATRIX + 12)) + (uint32_t)((int32_t)y * rd_s16(LIST_MATRIX + 14)) + (uint32_t)((int32_t)z * rd_s16(LIST_MATRIX + 16))) >> 8;
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

static int16_t row_dot(gaddr row, int16_t x, int16_t y, int16_t z) {
    return (int16_t)((int32_t)((uint32_t)((int32_t)x * rd_s16(row)) + (uint32_t)((int32_t)y * rd_s16(row + 2)) +
                               (uint32_t)((int32_t)z * rd_s16(row + 4))) >> 8);
}

static void rotate_store(gaddr matrix, int16_t x, int16_t y, int16_t z, gaddr out) {
    wr_s16(out, row_dot(matrix, x, y, z));
    wr_s16(out + 2, row_dot(matrix + 6, x, y, z));
    wr_s16(out + 4, row_dot(matrix + 12, x, y, z));
}

void transform_bound_points(int16_t count, int16_t first, gaddr frame) {
    gaddr bound = rd_u32(BOUND_RECORD), in = bound + 0xA + (gaddr)(int32_t)first, out;
    uint8_t mode = rd_u8(bound + 7);
    int down = (8 - rd_s16(frame - 6)) & 63, shift = rd_s16(frame - 8) & 63;
    int32_t x = rd_s32(frame - 0x20) + rd_s32(SHADOW_OFFSET_X);
    int32_t y = rd_s32(frame - 0x1C), z = rd_s32(frame - 0x18) + rd_s32(SHADOW_OFFSET_Z);

    if (mode & 1) {
        y += rd_s32(SHADOW_OFFSET_Y);
        x >>= down;
        y >>= down;
        z >>= down;
        out = WORKSPACES + (gaddr)(int32_t)first;
        do {
            int16_t px = (int16_t)(rd_s16(in) >> shift), py = (int16_t)(rd_s16(in + 2) >> shift);
            int16_t pz = (int16_t)(rd_s16(in + 4) >> shift);
            in += 6;
            if (rd_u8(frame - 0x7F) & 1) {
                view_transform(in - 6, rd_s16(frame - 8), out);
            } else {
                rotate_store(VIEW_ANGLE_MATRIX, (int16_t)(row_dot(BOUND_MATRIX, px, py, pz) + (int16_t)x),
                             (int16_t)(row_dot(BOUND_MATRIX + 6, px, py, pz) + (int16_t)y),
                             (int16_t)(row_dot(BOUND_MATRIX + 12, px, py, pz) + (int16_t)z), out);
            }
            out += 6;
        } while (--count > 0);
        return;
    }
    x >>= down;
    y >>= down;
    z >>= down;
    wr_u16(frame - 0x12, (uint16_t)y);
    if (!(mode & 2)) {
        out = WORKSPACES + (gaddr)(int32_t)first;
        do {
            int16_t px = (int16_t)((rd_s16(in) >> shift) + (int16_t)x);
            int16_t py = (int16_t)((rd_s16(in + 2) >> shift) + (int16_t)y);
            int16_t pz = (int16_t)((rd_s16(in + 4) >> shift) + (int16_t)z);
            in += 6;
            rotate_store(VIEW_ANGLE_MATRIX, px, py, pz, out);
            out += 6;
        } while (--count > 0);
        return;
    }
    /* Flat points: (x, z) pairs on the ground, moved by the frame's offset. */
    out = WORKSPACES + (gaddr)(int32_t)(int16_t)(first + (first >> 1));
    do {
        int16_t px = (int16_t)((rd_s16(in) >> shift) + (int16_t)x);
        int16_t pz = (int16_t)((rd_s16(in + 2) >> shift) + (int16_t)z);
        gaddr m = VIEW_ANGLE_MATRIX;
        in += 4;
        wr_s16(out, (int16_t)((int16_t)((int32_t)((uint32_t)((int32_t)px * rd_s16(m)) + (uint32_t)((int32_t)pz * rd_s16(m + 4))) >> 8) +
                              rd_s16(frame - 0x78)));
        wr_s16(out + 2, (int16_t)((int16_t)((int32_t)((uint32_t)((int32_t)px * rd_s16(m + 6)) + (uint32_t)((int32_t)pz * rd_s16(m + 10))) >> 8) +
                                  rd_s16(frame - 0x76)));
        wr_s16(out + 4, (int16_t)((int16_t)((int32_t)((uint32_t)((int32_t)px * rd_s16(m + 12)) + (uint32_t)((int32_t)pz * rd_s16(m + 16))) >> 8) +
                                  rd_s16(frame - 0x74)));
        out += 6;
    } while (--count > 0);
}
