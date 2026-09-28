#include "prepared_record_dispatch.h"

#include <assert.h>

int main(void) {
    /* Frame-601 `$C2005C`: offsets 0x2a, 0x24, terminal 0x8072; descriptor
     * 0x000c.  `$C1FB82` accepts, then `$C200C6` returns -1 (bit 14 clear). */
    const int16_t vertices[0x3c] = {
        [0x12] = 179, [0x13] = 113, [0x14] = 7,
        [0x15] = 206, [0x16] = 0, [0x17] = 222,
        [0x39] = 35, [0x3a] = 97, [0x3b] = 108
    };
    const int16_t stream[] = { 0x2a, 0x24, (int16_t)0x8072, 0x000c };
    int16_t workspace[12] = {0};
    FA18PreparedRecordDispatchResult result;
    FA18PreparedRecordDispatchInput input = {
        vertices, sizeof vertices / sizeof *vertices, stream,
        sizeof stream / sizeof *stream, 0,
        {{0}, 0, 0, 0, 0, {0, 0, 0}, 0, 0}, workspace,
        sizeof workspace / sizeof *workspace
    };
    assert(fa18_dispatch_prepared_record_triples(&input, &result) == 0);
    assert(result.route == FA18_PREPARED_RECORD_NEGATIVE_STATUS);
    assert(result.triple_count == 3 && result.next_stream_word_index == 4);
    assert(result.component_test_count_delta == 1 &&
           result.component_reject_count_delta == 0);
    assert(workspace[0] == 206 && workspace[5] == 7 && workspace[8] == 108);
    return 0;
}
