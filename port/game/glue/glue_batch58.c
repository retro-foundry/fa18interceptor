/* Glue for the face grids $C20C38/$C20C22 and lattices $C20A52/$C20A40.
 * The grids replay their faces after C. The lattices replay each clipper
 * immediately after C draws that face, while its clip output and previous
 * BLTSIZE are still available. D7's high word carries between faces. */
#include "glue.h"
#include "ports_glue.h"

#include "draw_stream.h"
#include "globals.h"
#include "machine.h"
#include "memory.h"
#include "glue_clip.h"

#define SEXT(v) ((uint32_t)(int32_t)(int16_t)(v))
#define W(n) ((int16_t)D(n))

typedef struct { int16_t x[3]; } V3;

static V3 at(gaddr a) {
    V3 v;
    int k;
    for (k = 0; k < 3; k++) v.x[k] = rd_s16(a + (gaddr)(2 * k));
    return v;
}

static V3 sub(V3 a, V3 b) {
    V3 v;
    int k;
    for (k = 0; k < 3; k++) v.x[k] = (int16_t)(a.x[k] - b.x[k]);
    return v;
}

static V3 add(V3 a, V3 b) {
    V3 v;
    int k;
    for (k = 0; k < 3; k++) v.x[k] = (int16_t)(a.x[k] + b.x[k]);
    return v;
}

static V3 halved(V3 a) {
    V3 v;
    int k;
    for (k = 0; k < 3; k++) v.x[k] = (int16_t)(a.x[k] >> 1);
    return v;
}

static V3 times(V3 a, int16_t n) {
    V3 v;
    int k;
    for (k = 0; k < 3; k++) v.x[k] = (int16_t)(a.x[k] * n);
    return v;
}

static void frame3(int off, V3 v) {
    int k;
    for (k = 0; k < 3; k++) wr_s16(A(6) + (gaddr)(int32_t)(off + 2 * k), v.x[k]);
}

/* Registers first..first+2 = `low` over the high words of `high`. */
static void regs3(int first, V3 high, V3 low) {
    int k;
    for (k = 0; k < 3; k++) D(first + k) = (SEXT((uint16_t)high.x[k]) & 0xFFFF0000u) | (uint16_t)low.x[k];
}

/* One face's call to the clipper. The snapshot is taken before the C runs
 * and carried from face to face, so each call replays from the clip state
 * the one before it left, as the original's did. */
static void face_call(ClipperSnapshot *s, uint16_t colour) {
    s->last_size = fa18_bltsize_at_draw_start;
    clipper_registers(s, colour, -1);
}

/* One face's corners into the clipper input, as the C wrote them. */
static void write_face(V3 p0, V3 p1, V3 p2, V3 p3) {
    const V3 *corner[4];
    int i, k;
    corner[0] = &p0; corner[1] = &p1; corner[2] = &p2; corner[3] = &p3;
    wr_u32(CLIP_INPUT, 4);
    for (i = 0; i < 4; i++)
        for (k = 0; k < 3; k++) wr_s16(CLIP_INPUT + 4 + (gaddr)(6 * i + 2 * k), corner[i]->x[k]);
}

static int16_t counted_down(int16_t n) {
    do n--; while (n > 0);
    return n;
}

static int grid_glue(int plain) {
    gaddr stream = A(2), start = A(2), s = A(2), base, q;
    uint32_t a1 = A(1), a5 = A(5);
    uint16_t colour, result;
    int16_t rows, columns = 0, r;
    V3 u, v, w, acc, along, a, qv;
    ClipperSnapshot snap;

    clipper_snapshot(&snap);
    result = (uint16_t)(plain ? draw_face_grid_plain(&stream) : draw_face_grid(&stream));
    colour = rd_u16(CURRENT_COLOUR);
    if (plain) s += 2;
    base = WORKSPACES + SEXT(rd_u16(s));
    rows = rd_s16(s + 2);
    s += 4;
    q = base + 18;
    u = sub(at(base), at(q));
    v = sub(at(base + 6), at(q));
    w = sub(at(base + 12), at(q));
    for (r = 0; r < (rows > 1 ? rows : 1); r++) {
        columns = rd_s16(s);
        s += 2;
    }
    acc = times(v, (int16_t)(columns > 1 ? columns - 1 : 0));
    frame3(-0x46, u);
    frame3(-0x40, v);
    frame3(-0x52, w);
    frame3(-0x58, acc);
    wr_s16(A(6) - 0x38, counted_down(rows));
    wr_s16(A(6) - 0x3A, counted_down(columns));
    wr_u16(A(6) - 0x7E, result);
    /* Each face's setup: D4-D6 = q + w + a over q, D1-D3 = that + u over the
     * half steps; A0 past the first two corners. */
    s = start + (plain ? 6 : 4);
    q = base + 18;
    for (r = rows;;) {
        int16_t left = rd_s16(s);
        s += 2;
        along.x[0] = along.x[1] = along.x[2] = 0;
        for (;;) {
            a = halved(along);
            qv = at(q);
            {
                V3 far = add(add(qv, w), a), near = add(qv, a);
                write_face(near, add(near, u), add(far, u), far);
                regs3(4, qv, far);
                regs3(1, along, add(far, u));
            }
            A(0) = CLIP_INPUT + 16;
            A(3) = q;
            A(2) = s;
            face_call(&snap, colour);
            if (--left <= 0) break;
            along = add(along, v);
        }
        if (--r <= 0) break;
        q += 6;
    }
    A(2) = s;
    A(3) = q;
    A(1) = a1;
    A(5) = a5;
    SET_W(D(0), result);
    flags_logic_w(D(0));
    return glue_return();
}

int glue_C20C38(void) { return grid_glue(0); }
int glue_C20C22(void) { return grid_glue(1); }

typedef struct {
    ClipperSnapshot snapshot;
    gaddr p2a, stream_end;
    V3 far2;
    uint16_t colour;
    uint32_t saved_a3, saved_a4;
} LatticeGlue;

static void lattice_before(int back, const int16_t along_words[3], void *context) {
    LatticeGlue *g = (LatticeGlue *)context;
    V3 along, far = at(CLIP_INPUT + 22), corner = at(CLIP_INPUT + 16);
    int k;
    for (k = 0; k < 3; ++k) along.x[k] = along_words[k];
    regs3(4, back ? g->far2 : at(g->p2a + 6), far);
    regs3(1, along, corner);
    A(0) = CLIP_INPUT + 16;
    A(2) = g->stream_end;
    if (!back) {
        A(3) = g->p2a;
        A(4) = g->p2a + 6;
    }
    g->saved_a3 = A(3);
    g->saved_a4 = A(4);
    clipper_snapshot(&g->snapshot);
}

static void lattice_after(int back, int drawn, void *context) {
    LatticeGlue *g = (LatticeGlue *)context;
    clipper_registers(&g->snapshot, g->colour, drawn);
    if (!back) {
        A(3) = g->saved_a3;
        A(4) = g->saved_a4;
    }
}

static int lattice_glue(int plain) {
    gaddr stream = A(2), s = A(2), base;
    uint32_t a1 = A(1), a5 = A(5);
    uint16_t result;
    int16_t count;
    V3 u, v, h, far1, acc;
    LatticeGlue g;
    LatticeFaceHooks hooks = {lattice_before, lattice_after, &g};

    if (plain) s += 2;
    base = WORKSPACES + SEXT(rd_u16(s));
    count = rd_s16(s + 2);
    s += 4;
    g.stream_end = s;
    g.p2a = base + 12;
    g.colour = plain ? rd_u16(A(2)) : 0x0D;
    u = sub(at(base), at(g.p2a));
    v = sub(at(base + 6), at(g.p2a));
    h = halved(v);
    far1 = add(sub(at(g.p2a), h), u);
    g.far2 = add(sub(at(g.p2a + 6), h), u);
    acc = times(v, (int16_t)(count > 1 ? count - 1 : 0));
    frame3(-0x46, u);
    frame3(-0x40, v);
    frame3(-0x4C, far1);
    frame3(-0x52, g.far2);
    frame3(-0x58, acc);
    result = (uint16_t)(plain ? draw_face_lattice_plain_with_hooks(&stream, &hooks)
                             : draw_face_lattice_with_hooks(&stream, &hooks));
    wr_s16(A(6) - 0x38, counted_down(count));
    wr_s16(A(6) - 0x3A, counted_down(count));
    wr_u16(A(6) - 0x7E, result);
    A(1) = a1;
    A(2) = g.stream_end;
    A(5) = a5;
    SET_W(D(0), result);
    flags_logic_w(D(0));
    return glue_return();
}

int glue_C20A52(void) { return lattice_glue(0); }
int glue_C20A40(void) { return lattice_glue(1); }
