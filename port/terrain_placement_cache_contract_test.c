#include "terrain_placement_cache.h"
#include <assert.h>
#include <string.h>

int main(void) {
    uint8_t workspace[0x600] = {0}, cache_bytes[48] = {0}, shifts[240] = {0};
    int8_t type_map[4] = {0}, type_pair[2] = {0}, placement_map[1] = {0};
    int16_t pairs[2] = {0};
    FA18ScenePlacementBuilderPrefixInput prefix = {
        0, type_map, 4, type_pair, 2, {0, 0}, 0, placement_map, 1,
        pairs, 2, pairs, 2, workspace, sizeof workspace
    };
    FA18TerrainPlacementDirectInput direct = {
        {0,0}, {0,0}, 0, shifts, sizeof shifts, 0xc22000, 1,
        0,0,0,0,0,0,0,0,0
    };
    FA18TerrainPlacementCache cache = {cache_bytes, sizeof cache_bytes, 99};
    FA18TerrainPlacementDirectResult result;
    workspace[0] = 0; workspace[1] = 0; workspace[2] = 0; workspace[3] = 0;
    workspace[4] = 0; workspace[5] = 0; workspace[6] = 0xff;
    assert(fa18_build_terrain_placement_cache(&prefix, &direct, &cache, &result) == 0);
    assert(cache.record_count == 1 && cache_bytes[0] == 0 && cache_bytes[24] == 0xff &&
           cache_bytes[25] == 0xff);
    return 0;
}
