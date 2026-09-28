#include "terrain_selector_origin_adjustment.h"

#include <assert.h>

static int scale(void *context, uint16_t input, int16_t *shift) {
    (void)context;
    assert(input == 0x200);
    *shift = 1;
    return 0;
}

int main(void) {
    FA18TerrainSelectorOriginAdjustmentState state = {
        .magnitude = 0x4800, .candidate = {0x120, -0x40, 0x20},
        .smoothed_delta = {0, 0, 0}, .origin = {0x401000, 0x2000, 0x3000},
        .scale = scale
    };
    assert(fa18_adjust_terrain_selector_origin(&state) == 0);
    /* Candidate is quartered, sign-extended from its low word, then doubled. */
    assert(state.smoothed_delta[0] == 0x90 && state.smoothed_delta[1] == -0x20 &&
           state.smoothed_delta[2] == 0x10);
    assert(state.origin[0] == 0x401090 && state.origin[1] == 0x1fe0 &&
           state.origin[2] == 0x3010);
    assert(state.negated_companion[0] == -0x1090 && state.negated_companion[1] == -0x1fe0 &&
           state.negated_companion[2] == -0x3010);
    assert(fa18_adjust_terrain_selector_origin(0) == -1);
    return 0;
}
