#include "control_record_matrix_route.h"
#include "run075_trig_asset.h"
#include "two_angle_matrix.h"

#include <assert.h>
#include <string.h>

static void put_word(uint8_t *bytes, uint16_t value) {
    bytes[0] = (uint8_t)(value >> 8);
    bytes[1] = (uint8_t)value;
}

int main(void) {
    uint8_t hunk63[0xae8 + sizeof fa18_run075_trig_bytes] = {0};
    FA18HunkSegment segments[64] = {0};
    FA18Hunks hunks = {segments, 64};
    FA18FlightTrigTable trig = {0};
    uint8_t record[FA18_CONTROL_RECORD_MATRIX_ROUTE_RECORD_BYTES] = {0};
    FA18ControlRecordMatrixRouteInput input = {0, 0, 0, {168, 252, 128}};
    FA18ControlRecordMatrixRouteOutput output;
    FA18ControlRecordMatrixRouteResult result;
    const int16_t expected_second[3][3] = {
        {167, 0, -8}, {0, 252, 0}, {6, 0, 127}
    };

    memcpy(hunk63 + 0xae8, fa18_run075_trig_bytes, sizeof fa18_run075_trig_bytes);
    segments[63] = (FA18HunkSegment){.data = hunk63, .size = sizeof hunk63};
    assert(fa18_load_two_angle_trig_table(&hunks, &trig) == 0);
    put_word(record + 0x66, 0);
    put_word(record + 0x68, 28600);
    put_word(record + 0x6a, 0);
    /* run075 local frame 189: `$C2DB9E` loads this triple before the direct
     * `$C2DC9A` second-cache update. */
    assert(fa18_update_default_control_record_matrices(
               &trig, record, sizeof record, &input, &output, &result) == 0 &&
           result == FA18_CONTROL_RECORD_MATRIX_ROUTE_DEFAULT);
    assert(!memcmp(output.auxiliary_angle, (int16_t[]){0, 28600, 0},
                   sizeof output.auxiliary_angle));
    assert(!memcmp(output.second_matrix, expected_second, sizeof expected_second));
    input.matrix_selector = 1;
    assert(fa18_update_default_control_record_matrices(
               &trig, record, sizeof record, &input, &output, &result) == 0 &&
           result == FA18_CONTROL_RECORD_MATRIX_ROUTE_UNPORTED_SELECTION);
    assert(fa18_update_default_control_record_matrices(
               &trig, record, sizeof record - 1, &input, &output, &result) == -1);
    return 0;
}
