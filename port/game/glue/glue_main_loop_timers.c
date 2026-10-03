#include "glue_main_loop_timers_math.h"
#include "glue_unsigned_division_step.h"
#include "glue_child_call.h"
#include "glue_main_loop_timers.h"
#include "main_loop_timers.h"
#include <stdio.h>
#include <stdlib.h>
static MainTimerBounds consume(void *context,enum MainTimerChild child) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc2574a,0xc252a8},{0xc16d04,0xc25318},{0xc25482,0xc253dc},
        {0xc25482,0xc253e6},{0xc25482,0xc253f0},{0xc16d04,0xc25426}
    };
    MainTimerBounds result; (void)context;
    glue_complete_child(sites[child].entry,sites[child].ret);
    result.width=(int16_t)D(5); result.height=(int16_t)D(7); result.cursor=A(0); result.record=A(1); return result;
}
static void full(unsigned reg,uint32_t v) { D(reg)=v; flags_logic_l(v); }
static void bounds_corners(void) {
    static const uint16_t coefficients[4][2]={{0xffa8,0xf1},{0xff0f,0xffa8},{0xffa8,0xff0f},{0xf1,0xffa8}};
    unsigned i;
    for(i=0;i<4;++i) {
        SET_W(D(6),D(5)); flags_logic_w(D(6)); SET_W(D(i),D(7)); flags_logic_w(D(i));
        renderer_multiply(&D(6),coefficients[i][0]); renderer_multiply(&D(i),coefficients[i][1]);
        step_add_long(&D(i),D(6)); step_asr_long(&D(i),8);
    }
    SET_W(D(4),D(0)); flags_logic_w(D(4)); SET_W(D(5),D(1)); flags_logic_w(D(5));
    renderer_negate(&D(4),2); renderer_negate(&D(5),2);
    step_add_word(&D(0),rd_u16(A(1))); step_add_word(&D(1),rd_u16(A(1)+2));
    step_add_word(&D(2),rd_u16(A(1))); step_add_word(&D(3),rd_u16(A(1)+2));
    step_add_word(&D(4),rd_u16(A(1)+4)); step_add_word(&D(5),rd_u16(A(1)+6));
}
static void outputs(void *context,enum MainTimerPhase p,uint32_t v,uint32_t other) {
    uint32_t temporary; unsigned i; (void)context;
    switch(p) {
    case MT_BOUNDS_BEGIN: A(0)=v; break;
    case MT_BOUNDS_OFFSET: SET_W(D(0),v); flags_logic_w(v); A(0)=other; if((int16_t)v>=0) A(1)=0xc44880u+(uint32_t)(int32_t)(int16_t)v; break;
    case MT_BOUNDS_LOAD: for(i=0;i<4;++i) D(4+i)=(uint32_t)(int32_t)rd_s16(v+2*i); break;
    case MT_BOUNDS_SENTINEL: step_compare_word(0xffff,D(4)); break;
    case MT_BOUNDS_VECTOR:
        step_subtract_word(&D(6),D(4)); step_subtract_word(&D(7),D(5)); full(5,0);
        temporary=D(5); D(5)=D(6); D(6)=temporary; full(0,46); break;
    case MT_BOUNDS_CORNERS: bounds_corners(); break;
    case MT_BOUNDS_NEXT: A(1)=v; break;
    case MT_WORD_STORE: flags_logic_w(v); break;
    case MT_LONG_STORE: flags_logic_l(v); break;
    case MT_BYTE_STORE: case MT_BYTE_TEST: flags_logic_b(v); break;
    case MT_LONG_TEST: flags_logic_l(v); break;
    case MT_ACCUMULATOR_LOAD: full(1,v); break;
    case MT_SAMPLE_SUBTRACT: case MT_TOTAL_SUBTRACT: step_subtract_long(&D(1),v); break;
    case MT_SAMPLE_NEGATE: renderer_negate(&D(1),4); break;
    case MT_SAMPLE_MULTIPLY: timer_multiply_unsigned(&D(1),(uint16_t)v); break;
    case MT_FRACTION_LOAD: full(0,v); break;
    case MT_FRACTION_SUBTRACT: step_subtract_long(&D(0),v); break;
    case MT_FRACTION_DIVIDE: renderer_divide(&D(0),(int16_t)v); break;
    case MT_FRACTION_EXTEND: full(0,v); break;
    case MT_SAMPLE_ADD: step_add_long(&D(1),D(0)); break;
    case MT_TOTAL_ADD: temporary=v; step_add_long(&temporary,other); break;
    case MT_PARTIAL_ADD: temporary=v; step_add_word(&temporary,other); break;
    case MT_PARTIAL_COMPARE: step_compare_word(other,v); break;
    case MT_TOTAL_COMPARE: case MT_POLL_COMPARE: case MT_READOUT_LIMIT: step_compare_long(other,D(1)); break;
    case MT_FLAGS_CLEAR: SET_W(D(0),D(0)&0x100); flags_logic_w(D(0)); break;
    case MT_FLAGS_SET: flags_logic_w(v); break;
    case MT_SAMPLE_COMPARE: step_compare_long(v,D(0)); break;
    case MT_DECREMENT_PRIMARY: case MT_SECONDARY_DECREMENT: temporary=v; step_subtract_byte(&temporary,1); break;
    case MT_SECONDARY_CURSOR: case MT_COUNT_CURSOR: A(0)=v; break;
    case MT_SECONDARY_CLEAR_BIT: FLAG_Z=v&(1u<<other); break;
    case MT_LEVEL_LOAD: SET_B(D(1),v); flags_logic_b(v); break;
    case MT_LEVEL_ADD: renderer_add_byte(&D(1),(uint8_t)v); break;
    case MT_LEVEL_COMPARE: step_compare_byte(other,D(1)); break;
    case MT_LEVEL_MAXIMUM: full(1,v); break;
    case MT_LEVEL_DECREMENT: step_subtract_byte(&D(1),(uint8_t)v); break;
    case MT_DIVISOR_LOAD: SET_W(D(other),v); flags_logic_w(v); break;
    case MT_DIVISOR_MASK: SET_W(D(2),D(2)&v); flags_logic_w(D(2)); break;
    case MT_DIVISOR_INCREMENT: step_add_word(&D(2),v); break;
    case MT_POLL_DIVIDE: step_divide_unsigned(&D(1),(uint16_t)v); break;
    case MT_THRESHOLD_BASE: A(0)=v; break;
    case MT_THRESHOLD_INDEX: SET_B(D(2),v); flags_logic_b(v); break;
    case MT_THRESHOLD_EXTEND: SET_W(D(2),v); flags_logic_w(v); break;
    case MT_THRESHOLD_SCALE: step_add_word(&D(2),D(2)); break;
    case MT_THRESHOLD_COMPARE: step_compare_word(v,D(1)); break;
    case MT_READOUT_SENTINEL: case MT_READOUT_DIVIDEND: full(0,v); break;
    case MT_READOUT_DIVIDE: step_divide_unsigned(&D(0),(uint16_t)v); break;
    case MT_MINIMUM_COMPARE: case MT_MAXIMUM_COMPARE: step_compare_word(v,D(0)); break;
    }
}
static uint32_t divide_zero(void *context) {
    uint32_t sp=A(7); (void)context;
    /* The original DIVU traps with the continuation at C254B0. Preserve that
     * exception frame and execute the actual handler through runtime dispatch,
     * just like a real child; never provide a substitute division result. */
    REG_PPC=0xc254ae; REG_PC=0xc254b0; REG_IR=0x80c1;
    step_divide_unsigned(&D(0),0);
    if(fa18_recomp_resume(0xc254b0,sp)!=FA18_RET) {
        fprintf(stderr,"readout divide exception cannot complete at %06X\n",REG_PC); abort();
    }
    return D(0);
}
static const MainTimerHooks hooks={consume,outputs,divide_zero,NULL};
int glue_C2527C(void) { prepare_setup_bounds(&hooks); return glue_return(); }
int glue_C25312(void) { advance_main_loop_timers(&hooks); return glue_return(); }
int glue_C2548A(void) { sample_main_loop_readout(&hooks); return glue_return(); }
