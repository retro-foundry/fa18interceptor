#include "polygon_display_pipeline.h"

#include "projection.h"

int fa18_run_polygon_display_pipeline(
    const FA18ClipTuple *input, uint16_t input_count, int16_t coordinate_shift,
    const FA18ProjectionPairSubmission *submission,
    FA18PolygonDisplayPipelineResult *result) {
    FA18ViewVertex vertices[FA18_POLYGON_MAX_VERTICES];
    FA18ScreenPolygon polygon;
    int projection_result;

    if (!input || !submission || !result ||
        fa18_clip_projection_polygon(input, input_count, coordinate_shift,
                                     result->clipped,
                                     sizeof result->clipped / sizeof *result->clipped,
                                     &result->clipped_count) != 0 ||
        result->clipped_count > FA18_POLYGON_MAX_VERTICES)
        return -1;
    for (uint16_t index = 0; index < result->clipped_count; ++index)
        vertices[index] = (FA18ViewVertex){result->clipped[index].x,
                                            result->clipped[index].y,
                                            result->clipped[index].z};
    projection_result = fa18_project_polygon(vertices, result->clipped_count, &polygon);
    if (projection_result < 0) return -1;
    if (!projection_result) {
        result->pair_count = 0;
        return 0;
    }
    result->pair_count = polygon.count;
    for (uint16_t index = 0; index < polygon.count; ++index)
        result->pairs[index] = (FA18ProjectionPairScreenPoint){polygon.point[index].x,
                                                                 polygon.point[index].y};
    if (fa18_submit_projection_pair_list(result->pairs, result->pair_count,
                                         submission, &result->bounds_route,
                                         &result->finalization_route) != 0)
        return -1;
    return 1;
}
