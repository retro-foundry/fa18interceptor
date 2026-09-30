#ifndef FA18_GAME_HISTORY_PROJECTION_H
#define FA18_GAME_HISTORY_PROJECTION_H

#include "memory.h"

/* Working state of the $C0D04C history-point projection loop. */
typedef struct HistoryProjectionWork {
    gaddr record, slot_address;
    int8_t remaining, slot_index;
    int16_t direction, shift;
    int32_t delta[3], absolute[3];
    int16_t previous[3], previous_radius, previous_shift;
    int16_t final_vector[3];
    int32_t final_y_full, final_z_full;
    uint32_t final_d6;
    int16_t final_prior[3];
    int16_t final_interpolation_d3, final_interpolation_d4, final_interpolation_d5;
    int16_t final_shift_difference;
    int final_interpolated;
    int final_intermediate_drawn, final_point_drawn;
    uint16_t drawn;
    int active;
} HistoryProjectionWork;

/* $C0D04C-$C0D10A: choose the first slot and traversal direction. Returns
 * zero when the source returns before entering its point loop. */
int begin_history_projection(HistoryProjectionWork *work);

/* $C0D110-$C0D1D0: load and scale the current slot's relative point, and
 * select the colour used by its projection. */
void prepare_history_projection_point(HistoryProjectionWork *work);

/* Complete memory and drawing side of $C0D04C. The register bridge replays
 * caller-visible register results separately. */
uint16_t draw_history_projection(HistoryProjectionWork *work);

#endif
