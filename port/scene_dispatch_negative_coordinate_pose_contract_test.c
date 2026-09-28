#include "scene_dispatch_negative_coordinate_pose.h"

#include <assert.h>
#include <string.h>

static void put_long(uint8_t *bytes, unsigned offset, uint32_t value) {
    bytes[offset] = (uint8_t)(value >> 24);
    bytes[offset + 1] = (uint8_t)(value >> 16);
    bytes[offset + 2] = (uint8_t)(value >> 8);
    bytes[offset + 3] = (uint8_t)value;
}

static int lookup(void *context, uint16_t selector_index, int16_t geometry[5]) {
    unsigned *calls = context;
    assert(selector_index == 26);
    ++*calls;
    geometry[0] = 70;
    geometry[1] = 112;
    geometry[2] = 6144;
    geometry[3] = 6144;
    geometry[4] = 0;
    return 0;
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

int main(void) {
    FA18SceneDispatchRecord target = {{0}};
    unsigned geometry_calls = 0, coordinate_calls = 0;
    const FA18RecordMatrixUpdateOps matrix_ops = {build, build, 0};

    put_long(target.bytes, 0x14, 0x11180000u);
    put_long(target.bytes, 0x1c, 0x11180000u);
    assert(fa18_publish_scene_dispatch_negative_coordinate_pose(
               &target, (int16_t)0x8d0e, lookup, &geometry_calls,
               coordinate_update, &coordinate_calls, &matrix_ops) == 0);
    assert(geometry_calls == 1 && coordinate_calls == 1 && target.bytes[0x38] == 0xff);
    assert(target.bytes[0x2c] == 0 && target.bytes[0x2d] == 70 &&
           target.bytes[0x2e] == 0 && target.bytes[0x2f] == 112 &&
           target.bytes[0x30] == 0x18 && target.bytes[0x31] == 0 &&
           target.bytes[0x32] == 0x18 && target.bytes[0x33] == 0 &&
           target.bytes[0x34] == 0 && target.bytes[0x37] == 0);
    assert(target.bytes[0x68] == 0x6f && target.bytes[0x69] == 0xb8 &&
           target.matrix_update.published[1] == 0x6fb8);
    assert(fa18_publish_scene_dispatch_negative_coordinate_pose(
               &target, (int16_t)0x8000, lookup, &geometry_calls,
               coordinate_update, &coordinate_calls, &matrix_ops) == 1);
    assert(geometry_calls == 1 && coordinate_calls == 1);
    return 0;
}
