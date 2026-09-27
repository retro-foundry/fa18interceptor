#include "flight_scene_pipeline.h"

int fa18_render_flight_scene_pipeline(
    const FA18ProjectionGrid *grid,
    const FA18FlightScenePipelineInput *input,
    const FA18FlightRendererPage *page_renderer,
    FA18FlightScenePipelineResult *result) {
    FA18SceneProjectionSeedRecord record;
    const FA18ProjectionGridSubmission *submission;

    if (!grid || !input || !input->scene_record_bytes || !input->matrix ||
        !page_renderer || !result)
        return -1;
    submission = fa18_flight_renderer_page_submission(page_renderer);
    if (!submission || fa18_decode_scene_projection_seed_record(
                           input->scene_record_bytes, input->scene_record_size,
                           &record) != 0 ||
        fa18_publish_scene_projection_seed(&record, &result->packet) != 0)
        return -1;
    return fa18_render_flight_projection_grid(
        grid, &result->packet, input->matrix, input->packet_mode,
        input->direct_pair_mode_limit, submission, &result->packet_state,
        &result->packet_route, &result->submitted_record_count);
}

int fa18_run_parent_flight_scene_pipeline(void *context) {
    FA18ParentFlightScenePipelineContext *pipeline = context;
    if (!pipeline) return -1;
    return fa18_render_flight_scene_pipeline(
        pipeline->grid, pipeline->input, pipeline->page_renderer,
        pipeline->result);
}
