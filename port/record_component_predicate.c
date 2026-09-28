#include "record_component_predicate.h"

static int16_t word_sub(int16_t left, int16_t right) {
    return (int16_t)((uint16_t)left - (uint16_t)right);
}

static int16_t word_add(int16_t left, int16_t right) {
    return (int16_t)((uint16_t)left + (uint16_t)right);
}

static int16_t word_asr(int16_t value, unsigned count) {
    uint16_t bits = (uint16_t)value >> count;
    if (value < 0) bits |= (uint16_t)(UINT16_C(0xffff) << (16u - count));
    return (int16_t)bits;
}

/* The final source BLT branches on N xor V from ADD.L, not just on sign. */
static int add_long_blt(uint32_t left, uint32_t right, uint32_t *sum) {
    const uint32_t result = left + right;
    const int negative = (result & UINT32_C(0x80000000)) != 0;
    const int overflow = ((~(left ^ right) & (left ^ result)) &
                          UINT32_C(0x80000000)) != 0;
    *sum = result;
    return negative != overflow;
}

static int finish(uint32_t input_d7, int branch_taken, uint32_t *result_d7,
                  FA18RecordComponentPredicateRoute *route) {
    if (branch_taken) {
        *result_d7 = input_d7 & UINT32_C(0xffff0000);
        *route = FA18_RECORD_COMPONENT_PREDICATE_REJECTED;
    } else {
        *result_d7 = 1;
        *route = FA18_RECORD_COMPONENT_PREDICATE_ACCEPTED;
    }
    return 0;
}

int fa18_test_record_component_predicate(
    const FA18RecordComponentPredicateState *state, uint32_t input_d7,
    uint16_t stream_word_index, uint32_t *result_d7,
    uint16_t *next_stream_word_index, FA18RecordComponentPredicateRoute *route) {
    int16_t d7 = (int16_t)input_d7;
    uint32_t first, total;

    if (!state || !result_d7 || !next_stream_word_index || !route) return -1;
    *next_stream_word_index = stream_word_index;
    if (((uint16_t)d7 & UINT16_C(0x0c00)) != 0) {
        *route = FA18_RECORD_COMPONENT_PREDICATE_C1FC3A_EXTERNAL;
        return 0;
    }
    if (((uint16_t)d7 & UINT16_C(0x3000)) != 0) {
        int16_t d0, d1, d2;
        const int16_t *record;
        if (!state->control_base || stream_word_index >= state->control_word_count)
            return -1;
        record = state->control_base + state->control_base[stream_word_index++];
        d0 = word_asr(record[0], (uint16_t)state->stream_stage_shift & 15u);
        d1 = word_asr(record[1], (uint16_t)state->stream_stage_shift & 15u);
        d2 = word_asr(record[2], (uint16_t)state->stream_stage_shift & 15u);
        d0 = word_sub(word_add(d0, state->record_component_x), state->local_component[0]);
        d1 = word_sub(d1, state->local_component[1]);
        d2 = word_sub(word_add(d2, state->record_component_z), state->local_component[2]);
        first = (uint32_t)((int32_t)d0 * record[3]);
        if (add_long_blt((uint32_t)((int32_t)d2 * record[5]), first, &total)) {
            *next_stream_word_index = stream_word_index;
            return finish(input_d7, 1, result_d7, route);
        }
        *next_stream_word_index = stream_word_index;
        return finish(input_d7, add_long_blt(total,
                                              (uint32_t)((int32_t)d1 * record[4]),
                                              &total), result_d7, route);
    }

    {
        int16_t u[3], v[3], normal[3], shift;
        uint32_t terms[3];
        for (unsigned i = 0; i < 3; ++i) {
            u[i] = word_sub(state->workspace[3 + i], state->workspace[i]);
            v[i] = word_sub(state->workspace[6 + i], state->workspace[i]);
        }
        shift = (int16_t)((int16_t)state->descriptor_word >> 7) & 7;
        if (shift) for (unsigned i = 0; i < 3; ++i) {
            u[i] = (int16_t)((uint16_t)u[i] << shift);
            v[i] = (int16_t)((uint16_t)v[i] << shift);
        }
        normal[0] = (int16_t)(((int32_t)u[1] * v[2] - (int32_t)v[1] * u[2]) >> 8);
        normal[1] = (int16_t)(((int32_t)u[2] * v[0] - (int32_t)v[2] * u[0]) >> 8);
        normal[2] = (int16_t)(((int32_t)u[0] * v[1] - (int32_t)v[0] * u[1]) >> 8);
        for (unsigned i = 0; i < 3; ++i)
            terms[i] = (uint32_t)((int32_t)normal[i] * state->workspace[i]);
        if (add_long_blt(terms[0], terms[2], &first))
            return finish(input_d7, 1, result_d7, route);
        return finish(input_d7, add_long_blt(first, terms[1], &total), result_d7, route);
    }
}
