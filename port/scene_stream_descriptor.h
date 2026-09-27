#ifndef FA18_SCENE_STREAM_DESCRIPTOR_H
#define FA18_SCENE_STREAM_DESCRIPTOR_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    FA18_SCENE_STREAM_DESCRIPTOR_PUBLISHED = 0,
    FA18_SCENE_STREAM_DESCRIPTOR_ALTERNATE = 1
} FA18SceneStreamDescriptorRoute;

typedef struct {
    uint16_t selected_word;
    uint32_t record_base;
    uint32_t stage_cursor;
    uint8_t shared_flag;
} FA18SceneStreamDescriptorSetupInput;

typedef struct {
    uint8_t local_flag;
    uint32_t descriptor_cursor;
    uint32_t published_stage_cursor;
} FA18SceneStreamDescriptorSetup;

/* `$C1EEDA-$C1EF15`: prepare the selected descriptor's local flag and,
 * on the observed publication route, its base-relative descriptor cursor and
 * the current control-stream cursor. */
int fa18_prepare_scene_stream_descriptor(
    const FA18SceneStreamDescriptorSetupInput *input,
    FA18SceneStreamDescriptorSetup *setup,
    FA18SceneStreamDescriptorRoute *route);

typedef struct {
    uint8_t control_byte;
    uint16_t word_a;
    uint16_t word_b;
    uint8_t high_nibble;
    uint8_t low_nibble;
} FA18SceneStreamDescriptorFields;

/* `$C1EF16-$C1EF2D`: decode the seven bytes immediately consumed from the
 * published descriptor cursor. */
int fa18_decode_scene_stream_descriptor(const uint8_t *bytes, size_t size,
                                        FA18SceneStreamDescriptorFields *fields);

#endif
