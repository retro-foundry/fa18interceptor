#include "negated_tuple_emit.h"
#include <assert.h>
int main(void) {
    FA18ClipTuple cache, output[2]; uint8_t cache_seen = 0, emitted_count = 0, emitted; uint16_t count = 0;
    assert(fa18_emit_negated_tuple_direct((FA18ClipTuple){10,2,-10}, &cache, &cache_seen, &emitted_count,
           output, 2, &count, &emitted) == 0 && emitted && count == 1 &&
           output[0].x == 10 && cache_seen == 1 && emitted_count == 1);
    assert(fa18_emit_negated_tuple_direct((FA18ClipTuple){10,2,-9}, &cache, &cache_seen, &emitted_count,
           output, 2, &count, &emitted) == 0 && !emitted && count == 1 && cache_seen == 1 && emitted_count == 1);
    return 0;
}
