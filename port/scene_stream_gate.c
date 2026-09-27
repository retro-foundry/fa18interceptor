#include "scene_stream_gate.h"

int fa18_select_scene_stream_gate(const FA18SceneStreamGateInput *input,
                                  FA18SceneStreamGateRoute *route) {
    if (!input || !route) return -1;
    if (input->post_stream_flags & 0x40u) {
        *route = FA18_SCENE_STREAM_GATE_PREDECESSOR;
    } else if (input->context_selection ||
               input->control_activity != input->current_record_offset) {
        *route = FA18_SCENE_STREAM_GATE_INDIRECT_STAGE;
    } else {
        *route = FA18_SCENE_STREAM_GATE_C1ED70_CONTINUATION;
    }
    return 0;
}
