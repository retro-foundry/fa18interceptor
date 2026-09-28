#include <assert.h>

#include "workspace_segment_projection.h"

typedef struct {
    unsigned calls;
    FA18ScreenPoint endpoints[2];
} Capture;

static int capture_line(void *context, int16_t x0, int16_t y0,
                        int16_t x1, int16_t y1) {
    Capture *capture = context;
    ++capture->calls;
    capture->endpoints[0] = (FA18ScreenPoint){x0, y0};
    capture->endpoints[1] = (FA18ScreenPoint){x1, y1};
    return 0;
}

int main(void) {
    Capture capture = {0};
    const FA18ViewVertex visible[2] = {{0, 0, 10}, {10, 10, 20}};
    const FA18ViewVertex rejected_depth[2] = {{0, 0, 0}, {0, 0, 1}};
    const FA18ViewVertex rejected_extent[2] = {{11, 0, 10}, {0, 0, 1}};

    assert(fa18_project_workspace_segment_to_line(visible, capture_line, &capture) == 1);
    assert(capture.calls == 1);
    assert(capture.endpoints[0].x == 159 && capture.endpoints[0].y == 89);
    assert(capture.endpoints[1].x == 79 && capture.endpoints[1].y == 44);
    assert(fa18_project_workspace_segment_to_line(rejected_depth, capture_line,
                                                   &capture) == 0);
    assert(fa18_project_workspace_segment_to_line(rejected_extent, capture_line,
                                                   &capture) == 0);
    assert(capture.calls == 1);
    return 0;
}
