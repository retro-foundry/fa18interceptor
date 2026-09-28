#ifndef FA18_DISPLAY_RECORD_PIPELINE_H
#define FA18_DISPLAY_RECORD_PIPELINE_H

#include "display_record_candidates.h"
#include "display_record_iterator.h"
#include "display_record_selector.h"

typedef struct {
    int16_t input_pairs[FA18_DISPLAY_RECORD_CANDIDATE_COUNT][2];
    int16_t source_component;
    int16_t matrix[3][3];
    int16_t adjustment_gate_5aca;
    FA18DisplayRecordSelectorInput selector;
} FA18DisplayRecordPipelineInput;

typedef struct {
    int16_t scratch[8];
    FA18DisplayRecordSelectorOutput selection;
} FA18DisplayRecordPipelineResult;

/* `$C0D752 -> $C2E758 -> $C0D7E0`: caller-owned workspace starts with the
 * source's remaining record/pair state; this routine performs only the proved
 * mutations and returns the selected `$C4B390` list. `adjustment_gate_5aca`
 * is the signed source word at `$C45ACA`. */
int fa18_run_display_record_pipeline(
    const FA18DisplayRecordPipelineInput *input,
    int16_t records[FA18_DISPLAY_RECORD_ITERATOR_RECORD_COUNT]
                   [FA18_DISPLAY_RECORD_ITERATOR_WORDS_PER_RECORD],
    int16_t workspace[FA18_DISPLAY_RECORD_ITERATOR_RECORD_COUNT][2],
    FA18DisplayRecordPipelineResult *result);

#endif
