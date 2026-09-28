#include "vertex_tail.h"

#include "globals.h"

/* A vertex: three coordinate words, wrapping as the 68000 word arithmetic
 * does. */
typedef struct {
    int16_t x, y, z;
} Vertex;

static Vertex get(gaddr w, int offset) {
    Vertex v;
    v.x = rd_s16(w + (gaddr)offset);
    v.y = rd_s16(w + (gaddr)offset + 2);
    v.z = rd_s16(w + (gaddr)offset + 4);
    return v;
}

static void put(gaddr w, int offset, Vertex v) {
    wr_s16(w + (gaddr)offset, v.x);
    wr_s16(w + (gaddr)offset + 2, v.y);
    wr_s16(w + (gaddr)offset + 4, v.z);
}

static Vertex add(Vertex a, Vertex b) {
    Vertex v = {(int16_t)(a.x + b.x), (int16_t)(a.y + b.y), (int16_t)(a.z + b.z)};
    return v;
}

static Vertex sub(Vertex a, Vertex b) {
    Vertex v = {(int16_t)(a.x - b.x), (int16_t)(a.y - b.y), (int16_t)(a.z - b.z)};
    return v;
}

static Vertex half(Vertex a) {
    Vertex v = {(int16_t)(a.x >> 1), (int16_t)(a.y >> 1), (int16_t)(a.z >> 1)};
    return v;
}

/* (a + b) >> 1, the sum wrapping to a word first. */
static Vertex midpoint(Vertex a, Vertex b) {
    return half(add(a, b));
}

void derive_vertex_tail(gaddr w) {
    Vertex p = get(w, 0x12), q = get(w, 0x18), t = get(w, 0x1E);
    Vertex r = get(w, 0x36), s = get(w, 0x30);
    Vertex m = midpoint(p, q), a, b, u, edge, x, l, n;

    put(w, 0x84, m);
    a = sub(m, sub(r, m));          /* 2m - r */
    put(w, 0x8A, a);
    b = add(sub(r, s), a);          /* 2m - s */
    put(w, 0x90, b);
    edge = sub(t, p);
    u = add(q, edge);
    put(w, 0x96, u);
    put(w, 0xE4, add(a, edge));
    put(w, 0xEA, add(b, edge));
    put(w, 0xA8, midpoint(add(b, edge), add(a, edge)));
    x = midpoint(u, t);
    put(w, 0x9C, midpoint(x, t));
    put(w, 0xA2, midpoint(x, u));
    put(w, 0xAE, midpoint(get(w, 0x0C), get(w, 0x06)));

    l = sub(get(w, 0x24), get(w, 0x2A));
    put(w, 0x7E, add(get(w, 0x3C), l));
    put(w, 0xB4, add(get(w, 0x48), l));
    put(w, 0xBA, add(get(w, 0x54), l));
    n = add(l, half(l));
    put(w, 0xC0, add(get(w, 0x06), n));
    put(w, 0xC6, add(get(w, 0x0C), n));
    put(w, 0xD8, add(get(w, 0xC0), l));
    put(w, 0xDE, add(get(w, 0xC6), l));
    n = add(n, l);
    put(w, 0xCC, add(get(w, 0x42), n));
    put(w, 0xD2, add(get(w, 0x4E), n));
}

gaddr derive_shown_vertices(gaddr stream) {
    gaddr shown = CONTROL_RECORDS + (gaddr)(int32_t)(int16_t)(rd_s16(SCRIPT_RECORD) + 0xA4);
    gaddr w = WORKSPACES + (gaddr)(int32_t)rd_s16(stream);

    derive_vertex_tail(shown);
    derive_vertex_tail(w);
    put(w, 0x294, midpoint(midpoint(get(w, 0x6C), get(w, 0x72)), get(w, 0x60)));
    return stream + 2;
}

gaddr derive_edge_vertices(gaddr stream) {
    int16_t a = rd_s16(stream), b = rd_s16(stream + 2), w = rd_s16(stream + 4);
    gaddr bank = WORKSPACES, target = WORKSPACES + (gaddr)(int32_t)w;
    Vertex edge = sub(get(bank + (gaddr)(int32_t)b, 0), get(bank + (gaddr)(int32_t)a, 0));
    int steps;

    put(target, 0x12, add(get(target, 0x00), edge));
    edge = half(edge);
    put(target, 0x18, add(get(target, 0x06), edge));
    put(target, 0x1E, add(get(target, 0x0C), edge));
    steps = (int8_t)(rd_u8(CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(SCRIPT_RECORD) + 0x7C) & 0x7F) >> 4;
    return stream + 6 + (gaddr)(14 * steps);
}
