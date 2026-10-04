/* Legacy C21060 quad-list adapter; complete face-stream owners are separate. */
#include "glue.h"
#include "ports_glue.h"

#include "draw_stream.h"
#include "globals.h"
#include "machine.h"
#include "memory.h"
#include "glue_clip.h"

#define SEXT(v) ((uint32_t)(int32_t)(int16_t)(v))
#define W(n) ((int16_t)D(n))



/* ---- $C2159E ------------------------------------------------------------- */



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
