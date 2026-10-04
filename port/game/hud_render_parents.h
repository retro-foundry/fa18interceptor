#ifndef FA18_HUD_RENDER_PARENTS_H
#define FA18_HUD_RENDER_PARENTS_H
#include "memory.h"
enum HudRenderField { HRR_VALUE,HRR_BASE,HRR_CONTROL,HRR_SECONDARY,HRR_SOURCE,HRR_STRIDE,HRR_SIZE,HRR_OFFSET,
    HRR_REGISTERS,HRR_TABLE,HRR_STREAM,HRR_DESCRIPTOR,HRR_SCREEN,HRR_MODULO,HRR_FRAME,HRR_STACK };
typedef struct {
    uint32_t value,base,control,secondary,source,stride,size,offset;
    gaddr registers,table,stream,descriptor,screen,modulo,frame,stack;
    int less,zero;
} HudRenderState;
enum HudRenderChild { HRR_CALL_C0999C,HRR_CALL_C09A5A,HRR_CALL_C099E8,HRR_CALL_C3332A,HRR_CALL_C3333A,HRR_CALL_C3334A,HRR_CALL_C3335C,HRR_CALL_C33368,HRR_CALL_C34160,HRR_CALL_C3416A,HRR_CALL_C34172,HRR_CALL_C3417A,HRR_CALL_C341AC,HRR_CALL_C341E6,HRR_CALL_C34218,HRR_CALL_C342B6,HRR_CALL_C34074,HRR_CALL_C3407E,HRR_CALL_C340BA,HRR_CALL_C340C2,HRR_CALL_C340CE,HRR_CALL_C3411E,HRR_CALL_C34126,HRR_CALL_C34132,HRR_CALL_C34514,HRR_CALL_C348A6,HRR_CALL_C33ED8,HRR_CALL_C33F40,HRR_CALL_C33F4C,HRR_CALL_C32496,HRR_CALL_C324C8,HRR_CALL_C32508,HRR_CALL_C325E6,HRR_CALL_C325F8,HRR_CALL_C3260A,HRR_CALL_C32658,HRR_CALL_C327EA,HRR_CALL_C327FE,HRR_CALL_C30078,HRR_CALL_C30104,HRR_CALL_C30136,HRR_CALL_C30142,HRR_CALL_C3014A,HRR_CALL_C3015E,HRR_CALL_C30166,HRR_CALL_C3016E,HRR_CALL_C30176,HRR_CALL_C30180,HRR_CALL_C30186,HRR_CALL_C3018C,HRR_CALL_C30194,HRR_CALL_C34606,HRR_CALL_C34630,HRR_CALL_C3465A,HRR_CALL_C34684,HRR_CALL_C346F8,HRR_CALL_C34744,HRR_CALL_C34792,HRR_CALL_C347E0,HRR_CALL_C3493C };
enum HudRenderPhase {
    HRR_WORD,HRR_BYTE,HRR_LONG,HRR_POINTER,HRR_ADD_WORD,HRR_SUB_WORD,HRR_ADD_LONG,HRR_SUB_LONG,HRR_NEG_WORD,
    HRR_SWAP,HRR_EXT_WORD,HRR_EXT_LONG,HRR_ASL_WORD,HRR_LSL_WORD,HRR_ASR_WORD,HRR_ASR_BYTE,HRR_LSR_LONG,HRR_ROR_WORD,
    HRR_AND_WORD,HRR_AND_BYTE,HRR_OR_WORD,HRR_COMPARE_WORD,HRR_COMPARE_BYTE,HRR_TEST_WORD,HRR_TEST_BYTE,
    HRR_BIT_TEST,HRR_BIT_CLEAR,HRR_STORE_BYTE,HRR_STORE_WORD,HRR_STORE_LONG,HRR_MEMORY_SUB_BYTE,
    HRR_DECREMENT,HRR_PUSH_LONG,HRR_POP_LONG,HRR_SAVE_WORDS,HRR_RESTORE_WORDS,HRR_ASR_LONG,HRR_LSR_BYTE,HRR_DIVU,HRR_DIVS,HRR_AND_LONG,HRR_COMPARE_LONG,HRR_MEMORY_LOGIC_LONG,HRR_SAVE_LONGS,HRR_RESTORE_LONGS,HRR_PUSH_WORD,HRR_POP_WORD,HRR_RAW_LONG,HRR_MULS,HRR_MULU,HRR_POP_POINTER,HRR_LOAD_WORDS_AT,HRR_STORE_WORDS_AT,HRR_LOAD_LONGS_AT,HRR_LINK_FRAME,HRR_UNLINK_FRAME,HRR_NEG_LONG,HRR_ADD_BYTE,HRR_MEMORY_ADD_BYTE,HRR_TEST_LONG,HRR_POP_MEMORY_LONG,HRR_SUB_BYTE,HRR_MEMORY_ADD_LONG,HRR_ASL_LONG,HRR_STORE_LONGS_AT,HRR_EXCHANGE_DATA,HRR_MEMORY_SUB_WORD,HRR_MEMORY_ADD_WORD,HRR_LSR_WORD,HRR_NEG_BYTE,HRR_BIT_SET,HRR_ROL_BYTE,HRR_WAIT_BLITTER,HRR_OR_BYTE
};
typedef struct {
    HudRenderState (*consume)(void *context,enum HudRenderChild child);
    void (*observe)(void *context,enum HudRenderPhase phase,enum HudRenderField field,uint32_t value,uint32_t operand);
    HudRenderState (*restored)(void *context);
    void *context;
} HudRenderHooks;
void hud_render_ground_points(HudRenderState w,const HudRenderHooks *h);
void hud_render_indexed_face(HudRenderState w,const HudRenderHooks *h);
void hud_render_outlined_face(HudRenderState w,const HudRenderHooks *h);
void hud_render_coloured_face(HudRenderState w,const HudRenderHooks *h);
void hud_render_view_marker(HudRenderState w,const HudRenderHooks *h);
void hud_render_hud_marks(HudRenderState w,const HudRenderHooks *h);
HudRenderState hud_render_frame_right(HudRenderState w,const HudRenderHooks *h);
int hud_render_page_delay(const HudRenderHooks *h);
void hud_render_tick_row(HudRenderState w,const HudRenderHooks *h);
void hud_render_target_box(HudRenderState w,const HudRenderHooks *h);
void hud_render_ring_point(HudRenderState w,const HudRenderHooks *h);
void hud_render_seeker_state(HudRenderState w,const HudRenderHooks *h);
void hud_render_message_line(HudRenderState w,const HudRenderHooks *h);
void hud_render_panel_mark(HudRenderState w,const HudRenderHooks *h);
void hud_render_blit_lane(HudRenderState w,const HudRenderHooks *h);
void hud_render_ring(HudRenderState w,const HudRenderHooks *h);
void hud_render_symbol(HudRenderState w,const HudRenderHooks *h);
#endif
