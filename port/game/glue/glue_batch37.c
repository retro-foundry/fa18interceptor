/* Glue for the outer polygon clipper $C2469E ($C246A0 past its NOP). Every
 * register is compared after it, so its register flow is replayed on a
 * snapshot of the clip state taken before the C, once the C has run (the
 * projection reads the clipped list the C writes): stage 0 inline, the
 * closing edges, the projection, and what draw_polygon leaves.
 * The closing crossings and the projection keep their DIVS remainders in
 * the upper words, so the rounding here is on whole longs. */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "hardware.h"
#include "memory.h"
#include "polygon_clip.h"
#include "glue_clip.h"

#define SEXT(v) ((uint32_t)(int32_t)(int16_t)(v))
#define W(n) ((int16_t)D(n))

void draw_polygon_registers(uint16_t last_size, uint16_t colour); /* glue_batch35.c */

/* DIVS.W D2,Dn on the MULS product in Dn: on overflow Dn stays. */
static void divs_long(int n, int16_t divisor) {
    int32_t dividend = (int32_t)D(n), q = dividend / divisor;
    if (q != (int16_t)q) return;
    D(n) = ((uint32_t)(uint16_t)(dividend % divisor) << 16) | (uint16_t)q;
}

static uint32_t swap(uint32_t v) { return (v << 16) | (v >> 16); }

/* MULS D5,Dn / DIVS D2,Dn / SWAP / the rounding by D6 / ADD.W base. */
static void rounded_long(int n, int base) {
    D(n) = (uint32_t)((int32_t)W(n) * W(5));
    divs_long(n, W(2));
    D(n) = swap(D(n));
    if (W(n) < 0) SET_W(D(n), (uint16_t)-W(n));
    if (W(6) > W(n)) {
        D(n) = swap(D(n));
    } else {
        D(n) = swap(D(n));
        SET_W(D(n), (uint16_t)(W(n) < 0 ? W(n) - 1 : W(n) + 1));
    }
    SET_W(D(n), (uint16_t)(W(n) + W(base)));
}

static void half_divisor(void) {
    SET_W(D(6), (uint16_t)((W(2) < 0 ? (int16_t)-W(2) : W(2)) >> 1));
}

static void neg_word(int n) { SET_W(D(n), (uint16_t)-W(n)); }

/* The crossing on the tested axis `a` (prev in `pa`) with the other axis
 * `o` (prev in `po`), cur in D0-D2 and prev in D3-D5. 0 when degenerate. */
static int crossing_regs(int a, int pa, int o, int po, int negative) {
    SET_W(D(a), W(a) - W(pa));
    SET_W(D(2), (uint16_t)(-W(2) + W(5) + (negative ? -W(a) : W(a))));
    if (!W(2)) return 0;
    SET_W(D(5), (uint16_t)(negative ? W(5) + W(pa) : W(5) - W(pa)));
    half_divisor();
    rounded_long(a, pa);
    SET_W(D(o), W(o) - W(po));
    rounded_long(o, po);
    SET_W(D(2), negative ? (uint16_t)-W(a) : (uint16_t)W(a));
    return 1;
}

static void to_scratch(ClipCopy *c) {
    c->scratch[0] = W(0);
    c->scratch[1] = W(1);
    c->scratch[2] = W(2);
}

/* Stage 0 for one input vertex, already in D0-D2. */
static void stage0_regs(ClipCopy *c) {
    int i;
    if (!c->started[0]) {
        for (i = 0; i < 3; i++) c->state[0][3 + i] = W(i);
        c->started[0]++;
    } else {
        for (i = 0; i < 3; i++) D(3 + i) = SEXT(c->state[0][i]);
        if ((W(1) > W(2)) != (W(4) > W(5))) {
            uint32_t c0 = D(0), c1 = D(1), c2 = D(2);
            if (!crossing_regs(1, 4, 0, 3, 0)) return; /* skipped; D1, D2 stay spoilt */
            to_scratch(c);
            D(0) = SEXT(c0); D(1) = SEXT(c1); D(2) = SEXT(c2);
            for (i = 0; i < 3; i++) c->state[0][i] = W(i);
            clip_stage_registers(1, c);
            c->passed[0]++;
            goto test;
        }
    }
    for (i = 0; i < 3; i++) c->state[0][i] = W(i);
test:
    if (!(W(1) > W(2))) {
        to_scratch(c);
        clip_stage_registers(1, c);
        c->passed[0]++;
    }
    D(0) = 1;
}

/* The closing edges; 0 when one is degenerate. */
static int closings_regs(ClipCopy *c) {
    int k, i;
    for (k = 0; k < 4; k++) {
        int y_axis = k < 2, negative = k == 1 || k == 3;
        int a = y_axis ? 1 : 0, pa = a + 3, o = y_axis ? 0 : 1, po = o + 3;
        if (!c->passed[k]) continue;
        for (i = 0; i < 6; i++) D(i) = SEXT(c->state[k][i]);
        if (negative) { neg_word(a); neg_word(pa); }
        if ((W(a) > W(2)) == (W(pa) > W(5))) continue; /* left negated */
        if (negative) { neg_word(a); neg_word(pa); }
        if (!crossing_regs(a, pa, o, po, negative)) return 0;
        if (k < 3) {
            to_scratch(c);
            clip_stage_registers(k + 1, c);
        } else {
            A(1) += 6;
            SET_W(D(7), W(7) + 1);
        }
    }
    return 1;
}

/* The projection: 1 when every vertex is in front. */
static int project_regs(void) {
    gaddr src = CLIP_OUTPUT;
    A(1) = CLIP_OUTPUT;
    A(0) = POLY_VERTICES + 2;
    SET_W(D(0), 0x13F);
    SET_W(D(1), 0xB3);
    SET_W(D(7), W(7) - 1);
    for (;;) {
        int i;
        for (i = 0; i < 3; i++) D(3 + i) = SEXT(rd_s16(src + (gaddr)(2 * i)));
        src += 6;
        A(1) += 6;
        if (W(5) <= 0) return 0;
        D(3) = (uint32_t)((int32_t)W(3) * 0xA0);
        divs_long(3, W(5));
        SET_W(D(3), W(3) + 0xA0);
        if (W(3) < 0) SET_W(D(3), 0);
        else if (W(3) >= 0x140) SET_W(D(3), 0x13F);
        D(4) = (uint32_t)((int32_t)W(4) * 0x5A);
        divs_long(4, W(5));
        SET_W(D(4), W(4) + 0x5A);
        if (W(4) < 0) SET_W(D(4), 0);
        else if (W(4) >= 0xB4) SET_W(D(4), 0xB3);
        SET_W(D(2), W(0) - W(3));
        SET_W(D(5), W(1) - W(4));
        A(0) += 4;
        SET_W(D(7), W(7) - 1);
        if (W(7) == -1) return 1;
    }
}

/* The registers $C246A0 leaves up to the projection, on the copy: 1 when
 * it goes on to project. */
static int clipper_regs(ClipCopy *c) {
    int16_t shift = rd_s16(CLIP_INPUT), count = rd_s16(CLIP_INPUT + 2), n;
    gaddr src = CLIP_INPUT + 4;
    int i;

    SET_W(D(7), 0);
    SET_W(D(0), (uint16_t)count);
    A(0) = CLIP_INPUT + 4;
    if (count < 3) return 0;
    A(0) += (uint32_t)(6 * count);
    A(1) = CLIP_OUTPUT;
    A(2) = CLIP_STATES;
    A(3) = CLIP_SCRATCH;
    A(4) = CLIP_FLAGS;
    for (i = 0; i < 4; i++) c->started[i] = c->passed[i] = 0;
    D(0) = 1;
    for (n = 0; n < count; n++, src += 6) {
        int s;
        for (i = 0; i < 3; i++) D(i) = SEXT(rd_s16(src + (gaddr)(2 * i)));
        SET_W(D(3), (uint16_t)shift);
        s = shift & 63;
        for (i = 0; i < 3; i++) D(i) = s >= 32 ? 0 : D(i) << s;
        stage0_regs(c);
    }
    if (!closings_regs(c)) {
        SET_W(D(7), 0);
        return 0;
    }
    if (W(7) == 0) return 0;
    if (W(7) <= 2) {
        SET_W(D(7), 0);
        return 0;
    }
    return 1;
}

void clipper_snapshot(ClipperSnapshot *s) {
    s->last_size = custom_written(BLTSIZE);
    load_clip_copy(&s->copy);
}

void clipper_registers(ClipperSnapshot *s, uint16_t colour, int drawn) {
    int projects = clipper_regs(&s->copy);
    if (projects && !project_regs()) SET_W(D(7), 0);
    if (drawn) draw_polygon_registers(s->last_size, colour);
    D(0) = (uint32_t)drawn;
    flags_logic_l(D(0));
}

int glue_C246A0(void) {
    uint16_t colour = rd_u16(CURRENT_COLOUR);
    ClipperSnapshot snapshot;

    clipper_snapshot(&snapshot);
    clipper_registers(&snapshot, colour, clip_and_draw_polygon());
    return glue_return();
}

int glue_C2469E(void) { return glue_C246A0(); }
