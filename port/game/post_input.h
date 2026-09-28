#ifndef FA18_GAME_POST_INPUT_H
#define FA18_GAME_POST_INPUT_H

/* After the post-input countdown expires: wait until the viewport has
 * reached its target mode, then schedule complete_post_input. */
void await_viewport_match(void);

/* After the countdown expires again: clear the event flag, mark the
 * auxiliary byte and schedule the next stage. */
void complete_post_input(void);

#endif
