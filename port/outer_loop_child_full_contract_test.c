#include "outer_loop_child.h"

#include <assert.h>

typedef struct {
    unsigned waits, blits, views, loads;
    FA18OuterPagePublication publication;
    const uint16_t *palettes[4];
} Log;

static int wait_viewport(void *context) { ++((Log *)context)->waits; return 0; }
static int wait_blit(void *context) { ++((Log *)context)->blits; return 0; }
static int load_view(void *context, const FA18OuterPagePublication *publication) {
    Log *log = context;
    ++log->views;
    log->publication = *publication;
    return 0;
}
static int load_rgb4(void *context, const uint16_t *palette, size_t words) {
    Log *log = context;
    assert(words == FA18_OUTER_LOOP_CHILD_RGB4_WORDS && log->loads < 4);
    log->palettes[log->loads++] = palette;
    return 0;
}

int main(void) {
    const uint32_t table_1[] = { 0xc074d8u, 0xc074e8u };
    const uint32_t table_2[] = { 0xc07f00u, 0xc07f10u };
    uint16_t static_palette[32] = { 1 };
    FA18ViewportPaletteBuffer dynamic_palette = { { 2 } };
    FA18OuterLoopChildState outer = { 2, 0, 0, &dynamic_palette };
    FA18ViewportModeState mode = { 15, 15, 0, 0 };
    FA18OuterLoopChildStep step;
    Log log = { 0 };
    FA18OuterLoopChildOps ops = {
        wait_viewport, load_rgb4, &log, wait_blit, load_view, static_palette
    };

    assert(fa18_run_outer_loop_child(&outer, &mode, table_1, table_2, 2,
                                     &ops, &step) == 0);
    assert(step.prefix_waited && step.display_view_loaded && step.activity_loop_completed);
    assert(step.publication.selected_pointer_1 == 0xc074d8u &&
           step.publication.selected_pointer_2 == 0xc07f00u);
    assert(log.views == 1 && log.blits == 1 && log.waits == 10 && log.loads == 4);
    assert(log.palettes[0] == static_palette && log.palettes[1] == dynamic_palette.words &&
           log.palettes[2] == static_palette && log.palettes[3] == dynamic_palette.words);
    assert(outer.activity_counter == 0 && outer.selected_index == 1 &&
           step.selected_index_after == 1);

    outer = (FA18OuterLoopChildState){ 0, 1, 0x0100, &dynamic_palette };
    assert(fa18_run_outer_loop_child(&outer, &mode, table_1, table_2, 2,
                                     &ops, &step) == 0);
    assert(step.prefix_waited && step.display_view_loaded && !step.activity_loop_completed &&
           outer.selected_index == 0);
    return 0;
}
