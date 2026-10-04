#include "glue_render_leaf_helpers_math.h"
#include "glue_unsigned_division_step.h"
#include "glue_corner_view.h"
#include "glue_child_call.h"
#include "corner_view.h"

static CornerViewState working(void) {
 CornerViewState w={D(0),D(1),D(2),D(3),D(4),D(5),D(6),D(7),A(0),A(1),A(2),A(3),A(4),A(5),A(6),A(7),COND_LT(),COND_EQ()};return w;
}
static CornerViewState consume(void *context,enum CornerViewChild child) {
 static const struct {uint32_t entry,ret;} sites[]={
  {0xc2ea5a,0xc2e7d4},{0xc2ead0,0xc2e7ea},{0xc2ead0,0xc2e81a},{0xc2ea5a,0xc2e830},
  {0xc2ebc2,0xc2e856},{0xc2eb4c,0xc2e874},{0xc2eb4c,0xc2e8a0},{0xc2ebc2,0xc2e8c6},
  {0xc2eb4c,0xc2e8f0},{0xc2ebc2,0xc2e904},{0xc2ebc2,0xc2e934},{0xc2eb4c,0xc2e948},
  {0xc2ead0,0xc2e96c},{0xc2ea5a,0xc2e984},{0xc2ea5a,0xc2e9a8},{0xc2ead0,0xc2e9c8},
  {0xc2ce82,0xc2cd06},{0xc2eca8,0xc2cd12},{0xc2ce82,0xc2ce3c},{0xc2ed70,0xc2ce52},
  {0xc2d3a4,0xc2d0ba},{0xc2d16c,0xc2d11c},{0xc2d16c,0xc2d13a},{0xc2d16c,0xc2d160},{0xc1d974,0xc2d3fa}
 };
 (void)context;glue_complete_child(sites[child].entry,sites[child].ret);return working();
}
static void outputs(void *context,enum CornerViewPhase phase,enum CornerViewField field,uint32_t v,uint32_t other) {
 unsigned r=(unsigned)field;uint32_t temporary;(void)context;
 switch(phase){
 case CV_WORD:SET_W(D(r),v);flags_logic_w(v);break;
 case CV_LONG:D(r)=v;flags_logic_l(v);break;
 case CV_POINTER:A(r-CV_CURSOR)=v;break;
 case CV_RAW_LONG:D(r)=v;break;
 case CV_ADD_WORD:step_add_word(&D(r),v);break;
 case CV_SUB_WORD:step_subtract_word(&D(r),v);break;
 case CV_NEG_WORD:renderer_negate(&D(r),2);break;
 case CV_AND_WORD:SET_W(D(r),D(r)&v);flags_logic_w(D(r));break;
 case CV_MULS:renderer_multiply(&D(r),(uint16_t)v);break;
 case CV_DIVS:renderer_divide(&D(r),(int16_t)v);break;
 case CV_SWAP:step_swap(&D(r));break;
 case CV_ASR_WORD:renderer_asr_word(&D(r),v);break;
 case CV_COMPARE_WORD:step_compare_word(other,v);break;
 case CV_COMPARE_LONG:step_compare_long(other,v);break;
 case CV_TEST_WORD:case CV_STORE_WORD:flags_logic_w(v);break;
 case CV_STORE_LONG:flags_logic_l(v);break;
 case CV_SAVE_LONGS:renderer_store(A(7),v,4,7);break;
 case CV_RESTORE_LONGS:renderer_load(A(7),v,4,7);break;
 case CV_LOAD_WORDS:renderer_load(v,other,2,-1);break;
 case CV_STORE_WORDS:renderer_store(v,other,2,-1);break;
 case CV_LINK:m68ki_push_32(A(6));A(6)=A(7);A(7)-=v;break;
 case CV_UNLINK:A(7)=A(6);A(6)=m68ki_pull_32();break;
 case CV_MEMORY_SUB_WORD:temporary=v;step_subtract_word(&temporary,other);break;
 case CV_PARALLEL_LOOP:REG_PC=REG_PPC=v;USE_CYCLES(10);break;
 case CV_ADD_LONG:step_add_long(&D(r),v);break;
 case CV_NEG_LONG:renderer_negate(&D(r),4);break;
 case CV_ASL_WORD:renderer_asl_word(&D(r),v);break;
 case CV_ASR_LONG:step_asr_long(&D(r),v);break;
 case CV_EXT_LONG:D(r)=(uint32_t)(int32_t)(int16_t)D(r);flags_logic_l(D(r));break;
 case CV_MULU:timer_multiply_unsigned(&D(r),(uint16_t)v);break;
 case CV_DIVU:step_divide_unsigned(&D(r),(uint16_t)v);break;
 case CV_LOAD_WORDS_POST:renderer_load(A(0),v,2,0);break;
 case CV_LOAD_WORDS_STREAM:renderer_load(A(2),v,2,2);break;
 case CV_RESTORE_WORDS:renderer_load(A(7),v,2,7);break;
 case CV_SAVE_WORDS:renderer_store(A(7),v,2,7);break;
 case CV_TEST_BYTE:case CV_STORE_BYTE:flags_logic_b(v);break;
 case CV_BIT_ZERO:FLAG_Z=v&1u;break;
 case CV_MEMORY_ADD_WORD:temporary=v;step_add_word(&temporary,other);break;
 case CV_COMPARE_BYTE:step_compare_byte(other,v);break;
 case CV_PUSH_POINTER:m68ki_push_32(v);flags_logic_l(v);break;
 case CV_POP_POINTER:A(r-CV_CURSOR)=m68ki_pull_32();break;
 case CV_RESTORE_LONGS_POST:renderer_load(A(7),v,4,7);break;
 case CV_LOAD_LONGS:renderer_load(v,other,4,-1);break;
 }
}
static CornerViewState restored(void *context){(void)context;return working();}
static const CornerViewHooks hooks={consume,outputs,restored,NULL};
int glue_C2E758(void){corner_project_edges(working(),&hooks);return glue_return();}
int glue_C2CE82(void){corner_rotate_view(working(),&hooks);return glue_return();}
int glue_C2CCA0(void){corner_test_record(working(),&hooks,0);return glue_return();}
int glue_C2CD28(void){corner_test_record(working(),&hooks,1);return glue_return();}
int glue_C2CD94(void){corner_draw_record_pairs(working(),&hooks);return glue_return();}
int glue_C2D082(void){corner_draw_layers(working(),&hooks);return glue_return();}
int glue_C2D3A4(void){corner_distance(working(),&hooks);return glue_return();}
int glue_C200F6(void){corner_return_list(working(),&hooks);return glue_return();}
int glue_C203CC(void){corner_reject(working(),&hooks,-1);return glue_return();}
int glue_C2058E(void){corner_reject(working(),&hooks,-1);return glue_return();}
int glue_C20826(void){corner_reject(working(),&hooks,0);return glue_return();}
int glue_C22C70(void){return glue_return();}
