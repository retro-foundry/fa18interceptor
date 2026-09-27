#include "angle_octant.h"

#include <assert.h>

int main(void) {
    FA18AngleOctantState state = { 0, 0, 0, 0xff };
    const int16_t starts[] = { 0, 0x0e10, 0x1c20, 0x2a30,
                               0x3840, 0x4650, 0x5460, 0x6270 };
    for (uint8_t octant = 0; octant != 8; ++octant) {
        state.source_select_state = 0;
        state.alternate_angle = starts[octant];
        assert(fa18_classify_angle_octant(&state) == 0 && state.octant == octant);
    }
    state.primary_angle = 0x1c1f;
    state.alternate_angle = 0x6270;
    state.source_select_state = 1;
    assert(fa18_classify_angle_octant(&state) == 0 && state.octant == 1);
    state.source_select_state = 0;
    state.alternate_angle = -1;
    assert(fa18_classify_angle_octant(&state) == 0 && state.octant == 0);
    assert(fa18_classify_angle_octant(0) == -1);
    return 0;
}
