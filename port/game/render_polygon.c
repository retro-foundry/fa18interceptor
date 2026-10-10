/* Polygon compositing.
 *
 * Polygons are rasterised by drawing their outline with one-dot lines into a
 * single-plane mask buffer and filling it with the blitter. The filled mask is
 * then applied to each of the page's bitplanes: planes whose colour bit is set
 * get the mask ORed in, the others get it cleared. All blits run descending,
 * from the last word of the bounding box. */
#include "render_polygon.h"

#include "globals.h"
#include "hardware.h"
#include "memory.h"
#include "plot.h"
#include "render_buffers.h"
#include "render_line.h"
#ifdef FA18_NATIVE
#include "render/raster.h"
#endif

/* Mask (A) combined with the page plane (B) into the plane (D). */
#ifndef FA18_NATIVE
static const uint16_t composite_minterm[] = {
    [PLANE_CLEAR] = MINTERM_NOTA_AND_B,
    [PLANE_SET] = MINTERM_A_OR_B,
    [PLANE_COMPLEMENT] = MINTERM_A_XOR_B,
};
#endif

void composite_polygon_plane(int plane_index, PlaneOp op) {
    gaddr planes = rd_u32(PAGE_PLANE_TABLE);
    gaddr plane = rd_u32(planes + (gaddr)(int32_t)(int16_t)(plane_index * 4));
    gaddr dest = plane + rd_u32(POLY_PLANE_OFFSET);

    wr_u16(POLY_PLANE_BITS, (uint16_t)(rd_u16(POLY_PLANE_BITS) >> 1));

#ifdef FA18_NATIVE
    native_raster_composite(rd_u32(POLY_MASK_SOURCE),dest,rd_u16(POLY_BLIT_SIZE),op);
#else
    wait_blitter();
    custom_write(BLTCON0, (uint16_t)(SRCA | SRCB | DEST | composite_minterm[op]));
    custom_write(BLTCON1, BLITREVERSE);
    custom_write_ptr(BLTAPT, rd_u32(POLY_MASK_SOURCE));
    custom_write_ptr(BLTBPT, dest);
    custom_write_ptr(BLTDPT, dest);
    custom_write(BLTSIZE, rd_u16(POLY_BLIT_SIZE));
#endif
}

/* One-dot blitter lines draw a single pixel per raster row, so the area fill
 * that follows toggles exactly once per edge on every row it crosses. The
 * shared vertex row belongs to one edge only (setup_line starts below the
 * upper end), and horizontal edges contribute nothing. */
void draw_polygon_edge(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t last_row) {
    LineSetup edge;
#ifndef FA18_NATIVE
    gaddr start;
#endif

    if (!setup_line(x0, y0, x1, y1, last_row, 0, 1, &edge)) return;
#ifdef FA18_NATIVE
    native_raster_line(rd_u32(POLY_MASK_PLANE),&edge,PLANE_COMPLEMENT,1);
#else
    start = rd_u32(POLY_MASK_PLANE) + (gaddr)edge.offset;

    wait_blitter();
    custom_write(BLTCON0, (uint16_t)(edge.shift + (SRCA | SRCC | DEST | MINTERM_LINE_XOR)));
    custom_write(BLTCON1, edge.con1);
    custom_write(BLTAMOD, (uint16_t)edge.step_both);
    custom_write(BLTDMOD, 40);
    custom_write(BLTCMOD, 40);
    custom_write(BLTAPTL, (uint16_t)edge.error);
    custom_write_ptr(BLTDPT, start);
    custom_write_ptr(BLTCPT, start);
    custom_write_ptr(BLTAFWM, 0xFFFFFFFFu);
    custom_write(BLTBDAT, 0xFFFF);
    custom_write(BLTADAT, 0x8000);
    custom_write(BLTBMOD, (uint16_t)edge.step_minor);
    custom_write(BLTSIZE, edge.size);
#endif
}

void clear_polygon_mask(void) {
    gaddr mask = rd_u32(POLY_MASK_END);

#ifdef FA18_NATIVE
    native_raster_clear(mask,rd_u16(POLY_BLIT_SIZE));
#else
    /* A, B and D all address the mask; NOT A AND B is zero everywhere. */
    wait_blitter();
    custom_write(BLTCON0, (uint16_t)(SRCA | SRCB | DEST | MINTERM_NOTA_AND_B));
    custom_write(BLTCON1, BLITREVERSE);
    custom_write_ptr(BLTAPT, mask);
    custom_write_ptr(BLTBPT, mask);
    custom_write_ptr(BLTDPT, mask);
    custom_write(BLTSIZE, rd_u16(POLY_BLIT_SIZE));
#endif
}

static int16_t abs16(int16_t v) { return v < 0 ? (int16_t)-v : v; }

/* The bounding box as the original scans it: the first two points order
 * each axis, and every later point only moves the side it passes (the
 * third point moves the minimum when below it, else the maximum). */
static void polygon_bounds(gaddr v, int count, int16_t *minx, int16_t *miny, int16_t *maxx, int16_t *maxy) {
    int16_t x0 = rd_s16(v), y0 = rd_s16(v + 2), x1 = rd_s16(v + 4), y1 = rd_s16(v + 6);
    int16_t x2 = rd_s16(v + 8), y2 = rd_s16(v + 10);
    int i;
    if (x1 < x0) { int16_t t = x0; x0 = x1; x1 = t; }
    if (x2 < x0) x0 = x2; else if (x2 > x1) x1 = x2;
    if (y1 < y0) { int16_t t = y0; y0 = y1; y1 = t; }
    if (y2 < y0) y0 = y2; else if (y2 > y1) y1 = y2;
    for (i = 3; i < count; i++) {
        int16_t x = rd_s16(v + (gaddr)(4 * i)), y = rd_s16(v + (gaddr)(4 * i + 2));
        if (x <= x0) x0 = x; else if (x > x1) x1 = x;
        if (y <= y0) y0 = y; else if (y > y1) y1 = y;
    }
    *minx = x0; *maxx = x1; *miny = y0; *maxy = y1;
}

int prepare_polygon(void) { return prepare_polygon_to_row(rd_s16(LINE_LAST_ROW)); }

int prepare_polygon_to_row(int16_t last) {
    int16_t minx, miny, maxx, maxy, height, width;
    gaddr v = POLY_VERTICES + 2;
    int16_t count = rd_s16(POLY_VERTICES), i;

    polygon_bounds(v, count, &minx, &miny, &maxx, &maxy);
    if (miny > last) return 1;
    height = abs16((int16_t)(maxy - miny));
    width = abs16((int16_t)(maxx - minx));
    if (height > 2 ? width > 1 : height == 2 ? width > 2 : 0) goto fill;
    if (height > 2 || (height < 2 && width > 1)) {
        uint32_t style = rd_u32(LINE_STYLE);
        if (!rd_u8(KEEP_LINE_STYLE)) wr_u32(LINE_STYLE, 0x000FFFFFu);
        draw_line(minx, miny, maxx, maxy);
        wr_u32(LINE_STYLE, style);
        return 1;
    }
    {
        int16_t row = (int16_t)(miny + 1);
        if (row > last) return 1;
        if (height == 2) plot_pixel_block(maxx, row);
        else if (width == 0) plot_pixel(minx, row);
        else plot_pixel_pair(maxx, row);
        return 1;
    }

fill:
    {
        int16_t left = (int16_t)(minx - 1 < 0 ? 0 : minx - 1);
        int16_t words, modulo, clipped = 0;
        int32_t corner, cut = 0;
        uint16_t size;

        wr_s16(POLY_MIN_X, left);
        wr_s16(POLY_MAX_X, maxx);
        wr_s16(POLY_MIN_Y, miny);
        wr_s16(POLY_MAX_Y, maxy);
        wr_u16(POLY_PLANE_BITS, rd_u16(CURRENT_COLOUR));
        wr_s16(POLY_EDGES_LEFT, (int16_t)(count - 1));
        i = 0;
        do {
            gaddr p = v + (gaddr)(4 * i);
            draw_polygon_edge(rd_s16(p), rd_s16(p + 2), rd_s16(p + 4), rd_s16(p + 6), last);
            i++;
            wr_s16(POLY_EDGES_LEFT, (int16_t)(rd_s16(POLY_EDGES_LEFT) - 1));
        } while (rd_s16(POLY_EDGES_LEFT) > 0);
        draw_polygon_edge(rd_s16(v + (gaddr)(4 * i)), rd_s16(v + (gaddr)(4 * i + 2)), rd_s16(v), rd_s16(v + 2), last);

        words = (int16_t)((maxx >> 4) - (left >> 4));
        modulo = (int16_t)(0x27 - 2 * words);
        corner = (int32_t)(uint16_t)(maxy * 40) + (int16_t)(2 * (maxx >> 4));
        if (maxy > last) {
            clipped = (int16_t)(maxy - last);
            cut = (int16_t)(clipped * 40);
        }
        wr_s32(POLY_PLANE_OFFSET, corner - cut);
        wr_s32(POLY_MASK_END, corner + rd_s32(POLY_MASK_PLANE) - cut);
        wr_s32(POLY_MASK_SOURCE, rd_s32(POLY_MASK_END));
        size = (uint16_t)(((uint16_t)((int16_t)(maxy - miny + 1) - clipped) << 6) + (uint16_t)(words + 1));
        wr_u16(POLY_BLIT_SIZE, size);

#ifdef FA18_NATIVE
        (void)modulo; /* Direct plane rows use the same effective stride. */
        native_raster_fill(rd_u32(POLY_MASK_END),size);
#else
        wait_blitter();
        custom_write(BLTCON0, 0x09F0);
        custom_write(BLTCON1, 0x000A);
        custom_write_ptr(BLTAPT, rd_u32(POLY_MASK_END));
        custom_write_ptr(BLTDPT, rd_u32(POLY_MASK_END));
        custom_write_ptr(BLTAFWM, 0xFFFFFFFFu);
        custom_write(BLTAMOD, (uint16_t)modulo);
        custom_write(BLTBMOD, (uint16_t)modulo);
        custom_write(BLTDMOD, (uint16_t)modulo);
        custom_write(BLTSIZE, size);
#endif
    }
    return 0;
}

void draw_polygon(void) {
    int bit, given, complement_all = 0;
    uint8_t planes;

#ifndef FA18_NATIVE
    custom_write(DMACON, 0x8400); /* blitter priority */
#endif
    if (prepare_polygon()) return;
    given = rd_s16(LINE_COLOUR) >= 0;
    if (given && rd_u16(POLY_MASK_BLIT)) {
        blit_mask_between_planes();
    } else {
        planes = rd_u8(LINE_PLANES);
        for (bit = 0; bit < 4; bit++) {
            int set, complement;
            if (!(planes & (1 << bit))) {
                /* A skipped plane still consumes its colour bit, except the
                 * last: the original goes straight on to the mask clear. */
                if (bit < 3) wr_u16(POLY_PLANE_BITS, (uint16_t)(rd_u16(POLY_PLANE_BITS) >> 1));
                continue;
            }
            if (given) {
                set = (rd_s16(LINE_COLOUR) >> bit) & 1;
                complement = (rd_s16(POLY_COMPLEMENT) >> bit) & 1;
            } else {
                set = rd_u16(POLY_PLANE_BITS) & 1;
                if (bit == 0) complement_all = rd_u16(POLY_COMPLEMENT) & 1;
                complement = complement_all;
            }
            composite_polygon_plane(3 - bit, complement ? PLANE_COMPLEMENT : set ? PLANE_SET : PLANE_CLEAR);
        }
    }
    clear_polygon_mask();
#ifndef FA18_NATIVE
    custom_write(DMACON, 0x0400);
#endif
}

#define MARK_POLYGON 0xC4B432u /* word count, then (x, y) word pairs at 1/256 scale */

int scale_mark_polygon(void) {
    gaddr from = MARK_POLYGON, to = POLY_VERTICES;
    int16_t count = rd_s16(from);

    from += 2;
    if (count <= 0) return 0;
    wr_u16(to, (uint16_t)count);
    for (to += 2; count-- > 0; from += 4, to += 4) {
        wr_u16(to, (uint16_t)((int16_t)((rd_u16(from) * 0x18u) >> 8) + 0xC1 + rd_s16(SPAN_ORIGIN_Y)));
        wr_u16(to + 2, (uint16_t)((int16_t)((rd_u16(from + 2) * 0x1Fu) >> 8) + 0xA2 + rd_s16(REDRAW_STATE_WORD)));
    }
    return 1;
}

void draw_mark_polygon(void) {
    if (!scale_mark_polygon() || prepare_polygon_to_row(0xC7)) return;
    blit_lane(4, 1);
    clear_polygon_mask();
}
