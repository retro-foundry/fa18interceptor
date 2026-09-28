#ifndef FA18_POLYGON_DISPLAY_PIPELINE_H
#define FA18_POLYGON_DISPLAY_PIPELINE_H

#include "polygon_clip_pipeline.h"
#include "projection.h"
#include "projection_grid.h"

typedef struct {
    FA18ClipTuple clipped[FA18_POLYGON_MAX_VERTICES];
    uint16_t clipped_count;
    FA18ProjectionPairScreenPoint pairs[FA18_POLYGON_MAX_VERTICES];
    uint16_t pair_count;
    FA18ProjectionPairBoundsRoute bounds_route;
    FA18ProjectionPairFinalizationRoute finalization_route;
} FA18PolygonDisplayPipelineResult;

/* `$C246A0 -> $C24CFE -> $C2FF48`: clip caller triples, project the positive
 * depth result to source screen-pair order, then use the distinct DMA-enabled
 * tuple-list submission wrapper. Returns zero for the source nonpositive
 * depth exit, one when it submits, and minus one for an invalid native input. */
int fa18_run_polygon_display_pipeline(
    const FA18ClipTuple *input, uint16_t input_count, int16_t coordinate_shift,
    const FA18ProjectionPairSubmission *submission,
    FA18PolygonDisplayPipelineResult *result);

#endif
