/* Commands of an object's draw stream. */
#include "draw_stream.h"

#include "globals.h"
#include "plane_tests.h"
#include "polygon_clip.h"
#include "render_line.h"
#include "projection.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct { int16_t x, y, z; } Vertex;

static int16_t next_word(gaddr *stream) {
    int16_t w = rd_s16(*stream);
    *stream += 2;
    return w;
}

static gaddr vertex_at(int16_t offset) { return WORKSPACES + (gaddr)(int32_t)offset; }

/* C206FE-C20798 store alternating lanes of fixed-point intermediate points.
 * DIVS overflow preserves the dividend before EXT.L takes its low word. */
static void interpolate_segment_lane(gaddr first,gaddr last,gaddr output,int16_t divisions) {
    int32_t position[3],step[3];
    if(!divisions) { /* Original DIVS divide-by-zero exception. */
        fputs("model interpolation has a zero divisor\n",stderr);abort();
    }
    for(unsigned k=0;k<3;++k) {
        position[k]=(int32_t)rd_s16(first+2*k)*16;
        int32_t delta=(int32_t)rd_s16(last+2*k)*16-position[k];
        int32_t quotient=delta/divisions;
        step[k]=quotient<-32768 || quotient>32767?(int16_t)delta:(int16_t)quotient;
    }
    const unsigned count=(uint16_t)(divisions-2)+1u; /* Initial body, then DBRA. */
    for(unsigned i=0;i<count;++i,output+=24) {
        for(unsigned k=0;k<3;++k) {
            position[k]=(int32_t)((uint32_t)position[k]+(uint32_t)step[k]);
            wr_u32(output+4*k,(uint32_t)position[k]);
        }
    }
}
int draw_interpolated_segments(gaddr *stream,gaddr frame) {
    const gaddr points=0xc4ad90u;
    wr_u16(frame-0x7e,0);
    const int16_t divisions=next_word(stream);
    wr_s16(frame-0x30,divisions);
    for(unsigned lane=0;lane<2;++lane) {
        gaddr first=vertex_at(next_word(stream)),last=vertex_at(next_word(stream));
        interpolate_segment_lane(first,last,points+12*lane,divisions);
    }
    wr_s16(CURRENT_COLOUR,next_word(stream));
    gaddr point=points;
    wr_u16(frame-0x30,(uint16_t)(rd_u16(frame-0x30)-1));
    do {
        for(unsigned k=0;k<6;++k) {
            const int32_t value=rd_s32(point+4*k);
            /* C207B4-C207D6 round the shifted low word using ASR's carry. */
            wr_s16(SEGMENT_POINTS+2*k,(int16_t)((value>>4)+((uint32_t)value>>3&1u)));
        }
        point+=24;
        wr_u16(frame-0x7e,(uint16_t)(rd_u16(frame-0x7e)|draw_clipped_segment()));
        wr_u16(frame-0x30,(uint16_t)(rd_u16(frame-0x30)-1));
    } while(rd_s16(frame-0x30)>0);
    return rd_u16(frame-0x7e);
}

int draw_stream_circles(gaddr *stream,gaddr frame) {
    wr_u16(frame-0x7e,0); wr_u16(frame-0x6e,0);
    while(!rd_u16(frame-0x6e)) {
        gaddr point=vertex_at(next_word(stream));
        wr_u16(CURRENT_COLOUR,(uint16_t)next_word(stream));
        uint16_t radius=(uint16_t)next_word(stream);
        if(radius&0x8000u) { wr_u16(frame-0x6e,1); radius&=0x7fffu; }
        draw_scaled_view_circle(point,rd_s16(frame-8),(int16_t)radius);
        wr_u16(frame-0x7e,(uint16_t)(rd_u16(frame-0x7e)|(rd_u32(PROJECTED_PAIR)!=0xffffffffu)));
    }
    return rd_u16(frame-0x7e);
}

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

static int selected_segment(gaddr *stream, int clipped) {
    gaddr first, second;
    uint16_t depth0, depth1;
    wr_u32(LINE_STYLE, 0xFFFFFFFFu);
    first = vertex_at(next_word(stream));
    wr_u32(SEGMENT_POINTS, rd_u32(first));
    depth0 = rd_u16(first + 4);
    wr_u16(SEGMENT_POINTS + 4, depth0);
    second = vertex_at(next_word(stream));
    wr_u32(SEGMENT_POINTS + 6, rd_u32(second));
    depth1 = rd_u16(second + 4);
    wr_u16(CURRENT_COLOUR, (uint16_t)next_word(stream));
    if ((int16_t)(depth0 & depth1) < 0) return 0;
    wr_u16(SEGMENT_POINTS + 10, depth1);
    return clipped ? draw_clipped_segment() : draw_projected_segment();
}

int draw_selected_segment(gaddr *stream) {
    return selected_segment(stream, 0);
}
int draw_selected_segment_clipped(gaddr *stream) {
    return selected_segment(stream, 1);
}

int draw_selected_segment_near(gaddr *stream) {
    if (rd_s32(PROJECTION_Y) >= -0xC0) {
        *stream += 6;
        return 0;
    }
    return selected_segment(stream, 1);
}

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

static int lattice_face(Vertex p0, Vertex p1, Vertex p2, Vertex p3,
                        const LatticeFaceHooks *hooks, int back, Vertex along) {
    int drawn;
    wr_u32(CLIP_INPUT, 4);
    put(CLIP_INPUT + 4, p0);
    put(CLIP_INPUT + 10, p1);
    put(CLIP_INPUT + 16, p2);
    put(CLIP_INPUT + 22, p3);
    if (hooks && hooks->before) {
        int16_t v[3] = {along.x, along.y, along.z};
        hooks->before(back, v, hooks->context);
    }
    drawn = clip_and_draw_polygon();
    if (hooks && hooks->after) hooks->after(back, drawn, hooks->context);
    return drawn;
}

static int face_lattice(gaddr *stream, const LatticeFaceHooks *hooks) {
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
        drawn |= lattice_face(p, plus(p, u), plus(r, u), r, hooks, 0, along);
        if (--n <= 0) break;
        along = plus(along, v);
    }
    /* The faces back from the far edge are drawn, but not counted. */
    along.x = along.y = along.z = 0;
    for (n = count;;) {
        Vertex a = half(along), p = minus(far1, a), r = minus(far2, a);
        lattice_face(p, minus(p, u), minus(r, u), r, hooks, 1, along);
        if (--n <= 0) break;
        along = plus(along, v);
    }
    return drawn;
}

int draw_face_lattice_with_hooks(gaddr *stream, const LatticeFaceHooks *hooks) {
    wr_u16(CURRENT_COLOUR, 0x0D);
    wr_u16(LINE_STYLE, 2);
    wr_u16(LINE_STYLE + 2, 0);
    wr_u16(POLY_COMPLEMENT, 2);
    wr_u16(POLY_MASK_BLIT, 0);
    return face_lattice(stream, hooks);
}

int draw_face_lattice_plain_with_hooks(gaddr *stream, const LatticeFaceHooks *hooks) {
    wr_u32(LINE_STYLE, 0x000FFFFF);
    wr_u16(CURRENT_COLOUR, (uint16_t)next_word(stream));
    return face_lattice(stream, hooks);
}

int draw_face_lattice(gaddr *stream) {
    return draw_face_lattice_with_hooks(stream, 0);
}

int draw_face_lattice_plain(gaddr *stream) {
    return draw_face_lattice_plain_with_hooks(stream, 0);
}

int draw_block_face(gaddr *stream) {
    gaddr block, in = CLIP_INPUT + 4;
    Vertex side, across, p;

    wr_u16(CURRENT_COLOUR, (uint16_t)next_word(stream));
    block = vertex_at(next_word(stream));
    if (rd_s32(PROJECTION_Y) < -0x80) return 0;
    wr_u16(CLIP_INPUT, 0);
    wr_u16(CLIP_INPUT + 2, 4);
    side = edge_after(block);
    across = edge_after(block + 6);
    p = get(block + 18);
    put(in, p);
    p = plus(p, side);
    put(in + 6, p);
    p = plus(p, across);
    put(in + 12, p);
    p = minus(p, side);
    put(in + 18, p);
    /* Every corner behind the eye: nothing. */
    if ((int16_t)(rd_u16(in + 4) & rd_u16(in + 10) & rd_u16(in + 16) & rd_u16(in + 22)) < 0) return 0;
    return clip_and_draw_polygon();
}

int draw_offset_run(gaddr *stream) {
    uint16_t head = (uint16_t)next_word(stream);
    int16_t count = (int16_t)(head >> 8);
    Vertex shift;
    gaddr points;
    int drawn = 0;

    wr_u16(CURRENT_COLOUR, head & 0x3F);
    shift = edge_after(vertex_at(next_word(stream)));
    points = vertex_at(next_word(stream));
    do {
        put(SEGMENT_POINTS, minus(get(points), shift));
        put(SEGMENT_POINTS + 6, minus(get(points + 6), shift));
        points += 12;
        drawn |= draw_clipped_segment();
    } while (--count > 0);
    return drawn;
}

int draw_split_square(gaddr *stream) {
    uint32_t colours = (uint32_t)rd_u16(*stream) << 16 | rd_u16(*stream + 2);
    Vertex corner = get(WORKSPACES + 6);
    int16_t shift = rd_s16(BOUND_SHIFT), dx, dz, a, b;
    int32_t sx, sz;
    int same;
    gaddr in = CLIP_INPUT + 4;

    *stream += 4;
    if (corner.x > corner.z || (int16_t)-corner.x > corner.z || corner.y > corner.z || (int16_t)-corner.y > corner.z)
        return -1;
    wr_u32(CLIP_INPUT, 3);
    a = (int16_t)((uint16_t)rd_s16(BOUND_OFFSET_X) << (shift & 63));
    b = (int16_t)((uint16_t)rd_s16(BOUND_OFFSET_Z) << (shift & 63));
    if ((shift & 63) >= 16) a = b = 0;
    /* The eye's offsets from the square's corner, with their true signs. */
    sx = (int32_t)(int16_t)-rd_s16(PROJECTION_WORDS) - a;
    sz = (int32_t)(int16_t)-rd_s16(PROJECTION_WORDS + 4) - b;
    dx = (int16_t)sx;
    dz = (int16_t)sz;
    same = (sx < 0) == (sz < 0);
    if (sx < 0) dx = (int16_t)-dx;
    if (sz < 0) dz = (int16_t)-dz;
    wr_u16(CURRENT_COLOUR, (uint16_t)(dz > dx ? colours : colours >> 16));
    put(in, get(WORKSPACES));
    if (same) {
        put(in + 6, get(WORKSPACES + 6));
        put(in + 12, get(WORKSPACES + 0x18));
    } else {
        put(in + 6, get(WORKSPACES + 0xC));
        put(in + 12, get(WORKSPACES + 0x12));
    }
    return clip_and_draw_polygon();
}

/* Which side of the bound record's edge a..b the eye is on: 1 at or in
 * front of it (the BGE after ADD.L takes the true sign). */
static int eye_side(int16_t a, int16_t b) {
    gaddr bound = rd_u32(BOUND_RECORD);
    int16_t shift = (int16_t)(rd_u8(bound + 6) & 15), grid = rd_s16(BOUND_SHIFT);
    int16_t ax = rd_s16(bound + 0xA + (gaddr)(int32_t)a), az = rd_s16(bound + 0xE + (gaddr)(int32_t)a);
    int16_t ex = (int16_t)(ax - rd_s16(bound + 0xA + (gaddr)(int32_t)b));
    int16_t ez = (int16_t)(az - rd_s16(bound + 0xE + (gaddr)(int32_t)b));

    ax = (int16_t)((ax >> shift) + rd_s16(BOUND_OFFSET_X));
    az = (int16_t)((az >> shift) + rd_s16(BOUND_OFFSET_Z));
    ax = (int16_t)((uint16_t)ax << (grid & 15));
    az = (int16_t)((uint16_t)az << (grid & 15));
    if ((grid & 63) >= 16) ax = az = 0;
    ax = (int16_t)(ax + rd_s16(PROJECTION_WORDS));
    az = (int16_t)(az + rd_s16(PROJECTION_WORDS + 4));
    return (int64_t)((int32_t)ax * ex) + (int32_t)az * ez >= 0;
}

int draw_side_triangle(gaddr *stream) {
    gaddr in = CLIP_INPUT + 4, block;
    Vertex edge, q0, q1, q2, p;
    int16_t a, b, c;
    int flagged, first_path;

    wr_u16(CLIP_INPUT + 2, 3); /* the shift word is left as it was */
    wr_u16(CURRENT_COLOUR, (uint16_t)next_word(stream));
    edge = edge_after(vertex_at(next_word(stream)));
    a = next_word(stream);
    b = next_word(stream);
    flagged = b < 0;
    first_path = eye_side(a, (int16_t)(b & 0x7FFF)) != flagged;
    c = next_word(stream);
    put(in, minus(get(vertex_at(c)), edge));
    block = vertex_at(next_word(stream));
    q0 = get(block);
    q1 = get(block + 6);
    q2 = get(block + 12);
    p = get(block + 18);
    if (first_path) {
        put(in + 6, p);
        p = plus(plus(p, minus(q1, q0)), minus(q2, q1));
    } else {
        p = plus(p, minus(q1, q0));
        put(in + 6, p);
        p = minus(plus(p, minus(q2, q1)), minus(q1, q0));
    }
    put(in + 12, p);
    if ((int16_t)(rd_u16(in + 4) & rd_u16(in + 10) & rd_u16(in + 16)) < 0) return 0;
    return clip_and_draw_polygon();
}

/* The ground-square colour choice of $C20592/$C203D0: 1 when the eye's x and
 * z offsets from the square's corner have the same sign; `colour` gets the
 * second word of `colours` when the z offset is the larger, else the first. */
int square_diagonal(uint32_t colours, uint16_t *colour) {
    int16_t shift = rd_s16(BOUND_SHIFT), a, b, dx, dz;
    int32_t sx, sz;

    a = (int16_t)((uint16_t)rd_s16(BOUND_OFFSET_X) << (shift & 63));
    b = (int16_t)((uint16_t)rd_s16(BOUND_OFFSET_Z) << (shift & 63));
    if ((shift & 63) >= 16) a = b = 0;
    sx = (int32_t)(int16_t)-rd_s16(PROJECTION_WORDS) - a;
    sz = (int32_t)(int16_t)-rd_s16(PROJECTION_WORDS + 4) - b;
    dx = (int16_t)(sx < 0 ? -(int16_t)sx : (int16_t)sx);
    dz = (int16_t)(sz < 0 ? -(int16_t)sz : (int16_t)sz);
    *colour = (uint16_t)(dz > dx ? colours : colours >> 16);
    return (sx < 0) == (sz < 0);
}

int draw_square_faces(gaddr *stream) {
    uint32_t colours = (uint32_t)rd_u16(*stream) << 16 | rd_u16(*stream + 2);
    uint16_t colour, second = rd_u16(*stream + 4);
    Vertex v0 = get(WORKSPACES), v1 = get(WORKSPACES + 6), v2 = get(WORKSPACES + 12), v3 = get(WORKSPACES + 18);
    Vertex d = minus(v0, v1), e = minus(v2, v1), p;
    gaddr in = CLIP_INPUT + 4;
    int drawn;

    *stream += 6;
    if (v1.x > v1.z || (int16_t)-v1.x > v1.z || v1.y > v1.z || (int16_t)-v1.y > v1.z) return -1;
    wr_u32(CLIP_INPUT, 4);
    if (square_diagonal(colours, &colour)) {
        wr_u16(CURRENT_COLOUR, colour);
        put(in, v0);
        put(in + 6, v1);
        p = plus(v3, e);
        put(in + 12, p);
        put(in + 18, plus(p, d));
        drawn = clip_and_draw_polygon();
        wr_u16(CURRENT_COLOUR, second);
        if (!rd_u8(ATTITUDE_LATCH)) return drawn;
        put(in + 12, get(in + 18));
        p = plus(v0, e);
        put(in + 6, p);
        put(in + 18, minus(v0, minus(p, get(in + 12))));
    } else {
        wr_u16(CURRENT_COLOUR, colour);
        put(in, v2);
        put(in + 6, v3);
        put(in + 12, plus(v3, d));
        put(in + 18, plus(v2, d));
        drawn = clip_and_draw_polygon();
        wr_u16(CURRENT_COLOUR, second);
        if (!rd_u8(ATTITUDE_LATCH)) return drawn;
        {
            Vertex c2 = get(in + 12), c3 = get(in + 18);
            put(in, v0);
            put(in + 6, c3);
            put(in + 18, c2);
            put(in + 12, plus(c3, minus(c2, v0)));
        }
    }
    return drawn | clip_and_draw_polygon();
}

/* Locals of the stream's caller that the shadow command reads and sets. */
#define F_SHIFT_TOTAL (-2)   /* word */
#define F_SCALE       (-6)   /* word */
#define F_VERTEX_SHIFT (-8)  /* word */
#define F_COUNT       (-0xE) /* word */
#define F_ORIGIN      (-0x14)/* word[3] */
#define F_X           (-0x20)/* long */
#define F_Y           (-0x1C)/* long */
#define F_Z           (-0x18)/* long */
#define F_POINT       (-0x94)/* long[3] */

static int16_t frame_w(gaddr frame, int off) { return rd_s16(frame + (gaddr)(int32_t)off); }
static int32_t frame_l(gaddr frame, int off) { return rd_s32(frame + (gaddr)(int32_t)off); }

/* Whether the shadow of `record` at `height` is drawn at all: 1 drawn,
 * 0 not (the command returns 1), -1 far below (the command returns -1). */
static int shadow_wanted(gaddr record, gaddr frame, int32_t *height) {
    int32_t level = rd_s32(POSITION_BIAS), floor;
    uint8_t kind = rd_u8(record + 4) & 0xC0;

    if (kind != 0 && kind != 0xC0) return 0;
    if (rd_u8(record + 4) & 0x40) {
        floor = (int32_t)(int16_t)-rd_s16(record + 0x4E) << 8;
        if (level >= floor) return 0;
        level = (level - floor) >> (rd_s16(BOUND_SHIFT) & 63);
        wr_u32(frame + (gaddr)(int32_t)(F_POINT + 4), (uint32_t)level);
    } else {
        wr_u32(frame + (gaddr)(int32_t)(F_POINT + 4), (uint32_t)frame_l(frame, F_Y));
    }
    *height = level;
    if (level < -0x100000) return -1;
    if (!rd_u8(CONTEXT_SELECT) && (int8_t)rd_u8(ATTITUDE_BAND) < 3) {
        int32_t limit;
        if (rd_s16(STREAM_MODE) == rd_s16(TARGET_RECORD)) {
            if (!rd_u8(ATTITUDE_BAND)) return 0;
            limit = -0x2000;
        } else {
            if (rd_u8(record + 0x62) == 0x30 || level >= -0x10000 || rd_u8(record + 0x62) == 0x14) return 1;
            if ((int8_t)rd_u8(ATTITUDE_BAND) <= 1) return 0;
            limit = -0xA0000;
        }
        if (level < limit) return 0;
    }
    return 1;
}

/* A hull vertex at `offset` in the record's list, placed at the shadow. */
static Vertex shadow_vertex(gaddr hull, int16_t offset, gaddr frame) {
    int16_t shift = frame_w(frame, F_VERTEX_SHIFT);
    int16_t x = (int16_t)((int16_t)(rd_s16(hull + (gaddr)(int32_t)offset) >> (shift & 63)) + frame_w(frame, F_ORIGIN));
    int16_t y = frame_w(frame, F_ORIGIN + 2);
    int16_t z = (int16_t)((int16_t)(rd_s16(hull + 4 + (gaddr)(int32_t)offset) >> (shift & 63)) + frame_w(frame, F_ORIGIN + 4));
    Vertex v;
    gaddr m = VIEW_ANGLE_MATRIX;
    v.x = (int16_t)((int32_t)((uint32_t)((int32_t)rd_s16(m) * x) + (uint32_t)((int32_t)rd_s16(m + 2) * y) +
                              (uint32_t)((int32_t)rd_s16(m + 4) * z)) >> 8);
    v.y = (int16_t)((int32_t)((uint32_t)((int32_t)rd_s16(m + 6) * x) + (uint32_t)((int32_t)rd_s16(m + 8) * y) +
                              (uint32_t)((int32_t)rd_s16(m + 10) * z)) >> 8);
    v.z = (int16_t)((int32_t)((uint32_t)((int32_t)rd_s16(m + 12) * x) + (uint32_t)((int32_t)rd_s16(m + 14) * y) +
                              (uint32_t)((int32_t)rd_s16(m + 16) * z)) >> 8);
    return v;
}

int draw_record_shadow(gaddr *stream, gaddr frame) {
    gaddr record = CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(SCRIPT_RECORD), hull = record + 0xA4;
    int32_t height, top, scaled_y;
    int16_t shift, extra;
    int want = 0;

    want = shadow_wanted(record, frame, &height);
    if (want <= 0) return want < 0 ? -1 : 1;
    top = (int32_t)next_word(stream) << 8;
    if (top < rd_s32(record + 0x18)) return 1;
    shift = rd_s16(BOUND_SHIFT);
    extra = (int16_t)((int16_t)((int32_t)(int16_t)((uint32_t)-top >> 16) >> 5) - shift);
    if (extra > 0) {
        shift = (int16_t)(shift + extra);
        wr_u32(frame + (gaddr)(int32_t)F_POINT, (uint32_t)(frame_l(frame, F_X) >> (extra & 63)));
        wr_u32(frame + (gaddr)(int32_t)(F_POINT + 4), (uint32_t)(top >> (shift & 63)));
        wr_u32(frame + (gaddr)(int32_t)(F_POINT + 8), (uint32_t)(frame_l(frame, F_Z) >> (extra & 63)));
    } else {
        extra = 0;
        wr_u32(frame + (gaddr)(int32_t)F_POINT, (uint32_t)frame_l(frame, F_X));
        wr_u32(frame + (gaddr)(int32_t)(F_POINT + 8), (uint32_t)frame_l(frame, F_Z));
    }
    wr_u16(frame + (gaddr)(int32_t)F_SHIFT_TOTAL, (uint16_t)shift);
    {
        int down = (8 - frame_w(frame, F_SCALE)) & 63;
        int by = extra > 0 ? shift & 63 : 0;
        int32_t x = frame_l(frame, F_POINT) + (rd_s32(SHADOW_OFFSET_X) >> by);
        int32_t z = frame_l(frame, F_POINT + 8) + (rd_s32(SHADOW_OFFSET_Z) >> by);
        scaled_y = frame_l(frame, F_POINT + 4);
        wr_u16(frame + (gaddr)(int32_t)F_ORIGIN, (uint16_t)(x >> down));
        wr_u16(frame + (gaddr)(int32_t)(F_ORIGIN + 2), (uint16_t)(scaled_y >> down));
        wr_u16(frame + (gaddr)(int32_t)(F_ORIGIN + 4), (uint16_t)(z >> down));
    }
    wr_u16(CURRENT_COLOUR, 0);
    wr_u16(CLIP_INPUT, 0);
    for (;;) {
        gaddr face = *stream;
        int16_t count = rd_s16(face);
        int visible = 1;
        if (rd_s16(face + 2) == 0) {
            /* One-sided: the turn of its first three corners, seen from above. */
            int16_t a = rd_s16(face + 4), b = rd_s16(face + 6), c = rd_s16(face + 8);
            int16_t x0 = rd_s16(hull + (gaddr)(int32_t)a), z0 = rd_s16(hull + 4 + (gaddr)(int32_t)a);
            int16_t dxb = (int16_t)(rd_s16(hull + (gaddr)(int32_t)b) - x0), dxc = (int16_t)(rd_s16(hull + (gaddr)(int32_t)c) - x0);
            int16_t dzb = (int16_t)(rd_s16(hull + 4 + (gaddr)(int32_t)b) - z0);
            int16_t dzc = (int16_t)(rd_s16(hull + 4 + (gaddr)(int32_t)c) - z0);
            visible = (int32_t)((uint32_t)((int32_t)dxc * dzb) - (uint32_t)((int32_t)dxb * dzc)) >> 8 >= 0;
        }
        if (visible) {
            gaddr in = CLIP_INPUT + 4;
            int16_t left = count;
            *stream = face + 4;
            wr_u16(CLIP_INPUT + 2, (uint16_t)count);
            do {
                put(in, shadow_vertex(hull, next_word(stream), frame));
                in += 6;
                wr_u16(frame + (gaddr)(int32_t)F_COUNT, (uint16_t)--left);
            } while (left > 0);
            clip_and_draw_polygon();
        } else {
            *stream = face + 2 + 2 * count + 2;
        }
        {
            int16_t next = next_word(stream);
            if (next < 0 || (int32_t)next < rd_s32(record + 0x10)) return 1;
        }
    }
}

/* ---- face tests ---------------------------------------------------------- */

static FaceTestResult stream_face_result(gaddr *stream, gaddr frame) {
    gaddr in = CLIP_INPUT + 4, faces;
    int16_t eye[3];
    uint16_t kind;
    int k;

    for (k = 0; k < 3; k++) put(in + (gaddr)(6 * k), get(vertex_at(next_word(stream))));
    kind = (uint16_t)next_word(stream);
    faces = *stream;
    for (k = 0; k < 3; k++) eye[k] = rd_s16(frame - 0x26 + (gaddr)(2 * k));
    FaceTestResult result=face_test_result(kind,rd_u32(frame-0x2c),&faces,eye);
    *stream=faces+(result.passes?0x12:0);
    return result;
}
int test_stream_face(gaddr *stream,gaddr frame) { return stream_face_result(stream,frame).passes; }
uint16_t test_stream_face_accumulation(gaddr *stream,gaddr frame) {
    return (uint16_t)stream_face_result(stream,frame).accumulation;
}

/* The vertex offsets at `*face`, the last flagged by bit 15, into the
 * clipper input; at least three are read, and the first two unflagged.
 * -1 when every vertex is behind, else 0 with the count stored. */
static int build_face_vertices(gaddr *face) {
    gaddr in = CLIP_INPUT + 4;
    int16_t count = 3, behind, offset;

    put(in, get(vertex_at(next_word(face))));
    behind = rd_s16(in + 4);
    in += 6;
    put(in, get(vertex_at(next_word(face))));
    behind &= rd_s16(in + 4);
    in += 6;
    for (;;) {
        offset = next_word(face);
        if (offset < 0) break;
        put(in, get(vertex_at(offset)));
        behind &= rd_s16(in + 4);
        in += 6;
        count++;
    }
    put(in, get(vertex_at((int16_t)(offset & 0x7FFF))));
    behind &= rd_s16(in + 4);
    if (behind < 0) return -1;
    wr_u16(CLIP_INPUT + 2, (uint16_t)count);
    return 0;
}

/* Whether the face is drawn, and in what colour: the kind's own, or the
 * colour that follows it when the face test passes. `face` reads the kind
 * and that colour, `stream` is what the test reads and advances; `tested`
 * is 1 when the test passed, -1 when it failed, 0 when it did not run. */
static int face_colour(uint16_t kind, gaddr *face, gaddr *stream, gaddr frame,
                       uint16_t *colour, int *tested) {
    gaddr faces = *stream;
    int16_t eye[3];
    int k;

    *tested = 0;
    *colour = kind;
    if ((int16_t)kind < 0) return 1;
    for (k = 0; k < 3; k++) eye[k] = rd_s16(frame - 0x26 + (gaddr)(2 * k));
    *tested = face_test_passes(kind, rd_u32(frame - 0x2C), &faces, eye) ? 1 : -1;
    *stream = faces;
    if (*tested < 0) return 1;
    if (!(kind & 0x4000)) return 0;
    *colour = (uint16_t)next_word(face);
    return 1;
}

/* The kind word and the draw, once the clipper input holds the face: the
 * test is counted in the frame at -$32 and its failures at -$34. */
static int tested_face_tail(gaddr *stream, gaddr frame) {
    uint16_t kind = (uint16_t)next_word(stream), colour;
    int tested, draw = face_colour(kind, stream, stream, frame, &colour, &tested);

    if (tested) wr_u16(frame - 0x32, (uint16_t)(rd_u16(frame - 0x32) + 1));
    if (tested < 0) {
        wr_u16(frame - 0x34, (uint16_t)(rd_u16(frame - 0x34) + 1));
        if (kind & 0x4000) *stream += 2;
    }
    if (!draw) return -1;
    wr_u16(CURRENT_COLOUR, colour);
    return clip_and_draw_polygon();
}

int draw_tested_face(gaddr *stream, gaddr frame) {
    wr_u16(CLIP_INPUT, 0);
    if (build_face_vertices(stream)) return -1;
    return tested_face_tail(stream, frame);
}

int draw_tested_parallelogram(gaddr *stream, gaddr frame) {
    gaddr in = CLIP_INPUT + 4;
    Vertex p0, p1, p2, q;

    wr_u32(CLIP_INPUT, 4);
    p0 = get(vertex_at(next_word(stream)));
    p1 = get(vertex_at(next_word(stream)));
    p2 = get(vertex_at(next_word(stream)));
    put(in, p0);
    put(in + 6, p1);
    put(in + 12, p2);
    q = minus(p2, minus(p1, p0));
    if ((int16_t)((uint16_t)p0.z & (uint16_t)p1.z & (uint16_t)p2.z & (uint16_t)q.z) < 0) return -1;
    put(in + 18, q);
    return tested_face_tail(stream, frame);
}

int draw_indexed_face_list(gaddr *stream, gaddr frame) {
    gaddr base = rd_u32(*stream);

    *stream += 4;
    wr_u16(CLIP_INPUT, 0);
    wr_u16(frame - 0x7E, 0);
    for (;;) {
        int16_t at = next_word(stream);
        gaddr face;
        uint16_t kind, colour;
        int tested;

        if (at < 0) return (int16_t)rd_u16(frame - 0x7E);
        face = base + (gaddr)(int32_t)at;
        if (build_face_vertices(&face)) continue;
        kind = (uint16_t)next_word(&face);
        if (!face_colour(kind, &face, stream, frame, &colour, &tested)) continue;
        wr_u16(CURRENT_COLOUR, colour);
        wr_u16(frame - 0x7E, (uint16_t)(rd_u16(frame - 0x7E) | (uint16_t)clip_and_draw_polygon()));
    }
}
