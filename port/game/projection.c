/* The bounded screen projection used by the HUD ($C2EC90). */
#include "projection.h"

#include "circle.h"
#include "fault.h"
#include "fixed_math.h"
#include "globals.h"
#include "memory.h"
#include "plot.h"
#include "render_line.h"
#include "render_polygon.h"

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

/* The shape tables ($C2D16C): per kind, byte (x, y, z) offsets for each
 * part and a colour per part; each part's word in SHAPE_POINT_SETS picks
 * its list of word (x, y, z) points. */
#define SHAPE_POINT_SETS 0xC2D03Eu
#define SHAPE_POINT_LISTS 0xC2CEBEu

typedef struct { gaddr offsets, colours; int parts; } ShapeKind;

static ShapeKind shape_kind(int8_t kind) {
    ShapeKind k = {0xC2CF6Eu, 0xC2D001u, 12};
    if (kind <= 1) return k;
    if (kind == 2) { k.colours = 0xC2D00Du; return k; }
    if (kind == 3) { k.offsets = 0xC2CFE3u; k.colours = 0xC2D019u; k.parts = 9; return k; }
    if (kind == 4) { k.offsets = 0xC2CFBCu; k.colours = 0xC2D025u; return k; }
    k.offsets = 0xC2CF95u;
    k.colours = 0xC2D031u;
    return k;
}

/* A view-space point to the screen, or 0 outside the view. */
static int shape_point(int16_t x, int16_t y, int16_t z, int16_t shift, int16_t *sx, int16_t *sy) {
    int16_t vx, vy, vz;
    x = (int16_t)(x >> shift);
    y = (int16_t)(y >> shift);
    z = (int16_t)(z >> shift);
    vx = (int16_t)((int32_t)((uint32_t)((int32_t)rd_s16(VIEW_ANGLE_MATRIX) * x) +
                             (uint32_t)((int32_t)rd_s16(VIEW_ANGLE_MATRIX + 2) * y) +
                             (uint32_t)((int32_t)rd_s16(VIEW_ANGLE_MATRIX + 4) * z)) >> 8);
    vy = (int16_t)((int32_t)((uint32_t)((int32_t)rd_s16(VIEW_ANGLE_MATRIX + 6) * x) +
                             (uint32_t)((int32_t)rd_s16(VIEW_ANGLE_MATRIX + 8) * y) +
                             (uint32_t)((int32_t)rd_s16(VIEW_ANGLE_MATRIX + 10) * z)) >> 8);
    {
        int32_t depth = (int32_t)((uint32_t)((int32_t)rd_s16(VIEW_ANGLE_MATRIX + 12) * x) +
                                  (uint32_t)((int32_t)rd_s16(VIEW_ANGLE_MATRIX + 14) * y) +
                                  (uint32_t)((int32_t)rd_s16(VIEW_ANGLE_MATRIX + 16) * z)) >> 8;
        if (depth <= 0) return 0;
        vz = (int16_t)depth;
    }
    if (vx > vz || (int16_t)-vx > vz || vy > vz || (int16_t)-vy > vz) return 0;
    *sx = (int16_t)((int32_t)vx * 160 / vz + 160);
    if (*sx < 0) *sx = 0;
    else if (*sx >= 320) *sx = 319;
    *sy = (int16_t)((int32_t)vy * 90 / vz + 90);
    if (*sy < 0) *sy = 0;
    else if (*sy >= 180) *sy = 179;
    *sx = (int16_t)(319 - *sx);
    *sy = (int16_t)(179 - *sy);
    return 1;
}

int draw_shape(int16_t x, int16_t y, int16_t z, uint16_t scale, int8_t kind, int16_t radius, int16_t shift) {
    ShapeKind k = shape_kind(kind);
    int drawn = 0, part;

    wr_u16(POLY_VERTICES, 3);
    for (part = k.parts - 1; part >= 0; part--) {
        gaddr offset = k.offsets + (gaddr)(3 * part), points;
        int16_t px = (int16_t)(x + (int16_t)((int16_t)((int8_t)rd_u8(offset) * scale) >> shift));
        int16_t py = (int16_t)(y + (int16_t)((int16_t)((int8_t)rd_u8(offset + 1) * scale) >> shift));
        int16_t pz = (int16_t)(z + (int16_t)((int16_t)((int8_t)rd_u8(offset + 2) * scale) >> shift));
        uint16_t set = rd_u16(SHAPE_POINT_SETS + (gaddr)(2 * part + 2 * scale));
        gaddr out = POLY_VERTICES + 2;
        int visible = 1;

        wr_u16(CURRENT_COLOUR, kind ? rd_u8(k.colours + (gaddr)part) : 13);
        points = rd_u32(SHAPE_POINT_LISTS + (gaddr)set);
        do {
            int16_t sx, sy, dx = 0, dy = 0, dz = 0;
            if (kind != 3) {
                dx = rd_s16(points);
                dy = rd_s16(points + 2);
                dz = rd_s16(points + 4);
                points += 6;
            }
            if (!shape_point((int16_t)(dx + px), (int16_t)(dy + py), (int16_t)(dz + pz), shift, &sx, &sy)) {
                visible = 0;
                break;
            }
            wr_u16(out, (uint16_t)sx);
            wr_u16(out + 2, (uint16_t)sy);
            out += 4;
        } while (kind != 3 && out < POLY_VERTICES + 14);
        if (!visible) continue;
        if (kind == 0) draw_line(rd_s16(POLY_VERTICES + 2), rd_s16(POLY_VERTICES + 4), rd_s16(POLY_VERTICES + 6),
                                 rd_s16(POLY_VERTICES + 8));
        else if (kind == 3) draw_filled_circle(rd_s16(POLY_VERTICES + 2), rd_s16(POLY_VERTICES + 4), radius);
        else draw_polygon();
        drawn = 1;
    }
    return drawn;
}
