#ifndef FA18_RENDER_PARENTS_H
#define FA18_RENDER_PARENTS_H
#include "memory.h"
enum RenderParentField { RDP_VALUE,RDP_BASE,RDP_CONTROL,RDP_SECONDARY,RDP_SOURCE,RDP_STRIDE,RDP_SIZE,RDP_OFFSET,
    RDP_REGISTERS,RDP_TABLE,RDP_STREAM,RDP_DESCRIPTOR,RDP_SCREEN,RDP_MODULO,RDP_FRAME,RDP_STACK };
typedef struct {
    uint32_t value,base,control,secondary,source,stride,size,offset;
    gaddr registers,table,stream,descriptor,screen,modulo,frame,stack;
    int less,zero;
} RenderParentState;
enum RenderParentChild { RDP_SHAPE_CIRCLE,RDP_SHAPE_POLYGON,RDP_SHAPE_LINE,RDP_BLOCK_FACE,RDP_OFFSET_SEGMENT,RDP_SPLIT_FACE,RDP_SIDE_FIRST_FACE,RDP_SIDE_SECOND_FACE,RDP_SQUARE_FIRST_FACE,RDP_SQUARE_ALTERNATE_FACE,RDP_SQUARE_SECOND_FACE,RDP_SHADOW_FACE,RDP_MARKER_CLASSIFY,RDP_MARKER_FILL,RDP_MARKER_OUTLINE,RDP_VERTEX_TRANSFORM };
enum RenderParentPhase {
    RDP_WORD,RDP_BYTE,RDP_LONG,RDP_POINTER,RDP_ADD_WORD,RDP_SUB_WORD,RDP_ADD_LONG,RDP_SUB_LONG,RDP_NEG_WORD,
    RDP_SWAP,RDP_EXT_WORD,RDP_EXT_LONG,RDP_ASL_WORD,RDP_LSL_WORD,RDP_ASR_WORD,RDP_ASR_BYTE,RDP_LSR_LONG,RDP_ROR_WORD,
    RDP_AND_WORD,RDP_AND_BYTE,RDP_OR_WORD,RDP_COMPARE_WORD,RDP_COMPARE_BYTE,RDP_TEST_WORD,RDP_TEST_BYTE,
    RDP_BIT_TEST,RDP_BIT_CLEAR,RDP_STORE_BYTE,RDP_STORE_WORD,RDP_STORE_LONG,RDP_MEMORY_SUB_BYTE,
    RDP_DECREMENT,RDP_PUSH_LONG,RDP_POP_LONG,RDP_SAVE_WORDS,RDP_RESTORE_WORDS,RDP_ASR_LONG,RDP_LSR_BYTE,RDP_DIVU,RDP_DIVS,RDP_AND_LONG,RDP_COMPARE_LONG,RDP_MEMORY_LOGIC_LONG,RDP_SAVE_LONGS,RDP_RESTORE_LONGS,RDP_PUSH_WORD,RDP_POP_WORD,RDP_RAW_LONG,RDP_MULS,RDP_MULU,RDP_POP_POINTER,RDP_LOAD_WORDS_AT,RDP_STORE_WORDS_AT,RDP_LOAD_LONGS_AT,RDP_LINK_FRAME,RDP_UNLINK_FRAME,RDP_NEG_LONG,RDP_ADD_BYTE,RDP_MEMORY_ADD_BYTE,RDP_TEST_LONG,RDP_POP_MEMORY_LONG,RDP_SUB_BYTE,RDP_MEMORY_ADD_LONG,RDP_ASL_LONG,RDP_STORE_LONGS_AT,RDP_EXCHANGE_DATA,RDP_MEMORY_SUB_WORD,RDP_MEMORY_ADD_WORD,RDP_LSR_WORD
};
typedef struct {
    RenderParentState (*consume)(void *context,enum RenderParentChild child);
    void (*observe)(void *context,enum RenderParentPhase phase,enum RenderParentField field,uint32_t value,uint32_t operand);
    RenderParentState (*restored)(void *context);
    void *context;
} RenderParentHooks;
void render_shape(RenderParentState w,const RenderParentHooks *h);
void render_block_face(RenderParentState w,const RenderParentHooks *h);
void render_offset_run(RenderParentState w,const RenderParentHooks *h);
void render_split_square(RenderParentState w,const RenderParentHooks *h);
void render_side_triangle(RenderParentState w,const RenderParentHooks *h);
void render_square_faces(RenderParentState w,const RenderParentHooks *h);
void render_record_shadow(RenderParentState w,const RenderParentHooks *h);
void render_marker_polygon(RenderParentState w,const RenderParentHooks *h);
void render_face_side(RenderParentState w,const RenderParentHooks *h);
void render_vertices(RenderParentState w,const RenderParentHooks *h);
#endif
