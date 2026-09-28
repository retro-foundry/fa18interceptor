#include "scene_renderer_defaults.h"

#include <assert.h>

int main(void) {
    FA18SceneRendererDefaults defaults;
    assert(fa18_initialize_scene_renderer_defaults(&defaults) == 0);
    assert(defaults.matrix_input[0] == 0x1c20 && defaults.matrix_input[1] == 0);
    assert(defaults.matrix_row_scale[0] == 0xa8 &&
           defaults.matrix_row_scale[1] == 0xfc &&
           defaults.matrix_row_scale[2] == 0x80);
    assert(defaults.display_bound_y == 0xa7 && defaults.display_vertical == 0x32 &&
           defaults.display_horizontal == 0x320);
    assert(fa18_initialize_scene_renderer_defaults(0) == -1);
    return 0;
}
