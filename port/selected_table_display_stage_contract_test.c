#include "selected_table_display_stage.h"
#include "five_plane_chip_binding.h"
#include "flight_renderer_page.h"
#include "projection_page_blitter.h"

#include <assert.h>
#include <string.h>

typedef struct { uint16_t calls; } CallbackState;

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
    /* The return-bounded run075 `$C2FEDE` witness reaches `$C30306` with
     * the selected page's first lower-plane base, bound 144, vertical 89,
     * and horizontal 0.  The native binding uses a nonzero caller-owned
     * page identity to retain the source pointer arithmetic. */
    uint8_t chip[0x10000 + FA18_COPPER_PAGE_BYTES * FA18_COPPER_PAGE_PLANES] = {0};
    const uint32_t planes[FA18_COPPER_PAGE_PLANES] = {
        0x10000, 0x11f40, 0x13e80, 0x15dc0, 0x17d00
    };
    FA18FivePlanePage page;
    FA18FivePlaneChipBinding binding;
    FA18FlightRendererPage renderer;
    FA18ProjectionPageBlitter page_blitter;
    FA18BlitOperation operation = { .bltafwm = 0xffff, .bltalwm = 0xffff };
    FA18RendererLaneStage lanes = {
        {0x15dc0, 0x13e80, 0x11f40, 0x10000},
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    FA18ProjectionPairSubmission submission;
    uint32_t saved_table_field = 0x12345678u;
    CallbackState lane = {0}, post = {0};
    FA18SelectedTableDisplayStageInput input;
    FA18SelectedTableDisplayStageResult result;
    const int16_t expected[] = {4,0,89,319,89,319,0,0,0,0,179,319};

    fa18_five_plane_page_init(&page);
    assert(fa18_five_plane_chip_binding_init(&binding, chip, sizeof chip, planes) == 0);
    assert(fa18_flight_renderer_page_init(&renderer, &page,
                                          &(FA18PlanarPixelState){0, 0, 0, 0},
                                          &(FA18LineStyle){0, 0, 0, 0},
                                          144, 89, 0, 0x10000, 0, 0) == 0);
    assert(fa18_projection_page_blitter_init(&page_blitter, &page, &binding,
                                              &operation, &lanes) == 0);
    assert(fa18_flight_renderer_page_bind_projection_page_blitter(
               &renderer, &page_blitter) == 0);
    submission = renderer.triangle_submission;
    input = (FA18SelectedTableDisplayStageInput){
        &pipeline, records, workspace, &submission, &saved_table_field, 0x10000,
        0, 0, callback, &lane, callback, &post
    };

    assert(fa18_run_selected_table_display_stage(&input, &result) == 0);
    assert(saved_table_field == 0x12345678u && lane.calls == 0 && post.calls == 1);
    assert(!result.output_cleared && !memcmp(result.output_words, expected, sizeof expected));
    assert(page_blitter.line_submissions == 2);
    size_t nonzero = 0;
    for (size_t index = 0; index < FA18_COPPER_PAGE_BYTES; ++index)
        nonzero += page.planes[0][index] != 0;
    assert(page.planes[0][0x0028] == 0x80 && nonzero &&
           page.planes[1][0x0028] == 0 && page.planes[2][0x0028] == 0 &&
           page.planes[3][0x0028] == 0);
    input.mode_flag = 1;
    assert(fa18_run_selected_table_display_stage(&input, &result) == 0);
    assert(result.output_cleared && result.output_words[0] == 0);
    input.mode_flag = 0;
    input.post_selection_pass = 0;
    assert(fa18_run_selected_table_display_stage(&input, &result) == -1);
    assert(fa18_run_selected_table_display_stage(0, &result) == -1);
    return 0;
}
