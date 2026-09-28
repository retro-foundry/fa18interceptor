#include "context_selector_pack.h"

static uint16_t read_be16(const uint8_t *bytes) {
    return (uint16_t)((uint16_t)bytes[0] << 8 | bytes[1]);
}

static int16_t asr_word(int16_t value, unsigned count) {
    return value >= 0 ? (int16_t)(value >> count) :
        (int16_t)-((-(int32_t)value + ((1 << count) - 1)) >> count);
}

static int16_t abs_word(int16_t value) {
    return value == INT16_MIN ? value : value < 0 ? (int16_t)-value : value;
}

static uint8_t selector_byte(uint16_t x, uint16_t y) {
    int16_t left = (int16_t)(x & 3u);
    int16_t right = (int16_t)(y & 3u);
    left = (int16_t)-(left - 3);
    right = (int16_t)-(right - 3);
    return (uint8_t)(left + right * 4);
}

static uint8_t classify(const uint8_t *record) {
    int16_t first = abs_word((int16_t)read_be16(record + 0x56));
    int16_t second = abs_word((int16_t)read_be16(record + 0x58));
    int16_t third = asr_word(abs_word((int16_t)read_be16(record + 0x5a)), 2);
    int16_t maximum = first;
    if (second > maximum) maximum = second;
    if (third > maximum) maximum = third;
    if (maximum <= 0x60) {
        int16_t secondary = abs_word((int16_t)read_be16(record + 0x6c));
        return secondary > 0x1000 ? 3 : 5;
    }
    return maximum > 0xc0 ? 1 : 3;
}

int fa18_update_context_selector_pack(
    const uint8_t *record, size_t record_size,
    FA18ContextSelectorPackState *state, uint8_t prior_change,
    FA18ContextSelectorOriginUpdate update_origin, void *context) {
    uint8_t changed = prior_change;
    if (!record || !state || record_size < FA18_CONTEXT_SELECTOR_RECORD_BYTES)
        return -1;
    if (state->context_selection == 0) {
        state->magnitude_class = classify(record);
        state->selector_word_x = read_be16(record + 6);
        state->selector_word_y = read_be16(record + 8);
        state->selector_byte_y = record[0x0a];
        state->selector_byte_x = selector_byte(state->selector_word_x,
                                                state->selector_word_y);
    } else {
        uint32_t x, z;
        int16_t x_word, z_word;
        if (!update_origin || update_origin(context, state->origin) != 0) return -1;
        state->magnitude_class = classify(record);
        x = (uint32_t)state->origin[0] & UINT32_C(0x1fffffff);
        z = (uint32_t)state->origin[2] & UINT32_C(0x1fffffff);
        x_word = asr_word((int16_t)(x >> 16), 4);
        z_word = asr_word((int16_t)(z >> 16), 4);
        const uint8_t next_y = selector_byte((uint16_t)x_word, (uint16_t)z_word);
        x_word = asr_word(x_word, 2);
        z_word = asr_word(z_word, 2);
        const uint8_t next_x = selector_byte((uint16_t)x_word, (uint16_t)z_word);
        if (state->selector_byte_y != next_y) {
            if (state->mode != 2 || state->projection_depth > -0x1000)
                changed = UINT8_MAX;
            state->selector_byte_y = next_y;
        }
        if (state->selector_byte_x != next_x) {
            if (state->mode != 2 || state->projection_depth > -0x1000)
                changed = UINT8_MAX;
            state->selector_byte_x = next_x;
        }
        if (state->selector_word_x != (uint16_t)x_word) {
            changed = UINT8_MAX;
            state->selector_word_x = (uint16_t)x_word;
        }
        if (state->selector_word_y != (uint16_t)z_word) {
            changed = UINT8_MAX;
            state->selector_word_y = (uint16_t)z_word;
        }
    }
    state->selector_change |= changed;
    return 0;
}
