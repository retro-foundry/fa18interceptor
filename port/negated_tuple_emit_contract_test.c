#include "negated_tuple_emit.h"
#include <assert.h>
int main(void) {
    FA18ClipTuple cache, output[2]; uint8_t cache_seen = 0, emitted_count = 0, emitted; uint16_t count = 0;
    assert(fa18_emit_negated_tuple_direct((FA18ClipTuple){10,2,-10}, &cache, &cache_seen, &emitted_count,
           output, 2, &count, &emitted) == 0 && emitted && count == 1 &&
           output[0].x == 10 && cache_seen == 1 && emitted_count == 1);
    assert(fa18_emit_negated_tuple_direct((FA18ClipTuple){10,2,-9}, &cache, &cache_seen, &emitted_count,
           output, 2, &count, &emitted) == 0 && !emitted && count == 1 && cache_seen == 1 && emitted_count == 1);
    FA18NegatedTupleState state = {0};
    count = 0;
    assert(fa18_process_negated_tuple(&(FA18ClipTuple){10,2,-10}, &state, output,
                                      2, &count, &emitted) == 0 && emitted &&
           count == 1 && state.cache_initialized == 1 &&
           state.first_tuple.x == 10 && state.cached_tuple.z == -10 &&
           state.output_counter == 1);
    FA18ClipTuple crossing = {-10,10,0};
    assert(fa18_process_negated_tuple(&crossing, &state, output, 2, &count,
                                      &emitted) == 0 && emitted && count == 2 &&
           output[1].x == 10 && output[1].y == 2 && output[1].z == -10 &&
           state.cached_tuple.x == -10 && state.cached_tuple.y == 10 &&
           state.cached_tuple.z == 0 && state.output_counter == 2 &&
           crossing.x == 10 && crossing.y == 2 && crossing.z == -10);

    state = (FA18NegatedTupleState){0};
    count = 0;
    FA18ClipTuple first = {10,2,-20};
    assert(fa18_process_negated_tuple(&first, &state, output, 2, &count,
                                      &emitted) == 0 && !emitted && !count);
    crossing = (FA18ClipTuple){-10,10,10};
    assert(fa18_process_negated_tuple(&crossing, &state, output, 2, &count,
                                      &emitted) == 0 && emitted && count == 2 &&
           output[0].x == -10 && output[0].y == 10 && output[0].z == 10 &&
           output[1].x == -10 && output[1].y == 10 && output[1].z == 10 &&
           state.output_counter == 2);
    return 0;
}
