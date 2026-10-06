#ifndef FA18_FLIGHT_MOTION_HELPERS_H
#define FA18_FLIGHT_MOTION_HELPERS_H
#include "memory.h"
/* Full working values preserve the original mixed word/long geometry.
 * The three normal components also carry incoming velocity in projection. */
enum MotionValue { MH_VALUE,MH_SELECTOR,MH_X,MH_Y,MH_Z,MH_NX,MH_NY,MH_NZ,
    MH_ROOT,MH_RECORD,MH_POINT_X,MH_SCENE,MH_TABLE,MH_FACE };
typedef struct {
    uint32_t value,selector,x,y,z,nx,ny,nz;
    gaddr root,record,point_x,scene,table,face;
    int child_equal;
} MotionState;
enum MotionPhase {
    MH_BYTE,MH_WORD,MH_LONG,MH_POINTER,MH_ADD_WORD,MH_ADD_LONG,MH_SUB_WORD,MH_SUB_LONG,
    MH_AND_WORD,MH_EXT_WORD,MH_EXT_LONG,MH_ASR_WORD,MH_ASR_LONG,MH_ASL_WORD,MH_ASL_LONG,
    MH_LSR_BYTE,MH_LSR_LONG,MH_NEG_WORD,MH_MULTIPLY,MH_DIVIDE,
    MH_COMPARE_BYTE,MH_COMPARE_WORD,MH_COMPARE_LONG,MH_TEST_WORD,MH_BIT_TEST,MH_BIT_CLEAR,
    MH_STORE_BYTE,MH_STORE_WORD,MH_STORE_LONG,MH_MEMORY_ADD_LONG,
    MH_LOAD_POSITION,MH_LOAD_NORMAL,MH_LOAD_VECTOR,MH_LOAD_SLOT,MH_LOAD_FACE,
    MH_SAVE_CURSORS,MH_RESTORE_CURSORS,MH_SCAN_EXHAUSTED
};
typedef struct {
    void (*observe)(void *context,enum MotionPhase phase,enum MotionValue field,uint32_t value,uint32_t operand);
    MotionState (*consume_face)(void *context,int upper_table);
    MotionState (*divide_exception)(void *context,unsigned site);
    void *context;
} MotionHooks;
/* Complete original callable owners, including their original exits. */
MotionState project_record_motion(MotionState w,const MotionHooks *h); /* C26322 */
void publish_motion_slot(MotionState w,const MotionHooks *h); /* C26352 */
void project_scene_motion(MotionState w,gaddr frame,const MotionHooks *h); /* C26C72 */
void test_component_motion(MotionState w,gaddr frame,const MotionHooks *h); /* C26CC0 */
void test_face_motion(MotionState w,gaddr frame,const MotionHooks *h); /* C26D8A */
#endif
