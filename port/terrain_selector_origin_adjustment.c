#include "terrain_selector_origin_adjustment.h"

static int32_t asr_long(int32_t value, unsigned count) {
    if (!count) return value;
    if (count >= 32u) return value < 0 ? -1 : 0;
    return value >= 0 ? value >> count :
        (int32_t)-((-(int64_t)value + ((INT64_C(1) << count) - 1)) >> count);
}

static int32_t add_long(int32_t left, int32_t right) {
    return (int32_t)((uint32_t)left + (uint32_t)right);
}

static int32_t sub_long(int32_t left, int32_t right) {
    return (int32_t)((uint32_t)left - (uint32_t)right);
}

static int32_t sign_extend_word(int32_t value) {
    return (int16_t)value;
}

static int32_t asl_long(int32_t value, unsigned count) {
    count &= 63u;
    if (count >= 32u) return 0;
    return (int32_t)((uint32_t)value << count);
}

int fa18_adjust_terrain_selector_origin(FA18TerrainSelectorOriginAdjustmentState *state) {
    int32_t candidate[3];

    if (!state || !state->normalize)
        return -1;
    for (unsigned axis = 0; axis != 3; ++axis)
        candidate[axis] = state->candidate[axis];
    while (state->magnitude >= 0x4800) {
        for (unsigned axis = 0; axis != 3; ++axis)
            candidate[axis] = asr_long(candidate[axis], 2);
        state->magnitude = asr_long(state->magnitude, 2);
    }
    if (state->normalize(state->normalize_context, 0x200, candidate) != 0)
        return -1;
    for (unsigned axis = 0; axis != 3; ++axis)
        candidate[axis] = asl_long(sign_extend_word(candidate[axis]), state->shift);
    if (state->smoothed_delta[0] || state->smoothed_delta[1] || state->smoothed_delta[2])
        for (unsigned axis = 0; axis != 3; ++axis)
            candidate[axis] = add_long(
                asr_long(sub_long(candidate[axis], state->smoothed_delta[axis]), 1),
                state->smoothed_delta[axis]);
    for (unsigned axis = 0; axis != 3; ++axis) {
        int32_t companion;
        state->smoothed_delta[axis] = candidate[axis];
        state->origin[axis] = add_long(state->origin[axis], candidate[axis]);
        companion = state->origin[axis];
        if (axis != 1)
            companion = (int32_t)((uint32_t)companion & 0x003fffffu);
        state->negated_companion[axis] =
            (int32_t)(UINT32_C(0) - (uint32_t)companion);
    }
    return 0;
}
