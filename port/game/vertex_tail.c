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

static void parallelogram_vertices(gaddr workspace) {
    static const int starts[]={0,0x12,0x2a,0x3c,0x4e};
    for(unsigned i=0;i<5;++i) {
        int at=starts[i];
        put(workspace,0x60+6*(int)i,
            add(get(workspace,at),sub(get(workspace,at+12),get(workspace,at+6))));
    }
}
void derive_shown_parallelogram_vertices(void) {
    parallelogram_vertices(CONTROL_RECORDS+(gaddr)(int32_t)(int16_t)(rd_s16(SCRIPT_RECORD)+0xa4));
    parallelogram_vertices(WORKSPACES);
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
static Vertex quarter(Vertex a) {
    Vertex v={(int16_t)(a.x>>2),(int16_t)(a.y>>2),(int16_t)(a.z>>2)};return v;
}
static void compact_tail(gaddr w) {
    Vertex p=get(w,0x12),q=get(w,0x18),edge=sub(get(w,0x1e),p);
    put(w,0x54,add(q,edge));
    put(w,0x5a,add(q,sub(p,get(w,0x2a))));
    put(w,0x3c,sub(q,sub(get(w,0x24),p)));
    Vertex upper=add(midpoint(p,q),quarter(edge));
    put(w,0x42,upper);put(w,0x48,midpoint(upper,get(w,0)));
    Vertex split=quarter(sub(get(w,0x36),get(w,0x2a)));
    put(w,0x4e,add(add(get(w,0x2a),split),half(split)));
}
gaddr derive_compact_shown_vertices(gaddr stream) {
    compact_tail(CONTROL_RECORDS+(gaddr)(int32_t)(int16_t)(rd_s16(SCRIPT_RECORD)+0xa4));
    compact_tail(WORKSPACES+(gaddr)(int32_t)rd_s16(stream));return stream+2;
}
static void extended_tail(gaddr w) {
    Vertex p=get(w,0x12),q=get(w,0x18),edge=sub(get(w,0x1e),p);
    put(w,0x96,add(q,edge));
    Vertex base=add(q,sub(p,get(w,0x30)));
    put(w,0x66,base);put(w,0x8a,add(base,edge));
    Vertex across=sub(get(w,0x24),get(w,6));
    put(w,0xa2,add(get(w,0x24),across));
    Vertex moved=add(get(w,0xc),across);
    put(w,0x6c,moved);put(w,0xa8,add(moved,across));
    put(w,0x9c,add(get(w,0x36),across));
    Vertex half_edge=half(across);
    put(w,0x72,add(get(w,0x2a),half_edge));put(w,0x78,add(get(w,0x60),half_edge));
    Vertex wing=add(midpoint(p,q),add(add(across,across),half_edge));
    put(w,0x7e,wing);put(w,0x84,midpoint(wing,get(w,0)));
    put(w,0xae,add(get(w,0),half_edge));put(w,0x90,midpoint(get(w,0x30),get(w,0x42)));
}
gaddr derive_extended_shown_vertices(gaddr stream) {
    extended_tail(CONTROL_RECORDS+(gaddr)(int32_t)(int16_t)(rd_s16(SCRIPT_RECORD)+0xa4));
    gaddr w=WORKSPACES+(gaddr)(int32_t)rd_s16(stream);extended_tail(w);
    put(w,0x258,midpoint(midpoint(get(w,0x4e),get(w,0x54)),get(w,0x3c)));
    return stream+2;
}
