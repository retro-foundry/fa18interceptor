#include "active_record_pose_commit.h"

#include <assert.h>

int main(void) {
    FA18ActiveRecordPoseFields record = {
        0x11982c00, 0x00072301, 0x1059a000, 0, 0
    };
    FA18ActiveRecordVerticalRoute vertical_route;
    FA18ActiveRecordHorizontalRoute horizontal_route;
    uint8_t status = 0xff;

    /* run060 frame-925 `$C14D32`: `$72301 - $2580 = $6FD81`. */
    assert(fa18_commit_active_record_vertical_component(
               &record, 0x2580, &status, 0, 0, &vertical_route) == 0);
    assert(record.component_y == 0x0006fd81 && status == 0xff &&
           vertical_route == FA18_ACTIVE_RECORD_POSE_C1505E_CONTINUATION);

    record.component_y = 0x709;
    status = 0xff;
    assert(fa18_commit_active_record_vertical_component(
               &record, 0, &status, 0, 0, &vertical_route) == 0 &&
           vertical_route == FA18_ACTIVE_RECORD_POSE_C1505E_CONTINUATION &&
           status == 0xff);
    record.component_y = 0x700;
    record.class_byte_62 = 0x10;
    record.word_4c = 0;
    status = 0xff;
    assert(fa18_commit_active_record_vertical_component(
               &record, 0, &status, 0, 0, &vertical_route) == 0 &&
           record.component_y == 0x708 && status == 0xfb &&
           vertical_route == FA18_ACTIVE_RECORD_POSE_C14D94_CONTINUATION);
    assert(fa18_commit_active_record_vertical_component(
               &record, 0, &status, 0xc0, 0x80, &vertical_route) == 0 &&
           vertical_route == FA18_ACTIVE_RECORD_POSE_C14DE0_CONTINUATION);

    record.class_byte_62 = 0x11;
    assert(fa18_commit_active_record_horizontal_pair(
               &record, -32, 48, &horizontal_route) == 0 &&
           record.component_x == -32 && record.component_z == 48 &&
           horizontal_route == FA18_ACTIVE_RECORD_POSE_C25E86_CONTINUATION);
    record.class_byte_62 = 0x15;
    assert(fa18_commit_active_record_horizontal_pair(
               &record, 1, 2, &horizontal_route) == 0 &&
           horizontal_route == FA18_ACTIVE_RECORD_POSE_CLASS15_CONTINUATION);
    assert(fa18_commit_active_record_vertical_component(
               0, 0, &status, 0, 0, &vertical_route) == -1);
    return 0;
}
