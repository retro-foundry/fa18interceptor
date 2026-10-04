#include "glue_render_leaf_helpers_math.h"
#include "glue_record_steering.h"
#include "record_steering.h"
static RecordSteeringState working(void){RecordSteeringState s={D(1),D(2),D(3),A(1)};return s;}
static void outputs(void *context,enum SteeringPhase phase,enum SteeringField field,uint32_t v,uint32_t other){
 unsigned r=(unsigned)field;(void)context;
 switch(phase){
 case ST_BYTE:SET_B(D(r),v);flags_logic_b(v);break;
 case ST_WORD:SET_W(D(r),v);flags_logic_w(v);break;
 case ST_LONG:D(r)=v;flags_logic_l(v);break;
 case ST_AND_BYTE:SET_B(D(r),D(r)&v);flags_logic_b(D(r));break;
 case ST_OR_BYTE:SET_B(D(r),D(r)|v);flags_logic_b(D(r));break;
 case ST_TEST_WORD:flags_logic_w(v);break;
 case ST_COMPARE_BYTE:step_compare_byte(other,v);break;
 case ST_COMPARE_WORD:step_compare_word(other,v);break;
 case ST_BIT_TEST:FLAG_Z=v&(1u<<other);break;
 case ST_STORE_BYTE:flags_logic_b(v);break;
 }
}
static const RecordSteeringHooks hooks={outputs,NULL};
int glue_C2CA26(void){select_record_turn(working(),&hooks);return glue_return();}
int glue_C2CA92(void){select_record_roll(working(),&hooks);return glue_return();}
int glue_C2CAA0(void){select_record_neutral(working(),&hooks);return glue_return();}
int glue_C2CB86(void){select_record_pitch(working(),&hooks);return glue_return();}
int glue_C2CB82(void){select_record_pitch_preserving_controls(working(),&hooks);return glue_return();}
int glue_C2CBBC(void){select_record_pitch_branch(working(),&hooks);return glue_return();}
