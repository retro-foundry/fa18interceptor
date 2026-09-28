/* Blitter line drawing. */
#include "render_line.h"

#include "globals.h"
#include "hardware.h"
#include "memory.h"

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
    LineSetup line;
    gaddr planes;
    int bit;

    wr_u16(POLY_PLANE_BITS, rd_u16(CURRENT_COLOUR));
    if (!setup_line(x0, y0, x1, y1, rd_s16(LINE_LAST_ROW), 1, 0, &line)) return;

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
}
