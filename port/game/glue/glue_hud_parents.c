#include "glue_hud_parents_math.h"
#include "glue_hud_parents.h"
#include "glue_child_call.h"
#include "hud_parents.h"
static HudParentState working(void) {
    HudParentState w={D(0),D(1),D(2),D(3),D(4),D(5),D(6),D(7),A(0),A(1),A(2),A(3),A(4),A(5),COND_LT()}; return w;
}
static HudParentState consume(void *context,enum HudParentChild child) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc310e2,0xc307ba},{0xc53f44,0xc307da},{0xc30904,0xc307fc},{0xc308f4,0xc30800},{0xc308f4,0xc30804},{0xc308f4,0xc30808},
        {0xc53f44,0xc308a0},{0xc308e2,0xc308be},{0xc308d8,0xc308c6},{0xc308d8,0xc308ce},{0xc308d8,0xc308d6},
        {0xc30eaa,0xc309e0},{0xc2fa78,0xc30bc4},{0xc310e2,0xc30bf2},{0xc30cc4,0xc30c20},{0xc310e2,0xc30c4e},{0xc30cc4,0xc30c6e},{0xc310e2,0xc30c9e},{0xc53f44,0xc30cf4},
        {0xc310e2,0xc30d54},{0xc30cc4,0xc30d7e},{0xc310e2,0xc30dda},{0xc53f44,0xc30e06},{0xc2fa78,0xc30ea8},
        {0xc310aa,0xc30f7c},{0xc310e2,0xc30fec},{0xc53f44,0xc3103a},{0xc2fa78,0xc310a8},
        {0xc2f63a,0xc3115c},{0xc2f64e,0xc31166},{0xc2f64e,0xc31188},{0xc2f64e,0xc31190},{0xc2f64e,0xc311b4},{0xc2f64e,0xc311bc},{0xc2f64e,0xc311de},{0xc2f64e,0xc311e6},{0xc2f64e,0xc3121a},{0xc2f64e,0xc31222},
        {0xc25a08,0xc31aa8},{0xc2f5c0,0xc31b46},{0xc25a08,0xc31b56},{0xc32806,0xc327ee},{0xc06c02,0xc32804}
    };
    (void)context; glue_complete_child(sites[child].entry,sites[child].ret); return working();
}
static void outputs(void *context,enum HudParentPhase phase,enum HudParentField field,uint32_t v,uint32_t other) {
    unsigned r=(unsigned)field; uint32_t temporary; (void)context;
    switch(phase) {
    case HP_WORD: SET_W(D(r),v); flags_logic_w(v); break; case HP_BYTE: SET_B(D(r),v); flags_logic_b(v); break;
    case HP_LONG: D(r)=v; flags_logic_l(v); break; case HP_POINTER: A(r-HP_REGISTERS)=v; break;
    case HP_ADD_WORD: step_add_word(&D(r),v); break; case HP_SUB_WORD: step_subtract_word(&D(r),v); break;
    case HP_ADD_LONG: step_add_long(&D(r),v); break; case HP_SUB_LONG: step_subtract_long(&D(r),v); break;
    case HP_NEG_WORD: renderer_negate(&D(r),2); break; case HP_SWAP: step_swap(&D(r)); break;
    case HP_EXT_WORD: SET_W(D(r),(int16_t)(int8_t)D(r)); flags_logic_w(D(r)); break;
    case HP_EXT_LONG: D(r)=(uint32_t)(int32_t)(int16_t)D(r); flags_logic_l(D(r)); break;
    case HP_ASL_WORD: renderer_asl_word(&D(r),v); break; case HP_LSL_WORD: menu_lsl_word(&D(r),v); break;
    case HP_ASR_WORD: renderer_asr_word(&D(r),v); break; case HP_ASR_BYTE: hud_parent_asr_byte(&D(r),v); break;
    case HP_LSR_LONG: step_lsr_long(&D(r),v); break; case HP_ROR_WORD: hud_parent_ror_word(&D(r),v); break;
    case HP_AND_WORD: SET_W(D(r),D(r)&v); flags_logic_w(D(r)); break; case HP_AND_BYTE: SET_B(D(r),D(r)&v); flags_logic_b(D(r)); break;
    case HP_OR_WORD: SET_W(D(r),D(r)|v); flags_logic_w(D(r)); break;
    case HP_COMPARE_WORD: step_compare_word(other,v); break; case HP_COMPARE_BYTE: step_compare_byte(other,v); break;
    case HP_TEST_WORD: case HP_STORE_WORD: flags_logic_w(v); break;
    case HP_TEST_BYTE: case HP_STORE_BYTE: flags_logic_b(v); break; case HP_STORE_LONG: flags_logic_l(v); break;
    case HP_BIT_TEST: FLAG_Z=v&(1u<<other); break;
    case HP_BIT_CLEAR: FLAG_Z=v&(1u<<other); D(r)&=~(1u<<other); break;
    case HP_MEMORY_SUB_BYTE: temporary=v; step_subtract_byte(&temporary,(uint8_t)other); break;
    case HP_DECREMENT: SET_W(D(r),v); break;
    case HP_PUSH_LONG: m68ki_push_32(v); flags_logic_l(v); break;
    case HP_POP_LONG: D(r)=m68ki_pull_32(); flags_logic_l(D(r)); break;
    case HP_SAVE_WORDS: renderer_store(A(7),0x8600,2,7); break;
    case HP_RESTORE_WORDS: renderer_load(A(7),0x0061,2,7); break;
    }
}
static HudParentState restored(void *context) { (void)context; return working(); }
static const HudParentHooks hooks={consume,outputs,restored,NULL};
int glue_C30764(void) { draw_counter_stream_display(working(),&hooks); return glue_return(); }
int glue_C309B6(void) { draw_table_stream_display(working(),&hooks); return glue_return(); }
int glue_C30B5C(void) { draw_status_stream_display(working(),&hooks); return glue_return(); }
int glue_C30D34(void) { draw_record_stream_display(working(),&hooks); return glue_return(); }
int glue_C30F78(void) { draw_cached_stream_display(working(),&hooks); return glue_return(); }
int glue_C3112A(void) { draw_bit_selected_points(working(),&hooks); return glue_return(); }
int glue_C31A64(void) { draw_record_class_digits(working(),&hooks); return glue_return(); }
int glue_C31ACC(void) { draw_record_scale_digits(working(),&hooks); return glue_return(); }
