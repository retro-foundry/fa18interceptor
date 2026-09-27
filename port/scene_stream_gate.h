#ifndef FA18_SCENE_STREAM_GATE_H
#define FA18_SCENE_STREAM_GATE_H

#include <stdint.h>

typedef struct {
    uint8_t post_stream_flags;
    uint8_t context_selection;
    uint16_t control_activity;
    uint16_t current_record_offset;
} FA18SceneStreamGateInput;

typedef enum {
    FA18_SCENE_STREAM_GATE_PREDECESSOR = 0,
    FA18_SCENE_STREAM_GATE_INDIRECT_STAGE = 1,
    FA18_SCENE_STREAM_GATE_C1ED70_CONTINUATION = 2
} FA18SceneStreamGateRoute;

/* `$C1ED4C-$C1ED6F`: select the entry path into the record-stream stage. */
int fa18_select_scene_stream_gate(const FA18SceneStreamGateInput *input,
                                  FA18SceneStreamGateRoute *route);

#endif
