#include "scene_projection_seed.h"

#include <assert.h>
#include <string.h>

static void put_be16(uint8_t *bytes, uint16_t value) {
    bytes[0] = (uint8_t)(value >> 8);
    bytes[1] = (uint8_t)value;
}

static void put_be32(uint8_t *bytes, uint32_t value) {
    bytes[0] = (uint8_t)(value >> 24);
    bytes[1] = (uint8_t)(value >> 16);
    bytes[2] = (uint8_t)(value >> 8);
    bytes[3] = (uint8_t)value;
}

int main(void) {
    FA18SceneProjectionSeedRecord record = {
        .type_byte_62 = 0x11,
        .matrix = {{64, 0, 0}, {0, 64, 0}, {0, 0, 64}},
        .root = {0x00100000, 0x00007708, 0x00200000}
    };
    FA18ProjectionPacket packet;
    assert(fa18_publish_scene_projection_seed(&record, &packet) == 0);
    /* Type $11 selects (0,5,$14), which the identity-at-shift-six matrix
     * passes unchanged before `$C1C5E0` masks/adds/negates/shifts it. */
    assert(packet.x == -4096 && packet.y == -120 && packet.z == -8193 &&
           packet.depth_metric == -30477);

    record.type_byte_62 = 0x30;
    assert(fa18_publish_scene_projection_seed(&record, &packet) == 0);
    assert(packet.x == -4096 && packet.y == -120 && packet.z == -8192);
    record.type_byte_62 = 0;
    assert(fa18_publish_scene_projection_seed(&record, &packet) == 0);
    assert(packet.y == -120 && packet.z == -8193);
    assert(fa18_publish_scene_projection_seed(0, &packet) == -1);

    uint8_t bytes[0xa4];
    memset(bytes, 0, sizeof bytes);
    put_be32(bytes + 0x14, 0x11982c00u);
    put_be32(bytes + 0x18, 0x00007708u);
    put_be32(bytes + 0x1c, 0x1059a000u);
    bytes[0x62] = 0x11;
    put_be16(bytes + 0x92, 64);
    put_be16(bytes + 0x9a, 64);
    put_be16(bytes + 0xa2, 64);
    assert(fa18_decode_scene_projection_seed_record(bytes, sizeof bytes, &record) == 0);
    assert(record.root.x == 0x11982c00 && record.root.y == 0x7708 &&
           record.root.z == 0x1059a000 && record.type_byte_62 == 0x11 &&
           record.matrix[0][0] == 64 && record.matrix[1][1] == 64 &&
           record.matrix[2][2] == 64);
    assert(fa18_decode_scene_projection_seed_record(bytes, sizeof bytes - 1, &record) == -1);
    assert(fa18_decode_scene_projection_seed_record(0, sizeof bytes, &record) == -1);
    return 0;
}
