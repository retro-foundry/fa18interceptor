/* Glue for render_polygon.c. */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "hardware.h"
#include "memory.h"
#include "render_polygon.h"

/* $C30466: D0.w plane table offset, D3 bit 0 colour bit, D4 bit 0 complement. */
int glue_C30466(void) {
    int16_t offset = (int16_t)D(0);
    PlaneOp op = (D(4) & 1) ? PLANE_COMPLEMENT : (D(3) & 1) ? PLANE_SET : PLANE_CLEAR;
    uint32_t plane_offset, plane;
    uint16_t size;

    composite_polygon_plane(offset >> 2, op);

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
    return glue_return();
}

/* $C304B2: no inputs. */
int glue_C304B2(void) {
    uint16_t size;

    clear_polygon_mask();

    size = rd_u16(POLY_BLIT_SIZE);
    D(2) = rd_u32(POLY_MASK_END);
    D(1) = D(2);
    SET_W(D(0), size);
    A(0) = 0xDFF000;
    flags_logic_w(size);
    return glue_return();
}

/* $C305AA: D0-D3 = x0, y0, x1, y1 (words), A4.w = last row. Live outputs at
 * its call sites: D4 (low word), A1, and the D4/D5 high words, which the
 * original swaps with EXG on y-major edges. */
int glue_C305AA(void) {
    int16_t x0 = (int16_t)D(0), y0 = (int16_t)D(1), x1 = (int16_t)D(2), y1 = (int16_t)D(3);
    int16_t last_row = (int16_t)A(4), row, dx, rows;

    draw_polygon_edge(x0, y0, x1, y1, last_row);

    if (y1 == y0) return glue_return();
    if ((uint16_t)y1 > (uint16_t)y0) {
        rows = (int16_t)(y1 - y0 - 1);
        row = (int16_t)(y0 + 1);
        if (row > last_row) return glue_return();
        dx = (int16_t)(x1 - x0);
    } else {
        rows = (int16_t)(y0 - y1 - 1);
        dx = (int16_t)(x0 - x1);
        SET_W(D(4), dx);
        row = (int16_t)(y1 + 1);
        if (row > last_row) return glue_return();
    }
    if (dx < 0) dx = (int16_t)-dx;
    if ((uint16_t)dx < (uint16_t)rows) { /* y-major: EXG D4,D5 */
        uint32_t d4 = D(4);
        D(4) = D(5);
        D(5) = d4;
    }
    A(1) = (uint32_t)(int32_t)row;
    SET_W(D(4), custom_written(BLTSIZE));
    return glue_return();
}
