#include "negated_tuple_emit.h"

int fa18_emit_negated_tuple_direct(FA18ClipTuple tuple, FA18ClipTuple *cache,
                                   uint8_t *cache_seen, uint8_t *emitted_count,
                                   FA18ClipTuple *output,
                                   size_t capacity, uint16_t *count,
                                   uint8_t *emitted_flag) {
    if (!cache || !cache_seen || !emitted_count || !output || !count || !emitted_flag) return -1;
    *cache = tuple;
    if (!*cache_seen) ++*cache_seen;
    *emitted_flag = 0;
    const int16_t negated_x = (int16_t)(uint16_t)(0u - (uint16_t)tuple.x);
    if (tuple.z > negated_x) return 0;
    if (*count >= capacity) return -1;
    output[*count] = tuple;
    ++*count;
    ++*emitted_count;
    *emitted_flag = 1;
    return 0;
}
