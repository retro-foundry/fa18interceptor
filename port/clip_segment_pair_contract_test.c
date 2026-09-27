#include "clip_segment_pair.h"

#include <assert.h>

int main(void) {
    FA18ClipSegmentPairState state = {0};
    FA18ClipTuple output[2];
    uint16_t count = 0;
    uint8_t emitted;
    FA18ClipTuple first = {1,2,2};
    assert(fa18_clip_projected_segment_pair(&first, &state, output, 2, &count,
                                            &emitted) == 0 && emitted && count == 1 &&
           state.cache_initialized == 1 && state.first_tuple.x == 1 &&
           state.cached_tuple.y == 2 && state.completion_counter == 1 &&
           output[0].x == 1 && output[0].y == 2 && output[0].z == 2);
    state = (FA18ClipSegmentPairState){0};
    count = 0;
    FA18ClipTuple rejected = {0,2,2};
    assert(fa18_clip_projected_segment_pair(&rejected, &state, output, 2, &count,
                                            &emitted) == -2 && emitted && count == 1 &&
           state.completion_counter == 0);
    return 0;
}
