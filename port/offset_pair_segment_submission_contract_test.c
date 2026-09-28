#include <assert.h>

#include "offset_pair_segment_submission.h"

typedef struct {
    unsigned calls;
    FA18ViewVertex endpoints[2];
} Capture;

static int capture_prepare(void *context, const FA18ViewVertex endpoints[2]) {
    Capture *capture = context;
    ++capture->calls;
    capture->endpoints[0] = endpoints[0];
    capture->endpoints[1] = endpoints[1];
    return 1;
}

int main(void) {
    const int16_t vertices[] = {
        1, 2, 3,  4, 5, 6,  7, 8, -9,  10, 11, 12
    };
    /* Selector, an ordinary pair, then the negative-second terminal pair. */
    const int16_t stream[] = {0x21, 0, 0, 12, (int16_t)0x800c};
    Capture capture = {0};
    FA18OffsetPairSegmentSubmissionResult result;
    const FA18OffsetPairSegmentSubmissionInput input = {
        vertices, sizeof vertices / sizeof *vertices,
        stream, sizeof stream / sizeof *stream, 0,
        capture_prepare, &capture
    };

    assert(fa18_submit_offset_pair_segments(&input, &result) == 0);
    assert(result.selector_word == 0x21 && result.next_stream_word_index == 5);
    assert(result.submitted_pairs == 1 && result.skipped_negative_depth_pairs == 1);
    assert(result.result_flags == 1 && capture.calls == 1);
    assert(capture.endpoints[0].x == 1 && capture.endpoints[1].x == 1);
    return 0;
}
