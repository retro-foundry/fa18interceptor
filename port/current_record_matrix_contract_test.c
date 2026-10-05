#include "current_record_matrix.h"
#include "run075_trig_asset.h"
#include "hunk.h"
#include "two_angle_matrix.h"

#include <assert.h>
#include <string.h>

static void put16(uint8_t *bytes, uint16_t value) {
    bytes[0] = (uint8_t)(value >> 8); bytes[1] = (uint8_t)value;
}

int main(void) {
    uint8_t trig_bytes[0xae8 + sizeof fa18_run075_trig_bytes] = {0};
    uint8_t record[FA18_CURRENT_RECORD_MATRIX_RECORD_BYTES] = {0};
    FA18HunkSegment segments[64] = {0};
    FA18Hunks hunks = {segments, 64};
    FA18FlightTrigTable trig = {0};
    FA18CurrentRecordMatrixState state;

    memcpy(trig_bytes + 0xae8, fa18_run075_trig_bytes, sizeof fa18_run075_trig_bytes);
    segments[63] = (FA18HunkSegment){
        .kind = FA18_HUNK_DATA, .data = trig_bytes, .size = sizeof trig_bytes
    };
    assert(fa18_load_two_angle_trig_table(&hunks, &trig) == 0);
    state = (FA18CurrentRecordMatrixState){record, sizeof record, &trig, {{0}}};
    put16(record + 0x68, 0x7080);
    assert(fa18_build_current_record_matrix(&state) == 0);
    assert(state.matrix[0][0] == 0x4000 && state.matrix[1][1] == 0x4000 &&
           state.matrix[2][2] == 0x4000);
    put16(record + 0x68, 0);
    assert(fa18_build_current_record_matrix(&state) == 0);
    assert(state.matrix[0][0] != 0 || state.matrix[0][2] != 0);
    assert(fa18_build_current_record_matrix(NULL) == -1);
    return 0;
}
