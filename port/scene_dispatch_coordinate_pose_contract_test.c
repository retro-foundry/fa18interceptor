#include "scene_dispatch_coordinate_pose.h"

#include <assert.h>
#include <string.h>

static void put_word(uint8_t *bytes, unsigned offset, uint16_t value) {
    bytes[offset] = (uint8_t)(value >> 8);
    bytes[offset + 1] = (uint8_t)value;
}

static void put_long(uint8_t *bytes, unsigned offset, uint32_t value) {
    put_word(bytes, offset, (uint16_t)(value >> 16));
    put_word(bytes, offset + 2, (uint16_t)value);
}

static int coordinate_update(void *context, const FA18SceneCoordinateUpdateInput *input,
                             int16_t output[2]) {
    unsigned *calls = context;
    assert(input->d0 == 0 && input->d1 == 0 && input->d2 == 0x00800000 &&
           input->d3 == 0 && input->d4 == 0x0b000000 && input->d5 == -1);
    ++*calls;
    output[0] = 0;
    output[1] = 0x6fb8;
    return 0;
}

static int build(void *context, const int16_t input[3], int16_t output[3][3]) {
    (void)context;
    memset(output, 0, sizeof(int16_t) * 9u);
    output[0][0] = input[0]; output[1][1] = input[1]; output[2][2] = input[2];
    return 0;
}

static int compose(void *context, const int16_t input[3], int16_t output[3][3]) {
    return build(context, input, output);
}

int main(void) {
    FA18SceneDispatchRecord records[17] = {{0}};
    FA18SceneDispatchRecord target = {{0}};
    unsigned calls = 0;
    const FA18RecordMatrixUpdateOps matrix_ops = {build, compose, 0};

    put_word(records[14].bytes, 0, 0x0040);
    put_word(records[14].bytes, 6, 0x1111);
    put_word(records[14].bytes, 8, 0x2222);
    put_word(records[14].bytes, 0x0c, 0x3333);
    put_word(records[14].bytes, 0x0e, 0x4444);
    put_long(records[14].bytes, 0x10, 0x55556666u);
    put_long(records[14].bytes, 0x14, 0x01000000u);
    put_long(records[14].bytes, 0x1c, 0x0e000000u);
    put_long(target.bytes, 0x14, 0x00800000u);
    put_long(target.bytes, 0x1c, 0x03000000u);

    assert(fa18_publish_scene_dispatch_coordinate_pose(
               &target, records, 17, 0x0e00, coordinate_update, &calls,
               &matrix_ops) == 0);
    assert(calls == 1 && target.bytes[0x38] == 0x8e);
    assert(target.bytes[0x2c] == 0x11 && target.bytes[0x2e] == 0x22 &&
           target.bytes[0x30] == 0x33 && target.bytes[0x32] == 0x44 &&
           target.bytes[0x34] == 0x55 && target.bytes[0x37] == 0x66);
    assert(target.bytes[0x66] == 0 && target.bytes[0x67] == 0 &&
           target.bytes[0x68] == 0x6f && target.bytes[0x69] == 0xb8 &&
           target.bytes[0x6a] == 0 && target.bytes[0x6b] == 0);
    assert(target.matrix_update.published[0] == 0 &&
           target.matrix_update.published[1] == 0x6fb8 &&
           target.matrix_update.published[2] == 0);
    assert(fa18_publish_scene_dispatch_coordinate_pose(
               &target, records, 14, 0x0e00, coordinate_update, &calls,
               &matrix_ops) == -1);
    assert(fa18_publish_scene_dispatch_coordinate_pose(
               &target, records, 17, -1, coordinate_update, &calls,
               &matrix_ops) == -1);
    return 0;
}
