#include "parent_update_pipeline.h"
#include <assert.h>
static int h(void*p){++*(unsigned*)p;return 0;}static int d(void*p,int32_t*r){h(p);*r=0;return 0;}
int main(void){unsigned n=0;FA18ParentUpdatePrefixOps a={h,h,h,h,h,h,&n};FA18ParentUpdateMiddleOps b={h,h,h,h,h,h,h,h,h,h,h,&n};FA18ParentFlightUpdateOps c={h,h,h,d,h,h,&n};FA18ParentUpdatePrefixState x={0,0,0,1};FA18ParentUpdateMiddleState y={0};FA18ParentFlightUpdateState z={0};FA18ParentFlightUpdateRoute q;FA18ParentUpdatePipeline p={&x,&a,&y,&b,&z,&c,&q};assert(!fa18_run_parent_update_pipeline(&p)&&n==23&&q==FA18_PARENT_FLIGHT_UPDATE_DECISION_FALSE);}
