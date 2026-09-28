#ifndef FA18_OFFSET_PAIR_SEGMENT_SUBMISSION_H
#define FA18_OFFSET_PAIR_SEGMENT_SUBMISSION_H

#include <stddef.h>
#include <stdint.h>

#include "projection.h"

typedef int (*FA18ProjectedSegmentPreparer)(void *context,
                                            const FA18ViewVertex endpoints[2]);

typedef struct {
    const int16_t *vertex_words; /* `$C48390`, big-endian decoded words. */
    size_t vertex_word_count;
    const int16_t *stream_words; /* A2 at `$C212C2`. */
    size_t stream_word_count;
    uint16_t stream_word_index;
    FA18ProjectedSegmentPreparer prepare_segment; /* `$C2EE4A`. */
    void *prepare_context;
} FA18OffsetPairSegmentSubmissionInput;

typedef struct {
    uint16_t next_stream_word_index;
    int16_t selector_word; /* `$C45954` */
    uint16_t submitted_pairs;
    uint16_t skipped_negative_depth_pairs;
    uint16_t result_flags; /* OR of each `$C2EE4A` result. */
} FA18OffsetPairSegmentSubmissionResult;

/* Source-addressed counterpart used when `$C1F942` supplies A2 directly.
 * Both byte ranges remain big-endian original-memory views. */
typedef struct {
    const uint8_t *vertex_bytes; /* `$C48390`. */
    size_t vertex_byte_count;
    const uint8_t *stream_bytes; /* A2 record stream. */
    size_t stream_byte_count;
    uint32_t stream_base;
    uint32_t stream_cursor;
    FA18ProjectedSegmentPreparer prepare_segment;
    void *prepare_context;
} FA18OffsetPairSegmentStreamInput;

/* `$C212B0-$C2131B`: resolve source offset pairs through the mutable
 * transformed-vertex table, preserve the negative-second-offset terminator,
 * and invoke the caller-owned exact `$C2EE4A` port for eligible pairs. */
int fa18_submit_offset_pair_segments(
    const FA18OffsetPairSegmentSubmissionInput *input,
    FA18OffsetPairSegmentSubmissionResult *result);

/* `$C212B0-$C2131B`, source-addressed form: decode the A2 selector/pair
 * record without reconstructing host-endian vertex or stream arrays. */
int fa18_submit_offset_pair_segment_stream(
    const FA18OffsetPairSegmentStreamInput *input,
    FA18OffsetPairSegmentSubmissionResult *result);

#endif
