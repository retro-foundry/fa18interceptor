#include "tuple_cache_stage.h"

static int16_t add_word(int16_t left, int16_t right) {
    return (int16_t)(uint16_t)((uint16_t)left + (uint16_t)right);
}

/* `$C248D8-$C24971`: DIVS quotient/remainder rounding, including the source's
 * exact-half-away-from-zero rule. */
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

static int submit_negated(FA18ClipTuple *tuple, FA18TupleCacheStageState *state,
                          FA18ClipTuple *output, size_t capacity,
                          uint16_t *count, uint8_t *emitted_flag) {
    return fa18_process_negated_tuple(tuple, &state->negated, output, capacity,
                                      count, emitted_flag);
}

int fa18_process_tuple_cache_stage(FA18ClipTuple *tuple,
                                   FA18TupleCacheStageState *state,
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
        const int current_first_greater = current.x > current.z;
        const int cached_first_greater = cached.x > cached.z;
        if (current_first_greater != cached_first_greater) {
            const int16_t denominator = add_word(
                add_word(cached.z, (int16_t)(uint16_t)(0u - (uint16_t)current.z)),
                add_word(current.x, (int16_t)(uint16_t)(0u - (uint16_t)cached.x)));
            if (!denominator) return -2;
            const int16_t numerator = add_word(
                cached.z, (int16_t)(uint16_t)(0u - (uint16_t)cached.x));
            tuple->x = add_word(divide_round_word(
                (int32_t)add_word(current.x, (int16_t)(uint16_t)(0u - (uint16_t)cached.x)) *
                numerator, denominator), cached.x);
            tuple->y = add_word(divide_round_word(
                (int32_t)add_word(current.y, (int16_t)(uint16_t)(0u - (uint16_t)cached.y)) *
                numerator, denominator), cached.y);
            tuple->z = tuple->x;
            uint8_t nested_emitted;
            const int result = submit_negated(tuple, state, output, capacity, count,
                                              &nested_emitted);
            if (result != 0) return result;
            *emitted_flag = nested_emitted;
            state->cached_tuple = current;
            ++state->completion_counter;
            if (current_first_greater) return 0;
        }
    }

    state->cached_tuple = current;
    if (current.x > current.z) return 0;
    *tuple = current;
    uint8_t nested_emitted;
    const int result = submit_negated(tuple, state, output, capacity, count,
                                      &nested_emitted);
    if (result != 0) return result;
    *emitted_flag |= nested_emitted;
    if (!current.x) return -3;
    ++state->completion_counter;
    return 0;
}
