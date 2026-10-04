/* Glue for the projected segment $C2ED70, the top-plane crossing $C2F128,
 * and the edge alignment tests $C2082A/$C2084A.
 * The register flows are replayed after the C: the projection keeps each
 * DIVS remainder in the upper word. */
#include "glue.h"
#include "ports_glue.h"

#include "control_records.h"
#include "globals.h"
#include "memory.h"
#include "plane_tests.h"
#include "render_line.h"
#include "view_marks.h"

#define SEXT(v) ((uint32_t)(int32_t)(int16_t)(v))
#define W(n) ((int16_t)D(n))

void line_registers(void);      /* glue_render_polygon.c */
void normalize_registers(void); /* glue_batch42.c */

static void divs_reg(int n, int16_t divisor) {
    int32_t dividend = (int32_t)D(n), q = dividend / divisor;
    if (q != (int16_t)q) return;
    D(n) = ((uint32_t)(uint16_t)(dividend % divisor) << 16) | (uint16_t)q;
}

/* One point of $C2ED70 in registers x, y, z (the scratch D5 alongside):
 * 1 when it projects, leaving x and y as the mirrored screen position. */
static int project_regs(gaddr p, int x, int y, int z) {
    SET_W(D(x), rd_u16(p));
    SET_W(D(y), rd_u16(p + 2));
    SET_W(D(z), rd_u16(p + 4));
    if (W(z) <= 0 || W(x) > W(z)) return 0;
    SET_W(D(5), (uint16_t)-W(x));
    if (W(5) > W(z) || W(y) > W(z)) return 0;
    SET_W(D(5), (uint16_t)-W(y));
    if (W(5) > W(z)) return 0;
    D(x) = (uint32_t)((int32_t)W(x) * 0xA0);
    divs_reg(x, W(z));
    SET_W(D(x), W(x) + 0xA0);
    if (W(x) < 0) SET_W(D(x), 0);
    else if (W(x) >= 0x140) SET_W(D(x), 0x13F);
    D(y) = (uint32_t)((int32_t)W(y) * 0x5A);
    divs_reg(y, W(z));
    SET_W(D(y), W(y) + 0x5A);
    if (W(y) < 0) SET_W(D(y), 0);
    else if (W(y) >= 0xB4) SET_W(D(y), 0xB3);
    SET_W(D(x), (uint16_t)(0x13F - W(x)));
    SET_W(D(y), (uint16_t)(0xB3 - W(y)));
    return 1;
}

void projected_segment_registers(void) {
    A(1) = SEGMENT_POINTS + 12;
    if (project_regs(SEGMENT_POINTS, 0, 1, 2) && project_regs(SEGMENT_POINTS + 6, 2, 3, 4)) {
        line_registers();
        D(0) = 1;
    } else {
        D(0) = 0;
    }
    flags_logic_l(D(0));
}

int glue_C2ED70(void) {
    draw_projected_segment();
    projected_segment_registers();
    return glue_return();
}

/* $C2084A's body, A3 the edge; the frame words -$26/-$22/-$28(A6) are the
 * eye x and z and the range. */






void plot_registers(gaddr masks, gaddr writers); /* glue_batch33.c */

/* $C348B2: D0/D1 the position, D4 the variant. The bounds checks' and the
 * pixel loop's registers, with plot_pixel's leftovers. */
