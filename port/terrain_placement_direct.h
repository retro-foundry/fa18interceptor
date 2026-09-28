#ifndef FA18_TERRAIN_PLACEMENT_DIRECT_H
#define FA18_TERRAIN_PLACEMENT_DIRECT_H

#include <stddef.h>
#include <stdint.h>

#include "terrain_placement_pipeline.h"

/* Live values consumed through `$C1DD98-$C1E0B0`.  `descriptor_table_base`
 * is the caller-owned native identity for `$C22188`.  The two record banks
 * bind `$C46184` (16 x 512) and `$C48184` (16 x 32); bit-4 records are mutable
 * because `$C1DE82` clears their bit 2 under the source mode gate. */
typedef struct {
    int16_t correction_word[2];
    int16_t projection_packet[2];
    int32_t projection_depth;
    const uint8_t *shift_table;
    size_t shift_table_size;
    uint32_t descriptor_table_base;
    uint16_t descriptor_table_entries;
    uint8_t *bit4_records;
    size_t bit4_records_size;
    const uint8_t *bit6_records;
    size_t bit6_records_size;
    uint8_t descriptor_record_mode;
    int32_t retained_third_work;
    uint8_t flagged_tail_byte;
    uint8_t append_enabled;
    uint8_t cycle_byte;
} FA18TerrainPlacementDirectInput;

typedef struct {
    FA18ScenePlacementBuilderPrefix prefix;
    FA18TerrainPlacementCellEnd cell_end;
    uint16_t emitted_count;
    uint8_t next_cycle_byte;
    uint16_t next_record_tail_word;
} FA18TerrainPlacementDirectResult;

/* `$C1DC44-$C1E11A` workspace route, including the bit-4 and bit-6 descriptor
 * branches through `$C1DE38`.  Its caller binds all live scratch and record
 * banks; this routine does not assign scene ownership or scheduling. */
int fa18_emit_direct_prefixed_placement_records(
    const FA18ScenePlacementBuilderPrefixInput *prefix_input,
    const FA18TerrainPlacementDirectInput *input,
    FA18TerrainPlacementEmit emit, void *context,
    FA18TerrainPlacementDirectResult *result);

#endif
