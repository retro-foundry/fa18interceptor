#ifndef FA18_CLIP_SEGMENT_PAIR_H
#define FA18_CLIP_SEGMENT_PAIR_H

#include "tuple_cache_stage.h"

/* `$C247C0` caller-owned second boundary cache (`A2+$10/+16`) and the
 * corresponding `A4+$1/+5` byte counters. */
typedef struct {
    FA18ClipTuple cached_tuple;
    FA18ClipTuple first_tuple;
    uint8_t cache_initialized;
    uint8_t completion_counter;
    FA18TupleCacheStageState tuple_cache;
} FA18ClipSegmentPairState;

/* `$C247C0-$C248B1`: process a projected tuple at the second clipping
 * boundary and submit accepted/interpolated tuples to `$C248B2`.
 * Returns -2 for the source's code-3 degenerate intersection branch. */
int fa18_clip_projected_segment_pair(FA18ClipTuple *tuple,
                                     FA18ClipSegmentPairState *state,
                                     FA18ClipTuple *output, size_t capacity,
                                     uint16_t *count, uint8_t *emitted_flag);

#endif
