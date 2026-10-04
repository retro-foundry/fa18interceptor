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
