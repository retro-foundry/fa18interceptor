#include "scene_placement_builder_tail.h"

#include <assert.h>
#include <string.h>

int main(void) {
    uint8_t bytes[FA18_SCENE_PLACEMENT_BYTES] = {0, 0x40};
    uint8_t shifts[0xf0] = {0};
    FA18ScenePlacementBuilderTailInput input = {
        { -16, -8, 14336 }, { 3, 48, 96 }, shifts, sizeof shifts,
        UINT32_C(0x00104183), 0
    };
    FA18ScenePlacementBuilderTailResult result;
    shifts[48] = 7;
    assert(fa18_finish_scene_placement_record(bytes, &input, &result) == 0);
    assert(result.shift_count == 7 && result.next_cycle_byte == 3);
    assert(!memcmp(bytes, (uint8_t[]){0, 0x47, 0, 0, 0, 0,
        0xff, 0xff, 0xff, 0xff, 0, 112, 0, 16, 65, 131,
        0, 0, 0, 0, 0, 0, 0, 0}, sizeof bytes));
    input.magnitude[2] = 0x1e0;
    assert(fa18_finish_scene_placement_record(bytes, &input, &result) == -1);
    return 0;
}
