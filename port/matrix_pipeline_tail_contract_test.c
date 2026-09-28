#include "matrix_pipeline_tail.h"
#include "run075_trig_asset.h"
#include "two_angle_matrix.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    uint8_t hunk63[0xae8 + sizeof fa18_run075_trig_bytes] = {0};
    FA18HunkSegment segments[64] = {0};
    FA18Hunks hunks = {segments, 64};
    FA18FlightTrigTable trig = {0};
    FA18MatrixPipelineTailInput input = {
        720, 28600, {168, 252, 128}, {0, 0, 0}
    };
    FA18MatrixPipelineTailState state = {0};

    memcpy(hunk63 + 0xae8, fa18_run075_trig_bytes, sizeof fa18_run075_trig_bytes);
    segments[63].data = hunk63;
    segments[63].size = sizeof hunk63;
    assert(fa18_load_two_angle_trig_table(&hunks, &trig) == 0);

    /* `$C2DAB0` from the bounded no-key trace at replay frame 608. */
    assert(fa18_run_matrix_pipeline_tail(&trig, &input, &state) == 0);
    const int16_t projection[3][3] = {
        {167, 0, -8}, {1, 248, 39}, {6, -21, 126}
    };
    const int16_t single_angle[3][3] = {
        {255, 0, -12}, {0, 256, 0}, {12, 0, 255}
    };
    assert(memcmp(state.projection_matrix, projection, sizeof projection) == 0);
    assert(memcmp(state.single_angle_matrix, single_angle, sizeof single_angle) == 0);

    input.record_auxiliary[0] = -1;
    input.record_auxiliary[1] = 0x1234;
    input.record_auxiliary[2] = (int16_t)0x8000;
    assert(fa18_run_matrix_pipeline_tail(&trig, &input, &state) == 0);
    assert(memcmp(state.auxiliary, input.record_auxiliary, sizeof state.auxiliary) == 0);
    assert(fa18_run_matrix_pipeline_tail(NULL, &input, &state) == -1);
    assert(fa18_run_matrix_pipeline_tail(&trig, NULL, &state) == -1);
    assert(fa18_run_matrix_pipeline_tail(&trig, &input, NULL) == -1);
    puts("matrix pipeline tail contract passed");
    return 0;
}
