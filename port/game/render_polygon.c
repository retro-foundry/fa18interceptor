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

/* Mask (A) combined with the page plane (B) into the plane (D). */
static const uint16_t composite_minterm[] = {
    [PLANE_CLEAR] = MINTERM_NOTA_AND_B,
    [PLANE_SET] = MINTERM_A_OR_B,
    [PLANE_COMPLEMENT] = MINTERM_A_XOR_B,
};

void composite_polygon_plane(int plane_index, PlaneOp op) {
    gaddr planes = rd_u32(PAGE_PLANE_TABLE);
    gaddr plane = rd_u32(planes + (gaddr)(int32_t)(int16_t)(plane_index * 4));
    gaddr dest = plane + rd_u32(POLY_PLANE_OFFSET);

    wr_u16(POLY_PLANE_BITS, (uint16_t)(rd_u16(POLY_PLANE_BITS) >> 1));

    wait_blitter();
    custom_write(BLTCON0, (uint16_t)(SRCA | SRCB | DEST | composite_minterm[op]));
    custom_write(BLTCON1, BLITREVERSE);
    custom_write_ptr(BLTAPT, rd_u32(POLY_MASK_SOURCE));
    custom_write_ptr(BLTBPT, dest);
    custom_write_ptr(BLTDPT, dest);
    custom_write(BLTSIZE, rd_u16(POLY_BLIT_SIZE));
}

/* One-dot blitter lines draw a single pixel per raster row, so the area fill
 * that follows toggles exactly once per edge on every row it crosses. An
 * edge covers the rows below its upper end down to its lower end (the shared
 * vertex row belongs to one edge only) and is clipped against `last_row`
 * along its major axis. Horizontal edges contribute nothing. */
void draw_polygon_edge(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t last_row) {
    int16_t x, row, dx, rows, rows_left, major, minor, length;
    uint16_t con0, con1 = LINEMODE | ONEDOT;
    int x_major;
    int32_t error;
    gaddr start;

    if (y1 == y0) return;
    if ((uint16_t)y1 > (uint16_t)y0) {
        rows = (int16_t)(y1 - y0 - 1);
        row = (int16_t)(y0 + 1);
        if (row > last_row) return;
        dx = (int16_t)(x1 - x0);
        x = x0;
    } else {
        rows = (int16_t)(y0 - y1 - 1);
        dx = (int16_t)(x0 - x1);
        row = (int16_t)(y1 + 1);
        if (row > last_row) return;
        x = x1;
    }
    start = rd_u32(POLY_MASK_PLANE) +
            (gaddr)(int32_t)(int16_t)(row * 40 + ((uint16_t)x >> 3));
    con0 = (uint16_t)(((x & 15) << 12) + (SRCA | SRCC | DEST | MINTERM_LINE_XOR));

    /* Octant: the blitter always steps down; x-major lines step x each dot. */
    if (dx >= 0) {
        x_major = (uint16_t)dx >= (uint16_t)rows;
        if (x_major) con1 |= OCT_SUD;
    } else {
        dx = (int16_t)-dx;
        x_major = (uint16_t)dx >= (uint16_t)rows;
        con1 |= x_major ? (OCT_SUD | OCT_AUL) : OCT_SUL;
    }

    rows_left = (int16_t)(last_row - row);
    if (x_major) {
        major = dx;
        minor = rows;
        if (minor > rows_left) {
            /* Shorten to the visible fraction, rounding to nearest:
             * length = 2 * rows_left * dx / rows, halved with round-up. */
            int32_t scaled = (int32_t)((uint32_t)((int32_t)rows_left * dx) * 2u);
            int32_t quotient = scaled / minor;
            uint16_t low = (quotient >= -32768 && quotient <= 32767) ? (uint16_t)quotient
                                                                    : (uint16_t)scaled;
            length = (int16_t)(((int16_t)low >> 1) + (low & 1));
        } else {
            length = major;
        }
    } else {
        major = rows;
        minor = dx;
        length = major > rows_left ? rows_left : major;
    }

    /* Bresenham terms in the blitter's 4x scale. */
    error = (int32_t)(int16_t)(minor * 4) - (int32_t)(int16_t)(major * 2);
    if (error < 0) con1 |= SIGNFLAG;

    wait_blitter();
    custom_write(BLTCON0, con0);
    custom_write(BLTCON1, con1);
    custom_write(BLTAMOD, (uint16_t)(minor * 4 - major * 4));
    custom_write(BLTDMOD, 40);
    custom_write(BLTCMOD, 40);
    custom_write(BLTAPTL, (uint16_t)(minor * 4 - major * 2));
    custom_write_ptr(BLTDPT, start);
    custom_write_ptr(BLTCPT, start);
    custom_write_ptr(BLTAFWM, 0xFFFFFFFFu);
    custom_write(BLTBDAT, 0xFFFF);
    custom_write(BLTADAT, 0x8000);
    custom_write(BLTBMOD, (uint16_t)(minor * 4));
    custom_write(BLTSIZE, (uint16_t)(((uint16_t)length << 6) + 0x42)); /* length+1 rows, 2 words */
}

void clear_polygon_mask(void) {
    gaddr mask = rd_u32(POLY_MASK_END);

    /* A, B and D all address the mask; NOT A AND B is zero everywhere. */
    wait_blitter();
    custom_write(BLTCON0, (uint16_t)(SRCA | SRCB | DEST | MINTERM_NOTA_AND_B));
    custom_write(BLTCON1, BLITREVERSE);
    custom_write_ptr(BLTAPT, mask);
    custom_write_ptr(BLTBPT, mask);
    custom_write_ptr(BLTDPT, mask);
    custom_write(BLTSIZE, rd_u16(POLY_BLIT_SIZE));
}
