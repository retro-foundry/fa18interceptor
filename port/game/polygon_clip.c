#include "polygon_clip.h"

#include "fault.h"
#include "globals.h"
#include "render_polygon.h"

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

/* Stage 0's closing edge and the later stages', in order: the last vertex
 * to the first, passed on without being counted. 0 on a degenerate edge. */
static int close_stages(ClipOutput *out) {
    int stage;
    for (stage = CLIP_Y_POS; stage <= CLIP_X_NEG; stage++) {
        ClipPoint last, first, crossing;
        if (!rd_u8(CLIP_FLAGS + 4 + (gaddr)stage)) continue;
        last = get(previous_of(stage));
        first = get(first_of(stage));
        if (inside(stage, last) == inside(stage, first)) continue;
        {
            /* A degenerate closing edge drops the polygon (error 6 + stage). */
            int y_axis = stage < CLIP_X_POS, negative = stage == CLIP_Y_NEG || stage == CLIP_X_NEG;
            int16_t delta = (int16_t)((y_axis ? last.y : last.x) - (y_axis ? first.y : first.x));
            if ((int16_t)((int16_t)(first.z - last.z) + (negative ? (int16_t)-delta : delta)) == 0) {
                wr_u16(ERROR_CODE, (uint16_t)(6 + stage));
                fault_hook();
                return 0;
            }
        }
        crossing = clip_crossing(stage, first, last);
        if (stage < CLIP_X_NEG) {
            put(CLIP_SCRATCH, crossing);
            clip_stage(stage + 1, crossing, out);
        } else {
            put(out->next, crossing);
            out->next += 6;
            out->count++;
        }
    }
    return 1;
}

/* Project the clipped vertices into POLY_VERTICES; 0 at z <= 0. */
static int project(const ClipOutput *out) {
    gaddr src = CLIP_OUTPUT, dst = POLY_VERTICES;
    int i;
    wr_u16(dst, out->count);
    dst += 2;
    for (i = 0; i < out->count; i++, src += 6) {
        ClipPoint p = get(src);
        int16_t x, y, q, r;
        if (p.z <= 0) return 0;
        divs_w((int32_t)p.x * 0xA0, p.z, &q, &r);
        x = (int16_t)(q + 0xA0);
        if (x < 0) x = 0; else if (x >= 0x140) x = 0x13F;
        divs_w((int32_t)p.y * 0x5A, p.z, &q, &r);
        y = (int16_t)(q + 0x5A);
        if (y < 0) y = 0; else if (y >= 0xB4) y = 0xB3;
        wr_s16(dst, (int16_t)(0x13F - x));
        wr_s16(dst + 2, (int16_t)(0xB3 - y));
        dst += 4;
    }
    return 1;
}

int clip_and_draw_polygon(void) {
    int16_t shift = rd_s16(CLIP_INPUT), count = rd_s16(CLIP_INPUT + 2), i;
    gaddr src = CLIP_INPUT + 4;
    ClipOutput out;

    if (count < 3) {
        wr_u16(ERROR_CODE, 0x1F);
        fault_hook();
        return 0;
    }
    out.next = CLIP_OUTPUT;
    out.count = 0;
    for (i = 0; i < 8; i++) wr_u8(CLIP_FLAGS + (gaddr)i, 0);

    /* Stage 0 inline: like clip_stage, but a degenerate crossing only
     * records error 1 and skips the vertex. */
    for (i = 0; i < count; i++, src += 6) {
        ClipPoint cur;
        int s = shift & 63;
        cur.x = (int16_t)(s >= 32 ? 0 : (uint32_t)(int32_t)rd_s16(src) << s);
        cur.y = (int16_t)(s >= 32 ? 0 : (uint32_t)(int32_t)rd_s16(src + 2) << s);
        cur.z = (int16_t)(s >= 32 ? 0 : (uint32_t)(int32_t)rd_s16(src + 4) << s);
        if (rd_u8(CLIP_FLAGS)) {
            ClipPoint prev = get(previous_of(CLIP_Y_POS));
            if (inside(CLIP_Y_POS, prev) != inside(CLIP_Y_POS, cur)) {
                int16_t delta = (int16_t)(cur.y - prev.y);
                if ((int16_t)((int16_t)(prev.z - cur.z) + delta) == 0) {
                    wr_u16(ERROR_CODE, 1);
                    fault_hook();
                    continue;
                }
            }
        }
        clip_stage(CLIP_Y_POS, cur, &out);
    }
    if (!close_stages(&out)) {
        wr_u16(CLIP_ERRORS, (uint16_t)(rd_u16(CLIP_ERRORS) + 1));
        wr_u16(ERROR_CODE, 0x0A);
        fault_hook();
        return 0;
    }
    if (out.count <= 2) return 0;
    if (!project(&out)) return 0;
    draw_polygon();
    wr_u16(LIST_COUNT, (uint16_t)(rd_u16(LIST_COUNT) + 1));
    return 1;
}
