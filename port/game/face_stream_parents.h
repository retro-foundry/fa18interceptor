#ifndef FA18_FACE_STREAM_PARENTS_H
#define FA18_FACE_STREAM_PARENTS_H
#include "memory.h"
enum FaceStreamField { FSP_VALUE,FSP_BASE,FSP_CONTROL,FSP_SECONDARY,FSP_SOURCE,FSP_STRIDE,FSP_SIZE,FSP_OFFSET,
    FSP_REGISTERS,FSP_TABLE,FSP_STREAM,FSP_DESCRIPTOR,FSP_SCREEN,FSP_MODULO,FSP_FRAME,FSP_STACK };
typedef struct {
    uint32_t value,base,control,secondary,source,stride,size,offset;
    gaddr registers,table,stream,descriptor,screen,modulo,frame,stack;
    int less,zero;
} FaceStreamState;
enum FaceStreamChild { FSP_SEGMENT_PAIRS,FSP_SEGMENT_RUN,FSP_OFFSET_SEGMENTS,FSP_PARALLELOGRAM_FACE,FSP_NEAR_PARALLELOGRAM_FACE,FSP_OFFSET_FACE,FSP_MIXED_FACE,FSP_SEGMENT_GRID,FSP_LATTICE_FIRST,FSP_LATTICE_SECOND,FSP_SIDE_FIRST,FSP_SIDE_SECOND,FSP_QUAD_STRIP };
enum FaceStreamPhase {
    FSP_WORD,FSP_BYTE,FSP_LONG,FSP_POINTER,FSP_ADD_WORD,FSP_SUB_WORD,FSP_ADD_LONG,FSP_SUB_LONG,FSP_NEG_WORD,
    FSP_SWAP,FSP_EXT_WORD,FSP_EXT_LONG,FSP_ASL_WORD,FSP_LSL_WORD,FSP_ASR_WORD,FSP_ASR_BYTE,FSP_LSR_LONG,FSP_ROR_WORD,
    FSP_AND_WORD,FSP_AND_BYTE,FSP_OR_WORD,FSP_COMPARE_WORD,FSP_COMPARE_BYTE,FSP_TEST_WORD,FSP_TEST_BYTE,
    FSP_BIT_TEST,FSP_BIT_CLEAR,FSP_STORE_BYTE,FSP_STORE_WORD,FSP_STORE_LONG,FSP_MEMORY_SUB_BYTE,
    FSP_DECREMENT,FSP_PUSH_LONG,FSP_POP_LONG,FSP_SAVE_WORDS,FSP_RESTORE_WORDS,FSP_ASR_LONG,FSP_LSR_BYTE,FSP_DIVU,FSP_DIVS,FSP_AND_LONG,FSP_COMPARE_LONG,FSP_MEMORY_LOGIC_LONG,FSP_SAVE_LONGS,FSP_RESTORE_LONGS,FSP_PUSH_WORD,FSP_POP_WORD,FSP_RAW_LONG,FSP_MULS,FSP_MULU,FSP_POP_POINTER,FSP_LOAD_WORDS_AT,FSP_STORE_WORDS_AT,FSP_LOAD_LONGS_AT,FSP_LINK_FRAME,FSP_UNLINK_FRAME,FSP_NEG_LONG,FSP_ADD_BYTE,FSP_MEMORY_ADD_BYTE,FSP_TEST_LONG,FSP_POP_MEMORY_LONG,FSP_SUB_BYTE,FSP_MEMORY_ADD_LONG,FSP_ASL_LONG,FSP_STORE_LONGS_AT,FSP_EXCHANGE_DATA,FSP_MEMORY_SUB_WORD,FSP_MEMORY_ADD_WORD,FSP_LSR_WORD
};
typedef struct {
    FaceStreamState (*consume)(void *context,enum FaceStreamChild child);
    void (*observe)(void *context,enum FaceStreamPhase phase,enum FaceStreamField field,uint32_t value,uint32_t operand);
    FaceStreamState (*restored)(void *context);
    void *context;
} FaceStreamHooks;
void face_stream_segment_pairs_near(FaceStreamState w,const FaceStreamHooks *h);
void face_stream_segment_pairs(FaceStreamState w,const FaceStreamHooks *h);
void face_stream_segment_run(FaceStreamState w,const FaceStreamHooks *h);
void face_stream_offset_segments(FaceStreamState w,const FaceStreamHooks *h);
void face_stream_parallelogram_face(FaceStreamState w,const FaceStreamHooks *h);
void face_stream_parallelogram_face_two(FaceStreamState w,const FaceStreamHooks *h);
void face_stream_near_parallelogram_face(FaceStreamState w,const FaceStreamHooks *h);
void face_stream_offset_face(FaceStreamState w,const FaceStreamHooks *h);
void face_stream_mixed_face(FaceStreamState w,const FaceStreamHooks *h);
void face_stream_extend_parallelograms(FaceStreamState w,const FaceStreamHooks *h);
void face_stream_extend_parallelograms_scaled(FaceStreamState w,const FaceStreamHooks *h);
void face_stream_segment_grid(FaceStreamState w,const FaceStreamHooks *h);
void face_stream_segment_lattice(FaceStreamState w,const FaceStreamHooks *h);
void face_stream_offset_block_copies(FaceStreamState w,const FaceStreamHooks *h);
void face_stream_extend_block_scaled(FaceStreamState w,const FaceStreamHooks *h);
void face_stream_side_face(FaceStreamState w,const FaceStreamHooks *h);
void face_stream_quad_strip(FaceStreamState w,const FaceStreamHooks *h);
#endif
