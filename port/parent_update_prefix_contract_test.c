#include "parent_update_prefix.h"
#include <assert.h>
static int h(void*p){++*(unsigned*)p;return 0;}int main(void){unsigned n=0;FA18ParentUpdatePrefixOps o={h,h,h,h,h,h,&n};FA18ParentUpdatePrefixState s={99};assert(!fa18_run_parent_update_prefix(&s,&o)&&n==3&&s.local_frame==99);s.skip_flag=1;assert(!fa18_run_parent_update_prefix(&s,&o)&&n==9&&s.stage_marker==16);}
