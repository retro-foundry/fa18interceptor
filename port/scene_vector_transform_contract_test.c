#include "scene_vector_transform.h"

#include <assert.h>

int main(void) {
    const FA18SceneVectorMatrix matrix = {
        {{16, 0, 0}, {0, -16, 0}, {0, 0, 32}}
    };
    const FA18SceneVectorBase base = {{0x11180000, -4, 7}};
    const FA18SceneVectorInput input = {{11, 2, -3}};
    FA18SceneVectorOutput output;
    assert(fa18_transform_scene_vector(&matrix, &base, &input, &output) == 0);
    assert(output.value[0] == 0x1118000b && output.value[1] == -6 && output.value[2] == 1);

    const FA18SceneVectorMatrix negative = {{{-1, 0, 0}, {0, 0, 0}, {0, 0, 0}}};
    const FA18SceneVectorBase zero = {{0, 0, 0}};
    const FA18SceneVectorInput one = {{1, 0, 0}};
    assert(fa18_transform_scene_vector(&negative, &zero, &one, &output) == 0);
    assert(output.value[0] == -1);
    assert(fa18_transform_scene_vector(NULL, &base, &input, &output) == -1);
    return 0;
}
