#include "scene_active_record.h"

int fa18_resolve_scene_active_record(const FA18SceneActiveRecordState *state,
                                     size_t minimum_size,
                                     const uint8_t **record) {
    ptrdiff_t offset;

    if (!state || !record || !state->bytes) return -1;
    if (state->base_offset > PTRDIFF_MAX) return -1;
    offset = (ptrdiff_t)state->base_offset + state->selected_offset;
    if (offset < 0 || (size_t)offset > state->size ||
        minimum_size > state->size - (size_t)offset)
        return -1;
    *record = state->bytes + offset;
    return 0;
}
