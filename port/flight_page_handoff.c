#include "flight_page_handoff.h"

#include <string.h>

typedef struct {
    FA18FlightPageHandoff *handoff;
    const FA18FlightPageHandoffOps *ops;
} HandoffContext;

static int wait_viewport(void *context) {
    HandoffContext *handoff = context;
    return handoff && handoff->ops && handoff->ops->wait_viewport ?
               handoff->ops->wait_viewport(handoff->ops->context) : -1;
}

static int wait_blit(void *context) {
    HandoffContext *handoff = context;
    return handoff && handoff->ops && handoff->ops->wait_blit ?
               handoff->ops->wait_blit(handoff->ops->context) : -1;
}

static int load_rgb4(void *context, const uint16_t *palette, size_t word_count) {
    HandoffContext *handoff = context;
    if (!handoff || !handoff->handoff || handoff->handoff->visible_index > 1)
        return -1;
    return fa18_five_plane_page_load_rgb4(
        &handoff->handoff->page[handoff->handoff->visible_index], palette, word_count);
}

static int load_view(void *context, const FA18OuterPagePublication *publication) {
    HandoffContext *context_state = context;
    FA18FlightPageHandoff *handoff;

    if (!context_state || !publication || !(handoff = context_state->handoff) ||
        publication->selected_index > 1 ||
        publication->selected_pointer_1 !=
            handoff->view_pair[publication->selected_index].view_pointer ||
        publication->selected_pointer_2 !=
            handoff->view_pair[publication->selected_index].display_instruction_pointer)
        return -1;
    handoff->visible_index = publication->selected_index;
    return 0;
}

int fa18_initialize_flight_page_handoff(
    FA18FlightPageHandoff *handoff, const FA18FlightPageViewPair view_pair[2],
    FA18ViewportPaletteBuffer *dynamic_palette,
    const FA18PlanarPixelState *pixel_state, const FA18LineStyle *line_style,
    int16_t display_bound_y, int16_t vertical_value, int16_t horizontal_value,
    uint32_t renderer_base_long, uint8_t mode_flag, uint32_t saved_line_scratch) {
    if (!handoff || !view_pair || !dynamic_palette || !pixel_state || !line_style)
        return -1;
    memset(handoff, 0, sizeof *handoff);
    memcpy(handoff->view_pair, view_pair, sizeof handoff->view_pair);
    handoff->outer_child.dynamic_palette = dynamic_palette;
    for (unsigned index = 0; index < 2; ++index) {
        fa18_five_plane_page_init(&handoff->page[index]);
        if (fa18_flight_renderer_page_init(
                &handoff->renderer[index], &handoff->page[index], pixel_state,
                line_style, display_bound_y, vertical_value, horizontal_value,
                renderer_base_long, mode_flag, saved_line_scratch) != 0)
            return -1;
    }
    return 0;
}

FA18FlightRendererPage *fa18_flight_page_handoff_selected_renderer(
    FA18FlightPageHandoff *handoff) {
    return handoff && handoff->outer_child.selected_index < 2 ?
               &handoff->renderer[handoff->outer_child.selected_index] : 0;
}

int fa18_run_flight_page_handoff_child(
    FA18FlightPageHandoff *handoff, FA18ViewportModeState *viewport_mode,
    const FA18FlightPageHandoffOps *ops, FA18Video *video,
    FA18OuterLoopChildStep *step) {
    uint32_t view_pointers[2], display_pointers[2];
    HandoffContext context;
    FA18OuterLoopChildOps child_ops;

    if (!handoff || !viewport_mode || !ops || !ops->wait_viewport || !video || !step)
        return -1;
    for (unsigned index = 0; index < 2; ++index) {
        view_pointers[index] = handoff->view_pair[index].view_pointer;
        display_pointers[index] = handoff->view_pair[index].display_instruction_pointer;
    }
    context = (HandoffContext){handoff, ops};
    child_ops = (FA18OuterLoopChildOps){
        wait_viewport, load_rgb4, &context, wait_blit, load_view, ops->static_palette
    };
    if (fa18_run_outer_loop_child(&handoff->outer_child, viewport_mode,
                                  view_pointers, display_pointers, 2,
                                  &child_ops, step) != 0)
        return -1;
    return fa18_five_plane_page_present(&handoff->page[handoff->visible_index], video);
}
