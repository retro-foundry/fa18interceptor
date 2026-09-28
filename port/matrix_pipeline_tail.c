#include "matrix_pipeline_tail.h"

#include "matrix_row_scale.h"
#include "two_angle_matrix.h"

#include <string.h>

int fa18_run_matrix_pipeline_tail(const FA18FlightTrigTable *trig_table,
                                  const FA18MatrixPipelineTailInput *input,
                                  FA18MatrixPipelineTailState *state) {
    if (!trig_table || !input || !state) return -1;
    if (fa18_build_two_angle_matrix(trig_table, input->input_x, input->input_z,
                                    state->projection_matrix) != 0 ||
        fa18_scale_matrix_rows(state->projection_matrix, input->row_scale) != 0 ||
        fa18_build_single_angle_matrix(trig_table, input->input_z,
                                       state->single_angle_matrix) != 0)
        return -1;
    memcpy(state->auxiliary, input->record_auxiliary, sizeof state->auxiliary);
    return 0;
}
