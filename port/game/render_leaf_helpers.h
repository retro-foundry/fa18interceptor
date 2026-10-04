#ifndef FA18_RENDER_LEAF_HELPERS_H
#define FA18_RENDER_LEAF_HELPERS_H
#include "memory.h"
enum RenderLeafField { RL_VALUE,RL_BASE,RL_CONTROL,RL_SECONDARY,RL_SOURCE,RL_STRIDE,RL_SIZE,RL_OFFSET,
    RL_REGISTERS,RL_TABLE,RL_STREAM,RL_DESCRIPTOR,RL_SCREEN,RL_MODULO,RL_FRAME,RL_STACK };
typedef struct {
    uint32_t value,base,control,secondary,source,stride,size,offset;
    gaddr registers,table,stream,descriptor,screen,modulo,frame,stack;
    int less,zero;
} RenderLeafState;
enum RenderLeafChild { RL_CALL_C2F5EA,RL_CALL_C2F616,RL_CALL_C2F61E,RL_CALL_C3028A,RL_CALL_C302B6,RL_CALL_C302CE,RL_CALL_C302D6,RL_CALL_C30324,RL_CALL_C3033C,RL_CALL_C2FEEC,RL_CALL_C2FEF4,RL_CALL_C2FF08,RL_CALL_C2FF1A };
enum RenderLeafPhase {
    RL_WORD,RL_BYTE,RL_LONG,RL_POINTER,RL_ADD_WORD,RL_SUB_WORD,RL_ADD_LONG,RL_SUB_LONG,RL_NEG_WORD,
    RL_SWAP,RL_EXT_WORD,RL_EXT_LONG,RL_ASL_WORD,RL_LSL_WORD,RL_ASR_WORD,RL_ASR_BYTE,RL_LSR_LONG,RL_ROR_WORD,
    RL_AND_WORD,RL_AND_BYTE,RL_OR_WORD,RL_COMPARE_WORD,RL_COMPARE_BYTE,RL_TEST_WORD,RL_TEST_BYTE,
    RL_BIT_TEST,RL_BIT_CLEAR,RL_STORE_BYTE,RL_STORE_WORD,RL_STORE_LONG,RL_MEMORY_SUB_BYTE,
    RL_DECREMENT,RL_PUSH_LONG,RL_POP_LONG,RL_SAVE_WORDS,RL_RESTORE_WORDS,RL_ASR_LONG,RL_LSR_BYTE,RL_DIVU,RL_DIVS,RL_AND_LONG,RL_COMPARE_LONG,RL_MEMORY_LOGIC_LONG,RL_SAVE_LONGS,RL_RESTORE_LONGS,RL_PUSH_WORD,RL_POP_WORD,RL_RAW_LONG,RL_MULS,RL_MULU,RL_POP_POINTER,RL_LOAD_WORDS_AT,RL_STORE_WORDS_AT,RL_LOAD_LONGS_AT,RL_LINK_FRAME,RL_UNLINK_FRAME,RL_NEG_LONG,RL_ADD_BYTE,RL_MEMORY_ADD_BYTE,RL_TEST_LONG,RL_POP_MEMORY_LONG,RL_SUB_BYTE,RL_MEMORY_ADD_LONG,RL_ASL_LONG,RL_STORE_LONGS_AT,RL_EXCHANGE_DATA,RL_MEMORY_SUB_WORD,RL_MEMORY_ADD_WORD,RL_LSR_WORD,RL_NEG_BYTE,RL_BIT_SET,RL_ROL_BYTE,RL_WAIT_BLITTER,RL_OR_BYTE,RL_ROL_WORD,RL_ROR_LONG,RL_POP_POINTER_WORD,RL_POLL_BUSY,RL_POLL_REPEAT,RL_POLL_PREFIX
};
typedef struct {
    RenderLeafState (*consume)(void *context,enum RenderLeafChild child);
    void (*observe)(void *context,enum RenderLeafPhase phase,enum RenderLeafField field,uint32_t value,uint32_t operand);
    RenderLeafState (*restored)(void *context);
    void *context;
} RenderLeafHooks;
void render_leaf_pixel_in_view(RenderLeafState w,const RenderLeafHooks *h);
void render_leaf_pixel_restored(RenderLeafState w,const RenderLeafHooks *h);
void render_leaf_pixel(RenderLeafState w,const RenderLeafHooks *h);
void render_leaf_pixel_pair(RenderLeafState w,const RenderLeafHooks *h);
void render_leaf_pixel_block(RenderLeafState w,const RenderLeafHooks *h);
void render_leaf_bound_span(RenderLeafState w,const RenderLeafHooks *h);
void render_leaf_polygon_to_row(RenderLeafState w,const RenderLeafHooks *h);
void render_leaf_clear_mask(RenderLeafState w,const RenderLeafHooks *h);
void render_leaf_glyph3(RenderLeafState w,const RenderLeafHooks *h);
void render_leaf_line_to_row(RenderLeafState w,const RenderLeafHooks *h);
void render_leaf_line(RenderLeafState w,const RenderLeafHooks *h);
void render_leaf_submit_planes(RenderLeafState w,const RenderLeafHooks *h);
RenderLeafState render_leaf_polygon_x_extent(RenderLeafState w,const RenderLeafHooks *h);
RenderLeafState render_leaf_polygon_y_extent(RenderLeafState w,const RenderLeafHooks *h);
#endif
