#ifndef FA18_SCENE_ROOT_SETUP_H
#define FA18_SCENE_ROOT_SETUP_H

#include <stdint.h>

enum {
    FA18_SCENE_ROOT_SETUP_BLOCKS = 3,
    FA18_SCENE_ROOT_SETUP_BLOCK_BYTES = 164
};

/* Direct fields owned by `$C0840E`, `$C09620`, and `$C095C0`. Field names
 * follow their offsets where consumers are not yet established. */
typedef struct {
    uint16_t word_00;
    uint8_t byte_21, byte_2b, byte_63, byte_65, byte_71;
    uint16_t word_6c, word_6e, word_78, word_7e;
    uint32_t long_3e, long_42, long_46, long_50, long_56;
    uint16_t word_54, word_5a;
    uint8_t reset_blocks[FA18_SCENE_ROOT_SETUP_BLOCKS][FA18_SCENE_ROOT_SETUP_BLOCK_BYTES];
    uint32_t display_pointer;
    uint8_t display_byte, c084_flags[7];
    uint16_t display_word, c084_word;
    uint8_t setup_flags[7];
    uint16_t setup_limit, setup_word;
    uint8_t setup_latch, mode_source;
    uint32_t transient_longs[2];
    uint16_t transient_words[3];
} FA18SceneRootSetupState;

/* `$C0840E`, called by `$C09620`. */
int fa18_initialize_scene_root_display_state(FA18SceneRootSetupState *state);
/* `$C09620`: direct preparation including its `$C0840E` call. */
int fa18_prepare_scene_root_state(FA18SceneRootSetupState *state);
/* `$C095C0`: direct transient reset. */
int fa18_reset_scene_root_transients(FA18SceneRootSetupState *state);
/* Ordered `$C09620`, `$C095C0` helper pair from `$C0924A`. */
int fa18_initialize_scene_root_setup(FA18SceneRootSetupState *state);
int fa18_initialize_scene_root_setup_callback(void *context);

#endif
