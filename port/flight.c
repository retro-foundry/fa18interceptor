#include "flight.h"

#include <limits.h>
#include <stddef.h>

static int8_t update_lane(int8_t value, uint8_t field, uint8_t increment_code,
                          int8_t minimum, int8_t maximum, int8_t step) {
    if (field == 0) return value;
    int value16 = value;
    if (field == increment_code) {
        value16 = value16 < 0 ? step : value16 + step;
    } else {
        value16 = value16 > 0 ? -step : value16 - step;
    }
    if (value16 < minimum) value16 = minimum;
    if (value16 > maximum) value16 = maximum;
    return (int8_t)value16;
}

int fa18_flight_update_control_lanes(FA18FlightControlLanes *lanes,
                                     uint8_t packed_control) {
    if (!lanes) return -1;
    lanes->lane[0] = update_lane(lanes->lane[0], packed_control & 0x30u,
                                 0x10u, -20, 20, 1);
    lanes->lane[1] = update_lane(lanes->lane[1], packed_control & 0xc0u,
                                 0x40u, -20, 20, 1);
    lanes->lane[2] = update_lane(lanes->lane[2], packed_control & 0x0cu,
                                 0x04u, -60, 60, 3);
    return 0;
}

int fa18_flight_scale_motion_words(int16_t first, int16_t second,
                                   int16_t third, FA18FlightMotionTerms *terms) {
    if (!terms) return -1;
    terms->first = -(int32_t)first * 4;
    terms->second = -(int32_t)second * 4;
    terms->third = -(int32_t)third * 4;
    return 0;
}

static int32_t arithmetic_shift_right(int32_t value, unsigned count) {
    if (value >= 0) return value >> count;
    const int32_t magnitude = -value;
    return -((magnitude + ((1 << count) - 1)) >> count);
}

int fa18_flight_adjust_signed_word_pair(int16_t first, int16_t second,
                                        int16_t *adjusted_first) {
    if (!adjusted_first) return -1;
    int32_t sum = (int32_t)first + (int32_t)second;
    const int32_t magnitude = sum < 0 ? -sum : sum;
    if (magnitude > 4) sum = arithmetic_shift_right(sum, 2);
    else if (magnitude > 2) sum = arithmetic_shift_right(sum, 1);
    sum -= second;
    if (sum < INT16_MIN || sum > INT16_MAX) return -1;
    *adjusted_first = (int16_t)sum;
    return 0;
}

int fa18_flight_prepare_scaled_motion(int16_t first_word,
                                      int16_t adjustment_word,
                                      int16_t second_word,
                                      int16_t third_word,
                                      FA18FlightMotionTerms *terms) {
    if (!terms) return -1;
    int16_t adjusted = 0;
    if (fa18_flight_adjust_signed_word_pair(first_word, adjustment_word,
                                             &adjusted) != 0) return -1;
    /* The original locals are ordered -$10, -$14, -$18 after the helper. */
    return fa18_flight_scale_motion_words(second_word, adjusted, third_word,
                                          terms);
}

static int32_t trig_product(int16_t left, int16_t right) {
    const int32_t product = (int32_t)left * (int32_t)right;
    return arithmetic_shift_right(product, 14);
}

static int16_t matrix_word(int32_t value) {
    return (int16_t)(uint16_t)value;
}

int fa18_flight_compose_attitude_matrix(const FA18FlightTrigState *trig,
                                        int16_t output[3][3]) {
    if (!trig || !output) return -1;
    const int16_t d0 = trig->d0, d1 = trig->d1, d2 = trig->d2;
    const int16_t d3 = trig->d3, d4 = trig->d4, d5 = trig->d5;
    output[0][0] = matrix_word(arithmetic_shift_right(
        trig_product(d4, d0) * d2 - (int32_t)d5 * d3, 14));
    output[0][1] = matrix_word(-arithmetic_shift_right(
        trig_product(d5, d0) * d2 + (int32_t)d4 * d3, 14));
    output[0][2] = matrix_word(trig_product(d1, d2));
    output[1][0] = matrix_word(trig_product(d4, d1));
    output[1][1] = matrix_word(trig_product(d5, d1));
    output[1][2] = matrix_word(d0);
    output[2][0] = matrix_word(-arithmetic_shift_right(
        trig_product(d4, d0) * d3 + (int32_t)d5 * d2, 14));
    output[2][1] = matrix_word(arithmetic_shift_right(
        trig_product(d5, d0) * d3 - (int32_t)d4 * d2, 14));
    output[2][2] = matrix_word(trig_product(d1, d3));
    return 0;
}

static int16_t trig_table_word(const FA18FlightTrigTable *table, int32_t offset) {
    const uint16_t bits = ((uint16_t)table->bytes[offset] << 8) |
                          table->bytes[offset + 1];
    return bits <= INT16_MAX ? (int16_t)bits : (int16_t)((int32_t)bits - 65536);
}

static int lookup_pair(const FA18FlightTrigTable *table, int16_t angle,
                       int16_t *sine, int16_t *cosine) {
    if (!table || !table->bytes || table->byte_count < 0x70au ||
        !sine || !cosine) return -1;
    const int32_t quadrant = 0x708;
    const int32_t half = 0xE10;
    const int32_t three_quarters = 0x1518;
    const int32_t full = 0x1C20;
    const int32_t doubled = (uint16_t)angle * 2;
    int32_t value_offset;
    int32_t cosine_offset;
    if (doubled < quadrant) {
        value_offset = doubled;
        cosine_offset = quadrant - doubled;
        *sine = trig_table_word(table, value_offset);
        *cosine = trig_table_word(table, cosine_offset);
    } else if (doubled < half) {
        value_offset = half - doubled;
        cosine_offset = doubled - quadrant;
        *sine = trig_table_word(table, value_offset);
        *cosine = (int16_t)-trig_table_word(table, cosine_offset);
    } else if (doubled < three_quarters) {
        value_offset = doubled - half;
        cosine_offset = three_quarters - doubled;
        *sine = (int16_t)-trig_table_word(table, value_offset);
        *cosine = (int16_t)-trig_table_word(table, cosine_offset);
    } else {
        value_offset = full - doubled;
        cosine_offset = doubled - three_quarters;
        *sine = (int16_t)-trig_table_word(table, value_offset);
        *cosine = trig_table_word(table, cosine_offset);
    }
    return 0;
}

int fa18_flight_lookup_sine_cosine(const FA18FlightTrigTable *table,
                                   int16_t angle, int16_t *sine,
                                   int16_t *cosine) {
    return lookup_pair(table, angle, sine, cosine);
}

int fa18_flight_lookup_two_sine_cosine(const FA18FlightTrigTable *table,
                                       int16_t first_angle,
                                       int16_t second_angle,
                                       FA18FlightTrigState *trig) {
    if (!trig) return -1;
    if (lookup_pair(table, first_angle, &trig->d0, &trig->d1) != 0 ||
        lookup_pair(table, second_angle, &trig->d2, &trig->d3) != 0) return -1;
    return 0;
}

int fa18_flight_publish_control_field(uint8_t *packed_control,
                                      uint8_t field_mask,
                                      uint8_t command,
                                      int enabled) {
    if (!packed_control || (field_mask != 0x30u && field_mask != 0xc0u &&
                            field_mask != 0x0cu) ||
        (command & (uint8_t)~field_mask) != 0) return -1;
    if (enabled) {
        *packed_control = (uint8_t)((*packed_control & (uint8_t)~field_mask) |
                                    command);
    }
    return 0;
}

int fa18_flight_decode_joy0dat(uint16_t joy0dat,
                               FA18Joy0DerivedInput *derived) {
    if (!derived) return -1;
    const uint16_t xor_test = (uint16_t)(((joy0dat & 0x0200u) >> 1) ^
                                         (joy0dat & 0x0100u));
    derived->first_derived = (uint8_t)(xor_test != 0);
    derived->second_derived = (uint8_t)(((joy0dat & 0x0002u) != 0) !=
                                        ((joy0dat & 0x0200u) != 0));
    return 0;
}

int fa18_flight_apply_joystick_direction(uint8_t direction,
                                         int pressed,
                                         uint8_t *packed_control) {
    uint8_t field_mask;
    uint8_t command;
    switch (direction) {
    case 4: field_mask = 0x30; command = 0x10; break;
    case 5: field_mask = 0x30; command = 0x20; break;
    case 6: field_mask = 0x0c; command = 0x08; break;
    case 7: field_mask = 0x0c; command = 0x04; break;
    default: return -1;
    }
    return fa18_flight_publish_control_field(packed_control, field_mask,
                                             pressed ? command : 0,
                                             1);
}

int fa18_flight_update_attitude(const FA18FlightTrigTable *table,
                                int16_t first_angle, int16_t second_angle,
                                int16_t third_angle, FA18FlightPose *pose) {
    if (!table || !pose) return -1;
    const int16_t first = (int16_t)((uint16_t)first_angle >> 3);
    const int16_t second = (int16_t)((uint16_t)second_angle >> 3);
    const int16_t third = (int16_t)((uint16_t)third_angle >> 3);
    FA18FlightTrigState trig = {0};
    if (fa18_flight_lookup_two_sine_cosine(table, first, second, &trig) != 0 ||
        fa18_flight_lookup_sine_cosine(table, third, &trig.d4, &trig.d5) != 0) {
        return -1;
    }
    return fa18_flight_compose_attitude_matrix(&trig, pose->attitude);
}

int fa18_flight_commit_vertical(FA18FlightPose *pose, int32_t delta) {
    if (!pose) return -1;
    pose->altitude += delta;
    pose->vertical_delta = delta;
    return 0;
}

int fa18_flight_publish_horizontal(FA18FlightPose *pose,
                                   int32_t lateral, int32_t forward) {
    if (!pose) return -1;
    pose->lateral_delta = lateral - pose->lateral;
    pose->forward_delta = forward - pose->forward;
    pose->lateral = lateral;
    pose->forward = forward;
    return 0;
}

int fa18_flight_apply_motion_terms(FA18FlightPose *pose,
                                   int32_t lateral_delta,
                                   int32_t vertical_delta,
                                   int32_t forward_delta) {
    if (!pose) return -1;
    if (fa18_flight_publish_horizontal(pose,
            pose->lateral + lateral_delta,
            pose->forward + forward_delta) != 0) return -1;
    return fa18_flight_commit_vertical(pose, vertical_delta);
}
