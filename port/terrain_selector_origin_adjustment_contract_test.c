#include "terrain_selector_origin_adjustment.h"

#include <assert.h>

static int normalize(void *context, uint16_t length, int32_t candidate[3]) {
    (void)context;
    assert(length == 0x200);
    assert(candidate[0] == 0x48 && candidate[1] == -0x10 && candidate[2] == 8);
    /* Distinct child outputs catch the old, incorrect shift-only callback
     * that never consumed the normalizer's modified triple. */
    candidate[0] = 0x101; candidate[1] = -0x22; candidate[2] = 0x33;
    return 0;
}

int main(void) {
    FA18TerrainSelectorOriginAdjustmentState state = {
        .magnitude = 0x4800, .candidate = {0x120, -0x40, 0x20},
        .smoothed_delta = {0, 0, 0}, .origin = {0x401000, 0x2000, 0x3000},
        .shift = 1, .normalize = normalize
    };
    assert(fa18_adjust_terrain_selector_origin(&state) == 0);
    /* The quartered triple enters the child; its output uses the saved shift. */
    assert(state.smoothed_delta[0] == 0x202 && state.smoothed_delta[1] == -0x44 &&
           state.smoothed_delta[2] == 0x66);
    assert(state.origin[0] == 0x401202 && state.origin[1] == 0x1fbc &&
           state.origin[2] == 0x3066);
    assert(state.negated_companion[0] == -0x1202 && state.negated_companion[1] == -0x1fbc &&
           state.negated_companion[2] == -0x3066);
    assert(fa18_adjust_terrain_selector_origin(0) == -1);
    return 0;
}
