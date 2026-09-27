#include "two_angle_matrix.h"

#include <stddef.h>
#include <stdint.h>

enum {
    TRIG_TABLE_SEGMENT = 63,
    /* `$C3E5E8 - $C3DB00`, with `$C3DB00` verified for Hunk 63 in
     * analysis/hunk_runtime_resolved.json. */
    TRIG_TABLE_OFFSET = 0xAE8,
    TRIG_TABLE_BYTES = 0x70A
};

static int16_t word_negate(int16_t value) {
    return (int16_t)(uint16_t)(UINT16_C(0) - (uint16_t)value);
}

static int16_t word_asr(int16_t value, unsigned count) {
    if (value >= 0) return (int16_t)(value >> count);
    const int32_t magnitude = -(int32_t)value;
    return (int16_t)-((magnitude + ((INT32_C(1) << count) - 1)) >> count);
}

/* `muls.w`, `swap`, then `asr.w #4`: only the signed product high word
 * reaches the arithmetic right shift. */
static int16_t product_high_asr4(int16_t left, int16_t right) {
    const uint32_t product = (uint32_t)((int32_t)left * (int32_t)right);
    return word_asr((int16_t)(product >> 16), 4);
}

static int32_t long_asr(int32_t value, unsigned count) {
    if (value >= 0) return value >> count;
    const int64_t magnitude = -(int64_t)value;
    return (int32_t)-((magnitude + ((INT64_C(1) << count) - 1)) >> count);
}

static int32_t long_add_wrap(int32_t left, int32_t right) {
    return (int32_t)((uint32_t)left + (uint32_t)right);
}

static int32_t long_subtract_wrap(int32_t left, int32_t right) {
    return (int32_t)((uint32_t)left - (uint32_t)right);
}

static int32_t mul_word_by_low(int32_t left, int16_t right) {
    return (int32_t)(int16_t)left * right;
}

static int16_t high_word_asr4(int32_t value) {
    return word_asr((int16_t)((uint32_t)value >> 16), 4);
}

int fa18_load_two_angle_trig_table(const FA18Hunks *hunks,
                                   FA18FlightTrigTable *table) {
    if (!hunks || !table || !hunks->segments ||
        hunks->count <= TRIG_TABLE_SEGMENT) return -1;
    const FA18HunkSegment *segment = &hunks->segments[TRIG_TABLE_SEGMENT];
    if (!segment->data || segment->size < TRIG_TABLE_OFFSET + TRIG_TABLE_BYTES)
        return -1;
    table->bytes = segment->data + TRIG_TABLE_OFFSET;
    table->byte_count = segment->size - TRIG_TABLE_OFFSET;
    return 0;
}

int fa18_build_two_angle_matrix(const FA18FlightTrigTable *table,
                                int16_t first_angle, int16_t second_angle,
                                int16_t output[3][3]) {
    if (!table || !output) return -1;
    FA18FlightTrigState trig = {0};
    const int16_t first = (int16_t)((uint16_t)first_angle >> 3);
    const int16_t second = (int16_t)((uint16_t)second_angle >> 3);
    if (fa18_flight_lookup_two_sine_cosine(table, first, second, &trig) != 0)
        return -1;

    output[0][0] = word_asr(trig.d3, 6);
    output[0][1] = 0;
    output[0][2] = word_asr(trig.d2, 6);
    output[1][0] = word_negate(product_high_asr4(trig.d2, trig.d0));
    output[1][1] = word_asr(trig.d1, 6);
    output[1][2] = product_high_asr4(trig.d3, trig.d0);
    output[2][0] = word_negate(product_high_asr4(trig.d2, trig.d1));
    output[2][1] = word_asr(word_negate(trig.d0), 6);
    output[2][2] = product_high_asr4(trig.d3, trig.d1);
    return 0;
}

int fa18_build_single_angle_matrix(const FA18FlightTrigTable *table,
                                   int16_t angle, int16_t output[3][3]) {
    if (!table || !output) return -1;
    int16_t sine = 0;
    int16_t cosine = 0;
    /* `$C2E346` uses ASR.W here, unlike the LSR.W pair in `$C2E38E`. */
    if (fa18_flight_lookup_sine_cosine(table, word_asr(angle, 3),
                                       &sine, &cosine) != 0) return -1;
    output[0][0] = word_asr(cosine, 6);
    output[0][1] = 0;
    output[0][2] = word_asr(sine, 6);
    output[1][0] = 0;
    output[1][1] = 0x100;
    output[1][2] = 0;
    output[2][0] = word_negate(word_asr(sine, 6));
    output[2][1] = 0;
    output[2][2] = word_asr(cosine, 6);
    return 0;
}

int fa18_build_single_angle_trig_matrix(const FA18FlightTrigTable *table,
                                        int16_t angle, int16_t output[3][3]) {
    if (!table || !output) return -1;
    int16_t sine = 0;
    int16_t cosine = 0;
    if (fa18_flight_lookup_sine_cosine(table, word_asr(angle, 3),
                                       &sine, &cosine) != 0) return -1;
    output[0][0] = cosine;
    output[0][1] = 0;
    output[0][2] = sine;
    output[1][0] = 0;
    output[1][1] = 0x4000;
    output[1][2] = 0;
    output[2][0] = word_negate(sine);
    output[2][1] = 0;
    output[2][2] = cosine;
    return 0;
}

int fa18_compose_three_angle_matrix(const FA18FlightTrigTable *table,
                                    int16_t first_angle, int16_t second_angle,
                                    int16_t third_angle, int16_t output[3][3]) {
    if (!table || !output) return -1;
    FA18FlightTrigState trig = {0};
    const int16_t first = (int16_t)((uint16_t)first_angle >> 3);
    const int16_t second = (int16_t)((uint16_t)second_angle >> 3);
    const int16_t third = (int16_t)((uint16_t)third_angle >> 3);
    if (fa18_flight_lookup_two_sine_cosine(table, first, second, &trig) != 0 ||
        fa18_flight_lookup_sine_cosine(table, third, &trig.d4, &trig.d5) != 0)
        return -1;

    int32_t term = long_asr((int32_t)trig.d2 * trig.d0, 14);
    term = mul_word_by_low(term, trig.d4);
    output[0][0] = high_word_asr4(long_add_wrap((int32_t)trig.d3 * trig.d5, term));
    output[0][1] = word_negate(product_high_asr4(trig.d1, trig.d4));
    term = long_asr((int32_t)trig.d3 * trig.d0, 14);
    term = mul_word_by_low(term, trig.d4);
    output[0][2] = high_word_asr4(long_subtract_wrap((int32_t)trig.d2 * trig.d5, term));

    term = long_asr((int32_t)trig.d2 * trig.d0, 14);
    term = mul_word_by_low(term, trig.d5);
    output[1][0] = high_word_asr4(long_subtract_wrap((int32_t)trig.d3 * trig.d4, term));
    output[1][1] = product_high_asr4(trig.d1, trig.d5);
    term = long_asr((int32_t)trig.d3 * trig.d0, 14);
    term = mul_word_by_low(term, trig.d5);
    output[1][2] = high_word_asr4(long_add_wrap((int32_t)trig.d2 * trig.d4, term));

    output[2][0] = word_negate(product_high_asr4(trig.d2, trig.d1));
    output[2][1] = word_negate(word_asr(trig.d0, 6));
    output[2][2] = product_high_asr4(trig.d3, trig.d1);
    return 0;
}
