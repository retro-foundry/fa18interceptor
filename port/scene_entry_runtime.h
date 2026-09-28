#ifndef FA18_SCENE_ENTRY_RUNTIME_H
#define FA18_SCENE_ENTRY_RUNTIME_H

#include <stdint.h>

#include "message_sequence.h"
#include "scene_dispatch_runtime.h"
#include "scene_finalization.h"
#include "scene_initialization.h"
#include "scene_positive_pose_tables.h"
#include "scene_root_placement.h"
#include "two_angle_matrix.h"

/* Runtime composition of the four ordered `$C0FAA4` helper calls.  The
 * selected mode is the live `$C458A6` value; the initializer publishes the
 * table selector consumed by `$C0924A` before its root-placement callback. */
typedef struct {
    const FA18Hunks *hunks;
    const FA18SceneDispatchTable *dispatch_table;
    const FA18SceneRecordTable *record_table;
    FA18FlightTrigTable trig_table;
    FA18ScenePositivePoseTables positive_tables;
    FA18ScenePositivePoseResolver positive_resolver;
    FA18SceneDispatchRuntime dispatch_runtime;
    FA18SceneRootPlacementState root_placement;
    FA18SceneRootPlacementRoute root_route;
    FA18MessageSequenceState message;
    FA18SceneFinalizationState finalization;
    const FA18SceneInitializationState *initialization_state;
    uint8_t mode;
} FA18SceneEntryRuntime;

int fa18_scene_entry_runtime_init(FA18SceneEntryRuntime *runtime,
                                  const FA18Hunks *hunks,
                                  const FA18SceneDispatchTable *dispatch_table,
                                  const FA18SceneRecordTable *record_table);

/* `$C0FAA4` invokes `$C28722`, `$C0924A`, `$C11312`, then `$C082B0`.
 * `mode` is supplied by the caller-owned post-input state, not a recording. */
int fa18_run_scene_entry_runtime(FA18SceneEntryRuntime *runtime, uint8_t mode,
                                 FA18SceneInitializationState *state,
                                 int16_t *countdown);

#endif
