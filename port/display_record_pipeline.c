#include "display_record_pipeline.h"

#include <string.h>

int fa18_run_display_record_pipeline(
    const FA18DisplayRecordPipelineInput *input, int16_t records[8][8],
    int16_t workspace[8][2], FA18DisplayRecordPipelineResult *result) {
    FA18DisplayRecordCandidate candidates[FA18_DISPLAY_RECORD_CANDIDATE_COUNT];
    int16_t *record_words;

    if (!input || !records || !workspace || !result ||
        fa18_prepare_display_record_candidates(input->input_pairs,
                                               input->source_component,
                                               input->matrix, candidates) != 0)
        return -1;
    record_words = &records[0][0];
    for (uint16_t index = 0; index < FA18_DISPLAY_RECORD_CANDIDATE_COUNT;
         ++index) {
        /* Each pass writes three words, then `$C0D7CC` advances A3 by `$1a`:
         * the next source record therefore starts 16 words later. */
        uint16_t offset = (uint16_t)(index * 16u);
        record_words[offset] = candidates[index].x;
        record_words[offset + 1u] = candidates[index].y;
        record_words[offset + 2u] = candidates[index].depth;
    }
    memset(result->scratch, 0, sizeof result->scratch);
    if (fa18_iterate_display_records(records, workspace, result->scratch,
                                     input->adjustment_gate_5aca) != 0)
        return -1;
    memcpy(result->selection.words, record_words, sizeof result->selection.words);
    if (fa18_select_display_record_pairs(workspace, result->scratch,
                                         &input->selector, &result->selection) < 0)
        return -1;
    memcpy(record_words, result->selection.words, sizeof result->selection.words);
    return result->selection.selection_flag ? 0 : 1;
}
