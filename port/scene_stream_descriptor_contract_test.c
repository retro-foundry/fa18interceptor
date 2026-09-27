#include "scene_stream_descriptor.h"

#include <assert.h>

int main(void) {
    FA18SceneStreamDescriptorSetup setup;
    FA18SceneStreamDescriptorRoute route;
    FA18SceneStreamDescriptorSetupInput input = {0x4234, 0x1000, 0x2000, 1};
    assert(fa18_prepare_scene_stream_descriptor(&input, &setup, &route) == 0);
    assert(route == FA18_SCENE_STREAM_DESCRIPTOR_PUBLISHED && !setup.local_flag &&
           setup.descriptor_cursor == 0x1234 && setup.published_stage_cursor == 0x2000);

    input.selected_word = 0x0234;
    assert(fa18_prepare_scene_stream_descriptor(&input, &setup, &route) == 0);
    assert(route == FA18_SCENE_STREAM_DESCRIPTOR_ALTERNATE && setup.local_flag);
    input.shared_flag = 0;
    assert(fa18_prepare_scene_stream_descriptor(&input, &setup, &route) == 0);
    assert(route == FA18_SCENE_STREAM_DESCRIPTOR_PUBLISHED && setup.local_flag &&
           setup.descriptor_cursor == 0x1234);

    const uint8_t bytes[] = {0xab, 0xcd, 0x12, 0x34, 0x56, 0x78, 0x9e};
    FA18SceneStreamDescriptorFields fields;
    assert(fa18_decode_scene_stream_descriptor(bytes, sizeof bytes, &fields) == 0);
    assert(fields.control_byte == 0xcd && fields.word_a == 0x1234 &&
           fields.word_b == 0x5678 && fields.high_nibble == 9 && fields.low_nibble == 14);
    assert(fa18_decode_scene_stream_descriptor(bytes, 6, &fields) == -1);
    return 0;
}
