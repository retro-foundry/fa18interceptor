#include "offset_pair_segment_submission.h"

static int copy_vertex(const FA18OffsetPairSegmentSubmissionInput *input,
                       uint16_t offset, FA18ViewVertex *vertex) {
    const size_t word = (size_t)offset / 2u;
    if ((offset & 1u) || word > input->vertex_word_count ||
        input->vertex_word_count - word < 3u)
        return -1;
    *vertex = (FA18ViewVertex){input->vertex_words[word],
                               input->vertex_words[word + 1u],
                               input->vertex_words[word + 2u]};
    return 0;
}

int fa18_submit_offset_pair_segments(
    const FA18OffsetPairSegmentSubmissionInput *input,
    FA18OffsetPairSegmentSubmissionResult *result) {
    uint16_t cursor;

    if (!input || !result || !input->vertex_words || !input->stream_words ||
        !input->prepare_segment || input->stream_word_index >= input->stream_word_count)
        return -1;
    cursor = input->stream_word_index;
    *result = (FA18OffsetPairSegmentSubmissionResult){0};
    result->selector_word = input->stream_words[cursor++];
    for (;;) {
        int16_t first, second;
        FA18ViewVertex endpoints[2];
        int prepared;

        if ((size_t)cursor + 2u > input->stream_word_count) return -1;
        first = input->stream_words[cursor++];
        second = input->stream_words[cursor++];
        if (second < 0) second = (int16_t)((uint16_t)second & UINT16_C(0x7fff));
        if (first < 0 || copy_vertex(input, (uint16_t)first, &endpoints[0]) != 0 ||
            copy_vertex(input, (uint16_t)second, &endpoints[1]) != 0)
            return -1;

        /* `$C212FC-$C21302`: AND the two depth words before the `$C2EE4A`
         * call; a negative result skips that pair without changing flags. */
        if ((int16_t)((uint16_t)endpoints[0].depth & (uint16_t)endpoints[1].depth) < 0) {
            ++result->skipped_negative_depth_pairs;
        } else {
            prepared = input->prepare_segment(input->prepare_context, endpoints);
            if (prepared < 0) return -1;
            result->result_flags |= (uint16_t)prepared;
            ++result->submitted_pairs;
        }
        if (input->stream_words[cursor - 1u] < 0) break;
    }
    result->next_stream_word_index = cursor;
    return 0;
}
