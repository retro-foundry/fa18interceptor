#include "control_record_matrix_route.h"

#include "matrix_cache_update.h"
#include "two_angle_matrix.h"

static int16_t read_word(const uint8_t *bytes) {
    return (int16_t)((uint16_t)bytes[0] << 8 | bytes[1]);
}

int fa18_update_default_control_record_matrices(
    const FA18FlightTrigTable *trig_table, const uint8_t *record,
    size_t record_size, const FA18ControlRecordMatrixRouteInput *input,
    FA18ControlRecordMatrixRouteOutput *output,
    FA18ControlRecordMatrixRouteResult *result) {
    FA18MatrixCacheUpdateInput cache_input;

    if (!trig_table || !record || record_size < FA18_CONTROL_RECORD_MATRIX_ROUTE_RECORD_BYTES ||
        !input || !output || !result)
        return -1;
    if (input->matrix_route_state || input->matrix_selector || input->matrix_mode > 1 ||
        (record[0x62] & 0xf0u) == 0x30u) {
        *result = FA18_CONTROL_RECORD_MATRIX_ROUTE_UNPORTED_SELECTION;
        return 0;
    }
    output->auxiliary_angle[0] = read_word(record + 0x66);
    output->auxiliary_angle[1] = read_word(record + 0x68);
    output->auxiliary_angle[2] = read_word(record + 0x6a);
    if (fa18_build_single_angle_matrix(trig_table, output->auxiliary_angle[1],
                                       output->third_matrix) != 0 ||
        fa18_compose_three_angle_matrix(trig_table, output->auxiliary_angle[0],
                                        output->auxiliary_angle[1],
                                        output->auxiliary_angle[2],
                                        output->first_matrix) != 0)
        return -1;
    cache_input = (FA18MatrixCacheUpdateInput){
        {output->auxiliary_angle[0], output->auxiliary_angle[1], output->auxiliary_angle[2]},
        {input->row_scale[0], input->row_scale[1], input->row_scale[2]}
    };
    if (fa18_update_projection_matrix_cache(trig_table, &cache_input,
                                            output->second_matrix) != 0)
        return -1;
    *result = FA18_CONTROL_RECORD_MATRIX_ROUTE_DEFAULT;
    return 0;
}
