#ifndef FA18_PARENT_FLIGHT_UPDATE_H
#define FA18_PARENT_FLIGHT_UPDATE_H

#include <stdint.h>

typedef struct {
    int32_t signed_stage_value;
    uint8_t flight_update_flag;
    uint16_t stage_marker;
} FA18ParentFlightUpdateState;

typedef int (*FA18ParentFlightUpdateStage)(void *context);
typedef int (*FA18ParentFlightDecision)(void *context, int32_t *result);

typedef struct {
    FA18ParentFlightUpdateStage first_update_stage;
    FA18ParentFlightUpdateStage second_update_stage;
    FA18ParentFlightUpdateStage renderer_packet;
    FA18ParentFlightDecision decision;
    FA18ParentFlightUpdateStage branch_stage;
    FA18ParentFlightUpdateStage followup_stage;
    void *context;
} FA18ParentFlightUpdateOps;

typedef enum {
    FA18_PARENT_FLIGHT_UPDATE_SKIPPED,
    FA18_PARENT_FLIGHT_UPDATE_FLAGGED_BRANCH,
    FA18_PARENT_FLIGHT_UPDATE_DECISION_TRUE,
    FA18_PARENT_FLIGHT_UPDATE_DECISION_FALSE
} FA18ParentFlightUpdateRoute;

/* `$C0F090-$C0F123`: source-ordered parent update slice. Every nested call
 * remains caller-owned; the literal stage markers and branch order are native
 * state so this scheduler cannot silently become a frame-specific route. */
int fa18_run_parent_flight_update(FA18ParentFlightUpdateState *state,
                                  const FA18ParentFlightUpdateOps *ops,
                                  FA18ParentFlightUpdateRoute *route);

#endif
