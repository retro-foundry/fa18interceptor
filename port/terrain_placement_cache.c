#include "terrain_placement_cache.h"

#include <string.h>

typedef struct { FA18TerrainPlacementCache *cache; } CacheContext;

static int append_record(void *context, const uint8_t record[FA18_SCENE_PLACEMENT_BYTES]) {
    CacheContext *cache_context = context;
    FA18TerrainPlacementCache *cache = cache_context ? cache_context->cache : NULL;
    size_t offset;
    if (!cache || !record) return -1;
    offset = (size_t)cache->record_count * FA18_SCENE_PLACEMENT_BYTES;
    if (offset > cache->size || cache->size - offset < FA18_SCENE_PLACEMENT_BYTES)
        return -1;
    memcpy(cache->bytes + offset, record, FA18_SCENE_PLACEMENT_BYTES);
    ++cache->record_count;
    return 0;
}

int fa18_build_terrain_placement_cache(
    const FA18ScenePlacementBuilderPrefixInput *prefix_input,
    const FA18TerrainPlacementDirectInput *direct_input,
    FA18TerrainPlacementCache *cache,
    FA18TerrainPlacementDirectResult *result) {
    CacheContext context;
    size_t terminator;
    if (!prefix_input || !direct_input || !cache || !cache->bytes || !result) return -1;
    cache->record_count = 0;
    context.cache = cache;
    if (fa18_emit_direct_prefixed_placement_records(prefix_input, direct_input,
                                                     append_record, &context, result) != 0)
        return -1;
    terminator = (size_t)cache->record_count * FA18_SCENE_PLACEMENT_BYTES;
    if (terminator > cache->size || cache->size - terminator < 2u) return -1;
    cache->bytes[terminator] = 0xffu;
    cache->bytes[terminator + 1u] = 0xffu;
    return 0;
}
