#ifndef FA18_DEFAULT_SCENE_RENDER_PASS_H
#define FA18_DEFAULT_SCENE_RENDER_PASS_H

#include <stdint.h>

#include "control_record_matrix_route.h"
#include "flight_scene_pipeline.h"
#include "scene_active_record.h"

/* Source-owned inputs spanning the observed default `$C2DB18` route and the
 * later `$C1C54E -> $C279D0` renderer packet. Page selection, palette
 * publication, and parent-update cadence remain outside this boundary. */
typedef struct {
    FA18SceneActiveRecordState active_record;
    FA18ControlRecordMatrixRouteInput matrix_route;
    uint8_t packet_mode;
    int16_t direct_pair_mode_limit;
} FA18DefaultSceneRenderPassInput;

typedef struct {
    FA18ControlRecordMatrixRouteOutput matrix_route;
    FA18ControlRecordMatrixRouteResult matrix_route_result;
    FA18FlightScenePipelineResult scene_pipeline;
} FA18DefaultSceneRenderPassResult;

/* Compose the dynamic run075-default route in source order:
 * `$C2DB18` publishes `$C45BD8`, then `$C1C54E` selects the same active
 * record and `$C279D0` consumes both. Returns one when `$C2DB18` selects an
 * as-yet-unported alternate route, zero when the default pass rendered, and
 * minus one for invalid inputs or a bounded renderer failure. */
int fa18_render_default_active_scene_pass(
    const FA18FlightTrigTable *trig_table, const FA18ProjectionGrid *grid,
    const FA18DefaultSceneRenderPassInput *input,
    FA18FlightRendererPage *page_renderer,
    FA18DefaultSceneRenderPassResult *result);

#endif
