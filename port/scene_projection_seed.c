#include "scene_projection_seed.h"

enum {
    SCENE_ROOT_X_OFFSET = 0x14,
    SCENE_ROOT_Y_OFFSET = 0x18,
    SCENE_ROOT_Z_OFFSET = 0x1c,
    SCENE_TYPE_OFFSET = 0x62,
    SCENE_MATRIX_OFFSET = 0x92,
    SCENE_MATRIX_WORDS = 9
};

static uint16_t read_be16(const uint8_t *bytes) {
    return (uint16_t)((uint16_t)bytes[0] << 8 | bytes[1]);
}

static int32_t read_be32(const uint8_t *bytes) {
    return (int32_t)((uint32_t)bytes[0] << 24 | (uint32_t)bytes[1] << 16 |
                     (uint32_t)bytes[2] << 8 | bytes[3]);
}

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

int fa18_decode_scene_projection_seed_record(const uint8_t *bytes, size_t size,
                                             FA18SceneProjectionSeedRecord *record) {
    if (!bytes || !record || size < FA18_SCENE_PROJECTION_SEED_RECORD_BYTES) return -1;
    record->root.x = read_be32(bytes + SCENE_ROOT_X_OFFSET);
    record->root.y = read_be32(bytes + SCENE_ROOT_Y_OFFSET);
    record->root.z = read_be32(bytes + SCENE_ROOT_Z_OFFSET);
    record->type_byte_62 = bytes[SCENE_TYPE_OFFSET];
    for (unsigned row = 0; row != 3; ++row)
        for (unsigned column = 0; column != 3; ++column)
            record->matrix[row][column] = (int16_t)read_be16(
                bytes + SCENE_MATRIX_OFFSET + (row * 3u + column) * 2u);
    return 0;
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
