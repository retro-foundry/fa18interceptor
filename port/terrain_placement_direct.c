#include "terrain_placement_direct.h"

#include <string.h>

typedef struct {
    const FA18TerrainPlacementDirectInput *input;
    const FA18ScenePlacementBuilderPrefix *prefix;
    FA18TerrainPlacementEmit emit;
    void *emit_context;
    uint8_t cycle_byte;
    uint16_t emitted_count;
    uint16_t record_tail_word;
    uint8_t current_flags;
} DirectContext;

static uint16_t read_word(const uint8_t *bytes) {
    return (uint16_t)((uint16_t)bytes[0] << 8 | bytes[1]);
}

static uint32_t flagged_record_tail(uint8_t value) {
    return UINT32_C(0x80000000) | (uint32_t)value << 24;
}

static int resolve_descriptor(DirectContext *direct, const uint8_t *item,
                              uint32_t *reference, uint16_t *component_index,
                              uint8_t **bit4_record) {
    uint8_t index;

    if (!direct || !item || !reference || !component_index || !bit4_record)
        return -1;
    index = item[1];
    if (index >= direct->input->descriptor_table_entries) return -1;
    *bit4_record = NULL;
    if (item[0] & 0x40u) {
        size_t offset = (size_t)index * 32u;
        if (!direct->input->bit6_records ||
            offset > direct->input->bit6_records_size ||
            direct->input->bit6_records_size - offset < 0x12u)
            return -1;
        *reference = direct->input->descriptor_table_base +
                     (uint32_t)read_word(direct->input->bit6_records + offset + 2u) * 20u;
        *component_index = index;
    } else {
        *reference = direct->input->descriptor_table_base + (uint32_t)index * 20u;
        *component_index = index;
        if (item[0] & 0x10u) {
            size_t offset = (size_t)index * 512u;
            if (!direct->input->bit4_records ||
                offset > direct->input->bit4_records_size ||
                direct->input->bit4_records_size - offset < 0x12u)
                return -1;
            *bit4_record = direct->input->bit4_records + offset;
        }
    }
    return 0;
}

static int resolve_direct_input(void *context, const uint8_t *item, uint8_t ordinal,
                                FA18ScenePlacementBuildInput *build) {
    DirectContext *direct = context;
    FA18ScenePlacementWorkInput work;
    FA18ScenePlacementMagnitudeInput magnitude_input;
    int32_t coordinate_work[3];
    uint8_t *bit4_record;
    uint16_t component_index;
    uint32_t descriptor_reference;
    const uint8_t *descriptor_record = NULL;
    uint8_t flags;

    if (!direct || !item || !build || (item[1] & 0x80u))
        return -1;
    flags = item[0] & 0x50u;
    if (resolve_descriptor(direct, item, &descriptor_reference, &component_index,
                           &bit4_record) != 0)
        return -1;
    work = (FA18ScenePlacementWorkInput){
        { flags ? 0 : (int16_t)((uint16_t)item[2] << 8 | item[3]),
          flags ? 0 : (int16_t)((uint16_t)item[4] << 8 | item[5]) },
        { direct->input->correction_word[0], direct->input->correction_word[1] },
        { direct->prefix->translation_component[0], direct->prefix->translation_component[1] },
        { direct->prefix->workspace_component[0], direct->prefix->workspace_component[1] },
        direct->input->append_enabled
    };
    if (fa18_build_scene_placement_work(&work, coordinate_work) != 0)
        return -1;
    if (flags) coordinate_work[2] = direct->input->retained_third_work;
    if (flags & 0x40u)
        descriptor_record = direct->input->bit6_records + (size_t)component_index * 32u;
    else if (bit4_record) {
        if (!direct->input->descriptor_record_mode) bit4_record[1] &= (uint8_t)~0x04u;
        descriptor_record = bit4_record;
    }
    magnitude_input = (FA18ScenePlacementMagnitudeInput){
        { coordinate_work[0], coordinate_work[1], coordinate_work[2] },
        { direct->input->projection_packet[0], direct->input->projection_packet[1] },
        direct->input->projection_depth,
        { descriptor_record ? read_word(descriptor_record + 0x0cu) : 0,
          descriptor_record ? read_word(descriptor_record + 0x0eu) : 0,
          descriptor_record ? read_word(descriptor_record + 0x10u) : 0 }, flags
    };
    *build = (FA18ScenePlacementBuildInput){
        item, &work,
        { { coordinate_work[0], coordinate_work[1], coordinate_work[2] }, { 0, 0, 0 },
          direct->input->shift_table, direct->input->shift_table_size,
          flags ? flagged_record_tail(direct->input->flagged_tail_byte) :
                  (uint32_t)ordinal << 16 | direct->record_tail_word,
          direct->cycle_byte },
        NULL, direct, 1, descriptor_reference
    };
    if (fa18_derive_scene_placement_magnitudes(&magnitude_input,
                                               build->tail.magnitude) != 0)
        return -1;
    direct->current_flags = flags;
    return 0;
}

static int emit_direct_record(void *context,
                              const uint8_t record[FA18_SCENE_PLACEMENT_BYTES]) {
    DirectContext *direct = context;
    if (!direct || direct->emit(direct->emit_context, record) != 0) return -1;
    direct->cycle_byte = direct->cycle_byte == 0 ? 3u : (uint8_t)(direct->cycle_byte - 1u);
    direct->record_tail_word = direct->current_flags ?
        (uint16_t)(direct->input->flagged_tail_byte | 0x80u) : direct->emitted_count + 1u;
    ++direct->emitted_count;
    return 0;
}

int fa18_emit_direct_prefixed_placement_records(
    const FA18ScenePlacementBuilderPrefixInput *prefix_input,
    const FA18TerrainPlacementDirectInput *input,
    FA18TerrainPlacementEmit emit, void *context,
    FA18TerrainPlacementDirectResult *result) {
    DirectContext direct;

    if (!input || !emit || !result || !input->shift_table ||
        !input->descriptor_table_entries ||
        fa18_prepare_scene_placement_builder_prefix(prefix_input, &result->prefix) != 0)
        return -1;
    memset(&direct, 0, sizeof direct);
    direct.input = input;
    direct.prefix = &result->prefix;
    direct.emit = emit;
    direct.emit_context = context;
    direct.cycle_byte = input->cycle_byte;
    direct.record_tail_word = (uint16_t)result->prefix.record_tail_word;
    if (fa18_emit_workspace_placement_records(result->prefix.workspace_cursor, 0x60,
                                              resolve_direct_input, emit_direct_record,
                                              &direct, &result->cell_end) != 0)
        return -1;
    result->emitted_count = direct.emitted_count;
    result->next_cycle_byte = direct.cycle_byte;
    result->next_record_tail_word = direct.record_tail_word;
    return 0;
}
