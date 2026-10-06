/* Glue for render_state.c, view.c, stages.c and small additions. */
#include "glue.h"
#include "ports_glue.h"

#include "audio.h"
#include "control_records.h"
#include "globals.h"
#include "memory.h"
#include "render_line.h"
#include "render_state.h"
#include "stages.h"
#include "view.h"

int glue_empty_stage(void) {
    empty_stage();
    return glue_return();
}

int glue_C25864(void) {
    reset_list();
    return glue_return();
}

int glue_C2F490(void) {
    reset_line_style();
    return glue_return();
}

/* $C4FFB4: D0 channel; the original leaves D0 * 4 and the voice in A0. */
int glue_C4FFB4(void) {
    int16_t channel = (int16_t)D(0);
    clear_voice_interrupt(channel);
    D(0) <<= 2;
    A(0) = rd_u32(VOICE_TABLE + (gaddr)(int32_t)(int16_t)D(0));
    return glue_return();
}

/* $C2F596: A1 block list; clobbers D0-D7/A0-A5, none of which are live. */
int glue_C2F596(void) {
    clear_renderer_blocks(A(1));
    return glue_return();
}

int glue_C08324(void) {
    set_zoom_maximum();
    return glue_return();
}

int glue_C095C0(void) {
    reset_player_record();
    A(1) = CONTROL_RECORDS;
    return glue_return();
}

/* $C1D722: D0.w value, D1.w stride, A1 destination (advanced). */
int glue_C1D722(void) {
    gaddr dest = A(1);
    fill_column(&dest, (uint16_t)D(0), (int16_t)D(1));
    A(1) = dest;
    flags_logic_w(D(0)); /* final MOVE.W D0,(A1) */
    return glue_return();
}

/* $C30F56: D2.w BLTCON0, D0 A, D3 B, D4 C/D, D6.w BLTSIZE. */
int glue_C30F56(void) {
    start_blit((uint16_t)D(2), D(0), D(3), D(4), (uint16_t)D(6));
    return glue_return();
}

/* $C25482: A0 timer byte. */
int glue_C25482(void) {
    tick_timer(A(0));
    return glue_return();
}
