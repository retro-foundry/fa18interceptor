/* Post-input stage sequence (STAGE_CALLBACK chain). */
#include "post_input.h"
#include "postflight_scheduler.h"

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
    if (!rd_u8(CONTEXT_SELECT)) {
        wr_u8(CONTEXT_STARTED, 1);
        wr_u8(CONTEXT_STATE, 0);
        wr_u8(CONTEXT_GATE, 0);
        wr_u8(CONTEXT_AUX, 0);
        wr_u16(POST_INPUT_COUNTDOWN, 5);
        wr_u32(STAGE_CALLBACK, ROUTINE_CONTEXT_STAGE);
    } else if ((int8_t)rd_u8(POST_INPUT_EVENT) < 0) {
        wr_u16(POST_INPUT_COUNTDOWN, 2);
        wr_u8(VIEWPORT_TARGET, 10);
        wr_u32(STAGE_CALLBACK, ROUTINE_VIEWPORT_CHANGE);
    }
}

void check_post_input_expiry(void) {
    if (rd_s16(POST_INPUT_COUNTDOWN) >= 0) return;
    wr_u8(POST_INPUT_EXPIRED, 1);
    play_tone_2();
    wr_u32(STAGE_CALLBACK, STAGE_AFTER_EXPIRY);
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
    if (!countdown_expired()) return;
    if (!rd_u8(SEQUENCE_FLAG)) {
        wr_u8(POST_INPUT_AUX, 0);
        wr_u16(MESSAGE_QUEUE, 0x49);
        next_stage(ROUTINE_LEAVE_ON_KEY);
    } else {
        wr_u8(POST_INPUT_AUX, 1);
        reset_message_sequence();
        next_stage(ROUTINE_AFTER_POST_INPUT);
    }
}

void leave_on_key_or_message(void) {
    if ((int8_t)rd_u8(MESSAGE_STATE_C) >= 0 && !rd_u8(KEY_TAKEN)) return;
    wr_u8(POST_INPUT_AUX, 1);
    reset_message_sequence();
    next_stage(ROUTINE_AFTER_POST_INPUT);
}

void reset_viewport_after_countdown(void) {
    if (!countdown_expired()) return;
    wr_u8(POST_INPUT_AUX, 0);
    wr_u8(VIEWPORT_TARGET, 0x0F);
    wr_u8(VIEWPORT_MODE, 0);
    next_stage(ROUTINE_ENTER_MODE_FOUR);
}

void enter_mode_four_when_ready(void) {
    if (rd_u8(VIEWPORT_MODE) != rd_u8(VIEWPORT_TARGET)) return;
    wr_u16(POST_INPUT_COUNTDOWN, 2);
    wr_u8(POST_INPUT_AUX, 1);
    wr_u8(UPDATE_MASK, 0xFF);
    wr_u8(CONTEXT_STATE, 4);
    wr_u8(CONTEXT_GATE, 1);
    wr_u16(POST_INPUT_COUNTDOWN, 5);
    next_stage(ROUTINE_MODE_FOUR);
}

void queue_message_four(void) {
    if (rd_u8(MESSAGE_STATE_C) != 1) return;
    wr_u16(MESSAGE_QUEUE, 4);
    wr_u8(MESSAGE_STATE_B, 0);
    wr_u8(MESSAGE_STATE_C, 3);
    next_stage(ROUTINE_START_OUTCOME);
}

void start_outcome_countdown(void) {
    if (!rd_u8(CONTEXT_REQUEST)) return;
    if (rd_u8(CONTEXT_SELECT)) wr_u8(CONTEXT_GATE, 2);
    reset_message_sequence();
    wr_u16(POST_INPUT_COUNTDOWN, 3);
    next_stage(ROUTINE_OUTCOME);
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
    gaddr queue = MESSAGE_QUEUE;
    int16_t mode;

    if (!countdown_expired()) return;
    wr_u8(POST_INPUT_AUX, 0);
    if (rd_u8(SEQUENCE_FLAG)) {
        wr_u8(CONTEXT_REQUEST, 1);
        wr_u8(CONTEXT_GATE, 2);
        wr_u16(POST_INPUT_COUNTDOWN, 3);
        next_stage(ROUTINE_OUTCOME);
        return;
    }
    mode = (int8_t)rd_u8(MODE_SELECT);
    if (!rd_u8(MODE_MESSAGES_OFF) && (uint16_t)mode >= 3 && (uint16_t)mode <= 8) {
        wr_u16(queue, mode == 3 ? 0x5F : 0x60);
        queue += 2;
    }
    wr_u8(MODE_MESSAGES_OFF, 0);
    wr_u16(queue, 0x47);
    wr_u16(queue + 2, 0);
    next_stage(ROUTINE_QUEUE_MESSAGE_FOUR);
}
