#include "scene_placement_builder_prefix.h"

enum { WORKSPACE_CELL_BYTES = 0x60, WORKSPACE_CELL_COUNT = 16 };

static int32_t scale_word_by_four(int16_t value) {
    return (int32_t)value * 4;
}

int fa18_prepare_scene_placement_builder_prefix(
    const FA18ScenePlacementBuilderPrefixInput *input,
    FA18ScenePlacementBuilderPrefix *output) {
    int16_t type_first, type_second;
    size_t type_pair_offset;
    uint8_t placement_cell;
    uint16_t first_byte, second_byte;
    size_t workspace_offset;

    if (!input || !output || !input->type_map || !input->type_pair_table ||
        !input->placement_map || !input->workspace_pair_table ||
        !input->translation_pair_table || !input->workspace ||
        input->type_selector > 0x14u ||
        input->type_selector >= input->type_map_count ||
        input->placement_selector > 0x0fu ||
        input->placement_selector >= input->placement_map_count)
        return -1;

    type_pair_offset = (size_t)(uint8_t)input->type_map[input->type_selector] * 2u;
    if (type_pair_offset > input->type_pair_count ||
        input->type_pair_count - type_pair_offset < 2u)
        return -1;
    type_first = input->type_pair_table[type_pair_offset];
    type_second = input->type_pair_table[type_pair_offset + 1u];
    first_byte = (uint16_t)((int16_t)(type_first + input->type_bias[0]));
    second_byte = (uint16_t)((int16_t)(type_second + input->type_bias[1]));
    if ((int16_t)first_byte < 0) first_byte = (uint16_t)(first_byte + 0x80u);
    if ((int16_t)second_byte < 0) second_byte = (uint16_t)(second_byte + 0x80u);

    placement_cell = (uint8_t)input->placement_map[input->placement_selector];
    if (placement_cell >= input->workspace_pair_count ||
        input->workspace_pair_count - placement_cell < 2u ||
        placement_cell >= input->translation_pair_count ||
        input->translation_pair_count - placement_cell < 2u ||
        placement_cell >= WORKSPACE_CELL_COUNT)
        return -1;
    workspace_offset = (size_t)placement_cell * WORKSPACE_CELL_BYTES;
    if (workspace_offset > input->workspace_size ||
        input->workspace_size - workspace_offset < WORKSPACE_CELL_BYTES)
        return -1;

    output->selector_packet = ((uint32_t)(uint8_t)first_byte << 8) |
                              (uint8_t)second_byte;
    output->record_tail_word = (int16_t)placement_cell;
    output->workspace_component[0] = scale_word_by_four(
        input->workspace_pair_table[placement_cell]);
    output->workspace_component[1] = scale_word_by_four(
        input->workspace_pair_table[placement_cell + 1u]);
    output->translation_component[0] = scale_word_by_four(
        input->translation_pair_table[placement_cell]);
    output->translation_component[1] = scale_word_by_four(
        input->translation_pair_table[placement_cell + 1u]);
    output->workspace_cursor = input->workspace + workspace_offset;
    output->workspace_end = output->workspace_cursor + WORKSPACE_CELL_BYTES;
    return 0;
}
