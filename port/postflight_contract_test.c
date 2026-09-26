#include "postflight.h"
#include "run075_frame395_postflight_data.h"

#include <assert.h>

typedef struct {
    FA18PostflightRenderer renderer;
    int16_t x;
    int16_t y;
    unsigned calls;
} Capture;

static int capture(FA18PostflightRenderer renderer, int16_t x, int16_t y,
                   void *context) {
    Capture *out = context;
    out->renderer = renderer;
    out->x = x;
    out->y = y;
    ++out->calls;
    return 0;
}

int main(void) {
    assert(FA18_RUN075_FRAME395_POSTFLIGHT_RECORDS == 32);
    assert(fa18_run075_frame395_postflight[1].x == 158);
    assert(fa18_run075_frame395_postflight[1].y == 167);
    assert(fa18_run075_frame395_postflight[22].flags == 1);
    unsigned adjacent = 0;
    for (unsigned i = 0; i < FA18_RUN075_FRAME395_POSTFLIGHT_RECORDS; ++i) {
        adjacent += (fa18_run075_frame395_postflight[i].flags & 1u) != 0;
    }
    assert(adjacent == 12);
    FA18PostflightScene scene;
    fa18_postflight_scene_init(&scene, 0, 10, 0);
    assert(scene.table_selection == 0 && scene.record_cursor == 0);
    assert(scene.record_limit == 10 && scene.vertical_offset == 0);
    assert(scene.renderer_mode == 0);
    FA18PostflightComponent component = {
        .horizontal_min = 1, .horizontal_max = 0x13d,
        .vertical_base = 0x81, .renderer_mode = 8
    };
    int16_t x = 0, y = 0;
    assert(fa18_postflight_component_coordinates(&component, &x, &y) == 0);
    assert(x == 160 && y == 129 && component.renderer_mode == 8);
    component.horizontal_offset = -200;
    assert(fa18_postflight_component_coordinates(&component, &x, &y) == 1);
    FA18PostflightState state = {.vertical_offset = 10, .table_limit = 2};
    Capture capture_state = {0};
    assert(fa18_postflight_submit(&state, (FA18PostflightRecord){158, 167, 0},
                                  capture, &capture_state) == 0);
    assert(capture_state.renderer == FA18_POSTFLIGHT_SHARED);
    assert(capture_state.x == 158 && capture_state.y == 177);
    assert(fa18_postflight_submit(&state, (FA18PostflightRecord){159, 168, 1},
                                  capture, &capture_state) == 0);
    assert(capture_state.renderer == FA18_POSTFLIGHT_ADJACENT);
    assert(state.submitted == 2);
    assert(fa18_postflight_submit(&state, (FA18PostflightRecord){1, 1, 0},
                                  capture, &capture_state) == 1);
    assert(state.rejected && capture_state.calls == 2);
    return 0;
}
