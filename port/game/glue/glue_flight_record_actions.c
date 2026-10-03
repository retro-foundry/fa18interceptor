#include "glue_flight_record_actions_math.h"
#include "glue_unsigned_division_step.h"
#include "glue_child_call.h"
#include "glue_flight_record_actions.h"
#include "flight_record_actions.h"
#include <stdio.h>
#include <stdlib.h>
static FlightActionState working(void) {
    FlightActionState w={D(0),D(1),D(2),D(3),D(4),D(5),D(6),D(7),A(0),A(1),A(2),A(3),A(4),A(5)}; return w;
}
static void full(unsigned r,uint32_t v) { D(r)=v; flags_logic_l(v); }
static void partial(unsigned r,uint16_t v) { SET_W(D(r),v); flags_logic_w(v); }
static void octet(unsigned r,uint8_t v) { SET_B(D(r),v); flags_logic_b(v); }
static void extended(unsigned r) { full(r,(uint32_t)(int32_t)(int16_t)D(r)); }
static FlightActionState consume(void *context,enum FlightActionChild child) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc23716,0xc23114},{0xc23186,0xc23140},{0xc236aa,0xc2316e},{0xc17ef2,0xc2319a},
        {0xc3316e,0xc2325c},{0xc25704,0xc232bc},{0xc091e0,0xc23300},{0xc28722,0xc23414},
        {0xc23578,0xc23426},{0xc25704,0xc234e6},{0xc28722,0xc23506},{0xc23578,0xc23518},
        {0xc25704,0xc23538},{0xc25704,0xc23620},{0xc1bee8,0xc236cc},{0xc2dee0,0xc236fe},
        {0xc2d954,0xc2370a},{0xc2574a,0xc23a0c},{0xc06c02,0xc257ea},{0xc1d974,0xc25816}
    };
    uint32_t temporary; (void)context;
    if(child==FA_CLONE_PROJECTION) { temporary=A(1); A(1)=A(2); A(2)=temporary; }
    glue_complete_child(sites[child].entry,sites[child].ret);
    if(child==FA_CLONE_PROJECTION) { temporary=A(1); A(1)=A(2); A(2)=temporary; }
    if(child==FA_SOUND_MESSAGE) A(7)+=36;
    return working();
}
static void outputs(void *context,enum FlightActionPhase phase,enum FlightActionValue field,uint32_t v,uint32_t other) {
    unsigned r=(unsigned)field; uint32_t temporary; (void)context;
    switch(phase) {
    case FA_BYTE: octet(r,v); break; case FA_WORD: partial(r,v); break; case FA_LONG: full(r,v); break;
    case FA_POINTER: A(r-FA_STATS)=v; break;
    case FA_ADD_BYTE: renderer_add_byte(&D(r),v); break; case FA_ADD_WORD: step_add_word(&D(r),v); break;
    case FA_ADD_LONG: if(field==FA_STATS) { temporary=v; step_add_long(&temporary,other); } else step_add_long(&D(r),v); break;
    case FA_SUB_BYTE: step_subtract_byte(&D(r),v); break; case FA_SUB_WORD: step_subtract_word(&D(r),v); break; case FA_SUB_LONG: step_subtract_long(&D(r),v); break;
    case FA_AND_BYTE: octet(r,D(r)&v); break; case FA_AND_WORD: partial(r,D(r)&v); break; case FA_OR_BYTE: octet(r,D(r)|v); break;
    case FA_EXT_WORD: partial(r,(int16_t)(int8_t)D(r)); break; case FA_EXT_LONG: extended(r); break;
    case FA_ASR_WORD: renderer_asr_word(&D(r),v); break; case FA_ASR_LONG: step_asr_long(&D(r),v); break;
    case FA_ASL_WORD: renderer_asl_word(&D(r),v); break; case FA_ASL_LONG: step_asl_long(&D(r),v); break;
    case FA_LSR_BYTE: action_lsr_byte(&D(r),v); break; case FA_LSR_WORD: flight_lsr_word(&D(r),v); break;
    case FA_SWAP: step_swap(&D(r)); break; case FA_NEG_WORD: renderer_negate(&D(r),2); break; case FA_MULTIPLY: renderer_multiply(&D(r),(uint16_t)v); break;
    case FA_TEST_BYTE: case FA_STORE_BYTE: flags_logic_b(v); break; case FA_TEST_WORD: case FA_STORE_WORD: flags_logic_w(v); break; case FA_STORE_LONG: flags_logic_l(v); break;
    case FA_COMPARE_BYTE: step_compare_byte(other,v); break; case FA_COMPARE_WORD: step_compare_word(other,v); break;
    case FA_COMPARE_LONG: case FA_COMPARE_ADDRESS: step_compare_long(other,v); break;
    case FA_BIT_TEST: case FA_MEMORY_BIT_SET: case FA_MEMORY_BIT_CLEAR: FLAG_Z=v&(1u<<other); break;
    case FA_MEMORY_ADD_WORD: temporary=v; step_add_word(&temporary,other); break;
    case FA_COPY_RECORD: A(4)=v+164; A(5)=other+164; full(0,40); SET_W(D(0),0xffff); break;
    case FA_SOUND_ARGUMENTS: renderer_load(v,0x1ff,2,-1); renderer_store(A(7),0xff80,4,7); break;
    case FA_SAVE_RECORD: m68ki_push_32(A(1)); flags_logic_l(A(1)); break; case FA_RESTORE_RECORD: A(1)=m68ki_pull_32(); break;
    case FA_SAVE_RECORD_SOURCES: renderer_store(A(7),0x60,4,7); break; case FA_RESTORE_RECORD_SOURCES: renderer_load(A(7),0x600,4,7); break;
    case FA_LOAD_DESCRIPTOR: renderer_load(v,0x7c,4,-1); break; case FA_LOAD_COEFFICIENTS: renderer_load(v,0x38,2,-1); break;
    case FA_LOAD_MOTION: renderer_load(v,7,4,-1); break; case FA_DIRECTION_ARGUMENTS: renderer_load(v,0xe1,4,-1); break;
    case FA_DIRECTION_DIVIDE: step_divide_unsigned(&D(0),(uint16_t)v); break;
    case FA_DIRECTION_PUBLISH: extended(5); extended(6); extended(7); break;
    }
}
static FlightActionState divide_exception(void *context) {
    uint32_t sp=A(7); (void)context;
    REG_PPC=0xc25838; REG_PC=0xc2583a; REG_IR=0x80c1; step_divide_unsigned(&D(0),0);
    if(fa18_recomp_resume(0xc2583a,sp)!=FA18_RET) { fprintf(stderr,"direction divide exception cannot complete at %06X\n",REG_PC); abort(); }
    return working();
}
static const FlightActionHooks hooks={consume,outputs,divide_exception,NULL};
static int select_action(int release) { m68ki_push_32(A(2)); flags_logic_l(A(2)); select_flight_record_action(working(),release,&hooks); A(2)=m68ki_pull_32(); return glue_return(); }
int glue_C230E8(void) { return select_action(1); }
int glue_C23116(void) { return select_action(0); }
int glue_C23186(void) { m68ki_push_32(A(1)); flags_logic_l(A(1)); queue_flight_record_action_sound(&hooks); A(1)=m68ki_pull_32(); return glue_return(); }
int glue_C23228(void) { advance_flight_record_control(working(),&hooks); return glue_return(); }
int glue_C233AA(void) { advance_flight_record_stream(working(),&hooks); return glue_return(); }
int glue_C23578(void) { select_next_flight_record_stream(working(),&hooks); return glue_return(); }
int glue_C236AA(void) { initialise_flight_record_manoeuvre(working(),&hooks); return glue_return(); }
int glue_C23716(void) { initialise_flight_record_release(working(),&hooks); return glue_return(); }
int glue_C2377E(void) { try_flight_record_action(working(),&hooks); return glue_return(); }
int glue_C257EC(void) { m68ki_push_32(A(6)); A(6)=A(7); normalise_flight_record_direction(A(6),&hooks); A(7)=A(6); A(6)=m68ki_pull_32(); return glue_return(); }
