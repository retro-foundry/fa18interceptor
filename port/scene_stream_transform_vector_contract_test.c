#include "scene_stream_transform_vector.h"

#include <assert.h>

int main(void) {
    FA18SceneStreamTransformVector vector;
    const FA18SceneStreamTransformVectorInput direct = {
        {0x00010000, 0, 0x00030000}, 0x00010000, 0x00020000, 0x00040000, 0
    };
    assert(fa18_prepare_scene_stream_transform_vector(&direct, &vector) == 0);
    assert(vector.shift_count == 8 && vector.component[0] == 512 &&
           vector.component[1] == 1024 && vector.component[2] == 1280);

    const FA18SceneStreamTransformVectorInput negative_shift = {
        {0, 0, 0}, 1, 2, -1, 15
    };
    assert(fa18_prepare_scene_stream_transform_vector(&negative_shift, &vector) == 0);
    assert(vector.shift_count == UINT16_C(0xfff9) && vector.component[0] == 0 &&
           vector.component[1] == -1 && vector.component[2] == 0);

    FA18SceneStreamTransformVectorInput invalid = direct;
    invalid.descriptor_low_nibble = 16;
    assert(fa18_prepare_scene_stream_transform_vector(&invalid, &vector) == -1);
    return 0;
}
