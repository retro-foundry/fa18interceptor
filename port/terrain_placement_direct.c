#include "terrain_placement_direct.h"

#include <string.h>

typedef struct {
    const FA18TerrainPlacementDirectInput *input;
    const FA18ScenePlacementBuilderPrefix *prefix;
    FA18TerrainPlacementEmit emit;
    void *emit_context;
    uint8_t cycle_byte;
    uint16_t emitted_count;
} DirectContext;

static int resolve_descriptor(void *context, uint8_t index, uint32_t *reference) {
    const DirectContext *direct = context;
    if (!direct || !reference || index >= direct->input->descriptor_table_entries)
        return -1;
    *reference = direct->input->descriptor_table_base + (uint32_t)index * 20u;
    return 0;
}

static int resolve_direct_input(void *context, const uint8_t *item, uint8_t ordinal,
                                FA18ScenePlacementBuildInput *build) {
    DirectContext *direct = context;
    FA18ScenePlacementWorkInput work;
    FA18ScenePlacementMagnitudeInput magnitude_input;
    int32_t coordinate_work[3];

    if (!direct || !item || !build || (item[0] & 0x50u) || (item[1] & 0x80u))
        return -1;
    work = (FA18ScenePlacementWorkInput){
        { (int16_t)((uint16_t)item[2] << 8 | item[3]),
          (int16_t)((uint16_t)item[4] << 8 | item[5]) },
        { direct->input->correction_word[0], direct->input->correction_word[1] },
        { direct->prefix->translation_component[0], direct->prefix->translation_component[1] },
        { direct->prefix->workspace_component[0], direct->prefix->workspace_component[1] },
        direct->input->append_enabled
    };
    if (fa18_build_scene_placement_work(&work, coordinate_work) != 0)
        return -1;
    magnitude_input = (FA18ScenePlacementMagnitudeInput){
        { coordinate_work[0], coordinate_work[1], coordinate_work[2] },
        { direct->input->projection_packet[0], direct->input->projection_packet[1] },
        direct->input->projection_depth, { 0, 0, 0 }, 0
    };
    *build = (FA18ScenePlacementBuildInput){
        item, &work,
        { { coordinate_work[0], coordinate_work[1], coordinate_work[2] }, { 0, 0, 0 },
          direct->input->shift_table, direct->input->shift_table_size,
          (uint32_t)ordinal << 16 | direct->prefix->selector_packet,
          direct->cycle_byte },
        resolve_descriptor, direct
    };
    return fa18_derive_scene_placement_magnitudes(&magnitude_input,
                                                  build->tail.magnitude);
}

static int emit_direct_record(void *context,
                              const uint8_t record[FA18_SCENE_PLACEMENT_BYTES]) {
    DirectContext *direct = context;
    if (!direct || direct->emit(direct->emit_context, record) != 0) return -1;
    direct->cycle_byte = direct->cycle_byte == 0 ? 3u : (uint8_t)(direct->cycle_byte - 1u);
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
    if (fa18_emit_workspace_placement_records(result->prefix.workspace_cursor, 0x60,
                                              resolve_direct_input, emit_direct_record,
                                              &direct, &result->cell_end) != 0)
        return -1;
    result->emitted_count = direct.emitted_count;
    result->next_cycle_byte = direct.cycle_byte;
    return 0;
}
