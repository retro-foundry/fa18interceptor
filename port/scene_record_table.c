#include "scene_record_table.h"

int fa18_load_scene_record_table(const FA18Hunks *hunks, FA18SceneRecordTable *table) {
    if (!hunks || !table || FA18_SCENE_RECORD_TABLE_HUNK >= hunks->count) return -1;
    const FA18HunkSegment *segment = &hunks->segments[FA18_SCENE_RECORD_TABLE_HUNK];
    if (!segment->data || segment->size < FA18_SCENE_RECORD_TABLE_B_OFFSET + 2u) return -1;
    table->table_a = segment->data + FA18_SCENE_RECORD_TABLE_A_OFFSET;
    table->table_b = segment->data + FA18_SCENE_RECORD_TABLE_B_OFFSET;
    return 0;
}

int fa18_scene_record_table_a_entry(const FA18SceneRecordTable *table,
                                    uint8_t index,
                                    int16_t words[FA18_SCENE_RECORD_TABLE_ENTRY_WORDS]) {
    if (!table || !table->table_a || !words || index >= FA18_SCENE_RECORD_TABLE_A_ENTRIES)
        return -1;
    const uint8_t *source = table->table_a +
        (uint32_t)index * FA18_SCENE_RECORD_TABLE_ENTRY_WORDS * 2u;
    for (unsigned word = 0; word < FA18_SCENE_RECORD_TABLE_ENTRY_WORDS; ++word)
        words[word] = (int16_t)fa18_be16(source + word * 2u);
    return 0;
}
