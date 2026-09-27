#ifndef FA18_RECORD_SCAN_CANDIDATE_PRELUDE_H
#define FA18_RECORD_SCAN_CANDIDATE_PRELUDE_H

#include <stdint.h>

#include "flagged_slot_scan.h"

typedef int (*FA18RecordScanCommandDispatch)(void *context, uint16_t first,
                                             uint16_t second);
typedef int (*FA18RecordScanIndexStage)(void *context, uint16_t index);

typedef struct {
    uint8_t table_end;
    uint8_t scan_counter;
    int16_t renderer_budget;
    uint16_t selected_state_word_3a;
    uint8_t state_c457c5;
    uint8_t flight_update_flag;
    uint8_t auxiliary_flag;
    uint8_t context_selection;
    uint8_t renderer_flags;
    uint8_t slot_c4588c;
    uint8_t slot_c4588d;
    uint8_t mode_c45843;
    uint8_t local_skip;
    uint8_t local_processed;
    uint8_t local_auxiliary;
    uint8_t local_table_high;
} FA18RecordScanCandidateState;

typedef struct {
    FA18RecordScanCommandDispatch command_dispatch;
    FA18RecordScanIndexStage indexed_stage;
    FA18RecordScanIndexStage slot_stage;
    void *context;
} FA18RecordScanCandidateOps;

typedef enum {
    FA18_RECORD_SCAN_CANDIDATE_ADVANCED,
    FA18_RECORD_SCAN_CANDIDATE_SLOT_STAGE
} FA18RecordScanCandidateRoute;

/* `$C1522E-$C153DB`: source-selected record prelude. The four child calls are
 * caller-owned, while every bounded slot/state mutation and loop route stays
 * native here. */
int fa18_run_record_scan_candidate_prelude(
    FA18FlaggedSlot *slot, uint16_t index, FA18RecordScanCandidateState *state,
    const FA18RecordScanCandidateOps *ops,
    FA18RecordScanCandidateRoute *route);

#endif
