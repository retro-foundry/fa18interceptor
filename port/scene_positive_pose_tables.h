#ifndef FA18_SCENE_POSITIVE_POSE_TABLES_H
#define FA18_SCENE_POSITIVE_POSE_TABLES_H

#include "hunk.h"
#include "scene_positive_pose.h"
#include "scene_record_table.h"

enum { FA18_SCENE_POSITIVE_TABLE_HUNK = 8, FA18_SCENE_POSITIVE_WORD_OFFSET = 0x151a,
       FA18_SCENE_POSITIVE_BYTE_OFFSET = 0x160e, FA18_SCENE_POSITIVE_WORD_PAIRS = 61,
       FA18_SCENE_POSITIVE_BYTE_PAIRS = 129 };

typedef struct { const uint8_t *word_pairs, *byte_pairs; } FA18ScenePositivePoseTables;
typedef struct { const FA18SceneRecordTable *records; const FA18ScenePositivePoseTables *tables; } FA18ScenePositivePoseResolver;
int fa18_load_scene_positive_pose_tables(const FA18Hunks *, FA18ScenePositivePoseTables *);
int fa18_resolve_scene_positive_pose(void *, uint8_t, FA18ScenePositivePoseInput *);
#endif
