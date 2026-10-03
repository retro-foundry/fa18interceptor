#include "glue_main_loop_flight_controls_math.h"
#include "glue_unsigned_division_step.h"
#include "glue_child_call.h"
#include "glue_main_loop_flight_controls.h"
#include "main_loop_flight_controls.h"
#include <stdio.h>
#include <stdlib.h>
static FlightWorking working(void) {
    FlightWorking w={D(0),D(1),D(2),D(3),D(4),D(5),D(6),D(7),A(0),A(1),A(2),A(3),A(4),COND_EQ()}; return w;
}
static void full(unsigned r,uint32_t v) { D(r)=v; flags_logic_l(v); }
static void partial(unsigned r,uint16_t v) { SET_W(D(r),v); flags_logic_w(v); }
static void octet(unsigned r,uint8_t v) { SET_B(D(r),v); flags_logic_b(v); }
static void extended(unsigned r) { full(r,(uint32_t)(int32_t)(int16_t)D(r)); }
static void push(uint32_t v) { m68ki_push_32(v); flags_logic_l(v); }
static FlightWorking consume(void *context,enum FlightChild child) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc25754,0xc14a5a},{0xc15138,0xc14ad2},{0xc15138,0xc14aec},{0xc15138,0xc14b2a},
        {0xc2b05a,0xc14d9a},{0xc16d04,0xc14e1a},{0xc1803c,0xc14ef0},{0xc1803c,0xc14f00},
        {0xc083e2,0xc14f5c},{0xc16d04,0xc15098},{0xc083a6,0xc15104},
        {0xc25704,0xc083ee},{0xc1d974,0xc25784},{0xc1d974,0xc245f6},{0xc2574a,0xc243a2},
        {0xc24568,0xc23b2c},{0xc06c02,0xc23d04},{0xc1bee8,0xc23e8e},{0xc1b7a6,0xc23e9a},{0xc091e0,0xc24180},{0xc2436a,0xc241b4},{0xc06c02,0xc24330}
    };
    unsigned arguments=0; uint32_t temporary; (void)context;
    if(child==FC_NORMALISE_CONTROL) { push(D(2)); push(D(4)); push(D(3)); push(D(1)); flags_logic_l(A(2)); arguments=16; }
    else if(child==FC_ATTENUATE_X || child==FC_ATTENUATE_Y) { push(D(1)); push(D(0)); arguments=8; }
    else if(child==FC_ATTENUATE_Z) { push(D(2)); push(D(1)); flags_logic_w(D(0)); arguments=8; }
    else if(child==FC_TOUCHDOWN_FAST_TONE || child==FC_TOUCHDOWN_SLOW_TONE) { full(0,child==FC_TOUCHDOWN_FAST_TONE?40:30); push(D(0)); arguments=4; }
    if(child==FC_PROJECT_VIEW) { temporary=A(1); A(1)=A(3); A(3)=temporary; }
    glue_complete_child(sites[child].entry,sites[child].ret); A(7)+=arguments;
    if(child==FC_PROJECT_VIEW) { temporary=A(1); A(1)=A(3); A(3)=temporary; }
    return working();
}
static void control_setup(uint32_t frame) {
    partial(0,rd_u16(0xc459b4)); full(1,9); flags_logic_w(D(0)); extended(0); step_asl_long(&D(0),D(1)); A(0)=D(0)+0xc46184; full(0,A(0));
    A(0)=D(0)+2; A(1)=D(0)+4; A(2)=D(0)+32; A(3)=D(0);
    partial(1,rd_u16(A(3)+150)); flags_logic_w(D(1)); partial(2,rd_u16(A(3)+150)); flags_logic_w(D(2));
    partial(1,rd_u16(A(3)+156)); flags_logic_w(D(1)); partial(2,rd_u16(A(3)+162)); flags_logic_w(D(2));
    partial(1,rd_u16(A(3)+110)); renderer_negate(&D(1),2); flags_logic_w(D(1)); extended(1);
    partial(3,rd_u16(A(3)+150)); extended(3); partial(4,rd_u16(A(3)+156)); extended(4); extended(2); (void)frame;
}
static void range_components(uint32_t record) {
    partial(4,rd_u16(record+46)); partial(0,rd_u16(record+48)); partial(1,rd_u16(record+50)); full(3,rd_u32(record+52)); extended(2); extended(4);
    step_subtract_word(&D(2),rd_u16(record+6)); step_swap(&D(2)); step_asr_long(&D(2),2); step_subtract_word(&D(0),rd_u16(record+12)); extended(0); step_add_long(&D(2),D(0)); if(COND_LT()) renderer_negate(&D(2),4);
    step_subtract_word(&D(4),rd_u16(record+8)); step_swap(&D(4)); step_asr_long(&D(4),2); step_subtract_word(&D(1),rd_u16(record+14)); extended(1); step_add_long(&D(4),D(1)); if(COND_LT()) renderer_negate(&D(4),4);
    step_subtract_long(&D(3),rd_u32(record+16)); if(COND_LT()) renderer_negate(&D(3),4);
}
static void outputs(void *context,enum FlightPhase p,uint32_t v,uint32_t other) {
    uint32_t temporary; unsigned i; (void)context;
    switch(p) {
#define BYTE_PHASE(name,reg) case name: octet(reg,v); break
#define WORD_PHASE(name,reg) case name: partial(reg,v); break
#define LONG_PHASE(name,reg) case name: full(reg,v); break
    BYTE_PHASE(FC_VALUE_BYTE,0); WORD_PHASE(FC_VALUE_WORD,0); LONG_PHASE(FC_VALUE_LONG,0);
    BYTE_PHASE(FC_SPEED_BYTE,1); WORD_PHASE(FC_SPEED_WORD,1); LONG_PHASE(FC_SPEED_LONG,1);
    BYTE_PHASE(FC_TURN_BYTE,2); WORD_PHASE(FC_TURN_WORD,2); LONG_PHASE(FC_TURN_LONG,2);
    BYTE_PHASE(FC_X_BYTE,3); WORD_PHASE(FC_X_WORD,3); LONG_PHASE(FC_X_LONG,3);
    BYTE_PHASE(FC_Y_BYTE,4); WORD_PHASE(FC_Y_WORD,4); LONG_PHASE(FC_Y_LONG,4);
    WORD_PHASE(FC_Z_WORD,5); LONG_PHASE(FC_Z_LONG,5); WORD_PHASE(FC_RATE_WORD,6); LONG_PHASE(FC_RATE_LONG,6); WORD_PHASE(FC_DEPTH_WORD,7); LONG_PHASE(FC_DEPTH_LONG,7);
#undef BYTE_PHASE
#undef WORD_PHASE
#undef LONG_PHASE
    case FC_CURRENT: A(0)=v; break; case FC_RECORD: A(1)=v; break; case FC_AUXILIARY: A(2)=v; break; case FC_VIEWER: A(3)=v; break; case FC_ROUTE: A(4)=v; break;
    case FC_STORE_BYTE: case FC_TEST_BYTE: flags_logic_b(v); break;
    case FC_STORE_WORD: case FC_TEST_WORD: flags_logic_w(v); break;
    case FC_STORE_LONG: case FC_TEST_LONG: flags_logic_l(v); break;
    case FC_COMPARE_BYTE: step_compare_byte(other,v); break; case FC_COMPARE_WORD: step_compare_word(other,v); break; case FC_COMPARE_LONG: step_compare_long(other,v); break;
    case FC_BIT_TEST: case FC_MEMORY_BIT_SET: case FC_MEMORY_BIT_CLEAR: case FC_MEMORY_BIT_CHANGE: FLAG_Z=v&(1u<<other); break;
    case FC_VALUE_EXT_WORD: partial(0,(int16_t)(int8_t)D(0)); break; case FC_SPEED_EXT_WORD: partial(1,(int16_t)(int8_t)D(1)); break;
#define EXT_PHASE(name,reg) case name: extended(reg); break
    EXT_PHASE(FC_VALUE_EXT_LONG,0); EXT_PHASE(FC_SPEED_EXT_LONG,1); EXT_PHASE(FC_TURN_EXT_LONG,2);
    EXT_PHASE(FC_X_EXT_LONG,3); EXT_PHASE(FC_Y_EXT_LONG,4); EXT_PHASE(FC_Z_EXT_LONG,5); EXT_PHASE(FC_DEPTH_EXT_LONG,7);
#undef EXT_PHASE
    case FC_VALUE_AND_BYTE: octet(0,D(0)&v); break; case FC_VALUE_AND_WORD: partial(0,D(0)&v); break;
    case FC_VALUE_OR_BYTE: octet(0,D(0)|v); break; case FC_VALUE_OR_WORD: partial(0,D(0)|v); break;
    case FC_SPEED_AND_WORD: partial(1,D(1)&v); break; case FC_TURN_AND_WORD: partial(2,D(2)&v); break; case FC_X_AND_WORD: partial(3,D(3)&v); break; case FC_Y_AND_WORD: partial(4,D(4)&v); break;
    case FC_SPEED_OR_BYTE: octet(1,D(1)|v); break;
#define ARITH_PHASE(name,reg,fn) case name: fn(&D(reg),v); break
    ARITH_PHASE(FC_VALUE_ADD_BYTE,0,renderer_add_byte); ARITH_PHASE(FC_VALUE_SUB_BYTE,0,step_subtract_byte);
    ARITH_PHASE(FC_VALUE_ADD_WORD,0,step_add_word); ARITH_PHASE(FC_VALUE_SUB_WORD,0,step_subtract_word); ARITH_PHASE(FC_VALUE_ADD_LONG,0,step_add_long); ARITH_PHASE(FC_VALUE_SUB_LONG,0,step_subtract_long);
    ARITH_PHASE(FC_SPEED_ADD_WORD,1,step_add_word); ARITH_PHASE(FC_SPEED_SUB_BYTE,1,step_subtract_byte); ARITH_PHASE(FC_SPEED_SUB_WORD,1,step_subtract_word); ARITH_PHASE(FC_SPEED_ADD_LONG,1,step_add_long); ARITH_PHASE(FC_SPEED_SUB_LONG,1,step_subtract_long);
    ARITH_PHASE(FC_TURN_ADD_WORD,2,step_add_word); ARITH_PHASE(FC_TURN_SUB_WORD,2,step_subtract_word); ARITH_PHASE(FC_TURN_ADD_LONG,2,step_add_long); ARITH_PHASE(FC_TURN_SUB_LONG,2,step_subtract_long);
    ARITH_PHASE(FC_X_ADD_BYTE,3,renderer_add_byte); ARITH_PHASE(FC_X_SUB_BYTE,3,step_subtract_byte); ARITH_PHASE(FC_X_ADD_WORD,3,step_add_word); ARITH_PHASE(FC_X_SUB_WORD,3,step_subtract_word); ARITH_PHASE(FC_X_ADD_LONG,3,step_add_long); ARITH_PHASE(FC_X_SUB_LONG,3,step_subtract_long);
    ARITH_PHASE(FC_Y_ADD_WORD,4,step_add_word); ARITH_PHASE(FC_Y_ADD_LONG,4,step_add_long); ARITH_PHASE(FC_Y_SUB_LONG,4,step_subtract_long); ARITH_PHASE(FC_Z_ADD_LONG,5,step_add_long); ARITH_PHASE(FC_Z_SUB_LONG,5,step_subtract_long); ARITH_PHASE(FC_DEPTH_ADD_LONG,7,step_add_long);
    ARITH_PHASE(FC_VALUE_ASR_WORD,0,renderer_asr_word); ARITH_PHASE(FC_SPEED_ASR_WORD,1,renderer_asr_word); ARITH_PHASE(FC_TURN_ASR_WORD,2,renderer_asr_word);
    ARITH_PHASE(FC_VALUE_ASR_LONG,0,step_asr_long); ARITH_PHASE(FC_SPEED_ASR_LONG,1,step_asr_long); ARITH_PHASE(FC_TURN_ASR_LONG,2,step_asr_long);
    ARITH_PHASE(FC_X_ASR_LONG,3,step_asr_long); ARITH_PHASE(FC_Y_ASR_LONG,4,step_asr_long); ARITH_PHASE(FC_Z_ASR_LONG,5,step_asr_long); ARITH_PHASE(FC_DEPTH_ASR_LONG,7,step_asr_long);
    ARITH_PHASE(FC_VALUE_ASL_WORD,0,renderer_asl_word); ARITH_PHASE(FC_VALUE_ASL_LONG,0,step_asl_long); ARITH_PHASE(FC_SPEED_ASL_WORD,1,renderer_asl_word); ARITH_PHASE(FC_SPEED_ASL_LONG,1,step_asl_long); ARITH_PHASE(FC_TURN_ASL_LONG,2,step_asl_long);
    ARITH_PHASE(FC_X_ASL_WORD,3,renderer_asl_word); ARITH_PHASE(FC_Z_ASL_WORD,5,renderer_asl_word); ARITH_PHASE(FC_VALUE_LSR_WORD,0,flight_lsr_word);
#undef ARITH_PHASE
#define SWAP_PHASE(name,reg) case name: step_swap(&D(reg)); break
    SWAP_PHASE(FC_VALUE_SWAP,0); SWAP_PHASE(FC_TURN_SWAP,2); SWAP_PHASE(FC_X_SWAP,3); SWAP_PHASE(FC_Y_SWAP,4); SWAP_PHASE(FC_Z_SWAP,5);
#undef SWAP_PHASE
#define NEG_PHASE(name,reg,width) case name: renderer_negate(&D(reg),width); break
    NEG_PHASE(FC_VALUE_NEG_WORD,0,2); NEG_PHASE(FC_VALUE_NEG_LONG,0,4); NEG_PHASE(FC_SPEED_NEG_WORD,1,2); NEG_PHASE(FC_SPEED_NEG_LONG,1,4); NEG_PHASE(FC_TURN_NEG_WORD,2,2); NEG_PHASE(FC_TURN_NEG_LONG,2,4);
    NEG_PHASE(FC_X_NEG_WORD,3,2); NEG_PHASE(FC_X_NEG_LONG,3,4); NEG_PHASE(FC_Y_NEG_BYTE,4,1); NEG_PHASE(FC_Y_NEG_WORD,4,2); NEG_PHASE(FC_Y_NEG_LONG,4,4);
    NEG_PHASE(FC_Z_NEG_WORD,5,2); NEG_PHASE(FC_Z_NEG_LONG,5,4); NEG_PHASE(FC_RATE_NEG_WORD,6,2); NEG_PHASE(FC_DEPTH_NEG_WORD,7,2);
#undef NEG_PHASE
#define MUL_PHASE(name,reg) case name: renderer_multiply(&D(reg),(uint16_t)v); break
    MUL_PHASE(FC_TURN_MULTIPLY,2); MUL_PHASE(FC_X_MULTIPLY,3); MUL_PHASE(FC_Y_MULTIPLY,4); MUL_PHASE(FC_Z_MULTIPLY,5); MUL_PHASE(FC_RATE_MULTIPLY,6); MUL_PHASE(FC_DEPTH_MULTIPLY,7);
#undef MUL_PHASE
    case FC_MEMORY_ADD_BYTE: temporary=v; renderer_add_byte(&temporary,other); break; case FC_MEMORY_SUB_BYTE: temporary=v; step_subtract_byte(&temporary,other); break;
    case FC_MEMORY_ADD_WORD: temporary=v; step_add_word(&temporary,other); break; case FC_MEMORY_SUB_WORD: temporary=v; step_subtract_word(&temporary,other); break;
    case FC_MEMORY_ADD_LONG: temporary=v; step_add_long(&temporary,other); break; case FC_MEMORY_SUB_LONG: temporary=v; step_subtract_long(&temporary,other); break;
    case FC_CONTROL_SETUP: control_setup(v); break;
    case FC_RESET_SLOTS:
        renderer_store(A(7),0x88c0,4,7); A(0)=0xc46384; full(0,2);
        for(i=0;i<3;++i) { full(4,40); A(1)=A(0)+164; flags_logic_l(0); SET_W(D(4),0xffff); A(0)+=512; SET_W(D(0),(uint16_t)(D(0)-1)); }
        renderer_load(A(7),0x311,4,7); break;
    case FC_NORMALISE_ARGUMENTS: renderer_load(v+8,0xe1,4,-1); break;
    case FC_NORMALISE_DIVIDE: step_divide_unsigned(&D(0),(uint16_t)v); break;
    case FC_NORMALISE_PRODUCTS:
        renderer_multiply(&D(5),D(0)); renderer_multiply(&D(6),D(0)); renderer_multiply(&D(7),D(0));
        step_asr_long(&D(5),D(2)); step_asr_long(&D(6),D(2)); step_asr_long(&D(7),D(2)); break;
    case FC_NORMALISE_PUBLISH: extended(5); extended(6); extended(7); break;
    case FC_RANGE_COMPONENTS: range_components(v); break;
    case FC_SIGHT_DIFFERENCE:
        renderer_load(other+20,0xe0,4,-1); step_subtract_long(&D(5),rd_u32(v+20)); step_subtract_long(&D(6),rd_u32(v+24)); step_subtract_long(&D(7),rd_u32(v+28));
        step_asr_long(&D(5),8); step_asr_long(&D(6),8); step_asr_long(&D(7),8); partial(0,192); break;
    case FC_SIGHT_FACING:
        renderer_multiply(&D(5),rd_u16(v+150)); renderer_multiply(&D(6),rd_u16(v+156)); renderer_multiply(&D(7),rd_u16(v+162)); step_add_long(&D(7),D(5)); step_add_long(&D(7),D(6)); break;
    case FC_SIGHT_ALIGNMENT:
        partial(2,rd_u16(other+150)); partial(3,rd_u16(other+156)); partial(4,rd_u16(other+162));
        renderer_multiply(&D(2),rd_u16(v+150)); renderer_multiply(&D(3),rd_u16(v+156)); renderer_multiply(&D(4),rd_u16(v+162)); step_add_long(&D(4),D(2)); step_add_long(&D(4),D(3)); break;
    case FC_EXCHANGE_Y_Z: temporary=D(4); D(4)=D(5); D(5)=temporary; break;
    case FC_SAVE_MOTION: renderer_store(A(7),0xe000,4,7); break; case FC_RESTORE_MOTION: renderer_load(A(7),7,4,7); break;
    case FC_MOTION_LOAD: renderer_load(v+62,7,4,-1); break; case FC_MOTION_STORE: break;
    case FC_POSITION_LOAD: renderer_load(v+20,7,4,-1); break; case FC_REFERENCE_LOAD: renderer_load(0xc46198,0x38,4,-1); break;
    case FC_ROUTE_LOAD: renderer_load(v,0x1f,2,-1); break; case FC_ROUTE_STORE: break;
    case FC_REFERENCE_PAIR_LOAD: renderer_load(v,0x0c,2,-1); break; case FC_REFERENCE_PAIR_STORE: break;
    case FC_ZONE_VIEW_LOAD: renderer_load(A(4),0x7c,2,4); break;
    case FC_SELECTED_CONTROL_LOAD: renderer_load(v,7,4,-1); break;
    case FC_PROJECTED_VIEW_STORE: break;
    }
}
static FlightWorking divide_exception(void *context) {
    uint32_t sp=A(7); (void)context;
    REG_PPC=0xc257a8; REG_PC=0xc257aa; REG_IR=0x80c1; step_divide_unsigned(&D(0),0);
    if(fa18_recomp_resume(0xc257aa,sp)!=FA18_RET) { fprintf(stderr,"normalisation divide exception cannot complete at %06X\n",REG_PC); abort(); }
    return working();
}
static const FlightHooks hooks={consume,outputs,divide_exception,NULL};
static void frame_begin(unsigned bytes,uint16_t saved) { m68ki_push_32(A(6)); A(6)=A(7); A(7)-=bytes; if(saved) renderer_store(A(7),saved,4,7); }
static int frame_end(uint16_t saved) { if(saved) renderer_load(A(7),saved,4,7); A(7)=A(6); A(6)=m68ki_pull_32(); return glue_return(); }
int glue_C149BE(void) { frame_begin(54,0x3830); advance_main_loop_flight_controls(A(6),&hooks); return frame_end(0xc1c); }
int glue_C083E2(void) { begin_main_loop_mission_reset(&hooks); return glue_return(); }
int glue_C25754(void) { frame_begin(4,0); normalise_main_loop_control_vector(A(6),&hooks); return frame_end(0); }
int glue_C23A7E(void) { advance_main_loop_flight_record(working(),&hooks); return glue_return(); }
int glue_C24568(void) { classify_main_loop_record_range(working(),&hooks); return glue_return(); }
int glue_C2436A(void) { update_main_loop_record_sight(working(),&hooks); return glue_return(); }
