/* Register flow through scene initialization ($C0FAA4). */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "memory.h"
#include "stages.h"
#include "scene_setup.h"
#include "cockpit.h"
#include "glue_text.h"

void scene_mode_run_with_registers(void); /* glue_scene_mode.c */

static void select_scene(void *context) {
    (void)context;
    scene_mode_run_with_registers();
}

static void reset_root(void *context) {
    int8_t pose = rd_s8(SCENE_POSE_ENTRY);
    (void)context;
    /* $C0924A ends with the selected root orientation. */
    reset_scene_context();
    scene_setup_registers(pose);
}

static void reset_messages(void *context) {
    (void)context;
    reset_message_sequence();
    D(0) = 0;
    A(0) = MESSAGE_QUEUE + 4;
}

static void finish(void *context) {
    (void)context;
    finish_scene_setup();
    D(0) = 0;
    D(4) = 3;
}

void scene_start_run_with_registers(void) {
    SceneStartHooks hooks = {select_scene, reset_root, reset_messages, finish, 0};
    D(0) = 0;
    initialize_scene_state(&hooks);
}

int glue_C0FAA4(void) {
    scene_start_run_with_registers();
    return glue_return();
}
