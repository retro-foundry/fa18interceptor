#include "flight_renderer_page.h"

#include <assert.h>

int main(void) {
    FA18FivePlanePage page;
    FA18FlightRendererPage renderer;
    const FA18PlanarPixelState pixels = { 0x0b, 0x0f, -1, 0 };
    const FA18LineStyle lines = { 0x0f, -1, 0, 0x06 };
    const FA18ProjectionGridSubmission *submission;

    fa18_five_plane_page_init(&page);
    assert(fa18_flight_renderer_page_init(
               &renderer, &page, &pixels, &lines, 179, 111, 106,
               0x0004db30u, 0, 0) == 0);
    submission = fa18_flight_renderer_page_submission(&renderer);
    assert(submission && submission->triangle_submission);

    assert(submission->primary_emitter(submission->emitter_context, 20, 10) == 0);
    assert(page.planes[0][402] == 0x08 && page.planes[1][402] == 0x08 &&
           page.planes[2][402] == 0 && page.planes[3][402] == 0x08);

    assert(submission->triangle_submission->dma_emitter(
               submission->triangle_submission->dma_context, 0x8400) == 0);
    assert(renderer.dma_call_count == 1 && renderer.last_dma_value == 0x8400);
    assert(submission->triangle_submission->line_emitter(
               submission->triangle_submission->line_context,
               20, 9, 23, 10, 179) == 0);
    assert(page.planes[1][402] == 0x0f && page.planes[2][402] == 0x0f);
    assert(!submission->triangle_submission->blitter_emitter &&
           !submission->triangle_submission->final_emitter);

    const FA18ProjectionGridPacketState packet_state = {
        { 4, 0, 0, -1 }, 3, 1, 0
    };
    assert(fa18_flight_renderer_page_apply_projection_grid_packet_state(
               &renderer, &packet_state) == 0);
    assert(renderer.direct_pixels.state == &renderer.pixel_state &&
           renderer.pixel_state.draw_mode == 3 &&
           renderer.pixel_state.active_plane_mask == 4 &&
           renderer.pixel_state.output_xor_enable == 0 &&
           renderer.pixel_state.output_xor_plane_mask == 0);
    assert(renderer.lines.style == &renderer.line_style &&
           renderer.line_style.active_plane_mask == 4 &&
           renderer.line_style.plane_mode == 0 && renderer.line_style.plane_bits == 0 &&
           renderer.line_style.control_plane_bits == 3);
    assert(fa18_flight_renderer_page_apply_projection_grid_packet_state(0, &packet_state) == -1);

    uint8_t chip[0xd000] = { 0 };
    const uint32_t plane_pointers[FA18_COPPER_PAGE_PLANES] = {
        0x1000, 0x3000, 0x5000, 0x7000, 0x9000
    };
    FA18FivePlaneChipBinding binding;
    FA18BlitOperation operation = { .bltafwm = 0xffff, .bltalwm = 0xffff };
    FA18RendererLaneStage lane_stage = {
        { 0x7000, 0x5000, 0x3000, 0x1000 }, 1, 3, 0, 0, 7,
        0, 0xc000, 0x3000, 0x0041, 0
    };
    assert(fa18_five_plane_chip_binding_init(&binding, chip, sizeof chip,
                                             plane_pointers) == 0);
    chip[0xc000] = 0x80;
    assert(fa18_flight_renderer_page_execute_lane_stage(
               &renderer, &page, &binding, &lane_stage, &operation) == 0);
    assert(page.planes[0][0] == 0x80 && chip[0x1000] == 0x80);
    lane_stage.plane_pointers[0] = 0;
    assert(fa18_flight_renderer_page_execute_lane_stage(
               &renderer, &page, &binding, &lane_stage, &operation) == -1);
    return 0;
}
