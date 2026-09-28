#include "scene_root_record.h"

#include <assert.h>

static uint16_t get16(const uint8_t *bytes, unsigned offset) {
    return (uint16_t)((uint16_t)bytes[offset] << 8 | bytes[offset + 1]);
}

static uint32_t get32(const uint8_t *bytes, unsigned offset) {
    return (uint32_t)get16(bytes, offset) << 16 | get16(bytes, offset + 2);
}

int main(void) {
    FA18SceneDispatchRecord record = {0};
    FA18SceneRootPlacementState placement = {0};
    placement.pose.flags_byte_04 = 0xc8;
    placement.pose.word_10 = 0x77;
    placement.pose.position[0] = 0x11183e1c;
    placement.pose.position[1] = 0x7708;
    placement.pose.position[2] = 0x11199dac;
    placement.pose.word_06 = 0x41;
    placement.pose.word_08 = 0x42;
    placement.pose.word_0a = 0x12;
    placement.pose.word_0b = 0x34;
    placement.pose.word_0c = 0x1459;
    placement.pose.word_0e = 0x2404;
    placement.pose.matrix_update.attitude_matrix[2][1] = 0x4000;
    record.bytes[0x62] = 0x11;
    assert(fa18_publish_scene_root_record(&record, &placement,
                                          FA18_SCENE_ROOT_PLACEMENT_NEGATIVE_APPLIED) == 0);
    assert(record.bytes[0x62] == 0x11 && record.bytes[4] == 0xc8 &&
           get16(record.bytes, 0x10) == 0x77 && get32(record.bytes, 0x14) == 0x11183e1c &&
           get32(record.bytes, 0x18) == 0x7708 && get32(record.bytes, 0x1c) == 0x11199dac &&
           get16(record.bytes, 0x06) == 0x41 && get16(record.bytes, 0x08) == 0x42 &&
           record.bytes[0x0a] == 0x12 && record.bytes[0x0b] == 0x34 &&
           get16(record.bytes, 0x0c) == 0x1459 && get16(record.bytes, 0x0e) == 0x2404 &&
           get16(record.bytes, 0x92 + 14) == 0x4000);
    return 0;
}
