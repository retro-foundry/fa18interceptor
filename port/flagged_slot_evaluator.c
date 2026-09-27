#include "flagged_slot_evaluator.h"

static uint16_t be16(const uint8_t *bytes) {
    return (uint16_t)((uint16_t)bytes[0] << 8 | bytes[1]);
}

static int32_t be32(const uint8_t *bytes) {
    return (int32_t)((uint32_t)bytes[0] << 24 | (uint32_t)bytes[1] << 16 |
                     (uint32_t)bytes[2] << 8 | bytes[3]);
}

static int32_t subtract_long(int32_t left, int32_t right) {
    return (int32_t)((uint32_t)left - (uint32_t)right);
}

static int32_t negate_long(int32_t value) {
    return (int32_t)(0u - (uint32_t)value);
}

static int32_t absolute_difference(int32_t left, int32_t right) {
    int32_t result = subtract_long(left, right);
    if (result < 0) result = negate_long(result);
    return result;
}

static int32_t arithmetic_shift_right_long(int32_t value, unsigned count) {
    if (value >= 0) return value >> count;
    return (int32_t)-((-(int64_t)value + (((int64_t)1 << count) - 1)) >> count);
}

static int32_t scaled_word_offset(int16_t value) {
    return (int32_t)((uint32_t)(int32_t)value << 22);
}

static int derive_scalar(const FA18SceneMagnitudeTable *table, int32_t x,
                         int32_t y, int32_t z, int16_t *result) {
    return fa18_scene_component_magnitude(
        table, (int16_t)arithmetic_shift_right_long(x, 8),
        (int16_t)arithmetic_shift_right_long(y, 8),
        (int16_t)arithmetic_shift_right_long(z, 8), result);
}

int fa18_evaluate_flagged_flight_slot(void *context, uint16_t slot,
                                      int32_t *result) {
    FA18FlaggedSlotEvaluator *evaluator = context;
    const FA18FlaggedLinkedRecord *linked;
    const uint8_t *record;
    int16_t first, second;
    int16_t linked_offset;
    int32_t x, y, z;
    if (!evaluator || !result || !evaluator->slots || !evaluator->magnitude_table ||
        !evaluator->resolve_linked_record || slot >= FA18_FLAGGED_SLOT_SCAN_SLOTS)
        return -1;
    record = evaluator->slots[slot].bytes;
    if (!(record[0x26] & 0x20u) && !evaluator->context_selection) {
        *result = 0;
        return 0;
    }
    linked_offset = (int16_t)((uint16_t)((uint16_t)be16(record + 0x2e) << 8) << 1);
    if (evaluator->resolve_linked_record(evaluator->resolve_context, linked_offset,
                                         &linked) != 0 || !linked)
        return -1;
    x = absolute_difference(be32(linked->bytes + 0x14), evaluator->origin_x);
    y = absolute_difference(be32(linked->bytes + 0x18), evaluator->origin_y);
    z = absolute_difference(be32(linked->bytes + 0x1c), evaluator->origin_z);
    if (derive_scalar(evaluator->magnitude_table, x, y, z, &first) != 0) return -1;
    x = subtract_long(be32(record), scaled_word_offset((int16_t)be16(record + 0x30)));
    z = subtract_long(be32(record + 8), scaled_word_offset((int16_t)be16(record + 0x32)));
    x = absolute_difference(x, evaluator->origin_x);
    y = absolute_difference(be32(record + 4), evaluator->origin_y);
    z = absolute_difference(z, evaluator->origin_z);
    if (derive_scalar(evaluator->magnitude_table, x, y, z, &second) != 0) return -1;
    *result = first <= second;
    return 0;
}
