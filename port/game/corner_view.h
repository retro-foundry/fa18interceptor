#ifndef FA18_CORNER_VIEW_H
#define FA18_CORNER_VIEW_H
#include "memory.h"
/* Full working values remain observable, including MUL/DIV high words.
 * first_x/first_y/depth become result, neighbour offset and far depth in
 * the corner owner; x/y/z are the close point. Pointer roles follow the
 * original matrix, points, templates, record and frame contracts. */
enum CornerViewField { CV_FIRST_X,CV_FIRST_Y,CV_DEPTH,CV_X,CV_Y,CV_Z,CV_SCRATCH,CV_SELECTOR,
 CV_CURSOR,CV_POINTS,CV_STREAM,CV_WORKSPACES,CV_TARGET,CV_MODULO,CV_FRAME,CV_STACK };
typedef struct {
 uint32_t first_x,first_y,depth,x,y,z,scratch,selector;
 gaddr cursor,points,stream,workspaces,target,modulo,frame,stack;
 int less,zero;
} CornerViewState;
enum CornerViewChild {
 CV_CROSS_E7D0,CV_CROSS_E7E6,CV_CROSS_E816,CV_CROSS_E82C,
 CV_CROSS_E852,CV_CROSS_E870,CV_CROSS_E89C,CV_CROSS_E8C2,
 CV_CROSS_E8EC,CV_CROSS_E900,CV_CROSS_E930,CV_CROSS_E944,
 CV_CROSS_E968,CV_CROSS_E980,CV_CROSS_E9A4,CV_CROSS_E9C4,
 CV_RECORD_ROTATION,CV_RECORD_TEST,CV_PAIR_ROTATION,CV_PAIR_LINE,
 CV_SHAPE_DEPTH,CV_SHAPE_FIRST,CV_SHAPE_SECOND,CV_SHAPE_THIRD,CV_DISTANCE
};
enum CornerViewPhase {
 CV_WORD,CV_LONG,CV_POINTER,CV_RAW_LONG,CV_ADD_WORD,CV_SUB_WORD,CV_NEG_WORD,CV_AND_WORD,
 CV_MULS,CV_DIVS,CV_SWAP,CV_ASR_WORD,CV_COMPARE_WORD,CV_COMPARE_LONG,CV_TEST_WORD,
 CV_STORE_WORD,CV_STORE_LONG,CV_SAVE_LONGS,CV_RESTORE_LONGS,CV_LOAD_WORDS,CV_STORE_WORDS,
 CV_LINK,CV_UNLINK,CV_MEMORY_SUB_WORD,CV_PARALLEL_LOOP,
 CV_ADD_LONG,CV_NEG_LONG,CV_ASL_WORD,CV_ASR_LONG,CV_EXT_LONG,CV_MULU,CV_DIVU,
 CV_LOAD_WORDS_POST,CV_RESTORE_WORDS,CV_SAVE_WORDS,CV_TEST_BYTE,CV_STORE_BYTE,
 CV_BIT_ZERO,CV_MEMORY_ADD_WORD,CV_COMPARE_BYTE,CV_PUSH_POINTER,CV_POP_POINTER,
 CV_RESTORE_LONGS_POST,CV_LOAD_LONGS,CV_LOAD_WORDS_STREAM
};
typedef struct {
 CornerViewState (*consume)(void *,enum CornerViewChild);
 void (*observe)(void *,enum CornerViewPhase,enum CornerViewField,uint32_t,uint32_t);
 CornerViewState (*restored)(void *);
 void *context;
} CornerViewHooks;
void corner_project_edges(CornerViewState,const CornerViewHooks *);
void corner_rotate_view(CornerViewState,const CornerViewHooks *);
void corner_test_record(CornerViewState,const CornerViewHooks *,int layered);
void corner_draw_record_pairs(CornerViewState,const CornerViewHooks *);
void corner_draw_layers(CornerViewState,const CornerViewHooks *);
void corner_distance(CornerViewState,const CornerViewHooks *);
void corner_return_list(CornerViewState,const CornerViewHooks *);
void corner_reject(CornerViewState,const CornerViewHooks *,int value);
#endif
