#ifndef FA18_GAME_CANDIDATE_RECORD_UPDATE_H
#define FA18_GAME_CANDIDATE_RECORD_UPDATE_H

#include "candidate_level_walk.h"
#include "candidate_record_scan.h"

typedef enum CandidateUpdatePath {
    CANDIDATE_UPDATE_EARLY,
    CANDIDATE_UPDATE_SIDE,
    CANDIDATE_UPDATE_FACE,
    CANDIDATE_UPDATE_TERMINAL,
    CANDIDATE_UPDATE_LEVEL
} CandidateUpdatePath;

typedef struct CandidateUpdateWork {
    CandidateScanWork scan;
    CandidateProbe probe;
    CandidateLevelWork level;
    CandidateUpdatePath path;
    int result;
    int pass;
    int had_probe;
    gaddr final_geometry_a4;
} CandidateUpdateWork;

/* Complete memory-side path of $C26EBE; caller-visible register glue is
 * separate. The result is the source's D0 value: 0, $10, $20 or $40. */
int update_candidate_record(CandidateUpdateWork *work,
                            int32_t relative_x, int32_t relative_y,
                            int32_t relative_z);

#endif
