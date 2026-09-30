/* Register bridge for candidate-record stage $C26EBE. */
#include "glue.h"
#include "ports_glue.h"

#include "candidate_record_update.h"
#include "globals.h"

int glue_C26EBE(void) {
    CandidateUpdateWork work = {0};
    int result = update_candidate_record(&work, (int32_t)D(2),
                                         (int32_t)D(3), (int32_t)D(4));
    if (work.path == CANDIDATE_UPDATE_EARLY ||
        work.path == CANDIDATE_UPDATE_TERMINAL) A(0) = CONTROL_RECORDS;
    if (work.path == CANDIDATE_UPDATE_TERMINAL ||
        work.path == CANDIDATE_UPDATE_LEVEL) A(3) = work.scan.selected_record;
    D(0) = (uint32_t)result;
    flags_logic_l(D(0));
    return glue_return();
}
