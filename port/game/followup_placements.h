#ifndef FA18_GAME_FOLLOWUP_PLACEMENTS_H
#define FA18_GAME_FOLLOWUP_PLACEMENTS_H
#include "memory.h"

/* Complete C1CCBC. Synchronous semantic observations expose computed data,
 * not CPU state. Descriptor consumers remain required independent children. */
enum FollowupPlacementPhase {
    FOLLOWUP_SELECTED_SCAN, FOLLOWUP_SELECTED_METRICS, FOLLOWUP_SELECTED_POINT,
    FOLLOWUP_DISTANCE_BEGIN, FOLLOWUP_DISTANCE_END, FOLLOWUP_DISPATCH,
    FOLLOWUP_ALTERNATE_SCAN, FOLLOWUP_ALTERNATE_DESCRIPTOR,
    FOLLOWUP_ALTERNATE_POINT, FOLLOWUP_ALTERNATE_PACKED,
    FOLLOWUP_CACHE, FOLLOWUP_REFRESH_PHASE, FOLLOWUP_RESULT,
    FOLLOWUP_CLOCK, FOLLOWUP_RELATIVE_BEGIN, FOLLOWUP_RELATIVE_SCAN,
    FOLLOWUP_RELATIVE_POINT, FOLLOWUP_RELATIVE_STORED, FOLLOWUP_RELATIVE_ADVANCE
};
typedef struct {
    enum FollowupPlacementPhase phase;
    gaddr record, descriptor, routine, parameters;
    int32_t point[3], terms[3], value;
    uint16_t header, index, shift;
    int selected, transformed;
    int32_t prior_result;
} FollowupPlacementEvent;
typedef struct {
    int32_t (*consume)(void *context, const FollowupPlacementEvent *call);
    void (*observe)(void *context, const FollowupPlacementEvent *event);
    void *context;
} FollowupPlacementHooks;
void visit_followup_placements(const FollowupPlacementHooks *hooks);

/* C1D0A4/C1D0B6 share the same complete position body, with signed workspace
 * or control-record selection. Results are packed long coordinates. */
gaddr accumulate_selected_position(int workspace, uint16_t header,
                                   int32_t point[3]);
#endif
