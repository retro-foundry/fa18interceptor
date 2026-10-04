#include "glue_hud_readout_parents_math.h"
#include "glue_hud_readout_parents.h"
#include "glue_child_call.h"
#include "hud_readout_parents.h"
#include "glue_unsigned_division_step.h"
static HudReadoutParentState working(void) {
    HudReadoutParentState w={D(0),D(1),D(2),D(3),D(4),D(5),D(6),D(7),A(0),A(1),A(2),A(3),A(4),A(5),COND_LT()}; return w;
}
static HudReadoutParentState consume(void *context,enum HudReadoutParentChild child) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc06c02,0xc32804},
        {0xc25a08,0xc31cfe},
        {0xc25a08,0xc31d4a},
        {0xc25a08,0xc31e50},
        {0xc25a08,0xc31efe},
        {0xc25a08,0xc31fa8},
        {0xc25a08,0xc320c2},
        {0xc25a08,0xc32158},
        {0xc25a08,0xc321b2},
        {0xc25a08,0xc3222e},
        {0xc25a08,0xc322bc},
        {0xc2f5c0,0xc31d98},
        {0xc2f5c0,0xc31dd6},
        {0xc31c20,0xc31f90},
        {0xc31c20,0xc32146},
        {0xc31c20,0xc321a0},
        {0xc3271a,0xc3200a},
        {0xc3271a,0xc3211a},
        {0xc32726,0xc31f38},
        {0xc32726,0xc31f48},
        {0xc32736,0xc32252},
        {0xc32736,0xc322e0},
        {0xc32794,0xc32982},
        {0xc32794,0xc32a14},
        {0xc32806,0xc327ee},
        {0xc32aa4,0xc31d14},
        {0xc32aa4,0xc31e5c},
        {0xc32aa6,0xc31d62},
        {0xc32aa6,0xc33fb0},
        {0xc32ab4,0xc31dc8},
        {0xc32ab4,0xc31eb2},
    };
    (void)context; glue_complete_child(sites[child].entry,sites[child].ret); return working();
}
static void outputs(void *context,enum HudReadoutParentPhase phase,enum HudReadoutParentField field,uint32_t v,uint32_t other) {
    unsigned r=(unsigned)field; uint32_t temporary; (void)context;
    switch(phase) {
    case HRP_WORD: SET_W(D(r),v); flags_logic_w(v); break; case HRP_BYTE: SET_B(D(r),v); flags_logic_b(v); break;
    case HRP_LONG: D(r)=v; flags_logic_l(v); break; case HRP_POINTER: A(r-HRP_REGISTERS)=v; break;
    case HRP_ADD_WORD: step_add_word(&D(r),v); break; case HRP_SUB_WORD: step_subtract_word(&D(r),v); break;
    case HRP_ADD_LONG: step_add_long(&D(r),v); break; case HRP_SUB_LONG: step_subtract_long(&D(r),v); break;
    case HRP_NEG_WORD: renderer_negate(&D(r),2); break; case HRP_SWAP: step_swap(&D(r)); break;
    case HRP_EXT_WORD: SET_W(D(r),(int16_t)(int8_t)D(r)); flags_logic_w(D(r)); break;
    case HRP_EXT_LONG: D(r)=(uint32_t)(int32_t)(int16_t)D(r); flags_logic_l(D(r)); break;
    case HRP_ASL_WORD: renderer_asl_word(&D(r),v); break; case HRP_LSL_WORD: menu_lsl_word(&D(r),v); break;
    case HRP_ASR_WORD: renderer_asr_word(&D(r),v); break; case HRP_ASR_BYTE: hud_parent_asr_byte(&D(r),v); break;
    case HRP_LSR_LONG: step_lsr_long(&D(r),v); break; case HRP_ROR_WORD: hud_parent_ror_word(&D(r),v); break;
    case HRP_AND_WORD: SET_W(D(r),D(r)&v); flags_logic_w(D(r)); break; case HRP_AND_BYTE: SET_B(D(r),D(r)&v); flags_logic_b(D(r)); break;
    case HRP_OR_WORD: SET_W(D(r),D(r)|v); flags_logic_w(D(r)); break;
    case HRP_COMPARE_WORD: step_compare_word(other,v); break; case HRP_COMPARE_BYTE: step_compare_byte(other,v); break;
    case HRP_TEST_WORD: case HRP_STORE_WORD: flags_logic_w(v); break;
    case HRP_TEST_BYTE: case HRP_STORE_BYTE: flags_logic_b(v); break; case HRP_STORE_LONG: flags_logic_l(v); break;
    case HRP_BIT_TEST: FLAG_Z=v&(1u<<other); break;
    case HRP_BIT_CLEAR: FLAG_Z=v&(1u<<other); D(r)&=~(1u<<other); break;
    case HRP_MEMORY_SUB_BYTE: temporary=v; step_subtract_byte(&temporary,(uint8_t)other); break;
    case HRP_DECREMENT: SET_W(D(r),v); break;
    case HRP_PUSH_LONG: m68ki_push_32(v); flags_logic_l(v); break;
    case HRP_POP_LONG: D(r)=m68ki_pull_32(); flags_logic_l(D(r)); break;
    case HRP_SAVE_WORDS: renderer_store(A(7),0x8600,2,7); break;
    case HRP_ASR_LONG: step_asr_long(&D(r),v); break;
    case HRP_LSR_BYTE: action_lsr_byte(&D(r),v); break;
    case HRP_DIVU: step_divide_unsigned(&D(r),(uint16_t)v); break;
    case HRP_DIVS: renderer_divide(&D(r),(int16_t)v); break;
    case HRP_AND_LONG: D(r)&=v; flags_logic_l(D(r)); break;
    case HRP_COMPARE_LONG: step_compare_long(other,v); break;
    case HRP_MEMORY_LOGIC_LONG: flags_logic_l(v); break;
    case HRP_SAVE_LONGS: renderer_store(A(7),v,4,7); break;
    case HRP_RESTORE_LONGS: renderer_load(A(7),v,4,7); break;
    case HRP_PUSH_WORD: m68ki_push_16((uint16_t)v); flags_logic_w(v); break;
    case HRP_POP_WORD: SET_W(D(r),m68ki_pull_16()); flags_logic_w(D(r)); break;
    case HRP_RESTORE_WORDS: renderer_load(A(7),0x0061,2,7); break;
    }
}
static HudReadoutParentState restored(void *context) { (void)context; return working(); }
static const HudReadoutParentHooks hooks={consume,outputs,restored,NULL};
int glue_C31F4C(void) { hud_readout_C31F4C(working(),&hooks); return glue_return(); }
int glue_C3201A(void) { hud_readout_C3201A(working(),&hooks); return glue_return(); }
int glue_C3212A(void) { hud_readout_C3212A(working(),&hooks); return glue_return(); }
int glue_C32178(void) { hud_readout_C32178(working(),&hooks); return glue_return(); }
int glue_C321D2(void) { hud_readout_C321D2(working(),&hooks); return glue_return(); }
int glue_C32260(void) { hud_readout_C32260(working(),&hooks); return glue_return(); }
int glue_C31EB6(void) { hud_readout_C31EB6(working(),&hooks); return glue_return(); }
int glue_C31C60(void) { hud_readout_C31C60(working(),&hooks); return glue_return(); }
int glue_C31D16(void) { hud_readout_C31D16(working(),&hooks); return glue_return(); }
int glue_C31E6C(void) { hud_readout_C31E6C(working(),&hooks); return glue_return(); }
int glue_C31D64(void) { hud_readout_C31D64(working(),&hooks); return glue_return(); }
int glue_C33F54(void) { hud_readout_C33F54(working(),&hooks); return glue_return(); }
int glue_C328A8(void) { hud_readout_C328A8(working(),&hooks); return glue_return(); }
