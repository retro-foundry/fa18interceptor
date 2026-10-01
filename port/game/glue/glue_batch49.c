/* Glue for the projected segment $C2ED70, the top-plane crossing $C2F128,
 * the in-sight flag $C2436A and the edge alignment tests $C2082A/$C2084A.
 * The register flows are replayed after the C: the projection keeps each
 * DIVS remainder in the upper word, and normalize_vector's register entry
 * is re-run through its idempotent helper. */
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

static void muls_mem(int n, gaddr a) { D(n) = (uint32_t)((int32_t)W(n) * rd_s16(a)); }

/* $C2436A: A1 target, A3 viewer. */
int glue_C2436A(void) {
    gaddr t = A(1), v = A(3);
    int k;

    SET_B(D(0), rd_u8(t + 0x39) & 0xF0);
    if ((uint8_t)D(0) == 0x10 && rd_s16(v + 0x4A) <= 0x3000) {
        for (k = 0; k < 3; k++)
            D(5 + k) = (uint32_t)((int32_t)(rd_u32(v + 0x14 + (gaddr)(4 * k)) - rd_u32(t + 0x14 + (gaddr)(4 * k))) >> 8);
        SET_W(D(0), 0xC0);
        normalize_registers();
        muls_mem(5, v + 0x96);
        muls_mem(6, v + 0x9C);
        muls_mem(7, v + 0xA2);
        {
            int32_t first = (int32_t)(D(7) + D(5));
            D(7) = (uint32_t)first + D(6);
            if ((int64_t)first + (int32_t)D(6) < 0 && (int32_t)D(7) <= -0x2C0000) {
                SET_W(D(2), rd_u16(v + 0x96));
                SET_W(D(3), rd_u16(v + 0x9C));
                SET_W(D(4), rd_u16(v + 0xA2));
                muls_mem(2, t + 0x96);
                muls_mem(3, t + 0x9C);
                muls_mem(4, t + 0xA2);
                D(4) += D(2);
                D(4) += D(3);
            }
        }
    }
    update_in_sight(t, v);
    return glue_return();
}

/* $C2084A's body, A3 the edge; the frame words -$26/-$22/-$28(A6) are the
 * eye x and z and the range. */
static void alignment_regs(int result) {
    int16_t eye_x = rd_s16(A(6) - 0x26), eye_z = rd_s16(A(6) - 0x22), range = rd_s16(A(6) - 0x28);
    int16_t n1[3];
    gaddr e = A(3);
    int k;

    SET_W(D(5), (uint16_t)-(int16_t)(rd_s16(e) - rd_s16(e + 4)));
    SET_W(D(7), (uint16_t)-(int16_t)(rd_s16(e + 2) - rd_s16(e + 6)));
    A(3) = e + 8;
    SET_W(D(6), 0);
    SET_W(D(0), 0x100);
    normalize_registers();
    for (k = 0; k < 3; k++) n1[k] = W(5 + k);
    SET_W(D(5), (uint16_t)(eye_x - rd_s16(BOUND_OFFSET_X)));
    SET_W(D(6), 0);
    SET_W(D(7), (uint16_t)(eye_z - rd_s16(BOUND_OFFSET_Z)));
    SET_W(D(0), 0x100);
    normalize_registers();
    for (k = 0; k < 3; k++) D(k) = (uint32_t)((int32_t)n1[k] * W(5 + k));
    {
        int32_t first = (int32_t)(D(2) + D(0));
        D(2) = (uint32_t)first + D(1);
        if ((int64_t)first + (int32_t)D(1) < 0) D(2) = 0u - D(2);
        D(2) = (uint32_t)((int32_t)D(2) >> 4);
    }
    A(3) = rd_s32(PROJECTION_Y) > -0x80 ? ALIGNMENT_NEAR : ALIGNMENT_FAR;
    SET_W(D(0), (uint16_t)(range >> 4));
    if (W(0) > 10) SET_W(D(0), 11);
    SET_W(D(0), (uint16_t)(W(0) * 2));
    SET_W(D(1), rd_u16(A(3) + SEXT(D(0))));
    D(0) = (uint32_t)result;
    flags_logic_l(D(0));
}

int glue_C2084A(void) {
    int16_t eye_x = rd_s16(A(6) - 0x26), eye_z = rd_s16(A(6) - 0x22), range = rd_s16(A(6) - 0x28);
    int result = edge_alignment(A(3), eye_x, eye_z, range);
    alignment_regs(result);
    return glue_return();
}

int glue_C2082A(void) {
    int16_t eye_x = rd_s16(A(6) - 0x26), eye_z = rd_s16(A(6) - 0x22), range = rd_s16(A(6) - 0x28);
    gaddr stream = A(2);
    int result;

    A(3) = rd_u32(BOUND_RECORD) + 0xA + SEXT(rd_u16(A(2)));
    result = edge_alignment_test(&stream, eye_x, eye_z, range);
    A(2) = stream;
    if (rd_s32(PROJECTION_Y) <= -0x140 || rd_u8(ATTITUDE_NEAR)) {
        D(0) = 0;
        flags_logic_l(0);
    } else {
        alignment_regs(result);
    }
    return glue_return();
}

void plot_registers(gaddr masks, gaddr writers); /* glue_batch33.c */

/* $C348B2: D0/D1 the position, D4 the variant. The bounds checks' and the
 * pixel loop's registers, with plot_pixel's leftovers. */
void symbol_registers(void) {
    int16_t x = W(0), y = W(1), large = W(4);
    gaddr shape;

    if (!large) {
        if (x <= 0x58 || x >= 0xE6 || y <= 0x27 || y >= 0x8D) return;
        shape = SYMBOL_SMALL;
    } else {
        SET_W(D(2), rd_u16(STREAM_SKIP) & 3);
        if (!W(2) || x <= 0x60 || x >= 0xDE || y <= 0x2E || y >= 0x86) return;
        shape = SYMBOL_LARGE;
    }
    A(0) = shape;
    SET_W(D(0), W(0) + rd_s16(SPAN_ORIGIN_Y));
    if (W(0) < 10 || W(0) > 0x136) return;
    SET_W(D(1), W(1) + rd_s16(REDRAW_STATE_WORD));
    x = W(0);
    y = W(1);
    for (;;) {
        SET_B(D(0), rd_u8(A(0)));
        SET_B(D(1), rd_u8(A(0) + 1));
        A(0) += 2;
        SET_B(D(2), (uint8_t)(D(0) | D(1)));
        if (!(uint8_t)D(2)) break;
        SET_W(D(0), (uint16_t)((int8_t)D(0) + x));
        SET_W(D(1), (uint16_t)((int8_t)D(1) + y));
        {
            uint32_t a0 = A(0);
            plot_registers(PIXEL_MASKS, PLOT_ROWS_1);
            A(0) = a0;
        }
    }
}

int glue_C348B2(void) {
    plot_symbol(W(0), W(1), W(4));
    symbol_registers();
    return glue_return();
}
