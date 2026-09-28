#ifndef FA18_TERRAIN_PLACEMENT_DIRECT_H
#define FA18_TERRAIN_PLACEMENT_DIRECT_H

#include <stddef.h>
#include <stdint.h>

#include "terrain_placement_pipeline.h"

/* Live values consumed on the direct (header bits 4/6 clear) route through
 * `$C1DD98-$C1E0B0`.  `descriptor_table_base` is the caller-owned native
 * identity for `$C22188`; the source writes that identity plus index * 20. */
typedef struct {
    int16_t correction_word[2];
    int16_t projection_packet[2];
    int32_t projection_depth;
    const uint8_t *shift_table;
    size_t shift_table_size;
    uint32_t descriptor_table_base;
    uint16_t descriptor_table_entries;
    uint8_t append_enabled;
    uint8_t cycle_byte;
} FA18TerrainPlacementDirectInput;

typedef struct {
    FA18ScenePlacementBuilderPrefix prefix;
    FA18TerrainPlacementCellEnd cell_end;
    uint16_t emitted_count;
    uint8_t next_cycle_byte;
} FA18TerrainPlacementDirectResult;

/* `$C1DC44-$C1E11A` direct workspace route. It accepts only the observed
 * descriptor-free header path; bit-4/bit-6 entries require their separate
 * descriptor branches and return -1. */
int fa18_emit_direct_prefixed_placement_records(
    const FA18ScenePlacementBuilderPrefixInput *prefix_input,
    const FA18TerrainPlacementDirectInput *input,
    FA18TerrainPlacementEmit emit, void *context,
    FA18TerrainPlacementDirectResult *result);

#endif
