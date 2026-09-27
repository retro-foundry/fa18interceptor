#include "update_stage_prefix.h"

#include <limits.h>

static int32_t negate_long(int32_t value) {
    return (int32_t)(UINT32_C(0) - (uint32_t)value);
}

static int16_t scaled_high_word(int32_t value) {
    const int16_t high_word = (int16_t)((uint32_t)value >> 16);
    if (high_word >= 0) return (int16_t)(high_word >> 5);
    return (int16_t)-((-(int32_t)high_word + 31) >> 5);
}

int fa18_prepare_update_stage_prefix(FA18UpdateStagePrefixState *state,
                                     FA18IndexedRecordUpdateStage update_stage,
                                     void *context, uint8_t *changed) {
    uint8_t flags = 0;
    int32_t inverted;
    int16_t scaled;
    if (!state || !update_stage || !changed) return -1;

    if (state->input_byte != state->input_byte_mirror) {
        state->input_byte_mirror = state->input_byte;
        if (!state->change_inhibit) flags |= 0x0b;
    }
    inverted = negate_long(state->signed_long);
    if (inverted < 0x0000a000) {
        if (state->long_mirror >= 0x0000a000) flags |= 0x0b;
    } else if (state->long_mirror < 0x0000a000) {
        flags |= 0x0b;
    }
    state->long_mirror = inverted;
    scaled = scaled_high_word(inverted);
    if (scaled != state->scaled_word) {
        if (state->mode_byte != 2u) flags |= 0x0b;
        state->scaled_word = scaled;
    }
    if (update_stage(context) != 0) return -1;
    *changed = flags;
    return 0;
}
