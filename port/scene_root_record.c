#include "scene_root_record.h"

static void put16(uint8_t *bytes, unsigned offset, uint16_t value) {
    bytes[offset] = (uint8_t)(value >> 8);
    bytes[offset + 1] = (uint8_t)value;
}

static void put32(uint8_t *bytes, unsigned offset, uint32_t value) {
    put16(bytes, offset, (uint16_t)(value >> 16));
    put16(bytes, offset + 2, (uint16_t)value);
}

static void publish_matrix(uint8_t *bytes, const FA18RecordMatrixUpdateState *matrix) {
    for (unsigned row = 0; row != 3; ++row)
        for (unsigned column = 0; column != 3; ++column)
            put16(bytes, 0x92 + (row * 3u + column) * 2u,
                  (uint16_t)matrix->attitude_matrix[row][column]);
}

int fa18_publish_scene_root_record(FA18SceneDispatchRecord *record,
                                   const FA18SceneRootPlacementState *placement,
                                   FA18SceneRootPlacementRoute route) {
    uint8_t *bytes;
    if (!record || !placement) return -1;
    bytes = record->bytes;
    if (route == FA18_SCENE_ROOT_PLACEMENT_NEGATIVE_APPLIED) {
        bytes[4] |= placement->pose.flags_byte_04;
        put16(bytes, 0x10, placement->pose.word_10);
        put32(bytes, 0x14, (uint32_t)placement->pose.position[0]);
        put32(bytes, 0x18, (uint32_t)placement->pose.position[1]);
        put32(bytes, 0x1c, (uint32_t)placement->pose.position[2]);
        put16(bytes, 0x06, placement->pose.word_06);
        put16(bytes, 0x08, placement->pose.word_08);
        bytes[0x0a] = placement->pose.word_0a;
        bytes[0x0b] = placement->pose.word_0b;
        put16(bytes, 0x0c, placement->pose.word_0c);
        put16(bytes, 0x0e, placement->pose.word_0e);
        publish_matrix(bytes, &placement->pose.matrix_update);
        return 0;
    }
    if (route == FA18_SCENE_ROOT_PLACEMENT_POSITIVE_APPLIED) {
        put32(bytes, 0x14, (uint32_t)placement->positive_pose.position[0]);
        put32(bytes, 0x18, (uint32_t)placement->positive_pose.position[1]);
        put32(bytes, 0x1c, (uint32_t)placement->positive_pose.position[2]);
        put16(bytes, 0x06, placement->positive_pose.word_06);
        put16(bytes, 0x08, placement->positive_pose.word_08);
        bytes[0x0b] = placement->positive_pose.byte_0b;
        put16(bytes, 0x0c, placement->positive_pose.word_0c);
        put16(bytes, 0x0e, placement->positive_pose.word_0e);
        publish_matrix(bytes, &placement->positive_pose.matrix_update);
        return 0;
    }
    return -1;
}
