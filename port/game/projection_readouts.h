#ifndef FA18_PROJECTION_READOUTS_H
#define FA18_PROJECTION_READOUTS_H
#include "memory.h"
enum ReadoutValue { PR_X,PR_Y,PR_VALUE,PR_SHIFT,PR_GLYPH,PR_OFFSET,PR_STRIDE,PR_MODE,
    PR_OUTPUT,PR_LAYOUT,PR_TEXT,PR_SHAPE,PR_DESTINATION,PR_PLANES };
typedef struct {
    uint32_t x,y,value,shift,glyph,offset,stride,mode;
    gaddr output,layout,text,shape,destination,planes;
} ReadoutState;
enum ReadoutChild { PR_FAULT,PR_PIXEL,PR_PAIR,PR_BLOCK,PR_CIRCLE,PR_TO_BCD,
    PR_GLYPH_FIRST,PR_GLYPH_SECOND,PR_GLYPH_THIRD,PR_GLYPH_FOURTH,PR_TICK,
    PR_SWEEP_CENTRE,PR_SWEEP_UP_PIXEL,PR_SWEEP_UP_PAIR,PR_SWEEP_DOWN_PIXEL,PR_SWEEP_DOWN_PAIR };
enum ReadoutPhase { PR_WORD,PR_BYTE,PR_LONG,PR_POINTER,PR_ADD_WORD,PR_ADD_LONG,PR_SUB_WORD,
    PR_AND_WORD,PR_OR_WORD,PR_SWAP,PR_EXT_LONG,PR_ASR_WORD,PR_ASL_WORD,PR_LSR_LONG,
    PR_NEG_WORD,PR_MULTIPLY,PR_DIVIDE,PR_COMPARE_WORD,PR_COMPARE_BYTE,PR_TEST_BYTE,
    PR_STORE_BYTE,PR_STORE_WORD,PR_STORE_LONG,PR_BIT_TEST,PR_BIT_CLEAR,PR_DECREMENT,
    PR_PUSH_WORD,PR_POP_WORD,PR_BEGIN_FRAME,PR_END_FRAME,PR_MEMORY_ADD_WORD };
typedef struct {
    ReadoutState (*consume)(void *context,enum ReadoutChild child);
    void (*observe)(void *context,enum ReadoutPhase phase,enum ReadoutValue field,uint32_t value,uint32_t operand);
    ReadoutState (*restored)(void *context);
    gaddr (*frame)(void *context);
    void *context;
} ReadoutHooks;
void project_and_plot_point(ReadoutState w,int selected_mode,const ReadoutHooks *h);
void finish_projected_x_limit(ReadoutState w,const ReadoutHooks *h); /* C2ECD2 */
void finish_projected_y_limit(ReadoutState w,const ReadoutHooks *h); /* C2ECE4 */
void draw_scene_numeric_label(ReadoutState w,const ReadoutHooks *h);
void draw_packed_numeric_readout(ReadoutState w,const ReadoutHooks *h);
void draw_speed_readout_tick(ReadoutState w,const ReadoutHooks *h);
void draw_altitude_readout_tick(ReadoutState w,const ReadoutHooks *h);
void draw_bounded_readout_sweep(ReadoutState w,const ReadoutHooks *h);
#endif
