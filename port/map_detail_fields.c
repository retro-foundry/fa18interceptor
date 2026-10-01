#include "map_detail_fields.h"

static const uint16_t map_detail_limit_lookup[18] = {
    0x0084, 0x0088, 0x0090, 0x0098, 0x009c, 0x00a0,
    0x00a2, 0x00a4, 0x00a6, 0x00a8, 0x00b0, 0x00c0,
    0x00f0, 0x0100, 0x0108, 0x0110, 0x0120, 0x0120
};

static int32_t add_long(int32_t left, int32_t right) {
    return (int32_t)((uint32_t)left + (uint32_t)right);
}

static int32_t asl_long_4(int32_t value) {
    return (int32_t)((uint32_t)value << 4);
}

static int32_t asr_long_8(int32_t value) {
    if (value >= 0) return value >> 8;
    return (int32_t)-((-(int64_t)value + 255) >> 8);
}

static int32_t negate_long(int32_t value) {
    return (int32_t)(UINT32_C(0) - (uint32_t)value);
}

static int32_t swap_words(int32_t value) {
    const uint32_t bits = (uint32_t)value;
    return (int32_t)((bits << 16) | (bits >> 16));
}

static int32_t rol_long_4(int32_t value) {
    const uint32_t bits = (uint32_t)value;
    return (int32_t)((bits << 4) | (bits >> 28));
}

static int32_t centered_magnitude(int32_t value) {
    value = add_long(value, INT32_C(0x00800000));
    return value < 0 ? negate_long(value) : value;
}

int fa18_apply_map_detail_fields(const FA18MapDetailFieldsInput *input,
                                 FA18MapDetailFieldsResult *result) {
    if (!input || !result) return -1;

    int32_t coordinate_x = input->coordinate_x;
    int32_t coordinate_y = input->coordinate_y;
    int32_t offset_x = input->offset_x;
    int32_t offset_y = input->offset_y;
    if (!input->alternate_layout) {
        offset_x = asl_long_4(offset_x);
        offset_y = asl_long_4(offset_y);
    }
    coordinate_x = add_long(coordinate_x, offset_x);
    coordinate_y = add_long(coordinate_y, offset_y);

    result->visible = 0;
    result->visibility_limit_written = 0;
    result->visibility_limit_register = 0;
    if (input->force_visible) {
        result->visible = 1;
    } else if (input->visibility_gate) {
        int16_t index = (int16_t)asr_long_8(input->detail_metric);
        if (index > 0x11) index = 0x11;
        if (index < 0) return -1;
        int32_t limit = map_detail_limit_lookup[index];
        if (!input->zoom_endpoint) {
            const uint16_t divisor = input->zoom_scale < 2 ? 2u :
                (uint16_t)input->zoom_scale;
            const uint32_t product = UINT32_C(0x8000) / divisor *
                                     (uint32_t)(uint16_t)limit;
            limit = asr_long_8((int32_t)product);
        }
        limit = swap_words(limit);
        result->visibility_limit_written = 1;
        result->visibility_limit_register = (uint32_t)limit;
        if (centered_magnitude(coordinate_x) > limit ||
            centered_magnitude(coordinate_y) > limit)
            result->visible = 1;
    }

    coordinate_x = swap_words(coordinate_x);
    coordinate_y = swap_words(coordinate_y);
    if (input->alternate_layout) {
        coordinate_x = rol_long_4(coordinate_x);
        coordinate_y = rol_long_4(coordinate_y);
    }
    result->coordinate_x = coordinate_x;
    result->coordinate_y = coordinate_y;
    return 0;
}
