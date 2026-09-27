#include "scene_projection_seed.h"

static int32_t add_wrap(int32_t left, int32_t right) {
    return (int32_t)((uint32_t)left + (uint32_t)right);
}

static int32_t asr_long(int32_t value, unsigned count) {
    if (value >= 0) return value >> count;
    return -((-(int64_t)value + (((int64_t)1 << count) - 1)) >> count);
}

static int32_t transform_row(const int16_t row[3], const int16_t seed[3]) {
    int32_t sum = (int32_t)row[0] * seed[0];
    sum = add_wrap(sum, (int32_t)row[1] * seed[1]);
    sum = add_wrap(sum, (int32_t)row[2] * seed[2]);
    return asr_long(sum, 6);
}

int fa18_publish_scene_projection_seed(const FA18SceneProjectionSeedRecord *record,
                                       FA18ProjectionPacket *packet) {
    if (!record || !packet) return -1;
    int16_t seed[3] = {0, 4, 0x12};
    if (record->type_byte_62 == 0x30u) {
        seed[1] = 1;
        seed[2] = -5;
    } else if (record->type_byte_62 == 0x11u) {
        seed[1] = 5;
        seed[2] = 0x14;
    }
    const FA18ProjectionInput input = {
        transform_row(record->matrix[0], seed),
        transform_row(record->matrix[1], seed),
        transform_row(record->matrix[2], seed)
    };
    return fa18_publish_projection_packet(&record->root, input, packet);
}
