#include "glue_hud_render_parents_math.h"
#include "glue_hud_render_parents.h"
#include "glue_child_call.h"
#include "hud_render_parents.h"
#include "hardware.h"
#include "glue_unsigned_division_step.h"
static HudRenderState working(void) {
    HudRenderState w={D(0),D(1),D(2),D(3),D(4),D(5),D(6),D(7),A(0),A(1),A(2),A(3),A(4),A(5),A(6),A(7),COND_LT(),COND_EQ()}; return w;
}
static HudRenderState consume(void *context,enum HudRenderChild child) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc2469e,0xc099a2},
        {0xc2469e,0xc09a60},
        {0xc2469e,0xc099ee},
        {0xc2f60a,0xc33330},
        {0xc2f60a,0xc33340},
        {0xc2f60a,0xc33350},
        {0xc2f60a,0xc33362},
        {0xc2f60a,0xc3336e},
        {0xc2f5c0,0xc34166},
        {0xc2f5d4,0xc34170},
        {0xc2f5d4,0xc34178},
        {0xc2f5f4,0xc34180},
        {0xc2fa7e,0xc341b2},
        {0xc2fa7e,0xc341ec},
        {0xc2fa7e,0xc3421e},
        {0xc2fa7e,0xc342bc},
        {0xc2f5c0,0xc3407a},
        {0xc2f5d4,0xc34084},
        {0xc2f5f4,0xc340c0},
        {0xc2f5d4,0xc340c8},
        {0xc2f5f4,0xc340d4},
        {0xc2f5f4,0xc34124},
        {0xc2f5d4,0xc3412c},
        {0xc2f5f4,0xc34138},
        {0xc2fa7e,0xc3451a},
        {0xc2f66e,0xc348ac},
        {0xc33da4,0xc33edc},
        {0xc348b2,0xc33f44},
        {0xc31e6c,0xc33f52},
        {0xc3267a,0xc3249a},
        {0xc3267a,0xc324cc},
        {0xc3267a,0xc3250c},
        {0xc32662,0xc325ea},
        {0xc32662,0xc325fc},
        {0xc32662,0xc3260e},
        {0xc32662,0xc3265c},
        {0xc32806,0xc327ee},
        {0xc06c02,0xc32804},
        {0xc310e2,0xc3007e},
        {0xc3019c,0xc30108},
        {0xc2fa78,0xc3013a},
        {0xc2f5c0,0xc30146},
        {0xc2f5f4,0xc3014e},
        {0xc2f5c0,0xc30162},
        {0xc2f5d4,0xc3016a},
        {0xc2f5d4,0xc30172},
        {0xc2f5d4,0xc3017a},
        {0xc2f5d4,0xc30184},
        {0xc2f5d4,0xc3018a},
        {0xc2f5d4,0xc30190},
        {0xc2f5d4,0xc30198},
        {0xc2f5f4,0xc3460c},
        {0xc2f5f4,0xc34636},
        {0xc2f5f4,0xc34660},
        {0xc2f5f4,0xc3468a},
        {0xc2f5f4,0xc346fe},
        {0xc2f5f4,0xc3474a},
        {0xc2f5f4,0xc34798},
        {0xc2f5f4,0xc347e6},
        {0xc2f5f4,0xc34942},
    };
    (void)context; glue_complete_child(sites[child].entry,sites[child].ret); return working();
}
static void outputs(void *context,enum HudRenderPhase phase,enum HudRenderField field,uint32_t v,uint32_t other) {
    unsigned r=(unsigned)field; uint32_t temporary; (void)context;
    switch(phase) {
    case HRR_WORD: SET_W(D(r),v); flags_logic_w(v); break; case HRR_BYTE: SET_B(D(r),v); flags_logic_b(v); break;
    case HRR_LONG: D(r)=v; flags_logic_l(v); break; case HRR_POINTER: A(r-HRR_REGISTERS)=v; break;
    case HRR_ADD_WORD: step_add_word(&D(r),v); break; case HRR_SUB_WORD: step_subtract_word(&D(r),v); break;
    case HRR_ADD_LONG: step_add_long(&D(r),v); break; case HRR_SUB_LONG: step_subtract_long(&D(r),v); break;
    case HRR_NEG_WORD: renderer_negate(&D(r),2); break; case HRR_SWAP: step_swap(&D(r)); break;
    case HRR_EXT_WORD: SET_W(D(r),(int16_t)(int8_t)D(r)); flags_logic_w(D(r)); break;
    case HRR_EXT_LONG: D(r)=(uint32_t)(int32_t)(int16_t)D(r); flags_logic_l(D(r)); break;
    case HRR_ASL_WORD: renderer_asl_word(&D(r),v); break; case HRR_LSL_WORD: menu_lsl_word(&D(r),v); break;
    case HRR_ASR_WORD: renderer_asr_word(&D(r),v); break; case HRR_ASR_BYTE: hud_parent_asr_byte(&D(r),v); break;
    case HRR_LSR_LONG: step_lsr_long(&D(r),v); break; case HRR_ROR_WORD: hud_parent_ror_word(&D(r),v); break;
    case HRR_AND_WORD: SET_W(D(r),D(r)&v); flags_logic_w(D(r)); break; case HRR_AND_BYTE: SET_B(D(r),D(r)&v); flags_logic_b(D(r)); break;
    case HRR_OR_WORD: SET_W(D(r),D(r)|v); flags_logic_w(D(r)); break;
    case HRR_COMPARE_WORD: step_compare_word(other,v); break; case HRR_COMPARE_BYTE: step_compare_byte(other,v); break;
    case HRR_TEST_WORD: case HRR_STORE_WORD: flags_logic_w(v); break;
    case HRR_TEST_BYTE: case HRR_STORE_BYTE: flags_logic_b(v); break; case HRR_STORE_LONG: flags_logic_l(v); break;
    case HRR_BIT_TEST: FLAG_Z=v&(1u<<other); break;
    case HRR_BIT_CLEAR: FLAG_Z=v&(1u<<other); D(r)&=~(1u<<other); break;
    case HRR_MEMORY_SUB_BYTE: temporary=v; step_subtract_byte(&temporary,(uint8_t)other); break;
    case HRR_DECREMENT: SET_W(D(r),v); break;
    case HRR_PUSH_LONG: m68ki_push_32(v); flags_logic_l(v); break;
    case HRR_POP_LONG: D(r)=m68ki_pull_32(); flags_logic_l(D(r)); break;
    case HRR_SAVE_WORDS: renderer_store(A(7),v,2,7); break;
    case HRR_ASR_LONG: step_asr_long(&D(r),v); break;
    case HRR_LSR_BYTE: action_lsr_byte(&D(r),v); break;
    case HRR_DIVU: step_divide_unsigned(&D(r),(uint16_t)v); break;
    case HRR_DIVS: renderer_divide(&D(r),(int16_t)v); break;
    case HRR_AND_LONG: D(r)&=v; flags_logic_l(D(r)); break;
    case HRR_COMPARE_LONG: step_compare_long(other,v); break;
    case HRR_MEMORY_LOGIC_LONG: flags_logic_l(v); break;
    case HRR_SAVE_LONGS: renderer_store(A(7),v,4,7); break;
    case HRR_RESTORE_LONGS: renderer_load(A(7),v,4,7); break;
    case HRR_PUSH_WORD: m68ki_push_16((uint16_t)v); flags_logic_w(v); break;
    case HRR_POP_WORD: SET_W(D(r),m68ki_pull_16()); flags_logic_w(D(r)); break;
    case HRR_RESTORE_WORDS: renderer_load(A(7),v,2,7); break;
    case HRR_RAW_LONG: D(r)=v; break;
    case HRR_MULS: renderer_multiply(&D(r),(uint16_t)v); break;
    case HRR_MULU: timer_multiply_unsigned(&D(r),(uint16_t)v); break;
    case HRR_POP_POINTER: A(r-HRR_REGISTERS)=m68ki_pull_32(); break;
    case HRR_LOAD_WORDS_AT: renderer_load(v,other,2,-1); break;
    case HRR_STORE_WORDS_AT: renderer_store(v,other,2,-1); break;
    case HRR_LOAD_LONGS_AT: renderer_load(v,other,4,-1); break;
    case HRR_LINK_FRAME: m68ki_push_32(A(6)); A(6)=A(7); A(7)-=v; break;
    case HRR_UNLINK_FRAME: A(7)=A(6); A(6)=m68ki_pull_32(); break;
    case HRR_NEG_LONG: renderer_negate(&D(r),4); break;
    case HRR_ADD_BYTE: renderer_add_byte(&D(r),v); break;
    case HRR_SUB_BYTE: step_subtract_byte(&D(r),v); break;
    case HRR_MEMORY_ADD_BYTE: temporary=v; renderer_add_byte(&temporary,other); break;
    case HRR_MEMORY_ADD_LONG: temporary=v; step_add_long(&temporary,other); break;
    case HRR_TEST_LONG: flags_logic_l(v); break;
    case HRR_POP_MEMORY_LONG: temporary=m68ki_pull_32(); wr_u32(v,temporary); flags_logic_l(temporary); break;
    case HRR_ASL_LONG: step_asl_long(&D(r),v); break;
    case HRR_LSR_WORD: message_lsr_word(&D(r),v); break;
    case HRR_STORE_LONGS_AT: renderer_store(v,other,4,-1); break;
    case HRR_OR_BYTE: SET_B(D(r),D(r)|v);flags_logic_b(D(r));break;
    case HRR_NEG_BYTE: renderer_negate(&D(r),1);break;
    case HRR_BIT_SET: FLAG_Z=v&(1u<<other);D(r)|=1u<<other;break;
    case HRR_ROL_BYTE: { unsigned n=v&63u; uint8_t b=(uint8_t)D(r); unsigned i; for(i=0;i<n;++i)b=(uint8_t)((b<<1)|(b>>7));SET_B(D(r),b);flags_logic_b(b);if(n)FLAG_C=(b&1)?CFLAG_SET:0;break; }
    case HRR_WAIT_BLITTER:
        REG_PPC=v;
        do { temporary=rd_u16(other+2); FLAG_Z=temporary&0x4000u; if(FLAG_Z)wait_blitter(); }while(FLAG_Z);
        break;
    case HRR_EXCHANGE_DATA: temporary=D(r);D(r)=D(v);D(v)=temporary;break;
    case HRR_MEMORY_SUB_WORD: temporary=v;step_subtract_word(&temporary,other);break;
    case HRR_MEMORY_ADD_WORD: temporary=v;step_add_word(&temporary,other);break;
    }
}
static HudRenderState restored(void *context) { (void)context; return working(); }
static const HudRenderHooks hooks={consume,outputs,restored,NULL};
void glue_hud_render_frame_right(void) { (void)hud_render_frame_right(working(),&hooks);REG_PC=0xc342aa; }
void glue_hud_render_page_delay(void) { REG_PC=hud_render_page_delay(&hooks)?0xc325a6:0xc32678; }
int glue_C098C6(void) { hud_render_ground_points(working(),&hooks);return glue_return(); }
int glue_C09952(void) { hud_render_indexed_face(working(),&hooks);return glue_return(); }
int glue_C099F6(void) { hud_render_outlined_face(working(),&hooks);return glue_return(); }
int glue_C099AA(void) { hud_render_coloured_face(working(),&hooks);return glue_return(); }
int glue_C332FE(void) { hud_render_view_marker(working(),&hooks);return glue_return(); }
int glue_C34146(void) { hud_render_hud_marks(working(),&hooks);return glue_return(); }
int glue_C34066(void) { hud_render_tick_row(working(),&hooks);return glue_return(); }
int glue_C342D0(void) { hud_render_target_box(working(),&hooks);return glue_return(); }
int glue_C347F2(void) { hud_render_ring_point(working(),&hooks);return glue_return(); }
int glue_C33DC8(void) { hud_render_seeker_state(working(),&hooks);return glue_return(); }
int glue_C322EE(void) { hud_render_message_line(working(),&hooks);return glue_return(); }
int glue_C3003A(void) { hud_render_panel_mark(working(),&hooks);return glue_return(); }
int glue_C304FA(void) { hud_render_blit_lane(working(),&hooks);return glue_return(); }
int glue_C345A0(void) { hud_render_ring(working(),&hooks);return glue_return(); }
int glue_C348B2(void) { hud_render_symbol(working(),&hooks);return glue_return(); }
