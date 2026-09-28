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
    /* First `$C2EE4A` pair in the frame-602 `$C212B0` P-code trace. It takes
     * `$C2EE94 -> $C2EF58 -> $C2EF88 -> $C2F042` for both endpoints. */
    FA18ProjectedSegmentPreparationState state = {{5605, 1703, 5605}, 0};
    const FA18ViewVertex endpoints[2] = {{-205, 570, 2334}, {-59, 570, 2334}};
    Capture result = {0};

    assert(fa18_prepare_projected_segment(&state, endpoints, capture, &result) == 1);
    /* `$C2F088` loads these endpoint words before its `$C2FA7E` call. */
    assert(result.calls == 1 && result.x0 == 173 && result.y0 == 68);
    assert(result.x1 == 163 && result.y1 == 68);
    return 0;
}
