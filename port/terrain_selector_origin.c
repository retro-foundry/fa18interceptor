#include "terrain_selector_origin.h"

static int32_t read_be32(const uint8_t *bytes) {
    return (int32_t)((uint32_t)bytes[0] << 24 | (uint32_t)bytes[1] << 16 |
                     (uint32_t)bytes[2] << 8 | bytes[3]);
}

int fa18_publish_terrain_selector_origin_direct(
    FA18TerrainSelectorOriginDirectState *state,
    FA18TerrainSelectorOriginResult *result) {
    if (!state || !result)
        return -1;
    if (state->prepare_matrix)
        state->prepare_matrix(state->prepare_context);
    if (!state->origin_enable || !state->gate_b || state->gate_a) {
        *result = FA18_TERRAIN_SELECTOR_ORIGIN_GATE_EXIT;
        return 0;
    }
    if (!state->gate_mode || state->detail_mode) {
        *result = FA18_TERRAIN_SELECTOR_ORIGIN_UNPORTED_MATRIX_ROUTE;
        return 0;
    }
    if (!state->active_record ||
        state->active_record_size < FA18_TERRAIN_SELECTOR_ORIGIN_DIRECT_RECORD_BYTES)
        return -1;

    state->origin[0] = read_be32(state->active_record + 0x14);
    /* C2908A jumps straight to C291C8. The floor calculation at C291A8
     * belongs to the matrix lane; the direct record route preserves Y. */
    state->origin[2] = read_be32(state->active_record + 0x1c);
    state->negated_companion[0] = (int32_t)(UINT32_C(0) -
        ((uint32_t)state->origin[0] & UINT32_C(0x003fffff)));
    state->negated_companion[1] = (int32_t)(UINT32_C(0) -
        (uint32_t)state->origin[1]);
    state->negated_companion[2] = (int32_t)(UINT32_C(0) -
        ((uint32_t)state->origin[2] & UINT32_C(0x003fffff)));
    *result = FA18_TERRAIN_SELECTOR_ORIGIN_DIRECT_PUBLISHED;
    return 0;
}

int fa18_publish_terrain_selector_origin_direct_callback(void *context,
                                                          int32_t origin[3]) {
    FA18TerrainSelectorOriginDirectState *state = context;
    FA18TerrainSelectorOriginResult result;

    if (!state || !origin ||
        fa18_publish_terrain_selector_origin_direct(state, &result) != 0 ||
        result != FA18_TERRAIN_SELECTOR_ORIGIN_DIRECT_PUBLISHED)
        return -1;
    origin[0] = state->origin[0];
    origin[1] = state->origin[1];
    origin[2] = state->origin[2];
    return 0;
}
