#include "scene_stream_vector.h"

#include <assert.h>

int main(void) {
    FA18SceneStreamVector vector;
    const FA18SceneStreamVectorInput direct = {{0x00010000, -0x00020000, 0x00030000}, 0};
    assert(fa18_prepare_scene_stream_vector(&direct, &vector) == 0);
    assert(vector.shifted_component[0] == 0x00010000 &&
           vector.shifted_component[1] == -0x00020000 &&
           vector.shifted_component[2] == 0x00030000);
    assert(vector.scaled_component[0] == 256 && vector.scaled_component[1] == -512 &&
           vector.scaled_component[2] == 768);
    assert(vector.negated_scaled_component[0] == -256 &&
           vector.negated_scaled_component[1] == 512 &&
           vector.negated_scaled_component[2] == -768);

    const FA18SceneStreamVectorInput shifted = {{0x00010000, -0x00010000, 0x00008000}, 4};
    assert(fa18_prepare_scene_stream_vector(&shifted, &vector) == 0);
    assert(vector.shifted_component[0] == 0x1000 && vector.shifted_component[1] == -0x1000 &&
           vector.shifted_component[2] == 0x0800);
    assert(vector.scaled_component[0] == 16 && vector.scaled_component[1] == -16 &&
           vector.scaled_component[2] == 8);

    const FA18SceneStreamVectorInput sign_extended = {{1, -1, 0}, 40};
    assert(fa18_prepare_scene_stream_vector(&sign_extended, &vector) == 0);
    assert(vector.shifted_component[0] == 0 && vector.shifted_component[1] == -1 &&
           vector.negated_scaled_component[1] == 1);
    assert(fa18_prepare_scene_stream_vector(0, &vector) == -1);
    return 0;
}
