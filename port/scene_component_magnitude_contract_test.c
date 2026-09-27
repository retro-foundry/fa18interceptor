#include "scene_component_magnitude.h"

#include <assert.h>

int main(void) {
    uint16_t words[342];
    for (unsigned index = 0; index != 342; ++index) words[index] = 0x4000;
    const FA18SceneMagnitudeTable table = {words, 342};
    int16_t result;

    assert(fa18_scene_component_magnitude(&table, 16, 0, 0, &result) == 0);
    assert(result == 16);
    assert(fa18_scene_component_magnitude(&table, 3, 4, 0, &result) == 0);
    assert(result == 4);
    assert(fa18_scene_component_magnitude(&table, 0, 0, 0, &result) == 0);
    assert(result == 0);

    words[0] = 0xffff;
    assert(fa18_scene_component_magnitude(&table, 0x7fff, 0, 0, &result) == 0);
    assert(result == 0x7fff);
    words[0] = 0x4000;

    const FA18SceneMagnitudeTable short_table = {words, 1};
    assert(fa18_scene_component_magnitude(&short_table, 3, 4, 0, &result) == -1);
    assert(fa18_scene_component_magnitude(&table, 0, 0, 0, 0) == -1);
    return 0;
}
