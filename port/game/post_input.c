/* Post-input stage sequence (STAGE_CALLBACK chain). */
#include "post_input.h"
#include "menu_context_finish.h"
#include "postflight_scheduler.h"
#include "menu_followup.h"
#include "menu_outcome.h"
#include "menu_return.h"

#include "audio.h"
#include "stages.h"

#include "globals.h"
#include "memory.h"

void await_viewport_match(void) {
    if (rd_s16(POST_INPUT_COUNTDOWN) >= 0) return;
    wr_u8(POST_INPUT_AUX, 0);
    if (rd_u8(VIEWPORT_MODE) != rd_u8(VIEWPORT_TARGET)) return;
    wr_u16(POST_INPUT_COUNTDOWN, 2);
    wr_u32(STAGE_CALLBACK, ROUTINE_COMPLETE_POST_INPUT);
}

void complete_post_input(void) {
    if (rd_s16(POST_INPUT_COUNTDOWN) >= 0) return;
    wr_u8(POST_INPUT_EVENT, 0);
    wr_u8(POST_INPUT_AUX, 1);
    wr_u32(STAGE_CALLBACK, ROUTINE_AFTER_POST_INPUT);
}

void start_context_stage(void) {
    begin_menu_context(NULL);
}

void check_post_input_expiry(void) {
    expire_menu_context(NULL);
}

/* The stages below run from STAGE_CALLBACK once per update; most wait for
 * POST_INPUT_COUNTDOWN to expire (go negative) and then install the next. */

static int countdown_expired(void) { return rd_s16(POST_INPUT_COUNTDOWN) < 0; }
static void next_stage(gaddr routine) { wr_u32(STAGE_CALLBACK, routine); }

void await_viewport_then_ready(void) {
    if (!countdown_expired() || rd_u8(VIEWPORT_MODE) != rd_u8(VIEWPORT_TARGET)) return;
    wr_u16(POST_INPUT_COUNTDOWN, 2);
    next_stage(ROUTINE_VIEWPORT_READY);
}

void mark_viewport_ready(void) {
    if (!countdown_expired()) return;
    wr_u8(POST_INPUT_AUX, 1);
    next_stage(ROUTINE_AFTER_VIEWPORT);
}

void choose_after_countdown(void) {
    choose_menu_exit_after_countdown(NULL);
}

void leave_on_key_or_message(void) {
    leave_menu_on_key_or_message(NULL);
}

void reset_viewport_after_countdown(void) {
    reset_menu_viewport_after_countdown(NULL);
}

void enter_mode_four_when_ready(void) {
    enter_menu_mode_four(NULL);
}

void queue_message_four(void) {
    queue_menu_message_four(NULL);
}

void start_outcome_countdown(void) {
    start_menu_outcome(NULL);
}

void expire_to_fire_state(void) {
    if (!countdown_expired()) return;
    wr_u16(COCKPIT_FLAGS, (uint16_t)(rd_u16(COCKPIT_FLAGS) | 0x40));
    wr_u8(FIRE_STATE, 0xFE);
    next_stage(STAGE_AFTER_EXPIRY);
}

void end_on_message(void) {
    if ((int8_t)rd_u8(MESSAGE_STATE_C) < 0) next_stage(ROUTINE_END_SEQUENCE);
}

void follow_message_or_phase(void) {
    if ((int8_t)rd_u8(MESSAGE_STATE_C) < 0) {
        wr_u16(COCKPIT_FLAGS, (uint16_t)(rd_u16(COCKPIT_FLAGS) & 0xFFBF));
        wr_u16(COCKPIT_FLAGS, (uint16_t)(rd_u16(COCKPIT_FLAGS) & 0x9FFF));
        wr_u16(POST_INPUT_COUNTDOWN, 0x64);
        next_stage(ROUTINE_RESTART_SEQUENCE);
    } else if (rd_u8(SEQUENCE_PHASE) == 0xFF) {
        wr_u8(SEQUENCE_PHASE, 0);
        wr_u8(SEQUENCE_FLAG, 0);
        next_stage(ROUTINE_END_SEQUENCE);
    } else if (rd_u8(SEQUENCE_PHASE) == 1) {
        wr_u16(POST_INPUT_COUNTDOWN, 0xFFFF);
        next_stage(ROUTINE_RESTART_SEQUENCE);
    }
}

void restart_after_countdown(void) {
    if (!countdown_expired()) return;
    wr_u8(SEQUENCE_PHASE, 0);
    wr_u8(PLAYER_PHASE, 0);
    if (rd_u8(CONTEXT_REQUEST)) wr_u8(SEQUENCE_FLAG, 1);
    wr_u8(POST_INPUT_EVENT, 1);
    wr_u16(POST_INPUT_COUNTDOWN, 3);
    wr_u8(POST_INPUT_AUX, 0);
    wr_u8(VIEWPORT_TARGET, 0);
    next_stage(ROUTINE_AWAIT_VIEWPORT);
}

void begin_phase_three(void) {
    schedule_postflight(POSTFLIGHT_MODE_NINE,0,0,NULL);
}

void raise_event_after_countdown(void) {
    if (!countdown_expired()) return;
    wr_u8(POST_INPUT_EVENT, 1);
    wr_u16(POST_INPUT_COUNTDOWN, 2);
    next_stage(ROUTINE_AFTER_EVENT);
    wr_u8(CONTEXT_GATE, 1);
}

void queue_mode_messages(void) {
    advance_menu_mode_messages(NULL);
}
