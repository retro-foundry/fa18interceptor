#include "record_scan_scalar.h"

static int32_t asr_long(int32_t value, unsigned count) {
    count &= 63u;
    if (!count || value >= 0) return value >> count;
    return (int32_t)-((-(int64_t)value + (((int64_t)1 << count) - 1)) >> count);
}

static int32_t asl_long(int32_t value, unsigned count) {
    return (int32_t)((uint32_t)value << (count & 31u));
}

static int16_t negate_word(int16_t value) {
    return (int16_t)(0u - (uint16_t)value);
}

int fa18_calculate_record_scan_scalar(const FA18SceneMagnitudeTable *table,
                                      int16_t selector, int16_t x, int16_t y,
                                      int16_t z, int16_t result[3]) {
    int16_t magnitude;
    int32_t d0;
    int16_t d2 = 3;
    uint32_t dividend;
    if (!table || !result) return -1;
    if (!selector) return 1;
    d0 = selector;
    if (d0 < 0) d0 = negate_word((int16_t)d0);
    if (fa18_scene_component_magnitude(table,
                                       x < 0 ? negate_word(x) : x,
                                       y < 0 ? negate_word(y) : y,
                                       z < 0 ? negate_word(z) : z,
                                       &magnitude) != 0 || !magnitude)
        return -1;
    while (d0 <= magnitude) {
        d0 = asl_long(d0, 2);
        d2 = (int16_t)(d2 + 2);
    }
    while (d0 > magnitude) {
        d0 = asr_long(d0, 2);
        d2 = (int16_t)(d2 - 2);
        if (d2 <= 1) break;
    }
    d0 = asl_long(d0, 2);
    d2 = (int16_t)(d2 + 2);
    dividend = (uint32_t)asl_long(d0, 8);
    if (dividend / (uint16_t)magnitude <= UINT16_MAX)
        d0 = (int32_t)((dividend & UINT32_C(0xffff0000)) |
                       (dividend / (uint16_t)magnitude));
    result[0] = (int16_t)asr_long((int32_t)(int16_t)(uint16_t)d0 * x, d2);
    result[1] = (int16_t)asr_long((int32_t)(int16_t)(uint16_t)d0 * y, d2);
    result[2] = (int16_t)asr_long((int32_t)(int16_t)(uint16_t)d0 * z, d2);
    if (selector < 0) {
        result[0] = negate_word(result[0]);
        result[1] = negate_word(result[1]);
        result[2] = negate_word(result[2]);
    }
    return 0;
}
