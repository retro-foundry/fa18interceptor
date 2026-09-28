#include "parent_update_pipeline.h"
int fa18_run_parent_update_pipeline(const FA18ParentUpdatePipeline*p){return !p||fa18_run_parent_update_prefix(p->prefix_state,p->prefix_ops)||fa18_run_parent_update_middle(p->middle_state,p->middle_ops)||fa18_run_parent_flight_update(p->flight_state,p->flight_ops,p->flight_route)?-1:0;}
