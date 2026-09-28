#include "scene_record_dispatch.h"

#include <string.h>

static void put_word(uint8_t *bytes, unsigned offset, uint16_t value) {
    bytes[offset] = (uint8_t)(value >> 8);
    bytes[offset + 1] = (uint8_t)value;
}

static void put_long(uint8_t *bytes, unsigned offset, uint32_t value) {
    put_word(bytes, offset, (uint16_t)(value >> 16));
    put_word(bytes, offset + 2, (uint16_t)value);
}

/* `$C28DD8-$C28DE4`: `SWAP` then `ASL.L #6` makes the signed coordinate a
 * 0x400000-scale term before the already-extended component's `ASL.L #8`. */
static uint32_t dispatch_position_component(int16_t coordinate, int16_t component) {
    return (uint32_t)((int64_t)coordinate * INT64_C(0x400000) +
                      (int64_t)component * 256);
}

int fa18_create_scene_dispatch_record(FA18SceneDispatchRecord *record,
                                      const FA18SceneDispatchCreateInput *input,
                                      const FA18RecordMatrixUpdateOps *matrix_ops) {
    if (!record || !input || !matrix_ops) return -1;

    /* MOVEQ #$28/D3, CLR.L (A1)+, DBRA: 41 longwords, not the whole slot. */
    memset(record->bytes, 0, 41u * sizeof(uint32_t));
    record->bytes[0x7d] = (uint8_t)((record->bytes[0x7d] & 0xf0u) |
                                     (input->class_nibble & 0x0fu));
    record->bytes[0x62] = input->record_type;
    record->bytes[0x5e] = input->record_index;
    record->bytes[0x3a] = (uint8_t)((input->source_word_1 & 0x7f00u) >> 8);
    if (input->source_flags & 0x8000u) record->bytes[1] |= 0x08u;
    if (input->source_flags & 0x4000u) record->bytes[5] = 8;
    if (input->source_flags & 0x2000u) record->bytes[1] |= 0x01u;

    const uint8_t d7 = (uint8_t)(input->inherited_d7 + 1);
    record->bytes[0x5d] = d7;
    record->bytes[0x7a] = 3;
    uint16_t control = (uint16_t)(0x0140u |
                                  ((uint16_t)record->bytes[0] << 8) |
                                  record->bytes[1]);
    control &= 0x7fffu;
    if ((input->record_type & 0xf0u) != 0x20u) control |= 0x1080u;
    put_word(record->bytes, 0, control);

    put_word(record->bytes, 6, (uint16_t)input->coordinate_x);
    put_word(record->bytes, 8, (uint16_t)input->coordinate_z);
    put_word(record->bytes, 0x0c, (uint16_t)input->component_x);
    put_word(record->bytes, 0x0e, (uint16_t)input->component_z);
    put_long(record->bytes, 0x10, (uint32_t)input->altitude);
    put_long(record->bytes, 0x14,
             dispatch_position_component(input->coordinate_x, input->component_x));
    put_long(record->bytes, 0x18, (uint32_t)input->altitude);
    put_long(record->bytes, 0x1c,
             dispatch_position_component(input->coordinate_z, input->component_z));
    record->bytes[0x5f] = 0x44;
    put_word(record->bytes, 0x60, 0x01f4);
    put_long(record->bytes, 0x72, 0x0061a800u);
    record->bytes[0x63] = 0x3d;
    record->bytes[0x71] = 0xff;
    record->bytes[0x64] = 0x10; /* source writes $06 then replaces it with $10 */
    if (input->altitude != 0) {
        record->bytes[0x7c] |= 0xe0u;
        put_word(record->bytes, 0x6c, 0x2000);
        put_word(record->bytes, 0x6e, 0x2000);
    }

    const FA18RecordMatrixUpdateInput matrix_input = {
        0, 0, 0, (int16_t)(input->record_type & 0xf0u)
    };
    return fa18_update_record_matrix(&record->matrix_update, &matrix_input, matrix_ops);
}
