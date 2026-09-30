/* Glue for the face commands $C2159E, $C21060 and $C210E6. Their callers
 * read every register. The C runs first; then the register flow is
 * replayed from the stream and the vertex table: the setup of each face,
 * and the clipper's registers for the last face that reached it (a later
 * clipper call overwrites what an earlier one left). The clipper's own
 * state needs no snapshot; its draw needs the BLTSIZE from just before
 * that last draw began, which the machine keeps (fa18_bltsize_at_draw_start). */
#include "glue.h"
#include "ports_glue.h"

#include "draw_stream.h"
#include "globals.h"
#include "machine.h"
#include "memory.h"
#include "glue_clip.h"

#define SEXT(v) ((uint32_t)(int32_t)(int16_t)(v))
#define W(n) ((int16_t)D(n))

static void last_clipper_call(uint16_t colour, int drawn) {
    ClipperSnapshot snapshot;
    clipper_snapshot(&snapshot);
    snapshot.last_size = fa18_bltsize_at_draw_start;
    clipper_registers(&snapshot, colour, drawn);
}

/* ---- $C2159E ------------------------------------------------------------- */

int glue_C2159E(void) {
    gaddr stream = A(2), bound = rd_u32(BOUND_RECORD), v;
    uint16_t colour = rd_u16(A(2));
    uint32_t a1 = A(1), a2, a5 = A(5);
    int16_t a = rd_s16(A(2) + 2), b = rd_s16(A(2) + 4), first = rd_s16(A(2) + 6), second = rd_s16(A(2) + 8);
    int16_t ax, az, grid = rd_s16(BOUND_SHIFT);
    int drawn, first_path, flagged = b < 0;

    drawn = draw_side_face(&stream);
    A(2) += 10;
    a2 = A(2);
    A(4) = bound;
    ax = rd_s16(bound + 0xA + SEXT((uint16_t)a));
    az = rd_s16(bound + 0xE + SEXT((uint16_t)a));
    SET_W(D(6), (uint16_t)b);
    b &= 0x7FFF;
    SET_W(D(1), (uint16_t)(ax - rd_s16(bound + 0xA + SEXT((uint16_t)b))));
    SET_W(D(2), (uint16_t)(az - rd_s16(bound + 0xE + SEXT((uint16_t)b))));
    SET_W(D(3), (uint16_t)ax);
    SET_W(D(4), (uint16_t)az);
    SET_W(D(7), rd_u8(bound + 6) & 15);
    SET_W(D(3), (uint16_t)(W(3) >> W(7)));
    SET_W(D(4), (uint16_t)(W(4) >> W(7)));
    SET_W(D(3), (uint16_t)(W(3) + rd_s16(BOUND_OFFSET_X)));
    SET_W(D(4), (uint16_t)(W(4) + rd_s16(BOUND_OFFSET_Z)));
    SET_W(D(7), (uint16_t)grid);
    {
        int count = grid & 63;
        SET_W(D(3), count >= 16 ? 0 : (uint16_t)((uint16_t)W(3) << count));
        SET_W(D(4), count >= 16 ? 0 : (uint16_t)((uint16_t)W(4) << count));
    }
    SET_W(D(3), (uint16_t)(W(3) + rd_s16(PROJECTION_WORDS)));
    SET_W(D(4), (uint16_t)(W(4) + rd_s16(PROJECTION_WORDS + 4)));
    D(3) = (uint32_t)((int32_t)W(3) * W(1));
    D(4) = (uint32_t)((int32_t)W(4) * W(2));
    first_path = ((int64_t)(int32_t)D(3) + (int32_t)D(4) >= 0) != flagged;
    D(4) += D(3);
    SET_W(D(0), (uint16_t)first);
    v = WORKSPACES + SEXT((uint16_t)second);
    A(3) = v;
    if (first_path) {
        A(0) = CLIP_INPUT + 20;
        SET_W(D(7), rd_u16(CLIP_INPUT + 8) & rd_u16(CLIP_INPUT + 14) & rd_u16(CLIP_INPUT + 20));
        if (W(7) < 0) goto behind;
    } else {
        int k;
        for (k = 0; k < 6; k++) D(2 + k) = SEXT(rd_u16(v + (gaddr)(2 * k)));
        A(3) = v + 12;
        for (k = 0; k < 3; k++) SET_W(D(5 + k), (uint16_t)(W(5 + k) - W(2 + k)));
        for (k = 0; k < 3; k++) D(2 + k) = SEXT(rd_u16(A(3) + (gaddr)(2 * k)));
        for (k = 0; k < 3; k++) SET_W(D(2 + k), (uint16_t)(W(2 + k) - W(5 + k)));
        A(0) = CLIP_INPUT + 16;
        SET_W(D(4), (uint16_t)(W(4) & rd_s16(CLIP_INPUT + 8) & rd_s16(CLIP_INPUT + 14)));
        if (W(4) < 0) goto behind;
    }
    last_clipper_call(colour, drawn);
    A(1) = a1;
    A(2) = a2;
    A(5) = a5;
    return glue_return();
behind:
    D(0) = 0;
    flags_logic_l(0);
    return glue_return();
}

/* ---- $C21060 ------------------------------------------------------------- */

/* One quad's vertices into the clipper input, as the C wrote them: every
 * call to the clipper is replayed, and each needs its own quad there. */
static void write_quad(gaddr offsets) {
    gaddr in = CLIP_INPUT + 4;
    int k;
    for (k = 0; k < 4; k++, in += 6) {
        gaddr v = WORKSPACES + SEXT(rd_u16(offsets + (gaddr)(2 * k)));
        wr_u32(in, rd_u32(v));
        wr_u16(in + 4, rd_u16(v + 4));
    }
}

int glue_C21060(void) {
    gaddr stream = A(2), quad = 0;
    uint32_t a1 = A(1), a5 = A(5);
    uint16_t result = 0;
    ClipperSnapshot snap;
    int i, k;

    /* Taken before the C, and carried from quad to quad: each call replays
     * from the clip state the one before it left. */
    clipper_snapshot(&snap);
    result = (uint16_t)draw_quad_list(&stream);
    SET_W(D(0), 8);
    SET_W(D(1), 8);
    SET_W(D(2), 0);
    SET_W(D(3), 0);
    for (i = 0;; i++) {
        SET_W(D(1), rd_u16(A(2)));
        A(2) += 2;
        if (W(1) < 0) break;
        for (k = 0; k < 3; k++) D(2 + k) = SEXT(rd_u16(A(2) + (gaddr)(2 * k)));
        A(2) += 6;
        A(0) = CLIP_INPUT + 4 + 24;
        A(3) = WORKSPACES;
        A(4) = WORKSPACES + SEXT(D(4)) + 4;
        SET_W(D(6), rd_u16(WORKSPACES + SEXT(D(1)) + 4) & rd_u16(WORKSPACES + SEXT(D(2)) + 4) &
                        rd_u16(WORKSPACES + SEXT(D(3)) + 4) & rd_u16(WORKSPACES + SEXT(D(4)) + 4));
        quad = A(2) - 8;
        if (W(6) < 0) continue;
        {
            uint32_t a2 = A(2);
            /* D0 is ORed into the result whatever the replay leaves it as;
             * D7's high word carries from one call to the next, so every
             * call is replayed, not just the last. */
            write_quad(quad);
            snap.last_size = fa18_bltsize_at_draw_start;
            clipper_registers(&snap, 12, -1);
            A(2) = a2;
        }
    }
    if (quad) write_quad(quad);
    wr_u16(A(6) - 0x7E, result);
    A(1) = a1;
    A(5) = a5;
    SET_W(D(0), result);
    flags_logic_w(D(0));
    return glue_return();
}

/* ---- $C210E6 ------------------------------------------------------------- */

int glue_C210E6(void) {
    gaddr stream = A(2), q;
    uint32_t a1 = A(1), a2, a5 = A(5);
    int16_t count = rd_s16(A(2) + 2), a[3], b[3], n;
    uint16_t result;
    int k;

    result = (uint16_t)draw_quad_strip(&stream);
    q = WORKSPACES + SEXT(rd_u16(A(2))) + 18;
    A(2) += 4;
    a2 = A(2);
    for (k = 0; k < 3; k++) {
        int16_t p0 = rd_s16(q - 18 + (gaddr)(2 * k)), p1 = rd_s16(q - 12 + (gaddr)(2 * k)), p3 = rd_s16(q + (gaddr)(2 * k));
        a[k] = (int16_t)((int16_t)((int16_t)(p1 - p3) * 3) >> 1);
        a[k] = (int16_t)(a[k] + (int16_t)(p0 - p3));
        b[k] = (int16_t)(rd_s16(q - 6 + (gaddr)(2 * k)) - p3);
        wr_s16(A(6) - 0x46 + (gaddr)(2 * k), a[k]);
        wr_s16(A(6) - 0x52 + (gaddr)(2 * k), b[k]);
    }
    /* The frame's count is left at its end value; the result word too. */
    n = count;
    do {
        n--;
    } while (n > 0);
    wr_s16(A(6) - 0x38, n);
    q += (gaddr)(6 * (count > 1 ? count - 1 : 0));
    A(3) = q;
    A(0) = CLIP_INPUT + 4 + 12;
    for (k = 0; k < 3; k++) {
        int16_t qk = rd_s16(q + (gaddr)(2 * k));
        D(1 + k) = (SEXT((uint16_t)qk) & 0xFFFF0000u) | (uint16_t)(qk + b[k] + a[k]);
        D(4 + k) = (SEXT((uint16_t)qk) & 0xFFFF0000u) | (uint16_t)(qk + b[k]);
    }
    last_clipper_call(rd_u16(CURRENT_COLOUR), -1);
    A(3) = q;
    A(1) = a1;
    A(2) = a2;
    A(5) = a5;
    wr_u16(A(6) - 0x7E, result);
    SET_W(D(0), result);
    flags_logic_w(D(0));
    return glue_return();
}
