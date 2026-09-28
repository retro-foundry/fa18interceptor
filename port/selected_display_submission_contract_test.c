#include "selected_display_submission.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef struct { int blits; int finals; } Adapter;
static int emit_blit(void *context, const FA18ProjectionPairBlitterWrites *writes) {
    Adapter *adapter = context; return adapter && writes ? (++adapter->blits, 0) : -1;
}
static int emit_final(void *context, const FA18ProjectionPairFinalState *state) {
    Adapter *adapter = context; return adapter && state ? (++adapter->finals, 0) : -1;
}
static int emit_line(void *context, int16_t a, int16_t b, int16_t c, int16_t d, int16_t y) {
    (void)context; (void)a; (void)b; (void)c; (void)d; (void)y; return 0;
}
static int emit_protected(void *context, int16_t a, int16_t b, int16_t c, int16_t d,
                          uint32_t scratch) {
    (void)context; (void)a; (void)b; (void)c; (void)d; (void)scratch; return 0;
}
int main(void) {
    FA18DisplayRecordSelectorOutput selection;
    Adapter adapter = {0};
    FA18ProjectionPairBoundsRoute route;
    FA18ProjectionPairFinalizationRoute finalization;
    const FA18ProjectionPairSubmission submission = {
        179, 125, 81, 0x6048, 0, 0,
        0, 0, emit_line, 0, emit_protected, 0, emit_blit, emit_final, &adapter
    };
    memset(&selection, 0, sizeof selection);
    selection.selection_flag = 1; selection.words[0] = 4;
    selection.words[1] = 0; selection.words[2] = 89;
    selection.words[3] = 319; selection.words[4] = 89;
    selection.words[5] = 319; selection.words[6] = 0;
    selection.words[7] = 0; selection.words[8] = 0;
    assert(fa18_submit_selected_display_record_list(&selection, &submission,
                                                     &route, &finalization) == 0);
    assert(route == FA18_PROJECTION_PAIR_BOUNDS_C302DE_CONTINUATION);
    assert(finalization == FA18_PROJECTION_PAIR_FINALIZATION_SUBMITTED);
    assert(adapter.blits && adapter.finals == 1);
    selection.selection_flag = 0;
    assert(fa18_submit_selected_display_record_list(&selection, &submission,
                                                     &route, &finalization) == -1);
    puts("selected display submission contract passed");
    return 0;
}
