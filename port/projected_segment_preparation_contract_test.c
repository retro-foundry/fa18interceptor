#include <assert.h>

#include "projected_segment_preparation.h"

typedef struct { unsigned calls; int16_t x0, y0, x1, y1; } Capture;

static int capture(void *context, int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
    Capture *result = context;
    ++result->calls;
    result->x0 = x0; result->y0 = y0; result->x1 = x1; result->y1 = y1;
    return 0;
}

int main(void) {
    FA18ProjectedSegmentPreparationState state = {{0, 0, 0}, -1};
    const FA18ViewVertex endpoints[2] = {{10, 0, 10}, {0, 0, 20}};
    Capture result = {0};

    assert(fa18_prepare_projected_segment(&state, endpoints, capture, &result) == 1);
    assert(result.calls == 1 && result.x0 == 0 && result.y0 == 89);
    assert(result.x1 == 0 && result.y1 == 89);
    return 0;
}
