#include "flight.h"

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
