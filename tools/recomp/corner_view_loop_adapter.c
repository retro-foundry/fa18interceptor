/* Test observation ends after 64 branches; no game return is supplied. */
#define glue_C2E758 loop_unused_C2E758
#define glue_C2CE82 loop_unused_C2CE82
#define glue_C2CCA0 loop_unused_C2CCA0
#define glue_C2CD28 loop_unused_C2CD28
#define glue_C2CD94 loop_unused_C2CD94
#define glue_C2D082 loop_unused_C2D082
#define glue_C2D3A4 loop_unused_C2D3A4
#define glue_C200F6 loop_unused_C200F6
#define glue_C203CC loop_unused_C203CC
#define glue_C2058E loop_unused_C2058E
#define glue_C20826 loop_unused_C20826
#define glue_C22C70 loop_unused_C22C70
#include "../../port/game/glue/glue_corner_view.c"
#include <setjmp.h>
static jmp_buf loop_observed;
static unsigned loop_count;
static void observe_loop(void *context,enum CornerViewPhase phase,enum CornerViewField field,uint32_t value,uint32_t other) {
 outputs(context,phase,field,value,other);
 if(phase==CV_PARALLEL_LOOP){if(value!=0xc2ea02u)abort();if(++loop_count==64)longjmp(loop_observed,1);}
}
int corner_view_observe_loop(void) {
 CornerViewHooks observed=hooks;observed.observe=observe_loop;loop_count=0;
 if(!setjmp(loop_observed)){corner_project_edges(working(),&observed);return 0;}
 return loop_count==64;
}
