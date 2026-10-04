/* Glue for render_polygon.c. */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "hardware.h"
#include "memory.h"
#include "render_line.h"
#include "render_polygon.h"

/* $C30466: D0.w plane table offset, D3 bit 0 colour bit, D4 bit 0 complement. */
void composite_registers(void);
void composite_registers(void) {
    int16_t offset = (int16_t)D(0);
    uint32_t plane_offset, plane;
    uint16_t size;

    size = rd_u16(POLY_BLIT_SIZE);
    A(2) = rd_u32(PAGE_PLANE_TABLE);
    plane = rd_u32(A(2) + (uint32_t)(int32_t)offset);
    plane_offset = rd_u32(POLY_PLANE_OFFSET);
    D(2) = rd_u32(POLY_MASK_SOURCE);
    D(1) = plane_offset + plane;
    SET_W(D(0), size);
    A(0) = 0xDFF000;
    /* X is the carry of ADD.L D2,D1 (offset + plane); later moves keep it. */
    FLAG_X = (D(1) < plane_offset) ? XFLAG_SET : XFLAG_CLEAR;
    flags_logic_w(size); /* final MOVE.W D0,BLTSIZE */
}

int glue_C30466(void) {
    PlaneOp op = (D(4) & 1) ? PLANE_COMPLEMENT : (D(3) & 1) ? PLANE_SET : PLANE_CLEAR;
    composite_polygon_plane((int16_t)D(0) >> 2, op);
    composite_registers();
    return glue_return();
}

/* $C304B2: no inputs. */
void clear_mask_registers(void);
void clear_mask_registers(void) {
    uint16_t size;

    size = rd_u16(POLY_BLIT_SIZE);
    D(2) = rd_u32(POLY_MASK_END);
    D(1) = D(2);
    SET_W(D(0), size);
    A(0) = 0xDFF000;
    flags_logic_w(size);
}



/* $C305AA: D0-D3 = x0, y0, x1, y1 (words), A4.w = last row. Live outputs at
 * its call sites: D4 (low word), A1, and the D4/D5 high words, which the
 * original swaps with EXG on y-major edges. */
/* The registers $C305AA leaves from D0-D3/A4 in the registers; `size` is the
 * BLTSIZE last written (its own when it drew). */
void polygon_edge_registers(uint16_t size);
void polygon_edge_registers(uint16_t size) {
    int16_t x0 = (int16_t)D(0), y0 = (int16_t)D(1), x1 = (int16_t)D(2), y1 = (int16_t)D(3);
    int16_t last_row = (int16_t)A(4), row, dx, rows;

    if (y1 == y0) return;
    if ((uint16_t)y1 > (uint16_t)y0) {
        rows = (int16_t)(y1 - y0 - 1);
        row = (int16_t)(y0 + 1);
        if (row > last_row) return;
        dx = (int16_t)(x1 - x0);
    } else {
        rows = (int16_t)(y0 - y1 - 1);
        dx = (int16_t)(x0 - x1);
        SET_W(D(4), dx);
        row = (int16_t)(y1 + 1);
        if (row > last_row) return;
    }
    if (dx < 0) dx = (int16_t)-dx;
    if ((uint16_t)dx < (uint16_t)rows) { /* y-major: EXG D4,D5 */
        uint32_t d4 = D(4);
        D(4) = D(5);
        D(5) = d4;
    }
    A(1) = (uint32_t)(int32_t)row;
    SET_W(D(4), size);
}

int glue_C305AA(void) {
    draw_polygon_edge((int16_t)D(0), (int16_t)D(1), (int16_t)D(2), (int16_t)D(3), (int16_t)A(4));
    polygon_edge_registers(custom_written(BLTSIZE));
    return glue_return();
}

/* $C2FA7E: D0-D3 = x0, y0, x1, y1 (words). Every register is live at its
 * call sites (polyline loops reuse the setup), so the original's leftovers
 * are rebuilt from the same LineSetup. */
/* The registers $C2FA7E leaves, from the D0-D6 in the registers (no drawing). */
void line_registers_to_row(int16_t last_row);
void line_registers(void);
void line_registers(void) { line_registers_to_row(rd_s16(LINE_LAST_ROW)); }

void line_registers_to_row(int16_t last_row) {
    int16_t x0 = (int16_t)D(0), y0 = (int16_t)D(1), x1 = (int16_t)D(2), y1 = (int16_t)D(3);
    uint32_t d2 = D(2), d3 = D(3), d4 = D(4), d5 = D(5), d6 = D(6), d3_high;
    LineSetup line;
    int bit, last_bit = -1;

    A(2) = (uint32_t)(int32_t)last_row;
    if (!setup_line(x0, y0, x1, y1, last_row, 1, 0, &line)) {
        if (y1 == y0) {
            SET_W(D(1), y0 + 1);
            SET_W(D(3), y1 + 1);
            SET_W(D(5), 0);
        } else if ((uint16_t)y1 > (uint16_t)y0) {
            SET_W(D(5), y1 - y0 - 1);
            SET_W(D(1), y0 + 1);
        } else {
            SET_W(D(5), y0 - y1 - 1);
            SET_W(D(4), x0 - x1);
            SET_W(D(3), y1 + 1);
        }
        return;
    }

    d3_high = d3 & 0xFFFF0000u;
    if (line.clipped) {
        /* MULS/ADD.L/DIVS left the remainder (or, on overflow, the dividend). */
        int16_t rows_left = (int16_t)(last_row - line.row);
        int32_t scaled = (int32_t)((uint32_t)((int32_t)rows_left * line.dx) * 2u);
        int32_t quotient = scaled / line.rows;
        if (quotient >= -32768 && quotient <= 32767)
            d3_high = (uint32_t)(uint16_t)(scaled % line.rows) << 16;
        else
            d3_high = (uint32_t)scaled & 0xFFFF0000u;
    }
    for (bit = 0; bit < 4; bit++)
        if ((rd_u8(LINE_PLANES) >> bit) & 1) last_bit = bit;

    D(0) = 0xFFFFFFFFu;
    D(1) = line.con1;
    D(2) = (d2 & 0xFFFF0000u) | (uint16_t)line.error;
    D(3) = d3_high | (uint16_t)line.step_minor;
    /* EXG D4,D5 on y-major lines swaps their high words. */
    D(4) = (line.x_major ? d4 : d5) & 0xFFFF0000u;
    D(5) = (line.x_major ? d5 : d4) & 0xFFFF0000u;
    SET_W(D(4), line.size);
    D(6) = (d6 & 0xFFFF0000u) | (uint16_t)(line.shift + 0x0B00);
    A(0) = 0xDFF000;
    A(1) = (uint32_t)line.offset;
    A(3) = (uint32_t)(int32_t)line.step_both;
    if (last_bit < 0) {
        SET_W(D(5), line.step_both);
        D(7) = (uint32_t)line.offset;
        A(2) = (uint32_t)(int32_t)line.length;
    } else {
        uint8_t colour = rd_s16(LINE_COLOUR) >= 0 ? rd_u8(LINE_COLOUR + 1) : rd_u8(POLY_PLANE_BITS + 1);
        A(2) = rd_u32(PAGE_PLANE_TABLE);
        SET_W(D(5), line.shift + 0x0B00 + (((colour >> last_bit) & 1) ? 0xFA : 0x0A));
        D(7) = rd_u32(A(2) + (uint32_t)(4 * (3 - last_bit))) + (uint32_t)line.offset;
    }
}
