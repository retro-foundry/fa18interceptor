#include "scene_dispatch_table.h"

#include <stddef.h>

static int word_at(const FA18SceneDispatchTable *table, uint32_t offset,
                   int16_t *value) {
    if (offset > table->size || table->size - offset < 2u) return -1;
    *value = (int16_t)fa18_be16(table->data + offset);
    return 0;
}

int fa18_load_scene_dispatch_table(const FA18Hunks *hunks,
                                   FA18SceneDispatchTable *table) {
    if (!hunks || !table || FA18_SCENE_DISPATCH_TABLE_HUNK >= hunks->count)
        return -1;
    const FA18HunkSegment *segment = &hunks->segments[FA18_SCENE_DISPATCH_TABLE_HUNK];
    if (!segment->data || segment->size < FA18_SCENE_DISPATCH_TABLE_OFFSET + 2u)
        return -1;
    table->segment = segment->data;
    table->segment_size = segment->size;
    table->data = segment->data + FA18_SCENE_DISPATCH_TABLE_OFFSET;
    table->size = segment->size - FA18_SCENE_DISPATCH_TABLE_OFFSET;
    return 0;
}

int fa18_select_scene_dispatch_records(
    const FA18SceneDispatchTable *table,
    const FA18SceneDispatchSelectionInput *input,
    FA18SceneDispatchSelection *selection) {
    if (!table || !table->data || !input || !selection) return -1;
    if (input->mode == 0x7d) return 1;

    uint16_t mode = input->mode;
    if (mode == 0x7e || mode == 0x7f) mode = 10;
    if (mode == 0 || mode > 10) return -1;

    uint16_t variant;
    if (input->alternate_table) {
        variant = input->alternate_variant;
    } else {
        variant = (uint16_t)(input->phase_word & 3u);
        if (variant) --variant;
    }
    const uint32_t entry = (uint32_t)(mode - 1u) * FA18_SCENE_DISPATCH_MODE_BYTES +
                           (uint32_t)variant * FA18_SCENE_DISPATCH_VARIANT_BYTES;
    int16_t list_block_offset, record_type_limit, list_offset;
    if (word_at(table, entry, &list_block_offset) != 0 ||
        word_at(table, entry + 2u, &record_type_limit) != 0)
        return -1;

    const int64_t block = list_block_offset;
    if (block < 0 || (uint64_t)block >= table->size) return -1;
    if (word_at(table, (uint32_t)block, &list_offset) != 0) return -1;
    const int16_t advance = (input->mode == 0x7e || input->mode == 0x7f)
                                ? 0 : (int16_t)(input->mode_list_offset * 2);
    const int64_t records = block + list_offset + advance;
    if (records < 0 || (uint64_t)records >= table->size) return -1;

    selection->records = table->data + (uint32_t)records;
    selection->record_type_limit = (uint8_t)record_type_limit;
    return 0;
}

int fa18_scene_dispatch_source_record(
    const FA18SceneDispatchTable *table, const uint8_t *records,
    uint16_t index, FA18SceneDispatchSourceRecord *record) {
    if (!table || !table->data || !records || !record) return -1;
    const uintptr_t table_start = (uintptr_t)table->data;
    const uintptr_t records_start = (uintptr_t)records;
    if (records_start < table_start || records_start - table_start >= table->size) return -1;
    const uint64_t offset = (uint64_t)(records_start - table_start) +
                            (uint64_t)index * FA18_SCENE_DISPATCH_RECORD_BYTES;
    if (offset > table->size || table->size - offset < FA18_SCENE_DISPATCH_RECORD_BYTES)
        return -1;
    const uint8_t *source = table->data + (uint32_t)offset;
    record->source_offset = (int16_t)fa18_be16(source);
    if (record->source_offset < 0) return 0;
    record->record_type = (uint8_t)fa18_be16(source + 2);
    record->source_word_1 = fa18_be16(source + 4);
    record->geometry_offset = fa18_be16(source + 6);
    record->source_flags = fa18_be16(source + 8);
    return 1;
}

int fa18_scene_dispatch_geometry(const FA18SceneDispatchTable *table,
                                 uint16_t offset,
                                 FA18SceneDispatchGeometry *geometry) {
    if (!table || !table->segment || !geometry ||
        offset > table->segment_size || table->segment_size - offset < 12u)
        return -1;
    const uint8_t *source = table->segment + offset;
    geometry->coordinate_x = (int16_t)fa18_be16(source);
    geometry->coordinate_z = (int16_t)fa18_be16(source + 2);
    geometry->component_x = (int16_t)fa18_be16(source + 4);
    geometry->component_z = (int16_t)fa18_be16(source + 6);
    geometry->altitude = (int32_t)fa18_be32(source + 8);
    return 0;
}
