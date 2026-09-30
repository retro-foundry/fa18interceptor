/* Register bridge for history projection $C0D04C. */
#include "glue.h"
#include "ports_glue.h"

#include "history_projection.h"
#include "globals.h"

void projection_mode_registers(int16_t mode, int entry);

int glue_C0D04C(void) {
    HistoryProjectionWork work = {0};
    uint16_t result = draw_history_projection(&work);
    if (work.record) A(1) = work.record;
    if (work.active) {
        int16_t x = work.final_vector[0], y = work.final_vector[1];
        gaddr matrix = VIEW_ANGLE_MATRIX;
        D(0) = ((uint32_t)((int32_t)rd_s16(matrix + 6) * x) & 0xFFFF0000u) |
               (uint16_t)work.previous[0];
        D(1) = ((uint32_t)((int32_t)rd_s16(matrix + 8) * y) & 0xFFFF0000u) |
               (uint16_t)work.previous[1];
        D(2) = ((uint32_t)work.final_z_full & 0xFFFF0000u) |
               (uint16_t)work.previous[2];
        D(3) = (uint32_t)((int32_t)rd_s16(matrix + 12) * x);
        D(4) = (uint32_t)((int32_t)rd_s16(matrix + 14) * y);
        D(5) = (uint32_t)work.final_z_full;
        D(6) = work.final_d6;
        if (work.final_interpolated) {
            D(1) = (uint32_t)(int32_t)(int16_t)D(1);
            D(2) = (uint32_t)(int32_t)(int16_t)D(2);
            D(4) = (D(4) & 0xFFFFu) |
                   ((uint32_t)(int32_t)work.final_prior[1] & 0xFFFF0000u);
            D(6) = (uint32_t)(int32_t)(int16_t)D(6);
        }
        A(0) = matrix + 18;
        A(3) = (uint32_t)(int32_t)work.previous_shift;
        if (work.final_interpolated && !work.final_intermediate_drawn &&
            !work.final_point_drawn) {
            D(3) = ((uint32_t)(int32_t)work.final_prior[0] & 0xFFFF0000u) |
                   (uint16_t)work.final_interpolation_d3;
            D(4) = ((uint32_t)(int32_t)work.final_prior[1] & 0xFFFF0000u) |
                   (uint16_t)work.final_interpolation_d4;
            D(5) = ((uint32_t)(int32_t)work.final_prior[2] & 0xFFFF0000u) |
                   (uint16_t)work.final_interpolation_d5;
            A(4) = (uint32_t)(int32_t)work.final_shift_difference;
        }
        projection_mode_registers(-4, 2);
    }
    if (work.record) SET_W(D(0), result);
    else D(0) = 0;
    flags_logic_w(D(0));
    return glue_return();
}
