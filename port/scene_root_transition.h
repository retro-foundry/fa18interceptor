#ifndef FA18_SCENE_ROOT_TRANSITION_H
#define FA18_SCENE_ROOT_TRANSITION_H

#include <stdint.h>

#include "scene_root_setup.h"

/* Direct `$C0924A-$C092D2` state, before the caller-owned runtime descriptor
 * selection at `$C22048`. */
typedef struct {
    uint8_t activity_a, activity_b;
    uint8_t latch_a, latch_b;
    uint32_t display_source_a, display_source_b;
    uint32_t display_cursor_a, display_cursor_b;
    uint16_t cursor_word_a, cursor_word_b;
    uint8_t marker;
    uint8_t root_index;
    uint16_t root_span_a, root_span_b;
    uint8_t negative_latch;
    uint16_t root_word;
    uint8_t source_selector;
} FA18SceneRootTransitionState;

/* `$C0924A-$C092D2`: copy the display cursors, perform the ordered nested
 * root reset, then establish the literal root counters. The table/descriptor
 * selection beginning at `$C092D4` is intentionally outside this boundary. */
int fa18_begin_scene_root_transition(FA18SceneRootTransitionState *transition,
                                     FA18SceneRootSetupState *root);

#endif
