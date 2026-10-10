/* Blitter line drawing. */
#include "render_line.h"

#include "clip.h"
#include "globals.h"
#include "hardware.h"
#include "memory.h"
#ifdef FA18_NATIVE
#include "render/raster.h"
#endif

#define ROW_BYTES 40

int setup_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t last_row,
               int keep_horizontal, int one_dot, LineSetup *line) {
    int16_t dx, rows_left;

    if (y1 == y0) {
        if (!keep_horizontal) return 0;
        line->rows = 0;
        line->row = (int16_t)(y0 + 1);
        if (line->row > last_row) return 0;
        dx = (int16_t)(x1 - x0);
        line->x = x0;
    } else if ((uint16_t)y1 > (uint16_t)y0) {
        line->rows = (int16_t)(y1 - y0 - 1);
        line->row = (int16_t)(y0 + 1);
        if (line->row > last_row) return 0;
        dx = (int16_t)(x1 - x0);
        line->x = x0;
    } else {
        line->rows = (int16_t)(y0 - y1 - 1);
        dx = (int16_t)(x0 - x1);
        line->dx = dx; /* C2FAC2 computes the delta before the last-row rejection. */
        line->row = (int16_t)(y1 + 1);
        if (line->row > last_row) return 0;
        line->x = x1;
    }
    line->offset = (int32_t)(int16_t)(line->row * ROW_BYTES + ((uint16_t)line->x >> 3));
    line->shift = (uint16_t)((line->x & 15) << 12);

    /* Octant: the blitter always steps down; x-major lines step x each dot. */
    line->con1 = (uint16_t)(LINEMODE | (one_dot ? ONEDOT : 0));
    if (dx >= 0) {
        line->x_major = (uint16_t)dx >= (uint16_t)line->rows;
        if (line->x_major) line->con1 |= OCT_SUD;
    } else {
        dx = (int16_t)-dx;
        line->x_major = (uint16_t)dx >= (uint16_t)line->rows;
        line->con1 |= line->x_major ? (OCT_SUD | OCT_AUL) : OCT_SUL;
    }
    line->dx = dx;

    rows_left = (int16_t)(last_row - line->row);
    {
        int16_t major, minor;
        line->clipped = 0;
        if (line->x_major) {
            major = dx;
            minor = line->rows;
            if (minor > rows_left) {
                /* Shorten to the visible fraction, rounding to nearest:
                 * length = 2 * rows_left * dx / rows, halved with round-up. */
                int32_t scaled = (int32_t)((uint32_t)((int32_t)rows_left * dx) * 2u);
                int32_t quotient = scaled / minor;
                uint16_t low = (quotient >= -32768 && quotient <= 32767) ? (uint16_t)quotient
                                                                        : (uint16_t)scaled;
                line->length = (int16_t)(((int16_t)low >> 1) + (low & 1));
                line->clipped = 1;
            } else {
                line->length = major;
            }
        } else {
            major = line->rows;
            minor = dx;
            line->length = major > rows_left ? rows_left : major;
        }
        /* Bresenham terms in the blitter's 4x scale. */
        if ((int32_t)(int16_t)(minor * 4) - (int32_t)(int16_t)(major * 2) < 0) line->con1 |= SIGNFLAG;
        line->error = (int16_t)(minor * 4 - major * 2);
        line->step_minor = (int16_t)(minor * 4);
        line->step_both = (int16_t)(minor * 4 - major * 4);
    }
    line->size = (uint16_t)(((uint16_t)line->length << 6) + 0x42);
    return 1;
}

/* The colour of a line: its own when LINE_COLOUR is non-negative, else the
 * current polygon colour. */
static int colour_bit(int plane_bit) {
    if (rd_s16(LINE_COLOUR) >= 0) return (rd_u8(LINE_COLOUR + 1) >> plane_bit) & 1;
    return (rd_u8(POLY_PLANE_BITS + 1) >> plane_bit) & 1;
}

void draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
    draw_line_to_row(x0, y0, x1, y1, rd_s16(LINE_LAST_ROW));
}

void draw_line_to_row(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t last_row) {
    draw_line_to_row_result(x0,y0,x1,y1,last_row);
}

LineDrawResult draw_line_to_row_result(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t last_row) {
    LineSetup line;
    gaddr planes;
    int bit;

    wr_u16(POLY_PLANE_BITS, rd_u16(CURRENT_COLOUR));
    if (!setup_line(x0, y0, x1, y1, last_row, 1, 0, &line)) {
        if((uint16_t)y1<(uint16_t)y0)
            return (LineDrawResult){.kind=LINE_DRAW_X_DELTA,.x_delta=line.dx};
        return (LineDrawResult){0};
    }

#ifdef FA18_NATIVE
    planes = rd_u32(PAGE_PLANE_TABLE);
    for(bit=0;bit<4;++bit) {
        if(!((rd_u8(LINE_PLANES)>>bit)&1)) continue;
        native_raster_line(rd_u32(planes+4u*(3-bit)),&line,
                          colour_bit(bit)?PLANE_SET:PLANE_CLEAR,0);
    }
#else
    wait_blitter();
    custom_write(BLTAMOD, (uint16_t)line.step_both);
    custom_write(BLTDMOD, ROW_BYTES);
    custom_write(BLTCMOD, ROW_BYTES);
    custom_write_ptr(BLTAFWM, 0xFFFFFFFFu);
    custom_write(BLTBDAT, 0xFFFF);

    /* Plane-enable bit n draws into plane table entry 3 - n. */
    planes = rd_u32(PAGE_PLANE_TABLE);
    for (bit = 0; bit < 4; bit++) {
        uint16_t minterm;
        gaddr dest;
        if (!((rd_u8(LINE_PLANES) >> bit) & 1)) continue;
        minterm = colour_bit(bit) ? 0xFA /* A | C */ : 0x0A /* NOT A AND C */;
        dest = rd_u32(planes + (gaddr)(4 * (3 - bit))) + (gaddr)line.offset;
        wait_blitter();
        custom_write(BLTCON0, (uint16_t)(line.shift + (SRCA | SRCC | DEST) + minterm));
        custom_write(BLTCON1, line.con1);
        custom_write(BLTAPTL, (uint16_t)line.error);
        custom_write_ptr(BLTCPT, dest);
        custom_write_ptr(BLTDPT, dest);
        custom_write(BLTADAT, 0x8000);
        custom_write(BLTBMOD, (uint16_t)line.step_minor);
        custom_write(BLTSIZE, line.size);
    }
#endif
    /* C2FB4C also returns the signed blitter stride. C20656 retains it as
     * the base of subsequent vertex offsets after a clipped pair draws. */
    return (LineDrawResult){.kind=LINE_DRAW_SIZE,.size=line.size,.step_both=line.step_both};
}

void reset_line_style(void) {
    /* One long write: LINE_PLANES = all four, LINE_COLOUR = -1 (object colour). */
    wr_u32(LINE_PLANES - 1, 0x000FFFFFu);
}

/* A view-space point on the 320 x 180 view, mirrored as the clipper does;
 * 0 when it is behind or outside the view pyramid. */
static int project_point(gaddr p, int16_t *x, int16_t *y) {
    int16_t px = rd_s16(p), py = rd_s16(p + 2), pz = rd_s16(p + 4), sx, sy;
    if (pz <= 0 || px > pz || -px > pz || py > pz || -py > pz) return 0;
    sx = (int16_t)((int32_t)px * 0xA0 / pz + 0xA0);
    if (sx < 0) sx = 0; else if (sx >= 0x140) sx = 0x13F;
    sy = (int16_t)((int32_t)py * 0x5A / pz + 0x5A);
    if (sy < 0) sy = 0; else if (sy >= 0xB4) sy = 0xB3;
    *x = (int16_t)(0x13F - sx);
    *y = (int16_t)(0xB3 - sy);
    return 1;
}

int draw_projected_segment(void) {
    int16_t x0, y0, x1, y1;
    if (!project_point(SEGMENT_POINTS, &x0, &y0) || !project_point(SEGMENT_POINTS + 6, &x1, &y1)) return 0;
    draw_line(x0, y0, x1, y1);
    return 1;
}

/* ---- a segment clipped to the view pyramid ($C2EE4A) ---------------------- */

typedef struct { int16_t x, y, z; } SegmentPoint;

static SegmentPoint segment_point(gaddr a) {
    SegmentPoint p;
    p.x = rd_s16(a);
    p.y = rd_s16(a + 2);
    p.z = rd_s16(a + 4);
    return p;
}

/* The truncated crossing with the segment's second point. */
static int enters(void *context, int axis, int side) {
    const int16_t *p = context;
    return !clip_to_view_plane(SEGMENT_POINTS + 6, p[0], p[1], p[2], axis, side, 0);
}

static int16_t divs_quotient(int32_t dividend, int16_t divisor) {
    int32_t q = dividend / divisor;
    return q == (int16_t)q ? (int16_t)q : (int16_t)dividend;
}

int draw_clipped_segment(void) {
    return draw_clipped_segment_result().drawn;
}

SegmentDrawResult draw_clipped_segment_result(void) {
    gaddr out = POLY_VERTICES;
    int pass;
    SegmentDrawResult result={0};

    for (pass = 0; pass < 2; pass++) {
        SegmentPoint p = segment_point(SEGMENT_POINTS), q = segment_point(SEGMENT_POINTS + 6), e;
        int16_t pv[3], qv[3], sx, sy;
        int plane;
        result=(SegmentDrawResult){.kind=SEGMENT_DRAW_ENDPOINT_Y,.y=p.y}; /* C2EE60. */
        pv[0] = p.x; pv[1] = p.y; pv[2] = p.z;
        qv[0] = q.x; qv[1] = q.y; qv[2] = q.z;
        switch (clip_edge_end(pv, qv, enters, pv, &plane)) {
        case CLIP_END_POINT: e = p; break;
        case CLIP_END_CROSSING: e = segment_point(CLIP_POINT); break;
        default: return result;
        }
        result.y=e.y; /* C2F03A loads the accepted crossing, when selected. */
        if (e.z <= 0) {
            wr_u16(ERROR_CODE, 0x16);
            return result;
        }
        sx = (int16_t)(divs_quotient((int32_t)e.x * 0xA0, e.z) + 0xA0);
        if (sx < 0) sx = 0; else if (sx >= 0x140) sx = 0x13F;
        sy = (int16_t)(divs_quotient((int32_t)e.y * 0x5A, e.z) + 0x5A);
        if (sy < 0) sy = 0; else if (sy >= 0xB4) sy = 0xB3;
        wr_s16(out, (int16_t)(0x13F - sx));
        wr_s16(out + 2, (int16_t)(0xB3 - sy));
        result.kind=SEGMENT_DRAW_SCREEN_Y;result.y=(int16_t)(0xB3-sy);
        out += 4;
        if (!pass) {
            /* The other end next: the two points change places. */
            SegmentPoint first = p;
            wr_s16(SEGMENT_POINTS, q.x); wr_s16(SEGMENT_POINTS + 2, q.y); wr_s16(SEGMENT_POINTS + 4, q.z);
            wr_s16(SEGMENT_POINTS + 6, first.x); wr_s16(SEGMENT_POINTS + 8, first.y); wr_s16(SEGMENT_POINTS + 10, first.z);
        }
    }
    result.line=draw_line_to_row_result(rd_s16(POLY_VERTICES), rd_s16(POLY_VERTICES + 2),
        rd_s16(POLY_VERTICES + 4), rd_s16(POLY_VERTICES + 6),rd_s16(LINE_LAST_ROW));
    if(result.line.kind!=LINE_DRAW_NONE) result.kind=SEGMENT_DRAW_LINE;
    result.drawn=1;
    return result;
}
