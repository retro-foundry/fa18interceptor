#include "polygon_display_pipeline.h"

#include <assert.h>

typedef struct { unsigned dma, blits, finals; } Calls;
static int dma(void *context, uint16_t value) {
    Calls *calls = context;
    return calls && value == 0x8400 ? (++calls->dma, 0) : -1;
}
static int line(void *context, int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                int16_t bound) {
    (void)context; (void)x0; (void)y0; (void)x1; (void)y1; (void)bound;
    return 0;
}
static int protected_line(void *context, int16_t x0, int16_t y0, int16_t x1,
                          int16_t y1, uint32_t scratch) {
    (void)context; (void)x0; (void)y0; (void)x1; (void)y1; (void)scratch;
    return 0;
}
static int blit(void *context, const FA18ProjectionPairBlitterWrites *writes) {
    Calls *calls = context;
    return calls && writes ? (++calls->blits, 0) : -1;
}
static int final(void *context, const FA18ProjectionPairFinalState *state) {
    Calls *calls = context;
    return calls && state ? (++calls->finals, 0) : -1;
}

int main(void) {
    const FA18ClipTuple input[] = {
        {9532,-32,7928}, {4120,-32,7476}, {4088,-32,7316},
        {2948,-32,6832}, {2424,-32,7160}, {2532,-32,7420},
        {1740,-32,7392}, {1096,-32,6956}, {2216,-32,7156},
        {944,-32,6728}, {-1348,-32,5244}, {-964,-32,-852},
        {10060,-32,-456}
    };
    Calls calls = {0};
    const FA18ProjectionPairSubmission submission = {
        144, 89, 0, 0x10000, 0, 0, dma, &calls, line, 0,
        protected_line, 0, blit, final, &calls
    };
    FA18PolygonDisplayPipelineResult result;

    assert(fa18_run_polygon_display_pipeline(input, 13, 0, &submission, &result) == 1);
    assert(result.clipped_count == 14 && result.pair_count == 14);
    assert(result.pairs[0].x == 0 && result.pairs[0].y == 89 &&
           result.pairs[11].x == 319 && result.pairs[11].y == 91 &&
           result.pairs[13].x == 0 && result.pairs[13].y == 179);
    assert(calls.dma == 1 && calls.blits && calls.finals == 1);
    return 0;
}
