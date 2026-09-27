#include "scene_fixed_point_stage.h"

#include <assert.h>

int main(void) {
    uint16_t words[342];
    for (unsigned index = 0; index != 342; ++index) words[index] = 0x4000;
    const FA18SceneMagnitudeTable table = {words, 342};
    const FA18SceneFixedPointInput input = {
        0x20, 0x100, 0x10, 0, 0, 0, 0x20, 0, 0x10
    };
    FA18SceneFixedPointRoute route;
    int16_t result;
    assert(fa18_run_scene_fixed_point_stage(&input, &table, &result, &route) == 0);
    assert(route == FA18_SCENE_FIXED_POINT_MAGNITUDE && result == 16);

    FA18SceneFixedPointInput alternate = input;
    alternate.alternate_long_mode = 1;
    alternate.alternate_long = -0x1000;
    assert(fa18_run_scene_fixed_point_stage(&alternate, &table, &result, &route) == 0);
    assert(route == FA18_SCENE_FIXED_POINT_MAGNITUDE && result == 4);

    FA18SceneFixedPointInput boundary = input;
    boundary.offset_long = 0x7fff0;
    assert(fa18_run_scene_fixed_point_stage(&boundary, &table, &result, &route) == 0);
    assert(route == FA18_SCENE_FIXED_POINT_C1D90A_BOUNDARY);
    assert(fa18_run_scene_fixed_point_stage(0, &table, &result, &route) == -1);
    return 0;
}
