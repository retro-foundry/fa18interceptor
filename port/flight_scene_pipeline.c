#include "flight_scene_pipeline.h"

int fa18_render_flight_scene_pipeline(
    const FA18ProjectionGrid *grid,
    const FA18FlightScenePipelineInput *input,
    FA18FlightRendererPage *page_renderer,
    FA18FlightScenePipelineResult *result) {
    FA18SceneProjectionSeedRecord record;
    const FA18ProjectionGridSubmission *submission;
    FA18ProjectionGridSetup setup;

    if (!grid || !input || !input->scene_record_bytes || !input->matrix ||
        !page_renderer || !result)
        return -1;
    submission = fa18_flight_renderer_page_submission(page_renderer);
    if (!submission || fa18_decode_scene_projection_seed_record(
                           input->scene_record_bytes, input->scene_record_size,
                           &record) != 0 ||
        fa18_publish_scene_projection_seed(&record, &result->packet) != 0)
        return -1;

    /* `$C279D4-$C279F2` initializes these renderer words before its route
     * gates. The same packet initializer is called again by the traversal;
     * this first call lets the bound page receive the exact source state. */
    if (fa18_initialize_projection_grid_packet(
            grid, input->packet_mode, result->packet.depth_metric, result->packet.y,
            result->packet.x, result->packet.z, &result->packet_state, &setup,
            &result->packet_route) != 0 ||
        fa18_flight_renderer_page_apply_projection_grid_packet_state(
            page_renderer, &result->packet_state) != 0)
        return -1;
    return fa18_render_flight_projection_grid(
        grid, &result->packet, input->matrix, input->packet_mode,
        input->direct_pair_mode_limit, submission, &result->packet_state,
        &result->packet_route, &result->submitted_record_count);
}

int fa18_render_active_flight_scene_pipeline(
    const FA18ProjectionGrid *grid,
    const FA18SceneActiveRecordState *active_record,
    const FA18FlightScenePipelineInput *input,
    FA18FlightRendererPage *page_renderer,
    FA18FlightScenePipelineResult *result) {
    const uint8_t *record;
    FA18FlightScenePipelineInput selected_input;

    if (!input || fa18_resolve_scene_active_record(
                      active_record, FA18_SCENE_PROJECTION_SEED_RECORD_BYTES,
                      &record) != 0)
        return -1;
    selected_input = *input;
    selected_input.scene_record_bytes = record;
    selected_input.scene_record_size = FA18_SCENE_PROJECTION_SEED_RECORD_BYTES;
    return fa18_render_flight_scene_pipeline(grid, &selected_input, page_renderer, result);
}

int fa18_run_parent_flight_scene_pipeline(void *context) {
    FA18ParentFlightScenePipelineContext *pipeline = context;
    if (!pipeline) return -1;
    return fa18_render_flight_scene_pipeline(
        pipeline->grid, pipeline->input, pipeline->page_renderer,
        pipeline->result);
}
