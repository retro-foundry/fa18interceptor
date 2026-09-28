#include "selected_table_display_stage.h"

#include <assert.h>
#include <string.h>

typedef struct { uint16_t calls; } CallbackState;

static int line(void *context, int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                int16_t display_bound_y) {
    (void)context; (void)x0; (void)y0; (void)x1; (void)y1; (void)display_bound_y;
    return 0;
}

static int protected_line(void *context, int16_t x0, int16_t y0, int16_t x1,
                          int16_t y1, uint32_t scratch) {
    (void)context; (void)x0; (void)y0; (void)x1; (void)y1; (void)scratch;
    return 0;
}

static int blitter(void *context, const FA18ProjectionPairBlitterWrites *writes) {
    (void)context; (void)writes;
    return 0;
}

static int final(void *context, const FA18ProjectionPairFinalState *state) {
    (void)context; (void)state;
    return 0;
}

static int callback(void *context, FA18DisplayRecordSelectorOutput *selection) {
    CallbackState *state = context;
    ++state->calls;
    (void)selection;
    return 0;
}

int main(void) {
    int16_t records[8][8] = {
        {-1,193,172,222,172,222,162,193}, {162,0,179,319,179,122,89,134},
        {89,110,89,137,89,200,89,319}, {91,319,179,0,179,0,0,0},
        {-10632,-1,8700,0,0,0,0,0}, {0}, {-1,-1,11421,0,0,0,0,0}, {0}
    };
    int16_t workspace[8][2] = {
        {-1,-96}, {-96,2069}, {2069,318}, {96,-96},
        {-96,96}, {7392,1096}, {2216,-32}, {-32,6728}
    };
    const FA18DisplayRecordPipelineInput pipeline = {
        {{10240,-9216},{-11008,-9728},{-10240,9216},{9728,11008}}, -1,
        {{167,0,-8},{0,252,0},{6,0,127}}, 584, {0,0,0,0,0}
    };
    const FA18ProjectionPairSubmission submission = {
        179, 111, 106, 0x0004db30u, 0, 0, 0, 0, line, 0,
        protected_line, 0, blitter, final, 0
    };
    uint32_t saved_table_field = 0x12345678u;
    CallbackState lane = {0}, post = {0};
    FA18SelectedTableDisplayStageInput input = {
        &pipeline, records, workspace, &submission, &saved_table_field, 0x00c45bd8u,
        1, 0, callback, &lane, callback, &post
    };
    FA18SelectedTableDisplayStageResult result;
    const int16_t expected[] = {4,0,89,319,89,319,0,0,0,0,179,319};

    assert(fa18_run_selected_table_display_stage(&input, &result) == 0);
    assert(saved_table_field == 0x12345678u && lane.calls == 1 && post.calls == 1);
    assert(!result.output_cleared && !memcmp(result.output_words, expected, sizeof expected));
    input.mode_flag = 1;
    assert(fa18_run_selected_table_display_stage(&input, &result) == 0);
    assert(result.output_cleared && result.output_words[0] == 0);
    input.mode_flag = 0;
    input.post_selection_pass = 0;
    assert(fa18_run_selected_table_display_stage(&input, &result) == -1);
    assert(fa18_run_selected_table_display_stage(0, &result) == -1);
    return 0;
}
