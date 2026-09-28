#include "scene_dispatch_coordinate_pose.h"

#include "hunk.h"

static uint16_t word_at(const uint8_t *bytes, unsigned offset) {
    return fa18_be16(bytes + offset);
}

static uint32_t long_at(const uint8_t *bytes, unsigned offset) {
    return fa18_be32(bytes + offset);
}

static void put_word(uint8_t *bytes, unsigned offset, uint16_t value) {
    bytes[offset] = (uint8_t)(value >> 8);
    bytes[offset + 1] = (uint8_t)value;
}

static void put_long(uint8_t *bytes, unsigned offset, uint32_t value) {
    put_word(bytes, offset, (uint16_t)(value >> 16));
    put_word(bytes, offset + 2, (uint16_t)value);
}

int fa18_publish_scene_dispatch_coordinate_pose(
    FA18SceneDispatchRecord *target,
    const FA18SceneDispatchRecord *records, size_t record_count,
    int16_t source_selector, FA18SceneCoordinateUpdate coordinate_update,
    void *coordinate_context, const FA18RecordMatrixUpdateOps *matrix_ops) {
    const FA18SceneDispatchRecord *selected;
    FA18SceneCoordinateUpdateInput coordinate_input;
    FA18RecordMatrixUpdateInput matrix_input;
    int16_t coordinate_output[2];
    unsigned selected_index;

    if (!target || !records || !coordinate_update || !matrix_ops || source_selector < 0)
        return -1;
    /* `$C28844-$C28856`: only a live source slot is a valid link target. */
    selected_index = (unsigned)((uint16_t)source_selector >> 8);
    if (selected_index >= record_count) return -1;
    selected = &records[selected_index];
    if (!(word_at(selected->bytes, 0) & 0x0040u)) return -1;

    /* `$C28858-$C2887C`: copy the five source placement fields. */
    put_word(target->bytes, 0x2c, word_at(selected->bytes, 6));
    put_word(target->bytes, 0x2e, word_at(selected->bytes, 8));
    put_word(target->bytes, 0x30, word_at(selected->bytes, 0x0c));
    put_word(target->bytes, 0x32, word_at(selected->bytes, 0x0e));
    put_long(target->bytes, 0x34, long_at(selected->bytes, 0x10));
    target->bytes[0x38] = (uint8_t)(selected_index | 0x80u);

    /* `$C28880-$C288A4`: source-width subtraction and the six pushed longs
     * for `$C123FA`; its first output is intentionally not consumed here. */
    coordinate_input = (FA18SceneCoordinateUpdateInput){
        0, 0,
        (int32_t)(long_at(selected->bytes, 0x14) - long_at(target->bytes, 0x14)),
        0,
        (int32_t)(long_at(selected->bytes, 0x1c) - long_at(target->bytes, 0x1c)),
        -1
    };
    if (coordinate_update(coordinate_context, &coordinate_input, coordinate_output) != 0)
        return -1;

    /* `$C288AA-$C288C0`: `C45AC2` becomes D5; `$C2D954` owns the matrix. */
    matrix_input = (FA18RecordMatrixUpdateInput){0, coordinate_output[1], 0, 0};
    if (fa18_update_record_matrix(&target->matrix_update, &matrix_input, matrix_ops) != 0)
        return -1;
    put_word(target->bytes, 0x66, 0);
    put_word(target->bytes, 0x68, (uint16_t)coordinate_output[1]);
    put_word(target->bytes, 0x6a, 0);
    return 0;
}
