#include "flagged_record_component_bound.h"
#include <assert.h>

int main(void) {
    uint8_t record[16] = {0};
    FA18FlaggedRecordComponentBoundResult result;
    FA18FlaggedRecordComponentBoundInput input = {
        record, sizeof record, 0, 0x0400, 0, 0, 0, 0, 0, -132
    };
    record[12] = 0; record[13] = 112; /* frame-601 selector-one component. */
    assert(fa18_test_flagged_record_component_bound(&input, &result) == 0);
    assert(result.zero == 1 && result.d7 == 132u);
    input.input_d7 = 0x1400;
    assert(fa18_test_flagged_record_component_bound(&input, &result) == 0);
    assert(result.zero == 0 && result.d7 == 132u);
    return 0;
}
