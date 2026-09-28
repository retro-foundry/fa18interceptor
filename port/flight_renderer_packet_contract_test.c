#include "flight_renderer_packet.h"

#include <assert.h>
#include <string.h>

typedef struct {
    unsigned primary_calls;
    unsigned adjacent_calls;
    unsigned complete_calls;
} TestLog;

static int emit_primary(void *context, int16_t x, int16_t y) {
    TestLog *log = context;
    (void)x;
    (void)y;
    ++log->primary_calls;
    return 0;
}

static int emit_adjacent(void *context, int16_t x, int16_t y) {
    TestLog *log = context;
    (void)x;
    (void)y;
    ++log->adjacent_calls;
    return 0;
}

/* This is deliberately replaced by the packet boundary. It verifies the
 * source completion write cannot be delegated to an unrelated page owner. */
static int unexpected_completion(void *context) {
    TestLog *log = context;
    ++log->complete_calls;
    return -1;
}

int main(void) {
    uint8_t records[6] = { 0, 0, 0, 0, 0, 2 };
    uint8_t bounds[0x400];
    FA18ProjectionGrid grid = { 1, 0x100, records, bounds, { 0 } };
    FA18ProjectionPacket packet = { 0, -0x80, 0, -0x80 };
    /* The middle matrix column produces positive perspective depth for the
     * packet's negative scaled input, allowing the direct route to submit. */
    FA18ProjectionPairMatrix matrix = { { 0, 0, 0, 0, 0, 0, 0, -64, 0 } };
    FA18ProjectionGridPacketState state;
    FA18ProjectionGridPacketRoute route;
    uint16_t submitted;
    TestLog log = { 0 };
    FA18ProjectionGridSubmission submission = {
        0, emit_primary, emit_adjacent, &log, unexpected_completion, &log
    };

    memset(bounds, 0, sizeof bounds);
    assert(fa18_render_flight_projection_grid(
               &grid, &packet, &matrix, 0, 200, &submission, &state, &route,
               &submitted) == 0);
    assert(route == FA18_PROJECTION_GRID_PACKET_READY);
    assert(submitted == 1 && log.primary_calls == 0 && log.adjacent_calls == 1);
    assert(log.complete_calls == 0 && state.line_emitter_mode_flag == 0);

    /* The renderer gate consumes the published middle word (`$C45A78`), not
     * the pre-shift projection intermediate retained for upstream consumers. */
    packet.depth_metric = -0x801;
    assert(fa18_render_flight_projection_grid(
               &grid, &packet, &matrix, 0, 200, &submission, &state, &route,
               &submitted) == 0);
    assert(route == FA18_PROJECTION_GRID_PACKET_READY && submitted == 1);

    packet.y = -0x801;
    assert(fa18_render_flight_projection_grid(
               &grid, &packet, &matrix, 0, 200, &submission, &state, &route,
               &submitted) == 0);
    assert(route == FA18_PROJECTION_GRID_PACKET_DEPTH_REJECT && submitted == 0);
    return 0;
}
