/* Expose the actual production domain/observer at its non-returning source
 * branch. The proof intercepts observation, never supplies a game return. */
#define glue_C1FF9C parallel_unused_C1FF9C
#define glue_C1FFA4 parallel_unused_C1FFA4
#define glue_C2ED70 parallel_unused_C2ED70
#define glue_C2EE4A parallel_unused_C2EE4A
#define glue_C2F0C6 parallel_unused_C2F0C6
#define glue_C2F0F4 parallel_unused_C2F0F4
#define glue_C2F128 parallel_unused_C2F128
#define glue_C2F156 parallel_unused_C2F156
#define glue_C2EA5A parallel_unused_C2EA5A
#define glue_C2EAD0 parallel_unused_C2EAD0
#define glue_C2EB4C parallel_unused_C2EB4C
#define glue_C2EBC2 parallel_unused_C2EBC2
#include "../../port/game/glue/glue_segment_projection.c"
#include <setjmp.h>
static jmp_buf loop_observed;
static unsigned loop_count;
static uint32_t expected_loop;
static void observe_parallel(void *context,enum SegmentProjectionPhase phase,enum SegmentProjectionField field,uint32_t value,uint32_t other) {
 outputs(context,phase,field,value,other);
 if(phase==SP_PARALLEL_LOOP){
  if(value!=expected_loop)abort();
  if(++loop_count==64)longjmp(loop_observed,1);
 }
}
int segment_projection_observe_parallel(uint32_t entry,uint32_t pc) {
 SegmentProjectionHooks observed=hooks;observed.observe=observe_parallel;
 expected_loop=pc;loop_count=0;
 if(!setjmp(loop_observed)) {
  segment_crossing(working(),&observed,entry==0xc2f128u||entry==0xc2f156u,entry==0xc2f0f4u||entry==0xc2f156u?-1:1,0);
  return 0;
 }
 return loop_count==64;
}
