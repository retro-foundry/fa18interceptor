#include "matrix_cache_update.h"

#include "matrix_row_scale.h"
#include "two_angle_matrix.h"

int fa18_update_projection_matrix_cache(
    const FA18FlightTrigTable *trig_table,
    const FA18MatrixCacheUpdateInput *input,
    int16_t projection_matrix[3][3]) {
    if (!trig_table || !input || !projection_matrix ||
        fa18_compose_three_angle_matrix(trig_table, input->angle[0],
                                        input->angle[1], input->angle[2],
                                        projection_matrix) != 0)
        return -1;
    return fa18_scale_matrix_rows(projection_matrix, input->row_scale);
}
