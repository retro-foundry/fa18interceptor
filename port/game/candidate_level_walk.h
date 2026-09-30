#ifndef FA18_GAME_CANDIDATE_LEVEL_WALK_H
#define FA18_GAME_CANDIDATE_LEVEL_WALK_H

#include "memory.h"

/* Source block $C27504-$C27666 inside $C26EBE. The plane walk follows. */
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
    gaddr register_a0;
    int16_t table_index;
    int16_t level_iteration;
    int32_t x, y, z;
    int32_t x_adjustment, z_adjustment;
    int32_t plane_d3, plane_d4;
    gaddr plane_normals_end;
    int terminal_from_planes;
    CandidateLevelRoute route;
} CandidateLevelWork;

void select_candidate_level(CandidateLevelWork *work);
/* $C2766A-$C278D0: returns the source's 0, $10 or $20 outcome. */
int walk_candidate_level_planes(CandidateLevelWork *work);

#endif
