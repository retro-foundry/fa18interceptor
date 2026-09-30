#ifndef FA18_GAME_HISTORY_PROJECTION_H
#define FA18_GAME_HISTORY_PROJECTION_H

#include "memory.h"

/* Working state of the $C0D04C history-point projection loop. */
typedef struct HistoryProjectionWork {
    gaddr record, slot_address;
    int8_t remaining, slot_index;
    int16_t direction, shift;
    int32_t delta[3], absolute[3];
    uint16_t drawn;
} HistoryProjectionWork;

/* $C0D04C-$C0D10A: choose the first slot and traversal direction. Returns
 * zero when the source returns before entering its point loop. */
int begin_history_projection(HistoryProjectionWork *work);

/* $C0D110-$C0D1D0: load and scale the current slot's relative point, and
 * select the colour used by its projection. */
void prepare_history_projection_point(HistoryProjectionWork *work);

#endif
