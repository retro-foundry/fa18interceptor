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
    {
        /* First real `$C212B0` pair at frame 602: A2 `$C3985A` reads
         * selector 10, then `$C48390+342`/`+348`. */
        uint8_t vertex_bytes[354] = {0};
        const uint8_t stream_bytes[] = {0,10, 1,86, (uint8_t)0x81,92};
        FA18OffsetPairSegmentStreamInput stream_input;
        vertex_bytes[342] = 0xff; vertex_bytes[343] = 0x33;
        vertex_bytes[344] = 0x02; vertex_bytes[345] = 0x3a;
        vertex_bytes[346] = 0x09; vertex_bytes[347] = 0x1e;
        vertex_bytes[348] = 0xff; vertex_bytes[349] = 0xc5;
        vertex_bytes[350] = 0x02; vertex_bytes[351] = 0x3a;
        vertex_bytes[352] = 0x09; vertex_bytes[353] = 0x1e;
        stream_input = (FA18OffsetPairSegmentStreamInput){
            vertex_bytes, sizeof vertex_bytes, stream_bytes, sizeof stream_bytes,
            0xc3985a, 0xc3985a, capture_prepare, &capture
        };
        capture.calls = 0;
        assert(fa18_submit_offset_pair_segment_stream(&stream_input, &result) == 0);
        assert(result.selector_word == 10 && result.submitted_pairs == 1 &&
               result.next_stream_word_index == 3 && capture.calls == 1);
        assert(capture.endpoints[0].x == -205 && capture.endpoints[1].x == -59);
    }
    return 0;
}
