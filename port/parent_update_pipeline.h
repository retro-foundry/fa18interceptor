#ifndef FA18_PARENT_UPDATE_PIPELINE_H
#define FA18_PARENT_UPDATE_PIPELINE_H
#include "parent_update_prefix.h"
#include "parent_update_middle.h"
#include "parent_flight_update.h"
typedef struct { FA18ParentUpdatePrefixState *prefix_state; const FA18ParentUpdatePrefixOps *prefix_ops; FA18ParentUpdateMiddleState *middle_state; const FA18ParentUpdateMiddleOps *middle_ops; FA18ParentFlightUpdateState *flight_state; const FA18ParentFlightUpdateOps *flight_ops; FA18ParentFlightUpdateRoute *flight_route; } FA18ParentUpdatePipeline;
/* Source-contiguous `$C0EFD4-$C0F123` composition. */
int fa18_run_parent_update_pipeline(const FA18ParentUpdatePipeline *);
#endif
