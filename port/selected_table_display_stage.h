#ifndef FA18_SELECTED_TABLE_DISPLAY_STAGE_H
#define FA18_SELECTED_TABLE_DISPLAY_STAGE_H

#include "display_record_pipeline.h"
#include "selected_display_submission.h"

enum { FA18_SELECTED_TABLE_DISPLAY_OUTPUT_WORDS = 12 };

/* `$C2FEDE` saves `$C456E2`, substitutes 12(A2), and restores it after the
 * renderer work.  The source producers of that field and the alternate
 * `$C0D74A` pass remain outside this bounded stage. */
typedef int (*FA18SelectedTableDisplayCallback)(
    void *context, FA18DisplayRecordSelectorOutput *selection);

typedef struct {
    const FA18DisplayRecordPipelineInput *pipeline;
    int16_t (*records)[FA18_DISPLAY_RECORD_ITERATOR_WORDS_PER_RECORD];
    int16_t (*workspace)[2];
    const FA18ProjectionPairSubmission *submission;
    uint32_t *saved_table_field;
    uint32_t source_table_field;
    uint8_t selector;
    uint8_t mode_flag;
    FA18SelectedTableDisplayCallback selector_lane_submit;
    void *selector_lane_context;
    FA18SelectedTableDisplayCallback post_selection_pass;
    void *post_selection_context;
} FA18SelectedTableDisplayStageInput;

typedef struct {
    int16_t output_words[FA18_SELECTED_TABLE_DISPLAY_OUTPUT_WORDS];
    FA18DisplayRecordPipelineResult pipeline;
    FA18ProjectionPairBoundsRoute bounds_route;
    FA18ProjectionPairFinalizationRoute finalization_route;
    uint8_t output_cleared;
} FA18SelectedTableDisplayStageResult;

/* `$C2FEDE-$C2FF45`.  The callbacks are required only for the source paths
 * that call `$C30466` and `$C0D74A`; they retain their caller-owned state. */
int fa18_run_selected_table_display_stage(
    const FA18SelectedTableDisplayStageInput *input,
    FA18SelectedTableDisplayStageResult *result);

#endif
