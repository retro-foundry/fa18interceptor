#ifndef FA18_GAME_CANDIDATE_LEVEL_WALK_H
#define FA18_GAME_CANDIDATE_LEVEL_WALK_H

#include "memory.h"

/* Source block $C27504-$C27666 inside $C26EBE. The plane walk that follows
 * is separate; this block is inactive until the parent is registered. */
typedef enum CandidateLevelRoute {
    CANDIDATE_LEVEL_TERMINAL,
    CANDIDATE_LEVEL_STOP_ZERO,
    CANDIDATE_LEVEL_PLANES
} CandidateLevelRoute;

typedef struct CandidateLevelWork {
    gaddr selected;
    gaddr volume_stream;
    gaddr bounds_stream;
    gaddr level_cursor;
    int16_t table_index;
    int16_t level_iteration;
    int32_t x, y, z;
    int32_t x_adjustment, z_adjustment;
    CandidateLevelRoute route;
} CandidateLevelWork;

void select_candidate_level(CandidateLevelWork *work);
/* $C2766A-$C278D0: returns the source's 0, $10 or $20 outcome. */
int walk_candidate_level_planes(CandidateLevelWork *work);

#endif
