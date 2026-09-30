#ifndef FA18_GAME_CANDIDATE_RECORD_SCAN_H
#define FA18_GAME_CANDIDATE_RECORD_SCAN_H

#include "memory.h"

/* The bounded record scan at $C26EBE-$C270AA. The rest of $C26EBE is
 * still translated, so this source block is not registered on its own. */
typedef enum CandidateScanRoute {
    CANDIDATE_SCAN_DONE,
    CANDIDATE_SCAN_CLASS_20,
    CANDIDATE_SCAN_SIDE_RESULT
} CandidateScanRoute;

typedef struct CandidateScanWork {
    gaddr selected_record;
    gaddr candidate_record;
    int16_t candidate_offset;
    uint8_t candidate_class;
    CandidateScanRoute route;
} CandidateScanWork;

void scan_candidate_record(CandidateScanWork *work,
                           int32_t relative_x, int32_t relative_y,
                           int32_t relative_z);

#endif
