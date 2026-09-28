#include "default_scene_render_pass.h"

#include <string.h>

int fa18_render_default_active_scene_pass(
    const FA18FlightTrigTable *trig_table, const FA18ProjectionGrid *grid,
    const FA18DefaultSceneRenderPassInput *input,
    FA18FlightRendererPage *page_renderer,
    FA18DefaultSceneRenderPassResult *result) {
    const uint8_t *record;
    FA18FlightScenePipelineInput scene_input;
    FA18ProjectionPairMatrix matrix;

    if (!trig_table || !grid || !input || !page_renderer || !result ||
        fa18_resolve_scene_active_record(
            &input->active_record, FA18_SCENE_PROJECTION_SEED_RECORD_BYTES,
            &record) != 0)
        return -1;
    if (fa18_update_default_control_record_matrices(
            trig_table, record, FA18_SCENE_PROJECTION_SEED_RECORD_BYTES,
            &input->matrix_route, &result->matrix_route,
            &result->matrix_route_result) != 0)
        return -1;
    if (result->matrix_route_result != FA18_CONTROL_RECORD_MATRIX_ROUTE_DEFAULT)
        return 1;
    memcpy(matrix.words, result->matrix_route.second_matrix, sizeof matrix.words);
    scene_input = (FA18FlightScenePipelineInput){
        record, FA18_SCENE_PROJECTION_SEED_RECORD_BYTES, &matrix,
        input->packet_mode, input->direct_pair_mode_limit
    };
    return fa18_render_flight_scene_pipeline(grid, &scene_input, page_renderer,
                                             &result->scene_pipeline);
}
