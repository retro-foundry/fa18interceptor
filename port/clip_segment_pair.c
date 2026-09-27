#include "clip_segment_pair.h"

static int16_t add_word(int16_t left, int16_t right) {
    return (int16_t)(uint16_t)((uint16_t)left + (uint16_t)right);
}

static int16_t negate_word(int16_t value) {
    return (int16_t)(uint16_t)(UINT16_C(0) - (uint16_t)value);
}

static int16_t divide_round_word(int32_t dividend, int16_t divisor) {
    const int32_t quotient = dividend / divisor;
    const int32_t remainder = dividend % divisor;
    const int32_t absolute_remainder = remainder < 0 ? -remainder : remainder;
    const int32_t absolute_half_divisor =
        (divisor < 0 ? -(int32_t)divisor : divisor) / 2;
    if (absolute_remainder <= absolute_half_divisor)
        return (int16_t)(uint16_t)quotient;
    return (int16_t)(uint16_t)(quotient + (quotient < 0 ? -1 : 1));
}

static int submit_tuple_cache(FA18ClipTuple *tuple,
                              FA18ClipSegmentPairState *state,
                              FA18ClipTuple *output, size_t capacity,
                              uint16_t *count, uint8_t *emitted_flag) {
    return fa18_process_tuple_cache_stage(tuple, &state->tuple_cache, output,
                                          capacity, count, emitted_flag);
}

int fa18_clip_projected_segment_pair(FA18ClipTuple *tuple,
                                     FA18ClipSegmentPairState *state,
                                     FA18ClipTuple *output, size_t capacity,
                                     uint16_t *count, uint8_t *emitted_flag) {
    if (!tuple || !state || !output || !count || !emitted_flag) return -1;
    const FA18ClipTuple current = *tuple;
    *emitted_flag = 0;
    if (!state->cache_initialized) {
        state->first_tuple = current;
        ++state->cache_initialized;
    } else {
        const FA18ClipTuple cached = state->cached_tuple;
        const int current_after_previous = negate_word(current.y) > current.z;
        const int cached_after_previous = negate_word(cached.y) > cached.z;
        if (current_after_previous != cached_after_previous) {
            const int16_t denominator = add_word(
                add_word(cached.z, negate_word(current.z)),
                add_word(negate_word(current.y), cached.y));
            if (!denominator) return -2;
            const int16_t numerator = add_word(cached.z, cached.y);
            tuple->y = add_word(divide_round_word(
                (int32_t)add_word(current.y, negate_word(cached.y)) * numerator,
                denominator), cached.y);
            tuple->x = add_word(divide_round_word(
                (int32_t)add_word(current.x, negate_word(cached.x)) * numerator,
                denominator), cached.x);
            tuple->z = negate_word(tuple->y);
            uint8_t nested_emitted;
            const int result = submit_tuple_cache(tuple, state, output, capacity,
                                                  count, &nested_emitted);
            /* `$C248B2`'s code-4 helper returns through this call boundary;
             * it is not a status return. Its source-visible effects remain,
             * and this stage continues with its own comparison. */
            if (result != 0 && result != -3) return result;
            *emitted_flag = nested_emitted;
            state->cached_tuple = current;
            ++state->completion_counter;
            if (current_after_previous) return 0;
        }
    }
    state->cached_tuple = current;
    if (negate_word(current.y) > current.z) return 0;
    *tuple = current;
    uint8_t nested_emitted;
    const int result = submit_tuple_cache(tuple, state, output, capacity, count,
                                          &nested_emitted);
    if (result != 0 && result != -3) return result;
    *emitted_flag |= nested_emitted;
    if (!current.x) return -2;
    ++state->completion_counter;
    return 0;
}
