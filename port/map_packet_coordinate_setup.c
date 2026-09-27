#include "map_packet_coordinate_setup.h"

static uint32_t swap_words(uint32_t value) {
    return (value << 16) | (value >> 16);
}

static uint32_t rol_long_4(uint32_t value) {
    return (value << 4) | (value >> 28);
}

static uint32_t lsr_word(uint32_t value, uint16_t count) {
    uint16_t low = (uint16_t)value;
    count &= 63u;
    low = count >= 16u ? 0u : (uint16_t)(low >> count);
    return (value & UINT32_C(0xffff0000)) | low;
}

static uint32_t add_word_4(uint32_t value) {
    return (value & UINT32_C(0xffff0000)) |
           (uint16_t)((uint16_t)value + 4u);
}

int fa18_prepare_map_packet_coordinate_setup(
    const FA18MapPacketCoordinateSetupInput *input,
    FA18MapPacketCoordinateSetupResult *result) {
    if (!input || !result) return -1;
    const int32_t *component = input->directory_selector_gate ?
        input->selector_component : input->control_component;

    uint32_t origin = (uint32_t)component[1] + UINT32_C(0x1000);
    origin = rol_long_4(swap_words(origin));
    result->origin_component = (int16_t)(uint16_t)(UINT32_C(0) - origin);

    uint32_t coordinate_x = swap_words((uint32_t)component[0]);
    uint32_t coordinate_y = swap_words((uint32_t)component[2]);
    coordinate_x = lsr_word(coordinate_x, input->coordinate_bin_shift);
    coordinate_y = lsr_word(coordinate_y, input->coordinate_bin_shift);
    if (!input->alternate_layout) {
        coordinate_x = add_word_4(coordinate_x);
        coordinate_y = add_word_4(coordinate_y);
    }
    result->coordinate_x = (int32_t)coordinate_x;
    result->coordinate_y = (int32_t)coordinate_y;
    result->row_min = (int16_t)coordinate_x;
    result->column_min = (int16_t)coordinate_y;
    return 0;
}
