/* Glue for the voice program, record helpers, blit restarts, post-input
 * stages, rounded division and the rotation matrix. */
#include "glue.h"
#include "ports_glue.h"

#include "audio.h"
#include "control_records.h"
#include "fixed_math.h"
#include "globals.h"
#include "memory.h"
#include "post_input.h"
#include "render_state.h"
#include "stages.h"

/* $C50212: A3 voice, A2 slot, D3 channel; no live register outputs. */
int glue_C50212(void) {
    step_voice_program(A(3), A(2), (int16_t)D(3));
    return glue_return();
}

/* $C4FFB0: compiled C, clear_voice_interrupt(int channel) with the
 * argument on the stack; leaves channel * 4 in D0 and the voice in A0. */
int glue_C4FFB0(void) {
    int16_t channel = (int16_t)rd_u32(A(7) + 4);
    clear_voice_interrupt(channel);
    D(0) = rd_u32(A(7) + 4) << 2;
    A(0) = rd_u32(VOICE_TABLE + (gaddr)(int32_t)(int16_t)D(0));
    return glue_return();
}

/* $C14876: compiled C, argument word at 6(A7). */
int glue_C14876(void) {
    ease_record_26(rd_s16(A(7) + 6));
    return glue_return();
}

int glue_C28F16(void) {
    set_record_view(A(0), (int16_t)D(2), (int16_t)D(3), (int16_t)D(4), (int16_t)D(5), D(6));
    return glue_return();
}

/* $C0FA4C: leaves the countdown in D0.w, or when expired the two viewport
 * mode bytes in D0.b/D1.b, and the scheduled routine in A0. */
int glue_C0FA4C(void) {
    int16_t countdown = rd_s16(POST_INPUT_COUNTDOWN);
    await_viewport_match();
    SET_W(D(0), countdown);
    if (countdown < 0) {
        SET_B(D(0), rd_u8(VIEWPORT_MODE));
        SET_B(D(1), rd_u8(VIEWPORT_TARGET));
        if (rd_u8(VIEWPORT_MODE) == rd_u8(VIEWPORT_TARGET)) A(0) = ROUTINE_COMPLETE_POST_INPUT;
    }
    return glue_return();
}

/* $C0FA80: leaves the countdown in D0.w and the scheduled routine in A0. */
int glue_C0FA80(void) {
    int16_t countdown = rd_s16(POST_INPUT_COUNTDOWN);
    complete_post_input();
    SET_W(D(0), countdown);
    if (countdown < 0) A(0) = ROUTINE_AFTER_POST_INPUT;
    return glue_return();
}

/* $C1FE20: MOVEQ #0,D0. */
int glue_C1FE20(void) {
    D(0) = (uint32_t)zero_result();
    flags_logic_l(D(0));
    return glue_return();
}

/* $C21960: skips a stream word (A2) and returns 0. */
int glue_C21960(void) {
    A(2) += 2;
    D(0) = (uint32_t)zero_result();
    flags_logic_l(D(0));
    return glue_return();
}

/* $C25980: saves and restores D0-D2. */
int glue_C25980(void) {
    divide_rounded();
    return glue_return();
}

/* $C2E370: D4.w angle, A1 output (advanced). Leaves -sine in D4.w, cosine
 * in D5.w and sin_cos's other leftovers. */
int glue_C2E370(void) {
    int16_t angle = (int16_t)((int16_t)D(4) >> 3), off = (int16_t)(angle * 2);
    Fixed14 s, c;

    y_rotation_matrix((int16_t)D(4), A(1));

    sin_cos(angle, &s, &c);
    SET_W(D(4), -s);
    SET_W(D(5), c);
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
    A(1) += 18;
    return glue_return();
}
