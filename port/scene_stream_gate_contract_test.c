#include "scene_stream_gate.h"

#include <assert.h>

int main(void) {
    FA18SceneStreamGateInput input = {0, 0, 0x42, 0x42};
    FA18SceneStreamGateRoute route;
    assert(fa18_select_scene_stream_gate(&input, &route) == 0);
    assert(route == FA18_SCENE_STREAM_GATE_C1ED70_CONTINUATION);
    input.context_selection = 1;
    assert(fa18_select_scene_stream_gate(&input, &route) == 0);
    assert(route == FA18_SCENE_STREAM_GATE_INDIRECT_STAGE);
    input.context_selection = 0;
    input.current_record_offset = 0x43;
    assert(fa18_select_scene_stream_gate(&input, &route) == 0);
    assert(route == FA18_SCENE_STREAM_GATE_INDIRECT_STAGE);
    input.post_stream_flags = 0x40;
    assert(fa18_select_scene_stream_gate(&input, &route) == 0);
    assert(route == FA18_SCENE_STREAM_GATE_PREDECESSOR);
    assert(fa18_select_scene_stream_gate(0, &route) == -1);
    return 0;
}
