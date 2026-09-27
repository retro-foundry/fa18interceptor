#include "scene_negative_pose.h"

static int16_t asr_word(int16_t value, unsigned count) {
    if (value >= 0) return (int16_t)(value >> count);
    const int32_t magnitude = -(int32_t)value;
    return (int16_t)-((magnitude + ((1 << count) - 1)) >> count);
}

static int16_t screen_part(int32_t value) {
    return (int16_t)(((uint32_t)value & 0x003fffffu) >> 8);
}

static int16_t high_shifted_part(int32_t value) {
    return asr_word((int16_t)((uint32_t)value >> 16), 4);
}

int fa18_initialize_negative_scene_pose(int16_t table_word,
                                        uint8_t *scene_index,
                                        const FA18SceneNegativePoseRecord *selected,
                                        const FA18SceneNegativePoseDescriptor *descriptor,
                                        int16_t inherited_d7,
                                        FA18SceneNegativePoseState *state,
                                        const FA18SceneNegativePoseOps *ops) {
    if (table_word >= 0 || !scene_index || !selected || !descriptor || !state ||
        !ops || !ops->matrix_update) return -1;
    if (!(selected->flags_byte_01 & 0x40u)) {
        *scene_index = 0;
        return 1;
    }
    if (descriptor->value_02 >= 0 &&
        (!ops->nonnegative_descriptor || ops->nonnegative_descriptor(ops->context) != 0))
        return -1;

    const uint32_t descriptor_bits = (uint32_t)descriptor->value_02 & UINT32_C(0x7fffffff);
    const FA18SceneVectorInput input = {{11, 0, 0x68}};
    FA18SceneVectorOutput output;
    if (fa18_transform_scene_vector(&selected->matrix, &selected->base, &input, &output) != 0)
        return -1;

    state->word_10 = descriptor_bits + 7u;
    state->position[1] = (int32_t)((descriptor_bits << 8) + 0x708u);
    state->flags_byte_04 |= 0xc8u;
    state->word_a4 = 0;
    state->position[0] = output.value[0];
    state->position[2] = output.value[2];
    state->word_0c = screen_part(output.value[0]);
    state->word_0e = screen_part(output.value[2]);

    const int16_t x = high_shifted_part(output.value[0]);
    const int16_t z = high_shifted_part(output.value[2]);
    state->word_0a = (uint8_t)(3u - ((uint16_t)x & 3u)) +
                      (uint8_t)((3u - ((uint16_t)z & 3u)) * 4u);
    state->word_06 = (uint16_t)asr_word(x, 2);
    state->word_08 = (uint16_t)asr_word(z, 2);
    state->word_0b = (uint8_t)(3u - ((uint16_t)state->word_06 & 3u)) +
                      (uint8_t)((3u - ((uint16_t)state->word_08 & 3u)) * 4u);

    const FA18RecordMatrixUpdateInput matrix_input = {
        selected->angles[0], selected->angles[1], selected->angles[2], inherited_d7
    };
    return fa18_update_record_matrix(&state->matrix_update, &matrix_input,
                                     ops->matrix_update);
}
