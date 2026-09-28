#include "current_record_matrix.h"

#include "two_angle_matrix.h"

static int16_t read_be16(const uint8_t *bytes) {
    return (int16_t)((uint16_t)bytes[0] << 8 | bytes[1]);
}

int fa18_build_current_record_matrix(FA18CurrentRecordMatrixState *state) {
    int16_t angle;
    if (!state || !state->active_record ||
        state->active_record_size < FA18_CURRENT_RECORD_MATRIX_RECORD_BYTES ||
        !state->trig_table)
        return -1;
    angle = read_be16(state->active_record + 0x68);
    if (angle)
        angle = (int16_t)(UINT16_C(0x7080) - (uint16_t)angle);
    return fa18_build_single_angle_matrix(state->trig_table, angle, state->matrix);
}

void fa18_build_current_record_matrix_callback(void *context) {
    (void)fa18_build_current_record_matrix(context);
}
