#include "polygon_clip.h"

#include "fault.h"
#include "globals.h"

/* Stage layout: stage k's previous vertex at CLIP_STATES + $10k, its first
 * at + 6; flags: started[k] at CLIP_FLAGS + k, passed[k] at + 4 + k. */
static gaddr previous_of(int stage) { return CLIP_STATES + (gaddr)(0x10 * stage); }
static gaddr first_of(int stage) { return CLIP_STATES + (gaddr)(0x10 * stage + 6); }

static ClipPoint get(gaddr a) {
    ClipPoint p;
    p.x = rd_s16(a);
    p.y = rd_s16(a + 2);
    p.z = rd_s16(a + 4);
    return p;
}

static void put(gaddr a, ClipPoint p) {
    wr_s16(a, p.x);
    wr_s16(a + 2, p.y);
    wr_s16(a + 4, p.z);
}

/* The coordinate the stage's plane tests, signed so that "inside" is
 * value <= z. */
static int16_t tested(int stage, ClipPoint p) {
    switch (stage) {
    case CLIP_Y_POS: return p.y;
    case CLIP_Y_NEG: return (int16_t)-p.y;
    case CLIP_X_POS: return p.x;
    default: return (int16_t)-p.x;
    }
}

static int inside(int stage, ClipPoint p) {
    return tested(stage, p) <= p.z;
}

/* 68000 DIVS.W: on overflow the dividend's words stand in. */
static void divs_w(int32_t dividend, int16_t divisor, int16_t *quotient, int16_t *remainder) {
    int32_t q = dividend / divisor;
    if (q != (int16_t)q) {
        *quotient = (int16_t)dividend;
        *remainder = (int16_t)((uint32_t)dividend >> 16);
        return;
    }
    *quotient = (int16_t)q;
    *remainder = (int16_t)(dividend % divisor);
}

static int16_t rounded(int32_t dividend, int16_t divisor, int16_t half) {
    int16_t q, r;
    divs_w(dividend, divisor, &q, &r);
    if (r < 0) r = (int16_t)-r;
    if (half > r) return q;
    return (int16_t)(q < 0 ? q - 1 : q + 1);
}

ClipPoint clip_crossing(int stage, ClipPoint prev, ClipPoint cur) {
    int y_axis = stage == CLIP_Y_POS || stage == CLIP_Y_NEG;
    int negative = stage == CLIP_Y_NEG || stage == CLIP_X_NEG;
    int16_t axis_cur = y_axis ? cur.y : cur.x, axis_prev = y_axis ? prev.y : prev.x;
    int16_t other_cur = y_axis ? cur.x : cur.y, other_prev = y_axis ? prev.x : prev.y;
    int16_t delta = (int16_t)(axis_cur - axis_prev);
    int16_t denominator = (int16_t)((int16_t)(prev.z - cur.z) + (negative ? (int16_t)-delta : delta));
    int16_t factor = (int16_t)(negative ? prev.z + axis_prev : prev.z - axis_prev);
    int16_t half, axis, other;
    ClipPoint p;

    if (denominator == 0) fatal_error((uint16_t)(stage + 2)); /* the original crashes */
    half = (int16_t)((denominator < 0 ? (int16_t)-denominator : denominator) >> 1);
    axis = (int16_t)(rounded((int32_t)delta * factor, denominator, half) + axis_prev);
    other = (int16_t)(rounded((int32_t)(int16_t)(other_cur - other_prev) * factor, denominator, half) + other_prev);
    if (y_axis) { p.y = axis; p.x = other; }
    else { p.x = axis; p.y = other; }
    p.z = negative ? (int16_t)-axis : axis;
    return p;
}

/* Pass a vertex on: into the next stage through the scratch point, or, from
 * the last stage, onto the output list. */
static void pass(int stage, ClipPoint p, ClipOutput *out, int through_scratch) {
    if (through_scratch) put(CLIP_SCRATCH, p);
    if (stage < CLIP_X_NEG) {
        clip_stage(stage + 1, p, out);
    } else {
        put(out->next, p);
        out->next += 6;
        out->count++;
    }
    wr_u8(CLIP_FLAGS + 4 + (gaddr)stage, (uint8_t)(rd_u8(CLIP_FLAGS + 4 + (gaddr)stage) + 1));
}

void clip_stage(int stage, ClipPoint cur, ClipOutput *out) {
    if (!rd_u8(CLIP_FLAGS + (gaddr)stage)) {
        put(first_of(stage), cur);
        wr_u8(CLIP_FLAGS + (gaddr)stage, (uint8_t)(rd_u8(CLIP_FLAGS + (gaddr)stage) + 1));
    } else {
        ClipPoint prev = get(previous_of(stage));
        if (inside(stage, prev) != inside(stage, cur)) {
            ClipPoint crossing = clip_crossing(stage, prev, cur);
            put(CLIP_SCRATCH, crossing);
            put(previous_of(stage), cur);
            pass(stage, crossing, out, 0);
            goto test;
        }
    }
    put(previous_of(stage), cur);
test:
    /* The last stage writes an inside vertex straight to the list. */
    if (inside(stage, cur)) pass(stage, cur, out, stage < CLIP_X_NEG);
}
