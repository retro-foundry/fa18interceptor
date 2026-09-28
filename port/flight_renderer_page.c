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
    renderer->pixel_state = *pixel_state;
    renderer->line_style = *line_style;
    renderer->direct_pixels.page = &renderer->lanes;
    renderer->direct_pixels.state = &renderer->pixel_state;
    renderer->lines.page = &renderer->lanes;
    renderer->lines.style = &renderer->line_style;
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

int fa18_flight_renderer_page_apply_projection_grid_packet_state(
    FA18FlightRendererPage *renderer,
    const FA18ProjectionGridPacketState *packet_state) {
    FA18PlanarPixelSourceState pixel_source;

    if (!renderer || !packet_state) return -1;
    pixel_source = (FA18PlanarPixelSourceState){
        packet_state->renderer_selector,
        (uint8_t)packet_state->renderer_state_words[0],
        packet_state->renderer_state_words[1],
        (uint8_t)packet_state->renderer_state_words[2]
    };
    if (fa18_decode_planar_pixel_state(&pixel_source, &renderer->pixel_state) != 0)
        return -1;

    /* `$C2FA82` copies `$C45954` to `$C45956`; the low byte at `$C45957`
     * controls the negative-mode branch. `$C456E8` and its low byte
     * `$C456E9` remain the plane mode and per-plane flag bits. */
    renderer->line_style = (FA18LineStyle){
        (uint8_t)packet_state->renderer_state_words[0],
        packet_state->renderer_state_words[1],
        (uint8_t)packet_state->renderer_state_words[1],
        packet_state->renderer_selector
    };
    return 0;
}

const FA18ProjectionGridSubmission *fa18_flight_renderer_page_submission(
    const FA18FlightRendererPage *renderer) {
    return renderer ? &renderer->grid_submission : 0;
}

int fa18_flight_renderer_page_execute_lane_stage(
    FA18FlightRendererPage *renderer, FA18FivePlanePage *page,
    const FA18FivePlaneChipBinding *binding, FA18RendererLaneStage *stage,
    FA18BlitOperation *operation) {
    uint32_t lane_pointers[4];

    if (!renderer || !page || !binding || !stage || !operation ||
        fa18_five_plane_chip_binding_renderer_lane_pointers(binding, lane_pointers) != 0)
        return -1;
    for (unsigned lane = 0; lane < 4; ++lane)
        if (stage->plane_pointers[lane] != lane_pointers[lane]) return -1;
    if (fa18_five_plane_chip_binding_store_page(binding, page) != 0 ||
        fa18_execute_renderer_lane_stage(stage, operation, binding->chip_bytes,
                                         binding->chip_byte_count) != 0)
        return -1;
    return fa18_five_plane_chip_binding_load_page(binding, page);
}
