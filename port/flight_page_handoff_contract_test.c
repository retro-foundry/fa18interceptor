#include "flight_page_handoff.h"

#include <assert.h>

typedef struct { unsigned waits, blits; } Log;

static int wait_viewport(void *context) { ++((Log *)context)->waits; return 0; }
static int wait_blit(void *context) { ++((Log *)context)->blits; return 0; }

int main(void) {
    FA18FlightPageHandoff handoff;
    const FA18FlightPageViewPair view_pair[2] = {{1, 3}, {2, 4}};
    FA18ViewportPaletteBuffer dynamic_palette = {{0}};
    FA18PlanarPixelState pixels = {0, 0, 0, 0};
    FA18LineStyle lines = {0, 0, 0, 0};
    FA18ViewportModeState mode = {0, 0, 0, 1};
    FA18Video video = {{0}};
    FA18OuterLoopChildStep step;
    uint16_t static_palette[FA18_OUTER_LOOP_CHILD_RGB4_WORDS] = {0};
    Log log = {0};
    const FA18FlightPageHandoffOps ops = {
        wait_viewport, wait_blit, static_palette, &log
    };

    dynamic_palette.words[1] = 0x0123;
    assert(fa18_initialize_flight_page_handoff(
               &handoff, view_pair, &dynamic_palette, &pixels, &lines,
               179, 111, 106, 0, 0, 0) == 0);
    assert(handoff.chip_binding[0].plane_pointers[0] == 0 &&
           handoff.chip_binding[0].plane_pointers[4] == 32000 &&
           handoff.chip_binding[1].plane_pointers[0] == 0 &&
           handoff.chip_binding[1].plane_pointers[4] == 32000);
    assert(fa18_flight_page_handoff_bind_page_blitter(
               &handoff, 0,
               &(FA18BlitOperation){.bltafwm = 0xffff, .bltalwm = 0xffff}) == 0);
    assert(handoff.renderer[0].triangle_submission.blitter_emitter ==
               fa18_projection_page_emit_blitter &&
           handoff.renderer[0].triangle_submission.final_emitter ==
               fa18_projection_page_emit_final);
    assert(fa18_flight_page_handoff_bind_page_blitter(&handoff, 2,
                                                       &(FA18BlitOperation){0}) == -1);
    assert(fa18_flight_page_handoff_selected_renderer(&handoff) == &handoff.renderer[0]);
    handoff.page[0].planes[0][0] = 0x80;
    assert(fa18_run_flight_page_handoff_child(&handoff, &mode, &ops, &video,
                                               &step) == 0);
    assert(step.publication.selected_index == 0 && handoff.visible_index == 0 &&
           handoff.outer_child.selected_index == 1 && mode.state == 0 &&
           handoff.page[0].display_state.palette[1] == 0x0123 &&
           video.pixels[0] == 1 && video.palette[1] == 0x0123 &&
           log.waits == 2 && !log.blits);
    assert(fa18_flight_page_handoff_selected_renderer(&handoff) == &handoff.renderer[1]);
    return 0;
}
