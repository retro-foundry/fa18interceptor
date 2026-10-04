#include "glue_render_leaf_helpers_math.h"
#include "glue_segment_projection.h"
#include "glue_child_call.h"
#include "segment_projection.h"

static SegmentProjectionState working(void) {
 SegmentProjectionState w={D(0),D(1),D(2),D(3),D(4),D(5),D(6),D(7),A(0),A(1),A(2),A(3),A(4),A(5),A(6),A(7),COND_LT(),COND_EQ()};return w;
}
static SegmentProjectionState consume(void *context,enum SegmentProjectionChild child) {
 static const struct {uint32_t entry,ret;} sites[]={
  {0xc2ed70,0xc1fff8},{0xc2ee4a,0xc1fff8},{0xc2fa7e,0xc2ee3c},
  {0xc2f0c6,0xc2ee78},{0xc2f0f4,0xc2ee8e},{0xc2f0f4,0xc2eeb0},{0xc2f0c6,0xc2eec6},
  {0xc2f156,0xc2eeec},{0xc2f128,0xc2ef0a},{0xc2f128,0xc2ef2a},{0xc2f156,0xc2ef50},
  {0xc2f128,0xc2ef6e},{0xc2f156,0xc2ef82},{0xc2f156,0xc2efa6},{0xc2f128,0xc2efba},
  {0xc2f0f4,0xc2efdc},{0xc2f0c6,0xc2eff4},{0xc2f0c6,0xc2f00c},{0xc2f0f4,0xc2f02c},
  {0xc2fa7e,0xc2f08e}
 };
 (void)context;
 /* C1FFF6 is JSR (A4). Its two actual LEA setups are source-sealed. */
 glue_complete_child(child<=SP_SELECTED_CLIPPED?A(4):sites[child].entry,sites[child].ret);return working();
}
static void outputs(void *context,enum SegmentProjectionPhase phase,enum SegmentProjectionField field,uint32_t v,uint32_t other) {
 unsigned r=(unsigned)field;uint32_t temporary;(void)context;
 switch(phase){
 case SP_WORD:SET_W(D(r),v);flags_logic_w(v);break;
 case SP_LONG:D(r)=v;flags_logic_l(v);break;
 case SP_POINTER:A(r-SP_CURSOR)=v;break;
 case SP_RAW_LONG:D(r)=v;break;
 case SP_ADD_WORD:step_add_word(&D(r),v);break;
 case SP_SUB_WORD:step_subtract_word(&D(r),v);break;
 case SP_NEG_WORD:renderer_negate(&D(r),2);break;
 case SP_AND_WORD:SET_W(D(r),D(r)&v);flags_logic_w(D(r));break;
 case SP_MULS:renderer_multiply(&D(r),(uint16_t)v);break;
 case SP_DIVS:renderer_divide(&D(r),(int16_t)v);break;
 case SP_SWAP:step_swap(&D(r));break;
 case SP_ASR_WORD:renderer_asr_word(&D(r),v);break;
 case SP_COMPARE_WORD:step_compare_word(other,v);break;
 case SP_COMPARE_LONG:step_compare_long(other,v);break;
 case SP_TEST_WORD:case SP_STORE_WORD:flags_logic_w(v);break;
 case SP_STORE_LONG:flags_logic_l(v);break;
 case SP_SAVE_LONGS:renderer_store(A(7),v,4,7);break;
 case SP_RESTORE_LONGS:renderer_load(A(7),v,4,7);break;
 case SP_LOAD_WORDS:renderer_load(v,other,2,-1);break;
 case SP_STORE_WORDS:renderer_store(v,other,2,-1);break;
 case SP_LINK:m68ki_push_32(A(6));A(6)=A(7);A(7)-=v;break;
 case SP_UNLINK:A(7)=A(6);A(6)=m68ki_pull_32();break;
 case SP_MEMORY_SUB_WORD:temporary=v;step_subtract_word(&temporary,other);break;
 case SP_PARALLEL_LOOP:REG_PC=REG_PPC=v;USE_CYCLES(10);break;
 }
}
static SegmentProjectionState restored(void *context){(void)context;return working();}
static const SegmentProjectionHooks hooks={consume,outputs,restored,NULL};
int glue_C1FF9C(void){segment_selected(working(),&hooks,0);return glue_return();}
int glue_C1FFA4(void){segment_selected(working(),&hooks,1);return glue_return();}
int glue_C2ED70(void){segment_projected(working(),&hooks);return glue_return();}
int glue_C2EE4A(void){segment_clipped(working(),&hooks);return glue_return();}
int glue_C2F0C6(void){segment_crossing(working(),&hooks,0,1,0);return glue_return();}
int glue_C2F0F4(void){segment_crossing(working(),&hooks,0,-1,0);return glue_return();}
int glue_C2F128(void){segment_crossing(working(),&hooks,1,1,0);return glue_return();}
int glue_C2F156(void){segment_crossing(working(),&hooks,1,-1,0);return glue_return();}
int glue_C2EA5A(void){segment_crossing(working(),&hooks,0,1,1);return glue_return();}
int glue_C2EAD0(void){segment_crossing(working(),&hooks,0,-1,1);return glue_return();}
int glue_C2EB4C(void){segment_crossing(working(),&hooks,1,1,1);return glue_return();}
int glue_C2EBC2(void){segment_crossing(working(),&hooks,1,-1,1);return glue_return();}
