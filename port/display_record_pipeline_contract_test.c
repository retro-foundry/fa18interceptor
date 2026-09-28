#include "display_record_pipeline.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    int16_t records[8][8] = {
        {-1,193,172,222,172,222,162,193}, {162,0,179,319,179,122,89,134},
        {89,110,89,137,89,200,89,319}, {91,319,179,0,179,0,0,0},
        {-10632,-1,8700,0,0,0,0,0}, {0,0,0,0,0,0,0,0},
        {-1,-1,11421,0,0,0,0,0}, {0,0,0,0,0,0,0,0}
    };
    int16_t workspace[8][2] = {
        {-1,-96}, {-96,2069}, {2069,318}, {96,-96},
        {-96,96}, {7392,1096}, {2216,-32}, {-32,6728}
    };
    const FA18DisplayRecordPipelineInput input = {
        {{10240,-9216},{-11008,-9728},{-10240,9216},{9728,11008}}, -1,
        {{167,0,-8},{0,252,0},{6,0,127}}, 584, {0,0,0,0,0}
    };
    const int16_t expected[] = {4,0,89,319,89,319,0,0,0,0,179,319,179,122,89,0};
    FA18DisplayRecordPipelineResult result;

    /* The chained run075 `$C0D752/$C2E758/$C0D7E0` before/after windows. */
    assert(fa18_run_display_record_pipeline(&input, records, workspace, &result) == 0);
    for (uint16_t index = 0; index < sizeof expected / sizeof expected[0]; ++index)
        assert((&records[0][0])[index] == expected[index]);
    assert(result.selection.selection_flag == 1 && result.scratch[1] == 1 &&
           result.scratch[3] == 1 && result.scratch[5] == 5 && result.scratch[7] == 4);
    assert(fa18_run_display_record_pipeline(NULL, records, workspace, &result) == -1);
    puts("display-record pipeline contract passed");
    return 0;
}
