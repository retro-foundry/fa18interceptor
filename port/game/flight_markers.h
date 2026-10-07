#ifndef FA18_FLIGHT_MARKERS_H
#define FA18_FLIGHT_MARKERS_H
#include "memory.h"
/* Source working values: packed X/Z offset, projected Y, point components,
 * matrix products, record and stream cursors. Partial words remain visible. */
enum MarkerValue { MM_OFFSET,MM_SCREEN_Y,MM_X,MM_Y,MM_Z,MM_ROW_X,MM_ROW_Y,MM_ROW_Z,
    MM_RECORD,MM_MATRIX,MM_GEOMETRY,MM_SCENE,MM_ORIGIN,MM_POINTS };
typedef struct {
    uint32_t offset,screen_y,x,y,z,row_x,row_y,row_z;
    gaddr record,matrix,geometry,scene,origin,points;
    int child_negative,child_equal;
} MarkerState;
enum MarkerChild {
    MM_SCENE_PROJECT,MM_SCENE_LABEL,MM_GRID_X_FIRST,MM_GRID_X_SECOND,MM_GRID_X_LINE,MM_GRID_X_LABEL,
    MM_GRID_Z_FIRST,MM_GRID_Z_SECOND,MM_GRID_Z_LINE,MM_GRID_Z_LABEL,MM_RECORD_POINT,
    MM_CLASS20_MARKER,MM_RECORD_MARKER,MM_CLASS20_PROJECT,MM_MARKER_POINT,MM_MARKER_PROJECT,MM_MARKER_LINE
};
enum MarkerPhase {
    MM_BYTE,MM_WORD,MM_LONG,MM_POINTER,MM_ADD_BYTE,MM_ADD_WORD,MM_ADD_LONG,MM_SUB_BYTE,MM_SUB_WORD,MM_SUB_LONG,
    MM_AND_BYTE,MM_AND_WORD,MM_AND_LONG,MM_OR_BYTE,MM_EXT_WORD,MM_EXT_LONG,MM_SWAP,MM_ASR_WORD,MM_ASR_LONG,
    MM_ASL_WORD,MM_ASL_LONG,MM_LSR_WORD,MM_ROL_LONG,MM_NEG_WORD,MM_NEG_LONG,MM_MULTIPLY,
    MM_TEST_BYTE,MM_TEST_WORD,MM_TEST_LONG,MM_COMPARE_BYTE,MM_COMPARE_WORD,MM_COMPARE_LONG,
    MM_BIT_TEST,MM_BIT_SET,MM_BIT_CLEAR,MM_STORE_BYTE,MM_STORE_WORD,MM_STORE_LONG,
    MM_MEMORY_ADD_BYTE,MM_MEMORY_ADD_WORD,MM_MEMORY_ADD_LONG,MM_MEMORY_SUB_WORD,MM_MEMORY_SUB_LONG,MM_DECREMENT,
    MM_LOAD_WORDS,MM_LOAD_LONGS,MM_STORE_WORDS,MM_STORE_LONGS,MM_BEGIN_FRAME,MM_END_FRAME,
    MM_NEG_BYTE,MM_EXCHANGE,MM_ASL_BYTE,MM_LSR_BYTE,
    MM_PUSH_WORD,MM_POP_WORD,MM_PUSH_LONG,MM_POP_LONG,MM_SAVE_SCENE,MM_RESTORE_SCENE,MM_SAVE_DRAW,MM_RESTORE_DRAW
};
typedef struct {
    MarkerState (*consume)(void *context,enum MarkerChild child);
    void (*observe)(void *context,enum MarkerPhase phase,enum MarkerValue field,uint32_t value,uint32_t operand);
    gaddr (*frame)(void *context);
    MarkerState (*restored)(void *context);
    gaddr (*stack)(void *context);
    void *context;
    MarkerState (*consume_values)(void *context,enum MarkerChild child,MarkerState values);
} MarkerHooks;
MarkerState transform_marker_point(MarkerState w,const MarkerHooks *h); /* C2AFFA */
/* C2B3C2: nonzero when the label pass was selected; skipped gates preserve
 * the preceding drawing return. */
int draw_scene_position_labels(MarkerState w,const MarkerHooks *h);
void draw_view_grid_and_markers(MarkerState w,const MarkerHooks *h); /* C2B564 */
MarkerState draw_class_twenty_marker(MarkerState w,const MarkerHooks *h); /* C2B928 */
MarkerState draw_record_position_marker(MarkerState w,gaddr frame,const MarkerHooks *h); /* C2B952 */
#endif
