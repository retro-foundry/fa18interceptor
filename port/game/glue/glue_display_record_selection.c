#include "glue_render_leaf_helpers_math.h"
#include "glue_display_record_selection.h"
#include "glue_child_call.h"
#include "display_record_selection.h"
#include <stdlib.h>
static DisplaySelectionState working(void){DisplaySelectionState s={D(0),D(1),D(2),D(3),D(4),D(5),D(6),D(7),A(0),A(1),A(2),A(3),A(4),A(5),A(6),A(7)};return s;}
static DisplaySelectionState restored(void *context){(void)context;return working();}
static DisplaySelectionState consume(void *context,gaddr ret){
 static const struct {uint32_t entry,ret;} sites[]={
  {0xc0daa0,0xc0d894},
  {0xc0daa0,0xc0d8b2},
  {0xc0daa0,0xc0d8d8},
  {0xc0daa0,0xc0d8fc},
  {0xc0daa0,0xc0d926},
  {0xc0daa0,0xc0d93a},
  {0xc0daa0,0xc0d974},
  {0xc0daa0,0xc0d998},
  {0xc0daa0,0xc0d9c0},
  {0xc0daa0,0xc0d9de},
  {0xc0daa0,0xc0da04},
  {0xc0daa0,0xc0da18},
  {0xc0dad0,0xc0d8aa},
  {0xc0dad0,0xc0d8f4},
  {0xc0dad0,0xc0d932},
  {0xc0dad0,0xc0d946},
  {0xc0dad0,0xc0d980},
  {0xc0dad0,0xc0d9a4},
  {0xc0dad0,0xc0d9c8},
  {0xc0dad0,0xc0d9e6},
  {0xc0dad0,0xc0da0c},
  {0xc0dad0,0xc0da20},
  {0xc0dad0,0xc0da46},
  {0xc0dad4,0xc0d8a6},
  {0xc0dad4,0xc0d8dc},
  {0xc0dad4,0xc0d900},
  {0xc0dad4,0xc0d92e},
  {0xc0dad4,0xc0d942},
  {0xc0dad4,0xc0d990},
  {0xc0dad4,0xc0d9d6},
  {0xc0dad4,0xc0da08},
  {0xc0dad4,0xc0da1c},
  {0xc0dad4,0xc0da4a},
  {0xc0dadc,0xc0d898},
  {0xc0dadc,0xc0d8b6},
  {0xc0dadc,0xc0d8e0},
  {0xc0dadc,0xc0d904},
  {0xc0dadc,0xc0d92a},
  {0xc0dadc,0xc0d93e},
  {0xc0dadc,0xc0d978},
  {0xc0dadc,0xc0d99c},
  {0xc0dadc,0xc0d9d2},
  {0xc0dadc,0xc0da34},
  {0xc0dadc,0xc0da4e},
  {0xc0dae6,0xc0d89c},
  {0xc0dae6,0xc0d8ba},
  {0xc0dae6,0xc0d8e4},
  {0xc0dae6,0xc0d908},
  {0xc0dae6,0xc0d956},
  {0xc0dae6,0xc0d97c},
  {0xc0dae6,0xc0d9a0},
  {0xc0dae6,0xc0d9c4},
  {0xc0dae6,0xc0d9e2},
  {0xc0dae6,0xc0da10},
  {0xc0dae6,0xc0da24},
  {0xc0dae6,0xc0da52},
  {0xc2e758,0xc0d7e0},
 };
 unsigned i;(void)context;
 for(i=0;i<sizeof sites/sizeof sites[0];++i)if(sites[i].ret==ret){glue_complete_child(sites[i].entry,ret);return working();}
 abort();
}
static void outputs(void *context,enum DisplaySelectionPhase phase,enum DisplaySelectionField field,uint32_t v,uint32_t other){
 unsigned r=(unsigned)field;uint32_t temporary;(void)context;
 switch(phase){
 case DS_WORD:SET_W(D(r),v);flags_logic_w(v);break;
 case DS_LONG:D(r)=v;flags_logic_l(v);break;
 case DS_POINTER:A(r-DS_COUNTER)=v;break;
 case DS_STORE_BYTE:case DS_TEST_BYTE:flags_logic_b(v);break;
 case DS_STORE_WORD:case DS_TEST_WORD:flags_logic_w(v);break;
 case DS_STORE_LONG:flags_logic_l(v);break;
 case DS_ADD_WORD:step_add_word(&D(r),v);break;
 case DS_SUB_WORD:step_subtract_word(&D(r),v);break;
 case DS_ADD_LONG:step_add_long(&D(r),v);break;
 case DS_AND_WORD:SET_W(D(r),D(r)&v);flags_logic_w(D(r));break;
 case DS_ASR_WORD:renderer_asr_word(&D(r),v);break;
 case DS_ASR_LONG:step_asr_long(&D(r),v);break;
 case DS_ASL_WORD:renderer_asl_word(&D(r),v);break;
 case DS_SWAP:D(r)=(D(r)<<16)|(D(r)>>16);flags_logic_l(D(r));break;
 case DS_MULTIPLY_WORD:renderer_multiply(&D(r),(int16_t)v);break;
 case DS_COMPARE_WORD:step_compare_word(other,v);break;
 case DS_MEMORY_SUB_WORD:temporary=v;step_subtract_word(&temporary,other);break;
 case DS_LINK:m68ki_push_32(A(6));A(6)=A(7);A(7)-=v;break;
 case DS_UNLINK:A(7)=A(6);A(6)=m68ki_pull_32();break;
 }
}
static const DisplaySelectionHooks hooks={consume,outputs,restored,NULL};
int glue_C0D74A(void){prepare_display_record_selection(working(),1,&hooks);return glue_return();}
int glue_C0D752(void){prepare_display_record_selection(working(),0,&hooks);return glue_return();}
int glue_C0DAA0(void){append_display_mirrored_pair(working(),&hooks);return glue_return();}
int glue_C0DAD0(void){append_display_zero_pair(working(),&hooks);return glue_return();}
int glue_C0DAD4(void){append_display_upper_right(working(),&hooks);return glue_return();}
int glue_C0DADC(void){append_display_lower_right(working(),&hooks);return glue_return();}
int glue_C0DAE6(void){append_display_lower_left(working(),&hooks);return glue_return();}
