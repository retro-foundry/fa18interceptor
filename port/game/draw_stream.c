/* Commands of an object's draw stream. */
#include "draw_stream.h"

#include "globals.h"
#include "polygon_clip.h"
#include "render_line.h"

typedef struct { int16_t x, y, z; } Vertex;

static int16_t next_word(gaddr *stream) {
    int16_t w = rd_s16(*stream);
    *stream += 2;
    return w;
}

static gaddr vertex_at(int16_t offset) { return WORKSPACES + (gaddr)(int32_t)offset; }

static Vertex get(gaddr a) {
    Vertex v;
    v.x = rd_s16(a);
    v.y = rd_s16(a + 2);
    v.z = rd_s16(a + 4);
    return v;
}

static void put(gaddr a, Vertex v) {
    wr_s16(a, v.x);
    wr_s16(a + 2, v.y);
    wr_s16(a + 4, v.z);
}

static Vertex minus(Vertex a, Vertex b) {
    Vertex v;
    v.x = (int16_t)(a.x - b.x);
    v.y = (int16_t)(a.y - b.y);
    v.z = (int16_t)(a.z - b.z);
    return v;
}

static Vertex plus(Vertex a, Vertex b) {
    Vertex v;
    v.x = (int16_t)(a.x + b.x);
    v.y = (int16_t)(a.y + b.y);
    v.z = (int16_t)(a.z + b.z);
    return v;
}

/* The next vertex's offset from `a`, less `a`: the edge from a vertex to
 * the one after it. */
static Vertex edge_after(gaddr a) { return minus(get(a + 6), get(a)); }

/* ---- segments ------------------------------------------------------------ */

int draw_segment_pairs(gaddr *stream) {
    int drawn = 0, last = 0;
    wr_u32(LINE_STYLE, 0xFFFFFFFFu);
    wr_u16(CURRENT_COLOUR, (uint16_t)next_word(stream));
    while (!last) {
        int16_t a = next_word(stream), b = next_word(stream);
        gaddr va, vb;
        if (b < 0) {
            last = 1;
            b &= 0x7FFF;
        }
        va = vertex_at(a);
        vb = vertex_at(b);
        put(SEGMENT_POINTS, get(va));
        wr_u32(SEGMENT_POINTS + 6, rd_u32(vb));
        if ((int16_t)(rd_u16(va + 4) & rd_u16(vb + 4)) < 0) continue; /* both behind */
        wr_u16(SEGMENT_POINTS + 10, rd_u16(vb + 4));
        drawn |= draw_clipped_segment();
    }
    return drawn;
}

int draw_segment_pairs_near(gaddr *stream) {
    if (rd_s32(PROJECTION_Y) < -0xC0) return draw_segment_pairs(stream);
    while (next_word(stream) >= 0) {}
    return 0;
}

int draw_segment_run(gaddr *stream) {
    uint16_t head = (uint16_t)next_word(stream);
    int16_t count = (int16_t)(head >> 8);
    gaddr v = vertex_at(next_word(stream));
    int drawn = 0;
    wr_u16(CURRENT_COLOUR, head & 0x3F);
    do {
        put(SEGMENT_POINTS, get(v));
        put(SEGMENT_POINTS + 6, get(v + 6));
        v += 12;
        drawn |= draw_clipped_segment();
    } while (--count > 0);
    return drawn;
}

int draw_offset_segments(gaddr *stream) {
    int16_t base;
    int drawn = 0, last = 0;
    Vertex shift;
    wr_u16(CURRENT_COLOUR, (uint16_t)next_word(stream));
    base = next_word(stream);
    shift = edge_after(vertex_at(base));
    while (!last) {
        int16_t a = next_word(stream), b;
        if (rd_s16(*stream) < 0) last = 1;
        b = (int16_t)(next_word(stream) & 0x7FFF);
        put(SEGMENT_POINTS, minus(get(vertex_at(a)), shift));
        put(SEGMENT_POINTS + 6, minus(get(vertex_at(b)), shift));
        drawn |= draw_clipped_segment();
    }
    return drawn;
}

/* ---- faces --------------------------------------------------------------- */

/* The clipper input header for four corners; returns where they go. */
static gaddr four_corners(void) {
    wr_u16(CLIP_INPUT, 0);
    wr_u16(CLIP_INPUT + 2, 4);
    return CLIP_INPUT + 4;
}

static int parallelogram(gaddr *stream, uint16_t planes, uint16_t colour, uint16_t complement) {
    gaddr v, in;
    Vertex p0, p1, p2, p3;
    wr_u16(LINE_STYLE, planes);
    wr_u16(LINE_STYLE + 2, colour);
    wr_u16(POLY_COMPLEMENT, complement);
    wr_u16(POLY_MASK_BLIT, 0);
    wr_u16(CURRENT_COLOUR, (uint16_t)next_word(stream));
    v = vertex_at(next_word(stream));
    in = four_corners();
    p0 = get(v);
    p1 = get(v + 6);
    p2 = get(v + 12);
    p3 = minus(p2, minus(p1, p0));
    put(in, p0);
    put(in + 6, p1);
    put(in + 12, p2);
    if ((int16_t)(p0.z & p1.z & p2.z & p3.z) < 0) return 0;
    put(in + 18, p3);
    return clip_and_draw_polygon();
}

int draw_parallelogram_face(gaddr *stream) { return parallelogram(stream, 8, 8, 0); }
int draw_parallelogram_face_2(gaddr *stream) { return parallelogram(stream, 2, 0, 2); }

int draw_parallelogram_face_near(gaddr *stream) {
    gaddr v, in;
    Vertex p0, p1, p2, p3;
    wr_u16(CURRENT_COLOUR, (uint16_t)next_word(stream));
    v = vertex_at(next_word(stream));
    if (rd_s32(PROJECTION_Y) < -0x80) return 0;
    in = four_corners();
    p0 = get(v);
    p1 = get(v + 6);
    p2 = get(v + 12);
    p3 = minus(p2, minus(p1, p0));
    put(in, p0);
    put(in + 6, p1);
    put(in + 12, p2);
    put(in + 18, p3);
    /* The original's offsets run past the corners (from the fourth). */
    if ((int16_t)(p3.z & rd_u16(in + 28) & rd_u16(in + 34) & rd_u16(in + 40)) < 0) return 0;
    return clip_and_draw_polygon();
}

int draw_offset_face(gaddr *stream) {
    int16_t a, b, c, d;
    gaddr in;
    Vertex vb, vc, vd, p3;
    wr_u16(CURRENT_COLOUR, (uint16_t)next_word(stream));
    a = next_word(stream);
    b = next_word(stream);
    c = next_word(stream);
    d = next_word(stream);
    in = four_corners();
    vb = get(vertex_at(b));
    vc = get(vertex_at(c));
    vd = get(vertex_at(d));
    p3 = minus(vd, edge_after(vertex_at(a)));
    put(in, vb);
    put(in + 6, vc);
    put(in + 12, vd);
    put(in + 18, p3);
    if ((int16_t)(vb.z & vc.z & vd.z & p3.z) < 0) return 0;
    return clip_and_draw_polygon();
}

int draw_mixed_face(gaddr *stream) {
    int16_t a, b, c;
    gaddr in;
    Vertex va, vc, shift, p1, p2;
    wr_u16(CURRENT_COLOUR, (uint16_t)next_word(stream));
    a = next_word(stream);
    in = four_corners();
    va = get(vertex_at(a));
    b = next_word(stream);
    shift = edge_after(vertex_at(b));
    p1 = minus(va, shift);
    c = next_word(stream);
    vc = get(vertex_at(c));
    p2 = minus(vc, shift);
    put(in, va);
    put(in + 6, p1);
    put(in + 12, p2);
    put(in + 18, vc);
    if ((int16_t)(va.z & p1.z & p2.z & vc.z) < 0) return 0;
    return clip_and_draw_polygon();
}

/* ---- derived points ------------------------------------------------------ */

static void parallelograms_at(gaddr v) {
    Vertex p0 = get(v), p1 = get(v + 6), p2 = get(v + 12), p3 = get(v + 18);
    Vertex d = minus(p0, p1), e = minus(p2, p1);
    put(v + 24, plus(p2, d));
    put(v + 30, plus(p3, d));
    put(v + 36, plus(p3, e));
    put(v + 42, plus(plus(p3, e), d));
}

void extend_parallelograms(gaddr *stream) { parallelograms_at(vertex_at(next_word(stream))); }

void extend_parallelograms_scaled(gaddr *stream) {
    gaddr v = vertex_at(next_word(stream));
    int16_t shift = next_word(stream);
    Vertex q1 = get(v + 6), q2 = get(v + 12), q3 = get(v + 18);
    Vertex a = minus(q1, q2), b = minus(q2, q3), p8;
    int s = shift < 0 ? (int16_t)-shift : shift;
    s &= 63;
    if (shift >= 0) {
        b.x = (int16_t)(s >= 16 ? 0 : (uint16_t)b.x << s);
        b.y = (int16_t)(s >= 16 ? 0 : (uint16_t)b.y << s);
        b.z = (int16_t)(s >= 16 ? 0 : (uint16_t)b.z << s);
    } else {
        b.x = (int16_t)(s >= 16 ? (b.x < 0 ? -1 : 0) : b.x >> s);
        b.y = (int16_t)(s >= 16 ? (b.y < 0 ? -1 : 0) : b.y >> s);
        b.z = (int16_t)(s >= 16 ? (b.z < 0 ? -1 : 0) : b.z >> s);
    }
    p8 = minus(q3, b);
    put(v + 48, p8);
    put(v + 54, minus(p8, a));
    parallelograms_at(v);
}

/* ---- grids of segments ---------------------------------------------------- */

static Vertex half(Vertex v) {
    Vertex h;
    h.x = (int16_t)(v.x >> 1);
    h.y = (int16_t)(v.y >> 1);
    h.z = (int16_t)(v.z >> 1);
    return h;
}

static void segment(Vertex a, Vertex b, int *drawn) {
    put(SEGMENT_POINTS, a);
    put(SEGMENT_POINTS + 6, b);
    *drawn |= draw_clipped_segment();
}

int draw_segment_grid(gaddr *stream) {
    gaddr q;
    int16_t rows;
    Vertex u, v;
    int drawn = 0;

    wr_u16(LINE_STYLE, 0x0F);
    wr_u16(LINE_STYLE + 2, 0xFFFF);
    wr_u16(POLY_COMPLEMENT, 0);
    wr_u16(POLY_MASK_BLIT, 0);
    wr_u16(CURRENT_COLOUR, (uint16_t)next_word(stream));
    q = vertex_at(next_word(stream)) + 12;
    rows = next_word(stream);
    u = minus(get(q - 12), get(q));
    v = minus(get(q - 6), get(q));
    for (;;) {
        int16_t columns = next_word(stream);
        Vertex along = {0, 0, 0};
        for (;;) {
            Vertex a = half(along);
            segment(plus(get(q), a), plus(plus(get(q), v), a), &drawn);
            if (--columns <= 0) break;
            along = plus(along, u);
        }
        if (--rows <= 0) break;
        q += 6;
    }
    return drawn;
}

int draw_segment_lattice(gaddr *stream) {
    gaddr q;
    int16_t count, n;
    Vertex u, v, far, along;
    int drawn = 0;

    wr_u16(LINE_STYLE, 0x0F);
    wr_u16(LINE_STYLE + 2, 0xFFFF);
    wr_u16(POLY_COMPLEMENT, 0);
    wr_u16(POLY_MASK_BLIT, 0);
    wr_u16(CURRENT_COLOUR, (uint16_t)next_word(stream));
    q = vertex_at(next_word(stream)) + 12;
    count = next_word(stream);
    u = minus(get(q - 12), get(q));
    v = minus(get(q - 6), get(q));
    far = plus(minus(get(q), half(v)), u);
    /* Lines across v, then lines across u from the far corner. */
    along.x = along.y = along.z = 0;
    for (n = count;;) {
        Vertex s0 = plus(get(q), half(along));
        segment(s0, plus(s0, u), &drawn);
        if (--n <= 0) break;
        along = plus(along, v);
    }
    along.x = along.y = along.z = 0;
    for (n = count;;) {
        Vertex s0 = minus(far, half(along));
        segment(s0, minus(s0, u), &drawn);
        if (--n <= 0) break;
        along = plus(along, v);
    }
    return drawn;
}

/* ---- more derived points --------------------------------------------------- */

void offset_block_copies(gaddr *stream) {
    gaddr base = vertex_at(next_word(stream)), ref = vertex_at(rd_s16(*stream));
    int k, i;
    *stream += 2;
    for (k = 0; k < 3; k++) {
        Vertex d = minus(get(ref + (gaddr)(6 * k)), get(base));
        for (i = 1; i <= 5; i++) put(base + 0xB4 + (gaddr)(0x1E * k) + (gaddr)(6 * (i - 1)), plus(get(base + (gaddr)(6 * i)), d));
    }
    *stream += 2 + (gaddr)(int32_t)rd_s16(*stream);
}

void extend_block_scaled(gaddr *stream) {
    gaddr v = vertex_at(next_word(stream));
    int16_t shift = next_word(stream);
    Vertex q1 = get(v + 6), q2 = get(v + 12), q3 = get(v + 18), b = minus(q2, q3), f;
    int s = (shift < 0 ? (int16_t)-shift : shift) & 63;
    if (shift >= 0) {
        b.x = (int16_t)(s >= 16 ? 0 : (uint16_t)b.x << s);
        b.y = (int16_t)(s >= 16 ? 0 : (uint16_t)b.y << s);
        b.z = (int16_t)(s >= 16 ? 0 : (uint16_t)b.z << s);
    } else {
        b.x = (int16_t)(s >= 16 ? (b.x < 0 ? -1 : 0) : b.x >> s);
        b.y = (int16_t)(s >= 16 ? (b.y < 0 ? -1 : 0) : b.y >> s);
        b.z = (int16_t)(s >= 16 ? (b.z < 0 ? -1 : 0) : b.z >> s);
    }
    put(v + 0x3C, minus(q3, b));
    put(v + 0x36, plus(q3, minus(q2, q1)));
    put(v + 0x42, plus(get(v + 0x2A), minus(get(v + 0x1E), get(v + 0x24))));
    put(v + 0x48, plus(get(v + 0x42), minus(get(v + 0x30), get(v + 0x24))));
    f = minus(get(v + 0x18), get(v + 0x24));
    put(v + 0x4E, plus(get(v + 0x1E), f));
    put(v + 0x54, plus(get(v + 0x30), f));
    put(v + 0x5A, plus(get(v + 0x2A), f));
    put(v + 0x60, plus(get(v + 0x42), f));
    put(v + 0x66, plus(get(v + 0x48), f));
}

/* ---- more faces ------------------------------------------------------------ */

int draw_side_face(gaddr *stream) {
    gaddr bound = rd_u32(BOUND_RECORD), in = CLIP_INPUT + 4, v;
    int16_t a, b, first, second, shift = (int16_t)(rd_u8(bound + 6) & 15), grid = rd_s16(BOUND_SHIFT);
    int16_t ax, az, ex, ez;
    int side, flagged, first_path;
    Vertex p0;

    wr_u16(CLIP_INPUT + 2, 3); /* the shift word is left as it was */
    wr_u16(CURRENT_COLOUR, (uint16_t)next_word(stream));
    a = next_word(stream);
    b = next_word(stream);
    flagged = b < 0;
    b &= 0x7FFF;
    ax = rd_s16(bound + 0xA + (gaddr)(int32_t)a);
    az = rd_s16(bound + 0xE + (gaddr)(int32_t)a);
    ex = (int16_t)(ax - rd_s16(bound + 0xA + (gaddr)(int32_t)b));
    ez = (int16_t)(az - rd_s16(bound + 0xE + (gaddr)(int32_t)b));
    /* Which side of the edge a..b the eye is on. */
    ax = (int16_t)((ax >> shift) + rd_s16(BOUND_OFFSET_X));
    az = (int16_t)((az >> shift) + rd_s16(BOUND_OFFSET_Z));
    ax = (int16_t)((uint16_t)ax << (grid & 15)) ;
    az = (int16_t)((uint16_t)az << (grid & 15));
    if ((grid & 63) >= 16) ax = az = 0;
    ax = (int16_t)(ax + rd_s16(PROJECTION_WORDS));
    az = (int16_t)(az + rd_s16(PROJECTION_WORDS + 4));
    side = (int64_t)((int32_t)ax * ex) + (int32_t)az * ez >= 0 ? 1 : -1; /* BGE after ADD.L: the true sign */
    first = next_word(stream);
    p0 = get(vertex_at(first));
    put(in, p0);
    second = next_word(stream);
    v = WORKSPACES + (gaddr)(int32_t)second;
    first_path = (side >= 0) != flagged;
    if (first_path) {
        Vertex p1 = get(v), p2 = get(v + 12);
        put(in + 6, p1);
        put(in + 12, p2);
        if ((int16_t)(p0.z & p1.z & p2.z) < 0) return 0;
    } else {
        Vertex q0 = get(v), q1 = get(v + 6), p2 = minus(get(v + 12), minus(q1, q0));
        put(in + 6, q1);
        put(in + 12, p2);
        if ((int16_t)(p2.z & p0.z & q1.z) < 0) return 0;
    }
    return clip_and_draw_polygon();
}

int draw_quad_list(gaddr *stream) {
    int drawn = 0;
    wr_u16(CURRENT_COLOUR, 12);
    wr_u16(LINE_STYLE, 8);
    wr_u16(LINE_STYLE + 2, 8);
    wr_u16(POLY_COMPLEMENT, 0);
    wr_u16(POLY_MASK_BLIT, 0);
    wr_u32(CLIP_INPUT, 4);
    for (;;) {
        int16_t o[4];
        gaddr in = CLIP_INPUT + 4;
        uint16_t behind = 0xFFFF;
        int k;
        o[0] = next_word(stream);
        if (o[0] < 0) break;
        for (k = 1; k < 4; k++) o[k] = next_word(stream);
        for (k = 0; k < 4; k++, in += 6) {
            Vertex p = get(vertex_at(o[k]));
            put(in, p);
            behind &= (uint16_t)p.z;
        }
        if ((int16_t)behind < 0) continue;
        drawn |= clip_and_draw_polygon();
    }
    return drawn;
}

int draw_quad_strip(gaddr *stream) {
    gaddr q;
    int16_t count;
    Vertex a, b;
    int drawn = 0;

    wr_u16(LINE_STYLE, 2);
    wr_u16(LINE_STYLE + 2, 0);
    wr_u16(POLY_COMPLEMENT, 2);
    wr_u16(POLY_MASK_BLIT, 0);
    q = vertex_at(next_word(stream)) + 18;
    count = next_word(stream);
    {
        Vertex p0 = get(q - 18), p1 = get(q - 12), p3 = get(q), d = minus(p1, p3);
        Vertex tripled;
        tripled.x = (int16_t)((int16_t)(d.x * 3) >> 1);
        tripled.y = (int16_t)((int16_t)(d.y * 3) >> 1);
        tripled.z = (int16_t)((int16_t)(d.z * 3) >> 1);
        a = plus(tripled, minus(p0, p3));
        b = minus(get(q - 6), p3);
    }
    wr_u32(CLIP_INPUT, 4);
    for (;;) {
        Vertex p = get(q);
        put(CLIP_INPUT + 4, p);
        put(CLIP_INPUT + 10, plus(p, a));
        put(CLIP_INPUT + 16, plus(plus(p, b), a));
        put(CLIP_INPUT + 22, plus(p, b));
        drawn |= clip_and_draw_polygon();
        if (--count <= 0) break;
        q += 6;
    }
    return drawn;
}

/* ---- grids of faces -------------------------------------------------------- */

static int grid_face(Vertex p0, Vertex p1, Vertex p2, Vertex p3) {
    wr_u32(CLIP_INPUT, 4);
    put(CLIP_INPUT + 4, p0);
    put(CLIP_INPUT + 10, p1);
    put(CLIP_INPUT + 16, p2);
    put(CLIP_INPUT + 22, p3);
    return clip_and_draw_polygon();
}

static int face_grid(gaddr *stream) {
    gaddr q;
    int16_t rows;
    Vertex u, v, w;
    int drawn = 0;

    q = vertex_at(next_word(stream)) + 18;
    rows = next_word(stream);
    u = minus(get(q - 18), get(q));
    v = minus(get(q - 12), get(q));
    w = minus(get(q - 6), get(q));
    for (;;) {
        int16_t columns = next_word(stream);
        Vertex along = {0, 0, 0};
        for (;;) {
            Vertex a = half(along), p = plus(get(q), a), r = plus(plus(get(q), w), a);
            drawn |= grid_face(p, plus(p, u), plus(r, u), r);
            if (--columns <= 0) break;
            along = plus(along, v);
        }
        if (--rows <= 0) break;
        q += 6;
    }
    return drawn;
}

int draw_face_grid(gaddr *stream) {
    wr_u16(CURRENT_COLOUR, 0x0D);
    wr_u16(LINE_STYLE, 2);
    wr_u16(LINE_STYLE + 2, 0);
    wr_u16(POLY_COMPLEMENT, 2);
    wr_u16(POLY_MASK_BLIT, 0);
    return face_grid(stream);
}

int draw_face_grid_plain(gaddr *stream) {
    wr_u32(LINE_STYLE, 0x000FFFFF);
    wr_u16(CURRENT_COLOUR, (uint16_t)next_word(stream));
    return face_grid(stream);
}

static int face_lattice(gaddr *stream) {
    gaddr p2a;
    int16_t count, n;
    Vertex u, v, h, far1, far2, along;
    int drawn = 0;

    p2a = vertex_at(next_word(stream)) + 12;
    count = next_word(stream);
    u = minus(get(p2a - 12), get(p2a));
    v = minus(get(p2a - 6), get(p2a));
    h = half(v);
    far1 = plus(minus(get(p2a), h), u);
    far2 = plus(minus(get(p2a + 6), h), u);
    along.x = along.y = along.z = 0;
    for (n = count;;) {
        Vertex a = half(along), p = plus(get(p2a), a), r = plus(get(p2a + 6), a);
        drawn |= grid_face(p, plus(p, u), plus(r, u), r);
        if (--n <= 0) break;
        along = plus(along, v);
    }
    /* The faces back from the far edge are drawn, but not counted. */
    along.x = along.y = along.z = 0;
    for (n = count;;) {
        Vertex a = half(along), p = minus(far1, a), r = minus(far2, a);
        grid_face(p, minus(p, u), minus(r, u), r);
        if (--n <= 0) break;
        along = plus(along, v);
    }
    return drawn;
}

int draw_face_lattice(gaddr *stream) {
    wr_u16(CURRENT_COLOUR, 0x0D);
    wr_u16(LINE_STYLE, 2);
    wr_u16(LINE_STYLE + 2, 0);
    wr_u16(POLY_COMPLEMENT, 2);
    wr_u16(POLY_MASK_BLIT, 0);
    return face_lattice(stream);
}

int draw_face_lattice_plain(gaddr *stream) {
    wr_u32(LINE_STYLE, 0x000FFFFF);
    wr_u16(CURRENT_COLOUR, (uint16_t)next_word(stream));
    return face_lattice(stream);
}
