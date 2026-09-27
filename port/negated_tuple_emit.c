#include "negated_tuple_emit.h"

static int16_t negate_word(int16_t value) {
    return (int16_t)(uint16_t)(UINT16_C(0) - (uint16_t)value);
}

static int append_tuple(FA18ClipTuple tuple, FA18ClipTuple *output,
                        size_t capacity, uint16_t *count) {
    if (*count >= capacity) return -1;
    output[*count] = tuple;
    ++*count;
    return 0;
}

/* The DIVS remainder route rounds to nearest, with exact half remainders
 * away from zero. This is the shared D0/D1 sequence at `$C249E6-$C24A3A`. */
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

int fa18_emit_negated_tuple_direct(FA18ClipTuple tuple, FA18ClipTuple *cache,
                                   uint8_t *cache_seen, uint8_t *emitted_count,
                                   FA18ClipTuple *output,
                                   size_t capacity, uint16_t *count,
                                   uint8_t *emitted_flag) {
    if (!cache || !cache_seen || !emitted_count || !output || !count || !emitted_flag) return -1;
    *cache = tuple;
    if (!*cache_seen) ++*cache_seen;
    *emitted_flag = 0;
    const int16_t negated_x = negate_word(tuple.x);
    if (tuple.z > negated_x) return 0;
    if (*count >= capacity) return -1;
    output[*count] = tuple;
    ++*count;
    ++*emitted_count;
    *emitted_flag = 1;
    return 0;
}

int fa18_process_negated_tuple(FA18ClipTuple *tuple,
                               FA18NegatedTupleState *state,
                               FA18ClipTuple *output, size_t capacity,
                               uint16_t *count, uint8_t *emitted_flag) {
    if (!tuple || !state || !output || !count || !emitted_flag) return -1;
    *emitted_flag = 0;
    const FA18ClipTuple current = *tuple;

    if (!state->cache_initialized) {
        state->first_tuple = current;
        ++state->cache_initialized;
    } else {
        const FA18ClipTuple cached = state->cached_tuple;
        const int current_first_greater = negate_word(current.x) > current.z;
        const int cached_first_greater = negate_word(cached.x) > cached.z;
        if (current_first_greater == cached_first_greater) {
            if (!current_first_greater) {
                state->cached_tuple = current;
                goto emit_current;
            }
        } else {
            const int16_t denominator = (int16_t)(uint16_t)(
                (uint16_t)cached.z - (uint16_t)current.z -
                (uint16_t)current.x + (uint16_t)cached.x);
            if (!denominator) return -2;
            const int16_t numerator = (int16_t)(uint16_t)(
                (uint16_t)cached.z + (uint16_t)cached.x);
            tuple->x = (int16_t)(uint16_t)(
                (uint16_t)divide_round_word((int32_t)(int16_t)(uint16_t)(
                    (uint16_t)current.x - (uint16_t)cached.x) * numerator,
                    denominator) + (uint16_t)cached.x);
            tuple->y = (int16_t)(uint16_t)(
                (uint16_t)divide_round_word((int32_t)(int16_t)(uint16_t)(
                    (uint16_t)current.y - (uint16_t)cached.y) * numerator,
                    denominator) + (uint16_t)cached.y);
            tuple->z = negate_word(tuple->x);
            if (append_tuple(*tuple, output, capacity, count) != 0) return -1;
            ++state->output_counter;
            *emitted_flag = 1;
            state->cached_tuple = current;
            if (negate_word(current.x) > current.z) return 0;
            if (append_tuple(current, output, capacity, count) != 0) return -1;
            ++state->output_counter;
            return 0;
        }
    }

emit_current:
    state->cached_tuple = current;
    if (negate_word(current.x) > current.z) return 0;
    if (append_tuple(current, output, capacity, count) != 0) return -1;
    ++state->output_counter;
    *emitted_flag = 1;
    return 0;
}
