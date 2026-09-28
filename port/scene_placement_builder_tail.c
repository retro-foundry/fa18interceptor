#include "scene_placement_builder_tail.h"

#include <string.h>

#include "hunk.h"

static int32_t asr_long(int32_t value, unsigned count) {
    count &= 63u;
    if (!count) return value;
    if (count >= 32u) return value < 0 ? -1 : 0;
    if (value >= 0) return value >> count;
    return -(int32_t)(((uint32_t)(-(int64_t)value) + ((UINT32_C(1) << count) - 1u)) >> count);
}

static int16_t asr_word(int16_t value, unsigned count) {
    count &= 63u;
    if (!count) return value;
    if (count >= 16u) return value < 0 ? -1 : 0;
    if (value >= 0) return (int16_t)((uint16_t)value >> count);
    return (int16_t)-(((-(int32_t)value) + ((1 << count) - 1)) >> count);
}

static int32_t neg_if_negative_68k(int32_t value) {
    if (value >= 0) return value;
    return (int32_t)(UINT32_C(0) - (uint32_t)value);
}

static int32_t add_long_68k(int32_t left, int32_t right) {
    return (int32_t)((uint32_t)left + (uint32_t)right);
}

static void write_word(uint8_t *bytes, uint16_t value) {
    bytes[0] = (uint8_t)(value >> 8);
    bytes[1] = (uint8_t)value;
}

static void write_long(uint8_t *bytes, uint32_t value) {
    bytes[0] = (uint8_t)(value >> 24);
    bytes[1] = (uint8_t)(value >> 16);
    bytes[2] = (uint8_t)(value >> 8);
    bytes[3] = (uint8_t)value;
}

int fa18_build_scene_placement_work(const FA18ScenePlacementWorkInput *input,
                                    int32_t work[3]) {
    int32_t first, third;
    if (!input || !work) return -1;
    first = input->workspace_word[0];
    third = input->workspace_word[1];
    if (!input->append_enabled) {
        first *= 4;
        third *= 4;
    }
    first -= (int32_t)input->correction_word[0] * 4;
    third -= (int32_t)input->correction_word[1] * 4;
    work[0] = first + input->translated_component[0] + input->origin_component[0];
    work[1] = 0; /* `$C1DE04` clears the middle scratch word on this route. */
    work[2] = third + input->translated_component[1] + input->origin_component[1];
    return 0;
}

int fa18_derive_scene_placement_magnitudes(
    const FA18ScenePlacementMagnitudeInput *input, int16_t magnitude[3]) {
    const int descriptor_selected = input && (input->header_flags & 0x50u);
    int32_t first, second, third;

    if (!input || !magnitude) return -1;
    first = add_long_68k(input->coordinate_work[0], input->projection_packet[0]);
    second = add_long_68k(input->coordinate_work[2], input->projection_packet[1]);
    third = input->projection_depth;
    if (descriptor_selected) {
        first = add_long_68k(first, input->descriptor_component[0] & 0x0fffu);
        second = add_long_68k(second, input->descriptor_component[1] & 0x0fffu);
        third = add_long_68k(third, input->descriptor_component[2] & 0x0fffu);
    }
    magnitude[0] = (int16_t)asr_long(neg_if_negative_68k(first), 12);
    magnitude[1] = (int16_t)asr_long(neg_if_negative_68k(second), 12);
    magnitude[2] = (int16_t)asr_long(neg_if_negative_68k(third), 11);
    return 0;
}

int fa18_decode_scene_placement_workspace_header(const uint8_t workspace[2],
                                                 FA18ScenePlacementHeader *header) {
    if (!workspace || !header || (workspace[1] & 0x80u)) return -1;
    header->selector_word = (uint16_t)((uint16_t)workspace[1] << 8 | workspace[0]);
    header->descriptor_index = workspace[1];
    return 0;
}

int fa18_finish_scene_placement_record(
    uint8_t bytes[FA18_SCENE_PLACEMENT_BYTES],
    const FA18ScenePlacementBuilderTailInput *input,
    FA18ScenePlacementBuilderTailResult *result) {
    int16_t maximum;
    uint16_t table_index;
    uint8_t shift;
    uint8_t cycle;

    if (!bytes || !input || !result || !input->shift_table) return -1;
    maximum = input->magnitude[0];
    if (input->magnitude[1] > maximum) maximum = input->magnitude[1];
    if (input->magnitude[2] > maximum) maximum = input->magnitude[2];
    table_index = (uint16_t)maximum >> 1;
    if (table_index > 0xefu || table_index >= input->shift_table_size) return -1;
    shift = input->shift_table[table_index];
    bytes[1] |= shift; /* `$C1DF3C`: OR.B D6,-5(A2). */
    write_word(bytes + 6, (uint16_t)asr_long(input->coordinate_work[0], shift));
    write_word(bytes + 8, (uint16_t)asr_word((int16_t)input->coordinate_work[1], shift));
    write_word(bytes + 10, (uint16_t)asr_long(input->coordinate_work[2], shift));
    write_long(bytes + 12, input->record_tail);
    write_word(bytes + 16, 0);
    cycle = input->cycle_byte;
    bytes[18] = cycle;
    bytes[19] = 0;
    write_long(bytes + 20, 0);
    result->shift_count = shift;
    result->next_cycle_byte = cycle == 0 ? 3u : (uint8_t)(cycle - 1u);
    return 0;
}

int fa18_build_scene_placement_record(const FA18ScenePlacementBuildInput *input,
                                      uint8_t bytes[FA18_SCENE_PLACEMENT_BYTES],
                                      FA18ScenePlacementBuilderTailResult *result) {
    FA18ScenePlacementHeader header;
    FA18ScenePlacementBuilderTailInput tail;
    uint32_t descriptor;
    int32_t work[3];
    if (!input || !bytes || !input->workspace_item || !input->work || !input->resolve ||
        fa18_decode_scene_placement_workspace_header(input->workspace_item, &header) ||
        input->resolve(input->context, header.descriptor_index, &descriptor) ||
        fa18_build_scene_placement_work(input->work, work)) return -1;
    memset(bytes, 0, FA18_SCENE_PLACEMENT_BYTES);
    write_word(bytes, header.selector_word);
    write_long(bytes + 2, descriptor);
    tail = input->tail;
    memcpy(tail.coordinate_work, work, sizeof work);
    return fa18_finish_scene_placement_record(bytes, &tail, result);
}
