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

#endif
