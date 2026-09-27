#include "scene_root_setup.h"

#include <string.h>

int fa18_initialize_scene_root_display_state(FA18SceneRootSetupState *state) {
    if (!state) return -1;
    state->display_pointer = 0x0061a800u;
    state->display_byte = 0x24;
    state->display_word = 0x01f4;
    state->c084_flags[0] = 3;
    state->c084_flags[1] = 3;
    state->c084_flags[2] = 0x10;
    state->c084_flags[3] = 0x10;
    state->c084_flags[4] = 0;
    state->c084_flags[5] = 0;
    state->c084_flags[6] = 0;
    state->c084_word = 0;
    memset(state->reset_blocks, 0, sizeof state->reset_blocks);
    return 0;
}

int fa18_prepare_scene_root_state(FA18SceneRootSetupState *state) {
    if (!state) return -1;
    state->byte_21 &= (uint8_t)~1u;
    state->byte_63 = 0x0d;
    if (fa18_initialize_scene_root_display_state(state) != 0) return -1;
    state->word_00 = 0x11c8;
    state->word_7e = 0x1400;
    memset(state->setup_flags, 0, sizeof state->setup_flags);
    state->setup_limit = 0x7fff;
    state->setup_latch = 1;
    state->byte_71 = 0xff;
    state->setup_word = 0xffff;
    if (state->mode_source) state->mode_source = 4;
    return 0;
}

int fa18_reset_scene_root_transients(FA18SceneRootSetupState *state) {
    if (!state) return -1;
    state->word_6c = 0;
    state->word_6e = 0;
    state->word_78 = 0;
    state->long_3e = 0;
    state->long_42 = 0;
    state->long_46 = 0;
    state->long_56 = 0;
    state->word_5a = 0;
    state->long_50 = 0;
    state->word_54 = 0;
    state->byte_65 = 0;
    state->byte_2b = 9;
    state->word_00 &= 0x7fff;
    state->transient_longs[0] = 0;
    state->transient_longs[1] = 0;
    state->transient_words[0] = 0;
    state->transient_words[1] = 0;
    state->transient_words[2] = 0xffff;
    return 0;
}

int fa18_initialize_scene_root_setup(FA18SceneRootSetupState *state) {
    if (fa18_prepare_scene_root_state(state) != 0) return -1;
    return fa18_reset_scene_root_transients(state);
}

int fa18_initialize_scene_root_setup_callback(void *context) {
    return fa18_initialize_scene_root_setup(context);
}
