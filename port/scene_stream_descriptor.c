#include "scene_stream_descriptor.h"

#include "hunk.h"

int fa18_prepare_scene_stream_descriptor(
    const FA18SceneStreamDescriptorSetupInput *input,
    FA18SceneStreamDescriptorSetup *setup,
    FA18SceneStreamDescriptorRoute *route) {
    if (!input || !setup || !route) return -1;

    setup->local_flag = 0;
    if (!(input->selected_word & UINT16_C(0x4000))) {
        setup->local_flag = 1;
        if (input->shared_flag) {
            *route = FA18_SCENE_STREAM_DESCRIPTOR_ALTERNATE;
            return 0;
        }
    }
    setup->descriptor_cursor = input->record_base +
                               (uint32_t)(input->selected_word & UINT16_C(0x0fff));
    setup->published_stage_cursor = input->stage_cursor;
    *route = FA18_SCENE_STREAM_DESCRIPTOR_PUBLISHED;
    return 0;
}

int fa18_decode_scene_stream_descriptor(const uint8_t *bytes, size_t size,
                                        FA18SceneStreamDescriptorFields *fields) {
    if (!bytes || !fields || size < 7u) return -1;
    fields->control_byte = (uint8_t)fa18_be16(bytes);
    fields->word_a = fa18_be16(bytes + 2u);
    fields->word_b = fa18_be16(bytes + 4u);
    const uint8_t packed = bytes[6u];
    fields->high_nibble = (uint8_t)(packed >> 4);
    fields->low_nibble = packed & 0x0fu;
    return 0;
}
