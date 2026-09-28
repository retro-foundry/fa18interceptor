#include "matrix_pipeline.h"

static int select_record(const uint8_t *store, size_t size,
                         const FA18MatrixPipelineState *state,
                         const uint8_t **record, int32_t *selector) {
    int16_t offset = state->active_record_offset;
    if (!store || !record || !selector) return -1;
    if (!offset) {
        offset = state->fallback_record_offset;
        if (offset < 0 || (size_t)offset + 0x63u > size) return -1;
        if (!(store[offset + 1] & 0x40u) || (store[offset + 0x62] & 0xf0u) != 0x30u)
            offset = 0;
    }
    if (offset < 0 || (size_t)offset + 0x63u > size) return -1;
    *record = store + offset;
    if (((*record)[0x62] & 0xf0u) == 0x30u) *selector = 0xfc;
    else *selector = state->mode == 6 ? 5 : 1;
    return 0;
}

int fa18_run_matrix_pipeline(const FA18FlightTrigTable *trig_table,
                             const uint8_t *record_store, size_t record_store_size,
                             FA18MatrixPipelineState *state,
                             const FA18MatrixPipelineOps *ops,
                             FA18MatrixPipelineTailState *result) {
    const uint8_t *record;
    int32_t transform_selector, transformed[3], relative[3], argument;
    FA18MatrixPipelineTailInput tail_input;
    if (!trig_table || !state || !ops || !ops->transform_record || !result) return -1;
    if (!state->enable_state) return -2; /* `$C2D9B0` is a distinct owner. */
    if (select_record(record_store, record_store_size, state, &record,
                      &transform_selector) != 0 ||
        ops->transform_record(ops->context, record, transform_selector, transformed) != 0)
        return -1;
    relative[0] = (int32_t)((uint32_t)transformed[0] - (uint32_t)state->origin[0]);
    relative[1] = (int32_t)((uint32_t)transformed[1] - (uint32_t)state->origin[1]);
    relative[2] = (int32_t)((uint32_t)transformed[2] - (uint32_t)state->origin[2]);
    if (!state->skip_coordinate_update) {
        if (state->enable_state < 0) argument = -1;
        else if (state->mode) argument = 0x230;
        else {
            int16_t key = (int16_t)((uint16_t)state->active_record_offset | state->flag_byte);
            if (key != state->selection_cache) {
                state->selection_cache = key;
                argument = -1;
            } else argument = 0x7d0;
        }
        if (!ops->update_coordinates ||
            ops->update_coordinates(ops->context, relative, argument,
                                    state->matrix_input[0], state->matrix_input[1],
                                    state->coordinate_output) != 0)
            return -1;
        state->matrix_input[0] = state->coordinate_output[0];
        state->matrix_input[1] = state->coordinate_output[1];
    }
    tail_input = (FA18MatrixPipelineTailInput){
        state->matrix_input[0], state->matrix_input[1],
        {state->row_scale[0], state->row_scale[1], state->row_scale[2]},
        {state->record_auxiliary[0], state->record_auxiliary[1], state->record_auxiliary[2]}
    };
    return fa18_run_matrix_pipeline_tail(trig_table, &tail_input, result);
}
