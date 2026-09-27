#ifndef FA18_FLIGHT_RENDERER_PACKET_H
#define FA18_FLIGHT_RENDERER_PACKET_H

#include <stdint.h>

#include "projection_grid.h"
#include "projection_packet.h"

/* `$C279D0-$C27D0F`, entered as the third child of `$C0F090`. The preceding
 * update stage owns publication of `packet`; this boundary consumes that
 * packet and dispatches the exact Hunk-25 grid traversal. Page and Blitter
 * ownership remain with the caller through `submission`. */
int fa18_render_flight_projection_grid(
    const FA18ProjectionGrid *grid, const FA18ProjectionPacket *packet,
    const FA18ProjectionPairMatrix *matrix, uint8_t packet_mode,
    int16_t direct_pair_mode_limit,
    const FA18ProjectionGridSubmission *submission,
    FA18ProjectionGridPacketState *packet_state,
    FA18ProjectionGridPacketRoute *packet_route,
    uint16_t *submitted_record_count);

#endif
