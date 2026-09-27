#include "flight_scene_pipeline.h"

#include <assert.h>
#include <string.h>

static void put16(uint8_t *p, uint16_t value) { p[0] = (uint8_t)(value >> 8); p[1] = (uint8_t)value; }
static void put32(uint8_t *p, uint32_t value) { p[0] = (uint8_t)(value >> 24); p[1] = (uint8_t)(value >> 16); p[2] = (uint8_t)(value >> 8); p[3] = (uint8_t)value; }

int main(void) {
    uint8_t record[0xa4] = {0};
    uint8_t grid_records[6] = {0, 0, 0, 0, 0, 2};
    uint8_t bounds[0x400] = {0};
    FA18ProjectionGrid grid = {1, 0x100, grid_records, bounds, {0}};
    FA18ProjectionPairMatrix matrix = {{0, 0, 0, 0, 0, 0, 0, -64, 0}};
    FA18FivePlanePage page;
    FA18FlightRendererPage renderer;
    FA18PlanarPixelState pixels = {0x0b, 0x0f, -1, 0};
    FA18LineStyle lines = {0x0f, -1, 0, 0x06};
    FA18FlightScenePipelineInput input;
    FA18FlightScenePipelineResult result;

    /* A synthetic root produces depth -$80 (the ready-side gate) without
     * embedding a captured scene packet. The zero matrix makes the selected
     * source seed contribute no additional root-relative component. */
    put32(record + 0x14, 0); put32(record + 0x18, 0x80); put32(record + 0x1c, 0);
    put16(record + 0x92 + 0, 0); put16(record + 0x92 + 2, 0); put16(record + 0x92 + 4, 0);
    put16(record + 0x92 + 6, 0); put16(record + 0x92 + 8, 0); put16(record + 0x92 + 10, 0);
    put16(record + 0x92 + 12, 0); put16(record + 0x92 + 14, 0); put16(record + 0x92 + 16, 0);
    fa18_five_plane_page_init(&page);
    assert(fa18_flight_renderer_page_init(&renderer, &page, &pixels, &lines,
                                           179, 111, 106, 0, 0, 0) == 0);
    input = (FA18FlightScenePipelineInput){record, sizeof record, &matrix, 0, 200};
    assert(fa18_render_flight_scene_pipeline(&grid, &input, &renderer, &result) == 0);
    assert(result.packet_route == FA18_PROJECTION_GRID_PACKET_READY &&
           result.submitted_record_count == 1);
    FA18ParentFlightScenePipelineContext callback = {&grid, &input, &renderer, &result};
    assert(fa18_run_parent_flight_scene_pipeline(&callback) == 0 &&
           result.packet_route == FA18_PROJECTION_GRID_PACKET_READY);
    input.scene_record_size = sizeof record - 1;
    assert(fa18_render_flight_scene_pipeline(&grid, &input, &renderer, &result) == -1);
    return 0;
}
