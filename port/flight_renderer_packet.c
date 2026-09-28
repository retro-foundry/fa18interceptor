#include "flight_renderer_packet.h"

int fa18_render_flight_projection_grid(
    const FA18ProjectionGrid *grid, const FA18ProjectionPacket *packet,
    const FA18ProjectionPairMatrix *matrix, uint8_t packet_mode,
    int16_t direct_pair_mode_limit,
    const FA18ProjectionGridSubmission *submission,
    FA18ProjectionGridPacketState *packet_state,
    FA18ProjectionGridPacketRoute *packet_route,
    uint16_t *submitted_record_count) {
    FA18ProjectionGridSetup setup;
    FA18ProjectionGridSubmission pass_submission;

    if (!grid || !packet || !matrix || !submission || !packet_state ||
        !packet_route || !submitted_record_count)
        return -1;

    if (fa18_initialize_projection_grid_packet(
            /* `$C279D0` reads the published, shifted middle component at
             * `$C45A78`, represented by packet.y.  depth_metric retains the
             * pre-shift intermediate used by its upstream publisher. */
            grid, packet_mode, packet->y, packet->y, packet->x,
            packet->z, packet_state, &setup, packet_route) != 0)
        return -1;
    if (*packet_route != FA18_PROJECTION_GRID_PACKET_READY) {
        *submitted_record_count = 0;
        return 0;
    }

    pass_submission = *submission;
    /* `$C27C48` clears the flag established by this packet's depth gate. */
    pass_submission.completion_emitter = fa18_complete_projection_grid_packet;
    pass_submission.completion_context = packet_state;
    return fa18_submit_projection_grid_pass(
        grid, &setup, matrix, packet_state->negative_kind_flag,
        direct_pair_mode_limit, &pass_submission, submitted_record_count);
}
