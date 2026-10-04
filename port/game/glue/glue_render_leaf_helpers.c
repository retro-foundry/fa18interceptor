#include "glue_render_leaf_helpers_math.h"
#include "glue_render_leaf_helpers.h"
#include "glue_child_call.h"
#include "render_leaf_helpers.h"
#include "hardware.h"
#include "glue_unsigned_division_step.h"
static RenderLeafState working(void) {
    RenderLeafState w={D(0),D(1),D(2),D(3),D(4),D(5),D(6),D(7),A(0),A(1),A(2),A(3),A(4),A(5),A(6),A(7),COND_LT(),COND_EQ()}; return w;
}
static RenderLeafState consume(void *context,enum RenderLeafChild child) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc2f688,0xc2f5ee},
        {0xc2f5f4,0xc2f618},
        {0xc2f5f4,0xc2f620},
        {0xc2f66e,0xc3028e},
        {0xc2fa7e,0xc302ba},
        {0xc2f5f4,0xc302d2},
        {0xc2f60a,0xc302da},
        {0xc305aa,0xc30328},
        {0xc305aa,0xc30340},
        {0xc0d752,0xc2fef2},
        {0xc301f6,0xc2fef8},
        {0xc30466,0xc2ff0c},
        {0xc0d74a,0xc2ff20},
    };
    (void)context; glue_complete_child(sites[child].entry,sites[child].ret); return working();
}
static void outputs(void *context,enum RenderLeafPhase phase,enum RenderLeafField field,uint32_t v,uint32_t other) {
    unsigned r=(unsigned)field; uint32_t temporary; (void)context;
    switch(phase) {
    case RL_WORD: SET_W(D(r),v); flags_logic_w(v); break; case RL_BYTE: SET_B(D(r),v); flags_logic_b(v); break;
    case RL_LONG: D(r)=v; flags_logic_l(v); break; case RL_POINTER: A(r-RL_REGISTERS)=v; break;
    case RL_ADD_WORD: step_add_word(&D(r),v); break; case RL_SUB_WORD: step_subtract_word(&D(r),v); break;
    case RL_ADD_LONG: step_add_long(&D(r),v); break; case RL_SUB_LONG: step_subtract_long(&D(r),v); break;
    case RL_NEG_WORD: renderer_negate(&D(r),2); break; case RL_SWAP: step_swap(&D(r)); break;
    case RL_EXT_WORD: SET_W(D(r),(int16_t)(int8_t)D(r)); flags_logic_w(D(r)); break;
    case RL_EXT_LONG: D(r)=(uint32_t)(int32_t)(int16_t)D(r); flags_logic_l(D(r)); break;
    case RL_ASL_WORD: renderer_asl_word(&D(r),v); break; case RL_LSL_WORD: menu_lsl_word(&D(r),v); break;
    case RL_ASR_WORD: renderer_asr_word(&D(r),v); break; case RL_ASR_BYTE: hud_parent_asr_byte(&D(r),v); break;
    case RL_LSR_LONG: step_lsr_long(&D(r),v); break; case RL_ROR_WORD: hud_parent_ror_word(&D(r),v); break;
    case RL_AND_WORD: SET_W(D(r),D(r)&v); flags_logic_w(D(r)); break; case RL_AND_BYTE: SET_B(D(r),D(r)&v); flags_logic_b(D(r)); break;
    case RL_OR_WORD: SET_W(D(r),D(r)|v); flags_logic_w(D(r)); break;
    case RL_COMPARE_WORD: step_compare_word(other,v); break; case RL_COMPARE_BYTE: step_compare_byte(other,v); break;
    case RL_TEST_WORD: case RL_STORE_WORD: flags_logic_w(v); break;
    case RL_TEST_BYTE: case RL_STORE_BYTE: flags_logic_b(v); break; case RL_STORE_LONG: flags_logic_l(v); break;
    case RL_BIT_TEST: FLAG_Z=v&(1u<<other); break;
    case RL_BIT_CLEAR: FLAG_Z=v&(1u<<other); D(r)&=~(1u<<other); break;
    case RL_MEMORY_SUB_BYTE: temporary=v; step_subtract_byte(&temporary,(uint8_t)other); break;
    case RL_DECREMENT: SET_W(D(r),v); break;
    case RL_PUSH_LONG: m68ki_push_32(v); flags_logic_l(v); break;
    case RL_POP_LONG: D(r)=m68ki_pull_32(); flags_logic_l(D(r)); break;
    case RL_SAVE_WORDS: renderer_store(A(7),v,2,7); break;
    case RL_ASR_LONG: step_asr_long(&D(r),v); break;
    case RL_LSR_BYTE: action_lsr_byte(&D(r),v); break;
    case RL_DIVU: step_divide_unsigned(&D(r),(uint16_t)v); break;
    case RL_DIVS: renderer_divide(&D(r),(int16_t)v); break;
    case RL_AND_LONG: D(r)&=v; flags_logic_l(D(r)); break;
    case RL_COMPARE_LONG: step_compare_long(other,v); break;
    case RL_MEMORY_LOGIC_LONG: flags_logic_l(v); break;
    case RL_SAVE_LONGS: renderer_store(A(7),v,4,7); break;
    case RL_RESTORE_LONGS: renderer_load(A(7),v,4,7); break;
    case RL_PUSH_WORD: m68ki_push_16((uint16_t)v); flags_logic_w(v); break;
    case RL_POP_WORD: SET_W(D(r),m68ki_pull_16()); flags_logic_w(D(r)); break;
    case RL_RESTORE_WORDS: renderer_load(A(7),v,2,7); break;
    case RL_RAW_LONG: D(r)=v; break;
    case RL_MULS: renderer_multiply(&D(r),(uint16_t)v); break;
    case RL_MULU: timer_multiply_unsigned(&D(r),(uint16_t)v); break;
    case RL_POP_POINTER: A(r-RL_REGISTERS)=m68ki_pull_32(); break;
    case RL_LOAD_WORDS_AT: renderer_load(v,other,2,-1); break;
    case RL_STORE_WORDS_AT: renderer_store(v,other,2,-1); break;
    case RL_LOAD_LONGS_AT: renderer_load(v,other,4,-1); break;
    case RL_LINK_FRAME: m68ki_push_32(A(6)); A(6)=A(7); A(7)-=v; break;
    case RL_UNLINK_FRAME: A(7)=A(6); A(6)=m68ki_pull_32(); break;
    case RL_NEG_LONG: renderer_negate(&D(r),4); break;
    case RL_ADD_BYTE: renderer_add_byte(&D(r),v); break;
    case RL_SUB_BYTE: step_subtract_byte(&D(r),v); break;
    case RL_MEMORY_ADD_BYTE: temporary=v; renderer_add_byte(&temporary,other); break;
    case RL_MEMORY_ADD_LONG: temporary=v; step_add_long(&temporary,other); break;
    case RL_TEST_LONG: flags_logic_l(v); break;
    case RL_POP_MEMORY_LONG: temporary=m68ki_pull_32(); wr_u32(v,temporary); flags_logic_l(temporary); break;
    case RL_ASL_LONG: step_asl_long(&D(r),v); break;
    case RL_LSR_WORD: message_lsr_word(&D(r),v); break;
    case RL_STORE_LONGS_AT: renderer_store(v,other,4,-1); break;
    case RL_OR_BYTE: SET_B(D(r),D(r)|v);flags_logic_b(D(r));break;
    case RL_NEG_BYTE: renderer_negate(&D(r),1);break;
    case RL_BIT_SET: FLAG_Z=v&(1u<<other);D(r)|=1u<<other;break;
    case RL_ROL_BYTE: { unsigned n=v&63u; uint8_t b=(uint8_t)D(r); unsigned i; for(i=0;i<n;++i)b=(uint8_t)((b<<1)|(b>>7));SET_B(D(r),b);flags_logic_b(b);if(n)FLAG_C=(b&1)?CFLAG_SET:0;break; }
    case RL_WAIT_BLITTER:
        REG_PPC=v;
        do { temporary=rd_u16(other+2); FLAG_Z=temporary&0x4000u; if(FLAG_Z)wait_blitter(); }while(FLAG_Z);
        break;
    case RL_ROL_WORD: { unsigned n=v&63u;uint16_t b=(uint16_t)D(r);unsigned i;for(i=0;i<n;++i)b=(uint16_t)((b<<1)|(b>>15));SET_W(D(r),b);flags_logic_w(b);if(n)FLAG_C=(b&1)?CFLAG_SET:0;break; }
    case RL_ROR_LONG: { unsigned n=v&63u;uint32_t b=D(r);unsigned i;for(i=0;i<n;++i)b=(b>>1)|(b<<31);D(r)=b;flags_logic_l(b);if(n)FLAG_C=(b&0x80000000u)?CFLAG_SET:0;break; }
    case RL_POP_POINTER_WORD: A(r-RL_REGISTERS)=(uint32_t)(int32_t)(int16_t)m68ki_pull_16();break;
    /* C2FE02/58/A2: BTST costs 16, BEQ taken 10 / not-taken 8.
     * ADDQ.W absolute and BRA cost 20+10, charged by POLL_REPEAT.
     * Source timing stays in this adapter; the domain owns every increment. */
    case RL_POLL_BUSY: REG_PPC=v;FLAG_Z=rd_u8(other+2)&64u;USE_CYCLES(FLAG_Z?24:26);break;
    case RL_POLL_REPEAT: USE_CYCLES(30);break;
    case RL_POLL_PREFIX: USE_CYCLES(v);break;
    case RL_EXCHANGE_DATA: temporary=D(r);D(r)=D(v);D(v)=temporary;break;
    case RL_MEMORY_SUB_WORD: temporary=v;step_subtract_word(&temporary,other);break;
    case RL_MEMORY_ADD_WORD: temporary=v;step_add_word(&temporary,other);break;
    }
}
static RenderLeafState restored(void *context) { (void)context; return working(); }
static const RenderLeafHooks hooks={consume,outputs,restored,NULL};
int glue_C2F5C0(void) { render_leaf_pixel_in_view(working(),&hooks);return glue_return(); }
int glue_C2F5D4(void) { render_leaf_pixel_restored(working(),&hooks);return glue_return(); }
int glue_C2F5F4(void) { render_leaf_pixel(working(),&hooks);return glue_return(); }
int glue_C2F60A(void) { render_leaf_pixel_pair(working(),&hooks);return glue_return(); }
int glue_C2F66E(void) { render_leaf_pixel_block(working(),&hooks);return glue_return(); }
int glue_C310E2(void) { render_leaf_bound_span(working(),&hooks);return glue_return(); }
int glue_C301F0(void) { render_leaf_polygon_to_row(working(),&hooks);return glue_return(); }
int glue_C304B2(void) { render_leaf_clear_mask(working(),&hooks);return glue_return(); }
int glue_C32806(void) { render_leaf_glyph3(working(),&hooks);return glue_return(); }
int glue_C2FA78(void) { render_leaf_line_to_row(working(),&hooks);return glue_return(); }
int glue_C2FA7E(void) { render_leaf_line(working(),&hooks);return glue_return(); }
int glue_C2FD8C(void) { render_leaf_submit_planes(working(),&hooks);return glue_return(); }

void glue_render_leaf_extent_segment(uint32_t entry) {
 if(entry==0xc30260u){(void)render_leaf_polygon_y_extent(working(),&hooks);REG_PC=0xc30268u;}
 else {(void)render_leaf_polygon_x_extent(working(),&hooks);REG_PC=entry==0xc30274u?0xc3027cu:entry==0xc30290u?0xc30298u:0xc302e6u;}
}

/* The same wait observer used by every line plane; exposed for the cold
 * first-plane busy branch proof without substituting hardware reads. */
void glue_render_leaf_first_plane_wait(void) {
 outputs(NULL,RL_WAIT_BLITTER,RL_VALUE,0xc2fbbau,A(0));
 REG_PC=0xc2fbc8u;
}
