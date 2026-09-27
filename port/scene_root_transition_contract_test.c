#include "scene_root_transition.h"

#include <assert.h>
#include <string.h>

int main(void) {
    FA18SceneRootTransitionState transition = {
        .activity_a = 1, .activity_b = 1, .display_source_a = 0x12345678u,
        .display_source_b = 0x89abcdefu, .cursor_word_a = 7, .cursor_word_b = 8
    };
    FA18SceneRootSetupState root;
    memset(&root, 0xff, sizeof root);
    assert(fa18_begin_scene_root_transition(&transition, &root) == 0);
    assert(!transition.activity_a && !transition.activity_b && transition.latch_a &&
           transition.latch_b && transition.display_cursor_a == 0x12345678u &&
           transition.display_cursor_b == 0x89abcdf3u && !transition.cursor_word_a &&
           !transition.cursor_word_b && transition.marker == 0xff &&
           !transition.root_word && transition.root_index == 9 &&
           transition.root_span_a == 72 && transition.root_span_b == 72 &&
           transition.negative_latch == 0xfe);
    assert(root.word_00 == 0x11c8 && root.word_7e == 0x1400 && root.byte_2b == 9);
    assert(fa18_begin_scene_root_transition(NULL, &root) == -1 &&
           fa18_begin_scene_root_transition(&transition, NULL) == -1);
    return 0;
}
