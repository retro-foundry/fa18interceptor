#include "scene_component_accumulation.h"

#include <assert.h>

int main(void) {
    FA18SceneComponentRecord records[22] = {{0}};
    records[21] = (FA18SceneComponentRecord){0x00f12345u, -256, 0x00fff000u};
    FA18SceneComponentAccumulation output = {0};

    assert(fa18_accumulate_scene_components(0x1502, records, 22, 100, -5,
                                            -256, &output) == 0);
    assert(output.component[0] == 25600 + 0x48d1);
    assert(output.component[1] == -128);
    assert(output.component[2] == -1280 + 261120);
    assert(output.accumulated == 1);

    records[0] = (FA18SceneComponentRecord){0xffffffffu, -3, 0xffffffffu};
    assert(fa18_accumulate_scene_components(0x0001, records, 22, -1, 1, -2,
                                            &output) == 0);
    assert(output.component[0] == -256 + 0x7ffff);
    assert(output.component[1] == -3);
    assert(output.component[2] == 256 + 0x7ffff);

    assert(fa18_accumulate_scene_components(0x1600, records, 22, 0, 0, 0,
                                            &output) == -1);
    assert(fa18_accumulate_scene_components(0, 0, 0, 0, 0, 0, &output) == -1);
    return 0;
}
