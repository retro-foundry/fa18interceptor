#include "outer_loop_child.h"

#include <assert.h>

typedef struct {
    unsigned waits;
    unsigned loads;
    const uint16_t *palette;
    size_t words;
} OperationLog;

static int wait_viewport(void *context) {
    OperationLog *log = context;
    ++log->waits;
    return 0;
}

static int load_rgb4(void *context, const uint16_t *palette, size_t words) {
    OperationLog *log = context;
    ++log->loads;
    log->palette = palette;
    log->words = words;
    return 0;
}

int main(void) {
    FA18ViewportPaletteBuffer palette = { 0 };
    FA18OuterLoopChildState outer = { 0, 0, 0, &palette };
    FA18ViewportModeState mode = { 15, 15, 2, 3 };
    OperationLog log = { 0 };
    FA18OuterLoopChildOps ops = { wait_viewport, load_rgb4, &log };
    FA18OuterLoopChildStep step;

    assert(fa18_advance_outer_loop_idle_child(&outer, &mode, &ops, &step) == 0);
    assert(mode.state == 2 && step.mode_state_decremented && step.dynamic_palette_loaded);
    assert(outer.selected_index == 1 && step.selected_index_after == 1);
    assert(log.waits == 1 && log.loads == 1 && log.palette == palette.words && log.words == 32);

    mode.state = 3;
    outer.status_word = 0;
    assert(fa18_advance_outer_loop_idle_child(&outer, &mode, NULL, &step) == -1);
    assert(mode.state == 3 && outer.selected_index == 1);

    outer.status_word = 0x0100;
    assert(fa18_advance_outer_loop_idle_child(&outer, &mode, NULL, &step) == 0);
    assert(mode.state == 2 && step.mode_state_decremented && !step.dynamic_palette_loaded);
    assert(outer.selected_index == 0);

    mode.state = 0;
    assert(fa18_advance_outer_loop_idle_child(&outer, &mode, NULL, &step) == 0);
    assert(!step.mode_state_decremented && outer.selected_index == 1);
    outer.activity_counter = 1;
    assert(fa18_advance_outer_loop_idle_child(&outer, &mode, &ops, &step) == -1);
    return 0;
}
