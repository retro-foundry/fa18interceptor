/* Glue for the face test command $C1FF0A. Its caller reads every register,
 * so the setup's own registers are rebuilt and then the test's glue is run
 * for the registers and flags it leaves; both of its paths only read. */
#include "glue.h"
#include "ports_glue.h"

#include "draw_stream.h"
#include "globals.h"
#include "memory.h"
#include "plane_tests.h"

#define SEXT(v) ((uint32_t)(int32_t)(int16_t)(v))

static void call_port(int (*glue)(void), uint32_t return_to) {
    A(7) -= 4;
    wr_u32(A(7), return_to);
    glue();
}

int glue_C1FF0A(void) {
    gaddr stream = A(2);
    uint16_t kind = rd_u16(stream + 6);
    int k;

    test_stream_face(&stream, A(6));
    for (k = 0; k < 3; k++) D(k) = SEXT(rd_u16(A(2) + (gaddr)(2 * k)));
    A(0) = CLIP_INPUT + 4 + 18;
    A(2) += 8;
    A(3) = SEXT(kind);
    SET_W(D(7), kind);
    call_port(glue_C1FB82, 0xC1FF3C);
    A(2) = stream;
    return glue_return();
}

/* ---- the indexed face commands $C2005C and $C20100 ----------------------- */

#include "glue_clip.h"
#include "machine.h"

#define W(n) ((int16_t)D(n))

static void last_clipper_call(uint16_t colour, int drawn) {
    ClipperSnapshot snapshot;
    clipper_snapshot(&snapshot);
    snapshot.last_size = fa18_bltsize_at_draw_start;
    clipper_registers(&snapshot, colour, drawn);
}

/* One vertex of a face list: A4 at its z word, A0 past it in the clipper
 * input, D6 the z words so far ANDed together. */
static void vertex_registers(int16_t offset, int first) {
    gaddr v = WORKSPACES + SEXT(offset);
    A(4) = v + 4;
    A(0) += 6;
    SET_W(D(6), first ? rd_u16(v + 4) : (uint16_t)(W(6) & rd_u16(v + 4)));
}

/* The register flow of one face's vertex list, advancing `*face` past it. */
static void build_registers(gaddr *face) {
    int16_t offset;
    int n;

    A(0) = CLIP_INPUT + 4;
    A(3) = WORKSPACES;
    D(7) = 3; /* MOVEQ: the whole register */
    for (n = 0;; n++) {
        offset = rd_s16(*face);
        *face += 2;
        SET_W(D(1), (uint16_t)offset);
        if (n >= 2) {
            if (offset < 0) break;
            vertex_registers(offset, 0);
            SET_W(D(7), W(7) + 1);
        } else {
            vertex_registers(offset, n == 0);
        }
    }
    SET_W(D(1), (uint16_t)(offset & 0x7FFF));
    vertex_registers((int16_t)W(1), 0);
}

/* The vertices of the face at `face` into the clipper input, as the C
 * wrote them: the glue rebuilds them for the face the clipper last saw. */
static void write_face_vertices(gaddr face) {
    gaddr in = CLIP_INPUT + 4, v;
    int16_t offset, count = 3;
    int n, last;

    for (n = 0;; n++) {
        offset = rd_s16(face);
        face += 2;
        last = 0;
        if (n >= 2) {
            if (offset < 0) { offset = (int16_t)(offset & 0x7FFF); last = 1; }
            else count++;
        }
        v = WORKSPACES + SEXT(offset);
        wr_u32(in, rd_u32(v));
        wr_u16(in + 4, rd_u16(v + 4));
        in += 6;
        if (last) break;
    }
    wr_u16(CLIP_INPUT + 2, (uint16_t)count);
}

/* The kind word's registers, and the face test when it runs: 1 when the
 * face goes on to the clipper. `stream` is what the test reads and
 * advances (A2); `skip_alternate` is $C2005C's step past the alternate
 * colour word when the test fails. */
static int kind_registers(gaddr *face, gaddr *stream, uint32_t after_test, int skip_alternate) {
    uint16_t kind = rd_u16(*face);

    *face += 2;
    A(3) = SEXT(kind);
    SET_W(D(7), kind);
    if ((int16_t)kind < 0) return 1;
    A(2) = *stream;
    call_port(glue_C1FB82, after_test);
    *stream = A(2);
    if (FLAG_Z) { /* the test passed */
        SET_W(D(1), (uint16_t)(kind & 0x4000));
        flags_logic_w(D(1));
        if (!(kind & 0x4000)) return 0;
        SET_W(D(7), rd_u16(*face));
        *face += 2;
        return 1;
    }
    SET_W(D(1), kind);
    SET_W(D(7), kind);
    if (skip_alternate) {
        SET_W(D(1), (uint16_t)(kind & 0x4000));
        flags_logic_w(D(1));
        if (kind & 0x4000) *stream += 2;
    }
    return 1;
}

static int dropped(gaddr stream) {
    A(2) = stream;
    D(0) = 0xFFFFFFFFu;
    flags_logic_l(D(0));
    return glue_return();
}

int glue_C2005C(void) {
    gaddr stream = A(2), p = A(2);
    uint32_t a1 = A(1), a5 = A(5);
    int drawn = draw_tested_face(&stream, A(6));
    int reached;

    build_registers(&p);
    reached = W(6) >= 0 && kind_registers(&p, &p, 0xC200BE, 1);
    if (reached) last_clipper_call(rd_u16(CURRENT_COLOUR), drawn);
    A(1) = a1;
    A(5) = a5;
    if (!reached) return dropped(stream);
    A(2) = stream;
    return glue_return();
}

/* Whether the face at `face` reaches the clipper, without drawing it:
 * every vertex behind, or the test dropping it, keeps it away. `stream`
 * advances as the test advances it. */
static int face_reaches_clipper(gaddr face, gaddr *stream, gaddr frame) {
    uint16_t behind = 0xFFFF, kind;
    int16_t offset, eye[3];
    gaddr faces;
    int n, k;

    for (n = 0;; n++) {
        int last = 0;
        offset = rd_s16(face);
        face += 2;
        if (n >= 2) {
            if (offset < 0) { offset = (int16_t)(offset & 0x7FFF); last = 1; }
        }
        behind &= rd_u16(WORKSPACES + SEXT(offset) + 4);
        if (last) break;
    }
    if ((int16_t)behind < 0) return 0;
    kind = rd_u16(face);
    if ((int16_t)kind < 0) return 1;
    faces = *stream;
    for (k = 0; k < 3; k++) eye[k] = rd_s16(frame - 0x26 + (gaddr)(2 * k));
    k = face_test_passes(kind, rd_u32(frame - 0x2C), &faces, eye);
    *stream = faces;
    return k ? (kind & 0x4000) != 0 : 1;
}

/* $C20100: the same face, once per offset from a base pointer. Only the
 * last face the clipper saw has its registers rebuilt, so its vertices go
 * back into the clipper input for that, and the C's own contents are put
 * back afterwards. */
int glue_C20100(void) {
    gaddr stream = A(2), p = A(2), scan, base = rd_u32(A(2)), built = 0, last_face = 0;
    uint32_t a1 = A(1), a5 = A(5);
    uint16_t count;
    int i, last = -1;

    draw_indexed_face_list(&stream, A(6));
    count = rd_u16(CLIP_INPUT + 2);

    /* Which face the clipper saw last. */
    scan = p + 4;
    for (i = 0;; i++) {
        int16_t at = rd_s16(scan);
        scan += 2;
        if (at < 0) break;
        if (face_reaches_clipper(base + SEXT(at), &scan, A(6))) { last = i; last_face = base + SEXT(at); }
    }

    p += 4;
    A(4) = base;
    for (i = 0;; i++) {
        int16_t at = rd_s16(p);
        gaddr face;
        p += 2;
        SET_W(D(1), (uint16_t)at);
        if (at < 0) break;
        built = base + SEXT(at);
        face = built;
        A(4) = built;
        build_registers(&face);
        if (W(6) >= 0 && kind_registers(&face, &p, 0xC20178, 0) && i == last) {
            write_face_vertices(last_face);
            last_clipper_call(rd_u16(CURRENT_COLOUR), -1);
        }
        A(4) = base;
    }
    if (last >= 0) {
        if (built != last_face) write_face_vertices(built);
        wr_u16(CLIP_INPUT + 2, count);
    }
    A(1) = a1;
    A(2) = stream;
    A(5) = a5;
    SET_W(D(0), rd_u16(A(6) - 0x7E));
    flags_logic_w(D(0));
    return glue_return();
}
