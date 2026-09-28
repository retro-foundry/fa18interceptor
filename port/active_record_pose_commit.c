#include "active_record_pose_commit.h"

int fa18_commit_active_record_vertical_component(
    FA18ActiveRecordPoseFields *record, int32_t local_delta,
    uint8_t *status_byte, uint8_t secondary_status_byte,
    uint16_t secondary_control_word, FA18ActiveRecordVerticalRoute *route) {
    const int32_t limit = 0x708;

    if (!record || !status_byte || !route) return -1;
    record->component_y = (int32_t)((uint32_t)record->component_y -
                                    (uint32_t)local_delta);
    if (record->component_y > limit) {
        *route = FA18_ACTIVE_RECORD_POSE_C1505E_CONTINUATION;
        return 0;
    }
    *status_byte &= (uint8_t)~UINT8_C(0x04);
    if (!(record->class_byte_62 & UINT8_C(0xf0))) {
        *route = FA18_ACTIVE_RECORD_POSE_C14D68_CONTINUATION;
        return 0;
    }
    record->component_y = limit;
    if (secondary_status_byte & UINT8_C(0xc0)) {
        *route = FA18_ACTIVE_RECORD_POSE_C14DE0_CONTINUATION;
        return 0;
    }
    if (!(secondary_control_word & UINT16_C(0x0080))) {
        *route = FA18_ACTIVE_RECORD_POSE_C14D94_CONTINUATION;
        return 0;
    }
    *route = record->word_4c & UINT16_C(0x0007) ?
        FA18_ACTIVE_RECORD_POSE_C14DE0_CONTINUATION :
        FA18_ACTIVE_RECORD_POSE_C14D94_CONTINUATION;
    return 0;
}

int fa18_commit_active_record_horizontal_pair(
    FA18ActiveRecordPoseFields *record, int32_t d2, int32_t d4,
    FA18ActiveRecordHorizontalRoute *route) {
    if (!record || !route) return -1;
    record->component_x = d2;
    record->component_z = d4;
    *route = record->class_byte_62 == UINT8_C(0x15) ?
        FA18_ACTIVE_RECORD_POSE_CLASS15_CONTINUATION :
        FA18_ACTIVE_RECORD_POSE_C25E86_CONTINUATION;
    return 0;
}
