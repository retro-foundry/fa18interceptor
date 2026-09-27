#ifndef FA18_SCENE_PROJECTION_SEED_H
#define FA18_SCENE_PROJECTION_SEED_H

#include <stddef.h>
#include <stdint.h>

#include "projection_packet.h"

/* Selected record fields consumed by `$C1C54E-$C1C5DF`. */
typedef struct {
    uint8_t type_byte_62;
    int16_t matrix[3][3];
    FA18ProjectionRoot root;
} FA18SceneProjectionSeedRecord;

/* Decode only the active-record fields observed at `$C1C54E-$C1C63D`:
 * `+$14/+18/+1C`, `+$62`, and the signed matrix words `+$92..+$A2`.
 * The caller retains ownership and meaning of the containing scene record. */
int fa18_decode_scene_projection_seed_record(const uint8_t *bytes, size_t size,
                                             FA18SceneProjectionSeedRecord *record);

/* `$C1C54E-$C1C63D`: select the literal type seed, transform it through the
 * selected record matrix, and publish the root-relative projection packet. */
int fa18_publish_scene_projection_seed(const FA18SceneProjectionSeedRecord *record,
                                       FA18ProjectionPacket *packet);

#endif
