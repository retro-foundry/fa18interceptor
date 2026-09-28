#ifndef FA18_PARENT_UPDATE_PIPELINE_H
#define FA18_PARENT_UPDATE_PIPELINE_H
#include "parent_update_prefix.h"
#include "parent_update_middle.h"
#include "parent_flight_update.h"
#include "parent_postflight_setup.h"
#include "parent_activity_stages.h"
#include "parent_update_tail.h"
typedef struct { FA18ParentUpdatePrefixState *prefix_state; const FA18ParentUpdatePrefixOps *prefix_ops; FA18ParentUpdateMiddleState *middle_state; const FA18ParentUpdateMiddleOps *middle_ops; FA18ParentFlightUpdateState *flight_state; const FA18ParentFlightUpdateOps *flight_ops; FA18ParentFlightUpdateRoute *flight_route; FA18ParentPostflightSetupState *postflight_state; const FA18ParentPostflightSetupOps *postflight_ops; FA18ParentActivityStagesState *activity_state; const FA18ParentActivityStagesOps *activity_ops; FA18ParentUpdateTailState *tail_state; const FA18ParentUpdateTailOps *tail_ops; } FA18ParentUpdatePipeline;
/* Source-contiguous `$C0EFD4-$C0F3C3` composition. */
int fa18_run_parent_update_pipeline(const FA18ParentUpdatePipeline *);
#endif
