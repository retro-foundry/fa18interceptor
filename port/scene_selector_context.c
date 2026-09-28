#include "scene_selector_context.h"

static int16_t read_be16(const uint8_t *bytes) {
    return (int16_t)((uint16_t)bytes[0] << 8 | bytes[1]);
}

static int16_t arithmetic_shift_right_word(int16_t value, unsigned count) {
    if (value >= 0) return (int16_t)(value >> count);
    return (int16_t)-((-(int32_t)value + ((1 << count) - 1)) >> count);
}

static int16_t alternate_selector_word(int32_t value) {
    const uint32_t masked = (uint32_t)value & UINT32_C(0x1fffffff);
    const int16_t source_word = (int16_t)(masked >> 16);
    return arithmetic_shift_right_word(source_word, 8);
}

int fa18_update_scene_selector_context(
    const FA18SceneSelectorContextInput *input,
    FA18SceneSelectorContextState *state) {
    if (!input || !state) return -1;
    if (input->alternate_source_enabled) {
        state->first_selector = alternate_selector_word(input->alternate_first);
        state->second_selector = alternate_selector_word(input->alternate_second);
    } else {
        if (!input->active_record ||
            input->active_record_size < FA18_SCENE_SELECTOR_CONTEXT_RECORD_BYTES)
            return -1;
        state->first_selector = arithmetic_shift_right_word(
            read_be16(input->active_record + 6), 2);
        state->second_selector = arithmetic_shift_right_word(
            read_be16(input->active_record + 8), 2);
    }
    state->append_enabled = 1;
    state->first_status = 0;
    state->second_status = 0;
    return 0;
}
