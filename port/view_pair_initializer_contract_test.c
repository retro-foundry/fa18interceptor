#include "view_pair_initializer.h"

#include <assert.h>

typedef struct {
    unsigned calls;
    uint32_t expected_source;
    uint32_t display_result;
    uint32_t view_result;
} Fixture;

static int build_display(void *context, uint32_t source, uint32_t *result) {
    Fixture *fixture = context;
    if (!fixture || !result || source != fixture->expected_source || fixture->calls != 0)
        return -1;
    ++fixture->calls;
    *result = fixture->display_result;
    return 0;
}

static int build_view(void *context, uint32_t source, uint32_t display,
                      uint32_t *result) {
    Fixture *fixture = context;
    if (!fixture || !result || source != fixture->expected_source ||
        display != fixture->display_result || fixture->calls != 1)
        return -1;
    ++fixture->calls;
    *result = fixture->view_result;
    return 0;
}

int main(void) {
    FA18ViewPairInitializerState state = {0};
    Fixture fixture = {0, 11, 31, 21};
    FA18ViewPairInitializerOps ops = {build_display, build_view, &fixture};

    assert(fa18_initialize_view_pair_slot(&state, 0, 11, &ops) == 0);
    assert(fixture.calls == 2 && state.active_raster_source == 11 &&
           state.live_pair.view_pointer == 21 &&
           state.live_pair.display_instruction_pointer == 31 &&
           state.pair[0].view_pointer == 21 &&
           state.pair[0].display_instruction_pointer == 31);
    fixture = (Fixture){0, 12, 41, 51};
    assert(fa18_initialize_view_pair_slot(&state, 1, 12, &ops) == 0);
    assert(fixture.calls == 2 && state.pair[0].view_pointer == 21 &&
           state.pair[1].view_pointer == 51 &&
           state.pair[1].display_instruction_pointer == 41);
    assert(fa18_initialize_view_pair_slot(&state, 2, 12, &ops) == -1);
    return 0;
}
