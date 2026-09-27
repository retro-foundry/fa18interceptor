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
    assert(page.planes[0][402] == 0 && page.planes[1][402] == 0x08 &&
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
    return 0;
}
