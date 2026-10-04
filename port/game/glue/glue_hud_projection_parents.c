#include "glue_hud_projection_parents_math.h"
#include "glue_hud_projection_parents.h"
#include "glue_child_call.h"
#include "hud_projection_parents.h"
#include "glue_unsigned_division_step.h"
static HudProjectionState working(void) {
    HudProjectionState w={D(0),D(1),D(2),D(3),D(4),D(5),D(6),D(7),A(0),A(1),A(2),A(3),A(4),A(5),A(6),A(7),COND_LT()}; return w;
}
static HudProjectionState consume(void *context,enum HudProjectionChild child) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc2ec9c,0xc0db40},
        {0xc1d974,0xc0d02a},
        {0xc2ec9c,0xc0d046},
        {0xc2ec90,0xc33d3a},
        {0xc31e6c,0xc33bea},
        {0xc2f5c0,0xc33c18},
        {0xc345a0,0xc33c42},
        {0xc347f2,0xc33c86},
        {0xc31d16,0xc33cc4},
        {0xc33cd2,0xc33cd0},
        {0xc332fe,0xc332d2},
        {0xc34146,0xc332d6},
        {0xc342d0,0xc332da},
        {0xc33dc8,0xc332de},
        {0xc31c60,0xc332e4},
        {0xc31d64,0xc332f2},
        {0xc33370,0xc332f6},
        {0xc33b38,0xc332fa},
        {0xc32806,0xc327ee},
        {0xc06c02,0xc32804},
    };
    (void)context; glue_complete_child(sites[child].entry,sites[child].ret); return working();
}
static void outputs(void *context,enum HudProjectionPhase phase,enum HudProjectionField field,uint32_t v,uint32_t other) {
    unsigned r=(unsigned)field; uint32_t temporary; (void)context;
    switch(phase) {
    case HPR_WORD: SET_W(D(r),v); flags_logic_w(v); break; case HPR_BYTE: SET_B(D(r),v); flags_logic_b(v); break;
    case HPR_LONG: D(r)=v; flags_logic_l(v); break; case HPR_POINTER: A(r-HPR_REGISTERS)=v; break;
    case HPR_ADD_WORD: step_add_word(&D(r),v); break; case HPR_SUB_WORD: step_subtract_word(&D(r),v); break;
    case HPR_ADD_LONG: step_add_long(&D(r),v); break; case HPR_SUB_LONG: step_subtract_long(&D(r),v); break;
    case HPR_NEG_WORD: renderer_negate(&D(r),2); break; case HPR_SWAP: step_swap(&D(r)); break;
    case HPR_EXT_WORD: SET_W(D(r),(int16_t)(int8_t)D(r)); flags_logic_w(D(r)); break;
    case HPR_EXT_LONG: D(r)=(uint32_t)(int32_t)(int16_t)D(r); flags_logic_l(D(r)); break;
    case HPR_ASL_WORD: renderer_asl_word(&D(r),v); break; case HPR_LSL_WORD: menu_lsl_word(&D(r),v); break;
    case HPR_ASR_WORD: renderer_asr_word(&D(r),v); break; case HPR_ASR_BYTE: hud_parent_asr_byte(&D(r),v); break;
    case HPR_LSR_LONG: step_lsr_long(&D(r),v); break; case HPR_ROR_WORD: hud_parent_ror_word(&D(r),v); break;
    case HPR_AND_WORD: SET_W(D(r),D(r)&v); flags_logic_w(D(r)); break; case HPR_AND_BYTE: SET_B(D(r),D(r)&v); flags_logic_b(D(r)); break;
    case HPR_OR_WORD: SET_W(D(r),D(r)|v); flags_logic_w(D(r)); break;
    case HPR_COMPARE_WORD: step_compare_word(other,v); break; case HPR_COMPARE_BYTE: step_compare_byte(other,v); break;
    case HPR_TEST_WORD: case HPR_STORE_WORD: flags_logic_w(v); break;
    case HPR_TEST_BYTE: case HPR_STORE_BYTE: flags_logic_b(v); break; case HPR_STORE_LONG: flags_logic_l(v); break;
    case HPR_BIT_TEST: FLAG_Z=v&(1u<<other); break;
    case HPR_BIT_CLEAR: FLAG_Z=v&(1u<<other); D(r)&=~(1u<<other); break;
    case HPR_MEMORY_SUB_BYTE: temporary=v; step_subtract_byte(&temporary,(uint8_t)other); break;
    case HPR_DECREMENT: SET_W(D(r),v); break;
    case HPR_PUSH_LONG: m68ki_push_32(v); flags_logic_l(v); break;
    case HPR_POP_LONG: D(r)=m68ki_pull_32(); flags_logic_l(D(r)); break;
    case HPR_SAVE_WORDS: renderer_store(A(7),v,2,7); break;
    case HPR_ASR_LONG: step_asr_long(&D(r),v); break;
    case HPR_LSR_BYTE: action_lsr_byte(&D(r),v); break;
    case HPR_DIVU: step_divide_unsigned(&D(r),(uint16_t)v); break;
    case HPR_DIVS: renderer_divide(&D(r),(int16_t)v); break;
    case HPR_AND_LONG: D(r)&=v; flags_logic_l(D(r)); break;
    case HPR_COMPARE_LONG: step_compare_long(other,v); break;
    case HPR_MEMORY_LOGIC_LONG: flags_logic_l(v); break;
    case HPR_SAVE_LONGS: renderer_store(A(7),v,4,7); break;
    case HPR_RESTORE_LONGS: renderer_load(A(7),v,4,7); break;
    case HPR_PUSH_WORD: m68ki_push_16((uint16_t)v); flags_logic_w(v); break;
    case HPR_POP_WORD: SET_W(D(r),m68ki_pull_16()); flags_logic_w(D(r)); break;
    case HPR_RESTORE_WORDS: renderer_load(A(7),v,2,7); break;
    case HPR_RAW_LONG: D(r)=v; break;
    case HPR_MULS: renderer_multiply(&D(r),(uint16_t)v); break;
    case HPR_MULU: timer_multiply_unsigned(&D(r),(uint16_t)v); break;
    case HPR_POP_POINTER: A(r-HPR_REGISTERS)=m68ki_pull_32(); break;
    }
}
static HudProjectionState restored(void *context) { (void)context; return working(); }
static const HudProjectionHooks hooks={consume,outputs,restored,NULL};
int glue_C0DAEE(void) { hud_fixed_mark(working(),&hooks); return glue_return(); }
int glue_C0CFFA(void) { hud_scaled_circle(working(),&hooks); return glue_return(); }
int glue_C33CD2(void) { hud_record_transform(working(),&hooks); return glue_return(); }
int glue_C33B38(void) { hud_weapon_cue(working(),&hooks); return glue_return(); }
int glue_C332BC(void) { hud_postflight_outer(working(),&hooks); return glue_return(); }
int glue_C32662(void) { hud_conditional_text(working(),&hooks); return glue_return(); }
