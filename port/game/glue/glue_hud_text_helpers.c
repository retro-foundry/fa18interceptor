#include "glue_hud_text_helpers_math.h"
#include "glue_hud_text_helpers.h"
#include "glue_child_call.h"
#include "hud_text_helpers.h"
#include "glue_unsigned_division_step.h"
static HudTextState working(void) {
    HudTextState w={D(0),D(1),D(2),D(3),D(4),D(5),D(6),D(7),A(0),A(1),A(2),A(3),A(4),A(5),COND_LT()}; return w;
}
static HudTextState consume(void *context,enum HudTextChild child) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc06c02,0xc32804},
        {0xc32806,0xc327ee},
        {0xc330fe,0xc32b72},
        {0xc330fe,0xc32b90},
        {0xc330fe,0xc32bae},
        {0xc330fe,0xc32bcc},
    };
    (void)context; glue_complete_child(sites[child].entry,sites[child].ret); return working();
}
static void outputs(void *context,enum HudTextPhase phase,enum HudTextField field,uint32_t v,uint32_t other) {
    unsigned r=(unsigned)field; uint32_t temporary; (void)context;
    switch(phase) {
    case HTH_WORD: SET_W(D(r),v); flags_logic_w(v); break; case HTH_BYTE: SET_B(D(r),v); flags_logic_b(v); break;
    case HTH_LONG: D(r)=v; flags_logic_l(v); break; case HTH_POINTER: A(r-HTH_REGISTERS)=v; break;
    case HTH_ADD_WORD: step_add_word(&D(r),v); break; case HTH_SUB_WORD: step_subtract_word(&D(r),v); break;
    case HTH_ADD_LONG: step_add_long(&D(r),v); break; case HTH_SUB_LONG: step_subtract_long(&D(r),v); break;
    case HTH_NEG_WORD: renderer_negate(&D(r),2); break; case HTH_SWAP: step_swap(&D(r)); break;
    case HTH_EXT_WORD: SET_W(D(r),(int16_t)(int8_t)D(r)); flags_logic_w(D(r)); break;
    case HTH_EXT_LONG: D(r)=(uint32_t)(int32_t)(int16_t)D(r); flags_logic_l(D(r)); break;
    case HTH_ASL_WORD: renderer_asl_word(&D(r),v); break; case HTH_LSL_WORD: menu_lsl_word(&D(r),v); break;
    case HTH_ASR_WORD: renderer_asr_word(&D(r),v); break; case HTH_ASR_BYTE: hud_parent_asr_byte(&D(r),v); break;
    case HTH_LSR_LONG: step_lsr_long(&D(r),v); break; case HTH_ROR_WORD: hud_parent_ror_word(&D(r),v); break;
    case HTH_AND_WORD: SET_W(D(r),D(r)&v); flags_logic_w(D(r)); break; case HTH_AND_BYTE: SET_B(D(r),D(r)&v); flags_logic_b(D(r)); break;
    case HTH_OR_WORD: SET_W(D(r),D(r)|v); flags_logic_w(D(r)); break;
    case HTH_COMPARE_WORD: step_compare_word(other,v); break; case HTH_COMPARE_BYTE: step_compare_byte(other,v); break;
    case HTH_TEST_WORD: case HTH_STORE_WORD: flags_logic_w(v); break;
    case HTH_TEST_BYTE: case HTH_STORE_BYTE: flags_logic_b(v); break; case HTH_STORE_LONG: flags_logic_l(v); break;
    case HTH_BIT_TEST: FLAG_Z=v&(1u<<other); break;
    case HTH_BIT_CLEAR: FLAG_Z=v&(1u<<other); D(r)&=~(1u<<other); break;
    case HTH_MEMORY_SUB_BYTE: temporary=v; step_subtract_byte(&temporary,(uint8_t)other); break;
    case HTH_DECREMENT: SET_W(D(r),v); break;
    case HTH_PUSH_LONG: m68ki_push_32(v); flags_logic_l(v); break;
    case HTH_POP_LONG: D(r)=m68ki_pull_32(); flags_logic_l(D(r)); break;
    case HTH_SAVE_WORDS: renderer_store(A(7),0x8600,2,7); break;
    case HTH_ASR_LONG: step_asr_long(&D(r),v); break;
    case HTH_LSR_BYTE: action_lsr_byte(&D(r),v); break;
    case HTH_DIVU: step_divide_unsigned(&D(r),(uint16_t)v); break;
    case HTH_DIVS: renderer_divide(&D(r),(int16_t)v); break;
    case HTH_AND_LONG: D(r)&=v; flags_logic_l(D(r)); break;
    case HTH_COMPARE_LONG: step_compare_long(other,v); break;
    case HTH_MEMORY_LOGIC_LONG: flags_logic_l(v); break;
    case HTH_SAVE_LONGS: renderer_store(A(7),v,4,7); break;
    case HTH_RESTORE_LONGS: renderer_load(A(7),v,4,7); break;
    case HTH_PUSH_WORD: m68ki_push_16((uint16_t)v); flags_logic_w(v); break;
    case HTH_POP_WORD: SET_W(D(r),m68ki_pull_16()); flags_logic_w(D(r)); break;
    case HTH_RESTORE_WORDS: renderer_load(A(7),0x0061,2,7); break;
    }
}
static HudTextState restored(void *context) { (void)context; return working(); }
static const HudTextHooks hooks={consume,outputs,restored,NULL};
int glue_C31C20(void) { hud_text_cache(working(),&hooks); return glue_return(); }
int glue_C3271A(void) { hud_text_small_hex(working(),&hooks); return glue_return(); }
int glue_C32726(void) { hud_text_small_fixed(working(),&hooks); return glue_return(); }
int glue_C32736(void) { hud_text_small_inverse(working(),&hooks); return glue_return(); }
int glue_C32794(void) { hud_text_small_line(working(),&hooks); return glue_return(); }
int glue_C32AA4(void) { hud_text_view_digits(working(),&hooks,1); return glue_return(); }
int glue_C32AA6(void) { hud_text_view_digits(working(),&hooks,0); return glue_return(); }
int glue_C32AB4(void) { hud_text_view_line(working(),&hooks); return glue_return(); }
