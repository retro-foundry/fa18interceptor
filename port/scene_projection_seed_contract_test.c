#include "scene_projection_seed.h"

#include <assert.h>

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
    return 0;
}
