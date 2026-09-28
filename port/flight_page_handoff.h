#ifndef FA18_FLIGHT_PAGE_HANDOFF_H
#define FA18_FLIGHT_PAGE_HANDOFF_H

#include <stdint.h>

#include "flight_renderer_page.h"
#include "outer_loop_child.h"

/* Caller-owned identities for the two View/ViewPort display publications.
 * They are native keys, not imported Amiga addresses. */
typedef struct {
    uint32_t view_pointer;
    uint32_t display_instruction_pointer;
} FA18FlightPageViewPair;

typedef struct {
    FA18FivePlanePage page[2];
    FA18FlightRendererPage renderer[2];
    FA18OuterLoopChildState outer_child;
    FA18FlightPageViewPair view_pair[2];
    uint16_t visible_index;
} FA18FlightPageHandoff;

typedef struct {
    FA18OuterLoopWaitViewport wait_viewport;
    FA18OuterLoopWaitBlit wait_blit;
    const uint16_t *static_palette;
    void *context;
} FA18FlightPageHandoffOps;

int fa18_initialize_flight_page_handoff(
    FA18FlightPageHandoff *handoff, const FA18FlightPageViewPair view_pair[2],
    FA18ViewportPaletteBuffer *dynamic_palette,
    const FA18PlanarPixelState *pixel_state, const FA18LineStyle *line_style,
    int16_t display_bound_y, int16_t vertical_value, int16_t horizontal_value,
    uint32_t renderer_base_long, uint8_t mode_flag, uint32_t saved_line_scratch);

/* `$C2F558` selection view. The outer child tail alone changes its index. */
FA18FlightRendererPage *fa18_flight_page_handoff_selected_renderer(
    FA18FlightPageHandoff *handoff);

/* `$C1612C-$C16283` against native pages: the published pair selects the
 * visible page, its RGB4 loads target that page, then it is presented. */
int fa18_run_flight_page_handoff_child(
    FA18FlightPageHandoff *handoff, FA18ViewportModeState *viewport_mode,
    const FA18FlightPageHandoffOps *ops, FA18Video *video,
    FA18OuterLoopChildStep *step);

#endif
