#include "tuple_cache_stage.h"

#include <assert.h>

int main(void) {
    FA18TupleCacheStageState state = {0};
    FA18ClipTuple output[3];
    uint16_t count = 0;
    uint8_t emitted;
    FA18ClipTuple first = {10,2,10};
    assert(fa18_process_tuple_cache_stage(&first, &state, output, 3, &count,
                                          &emitted) == 0 && emitted && count == 1 &&
           state.cache_initialized == 1 && state.first_tuple.x == 10 &&
           state.cached_tuple.z == 10 && state.completion_counter == 1 &&
           output[0].x == 10 && output[0].y == 2 && output[0].z == 10);

    state = (FA18TupleCacheStageState){0};
    count = 0;
    first = (FA18ClipTuple){10,2,-10};
    assert(fa18_process_tuple_cache_stage(&first, &state, output, 3, &count,
                                          &emitted) == 0 && !emitted && !count);
    FA18ClipTuple crossing = {-10,10,0};
    assert(fa18_process_tuple_cache_stage(&crossing, &state, output, 3, &count,
                                          &emitted) == 0);
    assert(!emitted && !count);
    assert(crossing.x == -10 && crossing.y == 10 && crossing.z == 0);
    assert(state.cached_tuple.x == -10 && state.cached_tuple.y == 10 &&
           state.cached_tuple.z == 0 && state.completion_counter == 2);

    state = (FA18TupleCacheStageState){0};
    count = 0;
    FA18ClipTuple rejected = {0,2,0};
    assert(fa18_process_tuple_cache_stage(&rejected, &state, output, 3, &count,
                                          &emitted) == -3 && emitted && count == 1 &&
           state.completion_counter == 0);
    return 0;
}
