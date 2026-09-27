#include "scene_root_transition.h"

int fa18_begin_scene_root_transition(FA18SceneRootTransitionState *transition,
                                     FA18SceneRootSetupState *root) {
    if (!transition || !root) return -1;
    transition->activity_a = 0;
    transition->activity_b = 0;
    transition->latch_a = 1;
    transition->latch_b = 1;
    transition->display_cursor_a = transition->display_source_a;
    transition->display_cursor_b = transition->display_source_b;
    transition->display_cursor_b += 4u;
    transition->cursor_word_a = 0;
    transition->cursor_word_b = 0;
    transition->marker = 0xff;
    if (fa18_initialize_scene_root_setup(root) != 0) return -1;
    transition->root_word = 0;
    transition->root_index = 9;
    transition->root_span_a = (uint16_t)(transition->root_index << 3);
    transition->root_span_b = transition->root_span_a;
    transition->negative_latch = 0xfe;
    return 0;
}
