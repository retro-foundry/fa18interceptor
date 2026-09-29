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
