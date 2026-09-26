#include "transform.h"

#include <limits.h>

static int16_t word_from_bits(uint16_t bits) {
    return bits <= INT16_MAX ? (int16_t)bits : (int16_t)((int32_t)bits - 65536);
}

static uint16_t word_bits(int16_t value) {
    return value < 0 ? (uint16_t)((int32_t)value + 65536) : (uint16_t)value;
}

static int16_t add_word(int16_t left, int16_t right) {
    return word_from_bits((uint16_t)(word_bits(left) + word_bits(right)));
}

static int16_t arithmetic_shift_word(int16_t value, uint8_t count) {
    const uint8_t shift = count & 63u; /* 68000 register-shift count. */
    if (!shift) return value;
    if (shift >= 16u) return value < 0 ? -1 : 0;
    const int32_t divisor = 1 << shift;
    return (int16_t)(value < 0 ? (value - (divisor - 1)) / divisor : value / divisor);
}

static int16_t dot_product_component(const int16_t input[3],
                                     const int16_t coefficients[3]) {
    uint32_t sum = 0;
    for (size_t index = 0; index < 3; ++index) {
        const int32_t product = (int32_t)input[index] * coefficients[index];
        sum += (uint32_t)product;
    }
    const int64_t signed_sum = (sum & UINT32_C(0x80000000))
        ? (int64_t)sum - INT64_C(4294967296) : (int64_t)sum;
    const int64_t shifted = signed_sum < 0 ? (signed_sum - 255) / 256 : signed_sum / 256;
    return word_from_bits((uint16_t)shifted);
}

int fa18_transform_vertices(const FA18VertexTransform *transform,
                            const FA18LocalVertex *local_vertices,
                            size_t vertex_count,
                            FA18TransformedVertex *output_vertices) {
    if (!transform || (!local_vertices && vertex_count) || (!output_vertices && vertex_count)) {
        return -1;
    }
    for (size_t index = 0; index < vertex_count; ++index) {
        const FA18LocalVertex local = local_vertices[index];
        const int16_t input[3] = {
            add_word(arithmetic_shift_word(local.x, transform->local_shift), transform->translation.x),
            add_word(arithmetic_shift_word(local.y, transform->local_shift), transform->translation.y),
            add_word(arithmetic_shift_word(local.z, transform->local_shift), transform->translation.z)
        };
        output_vertices[index].x = dot_product_component(input, transform->matrix.value[0]);
        output_vertices[index].y = dot_product_component(input, transform->matrix.value[1]);
        output_vertices[index].depth = dot_product_component(input, transform->matrix.value[2]);
    }
    return 0;
}
