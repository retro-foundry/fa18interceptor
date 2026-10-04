#ifndef FA18_DISPLAY_RECORD_SELECTION_H
#define FA18_DISPLAY_RECORD_SELECTION_H
#include "memory.h"
enum DisplaySelectionField { DS_FIRST,DS_SECOND,DS_X,DS_Y,DS_Z,DS_ROW_X,DS_ROW_Y,DS_ROW_Z,
 DS_COUNTER,DS_INPUT,DS_COEFFICIENTS,DS_OUTPUT,DS_MATRIX,DS_SPARE,DS_FRAME,DS_STACK };
typedef struct {
 uint32_t first,second,x,y,z,row_x,row_y,row_z;
 gaddr counter,input,coefficients,output,matrix,spare,frame,stack;
} DisplaySelectionState;
enum DisplaySelectionPhase { DS_WORD,DS_LONG,DS_POINTER,DS_STORE_BYTE,DS_STORE_WORD,DS_STORE_LONG,
 DS_ADD_WORD,DS_SUB_WORD,DS_ADD_LONG,DS_AND_WORD,DS_ASR_WORD,DS_ASR_LONG,DS_ASL_WORD,DS_SWAP,
 DS_MULTIPLY_WORD,DS_TEST_BYTE,DS_TEST_WORD,DS_COMPARE_WORD,DS_MEMORY_SUB_WORD,DS_LINK,DS_UNLINK };
typedef struct {
 DisplaySelectionState (*consume)(void *,gaddr original_return);
 void (*observe)(void *,enum DisplaySelectionPhase,enum DisplaySelectionField,uint32_t,uint32_t);
 DisplaySelectionState (*restored)(void *);
 void *context;
} DisplaySelectionHooks;
void prepare_display_record_selection(DisplaySelectionState,int wide,const DisplaySelectionHooks *);
void append_display_mirrored_pair(DisplaySelectionState,const DisplaySelectionHooks *);
void append_display_zero_pair(DisplaySelectionState,const DisplaySelectionHooks *);
void append_display_upper_right(DisplaySelectionState,const DisplaySelectionHooks *);
void append_display_lower_right(DisplaySelectionState,const DisplaySelectionHooks *);
void append_display_lower_left(DisplaySelectionState,const DisplaySelectionHooks *);
#endif
