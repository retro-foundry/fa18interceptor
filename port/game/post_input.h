#ifndef FA18_GAME_POST_INPUT_H
#define FA18_GAME_POST_INPUT_H

/* After the post-input countdown expires: wait until the viewport has
 * reached its target mode, then schedule complete_post_input. */
void await_viewport_match(void);

/* After the countdown expires again: clear the event flag, mark the
 * auxiliary byte and schedule the next stage. */
void complete_post_input(void);

/* Next stage: start the selected context, or, if one is running, wait for
 * its event and then change the viewport. */
void start_context_stage(void);

/* Once POST_INPUT_COUNTDOWN has gone negative ($C10D8A): mark it expired,
 * sound tone 2 and install STAGE_AFTER_EXPIRY as the stage callback. */
void check_post_input_expiry(void);

/* The rest of the sequence. "Once the countdown expires" means once
 * POST_INPUT_COUNTDOWN is negative. */

/* Once expired and the viewport is in its target mode: countdown 2, then
 * mark_viewport_ready ($C0F946). */
void await_viewport_then_ready(void);
/* Once expired: POST_INPUT_AUX = 1, then $C0F992 ($C0F974). */
void mark_viewport_ready(void);
/* Once expired: without SEQUENCE_FLAG queue message $49 and wait for a key
 * or message; with it, reset the messages and go on ($C0FB70). */
void choose_after_countdown(void);
/* On a finished message sequence or a taken key: reset the messages and go
 * on to ROUTINE_AFTER_POST_INPUT ($C0FBB6). */
void leave_on_key_or_message(void);
/* Once expired: viewport mode 0, target $0F, then the mode-four entry
 * ($C101FC). */
void reset_viewport_after_countdown(void);
/* Once the viewport reaches its target: context state 4, gate 1, countdown
 * 5, then $C10678 ($C10228). */
void enter_mode_four_when_ready(void);
/* When MESSAGE_STATE_C is 1: queue message 4 and start the outcome
 * countdown ($C1072E). */
void queue_message_four(void);
/* On a context request: reset the messages, countdown 3, then the outcome
 * stage ($C1075A). */
void start_outcome_countdown(void);
/* Once expired: COCKPIT_FLAGS bit 6, FIRE_STATE $FE, then
 * STAGE_AFTER_EXPIRY ($C11872). */
void expire_to_fire_state(void);
/* On a finished message sequence: end the sequence ($C118E6). */
void end_on_message(void);
/* On a finished message sequence restart after $64; at phase $FF end;
 * at phase 1 restart at once ($C11958). */
void follow_message_or_phase(void);
/* Once expired: back to phase 0, event 1, countdown 3, viewport target 0,
 * then await_viewport_then_ready ($C119D4). */
void restart_after_countdown(void);
/* Phase 3 once the player record is active, flagged $C080 in +2, with +$6E
 * clear, and PLAYER_PHASE free ($C0A2F0). */
void begin_phase_three(void);

#endif
