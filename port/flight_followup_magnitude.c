#include "flight_followup_magnitude.h"

static int32_t add_long(int32_t left, int32_t right) {
    return (int32_t)((uint32_t)left + (uint32_t)right);
}
static int32_t negate_long(int32_t value) { return (int32_t)(0u - (uint32_t)value); }
static int32_t asr_long(int32_t value, unsigned count) {
    if (value >= 0) return value >> count;
    return (int32_t)-((-(int64_t)value + (((int64_t)1 << count) - 1)) >> count);
}
static int32_t word_delta_shift(uint16_t value, int16_t selector) {
    const uint16_t delta = (uint16_t)(value - (uint16_t)selector);
    return (int32_t)((uint32_t)delta << 22);
}
static int32_t magnitude_component(int32_t raw, int16_t prepared) {
    int32_t value = add_long(asr_long(raw, 8), prepared);
    if (value < 0) value = negate_long(value);
    return asr_long(asr_long(value, 8), 4);
}

int fa18_calculate_flight_followup_magnitudes(
    const FA18FlightFollowupMagnitudeRecord *record,
    const FA18FlightFollowupMagnitudeInput *input,
    FA18FlightFollowupMagnitudeError error_callback, void *context,
    FA18FlightFollowupMagnitudeResult *result) {
    int32_t raw_x, raw_z, x, z, depth;
    int16_t maximum;
    if (!record || !input || !error_callback || !result) return -1;
    raw_x = add_long(word_delta_shift(record->coordinate_x, input->selector_x),
                     (int32_t)(record->origin_x & UINT32_C(0x003fffff)));
    raw_z = add_long(word_delta_shift(record->coordinate_z, input->selector_z),
                     (int32_t)(record->origin_z & UINT32_C(0x003fffff)));
    x = magnitude_component(raw_x, input->prepared_x);
    z = magnitude_component(raw_z, input->prepared_z);
    depth = add_long(record->depth, input->prepared_depth);
    if (depth < 0) depth = negate_long(depth);
    depth = asr_long(asr_long(depth, 8), 3);
    maximum = (int16_t)x;
    if ((int16_t)z > maximum) maximum = (int16_t)z;
    if ((int16_t)depth > maximum) maximum = (int16_t)depth;
    result->raw_x = raw_x;
    result->raw_z = raw_z;
    result->maximum_half = (uint16_t)maximum >> 1;
    result->capped = 0;
    if (result->maximum_half > 0xefu) {
        if (error_callback(context, 0x29) != 0) return -1;
        result->maximum_half = 0xef;
        result->capped = 1;
    }
    return 0;
}
