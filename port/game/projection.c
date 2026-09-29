/* The bounded screen projection used by the HUD ($C2EC90). */
#include "projection.h"

#include "circle.h"
#include "fault.h"
#include "fixed_math.h"
#include "globals.h"
#include "memory.h"
#include "plot.h"

int project_view_point(int16_t x, int16_t y, int16_t depth) {
    int16_t sx, sy;
    if (x >= depth || y >= depth || (int16_t)-x >= depth || (int16_t)-y >= depth) {
        wr_u32(PROJECTED_PAIR, 0xFFFFFFFFu);
        return 0;
    }
    if (depth <= 0) {
        wr_u16(ERROR_CODE, 0x1B);
        fault_hook();
        return 0;
    }
    sx = (int16_t)((int32_t)x * 160 / depth + 160);
    sy = (int16_t)((int32_t)y * 90 / depth + 90);
    if (sx < 0) sx = 0;
    else if (sx >= 320) sx = 319;
    if (sy < 0) sy = 0;
    else if (sy >= 180) sy = 179;
    sx = (int16_t)(319 - sx);
    sy = (int16_t)(180 - sy);
    if (sy > rd_s16(LINE_LAST_ROW)) {
        wr_u32(PROJECTED_PAIR, 0xFFFFFFFFu);
        return 0;
    }
    wr_u16(PROJECTED_PAIR, (uint16_t)sx);
    wr_u16(PROJECTED_PAIR + 2, (uint16_t)sy);
    return 1;
}

int project_view_point_mode(int16_t x, int16_t y, int16_t depth,
                            int16_t mode, int16_t size, int16_t radius) {
    int16_t px, py;
    if (!project_view_point(x, y, depth)) return 0;
    px = rd_s16(PROJECTED_PAIR);
    py = rd_s16(PROJECTED_PAIR + 2);
    if (mode >= 0) {
        int16_t block = (int16_t)(0x30 >> (mode >= 16 ? 15 : mode));
        int16_t pair = (int16_t)(0x50 >> (mode >= 16 ? 15 : mode));
        if (block >= size) plot_pixel_block(px, py);
        else if (pair >= size) plot_pixel_pair(px, py);
        else plot_pixel(px, py);
        return 1;
    }
    switch (mode) {
    case -1: plot_pixel(px, py); return 1;
    case -2: plot_pixel_pair(px, py); return 1;
    case -3: plot_pixel_block(px, py); return 1;
    case -4: draw_filled_circle(px, py, radius); return 1;
    default: return 1;
    }
}

void draw_display_stream_point(uint32_t stream, int16_t mode,
                               int16_t size, int16_t radius) {
    gaddr point = DISPLAY_VERTEX_BASE + (gaddr)(int32_t)rd_s16(stream);
    wr_u16(CURRENT_COLOUR, rd_u16(stream + 2));
    project_view_point_mode(rd_s16(point), rd_s16(point + 2),
                            rd_s16(point + 4), mode, size, radius);
}

void draw_fixed_matrix_mark(void) {
    static const int16_t point[3] = { (int16_t)0xE000, 0x3800, (int16_t)0xE000 };
    int16_t result[3];
    int row;
    for (row = 0; row < 3; row++) {
        gaddr m = VIEW_ANGLE_MATRIX + (gaddr)(row * 6);
        uint32_t sum = (uint32_t)((int32_t)point[0] * rd_s16(m));
        sum += (uint32_t)((int32_t)point[1] * rd_s16(m + 2));
        sum += (uint32_t)((int32_t)point[2] * rd_s16(m + 4));
        result[row] = (int16_t)((int32_t)sum >> 8);
    }
    wr_u16(CURRENT_COLOUR, 9);
    project_view_point_mode(result[0], result[1], result[2], -4,
                            0, 8);
}

static int16_t shift_word(int16_t value, int16_t count) {
    unsigned bits = (uint16_t)count & 63;
    return bits >= 16 ? 0 : (int16_t)((uint16_t)value << bits);
}

void draw_scaled_view_circle(uint32_t point, int16_t shift, int16_t radius) {
    int16_t x = shift_word(rd_s16(point), shift);
    int16_t y = shift_word(rd_s16(point + 2), shift);
    int16_t z = shift_word(rd_s16(point + 4), shift);
    int16_t ax = x < 0 ? (int16_t)-x : x;
    int16_t ay = y < 0 ? (int16_t)-y : y;
    int16_t az = z < 0 ? (int16_t)-z : z;
    int16_t length = (int16_t)magnitude3(ax, ay, az);
    int16_t shown = 127;
    if (length > 0) {
        uint32_t numerator = (uint32_t)(int32_t)radius;
        uint32_t quotient = numerator / (uint16_t)length;
        shown = quotient > 0xFFFFu ? radius : (int16_t)quotient;
        if (shown > 127) shown = 127;
    }
    project_view_point_mode(x, y, z, -4, 0, shown);
}

void draw_scaled_stream_circle(uint32_t stream, int16_t shift) {
    gaddr point = DISPLAY_VERTEX_BASE + (gaddr)(int32_t)rd_s16(stream);
    wr_u16(CURRENT_COLOUR, rd_u16(stream + 2));
    draw_scaled_view_circle(point, shift, rd_s16(stream + 4));
}
