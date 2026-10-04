/* Glue for fixed_math.c, audio.c and text.c. */
#include "glue.h"
#include "ports_glue.h"

#include "audio.h"
#include "fixed_math.h"
#include "globals.h"
#include "memory.h"
#include "player_input.h"
#include "numbers.h"
#include "render_page.h"
#include "text.h"

/* $C2E6DA: D4.w angle -> D4.w sine, D5.w cosine. The original leaves its
 * doubled table offsets in D6.w/D7.w and the table in A0; callers see all. */
int glue_C2E6DA(void) {
    int16_t angle = (int16_t)D(4), off = (int16_t)(angle * 2);
    Fixed14 sine, cosine;

    sin_cos(angle, &sine, &cosine);

    SET_W(D(4), sine);
    SET_W(D(5), cosine);
    SET_W(D(7), off);
    if (off < 0x708) {
        SET_W(D(6), 0x708 - off);
    } else if (off < 0xE10) {
        SET_W(D(6), 0x708);
        SET_W(D(7), off - 0x708);
    } else if (off < 0x1518) {
        SET_W(D(6), 0x1518 - off);
    } else {
        SET_W(D(6), 0x1518);
        SET_W(D(7), off - 0x1518);
    }
    A(0) = SINE_TABLE;
    return glue_return();
}

/* $C501E0: A0 channel registers, A3 voice record. Only D0 is scratch. */
int glue_C501E0(void) {
    set_voice_output(A(0), A(3)); /* D0 (scratch) is dead at its call sites */
    return glue_return();
}

/* $C24FE8: no inputs. While fading, the original leaves the target in D0,
 * the level in D1, and X from its ADDI.L/SUBI.L quarter step. */
int glue_C24FE8(void) {
    uint32_t level = rd_u32(MASTER_VOLUME), target = rd_u32(MASTER_VOLUME_TARGET);
    int fading = rd_u8(VOLUME_FADING) != 0;

    fade_master_volume();

    if (fading) {
        D(0) = target;
        D(1) = rd_u32(MASTER_VOLUME);
        if ((int32_t)level > (int32_t)target)
            FLAG_X = level < 0x4000u ? XFLAG_SET : XFLAG_CLEAR; /* borrow of SUBI.L */
        else if (level != target)
            FLAG_X = level > 0xFFFFFFFFu - 0x4000u ? XFLAG_SET : XFLAG_CLEAR; /* carry of ADDI.L */
    }
    return glue_return();
}

/* Shared register epilogue of the two glyph plotters: D2 high word = shift,
 * D2/D3 low words = the caller's D3, D3 high word = a plane longword of the
 * last row (`last`), A0/A3 advanced past the glyph. */
static void glyph_epilogue(uint32_t d3_in, int shift, int rows, gaddr glyph, gaddr dest, uint32_t last) {
    D(2) = (uint32_t)shift << 16 | (d3_in & 0xFFFF);
    D(3) = (last & 0xFFFF0000u) | (d3_in & 0xFFFF);
    A(0) = glyph + (gaddr)rows;
    A(3) = dest + (gaddr)(rows * 40);
}

static int glyph_rows(void) {
    int rows = (int)((D(7) & 0xFFFF) >> 6);
    return rows ? rows : 65536;
}

/* $C330FE: D4 glyph, D1 plane longword, D2.w bits 12-15 shift and bits 4-7
 * draw/clear, D7.w rows << 6. D0/D6 are saved and restored. */
int glue_C330FE(void) {
    uint32_t d3_in = D(3);
    int shift = (int)((D(2) >> 12) & 15), rows = glyph_rows();
    gaddr glyph = D(4), dest = D(1);

    plot_glyph8(glyph, dest, shift, rows, (D(2) & 0xF0) != 0);

    /* D3 holds the last row as written. */
    glyph_epilogue(d3_in, shift, rows, glyph, dest, rd_u32(dest + (gaddr)((rows - 1) * 40)));
    flags_logic_l(D(0)); /* final MOVE.L (A7)+,D0 restores D0 and sets its flags */
    return glue_return();
}

/* $C32806: as $C330FE with a 3-pixel cell; D2.w bits 4-7 select clear (0),
 * inverse (bits 4-5 only) or draw (bits 6-7). */


/* $C25A08: no register inputs; saves and restores everything it uses. */
int glue_C25A08(void) {
    pack_display_value();
    return glue_return();
}

/* $C1715C: returns the buttons in D0 (long). */
int glue_C1715C(void) {
    D(0) = (uint32_t)read_mouse_buttons();
    return glue_return();
}

/* $C2F558: leaves the selected tables in A0/A1. */
int glue_C2F558(void) {
    select_draw_page();
    A(0) = rd_u32(PAGE_PLANE_TABLE);
    A(1) = rd_u32(PAGE_POINTER_TABLE);
    return glue_return();
}
