#ifndef FA18_TUPLE_CACHE_STAGE_H
#define FA18_TUPLE_CACHE_STAGE_H

#include "negated_tuple_emit.h"

/* Caller-owned tuple/cache fields consumed by `$C248B2-$C24981`. The two
 * nested cache lanes mirror `A2+$20/+26` and `A2+$30/+36`; their scene
 * ownership remains unresolved. */
typedef struct {
    FA18ClipTuple cached_tuple;
    FA18ClipTuple first_tuple;
    uint8_t cache_initialized;
    uint8_t completion_counter;
    FA18NegatedTupleState negated;
} FA18TupleCacheStageState;

/* `$C248B2-$C24981`: update the first tuple-cache lane, insert its rounded
 * signed crossing when the tuple order changes, and submit each accepted
 * tuple to `$C24996`. The original's code-4 rejection helper is returned as
 * -3; its local zero-denominator rejection is returned as -2. */
int fa18_process_tuple_cache_stage(FA18ClipTuple *tuple,
                                   FA18TupleCacheStageState *state,
                                   FA18ClipTuple *output, size_t capacity,
                                   uint16_t *count, uint8_t *emitted_flag);

#endif
