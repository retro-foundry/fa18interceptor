#include "glue_control_readouts_math.h"
#include "glue_control_readouts.h"
#include "glue_child_call.h"
#include "control_readouts.h"
#include <stdlib.h>
static ControlReadoutState working(void){
 ControlReadoutState s={D(0),D(1),D(2),D(3),D(4),D(5),D(6),D(7),A(0),A(1),A(2),A(3),A(4),A(5),A(6),A(7),FLAG_X?1u:0u};return s;
}
static ControlReadoutState consume(void *context,gaddr ret){
 static const struct {uint32_t entry,ret;} sites[]={
  {0xc13176,0xc12ed0},
  {0xc13176,0xc1301a},
  {0xc131be,0xc12d40},
  {0xc13396,0xc1326a},
  {0xc13396,0xc1327e},
  {0xc13396,0xc132ea},
  {0xc133b2,0xc13116},
  {0xc133b2,0xc13128},
  {0xc17b08,0xc12c7c},
  {0xc17c62,0xc12f84},
  {0xc17c62,0xc12ff8},
  {0xc17cf6,0xc12e5a},
  {0xc17cf6,0xc12e84},
  {0xc17cf6,0xc12eae},
  {0xc17cf6,0xc12f10},
  {0xc17cf6,0xc12fa2},
  {0xc17cf6,0xc12fce},
  {0xc17cf6,0xc1305a},
  {0xc17cf6,0xc131b8},
  {0xc17d6e,0xc13088},
  {0xc17d6e,0xc130a6},
  {0xc17d6e,0xc130de},
  {0xc17d6e,0xc1310c},
  {0xc17daa,0xc12f30},
  {0xc17e4a,0xc13120},
  {0xc17e4a,0xc13132},
  {0xc17ef2,0xc12a56},
  {0xc17ef2,0xc12aaa},
  {0xc17ef2,0xc12ae6},
  {0xc17ef2,0xc12b2c},
  {0xc17ef2,0xc12b6a},
  {0xc17ef2,0xc12bac},
  {0xc17ef2,0xc12be8},
  {0xc17ef2,0xc12c26},
  {0xc17ef2,0xc12c62},
  {0xc17f8c,0xc12cc4},
  {0xc17f8c,0xc12ce8},
  {0xc18096,0xc129d2},
  {0xc18096,0xc129e0},
  {0xc180fc,0xc12a04},
  {0xc18108,0xc13192},
  {0xc52ec8,0xc12e10},
  {0xc52ec8,0xc13320},
 };
 unsigned i;(void)context;
 for(i=0;i<sizeof sites/sizeof sites[0];++i)if(sites[i].ret==ret){glue_complete_child(sites[i].entry,ret);return working();}
 abort();
}
static void outputs(void *context,enum ControlReadoutPhase phase,enum ControlReadoutField field,uint32_t v,uint32_t other){
 unsigned r=(unsigned)field;uint32_t temporary;(void)context;
 switch(phase){
 case CR_BYTE:SET_B(D(r),v);flags_logic_b(v);break;
 case CR_WORD:SET_W(D(r),v);flags_logic_w(v);break;
 case CR_LONG:D(r)=v;flags_logic_l(v);break;
 case CR_POINTER:A(r-CR_RECORD)=v;break;
 case CR_STORE_BYTE:case CR_TEST_BYTE:flags_logic_b(v);break;
 case CR_STORE_WORD:case CR_TEST_WORD:flags_logic_w(v);break;
 case CR_STORE_LONG:case CR_TEST_LONG:flags_logic_l(v);break;
 case CR_ADD_BYTE:renderer_add_byte(&D(r),v);break;
 case CR_ADD_WORD:step_add_word(&D(r),v);break;
 case CR_ADD_LONG:step_add_long(&D(r),v);break;
 case CR_SUB_BYTE:step_subtract_byte(&D(r),v);break;
 case CR_SUB_WORD:step_subtract_word(&D(r),v);break;
 case CR_SUB_LONG:step_subtract_long(&D(r),v);break;
 case CR_AND_BYTE:SET_B(D(r),D(r)&v);flags_logic_b(D(r));break;
 case CR_OR_BYTE:SET_B(D(r),D(r)|v);flags_logic_b(D(r));break;
 case CR_XOR_LONG:D(r)^=v;flags_logic_l(D(r));break;
 case CR_ASR_WORD:renderer_asr_word(&D(r),v);break;
 case CR_ASR_LONG:step_asr_long(&D(r),v);break;
 case CR_ASL_WORD:renderer_asl_word(&D(r),v);break;
 case CR_ASL_LONG:step_asl_long(&D(r),v);break;
 case CR_EXT_WORD:SET_W(D(r),(int16_t)(int8_t)D(r));flags_logic_w(D(r));break;
 case CR_EXT_LONG:D(r)=(uint32_t)(int32_t)(int16_t)D(r);flags_logic_l(D(r));break;
 case CR_COMPARE_BYTE:step_compare_byte(other,v);break;
 case CR_COMPARE_WORD:step_compare_word(other,v);break;
 case CR_COMPARE_LONG:step_compare_long(other,v);break;
 case CR_BIT_TEST:FLAG_Z=v&(1u<<other);break;
 case CR_NEGATE_MEMORY_WORD:temporary=v;renderer_negate(&temporary,2);break;
 case CR_ADD_MEMORY_WORD:temporary=v;step_add_word(&temporary,other);break;
 case CR_SUB_MEMORY_WORD:temporary=v;step_subtract_word(&temporary,other);break;
 case CR_LINK:m68ki_push_32(A(6));A(6)=A(7);A(7)-=v;break;
 case CR_UNLINK:A(7)=A(6);A(6)=m68ki_pull_32();break;
 case CR_SAVE_LONGS:renderer_store(A(7),v,4,7);break;
 case CR_RESTORE_LONGS:renderer_load(A(7),v,4,7);break;
 case CR_PUSH_LONG:A(7)-=4;flags_logic_l(v);break;
 case CR_STACK_ADD:A(7)+=v;break;
 case CR_NEG_LONG:renderer_negate(&D(r),4);break;
 case CR_ROXL_LONG:readout_roxl_long(&D(r),v);break;
 case CR_DBRA:SET_W(D(r),D(r)-1);break;
 }
}
static ControlReadoutState restored(void *context){(void)context;return working();}
static const ControlReadoutHooks hooks={consume,outputs,restored,NULL};
int glue_C12950(void){update_control_record_readouts(working(),&hooks);return glue_return();}
int glue_C131BE(void){scale_control_record_magnitude(working(),&hooks);return glue_return();}
int glue_C13176(void){play_control_record_event(working(),&hooks);return glue_return();}
int glue_C133B2(void){read_control_record_step(working(),&hooks);return glue_return();}
int glue_C13396(void){scale_control_five_eighths(working(),&hooks);return glue_return();}
int glue_C52EC8(void){divide_control_long(working(),&hooks);return glue_return();}
