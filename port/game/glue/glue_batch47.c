/* Glue for the post-input stages, the throttle and stick setters, the
 * flight recorder and small helpers. The stages' caller ($C0F808, through
 * STAGE_CALLBACK) reads every register: what each stage last loaded into D0
 * (MOVE.B and MOVE.W keep the upper bits), and A0 as its LEA of the next
 * stage leaves it (reset_message_sequence leaves its own first). */
#include "glue.h"
#include "ports_glue.h"

#include "control_records.h"
#include "faces.h"
#include "flight_recorder.h"
#include "globals.h"
#include "memory.h"
#include "messages.h"
#include "player_input.h"
#include "post_input.h"
#include "stages.h"

#define SEXT(v) ((uint32_t)(int32_t)(int16_t)(v))

/* $C11312's registers (glue_batch10.c). */
static void reset_regs(void) {
    D(0) = 0;
    A(0) = MESSAGE_QUEUE + 4;
}

static int countdown_regs(void) {
    SET_W(D(0), rd_u16(POST_INPUT_COUNTDOWN));
    return rd_s16(POST_INPUT_COUNTDOWN) < 0;
}

int glue_C0F946(void) {
    if (countdown_regs()) {
        SET_B(D(0), rd_u8(VIEWPORT_MODE));
        SET_B(D(1), rd_u8(VIEWPORT_TARGET));
        if ((uint8_t)D(0) == (uint8_t)D(1)) A(0) = ROUTINE_VIEWPORT_READY;
    }
    await_viewport_then_ready();
    return glue_return();
}

int glue_C0F974(void) {
    if (countdown_regs()) A(0) = ROUTINE_AFTER_VIEWPORT;
    mark_viewport_ready();
    return glue_return();
}

int glue_C0FB70(void) {
    if (countdown_regs()) {
        SET_B(D(0), rd_u8(SEQUENCE_FLAG));
        if (!(uint8_t)D(0)) A(0) = ROUTINE_LEAVE_ON_KEY;
        else { reset_regs(); A(0) = ROUTINE_AFTER_POST_INPUT; }
    }
    choose_after_countdown();
    return glue_return();
}

int glue_C0FBB6(void) {
    SET_B(D(0), rd_u8(MESSAGE_STATE_C));
    if ((int8_t)D(0) < 0 || rd_u8(KEY_TAKEN)) { reset_regs(); A(0) = ROUTINE_AFTER_POST_INPUT; }
    leave_on_key_or_message();
    return glue_return();
}

int glue_C101FC(void) {
    if (countdown_regs()) { D(0) = 0; A(0) = ROUTINE_ENTER_MODE_FOUR; }
    reset_viewport_after_countdown();
    return glue_return();
}

int glue_C10228(void) {
    SET_B(D(0), rd_u8(VIEWPORT_MODE));
    SET_B(D(1), rd_u8(VIEWPORT_TARGET));
    if ((uint8_t)D(0) == (uint8_t)D(1)) { D(0) = 1; A(0) = ROUTINE_MODE_FOUR; }
    enter_mode_four_when_ready();
    return glue_return();
}

int glue_C1072E(void) {
    SET_B(D(0), (uint8_t)(rd_u8(MESSAGE_STATE_C) - 1));
    if (!(uint8_t)D(0)) A(0) = ROUTINE_START_OUTCOME;
    queue_message_four();
    return glue_return();
}

int glue_C1075A(void) {
    if (rd_u8(CONTEXT_REQUEST)) { reset_regs(); A(0) = ROUTINE_OUTCOME; }
    start_outcome_countdown();
    return glue_return();
}

int glue_C11872(void) {
    if (countdown_regs()) SET_W(D(0), rd_u16(COCKPIT_FLAGS) | 0x40);
    expire_to_fire_state();
    return glue_return();
}

int glue_C118E6(void) {
    SET_B(D(0), rd_u8(MESSAGE_STATE_C));
    end_on_message();
    return glue_return();
}

int glue_C11958(void) {
    SET_B(D(0), rd_u8(MESSAGE_STATE_C));
    if ((int8_t)D(0) < 0) {
        SET_W(D(0), rd_u16(COCKPIT_FLAGS) & 0xFFBF & 0x9FFF);
        A(0) = ROUTINE_RESTART_SEQUENCE;
    } else {
        SET_B(D(0), rd_u8(SEQUENCE_PHASE));
        if ((uint8_t)D(0) == 0xFF) D(0) = 0;
        else {
            SET_B(D(0), (uint8_t)(rd_u8(SEQUENCE_PHASE) - 1));
            if (!(uint8_t)D(0)) A(0) = ROUTINE_RESTART_SEQUENCE;
        }
    }
    follow_message_or_phase();
    return glue_return();
}

int glue_C119D4(void) {
    if (countdown_regs()) D(0) = 0;
    restart_after_countdown();
    return glue_return();
}

int glue_C0A2F0(void) {
    gaddr player = CONTROL_RECORDS;
    if (!rd_u8(PLAYER_PHASE)) {
        A(1) = player;
        if (rd_u8(player + 1) & 0x40) {
            SET_W(D(0), rd_u16(player + 2) & 0xC080);
            if ((uint16_t)D(0) == 0xC080 && !rd_u16(player + 0x6E) && rd_u8(SEQUENCE_PHASE) != 3) D(0) = 0;
        }
    }
    begin_phase_three();
    return glue_return();
}

/* ---- throttle and stick: D1 the byte stored, D2 the field value -------- */

static int throttle_glue(uint8_t value, int release) {
    if (release) release_throttle_keys();
    else set_throttle_input(value);
    D(2) = value;
    SET_B(D(1), rd_u8(PLAYER_STICK));
    return glue_return();
}

int glue_C1B4D0(void) { return throttle_glue(THROTTLE_UP, 0); }
int glue_C1B4D4(void) { return throttle_glue(THROTTLE_DOWN, 0); }
int glue_C1B4D8(void) { return throttle_glue(THROTTLE_HOLD, 1); }
int glue_C1B4DE(void) { return throttle_glue(THROTTLE_HOLD, 0); }

static int stick_glue(uint8_t value, int y) {
    if (y) set_stick_y(value);
    else set_stick_x(value);
    D(2) = value;
    if (rd_u8(PAUSE_A) || rd_u8(CONTEXT_STARTED)) SET_B(D(1), rd_u8(PLAYER_STICK));
    return glue_return();
}

int glue_C1B50C(void) { return stick_glue(0x20, 1); }
int glue_C1B510(void) { return stick_glue(0x10, 1); }
int glue_C1B514(void) { return stick_glue(0, 1); }
int glue_C1B558(void) { return stick_glue(0x08, 0); }
int glue_C1B55C(void) { return stick_glue(0x04, 0); }
int glue_C1B560(void) { return stick_glue(0, 0); }

/* ---- the rest ------------------------------------------------------------ */

/* $C25A6A: D0/D1 as the tests and the full-buffer compare leave them, A0
 * the playback words or the byte cursor after the append. */
int glue_C25A6A(void) {
    if (rd_u8(RECORDER_ON)) {
        SET_W(D(0), rd_u16(COCKPIT_FLAGS) & 0x40);
        if ((uint16_t)D(0)) {
            uint8_t mode = rd_u8(RECORDER_MODE);
            SET_B(D(1), mode);
            if ((mode == 0 || mode == 1) && !rd_u8(POST_INPUT_EVENT)) {
                if (mode == 1) A(0) = rd_u32(PLAYBACK_WORDS);
                else {
                    D(0) = rd_u32(RECORDER_START) + rd_u32(RECORDER_SIZE);
                    A(0) = rd_u32(RECORDER_CURSOR);
                    if ((int32_t)D(0) > (int32_t)A(0)) A(0) += 1;
                }
            }
        }
    }
    record_flight_input();
    return glue_return();
}

/* $C33DA4: D5 = the tested bits. */
int glue_C33DA4(void) {
    D(5) = rd_u32(WARNING_CAUSES) & 0x4200;
    take_warning_events();
    return glue_return();
}

/* $C13C0A: ease_record_58(short) from the long at 4(A7), whose low word is
 * replaced by the target used; D0/D1/A0 as nudge_outside_dead_zone leaves
 * them. */
int glue_C13C0A(void) {
    gaddr value = rd_u32(CURRENT_RECORD) + 0x58;
    int16_t old = rd_s16(value), target = ease_record_58(rd_s16(A(7) + 6));
    int16_t eased = (int16_t)(old - (int16_t)((int16_t)(old - target) >> 2)); /* before the nudge */
    wr_s16(A(7) + 6, target);
    SET_W(D(0), rd_u16(value));
    SET_W(D(1), eased);
    A(0) = value;
    return glue_return();
}

/* $C21C4C: A3 points; D0 = 0 (MOVEQ). */
int glue_C21C4C(void) {
    split_edge(A(3));
    D(0) = 0;
    flags_logic_l(0);
    return glue_return();
}

/* $C1FED4: A2 stream; D0 = 0 with MOVEQ's flags. */
int glue_C1FED4(void) {
    A(2) = skip_word_for_mode_57(A(2));
    D(0) = 0;
    flags_logic_l(0);
    return glue_return();
}
