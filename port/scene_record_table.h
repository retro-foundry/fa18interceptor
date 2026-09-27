#ifndef FA18_SCENE_RECORD_TABLE_H
#define FA18_SCENE_RECORD_TABLE_H

#include <stdint.h>

#include "hunk.h"

enum {
    FA18_SCENE_RECORD_TABLE_HUNK = 67,
    FA18_SCENE_RECORD_TABLE_A_OFFSET = 0x32,
    FA18_SCENE_RECORD_TABLE_B_OFFSET = 0x84,
    FA18_SCENE_RECORD_TABLE_ENTRY_WORDS = 8,
    FA18_SCENE_RECORD_TABLE_A_ENTRIES = 5
};

typedef struct {
    const uint8_t *table_a;
    const uint8_t *table_b;
} FA18SceneRecordTable;

/* Load `$C42A02` and `$C42A54` from verified Hunk 67. */
int fa18_load_scene_record_table(const FA18Hunks *hunks, FA18SceneRecordTable *table);

/* Decode one 16-byte signed-word record from table A. */
int fa18_scene_record_table_a_entry(const FA18SceneRecordTable *table,
                                    uint8_t index,
                                    int16_t words[FA18_SCENE_RECORD_TABLE_ENTRY_WORDS]);

#endif
