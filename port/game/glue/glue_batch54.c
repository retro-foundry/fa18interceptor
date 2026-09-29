/* Glue for the segment grids $C20D68 and $C20904 and the block generators
 * $C21A20 and $C217EA. The grids keep their vectors, counters and result
 * in the caller's frame (-$38..-$7E(A6)); their register and frame traffic
 * is transcribed instruction by instruction, each segment's registers from
 * its two input points. */
#include "glue.h"
#include "ports_glue.h"

#include "draw_stream.h"
#include "globals.h"
#include "memory.h"

#define SEXT(v) ((uint32_t)(int32_t)(int16_t)(v))
#define W(n) ((int16_t)D(n))

void clipped_segment_registers(const int16_t p[3], const int16_t q[3]); /* glue_batch51.c */

static int16_t frame(int off) { return rd_s16(A(6) + (gaddr)(int32_t)off); }
static void set_frame(int off, int16_t v) { wr_s16(A(6) + (gaddr)(int32_t)off, v); }

/* MOVEM.W from memory into three data registers (sign-extended). */
static void load3(int first, gaddr a) {
    int k;
    for (k = 0; k < 3; k++) D(first + k) = SEXT(rd_u16(a + (gaddr)(2 * k)));
}

static void frame3_to(int first, int off) {
    int k;
    for (k = 0; k < 3; k++) D(first + k) = SEXT((uint16_t)frame(off + 2 * k));
}

static void to_frame3(int first, int off) {
    int k;
    for (k = 0; k < 3; k++) set_frame(off + 2 * k, W(first + k));
}

static void add_frame3(int first, int off) {
    int k;
    for (k = 0; k < 3; k++) SET_W(D(first + k), (uint16_t)(W(first + k) + frame(off + 2 * k)));
}

static void sub_frame3(int first, int off) {
    int k;
    for (k = 0; k < 3; k++) SET_W(D(first + k), (uint16_t)(W(first + k) - frame(off + 2 * k)));
}

static void add3(int dst, int src) {
    int k;
    for (k = 0; k < 3; k++) SET_W(D(dst + k), (uint16_t)(W(dst + k) + W(src + k)));
}

static void sub3(int dst, int src) {
    int k;
    for (k = 0; k < 3; k++) SET_W(D(dst + k), (uint16_t)(W(dst + k) - W(src + k)));
}

static void asr3(int first) {
    int k;
    for (k = 0; k < 3; k++) SET_W(D(first + k), (uint16_t)(W(first + k) >> 1));
}

static void copy3(int dst, int src) {
    int k;
    for (k = 0; k < 3; k++) SET_W(D(dst + k), D(src + k));
}

/* The segment from the registers first..first+2 to second..second+2. */
static void segment_regs(int first, int second) {
    int16_t p[3], q[3];
    int k;
    for (k = 0; k < 3; k++) {
        p[k] = W(first + k);
        q[k] = W(second + k);
    }
    clipped_segment_registers(p, q);
}

static void style_regs(void) {
    SET_W(D(0), 0x0F);
    SET_W(D(1), 0xFFFF);
    SET_W(D(2), 0);
    SET_W(D(3), 0);
}

int glue_C20D68(void) {
    gaddr stream = A(2);
    uint32_t a1 = A(1), a5 = A(5);
    uint16_t result = 0;
    int k;

    draw_segment_grid(&stream);
    style_regs();
    A(3) = WORKSPACES + SEXT(rd_u16(A(2) + 2));
    set_frame(-0x38, rd_s16(A(2) + 4));
    A(2) += 6;
    for (k = 0; k < 6; k++) D(1 + k) = SEXT(rd_u16(A(3) + (gaddr)(2 * k)));
    A(3) += 12;
    for (k = 0; k < 3; k++) SET_W(D(1 + k), (uint16_t)(W(1 + k) - rd_s16(A(3) + (gaddr)(2 * k))));
    to_frame3(1, -0x40);
    for (k = 0; k < 3; k++) SET_W(D(4 + k), (uint16_t)(W(4 + k) - rd_s16(A(3) + (gaddr)(2 * k))));
    to_frame3(4, -0x52);
    set_frame(-0x7E, 0);
    for (;;) {
        set_frame(-0x3A, rd_s16(A(2)));
        A(2) += 2;
        for (k = 0; k < 3; k++) SET_W(D(4 + k), 0);
        for (;;) {
            uint32_t a2, a3;
            to_frame3(4, -0x58);
            load3(1, A(3));
            asr3(4);
            add3(1, 4);
            {
                int16_t s0[3];
                for (k = 0; k < 3; k++) s0[k] = W(1 + k);
                load3(1, A(3));
                add_frame3(1, -0x52);
                add3(1, 4);
                a2 = A(2);
                a3 = A(3);
                {
                    int16_t s1[3];
                    for (k = 0; k < 3; k++) s1[k] = W(1 + k);
                    clipped_segment_registers(s0, s1);
                }
                A(2) = a2;
                A(3) = a3;
            }
            result |= (uint16_t)D(0);
            set_frame(-0x7E, (int16_t)result);
            set_frame(-0x3A, (int16_t)(frame(-0x3A) - 1));
            if (frame(-0x3A) <= 0) break;
            frame3_to(4, -0x58);
            add_frame3(4, -0x40);
        }
        set_frame(-0x38, (int16_t)(frame(-0x38) - 1));
        if (frame(-0x38) <= 0) break;
        A(3) += 6;
    }
    A(1) = a1;
    A(5) = a5;
    SET_W(D(0), result);
    flags_logic_w(D(0));
    return glue_return();
}

int glue_C20904(void) {
    gaddr stream = A(2);
    uint32_t a1 = A(1), a2, a5 = A(5);
    uint16_t result = 0;
    int pass, k;

    draw_segment_lattice(&stream);
    style_regs();
    set_frame(-0x7E, 0);
    A(3) = WORKSPACES + SEXT(rd_u16(A(2) + 2));
    set_frame(-0x38, rd_s16(A(2) + 4));
    set_frame(-0x3A, rd_s16(A(2) + 4));
    A(2) += 6;
    a2 = A(2);
    for (k = 0; k < 6; k++) D(1 + k) = SEXT(rd_u16(A(3) + (gaddr)(2 * k)));
    A(3) += 12;
    for (k = 0; k < 6; k++) SET_W(D(1 + k), (uint16_t)(W(1 + k) - rd_s16(A(3) + (gaddr)(2 * (k % 3)))));
    to_frame3(1, -0x46);
    to_frame3(4, -0x40);
    frame3_to(4, -0x40);
    asr3(4);
    load3(1, A(3));
    sub3(1, 4);
    add_frame3(1, -0x46);
    to_frame3(1, -0x4C);
    for (pass = 0; pass < 2; pass++) {
        int counter = pass ? -0x3A : -0x38;
        for (k = 0; k < 3; k++) D(4 + k) = 0;
        for (;;) {
            to_frame3(4, -0x58);
            asr3(4);
            if (!pass) {
                uint32_t a3 = A(3);
                load3(1, A(3));
                add3(1, 4);
                copy3(4, 1);
                add_frame3(4, -0x46);
                segment_regs(1, 4);
                A(3) = a3;
            } else {
                frame3_to(1, -0x4C);
                sub3(1, 4);
                copy3(4, 1);
                sub_frame3(4, -0x46);
                segment_regs(1, 4);
            }
            A(2) = a2;
            result |= (uint16_t)D(0);
            set_frame(-0x7E, (int16_t)result);
            set_frame(counter, (int16_t)(frame(counter) - 1));
            if (frame(counter) <= 0) break;
            frame3_to(4, -0x58);
            add_frame3(4, -0x40);
        }
    }
    A(1) = a1;
    A(2) = a2;
    A(5) = a5;
    SET_W(D(0), result);
    flags_logic_w(D(0));
    return glue_return();
}

/* $C21A20: the last pass's registers (d in D0-D2, point 5 + d in D3-D5,
 * point 4 + d in D6/D7/A4), then D0 = 0. */
int glue_C21A20(void) {
    gaddr stream = A(2), base = WORKSPACES + SEXT(rd_u16(A(2))), ref = WORKSPACES + SEXT(rd_u16(A(2) + 2)) + 12;
    int16_t d[3], p4[3], p5[3];
    int k;

    for (k = 0; k < 3; k++) {
        d[k] = (int16_t)(rd_s16(ref + (gaddr)(2 * k)) - rd_s16(base + (gaddr)(2 * k)));
        p4[k] = rd_s16(base + 24 + (gaddr)(2 * k));
        p5[k] = rd_s16(base + 30 + (gaddr)(2 * k));
    }
    offset_block_copies(&stream);
    for (k = 0; k < 3; k++) {
        D(k) = (SEXT(rd_u16(ref + (gaddr)(2 * k))) & 0xFFFF0000u) | (uint16_t)d[k];
        D(3 + k) = (SEXT((uint16_t)p5[k]) & 0xFFFF0000u) | (uint16_t)(p5[k] + d[k]);
    }
    D(6) = (SEXT((uint16_t)p4[0]) & 0xFFFF0000u) | (uint16_t)(p4[0] + d[0]);
    D(7) = (SEXT((uint16_t)p4[1]) & 0xFFFF0000u) | (uint16_t)(p4[1] + d[1]);
    A(4) = SEXT((uint16_t)p4[2]) + SEXT((uint16_t)d[2]);
    A(3) = WORKSPACES;
    A(2) = stream;
    D(0) = 0;
    flags_logic_l(0);
    return glue_return();
}

/* $C217EA: f = point 4 - point 6 in D0-D2 (then D0 = 0), point 11 + f in
 * D3-D5, point 12 + f in D6/D7/A4 (all read after the C wrote 11 and 12). */
int glue_C217EA(void) {
    gaddr stream = A(2), v = WORKSPACES + SEXT(rd_u16(A(2)));
    int16_t f[3], p11[3], p12[3];
    int k;

    extend_block_scaled(&stream);
    for (k = 0; k < 3; k++) {
        f[k] = (int16_t)(rd_s16(v + 0x18 + (gaddr)(2 * k)) - rd_s16(v + 0x24 + (gaddr)(2 * k)));
        p11[k] = rd_s16(v + 0x42 + (gaddr)(2 * k));
        p12[k] = rd_s16(v + 0x48 + (gaddr)(2 * k));
    }
    for (k = 0; k < 3; k++) {
        D(k) = (SEXT(rd_u16(v + 0x18 + (gaddr)(2 * k))) & 0xFFFF0000u) | (uint16_t)f[k];
        D(3 + k) = (SEXT((uint16_t)p11[k]) & 0xFFFF0000u) | (uint16_t)(p11[k] + f[k]);
    }
    D(6) = (SEXT((uint16_t)p12[0]) & 0xFFFF0000u) | (uint16_t)(p12[0] + f[0]);
    D(7) = (SEXT((uint16_t)p12[1]) & 0xFFFF0000u) | (uint16_t)(p12[1] + f[1]);
    A(4) = SEXT((uint16_t)p12[2]) + SEXT((uint16_t)f[2]);
    A(3) = v;
    A(2) = stream;
    D(0) = 0;
    flags_logic_l(0);
    return glue_return();
}
