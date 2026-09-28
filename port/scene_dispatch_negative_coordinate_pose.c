#include "scene_dispatch_negative_coordinate_pose.h"

#include "hunk.h"

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

static uint32_t swap_words(uint32_t value) {
    return (value << 16) | (value >> 16);
}

int fa18_publish_scene_dispatch_negative_coordinate_pose(
    FA18SceneDispatchRecord *target, int16_t source_selector,
    FA18SceneDispatchNegativeGeometryLookup geometry_lookup,
    void *geometry_context, FA18SceneCoordinateUpdate coordinate_update,
    void *coordinate_context, const FA18RecordMatrixUpdateOps *matrix_ops) {
    int16_t geometry[5];
    uint32_t d2, d3, d4, d5;
    FA18SceneCoordinateUpdateInput coordinate_input;
    FA18RecordMatrixUpdateInput matrix_input;
    int16_t coordinate_output[2];
    const uint16_t selector_bits = (uint16_t)source_selector & 0x7f00u;

    if (!target || !geometry_lookup || !coordinate_update || !matrix_ops ||
        source_selector >= 0)
        return -1;
    /* `$C28808-$C2880C`: a zero masked selector returns without touching A0. */
    if (!selector_bits) return 1;
    if (geometry_lookup(geometry_context, selector_bits >> 7, geometry) != 0)
        return -1;

    /* `$C2881C-$C28824` and `$C28F16`: MOVEM.W sign-extends each source word. */
    put_word(target->bytes, 0x2c, (uint16_t)geometry[0]);
    put_word(target->bytes, 0x2e, (uint16_t)geometry[1]);
    put_word(target->bytes, 0x30, (uint16_t)geometry[2]);
    put_word(target->bytes, 0x32, (uint16_t)geometry[3]);
    put_long(target->bytes, 0x34, (uint32_t)(int32_t)geometry[4]);
    target->bytes[0x38] = 0xff;

    /* `$C2882A-$C2883A`: retain the 68000 word/long width transitions. */
    d2 = swap_words((uint32_t)(int32_t)geometry[0]) << 6;
    d3 = swap_words((uint32_t)(int32_t)geometry[1]) << 6;
    d4 = (uint32_t)(int32_t)geometry[2] << 8;
    d5 = (uint32_t)(int32_t)geometry[3] << 8;
    d2 += d4;
    d3 += d5;
    d4 = d3;

    /* `$C28888-$C288A4`: wrapped deltas are the `$C123FA` input packet. */
    coordinate_input = (FA18SceneCoordinateUpdateInput){
        0, 0, (int32_t)(d2 - long_at(target->bytes, 0x14)), 0,
        (int32_t)(d4 - long_at(target->bytes, 0x1c)), -1
    };
    if (coordinate_update(coordinate_context, &coordinate_input, coordinate_output) != 0)
        return -1;

    matrix_input = (FA18RecordMatrixUpdateInput){0, coordinate_output[1], 0, 0};
    if (fa18_update_record_matrix(&target->matrix_update, &matrix_input, matrix_ops) != 0)
        return -1;
    put_word(target->bytes, 0x66, 0);
    put_word(target->bytes, 0x68, (uint16_t)coordinate_output[1]);
    put_word(target->bytes, 0x6a, 0);
    return 0;
}
