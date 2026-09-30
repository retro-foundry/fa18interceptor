/* Timer-gated scene transition ($C0FA04). */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "memory.h"
#include "render_buffers.h"
#include "stages.h"
#include "glue_text.h"

void scene_start_run_with_registers(void); /* glue_scene_start.c */

static void start_scene(void *context) {
    (void)context;
    scene_start_run_with_registers();
}

static void clear_buffers(void *context) {
    (void)context;
    clear_render_buffers();
    clear_render_buffers_registers();
}

int glue_C0FA04(void) {
    PostInputFollowupHooks hooks = {start_scene, clear_buffers, 0};
    int16_t countdown = rd_s16(POST_INPUT_COUNTDOWN);

    SET_W(D(0), countdown);
    finish_post_input_followup(&hooks);
    if (countdown < 0) {
        D(0) = 0;
        A(0) = ROUTINE_POST_INPUT_MATCH;
    }
    return glue_return();
}
