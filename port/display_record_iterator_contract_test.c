#include "display_record_iterator.h"

#include <assert.h>
#include <stdio.h>

int main(void) {
    int16_t records[8][8] = {
        {6968, -1, -4332, 0, 0, 0, 0, 193}, {162, 0, 179, 0, 0, 0, 0, 134},
        {-6877, -1, -5084, 0, 0, 0, 0, 319}, {91, 319, 179, 0, 0, 0, 0, 0},
        {-6968, -1, 4332, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0},
        {6002, -1, 5689, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}
    };
    int16_t workspace[8][2] = {
        {-1, -96}, {-96, 2069}, {2069, 318}, {96, -96},
        {-96, 96}, {7392, 1096}, {2216, -32}, {-32, 6728}
    };
    int16_t scratch[8] = {0};
    const int16_t expected_workspace[8][2] = {
        {0, 0}, {0, 0}, {0, 0}, {0, 0},
        {0, 90}, {319, 90}, {0, 0}, {0, 0}
    };
    const int16_t expected_scratch[8] = {0, 1, 0, 1, 0, 5, 0, 4};

    /* Return-bounded `build/run075_prepared_c2e758/` before/after windows. */
    assert(fa18_iterate_display_records(records, workspace, scratch, 584) == 0);
    for (uint16_t index = 0; index < 8; ++index) {
        if (workspace[index][0] != expected_workspace[index][0] ||
            workspace[index][1] != expected_workspace[index][1]) {
            fprintf(stderr, "workspace %u: %d,%d\n", index,
                    workspace[index][0], workspace[index][1]);
            return 1;
        }
        assert(scratch[index] == expected_scratch[index]);
    }
    assert(records[0][7] == 0);
    assert(fa18_iterate_display_records(NULL, workspace, scratch, 0) == -1);
    puts("display-record iterator contract passed");
    return 0;
}
