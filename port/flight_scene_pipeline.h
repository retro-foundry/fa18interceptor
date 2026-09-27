#ifndef FA18_FLIGHT_SCENE_PIPELINE_H
#define FA18_FLIGHT_SCENE_PIPELINE_H

#include <stddef.h>
#include <stdint.h>

#include "flight_renderer_page.h"
#include "flight_renderer_packet.h"
#include "scene_active_record.h"
#include "scene_projection_seed.h"

/* Caller-owned live inputs at the `$C1C54E -> $C279D0` composition boundary. */
typedef struct {
    const uint8_t *scene_record_bytes;
    size_t scene_record_size;
    const FA18ProjectionPairMatrix *matrix;
    uint8_t packet_mode;
    int16_t direct_pair_mode_limit;
} FA18FlightScenePipelineInput;

typedef struct {
    FA18ProjectionPacket packet;
    FA18ProjectionGridPacketState packet_state;
    FA18ProjectionGridPacketRoute packet_route;
    uint16_t submitted_record_count;
} FA18FlightScenePipelineResult;

/* Compose `$C1C54E-$C1C63D` packet publication with the bounded
 * `$C279D0-$C27D0F` Hunk-25 traversal. Scheduler, record selection, matrix
 * ownership, page selection, and Copper presentation remain caller-owned. */
int fa18_render_flight_scene_pipeline(
    const FA18ProjectionGrid *grid,
    const FA18FlightScenePipelineInput *input,
    FA18FlightRendererPage *page_renderer,
    FA18FlightScenePipelineResult *result);

/* Resolve the `$C46184 + $C458DE.w` record selected by `$C1C54E`, then run
 * the same bounded packet/page pipeline. The caller still owns record-store
 * updates, the live matrix, page selection, and scheduling. */
int fa18_render_active_flight_scene_pipeline(
    const FA18ProjectionGrid *grid,
    const FA18SceneActiveRecordState *active_record,
    const FA18FlightScenePipelineInput *input,
    FA18FlightRendererPage *page_renderer,
    FA18FlightScenePipelineResult *result);

/* Adapter for the `$C279D0` renderer-packet callback slot in
 * `fa18_run_parent_flight_update`. */
typedef struct {
    const FA18ProjectionGrid *grid;
    const FA18FlightScenePipelineInput *input;
    FA18FlightRendererPage *page_renderer;
    FA18FlightScenePipelineResult *result;
} FA18ParentFlightScenePipelineContext;

int fa18_run_parent_flight_scene_pipeline(void *context);

#endif
