#ifndef FA18_SEGMENT_PROJECTION_H
#define FA18_SEGMENT_PROJECTION_H
#include "memory.h"
/* Coordinates change roles when the second point is projected. Full working
 * words remain observable, including DIVS remainders and saved high halves. */
enum SegmentProjectionField { SP_FIRST_X,SP_FIRST_Y,SP_DEPTH,SP_X,SP_Y,SP_Z,SP_SCRATCH,SP_SELECTOR,
 SP_CURSOR,SP_POINTS,SP_STREAM,SP_WORKSPACES,SP_TARGET,SP_MODULO,SP_FRAME,SP_STACK };
typedef struct {
 uint32_t first_x,first_y,depth,x,y,z,scratch,selector;
 gaddr cursor,points,stream,workspaces,target,modulo,frame,stack;
 int less,zero;
} SegmentProjectionState;
enum SegmentProjectionChild {
 SP_SELECTED_PROJECTED,SP_SELECTED_CLIPPED,SP_PROJECTED_LINE,
 SP_CROSS_EE74,SP_CROSS_EE8A,SP_CROSS_EEAC,SP_CROSS_EEC2,SP_CROSS_EEE8,SP_CROSS_EF06,
 SP_CROSS_EF26,SP_CROSS_EF4C,SP_CROSS_EF6A,SP_CROSS_EF7E,SP_CROSS_EFA2,SP_CROSS_EFB6,
 SP_CROSS_EFD8,SP_CROSS_EFF0,SP_CROSS_F008,SP_CROSS_F028,SP_CLIPPED_LINE
};
enum SegmentProjectionPhase {
 SP_WORD,SP_LONG,SP_POINTER,SP_RAW_LONG,SP_ADD_WORD,SP_SUB_WORD,SP_NEG_WORD,SP_AND_WORD,
 SP_MULS,SP_DIVS,SP_SWAP,SP_ASR_WORD,SP_COMPARE_WORD,SP_COMPARE_LONG,SP_TEST_WORD,
 SP_STORE_WORD,SP_STORE_LONG,SP_SAVE_LONGS,SP_RESTORE_LONGS,SP_LOAD_WORDS,SP_STORE_WORDS,
 SP_LINK,SP_UNLINK,SP_MEMORY_SUB_WORD,SP_PARALLEL_LOOP
};
typedef struct {
 SegmentProjectionState (*consume)(void *,enum SegmentProjectionChild);
 void (*observe)(void *,enum SegmentProjectionPhase,enum SegmentProjectionField,uint32_t,uint32_t);
 SegmentProjectionState (*restored)(void *);
 void *context;
} SegmentProjectionHooks;
void segment_selected(SegmentProjectionState,const SegmentProjectionHooks *,int clipped);
void segment_projected(SegmentProjectionState,const SegmentProjectionHooks *);
void segment_clipped(SegmentProjectionState,const SegmentProjectionHooks *);
void segment_crossing(SegmentProjectionState,const SegmentProjectionHooks *,int axis,int side,int rounded);
#endif
