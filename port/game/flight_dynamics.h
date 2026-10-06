#ifndef FA18_FLIGHT_DYNAMICS_H
#define FA18_FLIGHT_DYNAMICS_H
#include "memory.h"
#include "record_matrix_update.h"
#include "indexed_record_update.h"
/* Working values retain original byte/word halves during mixed-width record,
 * scene and geometry operations. Consumers return all changed working values. */
enum DynamicsValue { DY_PRIMARY,DY_DETAIL,DY_X,DY_Y,DY_Z,DY_RATE_X,DY_RATE_Y,DY_RATE_Z,
    DY_ROOT,DY_RECORD,DY_GEOMETRY,DY_SCENE,DY_TABLE,DY_FACE };
typedef struct {
    uint32_t primary,detail,x,y,z,rate_x,rate_y,rate_z;
    gaddr root,record,geometry,scene,table,face;
    int child_equal;
} DynamicsState;
enum DynamicsChild {
    DY_CELL_MATRIX,DY_SELECTED_RECORD,DY_RECORD_ACTION,DY_RECORD_CONTROLS,DY_DESCENT_ALERT,DY_RECORD_ALERT,
    DY_RECORD_SELECTOR,DY_RECORD_MATRIX,DY_ROOT_FLIGHT,DY_MOTION_CANDIDATE,DY_COLLISION_SOUND,DY_COLLISION_MESSAGE,
    DY_COLLISION_FAULT,DY_GROUND_PROJECTION,DY_REGION_PROBE,DY_MOTION_SLOT,DY_RECORD_TIMER,
    DY_COMPONENT_COLLISION,DY_FACE_COLLISION,DY_REGION_ENTER,DY_REGION_ACTIVE,DY_REGION_RELEASE,DY_REGION_REPLACE,
    DY_PLACE_RECORD,DY_ORIENT_RECORD
};
enum DynamicsPhase {
    DY_BYTE,DY_WORD,DY_LONG,DY_POINTER,DY_ADD_BYTE,DY_ADD_WORD,DY_ADD_LONG,DY_SUB_BYTE,DY_SUB_WORD,DY_SUB_LONG,
    DY_AND_BYTE,DY_AND_WORD,DY_AND_LONG,DY_OR_BYTE,DY_EXT_WORD,DY_EXT_LONG,DY_SWAP,DY_ASR_WORD,DY_ASR_LONG,
    DY_ASL_WORD,DY_ASL_LONG,DY_LSR_WORD,DY_ROL_LONG,DY_NEG_WORD,DY_NEG_LONG,DY_MULTIPLY,
    DY_TEST_BYTE,DY_TEST_WORD,DY_TEST_LONG,DY_COMPARE_BYTE,DY_COMPARE_WORD,DY_COMPARE_LONG,
    DY_BIT_TEST,DY_BIT_SET,DY_BIT_CLEAR,DY_REGISTER_BIT_SET,DY_REGISTER_BIT_CLEAR,
    DY_STORE_BYTE,DY_STORE_WORD,DY_STORE_LONG,DY_MEMORY_ADD_BYTE,DY_MEMORY_ADD_WORD,DY_MEMORY_ADD_LONG,
    DY_MEMORY_SUB_WORD,DY_MEMORY_SUB_LONG,DY_DECREMENT,
    DY_LOAD_WORDS,DY_LOAD_LONGS,DY_STORE_WORDS,DY_STORE_LONGS,
    DY_SAVE_RECORD,DY_RESTORE_RECORD,DY_SAVE_CELL,DY_RESTORE_CELL,DY_SAVE_RATES,DY_RESTORE_RATES,
    DY_SAVE_ORIENTATION,DY_RESTORE_ORIENTATION,DY_SAVE_SCAN,DY_RESTORE_SCAN,DY_SOUND_ARGUMENTS,
    DY_BEGIN_FRAME,DY_END_FRAME,DY_AUTOPILOT_LIMIT,DY_AUTOPILOT_TOGGLE
};
typedef struct {
    DynamicsState (*consume)(void *context,enum DynamicsChild child);
    void (*observe)(void *context,enum DynamicsPhase phase,enum DynamicsValue field,uint32_t value,uint32_t operand);
    gaddr (*frame)(void *context);
    DynamicsState (*restored)(void *context);
    void *context;
} DynamicsHooks;
void advance_indexed_record_dynamics(DynamicsState w,const DynamicsHooks *h); /* C25B66 */
/* C25B66's matrix phase: ordinary C call ownership while its outer timing
 * boundary remains in the temporary resumable CPU adapter. */
void update_dynamics_record_matrix(const RecordMatrixInput *input, RecordMatrixResult *result,
                                   RecordMatrixSideHook side_hook, void *context);
/* Input and selected-record phases owned by the same live flight parent. */
void update_dynamics_record_input(gaddr record, uint32_t incoming);
void update_dynamics_selected_record(IndexedRecordWork *work);
void collide_scene_motion(DynamicsState w,const DynamicsHooks *h); /* C266AE */
void update_scene_regions(DynamicsState w,const DynamicsHooks *h); /* C28996 */
void spawn_region_records(DynamicsState w,const DynamicsHooks *h); /* C28B16 */
void dispatch_region_records(DynamicsState w,const DynamicsHooks *h); /* C28B34 shared body */
/* C2C392's forty original action arms. A phase is C continuation state,
 * not a guest PC. Existing children remain at the temporary outer boundary. */
enum AutopilotPhase {
    AP_BEGIN, AP_AFTER_FAULT, AP_AFTER_NORMALIZE, AP_AFTER_TURN,
    AP_AFTER_PITCH, AP_AFTER_SIMPLE_ROLL, AP_AFTER_WAIT_PITCH,
    AP_AFTER_RATE_ROLL, AP_AFTER_RATE_NEUTRAL, AP_AFTER_LOOP_PITCH,
    AP_AFTER_LOOP_LEVEL, AP_AFTER_BANK_ROLL, AP_AFTER_PITCH_ARC,
    AP_AFTER_LEVEL_ROLL, AP_AFTER_REVERSE_PITCH, AP_AFTER_COMBINED_ROLL, AP_AFTER_COMBINED_PITCH,
    AP_AFTER_REVERSE_ROLL, AP_AFTER_DIVE_PITCH, AP_AFTER_DIVE_LEVEL,
    AP_AFTER_CLIMB_PITCH, AP_AFTER_CLIMB_LEVEL,
    AP_ORIGINAL_TRANSFER, AP_COMPLETE
};
typedef struct {
    DynamicsState work;
    enum AutopilotPhase phase;
    int32_t positive_roll, positive_pitch, hysteresis;
    int32_t negative_roll, negative_pitch, negative_hysteresis;
    int32_t direction_limit, steep_limit;
    gaddr unresolved_target;
} AutopilotFrame;
/* Returns one at completion, zero when the frame needs its selected child.
 * The original out-of-table transfer is retained as an unresolved boundary. */
int advance_record_autopilot(AutopilotFrame *frame, const DynamicsHooks *hooks);
int update_dynamics_record_action(AutopilotFrame *frame, const DynamicsHooks *hooks);
#endif
