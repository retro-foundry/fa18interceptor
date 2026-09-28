#include "map_packet_polygon_display.h"
#include "five_plane_chip_binding.h"
#include "flight_renderer_page.h"
#include "projection_page_blitter.h"

#include <assert.h>

int main(void) {
    const FA18MapPacketProjectionRecord records[] = {
        {{2383,-8,1982}}, {{1030,-8,1869}}, {{1022,-8,1829}},
        {{737,-8,1708}}, {{606,-8,1790}}, {{633,-8,1855}},
        {{435,-8,1848}}, {{274,-8,1739}}, {{554,-8,1789}},
        {{236,-8,1682}}, {{-337,-8,1311}}, {{-241,-8,-213}},
        {{2515,-8,-114}}
    };
    uint8_t chip[0x10000 + FA18_COPPER_PAGE_BYTES * FA18_COPPER_PAGE_PLANES] = {0};
    const uint32_t planes[FA18_COPPER_PAGE_PLANES] = {
        0x10000, 0x11f40, 0x13e80, 0x15dc0, 0x17d00
    };
    FA18FivePlanePage page;
    FA18FivePlaneChipBinding binding;
    FA18FlightRendererPage renderer;
    FA18ProjectionPageBlitter page_blitter;
    FA18BlitOperation operation = { .bltafwm = 0xffff, .bltalwm = 0xffff };
    FA18RendererLaneStage lanes = {
        {0x15dc0, 0x13e80, 0x11f40, 0x10000},
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    FA18PolygonDisplayPipelineResult result;
    FA18MapPacketPolygonDisplay display;
    size_t nonzero = 0;

    fa18_five_plane_page_init(&page);
    assert(fa18_five_plane_chip_binding_init(&binding, chip, sizeof chip, planes) == 0);
    assert(fa18_flight_renderer_page_init(&renderer, &page,
                                          &(FA18PlanarPixelState){0, 0, 0, 0},
                                          &(FA18LineStyle){0, 0, 0, 0},
                                          144, 89, 0, 0x10000, 0, 0) == 0);
    assert(fa18_projection_page_blitter_init(&page_blitter, &page, &binding,
                                              &operation, &lanes) == 0);
    assert(fa18_flight_renderer_page_bind_projection_page_blitter(
               &renderer, &page_blitter) == 0);
    display = (FA18MapPacketPolygonDisplay){&renderer.triangle_submission, &result};

    assert(fa18_display_map_packet_polygon(&display, records, 13, 2) == 0);
    assert(result.clipped_count == 14 && result.pair_count == 14);
    assert(renderer.dma_call_count == 1 && renderer.last_dma_value == 0x8400);
    assert(page_blitter.line_submissions != 0);
    for (size_t index = 0; index < FA18_COPPER_PAGE_BYTES; ++index)
        for (unsigned plane = 0; plane < 4; ++plane)
            nonzero += page.planes[plane][index] != 0;
    assert(nonzero != 0);
    return 0;
}
