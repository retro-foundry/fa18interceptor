#ifndef FA18_TERRAIN_PLACEMENT_CACHE_H
#define FA18_TERRAIN_PLACEMENT_CACHE_H

#include "terrain_placement_direct.h"

typedef struct {
    uint8_t *bytes;
    size_t size;
    uint16_t record_count;
} FA18TerrainPlacementCache;

/* `$C1DC1C-$C1E11A` cache-owner boundary: emit the direct builder's records
 * into the mutable 24-byte placement list subsequently read at `$C1CB74`.
 * Its caller owns the live selector/workspace inputs; no schedule is implied. */
int fa18_build_terrain_placement_cache(
    const FA18ScenePlacementBuilderPrefixInput *prefix_input,
    const FA18TerrainPlacementDirectInput *direct_input,
    FA18TerrainPlacementCache *cache,
    FA18TerrainPlacementDirectResult *result);

#endif
