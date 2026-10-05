#ifndef FA18_NATIVE_RECORD_POSE_H
#define FA18_NATIVE_RECORD_POSE_H
#include "native_scene_records.h"

typedef enum {
    FA18_POSE_CELL_MATRIX,FA18_POSE_SELECTED_RECORD,FA18_POSE_RECORD_ACTION,
    FA18_POSE_RECORD_CONTROLS,FA18_POSE_MESSAGE,FA18_POSE_RECORD_SELECTOR,
    FA18_POSE_RECORD_MATRIX,FA18_POSE_ROOT_FLIGHT,FA18_POSE_MOTION_CANDIDATE,
    FA18_POSE_SOUND,FA18_POSE_FAULT,FA18_POSE_GROUND_PROJECTION,
    FA18_POSE_REGION_PROBE,FA18_POSE_MOTION_SLOT
} FA18NativeRecordPoseChild;
typedef struct {
    unsigned slot;
    uint16_t choice; /* message code or motion-slot selection */
    uint16_t sound_arguments[2];
    uint32_t point[3]; /* motion candidate */
    uint32_t velocity[3]; /* ground projection */
    int16_t angles[3]; /* cell matrix */
} FA18NativeRecordPoseInput;
typedef struct {
    uint16_t status;
    int clear; /* motion-candidate result, independent of child completion */
} FA18NativeRecordPoseResult;
typedef struct FA18NativeRecordPose FA18NativeRecordPose;
typedef struct {
    PortFieldByte *fields;
    size_t byte_count,origin; /* first C4FDD4 history tuple, signed row offsets */
} FA18NativeRecordMotionHistory;
typedef struct {
    int (*consume)(void *context,FA18NativeRecordPose *state,FA18NativeRecordPoseChild child,
        const FA18NativeRecordPoseInput *input,FA18NativeRecordPoseResult *result);
    void *context;
} FA18NativeRecordPoseOps;
struct FA18NativeRecordPose {
    FA18NativeSceneRecords *records;
    const FA18NativeRecordPoseOps *ops;
    const FA18NativeRecordMotionHistory *history;
    uint16_t *current_slot,*current_stride,*target_slot,*selector_word,*matrix_control;
    uint16_t *shown_message,*grid_x,*grid_z,*error_word,*collision_slot,*damage_count;
    uint32_t *events;
    uint8_t *cell_only,*post_input_event,*origin_enable,*activity_count,*scene_redraw;
    uint8_t *bar_redraw,*view_decay,*collision_enable,*collision_inhibit,*cockpit_a,*cockpit_b;
    uint8_t *collision_report,*mission_failure,*failure_view,*request_flag,*request_clear;
    uint8_t *history_count,*history_index;
};
/* Complete C25B66 with actual C2651E history owner. Actual lower motion,
 * matrix, input and collision children remain route-dependent. */
int fa18_update_native_record_pose(FA18NativeRecordPose *state,unsigned slot);
int fa18_update_native_record_motion_history(FA18NativeRecordPose *state);
#endif
