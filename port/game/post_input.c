/* Post-input stage sequence (STAGE_CALLBACK chain). */
#include "post_input.h"

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
