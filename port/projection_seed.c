#include "projection_seed.h"

#include <stdint.h>

static int32_t add_wrap(int32_t left, int32_t right) {
    return (int32_t)((uint32_t)left + (uint32_t)right);
}

static int32_t fixed14_component(const int16_t seed[3], const int16_t row[3]) {
    uint32_t sum = 0;
    for (int index = 0; index < 3; ++index) {
        sum += (uint32_t)((int32_t)seed[index] * row[index]);
    }
    const int64_t signed_sum = (sum & UINT32_C(0x80000000))
        ? (int64_t)sum - INT64_C(4294967296) : (int64_t)sum;
    return (int32_t)(signed_sum >> 6);
}

int fa18_select_projection_seed(uint8_t record_type,
                                const FA18Fixed14Matrix *matrix,
                                FA18ProjectionBase base,
                                FA18ProjectionSeedResult *result) {
    if (!matrix || !result) return -1;
    int16_t seed[3];
    if (record_type == 0x30u) {
        seed[0] = 0; seed[1] = 1; seed[2] = -5;
    } else if (record_type == 0x11u) {
        seed[0] = 0; seed[1] = 5; seed[2] = 20;
    } else {
        seed[0] = 0; seed[1] = 4; seed[2] = 18;
    }
    result->x = add_wrap(base.x, fixed14_component(seed, matrix->value[0]));
    result->y = add_wrap(base.y, fixed14_component(seed, matrix->value[1]));
    result->z = add_wrap(base.z, fixed14_component(seed, matrix->value[2]));
    return 0;
}
