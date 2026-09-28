#include "display_record_candidates.h"

#include <assert.h>
#include <stdio.h>

int main(void) {
    const int16_t input[FA18_DISPLAY_RECORD_CANDIDATE_COUNT][2] = {
        {10240, -9216}, {-11008, -9728}, {-10240, 9216}, {9728, 11008}
    };
    const int16_t matrix[3][3] = {
        {167, 0, -8}, {0, 252, 0}, {6, 0, 127}
    };
    const FA18DisplayRecordCandidate expected[FA18_DISPLAY_RECORD_CANDIDATE_COUNT] = {
        {6968, -1, -4332}, {-6877, -1, -5084},
        {-6968, -1, 4332}, {6002, -1, 5689}
    };
    FA18DisplayRecordCandidate output[FA18_DISPLAY_RECORD_CANDIDATE_COUNT];

    /* `build/run075_prepared_c0d752/`: the four live `$C0D720` pairs and
     * `$C45BD8` matrix written to `$C4B390 + 0x1a*n`. */
    assert(fa18_prepare_display_record_candidates(input, -1, matrix, output) == 0);
    for (uint16_t index = 0; index < FA18_DISPLAY_RECORD_CANDIDATE_COUNT;
         ++index) {
        assert(output[index].x == expected[index].x);
        assert(output[index].y == expected[index].y);
        assert(output[index].depth == expected[index].depth);
    }
    assert(fa18_prepare_display_record_candidates(NULL, -1, matrix, output) == -1);
    puts("display-record candidate contract passed");
    return 0;
}
