#include "parent_update_middle.h"
#include <assert.h>
static int hit(void *p){ ++*(unsigned*)p; return 0; }
int main(void){ unsigned n=0; FA18ParentUpdateMiddleOps o={hit,hit,hit,hit,hit,hit,hit,hit,hit,hit,hit,&n}; FA18ParentUpdateMiddleState s={0,0,0}; assert(fa18_run_parent_update_middle(&s,&o)==0&&n==11&&s.stage_marker==0x70); n=0;s.conditional_flag=1;s.conditional_inhibit=0;assert(fa18_run_parent_update_middle(&s,&o)==0&&n==10); return 0; }
