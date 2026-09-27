#include "flight_renderer_page.h"

static int record_dma_enable(void *context, uint16_t value) {
    FA18FlightRendererPage *renderer = context;
    if (!renderer || value != UINT16_C(0x8400)) return -1;
    renderer->last_dma_value = value;
    ++renderer->dma_call_count;
    return 0;
}

int fa18_flight_renderer_page_init(
    FA18FlightRendererPage *renderer, FA18FivePlanePage *page,
    const FA18PlanarPixelState *pixel_state, const FA18LineStyle *line_style,
    int16_t display_bound_y, int16_t vertical_value, int16_t horizontal_value,
    uint32_t renderer_base_long, uint8_t mode_flag, uint32_t saved_line_scratch) {
    if (!renderer || !page || !pixel_state || !line_style ||
        fa18_five_plane_page_lower_lanes(page, &renderer->lanes) != 0)
        return -1;

    renderer->last_dma_value = 0;
    renderer->dma_call_count = 0;
    renderer->direct_pixels.page = &renderer->lanes;
    renderer->direct_pixels.state = pixel_state;
    renderer->lines.page = &renderer->lanes;
    renderer->lines.style = line_style;
    renderer->triangle_submission = (FA18ProjectionPairSubmission){
        display_bound_y, vertical_value, horizontal_value, renderer_base_long,
        mode_flag, saved_line_scratch,
        record_dma_enable, renderer,
        fa18_emit_line_to_page, &renderer->lines,
        0, 0, 0, 0, 0
    };
    renderer->grid_submission = (FA18ProjectionGridSubmission){
        &renderer->triangle_submission,
        fa18_emit_primary_renderer_pixel_to_page,
        fa18_emit_adjacent_renderer_pixels_to_page,
        &renderer->direct_pixels,
        0, 0
    };
    return 0;
}

const FA18ProjectionGridSubmission *fa18_flight_renderer_page_submission(
    const FA18FlightRendererPage *renderer) {
    return renderer ? &renderer->grid_submission : 0;
}
