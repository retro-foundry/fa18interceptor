#ifndef FA18_SCENE_PROJECTION_SEED_H
#define FA18_SCENE_PROJECTION_SEED_H

#include <stdint.h>

#include "projection_packet.h"

/* Selected record fields consumed by `$C1C54E-$C1C5DF`. */
typedef struct {
    uint8_t type_byte_62;
    int16_t matrix[3][3];
    FA18ProjectionRoot root;
} FA18SceneProjectionSeedRecord;

/* `$C1C54E-$C1C63D`: select the literal type seed, transform it through the
 * selected record matrix, and publish the root-relative projection packet. */
int fa18_publish_scene_projection_seed(const FA18SceneProjectionSeedRecord *record,
                                       FA18ProjectionPacket *packet);

#endif
