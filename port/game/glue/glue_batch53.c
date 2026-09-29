/* Glue for the draw-stream commands ($C212B0, $C2129C, $C211DC, $C2131C,
 * $C20E4E, $C20E40, $C21490, $C2139E, $C21412, $C20F10, $C20EC4). Their
 * callers read every register. The C runs first; then each command's
 * register flow is replayed from the stream and the vertex table (neither
 * changes), with each segment's registers from its two input points and a
 * face's from the clipper snapshot taken before the C. The segment loops
 * keep their result and last-pair flag in the caller's frame, -$7E(A6) and
 * -$6E(A6) (a run's count at -$30(A6)); those are written too. */
#include "glue.h"
#include "ports_glue.h"

#include "draw_stream.h"
#include "globals.h"
#include "memory.h"
#include "glue_clip.h"

#define SEXT(v) ((uint32_t)(int32_t)(int16_t)(v))
#define W(n) ((int16_t)D(n))

void clipped_segment_registers(const int16_t p[3], const int16_t q[3]); /* glue_batch51.c */

static void read3(gaddr a, int16_t v[3]) {
    int k;
    for (k = 0; k < 3; k++) v[k] = rd_s16(a + (gaddr)(2 * k));
}

/* A segment call with A2 saved round it (MOVE.L A2,-(A7) ... MOVEA.L). */
static int segment_call(const int16_t p[3], const int16_t q[3]) {
    uint32_t a2 = A(2);
    clipped_segment_registers(p, q);
    A(2) = a2;
    return (int)(uint16_t)D(0);
}

static void loop_end(uint32_t a1, uint32_t a5, uint16_t result) {
    A(1) = a1;
    A(5) = a5;
    SET_W(D(0), result);
    flags_logic_w(D(0));
}

/* $C212B0's loop from A2 (past the colour). */
static int pairs_regs(void) {
    uint32_t a1 = A(1), a5 = A(5);
    uint16_t result = 0;
    int last = 0;
    wr_u16(A(6) - 0x7E, 0);
    wr_u16(A(6) - 0x6E, 0);
    while (!last) {
        gaddr va, vb;
        int16_t p[3], q[3];
        SET_W(D(1), rd_u16(A(2)));
        SET_W(D(2), rd_u16(A(2) + 2));
        A(2) += 4;
        if (W(2) < 0) {
            last = 1;
            wr_u16(A(6) - 0x6E, 1);
            SET_W(D(2), W(2) & 0x7FFF);
        }
        A(3) = WORKSPACES;
        va = WORKSPACES + SEXT(D(1));
        vb = WORKSPACES + SEXT(D(2));
        SET_W(D(6), rd_u16(va + 4));
        A(4) = vb + 4;
        A(0) = SEGMENT_POINTS + 10;
        SET_W(D(6), W(6) & rd_s16(vb + 4));
        if (W(6) < 0) continue;
        read3(va, p);
        read3(vb, q);
        result |= (uint16_t)segment_call(p, q);
        wr_u16(A(6) - 0x7E, result);
    }
    loop_end(a1, a5, result);
    return result;
}

int glue_C212B0(void) {
    gaddr stream = A(2);
    draw_segment_pairs(&stream);
    A(2) += 2;
    pairs_regs();
    return glue_return();
}

int glue_C2129C(void) {
    gaddr stream = A(2);
    draw_segment_pairs_near(&stream);
    if (rd_s32(PROJECTION_Y) < -0xC0) {
        A(2) += 2;
        pairs_regs();
    } else {
        A(2) = stream;
        D(0) = 0;
        flags_logic_l(0);
    }
    return glue_return();
}

int glue_C211DC(void) {
    gaddr stream = A(2);
    uint16_t head = rd_u16(A(2)), result = 0;
    int16_t count = (int16_t)(head >> 8);
    uint32_t a1 = A(1), a2, a5 = A(5);

    draw_segment_run(&stream);
    SET_W(D(0), head >> 8);
    SET_W(D(1), head & 0x3F);
    A(3) = WORKSPACES + SEXT(rd_u16(A(2) + 2));
    A(2) += 4;
    a2 = A(2);
    wr_u16(A(6) - 0x7E, 0);
    do {
        int16_t p[3], q[3];
        uint32_t a3;
        int k;
        read3(A(3), p);
        read3(A(3) + 6, q);
        for (k = 0; k < 3; k++) {
            D(k) = SEXT(p[k]);
            D(3 + k) = SEXT(q[k]);
        }
        A(3) += 12;
        a3 = A(3);
        clipped_segment_registers(p, q);
        A(3) = a3;
        result |= (uint16_t)D(0);
        wr_u16(A(6) - 0x7E, result);
    } while (--count > 0);
    wr_u16(A(6) - 0x30, (uint16_t)count);
    A(2) = a2;
    loop_end(a1, a5, result);
    return glue_return();
}

int glue_C2131C(void) {
    gaddr stream = A(2);
    uint32_t a1 = A(1), a5 = A(5);
    uint16_t result = 0;
    int16_t base, shift[3];
    int last = 0, k;

    draw_offset_segments(&stream);
    base = rd_s16(A(2) + 2);
    A(2) += 4;
    SET_W(D(0), (uint16_t)base);
    wr_u16(A(6) - 0x7E, 0);
    wr_u16(A(6) - 0x6E, 0);
    {
        int16_t v0[3], v1[3];
        read3(WORKSPACES + SEXT((uint16_t)base), v0);
        read3(WORKSPACES + SEXT((uint16_t)base) + 6, v1);
        for (k = 0; k < 3; k++) shift[k] = (int16_t)(v1[k] - v0[k]);
    }
    while (!last) {
        int16_t p[3], q[3], va[3], vb[3], a, b;
        a = rd_s16(A(2));
        A(2) += 2;
        if (rd_s16(A(2)) < 0) {
            last = 1;
            wr_u16(A(6) - 0x6E, 1);
        }
        b = (int16_t)(rd_s16(A(2)) & 0x7FFF);
        A(2) += 2;
        A(3) = WORKSPACES;
        A(0) = SEGMENT_POINTS;
        read3(WORKSPACES + SEXT((uint16_t)a), va);
        read3(WORKSPACES + SEXT((uint16_t)b), vb);
        {
            int16_t n1[3];
            read3(WORKSPACES + SEXT((uint16_t)base) + 6, n1);
            for (k = 0; k < 3; k++) D(5 + k) = (SEXT(n1[k]) & 0xFFFF0000u) | (uint16_t)shift[k];
        }
        for (k = 0; k < 3; k++) {
            p[k] = (int16_t)(va[k] - shift[k]);
            q[k] = (int16_t)(vb[k] - shift[k]);
            D(2 + k) = (SEXT(vb[k]) & 0xFFFF0000u) | (uint16_t)q[k];
        }
        SET_W(D(1), (uint16_t)b);
        result |= (uint16_t)segment_call(p, q);
        SET_W(D(0), (uint16_t)base);
        wr_u16(A(6) - 0x7E, result);
    }
    loop_end(a1, a5, result);
    return glue_return();
}

/* ---- faces: the registers at the clipper call, then the clipper's -------- */

static void face_end(ClipperSnapshot *snapshot, uint16_t colour, int drawn, int behind) {
    if (behind) {
        D(0) = 0;
        flags_logic_l(0);
    } else {
        clipper_registers(snapshot, colour, drawn);
    }
}

static int parallelogram_glue(uint16_t planes, uint16_t colour_word, uint16_t complement, int d0, int d1, int d2) {
    gaddr stream = A(2), v;
    uint16_t colour = rd_u16(A(2));
    uint32_t a1 = A(1), a2, a5 = A(5);
    int16_t p[4][3];
    ClipperSnapshot snapshot;
    int drawn, k, behind;

    clipper_snapshot(&snapshot);
    drawn = planes == 8 ? draw_parallelogram_face(&stream) : draw_parallelogram_face_2(&stream);
    (void)colour_word; (void)complement;
    SET_W(D(0), (uint16_t)d0);
    SET_W(D(1), (uint16_t)d1);
    SET_W(D(2), (uint16_t)d2);
    v = WORKSPACES + SEXT(rd_u16(A(2) + 2));
    A(2) += 4;
    a2 = A(2);
    read3(v, p[0]);
    read3(v + 6, p[1]);
    read3(v + 12, p[2]);
    for (k = 0; k < 3; k++) {
        D(2 + k) = SEXT(p[0][k]);
        D(5 + k) = SEXT(p[1][k]);
    }
    SET_W(D(0), W(4) & W(7));
    for (k = 0; k < 3; k++) SET_W(D(5 + k), W(5 + k) - W(2 + k));
    for (k = 0; k < 3; k++) D(2 + k) = SEXT(p[2][k]);
    SET_W(D(0), W(0) & W(4));
    for (k = 0; k < 3; k++) SET_W(D(2 + k), W(2 + k) - W(5 + k));
    SET_W(D(0), W(0) & W(4));
    A(0) = CLIP_INPUT + 4 + 18;
    A(3) = v + 12;
    behind = W(0) < 0;
    face_end(&snapshot, colour, drawn, behind);
    A(1) = a1;
    A(2) = a2;
    A(5) = a5;
    return glue_return();
}

int glue_C20E4E(void) { return parallelogram_glue(8, 8, 0, 8, 8, 0); }
int glue_C20E40(void) { return parallelogram_glue(2, 0, 2, 2, 0, 2); }

int glue_C21490(void) {
    gaddr stream = A(2), v;
    uint16_t colour = rd_u16(A(2));
    uint32_t a1 = A(1), a2, a5 = A(5);
    int16_t p[3][3];
    ClipperSnapshot snapshot;
    int drawn, k;

    clipper_snapshot(&snapshot);
    drawn = draw_parallelogram_face_near(&stream);
    v = WORKSPACES + SEXT(rd_u16(A(2) + 2));
    A(2) += 4;
    a2 = A(2);
    A(3) = v;
    if (rd_s32(PROJECTION_Y) < -0x80) {
        D(0) = 0;
        flags_logic_l(0);
        return glue_return();
    }
    read3(v, p[0]);
    read3(v + 6, p[1]);
    read3(v + 12, p[2]);
    A(3) = v + 12;
    for (k = 0; k < 3; k++) {
        D(2 + k) = SEXT(p[0][k]);
        D(5 + k) = SEXT(p[1][k]);
    }
    for (k = 0; k < 3; k++) SET_W(D(5 + k), W(5 + k) - W(2 + k));
    for (k = 0; k < 3; k++) D(2 + k) = SEXT(p[2][k]);
    for (k = 0; k < 3; k++) SET_W(D(2 + k), W(2 + k) - W(5 + k));
    A(0) = CLIP_INPUT + 4 + 18;
    SET_W(D(7), W(4) & rd_u16(A(0) + 10) & rd_u16(A(0) + 16) & rd_u16(A(0) + 22));
    face_end(&snapshot, colour, drawn, W(7) < 0);
    A(1) = a1;
    A(2) = a2;
    A(5) = a5;
    return glue_return();
}

int glue_C2139E(void) {
    gaddr stream = A(2);
    uint16_t colour = rd_u16(A(2));
    uint32_t a1 = A(1), a2, a5 = A(5);
    int16_t o[4], va[3], va1[3], vd[3];
    ClipperSnapshot snapshot;
    int drawn, k;

    clipper_snapshot(&snapshot);
    drawn = draw_offset_face(&stream);
    for (k = 0; k < 4; k++) o[k] = rd_s16(A(2) + 2 + (gaddr)(2 * k));
    A(2) += 10;
    a2 = A(2);
    A(3) = WORKSPACES;
    A(0) = CLIP_INPUT + 4 + 18;
    D(7) = SEXT((uint16_t)o[3]);
    SET_W(D(6), rd_u16(WORKSPACES + SEXT((uint16_t)o[1]) + 4) & rd_u16(WORKSPACES + SEXT((uint16_t)o[2]) + 4) &
                    rd_u16(WORKSPACES + SEXT((uint16_t)o[3]) + 4));
    read3(WORKSPACES + SEXT((uint16_t)o[0]), va);
    read3(WORKSPACES + SEXT((uint16_t)o[0]) + 6, va1);
    read3(WORKSPACES + SEXT((uint16_t)o[3]), vd);
    for (k = 0; k < 3; k++) {
        D(3 + k) = (SEXT(va1[k]) & 0xFFFF0000u) | (uint16_t)(va1[k] - va[k]);
        D(k) = (SEXT(vd[k]) & 0xFFFF0000u) | (uint16_t)(vd[k] - (int16_t)(va1[k] - va[k]));
    }
    SET_W(D(6), W(6) & W(2));
    if (W(6) < 0) {
        D(0) = 0;
        flags_logic_l(0);
        return glue_return();
    }
    clipper_registers(&snapshot, colour, drawn);
    A(1) = a1;
    A(2) = a2;
    A(5) = a5;
    return glue_return();
}

int glue_C21412(void) {
    gaddr stream = A(2);
    uint16_t colour = rd_u16(A(2));
    uint32_t a1 = A(1), a2, a5 = A(5);
    int16_t a, b, c, va[3], vb[3], vb1[3], vc[3], shift[3];
    ClipperSnapshot snapshot;
    int drawn, k;

    clipper_snapshot(&snapshot);
    drawn = draw_mixed_face(&stream);
    a = rd_s16(A(2) + 2);
    b = rd_s16(A(2) + 4);
    c = rd_s16(A(2) + 6);
    A(2) += 8;
    a2 = A(2);
    A(3) = WORKSPACES;
    A(0) = CLIP_INPUT + 4 + 22;
    read3(WORKSPACES + SEXT((uint16_t)a), va);
    read3(WORKSPACES + SEXT((uint16_t)b), vb);
    read3(WORKSPACES + SEXT((uint16_t)b) + 6, vb1);
    read3(WORKSPACES + SEXT((uint16_t)c), vc);
    for (k = 0; k < 3; k++) shift[k] = (int16_t)(vb1[k] - vb[k]);
    for (k = 0; k < 3; k++) {
        D(3 + k) = (SEXT(vb1[k]) & 0xFFFF0000u) | (uint16_t)shift[k];
        D(k) = (SEXT(vc[k]) & 0xFFFF0000u) | (uint16_t)(vc[k] - shift[k]);
    }
    D(7) = (D(7) & 0xFFFF0000u) | (uint16_t)c;
    SET_W(D(6), (uint16_t)(va[2] & (int16_t)(va[2] - shift[2]) & (int16_t)(vc[2] - shift[2]) & vc[2]));
    if (W(6) < 0) {
        D(0) = 0;
        flags_logic_l(0);
        return glue_return();
    }
    clipper_registers(&snapshot, colour, drawn);
    A(1) = a1;
    A(2) = a2;
    A(5) = a5;
    return glue_return();
}

/* ---- derived points ------------------------------------------------------ */

/* $C20F18 on the block at `v`: the registers the MOVEM.W / SWAP sequence
 * leaves (d = p0 - p1 in the high words of D0-D2 until the end, e = p2 -
 * p1 in D3-D5, the last point in D6/D7/A4). */
static void parallelograms_regs(gaddr v) {
    int16_t p0[3], p1[3], p2[3], p3[3], d[3], e[3];
    int k;
    read3(v, p0);
    read3(v + 6, p1);
    read3(v + 12, p2);
    read3(v + 18, p3);
    for (k = 0; k < 3; k++) {
        d[k] = (int16_t)(p0[k] - p1[k]);
        e[k] = (int16_t)(p2[k] - p1[k]);
    }
    D(0) = 0;
    D(1) = ((uint32_t)(uint16_t)p1[1] << 16) | (uint16_t)d[1];
    D(2) = ((uint32_t)(uint16_t)p1[2] << 16) | (uint16_t)d[2];
    for (k = 0; k < 3; k++) D(3 + k) = (SEXT(p2[k]) & 0xFFFF0000u) | (uint16_t)e[k];
    D(6) = (SEXT(p3[0]) & 0xFFFF0000u) | (uint16_t)(p3[0] + e[0] + d[0]);
    D(7) = (SEXT(p3[1]) & 0xFFFF0000u) | (uint16_t)(p3[1] + e[1] + d[1]);
    A(4) = SEXT(p3[2]) + SEXT(e[2]) + SEXT(d[2]);
    A(3) = v + 24;
    flags_logic_l(0);
}

int glue_C20F10(void) {
    gaddr stream = A(2), v = WORKSPACES + SEXT(rd_u16(A(2)));
    A(2) += 2;
    parallelograms_regs(v);
    extend_parallelograms(&stream);
    return glue_return();
}

int glue_C20EC4(void) {
    gaddr stream = A(2), v = WORKSPACES + SEXT(rd_u16(A(2)));
    A(2) += 4;
    parallelograms_regs(v);
    extend_parallelograms_scaled(&stream);
    return glue_return();
}
