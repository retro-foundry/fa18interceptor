#include "map_packet_polygon_display.h"

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
    const FA18MapPacketProjectionRecord records[] = {
        {{2383,-8,1982}}, {{1030,-8,1869}}, {{1022,-8,1829}},
        {{737,-8,1708}}, {{606,-8,1790}}, {{633,-8,1855}},
        {{435,-8,1848}}, {{274,-8,1739}}, {{554,-8,1789}},
        {{236,-8,1682}}, {{-337,-8,1311}}, {{-241,-8,-213}},
        {{2515,-8,-114}}
    };
    Calls calls = {0};
    const FA18ProjectionPairSubmission submission = {
        144, 89, 0, 0x10000, 0, 0, dma, &calls, line, 0,
        protected_line, 0, blit, final, &calls
    };
    FA18PolygonDisplayPipelineResult result;
    FA18MapPacketPolygonDisplay display = {&submission, &result};

    assert(fa18_display_map_packet_polygon(&display, records, 13, 2) == 0);
    assert(result.clipped_count == 14 && result.pair_count == 14);
    assert(result.pairs[0].x == 0 && result.pairs[0].y == 89 &&
           result.pairs[11].x == 319 && result.pairs[11].y == 91 &&
           result.pairs[13].x == 0 && result.pairs[13].y == 179);
    assert(calls.dma == 1 && calls.blits && calls.finals == 1);
    return 0;
}
