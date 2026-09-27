#ifndef FA18_ALTERNATE_RECORD_LOOP_H
#define FA18_ALTERNATE_RECORD_LOOP_H

#include "scene_descriptor_stage.h"

typedef enum { FA18_ALTERNATE_RECORD_LOOP_SKIPPED, FA18_ALTERNATE_RECORD_LOOP_DISPATCHED } FA18AlternateRecordLoopRoute;
typedef struct {
    uint16_t header, selected_kind, next_offset;
    uint8_t scene_active, refresh_byte;
    FA18SceneDescriptorStageRecord descriptor;
    FA18SceneDescriptorStageCall call;
    void *call_context;
} FA18AlternateRecordLoopInput;
typedef struct { uint16_t record_value, fixed_value, kind, next_offset; uint8_t record_byte; FA18SceneDescriptorStageState descriptor; } FA18AlternateRecordLoopState;
/* `$C1CF36-$C1CFC9`, ending at the next `$C1CE42` record preparation. */
int fa18_finish_alternate_record_loop(FA18ScenePlacementRecord *record, const FA18AlternateRecordLoopInput *input, FA18AlternateRecordLoopState *state, FA18AlternateRecordLoopRoute *route);
#endif
