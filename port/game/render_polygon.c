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
