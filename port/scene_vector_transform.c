#include "scene_vector_transform.h"

#include <stdint.h>

static int32_t signed_long(uint32_t bits) {
    return bits <= INT32_MAX ? (int32_t)bits : (int32_t)((int64_t)bits - INT64_C(4294967296));
}

static int32_t add_long(int32_t left, int32_t right) {
    return signed_long((uint32_t)left + (uint32_t)right);
}

static int32_t arithmetic_shift_long(int32_t value, unsigned count) {
    if (!count) return value;
    if (value >= 0) return value >> count;
    const int64_t magnitude = -(int64_t)value;
    return (int32_t)-((magnitude + ((INT64_C(1) << count) - 1)) >> count);
}

int fa18_transform_scene_vector(const FA18SceneVectorMatrix *matrix,
                                const FA18SceneVectorBase *base,
                                const FA18SceneVectorInput *input,
                                FA18SceneVectorOutput *output) {
    if (!matrix || !base || !input || !output) return -1;
    for (unsigned row = 0; row < 3; ++row) {
        int32_t sum = 0;
        for (unsigned column = 0; column < 3; ++column) {
            const int32_t product = (int32_t)matrix->value[row][column] * input->value[column];
            sum = add_long(sum, product);
        }
        output->value[row] = add_long(arithmetic_shift_long(sum, 4), base->value[row]);
    }
    return 0;
}
