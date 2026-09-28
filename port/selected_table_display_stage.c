#include "selected_table_display_stage.h"

#include <string.h>

int fa18_run_selected_table_display_stage(
    const FA18SelectedTableDisplayStageInput *input,
    FA18SelectedTableDisplayStageResult *result) {
    uint32_t saved_table_field;
    int status;

    if (!input || !result || !input->pipeline || !input->records ||
        !input->workspace || !input->submission || !input->saved_table_field)
        return -1;
    saved_table_field = *input->saved_table_field;
    *input->saved_table_field = input->source_table_field;
    result->output_cleared = 0;

    status = fa18_run_display_record_pipeline(input->pipeline, input->records,
                                              input->workspace, &result->pipeline);
    if (status < 0) {
        *input->saved_table_field = saved_table_field;
        return -1;
    }
    if (status == 0) {
        status = fa18_submit_selected_display_record_list(
            &result->pipeline.selection, input->submission, &result->bounds_route,
            &result->finalization_route);
        if (status == 0 && input->selector) {
            if (!input->selector_lane_submit) status = -1;
            else status = input->selector_lane_submit(input->selector_lane_context,
                                                      &result->pipeline.selection);
        }
    }
    *input->saved_table_field = saved_table_field;
    if (status < 0) return -1;

    if (input->mode_flag) {
        result->output_words[0] = 0;
        result->output_cleared = 1;
        return 0;
    }
    if (!input->post_selection_pass) return -1;
    if (input->post_selection_pass(input->post_selection_context,
                                   &result->pipeline.selection) != 0) {
        result->output_words[0] = 0;
        result->output_cleared = 1;
        return 0;
    }
    memcpy(result->output_words, result->pipeline.selection.words,
           sizeof result->output_words);
    return 0;
}
