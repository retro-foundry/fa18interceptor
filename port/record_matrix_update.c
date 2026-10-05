#include "record_matrix_update.h"

#include "flight.h"
#include "two_angle_matrix.h"

#include <string.h>

int fa18_update_record_matrix(FA18RecordMatrixUpdateState *state,
                              const FA18RecordMatrixUpdateInput *input,
                              const FA18RecordMatrixUpdateOps *ops) {
    if (!state || !input || !ops || !ops->build || !ops->compose) return -1;
    const int16_t build_input[3] = { input->d4, input->d5, input->d6 };
    const int16_t compose_input[3] = {
        input->d4 ? (int16_t)((uint16_t)0x7080u - (uint16_t)input->d4) : 0,
        input->d5 ? (int16_t)((uint16_t)0x7080u - (uint16_t)input->d5) : 0,
        input->d6 ? (int16_t)((uint16_t)0x7080u - (uint16_t)input->d6) : 0
    };
    state->control_byte_03 &= (uint8_t)~4u;
    state->published[0] = input->d4;
    state->published[1] = input->d5;
    state->published[2] = input->d6;
    if (ops->build(ops->context, build_input, state->build_matrix) != 0) return -1;
    return ops->compose(ops->context, compose_input, state->attitude_matrix);
}

int fa18_update_record_matrix_native(FA18RecordMatrixUpdateState *state,
                                     const FA18RecordMatrixUpdateInput *input,
                                     const FA18FlightTrigTable *trig_table) {
    if (!state || !input || !trig_table) return -1;
    const int16_t compose_input[3] = {
        input->d4 ? (int16_t)((uint16_t)0x7080u - (uint16_t)input->d4) : 0,
        input->d5 ? (int16_t)((uint16_t)0x7080u - (uint16_t)input->d5) : 0,
        input->d6 ? (int16_t)((uint16_t)0x7080u - (uint16_t)input->d6) : 0
    };
    state->control_byte_03 &= (uint8_t)~4u;
    state->published[0] = input->d4;
    state->published[1] = input->d5;
    state->published[2] = input->d6;
    if (fa18_build_rotation_matrix(trig_table, input->d4, input->d5, input->d6,
                                   state->build_matrix) != 0) return -1;
    FA18FlightPose pose = {0};
    if (fa18_flight_update_attitude(trig_table, compose_input[0],
                                    compose_input[1], compose_input[2],
                                    &pose) != 0) return -1;
    memcpy(state->attitude_matrix, pose.attitude, sizeof state->attitude_matrix);
    return 0;
}
