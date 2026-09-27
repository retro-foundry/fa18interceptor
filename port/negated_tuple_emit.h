#ifndef FA18_NEGATED_TUPLE_EMIT_H
#define FA18_NEGATED_TUPLE_EMIT_H

#include <stddef.h>
#include <stdint.h>

typedef struct { int16_t x, y, z; } FA18ClipTuple;

/* Caller-owned fields used by `$C24996-$C24A93`: the cache at `+$30`, the
 * first input saved at `+$36`, and the byte counters at `+3`/`+7`.  Their
 * higher-level ownership remains unresolved. */
typedef struct {
    FA18ClipTuple cached_tuple;
    FA18ClipTuple first_tuple;
    uint8_t cache_initialized;
    uint8_t output_counter;
} FA18NegatedTupleState;

/* Direct (non-interpolating) output path in `$C249E6-$C24A7C`: cache one
 * tuple and append it only when the source's negated-X plane comparison holds. */
int fa18_emit_negated_tuple_direct(FA18ClipTuple tuple, FA18ClipTuple *cache,
                                   uint8_t *cache_seen, uint8_t *emitted_count,
                                   FA18ClipTuple *output,
                                   size_t capacity, uint16_t *count,
                                   uint8_t *emitted_flag);

/* `$C24996-$C24A7E`: update the cached tuple, insert the source's rounded
 * signed crossing when the cached and current tuples straddle its negated-X
 * comparison, then conditionally append the current tuple. `tuple` is
 * overwritten only by the source's interpolation route. A zero interpolation
 * denominator follows the original error-helper branch and returns -2. */
int fa18_process_negated_tuple(FA18ClipTuple *tuple,
                               FA18NegatedTupleState *state,
                               FA18ClipTuple *output, size_t capacity,
                               uint16_t *count, uint8_t *emitted_flag);

#endif
