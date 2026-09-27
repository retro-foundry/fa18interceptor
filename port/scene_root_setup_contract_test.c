#include "scene_root_setup.h"

#include <assert.h>
#include <string.h>

int main(void) {
    FA18SceneRootSetupState state;
    memset(&state, 0xff, sizeof state);
    state.byte_21 = 0xff;
    state.mode_source = 1;
    assert(fa18_initialize_scene_root_setup_callback(&state) == 0);
    assert(state.word_00 == 0x11c8 && state.word_7e == 0x1400 &&
           state.byte_63 == 0x0d && state.byte_71 == 0xff &&
           state.byte_2b == 9 && !state.byte_65 &&
           !state.transient_longs[0] && !state.transient_longs[1] &&
           !state.transient_words[0] && !state.transient_words[1] &&
           state.transient_words[2] == 0xffff);
    assert(!(state.byte_21 & 1u) && state.setup_limit == 0x7fff &&
           state.setup_latch == 1 && state.setup_word == 0xffff &&
           state.display_pointer == 0x0061a800u && state.display_byte == 0x24 &&
           state.display_word == 0x01f4 && state.mode_source == 4 &&
           state.c084_flags[0] == 3 && state.c084_flags[1] == 3 &&
           state.c084_flags[2] == 0x10 && state.c084_flags[3] == 0x10 &&
           !state.c084_flags[4] && !state.c084_flags[5] && !state.c084_flags[6] &&
           !state.c084_word);
    for (unsigned block = 0; block < FA18_SCENE_ROOT_SETUP_BLOCKS; ++block)
        for (unsigned byte = 0; byte < FA18_SCENE_ROOT_SETUP_BLOCK_BYTES; ++byte)
            assert(!state.reset_blocks[block][byte]);
    assert(fa18_initialize_scene_root_setup(NULL) == -1);
    return 0;
}
