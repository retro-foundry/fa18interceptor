#include "glue_hud_history_stream_math.h"
#include "glue_hud_history_stream.h"
#include "glue_child_call.h"
#include "hud_history_stream.h"
#include "glue_unsigned_division_step.h"
static HudHistoryStreamState working(void) {
    HudHistoryStreamState w={D(0),D(1),D(2),D(3),D(4),D(5),D(6),D(7),A(0),A(1),A(2),A(3),A(4),A(5),A(6),A(7),COND_LT()}; return w;
}
static HudHistoryStreamState consume(void *context,enum HudHistoryStreamChild child) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc1d974,0xc0d1e4},
        {0xc2ce82,0xc0d1fa},
        {0xc2ec9c,0xc0d25c},
        {0xc2ec9c,0xc0d296},
        {0xc2ec9c,0xc0d2d0},
        {0xc2ec9c,0xc0d2f0},
        {0xc25a08,0xc333d6},
        {0xc32aa4,0xc333ee},
        {0xc32ab4,0xc33414},
        {0xc25a08,0xc3342e},
        {0xc25a08,0xc3346c},
        {0xc259c2,0xc33490},
        {0xc2f5d4,0xc334d0},
        {0xc33f8a,0xc334ec},
        {0xc2f5c0,0xc33524},
        {0xc33f8a,0xc33542},
        {0xc2f5c0,0xc3357e},
        {0xc33f8a,0xc3359c},
        {0xc33fb4,0xc335ae},
        {0xc33ad6,0xc335b6},
        {0xc25a08,0xc33600},
        {0xc32aa4,0xc33618},
        {0xc32ab4,0xc3363e},
        {0xc25a08,0xc3365a},
        {0xc25a08,0xc33698},
        {0xc259c2,0xc336bc},
        {0xc33f70,0xc336f8},
        {0xc33f70,0xc33736},
        {0xc33f70,0xc33776},
        {0xc33fb4,0xc33786},
        {0xc33b06,0xc3378e},
        {0xc2f60a,0xc337b4},
        {0xc2f5c0,0xc337c2},
        {0xc2f5d4,0xc337ca},
        {0xc2f60a,0xc337dc},
        {0xc2f5c0,0xc337ea},
        {0xc2f5d4,0xc337f4},
        {0xc2f5f4,0xc337fc},
        {0xc25a08,0xc3381e},
        {0xc25a08,0xc3384a},
        {0xc259c2,0xc33878},
        {0xc33f54,0xc338a8},
        {0xc33f54,0xc338fc},
        {0xc33f54,0xc33948},
        {0xc34066,0xc3395c},
        {0xc2ec94,0xc1fe40},
        {0xc2eca4,0xc1fe62},
        {0xc0cffa,0xc0cfb0},
    };
    (void)context; glue_complete_child(sites[child].entry,sites[child].ret); return working();
}
static void outputs(void *context,enum HudHistoryStreamPhase phase,enum HudHistoryStreamField field,uint32_t v,uint32_t other) {
    unsigned r=(unsigned)field; uint32_t temporary; (void)context;
    switch(phase) {
    case HHS_WORD: SET_W(D(r),v); flags_logic_w(v); break; case HHS_BYTE: SET_B(D(r),v); flags_logic_b(v); break;
    case HHS_LONG: D(r)=v; flags_logic_l(v); break; case HHS_POINTER: A(r-HHS_REGISTERS)=v; break;
    case HHS_ADD_WORD: step_add_word(&D(r),v); break; case HHS_SUB_WORD: step_subtract_word(&D(r),v); break;
    case HHS_ADD_LONG: step_add_long(&D(r),v); break; case HHS_SUB_LONG: step_subtract_long(&D(r),v); break;
    case HHS_NEG_WORD: renderer_negate(&D(r),2); break; case HHS_SWAP: step_swap(&D(r)); break;
    case HHS_EXT_WORD: SET_W(D(r),(int16_t)(int8_t)D(r)); flags_logic_w(D(r)); break;
    case HHS_EXT_LONG: D(r)=(uint32_t)(int32_t)(int16_t)D(r); flags_logic_l(D(r)); break;
    case HHS_ASL_WORD: renderer_asl_word(&D(r),v); break; case HHS_LSL_WORD: menu_lsl_word(&D(r),v); break;
    case HHS_ASR_WORD: renderer_asr_word(&D(r),v); break; case HHS_ASR_BYTE: hud_parent_asr_byte(&D(r),v); break;
    case HHS_LSR_LONG: step_lsr_long(&D(r),v); break; case HHS_ROR_WORD: hud_parent_ror_word(&D(r),v); break;
    case HHS_AND_WORD: SET_W(D(r),D(r)&v); flags_logic_w(D(r)); break; case HHS_AND_BYTE: SET_B(D(r),D(r)&v); flags_logic_b(D(r)); break;
    case HHS_OR_WORD: SET_W(D(r),D(r)|v); flags_logic_w(D(r)); break;
    case HHS_COMPARE_WORD: step_compare_word(other,v); break; case HHS_COMPARE_BYTE: step_compare_byte(other,v); break;
    case HHS_TEST_WORD: case HHS_STORE_WORD: flags_logic_w(v); break;
    case HHS_TEST_BYTE: case HHS_STORE_BYTE: flags_logic_b(v); break; case HHS_STORE_LONG: flags_logic_l(v); break;
    case HHS_BIT_TEST: FLAG_Z=v&(1u<<other); break;
    case HHS_BIT_CLEAR: FLAG_Z=v&(1u<<other); D(r)&=~(1u<<other); break;
    case HHS_MEMORY_SUB_BYTE: temporary=v; step_subtract_byte(&temporary,(uint8_t)other); break;
    case HHS_DECREMENT: SET_W(D(r),v); break;
    case HHS_PUSH_LONG: m68ki_push_32(v); flags_logic_l(v); break;
    case HHS_POP_LONG: D(r)=m68ki_pull_32(); flags_logic_l(D(r)); break;
    case HHS_SAVE_WORDS: renderer_store(A(7),v,2,7); break;
    case HHS_ASR_LONG: step_asr_long(&D(r),v); break;
    case HHS_LSR_BYTE: action_lsr_byte(&D(r),v); break;
    case HHS_DIVU: step_divide_unsigned(&D(r),(uint16_t)v); break;
    case HHS_DIVS: renderer_divide(&D(r),(int16_t)v); break;
    case HHS_AND_LONG: D(r)&=v; flags_logic_l(D(r)); break;
    case HHS_COMPARE_LONG: step_compare_long(other,v); break;
    case HHS_MEMORY_LOGIC_LONG: flags_logic_l(v); break;
    case HHS_SAVE_LONGS: renderer_store(A(7),v,4,7); break;
    case HHS_RESTORE_LONGS: renderer_load(A(7),v,4,7); break;
    case HHS_PUSH_WORD: m68ki_push_16((uint16_t)v); flags_logic_w(v); break;
    case HHS_POP_WORD: SET_W(D(r),m68ki_pull_16()); flags_logic_w(D(r)); break;
    case HHS_RESTORE_WORDS: renderer_load(A(7),v,2,7); break;
    case HHS_RAW_LONG: D(r)=v; break;
    case HHS_MULS: renderer_multiply(&D(r),(uint16_t)v); break;
    case HHS_MULU: timer_multiply_unsigned(&D(r),(uint16_t)v); break;
    case HHS_POP_POINTER: A(r-HHS_REGISTERS)=m68ki_pull_32(); break;
    case HHS_LOAD_WORDS_AT: renderer_load(v,other,2,-1); break;
    case HHS_STORE_WORDS_AT: renderer_store(v,other,2,-1); break;
    case HHS_LOAD_LONGS_AT: renderer_load(v,other,4,-1); break;
    case HHS_LINK_FRAME: m68ki_push_32(A(6)); A(6)=A(7); A(7)-=v; break;
    case HHS_UNLINK_FRAME: A(7)=A(6); A(6)=m68ki_pull_32(); break;
    case HHS_NEG_LONG: renderer_negate(&D(r),4); break;
    case HHS_ADD_BYTE: renderer_add_byte(&D(r),v); break;
    case HHS_SUB_BYTE: step_subtract_byte(&D(r),v); break;
    case HHS_MEMORY_ADD_BYTE: temporary=v; renderer_add_byte(&temporary,other); break;
    case HHS_MEMORY_ADD_LONG: temporary=v; step_add_long(&temporary,other); break;
    case HHS_TEST_LONG: flags_logic_l(v); break;
    case HHS_POP_MEMORY_LONG: temporary=m68ki_pull_32(); wr_u32(v,temporary); flags_logic_l(temporary); break;
    case HHS_DECIMAL_ADD: menu_decimal_flags(v,other); break;
    case HHS_DECIMAL_SUB: history_decimal_subtract_flags(v,other); break;
    }
}
static HudHistoryStreamState restored(void *context) { (void)context; return working(); }
static const HudHistoryStreamHooks hooks={consume,outputs,restored,NULL};
int glue_C0D04C(void) { hud_history_projection(working(),&hooks); return glue_return(); }
int glue_C33370(void) { hud_postflight_display(working(),&hooks); return glue_return(); }
int glue_C1FE24(void) { hud_stream_point(working(),&hooks,0); return glue_return(); }
int glue_C1FE46(void) { hud_stream_point(working(),&hooks,1); return glue_return(); }
int glue_C0CF98(void) { hud_stream_circle(working(),&hooks); return glue_return(); }
