#include "scene_placement.h"

#include "hunk.h"

int fa18_decode_scene_placement_record(const uint8_t bytes[FA18_SCENE_PLACEMENT_BYTES],
                                       FA18ScenePlacementRecord *record) {
    if (!bytes || !record) return -1;
    record->selector = (int16_t)fa18_be16(bytes);
    record->descriptor_reference = fa18_be32(bytes + 2);
    for (unsigned index = 0; index != 3; ++index)
        record->coordinate[index] = (int16_t)fa18_be16(bytes + 6u + index * 2u);
    record->field_0c = fa18_be16(bytes + 12);
    for (unsigned index = 0; index != 5; ++index)
        record->per_frame[index] = fa18_be16(bytes + 14u + index * 2u);
    return 0;
}

int fa18_traverse_scene_placements(const uint8_t *primary_table,
                                   size_t primary_size,
                                   const uint8_t *alternate_table,
                                   size_t alternate_size,
                                   int use_alternate_table,
                                   uint16_t initial_offset,
                                   int32_t gate_coordinate,
                                   FA18ScenePlacementDescriptorLookup lookup,
                                   FA18ScenePlacementConsumer consumer,
                                   void *context,
                                   FA18ScenePlacementTraversalState *state) {
    const uint8_t *table = use_alternate_table ? alternate_table : primary_table;
    const size_t table_size = use_alternate_table ? alternate_size : primary_size;
    uint16_t offset = initial_offset;

    if (!table || !lookup || !consumer || !state) return -1;
    *state = (FA18ScenePlacementTraversalState){0};
    for (;;) {
        FA18ScenePlacementRecord record;
        FA18ScenePlacementDescriptorProbe probe;
        if ((size_t)offset > table_size ||
            table_size - (size_t)offset < FA18_SCENE_PLACEMENT_BYTES)
            return -1;
        if (fa18_decode_scene_placement_record(table + offset, &record) != 0) return -1;
        if (record.selector == -1) {
            state->next_offset = offset;
            return 0;
        }
        if (lookup(context, record.descriptor_reference, &probe) != 0) return -1;
        if (probe.first_word < 0 ||
            (probe.first_long == UINT32_C(0x00c1ed48) &&
             gate_coordinate < -INT32_C(0x00380000))) {
            ++state->skipped_count;
        } else {
            state->selector_byte = (uint8_t)record.selector;
            state->selector_low_nibble = state->selector_byte & 0x0fu;
            for (unsigned index = 0; index != 3; ++index)
                state->projection_coordinate[index] = (int32_t)record.coordinate[index] << 8;
            if (consumer(context, &record, &probe, state) != 0) return -1;
            ++state->accepted_count;
        }
        offset = (uint16_t)(offset + FA18_SCENE_PLACEMENT_BYTES);
    }
}
