#include "display_record_candidates.h"

#include <assert.h>
#include <stdio.h>

int main(void) {
    uint8_t source[FA18_DISPLAY_RECORD_CANDIDATE_INPUT_BYTES] = {
        0x28,0x00,0xdc,0x00,0xd5,0x00,0xda,0x00,
        0xd8,0x00,0x24,0x00,0x26,0x00,0x2b,0x00
    };
    FA18HunkSegment segments[FA18_DISPLAY_RECORD_CANDIDATE_HUNK + 1u] = {0};
    FA18Hunks hunks = {segments, FA18_DISPLAY_RECORD_CANDIDATE_HUNK + 1u};
    int16_t input[FA18_DISPLAY_RECORD_CANDIDATE_COUNT][2];
    const int16_t matrix[3][3] = {
        {167, 0, -8}, {0, 252, 0}, {6, 0, 127}
    };
    const FA18DisplayRecordCandidate expected[FA18_DISPLAY_RECORD_CANDIDATE_COUNT] = {
        {6968, -1, -4332}, {-6877, -1, -5084},
        {-6968, -1, 4332}, {6002, -1, 5689}
    };
    FA18DisplayRecordCandidate output[FA18_DISPLAY_RECORD_CANDIDATE_COUNT];

    segments[FA18_DISPLAY_RECORD_CANDIDATE_HUNK].data = source;
    segments[FA18_DISPLAY_RECORD_CANDIDATE_HUNK].size = sizeof source;
    assert(fa18_load_display_record_candidate_input_pairs(&hunks, input) == 0);
    assert(input[0][0] == 10240 && input[0][1] == -9216 &&
           input[3][0] == 9728 && input[3][1] == 11008);
    /* `build/run075_prepared_c0d752/`: these source pairs and the live
     * `$C45BD8` matrix write the candidates at `$C4B390 + 0x1a*n`. */
    assert(fa18_prepare_display_record_candidates(input, -1, matrix, output) == 0);
    for (uint16_t index = 0; index < FA18_DISPLAY_RECORD_CANDIDATE_COUNT;
         ++index) {
        assert(output[index].x == expected[index].x);
        assert(output[index].y == expected[index].y);
        assert(output[index].depth == expected[index].depth);
    }
    hunks.count = FA18_DISPLAY_RECORD_CANDIDATE_HUNK;
    assert(fa18_load_display_record_candidate_input_pairs(&hunks, input) == -1);
    assert(fa18_prepare_display_record_candidates(NULL, -1, matrix, output) == -1);
    puts("display-record candidate contract passed");
    return 0;
}
