/* Glue for the clip stages $C247C0, $C248B2, $C24996. Every register and
 * flag is live after them, so their register flow is replayed here on a
 * private copy of the clip state (the C updates the real one): D0-D2 come
 * back sign-extended from the entry MOVEM.W, D3-D6 are what the last stage
 * to run left, D7 counts output vertices and A1 walks the output list. */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "memory.h"
#include "polygon_clip.h"
#include "glue_clip.h"

#define SEXT(v) ((uint32_t)(int32_t)(int16_t)(v))
#define W(n) ((int16_t)D(n))

void load_clip_copy(ClipCopy *c) {
    int k, i;
    for (k = 0; k < 4; k++) {
        for (i = 0; i < 6; i++) c->state[k][i] = rd_s16(CLIP_STATES + (gaddr)(0x10 * k + 2 * i));
        c->started[k] = rd_u8(CLIP_FLAGS + (gaddr)k);
        c->passed[k] = rd_u8(CLIP_FLAGS + 4 + (gaddr)k);
    }
    for (i = 0; i < 3; i++) c->scratch[i] = rd_s16(CLIP_SCRATCH + (gaddr)(2 * i));
}

static void neg_word(int n) { SET_W(D(n), (uint16_t)-W(n)); }

/* The rounded quotient the stages compute into register `n` (MULS, DIVS,
 * SWAP, rounding by D6), then ADD.W of the base register. */
static void rounded_into(int n, int16_t factor_reg_value, int base) {
    int32_t product = (int32_t)W(n) * factor_reg_value;
    int16_t divisor = W(2), q, r;
    int32_t full = product / divisor;
    if (full != (int16_t)full) { q = (int16_t)product; r = (int16_t)((uint32_t)product >> 16); }
    else { q = (int16_t)full; r = (int16_t)(product % divisor); }
    if (r < 0) r = (int16_t)-r;
    if (!(W(6) > r)) q = (int16_t)(q < 0 ? q - 1 : q + 1);
    SET_W(D(n), (uint16_t)(q + W(base)));
}


/* Pass the vertex in D0-D2 on (through the scratch point when asked). */
static void pass_regs(int k, ClipCopy *c, int through_scratch) {
    if (through_scratch) { c->scratch[0] = W(0); c->scratch[1] = W(1); c->scratch[2] = W(2); }
    if (k < 3) {
        clip_stage_registers(k + 1, c);
    } else {
        A(1) += 6;
        SET_W(D(7), W(7) + 1);
    }
    c->passed[k]++;
}

void clip_stage_registers(int k, ClipCopy *c) {
    uint32_t e0 = D(0), e1 = D(1), e2 = D(2);
    int y_axis = k < 2, negative = k == 1 || k == 3;
    int a = y_axis ? 1 : 0, pa = y_axis ? 4 : 3; /* the tested registers: cur, prev */
    int i, crossed = 0;

    for (i = 0; i < 3; i++) D(i) = SEXT(k ? c->scratch[i] : W(i));
    if (!c->started[k]) {
        for (i = 0; i < 3; i++) c->state[k][3 + i] = W(i);
        c->started[k]++;
    } else {
        int cur_out, prev_out;
        for (i = 0; i < 3; i++) D(3 + i) = SEXT(c->state[k][i]);
        if (k == 2) SET_W(D(6), D(2)); /* $C248D4: MOVE.W D2,D6 */
        if (negative) { neg_word(a); neg_word(pa); }
        cur_out = W(a) > W(2);
        prev_out = W(pa) > W(5);
        if (cur_out != prev_out) {
            uint32_t c0, c1, c2;
            int16_t factor;
            if (negative) { neg_word(a); neg_word(pa); }
            c0 = D(0); c1 = D(1); c2 = D(2);
            SET_W(D(a), W(a) - W(pa));                         /* delta */
            SET_W(D(2), (uint16_t)(-W(2) + W(5) + (negative ? -W(a) : W(a))));
            SET_W(D(5), (uint16_t)(negative ? W(5) + W(pa) : W(5) - W(pa)));
            factor = W(5);
            SET_W(D(6), (uint16_t)((W(2) < 0 ? (int16_t)-W(2) : W(2)) >> 1));
            rounded_into(a, factor, pa);
            {
                int o = y_axis ? 0 : 1, po = y_axis ? 3 : 4;
                SET_W(D(o), W(o) - W(po));
                rounded_into(o, factor, po);
            }
            SET_W(D(2), negative ? (uint16_t)-W(a) : (uint16_t)W(a));
            c->scratch[0] = W(0); c->scratch[1] = W(1); c->scratch[2] = W(2);
            D(0) = SEXT(c0); D(1) = SEXT(c1); D(2) = SEXT(c2);
            for (i = 0; i < 3; i++) c->state[k][i] = W(i);
            {
                /* The crossing is in the scratch point; pass it on. */
                uint32_t k0 = D(0), k1 = D(1), k2 = D(2);
                D(0) = SEXT(c->scratch[0]); D(1) = SEXT(c->scratch[1]); D(2) = SEXT(c->scratch[2]);
                pass_regs(k, c, 0);
                D(0) = SEXT(k0); D(1) = SEXT(k1); D(2) = SEXT(k2);
            }
            crossed = 1;
        } else if (negative) {
            neg_word(a); /* the current vertex's axis back */
        }
    }
    if (!crossed) for (i = 0; i < 3; i++) c->state[k][i] = W(i);
    /* Inside test (negating the current axis for the negative planes). */
    if (negative) neg_word(a);
    if (!(W(a) > W(2))) {
        if (negative) neg_word(a);
        pass_regs(k, c, k < 3);
    } else if (negative) {
        /* left negated; D0-D2 are restored below anyway */
    }
    D(0) = SEXT(e0);
    D(1) = SEXT(e1);
    D(2) = SEXT(e2);
}

static int stage_glue(int k) {
    ClipCopy copy;
    ClipOutput out = {0};
    ClipPoint p;

    load_clip_copy(&copy);
    p.x = rd_s16(CLIP_SCRATCH);
    p.y = rd_s16(CLIP_SCRATCH + 2);
    p.z = rd_s16(CLIP_SCRATCH + 4);
    out.next = A(1);
    out.count = (uint16_t)D(7);
    clip_stage_registers(k, &copy);
    clip_stage(k, p, &out);
    flags_logic_l(1); /* MOVEQ #1 before the final MOVEM.W */
    return glue_return();
}

int glue_C247C0(void) { return stage_glue(CLIP_Y_NEG); }
int glue_C248B2(void) { return stage_glue(CLIP_X_POS); }
int glue_C24996(void) { return stage_glue(CLIP_X_NEG); }
