#include "projection_grid.h"

#include <stddef.h>

int fa18_load_projection_grid(const FA18Hunks *hunks, FA18ProjectionGrid *grid) {
    if (!hunks || !grid || FA18_C279_PROJECTION_GRID_HUNK >= hunks->count) return -1;
    const FA18HunkSegment *segment = &hunks->segments[FA18_C279_PROJECTION_GRID_HUNK];
    if (!segment->data || segment->size < FA18_C279_PROJECTION_GRID_OFFSET + 4u) return -1;
    const uint8_t *data = segment->data + FA18_C279_PROJECTION_GRID_OFFSET;
    const uint16_t record_count = fa18_be16(data);
    const size_t records_size = (size_t)record_count * FA18_C279_PROJECTION_GRID_RECORD_BYTES;
    if (records_size > segment->size - FA18_C279_PROJECTION_GRID_OFFSET - 4u) return -1;
    grid->record_count = record_count;
    grid->bounds_limit = fa18_be16(data + 2u);
    grid->records = data + 4u;
    return 0;
}

int fa18_projection_grid_record(const FA18ProjectionGrid *grid, uint16_t index,
                                FA18ProjectionGridRecord *record) {
    if (!grid || !record || !grid->records || index >= grid->record_count) return -1;
    const uint8_t *data = grid->records + (size_t)index * FA18_C279_PROJECTION_GRID_RECORD_BYTES;
    record->x = (int16_t)fa18_be16(data);
    record->y = (int16_t)fa18_be16(data + 2u);
    record->kind = (int16_t)fa18_be16(data + 4u);
    return 0;
}
