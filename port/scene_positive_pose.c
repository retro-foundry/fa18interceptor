#include "scene_positive_pose.h"

static int16_t arithmetic_shift_right(int32_t value, unsigned count) {
    if (value >= 0) return (int16_t)(value >> count);
    return (int16_t)-((-(int64_t)value + (((int64_t)1 << count) - 1)) >> count);
}

int fa18_initialize_positive_scene_pose(
    const FA18ScenePositivePoseInput *input,
    FA18ScenePositivePoseState *state,
    const FA18RecordMatrixUpdateOps *matrix_ops) {
    int32_t x, z, delta_x, delta_z;
    int16_t grid_x, grid_z;
    uint16_t matrix_angle;
    FA18RecordMatrixUpdateInput matrix_input;

    if (!input || !state || !matrix_ops || !matrix_ops->build || !matrix_ops->compose ||
        input->entry_words[0] < 0)
        return -1;

    /* `$C093BE-$C093CA`: retain the table fields before their later scales. */
    state->byte_0b = (uint8_t)input->entry_words[2];

    grid_x = (int16_t)((int32_t)input->entry_words[0] * 4 +
                       input->grid_adjustment[0]);
    grid_z = (int16_t)((int32_t)input->entry_words[1] * 4 +
                       input->grid_adjustment[1]);
    state->word_06 = (uint16_t)grid_x;
    state->word_08 = (uint16_t)grid_z;

    x = ((int32_t)input->entry_words[0] << 24) +
        ((int32_t)input->position_adjustment[0] << 10) +
        ((int32_t)input->tail_words[0] << 4) +
        ((int32_t)input->entry_words[3] << 10);
    z = ((int32_t)input->entry_words[1] << 24) +
        ((int32_t)input->position_adjustment[1] << 10) +
        ((int32_t)input->tail_words[1] << 4) +
        ((int32_t)input->entry_words[4] << 10);
    state->position[0] = x;
    state->position[1] = 0x708;
    state->position[2] = z;

    delta_x = -(((int32_t)input->tail_words[0] << 4) +
                ((int32_t)input->entry_words[3] << 10));
    delta_z = -(((int32_t)input->tail_words[1] << 4) +
                ((int32_t)input->entry_words[4] << 10));
    state->published_delta[0] = delta_x;
    state->published_delta[1] = INT32_C(-0x708);
    state->published_delta[2] = delta_z;
    state->word_0c = (uint16_t)arithmetic_shift_right(-delta_x, 8);
    state->word_0e = (uint16_t)arithmetic_shift_right(-delta_z, 8);

    matrix_angle = (uint16_t)((uint16_t)input->tail_words[2] * 80u);
    matrix_input = (FA18RecordMatrixUpdateInput){0, (int16_t)matrix_angle, 0,
                                                   (int16_t)matrix_angle};
    return fa18_update_record_matrix(&state->matrix_update, &matrix_input, matrix_ops);
}
