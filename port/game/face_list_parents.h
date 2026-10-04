#ifndef FA18_FACE_LIST_PARENTS_H
#define FA18_FACE_LIST_PARENTS_H
#include "memory.h"
enum FaceListField { FLS_VALUE,FLS_BASE,FLS_CONTROL,FLS_SECONDARY,FLS_SOURCE,FLS_STRIDE,FLS_SIZE,FLS_OFFSET,
    FLS_REGISTERS,FLS_TABLE,FLS_STREAM,FLS_DESCRIPTOR,FLS_SCREEN,FLS_MODULO,FLS_FRAME,FLS_STACK };
typedef struct {
    uint32_t value,base,control,secondary,source,stride,size,offset;
    gaddr registers,table,stream,descriptor,screen,modulo,frame,stack;
    int less,zero;
} FaceListState;
enum FaceListChild { FLS_STREAM_FACE_TEST,FLS_TESTED_FACE_TEST,FLS_TESTED_FACE_CLIP,FLS_LIST_FACE_TEST,FLS_LIST_FACE_CLIP,FLS_QUAD_LIST,FLS_FACE_GRID,FLS_LATTICE_FIRST,FLS_LATTICE_SECOND,FLS_ALIGNMENT_EDGE,FLS_ALIGNMENT_EYE,FLS_SPLIT_RECORD };
enum FaceListPhase {
    FLS_WORD,FLS_BYTE,FLS_LONG,FLS_POINTER,FLS_ADD_WORD,FLS_SUB_WORD,FLS_ADD_LONG,FLS_SUB_LONG,FLS_NEG_WORD,
    FLS_SWAP,FLS_EXT_WORD,FLS_EXT_LONG,FLS_ASL_WORD,FLS_LSL_WORD,FLS_ASR_WORD,FLS_ASR_BYTE,FLS_LSR_LONG,FLS_ROR_WORD,
    FLS_AND_WORD,FLS_AND_BYTE,FLS_OR_WORD,FLS_COMPARE_WORD,FLS_COMPARE_BYTE,FLS_TEST_WORD,FLS_TEST_BYTE,
    FLS_BIT_TEST,FLS_BIT_CLEAR,FLS_STORE_BYTE,FLS_STORE_WORD,FLS_STORE_LONG,FLS_MEMORY_SUB_BYTE,
    FLS_DECREMENT,FLS_PUSH_LONG,FLS_POP_LONG,FLS_SAVE_WORDS,FLS_RESTORE_WORDS,FLS_ASR_LONG,FLS_LSR_BYTE,FLS_DIVU,FLS_DIVS,FLS_AND_LONG,FLS_COMPARE_LONG,FLS_MEMORY_LOGIC_LONG,FLS_SAVE_LONGS,FLS_RESTORE_LONGS,FLS_PUSH_WORD,FLS_POP_WORD,FLS_RAW_LONG,FLS_MULS,FLS_MULU,FLS_POP_POINTER,FLS_LOAD_WORDS_AT,FLS_STORE_WORDS_AT,FLS_LOAD_LONGS_AT,FLS_LINK_FRAME,FLS_UNLINK_FRAME,FLS_NEG_LONG,FLS_ADD_BYTE,FLS_MEMORY_ADD_BYTE,FLS_TEST_LONG,FLS_POP_MEMORY_LONG,FLS_SUB_BYTE,FLS_MEMORY_ADD_LONG,FLS_ASL_LONG,FLS_STORE_LONGS_AT,FLS_EXCHANGE_DATA,FLS_MEMORY_SUB_WORD,FLS_MEMORY_ADD_WORD,FLS_LSR_WORD
};
typedef struct {
    FaceListState (*consume)(void *context,enum FaceListChild child);
    void (*observe)(void *context,enum FaceListPhase phase,enum FaceListField field,uint32_t value,uint32_t operand);
    FaceListState (*restored)(void *context);
    void *context;
} FaceListHooks;
void face_list_test_stream_face(FaceListState w,const FaceListHooks *h);
void face_list_tested_face(FaceListState w,const FaceListHooks *h);
void face_list_indexed_face_list(FaceListState w,const FaceListHooks *h);
void face_list_quad_list(FaceListState w,const FaceListHooks *h);
void face_list_face_grid(FaceListState w,const FaceListHooks *h);
void face_list_face_grid_plain(FaceListState w,const FaceListHooks *h);
void face_list_face_lattice(FaceListState w,const FaceListHooks *h);
void face_list_face_lattice_plain(FaceListState w,const FaceListHooks *h);
void face_list_tested_parallelogram(FaceListState w,const FaceListHooks *h);
void face_list_edge_alignment(FaceListState w,const FaceListHooks *h);
void face_list_edge_alignment_test(FaceListState w,const FaceListHooks *h);
void face_list_derive_edge_vertices(FaceListState w,const FaceListHooks *h);
void face_list_split_edge(FaceListState w,const FaceListHooks *h);
void face_list_split_record_edges(FaceListState w,const FaceListHooks *h);
#endif
