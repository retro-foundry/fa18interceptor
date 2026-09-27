#include "scene_record_table.h"

#include <assert.h>
#include <string.h>

int main(void) {
    uint8_t bytes[FA18_SCENE_RECORD_TABLE_B_OFFSET + 2] = {0};
    const unsigned offset = FA18_SCENE_RECORD_TABLE_A_OFFSET +
        3u * FA18_SCENE_RECORD_TABLE_ENTRY_WORDS * 2u;
    bytes[offset] = 0x80;
    bytes[offset + 1] = 0x0e;
    bytes[offset + 14] = 0x12;
    bytes[offset + 15] = 0x34;
    FA18HunkSegment segments[FA18_SCENE_RECORD_TABLE_HUNK + 1] = {{0}};
    segments[FA18_SCENE_RECORD_TABLE_HUNK] =
        (FA18HunkSegment){ FA18_HUNK_CODE, bytes, sizeof bytes, NULL, 0 };
    FA18Hunks hunks = { segments, FA18_SCENE_RECORD_TABLE_HUNK + 1 };
    FA18SceneRecordTable table = {0};
    int16_t words[FA18_SCENE_RECORD_TABLE_ENTRY_WORDS];
    assert(fa18_load_scene_record_table(&hunks, &table) == 0);
    assert(fa18_scene_record_table_a_entry(&table, 3, words) == 0);
    assert(words[0] == (int16_t)0x800e && words[7] == 0x1234);
    assert(fa18_scene_record_table_a_entry(&table, 5, words) == -1);
    memset(&hunks, 0, sizeof hunks);
    assert(fa18_load_scene_record_table(&hunks, &table) == -1);
    return 0;
}
