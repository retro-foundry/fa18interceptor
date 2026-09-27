#include "scene_stream_pair_transform.h"

#include <assert.h>

int main(void) {
    const FA18SceneStreamPairTransform transform = {
        {10, -20}, {1, 2, 3},
        0,
        {{256, 99, 0, 0, 77, 256, 128, 55, 128}}
    };
    const FA18SceneStreamPair pairs[] = {{{2, 4}}, {{-2, 8}}};
    FA18SceneStreamPairOutput output[2];
    uint16_t transformed;
    assert(fa18_transform_scene_stream_pairs(&transform, pairs, 2, 2, output, 2,
                                              &transformed) == 0);
    assert(transformed == 2);
    assert(output[0].value[0] == 13 && output[0].value[1] == -14 &&
           output[0].value[2] == 1);
    assert(output[1].value[0] == 9 && output[1].value[1] == -10 &&
           output[1].value[2] == 1);

    FA18SceneStreamPairTransform shifted = transform;
    shifted.shift = 1;
    assert(fa18_transform_scene_stream_pairs(&shifted, pairs, 2, 1, output, 2,
                                              &transformed) == 0);
    assert(transformed == 1 && output[0].value[0] == 12 && output[0].value[1] == -16 &&
           output[0].value[2] == -1);
    assert(fa18_transform_scene_stream_pairs(&transform, 0, 0, 0, 0, 0,
                                              &transformed) == 0 && !transformed);
    assert(fa18_transform_scene_stream_pairs(&transform, pairs, 1, 2, output, 2,
                                              &transformed) == -1);
    return 0;
}
