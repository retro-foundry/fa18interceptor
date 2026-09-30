/* Glue for project_corner_edges $C2E758. Its caller reads the last corner's
 * registers: D1/D3 the neighbour offsets, D2/D6 as the plane tests left
 * them, D3-D5 the rounded screen position (DIVS remainders above) or the
 * corner itself, A3 its CORNER_SCREEN entry. The C runs first; the chain is
 * replayed with the crossings computed in hand, starting from CLIP_POINT
 * as it was (a parallel rounded test leaves it). The routine also writes 7
 * into its caller's argument word at 4(A7). */
#include "glue.h"
#include "ports_glue.h"

#include "clip.h"
#include "globals.h"
#include "memory.h"

#define SEXT(v) ((uint32_t)(int32_t)(int16_t)(v))
#define W(n) ((int16_t)D(n))

int edge_end_registers(const int16_t p[3], const int16_t q[3], int rounded, int16_t last[3]); /* glue_batch51.c */

static void divs_reg(int n, int16_t divisor) {
    int32_t dividend = (int32_t)D(n), q = dividend / divisor;
    if (q != (int16_t)q) return;
    D(n) = ((uint32_t)(uint16_t)(dividend % divisor) << 16) | (uint16_t)q;
}

/* MULS / DIVS / ASR.W #1 with the carry added back / ADDI and the clamp. */
static void screen_reg(int n, int16_t span, int16_t half) {
    int carry;
    D(n) = (uint32_t)((int32_t)W(n) * span);
    divs_reg(n, W(5));
    carry = W(n) & 1;
    SET_W(D(n), (uint16_t)(W(n) >> 1));
    if (carry) SET_W(D(n), (uint16_t)(W(n) + 1));
    SET_W(D(n), (uint16_t)(W(n) + half));
    if (W(n) < 0) SET_W(D(n), 0);
    else if (W(n) >= span) SET_W(D(n), (uint16_t)(span - 1));
}

void corner_edges_registers(int apply_stack) {
    int16_t last[3];
    int i, k, skip = 0;

    for (k = 0; k < 3; k++) last[k] = rd_s16(CLIP_POINT + (gaddr)(2 * k));
    if (apply_stack) wr_u16(A(7) + 4, 7);
    A(0) = CORNER_SCREEN;
    A(1) = CORNER_RECORDS;
    for (i = 0; i <= 7; i++) {
        int16_t from, to;
        SET_W(D(0), (uint16_t)i);
        if (i & 1) {
            from = (int16_t)(i + 1 > 7 ? 0 : i + 1);
            to = (int16_t)(i - 1);
        } else {
            from = (int16_t)i;
            to = (int16_t)(i + 2 > 7 ? 0 : i + 2);
        }
        SET_W(D(3), (uint16_t)from);
        SET_W(D(1), (uint16_t)(to << 4));
        SET_W(D(6), (uint16_t)(i << 3));
        A(3) = CORNER_SCREEN + SEXT(D(6));
        if (skip) {
            skip = 0;
            continue;
        }
        SET_W(D(3), (uint16_t)(from << 4));
        for (k = 0; k < 3; k++) D(3 + k) = SEXT(rd_u16(CORNER_RECORDS + (gaddr)(16 * from) + (gaddr)(2 * k)));
        SET_W(D(3), (uint16_t)(W(3) + 1));
        SET_W(D(4), (uint16_t)(W(4) + 1));
        {
            int16_t p[3], q[3];
            int end;
            for (k = 0; k < 3; k++) {
                p[k] = W(3 + k);
                q[k] = rd_s16(CORNER_RECORDS + (gaddr)(16 * to) + (gaddr)(2 * k));
            }
            end = edge_end_registers(p, q, 1, last);
            if (end == CLIP_END_POINT) {
                if (i & 1) skip = 1;
            } else if (end == CLIP_END_NONE) {
                SET_W(D(6), (uint16_t)(i << 4));
            } else {
                for (k = 0; k < 3; k++) D(3 + k) = SEXT((uint16_t)last[k]);
                screen_reg(3, 0x140, 0xA0);
                screen_reg(4, 0xB4, 0x5A);
            }
        }
    }
    SET_W(D(0), 8);
}

int glue_C2E758(void) {
    project_corner_edges();
    corner_edges_registers(1);
    return glue_return();
}
