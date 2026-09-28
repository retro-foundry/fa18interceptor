#include "default_scene_render_pass.h"
#include "run075_trig_asset.h"
#include "two_angle_matrix.h"

#include <assert.h>
#include <string.h>

static void put16(uint8_t *bytes, uint16_t value) {
    bytes[0] = (uint8_t)(value >> 8);
    bytes[1] = (uint8_t)value;
}

static void put32(uint8_t *bytes, uint32_t value) {
    put16(bytes, (uint16_t)(value >> 16));
    put16(bytes + 2, (uint16_t)value);
}

int main(void) {
    uint8_t hunk63[0xae8 + sizeof fa18_run075_trig_bytes] = {0};
    FA18HunkSegment segments[64] = {{0}};
    FA18Hunks hunks = {segments, 64};
    FA18FlightTrigTable trig = {0};
    uint8_t record[0xa4] = {0};
    uint8_t grid_records[6] = {0, 0, 0, 0, 0, 2};
    uint8_t bounds[0x400] = {0};
    FA18ProjectionGrid grid = {1, 0x100, grid_records, bounds, {0}};
    FA18FivePlanePage page;
    FA18FlightRendererPage renderer;
    FA18PlanarPixelState pixels = {0, 0, 0, 0};
    FA18LineStyle lines = {0, 0, 0, 0};
    FA18DefaultSceneRenderPassInput input;
    FA18DefaultSceneRenderPassResult result;

    memcpy(hunk63 + 0xae8, fa18_run075_trig_bytes, sizeof fa18_run075_trig_bytes);
    segments[63] = (FA18HunkSegment){.data = hunk63, .size = sizeof hunk63};
    assert(fa18_load_two_angle_trig_table(&hunks, &trig) == 0);
    put16(record + 0x66, 0); put16(record + 0x68, 28600); put16(record + 0x6a, 0);
    put32(record + 0x14, 0); put32(record + 0x18, 0x80); put32(record + 0x1c, 0);
    fa18_five_plane_page_init(&page);
    assert(fa18_flight_renderer_page_init(&renderer, &page, &pixels, &lines,
                                           179, 111, 106, 0, 0, 0) == 0);
    input = (FA18DefaultSceneRenderPassInput){
        {record, sizeof record, 0, 0}, {0, 0, 0, {168, 252, 128}}, 0, 200
    };
    assert(fa18_render_default_active_scene_pass(&trig, &grid, &input, &renderer,
                                                  &result) == 0);
    assert(result.matrix_route_result == FA18_CONTROL_RECORD_MATRIX_ROUTE_DEFAULT &&
           !memcmp(result.matrix_route.second_matrix,
                   (int16_t[3][3]){{167, 0, -8}, {0, 252, 0}, {6, 0, 127}},
                   sizeof result.matrix_route.second_matrix));
    /* The synthetic grid is intentionally only a route witness: the real
     * source matrix may cull its invented record.  Reaching READY proves the
     * default cache and active-record packet were composed into `$C279D0`. */
    assert(result.scene_pipeline.packet_route == FA18_PROJECTION_GRID_PACKET_READY &&
           renderer.pixel_state.draw_mode == 3 &&
           renderer.pixel_state.active_plane_mask == 4);
    input.matrix_route.matrix_selector = 1;
    assert(fa18_render_default_active_scene_pass(&trig, &grid, &input, &renderer,
                                                  &result) == 1 &&
           result.matrix_route_result == FA18_CONTROL_RECORD_MATRIX_ROUTE_UNPORTED_SELECTION);
    return 0;
}
